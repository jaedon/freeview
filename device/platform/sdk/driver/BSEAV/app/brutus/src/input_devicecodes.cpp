/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: input_devicecodes.cpp $
 * $brcm_Revision: 13 $
 * $brcm_Date: 1/12/12 10:15a $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/input_devicecodes.cpp $
 * 
 * 13   1/12/12 10:15a jrubio
 * SW7346-400: add silver remote
 * 
 * 12   7/30/10 12:55p erickson
 * SWDEPRECATED-2637: fix indenting
 *
 * 11   2/13/08 4:52p tokushig
 * PR39534: added bluetooth remote capabilities to brutus
 *
 * SanDiego_Brutus_3D/1   1/30/08 2:19p tokushig
 * refactor demo bluetooth remote implementation for integration into main
 * line
 *
 * 10   8/2/07 11:14a jgarrett
 * PR 33735: Fixing APP_B use of slow ff/rew.
 *
 * 9   6/7/07 10:30a erickson
 * PR30651: don't convert all pagedown/up to slow fwd/rev. instead,
 * convert just GI remote pagedown/up.
 *
 * 8   10/20/06 3:09p jrubio
 * PR24927: Updated echostar codes in Mode 5.
 *
 * 7   5/11/06 4:43p jrubio
 * PR18491: Added DirecTV codes
 *
 * 6   5/9/06 11:38a jrubio
 * PR18491:Added UHF/RF remote keys
 *
 * 5   1/16/06 4:29p jrubio
 * PR18491: added codes for legacy mode EchoStar UHF remote
 *
 * 4   8/22/05 9:24a erickson
 * PR16747: added entertext -tty key
 *
 * 3   7/6/05 4:39p erickson
 * PR16119: added --exec and -tty slow forward/reverse support
 *
 * 2   4/12/05 11:47a erickson
 * PR14701: added ENTER key to terminate incremental channel entry
 *
 * 1   2/7/05 8:11p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/13   2/4/05 10:47a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 *
 * Irvine_BSEAVSW_Devel/12   7/14/04 1:36a erickson
 * PR11287: refactor linux-specific code into brutus_linux.cpp for easier
 * porting
 *
 * Irvine_BSEAVSW_Devel/11   4/16/04 11:27a erickson
 * PR9029: convert to BrutusCommand for device-independent command
 *
 * Irvine_BSEAVSW_Devel/10   4/16/04 10:43a erickson
 * PR9029: made data structure private and removed frontpanel
 *
 * Irvine_BSEAVSW_Devel/9   3/19/04 6:50p bandrews
 * PR9760: Fixed the scan codes.  97328 is just like 97315.
 *
 * Irvine_BSEAVSW_Devel/8   2/7/04 1:11a erickson
 * PR9107: tty is now handled inside brutus, so translate the codes
 *
 * Irvine_BSEAVSW_Devel/7   11/6/03 5:12p erickson
 * PR8570: initial vxworks migration
 *
 * Irvine_BSEAVSW_Devel/PR8570/1   11/6/03 5:05p erickson
 * initial vxworks migration
 *
 * Irvine_BSEAVSW_Devel/6   10/22/03 11:22a erickson
 * settop api pass-through type conversion
 *
 * Irvine_BSEAVSW_Devel/5   9/10/03 3:54p erickson
 * 7328 support
 *
 * Irvine_BSEAVSW_Devel/4   7/30/03 9:29a erickson
 * changed name of "guide" to "playback", but still support guide command.
 *
 * Irvine_BSEAVSW_Devel/3   7/24/03 6:02p erickson
 * added new interactive mode
 *
 * Irvine_BSEAVSW_Devel/2   6/24/03 3:11p erickson
 * fixed #ifdef logic
 *
 * SanJose_Linux_Devel/13   5/1/03 2:13p erickson
 * Changed 97320 INFO to MENU, otherwise there's no way to get to the MENU
 *
 * SanJose_Linux_Devel/12   4/9/03 11:21a erickson
 * rework of pvrgui files and class names
 *
 ****************************************************************************/
#include "input_devicecodes.h"
#include <bstd.h>
BDBG_MODULE(brutus_input_devicecodes);
#include <string.h>
#include "microwidgets.h" // MWKEY_ defines
#define MAXINT 0x7fffffff

/*
 * GI Remote Control use only Data0
 */
