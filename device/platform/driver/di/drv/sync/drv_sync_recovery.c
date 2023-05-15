/*
* 2010/10/06: Add bEnableAudioTSM
*		- Like working the bEnableVideoTSM, it is checked for Audio TSM setting from DI_AUDIO.
*		- In some cases, Audio Async mode is needed from DI layer.
*		- So, sync module do not check audio decoder error status.
*
* 2010/09/18: Add bNoWrapTsm variable
*		- It is configured from CONFIG_WRAP_AROUND_VSYNC
*		- When current a/v mode is vsync and wrap-around would be done, not change to tsm mode.
*
* 2010/09/17: Add bWrapRestart variable
*		- Add its variable that affects to av restart without checking pts error. 
*
* 2010/09/16: Modify restart method when wrap-around. 
*		- As deciding wrap-around from live stream, not check pts error, just do restart a/v.
*
* 2010/05/18: Add to get Video decoder Extended Status and code to change video display mode
*		- Enforce to check video decoder status when the display status is continuousely error.
* 		- Tsm to vsync mode, when audio is already gone to vsync mode,
*		- but video is still being tsm mode and is going to occur pts Error.
*
* 2010/05/07: Add to check A/V decoder status
*		- There is a particular case that decoding status is normal but the display status is continueously error.
*		- At this time, this module must be stopped and wait until recovering normal status in display.
*		- After recovering this status, ptsErrorCount from decoder status must be ignored.
*		- Instead of the ptsErrorCount, local ptsErrorCount is used for check current error status.
*
* 2010/04/27: A bug is fixed that the bAudMainRestart is not cleared. Not only audio flag, also video restart flag.
*
* 2010/04/17: Modify to Check Normal Video Start or Still Video Start
*		- Add bEnableVideoTSM to check ptsError for video.
*
* 2010/01/19: Modify Restart conditional value, +-10000 becaue under its value and pts error are occurred.
*		- Separate conditional value about A/V. Video value is still same as previous value but Audio value is
*		- now +-4500.
*		- Enforce Wrap around method. Not only check delta between previous stc and current stc, but also
*		- ptc. Because stc value is not avaliable where one of the decoders is going to vsync mode.
*		- Only checking stc delta value is not affected to wrap around method.
*		- Enforce Wrap around method. As a/v decoder is at Vsync mode, stc value is not available.
*		- So check current sync mode and when being vsync mode, only watch at pts delta value that needs wrap
*		- around
*		- Wrap around for pts delta that enforces restart at first time.
*
* 2010/01/13: Add to clean gTimeLimit value when pts error counter is going to be increased, not to be over-ranged
*		  diff-value between stc and pts value.
*		- Add some debugging logs.
*
* 2009/12/29:
*		- Add a function about wrap-around state.
*		- Get current stc and delta-value with previous stc. If it is over or under 1 sec,
*		- it must be warp-around state at a/v decoder. So need to restart them.
*
* 2009/12/9: 
*		- Decoder Restart function is modified. It has to check whether the decoder is activated or not.
*		- If the decoder is not working, the restart function should stop. 
*		  (so the video stop function is moved to restart function inside.)
*
* 2009/12/5: VSYNC_RESTART_THRESHOLD changed.
*		- Seperates to video and audio threshold.
*		- Video threshold is varied into 20, but audio threshold is still 50.
*
* 2009/12/2: DI_SYNC_MODECHANGE_TIMELIMIT changed.
*		- DI_SYNC_MODECHANGE_TIMELIMIT  is fixed as 2 (200ms)
*		- uiTimeLimit is changed as a global variable gTimeLimit.
*		- gTimeLimit is initialized as 0 at every channel changes and protected by semaphore.
*
* 2009/12/1: DI_SYNC_MODECHANGE_TIMELIMIT added.
*		- DI_SYNC_MODECHANGE_TIMELIMIT is fixed as 3 (300ms)
*
* 2009/11/28: VK Msg function added.
*		- A/V restart functions is only controlled in this module.
*		- So, other modules can restart a/v decoder send by reqest message to this module.
*
* 2009/11/24: Audio-only Restart function added.
*		- If the video status is normal & the audio status is going to be that ptsError is occured,
*		restart only audio decoders.
*
* 2009/11/18: Semaphores added.
*		- sSync.eSyncMode is wrapped with semaphores.
*
* 2009/11/17: DRV_SYNC_TSM_CHECK status added.
*		- It doesn't go to the VSync mode from the Tsm mode directly.
*		- Status monitoring process is reconstructed.
*
* 2009/11/16: Code refinement.
*		- Code refining & optimization.
*
* 2009/11/7: Condition check is added.
*		- Check PCR PID & playback status -> if no PCR PID or playback, skip sync monitor
*
* 2009/11/2: Condition check variables are redefined.
*		- SYNC_RESTART_CLEAR_TIME and VSYNC_RESTART_THRESHOLD are modified.
*
* 2009/10/27: Bug fixed.
*		- It (started or not) should be checked that both of audio & video decoder.
*		- Restart condition shoulld not be checked when the new mode is decided.
*
* 2009/10/26: Use PCR value to check wrap around status.
*		- When Decoder is started, get the last pcr value.
*		- Since pts error is occurred, get the last pcr value at this instance and 
*		- compare it with the previous last pcr value whether it is over or not.
*		- If it is under the previous last pcr value, it would be wrap around.
*
* 2009/10/26: There are two things when restarting a/v decoder to vsync mode
*		- Only Video data is available.
*		- Both of Video data and PCR data are available.
*
* 2009/10/23: Sync mode change condition is modified. 
*		- Audio Status check is added.
*/

