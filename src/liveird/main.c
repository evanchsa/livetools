/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 

/***************************************************************************
                          main.c  -  Live!Drive IR Remote Daemon
                             -------------------
    begin                : Mon Dec 24 14:56:18 EST 2001
    copyright            : (C) 2001 by Stephen Evanchik
    email                : evanchsa@clarkson.edu
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "liveird.h"
#include "msgsrv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

/* Global IR port information */
static struct liveir_dev ir_port;

static int run_flag = 1;

/* Small flag to let us know if we are fully initialized */
static int full_init = 0, debug = 0;

char device_file[256], config_file[256];

int process_cmd_line(int argc, char **argv);
void print_usage(void);
int install_signals(void);
void custom_signals(int signal);

int main(int argc, char *argv[])
{
    /* Add default decoders */
    add_decoder(CL_RM900_IDENTIFIER, rm900_decoder);
    add_decoder(CL_iR2000_IDENTIFIER, ir2000_decoder);

	/* Just some friendly initialization */
	memset(device_file, 0, 256);
	memset(config_file, 0, 256);

	/* Just to be safe */
	strcpy(device_file, "/dev/midi");
	strcpy(config_file, "/usr/local/share/livetools/remotes.xml");

	/* Command line processing */
	if(process_cmd_line(argc, argv) < 0) {

		fprintf(stderr, "ERROR: Aborting due to errors in process_cmd_line().\n");
		return EXIT_FAILURE;
	}

	if(debug) {

		if(strlen(device_file) > 0) printf("DEBUG: Using %s as the device.\n", device_file);
		if(strlen(config_file) > 0) printf("DEBUG: Using %s for configuration.\n", config_file);
	}

	/* Initialize the signals before we create the message server */
	if(install_signals() < 0) {
        
		fprintf(stderr, "ERROR: Aborting due to errors in install_signals().\n");
		return EXIT_FAILURE;
	}

	if(init_raw_device(device_file, &ir_port) < 0) {
        
		fprintf(stderr, "ERROR: Aborting due to errors in init_device().\n");
		return EXIT_FAILURE;
	}

	if(create_msgbuf(device_file) < 0) {
        
		close_raw_device(&ir_port);

		fprintf(stderr, "ERROR: Aborting due to errors in create_msgbuf().\n");
		return EXIT_FAILURE;
	}

	/* We are ready to poll the device! */
	full_init = 1;

	/* Daemonize! */
	if(!debug) {

		if(fork()) exit(0);
	}

	while(run_flag) {

        if(poll_device(&ir_port, debug) < 0) {

            fprintf(stderr, "WARNING: Error reading key\n");

            ir_port.keys_lost++;
        }
    }

	if(destroy_msgbuf() < 0) {

		close_raw_device(&ir_port);

		fprintf(stderr, "ERROR: Aborting due to erros in destroy_msgbuf().\n");
		return EXIT_FAILURE;
	}

	/* We aren't fully initialized anymore */
	full_init = 0;

	if(close_raw_device(&ir_port) < 0) {
    
        fprintf(stderr, "ERROR: Aborting due to erros in cleanup_device().\n");
        return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}

/* Signal handling for the masses */
void custom_signals(int signal)
{
	switch(signal) {

		case SIGTERM:
			fprintf(stderr, "WARNING: SIGTERM signal received.\n");
		case SIGINT:
			run_flag = 0;

			fprintf(stderr, "WARNING: SIGINT signal received.\n");

			/* Do some cleanup */
			if(full_init) destroy_msgbuf();
			close_raw_device(&ir_port);			
			break;
		default:
			fprintf(stderr, "WARNING: Unhandled signal received (%d).\n", signal);
			break;
	}
}

/* Install the necessary signal handlers so that the message queue can be
 * cleaned up.
 */
int install_signals(void)
{
    sigset_t sigset;
    struct sigaction sig_action;

    sigemptyset(&sigset);
    sig_action.sa_mask = sigset;
    sig_action.sa_flags = 0;

    sig_action.sa_handler = custom_signals;

    if(sigaction(SIGINT, &sig_action, NULL) != 0) {

        fprintf(stderr, "ERROR: Unable to install custom handler for SIGINT.\n");
        return -1;
    }

    if(sigaction(SIGTERM, &sig_action, NULL) != 0) {

        fprintf(stderr, "ERROR: Unable to install custom handler for SIGTERM.\n");
        return -1;
    }

    return 0;
}

/* Process the command line and store the information in globals (icky!) */
int process_cmd_line(int argc, char **argv)
{
	int c;

	while((c = getopt(argc, argv, "dD:C:h")) != -1) {

		switch((char)c) {

			case 'd':
				debug = 1;
				break;
			case 'D':
				if(optarg) strncpy(device_file, optarg, 256);
				else {

					fprintf(stderr, "ERROR: You must specifiy a device.\n");
					return -1;
				}			
				break;
			case 'C':
				if(optarg) strncpy(config_file, optarg, 256);			
				else {

					fprintf(stderr, "ERROR: You must specifiy a configuration file to read from.\n");
					return -1;
				}
				break;
			case 'h':
				print_usage();
				exit(0);
		}
	}

	return 0;
}

void print_usage(void)
{
	printf("\n");

	printf("Creative Live!Drive IR daemon version %s.\n", VERSION);
	printf("Copyright 2003 Stephen Evanchik <evanchsa@clarkson.edu>\n");
	printf("Usage: liveird -d -R <filename> -D <device node>\n\n");
	printf("\t-d\t\tDebug mode, do not fork to the background and display verbose messages\n");
	printf("\t-D\t\tUse this device to read input from the remote (Default: %s)\n", device_file);
	printf("\t-C\t\tUse this remote definition file (Default: %s)\n", config_file);
	printf("\t-h\t\tPrint this help message and exit\n");

	printf("\n\n");
}
