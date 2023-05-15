/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: control.cpp $
 * $brcm_Revision: 308 $
 * $brcm_Date: 10/9/12 3:06p $
 *
 * Module Description: Central control of Brutus
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/control.cpp $
 * 
 * 308   10/9/12 3:06p erickson
 * SW7435-392: convert ERR to WRN
 * 
 * 307   8/23/12 5:01p erickson
 * SW7425-3743: don't init frontend if there's no channel map
 * 
 * 305   3/21/12 1:53p erickson
 * SW7425-2664: remove old code
 *
 * 304   3/21/12 12:01p erickson
 * SW7425-2664: remove old code
 *
 * 303   3/19/12 6:44p erickson
 * SW7425-2664: purge old code
 *
 * 302   2/24/12 9:46a gmohile
 * SW7425-2473 : Fix frontend initialization
 *
 * 301   2/14/12 4:26p erickson
 * SW7425-2363: change file suffix to avoid confusing with linear TS or
 * NAV files
 *
 * 300   1/19/12 4:09p gmohile
 * SW7425-2213 : Fix S1 standby
 *
 * 299   1/18/12 4:11p jtna
 * SW7425-2213: add deep sleep standby to brutus
 *
 * 298   7/19/11 3:41p jtna
 * SW7344-126: NEXUS_VideoWindow_SetSettings() on a mosaic parent window
 * is no longer supported. Get the basic Brutus 3DTV working again, but
 * some obscure 3D features have been broken on purpose.
 *
 * 297   6/23/11 11:27a randyjew
 * SW7344-65:Remove exit() on Halt to avoid kernel oops
 *
 * 296   5/31/11 11:12a randyjew
 * SW7425-427: Change debug print level for video source change
 *
 * 295   5/26/11 3:16p jessem
 * SW7425-427: Updated comment on Control::video_source_changed().
 *
 * 294   5/19/11 6:50p jtna
 * SW7425-236: set videodecoder source orientation to 2D. Brutus and other
 * 3D apps currently don't utilize new BVN HW to do 3D
 *
 * 293   4/27/11 4:42p jrubio
 * SW7346-158: fix frontpanel for 7346
 *
 * 292   4/8/11 4:23p ahulse
 * SW7405-5212: Add cfg to force aspect ratio on composite output for a 3d
 *
 * 291   4/8/11 12:14p jtna
 * SW7335-1175: add a runtime warning if NEXUS_NUM_MOSAIC_DECODES==0 and
 * Brutus 3DTV mode on
 *
 * 290   4/6/11 6:07p gmohile
 * SW7420-1792 : Fix platforms without standby support
 *
 * 289   4/6/11 2:30p gmohile
 * SW7420-1792 : Add power standby support
 *
 * 288   3/24/11 2:50p hongtaoz
 * SW7425-199: emulate customer AUTO_FORMAT behavior by doing check once
 * per StartDecode event;
 *
 * 287   2/22/11 5:14p jtna
 * SW7405-5135: assume 2D if cancel flag is set
 *
 * 286   2/16/11 3:53p nickh
 * SW7422-161: Fix API mismatch
 *
 * 285   1/21/11 2:19p ahulse
 * SW7405-5095: On HDMI hotplug re-evaluate 3D source
 *
 * 284   12/15/10 2:18p jtna
 * SW7405-4998: fix for DUAL_OUTPUT=1
 *
 * 283   11/19/10 4:56p jtna
 * SW7405-4998: even more Brutus polishing
 *
 * 282   11/16/10 5:36p jtna
 * SW7405-4998: avoid redundant 3DTV mode changes. remove userdata parsing
 * hack.
 *
 * 281   11/16/10 12:15p jtna
 * SW7405-4998: more 3D refactoring
 *
 * 280   11/16/10 9:58a jtna
 * SW7405-4998: fix for non-nexus platforms
 *
 * 279   11/15/10 4:20p jtna
 * SW7405-4998: refactor 3DTV code to allow transition in PIG and better
 * content mode handling
 *
 * 278   11/10/10 5:32p jtna
 * SW7401-4456: update
 *
 * 277   10/13/10 7:13p jtna
 * SW7401-4456: 2D/3D switch based on EDID
 *
 * 276   10/12/10 6:06p jtna
 * SW7405-4908: fix mosaic window source clipping for 3DTV:OU mode
 *
 * 275   10/12/10 5:53p jtna
 * SW7401-4455: add composite output for 3DTV. this removes the
 * HDSD_SINGLE=n requirement
 *
 * 274   10/6/10 2:14p calvinho
 * SW7401-4460: Add conditional check before hdmiSetVsi
 *
 * 273   10/1/10 4:21p gskerl
 * SW7468-102: Added mpu (multi-processor) field to the brutus_power_state
 * structure
 *
 * 272   8/21/10 3:18p gskerl
 * SW7125-237: Reverted back version 270 to re-enable passive standby on
 * UI
 *
 * 271   8/6/10 5:21p gskerl
 * SW7125-237: Temporarily disable Passive Standby on the 7125 until it is
 * completed
 *
 * 270   7/23/10 12:37p jtna
 * SW7550-478: extend video primer FCC support to platforms with fewer STC
 * channels
 *
 * 269   7/12/10 2:24p calvinho
 * SW7408-99: Fix brutus.cfg command FIRST_CHANNEL=play(x)
 *
 * 268   7/9/10 12:11p jtna
 * SW7405-3999: always change the 3DTV mode in eTV screen
 *
 * 267   7/6/10 5:17p mward
 * SW7405-4420: condition with NEXUS_BUILD to allow BUILD_SYSTEM=magnum
 * compile to be maintained.
 *
 * 266   6/29/10 6:10p jtna
 * SW7405-4420: fix for DUAL_OUTPUT=1
 *
 * 265   6/29/10 2:27p jtna
 * SW7405-3998: update NEXUS_VideoDecoder_Get3DTVStatus to return codec-
 * generic 3DTV status
 *
 * 264   6/25/10 6:14p jtna
 * SW7405-4420: handle 3dtv source/output permutations better
 *
 * 263   6/25/10 5:31p gskerl
 * SW7125-403: Merged Active Standby changes back to main branch.
 *
 * 262   6/15/10 5:54p jtna
 * SW7401-4412: fix parserband setting for 7401/7403 required for 3DTV
 *
 * 261   6/10/10 5:00p erickson
 * SWDEPRECATED-2726: prevent switch from continuous play to live when
 * pausing or rewinding
 *
 * 260   5/24/10 11:53a jtna
 * SW7405-4379: disabled 3DTV 2D->3D coversion
 *
 * 259   5/12/10 5:51p jtna
 * SW7405-3999: added 3DTV auto mode
 *
 * 258   5/5/10 5:05p jtna
 * SW7420-527: added libtune memset to 0
 *
 * 257   5/4/10 1:29p gskerl
 * SW7405-4233: If video format specified by OUTPUT_FORMAT (or by -pal)
 * does not validate against EDID, then pick a valid format from the EDID
 *
 * 257   5/4/10 1:28p gskerl
 * SW7405-4233: If video format specified by OUTPUT_FORMAT (or by -pal)
 * does not validate against EDID, then pick a valid format from the EDID
 *
 * 256   4/22/10 3:51p jtna
 * SW7405-3999: add 3DTV 2D/3D conversion option
 *
 * 255   4/22/10 11:12a calvinho
 * SW7420-466: Added fix for applying DNR settings in onTimeout callback
 * (after decoder has started)
 *
 * 254   4/20/10 11:57a jtna
 * SW7420-527: moved libtune memset to control_libtune.cpp. and shortened
 * comment header
 *
 * 253   4/19/10 3:40p calvinho
 * SW7420-466: Added feature for setting aspect ratio in cfg file. Added
 * bound checking for DNR settings.
 *
 * 252   4/16/10 4:06p erickson
 * SW7405-4089: make brutus resilient to config with zero decoders
 *
 * 251   4/8/10 2:19p jtna
 * SW7420-466: added VIDEO_ANR and VIDEO_SPLIT_SCREEN_MODE cfgsettings
 *
 * 250   4/5/10 11:30a erickson
 * SW7405-3997: set HDMI VSI for 3DTV
 *
 * 249   3/30/10 3:15p jtna
 * SW7420-527: moved libtune code from control.cpp to control_libtune.cpp
 *
 * 248   3/29/10 11:43a erickson
 * SW7405-3625: clean up 3DTV full screen/PIG transition
 *
 * 247   3/4/10 5:35p jtna
 * SW7420-527: temp workaround to enable sharpness settings at init
 *
 * 246   3/2/10 4:28p erickson
 * SW7420-527: fix the fix for non-mosaic platforms
 *
 * 245   3/2/10 4:00p jtna
 * SW7420-527: avoid tuning before decode is active
 *
 * 244   3/2/10 2:54p erickson
 * SW7405-3625: fix non-mosaic platforms
 *
 * 243   3/2/10 11:25a erickson
 * SW7405-3625: refactor 3DTV mosaic to use non-mosaic for fullscreen TV
 *
 * 242   3/1/10 7:28p jtna
 * SW7420-527: added LIBTUNE_DBNAME cfgsetting
 *
 * 241   3/1/10 6:52p jtna
 * SW7420-527: initial Brutus libtune integration
 *
 * 240   2/26/10 1:56p gskerl
 * SW7405-3625: More 3D PIG: Added closing of mosaic windows at Brutus
 * shutdown
 *
 * 239   2/26/10 11:14a gskerl
 * SW7405-3625: Added 3D PIG functionality using nexus mosaic windows
 *
 * 238   1/21/10 5:41p mward
 * SW7400-2517: merge to main support for BUILD_SYSTEM=magnum.
 *
 * SW7400-2517/5   1/7/10 11:57a mward
 * SW7400-2517: merge from main
 *
 * 237   12/29/09 3:38p erickson
 * SW7420-522: brutus.cfg FIRST_CHANNEL=play(X) will start brutus playing
 * stream X
 *
 * 236   12/28/09 2:32p erickson
 * SW7420-516: destroy brutus screens before playback. some screens have
 * hooks to PlayEngine and need to be destroyed first.
 *
 * 235   12/22/09 4:00p erickson
 * SW7420-70: use IR remote "power" button to enter passive standby
 *
 * SW7400-2517/4   12/16/09 11:25a mward
 * SW7400-2517: merge latest from main.
 *
 * 234   12/11/09 6:12p mphillip
 * SW7550-112: Merge 7550 changes to main branch
 *
 * Refsw_7550/1   11/18/09 4:18p chengli
 * SW7550-64 : enable channel map screen even B_HAS_IP is not defined
 *
 * SW7400-2517/3   12/2/09 5:40p mward
 * SW7125-73: setOutput() also indicate failure to set HD format if it's
 * set first.
 *
 * 233   12/2/09 5:10p mward
 * SW7125-73: setOutput() also indicate failure to set HD format if it's
 * set first.
 *
 * SW7400-2517/2   11/18/09 9:14a mward
 * SW7400-2517: merge latest from main.
 *
 * 232   11/11/09 6:37p mward
 * SW7400-2606: Sharpness should default to minimum.
 *
 * 231   10/23/09 4:28p jtna
 * SW7405-3265: handle video primer FCC better
 *
 * SW7400-2517/1   9/14/09 3:09p mward
 * SW7400-2517: Maintain Native Settop API (BUILD_SYSTEM=magnum) build for
 * 97400.
 *
 * 230   9/10/09 12:27p jtna
 * SWDEPRECATED-3910: update BDBG_WRN
 *
 * 229   9/4/09 11:44a jgarrett
 * SWDEPRECATED-3910: Removing setenv call
 *
 * 228   8/25/09 12:48p jtna
 * SWDEPRECATED-3910: video primer fast channel change feature
 *
 * 227   8/14/09 3:37p erickson
 * PR55994: refactor thumbnail
 *
 * 226   8/10/09 1:36p jrubio
 * PR55232: add 7340/7342
 *
 * 225   8/3/09 10:55a mward
 * PR56956: Indicate brutus init is complete and enable script when
 * PVR_SUPPORT is not defined.
 *
 * 224   7/28/09 11:24a lwhite
 * PR55693: Added DTCP/IP intialization
 *
 * 223   7/27/09 11:12a jtna
 * PR56956: defer script execution until brutus init is complete
 *
 * 222   7/9/09 11:52a jtna
 * PR56522: Remove multi-decode fast channel change feature. To be
 * replaced with video primer FCC.
 *
 * 221   7/8/09 10:40a erickson
 * PR56373: add FIRST_CHANNEL option. restore user's channel after wakeup
 * from passive standby mode.
 *
 * 220   7/8/09 8:44a erickson
 * PR56373: use counter so that playback list is not read until after live
 * decode has started or a timeout occurs
 *
 * 219   7/2/09 5:47p erickson
 * PR56373: move standby logic to Control where it can be called from
 * multiple spots in the UI
 *
 * 218   7/2/09 5:09p erickson
 * PR56373: rework brutus_power to allow for deferred usb & sata power,
 * deferred mount, deferred videos scan
 *
 * 217   7/2/09 10:57a erickson
 * PR56558: defer frontend init and channel scan until after the GUI is up
 *
 * 216   5/29/09 2:39p jtna
 * PR43001: refactor brutus record logic
 *
 * 215   5/26/09 6:45p mward
 * PR53373: Second Brutus does not have HDMI.
 *
 * 214   5/22/09 3:36p jjordan
 * PR55415: PR55415: Add setting of audio category code and copyright bit
 * on SPDIF and HDMI outputs via brutus.cfg
 *
 * 213   5/19/09 10:32a jjordan
 * PR54879: PR54879: support setting of SD output format separately
 *
 * 212   5/15/09 3:44p jjordan
 * PR52687: PR52687: improve AAC downmix API
 * PR52687: Add method to set audio downmix type
 *
 * 211   5/11/09 7:40p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 *
 * 210   5/11/09 5:12p erickson
 * PR54915: remove unused and misleading code
 *
 ***************************************************************************/
#include "control.h"
#include "ledengine.h"
#include "channelmgr.h"
#include "mmessagebox.h"
#include "mapplication.h"
#include "bsettop.h"
#include "cfgsettings.h"
#include "video.h"
#include "programguide.h"
#include "wow.h"
#include "pep_config.h"
#include <bstd.h>
#include <bkni.h>
BDBG_MODULE(brutus_control);

