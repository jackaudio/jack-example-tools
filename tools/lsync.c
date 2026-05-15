/*
 *  lsync - Insert delay to compensate for port latency differences
 *
 *  Copyright (C) 2016 Alba Mendez
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <getopt.h>
#include <math.h>
#include <jack/jack.h>
#include <pthread.h>

/*
 * AUDIO DELAY LINE
 * Implements a resizeable delay line for audio. NOT thread-safe.
 */

typedef struct audio_delay_line {
	/* Where accumulated samples are stored. */
	jack_default_audio_sample_t *data;
	/* Current delay, in samples */
	jack_nframes_t size;
	/* Allocated size of `data`, in samples. */
	jack_nframes_t alloc_size;
	/* Position of next sample to read. */
	jack_nframes_t pos;
} audio_delay_line;

/* Allocate a delay line. Starts with delay = 0. */
audio_delay_line *
audio_delay_line_new()
{
	audio_delay_line *line = malloc(sizeof(audio_delay_line));
	if (!line) return NULL;

	line->size = line->alloc_size = line->pos = 0;
	line->data = NULL;
	return line;
}

/* Deallocate a delay line and its associated resources. */
void
audio_delay_line_free(audio_delay_line *line)
{
	if (!line) return;
	free(line->data);
	free(line);
}

/* Resize a delay line to a new size (delay). */
int
audio_delay_line_resize(audio_delay_line *line, jack_nframes_t size)
{
	/* Allocate more memory if needed. */
	if (size > line->alloc_size) {
		line->alloc_size = size;
		line->data = realloc(line->data, size * sizeof(jack_default_audio_sample_t));
		if (!line->data) return 1;
	}

	/* Calculate the new value for line->pos. */
	int pos = line->pos + size - (int)line->size;
	if (pos < 0) pos = 0;

	/* Starting from current line->pos, drag everything to start. */
	memmove(line->data + line->pos, line->data + pos, (size - pos) * sizeof(jack_default_audio_sample_t));

	/* If we are growing the line, fill the new samples with silence. */
	if (size > line->size)
		memset(line->data + line->pos, 0x00, (size - line->size) * sizeof(jack_default_audio_sample_t));

	line->pos = line->size ? pos : 0;
	line->size = size;
	return 0;
}

/* Process audio with the delay line. */
void
audio_delay_line_process(audio_delay_line *line, const jack_default_audio_sample_t *in, jack_default_audio_sample_t *out, jack_nframes_t period)
{
	jack_nframes_t to_swap = period, chunk1, chunk2;
	if (to_swap > line->size) to_swap = line->size;

	if (line->size) {
		/* Copy next to_swap samples in data to out, replace with last samples at in. */
		chunk1 = line->size - line->pos;
		if (chunk1 > to_swap) chunk1 = to_swap;
		memcpy(out, line->data + line->pos, chunk1 * sizeof(jack_default_audio_sample_t));
		memcpy(line->data + line->pos, in + (period - to_swap), chunk1 * sizeof(jack_default_audio_sample_t));

		chunk2 = to_swap - chunk1;
		memcpy(out + chunk1, line->data, chunk2 * sizeof(jack_default_audio_sample_t));
		memcpy(line->data, in + (period - chunk2), chunk2 * sizeof(jack_default_audio_sample_t));

		line->pos = (line->pos + to_swap) % line->size;
	}

	/* Remaining samples are copied directly from input to output. */
	memcpy(out + to_swap, in, (period - to_swap) * sizeof(jack_default_audio_sample_t));
}

/*
 * CLIENT CODE
 * Registers the client and ports, and manages the delay lines.
 */

/* Represents an input/output port pair, connected through a delay line. */
typedef struct port_pair {
	jack_port_t *input;
	jack_port_t *output;
	audio_delay_line *line;
	float latency;
	jack_nframes_t delay;
} port_pair;

static int equalize_capture = 0;
static int equalize_playback = 0;
static int keep_maximum = 0;
static float latency_coefficient = 0.5;

static jack_client_t *client;
static port_pair *pairs;
static size_t pairs_count;
static pthread_mutex_t pairs_lock = PTHREAD_MUTEX_INITIALIZER;
static float max_latency = 0;

