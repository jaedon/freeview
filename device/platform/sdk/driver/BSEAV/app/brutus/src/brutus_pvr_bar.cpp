/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brutus_pvr_bar.cpp $
 * $brcm_Revision: 20 $
 * $brcm_Date: 5/19/08 12:53p $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/brutus_pvr_bar.cpp $
 * 
 * 20   5/19/08 12:53p erickson
 * PR42752: check pointer before deref
 *
 * 19   4/21/08 12:35p ssood
 * PR36762: merging fix to mail line
 *
 * PR36762/1   4/18/08 3:41p ssood
 * PR36762: previous fix for this PR breaks PVR timeline where by it stays
 * on when u tune from a HTTP to a Live channel
 *
 * 18   3/22/08 2:09a erickson
 * PR40307: thumbnail demo update
 *
 * 17   3/20/08 3:18a erickson
 * PR40307: hide thumbnail bar
 *
 * 16   3/20/08 1:49a erickson
 * PR40307: update
 *
 * 15   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 *
 * 14   3/12/08 4:28p jrubio
 * PR36762: check _engine before calling onStateChanged
 *
 * 13   2/19/08 11:15a yrajakar
 * PR36762: Update status with first_pts, and duration
 *
 * 12   9/11/07 2:50p jrubio
 * PR34614: adding PVR timeline to HTTP channels
 *
 * 11   11/6/06 12:17p arbisman
 * PR25559: Restore PVR timeline
 *
 * 10   9/28/06 7:04p arbisman
 * PR24289: Backed-out changes
 *
 * 8   1/25/06 3:33p mphillip
 * PR16055: Missed a file that has PVR_TIMELINE_ENABLED support
 *
 * 7   8/12/05 10:43a jjordan
 * PR16669: add IP STB playback from HDD capability
 *
 * 6   3/25/05 2:07p erickson
 * PR14593: hide timeline until PlayEngine gets set because it must be
 * invisible if PlayEngine is never set
 *
 * 5   2/25/05 12:40p erickson
 * PR14162: pop up the timeline for a bit when jumping forward/back
 *
 * 4   2/15/05 5:22p erickson
 * PR13387: added primitive palettized color support based on MStyle's
 * palette trick
 *
 * 3   2/15/05 10:59a erickson
 * PR11077: expand play timeout
 *
 * 2   2/14/05 5:01p erickson
 * PR11077: add option to hide timeline during normal play
 *
 * 1   2/7/05 8:17p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 ***************************************************************************/
#include "brutus_pvr_bar.h"
#include "cfgsettings.h"
#include "mapplication.h"
#include "video.h"
#include "bsettop.h"
#include "control.h"

BDBG_MODULE(brutus_pvr_bar);

// hardcoded dimensions of the UI element
#define STATE_LABEL_WIDTH 120
#define STATE_HEIGHT 20
#define OUTLINE_THICKNESS 4

BrutusPvrBar::BrutusPvrBar(Control *control, MWidget *parent) :
    MWidget(parent), _timer(parent->app())
{
    _control = control;
    setBackgroundColor(app()->style()->color(MStyle::TRANSPARENT));
    _timer.addHandler(this);
    _engine = NULL;
    hide(); /* explicitly hide until a PlayEngine is set */

    _timeline = new PvrTimeline(control->cfg(), this);
    _stateicon = new PvrStateIcon(control->cfg(), this, MRect(0,0,STATE_HEIGHT,STATE_HEIGHT));
    _statelabel = new PvrStateLabel(this, MRect(STATE_HEIGHT+5,1,STATE_LABEL_WIDTH-5-STATE_HEIGHT,STATE_HEIGHT+2));
    _statelabel->setBackgroundColor(control->cfg()->getInt(Config::PVRBAR_OUTLINE_COLOR));
    if (control->cfg()->getBool(Config::THUMBNAIL_MODE)) {
        _thumbnailPanel = new MThumbnailPanel(parent, control, MRect(60,310,550,65));
        _thumbnailPanel->setViewWindow(
            control->cfg()->getInt(Config::THUMBNAIL_SPACING, 10),
            0x00000000);
    }
    else {
        _thumbnailPanel = NULL;
    }
}

BrutusPvrBar::~BrutusPvrBar()
{
    // unlink
    setPlayEngine(NULL);
}

