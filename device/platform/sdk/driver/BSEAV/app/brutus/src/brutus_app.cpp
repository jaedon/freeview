/***************************************************************************
 *     Copyright (c) 2002-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brutus_app.cpp $
 * $brcm_Revision: 41 $
 * $brcm_Date: 2/9/11 12:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/brutus_app.cpp $
 * 
 * 41   2/9/11 12:29p nitinb
 * SW7550-529: Undefined macro B_PAL_SIZED_GRAPHICS for 7550 to correct
 * video window display in Brutus menu screen
 * 
 * 40   11/19/10 4:56p jtna
 * SW7405-4998: even more Brutus polishing
 * 
 * 39   11/16/10 12:16p jtna
 * SW7405-4998: more 3D refactoring
 * 
 * 38   11/15/10 4:20p jtna
 * SW7405-4998: refactor 3DTV code to allow transition in PIG and better
 * content mode handling
 * 
 * 37   11/10/10 5:32p jtna
 * SW7401-4456: update
 * 
 * 36   6/25/10 6:15p jtna
 * SW7405-4420: handle 3dtv source/output permutations better
 * 
 * 35   5/12/10 5:52p jtna
 * SW7405-3999: added 3DTV auto mode
 * 
 * 34   4/22/10 3:52p jtna
 * SW7405-3999: add 3DTV 2D/3D conversion option
 * 
 * 33   4/5/10 11:30a erickson
 * SW7405-3997: set HDMI VSI for 3DTV
 *
 * 32   3/29/10 10:37a erickson
 * SW7405-3625: add 3DTV ui options
 *
 * 31   3/1/10 10:57a erickson
 * SW7550-272: add graphics options for 7550
 *
 * 30   1/22/10 2:46p erickson
 * SW7550-210: refactor BCHP_CHIP lists
 *
 * 29   11/10/09 1:07p nickh
 * SW7420-166:  Add support for 3rd display and DUAL_OUTPUT
 *
 * 28   6/26/09 2:08p erickson
 * PR56373: rework brutus passive standby to not exit the app
 *
 * 27   9/15/08 12:40p tokushig
 * PR43426: merge to main
 *
 * BRUTUS_97405_97043/1   8/29/08 10:48a tokushig
 * PR43596: fix bus error on exit with blue skin.  also fixes intermittent
 * assertion in bwin (during quit)
 *
 * 26   6/9/08 8:59p mphillip
 * PR39626: Allow power button to quit brutus when POWER_STANDBY=y
 *
 * 25   3/17/08 3:17p erickson
 * PR40307: remove old, unused code before adding new
 *
 * 24   3/6/08 11:58a tokushig
 * PR20349: Caution must be taken when using the MHash class because it
 * relies on MAutoList,  Do not add static objects or dynamically
 * allocated objects that are deleted elsewhere, to an MHash.  Fixed
 * cases where this occurs in the skin.  Also searched thorough entire
 * brutus/mlibs source to ensure a similar problem does not exist.
 * MAutoList (and any class that use it) is a very dangerous class to use
 * unless you have full knowledge of it's limitations.  this fix will
 * correct all of the seemingly random segmentation faults that occur
 * when quitting brutus.
 *
 * 23   1/3/08 6:22p erickson
 * PR36068: remove DUAL_CMP_SUPPORT check on script engine
 *
 * 22   11/21/07 4:31p mward
 * PR36867: Delete objects in reverse order of creation.
 *
 * 21   8/20/07 11:34a tokushig
 * PR23101: do not attempt to load skin xml file if option is not
 * specified
 *
 * 20   2/1/07 1:39p rjlewis
 * PR27536: fixed memory leak (can't wipeout only pointer to data).
 *
 * 19   1/31/07 11:47a erickson
 * PR19687: stop all decode and playback on standby
 *
 * 18   1/31/07 11:31a erickson
 * PR19687: add Standby button to brutus. call settop api functions.
 * enable POWER button to enter Standby.
 *
 * 17   9/26/06 11:29a erickson
 * PR24374: convert to DUAL_CMP_SUPPORT
 *
 * 16   6/6/06 11:48a tokushig
 * PR20349: ensure all dynamically allocated bwin framebuffers are
 * destroyed before deleting display
 *
 * SanDiego_Brutus_Skin/7   6/6/06 11:04a tokushig
 * delete skin in brutus destructor
 *
 * 15   5/31/06 4:21p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/6   5/16/06 10:16a tokushig
 * Merge from main
 *
 * 14   5/15/06 6:54p vle
 * PR 21288: initialize scriptEngine to prevent seg. fault upon deletion
 * of scriptEngine in ~Brutus()
 *
 * 12   5/4/06 12:06p jgarrett
 * PR 21266: Merging 3-display code to main.
 *
 * REFSW_97400_3DISPLAY/2   5/3/06 1:37p jgarrett
 * PR 21266: Correcting name for (B) display
 *
 * REFSW_97400_3DISPLAY/1   5/3/06 1:34p jgarrett
 * PR 21266: Adding 3rd display
 *
 * 10   4/17/06 5:08p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/5   4/27/06 10:59a tokushig
 * change brutus blue color surround (around drawable area) to black for
 * skin
 *
 * SanDiego_Brutus_Skin/4   3/13/06 5:22p tokushig
 * moved skin object creation to brutus_app.cpp so that we can ascertain
 * pixel format before creating the control object.
 *
 * SanDiego_Brutus_Skin/3   2/23/06 2:27p tokushig
 * merge from 97400 prerel label
 *
 * 9   2/13/06 11:31a erickson
 * PR17108: refactor Brutus class to make it more sane
 *
 * SanDiego_Brutus_Skin/2   1/18/06 4:06p tokushig
 * set drawable area of framebuffer - set equally left/right and
 * top/bottom
 *
 * SanDiego_Brutus_Skin/1   1/3/06 10:25a tokushig
 * Added XML based skin capability to brutus.  "-skin <name>" command line
 * option
 * allows user to specify skin on startup.  Note that running brutus
 * without the
 * -skin option will use the default GUI.
 *
 * 8   9/8/05 10:00a erickson
 * PR17048: switch individual chip defines to B_HAS_VDC for all 7038 and
 * beyond platforms
 *
 * 7   6/14/05 2:14p erickson
 * PR15216: converted to BKNI_Snprintf
 *
 * 6   4/22/05 2:32p erickson
 * PR14701: test bool, not string, for a couple options
 *
 * 5   3/24/05 5:46p vsilyaev
 * PR 14593: Fixed 3560 configuration.
 *
 * 4   3/24/05 1:21p erickson
 * PR14593: removed old platform
 *
 * 3   2/23/05 9:31a erickson
 * PR14180: fixed for no ADMIN_SUPPORT
 *
 * 2   2/15/05 5:22p erickson
 * PR13387: added primitive palettized color support based on MStyle's
 * palette trick
 *
 * 1   2/7/05 7:55p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/21   10/27/04 3:18p erickson
 * PR13000: modified Run Script ui so that admin button is disabled if
 * there is no ScriptEngine available
 *
 * Irvine_BSEAVSW_Devel/20   9/15/04 2:41p erickson
 * PR9683: use explicity _FLAVOR or _FAMILY instead of BCM7### defines
 *
 * Irvine_BSEAVSW_Devel/19   9/15/04 1:53p erickson
 * PR9683: convert from B_CHIP to BCHP_CHIP in brutus
 *
 * Irvine_BSEAVSW_Devel/18   8/16/04 9:22a erickson
 * PR10628: added more debug
 *
 * Irvine_BSEAVSW_Devel/17   8/13/04 10:08a erickson
 * PR10628: make mapp and fb accesible outside Brutus object
 *
 * Irvine_BSEAVSW_Devel/16   8/3/04 9:56a erickson
 * PR10664: allow single script engine for all DUAL_OUTPUT script usages
 *
 * Irvine_BSEAVSW_Devel/15   7/23/04 2:28p erickson
 * PR11869: set _display to init state
 *
 * Irvine_BSEAVSW_Devel/14   6/11/04 2:43p erickson
 * PR11420: free resources
 *
 * Irvine_BSEAVSW_Devel/13   5/28/04 2:30p erickson
 * PR11080: set mainsurface alpha after the bgraphics_sync call so that we
 * don't have a long time interval which appears as a glitch in the UI
 *
 * Irvine_BSEAVSW_Devel/12   5/4/04 10:15a erickson
 * PR10927: using bconfig->resource for brutus configuration
 *
 * Irvine_BSEAVSW_Devel/11   4/30/04 12:06p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/10   4/26/04 5:05p erickson
 * PR10815: apply -scr and -scrtext to first app only
 *
 * Irvine_BSEAVSW_Devel/9   4/22/04 10:35p erickson
 * PR10668: implement 97038 HD/SD simul, dual(triple) record and other
 * modes
 *
 * Irvine_BSEAVSW_Devel/8   4/21/04 1:45p erickson
 * PR10668: reworked display config for brutus pip/dual configurations
 *
 * Irvine_BSEAVSW_Devel/7   4/20/04 3:04p erickson
 * PR10668: enabled hd/sd simul for 97038
 *
 * Irvine_BSEAVSW_Devel/6   4/16/04 2:49p erickson
 * PR10664: added app(#) command to switch between dual output instances
 * on 97038
 *
 * Irvine_BSEAVSW_Devel/5   4/9/04 2:17p erickson
 * PR10505: make sure rf mod is 2nd output as well
 *
 * Irvine_BSEAVSW_Devel/4   3/22/04 12:49p erickson
 * PR10143: app0 should always have component, app1 should always have
 * svideo/composite
 *
 * Irvine_BSEAVSW_Devel/3   3/18/04 3:36p erickson
 * PR10129: support SCRIPT_SUPPORT=n
 *
 * Irvine_BSEAVSW_Devel/2   3/9/04 12:21p erickson
 * PR9883: don't touch default svideo/composite
 *
 * Irvine_BSEAVSW_Devel/1   3/9/04 10:34a erickson
 * PR9883: reconfigured Brutus app bringup to support DUAL_OUTPUT mode on
 * 7038
 *
 ******************************************************************/
