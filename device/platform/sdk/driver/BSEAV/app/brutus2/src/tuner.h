/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: tuner.h $
* $brcm_Revision: 21 $
* $brcm_Date: 11/15/12 11:04a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/tuner.h $
* 
* 21   11/15/12 11:04a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 20   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 19   11/13/12 10:43p tokushig
* SW7231-749: improve resiliency of tune/scan.  handle case where fast
* status indicates not locked but also not still searching - we don't
* want to give up until status indicates unlocked or we timeout.
* 
* 18   11/12/12 3:40p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 17   10/19/12 5:43p tokushig
* SW7231-749: ensure tuners are properly checked in/out during tune/scan.
* 
* 16   10/16/12 3:47p tokushig
* SW7231-749: fix tuner issue with 97231FBTSFF board
* 
* 15   10/9/12 4:06p tokushig
* SW7231-749: add debug to tuner
* 
* 14   10/8/12 4:17p jrubio
* SW7346-1004: add getSoftDecisions functions
* 
* 13   8/27/12 10:06a tokushig
* SW7231-749: move psip acquisition to tuner class so subclasses can all
* inheirit
* 
* 12   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 11   6/15/12 4:51p tokushig
* SW7231-749: update scan to handle error where intended tuner is
* unavailable.  also better handle display of progress meter so 100% is
* displayed before meter is hidden
* 
* 10   4/20/12 2:45p tokushig
* SW7231-749: add support for scan menu
* 
* 9   4/19/12 7:35p tokushig
* SW7231-749: fix problem with scan thread where notifications were not
* synchronized with the bwidget main loop.
* 
* 8   3/19/12 4:25p tokushig
* SW7405-5581: Add support for filtering channel list during scan.
* Ignore encrypted channels, channels with invalid pcr pids, and audio
* only channels.
* 
* 7   3/14/12 12:40p jrubio
* SW7346-644: add tuner sat
* 
* 6   3/1/12 3:56p tokushig
* SW7405-5581: moved qam scan into separate thread to enable screen
* updates while scan is running.  added on screen scan progress to test.
* NOTE: this does not yet have protection against conflicts so trying to
* channel up/down while scan is running will result in a crash
* 
* 5   2/27/12 4:17p tokushig
* SW7405-5581: break out qam tuner from tuner files
* 
* 
*********************************************************************************/

#ifndef TUNER_H__
#define TUNER_H__

#include "brutus_cfg.h"
#include "brutus_os.h"
#include "resource.h"
#include "bwidgets.h"
#include "tspsimgr2.h"

class CModelChannel;

/* returns true if channel was added, false if channel was ignored */
typedef bool (*CTunerScanCallback)(CModelChannel * pChannel, void * context);

#if NEXUS_HAS_FRONTEND

class CModelConfig;
class CTuner;
class CParserBand;

/* notification data sent during CTuner based notifyObservers() calls */
class CTunerScanNotificationData
{
public:
    CTunerScanNotificationData(CTuner * pTuner = NULL, uint8_t percent = 0) : 
        _pTuner(pTuner), 
        _percent(percent), 
        _appendToChannelList(false) {};

    uint8_t getPercent(void) { return _percent; };
    void setPercent(uint8_t percent) { _percent = percent; };
    CTuner * getTuner(void) { return _pTuner; };
    void setTuner(CTuner * pTuner) { _pTuner = pTuner; };
    bool getAppendToChannelList(void) { return _appendToChannelList; };
    void setAppendToChannelList(bool append) { _appendToChannelList = append; };
    
protected:
    uint8_t   _percent;
    CTuner  * _pTuner;
    bool      _appendToChannelList;
};

/* Tuner scan events are stored temporarily until the Brutus main loop has a chance to service them.
   This is done because the scan operates in a thread.  Data associated with the Tuner event is allocated
   when the scan notification is sent, and freed after the associated action is completed.  The CTunerEvent
   defines a Tuner event that contains no notification data.  Tuner events that require data subclass
   from CTunerEvent.
 */
class CTunerEvent
{
public:
    CTunerEvent(eNotification id) :
        _id(id) {};

    eNotification  getId(void) { return _id; };
    virtual void * getDataPtr(void) { return NULL; };
    
protected:
    eNotification   _id;
};

template<class T>  
class CTunerDataEvent : public CTunerEvent
{
public:
    CTunerDataEvent(eNotification id, T * data) : 
        CTunerEvent(id),
        _data(data) {};

    ~CTunerDataEvent(void) { if (NULL != _data) delete _data; };

