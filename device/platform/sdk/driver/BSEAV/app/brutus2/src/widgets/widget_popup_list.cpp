/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_popup_list.cpp $
* $brcm_Revision: 7 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_popup_list.cpp $
* 
* 7   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 6   10/7/12 3:31p tokushig
* SW7231-749: add new find button method
* 
* 5   9/29/12 9:26p tokushig
* SW7231-749: add ability to select popup buttons by name and by value
* 
* 4   9/21/12 4:20p tokushig
* SW7231-749: fix right justified text in popup list widget
* 
* 3   8/8/12 5:58p tokushig
* SW7231-749: add support for hiding listview items in menu
* 
* 2   7/9/12 4:20p tokushig
* SW7231-749: remove popup parent widget parameter.  widget will find
* framebuffer bwin internally for popup.  this will allow the popup to
* display outside the confines of it's parent widget's geometry.
* 
* 1   5/25/12 12:36a tokushig
* SW7231-749: initial creation
* 
* 
*********************************************************************************/

#include "screen.h"
#include "widget_popup_list.h"
#include "widget_check_button.h"
#include "widget_base.h"
#include "string.h"
#include "bdbg.h"

BDBG_MODULE(widget_popup_list);


static void clickCallback(bwidget_t widget)
{
    eRet                 ret     = eRet_Ok;
    CWidgetPopupList   * pWidget = NULL;
    CWidgetBase        * pParent = NULL;

    bwidget_settings bwidgetSettings;
    bwidget_get_settings(widget, &bwidgetSettings);
    pWidget = (CWidgetPopupList *)bwidgetSettings.data;
    BDBG_ASSERT(NULL != pWidget);

    if (true == pWidget->isVisible())
    {
        /* widget is visible, so handle click */
        pWidget->onClick(widget);
    }
}

