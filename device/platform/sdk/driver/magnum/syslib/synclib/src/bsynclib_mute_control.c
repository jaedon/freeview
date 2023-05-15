/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_mute_control.c $
* $brcm_Revision: Hydra_Software_Devel/8 $
* $brcm_Date: 5/28/10 6:31p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_mute_control.c $
* 
* Hydra_Software_Devel/8   5/28/10 6:31p bandrews
* SW7405-4436: printing channel index in dbg messages
* 
* Hydra_Software_Devel/7   3/22/10 5:47p bandrews
* sw7408-83: add unconditional video unmute; fix dbg trace messages;
* consolidate debug prints for starting unmute and canceling unmute
* timers
* 
* Hydra_Software_Devel/6   3/19/10 2:49p bandrews
* SW7405-3774: merge mute control
* 
* Hydra_Software_Devel/SW7405-3774/2   3/18/10 10:03p bandrews
* SW7405-3774: fixed case where video may unmute even with pending
* adjustment
* 
* Hydra_Software_Devel/SW7405-3774/1   2/17/10 6:17p bandrews
* SW7405-3774: attempt to fix early audio unmute then mute
* 
* Hydra_Software_Devel/5   2/12/10 8:47p bandrews
* SW7405-3912: assert all input params are not null
* 
* Hydra_Software_Devel/5   2/12/10 8:46p bandrews
* SW7405-3912: assert all inputs are not null
* 
* Hydra_Software_Devel/4   2/4/10 3:43p bandrews
* SW7405-3774: fix PIG behavior
* 
* Hydra_Software_Devel/3   2/1/10 8:01p bandrews
* SW7405-3774: only mute/unmute synchronized sources
* 
* Hydra_Software_Devel/2   2/1/10 3:28p bandrews
* SW7405-3774: default mute control to off
* 
* Hydra_Software_Devel/1   1/26/10 9:00p bandrews
* SW7405-3774: added mute control support
***************************************************************************/

#include "bstd.h"
#include "bsyslib.h"
#include "bsyslib_list.h"
#include "bsynclib.h"
#include "bsynclib_priv.h"
#include "bsynclib_channel_priv.h"
#include "bsynclib_mute_control.h"
#include "bsynclib_mute_control_priv.h"
#include "bsynclib_audio_source.h"
#include "bsynclib_video_source.h"

BDBG_MODULE(synclib);

BERR_Code BSYNClib_MuteControl_ScheduleTask_isr(BSYNClib_Channel_Handle hChn)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_Channel_Results * psResults;

	BDBG_ENTER(BSYNClib_MuteControl_ScheduleTask_isr);

	BDBG_ASSERT(hChn);

	psResults = &hChn->sResults;

	if (!psResults->bMuteTaskScheduled)
	{
#ifdef BSYSLIB_TASK_SUPPORT
		if (hChn->sSettings.cbTask.pfSchedule_isr)
		{
			BSYSlib_Task_Settings sTask;

			sTask.pvParm1 = hChn;
			sTask.iParm2 = 0;
			sTask.pfDoTask = &BSYNClib_MuteControl_P_Process;

			rc = hChn->sSettings.cbTask.pfSchedule_isr(hChn->sSettings.cbTask.pvParm1, 
				hChn->sSettings.cbTask.iParm2, &sTask);
		}
#else
	rc = BSYNClib_Channel_P_StartTimer_isr(hChn, 
		hChn->psMuteControlTaskTimer, 0, 
		&BSYNClib_MuteControl_P_TaskTimerExpired, hChn, 0);
#endif

		if (!rc)
		{
			psResults->bMuteTaskScheduled = true;
			BDBG_MSG(("[%d] Mute control task scheduled", hChn->iIndex));
		}
		else
		{
			psResults->bMuteTaskScheduled = false;
		}
		if (rc) goto error;
	}
	else
	{
		BDBG_MSG(("[%d] Mute control task already scheduled", hChn->iIndex));
	}

	goto end;

error:
	
end:
	BDBG_LEAVE(BSYNClib_MuteControl_ScheduleTask_isr);
	return rc;
}