#include <string.h>
#include "linden_trace.h"

#include "drv_err.h"
#include "di_video.h"
#include "drv_video.h"
#include "drv_demux.h"
#include "drv_sync_recovery.h"
#include "di_audio.h"
#include "drv_audio.h"
#include "vkernel.h"
#include "taskdef.h"
#include "drv_sync_recovery.h"

#include "nexus_timebase.h"
#include "nexus_pid_channel.h"
#include "nexus_sync_channel.h"
#include "nexus_video_decoder_extra.h"
#include <stdlib.h>

#define DEBUG_MODULE        TRACE_MODULE_DRV_SYNC

#define SYNC_MESSAGE_QUEUE_SIZE 10
#define SYNC_TASK_CHECK_STATE_TIME 100

/*	DI_SYNC_MODECHANGE_TIMELIMIT is the time amount to wait for becoming normal status.
*	If it keeps the abnormal status more than this time duration, the sync mode moves toward VSync. The unit is ms.
*/
#define DI_SYNC_MODECHANGE_TIMELIMIT 5

/*	Notice!!
*	SYNC_RESTART_CLEAR_TIME is a time interval for the monitoring task.
*	The monitoring task checks decoder status at least after this time.
*	Because decoders need a little bit time to update its status.
*	Don't forget this value is experimental value. It should be compensated on&on.
*/
#define SYNC_RESTART_CLEAR_TIME 1000

/*	Notice!!
*	VSYNC_VID_RESTART_THRESHOLD / VSYNC_AUD_RESTART_THRESHOLD are a count value for the monitoring task.
*	The monitoring task makes decisions whether the Sync mode should be VSYNC or TSM with this value.
*	If PTS Err Count exceeds this value, The Sync mode becomes VSYNC mode.
*	Don't forget this value is experimental value. It should be compensated on&on.
*/
#define VSYNC_VID_RESTART_THRESHOLD 20 
#define VSYNC_AUD_RESTART_THRESHOLD 50 


/*	Notice!!
*	WRAP_WAROUND_MAX or MIN_THRESHOLD are a count value for the monitoring task.
*	The monitoring task makes decisions whether current a/v decoder status are now warp-around.
*	If stc delta value, different between current stc and previous stc, is over +- 1sec,
*	it would be wrap-around state.
*	Don't forget this value is experimental value. It should be compensated on&on.
*/
#define WRAP_WAROUND_MAX_THRESHOLD (45000) /* 1 sec (45KHz) */
#define WRAP_WAROUND_MIN_THRESHOLD (-45000) /* 1 sec (45KHz) */

#define SYNC_AV_START_TIMEOUT	300 /* 300 msec */

#if defined(CONFIG_WRAP_AROUND_VSYNC)
static HBOOL bNoWrapTsm = TRUE;		/* When doing wrap-around work and a/v sync is already vsync, the av mode is vsync. */
#else
static HBOOL bNoWrapTsm = FALSE;
#endif
/*	If there's problem, look at it again! 
*	and the problem keeps on, go to the VSYNC mode. */
typedef enum {
	DRV_SYNC_TSM=0,
	DRV_SYNC_TSM_CHECK,
	DRV_SYNC_VSYNC,
	DRV_SYNC_NOT_RESTART=255
} DRV_SYNC_MODE;

