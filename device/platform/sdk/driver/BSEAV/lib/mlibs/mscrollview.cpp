/***************************************************************************
 *     Copyright (c) 2002-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mscrollview.cpp $
 * $brcm_Revision: 6 $
 * $brcm_Date: 3/26/10 5:17p $
 *
 * Module Description:
 *
 * MScrollView provides a clipped area (called the viewport) on which you can put widgets.
 * The viewport may be scrolled.
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mscrollview.cpp $
 * 
 * 6   3/26/10 5:17p erickson
 * SW7405-3625: avoid possible div-by-zero failure
 *
 * 5   4/16/07 10:28a erickson
 * PR29757: MScrollView should consume the page up/down keys
 *
 * 4   2/27/07 4:16p erickson
 * PR24971: hooked up scrollbar Scroll event for debug
 *
 * 3   5/31/06 4:17p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/1   4/27/06 10:48a tokushig
 * add scrollbar support
 *
 * 2   10/3/05 11:18a erickson
 * PR17108: fixed linux 2.6 compilation warnings/errors
 *
 * 1   2/7/05 11:17p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/5   7/14/04 1:48a erickson
 * PR11287: removed unneeded linux-specific code
 *
 * Irvine_BSEAVSW_Devel/4   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/3   11/6/03 5:11p erickson
 * PR8570: I'm not done, but I need to work on another bug.
 *
 * Irvine_BSEAVSW_Devel/PR8570/1   11/6/03 11:40a erickson
 * I'm not done, but I need to work on another bug.
 *
 * Irvine_BSEAVSW_Devel/2   6/11/03 11:55a erickson
 * 7319 uclibc support
 *
 * Irvine_BSEAVSW_Devel/5   4/30/03 9:08a erickson
 * made Auto mode work for scrollbars
 * fixed scrollTo
 * fixed up/down keys
 * fixed page up/down
 *
 * SanJose_Linux_Devel/4   11/21/02 1:59p erickson
 * set _clipper name correctly
 *
 * SanJose_Linux_Devel/3   10/18/02 11:14a erickson
 * correctly passing name to sub-constructor
 *
 ***************************************************************************/
#include "mscrollview.h"
#include "mapplication.h"

BDBG_MODULE(mscrollview);

#define MAXINT 0x7fffffff

#define DEFAULT_VIEWPORT_WIDTH 2000
#define DEFAULT_VIEWPORT_HEIGHT 2000

MScrollView::MScrollView(MWidget *parent, const MRect &rect, const char *name) :
    MWidget(parent, rect, NULL, name)
{
    _clipper = new MWidget(this, clientRect(), NULL, "clipper");
    _viewport = new MWidget(_clipper, MRect(0,0,DEFAULT_VIEWPORT_WIDTH,DEFAULT_VIEWPORT_HEIGHT),
        NULL, "__viewport");
    _vsb = new MScrollBar(this, MScrollBar::Vertical, "vsb");
    _vsb->addHandler(this);
    _hsb = new MScrollBar(this, MScrollBar::Horizontal, "hsb");
    _hsb->addHandler(this);
    _vsbmode = Auto;
    _hsbmode = AlwaysOff;
    layout();
}

void MScrollView::addChild(MWidget *widget, int x, int y) {
    widget->reparent(viewport());
    widget->moveTo(x,y);
    layout();
}

void MScrollView::removeChild(MWidget *widget) {
    widget->reparent(NULL);
    layout();
}

void MScrollView::moveChild(MWidget *widget, int x, int y) {
    widget->moveTo(x-viewport()->x(), y-viewport()->y());
}

void MScrollView::setVScrollBarMode(ScrollBarMode mode) {
    _vsbmode = mode;
    layout();
}

void MScrollView::setHScrollBarMode(ScrollBarMode mode) {
    _hsbmode = mode;
    layout();
}

void MScrollView::setGeometry(const MRect &rect) {
    MWidget::setGeometry(rect);
    layout();
}

#define SCROLLBAR_WIDTH 20

/**
* Layout the clipper and scrollbars.
*
*/
//TODO: set the scrollbars based on max x&&y of children.
void MScrollView::layout() {
    if (_updating)
        return;
    //printf("MScrollView::layout\n");
    int w = width();
    int h = height();

    int sx = MAXINT, sy = MAXINT, sr = 0, sb = 0;
    for (MWidget *child = viewport()->firstChild(); child; child = viewport()->nextChild()) {
        if (child->x() < sx)
            sx = child->x();
        if (child->y() < sy)
            sy = child->y();
        if (child->right() > sr)
            sr = child->right();
        if (child->bottom() > sb)
            sb = child->bottom();
    }

    //set rect containing all viewport children
    _rectChildren.set(sx, sy, sr, sb);

    // figure out what's visible
    bool vsb = false, hsb = false;
    switch (_vsbmode) {
    case Auto:
        if (sb - sy > (int)height())
            vsb = true;
        break;
    case AlwaysOn:
        vsb = true;
        break;
    }
    switch (_hsbmode) {
    case Auto:
        if (sr - sx > (int)width())
            hsb = true;
        break;
    case AlwaysOn:
        hsb = true;
        break;
    }

    if (vsb) {
        w -= SCROLLBAR_WIDTH;
        _vsb->setGeometry(MRect(w,0,SCROLLBAR_WIDTH,height()-(hsb?SCROLLBAR_WIDTH:0)));
        _vsb->show();
    }
    else
        _vsb->hide();
    if (hsb) {
        h -= SCROLLBAR_WIDTH;
        _hsb->setGeometry(MRect(0,h,width()-(vsb?SCROLLBAR_WIDTH:0),SCROLLBAR_WIDTH));
        _hsb->show();
    }
    else
        _hsb->hide();

    _clipper->setGeometry(0,0,w,h);
}

