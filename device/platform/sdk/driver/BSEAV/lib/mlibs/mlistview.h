/***************************************************************************
 *     Copyright (c) 2002-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mlistview.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 8/25/06 1:25p $
 *
 * Module Description: List of widgets in a scroll view
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/mlibs/mlistview.h $
 * 
 * 3   8/25/06 1:25p tokushig
 * PR22347: in non-skinned brutus, images w/o embedded thumbnails will
 * display full image as thumb.  if using a skin, small images w/o
 * embedded thumbnails will display full image as thumb but large images
 * w/o embedded thumbnails will display a placeholder icon instead
 * (images are currently considered large if they have more than 640 *
 * 480 = 307200pixels.)  thumbnail please wait popup was replaced by
 * sequential drawing of thumbs which provides better feedback to users.
 * implemented image caching on thumbs (default: last 30) and for full
 * screen image view (default: last 3).
 * 
 * SanDiego_Brutus_Skin/2   8/23/06 4:58p tokushig
 * added new thumbnail drawing logic which chooses between embedded thumb,
 * full sized image, or icon placeholder.  thumbs are also drawn
 * immediately to give user feedback
 * 
 * 2   6/6/06 11:49a tokushig
 * PR20349: ensure all dynamically allocated bwin framebuffers are
 * destroyed before deleting display
 * 
 * SanDiego_Brutus_Skin/1   6/6/06 11:13a tokushig
 * add destructor to delete internally allocated thumbnail pixmap _cache
 * 
 * 1   2/7/05 11:15p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/2   1/29/04 2:26p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/8   5/12/03 4:42p erickson
 * added index() method
 * 
 * Irvine_BSEAVSW_Devel/7   4/30/03 9:07a erickson
 * added selected feature
 * fixed layout
 * 
 * SanJose_Linux_Devel/6   10/29/02 9:57a erickson
 * added remove(int index) method
 * 
 * SanJose_Linux_Devel/5   10/18/02 11:15a erickson
 * when updating label/description/icon, update the layout...but
 * only if necessary
 * 
 * SanJose_Linux_Devel/4   9/30/02 4:35p erickson
 * lots of painter/pixmap work to enable jpeg viewer in pvrgui
 *
 ***************************************************************************/
#ifndef MLISTVIEW_H
#define MLISTVIEW_H

#include "mscrollview.h"
#include "mpixmap.h"
#include "mcache.h"

class MListView;

BEGIN_EV(MListView)
    EV_METHOD(MListView, Click)
    EV_METHOD(MListView, Layout) // called after MListViewItem's are arranged
END_EV()

class MListViewItem : public MPushButton {
public:
    MListViewItem(MListView *v, MWidget *parent, const char *name = NULL);
    ~MListViewItem();
    const char *className() const {return "MListViewItem";}

    MString &label() {return _label;}
    MString &description() {return _description;}
    const MImage *icon() {return _image;}

    void setLabel(const char *label);
    void setDescription(const char *description);
    void setIcon(const MImage *image);

    MListView *view() const {return _view;}

    int measureWidth() const;
    virtual void focusRepaint();

protected:
    MString _label;
    MString _description;
    const MImage *_image;
    MListView *_view;

    void draw(const MRect &cliprect);
};

class MListView : public MScrollView, public MButtonEventHandler {
    friend class MListViewItem;

public:
    MListView(MWidget *parent, const MRect &rect, const char *name = NULL);
    ~MListView();

    MListViewItem *add();
    MListViewItem *insert(int index);
    void remove(MListViewItem *item);
    void remove(int index);
    void clear();
    int total() const {return _items.total();}
    MListViewItem *first() {return _items.first();}
    MListViewItem *next() {return _items.next();}

    enum View {ListView, DetailedListView, IconView};
    void setView(View view);
    View view() const {return _view;}
    void setIconWidth(int iconwidth, int iconborder);
    void setNoThumbImage(MImage * iconNoThumbnail);
    MImage * noThumbImage() { return _iconNoThumbnail; };

    MListViewItem *item(int index);
    int index(MListViewItem *item);
    MListViewItem *current();
    int currentIndex();

    void setSelected(MListViewItem *child);
    MListViewItem *selected() const {return _selected;}

    MCache<MPixmap> * cache() { return &_cacheThumbs; };

    // events
    void onClick(MButton *sender);

protected:
    MAutoList<MListViewItem> _items;
    View _view;
    int _iconwidth, _iconborder;
    MListViewItem *_selected;
    MCache<MPixmap> _cacheThumbs;
    MImage * _iconNoThumbnail;

    virtual MListViewItem *createItem();

    // overrides
    void layout();

    SUPPORT_EV(MListView)
    SUPPORT_EV_METHOD(Click)
    SUPPORT_EV_METHOD(Layout)
};

#endif
