#include "AVStatusManager.h"
#include "DeviceLibNative.h"

#include <nrd/IPlaybackDevice.h>
#include <nrdbase/ScopedMutex.h>
#include <nrdbase/MutexRanks.h>

#include "cso_def.h"
#include "cso_uva.h"

#define AUDIOEASE_DEFAULT_VOLUME	(1.0)

using namespace netflix;
using namespace netflix::device::esplayer;
using namespace netflix::device;
using namespace std;

AVStatusManager* AVStatusManager::s_hStatusManager = NULL;
pthread_mutex_t	AVStatusManager::s_mtxStatusManager;

AVStatusManager::AVStatusManager()
	:	m_bSetAudioSourceBuffer(false),
		m_bSetVideoSourceBuffer(false),
		m_eFlushStatus(AVStatusMgr_FlushStatus_None),
		m_bAudioFlushStatus(false),
		m_bAudioCodecSwitchStatus(false),
		m_ePlaybackStatus(AVStatusMgr_PlaybackStatus_Stop),
		m_eBufferStatus(AVStatusMgr_BufferStatus_None),
		m_eProbeStatus(AVStatusMgr_ProbeStatus_None),
		m_bEndOfStreamStatus(false),
		m_bCsoAudioFlushStatus(false),
		m_dblCurrentVolume(AUDIOEASE_DEFAULT_VOLUME),
		m_dblTargetVolume(AUDIOEASE_DEFAULT_VOLUME),
		m_bIsFadeFailed(false),
		m_eDecodingStatus(AVStatusMgr_DecodingStatus_None)
{
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
}

AVStatusManager::~AVStatusManager()
{
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	MutexLock();
#if 0	/* For debugging */
	std::list<stEaseEntry_t>::iterator i;
	for(i = m_listEaseEntry.begin(); i != m_listEaseEntry.end(); i++)
	{
		printf("\033[1;31m @@@@@ [%s:%d] dblStartVolume = %lf\033[0m\n", __func__, __LINE__, i->dblStartVolume);
		printf("\033[1;31m @@@@@ [%s:%d] dblTargetVolume = %lf\033[0m\n", __func__, __LINE__, i->dblTargetVolume);
		printf("\033[1;31m @@@@@ [%s:%d] eEase = %d\033[0m\n", __func__, __LINE__, i->eEase);
		printf("\033[1;31m @@@@@ [%s:%d] nEaseTimeElapsed = %d\033[0m\n", __func__, __LINE__, i->nEaseTimeElapsed);
		printf("\033[1;31m @@@@@ [%s:%d] nEaseTransitionFrames = %d\033[0m\n", __func__, __LINE__, i->nEaseTransitionFrames);
	}
#endif
	m_listEaseEntry.clear();
	s_hStatusManager = NULL;
	MutexUnlock();
	pthread_mutex_destroy(&s_mtxStatusManager);

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
}

void AVStatusManager::setVideoSourceBufferFlag(bool bFlag)
{
	/* need a mutex to sync */
	m_bSetVideoSourceBuffer = bFlag;
}

void AVStatusManager::setAudioSourceBufferFlag(bool bFlag)
{
	/* need a mutex to sync */
	m_bSetAudioSourceBuffer = bFlag;
}

bool AVStatusManager::getVideoSourceBufferFlag()
{
	/* need a mutex to sync */
	return m_bSetVideoSourceBuffer;
}

bool AVStatusManager::getAudioSourceBufferFlag()
{
	/* need a mutex to sync */
	return m_bSetAudioSourceBuffer;
}


AVStatusMgr_PlaybackStatus_e AVStatusManager::getPlaybackStatus()
{
	/* need a mutex to sync */
	return m_ePlaybackStatus;
}

void AVStatusManager::setPlaybackStatus(AVStatusMgr_PlaybackStatus_e ePlayStatus)
{
	/* need a mutex to sync */
	m_ePlaybackStatus = ePlayStatus;	/* 0: PAUSE, 1: PLAY */
}

AVStatusMgr_FlushStatus_e AVStatusManager::getFlushStatus()
{
	return m_eFlushStatus;
}

