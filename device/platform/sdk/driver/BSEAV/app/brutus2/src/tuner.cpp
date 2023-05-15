/***************************************************************************
* Copyright (c) 2002-2012, Broadcom Corporation
* All Rights Reserved
* Confidential Property of Broadcom Corporation
*
* THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
* AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
* EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: tuner.cpp $
* $brcm_Revision: 24 $
* $brcm_Date: 12/5/12 7:02p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /BSEAV/app/brutus2/src/tuner.cpp $
* 
* 24   12/5/12 7:02p tokushig
* SW7231-1086: fix comparison for duplicate channels during scan with
* append option set.  comparison was only comparing pids and ignoring
* freq, bandwidth, and other frontend specific parameters.
* 
* 23   11/15/12 10:46a tokushig
* SW7429-369: fix build for 97429/97241 and other platforms where
* NEXUS_HAS_FRONTEND is false
* 
* 22   11/13/12 10:43p tokushig
* SW7231-749: improve resiliency of tune/scan.  handle case where fast
* status indicates not locked but also not still searching - we don't
* want to give up until status indicates unlocked or we timeout.
* 
* 21   11/12/12 3:40p tokushig
* SW7231-749: refactoring of fast scan and tuning
* 
* 20   11/7/12 5:53p jrubio
* SW7231-1060: make Locked Function Generic
* 
* 19   10/22/12 4:46p tokushig
* SW7231-749: modify tuner handling to check tuner status after lock
* callback only.  also change callback handling to only set even in
* nexus callback.
* 
* 18   10/10/12 8:36p tokushig
* SW7231-749: remove extra semicolon
* 
* 17   10/9/12 4:06p tokushig
* SW7231-749: add debug to tuner
* 
* 16   10/8/12 4:17p jrubio
* SW7346-1004: add getSoftDecisions functions
* 
* 15   8/27/12 10:06a tokushig
* SW7231-749: move psip acquisition to tuner class so subclasses can all
* inheirit
* 
* 14   7/23/12 4:14p tokushig
* SW7231-749: add ofdm tuning/scanning and gui support (no lua support)
* 
* 13   6/15/12 4:51p tokushig
* SW7231-749: update scan to handle error where intended tuner is
* unavailable.  also better handle display of progress meter so 100% is
* displayed before meter is hidden
* 
* 12   5/25/12 3:12p tokushig
* SW7231-749: remove platform specific file and move to common
* platform.h.  also change appropriate brutus defines to nexus defines
* when available.
* 
* 11   4/20/12 3:34p tokushig
* SW7231-749: fix remote number button mapping issue and adjust size of
* scan menu
* 
* 10   4/19/12 7:35p tokushig
* SW7231-749: fix problem with scan thread where notifications were not
* synchronized with the bwidget main loop.
* 
* 9   3/27/12 10:30a tokushig
* SW7405-5581: add vsb tuning and channel scan
* 
* 8   3/14/12 12:40p jrubio
* SW7346-644: add tuner sat
* 
* 7   2/27/12 4:17p tokushig
* SW7405-5581: break out qam tuner from tuner files
* 
* 
*********************************************************************************/

#if NEXUS_HAS_FRONTEND 

#include "tuner.h"
#include "ts_psi.h"
#include "ts_pat.h"
#include "tspsimgr2.h"
#include "bwidgets.h"
#include "channel.h"

BDBG_MODULE(brutus_tuner);


