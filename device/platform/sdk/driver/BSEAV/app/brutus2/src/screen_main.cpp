/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: screen_main.cpp $
* $brcm_Revision: 47 $
* $brcm_Date: 11/15/12 10:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/screen_main.cpp $ 
* 
* 47   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 46   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 45   10/11/12 7:38p tokushig
* SW7231-749: make record indicator capable of showing multiple
* recordings
* 
* 44   10/11/12 4:05p tokushig
* SW7231-749: add record indicator
* 
* 43   10/11/12 2:38p jrubio
* SW7346-1004: rename Playback Notification
* 
* 42   10/9/12 1:20p tokushig
* SW7231-749: change debug printout level
* 
* 41   10/8/12 4:23p jrubio
* SW7346-1004: fix compile issue
* 
* 40   10/8/12 2:58p tokushig
* SW7231-749: update button labels
* 
* 40   10/8/12 2:56p tokushig
* SW7231-749: update button labels
* 
* 39   10/8/12 9:18a jrubio
* SW7346-1004: add tuner constallation menu
* 
* 38   10/7/12 3:09p tokushig
* SW7231-749: add display menu
* 
* 38   10/7/12 3:08p tokushig
* SW7231-749: add display menu
* 
* 37   10/3/12 5:15p jrubio
* SW7346-1004: add pvr timeline
* 
* 36   9/21/12 8:27p tokushig
* SW7231-749: add audio panel
* 
* 36   9/21/12 8:25p tokushig
* SW7231-749: add audio panel
* 
* 36   9/21/12 8:24p tokushig
* SW7231-749: add audio panel
* 
* 35   9/17/12 10:31a jrubio
* SW7346-1004: add record panel
* 
* 34   9/15/12 1:10a tokushig
* SW7231-749: get fonts from graphics object
* 
* 33   9/10/12 4:21p tokushig
* SW7231-749: replace channel number with playback on screen when playing
* back a file.
* 
* 32   8/8/12 6:05p tokushig
* SW7231-749: add support for named widgets in listview widgets.  this is
* used to support menu widgets that allow the user to select the visible
* widgets.
* 
* 31   8/2/12 10:09a tokushig
* SW7231-749: simplify panel handling in main screen
* 
* 31   8/2/12 10:07a tokushig
* SW7231-749: simplify panel handling in main screen
* 
* 30   7/27/12 11:16a jrubio
* SW7346-848: remove debug
* 
* 29   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 28   7/13/12 5:21p tokushig
* SW7231-749: integrate main screen widget code into CScreenMain.
* 
* 27   7/12/12 12:26p tokushig
* SW7231-749: refactor buffer screen to be panel based.
* 
* 26   7/11/12 11:35a tokushig
* SW7231-749: refactor decode screen to panel based class
* 
* 25   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 24   6/26/12 3:59p tokushig
* SW7231-749: Add modal message box to scan qam menu when setting annex
* to A or B.  the msg box will ask the user to change the bandwidth to a
* value that coincides.
* 
* 23   5/25/12 11:24a tokushig
* SW7231-749: adjust size of playback menu and playback menu items.  also
* enlarge channel number banner to better show status during playback.
* 
* 22   5/25/12 2:06a tokushig
* SW7231-749: split scan menu
* 
* 22   5/25/12 2:00a tokushig
* SW7231-749: split scan menu.
* 
* 21   5/10/12 3:45p tokushig
* SW7231-749: add buffers/tsm popup including new meter widget
* 
* 20   5/4/12 2:04p tokushig
* SW7231-749: add buffers screen, checkbox widget, and new fonts
* 
* 19   4/20/12 5:03p tokushig
* SW7405-5581: fix problem with uninitialized playback list pointer
* 
* 18   4/20/12 2:45p tokushig
* SW7231-749: add support for scan menu
* 
* 17   4/19/12 7:57p tokushig
* SW7231-749: add dynamic scan options to main menu.  buttons will only
* correspond to allowed scan types based on the types of available
* tuners
* 
* 17   4/19/12 7:52p tokushig
* SW7231-749: add dynamic scan options to main menu.  buttons will only
* correspond to allowed scan types based on the types of available
* tuners
* 
* 16   4/19/12 9:59a jrubio
* SW7346-644: remove playback menu items use PlaybackButtonList
* 
* 15   4/17/12 4:36p tokushig
* SW7231-749: menu button on remote will show/hide menu.  on screen
* channel number will timeout in 3 secs if the menu is not shown.  if
* the menu is shown, the channel number will always be displayed.
* 
* 14   4/15/12 9:53p tokushig
* SW7231-749: added decoder info menu, split notification handling in
* screen_test, and modified main menu button sizes
* 
* 13   4/14/12 9:10p tokushig
* SW7231-749: split off menu screen implementation code into separate
* files
* 
* 12   4/14/12 5:28p tokushig
* SW7231-749: updated screen and test screen classes to work with new
* widget classes.
* 
* 10   4/11/12 12:16p tokushig
* SW7231-749: updated screens to use new label/button widget classes
* 
* 9   4/6/12 12:43p tokushig
* SW7231-749: fixed callback handling issue in register/notification
* mechanism which prevented control from manually notifying views.  view
* and controller classes now hide the callback mechanism from the mvc
* based classes for notifications and there is a consistent
* processNotification() method for both views and controllers (models do
* not receive notifications.
* 
* 8   3/13/12 4:00p tokushig
* SW7405-5581: more CModelChannel base class out of channelmgr file and
* into channel file
* 
* 7   3/2/12 3:53p tokushig
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
* 
*********************************************************************************/

#include "brutus.h"
#include "brutus_os.h"
#include "screen_main.h"
#include "channel.h"
#include "channelmgr.h"
#include "timer.h"

#include "widget_edit.h"
#include "widget_check_button.h"

BDBG_MODULE(brutus_screen_main);


CScreenMain::CScreenMain(bwidget_engine_t widgetEngine, CModelConfig * pModelConfig, CWidgetBase *pParent) : 
    CScreen(widgetEngine, pModelConfig, pParent),
    _pLabelScan(NULL),

    _pLabelChannelNum(NULL),
    _pLabelChannelNumText(NULL),

    _pMainMenu(NULL),
    _Display(NULL),
    _Decode(NULL),
    _Audio(NULL),
    _Playback(NULL),
#if NEXUS_HAS_FRONTEND
    _Tuner(NULL),
    _ScanQam(NULL),
    _ScanVsb(NULL),
    _ScanSat(NULL),
    _ScanOfdm(NULL),
#endif 
    _Buffers(NULL),
    _Back(NULL),

    _pDecodeMenu(NULL),
    _pDisplayMenu(NULL),
    _pAudioMenu(NULL),
    _pPlaybackMenu(NULL),
    _pRecordMenu(NULL),
#if NEXUS_HAS_FRONTEND
    _pTunerMenu(NULL),
    _pScanVsbMenu(NULL),
    _pScanQamMenu(NULL),
    _pScanSatMenu(NULL),
    _pScanOfdmMenu(NULL),
#endif
    _pBuffersMenu(NULL),
    _pTimelineMenu(NULL),

    _timerChannel(widgetEngine, this, 3000)
{
}

eRet CScreenMain::initialize(CModel * pModel)
{
    eRet                  ret            = eRet_Ok;
    CGraphics           * pGraphics      = NULL;
    bbutton_settings      buttonSettings;
    blabel_settings       labelSettings;
    bwin_font_t           font12;
    bwin_font_t           font14;
    bwin_font_t           font17;
    uint32_t              graphicsWidth  = 0;
    uint32_t              graphicsHeight = 0;
    bool                  tuner = false;

    CScreen::initialize(pModel); /* call base class */

    pGraphics = _pModel->getGraphics();
    BDBG_ASSERT(NULL != pGraphics);

    graphicsWidth  = GET_INT(_pCfg, GRAPHICS_SURFACE_WIDTH); 
    graphicsHeight = GET_INT(_pCfg, GRAPHICS_SURFACE_HEIGHT); 

    font12 = pGraphics->getFont(12);
    BDBG_ASSERT(NULL != font12);
    font14 = pGraphics->getFont(14);
    BDBG_ASSERT(NULL != font14);
    font17 = pGraphics->getFont(17);
    BDBG_ASSERT(NULL != font17);

    /* current channel number widget */
    _rectChannelNum.set(graphicsWidth - 150, 50, 90, 40);
    _pLabelChannelNum = new CWidgetLabel(getEngine(), this, _rectChannelNum, font17);
    CHECK_PTR_GOTO("unable to allocate label widget", _pLabelChannelNum, ret, eRet_OutOfMemory, error);
    _pLabelChannelNum->setBackgroundColor(0xFF222222);
    _pLabelChannelNum->show(false);

    _rectChText = _rectChannelNum;
    _rectChText.setX(_pLabelChannelNum->getBevel());
    _rectChText.setY(_pLabelChannelNum->getBevel());
    _rectChText.grow(_pLabelChannelNum->getBevel() * -2);
    _pLabelChannelNumText = new CWidgetLabel(getEngine(), _pLabelChannelNum, _rectChText, font17);
    CHECK_PTR_GOTO("unable to allocate label widget", _pLabelChannelNum, ret, eRet_OutOfMemory, error);
    _pLabelChannelNumText->setBevel(0);
    _pLabelChannelNumText->setText("0.0", bwidget_justify_horiz_center, bwidget_justify_vert_middle);
    _pLabelChannelNumText->setTextColor(0xFF222222);

    /* scan status widget */
    _pLabelScan = new CWidgetProgress(getEngine(), this, MRect((graphicsWidth / 2) - 200, graphicsHeight - 50, 400, 25), font14);
    CHECK_PTR_GOTO("unable to allocate label widget", _pLabelScan, ret, eRet_OutOfMemory, error);
    _pLabelScan->setText("No Scan", bwidget_justify_horiz_center, bwidget_justify_vert_middle);
    _pLabelScan->show(false);

    /* main menu */
    _pMainMenu = new CWidgetMenu(getEngine(), this, MRect(50, 50, 150, 151), font14, font12);
    CHECK_PTR_GOTO("unable to allocate menu widget", _pMainMenu, ret, eRet_OutOfMemory, error);
    _pMainMenu->setMenuTitle("Menu", "Options");
    _pMainMenu->showSubTitle(false);
    _pMainMenu->setScroll(false);
    _pMainMenu->show(false);
    {
        _Decode = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 22), font12);
        CHECK_PTR_GOTO("unable to allocate label widget", _Decode, ret, eRet_OutOfMemory, error);
        _Decode->setText("Decode", bwidget_justify_horiz_left);
        _pMainMenu->addButton(_Decode, "Decode");

        _Display = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 22), font12);
        CHECK_PTR_GOTO("unable to allocate label widget", _Display, ret, eRet_OutOfMemory, error);
        _Display->setText("Display", bwidget_justify_horiz_left);
        _pMainMenu->addButton(_Display, "Display");

        _Playback = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 22), font12);
        CHECK_PTR_GOTO("unable to allocate button widget", _Playback, ret, eRet_OutOfMemory, error);
        _Playback->setText("Playback", bwidget_justify_horiz_left);
        _pMainMenu->addButton(_Playback, "Playback");
 
        _Audio = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 22), font12);
        CHECK_PTR_GOTO("unable to allocate button widget", _Audio, ret, eRet_OutOfMemory, error);
        _Audio->setText("Audio", bwidget_justify_horiz_left);
        _pMainMenu->addButton(_Audio, "Audio");
 
        _Record = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 22), font12);
        CHECK_PTR_GOTO("unable to allocate button widget", _Record, ret, eRet_OutOfMemory, error);
        _Record->setText("Record", bwidget_justify_horiz_left);
        /*_pMainMenu->addButton(_Record, "Record"); Do not add the button */
 
