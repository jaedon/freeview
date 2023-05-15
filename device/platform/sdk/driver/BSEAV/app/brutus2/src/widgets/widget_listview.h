/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_listview.h $
* $brcm_Revision: 11 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_listview.h $
* 
* 11   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 10   10/7/12 4:55p tokushig
* SW7231-749: add sort capability for list based on button text
* 
* 9   10/7/12 3:26p tokushig
* SW7231-749: add method to get parent bwin win
* 
* 8   8/8/12 5:58p tokushig
* SW7231-749: add support for hiding listview items in menu
* 
* 7   8/2/12 2:53p tokushig
* SW7231-749: add clear method for listview and use in menu widget
* 
* 6   5/25/12 12:31a tokushig
* SW7231-749: added listview container support for buttons with sub-
* widgets being added to the list view.  also added listview support for
* popup list widgets.
* 
* 5   4/19/12 7:40p tokushig
* SW7231-749: add support for non scrolling listview.  these will auto
* resize to fit their content.
* 
* 5   4/19/12 7:38p tokushig
* SW7231-749: add support for non scrolling listview.  these will auto
* resize to fit their content.
* 
* 4   4/18/12 5:15p jrubio
* SW7346-644: add remove (element) instead of just remove (index)
* 
* 3   4/15/12 9:45p tokushig
* SW7231-749: support re-layout of listview and menu widgets when resized
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

#ifndef BRUTUS_WIDGET_LISTVIEW_H__
#define BRUTUS_WIDGET_LISTVIEW_H__

#include "widget_label.h"
#include "widget_button.h"
#include "mlist.h"

#ifdef __cplusplus
extern "C" {
#endif


class CWidgetListItem 
{
public:
    CWidgetListItem(CWidgetBase * pListView, CWidgetButton * pButton) :
        _pListView(pListView),
        _pButton(pButton),
        _pButtonParent(pButton->getParent()) 
    {
        bbutton_settings buttonSettings;

        BDBG_ASSERT(NULL != pListView);
        BDBG_ASSERT(NULL != pButton);

        /* make sure new widgetItem is a child of list view */
        pButton->setParentWin(pListView->getWin());
    };

    CWidgetButton * getButton(void) { return _pButton; };
    CWidgetBase   * getParent(void) { return _pButtonParent; };

protected:
    CWidgetBase     * _pListView;
    CWidgetButton   * _pButton;
    CWidgetBase     * _pButtonParent;
};


class CWidgetListView : public CWidgetLabel
{
public:
    CWidgetListView(bwidget_engine_t engine, 
                    CWidgetBase * pParentWidget,
                    MRect geometry,
                    bwin_font_t font,
                    bwin_t parentWin = NULL);
    ~CWidgetListView(void);

    virtual void show(bool bShow);
    virtual void show(bool bShow, bool bLockFocus);

    uint16_t getSeparatorSize(void) { return _separatorSize; };
    void     setSeparatorSize(uint16_t size) { _separatorSize = size; };

    void add(CWidgetButton * pWidgetItem);
    void remove(CWidgetButton * pWidgetItem);
    CWidgetButton * remove(uint16_t index);
    eRet sort(void);
    void clear(void);
    eRet layout(int index = 0, bool topDown = true);

    virtual void setGeometry(MRect geometry);

    int  getItemListIndex(bwidget_t widget);
    bool isFullyVisible(CWidgetButton * pWidget);
    bool isFirstVisible(CWidgetButton * pWidget);
    bool isLastVisible(CWidgetButton * pWidget);
    MAutoList <CWidgetListItem> * getWidgetList(void) { return &_widgetList; };

    bool getScroll(void) { return _scroll; };
    void setScroll(bool scroll);
    bool getAutoResize(void) { return _autoResize; };
    void setAutoResize(bool autoResize);
    
    void lockFocus(bool lock) { _lockFocus = lock; };

    eRet onKeyDown(bwidget_t widget, bwidget_key key);

protected:
    MAutoList <CWidgetListItem> _widgetList;
    uint16_t                    _separatorSize;
    bool                        _scroll;
    bool                        _autoResize;
    bool                        _lockFocus;
};


class CWidgetListViewContainer : public CWidgetButton
{
public:
    CWidgetListViewContainer(bwidget_engine_t   engine, 
                             CWidgetBase      * pParentWidget,
                             MRect              geometry,
                             bwin_font_t        font,
                             bwin_t             parentWin = NULL); /* optional */

    virtual bool setFocus(void);

    void setFocusWidget(CWidgetBase * pFocusWidget) { _pFocusWidget = pFocusWidget; };

protected:
    /* if non NULL, this widget will gain focus if the list view container is given focus.
       this helper class will manage focus in the event the container list view item
       contains one or more sub widgets */
    CWidgetBase * _pFocusWidget; 
};

#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_WIDGET_LISTVIEW_H__ */