// brutus screens
#include "homescreen.h"
#include "tvscreen.h"
#ifdef INFOPANEL_SUPPORT
#include "info_outputs.h"
#endif
#ifdef ADMIN_SUPPORT
#include "adminscreen.h"
#include "editchannelsscreen.h"
#include "runscriptscreen.h"
#endif
#ifdef PVR_SUPPORT
#include "playbackscreen.h"
#include "videoscreen.h"
#include "recordscreen.h"
#endif
#ifdef PICTURES_SUPPORT
#include "picturesscreen.h"
#endif
#ifdef AUDIO_SUPPORT
#include "audioscreen.h"
#include "audiocontrol.h"
#endif
#include "ipstbconfigscreen.h"
#ifdef LIVEMEDIA_SUPPORT
#include "lm_helper.h"
#include "ipstbchannellineupacquisition.h"
#endif
#ifdef B_HAS_DRM_ND
#include "extServerScreen.h"
#endif
#include "chanmapscreen.h"
#if POWERSTANDBY_SUPPORT
#include "brutus_power.h"
#endif

#ifdef HDMI_SUPPORT
#include "hdmiscreen.h"
#endif
#ifdef HDMI_CEC_SUPPORT
#include "hdmicecscreen.h"
#endif

#include "info_audio.h"

#include "skin.h"

#ifdef B_HAS_DTCP_IP
#include "b_dtcp_applib.h"
#endif

#ifdef NEXUS_BUILD
#include "nexus_platform.h"
#include "nexus_core_utils.h"
#include "nexus_display.h"
#include "nexus_video_decoder_extra.h"
#if NEXUS_NUM_MOSAIC_DECODES
#include "nexus_mosaic_display.h"
#endif
#include "nexus_video_window.h"

static NEXUS_VideoWindowHandle g_mosaicWindow[2]; /* mosaics */

/* Brutus implements mosaic windows (for 3D) by accessing Nexus directly */
#include "bsettop_types.h"
#define BSETTOP_NO_API_THUNKS
extern "C" {
    #include "bsettop_impl.h"
}
#endif  /* NEXUS_BUILD */

#define tv()            ((TVScreen *)_screens[eTV])
#define videoScreen()   ((VideoScreen *)_screens[eVideo])
#define recordScreen()  ((RecordScreen *)_screens[eRecord])
#define adminScreen()   ((AdminScreen *)_screens[eAdmin])
#define audioScreen()   ((AudioScreen *)_screens[eAudio])

#ifdef HDMI_SUPPORT
#define hdmiScreen()    ((HdmiScreen *)_screens[eHdmi])
#endif

#ifdef HDMI_CEC_SUPPORT
#define hdmiCecScreen() ((HdmiCecScreen *)_screens[eHdmiCec])
#endif

#ifdef B_HAS_DRM_ND
#define extServerScreen()   ((ExtServerScreen *)_screens[eExtserver])
#endif

    /* Refresh function for Chanmap Screen */
#define chanmapScreen() ((ChanMapScreen*)_screens[eChanMap])


/* #define W(MSG) do {_leds->write(MSG); BKNI_Printf("%s\n", (const char *)(MSG)); } while(0)
*/
#define W(MSG) _leds->write(MSG)

/* TODO: async tune/acquire would speed up channel change without deferment delays */
#define DEFER_SET_CHANNEL_TIME 450

/* NOTE: Not all of these settings use a PEP block. Some use the compositor's color space convertor. */
const PepConfig g_pep_settings =
{
    0, 0, 0, 0, -32768       /* default values */
};

void Control::chanmapScreenRefresh(void)
{
    chanmapScreen()->refresh();
}

/* Settop API callbacks. We are re-entrant into Brutus state, so we should only set an atomic state variable,
then service it in Control::onTimeout. */
void Control::video_source_changed(void *context)
{
    Control *control = (Control *)context;
    BDBG_MSG(("video_source_changed"));
    /* AUTO_OUTPUT_FORMAT means change the display format, if needed, to the new channel's initial format.
       In other words, only change the format at the onset of channel change based on the new channel's initial format,
       ie., after the StartDecode event. Any subsequent format changes  while the channel is displayed is ignored.
       This is because a format change could flick the display if the stream is not coded properly. */
    if (control->cfg()->getBool(Config::AUTO_OUTPUT_FORMAT) && control->_started_decode) {
        control->_check_optimal_display = true;
        control->_started_decode = false;
    }

    control->_check_3dtv = true;

#ifdef BRUTUS_LIBTUNE_SUPPORT
    if (control->libtune.handle) {
        control->_check_libtune = true;
    }
#endif

    if (control->_apply_dnr_settings) {
        control->_check_dnr_settings = true;
    }
}

/* Settop API callbacks. We are re-entrant into Brutus state, so we should only set an atomic state variable,
then service it in Control::onTimeout. */
void Control::audio_source_changed(void *context)
{
    Control *control = (Control *)context;
    BDBG_MSG(("%p audio changed", control));
}

static void brutus_delete_surface(void *context, void *surface)
{
    BSTD_UNUSED(context);
    bsurface_destroy((bsurface_t)surface);
}

#ifdef NEXUS_BUILD
void Control::parseSeiMessage(NEXUS_VideoDecoderHandle videoDecoder, Brutus3DTVMode *mode)
{
    NEXUS_VideoDecoder3DTVStatus status;
    NEXUS_VideoDecoder_Get3DTVStatus(videoDecoder, &status);
    *mode = Brutus3DTVMode_eOff;
    bool valid;
    if (status.codec == NEXUS_VideoCodec_eH264) {
        valid = status.codecData.avc.valid;
        if (status.codecData.avc.data.flags & 0x1) { /* if cancel flag set, then assume 2D */
            return;
        }
    }
    else {
        valid = status.codecData.mpeg2.valid;
    }

    if (!valid) { /* assume 2D */
        return;
    }

    switch (status.format) {
        case NEXUS_VideoDecoder3DTVFormat_eNone:
            *mode = Brutus3DTVMode_eOff;
            break;
        case NEXUS_VideoDecoder3DTVFormat_eSideBySideHalf:
            *mode = Brutus3DTVMode_eSBS;
            break;
        case NEXUS_VideoDecoder3DTVFormat_eTopAndBottomHalf:
            *mode = Brutus3DTVMode_eOU;
            break;
        default:
            BDBG_WRN(("Unsupported 3D format"));
            *mode = Brutus3DTVMode_eOff;
            break;
    }
    return;
}