void AVStatusManager::setFlushStatus(AVStatusMgr_FlushStatus_e eFlushStatus)
{
	m_eFlushStatus = eFlushStatus;
}

AVStatusMgr_ProbeStatus_e AVStatusManager::getProbeStatus()
{
	return m_eProbeStatus;
}

void AVStatusManager::setProbeStatus(AVStatusMgr_ProbeStatus_e eProbeStatus)
{
	m_eProbeStatus = eProbeStatus;
}

bool AVStatusManager::getAudioFlushStatus()
{
	return m_bAudioFlushStatus;
}

void AVStatusManager::setAudioFlushStatus(bool bAudioFlushStatus)
{
	m_bAudioFlushStatus = bAudioFlushStatus;
}

void AVStatusManager::setAudioCodecSwitchStatus(bool bAudioCodecSwitchStatus)
{
	m_bAudioCodecSwitchStatus = bAudioCodecSwitchStatus;
}

bool AVStatusManager::getAudioCodecSwitchStatus()
{
	return m_bAudioCodecSwitchStatus;
}

bool AVStatusManager::updatePlayPosition(uint32_t unPosition)
{
	bool bRet = true, bEndOfStreamStatus = false;
	shared_ptr<IESPlayerCallback> pCallback;
 	VideoESPlayerNative *hVideoPlayer = NULL;
	uint32_t unPrePosition = 0;
	AVStatusMgr_BufferStatus_e eBufferStatus = AVStatusMgr_BufferStatus_None;
	AVStatusMgr_FlushStatus_e eFlushStatus = AVStatusMgr_FlushStatus_None;
	
	//NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	/* If EndOfStream Status is set to TRUE, updating play position should be skipped */
	bEndOfStreamStatus = getEndOfStreamStatus();
	if(bEndOfStreamStatus)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] bEndOfStreamStatus has been set to %d, so do not update play postion\n", __func__, __LINE__, bEndOfStreamStatus);
		goto FUNCTION_EXIT;	
	}

	eFlushStatus = getFlushStatus();
	if(eFlushStatus != AVStatusMgr_FlushStatus_None)
	{
		if(unPosition == 0)
		{
			Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] Player is flushing now and unPosition is %d, so skip updating position\n", __func__, __LINE__, unPosition);
			goto FUNCTION_EXIT;
		}
		else	/* Player finished to flush and restarted to play the content */
		{
			unPrePosition = getPrePlayPosition();
			if((unPrePosition - 3000 < unPosition) && (unPosition < unPrePosition + 3000))
			{
				Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] Player is flushing now, unPrePosition = %d, unPosition = %d : unPosition has wrong value, so skip updating position\n", __func__, __LINE__, unPosition, unPrePosition);
				goto FUNCTION_EXIT;
			}

			Log::info(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] Before setFlushStatus(AVStatusMgr_FlushStatus_None), POS = %d\n", __func__, __LINE__, unPosition);
			eFlushStatus = AVStatusMgr_FlushStatus_None;
			setFlushStatus(eFlushStatus);
		}
	}

	unPrePosition = getPrePlayPosition();
	if(unPrePosition == unPosition)
	{
		eBufferStatus = getBufferStatus();
		if(eBufferStatus == AVStatusMgr_BufferStatus_NotEnoughBuffer)
		{
			/* If state is underflow, skip updating position */
			Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] unPrePosition(%d) is same as unPosition(%d), so skip updating position\n", __func__, __LINE__, unPrePosition, unPosition);
			goto FUNCTION_EXIT;
		}
	}

	VideoESPlayerNative::MutexLock();
	hVideoPlayer = VideoESPlayerNative::getVideoPlayer();
	if(hVideoPlayer == NULL)
	{
		VideoESPlayerNative::MutexUnlock();
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] hVideoPlayer is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	pCallback = hVideoPlayer->getCallback();
	if(pCallback == NULL)
	{
		VideoESPlayerNative::MutexUnlock();
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] pCallback is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;		
	}

	//Log::info(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] unPosition = %d\n", __func__, __LINE__, unPosition);
	pCallback->updatePlaybackPosition((int64_t)unPosition);
	VideoESPlayerNative::MutexUnlock();

	setPrePlayPosition(unPosition);

