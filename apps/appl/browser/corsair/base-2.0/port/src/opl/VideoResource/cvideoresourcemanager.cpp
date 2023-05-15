
#include <algorithm>
#include <iostream>

#include <apk.h>
#include <hapi.h>
#include "cvideoresourcemanager.h"
#include "cvideoresource.h"
#include "oplvideohandle.h"
#include "oplvideobroadcasthandle.h"

#undef printf
static CVideoResourceManager* sVideoResouceManager = NULL;

using namespace std;

static const char* getEventTypeStr(const CVideoResourceManager::TEventType& aEventType) {
	switch (aEventType)
	{
	case CVideoResourceManager::EEventTypeOnVideoHandleCreate:
		return "create";
	case CVideoResourceManager::EEventTypeOnVideoHandleDestroyed:
		return "destroyed";
	case CVideoResourceManager::EEventTypeOnVideoHandleShown:
		return "shown";
	case CVideoResourceManager::EEventTypeOnVideoHandleHidden:
		return "hidden";
	case CVideoResourceManager::EEventTypeOnVideoResourceRequired:
		return "required";
	case CVideoResourceManager::EEventTypeOnVideoResourceNotRequired:
		return "not_required";
	default:
		return "unknown";
	}
}

static const char* getVideoResourceDeAllocTypeStr(const CVideoResourceManager::TVideoResourceDeAllocType& aVideoResourceDeAllocType) {
	switch (aVideoResourceDeAllocType)
	{
	case CVideoResourceManager::EVideoResourceDeAllocTypeNone:
		return "None";
	case CVideoResourceManager::EVideoResourceDeAllocTypeDeAllocSelf:
	    return "DeAllocSelf";
	case CVideoResourceManager::EVideoResourceDeAllocTypeRecoverPrevBroadcast:
	    return "RecoverPrevBroadcast";
	case CVideoResourceManager::EVideoResourceDeAllocTypeRecoverPrevCreated:
		return "RecoverPrevCreated";
	default:
		return "unknown";
	}
}

static const char* getOwnerTypeStr(const OplVideoHandle::THandleOwnerType& aOwnerType) {
	switch (aOwnerType)
	{
	case OplVideoHandle::EHandleOwnerTypeSystem:
		return "system";
	case OplVideoHandle::EHandleOwnerTypeUserOipf:
		return "user_oipf";
	case OplVideoHandle::EHandleOwnerTypeUserHbbtv:
		return "user_hbbtv";
	default:
		return "unknown";
	}
}

static const char* getVideoHandleTypeStr(const OplVideoHandle::TVideoHandleType& aVideoHandleType) {
	switch (aVideoHandleType)
	{
	case OplVideoHandle::EVideoHandleTypeBroadcast:
		return "vb";
	case OplVideoHandle::EVideoHandleTypeVod:
		return "vod";
	default:
		return "unknown";
	}
}

const CVideoResourceManager::TAllocPolicy sAllocPolicy[] = {
	CVideoResourceManager::TAllocPolicy(
		CVideoResourceManager::EEventTypeOnVideoHandleShown,
		OplVideoHandle::EHandleOwnerTypeUserOipf | OplVideoHandle::EHandleOwnerTypeUserHbbtv,
		OplVideoHandle::EVideoHandleTypeBroadcast,
		CVideoResourceManager::EVideoResourceAllocTypeMain),

	CVideoResourceManager::TAllocPolicy(
		CVideoResourceManager::EEventTypeOnVideoResourceRequired,
		OplVideoHandle::EHandleOwnerTypeUserOipf | OplVideoHandle::EHandleOwnerTypeUserHbbtv,
		OplVideoHandle::EVideoHandleTypeAll,
		CVideoResourceManager::EVideoResourceAllocTypeMain),

	CVideoResourceManager::TAllocPolicy(
		CVideoResourceManager::EEventTypeOnVideoHandleCreate,
		OplVideoHandle::EHandleOwnerTypeSystem,
		OplVideoHandle::EVideoHandleTypeAll,
		CVideoResourceManager::EVideoResourceAllocTypeMainIfAvailable),

	CVideoResourceManager::TAllocPolicy(
		CVideoResourceManager::EEventTypeOnVideoHandleCreate,
		OplVideoHandle::EHandleOwnerTypeUserHbbtv,
		OplVideoHandle::EVideoHandleTypeBroadcast,
		CVideoResourceManager::EVideoResourceAllocTypeMain),

	CVideoResourceManager::TAllocPolicy(
		CVideoResourceManager::EEventTypeOnVideoResourceRequired,
		OplVideoHandle::EHandleOwnerTypeSystem,
		OplVideoHandle::EVideoHandleTypeAll,
		CVideoResourceManager::EVideoResourceAllocTypeMainIfAvailable),

	CVideoResourceManager::TAllocPolicy(
		CVideoResourceManager::EEventTypeOnVideoResourceRequired,
		OplVideoHandle::EHandleOwnerTypeSystem,
		OplVideoHandle::EVideoHandleTypeAll,
		CVideoResourceManager::EVideoResourceAllocTypeSub),

	CVideoResourceManager::TAllocPolicy(
		CVideoResourceManager::EEventTypeOnVideoResourceRequired,
		OplVideoHandle::EHandleOwnerTypeSystem,
		OplVideoHandle::EVideoHandleTypeVod,
		CVideoResourceManager::EVideoResourceAllocTypeMain),
};