void Control::sei_callback(void *context, int param)
{
    Control *control = (Control*)context;
    BSTD_UNUSED(param);

    control->_check_3dtv = true;
    BDBG_WRN(("SEI callback"));
    /* the message is parsed in onTimeout() */
}
#endif
Control::Control(MFrameBuffer *fb, Brutus *brutus, ChannelManager *mgr,
    RecordManager *recordManager, AVManager *avManager, Config *acfg) :
    _fb(fb),
    _timer(fb->app())
{
#if B_HAS_TRANSCODE
    btranscode_t transcode = NULL;
#endif

    _liveDecodeEnabled = true;
    _valid = false;
    _inFade = false;
    _repeatKeyFunc = NULL;
    _cfg = acfg; /* now we can read configuration information using cfg() */
    _chMgr = mgr;
    _channelMap = _chMgr->map();
    _recordManager = recordManager;
    _avManager = avManager;
    _decodeManager = new DecodeManager();
#ifdef PVR_SUPPORT
    _playbackManager = new PlaybackManager();
    _initialDiskReadCounter = 2000; /* don't read the playback list until either we start live decode or 2 seconds have expired */
#endif
#ifdef BRUTUS_LIBTUNE_SUPPORT
    memset(&libtune, 0, sizeof(libtune));
#endif

    memset(_live, 0, sizeof(_live));
    memset(_decode, 0, sizeof(_decode));
    memset(_window, 0, sizeof(_window));
    memset(_channel, 0, sizeof(_channel));
    _brutus = brutus;
    _incrementalChEntry.digits = 0;
    _deferSetChannel.tickCount = 0;
    _pipVisible = false;
    _currentScreen = eHome;
    _currentDecode = secondDisplay()?1:0;
    _hdmiHdcpState = boutput_hdmi_hdcp_state_disabled;
    _check_hdmi_cec = false;
    _check_3dtv = false;
    _audioSourceChanged = false;
    _standbyState = brutus_standby_state_eOn;

    /* skin and control objects link to one another */
    _skin = brutus->skin();
    brutus->skin()->setControl(this);
    _fullScreenAlpha = cfg()->getInt(Config::FULLSCREEN_TV_ALPHA, 0xAA);

    /* create object for active IP channel */
    _ipVideo = new Video();
#ifdef LIVEMEDIA_SUPPORT
    liveMediaOpen();
        /* start the IP Channel Acquisition */
        ipChannelLineupAcquisitionStart(mgr, avManager, cfg()->getInt(Config::SAP_TIMEOUT_INTERVAL));
#endif

#ifdef B_HAS_DTCP_IP
    _ipVideo->dtcpIpCtx = DtcpAppLib_Startup(B_DeviceMode_eSink, false);
    if(_ipVideo->dtcpIpCtx == NULL) {
       BDBG_ERR(("Failed to Initialize Dtcp/Ip Library: DTCP/IP encryption/decryption won't work\n" ));
    }
#endif

#ifdef PVR_SUPPORT
    BKNI_Memset(&_thumbnail, 0, sizeof(_thumbnail));
    if (cfg()->getBool(Config::THUMBNAIL_MODE)) {
#ifdef NEXUS_BUILD
        bthumbnail_manager_settings mgr_settings;
        bthumbnail_extractor_create_settings extractor_create_settings;

        bthumbnail_manager_get_default_settings(&mgr_settings);
        /* TODO: mgr_settings.request_ready */
        mgr_settings.delete_surface = brutus_delete_surface;
        mgr_settings.maximum_memory = cfg()->getInt(Config::THUMBNAIL_MEMORY,16);
        mgr_settings.maximum_memory *= 1024 * 1024;
        _thumbnail.manager = bthumbnail_manager_create(&mgr_settings);

        bthumbnail_extractor_get_default_create_settings(&extractor_create_settings);
        _thumbnail.extractor = bthumbnail_extractor_create(&extractor_create_settings);
#else
        BDBG_ERR(("THUMBNAIL_MODE is only supported in Nexus.  Don't use BUILD_SYSTEM=magnum."));
#endif
    }
#endif
    /* Create the led output device */
    _leds = new LedEngine(fb->app(), !cfg()->getBool(Config::SECOND_DISPLAY));

    /* Now we can write our first led message */
    W("1");

    /* Start idle time processing */
    _timer.addHandler(this);
    _timer.start(0,MTimer::eForever);

    /* Select if we're going to open decode 0, 1 or both. */
    int start = 0;
    int pbStart = 0;
    _totalDecodes = 1;
    /* By default, brutus will take the majority of playback channels
       when divided by the # of decodes (some have odd #'s, some have 1 decoder)

       Examples: 7401 C0 has 1 decode , 4 playbacks.  4 can be allocated by the first brutus ((4+0)/1)
                 7400 A0 has 2 decodes, 4 playbacks.  2 can be allocated by the first brutus ((4+1)/2)
                 7400 B0 has 2 decodes, 5 playbacks.  3 can be allocated by the first brutus ((5+1)/2)
    */
    if (bconfig->resources.decode.total) {
        _totalPlaybacks = (bconfig->resources.playback.total + (bconfig->resources.decode.total-1)) /
            bconfig->resources.decode.total;
    }
    else {
        _totalPlaybacks = 0;
    }

    if (pipEnabled())
    {
        _totalDecodes = 2;
        _totalPlaybacks = bconfig->resources.playback.total;    /* Allow all playback resources on one brutus */
    }
    else if (secondDisplay())
    {
        start = 1;
        _totalDecodes = 2;
        pbStart = _totalPlaybacks; /* Take remainder on a second brutus */
        _totalPlaybacks = bconfig->resources.playback.total - pbStart;
    }
    if (_totalDecodes > (int)bconfig->resources.decode.total) {
        BDBG_ERR(("Brutus can only open %d decode(s)", bconfig->resources.decode.total));
        _totalDecodes = bconfig->resources.decode.total;
    }

#ifdef PVR_SUPPORT
    for (int i=pbStart; i < pbStart+_totalPlaybacks; i++)
    {
        PlayEngine *p = new PlayEngine(fb->app(), cfg(), "play", i);
        /* If PlayEngine cannot open a playback (maybe this platform doesn't
        support it, handle it gracefully. */
        if (!p->valid()) {
            delete p;
            p = NULL;
        }
        else {
            p->addHandler(this);
            _playbackManager->add(p);
        }
    }
#endif

    /* Now open the DecodeEngine's and PlayEngine's. Allocate the settop api
    resources needed. */
    for (int i=start;i<_totalDecodes;i++) {
        DecodeEngine::Settings settings;

        BKNI_Memset(&settings, 0, sizeof(settings));
        settings.mgr = _decodeManager;
#ifdef PVR_SUPPORT
        settings.playManager = _playbackManager;
#endif

        settings.decode = bdecode_open(B_ID(i));
        if (!settings.decode) {
            continue;
        }

        if (pipEnabled() && i != 0) {
            /* Throw away the 2nd audio decode if it exists */
            bdecode_detach_audio(settings.decode);
            settings.audioEnabled = false;
        }
        else {
            settings.audioEnabled = true;
        }

        settings.window = bdecode_window_open(B_ID(i), display()->display);
        BDBG_ASSERT(settings.window);
        _ipVideo->awindow[i] = settings.window;
        _ipVideo->adecode[i] = settings.decode;


        /* If we're running an HD/SD mode, we need to drive the vbi out the SD. */
        if (cfg()->getBool(Config::VBI_ENABLED)) {
            if (cfg()->getBool(Config::HDSD_SINGLE))
                settings.vbi_display = display()->display2;
            else
                settings.vbi_display = display()->display;
        }

#if B_HAS_TRANSCODE
    if(transcode == NULL)
        transcode = btranscode_open(B_ID(0));
    settings.transcode = transcode;
#endif

        _decode[i] = new DecodeEngine(&settings);
        _decodeManager->add(_decode[i]);
        _window[i] = settings.window;

#if NEXUS_NUM_MOSAIC_DECODES
        /* For the first (zeroth?) window, check to see if 3D mode is enabled, if so,
           create a couple of nexus mosaic windows */
        if (i == 0 && (display()->s3dTv.output>Brutus3DTVMode_eOff || display()->s3dTv.automode)) {
            NEXUS_VideoDecoderSettings videoDecoderSettings;
            NEXUS_VideoDecoderHandle videoDecoder = getDecode(0)->video_decode->nVideoDecoder;
            NEXUS_Error rc;

            /* Create the first (left half) mosaic window */
            g_mosaicWindow[0] = NEXUS_VideoWindow_OpenMosaic(_window[i]->nWindow, 0);
            BDBG_ASSERT(g_mosaicWindow[0]);

            /* Create the second (right half) mosaic window */
            g_mosaicWindow[1] = NEXUS_VideoWindow_OpenMosaic(_window[i]->nWindow, 1);
            BDBG_ASSERT(g_mosaicWindow[1]);

            NEXUS_VideoDecoder_GetSettings(videoDecoder, &videoDecoderSettings);
            /* Brutus does not yet utilize new BVN HW to do 3D */
            videoDecoderSettings.customSourceOrientation = true;
            videoDecoderSettings.sourceOrientation = NEXUS_VideoDecoderSourceOrientation_e2D;
            videoDecoderSettings.manualPowerState = true;
            rc = NEXUS_VideoDecoder_SetSettings(videoDecoder, &videoDecoderSettings);
            BDBG_ASSERT(!rc);

            /* install SEI frame packing message callback */
            NEXUS_VideoDecoderExtendedSettings extSettings;
            NEXUS_VideoDecoder_GetExtendedSettings(videoDecoder, &extSettings);
            extSettings.s3DTVStatusChanged.callback = sei_callback;
            extSettings.s3DTVStatusChanged.context = (void*)this;
            extSettings.s3DTVStatusEnabled = true;
            extSettings.s3DTVStatusTimeout = cfg()->getInt(Config::OUTPUT_3D_STATUS_TIMEOUT, 0);
            NEXUS_VideoDecoder_SetExtendedSettings(videoDecoder, &extSettings);

            /* 3D test stream requires larger parserband bandwidth. temporarily increase it.
               this is tricky to do properly without shim hacking */
            unsigned ib;
            NEXUS_Platform_GetStreamerInputBand(0, (NEXUS_InputBand*)&ib);
            NEXUS_ParserBand parserBand = (NEXUS_ParserBand)ib; /* shim uses the same number */
            NEXUS_ParserBandSettings parserBandSettings;
            NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
            parserBandSettings.maxDataRate = 25 * 1024 * 1024;
            rc = NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

            /* assume same source orientation as output. auto mode assumes 2D source */
            _brutus->display()->s3dTv.source = display()->s3dTv.output;
            _brutus->display()->s3dTv.cfg = display()->s3dTv.output;
        }
#else
        if (display()->s3dTv.output>Brutus3DTVMode_eOff) {
            BDBG_ERR(("NEXUS_NUM_MOSAIC_DECODES must be >0 for Brutus 3DTV"));
        }
#endif
        if (cfg()->getBool(Config::FORCE_3D_OUTPUT_FOR_INCOMPATIBLE_TV)==false) {
            _check_optimal_display = true;
        }

        /* Video quality settings */
        bdecode_window_settings ws;
        bdecode_window_get(_window[i], &ws);
        ws.contrast = cfg()->getInt(Config::VIDEO_CONTRAST);
        ws.brightness = cfg()->getInt(Config::VIDEO_BRIGHTNESS);
        ws.hue = cfg()->getInt(Config::VIDEO_HUE);
        ws.saturation = cfg()->getInt(Config::VIDEO_SATURATION);
        ws.sharpness = cfg()->getInt(Config::VIDEO_SHARPNESS);
        ws.dcr_level = cfg()->getInt(Config::VIDEO_DCR) > bwindow_dnr_level_max ? bwindow_dnr_level_max : (bwindow_dnr_level)cfg()->getInt(Config::VIDEO_DCR);
        ws.mnr_level = cfg()->getInt(Config::VIDEO_MNR) > bwindow_dnr_level_max ? bwindow_dnr_level_max : (bwindow_dnr_level)cfg()->getInt(Config::VIDEO_MNR);
        ws.bnr_level = cfg()->getInt(Config::VIDEO_BNR) > bwindow_dnr_level_max ? bwindow_dnr_level_max : (bwindow_dnr_level)cfg()->getInt(Config::VIDEO_BNR);
        ws.anr_level = cfg()->getInt(Config::VIDEO_ANR) > bwindow_dnr_level_max ? bwindow_dnr_level_max : (bwindow_dnr_level)cfg()->getInt(Config::VIDEO_ANR);
        ws.splitscreen_mode = (bwindow_splitscreen_mode)cfg()->getInt(Config::VIDEO_SPLIT_SCREEN_MODE);
        ws.green_stretch = cfg()->getInt(Config::VIDEO_GREEN_STRETCH);
        ws.blue_stretch = cfg()->getInt(Config::VIDEO_BLUE_STRETCH);
        ws.auto_flesh_tone = cfg()->getInt(Config::VIDEO_FLESH_TONE);
        ws.dynamic_contrast = cfg()->getInt(Config::VIDEO_DYNAMIC_CONTRAST);
        bdecode_window_set(_window[i], &ws);

        _apply_dnr_settings = ws.dcr_level || ws.mnr_level || ws.bnr_level || ws.anr_level;
        _check_dnr_settings = false;

        /* allow the main decode to feedback to app */
        if (start == i) {
            bdecode_settings decode_settings;
            bdecode_get(settings.decode, &decode_settings);
            decode_settings.video.source_changed = video_source_changed;
            decode_settings.audio.source_changed = audio_source_changed;
            decode_settings.callback_context = (void*)this;
            bdecode_set(settings.decode, &decode_settings);
        }
        if (_cfg->getBool(Config::ARIB_DOWNMIX_MODE_ENABLED)) {
            bdecode_settings decode_settings;
            bdecode_get(settings.decode, &decode_settings);
            decode_settings.audio.codec.aac.downmix = baudio_aac_downmix_arib;
            bdecode_set(settings.decode, &decode_settings);
        }
    }

    if (cfg()->getBool(Config::HDSD_SINGLE)) {
        BDBG_ASSERT(!secondDisplay());
        if (bconfig->resources.decode.total == 1) {
            /* single decode system */
            _window[2] = bdecode_window_clone(_window[0], B_ID(1), display()->display2);
        }
        else {
            /* dual decode system */
            _window[2] = bdecode_window_clone(_window[0], B_ID(2), display()->display2);
            if (_window[1])
                _window[3] = bdecode_window_clone(_window[1], B_ID(3), display()->display2);
        }
    }

#ifdef PVR_SUPPORT
    if (bconfig->resources.decode_still.total > secondDisplay()?1:0) {
        bdecode_settings decode_settings;
        _still_decode = bdecode_open(bconfig->resources.decode_still.objects[secondDisplay()?1:0]);
        if (_still_decode) {
            bdecode_get(_still_decode, &decode_settings);
            decode_settings.still_picture_done = Control::decoded_still_ready;
            decode_settings.callback_context = (void*)this;
            bdecode_set(_still_decode, &decode_settings);
        }
    }
    else {
        _still_decode = NULL;
    }
#endif
    /* get default display from settop api */
    bdisplay_get(display()->display, &_display_settings);
    if (display()->display2)
        bdisplay_get(display()->display2, &_display_settings2);
    if (_display_settings.spdif) {
        boutput_spdif_settings s;
        boutput_spdif_get(_display_settings.spdif, &s);
        s.pcm = !_cfg->getBool(Config::SPDIF_PASSTHROUGH_ENABLED);
        if(_cfg->get(Config::AUDIO_CATEGORY_CODE)) {
            s.spdif_channel_status.category_code = _cfg->getInt(Config::AUDIO_CATEGORY_CODE);
        }
        if(_cfg->get(Config::AUDIO_COPYRIGHT)) {
            s.spdif_channel_status.copyright = _cfg->getBool(Config::AUDIO_COPYRIGHT);
        }
        boutput_spdif_set(_display_settings.spdif, &s);
    }
    if (_display_settings.dvi) {
        boutput_hdmi_settings s;
        boutput_hdmi_get(_display_settings.dvi, &s);
        s.hdmi_audio_mode = _cfg->getBool(Config::HDMI_PASSTHROUGH_ENABLED) ?
            boutput_hdmi_audio_mode_compressed : boutput_hdmi_audio_mode_pcm;
        if(_cfg->get(Config::AUDIO_CATEGORY_CODE)) {
            s.hdmi_spdif_channel_status.category_code = _cfg->getInt(Config::AUDIO_CATEGORY_CODE);
        }
        if(_cfg->get(Config::AUDIO_COPYRIGHT)) {
            s.hdmi_spdif_channel_status.copyright = _cfg->getBool(Config::AUDIO_COPYRIGHT);
        }
        boutput_hdmi_set(_display_settings.dvi, &s);
    }

    /* Unswapped state */
    _windowSource[0] = 0;
    _windowSource[1] = 1;

    /* apply MAIN_WINDOW and PIP_WINDOW overrides */
    if (_cfg->get(Config::MAIN_WINDOW)) {
        unsigned x,y,w,h;
        if (sscanf(_cfg->get(Config::MAIN_WINDOW), "%d,%d,%d,%d", &x, &y, &w, &h) == 4) {
            setWindowOverride(0, x, y, w, h);
        }
    }
    if (_cfg->get(Config::PIP_WINDOW)) {
        unsigned x,y,w,h;
        if (sscanf(_cfg->get(Config::PIP_WINDOW), "%d,%d,%d,%d", &x, &y, &w, &h) == 4) {
            setWindowOverride(1, x, y, w, h);
        }
    }

    _guide = new ProgramGuide;
    _guide->read(_cfg->get(Config::PROGRAM_GUIDE));

    // NULL screens now because setOutput() might try to use it
    memset(_screens, 0, sizeof(_screens));

    //generate skin images/pixmaps if necessary.  we must do this
    //prior to creating any screens as they may require
    //skin images/pixmaps
    if (_skin->isValid())
    {
        _skin->createImagesPixmaps();
    }

    // screens
    W("SC1");
    _screens[eHome] = new HomeScreen(this);

    W("SC3");
    _screens[eTV] = new TVScreen(this);
#ifdef AUDIO_SUPPORT
    if (cfg()->getBool(Config::MP3_ENABLED)) {
        W("SC4");
        _audioControl = new AudioControl(this);
        _screens[eAudio] = new AudioScreen(this);
    }
    else {
        _audioControl = NULL;
    }
#endif

    W("SC5");
    /* set up default output. this can't be done until after tv screen is created */
    if (!secondDisplay())
    {
    /* default to one of the 1080i formats */
        if (cfg()->getBool(Config::HDSD_SINGLE) && !cfg()->get(Config::OUTPUT_FORMAT)) {
            _display_settings.format = isPalMode() ? bvideo_format_1080i_50hz : bvideo_format_1080i;
        }

        /* If we have a DVI/HDMI connection, we may want to switch to its preferred
        format. The following config option allows this to be overridden. */
        if ( _display_settings.dvi ) {
            initHdmiOutput();
        }

#ifdef INFOPANEL_SUPPORT
        /* Read brutus.cfg for default output settings. Let setOutput figure out the default outputs. */
    /* Note: if this format is not supported by HDMI monitor we could end up in a bad state. */
        if (cfg()->get(Config::OUTPUT_FORMAT)) {
            OutputsInfoBarPlugin::setFormat(cfg()->get(Config::OUTPUT_FORMAT), &_display_settings);
        }

         if (cfg()->get(Config::ASPECT_RATIO_HD)) {
            OutputsInfoBarPlugin::setAspectRatio(cfg()->get(Config::ASPECT_RATIO_HD), true, &_display_settings);
        }
#endif

        if (!setOutput(&_display_settings)) {

            /* We can't set the requested output format.  Maybe because we have an HDMI connection and
             * the format is not in the EDID.  If so, then call setOptimalDisplay to pick a valid output
             * format from the EDID.  */
            BDBG_WRN(("Failed to set requested video format %d", _display_settings.format));
            if ( _display_settings.dvi && cfg()->getBool(Config::DVI_VALIDATE_EDID_FORMAT)) {
                BDBG_WRN(("Choosing video format from EDID"));
                cfg()->set(Config::DVI_USE_PREFERRED_FORMAT, true); /* Need to set this for setOptimalDisplay() to work */
                setOptimalDisplay();
                BDBG_WRN(("Chose format %d from EDID", _display_settings.format));
            }
        }
    }
    else
    {
        /* Obey output format for second UI as well. */
        bdisplay_settings prev_display_settings = _display_settings;
#ifdef INFOPANEL_SUPPORT
        /* Read brutus.cfg for default output settings. Let setOutput figure out the default outputs. */
        if (cfg()->get(Config::OUTPUT_FORMAT)) {
            OutputsInfoBarPlugin::setFormat(cfg()->get(Config::OUTPUT_FORMAT), &_display_settings);
        }

        if (cfg()->get(Config::ASPECT_RATIO_HD)) {
            OutputsInfoBarPlugin::setAspectRatio(cfg()->get(Config::ASPECT_RATIO_HD), true, &_display_settings);
        }
#endif
        if (!(setOutput(&_display_settings))){
            _display_settings = prev_display_settings;
            /* selected an unsupported (HD) format, restore previous */
            setOutput(&_display_settings);
        }
    }

#ifdef BRUTUS_LIBTUNE_SUPPORT
    /* do this after display settings have been applied */
    if (_decode[start]) {
        Libtune_Init(_decode[start], _window);
    }
#endif

#ifdef PVR_SUPPORT
    W("PVR");
    _videos = new VideoList(thumbnail_manager());

    // search cfg file for videos directories. if not there, VideoList will use default.
    {
        int i=0;
        while (1) {
            char buf[30];
            sprintf(buf, "VIDEODIR%d", i);
            const char *dir = cfg()->get(buf);
            if (!dir) break;
            sprintf(buf, "VIDEODIR%d_LABEL", i);
            videos()->addDir(cfg()->get(buf), dir);
            i++;
        }
    }

    if (cfg()->getBool(Config::PLAYBACK_ENABLED)) {
        W("PLAY");
        _screens[ePlayback] = new PlaybackScreen(this, videos());
        _screens[eVideo] = new VideoScreen(this);
    }

    if (cfg()->getBool(Config::RECORD_ENABLED)) {
        W("REC");
        _screens[eRecord] = new RecordScreen(this);
        // do the initial disk scan right away so that the first
        // time a user hits "record" there isn't a big delay.
        recordScreen()->newVideo();
    }

    if (cfg()->getBool(Config::CONTINUOUS_RECORD_ENABLED)) {
        int ix;
        char buf[30];
        for (ix=0; ix < 2; ix++) {
            if (secondDisplay())
                sprintf(buf,"cont%02db",ix);
            else
                sprintf(buf,"cont%02d",ix);
            _continuous[ix] = new Video();
            _continuous[ix]->setPath(videos()->defaultVideoDir());
            _continuous[ix]->title = "Timeshift";
            _continuous[ix]->infofile = NULL;
            _continuous[ix]->mediafile = MString(buf) + ".fifompg";
            _continuous[ix]->indexfile = MString(buf) + ".fifonav";
            _continuous[ix]->encodingQuality = bencode_quality_best;
            _continuous[ix]->isContinuousRecord = true;
            _continuous[ix]->continuousRecordDuration = cfg()->getInt(Config::CONTINUOUS_RECORD_DURATION);
        }
    }
#endif
#ifdef PICTURES_SUPPORT
    if (cfg()->getBool(Config::PICTURES_ENABLED)) {
        W("PICS");
        _screens[ePictures] = new PicturesScreen(this);
    }
#endif
#ifdef ADMIN_SUPPORT
    W("SC8");
    _screens[eAdmin] = new AdminScreen(this);
#ifdef SCRIPT_SUPPORT
    W("SC9");
    _screens[eRunScript] = new RunScriptScreen(this);
#endif
    W("SC10");
    _screens[eEditChannels] = new EditChannelsScreen(this);
#endif
#ifdef B_HAS_DRM_ND
    W("SC11");
    _screens[eExtserver] = new ExtServerScreen(this);
#endif
    _screens[eIpStbConfig] = new IpStbConfigScreen(this);

    _screens[eChanMap] = new ChanMapScreen(this);

    W("TUNE");
    if (cfg()->getBool(Config::START_FULLSCREEN)) {
        showScreen(eTV);
        /* we have to hide the borders initially */
        _brutus->setFullscreenGraphics(false);
    } else {
        showScreen(eHome);
        /* we have to draw the borders initially */
        _brutus->setFullscreenGraphics(true);
    }

    /* if the user hasn't specified a first channel, search for the first non-disabled channel */
    if (!cfg()->get(Config::FIRST_CHANNEL)) {
        ChannelMap::Channel ch;
        if (!channelMap()->getChannel(0, &ch) && ch.entry.disabled) {
            cfg()->set(Config::FIRST_CHANNEL, channelMap()->findNextChannel(0, 1));
        }
    }

#if B_HAS_DRM
    _msgDRMStatus = new MMessageBox(fb, 300, 100);
#endif

#ifdef HDMI_SUPPORT
    if (!secondDisplay())
    {
        W("SC12");
        _screens[eHdmi] = new HdmiScreen(this);
    }
#endif

#ifdef HDMI_CEC_SUPPORT
    W("SC13");
    _screens[eHdmiCec] = new HdmiCecScreen(this);
#endif

    _valid = true;
    BKNI_Printf(
        "\n"
        "Brutus is running.\n"
        "\n");
}

