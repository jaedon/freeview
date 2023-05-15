/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: tvscreen.cpp $
 * $brcm_Revision: 25 $
 * $brcm_Date: 10/27/08 4:12p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/tvscreen.cpp $
 * 
 * 25   10/27/08 4:12p erickson
 * PR48439: added STREAM_INFO_DISABLED
 *
 * 24   7/10/08 11:38p ssood
 * PR42739: Cleaned up logic on accessing playengines for IP playback
 *
 * 23   5/6/08 4:46p mward
 * PR40307: With DUAL_OUTPUT=1, decode[1] may come up before decode[0].
 *
 * 23   5/6/08 4:02p mward
 * PR40307: With DUAL_OUTPUT=1, decode[1] may come up before decode[0].
 *
 * 23   5/6/08 2:36p mward
 * PR40307: With DUAL_OUTPUT=1, decode[1] may come up before decode[0].
 *
 * 23   5/6/08 12:01p mward
 * PR40307: With DUAL_OUTPUT=1, decode[1] may come up before decode[0].
 *
 * 23   5/6/08 11:51a mward
 * PR40307: With DUAL_OUTPUT=1, decode[1] may come up before decode[0].
 *
 * 23   5/6/08 11:49a mward
 * PR40307: With DUAL_OUTPUT=1, _decode[1] may come up before _decode[0].
 *
 * 23   5/6/08 11:48a mward
 * PR40307: With DUAL_OUTPUT=1, _decode[1] may come up before _decode[0].
 *
 * 22   3/20/08 11:55a erickson
 * PR40307: fix pip show
 *
 * 21   3/20/08 1:49a erickson
 * PR40307: update
 *
 * 20   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 *
 * 19   3/17/08 3:17p erickson
 * PR40307: remove old, unused code before adding new
 *
 * 18   2/19/08 11:15a yrajakar
 * PR36762: Update status with first_pts, and duration
 *
 * 17   12/6/07 12:53p erickson
 * PR37967: rename functions for consistency
 *
 * 16   12/5/07 4:10p erickson
 * PR37899: remove hide() call in setPlayEngines. it was causing eratic
 * timeline behavior.
 *
 * 15   9/11/07 2:51p jrubio
 * PR34614: adding PVR timeline to HTTP channels
 *
 * 14   4/19/07 6:19p jrubio
 * PR30010: put in code to be used later for IP ticker bar
 *
 * 13   11/6/06 12:15p arbisman
 * PR25559: PlayEngine for PVR timeline will be set when playback starts
 *
 * 12   10/19/06 3:18p erickson
 * PR24374: hide record icon with PVR_TIMELINE_ENABLED
 *
 * 11   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/4   5/16/06 11:33a tokushig
 * merge from main
 *
 * 9   4/17/06 5:06p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/3   4/14/06 8:02p tokushig
 * moved info screen location to avoid overscan cutoff
 *
 * SanDiego_Brutus_Skin/2   2/17/06 12:03p tokushig
 * partial skinning of pvrbar - can set color
 *
 * SanDiego_Brutus_Skin/1   2/13/06 4:31p tokushig
 * add full screen transparent label to tvscreen - this will force
 * previous window to be cleared from screen when using skins
 *
 * 8   1/20/06 2:32p mphillip
 * PR16055: Add SIDEBYSIDE_ENABLED (and PVR_TIMELINE_ENABLED) support in
 * brutus.cfg
 *
 * 7   1/13/06 1:53p mphillip
 * PR19051: Replaced a pip frame resize call from prior to the sbs/wow
 * rework of pip.
 *
 * 6   12/20/05 5:28p mphillip
 * PR16055: Initial rework of the PIP system to include a generic Window
 * on Window architecture
 *
 * 5   10/10/05 3:27p erickson
 * PR17108: switch to more generic B_HAS_VDC
 *
 * 4   3/24/05 1:21p erickson
 * PR14593: removed old platform
 *
 * 3   2/17/05 1:47p erickson
 * PR9497: fixed header file for no-pvr
 *
 * 2   2/15/05 5:22p erickson
 * PR13387: added primitive palettized color support based on MStyle's
 * palette trick
 *
 * 1   2/7/05 8:18p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/54   1/13/05 5:22p erickson
 * PR12290: readded legacy pip window size
 *
 * Irvine_BSEAVSW_Devel/53   12/20/04 10:29a erickson
 * PR12290: modified pip size for 480i so that graphics are the same for
 * all outputs
 *
 * Irvine_BSEAVSW_Devel/52   12/17/04 9:12a erickson
 * PR12388: added comment
 *
 * Irvine_BSEAVSW_Devel/51   12/3/04 3:19p erickson
 * PR13256: moved pip gfx size logic to tvscreen
 *
 * Irvine_BSEAVSW_Devel/50   11/12/04 12:17p erickson
 * PR13189: don't call focusUpperLeft on tvscreen, otherwise it grabs a
 * wrong part of the info panel
 *
 * Irvine_BSEAVSW_Devel/49   10/15/04 10:51a erickson
 * PR13014: infobar rework
 *
 * Irvine_BSEAVSW_Devel/48   9/16/04 3:38p erickson
 * PR11081: more work on info panel display
 *
 * Irvine_BSEAVSW_Devel/47   9/13/04 1:55p erickson
 * PR11081: info panel is now tightly integrated with TVMessage
 *
 * Irvine_BSEAVSW_Devel/46   8/17/04 4:41p erickson
 * PR11770: removed brutus inputband support. added streamer as a
 * registered frontend resource, with a mapping between index number and
 * settop api id.
 *
 * Irvine_BSEAVSW_Devel/45   8/9/04 3:47p erickson
 * PR12104: added streamer mpeg_type info
 *
 * Irvine_BSEAVSW_Devel/44   8/3/04 3:13p erickson
 * PR11770: Report when band is available for tuner
 *
 * Irvine_BSEAVSW_Devel/43   8/3/04 9:30a erickson
 * PR11682: added PVR ifdef
 *
 * Irvine_BSEAVSW_Devel/42   7/28/04 10:38a erickson
 * PR11682: added (PIP) message
 *
 * Irvine_BSEAVSW_Devel/41   7/21/04 4:14p erickson
 * PR11682: improved brutus status msg
 *
 * Irvine_BSEAVSW_Devel/40   7/20/04 10:38a erickson
 * PR11682: updated tvscreen ui
 *
 * Irvine_BSEAVSW_Devel/39   7/14/04 3:15p erickson
 * PR11682: removed old pod demo code
 *
 * Irvine_BSEAVSW_Devel/38   5/4/04 10:12a erickson
 * PR10782: more space given
 *
 * Irvine_BSEAVSW_Devel/37   4/30/04 12:05p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/36   4/19/04 2:02p erickson
 * PR10471: made all filepaths configurable using brutus.cfg
 *
 * Irvine_BSEAVSW_Devel/35   4/16/04 10:44a erickson
 * PR9029: new object-oriented input device design
 *
 * Irvine_BSEAVSW_Devel/34   4/12/04 10:46a erickson
 * PR10368: check if pipoutline is initialized before changing zorder
 *
 * Irvine_BSEAVSW_Devel/33   4/6/04 3:48p bandrews
 * PR10368: Added goto(info) command to reset infobar plugin focus to the
 * top.  goto(tv) will now turn off info as well.
 *
 * Irvine_BSEAVSW_Devel/32   4/2/04 5:39p bandrews
 * PR10031: Fixed to use timeline bgcolor and make the recordindicator
 * look like it's part of the timeline while playback is enabled.
 *
 * Irvine_BSEAVSW_Devel/31   2/19/04 4:01p erickson
 * PR9684: removed unneeded key processing
 *
 * Irvine_BSEAVSW_Devel/30   2/19/04 2:03p bandrews
 * PR9723: Added opaque alpha to colors without it.
 *
 * Irvine_BSEAVSW_Devel/29   2/11/04 10:54a erickson
 * PR9715: display() now returns BrutusDisplay* which gives more access
 *
 * Irvine_BSEAVSW_Devel/28   2/4/04 2:12p erickson
 * PR9107: tvscreen no longer needs to access the InputEngine
 *
 * Irvine_BSEAVSW_Devel/27   2/3/04 10:13a erickson
 * PR9497: need impl for TVScreen::focus even if no admin support
 *
 * Irvine_BSEAVSW_Devel/26   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/25   12/23/03 12:56a erickson
 * PR8731: PodInfoBar is a runtime InfoBar replacement
 *
 * Irvine_BSEAVSW_Devel/24   12/19/03 10:05a erickson
 * PR8731: For POD demo, we undef PVR_SUPPORT only for the UI. Also, I
 * removed the "NEW_UI" code.
 *
 * Irvine_BSEAVSW_Devel/23   12/16/03 12:25a erickson
 * PR8731: POD ui work
 *
 * Irvine_BSEAVSW_Devel/22   12/11/03 12:04a erickson
 * PR8813: now that the info bar creates widgets which aren't direct
 * children, we need to repaint the entire
 *
 * Irvine_BSEAVSW_Devel/21   12/10/03 1:35p erickson
 * PR8813: added POD plugin and resized bar for new ui
 *
 ****************************************************************************/