static int recalc_required = 0;
static pthread_mutex_t main_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t main_cond = PTHREAD_COND_INITIALIZER;

/* Stuff to calculate/set delays and latencies. */

float
get_port_latency(jack_port_t *port, jack_latency_callback_mode_t mode)
{
	jack_latency_range_t range;
	jack_port_get_latency_range(port, mode, &range);
	return latency_coefficient * range.max + (1-latency_coefficient) * range.min;
}

void
recalculate_pair_delays()
{
	size_t i;
	int lock_acquired = 0;

	/* First, recalculate pair latencies and max_latency. */
	if (!keep_maximum) max_latency = 0;
	for (i = 0; i < pairs_count; i++) {
		pairs[i].latency = 0;
		if (equalize_capture)
			pairs[i].latency += get_port_latency(pairs[i].input, JackCaptureLatency);
		if (equalize_playback)
			pairs[i].latency += get_port_latency(pairs[i].output, JackPlaybackLatency);

		if (max_latency < pairs[i].latency)
			max_latency = pairs[i].latency;
	}

	/* Then calculate (and set) appropiate delays. */
	for (i = 0; i < pairs_count; i++) {
		/* If the delay for this pair stays the same, skip. */
		jack_nframes_t new_delay = roundf(max_latency - pairs[i].latency);
		if (pairs[i].delay == new_delay) continue;

		/* Otherwise, acquire the lock and resize the delay line. */
		if (!lock_acquired)
			pthread_mutex_lock(&pairs_lock);
		lock_acquired = 1;

		audio_delay_line_resize(pairs[i].line, new_delay);
		pairs[i].delay = new_delay;
	}

	if (lock_acquired) {
		pthread_mutex_unlock(&pairs_lock);
		/* Some delays have changed; request recomputation. */
		pthread_mutex_lock(&main_lock);
		recalc_required = 1;
		pthread_cond_signal(&main_cond);
		pthread_mutex_unlock(&main_lock);
	}
}

void
jack_latency(jack_latency_callback_mode_t mode, void *arg)
{
	size_t i;
	recalculate_pair_delays();

	for (i = 0; i < pairs_count; i++) {
		float delay = pairs[i].delay;
		jack_port_t *get, *set;
		jack_latency_range_t range;

		if (mode == JackCaptureLatency)
			get = pairs[i].input, set = pairs[i].output;
		else
			get = pairs[i].output, set = pairs[i].input;

		jack_port_get_latency_range(get, mode, &range);
		range.min += delay;
		range.max += delay;
		jack_port_set_latency_range(set, mode, &range);
	}
}

/* The process callback does not touch any variables, except the ports
 * of each pair and its delay line. */

int
jack_process(jack_nframes_t nframes, void *arg)
{
	size_t i;
	if (pthread_mutex_trylock(&pairs_lock) != 0) return 0;

	for (i = 0; i < pairs_count; i++) {
		jack_default_audio_sample_t *in = jack_port_get_buffer(pairs[i].input, nframes);
		jack_default_audio_sample_t *out = jack_port_get_buffer(pairs[i].output, nframes);
		audio_delay_line_process(pairs[i].line, in, out, nframes);
	}

	pthread_mutex_unlock(&pairs_lock);
	return 0;
}

/* Option parsing, initialization and shutdown. */

void
signal_handler(int sig)
{
	jack_client_close(client);
	exit(EXIT_SUCCESS);
}

void
jack_shutdown(void *arg)
{
	exit(EXIT_FAILURE);
}