#include "brutus_app.h"
#include "control.h"
#include "inputengine.h"
#ifdef SCRIPT_SUPPORT
#include "scriptengine.h"
#include "runscriptscreen.h"
#ifdef ADMIN_SUPPORT
#include "adminscreen.h"
#endif
#endif
#include <bstd.h>
BDBG_MODULE(brutus_app);

static bgraphics_pixel_format parse_pixel_format(const char *format);

/**
Start an instance of brutus
**/
Brutus::Brutus(ChannelManager *channelManager, RecordManager *recordManager,
    AVManager *avManager, Config *cfg)
{
    int chromakeyColor;

    scriptEngine = NULL;
    _control = NULL;
    _valid = false;
    memset(&_display, 0, sizeof(_display));
    _displayindex = cfg->getInt(Config::SECOND_DISPLAY);
    #if B_TRIPLE_VEC_SYSTEM
    if (_displayindex) _displayindex=2;
    #endif

    if (cfg->getBool(Config::DUAL_OUTPUT)) {
        char buf[256];
        BKNI_Snprintf(buf, 256, "%s (%c)", cfg->get(Config::HOME_TITLE), _displayindex?'B':'A');
        cfg->set(Config::HOME_TITLE, buf);
    }


    /* skin name can be set in config file or overridden by */
    /* using -skin option when running brutus               */
    MString strSkinPath = cfg->get(Config::SKINS_PATH);
    strSkinPath += "/";
    strSkinPath += cfg->get(Config::SKIN);
    strSkinPath += "/";
    strSkinPath += cfg->get(Config::SKIN_FILE);

    _skin = new Skin(NULL);
    if (!_skin)
    {
        BDBG_ERR(("Cannot construct new Skin"));
        return;
    }

    if (MString(cfg->get(Config::SKIN)).length())
    {
        BDBG_MSG(("Using skin file: '%s'\n", strSkinPath.s()));
        _skin->parse(strSkinPath.s());
    }

    /**
    Determine the configuration based on Config options.
    **/
    _displayConfig.pixel_format = parse_pixel_format(cfg->get(Config::PIXEL_FORMAT));

    if (_skin->isValid())
    {
        /* override config specified pixel format if exists in skin */
        MString strFormat;
        _skin->getSkinAttrValue(XML_ATT_PIXEL_FORMAT, strFormat);
        if (!strFormat.isEmpty())
        {
            _displayConfig.pixel_format = parse_pixel_format(strFormat.s());
            BDBG_MSG(("skin specified pixel format: %s\n", strFormat.s()));
        }
    }

    switch (_displayConfig.pixel_format) {
    case bgraphics_pixel_format_palette8:
    case bgraphics_pixel_format_a8_r8_g8_b8:
    case bgraphics_pixel_format_a1_r5_g5_b5:
        /* shut off chroma key color for alpha per pixel */
        chromakeyColor = 0x00000000;
        break;
    case bgraphics_pixel_format_r5_g6_b5:
    default:
        chromakeyColor = 0xff000000 | cfg->getInt(Config::CHROMAKEY_COLOR);
        break;
    }
    /* fix chroma key for pixel format */
    cfg->set(Config::CHROMAKEY_COLOR, chromakeyColor);

    _displayConfig.backgroundColor = cfg->getInt(Config::BACKGROUND_COLOR);
    _displayConfig.chromakeyColor = cfg->getInt(Config::CHROMAKEY_COLOR);
    _displayConfig.width = 720;
    _displayConfig.height = 480;

    /* this is the drawable area inside of the surface which will be the framebuffer */
    _displayConfig.view_rect.x = cfg->getInt(Config::SCREEN_X);
    _displayConfig.view_rect.y = cfg->getInt(Config::SCREEN_Y);
    _displayConfig.view_rect.width = 665;
    _displayConfig.view_rect.height = 440;

    /**
    Open the bsettop and bwin handles based on the config information.
    **/
    BDBG_MSG(("Creating brutus display"));
    if (openDisplay(cfg)) {
        BDBG_ERR(("openDisplay failed"));
        return;
    }

    /**
    Now bring up the application
    **/
    BDBG_MSG(("Creating mlibs"));
    MApplication::Settings mapp_settings;
    MApplication::getDefaultSettings(&mapp_settings);
    mapp_settings.win_engine = _display.win_engine;
    _mapp = new MApplication(&mapp_settings, MString(_displayindex));
    if (!_mapp->isValid()) {
        BDBG_ERR(("Cannot construct new MApplication"));
        return;
    }

    MFrameBuffer::Settings fb_settings;
    MFrameBuffer::getDefaultSettings(&fb_settings);
    fb_settings.framebuffer = _display.framebuffer;
    _fb = new MFrameBuffer(_mapp, &fb_settings);

    _mapp->setDefaultFont(MFont(_mapp, cfg->get(Config::NORMAL_FONT)));
    setColors(cfg);

    /* load these colors back, in case we're converted to a palette */

    if (_skin->isValid())
    {
        /* override config specified main window border color */
        _displayConfig.backgroundColor = _skin->getBorderColor();
        BDBG_MSG(("skin specified main window border color: 0x%x\n", _displayConfig.backgroundColor));
    }
    else
    {
        _displayConfig.backgroundColor = _mapp->style()->color(MStyle::BACKGROUND);
    }

    _displayConfig.chromakeyColor = _mapp->style()->color(MStyle::TRANSPARENT);

    // start brutus
    BDBG_MSG(("Creating Control"));
    _control = new Control(_fb, this, channelManager, recordManager, avManager, cfg);
    if (!_control->valid()) {
        BDBG_ERR(("Control cannot be constructed"));
        return;
    }

    BDBG_MSG(("Creating input"));
    _inputEngine = new InputEngine(_control);
    _mapp->setGlobalKeyProcessor(_inputEngine);

#if defined(SCRIPT_SUPPORT)
    /* DUAL_OUTPUT runs with a single ScriptEngine which is created outside of this,
    which means that the RunScriptScreen has no ScriptEngine. */
    if (!cfg->getBool(Config::DUAL_OUTPUT))
    {
        bool enableScriptScreen = true;
        scriptEngine = new ScriptEngine(_inputEngine);

        if (!cfg->getInt(Config::SECOND_DISPLAY))
        {
            if (cfg->getBool(Config::INTERACTIVE_MODE)) {
                scriptEngine->runInteractiveMode();
                enableScriptScreen = false; /* can't use scripts screen in interactive mode */
            }
            else if (cfg->get(Config::INITIAL_SCRIPT))
                scriptEngine->runScript(cfg->get(Config::INITIAL_SCRIPT));
            else if (cfg->get(Config::INITIAL_SCRIPTTEXT))
                scriptEngine->runScriptText(cfg->get(Config::INITIAL_SCRIPTTEXT));
        }

        /* Otherwise we can start up a script using the Run Script screen */
        if (enableScriptScreen) {
            RunScriptScreen *scr = (RunScriptScreen *)_control->screen(Control::eRunScript);
            if (scr) {
                scr->setScriptEngine(scriptEngine);
#ifdef ADMIN_SUPPORT
                ((AdminScreen*)_control->screen(Control::eAdmin))->enableRunScript();
#endif
            }
        }
    }
#endif

    _valid = true;
}

