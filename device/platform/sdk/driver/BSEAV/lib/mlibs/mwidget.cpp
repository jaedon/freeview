/***************************************************************************
 *     Copyright (c) 2002-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mwidget.cpp $
 * $brcm_Revision: 6 $
 * $brcm_Date: 8/8/06 9:51a $
 *
 * Module Description: Fundamental graphics object in the mwidgets library
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mwidget.cpp $
 * 
 * 6   8/8/06 9:51a erickson
 * PR23250: added setZOrder
 * 
 * 5   5/31/06 4:16p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/7   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 4   4/17/06 5:08p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/6   3/22/06 5:08p tokushig
 * set background pixmaps to be scaled to full resolution of background.
 * this may include unproportional stretching
 * 
 * SanDiego_Brutus_Skin/5   3/10/06 12:10p tokushig
 * revert last changes from last check in...cliprect handled by mpainter
 * 
 * SanDiego_Brutus_Skin/4   3/10/06 11:52a tokushig
 * when drawing background image/pixmap, set given cliprect instead of
 * just painting the entire image. this will speed up menu traversal
 * since the entire background does not need to be redrawn each time
 * focus is moved.
 * 
 * SanDiego_Brutus_Skin/3   2/28/06 1:25p tokushig
 * added dropshadow attribute
 * 
 * SanDiego_Brutus_Skin/2   1/26/06 9:54a tokushig
 * add action get/set methods to allow each widget to be associated with a
 * generic action
 * 
 * SanDiego_Brutus_Skin/1   1/11/06 11:20a tokushig
 * add handling for text focus color and for background pixmap
 * 
 * 3   4/18/05 3:07p erickson
 * PR14593: drawNow shouldn't draw invisible widgets
 * 
 * 2   4/18/05 10:51a erickson
 * PR14593: added findBackgroundColor to walk the parent tree
 * 
 * 1   2/7/05 11:20p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/14   1/7/05 12:56p erickson
 * PR13639: default background to be strech, not tile
 * 
 * Irvine_BSEAVSW_Devel/13   9/13/04 1:52p erickson
 * PR11081: added code to prevent possible dereference of NULL pointer
 * during constructor, which can be hard to debug
 * 
 * Irvine_BSEAVSW_Devel/12   9/9/04 1:39p erickson
 * PR12608: improved debug
 * 
 * Irvine_BSEAVSW_Devel/11   7/28/04 11:59a erickson
 * PR11768: convert printfs to BDBG_MSG
 * 
 * Irvine_BSEAVSW_Devel/10   7/14/04 1:48a erickson
 * PR11287: removed unneeded linux-specific code
 * 
 * Irvine_BSEAVSW_Devel/9   4/30/04 3:12p erickson
 * PR10883: implemented real raise/lower after discovering min/max zorder
 * 
 * Irvine_BSEAVSW_Devel/8   4/22/04 10:15p erickson
 * PR9107: need to sync after any drawNow
 * 
 * Irvine_BSEAVSW_Devel/7   3/23/04 5:37p erickson
 * PR9753: adapted to bwin changes now that zorder is implemented
 * 
 * Irvine_BSEAVSW_Devel/6   3/4/04 11:33a erickson
 * PR9023: don't allow focus to land on disabled widget
 * 
 * Irvine_BSEAVSW_Devel/5   2/26/04 11:03a erickson
 * PR9900: now using MListItr to allow const access to MList's
 * 
 * Irvine_BSEAVSW_Devel/4   2/18/04 2:01p erickson
 * PR9752: Invisible widgets never have focus, and unset _focus in the
 * widgetDestroyed function
 * 
 * Irvine_BSEAVSW_Devel/3   2/11/04 12:36p erickson
 * PR9691: need MPainter::sync call (which involved moving MFrameBuffer
 * into MPaintDevice)
 * 
 * Irvine_BSEAVSW_Devel/2   2/5/04 7:43p bandrews
 * PR9107: Fixed drawBackground to use x=0 and y=0 instead of the widget's
 * x and y.
 * 
 * Irvine_BSEAVSW_Devel/1   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/6   12/10/03 1:34p erickson
 * PR8813: Copybackground with a backgroundPixmap wasn't working. We
 * needed a way to determine which was closer, a pixmap or a background
 * color.
 * 
 * Irvine_BSEAVSW_Devel/5   12/8/03 11:29a erickson
 * PR8813: throwFocus should only return true if focus is actually set.
 * This means testing both focusable() and focus()
 * 
 * Irvine_BSEAVSW_Devel/4   12/5/03 2:13p erickson
 * PR8813: Need to query not just if a widget is actually visible, but
 * whether it is potentially visible based on its parent's state. Added
 * visibleSet().
 * 
 * Irvine_BSEAVSW_Devel/3   12/4/03 9:38a erickson
 * PR8813: call GrSetWindowBackgroundColor AFTER setting the background
 * mode, otherwise it doesn't work in all cases
 * 
 * Irvine_BSEAVSW_Devel/2   11/6/03 5:11p erickson
 * PR8570: I'm not done, but I need to work on another bug.
 * 
 * Irvine_BSEAVSW_Devel/PR8570/1   11/6/03 11:40a erickson
 * I'm not done, but I need to work on another bug.
 * 
 * Irvine_BSEAVSW_Devel/23   4/30/03 9:09a erickson
 * setGeometry only resizes when changed
 * removed "trySharedParentFirst" logic
 * 
 * SanJose_Linux_Devel/22   11/21/02 1:59p erickson
 * default draw() now instatiates MPainter() in order to set up background
 * modes
 * 
 * SanJose_Linux_Devel/21   10/30/02 1:51p erickson
 * added tabOrder methods
 *
 * SanJose_Linux_Devel/20   10/29/02 9:55a erickson
 * removed clearParent() nonsense
 * made copyBackground() a real state instead of a copy
 *
 * SanJose_Linux_Devel/19   9/30/02 4:36p erickson
 * lots of painter/pixmap work to enable jpeg viewer in pvrgui
 *
 * SanJose_Linux_Devel\18   4/17/02 10:48a erickson
 * Fixed tabbing problem
 *
 * \main\SanJose_Linux_Devel\17   2/27/02 9:34a erickson
 * HD/dual pvr demo for China
 *
 ***************************************************************************/
