#include "AVController.h"
#include "AVStatusManager.h"
#include "DeviceLibNative.h"

#include <nrd/IPlaybackDevice.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/MutexRanks.h>

using namespace netflix;
using namespace netflix::device::esplayer;
using namespace netflix::device;
using namespace std;

AVController* 			AVController::s_hController = NULL;
pthread_mutex_t			AVController::s_mtxController;

typedef enum
{
	eOxMP_MEDIATYPE_NONE         = 0,
	eOxMP_MEDIATYPE_LIVE,
	eOxMP_MEDIATYPE_PVRPLAYBACK,
	eOxMP_MEDIATYPE_TSRPLAYBACK,
	eOxMP_MEDIATYPE_MEDIAPLAY,
	eOxMP_MEDIATYPE_ENDOFCASE
} OxMediaPlay_MediaType_e;

void AVController::onEventCallback(uint32_t ulViewId, uint32_t ulSessionId, uint32_t eType, uint32_t eEvent, void *pstNotifyData, void *pvUserData)
{
	/* (uint32_t ulViewId, uint32_t ulSessionId, OxMediaPlay_MediaType_e eType, OxMediaPlay_Event_e eEvent, OxMediaPlay_NotifierData_t *pstNotifyData, void *pvUserData) */
	AVController *hController = NULL; 
	AVCtrl_EventType_e eAVCtrlEventType = AVCTRL_EVENT_TYPE_Unknown;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	pstNotifyData = pstNotifyData;	pvUserData = pvUserData;
	
	MutexLock();
	
	/* Temporary code, modified by thoh */
	hController = TEMP_getInstanceEventCallback();
	if(hController == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hController is NULL\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}

	if (eType != eOxMP_MEDIATYPE_MEDIAPLAY)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] unmatched media type\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}
	
	eRet = NativeBridge::playbackGroup().getNotifyArgs(ulViewId, ulSessionId, eType, eEvent, &eAVCtrlEventType);
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to get notified arguments\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}

	switch(eAVCtrlEventType)
	{
		case AVCTRL_EVENT_TYPE_Started:
			break;
		case AVCTRL_EVENT_TYPE_Stopped:
			hController->m_pfEventCallback(hController, eAVCtrlEventType);           
			break;
		case AVCTRL_EVENT_TYPE_PlaytimeChanged:
			hController->m_pfEventCallback(hController, eAVCtrlEventType);
			break;
		case AVCTRL_EVENT_TYPE_PlayStateChanged:
			hController->m_pfEventCallback(hController, eAVCtrlEventType);
			break;
		case AVCTRL_EVENT_TYPE_PumpIndexChanged:
			hController->m_pfEventCallback(hController, eAVCtrlEventType);
			break;
		case AVCTRL_EVENT_TYPE_SeekReady:
			hController->m_pfEventCallback(hController, eAVCtrlEventType);
			break;
		case AVCTRL_EVENT_TYPE_AudioCodecSwitched:
			hController->m_pfEventCallback(hController, eAVCtrlEventType);
			break;
		case AVCTRL_EVENT_TYPE_BufferStateChanged:
			hController->m_pfEventCallback(hController, eAVCtrlEventType);
			break;
		case AVCTRL_EVENT_TYPE_AudioFadeCompleted:
			hController->m_pfEventCallback(hController, eAVCtrlEventType);
			break;
        case AVCTRL_EVENT_TYPE_AudioFlushCompleted:
            hController->m_pfEventCallback(hController, eAVCtrlEventType);
            break;
        case AVCTRL_EVENT_TYPE_AudioRestartCompleted:
            hController->m_pfEventCallback(hController, eAVCtrlEventType);
            break;
		case AVCTRL_EVENT_TYPE_AudioFadeFailed:
			hController->m_pfEventCallback(hController, eAVCtrlEventType);
			break;
		case AVCTRL_EVENT_TYPE_AudioDecoderVolumeChanged:
			hController->m_pfEventCallback(hController, eAVCtrlEventType);
			break;			
		default:
			break;
	}

FUNCTION_EXIT:	
	MutexUnlock();
	return;
}