/* debug function to print out results of channel scan */
static void printinfo(CHANNEL_INFO_T * info) 
{
	int i,j;

	BDBG_MSG((
		"\n"
        "CHANNEL_INFO_T\n"
        "==============\n"
		"version %d\n"
		"transport_stream_id %d\n"
		"sectionBitmap %d\n"
		"num_programs %d\n\n",
		info->version,
		info->transport_stream_id,
		info->sectionBitmap,
		info->num_programs));
	for (i=0;i<info->num_programs;i++) 
    {
        char str[255];
        char * pStr = NULL;

		BDBG_MSG(("program_number %d", info->program_info[i].program_number));
		BDBG_MSG(("  version %d", info->program_info[i].version));
		BDBG_MSG(("  pcr pid   : 0x%x", info->program_info[i].pcr_pid));
			
			
			
        str[0] = 0;
        pStr = str;
		for (j=0;j<info->program_info[i].num_video_pids;j++)
        {
			sprintf(pStr,
                " 0x%x/%d",
                info->program_info[i].video_pids[j].pid,
                info->program_info[i].video_pids[j].streamType);
            pStr = str + strlen(str);
        }
        BDBG_MSG(("  video_pids:%s", str));

        str[0] = 0;
        pStr = str;
		for (j=0;j<info->program_info[i].num_audio_pids;j++)
        {
			sprintf(pStr,
                " 0x%x/%d",
                info->program_info[i].audio_pids[j].pid,
                info->program_info[i].audio_pids[j].streamType);
            pStr = str + strlen(str);
        }
		BDBG_MSG(("  audio_pids:%s", str));

        str[0] = 0;
        pStr = str;
		for (j=0;j<info->program_info[i].num_other_pids;j++)
        {
			sprintf(pStr, 
                " 0x%x/%d",
                info->program_info[i].other_pids[j].pid,
                info->program_info[i].other_pids[j].streamType);
            pStr = str + strlen(str);
        }
		BDBG_MSG(("  other_pids:%s", str));
	}
}


/* bwin io callback that is executed when tuner io is triggered */
void bwinTunerCallback(bwidget_engine_t engine, bwin_io_handle handle)
{
    eRet          ret      = eRet_Ok;
    CTuner      * pTuner   = (CTuner *)handle;
    CTunerEvent * pEvent   = NULL;

    BDBG_ASSERT(NULL != pTuner);

    BDBG_MSG(("in bwinTunerCallback()"));

    /* handle all queued Tuner events */
    while (NULL != (pEvent = pTuner->getEvent()))
    {
        BDBG_WRN(("Notify Observers for Tuner event code: %#x", pEvent->getId()));

        ret = pTuner->CSubject::notifyObservers(pEvent->getId(), (void *)pEvent->getDataPtr());
        CHECK_ERROR_GOTO("error notifying observers", ret, error);

        /* delete Tuner event and any associated data */
        pTuner->removeEvent();
        delete pEvent;
    }

error:
    return;
}

#if NEXUS_HAS_FRONTEND
static void tunerLockHandler(void * context)
{
    CTuner                   * pTuner    = (CTuner *)context;
    NEXUS_FrontendLockStatus   lockState = NEXUS_FrontendLockStatus_eUnknown;
    NEXUS_Error                nerror    = NEXUS_SUCCESS;

    lockState = pTuner->isLocked();
    BDBG_MSG(("Tuner %s lock handler lockState:%d", pTuner->getName(),lockState));

    pTuner->setLockState(NEXUS_FrontendLockStatus_eLocked == lockState);
    
    if (NEXUS_FrontendLockStatus_eLocked == lockState)
    {
        /* set wait event if locked */
        B_Event_Set(pTuner->getWaitEvent());
    }
    else
    if ((false == pTuner->isAcquireInProgress()) && (NEXUS_FrontendLockStatus_eUnlocked != lockState))
    {
        /* set wait event if tuner is done searching for lock */
        B_Event_Set(pTuner->getWaitEvent());
    }
}
#endif


