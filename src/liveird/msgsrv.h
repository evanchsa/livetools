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
                          msgsrv.h  -  Live!Drive IR Port IPC Message Server
                             -------------------
    begin                : Sun Dec 23 2001
    copyright            : (C) 2001 by Stephen Evanchik
    email                : evanchsa@gmail.com
 ***************************************************************************/

#include "liveird.h"

/* Some message types */
#define MSG_KEY_SENT 1

/* Function prototypes */
int create_msgbuf(char *path);
int dispatch_message(remote_control_key_t *key);
int destroy_msgbuf(void);

