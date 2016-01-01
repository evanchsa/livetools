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
#include <stdlib.h>

#include "livetools.h"

/*
 * List structure used to hold the various decoders that the system will have
 *
 */
typedef struct _decoder_list_t {

    liveir_decoder_t *decoder_def;

    struct _decoder_list_t *next;

} liveir_decoder_list_t;

/*
 * Global list of known remote decoders
 */
static liveir_decoder_list_t *head = NULL;

int add_decoder(int remote_identifier, liveir_decoder_fn fn)
{
    liveir_decoder_t *new_decoder;

    liveir_decoder_list_t *new_decoder_node, *temp;

    new_decoder = (liveir_decoder_t *)malloc(sizeof(liveir_decoder_t));

    if(new_decoder == NULL) {

        fprintf(stderr, "ERROR: Unable to allocate memory for new decoder in add_decoder()\n");
        return -1;
    }

    new_decoder_node = (liveir_decoder_list_t *)malloc(sizeof(liveir_decoder_list_t));

    if(new_decoder_node == NULL) {

        fprintf(stderr, "ERROR: Unable to allocate memory for new decode list noder in add_decoder()\n");
        return -1;
    }

    /* Build the new decoder definition */
    new_decoder->remote_identifier = remote_identifier;
    new_decoder->decoder_fn = fn;

    /* Populate the new doecder node with the definition */
    new_decoder_node->decoder_def = new_decoder;
    new_decoder_node->next = NULL;

    if(!head) {
        
        head = new_decoder_node;
    }
    else {

        temp = head;

        while(temp->next != NULL) {

            /* Duplicates aren't allowed */
            if(temp->decoder_def->remote_identifier == 
                new_decoder_node->decoder_def->remote_identifier) {
                    
                    fprintf(stderr, "ERROR: Discarding duplicate decoder\n");
                    
                    free(new_decoder_node->decoder_def);
                    free(new_decoder_node);
                    
                    return 0;
            }
            temp = temp->next;
        }

        temp->next = new_decoder_node;
    }

    return 0;
}

int remove_decoder(int remote_identifier)
{
    liveir_decoder_list_t *temp = head, *last = NULL;

    while(temp != NULL) {

        if(temp->decoder_def->remote_identifier == remote_identifier) {
    
            last->next = temp->next;
            free(temp);

            return 0;
        }

        last = temp;
        temp = temp->next;
    }

    return -1;
}

remote_control_key_t *call_decoder(struct liveir_pkt *pkt)
{
    int remote_id = 0;
    liveir_decoder_list_t *temp = head;

    /* Build the remote identifier */
    remote_id = pkt->high_remote_type;
    remote_id = remote_id << 8;
    remote_id |= pkt->low_remote_type;


    while(temp != NULL) {

        if(temp->decoder_def->remote_identifier == remote_id) {

#ifdef DEBUG
			
		    fprintf(stderr, "INFO: Calling Decoder with ID %04x\n", remote_id);

#endif
			
            return (remote_control_key_t *)temp->decoder_def->decoder_fn(pkt);
        }

        temp = temp->next;
    }

    return NULL;
}