static void popup_list_bwidget_draw(bwidget_t widget, const bwin_rect *cliprect)
{
    bwin_t win = bwidget_win(widget);
    bwin_settings win_settings;
    int right, bottom;
    blabel_settings *label_settings = NULL;
    bbutton_settings buttonSettings;
    CWidgetPopupList * pButton = NULL;
    bool down = false;
    int box_size = 12;
    
    bbutton_get(widget, &buttonSettings);
    label_settings = &(buttonSettings.label);

    pButton = (CWidgetPopupList *)buttonSettings.label.widget.data;
    BDBG_ASSERT(NULL != pButton);

    down = buttonSettings.down;

    /* TODO: for efficieny, we may want bwidgets to have access to bwin private structures. */
    bwin_get(win, &win_settings);
    
    /* only draw if something's within the cliprect */
    if (cliprect && !BWIN_RECT_ISINTERSECTING(&win_settings.rect, cliprect)) {
        return;
    }
    
    right = win_settings.rect.x + win_settings.rect.width;
    bottom = win_settings.rect.y + win_settings.rect.height;

    if (label_settings->image) {
        bwin_image_render(win, label_settings->image, label_settings->render_mode, &win_settings.rect, NULL, cliprect);
    } 
    else {
        bwin_fill_rect(win, &win_settings.rect, label_settings->background_color, cliprect);
        
        if (label_settings->bevel) {
            int i;
            /* top */
            for (i=0;i<label_settings->bevel;i++) {
                bwin_draw_line(win, i, i, win_settings.rect.width-i, i,
                    label_settings->bevel_color[down?2:0], cliprect);
            }
            /* right */
            for (i=0;i<label_settings->bevel;i++) {
                bwin_draw_line(win, right-i-1, i, right-i-1, win_settings.rect.height-i,
                    label_settings->bevel_color[down?3:1], cliprect);
            }
            /* bottom */
            for (i=0;i<label_settings->bevel;i++) {
                bwin_draw_line(win, i, bottom-i-1, win_settings.rect.width-i, bottom-i-1,
                    label_settings->bevel_color[down?0:2], cliprect);
            }
            /* left */
            for (i=0;i<label_settings->bevel;i++) {
                bwin_draw_line(win, i, i, i, win_settings.rect.height-i,
                    label_settings->bevel_color[down?1:3], cliprect);
            }
        }
    }

    /* draw check box */

    {
        int i        = 0;
        int margin   = 10;
        int x        = 0;
        int y        = 0;
        int width    = 0;
        int height   = 0;
        MRect rect(win_settings.rect.x + win_settings.rect.width - box_size - margin,
                   win_settings.rect.y + (win_settings.rect.height / 2) - (box_size / 2),
                   box_size,
                   box_size);
        rect.setX(rect.x() - label_settings->bevel);

        x      = rect.x();
        y      = rect.y();
        width  = rect.width() + 1;
        height = rect.height();

        /* draw pulldown triangle indicator */
        for (i = 0; 1 < width; i++)
        {
            bwin_draw_line(win, x, y, x + width, y, 0xFF22527A, cliprect);
            y += 1;
            bwin_draw_line(win, x, y, x + width, y, 0xFF22527A, cliprect);
            x += 1;
            y += 1;
            width -= 2;
        }

        /* draw focus */
        rect.setX(rect.x() - 1);
        rect.setY(rect.y() - 1);
        rect.grow(2);
    
        if (widget == bwidget_get_focus(pButton->getEngine())) {
            /* draw focus */
            for (i=0;i<2;i++) {
                bwin_draw_line(win, rect.x(), rect.y(), rect.x() + rect.width(), rect.y(),
                    label_settings->focus_color, cliprect);
                bwin_draw_line(win, rect.x() + rect.width(), rect.y(), rect.x() + rect.width(), rect.y() + rect.height(),
                    label_settings->focus_color, cliprect);
                bwin_draw_line(win, rect.x(), rect.y() + rect.height(), rect.x() + rect.width() + 1, rect.y() + rect.height(),
                    label_settings->focus_color, cliprect);
                bwin_draw_line(win, rect.x(), rect.y(), rect.x(), rect.y() + rect.height(),
                    label_settings->focus_color, cliprect);

                rect.setX(rect.x() - 1);
                rect.setY(rect.y() - 1);
                rect.grow(2);
            }
        }
    }

    if (label_settings->text) {
        int width, height, base, x, y;
        bwin_measure_text(label_settings->font, label_settings->text, -1,
            &width, &height, &base);
        
        /* calculate vertical justification */
        y = 0;
        if (label_settings->text_justify_vert == bwidget_justify_vert_middle) {
            y = (win_settings.rect.height - height) / 2;
        } 
        else if (label_settings->text_justify_vert == bwidget_justify_vert_bottom) {
            y += label_settings->text_margin;
        }
        else { /* bwidget_justify_vert_top */
            y = win_settings.rect.height - height;
            y -= label_settings->text_margin;
        }

        /* calculate horizontal justification */
        x = 0;
        if (label_settings->text_justify_horiz == bwidget_justify_horiz_center) {
            x = (win_settings.rect.width - width) / 2;
        } 
        else if (label_settings->text_justify_horiz == bwidget_justify_horiz_right) {
            x = win_settings.rect.width - width;
            x -= label_settings->text_margin;
            x -= box_size + 16;
        }
        else { /* bwidget_justify_horiz_left */
            x += label_settings->text_margin;
        }

        bwin_draw_text(win, label_settings->font, x, y,
            label_settings->text, -1, label_settings->text_color,
            cliprect);
    }
}

CWidgetPopupList::CWidgetPopupList(bwidget_engine_t   engine, 
                                   CWidgetBase      * pParentWidget,
                                   MRect              geometry,
                                   bwin_font_t        font,
                                   bwin_t             parentWin) : 
    CWidgetButton(engine, pParentWidget, geometry, font, parentWin),
    _pListView(NULL),
    _showPopup(false),
    _pSelectedButton(NULL)
{
    eRet             ret              = eRet_Ok;
    bbutton_settings buttonSettings;

    bbutton_get(getWidget(), &buttonSettings);
    buttonSettings.click = clickCallback;
    buttonSettings.label.widget.draw = popup_list_bwidget_draw;
    bbutton_set(getWidget(), &buttonSettings);

    _pListView = new CWidgetListView(getEngine(), this, geometry, font);
    CHECK_PTR_GOTO("unable to allocate listview widget", _pListView, ret, eRet_OutOfMemory, error);
    _pListView->setScroll(false);
    _pListView->setZOrder(getZOrder() + 1);
    _pListView->show(false);

    /* force call to calc geometry - the list view coordinate system differs from that of
       this widget because it must be able to draw outside the boundaries of this widget.
       setGeometry() will account for this fact and can adjust the listview coords
       accordingly. */
    setGeometry(geometry);

error:
    return;
}

