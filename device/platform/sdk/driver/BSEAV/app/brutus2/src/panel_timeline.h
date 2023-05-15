/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel_timeline.h $
* $brcm_Revision: 1 $
* $brcm_Date: 10/3/12 5:29p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel_timeline.h $
* 
* 1   10/3/12 5:29p jrubio
* SW7346-1004: add panel_timeline
* 
* 
* 
*********************************************************************************/

#ifndef BRUTUS_PANEL_TIMELINE_H__
#define BRUTUS_PANEL_TIMELINE_H__

#include "panel.h"
#include "timer.h"
#include "widget_label.h"
#include "widget_button.h"
#include "widget_progress.h"
#include "widget_meter.h"

#ifdef __cplusplus
extern "C" {
#endif


class CPanelTimeline : public CPanel
{
public:
    CPanelTimeline(bwidget_engine_t   engine, 
                  CWidgetBase      * pParentWidget, 
                  MRect              geometry,
                  bwin_font_t        font,
                  bwin_t             parentWin = NULL); /* optional */
    ~CPanelTimeline(void);

    eRet initialize(CModel * pModel, CModelConfig * pModelConfig);
    void uninitialize(void);

    void onClick(bwidget_t widget);
    void processNotification(CNotification & notification);
    void show(bool bShow);
    
    bool isPlaybackActive(void);
    eRet updateBuffersData(void);

protected:
    MString printTime(int format);

    CWidgetProgress * _PlaybackMeter;    
    CWidgetLabel    * _PlaybackTime; 
    CWidgetLabel    * _PlaybackSymbol;
    CWidgetLabel    * _PlaybackName;


    uint32_t          _graphicsWidth;
    uint32_t          _graphicsHeight;
    CTimer            _timerBuffers;
    bool              _timeSet;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_PANEL_TIMELINE_H__ */