const CVideoResourceManager::TDeAllocPolicy sDeAllocPolicy[] = {
/*
	CVideoResourceManager::TDeAllocPolicy(
		CVideoResourceManager::EEventTypeOnVideoHandleHidden,
		OplVideoHandle::EHandleOwnerTypeAll,
		OplVideoHandle::EVideoHandleTypeAll,
		CVideoResourceManager::EVisibilityConditionDontCare,
		OplVideoHandle::EResourceNecessityUnknown,
		CVideoResourceManager::EVideoResourceDeAllocTypeRecoverPrevBroadcast),
*/
	CVideoResourceManager::TDeAllocPolicy(
		CVideoResourceManager::EEventTypeOnVideoResourceNotRequired,
		OplVideoHandle::EHandleOwnerTypeSystem,
		OplVideoHandle::EVideoHandleTypeVod,
		CVideoResourceManager::EVisibilityConditionDontCare,
		OplVideoHandle::EResourceNecessityUnknown,
		CVideoResourceManager::EVideoResourceDeAllocTypeDeAllocSelf),

	CVideoResourceManager::TDeAllocPolicy(
		CVideoResourceManager::EEventTypeOnVideoHandleHidden,
		OplVideoHandle::EHandleOwnerTypeUserOipf | OplVideoHandle::EHandleOwnerTypeUserHbbtv,
		OplVideoHandle::EVideoHandleTypeVod,
		CVideoResourceManager::EVisibilityConditionDontCare,
		OplVideoHandle::EResourceNecessityNotRequired,
		CVideoResourceManager::EVideoResourceDeAllocTypeRecoverPrevBroadcast),

	CVideoResourceManager::TDeAllocPolicy(
		CVideoResourceManager::EEventTypeOnVideoHandleHidden,
		OplVideoHandle::EHandleOwnerTypeUserOipf | OplVideoHandle::EHandleOwnerTypeUserHbbtv,
		OplVideoHandle::EVideoHandleTypeBroadcast,
		CVideoResourceManager::EVisibilityConditionDontCare,
		OplVideoHandle::EResourceNecessityRequired,
		CVideoResourceManager::EVideoResourceDeAllocTypeRecoverPrevBroadcast),

	CVideoResourceManager::TDeAllocPolicy(
		CVideoResourceManager::EEventTypeOnVideoHandleDestroyed,
		OplVideoHandle::EHandleOwnerTypeUserOipf | OplVideoHandle::EHandleOwnerTypeUserHbbtv,
		OplVideoHandle::EVideoHandleTypeAll,
		CVideoResourceManager::EVisibilityConditionDontCare,
		OplVideoHandle::EResourceNecessityUnknown,
		CVideoResourceManager::EVideoResourceDeAllocTypeRecoverPrevBroadcast),

	CVideoResourceManager::TDeAllocPolicy(
		CVideoResourceManager::EEventTypeOnVideoHandleDestroyed,
		OplVideoHandle::EHandleOwnerTypeAll,
		OplVideoHandle::EVideoHandleTypeAll,
		CVideoResourceManager::EVisibilityConditionDontCare,
		OplVideoHandle::EResourceNecessityUnknown,
		CVideoResourceManager::EVideoResourceDeAllocTypeDeAllocSelf),
};

CVideoResourceManager* CVideoResourceManager::getInstance()
{
	if (sVideoResouceManager == NULL)
	{
		sVideoResouceManager = new CVideoResourceManager();
	}
	return sVideoResouceManager;
}

bool CVideoResourceManager::startLiveTV()
{
    return getInstance()->controllLiveTV(true);
}	

bool CVideoResourceManager::stopLiveTV()
{
    return getInstance()->controllLiveTV(false);
}	

bool CVideoResourceManager::recoverLiveTVIfPossible()
{
    return getInstance()->_recoverLiveTVIfPossible();
}	

