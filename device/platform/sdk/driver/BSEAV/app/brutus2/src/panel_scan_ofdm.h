/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel_scan_ofdm.h $
* $brcm_Revision: 2 $
* $brcm_Date: 8/2/12 11:16a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel_scan_ofdm.h $
* 
* 2   8/2/12 11:16a tokushig
* SW7231-749: add missing destructor and enable last focus support
* 
* 1   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 
*********************************************************************************/

#ifndef BRUTUS_PANEL_SCAN_OFDM_H__
#define BRUTUS_PANEL_SCAN_OFDM_H__

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


class CPanelScanOfdm : public CPanel
{
public:
    CPanelScanOfdm(bwidget_engine_t   engine, 
                   CWidgetBase      * pParentWidget, 
                   MRect              geometry,
                   bwin_font_t        font,
                   bwin_t             parentWin = NULL); /* optional */
    ~CPanelScanOfdm(void);

    eRet initialize(CModel * pModel, CModelConfig * pModelConfig);
    void uninitialize(void);

    void onClick(bwidget_t widget);
    void processNotification(CNotification & notification);

protected:
    CWidgetMenu           * _pScanOfdmMenu;
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
    CWidgetCheckButton    * _ModeDVBT;
    CWidgetCheckButton    * _ModeDVBT2;
    CWidgetCheckButton    * _ModeISDBT;
    CWidgetButton         * _Scan;
    CWidgetButton         * _Back;
    CWidgetModalMsgBox    * _MsgBox;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_PANEL_SCAN_OFDM_H__ */