void Control::stopAllAudioVideo()
{
#ifdef AUDIO_SUPPORT
    if (audioControl() && audioControl()->isPlaying())
        audioControl()->stopAudio();
#endif
#ifdef PVR_SUPPORT
    for (int i=0;i<_totalDecodes;i++) {
        stopPlayback(i);
    }
#endif
    for (int i=0;i<_totalDecodes;i++) {
        stopLiveDecode(i);
    }
}

void Control::startLiveDecode()
{
    int saveCurrentDecode = _currentDecode;
    for (int i=0;i<_totalDecodes;i++) {
        _currentDecode = i;
        setChannel(channel(i));
    }
    _currentDecode = saveCurrentDecode;
}

Control::~Control() {
    stopAllAudioVideo();
#ifdef PVR_SUPPORT
    stopAllPvr(); /* this also stops record */
#endif

#if B_HAS_DRM
    if (_msgDRMStatus) {
        delete _msgDRMStatus;
    }
#endif

#if NEXUS_NUM_MOSAIC_DECODES
    /* If we created any mosaic windows (for 3D), get rid of them */
    if (g_mosaicWindow[0]) {
        NEXUS_VideoWindow_RemoveAllInputs(g_mosaicWindow[0]);
        NEXUS_VideoWindow_Close(g_mosaicWindow[0]);
        g_mosaicWindow[0] = NULL;
    }

    if (g_mosaicWindow[1]) {
        NEXUS_VideoWindow_RemoveAllInputs(g_mosaicWindow[1]);
        NEXUS_VideoWindow_Close(g_mosaicWindow[1]);
        g_mosaicWindow[1] = NULL;
    }
#endif

    /* Free up settop api handles */
    for (int i=MAX_WINDOWS-1;i>=0;i--) { /* free in reverse order so that cloned windows
                                close first. */
        if (_window[i]) {
            bdecode_window_close(_window[i]);
        }
    }
#ifdef PVR_SUPPORT
    if (_still_decode) {
        bdecode_close(_still_decode);
        _still_decode = NULL;
    }
    if (_thumbnail.manager) {
        bthumbnail_manager_destroy(_thumbnail.manager);
    }
#ifdef NEXUS_BUILD
    if (_thumbnail.extractor) {
        bthumbnail_extractor_destroy(_thumbnail.extractor);
    }
#endif
#endif
#ifdef BRUTUS_LIBTUNE_SUPPORT
    LibTune_Close(libtune.handle);
#endif
    for (int i=0;i<2;i++) {
        if (_decode[i]) {
            DecodeEngine::Settings settings;
            _decode[i]->getSettings(&settings);

            bdecode_close(settings.decode);
            _decode[i] = NULL;
        }
    }

    /* some screens are connected to PlayEngine's, so we must free screens first. */
    for (int s = 0; s < eTotalScreens; s++) {
        if (_screens[s]) {
            delete _screens[s];
            _screens[s] = NULL;
        }
    }

#ifdef PVR_SUPPORT
    /* Check out all the PlayEngine's and destroy them */
    PlayEngine *play;
    while ((play = _playbackManager->checkOut())) {
        delete play;
    }

    if (_playbackManager) {
        delete _playbackManager;
        _playbackManager = NULL;
    }
#endif

    /* Free control structores */
    for (int i=0;i<_totalDecodes;i++) {
        if (_decode[i]) {
            delete _decode[i];
            _decode[i] = NULL;
        }
    }

    if (_decodeManager) {
        delete _decodeManager;
        _decodeManager = NULL;
    }

    if (_leds) {
        delete _leds;
        _leds = NULL;
    }

#ifdef AUDIO_SUPPORT
    if (_audioControl) {
        delete _audioControl;
        _audioControl = NULL;
    }
#endif
#if defined LIVEMEDIA_SUPPORT
    liveMediaClose();

    /* stop the IP Channel Acquisition */
    ipChannelLineupAcquisitionStop();
#endif

#ifdef B_HAS_DTCP_IP
    DtcpAppLib_Shutdown(_ipVideo->dtcpIpCtx);
#endif
}

bool Control::deferredSetChannelPending() const
{
    return _deferSetChannel.tickCount;
}

/**
* MOVE button
**/
void Control::moveWindowControl() {
    // Both Main and PIP must be visible, otherwise things get goofy
    if (!_pipVisible || currentScreen() != eTV)
        return;

    // finish any deferred SetChannel
    if (_deferSetChannel.tickCount) {
        doDeferredSetChannel();
    }

    _currentDecode = 1 - _currentDecode;
    enableAudio(_currentDecode);
    tv()->showPipOutline(_currentDecode == _windowSource[1]);
    showModeMessage();
    fireEvent(evMoveWindowControl);
}

/**
* SWAP button
**/
void Control::swapWindows() {
#ifdef LIMITED_PIP_SIZE
    MMessageBox::info(fb(), "PIP swap not supported on this platform");
    return;
#endif

    // Both Main and PIP must be visible, otherwise things get goofy
    if (!_pipVisible || currentScreen() != eTV)
        return;

    _windowSource[0] = 1-_windowSource[0];
    _windowSource[1] = 1-_windowSource[1];
    _currentDecode = 1-_currentDecode;
    tv()->showPipOutline(_currentDecode == 1);

    /* show top one first to release MAD to the bottom full-screen */
    showWindow(1);
    showWindow(0);

    showModeMessage();
#ifdef PVR_SUPPORT
    tv()->setPlayEngines();
#endif
    moveWindowControl();
}

bool Control::areWindowsSwapped() const {
    return _windowSource[0] == 1;
}

/* We need an absolute hide because of auto-hides in other places */
void Control::hidePip()
{
    if (!pipEnabled()) return;

    // Need to explicity hide because main won't hide by itself
    hideWindow(1);
    if (_currentScreen != eTV)
        showWindow(0);

    // stop PIP live decode
    stopLiveDecode(_windowSource[1]);

    tv()->hidePipOutline();
    _pipVisible = false;
    _currentDecode = _windowSource[0];

    enableAudio(_windowSource[0]);
}

/**
* Toggles whether pip display is visible or invisible. If pip is visible but not current,
* it will be made invisible and main will remain current.
*
* NOTE about swapping:
* source 0 is always pvr(0)
* display 0 is always main display, which only corresponds to source 0 in unswapped state.
**/
void Control::togglePip() {
    // handle special conditions
    if (!pipEnabled() || _totalDecodes < 2)
        return;

    // finish any deferred SetChannel
    if (_deferSetChannel.tickCount) {
        doDeferredSetChannel();
    }

    if (!_pipVisible) {

        // here's the toggle
        _currentDecode = _windowSource[1];

        enableAudio();

        // start PIP's live decode if not PVR
        BDBG_MSG(("Start livedecode for PIP"));
        if (!isPlaying())
            setChannel(channel(), eDecode_Immediate);   /* This must be an immediate tune, else there is a race condition with decode_window */

        /* When showing PIP when resized, we need to hide main, otherwise if there's
        no PIP video, we'll see main instead. This way, we'll see black. */
        if (_currentScreen != eTV)
            hideWindow(0);

        showWindow(1);

        tv()->showPipOutline(true);
        _pipVisible = true;
    }
    else {
        hidePip();
    }

    // Allow changeable PIP states to resize the windows as necessary
    if (tv()->wow()->IsChangeable())
        setWindowSizes();

    if (currentScreen() != eTV)
        ((GenericScreen*)screen(currentScreen()))->repaintTVBevel();
    showModeMessage();
    fireEvent(evMoveWindowControl);
}

