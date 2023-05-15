/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: widget_popup_list.h $
* $brcm_Revision: 7 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/widgets/widget_popup_list.h $
* 
* 7   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 6   10/7/12 4:56p tokushig
* SW7231-749: add sort capability for list based on button text
* 
* 5   10/7/12 3:31p tokushig
* SW7231-749: add new find button method
* 
* 4   9/29/12 9:26p tokushig
* SW7231-749: add ability to select popup buttons by name and by value
* 
* 3   9/21/12 4:20p tokushig
* SW7231-749: fix right justified text in popup list widget
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

#ifndef BRUTUS_WIDGET_POPUP_LIST_H__
#define BRUTUS_WIDGET_POPUP_LIST_H__

#include "widget_button.h"
#include "widget_listview.h"
#include "widget_check_button.h"

#ifdef __cplusplus
extern "C" {
#endif

class CWidgetPopupList : public CWidgetButton
{
public:
    CWidgetPopupList(bwidget_engine_t   engine, 
                       CWidgetBase    * pParentWidget,
                       MRect            geometry,
                       bwin_font_t      font,
                       bwin_t           parentWin = NULL); 
    ~CWidgetPopupList();

    virtual void setGeometry(MRect geometry);

    void layout(void);
    void popup(bool show);
    CWidgetCheckButton * addButton(const char * name, uint16_t width , uint16_t height);
    CWidgetCheckButton * findButton(int value);
    int  getItemListIndex(bwidget_t widget) { return _pListView->getItemListIndex(widget); };
    void clearButtons(void);
    eRet select(CWidgetCheckButton * pButton);
    eRet select(const char * name);
    eRet select(const int value);
    CWidgetCheckButton * getSelection(void) { return _pSelectedButton; };
    void onClick(bwidget_t widget);
    eRet sort(void) { return _pListView->sort(); };

protected:
    CWidgetListView                 * _pListView;
    bool                              _showPopup;
    CWidgetCheckButton              * _pSelectedButton;
    MAutoList <CWidgetCheckButton>    _listCheckButtons;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_WIDGET_POPUP_LIST_H__ */