#define IRR_CMD_PLAY_PAUSE  0x1F
#define IRR_CMD_PLAY_STOP   0x39
#define IRR_CMD_PLAY        0x38 /* Play button */
#define IRR_CMD_POWER       0x0a /* POWER */
#define IRR_CMD_RECORD      0x31 /* REC */
#define IRR_CMD_PLAY_FF     0x1d /* FF */
#define IRR_CMD_PLAY_REW    0x1e /* REW */
#define IRR_CMD_CHAN_UP     0x0b /* Channel Up */
#define IRR_CMD_CHAN_DOWN   0x0c /* Channel Down */
#define IRR_CMD_MUTE        0x0f
#define IRR_CMD_1           0x01
#define IRR_CMD_2           0x02
#define IRR_CMD_3           0x03
#define IRR_CMD_4           0x04
#define IRR_CMD_5           0x05
#define IRR_CMD_6           0x06
#define IRR_CMD_7           0x07
#define IRR_CMD_8           0x08
#define IRR_CMD_9           0x09
#define IRR_CMD_0           0x00
#define IRR_CMD_GUIDE       0x30
#define IRR_CMD_MENU        0x19
#define IRR_CMD_SELECT      0x11
#define IRR_CMD_VOL_UP      0x0d
#define IRR_CMD_VOL_DOWN    0x0e
#define IRR_CMD_RIGHT       0x37 /* Right Arrow */
#define IRR_CMD_LEFT        0x36 /* Left Arrow */
#define IRR_CMD_UP          0x34 /* Select Up */
#define IRR_CMD_DOWN        0x35 /* Select Down */
#define IRR_CMD_A           0x17
#define IRR_CMD_B           0x27
#define IRR_CMD_C           0x28
#define IRR_CMD_FAVORITE    0x15
#define IRR_CMD_LOCK        0x16
#define IRR_CMD_EXIT        0x12
#define IRR_CMD_INFO        0x33
#define IRR_CMD_BYPASS      0x14
#define IRR_CMD_MUSIC       0x10
#define IRR_CMD_LAST        0x13
#define IRR_CMD_PAGEUP      0x3a
#define IRR_CMD_PAGEDOWN    0x3b

//unused
#define IRR_CMD_HELP        0x32


/*
 * Sejin Remote Control use both Data0 and Data1
 */


#define SEJIN_PAUSE         0x026b /* Help button */
#define SEJIN_PLAY_STOP     0x0e49 /* Stop */
#define SEJIN_PLAY          0x0d39 /* Play/Pause button */
#define SEJIN_POWER         0x0908 /* POWER */
#define SEJIN_RECORD        0x047c /* INFO key */
#define SEJIN_RECORD_STOP   0      /* TBD */
#define SEJIN_PLAY_FF       0x0a09 /* FF */
#define SEJIN_PLAY_REW      0x0b28 /* REW */
#define SEJIN_PLAY_SLOW     0x0e58 /* Select Right */
#define SEJIN_PLAY_SLOWR    0x0200 /* Select Left */
#define SEJIN_FRAME_ADV     0x0b04 /* Select Up */
#define SEJIN_FRAME_REV     0x02fc /* Select Down */
#define SEJIN_CHAN_UP       0x0c38 /* Channel Up */
#define SEJIN_CHAN_DOWN     0x0d48 /* Channel Down */
#define SEJIN_TOGGLE_CTRL   0x0800 /* Select Button */
#define SEJIN_CMD_1         0x01
#define SEJIN_CMD_2         0x02
#define SEJIN_CMD_3         0x03
#define SEJIN_CMD_4         0x04
#define SEJIN_CMD_5         0x05
#define SEJIN_CMD_6         0x06
#define SEJIN_CMD_7         0x07
#define SEJIN_CMD_8         0x08
#define SEJIN_CMD_9         0x09
#define SEJIN_CMD_0         0x00
#define SEJIN_CATCHUP       0x033f /* Exit, below FF */

/* On the Sejin remote control Play and Pause are toggled, which
 * requires tough timing codes in the IRR codes.
 * Therefore, we match the key Help to be Pause, and let
 * both Play/Pause to mean just Play
 */
#define SEJIN_PLAY_PAUSE    0x0539 /* Play/Pause button */

typedef struct {
    const char *str;
    unsigned long cmd[TotalInputDevices];
} InputCommand;




/**
* NOTE: For giRemote, I use MAXINT instead of 0 because of IRR_CMD_0
**/