typedef struct DRV_SYNC {
/*	Video stuffs */
	NEXUS_VideoDecoderHandle hVidDecoder;
	NEXUS_VideoDecoderStatus VidCurStatus;
	NEXUS_VideoDecoderStatus VidPrevStatus;
	NEXUS_VideoDecoderExtendedStatus VidCurExtStatus;
	DI_VIDEO_FORMAT eVidFmt;
	HBOOL	bEnableVideoTSM;
	HBOOL	bEnableAudioTSM;
	
/*	Audio stuffs */
	NEXUS_AudioDecoderHandle hAudMainDecoder;
	NEXUS_AudioDecoderHandle hAudSpdifDecoder;
	NEXUS_AudioDecoderStatus AudMainCurStatus;
	NEXUS_AudioDecoderStatus AudMainPrevStatus;
	NEXUS_AudioDecoderStatus AudSpdifCurStatus;
	DI_AUDIO_FORMAT eAudFmt;

	DRV_SYNC_MODE	eSyncMode;

	HBOOL	bSpdifDecRun;
	HBOOL	bTimerClear;
	HBOOL	bVideoRestart;
	HBOOL	bWrapRestart;
	HUINT32 ulPrevLastStc;
	HINT32 iVidPrevLastPts;
	HUINT32 ulVidPtsErrCnt;
	HUINT32 ulVidLastPtsErrCnt;
	HUINT32 ulAudMainPtsErrCnt;
	HUINT32 ulAudMainLastPtsErrCnt;	
	HUINT32 ulAudSpdifPtsErrCnt;
	HUINT32 ulAudSpdifLastPtsErrCnt;	
	unsigned long ulTimerClearId;
} DRV_SYNC_s;

static unsigned long ulSyncMonitorTaskId;
static unsigned long ulSyncMsgQueueId;
static unsigned long s_ulSyncSemId;
static DRV_SYNC_s sSync;
static HUINT32 gTimeLimit;
#if __________p____________
#endif

static DRV_Error P_SYNC_GetDecoderStatus(void)
{
	NEXUS_Error nResult;
	
	nResult = NEXUS_VideoDecoder_GetStatus(sSync.hVidDecoder,&sSync.VidCurStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_VideoDecoder_GetStatus failed!\n");
		return DRV_ERR;
	}
	nResult = NEXUS_VideoDecoder_GetExtendedStatus(sSync.hVidDecoder,&sSync.VidCurExtStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_VideoDecoder_GetExtendedStatus failed!\n");
		return DRV_ERR;
	}

	nResult = NEXUS_AudioDecoder_GetStatus(sSync.hAudMainDecoder, &sSync.AudMainCurStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
		return DRV_ERR;
	}
	
	nResult = NEXUS_AudioDecoder_GetStatus(sSync.hAudSpdifDecoder, &sSync.AudSpdifCurStatus);
	if(nResult != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_AudioDecoder_GetStatus failed!\n");
		return DRV_ERR;
	}
	sSync.bSpdifDecRun = sSync.AudSpdifCurStatus.started;
	
	return DRV_OK;	
}

static HBOOL P_SYNC_CheckRestartCondition(HINT32 iPtsStcDiff, DRV_SYNC_AV eAvMode, HBOOL bTimerClear)
{
	HBOOL bRestart = FALSE;
	HINT32 iDiffValue=0;

	switch(eAvMode)
	{
		case DRV_SYNC_VID:
			iDiffValue = 10000;
			break;
		case DRV_SYNC_AUDMAIN:
		case DRV_SYNC_AUDSPDIF:
			iDiffValue = 4500;
			break;
		default:
			iDiffValue = 10000;
			break;
	}	

	if(((iPtsStcDiff > iDiffValue) || (iPtsStcDiff < -iDiffValue)) 
		&& (bTimerClear == TRUE))
	{
		bRestart = TRUE;
	}

	return bRestart;
}

