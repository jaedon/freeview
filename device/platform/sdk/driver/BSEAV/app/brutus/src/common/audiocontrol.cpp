/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reservjed
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: audiocontrol.cpp $
 * $brcm_Revision: 22 $
 * $brcm_Date: 4/2/09 12:36p $
 *
 * Module Description: PCM playback control
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/common/audiocontrol.cpp $
 * 
 * 22   4/2/09 12:36p erickson
 * PR42430: fix AudioControl memory leak
 *
 * 21   5/5/08 1:43a jrubio
 * PR42430: check pcm handle before you try to close it
 *
 * 20   4/10/08 6:42p jgarrett
 * PR 41620: MP3 playback crashes after stop
 *
 * 19   3/17/08 3:18p erickson
 * PR40307: remove old, unused code before adding new
 *
 * 18   2/5/08 4:35p jrubio
 * PR39363: fix warnings with ne w toolchain
 *
 * 17   11/7/07 3:42p jrubio
 * PR35015: include linux/limits.h for linux
 *
 * 16   6/4/07 3:37p jgarrett
 * PR 22414: Converting 7400 to madplay for all revs
 *
 * 15   12/29/06 4:43p erickson
 * PR26813: b_start_task now requires b_task_params, which should be
 * initialized with b_task_params_init
 *
 * 14   11/4/06 4:11p dlwin
 * PR 25457:MP3 Playback results in a seg-fault on 97398 platform
 *
 * 13   6/30/06 3:10p jgarrett
 * PR 20139: Removing compiler warning
 *
 * 12   5/25/06 2:48p jgarrett
 * PR 21764: Switching from mpg123 to madplay for 7400A0.  Also searches
 * PATH for application.
 *
 * 11   3/15/06 6:12p vle
 * PR 20235: Ensure bpcm_play_open is called with different B_ID in dual
 * output mode. Otherwise, kernel oops occurred since pcmplay will be
 * auto closed by handle mananger.
 *
 * 10   2/22/06 5:02p jrubio
 * PR19756: removed extra call to clear interrupt in the app.
 *
 * 9   1/10/06 3:35p jrubio
 * PR18606: Fix pcm play issue, start stop start stop play sequence should
 * work now. Added destructor.
 *
 * 8   12/14/05 9:43a erickson
 * PR18606: added MIX_PCM_WITH_DECODE cfg option
 *
 * 7   12/7/05 2:09p erickson
 * PR17108: added 7400 mpg123 and 7401 madplay, and fixed madplay to
 * stdout code
 *
 * 6   9/8/05 10:01a erickson
 * PR17048: switch individual chip defines to B_HAS_VDC for all 7038 and
 * beyond platforms
 *
 * 5   8/24/05 6:12p arbisman
 * PR16678: Add support for WinCE
 *
 * 4   7/18/05 12:08p erickson
 * PR15793: fixed for legacy, replaced pthread with b_start/stop_task
 *
 * 3   6/30/05 8:20a erickson
 * PR15793: made pcmthread support more generic, and disabld 3560 support
 * which has multiple problems
 *
 * 2   3/24/05 1:21p erickson
 * PR14593: removed old platform
 *
 * 1   2/7/05 8:03p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/15   11/2/04 3:10p erickson
 * PR10763: switched 7038 to mpg123
 *
 * Irvine_BSEAVSW_Devel/14   10/26/04 5:05p erickson
 * PR10763: added user-mode PCM playback support (enabling MP3 decode on
 * 97038 and others)
 *
 * Irvine_BSEAVSW_Devel/13   9/15/04 2:41p erickson
 * PR9683: use explicity _FLAVOR or _FAMILY instead of BCM7### defines
 *
 * Irvine_BSEAVSW_Devel/12   9/15/04 1:53p erickson
 * PR9683: convert from B_CHIP to BCHP_CHIP in brutus
 *
 * Irvine_BSEAVSW_Devel/11   7/14/04 1:36a erickson
 * PR11287: refactor linux-specific code into brutus_linux.cpp for easier
 * porting
 *
 * Irvine_BSEAVSW_Devel/10   6/14/04 12:53p erickson
 * PR10701: reenable audio if mp3 decode fails
 *
 * Irvine_BSEAVSW_Devel/9   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 ***************************************************************************/
