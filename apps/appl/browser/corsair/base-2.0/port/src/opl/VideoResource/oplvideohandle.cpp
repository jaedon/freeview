#include <hlib.h>
#include <apk.h>
#include "oplvideohandle.h"
#include "cvideoresourcemanager.h"
#include "cvideoresource.h"

static const char* KStrReasonUnKnown = "unknown";

OplVideoHandle::OplVideoHandle(
		const THandleOwnerType& aHandleOwnerType,
		const TVideoHandleType& aVideoHandleType)
  : mHandleOwnerType(aHandleOwnerType),
    mVideoHandleType(aVideoHandleType),
	mResourceNecessity(EResourceNecessityUnknown),
	mVisibility(false),
    mVideoResouceManager(*CVideoResourceManager::getInstance()),
	mAudioPathChangeListener(0),
	mAllocatedResource(NULL)
{
}

void OplVideoHandle::init()
{
}

void OplVideoHandle::deInit()
{
}

void OplVideoHandle::regist(OplVideoHandle* aVideoHandle)
{
	CVideoResourceManager::getInstance()->registerVideoHandle(aVideoHandle);
	aVideoHandle->init();
}

void OplVideoHandle::unRegist(OplVideoHandle* aVideoHandle)
{
	aVideoHandle->deInit();
	CVideoResourceManager::getInstance()->unRegisterVideoHandle(aVideoHandle);
}

OplVideoHandle::~OplVideoHandle()
{
	HxLOG_Critical("[VRM] VRM.OplVideoHandle::~OplVideoHandle 0x%08X\n", this);
	HxLOG_Assert(!CVideoResourceManager::getInstance()->isRegistered(this));
}

OplVideoHandle::TResourceNecessity OplVideoHandle::getResourceNecessity()
{
	return mResourceNecessity;
}

void OplVideoHandle::setAudioPathChangeListener(IAudioPathChangeListener* aListener)
{
	mAudioPathChangeListener = aListener;
}

void OplVideoHandle::requestAudioPath()
{
	videoResourceRequired();
	mVideoResouceManager.requestAudioPath(this);
}

bool OplVideoHandle::hasAudioPath()
{
	return getAudioPathState() == EAudioPathStateAcquired;
}

bool OplVideoHandle::setVisibility(bool visible)
{
	if (visible)
	{
		mVideoResouceManager.videoHandleShown(this);
	}
	mVisibility = visible;
	applyVisibility();
	if (!visible)
	{
		mVideoResouceManager.videoHandleHidden(this);
	}
	return true;
}

bool OplVideoHandle::isVisible()
{
	return mVisibility;
}

OplVideoHandle::THandleOwnerType OplVideoHandle::getHandleOwnerType()
{
	return mHandleOwnerType;
}

OplVideoHandle::TVideoHandleType OplVideoHandle::getVideoHandleType()
{
	return mVideoHandleType;
}

OplVideoHandle::TAudioPathState OplVideoHandle::getAudioPathState()
{
	OplVideoHandle::TAudioPathState ret = EAudioPathStateNone;
	if (mAllocatedResource)
	{
		ret = mAllocatedResource->isMain() ? EAudioPathStateAcquired: EAudioPathStateReleased;
	}
	return ret;
}

void OplVideoHandle::changeAudioPathState(const TAudioPathState& aAudioPathState)
{
	if (mAudioPathChangeListener)
	{
		mAudioPathChangeListener->audioPathChanged(aAudioPathState == EAudioPathStateAcquired);
	}
}

void OplVideoHandle::videoResouceAllocated(CVideoResource* aVideoResource)
{
	HxLOG_Assert(mAllocatedResource == NULL);
	mAllocatedResource = aVideoResource;
	updateAudioPathState();
	applyVisibility(true);
	processVideoResourceAllocated(aVideoResource);
}

void OplVideoHandle::videoResouceDeAllocated(CVideoResource* aVideoResource, const TVideoHandleType& aVideoHandleType)
{
	HxLOG_Critical("[VRM] VRM.[%d].videoResouceDeAllocated prev=0x%08X new=0x%08X\n", getIndex(),mAllocatedResource, aVideoResource);
	HxLOG_Assert(mAllocatedResource == aVideoResource);
	mAllocatedResource = NULL;
	updateAudioPathState();
	processVideoResourceDeAllocated(aVideoResource, aVideoHandleType);
}

CVideoResource* OplVideoHandle::getAllocatedResource()
{
	return mAllocatedResource;
}

void OplVideoHandle::updateAudioPathState()
{
	OplVideoHandle::TAudioPathState audioState = EAudioPathStateNone;
	if (mAllocatedResource)
	{
		audioState = mAllocatedResource->isMain() ? EAudioPathStateAcquired: EAudioPathStateReleased;
	}
	changeAudioPathState(audioState);
}

void OplVideoHandle::applyVisibility(bool showAlways)
{
	if (mAllocatedResource && getHandleOwnerType() == EHandleOwnerTypeSystem )
	{
		int resourceId = mAllocatedResource->getVideoResourceId();
		bool visibility = mVisibility || showAlways;
		HxLOG_Critical("[VRM] VRM.[%d].setVisibility resourceId=%d visibility=%d\n", getIndex(), resourceId, visibility);
		(void)APK_MEDIA_PLAY_SetVideoHide(resourceId/*ulViewId*/, !visibility);
	}
}

void OplVideoHandle::videoResourceRequired(const char* aReason)
{
	mResourceNecessity = EResourceNecessityRequired;
	if (!mAllocatedResource)
	{
		mVideoResouceManager.videoResourceRequired(this, aReason ? aReason: KStrReasonUnKnown);
	}
}

void OplVideoHandle::videoResourceNotRequired(const char* aReason)
{
	mResourceNecessity = EResourceNecessityNotRequired;
	if (mAllocatedResource)
	{
		mVideoResouceManager.videoResourceNotRequired(this, aReason ? aReason: KStrReasonUnKnown);
	}
}

bool OplVideoHandle::isResourceAllocated()
{
	return mAllocatedResource != NULL;
}

int OplVideoHandle::getIndex()
{
	return mVideoResouceManager.findIndex(this);
}
