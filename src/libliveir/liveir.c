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
                          liveir.c  -  Live!Drive IR SDK Common Routines
                             -------------------
    begin                : Mon Dec 24 2001
    copyright            : (C) 2001 by Stephen Evanchik
    email                : evanchsa@clarkson.edu
 ***************************************************************************/

#include "livetools.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int conn_msgbuf(void)
{
	int MSG_QUEUE_ID;
	key_t MSG_QUEUE_KEY;

	MSG_QUEUE_KEY = ftok("/dev/sequencer", MSG_PROJECT);
	
	if(MSG_QUEUE_KEY < 0)
	{
		perror("ftok");
		return -1;
	}

	if( (MSG_QUEUE_ID = msgget(MSG_QUEUE_KEY, O_RDONLY)) < 0)
	{
		perror("msgget");
		return -1;
	}

	return MSG_QUEUE_ID;

}

int read_msgbuf(int id, remote_control_key_t *rm)
{
	int ret;
	struct liveir_msgbuf msg;

	ret = msgrcv(id, &msg, MSG_SIZE, 0, 0);

	memcpy(rm, msg.mtext, sizeof(remote_control_key_t));

	return ret;
}
