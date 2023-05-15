/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: main.cpp $
* $brcm_Revision: 87 $
* $brcm_Date: 3/21/12 1:53p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus/src/main.cpp $
* 
* 87   3/21/12 1:53p erickson
* SW7425-2664: remove old code
* 
* 86   4/6/11 2:30p gmohile
* SW7420-1792 : Add power standby support
* 
* 85   3/18/11 1:05p erickson
* SWDEPRECATED-2425: update brutus (c)
*
* 84   10/28/10 1:17p ahulse
* SW7405-4951: If ulimit is set non-zero, disable console stack dump
*
* 83   10/22/10 8:52a ahulse
* SW7405-4951: remove devel debug prints
*
* 82   10/20/10 1:59p gskerl
* SW7125-237: Removed temp code that prevented SATA from powering off for
* the 7125
*
* 81   10/19/10 8:25a ahulse
* SW7405-4951: Add console dump of crash stack trace for kernels 2.6.31
* and above
*
* 80   10/1/10 4:20p gskerl
* SW7468-102: Added mpu (multi-processor) field to the brutus_power_state
* structure
*
* 79   9/24/10 1:15p gskerl
* SW7468-102: Don't use DDR self refresh for passive standby because it
* seems to prevent the board from waking up
*
* 78   8/24/10 2:35p gskerl
* SW7125-237: For passive standby, leave CPU at full speed and
* (temporarily) leave SATA on
*
* 77   6/25/10 5:31p gskerl
* SW7125-403: Merged Active Standby changes back to main branch.
*
* BRUTUS_97125_ACTIVE_STANDBY/2   6/25/10 3:46p gskerl
* SW7125-403: Renamed brutus_standby to brutus_passive_standby
*
* BRUTUS_97125_ACTIVE_STANDBY/1   5/29/10 3:39p gskerl
* SW7125-403: Modified Brutus for testing Active Standby
*
* 76   2/26/10 3:12p gskerl
* SW7405-3625: More 3D PIG: When OUPUT_3D_MODE is defined, force
* PIP_ENABLED to false
*
* 75   12/22/09 4:00p erickson
* SW7420-70: add POWERSTANDBY_TIMEOUT option to brutus.cfg
*
* 74   9/29/09 3:41p jtna
* SWDEPRECATED-3910: increase video CDB and disable 2nd video decoder for
* FCC
*
* 73   9/24/09 11:55a erickson
* SW7405-3069: add passive standby options for USB, SATA and ETHERNET.
* default to minimum power.
*
* 72   9/14/09 3:05p jgarrett
* SW7405-3011: Splitting sync control into precision and basic
*
* 71   9/11/09 10:04a jtna
* SWDEPRECATED-3910: disable sync channel for FCC
*
* 70   9/4/09 11:44a jgarrett
* SWDEPRECATED-3910: Adding video fifo size option for fast channel
* change
*
* 69   8/12/09 2:42p jjordan
* PR51648: Add Bandgap adjustment to Brutus
*
* 68   7/13/09 2:45p erickson
* PR56516: enabling SATA power down. I had a bad power supply.
*
* 67   7/13/09 2:18p erickson
* PR56516: can't power down SATA on any platform
*
* 66   7/10/09 4:42p erickson
* PR56517: call brutus_standby. brcmpmdrv is now fixed.
*
* 65   7/8/09 10:40a erickson
* PR56373: add FIRST_CHANNEL option. restore user's channel after wakeup
* from passive standby mode.
*
* 64   7/2/09 5:09p erickson
* PR56373: rework brutus_power to allow for deferred usb & sata power,
* deferred mount, deferred videos scan
*
* 63   7/2/09 10:57a erickson
* PR56558: defer frontend init and channel scan until after the GUI is up
*
* 62   6/26/09 3:53p erickson
* PR56373: remove unused code
*
* 61   6/26/09 2:29p erickson
* PR56373: fix POWERSTANDBY_SUPPORT=n
*
* 60   6/26/09 2:08p erickson
* PR56373: rework brutus passive standby to not exit the app
*
* 59   3/18/09 12:30p erickson
* PR42789: fix non-bluetooth builds
*
* 58   3/18/09 11:41a randyjew
* PR42789: updates for BLUETOOTH_SUPPORT, added bt remote/picture push
*
* 57   2/6/09 10:20a erickson
* PR35457: updated copyright date
*
* 56   8/18/08 9:45a ahulse
* PR44902: Add StandbyActive mode to brutus shutdown
*
* 55   8/15/08 10:08a erickson
* PR42789: bluetooth update
*
* 54   8/13/08 10:12a erickson
* PR42789: fix BT_ENABLED check
*
* 53   8/13/08 5:26a erickson
* PR42789: updates for BLUETOOTH_SUPPORT
*
* 52   5/21/08 12:02p rjlewis
* PR40352: function doesn't exist anymore.
*
* 51   4/2/08 2:40p rjlewis
* PR38655: Use new definitions to enable disks.
*
* 50   3/17/08 3:17p erickson
* PR40307: remove old, unused code before adding new
*
* 49   3/3/08 3:46p erickson
* PR40168: added VIDEO_TOOL brutus.cfg optoin
*
* 48   2/13/08 4:50p tokushig
* PR39534: added bluetooth remote functionality to brutus
*
* 47   7/9/07 4:29p erickson
* PR32256: update Brutus copyright
*
* 46   4/24/07 9:10a jrubio
* PR30010: took out old IP Streamer code
*
* 45   3/12/07 3:55p erickson
* PR28339: use stack pad in Brutus init to fool stackguard. this allows
* it to run as normal. there should be no more warnings.
*
* 44   2/1/07 1:37p rjlewis
* PR27535: cleanup indentation and added comments.
*
* 43   12/29/06 4:43p erickson
* PR26813: b_start_task now requires b_task_params, which should be
* initialized with b_task_params_init
*
* 42   10/6/06 6:46p rjlewis
* PR24703: restore checkin comments deleted by merge (scc).
*
* 41   10/6/06 6:36p rjlewis
* PR24703: Discontinue disk setup if any of the setup steps fail.  this
* will prevent the system from crashing if no disk is attached.
*
* 40   10/2/06 10:44a jrubio
* PR24672: Added IP Streamer Support
*
* 39   9/29/06 10:59a rjlewis
* PR24594: Don't include these functions of no PVR.
*
* 38   9/28/06 11:53p ssood
* PR24576: Integration w/ BLIVE_EXT Library (SAP + LM Scheduler)
*
* 37   9/26/06 11:28a erickson
* PR24374: remove unneeded #ifdef's, make DUAL_OUTPUT test generic using
* bconfig
*
* 36   9/1/06 2:31p erickson
* PR21941: fix/clarify --help
*
* 34   7/20/06 11:19a erickson
* PR21941: process rc to avoid warning
*
* 33   6/30/06 2:51p erickson
* PR21941: fix warning
*
* SanDiego_Brutus_3D/5   1/30/08 2:19p tokushig
* refactor demo bluetooth remote implementation for integration into main
* line
*
* SanDiego_Brutus_3D/4   12/21/07 2:40p tokushig
* merged BT_REMOTE_ACCESS brutus config option
*
* SanDiego_Brutus_BTRemote/2   12/21/07 1:11p randyjew
* improved bluetooth modificatiosn for allowing commands during messages
* boxes and to be able to configure address in brutus.cfg file
* BT_REMOTE_ADDRESS=<12_character_address>
*
* SanDiego_Brutus_3D/3   12/20/07 4:58p tokushig
* added support for bluetooth remote.  note that user must press and hold
* down <enter> and <start> remote keys before starting brutus and must
* keep them held down until graphics are displayed on screen - this is
* so the remote can pair with the bluetooth usb dongle
*
* SanDiego_Brutus_BTRemote/1   12/17/07 1:26p randyjew
* BT Remote demo
*
* SanDiego_Brutus_3D/2   12/14/07 3:52p tokushig
* 3d working with brutus.  minor draw issues when going back to
* fullscreen. and problem drawing bTV icon when going back into the
* menu.  audio not working but prob config spec issue.
*
* SanDiego_Brutus_3D/1   12/10/07 3:54p tokushig
* 720p brutus working with integrated 720p btv
*
* 47   7/9/07 4:29p erickson
* PR32256: update Brutus copyright
*
* 46   4/24/07 9:10a jrubio
* PR30010: took out old IP Streamer code
*
* 45   3/12/07 3:55p erickson
* PR28339: use stack pad in Brutus init to fool stackguard. this allows
* it to run as normal. there should be no more warnings.
*
* 44   2/1/07 1:37p rjlewis
* PR27535: cleanup indentation and added comments.
*
* 43   12/29/06 4:43p erickson
* PR26813: b_start_task now requires b_task_params, which should be
* initialized with b_task_params_init
*
* 42   10/6/06 6:46p rjlewis
* PR24703: restore checkin comments deleted by merge (scc).
*
* 41   10/6/06 6:36p rjlewis
* PR24703: Discontinue disk setup if any of the setup steps fail.  this
* will prevent the system from crashing if no disk is attached.
*
* 40   10/2/06 10:44a jrubio
* PR24672: Added IP Streamer Support
*
* 39   9/29/06 10:59a rjlewis
* PR24594: Don't include these functions of no PVR.
*
* 38   9/28/06 11:53p ssood
* PR24576: Integration w/ BLIVE_EXT Library (SAP + LM Scheduler)
*
* 37   9/26/06 11:28a erickson
* PR24374: remove unneeded #ifdef's, make DUAL_OUTPUT test generic using
* bconfig
*
* 36   9/1/06 2:31p erickson
* PR21941: fix/clarify --help
*
* 34   7/20/06 11:19a erickson
* PR21941: process rc to avoid warning
*
* 33   6/30/06 2:51p erickson
* PR21941: fix warning
*
* 32   5/31/06 4:20p tokushig
* PR21891: Merging updated picture viewer into mainline.
*
* SanDiego_Brutus_Skin/7   5/16/06 11:33a tokushig
* merge from main
*
* 31   5/10/06 4:47p rjlewis
* PR21414: indentation and comments.  Also debug prints when creating
* brutus threads.
*
* 30   5/4/06 12:05p jgarrett
* PR 21266: Merging 3-display code to main.
*
* 29   5/3/06 2:44p mphillip
* PR21341: Add change to head as well as release branch
*
* REFSW_97400_3DISPLAY/1   5/3/06 1:34p jgarrett
* PR 21266: Adding HDSD_SINGLE + DUAL_OUTPUT
*
* 27   4/17/06 5:05p jgarrett
* PR 20951: Merging skinned UI into mainline
*
* SanDiego_Brutus_Skin/6   4/13/06 3:16p tokushig
* merge from main to SanDiego_Brutus_Skin branch
*
* SanDiego_Brutus_Skin/5   2/23/06 2:27p tokushig
* merge from 97400 prerel label
*
* 25   2/17/06 12:07p rjlewis
* PR19722: document says scripts, make app use plural too.
*
* 24   2/17/06 12:01p rjlewis
* PR19706: don't allow DUAL_OUTPUT setting on 97401.
*
* 23   2/14/06 12:11p rjlewis
* PR19044: only need to set priority in one place.  Added a means to run
* without the target server.
*
* SanDiego_Brutus_Skin/4   2/15/06 9:56a tokushig
* change screen_x and screen_y if -skin option is specified.  this will
* give the skin the entire tv screen to work with.
*
* SanDiego_Brutus_Skin/3   2/2/06 2:32p tokushig
* update from main
*
* SanDiego_Brutus_Skin/2   1/19/06 12:48p tokushig
* update merge from main
*
* SanDiego_Brutus_Skin/1   1/3/06 10:25a tokushig
* Added XML based skin capability to brutus.  "-skin <name>" command line
* option
* allows user to specify skin on startup.  Note that running brutus
* without the
* -skin option will use the default GUI.
*
* 19   10/25/05 11:52a qcheng
* PR17802: add support  SECOND_APP_DELAY for dual channel output in
* brutus.cfg
*
* 18   9/8/05 10:01a erickson
* PR17048: switch individual chip defines to B_HAS_VDC for all 7038 and
* beyond platforms
*
* 17   8/24/05 6:14p arbisman
* PR16678: Add support for WinCE
*
* 16   8/24/05 11:14a erickson
* PR16609: removed some revision history, especially the duplicates from
* a bad merge
*
* 15   8/9/05 1:43p rjlewis
* PR16609: Merged vxworks changes from branch PR15236.
* PR15236: PR16379: Adding overloaded new and delete to set memory to
* zero.  This ensures that, like the Linux release, all uninitialized
* variables start off as zero.  This corrects a number of exception type
* problems in the VxWorks version of Brutus.
* PR15236: Added for vxworks the autocreation of the videos directory
* PR15236: Set bypass cache to 128 or 64K now.
* PR15236: Added cachetune and cache resize calls for DosFs cache
*
* 14   6/7/05 3:27p erickson
* PR14721: readded channelmanager scan for legacy
*
* 13   5/23/05 12:03p erickson
* PR15017: removed brutus1Sem, b_start/stop_task now handles this
*
* 12   4/29/05 11:41a erickson
* PR15017: added brutus1Sem give
*
* 11   4/29/05 11:13a erickson
* PR14871: added time to buildDate
*
* 10   4/27/05 4:25p erickson
* PR14708: increase priority of brutus thread
*
* 9   4/27/05 1:14p erickson
* PR14708: bring in standard /ata0 mounting for vxworks
*
* 8   4/22/05 3:09p erickson
* PR14708: added jose's automount code, but left it commented out. wasn't
* working for me.
*
* 7   4/19/05 3:46p erickson
* PR14701: make digital subchannel support standard for all platforms
*
* 6   4/9/05 12:19a erickson
* PR14708: better definition of linux/vxworks
*
* 5   4/7/05 5:12p erickson
* PR14708: added vxworks sem fixes
*
* 4   2/24/05 1:21p erickson
* PR14180: removed hack
*
* 3   2/23/05 9:32a erickson
* PR14180: fixed no SCRIPT_SUPPORT on vxworks
*
* 2   2/17/05 2:44p ywu
* PR 13748: Create an IR keystrokes automation utility
*
* 1   2/7/05 8:14p dlwin
* Merge down for release 2005_REFSW_MERGETOMAIN:
*
***************************************************************************/
#include "brutus_app.h"
#include "bsettop_os.h"
#include "brutus_os.h"
#include "bstd.h"