#if NEXUS_HAS_FRONTEND
        /* find available tuners and add corresponding scan buttons */
        CBoardResources * pResources = _pModelConfig->getBoardResources();
        CHECK_PTR_GOTO("board resources ptr is null", _pModelConfig, ret, eRet_NotAvailable, error);
        if (true == pResources->findResource(_pModel->getId(), eBoardResource_frontendQam))
        {
            _ScanQam = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 22), font12);
            CHECK_PTR_GOTO("unable to allocate button widget", _ScanQam, ret, eRet_OutOfMemory, error);
            _ScanQam->setText("Scan QAM", bwidget_justify_horiz_left);
            _pMainMenu->addButton(_ScanQam, "Scan QAM");
            tuner = true;
        }
        
        if (true == pResources->findResource(_pModel->getId(), eBoardResource_frontendVsb))
        {
            _ScanVsb = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 22), font12);
            CHECK_PTR_GOTO("unable to allocate button widget", _ScanVsb, ret, eRet_OutOfMemory, error);
            _ScanVsb->setText("Scan VSB", bwidget_justify_horiz_left);
            _pMainMenu->addButton(_ScanVsb, "Scan VSB");
            tuner = true;
        }
       
        if (true == pResources->findResource(_pModel->getId(), eBoardResource_frontendSds))
        {
            _ScanSat = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 22), font12);
            CHECK_PTR_GOTO("unable to allocate button widget", _ScanSat, ret, eRet_OutOfMemory, error);
            _ScanSat->setText("Scan Satellite", bwidget_justify_horiz_left);
            _pMainMenu->addButton(_ScanSat, "Scan Sat");
            tuner = true;
        }
       
        if (true == pResources->findResource(_pModel->getId(), eBoardResource_frontendOfdm))
        {
            _ScanOfdm = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 22), font12);
            CHECK_PTR_GOTO("unable to allocate button widget", _ScanOfdm, ret, eRet_OutOfMemory, error);
            _ScanOfdm->setText("Scan OFDM", bwidget_justify_horiz_left);
            _pMainMenu->addButton(_ScanOfdm, "Scan OFDM");
            tuner = true;
        }

        if (true == tuner)
        {
           _Tuner = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 22), font12);
           CHECK_PTR_GOTO("unable to allocate button widget", _Audio, ret, eRet_OutOfMemory, error);
           _Tuner->setText("Tuner", bwidget_justify_horiz_left);
           _pMainMenu->addButton(_Tuner, "Tuner");
        }
