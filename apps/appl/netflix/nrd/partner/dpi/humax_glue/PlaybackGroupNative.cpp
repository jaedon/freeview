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
#include "ReferenceClockNative.h"
#include "ESManagerNative.h"
#include "AudioESPlayerNative.h"
#include "VideoESPlayerNative.h"
#include "DeviceLibNative.h"
#include <nrd/IPlaybackDevice.h>
#include <nrd/NrdApplication.h>
#include <nrdbase/Log.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/MutexRanks.h>
#include <nrdbase/Time.h>
#include <nrdbase/Thread.h>

#include "AVController.h"
#include "AVStatusManager.h"

#define WAIT_FOR_EVENT_TIME    (5000)

using namespace netflix;
using namespace netflix::device::esplayer;
using namespace netflix::device;
using namespace std;

PlaybackGroupNative* PlaybackGroupNative::s_hPlaybackGroup = NULL;
bool PlaybackGroupNative::s_bWaitForSeek = false;
pthread_mutex_t PlaybackGroupNative::s_mtxPlaybackGroup;

class WaitForEventTimer : public Application::Timer
{
public:
    WaitForEventTimer(int interval, PlaybackGroupNative *hPlaybackGroup)
        : Timer(interval)
        , m_hPlaybackGroup(hPlaybackGroup)
    {
    }

    virtual void timerFired()
    {
        if(m_hPlaybackGroup == NULL)
        {
            Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [WaitForEventTimer::%s:%d] m_hPlaybackGroup is NULL\n", __func__, __LINE__);
        }
        else
        {
            if(!m_hPlaybackGroup->getWaitForSeekFlag())
            {
                Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [WaitForEventTimer::%s:%d] Timer Fired!\n", __func__, __LINE__); 
                m_hPlaybackGroup->setWaitForSeekFlag(true);
            }
        }
    }
private:
    PlaybackGroupNative *m_hPlaybackGroup;
};

PlaybackGroupNative::PlaybackGroupNative(ESManagerNative& ESManager, uint32_t pipelineId) :	
  m_bPlaybackStopped(false),
  mPlaybackState(PAUSE),
  mPlaybackStateMutex(ESP_PLAYBACKGROUP_MUTEX, "ESP Playback Group Mutex"),
  mWaitForEventMutex(ESP_PLAYBACKGROUP_MUTEX, "ESP Wait For Event Mutex"),
  mESManager(ESManager),
  m_session_id(0), // pipelineId
  m_unCurrentAudioTrack(0),
  mPipelineId(pipelineId),
  m_bNeedToFlushAudio(false)
{
    bool bRet = true;

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
    
    ESPlayerConstants::VIDEO_REN_SCREEN_WIDTH = sConfiguration->videoRendererScreenWidth;
    ESPlayerConstants::VIDEO_REN_SCREEN_HEIGHT = sConfiguration->videoRendererScreenHeight;

    mReferenceClock.reset(new ReferenceClockNative);
    mReferenceClock->setSpeed(IPlaybackDevice::PAUSED_SPEED);

    bRet = avOpen();
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to open AV Modules\n", __func__, __LINE__); 
    }
    
    bRet = csoOpen();
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to open UVA\n", __func__, __LINE__);
    }

    pthread_mutex_init(&s_mtxPlaybackGroup, NULL);
    MutexLock();
    s_hPlaybackGroup = this;
    MutexUnlock();
    setWaitForSeekFlag(false);

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
}

PlaybackGroupNative::~PlaybackGroupNative()
{
    bool bRet = true;
    
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
    
    // Delete any stream players that have not been deleted
    set<ESPlayerNative*>::iterator it;
    for(it = mStreamPlayers.begin(); it != mStreamPlayers.end(); it++)
    {
        delete *it;
    }

    bRet = csoClose();
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to close UVA\n", __func__, __LINE__); 
    }

    bRet = avClose();
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to close AV Modules\n", __func__, __LINE__);
    }

    setWaitForSeekFlag(false);

    MutexLock();
	setNeedToFlushAudio(false);
	s_hPlaybackGroup = NULL;
    MutexUnlock();
    pthread_mutex_destroy(&s_mtxPlaybackGroup);
    
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
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
        // Check audio attributes pointer
        assert(initData.mInitialStreamAttributes->mAudioAttributes != NULL);
        if(initData.mInitialStreamAttributes->mAudioAttributes == NULL)
        {
            // this should be replaced by partner specific error code
            deviceSpecificErrorCode = skeletonDeviceSpecific_NoInitialAudioStreamAttributes;
            deviceSpecificErrorMsg = "no initialStreamAttributes for Audio";
            Variant errInfo;
            errInfo["errorDescription"] = "no initialStreamAttributes for Audio";
            return err.push(new IDeviceError(INITIALIZATION_ERROR,
                                             deviceSpecificErrorCode,
                                             errInfo));
        }
#if 0
        player = new AudioESPlayerNative;
#else
        player = new AudioESPlayerNative(m_session_id);
#endif
    } else {
        // Initialize a video player
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
            deviceSpecificErrorCode = skeletonDeviceSpecific_Wrong3DMode;
            Variant errInfo;
            errInfo["errorDescription"] = "wrong 3D stream architecture";
            return err.push(new IDeviceError(SPLIT_MVC_REQUIRED,
                                             deviceSpecificErrorCode,
                                             errInfo));
        }
#endif
#if 0
        player = new VideoESPlayerNative;
#else
        player = new VideoESPlayerNative(m_session_id);
