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

#ifndef _LIVETOOLS_H
#define _LIVETOOLS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdint.h>

#include <alsa/asoundlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* 
 * Global data structures
 *
 * Live!Drive IR device specific structure
 */
struct liveir_dev {
    
    /* 
     * Configuration varibles 
     */
    int type;

    /* 
     * RAW IR port access
     */
    FILE *fstream;

    char *device_file;
    
    /*
     * ALSA Specific Device information
     */
    snd_seq_t *alsa_ir;
    int alsa_seq_portid;

    /* 
     * Statistics 
     */
    int bytes_read;
    
    int keys_read;
    int keys_lost;
};

/*
 * Live!Drive remote control related structures
 */
struct liveir_remote {
    
    int remote_type;
    char *remote_name;

    /* Lits of keys */
    int num_keys;
    struct remote_control_key_list *keys;
        
    /* Helper pointer to the last key */
    struct remote_control_key_list *last;
};

/*
 * Remote Control key
 * 
 * Simple structure with a code and a description
 */
struct __remote_control_key {
	
    long key;
    char desc[16];
};

typedef struct __remote_control_key remote_control_key_t;

/*
 * List of remote control keys
 *
 *
 */
struct remote_control_key_list {
	
    remote_control_key_t rmkey;
    struct remote_control_key_list *next;
};

/*
 * MIDI Packet
 * 
 * MIDI Packet that is created when a key is pressed on
 * the remote control. This is a packed struct to keep
 * things clean and easy to work with.
 */
struct liveir_pkt {
    
    uint8_t unused1;
    
    uint8_t mfgid_1;
    uint8_t mfgid_2;
    uint8_t mfgid_3;

    uint8_t device_id;

    uint8_t unused6;
    uint8_t unused7;

    /* I'm not sure how this plays in to the system
     * but I know it differentiates some keys from
     * others
     */
    uint8_t check_bit;

    
    uint8_t low_remote_type;
    uint8_t high_remote_type;

    uint8_t low_key_id;
    uint8_t high_key_id;
};

/*
 * The length of a proper packet, including MIDI codes
 */
const static int CL_PKT_LEN = 12;

/* 
 * Remote control key deocder related items
 *
 * The signature of the decoder function
 */
typedef remote_control_key_t * (*liveir_decoder_fn)(struct liveir_pkt *);

/*
 * A structure that defines a particular decoder
 */
typedef struct {

    int remote_identifier;
    liveir_decoder_fn decoder_fn;
} liveir_decoder_t;

/*
 *
 *
 * Constants used throughout the system
 *
 *
 *
 */

/* Sound Device Types */
#define SND_ALSA	0
#define SND_OSS		1
#define SND_RAW		2

/* MIDI related constants */
#define MIDI_SYS_EX_BEGIN 0xF0
#define MIDI_SYS_EX_END   0xF7

/* Creative MIDI Constants */
#define CL_MIDI_MFG_ID_1     0x00
#define CL_MIDI_MFG_ID_2     0x20
#define CL_MIDI_MFG_ID_3     0x21
#define CL_MIDI_DEV_ID       0x60

/* Remote Type Constants.
 * There are many remote controls that will work with the Live!Drive IR port.
 * I've used the Creative Infra Drive remote successfully so let's be reasonable and
 * assume that someone will want to use that remote one day.
 */ 
#define CL_RM900_IDENTIFIER     0x4441
#define CL_iR2000_IDENTIFIER    0x2C21


/* Remote Control Key constants
 * Observations:
 * 1. The Live!Drive IR seems to output commands in 16bit words from the sequencer
 * 2. A command is delimited by 0xF0 and 0xF7
 * 3. The command itself is in the form 0x0005 (vendor ID) followed by <0x****, 0x005>*
 * 4. After removing the 0x0005's, all but three bytes change for each button
 * One byte is a flag of some sort (probably parity) and the other two are the actual key pressed.
 * There are two other bytes, 0x44 0x41 (0xC1 after the transformation is applied) that  form the low
 * word of the command with the upper word being the key. I think the low word is the remote type.
 * See liveird to understand how to convert the raw keycodes to these.
 */
#define CL_KEY_RM900_ONE                0x000A512E
#define CL_KEY_RM900_TWO                0x000A710E
#define CL_KEY_RM900_THREE              0x00090976
#define CL_KEY_RM900_FOUR               0x0009512E
#define CL_KEY_RM900_FIVE               0x0009215E
#define CL_KEY_RM900_SIX                0x00091E61
#define CL_KEY_RM900_SEVEN              0x000A116E
#define CL_KEY_RM900_EIGHT              0x000A413E
#define CL_KEY_RM900_NINE               0x000A6E11
#define CL_KEY_RM900_ZERO               0x0009017E

#define CL_KEY_RM900_STOPEJECT          0x000A215E
#define CL_KEY_RM900_PLAYPAUSE          0x000A1E61
#define CL_KEY_RM900_SLOW               0x00097E01
#define CL_KEY_RM900_PREVIOUS           0x000A7E01
#define CL_KEY_RM900_NEXT               0x00095E21
#define CL_KEY_RM900_STEP               0x00097E01

#define CL_KEY_RM900_MUTE               0x00097609
#define CL_KEY_RM900_VOLDOWN            0x000A4639
#define CL_KEY_RM900_VOLUP              0x00094639