#ifndef _WIN32_WCE
    #include <stdio.h>
    #include <signal.h>
#endif

#ifdef INFOPANEL_SUPPORT
#include "info_outputs.h" // for #defines
#endif
#ifdef __vxworks
#include <taskLib.h>
#include "usrLib.h"
#ifdef PVR_SUPPORT
#include "drv/hdisk/ataDrv.h"
#include "dosFsLib.h"
#include "dpartCbio.h"
#include "usrFdiskPartLib.h"
#include "dcacheCbio.h"
#include "dosFsLib.h"
#include <private/dosFsLibP.h>
#include <private/cbioLibP.h>
#endif
#endif

#include "scriptengine.h"
#include "brutus_video_tool.h"

#ifdef BLUETOOTH_SUPPORT
#include "easy_bt.h"
#include "bt_hook_priv.h"
#endif

#ifdef STACK_TRACE
#include <sys/resource.h>
#include "brcm_sig_hdl.h"
#endif

#if POWERSTANDBY_SUPPORT
#include "brutus_power.h"
#endif

BDBG_MODULE(brutus_main);

#ifdef __vxworks
// It was discovered that the linux kernel zeros the heap before starting an application (i.e. Brutus) whereas vxWorks does not!
// Thus, Linux 'malloc' and 'new' requests initially return memory that is initialized to zero.
// This allows many of the uninitialized variables to start out with a "known" value of zero.
// By not having this memory set to zero VxWorks generates a number of exceptions that don't show up in the lixux release.
// This is a temporary patch to make the VxWorks version much more stable.
// The oppasite of this patch (one that sets memory to non-zero value) will be added to the Linux version to help "discover'
// where data is NOT being properly initialized to zero when it should.  This patch should NOT be applied to the next release!
void *operator new(size_t sz)
{
    void *mem = malloc(sz);
    if (mem) memset(mem, 0, sz);
    return mem;
}
// We technically don't need to overload the delete as this "should" be doing what the real delete is doing, right?
void operator delete(void *mem)
{
    free(mem);
}
#endif