bool CVideoResourceManager::isPlayingMedia()
{
    return getInstance()->_isPlayingMedia();
}
	
CVideoResourceManager::CVideoResourceManager()
	: mMainVBHandle(NULL)
	, m_live_recovery_enabled(true)
{
	//일단 동작 확인을 위해서 view_0 만 사용함
#if 0
	HERROR hErr;
	HINT32 i;
	HUINT32 ulViewNum;

	hErr = APK_MEDIA_PLAY_GetViewNumber(&ulViewNum);
	if (ERR_OK == hErr)
	{
		for (i=0 ; i<ulViewNum ; i++)
		{
			mVideoResouces.push_back(CVideoResource::create( i /*resourceId*/ ));
		}
	}
	else
#endif
	{
		mVideoResouces.push_back(CVideoResource::create( 0 /*default resourceId*/));
	}

	updateMainView();
}

CVideoResourceManager::~CVideoResourceManager()
{
	// TODO: Deallocation of CVideoResource
}

void CVideoResourceManager::registerVideoHandle(OplVideoHandle* aVideoHandle)
{
	if (aVideoHandle->getHandleOwnerType() == OplVideoHandle::EHandleOwnerTypeSystem &&
		aVideoHandle->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeBroadcast)
	{
		mMainVBHandle = reinterpret_cast<OplVideoBroadcastHandle*>(aVideoHandle);
	}
	mVideoHandleList.push_back(aVideoHandle);
	HxLOG_Critical("[VRM] \n");
	HxLOG_Critical("[VRM] VRM.onCreate 0x%08X:%08d\n", aVideoHandle, findIndex(aVideoHandle));
	processAllocPolicy(EEventTypeOnVideoHandleCreate, aVideoHandle);
}

void CVideoResourceManager::unRegisterVideoHandle(OplVideoHandle* aVideoHandle)
{
	HxLOG_Critical("[VRM] \n");
	HxLOG_Critical("[VRM] VRM.onDestroy 0x%08X:%08d\n", aVideoHandle, findIndex(aVideoHandle));
	processDeAllocPolicy(EEventTypeOnVideoHandleDestroyed, aVideoHandle);

	mVideoHandleList.remove(aVideoHandle);
	mAllocHistoryMain.remove(aVideoHandle);
	mAllocHistoryPiP.remove(aVideoHandle);
}

bool CVideoResourceManager::isRegistered(OplVideoHandle* aVideoHandle)
{
	HxLOG_Critical("[VRM] VRM.isRegistered 0x%08X\n", aVideoHandle);
	std::list<OplVideoHandle*>::iterator i = std::find(mVideoHandleList.begin(), mVideoHandleList.end(), aVideoHandle);
	return i != mVideoHandleList.end();
}

void CVideoResourceManager::videoHandleShown(OplVideoHandle* aVideoHandle)
{
	HxLOG_Critical("[VRM] \n");
	HxLOG_Critical("[VRM] VRM.onShown 0x%08X:%08d\n", aVideoHandle, findIndex(aVideoHandle));
	processAllocPolicy(EEventTypeOnVideoHandleShown, aVideoHandle);
}

void CVideoResourceManager::videoHandleHidden(OplVideoHandle* aVideoHandle)
{
	HxLOG_Critical("[VRM] \n");
	HxLOG_Critical("[VRM] VRM.onHidden 0x%08X:%08d\n", aVideoHandle, findIndex(aVideoHandle));
	processDeAllocPolicy(EEventTypeOnVideoHandleHidden, aVideoHandle);
}

void CVideoResourceManager::videoResourceRequired(OplVideoHandle* aVideoHandle, const char* aReason)
{
	HxLOG_Critical("[VRM] \n");
	HxLOG_Critical("[VRM] VRM.onRequired 0x%08X:%08d reason=[%s]\n", aVideoHandle, findIndex(aVideoHandle), aReason);
	processAllocPolicy(EEventTypeOnVideoResourceRequired, aVideoHandle);
}

void CVideoResourceManager::videoResourceNotRequired(OplVideoHandle* aVideoHandle, const char* aReason)
{
	HxLOG_Critical("[VRM] \n");
	HxLOG_Critical("[VRM] VRM.onNotRequired 0x%08X:%08d reason=[%s]\n", aVideoHandle, findIndex(aVideoHandle), aReason);
	processDeAllocPolicy(EEventTypeOnVideoResourceNotRequired, aVideoHandle);
}