#define CL_KEY_RM900_EAX                0x0009314E
#define CL_KEY_RM900_OPTIONS            0x9998413E
#define CL_KEY_RM900_DISPLAY            0x00096E11
#define CL_KEY_RM900_RETURN             0x0009710E
#define CL_KEY_RM900_STARTMENU          0x0009116E
#define CL_KEY_RM900_CLOSE              0x00093E41

#define CL_KEY_RM900_SELECT             0x000A017E
#define CL_KEY_RM900_UP                 0x000A5E21
#define CL_KEY_RM900_DOWN               0x000A314E
#define CL_KEY_RM900_REVERSE            0x000A611E
#define CL_KEY_RM900_FORWARD            0x000A2E51

/* Creative RM-1000W Constants (Same remote as RM-900)
 *
 */
#define CL_KEY_RM1000W_POWER			0x000A512E
#define CL_KEY_RM1000W_CMSS				0x000A0E71
#define CL_KEY_RM1000W_RECORD			0x000A4E31
#define CL_KEY_RM1000W_SLOW				0x000A3E41
#define CL_KEY_RM1000W_OPTIONS			0x0009413E


/* Creative iR2000 Constants
 *
 */
#define CL_KEY_iR2000_PAUSE             0x0006007F
#define CL_KEY_iR2000_PLAYNEXT          0x0005403F
#define CL_KEY_iR2000_STOPEJECT         0x0006403F
#define CL_KEY_iR2000_REVERSE           0x0006205F
#define CL_KEY_iR2000_PREVIOUS          0x0005601F
#define CL_KEY_iR2000_FORWARD           0x0006601F

#define CL_KEY_iR2000_MENU              0x0005205F
#define CL_KEY_iR2000_MOUSE             0x0005106F
#define CL_KEY_iR2000_MUTE              0x0005304F
#define CL_KEY_iR2000_VOLUP             0x00050877
#define CL_KEY_iR2000_VOLDOWN           0x00052857
#define CL_KEY_iR2000_MAGNIFY           0x00051867

#define CL_KEY_iR2000_ONE               0x0006106F
#define CL_KEY_iR2000_TWO               0x0005502F
#define CL_KEY_iR2000_THREE             0x0006502F
#define CL_KEY_iR2000_FOUR              0x0006304F
#define CL_KEY_iR2000_FIVE              0x0005700F
#define CL_KEY_iR2000_SIX               0x0006700F
#define CL_KEY_iR2000_SEVEN             0x00060877
#define CL_KEY_iR2000_EIGHT             0x00054837
#define CL_KEY_iR2000_NINE              0x00064837
#define CL_KEY_iR2000_ZERO              0x00056817
#define CL_KEY_iR2000_SHIFT             0x00062857
#define CL_KEY_iR2000_ENTER             0x00066817

/* The maximum size of a message */
#define MSG_SIZE 32

/* The internal message buffer */
struct liveir_msgbuf
{
	long mtype;
	char mtext[MSG_SIZE];
};

/* A global constant used to define the message queue */
static const char MSG_PROJECT = 'L';


/*
 *
 * Function prototypes
 *
 * OSS/RAW Device Support
 *
 * init_raw_device
 *
 *
 * Description: This opens the raw device file where the MIDI SYSEX messages
 *              will arrive.
 *
 *
 */
int init_raw_device(char *device, struct liveir_dev *ir_port);

/*
 * close_raw_device
 *
 *
 * Description: Close and free and associated resources with a raw device 
 *
 */
int close_raw_device(struct liveir_dev *ir_port);

/*
 * read_raw_device
 *
 *
 * Description: Reads a single packet from the raw device. This tries to be smart
 *              by only processing the packets that seem to come from Creative Labs
 *              IR ports.
 *
 */
struct liveir_pkt *read_raw_device(struct liveir_dev *ir_port);

/*
 * print_code_packet
 *
 *
 * Description: Display, to stderr, the formatted code packet that will be used
 *              throughout the decoding process
 *
 */
void print_code_packet(struct liveir_pkt *pkt);
	
/*
 * ALSA Support
 * 
 * init_alsa_device
 *
 *
 * Description: This will initialize the liveir_port's ALSA sequencer device.
 *              It can take as an input the device_name -- however I don't know
 *              when anything other than "default" will be the value....
 *
 */
int init_alsa_device(struct liveir_dev *ir_port, char *device_name);
    
/*
 * free_alsa_device
 *
 *
 * Description: This will destory all of the ALSA objects that were created when
 *              the system was initialized.
 *
 */
int free_alsa_device(struct liveir_dev *ir_port);
    
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
struct liveir_pkt *read_alsa_device(struct liveir_dev *ir_port);

/*
 * validate_code_packet
 *
 *
 * Description: 
 *              
 *
 */
int validate_code_packet(struct liveir_pkt *pkt);

/*
 * Decoder functions
 *
 * add_decoder
 *
 *
 * Description: 
 *              
 *
 */
int add_decoder(int remote_identifier, liveir_decoder_fn fn);

/*
 * remove_decoder
 *
 *
 * Description: 
 *              
 *
 */
int remove_decoder(int remote_identifier);

/*
 * call_decoder
 *
 *
 * Description: 
 *              
 *
 */
remote_control_key_t *call_decoder(struct liveir_pkt *pkt);

/*
 *
 * Default decoder functions
 *
 * Description: These are the two remotes that I have tested
 *
 */
remote_control_key_t *rm900_decoder(struct liveir_pkt *pkt);
remote_control_key_t *ir2000_decoder(struct liveir_pkt *pkt);

#ifdef __cplusplus
}
#endif

#endif /* _LIVETOOLS_H */