/* Only check files if they are specified on the cmd line.
We want to make sure that if you specify something, you get it right.
If a default file doesn't exist, that's ok. */
static int checkfile(const char *filename)
{
    int64_t size;
    if (filename && b_get_file_size(filename, &size)) {
        /* can't use debug interface at this point */
        fprintf(stderr, "'%s' doesn't exist\n", filename);
        return -1;
    }
    return 0;
}

static BDBG_Level parse_debug_level(const char *level)
{
    if (!strcasecmp(level, "trace"))
        return BDBG_eTrace;
    else if (!strcasecmp(level, "wrn"))
        return BDBG_eWrn;
    else if (!strcasecmp(level, "msg"))
        return BDBG_eMsg;
    else
        return BDBG_eErr;
}

static void *startBrutus_from_task(void *data)
{
    return (void*)((Brutus*)data)->start();
}


/**
main.cpp must be recompiled every time in order to make sure this
function returns the last build date. Other files can use this
and so don't have to be recompiled every time.
**/
const char *buildDate() {
    return __DATE__ " " __TIME__;
}

void printUsage()
{
    printf(
    "Usage: brutus [options]\n"
    "  --help          Print this help screen\n"
    "  --help-cfg      Print config file options\n"
    "  -cfg FILE       Specify a config file (default is brutus.cfg)\n"
    "  -ch FILE        Specify a channel map file (default is channels.txt)\n"
    "  -pg FILE        Specify a program guide file (default is programguide.txt)\n"
    "  -pal            576i (PAL) display mode\n"
    "  -admin          Set admin sticky mode by default\n"
#ifndef __vxworks
    "  -tty            Use tty input mode instead of Sejin IR keyboard\n"
#endif
    "  -skin NAME      Use given skin for GUI\n"
    "  -dbg {err|wrn|msg}    Set the debug level\n"
#ifdef SCRIPT_SUPPORT
    "  --exec          Start interactive script mode.\n"
    "  -scr SCRIPTFILE Run a script file.\n"
    "  -scrtext SCRIPT Run a script from command line.\n"
#endif
#ifdef HD_SUPPORT
//  "  -16x9           Render into a 16x9 graphics buffer\n"
    "  -hd             1080i display mode\n"
#endif
    );
}

