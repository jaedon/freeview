/*
   (c) Copyright 2001-2010  The world wide DirectFB Open Source Community (directfb.org)
   (c) Copyright 2000-2004  Convergence (integrated media) GmbH

   All rights reserved.

   Written by Denis Oliver Kropp <dok@directfb.org>,
              Andreas Hundt <andi@fischlustig.de>,
              Sven Neumann <neo@directfb.org>,
              Ville Syrjälä <syrjala@sci.fi> and
              Claudio Ciccani <klan@users.sf.net>.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <config.h>
#include <linux/version.h>
    
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,16)
    typedef unsigned long kernel_ulong_t;
#define BITS_PER_LONG    (sizeof(long)*8)
#endif
    
#include <linux/input.h>
    
#ifndef KEY_OK
    /* Linux kernel 2.5.42+ defines additional keys in linux/input.h */
#include "input_fake.h"
#endif
    
#ifndef EV_CNT
#define EV_CNT (EV_MAX+1)
#define KEY_CNT (KEY_MAX+1)
#define REL_CNT (REL_MAX+1)
#define ABS_CNT (ABS_MAX+1)
#define LED_CNT (LED_MAX+1)
#endif
    
    /* compat defines for older kernel like 2.4.x */
#ifndef EV_SYN
#define EV_SYN                  0x00
#define SYN_REPORT              0
#define SYN_CONFIG              1
#define ABS_TOOL_WIDTH          0x1c
#define BTN_TOOL_DOUBLETAP      0x14d
#define BTN_TOOL_TRIPLETAP      0x14e
#endif
    
#ifndef EVIOCGLED
#define EVIOCGLED(len) _IOC(_IOC_READ, 'E', 0x19, len)
#endif
    
#ifndef EVIOCGRAB
#define EVIOCGRAB _IOW('E', 0x90, int)
#endif
    
#include <linux/keyboard.h>
    
    
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
    
#include <dfb_types.h>
    
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/kd.h>
#include <stdlib.h>
    
/*#define DFB_INPUTDRIVER_HAS_AXIS_INFO */

#include <directfb.h>
#include <directfb_keyboard.h>
    
#include <core/coredefs.h>
#include <core/coretypes.h>
#include <core/system.h>    
#include <core/input.h>

#include <direct/debug.h>
#include <direct/mem.h>
#include <direct/messages.h>
#include <direct/thread.h>
#include <direct/util.h>
    
#include <misc/conf.h>
#include <misc/util.h>
    
#include <sys/socket.h>
#include <sys/un.h>
    
/* Exclude hot-plug stub functionality from this input provider. */
/*#define DISABLE_INPUT_HOTPLUG_FUNCTION_STUB*/
    
#ifdef LINUX_INPUT_USE_FBDEV
#include <fbdev/fbdev.h>
#endif       
    
#include "platform_init.h"
#include <linux/input.h>
#include <core/core.h>
#include <core/input_driver.h>

#ifndef BITS_PER_LONG
#define BITS_PER_LONG        (sizeof(long) * 8)
#endif
#define NBITS(x)             ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)               ((x)%BITS_PER_LONG)
#define BIT(x)               (1UL<<OFF(x))
#define LONG(x)              ((x)/BITS_PER_LONG)
#undef test_bit
#define test_bit(bit, array) ((array[LONG(bit)] >> OFF(bit)) & 1)

#define MAX_LENGTH_OF_EVENT_STRING  1024
#define MAX_LINUX_INPUT_DEVICES     16


/* compat for 2.4.x kernel - just a compile fix */
#ifndef HAVE_INPUT_ABSINFO
typedef struct {
        s32 value;
        s32 minimum;
        s32 maximum;
        s32 fuzz;
        s32 flat;
} Input_AbsInfo;
#else
typedef struct input_absinfo Input_AbsInfo;
#endif

