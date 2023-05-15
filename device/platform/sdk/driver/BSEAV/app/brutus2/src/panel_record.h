/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel_record.h $
* $brcm_Revision: 1 $
* $brcm_Date: 9/17/12 10:30a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel_record.h $
* 
* 1   9/17/12 10:30a jrubio
* SW7346-1004: add record panel
* 
* 
*********************************************************************************/

#ifndef BRUTUS_PANEL_RECORD_H__
#define BRUTUS_PANEL_RECORD_H__

#include "panel.h"
#include "timer.h"
#include "videolist.h"
#include "mlist.h"

#include "widget_label.h"
#include "widget_button.h"
#include "widget_menu.h"

#ifdef __cplusplus
extern "C" {
#endif


class CPanelRecord : public CPanel
{
public:
    CPanelRecord(bwidget_engine_t   engine, 
                   CWidgetBase      * pParentWidget, 
                   MRect              geometry,
                   bwin_font_t        font,
                   bwin_t             parentWin = NULL); /* optional */
    ~CPanelRecord(void);

    eRet initialize(CModel * pModel, CModelConfig * pModelConfig);
    void uninitialize(void);

    void onClick(bwidget_t widget);
    void processNotification(CNotification & notification);

protected:
    CWidgetMenu               * _pRecordMenu;
    CWidgetButton             * _Back;
    MAutoList <CWidgetButton>   _Buttons;

};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_PANEL_RECORD_H__ */