#include "audiocontrol.h"
#include "cfgsettings.h"
#include <murl.h>

#ifndef _WIN32_WCE
    #include <unistd.h>
    #include <errno.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <linux/limits.h>
#endif
#include "mmessagebox.h"
#include "bstd.h"
BDBG_MODULE(audiocontrol);

extern char **environ; // needed for execve, see man page

/**
AudioControl allows MP3 playback in brutus using an external MP3
decode app (e.g. mpg123 or madplay). A process is forked and the
app executed.
**/

/* When running user mode (magnum-based platforms) Settop API, we can't use a /dev/dsp node.
Therefore we start the MP3 decoder in a process and redirect stdout
back to our application. Then we start a thread which reads that
data and sends to the bpcm_play interface.
**/
#ifndef _WIN32_WCE
    #define PCMPLAY_THREAD_SUPPORT
#endif

AudioControl::AudioControl(Control *control)
{
    _control = control;
    _audioPid = 0;
    _task = 0;
#ifdef PCMPLAY_THREAD_SUPPORT
    _pcm_play = bpcm_play_open(B_ID(control->secondDisplay()));
    if (!_pcm_play) {
        _pcm_play = NULL;
    }
    if ( BKNI_CreateEvent(&_taskEvent) )
    {
        _taskEvent = NULL;
    }
    if ( BKNI_CreateEvent(&_stoppedEvent) )
    {
        _stoppedEvent = NULL;
    }
#endif
}

AudioControl::~AudioControl()
{
#ifdef PCMPLAY_THREAD_SUPPORT
    if(_pcm_play) {
        bpcm_play_close(_pcm_play);
    }
    if (_taskEvent) {
        BKNI_DestroyEvent(_taskEvent);
    }
    if (_stoppedEvent) {
        BKNI_DestroyEvent(_stoppedEvent);
    }
#endif
}

bool AudioControl::playAudio(MString file) {
    MStringList list;
    list.add(file);
    return playAudio(list);
}

