/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel_audio.h $
* $brcm_Revision: 2 $
* $brcm_Date: 10/7/12 3:38p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel_audio.h $
* 
* 2   10/7/12 3:38p tokushig
* SW7231-749: remove unused widget
* 
* 1   9/21/12 8:08p tokushig
* SW7231-749: add audio panel - currently only pid channel control is
* working
* 
*********************************************************************************/

#ifndef BRUTUS_PANEL_AUDIO_H__
#define BRUTUS_PANEL_AUDIO_H__

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


class CPanelAudio : public CPanel
{
public:
    CPanelAudio(bwidget_engine_t   engine, 
                CWidgetBase      * pParentWidget, 
                MRect              geometry,
                bwin_font_t        font,
                bwin_t             parentWin = NULL); /* optional */
    ~CPanelAudio(void);

    eRet initialize(CModel * pModel, CModelConfig * pModelConfig);
    void uninitialize(void);

    void onClick(bwidget_t widget);
    void processNotification(CNotification & notification);

protected:
    CWidgetMenu                 * _pAudioMenu;
    CWidgetMenu                 * _settings;
    CWidgetCheckButton          * _dialogNorm;

    CWidgetButton               * _Pid;
    CWidgetLabel                * _PidLabel;
    CWidgetPopupList            * _PidPopup;
    CWidgetButton               * _Spdif;
    CWidgetLabel                * _SpdifLabel;
    CWidgetPopupList            * _SpdifPopup;
    CWidgetButton               * _Hdmi;
    CWidgetLabel                * _HdmiLabel;
    CWidgetPopupList            * _HdmiPopup;
    CWidgetButton               * _Downmix;
    CWidgetLabel                * _DownmixLabel;
    CWidgetPopupList            * _DownmixPopup;
    CWidgetButton               * _DualMono;
    CWidgetLabel                * _DualMonoLabel;
    CWidgetPopupList            * _DualMonoPopup;
    CWidgetButton               * _Compression;
    CWidgetLabel                * _CompressionLabel;
    CWidgetPopupList            * _CompressionPopup;

    CWidgetButton               * _Back;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_PANEL_AUDIO_H__ */

