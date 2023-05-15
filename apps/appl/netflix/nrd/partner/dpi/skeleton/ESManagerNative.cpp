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

using namespace netflix;
using namespace netflix::device;
using namespace netflix::device::esplayer;
using namespace std;

ESManagerNative::ESManagerNative()
{
    mMultiSessionDrmManager = netflix::DrmManager::instance();
    mIsMediaPipelineInUse.clear();
}


ESManagerNative::~ESManagerNative()
{
    // Delete any stream players that have not been deleted
    set<PlaybackGroupNative*>::iterator it;
    for(it = mPlaybackGroups.begin(); it != mPlaybackGroups.end(); it++)
    {
        delete *it;
    }
    mIsMediaPipelineInUse.clear();
}

NFErr ESManagerNative::createPlaybackGroup(IPlaybackGroup*& playbackGroup)
{
    return createPlaybackGroup(playbackGroup, 0);
}

NFErr ESManagerNative::createPlaybackGroup(IPlaybackGroup*& playbackGroup, uint32_t pipelineId)
{
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


netflix::DrmManager* ESManagerNative::getMultiSessionDrmManager()
{
    return mMultiSessionDrmManager;
}