#include "control.h"
#include "cfgsettings.h"
#include "channelmgr.h"
#include "inputengine.h"
#include <mpainter.h>
#include <mapplication.h>
#include "tvscreen.h"

#ifdef NO_PVR_GRAPHICS
#undef PVR_SUPPORT
#endif

#ifdef PVR_SUPPORT
#include "brutus_pvr_bar.h"
#include "recstateicon.h"
#endif
#ifdef INFOPANEL_SUPPORT
#include "infobar.h"
#endif
#include "bstd.h"

BDBG_MODULE(brutus_tvscreen);

///////////////////////////////////////////////

struct TVScreenData {
    WindowOnWindow *_wow;
#ifdef INFOPANEL_SUPPORT
    InfoBar *_infoBar;
#endif
#ifdef PVR_SUPPORT
    RecordIndicator *_recordIndicator;
    BrutusPvrBar *_timeline[2];
#endif
};

TVScreen::TVScreen(Control *acontrol) :
    GenericScreen(acontrol, "tvscreen")
{
    _private = new TVScreenData;

    Skin * pSkin = control()->getSkin();
    if (pSkin->isValid(XML_SCREEN_TV))
    {
        pSkin->createWidgets(XML_SCREEN_TV, this);
        //return;  do not return since there are required widgets below

        MString strPvrbarColor;

        pSkin->getAttrValue(XML_SCREEN_TV,
                            XML_ATT_PVRBAR_COLOR,            strPvrbarColor);

        //set pvrbar color if necessary
        if (!strPvrbarColor.isEmpty())
        {
            cfg()->set(Config::PVRBAR_OUTLINE_COLOR,
                       (int)pSkin->getColorValue(strPvrbarColor.s()));
        }
    }

#ifdef PVR_SUPPORT
    _private->_timeline[0] = _private->_timeline[1] = NULL;
    _private->_timeline[0] = new BrutusPvrBar(control(), this);
    _private->_timeline[0]->setGeometry(timelineRect(0));
#endif

    if (cfg()->getBool(Config::SIDEBYSIDE_ENABLED)) {
        _private->_wow = new SbsWindowOnWindow(this, control());
    } else {
        _private->_wow = new PipWindowOnWindow(this, control());
    }

    if (control()->pipEnabled()) {
#ifdef PVR_SUPPORT
        _private->_timeline[1] = new BrutusPvrBar(control(), wow()->WowWidget());
#endif
        setPipGeometry();
    }


#ifdef INFOPANEL_SUPPORT
    // infoPanel must come after (thereby on top) of pipoutline
    _private->_infoBar = new InfoBar(control(), this, MRect(25,25,170,10));
    _private->_infoBar->hide();
#endif

#ifdef PVR_SUPPORT
    _private->_recordIndicator = new RecordIndicator(this, control());
    _private->_recordIndicator->setGeometry(recordIndicatorRect());
    _private->_recordIndicator->hide();
#endif

    // by default, no pip
    if (control()->pipEnabled())
        hidePipOutline();

    //create fullscreen transparent label - this will ensure the screen is
    //cleared of other windows when going to fullscreen tv.  this also gives
    //the widgets on this screen more flexibility in their alpha values.
    MLabel * lbl = new MLabel(this, MRect(0, 0, 720, 1080));
    lbl->setBackgroundColor(control()->getSkin()->getColorValue("00000000"));
}