#include "mwidget.h"
#include "bwin.h"
#include "mapplication.h"
#include "mpainter.h"
#include "mscrollview.h"
#include "mapp_priv.h"
#include <assert.h>
#define MAXINT 0x7fffffff

#define DEFAULT_WM_FLAGS GR_WM_PROPS_DONTCLIPBACKGROUND

#include <bstd.h>
BDBG_MODULE(mwidget);

MWidget::MWidget(MFrameBuffer *fb, const char *name) : MPaintDevice(fb) {
    init(NULL, MRect(0,0,10,10), NULL, name);
}

MWidget::MWidget(MFrameBuffer *fb, const MRect &rect, const char *text, const char *name) :
    MPaintDevice(fb)
{
    init(NULL, rect, text, name);
}

MWidget::MWidget(MWidget *parent, const char *name) : 
    /* don't allow possible null dereference here, because debugger won't find it */
    MPaintDevice(parent?parent->fb():NULL)
{
    init(parent, MRect(0,0,10,10), NULL, name);
}

MWidget::MWidget(MWidget *parent, const MRect &rect, const char *text, const char *name) :
    /* don't allow possible null dereference here, because debugger won't find it */
    MPaintDevice(parent?parent->fb():NULL)
{
    init(parent, rect, text, name);
}

void MWidget::init(MWidget *parent, const MRect &rect, const char *text, const char *name) {
    /* if NULL parent was passed, we'll catch it here. easy to find. */
    BDBG_ASSERT(fb());
    
    _backgroundColor = -1;
    _backgroundImage = NULL;
    _backgroundPixmap = NULL;
    _parent = parent;
    _tag = 0;
    _action = 0;
    _actionData = NULL;
    _name = name?name:text;
    _text = NULL;
    _focusable = false;
    _enabled = true;
    _style = NULL;
    _textColor = -1;
    _textColorFocus = -1;
    _dropshadow = 0;
    _wid = 0;
    setText(text);

    // create the window
    create(rect);

    // bookkeeping
    if (_parent) {
        _parent->addChild(this);
        _descendentWidgets = NULL;
    }
    else {
        fb()->_topWidgets.add(this);
        _descendentWidgets = new MList<MWidget>;
    }
    topLevelWidget()->addDescendent(this);
}

MWidget::~MWidget() {
    deleteChildren();
    destroy();

    // bookkeeping
    topLevelWidget()->removeDescendent(this);
    if (_parent) {
        _parent->removeChild(this);
    }
    else {
        delete _descendentWidgets;
        fb()->_topWidgets.remove(this);
    }
}