#endif
    }

    if(player == NULL)
    {
        if(streamType == AUDIO){
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
    bool bRet = true, bResume = false;
    AVController *hController = NULL;
    AVCtrl_PlaybackStatus_e ePlaybackStatus = AVCtrl_PlaybackStatus_Stop;
    AVCtrl_ProbeStatus_e eProbeStatus = AVCtrl_ProbeStatus_None;
    
    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackGroupNative::setPlaybackState(): "
           "state = %s\n", state == PLAY ? "PLAY":"PAUSE");

	AVController::MutexLock();

    hController = AVController::getInstance();
    if(hController == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
        bRet = false;
        goto func_exit;
    }

    if(state == PAUSE)
    {
        ePlaybackStatus = hController->getPlaybackStatus();
        if(ePlaybackStatus == AVCtrl_PlaybackStatus_Playing)
        {
        	bRet = hController->pause();
            if(!bRet)
            {
                Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to pause\n", __func__, __LINE__);
                goto func_exit;                
            }
            // PARTNER: Pause the pipeline
            mReferenceClock->setSpeed(IPlaybackDevice::PAUSED_SPEED);
            ePlaybackStatus = AVCtrl_PlaybackStatus_Pause;
            hController->setPlaybackStatus(ePlaybackStatus);
            if (1) (void)NativeBridge::playbackGroup().informLastPlayCmd(NativeBridge_PLAYCMD_PAUSE);
        }
    }
    else if(state == PLAY)
    {
    	if(!streamPlayersAreReadyForPlaybackStart() )
        {
            // Players are not ready to transition from paused to play. Return
            // false.  The SDK will wait a few ms and try again.
            bRet = false;
            goto func_exit;
        }

        ePlaybackStatus = hController->getPlaybackStatus();
        if(ePlaybackStatus == AVCtrl_PlaybackStatus_Stop)
        {
#if 1
            /* parkjh4 : Stop mixeroutput first */
            mESManager.detachMixerOutput();
#endif

            eProbeStatus = hController->getProbeStatus();
            if(eProbeStatus == AVCtrl_ProbeStatus_None)
            {
                bRet = csoProbe();
                if(!bRet)
                {
                    Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to probe or to play the content\n", __func__, __LINE__);
                    goto func_exit;
                }

                eProbeStatus = AVCtrl_ProbeStatus_Probing;
                hController->setProbeStatus(eProbeStatus);
                /* Player isn't ready to play yet */
                bRet = false;
                goto func_exit;
            }
            else if(eProbeStatus == AVCtrl_ProbeStatus_ProbeCompleted)
            {
                /* Start a media player */
				bRet = hController->play();
                if(!bRet)
                {
                    Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Playing the content failed\n", __func__, __LINE__);
                    goto func_exit;
                }
                
                mReferenceClock->setSpeed(IPlaybackDevice::NORMAL_SPEED);
                ePlaybackStatus = AVCtrl_PlaybackStatus_Playing;
                hController->setPlaybackStatus(ePlaybackStatus);
                if (1) (void)NativeBridge::playbackGroup().informLastPlayCmd(NativeBridge_PLAYCMD_PLAY);
            }
            else    /* Probing */
            {
                bRet = false;
                goto func_exit;
            }
        }
        else if(ePlaybackStatus == AVCtrl_PlaybackStatus_Pause)
        {
			bResume = hController->readyToResume();
			if(!bResume)
			{
				bRet = false;
				goto func_exit;
			}

			bRet = csoResume();
            if(!bRet)
            {
                Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to resume the content\n", __func__, __LINE__);
                goto func_exit;
            }
            
            mReferenceClock->setSpeed(IPlaybackDevice::NORMAL_SPEED);
            ePlaybackStatus = AVCtrl_PlaybackStatus_Playing;
            hController->setPlaybackStatus(ePlaybackStatus);    /* PLAY */
            if (1) (void)NativeBridge::playbackGroup().informLastPlayCmd(NativeBridge_PLAYCMD_PLAY);
        }
        else    /* Already play */
        {
            // PARTNER: Play the pipeline
            mReferenceClock->setSpeed(IPlaybackDevice::NORMAL_SPEED);
            ePlaybackStatus = AVCtrl_PlaybackStatus_Playing;
            hController->setPlaybackStatus(ePlaybackStatus);    /* PLAY */            
            if (1) (void)NativeBridge::playbackGroup().informLastPlayCmd(NativeBridge_PLAYCMD_PLAY);
        }
    }
    else
    {
        AVController::MutexUnlock();
        // We only support PAUSE and PLAY.  If a new state is added make sure handling gets implemented here!
        assert(0);
    }

    {
        ScopedMutex cs(mPlaybackStateMutex);
        mPlaybackState = state;
    }
    
func_exit:
    AVController::MutexUnlock();
	return bRet;    
}

IPlaybackGroup::PlaybackState PlaybackGroupNative::getPlaybackState()
{
    ScopedMutex cs(mPlaybackStateMutex);
    return mPlaybackState;
}