#endif /* NEXUS_HAS_FRONTEND */

        _Buffers = new CWidgetCheckButton(getEngine(), this, MRect(0, 0, 0, 22), font12);
        CHECK_PTR_GOTO("unable to allocate button widget", _Buffers, ret, eRet_OutOfMemory, error);
        _Buffers->setText("Buffers / TSM", bwidget_justify_horiz_left);
        _pMainMenu->addButton(_Buffers, "Buffers / TSM");

        /* back button */
        _Back = new CWidgetButton(getEngine(), this, MRect(0, 0, 0, 0), font12);
        CHECK_PTR_GOTO("unable to allocate button widget", _Back, ret, eRet_OutOfMemory, error);
        _Back->setText("Hide");
        _pMainMenu->addBackButton(_Back);
    }

    /* initialize sub menus */
    _pDecodeMenu = new CPanelDecode(getEngine(), this, MRect(0, 0, 10, 10), font14);
    CHECK_PTR_GOTO("unable to initialize Decode menu", _pDecodeMenu, ret, eRet_OutOfMemory, error);
    _pDecodeMenu->initialize(pModel, _pModelConfig);
    _pDecodeMenu->show(false);
    _panelList.add(_pDecodeMenu);

    _pDisplayMenu = new CPanelDisplay(getEngine(), this, MRect(0, 0, 10, 10), font14);
    CHECK_PTR_GOTO("unable to initialize Display menu", _pDisplayMenu, ret, eRet_OutOfMemory, error);
    _pDisplayMenu->initialize(pModel, _pModelConfig);
    _pDisplayMenu->show(false);
    _panelList.add(_pDisplayMenu);

    _pAudioMenu = new CPanelAudio(getEngine(), this, MRect(0, 0, 10, 10), font14);
    CHECK_PTR_GOTO("unable to initialize Audio menu", _pAudioMenu, ret, eRet_OutOfMemory, error);
    _pAudioMenu->initialize(pModel, _pModelConfig);
    _pAudioMenu->show(false);
    _panelList.add(_pAudioMenu);

    _pPlaybackMenu = new CPanelPlayback(getEngine(), this, MRect(0, 0, 10, 10), font14);
    CHECK_PTR_GOTO("unable to initialize Playback menu", _pPlaybackMenu, ret, eRet_OutOfMemory, error);
    _pPlaybackMenu->initialize(pModel, _pModelConfig);
    _pPlaybackMenu->show(false);
    _panelList.add(_pPlaybackMenu);

    _pRecordMenu = new CPanelRecord(getEngine(), this, MRect(0, 0, 10, 10), font14);
    CHECK_PTR_GOTO("unable to initialize Record menu", _pRecordMenu, ret, eRet_OutOfMemory, error);
    _pRecordMenu->initialize(pModel, _pModelConfig);
    _pRecordMenu->show(false);
    _panelList.add(_pRecordMenu);