void MWidget::deleteChildren() {
    MWidget *child;
    while ((child = firstChild())) {
        delete child;
    }
}

void MWidget::reparent(MWidget *p, int ax, int ay) {
    if (parent()) {
        parent()->removeChild(this);
        parent()->repaint(clientRect());
        topLevelWidget()->removeDescendent(this);
    }
    else {
        delete _descendentWidgets;
        _descendentWidgets = NULL;
        fb()->_topWidgets.remove(this);
    }

    if (ax == -1)
        ax = x();
    if (ay == -1)
        ay = y();

    bwin_settings s;
    bwin_get((bwin_t)id(), &s);
    if (p) {
        s.parent = (bwin_t)p->id();
    }
    else {
        bwin_framebuffer_settings fb_settings;
        bwin_get_framebuffer_settings((bwin_framebuffer_t)fb()->id(), &fb_settings);
        s.parent = fb_settings.window;
    }
    s.x = ax;
    s.y = ay;
    bwin_set((bwin_t)id(), &s);
    //GrReparentWindow(id(), p?p->id():0, ax, ay);

    // automatically show if parent is hidden
    if (p && !p->visible())
        show();

    _parent = p;
    if (parent()) {
        parent()->addChild(this);
        parent()->repaint(clientRect());
        topLevelWidget()->addDescendent(this);

        /* don't allow widgets to switch framebuffers */
        assert(_parent->_fb == _fb);
    }
    else {
        fb()->_topWidgets.add(this);
        _descendentWidgets = new MList<MWidget>;
    }
}

int MWidget::textColor() const {
    return _textColor==-1?style()->color(MStyle::TEXT, enabled()) : _textColor;
}

int MWidget::textColorFocus() const {
    return _textColorFocus==-1?style()->color(MStyle::TEXT_FOCUSED, enabled()) : _textColorFocus;
}

void MWidget::create(const MRect &rect) {
    bwin_settings s;
    bwin_settings_init(&s);

    _rect = rect;
    if (_parent) {
        s.parent = (bwin_t)_parent->id();
    }
    else {
        bwin_framebuffer_settings fb_settings;
        bwin_get_framebuffer_settings((bwin_framebuffer_t)fb()->id(), &fb_settings);
        s.parent = fb_settings.window;
    }
    s.x = rect.x();
    s.y = rect.y();
    s.rect.width = rect.width();
    s.rect.height = rect.height();
    s.name = _name;

    _wid = (int)bwin_open(&s);

    // automatically show if parent is hidden
    if (parent() && !parent()->visible())
        show();
}

void MWidget::destroy() {
    hide();
    bwin_close((bwin_t)_wid);
    app()->widgetDestroyed(this);
}

bool MWidget::getChildrenZorderRange(int *min, int *max, MWidget *excludeChild) const
{
    bool result = false;
    MListItr<MWidget> itr(&_children);
    for (MWidget *child = itr.first(); child; child = itr.next()) {
        if (excludeChild == child)
            continue;
        if (!result) {
            *max = *min = child->zorder();
            result = true;
        }
        else {
            if (child->zorder() > *max)
                *max = child->zorder();
            else if (child->zorder() < *min)
                *min = child->zorder();
        }
    }
    return result;
}

int MWidget::zorder() const {
	bwin_settings s;
	bwin_get((bwin_t)id(), &s);
	return s.zorder;
}

void MWidget::setZOrder(int zorder) {
	bwin_settings s;
	bwin_get((bwin_t)id(), &s);
	s.zorder = zorder;
	bwin_set((bwin_t)id(), &s);
}

void MWidget::raise() {
	int min, max;
	bool result;
	if (parent())
		result = parent()->getChildrenZorderRange(&min, &max, this);
	else
		result = fb()->getChildrenZorderRange(&min, &max, this);
	
	if (result) {
		bwin_settings s;
		bwin_get((bwin_t)id(), &s);
		s.zorder = max + 1;
		bwin_set((bwin_t)id(), &s);
	}
}

void MWidget::lower() {
	int min, max;
	bool result;
	if (parent())
		result = parent()->getChildrenZorderRange(&min, &max, this);
	else
		result = fb()->getChildrenZorderRange(&min, &max, this);
	
	if (result) {
		bwin_settings s;
		bwin_get((bwin_t)id(), &s);
		s.zorder = min - 1; // this assumed we can go negative
		bwin_set((bwin_t)id(), &s);
	}
}

