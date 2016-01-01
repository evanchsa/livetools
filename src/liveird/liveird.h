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
 

#ifndef LIVEIRD_H
#define LIVEIRD_H
/***************************************************************************
                          liveird.h  -  Prototype and data structure declarations
                             -------------------
    begin                : Sat Dec 22 2001
    copyright            : (C) 2001 by Stephen Evanchik
    email                : evanchsa@gmail.com
 ***************************************************************************/

#include "livetools.h"

/* Function Prototypes */


int poll_device(struct liveir_dev *ir_port, int debug);

#endif

