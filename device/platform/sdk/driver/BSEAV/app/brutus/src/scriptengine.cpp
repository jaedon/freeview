/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: scriptengine.cpp $
 * $brcm_Revision: 24 $
 * $brcm_Date: 11/14/08 6:14p $
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/scriptengine.cpp $
 * 
 * 24   11/14/08 6:14p jtna
 * PR45349: resume script cleanly after rejecting cancellation
 * 
 * 23   11/5/08 7:30p katrep
 * PR47611: Fixed crash due to # chanracter in the command
 * 
 * 22   10/3/08 3:34p rjlewis
 * PR46522: don't use select for VxWorks.
 * 
 * 21   9/25/08 4:46p mward
 * PR46522:  Use select() to periodically check for termination while
 * waiting for input.
 * 
 * 20   9/9/08 2:02p mward
 * PR46522:  Indicate that the thread should quit.
 * 
 * 19   8/1/08 11:01a erickson
 * PR45286: fix crash if user input interrupts script, converted printf's
 * to DBG
 *
 * 18   6/27/08 11:37a erickson
 * PR43113: rework scripting so that ; delimited commands are processed
 * just line newline delimited commands
 *
 * 17   5/9/08 11:48a erickson
 * PR42329: script cmd 'repeat' needs to block until script queue empties,
 * otherwise we need filling the queue forever and will run out of
 * memory.
 *
 * 16   5/6/08 5:08p jgarrett
 * PR 42024: Fixing leaked scriptengine thread
 *
 * 15   2/23/07 12:48p erickson
 * PR27948: don't allow interactive mode "repeat" with no commands to
 * repeat
 *
 * 14   2/1/07 1:36p rjlewis
 * PR27535: Fixed the "run" command. Delete the private semaphore on
 * destruct. Sleep defaults to 1  if non-number supplied.
 *
 * 13   12/29/06 4:43p erickson
 * PR26813: b_start_task now requires b_task_params, which should be
 * initialized with b_task_params_init
 *
 * 12   12/8/06 1:10p gmohile
 * PR 26059: Support backspace in --exec
 *
 * 11   5/10/06 4:45p rjlewis
 * PR21414: print error when -scr script file can't be found.
 *
 * 9   12/20/05 5:04p rjlewis
 * PR17283: Add support for loop command (nested repeat loops). Handle
 * whitespace better (more relaxed formatting).
 *
 * 8   5/23/05 12:10p erickson
 * PR15067: extend b_start_task to take optional parameters, required for
 * vxworks
 *
 * 7   4/27/05 1:15p erickson
 * PR15075: use int64_t for 64 bit file offset instead of assuming off_t
 * is 64 bits (it's not on vxworks)
 *
 * 6   4/11/05 3:22p erickson
 * PR14701: protect more of ScriptEngine state, and short circuit
 * process() is terminating
 *
 * 5   4/7/05 5:12p erickson
 * PR14708: made script threading generic
 *
 * 4   3/22/05 10:51a mphillip
 * PR14551: Change sleep from milliseconds to seconds by multiplying by
 * 1000
 *
 * 3   2/23/05 9:32a erickson
 * PR14180: fixed no SCRIPT_SUPPORT on vxworks
 *
 * 2   2/12/05 11:12a dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/24   2/11/05 5:05p bandrews
 * PR14123: Fixed
 *
 * Irvine_BSEAVSW_Devel/23   8/23/04 11:41a erickson
 * PR11643: return true even if script not stopped because the key has
 * been consumed
 *
 * Irvine_BSEAVSW_Devel/22   7/26/04 7:15p liangl
 * PR11643: merging brutus vxworks support
 *
 * Irvine_BSEAVSW_Devel/21   7/23/04 3:37p erickson
 * PR11751: added sleep to make Brutus> prompt placement better
 *
 * Irvine_BSEAVSW_Devel/20   7/23/04 2:28p erickson
 * PR11869: allow brutus to be restarted without app going down
 *
 * Irvine_BSEAVSW_Devel/Brutus_Vxworks_PreRelease/2   7/7/04 3:11p liangl
 * PR11713: Channel change in App B (SD output) in dual decode causes V/A
 * disppear
 *
 * Irvine_BSEAVSW_Devel/Brutus_Vxworks_PreRelease/1   6/17/04 3:26p liangl
 * added script engine for support
 *
 * Irvine_BSEAVSW_Devel/19   6/30/04 11:45a erickson
 * PR11751: set default value
 *
 * Irvine_BSEAVSW_Devel/18   5/27/04 9:56a erickson
 * PR11121: need to short-circuit process() function if repeat or quit is
 * issued
 *
 * Irvine_BSEAVSW_Devel/17   5/5/04 2:45p erickson
 * PR10814: use a static variable to report termination to all apps. quit
 * wasn't working with DUAL_OUTPUT when using -scrtext or -scr. It was
 * only working for --exec where there is a single shared ScriptEngine.
 *
 * Irvine_BSEAVSW_Devel/16   4/30/04 1:57p erickson
 * PR10885: the repeatScriptCount wasn't getting reset when starting a new
 * script
 *
 * Irvine_BSEAVSW_Devel/15   4/26/04 5:15p erickson
 * PR10814: terminate all apps, not just current
 *
 * Irvine_BSEAVSW_Devel/14   4/16/04 2:49p erickson
 * PR10664: added app(#) command to switch between dual output instances
 * on 97038
 *
 * Irvine_BSEAVSW_Devel/13   3/19/04 5:28p erickson
 * PR9883: if thread terminates itself, then we still need to join to
 * cleanup
 *
 ******************************************************************/
#include <stdio.h>
#ifndef __vxworks
#include <sys/select.h>
#include <fcntl.h>
#endif

#include <mapplication.h>
#include "inputengine.h"
#include "scriptengine.h"
#include <mmessagebox.h>
#include <unistd.h>
#include <errno.h>
#include <bstd.h>
#include <bkni.h>
#include <bkni_multi.h>
#include <bsettop_os.h>

BDBG_MODULE(brutus_scriptengine);

#ifdef READLINE_SUPPORT
extern "C" {
#include <readline/readline.h>
#include <readline/history.h>
}
extern HIST_ENTRY **history_list();
#endif

/* ScriptEngine uses a thread for a couple reasons:
1) so it can block on fgets in interactive mode and for the prompt command
2) so it can sleep

An alternative is to add stdin as a bwin_io_handle, then we'll be notified when something is
present. This means not using fgets, but writing our own. This also means
measuring sleep time during idletime callbacks.

Another alternative, below, is fgets that blocks, but periodically checks for termination.
*/

#ifndef __vxworks
char * ScriptEngine::terminable_fgets(char * buf, int n, FILE *infile, ScriptEngine * engine)
{
    fd_set rfds;
    struct timeval tv = {0,0};
    int retval;

    if ((n<=0) || !infile || !engine) {
        return NULL;
    }
    fcntl(fileno(infile),F_SETFD,O_NONBLOCK);  /* set to non-blocking input */

    while (!engine->_terminate)
    {
        FD_ZERO(&rfds);
        FD_SET(fileno(infile), &rfds);
        retval = select(1, &rfds, NULL, NULL, &tv);
        if (retval < 0) {
            return NULL;
        }
        else if (retval > 0) {
            return fgets(buf,n,infile);
        }
        tv.tv_sec = 1;    /* Check _terminate once per second */
        tv.tv_usec = 0;
    }
    return NULL;
}
#endif

class ScriptEnginePrivate {
public:
    b_task_t thread;
    BKNI_MutexHandle mutex;
};

void ScriptEngine::clearLoops(void *data)
{
    ScriptEngine *engine = (ScriptEngine *)data;
    engine->_processingLoops = 0;
    engine->_loopCount = 0;
    engine->_nestedRepeats = 0;
    for (int i=0; i<MaxNestedRepeats; i++) {
        engine->_scriptRepeats[i].count = -1;
        engine->_scriptRepeats[i].loops = 0;
    }
}

void *ScriptEngine::scriptfunc(void *data)
{
    ScriptEngine *engine = (ScriptEngine *)data;

    BDBG_MSG(("starting thread"));

    while (!engine->_terminate) /* atomic read needs no protection */
    {
        const char *buf;

        buf = engine->nextScriptCmd();

        if (!buf && engine->_interactive)
        {
            clearLoops(engine);

            /* Let the scheduler run so that the next Brutus> prompt is
               more likely to appear after the new output. */
            BKNI_Sleep(1);

#ifdef READLINE_SUPPORT
            buf = (const char *)readline("Brutus>");
            if (!buf)
                break;
            if (*buf)
                add_history(buf);
            BDBG_MSG(("Processing '%s'", buf));
#else
            char buffer[256]; int len;
            printf("Brutus>"); fflush(stdout);
#ifdef __vxworks
            buf = fgets(buffer, 256, stdin);
#else
            buf = engine->terminable_fgets(buffer, 256, stdin, engine);
#endif
            if (!buf) {
                BDBG_MSG(("fgets failed"));
                break;
            }
            len = strlen(buffer);
            if (len) buffer[len-1] = 0;

            if (strstr(buffer, "\b")) {
              BDBG_WRN(("BACKSPACE CHARACTER SENT BY TERMINAL IS ASCII 0x8."));
              BDBG_WRN(("ENABLE TERMINAL TO SENT ASCII 0x7 AS BACKSPACE."));
              BDBG_WRN(("ELSE USE 'DEL' KEY TO DELETE CHARACTER."));
            }
            BDBG_MSG(("Processing '%s'", buf));
#endif
            /* This will add entries to the ScriptCmd list which will get processed on the next time round. */
            engine->addText(buf);
        }
        else {
            if (!buf)
                break;
            if (engine->process(buf))
                break;
        }
    }

    BDBG_MSG(("thread done"));
    return NULL;
}

ScriptEngine::ScriptEngine(InputEngine *inputEngine)
{
    _appTerminate = false;
    _currentScriptCmd = -1;
    _inputEngine = NULL;
    _executingRun = false;

    clearLoops(this);

    if (inputEngine)
        addInputEngine(inputEngine);

    _interactive = false;

    _private = new ScriptEnginePrivate;
    _private->thread = 0;
    BKNI_CreateMutex(&_private->mutex);
}

ScriptEngine::~ScriptEngine()
{
    _inputEngine->set(NULL);
    if ( _private->thread )
    {
        _terminate = true; /* tell the thread to quit */
        b_stop_task(_private->thread);
    }
    BKNI_DestroyMutex(_private->mutex);
    delete _private;
}

void ScriptEngine::addInputEngine(InputEngine *inputEngine)
{
    _inputEngines.add(inputEngine);

    if (!_inputEngine)
        setInputEngine(0);
}

void ScriptEngine::setInputEngine(int index)
{
    if (_inputEngine)
        _inputEngine->set(NULL); /* unlink */

    _inputEngine = _inputEngines.at(index);

    if (_inputEngine)
        _inputEngine->set(this); /* cross reference them. we need to know if the script is interrupted. */
}

void ScriptEngine::start()
{
    int i;
    b_task_params task_params;

    clearLoops(this);

    b_task_params_init(&task_params);
    task_params.priority = 100 /*hint: lowest*/;
    task_params.name = "scriptengine";

    /* The thread is non-null, and we're starting, then we need to join first. */
    if (_private->thread) {
        _terminate = true; /* stop the current thread */
        BDBG_MSG(("stopping private thread!"));
        b_stop_task(_private->thread);
        _private->thread = 0;
    }

    _terminate = false; /* enable new thread */
    BDBG_MSG(("creating script processing task!"));
    b_start_task(&_private->thread, &task_params, (b_task_func)scriptfunc, (void *)this);
}

void ScriptEngine::runInteractiveMode()
{
    /* When you start interactive, you can never stop it. Restart the app
    if you want to not be interactive anymore. */
    _interactive = true;
    start();
}

int ScriptEngine::runScript(const char *filename)
{
    _scriptCmds.clear();
    _currentScriptCmd = -1;
    if (!readFile(filename, _scriptCmds)) {
        BDBG_ERR(("Failed to open and process script file: '%s'", filename));
        return errno;
    }
    _currentScriptCmd = 0;
    preprocess();
    start();
    return 0;
}

int ScriptEngine::addText(const char *text)
{
    BKNI_AcquireMutex(_private->mutex);
    _scriptCmds.clear();
    _currentScriptCmd = 0;
    _scriptCmds.add(text);
    preprocess();
    BKNI_ReleaseMutex(_private->mutex);
    return 0;
}

int ScriptEngine::runScriptText(const char *text)
{
    addText(text);
    start();
    return 0;
}

/* walk through _scriptCmds and get ready to run */
void ScriptEngine::preprocess()
{
    int i;

    /* break all ; delimited commands into separate lines so that the line-processing logic is simple */
    for (i=0;i<_scriptCmds.total();i++) {
        const char *s = _scriptCmds[i];

        s += strspn(s, " \t");
        if (*s == '#') {
            s = "";
        }
        if (!*s) {
            _scriptCmds.remove(i);
            i--;
            continue;
        }

        _scriptCmds[i] = s;

        const char *delim = strchr(s, ';');
        if (delim) {
            MString s1(s, delim-s);
            MString s2(delim+1);
            _scriptCmds.insert(i, s1);
            _scriptCmds[i+1] = s2;
            i--;
        }
    }
#if 0
    for (i=0;i<_scriptCmds.total();i++) {
        BDBG_WRN(("script[%d]: %s", i, (const char *)_scriptCmds[i]));
    }
#endif
}

int ScriptEngine::process(const char *text)
{
    BKNI_AcquireMutex(_private->mutex);

    /* verify that preprocess was called */
    BDBG_ASSERT(!strchr(text, ';'));
    /* we do not really remove #,# in the command causes assert */
    /* BDBG_ASSERT(!strchr(text, '#')); */
    processOne(text);

    BKNI_ReleaseMutex(_private->mutex);
    return 0;
}

const char *ScriptEngine::nextScriptCmd()
{
    BKNI_AcquireMutex(_private->mutex);

    if (_terminate) {
        BKNI_ReleaseMutex(_private->mutex);
        return NULL;
    }

    const char *cmd = _scriptCmds.at(_currentScriptCmd);
    if (!cmd)
        _currentScriptCmd = -1; // eof
    else
        _currentScriptCmd++;

    BKNI_ReleaseMutex(_private->mutex);
    return cmd;
}

#include <ctype.h> // for isspace

/* Return non-zero if the process() function should short-circuit and stop immediately.
This is used for app termination and "repeat".
*/
int ScriptEngine::processOne(const char *cmd)
{
    static bool echo = 0;
    BDBG_MSG(("script: '%s'", cmd));

    // remove line comment
    char *comment = strchr(cmd, '#');
    if (comment) *comment = 0;
    // skip leading whitespace
    cmd += strspn(cmd, " \t");
    // if after that we have nothing, be done
    if (!*cmd) return 0;
    // strip trailing whitespace
    char *c=(char*)cmd; for (char *x=(char*)cmd; *x; x++) if (!isspace(*x)) c=x; if (c[1]) c[1]=0;

    // Putting a "loop" in the script sets a beginning position for repeats.
    // The repeats should match to the corresponding loop.
    if (strstr(cmd, "loop") == cmd)
    {
        _loopCount++; // this counts the loops from the beginning of the file.

        // If I'm not currently processing a repeat loop, then this is a new (nested?) loop/repeat pair.
        // Keep count of the nested loop/repeats as they have individual repeat counts.
        if (!_processingLoops) {
            if (++_nestedRepeats >= MaxNestedRepeats) {
                BDBG_ERR(("script exceeds maximum nested loops (%d) -- cannot continue!", MaxNestedRepeats));
                _appTerminate = true;
                _terminate = true;
                return 1;
            }
            _scriptRepeats[_nestedRepeats].count = -1; // assume forever loop (update when we see repeat command)
            _scriptRepeats[_nestedRepeats].loops = _loopCount;
        }
        // If processing a loop we skip evey command untill we get back to the 'loop' command that started the loop.
        if (_processingLoops && _loopCount >= _processingLoops)
            _processingLoops = 0;
        return 0;
    }

    // We skip over every command until we find the loop command that started this repeat.
    if (_processingLoops) {
        if (echo) printf("-----> Skipping over command (processing=%d, count=%d, nested=%d): %s\n",
            _processingLoops, _loopCount, _nestedRepeats, cmd);
        return 0;
    }

    // We're not or no longer processing a loop -- we're going to execute this command!
    if (echo) printf("-----> Processing command (loop=%d, nested=%d, repeat=%d): '%s'\n",
             _loopCount, _nestedRepeats, _scriptRepeats[_nestedRepeats].count, cmd);

    // A repeat command specifies a block to "do again".
    // A repeat should have a corresponding "loop" command (to specify the extent of the repeat loop, but doesn't have to.
    // A repeat without a matching loop is considered a "dangling" repeat.  When found, it repeats from the beginning.
    // Multiple dangling repeats will create a "forever loop" in that the second dangling repeat will never exit (even if it has a count).
    // Note: the repeat count (optional) indicate the number of times to do the block AGAIN (not how many times it gets executed).
    if (strstr(cmd, "repeat") == cmd)
    {
        if (_currentScriptCmd == 0) {
            BDBG_ERR(("No commands to repeat. Short circuiting infinite loop."));
            BDBG_ERR(("Be aware that interactive scripting can only repeat commands on a single line, separated with ;"));
            return 0;
        }

        /* block until the script queue goes to zero. otherwise the queue will fill endlessly and run out of memory. */
        while (1) {
            BKNI_ReleaseMutex(_private->mutex);
            app()->acquireIdleLock();
            int total = _inputEngine->scriptCmdTotal();
            app()->releaseIdleLock();
            BKNI_AcquireMutex(_private->mutex);
            if (!total)
                break;
            else
                BKNI_Sleep(1);
        }

        // -1 means uninitialized or forever
        // if it's -1 here, then this is a new repeat command or we're looping forever
        if (_scriptRepeats[_nestedRepeats].count == -1) {
            cmd += 6; cmd += strspn(cmd, "( \t");
            if (*cmd) // if there's something, then get the initial repeat count
                _scriptRepeats[_nestedRepeats].count = atoi(cmd);
            if (_scriptRepeats[_nestedRepeats].count == 0)
                _scriptRepeats[_nestedRepeats].count = -1;
        }
        // decrement the counter if initialized and not done
        if (_scriptRepeats[_nestedRepeats].count == -1 || _scriptRepeats[_nestedRepeats].count > 0) {
            if (_scriptRepeats[_nestedRepeats].count != -1)
                _scriptRepeats[_nestedRepeats].count--;

            _currentScriptCmd = 0; // go to the beginning of the script
            _processingLoops = _scriptRepeats[_nestedRepeats].loops;
            _loopCount = 0;

            if (echo) printf("=====> Repeat starting at loop count = %d, nested = %d, repeat count = %d\n",
                     _processingLoops, _nestedRepeats, _scriptRepeats[_nestedRepeats].count);

            // Special case a dangling repeat (repeat without matching loop) -- it starts processing at beginning of file!
            if (!_nestedRepeats) {
                if (echo) printf("=====> Found a dangling repeat -- looping back to beginning (restart)\n");
                _processingLoops = 0;
            }

            return 1; // process() should short-circuit
        }

        // We get here when we've exhausted a loop count -- reset for next one (if any).
        _scriptRepeats[_nestedRepeats].count = -1;
        if (_nestedRepeats) _nestedRepeats--;

        if (echo) printf("=====> Completed repeat loop (nested=%d, RepeatCount=%d)\n",
                 _nestedRepeats, _scriptRepeats[_nestedRepeats].count);
        return 0;
    }

    if (strstr(cmd, "prompt") == cmd) {
        cmd += 6; cmd += strspn(cmd, " \t");
        if (*cmd == '(') {
            const char *text = cmd + 1;
            const char *endoftext = strchr(text, ')');
            if (!endoftext) return 0;
            puts(MString(text, endoftext-text));
        }
        char buf[10];
        printf(":"); fflush(stdout);
#ifdef __vxworks
        fgets(buf, 10, stdin);
#else
        terminable_fgets(buf, 10, stdin,this);
#endif
    }
    else if (strstr(cmd, "echo") == cmd) {
        cmd += 4; cmd += strspn(cmd, "( \t");
        if (strstr(cmd, "on")  || strstr(cmd, "1")) echo = true;
        if (strstr(cmd, "off") || strstr(cmd, "0")) echo = false;
    }
    else if (strstr(cmd, "sleep") == cmd) {
        cmd += 5; cmd += strspn(cmd, "( \t");
        if (*cmd) {// if there's something, then get the sleep count
            int sleepFor = atoi(cmd)*1000;

            BKNI_ReleaseMutex(_private->mutex);
            BKNI_Sleep(sleepFor?sleepFor:1); /* handle sleep 0 as sleep 1 */
            BKNI_AcquireMutex(_private->mutex);
        }
    }
    else if (strstr(cmd, "app") == cmd) {
        cmd += 3; cmd += strspn(cmd, "( \t");
        int app = atoi(cmd);
        BDBG_WRN(("Selecting app %d", app));
        setInputEngine(app);
    } else if (strstr(cmd, "run") == cmd) {
        const char *filename = cmd + 3;
        filename += strspn(filename, " \t");
        if (!_interactive)
        {
            BDBG_ERR(("Cannot use the 'run' command from a script -- ignoring command!"));
        } else {
            _currentScriptCmd = -1;
            _scriptCmds.clear();

            if (!readFile(filename, _scriptCmds)) {
                BDBG_ERR(("Failed to open and process script file: '%s'", filename));
            } else {
                _currentScriptCmd = 0;
                preprocess();
                /* We want to execute a script from interactive mode (--exec) AND come
                ** back when the script finishes or is terminated.  Instead of using another
                ** thread for this, we'll fake the conditions that says we're not in interactive
                ** mode, execute the script, and return the state back to interactive mode.
                ** This is a trick in that we're inside the function 'scriptfunc' and need to
                ** re-enter it in order to execute the script commands.
                **
                ** We need to know we're performing a 'run' or else hitting a key on the
                ** remote will ask if they want to terminate AND, if they do, then we would
                ** kill the interactive thread.  The 'executingRun' prevents that from happening.
                */
                _executingRun = true;
                _interactive = false;
                BKNI_ReleaseMutex(_private->mutex);
                BDBG_MSG(("Using the run command to execute script: '%s'", filename));
                scriptfunc(this);
                BDBG_MSG(("Back - done executing script: '%s'", filename));
                BKNI_AcquireMutex(_private->mutex);
                _interactive = true;
                _executingRun = false;

                _terminate = false; /* if they terminated the script then reset */
            }
        }
    }
    else if (!strcmp(cmd, "quit")) {
//      fclose(stdin); // this allows interactive_mode to quit

        /* set a static variable. we're relying on InputEngine to read this
        and kill the app. this is necessary so that a quit in one app instance
        will kill all of them. */
        _appTerminate = true;
        _terminate = true;
        return 1;
    }
    else if (_inputEngine) {
        /* If it's not a script execution command, then pass it into the InputEngine.
        sync up with GUI thread first. */
        BKNI_ReleaseMutex(_private->mutex);
        app()->acquireIdleLock();
        _inputEngine->queueScriptCmd(cmd);
        app()->releaseIdleLock();
        BKNI_AcquireMutex(_private->mutex);
    }
    else {
        BDBG_ERR(("Ignoring unknown command found in script ('%s')", cmd));
    }

    return 0;
}

bool ScriptEngine::isActive()
{
    return _currentScriptCmd != -1 && !_interactive && _inputEngine;
}

bool ScriptEngine::isInteractive()
{
    return _interactive;
}

bool ScriptEngine::promptToStop()
{
    int remember;

    if (!isActive()) return false;

    BDBG_ASSERT(_private->thread);

    /* don't allow the script to run while prompting to stop it */
    BKNI_AcquireMutex(_private->mutex);

    /* The running script is now blocked on _mutex. We're free to ask about
    it without interference in the GUI. */

    remember = _currentScriptCmd;
    _currentScriptCmd = -1;

    if (MMessageBox::show(_inputEngine->fb(), "Stop the script?",
        MMessageBox::bYes|MMessageBox::bNo,MMessageBox::bYes, 10*1000) == MMessageBox::bYes)
    {
        /* Stop the thread */
        _terminate = true;
        BKNI_ReleaseMutex(_private->mutex);
        if (!_executingRun) {
            b_stop_task(_private->thread);
            _private->thread = 0;
            _currentScriptCmd = -1;
        }
        return true;
    }

    _currentScriptCmd = remember;
    BKNI_ReleaseMutex(_private->mutex);
    return true;
}

MApplication *ScriptEngine::app() const {
    return _inputEngine?_inputEngine->app():NULL;
}

bool ScriptEngine::_appTerminate = false; // the app should go down