static void P_SYNC_ClearTimer(unsigned long TimerId, void *params)
{
	BSTD_UNUSED(TimerId);
	BSTD_UNUSED(params);
	
	if(sSync.ulTimerClearId != VK_TIMERID_NULL)
	{
		sSync.bTimerClear=TRUE;
		sSync.ulTimerClearId=VK_TIMERID_NULL;
	}
}

static void P_SYNC_Restart(HBOOL bTsm)
{
	DRV_Error nDrvErr = DRV_OK;

	PrintError("[%s] A/V Decoder Restart (%s)\n",__func__,(bTsm == TRUE)?"TSM":"VSYNC");

/*	Decoder Status Check
*	If the decoder is not active, Just don't restart.
*/
	nDrvErr = P_SYNC_GetDecoderStatus();
	if(nDrvErr != DRV_OK)
	{
		PrintError("P_SYNC_GetDecoderStatus failed!\n");
		return;
	}

	if(sSync.VidCurStatus.started == TRUE)
	{
		if(sSync.bVideoRestart == TRUE)
		{
			/* Start Video and Audio */
			if(sSync.bEnableVideoTSM == TRUE)
				nDrvErr = DRV_VIDEO_Restart(VID_DEC_INSTANCE, bTsm);
			else
				nDrvErr = DRV_VIDEO_Restart(VID_DEC_INSTANCE, FALSE);

			if(nDrvErr != DRV_OK)
			{
				PrintError("DRV_VIDEO_Restart failed!\n");
			}
		}
	}

	if(sSync.AudMainCurStatus.started == TRUE)
	{
		if(sSync.bEnableAudioTSM == TRUE)
			nDrvErr=DRV_AUDIO_ChangeSyncMode(0,bTsm);
		else
			nDrvErr=DRV_AUDIO_ChangeSyncMode(0,FALSE);

		if(nDrvErr != DRV_OK)
		{
			PrintError("[%s] Audio Decoder[0] is not started!!\n",__func__);
		}
	}

	if(sSync.AudSpdifCurStatus.started == TRUE)
	{
		if(sSync.bSpdifDecRun == TRUE)
		{
			if(sSync.bEnableAudioTSM == TRUE)
				DRV_AUDIO_ChangeSyncMode(2, bTsm);
			else
				DRV_AUDIO_ChangeSyncMode(2, FALSE);
		}
	}

	sSync.ulPrevLastStc=0;
	sSync.iVidPrevLastPts=0;
	sSync.ulVidPtsErrCnt=0;
	sSync.ulVidLastPtsErrCnt=0;
	sSync.ulAudMainPtsErrCnt=0;
	sSync.ulAudMainLastPtsErrCnt=0;
	sSync.ulAudSpdifPtsErrCnt=0;
	sSync.ulAudSpdifLastPtsErrCnt=0;
	sSync.bVideoRestart = FALSE;
	sSync.bWrapRestart = FALSE;
}

static void P_SYNC_Reset(void)
{
	VK_SEM_Get(s_ulSyncSemId);
	gTimeLimit = 0;
	sSync.ulTimerClearId = VK_TIMERID_NULL;
	sSync.bTimerClear = TRUE;
	sSync.ulPrevLastStc = 0;
	sSync.iVidPrevLastPts = 0;
	sSync.ulVidPtsErrCnt=0;
	sSync.ulVidLastPtsErrCnt=0;
	sSync.ulAudMainPtsErrCnt=0;
	sSync.ulAudMainLastPtsErrCnt=0;
	sSync.ulAudSpdifPtsErrCnt=0;
	sSync.ulAudSpdifLastPtsErrCnt=0;
	sSync.eSyncMode = DRV_SYNC_TSM;
	sSync.bVideoRestart = FALSE;
	sSync.bWrapRestart = FALSE;
	VK_memset(&sSync.VidPrevStatus,0,sizeof(NEXUS_VideoDecoderStatus));
	VK_memset(&sSync.AudMainPrevStatus,0,sizeof(NEXUS_AudioDecoderStatus));
	VK_SEM_Release(s_ulSyncSemId);
}

static void P_SYNC_SetVideoTsm(HBOOL bOnOff)
{
	VK_SEM_Get(s_ulSyncSemId);
	sSync.bEnableVideoTSM = bOnOff;
	VK_SEM_Release(s_ulSyncSemId);
}

