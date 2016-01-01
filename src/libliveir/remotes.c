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
 * Default decoder functions
 *
 * rm900_decoder - RM900 Remote that ships with the Live!Drive IR
 *
 */

remote_control_key_t *rm900_decoder(struct liveir_pkt *pkt)
{
	int i = 0;
    unsigned long key;

    static remote_control_key_t key_list[] =
    {
        /* Begin RM-900 Key list */
        { CL_KEY_RM900_ONE,                 "1"            },
        { CL_KEY_RM900_TWO,                 "2"            },
        { CL_KEY_RM900_THREE,               "3"            },
        { CL_KEY_RM900_FOUR,                "4"            },
        { CL_KEY_RM900_FIVE,                "5"            },
        { CL_KEY_RM900_SIX,                 "6"            },
        { CL_KEY_RM900_SEVEN,               "7"            },
        { CL_KEY_RM900_EIGHT,               "8"            },
        { CL_KEY_RM900_NINE,                "9"            },
        { CL_KEY_RM900_ZERO,                "0"            },
    
        { CL_KEY_RM900_STOPEJECT,           "Stop/Eject"   },
        { CL_KEY_RM900_PLAYPAUSE,           "Play/Pause"   },
        { CL_KEY_RM900_SLOW,                "Slow"         },
        { CL_KEY_RM900_PREVIOUS,            "Previous"     },
        { CL_KEY_RM900_NEXT,                "Next"         },
        { CL_KEY_RM900_STEP,                "Step"         },
    
        { CL_KEY_RM900_MUTE,                "Mute"         },
        { CL_KEY_RM900_VOLDOWN,             "Volume Down"  },
        { CL_KEY_RM900_VOLUP,               "Volume Up"    },
    
        { CL_KEY_RM900_EAX,                 "EAX"          },
        { CL_KEY_RM900_OPTIONS,             "Options"      },
        { CL_KEY_RM900_DISPLAY,             "Display"      },
        { CL_KEY_RM900_RETURN,              "Return"       },
        { CL_KEY_RM900_STARTMENU,           "Start Menu"   },
        { CL_KEY_RM900_CLOSE,               "Close"        },
    
        { CL_KEY_RM900_SELECT,              "Select"       },
        { CL_KEY_RM900_UP,                  "Up"           },
        { CL_KEY_RM900_DOWN,                "Down"         },
        { CL_KEY_RM900_REVERSE,             "Reverse"      },
        { CL_KEY_RM900_FORWARD,             "Forward"      },
		
        { CL_KEY_RM1000W_POWER,             "Power"        },
        { CL_KEY_RM1000W_CMSS,				"CMSS"	       },
        { CL_KEY_RM1000W_RECORD,            "Record"       },
        { CL_KEY_RM1000W_SLOW,              "Slow"         },
        { CL_KEY_RM1000W_OPTIONS,              "Options"         },
		
        { 0,                                "\0"           }
    };

    remote_control_key_t *rm;

    /*
     * Create the key
     */
    key = pkt->check_bit<<16;

    key |= pkt->low_key_id<<8;

    key |= pkt->high_key_id;

#ifdef DEBUG

    fprintf(stderr, "WARNING: Key sent: %08x\n", key);

#endif

    rm = (remote_control_key_t *)malloc(sizeof(remote_control_key_t));

    if(rm == NULL) {

        fprintf(stderr, "ERROR: Unable to allocate memory for remote control key\n");
        return NULL;
    }

	do {

		/* Remotes may have the same key (maybe?). I think I'll play it safe */
		if(key_list[i].key == key) {

			memcpy(rm, &key_list[i], sizeof(remote_control_key_t));
			return rm;
		}

		i++;
	} while(key_list[i].key != 0);

    return NULL;
}

remote_control_key_t *ir2000_decoder(struct liveir_pkt *pkt)
{
	int i = 0;
    unsigned long key;

    static remote_control_key_t key_list[] =
    {
        /* Begin ir2000 Key list */
        { CL_KEY_iR2000_ONE,                 "1"            },
        { CL_KEY_iR2000_TWO,                 "2"            },
        { CL_KEY_iR2000_THREE,               "3"            },
        { CL_KEY_iR2000_FOUR,                "4"            },
        { CL_KEY_iR2000_FIVE,                "5"            },
        { CL_KEY_iR2000_SIX,                 "6"            },
        { CL_KEY_iR2000_SEVEN,               "7"            },
        { CL_KEY_iR2000_EIGHT,               "8"            },
        { CL_KEY_iR2000_NINE,                "9"            },
        { CL_KEY_iR2000_ZERO,                "0"            },
    
        { CL_KEY_iR2000_STOPEJECT,           "Stop/Eject"   },
        { CL_KEY_iR2000_PLAYNEXT,            "Play/Next"   },
        { CL_KEY_iR2000_MENU,                "Menu"         },
        { CL_KEY_iR2000_PREVIOUS,            "Previous"     },
    
        { CL_KEY_iR2000_MUTE,                "Mute"         },
        { CL_KEY_iR2000_VOLDOWN,             "Volume Down"  },
        { CL_KEY_iR2000_VOLUP,               "Volume Up"    },

        { CL_KEY_iR2000_MAGNIFY,             "Magnify"      },
    
        { CL_KEY_iR2000_ENTER,               "Enter"        },
        { CL_KEY_iR2000_SHIFT,               "Shift"        },
        { CL_KEY_iR2000_MOUSE,               "Mouse"        },
        { CL_KEY_iR2000_REVERSE,             "Reverse"      },
        { CL_KEY_iR2000_FORWARD,             "Forward"      },
        { 0,                                "\0"            }
    };

    remote_control_key_t *rm;

    /*
     * Create the key
     */
    key = pkt->check_bit<<16;

    key |= pkt->low_key_id<<8;

    key |= pkt->high_key_id;

#ifdef DEBUG

    fprintf(stderr, "WARNING: Key sent: %08x\n", key);

#endif

    rm = (remote_control_key_t *)malloc(sizeof(remote_control_key_t));

    if(rm == NULL) {

        fprintf(stderr, "ERROR: Unable to allocate memory for remote control key\n");
        return NULL;
    }

	do {

		/* Remotes may have the same key (maybe?). I think I'll play it safe */
		if(key_list[i].key == key) {

			memcpy(rm, &key_list[i], sizeof(remote_control_key_t));
			return rm;
		}

		i++;
	} while(key_list[i].key != 0);

    return NULL;
}