/* showWindow is called from all over the app with 0/1.
For HDSD_SINGLE mode, we recursively call it again with 2/3. */
void Control::showWindow(int window) {
    if (window <= 1 && cfg()->getBool(Config::HDSD_SINGLE)) {
        showWindow(window+2);
        /* then fall through */
    }

    static bool hd_contentmodeSwitched = false;
    static bool sd_contentmodeSwitched = false;
    static bool connectedFullScreen = true;
    bool hdsd_single = cfg()->getBool(Config::HDSD_SINGLE);

#if NEXUS_NUM_MOSAIC_DECODES
    NEXUS_VideoWindowHandle parentWindow[2];
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_Error rc;

    if (window == 0) {
        videoDecoder = getDecode(0)->video_decode->nVideoDecoder;
        parentWindow[0] = _window[0]->nWindow;
        parentWindow[1] = NULL;
        if (hdsd_single) {
            parentWindow[1] = _window[2]->nWindow;
        }
    }

    /* For the first window, check to see if 3D mode is enabled, if so,
       bypass the bsettop stuff and adjust the sizes of the nexus mosaic windows */
    if (window == 0 && display()->s3dTv.output>Brutus3DTVMode_eOff) {
        /* Adjust the Nexus mosaic window sizes to correspond to the parent's size */
        NEXUS_VideoWindowSettings mosaicWindowSettings[2];
        NEXUS_VideoWindowSettings parentWindowSettings;
        NEXUS_VideoWindowMosaicSettings mosaicSettings;
        NEXUS_VideoFormatInfo displayFormatInfo;
        NEXUS_DisplaySettings displaySettings;

        NEXUS_Display_GetSettings(_window[0]->display->nDisplay, &displaySettings);
        NEXUS_VideoFormat_GetInfo(displaySettings.format, &displayFormatInfo);

        if (connectedFullScreen) {
            NEXUS_VideoWindow_RemoveAllInputs(parentWindow[0]);
            if (hdsd_single) {
                NEXUS_VideoWindow_RemoveAllInputs(parentWindow[1]);
            }
        }
        else {
            if (hdsd_single) {
                NEXUS_VideoWindow_RemoveAllInputs(parentWindow[1]); /* must be done first */
            }
            NEXUS_VideoWindow_RemoveAllInputs(g_mosaicWindow[0]);
            NEXUS_VideoWindow_RemoveAllInputs(g_mosaicWindow[1]);
        }
        NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));

        if (window==0) {
#if 0
            /* restore any clipping that may have been done from 3D->2D conversion */
            bdecode_window_settings settings;
            bdecode_window_get(_window[0], &settings);
            settings.cliprect.x = 0;
            settings.cliprect.y = 0;
            settings.cliprect.height = 0;
            settings.cliprect.width = 0;
            bdecode_window_set(_window[0], &settings);
#endif

            /* restore previous content mode from before 3D->2D conversion */
            if (hd_contentmodeSwitched) {
                bdisplay_get(display()->display, &_display_settings);
                _display_settings.content_mode = bdisplay_content_mode_box;
                bdisplay_set(display()->display, &_display_settings);
                hd_contentmodeSwitched = false;
            }
        }

        if (currentScreen() == eTV) {
            NEXUS_VideoWindow_RemoveAllInputs(parentWindow[0]);
            if (hdsd_single) {
                NEXUS_VideoWindow_RemoveAllInputs(parentWindow[1]);
            }
#if 0
            NEXUS_VideoWindow_GetSettings(parentWindow[0], &parentWindowSettings);
            parentWindowSettings.visible = true;
            parentWindowSettings.position.x = 0;
            parentWindowSettings.position.y = 0;
            parentWindowSettings.position.width = displayFormatInfo.width;
            if (displayFormatInfo.height == 482) {
                parentWindowSettings.position.height = 480;
            }
            else {
                parentWindowSettings.position.height = displayFormatInfo.height;
            }
            rc = NEXUS_VideoWindow_SetSettings(parentWindow[0], &parentWindowSettings);
            BDBG_ASSERT(!rc);
#endif

            rc = NEXUS_VideoWindow_AddInput(parentWindow[0], NEXUS_VideoDecoder_GetConnector(videoDecoder));
            BDBG_ASSERT(!rc);

            if (hdsd_single) {
                /* this handles the 3D->2D conversion on the SD outputs, in full-screen video mode.
                   no conversion is necessary in PIG because we're relying on the behavior that a cloned
                   window's source will be the parent window's mosaic source.

                   use bdecode_window_set instead of NEXUS_VideoWindow_SetSettings to defer the content
                   mode handling to shim */
#if 1
                bdecode_window_settings settings;
                int source = _windowSource[0];
                bdecode_window_t w = _window[source+2];

                bdecode_window_get(w, &settings);
                settings.visible = 1;
                settings.position.x = _windowRect[2].x();
                settings.position.y = _windowRect[2].y();
                settings.position.width = _windowRect[2].width();
                settings.position.height = _windowRect[2].height();
                settings.zorder = 0;
                if (display()->s3dTv.output == Brutus3DTVMode_eSBS) {
                    if ( cfg()->getBool(Config::FORCE_COMPOSITE_OUTPUT_AR_FOR_3D_TO_2D) ) {
                        /* do A/R adjust using clipping */
                        settings.cliprect.width  = _windowRect[2].width()/2;
                        settings.cliprect.height = _windowRect[2].height()*6/8;

                        settings.position.y      = _windowRect[2].height()/8;
                        settings.position.height = _windowRect[2].height()*6/8;
                    }
                    else {
                        settings.cliprect.width = _windowRect[2].width()/2;
                        settings.cliprect.height = _windowRect[2].height();
                    }
                }
                else {
                    settings.cliprect.width = _windowRect[2].width();
                    settings.cliprect.height = _windowRect[2].height()/2;
                }
                bdecode_window_set(w, &settings);

                if (!sd_contentmodeSwitched) {
                    bdisplay_get(display()->display2, &_display_settings2);
                    _display_settings2.content_mode = bdisplay_content_mode_full;
                    bdisplay_set(display()->display2, &_display_settings2);
                    sd_contentmodeSwitched = true;
                    fireEvent(evSetOutput);
                }
#endif

                rc = NEXUS_VideoWindow_AddInput(parentWindow[1], NEXUS_VideoDecoder_GetConnector(videoDecoder));
                BDBG_ASSERT(!rc);
            }
            connectedFullScreen = true;
        }
        else {
            /* Get the parent window's settings so we know how to resize the mosaic's */
            NEXUS_VideoWindow_GetSettings(parentWindow[0], &parentWindowSettings);
#if 0
            parentWindowSettings.visible = false;
            rc = NEXUS_VideoWindow_SetSettings(parentWindow[0], &parentWindowSettings);
            BDBG_ASSERT(!rc);
#endif
            parentWindowSettings.position.x = _windowRect[window].x();
            parentWindowSettings.position.y = _windowRect[window].y();
            parentWindowSettings.position.width = _windowRect[window].width();
            parentWindowSettings.position.height = _windowRect[window].height();

            /* Now resize the left mosaic window */
            NEXUS_VideoWindow_GetSettings(g_mosaicWindow[0], &mosaicWindowSettings[0]);
            mosaicWindowSettings[0].visible = false;
            if (display()->s3dTv.output == Brutus3DTVMode_eSBS) {
                mosaicWindowSettings[0].position.x = parentWindowSettings.position.x / 2;
                mosaicWindowSettings[0].position.y =  parentWindowSettings.position.y;
                mosaicWindowSettings[0].position.width  = parentWindowSettings.position.width / 2;
                mosaicWindowSettings[0].position.height = parentWindowSettings.position.height;
            }
            else {
                mosaicWindowSettings[0].position.x = parentWindowSettings.position.x;
                mosaicWindowSettings[0].position.y = parentWindowSettings.position.y / 2;
                mosaicWindowSettings[0].position.width = parentWindowSettings.position.width;
                mosaicWindowSettings[0].position.height = parentWindowSettings.position.height / 2;
            }
            rc = NEXUS_VideoWindow_SetSettings(g_mosaicWindow[0], &mosaicWindowSettings[0]);
            BDBG_ASSERT(!rc);

            /* Now define the source rectangle for the left mosaic */
            NEXUS_VideoWindow_GetMosaicSettings(g_mosaicWindow[0], &mosaicSettings);
            mosaicSettings.backendMosaic.enabled = true;
            mosaicSettings.backendMosaic.clipRect.x = 0;
            mosaicSettings.backendMosaic.clipRect.y = 0;
            if (display()->s3dTv.output == Brutus3DTVMode_eSBS) {
                mosaicSettings.backendMosaic.clipRect.width = 1920/2;
                mosaicSettings.backendMosaic.clipRect.height = 1080;
            }
            else {
                mosaicSettings.backendMosaic.clipRect.width = 1920;
                mosaicSettings.backendMosaic.clipRect.height = 1080/2;
            }
            mosaicSettings.backendMosaic.clipBase.width = 1920;
            mosaicSettings.backendMosaic.clipBase.height = 1080;
            rc = NEXUS_VideoWindow_SetMosaicSettings(g_mosaicWindow[0], &mosaicSettings);
            BDBG_ASSERT(!rc);

            /* Now resize the right mosaic window */
            NEXUS_VideoWindow_GetSettings(g_mosaicWindow[1], &mosaicWindowSettings[1]);
            mosaicWindowSettings[1].position = mosaicWindowSettings[0].position;
            mosaicWindowSettings[1].visible = false;
            if (display()->s3dTv.output == Brutus3DTVMode_eSBS) {
                mosaicWindowSettings[1].position.x = mosaicWindowSettings[1].position.x + displayFormatInfo.width / 2;
            }
            else {
                mosaicWindowSettings[1].position.y = mosaicWindowSettings[1].position.y + displayFormatInfo.height / 2;
            }
            rc = NEXUS_VideoWindow_SetSettings(g_mosaicWindow[1], &mosaicWindowSettings[1]);
            BDBG_ASSERT(!rc);

            /* Now define the source rectangle for the right mosaic */
            NEXUS_VideoWindow_GetMosaicSettings(g_mosaicWindow[1], &mosaicSettings);
            mosaicSettings.backendMosaic.enabled = true;
            if (display()->s3dTv.output == Brutus3DTVMode_eSBS) {
                mosaicSettings.backendMosaic.clipRect.x = 1920/2;
                mosaicSettings.backendMosaic.clipRect.y = 0;
                mosaicSettings.backendMosaic.clipRect.width = 1920/2;
                mosaicSettings.backendMosaic.clipRect.height = 1080;
            }
            else {
                mosaicSettings.backendMosaic.clipRect.x = 0;
                mosaicSettings.backendMosaic.clipRect.y = 1080/2;
                mosaicSettings.backendMosaic.clipRect.width = 1920;
                mosaicSettings.backendMosaic.clipRect.height = 1080/2;
            }
            mosaicSettings.backendMosaic.clipBase.width = 1920;
            mosaicSettings.backendMosaic.clipBase.height = 1080;
            rc = NEXUS_VideoWindow_SetMosaicSettings(g_mosaicWindow[1], &mosaicSettings);
            BDBG_ASSERT(!rc);

            rc = NEXUS_VideoWindow_AddInput(g_mosaicWindow[0], NEXUS_VideoDecoder_GetConnector(videoDecoder));
            BDBG_ASSERT(!rc);
            rc = NEXUS_VideoWindow_AddInput(g_mosaicWindow[1], NEXUS_VideoDecoder_GetConnector(videoDecoder));
            BDBG_ASSERT(!rc);

            NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eManual);
            NEXUS_VideoWindow_GetSettings(g_mosaicWindow[0], &mosaicWindowSettings[0]);
            mosaicWindowSettings[0].visible = true;
            rc = NEXUS_VideoWindow_SetSettings(g_mosaicWindow[0], &mosaicWindowSettings[0]);
            BDBG_ASSERT(!rc);
            NEXUS_VideoWindow_GetSettings(g_mosaicWindow[1], &mosaicWindowSettings[1]);
            mosaicWindowSettings[1].visible = true;
            rc = NEXUS_VideoWindow_SetSettings(g_mosaicWindow[1], &mosaicWindowSettings[1]);
            BDBG_ASSERT(!rc);
            NEXUS_DisplayModule_SetUpdateMode(NEXUS_DisplayUpdateMode_eAuto);

            if (hdsd_single) {
                rc = NEXUS_VideoWindow_AddInput(parentWindow[1], NEXUS_VideoDecoder_GetConnector(videoDecoder));
                BDBG_ASSERT(!rc);
            }

            connectedFullScreen = false;
        }

        _brutus->resetGraphics(); /* stream-triggered 2D/3D transitions require a graphics reset */
        return;
    }

    /* 2D mode */

    if (window == 0 && (!connectedFullScreen && currentScreen()!=eTV)) { /* needed to allow 3D->2D switch in PIG */
        if (hdsd_single) {
            NEXUS_VideoWindow_RemoveAllInputs(parentWindow[1]); /* must be done first */
        }
        NEXUS_VideoWindow_RemoveAllInputs(g_mosaicWindow[0]);
        NEXUS_VideoWindow_RemoveAllInputs(g_mosaicWindow[1]);
        NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));

        rc = NEXUS_VideoWindow_AddInput(parentWindow[0], NEXUS_VideoDecoder_GetConnector(videoDecoder));
        BDBG_ASSERT(!rc);
        if (hdsd_single) {
            rc = NEXUS_VideoWindow_AddInput(parentWindow[1], NEXUS_VideoDecoder_GetConnector(videoDecoder));
            BDBG_ASSERT(!rc);
        }
    }

    if (display()->s3dTv.output > Brutus3DTVMode_eOff) {
        if (!connectedFullScreen && currentScreen()==eTV) { /* PIG -> fullscreen transition */
            return; /* this is a hack to reduce flicker on right mosaic window during PIG -> fullscreen transition */
        }
    }

    _brutus->resetGraphics(); /* stream-triggered 2D/3D transitions require a graphics reset */

#endif /* NEXUS_NUM_MOSAIC_DECODES */

    bdecode_window_settings settings;
    int source = _windowSource[window%2];
    bdecode_window_t w = _window[window>1?source+2:source];
    bool updateGUI = false;

    if (!w)
        return;

    bdecode_window_get(w, &settings);
    settings.visible = 1;
    if (_windowOverride[window].width()) {
        settings.position.x = _windowOverride[window].x();
        settings.position.y = _windowOverride[window].y();
        settings.position.width = _windowOverride[window].width();
        settings.position.height = _windowOverride[window].height();
    }
    else {
        settings.position.x = _windowRect[window].x();
        settings.position.y = _windowRect[window].y();
        settings.position.width = _windowRect[window].width();
        settings.position.height = _windowRect[window].height();
    }
    settings.zorder = window%2; // PIP is always on top, therefore use 1
    settings.cliprect.width = 0;
    settings.cliprect.height = 0;

    /* 3D source, 2D output */
    if (display()->s3dTv.source > Brutus3DTVMode_eOff) {
        if (window==0) {
            /* this handles 3D->2D conversion on the HD outputs */
            settings.cliprect.x = 0;
            settings.cliprect.y = 0;
            if (display()->s3dTv.source == Brutus3DTVMode_eSBS) {
            }
            else {
                settings.cliprect.height = settings.position.height/2;
                settings.cliprect.width = settings.position.width;
            }
        }
        else if (window==2) {
            if (display()->s3dTv.output == Brutus3DTVMode_eOff) {
                /* needed because clipping code in 3D mode did not execute  */
                if (display()->s3dTv.source == Brutus3DTVMode_eSBS) {

                    if ( cfg()->getBool(Config::FORCE_COMPOSITE_OUTPUT_AR_FOR_3D_TO_2D) ) {
                        if (currentScreen() == eTV) {
                            /* do A/R adjust using clipping */
                            settings.cliprect.width = _windowRect[2].width()/2;

                            settings.position.y = _windowRect[2].height()/8;
                            settings.position.height = _windowRect[2].height()*6/8;
                            settings.cliprect.height = _windowRect[2].height()*6/8;
                        }
                        else {
                            /* revert A/R settings during PIG */
                            settings.cliprect.width = _windowRect[2].width()/2;

                            settings.position.y = _windowRect[2].y();
                            settings.position.height = _windowRect[2].height();
                            settings.cliprect.height = _windowRect[2].height();
                        }
                    }
                    else {
                        settings.cliprect.height = settings.position.height;
                        settings.cliprect.width = settings.position.width/2;
                    }

                }
                else {
                    settings.cliprect.height = settings.position.height/2;
                    settings.cliprect.width = settings.position.width;
                }
            }
        }

        /* force content mode to full */
        if (!hd_contentmodeSwitched) {
            bdisplay_get(display()->display, &_display_settings);
            _display_settings.content_mode = bdisplay_content_mode_full;
            bdisplay_set(display()->display, &_display_settings);
            hd_contentmodeSwitched = true;
            updateGUI = true;
        }
#if 0
        if (!sd_contentmodeSwitched) {
            bdisplay_get(display()->display2, &_display_settings2);
            _display_settings2.content_mode = bdisplay_content_mode_full;
            bdisplay_set(display()->display2, &_display_settings2);
            sd_contentmodeSwitched = true;
            updateGUI = true;
        }
#endif
    }
    else {
#if 0
        if (sd_contentmodeSwitched) {
            /* restore content mode for SD outputs */
            bdisplay_get(display()->display2, &_display_settings2);
            _display_settings2.content_mode = bdisplay_content_mode_box;
            bdisplay_set(display()->display2, &_display_settings2);
            sd_contentmodeSwitched = false;
            updateGUI = true;
        }
#endif
    }

    if (window == 0) {
        connectedFullScreen = true;
    }

    if (updateGUI) {
        fireEvent(evSetOutput);
    }

    BDBG_MSG(("window[%d] %s %s %d,%d,%d,%d",
        window,settings.visible?"show":"hide",
        settings.zorder?"top":"bottom",
        settings.position.x,settings.position.y,settings.position.width,settings.position.height));
    bdecode_window_set(w, &settings);
}

/* hideWindow only accepts window 0 & 1 and takes care of 2 & 3 if needed. */
void Control::hideWindow(int window) {
    if (window <= 1 && cfg()->getBool(Config::HDSD_SINGLE)) {
        hideWindow(window+2);
        /* then fall through */
    }

    bdecode_window_settings settings;
    int source = _windowSource[window%2];
    bdecode_window_t w = _window[window>1?source+2:source];

    if (!w)
        return;

    bdecode_window_get(w, &settings);
    // Hiding a display that's already hidden causes problems because
    // certain PI code will get re-run. This is the spot to prevent it.
    if (settings.visible) {
        settings.visible = 0;
        bdecode_window_set(w, &settings);
    }
}

