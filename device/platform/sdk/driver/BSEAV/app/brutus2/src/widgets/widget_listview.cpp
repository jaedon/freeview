/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_listview.cpp $
* $brcm_Revision: 12 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_listview.cpp $
* 
* 12   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 11   10/7/12 4:55p tokushig
* SW7231-749: add sort capability for list based on button text
* 
* 10   8/8/12 5:58p tokushig
* SW7231-749: add support for hiding listview items in menu
* 
* 9   8/2/12 2:53p tokushig
* SW7231-749: add clear method for listview and use in menu widget
* 
* 8   5/25/12 12:31a tokushig
* SW7231-749: added listview container support for buttons with sub-
* widgets being added to the list view.  also added listview support for
* popup list widgets.
* 
* 7   5/4/12 2:04p tokushig
* SW7231-749: add buffers screen, checkbox widget, and new fonts
* 
* 6   4/19/12 7:40p tokushig
* SW7231-749: add support for non scrolling listview.  these will auto
* resize to fit their content.
* 
* 6   4/19/12 7:38p tokushig
* SW7231-749: add support for non scrolling listview.  these will auto
* resize to fit their content.
* 
* 5   4/18/12 5:15p jrubio
* SW7346-644: add remove (element) instead of just remove (index)
* 
* 4   4/15/12 9:45p tokushig
* SW7231-749: support re-layout of listview and menu widgets when resized
* 
* 3   4/14/12 7:26p tokushig
* SW7231-749: change the way click/keydown callbacks execute and modify
* way listview widget redirects keydown events to itself (so it can do
* scrolling internally)
* 
* 2   4/14/12 5:24p tokushig
* SW7231-749: refactor widget classes to hide keypress/click callback
* mechanism and simplify screen creation.  also added new menu widget.
* 
* 1   4/13/12 7:26p tokushig
* SW7231-749: added scrolling list view
* 
* 1   4/11/12 12:13p tokushig
* SW7231-749: initial creation
* 
* 
*********************************************************************************/

#include "widget_listview.h"
#include "widget_button.h"
#include "string.h"
#include "bdbg.h"

BDBG_MODULE(widget_listview);

CWidgetListView::CWidgetListView(bwidget_engine_t engine, 
                                 CWidgetBase * pParentWidget, 
                                 MRect geometry,
                                 bwin_font_t font,
                                 bwin_t parentWin) :
    CWidgetLabel(engine, pParentWidget, geometry, font, parentWin),
    _separatorSize(1),
    _scroll(true),
    _lockFocus(false)
{
    blabel_settings labelSettings;

    _widgetList.clear();

    blabel_get(_widget, &labelSettings);
    labelSettings.bevel                      = 3;
    labelSettings.text_color                 = 0xFF82C345;
    labelSettings.text_justify_horiz         = bwidget_justify_horiz_left;
    labelSettings.text_margin                = 5;
    labelSettings.background_color           = 0xFFCCCCCC;
    blabel_set(_widget, &labelSettings);

    /* force key press callbacks to come to this object (not the parent).
       we need to do this to handle scrolling within the list view */
    {
        blabel_get(_widget, &labelSettings);
        labelSettings.widget.data                = this;
        blabel_set(_widget, &labelSettings);

        _pParent = this;
    }
}

CWidgetListView::~CWidgetListView()
{
    _widgetList.clear();
}

void CWidgetListView::show(bool bShow, bool bLockFocus)
{
    show(bShow);
    lockFocus(bLockFocus);
}

void CWidgetListView::show(bool bShow)
{
    CWidgetLabel::show(bShow);
    if (true == bShow)
    {
        layout();
    }
    else
    {
        lockFocus(false);
    }
}

/* adds a button widget to this list view.  we require button widgets because all widgets
   must be selectable (but since button widgets can simulate label widgets this should be
   a non-restrictive requirment.  note that you must still call layout() to properly layout
   the listview after adding new elements */
void CWidgetListView::add(CWidgetButton * pButton)
{
    CWidgetListItem * pWidgetItem       = NULL;
    bwin_t            itemWin           = NULL;
    bwin_t            listWin           = NULL;
    bwin_settings     bwinSettings;
    bbutton_settings  buttonSettings;
    eRet              ret               = eRet_Ok;

    BDBG_ASSERT(NULL != pButton);

    pButton->setTextMargin(5);
    pButton->setBevel(0);

    /* hide new widget - layout() will show */
    pButton->show(false);

    pWidgetItem = new CWidgetListItem(this, pButton);
    CHECK_PTR_GOTO("unable to allocate widget list item", pWidgetItem, ret, eRet_OutOfMemory, error);
    _widgetList.add(pWidgetItem);

    layout();

error:
    return;
}