void CVideoResourceManager::requestAudioPath(OplVideoHandle* aVideoHandle)
{
	HxLOG_Critical("[VRM] \n");
	HxLOG_Critical("[VRM] VRM.requestAudioPath 0x%08X:%08d\n", aVideoHandle, findIndex(aVideoHandle));
	CVideoResource* mainResource = getMain();
	OplVideoHandle* oldVideoHandle = NULL;
	if (mainResource->isAllocated() && mainResource->getVideoHandle()->getHandleOwnerType() == OplVideoHandle::EHandleOwnerTypeUserOipf)
	{
		oldVideoHandle = mainResource->releaseVideoHandle();
	}
	setMainView(aVideoHandle->getAllocatedResource());
	if (oldVideoHandle)
	{
		HxLOG_Critical("[VRM] VRM.requestAudioPath %d from 0x%08X:%08d\n", mainResource->getVideoResourceId(), aVideoHandle, findIndex(aVideoHandle));
		oldVideoHandle->videoResouceDeAllocated(mainResource, aVideoHandle->getVideoHandleType());
	}
}

OplVideoHandle* CVideoResourceManager::getMainVideoHandle()
{
	CVideoResource* mainResource = getMain();
	if (mainResource->isAllocated())
	{
		return mainResource->getVideoHandle();
	}
	return NULL;
}

OplVideoBroadcastHandle* CVideoResourceManager::getMainVideoBroadcastHandle()
{
	CVideoResource* mainResource = getMain();
	if (mainResource->isAllocated() && mainResource->getVideoHandle()->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeBroadcast)
	{
		return reinterpret_cast<OplVideoBroadcastHandle*>(mainResource->getVideoHandle());
	}
	return NULL;
}

OplVodHandle* CVideoResourceManager::getMainVodHandle()
{
	CVideoResource* mainResource = getMain();
	if (mainResource->isAllocated() && mainResource->getVideoHandle()->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeVod)
	{
		return reinterpret_cast<OplVodHandle*>(mainResource->getVideoHandle());
	}
	return NULL;
}

int CVideoResourceManager::findIndex(OplVideoHandle* aVideoHandle)
{
	std::list<OplVideoHandle*>::const_iterator i(mVideoHandleList.begin());
	int count = 0;
	for (;i!=mVideoHandleList.end();i++) {
		if (*i == aVideoHandle) {
			return count;
		}
		count++;
	}
	return -1;
}

/* this function will be called by HbbTV controller. */
bool CVideoResourceManager::controllLiveTV(bool on_off)
{
    /* if HbbTV controller want to turn on Live, VRM shall stop with the system broadcast instance or main oipf.*/
    
    /* if HbbTV controller turned off live(BI application luanched), allocate and deallocate process function will not discover live */
    m_live_recovery_enabled = on_off;
    
    /* to turn on or off live broadcast, we have to use the system vb instance or oipf instance has presenting status */
    OplVideoBroadcastHandle* system_broadcast = getSystemVideoBroadcastHandle();
    if ( system_broadcast != NULL )
    {
        HxLOG_Critical("[VRM] VRM. controllSystemBroadcast 0x%08X:%08d call %s\n", system_broadcast, findIndex(system_broadcast), on_off ? "bindToCurrentChannel" : "stop");
        if ( on_off )
        {
            system_broadcast->bindToCurrentChannel();
        }
        else
        {
            if ( system_broadcast->stop() )
            {
                system_broadcast->pauseHbbtvMonitoring();
            }
        }
        return true;
    }
    
    return false;
}

/* this function will be called by HbbTV controller. 
   when VOD instance is stopped, corsair call this function. at the calling time, we have to check the status of each video instance.
   and if possible and meet the condition to recover, we have to call bindToCurrentChannel of the main VB instance.*/
