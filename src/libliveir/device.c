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
#include <string.h>
#include <stdlib.h>

#include "livetools.h"

/*
 * Internal display for debugging purposes
 */
void print_raw_bytes(uint8_t *bytes);

int get_byte(struct liveir_dev *ir_port, uint8_t  *byte);
int get_word(struct liveir_dev *ir_port, uint16_t *word);

/*
 * Raw device accessor functions
 *
 *
 *
 * init_raw_device
 *
 *
 * Description: This opens the raw device file where the MIDI SYSEX messages
 *              will arrive.
 *
 *
 */
int init_raw_device(char *device, struct liveir_dev *ir_port)
{
    if(!ir_port) {

         fprintf(stderr, "ERROR: Invalid liveir_dev passed to init_raw_device\n");
         return -1;
    }

    ir_port->fstream = fopen(device, "r");

    if(!ir_port->fstream) {

        fprintf(stderr, "ERROR: Unable to open device '%s' in init_raw_device\n", device);
        return -1;
    }

    /* Copy the device to the IR port's data. We may need this for debugging later */
    ir_port->device_file = (char *)malloc(strlen(device));

    if(!ir_port->device_file) {

        fprintf(stderr, "ERROR: Unable to allocate memory in init_raw_device\n");
        return -1;
    }

    strncpy(ir_port->device_file, device, strlen(device));
	
	ir_port->type = SND_RAW;
    
    /* Initialize the statistics */
    ir_port->bytes_read = 0;
    ir_port->keys_read = 0;
    ir_port->keys_lost = 0;

    return 0;
}

/*
 * close_raw_device
 *
 *
 * Description: Close and free and associated resources with a raw device 
 *
 */
int close_raw_device(struct liveir_dev *ir_port)
{
	if(!ir_port) {
        
		fprintf(stderr, "ERROR: Invalid liveir_dev passed to close_raw_device()\n");
        
		return -1;
	}

	if(ir_port->fstream) {
        
		fclose(ir_port->fstream);
		ir_port->fstream = NULL;
	}

	if(ir_port->device_file) {
        
		free(ir_port->device_file);
		ir_port->device_file = NULL;
	}
    
    return 0;
}

/*
 * read_raw_device
 *
 *
 * Description: Reads a single packet from the raw device. This tries to be smart
 *              by only processing the packets that seem to come from Creative Labs
 *              IR ports.
 *
 */
struct liveir_pkt *read_raw_device(struct liveir_dev *ir_port)
{
    int capture = 0, i = 0;
    uint8_t byte, *raw_code;

    struct liveir_pkt *pkt;

    raw_code = (uint8_t *)malloc(sizeof(uint8_t) * CL_PKT_LEN);

    if(raw_code == NULL) {

        fprintf(stderr, "ERROR: Unable to allocate raw code buffer in read_device()\n");
        
        return NULL;
    }

    while(get_byte(ir_port, &byte)) {

        ir_port->bytes_read++;
        
        if(byte == MIDI_SYS_EX_BEGIN) {

            i = 0;
            capture = 1;

            /* raw_code[i] = byte; */

            i++;
        }
        else if(capture && byte == MIDI_SYS_EX_END) {

#ifdef DEBUG
			
            /* raw_code[i] = byte; */
            fprintf(stderr, "INFO: Bytes Read: %i\n", i);
			
#endif			
            
            ir_port->keys_read++;
            
            return (struct liveir_pkt *)raw_code;
        }
        else if(capture) {

            raw_code[i] = byte;
            i++;
        } /* End else if */

    } /* End while */

    return NULL;
}

/*
 * validate_code_packet
 *
 *
 * Description: This tries to validate the packet in the loosest possible fashion
 *
 */
int validate_code_packet(struct liveir_pkt *pkt)
{  
    /* Check the manufacturer ID */ 
    if(pkt->mfgid_1 != CL_MIDI_MFG_ID_1 || 
       pkt->mfgid_2 != CL_MIDI_MFG_ID_2 ||
       pkt->mfgid_3 != CL_MIDI_MFG_ID_3) {

#ifdef DEBUG

        fprintf(stderr, "WARNING: Manufacturer IDs are not Creative Labs'\n");
        fprintf(stderr, "MFG ID 1: %02x\n", pkt->mfgid_1);
        fprintf(stderr, "MFG ID 2: %02x\n", pkt->mfgid_2);
        fprintf(stderr, "MFG ID 3: %02x\n", pkt->mfgid_3);

#endif

    }

    /* Compare the device ID to the Creative constant */
    if(pkt->device_id != CL_MIDI_DEV_ID) {

        return -1;
    }

    return 0;
}

/*
 * print_code_packet
 *
 *
 * Description: Display, to stderr, the formatted code packet that will be used
 *              throughout the decoding process
 *
 */
void print_code_packet(struct liveir_pkt *pkt)
{
    /* CL_PKT_LEN (12) 8bit integers */
    fprintf(stderr, "WARNING: Begin packet dump:\n");

    fprintf(stderr, "%02x ", pkt->unused1);
    
    fprintf(stderr, "%02x ", pkt->mfgid_1);
    fprintf(stderr, "%02x ", pkt->mfgid_2);
    fprintf(stderr, "%02x ", pkt->mfgid_3);

    fprintf(stderr, "%02x ", pkt->device_id);

    fprintf(stderr, "%02x ", pkt->unused6);
    fprintf(stderr, "%02x ", pkt->unused7);

    fprintf(stderr, "%02x ", pkt->check_bit);

    fprintf(stderr, "%02x ", pkt->low_remote_type);
    fprintf(stderr, "%02x ", pkt->high_remote_type);

    fprintf(stderr, "%02x ", pkt->low_key_id);
    fprintf(stderr, "%02x ", pkt->high_key_id);

    fprintf(stderr, "\nWARNING: Packet dump complete\n");
}

/*
 * print_raw_bytes
 *
 *
 * Description: Display, to stderr, the raw bytes of the packet, without any
 *              formatting.
 *              
 *
 */
void print_raw_bytes(uint8_t *bytes)
{
    int i = 0;

    fprintf(stderr, "WARNING: Begin Raw Code Dump -\n");

    while(i < CL_PKT_LEN && bytes[i] != MIDI_SYS_EX_END) {

        fprintf(stderr, "%02x ", bytes[i]);

        i++;
    }

    fprintf(stderr, "\nWARNING: Raw code dump complete\n");
}

/*
 * get_byte, get_word
 *
 *
 * Description: Two small helper functions that get a byte or word from a FILE
 *              stream.
 *
 */
int get_byte(struct liveir_dev *ir_port, uint8_t *byte)
{
    return fread(byte, sizeof(int8_t), 1, ir_port->fstream);
}

int get_word(struct liveir_dev *ir_port, uint16_t *word)
{
    return fread(word, sizeof(uint16_t), 1, ir_port->fstream);
}

