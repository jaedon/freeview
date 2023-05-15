/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_menu.cpp $
* $brcm_Revision: 11 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_menu.cpp $
* 
* 11   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 10   8/8/12 5:58p tokushig
* SW7231-749: add support for hiding listview items in menu
* 
* 9   8/2/12 2:53p tokushig
* SW7231-749: add clear method for listview and use in menu widget
* 
* 8   5/25/12 12:33a tokushig
* SW7231-749: updated to match fix for vertical justification top/bottom
* bug.
* 
* 7   5/4/12 2:04p tokushig
* SW7231-749: add buffers screen, checkbox widget, and new fonts
* 
* 6   4/19/12 7:43p tokushig
* SW7231-749: add support for scrolling or non-scrolling internal
* listview.  fixed problems in layout method.
* 
* 6   4/19/12 7:41p tokushig
* SW7231-749: add support for scrolling or non-scrolling internal
* listview.  fixed problems in layout method.
* 
* 5   4/18/12 5:15p jrubio
* SW7346-644: add remove (element) instead of just remove (index)
* 
* 4   4/15/12 9:45p tokushig
* SW7231-749: support re-layout of listview and menu widgets when resized
* 
* 3   4/14/12 9:08p tokushig
* SW7231-749: add method to set menu title and sub title
* 
* 2   4/14/12 7:25p tokushig
* SW7231-749: make menu dynamically resize internal components when size
* is changed
* 
* 
*********************************************************************************/

#include "widget_menu.h"
#include "widget_button.h"
#include "widget_listview.h"
#include "widget_check_button.h"
#include "string.h"
#include "bdbg.h"

BDBG_MODULE(widget_menu);

CWidgetMenu::CWidgetMenu(bwidget_engine_t engine, 
                         CWidgetBase * pParentWidget, 
                         MRect geometry,
                         bwin_font_t fontMenu,
                         bwin_font_t fontSubMenu) :
    CWidgetLabel(engine, pParentWidget, geometry, fontMenu),
    _pMenuBar(NULL),
    _pMenuBarBack(NULL),
    _pMenuSubTitle(NULL),
    _pEdit(NULL),
    _pListView(NULL),
    _fontMenu(fontMenu)
{
    eRet            ret            = eRet_Ok;
    blabel_settings labelSettings;

    blabel_get(_widget, &labelSettings);
    labelSettings.bevel            = 0;
    labelSettings.background_color = 0xFF222222;
    blabel_set(_widget, &labelSettings);

    MRect rectMenu = getGeometry();

    _pMenuBar = new CWidgetLabel(getEngine(), this, MRect(0, 0, 0, 0), fontMenu);
    CHECK_PTR_GOTO("unable to allocate label widget", _pMenuBar, ret, eRet_OutOfMemory, error);
    ret = _pMenuBar->loadImage("images/menu_bar_gradient.jpg", bwin_image_render_mode_tile);
    CHECK_ERROR_GOTO("unable to set label widget image", ret, error);
    _pMenuBar->setText("Menu", bwidget_justify_horiz_center, bwidget_justify_vert_middle);
    _pMenuBar->setTextColor(0xFFF0F0F0);
    _pMenuBar->setBevel(0);
    setActive(_pMenuBar, true);

    _pMenuSubTitle = new CWidgetLabel(getEngine(), this, MRect(0, 0, 0, 0), fontSubMenu ? fontSubMenu : fontMenu);
    CHECK_PTR_GOTO("unable to allocate label widget", _pMenuSubTitle, ret, eRet_OutOfMemory, error);
    _pMenuSubTitle->setBevel(0);
    _pMenuSubTitle->setBackgroundColor(0xFF222222);
    _pMenuSubTitle->setText("Label Text", bwidget_justify_horiz_left, bwidget_justify_vert_top);
    _pMenuSubTitle->setTextColor(0xFF80C42F);
    setActive(_pMenuSubTitle, true);

    _pEdit = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 0), fontSubMenu ? fontSubMenu : fontMenu);
    _pEdit->setBevel(0);
    _pEdit->setText("+/-", bwidget_justify_horiz_center, bwidget_justify_vert_middle);
    _pEdit->setBackgroundColor(0xFF222222);
    _pEdit->setTextColor(0xFFCCCCCC);
    setActive(_pEdit, true);

    _pListView = new CWidgetListView(getEngine(), this, MRect(0, 0, 0, 0), fontMenu);
    CHECK_PTR_GOTO("unable to allocate listview widget", _pListView, ret, eRet_OutOfMemory, error);
    setActive(_pListView, true);

    _pListViewSelection = new CWidgetListView(getEngine(), this, MRect(0, 0, 0, 0), fontMenu);
    CHECK_PTR_GOTO("unable to allocate listview widget", _pListViewSelection, ret, eRet_OutOfMemory, error);
    setActive(_pListViewSelection, false);

    layout();

    return;
