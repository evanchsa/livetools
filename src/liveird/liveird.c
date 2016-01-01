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
                          liveird.c  -  Live!Drive IR device interface routines
                             -------------------
    begin                : Sat Dec 22 17:04:30 EST 2001
    copyright            : (C) 2001 by Stephen Evanchik
    email                : evanchsa@clarkson.edu
 ***************************************************************************/

#include "liveird.h"
#include "msgsrv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* poll_device
 *
 *
 *
 * Description: This is the main loop of the daemon. 
 *				It continuously reads data from the MIDI port 
 */
int poll_device(struct liveir_dev *ir_port, int debug)
{
    struct liveir_pkt *pkt;
    remote_control_key_t *rmt_key;

	switch(ir_port->type) {
		/* RAW Device is probably the most common */
		case SND_RAW:
		    pkt = read_raw_device(ir_port);
			break;
		/* Someday this could actually do something different */
		case SND_OSS:
		    pkt = read_raw_device(ir_port);
			break;
		/* ALSA is the best for performance and robustness */
		case SND_ALSA:
			pkt = read_alsa_device(ir_port);
			break;
	}
	
	if(pkt == NULL) 
		return -1;

    if(debug)    
        print_code_packet(pkt);
    
    if(validate_code_packet(pkt) < 0) {

        fprintf(stderr, "ERROR: Ignoring invalid code packet\n");

        return -1;
    }

    if( (rmt_key = call_decoder(pkt)) == NULL) {

        fprintf(stderr, "ERROR: Undecodable packet found\n");

        return -1;
    }

    if(dispatch_message(rmt_key) < 0) {

        fprintf(stderr, "ERROR: Unable to dispatch message to the message server\n");

        return -1;
    }

	return 0;
}

