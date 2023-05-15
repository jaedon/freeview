/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel_tuner.h $
* $brcm_Revision: 2 $
* $brcm_Date: 10/8/12 10:54p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel_tuner.h $
* 
* 2   10/8/12 10:54p jrubio
* SW7346-1004: fix panel tuner
* 
* 1   10/8/12 4:32p jrubio
* SW7346-1004: add tuner panel
* 
* 
* 
*********************************************************************************/

#ifndef BRUTUS_PANEL_TUNER_H__
#define BRUTUS_PANEL_TUNER_H__

#include "panel.h"
#include "timer.h"
#include "mlist.h"
#include "tuner.h"

#include "widget_label.h"
#include "widget_button.h"
#include "widget_menu.h"
#include "widget_grid.h"


#ifdef __cplusplus
extern "C" {
#endif

class CPanelTuner : public CPanel
{
public:
    CPanelTuner(bwidget_engine_t   engine, 
                   CWidgetBase      * pParentWidget, 
                   MRect              geometry,
                   bwin_font_t        font,
                   bwin_t             parentWin = NULL); /* optional */
    ~CPanelTuner(void);
    void show(bool bShow);

    eRet initialize(CModel * pModel, CModelConfig * pModelConfig);
    void uninitialize(void);

    void onClick(bwidget_t widget);
    void processNotification(CNotification & notification);
    eRet updateGridData(void);
        
protected:
    CWidgetMenu                 * _pTunerMenu;
    CWidgetButton               * _Back;
    CWidgetGrid                 * _pTunerGrid;
    CTimer                        _timerGrid;
    uint32_t                      _numPoints;
    NEXUS_FrontendSoftDecision  * _pSoftDecisions;
    int                           _counter;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_PANEL_TUNER_H__ */

