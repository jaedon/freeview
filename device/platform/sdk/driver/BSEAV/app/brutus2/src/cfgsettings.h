/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: cfgsettings.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 1/12/12 7:20p $
 *
 * Module Description: Config is pvrgui's registry
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus2/src/cfgsettings.h $
 * 
 * 1   1/12/12 7:20p tokushig
 * SW7405-5581: Brutus2 initial check in
 * 
 * 84   11/19/10 4:56p jtna
 * SW7405-4998: even more Brutus polishing
 * 
 * 83   11/8/10 5:40p vle
 * SW7405-4990: Add Brutus configuration to control the time out delay of
 * HDCP OSD error messages. The specific time out delay has a big impact
 * on Simplay test results.
 * 
 * 82   10/19/10 8:21a ahulse
 * SW7405-4951: Set DISABLE_CONSOLE_STACK_DUMP = true to disable dump of
 * crash stack trace
 * 
 * 81   10/6/10 2:13p calvinho
 * SW7401-4460: Add cfg option FORCE_3D_OUTPUT_FOR_INCOMPATIBLE_TV
 * 
 * 80   7/9/10 12:12p jtna
 * SW7405-3999: update help string
 * 
 * 79   4/19/10 3:37p calvinho
 * SW7420-466: Added options ASPECT_RATIO_HD and ASPECT_RATIO_SD in cfg
 * 
 * 78   4/15/10 10:23a calvinho
 * SW7420-466: Added valid range for video options in Help
 * 
 * 77   4/8/10 2:19p jtna
 * SW7420-466: added VIDEO_ANR and VIDEO_SPLIT_SCREEN_MODE cfgsettings
 * 
 * 76   3/29/10 10:45a erickson
 * SW7405-3625: add TVBEVEL_TEXT_DISABLED option
 *
 * 75   3/1/10 7:28p jtna
 * SW7420-527: added LIBTUNE_DBNAME cfgsetting
 *
 * 74   12/29/09 3:38p erickson
 * SW7420-522: brutus.cfg FIRST_CHANNEL=play(X) will start brutus playing
 * stream X
 *
 * 73   12/22/09 4:00p erickson
 * SW7420-70: add POWERSTANDBY_TIMEOUT option to brutus.cfg
 *
 * 72   12/1/09 4:44p mward
 * SW7125-129: Default DQT on.
 *
 * SW7125-129/1   11/25/09 4:56p mward
 * SW7125-129: Default DQT on.
 *
 * 71   10/1/09 12:47p erickson
 * SW7405-3106: add OUTPUT_3D_ZNORM. renamed OUTPUT_3D_MODE values to sbs
 * and ou.
 *
 * 70   9/30/09 10:38a erickson
 * SW7405-3106: add OUTPUT_3D_MODE
 *
 * 69   9/24/09 11:55a erickson
 * SW7405-3069: add passive standby options for USB, SATA and ETHERNET.
 * default to minimum power.
 *
 * 68   9/11/09 11:39a jjordan
 * SW7405-3017: add option to force compressed audio on HDMI
 *
 * 67   8/25/09 12:48p jtna
 * SWDEPRECATED-3910: video primer fast channel change feature
 *
 * 66   8/12/09 11:27a jjordan
 * PR51648: Add Bandgap adjustment to Brutus
 *
 * 65   8/4/09 1:26p vle
 * PR49558: Add support to control av mute delay before and after format
 * change separately.
 *
 * 64   7/9/09 11:52a jtna
 * PR56522: Remove multi-decode fast channel change feature. To be
 * replaced with video primer FCC.
 *
 * 63   7/8/09 10:40a erickson
 * PR56373: add FIRST_CHANNEL option. restore user's channel after wakeup
 * from passive standby mode.
 *
 * 62   6/24/09 7:17p lwhite
 * PR53773: Adding DTCP/IP + SSL Support in Brutus
 *
 * 61   5/22/09 3:28p jjordan
 * PR55415: Add setting of audio category code and copyright bit on SPDIF
 * and HDMI outputs via brutus.cfg
 *
 * 60   5/19/09 10:26a jjordan
 * PR54879: support setting of SD output format separately
 *
 * 59   5/12/09 3:27p jjordan
 * PR52687: Add method to set audio downmix type
 *
 * 58   5/6/09 3:55p jjordan
 * PR50291: add Wide GA mode (config from brutus.cfg)
 *
 * 57   4/16/09 12:27p mward
 * PR53139: Create brutus.cfg HDMI_PASSTHROUGH_ENABLED to initialize HDMI
 * to compressed audio.  Default "no".
 *
 * 56   3/16/09 4:47p randyjew
 * PR42789: updates for BLUETOOTH_SUPPORT
 *
 * 55   10/31/08 12:27p jjordan
 * PR47230: Port TTS Pacing to Nexus
 *
 * 54   10/27/08 4:12p erickson
 * PR48439: added STREAM_INFO_DISABLED
 *
 * 53   8/15/08 10:08a erickson
 * PR42789: bluetooth update
 *
 * 52   8/13/08 5:26a erickson
 * PR42789: updates for BLUETOOTH_SUPPORT
 *
 * 51   8/1/08 11:00a erickson
 * PR45286: add ALLOW_INPUT_DURING_SCRIPT option
 *
 * 50   7/9/08 9:47a erickson
 * PR17425: added MAIN_WINDOW and PIP_WINDOW brutus.cfg options
 *
 * 49   5/27/08 4:56p jtna
 * PR42200: Multi-decode fast channel change feature
 *
 * 48   3/28/08 5:17p vle
 * PR41000: Add Simplay test mode and additional HDCP error state.
 *
 * 47   3/22/08 2:09a erickson
 * PR40307: thumbnail demo update
 *
 * 46   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 *
 * 45   3/18/08 4:05p erickson
 * PR40307: added thumbnail feature
 *
 * 44   3/3/08 3:46p erickson
 * PR40168: added VIDEO_TOOL brutus.cfg optoin
 *
 * 43   2/13/08 4:52p tokushig
 * PR39534: added bluetooth remote capabilities to brutus
 *
 * 42   12/7/07 2:01p erickson
 * PR37590: add new config options to control PVR action and BOF/EOF w/
 * and w/o record
 *
 * 41   12/6/07 2:08p erickson
 * PR37038: add CONTINUOUS_RECORD_DURATION, remove unused fixedSize and
 * CIRCFILE.
 *
 * 40   11/28/07 3:38p vle
 * PR 36971: Add Brutus configuration delay after AVMute and before
 * AVUnmute.
 *
 * SanDiego_Brutus_3D/5   1/30/08 2:19p tokushig
 * refactor demo bluetooth remote implementation for integration into main
 * line
 *
 * SanDiego_Brutus_3D/4   1/4/08 6:37p tokushig
 * added youtube/mydeo icon to fullscreen and network menus.  added brcm
 * logo to network menu items.  added autozoom feature for youtube video.
 * still need to update graphics for youtube video.
 *
 * SanDiego_Brutus_3D/3   12/21/07 5:04p tokushig
 * added <X> button on bluetooth remote...will show 3d intro before going
 * to main menu. <options> key will skip intro and just go to menu.  note
 * there is no corresponding <x> key on the allforone ir remote so this
 * option is not available...in this case the 3d intro will be seen on
 * brutus startup only
 *
 * SanDiego_Brutus_3D/2   12/21/07 3:44p tokushig
 * add option to brutus.cfg USE_NEW_3D_ICON_SET (defaults to 1) which
 * allows user to choose between new icon set and brcm icon set - must
 * restart brutus for changes to take effect.
 *
 * SanDiego_Brutus_3D/1   12/21/07 2:40p tokushig
 * merged BT_REMOTE_ACCESS brutus config option
 *
 * SanDiego_Brutus_BTRemote/1   12/21/07 1:11p randyjew
 * improved bluetooth modificatiosn for allowing commands during messages
 * boxes and to be able to configure address in brutus.cfg file
 * BT_REMOTE_ADDRESS=<12_character_address>
 *
 * 39   10/18/07 5:05p vle
 * PR 36200: Add Brutus config option to disable HDCP OSD error messages.
 *
 * 38   9/17/07 12:30p jrubio
 * PR34613: added new CLIENT ID field for NetRecord
 *
 * 37   8/9/07 5:28p jrubio
 * PR33830: added Network record settings
 *
 * 36   6/12/07 1:32p erickson
 * PR31914: add AUTO_OUTPUT_FORMAT
 *
 * 35   4/27/07 2:44p ahulse
 * PR29929: Add user settable compositor background color
 *
 * 34   4/18/07 1:46p vle
 * PR 24304: Add HDMI_HDCP_ENABLED config setting to enable/disable HDCP
 * when starting Brutus
 *
 * 33   3/8/07 5:03p jgarrett
 * PR 28553: Adding variables for video settings
 *
 * 32   1/24/07 11:42a ahulse
 * PR25913: set rfmod country code according to 2 letter country code set
 * by user
 *
 * 31   12/6/06 10:43a ahulse
 * PR25913: Add INFO_SHOW_ALL_FORMATS to display all output formats
 * supported by settopApi in info panel
 *
 * 30   11/30/06 6:32p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 *
 * 29   11/21/06 8:52a erickson
 * PR23575: add DEMUX_PROGRAMS feature (defaulted off) to create separate
 * channels for streams with multiple audio/video pids.
 *
 * 28   11/16/06 6:25p gmohile
 * PR 25901: Remove HDSD_SIMUL support
 *
 * 27   9/29/06 12:15p arbisman
 * PR24576: Integration w/ BLIVE_EXT Library (build error if built without
 * Livemedia)
 *
 * 26   9/28/06 11:52p ssood
 * PR24576: Integration w/ BLIVE_EXT Library (SAP + LM Scheduler)
 *
 * 25   8/8/06 3:28p erickson
 * PR23129: converted B_HAS_656_INPUT to B_HAS_EXTERNAL_ANALOG for 656/I2S
 * input support
 *
 * 24   7/21/06 10:41a erickson
 * PR22837: added B_HAS_656_INPUT support for 740x
 *
 * 23   6/20/06 6:24p rgreen
 * PR20869: Add control options for enabling pre-emphasis, HDCP Pj
 * checking, and HDCP retry delay
 *
 * 22   5/31/06 4:21p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 ****************************************************************************/