bool CVideoResourceManager::_recoverLiveTVIfPossible(void)
{    
    HxLOG_Critical("[VRM] VRM. %s +\n", __FUNCTION__ );
    
    bool recover = false;
    
    if ( false == m_live_recovery_enabled )  // false means that BI application is running or blocked liveTV recovery
    {
        HxLOG_Critical("[VRM] VRM. recoverLiveTV does not need, maybe Broadcast-Independent Application is running \n");
        return false;
    }

    logVideoHandle(NULL);
        
    /* to turn on or off live broadcast, we have to use the system vb instance or oipf instance has presenting status */
    std::list<OplVideoHandle*>::iterator it;
    if ( mVideoHandleList.size() > 0 )
    {
        /* now we have to put the code to find the case of recovery need */
        
        /* case 1, if there is no hbbtv type object then we have to recover liveTV */
        {
            int count_hbbtv_vod_object = 0;
            int count_hbbtv_vb_object = 0;
            
            for (it = mVideoHandleList.begin(); it != mVideoHandleList.end(); it++) 
            {
                if ( (*it)->getHandleOwnerType() == OplVideoHandle::EHandleOwnerTypeUserHbbtv &&
                    (*it)->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeVod )
                {
                    count_hbbtv_vod_object++;
                }
                
                if ( (*it)->getHandleOwnerType() == OplVideoHandle::EHandleOwnerTypeUserHbbtv &&
                     (*it)->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeBroadcast )
                {
                    count_hbbtv_vb_object++;
                }
            }
            
            if ( 0 == count_hbbtv_vod_object && 0 == count_hbbtv_vb_object )
            {
                recover = true;
                HxLOG_Critical("[VRM] VRM. recoverLiveTV Case 1\n");
                goto RECOVER_LIVETV;
            }
        }
        
        /* case 2, if there is hbbtv type object then we have to recover liveTV */
        {
            
        }
    }
            
RECOVER_LIVETV:
    if( true == recover )
    {
        HxLOG_Critical("[VRM] VRM. recover LiveTV !!!!\n");
        OplVideoBroadcastHandle* system_broadcast = getSystemVideoBroadcastHandle();
        if ( system_broadcast != NULL )
        {
            /* we need to check the system vb object is in TSR and Chase play mode ,
               in a channel has HbbTV Application, if user start TSR then web prism call a function to stop hbbTV.
               at this time, stopped HbbTV application make stop signal, then cosair get the signal to recover live.
               but user already start tsr, so we shall not call bindToCurrentChannel(). */
            bool tsr_enabled = false;
            if ( system_broadcast->isTimeshiftEnabled(&tsr_enabled) && tsr_enabled == false ) 
            {
                HxLOG_Critical("[VRM] VRM. recoverLiveTVIfPossible 0x%08X:%08d\n", system_broadcast, findIndex(system_broadcast) );
                system_broadcast->bindToCurrentChannel();
                return true;
            }
        }
    }
    return false;
}

bool CVideoResourceManager::_isPlayingMedia(void)
{
    OplVideoHandle* main_video = getMainVideoHandle(); 
    if( NULL != main_video )
    {
        if ( main_video->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeVod )
        {
            HxLOG_Critical("[VRM] _isPlayingMedia : main video is VOD!!\n");
            return true;
        }
            
        if ( main_video->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeBroadcast )
        {
            /* in the case of which type is VB, we need to check below condition to know it playing TSR or Chase playing */
            OplVideoBroadcastHandle* broadcast_h = static_cast<OplVideoBroadcastHandle*>(main_video);
            bool tsr_enabled = false;
            if ( broadcast_h->isTimeshiftEnabled(&tsr_enabled) )
            {
                /* if speed is not 0, it's playing some contents by TSR */
                if ( true == tsr_enabled )
                {
                    HxLOG_Critical("[VRM] _isPlayingMedia : main video is VB and playing TSR!!\n");
                    return true;
                }
            }
        }
    }
    return false;   
}

OplVideoBroadcastHandle* CVideoResourceManager::getSystemVideoBroadcastHandle()
{
    std::list<OplVideoHandle*>::iterator it;
    if ( mVideoHandleList.size() > 0 )
    {
    	for (it = mVideoHandleList.begin(); it != mVideoHandleList.end(); it++) 
        {
            if ( (*it)->getHandleOwnerType() == OplVideoHandle::EHandleOwnerTypeSystem && 
                 (*it)->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeBroadcast )
            {
                OplVideoBroadcastHandle* system_broadcast = static_cast<OplVideoBroadcastHandle*>((*it));
                if ( system_broadcast != NULL )
                {
                    return system_broadcast;
                }
            }
        }
    }
	return NULL;
}

void CVideoResourceManager::setMainView(CVideoResource* aVideoResouce)
{
	int resouceId = aVideoResouce->getVideoResourceId();
	HxLOG_Critical("[VRM] VRM.setMainView %d\n", resouceId);
	APK_MEDIA_PLAY_SetMainViewId( resouceId /*ulViewId*/ );
	updateMainView(resouceId);
}

void CVideoResourceManager::updateMainView(int aResouceId)
{
	int resouceId = aResouceId;
	list<CVideoResource*>::iterator i(mVideoResouces.begin());
	while (i != mVideoResouces.end()) {
		CVideoResource* r = (*i++);
		bool mainView = r->getVideoResourceId() == resouceId;
		bool changed = r->setMain(mainView);
		if (changed && r->isAllocated())
		{
			if (mainView)
			{
				r->mVideoHandle->changeAudioPathState(OplVideoHandle::EAudioPathStateAcquired);
			}
			else
			{
				r->mVideoHandle->changeAudioPathState(OplVideoHandle::EAudioPathStateReleased);
			}
		}
	}

}