void PlaybackGroupNative::flush()
{
    set<ESPlayerNative*>::iterator it;
    bool bRet = true;
    AVController *hController = NULL;
    CSO_ERR csoErr = CSO_ERR_OK;
    CSO_CTRL_PARAM_t cso_ctrl_info;
    static const Time WAIT_WHILE_CSO_SEEK(10);
    int32_t nInterval = 0;

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackGroupNative::flush() begin");

    // PARTNER: Pause the pipeline
    mReferenceClock->setSpeed(IPlaybackDevice::PAUSED_SPEED);

    for(it = mStreamPlayers.begin(); it != mStreamPlayers.end(); it++)
    {
        (*it)->beginFlush();
    }

    // PARTNER: Invalidate the presentation time (if needed).
    mReferenceClock->invalidateCurrPresentationTime();
    mReferenceClock->invalidateAudioPresentationTime();
    mReferenceClock->invalidateVideoPresentationTime();

    memset(&cso_ctrl_info, 0x00, sizeof(CSO_CTRL_PARAM_t));
    csoErr = CSO_UVA_Control(m_session_id, eCSO_CTRL_SEEK, &cso_ctrl_info);
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to seek, csoErr = %d\n", __func__, __LINE__, csoErr);
    }

    /* Prevent the infinite loop */
    nInterval = WAIT_FOR_EVENT_TIME;

    shared_ptr<WaitForEventTimer> waitForEventTimer(new WaitForEventTimer(nInterval, this));
    waitForEventTimer->start();

    /* Wait for CSO_EVENT_MEDIA_SEEK_COMPLETE event from Calypso */
    while(!getWaitForSeekFlag())
    {
        Thread::Sleep(WAIT_WHILE_CSO_SEEK);
        continue;
    }
    
    waitForEventTimer->stop();
    waitForEventTimer.reset();
    setWaitForSeekFlag(false);

    AVController::MutexLock();
    
    hController = AVController::getInstance();
    if(hController == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
    }
    else
    {
        /* Flush the data in decoder */
        bRet = hController->flush();
        if(!bRet)
        {            
            Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to flush the data in decoder\n", __func__, __LINE__);
        }
    }
    AVController::MutexUnlock();

    csoErr = CSO_UVA_Control(m_session_id, eCSO_CTRL_RESUME, &cso_ctrl_info);
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to resume, csoErr = %d\n", __func__, __LINE__, csoErr);
    }

    for(it = mStreamPlayers.begin(); it != mStreamPlayers.end(); it++)
    {
        (*it)->endFlush();
    }

    NTRACE(TRACE_MEDIAPLAYBACK, "PlaybackGroupNative::flush() end");
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
    return;
}

void PlaybackGroupNative::audioFlushed()
{
    CSO_ERR 			csoErr = CSO_ERR_OK;
    CSO_CTRL_PARAM_t 	cso_ctrl_info;
    uint32_t			unCurrentAudioTrack = 0;
	
	csoErr = CSO_UVA_AudioFlush(m_session_id);    
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to request switching an audio track, csoErr = %d\n", __func__, __LINE__, csoErr);
		if(csoErr == CSO_ERR_NOT_READY)
    	{
    	/* 	Fix codes for NTS AUDIO-026-TCs.
			AUDIO-026-TCs fail when NTS tries to change audio track before ready. 
			In this case, we need to change audio track after CSO_UVA_Start() is called	*/
    		MutexLock();
			if(!getNeedToFlushAudio())
			{
				setNeedToFlushAudio(true);
			}
			MutexUnlock();
		}
		
		goto func_exit;        
    }
    
    unCurrentAudioTrack = getCurrentAudioTrack();
    if(unCurrentAudioTrack & 0x01)    /* unCurrentAudioTrack == 1 or 3 */
    {
        unCurrentAudioTrack--;
    }
    else                            /* unCurrentAudioTrack == 0 or 2 */
    {
        unCurrentAudioTrack++;
    }
    
    memset(&cso_ctrl_info, 0x00, sizeof(CSO_CTRL_PARAM_t));
    cso_ctrl_info.ulParam1 = unCurrentAudioTrack + 1;
    csoErr = CSO_UVA_Control(m_session_id, eCSO_CTRL_SELECT_AUDIO_TRACK, &cso_ctrl_info);
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to select an audio track, csoErr = %d\n", __func__, __LINE__, csoErr);
		if(csoErr == CSO_ERR_NOT_READY)
    	{
    	/* 	Fix codes for NTS AUDIO-026-TCs.
			AUDIO-026-TCs fail when NTS tries to change audio track before ready. 
			In this case, we need to change audio track after CSO_UVA_Start() is called	*/    	
    		MutexLock();
			if(!getNeedToFlushAudio())
			{
				setNeedToFlushAudio(true);
			}
			MutexUnlock();
		}
		
		goto func_exit;
    }

	setCurrentAudioTrack(unCurrentAudioTrack);

func_exit:
    // PARTNER: Invalidate audio PTS
    mReferenceClock->invalidateAudioPresentationTime();
    return;
}

// to be deprecated
IPlaybackGroup::Rect PlaybackGroupNative::setVideoWindow(const Rect& targetRect, uint32_t transitionDuration) // 
{
    return setVideoWindow(targetRect, transitionDuration, 0); // 
}

void PlaybackGroupNative::audioCodecSwitch(int32_t nCsoAudioCodec)
{
    AVController *hController = NULL;
    bool bRet = true;
    int32_t nAudioCodec = 0;
    uint32_t unCurrentAudioTrack = 0;
    CSO_ERR csoErr = CSO_ERR_OK;
    CSO_CTRL_PARAM_t cso_ctrl_info;
    
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
    
    nAudioCodec = (int32_t)convertAudioCodecFromCsoToPlaybackGroup((CSO_ACODEC)nCsoAudioCodec);

    unCurrentAudioTrack = getCurrentAudioTrack();
    if(unCurrentAudioTrack < 2)
    {
        unCurrentAudioTrack = 2;
    }
    else
    {
        unCurrentAudioTrack = 0;
    }

    AVController::MutexLock();
    
    hController = AVController::getInstance();
    if(hController == NULL)
    {
        AVController::MutexUnlock();
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
        goto func_exit;
    }
    
    bRet = hController->audioCodecSwitch(unCurrentAudioTrack, nAudioCodec);
    if(!bRet)
    {
        AVController::MutexUnlock();
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to switch an audio codec\n", __func__, __LINE__);
        goto func_exit;
    }

    AVController::MutexUnlock();

    memset(&cso_ctrl_info, 0x00, sizeof(CSO_CTRL_PARAM_t));
    cso_ctrl_info.ulParam1 = unCurrentAudioTrack + 1;

    csoErr = CSO_UVA_Control(m_session_id, eCSO_CTRL_SELECT_AUDIO_TRACK, &cso_ctrl_info);
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to select an audio track, csoErr = %d\n", __func__, __LINE__, csoErr);
        goto func_exit;
    }

	setCurrentAudioTrack(unCurrentAudioTrack);
	