typedef struct {
     DFB_PlatformNexusHandle     keyboardInput;
     DFB_PlatformNexusHandle     pointerInput;
     CoreInputDevice             *device;
     DirectThread                *thread;     
     FusionSkirmish              lock;           /* Lock */
     bool                        cancel_thread;  /* set to true to cancel thread */
     InputEventType              inputDevice;
     bool                        has_keys;
     int                         dx;
     int                         dy;
} BDFBInputDeviceData;


/*Helper Functions*/
/*int translate_key( unsigned short code );*/
void flush_xy( BDFBInputDeviceData *data, bool last );
bool translate_event( const struct input_event *levt,
                               DFBInputEvent            *devt );

/* Flag that indicates if the driver is suspended when true. */


static const
int basic_keycodes [] = {
     DIKI_UNKNOWN, DIKI_ESCAPE, DIKI_1, DIKI_2, DIKI_3, DIKI_4, DIKI_5,
     DIKI_6, DIKI_7, DIKI_8, DIKI_9, DIKI_0, DIKI_MINUS_SIGN,
     DIKI_EQUALS_SIGN, DIKI_BACKSPACE,

     DIKI_TAB, DIKI_Q, DIKI_W, DIKI_E, DIKI_R, DIKI_T, DIKI_Y, DIKI_U,
     DIKI_I, DIKI_O, DIKI_P, DIKI_BRACKET_LEFT, DIKI_BRACKET_RIGHT,
     DIKI_ENTER,

     DIKI_CONTROL_L, DIKI_A, DIKI_S, DIKI_D, DIKI_F, DIKI_G, DIKI_H, DIKI_J,
     DIKI_K, DIKI_L, DIKI_SEMICOLON, DIKI_QUOTE_RIGHT, DIKI_QUOTE_LEFT,

     DIKI_SHIFT_L, DIKI_BACKSLASH, DIKI_Z, DIKI_X, DIKI_C, DIKI_V, DIKI_B,
     DIKI_N, DIKI_M, DIKI_COMMA, DIKI_PERIOD, DIKI_SLASH, DIKI_SHIFT_R,
     DIKI_KP_MULT, DIKI_ALT_L, DIKI_SPACE, DIKI_CAPS_LOCK,

     DIKI_F1, DIKI_F2, DIKI_F3, DIKI_F4, DIKI_F5, DIKI_F6, DIKI_F7, DIKI_F8,
     DIKI_F9, DIKI_F10, DIKI_NUM_LOCK, DIKI_SCROLL_LOCK,

     DIKI_KP_7, DIKI_KP_8, DIKI_KP_9, DIKI_KP_MINUS,
     DIKI_KP_4, DIKI_KP_5, DIKI_KP_6, DIKI_KP_PLUS,
     DIKI_KP_1, DIKI_KP_2, DIKI_KP_3, DIKI_KP_0, DIKI_KP_DECIMAL,

     /*KEY_103RD,*/ DIKI_BACKSLASH,
     /*KEY_F13,*/ DFB_FUNCTION_KEY(13),
     /*KEY_102ND*/ DIKI_LESS_SIGN,

     DIKI_F11, DIKI_F12, DFB_FUNCTION_KEY(14), DFB_FUNCTION_KEY(15),
     DFB_FUNCTION_KEY(16), DFB_FUNCTION_KEY(17), DFB_FUNCTION_KEY(18),
     DFB_FUNCTION_KEY(19), DFB_FUNCTION_KEY(20),

     DIKI_KP_ENTER, DIKI_CONTROL_R, DIKI_KP_DIV, DIKI_PRINT, DIKS_ALTGR,

     /*KEY_LINEFEED*/ DIKI_UNKNOWN,

     DIKI_HOME, DIKI_UP, DIKI_PAGE_UP, DIKI_LEFT, DIKI_RIGHT,
     DIKI_END, DIKI_DOWN, DIKI_PAGE_DOWN, DIKI_INSERT, DIKI_DELETE,

     /*KEY_MACRO,*/ DIKI_UNKNOWN,

     DIKS_MUTE, DIKS_VOLUME_DOWN, DIKS_VOLUME_UP, DIKS_POWER, DIKI_KP_EQUAL,

     /*KEY_KPPLUSMINUS,*/ DIKI_UNKNOWN,

     DIKS_PAUSE, DFB_FUNCTION_KEY(21), DFB_FUNCTION_KEY(22),
     DFB_FUNCTION_KEY(23), DFB_FUNCTION_KEY(24),

     DIKI_KP_SEPARATOR, DIKI_META_L, DIKI_META_R, DIKI_SUPER_L,

     DIKS_STOP,

     /*DIKS_AGAIN, DIKS_PROPS, DIKS_UNDO, DIKS_FRONT, DIKS_COPY,
     DIKS_OPEN, DIKS_PASTE, DIKS_FIND, DIKS_CUT,*/
     DIKI_UNKNOWN, DIKI_UNKNOWN, DIKI_UNKNOWN, DIKI_UNKNOWN, DIKI_UNKNOWN,
     DIKI_UNKNOWN, DIKI_UNKNOWN, DIKI_UNKNOWN, DIKI_UNKNOWN,

     DIKS_HELP, DIKS_MENU, DIKS_CALCULATOR, DIKS_SETUP,

     /*KEY_SLEEP, KEY_WAKEUP, KEY_FILE, KEY_SENDFILE, KEY_DELETEFILE,
     KEY_XFER,*/
     DIKI_UNKNOWN, DIKI_UNKNOWN, DIKI_UNKNOWN, DIKI_UNKNOWN, DIKI_UNKNOWN,
     DIKI_UNKNOWN,

     /*KEY_PROG1, KEY_PROG2,*/
     DIKS_CUSTOM1, DIKS_CUSTOM2,

     DIKS_INTERNET,

     /*KEY_MSDOS, KEY_COFFEE, KEY_DIRECTION, KEY_CYCLEWINDOWS,*/
     DIKI_UNKNOWN, DIKI_UNKNOWN, DIKI_UNKNOWN, DIKI_UNKNOWN,

     DIKS_MAIL,

     /*KEY_BOOKMARKS, KEY_COMPUTER, */
     DIKI_UNKNOWN, DIKI_UNKNOWN,

     DIKS_BACK, DIKS_FORWARD,

     /*KEY_CLOSECD, KEY_EJECTCD, KEY_EJECTCLOSECD,*/
     DIKS_EJECT, DIKS_EJECT, DIKS_EJECT,

     DIKS_NEXT, DIKS_PLAYPAUSE, DIKS_PREVIOUS, DIKS_STOP, DIKS_RECORD,
     DIKS_REWIND, DIKS_PHONE,

     /*KEY_ISO,*/ DIKI_UNKNOWN,
     /*KEY_CONFIG,*/ DIKS_SETUP,
     /*KEY_HOMEPAGE, KEY_REFRESH,*/ DIKI_UNKNOWN, DIKS_SHUFFLE,

     DIKS_EXIT, /*KEY_MOVE,*/ DIKI_UNKNOWN, DIKS_EDITOR,

     /*KEY_SCROLLUP,*/ DIKS_PAGE_UP,
     /*KEY_SCROLLDOWN,*/ DIKS_PAGE_DOWN,
     /*KEY_KPLEFTPAREN,*/ DIKI_UNKNOWN,
     /*KEY_KPRIGHTPAREN,*/ DIKI_UNKNOWN,

     /* unused codes 181-182: */
     DIKI_UNKNOWN, DIKI_UNKNOWN,

     DFB_FUNCTION_KEY(13), DFB_FUNCTION_KEY(14), DFB_FUNCTION_KEY(15),
     DFB_FUNCTION_KEY(16), DFB_FUNCTION_KEY(17), DFB_FUNCTION_KEY(18),
     DFB_FUNCTION_KEY(19), DFB_FUNCTION_KEY(20), DFB_FUNCTION_KEY(21),
     DFB_FUNCTION_KEY(22), DFB_FUNCTION_KEY(23), DFB_FUNCTION_KEY(24),

     /* unused codes 195-199: */
     DIKI_UNKNOWN, DIKI_UNKNOWN, DIKI_UNKNOWN, DIKI_UNKNOWN, DIKI_UNKNOWN,

     /* KEY_PLAYCD, KEY_PAUSECD */
     DIKS_PLAY, DIKS_PAUSE,

     /*KEY_PROG3, KEY_PROG4,*/
     DIKS_CUSTOM3, DIKS_CUSTOM4,

     DIKI_UNKNOWN,

     /*KEY_SUSPEND, KEY_CLOSE*/
     DIKI_UNKNOWN, DIKI_UNKNOWN,

     /* KEY_PLAY */
     DIKS_PLAY,

     /* KEY_FASTFORWARD */
     DIKS_FASTFORWARD,

     /* KEY_BASSBOOST */
     DIKI_UNKNOWN,

     /* KEY_PRINT */
     DIKS_PRINT,

     /* KEY_HP             */  DIKI_UNKNOWN,
     /* KEY_CAMERA         */  DIKI_UNKNOWN,
     /* KEY_SOUND          */  DIKS_AUDIO,
     /* KEY_QUESTION       */  DIKS_HELP,
     /* KEY_EMAIL          */  DIKS_MAIL,
     /* KEY_CHAT           */  DIKI_UNKNOWN,
     /* KEY_SEARCH         */  DIKI_UNKNOWN,
     /* KEY_CONNECT        */  DIKI_UNKNOWN,
     /* KEY_FINANCE        */  DIKI_UNKNOWN,
     /* KEY_SPORT          */  DIKI_UNKNOWN,
     /* KEY_SHOP           */  DIKI_UNKNOWN,
     /* KEY_ALTERASE       */  DIKI_UNKNOWN,
     /* KEY_CANCEL         */  DIKS_CANCEL,
     /* KEY_BRIGHTNESSDOWN */  DIKI_UNKNOWN,
     /* KEY_BRIGHTNESSUP   */  DIKI_UNKNOWN,
     /* KEY_MEDIA          */  DIKI_UNKNOWN,
};

