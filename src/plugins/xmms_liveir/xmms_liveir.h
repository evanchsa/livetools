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
 
#ifndef XMMS_LIVEIR_H
#define XMMS_LIVEIR_H

#include <gtk/gtk.h>

#include "livetools.h"


struct liveir_conf
{
	gchar *device;
};

extern struct liveir_conf config_info;

void liveir_rm900_keypress(remote_control_key_t *key);
void liveir_ir2000_keypress(remote_control_key_t *key);
void liveir_about();
void liveir_configure();
void liveir_cleanup();

/*
 * Extern function prototypes
 */
extern void liveir_read_config(struct liveir_conf *conf);


#endif