void CWidgetListView::remove(CWidgetButton * pButton)
{
    CWidgetListItem             * pWidgetItem      = NULL;
    MListItr <CWidgetListItem>    itr(&_widgetList);

    BDBG_ASSERT(_widgetList.total() > 0);

    /* search list for matching button and remove it. */
    for (pWidgetItem = itr.first(); pWidgetItem; pWidgetItem = itr.next())
    {
        if (pWidgetItem->getButton() == pButton)
        {
            _widgetList.remove(pWidgetItem);
            pButton->show(false);
            break;
        }
    }

    if (false == _scroll)
    {
        /* scrolling is off so resize to fit */
        layout();
    }
}

CWidgetButton * CWidgetListView::remove(uint16_t index)
{
    CWidgetListItem  * pWidgetItem = NULL;
    CWidgetButton    * pButton     = NULL;
    bbutton_settings   buttonSettings;
    BDBG_ASSERT(_widgetList.total() > index);

    /* _widgetList is an auto list so we must retrieve/save
       button pointer before removing. */
    pWidgetItem = _widgetList.get(index);
    pButton     = pWidgetItem->getButton();
    _widgetList.remove(index);

    pButton->show(false);

    if (false == _scroll)
    {
        /* scrolling is off so resize to fit */
        layout();
    }

    return pButton;
}

static int compareWidgetListItems(CWidgetListItem * item1, CWidgetListItem * item2)
{
    return strcmp(item1->getButton()->getText().s(), item2->getButton()->getText().s());
}

eRet CWidgetListView::sort()
{
    eRet ret = eRet_Ok;

    _widgetList.sort(compareWidgetListItems);
    return ret;
}

void CWidgetListView::clear()
{
    _widgetList.clear();
}

/* layout list view using widgets in widget list.  start with 'index' and go 'topDown' (or bottomUp).
   note: if ALL of the list items fit in the listview, then layout() will force index=0 and topDown=true */
eRet CWidgetListView::layout(int index, bool topDown)
{
    eRet                       ret                = eRet_Ok;
    CWidgetListItem          * pListItem          = NULL;
    MRect                      listRect           = getGeometry();
    MRect                      itemRect;
    blabel_settings            listViewSettings;
    int                        margin             = 0;
    uint16_t                   openSlotX          = 0;
    uint16_t                   openSlotY          = 0;
    MListItr <CWidgetListItem> itr(&_widgetList);

    BDBG_MSG(("layout() index:%d topDown:%d", index, topDown));

    /* determine margin (bevel size) for list view - contents must shrink appropriately */
    blabel_get(getWidget(), &listViewSettings);
    margin = listViewSettings.bevel;

    /* if scrolling is OFF, resize list view to fit all buttons */
    if ((false == _scroll) || (true == _autoResize))
    {
        uint16_t totalHeight = 0;
        MRect    rectGeom;

        /* find height of contents */
        for (pListItem = itr.first(); pListItem; pListItem = itr.next())
        {
            rectGeom = pListItem->getButton()->getGeometry();
            totalHeight += rectGeom.height() + getSeparatorSize();
        }

        /* grow for margins*/
        totalHeight += margin * 2;

        /* resize listview */
        listRect = getGeometry();
        listRect.setHeight(totalHeight);
        CWidgetLabel::setGeometry(listRect);
    }
    
    /* set listRect to contain the available space in list view */
    listRect = getGeometry();
    listRect.setX(margin);
    listRect.setY(margin);
    listRect.grow(margin * 2 * -1); /* shrink for margins */

    /* sanity check!
       before we begin layout, we must determine if ALL the widgets will
       fit in the listview. if so, then we will force topDown */
    {
        uint16_t totalHeight = 0;
        MRect    rectGeom;
        for (pListItem = itr.first(); pListItem; pListItem = itr.next())
        {
            rectGeom = pListItem->getButton()->getGeometry();
            totalHeight += rectGeom.height() + getSeparatorSize();
        }

        if (listRect.height() > totalHeight)
        {
            /* all items will fit in list view so force topDown layout */
            topDown = true;
            index   = 0;
        }
    }

    if (true == topDown)
    {
        /* layout view top down */
        for (pListItem = itr.at(index); pListItem; pListItem = itr.next())
        {
            /* adjust new item's geometry to fit first available spot in listRect */
            itemRect = pListItem->getButton()->getGeometry();
            itemRect.setX(listRect.x());
            itemRect.setY(listRect.y());
            itemRect.setWidth(listRect.width());

            if ((0 < listRect.height()) && (listRect.intersects(itemRect)))
            {
                BDBG_MSG(("widget fits in listview!"));
                /* item fits so apply geometry changes to item widget */
                pListItem->getButton()->setGeometry(itemRect);
                pListItem->getButton()->show(true);
                
                /* shrink available space */
                listRect.setY(listRect.y() + itemRect.height());
                listRect.setBottom(listRect.bottom() - itemRect.height());

                /* adjust available space for separator */
                listRect.setY(listRect.y() + getSeparatorSize());
                listRect.setBottom(listRect.bottom() - getSeparatorSize());
            }
            else
            {
                BDBG_MSG(("widget does NOT fit in listview!"));
                pListItem->getButton()->show(false);
            }
        }
    }
    else
    {
        /* layout view bottom up */
        for (pListItem = itr.at(index); pListItem; pListItem = itr.prev())
        {
            /* adjust new item's geometry to fit last available spot in listRect */
            itemRect = pListItem->getButton()->getGeometry();
            itemRect.setX(listRect.x());
            itemRect.setY(listRect.bottom() - itemRect.height());
            /* adjust Y for separator */
            itemRect.setY(itemRect.y() - getSeparatorSize());
            itemRect.setWidth(listRect.width());

            if (listRect.intersects(itemRect))
            {
                BDBG_MSG(("widget fits in listview!"));
                /* item fits so apply geometry changes to item widget */
                pListItem->getButton()->setGeometry(itemRect);
                pListItem->getButton()->show(true);
                
                /* shrink available space */
                listRect.setBottom(listRect.bottom() - itemRect.height());

                /* adjust available space for separator */
                listRect.setBottom(listRect.bottom() - getSeparatorSize());
            }
            else
            {
                BDBG_MSG(("widget does NOT fit in listview!"));
                pListItem->getButton()->show(false);
            }
        }
    }

    return ret;
}

