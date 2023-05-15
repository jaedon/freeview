/***************************************************************************
 *     Copyright (c) 2001-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mwidget.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 8/8/06 9:51a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mwidget.h $
 * 
 * 5   8/8/06 9:51a erickson
 * PR23250: added setZOrder
 * 
 * 4   5/31/06 4:16p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/4   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 3   4/17/06 5:05p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/3   2/28/06 1:23p tokushig
 * add dropshadow attribute
 * 
 * SanDiego_Brutus_Skin/2   1/26/06 9:50a tokushig
 * add action get/set methods to allow each widget to be associated with a
 * generic action
 * 
 * SanDiego_Brutus_Skin/1   1/11/06 11:16a tokushig
 * added handling for text focus color and background pixmap image
 * 
 * 2   4/18/05 10:51a erickson
 * PR14593: added findBackgroundColor to walk the parent tree
 * 
 * 1   2/7/05 11:20p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/12   4/30/04 3:12p erickson
 * PR10883: implemented real raise/lower after discovering min/max zorder
 * 
 * Irvine_BSEAVSW_Devel/11   4/22/04 10:15p erickson
 * PR9107: need to sync after any drawNow
 * 
 * Irvine_BSEAVSW_Devel/10   4/6/04 3:46p bandrews
 * PR10368: Made focusUpperLeft() overridable
 * 
 * Irvine_BSEAVSW_Devel/9   2/26/04 11:03a erickson
 * PR9900: now using MListItr to allow const access to MList's
 *
 ****************************************************************************/
#ifndef MWIDGET_H
#define MWIDGET_H

#include "microwidgets.h"
#include "mpixmap.h"
#include "mfont.h"
#include "mfontmetrics.h"
#include "mstring.h"
#include "mstyle.h"
#include "mpaintdevice.h"
#include "mframebuffer.h"

BEGIN_EV(MWidget)
    EV_METHOD(MWidget,FocusIn);
    EV_METHOD(MWidget,FocusOut);
END_EV()