CTuner::CTuner(const char * name, const uint16_t number, eBoardResource resource, CConfiguration * pCfg) :
    CResource(name, number, resource, pCfg),
    _scanThread_handle(NULL),
    _scanThread_id(NULL),
    _scanThread_callback(NULL),
    _scanThread_context(NULL),
    _locked(false),
    _acquireInProgress(false)
{
    eRet ret = eRet_Ok;

    _lockEvent = B_Event_Create(NULL);
    CHECK_PTR_GOTO("tuner lock event create failed", _lockEvent, ret, eRet_ExternalError, error);

    _waitEvent = B_Event_Create(NULL);
    CHECK_PTR_GOTO("tuner wait event create failed!", _waitEvent, ret, eRet_ExternalError, error);

    _lockEventId = B_Scheduler_RegisterEvent(gScheduler, gLock, _lockEvent, tunerLockHandler, this);
    CHECK_PTR_GOTO("tuner lock event registration failed!", _lockEventId, ret, eRet_ExternalError, error);

    _eventMutex = B_Mutex_Create(NULL);
    CHECK_PTR_GOTO("unable to create mutex", _eventMutex, ret, eRet_ExternalError, error);

error:
    BDBG_ASSERT(eRet_Ok == ret);
}

CTuner::~CTuner()
{
    if (NULL != _eventMutex)
    {
        B_Mutex_Destroy(_eventMutex);
        _eventMutex = NULL;
    }

    if (NULL != _lockEventId)
    {
        B_Scheduler_UnregisterEvent(gScheduler, _lockEventId);
        _lockEventId = NULL;
    }

    if (NULL != _waitEvent)
    {
        B_Event_Destroy(_waitEvent);
        _waitEvent = NULL;
    }

    if (NULL != _lockEvent)
    {
        B_Event_Destroy(_lockEvent);
        _lockEvent = NULL;
    }
}

eRet CTuner::initialize()
{
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_FrontendCapabilities  capabilities;
    eRet                        ret             = eRet_Ok;

    BDBG_ASSERT(NEXUS_MAX_FRONTENDS > _number);
    BKNI_Memset(&_capabilities, 0, sizeof(NEXUS_FrontendCapabilities));

    NEXUS_Platform_GetConfiguration(&platformConfig);
    if (platformConfig.frontend[_number])
    {
        /* save tuner capabilities - retrieve from nexus */
        NEXUS_Frontend_GetCapabilities(platformConfig.frontend[_number], &_capabilities);
        _frontend = platformConfig.frontend[_number];
    }

    return ret;
}
    
static void scanThread(void * pParam)
{
    CTuner * pTuner = (CTuner *)pParam;
    BDBG_ASSERT(NULL != pParam);

    pTuner->doScan();
}

/* Scans the given list of frequencies for valid QAM channels.  Newly found channels are returned
   using the given callback.  The caller is responsible for proper deletion of the found
   channel objects when appropriate.
 */
eRet CTuner::scan(void * id, 
                  CTunerScanData     * pScanData, 
                  CTunerScanCallback   callback, 
                  bwidget_engine_t     widgetEngine, 
                  void               * context)
{
    eRet ret = eRet_Ok;

    BDBG_ASSERT(NULL != id);
    BDBG_ASSERT(NULL != pScanData);
    BDBG_ASSERT(NULL != callback);
    BDBG_ASSERT(NULL != widgetEngine);
    BDBG_ASSERT(NULL != _pModelConfig);     /* tuner must have access to model config prior to scan */

    _widgetEngine = widgetEngine;

    /* save scan data thread will need */
    _scanThread_id           = id;
    _scanThread_callback     = callback;
    _scanThread_context      = context;
    saveScanData(pScanData);

    _scanThread_handle       = B_Thread_Create(_scanThread_name, scanThread, this, NULL);
    CHECK_PTR_GOTO("thread creation failed!", _scanThread_handle, ret, eRet_ExternalError, error);

    bwidget_add_io_handle(getWidgetEngine(), (bwin_io_handle)this, bwinTunerCallback);
error:
    return ret;
}