bool AudioControl::playAudio(MStringList &list) {
    int audioapptype;
    int needToSleep = 0;
    char audioapppath[PATH_MAX];
    const char *audioapp;
    const char *path;
    const char *curpath;
    bool disabledAudio = false;

#if BCHP_CHIP == 3560
    /* Multiple reasons for this:
    1. probably not enough horsepower
    2. madplay (required for fixed point) has no stdout feature, so we'd have to hack it to make the redirect
    work.
    3. not required for TV applications
    */
    MMessageBox::info(_control->fb(), "MP3 decode not supported on 3560.");
    return false;
#endif

    // only disableAudio if PCM wasn't playing
    if (!stopAudio(false)) {
        if (!control()->cfg()->getBool(Config::MIX_PCM_WITH_DECODE)) {
            control()->disableAudio();
            disabledAudio = true;
        }
    }
    else {
        needToSleep = 1;
    }

#if defined BCM732X_FAMILY || BCHP_CHIP == 7038
    // fpu systems
    audioapptype = 0; // mpg123, which can handle http:
#else
    if (strstr(list[0], "http:") == list[0])
        audioapptype = 2; // webclient | madplay (because madplay can't handle http)
    else
        audioapptype = 1; // madplay
#endif
    switch (audioapptype) {
    case 0:
        audioapp = "mpg123";
        break;
    case 1:
    case 2:
        audioapp = "madplay";
        break;
    default:
        assert(audioapptype < 3);
        return false;
    }

    /* check if the app exists before forking. It may not because these apps are
    GPL'd and must be manually installed. */
    /* Search PATH for madplay/mpg123 */
    path = getenv("PATH");

    if ( NULL == path )
        path="/bin";

    while ( path )
    {
        int string_len;
        const char *next_path = strchr(path, ':');
        if ( NULL == next_path )
            string_len = strlen(path);
        else
        {
            string_len = next_path - path;
            next_path++;    /* skip ':' */
        }

        strncpy(audioapppath,path,string_len);
        sprintf(audioapppath+string_len,"/%s",audioapp);

        path = next_path;

        struct stat st;

        BDBG_MSG(("Looking for app %s", audioapppath));

        if ( 0 == stat(audioapppath, &st))
            goto found;
    }

    /* If we reach here, we didn't find the app */
    MMessageBox::info(_control->fb(), "Unable to find MP3 decode application. This must be installed manually.");
    goto error;

found:
    /* Found the app, continue */

    BDBG_MSG(("Found app %s", audioapppath));

    _pipe[0] = -1;
#ifdef PCMPLAY_THREAD_SUPPORT
    pipe(_pipe);
#endif

    _audioPid = fork();
    if (_audioPid == -1) {
        MMessageBox::info(_control->fb(), "Unable to start MP3 decode application.");
        goto error;
    }
    else if (_audioPid == 0) {
        // child
        char *argv[10];
        int index = 0;

        char *audionode;

#if BCHP_CHIP == 7115 || BCHP_CHIP == 7110
        audionode = "/dev/bcmpcm_1";
#else
        if (!_control->secondDisplay())
            audionode = (char *)"/dev/bcmpcm_0";
        else
            audionode = (char *) "/dev/bcmpcm_1";
#endif

        if (_pipe[0] != -1) {
#ifdef PCMPLAY_THREAD_SUPPORT
            /* replace stdout with pipe */
            close(_pipe[0]); // close read
            close(1); // close stdout
            dup2(_pipe[1], 1); // replace stdout with our pipe
#endif
        }

        switch (audioapptype) {
        case 0: /* mpg123 */
            argv[index++] = (char *)audioapppath;
            argv[index++] = (char *) "-q";
            if (_pipe[0] != -1) {
                argv[index++] = (char *)"-s"; /* redirect to stdout */
            }
            else {
                argv[index++] = (char *) "-a";
                argv[index++] = audionode;
            }
            break;
        case 1: /* madplay */
            argv[index++] = (char *)audioapppath;
            argv[index++] = (char *)"-q";
            argv[index++] = (char *)"-o";
            if (_pipe[0] != -1) {
                argv[index++] = (char *)"raw:-"; /* redirect to stdout */
            }
            else {
                argv[index++] = audionode;
            }
            break;
        case 2: /* madplay over webclient http */
            {
            char buf[1024];
            argv[index++] = (char *)"/bin/bash";
            argv[index++] = (char *) "-i"; // this forces children to be killed,
                // but swallows first Ctrl-C as well.
            argv[index++] = (char *)"-c";
            sprintf(buf, "/bin/webclient %s|%s -q -o %s -",
                (const char *)MUrl::toEncoding(list[0]), audioapppath, audionode);
            //printf("running '%s'\n", buf);
            argv[index++] = buf;
            }
            break;
        default:
            return false;
        }

        //NOTE: madplay doesn't support filelist, so I past a list of
        // files on the command line
        if (audioapptype != 2)
            for (const char *s = list.first(); s; s = list.next())
                argv[index++] = (char*)s;
        argv[index++] = 0;

        // need to sleep a little so that the driver is truly closed
        if (needToSleep)
            sleep(1);

        execve(argv[0], argv, environ);

        /* If this returns, then we weren't able to start the app */
        fprintf(stderr, "Unable to exec %s\n", argv[0]);
        exit(127);
    }
    else {
        /* parent process - if we created a pipe, it's because we're
        playing PCM audio read from child process */
        if (_pipe[0] != -1) {
            startAudioThread();
        }
    }
    return true;

error:
    if (disabledAudio)
        control()->enableAudio();
    return false;
}

