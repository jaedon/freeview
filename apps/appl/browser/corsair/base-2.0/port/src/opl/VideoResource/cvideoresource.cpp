
#include <map>
#include <hlib.h>
#include <apk.h>
#include "cvideoresource.h"
#include "oplvideohandle.h"
#include "oplvodhandle.h"
#include "oplbroadcast.h"

CVideoResource* CVideoResource::create(const int& aVideoResourceId)
{
	CVideoResource* ret = new CVideoResource(aVideoResourceId);
	ret->initialize(aVideoResourceId);
	return ret;
}

CVideoResource::CVideoResource(const int& aVideoResourceId)
  : mVideoResourceId(aVideoResourceId),
    mVideoHandle(NULL),
    mMain(false)
{
}

CVideoResource::~CVideoResource()
{
	delete mVideoBroadcast;
}

void CVideoResource::initialize(const int& aVideoResourceId)
{
	mVideoBroadcast = new OplVideoBroadcast(this, aVideoResourceId);
	mVideoBroadcast->setLiveEventListener(CVideoResource::CbLive);
	mVideoBroadcast->setTsrEventListener(CVideoResource::CbTsr);
	mVideoBroadcast->setCasEventListener(CVideoResource::CbCas);
	mVideoBroadcast->setRecEventListener(CVideoResource::CbRec);
	mVideoBroadcast->setMailEventListener(CVideoResource::CbMail);
	mVideoBroadcast->setBmlEventListener(CVideoResource::CbBml);
	mVideoBroadcast->setMhegEventListener(CVideoResource::CbMheg);
}

int CVideoResource::getVideoResourceId()
{
	return mVideoResourceId;
}

bool CVideoResource::isMain()
{
	return mMain;
}

bool CVideoResource::setMain(bool aMain)
{
	bool changed(false);
	if (mMain != aMain)
	{
		mMain = aMain;
		changed = true;
	}
	return changed;
}

bool CVideoResource::isAllocated()
{
	return mVideoHandle != NULL;
}

OplVideoHandle* CVideoResource::getVideoHandle()
{
	return mVideoHandle;
}

void CVideoResource::setVideoHandle(OplVideoHandle* aVideoHandle)
{
	HxLOG_Assert(mVideoHandle == NULL);
	if (aVideoHandle->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeVod)
	{
		mVideoBroadcast->stop();
	}
	HxLOG_Critical("[VRM] VRM.RESOURCE[%d] 0x%08X ASSIGNED\n", mVideoResourceId, aVideoHandle);
	mVideoHandle = aVideoHandle;
}

OplVideoHandle* CVideoResource::releaseVideoHandle(void)
{
	HxLOG_Assert(mVideoHandle != NULL);
	HxLOG_Critical("[VRM] VRM.RESOURCE[%d] 0x%08X RELEASED\n", mVideoResourceId, mVideoHandle);
	OplVideoHandle* ret = mVideoHandle;
	mVideoHandle = NULL;
	return ret;
}

bool CVideoResource::isAllocatedHandleIdentical(OplVideoHandle* aVideoHandle)
{
	return mVideoHandle == aVideoHandle;
}

bool CVideoResource::releaseVideoHandleIfIdentical(OplVideoHandle* aVideoHandle)
{
	if (mVideoHandle == aVideoHandle)
	{
		HxLOG_Critical("[VRM] VRM.RESOURCE[%d] 0x%08X RELEASED\n", mVideoResourceId, aVideoHandle);
		mVideoHandle = NULL;
		return true;
	}
	return false;
}

void CVideoResource::setUsing(bool aUsing)
{
	mResourceUsing = aUsing;
}

bool CVideoResource::isBeingUsed()
{
	return mResourceUsing;
}

OplVideoBroadcast* CVideoResource::getVideoBroadcastResource()
{
	return mVideoBroadcast;
}

void CVideoResource::addVideoBroadcastListener(IVideoBroadcastListener* aListener)
{
	HxLOG_Critical("[VRM] VRM.RESOURCE addVideoBroadcastListener video(0x%08X), 0x%08X\n", mVideoHandle, aListener);
	mVideoBroadcastListeners.insert(aListener);
}