void
show_usage()
{
	fprintf(stderr, "Usage: jack_lsync [options]\n");
	fprintf(stderr, "Delay a set of ports as appropiate to compensate for latency differences.\n\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "      -a, --audio-ports <n>  Number of audio port pairs. Default: 2\n");
	fprintf(stderr, "      -C, --capture          Align the capture latencies.\n");
	fprintf(stderr, "      -P, --playback         Align the playback latencies (default).\n");
	fprintf(stderr, "      -k, --keep             Keep the maximum latency; don't reduce delays.\n");
	fprintf(stderr, "      -l, --coefficient <k>  Set the latency coefficient:\n"
			"                             if 0, latencies will be aligned to their minimum;\n"
			"                             if 1, to their maximum. Default: 0.5, to center\n");
	fprintf(stderr, "      -n, --name <name>      Set the name of the JACK client to <name>.\n");
	fprintf(stderr, "      -s, --server <name>    Connect to the JACK server named <name>.\n");
	fprintf(stderr, "      -h, --help             Display this help message.\n");
	fprintf(stderr, "\nFor more information see http://jackaudio.org/\n");
}

int
main(int argc, char *argv[])
{
	jack_options_t options = 0;
	jack_status_t status;
	const char *name = "lsync";
	const char *server = NULL;
	int c, audio_ports = 2;
	unsigned int i;

	/* Parse command line options. */
	struct option long_options[] = {
		{ "audio-ports", 1, 0, 'a' },
		{ "capture", 0, 0, 'C' },
		{ "playback", 0, 0, 'P' },
		{ "keep", 0, 0, 'k' },
		{ "coefficient", 1, 0, 'l' },
		{ "name", 1, 0, 'n' },
		{ "server", 1, 0, 's' },
		{ "help", 0, 0, 'h' },
		{ 0, 0, 0, 0 }
	};
	while ((c = getopt_long(argc, argv, "a:CPkl:n:s:h", long_options, NULL)) != -1) {
		switch (c) {
		case 'a':
			audio_ports = atoi(optarg);
			if (audio_ports < 0) {
				show_usage();
				return EXIT_FAILURE;
			}
			break;
		case 'C':
			equalize_capture = 1;
			break;
		case 'P':
			equalize_playback = 1;
			break;
		case 'k':
			keep_maximum = 1;
			break;
		case 'l':
			latency_coefficient = atof(optarg);
			break;
		case 'h':
			show_usage();
			return EXIT_SUCCESS;
		case 'n':
			name = optarg;
			options |= JackUseExactName;
			break;
		case 's':
			server = optarg;
			options |= JackServerName;
			break;
		default:
			show_usage();
			return EXIT_FAILURE;
		}
	}

	pairs_count = audio_ports;
	if (!equalize_capture && !equalize_playback)
		equalize_playback = 1;
	if (optind < argc || pairs_count == 0) {
		show_usage();
		return EXIT_FAILURE;
	}

	/* Open JACK client. */
	client = jack_client_open(name, options, &status, server);
	if (!client) {
		fprintf(stderr, "jack_client_open() failed, status = %#2.0x\n", status);
		return EXIT_FAILURE;
	}

	/* Set callbacks. */
	if (jack_set_latency_callback(client, jack_latency, NULL) ||
	    jack_set_process_callback(client, jack_process, NULL)) {
		fprintf(stderr, "Could not set client callbacks\n");
		jack_client_close(client);
		return EXIT_FAILURE;
	}

	/* Setup port pairs. */
	pairs = malloc(pairs_count * sizeof(port_pair));
	assert(pairs);

	for (i = 0; i < pairs_count; i++) {
		char port_name [32];
		const char* port_type = JACK_DEFAULT_AUDIO_TYPE;

		pairs[i].delay = pairs[i].latency = 0;
		pairs[i].line = audio_delay_line_new();
		assert(pairs[i].line);

		snprintf(port_name, 32, "input_%u", i+1);
		pairs[i].input = jack_port_register(client, port_name, port_type, JackPortIsInput, 0);

		snprintf(port_name, 32, "output_%u", i+1);
		pairs[i].output = jack_port_register(client, port_name, port_type, JackPortIsOutput, 0);

		if (!pairs[i].output || !pairs[i].input) {
			fprintf(stderr, "Failed to register ports\n");
			jack_client_close(client);
			return EXIT_FAILURE;
		}
	}

	/* Ensure safe shutdown. */
	jack_on_shutdown(client, jack_shutdown, NULL);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
#ifdef WIN32
	signal(SIGABRT, signal_handler);
#else
	signal(SIGQUIT, signal_handler);
	signal(SIGHUP, signal_handler);
#endif

	/* Start! */
	if (jack_activate(client)) {
		fprintf(stderr, "Could not activate client\n");
		jack_client_close(client);
		return EXIT_FAILURE;
	}

	pthread_mutex_lock(&main_lock);
	while (1) {
		pthread_cond_wait(&main_cond, &main_lock);
		if (recalc_required) jack_recompute_total_latencies(client);
		recalc_required = 0;
	}
}