error:
    BDBG_ASSERT(0);
}

CWidgetMenu::~CWidgetMenu()
{
    _itemList.clear();
    DEL(_pListViewSelection);
    DEL(_pListView);
    DEL(_pEdit);
    DEL(_pMenuSubTitle);
    DEL(_pMenuBar);
}

#define EDIT_IND_WIDTH 25
#define EDIT_IND_HEIGHT 16
#define SUBTITLE_HEIGHT 25

void CWidgetMenu::layout()
{
    MRect rectMenu     = getGeometry();
    MRect rectMenuBar;
    MRect rectSubTitle;
    MRect rectEdit;
    MRect rectListView;
    MRect rectListViewSelection;
    int   outsideMargin = 7;
    int   vMargin       = 5;
    int   heightTotal   = 0;

    if (true == isActive(_pMenuBar))
    {
        rectMenuBar.setX(0);
        rectMenuBar.setY(0);
        rectMenuBar.setWidth(rectMenu.width());
        rectMenuBar.setHeight(30);
        _pMenuBar->setGeometry(rectMenuBar);
        heightTotal += rectMenuBar.height();
    }
    else
    {
        rectMenuBar.setHeight(0);
    }

    if (true == isActive(_pMenuSubTitle))
    {
        rectSubTitle.setX(outsideMargin);
        rectSubTitle.setY(heightTotal);
        rectSubTitle.setWidth(rectMenu.width() - rectEdit.width() - (outsideMargin * 2) - EDIT_IND_WIDTH);
        rectSubTitle.setHeight(SUBTITLE_HEIGHT);
        _pMenuSubTitle->setGeometry(rectSubTitle);
    }
    
    /* adjust total height based on edit and sub title active state */
    if ((true == isActive(_pEdit)) || (true == isActive(_pMenuSubTitle)))
    {
        heightTotal += MAX(rectSubTitle.height(), EDIT_IND_HEIGHT);
    }
    else
    {
        /* add margin for area under menu bar */
        heightTotal += outsideMargin;
        rectSubTitle.setHeight(0);
    }

    if (true == isActive(_pEdit))
    {
        /* layout edit button/icon */
        rectEdit.setWidth(EDIT_IND_WIDTH);
        rectEdit.setHeight(EDIT_IND_HEIGHT);
        rectEdit.setX(rectMenu.width() - outsideMargin - rectEdit.width());
        rectEdit.setY(heightTotal - EDIT_IND_HEIGHT);
        _pEdit->setGeometry(rectEdit);
    }

    /* either _pListView or _pListViewSelection is displayed - never both */
    if (true == isActive(_pListView))
    {
        rectListView.setX(outsideMargin);
        rectListView.setY(heightTotal);

        rectListView.setWidth(rectMenu.width() - (outsideMargin * 2));

        if (true == _pListView->getScroll())
        {
            /* list view scrolling is ON so resize list view to
               fit in the remaining space in this menu widget */
            rectListView.setHeight(rectMenu.height() - rectListView.y() - outsideMargin);
            _pListView->setGeometry(rectListView);

            heightTotal += rectListView.height() + vMargin;
        }
        else
        {
            MRect rectListViewActual = _pListView->getGeometry();
            rectListView.setHeight(rectListViewActual.height());
            _pListView->setGeometry(rectListView);

            heightTotal += rectListViewActual.height() + outsideMargin;

            /* list view scrolling is OFF so resize menu widget to
               fit the enclosed list widget */
            rectMenu.setHeight(heightTotal); 
            CWidgetLabel::setGeometry(rectMenu); 
        }
    }
    else
    if (true == isActive(_pListViewSelection))
    {
        rectListViewSelection.setX(outsideMargin);
        rectListViewSelection.setY(heightTotal);

        BDBG_ASSERT(true == _pListViewSelection->getScroll());
        rectListViewSelection.setWidth(rectMenu.width() - (outsideMargin * 2));

        /* selection list view scrolling should be always ON so resize list view to
           fit in the remaining space in this menu widget */
        rectListViewSelection.setHeight(rectMenu.height() - rectListViewSelection.y() - outsideMargin);
        _pListViewSelection->setGeometry(rectListViewSelection);

        heightTotal += rectListViewSelection.height() + vMargin;
    }
}