CWidgetPopupList::~CWidgetPopupList()
{
    clearButtons();
    DEL(_pListView);
}

void CWidgetPopupList::layout()
{
    _pListView->layout();
}

void CWidgetPopupList::popup(bool show)
{
    if (_showPopup == show)
    {
        return;
    }

    if (0 == _pListView->getWidgetList()->total())
    {
        return;
    }

    _showPopup = show;
    if (true == show)
    {
        bwin_t        popupWin    = getWin();
        bwin_settings popupSettings;

        /* traverse up bwin hierarchy to get to framebuffer - save in popupWin */
        bwin_get(popupWin, &popupSettings);
        while (NULL != popupSettings.parent)
        {
            popupWin = popupSettings.parent;
            bwin_get(popupWin, &popupSettings);
        }

        /* reparent to _popupWin so we can draw the list widget outside the
           boundaries of this widget */
        _pListView->setParentWin(popupWin);
        _pListView->show(true, true);

        _pSelectedButton->setFocus();
    }
    else
    {
        /* reset parent bwin */
        _pListView->setParentWin(getWin());
        _pListView->show(false);

        /* closing popup, so return focus to button */
        setFocus();
    }
}

/* override setGeometry so we can adjust our internal list view everytime the popup list
   geometry changes */
void CWidgetPopupList::setGeometry(MRect geometry)
{
    bwin_t        win           = getWin();
    bwin_settings winSettings;
    int           x             = 0;
    int           y             = 0;
    MRect         rectListView;

    /* call base class */
    CWidgetButton::setGeometry(geometry);

    /* our internal list view's parent is actually the popupWin given in the constructor.
       (so it can draw outside the bounds of the popup list and menu)
       so we will set the position and width to be relative to that bwin's coordinate system.
       note that the height is auto generated by the listview's addButton() method so we
       won't alter that here */

    /* traverse up thru window heirarchy to add up relative x and y coordinates until we get
       to the listview's parent bwin (we'll also check for NULL but it should never happen) */
    while ((NULL != win) && (_pListView->getParent()->getWin() != win))
    {
        bwin_get(win, &winSettings);
        x += winSettings.x;
        y += winSettings.y;

        win = winSettings.parent;
    }

    rectListView.setX(x);
    rectListView.setY(y);
    rectListView.setWidth(geometry.width());
    _pListView->setGeometry(rectListView);

    layout();
}

CWidgetCheckButton * CWidgetPopupList::addButton(const char * name, uint16_t width , uint16_t height)
{
    eRet                 ret            = eRet_Ok;
    CWidgetCheckButton * pButton        = NULL;
    bbutton_settings     buttonSettings;
    MRect                rectButton;

    bbutton_get(getWidget(), &buttonSettings);

    pButton = new CWidgetCheckButton(getEngine(), this, MRect(0, 0, width, height), buttonSettings.label.font);
    CHECK_PTR_GOTO("unable to allocate check button widget", pButton, ret, eRet_OutOfMemory, error);
    if (NULL != name)
    {
        pButton->setText(name, bwidget_justify_horiz_left, bwidget_justify_vert_middle);
    }

    _pListView->add(pButton);

    /* keep track of new check buttons */
    _listCheckButtons.add(pButton);

    if (NULL == _pSelectedButton)
    {
        /* set selected button if not set */
        select(pButton);
    }

error:
    return pButton;
}

/* search internal listview for button matching given value */
CWidgetCheckButton * CWidgetPopupList::findButton(int value)
{
    MList <CWidgetListItem>     * pWidgetList       = _pListView->getWidgetList();
    MListItr <CWidgetListItem>    itr(pWidgetList);
    CWidgetListItem             * pItem             = NULL;

    for (pItem = itr.first(); pItem; pItem = itr.next())
    {
        if (value == pItem->getButton()->getValue())
        {
            break;
        }
    }

    return (CWidgetCheckButton *)pItem->getButton();
}

void CWidgetPopupList::clearButtons()
{
    _pListView->clear();
    _listCheckButtons.clear();
    _pSelectedButton = NULL;
}