#if NEXUS_HAS_FRONTEND
    _pTunerMenu = new CPanelTuner(getEngine(), this, MRect(0, 0, 10, 10), font14);
    CHECK_PTR_GOTO("unable to initialize Playback menu", _pTunerMenu, ret, eRet_OutOfMemory, error);
    _pTunerMenu->initialize(pModel, _pModelConfig);
    _pTunerMenu->show(false);
    _panelList.add(_pTunerMenu);

    _pScanQamMenu = new CPanelScanQam(getEngine(), this, MRect(0, 0, 10, 10), font14);
    CHECK_PTR_GOTO("unable to initialize scan QAM menu", _pScanQamMenu, ret, eRet_OutOfMemory, error);
    _pScanQamMenu->initialize(pModel, _pModelConfig);
    _pScanQamMenu->show(false);
    _panelList.add(_pScanQamMenu);

    _pScanVsbMenu = new CPanelScanVsb(getEngine(), this, MRect(0, 0, 10, 10), font14);
    CHECK_PTR_GOTO("unable to initialize scan Vsb menu", _pScanVsbMenu, ret, eRet_OutOfMemory, error);
    _pScanVsbMenu->initialize(pModel, _pModelConfig);
    _pScanVsbMenu->show(false);
    _panelList.add(_pScanVsbMenu);

    _pScanSatMenu = new CPanelScanSat(getEngine(), this, MRect(0, 0, 10, 10), font14);
    CHECK_PTR_GOTO("unable to initialize scan Sat menu", _pScanSatMenu, ret, eRet_OutOfMemory, error);
    _pScanSatMenu->initialize(pModel, _pModelConfig);
    _pScanSatMenu->show(false);
    _panelList.add(_pScanSatMenu);

    _pScanOfdmMenu = new CPanelScanOfdm(getEngine(), this, MRect(0, 0, 10, 10), font14);
    CHECK_PTR_GOTO("unable to initialize scan Ofdm menu", _pScanOfdmMenu, ret, eRet_OutOfMemory, error);
    _pScanOfdmMenu->initialize(pModel, _pModelConfig);
    _pScanOfdmMenu->show(false);
    _panelList.add(_pScanOfdmMenu);