bool AVController::setListener(pfAVCtrlListener pfListener)
{
	bool bRet = true;
	
	if(pfListener == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] pfListener is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	m_pfEventCallback = pfListener;

FUNCTION_EXIT:
	return bRet;
}

void AVController::EaseTask()
{
	static const Time WAIT_WHILE_IDLING(100);
	AVStatusManager *hStatusManager = NULL;
	double dblPriorVolume = 0.0, dblStartVolume = 0.0, dblTargetVolume = 0.0, dblBackupVolume = 0.0;
	int32_t nEase = -1, nEaseTimeElapsed = -1, nEaseTransitionFrames = -1;
	bool bRet = false, bIsFadeFailed = false, bIsEntryEmpty = false;
	
	AVStatusManager::MutexLock();
	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		AVStatusManager::MutexUnlock();
        Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
        goto FUNCTION_EXIT;
	}
	dblPriorVolume = hStatusManager->getCurrentVolume();
	AVStatusManager::MutexUnlock();
	
	while(closeEaseThreadFlagIsSet() == false)
	{
		AVStatusManager::MutexLock();

		bIsEntryEmpty = hStatusManager->getIsEntryEmpty();
		if(bIsEntryEmpty)	/* AudioEase entry is empty */
		{
			AVStatusManager::MutexUnlock();
			Thread::Sleep(WAIT_WHILE_IDLING);
			continue;
		}
		else
		{
			bIsFadeFailed = hStatusManager->getIsFadeFailed();
			if(bIsFadeFailed)	/* Failed to send fade message to OBAMA */
			{
				/* restore prior volume */
				if(nEaseTransitionFrames == 0)
				{
					dblPriorVolume = dblBackupVolume;
				}
				else
				{
					dblPriorVolume = hStatusManager->getCurrentVolume();
				}
				
				bIsFadeFailed = false;
				hStatusManager->setIsFadeFailed(bIsFadeFailed);
			}
			
			dblTargetVolume = hStatusManager->getTargetVolume();

			if(dblPriorVolume == dblTargetVolume)
			{
				/* Do not need to ease with this target volume */
				AVStatusManager::MutexUnlock();
				Thread::Sleep(WAIT_WHILE_IDLING);
				continue;
			}
			else	/* Try to ease */
			{
				bRet = hStatusManager->getFadeValue(&dblStartVolume, 
													&dblTargetVolume, 
													&nEase, 
													&nEaseTimeElapsed, 
													&nEaseTransitionFrames);
				AVStatusManager::MutexUnlock();
				if(!bRet)
				{
					Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to get Fade Value\n", __func__, __LINE__);
					Thread::Sleep(WAIT_WHILE_IDLING);
					continue;
				}

				/* Set Fade to OBAMA */
				NativeBridge::playbackGroup().setFade(	(uint32_t)(dblStartVolume * 100),
														(uint32_t)(dblTargetVolume * 100),
														nEase,
														nEaseTimeElapsed,
														nEaseTransitionFrames);

				dblBackupVolume = dblPriorVolume;
				dblPriorVolume = dblTargetVolume;

				Thread::Sleep(WAIT_WHILE_IDLING);
				continue;
			}
		}	
	}

FUNCTION_EXIT:
	return;
}

bool AVController::closeEaseThreadFlagIsSet()
{
	ScopedMutex cs(m_mtxEaseTask);
	return m_bRunning;
}

void AVController::setCloseEaseThreadFlag()
{
	ScopedMutex cs(m_mtxEaseTask);
	m_bRunning = true;
}

AVController::AVController()
	: m_mtxEaseTask(UNTRACKED_MUTEX, "AVControllerEaseThreadMutex"),
	  m_bRunning(false)
{
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
	
	eRet = NativeBridge::playbackGroup().registerNotifier((void *)onEventCallback);
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to register a notifier\n", __func__, __LINE__);
	}

	eRet = NativeBridge::playbackGroup().setPlayerType();
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to set player type, eRet = %d\n", __func__, __LINE__, eRet);
	}

	m_hEaseThread.reset(new AVControllerEaseThread(*this, &AVController::EaseTask, &THREAD_DPI_AVCONTROLLER_EASE));

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
}