void CWidgetListView::setGeometry(MRect geometry)
{
    CWidgetLabel::setGeometry(geometry); /* call base class */
    layout();
}

/* returns index of item in list (or it's ancestor) that corresponds with the given widget.
   otherwise returns -1 */
int CWidgetListView::getItemListIndex(bwidget_t widget)
{
    CWidgetListItem * pListItem = NULL;
    int               index     = -1;

    BDBG_ASSERT(NULL != widget);

    MListItr <CWidgetListItem> itr(&_widgetList);
    for (pListItem = itr.first(); pListItem; pListItem = itr.next())
    {
        if (widget == pListItem->getButton()->getWidget())
        {
            /* widget matches pListItem*/
            break;
        }
        else
        {
            bwidget_settings bwidgetSettings;
            bwin_t           currentWin;
            bwin_t           parentWin;

            bwidget_get_settings(pListItem->getButton()->getWidget(), &bwidgetSettings);
            currentWin = bwidgetSettings.win;
            bwidget_get_settings(widget, &bwidgetSettings);
            parentWin = bwidgetSettings.window.parent;

            /* pListItem does not match given widget - check ancestors.
               the given widget may be an ancestor of pListItem.  this can
               occur if the listview is populated with buttons that have
               focusable sub widgets. */
            while ((NULL != parentWin) && (currentWin != parentWin))
            {
                bwin_settings bwinSettings;

                bwin_get(parentWin, &bwinSettings);
                parentWin = bwinSettings.parent;
            }

            if (NULL != parentWin)
            {
                /* ancestor of widget matches current pListItem */
                break;
            }
        }
    }

    if (NULL != pListItem)
    {
        /* pListItem matches widget or ancestor of widget */
        index = itr.index(pListItem);
    }

    return index;
}

bool CWidgetListView::isFirstVisible(CWidgetButton * pWidget)
{
    bool                         visible            = false;
    MListItr <CWidgetListItem>   itr(&_widgetList);
    CWidgetListItem            * pWidgetFirst       = NULL;

    BDBG_ASSERT(NULL != pWidget);

    for (pWidgetFirst = itr.first(); pWidgetFirst; pWidgetFirst = itr.next())
    {
        if (pWidgetFirst->getButton()->isVisible())
        {
            break;
        }
    }

    if ((NULL != pWidgetFirst->getButton()) && (pWidget == pWidgetFirst->getButton()))
    {
        visible = true;
    }

    return visible;
}

bool CWidgetListView::isLastVisible(CWidgetButton * pWidget)
{
    bool                         visible            = false;
    MListItr <CWidgetListItem>   itr(&_widgetList);
    CWidgetListItem            * pWidgetLast        = NULL;

    BDBG_ASSERT(NULL != pWidget);

    for (pWidgetLast = itr.last(); pWidgetLast; pWidgetLast = itr.prev())
    {
        if (pWidgetLast->getButton()->isVisible())
        {
            break;
        }
    }

    if ((NULL != pWidgetLast->getButton()) && (pWidget == pWidgetLast->getButton()))
    {
        visible = true;
    }

    return visible;
}


