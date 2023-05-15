/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel.h $
* $brcm_Revision: 3 $
* $brcm_Date: 9/21/12 8:07p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel.h $
* 
* 3   9/21/12 8:07p tokushig
* SW7231-749: add label size adjustability to addLabelPopupButton()
* 
* 2   8/8/12 6:05p tokushig
* SW7231-749: add support for named widgets in listview widgets.  this is
* used to support menu widgets that allow the user to select the visible
* widgets.
* 
* 1   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 
*********************************************************************************/

#ifndef BRUTUS_PANEL_H__
#define BRUTUS_PANEL_H__

#include "widget_menu.h"
#include "widget_button.h"
#include "widget_label.h"
#include "widget_edit.h"
#include "widget_popup_list.h"
#include "screen.h"

#ifdef __cplusplus
extern "C" {
#endif

class CPanel : public CWidgetLabel
{
public:
    CPanel(bwidget_engine_t   engine, 
               CWidgetBase      * pParentWidget, 
               MRect              geometry,
               bwin_font_t        font,
               bwin_t             parentWin = NULL); /* optional */

    virtual void show(bool bShow);
    virtual void processNotification(CNotification & notification) = 0;

    CModelConfig * getModelConfig(void) { return _pModelConfig; };
    void setModelConfig(CModelConfig * pModelConfig) { _pModelConfig = pModelConfig; _pCfg = pModelConfig->getCfg(); };
    CConfiguration * getCfg(void) { return _pCfg; };


protected:
    eRet addDualLabelButton(CWidgetMenu   * pMenu, 
                            const char    *  strName,
                            CWidgetButton ** pButton, 
                            CWidgetLabel  ** pLabel, 
                            CWidgetLabel  ** pValue,
                            bwin_font_t      font);

    eRet addDualLabelEditButton(CWidgetMenu   *  pMenu, 
                                const char    *  strName,
                                CWidgetButton ** pButton, 
                                CWidgetLabel  ** pLabelLeft, 
                                CWidgetEdit   ** pEdit,
                                CWidgetLabel  ** pLabelRight,
                                bwin_font_t      font);

    eRet addLabelPopupButton(CWidgetMenu      *  pMenu,
                             const char       *  strName,
                             CWidgetButton    ** pButton,
                             CWidgetLabel     ** pLabel, 
                             CWidgetPopupList ** pPopup,
                             bwin_font_t         font,
                             uint8_t             labelPercentage = 50);

protected:
    CModelConfig   * _pModelConfig;
    CConfiguration * _pCfg;
    bwidget_t        _focusWidget;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_PANEL_H__ */
