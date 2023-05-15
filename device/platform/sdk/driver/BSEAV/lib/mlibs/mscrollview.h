/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mscrollview.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 3/26/10 5:17p $
 *
 * Module Description:
 *
 * MScrollView provides a clipped area (called the viewport) on which you can put widgets.
 * The viewport may be scrolled.
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mscrollview.h $
 * 
 * 5   3/26/10 5:17p erickson
 * SW7405-3625: fix #define
 *
 * 4   2/27/07 4:16p erickson
 * PR24971: hooked up scrollbar Scroll event for debug
 *
 * 3   5/31/06 4:17p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/3   5/16/06 10:17a tokushig
 * Merge from main
 *
 * SanDiego_Brutus_Skin/2   4/27/06 10:48a tokushig
 * add scrollbar support
 *
 * 2   4/17/06 5:05p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/1   2/17/06 3:25p tokushig
 * added methods for scrollbar access
 *
 * 1   2/7/05 11:18p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/3   4/30/03 9:08a erickson
 * made Auto mode work for scrollbars
 * fixed scrollTo
 * fixed up/down keys
 * fixed page up/down
 *
 * NOTE: There are some hooks in mwidget.cpp because we want scrollview's children to
 * be visible when focused.
 ***************************************************************************/
#ifndef MSCROLLVIEW_H
#define MSCROLLVIEW_H

#include "mwidget.h"
#include "mscrollbar.h"

class MScrollView : public MWidget, public MScrollBarEventHandler {
public:
    MScrollView(MWidget *parent, const MRect &rect, const char *name = NULL);
    const char *className() const {return "MScrollView";}

    void beginUpdates() {_updating = true;}
    void endUpdates() {_updating = false; layout();}

    /**
    * Reparents the child from the viewport.
    */
    void addChild(MWidget *widget, int x = 0, int y = 0);
    /**
    * Removes the child from the viewport, but doesn't delete
    * the widget.
    */
    void removeChild(MWidget *widget);

    void moveChild(MWidget *widget, int x, int y);

    /**
    * Create widgets with viewport() as parent, or call addChild()
    */
    MWidget *viewport() const {return _viewport;}
    /**
    * Clipper clips the viewport.
    */
    MWidget *clipper() const {return _clipper;}

    int contentsX() const {return -_viewport->x();}
    int contentsY() const {return -_viewport->y();}
    int contentsWidth() const {return _viewport->width();}
    int contentsHeight() const {return _viewport->height();}

    // x and y are child coordinates
    void scrollTo(int x, int y);

    // position dx/dy means right and down
    void scrollBy(int dx, int dy) {scrollTo(contentsX()+dx,contentsY()+dy);}

    /**
    * X coordinate relative to the clipper
    */
    int childX(MWidget *w) const {return w->x() + _viewport->x();}
    /**
    * Y coordinate relative to the clipper
    */
    int childY(MWidget *w) const {return w->y() + _viewport->y();}
    void showChild(MWidget *w);

    enum ScrollBarMode {Auto, AlwaysOff, AlwaysOn};

    ScrollBarMode vScrollBarMode() const {return _vsbmode;}
    void setVScrollBarMode(ScrollBarMode mode);
    ScrollBarMode hScrollBarMode() const {return _hsbmode;}
    void setHScrollBarMode(ScrollBarMode mode);

    MScrollBar * vScrollbar() {return _vsb;};
    MScrollBar * hScrollbar() {return _hsb;};

    bool focus();

protected:
    MWidget *_viewport, *_clipper;
    MScrollBar *_vsb, *_hsb;
    ScrollBarMode _vsbmode, _hsbmode;
    bool _updating;
    MRect _rectChildren;

    // overrides
    void setGeometry(const MRect &rect);
    bool keyDown(int ascii, int modifiers, int scancode);
    MWidget *findClosestPeer(int x, int y, int dx, int dy, MWidget *ignoreWidget = NULL);
    void onScroll(MScrollBar *scrollBar);

    virtual void layout();
};

#endif