/**
Summary:
A widget is the most basic GUI object upon which all other GUI objects must be built.
Description:
A widget can paint itself. It has focus and so can receive keyboard input.
It is not abstract, which means that you can instantiate a widget
even though it can only paint it's background.
A widget has a window. As a window, it also has a parent or is a top level window. It also may have
0 or more children.

A widget has some commonly used properties (like text and font) which it doesn't use directly but many
of it's descendents use.

A widget's background can be in one of four states:
1. Background color.
2. Background pixmap.
3. Non-transparent background. This means that the widget draws in it's entire area, but doesn't use
    a background color or pixmap to do it. The windowing system uses this fact to reduce the number
    of repaints needed.
4. Transparent background. This means that the widget does not draw it's entire area and requires
    that the parent be repainted when it repaints itself.
**/
class MWidget :
    public MPaintDevice {
    friend class MApplication;

public:
    /* toplevel widget */
    MWidget(MFrameBuffer *fb, const char *name = NULL);
    MWidget(MFrameBuffer *fb, const MRect &rect, const char *text = NULL, const char *name = NULL);

    /* child widget */
    MWidget(MWidget *parent, const char *name = NULL);
    MWidget(MWidget *parent, const MRect &rect, const char *text = NULL, const char *name = NULL);
    virtual ~MWidget();

    int id() const {return _wid;}

    /* If not NULL, this widget is toplevel */
    MWidget *parent() const {return _parent;}

    const char *name() const {return _name;}
    void setName(const MString strName) { _name = strName; };

    virtual const char *className() const {return "MWidget";}

    /**
    * Returns the parent window at the top (i.e. that has no parent)
    */
    const MWidget *topLevelWidget() const;
    MWidget *topLevelWidget();

    void reparent(MWidget *parent, int x = -1, int y = -1);
    bool hasParent(MWidget *parent) const;

    /**
    * tag() can be used to identify a widget for any purpose.
    * See findChildByTag().
    */
    int tag() const {return _tag;}
    void setTag(int tag) {_tag = tag;}

    int action() const {return _action;}
    void setAction(int action) {_action = action;}

    void * actionData() const {return _actionData;}
    void setActionData(void * data) {_actionData = data;}

    // visibility, zorder, enabled
    virtual void show();
    virtual void hide();

    /* Returns true if the widget is actually visible. This means that
    that visibleSet() is true and parent()->visible() is true.
    */
    bool visible() const;

    /* Returns true if the widget will be visible if its parent is visible. */
    bool visibleSet() const;

    /* Z-order specifies the stacking of siblings relative to each other.
    It does not apply to parents and children; children are always on top of parents.
    It does not apply to children of different parents, only children of the same parent (siblings). */
    int zorder() const;
    void setZOrder(int zorder);
    /* increase this widget's z-order so that it is on top of any other sibling */
    void raise();
    /* decrease this widget's z-order so that it is on below any other sibling */
    void lower();
    
    /**
    Returns true if it finds at least one child widget with which is could get a zorder.
    Otherwise min and max are untouched.
    If excludeChild is specified, that child is excluded.
    **/
    bool getChildrenZorderRange(int *min, int *max, MWidget *excludeChild = NULL) const;

    bool enabled() const {return _enabled;}
    virtual void setEnabled(bool enabled) {_enabled = enabled; repaint();}

    int tabOrder();
    void setTabOrder(int order);

    /**
    * Returns this widget's personal style, but if NULL it returns
    * MApplication's style.
    */
    MStyle *style() const;
    /**
    * Set this widget's personal style.
    */
    void setStyle(MStyle *style) {_style = style; repaint();}

    virtual void setGeometry(const MRect &rect);
    void setGeometry(int x, int y, unsigned w, unsigned h) {setGeometry(MRect(x,y,w,h));}
    void setSize(unsigned w, unsigned h) {setGeometry(MRect(x(),y(),w,h));}
    void moveTo(int x, int y) {setGeometry(MRect(x,y,width(),height()));}
    void moveBy(int dx, int dy) {setGeometry(MRect(x()+dx,y()+dy,width(),height()));}
    const MRect &rect() const {return _rect;}
    unsigned width() const {return _rect.width();}
    unsigned height() const {return _rect.height();}
    int x() const {return _rect.x();}
    int y() const {return _rect.y();}
    int right() const {return _rect.right();}
    int bottom() const {return _rect.bottom();}

    int globalX() const {return relativeX(NULL);}
    int globalY() const {return relativeY(NULL);}
    int relativeX(const MWidget *parent) const;
    int relativeY(const MWidget *parent) const;
    MPoint mapToGlobal(const MPoint &point) const;

    /* If you set a background color, this will be drawn before draw() is called.
    Setting -1 is the same as setTransparent(true).
    */
    void setBackgroundColor(int color);
    int backgroundColor() const {return _backgroundColor;}
    
    /* Get the background color from this widget or any parent. If a background
    image is encountered first, -1 is returned.
     */
    int findBackgroundColor() const;

    /* If you set a background pixmap, this will be drawn before draw() is called.
    Setting NULL is the same as settings setTransparent(true).
    The MWidget does not copy the MPixmap. It must stay allocated.
    */
    void setBackgroundImage(const MImage *image);
    const MImage *backgroundImage() const {return _backgroundImage;}

    void setBackgroundPixmap(const MPixmap *pixmap);
    const MPixmap *backgroundPixmap() const {return _backgroundPixmap;}

    /* By default, you are transparent. If you know that you will paint your
    entire region, set this to false. Then parents won't be redrawn.
    If you call setBackgroundColor (not -1) or setBackgroundPixmap (non-null),
    it will setTransparent(false).
    */
    void setTransparent(bool transparent);
    bool transparent() const;

    /**
    * There are so many widgets that have some main text,
    * I've put it in the base.
    */
    virtual void setText(const char *t);
    const char *text() const {return _text;}
    void setTextColor(int tc) {_textColor = tc;}
    void setTextColorFocus(int tc) {_textColorFocus = tc;}
    int textColor() const;
    int textColorFocus() const;
    void setFont(const MFont &font) {_font = font;}
    MFont font() const {return _font;}
    MFont currentFont() const;
    MFontMetrics fontMetrics() const {return MFontMetrics(currentFont());}
    int dropShadow() const {return _dropshadow;}
    void setDropShadow(int dropshadow) {_dropshadow = dropshadow;}

    virtual bool focus();
    virtual bool focusUpperLeft();
    bool hasFocus() const;

    /**
    * Returns true if widget, or any descendent, has focus
    */
    bool containsFocus() const;

    void setFocusable(bool focusable);
    bool focusable() const {return _focusable;}

    void repaint() {repaint(clientRect());}
    void repaint(const MRect &rect);
    virtual void focusRepaint() {repaint(clientRect());}

    int totalChildren() const {return _children.total();}
    MWidget *firstChild() {return _children.first();}
    MWidget *nextChild() {return _children.next();}
    MWidget *lastChild() {return _children.last();}
    MWidget *child(int index) {return _children.at(index);}
    MWidget *findChildByTag(int tag);
    void deleteChildren();

    /**
    * Find the descendent widget that is closest to the coordinate. Use -1 if you
    * don't care. To find the widget closest to:
    *
    *   upperleft   0,0,-1,-1.
    *   left        0,-1,-1,-1
    *   top         -1,0,-1,-1
    *   bottom      -1,-1,-1,height()
    *   center      0,0,width(),height()
    */
    //MWidget *findClosestChild(int left, int top, int right, int bottom, bool mustBeFocusable = false);
    MWidget *findUpperLeft(bool mustBeFocusable = false) const;

    /**
    * Draw apart from message loop. This is immediate. It calls drawBackground and draw for
    * this window and all children.
    */
    void drawNow();

    enum TabDirection {tabUp,tabDown,tabRight,tabLeft,tabNext,tabPrev};

protected:
    int _wid;
    MString _name;
    MRect _rect;
    bool _focusable;
    int _tag;
    int _action;
    void *_actionData;
    MStyle *_style;
    bool _enabled;

    MString _text;
    int _textlen;
    MFont _font;
    int _dropshadow;

    void addChild(MWidget *child) {_children.add(child);}
    void removeChild(MWidget *child) {_children.remove(child);}
    MList<MWidget> _children;

    virtual void drawBackground(const MRect &cliprect);
    virtual void draw(const MRect &cliprect);
    virtual bool keyUp(int ascii, int modifiers, int scancode) {return false;}
    virtual bool keyDown(int ascii, int modifiers, int scancode) {return false;}

    void create(const MRect &rect);
    void destroy();
    bool isScrollViewChild() const;

    /**
    * Call this when you can't receive focus, but don't know where to put it.
    * It walks through parents. Returns true if someone accepted focus.
    * WARNING: Don't call throwFocus from inside a focus function! This can easily
    * lead to infinite loops. An example of when to use throwFocus is if you 
    * delete a data item and can no longer hold focus.
    */
    bool throwFocus();

    // all descendents are stored in the topLevel() widget.
    void addDescendent(MWidget *w);
    void removeDescendent(MWidget *w);
    MWidget *findDescendentById(int id) const;
    //TODO: allow toplevel widgets to be linked together in a tab order.
    void tab(MWidget *w, TabDirection dir);
    MList<MWidget> *_descendentWidgets;

    SUPPORT_EV(MWidget)
    SUPPORT_EV_METHOD(FocusIn)
    SUPPORT_EV_METHOD(FocusOut)

private:
    int _textColor;
    int _textColorFocus;
    int _backgroundColor;
    const MImage *_backgroundImage;
    const MPixmap *_backgroundPixmap;
    MWidget *_parent;
    void init(MWidget *parent, const MRect &rect, const char *text, const char *name);
    void drawNow_internal();
};

#endif //MWIDGET_H