void MWidget::show() {
    bwin_settings s;
    bwin_get((bwin_t)id(), &s);
    s.visible = true;
    bwin_set((bwin_t)id(), &s);
}

void MWidget::hide() {
    bwin_settings s;
    bwin_get((bwin_t)id(), &s);
    s.visible = false;
    bwin_set((bwin_t)id(), &s);
}

bool MWidget::visible() const {
    if (parent() && !parent()->visible())
        return false;
    return visibleSet();
}

bool MWidget::visibleSet() const {
    bwin_settings s;
    bwin_get((bwin_t)id(), &s);
    return s.visible;
}

void MWidget::setGeometry(const MRect &r) {
    if (rect() != r) {
        _rect = r;
        bwin_settings s;
        bwin_get((bwin_t)id(), &s);
        s.x = x();
        s.y = y();
        s.rect.width = width();
        s.rect.height = height();
        bwin_set((bwin_t)id(), &s);
    }
}

void MWidget::setBackgroundColor(int color) {
    _backgroundColor = color;
    _backgroundImage = NULL;
    _backgroundPixmap = NULL;
    setTransparent(color == -1); /* this causes a repaint */
}

void MWidget::setBackgroundImage(const MImage *image) {
    _backgroundColor = -1;
    _backgroundImage = image;
    setTransparent(!image); /* this causes a repaint */
}

void MWidget::setBackgroundPixmap(const MPixmap *pixmap) {
    _backgroundColor = -1;
    _backgroundPixmap = pixmap;
    setTransparent(!pixmap); /* this causes a repaint */
}

bool MWidget::transparent() const {
    bwin_settings s;
    bwin_get((bwin_t)id(), &s);
    return s.transparent;
}

void MWidget::setTransparent(bool transparent) {
    bwin_settings s;
    bwin_get((bwin_t)id(), &s);
    s.transparent = transparent;
    //printf("setTransparent %s, %d\n", name(), s.transparent);
    bwin_set((bwin_t)id(), &s);
    if (transparent) {
        _backgroundColor = -1;
        _backgroundImage = NULL; /* we didn't allocate this */
        _backgroundPixmap = NULL; /* we didn't allocate this */
    }
    repaint();
}

int MWidget::relativeX(const MWidget *p) const {
    if (parent() != p)
        return x() + parent()->globalX();
    else
        return x();
}

int MWidget::relativeY(const MWidget *p) const {
    if (parent() != p)
        return y() + parent()->globalY();
    else
        return y();
}

MPoint MWidget::mapToGlobal(const MPoint &point) const {
    if (parent())
        return point + parent()->mapToGlobal(point);
    else
        return point;
}

bool MWidget::isScrollViewChild() const {
    return
        parent() &&
        parent()->name() &&
        !strcmp(parent()->name(), "__viewport") &&
        parent()->parent() &&
        parent()->parent()->parent() &&
        !strcmp(parent()->parent()->parent()->className(), "MScrollView");
}

bool MWidget::focus() {
    bool result;
    BDBG_MSG(("focus %s,%sfocusable,%svisible",
        name(), _focusable?"":"not ", visible()?"":"not "));
    if (_focusable) {
        if (visible()) {
            app()->setFocus(this);

            // If widget is child of scrollview, then make it visible
            // This is a bit of a hack. Any better ideas?
            if (isScrollViewChild())
            {
                BDBG_MSG(("focus calling MScrollView->showChild()"));
                ((MScrollView*)parent()->parent()->parent())->showChild(this);
            }
            result = true;
        }
        else
            // ignore the focus command
            result = false;
    }
    else if (parent()) {
        result = topLevelWidget()->focus();
    }
    else {
        // this is a top level window that isn't focusable.
        // find the first descendent that is.
        result = focusUpperLeft();
    }
    BDBG_MSG(("focus %s result=%d", name(), result));
    return result;
}

bool MWidget::focusUpperLeft() {
    MWidget *ul = findUpperLeft(true);
    return ul?ul->focus():false;
}

MWidget *MWidget::findUpperLeft(bool mustBeFocusable) const {
    int mindist = MAXINT;
    MWidget *ul = NULL;
    MListItr<MWidget> itr(&_children);

    BDBG_MSG(("findUpperLeft %d", mustBeFocusable));
    for (MWidget *child = itr.first(); child; child = itr.next()) {
        // if this isn't focusable and needs to be,
        // then find a child.
        bool canFocus = child->focusable() && child->visible() && child->enabled();
        if (mustBeFocusable && !canFocus) {
            // don't bother if it's further away
            if (child->x()*child->x()+child->y()*child->y() > mindist)
                continue;
            child = child->findUpperLeft(true);
            if (!child)
                continue;
        }
        int x = child->relativeX(this);
        int y = child->relativeY(this);
        int dist = x*x+y*y;
        BDBG_MSG(("child %s: %d (min %d)", child->name(), dist, mindist));
        if (dist < mindist) {
            mindist = dist;
            ul = child;
        }
    }
    return ul;
}

