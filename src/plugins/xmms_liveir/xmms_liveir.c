/*
 * Copyright (C) Stephen Evanchik <evanchsa@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/*
 * xmms_liveir.c - a XMMS plugin for Creative Live!Drive IR
 *
 * Based on code by:
 * Ryan Veety <ryan@ryanspc.com> for the AST remote control 
 *  (yeah it's pretty much the same code)
 */

#include <xmms/plugin.h>
#include <xmms/xmmsctrl.h>
#include <xmms/configfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "xmms_liveir.h"

static pthread_t tid, pause_tid;
static pthread_mutex_t start_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t start_cond = PTHREAD_COND_INITIALIZER;

gint session = 0;

struct liveir_conf config_info;

int do_msgrcv(int id);

void liveir_ir2000_keypress(remote_control_key_t *rm)
{
    gint time;
    static gint vol;
    static int mute = FALSE;

    switch (rm->key)
    {
		case CL_KEY_iR2000_PAUSE:
			if(xmms_remote_is_playing(session)) {
                
				xmms_remote_pause(session);
			}
			break;
        case CL_KEY_iR2000_PLAYNEXT:
            if(xmms_remote_is_playing(session)) {
                
                    playlist_next(session);
            }
            else xmms_remote_play(session);
            break;
        case CL_KEY_iR2000_STOPEJECT:
            if(!xmms_remote_is_playing(session) &&
               !xmms_remote_is_paused(session))
            {
            }
            else xmms_remote_stop(session);
            break;
        case CL_KEY_iR2000_PREVIOUS:
            playlist_prev();
            break;
        case CL_KEY_iR2000_FORWARD:
            time = xmms_remote_get_output_time(session);
            time += 5000;
            xmms_remote_jump_to_time(session, time);
            break;
        case CL_KEY_iR2000_REVERSE:
            time = xmms_remote_get_output_time(session);
            time -= 5000;
            xmms_remote_jump_to_time(session, time);
            break;
        case CL_KEY_iR2000_VOLUP:
            if(!mute) {
                
                vol = xmms_remote_get_main_volume(session);
                ((vol>=100 ) ? vol=100:(vol+=3));
                xmms_remote_set_main_volume(session, vol);
            }
            else {
                
                xmms_remote_set_main_volume(session, vol);
                mute = FALSE;
            }
            break;
        case CL_KEY_iR2000_VOLDOWN:
            if(!mute) {
                
                vol = xmms_remote_get_main_volume(session);
                ((vol<=0 ) ? vol=0:(vol-=3));
                xmms_remote_set_main_volume(session, vol);
            }
            else {
            
                xmms_remote_set_main_volume(session, vol);
                mute = FALSE;
            }
            break;
        case CL_KEY_iR2000_MUTE:
            if(mute) {
                
                xmms_remote_set_main_volume(session, vol);
                mute=FALSE;
            }
            else {
                
                vol =  xmms_remote_get_main_volume(session);
                xmms_remote_set_main_volume(session, 0);
                mute=TRUE;
            }
            break;
        case CL_KEY_iR2000_ENTER:
            xmms_remote_play(session);
            break;
	}
}