static void P_SYNC_SetAudioTsm(HBOOL bOnOff)
{
	VK_SEM_Get(s_ulSyncSemId);
	sSync.bEnableAudioTSM = bOnOff;
	VK_SEM_Release(s_ulSyncSemId);
}

static void P_SYNC_SetTimer(HUINT32 uiTimeOut)
{
	if(sSync.ulTimerClearId != VK_TIMERID_NULL)
	{
		VK_TIMER_Cancel(sSync.ulTimerClearId);
	}
	VK_TIMER_EventAfter(uiTimeOut, P_SYNC_ClearTimer, NULL, 0, (unsigned long *)&sSync.ulTimerClearId);
	sSync.bTimerClear=FALSE;
}

#if __________drv____________
#endif

void DRV_SYNC_SetTsmMode()
{
	P_SYNC_Reset();
	P_SYNC_SetTimer(SYNC_AV_START_TIMEOUT);
}

void DRV_SYNC_SetVideoTsmMode(HBOOL bOnOff)
{
	P_SYNC_SetVideoTsm(bOnOff);
}

void DRV_SYNC_SetAudioTsmMode(HBOOL bOnOff)
{
	P_SYNC_SetAudioTsm(bOnOff);
}

unsigned int DRV_SYNC_GetPcrValue(void)
{
	NEXUS_TimebaseStatus timeStatus;
	NEXUS_Timebase_GetStatus(NEXUS_Timebase_e0,&timeStatus);
	return timeStatus.lastValue;
}

