/***************************************************************************
 *     Copyright (c) 2002-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mlistbox.h $
 * $brcm_Revision: 7 $
 * $brcm_Date: 9/15/08 12:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mlistbox.h $
 * 
 * 7   9/15/08 12:40p tokushig
 * PR43426: merge to main
 * 
 * BRUTUS_97405_97043/2   8/21/08 4:06p tokushig
 * PR43426: added scrolling button listboxes to both homescreen and
 * videoscreen.  separated buttonlistbox class to common file for reuse
 * (buttonlistbox.cpp/h)
 * 
 * BRUTUS_97405_97043/1   8/5/08 6:00p tokushig
 * PR43426: remove button list scrollbar and add arrow indicators
 * 
 * 6   2/27/07 4:16p erickson
 * PR24971: hooked up scrollbar Scroll event for debug
 * 
 * 5   5/31/06 4:18p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/2   5/16/06 11:33a tokushig
 * merge from main
 * 
 * 4   5/11/06 5:25p erickson
 * PR20951: removed duplicate textColor variables. equivalent logic is
 * available in MWidgets and is initialized correctly.
 * 
 **************************************************************************/
#ifndef MLISTBOX_H
#define MLISTBOX_H

#include "mwidget.h"
#include "mstring.h"
#include "mpainter.h"
#include "mscrollbar.h"
#include "mstringlist.h"

class MListBox;

BEGIN_EV(MListBox)
    EV_METHOD(MListBox,ItemClicked);
    EV_METHOD(MListBox,CurrentItemChanged);
    EV_METHOD(MListBox,InsertItem);
    EV_METHOD(MListBox,DeleteItem);
END_EV()

class MListBox : public MWidget, public MScrollBarEventHandler {
public:
    MListBox(MWidget *parent, const char *name = NULL);
    MListBox(MWidget *parent, const MRect &rect, int scrollBarWidth = 15, const char *name = NULL);

    // values
    void add(const char *text);
    void insert(int index, const char *text);
    void change(int index, const char *text);
    const char *item(int index);
    void remove(int index);
    void clear();
    int total() const {return _values.total();}
    int find(const char *text);

    // layout
    int indent() const {return _indent;}
    void setIndent(int indent) {_indent = indent;}

    /* currentIndex is the absolute index of the current selection.
    The relative index is currentIndex - topIndex */
    int currentIndex() const {return _currentIndex;}
    void setCurrentIndex(int i, bool changeFocus = true);

    /* focusIndex is the absolute index of the currently focused selection */
    int focusIndex();
    bool setFocus(int index);

    int topIndex() const {return _topIndex;}
    void setTopIndex(int topIndex);
    int totalVisible() const {return _items.total();}
    void setTotalVisible(int totalVisible);

    // behavior
    void setHighlightCurrent(bool highlightCurrent);
    void setRequireClickToMakeCurrent(bool requireClickToMakeCurrent);

    void beginUpdates();
    void endUpdates();

    //events
    void onScroll(MScrollBar *self);

    void setScrollBar(MScrollBar *bar, int width);
    MScrollBar *scrollBar() const {return _bar;}
    void captureKeys(bool up, bool down) {
        _captureUp = up;
        _captureDown = down;
    }

    bool focus();

protected:
    class MListItem : public MWidget {
        friend class MListBox;
    public:
        MListItem(MListBox *listbox, const char *name = NULL);

        void click();

        // overrides
        bool focus();
    protected:
        void setIndex(int index) {_index = index;}
        // overrides
        void draw(const MRect &cliprect);
        bool keyDown(int ascii, int modifiers, int scancode);

        int _index; /* relative index */
    };
    friend class MListItem;
    virtual MListItem *createItem();
    void click(int index /* relative index */);

    // overrides
    void setGeometry(const MRect &rect);
    virtual void layout();

    int _indent;
    MStringList _values;
    MList<MListItem> _items;
    int _topIndex;
    MScrollBar *_bar;
    MScrollBar _defaultBar;
    bool _captureUp, _captureDown, _highlightCurrent, _requireClickToMakeCurrent;

private:
    void resetData();
    void init(int scrollBarWidth);
    bool _updating;
    int _currentIndex;

    SUPPORT_EV(MListBox)
    SUPPORT_EV_METHOD(ItemClicked)
    SUPPORT_EV_METHOD(CurrentItemChanged)
    SUPPORT_EV_METHOD(InsertItem)
    SUPPORT_EV_METHOD(DeleteItem)
};

///////////////////////////////////////

#define LISTBOX ((MListBox*)parent())

class MColumnListBox : public MListBox {
public:
    MColumnListBox(MWidget *parent, const char *name = NULL);
    MColumnListBox(MWidget *parent, const MRect &rect, int scrollBarWidth = 15, const char *name = NULL);

    void setDelimeter(char delim) {_delim = delim;}
    void setFocusImage(MImage * pImg = NULL) {_imgFocus = pImg;}

    void addColumn(int width,
        MPainter::Alignment al = MPainter::alLeft,
        MPainter::VAlignment val = MPainter::valCenter) {_cols.add(new Column(width,al,val));}
    void removeColumn(int index) {_cols.remove(index);}
    int totalColumns() const {return _cols.total();}

protected:
    struct Column {
        int size;
        MPainter::Alignment al;
        MPainter::VAlignment val;
        Column(int s, MPainter::Alignment a, MPainter::VAlignment va) {
            size=s;al=a;val=va;
        }
    };
    MAutoList<Column> _cols;
    char _delim;

    MImage * _imgFocus;

    class MColumnListItem : public MListItem {
    public:
        MColumnListItem(MListBox *listbox, const char *name = NULL) :
            MListItem(listbox, name) {}
    protected:
        virtual void draw(const MRect &cliprect);
        virtual void drawColumn(
            MPainter &ptr, int col, const char *text, int textlen, const MRect &rect,
            MPainter::Alignment al, MPainter::VAlignment val);
    };
    friend class MColumnListItem;
    MListItem *createItem();
};

#endif //MLISTBOX_H
