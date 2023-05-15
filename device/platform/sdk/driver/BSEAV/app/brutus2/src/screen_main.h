/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: screen_main.h $
* $brcm_Revision: 36 $
* $brcm_Date: 11/15/12 10:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/screen_main.h $
* 
* 36   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 35   10/11/12 7:38p tokushig
* SW7231-749: make record indicator capable of showing multiple
* recordings
* 
* 34   10/11/12 4:05p tokushig
* SW7231-749: add record indicator
* 
* 33   10/8/12 9:18a jrubio
* SW7346-1004: add tuner constallation menu
* 
* 32   10/7/12 3:10p tokushig
* SW7231-749: add display menu
* 
* 32   10/7/12 3:09p tokushig
* SW7231-749: add display menu
* 
* 32   10/7/12 3:08p tokushig
* SW7231-749: add display menu
* 
* 31   10/3/12 5:15p jrubio
* SW7346-1004: add pvr timeline
* 
* 30   9/21/12 8:27p tokushig
* SW7231-749: add audio panel
* 
* 30   9/21/12 8:25p tokushig
* SW7231-749: add audio panel
* 
* 30   9/21/12 8:24p tokushig
* SW7231-749: add audio panel
* 
* 29   9/17/12 10:28a jrubio
* SW7346-1004: record screen
* 
* 28   8/2/12 10:09a tokushig
* SW7231-749: simplify panel handling in main screen
* 
* 27   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 26   7/13/12 5:21p tokushig
* SW7231-749: integrate main screen widget code into CScreenMain.
* 
* 25   7/12/12 12:26p tokushig
* SW7231-749: refactor buffer screen to be panel based.
* 
* 24   7/11/12 11:35a tokushig
* SW7231-749: refactor decode screen to panel based class
* 
* 23   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 22   6/26/12 3:59p tokushig
* SW7231-749: Add modal message box to scan qam menu when setting annex
* to A or B.  the msg box will ask the user to change the bandwidth to a
* value that coincides.
* 
* 21   5/25/12 2:05a tokushig
* SW7231-749: split scan menu
* 
* 21   5/25/12 2:00a tokushig
* SW7231-749: split scan menu.
* 
* 20   5/10/12 3:45p tokushig
* SW7231-749: add buffers/tsm popup including new meter widget
* 
* 19   5/4/12 2:04p tokushig
* SW7231-749: add buffers screen, checkbox widget, and new fonts
* 
* 18   4/20/12 5:25p tokushig
* SW7405-5581: fix compiler warning on 97405
* 
* 17   4/20/12 2:45p tokushig
* SW7231-749: add support for scan menu
* 
* 16   4/19/12 7:57p tokushig
* SW7231-749: add dynamic scan options to main menu.  buttons will only
* correspond to allowed scan types based on the types of available
* tuners
* 
* 16   4/19/12 7:52p tokushig
* SW7231-749: add dynamic scan options to main menu.  buttons will only
* correspond to allowed scan types based on the types of available
* tuners
* 
* 15   4/18/12 5:19p jrubio
* SW7346-644: remove playback menu items use PlaybackButtonList
* 
* 14   4/17/12 4:36p tokushig
* SW7231-749: menu button on remote will show/hide menu.  on screen
* channel number will timeout in 3 secs if the menu is not shown.  if
* the menu is shown, the channel number will always be displayed.
* 
* 13   4/15/12 9:53p tokushig
* SW7231-749: added decoder info menu, split notification handling in
* screen_main, and modified main menu button sizes
* 
* 12   4/14/12 9:10p tokushig
* SW7231-749: split off menu screen implementation code into separate
* files
* 
* 10   4/13/12 7:29p tokushig
* SW7231-749: update screen_main to show list view widget
* 
* 9   4/11/12 12:16p tokushig
* SW7231-749: updated screens to use new label/button widget classes
* 
* 8   4/6/12 12:43p tokushig
* SW7231-749: fixed callback handling issue in register/notification
* mechanism which prevented control from manually notifying views.  view
* and controller classes now hide the callback mechanism from the mvc
* based classes for notifications and there is a consistent
* processNotification() method for both views and controllers (models do
* not receive notifications.
* 
* 7   3/2/12 3:54p tokushig
* SW7405-5581: propagate key presses to active window and its parents if
* necessary.  allow controller last chance to act on key presses ignored
* by screens/bwidgets.  move some callback code to cscreen to hide from
* individual screen classes.
* 
* 6   3/1/12 3:56p tokushig
* SW7405-5581: moved qam scan into separate thread to enable screen
* updates while scan is running.  added on screen scan progress to test.
* NOTE: this does not yet have protection against conflicts so trying to
* channel up/down while scan is running will result in a crash
* 
* 5   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef BRUTUS_SCREEN_MAIN_H__
#define BRUTUS_SCREEN_MAIN_H__

#include "screen.h"
#include "timer.h"
#include "mlist.h"
#include "widget_label.h"
#include "widget_button.h"
#include "widget_check_button.h"
#include "widget_menu.h"
#include "widget_progress.h"
#include "widget_meter.h"
#include "widget_edit.h"
#include "widget_popup_list.h"
#include "widget_modalmsgbox.h"

#include "panel_display.h"
#include "panel_decode.h"
#include "panel_audio.h"
#include "panel_playback.h"
#if NEXUS_HAS_FRONTEND
#include "panel_tuner.h"
#include "panel_scan_qam.h"
#include "panel_scan_vsb.h"
#include "panel_scan_sat.h"
#include "panel_scan_ofdm.h"
#endif
#include "panel_record.h"
#include "panel_buffers.h"
#include "panel_timeline.h"

#ifdef __cplusplus
extern "C" {
#endif

class CModelChannel;

class CScreenMain : public CScreen
{
public:
    CScreenMain(bwidget_engine_t widgetEngine, CModelConfig * pModelConfig, CWidgetBase *pParent); 
    ~CScreenMain(void);

    eRet initialize(CModel * pModel);
    void uninitialize();

    void onClick(bwidget_t widget);

    eRet onKeyDown(bwidget_t widget, bwidget_key key);

    void processNotification(CNotification & notification);

    void setChannelNum(const char * strChannelNum);
    void setChannelNum(CModelChannel * pChannel);
    void setRecordIndicator(CModelChannelMgr * pChannelMgr);
#if NEXUS_HAS_FRONTEND
    void setScanProgress(CTuner * pTuner, uint8_t progress);
#endif

    virtual void show(bool bShow);
    virtual bool isVisible(void);

    eRet registerObserver(CObserver * pObserver, eNotification notification = eNotify_All);

protected:
    bool isPlaybackActive(void);
    void showMenu(CWidgetMenu * pMenu);

protected:
    CWidgetProgress           * _pLabelScan;

    MRect                       _rectChannelNum;
    MRect                       _rectChText;
    CWidgetLabel              * _pLabelChannelNum;
    CWidgetLabel              * _pLabelChannelNumText;

    CWidgetMenu               * _pMainMenu;
    CWidgetButton             * _Display;
    CWidgetButton             * _Decode;
    CWidgetButton             * _Playback;
    CWidgetButton             * _Audio;
    CWidgetButton             * _Record;
#if NEXUS_HAS_FRONTEND
    CWidgetButton             * _Tuner;
    CWidgetButton             * _ScanQam;
    CWidgetButton             * _ScanVsb;
    CWidgetButton             * _ScanSat;
    CWidgetButton             * _ScanOfdm;
#endif
    CWidgetCheckButton        * _Buffers;
    CWidgetCheckButton        * _Timeline;
    CWidgetButton             * _Back;

    CPanelDecode              * _pDecodeMenu;
    CPanelDisplay             * _pDisplayMenu;
    CPanelAudio               * _pAudioMenu;
    CPanelPlayback            * _pPlaybackMenu;
#if NEXUS_HAS_FRONTEND
    CPanelTuner               * _pTunerMenu;
    CPanelScanQam             * _pScanQamMenu;
    CPanelScanVsb             * _pScanVsbMenu;
    CPanelScanSat             * _pScanSatMenu;
    CPanelScanOfdm            * _pScanOfdmMenu;
#endif
    CPanelRecord              * _pRecordMenu;
    CPanelBuffers             * _pBuffersMenu;
    CPanelTimeline            * _pTimelineMenu;

    MAutoList <CWidgetLabel>    _recIndicatorList;
    MList <CPanel>              _panelList;
    CTimer                      _timerChannel;
};


#ifdef __cplusplus
}
#endif

#endif /* BRUTUS_SCREEN_MAIN_H__ */

