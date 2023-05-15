/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: control.h $
 * $brcm_Revision: 98 $
 * $brcm_Date: 3/21/12 1:53p $
 *
 * Module Description: App logic for pvrgui app
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/control.h $
 * 
 * 98   3/21/12 1:53p erickson
 * SW7425-2664: remove old code
 * 
 * 97   3/21/12 12:01p erickson
 * SW7425-2664: remove old code
 * 
 * 96   3/19/12 6:44p erickson
 * SW7425-2664: purge old code
 * 
 * 95   1/18/12 4:11p jtna
 * SW7425-2213: add deep sleep standby to brutus
 * 
 * 94   4/6/11 2:30p gmohile
 * SW7420-1792 : Add power standby support
 * 
 * 93   3/24/11 2:50p hongtaoz
 * SW7425-199: emulate customer AUTO_FORMAT behavior by doing check once
 * per StartDecode event;
 * 
 * 92   11/16/10 12:23p jtna
 * SW7405-4998: more 3D refactoring
 * 
 * 91   10/6/10 2:19p calvinho
 * SW7401-4460: Add function prototypes hdmiVideoFormatHas3DSupport and
 * hdmiTVHas3DSupport
 * 
 * 90   6/25/10 6:15p jtna
 * SW7405-4420: handle 3dtv source/output permutations better
 * 
 * 89   6/14/10 12:29p jtna
 * SW7401-4412: add missing include for 7401
 * 
 * 88   5/12/10 5:52p jtna
 * SW7405-3999: added 3DTV auto mode
 * 
 * 87   4/22/10 4:03p erickson
 * SW7125-325: fix gcc-4.4.3 warning
 * 
 * 86   4/22/10 10:57a calvinho
 * SW7420-466: Added member variables for applying DNR settings after
 * decode has started
 * 
 * 85   4/5/10 11:30a erickson
 * SW7405-3997: set HDMI VSI for 3DTV
 *
 * 84   3/30/10 7:47p jtna
 * SW7420-527: add support uiControl
 *
 * 83   3/2/10 4:00p jtna
 * SW7420-527: avoid tuning before decode is active
 *
 * 82   3/1/10 6:52p jtna
 * SW7420-527: initial Brutus libtune integration
 *
 * 81   1/21/10 5:41p mward
 * SW7400-2517: merge to main support for BUILD_SYSTEM=magnum.
 *
 * SW7400-2517/2   12/16/09 11:25a mward
 * SW7400-2517: merge latest from main.
 *
 * 80   12/11/09 6:12p mphillip
 * SW7550-112: Merge 7550 changes to main branch
 *
 * Refsw_7550/1   11/18/09 4:23p chengli
 * SW7550-64 : enable channel map screen even B_HAS_IP is not defined
 *
 * SW7400-2517/1   9/14/09 3:10p mward
 * SW7400-2517: Maintain Native Settop API (BUILD_SYSTEM=magnum) build for
 * 97400.
 *
 * 79   8/25/09 12:49p jtna
 * SWDEPRECATED-3910: video primer fast channel change feature
 *
 * 78   8/14/09 3:38p erickson
 * PR55994: refactor thumbnail
 *
 * 77   7/9/09 11:53a jtna
 * PR56522: Remove multi-decode fast channel change feature. To be
 * replaced with video primer FCC.
 *
 * 76   7/8/09 8:45a erickson
 * PR56373: use counter so that playback list is not read until after live
 * decode has started or a timeout occurs
 *
 * 75   7/2/09 5:47p erickson
 * PR56373: move standby logic to Control where it can be called from
 * multiple spots in the UI
 *
 * 74   7/2/09 5:09p erickson
 * PR56373: rework brutus_power to allow for deferred usb & sata power,
 * deferred mount, deferred videos scan
 *
 * 73   6/12/09 11:11a jtna
 * PR43001: support TS/TTS record
 *
 * 72   6/1/09 7:55p jtna
 * PR43001: added record-related brutus script commands
 *
 * 71   5/29/09 2:40p jtna
 * PR43001: refactor brutus record logic
 *
 * 70   5/11/09 7:40p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 *
 * 69   5/11/09 5:12p erickson
 * PR54915: remove unused and misleading code
 *
 * PR53373_DeepColor/1   4/16/09 3:57p vle
 * PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
 *
 * 68   2/23/09 6:13p vle
 * PR 48785: Add HDMI CEC screen to BRUTUS
 * Merge to main branch
 *
 * HDMI_TX_Plugfest11_200811/1   11/7/08 4:55p vle
 * PR48785: Add HDMI CEC screen to BRUTUS demo app
 *
 * 67   8/20/08 5:48p lwhite
 * PR42739: replaced B_HAS_NETACCEL with B_HAS_IP
 *
 * 66   7/30/08 7:59p shyam
 * PR43426 : Add dynamic toggling between transcode and decode
 *
 * 65   7/30/08 12:36p shyam
 * PR 45076 : Add trasncode support for playback streams
 *
 * 64   7/10/08 11:38p ssood
 * PR42739: Cleaned up logic on accessing playengines for IP playback
 *
 * 63   7/2/08 4:37p erickson
 * PR17425: added main_window and pip_window commands
 *
 * 62   6/25/08 12:54p shyam
 * PR43426 : Add 7043 Transcode support
 *
 * 61   5/29/08 4:09p jtna
 * PR42200: Service pretune request in onTimeout()
 *
 * 60   5/27/08 4:43p jtna
 * PR42200: Multi-decode fast channel change feature
 *
 * 59   5/20/08 10:56p vle
 * PR 40497: Add HDMI CEC support to Brutus.
 *
 * 58   5/19/08 12:09p erickson
 * PR42752: fix reentrancy bug in audio_source_changed callback. just set
 * a boolean and service it in onTimeout.
 *
 * 57   4/16/08 6:25p jtna
 * PR37429: Handle NTSC<->PAL switch better
 *
 * 56   4/2/08 3:55p mward
 * PR40307: Don't include any thumbnail support with PVR_SUPPORT=n.
 *
 * 55   3/20/08 3:18a erickson
 * PR40307: reduce busy loop
 *
 * 54   3/20/08 1:49a erickson
 * PR40307: update
 *
 * 53   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 *
 * 52   3/18/08 4:05p erickson
 * PR40307: added thumbnail feature
 *
 * 51   3/17/08 3:17p erickson
 * PR40307: remove old, unused code before adding new
 *
 * 50   3/14/08 3:53p vle
 * PR 37781: Merge to main branch
 *
 * PR37781/3   3/12/08 8:30p vle
 * PR 37781: update to avoid re-entrance of Brutus callback and Settop API
 *
 * PR37781/2   3/12/08 3:28p vle
 * PR 37781: Update code to use new API name
 *
 * PR37781/1   3/11/08 6:47p vle
 * PR37781, PR40497: Add initial HDMI CEC support in Brutus
 * Add support for <ReportPhysicalAddress> and <SetOSDName>.
 *
 * 49   2/19/08 11:17a yrajakar
 * PR36762: Update status with first_pts, and duration
 *
 * 48   12/6/07 12:52p erickson
 * PR37967: rename functions for consistency
 *
 * 47   9/27/07 3:58p mward
 * PR34614: Fix PVR_SUPPORT=n
 *
 * 46   9/17/07 12:32p jrubio
 * PR34613: added ability to refresh ChanmapScreen NETACCEL
 *
 * 45   9/11/07 4:59p jrubio
 * PR34614: adding PVR timeline to HTTP channels
 *
 * 44   9/10/07 4:15p ahulse
 * PR28979: Have control poll recordengine for record error condition
 *
 * 43   8/10/07 1:55p ssood
 * PR33786: Net DMA & IP Streamer refactoring: replace STREAMER_SUPPORT
 * variable by NETACCEL_SUPPORT
 *
 * 42   8/7/07 12:11p jgarrett
 * PR 31335: Adding isValidHdmiFormat()
 *
 * 41   8/1/07 7:59p vle
 * PR 33721: Add support for vesa formats to getOptimalDisplay
 *
 * 40   7/12/07 7:34p vle
 * PR 26747: Report HDCP authentication failures using on screen graphics.
 *
 * 39   7/12/07 4:26p rjlewis
 * PR32982: functions only apply when PVR_SUPPORT enabled.
 *
 * 38   6/22/07 1:51p erickson
 * PR30701: reimplemented AVStream's record on lock feature. we must
 * support AVStream's which don't have bstream_t yet.
 *
 * 37   6/8/07 12:16p erickson
 * PR31914: added setOptimalDisplay which works with
 * bdecode_settings.video.source_changed and HDMI preferences and
 * capabilities. This replaces AUTO_HDMI_MODE.
 *
 * 36   4/24/07 12:52p jrubio
 * PR30010: added channel server screen for ip server
 *
 * 35   3/14/07 10:15a erickson
 * PR26267: refactor Brutus into separate VideoList and PlaybackScreen
 *
 * 34   3/9/07 2:12p piyushg
 * PR28588: Add DRM-NDR support in brutus
 *
 * 33   1/31/07 11:47a erickson
 * PR19687: stop all decode and playback on standby
 *
 * 32   1/26/07 11:12a erickson
 * PR27149: info panel displays frontend status for timeshifting playback.
 * Control extended to lookup AVStream.
 *
 * 31   12/7/06 10:48a jgarrett
 * PR 26293: Dividing playback resources in DUAL_OUTPUT mode
 *
 * 30   11/30/06 6:32p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 29   11/2/06 12:00p ssood
 * PR24977: Merging IP STB 2.5 related changes back to the main branch
 *
 * PROD_IPSTB_REFSW_Devel/3   10/12/06 7:35p arbisman
 * PR23775: Use playback manager for IP channels
 *
 * PROD_IPSTB_REFSW_Devel/2   10/11/06 6:43p arbisman
 * PR23775: Merge from main
 *
 ***************************************************************************/
