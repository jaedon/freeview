/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */

#include <nrd/DeviceConstants.h>
#include <nrd/IDeviceError.h>
#include <nrd/NrdApplication.h>
#include <nrd/DrmManager.h>

#include "ESManagerNative.h"
#include "PlaybackGroupNative.h"
#include "DecoderManager.h"
#include "RendererManager.h"

using namespace netflix;
using namespace netflix::device;
using namespace netflix::device::esplayer;
using namespace std;

ESManagerNative::ESManagerNative()
{
    mDecoderManager.reset(new DecoderManager);
    mRendererManager.reset(new RendererManager);
    mDecoderManager->initFramework();
    mMultiSessionDrmManager = netflix::DrmManager::instance();
    mIsMediaPipelineInUse.clear();

    m_stResourceMap.hPlaypumpList = NULL;
    m_stResourceMap.hDmaList = NULL;
    m_stResourceMap.uiNumOfPlaypump = 0;
    m_stResourceMap.uiNumOfDma = 0;
	requestResourceMap();
}


ESManagerNative::~ESManagerNative()
{
    // Delete any stream players that have not been deleted
    set<PlaybackGroupNative*>::iterator it;
    for(it = mPlaybackGroups.begin(); it != mPlaybackGroups.end(); it++)
    {
        delete *it;
    }
    mDecoderManager->quitFramework();
    mIsMediaPipelineInUse.clear();
}

void ESManagerNative::registerAudioMixerOutput(shared_ptr<IAudioMixerOutput> &mMixerOutputHandle)
{
    mMixerOutput.reset();
    mMixerOutput = mMixerOutputHandle;
}

NFErr ESManagerNative::createPlaybackGroup(IPlaybackGroup*& playbackGroup)
{
    return createPlaybackGroup(playbackGroup, 0);
}

NFErr ESManagerNative::createPlaybackGroup(IPlaybackGroup*& playbackGroup, uint32_t pipelineId)
{
	requestResourceMap();

    NTRACE(TRACE_MEDIAPLAYBACK
           , "ESManagerNative::createPlaybackGroup with pipelineId %d"
           , pipelineId);

    if(mPlaybackGroups.size() == 0){
        uint32_t numPipelines = 1;
        if(nrdApp()) {
            nrdApp()->getSystem()->maxVideoPipelines();
        }
        for(uint32_t i=0; i<numPipelines; i++){
            mIsMediaPipelineInUse[i] = false; // false means, not in use. true means in use.
        }
    }

    NFErr err;
    ullong deviceSpecificErroCode = skeletonDeviceSpecific_createPlaybackGroupFailed;

    if(mIsMediaPipelineInUse[pipelineId] == true){
        // disable until nrdjs passes correct pipelineId
        /*
        std::ostringstream ss;
        ss << "ESManagerNative::createPlaybackGroup failed : pipeline already in use";
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        err.push(new IDeviceError(INITIALIZATION_ERROR,
                                  deviceSpecificErroCode,
                                  errInfo));
        return err;
        */
    }

    PlaybackGroupNative* group = new PlaybackGroupNative(*this, pipelineId);
    if(group == NULL)
    {
        std::ostringstream ss;
        ss << "ESManagerNative::createPlaybackGroup failed to instantiate PlaybackGroupNative";
        Variant errInfo;
        errInfo["errorDescription"] = ss.str();
        err.push(new IDeviceError(INITIALIZATION_ERROR,
                                  deviceSpecificErroCode,
                                  errInfo));
        return err;
    } else {
        mPlaybackGroups.insert(group);
        playbackGroup = group;
        mIsMediaPipelineInUse[pipelineId] = true; // false means, not in use. true means in use.
        return err;
    }
}

void ESManagerNative::destroyPlaybackGroup(IPlaybackGroup* playbackGroup)
{
    PlaybackGroupNative* group = static_cast<PlaybackGroupNative*>(playbackGroup);
    assert(mPlaybackGroups.find(group) != mPlaybackGroups.end());
    if(mPlaybackGroups.find(group) != mPlaybackGroups.end())
    {
        mIsMediaPipelineInUse[group->getPipelineId()] = false;
        mPlaybackGroups.erase(group);
        delete group;
    }
}

shared_ptr<RendererManager> ESManagerNative::getRendererManager()
{
    return mRendererManager;
}

shared_ptr<DecoderManager> ESManagerNative::getDecoderManager()
{
    return mDecoderManager;
}

netflix::DrmManager* ESManagerNative::getMultiSessionDrmManager()
{
    return mMultiSessionDrmManager;
}

ESManagerNativeResourceMap_t* ESManagerNative::getResourceMap()
{
    return &m_stResourceMap;
}

void ESManagerNative::requestResourceMap()
{
	ESManagerNativeResourceMap_t  *ptResourceMap = NULL;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	ptResourceMap = getResourceMap();
	if(ptResourceMap == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [ESManagerNative::%s:%d] ptResourceMap is NULL\n", __func__, __LINE__); 
		goto func_exit;
	}
		
	eRet = NativeBridge::esManager().getResourceMap((void *)&m_stResourceMap);
    if(eRet != NativeBridge_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [ESManagerNative::%s:%d] Failed to request resource map, eRet = %d\n", __func__, __LINE__, eRet); 
		goto func_exit;
	}

func_exit:
	return;
}

#if 1
void ESManagerNative::detachMixerOutput()
{
    AudioMixerOutputBRCM* mMixerOutputBrcm = (AudioMixerOutputBRCM*)mMixerOutput.get();
    if(mMixerOutputBrcm)
    {
        (void) mMixerOutputBrcm->detachOutput();
    }
}

void ESManagerNative::attachMixerOutput()
{
    AudioMixerOutputBRCM* mMixerOutputBrcm = (AudioMixerOutputBRCM*)mMixerOutput.get();
    if(mMixerOutputBrcm)
    {
        (void) mMixerOutputBrcm->attachOutput();
    }
}

void ESManagerNative::pauseMixerTransfer()
{
    AudioMixerOutputBRCM* mMixerOutputBrcm = (AudioMixerOutputBRCM*)mMixerOutput.get();
    if(mMixerOutputBrcm)
    {
        (void) mMixerOutputBrcm->pauseTransfer();       
    }
}

void ESManagerNative::resumeMixerTransfer()
{
    AudioMixerOutputBRCM* mMixerOutputBrcm = (AudioMixerOutputBRCM*)mMixerOutput.get();
    if(mMixerOutputBrcm)
    {
        (void) mMixerOutputBrcm->resumeTransfer();
    }
}
#endif