/* Handles all of our keys from the IR port */
void liveir_rm900_keypress(remote_control_key_t *rm)
{
	gint time;
	static gint vol;
	static int mute = FALSE;

    switch (rm->key)
    {
        case CL_KEY_RM900_PLAYPAUSE:
            if(!xmms_remote_is_playing(session)) {
                
                xmms_remote_play(session);
			}
            else xmms_remote_pause(session);
            break;
        case CL_KEY_RM900_STOPEJECT:
			if(!xmms_remote_is_playing(session) &&
			   !xmms_remote_is_paused(session))
			{
			}
			else xmms_remote_stop(session);
            break;
        case CL_KEY_RM900_NEXT:
            playlist_next();
            break;
        case CL_KEY_RM900_PREVIOUS:
            playlist_prev();
            break;
		case CL_KEY_RM900_FORWARD:
			time = xmms_remote_get_output_time(session);
			time += 5000;
			xmms_remote_jump_to_time(session, time);
			break;
		case CL_KEY_RM900_REVERSE:
            time = xmms_remote_get_output_time(session);
            time -= 5000;
            xmms_remote_jump_to_time(session, time);
			break;
        case CL_KEY_RM900_VOLUP:
            if(!mute) {
                
				vol = xmms_remote_get_main_volume(session);
                ((vol>=100 ) ? vol=100:(vol+=3));
                xmms_remote_set_main_volume(session, vol); 
            }
            else {
                
                xmms_remote_set_main_volume(session, vol);
                mute = FALSE;
            }
            break;
        case CL_KEY_RM900_VOLDOWN:
            if(!mute) {
                
                vol = xmms_remote_get_main_volume(session);
                ((vol<=0 ) ? vol=0:(vol-=3));
                xmms_remote_set_main_volume(session, vol);
            }
            else {
                
                xmms_remote_set_main_volume(session, vol);
                mute = FALSE;
            }
            break;
        case CL_KEY_RM900_MUTE:
            if(mute) {
                
                xmms_remote_set_main_volume(session, vol);
                mute=FALSE;
            }
            else {
                
                vol =  xmms_remote_get_main_volume(session);
                xmms_remote_set_main_volume(session, 0);
                mute=TRUE;
            }
            break;
		case CL_KEY_RM900_SELECT:
			xmms_remote_play(session);
			break;
        case CL_KEY_RM900_DOWN:
            if(xmms_remote_is_pl_win(session)) {
                
                xmms_remote_set_playlist_pos(session, 
                xmms_remote_get_playlist_pos(session)+1);
            }
            break;
        case CL_KEY_RM900_UP:
            if(xmms_remote_is_pl_win(session)) {
                
                xmms_remote_set_playlist_pos(session,
                xmms_remote_get_playlist_pos(session)-1);
            }
            break;
		case CL_KEY_RM900_CLOSE:
			xmms_remote_quit(session);
			break;
        }
}

int do_msgrcv(int id)
{
	int ret; 
	struct liveir_msgbuf msg;
	remote_control_key_t key;

	ret = msgrcv(id, &msg, MSG_SIZE, 0, 0);

	if(ret != -1) {
        
		memcpy(&key, msg.mtext, sizeof(remote_control_key_t));
		liveir_rm900_keypress(&key);
		liveir_ir2000_keypress(&key);
	}

	return ret;
}

void* thread_loop(void* args)
{
	int init = 0;

	int id;
	key_t k;

	while (1) {
        
		if(!init) {
		    
			k = ftok((char *)config_info.device, MSG_PROJECT);
			fprintf(stderr, "%s\n", config_info.device);

			if(k < 0) {

			perror("ftok");
			}

			if( (id = msgget(k, IPC_EXCL)) < 0) {

			perror("msgget");
			}
			init = 1;
		}
		else {
		    
		    if(do_msgrcv(id) < 0) {
			
			/* Sleep this thread after setting
			 * init to false so that we may 
			 * recover.
			 */
			init = 0;
			sleep(2);
		    }
		}
	}
}

void liveir_init()
{
   pthread_attr_t attr;

   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
   pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
   pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
   pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
   pthread_create(&tid, &attr, thread_loop, NULL);

	liveir_read_config(&config_info);
/*   strncpy(config_info.device, "/dev/midi", 255);*/
}

void liveir_cleanup(void)
{
   free(config_info.device);
   pthread_cancel(tid);
}

GeneralPlugin liveir_plugin = {
   NULL,
   NULL,
   -1,
   "Creative Live!Drive IR Plugin 1.0",
   liveir_init,
   liveir_about,
   liveir_configure,
   liveir_cleanup,
};

GeneralPlugin* get_gplugin_info()
{
   return &liveir_plugin;
}