bool MWidget::hasFocus() const {
    return (app()->focusedWidget() == this);
}

bool MWidget::containsFocus() const {
    MWidget *fw = app()->focusedWidget();
    return fw && (fw == this || fw->hasParent((MWidget*)this));
}

bool MWidget::hasParent(MWidget *p) const {
    MWidget *search = parent();
    while (search) {
        if (search == p)
            return true;
        search = search->parent();
    }
    return false;
}

void MWidget::setFocusable(bool focusable) {
    _focusable = focusable;
}

void MWidget::repaint(const MRect &rect) {
    bwin_rect r = {rect.x(), rect.y(), rect.width(), rect.height()};
//  printf("MWidget::repaint %s: %d,%d,%u,%u\n",name(),rect.x(), rect.y(), rect.width(), rect.height());
//  printf("  %d,%d,%u,%u\n",_rect.x(), _rect.y(), _rect.width(), _rect.height());
    bwin_repaint((bwin_t)id(), &r);
}

void MWidget::setText(const char *t) {
    _text = t;
    if (_text)
        _textlen = strlen(_text);
    else
        _textlen = 0;
    if (id())
        repaint();
}

FIRE_EV_METHOD(MWidget,FocusIn)
FIRE_EV_METHOD(MWidget,FocusOut)

MFont MWidget::currentFont() const {
    if (!_font.isNull()) {
        return _font;
    }
    else if (parent())
        return parent()->currentFont();
    else
        return app()->defaultFont();
}

MWidget *MWidget::findChildByTag(int tag) {
    for (MWidget *child = firstChild(); child; child = nextChild())
        if (child->tag() == tag)
            return child;
    return NULL;
}

const MWidget *MWidget::topLevelWidget() const {
    if (parent())
        return parent()->topLevelWidget();
    else
        return (const MWidget *)this;
}

MWidget *MWidget::topLevelWidget() {
    if (parent())
        return parent()->topLevelWidget();
    else
        return this;
}

MWidget *MWidget::findDescendentById(int id) const {
    MListItr<MWidget> itr(_descendentWidgets);
    for (MWidget *d = itr.first(); d; d = itr.next())
        if (d->id() == id)
            return d;
    return NULL;
}

void MWidget::tab(MWidget *w, TabDirection dir) {
    if (_descendentWidgets->index(w) == -1) {
        w = _descendentWidgets->first();
    }
    if (!w) return;

    // now go forwards or backwards, detect a complete loop
    MWidget *start = w;
    if (dir == tabNext) {
        do {
            w = _descendentWidgets->next();
            if (!w)
                w = _descendentWidgets->first();
        } while ((w == this || !w->enabled() || !w->focusable() || !w->visible()) && w != start);
    }
    else if (dir == tabPrev) {
        do {
            w = _descendentWidgets->prev();
            if (!w)
                w = _descendentWidgets->last();
        } while ((w == this || !w->enabled() || !w->focusable() || !w->visible()) && w != start);
    }
    else {
        // 2D tabbing
        int lastdist = MAXINT;
        MWidget *last = NULL;
        // get the global coordinates of start
        int sx = start->globalX();
        int sy = start->globalY();
        int sr = sx+start->width();
        int sb = sy+start->height();
//      bool trySharedParentFirst = start->isScrollViewChild();
        bool sharedParent = false;
        for (MWidget *d = _descendentWidgets->first(); d; d = _descendentWidgets->next()) {
            if (d == start || !d->enabled() || !d->focusable() || !d->visible())
                continue;

            // get globalcoordinates for test widget
            int wx = d->globalX();
            int wy = d->globalY();
            int wr = wx+d->width();
            int wb = wy+d->height();

            // compute deltas
            int dx = sx-wx;
            int dy = sy-wy;

            //printf("try %d,%d,%d,%d,  %d,%d\n", wx,wy,wr,wb,dx,dy);
            switch (dir) {
            case tabLeft:
                dx = sx - wr;
                //if (dx < 0) continue;
                if (dx <= -(int)start->width()) continue;
                if (wb < sy || wy > sb)
                    dy = abs(dy) + start->height();
                break;
            case tabRight:
                dx = wx - sr;
                //if (dx < 0) continue;
                if (dx <= -(int)start->width()) continue;
                if (wb < sy && wy > sb)
                    dy = abs(dy) + start->height();
                break;
            case tabUp:
                dy = sy - wb;
                //if (dy < 0) continue;
                if (dy <= -(int)start->height()) continue;
                if (wr < sx || wx > sr)
                    dx = abs(dx) + start->width();
                break;
            case tabDown:
                dy = wy - sb;
                //if (dy < 0) continue;
                if (dy <= -(int)start->height()) continue;
                if (wr < sx || wx > sr)
                    dx = abs(dx) + start->width();
                break;
            }
            // scale down x because of non-square pixels
            //dx /= 2;
            dx = dx * 4 / 3;
            int dist = dx*dx + dy*dy;

            // children with the same parent get preference.
            // this is needed for mscrollview children.
#if 0
            if (trySharedParentFirst) {
                if (d->parent() == start->parent() && !sharedParent) {
                    // reset so that this one will succeed
                    lastdist = MAXINT;
                    sharedParent = true;
                }
                else if (d->parent() != start->parent() && sharedParent)
                    continue;
            }
#endif

            if (dist < lastdist) {
                lastdist = dist;
                last = d;
            }
        }
        w = last;
    }
    if (w != start && w) {
        //printf("tabbing to %s, %s at %d,%d\n", w->className(), w->name(), w->globalX(), w->globalY());
        w->focus();
    }
}

