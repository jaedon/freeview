/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: panel_playback.h $
* $brcm_Revision: 2 $
* $brcm_Date: 8/2/12 11:14a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/panel_playback.h $
* 
* 2   8/2/12 11:14a tokushig
* SW7231-749: add missing destructor and enable last focus support
* 
* 1   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 
*********************************************************************************/

#ifndef BRUTUS_PANEL_PLAYBACK_H__
#define BRUTUS_PANEL_PLAYBACK_H__

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


class CPanelPlayback : public CPanel
{
public:
    CPanelPlayback(bwidget_engine_t   engine, 
                   CWidgetBase      * pParentWidget, 
                   MRect              geometry,
                   bwin_font_t        font,
                   bwin_t             parentWin = NULL); /* optional */
    ~CPanelPlayback(void);

    eRet initialize(CModel * pModel, CModelConfig * pModelConfig);
    void uninitialize(void);

    void onClick(bwidget_t widget);
    void processNotification(CNotification & notification);
    void refreshPlaybackButtonList(void);

protected:
    CWidgetMenu               * _pPlaybackMenu;
    CWidgetButton             * _Back;
    MAutoList <CWidgetButton>   _Buttons;
    CPlaybackList             * _pList;

};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_PANEL_PLAYBACK_H__ */