BERR_Code BSYNClib_MuteControl_CancelUnmuteTimers_isr(BSYNClib_Channel_Handle hChn)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYSlib_List_IteratorHandle hIterator;
	BSYNClib_VideoSource * psVideoSource;
	BSYNClib_AudioSource * psAudioSource;

	BDBG_ASSERT(hChn);

	BDBG_MSG(("[%d] Canceling all unmute timers", hChn->iIndex));

	hIterator = BSYSlib_List_AcquireIterator_isr(hChn->sVideo.hSources);
	while (BSYSlib_List_HasNext_isr(hIterator))
	{
		psVideoSource = (BSYNClib_VideoSource *)BSYSlib_List_Next_isr(hIterator);

		rc = BSYNClib_Channel_P_CancelTimer_isr(hChn, psVideoSource->psUnmuteTimer);
		if (rc) goto error;
	}
	BSYSlib_List_ReleaseIterator_isr(hIterator);

	hIterator = BSYSlib_List_AcquireIterator_isr(hChn->sAudio.hSources);
	while (BSYSlib_List_HasNext_isr(hIterator))
	{
		psAudioSource = (BSYNClib_AudioSource *)BSYSlib_List_Next_isr(hIterator);

		rc = BSYNClib_Channel_P_CancelTimer_isr(hChn, psAudioSource->psUnmuteTimer);
		if (rc) goto error;
	}
	BSYSlib_List_ReleaseIterator_isr(hIterator);

	goto end;

error:
	if (hIterator)
	{
		BSYSlib_List_ReleaseIterator_isr(hIterator);
	}

end:
	return rc;
}

BERR_Code BSYNClib_MuteControl_StartUnmuteTimers(BSYNClib_Channel_Handle hChn)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYSlib_List_IteratorHandle hIterator;
	BSYNClib_VideoSource * psVideoSource;
	BSYNClib_AudioSource * psAudioSource;
	unsigned int count;

	BDBG_ASSERT(hChn);

	count = 0;
	hIterator = BSYSlib_List_AcquireIterator(hChn->sVideo.hSources);
	while (BSYSlib_List_HasNext(hIterator))
	{
		psVideoSource = (BSYNClib_VideoSource *)BSYSlib_List_Next(hIterator);

		if (psVideoSource->sStatus.bMuted)
		{
			count++;
			BKNI_EnterCriticalSection();
			rc = BSYNClib_Channel_P_StartTimer_isr(hChn, psVideoSource->psUnmuteTimer, 
				hChn->hParent->sSettings.sVideo.uiUnmuteTimeout, 
				&BSYNClib_MuteControl_P_VideoSourceUnmuteTimerExpired, psVideoSource, 0);
			BKNI_LeaveCriticalSection();
			if (rc) goto error;
		}
	}
	BSYSlib_List_ReleaseIterator(hIterator);

	hIterator = BSYSlib_List_AcquireIterator(hChn->sAudio.hSources);
	while (BSYSlib_List_HasNext(hIterator))
	{
		psAudioSource = (BSYNClib_AudioSource *)BSYSlib_List_Next(hIterator);

		if (psAudioSource->sStatus.bMuted)
		{
			count++;
			BKNI_EnterCriticalSection();
			rc = BSYNClib_Channel_P_StartTimer_isr(hChn, psAudioSource->psUnmuteTimer, 
				hChn->hParent->sSettings.sAudio.uiUnmuteTimeout, 
				&BSYNClib_MuteControl_P_AudioSourceUnmuteTimerExpired, psAudioSource, 0);
			BKNI_LeaveCriticalSection();
			if (rc) goto error;
		}
	}
	BSYSlib_List_ReleaseIterator(hIterator);

	if (count)
	{
		BDBG_MSG(("[%d] Scheduled %u unmute timers", hChn->iIndex, count));
	}

	goto end;

error:
	if (hIterator)
	{
		BSYSlib_List_ReleaseIterator(hIterator);
	}

end:
	return rc;
}

BERR_Code BSYNClib_MuteControl_P_TaskTimerExpired(void * pvParm1, int iParm2, BSYSlib_Timer_Handle hTimer)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_Channel_Handle hChn = pvParm1;

	BSTD_UNUSED(iParm2);
	BDBG_ASSERT(hChn);
	BDBG_ASSERT(hTimer);

	BDBG_MSG(("[%d] Mute task timer expired", hChn->iIndex));

	/* clean up this timer */
	rc = BSYNClib_Channel_P_TimerExpired(hChn, hTimer);
	if (rc) goto end;

	rc = BSYNClib_MuteControl_P_Process(hChn, 0);

end:
	return rc;
}

