/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: inputengine.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 7/30/10 1:11p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/inputengine.h $
 * 
 * 5   7/30/10 1:11p erickson
 * SW7408-91: add '#' -tty option for scanning channels, remove BCHP_CHIP
 * list
 * 
 * 4   7/27/09 11:13a jtna
 * PR56956: defer script execution until brutus init is complete
 * 
 * 3   5/9/08 11:48a erickson
 * PR42329: script cmd 'repeat' needs to block until script queue empties,
 * otherwise we need filling the queue forever and will run out of
 * memory.
 *
 * 2   10/3/05 11:18a erickson
 * PR17108: fixed linux 2.6 compilation warnings
 *
 * 1   2/7/05 8:13p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/15   11/23/04 9:43a erickson
 * PR13336: reworked repeat key handling to allow more flexibility
 *
 * Irvine_BSEAVSW_Devel/14   4/16/04 11:27a erickson
 * PR9029: convert to BrutusCommand for device-independent command
 *
 * Irvine_BSEAVSW_Devel/13   4/16/04 10:44a erickson
 * PR9029: new object-oriented input device design
 *
 * Irvine_BSEAVSW_Devel/12   2/7/04 1:11a erickson
 * PR9107: -tty is now handled inside brutus
 *
 * Irvine_BSEAVSW_Devel/11   2/4/04 2:15p erickson
 * PR9107: InputEngine and ScriptEngine are now external to Control, which
 * makes the design more modular
 *
 * Irvine_BSEAVSW_Devel/10   10/23/03 4:22p erickson
 * PR8424: reworked interactive thread so that it queues up commands to be
 * run at idle time instead of blocking.
 * this allows it to work with modal dialogs
 *
 * Irvine_BSEAVSW_Devel/9   9/22/03 2:33p erickson
 * reworked scripting to implemented prompt command. Now all scripting
 * happens in another thread.
 * This allows sleep to be a real sleep and allows blocking on read(stdin)
 *
 * Irvine_BSEAVSW_Devel/8   8/29/03 11:01a erickson
 * add 1/2 second delay for repeat keys
 *
 * Irvine_BSEAVSW_Devel/7   8/19/03 11:55a erickson
 * conversion to settop api in progress
 *
 * Irvine_BSEAVSW_Devel/6   8/13/03 10:43a erickson
 * fixed default key processing
 *
 * Irvine_BSEAVSW_Devel/5   7/24/03 6:03p erickson
 * added new interactive mode
 *
 * Irvine_BSEAVSW_Devel/4   7/23/03 4:00p erickson
 * renamed from pvrgui to brutus
 *
 * Irvine_BSEAVSW_Devel/2   4/11/03 2:17p erickson
 * 7115 work
 *
 * Irvine_BSEAVSW_Devel/1   4/9/03 11:21a erickson
 * rework of pvrgui files and class names
 *
 ****************************************************************************/
#ifndef INPUTENGINE_H__
#define INPUTENGINE_H__

#include <mstringlist.h>
#include <mwidget.h>
#include <mtimer.h>
#include "input_devicecodes.h"
class Config;
class Control;
class ScriptEngine;
#include "inputdevice.h"


/**
Summary:
InputEngine manages all user input to Brutus.

Description:
It registers MIoDevice's with MApplication in order to receive external IO.
It is an invisible MWidget which receives the global keyDowns.
It also processes the script queue if there is a script engine running.
**/
class InputEngine : public MWidget, public MTimerEventHandler {
public:
    InputEngine(Control *control);

    /* Run a command immediately. This should only be called when you are in the main thread.
    This is called from onTimeout. */
    int runScriptCmd(const char *cmd);

    /* Queue a script command to be run at idle time. This must be called from the main thread
    but it doesn't actually execute the command, therefore it is quick. */
    int queueScriptCmd(const char *cmd);

    /* return number of script commands currently queued */
    int scriptCmdTotal() const;

    /* onTimeout is called at idle time, which is when script commands are run. */
    void onTimeout(MTimer *timer);

    /* This allows the ScriptEngine to be cross-referenced in order to be interrupted. */
    void set(ScriptEngine *scriptEngine);

    /* If false, don't start scripts from onTimeout. This allows script execution to be delayed until Brutus init is complete */
    bool ready;

    /**
    Called by InputDevice objects.
    **/
    bool processCmd(
        BrutusCommand cmd, /* Device-independent command */
        bool isScriptCmd = false,
        bool isRepeat = false
        );

    Control *control() const {return _control;}

protected:
    MTimer _timer;
    Control *_control;
    ScriptEngine *_scriptEngine;
    MStringList _scriptCmd;
    MAutoList<InputDevice> _devices;

    // override, receives global keyDown's
    bool keyDown(int ascii, int modifiers, int scancode);
};

#endif // INPUTENGINE_H__