#define cfg() (_control->cfg())


void BrutusPvrBar::setGeometry(const MRect &rect) {
    MWidget::setGeometry(rect);
    _timeline->setGeometry(MRect(
        OUTLINE_THICKNESS,
        STATE_HEIGHT+OUTLINE_THICKNESS,
        width()-OUTLINE_THICKNESS*2,
        height()-STATE_HEIGHT-OUTLINE_THICKNESS*2
        ));
}


void BrutusPvrBar::draw(const MRect &cliprect) {
    MPainter ptr(this, cliprect);
    ptr.setBrush(cfg()->getInt(Config::PVRBAR_OUTLINE_COLOR));

    // draw bar around timeline
    ptr.fillRect(MRect(0,STATE_HEIGHT,width(),OUTLINE_THICKNESS));
    ptr.fillRect(MRect(0,STATE_HEIGHT,OUTLINE_THICKNESS,height()-STATE_HEIGHT));
    ptr.fillRect(MRect(0,height()-OUTLINE_THICKNESS,width(),OUTLINE_THICKNESS));
    ptr.fillRect(MRect(width()-OUTLINE_THICKNESS,STATE_HEIGHT,OUTLINE_THICKNESS,height()-STATE_HEIGHT));

    // draw angled surface in back of statelabel
    ptr.setPen(cfg()->getInt(Config::PVRBAR_OUTLINE_COLOR));
    for (int y=0;y<STATE_HEIGHT+OUTLINE_THICKNESS;y++)
        ptr.drawLine(STATE_HEIGHT,y,STATE_LABEL_WIDTH+y,y);
}

void BrutusPvrBar::setPlayEngine(PlayEngine *engine) {
    if (_engine != engine) {
        if (_engine)
            _engine->removeHandler(this);

        _engine = engine;
        _timeline->setPlayEngine(_engine);
        _stateicon->setPlayEngine(_engine);
        _statelabel->setPlayEngine(_engine);

        if (_engine) {
            _engine->addHandler(this);
        }
    }
    onStateChanged(_engine);
}

void BrutusPvrBar::show() {
    _timeline->setRecordTimeWindow(cfg()->getInt(Config::RECORD_TIME_WINDOW) * 1000);
    MWidget::show();
}

void BrutusPvrBar::onTimeout(MTimer *timer) {
    hide();
}

void BrutusPvrBar::onSpecialStateMessage(PlayEngine *playEngine)
{
    testShow();
}

void BrutusPvrBar::onStateChanged(PlayEngine *playEngine)
{
    testShow();
}

void BrutusPvrBar::testShow()
{
    if (cfg()->getBool(Config::PVR_TIMELINE_ENABLED)) {
        if (cfg()->getBool(Config::HIDE_PVR_TIMELINE) &&
            _engine &&
            _engine->state() == bplayback_state_playing)
        {
            /* allow the play state to be shown for a little */
            _timer.start(2000, MTimer::eOnce);
        }
        else {
            _timer.stop();
        }

        if (_engine && _engine->isActive()) {
            show();
            if (_thumbnailPanel) _thumbnailPanel->show();
        }
        else {
            hide();
            if (_thumbnailPanel) _thumbnailPanel->hide();
        }
    }
}

void BrutusPvrBar::onPosChanged(PlayEngine *playEngine)
{
    if (_thumbnailPanel && visible()) {
        _thumbnailPanel->setVisible(NULL, true, false);
        _thumbnailPanel->setVisible(playEngine->video(), true, true);
        _thumbnailPanel->setVideo(playEngine->video());
        _thumbnailPanel->setPlayerPos(playEngine->pos()/1000);
        _thumbnailPanel->repaint();
    }
}

bool BrutusPvrBar::processCommand(int ascii)
{
    if (_engine && _thumbnailPanel) {
        switch (ascii) {
        case MWKEY_RIGHT:
            _thumbnailPanel->userNavigate(+1);
            return true;
        case MWKEY_LEFT:
            _thumbnailPanel->userNavigate(-1);
            return true;
        case MWKEY_ENTER:
            _engine->gotoTimestamp(_thumbnailPanel->currentThumbnailPos()*1000);
            return true;
        }
    }
    return false;
}
