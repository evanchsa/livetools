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
 
 
#include <stdio.h>
#include <alsa/asoundlib.h>

#include "livetools.h"

/*
 * ALSA Sequencer access functions
 *
 *
 *
 * init_alsa_device
 *
 *
 * Description: This will initialize the liveir_port's ALSA sequencer device.
 *
 */
int init_alsa_device(struct liveir_dev *ir_port, char *device_name)
{
#if 0
    snd_seq_client_info_t client_info;
    
    /* 
     * We only need to open an input port. The question is: are these copies
     * of the events sent from the MIDI device or are they the Real Thing?
     */
    if (snd_seq_open(ir_port->alsa_ir, device_name, SND_SEQ_OPEN_INPUT, 0) < 0) {
        
        fprintf(stderr, "ERROR: Unable to open ALSA sequencer\n");
        
        return -1;
    }
    
    /*
     * Setup the client with all of it's necessary information.
     *
     * This is critical because we only want SYSEX events.
     */
    snd_seq_set_client_name(ir_port->alsa_ir, "livetools");
    
    if(snd_seq_get_client_info(ir_port->alsa_ir, &client_info) < 0) {
        
        fprintf(stderr, "ERROR: Unable to GET client information\n");
        
        return -1;
    }
    
    client_info.filter |= SND_SEQ_FILTER_USE_EVENT;
    
    if(snd_seq_set_client_info(ir_port->alsa_ir, &client_info) < 0) {
        
        fprintf(stderr, "ERROR: Unable to SET client information\n");
        
        return -1;        
    }
    
    /*
     * Here is the code that actually limits events to SYSEX
     */
    if(snd_seq_set_client_event_filter(ir_port->alsa_ir, SND_SEQ_EVENT_SYSEX) < 0) {
        
        fprintf(stderr, "ERROR: Unable to filter events (SYSEX only)\n");
        
        return -1;        
    }
    
    /*
     * Create a simple port to read events from... a lot of assumptions are made
     * here, see above for details.
     */
    if ((ir_port->alsa_seq_portid = snd_seq_create_simple_port(ir_port->alsa_ir, "livetools",
            SND_SEQ_PORT_CAP_READ, SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
                
        fprintf(stderr, "ERROR: Unable to create ALSA sequencer port\n");
                
        return -1;
    }
    
    return 0;
#endif
}

/*
 * free_alsa_device
 *
 *
 * Description: This will destory all of the ALSA objects that were created when
 *              the system was initialized.
 *
 */
int free_alsa_device(struct liveir_dev *ir_port)
{
#if 0
    if(snd_seq_close(ir_port->alsa_ir) < 0) {
        
        fprintf(stderr, "ERROR: Unable to close ALSA sequencer\n");
    }
    
    ir_port->alsa_seq_portid = -1;
    
    return 0;
#endif
}

/*
 * read_alsa_device
 *
 *
 * Description: This is where things get complicated. The function must only process_cmd_line
 *              System Exclusive messages that are from the Creative Live!Drive IR. This is
 *              accomplished by validating the code and works COMPLETELY under the assumption
 *              that this port is just subscribed to the main ALSA port and all these events
 *              are just copies of the original.
 *              
 *
 */
struct liveir_pkt *read_alsa_device(struct liveir_dev *ir_port)
{
#if 0
    struct liveir_pkt *pkt;
    struct sndrv_seq_ev_ext sysex;
    
    snd_seq_event_t *ev;    

    /* Only bother to get an event if there is one waiting on the device */
    if(snd_seq_event_input_pending(ir_port->alsa_ir, 0) > 0) {
        
        snd_seq_event_input(ir_port->alsa_ir, &ev);
        
        if(ev->type == SND_SEQ_EVENT_SYSEX) {
            
            sysex = ev->data;
        
            if(sysex.len == CL_PKT_LEN) {
                
                pkt = (struct liveir_pkt *)malloc(sizeof(struct liveir_pkt));
                    
                if(pkt == NULL) {
                    
                    fprintf(stderr, "ERROR: Unable to allocate memory for incoming packet\n");
                    
                    snd_seq_free_event(ev);
                    
                    return NULL;
                }
                                                        
                memcpy(pkt, sysex.ptr, sizeof(struct liveir_pkt));
                    
                /* This wasn't needed in earlier versions of ALSA, it may be a noop */
                snd_seq_free_event(ev);
                
                if(validate_code_packet(pkt) == 0) return pkt;

                free(pkt);
                
                return NULL;
            }
            else {
                
                fprintf(stderr, "ERROR: Packet length mismatch\n");
                
                return NULL;
            }
        } /* End ev->type */
        else {
            
            /* Ummm.. this shouldn't happen.. ever. */
            fprintf(stderr, "ERROR: Recieved non-SYSEX event. Exiting\n");
            
            exit(-1);
        } 
    } /* End input_pending */
    
    return NULL;
#endif
}
