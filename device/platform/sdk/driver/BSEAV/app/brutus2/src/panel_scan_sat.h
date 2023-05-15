/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel_scan_sat.h $
* $brcm_Revision: 2 $
* $brcm_Date: 8/2/12 11:14a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel_scan_sat.h $
* 
* 2   8/2/12 11:14a tokushig
* SW7231-749: add missing destructor and enable last focus support
* 
* 1   7/11/12 11:34a tokushig
* SW7231-749: update sat scan to new panel class
* 
* 
*********************************************************************************/

#ifndef BRUTUS_PANEL_SCAN_SAT_H__
#define BRUTUS_PANEL_SCAN_SAT_H__

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


class CPanelScanSat : public CPanel
{
public:
    CPanelScanSat(bwidget_engine_t   engine, 
                  CWidgetBase      * pParentWidget, 
                  MRect              geometry,
                  bwin_font_t        font,
                  bwin_t             parentWin = NULL); /* optional */
    ~CPanelScanSat(void);

    eRet initialize(CModel * pModel, CModelConfig * pModelConfig);
    void uninitialize(void);

    void onClick(bwidget_t widget);
    void processNotification(CNotification & notification);

protected:
    CWidgetMenu           * _pScanSatMenu;
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
    CWidgetButton         * _Scan;
    CWidgetButton         * _Back;
    CWidgetModalMsgBox    * _MsgBox;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_PANEL_SCAN_SAT_H__ */