#ifdef LINUX
/* In Linux usermode, the main thread will call into Brutus and Settop API.
The stackguard debug feature in Settop API tries to protect it against stack overflow,
especially in small stack environments (i.e. Linux kernel mode). But Brutus
requires a lot of stack in a variety of places. The b_start_settopapi function fools stackguard
into thinking the Settop API's use of the stack is larger. This avoids false stack overflow
detects, but still keeps some benefit in other threads like event and ISR processing.
*/
int b_start_settopapi() /* This function cannot be static. */
{
    uint8_t stack_reserve[8192];
    BKNI_Memset(stack_reserve, 0, sizeof(stack_reserve));

    // start settop api
    if (bsettop_init(BSETTOP_VERSION))
        return -1;
    return 0;
}
#endif


/**
This is the universal entry point for Brutus and
is entry point for Linux. Other operating systems (like VxWorks)
have other entry points which then call main().
**/
int main(int argc, char **argv)
{
    const char *cfgfile;
    char *ipstr = NULL;
    bool scriptmode = false;
    bool interactivemode = false;
    Config *cfg;
    bool skin_specified = false;
    char *skin_name = NULL;
    BERR_Code rc;
    bsettop_init_settings init_settings;
    int wakeupFirstChannel = 0;

    fprintf(stderr,
        "brutus (C)2001-2011, Broadcom Corporation\n"
    "  Built on %s, BCHP_CHIP %d\n", buildDate(), BCHP_CHIP);

    cfg = new Config;

#if POWERSTANDBY_SUPPORT
    /* set an initial state in case the app terminated with power off */
    brutus_init_power_state();
#endif

    scriptmode = false;
    interactivemode = false;

    /**
    ** Set default Config values before reading cmdline or config file
    **/
    cfgfile = "brutus.cfg";
    setDefaults(cfg);

    /**
    ** Read cmdline options
    **/
    for (int i=1; i<argc; i++)
    {
        if (!strcasecmp(argv[i], "-admin"))
            cfg->set(Config::ADMIN, true);
        else if (!strcasecmp(argv[i], "-pal"))
            cfg->set(Config::OUTPUT_FORMAT, "576i");
#ifdef HD_SUPPORT
        else if (!strcasecmp(argv[i], "-hd"))
            cfg->set(Config::OUTPUT_FORMAT, "1080i");
#endif
#ifndef __vxworks
        else if (!strcasecmp(argv[i], "-tty")) {
            if (interactivemode) {
                fprintf(stderr, "ERROR: Cannot run more than one interactive mode\n\n");
                printUsage();
                return -1;
            }
            cfg->set(Config::TTY, true);
            interactivemode = true;
        }
#endif
        else if ((!strcasecmp(argv[i], "-skin")) && (i<argc-1)) {
            /* we can't process the skin info until after we process the config file */
            skin_specified = true;
            skin_name = argv[++i];
        }
        else if (!strcasecmp(argv[i], "-initonly")) {
            cfg->set(Config::INIT_ONLY, true);
        }
        else if (!strcasecmp(argv[i], "-cfg") && i<argc-1) {
            cfgfile = argv[++i];
            if (checkfile(cfgfile)) return -1;
        }
        else if (!strcasecmp(argv[i], "-ch") && i<argc-1) {
            const char *file = argv[++i];
            if (!strcmp(file, "none"))
                file = NULL;
            else {
                printf("-ch %s\n", file);
                if (checkfile(file)) return -1;
            }
            cfg->set(Config::CHANNEL_MAP, file);
        }
        else if (!strcasecmp(argv[i], "-pg") && i<argc-1) {
            const char *file = argv[++i];
            cfg->set(Config::PROGRAM_GUIDE, file);
            if (checkfile(file)) return -1;
        }
#ifdef SCRIPT_SUPPORT
        else if (!strcasecmp(argv[i], "--exec")) {
            if (interactivemode) {
                fprintf(stderr, "ERROR: Cannot run more than one interactive mode\n\n");
                printUsage();
                return -1;
            }
            if (scriptmode) {
                fprintf(stderr, "ERROR: Cannot run more than one script mode\n\n");
                printUsage();
                return -1;
            }
            /* We must remember that we're in INTERACTIVE_MODE mode so that
            the user won't be allowed to start a script from the admin screen. */
            cfg->set(Config::INTERACTIVE_MODE, true);
            scriptmode = true;
            interactivemode = true;
        }
        else if (!strcasecmp(argv[i], "-scr") && i<argc-1) {
            if (scriptmode) {
                fprintf(stderr, "ERROR: Cannot run more than one script mode\n\n");
                printUsage();
                return -1;
            }
            cfg->set(Config::INITIAL_SCRIPT, argv[++i]);
            scriptmode = true;
        }
        else if (!strcasecmp(argv[i], "-scrtext") && i<argc-1) {
            if (scriptmode) {
                fprintf(stderr, "ERROR: Cannot run more than one script mode\n\n");
                printUsage();
                return -1;
            }
            cfg->set(Config::INITIAL_SCRIPTTEXT, argv[++i]);
            scriptmode = true;
        }
#endif /* SCRIPT_SUPPORT */
        else if (!strcasecmp(argv[i], "-dbg") && i<argc-1) {
            char *level = argv[++i];
            cfg->set(Config::APP_DEBUG_LEVEL, level);
        }
        else if (!strcasecmp(argv[i], "-app_dbg") && i<argc-1) {
            cfg->set(Config::APP_DEBUG_LEVEL, argv[++i]);
        }
        else if (!strcasecmp(argv[i], "--help")) {
            printUsage();
            return 0;
        }
        else if (!strcasecmp(argv[i], "--help-cfg")) {
            cfg->printHelp();
            return 0;
        }
        else {
            /* It's important to fail on a bad parameter.
            ** It makes debugging problems over the phone much easier.
            */
            fprintf(stderr, "ERROR: Unrecognized parameter: %s\n\n", argv[i]);
            printUsage();
            return 1;
        }
    }

#ifdef __vxworks
    {
        /*
        ** The initialized and uninitialized data is ONLY set when the image is loaded.
        ** If they "quit" then they cannot restart without reloading the application (and thus reinitialize the environment)!
        ** Let them know they have to do this (print to stderr goes to the serial port, stdout goes to the target server shell)!
        */
        static bool already_ran;
        if (already_ran) {
            fprintf(stdout/*stderr*/, "You need to reload the image(s) in order to run again!\n");
            return 1;
        }
        already_ran = true;
    }
#endif

    /**
    ** Now start configuring the system
    **/

#ifndef _WIN32_WCE
    // picture http client might get this...best not to kill the app anyway
    signal(SIGPIPE, SIG_IGN);

#endif /* _WIN32_WCE */

    bsettop_get_init_settings(&init_settings);
    init_settings.open_frontend = false;
    /**
    ** Read the config file.
    **/
    BKNI_Printf("  cfgfile = %s\n", cfgfile);

    /* if you can't load the config file, the default is not there. that's ok. */
    cfg->read(cfgfile);

    if(cfg->getInt(Config::VIDEO_DAC0_BAND_GAP)) {
        init_settings.video_dac_band_gap[0] = cfg->getInt(Config::VIDEO_DAC0_BAND_GAP);
    }
    if(cfg->getInt(Config::VIDEO_DAC1_BAND_GAP)) {
        init_settings.video_dac_band_gap[1] = cfg->getInt(Config::VIDEO_DAC1_BAND_GAP);
    }
    if(cfg->getInt(Config::VIDEO_DAC2_BAND_GAP)) {
        init_settings.video_dac_band_gap[2] = cfg->getInt(Config::VIDEO_DAC2_BAND_GAP);
    }
    if(cfg->getInt(Config::VIDEO_DAC3_BAND_GAP)) {
        init_settings.video_dac_band_gap[3] = cfg->getInt(Config::VIDEO_DAC3_BAND_GAP);
    }
    if(cfg->getInt(Config::VIDEO_DAC4_BAND_GAP)) {
        init_settings.video_dac_band_gap[4] = cfg->getInt(Config::VIDEO_DAC4_BAND_GAP);
    }
    if(cfg->getInt(Config::VIDEO_DAC5_BAND_GAP)) {
        init_settings.video_dac_band_gap[5] = cfg->getInt(Config::VIDEO_DAC5_BAND_GAP);
    }
    if(cfg->getInt(Config::VIDEO_DAC6_BAND_GAP)) {
        init_settings.video_dac_band_gap[6] = cfg->getInt(Config::VIDEO_DAC6_BAND_GAP);
    }
    if(cfg->getBool(Config::VIDEO_PRIMER_FASTCHANNELCHANGE_ENABLED)) {
        init_settings.video_fifo_size = 5*1024*1024; /* larger CDB needed for HD fcc */
        init_settings.lipsync_mode = bsettop_lipsync_mode_tsm; /* disable sync channel for now */

        /* don't open 2nd video decoder. memory for CDB is limited */
        cfg->set(Config::SECOND_DISPLAY, 0);
        cfg->set(Config::PIP_ENABLED, 0);
    }

    if(cfg->get(Config::OUTPUT_3D_MODE)) {
        if(cfg->getBool(Config::PIP_ENABLED)) {
            BDBG_WRN(("OUTPUT_3D_MODE is defined, disabling PIP_ENABLED"));
            cfg->set(Config::PIP_ENABLED, 0);
        }
    }
#ifdef STACK_TRACE
    {
        struct rlimit rlim;

        /* Get the core dump limitation */
        getrlimit(RLIMIT_CORE, &rlim);

        if( rlim.rlim_cur==0 && cfg->getBool(Config::DISABLE_CONSOLE_STACK_DUMP) == false ) {
            installSignalHandler( 0 );
        }
    }
#endif

    bsettop_set_init_settings(&init_settings);

#ifdef LINUX
    if (b_start_settopapi())
        return -1;
#else
    if (bsettop_init(BSETTOP_VERSION))
        return -1;
#endif

    /* we must conformConfig after bsettop_init so that bconfig is initialized. */
    if (conformConfig(cfg))
        return 1;

    /* when waking up from standby, the brutus.cfg FIRST_CHANNEL should be overridden with the user's current channel */
    if (wakeupFirstChannel) {
        cfg->set(Config::FIRST_CHANNEL, wakeupFirstChannel);
    }

#if defined(DUAL_CMP_SUPPORT)
    /* DUAL_OUTPUT not valid for this single decode systems */
    if (cfg->getBool(Config::DUAL_OUTPUT) && bconfig->resources.decode.total == 1) {
        BKNI_Printf("Error: Cannot use 'DUAL_OUTPUT' setting in configuration file on a single decode chip.\n");
        return -1; // don't continue -- don't let them run with incorrect settings!
}
#endif

    if (cfg->getBool(Config::VIDEO_TOOL)) {
        launchVideoTool();
    }

#ifdef BLUETOOTH_SUPPORT
    if (cfg->getBool(Config::BT_ENABLED)) {
        EasyBtInitSettings initSettings;

        EasyBt_GetDefaultInitSettings(&initSettings);
        EasyBt_Init(&initSettings);
        if (cfg->get(Config::BT_REMOTE_ADDRESS))
        {
            printf("\n\nPress and hold the BD remote's ENTER and Start buttons, then press ENTER on the console. Do not release until you see BTUI_MMI_HID_CONNECT on the console.\n");
            getchar();
            EasyBt_ConnectRemote(cfg->get(Config::BT_REMOTE_ADDRESS));
        }
    }
    else {
        /* If BT_ENABLED is not set, turn off all BT features */
        cfg->set(Config::BT_AUDIO_INPUT_ENABLED, "false");
        cfg->set(Config::BT_REMOTE_ADDRESS, NULL);
    }
#endif

    /* We can't check the skins information until AFTER we process the configuration file. */
    if (skin_specified) {
        char filename[255];
        strncpy(filename, cfg->get(Config::SKINS_PATH), sizeof(filename));
        strncat(filename, "/", sizeof(filename));
        strncat(filename, skin_name, sizeof(filename));
        strncat(filename, "/", sizeof(filename));
        strncat(filename, cfg->get(Config::SKIN_FILE), sizeof(filename));
        if (checkfile(filename)) return -1;

        cfg->set(Config::SKIN, skin_name);
        cfg->set(Config::SCREEN_X,25);
        cfg->set(Config::SCREEN_Y,20);
    }

    // set the debug level after starting settop api, because it sets its own default
    rc = BDBG_SetLevel(parse_debug_level(cfg->get(Config::APP_DEBUG_LEVEL)));
    if (rc) return -1;

    /* Create ChannelManager and AVManager which handle global resources */
    ChannelManager *channelManager = createChannelManager(cfg);
    AVManager *avManager = new AVManager(channelManager);
#ifdef PVR_SUPPORT
    BKNI_Printf("  Supporting %d record%s\n", bconfig->resources.record.total,bconfig->resources.record.total==1?"":"s");
    RecordManager *recordManager = createRecordManager(bconfig->resources.record.total, cfg);
#else
    RecordManager *recordManager = NULL; // unused
#endif

    /*
    ** In Dual Output mode we use a separate application (thread) for each of the outputs.
    ** The second output gets a new thread (task) and we use this thread for the primary output.
    */
    if (cfg->getBool(Config::DUAL_OUTPUT))
    {
        Brutus *brutus0 = NULL;
        Brutus *brutus1 = NULL;

        Config cfg2(*cfg); /* make a copy */
        ScriptEngine *scriptEngine = NULL;

        cfg->set(Config::SECOND_DISPLAY, 0);
        cfg->set(Config::SA_REMOTE, 0);
        BDBG_MSG(("Creating brutus 0"));
        brutus0 = new Brutus(channelManager, recordManager, avManager, cfg);

        cfg2.set(Config::SECOND_DISPLAY, 1);
        cfg2.set(Config::SA_REMOTE, 1);
        cfg2.set(Config::HDSD_SINGLE, false);
        BDBG_MSG(("Creating brutus 1"));
        brutus1 = new Brutus(channelManager, recordManager, avManager, &cfg2);

#ifdef SCRIPT_SUPPORT
        /* Create a single ScriptEngine for both instances of Brutus */
        scriptEngine = new ScriptEngine();
        scriptEngine->addInputEngine(brutus0->inputEngine());
        scriptEngine->addInputEngine(brutus1->inputEngine());
        if (cfg->get(Config::INTERACTIVE_MODE))
            scriptEngine->runInteractiveMode();
        else if (cfg->get(Config::INITIAL_SCRIPT))
            scriptEngine->runScript(cfg->get(Config::INITIAL_SCRIPT));
        else if (cfg->get(Config::INITIAL_SCRIPTTEXT))
            scriptEngine->runScriptText(cfg->get(Config::INITIAL_SCRIPTTEXT));
#endif /* SCRIPT_SUPPORT */

        if (!cfg->getBool(Config::INIT_ONLY) && brutus0->valid() && brutus1->valid())
        {
            b_task_t task;
            b_task_params params = {40, 20*1024, "brutus1"};

            /* Run the second app from a background thread */
            BDBG_MSG(("Starting brutus 1"));
            b_start_task(&task, &params, startBrutus_from_task, brutus1);

            if (cfg->get("SECOND_APP_DELAY")) {
                BKNI_Sleep(atoi(cfg->get("SECOND_APP_DELAY")));
            }

            /* Run the first app in the foreground (this thread) */
            BDBG_MSG(("Starting brutus 0"));
            brutus0->start();

            /* stop will wait for background thread to exit */
            b_stop_task(task);
        }

        /* We'll get here only after the application is told to quit */
        if (scriptEngine)
            delete scriptEngine;
        delete brutus1;
        delete brutus0;
    }
    else
    {
        Brutus *brutus = NULL;

        /*
        ** When not in DUAL_OUTPUT mode, we use a single (this) thread for the output.
        */
        BDBG_MSG(("Creating brutus"));
        brutus = new Brutus(channelManager, recordManager, avManager, cfg);

        if (!cfg->getBool(Config::INIT_ONLY) && brutus->valid())
        {
            BDBG_MSG(("Starting brutus"));
            /* run the brutus app in the forground */
            brutus->start();
            /* Note: in single output mode the --scr and --scrtext script
               processing gets handled in the Brutus constructor. */

            /* capture current channel for passive standby */
            wakeupFirstChannel = brutus->control()->channel();
        }

        /* We'll get here only after the application is told to quit */
        delete brutus;
    }

#ifdef PVR_SUPPORT
    destroyRecordManager(recordManager);
#endif
    delete avManager;
    destroyChannelManager(channelManager);
    bsettop_uninit();

    delete cfg;

    return 0;
}

