#ifndef AVCONTROLLER_H
#define AVCONTROLLER_H

#include <nrdbase/StdInt.h>
#include <nrdbase/NFErr.h>
#include <nrdbase/Log.h>
#include <nrdbase/Mutex.h>

#include "PlaybackGroupNative.h"
#include "AVControllerEaseThread.h"
#include "NativeBridge.h"
#include <pthread.h>

#include "cso_def.h"
#include "cso_uva.h"

namespace netflix {
namespace device {
namespace esplayer {

/* 	Below enumeration(AVCtrl_EventType_e) references nxlink.h 
	If nxlink.h changes, you should check below enumerations */
typedef enum
{
	AVCTRL_EVENT_TYPE_Unknown            = 0,
	AVCTRL_EVENT_TYPE_Started            = 1,
	AVCTRL_EVENT_TYPE_Stopped            = 2,
	AVCTRL_EVENT_TYPE_PlaytimeChanged    = 3,
	AVCTRL_EVENT_TYPE_PlayStateChanged   = 4,
	AVCTRL_EVENT_TYPE_PumpIndexChanged   = 5,
	AVCTRL_EVENT_TYPE_SeekReady          = 6,
	AVCTRL_EVENT_TYPE_AudioCodecSwitched = 7,
	AVCTRL_EVENT_TYPE_BufferStateChanged = 8,	/* Group #2 models check underflow through buffer state. */
	AVCTRL_EVENT_TYPE_AudioFadeCompleted = 9,
	AVCTRL_EVENT_TYPE_AudioFlushCompleted = 10,
	AVCTRL_EVENT_TYPE_AudioRestartCompleted = 11,
	AVCTRL_EVENT_TYPE_AudioFadeFailed 	 		= 12,
	AVCTRL_EVENT_TYPE_AudioDecoderVolumeChanged = 13,
}AVCtrl_EventType_e;

typedef enum
{
	AVCTRL_PLAY_SPEED_PAUSE = 0,
	AVCTRL_PLAY_SPEED_NORMAL = 100,
}AVCtrl_PlaySpeed_e;

/* 	Below enumeration(AVCtrl_MCtrlPlayState_e) references nxlink.h 
	If nxlink.h changes, you should check below enumerations */
typedef enum
{
	AVCTRL_MCTRL_PLAY_STATE_Stop               = 0,
	AVCTRL_MCTRL_PLAY_STATE_Playing            = 1,
	AVCTRL_MCTRL_PLAY_STATE_Paused             = 2,
	AVCTRL_MCTRL_PLAY_STATE_EndOfStream        = 3,
	AVCTRL_MCTRL_PLAY_STATE_Error              = 4,
	AVCTRL_MCTRL_PLAY_STATE_BufferingUnderflow = 5,
}AVCtrl_MCtrlPlayState_e;

/* 	Below enumeration(AVCtrl_MCtrlBufferState_e) references nxlink.h 
	If nxlink.h changes, you should check below enumerations */
typedef enum
{
	AVCTRL_MCTRL_BUFFER_STATE_None = 0,
	AVCTRL_MCTRL_BUFFER_STATE_CanPlay,
	AVCTRL_MCTRL_BUFFER_STATE_NotEnough,
	AVCTRL_MCTRL_BUFFER_STATE_Enough,
	AVCTRL_MCTRL_BUFFER_STATE_Buffering_Finished,

	AVCTRL_MCTRL_BUFFER_STATE_ENDOFCASE
}AVCtrl_MCtrlBufferState_e;

typedef struct AVCtrlProbeInfo
{
	int32_t video_codec;
	int32_t audio_codec;
	uint32_t video_pid;
	uint32_t audio_pid;
} AVCtrl_ProbeInfo_t;

/* 	Below enumeration(AVCtrl_PlaybackStatus_e) references AVStatusManager.h 
	If AVStatusManager.h changes, you should check below enumerations */
typedef enum
{
	AVCtrl_PlaybackStatus_Stop = 0,
	AVCtrl_PlaybackStatus_Pause,
	AVCtrl_PlaybackStatus_Playing,
}AVCtrl_PlaybackStatus_e;

/* 	Below enumeration(AVCtrl_ProbeStatus_e) references AVStatusManager.h 
	If AVStatusManager.h changes, you should check below enumerations */
typedef enum
{
	AVCtrl_ProbeStatus_None = 0,
	AVCtrl_ProbeStatus_Probing,
	AVCtrl_ProbeStatus_ProbeCompleted,
}AVCtrl_ProbeStatus_e;

typedef void (*pfAVCtrlListener)(void *hHandle, AVCtrl_EventType_e eEvent);

class AVStatusManager;
class AVController
{
public:
	AVController();
	virtual ~AVController();

