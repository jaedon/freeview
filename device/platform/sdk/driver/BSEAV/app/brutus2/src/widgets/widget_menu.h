/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_menu.h $
* $brcm_Revision: 9 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_menu.h $
* 
* 9   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 8   8/8/12 5:58p tokushig
* SW7231-749: add support for hiding listview items in menu
* 
* 7   8/2/12 2:53p tokushig
* SW7231-749: add clear method for listview and use in menu widget
* 
* 6   5/4/12 2:04p tokushig
* SW7231-749: add buffers screen, checkbox widget, and new fonts
* 
* 5   4/19/12 7:43p tokushig
* SW7231-749: add support for scrolling or non-scrolling internal
* listview.  fixed problems in layout method.
* 
* 5   4/19/12 7:41p tokushig
* SW7231-749: add support for scrolling or non-scrolling internal
* listview.  fixed problems in layout method.
* 
* 4   4/18/12 5:15p jrubio
* SW7346-644: add remove (element) instead of just remove (index)
* 
* 3   4/15/12 9:45p tokushig
* SW7231-749: support re-layout of listview and menu widgets when resized
* 
* 2   4/14/12 9:08p tokushig
* SW7231-749: add method to set menu title and sub title
* 
* 1   4/14/12 5:24p tokushig
* SW7231-749: refactor widget classes to hide keypress/click callback
* mechanism and simplify screen creation.  also added new menu widget.
* 
* 
*********************************************************************************/

#ifndef BRUTUS_WIDGET_MENU_H__
#define BRUTUS_WIDGET_MENU_H__

#include "widget_label.h"
#include "widget_check_button.h"
#include "mlist.h"
#include "mstring.h"

#ifdef __cplusplus
extern "C" {
#endif

class CWidgetButton;
class CWidgetListView;

class CWidgetMenuListItem
{
public:
    CWidgetMenuListItem(CWidgetButton            * pButton,
                        CWidgetCheckButton       * pCheckButton) :
        _pButton(pButton),
        _pButtonParent(pButton->getParent()),
        _pCheckButton(pCheckButton) {};

    ~CWidgetMenuListItem(void)
    {
        bbutton_settings buttonSettings;

        /* do NOT delete _pButton as it is owned by CWidgetMenu calling code */
        if (NULL != _pCheckButton)  { delete _pCheckButton; };

        #if 0
        /* restore removed button original parent */
        bbutton_get(_pButton->getWidget(), &buttonSettings);
        buttonSettings.label.widget.window.parent = _pButtonParent->getWin();
        bbutton_set(_pButton->getWidget(), &buttonSettings);
        #endif
    };
    
    CWidgetButton * getButton(void) { return _pButton; };
    CWidgetBase   * getParent(void) { return _pButtonParent; };
    CWidgetCheckButton * getCheckButton(void) { return _pCheckButton; };

protected:
    CWidgetButton            * _pButton;
    CWidgetBase              * _pButtonParent;
    CWidgetCheckButton       * _pCheckButton;
};


class CWidgetMenu : public CWidgetLabel
{
public:
    CWidgetMenu(bwidget_engine_t engine, 
                CWidgetBase * pParentWidget,
                MRect geometry,
                bwin_font_t fontMenu,
                bwin_font_t fontSubMenu = NULL);
    ~CWidgetMenu(void);

    void layout(void);

    void setMenuTitle(const char * title = NULL, const char * subTitle = NULL);
    void showMenuBar(bool show);
    void showSubTitle(bool show);
    void showListView(bool show);
    void showEdit(bool show);
    void showListViewSelection(bool show);

    eRet addButton(CWidgetButton * pWidgetItem, const char * strName, bool bVisible = true);
    void removeButton(CWidgetButton * pWidgetItem);
    void removeButton(uint16_t index);
    void clearButtons(void);
    void addBackButton(CWidgetButton * pWidgetItem);
    void removeBackButton(void);
    void setScroll(bool scroll);
    void setAutoResize(bool autoResize);

    void setActive(CWidgetBase * pWidget, bool bActive);
    bool isActive(CWidgetBase * pWidget);

    void onClick(bwidget_t widget);

    virtual void setGeometry(MRect geometry);

protected:
    CWidgetLabel    * _pMenuBar;
    CWidgetButton   * _pMenuBarBack;
    CWidgetLabel    * _pMenuSubTitle;
    CWidgetButton   * _pEdit;
    CWidgetListView * _pListView;
    CWidgetListView * _pListViewSelection;

    /* list of active controls */
    MList <CWidgetBase> _activeList;

    /* list of containers comprised of all added button widgets and
       their associated check buttons used in the selection list */
    MAutoList <CWidgetMenuListItem> _itemList;

    bwin_font_t       _fontMenu;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_WIDGET_MENU_H__ */