#endif /* NEXUS_HAS_FRONTEND */

    _pBuffersMenu = new CPanelBuffers(getEngine(), this, MRect(0, 0, 10, 10), font14);
    CHECK_PTR_GOTO("unable to initialize scan Sat menu", _pBuffersMenu, ret, eRet_OutOfMemory, error);
    _pBuffersMenu->initialize(pModel, _pModelConfig);
    _pBuffersMenu->show(false);
    _panelList.add(_pBuffersMenu);

    _pTimelineMenu = new CPanelTimeline(getEngine(), this, MRect(0, 0, 10, 10), font14);
    CHECK_PTR_GOTO("unable to initialize scan Sat menu", _pTimelineMenu, ret, eRet_OutOfMemory, error);
    _pTimelineMenu->initialize(pModel, _pModelConfig);
    _pTimelineMenu->show(false);
    _panelList.add(_pTimelineMenu);

    goto done;
error:
    uninitialize();
done:
    return ret;
}

void CScreenMain::uninitialize()
{
    _timerChannel.stop();
    
    DEL(_pTimelineMenu);
    DEL(_pBuffersMenu);
#if NEXUS_HAS_FRONTEND
    DEL(_pScanOfdmMenu);
    DEL(_pScanSatMenu);
    DEL(_pScanVsbMenu);
    DEL(_pScanQamMenu);
    DEL(_pTunerMenu);
#endif
    DEL(_pRecordMenu);
    DEL(_pPlaybackMenu);
    DEL(_pAudioMenu);
    DEL(_pDisplayMenu);
    DEL(_pDecodeMenu);
    DEL(_Back);
    DEL(_Buffers);
#if NEXUS_HAS_FRONTEND
    DEL(_Tuner);
    DEL(_ScanOfdm);
    DEL(_ScanSat);
    DEL(_ScanVsb);
    DEL(_ScanQam);
#endif
    DEL(_Record);
    DEL(_Audio);
    DEL(_Playback);
    DEL(_Display);
    DEL(_Decode);
    DEL(_pMainMenu);
    DEL(_pLabelScan);
    DEL(_pLabelChannelNumText);
    DEL(_pLabelChannelNum);

    _panelList.clear();
}