#ifndef CONTROL_H
#define CONTROL_H

#include "bsettop.h"
#include "brutus_app.h"
#include <mstringlist.h>
#include <mwidget.h>
#include <mtimer.h>
#include <assert.h>

#include "video.h"
#include "videolist.h"
#ifdef PVR_SUPPORT
#include "playengine.h"
#include "recordengine.h"
#else
class RecordManager;
#endif
#include "brutus_os.h"
#include "avmanager.h"
#include "decodeengine.h"
#include "channelmgr.h"
#include "channelmap.h"
#include "skin.h"
#if defined (PVR_SUPPORT)
#include "bthumbnail.h"
#ifdef NEXUS_BUILD
#include "bthumbnail_extractor.h"
#include "nexus_video_decoder_types.h"
#endif
#endif

#if defined (BRUTUS_LIBTUNE_SUPPORT) && defined (NEXUS_BUILD)
#include "libtune.h"
#include "nexus_video_window.h"
#include "nexus_display.h"
#endif

// The following are constant, based on hardcoded screen layout
#define LARGETV MRect(265-38,95,340,340*3/4)
#define SMALLTV MRect(445-38,20,210,210*3/4)
#define NOTV MRect(0,0,1,1)

#define IS_HD(format_settings) \
    (format_settings.width > 720 || !format_settings.interlaced)