FUNCTION_EXIT:	
	//NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
	return bRet;
}

uint32_t AVStatusManager::getPrePlayPosition()
{
	return m_unPrePlayPosition;
}

void AVStatusManager::setPrePlayPosition(uint32_t unPrePlayPosition)
{
	m_unPrePlayPosition = unPrePlayPosition;
}

AVStatusMgr_BufferStatus_e AVStatusManager::getBufferStatus()
{
	return m_eBufferStatus;
}

void AVStatusManager::setBufferStatus(AVStatusMgr_BufferStatus_e eBufferStatus)
{
	m_eBufferStatus = eBufferStatus;
}

bool AVStatusManager::sendReportUnderflow()
{
	bool bRet = true;
	VideoESPlayerNative *hVideoPlayer = NULL;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	VideoESPlayerNative::MutexLock();

	hVideoPlayer = VideoESPlayerNative::getVideoPlayer();
	if(hVideoPlayer == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] hVideoPlayer is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	hVideoPlayer->underflowReporter();
	
FUNCTION_EXIT:
	VideoESPlayerNative::MutexUnlock();
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVStatusManager::sendPlaybackComplete()
{
	bool bRet = true, bEndOfStreamStatus = false;
	AudioESPlayerNative *hAudioPlayer = NULL;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	/* NRD Lib handles and reports AUDIO EOS only (REF: PlaybackDevice::playbackCompleteReported()) */

	AudioESPlayerNative::MutexLock();

	hAudioPlayer = AudioESPlayerNative::getAudioPlayer();
	if(hAudioPlayer == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] hAudioPlayer is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	hAudioPlayer->underflowReporter();

	/* Set EndOfStream Status */
	bEndOfStreamStatus = getEndOfStreamStatus();
	if(!bEndOfStreamStatus)
	{
		bEndOfStreamStatus = true;
		setEndOfStreamStatus(bEndOfStreamStatus);
	}
	
FUNCTION_EXIT:
	AudioESPlayerNative::MutexUnlock();
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVStatusManager::getLastCurrentAudioPts(double *pCurAudioPts)
{
	bool bRet = true;
	double dblLastCurrentAudioPts = 0.0;
	AudioESPlayerNative *hAudioPlayer = NULL;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);
	
	AudioESPlayerNative::MutexLock();

	hAudioPlayer = AudioESPlayerNative::getAudioPlayer();
	if(hAudioPlayer == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] hAudioPlayer is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	dblLastCurrentAudioPts = hAudioPlayer->getLastCurrentAudioPts();
	
FUNCTION_EXIT:
	AudioESPlayerNative::MutexUnlock();
	*pCurAudioPts = dblLastCurrentAudioPts;
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVStatusManager::getLastCurrentVideoPts(double *pCurVideoPts)
{
	bool bRet = true;
	double dblLastCurrentVideoPts = 0.0;
	VideoESPlayerNative *hVideoPlayer = NULL;

	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	VideoESPlayerNative::MutexLock();

	hVideoPlayer = VideoESPlayerNative::getVideoPlayer();
	if(hVideoPlayer == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] hVideoPlayer is NULL\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}

	dblLastCurrentVideoPts = hVideoPlayer->getLastCurrentVideoPts();

FUNCTION_EXIT:
	VideoESPlayerNative::MutexUnlock();
	*pCurVideoPts = dblLastCurrentVideoPts;
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

bool AVStatusManager::judgeUnderflow()
{
	bool bRet = true, bAudioFlushStatus = false, bAudioCodecSwitchStatus = false;
	AVStatusMgr_BufferStatus_e eBufferStatus = AVStatusMgr_BufferStatus_None;
	AVStatusMgr_FlushStatus_e eFlushStatus = AVStatusMgr_FlushStatus_None;
	AVStatusMgr_DecodingStatus_e eDecodingStatus = AVStatusMgr_DecodingStatus_None;
	
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (+)\n", __func__, __LINE__, __func__);

	eDecodingStatus = getDecodingStatus();
	if(eDecodingStatus != AVStatusMgr_DecodingStatus_Decoding)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] When a/v decoder is running, do not report underflow event! eDecodingStatus = %d\n", __func__, __LINE__, eDecodingStatus);
		bRet = false;
		goto FUNCTION_EXIT;
	}
	
	eBufferStatus = getBufferStatus();
	if(eBufferStatus == AVStatusMgr_BufferStatus_NotEnoughBuffer)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] Player status is already underflow, so skip reporting underflow\n", __func__, __LINE__);
		bRet = false;
		goto FUNCTION_EXIT;
	}
	else
	{
		eFlushStatus = getFlushStatus();
		if(eFlushStatus != AVStatusMgr_FlushStatus_None)
		{
		#if 0	/* When player is flushing, do not report underflow event. */
			eBufferStatus = AVStatusMgr_BufferStatus_NotEnoughBuffer;
			setBufferStatus(eBufferStatus);
		#endif
			bRet = false;
			goto FUNCTION_EXIT;
		}
		else
		{
			/* Switching audio */
			bAudioFlushStatus = getAudioFlushStatus();
			if(bAudioFlushStatus)	/* Player is flushing audio data in decoder buffer and source buffer */
			{
				Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] Player is flushing audio data in decoder buffer and source buffer\n", __func__, __LINE__);
				bRet = false;	/* do not report underflow */
				goto FUNCTION_EXIT;				
			}
		}

		bAudioCodecSwitchStatus = getAudioCodecSwitchStatus();
		if(bAudioCodecSwitchStatus)
		{
			Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] Player is switching an audio codec, so do not report underflow event\n", __func__, __LINE__);
			bRet = false;	/* do not report underflow */
			goto FUNCTION_EXIT;
		}

		/* Normal underflow case */
		eBufferStatus = AVStatusMgr_BufferStatus_NotEnoughBuffer;
		setBufferStatus(eBufferStatus);
	}
	