AVController::~AVController()
{
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
	
#if 0	/* Temporary code, modified by thoh */
	eRet = NativeBridge::playbackGroup().unregisterNotifier((void *)onEventCallback);
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to unregister a notifier\n", __func__, __LINE__);
	}
#endif

	setCloseEaseThreadFlag();
	m_hEaseThread.reset();	// Thread destructors wait for threads to exit.
	
	MutexLock();
	s_hController = NULL;
	MutexUnlock();

	pthread_mutex_destroy(&s_mtxController);
	
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
}

bool AVController::setProbeInfo(PlaybackGroupProbeInfo_t *pstProbeInfo)
{
	bool bRet = true;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
	
	if(pstProbeInfo == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] pstProbeInfo is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	m_stProbeInfo.audio_pid = pstProbeInfo->audio_pid;
	m_stProbeInfo.video_pid = pstProbeInfo->video_pid;
	m_stProbeInfo.audio_codec = pstProbeInfo->audio_codec;
	m_stProbeInfo.video_codec = pstProbeInfo->video_codec;
	
FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::flush()
{
	bool bRet = true;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;
	uint32_t ulPlayTime = 0;
	AVStatusManager *hStatusManager = NULL;
	AVStatusMgr_FlushStatus_e eFlushStatus = AVStatusMgr_FlushStatus_None;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
		
	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		AVStatusManager::MutexUnlock();
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	eFlushStatus = hStatusManager->getFlushStatus();
	if(eFlushStatus != AVStatusMgr_FlushStatus_Flushing)
	{
		eFlushStatus = AVStatusMgr_FlushStatus_Flushing;
		hStatusManager->setFlushStatus(eFlushStatus);
	}
	
	AVStatusManager::MutexUnlock();
	
	/* Seek position(ulPlayTime) doesn't matter at this function */
	eRet = NativeBridge::playbackGroup().jump(ulPlayTime /* 0 */);
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to jump to position(%d)\n", __func__, __LINE__, ulPlayTime);
		bRet = false;
		goto FUNCTION_EXIT;
	}
	
FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::stop()
{
	bool bRet = true;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	eRet = NativeBridge::playbackGroup().mediaPbStop();
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to stop Media PB, eRet = %d\n", __func__, __LINE__, eRet);
		bRet = false;
		goto FUNCTION_EXIT;
	}
	
FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::play()
{
	bool bRet = true;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	eRet = NativeBridge::playbackGroup().mediaPbStart((void *)&m_stProbeInfo);
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to start Media PB, eRet = %d\n", __func__, __LINE__, eRet);
		bRet = false;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::pause()
{
	bool bRet = true;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
	
	bRet = setPlaySpeed(AVCTRL_PLAY_SPEED_PAUSE);
	if(!bRet)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Setting speed(AVCTRL_PLAY_SPEED_PAUSE) failed\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::resume()
{
	bool bRet = true;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
	
	bRet = setPlaySpeed(AVCTRL_PLAY_SPEED_NORMAL);
	if(!bRet)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Setting speed(AVCTRL_PLAY_SPEED_NORMAL) failed\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}
	
FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::audioRestart()
{
	bool bRet = true;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	eRet = NativeBridge::playbackGroup().audioRestart();
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to restart an audio decoder\n", __func__, __LINE__);	
		bRet = false;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::audioFlush(uint32_t unTryToChangeAudioTrack)
{
	bool bRet = true, bAudioFlushStatus = false, bCsoAudioFlushStatus = false;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;
	AVStatusManager *hStatusManager = NULL;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	eRet = NativeBridge::playbackGroup().audioFlush(unTryToChangeAudioTrack);
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to flush an audio decoder buffer, eRet = %d\n", __func__, __LINE__, eRet);	
		bRet = false;
		goto FUNCTION_EXIT;
	}

	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		AVStatusManager::MutexUnlock();
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	bCsoAudioFlushStatus = hStatusManager->getCsoAudioFlushStatus();
	if(bCsoAudioFlushStatus)
	{
		bCsoAudioFlushStatus = false;
		hStatusManager->setCsoAudioFlushStatus(bCsoAudioFlushStatus);
	}

	bAudioFlushStatus = hStatusManager->getAudioFlushStatus();
	if(!bAudioFlushStatus)
	{
		bAudioFlushStatus = true;
		hStatusManager->setAudioFlushStatus(bAudioFlushStatus);
	}

	AVStatusManager::MutexUnlock();

FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::checkAudioFlush(uint32_t unTryToChangeAudioTrack)
{
	bool bRet = true, bCsoAudioFlushStatus = false;
	AVStatusManager *hStatusManager = NULL;
	
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		AVStatusManager::MutexUnlock();
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	bCsoAudioFlushStatus = hStatusManager->getCsoAudioFlushStatus();
	AVStatusManager::MutexUnlock();

	if(bCsoAudioFlushStatus)
	{
		bRet = audioFlush(unTryToChangeAudioTrack);
		if(!bRet)
		{
			Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to flush an audio.\n", __func__, __LINE__);
			goto FUNCTION_EXIT;
		}
	}
	
FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::audioCodecSwitch(uint32_t unTryToChangeAudioTrack, int32_t nAudioCodec)
{
	bool bRet = true;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	eRet = NativeBridge::playbackGroup().audioCodecSwitch(unTryToChangeAudioTrack, nAudioCodec);
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to switch an audio codec, eRet = %d\n", __func__, __LINE__, eRet);
		bRet = false;
		goto FUNCTION_EXIT;		
	}

FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::getPumpIndex(uint32_t *punIndex)
{
	bool bRet = true;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;
	
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	if(punIndex == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] punIndex is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}
	eRet = NativeBridge::playbackGroup().getPumpIndex(punIndex);
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to get a playpump index, eRet = %d\n", __func__, __LINE__, eRet);
		bRet = false;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::setPlaySpeed(int32_t speed_sec)
{
	bool bRet = true;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
	
	eRet = NativeBridge::playbackGroup().setPlaySpeed(speed_sec);
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to set playing speed\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::getPlayPosition(uint32_t *punPosition)
{
	bool bRet = true;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;

	if(punPosition == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] punPosition is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}
	
	eRet = NativeBridge::playbackGroup().getPlayPosition(punPosition);
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to get a playing position, eRet = %d\n", __func__, __LINE__, eRet);
		bRet = false;
		goto FUNCTION_EXIT;
	}
	
FUNCTION_EXIT:
	return bRet;
}

bool AVController::updatePlayPosition(uint32_t unPosition)
{
	bool bRet = true;
	AVStatusManager *hStatusManager = NULL;
	
	//NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	AVStatusManager::MutexLock();
	
	/* Need a mutex to sync */
	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}	

	bRet = hStatusManager->updatePlayPosition(unPosition);
	if(!bRet)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to update playing position\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	
	//NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::procMediaCtrlPlayState()
{
	bool bRet = true;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;
	AVCtrl_MCtrlPlayState_e ePlayState = AVCTRL_MCTRL_PLAY_STATE_Stop;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
	
	eRet = NativeBridge::playbackGroup().getMediaCtrlPlayState((void *)&ePlayState);
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to get a MCtrl play state, eRet = %d\n", __func__, __LINE__, eRet);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	switch(ePlayState)
	{
	case AVCTRL_MCTRL_PLAY_STATE_Stop:
		{
			bRet = handlePlayStateStop();
			if(!bRet)
			{
				Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] handlePlayStateStop() returned failure\n", __func__, __LINE__);
				goto FUNCTION_EXIT;
			}
		}
		break;
	case AVCTRL_MCTRL_PLAY_STATE_Playing:
		{
			bRet = handlePlayStatePlaying();
			if(!bRet)
			{
				Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] handlePlayStatePlaying() returned failure\n", __func__, __LINE__);
				goto FUNCTION_EXIT;
			}
		}
		break;
	case AVCTRL_MCTRL_PLAY_STATE_Paused:
		{
			bRet = handlePlayStatePaused();
			if(!bRet)
			{
				Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] handlePlayStatePaused() returned failure\n", __func__, __LINE__);
				goto FUNCTION_EXIT;
			}
		}
		break;
	case AVCTRL_MCTRL_PLAY_STATE_EndOfStream:
		{
			bRet = handleEndOfStream();
			if(!bRet)
			{
				Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] handleEndOfStream() returned failure\n", __func__, __LINE__);
				goto FUNCTION_EXIT;
			}
		}
		break;
	case AVCTRL_MCTRL_PLAY_STATE_Error:
		break;
	case AVCTRL_MCTRL_PLAY_STATE_BufferingUnderflow:
		{
			bRet = handlePlayStateBufferingUnderflow();
			if(!bRet)
			{
				Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] handlePlayStateBufferingUnderflow() returned failure\n", __func__, __LINE__);
				goto FUNCTION_EXIT;
			}
		}
		break;
	default:
		break;
	}
	
FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::procMediaCtrlBufferState()
{
	bool bRet = true;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;
	AVCtrl_MCtrlBufferState_e eBufferState = AVCTRL_MCTRL_BUFFER_STATE_None;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	eRet = NativeBridge::playbackGroup().getMediaCtrlBufferState((void *)&eBufferState);
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to get a MCtrl buffer state, eRet = %d\n", __func__, __LINE__, eRet);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	switch(eBufferState)
	{
		/* ONLY USE AVCTRL_MCTRL_BUFFER_STATE_NotEnough */
		case AVCTRL_MCTRL_BUFFER_STATE_None:
		case AVCTRL_MCTRL_BUFFER_STATE_CanPlay:
		case AVCTRL_MCTRL_BUFFER_STATE_Enough:	/* Player resumes through AVCTRL_MCTRL_PLAY_STATE_Playing, so do not use AVCTRL_MCTRL_BUFFER_STATE_Enough */
		case AVCTRL_MCTRL_BUFFER_STATE_Buffering_Finished:
			break;
		case AVCTRL_MCTRL_BUFFER_STATE_NotEnough:
			{
				/* reuse below PlayState function to check underflow */
				bRet = handlePlayStateBufferingUnderflow();
				if(!bRet)
				{
					Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] handlePlayStateBufferingUnderflow() returned failure\n", __func__, __LINE__);
					goto FUNCTION_EXIT;
				}
			}
			break;
		default:
			break;
	}


FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::setDuration()
{
	bool bRet = true;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;
	AVStatusManager *hStatusManager = NULL;
	double dblCurrentAudioPts = 0.0, dblCurrentVideoPts = 0.0, dblCurrentMaxPts = 0.0;
	
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		AVStatusManager::MutexUnlock();
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	bRet = hStatusManager->getLastCurrentAudioPts(&dblCurrentAudioPts);
	if(!bRet)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to get a current audio PTS\n", __func__, __LINE__);
	}

	bRet = hStatusManager->getLastCurrentVideoPts(&dblCurrentVideoPts);
	if(!bRet)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to get a current video PTS\n", __func__, __LINE__);
	}

	AVStatusManager::MutexUnlock();

	Log::info(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] dblCurrentAudioPts = %lf\n", __func__, __LINE__, dblCurrentAudioPts);
	Log::info(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] dblCurrentVideoPts = %lf\n", __func__, __LINE__, dblCurrentVideoPts);
		
	dblCurrentMaxPts = (dblCurrentAudioPts > dblCurrentVideoPts) ? dblCurrentAudioPts : dblCurrentVideoPts;

	Log::info(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] dblCurrentMaxPts = %lf\n", __func__, __LINE__, dblCurrentMaxPts);

	eRet = NativeBridge::playbackGroup().setDuration(dblCurrentMaxPts);
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to set a duration, eRet = %d\n", __func__, __LINE__, eRet);
		bRet = false;
		goto FUNCTION_EXIT;
	}
	
FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::handleEndOfStream()
{
	bool bRet = true;
	AVStatusManager *hStatusManager = NULL;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	bRet = hStatusManager->sendPlaybackComplete();
	if(!bRet)
	{	
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to complete the playback\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}
	
FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::handlePlayStateStop()
{
	bool bRet = true;
	AVStatusManager *hStatusManager = NULL;
	AVStatusMgr_DecodingStatus_e eDecodingStatus = AVStatusMgr_DecodingStatus_None;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
	
	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	eDecodingStatus = hStatusManager->getDecodingStatus();
	if(eDecodingStatus != AVStatusMgr_DecodingStatus_NotDecoding)
	{
		eDecodingStatus = AVStatusMgr_DecodingStatus_NotDecoding;
		hStatusManager->setDecodingStatus(eDecodingStatus);
	}

FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}
bool AVController::handlePlayStatePaused()
{
	bool bRet = true;
	AVStatusManager *hStatusManager = NULL;
	AVStatusMgr_DecodingStatus_e eDecodingStatus = AVStatusMgr_DecodingStatus_None;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
	
	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	eDecodingStatus = hStatusManager->getDecodingStatus();
	if(eDecodingStatus != AVStatusMgr_DecodingStatus_NotDecoding)
	{
		eDecodingStatus = AVStatusMgr_DecodingStatus_NotDecoding;
		hStatusManager->setDecodingStatus(eDecodingStatus);
	}

FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}
bool AVController::handlePlayStatePlaying()
{
	bool bRet = true, bAudioFlushStatus = false;
	AVStatusManager *hStatusManager = NULL;
	AVStatusMgr_BufferStatus_e eBufferStatus = AVStatusMgr_BufferStatus_None;
	AVStatusMgr_FlushStatus_e eFlushStatus = AVStatusMgr_FlushStatus_None;
	AVStatusMgr_DecodingStatus_e eDecodingStatus = AVStatusMgr_DecodingStatus_None;
	
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	eFlushStatus = hStatusManager->getFlushStatus();
	if(eFlushStatus != AVStatusMgr_FlushStatus_None)
	{
		eFlushStatus = AVStatusMgr_FlushStatus_None;
		hStatusManager->setFlushStatus(eFlushStatus);
	}

	bAudioFlushStatus = hStatusManager->getAudioFlushStatus();
	if(bAudioFlushStatus)
	{
		bAudioFlushStatus = false;
		hStatusManager->setAudioFlushStatus(bAudioFlushStatus);
	}
	
	eBufferStatus = hStatusManager->getBufferStatus();
	if(eBufferStatus != AVStatusMgr_BufferStatus_EnoughBuffer)
	{
		eBufferStatus = AVStatusMgr_BufferStatus_EnoughBuffer;
		hStatusManager->setBufferStatus(eBufferStatus);
	}

	eDecodingStatus = hStatusManager->getDecodingStatus();
	if(eDecodingStatus != AVStatusMgr_DecodingStatus_Decoding)
	{
		eDecodingStatus = AVStatusMgr_DecodingStatus_Decoding;
		hStatusManager->setDecodingStatus(eDecodingStatus);
	}

FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::handlePlayStateBufferingUnderflow()
{
	bool bRet = true;
	AVStatusManager *hStatusManager = NULL;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	/* Judge if status is underflow or not */
	bRet = hStatusManager->judgeUnderflow();
	if(!bRet)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Status is not underflow, so skip reporting\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}
	
	bRet = hStatusManager->sendReportUnderflow();
	if(!bRet)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to report underflow\n", __func__, __LINE__);
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::seekReady()
{
	bool bRet = true;
	AVStatusManager *hStatusManager = NULL;
	AVStatusMgr_FlushStatus_e eFlushStatus = AVStatusMgr_FlushStatus_None;
	
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	eFlushStatus = hStatusManager->getFlushStatus();
	if(eFlushStatus != AVStatusMgr_FlushStatus_FlushCompleted)
	{
		eFlushStatus = AVStatusMgr_FlushStatus_FlushCompleted;
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] setFlushStatus(AVStatusMgr_FlushStatus_FlushCompleted)\n", __func__, __LINE__);
		hStatusManager->setFlushStatus(eFlushStatus);
	}

FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::audioCodecSwitched()
{
	bool bRet = true;
	AVStatusManager *hStatusManager = NULL;
	bool bAudioCodecSwitchStatus = false;
	
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	bAudioCodecSwitchStatus = hStatusManager->getAudioCodecSwitchStatus();
	if(bAudioCodecSwitchStatus)
	{
		bAudioCodecSwitchStatus = false;
		hStatusManager->setAudioCodecSwitchStatus(bAudioCodecSwitchStatus);
	}

FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::audioFadeCompleted()
{
	bool bRet = true;
	AVStatusManager *hStatusManager = NULL;
	
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
	
	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	hStatusManager->nextEaseEntry();

FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::audioFadeFailed()
{
	bool bRet = true, bIsFadeFailed = true;
	AVStatusManager *hStatusManager = NULL;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
		
	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	bIsFadeFailed = true;
	hStatusManager->setIsFadeFailed(bIsFadeFailed);
	
FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);

	return bRet;
}

bool AVController::audioDecoderVolumeChanged()
{
	bool bRet = true;
	NativeBridge_ERR_e eRet = NativeBridge_ERR_OK;
	AVStatusManager *hStatusManager = NULL;
	uint32_t unVolume = 0;
	double dblVolume = 0.0;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
		
	eRet = NativeBridge::playbackGroup().getAudioDecoderVolume(&unVolume);
	if(eRet != NativeBridge_ERR_OK)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Failed to get audio decoder volume, eRet = %d\n", __func__, __LINE__, eRet);
		bRet = false;
		goto FUNCTION_EXIT;
	}
	
	AVStatusManager::MutexLock();
	
	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		AVStatusManager::MutexUnlock();
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	dblVolume = ((double)unVolume) / 100.0;

	hStatusManager->setCurrentVolume(dblVolume);

	AVStatusManager::MutexUnlock();

FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVController::probeComplete()
{
	bool bRet = true;
	AVCtrl_ProbeStatus_e eProbeStatus = AVCtrl_ProbeStatus_None;

	eProbeStatus = getProbeStatus();
	if(eProbeStatus != AVCtrl_ProbeStatus_ProbeCompleted)
	{
		eProbeStatus = AVCtrl_ProbeStatus_ProbeCompleted;
		setProbeStatus(eProbeStatus);
	}
	else
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] Player has already probed.\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

FUNCTION_EXIT:
	return bRet;
}

void AVController::setPlaybackStatus(AVCtrl_PlaybackStatus_e ePlaybackStatus)
{
	AVStatusManager *hStatusManager = NULL;

	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{	
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		goto FUNCTION_EXIT;	
	}
	
	hStatusManager->setPlaybackStatus((AVStatusMgr_PlaybackStatus_e)ePlaybackStatus);
	
FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	return;
}
	
AVCtrl_PlaybackStatus_e AVController::getPlaybackStatus()
{
	AVCtrl_PlaybackStatus_e ePlaybackStatus = AVCtrl_PlaybackStatus_Stop;
	AVStatusManager *hStatusManager = NULL;

	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		goto FUNCTION_EXIT;		
	}
	
	ePlaybackStatus = (AVCtrl_PlaybackStatus_e) hStatusManager->getPlaybackStatus();

FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	return ePlaybackStatus;
}

void AVController::setProbeStatus(AVCtrl_ProbeStatus_e eProbeStatus)
{
	AVStatusManager *hStatusManager = NULL;

	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		goto FUNCTION_EXIT;	
	}
	
	hStatusManager->setProbeStatus((AVStatusMgr_ProbeStatus_e)eProbeStatus);
	
FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	return;
}

AVCtrl_ProbeStatus_e AVController::getProbeStatus()
{
	AVCtrl_ProbeStatus_e eProbeStatus = AVCtrl_ProbeStatus_None;
	AVStatusManager *hStatusManager = NULL;

	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		goto FUNCTION_EXIT;		
	}
	
	eProbeStatus = (AVCtrl_ProbeStatus_e) hStatusManager->getProbeStatus();

FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	return eProbeStatus;	
}

bool AVController::readyToResume()
{
	AVStatusMgr_FlushStatus_e eFlushStatus = AVStatusMgr_FlushStatus_None;
	AVStatusManager *hStatusManager = NULL;
	bool bRet = false;
	
	AVStatusManager::MutexLock();

	hStatusManager = AVStatusManager::getInstance();
	if(hStatusManager == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVController::%s:%d] hStatusManager is NULL\n", __func__, __LINE__);
		goto FUNCTION_EXIT;		
	}

	eFlushStatus = hStatusManager->getFlushStatus();
	if(eFlushStatus != AVStatusMgr_FlushStatus_Flushing)
	{
		bRet = true;
	}
	
FUNCTION_EXIT:
	AVStatusManager::MutexUnlock();
	return bRet;
}