func_exit:
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
    return;
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

ReferenceClockNative* PlaybackGroupNative::getReferenceClock()
{
    return mReferenceClock.get();
}

ESManagerNative* PlaybackGroupNative::getESManager()
{
    return &mESManager;
}

uint32_t PlaybackGroupNative::getPipelineId() const
{
    return mPipelineId;
}

PlaybackGroup_AudioCodec_e PlaybackGroupNative::convertAudioCodecFromCsoToPlaybackGroup(CSO_ACODEC eAudioCodec)
{
    PlaybackGroup_AudioCodec_e ePlaybackGroupAudioCodec = PLAYBACKGROUP_AUDIO_CODEC_UNKNOWN;
    
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] eAudioCodec = %d\n", __func__, __LINE__, eAudioCodec);
    switch(eAudioCodec)
    {
        /* Add Audio Codec */
        case CSO_ACODEC_MP3:
            ePlaybackGroupAudioCodec = PLAYBACKGROUP_AUDIO_CODEC_MP3;
            break;
        case CSO_ACODEC_AAC:
            ePlaybackGroupAudioCodec = PLAYBACKGROUP_AUDIO_CODEC_AAC;
            break;
        case CSO_ACODEC_AC3:
            ePlaybackGroupAudioCodec = PLAYBACKGROUP_AUDIO_CODEC_DOLBY_AC3;
            break;
        case CSO_ACODEC_AC3_PLUS:
            ePlaybackGroupAudioCodec = PLAYBACKGROUP_AUDIO_CODEC_DOLBY_AC3P;
            break;
        default:
            break;
    }
    return ePlaybackGroupAudioCodec;
}

PlaybackGroup_VideoCodec_e PlaybackGroupNative::convertVideoCodecFromCsoToPlaybackGroup(CSO_VCODEC eVideoCodec)
{
    PlaybackGroup_VideoCodec_e ePlaybackVideoCodec = PLAYBACKGROUP_VIDEO_CODEC_UNKNOWN;

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] eVideoCodec = %d\n", __func__, __LINE__, eVideoCodec);

    switch(eVideoCodec)
    {
        /* Add Video Codec */
        case CSO_VCODEC_H264:
            ePlaybackVideoCodec = PLAYBACKGROUP_VIDEO_CODEC_H264;
            break;
        case CSO_VCODEC_MPEG2:
            ePlaybackVideoCodec = PLAYBACKGROUP_VIDEO_CODEC_MPEG2;
            break;
        case CSO_VCODEC_MPEG4Part2:
            ePlaybackVideoCodec = PLAYBACKGROUP_VIDEO_CODEC_MPEG4Part2;
            break;
        default:
            break;
    }

    return ePlaybackVideoCodec;
}

bool PlaybackGroupNative::avOpen()
{
    bool bRet = true;
    AVController *hController = NULL;
    AVStatusManager *hStatusManager = NULL;

    /* new AVStatusManager */
    hStatusManager = AVStatusManager::getInstance();
    if(hStatusManager == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
        bRet = false;
        goto func_exit;
    }

    /* new AVController */
    hController = AVController::getInstance();
    if(hController == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
        bRet = false;
        goto func_exit;
    }
    
    bRet = hController->setListener(reinterpret_cast<void (*)(void*, AVCtrl_EventType_e)>(onOctoEventCallback));
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to set a listener\n", __func__, __LINE__);
        goto func_exit;
    }

func_exit:
    return bRet;
}

bool PlaybackGroupNative::avClose()
{
    bool bRet = true;
    AVController *hController = NULL;
    AVStatusManager *hStatusManager = NULL;
	AVCtrl_PlaybackStatus_e ePlaybackStatus = AVCtrl_PlaybackStatus_Stop;

    AVController::MutexLock();
    
    hController = AVController::getInstance();
    if(hController == NULL)
    {
        AVController::MutexUnlock();
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
        bRet = false;
    }
    else
    {   
#if 1            
        getPlaybackGroup()->getESManager()->pauseMixerTransfer();
#endif    
        bRet = hController->stop();
        if(!bRet)
        {
            Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to stop AV Controller\n", __func__, __LINE__);
            getPlaybackGroup()->getESManager()->resumeMixerTransfer();            
        }
        else
        {
			ePlaybackStatus = hController->getPlaybackStatus();
			if(ePlaybackStatus == AVCtrl_PlaybackStatus_Stop)
			{
				Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] A/V Playback was already stopped.\n", __func__, __LINE__);
				getPlaybackGroup()->getESManager()->resumeMixerTransfer(); 
			}
			else
			{				
				int nSleepUnit = 10;
				int nMaxSleepTime = 10000;	/* Waiting for 10 seconds */
				
				Log::error(TRACE_MEDIAPLAYBACK, "Wait for playback stopped event...\n");
	           
	  			AVController::MutexUnlock();
	            
				Log::error(TRACE_MEDIAPLAYBACK, "Timeout is set to %d", nMaxSleepTime);
				for(int nCurrentSleep = 0; !m_bPlaybackStopped && nCurrentSleep < nMaxSleepTime; nCurrentSleep += nSleepUnit)
				{
					Thread::Sleep(10);
				}

				if(!m_bPlaybackStopped)
				{
					Log::error(TRACE_MEDIAPLAYBACK, "Timeout! Audio Mixer could not work properly");
				}
	            
				AVController::MutexLock();
	           
	            Log::error(TRACE_MEDIAPLAYBACK, "Got playback stopped event...\n");  
			}     
        }
        
        if (1) (void)NativeBridge::playbackGroup().informLastPlayCmd(NativeBridge_PLAYCMD_STOP);
        AVController::MutexUnlock();
        
        delete hController;
        hController = NULL;
    }

    

    AVStatusManager::MutexLock();
    
    hStatusManager = AVStatusManager::getInstance();
    if(hStatusManager == NULL)
    {
        AVStatusManager::MutexUnlock();
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
        bRet = false;
    }
    else
    {
        AVStatusManager::MutexUnlock();
        delete hStatusManager;
        hStatusManager = NULL;
    }

    return bRet;
}