    void * getDataPtr(void) { return (void *)_data; };

protected:
    T * _data;
};


class CTunerScanData
{
public:
    CTunerScanData(eBoardResource tunerType, bool appendToChannelList = false, uint16_t majorStartNum = 1) :
        _tunerType(tunerType),
        _appendToChannelList(appendToChannelList),
        _majorStartNum(majorStartNum) {};

    void setTunerType(eBoardResource tunerType) { _tunerType = tunerType; };
    eBoardResource getTunerType(void) { return _tunerType; };

    /* assignment operator overload to manually copy internal MAutoList */
    CTunerScanData & operator=(const CTunerScanData & rhs) {
        MListItr <uint32_t> itr(&rhs._freqList);
        uint32_t * pFreq = NULL;

        _tunerType           = rhs._tunerType;
        _appendToChannelList = rhs._appendToChannelList;
        _majorStartNum       = rhs._majorStartNum;

        /* copy frequency autolist manually */
        _freqList.clear();
        for (pFreq = (uint32_t *)itr.first(); pFreq; pFreq = (uint32_t *)itr.next())
        {
            _freqList.add(new uint32_t(*pFreq));
        }

        return *this;
    }

protected:
    eBoardResource _tunerType;
public:
    MAutoList <uint32_t>   _freqList;
    bool                   _appendToChannelList;
    uint16_t               _majorStartNum;
};


class CTuner : public CResource
{
public:
    CTuner(const char * name, const uint16_t number, eBoardResource resource, CConfiguration * pCfg);
    ~CTuner(void);
    
    virtual eRet initialize(void);

    virtual void saveScanData(CTunerScanData * pScanData) = 0; /* requires implementation by sub class to save tuner specific scan data */
    virtual void doScan(void) = 0; /* requires implementation by sub class to do actual scanning */
    virtual NEXUS_FrontendLockStatus isLocked(void) = 0; /* requires implementation by sub class to determine lock status */
    virtual void unTune(void) = 0; /* requirest implementation by sub class to untune */

    virtual eRet scan(void * id, CTunerScanData * pScanData, CTunerScanCallback callback, bwidget_engine_t widgetEngine, void * context);
    virtual void scanDone(CTunerScanNotificationData * pNotifyData);

    virtual void dump(void);

    void setModelConfig(CModelConfig * pModelConfig) { _pModelConfig = pModelConfig; };
    CModelConfig * getModelConfig(void) { return _pModelConfig; };
    eRet getSoftDecisions(NEXUS_FrontendSoftDecision *pSoftDecisions, int length);
   
    B_EventHandle getLockEvent(void) { return _lockEvent; };
    B_EventHandle getWaitEvent(void) { return _waitEvent; };
    void setLockState(bool locked);
    bool getLockState(void) { return _locked; };

    NEXUS_FrontendHandle getFrontend() { return _frontend; };
    NEXUS_FrontendConnectorHandle getFrontendConnector() { return NEXUS_Frontend_GetConnector(_frontend); };

    /* replace CSubject::notifyObservers() with one specifically for scan that will
       synchronize with the bwin main loop (since scan runs in a thread).  note that
       if notifyObservers() is called with the 2nd parameter as a (void *), then
       it will be calling the CSubject::notifyObservers() version. */
    eRet notifyObservers(eNotification id, CTunerScanNotificationData * data);

    void addEvent(CTunerEvent * pEvent);
    CTunerEvent * getEvent(void);
    void flushEvents();
    CTunerEvent * removeEvent(void);
    bool isAcquireInProgress(void) { return _acquireInProgress; };

protected:
    bwidget_engine_t getWidgetEngine(void) { return _widgetEngine; };
    void trigger(CTunerEvent * pTunerEvent);

protected:
    CModelConfig               * _pModelConfig;
    NEXUS_FrontendCapabilities   _capabilities;
    NEXUS_FrontendHandle         _frontend;
    B_EventHandle                _waitEvent;
    B_EventHandle                _lockEvent;
    B_SchedulerEventId           _lockEventId;
    bool                         _locked;
    bool                         _acquireInProgress;
    MList <CTunerEvent>          _eventList;
    B_MutexHandle                _eventMutex;
    bwidget_engine_t             _widgetEngine;

    B_ThreadHandle               _scanThread_handle;
    MString                      _scanThread_name;
    void                       * _scanThread_id;
    CTunerScanCallback           _scanThread_callback;
    void                       * _scanThread_context;
};


#endif /* NEXUS_HAS_FRONTEND */
#endif /* TUNER_H__ */
