/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: control.cpp $
* $brcm_Revision: 77 $
* $brcm_Date: 11/15/12 10:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/control.cpp $
* 
* 77   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 76   10/31/12 2:05p jrubio
* SW7346-1071: fix crash when changing audio or video settings, playback
* must be paused not stopped
* 
* 75   10/30/12 3:39p jrubio
* SW7346-1071: check pcrPid vs videoPid
* 
* 74   10/25/12 3:02p tokushig
* SW7231-749: fix channel numbering problem during auto scan
* 
* 73   10/19/12 5:43p tokushig
* SW7231-749: ensure tuners are properly checked in/out during tune/scan.
* 
* 72   10/19/12 1:02p jrubio
* SW7346-1071: remove duplicate Playback Start function, do cleanup after
* playback failure
* 
* 71   10/19/12 12:24p tokushig
* SW7231-749: force msg debug level when executing lua command to print
* the channel list
* 
* 70   10/19/12 10:47a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 70   10/19/12 10:46a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 70   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 69   10/18/12 4:10p jrubio
* SW7346-1071: fix VIDEO_PATHS
* 
* 68   10/12/12 4:37p jrubio
* SW7346-1004: fix channel scan hangup
* 
* 67   10/12/12 1:00p jrubio
* SW7346-1004: added custom LUA ONLY trickmodes
* 
* 66   10/12/12 12:14a tokushig
* SW7231-749: restrict tuning while playback is active
* 
* 65   10/11/12 11:31p jrubio
* SW7346-1004: redo recordStart to accept a channel handle
* 
* 64   10/11/12 11:24p tokushig
* SW7231-749: fix issue where a tuner reference is inadvertantly copied
* when a pid-less channel is scanned for psi info on the fly.  newly
* found programs should not share the tuner.
* 
* 63   10/11/12 9:37p jrubio
* SW7346-1004: add multiple records
* 
* 62   10/11/12 7:51p jrubio
* SW7346-1004: remove timeline after stop
* 
* 61   10/11/12 5:03p jrubio
* SW7346-1004: reset timeline when switching playbacks
* 
* 60   10/11/12 4:02p tokushig
* SW7231-749: add assert for current channel
* 
* 59   10/11/12 3:22p tokushig
* SW7231-749: add member variable initializations
* 
* 58   10/11/12 2:35p jrubio
* SW7346-1004: rename record/playback functions
* 
* 57   10/10/12 10:00p jrubio
* SW7346-1004: fix bug with Playback
* 
* 56   10/9/12 1:20p tokushig
* SW7231-749: remove dead code and change debug printout level
* 
* 55   10/8/12 6:47p tokushig
* SW7231-749: add lua commands for display settings
* 
* 54   10/8/12 4:15p jrubio
* SW7346-1004: create nfo with playback pids
* 
* 53   10/7/12 2:58p tokushig
* SW7231-749: add format change support and fixes for audio settings.
* 
* 53   10/7/12 2:57p tokushig
* SW7231-749: add format change support and fixes for audio settings.
* 
* 52   10/3/12 5:13p jrubio
* SW7346-1004: add new create info function
* 
* 51   9/29/12 9:24p tokushig
* SW7231-749: connect audio settings window panel to simple audio
* decoder.  pid, spdif, hdmi, downmix, dualmono, dolby drc, and dialog
* norm are implemented.
* 
* 50   9/26/12 4:51p tokushig
* SW7231-749: add ability to set hdmi/spdif output format to controller
* 
* 50   9/26/12 4:49p tokushig
* SW7231-749: add ability to set hdmi/spdif output format to controller
* 
* 49   9/25/12 12:45p jrubio
* SW7346-1004: record needs no arguments
* 
* 48   9/21/12 8:40p tokushig
* SW7231-749: scan->scanStart notification, get mode from model, add
* setAudioProgram notification command
* 
* 47   9/18/12 5:43p jrubio
* SW7346-1004: add use of video object
* 
* 46   9/17/12 10:25a jrubio
* SW7346-1004: modify record functions only filename/path are needed
* 
* 45   9/10/12 3:45p tokushig
* SW7231-749: use simple audio/video decoder instead of regular
* audio/video decoder
* 
* 45   9/10/12 3:43p tokushig
* SW7231-749: use simple audio/video decoder instead of regular
* audio/video decoder
* 
* 44   8/29/12 10:18a jrubio
* SW7346-1004: add record capabilities
* 
* 43   8/27/12 10:14a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 43   8/27/12 10:13a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 42   7/27/12 10:45a jrubio
* SW7346-848: cleanup debug
* 
* 41   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 40   6/15/12 4:51p tokushig
* SW7231-749: update scan to handle error where intended tuner is
* unavailable.  also better handle display of progress meter so 100% is
* displayed before meter is hidden
* 
* 39   5/21/12 5:37p jrubio
* SW7346-644: pass video object
* 
* 38   5/10/12 3:40p tokushig
* SW7231-749: add support for playback notifications.
* 
* 36   4/20/12 2:55p jrubio
* SW7346-644: remove unused eNotify vars
* 
* 35   4/20/12 2:20p jrubio
* SW7346-644: use GET_STR to get default VIDEOS_PATH
* 
* 34   4/20/12 1:26p jrubio
* SW7346-644: add rewind and more .info file information
* 
* 33   4/19/12 11:11p jrubio
* SW7346-644: add more playback functions and handling of pvr key presses
* 
* 32   4/19/12 7:31p tokushig
* SW7231-749: restrict conflicting actions during scan
* 
* 32   4/19/12 7:29p tokushig
* SW7231-749: restrict conflicting actions during scan
* 
* 31   4/18/12 5:14p jrubio
* SW7346-644: modify playback start, add key presses
* 
* 30   4/18/12 12:46a jrubio
* SW7346-644: add refresh from disk function, refresh
* notification,CPlaybackList class, resave changes from version 27
* 
* 29   4/17/12 6:45p tokushig
* SW7231-749: add exit key handling - closes main menu
* 
* 26   4/15/12 9:50p tokushig
* SW7231-749: modify control handing of video source changed notification
* 
* 26   4/15/12 9:41p tokushig
* SW7231-749: added audio source changed callback support
* 
* 25   4/15/12 12:11a jrubio
* SW7346-644: add logic for playback stop
* 
* 24   4/13/12 11:42a jrubio
* SW7346-644: add stc/pid/playback start logic
* 
* 23   4/12/12 12:11a jrubio
* SW7346-644: add more logic for checking out playback resource
* 
* 22   4/11/12 1:20p jrubio
* SW7346-644: add Playback and reworked CPidMgr, Lua Command invokes
* Playback Start
* 
* 21   4/9/12 7:29p tokushig
* SW7405-5581: added video source changed callback support
* 
* 20   4/6/12 12:43p tokushig
* SW7231-749: fixed callback handling issue in register/notification
* mechanism which prevented control from manually notifying views.  view
* and controller classes now hide the callback mechanism from the mvc
* based classes for notifications and there is a consistent
* processNotification() method for both views and controllers (models do
* not receive notifications.
* 
* 19   3/30/12 9:30p tokushig
* SW7231-749: printout scanned channel list before attempting to tune to
* first channel in list
* 
* 18   3/27/12 3:30p tokushig
* SW7405-5581: synchronize lua commands
* 
* 17   3/19/12 5:27p tokushig
* SW7405-5581: add config entries to enable channel scan to save
* encrypted channels, audio only channels, and duplicate channels
* 
* 15   3/12/12 8:52p tokushig
* SW7405-5581: add brutus.channelListDump() lua command
* 
* 14   3/12/12 4:42p tokushig
* SW7405-5581: after channel load, unnumbered channels will be auto-
* numbered starting with the last valid channel number in the channel
* list.  channel numbers can not be saved/restored in channel list
* save/load.  moved code to load channels from xml to the appropriate
* channel classes.
* 
* 13   3/9/12 3:46p tokushig
* SW7405-5581: integrate lua channel list save command with channelmgr
* code.
* 
* 12   3/9/12 2:55p tokushig
* SW7405-5581: added support for channel list save.  also added initial
* code to handle tuner lock/unlock after first tune.
* 
* 11   3/2/12 4:54p tokushig
* SW7405-5581: brutus.channelListLoad() lua command working.
* 
* 10   3/2/12 3:53p tokushig
* SW7405-5581: propagate key presses to active window and its parents if
* necessary.  allow controller last chance to act on key presses ignored
* by screens/bwidgets.  move some callback code to cscreen to hide from
* individual screen classes.
* 
* 9   3/1/12 3:56p tokushig
* SW7405-5581: moved qam scan into separate thread to enable screen
* updates while scan is running.  added on screen scan progress to test.
* NOTE: this does not yet have protection against conflicts so trying to
* channel up/down while scan is running will result in a crash
* 
* 7   2/28/12 3:01p tokushig
* SW7405-5581: add timer support and use it for deferred tuning using ch
* up/down.  fast pressing of ch up/down remote buttons will update the
* ui but not tune until button presses stop for 500msecs.  this allows
* the user to skip channels while traversing the channel list using the
* remote
* 
* 
*********************************************************************************/

#include "control.h"
#include "notification.h"
#include "brutus.h"
#include "channelmgr.h"
#include "channel_qam.h"
#include "playback.h"
#include "record.h"
#include "screen.h"
#include "screen_main.h"

#include "nexus_video_decoder.h"

BDBG_MODULE(brutus_control);

CControl::CControl() :
    _pModel(NULL),
    _pModelConfig(NULL),
    _pModelChannelMgr(NULL),
    _pCfg(NULL),
    _widgetEngine(NULL),
    _deferredChannelTimer(this),
    _tunerLockCheckTimer(this),
    _viewList(false),
    _recordingChannels(false),
    _pPlayback(NULL)
{
    _viewList.clear();
    _recordingChannels.clear();
    /* initialize notification filter */
}

CControl::~CControl()
{
    uninitialize();
}

eRet CControl::initialize(CModelConfig * pModelConfig, CModelChannelMgr * pModelChannelMgr, bwidget_engine_t widgetEngine)
{
    eRet ret = eRet_Ok;

    BDBG_ASSERT(NULL != pModelConfig);
    BDBG_ASSERT(NULL != pModelChannelMgr);
    BDBG_ASSERT(NULL != widgetEngine);

    _pModelConfig     = pModelConfig;
    _pModelChannelMgr = pModelChannelMgr;
    _pCfg             = pModelConfig->getCfg();
    _widgetEngine     = widgetEngine;

    _deferredChannelTimer.setWidgetEngine(_widgetEngine);
    _deferredChannelTimer.setTimeout(GET_INT(_pCfg, DEFERRED_CHANNEL_CHANGE_TIMEOUT));

    _tunerLockCheckTimer.setWidgetEngine(_widgetEngine);
    _tunerLockCheckTimer.setTimeout(GET_INT(_pCfg, TUNER_LOCK_CHECK_TIMEOUT));

    return ret;
}

eRet CControl::uninitialize()
{
    eRet ret = eRet_Ok;

    _pModelConfig     = NULL;
    _pModelChannelMgr = NULL;
    _pCfg             = NULL;
    _widgetEngine     = NULL;

    _deferredChannelTimer.setWidgetEngine(_widgetEngine);
    _tunerLockCheckTimer.setWidgetEngine(_widgetEngine);

    return ret;
}

void CControl::addView(CView * pView, const char * name)
{
    BDBG_ASSERT(NULL != pView);
    BDBG_ASSERT(NULL != name);

    CViewListNode * pNode = new CViewListNode(pView, name);
    BDBG_ASSERT(NULL != pNode);
    _viewList.add(pNode);
}

CView * CControl::findView(const char * name)
{
    MListItr <CViewListNode>   itr(&_viewList);
    CViewListNode            * pNode            = NULL;
    CView                    * pView            = NULL;
    BDBG_ASSERT(name);

    for (pNode = itr.first(); NULL != pNode; pNode = itr.next())
    {
        if (pNode->_strName == name)
        {
            /* found it */
            pView = pNode->_pView;
            break;
        }
    }

    return pView;
}

/* at this point, all widgets/windows and bwidgets have decided to ignore this key press
   so we get one last chance to respond to them */
eRet CControl::processKeyEvent(CRemoteEvent * pRemoteEvent)
{
    eRet      ret            = eRet_Ok;
    bwidget_t widget;

    BDBG_ASSERT(NULL != pRemoteEvent);
    BDBG_ASSERT(NULL != _widgetEngine);

    switch(pRemoteEvent->getCode())
    {
    case eKey_ChannelUp:
        channelUp();
        break;

    case eKey_ChannelDown:
        channelDown();
        break;

    case eKey_Menu:
    {
        CScreenMain * pMainScreen = (CScreenMain *)findView("mainScreen");
        BDBG_ASSERT(NULL != pMainScreen);
        pMainScreen->show(pMainScreen->isVisible() ? false : true);
    }
        break;

    case eKey_Info:
        break;

    case eKey_Exit:
    {
        CScreenMain * pMainScreen = (CScreenMain *)findView("mainScreen");
        BDBG_ASSERT(NULL != pMainScreen);
        pMainScreen->show(false);
    }
        break;

    case eKey_Stop:
    {
        if(_pPlayback && _pPlayback->isActive())
            playbackStop(MString());
        else 
            recordStop();
    }
        break;

    case eKey_FastForward:
    {
        if(_pPlayback && _pPlayback->isActive())
            _pPlayback->trickMode(true);
    }
        break;
    case eKey_Rewind:
    {
        if(_pPlayback && _pPlayback->isActive())
            _pPlayback->trickMode(false);
    }
        break;
    case eKey_Play:
    {
        if(_pPlayback)
            _pPlayback->start(MString(),MString(),MString());
    }
        break;
    case eKey_Pause:
    {
        if(_pPlayback && _pPlayback->isActive())
            _pPlayback->pause();
    }
        break;
    case eKey_Record:
    {
           recordStart(MString(),MString());
    }
        break;

    default:
        break;
    }

error:
    return ret;
}


void CControl::onIdle()
{
    if (true == GET_BOOL(_pCfg, FIRST_TUNE))
    {
        SET(_pCfg, FIRST_TUNE, false);
        tuneChannel();
    }

    if (true == GET_BOOL(_pCfg, EXIT_APPLICATION))
    {
        bwidget_terminate(_widgetEngine);
    }
}

/* check validity of notification based on mode.  return true if given notification
   was valid.  returns false otherwise.  note that this method may make changes
   to ensure the given notification is valid.  for example if we are in live mode
   and tuned to a channel when a eNotify_PlaybackStart notification arrives,
   this method will unTune and return true.  
   */
bool CControl::validateNotification(CNotification & notification, eMode mode)
{
    bool bValid = false;

    switch (mode)
    {
    case eMode_Live:
        bValid = true;
        break;

    case eMode_Scan:
        switch (notification.getId())
        {
        case eNotify_KeyUp:
        case eNotify_KeyDown:
        {
            CRemoteEvent * pRemoteEvent =  (CRemoteEvent *)notification.getData();
            switch(pRemoteEvent->getCode())
            {
            case eKey_ChannelUp:
            case eKey_ChannelDown:
                /* no tuning during channel scan */
                break;
            default:
                bValid = true;
                break;
            }
        }
            break;
        case eNotify_ScanStop:
        case eNotify_ScanStopped:
            bValid = true;
            break;
        default:
        case eNotify_RecordStart:
        case eNotify_RecordStop:
            /* no tuning, playback, or channel list changes
               during channel scan */
            break;
        }
        break;
    case eMode_Playback:
        /* JOSE TODO: add restricted operations during playback */
        bValid = true;
        break;
    case eMode_Record:
        break;
    default:
        bValid = true;

        break;
    }

    return bValid;
}

void CControl::processNotification(CNotification & notification)
{
    eRet ret = eRet_Ok;
    
    BDBG_MSG(("--- CControl Notification Received: %d ---", notification.getId()));

    if (false == validateNotification(notification, _pModel->getMode()))
    {
        /* notification consumed (invalid) so return */
        goto done;
    }

    /* handle key event notification */
    switch (notification.getId())
    {
    case eNotify_KeyUp:
    case eNotify_KeyDown:
        ret = processKeyEvent((CRemoteEvent *)notification.getData());
        break;

    case eNotify_ChUp:
        channelUp();
        break;

    case eNotify_ChDown:
        channelDown();
        break;

    case eNotify_RefreshPlaybackList:
        {
            updatePlaybackList();
        }
        break;

#if NEXUS_HAS_FRONTEND
    case eNotify_ScanStart:
        {
            CTunerScanData * pScanData = (CTunerScanData *)notification.getData();
            scanTuner(pScanData);
        }
        break;
#endif

    case eNotify_ChannelListLoad:
        {
            CChannelMgrLoadSaveData * pLoadData = (CChannelMgrLoadSaveData *)notification.getData();
            int totalChannels = 0;
            BDBG_WRN(("Load channel list command (%s/%s) append:%d", pLoadData->_strFileName.s(), pLoadData->_strListName.s(), pLoadData->_append));

            /* Check the list of recorded channels and stop them */
            totalChannels = _recordingChannels.total();
            while(totalChannels)
            {
                recordStop();
                totalChannels--;
            }
            unTuneChannel();
            _pModelChannelMgr->loadChannelList(pLoadData->_strFileName.s(), pLoadData->_append);
            _pModelChannelMgr->dumpChannelList(true);
            tuneChannel();
        }
        break;

    case eNotify_ChannelListSave:
        {
            CChannelMgrLoadSaveData * pSaveData = (CChannelMgrLoadSaveData *)notification.getData();
            BDBG_WRN(("Save channel list command (%s/%s)", pSaveData->_strFileName.s(), pSaveData->_strListName.s()));

            _pModelChannelMgr->saveChannelList(pSaveData->_strFileName.s(), pSaveData->_append);
        }
        break;

    case eNotify_ChannelListDump:
        {
            _pModelChannelMgr->dumpChannelList(true);
        }
        break;

    case eNotify_SetAudioProgram:
        {
            uint16_t * pPid = (uint16_t *)notification.getData();
            CHECK_PTR_GOTO("pid not specified in setAudioProgram notification", pPid, ret, eRet_InvalidParameter, error);

            setAudioProgram(*pPid);
        }
        break;

    case eNotify_SetSpdifInput:
        {
            eSpdifInput * pSpdifInput = (eSpdifInput *)notification.getData();
            CHECK_PTR_GOTO("Spdif input not specified in setSpdifInput notification", pSpdifInput, ret, eRet_InvalidParameter, error);

            setSpdifInput(*pSpdifInput);
        }
        break;

    case eNotify_SetHdmiAudioInput:
        {
            eHdmiAudioInput * pHdmiInput = (eHdmiAudioInput *)notification.getData();
            CHECK_PTR_GOTO("Hdmi input not specified in setHdmiAudioInput notification", pHdmiInput, ret, eRet_InvalidParameter, error);

            setHdmiAudioInput(*pHdmiInput);
        }
        break;

    case eNotify_SetAudioDownmix:
        {
            eAudioDownmix * pDownmix = (eAudioDownmix *)notification.getData();
            CHECK_PTR_GOTO("Audio downmix not specified in setAudioDownmix notification", pDownmix, ret, eRet_InvalidParameter, error);

            setAudioDownmix(*pDownmix);
        }
        break;

    case eNotify_SetAudioDualMono:
        {
            eAudioDualMono * pDualMono = (eAudioDualMono *)notification.getData();
            CHECK_PTR_GOTO("Audio DualMono not specified in setAudioDualMono notification", pDualMono, ret, eRet_InvalidParameter, error);

            setAudioDualMono(*pDualMono);
        }
        break;

    case eNotify_SetDolbyDRC:
        {
            eDolbyDRC * pDolbyDRC = (eDolbyDRC *)notification.getData();
            CHECK_PTR_GOTO("Dolby DRC not specified in setDolbyDRC notification", pDolbyDRC, ret, eRet_InvalidParameter, error);

            setDolbyDRC(*pDolbyDRC);
        }
        break;

    case eNotify_SetDolbyDialogNorm:
        {
            bool * pDolbyDialogNorm = (bool *)notification.getData();
            CHECK_PTR_GOTO("Dolby Dialog Normalization not specified in setDolbyDialogNorm notification", pDolbyDialogNorm, ret, eRet_InvalidParameter, error);

            setDolbyDialogNorm(*pDolbyDialogNorm);
        }
        break;

    case eNotify_SetColorSpace:
        {
            NEXUS_ColorSpace * pColorSpace = (NEXUS_ColorSpace *)notification.getData();
            CHECK_PTR_GOTO("Color space not specified in setColorSpace notification", pColorSpace, ret, eRet_InvalidParameter, error);

            setColorSpace(*pColorSpace);
        }
        break;

    case eNotify_SetMpaaDecimation:
        {
            bool * pMpaaDecimation = (bool *)notification.getData();
            CHECK_PTR_GOTO("MPAA decimation not specified in setMpaaDecimation notification", pMpaaDecimation, ret, eRet_InvalidParameter, error);

            setMpaaDecimation(*pMpaaDecimation);
        }
        break;

    case eNotify_SetDeinterlacer:
        {
            bool * pDeinterlacer = (bool *)notification.getData();
            CHECK_PTR_GOTO("Deinterlace setting not specified in setDeinterlacer notification", pDeinterlacer, ret, eRet_InvalidParameter, error);

            setDeinterlacer(*pDeinterlacer);
        }
        break;

    case eNotify_SetBoxDetect:
        {
            bool * pBoxDetect = (bool *)notification.getData();
            CHECK_PTR_GOTO("BoxDetect setting not specified in setBoxDetect notification", pBoxDetect, ret, eRet_InvalidParameter, error);

            setBoxDetect(*pBoxDetect);
        }
        break;

    case eNotify_SetAspectRatio:
        {
            NEXUS_DisplayAspectRatio * pAspectRatio = (NEXUS_DisplayAspectRatio *)notification.getData();
            CHECK_PTR_GOTO("AspectRatio setting not specified in setAspectRatio notification", pAspectRatio, ret, eRet_InvalidParameter, error);

            setAspectRatio(*pAspectRatio);
        }
        break;

    case eNotify_SetVideoFormat:
        {
            NEXUS_VideoFormat * pVideoFormat = (NEXUS_VideoFormat *)notification.getData();
            CHECK_PTR_GOTO("Video format not specified in setVideoFormat notification", pVideoFormat, ret, eRet_InvalidParameter, error);

            setVideoFormat(*pVideoFormat);
        }
        break;

    case eNotify_SetAutoVideoFormat:
        {
            bool * pAutoVideoFormat = (bool *)notification.getData();
            CHECK_PTR_GOTO("Video format not specified in setAutoVideoFormat notification", pAutoVideoFormat, ret, eRet_InvalidParameter, error);

            setAutoVideoFormat(*pAutoVideoFormat);
        }
        break;

    case eNotify_PlaybackStart:
        {
            CPlaybackData * pSaveData = (CPlaybackData *)notification.getData();
            BDBG_MSG(("Playback File (%s/%s) Video Path (%s), Video 0x%p", pSaveData->_strFileName.s(), pSaveData->_strIndexName.s(),pSaveData->_strPath.s(),pSaveData->_video ));

            if(pSaveData->_video)
                playbackStart(pSaveData->_video->_mediaName,pSaveData->_strIndexName, pSaveData->_strPath);
            else
                playbackStart(pSaveData->_strFileName, pSaveData->_strIndexName, pSaveData->_strPath);
        }
        break;

    case eNotify_PlaybackTrickMode:
        {
            CPlaybackTrickModeData * pSaveData = (CPlaybackTrickModeData *)notification.getData();
            
            if(pSaveData && _pPlayback && _pPlayback->isActive())
                _pPlayback->trickMode(pSaveData);
            
        }
        break;

    case eNotify_PlaybackStop:
        {
            CPlaybackData * pSaveData = (CPlaybackData *)notification.getData();
            BDBG_WRN(("Stop Playback File (%s) ", pSaveData->_strFileName.s()));
            if(_pPlayback && _pPlayback->isActive())
                playbackStop(pSaveData->_strFileName);
        }
        break;

    case eNotify_RecordStart:
        {
            CRecordData * pSaveData = (CRecordData *)notification.getData();
            BDBG_WRN(("Record File (%s) Video Path (%s)", pSaveData->_strFileName.s(),pSaveData->_strPath.s()));

            recordStart(pSaveData->_strFileName, pSaveData->_strPath);
        }
        break;

    case eNotify_RecordStop:
        {
            CRecordData * pSaveData = (CRecordData *)notification.getData();

            BDBG_WRN(("Stop Record of File (%s) ", pSaveData->_strFileName.s()));
            recordStop();
        }
        break;

#if NEXUS_HAS_FRONTEND
    case eNotify_ScanStopped:
        {
            CTunerScanNotificationData * pScanData       = (CTunerScanNotificationData *)notification.getData();
            CBoardResources            * pBoardResources =  _pModelConfig->getBoardResources();
            
            /* the scan is done - check tuner back in */
            BDBG_ASSERT(NULL != pScanData->getTuner());
            pBoardResources->checkinResource(pScanData->getTuner());

            _pModel->setMode(eMode_Invalid);

            if (false == pScanData->getAppendToChannelList())
            {
                /* channel list was replaced so set a new current channel */
                _pModelChannelMgr->setCurrentChannel();
            }

            _pModelChannelMgr->dumpChannelList(true);
            tuneChannel();
        }
        break;
#endif

    case eNotify_VideoSourceChanged:
        {
            CSimpleVideoDecode       * pVideoDecode = (CSimpleVideoDecode *)notification.getData();
            CDisplay                 * pDisplay     = (CDisplay *)_pModel->getDisplay();
            NEXUS_Error nerror = NEXUS_SUCCESS;
            NEXUS_VideoDecoderStatus status;

            BDBG_ASSERT(NULL != pVideoDecode);
            BDBG_ASSERT(NULL != pDisplay);

            nerror = pVideoDecode->getStatus(&status);
            if (eRet_Ok == CHECK_NEXUS_ERROR("Video source changed notification received but unable to get video decoder status", nerror))
            {
                if (true == pDisplay->isAutoFormat())
                {
                    MList <CVideoOutput>    * pListOutputs       = pDisplay->getOutputList();
                    CVideoOutput            * pOutput            = NULL;
                    MListItr <CVideoOutput>   itr(pListOutputs);
                    NEXUS_VideoFormat         formatOptimal;

                    /* check video outputs for optimal video format */
                    for (pOutput = pListOutputs->first(); pOutput; pOutput = pListOutputs->next())
                    {
                        if (eRet_Ok == pVideoDecode->getOptimalVideoFormat(pOutput, &formatOptimal))
                        {
                            setVideoFormat(formatOptimal);
                            break;
                        }
                    }

                    if (NULL == pOutput)
                    {
                        BDBG_WRN(("unable to find valid video format!"));
                    }
                }
            }
        }
        break;

    case eNotify_Exit:
        SET(_pCfg, EXIT_APPLICATION, "true");
        break;

    case eNotify_Timeout:
        {
            CTimer * pTimer = (CTimer *)notification.getData();

            if (pTimer == &_deferredChannelTimer)
            {
                ret = tuneChannel(_pModelChannelMgr->getDeferredChannel()); 
                _pModelChannelMgr->setDeferredChannel(NULL);
                CHECK_ERROR_GOTO("Unable to tune", ret, error);
            }
            else
            if (pTimer == &_tunerLockCheckTimer)
            {
                BDBG_WRN(("tuner lock check timer fired"));
            }
        }
        break;

    default:
        BDBG_MSG(("*** Received unhandled notification ***"));
        break;
    }

done:
error:
    return;
}


/* the given channel is copied and added to the channel list if it passes filters */
eRet CControl::addChannelToChList(CModelChannel * pChannel)
{
    eRet            ret       = eRet_Ok;
    uint32_t        retFilter = CHMGR_FILTER_PASS;
    CModelChannel * pChNew    = NULL; 

    retFilter = _pModelChannelMgr->filterChannel(pChannel);
    if (CHMGR_FILTER_PASS == retFilter)
    {
        /* given channel passed filter based on content */

        /* each program will be represented by it's own channel object */
        /* use copy constructor */
        pChNew = pChannel->createCopy(pChannel);
        CHECK_PTR_GOTO("unable to create new channel for adding to channel list", pChNew, ret, eRet_OutOfMemory, error);

        _pModelChannelMgr->addChannel(pChNew);
    }
    
    ret = (CHMGR_FILTER_PASS == retFilter ? eRet_Ok : eRet_InvalidParameter);

error:
    return ret;
}


/* returns 1 if channel was added to channel list, 0 otherwise */
static bool addChannelCallback(CModelChannel * pNewChannel, void * context)
{
    CControl * pControl = (CControl *)context;
    BDBG_ASSERT(NULL != pControl);

    return (eRet_Ok == pControl->addChannelToChList(pNewChannel));
}

eRet CControl::unTuneChannel(CModelChannel * pChannel)
{
    eRet            ret             = eRet_Ok;
    CPid          * pVideoPid       = NULL;
    CPid          * pAudioPid       = NULL;
    CPid          * pPcrPid         = NULL;
    CRecord       * pRecord         = NULL;
    CStc          * pStc            = _pModel->getStc();
    CSimpleVideoDecode  * pVideoDecode    = _pModel->getSimpleVideoDecode();
    CSimpleAudioDecode  * pAudioDecode    = _pModel->getSimpleAudioDecode();

    if (NULL == pChannel )
    {
        pChannel = _pModelChannelMgr->getCurrentChannel();
        CHECK_PTR_GOTO("current channel is NULL, ignoring untune request", pChannel, ret, eRet_NotAvailable, error);
    } 
    
    if (!pChannel->isTuned())
    {
        BDBG_WRN((" Already untuned channel"));
        ret = eRet_InvalidState;
        goto error;
    } 
    
    if ( pChannel == _pModelChannelMgr->getCurrentChannel())
    {
       pAudioDecode->stop();
       pVideoDecode->stop();
    }

    pPcrPid   = pChannel->getPid(0, ePidType_Pcr);
    pAudioPid = pChannel->getPid(0, ePidType_Audio);
    pVideoPid = pChannel->getPid(0, ePidType_Video);

    /* Check to see if PCR pid is the same as Video Pid */
    if (pPcrPid == pVideoPid)
    {
       pPcrPid= NULL;
    }

    if (NULL != pPcrPid)   { pPcrPid->close();   }
    if (NULL != pAudioPid) { pAudioPid->close(); }
    if (NULL != pVideoPid) { pVideoPid->close(); }

    if (!pChannel->isRecording())
    {
       BDBG_MSG(("Channel is not recording. Completely untune this Channel/Tuner"));
       ret = pChannel->unTune(_pModelConfig); 
       CHECK_ERROR_GOTO("unable to unTune!", ret, error);
    }

    
error:
    return ret;
}

eRet CControl::tuneChannel(CModelChannel * pChannel)
{
    eRet ret = eRet_Ok;

    CModelChannel * pCurrentChannel = _pModelChannelMgr->getCurrentChannel();
    CPid          * pVideoPid       = NULL;
    CPid          * pAudioPid       = NULL;
    CPid          * pPcrPid         = NULL;
    CStc          * pStc            = _pModel->getStc();
    CSimpleVideoDecode  * pVideoDecode    = _pModel->getSimpleVideoDecode();
    CSimpleAudioDecode  * pAudioDecode    = _pModel->getSimpleAudioDecode();

    BDBG_ASSERT(NULL != _pModel);

    if ((NULL != _pPlayback) && (true == _pPlayback->isActive()))
    {
        return ret;
    }

    if (NULL == pChannel)
    {
        /* forced retune to current channel */
        pChannel = pCurrentChannel;
        CHECK_PTR_GOTO("Channel list empty - Unable to tune!", pChannel, ret, eRet_NotAvailable, error);

        if (pChannel->isTuned())
        {
            unTuneChannel(pChannel);
        }
    }

    if ((NULL != pCurrentChannel) && (true == pCurrentChannel->isTuned()))
    {
        /* we are currently tuned */
        if (pChannel == pCurrentChannel) 
        {
            /* already tuned to caller specified channel */
            goto done;
        }
        else 
        {
            /* tuned to wrong channel */
            unTuneChannel(pCurrentChannel);
        }
    }

    if (!pChannel->isRecording())
    {
       ret = pChannel->tune(this, _pModelConfig, true);
       CHECK_ERROR_GOTO("tuning failed", ret, error);
    }

    {
        CParserBand * pParserBand = pChannel->getParserBand();

        pVideoPid = pChannel->getPid(0, ePidType_Video);
        pAudioPid = pChannel->getPid(0, ePidType_Audio);
        pPcrPid   = pChannel->getPid(0, ePidType_Pcr);

        if ((NULL == pVideoPid) && (NULL == pAudioPid))
        {
            CHANNEL_INFO_T   chanInfo;
            int              minor      = 1;

            /* no pids - let's look for some */

            /* get PSI channel info for current tuned channel */
            ret = pChannel->getChannelInfo(&chanInfo, true);
            CHECK_ERROR_GOTO("PSI channel info retrieval failed, skipping to next channel to be scanned", ret, error);

            /*pChannel->unTune(_pModelConfig);*/

            if (0 < chanInfo.num_programs)
            {
                CModelChannel * pChNew = NULL;
                pChNew = pChannel->createCopy(pChannel);
                CHECK_PTR_GOTO("error copying channel object", pChNew, ret, eRet_OutOfMemory, error);

                /* update missing pid info in current channel */
                pChannel->initialize(&chanInfo.program_info[0]);
                pChannel->setMinor(minor);
                minor++;

                /* try to add additional found programs if valid */
                for (int i = 1; i < chanInfo.num_programs; i++) 
                {
                    /* since all these channels share a common frequency, we'll assign minor
                       channel numbers here (minor numbers start at 1) */
                    pChNew->initialize(&chanInfo.program_info[i]);
                    pChNew->setMinor(minor);

                    /* try adding channel to channel list */
                    if (eRet_Ok == addChannelToChList(pChNew))
                    {
                        minor++;
                    }
                }

                /* newly added channels are probably out of order with respect to the
                   other existing channels, so we'll sort */
                _pModelChannelMgr->sortChannelList();

                delete pChNew;
            }

            /* get newly found pids */
            pVideoPid = pChannel->getPid(0, ePidType_Video);
            pAudioPid = pChannel->getPid(0, ePidType_Audio);
            pPcrPid   = pChannel->getPid(0, ePidType_Pcr);
        }

        /* open pids */
        if (pVideoPid)
        {
            CHECK_PTR_GOTO("missing parser band - tune failed", pParserBand, ret, eRet_InvalidState, error);
            ret = pVideoPid->open(pParserBand);
            CHECK_ERROR_GOTO("open video pid channel failed", ret, error);
        }

        if (pAudioPid)
        {
            CHECK_PTR_GOTO("missing parser band - tune failed", pParserBand, ret, eRet_InvalidState, error);
            ret = pAudioPid->open(pParserBand);
            CHECK_ERROR_GOTO("open audio pid channel failed", ret, error);
        }

        if (pPcrPid)
        {
            /* only open pcr pid channel if it is different from audio/video pids */
            if (false == pPcrPid->isOpen())
            {
                CHECK_PTR_GOTO("missing parser band - tune failed", pParserBand, ret, eRet_InvalidState, error);
                ret = pPcrPid->open(pParserBand);
                CHECK_ERROR_GOTO("open Pcr pid channel failed", ret, error);
            }
        }
    }

    /* dtt - todo configure timebase */

    pStc->setStcType(eStcType_ParserBand);
    ret = pStc->configure(pPcrPid);
    CHECK_ERROR_GOTO("error configuring stc channel", ret, error);

    if (NULL != pVideoPid)
    {
        ret = pVideoDecode->start(pVideoPid);
        CHECK_ERROR_GOTO("unable to start video decode", ret, error);
    }

    if (NULL != pAudioPid)
    {
        ret = pAudioDecode->start(pAudioPid);
        CHECK_ERROR_GOTO("unable to start audio decode", ret, error);
    }

done:
error:
    _pModel->setMode(eMode_Live);
    _pModelChannelMgr->setCurrentChannel(pChannel);
    return ret;
}

eRet CControl::recordStart(MString fileName, MString path)
{
    eRet ret = eRet_Ok;

    CModelChannel   * pCurrentChannel = _pModelChannelMgr->getCurrentChannel();
    CBoardResources * pBoardResources = NULL;
    CPid            * pVideoPid       = NULL;
    CPid            * pAudioPid       = NULL;
    CPid            * pPcrPid         = NULL;
    CStc            * pStc            = _pModel->getStc();
    CVideoDecode    * pVideoDecode    = _pModel->getVideoDecode();
    CAudioDecode    * pAudioDecode    = _pModel->getAudioDecode();
    CPlaybackList   * pPlaybackList   = _pModel->getPlaybackList();
    CParserBand     * pParserBand     = pCurrentChannel->getParserBand();
    CConfiguration  * pCfg            = NULL;
    CVideo          * video           = new CVideo(); /* Playback List needs to close all videos */
    CRecord         * pRecord         = NULL;
    MString           mediaName;
    MString           indexName;

    BDBG_ASSERT(NULL != _pModel);
    BDBG_ASSERT(NULL != _pModelConfig);
    BDBG_ASSERT(NULL != pPlaybackList);
    BDBG_ASSERT(NULL != pCurrentChannel);

    pBoardResources = _pModelConfig->getBoardResources();
    pCfg            = _pModelConfig->getCfg();
    if (false == pCurrentChannel->isTuned() & pCurrentChannel->isRecording())
                    {
       BDBG_WRN(("Must be tuned to a channel and Channel must not be recording already %s",pCurrentChannel->isRecording()?"true":"false"));
       ret = eRet_NotSupported;
       goto done;
    }

    pRecord = (CRecord *)pBoardResources->checkoutResource(this, eBoardResource_record);
    CHECK_PTR_GOTO("unable to checkout Playback ", pRecord, ret, eRet_NotAvailable, done);
    _pModel->addRecord(pRecord);
    
    
    pRecord->setBand(pParserBand);
    
    /* Check Path */
   if (!path)
      path = GET_STR(pCfg,VIDEOS_PATH);

   /* add the trailing forward Slash to the Path just in case it was not present */
   path += "/";

   video->_path= path;
   if (!fileName || fileName.isEmpty() == 0)
   {
      mediaName =pPlaybackList->getNextBrutusName();
      video->_mediaName = mediaName;
   } else
      video->_mediaName = fileName;

        
   /* indexName is the same as fileName */
   indexName = video->_mediaName;
   indexName.truncate(video->_mediaName.find(".",0, false));
   indexName= indexName+".nav"; 
   video->_indexName = indexName;
   /* Create info file and add to Playback list!!!*/
   video->_pidMgr = *(pCurrentChannel->getPidMgr());

   /* get newly found pids, set Playback flag*/
   pVideoPid = video->getPid(0, ePidType_Video);
   pVideoPid->setPlayback(true);
   pAudioPid = video->getPid(0, ePidType_Audio);
   pAudioPid->setPlayback(true);
   pPcrPid   = video->getPid(0, ePidType_Pcr);
   pPcrPid->setPlayback(true);
   pRecord->setVideo(video);
   
   /* open pids */
   if (pVideoPid)
   {
       CHECK_PTR_GOTO("missing parser band - tune failed", pParserBand, ret, eRet_InvalidState, error);
       ret = pVideoPid->open(pRecord);
       CHECK_ERROR_GOTO("open video pid channel failed", ret, error);
   }

   if (pAudioPid)
   {
       CHECK_PTR_GOTO("missing parser band - tune failed", pParserBand, ret, eRet_InvalidState, error);
       ret = pAudioPid->open(pRecord);
       CHECK_ERROR_GOTO("open audio pid channel failed", ret, error);
   }

   pCurrentChannel->setRecord(pRecord);
   _recordingChannels.add(pCurrentChannel);
   ret = pRecord->start();
   if (ret != eRet_Ok)
   {
      goto error;
   }
   pPlaybackList->addVideo(video);
   pPlaybackList->createInfo(video);
   pPlaybackList->sync();

   BDBG_MSG(("Added a Channel"));



    

done:
return ret;
error:
   if (pVideoPid)
   {
      pVideoPid->close(pRecord);
   }

   if (pAudioPid)
   {
      pAudioPid->close(pRecord);
   }
   DEL(video);
   _recordingChannels.remove(pCurrentChannel);
   pCurrentChannel->setRecord(NULL);
   pRecord->setBand(NULL);

   /* check in Record now */
    if (pRecord != NULL)
    {
        CBoardResources * pBoardResources = _pModelConfig->getBoardResources();

        ret = pBoardResources->checkinResource(pRecord);
        if (ret != eRet_Ok)
        {
           BDBG_ERR(("unable to checkin Record "));
        }
    }


return ret;
}

/* Right now we don't use fileName.
   Multiple Records will need fileName */
eRet CControl::recordStop(CModelChannel * pChannel)
{
    eRet ret = eRet_Ok;
    CModelChannel   * pCurrentChannel = _pModelChannelMgr->getCurrentChannel();
    MListItr<CModelChannel> itr(&_recordingChannels);
    CRecord * pRecord = NULL;

    /*_pRecord->printPids(); */
    
    if (pChannel != NULL)
    {
        if (pChannel->isRecording())
        {
             pRecord = pChannel->getRecord();
             ret = pRecord->stop();
             _recordingChannels.remove(pChannel);
             pChannel->setRecord(NULL);
             if (pChannel != pCurrentChannel)
             {
                 unTuneChannel(pChannel);
             }
             goto done;
          }else
          {
              BDBG_WRN(("cannot call record stop on a no recording Channel"));
              ret = eRet_ExternalError;
              goto error;
          }
        
    }

    if (pCurrentChannel->isRecording())
    {
       pRecord = pCurrentChannel->getRecord();
       ret = pRecord->stop();
       _recordingChannels.remove(pCurrentChannel);
       pCurrentChannel->setRecord(NULL);
       CHECK_ERROR_GOTO("Record error!", ret, error);
    }else
    {
       for (pChannel=itr.first();pChannel!=NULL; pChannel=itr.next())
       {
          if (pChannel->isRecording())
          {
             pRecord = pChannel->getRecord();
             ret = pRecord->stop();
             _recordingChannels.remove(pChannel);
             pChannel->setRecord(NULL);
             unTuneChannel(pChannel);
             break;
          }
       }
    }

done:
    /* check in Record now */
    if (pRecord != NULL)
    {
        CBoardResources * pBoardResources = _pModelConfig->getBoardResources();

        ret = pBoardResources->checkinResource(pRecord);
        CHECK_ERROR_GOTO("unable to checkin Record ", ret, error);
    }

    _pModel->removeRecord(pRecord);
    
error:
return ret;
}

eRet CControl::playbackStart(MString fileName, MString indexName, MString path)
{
    
    CModelChannel                * pCurrentChannel = _pModelChannelMgr->getCurrentChannel();
    CPid                         * pVideoPid       = NULL;
    CPid                         * pAudioPid       = NULL;
    CPid                         * pPcrPid         = NULL;
    CVideo                       * pVideo           = NULL;
    CStc                         * pStc            = _pModel->getStc();
    CSimpleVideoDecode           * pVideoDecode    = _pModel->getSimpleVideoDecode();
    CSimpleAudioDecode           * pAudioDecode    = _pModel->getSimpleAudioDecode();
    CPlaybackList                * pPlaybackList   = _pModel->getPlaybackList();
    NEXUS_Error                    nerror          = NEXUS_SUCCESS;
    CPlayback                    * pPlayback       = NULL;
    CBoardResources              * pBoardResources = NULL;
    CConfiguration               * pCfg            = NULL;
    MString                        infoFile        = fileName;
    eRet                           ret             = eRet_Ok;
    NEXUS_PlaybackSettings         playbackSettings;
    NEXUS_PlaybackPidChannelSettings  playbackPidSettings;

    BDBG_ASSERT(NULL != _pModel);
    BDBG_ASSERT(NULL != _pModelConfig);
    pBoardResources = _pModelConfig->getBoardResources();
    pCfg            = _pModelConfig->getCfg();

    /* Check the media Filename First */
    if (pPlaybackList != NULL)
    {              
       pVideo = pPlaybackList->checkName(fileName);
       
       /* Check it one last time if its still NULL*/
       if (pVideo == NULL)
       {
             BDBG_WRN((" This file is not in playbackList, Try to add new file"));
             updatePlaybackList(); /* use default Path for now for right now */
             pPlaybackList->sync();
             pVideo = pPlaybackList->checkName(fileName);
       }
                                      
       /* Check for empty Filename this means resume playback */
       if (!fileName && _pPlayback->isActive())
       {
          /* resume playback */
          ret = _pPlayback->play(); 
          goto done;
       }
                                      
       if (!pVideo)
       {
          BDBG_WRN((" Filename does not exist!"));
          goto done;
       }

    }

    if (NULL == _pPlayback)
    {
        _pPlayback = (CPlayback *)pBoardResources->checkoutResource(this, eBoardResource_playback);
        CHECK_PTR_GOTO("unable to checkout Playback ", _pPlayback, ret, eRet_NotAvailable, done);
        _pModel->addPlayback(_pPlayback);
    } else 
    {

       /* Change to new playback */
       if(_pPlayback->getVideo() != pVideo)
       {
             BDBG_MSG((" Switch Playback %s from %s to %s ",__FUNCTION__,fileName.s(),pVideo->_mediaName.s() ));
             pAudioDecode->stop();
             pVideoDecode->stop();
             _pPlayback->stop();
       } else
       {
          return _pPlayback->play();     
       }
    }
    
    /* Check Path */
    if (!path)
    {
       MString newPath = GET_STR(pCfg,VIDEOS_PATH);
       newPath += "/";
       /* check video path now */
       if (pVideo && pVideo->_path.isNull())
       {
          pVideo->_path = newPath;
       }else if (pVideo && pVideo->_path.strncmp(newPath) != 0)
       {
            /* overwrite */
            pVideo->_path = newPath;
       }
    }
    
    if ((NULL != pCurrentChannel) && (true == pCurrentChannel->isTuned()))
    {

        BDBG_WRN((" Current Channel is Tuned, This channel will now be UnTuned"));
        /* Untune the Channel */
        unTuneChannel(pCurrentChannel);
    }

    //_pPlayback->dump();
    _pPlayback->setVideo(pVideo);

    /* Populate pids */
    pVideoPid = _pPlayback->getPid(0, ePidType_Video);
    pAudioPid = _pPlayback->getPid(0, ePidType_Audio);
    pPcrPid   = _pPlayback->getPid(0, ePidType_Pcr);

    if (pPcrPid == NULL) 
    {
        BDBG_WRN(("NULL PCRPID"));
    }

    if (pStc == NULL) 
    {
        BDBG_WRN(("NULL STC"));
    }

    /* Configure STC first */
    pStc->setStcType(eStcType_PvrPlayback);
    ret = pStc->configure(pPcrPid);
    CHECK_ERROR_GOTO("error configuring stc channel", ret, error);
    
    /*_pPlayback->printPids(); */
    playbackSettings = _pPlayback->getSettings();
    playbackSettings.stcChannel = pStc->getStcChannel();
    playbackSettings.stcTrick = true;
    _pPlayback->setSettings(playbackSettings);

    /* open pids */
    if (pVideoPid)
    {
        CHECK_PTR_GOTO("missing playback - playback failed", _pPlayback, ret, eRet_InvalidState, error);
        pVideoPid->getSettings(&playbackPidSettings);
        playbackPidSettings.pidTypeSettings.video.simpleDecoder = pVideoDecode->getSimpleDecoder();
        pVideoPid->setSettings(&playbackPidSettings);
        ret = pVideoPid->open(_pPlayback);
        CHECK_ERROR_GOTO("open playback video pid channel failed", ret, error);
    }

    if (pAudioPid)
    {
        CHECK_PTR_GOTO("missing playback - playback failed", _pPlayback, ret, eRet_InvalidState, error);
        pAudioPid->getSettings(&playbackPidSettings);
        playbackPidSettings.pidTypeSettings.audio.simpleDecoder = pAudioDecode->getSimpleDecoder();
        pAudioPid->setSettings(&playbackPidSettings);
        ret = pAudioPid->open(_pPlayback);
        CHECK_ERROR_GOTO("open playback video pid channel failed", ret, error);
    }

    if (pPcrPid)
    {
        /* only open pcr pid channel if it is different from audio/video pids */
        if (false == pPcrPid->isOpen())
        {
            CHECK_PTR_GOTO("missing playback - playback failed", _pPlayback, ret, eRet_InvalidState, error);
            ret = pPcrPid->open(_pPlayback);
            CHECK_ERROR_GOTO("open playback video pid channel failed", ret, error);
        }
    }

    if (NULL != pVideoPid)
    {
        ret = pVideoDecode->start(pVideoPid);
        CHECK_ERROR_GOTO("unable to start video decode", ret, error);
    }

    if (NULL != pAudioPid)
    {
        ret = pAudioDecode->start(pAudioPid);
        CHECK_ERROR_GOTO("unable to start audio decode", ret, error);
    }

    ret = _pPlayback->start();
    if (ret != eRet_Ok)
    {
       goto error;
    }

    _pModel->setMode(eMode_Playback);
    
done:
   return ret;
error:
    if (pVideoPid)
    {
       /* Check to see if PCR pid is the same as Video Pid */
       if (pPcrPid == pVideoPid)
       {
          pPcrPid= NULL;
       }
       pVideoDecode->stop();
       pVideoPid->close(_pPlayback);

    }
    if (pAudioPid)
    {
       pAudioDecode->stop();
       pAudioPid->close(_pPlayback);
    }
    if (pPcrPid)
    {
       pPcrPid->close(_pPlayback);
    }

    /* Re Tune the channel */
    if (NULL != pCurrentChannel)
    {
       BDBG_WRN((" Playback Failed return to Previous Channel "));
       /* Untune the Channel */
       tuneChannel(pCurrentChannel);
    }

return ret;

}

/* Right now we don't use mediaName.
   Multiple Playbacks will need MediaName */
eRet CControl::playbackStop(MString mediaName)
{
    eRet ret = eRet_Ok;
    CSimpleVideoDecode  * pVideoDecode    = _pModel->getSimpleVideoDecode();
    CSimpleAudioDecode  * pAudioDecode    = _pModel->getSimpleAudioDecode();
    
    BDBG_ASSERT(NULL != _pModel);
    
    pAudioDecode->stop();
    pVideoDecode->stop();

    //_pPlayback->dump();
    /*_pPlayback->printPids(); */
    
    ret = _pPlayback->stop();
    CHECK_ERROR_GOTO("playback error!", ret, error);

    /* check in playback?! */
    {
        CBoardResources * pBoardResources = _pModelConfig->getBoardResources();

        ret = pBoardResources->checkinResource(_pPlayback);
        CHECK_ERROR_GOTO("unable to checkin Playback ", ret, error);
    }

    _pPlayback = NULL;
    _pModel->addPlayback(NULL);

    /* Tune to the last Channel */
    tuneChannel(NULL);
    
    
done:
error:
return ret;

}

#if NEXUS_HAS_FRONTEND
eRet CControl::scanTuner(CTunerScanData * pScanData)
{
    eRet                        ret                   = eRet_Ok;
    CBoardResources           * pBoardResources       = NULL;
    CTuner                    * pTuner                = NULL;
    CModelChannel             * pCurrentChannel       = _pModelChannelMgr->getCurrentChannel();
    CModelChannel             * pChannel              = NULL;
    MList <CModelChannel>       foundChannelList;

    BDBG_ASSERT(NULL != pScanData);
    pBoardResources = _pModelConfig->getBoardResources();

    if (true == pScanData->_appendToChannelList)
    {
        CModelChannel * pChannelLast = _pModelChannelMgr->getLastChannel();

        /* untune only channels matching tuner type if appending to current channel list */
        while (NULL != (pChannel = _pModelChannelMgr->findTunedChannel(pScanData->getTunerType())))
        {
            /* untune tuners */
            if (pChannel->isRecording())
            {
                recordStop(pChannel); 
                if (pChannel == pCurrentChannel)
                {
                    unTuneChannel(pChannel);
                    pCurrentChannel = NULL;
                }
            } else
                unTuneChannel(pChannel);
        }

        /* set the starting major channel number for the newly found channnels */
        if (NULL != pChannelLast)
        {
            pScanData->_majorStartNum = pChannelLast->getMajor() + 1;
        }
    }
    else
    {
        /* untune all channels if replacing current channel list */
        while (NULL != (pChannel = _pModelChannelMgr->findTunedChannel()))
        {
            /* untune tuners */
            if (pChannel->isRecording())
            {
                recordStop(pChannel);
                if (pChannel == pCurrentChannel)
                {
                    unTuneChannel(pChannel);
                    pCurrentChannel = NULL;
                }
            }else 
                unTuneChannel(pChannel);
        }
    }

    /* check out tuner to use for scan */
    pTuner = (CTuner *)pBoardResources->checkoutResource(this, pScanData->getTunerType());
    CHECK_PTR_GOTO("No tuner available - scan failed!", pTuner, ret, eRet_NotAvailable, errorNoTuner);

    BDBG_ASSERT(true == pTuner->isFrontend());
    pTuner->setModelConfig(_pModelConfig);

    if (false == pScanData->_appendToChannelList)
    {
        /* we are replacing current channel list so clear it */
        _pModelChannelMgr->clearChannelList();
    }

    /* scan for channels - note that scan() will dynamically allocate found channel objects and
       return them to the specified callback routine.  this is an asynchronous call so it will
       return immediately. */
    ret = pTuner->scan(this, pScanData, addChannelCallback, getWidgetEngine(), (void *)this);
                                                  
    _pModel->setMode(eMode_Scan);
    goto done;

errorNoTuner:
    tuneChannel();

done:
    return ret;
}
#endif /* NEXUS_HAS_FRONTEND */


eRet CControl::channelUp()
{
    eRet            ret      = eRet_Ok;
    CModelChannel * pChannel = NULL;

    if(_pPlayback && _pPlayback->isActive())
    {
        playbackStop(MString(""));
    }

    pChannel = _pModelChannelMgr->getNextChannel(_pModelChannelMgr->getDeferredChannel());
    CHECK_PTR_GOTO("Unable to tune - channel list is empty", pChannel, ret, eRet_NotAvailable, error);

    _pModelChannelMgr->setDeferredChannel(pChannel);
    _deferredChannelTimer.start();

error:
    return ret;
}

eRet CControl::channelDown()
{
    eRet            ret      = eRet_Ok;
    CModelChannel * pChannel = NULL;

    if(_pPlayback && _pPlayback->isActive())
    {
        playbackStop(MString(""));
    }

    pChannel = _pModelChannelMgr->getPrevChannel(_pModelChannelMgr->getDeferredChannel());
    CHECK_PTR_GOTO("Unable to tune - channel list is empty", pChannel, ret, eRet_NotAvailable, error);

    _pModelChannelMgr->setDeferredChannel(pChannel);
    _deferredChannelTimer.start();

error:
    return ret;
}

void CControl::updatePlaybackList()
{
    CPlaybackList * pPlaybackList = _pModel->getPlaybackList();
    MString path = MString(GET_STR(_pCfg,VIDEOS_PATH))+MString("/");

    BDBG_ASSERT(NULL != pPlaybackList);
    pPlaybackList->refreshFromDisk(path);
}

eRet CControl::setAudioProgram(uint16_t pid)
{
    CSimpleAudioDecode * pAudioDecode = _pModel->getSimpleAudioDecode();
    CPid               * pPidOld      = NULL;
    CPid               * pPidNew      = NULL;
    eRet                 ret          = eRet_Ok;

    BDBG_ASSERT(NULL != pAudioDecode);

    switch (_pModel->getMode())
    {
    case eMode_Live:
        {
            CModelChannel * pCurrentCh = _pModelChannelMgr->getCurrentChannel();
            BDBG_ASSERT(NULL != pCurrentCh);

            /* find pid object matching given pid num */
            pPidNew = pCurrentCh->findPid(pid, ePidType_Audio);
            CHECK_PTR_GOTO("unable to find given pid number in current channel", pPidNew, ret, eRet_InvalidParameter, error);

            if (pPidNew != pAudioDecode->getPid())
            {
                /* newly requested pid is different than current so change to it */
                pPidOld = pAudioDecode->stop();
                pPidOld->close();

                pPidNew->open(pCurrentCh->getParserBand());
                pAudioDecode->start(pPidNew);
            }
        }
        break;

    case eMode_Playback:
        {
            CPlayback * pPlayback = _pModel->getPlayback();

            /* find pid object matching given pid num */
            pPidNew = pPlayback->findPid(pid, ePidType_Audio);
            CHECK_PTR_GOTO("unable to find given pid number in current channel", pPidNew, ret, eRet_InvalidParameter, error);

            if (pPidNew != pAudioDecode->getPid())
            {
                NEXUS_PlaybackPidChannelSettings  playbackPidSettings;

                /* newly requested pid is different than current so change to it */
                pPidOld = pAudioDecode->stop();
                pPidOld->close();

                pPidNew->open(pPlayback);
                pPidNew->getSettings(&playbackPidSettings);
                playbackPidSettings.pidTypeSettings.audio.simpleDecoder = pAudioDecode->getSimpleDecoder();
                pPidNew->setSettings(&playbackPidSettings);
                pAudioDecode->start(pPidNew);
            }
        }
        break;

    default:
        break;
    }

error:
    return ret;
}

eRet CControl::setSpdifInput(eSpdifInput spdifInput)
{
    CPlayback                * pPlayback       = _pModel->getPlayback();
    CSimpleAudioDecode       * pAudioDecode    = _pModel->getSimpleAudioDecode();
    bool                       restartPlayback = false;
    bool                       restartDecode   = false;
    CPid                     * pPid            = NULL;
    eRet                       ret             = eRet_Ok;

    BDBG_ASSERT(NULL != pAudioDecode);

    if ((NULL != pPlayback) && (true == pPlayback->isActive()))
    {
        ret = pPlayback->pause();
        CHECK_ERROR_GOTO("unable to pause playback", ret, error);
        restartPlayback = true;
    }

    if ((NULL != pAudioDecode) && (true == pAudioDecode->isStarted()))
    {
        pPid = pAudioDecode->stop();
        CHECK_PTR_GOTO("unable to stop decode", pPid, ret, eRet_InvalidParameter, error);
        restartDecode = true;
    }

    pAudioDecode->setSpdifInputDecoderType(spdifInput);

    if ((NULL != pAudioDecode) && (true == restartDecode))
    {
        ret = pAudioDecode->start(pPid);
        CHECK_ERROR_GOTO("unable to restart decoder", ret, error);
    }

    if ((NULL != pPlayback) && (true == restartPlayback))
    {
        ret = pPlayback->start();
        CHECK_ERROR_GOTO("unable to restart playback", ret, error);
    }

error:
    return ret;
}

eRet CControl::setHdmiAudioInput(eHdmiAudioInput hdmiInput)
{
    CPlayback                * pPlayback       = _pModel->getPlayback();
    CSimpleAudioDecode       * pAudioDecode    = _pModel->getSimpleAudioDecode();
    bool                       restartPlayback = false;
    bool                       restartDecode   = false;
    CPid                     * pPid            = NULL;
    eRet                       ret             = eRet_Ok;

    BDBG_ASSERT(NULL != pAudioDecode);

    if ((NULL != pPlayback) && (true == pPlayback->isActive()))
    {
        ret = pPlayback->pause();
        CHECK_ERROR_GOTO("unable to pause playback", ret, error);
        restartPlayback = true;
    }

    if ((NULL != pAudioDecode) && (true == pAudioDecode->isStarted()))
    {
        pPid = pAudioDecode->stop();
        CHECK_PTR_GOTO("unable to stop decode", pPid, ret, eRet_InvalidParameter, error);
        restartDecode = true;
    }

    pAudioDecode->setHdmiInputDecoderType(hdmiInput);

    if ((NULL != pAudioDecode) && (true == restartDecode))
    {
        ret = pAudioDecode->start(pPid);
        CHECK_ERROR_GOTO("unable to restart decoder", ret, error);
    }

    if ((NULL != pPlayback) && (true == restartPlayback))
    {
        ret = pPlayback->start();
        CHECK_ERROR_GOTO("unable to restart playback", ret, error);
    }

error:
    return ret;
}


eRet CControl::setAudioDownmix(eAudioDownmix audioDownmix)
{
    CSimpleAudioDecode * pAudioDecode    = _pModel->getSimpleAudioDecode();
    CPlayback          * pPlayback       = _pModel->getPlayback();
    CPid               * pPid            = NULL;
    bool                 restartPlayback = false;
    bool                 restartDecode   = false;
    eRet                 ret             = eRet_Ok;

    BDBG_ASSERT(NULL != pAudioDecode);

    if ((NULL != pPlayback) && (true == pPlayback->isActive()))
    {
        ret = pPlayback->pause();
        CHECK_ERROR_GOTO("unable to pause playback", ret, error);
        restartPlayback = true;
    }

    if ((NULL != pAudioDecode) && (true == pAudioDecode->isStarted()))
    {
        pPid = pAudioDecode->stop();
        CHECK_PTR_GOTO("unable to stop decode", pPid, ret, eRet_InvalidParameter, error);
        restartDecode = true;
    }

    pAudioDecode->setDownmix(audioDownmix);

    if ((NULL != pAudioDecode) && (true == restartDecode))
    {
        ret = pAudioDecode->start(pPid);
        CHECK_ERROR_GOTO("unable to restart decoder", ret, error);
    }

    if ((NULL != pPlayback) && (true == restartPlayback))
    {
        ret = pPlayback->start();
        CHECK_ERROR_GOTO("unable to restart playback", ret, error);
    }

error:
    return ret;
}

eRet CControl::setAudioDualMono(eAudioDualMono audioDualMono)
{
    CSimpleAudioDecode * pAudioDecode    = _pModel->getSimpleAudioDecode();
    CPlayback          * pPlayback       = _pModel->getPlayback();
    CPid               * pPid            = NULL;
    bool                 restartPlayback = false;
    bool                 restartDecode   = false;
    eRet                 ret             = eRet_Ok;

    BDBG_ASSERT(NULL != pAudioDecode);

    if ((NULL != pPlayback) && (true == pPlayback->isActive()))
    {
        ret = pPlayback->pause();
        CHECK_ERROR_GOTO("unable to pause playback", ret, error);
        restartPlayback = true;
    }

    if ((NULL != pAudioDecode) && (true == pAudioDecode->isStarted()))
    {
        pPid = pAudioDecode->stop();
        CHECK_PTR_GOTO("unable to stop decode", pPid, ret, eRet_InvalidParameter, error);
        restartDecode = true;
    }

    pAudioDecode->setDualMono(audioDualMono);

    if ((NULL != pAudioDecode) && (true == restartDecode))
    {
        ret = pAudioDecode->start(pPid);
        CHECK_ERROR_GOTO("unable to restart decoder", ret, error);
    }

    if ((NULL != pPlayback) && (true == restartPlayback))
    {
        ret = pPlayback->start();
        CHECK_ERROR_GOTO("unable to restart playback", ret, error);
    }

error:
    return ret;
}

eRet CControl::setDolbyDRC(eDolbyDRC dolbyDRC)
{
    CSimpleAudioDecode * pAudioDecode    = _pModel->getSimpleAudioDecode();
    CPlayback          * pPlayback       = _pModel->getPlayback();
    CPid               * pPid            = NULL;
    bool                 restartPlayback = false;
    bool                 restartDecode   = false;
    eRet                 ret             = eRet_Ok;

    BDBG_ASSERT(NULL != pAudioDecode);

    if ((NULL != pPlayback) && (true == pPlayback->isActive()))
    {
        ret = pPlayback->pause();
        CHECK_ERROR_GOTO("unable to pause playback", ret, error);
        restartPlayback = true;
    }

    if ((NULL != pAudioDecode) && (true == pAudioDecode->isStarted()))
    {
        pPid = pAudioDecode->stop();
        CHECK_PTR_GOTO("unable to stop decode", pPid, ret, eRet_InvalidParameter, error);
        restartDecode = true;
    }

    pAudioDecode->setDolbyDRC(dolbyDRC);

    if ((NULL != pAudioDecode) && (true == restartDecode))
    {
        ret = pAudioDecode->start(pPid);
        CHECK_ERROR_GOTO("unable to restart decoder", ret, error);
    }

    if ((NULL != pPlayback) && (true == restartPlayback))
    {
        ret = pPlayback->start();
        CHECK_ERROR_GOTO("unable to restart playback", ret, error);
    }

error:
    return ret;
}

eRet CControl::setDolbyDialogNorm(bool dolbyDialogNorm)
{
    CSimpleAudioDecode * pAudioDecode    = _pModel->getSimpleAudioDecode();
    CPlayback          * pPlayback       = _pModel->getPlayback();
    CPid               * pPid            = NULL;
    bool                 restartPlayback = false;
    bool                 restartDecode   = false;
    eRet                 ret             = eRet_Ok;

    BDBG_ASSERT(NULL != pAudioDecode);

    if ((NULL != pPlayback) && (true == pPlayback->isActive()))
    {
        ret = pPlayback->pause();
        CHECK_ERROR_GOTO("unable to pause playback", ret, error);
        restartPlayback = true;
    }

    if ((NULL != pAudioDecode) && (true == pAudioDecode->isStarted()))
    {
        pPid = pAudioDecode->stop();
        CHECK_PTR_GOTO("unable to stop decode", pPid, ret, eRet_InvalidParameter, error);
        restartDecode = true;
    }

    pAudioDecode->setDolbyDialogNorm(dolbyDialogNorm);

    if ((NULL != pAudioDecode) && (true == restartDecode))
    {
        ret = pAudioDecode->start(pPid);
        CHECK_ERROR_GOTO("unable to restart decoder", ret, error);
    }

    if ((NULL != pPlayback) && (true == restartPlayback))
    {
        ret = pPlayback->start();
        CHECK_ERROR_GOTO("unable to restart playback", ret, error);
    }

error:
    return ret;
}

eRet CControl::setColorSpace(NEXUS_ColorSpace colorSpace)
{
    CDisplay  * pDisplay  = _pModel->getDisplay();
    eRet        ret       = eRet_Ok;

    BDBG_ASSERT(NULL != pDisplay);
   
    ret = pDisplay->setColorSpace(colorSpace);
    CHECK_ERROR_GOTO("unable to set color space", ret, error);

error:
    return ret;
}

eRet CControl::setMpaaDecimation(bool bMpaaDecimation)
{
    CDisplay  * pDisplay  = _pModel->getDisplay();
    eRet        ret       = eRet_Ok;

    BDBG_ASSERT(NULL != pDisplay);
   
    ret = pDisplay->setMpaaDecimation(bMpaaDecimation);
    CHECK_ERROR_GOTO("unable to set MPAA decimation", ret, error);

error:
    return ret;
}

eRet CControl::setDeinterlacer(bool bDeinterlacer)
{
    CDisplay  * pDisplay  = _pModel->getDisplay();
    eRet        ret       = eRet_Ok;

    BDBG_ASSERT(NULL != pDisplay);
   
    ret = pDisplay->setDeinterlacer(bDeinterlacer);
    CHECK_ERROR_GOTO("unable to set deinterlacer", ret, error);

error:
    return ret;
}

eRet CControl::setBoxDetect(bool bBoxDetect)
{
    CDisplay  * pDisplay  = _pModel->getDisplay();
    eRet        ret       = eRet_Ok;

    BDBG_ASSERT(NULL != pDisplay);
   
    ret = pDisplay->setBoxDetect(bBoxDetect);
    CHECK_ERROR_GOTO("unable to set Box Detect", ret, error);

error:
    return ret;
}

eRet CControl::setAspectRatio(NEXUS_DisplayAspectRatio aspectRatio)
{
    CDisplay  * pDisplay  = _pModel->getDisplay();
    eRet        ret       = eRet_Ok;

    BDBG_ASSERT(NULL != pDisplay);
   
    ret = pDisplay->setAspectRatio(aspectRatio);
    CHECK_ERROR_GOTO("unable to set Aspect Ratio", ret, error);

error:
    return ret;
}

eRet CControl::setVideoFormat(NEXUS_VideoFormat videoFormat)
{
    CDisplay  * pDisplay  = _pModel->getDisplay();
    CGraphics * pGraphics = _pModel->getGraphics();
    eRet        ret       = eRet_Ok;

    BDBG_ASSERT(NULL != pDisplay);
    BDBG_ASSERT(NULL != pGraphics);
   
    ret = pDisplay->setFormat(videoFormat, pGraphics);
    CHECK_ERROR_GOTO("unable to set video format", ret, error);

error:
    return ret;
}

eRet CControl::setAutoVideoFormat(bool bAutoVideoFormat)
{
    CDisplay  * pDisplay  = _pModel->getDisplay();
    eRet        ret       = eRet_Ok;

    BDBG_ASSERT(NULL != pDisplay);
   
    pDisplay->setAutoFormat(bAutoVideoFormat);

error:
    return ret;
}
