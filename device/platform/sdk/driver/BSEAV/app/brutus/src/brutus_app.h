/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brutus_app.h $
 * $brcm_Revision: 31 $
 * $brcm_Date: 3/20/12 10:33a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/brutus_app.h $
 * 
 * 31   3/20/12 10:33a erickson
 * SW7425-2664: remove BCHP_CHIP lists
 * 
 * 30   2/2/12 5:25p katrep
 * SW7429-1:enabled sharpness for 7429
 * 
 * 29   12/21/11 3:52p mward
 * SW7435-7:  Support Brutus build on (new, 7435-based) platform 97435.
 * 
 * 28   11/16/11 9:41a erickson
 * SW7425-1747: remove old code
 * 
 * 27   5/23/11 12:57p katrep
 * SW7231-69:vertical scaling done through scaler not throguh memc for
 * 7231
 * 
 * 26   3/11/11 9:29a katrep
 * SW7231-69:disable vertical scalar
 * 
 * 25   3/9/11 3:37p katrep
 * SW7231-47:enable verticle scalar,PEP for 7231
 * 
 * 24   2/15/11 11:53a xhuang
 * SW7358-34: Add 7358/7552 support
 * 
 * 23   2/9/11 12:29p nitinb
 * SW7550-529: Undefined macro B_PAL_SIZED_GRAPHICS for 7550 to correct
 * video window display in Brutus menu screen
 * 
 * 22   12/28/10 3:15p hongtaoz
 * SW7425-49: add 7425 support;
 * 
 * 21   12/15/10 10:37a jrubio
 * SW7344-9: add 7344/7346
 * 
 * 20   11/19/10 4:57p jtna
 * SW7405-4998: even more Brutus polishing
 * 
 * 19   11/16/10 12:17p jtna
 * SW7405-4998: more 3D refactoring
 * 
 * 18   11/4/10 2:29p hongtaoz
 * SW7425-49: added 7425 support;
 * 
 * 17   10/21/10 4:52p nickh
 * SW7422-80: Add 7422 support for triple display and PEP support
 * 
 * 16   6/25/10 6:15p jtna
 * SW7405-4420: handle 3dtv source/output permutations better
 * 
 * 15   5/12/10 5:53p jtna
 * SW7405-3999: added 3DTV auto mode
 * 
 * 14   4/22/10 3:52p jtna
 * SW7405-3999: add 3DTV 2D/3D conversion option
 * 
 * 13   3/29/10 10:37a erickson
 * SW7405-3625: add 3DTV ui options
 *
 * 12   3/26/10 5:20p erickson
 * SW7405-3625: move BCHP_CHIP options from info_video.cpp
 *
 * 11   3/1/10 10:57a erickson
 * SW7550-272: add graphics options for 7550
 *
 * 10   1/22/10 2:46p erickson
 * SW7550-210: refactor BCHP_CHIP lists
 *
 * 9   10/20/09 1:47p erickson
 * SW7405-3106: add support for negative znorm values
 *
 * 8   10/1/09 12:47p erickson
 * SW7405-3106: add OUTPUT_3D_ZNORM. renamed OUTPUT_3D_MODE values to sbs
 * and ou.
 *
 * 7   9/30/09 10:38a erickson
 * SW7405-3106: add OUTPUT_3D_MODE
 *
 * 6   3/13/07 9:54a erickson
 * PR28644: use opaque black pixel in pictures screen full-screen mode
 * border. don't rely on alpha 0 and no video.
 *
 * 5   1/31/07 11:31a erickson
 * PR19687: add Standby button to brutus. call settop api functions.
 * enable POWER button to enter Standby.
 *
 * 4   5/31/06 4:20p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/3   5/16/06 10:16a tokushig
 * Merge from main
 *
 * SanDiego_Brutus_Skin/2   5/3/06 12:32p tokushig
 * added support for hardware acceleration of offscreen buffers in bwin
 *
 * 3   4/17/06 5:09p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/1   3/13/06 5:24p tokushig
 * moved skin object creation to brutus_app.cpp so that we can ascertain
 * pixel format before creating the control object.
 *
 * 2   2/13/06 11:31a erickson
 * PR17108: refactor Brutus class to make it more sane
 *
 * 1   2/7/05 7:55p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/5   8/13/04 10:08a erickson
 * PR10628: make mapp and fb accesible outside Brutus object
 *
 * Irvine_BSEAVSW_Devel/4   5/28/04 2:30p erickson
 * PR11080: set mainsurface alpha after the bgraphics_sync call so that we
 * don't have a long time interval which appears as a glitch in the UI
 *
 * Irvine_BSEAVSW_Devel/3   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/2   4/16/04 2:49p erickson
 * PR10664: added app(#) command to switch between dual output instances
 * on 97038
 *
 * Irvine_BSEAVSW_Devel/1   3/9/04 10:34a erickson
 * PR9883: reconfigured Brutus app bringup to support DUAL_OUTPUT mode on
 * 7038
 *
 ******************************************************************/
#ifndef BRUTUS_APP_H__
#define BRUTUS_APP_H__

#include "bsettop.h"
#include "bwin.h"
#include "brutus_config.h"
#include "cfgsettings.h"
#include "mapplication.h"
#include "avmanager.h"
#include "channelmgr.h"
#include "recordengine.h"
#include "nexus_platform_features.h"

class InputEngine;
class ScriptEngine;
class Control;
class Brutus;
class Skin;

/* In lieu of brutus_frontend.h */
extern ChannelManager *createChannelManager(const Config *cfg);
extern void destroyChannelManager(ChannelManager *channelManager);

/* In lieu of brutus_avmanager.h */
extern RecordManager *createRecordManager(int records, const Config *cfg);
extern void destroyRecordManager(RecordManager *recordManager);

/**
Data structure used for passing draw data to bwin hardware optimized drawops
**/
typedef struct
{
    Brutus     * pBrutus;
    bsurface_t   surface;
} brutus_fb_data;

/**
Inputs for creating a display
**/
typedef struct {
    unsigned width, height; /* Size of brutus framebuffer. Depending on the system, this may not be the actual framebuffer.  */
    bsettop_rect view_rect; /* Area within framebuffer where app actually draws. This corresponds roughly to the viewable area on
        a TV. The space outside of the viewable area is handled with four "borders". They are drawn separately. */

    uint32_t chromakeyColor; /* ARGB8888 */
    uint32_t backgroundColor; /* ARGB8888 */
    bgraphics_pixel_format pixel_format; /* Pixel format of the framebuffer and drawing surfaces. */
} BrutusDisplayConfiguration;

/* 3D output formats */
typedef enum Brutus3DTVMode {
    Brutus3DTVMode_eOff,
    Brutus3DTVMode_eSBS,
    Brutus3DTVMode_eOU
} Brutus3DTVMode;

/**
Collection of handles which represents the Brutus UI
**/
typedef struct {
    bgraphics_t graphics;
    bgraphics_t graphics2; /* Needed for HDSD_SINGLE only. */
    bdisplay_t display;
    bdisplay_t display2;    /* If need_second_display was true, this contains the handle
                                for the second display. */
    bool spdif_pcm;         /* If true, then we should only put out PCM on spdif.
                                If false, allow compressed, but only for typically supported formats.
                                MPEG audio should never be output compressed. Settop API allows it. */
    bsurface_t mainsurface; /* The surface into which brutus will draw */
    bsurface_t actual_framebuffer;  /* The surface that is visible. On some platforms
                                this is the same as mainsurface. */
    bsurface_t actual_framebuffer2; /* Needed for HDSD_SINGLE only. */

    bwin_engine_t win_engine; /* Bwin engine for this display. */
    bwin_framebuffer_t framebuffer; /* Bwin framebuffer for this display. */

    bsettop_rect view_rect; /* area within framebuffer where app actually draws */

    struct {
        Brutus3DTVMode output; /* current output */
        bool forceOutputChange;
        Brutus3DTVMode cfg;    /* output originally requested in brutus.cfg */
        Brutus3DTVMode source;
        Brutus3DTVMode message;
        bool automode;
        int znorm;
    } s3dTv;
} BrutusDisplay;

/**
Summary:
Brutus application is an Broadcom set-top box demo application.

Description:
Each instance of Brutus is an end-user application. For a dual output system, you'll
need to instantiate two instances of Brutus, either in the same app or different apps
(depending on the architecture).
**/
class Brutus {
    friend class Control;
public:
    /**
    Summary:
    Construct a new instance of the Brutus application.

    Description:
    The ChannelManager and AVManager control global resources and must be passed
    into Brutus.
    **/
    Brutus(ChannelManager *channelManager, RecordManager *recordManager,
        AVManager *avManager, Config *cfg);

    /**
    Summary:
    Destroy the Brutus application.
    **/
    virtual ~Brutus();

    /**
    Summary:
    Test if Brutus was constructed successfully. If not, then don't start().
    **/
    bool valid() const {return _valid;}

    /**
    Summary:
    Start the execution of the Brutus application.

    Description:
    This will return when the application
    has been exited using the "quit" script command or another GUI control.
    **/
    int start();

    int displayindex() const {return _displayindex;}
    Control *control() {return _control;}
    BrutusDisplay *display() {return &_display;}
    InputEngine *inputEngine() const {return _inputEngine;}
    MFrameBuffer *fb() const {return _fb;}
    Skin *skin() const {return _skin;}

    /**
    Summary:
    Set graphics border depending on Brutus mode. If fullscreen graphics, there is a border
    which is the same color as the Brutus background. If not fullscreen graphics, the border
    is the chromakey color.
    **/
    void setFullscreenGraphics(bool enabled);

    /**
    Set the border to be a specific color, overriding any defaults.
    */
    void setFullscreenGraphics(uint32_t color);

    /**
    3DTV functions
    */  
    void set3DTVAutoMode(bool enabled); /* auto mode is either on or off */
    void set3DTVOutput(Brutus3DTVMode mode); /* display output mode */
    void set3DTVSource(Brutus3DTVMode mode); /* source type */
    void set3DTVMessage(Brutus3DTVMode mode); /* stream in-band 3D orientation info, if any. note that there can be a mismatch between Source and Message */
    void set3DTVZnorm(int znorm);

    /**
    Summary:
    Write the contents of the mainsurface to a binary file.
    **/
    int screenCapture(const char *filename);

    /**
    Summary:
    On some platforms, we have to reset the graphics in order to scale properly.
    **/
    int resetGraphics();

    /* returns true if it powered down */
    bool powerDown();

    /* returns true if it powered up */
    bool powerUp();

protected:
    bool _valid;

    /* Backend */
    AVManager *avManager;
    Control *_control;
    int _displayindex;
    BrutusDisplay _display;
    BrutusDisplayConfiguration _displayConfig;
    MApplication *_mapp;
    MFrameBuffer *_fb;
    brutus_fb_data fb_data;
    Skin *_skin;

    /* Frontend */
    ChannelManager *channelManager;

    /* Input */
    InputEngine *_inputEngine;
    ScriptEngine *scriptEngine;

    int openDisplay(Config *cfg);
    int closeDisplay();
    void getBorderRects(bwin_rect borderRect[]);
    void setColors(Config *cfg);
};

/**
Place for brutus-wide BCHP_CHIP lists
**/
#if BCHP_CHIP == 7550
/* Defaulting to 32 bpp increases MEMC RTS stress */
#define B_DEFAULT_TO_ARGB8888_PIXEL 1
/* Some chips don't have RGB support in GFD1 */
#define B_GFD_1_YCRCB_ONLY 1
#endif
#if BCHP_CHIP==7400 || BCHP_CHIP==7420
/* A triple VEC system can do DUAL_OUTPUT and HDSD_SINGLE at the same time */
#define B_TRIPLE_VEC_SYSTEM 1
#endif

#if BCHP_CHIP==7422 || BCHP_CHIP==7425 || BCHP_CHIP == 7435
/* A triple display system can do DUAL_OUTPUT and HDSD_SINGLE at the same time */
#define B_TRIPLE_DISPLAY_DUAL_VEC_SYSTEM 1
#endif

#define B_HAS_GFD_VERTICAL_UPSCALER NEXUS_HAS_GFD_VERTICAL_UPSCALE

#if NEXUS_NUM_DISPLAYS > 1
#define DUAL_CMP_SUPPORT 1
#endif

#endif //BRUTUS_APP_H__
