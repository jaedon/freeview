/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: input_keypad.cpp $
 * $brcm_Revision: 25 $
 * $brcm_Date: 5/18/12 8:04p $
 * $brcm_Revision: 25 $ *
 * Module Description: Brutus input device code
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/input_keypad.cpp $
 * 
 * 25   5/18/12 8:04p bselva
 * SW7360-19: Add support for 7360 platform
 * 
 * 24   4/6/12 12:18p mward
 * SW7435-56:  Use BDBG_WRN for unknown command from keypad.  Can be
 * normal for some board configurations.
 * 
 * 23   3/21/12 1:53p erickson
 * SW7425-2664: remove old code
 * 
 * 22   11/16/11 9:41a erickson
 * SW7425-1747: remove old code
 * 
 * 21   7/18/11 7:31p xhuang
 * SW7552-25: Add front panel support for 7552
 * 
 * 20   6/21/11 4:15p bselva
 * SW7358-51:Add Front panel support for 7358 platform
 * 
 * 19   4/27/11 4:42p jrubio
 * SW7346-158: fix frontpanel for 7346
 * 
 * 18   2/15/11 3:54p randyjew
 * SW7344-25: Add Keypad Codes for 7344
 * 
 * 17   10/5/10 4:34p nitinb
 * SW7550-305: Added keycode for 7550 power button
 * 
 * 16   2/25/10 4:33p nitinb
 * SW7550-209: Added support for 7550
 * 
 * 15   10/21/09 11:39a jrubio
 * SW7340-51: fix keypad for 7340/7342
 * 
 * 14   9/16/09 5:56p jrubio
 * SW7325-546: Fix Keypad Mapping
 * 
 * 13   8/10/09 1:36p jrubio
 * PR55232: add 7340/7342
 * 
 * 12   2/4/09 10:27a jrubio
 * PR51629: add 7336 support
 * 
 * 11   6/27/08 2:36p erickson
 * PR44176: make test for repeat keypad keys universal
 * 
 * 10   6/24/08 2:20p erickson
 * PR43831: add 3548/3556 keypad codes to Brutus
 *
 * 9   5/22/08 6:33p jrubio
 * PR42353: add 7335/7325 support
 *
 * 8   4/24/08 9:15a erickson
 * PR39453: add 3548/3556
 *
 * 7   3/19/08 1:15p erickson
 * PR36068: add 3563 keypad input
 *
 * 6   11/12/07 10:55a jtna
 * PR32100: Keypad generates unknown command errors
 *
 * 5   9/22/06 3:17p gmohile
 * PR 24129: Added support for 7313
 *
 * 4   9/13/06 9:49a erickson
 * PR24230: remove 7327 and 7329
 *
 * 3   4/22/05 2:32p erickson
 * PR14708: hex msg is more useful
 *
 * 2   4/13/05 10:53a erickson
 * PR14784: added frontpanel support for 97315 magnum-based platforms,
 * removed 2nd layer of #defines
 *
 * 1   2/7/05 8:12p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/9   1/26/05 5:32p bandrews
 * PR13187: Fixed for 7329
 *
 * Irvine_BSEAVSW_Devel/8   1/17/05 7:17p bandrews
 * PR13158: 7318 front panel codes
 *
 * Irvine_BSEAVSW_Devel/7   10/6/04 2:07p bandrews
 * PR12687: 7312 fp works like 7315
 *
 * Irvine_BSEAVSW_Devel/6   9/15/04 2:41p erickson
 * PR9683: use explicity _FLAVOR or _FAMILY instead of BCM7### defines
 *
 * Irvine_BSEAVSW_Devel/5   7/26/04 7:02p liangl
 * PR11643: merging brutus vxworks support
 *
 * Irvine_BSEAVSW_Devel/Brutus_Vxworks_PreRelease/1   6/17/04 1:19p liangl
 * compiled vxWorks version
 *
 * Irvine_BSEAVSW_Devel/4   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/3   4/19/04 2:02p erickson
 * PR9029: need to use unique values for keypad switch statement
 *
 * Irvine_BSEAVSW_Devel/2   4/16/04 11:27a erickson
 * PR9029: convert to BrutusCommand for device-independent command
 *
 * Irvine_BSEAVSW_Devel/1   4/16/04 10:44a erickson
 * PR9029: new object-oriented input device design
 *
 *****************************************************************/
#include "input_keypad.h"
#include "input_devicecodes.h"
#include "inputengine.h"
#include "control.h"
#include "tvscreen.h"
#include <bstd.h>
BDBG_MODULE(brutus_input_keypad);

#define tv()            ((TVScreen *)_control->screen(Control::eTV))

// taken from diags frontpanel.c
#if BCHP_CHIP == 7325 || BCHP_CHIP==7335 || \
        BCHP_CHIP == 7340 || BCHP_CHIP == 7342 ||\
        BCHP_CHIP == 7344 || BCHP_CHIP == 7346
    