#ifdef BLUETOOTH_SUPPORT
int EasyBt_Init(const EasyBtInitSettings *pSettings)
{
    b_task_t hTask;
    b_task_params params;
    b_task_params_init( &params );
    params.name = "bluetooth";


  /* bring up bluetooth stack */
     b_start_task(&hTask, &params, BtHook_BrutusInit, (void *) pSettings );

     return 0;
}
#endif

//\\/\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifdef __vxworks

/*
** VxWorks uses a target shell to start up the application.  This shell lets the user run
** any function interactively from a thread created just for processing an application.
** The following routines are here to support utility functions (i.e. setup disk, format, etc.).
** This also includes the main entry point for the application.
*/

/* This gives a flat entry point for vxworks. */
int vxworks_main(
    char *arg1,char *arg2,char *arg3,char *arg4,char *arg5,
    char *arg6,char *arg7,char *arg8,char *arg9,char *arg10)
{
    /* VxWorks starts the default task off with a high priority.
    ** Other OSs start off with a low priority.  Simulate that environment by lowering our priority.
    */
    taskPrioritySet(taskIdSelf(),102);

    char *argv[11] = {"brutus",arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10};

    int argc;
    for (argc=1; argc<11; argc++)
        if (!argv[argc]) break;
    return main(argc,argv);
}