	static AVController* getInstance()
	{
		if(s_hController == NULL)
		{
			pthread_mutex_init(&s_mtxController, NULL);
			
			s_hController = new AVController();
			if(s_hController == NULL)
			{
				NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] Cannot construct AVController : s_hController is NULL\n", __func__, __LINE__);
			}
		}
	
		return s_hController;
	}

	static void MutexLock()
	{
		int32_t nRet = 0;

		nRet = pthread_mutex_lock(&s_mtxController);
		if(nRet)
		{
			NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] Failed to lock s_mtxController, nRet = %d\n", __func__, __LINE__, nRet);
		}
	}
	
	static void MutexUnlock()
	{
		int32_t nRet = 0;
		
		nRet = pthread_mutex_unlock(&s_mtxController);
		if(nRet)
		{
			NTRACE(TRACE_MEDIAPLAYBACK, "@@@@@ [%s:%d] Failed to unlock s_mtxController, nRet = %d\n", __func__, __LINE__, nRet);
		}
	}

	bool 		setListener(pfAVCtrlListener pfListener);
	bool 		setProbeInfo(PlaybackGroupProbeInfo_t *pstProbeInfo);
	bool 		setPlaySpeed(int32_t speed_sec);
	bool 		getPumpIndex(uint32_t *punIndex);
	bool		getPlayPosition(uint32_t *punPosition);
	bool		updatePlayPosition(uint32_t unPosition);
	bool 		flush();
	bool 		stop();
	bool 		play();
	bool 		pause();
	bool 		resume();
	bool 		audioFlush(uint32_t unTryToChangeAudioTrack);
	bool		audioRestart();
	bool		audioCodecSwitch(uint32_t unTryToChangeAudioTrack, int32_t nAudioCodec);
	bool		checkAudioFlush(uint32_t unTryToChangeAudioTrack);
	bool 		procMediaCtrlPlayState();
	bool 		procMediaCtrlBufferState();
	bool		setDuration();
	bool 		seekReady();
	bool		audioCodecSwitched();
	bool		audioFadeCompleted();
	bool 		audioFadeFailed();
	bool		audioDecoderVolumeChanged();
	bool 		probeComplete();
	void					setPlaybackStatus(AVCtrl_PlaybackStatus_e ePlaybackStatus);
	AVCtrl_PlaybackStatus_e getPlaybackStatus();
	void 					setProbeStatus(AVCtrl_ProbeStatus_e eProbeStatus);
	AVCtrl_ProbeStatus_e 	getProbeStatus();
	bool 					readyToResume();

	/* Audio Ease Thread, Function */;
	void		EaseTask();
	void		setCloseEaseThreadFlag();
	bool		closeEaseThreadFlagIsSet();
	
	/* Temporary Code, modified by thoh */
	static AVController* TEMP_getInstanceEventCallback() { return s_hController; }

	pfAVCtrlListener m_pfEventCallback;

private:
	/* This function handles events from APPKIT */
#if 0
	static void onEventCallback(uint32_t p1, uint32_t p2, uint32_t p3);
#else
	static void onEventCallback(uint32_t ulViewId, uint32_t ulSessionId, uint32_t eType, uint32_t eEvent, void *pstNotifyData, void *pvUserData);
#endif
	bool		handlePlayStateStop();
	bool		handlePlayStatePaused();
	bool 		handlePlayStatePlaying();
	bool 		handlePlayStateBufferingUnderflow();
	bool 		handleEndOfStream();
	
	static pthread_mutex_t		s_mtxController;
	static AVController 		*s_hController;
	AVCtrl_ProbeInfo_t 			m_stProbeInfo;

	/* Audio Ease Variables */
	shared_ptr<AVControllerEaseThread> m_hEaseThread;
	Mutex 	m_mtxEaseTask;
	bool 	m_bRunning;
};

}	// namespace esplayer
}	// device
}	// namespace netflix

#endif	/* AVCONTROLLER_H */