FUNCTION_EXIT:
	NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] %s() (-)\n", __func__, __LINE__, __func__);
	return bRet;
}

void AVStatusManager::setEndOfStreamStatus(bool bEndOfStreamStatus)
{
	m_bEndOfStreamStatus = bEndOfStreamStatus;
}

bool AVStatusManager::getEndOfStreamStatus()
{
	return m_bEndOfStreamStatus;
}

void AVStatusManager::setCsoAudioFlushStatus(bool bCsoAudioFlushStatus)
{
	m_bCsoAudioFlushStatus = bCsoAudioFlushStatus;
}

bool AVStatusManager::getCsoAudioFlushStatus()
{
	return m_bCsoAudioFlushStatus;
}

void AVStatusManager::setFade(double dblTargetVolume, uint32_t unDuration, AVStatusMgr_Ease_e eEase)
{
	stEaseEntry_t stEntry;
	double dblCurrentTargetVolume = 0.0;
	
	if(!getIsEntryEmpty())
	{
		if(m_listEaseEntry.size() > 30)
		{
			Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] Too many entries to push elements..\n", __func__, __LINE__);	
			goto FUNCTION_EXIT;
		}
		
		stEaseEntry_t &stBackEntry = m_listEaseEntry.back();
		if(stBackEntry.dblTargetVolume == dblTargetVolume)
		{
			Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] There are the same volume between before(%lf) and after(%lf)\n", __func__, __LINE__, stBackEntry.dblTargetVolume, dblTargetVolume);
			goto FUNCTION_EXIT;
		}
		else
		{
			if(stBackEntry.nEaseTransitionFrames > 0)
			{
				dblCurrentTargetVolume = getCurrentVolume();
			}
			else
			{
				dblCurrentTargetVolume = stBackEntry.dblTargetVolume;
			}
			
		}
	}
	else
	{
		dblCurrentTargetVolume = getTargetVolume();
		if(dblCurrentTargetVolume == dblTargetVolume)
		{
			Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] There are the same volume between before(%lf) and after(%lf).\n", __func__, __LINE__, dblCurrentTargetVolume, dblTargetVolume);
			goto FUNCTION_EXIT;
		}
	}

	if(unDuration)
	{
		stEntry.dblStartVolume = dblCurrentTargetVolume;
		stEntry.dblTargetVolume = dblTargetVolume;
		stEntry.eEase = eEase;
		stEntry.nEaseTimeElapsed = 0;
		/* do not multiply 48. We should only control decoder volume, not audio samples */
		//m_nEaseTransitionFrames = unDuration * 48;	/* 48k sample rate */
		stEntry.nEaseTransitionFrames = unDuration;		
	}
	else
	{
		stEntry.dblStartVolume = dblTargetVolume;
		stEntry.dblTargetVolume = dblTargetVolume;
		m_dblCurrentVolume = dblTargetVolume;	/* Fixed for NTS AUDIO-026-TCs */
		stEntry.eEase = eEase;
		stEntry.nEaseTimeElapsed = 0;
		stEntry.nEaseTransitionFrames = 0;
	}

	m_listEaseEntry.push_back(stEntry);