/* If called from CPP code (like cable modem stack) these have to be straight "C" calls */
extern "C" {

/* VxWorks needs to mount and setup the target disk when used for PVR */
#if defined(SATA_DISK_SUPPORT) || defined(USB_DISK_SUPPORT)
# define INCLUDE_DISK_SUPPORT
#endif

#ifdef INCLUDE_DISK_SUPPORT

/* You only get to build for one or the other -- not both */
# ifdef SATA_DISK_SUPPORT
# define VXWORKS_DISK "/ata0"   /* SATA disk */
# else
# define VXWORKS_DISK "/bd0"    /* USB external disk */
# endif

static CBIO_DEV_ID dosFsGetCbio(char *devName)
{
    DOS_VOLUME_DESC_ID pVolDesc;

    if (devName == NULL) devName = VXWORKS_DISK;
    pVolDesc = dosFsVolDescGet(devName, NULL);
    if (pVolDesc == NULL)
    {
        printf("Error getting volume descriptor\n");
        return((CBIO_DEV_ID)ERROR);
    }
    return (pVolDesc->pCbio->cbioSubDev);
}

/* This is just easier than trying to remember the sequence. */
int formatDisk()
{
#ifdef SATA_DISK_SUPPORT
    usrAtaConfig(0,0,VXWORKS_DISK);
#endif
    dosFsVolFormat((void*)VXWORKS_DISK, 0x312, (FUNCPTR)8192);
    return 0;
}

void setupDisk()
{
    /* if this gets run from the shell before starting brutus, don't do this again! */
    static int doOnce;
    if (doOnce) return; doOnce = 1;

    /* For VxWorks automount disk partition:

       If you have not formatted the hard drive please do the following command in the host shell before continuing:
           -> usrAtaConfig(0,0,"/ata0")
           -> dosFsVolFormat("/ata0",0x312, 8192)
    or
           -> dosFsVolFormat("/bd0",0x312, 8192)

       If you want to create the videos directory manually under /ata0 do the command below (make sure your brutus.cfg
       has /ata0/videos as the VIDEOSDIR if you do the command below)  otherwise this function automatically creates it if
       it is not present.
           -> mkdir("/ata0/videos")
    or
           -> mkdir("/bd0/videos")

       That should format the drive then reset the board and the code below will automount the drive.
       If you created the 'videos' directroy it will also appear under /ata0 or /bd0.
    */

#ifdef SATA_DISK_SUPPORT
    if (usrAtaConfig(0,0,VXWORKS_DISK)) {
        printf("Failed to setup device '%s'\n", VXWORKS_DISK);
        return;
    }
#endif

    /* This code tunes and resizes the cache to 1MB */
        CBIO_DEV_ID cbio;

        /* Decrease the bypass threshold value to less then 4k */

        cbio = (CBIO_DEV_ID) dosFsGetCbio(VXWORKS_DISK);
    if (!cbio) {
        printf("Failed to get CBIO data for device: '%s'\n", VXWORKS_DISK);
        return;
    }
    if (dcacheDevMemResize(cbio,1048576)) {
        printf("Failed to resize CBIO cache for device: '%s'\n", VXWORKS_DISK);
        return;
    }
    if (dcacheDevTune(cbio,0,128,0,-1)) {
        printf("Failed to tune cache parameters for device: '%s'\n", VXWORKS_DISK);
        return;
    }

        struct stat Stat;
    char videos[50];
    sprintf(videos, "%s/videos", VXWORKS_DISK);

        /* Make sure that we create a videos directory, if it does not already exist. */
        if (stat(videos,&Stat) == -1)
            mkdir(videos);
}

#endif /*INCLUDE_DISK_SUPPORT*/

/* This is the main entry point when using VxWorks Tornado or Workbench.
** This is normally used to start up the Brutus application from the target shell.
** The application run until the user quits (you can't ^C like you can on Linux.
** The environment is setup when the image is loaded (i.e. initialized variables, memory, etc.).
** Thus, you can only run this once per load (it is suggested that you reset the board per run).
** Note: vxworks defaults all other parameters NOT supplied on the command line to zero (NULL).
*/
int go(char *arg6,char *arg7,char *arg8,char *arg9,char *arg10)
{
#ifdef INCLUDE_DISK_SUPPORT
    /* VxWorks does not automatically mount disk and filesystem.
    ** Do it now!  Note: can be done more than once.
    */
    setupDisk();
#endif

    /* This runs in interactive mode (--exec).
    ** To set debug levels you must insert also "--dbg" and "dbg_level" in seperate entries.
    */
    return vxworks_main("-ch","/tgtsvr/channels.txt", "-cfg", "/tgtsvr/brutus.cfg", "--exec", arg6, arg7, arg8, arg9, arg10);
}

/* This is a alternate version of the "go" command.
** Note: vxworks defaults all other parameters NOT supplied on the command line to zero.
** This does NOT set the "--exec" parameter so scripts can be used. Note: using 'alt_go("--exec")' is equivalent to using 'go()' above.
*/
int alt_go(char *arg5,char *arg6,char *arg7,char *arg8,char *arg9,char *arg10)
{
#ifdef INCLUDE_DISK_SUPPORT
    setupDisk();
#endif
    return vxworks_main("-ch","/tgtsvr/channels.txt", "-cfg", "/tgtsvr/brutus.cfg", arg5, arg6, arg7, arg8, arg9, arg10);
}

/* This is a alternate version of the "go" command that is used for script processing.
** Its exactly like the "alt_go" but sounds better.
*/
int scripts()
{
    return alt_go(NULL, NULL, NULL, NULL, NULL, NULL);
}

#ifdef INCLUDE_DISK_SUPPORT
/* This is a alternate version of the "go" command that uses a different set of config and channels files.
** Note: vxworks defaults all other parameters NOT supplied on the command line to zero.
** This uses disk (instead of target server) to get configuration files.
*/
int disk_go(char *arg5,char *arg6,char *arg7,char *arg8,char *arg9,char *arg10)
{
    setupDisk();
    char channels[20], config[20];
    sprintf(channels, "%s/channels.txt", VXWORKS_DISK);
    sprintf(config, "%s/brutus.txt", VXWORKS_DISK);
    return vxworks_main("-ch",channels, "-cfg", config, arg5, arg6, arg7, arg8, arg9, arg10);
}
#endif

/* This is a alternate version of the "go" command that uses a different set of config and channels files.
** Note: vxworks defaults all other parameters NOT supplied on the command line to zero.
** This uses FTP (not target server) to get configuration files.
*/
int ftp_go(char *arg5,char *arg6,char *arg7,char *arg8,char *arg9,char *arg10)
{
#ifdef INCLUDE_DISK_SUPPORT
    setupDisk();
#endif
    return vxworks_main("-ch","channels.txt", "-cfg", "brutus.cfg", arg5, arg6, arg7, arg8, arg9, arg10);
}

} /* extern "C" */

#endif /* __vxworks */

//\\/\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifdef _WIN32_WCE

#include <windows.h>
#define COUNTOF(a) (sizeof(a)/sizeof(*(a)))

//******************************************************************************
// Windows CE does not support console applications.  As a hack, we use WinMain
// as our entry point, convert the command line string to argc/argv format, and
// then call our main() routine.

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    int argc = 5;
    char *argv[5] = {"brutus.exe", "-ch", "\\Windows\\channels.txt", "-cfg", "\\Windows\\brutus.cfg"};

    main(argc, argv);
    return 0;
}

#endif /* _WIN32_WCE */