static const
int ext_keycodes [] = {
     DIKS_OK, DIKS_SELECT, DIKS_GOTO, DIKS_CLEAR, DIKS_POWER2, DIKS_OPTION,
     DIKS_INFO, DIKS_TIME, DIKS_VENDOR, DIKS_ARCHIVE, DIKS_PROGRAM,
     DIKS_CHANNEL, DIKS_FAVORITES, DIKS_EPG, DIKS_PVR, DIKS_MHP,
     DIKS_LANGUAGE, DIKS_TITLE, DIKS_SUBTITLE, DIKS_ANGLE, DIKS_ZOOM,
     DIKS_MODE, DIKS_KEYBOARD, DIKS_SCREEN, DIKS_PC, DIKS_TV, DIKS_TV2,
     DIKS_VCR, DIKS_VCR2, DIKS_SAT, DIKS_SAT2, DIKS_CD, DIKS_TAPE,
     DIKS_RADIO, DIKS_TUNER, DIKS_PLAYER, DIKS_TEXT, DIKS_DVD, DIKS_AUX,
     DIKS_MP3, DIKS_AUDIO, DIKS_VIDEO, DIKS_DIRECTORY, DIKS_LIST, DIKS_MEMO,
     DIKS_CALENDAR, DIKS_RED, DIKS_GREEN, DIKS_YELLOW, DIKS_BLUE,
     DIKS_CHANNEL_UP, DIKS_CHANNEL_DOWN, DIKS_FIRST, DIKS_LAST, DIKS_AB,
     DIKS_NEXT, DIKS_RESTART, DIKS_SLOW, DIKS_SHUFFLE, DIKS_FASTFORWARD,
     DIKS_PREVIOUS, DIKS_NEXT, DIKS_DIGITS, DIKS_TEEN, DIKS_TWEN, DIKS_BREAK
};