void CVideoResource::removeVideoBroadcastListener(IVideoBroadcastListener* aListener)
{
	HxLOG_Critical("[VRM] VRM.RESOURCE removeVideoBroadcastListener video(0x%08X), 0x%08X\n", mVideoHandle, aListener);
	mVideoBroadcastListeners.erase(aListener);
}

void CVideoResource::CbLive(void *aSelf, eOplBroadcastEventType aEventType)
{
	CVideoResource* self = reinterpret_cast<CVideoResource*>(aSelf);
	self->deliverVideoBroadcastEvent(IVideoBroadcastListener::EEventLive, aEventType);
}

void CVideoResource::CbTsr(void *aSelf, eOplBroadcastEventType aEventType)
{
	CVideoResource* self = reinterpret_cast<CVideoResource*>(aSelf);
	self->deliverVideoBroadcastEvent(IVideoBroadcastListener::EEventTsr, aEventType);
}

void CVideoResource::CbCas(void *aSelf, eOplBroadcastEventType aEventType)
{
	CVideoResource* self = reinterpret_cast<CVideoResource*>(aSelf);
	self->deliverVideoBroadcastEvent(IVideoBroadcastListener::EEventCas, aEventType);
}

void CVideoResource::CbRec(void *aSelf, eOplBroadcastEventType aEventType)
{
	CVideoResource* self = reinterpret_cast<CVideoResource*>(aSelf);
	self->deliverVideoBroadcastEvent(IVideoBroadcastListener::EEventRec, aEventType);
}

void CVideoResource::CbMail(void *aSelf, eOplBroadcastEventType aEventType)
{
	CVideoResource* self = reinterpret_cast<CVideoResource*>(aSelf);
	self->deliverVideoBroadcastEvent(IVideoBroadcastListener::EEventMail, aEventType);
}

void CVideoResource::CbBml(void *aSelf, eOplBroadcastEventType aEventType)
{
	CVideoResource* self = reinterpret_cast<CVideoResource*>(aSelf);
	self->deliverVideoBroadcastEvent(IVideoBroadcastListener::EEventBml, aEventType);
}

void CVideoResource::CbMheg(void *aSelf, eOplBroadcastEventType aEventType)
{
	CVideoResource* self = reinterpret_cast<CVideoResource*>(aSelf);
	self->deliverVideoBroadcastEvent(IVideoBroadcastListener::EEventMheg, aEventType);
}

void CVideoResource::deliverVideoBroadcastEvent(
		const IVideoBroadcastListener::TEventType& aEventType,
		const eOplBroadcastEventType& aOplEventType)
{
	bool deliverEvent = false;
	if (mVideoHandle)
	{
		if (aEventType == IVideoBroadcastListener::EEventCas) {
			deliverEvent = true;
		}
		else if (mVideoHandle->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeBroadcast)
		{
			deliverEvent = true;
		}
		else if (mVideoHandle->getVideoHandleType() == OplVideoHandle::EVideoHandleTypeVod)
		{
			OplVodHandle* vodHandle = reinterpret_cast<OplVodHandle*>(mVideoHandle);
			if (!vodHandle->isPlaying() && vodHandle->getPlayState() == OPLVOD_PLAY_STATE_STOPPED)
			{
				//deliverEvent = true;
			}
		}
		else if (aEventType == IVideoBroadcastListener::EEventMheg) {
			deliverEvent = true;
		}
	} 
	else {
		if( aEventType == IVideoBroadcastListener::EEventMheg )
		{
			deliverEvent = true;
		}
	}

	if (deliverEvent)
	{
		std::set<IVideoBroadcastListener*>::iterator i(mVideoBroadcastListeners.begin());
		while (i != mVideoBroadcastListeners.end()) {
			IVideoBroadcastListener* listener = (*i++);
			HxLOG_Debug("[VRM] VRM.RESOURCE DELIVER VB EVENT TO video(0x%08X), listenr(0x%08X), aEventType( %d ) aOplEventType( %d )\n", mVideoHandle, listener, aEventType, aOplEventType);
			listener->handleVideoBroadcastResourceEvent(aEventType, aOplEventType);
		}
	}
}