void CVideoResourceManager::processAllocPolicy(const TEventType& aEventType, OplVideoHandle* aVideoHandle)
{
	logVideoHandle(aVideoHandle);

	HxLOG_Debug("[VRM] VRM.processAllocPolicy 0x%08X:%08d eventType=%s ownerType=%s type=%s\n",
				   aVideoHandle,
				   findIndex(aVideoHandle),
				   getEventTypeStr(aEventType),
				   getOwnerTypeStr(aVideoHandle->getHandleOwnerType()),
				   getVideoHandleTypeStr(aVideoHandle->getVideoHandleType()));
	CVideoResource* allocatedResource(NULL);
	for (unsigned int i=0; i<sizeof(sAllocPolicy)/sizeof(TAllocPolicy); i++)
	{
		TAllocPolicy allocPolicy = sAllocPolicy[i];
		if (allocPolicy.mEventType == aEventType)
		{
		    if (allocPolicy.mVideoHandleOwnerType & aVideoHandle->getHandleOwnerType())
			{
				if (allocPolicy.mVideoHandleType & aVideoHandle->getVideoHandleType())
				{
					switch (allocPolicy.mAllocType)
					{
					case EVideoResourceAllocTypeMain:
						allocatedResource = getMain();
						break;
					case EVideoResourceAllocTypeMainIfAvailable:
						allocatedResource = getMainIfAvailable();
						break;
					case EVideoResourceAllocTypeSub:
						allocatedResource = getSub();
						break;
					default:
						break;
					}
					
					if (allocatedResource && allocatedResource->getVideoHandle() != aVideoHandle)
					{
						allocate(allocatedResource, aVideoHandle);
						break;
					}
				}

			}
		}
	}

	logVideoHandle(aVideoHandle);
}

void CVideoResourceManager::allocate(CVideoResource* aVideoResource, OplVideoHandle* aVideoHandle)
{
	int resourceId = aVideoResource->getVideoResourceId();
	if (aVideoResource->isAllocated())
	{
		OplVideoHandle* oldVideoHandle = aVideoResource->releaseVideoHandle();
		HxLOG_Critical("[VRM] VRM.allocate DEALLOC %d from 0x%08X:%08d\n", resourceId, oldVideoHandle, findIndex(oldVideoHandle));
		oldVideoHandle->videoResouceDeAllocated(aVideoResource, aVideoHandle->getVideoHandleType());

		if (oldVideoHandle->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeBroadcast)
		{
			if (aVideoResource->isMain())
			{
				mAllocHistoryMain.remove(oldVideoHandle);
				mAllocHistoryMain.push_back(oldVideoHandle);
			}
			else
			{
				mAllocHistoryPiP.remove(oldVideoHandle);
				mAllocHistoryPiP.push_back(oldVideoHandle);
			}
		}
	}
	aVideoResource->setVideoHandle(aVideoHandle);
	HxLOG_Critical("[VRM] VRM.allocate ALLOC %d to 0x%08X:%08d\n", resourceId, aVideoHandle, findIndex(aVideoHandle));
	aVideoHandle->videoResouceAllocated(aVideoResource);
}

CVideoResource* CVideoResourceManager::getMain()
{
	CVideoResource* ret(NULL);
	list<CVideoResource*>::iterator i(mVideoResouces.begin());
	while (i != mVideoResouces.end()) {
		CVideoResource* resource = (*i++);
		if (resource->isMain())
		{
			ret = resource;
			break;
		}
	}
	return ret;
}

CVideoResource* CVideoResourceManager::getMainIfAvailable()
{
	CVideoResource* ret(NULL);
	list<CVideoResource*>::iterator i(mVideoResouces.begin());
	while (i != mVideoResouces.end()) {
		CVideoResource* resource = (*i++);
		if (resource->isMain() && !resource->isAllocated())
		{
			ret = resource;
			break;
		}
	}
	return ret;
}

CVideoResource* CVideoResourceManager::getSub()
{
	CVideoResource* ret(NULL);
	list<CVideoResource*>::iterator i(mVideoResouces.begin());
	while (i != mVideoResouces.end()) {
		CVideoResource* resource = (*i++);
		if (!resource->isMain())
		{
			ret = resource;
		}
	}
	return ret;
}

