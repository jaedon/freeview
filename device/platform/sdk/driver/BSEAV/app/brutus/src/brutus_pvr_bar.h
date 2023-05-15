/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brutus_pvr_bar.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 3/22/08 2:09a $
 *
 * Module Description: GUI for pvr state
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/brutus_pvr_bar.h $
 * 
 * 7   3/22/08 2:09a erickson
 * PR40307: thumbnail demo update
 *
 * 6   3/20/08 1:49a erickson
 * PR40307: update
 *
 * 5   3/19/08 9:35p erickson
 * PR40307: thumbnail update
 *
 * 4   9/11/07 2:50p jrubio
 * PR34614: adding PVR timeline to HTTP channels
 *
 * 3   2/25/05 12:40p erickson
 * PR14162: pop up the timeline for a bit when jumping forward/back
 *
 * 2   2/14/05 5:01p erickson
 * PR11077: add option to hide timeline during normal play
 *
 * 1   2/7/05 8:17p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/6   2/18/04 2:22p erickson
 * PR9757: unlink PlayEngine's in destructor
 *
 * Irvine_BSEAVSW_Devel/5   12/19/03 10:05a erickson
 * PR8731: moved TVMessage code to tvscreen because we need this even if
 * there is no PVR_SUPPORT
 *
 * Irvine_BSEAVSW_Devel/4   7/23/03 4:01p erickson
 * renamed from pvrgui to brutus
 *
 * Irvine_BSEAVSW_Devel/3   7/1/03 5:57p erickson
 * got rid of bad repaint and unneeded shouldBeVisible func
 *
 * Irvine_BSEAVSW_Devel/2   6/30/03 1:34p erickson
 * new ui work
 *
 * Irvine_BSEAVSW_Devel/1   6/27/03 10:11a erickson
 * broke pvrtimeline into components
 *
 * SanJose_Linux_Devel/14   4/11/03 2:17p erickson
 * 7115 work
 *
 * SanJose_Linux_Devel/13   4/9/03 11:22a erickson
 * rework of pvrgui files and class names
 *
 * SanJose_Linux_Devel/12   3/13/03 12:06p erickson
 * Use cachedPos() in OnPosChanged
 *
 * SanJose_Linux_Devel/11   10/29/02 9:58a erickson
 * added PAL support, EditChannelsScreen
 *
 * SanJose_Linux_Devel\10   6/17/02 4:29p erickson
 * removed record from state
 *
 * SanJose_Linux_Devel\9   3/21/02 10:26a erickson
 * PVRGUI_1_2 release
 *
 ***************************************************************************/
#ifndef BRUTUS_PVR_BAR_H__
#define BRUTUS_PVR_BAR_H__

#include "mwidget.h"
#include "playengine.h"
#include "mpainter.h"
#include "pvrtimeline.h"
#include "pvrstateicon.h"
#include "thumbnail_panel.h"

class Control;

/**
BrutusPvrBar shows a status bar for PVR playback.
It collects a set of other widgets to display a timeline, state icon & text.
**/
class BrutusPvrBar : public MWidget,
    public PlayEngineEventHandler,
    public MTimerEventHandler
{
public:
    BrutusPvrBar(Control *control, MWidget *parent);
    ~BrutusPvrBar();
    void setPlayEngine(PlayEngine *engine);

    // overrides
    void setGeometry(const MRect &rect);
    void show();
    // events
    void onPosChanged(PlayEngine *playEngine);
    void onStateChanged(PlayEngine *playEngine);
    void onSpecialStateMessage(PlayEngine *playEngine);
    void onTimeout(MTimer *timer);

    bool processCommand(int ascii);

protected:
    void draw(const MRect &cliprect);
    void testShow();

    MTimer _timer;
    Control *_control;
    PlayEngine *_engine;
    PvrTimeline *_timeline;
    PvrStateIcon *_stateicon;
    PvrStateLabel *_statelabel;
    MThumbnailPanel *_thumbnailPanel;
};

#endif //BRUTUS_PVR_BAR_H__
