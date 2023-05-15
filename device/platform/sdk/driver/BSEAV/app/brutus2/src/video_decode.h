/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: video_decode.h $
* $brcm_Revision: 7 $
* $brcm_Date: 10/7/12 3:11p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/video_decode.h $
* 
* 7   10/7/12 3:11p tokushig
* SW7231-749: add support for auto format change
* 
* 6   9/21/12 4:11p tokushig
* SW7231-749: fix close/stop in video decode
* 
* 5   9/10/12 3:49p tokushig
* SW7231-749: add simple video decoder support
* 
* 4   4/15/12 9:47p tokushig
* SW7231-749: add start/stop decode notification
* 
* 3   4/9/12 7:29p tokushig
* SW7405-5581: added video source changed callback support
* 
* 2   2/28/12 5:03p tokushig
* SW7405-5581: add header comments
* 
* 
*********************************************************************************/

#ifndef VIDEO_DECODE_H__
#define VIDEO_DECODE_H__

#include "brutus.h"
#include "brutus_cfg.h"
#include "brutus_os.h"
#include "pid.h"
#include "stc.h"
#include "bwidgets.h"

#include "nexus_video_input.h"
#include "nexus_video_decoder_types.h"
#if NEXUS_HAS_SYNC_CHANNEL
#include "nexus_sync_channel.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

class CVideoWindow;
class CVideoOutput;

class CVideoDecode : public CResource
{
public:
    CVideoDecode(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CVideoDecode(void);

    virtual eRet open(bwidget_engine_t widgetEngine, CStc * pStc);
    virtual CStc * close(void);
    virtual eRet start(CPid * pPid);
    virtual CPid * stop(void);

    CPid * getPid(void) { return _pPid; };
    CStc * getStc(void) { return _pStc; };

    bool isStarted() { return _started; };
    bool isOpened() { return _decoder ? true : false; };
    NEXUS_VideoDecoderHandle getDecoder(void)  {  return _decoder; };

    bwidget_engine_t getWidgetEngine(void) { return _widgetEngine; };
    bool isSourceChanged() { return _sourceChanged; };
    void setSourceChanged(bool sourceChanged) { _sourceChanged = sourceChanged; };
    eRet getStatus(NEXUS_VideoDecoderStatus * pStatus);

protected:
    NEXUS_VideoDecoderHandle    _decoder;
#if NEXUS_HAS_SYNC_CHANNEL
    NEXUS_SyncChannelHandle     _syncChannel;    /* Nexus Sync channel associated with each decode */
#endif
    CStc                      * _pStc;
    CPid                      * _pPid;
    bwidget_engine_t            _widgetEngine;
    bool                        _started;
    bool                        _sourceChanged;
};

class CBoardResources;

class CSimpleVideoDecode : public CVideoDecode
{
public:
    CSimpleVideoDecode(const char * name, const uint16_t number, CConfiguration * pCfg);
    ~CSimpleVideoDecode();

    virtual eRet open(bwidget_engine_t widgetEngine, CStc * pStc);
    virtual CStc * close(void);
    virtual eRet start(CPid * pPid);
    virtual CPid * stop(void);
    virtual bool isOpened() { return _simpleDecoder ? true : false; };

    eRet setDisplay(CDisplay * pDisplay) { _pDisplay = pDisplay; };
    void setResources(void * id, CBoardResources * pResources) { _pBoardResources = pResources; };
    eRet getStatus(NEXUS_VideoDecoderStatus * pStatus);
    eRet getOptimalVideoFormat(CVideoOutput * pOutput, NEXUS_VideoFormat * pFormat);

    virtual NEXUS_SimpleVideoDecoderHandle getSimpleDecoder() { return _simpleDecoder; };

protected:
    NEXUS_SimpleVideoDecoderHandle   _simpleDecoder;
    CBoardResources                * _pBoardResources;
    CVideoDecode                   * _pDecoder;
    CDisplay                       * _pDisplay;
    CVideoWindow                   * _pVideoWindow;
    void                           * _resourceId;
};


#ifdef __cplusplus
}
#endif

#endif