/**
* setWindowSizes is called when the screen changes or when output
* mode changes. _windowRect[] is set, then showWindow/hideWindow is called
* as needed. showWindow is necessary to apply _windowRect[] changes.
*
* If showMain is false, then this routine won't show the main decode window.
* This is needed for clean transitions into and out of RMM mode. See
* showScreen() for logic.
*/
void Control::setWindowSizes(int display) {
    BDBG_MSG(("Control::setWindowSizes"));

    /* By default, this is called with -1 and then we figure out which
    displays to work with */
    if (display == -1) {
        /* Implement this with recursion into this function */
        if (cfg()->getBool(Config::HDSD_SINGLE))
            setWindowSizes(1);

        /* And fall-through to handle display 0 */
        display = 0;
    }

    /* Usually we access window0 & 1, but when we're doing HDSD_SINGLE,
    we also want window2 & 3 along with _display_settings2. */
    int base = display?2:0;
    bdisplay_settings *display_settings = display?&_display_settings2:&_display_settings;

    /* calc the window sizes */
    if (_currentScreen == eTV || !cfg()->getBool(Config::SCALED_VIDEO_ENABLED)) {
        _windowRect[base] = tv()->wow()->MainRect(eFullScreenTV, display_settings);
        if (pipEnabled())
            /* this is pip */
            _windowRect[base+1] = tv()->wow()->WowRect(eFullScreenTV, display_settings);
        else
            /* this is 2nd display */
            _windowRect[base+1] = tv()->wow()->MainRect(eFullScreenTV, display_settings);
    }
    else {
        eScreenType destScreenType;
        if (_currentScreen == eHome)
            destScreenType = eLargeTV;
        else
            destScreenType = eSmallTV;

        _windowRect[base] = tv()->wow()->MainRect(destScreenType, display_settings);
        _windowRect[base+1] = tv()->wow()->MainRect(destScreenType, display_settings);
    }



    if (_skin->isValid(_screenname[_currentScreen]))
    {
        eScreenType destScreenType;
        destScreenType = eFullScreenTV;

        //get size of tvwindow from skin
        _windowRect[base] = _skin->getTVWindowSize(_screenname[_currentScreen],
                                                   display_settings);

        if (pipEnabled())
        {
            /* this is pip */

            /* In order to allow side-by-side to work with skins, the following is necessary until
               the skin over-rides work with side-by-side */
            if (tv()->wow()->IsChangeable())
                _windowRect[base+1] = tv()->wow()->WowRect(eFullScreenTV, display_settings);
            else
                _windowRect[base+1] = _skin->getPIPWindowSize(_screenname[_currentScreen], display_settings);
        }
        else
        {
            /* this is 2nd display */
            _windowRect[base+1] = _skin->getTVWindowSize(_screenname[_currentScreen], display_settings);
        }

        MRect rectWin = tv()->wow()->MainRect(eFullScreenTV, display_settings);

        if ((_windowRect[base].x() + _windowRect[base].width() >
             rectWin.width()) ||
            (_windowRect[base].y() + _windowRect[base].height() >
             rectWin.height()) ||
            (_windowRect[base].width() == 0) ||
            (_windowRect[base].height() == 0)
           )
        {
            //specified size is outside allowable range so
            //revert to full screen settings
            _windowRect[base] = rectWin;
        }
    }

    /* If this is the 2nd display, short circuit here because show/hideWindow
    take care of both themselves. */
    if (display == 1)
        return;

    /* perform show/hide */
    if (_currentScreen == eTV) {
        if (!secondDisplay()) {
            showWindow(0);
        }
        if (_pipVisible || secondDisplay())
            showWindow(1);
    }
    else {
        /* hide the 2nd window first to release MAD to the focused window */
        if (pipEnabled() && !secondDisplay())
            hideWindow(1-currentWindow());
        showWindow(currentWindow());
    }
}

void Control::showScreen(Screen scr) {
    BDBG_ASSERT(scr < eTotalScreens);

    if (fb()->app()->inModalState()) {
        BDBG_ERR(("Modal dialog box is currently displayed. You cannot change screens."));
        BDBG_ERR(("Remember that Brutus scripts must dismiss all modals."));
        BDBG_ERR(("Unanticipated modals lead to script failure."));
        return;
    }

    GenericScreen *nextScreen = _screens[scr];
    GenericScreen *cur = screen(currentScreen());
    Screen org_scr = _currentScreen;

#if defined(LIVEMEDIA_SUPPORT)
    if(scr == eRecord)
    {
        ChannelMap::Channel currentChannel;

        if(!channelMap()->getChannel(channel(), &currentChannel) &&
                   currentChannel.entry.getType() == btuner_ip_stream &&
           currentChannel.entry.ip.params.open_params.protocol == bsettop_ip_protocol_rtsp ||
           currentChannel.entry.ip.params.open_params.protocol == bsettop_ip_protocol_http) {
            MMessageBox mbox(fb(), 350, 200);
            mbox.set("RTSP record not supported", MMessageBox::bOK);
            mbox.setWrapMode(MLabel::NewLineWrap);
            mbox.process();
            return;
        }
    }
#endif

#ifdef PVR_SUPPORT
    if (scr == ePlayback) {
        if (_initialDiskReadCounter) {
            /* consider fixing this */
            BDBG_WRN(("The playback list has not been populated yet."));
        }
    }
#endif

    BDBG_MSG(("showScreen: %d ==> %d", _currentScreen, scr));

#ifdef ADMIN_SUPPORT
    if (currentScreen() == eEditChannels) {
        ((EditChannelsScreen*)cur)->checkModified();
    }
#endif
    if (cur)
        cur->rememberFocus();
    _currentScreen = scr;

    // the format of the msg might change
    showModeMessage();

    // cur is now the previous screen
    if (cur && cur != nextScreen) {
        if (cur->visible())
            cur->hide();
    }

    nextScreen->show();
    nextScreen->focus();

    /* get all the drawing work done now, before resizing the decode windows. this
    way the UI will appear more in sync to the user */
    while (fb()->app()->eventPending()) {
        if (!fb()->app()->processEvent(true)) break;
    }
    fb()->app()->clearModalState();

    if (org_scr == eTV && scr != eTV) {
        fade(eFadeIn);
    }
    else if (org_scr != eTV && scr == eTV) {
        fade(eFadeOut);
    }

    if (nextScreen->hasTV() || scr == eTV)
    {
        setWindowSizes();
    }
}

void Control::flipDone()
{
    /* Check if we're in a fade, which is an override */
    if (!_inFade) {
        unsigned short alpha = (currentScreen() != eTV)?0xFF:_fullScreenAlpha;
/* NOTE: Uncomment the following if you want to test the picture-in-graphics overlap.
        alpha = _fullScreenAlpha;
*/

        if (_skin->isValid(XML_SCREEN_TV))
        {
            //if using a valid skin, then force alpha to always be fully opaque.
            //window/widget transparency will be handled by the skin
            alpha = 0xff;
        }

        /* After we sync, we can set the graphics engine properties.
        We make an exception if pip is visible because we want to cover up VBI. */
        bsurface_settings settings;
        bsurface_get(display()->actual_framebuffer, &settings);
        if (settings.alpha != alpha) {
            settings.alpha = alpha;
            bsurface_set(display()->actual_framebuffer, &settings);
        }
        if (display()->actual_framebuffer2) {
            bsurface_get(display()->actual_framebuffer2, &settings);
            if (settings.alpha != alpha) {
                settings.alpha = alpha;
                bsurface_set(display()->actual_framebuffer2, &settings);
            }
        }
    }
}

void Control::fade(FadeType fadetype)
{
    if (cfg()->getBool(Config::GUI_TRANSITIONS_ENABLED)) {
        bsurface_settings settings;
        eScreenType destScreenType;

        bsurface_get(display()->actual_framebuffer, &settings);

        if (fadetype == eFadeIn) {
            settings.alpha = 0;
            bsurface_set(display()->actual_framebuffer, &settings);

            if (_currentScreen == eHome)
                destScreenType = eLargeTV;
            else
                destScreenType = eSmallTV;

            /* we need to see the new screen immediately, but 0 alpha */
            _inFade = true;
            _brutus->setFullscreenGraphics(true);
            screen(currentScreen())->drawNow();
            _inFade = false;
        }
        else {
            destScreenType = eFullScreenTV;
        }

        /* show/hide the other decode window if needed */
        if (pipVisible()) {
            if (fadetype == eFadeIn && currentWindow() == 0) {
                hideWindow(1);
            }
            else if (fadetype == eFadeOut && currentWindow() == 1) {
                //_windowRect[0] = getWindowSize(0, destScreenType, &_display_settings);
                _windowRect[0] = tv()->wow()->MainRect(destScreenType, &_display_settings);
                showWindow(0);
            }
        }

        /* fade out/in */
        unsigned short alpha;
        unsigned short end;
        int inc;
        if (fadetype == eFadeIn) {
            alpha = 0;
            end = 0xFF;
            inc = 8;
        }
        else {
            alpha = 0xFF;
            end = 0;
            inc = -8;
        }

        //MRect destrect = getWindowSize(pipEnabled()?currentWindow():0, destScreenType, &_display_settings);
        MRect destrect;
        if (pipEnabled()) {
            destrect = tv()->wow()->WowRect(destScreenType, &_display_settings);
        } else {
            destrect = tv()->wow()->MainRect(destScreenType, &_display_settings);
        }

        bdecode_window_settings window_settings;
        bdecode_window_t window = _window[currentDecode()];

        if (!window) return;
        bdecode_window_get(window, &window_settings);

        BDBG_ASSERT(window_settings.position.width > 0);
        BDBG_ASSERT(window_settings.position.height > 0);

        int steps = (end - alpha) / inc;
        /* resize window with fixed point using .00 accurary. no need for float here. */
        int w_inc = ((int)destrect.width() - (int)window_settings.position.width)*100/steps;
        int h_inc = ((int)destrect.height() - (int)window_settings.position.height)*100/steps;
        int x_inc = (destrect.x() - window_settings.position.x)*100/steps;
        int y_inc = (destrect.y() - window_settings.position.y)*100/steps;
        int temp_w = window_settings.position.width * 100;
        int temp_h = window_settings.position.height * 100;
        int temp_x = window_settings.position.x * 100;
        int temp_y = window_settings.position.y * 100;

        while (inc>0?alpha<end:alpha>end) {
            settings.alpha = alpha;
            bsurface_set(display()->actual_framebuffer, &settings);

            alpha += inc;
            temp_w += w_inc;
            temp_h += h_inc;
            temp_x += x_inc;
            temp_y += y_inc;

            window_settings.position.x = temp_x / 100;
            window_settings.position.y = temp_y / 100;
            window_settings.position.width = temp_w / 100;
            window_settings.position.height = temp_h / 100;

            /* this allows our math to have rounding error */
            if (window_settings.position.x < 0 ||
                window_settings.position.y < 0) break;

            BDBG_MSG(("decode_window %d,%d,%d,%d, alpha %x",
                window_settings.position.x,
                window_settings.position.y,
                window_settings.position.width,
                window_settings.position.height, alpha));
            bdecode_window_set(window, &window_settings);

            /* TODO: do this with a timer and keep the UI active. I don't know
            if it's worth the work. */
        }
        if (fadetype == eFadeOut) {
            _brutus->setFullscreenGraphics(false);
        }
    }
    else {
        _brutus->setFullscreenGraphics(fadetype == eFadeIn);
    }
}

void Control::showVolumeMessage() {
    baudio_volume vol;

    /* Show graphics */
    tv()->showMessage();

    /* Show LEDS */
    bdisplay_get_dac_audio_volume(display()->display, &vol);
    if (vol.muted)
        W("MUTE");
    else {
        int level = (vol.left + 100) / 20;
        W(MString(level));
    }
}

void Control::showModeMessage() {
#ifdef PVR_SUPPORT
    if (recordScreen())
        recordScreen()->setSource();
    if (isPlaying()) {
        W("PLAY");
    }
    else
#endif
    {
        W("CH"+MString(MAINCH(channel())));
    }

    BDBG_MSG(("showModeMessage %d", channel()));
    tv()->showMessage();
    if (!tv()->visible())
        screen(currentScreen())->repaintTVBevel();
}

void Control::doDeferredSetChannel()
{
    BDBG_ASSERT(_deferSetChannel.tickCount);
    setChannel(_deferSetChannel.channel, eDecode_Immediate, _deferSetChannel.ignoreContinuous, _deferSetChannel.decodeOverride);
}