#ifndef CFG_SETTINGS
#define CFG_SETTINGS

#include "mstringhash.h"

#ifndef DECLARE_CONFIGSETTING_VALUES
#define MAX_CONFIGSETTING_VALUE 256
#define DECLARE(SETTING,TYPE,DESCRIPTION) SETTING

/*=*********************************
Config is like the "Windows Registry" for brutus.
It contains all configuration information for the application.
Some config values can be set by command line parameters.

The command line does not override the config file.

Data storage is name, value pairs.
There are predefined names which can be accessed with an enum. This gives the application
some compile-time checking without eliminating the ability to do programmatic access
(e.g. using sprintf to have an indexed setting.)

It stores and retrives string, integer, float and boolean data types.
Data conversion is implicit. Native storage format is a string.
Boolean true is defined as any string which begins with 't' or 'T', or which
is converted to a numeric non-zero value.

***********************************/

/* In this mode, define the enum and class. */
class Config {
private:
    struct Predefined {
        // enum is the index into the array
        const char *name;
        const char *type;
        const char *description;
    };
public:
enum {

#else // DECLARE_CONFIGSETTING_VALUES

/* In this mode, define the data structure. */
#define DECLARE(SETTING,TYPE,DESCRIPTION) {#SETTING, #TYPE, DESCRIPTION}
Config::Predefined Config::predefines[] = {
#endif


/* Here's the dual-purpose code */
    DECLARE(RECORD_TIME_WINDOW, int, "Specify how many seconds is represented by the pvr timeline bar."),
    DECLARE(BEST_QUALITY_BITRATE, int, "Encoder bitrate in Mbits/sec"),
    DECLARE(HIGH_QUALITY_BITRATE, int, "Encoder bitrate in Mbits/sec"),
    DECLARE(MEDIUM_QUALITY_BITRATE, int, "Encoder bitrate in Mbits/sec"),
    DECLARE(BASIC_QUALITY_BITRATE, int, "Encoder bitrate in Mbits/sec"),
    DECLARE(VPP_ENABLED, int, "Enable configurable Vpp settings in record menu"),
    DECLARE(PLAYBACK_ENABLED, bool, "Runtime switch for playback"),
    DECLARE(RECORD_ENABLED, bool, "Runtime switch for record"),
    DECLARE(VBI_ENABLED, bool, "Runtime switch to enable VBI to be encoded to the display"),
    DECLARE(MP3_ENABLED, bool, "Runtime switch for MP3 screen"),
    DECLARE(PICTURES_ENABLED, bool, "Runtime switch for Pictures screen"),
    DECLARE(MAX_PICTURES, int, "Threshold for automatically deleting pictures to prevent disk full during demos"),
    DECLARE(VIDEO_PRIMER_FASTCHANNELCHANGE_ENABLED, bool, "Runtime switch for video primer fast channel change"),
    DECLARE(PIP_ENABLED, bool, "Runtime switch for PIP"),
    DECLARE(SIDEBYSIDE_ENABLED, bool, "Runtime switch for Side-by-Side mode for PIP (also removes PVR timeline"),
    DECLARE(NEW_VIDEO_DESCRIPTION, string, "Description embedded in info files for new recordings"),
    DECLARE(MAIN_WINDOW, string, "Override size and position of main window. MAIN_WINDOW=x,y,width,height"),
    DECLARE(PIP_WINDOW, string, "Override size and position of PIP window. PIP_WINDOW=x,y,width,height"),
    DECLARE(FIRST_CHANNEL, string, "If an integer, the live channel number to start on. If play(X), start playing stream X."),

    /* PVR options */
    DECLARE(LOOP_PLAYBACK, boolean, "See PLAYBACK_AT_BOF and PLAYBACK_AT_EOF."),
    DECLARE(PLAYBACK_AT_BOF_WITH_RECORD, string, "What action to take at beginning-of-file when timeshifting. Set to 'play', 'loop', or 'pause'. Default is 'pause'."),
    DECLARE(PLAYBACK_AT_EOF_WITH_RECORD, string, "What action to take at end-of-file when timeshifting. Set to 'play', 'loop', or 'pause'. Default is 'play'."),
    DECLARE(PLAYBACK_AT_BOF, string, "What action to take at beginning-of-file when not timeshifting. Set to 'play', 'loop', or 'pause'. Default is 'loop'."),
    DECLARE(PLAYBACK_AT_EOF, string, "What action to take at end-of-file when not timeshifting. Set to 'play', 'loop', or 'pause'. Default is 'loop'."),
    DECLARE(CONTINUOUS_RECORD_ENABLED, bool, "Enable continuous recording on live channels"),
    DECLARE(CONTINUOUS_RECORD_DURATION, int, "Number of seconds stored in the continuous record buffer"),

    DECLARE(HOME_LOGO, filename, "Image used on Brutus home screen"),
    DECLARE(HOME_TITLE, string, "Title displayed on Brutus home screen"),

    DECLARE(START_FULLSCREEN, bool, "Start Brutus with the full-screen decode window"),

    DECLARE(LIVEDECODE_ENABLED, bool, "debug mode"), /* deprecated. just run with "-ch none" */
    DECLARE(AUDIODECODE_ENABLED, bool, "debug mode"),
    DECLARE(VIDEODECODE_ENABLED, bool, "debug mode"),
    DECLARE(TRICKMODES_ENABLED, bool, " debug mode"),
    DECLARE(ENCODER_ENABLED, bool, "debug mode"),
    DECLARE(SCALED_VIDEO_ENABLED, bool, "debug mode"),
    DECLARE(FORCE_BCM_TRICK_MODES, bool, "debug mode"),
    DECLARE(FORCE_HOST_TRICK_MODES, bool, "debug mode"),
    DECLARE(FORCE_IFRAME_TRICK_MODES, bool, "debug mode"),
    DECLARE(DQT_ENABLED, bool, "Display Queue Trick mode. Default is enabled."),
	DECLARE(DQT_PICS_PER_GOP, int, "Number of pictures per GOP for DQT. Default is 10."),

    DECLARE(VIDEO_TOOL, bool, "Launch BBS VideoTool client"),

    DECLARE(CONTINUITY_COUNTER_DISABLED, bool, "debug mode"),

    DECLARE(DEFERRED_CHANNELCHANGED_ENABLED, bool, "remove user interface delay for channel change"),

    DECLARE(RESUME_ENABLED, bool, "Allow PVR playback to be resumed from a location"),
    DECLARE(ADMIN, bool, "If false, some features are disabled, including the Admin screen. "
        "This is used to keep certain demo features protected at a tradeshow."),
    DECLARE(ADMIN_PASSWORD, string, "The password needed to access the Admin screen if ADMIN mode is false."),
    DECLARE(REMOTE_ENABLED, bool, "GI IR remote"),
    DECLARE(SA_REMOTE, bool, "SA IR remote (normally used for App B)"),
    DECLARE(IRECHO_REMOTE, bool, "Echostar IR remote"),
    DECLARE(ECHO_REMOTE, bool, "EchoStar UHF remote for 740x"),
    DECLARE(DRCTV_REMOTE, bool, "DirecTV UHF remote for 740x"),

    DECLARE(BT_ENABLED, bool, "Enable embedded bluetooth stack"),
    DECLARE(BT_REMOTE_ADDRESS, string, "Sony Playstation 3 Bluetooth remote address"),
    DECLARE(BT_AUDIO_INPUT_ENABLED, bool, "Enable audio input from Bluetooth device"),
    DECLARE(BT_AUDIO_SINK_ENABLED, bool, "Enable audio sink from Bluetooth device"),

    DECLARE(B_HAS_EXTERNAL_ANALOG, bool, "Support 656/I2S input using ANALOG channel map entry"), /* deprecated. switch to bconfig->features.analog. */

    /* DUAL_OUTPUT options */
    DECLARE(FRONTPANEL_FOR_SECOND_DISPLAY, bool, "which App has this input"),
    DECLARE(IRKEYBOARD_FOR_SECOND_DISPLAY, bool, "which App has this input"),
    DECLARE(SVIDEO_FOR_SECOND_OUTPUT, bool, "which App has this output"),
    DECLARE(RFMOD_FOR_SECOND_OUTPUT, bool, "which App has this output"),
    DECLARE(SECOND_DISPLAY, bool, "Internal"),

    DECLARE(DUAL_OUTPUT, bool, "Run two instances of Brutus for SD and HD outputs, separate UI's"),
    DECLARE(HDSD_SINGLE, bool, "Drive SD and HD outputs with same UI"),
    DECLARE(INFO_SHOW_ALL_FORMATS, bool, "Show all supported display formats in info panel"),

    DECLARE(CHANNEL_MAP, filename, "Channel map for PSI channel scan"),
    DECLARE(PROGRAM_GUIDE, filename, "Program names"),
    DECLARE(OUTPUT_SETTINGS_FILE, filename, "Allows brutus to save last output setting"),
    DECLARE(CHANNEL_MAP_SELECT_FILE, filename, "Allows brutus to select from multiple channel maps"),
    DECLARE(CHANNEL_MAP_LIST, filename, "Allows brutus to select from multiple channel maps"),
    DECLARE(POD_CHANNEL_MAP, filename, "Enables POD-based frontend"),
    DECLARE(FORCE_USE_OF_DAUGHTERCARDS, bool, "Prevents use of on-board tuners in preference of daughter cards"),

    // output settings
    DECLARE(RFMOD_CHANNEL, int, "Default channel for RF modulated output"),
    DECLARE(OUTPUT_FORMAT, string, "Set initial main display format. Use same text as appears on Info panel."),
    DECLARE(SD_OUTPUT_FORMAT, string, "Set initial SD output display format. Use same text as appears on Info panel."),
    DECLARE(AUTO_OUTPUT_FORMAT, bool, "If true, have the display video format match the source video format."),
    DECLARE(OUTPUT_SELECT, string, ""),
    DECLARE(ASPECT_RATIO_SD, string, "Set initial SD aspect ratio. Options are Zoom, Box, Pan Scan, Full, and Nonlinear."),
    DECLARE(ASPECT_RATIO_HD, string, "Set initial HD aspect ratio. Options are Zoom, Box, Pan Scan, Full, and Nonlinear."), 
    DECLARE(RFMOD_COUNTRY, string, "Specify ISO 3166 2 char country code"),
    DECLARE(NO_SIGNAL_BACKGROUND_COLOR, color, "Specify Color (ARGB) for no signal condition or side bars"),
    DECLARE(OUTPUT_3D_MODE, string, "3D-TV output mode, set to either 'sbs' or 'ou' or 'auto'"),
    DECLARE(OUTPUT_3D_ZNORM, int, "3D-TV z-norm value"),
    DECLARE(OUTPUT_3D_STATUS_TIMEOUT, int, "3D-TV status timeout in milliseconds"),
    DECLARE(FORCE_3D_OUTPUT_FOR_INCOMPATIBLE_TV, bool, "Force 3D output regardless of TV's EDID information"),

    DECLARE(MIX_PCM_WITH_DECODE, bool, "When playing MP3, don't mute audio. This causes the two audio streams to be mixed."),

    DECLARE(ALWAYS_REFRESH_MP3S, bool, "Read the contents of the disk whenever displaying the screen"),
    DECLARE(ALWAYS_REFRESH_SHOWS, bool, "Read the contents of the disk whenever displaying the screen"),
    DECLARE(WEBSERVER_PORT, int, "Used for image-transfer demo"),
    DECLARE(WEBSERVER_ENABLED, bool, "Used for image-transfer demo"),
    DECLARE(SHOW_VIDEODIR_LABELS, bool, ""),
    DECLARE(THUMBNAIL_MODE, bool, "Enable thumbnails for Playback files"),
    DECLARE(THUMBNAIL_MEMORY, bool, "Maximum memory for thumbnails, in units of megabytes"),
    DECLARE(THUMBNAIL_SPACING, bool, "Spacing of thumbnails on TV screen, in units of seconds"),
    DECLARE(FULLSCREEN_TV_ALPHA, int, "OSD alpha-blending value when in full-screen TV mode"),
    DECLARE(LIBTUNE_DBNAME, string, "filename for libtune database"),

    DECLARE(AUDIO_ONLY_CHANNELS_ENABLED, bool, "PSI filtering param"),
    DECLARE(ENCRYPTED_CHANNELS_ENABLED, bool, "PSI filtering param"),
    DECLARE(DATA_CHANNELS_ENABLED, bool, "PSI filtering param"),
    DECLARE(DUPLICATE_CHANNELS_ENABLED, bool, "PSI filtering param"),
    DECLARE(DEMUX_PROGRAMS, bool, "If multiple pids in program, demux into different channels"),

    DECLARE(INITIAL_SCRIPT, filename, "Equivalent to -scr cmd line option"),
    DECLARE(INITIAL_SCRIPTTEXT, string, "Equivalent to -scrtext cmd line option"),
    DECLARE(INTERACTIVE_MODE, bool, "Equivalent to --exec cmd line option"),
    DECLARE(TTY, bool, "Equivalent to -tty cmd line option"),
    DECLARE(ALLOW_INPUT_DURING_SCRIPT, bool, "Allow other user input to be processed without terminating a script"),

    DECLARE(APP_DEBUG_LEVEL, string, "Either msg, wrn or err for DEBUG=y modes"),
    DECLARE(INIT_ONLY, bool, "If true, brutus will quit after initialization"),

    DECLARE(AUDIO_PATH, string, "Path to scan for MP3's"),
    DECLARE(PICTURES_PATH, string, "Path to scan for Pictures"),
    DECLARE(SCRIPTS_PATH, string, "Path to scan for Scripts"),
    DECLARE(IMAGES_PATH, string, "Path to scan for internal images"),
    DECLARE(SKINS_PATH, string, "Path to scan for Skins"),
    DECLARE(FONTS_PATH, string, "Path to scan for Fonts"),

    /* HDMI/DVI options */
    DECLARE(DVI_USE_PREFERRED_FORMAT, bool, "If true, set the initial display format to be the DVI/HDMI preferred format if available."),
    DECLARE(DVI_VALIDATE_EDID_FORMAT, string, "HDMI will check if a display format is supported in the EDID before switching."),
    DECLARE(HDMI_HDCP_PJ_CHECK, bool, "Enable HDCP 1.1 Optional Pj Checking"),
    DECLARE(HDMI_HDCP_AUTH_RETRY_DELAY, int, "Delay after HDCP failure until Re-Authentication attempt"),
    DECLARE(HDMI_HDCP_AUTO_PRE_EMPHASIS, bool, "Automatically enable HDMI PreEmpahsis if HDCP link errors"),
    DECLARE(HDMI_HDCP_OSD_DISABLED, bool, "Disable HDCP OSD error messages"),
    DECLARE(HDMI_HDCP_OSD_TIMEOUT_DELAY, int, "Time out period for HDCP OSD error messages"),
    DECLARE(HDMI_HDCP_SIMPLAY_MODE_ENABLED, bool, "Enable Simplay test mode"),
    DECLARE(HDMI_AVMUTE_DELAY_PRE_FORMAT_CHANGE, int, "Delay after AVMute before format change"),
    DECLARE(HDMI_AVMUTE_DELAY_POST_FORMAT_CHANGE, int, "Delay before AVUnmute, after format change"),
#if BDBG_DEBUG_BUILD
    /* User Access to software switches to enable/disable HDCP is a violation of the HDCP Robustness rules.
        Such functionality should not be part of any production code.   */
    DECLARE(HDMI_HDCP_ENABLED, bool, "Automatically enable HDCP"),
#endif
    DECLARE(HDMI_FORCE_COMPRESSED_MENU_ITEM, bool, "Allow selection of compressed audio on HDMI, even if the EDID indicates no support"),

    /* If you're running in scripting mode, you might want no prompts, even on errors.
    This is especially true for anything related to live feeds, where it may or may not
    have a problem. If the prompt is guaranteed to always come up (like delete a video
    confirmation), then it is not disabled. TODO: we might need a more thorough
    use of this. PR 10159. */
    DECLARE(PROMPT_ENABLED, bool, ""),

    // skin params
    DECLARE(PIXEL_FORMAT, string, "ARGB1555, ARGB8888, RGB565 or PALETTE8"),
    DECLARE(CHROMAKEY_COLOR, color, "The color used to punch video through graphics if there's no alpha-per-pixel"),
    DECLARE(BACKGROUND_COLOR, color, "The color which will be used as the background for all screens"),
    DECLARE(BACKGROUND_IMAGE, filename, "An image which will be used as the background for all screens"),
    DECLARE(TEXT_COLOR, color, ""),
    DECLARE(FOCUS_COLOR, color, ""),
    DECLARE(PVRBAR_PLAYBACK_COLOR, color, ""),
    DECLARE(PVRBAR_RECORDING_COLOR, color, ""),
    DECLARE(PVRBAR_CURSOR_COLOR, color, ""),
    DECLARE(PVRBAR_CURSOR_WIDTH, color, ""),
    DECLARE(TVBEVEL_WIDTH, int, "Size of bevel around tv window"),
    DECLARE(TVBEVEL_COLOR, int, "Color of bevel around tv window"),
    DECLARE(TVBEVEL_TEXT_DISABLED, bool, "Disable text in the tv window"),
    DECLARE(PVRBAR_OUTLINE_COLOR, color, ""),
    DECLARE(BUTTON_FACE_FOCUSED_COLOR, color, ""),
    DECLARE(TEXT_FOCUSED_COLOR, color, ""),
    DECLARE(GUI_TRANSITIONS_ENABLED, bool, ""),
    DECLARE(HIDE_PVR_TIMELINE, bool, "Hide the PVR timeline after a few seconds in normal play mode"),
    DECLARE(PVR_TIMELINE_ENABLED, bool, "Enable/disable the PVR timeline"),

    DECLARE(SKIN, filename, "XML Skin directory to use for GUI"), /* this is the directory that contains the skin definition */
    DECLARE(SKIN_FILE, filename, "XML file to define Skin"),      /* this is the xml file that defines the skin */
    DECLARE(SCREEN_X, int, "Change origin of GUI to adapt to TV's viewable area"),
    DECLARE(SCREEN_Y, int, "Change origin of GUI to adapt to TV's viewable area"),

    DECLARE(INFOPANEL_STICKY, bool, "If true, the info panel will never automatically hide"),
    DECLARE(STREAM_INFO_DISABLED, bool, "Don't show stream info unless the info panel is requested."),
    DECLARE(BTSC_SAP_ENABLED, bool, ""),
    DECLARE(SPDIF_PASSTHROUGH_ENABLED, bool, ""),
    DECLARE(HDMI_PASSTHROUGH_ENABLED, bool, ""),

    DECLARE(ARIB_DOWNMIX_MODE_ENABLED, bool, "Enable ARIB audio downmix mode"),
    DECLARE(WGA_MODE_ENABLED, bool, "Enable audio Wide Gross Adjustment (GA) mode"),
    // font string format: name;size[;AA][;PK]
    DECLARE(NORMAL_FONT, font, "Main font"),
    DECLARE(MAINMENU_TITLE_FONT, font, "Large main menu title font"),
    DECLARE(SCREEN_TITLE_FONT, font, "Title font for each screen"),
    DECLARE(TINY_FONT, font, "Used for very small buttons"),
    DECLARE(TVSCREEN_MESSAGE_FONT, font, "Non-antialiased font for use over colorkey"),
    DECLARE(TVSCREEN_MESSAGE_FONT_DROPSHADOW, font, "Drop shadow for tv window"),

    DECLARE(MAX_RECORD_TIME, int, "")   /* In seconds. For some demos, we want a maximum recording time
                                    so that it doesn't get left on and fill up the disk.
                                    If the time is reached, the recording is stopped and
                                    restarted for the same filename. A brief user message
                                    is given. */
    ,
    DECLARE(MANAGE_IP_NETWORK_BUFFER, bool, "If true, the IP STB network buffer will be managed"),
    DECLARE(IPSERVER_IPV4, string, "IP Address of IP Streamer "),
    DECLARE(IPSERVER_REC_PORT, int, "PORT number to send request for NET RECORD"),
    DECLARE(IPSERVER_CLIENT_ID, string, "Give this Client a name such as ClientA or ClientB, No more then 16 characters Long"),
    DECLARE(IP_USE_LIVE_PLAYBACK, bool, "If true, the IP STB will run in live playback mode"),
    DECLARE(TTS_AUTO_DETECT, bool, "Automatically detect if the stream is TTS or TS"),
    DECLARE(TTS_PACING_MAX_ERROR, int, "Set the timestamp error bound, as used by the playback pacing logic"),
    DECLARE(TTS_MIN_BUF_DEPTH, int, "minimum buffer depth before tts throttle buffer depth violation"),
    DECLARE(TTS_INIT_BUF_DEPTH, int,"initial buffer depth to achieve before starting playback"),
    DECLARE(TTS_MAX_BUF_DEPTH, int, "maximum buffer depth before tts throttle buffer depth violation"),
    DECLARE(TTS_MAX_CLOCK_MISMATCH, int, "specify the maximum clock mismatch (in ppm) between server/encoder and STB"),
    DECLARE(VIDEO_DAC0_BAND_GAP, int, "Specify the video DAC0 band gap adjustment"),
    DECLARE(VIDEO_DAC1_BAND_GAP, int, "Specify the video DAC1 band gap adjustment"),
    DECLARE(VIDEO_DAC2_BAND_GAP, int, "Specify the video DAC2 band gap adjustment"),
    DECLARE(VIDEO_DAC3_BAND_GAP, int, "Specify the video DAC3 band gap adjustment"),
    DECLARE(VIDEO_DAC4_BAND_GAP, int, "Specify the video DAC4 band gap adjustment"),
    DECLARE(VIDEO_DAC5_BAND_GAP, int, "Specify the video DAC5 band gap adjustment"),
    DECLARE(VIDEO_DAC6_BAND_GAP, int, "Specify the video DAC6 band gap adjustment"),
#ifdef LIVEMEDIA_SUPPORT
    DECLARE(SAP_TIMEOUT_INTERVAL, int, "Specify number of seconds after which a IP Channel Entry is timed out"),
#endif
    DECLARE(AUDIO_CATEGORY_CODE, int, "Sets the audio category code in SPDIF and HDMI outputs"),
    DECLARE(AUDIO_COPYRIGHT, bool, "Sets the audio copyright bit in SPDIF and HDMI outputs"),
    DECLARE(VIDEO_CONTRAST, int, "Contrast Level. Valid range is [-32768,32767]"),
    DECLARE(VIDEO_SATURATION, int, "Saturation Level. Valid range is [-32768,32767]"),
    DECLARE(VIDEO_HUE, int, "Hue Level. Valid range is [-32768,32767]"),
    DECLARE(VIDEO_BRIGHTNESS, int, "Brightness Level. Valid range is [-32768,32767]"),
    DECLARE(VIDEO_SHARPNESS, int, "Sharpness Level. Valid range is [-32768,32767]"),
    DECLARE(VIDEO_BNR, int, "Block noise reduction Level. Valid range is [0,5]"),
    DECLARE(VIDEO_MNR, int, "Mosquito noise reduction Level. Valid range is [0,5]"),
    DECLARE(VIDEO_DCR, int, "Digital contour removal Level. Valid range is [0,5]"),
    DECLARE(VIDEO_ANR, int, "Active noise reduction Level. Valid range is [0,5]"),
    DECLARE(VIDEO_SPLIT_SCREEN_MODE, int, "Split screen mode. 0: wholescreen, 1: left, 2: right"),
    DECLARE(VIDEO_DYNAMIC_CONTRAST, int, "Dynamic Contrast Level. Valid range is [0,255]"),
    DECLARE(VIDEO_FLESH_TONE, int, "Video Flesh Tone Level. Valid range is [0,4]"),
    DECLARE(VIDEO_GREEN_STRETCH, int, "Green Stretch Level. Valid range is [0,4]"),
    DECLARE(VIDEO_BLUE_STRETCH, int, "Blue Stretch Level. Valid range is [0,4]"),
    DECLARE(SSL_CA_FILE, string, "Certification Authority file "),
    DECLARE(SSL_CA_PATH, string, "CA path - not implemented"),
    DECLARE(SSL_CLIENT_CERT_FILE, string, "Client certificate file"),
    DECLARE(SSL_CLIENT_CERT_KEY, string, "Client certificate key file"),
    DECLARE(SSL_CLIENT_CERT_PASSWORD, string, "Client password"),
    DECLARE(DTCP_IP_AKE_SERVER_PORT, int, "DTCP/IP AKE server port"),

    /* power standby options */
    DECLARE(POWERSTANDBY_USB, bool, "Power down USB in passive standby mode"),
    DECLARE(POWERSTANDBY_SATA, bool, "Power down SATA in passive standby mode"),
    DECLARE(POWERSTANDBY_ETHERNET, bool, "Power down ethernet in passive standby mode"),
    DECLARE(POWERSTANDBY_TIMEOUT, int, "Passive standby mode timeout (in seconds). Zero (default) means no timeout."),

    /* debug when crash */
    DECLARE(DISABLE_CONSOLE_STACK_DUMP, bool, "Disable default dump crash stack to console.")
};

#ifndef DECLARE_CONFIGSETTING_VALUES