bool PlaybackGroupNative::csoOpen()
{
    bool bRet = true;
    CSO_ERR csoErr = CSO_ERR_OK;
    CSO_RESOURCE_MAP_t cso_map;

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
    if (1) (void)NativeBridge::playbackGroup().informLastPlayCmd(NativeBridge_PLAYCMD_OPEN);
    bRet = getResourceMap();
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Getting resource failed\n", __func__, __LINE__);
        goto func_exit;
    }

    /* Copy Resource Map */
    memset(&cso_map, 0x00, sizeof(CSO_RESOURCE_MAP_t));
    cso_map.uiNumOfPlaypump = m_stResourceMap.uiNumOfPlaypump;
    cso_map.hPlaypumpList = m_stResourceMap.hPlaypumpList;
    cso_map.uiNumOfDma = m_stResourceMap.uiNumOfDma;
    cso_map.hDmaList = m_stResourceMap.hDmaList;

    /* Set Resource Map */
    csoErr = CSO_UVA_SetResourceMap(&cso_map);
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to set a resource map, csoErr = %d\n", __func__, __LINE__, csoErr);
        bRet = false;
        goto func_exit;
    }

    /* Initialize CSO UVA */
    csoErr = CSO_UVA_Init(NULL);
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to initialize UVA or it might be already initialized, csoErr = %d\n", __func__, __LINE__, csoErr);
        
        /* It might be already initialized, so do not set error */
        //bRet = false;
        //goto func_exit;
    }

    /* Open CSO UVA */
    csoErr = CSO_UVA_NETFLIX_Open(&m_session_id);
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to open UVA, csoErr = %d\n", __func__, __LINE__, csoErr);
        bRet = false;
        goto func_exit;
    }

    csoErr = CSO_UVA_ResigterEventCallback(m_session_id, (void *)onCsoEventCallback);
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to register a event callback, csoErr = %d\n", __func__, __LINE__, csoErr);
        bRet = false;
        goto func_exit;
    }

func_exit:
    if (1) (void)NativeBridge::playbackGroup().informLastPlayCmd(NativeBridge_PLAYCMD_OPEN);
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
    return bRet;
}

bool PlaybackGroupNative::csoClose()
{
    bool bRet = true;
    CSO_ERR csoErr = CSO_ERR_OK;

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
    /* if (1) (void)NativeBridge::playbackGroup().informLastPlayCmd(NativeBridge_PLAYCMD_NA); */
    csoErr = CSO_UVA_Stop(m_session_id);
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to stop UVA, csoErr = %d\n", __func__, __LINE__, csoErr);
        bRet = false;
        goto func_exit;
    }
    if (1) (void)NativeBridge::playbackGroup().informLastPlayCmd(NativeBridge_PLAYCMD_STOP);

    csoErr = CSO_UVA_Close(m_session_id);
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to close UVA, csoErr = %d\n", __func__, __LINE__, csoErr);
        bRet = false;
        goto func_exit;
    }

    m_session_id = 0;

func_exit:
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
    if (1) (void)NativeBridge::playbackGroup().informLastPlayCmd(NativeBridge_PLAYCMD_NA);
    return bRet; 
}
bool PlaybackGroupNative::csoProbe()
{
    bool bRet = false;
    AVController *hController = NULL;
    PlaybackGroupProbeInfo_t stPlaybackGroupProbeInfo;
    CSO_ERR csoErr = CSO_ERR_OK;
    CSO_ProbeInfo_t info;

    csoErr = CSO_UVA_Probe(m_session_id);
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to probe, csoErr = %d\n", __func__, __LINE__, csoErr);
        bRet = false;
        goto func_exit;
    }

    memset(&info , 0x00, sizeof(CSO_ProbeInfo_t));

    csoErr = CSO_UVA_ProbeInfo(m_session_id, &info);
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Getting probe information failed, csoErr = %d\n", __func__, __LINE__, csoErr);
        bRet = false;
        goto func_exit;
    }

    memset(&stPlaybackGroupProbeInfo, 0x0, sizeof(PlaybackGroupProbeInfo_t));

    stPlaybackGroupProbeInfo.audio_pid = info.audio_stream_id;
    stPlaybackGroupProbeInfo.video_pid = info.video_stream_id;
    stPlaybackGroupProbeInfo.audio_codec = (int32_t)convertAudioCodecFromCsoToPlaybackGroup((CSO_ACODEC)info.audio_codec);
    stPlaybackGroupProbeInfo.video_codec = (int32_t)convertVideoCodecFromCsoToPlaybackGroup((CSO_VCODEC)info.video_codec);

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] audio_pid = 0x%03X\n", __func__, __LINE__, stPlaybackGroupProbeInfo.audio_pid);
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] video_pid = 0x%03X\n", __func__, __LINE__, stPlaybackGroupProbeInfo.video_pid);
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] audio_codec = %d\n", __func__, __LINE__, stPlaybackGroupProbeInfo.audio_codec);
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] video_codec = %d\n", __func__, __LINE__, stPlaybackGroupProbeInfo.video_codec);

    hController = AVController::getInstance();
    if(hController == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
        bRet = false;
        goto func_exit;
    }

    bRet = hController->setProbeInfo(&stPlaybackGroupProbeInfo);
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Setting probe information failed\n", __func__, __LINE__);
        bRet = false;
        goto func_exit;
    }

func_exit:
    return bRet;
}

