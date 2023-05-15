/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel_display.h $
* $brcm_Revision: 1 $
* $brcm_Date: 10/7/12 3:38p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel_display.h $
* 
* 1   10/7/12 3:38p tokushig
* SW7231-749: add display panel
* 
* 1   9/21/12 8:08p tokushig
* SW7231-749: add audio panel - currently only pid channel control is
* working
* 
*********************************************************************************/

#ifndef BRUTUS_PANEL_DISPLAY_H__
#define BRUTUS_PANEL_DISPLAY_H__

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


class CPanelDisplay : public CPanel
{
public:
    CPanelDisplay(bwidget_engine_t   engine, 
                  CWidgetBase      * pParentWidget, 
                  MRect              geometry,
                  bwin_font_t        font,
                  bwin_t             parentWin = NULL); /* optional */
    ~CPanelDisplay(void);

    eRet initialize(CModel * pModel, CModelConfig * pModelConfig);
    void uninitialize(void);

    void onClick(bwidget_t widget);
    void processNotification(CNotification & notification);

protected:
    CWidgetMenu                 * _pDisplayMenu;
    CWidgetMenu                 * _settings;

    CWidgetButton               * _Format;
    CWidgetLabel                * _FormatLabel;
    CWidgetPopupList            * _FormatPopup;
    CWidgetCheckButton          * _AutoFormat;
    CWidgetButton               * _Color;
    CWidgetLabel                * _ColorLabel;
    CWidgetPopupList            * _ColorPopup;
    CWidgetCheckButton          * _Deinterlacer;
    CWidgetCheckButton          * _BoxDetect;
    CWidgetCheckButton          * _MpaaDecimation;
    CWidgetButton               * _AspectRatio;
    CWidgetLabel                * _AspectRatioLabel;
    CWidgetPopupList            * _AspectRatioPopup;

    CWidgetModalMsgBox          * _MsgBox;
     CWidgetButton              * _Back;

    NEXUS_VideoFormat             _lastVideoFormat;
    bool                          _showFormatConfirmMsgBox;
    CTimer                        _timerMsgBox;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_PANEL_DISPLAY_H__ */