/* Jimmy, copied from latest Gstreamer */
InputCommand inputCommands[] = {
    /* script   giRemote             sejinIrKeyboard   nothing  tty             SARemote    CirSony    IR Echostar  Uhf DirecTV    Uhf EchoStar     BT Sony PS3  CirNec*/
    { "pause",  IRR_CMD_PLAY_PAUSE,  MWKEY_F2,         0,       'x',            0x3a20bb,   0x599,      0x01,       0xe24032b,      0x20201d9,      0x39,        0xe31cff00},
    { "stop",   IRR_CMD_PLAY_STOP,   MWKEY_F1,         0,       'z',            0xb269b,    0x598,      0x21,       0xe24031a,      0x212012e,      0x38,        0xa35cff00},
    { "play",   IRR_CMD_PLAY,        MWKEY_F3,         0,       'c',            0xc267b,    0x59a,      0x30,       0xe240309,      0x032408e,      0x32,        0xe21dff00},
    { "power",  IRR_CMD_POWER,       MWKEY_ESCAPE,     0,       MWKEY_ESCAPE,   0x3820fb,   0x95,       0x10,       0xe240107,      0x0220373,      MAXINT,      0xf50aff00},
    { "rec",    IRR_CMD_RECORD,      MWKEY_F7,         0,       'r',            0xa26bb,    0x59d,      0x3e,       0xe24035e,      0x1f2012b,      0x43,        0xab54ff00},
    { "ff",     IRR_CMD_PLAY_FF,     MWKEY_F6,         0,       'n',            0x17251b,   0x59c,      0x13,       0xe24034d,      0x322429f,      0x34,        0xa659ff00},
    { "rew",    IRR_CMD_PLAY_REW,    MWKEY_F5,         0,       'b',            0x16253b,   0x59b,      0x23,       0xe24033c,      0x3124386,      0x33,        0xe619ff00},
    { "slow",   IRR_CMD_PAGEDOWN,    MWKEY_F4,         0,       '.',            0x926db,    0,          0,          MAXINT,         MAXINT,         0x61,        0},
    { "slowrew",IRR_CMD_PAGEUP,      0,                0,       ',',            0x826fb,    0,          0,          MAXINT,         MAXINT,         0x60,        0},
    { "",       MAXINT,              0,                0,       0,              0,          0,          0,          MAXINT,         MAXINT,         MAXINT,      0},
    { "",       MAXINT,              0,                0,       0,              0,          0,          0,          MAXINT,         MAXINT,         MAXINT,      0},
    { "chanup", IRR_CMD_CHAN_UP,     '=',              0,       '=',            0x2223bb,   0xb90,      0,          0xe2400d3,      0,              0x5b,        0xf609ff00},
    { "chandown",IRR_CMD_CHAN_DOWN,  '-',              0,       '-',            0x2123db,   0xb91,      0,          0xe2400e4,      0,              0x59,        0xf20dff00},
    { "",       IRR_CMD_BYPASS,      0,                0,       0,              0x3221bb,   0,          0,          0,              0,              MAXINT,      0},
    { "pip",    IRR_CMD_A,           MWKEY_RMETA,      0,       'p',            0x1025fb,   0xe4,       0x17,       0,              0x3a2424e,      MAXINT,      0xe916ff00},
    { "0",      IRR_CMD_0,           '0',              0,       '0',            0x26233b,   0xb89,      0x22,       0xe240118,      0x11203c8,      0x09,        0xad52ff00},
    { "1",      IRR_CMD_1,           '1',              0,       '1',            0x2f221b,   0xb80,      0x08,       0xe240017,      0x0420141,      0x00,        0xe01fff00},
    { "2",      IRR_CMD_2,           '2',              0,       '2',            0x2e223b,   0xb81,      0x28,       0xe240028,      0x05201b6,      0x01,        0xa15eff00},
    { "3",      IRR_CMD_3,           '3',              0,       '3',            0x2d225b,   0xb82,      0x18,       0xe240039,      0x06200af,      0x02,        0xa05fff00},
    { "4",      IRR_CMD_4,           '4',              0,       '4',            0x2c227b,   0xb83,      0x04,       0xe24004a,      0x082024c,      0x03,        0xe41bff00},
    { "5",      IRR_CMD_5,           '5',              0,       '5',            0x2b229b,   0xb84,      0x24,       0xe24005b,      0x09202bb,      0x04,        0xa55aff00},
    { "6",      IRR_CMD_6,           '6',              0,       '6',            0x2a22bb,   0xb85,      0x14,       0xe24006c,      0x0a203a2,      0x05,        0xa45bff00},
    { "7",      IRR_CMD_7,           '7',              0,       '7',            0x2922db,   0xb86,      0x0c,       0xe24007d,      0x0c20190,      0x06,        0xe817ff00},
    { "8",      IRR_CMD_8,           '8',              0,       '8',            0x2822fb,   0xb87,      0x2c,       0xe24008e,      0x0d20167,      0x07,        0xa956ff00},
    { "9",      IRR_CMD_9,           '9',              0,       '9',            0x27231b,   0xb88,      0x1c,       0xe24009f,      0x0e2007e,      0x08,        0xa857ff00},
    { "lock",   IRR_CMD_LOCK,        MWKEY_F8,         0,       'm',            0,          0,          0,          0,              0,              MAXINT,      0},
    { "swap",   IRR_CMD_C,           MWKEY_SYSREQ,     0,       'o',            0xe263b,    0,          0x2f,       0,              0x3d2408b,      MAXINT,      0},
    { "move",   IRR_CMD_B,           MWKEY_SCROLLOCK,  0,       'i',            0xf261b,    0x97,       0x37,       0,              0x3b242b9,      MAXINT,      0},
    { "volup",  IRR_CMD_VOL_UP,      ']',              0,       ']',            0x1f241b,   0x92,       0x26,       0,              0,              0x5a,        0xb748ff00},
    { "voldown",IRR_CMD_VOL_DOWN,    '[',              0,       '[',            0x1e243b,   0x93,       0x2e,       0,              0,              0x58,        0xb34cff00},
    { "u",      IRR_CMD_UP,          0,                0,       'k',            0x3d205b,   0xbf2,      0x16,       0xe240219,      0x1a20200,      0x54,        0xb14eff00},
    { "d",      IRR_CMD_DOWN,        0,                0,       'j',            0x34217b,   0xbf3,      0x1e,       0xe24022a,      0x1e201dc,      0x56,        0xf30cff00},
    { "l",      IRR_CMD_LEFT,        0,                0,       'h',            0x3c207b,   0xbf5,      0x0e,       0xe24023b,      0x1c20032,      0x57,        0xf40bff00},
    { "r",      IRR_CMD_RIGHT,       0,                0,       'l',            0x3b209b,   0xbf4,      0x06,       0xe24024c,      0x18203ee,      0x55,        0xb649ff00},
    { "s",      IRR_CMD_SELECT,      0,                0,       ' ',            0x33219b,   0xbf6,      0x02,       0xe24025d,      0x102033f,      0x0b,        0xf708ff00},
    { "playback",IRR_CMD_GUIDE,      MWKEY_F11,        0,       'e',            0x3920db,   0xbee,      0x0a,       0xe240280,      0x14200e3,      0x5d,        0},
    { "menu",   IRR_CMD_MENU,        MWKEY_F10,        0,       't',            0x6273b,    0xbb8,      0x34,       0xe240208,      0x0b20355,      0x5e,        0xb04fff00},
    { "mute",   IRR_CMD_MUTE,        '\\',             0,       '\\',           0x1d245b,   0x94,       0x2a,       0xe2405b6,      0x2820661,      MAXINT,      0xfe01ff00},
    { "exit",   IRR_CMD_EXIT,        MWKEY_F9,         0,       'w',            0x13259b,   0xa5,       0,          0xe24026e,      0x1b202f7,      MAXINT,      0xb24dff00},
    { "info",   IRR_CMD_INFO,        MWKEY_F12,        0,       'u',            0x37211b,   0xbba,      0x00,       0xe2402e6,      0x002029d,      0x5f,        0xf00fff00},
    { "pageup", 0,                   0,                0,       0,              0,          0,          0x11,       0,              0x0f24383,      0x4b,        0},
    { "pagedown",0,                  0,                0,       0,              0,          0,          0x31,       0,              0x00724352,     0x4e,        0},
    { "music",  0,                   0,                0,       0,              0x2023fb,   0xbe9,      0,          0,              0,              0x64,        0},
    { "enter",  IRR_CMD_MUSIC,       0,                0,       0,              0,          0,          0,          0,              0,              0x50,        0},
    { "entertext",0,                 0,                0,       0,              0,          0,          0,          0,              0,              MAXINT,      0},
    { "dvr",    0,                   0,                0,       0,              0,          0,          0,          0,              0x3924357,      MAXINT,      0},
    { "skipfwd",0,                   0,                0,       0,              0,          0,          0,          0,              0x37241b4,      0x31,        0},
    { "skipbck",0,                   0,                0,       0,              0,          0,          0,          0,              0x3624143,      0x30,        0},
    {0, 0}
};



int translateCmd(DeviceEnum dev, uint32_t indata, BrutusCommand *outdata)
{
    int i;
    for (i=0;inputCommands[i].str;i++) {
        if (inputCommands[i].cmd[dev] == indata)
            break;
    }
        if (i == MAX_CMD_INDEX) {
        BDBG_ERR(("Unknown command: %x", indata));
        return -1;
    }
    *outdata = (BrutusCommand)i;
    return 0;
}

int lookupCmd(const char *cmd, BrutusCommand *outdata)
{
    // map command directly to keystroke
    for (int i=0;inputCommands[i].str;i++) {
        if (!strcmp(inputCommands[i].str, cmd)) {
            *outdata = (BrutusCommand)i;
            return 0;
        }
    }
    return -1;
}
