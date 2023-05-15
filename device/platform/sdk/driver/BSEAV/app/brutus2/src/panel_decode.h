/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel_decode.h $
* $brcm_Revision: 2 $
* $brcm_Date: 8/2/12 11:14a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel_decode.h $
* 
* 2   8/2/12 11:14a tokushig
* SW7231-749: add missing destructor and enable last focus support
* 
* 1   7/11/12 11:35a tokushig
* SW7231-749: refactor decode screen to panel based class
* 
* 
*********************************************************************************/

#ifndef BRUTUS_PANEL_DECODE_H__
#define BRUTUS_PANEL_DECODE_H__

#include "panel.h"
#include "timer.h"
#include "widget_label.h"
#include "widget_button.h"
#include "widget_menu.h"

#ifdef __cplusplus
extern "C" {
#endif


class CPanelDecode : public CPanel
{
public:
    CPanelDecode(bwidget_engine_t   engine, 
                 CWidgetBase      * pParentWidget, 
                 MRect              geometry,
                 bwin_font_t        font,
                 bwin_t             parentWin = NULL); /* optional */
    ~CPanelDecode(void);

    eRet initialize(CModel * pModel, CModelConfig * pModelConfig);
    void uninitialize(void);

    void onClick(bwidget_t widget);
    void processNotification(CNotification & notification);

    void layoutInfoMenu(void);

protected:
    CWidgetMenu           * _pDecodeMenu;

    CWidgetMenu           * _Video;
    CWidgetButton         * _Video_pid;
    CWidgetLabel          * _Video_pidLabel;
    CWidgetLabel          * _Video_pidValue;
    CWidgetButton         * _Video_codec;
    CWidgetLabel          * _Video_codecLabel;
    CWidgetLabel          * _Video_codecValue;
    CWidgetButton         * _Video_size;
    CWidgetLabel          * _Video_sizeLabel;
    CWidgetLabel          * _Video_sizeValue;
    CWidgetButton         * _Video_framerate;
    CWidgetLabel          * _Video_framerateLabel;
    CWidgetLabel          * _Video_framerateValue;
    CWidgetButton         * _Video_aspect;
    CWidgetLabel          * _Video_aspectLabel;
    CWidgetLabel          * _Video_aspectValue;

    CWidgetMenu           * _Audio;
    CWidgetButton         * _Audio_pid;
    CWidgetLabel          * _Audio_pidLabel;
    CWidgetLabel          * _Audio_pidValue;
    CWidgetButton         * _Audio_codec;
    CWidgetLabel          * _Audio_codecLabel;
    CWidgetLabel          * _Audio_codecValue;
    CWidgetButton         * _Audio_channels;
    CWidgetLabel          * _Audio_channelsLabel;
    CWidgetLabel          * _Audio_channelsValue;
    CWidgetButton         * _Audio_sample;
    CWidgetLabel          * _Audio_sampleLabel;
    CWidgetLabel          * _Audio_sampleValue;

    CWidgetMenu           * _Pcr;
    CWidgetButton         * _Pcr_pid;
    CWidgetLabel          * _Pcr_pidLabel;
    CWidgetLabel          * _Pcr_pidValue;

    CWidgetButton         * _Back;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_PANEL_DECODE_H__ */

