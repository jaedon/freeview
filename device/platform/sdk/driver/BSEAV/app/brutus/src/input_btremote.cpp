/***************************************************************************
 *     Copyright (c) 2004-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: input_btremote.cpp $
 * $brcm_Revision: 5 $
 * $brcm_Date: 3/16/09 4:48p $
 *
 * Module Description: Brutus input device code
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/input_btremote.cpp $
 * 
 * 5   3/16/09 4:48p randyjew
 * PR42789: updates for BLUETOOTH_SUPPORT
 * 
 * 4   8/15/08 10:08a erickson
 * PR42789: bluetooth update
 *
 * 3   8/13/08 6:44a erickson
 * PR42789: udpate
 *
 * 2   8/13/08 5:26a erickson
 * PR42789: updates for BLUETOOTH_SUPPORT
 *
 * 1   2/13/08 4:54p tokushig
 * PR39534: added bluetooth remote capabilities to brutus
 *
 * SanDiego_Brutus_3D/2   1/30/08 2:20p tokushig
 * refactor demo bluetooth remote implementation for integration into main
 * line
 *
 * SanDiego_Brutus_3D/1   12/20/07 4:59p tokushig
 * added support for bluetooth remote.  note that user must press and hold
 * down <enter> and <start> remote keys before starting brutus and must
 * keep them held down until graphics are displayed on screen - this is
 * so the remote can pair with the bluetooth usb dongle
 *
 * SanDiego_Brutus_BTRemote/1   12/17/07 1:28p randyjew
 * bluetooth remote demo
 *
 * 3   5/9/06 11:40a jrubio
 * PR18491: Had to mod IR Echostar
 *
 * 2   1/16/06 4:31p jrubio
 * PR18491: added function for EchoStar UHF legacy mode remote
 *
 * 1   2/7/05 8:12p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/6   1/19/05 4:59p biyongc
 * PR2807: BSE PR - roll back DSG demo IP video suppport
 *
 * Irvine_BSEAVSW_Devel/4   12/20/04 2:00p erickson
 * PR13604: pull out repeat code for IR remove B before forward on data
 *
 * Irvine_BSEAVSW_Devel/3   11/23/04 9:43a erickson
 * PR13336: reworked repeat key handling to allow more flexibility
 *
 * Irvine_BSEAVSW_Devel/2   4/16/04 11:27a erickson
 * PR9029: convert to BrutusCommand for device-independent command
 *
 * Irvine_BSEAVSW_Devel/1   4/16/04 10:44a erickson
 * PR9029: new object-oriented input device design
 *
 *****************************************************************/
#ifdef BLUETOOTH_SUPPORT
#include "input_btremote.h"
#include "input_devicecodes.h"
#include "inputengine.h"
#include <bstd.h>
#include "easy_bt.h"
#include "bt_hook_priv.h"

BDBG_MODULE(brutus_input_btremote);

unsigned g_data;
static void bluetoothCallback(unsigned data, void *context)
{
    g_data = data;
    ((MIoDevice *)context)->trigger();
}

BluetoothInput::BluetoothInput(MApplication *app, InputEngine *ie) : InputDevice(app, ie, -1)
{
    /* InputDevice constructor does not fully initialize MIoDevice if the
       input device is not a settop api device.  so finish initialization here. */

    /* register callback for bluetooth key events */
#if EASY_BT
    EasyBtSettings btSettings;
    EasyBt_GetSettings(&btSettings);
    btSettings.remote.callback = bluetoothCallback;
    btSettings.remote.callbackContext = &_dev;
    EasyBt_SetSettings(&btSettings);
#else
    BtHookHidSettings btHidSettings; 
    BtHook_GetHidSettings(&btHidSettings); 
    btHidSettings.callback = bluetoothCallback;
    btHidSettings.callbackContext = &_dev; 
    BtHook_SetHidSettings(&btHidSettings);
#endif

    _dev.addHandler(this);
    _dev.start();
}

void BluetoothInput::process(uint32_t data)
{
    BrutusCommand cmd;

    BDBG_MSG(("bt remote: %#x", g_data));
    if (!translateCmd(dev_btSonyPS3, g_data, &cmd))
        _inputEngine->processCmd(cmd, false, false);
}

#endif /* BLUETOOTH_SUPPORT */
