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
                          msgsrv.c  -  Live!Drive IR Port IPC Message Server
                             -------------------
    begin                : Sun Dec 23 2001
    copyright            : (C) 2001 by Stephen Evanchik
    email                : evanchsa@clarkson.edu
 ***************************************************************************/

#include "msgsrv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/* Message queue size constant */
const static int MAX_MSG_QUEUE_SIZE = 4 * MSG_SIZE;

/* Gloabl message queue identifier */
int	MSG_QUEUE_ID;

int create_msgbuf(char *path)
{
	key_t MSG_QUEUE_KEY;
	struct msqid_ds tmp_info;

	/* Create the unique key from the specified path and project constant */
	MSG_QUEUE_KEY = ftok(path, MSG_PROJECT);
	
	if(MSG_QUEUE_KEY < 0) {
        
		perror("create_msgbuf - ftok");
		return -1;
	}

	/* Set up the message queue's permissions.
	 * Owner: RW, Group: R, Others: R
	 * Actually create the message queue
	 */
	if( (MSG_QUEUE_ID = msgget(MSG_QUEUE_KEY, IPC_CREAT|0644)) < 0) {
        
		perror("create_msgbuf - msgget");
		return -1;
	}

	/* Reduce the amount of memory available for the queue.
	 * Only allocate enough for 4, 16 byte messages (by default)
	 */
	if(msgctl(MSG_QUEUE_ID, IPC_STAT, &tmp_info) < 0) {
        
		perror("create_msgbuf - msgctl");
		/* return -1; */
	}

	tmp_info.msg_qbytes = MAX_MSG_QUEUE_SIZE;

	if(msgctl(MSG_QUEUE_ID, IPC_SET, &tmp_info) < 0) {
		perror("create_msgbuf - msgctl");
		/* return -1; */
	}

	return 0;
}

int dispatch_message(remote_control_key_t *key)
{
	struct liveir_msgbuf msg, tmp;

#ifdef DEBUG

	printf("DEBUG: Remote Key ID: %#08lx\n", key->key);
	printf("DEBUG: Remote Key Text: %s\n", key->desc);

#endif

	msg.mtype = MSG_KEY_SENT;
	memcpy(msg.mtext, key, sizeof(remote_control_key_t));
/*
	strncpy(msg.mtext, key->desc, MSG_SIZE);
*/
	/* Validate the msgbuf */
	if(msg.mtype < 0) {
        
		fprintf(stderr, "ERROR: dispatch_message has message type as < 0 (%ld).", msg.mtype);
		return -1;
	}

	/* Attempt to send the message, if the queue is full, return
	 * immediatly. In the future we should probably clear the queue
	 * if it is full.
	 */
	if(msgsnd(MSG_QUEUE_ID, &msg, MSG_SIZE, IPC_NOWAIT) < 0 && errno != EAGAIN) {
        
		perror("dispatch_message - msgsnd");
		return -1;
	}

	/* The queue is full, we should clear it for next time ;)
	 */
	if(errno == EAGAIN) {
        
		while(msgrcv(MSG_QUEUE_ID, &tmp, MSG_SIZE, 0, IPC_NOWAIT) > 0)

#ifdef DEBUG

		{
			printf("DEBUG: Removing message\n");	
		}

		printf("DEBUG: MESSAGE RESEND IN PROGRESS\n");
#else

		;

#endif

		/* Attempt to resend, this time with all errors enforced */
		if(msgsnd(MSG_QUEUE_ID, &msg, MSG_SIZE, IPC_NOWAIT) < 0) {
            
			perror("dispatch_message - msgsnd");
			return -1;
		}
	} /* End if */

	return 0;
}

int destroy_msgbuf(void)
{
	/* Destroy the message queue */
	if(msgctl(MSG_QUEUE_ID, IPC_RMID, NULL) < 0 && errno != EIDRM) {
        
		perror("destroy_msgbuf - msgctl");
		/* return -1; */
	}
	return 0;
}

