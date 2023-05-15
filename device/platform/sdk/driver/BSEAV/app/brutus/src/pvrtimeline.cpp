/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: pvrtimeline.cpp $
 * $brcm_Revision: 6 $
 * $brcm_Date: 3/19/08 9:35p $
 *
 * Module Description: Timeline showing current position of PVR
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/pvrtimeline.cpp $
 * 
 * 6   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 *
 * 5   12/7/07 2:00p erickson
 * PR37590: rework continuous record timeline UI
 *
 * 4   9/6/06 4:48p erickson
 * PR21941: don't redraw pvr cursor unless there's an actual change
 *
 * 3   2/15/05 5:22p erickson
 * PR13387: added primitive palettized color support based on MStyle's
 * palette trick
 *
 * 2   2/14/05 5:01p erickson
 * PR11077: renamed self to playEngine
 *
 * 1   2/7/05 8:15p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/14   2/4/05 10:47a erickson
 * PR12728: cleanup for -pedantic warnings and errors
 *
 * Irvine_BSEAVSW_Devel/13   8/23/04 11:43a erickson
 * PR8850: call ptr.sync after onFileChanged as well
 *
 * Irvine_BSEAVSW_Devel/12   4/30/04 12:05p erickson
 * PR10832: rework of brutus to support single AVManager and
 * ChannelManager for system
 *
 * Irvine_BSEAVSW_Devel/11   4/12/04 2:47p erickson
 * PR8850: Need to call MPainter::sync after updating cursor
 *
 * Irvine_BSEAVSW_Devel/10   3/30/04 7:12p bandrews
 * PR10031: Fixed by calling setBrush instead of setPen for painting the
 * blank area.
 *
 * Irvine_BSEAVSW_Devel/9   2/18/04 2:22p erickson
 * PR9757: unlink PlayEngine's in destructor
 *
 * Irvine_BSEAVSW_Devel/8   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/7   10/15/03 3:52p erickson
 * recording color should be based on timeshifting state, not video
 * current usage.
 *
 * Irvine_BSEAVSW_Devel/6   10/7/03 1:19p erickson
 * PR8241: don't update pvr timeline or state icon state unless pvr is
 * active
 *
 * Irvine_BSEAVSW_Devel/5   10/6/03 11:43a erickson
 * PR8105: PvrStateIcon, PvrStateLabel and PvrTimeline need to update
 * state when the PlayEngine is
 * reset. This happens during swap.
 *
 * Irvine_BSEAVSW_Devel/4   8/26/03 11:32a erickson
 * added new playengine monitor thread so that gui doesn't block when
 * reading pvr position
 *
 * Irvine_BSEAVSW_Devel/3   7/23/03 4:01p erickson
 * renamed from pvrgui to brutus
 *
 * Irvine_BSEAVSW_Devel/2   6/27/03 10:11a erickson
 * broke pvrtimeline into components
 *
 ***************************************************************************/
#include "pvrtimeline.h"
#include "cfgsettings.h"
#include "mmessagebox.h"
#include "mapplication.h"
#include "video.h"
#include "bstd.h"

BDBG_MODULE(brutus_pvrtimeline);

#define DEFAULT_VBORDER 1
#define DEFAULT_HBORDER 4
#define DEFAULT_TIME_RANGE (3*60*1000) //3 minutes

PvrTimeline::PvrTimeline(const Config *cfg, MWidget *parent) :
    MWidget(parent)
{
    _cfg = cfg;

    _vborder = DEFAULT_VBORDER;
    _hborder = DEFAULT_HBORDER;
    setBackgroundColor(app()->style()->color(MStyle::TRANSPARENT));
    _engine = NULL;
    _prevBarWasFullWidth = false;
    _recordTimeWindow = DEFAULT_TIME_RANGE;
    setTimeRange(0,_recordTimeWindow);
}

PvrTimeline::~PvrTimeline()
{
    // unlink
    setPlayEngine(NULL);
}

void PvrTimeline::setRecordTimeWindow(int msec) {
    if (!msec)
        msec = DEFAULT_TIME_RANGE;
    if (_recordTimeWindow != msec) {
        _recordTimeWindow = msec;
        setTimeRange(0,_recordTimeWindow);
    }
}

void PvrTimeline::setTimeRange(int start, int end) {
    _startTime = start;
    _endTime = end;
    _diffTime = _endTime - _startTime;
}

void PvrTimeline::setGeometry(const MRect &rect) {
    MWidget::setGeometry(rect);
    _barWidth = width() - _hborder*2;
}

int PvrTimeline::timeToCoord(int msec) {
    if (_diffTime && msec > (int)_startTime) {
        float temp = msec - _startTime;
        temp /= _diffTime;
        temp *= _barWidth;
        return _hborder + (int)temp;
    }
    else {
        return _hborder;
    }
}

void PvrTimeline::draw(const MRect &cliprect) {
    MPainter ptr(this, cliprect);

    if (_engine && _engine->isActive()) {
        int pos = _engine->pos();
        calcBar(pos);
        calcCursorRect(pos);
        paintBar(ptr, false);
    }
}