Brutus::~Brutus()
{
#ifdef SCRIPT_SUPPORT
    if (scriptEngine)
        delete scriptEngine;
#endif
    delete _inputEngine;
    delete _control;
    delete _fb;
    delete _mapp;

    /* although _skin is created before the display is opened, it must be destroyed
       first because it may have created mpixmaps - which internally create bwin
       framebuffers.  these additional framebuffers will be tied to the display's
       winengine...and must be closed first. */
    delete _skin;

    closeDisplay();
}

int Brutus::start()
{
    if (!valid())
        return -1;
    else
        return _mapp->run();
}

/**
Apply the Config color settings to mwidgets' MStyle.
**/
void Brutus::setColors(Config *cfg)
{
#define setStyle(STYLE, CONFIG) \
do {if (cfg->get(CONFIG)) _mapp->style()->set(STYLE, cfg->getInt(CONFIG));} while(0)

    setStyle(MStyle::BUTTON_FACE, Config::BACKGROUND_COLOR);
    setStyle(MStyle::BACKGROUND, Config::BACKGROUND_COLOR);
    setStyle(MStyle::TEXT, Config::TEXT_COLOR);
    setStyle(MStyle::FOCUS, Config::FOCUS_COLOR);
    setStyle(MStyle::BUTTON_FACE_FOCUSED, Config::BUTTON_FACE_FOCUSED_COLOR);
    setStyle(MStyle::TEXT_FOCUSED, Config::TEXT_FOCUSED_COLOR);
    setStyle(MStyle::TRANSPARENT, Config::CHROMAKEY_COLOR);

/* TODO: complete this list
    // Other MStyle colors:
    BUTTON_LEFTBEVEL,
    BUTTON_TOPBEVEL,
    BUTTON_RIGHTBEVEL,
    BUTTON_BOTTOMBEVEL,
    MESSAGEBOX_BACKGROUND,
    MESSAGEBOX_BORDER,
    MESSAGEBOX_TEXT,
    LABEL_DROPSHADOW,
    LINEEDIT_FOCUSBORDER,
    CHECKBOX_CHECK,
    LINEEDIT_TEXT,
*/

    if (_displayConfig.pixel_format == bgraphics_pixel_format_palette8) {
        unsigned long *palette;
        int length;
        bgraphics_palette gp;

        /* This causes mlibs to convert to palettized color and return
        an array of ARGB8888 values which should be used as the palette */
        _mapp->style()->convertToPalette(&palette, &length);

        /* apply it to Settop API */
        gp.palette = (bgraphics_pixel*)palette;
        gp.length = length;
        bsurface_load_clut(_display.mainsurface, gp);
        bsurface_load_clut(_display.actual_framebuffer, gp);

        /* copy was made, so free it */
        _mapp->style()->freePalette(palette);
    }
}