bool PlaybackGroupNative::csoResume()
{
    bool bRet = true;
    AVController *hController = NULL;
    CSO_ERR csoErr = CSO_ERR_OK;
    CSO_CTRL_PARAM_t cso_ctrl_info;

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
    
    csoErr = CSO_UVA_Control(m_session_id, eCSO_CTRL_RESUME, &cso_ctrl_info);
    if(csoErr != CSO_ERR_OK)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to resume, csoErr = %d\n", __func__, __LINE__, csoErr);
        bRet = false;
        goto func_exit;
    }

    hController = AVController::getInstance();
    if(hController == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
        goto func_exit;
    }

    bRet = hController->resume();
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to resume in OCTO\n", __func__, __LINE__);
        goto func_exit;
    }
    if (1) (void)NativeBridge::playbackGroup().informLastPlayCmd(NativeBridge_PLAYCMD_PLAY);
    
func_exit:
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
    return bRet;
}

bool PlaybackGroupNative::getResourceMap()
{ 
    bool bRet = true;
    ESManagerNativeResourceMap_t *pstResourceMap = NULL;
    
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

    pstResourceMap = mESManager.getResourceMap();
    if(pstResourceMap == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] pstResourceMap is NULL\n", __func__, __LINE__); 
        bRet = false;
        goto func_exit;
    }

    m_stResourceMap.uiNumOfPlaypump = pstResourceMap->uiNumOfPlaypump;
    m_stResourceMap.hPlaypumpList = pstResourceMap->hPlaypumpList;
    m_stResourceMap.uiNumOfDma = pstResourceMap->uiNumOfDma;
    m_stResourceMap.hDmaList = pstResourceMap->hDmaList;
    
func_exit:
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
    return bRet;
}

void PlaybackGroupNative::onCsoEventCallback(unsigned int session_id, int evt, long long int param)
{
    PlaybackGroupNative *hPlaybackGroup = NULL;
    uint32_t unTryToChangeAudioTrack = 0;
    
    session_id = session_id; param = param;
    
    MutexLock();
    /* need a mutex to sync */
    hPlaybackGroup = getPlaybackGroup();
    if(hPlaybackGroup == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hPlaybackGroup is NULL\n", __func__, __LINE__);
        goto func_exit;
    }

    switch(evt)
    {
        case CSO_EVENT_MEDIA_SEEK_COMPLETE:
            hPlaybackGroup->setWaitForSeekFlag(true);
            break;
        case CSO_EVENT_MEDIA_NEEDMOREDATA:
            break;
        case CSO_EVENT_MEDIA_ENDOFSTREAM:
            onCsoEventEndOfStream();
            break;
        case CSO_EVENT_MEDIA_AUDIO_FLUSH:
            unTryToChangeAudioTrack = hPlaybackGroup->getCurrentAudioTrack();
            onCsoEventAudioFlush(unTryToChangeAudioTrack);
            break;
        case CSO_EVENT_MEDIA_AUDIO_RESTART:
            onCsoEventAudioRestart();
            break;
        case CSO_EVENT_PROBE_COMPLETE:
            onCsoEventProbeComplete();
            break;
        default:
            break;
    }

func_exit:
    MutexUnlock();
    return;
}

void PlaybackGroupNative::onCsoEventProbeComplete()
{
    AVController *hController = NULL;
    bool bRet = true;

    AVController::MutexLock();

    hController = AVController::getInstance();
    if(hController == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
        goto func_exit;
    }

    bRet = hController->probeComplete();
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to set a probe status\n", __func__, __LINE__);
        goto func_exit;
    }

func_exit:
    AVController::MutexUnlock();
    return;
}

void PlaybackGroupNative::onCsoEventAudioRestart()
{
    AVController *hController = NULL;
    bool bRet = true;

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

    AVController::MutexLock();

    hController = AVController::getInstance();
    if(hController == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
        goto func_exit;        
    }

    /* parkjh4 : Need to attach audiomixer */
    getPlaybackGroup()->getESManager()->detachMixerOutput();
    
    bRet = hController->audioRestart();
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to restart an audio decoder\n", __func__, __LINE__);
        goto func_exit;    
    }
    
func_exit:
    AVController::MutexUnlock();
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
    return;
}

void PlaybackGroupNative::onCsoEventAudioFlush(uint32_t unTryToChangeAudioTrack)
{
    AVController *hController = NULL;
    bool bRet = true;

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

    /* Octo Audio Flush */
    AVController::MutexLock();

    hController = AVController::getInstance();
    if(hController == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
        goto func_exit;
    }

	/* parkjh4 : Pause mixer before audio flush */
	getPlaybackGroup()->getESManager()->pauseMixerTransfer();

    bRet = hController->audioFlush(unTryToChangeAudioTrack);
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to flush an audio decoder buffer\n", __func__, __LINE__);
        goto func_exit;
    }
    
func_exit:
    AVController::MutexUnlock();

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
    return;
}

void PlaybackGroupNative::onCsoEventEndOfStream()
{
    bool bRet = true;
    AVController *hController = NULL;
    
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
    
    AVController::MutexLock();

    hController = AVController::getInstance();
    if(hController == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
        goto func_exit;
    }

    /* Set the current PTS. It enables OCTO to fire a EOS event */
    bRet = hController->setDuration();
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to complete playback\n", __func__, __LINE__);
        goto func_exit;
    }
    
func_exit:
    AVController::MutexUnlock();
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
    return;
}