void CVideoResourceManager::processDeAllocPolicy(const TEventType& aEventType, OplVideoHandle* aVideoHandle)
{
	logVideoHandle(aVideoHandle);
	HxLOG_Debug("[VRM] VRM.processDeAllocPolicy 0x%08X:%08d eventType=%s ownerType=%s type=%s\n",
				   aVideoHandle,
				   findIndex(aVideoHandle),
				   getEventTypeStr(aEventType),
				   getOwnerTypeStr(aVideoHandle->getHandleOwnerType()),
				   getVideoHandleTypeStr(aVideoHandle->getVideoHandleType()));
	for (unsigned int i=0; i<sizeof(sDeAllocPolicy)/sizeof(TDeAllocPolicy); i++)
	{
		TDeAllocPolicy deAllocPolicy = sDeAllocPolicy[i];
		if (deAllocPolicy.mEventType == aEventType)
		{
			if (deAllocPolicy.mVideoHandleOwnerType & aVideoHandle->getHandleOwnerType())
			{
				if (deAllocPolicy.mVideoHandleType & aVideoHandle->getVideoHandleType())
				{
					if (deAllocPolicy.mVisibilityCondition == EVisibilityConditionNotVisible && aVideoHandle->isVisible())
					{
						continue;
					}

					if (deAllocPolicy.mVisibilityCondition == EVisibilityConditionVisible && !aVideoHandle->isVisible())
					{
						continue;
					}

					if (deAllocPolicy.mResourceNecessity != OplVideoHandle::EResourceNecessityUnknown && \
                        deAllocPolicy.mResourceNecessity != aVideoHandle->getResourceNecessity())
					{
						continue;
					}

					HxLOG_Critical("[VRM] VRM. Use sDeAllocPolicy[%d] matched type=%s\n", i, getVideoResourceDeAllocTypeStr(deAllocPolicy.mDeAllocType));
					CVideoResource* videoResource = aVideoHandle->getAllocatedResource();
					switch(deAllocPolicy.mDeAllocType)
					{
						case EVideoResourceDeAllocTypeRecoverPrevBroadcast:
						{
						    HxLOG_Critical("[VRM] VRM. LiveRecovery Scenario %s\n", (m_live_recovery_enabled ? "enable" : "disable"));
						    bool recovered = false;
						    deAllocate(aVideoHandle);
#if 1 /* to handle the Live recovery scenarion on Corsair, we wll make a signal when the recovery need */
                            if ( OplVideoHandle::EHandleOwnerTypeUserHbbtv == aVideoHandle->getHandleOwnerType() )
                            	HAPI_BroadcastSignal("signal:onHbbTvVodStopped", 0, "i", 0);
#else						    
						        /* we have to check this deallcation make blocking of the main screen,
						           so if the deallocated instance is not the main resource, we do not need any recovery control */
    							if (videoResource)
    							{
    								if (videoResource->isMain())
    								{
    									OplVideoBroadcastHandle* prevBroadcast(NULL);
    									if ( mAllocHistoryMain.size() && mAllocHistoryMain.back()->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeBroadcast )
    									{
    										prevBroadcast = reinterpret_cast<OplVideoBroadcastHandle*>(mAllocHistoryMain.back());
    										mAllocHistoryMain.pop_back();
    									}

                                        /* check condition to recover live broadcast from HbbTV,
                                           we need to separate what we have to checkby each type of last instance */
                                    if ( aVideoHandle->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeBroadcast ||
                                         aVideoHandle->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeVod )
                                        {
                                            /* if the last instance is broacast object, we have to check belows */
                                            if ( prevBroadcast &&
                                                 prevBroadcast->getResourceNecessity() == OplVideoHandle::EResourceNecessityRequired )
                                            {
                                                recovered = true;
                                            }
#ifdef DEBUG
            								else
                                            {
                                                HxLOG_Critical("[VRM] VRM.	Don't recovery because prevBroadcast 0x%08X:%08d is %s\n", \
                                                    prevBroadcast, findIndex(prevBroadcast), \
                                                    ( prevBroadcast->getResourceNecessity() == OplVideoHandle::EResourceNecessityRequired ? "EResourceNecessityRequired":"not EResourceNecessityRequired") );
                                            }
#endif
                                        }
                                        
                                        if ( recovered )
    									{
                                            HxLOG_Critical("[VRM] VRM. Recover PrevBroadcast 0x%08X:%08d \n", prevBroadcast, findIndex(prevBroadcast));

    										allocate(videoResource, prevBroadcast);
                                            prevBroadcast->bindToCurrentChannel();
    									}
    								}
#ifdef DEBUG
    								else
                                    {
                                        HxLOG_Critical("[VRM] VRM. Don't recovery videoResource of 0x%08X:%08d is not Main\n", aVideoHandle, \
                                                    findIndex(aVideoHandle));
                                    }
#endif
    							}
#ifdef DEBUG
                                else
                                {
                                    HxLOG_Critical("[VRM] VRM. Don't recovery videoResource of 0x%08X:%08d is NULL\n", aVideoHandle, \
                                                findIndex(aVideoHandle));
                                }
#endif
#endif    					    
    					    if (!recovered && !getMain()->isAllocated())
							{
								videoResource = videoResource? videoResource: getMain();
								if ( mMainVBHandle && NULL != videoResource )
								{
									HxLOG_Debug("[VRM] VRM. Recover TO SYSTEM 0x%08X:%08d (Just Allocate)\n", mMainVBHandle, findIndex(mMainVBHandle));
									allocate(videoResource, mMainVBHandle);
							    }
							}
    					    break;
						}
						case EVideoResourceDeAllocTypeDeAllocSelf:
						default:
							deAllocate(aVideoHandle);
							break;
					}
					break;
				}
			}
		}
	}

	logVideoHandle(aVideoHandle);

}