void CTuner::scanDone(CTunerScanNotificationData * pNotifyData)
{
    CTunerEvent * pEvent = getEvent();

    /* scan thread is done, but all notifications might not have been
       sent yet (might still be waiting for bwin main loop
       synchronization).  so we'll wait here until all queued
       notifications have been sent. should be fast as there typically
       are only 1 or 2 waiting. */
    flushEvents();

    /* notify scan is stopped and wait until complete */
    notifyObservers(eNotify_ScanStopped, pNotifyData);
    flushEvents();

    bwidget_remove_io_handle(getWidgetEngine(), (bwin_io_handle)this);
    _widgetEngine = NULL;
}

void CTuner::dump()
{
    BDBG_WRN(("<%d>%s:%d %s%s%s%s%s", 
              _type, 
              _name.s(), 
              _number,
              _capabilities.analog ? "(analog)" : "",
              _capabilities.qam ? "(qam)" : "",
              _capabilities.ofdm ? "(ofdm)" : "",
              _capabilities.satellite ? "(sds)" : "",
              _capabilities.vsb ? "(vsb)" : ""));
}


void CTuner::trigger(CTunerEvent * pTunerEvent)
{
    BDBG_ASSERT(NULL != pTunerEvent);

    bwidget_trigger_io(getWidgetEngine(), (bwin_io_handle)this);
}

/* since scan runs in a separate thread, we will use this method to
   synchronize with the bwin main loop when notifying observers from
   that thread. */
eRet CTuner::notifyObservers(eNotification id, CTunerScanNotificationData * data)
{
    eRet ret = eRet_Ok;

    /* create tuner event and give it playback info */
    /* note that we are not setting a wait notification so we will not wait for this command to complete */
    CTunerDataEvent <CTunerScanNotificationData> * pTunerEvent = 
        new CTunerDataEvent <CTunerScanNotificationData>(id, data);
    CHECK_PTR_GOTO("Unable to malloc CTunerDataEvent", pTunerEvent, ret, eRet_OutOfMemory, error);

    /* save Tuner event to queue - this event will be serviced when we get the bwin io callback:
       bwinTunerCallback() */
    addEvent(pTunerEvent);

    /* trigger bwin io event here */
    trigger(pTunerEvent);

error:
    return ret;
}

void CTuner::addEvent(CTunerEvent * pEvent)
{
    CScopedMutex scopedMutex(_eventMutex);

    _eventList.add(pEvent);
}

CTunerEvent * CTuner::getEvent()
{
    CTunerEvent    * pTunerEvent                 = NULL;
    CScopedMutex     scopedMutex(_eventMutex);

    if (0 < _eventList.total())
    {
        pTunerEvent = (CTunerEvent *)_eventList.first();
    }

    return pTunerEvent;
}

void CTuner::flushEvents()
{
    /* wait until all events have been serviced */
    while (NULL != getEvent())
    {
        B_Thread_Sleep(100);
    }
}

CTunerEvent * CTuner::removeEvent()
{
    CTunerEvent    * pTunerEvent                 = NULL;
    CScopedMutex     scopedMutex(_eventMutex);

    if (0 < _eventList.total())
    {
        pTunerEvent = (CTunerEvent *)_eventList.first();
        _eventList.remove();
    }

    return pTunerEvent;
}


void CTuner::setLockState(bool locked) 
{ 
    BDBG_MSG(("tuner locked:%d", locked));
    _locked = locked; 
}

/* Get Soft Decisions to create Constellation */
eRet CTuner::getSoftDecisions(NEXUS_FrontendSoftDecision *pSoftDecisions, int length)
{
   eRet ret = eRet_Ok;
   NEXUS_Error rc;

   if (NULL == pSoftDecisions)
   {
      ret = eRet_InvalidParameter;
      goto error;
   }

   BDBG_MSG(("Reading %d soft decisions", length));

   rc = NEXUS_Frontend_GetSoftDecisions(getFrontend(), pSoftDecisions, length);
   if (rc != NEXUS_SUCCESS)
      ret = eRet_ExternalError;

error:
   return ret;
}

#endif /* NEXUS_HAS_FRONTEND */