class GenericScreen;
class InputEngine;
class ScriptEngine;
class ChannelManager;
class Config;
class LedEngine;
class AudioControl;
class ProgramGuide;
class ControlEventHandler;
class Skin;

typedef enum brutus_shutdown_state {  
    brutus_shutdown_state_eQuit,
    brutus_shutdown_state_eHalt
} brutus_shutdown_state;

typedef enum brutus_standby_state {
    brutus_standby_state_eOn,
    brutus_standby_state_ePassiveStandby,
    brutus_standby_state_eActiveStandby,
    brutus_standby_state_eDeepSleep
} brutus_standby_state;

/**
* Central administration for Brutus
**/
class Control : MTimerEventHandler
#ifdef PVR_SUPPORT
    , PlayEngineEventHandler, RecordEngineEventHandler
#endif
    {
public:
    Control(MFrameBuffer *fb, Brutus *brutus, ChannelManager *mgr,
        RecordManager *recordManager, AVManager *avManager, Config *cfg);
    virtual ~Control();

    /**
    * Did the constructor initialize fully?
    */
    bool valid() const {return _valid;}
    MFrameBuffer *fb() const {return _fb;}

    /**
    * Screen control. NOTE: keep _screenname up to date.
    **/
    enum Screen {
        eTV, eHome, ePlayback, eRecord,
        eAdmin, ePictures, eAudio,
        eVideo, eRunScript, eEditChannels,
        eDsg, eHdmi, eHdmiCec,
        eIpStbConfig,
#ifdef B_HAS_DRM_ND
        eExtserver,
#endif
        eChanMap,
#ifdef B_HAS_TRANSCODE
        eTranscode,
#endif
        eTotalScreens
    };
    void showScreen(Screen screen);
    Screen currentScreen() const {return _currentScreen;}
    GenericScreen *screen(Screen scr) const {return _screens[scr];}
    static const char *screenName(Screen scr) {return _screenname[scr];}

    /* Refresh function for Chanmap Screen */
    void chanmapScreenRefresh();

    /**
    Set a widget to allow repeated keys to get through InputEngine
    **/
    typedef bool (*RepeatKeyFunc)(void *context, int cmd);
    void setRepeatKeyFunc(RepeatKeyFunc func, void *context);
    bool testRepeatKey(int cmd) const;

    /* This is called from the bwin sync callback after the bgraphics_flip
    call has been made. */
    void flipDone();

    typedef enum {
        evChangeChannel,    /* sent after tuning but before decoding */
        evDeferredChangeChannel, /* sent when the change channel is requested but before it is completed */
        evMoveWindowControl, /* sent after changing control between main/pip windows */
        evStopDecode,   /* sent after decode has stopped (live or pvr) */
        evStartDecode,  /* sent after decode has started (live or pvr) */
        evSetOutput     /* sent after display format has been changed */

    } Event;
    void addControlEventHandler(ControlEventHandler *handler);
    void removeControlEventHandler(ControlEventHandler *handler);

    VideoList *videos() {return _videos;}
#ifdef PVR_SUPPORT
    bool playVideo(Video *vid, bool resume = false, bool catchup = false, bool ignoreContinuous = false /* should be true only in internal calls */);
    void stopPlayback(int source = -1);
    void stopPlayback(Video *video, bool startLiveDecode = true, bool ignoreContinuous = false /* should be true only in internal calls */);
    bool isPlaying(int source = -1) const;
    PlayEngine *playEngine(int index = -1);

#if B_HAS_TRANSCODE
    bool transcodePlaybackVideo(Video *vid, bool ignoreContinuous=false);
#endif

    bool recordVideo(Video *vid, int ch);
    bool recordVideoPrepare(Video* vid);

    int totalRecords();
    void stopRecord(
        int index /* index into the _records structure. It has nothing to do with the decode index. */
        );
    void stopRecord(AVStream *avstream);
    void stopRecord(Video *video);
    bool stopRecordCh(int channel);
    int stopRecordLast();
    int stopRecordAll();
    bool deleteStream(int index);
    RecordEngine *recordEngine(Video *video);

    /* Stop all playbacks and recordings. */
    void stopAllPvr();

    /* For certain PVR-related commands, the live/playback transition should be handled here */
    void stopCommand();
    void pauseCommand();
    void trickCommand(int fast, int dir);
#else
    bool isPlaying(int source = -1) const {return false;}
    int totalRecords() const {return 0;}
#endif

    Brutus *brutus() {return _brutus;}
    const BrutusDisplay *display() const {return _brutus->display();}

    bool setOutput(const bdisplay_settings *settings);
    bool getOutput(bdisplay_settings *settings) const;
    DecodeEngine *decode(int index = -1) const {return _decode[index==-1?currentDecode():index];}
    bool isPalMode() const;
    bool isPalFormat(bvideo_frame_rate frame_rate) const;

    /**
    * Channel Manager and program guide
    **/
    ChannelManager *channelManager() {return _chMgr;}
    ChannelMap *channelMap() {return _channelMap;}
    ProgramGuide *guide() const {return _guide;}
    int scan();

    typedef enum {
        eDecode_Deferred, /* apply the change to the UI immediately, but
            wait for UI to update before channel changing */
        eDecode_Immediate /* No deferral */
    } StartDecodeType;
    bool setChannel(int ch, StartDecodeType decodeType = eDecode_Deferred, bool ignoreContinuous = false /* should be true only in internal calls */, int decodeOverride = -1 /* should only be a value on internal calls */);
    bool setChannelFast(int ch);
    bool deferredSetChannelPending() const;

    void setIncrementalChannel();
    void getIncrementalChannel(int *ch, int *digits);

    /**
    Stop a live decode which was stared by setChannel
    **/
    bool stopLiveDecode(int source = -1 /* current */, bool ignoreContinuous = false /* should be true only in internal calls */);
    void setLiveDecodeEnabled(bool enabled);

    /* stop all PVR, live decode and PCM playback.
    used for shutdown and standby. */
    void stopAllAudioVideo();

    /* start up live decode, regardless of previous state.
    used for coming out of standby. */
    void startLiveDecode();

    void changeChannel(int inc);
    void addChannelDigit(int digit);

    /* Returns current channel, taking into account any current deferral */
    int channel(int index = -1) const;
    btuner_t tuner(int index = -1) const;
    bdecode_window_t window(int index = -1) const;

#ifdef AUDIO_SUPPORT
    AudioControl *audioControl() const {return _audioControl;}
#endif

    /**
    * Enable/disable mpeg audio decode so that mp3 can be heard by itself,
    * or when toggling between main and pip
    **/
    bool enableAudio(int source = -1);
    bool disableAudio(int source = -1);

    /**
    * 0 == main window, 1 == pip window
    */
    int currentWindow() const {return _windowSource[_currentDecode];}
    /**
    * 0 == 1st decode, 1 == 2nd decode.
    */
    int currentDecode() const {return _currentDecode;}
    AVStream* getLiveStream() const { return _live[_currentDecode];}

#ifdef PVR_SUPPORT
    /* find the live stream that is being timeshifted. if null, this playback is not being timeshifting. */
    AVStream* getLiveStream(PlayEngine *playEngine) const;
#endif

    /**
    * Makes pip visible or invisible
    **/
    void togglePip();
    void hidePip();

    /**
    * If pip is visible, toggle the video between main and pip
    **/
    void swapWindows();
    bool areWindowsSwapped() const;

    /**
    * If pip is visible, toggle the control and audio between
    * main and pip.
    **/
    void moveWindowControl();

    /**
    * Returns true if you can display PIP
    **/
    bool pipEnabled() const;
    bool pipVisible() const {return _pipVisible;}

    void setWindowOverride(int window, int x, int y, unsigned width, unsigned height);

    /**
    * Returns true if this is the 2nd app in DUAL_OUTPUT configuration
    **/
    bool secondDisplay() const;

    /**
    The value of "window" is not affected by swap.
    0 always means Main (fullscreen, zorder=0)
    1 always means PIP (downscaled, zorder=1)
    */
    void showWindow(int window);
    void hideWindow(int window);

    void showModeMessage();
    void showVolumeMessage();

    Skin * getSkin() {return _skin;}

    Config *cfg() {return _cfg;}
    const Config *cfg() const {return _cfg;}

    /**
    Admin privilege allows demos to be a little safer by preventing curious users
    from doing things they shouldn't.
    **/
    bool verifyAdminPassword();
    bool isAdmin() const;

    /**
    * Update leds based on current application status.
    **/
    void updateLeds();

    //events
    void onTimeout(MTimer *self);
#ifdef PVR_SUPPORT
    void onStateChanged(PlayEngine *self);
#if B_HAS_DRM
    void onDRMLicenseStateChanged(PlayEngine *self);
    void onDRMHTTPStateChanged(PlayEngine *self);
#endif /* B_HAS_DRM */
    void onStateChanged(RecordEngine *self);
    void checkForRecordError(RecordEngine *self);
#endif

    void setWindowSizes(int display = -1);

    // HDMI Status events -- return true if HDMI is connected.
    bool hdmiConnected() const {return _hdmiConnected;};
    bool hdmiGetCapabilities(boutput_dvi_capabilities *pCaps);
    bresult hdmiHdcpUpdateStatus();
    void hdmiHdcpStart();
    bresult hdmiCecHandler();
    bresult hdmiSetVsi();
    bool hdmiVideoFormatHas3DSupport(NEXUS_VideoFormat vFmt);
    bool hdmiTVHas3DSupport();

    /***
        CEC message handler structure consist of an opcode and the handler
        function for that specific opcode.
       ***/
    struct HdmiCecHandlerType
    {
        uint8_t opcode;
        bsettop_callback cecMessageHandlerCallback;
    };

    /* function prototype for HDMI CEC message handler functions */
    static void hdmiCecPowerUp(void *context);
    static void hdmiCecPowerDown(void *context);

    static void hdmiCecImageViewOnMessage(void *context);
    static void hdmiCecTextViewOnMessage(void *context);
    static void hdmiCecActiveSourceMessage(void *context);
    static void hdmiCecInactiveSourceMessage(void *context);
    static void hdmiCecSetStreamPath(void *context);
    static void hdmiCecOneTouchPlay(void *context);

    static void hdmiCecStandbyMessage(void *context);

    static void hdmiCecVersionMessage(void *context);
    static void hdmiCecGetVersionMessage(void *context);
    static void hdmiCecGivePhysicalAddressMessage(void *context);
    static void hdmiCecReportPhysicalAddressMessage(void *context);
    static void hdmiCecGetMenuLanguageMessage(void *context);
    static void hdmiCecSetMenuLanguageMessage(void *context);

    static void hdmiCecSetOSDNameMessage(void *context);
    static void hdmiCecSetOSDStringMessage(void *context);

    static void hdmiCecGiveDevicePowerStatusMessage(void *context);
    static void hdmiCecReportPowerStatusMessage(void *context);

    static void hdmiCecReportAudioStatusMessage(void *context);
    static void hdmiCecDeviceVendorIDMessage(void *context);
    static void hdmiCecFeatureAbortMessage(void *context);


    /* public APIs to handle specific CEC related request */
    boutput_hdmi_cec_message_data *hdmiCecMessageData() {return &_hdmiCecMessageData;}
    boutput_hdmi_cec_configuration *hdmiCecConfiguration() {return &_hdmiCecConfiguration;}

    /* select the optimal display format based on decoding source and HDMI capabilities */
    void getOptimalDisplay(bdisplay_settings *display_settings);

    /* set the optimal display */
    void setOptimalDisplay();

    bdecode_t getDecode(int index = -1) const;
    bdecode_window_t getWindow(int index = -1) const;

#ifdef PVR_SUPPORT
    /* check out a new PlayEngine */
    PlayEngine *checkoutPlayEngine();
    /* check in a PlayEngine which is currently checked out */
    void checkinPlayEngine(PlayEngine *engine);
#endif

    /* returns true if it's an HTTP channel */
    bool isHttpChannel(int ch);
    /* return true if it's a RTSP channel */
    bool isRtspChannel(int ch);
    /* returns true if its a Live IP Channel (non-HTTP IP channel) */
    bool isLiveIpChannel(int ch);

#if defined (PVR_SUPPORT)
    /* PVR thumbnail manager */
    bthumbnail_manager_t thumbnail_manager() {return _thumbnail.manager;}
#endif

#ifdef B_HAS_TRANSCODE
    void transcodeChannel(bool transcodeEnable);
#endif

    void check3dtvSetting(bool force_warning);
    void shutdown(brutus_shutdown_state state);
    void standby(brutus_standby_state state);
    brutus_standby_state getStandbyState() {return _standbyState;}

private:
    MFrameBuffer *_fb;
    bool _valid;

    uint8_t _fullScreenAlpha; /* alpha value when on fullscreen TV */

    Brutus *_brutus;
    bdisplay_settings _display_settings;
    bdisplay_settings _display_settings2;
    int _totalDecodes;
    int _totalPlaybacks;
    int _channel[2];
    DecodeEngine *_decode[2];
    AVStream *_live[2]; /* If set, the live stream is currently being decoded */
    VideoList *_videos;
#ifdef PVR_SUPPORT
    Video *_continuous[2]; /* continuous record *Video streams */
#endif
    struct RecordingStream {
        AVStream *stream;
        RecordEngine *record;
        RecordingStream(AVStream *s,RecordEngine *r) {stream=s;record=r;}
    };
    MAutoList<RecordingStream> _records;
    static void avstreamCallback(void *context);
    bool _audioSourceChanged;

#if defined (PVR_SUPPORT)
    /* thumbnail feature */
    bdecode_t _still_decode;
    struct {
        bthumbnail_manager_t manager;
        bthumbnail_t current;
#ifdef NEXUS_BUILD
        bthumbnail_extractor_t extractor; /* single extractor will be reused for each stream */
#endif
        bool ready;
        unsigned startTime; /* time last still decode started. this allows us to catch decoder hangs */
        unsigned emptyGetTime; /* time last still decode request was made which has not still requests */
    } _thumbnail;
    int decode_still();
    void process_still_decode();
    static void decoded_still_ready(void *context);
#endif
#ifdef PVR_SUPPORT
	int _initialDiskReadCounter;
#endif
#define MAX_WINDOWS 4
    bdecode_window_t _window[MAX_WINDOWS];
    MRect _windowRect[MAX_WINDOWS]; // size and position of a window, applied in showDisplay()
    MRect _windowOverride[MAX_WINDOWS]; // size and position of a window, applied in showDisplay()
    int _windowSource[2];   // which source is assigned to a window, only keep track of
        // the two main windows, not clones

    // screens
    GenericScreen *_screens[eTotalScreens];
    static const char *_screenname[eTotalScreens];
    Screen _currentScreen;
    bool _inFade;
    bool _liveDecodeEnabled;
    RepeatKeyFunc _repeatKeyFunc;
    void *_repeatKeyContext;
    Skin * _skin;

    // auxiliary engines
    RecordManager *_recordManager;
    AVManager *_avManager;
    DecodeManager *_decodeManager;
    PlaybackManager *_playbackManager;
    ChannelManager *_chMgr;
    ChannelMap *_channelMap; // copy of ptr from channelManager
    ProgramGuide *_guide;
    Config *_cfg;
    LedEngine *_leds;

    /* the IP channel video object */
    Video * _ipVideo;
    PlayEngine *_ipRecordPlayEngine;    // Playpump used for IP recording input

    MList<ControlEventHandler> _handlers;
    void fireEvent(Event event);

    // state variables
    int _currentDecode;     // current decode that has focus
    MTimer _timer;
    bool _pipVisible;
    bool _hdmiConnected;
    boutput_dvi_capabilities _hdmiCapabilities;
    boutput_hdmi_hdcp_state _hdmiHdcpState;
    boutput_hdmi_cec_message_data _hdmiCecMessageData;
    boutput_hdmi_cec_configuration _hdmiCecConfiguration;
    void initHdmiOutput();
    void handleHdmiHdcp();
#ifdef NEXUS_BUILD
    void parseSeiMessage(NEXUS_VideoDecoderHandle videoDecoder, Brutus3DTVMode *mode);
#endif

    // incremental channel entry
    struct {
        int ch; /* current channel */
        int digits; /* current number of digits entered */
        int timeout; /* tickCount at which this should be applied */
    } _incrementalChEntry;

    // deferred channel change
    struct {
        int tickCount; // if !0, then deferral is pending
        bool digital;
        int channel; // channel only matters for digital
        bool ignoreContinuous;
        int decodeOverride;
    } _deferSetChannel;
    void doDeferredSetChannel();

    brutus_standby_state _standbyState;

#if defined (BRUTUS_LIBTUNE_SUPPORT) && defined (NEXUS_BUILD)
public:
    void Libtune_TuneWindows();
    void Libtune_SetUserMode(int usermode);
    void Libtune_SetUiControlValue(unsigned value);
private:
    int Libtune_Init(DecodeEngine const * const de, bdecode_window_t const * const window);
    int Libtune_CreateKey(int idx);
    void Libtune_TuneWindow(int idx);
    void Libtune_SourceChangeHandler();
    void Libtune_SetDisplayFormat();

    typedef enum {
        Libtune_UserMode_eStandard,
        Libtune_UserMode_eVivid,
        Libtune_UserMode_eMax
    } LibTune_UserMode;

    struct {
        /* handles */
        LIBTUNE_HANDLE handle;
        NEXUS_VideoInput input;
        NEXUS_VideoWindowHandle window[2];
        NEXUS_DisplayHandle display[2];

        /* not strictly needed by libtune, but useful for brutus integration */
        NEXUS_VideoDecoderHandle vdecoder;
        DecodeEngine *de;

        /* settings */
        NEXUS_VideoFormat sourceFormat;
        NEXUS_VideoFormat displayFormat[2];
        LibTune_UserMode userMode; /* user-adjustable only for main display */
        unsigned uiValue; /* assume only one uiControl group for now */
    } libtune;
#endif

#ifdef PVR_SUPPORT
    void showRecordingTerminationReason(PlayEngine *self);
#endif
#ifdef AUDIO_SUPPORT
    AudioControl *_audioControl;
#endif

    typedef enum {
        eFadeIn,
        eFadeOut
    } FadeType;
    void fade(FadeType fadetype);

    /* Settop API callbacks */
    static void video_source_changed(void *context);
    static void audio_source_changed(void *context);
    static void hdmi_hotplug_callback(void *context);
    static void hdmi_disconnect_callback(void *context);
    static void hdmi_hdcp_callback(void *context);
    static void hdmi_cec_message_callback(void *context);
    static void sei_callback(void *context, int param); /* 3dtv SEI frame packing message */

    /* This is set at Settop API callback-time to request idle-time execution. */
    bool _started_decode;
    bool _check_optimal_display;
    bool _check_hdmi_cec;
    bool _check_libtune;
    bool _check_3dtv;
    bool isValidHdmiFormat(bvideo_format format);
    bool _check_dnr_settings;
    bool _apply_dnr_settings;
    
#if B_HAS_DRM
    MMessageBox * _msgDRMStatus;
#endif

};

/**
* Generic interface for Control callbacks.
**/
class ControlEventHandler {
public:
    virtual void onControlEvent(Control *control, Control::Event event) {}
};

#define FADE_IN

#endif