TVScreen::~TVScreen() {
    delete _private;
}

#ifdef PVR_SUPPORT
void TVScreen::updateRecordIndicator() {
    if (control()->totalRecords() && cfg()->getBool(Config::PVR_TIMELINE_ENABLED)) {
        if (_private->_recordIndicator->visible())
            _private->_recordIndicator->repaint();
        else
            _private->_recordIndicator->show();
    }
    else {
        _private->_recordIndicator->hide();
    }
}

void TVScreen::setPlayEngines() {
	PlayEngine *playEngine0 = control()->playEngine(0);
    PlayEngine *playEngine1 = control()->playEngine(1);

    bool swapped = control()->areWindowsSwapped() || control()->secondDisplay();

    if(!swapped){
        if(_private->_timeline[0]) _private->_timeline[0]->setPlayEngine(playEngine0);
        if(_private->_timeline[1]) _private->_timeline[1]->setPlayEngine(playEngine1);
    }
    else{
        if(_private->_timeline[1]) _private->_timeline[1]->setPlayEngine(playEngine0);
        if(_private->_timeline[0]) _private->_timeline[0]->setPlayEngine(playEngine1);
    }

}
#else
void TVScreen::setPlayEngines() {}
void TVScreen::updateRecordIndicator() {}
#endif

