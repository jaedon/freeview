/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: inputengine.cpp $
 * $brcm_Revision: 104 $
 * $brcm_Date: 1/20/12 2:21p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/inputengine.cpp $
 * 
 * 104   1/20/12 2:21p jtna
 * SW7425-2213: fix S3 wakeup
 * 
 * 103   1/12/12 4:18p jrubio
 * SW7346-400: enable NEC_remote only if set in brutus.cfg, ignore
 * SA_REMOTE
 * 
 * 102   4/6/11 2:30p gmohile
 * SW7420-1792 : Add power standby support
 * 
 * 101   3/3/11 9:45a erickson
 * SWDEPRECATED-3869: rename brutus "timeshifting" to be "playback &
 * record"
 *
 * 100   1/7/11 3:17p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 * PTS - STC difference
 *
 * 99   9/9/10 4:42p erickson
 * SW7550-559: move seek() command to PVR_SUPPORT section
 *
 * 98   9/2/10 4:26p erickson
 * SWDEPRECATED-1003: added TimeSkip mode for trick()
 *
 * 97   8/31/10 2:11p erickson
 * SWDEPRECATED-1003: add seek(milliseconds) script command
 *
 * 96   6/7/10 2:30p erickson
 * SW7405-4447: trim whitespace before and after brutus script commands
 * and params. also, made string memory storage safer (remove dangerous
 * typecasts away from const)
 *
 * 95   4/16/10 4:06p erickson
 * SW7405-4089: make brutus resilient to config with zero decoders
 *
 * 94   4/7/10 8:20a calvinho
 * SW7405-4127: wrapped 'NEXUS_PlaybackStatus pstatus' in PVR_SUPPORT
 *
 * 93   4/6/10 2:14p calvinho
 * SW7405-4127: wrapped st command around PVR_SUPPORT and NEXUS_BUILD
 *
 * 92   4/1/10 6:01p calvinho
 * SW7405-4127: added st command
 *
 * 91   12/22/09 4:00p erickson
 * SW7420-70: use IR remote "power" button to enter passive standby
 *
 * 90   12/2/09 5:29p mward
 * SW7125-73: output_format command must restore display settings if
 * setOutput()is not successful.
 *
 * 89   8/3/09 10:22a mward
 * PR43001: Condition record-related brutus script commands with
 * PVR_SUPPORT.
 *
 * 87   7/27/09 11:12a jtna
 * PR56956: defer script execution until brutus init is complete
 *
 * 86   6/24/09 2:57p jtna
 * PR56069: added cmd command macrovision
 *
 * 85   6/12/09 11:12a jtna
 * PR43001: support TS/TTS record
 *
 * 84   6/1/09 7:55p jtna
 * PR43001: added record-related brutus script commands
 *
 * 83   3/19/09 12:09p jtna
 * PR45349: fix -scrtext mode
 *
 * 82   2/25/09 7:01p mward
 * PR52442:  If app() command was processed, the _scriptEngine may have
 * gone away.
 *
 * 81   2/23/09 6:14p vle
 * PR 48785: Add HDMI CEC screen to BRUTUS
 * Merge to main branch
 *
 * 80   12/1/08 10:24a jtna
 * PR49636: enable reg_rd for Linux only
 *
 * 79   11/19/08 2:30p jtna
 * PR33616: added help and option to specify offset for reg_rd cmd
 *
 * 78   11/17/08 5:03p jtna
 * PR33616: added reg_rd command
 *
 * 77   11/14/08 6:14p jtna
 * PR45349: resume script cleanly after rejecting cancellation
 *
 * HDMI_TX_Plugfest11_200811/1   11/12/08 4:45p vle
 * PR 48785: Send CEC <Standby> message when Brutus power down.
 *
 * 75   8/13/08 6:44a erickson
 * PR42789: udpate
 *
 * 74   8/13/08 5:26a erickson
 * PR42789: updates for BLUETOOTH_SUPPORT
 *
 * 73   8/13/08 5:19a erickson
 * PR42789: remove unneeded header file
 *
 * 72   8/1/08 11:02a erickson
 * PR45286: added ALLOW_INPUT_DURING_SCRIPT brutus.cfg option, enabled the
 * set(name,value) script command
 *
 * 71   7/16/08 10:09a erickson
 * PR17425: added missing success return code
 *
 * 70   7/2/08 4:37p erickson
 * PR17425: added main_window and pip_window commands
 *
 * 69   5/9/08 11:48a erickson
 * PR42329: script cmd 'repeat' needs to block until script queue empties,
 * otherwise we need filling the queue forever and will run out of
 * memory.
 *
 * 68   3/17/08 3:17p erickson
 * PR40307: remove old, unused code before adding new
 *
 * 67   2/21/08 2:12p ahulse
 * PR25542: Add commands audiolist, piclist,videolist
 *
 * 66   2/13/08 4:52p tokushig
 * PR39534: added bluetooth remote capabilities to brutus
 *
 * 65   1/30/08 11:04a gmohile
 * PR 39014 : Seperate DQT input params
 *
 * 64   12/6/07 12:52p erickson
 * PR37967: rename functions for consistency
 *
 * SanDiego_Brutus_3D/9   1/30/08 2:19p tokushig
 * refactor demo bluetooth remote implementation for integration into main
 * line
 *
 * SanDiego_Brutus_3D/8   12/21/07 5:04p tokushig
 * added <X> button on bluetooth remote...will show 3d intro before going
 * to main menu. <options> key will skip intro and just go to menu.  note
 * there is no corresponding <x> key on the allforone ir remote so this
 * option is not available...in this case the 3d intro will be seen on
 * brutus startup only
 *
 * SanDiego_Brutus_3D/7   12/21/07 2:40p tokushig
 * merged BT_REMOTE_ACCESS brutus config option
 *
 * SanDiego_Brutus_BTRemote/2   12/21/07 1:11p randyjew
 * improved bluetooth modificatiosn for allowing commands during messages
 * boxes and to be able to configure address in brutus.cfg file
 * BT_REMOTE_ADDRESS=<12_character_address>
 *
 * SanDiego_Brutus_3D/6   12/20/07 5:36p tokushig
 * disable problematic bluetooth remote buttons such as <audio> <pageup>
 * and <pagedown> for demo
 *
 * SanDiego_Brutus_3D/5   12/20/07 5:18p tokushig
 * fix errant merge problem in mapplication.cpp and remove printout from
 * inputengine.cpp
 *
 * SanDiego_Brutus_3D/4   12/20/07 4:58p tokushig
 * added support for bluetooth remote.  note that user must press and hold
 * down <enter> and <start> remote keys before starting brutus and must
 * keep them held down until graphics are displayed on screen - this is
 * so the remote can pair with the bluetooth usb dongle
 *
 * SanDiego_Brutus_BTRemote/1   12/17/07 1:27p randyjew
 * bt remote demo
 *
 * SanDiego_Brutus_3D/3   12/15/07 9:45p tokushig
 * audio working!  hardcoded music list in bTV.c
 *
 * SanDiego_Brutus_3D/2   12/14/07 5:49p tokushig
 * restrict info button on 3d homescreen
 *
 * SanDiego_Brutus_3D/1   12/14/07 3:52p tokushig
 * 3d working with brutus.  minor draw issues when going back to
 * fullscreen. and problem drawing bTV icon when going back into the
 * menu.  audio not working but prob config spec issue.
 *
 * 63   10/1/07 4:47p mward
 * PR35533:  If DUAL_OUTPUT, open UHF only for SECOND_DISPLAY.
 *
 * 62   6/7/07 10:30a erickson
 * PR30651: don't convert all pagedown/up to slow fwd/rev. instead,
 * convert just GI remote pagedown/up.
 *
 * 61   5/17/07 1:23p erickson
 * PR30310: add bplayback_player_mode_by_gop
 *
 * 60   4/25/07 1:58p erickson
 * PR29185: move powerUp code below isRepeat check to avoid POWER key
 * repeats
 *
 * 59   4/19/07 6:21p jrubio
 * PR30010: added support for http channel
 *
 * 58   4/16/07 10:29a erickson
 * PR29757: backout fix for PR 27409, which is now fixed in MScrollView
 *
 * 57   3/26/07 1:09p gmohile
 * PR 28585: fix cmdparams check for decoder trick modes.
 *
 * 56   3/26/07 1:48a erickson
 * PR28741: fix PVR_SUPPORT=n support
 *
 * 55   3/14/07 10:15a erickson
 * PR26267: refactor Brutus into separate VideoList and PlaybackScreen
 *
 * 54   3/6/07 7:44p gmohile
 * PR 27409: do not send slow rewind or slow fwd command for page up/page
 * down
 *
 * 53   2/20/07 12:25p erickson
 * PR26662: check pointer before using
 *
 * 52   2/13/07 11:33a rjlewis
 * PR17283: before saying unknown command, see if input is a channel
 * change (same as "ch(xxx)").
 *
 * 51   2/1/07 3:43p jgarrett
 * PR 20139: Removing warning
 *
 * 50   1/31/07 11:31a erickson
 * PR19687: add Standby button to brutus. call settop api functions.
 * enable POWER button to enter Standby.
 *
 * 49   1/29/07 11:52a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 *
 * 49   1/29/07 11:17a ssood
 * PR24918: Channel Map Redesign to make its access Thread-safe
 *
 * 48   1/29/07 8:47a erickson
 * PR26937: fix play(++) and play(--) wraparound
 *
 * 47   1/26/07 5:08p erickson
 * PR26661: superseding logic added to control.cpp
 *
 * 46   1/24/07 3:06p mward
 * PR26936: Check if expected params are missing, segfaults otherwise.
 *
 * 45   1/9/07 2:53p erickson
 * PR25719: added dbglevel command
 *
 * 44   1/5/07 4:51p rjlewis
 * PR26937: fixed play(-n) and show_img(-n).  Added support for "play(.)",
 * "play(.+n)", and "play(.-n)".
 *
 * 43   12/28/06 10:25a erickson
 * PR25538: added "rate" to trick command for VC1 support
 *
 * 42   12/21/06 11:45a jjordan
 * PR26764: move RTSP record request supression
 *
 * 41   12/12/06 5:31p jjordan
 * PR25142: disable "Rec" remote command during RTSP
 *
 * 40   12/5/06 11:24a erickson
 * PR25987: check if currentPvr is active before using it
 *
 * 39   12/1/06 10:07a ahulse
 * PR26122: Check if PLAYBACK_ENABLED=0 before displaying playback screen
 *
 * 38   11/30/06 6:54p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 37   11/22/06 3:56p ahulse
 * PR25543: Add script support to display image with script command
 * show_img(
 *
 * 36   11/17/06 1:13p erickson
 * PR25538: added trick command, also added parse_cmd to simpler
 * function(paren) parsing
 *
 * 35   11/2/06 11:00a arbisman
 * PR23775: Modified to use playback manager
 *
 * 34   10/30/06 3:34p ahulse
 * PR25172: If PLAYBACK_ENABLED=0 don't attempt to play
 *
 * 33   10/27/06 11:47a erickson
 * PR24374: add play(++) support, echo script commands to console
 *
 * 32   10/20/06 3:10p jrubio
 * PR24927: Added some more keys for Echostar UHF remotes
 *
 * 31   10/9/06 9:26p vle
 * PR 24633: make sure to check for permission when issue command
 * goto(admin) in brutus --exec mode.
 *
 * 30   10/9/06 1:33p gmohile
 * PR 24130: remove redundant code and fix display issue on switching
 * HD/SD Dual o/p mode
 *
 * 29   9/28/06 7:02p arbisman
 * PR24289: Back-out changes
 *
 * 27   9/6/06 2:43p erickson
 * PR21941: added output_format(--) and made -- and ++ relative to current
 * format
 *
 * 26   9/6/06 1:22p erickson
 * PR21941: added output_format(++)
 *
 * 25   6/14/06 12:18p jgarrett
 * PR 22048: Fixing RFmod mute for HDSD_SINGLE
 *
 * 24   5/9/06 11:37a jrubio
 * PR18491: Added UHF/RF remote support
 *
 * 23   5/4/06 12:05p jgarrett
 * PR 21266: Merging 3-display code to main.
 *
 * REFSW_97400_3DISPLAY/1   5/3/06 1:34p jgarrett
 * PR 21266: Power key should not work on second display
 *
 * 22   4/10/06 5:03p jgarrett
 * PR 20747: Power key should not re-assign outputs in dual display modes.
 *
 * 21   3/15/06 4:09p mphillip
 * PR19786: Initial implementation.  Add CONTINUOUS_RECORD_ENABLED=1 to
 * brutus.cfg to enable
 *
 * 20   2/28/06 10:40a jjordan
 * PR18572: merge timeshifting fix for IP STB
 *
 * 19   2/13/06 11:31a erickson
 * PR17108: refactor Brutus class to make it more sane
 *
 * 18   1/16/06 4:33p jrubio
 * PR18491: added detection of UHF_REMOTE
 *
 * 17   12/20/05 5:02p rjlewis
 * PR17283: strip trailing whitespace from end of command.  Can cause
 * command to fail strcmp.
 *
 * 16   10/10/05 3:27p erickson
 * PR17108: switch to more generic B_HAS_VDC
 *
 * 15   8/22/05 9:25a erickson
 * PR16747: only allow entertext for MLineEdit
 *
 * 14   8/12/05 10:41a jjordan
 * PR16668: add IP STB config screen to IP STB GUI
 *
 * 13   8/4/05 5:23p erickson
 * PR15137: added output_format(FORMAT) script command
 *
 * 12   7/6/05 4:40p erickson
 * PR16119: added --exec and -tty slow forward/reverse support
 *
 * 11   6/14/05 10:03a erickson
 * PR15831: restored channel scan to admin page with different
 * functionality for magnum vs. legacy
 *
 * 10   5/2/05 12:51p erickson
 * PR15138: added subchannel support to ch(#)
 *
 * 9   4/19/05 3:46p erickson
 * PR14701: make digital subchannel support standard for all platforms
 *
 * 8   4/12/05 11:47a erickson
 * PR14701: added ENTER key to terminate incremental channel entry
 *
 * 7   3/24/05 1:21p erickson
 * PR14593: removed old platform
 *
 * 6   3/15/05 10:35a erickson
 * PR11077: added POWER_TO_SCAN_CHANNELS feature to make settop better lab
 * decoder tool
 *
 * 5   2/28/05 12:26p erickson
 * PR14281: fixed screencapture command
 *
 * 4   2/24/05 3:39p erickson
 * PR14180: fix POWER output toggle
 *
 * 3   2/23/05 9:31a erickson
 * PR14180: fixed for no SCRIPT_SUPPORT
 *
 * 2   2/17/05 2:45p ywu
 * PR 13748: Create an IR keystrokes automation utility
 *
 * 1   2/7/05 8:13p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/89   2/4/05 10:47a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 *
 * Irvine_BSEAVSW_Devel/88   1/12/05 11:50a erickson
 * PR13097: 7038 power button should toggle HD/SD
 *
 * Irvine_BSEAVSW_Devel/87   12/20/04 2:06p erickson
 * PR13605: handle repeat keys before checking to stop the script
 *
 * Irvine_BSEAVSW_Devel/86   11/23/04 9:43a erickson
 * PR13336: reworked repeat key handling to allow more flexibility
 *
 * Irvine_BSEAVSW_Devel/85   10/27/04 12:16p erickson
 * PR10067: 10067
 *
 * Irvine_BSEAVSW_Devel/84   10/26/04 5:14p erickson
 * PR10067: if audio is playing, stop that first from any screen
 *
 * Irvine_BSEAVSW_Devel/83   10/14/04 1:57p erickson
 * PR12977: removed outputs from screen list
 *
 * Irvine_BSEAVSW_Devel/82   9/15/04 1:53p erickson
 * PR9683: convert from B_CHIP to BCHP_CHIP in brutus
 *
 * Irvine_BSEAVSW_Devel/81   9/13/04 1:56p erickson
 * PR11081: refactored api for controlling info panel
 *
 * Irvine_BSEAVSW_Devel/80   9/9/04 2:00p erickson
 * PR12608: converted JPEG_SUPPORT to PICTURES_SUPPORT
 *
 * Irvine_BSEAVSW_Devel/79   8/16/04 1:58p erickson
 * PR12283: added inputengine_ext.c for brutus script extensions and
 * settop api internal access
 *
 * Irvine_BSEAVSW_Devel/78   7/23/04 3:28p erickson
 * PR11765: don't allow gotos when in a modal state
 *
 * Irvine_BSEAVSW_Devel/77   6/30/04 11:45a erickson
 * PR11751: fixed quit for DUAL_OUTPUT=1
 *
 * Irvine_BSEAVSW_Devel/76   6/30/04 11:31a erickson
 * PR11751: fixed help
 *
 * Irvine_BSEAVSW_Devel/75   6/22/04 5:08p erickson
 * PR11135: reenabled slowmotion
 *
 * Irvine_BSEAVSW_Devel/74   6/11/04 3:17p erickson
 * PR11404: added MVD dump support (requires hijack)
 *
 * Irvine_BSEAVSW_Devel/73   5/27/04 9:55a erickson
 * PR11121: touched up dbg msgs
 *
 * Irvine_BSEAVSW_Devel/72   5/25/04 11:51a erickson
 * PR11135: use boutput_XX_get/set for proxy support
 *
 * Irvine_BSEAVSW_Devel/71   5/24/04 1:53p erickson
 * PR11083: added INFOPANEL_SUPPORT conditionals
 *
 * Irvine_BSEAVSW_Devel/70   5/24/04 1:48p erickson
 * PR11085: added SCRIPT_SUPPORT conditionals
 *
 * Irvine_BSEAVSW_Devel/69   5/5/04 2:45p erickson
 * PR10814: use a static variable to report termination to all apps. quit
 * wasn't working with DUAL_OUTPUT when using -scrtext or -scr. It was
 * only working for --exec where there is a single shared ScriptEngine.
 *
 * Irvine_BSEAVSW_Devel/68   4/30/04 12:05p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/67   4/22/04 10:35p erickson
 * PR10763: Fix code if AUDIO_SUPPORT not defined
 *
 * Irvine_BSEAVSW_Devel/66   4/16/04 4:18p erickson
 * PR9883: TTYInput now needs appIndex so it can switch
 *
 * Irvine_BSEAVSW_Devel/65   4/16/04 2:49p erickson
 * PR10664: added app(#) command to switch between dual output instances
 * on 97038
 *
 * Irvine_BSEAVSW_Devel/64   4/16/04 11:27a erickson
 * PR9029: convert to BrutusCommand for device-independent command
 *
 * Irvine_BSEAVSW_Devel/63   4/16/04 10:44a erickson
 * PR9029: new object-oriented input device design
 *
 * Irvine_BSEAVSW_Devel/62   4/6/04 3:48p bandrews
 * PR10368: Added goto(info) command to reset infobar plugin focus to the
    * top.  goto(tv) will now turn off info as well.
 *
 * Irvine_BSEAVSW_Devel/61   3/8/04 11:33a erickson
 * PR9883: added a little more debug
 *
 ****************************************************************************/
#include "inputengine.h"
#include "control.h"
#include "cfgsettings.h"
#include "input_sejinkbd.h"
#include "input_irremote.h"
#include "input_uhfremote.h"
#include "input_tty.h"
#include "input_keypad.h"
#include "input_btremote.h"
#include "scriptengine.h"
#include "mlineedit.h"
#include "mmessagebox.h"

#include "info_outputs.h"
#include "genericscreen.h"
#include "picturesscreen.h"
#include "tvscreen.h"
#ifdef PVR_SUPPORT
#include "playengine.h"
#include "recordengine.h"
#include "playbackscreen.h"
#include "videoscreen.h"
#include "recordscreen.h"
#include "programguide.h"
#endif
#ifdef ADMIN_SUPPORT
#include "editchannelsscreen.h"
#endif
#ifdef AUDIO_SUPPORT
#include "audioscreen.h"
#include "audiocontrol.h"
#define audioScreen()   ((AudioScreen *)control()->screen(Control::eAudio))
#endif
#include "ipstbconfigscreen.h"
#include "bsettop_save_settings.h"

#include "bstd.h"

#ifdef LINUX
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

#ifdef NEXUS_BUILD
#include "nexus_platform.h"
#include "nexus_video_decoder.h"
#include "nexus_audio_decoder.h"

#define BSETTOP_NO_API_THUNKS
extern "C" {
    #include "bsettop_types.h"
    #include "bsettop_impl.h"
}

#endif


BDBG_MODULE(input);

#define tv()                ((TVScreen *)control()->screen(Control::eTV))
#define videoScreen()       ((VideoScreen *)control()->screen(Control::eVideo))
#define recordScreen()      ((RecordScreen *)control()->screen(Control::eRecord))
#define playbackScreen()    ((PlaybackScreen *)control()->screen(Control::ePlayback))
#define pic()               ((PicturesScreen *)control()->screen(Control::ePictures))
#define editChannelsScreen()((EditChannelsScreen*)control()->screen(Control::eEditChannels))
#define ipStbConfigScreen() ((EditChannelsScreen*)control()->screen(Control::eIpStbConfig))

#define cfg() _control->cfg()

extern "C" {
/* This function also the script interface to be customized. Just override the
inputengine_ext.c file and provide your own function. */
int inputengine_ext(const char *text);
}

static int string2channel(MString string) {
    int ch = atoi(string); /* atoi will ignore numbers after . */
    /* look for optional subchannel and transform ch to handle it */
    char *sub = strchr(string, '.');
    if (sub) {
        int subch = atoi(sub+1);
        if (subch) subch--;
        ch = MAKESUBCH(ch, subch);
    }
    return ch;
}

/* set uniform volume to all audio output devices */
static bresult set_volume(bdisplay_t display, const baudio_volume *volume)
{
    bdisplay_settings settings;
    bresult result;

    bdisplay_get(display, &settings);

    result = bdisplay_set_dac_audio_volume(display, volume);
    if (result) return result;

    if (settings.rf) {
        result = boutput_rf_set_audio_volume(settings.rf, volume);
        if (result) return result;
    }
    if (settings.spdif) {
        result = boutput_spdif_set_audio_volume(settings.spdif, volume);
        if (result) return result;
    }
    return b_ok;
}

InputEngine::InputEngine(Control *control) :
    MWidget(control->fb()), _timer(control->fb()->app())
{
#ifdef SCRIPT_SUPPORT
    _timer.addHandler(this);
    _timer.start(0, MTimer::eForever); // idle time processing
#endif

    ready = false;
    _scriptEngine = NULL;
    _control = control;

    /* bring up the standard input devices, based on Config options */

    /* By added the InputDevice objects to an MAutoList, they will be freed
    when InputEngine is destroyed. */

    if (cfg()->getBool(Config::REMOTE_ENABLED))
    {
        /* pick the settop api for the type of remote we're going to use */
         if (cfg()->getBool(Config::NEC_REMOTE)) {
            _devices.add(new RemoteDInput(app(), this));
        }else if (cfg()->getBool(Config::SA_REMOTE))
            _devices.add(new RemoteBInput(app(), this));
        else if(cfg()->getBool(Config::IRECHO_REMOTE))
            _devices.add(new RemoteCInput(app(), this));
        else
            _devices.add(new RemoteAInput(app(), this));

       
     
        if (!(cfg()->getBool(Config::DUAL_OUTPUT) && !cfg()->getBool(Config::SECOND_DISPLAY)))
        {
            if (cfg()->getBool(Config::ECHO_REMOTE))
                _devices.add(new uhfEchostar(app(), this));
            else if (cfg()->getBool(Config::DRCTV_REMOTE))
                _devices.add(new uhfDirecTV(app(), this));
        }

#if BLUETOOTH_SUPPORT
        if (cfg()->get(Config::BT_REMOTE_ADDRESS)) {
            _devices.add(new BluetoothInput(app(),this));
        }
#endif
    }

#ifndef IR_AUTO_ENABLE
    if (cfg()->getBool(Config::SECOND_DISPLAY) ==
        cfg()->getBool(Config::FRONTPANEL_FOR_SECOND_DISPLAY))
    {
        _devices.add(new KeypadInput(control, app(), this));
    }

    if (cfg()->getBool(Config::SECOND_DISPLAY) ==
        cfg()->getBool(Config::IRKEYBOARD_FOR_SECOND_DISPLAY))
    {
        _devices.add(new SejinIRKeyboardInput(app()));
    }
#endif

#ifdef LINUX
    if (cfg()->getBool(Config::TTY))
    {
        _devices.add(new TTYInput(control->secondDisplay(), app(), this));
    }
#endif
}

bool InputEngine::keyDown(int ascii, int modifiers, int scancode) {
    // Some special keys are translated into normal keys by processCmd.
    // These must be processed here to avoid an infinite loop. An example
    // is IND_0 which is translated into '0'. This allows the remote control
    // to input 0-9 into edit boxes.
    switch (ascii) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        control()->addChannelDigit(ascii-'0');
        return true;
    case '`':
        if (cfg()->getBool(Config::TTY)) {
            app()->tab(MWidget::tabPrev);
            return true;
        }
        break;
    case 'j':
        app()->sendKeyDown(app()->focusedWidget(), MWKEY_DOWN, 0, 0);
        return true;
    case 'k':
        app()->sendKeyDown(app()->focusedWidget(), MWKEY_UP, 0, 0);
        return true;
    case 'h':
        app()->sendKeyDown(app()->focusedWidget(), MWKEY_LEFT, 0, 0);
        return true;
    case 'l':
        app()->sendKeyDown(app()->focusedWidget(), MWKEY_RIGHT, 0, 0);
        return true;
    case MWKEY_F1: ascii = IND_STOP; break;
    case MWKEY_F2: ascii = IND_PAUSE; break;
    case MWKEY_F3: ascii = IND_PLAY; break;
    case MWKEY_F4: ascii = IND_SLOW_FWD; break;
    case MWKEY_F5: ascii = IND_REW; break;
    case MWKEY_F6: ascii = IND_FWD; break;
    case MWKEY_F7: ascii = IND_RECORD; break;
    //case MWKEY_F8: ascii = IND_STOP; break;
    case MWKEY_F9: ascii = IND_TV; break;
    case MWKEY_F10: ascii = IND_MENU; break;
    case MWKEY_F11: ascii = IND_GUIDE; break;
    case MWKEY_F12: ascii = IND_INFO; break;
    case MWKEY_SYSREQ: ascii = IND_TOGGLE_VIDEO; break; /* swap main and pip */
    case MWKEY_SCROLLOCK: ascii = IND_TOGGLE_AUDIO; break; /* move control between main and pip */
    case '-': ascii = IND_CHAN_DOWN; break;
    case '=':
    case '+': ascii = IND_CHAN_UP; break;
    case '[': ascii = IND_VOL_DOWN; break;
    case ']': ascii = IND_VOL_UP; break;
    case '\\': ascii = IND_MUTE; break;
    case MWKEY_RMETA: ascii = IND_TOGGLE_CTRL; break; /* bring pip up and down */
    default:
        return false;
    }

    return processCmd((BrutusCommand)ascii);
}

bool InputEngine::processCmd(BrutusCommand cmd, bool isScriptCmd, bool isRepeat) {
    // cache some control() ptrs
    int currentScreen = control()->currentScreen();
#ifdef PVR_SUPPORT
    PlayEngine *currentPvr = control()->decode() ? control()->decode()->playEngine() : NULL;

#if defined(LIVEMEDIA_SUPPORT)
    ChannelMap::Channel currentChannel;
        if(!control()->channelMap()->getChannel(control()->channel(), &currentChannel) &&
           currentChannel.entry.getType() == btuner_ip_stream &&
           currentChannel.entry.ip.params.open_params.protocol == bsettop_ip_protocol_rtsp ||
           currentChannel.entry.ip.params.open_params.protocol == bsettop_ip_protocol_http )
        {
                /* Brutus can get here before playback started, check for stream */
                if( control()->getLiveStream() )
                        currentPvr = control()->getLiveStream()->playEngine();
        }
#endif
#endif

    BDBG_MSG(("IND command: %d", cmd));

    /**
    This is where we cut out repeat keys...unless they are explicitly
    requested by the UI. When a control gains focus and wants repeat
    keys, it should set the Control global. This way it stays out of mlibs.
    **/
    if (isRepeat && !control()->testRepeatKey(cmd)) {
        return false;
    }

    /* check for power state wakeup */
    if (control()->brutus()->powerUp()) {
        /* consume the keystroke */
        return true;
    }

#ifdef SCRIPT_SUPPORT
    if (!isScriptCmd) {
        if (_scriptEngine && _scriptEngine->isActive() && !cfg()->getBool(Config::ALLOW_INPUT_DURING_SCRIPT)) {
            if (_scriptEngine->promptToStop())
                /* If we stopped, then throw away the command that caused it to stop */
                return true;
        }
    }
#endif

#ifdef PICTURES_SUPPORT
    // try the pictures screen first
    if (currentScreen == Control::ePictures) {
        if (pic()->processCommand(cmd))
            return true;
    }
#endif

    // these events are allowable even with a modal dialog up
    switch (cmd) {
    case IND_POWER:
        if (control()->getStandbyState()==brutus_standby_state_eActiveStandby || 
            control()->getStandbyState()==brutus_standby_state_ePassiveStandby || 
            control()->getStandbyState()==brutus_standby_state_eDeepSleep) 
        {
            control()->standby(brutus_standby_state_eOn);
        } 
        else {
            control()->standby(brutus_standby_state_ePassiveStandby);
        }
        return true;
    case IND_UP:
        app()->sendKeyDown(app()->focusedWidget(), MWKEY_UP, 0, 0);
        return true;
    case IND_DOWN:
        app()->sendKeyDown(app()->focusedWidget(), MWKEY_DOWN, 0, 0);
        return true;
    case IND_LEFT:
        app()->sendKeyDown(app()->focusedWidget(), MWKEY_LEFT, 0, 0);
        return true;
    case IND_RIGHT:
        app()->sendKeyDown(app()->focusedWidget(), MWKEY_RIGHT, 0, 0);
        return true;
    case IND_SELECT:
        app()->sendKeyDown(app()->focusedWidget(), MWKEY_ENTER, 0, 0);
        return true;
    case IND_PAGEUP:
        app()->sendKeyDown(app()->focusedWidget(), MWKEY_PAGEUP, 0, 0);
        return true;
    case IND_PAGEDOWN:
        app()->sendKeyDown(app()->focusedWidget(), MWKEY_PAGEDOWN, 0, 0);
        return true;
    case IND_0:
    case IND_1:
    case IND_2:
    case IND_3:
    case IND_4:
    case IND_5:
    case IND_6:
    case IND_7:
    case IND_8:
    case IND_9:
        app()->sendKeyDown(app()->focusedWidget(), cmd-IND_0+'0', 0, 0);
        return true;
    case IND_CHAN_UP:
        if (app()->focusedWidget() && !strcmp(app()->focusedWidget()->className(), "MLineEdit"))
        {
            ((MLineEdit*)app()->focusedWidget())->del();
            return true;
        }
        break;
    case IND_CHAN_DOWN:
        if (app()->focusedWidget() && !strcmp(app()->focusedWidget()->className(), "MLineEdit"))
        {
            ((MLineEdit*)app()->focusedWidget())->backspace();
            return true;
        }
        break;
    default: break; /* for compiler warnings */
    }

    // anything after this requires that we not be in modal state
    if (app()->inModalState())
        return false;

    switch (cmd) {
    case IND_MENU:
        if (currentScreen == Control::eHome)
            control()->showScreen(Control::eTV);
        else
            control()->showScreen(Control::eHome);
        break;
    case IND_TV: // exit
        if (currentScreen != Control::eTV)
            control()->showScreen(Control::eTV);
        else
            tv()->hideMessage();
        break;
#ifdef INFOPANEL_SUPPORT
    case IND_INFO:
        if (currentScreen != Control::eTV) {
            control()->showScreen(Control::eTV);
            tv()->showMessage(true);
        }
        else {
            if (tv()->isInfoVisible())
                tv()->hideMessage();
            else {
                tv()->showMessage(true);
            }
        }
        break;
#endif
    case IND_DVR:
    case IND_GUIDE:
        if (currentScreen == Control::ePlayback)
            control()->showScreen(Control::eTV);
        else if (cfg()->getBool(Config::PLAYBACK_ENABLED))
            control()->showScreen(Control::ePlayback);
        break;
#ifdef PVR_SUPPORT
    case IND_RECORD:
        if (recordScreen()) {
            if (recordScreen()->visible()) {
                if (recordScreen()->canRecord()) {
                    recordScreen()->startRecord();
                }
            }
            else if (cfg()->getBool(Config::RECORD_ENABLED))
                control()->showScreen(Control::eRecord);
        }
        break;
    case IND_STOP:
#ifdef AUDIO_SUPPORT
        if (currentScreen == Control::eAudio ||
            (control()->audioControl() && control()->audioControl()->isPlaying())) {
            audioScreen()->stopAudio();
        }
        else
#endif
        if (currentScreen == Control::ePlayback) {
            Video *vid = playbackScreen()->currentVideo();
            if (vid && vid->playRefCnt) {
                int cnt = 0;
                /* There could be multiple playbacks */
                while (vid->playRefCnt) {
                    control()->stopPlayback(vid, true);

                    /* if something goes wrong, don't loop forever */
                    if (++cnt > 5)
                        break;
                }
            }
            else if (vid && vid->record) {
                control()->stopRecord(vid);
            }
        }
        else {
            /* stop either playback or record */
            control()->stopCommand();
        }
        break;
    case IND_PAUSE:
        control()->pauseCommand();
        break;
    case IND_PLAY:
#ifdef AUDIO_SUPPORT
        if (currentScreen == Control::eAudio) {
            audioScreen()->playAudio();
        }
        else
#endif
        if (currentScreen == Control::ePlayback) {
            if (playbackScreen()->selectVideo(true))
                control()->showScreen(Control::eTV);
        }
        else if (currentScreen == Control::eVideo) {
            videoScreen()->playVideo();
        }
        else {
            if (currentPvr && currentPvr->isActive())
                currentPvr->play();
        }

        break;
    case IND_REW:
    case IND_FWD:
    case IND_SKIPFWD:
    case IND_SKIPBCK:
    case IND_SLOW_FWD:
    case IND_SLOW_REW:
        {
            int dir = (cmd==IND_REW||cmd==IND_SLOW_REW||cmd==IND_SKIPBCK)?-1:1;
            int fast = (cmd==IND_REW||cmd==IND_FWD||cmd==IND_SKIPFWD)?1:0;
            control()->trickCommand(fast,dir);
        }
        break;
    case IND_FRAME_ADV:
        if (currentPvr && currentPvr->isActive()) {
            currentPvr->frameadvance(1);
        }
        break;
    case IND_FRAME_REW:
        if (currentPvr && currentPvr->isActive()) {
            currentPvr->frameadvance(-1);
        }
        break;
    case IND_CATCHUP:
        if (currentPvr && currentPvr->isActive()) {
            currentPvr->catchup();
        }
        break;
#else /* PVR_SUPPORT */
    case IND_STOP:
#ifdef AUDIO_SUPPORT
        if (currentScreen == Control::eAudio ||
            (control()->audioControl() && control()->audioControl()->isPlaying())) {
            audioScreen()->stopAudio();
        }
#endif
        break;
    case IND_PLAY:
#ifdef AUDIO_SUPPORT
        if (currentScreen == Control::eAudio) {
            audioScreen()->playAudio();
        }
#endif
        break;
#endif /* PVR_SUPPORT */

    case IND_TOGGLE_CTRL: control()->togglePip(); break;
    case IND_TOGGLE_AUDIO: control()->moveWindowControl(); break;
    case IND_TOGGLE_VIDEO: control()->swapWindows(); break;

    case IND_MUTE:
    case IND_VOL_DOWN:
    case IND_VOL_UP:
        {
        baudio_volume vol;
        bdisplay_get_dac_audio_volume(control()->display()->display, &vol);

        switch (cmd) {
        case IND_MUTE:     vol.muted = !vol.muted; break;
        case IND_VOL_DOWN: vol.left = vol.right -= 20; vol.muted = false; break;
        case IND_VOL_UP:   vol.left = vol.right += 20; vol.muted = false; break;
        default: break; /* for compiler warnings */
        }

        set_volume(control()->display()->display, &vol);
        if ( control()->display()->display2 )
        {
            /* PR 22048: Must set display2 for HDSD_SINGLE (contains RF) */
            set_volume(control()->display()->display2, &vol);
        }
        control()->showVolumeMessage();
        }
        break;

    case IND_CHAN_UP:
#ifdef ADMIN_SUPPORT
        if (currentScreen == Control::eEditChannels)
            editChannelsScreen()->rearrange(-1);
        else
#endif
#ifdef AUDIO_SUPPORT
        if (currentScreen == Control::eAudio)
            app()->sendKeyDown(app()->focusedWidget(), MWKEY_PAGEUP, 0, 0);
        else
#endif
            control()->changeChannel(1);
        break;
    case IND_CHAN_DOWN:
#ifdef ADMIN_SUPPORT
        if (currentScreen == Control::eEditChannels)
            editChannelsScreen()->rearrange(1);
        else
#endif
#ifdef AUDIO_SUPPORT
        if (currentScreen == Control::eAudio)
            app()->sendKeyDown(app()->focusedWidget(), MWKEY_PAGEDOWN, 0, 0);
        else
#endif
            control()->changeChannel(-1);
        break;
    case IND_MUSIC:
        if (cfg()->getBool(Config::MP3_ENABLED))
            control()->showScreen(Control::eAudio);
        break;
    case IND_ENTER:
        control()->setIncrementalChannel();
        break;
    default:
        return false;
    }
    return true;
}


const char *script_conversion[] = {
    "record", "rec",
    "up", "u",
    "down", "d",
    "right", "r",
    "left", "l",
    "select", "s",
    "guide", "playback",
    NULL
};

void printHelp() {
    BKNI_Printf(
    "\nNavigation:\n"
    "  goto(SCREEN)    - goto a screen\n"
    "    SCREEN: tv,menu,playback,record,admin,mp3s,pictures\n"
    "  u[p],d[own],r[ight],l[eft],s[elect]   - moving the focus box\n"
    "\n"
    "General A/V commands:\n"
    "  ch(#)           - select a live channel. supports #.# as well.\n"
    "  scan            - rescan or refresh channel list\n"
    "  vol(#), mute    - volume controls\n"
    "  pip             - toggle PIP\n"
    "  move            - switch control between Main and PIP\n"
    "  swap            - swap context between Main and PIP\n"
    "  info            - toggle the info panel\n"
    "  app(#)          - switch between application instances if DUAL_OUTPUT is on\n"
    "  output_format(FORMAT) - change output format of the display.\n"
    "    FORMAT: 480i,480p,720p,1080i,576i,etc.\n"
    "    FORMAT: ++ and -- is also supported for iteration.\n"
    "  macrovision(TYPE)\n"
    "    TYPE: none,type1,type2,type3,test1,test2\n"
    "  st              - print decoder status\n"
    "\n"
#ifdef PVR_SUPPORT
    "PVR:\n"
    "  play(INDEX)         - select a stream and start playback\n"
    "    INDEX: index of stream in \"Playback\" screen\n"
    "    INDEX: ++ and -- is also supported for iteration.\n"
    "  rew, ff, slow, slowrew, pause, play, stop    - PVR trickmodes and control\n"
    "  rec             - goto the record screen\n"
    "  trick(MODE,MODIFIER,SLOWMOTIONRATE,DECODER_MODE)   Set trick mode\n"
    "    MODE = Normal,SkipB,IP,SkipP,I,BCM,gop,rate,TimeSkip\n"
    "    DECODER_MODE = all, IP, I\n"
    "    \"rate\" requires MODIFIER to BPLAYPUMP_NORMALPLAY_SPEED based. See Settop API.\n"
    "  videolist       - lists out video files available for playback.\n"
    "  record_start(channel, output_file/default, TS/TTS, none/AES/3DES)\n"
    "  record_stop( /channel/all) #last/channel/all\n"
    "  delete_stream(index)\n"
    "\n"
#endif
    "General Control:\n"
    "  prompt(TEXT)    - pause the script until the user presses ENTER. (TEXT) is optional.\n"
    "  screencapture   - write contents of screen to brutus_capture.dat\n"
    "  reg_rd(hexval)  - print out value of register at address hexval\n"
    "  sleep SECONDS\n"
    "  help, ?\n"
    "  dbglevel(module,LEVEL) - LEVEL is msg,wrn,err\n"
    "  quit\n"
    );
}

int InputEngine::queueScriptCmd(const char *cmd) {
    _scriptCmd.add(cmd); // to the end
    return 0;
}

int InputEngine::scriptCmdTotal() const {
    return _scriptCmd.total();
}

#include <ctype.h> // for isspace

/* trim_leading_trailing_whitespace will modify the string in place */
static char *trim_leading_trailing_whitespace(char *start)
{
    char *temp;

    /* trim leading */
    start += strspn(start, " \t");

    /* trail trailing */
    temp = start;
    while (1) {
        /* skip non-whitespace */
        temp = strpbrk(temp, " \t");
        if (!temp) {
            /* no whitespace at end */
            break;
        }
        else {
            char *end = temp;
            temp += strspn(temp, " \t");
            if (!*temp) {
                *end = 0;
                break;
            }
            /* more after whitespace */
        }
    }
    return start;
}


/* parse_cmd:
Input:
    cmd = function(paren1,paren2,...)
Output:
    cmdname = function
    cmdparams = list of {paren1, paren2, ...}
*/
static void parse_cmd(const char *cmd, MString &cmdname, MStringList &cmdparams)
{
    const char *paren = strchr(cmd, '(');
    if (paren) {
        cmdname.strncpy(cmd, paren-cmd);

        /* parse function params */
        MString params;
        paren++;
        const char *closeparen = strchr(cmd, ')');
        if (closeparen) {
            params.strncpy(paren, closeparen-paren);
        }
        else {
            params = paren;
        }
        MStringList::split(params, ",", cmdparams);

        /* trim whitespace before & after each param */
        for (int i=0;i<cmdparams.total();i++) {
            cmdparams[i] = trim_leading_trailing_whitespace((char *)cmdparams[i].s());
        }
    }
    else {
        cmdname = cmd;
    }
}

int InputEngine::runScriptCmd(const char *const_cmd)
{
    // don't allow NULL
    BDBG_ASSERT(const_cmd);

    /* copy the string so we can modify in this function */
    MString copy_cmd = const_cmd;
    char *cmd = (char *)copy_cmd.s(); /* we will modify inside this memory, but it's local */

    // remove line comment
    char *comment = strchr(cmd, '#');
    if (comment) *comment = 0;

    cmd = trim_leading_trailing_whitespace(cmd);
    // if after that we have nothing, be done
    if (!*cmd) return 0;

    BDBG_WRN(("script: %s", cmd));

    // convert some
    for (int i=0;script_conversion[i];i+=2) {
        if (!strcmp(script_conversion[i], cmd)) {
            copy_cmd = script_conversion[i+1]; /* must copy before assigning. don't want to overwrite our const storage. */
            cmd = (char *)copy_cmd.s();
        }
    }

    /* parse function and parameters */
    MString cmdname;
    MStringList cmdparams;
    parse_cmd(cmd, cmdname, cmdparams);

    if (cmdname == "quit") {
        fclose(stdin); // this allows interactive_mode to quit
        app()->terminate(); // this kills all microwidgets message loops
        return 0;
    }
    else if (cmdname == "goto") {
        // gonna check "info" first, since it's not really a screen
        if (cmdparams[0] == "info")
        {
            // tv must be showing to show info
            if (control()->screen(Control::eTV))
            {
                control()->showScreen(Control::eTV);
            }
#ifdef INFOPANEL_SUPPORT
            tv()->showMessage(true);
#endif
            // focus at top of info
            control()->screen(Control::eTV)->focusUpperLeft();
        }
        else
        {
#ifdef INFOPANEL_SUPPORT
            if (cmdparams[0] == "tv") {
                tv()->hideMessage();
            }
#endif

            int i;
            for (i=0;i<Control::eTotalScreens;i++) {
                Control::Screen scr = (Control::Screen)i;
                if (Control::screenName(scr) == cmdparams[0]) {
                    // if the screen isn't allocated, then it isn't enabled
                    if (control()->screen(scr)) {
                        if (cmdparams[0] == "admin") {
                            if (control()->verifyAdminPassword()) {
                                control()->showScreen(scr);
                                control()->screen(scr)->focusUpperLeft();
                            }
                        }
                        else {
                            if ( cmdparams[0] == "playback" && !cfg()->getBool(Config::PLAYBACK_ENABLED) ) {
                                /* If RECORD_ENABLED=1, playback screen will still be in list of screens */
                                /* Don't display if PLAYBACK_ENABLED=0 */
                                BDBG_WRN(("Screen disabled in brutus.cfg !"));
                            }
                            else {
                                control()->showScreen(scr);
                                control()->screen(scr)->focusUpperLeft();
                            }
                        }
                    }
                    else {
                        BDBG_ERR(("Screen not available"));
                    }
                    break;
                }
            }
            if (i == Control::eTotalScreens) {
                BDBG_ERR(("Invalid screen: %s", cmdparams[0].s()));
            }
        }
        return 0;
    }
#ifdef PVR_SUPPORT
    else if (cmdname == "play" && cmdparams.total() > 0) {
        Video *video=NULL;

        if ( !cfg()->getBool(Config::PLAYBACK_ENABLED) ) {
            BDBG_WRN(("  Playback is disabled in brutus.cfg !"));
            return -1;
        }

        static int current_play_index = -1; /* keep track of last one played using this command.
            start with -1 so that first ++ starts with 0. */

        if (cmdparams[0] == ".") {
        /* don't do anything -- play last selected progrqam */
        }
        else if (*cmdparams[0] == '.') {
            char *value = (char*)cmdparams[0].s();
            current_play_index += atoi(++value);
        }
        else if (cmdparams[0] == "++") {
            current_play_index++;
            if (current_play_index >= control()->videos()->total()) {
                current_play_index = 0;
            }
        }
        else if (cmdparams[0] == "--") {
            current_play_index--;
            if (current_play_index < 0) {
                current_play_index = control()->videos()->total() - 1;
            }
        }
        else {
            /* Try to find name in list of video files */
            video = control()->videos()->findMedia( cmdparams[0].s() );

            if( !video ) {
                /* Not a video file, so treat as a number */
                current_play_index = atoi(cmdparams[0]); // I don't care about the remainder
                if (current_play_index > 0) current_play_index--; // convert to 0 based
            }
            else
                goto GotVideo;
        }

        /* if its negative index, index from the end of the list */
        if (current_play_index < 0) {
            current_play_index = control()->videos()->total() - abs(current_play_index);
        }
        /* if they entered play(-1000) it'll go off the list -- jump to the first video */
        if (current_play_index < 0) {
            current_play_index = 0;
        }
        /* if the entered play(1000) it'll go off the list -- jump to the last video */
        if (current_play_index >= control()->videos()->total()) {
            current_play_index = control()->videos()->total() - 1;
        }

        video = control()->videos()->get(current_play_index);

GotVideo:
        if (video) {
            if (control()->playVideo(video))
                control()->showScreen(Control::eTV);
        }
        return 0;
    }
    else if (cmdname == "videolist"  ) {
        int idx;
        const char *pMediaName;

        /* Dump out list of files */
        for( idx=0; idx < control()->videos()->total(); idx++ ) {
            pMediaName = control()->videos()->getMediaFileName(idx);
            BKNI_Printf( "%4d  %s\n", idx+1, pMediaName );
        }
    }
    else if (cmdname == "main_window" && cmdparams.total() >= 4) {
        control()->setWindowOverride(0,
            atoi(cmdparams[0]), atoi(cmdparams[1]), atoi(cmdparams[2]),atoi(cmdparams[3]));
        return 0;
    }
    else if (cmdname == "pip_window" && cmdparams.total() >= 4) {
        control()->setWindowOverride(1,
            atoi(cmdparams[0]), atoi(cmdparams[1]), atoi(cmdparams[2]),atoi(cmdparams[3]));
        return 0;
    }
    else if (cmdname == "trick") {
        PlayEngine *playEngine = control()->playEngine();
        if (!playEngine) return -1;

        bplayback_trickmode_params trickmode_params;
        bplayback_trickmode_params_init(&trickmode_params);

        if (!cmdparams[0].strncasecmp("SkipB"))
            trickmode_params.mode = bplayback_player_mode_skip_b;
        else if (!cmdparams[0].strncasecmp("SkipP"))
            trickmode_params.mode = bplayback_player_mode_skip_p;
        else if (!cmdparams[0].strncasecmp("IP"))
            trickmode_params.mode = bplayback_player_mode_ip;
        else if (!cmdparams[0].strncasecmp("I"))
            trickmode_params.mode = bplayback_player_mode_i;
        else if (!cmdparams[0].strncasecmp("BCM"))
            trickmode_params.mode = bplayback_player_mode_brcm;
        else if (!cmdparams[0].strncasecmp("gop"))
            trickmode_params.mode = bplayback_player_mode_by_gop;
        else if (!cmdparams[0].strncasecmp("rate"))
            trickmode_params.mode = bplayback_player_mode_by_rate;
        else if (!cmdparams[0].strncasecmp("TimeSkip"))
            trickmode_params.mode = bplayback_player_mode_time_skip;
        else
            trickmode_params.mode = bplayback_player_mode_normal;

        if (cmdparams.total() > 1) {
            trickmode_params.mode_modifier = atoi(cmdparams[1]);
        }

        if (cmdparams.total() > 2) {
            trickmode_params.slow_motion_rate = atoi(cmdparams[2]);
        }

        if (cmdparams.total() > 3) {
            if (!cmdparams[3].strncasecmp("IP"))
                trickmode_params.decode_mode = bplaypump_decode_mode_ip;
            else if (!cmdparams[3].strncasecmp("I"))
                trickmode_params.decode_mode = bplaypump_decode_mode_i;
            else
                trickmode_params.decode_mode = bplaypump_decode_mode_all;
        }

        if (cmdparams.total() > 4) {
            if(trickmode_params.mode == bplayback_player_mode_by_gop) {
                trickmode_params.gop_skip_count = atoi(cmdparams[4]);
            }
            else{
                BDBG_WRN(("GOP SKIP COUNT IS TO BE USED ONLY FOR DQT"));
                trickmode_params.gop_skip_count = 0;
            }
        }

        playEngine->setTrickMode(&trickmode_params);
        return 0;
    }
#endif
#ifdef AUDIO_SUPPORT
    else if (cmdname == "audiolist"  ) {
        int idx;
        const char *pMediaName;
        int currentScreen = control()->currentScreen();

        if (currentScreen != Control::eAudio) {
            /* Need audio screen in order to generate audio filelist ! */
            control()->showScreen(Control::eAudio);
        }
        /* List all audio files in current dir */
        audioScreen()->listAll();
    }
    else if ( strstr(cmd, "play_aud(") == cmd  ) {
        int currentScreen = control()->currentScreen();

        if (currentScreen != Control::eAudio) {
            /* Need audio screen in order to play audio file ! */
            control()->showScreen(Control::eAudio);
        }
        cmd += 9;
        audioScreen()->playAudio( atoi(cmd) );
        return 0;
    }
#endif
    else if (strstr(cmd, "show_img(") == cmd) {
        static int current_pic_index = -1; /* keep track of last one displayed using this command.
                                              start with -1 so that first ++ starts with 0. */
        int currentScreen = control()->currentScreen();

        cmd += 9;

        if (currentScreen != Control::ePictures) {
            /* Need pictures screen in order to display pictures */
            control()->showScreen(Control::ePictures);
        }

        if (strstr(cmd, "++") == cmd) {
            current_pic_index++;
        }
        else if (strstr(cmd, "--") == cmd) {
            current_pic_index--;
        }
        else {
            if( !pic()->showPicture( cmd ) )
                return 0;


            current_pic_index = atoi(cmd); // I don't care about the remainder
            if (current_pic_index > 0) current_pic_index--; // convert to 0 based
        }

        /* if its negative index, index from the end of the list */
        if (current_pic_index < 0) {
            current_pic_index = pic()->totalPictures() - abs(current_pic_index);
        }
        /* if they entered show_img(-1000) it'll go off the list -- jump to the first picture */
        if (current_pic_index < 0) {
            current_pic_index = 0;
        }
        /* if they entered show_img(1000) it'll go off the list -- jump to the last picture */
        if (current_pic_index >= pic()->totalPictures()) {
            current_pic_index = pic()->totalPictures() - 1;
        }

        pic()->showPicture(current_pic_index);
        return 0;
    }
    else if (cmdname == "piclist"  ) {
        int idx;
        const char *pMediaName;
        int currentScreen = control()->currentScreen();

        if (currentScreen != Control::ePictures) {
            /* Need pictures screen in order to generate pictures filelist */
            control()->showScreen(Control::ePictures);
        }

        /* Dump out list of files */
        for( idx=0; idx < pic()->totalPictures(); idx++ ) {
            pMediaName = pic()->getMediaFileName(idx);
            BKNI_Printf( "%4d  %s\n", idx+1, pMediaName );
        }
    }
    else if (cmdname == "output_format") {
        bdisplay_settings display_settings;
        bdisplay_extended_settings savesettings;

        control()->getOutput(&display_settings);
        bdisplay_save_settings(&savesettings, &display_settings);

        if(!cmdparams.total())
        {
            BDBG_ERR(("  Invalid params"));
            return -1;
        }

        /* look for increment */
        if (cmdparams[0] == "++" || cmdparams[0] == "--") {
            int current_format = OutputsInfoBarPlugin::getFormatIndex(&display_settings);

            if (current_format == -1) {
                BDBG_ERR(("Unable to recognize current format. Resetting to NTSC."));
                current_format = 0;
            }
            else {
                current_format += (cmdparams[0] == "++")?1:-1;
                if (current_format < 0)
                    current_format = OutputsInfoBarPlugin::totalFormats()-1;
                else if (current_format >= OutputsInfoBarPlugin::totalFormats())
                    current_format = 0;
            }

            if (!OutputsInfoBarPlugin::setFormat(current_format, &display_settings)) {
                if (!control()->setOutput(&display_settings)) {
                    bdisplay_restore_settings(&display_settings, &savesettings);
                    _control->setOutput(&display_settings);
                }
            }
            return 0;
        }

        if (!OutputsInfoBarPlugin::setFormat(cmdparams[0], &display_settings)) {
            if (!control()->setOutput(&display_settings)) {
                bdisplay_restore_settings(&display_settings, &savesettings);
                _control->setOutput(&display_settings);
            }
        }
        return 0;
    }
    else if (cmdname == "ch") {
        if(!cmdparams.total())
        {
            BDBG_ERR(("  Invalid params"));
            return -1;
        }
        int ch = string2channel(cmdparams[0]);
        control()->setChannel(ch);
        return 0;
    }
    else if (cmdname == "vol") {
        baudio_volume vol;
        int n;
        if(!cmdparams.total())
        {
            BDBG_ERR(("  Invalid params"));
            return -1;
        }
        bdisplay_get_dac_audio_volume(control()->display()->display, &vol);
        n = atoi(cmdparams[0]);
        // convert to settop api level
        vol.left = vol.right = n * 20 - 100;
        vol.muted = false;
        set_volume(control()->display()->display, &vol);
        if ( control()->display()->display2 )
        {
            /* PR 22048: Must set display2 for HDSD_SINGLE (contains RF) */
            set_volume(control()->display()->display2, &vol);
        }
        control()->showVolumeMessage();
        return 0;
    }
    else if (cmdname == "screencapture") {
        /* capture the current framebuffer to file */
        const char *filename = "brutus_screen.dat";
        BDBG_WRN(("script: Writing screen to %s", filename));
        control()->brutus()->screenCapture(filename);
        return 0;
    }
    else if (cmdname == "dbglevel") {
        BDBG_Level level = BDBG_eWrn; // default
        if(cmdparams.total() < 2)
        {
            BDBG_ERR(("  Invalid params"));
            return -1;
        }
        if (!cmdparams[1].strncasecmp("msg")) {
            level = BDBG_eMsg;
        }
        else if (!cmdparams[1].strncasecmp("err")) {
            level = BDBG_eErr;
        }
        (void)BDBG_SetModuleLevel(cmdparams[0], level);
        return 0;
    }
    else if (cmdname == "set") {
        cfg()->set(cmdparams[0],cmdparams[1]);
        return 0;
    }
    else if (cmdname == "entertext") {
        MWidget *widget = app()->focusedWidget();
        if (widget && !strcmp(widget->className(), "MLineEdit")) {
            widget->setText(cmdparams[0]);
        }
        return 0;
    }
    else if (cmdname == "help" || cmdname == "?") {
        printHelp();
        return 0;
    }
    else if (cmdname == "scan") {
        control()->scan();
        return 0;
    }
#ifdef LINUX
    else if (cmdname == "reg_rd") { /* reg_rd expects the physical address OR offset of the register in hex */
        int reg, fd;
        unsigned long base_address;
        if(!cmdparams.total())
        {
            BDBG_ERR(("  Invalid params"));
            return -1;
        }
        sscanf(cmdparams[0], "%x", &reg);
        #define START_ADDRESS 0x10000000
        if (reg < START_ADDRESS) { /* if given value less than START_ADDRESS, then this is an offset */
            reg += START_ADDRESS;
        }

        fd = open("/dev/mem",O_RDONLY|O_SYNC); /* O_SYNC is for non cached access. */
        if (fd==-1) {
            BDBG_ERR(("Error opening /dev/mem"));
            return -1;
        }

        base_address = (unsigned long)mmap(0, reg+4, PROT_READ, MAP_SHARED, fd, 0);
        if ((int)base_address != -1) {
            uint32_t val = *(volatile uint32_t *)(base_address + reg);
            BDBG_WRN(("reg_rd(address=0x%x) = 0x%08x = %d", reg, val, val));
            munmap((void*)base_address, reg+4);
            close(fd);
            return 0;
        }
        else {
            BDBG_ERR(("Specified mem address is out of range"));
            close(fd);
            return -1;
        }
    }
#endif
#ifdef PVR_SUPPORT
    else if (cmdname=="record_start" || cmdname=="record_start_timeshift") {
        int ch;
        Video *vid = new Video;

        vid->isEncoded = false;
        vid->isNetRecord = false;
        vid->startPlaybackOfRecord = false;

        if (cmdparams.total()!=4) {
            goto start_usage;
        }

        ch = string2channel(cmdparams[0]);

        /* generate title, similar to what's done in recordscreen */
        char buf[256];
        const char* title;
        title = control()->guide()->programName(ch);
        if (!title) {
            ChannelMap::Channel channel;
            if (!control()->channelMap()->getChannel(ch, &channel)) {
                title = buf;
                char *ptr = buf;
                ptr += b_print_chnum(ptr, 256, &channel);
                ptr += sprintf(ptr, ", ");
                b_print_channel_description(ptr, 200, &channel.entry);
            }
            else {
                title = "Recording";
            }
        }
        vid->title = title;

        if (!cmdparams[1].strncasecmp("default")) {
            vid->mediafile = NULL;
        }
        else {
            if (strrchr(cmdparams[1],'/'))
                vid->mediafile = strrchr(cmdparams[1],'/')+1;
            else
                vid->mediafile = cmdparams[1];
        }

        if (!cmdparams[2].strncasecmp("TS")) {
            vid->timestamp_enabled = false;
            vid->timestamp_override = true;
        }
        else if (!cmdparams[2].strncasecmp("TTS")) {
            vid->timestamp_enabled = true;
            vid->timestamp_override = true;
        }
        else {
            goto start_usage;
        }

        if (!cmdparams[3].strncasecmp("none")) {
            vid->mpeg.encryption.type = bencryption_type_none;
            vid->mpeg.encryption.key_length = 0;
        }
        else if (!cmdparams[3].strncasecmp("AES")) {
            vid->mpeg.encryption.type = bencryption_type_aes;
            vid->mpeg.encryption.key_length = 128;
        }
        else if (!cmdparams[3].strncasecmp("3DES")) {
            vid->mpeg.encryption.type = bencryption_type_3des;
            vid->mpeg.encryption.key_length = 128;
        }
        else {
            goto start_usage;
        }

        if (cmdname=="record_start_timeshift") {
            /* record_start_timeshift is not really "timeshifting". it is just starting playback of
            a linear recording file. true timeshift uses a fifo (circular) recording file.
            see CONTINUOUS_RECORD_ENABLED. */
            unsigned count = 0;
            AVStream *stream = control()->getLiveStream();
            vid->startPlaybackOfRecord = true;
            control()->setChannel(ch, control()->eDecode_Immediate, false, -1);

            /* for timeshift, decode must be started before record is started.
               the lock may not occur until later, so we wait and poll */
            while (!stream->isLocked() || !stream->isDecoding()) {
                count++;
                BKNI_Sleep(100);
                control()->getLiveStream()->checkLock(false);
                if (count>50) {
                    BDBG_ERR(("Could not start decode before timeshift record"));
                    break;
                }
            }
        }

        vid->isBackgroundRecord = true;
        if (control()->recordVideoPrepare(vid)==true) {
            if (control()->recordVideo(vid, ch)==true) {
                BDBG_WRN(("Record started on ch %d.%d", MAINCH(ch), SUBCH(ch)+1));
            }
        }

        vid = NULL;
        return 0;
start_usage:
        BDBG_ERR(("Usage: record_start(channel, output_file/default, TS/TTS, none/AES/3DES)"));
        return -1;
    }

    else if (cmdname == "record_stop") {
        int ch;
        if (cmdparams.total()==0) {
            ch = control()->stopRecordLast();
            if (ch!=-1) {
                BDBG_WRN(("Record stopped on ch %d.%d", MAINCH(ch), SUBCH(ch)+1));
            }
        }
        else if (!cmdparams[0].strncasecmp("all")) {
            ch = control()->stopRecordAll();
            BDBG_WRN(("Stopped %d records", ch));
        }
        else {
            ch = string2channel(cmdparams[0]);
            if (control()->stopRecordCh(ch)) {
                BDBG_WRN(("Record stopped on ch %d.%d", MAINCH(ch), SUBCH(ch)+1));
            }
            else {
                BDBG_WRN(("There is no record on ch %d.%d", MAINCH(ch), SUBCH(ch)+1));
            }
        }
        return 0;
    }
    else if (cmdname == "delete_stream") {
        int i;
        if (cmdparams.total()) {
            int ch = atoi(cmdparams[0]);
            if (control()->deleteStream(ch)) {
                BDBG_WRN(("Deleted stream %d", ch));
            }
        }
        return 0;
    }
    else if (cmdname == "seek") {
        PlayEngine *playEngine = control()->playEngine();
        if (!playEngine) return -1;
        playEngine->gotoTimestamp(cmdparams[0].toInt());
        return 0;
    }
#endif
    else if (cmdname == "macrovision") {
        bdisplay_settings settings;
        bmacrovision_type type;

        bdisplay_get(control()->display()->display, &settings);
        if (!cmdparams[0].strncasecmp("none")) {
            type = bmacrovision_type_none;          /* null */
        }
        else if (!cmdparams[0].strncasecmp("type1")) {
            type = bmacrovision_type_agc_only;      /* Type 1 */
        }
        else if (!cmdparams[0].strncasecmp("type2")) {
            type = bmacrovision_type_agc_2lines;    /* Type 2 */
        }
        else if (!cmdparams[0].strncasecmp("type3")) {
            type = bmacrovision_type_agc_4lines;    /* Type 3 */
        }
        else if (!cmdparams[0].strncasecmp("test1")) {
            type = bmacrovision_type_test01;        /* Test 1 */
        }
        else if (!cmdparams[0].strncasecmp("test2")) {
            type = bmacrovision_type_test02;        /* Test 2 */
        }
        else {
            BDBG_WRN(("invalid argument. valid arguments are: none,type1,type2,type3,test1,test2"));
            return 0;
        }

        settings.macrovision_type = type;
        bdisplay_set(control()->display()->display, &settings);
        return 0;
    }
#ifdef NEXUS_BUILD
    else if (cmdname == "st") {
        if (control()->decode()) {

            DecodeEngine::Settings settings;
            control()->decode()->getSettings(&settings);
            NEXUS_VideoDecoderStatus vstatus;
            NEXUS_AudioDecoderStatus astatus;

            /*common status*/
            uint32_t stc = 0;
            if(settings.decode->stcChannel)
                NEXUS_StcChannel_GetStc(settings.decode->stcChannel, &stc);

            /*Video status*/
            NEXUS_VideoDecoder_GetStatus(settings.decode->video_decode->nVideoDecoder, &vstatus);
            BKNI_Printf("Video decoder started:%s\n", (vstatus.started)? "y": "n");
            if(vstatus.started){
                uint32_t frameRate = b_nexus2framerate(vstatus.frameRate);
                BKNI_Printf("TSM:%s\n", (vstatus.tsm)? "y": "n");
                BKNI_Printf("Video FIFO: %u/%u (%u%%)\n", vstatus.fifoDepth, vstatus.fifoSize, (vstatus.fifoDepth * 100)/vstatus.fifoSize);
                BKNI_Printf("video PTS: %u   STC: %u   (PTS-STC)raw: %d   Diff: %d\n", vstatus.pts, stc, vstatus.ptsStcDifference, vstatus.ptsStcDifference);
                BKNI_Printf("Frame rate: %u.%u fps %s\n", frameRate/100, frameRate%100, (vstatus.interlaced)?"I":"P");
                    NEXUS_VideoDecoder_GetStatus(settings.decode->video_decode->nVideoDecoder, &vstatus);
            }

            NEXUS_AudioDecoder_GetStatus(settings.decode->audio_decode->nAudioDecoder, &astatus);
            /*Audio status*/
            BKNI_Printf("Audio decoder started:%s\n", (astatus.started)? "y": "n");
            if(astatus.started){
                BKNI_Printf("TSM:%s\n", (astatus.tsm)?"y":"n");
                BKNI_Printf("Audio FIFO: %u/%u (%u%%)\n",astatus.fifoDepth, astatus.fifoSize, (astatus.fifoDepth * 100)/astatus.fifoSize);
                BKNI_Printf("Audio PTS: %u   STC: %u   (PTS-STC)raw: %d   Diff: %d\n", astatus.pts, stc, astatus.ptsStcDifference, astatus.ptsStcDifference);
            }
 #ifdef PVR_SUPPORT

            /*Playback status*/
            NEXUS_PlaybackStatus pstatus;
            if(settings.playEngine){
                NEXUS_Playback_GetStatus(settings.playEngine->handle()->nPlayback, &pstatus);
                BKNI_Printf("Playback FIFO: %u/%u (%u%%)\n", pstatus.fifoDepth, pstatus.fifoSize, (pstatus.fifoDepth * 100)/pstatus.fifoSize);
            }
#endif
        }
        return 0;
    }
#endif
    else if (cmdname == "standby") {
	control()->standby(brutus_standby_state_ePassiveStandby);
    }

    /* Process the commands that have a keypad or remote equivalent */
    BrutusCommand data;
    if (!lookupCmd(cmd, &data)) {
        int rc = processCmd(data,true)?0:-1;
        // it's important to know if the command completed for debugging
        BDBG_MSG(("script: rc=%d", rc));
        return rc;
    }

    /* process debug commands */
    int rc = inputengine_ext(cmd);
    if (!rc) return 0;

    /* if everything else fails -- see if the command is a channel number (save me from having to type "ch(11.2)" */
    /* This just saves me from having to type "ch(" for complex channel numbers or channels above 9 */
    int ch = atoi(cmd); /* atoi will ignore #'s after . */
    char *sub = strchr(cmd, '.');
    int subch = sub?atoi(sub+1):0;
    if (ch || (sub && subch)) {
        if (subch) subch--;
        ch = MAKESUBCH(ch, subch);
        control()->setChannel(ch);
        return 0;
    }

    BDBG_ERR(("  Unknown command"));
    return -1;
}

void InputEngine::onTimeout(MTimer *timer) {
#ifdef SCRIPT_SUPPORT
    if (!ready) return;

    if (ScriptEngine::appTerminate()) {
        /* we're done, there's no point in doing more. */
        app()->terminate();
        return;
    }

    if (_scriptCmd.total()) {
        char *cmd = (char *)BKNI_Malloc(_scriptCmd[0].length() + 1);
        strcpy(cmd, _scriptCmd[0]);
        _scriptCmd.remove(0); // FIFO
        runScriptCmd(cmd);
        BKNI_Free(cmd);
    }
#endif
}

void InputEngine::set(ScriptEngine *scriptEngine)
{
    _scriptEngine = scriptEngine;
}