FUNCTION_EXIT:
	return;
}

bool AVStatusManager::getFadeValue(	double *pdblStartVolume,
									double *pdblTargetVolume,
									int32_t *pnEase,
									int32_t *pnEaseTimeElapsed,
									int32_t *pnEaseTransitionFrames)
{
	if(getIsEntryEmpty())
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] m_listEaseEntry is EMPTY!\n", __func__, __LINE__);
		return false;
	}

	if(pdblStartVolume == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] pdblStartVolume is NULL\n", __func__, __LINE__);
		return false;
	}
	if(pdblTargetVolume == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] pdblTargetVolume is NULL\n", __func__, __LINE__);
		return false;
	}
	if(pnEase == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] pnEase is NULL\n", __func__, __LINE__);
		return false;
	}
	if(pnEaseTimeElapsed == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] pnEaseTimeElapsed is NULL\n", __func__, __LINE__);
		return false;
	}
	if(pnEaseTransitionFrames == NULL)
	{
		Log::error(TRACE_MEDIAPLAYBACK, "@@@@@ [AVStatusManager::%s:%d] pnEaseTransitionFrames is NULL\n", __func__, __LINE__);
		return false;
	}
	
	stEaseEntry_t &stEntry = m_listEaseEntry.front();

	*pdblStartVolume = stEntry.dblStartVolume;
	*pdblTargetVolume = stEntry.dblTargetVolume;
	*pnEase = (int32_t)stEntry.eEase;
	*pnEaseTimeElapsed = stEntry.nEaseTimeElapsed;
	*pnEaseTransitionFrames = stEntry.nEaseTransitionFrames;

	return true;
}

void AVStatusManager::nextEaseEntry()
{
	if(!getIsEntryEmpty())
	{
		m_listEaseEntry.pop_front();
	}
}

double AVStatusManager::getCurrentVolume()
{
	return m_dblCurrentVolume;
}

void AVStatusManager::setCurrentVolume(double dblCurrentVolume)
{
	m_dblCurrentVolume = dblCurrentVolume;
}

double AVStatusManager::getTargetVolume()
{
	if(!getIsEntryEmpty())
	{
		stEaseEntry_t &stEntry = m_listEaseEntry.front();
		m_dblTargetVolume = stEntry.dblTargetVolume;
	}
	
	return m_dblTargetVolume;
}

void AVStatusManager::setIsFadeFailed(bool bIsFadeFailed)
{
	m_bIsFadeFailed = bIsFadeFailed;
}

bool AVStatusManager::getIsFadeFailed()
{
	return m_bIsFadeFailed;
}

bool AVStatusManager::getIsEntryEmpty()
{
	return m_listEaseEntry.empty();
}

void AVStatusManager::setDecodingStatus(AVStatusMgr_DecodingStatus_e eDecodingStatus)
{
	m_eDecodingStatus = eDecodingStatus;
}

AVStatusMgr_DecodingStatus_e AVStatusManager::getDecodingStatus()
{
	return m_eDecodingStatus;
}