void CWidgetMenu::setGeometry(MRect geometry)
{
    CWidgetLabel::setGeometry(geometry); /* call base class */
    layout();
}

void CWidgetMenu::showMenuBar(bool show)
{
    setActive(_pMenuBar, show);
    layout();
}

void CWidgetMenu::showSubTitle(bool show)
{
    setActive(_pMenuSubTitle, show);
    layout();
}

void CWidgetMenu::showListView(bool show)
{
    setActive(_pListView, show);
    layout();
}

void CWidgetMenu::showEdit(bool show)
{
    setActive(_pEdit, show);
    layout();
}

void CWidgetMenu::showListViewSelection(bool show)
{
    setActive(_pListViewSelection, show);
    layout();
}

void CWidgetMenu::setActive(CWidgetBase * pWidget, bool bActive)
{
    MListItr <CWidgetBase>   itr(&_activeList);
    CWidgetBase            * pActiveWidget           = NULL;

    /* remove from active list if it exists */
    for (pActiveWidget = itr.first(); pActiveWidget; pActiveWidget = itr.next())
    {
        if (pActiveWidget == pWidget)
        {
            _activeList.remove(pActiveWidget);
            break;
        }
    }

    if (true == bActive)
    {
        _activeList.add(pWidget);
    }

    pWidget->show(bActive);
}

bool CWidgetMenu::isActive(CWidgetBase * pWidget)
{
    MListItr <CWidgetBase>   itr(&_activeList);
    CWidgetBase            * pActiveWidget     = NULL;
    bool                     bActive           = false;

    /* search for widget in active list */
    for (pActiveWidget = itr.first(); pActiveWidget; pActiveWidget = itr.next())
    {
        if (pActiveWidget == pWidget)
        {
            bActive = true;
            break;
        }
    }

    return bActive;
}

/* sets menubar title and sub title if non-NULL */
void CWidgetMenu::setMenuTitle(const char * title, const char * subTitle)
{
    if (NULL != title)
    {
        _pMenuBar->setText(title);
    }

    if (NULL != subTitle)
    {
        _pMenuSubTitle->setText(subTitle);
    }
}

/* adds a button widget to this list view.  click callbacks are handled through the button
   widget settings - calling code will handle click processing.  we will also create
   a checkbox/button combo for the widget selection list which will allow users to
   specify which list view element to show/hide (this is handy when there are a large
   number of elements but the user may only be interested in working with a small subset) */
eRet CWidgetMenu::addButton(CWidgetButton * pWidgetItem, const char * strName, bool bVisible)
{
    eRet                       ret        = eRet_Ok;
    CWidgetCheckButton       * pButton    = NULL;
    CWidgetMenuListItem      * pItem      = NULL;
    BDBG_ASSERT(NULL != pWidgetItem);

    /* inherit size/font from given widget */
    MRect       rect = pWidgetItem->getGeometry();
    bwin_font_t font = pWidgetItem->getFont();

    /* add check button to selection list view.  these widgets will be owned by
       the _itemList autolist and will be auto destroyed when removed. */
    pButton = new CWidgetCheckButton(getEngine(), this, rect, font); 
    CHECK_PTR_GOTO("unable to allocate label widget", pButton, ret, eRet_OutOfMemory, error);
    pButton->setBevel(0);
    pButton->setCheck(bVisible);
    pButton->setText(strName, bwidget_justify_horiz_left);

    /* add to selection list view */
    _pListViewSelection->add(pButton);

    if (true == bVisible)
    {
        /* given button is visible so add to listview */
        _pListView->add(pWidgetItem);
    }

    /* save all added buttons */
    pItem = new CWidgetMenuListItem(pWidgetItem, pButton);
    CHECK_PTR_GOTO("unable to create menu list item", pItem, ret, eRet_OutOfMemory, error);
    _itemList.add(pItem);

    if (false == _pListView->getScroll())
    {
        layout();
    }

    goto done;
error:
    if (NULL != pButton)    { delete pButton; }
    if (NULL != pItem)      { delete pItem; }
done:
    return ret;;
}

