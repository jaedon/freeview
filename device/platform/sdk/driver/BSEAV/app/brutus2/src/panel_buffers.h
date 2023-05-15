/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel_buffers.h $
* $brcm_Revision: 4 $
* $brcm_Date: 10/19/12 10:36a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel_buffers.h $
* 
* 4   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 3   9/10/12 4:01p tokushig
* SW7231-749: update GUI panels to support simple audio/video decoders
* 
* 2   8/2/12 11:15a tokushig
* SW7231-749: add missing destructor and disable last focus support
* 
* 1   7/13/12 5:21p tokushig
* SW7231-749: integrate main screen widget code into CScreenMain.
* 
* 
*********************************************************************************/

#ifndef BRUTUS_PANEL_BUFFERS_H__
#define BRUTUS_PANEL_BUFFERS_H__

#include "panel.h"
#include "timer.h"
#include "widget_label.h"
#include "widget_button.h"
#include "widget_progress.h"
#include "widget_meter.h"

#ifdef __cplusplus
extern "C" {
#endif


class CPanelBuffers : public CPanel
{
public:
    CPanelBuffers(bwidget_engine_t   engine, 
                  CWidgetBase      * pParentWidget, 
                  MRect              geometry,
                  bwin_font_t        font,
                  bwin_t             parentWin = NULL); /* optional */
    ~CPanelBuffers(void);

    eRet initialize(CModel * pModel, CModelConfig * pModelConfig);
    void uninitialize(void);

    void onClick(bwidget_t widget);
    void processNotification(CNotification & notification);
    void show(bool bShow);

    bool isPlaybackActive(void);
    void layoutBuffersMenu(void);
    eRet updateBuffersData(void);

protected:
    CWidgetLabel    * _pBuffersMenu;

    CWidgetLabel    * _Bkgnd;
    CWidgetLabel    * _LabelSeparator1;
    CWidgetProgress * _VideoMeter;
    CWidgetLabel    * _VideoLabel;
    CWidgetLabel    * _VideoBufferLabel;
    CWidgetLabel    * _VideoValue;
    CWidgetLabel    * _LabelSeparator2;
    CWidgetLabel    * _VideoPtsStcMinLabel;
    CWidgetLabel    * _VideoPtsStcMaxLabel;
    CWidgetLabel    * _VideoPtsStcLabel;
    CWidgetMeter    * _VideoPtsStcMeter;
    CWidgetProgress * _AudioMeter;
    CWidgetLabel    * _AudioLabel;
    CWidgetLabel    * _AudioBufferLabel;
    CWidgetLabel    * _AudioValue;
    CWidgetLabel    * _LabelSeparator3;
    CWidgetLabel    * _AudioPtsStcMinLabel;
    CWidgetLabel    * _AudioPtsStcMaxLabel;
    CWidgetLabel    * _AudioPtsStcLabel;
    CWidgetMeter    * _AudioPtsStcMeter;
    CWidgetLabel    * _PlaybackLabel;
    CWidgetProgress * _PlaybackMeter;
    CWidgetLabel    * _PlaybackBufferLabel;
    CWidgetLabel    * _PlaybackValue;
    CWidgetLabel    * _LabelSeparator4;

    CWidgetButton   * _Back;

    uint32_t          _graphicsWidth;
    uint32_t          _graphicsHeight;
    CTimer            _timerBuffers;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_PANEL_BUFFERS_H__ */