static void DRV_SYNC_MonitorTask(void *pvParam)
{
	(void)pvParam;
	NEXUS_PidChannelHandle pcrPidchannelhandle = NULL;
	
	DRV_Error dResult;
	DRV_DEMUX_PLAYBACK_MODE ePlaybackMode;
	
	HUINT32 uiStc=0;
	HINT32 iVidPtsStcDiff=0;
	HINT32 iAudMainPtsStcDiff=0;
	HINT32 iAudSpdifPtsStcDiff=0;
	
	HBOOL bIsPlayback = FALSE;
	HBOOL bAudMainRestart = FALSE;
	HBOOL bAudSpdifRestart = FALSE;
	HBOOL bVidRestart = FALSE;

	DRV_SYNC_MSG SyncMsg;
	DRV_SYNC_MSG_TYPE eSyncMsgType=DRV_SYNC_NULL;

	HINT32 iStcDelta=0;
	HINT32 iVidPtsDelta=0;
	
	while(1)
	{
		if(VK_MSG_ReceiveTimeout(ulSyncMsgQueueId, &SyncMsg, sizeof(SyncMsg), SYNC_TASK_CHECK_STATE_TIME) == VK_OK)
		{
			if(SyncMsg.ulParam0 == DRV_SYNC_REQ_RESTART)
			{
				PrintDebug("Restart request from external module (TSM) \n");	
				eSyncMsgType = SyncMsg.ulParam0;
			}
		}
/*************************
*	1. Time Out
*************************/
		if(eSyncMsgType == DRV_SYNC_NULL)
		{
			bIsPlayback = FALSE;
			pcrPidchannelhandle = DRV_DEMUX_GetPcrPidChannel();
			if(pcrPidchannelhandle)
			{
				bIsPlayback = DRV_DEMUX_IsPlaybackPidChannel(pcrPidchannelhandle, &ePlaybackMode);
			}
			if(!pcrPidchannelhandle || bIsPlayback)
			{
				P_SYNC_Reset();
				continue;
			}
		}

/*************************
*	2. Get the current decoder status
*************************/
		dResult = P_SYNC_GetDecoderStatus();
		if(dResult != DRV_OK)
		{
			PrintError("P_SYNC_GetDecoderStatus failed!\n");
			continue;
		}
/*************************
*	3. Decoder status check
*************************/
		if((sSync.VidCurStatus.started == false) && (sSync.AudMainCurStatus.started == false))
		{
			P_SYNC_Reset();
			eSyncMsgType = DRV_SYNC_NULL;
			continue;
		}
/*************************
*	4. Skip all the work if a timer is on
*************************/
		if(sSync.bTimerClear == FALSE) 
		{
			continue;
		}
/*************************
*	5. Main check routine
*************************/
		if(sSync.bEnableVideoTSM == FALSE)
		{
			sSync.eSyncMode = DRV_SYNC_VSYNC;
		}

		switch(eSyncMsgType)
		{
			case DRV_SYNC_NULL:
				dResult = DRV_VIDEO_GetCurrentStc(VID_DEC_INSTANCE,&uiStc);
				if(dResult != DRV_OK)
				{
					PrintError("DRV_VIDEO_GetCurrentStc failed!\n");
					continue;
				}
				
/*************************
*	5.1. Stc check in TSM mode
*************************/				
				if(sSync.eSyncMode < DRV_SYNC_VSYNC)
				{
					iStcDelta=(int)(uiStc - sSync.ulPrevLastStc);
					if((iStcDelta > WRAP_WAROUND_MAX_THRESHOLD || iStcDelta < WRAP_WAROUND_MIN_THRESHOLD)
						&& (sSync.ulPrevLastStc != 0))
					{
						sSync.bVideoRestart = TRUE;
						sSync.bWrapRestart = TRUE;

						VK_SEM_Get(s_ulSyncSemId);
						gTimeLimit = DI_SYNC_MODECHANGE_TIMELIMIT;
						sSync.eSyncMode = DRV_SYNC_TSM;
						VK_SEM_Release(s_ulSyncSemId);
						PrintError("(TSM) Wrap Around so A/V Restart to TSM mode\n");
					}
					sSync.ulPrevLastStc = uiStc;
				}
/*************************
*	5.2. Stc check in Vsync mode
*************************/
				else
				{
					iVidPtsDelta=(int)sSync.VidCurStatus.pts-sSync.iVidPrevLastPts;
					if(iVidPtsDelta < 0 && sSync.iVidPrevLastPts != 0)
					{
						if(bNoWrapTsm == FALSE) 
						{
							sSync.bVideoRestart = TRUE;
							sSync.bWrapRestart = TRUE;

							VK_SEM_Get(s_ulSyncSemId);
							gTimeLimit = DI_SYNC_MODECHANGE_TIMELIMIT;

							if(sSync.bEnableVideoTSM == TRUE)
							{
								sSync.eSyncMode = DRV_SYNC_TSM;
							}

							VK_SEM_Release(s_ulSyncSemId);
							PrintError("(VSYNC) Wrap Around so A/V Restart to TSM mode\n");
						}
						else
						{
							PrintError("(VSYNC) Wrap Around but Not A/V Restart\n");
						}
							
					}
					sSync.iVidPrevLastPts=sSync.VidCurStatus.pts;
				}

/*************************
*	5.3. PtsError Check
*	If there's no problem, goes out at here.
*************************/
				if(sSync.bWrapRestart == FALSE)
				{
					if(sSync.bEnableVideoTSM == TRUE)
					{

/*************************
*	5.3.1. Video Decoder Check
*************************/
						if((sSync.VidCurStatus.queueDepth == 0) &&
							(sSync.VidCurStatus.numDisplayUnderflows > sSync.VidPrevStatus.numDisplayUnderflows) &&
							(sSync.VidCurStatus.numDisplayErrors > sSync.VidPrevStatus.numDisplayErrors) &&
							(sSync.VidCurStatus.numDecoded == sSync.VidPrevStatus.numDecoded) &&
							(sSync.VidCurStatus.numDisplayed == sSync.VidPrevStatus.numDisplayed) &&
							(sSync.VidCurExtStatus.pictureRepeatFlag == true))
						{
							sSync.ulVidLastPtsErrCnt = sSync.VidCurStatus.ptsErrorCount;
							VK_memcpy(&sSync.VidPrevStatus,&sSync.VidCurStatus,sizeof(NEXUS_VideoDecoderStatus));
							continue;
						}

/*************************
*	5.3.2. Video PtsError Check
*************************/
						sSync.ulVidPtsErrCnt = sSync.VidCurStatus.ptsErrorCount - sSync.ulVidLastPtsErrCnt;
						if(sSync.ulVidPtsErrCnt > VSYNC_VID_RESTART_THRESHOLD)
						{
							iVidPtsStcDiff = abs(uiStc - sSync.VidCurStatus.pts);
							bVidRestart = P_SYNC_CheckRestartCondition(iVidPtsStcDiff, DRV_SYNC_VID, sSync.bTimerClear);
							sSync.bVideoRestart = bVidRestart;					
							if(sSync.eSyncMode == DRV_SYNC_NOT_RESTART && sSync.AudMainCurStatus.tsm == false)
							{
								VK_SEM_Get(s_ulSyncSemId);
								gTimeLimit=DI_SYNC_MODECHANGE_TIMELIMIT;
								sSync.eSyncMode = DRV_SYNC_VSYNC;
								VK_SEM_Release(s_ulSyncSemId);
							}
							
						}
						else
						{
							bVidRestart = FALSE;
							sSync.bVideoRestart = bVidRestart;
						}
					}
					VK_memcpy(&sSync.VidPrevStatus,&sSync.VidCurStatus,sizeof(NEXUS_VideoDecoderStatus));
/*************************
*	5.3.3. Audio Decoder Check
*************************/

					if(sSync.bEnableAudioTSM == TRUE) 
					{
						if((sSync.AudMainCurStatus.sampleRate != 0) && 
							(sSync.AudMainCurStatus.pts != 0) && 
							(sSync.AudMainPrevStatus.pts == sSync.AudMainCurStatus.pts))
						{
							sSync.ulAudMainLastPtsErrCnt = sSync.AudMainCurStatus.ptsErrorCount;
							sSync.ulAudSpdifLastPtsErrCnt = sSync.AudSpdifCurStatus.ptsErrorCount;
							VK_memcpy(&sSync.AudMainPrevStatus,&sSync.AudMainCurStatus,sizeof(NEXUS_AudioDecoderStatus));
							continue;
						}

/*************************
*	5.3.4. Audio PtsError Check
*************************/
						sSync.ulAudMainPtsErrCnt = sSync.AudMainCurStatus.ptsErrorCount - sSync.ulAudMainLastPtsErrCnt;
						sSync.ulAudSpdifPtsErrCnt = sSync.AudSpdifCurStatus.ptsErrorCount-sSync.ulAudSpdifLastPtsErrCnt;
						if((sSync.ulAudMainPtsErrCnt > VSYNC_AUD_RESTART_THRESHOLD)||
							(sSync.AudSpdifCurStatus.ptsErrorCount > VSYNC_AUD_RESTART_THRESHOLD))
						{
							iAudMainPtsStcDiff = abs(uiStc - sSync.AudMainCurStatus.pts);
							bAudMainRestart = P_SYNC_CheckRestartCondition(iAudMainPtsStcDiff, DRV_SYNC_AUDMAIN, sSync.bTimerClear);

							if(sSync.bSpdifDecRun == TRUE)
							{
								iAudSpdifPtsStcDiff = abs(uiStc - sSync.AudSpdifCurStatus.pts);
								bAudSpdifRestart = P_SYNC_CheckRestartCondition(iAudSpdifPtsStcDiff, DRV_SYNC_AUDSPDIF, sSync.bTimerClear); 
							}
							else
							{
								bAudSpdifRestart = FALSE;
							}
						}				
						else
						{
							if(bAudMainRestart == TRUE)
							{
								bAudMainRestart = FALSE;
							}
							if(bAudSpdifRestart == TRUE)
							{
								bAudSpdifRestart = FALSE;
							}
						}
					}
					VK_memcpy(&sSync.AudMainPrevStatus,&sSync.AudMainCurStatus,sizeof(NEXUS_AudioDecoderStatus));
				
/*************************
*	5.3.5. Restart gauge update
*************************/
					if(bVidRestart == true || bAudMainRestart == true || bAudSpdifRestart == true)
					{
						PrintDebug("V = (%s), AM = (%s) AS = (%s)\n", (bVidRestart == true) ? "T" : "F", (bAudMainRestart == true) ? "T" : "F", (bAudSpdifRestart == true) ? "T" : "F");
						VK_SEM_Get(s_ulSyncSemId);
						gTimeLimit++;
						VK_SEM_Release(s_ulSyncSemId);
					}
					else
					{
						/* Don't trigger the timer unless there's a mode change!
						*/
						gTimeLimit=0;
						continue;
					}
				}
				break;

/*************************
*	5.4. Restart Request handling
*************************/
			case DRV_SYNC_REQ_RESTART:
				/* 	Other Modules(e.g. Video) request restarting a/v decoder.
				*	In this time, to do is only setting tsm mode.
				*/
				sSync.bVideoRestart = TRUE;

				VK_SEM_Get(s_ulSyncSemId);
				gTimeLimit=DI_SYNC_MODECHANGE_TIMELIMIT;
				sSync.eSyncMode = DRV_SYNC_TSM;
				VK_SEM_Release(s_ulSyncSemId);
				break;
			default:
				break;
				
		}
		
/*************************
*	6. Sync mode change
*************************/
		if(gTimeLimit < DI_SYNC_MODECHANGE_TIMELIMIT)
		{
			continue;
		}
		else
		{
			if(sSync.eSyncMode < DRV_SYNC_VSYNC)
			{
				VK_SEM_Get(s_ulSyncSemId);
				gTimeLimit=0;
				sSync.eSyncMode++;
				VK_SEM_Release(s_ulSyncSemId);
			}
		}

/*************************
*	7. Decoder Restart
*************************/
		switch(sSync.eSyncMode)
		{
			case DRV_SYNC_TSM_CHECK:
				P_SYNC_Restart(TRUE); /* Go to Tsm Mode */
				break;
			case DRV_SYNC_VSYNC:
				P_SYNC_Restart(FALSE); /* Go to Vsync Mode */
				sSync.eSyncMode=DRV_SYNC_NOT_RESTART;
				break;
			default:
				/* Noting to do */
				break;
		}

		if(sSync.eSyncMode == DRV_SYNC_VSYNC || sSync.eSyncMode == DRV_SYNC_TSM_CHECK)
		{
			P_SYNC_SetTimer(SYNC_RESTART_CLEAR_TIME);
		}
		eSyncMsgType = DRV_SYNC_NULL;
	}
}