/**
* calcCursorRect returns true if the cursor is within the current bar range.
* Returns false if calcBar must be called.
* Don't call this unless you KNOW you will repaint the cursor, otherwise
* _cursorRect state goes invalid.
*/
bool PvrTimeline::calcCursorRect(int pos) {
    bool inRange = (pos >= (int)_startTime && pos < (int)_endTime);

    _cursorRect = MRect(timeToCoord(pos),_vborder,
        _cfg->getInt(Config::PVRBAR_CURSOR_WIDTH),
        height()-_vborder*2);

    return inRange;
}

/**
* calcBar returns true if the bar needs to be repainted.
*/
bool PvrTimeline::calcBar(int pos) {
    bool rc = true;

    if (_engine->video()->isContinuousRecord) {
        int last = _engine->lastpos();
        setTimeRange(last - _engine->video()->continuousRecordDuration*1000, last);
    }
    else if (!_engine->recordHandle()) {
        // playback bar gets a file width
        int lp = _engine->lastpos();
        if ((int)_startTime != 0 || (int)_endTime != lp) {
            //printf("setTimeRange 0-%ld\n", lp);
            setTimeRange(0, lp);
        }
        else
            rc = false;
    }
    else if (pos < (int)_startTime || pos > (int)_endTime ||
        _endTime-_startTime != (unsigned)_recordTimeWindow)
    {
        // recording bar gets a fixed width
        pos = pos - pos%_recordTimeWindow;
        setTimeRange(pos, pos + _recordTimeWindow);
    }
    else {
        // assume files never shrink
        if (_prevBarWasFullWidth &&
            _engine->firstpos() <= _startTime &&
            _engine->lastpos() >= _endTime)
            // we don't need to repaint the bar
            return false;
    }

    return rc;
}

/**
* Paint the entire bar. fillblankarea is set to true if the entire widget is
* being painted, and the blank areas are already clear.
*/
void PvrTimeline::paintBar(MPainter &ptr, bool fillblankarea) {
    int fp = _engine->firstpos();
    int lp = _engine->lastpos();

    // constants
    int h = height()-_vborder*2;
    int y = _vborder;
    int xlimit = width()-_hborder; /* the farthest x coordinate to draw */

    int x1 = timeToCoord(fp); /* left edge of the bar */
    int x2 = (lp==-1)?x1:timeToCoord(lp); /* right edge of the bar */
    if (x2 >= xlimit) {
        _prevBarWasFullWidth = (x1 == _hborder);
        x2 = xlimit;
    }
    else
        _prevBarWasFullWidth = false;
    if (x1 > xlimit)
        x1 = xlimit;

    BDBG_MSG(("full paintBar: %d..%d (time %d %d), cursor %d", x1, x2, fp, lp, _cursorRect.x()));
    ptr.setBrush(barColor());
    ptr.fillRect(MRect(x1,y,x2-x1,h));

    if (fillblankarea) {
        ptr.setBrush(backgroundColor());
        ptr.fillRect(MRect(_hborder,y,x1-_hborder,h));
        ptr.fillRect(MRect(x2,y,xlimit-x2+_cfg->getInt(Config::PVRBAR_CURSOR_WIDTH),h));
    }

    ptr.setBrush(_cfg->getInt(Config::PVRBAR_CURSOR_COLOR));
    ptr.fillRect(_cursorRect);
}

void PvrTimeline::setPlayEngine(PlayEngine *engine) {
    if (_engine != engine) {
        if (_engine)
            _engine->removeHandler(this);

        _engine = engine;

        if (_engine) {
            _engine->addHandler(this);
            if (_engine->video()) {
                onFileChanged(engine);
                onPosChanged(engine);
            }
        }
    }
}

int PvrTimeline::barColor() const {
    if (_engine->recordHandle())
        return _cfg->getInt(Config::PVRBAR_RECORDING_COLOR);
    else
        return _cfg->getInt(Config::PVRBAR_PLAYBACK_COLOR);
}

void PvrTimeline::onPosChanged(PlayEngine *playEngine) {
    if (!visible())
        return;

    MPainter ptr(this);
    MRect save = _cursorRect;
    int pos = playEngine->pos();
    // check if we need to paint the whole bar, or just the cursor
    if (!calcCursorRect(pos)) {
        calcBar(pos);
        paintBar(ptr);
    }
    else {
        MRect bardiff = save.minus(_cursorRect);
        MRect cursordiff = _cursorRect.minus(save);
        if (bardiff.width() || cursordiff.width()) {
            ptr.setBrush(barColor());
            ptr.fillRect(bardiff);
            ptr.setBrush(_cfg->getInt(Config::PVRBAR_CURSOR_COLOR));
            ptr.fillRect(cursordiff);
            ptr.sync();
            BDBG_MSG(("paintCursor: cursor %d", _cursorRect.x()));
        }
    }
}

void PvrTimeline::onFileChanged(PlayEngine *playEngine) {
    if (!visible())
        return;

    MPainter ptr(this);
    int pos = playEngine->pos(); // this is set in the same place, so it's safe
    if (calcBar(pos)) {
        // if false, then we need to paint the whole bar
        calcCursorRect(pos);
        paintBar(ptr);
    }
    ptr.sync();
}