BERR_Code BSYNClib_MuteControl_P_Process(void * pvParm1, int iParm2)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_Channel_Handle hChn = pvParm1;
	BSYSlib_List_IteratorHandle hIterator;
	BSYNClib_VideoSource * psVideoSource;
	BSYNClib_AudioSource * psAudioSource;
	bool bMutePending;
	bool bStarted;
	unsigned count;

	BSTD_UNUSED(iParm2);
	BDBG_ASSERT(hChn);

	BKNI_EnterCriticalSection();
	hChn->sResults.bMuteTaskScheduled = false;
	BKNI_LeaveCriticalSection();

	BDBG_MSG(("[%d] Mute control process invoked", hChn->iIndex));

	if (hChn->sConfig.sMuteControl.bEnabled)
	{
		/* check fullscreen-ed-ness */
		if (!BSYNClib_MuteControl_P_FullScreenCheck(hChn))
		{
			rc = BSYNClib_MuteControl_P_UnmuteAll(hChn);
		}
		else
		{
			BDBG_MSG(("[%d]  Checking video sources for pending mutes or starts", hChn->iIndex));
			count = 0;
			/* check for pending video source mutes */
			hIterator = BSYSlib_List_AcquireIterator(hChn->sVideo.hSources);
			while (BSYSlib_List_HasNext(hIterator))
			{
				psVideoSource = (BSYNClib_VideoSource *)BSYSlib_List_Next(hIterator);
				BKNI_EnterCriticalSection();
				bMutePending = psVideoSource->sResults.bMutePending;
				bStarted = psVideoSource->sElement.sData.bStarted;
				BKNI_LeaveCriticalSection();

				if (bMutePending)
				{
					count++;
					rc = BSYNClib_MuteControl_P_HandleVideoSourceMutePending(hChn, psVideoSource);
					if (rc) goto end;
				}

#if BSYNCLIB_UNCONDITIONAL_VIDEO_UNMUTE_SUPPORT
				if (bStarted)
				{
					count++;
					/* need to start this when video is started, not when it is muted */
					if (!psVideoSource->psUnconditionalUnmuteTimer->bScheduled)
					{
						BDBG_MSG(("[%d]  Scheduling video unconditional unmute timer", hChn->iIndex));
						/* schedule unconditional unmute timer */
						BKNI_EnterCriticalSection();
						rc = BSYNClib_Channel_P_StartTimer_isr(hChn, psVideoSource->psUnconditionalUnmuteTimer, 
							hChn->hParent->sSettings.sVideo.uiUnconditionalUnmuteTimeout, 
							&BSYNClib_MuteControl_P_VideoSourceUnconditionalUnmuteTimerExpired, psVideoSource, 0);
						BKNI_LeaveCriticalSection();
						if (rc) goto end;
					}
				}
#endif
			}
			BSYSlib_List_ReleaseIterator(hIterator);
			if (!count) 
			{
				BDBG_MSG(("[%d]    No pending video source mutes or starts found", hChn->iIndex));
			}

			BDBG_MSG(("[%d]  Checking audio sources for pending mutes or starts", hChn->iIndex));
			count = 0;
			/* check for pending audio source mutes and start events */
			hIterator = BSYSlib_List_AcquireIterator(hChn->sAudio.hSources);
			while (BSYSlib_List_HasNext(hIterator))
			{
				psAudioSource = (BSYNClib_AudioSource *)BSYSlib_List_Next(hIterator);
				BKNI_EnterCriticalSection();
				bMutePending = psAudioSource->sResults.bMutePending;
				bStarted = psAudioSource->sElement.sData.bStarted;
				BKNI_LeaveCriticalSection();

				if (bMutePending)
				{
					count++;
					rc = BSYNClib_MuteControl_P_HandleAudioSourceMutePending(hChn, psAudioSource);
					if (rc) goto end;
				}

#if BSYNCLIB_UNCONDITIONAL_AUDIO_UNMUTE_SUPPORT
				if (bStarted)
				{
					count++;
					/* need to start this when audio is started, not when it is muted */
					if (!psAudioSource->psUnconditionalUnmuteTimer->bScheduled)
					{
						BDBG_MSG(("[%d]  Scheduling audio unconditional unmute timer", hChn->iIndex));
						/* schedule unconditional unmute timer */
						BKNI_EnterCriticalSection();
						rc = BSYNClib_Channel_P_StartTimer_isr(hChn, psAudioSource->psUnconditionalUnmuteTimer, 
							hChn->hParent->sSettings.sAudio.uiUnconditionalUnmuteTimeout, 
							&BSYNClib_MuteControl_P_AudioSourceUnconditionalUnmuteTimerExpired, psAudioSource, 0);
						BKNI_LeaveCriticalSection();
						if (rc) goto end;
					}
				}
#endif
			}
			BSYSlib_List_ReleaseIterator(hIterator);
			if (!count) 
			{
				BDBG_MSG(("[%d]    No pending audio source mutes or starts found", hChn->iIndex));
			}
		}
	}
	else
	{
		BDBG_MSG(("[%d] Mute control process called while mute control disabled", hChn->iIndex));
	}

