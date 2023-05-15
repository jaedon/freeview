/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: brutus_cfg.h $
* $brcm_Revision: 23 $
* $brcm_Date: 11/12/12 3:32p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/brutus_cfg.h $
* 
* 23   11/12/12 3:32p tokushig
* SW7231-749: update PAT/PMT timeouts
* 
* 22   10/10/12 9:57p jrubio
* SW7346-1004: update brutus.cfg with new defines
* 
* 21   10/8/12 10:44p jrubio
* SW7346-1004: add grid timeouts
* 
* 20   10/8/12 4:16p jrubio
* SW7346-1004: add constellation timeouts
* 
* 19   9/29/12 9:20p tokushig
* SW7231-749: connect audio settings window panel to simple audio
* decoder.  pid, spdif, hdmi, downmix, dualmono, dolby drc, and dialog
* norm are implemented.
* 
* 18   7/27/12 11:20a jrubio
* SW7346-848: lower PAT/PMT timeouts
* 
* 17   7/27/12 10:46a jrubio
* SW7346-848: Add PAT/PMT timeouts
* 
* 16   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 15   5/4/12 2:04p tokushig
* SW7231-749: add buffers screen, checkbox widget, and new fonts
* 
* 14   4/20/12 2:18p jrubio
* SW7346-644: add default VIDEOS_PATH to brutus config
* 
* 13   4/19/12 7:52p tokushig
* SW7231-749: add dynamic scan options to main menu.  buttons will only
* correspond to allowed scan types based on the types of available
* tuners
* 
* 12   4/17/12 4:33p tokushig
* SW7231-749: add timeout value for display of ui channel num
* 
* 11   4/9/12 7:30p tokushig
* SW7405-5581: set up hd graphics.  using 960x540 for easy scaling for
* both 720p and 1080i/p
* 
* 10   4/6/12 10:04a tokushig
* SW7405-5581: advanced scan changes for VSB tuner.
* 
* 9   3/30/12 9:37p tokushig
* SW7231-749: modify fast qam scan and test on 97231
* 
* 8   3/27/12 10:30a tokushig
* SW7405-5581: add vsb tuning and channel scan
* 
* 7   3/19/12 5:27p tokushig
* SW7405-5581: add config entries to enable channel scan to save
* encrypted channels, audio only channels, and duplicate channels
* 
* 6   3/12/12 4:42p tokushig
* SW7405-5581: after channel load, unnumbered channels will be auto-
* numbered starting with the last valid channel number in the channel
* list.  channel numbers can not be saved/restored in channel list
* save/load.  moved code to load channels from xml to the appropriate
* channel classes.
* 
* 5   3/9/12 2:55p tokushig
* SW7405-5581: added support for channel list save.  also added initial
* code to handle tuner lock/unlock after first tune.
* 
* 4   2/28/12 3:01p tokushig
* SW7405-5581: add timer support and use it for deferred tuning using ch
* up/down.  fast pressing of ch up/down remote buttons will update the
* ui but not tune until button presses stop for 500msecs.  this allows
* the user to skip channels while traversing the channel list using the
* remote
* 
* 3   2/20/12 4:48p jrubio
* SW7346-644: add CHANNELS_LIST var
* 
* 
*********************************************************************************/
#ifndef BRUTUS_CFG_H__
#define BRUTUS_CFG_H__

#include "mstringhash.h"

/* config macros to simplify get/set */
#define GET_NAME(CFG, NAME)      ((CFG)->getName(CConfiguration::NAME))
#define GET_DESCR(CFG, NAME)     ((CFG)->getDescription(CConfiguration::NAME))
#define GET_BOOL(CFG, NAME)      ((CFG)->getBool(CConfiguration::NAME))
#define GET_DOUBLE(CFG, NAME)    ((CFG)->getDouble(CConfiguration::NAME))
#define GET_INT(CFG, NAME)       ((CFG)->getInt(CConfiguration::NAME))
#define GET_STR(CFG, NAME)       ((CFG)->get(CConfiguration::NAME))
#define SET(CFG, NAME, VALUE)    ((CFG)->set(CConfiguration::NAME, VALUE))