void PlaybackGroupNative::onOctoEventCallback(void *handle, int nEventType)
{
    AVCtrl_EventType_e eEventType = AVCTRL_EVENT_TYPE_Unknown;

    eEventType = (AVCtrl_EventType_e)nEventType;
    
    switch(eEventType)
    {
        case AVCTRL_EVENT_TYPE_Started: // TODO : check this!
            break;
        case AVCTRL_EVENT_TYPE_PlaytimeChanged:
            onPlaytimeChanged(handle);
            break;
        case AVCTRL_EVENT_TYPE_PlayStateChanged:
            onPlayStateChanged(handle);
            break;
        case AVCTRL_EVENT_TYPE_PumpIndexChanged:
            onPlaypumpIndexChanged(handle);
#if 1            
            getPlaybackGroup()->getESManager()->resumeMixerTransfer();
#endif
            break;
        case AVCTRL_EVENT_TYPE_SeekReady:
            onSeekReady(handle);
            break;
        case AVCTRL_EVENT_TYPE_AudioCodecSwitched:
            onAudioCodecSwitched(handle);
            break;
#if 1             
        case AVCTRL_EVENT_TYPE_Stopped:
            getPlaybackGroup()->getESManager()->attachMixerOutput();
            getPlaybackGroup()->getESManager()->resumeMixerTransfer();
            getPlaybackGroup()->m_bPlaybackStopped = true;
#endif  
            break;
		case AVCTRL_EVENT_TYPE_BufferStateChanged:
			onBufferStateChanged(handle);
			break;
		case AVCTRL_EVENT_TYPE_AudioFadeCompleted:
			onAudioFadeCompleted(handle);
			break;
        case AVCTRL_EVENT_TYPE_AudioFlushCompleted:
            onAudioFlushCompleted(handle);
            break;
        case AVCTRL_EVENT_TYPE_AudioRestartCompleted:
            onAudioRestartCompleted(handle);
            break;
		case AVCTRL_EVENT_TYPE_AudioFadeFailed:
			onAudioFadeFailed(handle);
			break;
		case AVCTRL_EVENT_TYPE_AudioDecoderVolumeChanged:
			onAudioDecoderVolumeChanged(handle);
			break;	           
        default:
            break;
    }

    return;
}

void PlaybackGroupNative::onSeekReady(void *handle)
{
    AVController *hController = NULL;
    bool bRet = true;
    PlaybackGroupNative *hPlaybackGroup = NULL;
    uint32_t unTryToChangeAudioTrack = 0;
    
    if(handle == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] handle is NULL\n", __func__, __LINE__);
        goto func_exit;        
    }

    hController = static_cast<AVController *>(handle);
    if(hController == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
        goto func_exit;    
    }

    bRet = hController->seekReady();
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] seekReady() returned false\n", __func__, __LINE__);
        goto func_exit;
    }

    MutexLock();

    /* need a mutex to sync */
    hPlaybackGroup = getPlaybackGroup();
    if(hPlaybackGroup == NULL)
    {
        MutexUnlock();
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hPlaybackGroup is NULL\n", __func__, __LINE__);
        goto func_exit;
    }
    
    unTryToChangeAudioTrack = hPlaybackGroup->getCurrentAudioTrack();

    MutexUnlock();

    bRet = hController->checkAudioFlush(unTryToChangeAudioTrack);
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to check if user tries to change audio track.\n", __func__, __LINE__);
        goto func_exit;
    }
    
func_exit:
    return;
}

void PlaybackGroupNative::onAudioCodecSwitched(void *handle)
{
    AVController *hController = NULL;
    bool bRet = true;
    
    if(handle == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] handle is NULL\n", __func__, __LINE__);
        goto func_exit;        
    }

    hController = static_cast<AVController *>(handle);
    if(hController == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
        goto func_exit;    
    }

    bRet = hController->audioCodecSwitched();
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] audioCodecSwitched() returned false\n", __func__, __LINE__);
        goto func_exit;    
    }

func_exit:
    return;
}

void PlaybackGroupNative::onPlaytimeChanged(void *handle)
{
    AVController *hController = NULL;
    uint32_t unPosition = 0;
    bool bRet = true;
    
    if(handle == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] handle is NULL\n", __func__, __LINE__);
        goto func_exit;
    }

    hController = static_cast<AVController *>(handle);
    if(hController == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
        goto func_exit;
    }

    bRet = hController->getPlayPosition(&unPosition);
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to get a playing position\n", __func__, __LINE__);
        goto func_exit;
    }
    
    bRet = hController->updatePlayPosition(unPosition);
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to update playing position\n", __func__, __LINE__);
        goto func_exit;
    }

func_exit:
    return;
}

void PlaybackGroupNative::onPlayStateChanged(void *handle)
{
    AVController *hController = NULL;
    bool bRet = true;

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

    if(handle == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] handle is NULL\n", __func__, __LINE__);        
        goto func_exit;
    }

    hController = static_cast<AVController *>(handle);
    if(hController == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);        
        goto func_exit;
    }

    bRet = hController->procMediaCtrlPlayState();
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to process OCTO play states\n", __func__, __LINE__);        
        goto func_exit;
    }
    
func_exit:
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
    return;
}