end:
	return rc;
}

BERR_Code BSYNClib_MuteControl_P_HandleVideoSourceMutePending(BSYNClib_Channel_Handle hChn, BSYNClib_VideoSource * psSource)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_AudioSource * psAudioSource;
	bool bAudioSourceSynchronized;
	BSYSlib_List_IteratorHandle hIterator;

	BDBG_ASSERT(hChn);
	BDBG_ASSERT(psSource);

	BDBG_MSG(("[%d]  Found pending video source mute", hChn->iIndex));

	/* check if any audio sources are synchronized */
	bAudioSourceSynchronized = false;
	hIterator = BSYSlib_List_AcquireIterator(hChn->sAudio.hSources);
	while (BSYSlib_List_HasNext(hIterator))
	{
		psAudioSource = (BSYNClib_AudioSource *)BSYSlib_List_Next(hIterator);

		if (psAudioSource->sConfig.bSynchronize)/* at mute time, no snapshot has been made */
		{
			bAudioSourceSynchronized = true;
			break;
		}
	}
	BSYSlib_List_ReleaseIterator(hIterator);
	
	if (bAudioSourceSynchronized)
	{
		/* only mute synchronized video sources */
		if (psSource->sConfig.bSynchronize) /* at mute time, no snapshot has been made */
		{
			/* only mute if full screen check passes */
			if (BSYNClib_MuteControl_P_FullScreenCheck(hChn))
			{
				/* user intended this to be an av session, mute video until done with sync */
				BDBG_MSG(("[%d]    Video source synchronized in av session, muting", hChn->iIndex));
				rc = BSYNClib_VideoSource_SetMute(psSource, true);
				if (rc) goto end;
			}
			else
			{
				BSYNClib_MuteControl_P_UnmuteAll(hChn);
			}
		}
		else
		{
			BDBG_MSG(("[%d]    Video source not synchronized, pending mute postponed", hChn->iIndex));
		}
	}
	else
	{
		BDBG_MSG(("[%d]    No synchronized audio sources, pending mute postponed", hChn->iIndex));
	}

end:

	return rc;
}

BERR_Code BSYNClib_MuteControl_P_HandleAudioSourceMutePending(BSYNClib_Channel_Handle hChn, BSYNClib_AudioSource * psSource)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_VideoSource * psVideoSource;
	bool bVideoSourceSynchronized;
	BSYSlib_List_IteratorHandle hIterator;

	BDBG_ASSERT(hChn);
	BDBG_ASSERT(psSource);

	BDBG_MSG(("[%d]  Found pending audio source mute", hChn->iIndex));

	/* check if any video sources are synchronized */
	bVideoSourceSynchronized = false;
	hIterator = BSYSlib_List_AcquireIterator(hChn->sVideo.hSources);
	while (BSYSlib_List_HasNext(hIterator))
	{
		psVideoSource = (BSYNClib_VideoSource *)BSYSlib_List_Next(hIterator);

		if (psVideoSource->sConfig.bSynchronize)/* at mute time, no snapshot has been made */
		{
			bVideoSourceSynchronized = true;
			break;
		}
	}
	BSYSlib_List_ReleaseIterator(hIterator);
	
	if (bVideoSourceSynchronized)
	{
		/* only mute synchronized audio sources */
		if (psSource->sConfig.bSynchronize)/* at mute time, no snapshot has been made */
		{
			/* only mute if full screen check passes */
			if (BSYNClib_MuteControl_P_FullScreenCheck(hChn))
			{
				/* user intended this to be an av session, mute audio until done with sync */
				BDBG_MSG(("[%d]    Audio source synchronized in av session, muting", hChn->iIndex));
				rc = BSYNClib_AudioSource_SetMute(psSource, true);
				if (rc) goto end;
			}
			else
			{
				BDBG_MSG(("[%d] Ignoring audio mute request", hChn->iIndex));
			}
		}
		else
		{
			BDBG_MSG(("[%d]    Audio source not synchronized, pending mute postponed", hChn->iIndex));
		}
	}
	else
	{
		BDBG_MSG(("[%d]    No synchronized video sources, pending mute postponed", hChn->iIndex));
	}

