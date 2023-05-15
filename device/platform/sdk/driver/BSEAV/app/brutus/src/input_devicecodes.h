/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: input_devicecodes.h $
 * $brcm_Revision: 8 $
 * $brcm_Date: 1/12/12 10:15a $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/input_devicecodes.h $
 * 
 * 8   1/12/12 10:15a jrubio
 * SW7346-400: add silver remote
 * 
 * 7   2/13/08 4:52p tokushig
 * PR39534: added bluetooth remote capabilities to brutus
 * 
 * SanDiego_Brutus_3D/1   1/30/08 2:19p tokushig
 * refactor demo bluetooth remote implementation for integration into main
 * line
 * 
 * 6   10/20/06 3:12p jrubio
 * PR24927: Updated Echostar Keys
 * 
 * 5   5/9/06 11:39a jrubio
 * PR18491:Added true UHF/RF support
 * 
 * 4   1/16/06 4:30p jrubio
 * PR18491: added uhf legacy dev
 * 
 * 3   8/22/05 9:24a erickson
 * PR16747: added entertext -tty key
 * 
 * 2   4/12/05 11:47a erickson
 * PR14701: added ENTER key to terminate incremental channel entry
 * 
 * 1   2/7/05 8:11p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   4/16/04 11:27a erickson
 * PR9029: convert to BrutusCommand for device-independent command
 * 
 * Irvine_BSEAVSW_Devel/2   4/16/04 10:43a erickson
 * PR9029: made data structure private and removed frontpanel
 * 
 * SanJose_Linux_Devel/9   4/9/03 11:21a erickson
 * rework of pvrgui files and class names
 *
 ****************************************************************************/
#ifndef INPUT_DEVICECODES_H__
#define INPUT_DEVICECODES_H__

#include <bstd.h>

/**
Summary:
Device independent commands
Description:
The various InputDevice instances must translate their input into these
device independent commands.
**/
enum BrutusCommand {
	IND_PAUSE,
	IND_STOP,
	IND_PLAY,
	IND_POWER,
	IND_RECORD,
	IND_FWD,
	IND_REW,
	IND_SLOW_FWD,
	IND_SLOW_REW,
	IND_FRAME_ADV,
	IND_FRAME_REW,
	IND_CHAN_UP,
	IND_CHAN_DOWN,
	IND_TOGGLE_MODE,
	IND_TOGGLE_CTRL,
	IND_0,
	IND_1,
	IND_2,
	IND_3,
	IND_4,
	IND_5,
	IND_6,
	IND_7,
	IND_8,
	IND_9,
	IND_CATCHUP,
	IND_TOGGLE_VIDEO,
	IND_TOGGLE_AUDIO,
	IND_VOL_UP,
	IND_VOL_DOWN,
	IND_UP,
	IND_DOWN,
	IND_LEFT,
	IND_RIGHT,
	IND_SELECT,
	IND_GUIDE,
	IND_MENU,
	IND_MUTE,
	IND_TV,
	IND_INFO,
	IND_PAGEUP,
	IND_PAGEDOWN,
	IND_MUSIC,
	IND_ENTER, /* this is different from select. this terminates channel digit entry. */
	IND_ENTERTEXT, /* allow text entry on -tty input */
    IND_DVR,
    IND_SKIPFWD,
    IND_SKIPBCK,
	MAX_CMD_INDEX
};

enum DeviceEnum {
	dev_giRemote,
	dev_sejinIrKeyboard,
	dev_NOT_USED,
	dev_tty,
	dev_SARemote,
	dev_CirSonyRemote,
    dev_IREchoStar,
    dev_uhfDirecTV,
    dev_uhfEchostar,
	dev_btSonyPS3,
	dev_CirNecRemote,
	TotalInputDevices
};

/**
Summary:
Translate from a particular device to a device independent command.

Description:
This is a system global, but it's helpful to have all the commands
next to each other to prevent errors. Some exceptions include
keypad (sparse matrix) and Sejin IR keyboard.

Return Values:
0 success
-1 failure
**/
int translateCmd(DeviceEnum dev, uint32_t indata, BrutusCommand *outdata);

/**
Summary:
Lookup the text of the IND command and return the number.

Return Values:
0 success
-1 failure
**/
int lookupCmd(const char *cmd, BrutusCommand *outdata);

#endif //INPUT_DEVICECODES_H__