CScreenMain::~CScreenMain()
{
    uninitialize();
}

/* override registerObserver so we can propgate the observer to the menu panels.  this will allow
   the menu panels to notify those observers that are interested in this screens notifications. */
eRet CScreenMain::registerObserver(CObserver * pObserver, eNotification notification)
{
    MListItr <CPanel>   itr(&_panelList);
    CPanel            * pPanel              = NULL;

    BDBG_ASSERT(NULL != _pMainMenu);
    BDBG_ASSERT(0 < _panelList.total());

    CSubject::registerObserver(pObserver, notification);

    for (pPanel = itr.first(); pPanel; pPanel = itr.next())
    {
        pPanel->registerObserver(pObserver, notification);
    }
}

void CScreenMain::processNotification(CNotification & notification)
{
    MListItr <CPanel>   itr(&_panelList);
    CPanel            * pPanel              = NULL;
    eRet ret = eRet_Ok;

    BDBG_MSG(("--- CScreenMain Notification Received: %d ---", notification.getId()));

    /* we could have simply handled all the incomming notifications for all the panels
       here, but instead we'll split them off and keep the notification handling code
       and the respective panel code in the same file */
    for (pPanel = itr.first(); pPanel; pPanel = itr.next())
    {
        pPanel->processNotification(notification);
    }

    switch(notification.getId())
    {
    case eNotify_CurrentChannel:
        {
            CModelChannelMgr * pModelChannelMgr = NULL;
            CModelChannel    * pChannel         = NULL;

            pModelChannelMgr = _pModel->getModelChannelMgr();
            CHECK_PTR_GOTO("unable to get channel mgr", pModelChannelMgr, ret, eRet_NotAvailable, error);

            pChannel = pModelChannelMgr->getCurrentChannel();
            CHECK_PTR_GOTO("unable to get current channel", pChannel, ret, eRet_NotAvailable, error);

            setChannelNum(pChannel);
        }
        break;

    case eNotify_DeferredChannel:
        {
            CModelChannelMgr * pModelChannelMgr = NULL;
            CModelChannel    * pChannel         = NULL;

            pModelChannelMgr = _pModel->getModelChannelMgr();
            CHECK_PTR_GOTO("unable to get channel mgr", pModelChannelMgr, ret, eRet_NotAvailable, error);

            pChannel = pModelChannelMgr->getDeferredChannel();
            CHECK_PTR_GOTO("unable to get deferred channel", pChannel, ret, eRet_NotAvailable, error);

            setChannelNum(pChannel);
        }
        break;

    case eNotify_RecordStarted:
    case eNotify_RecordStopped:
        {
            CModelChannelMgr * pModelChannelMgr = NULL;
            CModelChannel    * pChannel         = NULL;

            pModelChannelMgr = _pModel->getModelChannelMgr();
            CHECK_PTR_GOTO("unable to get channel mgr", pModelChannelMgr, ret, eRet_NotAvailable, error);

            setRecordIndicator(pModelChannelMgr);
        }
        break;

    case eNotify_Timeout:
        {
            CTimer * pTimer = (CTimer *)notification.getData();
            if (&_timerChannel == pTimer)
            {
                /* if menu is visible do not hide channel num */
                if (false == isVisible())
                {
                    _pLabelChannelNum->show(false);
                }
            }
        }
        break;

    case eNotify_PlaybackStateChanged:
        {
            setChannelNum("Playback");
        }
        break;

#if NEXUS_HAS_FRONTEND
    case eNotify_ScanStarted:
        {
            _pLabelScan->setLevel(0);
            _pLabelScan->showProgress(true);
            _pLabelScan->show(true);
        }
        break;

    case eNotify_ScanStopped:
        {
            _pLabelScan->show(false);
        }
        break;

    case eNotify_ScanProgress:
        {
            CTunerScanNotificationData * pScanData = (CTunerScanNotificationData *)notification.getData();
            BDBG_ASSERT(pScanData);
            setScanProgress(pScanData->getTuner(), pScanData->getPercent());
        }
        break;
#endif /* NEXUS_HAS_FRONTEND */

    default:
        break;
    }

error:
    return;
}

