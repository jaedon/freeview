/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel_scan_vsb.h $
* $brcm_Revision: 2 $
* $brcm_Date: 8/2/12 11:12a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel_scan_vsb.h $
* 
* 2   8/2/12 11:12a tokushig
* SW7231-749: add missing destructor and enable last focus support
* 
* 1   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 
*********************************************************************************/

#ifndef BRUTUS_PANEL_SCAN_VSB_H__
#define BRUTUS_PANEL_SCAN_VSB_H__

#include "panel.h"
#include "timer.h"
#include "widget_label.h"
#include "widget_button.h"
#include "widget_check_button.h"
#include "widget_menu.h"
#include "widget_edit.h"
#include "widget_popup_list.h"
#include "widget_modalmsgbox.h"

#ifdef __cplusplus
extern "C" {
#endif


class CPanelScanVsb : public CPanel
{
public:
    CPanelScanVsb(bwidget_engine_t   engine, 
                  CWidgetBase      * pParentWidget, 
                  MRect              geometry,
                  bwin_font_t        font,
                  bwin_t             parentWin = NULL); /* optional */
    ~CPanelScanVsb(void);

    eRet initialize(CModel * pModel, CModelConfig * pModelConfig);
    void uninitialize(void);

    void onClick(bwidget_t widget);
    void processNotification(CNotification & notification);

protected:
    CWidgetMenu           * _pScanVsbMenu;
    CWidgetMenu           * _Settings;
    CWidgetButton         * _StartFreq;
    CWidgetLabel          * _StartFreqLabel;
    CWidgetEdit           * _StartFreqEdit;
    CWidgetLabel          * _StartFreqUnits;
    CWidgetButton         * _EndFreq;
    CWidgetLabel          * _EndFreqLabel;
    CWidgetEdit           * _EndFreqEdit;
    CWidgetLabel          * _EndFreqUnits;
    CWidgetButton         * _StepFreq;
    CWidgetLabel          * _StepFreqLabel;
    CWidgetEdit           * _StepFreqEdit;
    CWidgetLabel          * _StepFreqUnits;
    CWidgetCheckButton    * _Append;
    CWidgetButton         * _Mode;
    CWidgetLabel          * _ModeLabel;
    CWidgetPopupList      * _ModePopup;
    CWidgetCheckButton    * _Mode8;
    CWidgetCheckButton    * _Mode16;
    CWidgetButton         * _Scan;
    CWidgetButton         * _Back;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_PANEL_SCAN_VSB_H__ */