void PlaybackGroupNative::onPlaypumpIndexChanged(void *handle)
{
    PlaybackGroupNative *hPlaybackGroup = NULL;
    AVController *hController = NULL;
    uint32_t unIndex = 0;
    bool bRet = true;
    CSO_ERR csoErr = CSO_ERR_OK;
	ESPlayerNative *pAudioPlayer = NULL;
	set<ESPlayerNative *>::iterator it;

    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

    if(handle == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] handle is NULL\n", __func__, __LINE__);
        goto func_exit;
    }

    hController = static_cast<AVController *>(handle);
    if(hController == NULL)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
        goto func_exit;
    }

    bRet = hController->getPumpIndex(&unIndex);
    if(!bRet)
    {
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to get a playpump index\n", __func__, __LINE__);
        goto func_exit;
    }

    MutexLock();
    /* need a mutex to sync */
    hPlaybackGroup = getPlaybackGroup();
    if(hPlaybackGroup == NULL)
    {
        MutexUnlock();
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hPlaybackGroup is NULL\n", __func__, __LINE__);
        goto func_exit;
    }

    hPlaybackGroup->setPumpIndex(unIndex);
    
    csoErr = CSO_UVA_Start(hPlaybackGroup->m_session_id, unIndex);
    if(csoErr != CSO_ERR_OK)
    {
        MutexUnlock();
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to start UVA, csoErr = %d\n", __func__, __LINE__, csoErr);
        goto func_exit;
    }

	/* 	Fix codes for NTS AUDIO-026-TCs.
	AUDIO-026-TCs fail when NTS tries to change audio track before ready. 
	In this case, we need to change audio track after CSO_UVA_Start() is called	*/
	if(hPlaybackGroup->getNeedToFlushAudio())
	{
		for(it = hPlaybackGroup->mStreamPlayers.begin(); it != hPlaybackGroup->mStreamPlayers.end(); it++)
		{
			if((*it)->getMediaType() == AUDIO)
			{
				pAudioPlayer = *it;
				break;
			}
		}
		
		if(pAudioPlayer != NULL)
		{
			pAudioPlayer->flush();
			hPlaybackGroup->setNeedToFlushAudio(false);
		}
	}
	
    MutexUnlock();

func_exit:
    NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
    return;
}

void PlaybackGroupNative::onBufferStateChanged(void *handle)
{
	AVController *hController = NULL;
	bool bRet = true;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	if(handle == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] handle is NULL\n", __func__, __LINE__);        
        goto func_exit;
	}

	hController = static_cast<AVController *>(handle);
	if(hController == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
		goto func_exit;
	}

	bRet = hController->procMediaCtrlBufferState();
	if(!bRet)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] Failed to process OCTO play states\n", __func__, __LINE__);
		goto func_exit;
	}
	
func_exit:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return;
}

void PlaybackGroupNative::onAudioFadeCompleted(void *handle)
{
	AVController *hController = NULL;
	bool bRet = true;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	if(handle == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] handle is NULL\n", __func__, __LINE__);        
        goto func_exit;
	}

	hController = static_cast<AVController *>(handle);
	if(hController == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
		goto func_exit;
	}

	bRet = hController->audioFadeCompleted();
	if(!bRet)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] audioFadeCompleted() returned FALSE\n", __func__, __LINE__);
		goto func_exit;
	}
	
func_exit:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return;	
}

void PlaybackGroupNative::onAudioFlushCompleted(void *handle)
{
	AVController *hController = NULL;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
    
	if(handle == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] handle is NULL\n", __func__, __LINE__);        
        goto func_exit;
	}

	hController = static_cast<AVController *>(handle);
	if(hController == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
		goto func_exit;
	}

	/* parkjh4 : Need to attach audio */
	getPlaybackGroup()->getESManager()->attachMixerOutput();
	getPlaybackGroup()->getESManager()->resumeMixerTransfer();

func_exit:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return;	
}

void PlaybackGroupNative::onAudioRestartCompleted(void *handle)
{
	AVController *hController = NULL;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
    
	if(handle == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] handle is NULL\n", __func__, __LINE__);        
        goto func_exit;
	}

	hController = static_cast<AVController *>(handle);
	if(hController == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
		goto func_exit;
	}

	getPlaybackGroup()->getESManager()->resumeMixerTransfer();

func_exit:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return;	
}

void PlaybackGroupNative::onAudioFadeFailed(void *handle)
{
	AVController *hController = NULL;
	bool bRet = true;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	if(handle == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] handle is NULL\n", __func__, __LINE__);        
        goto func_exit;
	}

	hController = static_cast<AVController *>(handle);
	if(hController == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
		goto func_exit;
	}

	bRet = hController->audioFadeFailed();
	if(!bRet)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] onAudioFadeFailed() returned FALSE\n", __func__, __LINE__);
		goto func_exit;
	}
	
func_exit:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return;	
}

void PlaybackGroupNative::onAudioDecoderVolumeChanged(void *handle)
{
	AVController *hController = NULL;
	bool bRet = true;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	if(handle == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] handle is NULL\n", __func__, __LINE__); 	   
		goto func_exit;
	}

	hController = static_cast<AVController *>(handle);
	if(hController == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] hController is NULL\n", __func__, __LINE__);
		goto func_exit;
	}

	bRet = hController->audioDecoderVolumeChanged();
	if(!bRet)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] onAudioDecoderVolumeChanged() returned FALSE\n", __func__, __LINE__);
		goto func_exit;
	}
	
func_exit:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [PlaybackGroupNative::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return;
}

PlaybackGroupNative* PlaybackGroupNative::getPlaybackGroup()
{
    return s_hPlaybackGroup;
}

bool PlaybackGroupNative::getWaitForSeekFlag()
{
    ScopedMutex cs(mWaitForEventMutex);
    return s_bWaitForSeek;
}

void PlaybackGroupNative::setWaitForSeekFlag(bool bFlag)
{
    ScopedMutex cs(mWaitForEventMutex);
    s_bWaitForSeek = bFlag;
}

void PlaybackGroupNative::setPumpIndex(uint32_t unIndex)
{
    m_unPumpIndex = unIndex;
}

uint32_t PlaybackGroupNative::getPumpIndex()
{
    return m_unPumpIndex;
}

void PlaybackGroupNative::setCurrentAudioTrack(uint32_t unCurrentAudioTrack)
{
    m_unCurrentAudioTrack = unCurrentAudioTrack;
}

uint32_t PlaybackGroupNative::getCurrentAudioTrack()
{
    return m_unCurrentAudioTrack;
}

void PlaybackGroupNative::setNeedToFlushAudio(bool bNeedToFlushAudio)
{
    m_bNeedToFlushAudio = bNeedToFlushAudio;
}

bool PlaybackGroupNative::getNeedToFlushAudio()
{
    return m_bNeedToFlushAudio;
}