bool AudioControl::stopAudio(bool reenable) {
    if (_audioPid > 0) {
        int status;

        /* Stop mp3 decode process */
        kill(_audioPid, SIGHUP); //SIGKILL);
        if (waitpid(_audioPid, &status, WNOHANG) == -1)
            BDBG_ERR(("stopAudio, waitpid failed: %d", errno));
        _audioPid = 0;

        /* Stop pcmplay thread, if started */
        stopAudioThread();

        if (reenable)
            control()->enableAudio();
        return true;
    }
    return false;
}

void AudioControl::startAudioThread()
{
#ifdef PCMPLAY_THREAD_SUPPORT
    b_task_params task_params;
    b_task_params_init(&task_params);
    task_params.name = "audiocontrol";
    _stopped = false;
    BKNI_ResetEvent(_taskEvent);
    BKNI_ResetEvent(_stoppedEvent);
    b_start_task(&_task, &task_params, b_play_audio, this);
#endif
}

void AudioControl::stopAudioThread()
{
#ifdef PCMPLAY_THREAD_SUPPORT
    if (_task) {
        BDBG_MSG(("joining audio thread"));
        _stopped = true;
        BKNI_SetEvent(_taskEvent);
        BDBG_MSG(("waiting for audio thread exit"));
        if ( BKNI_WaitForEvent(_stoppedEvent, 2000) )
            BDBG_WRN(("Timeout waiting for task stop notification"));
        BDBG_MSG(("stopping task"));
        b_stop_task(_task);
        BDBG_MSG(("done"));
        close(_pipe[0]); /* close the read handle */
        _task = 0;
    }
#endif
}

#ifdef PCMPLAY_THREAD_SUPPORT
/*******
Simple settop api pcm playback engine
********/
#include <bkni.h>

static void pcm_callback(void *context)
{
    BKNI_SetEvent((BKNI_EventHandle)context);
}
#endif

void *AudioControl::b_play_audio(void *data) {
#ifdef PCMPLAY_THREAD_SUPPORT
    AudioControl *audioControl = (AudioControl *)data;
    int fd;
    bdisplay_t display = audioControl->control()->display()->display;
    bpcm_play_settings settings;
    BKNI_EventHandle event = audioControl->getTaskEvent();

    BDBG_MSG(("b_play_audio started"));
    close(audioControl->_pipe[1]); /* close the write handle */
    fd = audioControl->_pipe[0]; /* read pipe */
    if (!audioControl->_pcm_play) {
        BKNI_SetEvent(audioControl->getStoppedEvent());
        return NULL;
    }

    bpcm_play_settings_init(&settings, audioControl->_pcm_play);
    settings.callback = pcm_callback;
    settings.callback_context = event;
    if (bpcm_play_start(audioControl->_pcm_play, display, &settings))
        goto done;

    while (!audioControl->stopRequested())
    {
        void *buffer;
        size_t size;
        ssize_t result_size;

        if (bpcm_play_get_buffer(audioControl->_pcm_play, &buffer, &size))
            goto done;

        if (!size) {
            if (BKNI_WaitForEvent(event, 0xFFFFFFFF))
                break;
            continue;
        }

        /* read as much as possible from stdin */
        result_size = read(fd, buffer, size);
        BDBG_MSG(("read %d", result_size));
        if (result_size < 0)
            goto done;
        else if (result_size == 0) /* EOF */
            break;

        if (bpcm_play_write_complete(audioControl->_pcm_play, result_size))
            goto done;
    }

done:
    BDBG_MSG(("stopping pcm playback"));
    bpcm_play_stop(audioControl->_pcm_play);
    BDBG_MSG(("exiting audio thread (done stopping)"));
    BKNI_SetEvent(audioControl->getStoppedEvent());
    return NULL;
#endif
}