void TVScreen::showPipOutline(bool focused) {
#ifdef PVR_SUPPORT
    int pipsource = control()->areWindowsSwapped() ? 0 : 1;
    if (control()->isPlaying(pipsource) && !cfg()->getBool(Config::HIDE_PVR_TIMELINE)) {
        if (cfg()->getBool(Config::PVR_TIMELINE_ENABLED)) {
            _private->_timeline[1]->show();
        }
    }
#endif
    wow()->ShowWow(true, focused);
}

void TVScreen::hidePipOutline() {
#ifdef PVR_SUPPORT
    if (_private->_timeline[1])
        _private->_timeline[1]->hide();
#endif
    wow()->ShowWow(false, false);
}

#ifdef INFOPANEL_SUPPORT
bool TVScreen::focusUpperLeft() {
    if (isInfoVisible()) {
        _private->_infoBar->setCurrent(0);
        return true;
    }
    return focus();
}

bool TVScreen::focus()
{
    if (isInfoVisible())
        return _private->_infoBar->focus();
    else
        return MWidget::focus();
}

bool TVScreen::isInfoVisible() {
    /* we don't care if it's actually visible, because we may be changing screens
    and haven't are testing this midway. The issue is whether showInfo() has been
    called, therefore use visibleSet(), not visible(). */
    return _private->_infoBar &&
        _private->_infoBar->visibleSet() &&
        _private->_infoBar->mode() == InfoBar::eFull;
}
#else
bool TVScreen::focus() {
    return MWidget::focus();
}
bool TVScreen::focusUpperLeft() {
    return MWidget::focusUpperLeft();
}
bool TVScreen::isInfoVisible() {
    return false;
}
#endif

/**
PIP involves coordinating graphics and decode window size & position.
Because graphics is drawn one way, then scaled to various display sizes,
the application makes the PIP graphics rectangle is primary.
After drawing the rectangle, the decode window must be fit to the graphics window
using an empirical (i.e. trial-and-error) approach.
**/
MRect TVScreen::fullScreenPipOutlineRect() const
{
    bdisplay_settings display_settings;
    bvideo_format_settings format_settings;

    /* get the format */
    control()->getOutput(&display_settings);
    bdisplay_get_video_format_settings(&display_settings, &format_settings);

    switch (format_settings.height) {
    case 1080:
    case 720:
    default: //480
        return MRect(330,3,288,216); /* NOTE: This results in a 16x9 PIP because
            we're rendering a 4x3 PIP into a 4x3 framebuffer, then scaling
            the framebuffer to 16x9. The result is a PIP which is 16x9.
            Also, it's important that the graphics size be the same for all
            formats, otherwise HDSD_SINGLE mode breaks. */

    case 576:
        // for some content, there is a little black INSIDE the box, but that
        // is better than having anything OUTSIDE the box.
        return MRect(306,9,322,294);
    }
}