    Config(const char *filename = NULL);
    Config(const Config &cfg);

    virtual ~Config();
    int read(const char *filename);
    void print() const {_inifile->print();}
    void printHelp() const;

    /**
    * Predefines
    **/
    static int total();
    static const char *getName(int index);
    static const char *getDescription(int index);

    const char *get(const char *name, const char *defaultvalue = NULL) const;
    int getInt(const char *name, int defaultvalue = 0) const;
    double getDouble(const char *name, double defaultvalue = 0.0) const;
    bool getBool(const char *name, bool defaultvalue = false) const;

    void set(const char *name, const char *value);
    void set(const char *name, double value);
    void set(const char *name, int value);

    /**
    * Convert index to name
    **/
    const char *get(int predefinedIndex, const char *defaultvalue = NULL) const
        {return get(getName(predefinedIndex), defaultvalue);}
    int getInt(int predefinedIndex, int defaultvalue = 0) const
        {return getInt(getName(predefinedIndex), defaultvalue);}
    double getDouble(int predefinedIndex, double defaultvalue = 0.0) const
        {return getDouble(getName(predefinedIndex), defaultvalue);}
    bool getBool(int predefinedIndex, bool defaultvalue = false) const
        {return getBool(getName(predefinedIndex), defaultvalue);}
    void set(int predefinedIndex, const char *value)
        {set(getName(predefinedIndex), value);}
    void set(int predefinedIndex, double value)
        {set(getName(predefinedIndex), value);}
    void set(int predefinedIndex, int value)
        {set(getName(predefinedIndex), value);}

private:
    static Predefined predefines[];
    MStringHash *_inifile;
};
#endif

#endif /* CFG_SETTINGS */