static bgraphics_pixel_format parse_pixel_format(const char *format)
{
    if (format) {
        if (!strcasecmp(format, "argb8888"))
            return bgraphics_pixel_format_a8_r8_g8_b8;
        else if (!strcasecmp(format, "argb1555"))
            return bgraphics_pixel_format_a1_r5_g5_b5;
        else if (!strcasecmp(format, "palette8"))
            return bgraphics_pixel_format_palette8;
    }
    return bgraphics_pixel_format_r5_g6_b5;
}

bool Brutus::powerDown()
{
    bsettop_power_state state;
    bsettop_get_power_state(&state);
    if (state.av_outputs == true) {
        state.av_outputs = false;
        bsettop_set_power_state(&state);

        _control->stopAllAudioVideo();
        return true;
    }
    return false;
}

bool Brutus::powerUp()
{
    bsettop_power_state state;
    bsettop_get_power_state(&state);
    if (state.av_outputs == false) {
        state.av_outputs = true;
        bsettop_set_power_state(&state);

        _control->startLiveDecode();
        return true;
    }
    return false;
}

void Brutus::set3DTVZnorm(int znorm)
{
    _display.s3dTv.znorm = znorm;
    _control->screen(_control->currentScreen())->repaint();
}

void Brutus::set3DTVAutoMode(bool enabled)
{
    _display.s3dTv.automode = enabled;
}

void Brutus::set3DTVOutput(Brutus3DTVMode mode)
{
    _display.s3dTv.output = mode;

    /* this overrides the .enabled if the user specifies so */
    if (_control->cfg()->getBool(Config::FORCE_3D_OUTPUT_FOR_INCOMPATIBLE_TV)==false) {
        if (_control->hdmiTVHas3DSupport()==false) {
            if (mode > Brutus3DTVMode_eOff) {
                BDBG_WRN(("Overriding 3DTV mode to off"));
                _display.s3dTv.output = Brutus3DTVMode_eOff;
                if (!_display.s3dTv.forceOutputChange) {
                    return;
                }
                else {
                    _display.s3dTv.forceOutputChange = false;
                    /* go ahead and call showWindow, to handle situations like 2D->3D conversion */
                }
            }
        }
    }

    /* reconfigure the windows and displays */
    _control->showWindow(0);

    /* HDMI output information */
    _control->hdmiSetVsi();
}

void Brutus::set3DTVSource(Brutus3DTVMode mode)
{
    _display.s3dTv.source = mode;
}

void Brutus::set3DTVMessage(Brutus3DTVMode mode)
{
    _display.s3dTv.message = mode;
}
