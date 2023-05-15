/***************************************************************************
 *     Copyright (c) 2004-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: inputdevice.cpp $
 * $brcm_Revision: 3 $
 * $brcm_Date: 6/26/09 11:38a $
 * $brcm_Revision: 3 $
 * $brcm_Date: 6/26/09 11:38a $
 *
 * Module Description: Brutus input device code
 *
 * Revision History:
 *
 *
 * $brcm_Log: /BSEAV/app/brutus/src/inputdevice.cpp $
 * 
 * 3   6/26/09 11:38a erickson
 * PR56373: remove unused feature
 *
 * 2   2/17/05 2:45p ywu
 * PR 13748: Create an IR keystrokes automation utility
 *
 * 1   2/7/05 8:13p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/3   7/26/04 7:03p liangl
 * PR11643: merging brutus vxworks support
 *
 * Irvine_BSEAVSW_Devel/Brutus_Vxworks_PreRelease/1   6/17/04 12:51p liangl
 * added support for vxWorks
 *
 * Irvine_BSEAVSW_Devel/2   7/6/04 3:56p erickson
 * PR11771: settop api dataflow redesign
 *
 * Irvine_BSEAVSW_Devel/1   4/16/04 10:44a erickson
 * PR9029: new object-oriented input device design
 *
 *****************************************************************/
#include "inputdevice.h"
#include <bstd.h>
BDBG_MODULE(brutus_inputdevice);

void user_callback(void *context)
{
    /* This may be called from quasi-ISR context. Do minimal work. */
    BDBG_MSG(("user_callback"));
    ((MIoDevice *)context)->trigger();
}

InputDevice::InputDevice(MApplication *app, InputEngine *ie, int settopApiId) : _dev(app)
{
    buser_input_settings settings;

    _inputEngine = ie;

    /* Check if this is not a settop api device. If so, they can set
    it up manually. */
    if (settopApiId == -1) {
        _ui = NULL;
        return;
    }

    _ui = buser_input_open(B_ID(settopApiId));

    if (!_ui)
        return;

    buser_input_get_settings(_ui, &settings);
    settings.data_ready_callback = user_callback;
    settings.callback_context = &_dev;
    buser_input_set_settings(_ui, &settings);

    _dev.addHandler(this);
    _dev.start();
}

InputDevice::~InputDevice()
{
    _dev.stop();
    if (_ui)
        buser_input_close(_ui);
}

void InputDevice::onReady(MIoDevice *dev)
{
    buser_input_event data;
    unsigned total;

    if (!_ui) {
        process(0); /* subclass will handle it manually */
        return;
    }

    while (!buser_input_get_event(_ui, &data, 1, &total) && total) {
        BDBG_MSG(("buser_input_get_event returned %x", data.code));
        process(data.code);
    }
}
