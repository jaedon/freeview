#ifndef AVSTATUSMANAGER_H
#define AVSTATUSMANAGER_H

#include <nrdbase/StdInt.h>
#include <nrdbase/NFErr.h>
#include <nrdbase/Log.h>

#include <list>

namespace netflix {
namespace device {
namespace esplayer {

/* 	Below enumeration(AVStatusMgr_PlaybackStatus_e) references AVController.h 
	If AVController.h changes, you should check below enumerations */
typedef enum
{
	AVStatusMgr_PlaybackStatus_Stop = 0,
	AVStatusMgr_PlaybackStatus_Pause,
	AVStatusMgr_PlaybackStatus_Playing,
}AVStatusMgr_PlaybackStatus_e;

typedef enum
{
	AVStatusMgr_BufferStatus_None = 0,
	AVStatusMgr_BufferStatus_NotEnoughBuffer,
	AVStatusMgr_BufferStatus_EnoughBuffer,
}AVStatusMgr_BufferStatus_e;

typedef enum
{
	AVStatusMgr_FlushStatus_None = 0,
	AVStatusMgr_FlushStatus_Flushing,
	AVStatusMgr_FlushStatus_FlushCompleted,
}AVStatusMgr_FlushStatus_e;

/* 	Below enumeration(AVStatusMgr_ProbeStatus_e) references AVController.h 
	If AVController.h changes, you should check below enumerations */
typedef enum
{
	AVStatusMgr_ProbeStatus_None = 0,
	AVStatusMgr_ProbeStatus_Probing,
	AVStatusMgr_ProbeStatus_ProbeCompleted,
}AVStatusMgr_ProbeStatus_e;

typedef enum
{
    EaseLinear = 0,
    EaseInCubic,
    EaseOutCubic,
    EaseCount	
}AVStatusMgr_Ease_e;

typedef enum
{
	AVStatusMgr_DecodingStatus_None = 0,
	AVStatusMgr_DecodingStatus_Decoding,
	AVStatusMgr_DecodingStatus_NotDecoding,
}AVStatusMgr_DecodingStatus_e;

class VideoESPlayerNative;
class AudioESPlayerNative;
class AVStatusManager
{
public:
	AVStatusManager();
	virtual ~AVStatusManager();

	static AVStatusManager* getInstance()
	{
		if(s_hStatusManager == NULL)
		{
			pthread_mutex_init(&s_mtxStatusManager, NULL);
			
			s_hStatusManager = new AVStatusManager();
			if(s_hStatusManager == NULL)
			{
				NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] Cannot construct AVStatusManager : s_hStatusManager is NULL\n", __func__, __LINE__);
			}
		}
		
		return s_hStatusManager; 
	}
	
	static void MutexLock()
	{
		int32_t nRet = 0;

		nRet = pthread_mutex_lock(&s_mtxStatusManager);
		if(nRet)
		{
			NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] Failed to lock s_mtxStatusManager, nRet = %d\n", __func__, __LINE__, nRet);
		}
	}
	
	static void MutexUnlock()
	{
		int32_t nRet = 0;
		
		nRet = pthread_mutex_unlock(&s_mtxStatusManager);
		if(nRet)
		{
			NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] Failed to unlock s_mtxStatusManager, nRet = %d\n", __func__, __LINE__, nRet);
		}
	}
	
	bool 							updatePlayPosition(uint32_t unPosition);
	void 							setVideoSourceBufferFlag(bool bFlag);
	void 							setAudioSourceBufferFlag(bool bFlag);
	bool 							getVideoSourceBufferFlag();
	bool 							getAudioSourceBufferFlag();
	AVStatusMgr_PlaybackStatus_e 	getPlaybackStatus();	
	void 							setPlaybackStatus(AVStatusMgr_PlaybackStatus_e ePlaybackStatus);
	AVStatusMgr_BufferStatus_e 		getBufferStatus();
	void 							setBufferStatus(AVStatusMgr_BufferStatus_e eBufferStatus);
	AVStatusMgr_ProbeStatus_e 		getProbeStatus();
	void 							setProbeStatus(AVStatusMgr_ProbeStatus_e eProbeStatus);
	bool 							sendReportUnderflow();
	bool							sendPlaybackComplete();
	bool 							getLastCurrentAudioPts(double *pCurAudioPts);
	bool							getLastCurrentVideoPts(double *pCurVideoPts);
	void 							setFlushStatus(AVStatusMgr_FlushStatus_e eFlushStatus);
	AVStatusMgr_FlushStatus_e		getFlushStatus();
	void 							setAudioFlushStatus(bool bAudioFlushStatus);
	bool 							getAudioFlushStatus();
	void							setAudioCodecSwitchStatus(bool bAudioCodecSwitchStatus);
	bool 							getAudioCodecSwitchStatus();
	bool 							judgeUnderflow();
	void							setEndOfStreamStatus(bool bEndOfStreamStatus);
	bool							getEndOfStreamStatus();
	void 							setCsoAudioFlushStatus(bool bCsoAudioFlushStatus);
	bool							getCsoAudioFlushStatus();
	void							setFade(double dblTargetVolume, uint32_t unDuration, AVStatusMgr_Ease_e eEase);
	bool 							getFadeValue( double *pdblStartVolume, double *pdblTargetVolume, int32_t *pnEase, int32_t *pnEaseTimeElapsed, int32_t *pnEaseTransitionFrames);
	void 							nextEaseEntry();
	double							getCurrentVolume();
	void 							setCurrentVolume(double dblCurrentVolume);
	double							getTargetVolume();
	void							setIsFadeFailed(bool bIsFadeFailed);
	bool							getIsFadeFailed();
	bool 							getIsEntryEmpty();
	void 							setDecodingStatus(AVStatusMgr_DecodingStatus_e eDecodingStatus);
	AVStatusMgr_DecodingStatus_e 	getDecodingStatus();
	
private:
	typedef struct _stEaseEntry_t
	{
		double dblStartVolume;
		double dblTargetVolume;
		AVStatusMgr_Ease_e eEase;
		int32_t nEaseTimeElapsed;
		int32_t nEaseTransitionFrames;
	}stEaseEntry_t;
	
	uint32_t 						getPrePlayPosition();
	void 							setPrePlayPosition(uint32_t unPrePlayPosition);
	
	bool 							m_bSetAudioSourceBuffer;
	bool 							m_bSetVideoSourceBuffer;
	AVStatusMgr_FlushStatus_e		m_eFlushStatus;
	bool							m_bAudioFlushStatus;
	bool							m_bAudioCodecSwitchStatus;
	uint32_t 						m_unPrePlayPosition;
	AVStatusMgr_PlaybackStatus_e 	m_ePlaybackStatus;
	AVStatusMgr_BufferStatus_e		m_eBufferStatus;
	AVStatusMgr_ProbeStatus_e		m_eProbeStatus;
	double							m_dblLastCurrentAudioPts;
	double							m_dblLastCurrentVideoPts;
	bool							m_bEndOfStreamStatus;
	bool							m_bCsoAudioFlushStatus;
	double							m_dblCurrentVolume;
	double							m_dblTargetVolume;
	bool							m_bIsFadeFailed;
	AVStatusMgr_DecodingStatus_e	m_eDecodingStatus;
	
	static AVStatusManager 			*s_hStatusManager;
	static pthread_mutex_t			s_mtxStatusManager;

	/* For Audio Ease function */
	std::list<stEaseEntry_t>		m_listEaseEntry;
};

}	// namespace esplayer
}	// device
}	// namespace netflix

#endif	/* AVSTATUSMANAGER_H */

