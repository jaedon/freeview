/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: scriptengine.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 11/14/08 6:14p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/scriptengine.h $
 * 
 * 9   11/14/08 6:14p jtna
 * PR45349: resume script cleanly after rejecting cancellation
 * 
 * 8   9/25/08 4:47p mward
 * PR46522:  Use select() to periodically check for termination while
 * waiting for input.
 * 
 * 7   6/27/08 11:37a erickson
 * PR43113: rework scripting so that ; delimited commands are processed
 * just line newline delimited commands
 *
 * 6   2/23/07 12:48p erickson
 * PR27948: don't allow interactive mode "repeat" with no commands to
 * repeat
 *
 * 5   2/1/07 1:36p rjlewis
 * PR27535: fixed "run" command.
 *
 * 4   12/20/05 5:05p rjlewis
 * PR17283: Added support for loop command (nested repeats).
 *
 * 3   10/3/05 11:18a erickson
 * PR17108: fixed linux 2.6 compilation warnings
 *
 * 2   2/23/05 9:32a erickson
 * PR14180: fixed no SCRIPT_SUPPORT on vxworks
 *
 * 1   2/7/05 8:17p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/8   5/5/04 2:45p erickson
 * PR10814: use a static variable to report termination to all apps. quit
 * wasn't working with DUAL_OUTPUT when using -scrtext or -scr. It was
 * only working for --exec where there is a single shared ScriptEngine.
 *
 * Irvine_BSEAVSW_Devel/7   4/16/04 2:49p erickson
 * PR10664: added app(#) command to switch between dual output instances
 * on 97038
 *
 * Irvine_BSEAVSW_Devel/6   3/9/04 10:15a erickson
 * PR9883: close down thread cleanly when exiting script
 *
 * Irvine_BSEAVSW_Devel/5   2/25/04 12:06p erickson
 * PR9703: updated comments
 *
 * Irvine_BSEAVSW_Devel/4   2/4/04 2:15p erickson
 * PR9107: InputEngine and ScriptEngine are now external to Control, which
 * makes the design more modular
 *
 * Irvine_BSEAVSW_Devel/3   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/2   9/22/03 2:33p erickson
 * reworked scripting to implemented prompt command. Now all scripting
 * happens in another thread.
 * This allows sleep to be a real sleep and allows blocking on read(stdin)
 *
 ******************************************************************/
#ifndef SCRIPTENGINE_H__
#define SCRIPTENGINE_H__

#include <mstringlist.h>
#include "inputengine.h"

#ifdef SCRIPT_SUPPORT
class ScriptEnginePrivate;

/**
Summary:
The ScriptEngine executes the scripts and forwards commands on to the InputEngine.

Description:
ScriptEngine can be run in three mutually exclusive modes:
    interactive (command line --exec)
    file (command line -scr)
    single text line (command line -scrtext)

Once it is started in one mode, it cannot be switched.

All the protected instance variables are accessed from a pthread.
Synchronization with the GUI's main thread is handled internally.
*/
class ScriptEngine {
public:
    /**
    Summary:
    Create a new ScriptEngine linked to an InputEngine.
    */
    ScriptEngine(InputEngine *input = NULL);

    ~ScriptEngine();

    /**
    Summary:
    Add another InputEngine which the ScriptEngine can switch between
    */
    void addInputEngine(InputEngine *input);

    /**
    Summary:
    Allow script commands to be entered at runtime using a prompt
    */
    void runInteractiveMode();

    /**
    Summary:
    Process a string as a script
    */
    int runScriptText(const char *text);

    /**
    Summary:
    Read a script file
    */
    int runScript(const char *filename);

    /**
    Summary:
    Check if a script is running, not including interactive mode.
    Description:
    This returns true if a script file or script text is running. When the script
    finishes, or is stopped via promptToStop(), it will return false.
    Interactive mode always returns false and cannot be stopped using promptToStop().
    */
    bool isActive();

    /**
    Summary:
    Returns _interactive
    */
    bool isInteractive();

    /**
    Summary:
    Prompts the user if they want to stop the script. Returns
    true if it was running and was stopped.
    */
    bool promptToStop();

    void setInputEngine(int index);

    /**
    If true, then the "quit" command has been issues and the app should go down.
    This is done with a static variable so that all instances will quit together.
    **/
    static bool appTerminate() {return _appTerminate;}

protected:
    bool _interactive;
    bool _terminate; // terminate the current script
    bool _executingRun; // executing script from interactive mode
    static bool _appTerminate; // the app should go down
    MList<InputEngine> _inputEngines;
    InputEngine *_inputEngine; /* current input engine */
    MApplication *app() const;
    ScriptEnginePrivate *_private;
    MStringList _scriptCmds;
    int _currentScriptCmd;
#define MaxNestedRepeats 20
    struct {
        int count;
        int loops;
    } _scriptRepeats[MaxNestedRepeats];
    int _nestedRepeats;   // depth of nested loop/repeat pairs
    int _processingLoops; // flag -- skipping commands until matching loop command
    int _loopCount;       // count of loop commands from beginning of file
    static void clearLoops(void *data);

    void start();
    void preprocess();
    int addText(const char *text);
    int process(const char *text);
    int processOne(const char *cmd);
    const char *nextScriptCmd();
    static void *scriptfunc(void *data);
    char *terminable_fgets(char * buf, int size, FILE *infile, ScriptEngine * engine);
};
#else
/* vxworks requires some definition for the class */
class ScriptEngine {
};
#endif

#endif //SCRIPTENGINE_H__