void MWidget::drawBackground(const MRect &cliprect) {

    if (_backgroundPixmap)
    {
        //drawing pixmaps is much faster than an image because there is no 
        //image decoding.
        MPainter ptr(this, cliprect);
        ptr.drawPixmap(*_backgroundPixmap, 0, 0, width(), height(), MPainter::eStretch);
    }
    else
    if (_backgroundImage) {
        MPainter ptr(this, cliprect);
/*      printf("Drawing background image: image dimensions = [ %d %d ]\n",  
            _backgroundImage->width(), _backgroundImage->height());*/
/*      printf("Drawing background image: dest_rect = [ %d %d %d %d ]\n", x(), y(), width(), height()); */
        ptr.drawImage(*_backgroundImage, 
/*          x(), y(), width(), height(), */
            0, 0, width(), height(), 
            MPainter::eStretch, MPainter::alLeft, MPainter::valTop);
    }
    else if (backgroundColor() != -1) {
        MPainter ptr(this, cliprect);
        ptr.setBrush(backgroundColor());
        ptr.fillRect(clientRect());
    }
    /* else, no background */
}

void MWidget::draw(const MRect &cliprect) {
    /* draw nothing */
}

void MWidget::drawNow() {
    if (visible()) {
        drawNow_internal();
        
        /* only sync after all drawNow_internals's are complete */
        MPainter ptr(this);
        ptr.sync();
    }
}

void MWidget::drawNow_internal() {
    drawBackground(clientRect());
    draw(clientRect());
    for (MWidget *child = firstChild(); child; child = nextChild()) {
        if (child->visibleSet())
            child->drawNow_internal();
    }
}

MStyle *MWidget::style() const {
    if (_style)
        return _style;
    else
        return app()->style();
}

bool MWidget::throwFocus() {
    MWidget *p = this;
    while ((p = p->parent())) {
        if (p->focusable() && p->focus()) {
            return true;
        }
    }
    return false;
}

void MWidget::addDescendent(MWidget *w) {
    _descendentWidgets->add(w);
}

void MWidget::removeDescendent(MWidget *w) {
    // I have to check because of ::deleteChildren
//  if (_descendentWidgets)
    _descendentWidgets->remove(w);
}

int MWidget::tabOrder() {
    return topLevelWidget()->_descendentWidgets->index(this);
}

void MWidget::setTabOrder(int order) {
    if (topLevelWidget()->_descendentWidgets->remove(this))
        topLevelWidget()->_descendentWidgets->insert(order, this);
}

int MWidget::findBackgroundColor() const
{
    int bgcolor = -1;
    const MWidget *widget = this;
    do {
        if ((widget->backgroundImage()) || (widget->backgroundPixmap()))
            break;
        bgcolor = widget->backgroundColor();
        widget = widget->parent();
    } while (bgcolor == -1 && widget);
    return bgcolor;
}