void CWidgetMenu::removeButton(CWidgetButton * pWidgetItem)
{
    MListItr <CWidgetMenuListItem>    itr(&_itemList);
    CWidgetMenuListItem             * pItem             = NULL;

    /* find given button in list of previously added buttons */
    for (pItem = itr.first(); pItem; pItem = itr.next())
    {
        if (pWidgetItem == pItem->getButton())
        {
            /* found button so remove it from list view controls */
            _pListViewSelection->remove(pItem->getButton());
            _pListView->remove(pWidgetItem);

            /* remove from list of added widgets */
            _itemList.remove(pItem);
            break;
        }
    }
}

void CWidgetMenu::clearButtons()
{
    /* remove buttons from list view controls */
    _pListView->clear();
    _pListViewSelection->clear();

    /* remove buttons from list of added widgets */
    _itemList.clear();
}

void CWidgetMenu::addBackButton(CWidgetButton * pWidgetItem)
{
    bbutton_settings buttonSettings;
    BDBG_ASSERT(NULL != pWidgetItem);

    /* make sure new widgetItem is a child of the menu bar */
    bbutton_get(pWidgetItem->getWidget(), &buttonSettings);
    buttonSettings.label.widget.window.parent = _pMenuBar->getWin();
    bbutton_set(pWidgetItem->getWidget(), &buttonSettings);

    /* adjust position/size */
    pWidgetItem->setGeometry(MRect(5, 4, 45, 22));
    pWidgetItem->setTextMargin(0);
    pWidgetItem->setBevel(2);

    _pMenuBarBack = pWidgetItem;
}

/* since added back button widget was originally created by the calling code,
   we will only re-parent it here.  it remains the calling code's responsibility
   to unallocate the back button widget when appropriate. */
void CWidgetMenu::removeBackButton()
{
    bbutton_settings buttonSettings;

    BDBG_ASSERT(NULL != _pMenuBarBack);

    /* make sure new widgetItem is a child of menu bar's parent */
    bbutton_get(_pMenuBarBack->getWidget(), &buttonSettings);
    buttonSettings.label.widget.window.parent = _pParent->getWin();
    bbutton_set(_pMenuBarBack->getWidget(), &buttonSettings);
}

/* turning OFF scrolling for the list view will cause it to auto resize based
   on contents - the menu will then resize accordingly.
   turning ON scrolling for the list view will cause the list view to resize
   based on the current size of the menu */
void CWidgetMenu::setScroll(bool scroll)
{
    _pListView->setScroll(scroll);
    if (false == scroll)
    {
        layout();
    }
}

void CWidgetMenu::onClick(bwidget_t widget)
{
    CWidgetBase   * pWidget       = NULL;

    /* find the widget object that corresponds to the given bwidget_t */
    {
        blabel_settings   labelSettings;
        blabel_get(widget, &labelSettings);
        pWidget = (CWidgetBase *)labelSettings.widget.data;
    }

    if (_pEdit == pWidget)
    {
        bool bListViewActive = isActive(_pListView);

        if (true == isActive(_pListViewSelection))
        {
            MListItr <CWidgetMenuListItem>   itr(&_itemList);
            CWidgetMenuListItem            * pItem              = NULL;

            /* selection list view is about to be hidden, so adjust
               active list view based on user selections */
            _pListView->clear();

            for (pItem = itr.first(); pItem; pItem = itr.next())
            {
                if ((NULL != pItem->getCheckButton()) && (true == pItem->getCheckButton()->isChecked()))
                {
                    /* user selected this control to add it to list view */
                    _pListView->add(pItem->getButton());
                }
            }
        }

        showListView(!bListViewActive);
        showListViewSelection(bListViewActive);
    }
}