void Control::onTimeout(MTimer *timer) {
    //printf("Enter onTimeout[%d], %d\n", secondDisplay(), _deferSetChannel.tickCount);

    if (!channelManager()->isFrontEndPopulated()) {
        if (channelMap()->total()) {
            bsettop_init_frontend();
        }
        channelManager()->populateFrontEnd();

        const char *channelStr = cfg()->get(Config::FIRST_CHANNEL);
        unsigned channel = cfg()->getInt(Config::FIRST_CHANNEL);

        /* FIRST_CHANNEL=play(#) should start initial PVR */
        if (!channelStr || !strstr(channelStr, "play(")) {
            /* start live */
            setChannel(channel, eDecode_Immediate);
        }
    }

#ifdef PVR_SUPPORT
    /* */
    if (_initialDiskReadCounter) {
        if (_live[0] && _live[0]->isDecoding()) {
            /* we've started decode, so go right now */
            _initialDiskReadCounter = 0;
        }
        else {
            _initialDiskReadCounter -= 100;
        }
        if (!_initialDiskReadCounter) {
            /* Read Videos from disk. It will populate PlaybackScreen. */
            videos()->refreshFromDisk();

            /* FIRST_CHANNEL=play(#) should start initial PVR */
            const char *channelStr = cfg()->get(Config::FIRST_CHANNEL);
            if (channelStr && strstr(channelStr, "play(")) {
                /* start pvr */
                Video *video = videos()->get(atoi(channelStr+5)-1);
                if (video) {
                    if (playVideo(video)) {
                        showScreen(Control::eTV);
                    }
                }
            }

            /* scripts can be executed now */
            brutus()->_inputEngine->ready = true;
        }
    }
#else
    /* scripts can be executed now, no disk reads to wait for. */
    brutus()->_inputEngine->ready = true;
#endif

    /* provide timer for AVManager */
    _avManager->timer(MApplication::tickCount());

    // incremental channel change
    if (_incrementalChEntry.digits && _incrementalChEntry.timeout < (int)MApplication::tickCount()) {
        setIncrementalChannel();
    }

    /* see if a deferred SetChannel has expired */
    if (_deferSetChannel.tickCount && _deferSetChannel.tickCount < (int)MApplication::tickCount()) {
        doDeferredSetChannel();
    }

#ifdef PVR_SUPPORT
    /* This is a giant hack. If you don't want to keep recording, but you want the
    app to look like it is still recording, here's your solution. */
    if (cfg()->get(Config::MAX_RECORD_TIME)) {
        static int cnt = 0;
        if (cnt++ > 200) {  /* only check this every 2 seconds */
            cnt = 0;

            /* Check each record */
            int i = 0;
            for (RecordingStream *rs = _records.first(); rs; rs = _records.next(), i++) {
                Video *video = rs->record->video();
                int len = video->length()/1000;
                if (len > cfg()->getInt(Config::MAX_RECORD_TIME)) {
                    printf("MAX_RECORD_TIME exceeded. Restarting PVR for same file.\n");
                    stopRecord(i);
                    recordVideo(video, channel());
                    BKNI_Sleep(2000); /* make sure some index gets written */
                    playVideo(video);
                }
            }
        }
    }
#endif

#ifdef PVR_SUPPORT
    /* Add support for end_of_stream transitions to live decode, where possible */
    if (cfg()->get(Config::CONTINUOUS_RECORD_ENABLED)) {
        int ix;
        PlayEngine *pe;
        for (ix=0; ix < 2; ix++) {
            pe = playEngine(ix);
            if (pe &&
                (pe->state() == bplayback_state_playing ||
                 (pe->state() == bplayback_state_trickmode && pe->direction() > 0 && pe->isFastTrickMode())) &&
                pe->isStreamEnded())
            {
                /* go back to live mode */
                pe->clearStreamEnded();
                stopPlayback(pe->video(), true, true);
                BDBG_WRN(("continuous record: switch to live decode"));
            }
        }
    }

    /* Check for record errors */
    for (RecordingStream *rs = _records.first(); rs; rs = _records.next()) {
        if (rs->record) {
            checkForRecordError(rs->record);
        }
    }
#endif

    if (_hdmiHdcpState != boutput_hdmi_hdcp_state_disabled) {
        handleHdmiHdcp();
    }

    if (_check_hdmi_cec)
    {
        bresult src = b_ok;

        _check_hdmi_cec = false;
        src = hdmiCecHandler();
        if (src != b_ok)
            BDBG_WRN(("Error handling cec event"));
    }

    /* We need to check and possibly change the display. This is triggered by HDMI hotplug
    or by video source change. */
    if (_check_optimal_display) {
        _check_optimal_display = false;
        _check_3dtv = true;
        setOptimalDisplay();
    }

#if NEXUS_BUILD
    if (_check_3dtv) {
        _check_3dtv = false;

        if (currentDecode()==0) {
            NEXUS_VideoDecoderHandle videoDecoder = getDecode(0)->video_decode->nVideoDecoder;
            Brutus3DTVMode mode;
            parseSeiMessage(videoDecoder, &mode);
            brutus()->set3DTVMessage(mode);

            if (display()->s3dTv.automode) {
                if ((mode != display()->s3dTv.output) || (mode != display()->s3dTv.source)) {
                    if (mode > Brutus3DTVMode_eOff) {
                        BDBG_WRN(("Setting 3DTV mode to %s via 3DTV status callback", mode==Brutus3DTVMode_eSBS?"SBS":"OU"));
                    }
                    else {
                        BDBG_WRN(("Setting 3DTV mode to off"));
                    }

                    brutus()->_display.s3dTv.forceOutputChange = true; /* SEI-triggered output changes must call showWindow */
                    brutus()->set3DTVSource(mode); /* source tracks message */
                    brutus()->set3DTVOutput(mode); /* output tracks source */
                    fireEvent(evChangeChannel); /* use this to refresh info_video panel */
                }
            }
        }
    }
#else
    _check_3dtv = false;
#endif
#ifdef BRUTUS_LIBTUNE_SUPPORT
    if (_check_libtune) {
        _check_libtune = false;
        Libtune_SourceChangeHandler();
    }
#endif

    /* We need to set video DNR settings after decoder has started since NEXUS_VideoWindow_SetCustomDnrSettings
    has a snippet that checks whether a window is connected - if not the routine returns without propagating
    these settings to VDC */
    if(_check_dnr_settings){
        bdecode_window_settings ws;
        for (int i=0;i<_totalDecodes;i++) {
            bdecode_window_get(_window[i], &ws);
            bdecode_window_set(_window[i], &ws);
        }
        _check_dnr_settings = false;
        _apply_dnr_settings = false;
    }

#ifdef PVR_SUPPORT
    if (cfg()->getBool(Config::THUMBNAIL_MODE)) {
        if (!_still_decode) {
            BDBG_ERR(("Unable to decode thumbnail. No still decoder. Unset THUMBNAIL_MODE."));
        }
        else {
            process_still_decode();
        }
    }
#endif

    if (_audioSourceChanged) {
        _audioSourceChanged = false;
        fireEvent(evStartDecode);
    }
}

bool Control::verifyAdminPassword() {
    MString password;

    // if we're in admin state, don't bother with the password
    if (isAdmin())
        return true;

    if (MMessageBox::input(fb(), "Enter password", 80,
        MMessageBox::bOK|MMessageBox::bCancel, password, -1,
        MLineEdit::Password) == MMessageBox::bOK)
    {
        if (password == cfg()->get(Config::ADMIN_PASSWORD, "8822")) {
            return true;
        }
        else
            MMessageBox::info(fb(), "Incorrect password.");
    }
    return false;
}

void Control::updateLeds() {
    bool p = false;
#ifdef PVR_SUPPORT
    for (int i=0;i<_totalDecodes;i++)
        if (_decode[i] && _decode[i]->isPlaying()) {
            p = true;
            break;
        }
#endif
#if BCHP_CHIP != 7335 && BCHP_CHIP != 7325 && BCHP_CHIP !=7340 && BCHP_CHIP !=7342 && BCHP_CHIP !=7344 && BCHP_CHIP !=7346
    _leds->setLeds(
        1, /* power */
        p, /* play */
        totalRecords(), /* record */
        0); /* msg */
#endif
}

/* This is hooked up to any new AVStream for ui updates */
void Control::avstreamCallback(void *context)
{
    /* just update the UI */
    ((Control*)context)->showModeMessage();
}

int Control::channel(int source) const {
    if (source == -1)
        source = currentDecode();
    if (_deferSetChannel.tickCount && source == currentDecode())
        return _deferSetChannel.channel;
    else
        return _channel[source];
}

bdecode_window_t Control::window(int index) const
{
    if (index == -1)
        index = currentDecode();
    return _window[index];
}

btuner_t Control::tuner(int index) const {
    if (index == -1)
        index = currentDecode();
    if (_live[index])
        return _live[index]->tuner();

    /* timeshifting playbacks also have a tuner */
#ifdef PVR_SUPPORT
    if (_decode[index] && _decode[index]->isPlaying()) {
        Video *video = _decode[index]->playEngine()->video();
        if (video->tuner) {
            return video->tuner;
        }
    }
#endif
    return NULL;
}

void Control::changeChannel(int inc) {
    int ch = channelMap()->findNextChannel(channel(), inc);
    BDBG_MSG(("changeChannel(%d): %d->%d", inc, channel(), ch));

    setChannel(ch, eDecode_Deferred);

    /* Extend the wait time out slightly so that the user has time to chanup/down
    more without waiting */
     if (_deferSetChannel.tickCount)
        _deferSetChannel.tickCount += DEFER_SET_CHANNEL_TIME;
}

#ifdef B_HAS_TRANSCODE
void Control::transcodeChannel(bool transcodeEnable) {
    AVStream *trans = _avManager->find(bchannel_entry_static, channel());
    trans->startLiveTranscode(bencode_quality_best, transcodeEnable);
    showScreen(eTV);
}
#endif

void Control::getIncrementalChannel(int *ch, int *digits)
{
    *ch = _incrementalChEntry.ch;
    *digits  = _incrementalChEntry.digits;
}

void Control::addChannelDigit(int digit) {
    if (_incrementalChEntry.digits)
        _incrementalChEntry.ch *= 10;
    else
        _incrementalChEntry.ch = 0;
    _incrementalChEntry.ch += digit;
    _incrementalChEntry.digits++;

#if 1
    /* always 3 digit entry for TV's */
    int maxDigits = 3;
#else
    // max digits
    int maxDigits;
    if (channelMap()->totalChannels() > 99)
        maxDigits = 3;
    else if (channelMap()->totalChannels() > 9)
        maxDigits = 2;
    else
        maxDigits = 1;
#endif

    if (_incrementalChEntry.digits == maxDigits) {
        setChannel(_incrementalChEntry.ch, eDecode_Deferred);
    }
    else {
        _incrementalChEntry.timeout = MApplication::tickCount() + 1000;
        showModeMessage(); /* the ui will update */
    }
}

void Control::setIncrementalChannel()
{
    if (_incrementalChEntry.digits)
        setChannel(_incrementalChEntry.ch);
}

void Control::setLiveDecodeEnabled(bool enabled)
{
    if (_liveDecodeEnabled != enabled) {
        _liveDecodeEnabled = enabled;
        if (!_liveDecodeEnabled) {
            stopLiveDecode();
        }
        else {
            if (!isPlaying())
                setChannel(channel());
        }
    }
}

/**
* setChannel() is how channels are tuned and decode is started.
* For analog, we need to wait at least 150ms after tuning before starting decode.
*   Therefore I defer the startDecode to onTimeout() or I sleep.
* For digital, fast channel change involves defering everthing: tune/acquire/decode!
*   And we only want to defer when doing channel up/down.
**/
bool Control::setChannel(int ch, StartDecodeType decodeType, bool ignoreContinuous, int decodeOverride)
{
    bool result = false;
    bool isAnalog = false;
    int dec = currentDecode();
    ChannelMap::Channel currentChannel;

    /* see if we can do a video primer fast channel change */
    if (_live[dec] /* set when a normal decode is started via shim */
        && _live[dec]->getNumOpenedPrimers() && ch!=_channel[dec] /* handle it below */
        && (MAINCH(ch)==MAINCH(_channel[dec]))) {

            if (setChannelFast(ch)==true) {
                return true;
            }
            /* if it fails, keep going and try to do a normal channel change */
    }

    if (decodeOverride != -1)
        dec = decodeOverride;

    _incrementalChEntry.digits = 0;
    _deferSetChannel.tickCount = 0;

    if (!cfg()->getBool(Config::DEFERRED_CHANNELCHANGED_ENABLED)) {
        decodeType = eDecode_Immediate;
    }
    BDBG_MSG(("setChannel(%d,%s,%s,%d)", ch, decodeType==eDecode_Immediate?"immed":"defer",ignoreContinuous?"true":"false",decodeOverride));
    if (!_decode[dec])
        return false;

    // If we're already decoding the correct channel, we're out of here
    if (ch == _channel[dec] && _live[dec] &&
        !cfg()->getBool("FORCE_RETUNE"))
    {
        BDBG_MSG(("Already set to channel %d", ch));
        result = true;
        goto done;
    }

    // stop decode or playback before tuning
#ifdef PVR_SUPPORT
    if (isPlaying(dec))
        stopPlayback(dec);
    else
#endif
        stopLiveDecode(dec);

    if (cfg()->getBool("FORCE_RESCAN") && MAINCH(ch) != MAINCH(_channel[dec])) {
        /* always rescan on mainchannel change (but not subchannel change) */
        channelMap()->clearPrograms();
    }

    /* Here's where we change state. We have now changed channel. */
    _channel[dec] = ch;
    result = true;

    if (!_liveDecodeEnabled) goto done;

    isAnalog = !channelMap()->isDigital(ch);

    /* defer tune & decode for digital */
    if (decodeType != eDecode_Immediate) {
        BDBG_MSG(("Defer digital tune and decode %d", ch));
        _deferSetChannel.tickCount = MApplication::tickCount();
        _deferSetChannel.channel = ch;
        _deferSetChannel.digital = !isAnalog;
        _deferSetChannel.ignoreContinuous = ignoreContinuous;
        _deferSetChannel.decodeOverride = decodeOverride;
        fireEvent(evDeferredChangeChannel);
        goto done;
    }

    // tune
    BDBG_ASSERT(!_live[dec]);
    // allow getting a recording (but not decoding) stream

    /* create channel object for use by IP channel */
    if (!_channelMap->getChannel(channel(), &currentChannel) &&
        currentChannel.entry.getType() == btuner_ip_stream)
    {
        _ipVideo->decoderIndex = dec;
        _live[dec] = _avManager->tune(ch, true, _ipVideo, _playbackManager);
    }
    else
    {
        _live[dec] = _avManager->tune(ch, true, NULL, NULL);

        if (_live[dec] && cfg()->getBool(Config::VIDEO_PRIMER_FASTCHANNELCHANGE_ENABLED)) {
            _live[dec]->setEnablePrimers(true);
            if (currentChannel.entry.getType() == btuner_streamer_stream && _decode[dec]) {
                _live[dec]->initPrimers(_decode[dec],SUBCH(ch)); /* streamer is a special case */
            }
        }
    }

    if (!_live[dec]) {
        BDBG_WRN(("Unable to tune channel %d", ch));
        result = false;
        goto done;
    }

    tv()->setPlayEngines();

    _live[dec]->setCallback(avstreamCallback, this);

    if (isAnalog) {
        decodeType = eDecode_Immediate;
    }

    if (decodeType == eDecode_Immediate) {
        if (cfg()->getBool(Config::LIVEDECODE_ENABLED) && _decode[dec]) {
            /* Here's where live decode is started */
            _live[dec]->startDecode(_decode[dec]);
            fireEvent(evStartDecode);

            BDBG_MSG(("  livedecode[%d] started AVStream=%p", dec, _live[dec]));
#ifdef PVR_SUPPORT
            if (!ignoreContinuous && cfg()->getBool(Config::CONTINUOUS_RECORD_ENABLED))
                recordVideo(_continuous[dec], _channel[dec]);
#endif
        }
    }

done:
    /* This when the user sees it */
    fireEvent(evChangeChannel);
    showModeMessage();
    updateLeds();
    return result;
}

/* video primer fast channel change */
bool Control::setChannelFast(int ch)
{
    bool result = false;
    int dec = currentDecode();
    ChannelMap::Channel currentChannel;
    BDBG_WRN(("setChannelFast: %d.%d -> %d.%d", MAINCH(_channel[dec]), SUBCH(_channel[dec])+1, MAINCH(ch), SUBCH(ch)+1)); /* Brutus GUI labels subchannels starting at 1. internally, they start at 0. keep this warning message in sync with GUI */

    if (cfg()->getBool(Config::LIVEDECODE_ENABLED)) {
        /* instead of stopLiveDecode() and killing the AVStream, keep it alive and just modify it */
        if (_live[dec]->setChannelFromPrimer(_channel[dec], ch)) {
            return result;
        }
    }

    /* change state. we have now changed channel */
    _channel[dec] = ch;
    result = true;

    tv()->setPlayEngines();
    _live[dec]->setCallback(avstreamCallback, this);

    fireEvent(evStartDecode);

done:
    /* This when the user sees it */
    fireEvent(evChangeChannel);
    showModeMessage();
    updateLeds();
    return result;
}