DRV_Error DRV_SYNC_GetMsgId(unsigned long *ulSyncMsgId)
{
	*ulSyncMsgId = ulSyncMsgQueueId;
	return DRV_OK;
}


DRV_Error DRV_SYNC_RECOVERY_Init(DRV_SYNC_DECODER_s hdecoder)
{
	DRV_Error nDrvErr=DRV_OK;
	HINT32 vResult;

	/* Before starting this task, we need to get video handle */
#if 0
	DRV_VIDEO_GetVideoHandle(VID_DEC_INSTANCE, &sSync.hVidDecoder);
	DRV_AUDIO_GetDecoderHandle(DI_AUDIO_DECODER_MAIN, &sSync.hAudMainDecoder);
	DRV_AUDIO_GetDecoderHandle(DI_AUDIO_DECODER_SPDIF, &sSync.hAudSpdifDecoder);
#else
	sSync.hVidDecoder = hdecoder.hVidDecoder;
	sSync.hAudMainDecoder = hdecoder.hAudMainDecoder;
	sSync.hAudSpdifDecoder = hdecoder.hAudSpdifDecoder;
#endif

	if(VK_MSG_Create(SYNC_MESSAGE_QUEUE_SIZE, sizeof(DRV_SYNC_MSG),"SyncMsgQueue",&ulSyncMsgQueueId,VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		PrintError("[%s] Creating Sync Msg Qeueu failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	if(VK_TASK_Create(DRV_SYNC_MonitorTask, SYNC_TASK_PRIORITY, SYNC_TASK_STACK_SIZE, "DRV_VIDEO_MonitorTask", (void *)0, &ulSyncMonitorTaskId, FALSE)!=VK_OK)
	{
		PrintError("[%s] Creating Sync Task failed!\n",__FUNCTION__);
		nDrvErr=DRV_ERR;
		goto done;
	}
	else
	{
		VK_TASK_Start(ulSyncMonitorTaskId);
	}

	vResult = VK_SEM_Create(&s_ulSyncSemId, "drv_sync", DEFAULT_SUSPEND_TYPE);
	if(vResult != VK_OK)
	{
		PrintError("VK_SEM_Create failed!\n");
		return DRV_ERR;
	}

	sSync.ulTimerClearId = VK_TIMERID_NULL;
	sSync.bTimerClear = TRUE;
	sSync.bSpdifDecRun = FALSE;
	sSync.bVideoRestart = FALSE;
	sSync.ulVidPtsErrCnt=0;
	sSync.ulVidLastPtsErrCnt=0;
	sSync.ulAudMainPtsErrCnt=0;
	sSync.ulAudMainLastPtsErrCnt=0;
	sSync.ulAudSpdifPtsErrCnt=0;
	sSync.ulAudSpdifLastPtsErrCnt=0;

done:
	return nDrvErr;
}
