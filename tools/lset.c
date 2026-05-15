/*
 *  lset - Correct latency reported by port
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
#ifndef WIN32
#include <unistd.h>
#endif

/* Represents an input/output port pair. */
typedef struct port_pair {
	jack_port_t *input;
	jack_port_t *output;
} port_pair;

static int correct_capture = 0;
static int correct_playback = 0;
static int join_range = 0;
static int absolute = 0;
static float latency_coefficient = 0.5;
static float amount = 1;

static jack_client_t *client;
static port_pair *pairs;
static size_t pairs_count;

void
jack_latency(jack_latency_callback_mode_t mode, void *arg)
{
	size_t i;
	for (i = 0; i < pairs_count; i++) {
		jack_port_t *get, *set;
		jack_latency_range_t range;

		if (mode == JackCaptureLatency)
			get = pairs[i].input, set = pairs[i].output;
		else
			get = pairs[i].output, set = pairs[i].input;

		jack_port_get_latency_range(get, mode, &range);

		if ((correct_capture && mode == JackCaptureLatency) ||
		    (correct_playback && mode == JackPlaybackLatency)) {
			/* Correct the latency. */
			float latency = latency_coefficient * range.max + (1-latency_coefficient) * range.min;
			float correction = amount - (absolute ? latency : 0);
			range.min = roundf((join_range ? latency : range.min) + correction);
			range.max = roundf((join_range ? latency : range.max) + correction);
		}

		jack_port_set_latency_range(set, mode, &range);
	}
}

int
jack_process(jack_nframes_t nframes, void *arg)
{
	size_t i;
	for (i = 0; i < pairs_count; i++) {
		jack_default_audio_sample_t *in = jack_port_get_buffer(pairs[i].input, nframes);
		jack_default_audio_sample_t *out = jack_port_get_buffer(pairs[i].output, nframes);
		memcpy(out, in, nframes * sizeof(jack_default_audio_sample_t));
	}
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
	fprintf(stderr, "Usage: jack_lset [options] <amount>[ms|s]\n");
	fprintf(stderr, "Passthrough client that corrects latency reported by another port.\n\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "      -a, --audio-ports <n>  Number of audio port pairs. Default: 1\n");
	fprintf(stderr, "      -C, --capture          Correct capture latencies.\n");
	fprintf(stderr, "      -P, --playback         Correct playback latencies (default).\n");
	fprintf(stderr, "      -j, --join             Join minimum-maximum values into one.\n");
	fprintf(stderr, "      -A, --absolute         Replace reported latency instead of adding.\n");
	fprintf(stderr, "      -l, --coefficient <k>  Set the latency coefficient for -j or -A:\n"
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
	const char *name = "lset";
	const char *server = NULL;
	int c, audio_ports = 1;
	unsigned int i, amount_len;
	char *amount_str;

	/* Parse command line options. */
	struct option long_options[] = {
		{ "audio-ports", 1, 0, 'a' },
		{ "capture", 0, 0, 'C' },
		{ "playback", 0, 0, 'P' },
		{ "join", 0, 0, 'j' },
		{ "absolute", 0, 0, 'A' },
		{ "coefficient", 1, 0, 'l' },
		{ "name", 1, 0, 'n' },
		{ "server", 1, 0, 's' },
		{ "help", 0, 0, 'h' },
		{ 0, 0, 0, 0 }
	};
	while ((c = getopt_long(argc, argv, "a:CPjAl:n:s:h", long_options, NULL)) != -1) {
		switch (c) {
		case 'a':
			audio_ports = atoi(optarg);
			if (audio_ports < 0) {
				show_usage();
				return EXIT_FAILURE;
			}
			break;
		case 'C':
			correct_capture = 1;
			break;
		case 'P':
			correct_playback = 1;
			break;
		case 'j':
			join_range = 1;
			break;
		case 'A':
			absolute = 1;
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
	if (!correct_capture && !correct_playback)
		correct_playback = 1;
	if (optind + 1 != argc || pairs_count == 0) {
		show_usage();
		return EXIT_FAILURE;
	}
	amount_str = argv[optind];
	amount_len = strlen(amount_str);

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

	/* Calculate amount. */
	if (amount_len >= 1 && amount_str[amount_len-1] == 's') {
		amount_str[--amount_len] = 0;
		amount *= jack_get_sample_rate(client);
		if (amount_len >= 1 && amount_str[amount_len-1] == 'm') {
			amount_str[--amount_len] = 0;
			amount /= 1000;
		}
	}
	if (amount_len == 0) {
		show_usage();
		jack_client_close(client);
		return EXIT_FAILURE;
	}
	amount *= atof(amount_str);

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

	while (1) {
#ifdef WIN32
		Sleep(1000);
#else
		sleep(1);
#endif
	}
}
