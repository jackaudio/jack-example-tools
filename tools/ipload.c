/*
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <getopt.h>
#include <jack/jack.h>
#include <jack/intclient.h>

jack_client_t *client;
jack_intclient_t intclient;
char *client_name;
char *intclient_name;
char *load_name;
char *load_init = "";
char *server_name = NULL;
int autoclose_opt = 0;
int wait_opt = 0;
volatile int idling = 1;

static void
signal_handler (int sig)
{
	/* do nothing if internal client closed itself */
	if (idling == 0)
		return;

	jack_status_t status;

	fprintf (stderr, "signal received, unloading...");
	status = jack_internal_client_unload (client, intclient);
	if (status & JackFailure)
		fprintf (stderr, "(failed), status = 0x%2.0x\n", status);
	else
		fprintf (stderr, "(succeeded)\n");
	if (autoclose_opt)
		jack_deactivate(client);
	jack_client_close (client);
	exit (0);
}

static void
registration_callback (const char *name, int reg, void *arg)
{
	if (reg || strcmp(intclient_name, name))
		return;

	/* this will stop the wait loop and thus close this application. */
	idling = 0;
	return;

	/* unused */
	(void)arg;
}

static void
show_usage ()
{
	fprintf (stderr, "usage: %s [ options ] client-name [ load-name "
		 "[ init-string]]\n\noptions:\n", client_name);
	fprintf (stderr,
		 "\t-h, --help \t\t print help message\n"
		 "\t-a, --autoclose\t automatically close when intclient is unloaded\n"
		 "\t-i, --init string\t initialize string\n"
		 "\t-s, --server name\t select JACK server\n"
		 "\t-w, --wait \t\t wait for signal, then unload\n"
		 "\n"
		);
}

static int
parse_args (int argc, char *argv[])
{
	int c;
	int option_index = 0;
	char *short_options = "hai:s:w";
	struct option long_options[] = {
		{ "help", 0, 0, 'h' },
		{ "autoclose", 0, 0, 'a' },
		{ "init", required_argument, 0, 'i' },
		{ "server", required_argument, 0, 's' },
		{ "wait", 0, 0, 'w' },
		{ 0, 0, 0, 0 }
	};

	client_name = strrchr(argv[0], '/');
	if (client_name == NULL) {
		client_name = argv[0];
	} else {
		client_name++;
	}

	while ((c = getopt_long (argc, argv, short_options, long_options,
				 &option_index)) >= 0) {
		switch (c) {
		case 'a':
			autoclose_opt = 1;
			break;
		case 'i':
			load_init = optarg;
			break;
		case 's':
			server_name = optarg;
			break;
		case 'w':
			wait_opt = 1;
			break;
		case 'h':
		default:
			show_usage ();
			return 1;
		}
	}

	/* autoclose makes no sense without wait */
	if (autoclose_opt && ! wait_opt)
		autoclose_opt = 0;

	if (optind == argc) {		/* no positional args? */
		show_usage ();
		return 1;
	}
	if (optind < argc)
		load_name = intclient_name = argv[optind++];

	if (optind < argc)
		load_name = argv[optind++];

	if (optind < argc)
		load_init = argv[optind++];

	//fprintf (stderr, "client-name = `%s', load-name = `%s', "
	//	 "load-init = `%s', wait = %d\n",
	//	 intclient_name, load_name, load_init, wait_opt);

	return 0;			/* args OK */
}

int
main (int argc, char *argv[])
{
	jack_status_t status;
	char* name;

	/* parse and validate command arguments */
	if (parse_args (argc, argv))
		exit (1);		/* invalid command line */

	/* first, become a JACK client */
	client = jack_client_open (client_name, JackServerName,
				   &status, server_name);
	if (client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}

	/* then, load the internal client */
	intclient = jack_internal_client_load (client, intclient_name,
					       (JackLoadName|JackLoadInit),
					       &status, load_name, load_init);
	if (status & JackFailure) {
		fprintf (stderr, "could not load %s, intclient = %d status = 0x%2.0x\n",
			 load_name, (int)intclient, status);
		return 2;
	}
	if (status & JackNameNotUnique) {
		intclient_name =
			jack_get_internal_client_name (client, intclient);
		fprintf (stderr, "unique internal client name `%s' assigned\n",
			 intclient_name);
	}

	fprintf (stdout, "%s is running.\n", load_name);

	name = jack_get_internal_client_name(client, intclient);
	if (name) {
		printf("client name = %s\n", name);
		free(name);
	}
	fflush(stdout);

	if (autoclose_opt) {
		jack_set_client_registration_callback(client, registration_callback, NULL);
		jack_activate(client);
	}

	if (wait_opt) {
		/* define a signal handler to unload the client, then
		 * wait for it to exit */
	#ifdef WIN32
		signal(SIGINT, signal_handler);
		signal(SIGABRT, signal_handler);
		signal(SIGTERM, signal_handler);
	#else
		signal(SIGQUIT, signal_handler);
		signal(SIGTERM, signal_handler);
		signal(SIGHUP, signal_handler);
		signal(SIGINT, signal_handler);
	#endif

		while (idling) {
			#ifdef WIN32
				Sleep(1000);
			#else
				sleep (1);
			#endif
		}
	}

	if (autoclose_opt) {
		jack_deactivate(client);
	}

	jack_client_close(client);
	return 0;
}