void CScreenMain::setChannelNum(const char * strChannelNum)
{
    BDBG_ASSERT(NULL != strChannelNum);

    _pLabelChannelNumText->setText(strChannelNum);
    _pLabelChannelNum->show(true);
    _timerChannel.start(GET_INT(_pCfg, UI_CHANNEL_NUM_TIMEOUT));
}

void CScreenMain::setChannelNum(CModelChannel * pChannel)
{
    char strChNum[16];
   
    BDBG_ASSERT(NULL != pChannel);
    strChNum[0] = 0;   
    snprintf(strChNum, 16, "%d.%d", pChannel->getMajor(), pChannel->getMinor());
    setChannelNum(strChNum);
}

void CScreenMain::setRecordIndicator(CModelChannelMgr * pChannelMgr)
{
    char            strChNum[32];
    MRect           rectChNum;
    CModelChannel * pChannel            = NULL;
    bool            bRecording          = false;
    uint16_t        recIndWidgetHeight  = 20;
    CGraphics     * pGraphics           = NULL;
    bwin_font_t     font12;
    eRet            ret                 = eRet_Ok;

    BDBG_ASSERT(NULL != pChannelMgr);
    strChNum[0] = 0;   
    pGraphics = _pModel->getGraphics();
    BDBG_ASSERT(NULL != pGraphics);
    font12 = pGraphics->getFont(12);
    BDBG_ASSERT(NULL != font12);

    /* delete all previous record indicator labels */
    _recIndicatorList.clear();

    /* start with original size of channel number widget */
    rectChNum = _rectChannelNum;

    /* add record indicator for each recorded channel */
    for (pChannel = pChannelMgr->getFirstChannel();
         pChannel;
         pChannel = pChannelMgr->getNextChannel(pChannel, false))
    {
        BDBG_MSG(("is channel:%d.%d recording? %d", pChannel->getMajor(), pChannel->getMinor(), pChannel->isRecording()));
        if (false == pChannel->isRecording())
        {
            continue;
        }

        bRecording = true;
        snprintf(strChNum, 32, "Rec %d.%d", pChannel->getMajor(), pChannel->getMinor());

        /* recording so enlarge channel num widget to contain a new record indicator */
        rectChNum.setHeight(rectChNum.height() + recIndWidgetHeight);

        /* create new record indicator widget */
        {
            CWidgetLabel  * pRecordIndicator    = NULL;
            MRect           rectRecIndicator;

            rectRecIndicator = _rectChText;
            /* move rect below channel text widget */
            rectRecIndicator.setY(rectRecIndicator.y() + _rectChText.height());
            /* move rect below any existing rec indicator widgets */
            rectRecIndicator.setY(rectRecIndicator.y() + _recIndicatorList.total() * recIndWidgetHeight);

            rectRecIndicator.setHeight(recIndWidgetHeight);
            pRecordIndicator = new CWidgetLabel(getEngine(), _pLabelChannelNum, rectRecIndicator, font12);
            CHECK_PTR_GOTO("unable to allocate label widget", pRecordIndicator, ret, eRet_OutOfMemory, error);
            pRecordIndicator->setBevel(1);
            pRecordIndicator->setTextColor(pRecordIndicator->getBackgroundColor());
            pRecordIndicator->setBackgroundColor(0xFFFF0000);
            pRecordIndicator->setText(strChNum, bwidget_justify_horiz_center, bwidget_justify_vert_middle);
            pRecordIndicator->show(true);

            _recIndicatorList.add(pRecordIndicator);
        }
    }
    
error:
done:
    if (false == bRecording)
    {
        /* not recording so use original geometry */
        rectChNum = _rectChannelNum;
    }

    /* adjust channel number widget geometry to include all rec indicator widgets */
    _pLabelChannelNum->setGeometry(rectChNum);
}

#if NEXUS_HAS_FRONTEND
#define PERCENT_TO_UINT16(percent) ((percent) * 65535 / 100)
void CScreenMain::setScanProgress(CTuner * pTuner, uint8_t progress)
{
    char strScan[32];

    BDBG_ASSERT(NULL != pTuner);
    BDBG_ASSERT(100 >= progress);

    snprintf(strScan, 32, "Scanning %s%d : %3d%%", pTuner->getName(), pTuner->getNumber(), progress);
    _pLabelScan->setText(strScan);
    _pLabelScan->setLevel(PERCENT_TO_UINT16(progress));
}
#endif /* NEXUS_HAS_FRONTEND */