end:

	return rc;
}

bool BSYNClib_MuteControl_P_FullScreenCheck(BSYNClib_Channel_Handle hChn)
{
	BSYSlib_List_IteratorHandle hIterator;
	BSYNClib_VideoSink * psVideoSink;
	bool bFullScreenPass = true;

	BDBG_ASSERT(hChn);

	if (hChn->hParent->sSettings.sVideo.bRequireFullScreen)
	{
		hIterator = BSYSlib_List_AcquireIterator(hChn->sVideo.hSinks);
		while (BSYSlib_List_HasNext(hIterator))
		{
			psVideoSink = (BSYNClib_VideoSink *)BSYSlib_List_Next(hIterator);

			if (!psVideoSink->sConfig.bFullScreen)/* at mute time, no snapshot has been made */
			{
				BDBG_MSG(("[%d] Full screen check fail", hChn->iIndex));
				bFullScreenPass = false;
				break;
			}
		}
		BSYSlib_List_ReleaseIterator(hIterator);
	}

	return bFullScreenPass;
}

BERR_Code BSYNClib_MuteControl_P_UnmuteAll(BSYNClib_Channel_Handle hChn)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYSlib_List_IteratorHandle hIterator;
	BSYNClib_VideoSource * psVideoSource;
	BSYNClib_AudioSource * psAudioSource;

	BDBG_ASSERT(hChn);

	BDBG_MSG(("[%d] Unmuting all sources", hChn->iIndex));

	hIterator = BSYSlib_List_AcquireIterator(hChn->sVideo.hSources);
	while (BSYSlib_List_HasNext(hIterator))
	{
		psVideoSource = (BSYNClib_VideoSource *)BSYSlib_List_Next(hIterator);

		/* unmute */
		rc = BSYNClib_VideoSource_SetMute(psVideoSource, false);
		if (rc) goto error;
	}
	BSYSlib_List_ReleaseIterator(hIterator);

	hIterator = BSYSlib_List_AcquireIterator(hChn->sAudio.hSources);
	while (BSYSlib_List_HasNext(hIterator))
	{
		psAudioSource = (BSYNClib_AudioSource *)BSYSlib_List_Next(hIterator);

		/* unmute */
		rc = BSYNClib_AudioSource_SetMute(psAudioSource, false);
		if (rc) goto error;
	}
	BSYSlib_List_ReleaseIterator(hIterator);

	goto end;

error:
	if (hIterator)
	{
		BSYSlib_List_ReleaseIterator(hIterator);
	}

end:
	return rc;
}

BERR_Code BSYNClib_MuteControl_P_VideoSourceUnmuteTimerExpired(void * pvParm1, int iParm2, BSYSlib_Timer_Handle hTimer)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_VideoSource * psSource = pvParm1;
	BSYNClib_Channel_Handle hChn;
	
	BDBG_ENTER(BSYNClib_MuteControl_P_VideoSourceUnmuteTimerExpired);

	BSTD_UNUSED(iParm2);

	BDBG_ASSERT(psSource);
	BDBG_ASSERT(hTimer);

	hChn = psSource->sElement.hParent;

	/* clean up timer */
	rc = BSYNClib_Channel_P_TimerExpired(hChn, hTimer);
	if (rc) goto end;

	BDBG_MSG(("[%d] Video source %u unmute timer expired", hChn->iIndex, psSource->sElement.uiIndex));

	if (hChn->sConfig.sMuteControl.bSimultaneousUnmute)
	{
		/* TODO: may want to make this fire on longest timeout, instead of shortest */
		rc = BSYNClib_MuteControl_P_UnmuteAll(hChn);
	}
	else
	{
		rc = BSYNClib_VideoSource_SetMute(psSource, false);
		if (rc) goto end;
	}

end:
	BDBG_LEAVE(BSYNClib_MuteControl_P_VideoSourceUnmuteTimerExpired);
	return rc;
}

