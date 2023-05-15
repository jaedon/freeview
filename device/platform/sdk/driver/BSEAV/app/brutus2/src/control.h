#ifndef CONTROL2_H__
#define CONTROL2_H__

/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: control.h $
* $brcm_Revision: 32 $
* $brcm_Date: 11/15/12 10:46a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/control.h $
* 
* 32   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 31   10/19/12 1:02p jrubio
* SW7346-1071: remove duplicate Playback Start function, do cleanup after
* playback failure
* 
* 30   10/19/12 10:36a tokushig
* SW7231-749: fix brutus shutdown by freeing resources in the proper
* order.
* 
* 29   10/12/12 12:14a tokushig
* SW7231-749: restrict tuning while playback is active
* 
* 28   10/11/12 11:31p jrubio
* SW7346-1004: redo recordStart to accept a channel handle
* 
* 27   10/11/12 9:37p jrubio
* SW7346-1004: add multiple records
* 
* 26   10/11/12 4:02p tokushig
* SW7231-749: remove dead code
* 
* 25   10/8/12 6:47p tokushig
* SW7231-749: add lua commands for display settings
* 
* 24   10/7/12 2:57p tokushig
* SW7231-749: add format change support and fixes for audio settings.
* 
* 23   9/29/12 9:24p tokushig
* SW7231-749: connect audio settings window panel to simple audio
* decoder.  pid, spdif, hdmi, downmix, dualmono, dolby drc, and dialog
* norm are implemented.
* 
* 22   9/26/12 4:49p tokushig
* SW7231-749: add ability to set hdmi/spdif output format to controller
* 
* 21   9/21/12 8:42p tokushig
* SW7231-749: scan->scanStart notification, get mode from model, add
* setAudioProgram notification command
* 
* 21   9/21/12 8:40p tokushig
* SW7231-749: scan->scanStart notification, get mode from model, add
* setAudioProgram notification command
* 
* 20   9/18/12 5:43p jrubio
* SW7346-1004: add use of video object
* 
* 19   9/17/12 10:26a jrubio
* SW7346-1004: modify record functions only filename/path are needed
* 
* 18   8/29/12 10:18a jrubio
* SW7346-1004: add record capabilities
* 
* 17   8/27/12 10:13a tokushig
* SW7231-749: add support for updated scan and auto pid acquisition on
* tune
* 
* 16   7/9/12 4:30p tokushig
* SW7231-749: refactor the main screen so that it can coexist with other
* screens and properly segment code into cpanels.  partially complete -
* info menu and buffers menu still need refactoring.
* 
* 15   5/21/12 5:37p jrubio
* SW7346-644: pass video object
* 
* 14   4/19/12 7:31p tokushig
* SW7231-749: restrict conflicting actions during scan
* 
* 14   4/19/12 7:29p tokushig
* SW7231-749: restrict conflicting actions during scan
* 
* 13   4/18/12 5:14p jrubio
* SW7346-644: modify playback start, add key presses
* 
* 12   4/18/12 12:47a jrubio
* SW7346-644: add CPlaybackList videoList
* 
* 11   4/15/12 12:11a jrubio
* SW7346-644: add playback stop function
* 
* 10   4/12/12 12:11a jrubio
* SW7346-644: add more logic for checking out playback resource
* 
* 9   4/11/12 1:20p jrubio
* SW7346-644: add Playback and reworked CPidMgr, Lua Command invokes
* Playback Start
* 
* 8   4/6/12 12:43p tokushig
* SW7231-749: fixed callback handling issue in register/notification
* mechanism which prevented control from manually notifying views.  view
* and controller classes now hide the callback mechanism from the mvc
* based classes for notifications and there is a consistent
* processNotification() method for both views and controllers (models do
* not receive notifications.
* 
* 7   3/19/12 5:27p tokushig
* SW7405-5581: add config entries to enable channel scan to save
* encrypted channels, audio only channels, and duplicate channels
* 
* 6   3/9/12 2:55p tokushig
* SW7405-5581: added support for channel list save.  also added initial
* code to handle tuner lock/unlock after first tune.
* 
* 5   2/28/12 3:01p tokushig
* SW7405-5581: add timer support and use it for deferred tuning using ch
* up/down.  fast pressing of ch up/down remote buttons will update the
* ui but not tune until button presses stop for 500msecs.  this allows
* the user to skip channels while traversing the channel list using the
* remote
* 
* 
*********************************************************************************/

#include "brutus_cfg.h"
#include "view.h"
#include "config.h"
#include "timer.h"
#include "controller.h"
#include "channelmgr.h"
#include "channel.h"

class CPlayback;
class CRecord;

#ifdef __cplusplus
extern "C" {
#endif

class CControl : public CController
{
public:
    CControl(void);
    ~CControl(void);
    eRet initialize(CModelConfig * pModelConfig, CModelChannelMgr * pModelChannelMgr, bwidget_engine_t widgetEngine);
    eRet uninitialize();
    void processNotification(CNotification & notification);
    eRet processKeyEvent(CRemoteEvent * pRemoteEvent);
    void onIdle(void);

    bwidget_engine_t getWidgetEngine(void) { return _widgetEngine; };

    eRet tuneChannel(CModelChannel * pChannel = NULL);
    eRet unTuneChannel(CModelChannel * pChannel = NULL);
    eRet addChannelToChList(CModelChannel * pChannel);
#if NEXUS_HAS_FRONTEND
    eRet scanTuner(CTunerScanData * pScanData);
#endif
    eRet channelUp(void);
    eRet channelDown(void);
    eRet playbackStart(MString fileName, MString indexName, MString path);
    eRet playbackStop(MString MediaName);
    eRet recordStart(MString fileName, MString path);
    eRet recordStop(CModelChannel * pChannel = NULL);
    eRet setAudioProgram(uint16_t pid);
    eRet setSpdifInput(eSpdifInput spdifInput);
    eRet setHdmiAudioInput(eHdmiAudioInput hdmiInput);
    eRet setAudioDownmix(eAudioDownmix audioDownmix);
    eRet setAudioDualMono(eAudioDualMono audioDualMono);
    eRet setDolbyDRC(eDolbyDRC dolbyDRC);
    eRet setDolbyDialogNorm(bool dolbyDialogNorm);
    eRet setColorSpace(NEXUS_ColorSpace colorSpace);
    eRet setMpaaDecimation(bool bMpaaDecimation);
    eRet setDeinterlacer(bool bDeinterlacer);
    eRet setBoxDetect(bool bBoxDetect);
    eRet setAspectRatio(NEXUS_DisplayAspectRatio aspectRatio);
    eRet setVideoFormat(NEXUS_VideoFormat videoFormat);
    eRet setAutoVideoFormat(bool bAutoVideoFormat);

    void setModel(CModel * pModel) { _pModel = pModel; };
    void setModelChannelMgr(CModelChannelMgr * pModelChannelMgr) { _pModelChannelMgr = pModelChannelMgr; };
    CModelChannelMgr * getModelChannelMgr(void) { return _pModelChannelMgr; };
    CConfiguration * getCfg(void) { return _pCfg; };
    CPlayback * getPlayack(void) { return _pPlayback; };
    void updatePlaybackList();

    void addView(CView * pView, const char * name);
    CView * findView(const char * name);

    bool validateNotification(CNotification & notification, eMode mode);

protected:
    class CViewListNode
    {
    public:
        CViewListNode(CView * pView, const char * name) :
            _pView(pView),
            _strName(name)
        {};

    public:
        CView   * _pView;
        MString   _strName;
    };

protected:
    CModel                * _pModel;
    CModelConfig          * _pModelConfig;
    CModelChannelMgr      * _pModelChannelMgr;
    CConfiguration        * _pCfg;
    bwidget_engine_t        _widgetEngine;
    CTimer                  _deferredChannelTimer;
    CTimer                  _tunerLockCheckTimer;
    MList <CViewListNode>   _viewList;
    MList <CModelChannel>   _recordingChannels;
    CPlayback             * _pPlayback;
};


#ifdef __cplusplus
}
#endif

#endif /* CONTROL2_H__ */