#ifndef BRUTUS_DECLARE_CONFIGSETTING_VALUES
#define MAX_CONFIGSETTING_VALUE 256
#define BRUTUS_DECLARE(SETTING,TYPE,DEFAULT,DESCRIPTION) SETTING

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
class CConfiguration {
private:
    struct Predefined {
        // enum is the index into the array
        const char * _name;
        const char * _type;
        const char * _default;
        const char * _description;
    };
public:
enum {

#else // BRUTUS_DECLARE_CONFIGSETTING_VALUES

/* In this mode, define the data structure. */
#define BRUTUS_DECLARE(SETTING,TYPE,DEFAULT,DESCRIPTION) {#SETTING, #TYPE, DEFAULT, DESCRIPTION}
CConfiguration::Predefined CConfiguration::_predefines[] = {
#endif


/* Here's the dual-purpose code */
    BRUTUS_DECLARE(EXIT_APPLICATION,                 bool,   "false",            "Gracefully quit the application."),
    BRUTUS_DECLARE(FIRST_TUNE,                       bool,   "true",             "Tune to current channel in idle loop."),
    BRUTUS_DECLARE(CONFIG_FILENAME,                  string, "brutus.cfg",       "Filename of Brutus configuration file (contains default settings)"),
    BRUTUS_DECLARE(GRAPHICS_SURFACE_WIDTH,           int,    "960",              "Width of the display's graphics video surface"),
    BRUTUS_DECLARE(GRAPHICS_SURFACE_HEIGHT,          int,    "540",              "Height of the display's graphics video surface"),
    BRUTUS_DECLARE(VIDEO_WINDOW_WIDTH,               int,    "1920",             "Default width of the display's video windows"),
    BRUTUS_DECLARE(VIDEO_WINDOW_HEIGHT,              int,    "1080",             "Default width of the display's video windows"),
    BRUTUS_DECLARE(WINDOW_BACKGROUND_COLOR,          int,    "0x00000000",       "Default windows background color"),
    BRUTUS_DECLARE(SPDIF_OUTPUT_DELAY,               int,    "0",                "Additional delay for SPDIF outputs in millisecs"),
    BRUTUS_DECLARE(HDMI_OUTPUT_DELAY,                int,    "0",                "Additional delay for hdmi audio outputs in millisecs"),
    BRUTUS_DECLARE(DAC_OUTPUT_DELAY,                 int,    "0",                "Additional delay for dac audio outputs in millisecs"),
    BRUTUS_DECLARE(VIDEO_DECODER_FIFO_SIZE,          int,    "0",                "Size of the video decoder FIFO in kilobytes"),
    BRUTUS_DECLARE(AUDIO_DECODER_FIFO_SIZE,          int,    "0",                "Size of the audio decoder FIFO in kilobytes"),
    BRUTUS_DECLARE(TUNE_QAM_TIMEOUT,                 int,    "3000",             "Length of QAM tune lock timeout in millisecs"),
    BRUTUS_DECLARE(TUNE_QAM_SCAN_TIMEOUT,            int,    "500",              "Length of QAM tune lock timeout in millisecs during channel scan"),
    BRUTUS_DECLARE(TUNE_OFDM_TIMEOUT,                int,    "3000",             "Length of OFDM tune lock timeout in millisecs"),
    BRUTUS_DECLARE(TUNE_OFDM_SCAN_TIMEOUT,           int,    "1000",             "Length of OFDM tune lock timeout in millisecs during channel scan"),
    BRUTUS_DECLARE(TUNE_OFDM_PAT_TIMEOUT,            int,    "5000",             "PAT Timeout in Millisecs "),
    BRUTUS_DECLARE(TUNE_OFDM_PMT_TIMEOUT,            int,    "5000",             "PMT Timeout in Millisecs "),
    BRUTUS_DECLARE(TUNE_VSB_TIMEOUT,                 int,    "3000",             "Length of VSB tune lock timeout in millisecs"),
    BRUTUS_DECLARE(TUNE_VSB_SCAN_TIMEOUT,            int,    "500",              "Length of VSB tune lock timeout in millisecs during channel scan"),
    BRUTUS_DECLARE(TUNE_SAT_TIMEOUT,                 int,    "5000",             "Length of SAT tune lock timeout in millisecs"),
    BRUTUS_DECLARE(DEFERRED_CHANNEL_CHANGE_TIMEOUT,  int,    "500",              "Length of deferred channel change timeout for ch up/down in millisecs"),
    BRUTUS_DECLARE(UI_BUFFERS_UPDATE_TIMEOUT,        int,    "333",              "Update period for user interface audio/video buffer updates in millisecs"),
    BRUTUS_DECLARE(UI_GRID_UPDATE_TIMEOUT,           int,    "500",            "Update period for Grid points used in Constellation update in millisecs"),
    BRUTUS_DECLARE(GRID_POINT_MAX,                   int,    "30",               "Maximum number of Grid Points"),
    BRUTUS_DECLARE(TUNER_LOCK_CHECK_TIMEOUT,         int,    "2000",             "Length of time between check for tuner lock in millisecs"),
    BRUTUS_DECLARE(DROP_FIELD_MODE_ENABLED,          bool,   "false",            "If true, when the video decoder needs to drop pictures because of TSM, it will drop only single fields.  Otherwise it will drop field pairs."),
    BRUTUS_DECLARE(AUDIO_DECODER_MULTICH_FORMAT,     int,    "1",                "See NEXUS_AudioMultichannelFormat enum definition for valid values"),
    BRUTUS_DECLARE(AUDIO_CAPTURE,                    bool,   "false",            "Capture audio PCM data"),
    BRUTUS_DECLARE(AUDIO_CAPTURE_COMPRESSED,         bool,   "false",            "Capture compressed audio PCM data"),
    BRUTUS_DECLARE(AUDIO_DOLBY_DRC_AC3_BOOST,        int,    "100",              "Dolby Dynamic Range Control boost value for moderate setting"),
    BRUTUS_DECLARE(AUDIO_DOLBY_DRC_AC3_CUT,          int,    "100",              "Dolby Dynamic Range Control cut value for moderate setting"),
    BRUTUS_DECLARE(MPAA_ENABLED,                     bool,   "false",            "Enable MPAA if available"),
    BRUTUS_DECLARE(USE_FIRST_PTS,                    bool,   "false",            "The STC will be driven by either the video or audio PTS, depending on stream muxing and error conditions."),
    BRUTUS_DECLARE(PRECISION_LIPSYNC_ENABLED,        bool,   "true",             "Enables subframe audio adjustments based on video feedback"),
    BRUTUS_DECLARE(LUA_PROMPT,                       string, "brutus lua> ",     "Prompt string used by Lua scripting engine"),
    BRUTUS_DECLARE(LUA_SCRIPT,                       string, "",                 "Script to run on startup."),
    BRUTUS_DECLARE(LUA_HISTORY_FILENAME,             string, "lua_history.txt",  "Filename used by Lua scripting engine to save command history"),
    BRUTUS_DECLARE(CHANNELS_LIST,                    string, "channels.xml",     "Filename containing the default channel list which is read in on startup"),
    BRUTUS_DECLARE(SCAN_ENCRYPTED_CHANNELS,          bool,   "false",            "Add found encrypted channels to the channel list during scan."),
    BRUTUS_DECLARE(SCAN_AUDIO_ONLY_CHANNELS,         bool,   "false",            "Add found audio only channels to the channel list during scan."),
    BRUTUS_DECLARE(SCAN_DUPLICATE_CHANNELS,          bool,   "false",            "Add found duplicate channels to the channel list during scan."),
    BRUTUS_DECLARE(UI_CHANNEL_NUM_TIMEOUT,           int,    "3000",             "Length of time to display channel number on screen after it is updated in millisecs"),
    BRUTUS_DECLARE(VIDEOS_PATH,                      string, "videos/",          "Path where Videos are stored"),
    BRUTUS_DECLARE(FORCE_BCM_TRICK_MODES,            bool,   "false",            "Force Brutus to use only BRCM trickmodes"),
    BRUTUS_DECLARE(VIDEODECODE_ENABLED,              bool,   "true",             "Force Audio Only trickmodes"),
    BRUTUS_DECLARE(FORCE_HOST_TRICK_MODES,           bool,   "false",            "Force Brutus to use only HOST trickmodes"),
    BRUTUS_DECLARE(ALLOW_ALL_AVC_TRICK_MODES,        bool,   "false",            "Allow all AVC trickmodes in Brutus"),
    BRUTUS_DECLARE(FORCE_IFRAME_TRICK_MODES,         bool,   "false",            "Force Brutus to only do I frame trickmodes"),
    BRUTUS_DECLARE(DQT_ENABLED,                      bool,   "false",            "Enable DQT trickmodes for AVC streams"),
    BRUTUS_DECLARE(DQT_PICS_PER_GOP,                 int,    "10",               "Number of Pics per DQT")

};

#ifndef BRUTUS_DECLARE_CONFIGSETTING_VALUES

    CConfiguration(const char *filename = NULL);
    CConfiguration(const CConfiguration & cfg);

    virtual ~CConfiguration();
    int read(const char *filename);
    void print() const {_pRegistry->print();}
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
    static Predefined _predefines[];
    MStringHash * _pRegistry;
};
#endif

#endif /* BRUTUS_CFG_H__ */