#define KEYPAD_POWER                       0
#define KEYPAD_ARROW_UP                    0x80
#define KEYPAD_SELECT                      0x40
#define KEYPAD_ARROW_LEFT                  0x20
#define KEYPAD_ARROW_RIGHT                 0x10
#define KEYPAD_MENU                        0x08
#define KEYPAD_GUIDE                       0x04
#define KEYPAD_ARROW_DOWN                  0x02
#define KEYPAD_INFO                        -5
/* the following don't exist on this platform */
#define KEYPAD_VOLUME_UP    -1
#define KEYPAD_VOLUME_DOWN  -2
#define KEYPAD_CHANNEL_UP   -3
#define KEYPAD_CHANNEL_DOWN -4

#elif BCHP_CHIP == 7550 || BCHP_CHIP == 7358 || BCHP_CHIP == 7552 || BCHP_CHIP == 7360
/* 7550 */
#define KEYPAD_POWER                       0xFEFE
#define KEYPAD_ARROW_UP                    0xDFFF
#define KEYPAD_SELECT                      0xFDFF
#define KEYPAD_ARROW_LEFT                  0xFFFD
#define KEYPAD_ARROW_RIGHT                 0xFEFF
#define KEYPAD_MENU                        0xEFFF // "INFO" is actually "MENU"
#define KEYPAD_ARROW_DOWN                  0xFFDF
#define KEYPAD_INFO                        0
/* the following don't exist on this platform */
#define KEYPAD_VOLUME_UP    -1
#define KEYPAD_VOLUME_DOWN  -2
#define KEYPAD_CHANNEL_UP   -3
#define KEYPAD_CHANNEL_DOWN -4
#define KEYPAD_GUIDE        -6
#else

#define KEYPAD_RELEASE         0xFFFF
#define KEYPAD_VOLUME_UP       0xFBFF
#define KEYPAD_VOLUME_DOWN     0xBFFF
#define KEYPAD_CHANNEL_UP      0xFFFB
#define KEYPAD_CHANNEL_DOWN    0xFFBF
#define KEYPAD_ARROW_UP        0xFEFF
#define KEYPAD_ARROW_DOWN      0xFFFD
#define KEYPAD_ARROW_LEFT      0xFFEF
#define KEYPAD_ARROW_RIGHT     0xFFDF
#define KEYPAD_SELECT          0xEFFF
#define KEYPAD_MENU            0xDFFF
#define KEYPAD_GUIDE           0xFDFF
#define KEYPAD_POWER           0xFFFE
#define KEYPAD_INFO 0

#endif

void KeypadInput::process(uint32_t data)
{
	/* reject repeats */
	if (0x80000000 & data) return;

    /* There's a limited set of keys, so I'm using switch instead of a sparse matrix. */
    switch (data) {
    case KEYPAD_POWER:      data = IND_POWER; break;
    case KEYPAD_CHANNEL_UP: data = IND_CHAN_UP; break;
    case KEYPAD_CHANNEL_DOWN: data = IND_CHAN_DOWN; break;
    case KEYPAD_VOLUME_UP:  data = IND_VOL_UP; break;
    case KEYPAD_VOLUME_DOWN:data = IND_VOL_DOWN; break;
    case KEYPAD_ARROW_UP:   data = IND_UP; break;
    case KEYPAD_ARROW_DOWN: data = IND_DOWN; break;
    case KEYPAD_ARROW_LEFT: data = IND_LEFT; break;
    case KEYPAD_ARROW_RIGHT:data = IND_RIGHT; break;
    case KEYPAD_SELECT:     data = IND_SELECT; break;
    case KEYPAD_GUIDE:      data = IND_GUIDE; break;
    case KEYPAD_MENU:       data = IND_MENU; break;
    case KEYPAD_INFO:   data = IND_INFO; break;
    default:
        BDBG_WRN(("Unknown command %#x", data));
        return;
    }

    /**
    * Change the meaning of front panel keys in certain contexts
    * in order to make it more functional.
    **/
    int currentScreen = _control->currentScreen();
    if (currentScreen == Control::eTV && !app()->inModalState()) {
#ifdef INFOPANEL_SUPPORT
        if (_control->isPlaying() && !tv()->isInfoVisible())
            switch (data) {
            case IND_LEFT: data = IND_REW; break;
            case IND_RIGHT: data = IND_FWD; break;
            case IND_UP: data = IND_PAUSE; break;
            case IND_DOWN: data = IND_STOP; break;
            case IND_SELECT:
#ifdef PVR_SUPPORT
                if (_control->playEngine()->state() != bplayback_state_playing)
                    data = IND_PLAY;
                else
#endif
                    data = IND_INFO;
                break;
            }
        else if (!tv()->isInfoVisible())
#endif
        {

            switch (data) {
            case IND_UP: data = IND_CHAN_UP; break;
            case IND_DOWN:
                if (_control->totalRecords())
                    data = IND_STOP;
                else
                    data = IND_CHAN_DOWN;
                break;
            case IND_SELECT: data = IND_INFO; break;
            }
        }
    }

    _inputEngine->processCmd((BrutusCommand)data);
}

