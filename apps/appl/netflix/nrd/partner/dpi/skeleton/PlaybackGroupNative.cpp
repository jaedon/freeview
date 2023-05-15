/*
 * (c) 1997-2015 Netflix, Inc.  All content herein is protected by
 * U.S. copyright and other applicable intellectual property laws and
 * may not be copied without the express permission of Netflix, Inc.,
 * which reserves all rights.  Reuse of any of this content for any
 * purpose without the permission of Netflix, Inc. is strictly
 * prohibited.
 */
#include "PlaybackGroupNative.h"
#include "ESPlayerConstants.h"
#include "ESManagerNative.h"
#include "AudioESPlayerNative.h"
#include "VideoESPlayerNative.h"
#include "DeviceLibNative.h"
#include <nrd/IPlaybackDevice.h>
#include <nrdbase/Log.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/MutexRanks.h>

using namespace netflix;
using namespace netflix::device::esplayer;
using namespace netflix::device;
using namespace std;

PlaybackGroupNative::PlaybackGroupNative(ESManagerNative& ESManager, uint32_t pipelineId) :
  mPlaybackState(PAUSE),
  mPlaybackStateMutex(ESP_PLAYBACKGROUP_MUTEX, "ESP Playback Group Mutex"),
  mESManager(ESManager),
  mPipelineId(pipelineId)
{
}

PlaybackGroupNative::~PlaybackGroupNative()
{
    // Delete any stream players that have not been deleted
    set<ESPlayerNative*>::iterator it;
    for(it = mStreamPlayers.begin(); it != mStreamPlayers.end(); it++)
    {
        delete *it;
    }
}

NFErr
PlaybackGroupNative::createStreamPlayer(const struct StreamPlayerInitData& initData,
                                        shared_ptr<IESPlayerCallback> callback,
                                        IElementaryStreamPlayer*& pStreamPlayer)
{
    NFErr err;
    ullong deviceSpecificErrorCode = 0;
    std::string deviceSpecificErrorMsg;
    std::string deviceStackTraceMsg;
    MediaType type = AUDIO; // initialize to audio
    ESPlayerNative* player = NULL;
    pStreamPlayer = NULL;

    // Check initData pointers.
    if(initData.mInitialStreamAttributes == NULL) {
        assert(initData.mInitialStreamAttributes != NULL);

        //this should be replaced by partner specific error code
        deviceSpecificErrorCode = skeletonDeviceSpecific_NoInitialStreamAttributes;
        Variant errInfo;
        errInfo["errorDescription"] = "initial stream attributes NULL";
        return err.push(new IDeviceError(INITIALIZATION_ERROR,
                                         deviceSpecificErrorCode,
                                         errInfo));
    }

    MediaType streamType = initData.mInitialStreamAttributes->mStreamType;

    // Initialize the audio player if this is an audio stream.
    if(streamType == AUDIO)
    {
        type = AUDIO;

        // Check audio attributes pointer
        assert(initData.mInitialStreamAttributes->mAudioAttributes != NULL);
        if(initData.mInitialStreamAttributes->mAudioAttributes == NULL)
        {
            // this should be replaced by partner specific error code
            deviceSpecificErrorCode = skeletonDeviceSpecific_NoInitialAudioStreamAttributes;
            Variant errInfo;
            errInfo["errorDescription"] = "no initialStreamAttributes for Audio";
            return err.push(new IDeviceError(INITIALIZATION_ERROR,
                                             deviceSpecificErrorCode,
                                             errInfo));
        }
        player = new AudioESPlayerNative;

    } else {
        // Initialize a video player
        type = VIDEO;
        // Check video attributes pointer.
        assert(initData.mInitialStreamAttributes->mVideoAttributes != NULL);
        if(initData.mInitialStreamAttributes->mVideoAttributes == NULL)
        {
            // this should be replaced by partner specific error code
            deviceSpecificErrorCode = skeletonDeviceSpecific_NoInitialVideoStreamAttributes;
            Variant errInfo;
            errInfo["errorDescription"] = "no initialStreamAttributes for Video";
            return err.push(new IDeviceError(INITIALIZATION_ERROR,
                                             deviceSpecificErrorCode,
                                             errInfo));
        }

        // If a device requires MVC delivered with the base and depenent view
        // NALUs in separate buffers, USE_MVC_SPLIT should be defined.  See
        // IElementaryStreamPlayer.h for details about MVC_SPLIT and
        // MVC_COMBINED mode.
// #define USE_MVC_SPLIT
#ifdef USE_MVC_SPLIT
        if(initData.mInitialStreamAttributes->mVideoAttributes->mFormat3D == MVC_COMBINED)
        {
            // this should be replaced by partner specific error code
            deviceSpecificErrorCode = refAppDeviceSpecific_Wrong3DMode;
            Variant errInfo;
            errInfo["errorDescription"] = "wrong 3D stream architecture";
            return err.push(new IDeviceError(SPLIT_MVC_REQUIRED,
                                             deviceSpecificErrorCode,
                                             errInfo));
        }
#endif
        player = new VideoESPlayerNative;

    }

    if(player == NULL)
    {
        if(type == AUDIO){
            deviceSpecificErrorCode = skeletonDeviceSpecific_AudioPlayerCreationFailed;
        } else {
            deviceSpecificErrorCode = skeletonDeviceSpecific_VideoPlayerCreationFailed;
        }
        Variant errInfo;
        errInfo["errorDescription"] = "failed to create player";
        return err.push(new IDeviceError(INITIALIZATION_ERROR,
                                         deviceSpecificErrorCode,
                                         errInfo));
    }

    err = player->init(initData, callback, this);
    if(!err.ok()){
        delete player;
        return err;
    }

    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackGroupNative::createStreamPlayer: %s player initialized",
           streamType == AUDIO ? "audio" : "video");

    pStreamPlayer = player;
    mStreamPlayers.insert(player);

    return err;
}