/**
* PIP geometry can change depending on output settings
**/
void TVScreen::setPipGeometry() {
    assert(control()->pipEnabled());
#ifdef PVR_SUPPORT
    _private->_timeline[1]->setGeometry(timelineRect(1));
#endif

#ifdef INFOPANEL_SUPPORT
    // I have to repaint the tvscreen (infobar and friends) because
    // it's going to get clipped and not redrawn (possible mlibs bug)
    if (_private->_infoBar)
        repaint();
#endif
    wow()->Resized();
}

bool TVScreen::keyDown(int ascii, int modifiers, int scancode) {
#ifdef INFOPANEL_SUPPORT
    // When the info panel is up, any key it doesn't process should be ignored.
    if (isInfoVisible()) {
        return false;
    }
#endif

    /* This remaps certain functions only when PIP requires input, and
     * PIP is currently enabled.  This includes the left/right
     * mapping that PVR usually enjoys.
     */
    if (control()->pipEnabled() && wow()->HandlesInput()) {
        bool ret = false;
        switch(ascii) {
        case MWKEY_LEFT:
            wow()->ProcessCommand(IND_LEFT);
            ret = true;
            break;
        case MWKEY_RIGHT:
            wow()->ProcessCommand(IND_RIGHT);
            ret = true;
            break;
        case MWKEY_UP:
            wow()->ProcessCommand(IND_UP);
            ret = true;
            break;
        case MWKEY_DOWN:
            wow()->ProcessCommand(IND_DOWN);
            ret = true;
            break;
        case MWKEY_ENTER:
            wow()->ProcessCommand(IND_SELECT);
            ret = true;
            break;
        }
        if (wow()->IsChangeable()) {
            control()->setWindowSizes(); /* reload window sizes */
        }
        return ret;
    }

#ifdef PVR_SUPPORT
    if (control()->cfg()->getBool(Config::THUMBNAIL_MODE)) {
        switch (ascii) {
        case MWKEY_ENTER:
        case MWKEY_RIGHT:
        case MWKEY_LEFT:
            if (_private->_timeline[0]->processCommand(ascii))
                return true;
            break;
        }
    }

    /* PVR controls that should work for every input device.
    NOTE: there are special PVR controls for frontpanel which are
    present in inputengine.cpp */
    if (control()->isPlaying()) {
        PlayEngine *playEngine = control()->playEngine();
        switch (ascii) {
        case MWKEY_RIGHT:
            playEngine->skip(30 * 1000);
            return true;
        case MWKEY_LEFT:
            playEngine->skip(-5 * 1000);
            return true;
        }
    }
#endif

    return false;
}

void TVScreen::showMessage(bool withinfo) {
#ifdef INFOPANEL_SUPPORT
    if (!withinfo && !isInfoVisible()) {
        if (cfg()->getBool(Config::STREAM_INFO_DISABLED)) {
            return;
        }
    }
    if (withinfo || !isInfoVisible()) {
        _private->_infoBar->setMode(withinfo?InfoBar::eFull:InfoBar::eCompact);
    }
    _private->_infoBar->show();
    if (withinfo) {
        _private->_infoBar->focus();
    }
#endif
}

void TVScreen::hideMessage() {
#ifdef INFOPANEL_SUPPORT
    _private->_infoBar->hide();
#endif
    focus(); // tvscreen
}

MRect TVScreen::timelineRect(int disp) const {
    MRect tv;
    if (disp == 0) {
        /* TODO: it would be nice to position the timeline lower, but this
        would involve increasing the screen height. Is it worth it? */
        tv = MRect(0,0,control()->fb()->screenWidth(),control()->fb()->screenHeight());
    }
    else {
        tv = fullScreenPipOutlineRect();
    }
    int w = tv.width() * 5 / 6;
    int h = 40;
    int y = tv.height()-h-h/2;
    return MRect((tv.width()-w)/2, y, w, h);
}

MRect TVScreen::recordIndicatorRect() const {
    MRect tv;
    tv = MRect(0,0,control()->fb()->screenWidth(),control()->fb()->screenHeight());
    int wT = tv.width() * 5 / 6;
    int h = 40;
    int y = tv.height()-h-h/2 + 40;
    /* Make sure there is enough room for 3 record indicators, or extend
    the widget to paint unused area with colorkey value. */
    return MRect((tv.width()-wT)/2, y, 110, h / 2);
}

WindowOnWindow *TVScreen::wow() {
    return _private->_wow;
}
