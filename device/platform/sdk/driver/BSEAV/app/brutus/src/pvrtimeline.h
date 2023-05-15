/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: pvrtimeline.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 3/19/08 9:35p $
 *
 * Module Description: Timeline showing current position of PVR
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/pvrtimeline.h $
 * 
 * 4   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 * 
 * 3   12/7/07 2:00p erickson
 * PR37590: rework continuous record timeline UI
 * 
 * 2   2/14/05 5:01p erickson
 * PR11077: renamed self to playEngine
 * 
 * 1   2/7/05 8:16p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/4   2/18/04 2:22p erickson
 * PR9757: unlink PlayEngine's in destructor
 * 
 * Irvine_BSEAVSW_Devel/3   7/23/03 4:01p erickson
 * renamed from pvrgui to brutus
 * 
 * Irvine_BSEAVSW_Devel/2   6/27/03 10:11a erickson
 * broke pvrtimeline into components
 *
 ***************************************************************************/
#ifndef PVRTIMELINE_H
#define PVRTIMELINE_H

#include "mwidget.h"
#include "playengine.h"
#include "mpainter.h"

/**
* Draws a timeline for PVR playback. Includes a cursor and a border around the
* bar which allows the cursor to extend above and below the bar.
**/
class PvrTimeline : public MWidget,
    public PlayEngineEventHandler
{
public:
    PvrTimeline(const Config *cfg, MWidget *parent);
    ~PvrTimeline();

    PlayEngine *pvrEngine() const {return _engine;}
    void setPlayEngine(PlayEngine *engine);

    void setRecordTimeWindow(int msec);

    // events
    void onPosChanged(PlayEngine *playEngine);
    void onFileChanged(PlayEngine *playEngine);

    // overrides
    void setGeometry(const MRect &rect);

protected:
    void draw(const MRect &cliprect);

    void setTimeRange(int start, int end);
    int timeToCoord(int msec);
    void stateChange();

    // calc and paint methods
    bool calcCursorRect(int pos);
    bool calcBar(int pos);
    int barColor() const;
    void paintBar(MPainter &ptr, bool fillblankarea = true);

    const Config *_cfg;
    PlayEngine *_engine;
    unsigned long _startTime, _endTime; /* timerange of pvrtimeline in milliseconds */
    int _barWidth; /* in pixels */
    int _diffTime;
    int _hborder, _vborder; /* in pixels */
    MRect _cursorRect;
    bool _prevBarWasFullWidth;
    int _recordTimeWindow;
};

#endif //PVRTIMELINE_H