bool Control::stopLiveDecode(int decode, bool ignoreContinuous) {
    if (decode == -1)
        decode = currentDecode();
    AVStream *str = _live[decode];

    BDBG_MSG(("stopLiveDecode(%d) AVStream %p", decode, str));
    if (!str) return false;

    /* stop the matching continuous record, unless told otherwise */
#ifdef PVR_SUPPORT
    if (!ignoreContinuous && cfg()->getBool(Config::CONTINUOUS_RECORD_ENABLED)) {
        stopRecord(_live[decode]);
    }
#endif
    /* This stream may not be decoding because we defer analog decode */
    str->stopDecode();
    if (!str->isRecording())
        _avManager->untune(str);
    _live[decode] = NULL;
    fireEvent(evStopDecode);
    return true;
}

bool Control::disableAudio(int source) {
    if (source == -1)
        source = currentDecode();
    BDBG_MSG(("disableAudio[%d]", source));
    if (_decode[source])
        return !_decode[source]->disableAudio();
    else
        return false;
}

bool Control::enableAudio(int source) {
#ifdef AUDIO_SUPPORT
    /* If MP3 is playing, don't enable */
    if (audioControl() && audioControl()->isPlaying())
        return false;
#endif

    if (source == -1)
        source = currentDecode();

    // disable the other (if decoding)
    disableAudio(1-source);

    BDBG_MSG(("enableAudio[%d]", source));
    if (_decode[source])
        return !_decode[source]->enableAudio();
    else
        return false;
}

/* This array must be kept in sync with Control::Screen enum */
const char *Control::_screenname[eTotalScreens] = {
    "tv", "menu", "playback", "record", "admin",
    "pictures", "mp3s", "video", "runscript", "editchannels",
    "dsg" , "hdmi", "hdmi_cec" ,"ipstb"
#ifdef B_HAS_DRM_ND
    ,"extserver"
#endif
    ,"chan_map"
#ifdef B_HAS_TRANSCODE
    ,"transcode"
#endif
};

bool Control::pipEnabled() const {return cfg()->getBool(Config::PIP_ENABLED)!=0;}
bool Control::secondDisplay() const {return cfg()->getBool(Config::SECOND_DISPLAY)!=0;}
bool Control::isAdmin() const {return cfg()->getBool(Config::ADMIN);}
bool Control::isPalMode() const {
    bvideo_format_settings s;
    bdisplay_get_video_format_settings(&_display_settings, &s);
    return s.refresh_rate == 50 || s.refresh_rate == 25;
}
bool Control::isPalFormat(bvideo_frame_rate frame_rate) const {
    bool b = false;
    switch (frame_rate) {
        case bvideo_frame_rate_25:
        case bvideo_frame_rate_50:
            b = true;
            break;
        case bvideo_frame_rate_unknown:
        case bvideo_frame_rate_23_976:
        case bvideo_frame_rate_24:
        case bvideo_frame_rate_29_97:
        case bvideo_frame_rate_30:
        case bvideo_frame_rate_59_94:
        case bvideo_frame_rate_60:
        default:
            b = false;
            break;
    }
    return b;
}

bool Control::getOutput(bdisplay_settings *settings) const
{
    *settings = _display_settings;
    return true;
}

/**
* bdisplay_set() can only be called from Control::setOutput because
* the _display_settings cache must be maintained, the graphics must
* be scaled appropriately, and decode may need to be restarted.
**/
bool Control::setOutput(const bdisplay_settings *settings)
{
    bresult rc = b_ok;
    bvideo_format_settings video_format_settings;

    /* get format settings from enum or vesa_settings */
    bdisplay_get_video_format_settings(settings, &video_format_settings);

    // first, cache values. I remember them BEFORE
    // setting, because displaymode_set may fail, yet we don't want these values
    // to be uninitialized (for example, HD source on 711x will cause ConfigDisplaySetOutput
    // to fail.)
    _display_settings = *settings;

    _display_settings.background_color = cfg()->getInt(Config::NO_SIGNAL_BACKGROUND_COLOR);

    if (_display_settings.format == bvideo_format_vesa) {
        /* NOTE: VESA will cause IS_HD() to return true, so test it first */
        if (secondDisplay()) {
            BDBG_ERR(("VESA output only valid for first display"));
            return false;
        }

        _display_settings.component = NULL;
        _display_settings.svideo = NULL;
        _display_settings.composite = NULL;
        _display_settings.rf = NULL;
    }
    else if (IS_HD(video_format_settings)) {
        if (secondDisplay()) {
            BDBG_ERR(("HD output only valid for first display"));
            return false;
        }

        _display_settings.component = boutput_component_open(B_ID(0));
        _display_settings.svideo = NULL;
        _display_settings.composite = NULL;
        _display_settings.rf = NULL;
    }
    else if (!secondDisplay()) {
        _display_settings.component = boutput_component_open(B_ID(0));
        if (!cfg()->getBool(Config::DUAL_OUTPUT) &&
            !cfg()->getBool(Config::HDSD_SINGLE))
        {
            _display_settings.composite = boutput_composite_open(B_ID(0));
            _display_settings.svideo = boutput_svideo_open(B_ID(0));
            _display_settings.rf = boutput_rf_open(B_ID(0));
        }
    }

    bdisplay_settings current_display_settings;
    bvideo_format_settings current_video_format_settings;
    bdecode_status current_decode_status;
    bool setSD_path_first = false;

    bdisplay_get(display()->display, &current_display_settings);
    bdisplay_get_video_format_settings(&current_display_settings, &current_video_format_settings); /* get current display settings */
    if (getDecode()) {
        bdecode_get_status(getDecode(), &current_decode_status); /* get source settings */
    }
    else {
        BKNI_Memset(&current_decode_status, 0, sizeof(current_decode_status));
    }

    /* With HD source formats, we must be careful which output (HD or SD) to set first when switching between NTSC and
    PAL output formats, or VDC will run out of memory during frame capture.
    The general rule to follow is, if the HD path's current display frame rate goes from matching the source frame rate to not
    matching it, set the SD path first. In all other cases, set the HD path first. See PR37429. */

    if ( (isPalFormat(current_video_format_settings.frame_rate) == isPalFormat(current_decode_status.video_frame_rate)) &&  /* if the HD path's current display settings match the source frame rate ...*/
         (isPalFormat(current_video_format_settings.frame_rate) != isPalFormat(video_format_settings.frame_rate)) ) {       /* AND the new settings do not match the current frame rate */
            setSD_path_first = true;
    }

    if (!setSD_path_first) {
        rc = bdisplay_set(display()->display, &_display_settings);
        if (rc) {
            BDBG_ERR(("Unable to set HD path display settings: %d", rc));
            return false;
        }
    }
    if (display()->display2) { /* HDSD_SINGLE */
        /* Set display1 to SD, either PAL or NTSC, to match display0 */
        bdisplay_get(display()->display2, &_display_settings2);

        if (cfg()->get(Config::SD_OUTPUT_FORMAT)) {
            OutputsInfoBarPlugin::setFormat(cfg()->get(Config::SD_OUTPUT_FORMAT), &_display_settings2);
        }
        else {
            /* if display0 is "SECAM" then what's really happening is that display0 is
            576i and display1 is SECAM. SECAM only applies to how chroma is modulated in
            a composite signal. */
            if (_display_settings.format == bvideo_format_secam)
                _display_settings2.format = bvideo_format_secam;
            else if (isPalMode() || _display_settings.format==bvideo_format_pal_m) {
                if ( IS_HD(video_format_settings) )
                    _display_settings2.format = bvideo_format_pal;
                else
                    _display_settings2.format = _display_settings.format;
            }
            else if (_display_settings.format == bvideo_format_ntsc_japan)
                _display_settings2.format = bvideo_format_ntsc_japan;
            else
                _display_settings2.format = bvideo_format_ntsc;
        }

        if (cfg()->get(Config::ASPECT_RATIO_SD)) {
            OutputsInfoBarPlugin::setAspectRatio(cfg()->get(Config::ASPECT_RATIO_SD), false, &_display_settings2);
        }

#ifdef DCS_SUPPORT
        _display_settings2.dcs_type = _display_settings.dcs_type;
#endif
        _display_settings2.background_color = _display_settings.background_color;

        bdisplay_set(display()->display2, &_display_settings2);
    }

    if (setSD_path_first) {
        rc = bdisplay_set(display()->display, &_display_settings);
        if (rc) {
            BDBG_ERR(("Unable to set display settings: %d", rc));
            return false;
        }
    }

    /* On some platforms, we have to reset the graphics in order to scale
    properly. */
    _brutus->resetGraphics();

#ifdef BRUTUS_LIBTUNE_SUPPORT
    Libtune_SetDisplayFormat();
    Libtune_TuneWindows();
#endif

    // tv() might be NULL because this is called during Control's constructor
    if (tv() && pipEnabled())
        tv()->setPipGeometry();
    setWindowSizes();
    hdmiSetVsi();

    fireEvent(evSetOutput);

    return true;
}

void Control::addControlEventHandler(ControlEventHandler *handler)
{
    _handlers.add(handler);
}

void Control::removeControlEventHandler(ControlEventHandler *handler)
{
    _handlers.remove(handler);
}

void Control::fireEvent(Event event)
{
    for (ControlEventHandler *h = _handlers.first();h;h=_handlers.next())
        h->onControlEvent(this, event);
    switch (event) {
        case Control::evStartDecode: this->_started_decode = true;
    }
}

void Control::setRepeatKeyFunc(RepeatKeyFunc func, void *context)
{
    _repeatKeyFunc = func;
    _repeatKeyContext = context;
}

bool Control::testRepeatKey(int cmd) const
{
    return _repeatKeyFunc && _repeatKeyFunc(_repeatKeyContext, cmd);
}

int Control::scan() {
    int rc;
    int ch1 = MAINCH(channel(0));
    int ch2 = MAINCH(channel(1));
    int ds1 = !secondDisplay() && !isPlaying(0);
    int ds2 = (secondDisplay() || pipEnabled()) && !isPlaying(1) && _pipVisible;

    if (ds1)
        stopLiveDecode(0);
    if (ds2)
        stopLiveDecode(1);

    rc = !_channelMap->clearPrograms();
    if (!rc)
        MMessageBox::info(fb(), "Channel listings have been cleared. New scan will occur on each channel change.");

#ifdef B_HAS_IP
    rc  = channelManager()->psiScan(cfg()->get(Config::CHANNEL_MAP), cfg()->get(Config::IPSERVER_IPV4));
    chanmapScreenRefresh();
#endif

    if (rc) {
        MMessageBox::info(fb(), "Channel scan failed");
        return rc;
    }

    if (ds1) {
        _currentDecode = 0;
        setChannel(ch1);
    }
    if (ds2) {
        _currentDecode = 1;
        setChannel(ch2);
    }

    return rc;
}

bdecode_t Control::getDecode(int index) const
{
    DecodeEngine *d = decode(index);
    if (!d) return NULL;

    DecodeEngine::Settings s;
    d->getSettings(&s);
    return s.decode;
}

/* NOTE: this is a duplicate of Control::window() */
bdecode_window_t Control::getWindow(int index) const
{
    DecodeEngine *d = decode(index);
    if (!d) return NULL;

    DecodeEngine::Settings s;
    d->getSettings(&s);
    return s.window;
}

void Control::setWindowOverride(int window, int x, int y, unsigned width, unsigned height)
{
    _windowOverride[window] = MRect(x,y,width,height);
    showWindow(window);
}

void Control::standby(brutus_standby_state state)
{
#ifdef POWERSTANDBY_SUPPORT
    brutus_standby_state prev_state = _standbyState;

    if (state != brutus_standby_state_eOn) {
    /* HDMI CEC Support - broadcast HDMI CEC standby message */
    hdmiCecStandbyMessage(this);
        stopAllAudioVideo();
    _standbyState = state;
    fb()->app()->standby();
    }

    switch (state) {
    case brutus_standby_state_eDeepSleep:
    channelManager()->depopulateFrontEnd();
    bsettop_uninit_frontend();
        brutus_deep_sleep(_cfg->getInt(Config::POWERSTANDBY_TIMEOUT));
    bsettop_init_frontend();
    channelManager()->populateFrontEnd();
        break;
    case brutus_standby_state_ePassiveStandby:
        brutus_passive_standby(_cfg->getInt(Config::POWERSTANDBY_TIMEOUT));
        break;
    case brutus_standby_state_eActiveStandby:
        brutus_active_standby(_cfg->getInt(Config::POWERSTANDBY_TIMEOUT));
        break;
    case brutus_standby_state_eOn:
        if (prev_state == brutus_standby_state_eActiveStandby) {
            brutus_resume();
        }
        break;
    }

    if (state == brutus_standby_state_eOn) {
    fb()->app()->resume();
    _standbyState = state;
    startLiveDecode();
    /* HDMI CEC Support - broadcast HDMI CEC standby message */
    hdmiCecStandbyMessage(this);
    }
#else
    BSTD_UNUSED(state);
#endif
}

#if defined(_WIN32_WCE)
#define TERMINATE(HALT) ExitProcess(0)
#elif defined __vxworks
#define TERMINATE(HALT) do {fclose(stdin); fb()->app()->terminate(); } while (0)
#else
#define TERMINATE(HALT) \
    do { \
        if (HALT) { \
            if (system("/sbin/halt")) \
                MMessageBox::info(fb(), "Unable to halt"); \
        } \
        else { \
            fb()->app()->terminate(); \
        }     \
    } while (0)
#endif

void Control::shutdown(brutus_shutdown_state state)
{
    /* HDMI CEC Support - broadcast HDMI CEC standby message */
    hdmiCecStandbyMessage(this);

    switch (state) {
    case brutus_shutdown_state_eQuit:
        if (MMessageBox::show(fb(),
            "This will shut down the application. Are you sure?",
            MMessageBox::bYes|MMessageBox::bNo,
            MMessageBox::bNo) == MMessageBox::bYes)
        {
            TERMINATE(false);
        }
        break;
    case brutus_shutdown_state_eHalt:
        if (MMessageBox::show(fb(),
            "This will shut down the box and cleanly dismount your hard disks. Do you want to halt?",
            MMessageBox::bYes|MMessageBox::bNo,
            MMessageBox::bNo) == MMessageBox::bYes)
        {
            TERMINATE(true);
        }
        break;
    }
}