CVideoResource* CVideoResourceManager::deAllocate(OplVideoHandle* aVideoHandle)
{
	CVideoResource* ret(NULL);
	list<CVideoResource*>::iterator i(mVideoResouces.begin());
	HxLOG_Critical("[VRM] VRM.deAllocate type=%s 0x%08X:%08d\n", getVideoHandleTypeStr(aVideoHandle->getVideoHandleType()), \
        aVideoHandle, findIndex(aVideoHandle));
    if ( true == aVideoHandle->isResourceAllocated() )
    {
    	while (i != mVideoResouces.end()) {
    		CVideoResource* resource = (*i++);
    		if (resource->releaseVideoHandleIfIdentical(aVideoHandle))
    		{
    			HxLOG_Critical("[VRM] VRM.deAllocate DEALLOC %d from 0x%08X:%08d\n", resource->getVideoResourceId(), aVideoHandle, findIndex(aVideoHandle));
    			aVideoHandle->videoResouceDeAllocated(resource, OplVideoHandle::EVideoHandleTypeNone);
    			ret = resource;
    		}
    	}

	    mAllocHistoryMain.remove(aVideoHandle);
	    mAllocHistoryPiP.remove(aVideoHandle);
    }
    
	return ret;
}

CVideoResource* CVideoResourceManager::findAllocatedResourceByHandle(OplVideoHandle* aVideoHandle)
{
	CVideoResource* ret(NULL);
	list<CVideoResource*>::iterator i(mVideoResouces.begin());
	while (i != mVideoResouces.end()) {
		CVideoResource* resource = (*i++);
		if (resource->isAllocatedHandleIdentical(aVideoHandle))
		{
			ret = resource;
		}
	}
	return ret;
}

void CVideoResourceManager::logVideoHandle(OplVideoHandle* aVideoHandle)
{
    HxLOG_Critical("[VRM] logVideoHandle + aVideoHandle %p\n", aVideoHandle);
	if( mVideoHandleList.size() > 0 )
	{
		OplVideoHandle* main_video = NULL;
		CVideoResource* main_resource = this->getMain();
		if( main_resource ) main_video = main_resource->getVideoHandle();

		int j = 0;
		list<OplVideoHandle*>::iterator i(mVideoHandleList.begin());
		while (i != mVideoHandleList.end()) {
			OplVideoHandle *v = (*i++);
			bool is_main = false;
			if( main_video == v ) is_main = true;
			HxLOG_Critical("+++++ [VRM] input_video(%p), video_list(index=%d, handle_type=%s, owner_type=%s, necessity=%d, handle=%p, is_main=%d) \n", \
				v, j++,  getVideoHandleTypeStr(v->getVideoHandleType()), getOwnerTypeStr(v->getHandleOwnerType()), v->getResourceNecessity(), v, is_main);
		}
	}
	HxLOG_Critical("[VRM] logVideoHandle -\n");
}

bool CVideoResourceManager::hasHandle(OplVideoHandle* aVideoHandle,
		const OplVideoHandle::TVideoHandleType type, const OplVideoHandle::THandleOwnerType owner)
{
	if( mVideoHandleList.size() > 0 )
	{
		list<OplVideoHandle*>::iterator i(mVideoHandleList.begin());
		while (i != mVideoHandleList.end()) {
			OplVideoHandle *v = (*i++);
			if ( (v->getVideoHandleType() == type) &&
				   	(v->getHandleOwnerType() == owner) )
				return true;
		}
	}
	return false;
}