void CScreenMain::onClick(bwidget_t widget)
{
    eNotification   notification  = eNotify_Invalid;
    CWidgetBase   * pWidget       = NULL;

   
    /* find the widget object that corresponds to the given bwidget_t */
    {
        blabel_settings   labelSettings;
        blabel_get(widget, &labelSettings);
        pWidget = (CWidgetBase *)labelSettings.widget.data;
    }
    
    if (_Decode == pWidget)
    {
        showMenu(NULL);
        _pDecodeMenu->show(true);
    }
    else
    if (_Display == pWidget)
    {
        showMenu(NULL);
        _pDisplayMenu->show(true);
    }
    else
    if (_Audio == pWidget)
    {
        showMenu(NULL);
        _pAudioMenu->show(true);
    }
    else
    if (_Playback == pWidget)
    {
        showMenu(NULL);
        _pPlaybackMenu->show(true);
    }
    else
#if NEXUS_HAS_FRONTEND
    if (_Tuner == pWidget)
    {
        showMenu(NULL);
        _pTunerMenu->show(true);
    }
    else
#endif
    if (_Record == pWidget)
    {
        showMenu(NULL);
        _pRecordMenu->show(false);
    }
    else
#if NEXUS_HAS_FRONTEND
    if (_ScanQam == pWidget)
    {
        showMenu(NULL);
        _pScanQamMenu->show(true);
    }
    else
    if (_ScanVsb == pWidget)
    {
        showMenu(NULL);
        _pScanVsbMenu->show(true);
    }
    else
    if (_ScanSat == pWidget)
    {
        showMenu(NULL);

        _pScanSatMenu->show(true);
    }
    else
    if (_ScanOfdm == pWidget)
    {
        showMenu(NULL);

        _pScanOfdmMenu->show(true);
    }
    else
#endif
    if (_Buffers == pWidget)
    {
        _pBuffersMenu->show(_Buffers->isChecked());
    }
    else
    if (_Back == pWidget)
    {
        show(false);
    }
    else
    {
        BDBG_MSG(("Unhandled button click (%s)", __FUNCTION__));
    }

done:
    return;
}

eRet CScreenMain::onKeyDown(bwidget_t widget, bwidget_key key)
{
    BDBG_MSG(("unhandled key down: %d", key));
    return eRet_NotSupported; /* key not consumed */
    return eRet_Ok; /* key consumed */
}

void CScreenMain::show(bool bShow)
{
    /* show/hide menu and channel number together */
    showMenu(NULL);
    _pMainMenu->show(bShow);
    _pLabelChannelNum->show(bShow);

    if (true == bShow)
    {
        _Back->setFocus();
    }

    /* we do NOT want to hide this entire screen since various elements of it may be
       displayed at any time (channel number, buffer/tsm popup, etc) */
    /* CScreen::show(bShow); */
}

bool CScreenMain::isVisible()
{
    return _pMainMenu->isVisible()     ||
           _pDecodeMenu->isVisible()   ||
           _pDisplayMenu->isVisible()  ||
           _pDisplayMenu->isVisible()  ||
           _pAudioMenu->isVisible()    ||
           _pPlaybackMenu->isVisible() ||
#if NEXUS_HAS_FRONTEND
           _pScanQamMenu->isVisible()  ||
           _pScanVsbMenu->isVisible()  ||
           _pScanSatMenu->isVisible()  ||
           _pScanOfdmMenu->isVisible() ||
           _pTunerMenu->isVisible()    ||
#endif
           _pRecordMenu->isVisible();
}


void CScreenMain::showMenu(CWidgetMenu * pMenu)
{
    /* hide all other menus */
    _pMainMenu->show(false);
    _pPlaybackMenu->show(false);
    _pRecordMenu->show(false);
    _pDisplayMenu->show(false);
    _pDecodeMenu->show(false);
    _pAudioMenu->show(false);
#if NEXUS_HAS_FRONTEND
    _pScanQamMenu->show(false);
    _pScanVsbMenu->show(false);
    _pScanSatMenu->show(false);
    _pScanOfdmMenu->show(false);
    _pTunerMenu->show(false);
#endif

    if (NULL != pMenu)
    {
        pMenu->show(true);
    }
}