bool MScrollView::focus() {
    // closest child to upper left in the right/down direction gets focus
    MWidget *closest = findClosestPeer(0,0,1,1);
    if (closest)
        return closest->focus();
    else
        return false;
}

MWidget *MScrollView::findClosestPeer(int x, int y, int dx, int dy, MWidget *ignoreWidget) {
    // closest child to upper left gets focus
    int min = MAXINT;
    MWidget *closest = NULL;
    for (MWidget *child = viewport()->firstChild(); child; child = viewport()->nextChild()) {
        int diffx = child->x() - x;
        int diffy = child->y() - y;

        // is this in the desired direction?
        if (diffx*dx<0 || diffy*dy<0 || ignoreWidget == child)
            continue;

        int dist = diffx*diffx + diffy*diffy;
        if (dist < min) {
            min = dist;
            closest = child;
        }
    }
    return closest;
}

void MScrollView::showChild(MWidget *w) {
    int sx = contentsX();
    int sy = contentsY();
    int x = childX(w);
    int y = childY(w);
    if (x < 0)
        sx = w->x();
    else if (x+(int)w->width() > _clipper->right()) {
        int diff = _clipper->width() - (int)w->width();
        if (diff < 0) diff = 0;
        sx = w->x()-diff;
    }
    if (y < 0)
        sy = w->y();
    else if (y+(int)w->height() > _clipper->bottom()) {
        int diff = _clipper->height()-w->height();
        if (diff < 0) diff = 0;
        sy = w->y()-diff;
    }
    scrollTo(sx,sy);
}

bool MScrollView::keyDown(int ascii, int modifiers, int scancode) {
    MWidget *child = NULL;
    switch (ascii) {
    case MWKEY_PAGEDOWN:
        //TODO: What about partial elements...it would be nice to align to them.
        child = findClosestPeer(contentsX(),contentsY()+height()*2,1,-1);
        if (child) {
            child->focus();
            // scroll back up to make the last child on the bottom
            if (childY(child) + child->height() < height())
                scrollBy(0, (childY(child) + child->height()) - height());
            return true;
        }
        break;
    case MWKEY_PAGEUP:
        child = findClosestPeer(contentsX(),contentsY()-height(),1,1);
        if (child) {
            child->focus();
            return true;
        }
        break;

    /* TODO: right and left page scrolling */
    case MWKEY_UP:
        // page scrolling
        if (hScrollBarMode() == AlwaysOff) {
            MWidget *cur = app()->focusedWidget();
            if (cur->parent() == _viewport)
                child = findClosestPeer(cur->x(), cur->y(), 0, -1, cur);
            if (child && childY(child) < 0) {
                scrollBy(0,-height());
            }
            /* do not consume key. this code does not change focus. */
        }
        break;
    case MWKEY_DOWN:
        // page scrolling
        if (hScrollBarMode() == AlwaysOff) {
            MWidget *cur = app()->focusedWidget();
            if (cur->parent() == _viewport)
                child = findClosestPeer(cur->x(), cur->y(), 0, 1, cur);
            if (child && childY(child) + child->height() > height()) {
                scrollBy(0,height());
            }
            /* do not consume key. this code does not change focus. */
        }
        break;
    }
    return MWidget::keyDown(ascii, modifiers, scancode);
}

void MScrollView::scrollTo(int x, int y) {
    if (x < 0)
        x = 0;
    else if (x > (int)_viewport->width() - (int)width())
        x = _viewport->width() - width();
    if (y < 0)
        y = 0;
    else if (y > (int)_viewport->height() - (int)height())
        y = _viewport->height() - height();
    BDBG_MSG(("scrollTo %d,%d", x, y));
    _viewport->moveTo(-x,-y);

    if (_rectChildren.width()) {
        _hsb->setValue((uint32_t)x * 100 / _rectChildren.width());
    }
    if (_rectChildren.height()) {
        _vsb->setValue((uint32_t)y * 100 / _rectChildren.height());
    }
}

void MScrollView::onScroll(MScrollBar *scrollBar)
{
	/* NOTE: This code does not work. It may be interesting to do a reverse mapping, but it's not really needed. */
	BDBG_MSG(("scrollbar %d", scrollBar->value()));
	if (scrollBar == _vsb) {
		_viewport->moveTo(_viewport->x(), _vsb->value() * _rectChildren.height() / 100);
	}
	else {
		_viewport->moveTo(_hsb->value()  * _rectChildren.width() / 100, _viewport->y());
	}
}