void PlaybackGroupNative::destroyStreamPlayer(IElementaryStreamPlayer* iesPlayer)
{
    ESPlayerNative* streamPlayer = static_cast<ESPlayerNative*>(iesPlayer);

    assert(mStreamPlayers.find(streamPlayer) != mStreamPlayers.end());
    if(mStreamPlayers.find(streamPlayer) != mStreamPlayers.end())
    {
        mStreamPlayers.erase(streamPlayer);
        delete streamPlayer;
    }
}

bool PlaybackGroupNative::streamPlayersAreReadyForPlaybackStart()
{
    set<ESPlayerNative*>::iterator it;

    for(it = mStreamPlayers.begin(); it != mStreamPlayers.end(); it++)
    {
        if(!(*it)->readyForPlaybackStart())
        {
            return false;
        }
    }
    return true;
}

bool PlaybackGroupNative::setPlaybackState(PlaybackState state)
{
    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackGroupNative::setPlaybackState(): "
           "state = %s\n", state == PLAY ? "PLAY":"PAUSE");

    if(state == PAUSE)
    {
        // PARTNER: Pause the pipeline
    }
    else if(state == PLAY)
    {
        if(!streamPlayersAreReadyForPlaybackStart() )
        {
            // Players are not ready to transition from paused to play. Return
            // false.  The SDK will wait a few ms and try again.
            return false;
        }
        // PARTNER: Play the pipeline
    }
    else {
        // We only support PAUSE and PLAY.  If a new state is added make sure handling gets implemented here!
        assert(0);
    }

    {
        ScopedMutex cs(mPlaybackStateMutex);
        mPlaybackState = state;
    }
    return true;
}

IPlaybackGroup::PlaybackState PlaybackGroupNative::getPlaybackState()
{
    ScopedMutex cs(mPlaybackStateMutex);
    return mPlaybackState;
}

void PlaybackGroupNative::flush()
{
    set<ESPlayerNative*>::iterator it;
    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackGroupNative::flush() begin");

    // PARTNER: Pause the pipeline

    for(it = mStreamPlayers.begin(); it != mStreamPlayers.end(); it++)
    {
        (*it)->beginFlush();
    }

    // PARTNER: Invalidate the presentation time (if needed).

    for(it = mStreamPlayers.begin(); it != mStreamPlayers.end(); it++)
    {
        (*it)->endFlush();
    }

    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackGroupNative::flush() end");

}

void PlaybackGroupNative::audioFlushed()
{
    // PARTNER: Invalidate audio PTS
}

// to be deprecated
IPlaybackGroup::Rect PlaybackGroupNative::setVideoWindow(const Rect& targetRect, uint32_t transitionDuration)
{
    return setVideoWindow(targetRect, transitionDuration, 0);
}

IPlaybackGroup::Rect PlaybackGroupNative::setVideoWindow(const Rect& targetRect,
                                                         uint32_t transitionDuration,
                                                         uint32_t /*zorder*/) //fix warning --Sam
{
    // Find the video player and set the video window. We assume there's one
    // video player in the group.
    set<ESPlayerNative*>::iterator it;
    for(it = mStreamPlayers.begin(); it != mStreamPlayers.end(); it++)
    {
        if((*it)->getMediaType() == VIDEO)
        {
            return (*it)->setVideoWindow(targetRect, transitionDuration);
        }
    }
    assert(0);
    return targetRect;
}

void PlaybackGroupNative::bringVideoToFront()
{
    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackGroupNative::bringVideoToFront() called");
    // Does nothing when in its very own DFB window
}

void PlaybackGroupNative::sendVideoToBack()
{
    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackGroupNative::sendVideoToBack() called");
    // Does nothing when in its very own DFB window
}


ESManagerNative* PlaybackGroupNative::getESManager()
{
    return &mESManager;
}

uint32_t PlaybackGroupNative::getPipelineId() const
{
    return mPipelineId;
}