eRet CWidgetPopupList::select(CWidgetCheckButton * pButton)
{
    eRet                      ret         = eRet_Ok;
    MList <CWidgetListItem> * pWidgetList = NULL;
    CWidgetListItem         * pListItem   = NULL;
    bool                      bFound      = false;

    pWidgetList = _pListView->getWidgetList();
    CHECK_PTR_GOTO("no buttons added to list view - aborting select request", pWidgetList, ret, eRet_NotAvailable, error);

    /* check the given button while unchecking all others in list */
    {
        MListItr <CWidgetListItem>   itr(pWidgetList);
        CWidgetCheckButton         * pCheckButton = NULL;

        for (pListItem = itr.first();  NULL != pListItem;  pListItem = itr.next())
        {
            pCheckButton = (CWidgetCheckButton *)pListItem->getButton();

            if (pCheckButton == pButton)
            {
                pCheckButton->setCheck(true);
                /* update button text to match new selection */
                setText(pCheckButton->getText().s());
                _pSelectedButton = pCheckButton;
                bFound = true;
            }
            else
            {
                pCheckButton->setCheck(false);
            }
        }
    }

    if (false == bFound)
    {
        ret = eRet_NotAvailable;
    }

error:
    return ret;
}

eRet CWidgetPopupList::select(const char * name)
{
    eRet                      ret         = eRet_Ok;
    MList <CWidgetListItem> * pWidgetList = NULL;
    CWidgetListItem         * pListItem   = NULL;
    bool                      bFound      = false;

    pWidgetList = _pListView->getWidgetList();
    CHECK_PTR_GOTO("no buttons added to list view - aborting select request", pWidgetList, ret, eRet_NotAvailable, error);

    /* check the given button while unchecking all others in list */
    {
        MListItr <CWidgetListItem>   itr(pWidgetList);
        CWidgetCheckButton         * pCheckButton = NULL;

        for (pListItem = itr.first();  NULL != pListItem;  pListItem = itr.next())
        {
            pCheckButton = (CWidgetCheckButton *)pListItem->getButton();

            if (pCheckButton->getText() == MString(name))
            {
                pCheckButton->setCheck(true);
                /* update button text to match new selection */
                setText(pCheckButton->getText().s());
                _pSelectedButton = pCheckButton;
                bFound = true;
            }
            else
            {
                pCheckButton->setCheck(false);
            }
        }
    }

    if (false == bFound)
    {
        ret = eRet_NotAvailable;
    }

error:
    return ret;
}

eRet CWidgetPopupList::select(const int value)
{
    eRet                      ret         = eRet_Ok;
    MList <CWidgetListItem> * pWidgetList = NULL;
    CWidgetListItem         * pListItem   = NULL;
    bool                      bFound      = false;

    pWidgetList = _pListView->getWidgetList();
    CHECK_PTR_GOTO("no buttons added to list view - aborting select request", pWidgetList, ret, eRet_NotAvailable, error);

    /* check the given button while unchecking all others in list */
    {
        MListItr <CWidgetListItem>   itr(pWidgetList);
        CWidgetCheckButton         * pCheckButton = NULL;

        for (pListItem = itr.first();  NULL != pListItem;  pListItem = itr.next())
        {
            pCheckButton = (CWidgetCheckButton *)pListItem->getButton();

            if (pCheckButton->getValue() == value)
            {
                pCheckButton->setCheck(true);
                /* update button text to match new selection */
                setText(pCheckButton->getText().s());
                _pSelectedButton = pCheckButton;
                bFound = true;
            }
            else
            {
                pCheckButton->setCheck(false);
            }
        }
    }

    if (false == bFound)
    {
        ret = eRet_NotAvailable;
    }

error:
    return ret;
}

void CWidgetPopupList::onClick(bwidget_t widget)
{
    eNotification   notification  = eNotify_Invalid;
    CWidgetBase   * pWidget       = NULL;

    /* find the widget object that corresponds to the given bwidget_t */
    {
        blabel_settings   labelSettings;
        blabel_get(widget, &labelSettings);
        pWidget = (CWidgetBase *)labelSettings.widget.data;
    }

    if (this == pWidget)
    {
        /* click on popup button */
        popup(true);
    }
    else
    {
        /* click on check button in popup list view - select it
           and clear others, then hide popup list view */
        select((CWidgetCheckButton *)pWidget);
        popup(false);
        
        /* up to now, we have hijacked the button click internally - now forward
           click to parent widget for possible additional handling */
        getParent()->onClick(widget);
    }
}