bool CWidgetListView::isFullyVisible(CWidgetButton * pWidget)
{
    BDBG_ASSERT(NULL != pWidget);

    if (false == pWidget->isVisible())
    {
        return false;
    }

    /* pWidget is already visible so check to make sure it is
       fully contained within the list view (could be only partially shown) */
    MRect listRect;
    MRect itemRect;
    listRect = getGeometry();
    itemRect = pWidget->getGeometry();

    BDBG_MSG(("listRect - x:%d y:%d w:%d h:%d", listRect.x(), listRect.y(), listRect.width(), listRect.height()));
    BDBG_MSG(("itemRect - x:%d y:%d w:%d h:%d", itemRect.x(), itemRect.y(), itemRect.width(), itemRect.height()));

    return (listRect.contains(itemRect) ? true : false);
}

void CWidgetListView::setScroll(bool scroll)
{
    _scroll = scroll;
    if (false == _scroll)
    {
        layout();
    }
}

void CWidgetListView::setAutoResize(bool autoResize)
{
    _autoResize = autoResize;
    if (true == _autoResize)
    {
        layout();
    }
}

eRet CWidgetListView::onKeyDown(bwidget_t widget, bwidget_key key)
{
    eRet      ret           = eRet_NotSupported;  /* assume key not consumed */
    bwidget_t focusedWidget = NULL;

    focusedWidget = bwidget_get_focus(getEngine());

    switch (key)
    {
    case bwidget_key_up:
    {
        /* handle scrolling UP within list view */
        MListItr <CWidgetListItem>   itr(&_widgetList);
        int                          index              = -1;
        CWidgetListItem            * pWidget            = NULL;
        CWidgetListItem            * pWidgetPrev        = NULL;

        /* find current widget in list view that corresponds to focused widget */
        index = getItemListIndex(focusedWidget);
        if (0 <= index)
        {
            /* widget is in list view so get previous */
            pWidget             = itr.at(index);
            pWidgetPrev         = itr.prev();
            if (NULL != pWidgetPrev)
            {
                /* there is a previous widget to move focus to */
                if (isFirstVisible(pWidget->getButton()) || isFirstVisible(pWidgetPrev->getButton()))
                {
                    /* widget or next widget is at top of list view - may have to layout() */
                    if (false == isFullyVisible(pWidgetPrev->getButton()))
                    {
                        /* layout starting with previous widget (if exists) */
                        layout(itr.index(pWidgetPrev), true);
                    }
                }

                pWidgetPrev->getButton()->setFocus();

                /* consume key */
                ret = eRet_Ok; 
            }
            else
            {
                /* no prev widget - if focus locked, consume key */
                if (true == _lockFocus)
                {
                    ret = eRet_Ok;
                }
            }
        }
    }
        break;

    case bwidget_key_down:
    {
        /* handle scrolling DOWN within list view */
        MListItr <CWidgetListItem>   itr(&_widgetList);
        int                          index              = -1;
        CWidgetListItem            * pWidget            = NULL;
        CWidgetListItem            * pWidgetNext        = NULL;

        /* find current widget in list view */
        index = getItemListIndex(focusedWidget);

        if (0 <= index)
        {
            /* widget is in list view so get next */
            pWidget            = itr.at(index);
            pWidgetNext        = itr.next();
            if (NULL != pWidgetNext)
            {
                /* there is a next widget to move focus to */
                if (isLastVisible(pWidget->getButton()) || isLastVisible(pWidgetNext->getButton()))
                {
                    /* widget is at bottom of list view */
                    if (false == isFullyVisible(pWidgetNext->getButton()))
                    {
                        /* layout starting with next widget (if exists) */
                        layout(itr.index(pWidgetNext), false);
                    }
                }

                pWidgetNext->getButton()->setFocus();

                /* consume key */
                ret = eRet_Ok;
            }
            else
            {
                /* no next widget - if focus locked, consume key */
                if (true == _lockFocus)
                {
                    ret = eRet_Ok;
                }
            }
        }
    }
        break;

    case bwidget_key_left:
    case bwidget_key_right:
        if (_lockFocus)
        {
            /* consume directional keys */
            ret = eRet_Ok;
        }
        break;

    default:
        break;
    }

    return ret;
}



CWidgetListViewContainer::CWidgetListViewContainer(bwidget_engine_t   engine, 
                                                   CWidgetBase      * pParentWidget,
                                                   MRect              geometry,
                                                   bwin_font_t        font,
                                                   bwin_t             parentWin) :
    CWidgetButton(engine, pParentWidget, geometry, font, parentWin),
    _pFocusWidget(NULL)
{
}

bool CWidgetListViewContainer::setFocus()
{
    bool ret = false;

    if (NULL != _pFocusWidget)
    {
        /* focus widget override */
        ret = _pFocusWidget->setFocus();
    }
    else
    {
        /* call base class to focus on this widget */
        ret = CWidgetButton::setFocus();
    }

    return ret;
}