BERR_Code BSYNClib_MuteControl_P_AudioSourceUnmuteTimerExpired(void * pvParm1, int iParm2, BSYSlib_Timer_Handle hTimer)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_AudioSource * psSource = pvParm1;
	BSYNClib_Channel_Handle hChn;
	
	BDBG_ENTER(BSYNClib_MuteControl_P_AudioSourceUnmuteTimerExpired);

	BSTD_UNUSED(iParm2);

	BDBG_ASSERT(psSource);
	BDBG_ASSERT(hTimer);

	hChn = psSource->sElement.hParent;

	rc = BSYNClib_Channel_P_TimerExpired(hChn, hTimer);
	if (rc) goto end;

	BDBG_MSG(("[%d] Audio source %u unmute timer expired", hChn->iIndex, psSource->sElement.uiIndex));

	if (hChn->sConfig.sMuteControl.bSimultaneousUnmute)
	{
		/* TODO: may want to make this fire on longest timeout, instead of shortest */
		rc = BSYNClib_MuteControl_P_UnmuteAll(hChn);
	}
	else
	{
		/* unmute the audio */
		rc = BSYNClib_AudioSource_SetMute(psSource, false);
	}

end:

	BDBG_LEAVE(BSYNClib_MuteControl_P_AudioSourceUnmuteTimerExpired);
	return rc;
}

#if BSYNCLIB_UNCONDITIONAL_AUDIO_UNMUTE_SUPPORT
BERR_Code BSYNClib_MuteControl_P_AudioSourceUnconditionalUnmuteTimerExpired(void *pvParm1,int iParm2,BSYSlib_Timer_Handle hTimer)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_AudioSource * psSource = pvParm1;
	BSYNClib_Channel_Handle hChn;
	
	BDBG_ENTER(BSYNClib_MuteControl_P_AudioSourceUnconditionalUnmuteTimerExpired);

	BSTD_UNUSED(iParm2);

	BDBG_ASSERT(psSource);
	BDBG_ASSERT(hTimer);

	hChn = psSource->sElement.hParent;

	rc = BSYNClib_Channel_P_TimerExpired(hChn, hTimer);
	if (rc) goto end;

	BDBG_MSG(("[%d] Audio source %u unconditional unmute timer expired", hChn->iIndex, psSource->sElement.uiIndex));

	if (hChn->sConfig.sMuteControl.bSimultaneousUnmute)
	{
		/* TODO: may want to make this fire on longest timeout, instead of shortest */
		rc = BSYNClib_MuteControl_P_UnmuteAll(hChn);
	}
	else
	{
		/* unmute the audio */
		rc = BSYNClib_AudioSource_SetMute(psSource, false);
	}

end:

	BDBG_LEAVE(BSYNClib_MuteControl_P_AudioSourceUnconditionalUnmuteTimerExpired);
	return rc;
}
#endif

#if BSYNCLIB_UNCONDITIONAL_VIDEO_UNMUTE_SUPPORT
BERR_Code BSYNClib_MuteControl_P_VideoSourceUnconditionalUnmuteTimerExpired(void *pvParm1,int iParm2,BSYSlib_Timer_Handle hTimer)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_VideoSource * psSource = pvParm1;
	BSYNClib_Channel_Handle hChn;
	
	BDBG_ENTER(BSYNClib_MuteControl_P_VideoSourceUnconditionalUnmuteTimerExpired);

	BSTD_UNUSED(iParm2);

	BDBG_ASSERT(psSource);
	BDBG_ASSERT(hTimer);

	hChn = psSource->sElement.hParent;

	rc = BSYNClib_Channel_P_TimerExpired(hChn, hTimer);
	if (rc) goto end;

	BDBG_MSG(("[%d] Video source %u unconditional unmute timer expired", hChn->iIndex, psSource->sElement.uiIndex));

	if (hChn->sConfig.sMuteControl.bSimultaneousUnmute)
	{
		/* TODO: may want to make this fire on longest timeout, instead of shortest */
		rc = BSYNClib_MuteControl_P_UnmuteAll(hChn);
	}
	else
	{
		/* unmute the video */
		rc = BSYNClib_VideoSource_SetMute(psSource, false);
	}

end:

	BDBG_LEAVE(BSYNClib_MuteControl_P_VideoSourceUnconditionalUnmuteTimerExpired);
	return rc;
}
#endif

