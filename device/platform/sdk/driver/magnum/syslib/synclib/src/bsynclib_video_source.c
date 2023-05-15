/***************************************************************************
*	  Copyright (c) 2004-2012, Broadcom Corporation
*	  All Rights Reserved
*	  Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_video_source.c $
* $brcm_Revision: Hydra_Software_Devel/17 $
* $brcm_Date: 8/31/12 4:51p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_video_source.c $
* 
* Hydra_Software_Devel/17   8/31/12 4:51p bandrews
* SW7420-2314: ensure that sync mute state matches decoder mute state
* 
* Hydra_Software_Devel/16   8/3/12 7:02p bandrews
* SW7420-2314: merge to main
* 
* Hydra_Software_Devel/SW7420-2314/1   6/12/12 9:05p bandrews
* SW7420-2314: don't mute decoders that are already started
* 
* Hydra_Software_Devel/15	1/19/12 8:39p bandrews
* SW7346-640: fix the units between estimated delay and measured delay
* and threshold
* 
* Hydra_Software_Devel/14	1/16/12 8:26p bandrews
* SW7346-640: preserve delay estimation when callbacks come in an
* unexpected order
* 
* Hydra_Software_Devel/13	5/28/10 6:37p bandrews
* SW7405-4436: print channel index in debug messages
* 
* Hydra_Software_Devel/12	4/29/10 2:52p bandrews
* SW7405-4083: forgot to change debug statements in isr to use isr
* version of convert
* 
* Hydra_Software_Devel/11	3/22/10 5:45p bandrews
* sw7405-4083: fix calls to isr
* 
* Hydra_Software_Devel/10	3/19/10 2:42p bandrews
* SW7405-3774: mute control merge
* 
* Hydra_Software_Devel/SW7405-3774/4   3/18/10 10:03p bandrews
* SW7405-3774: fixed case where video may unmute even with pending
* adjustment
* 
* Hydra_Software_Devel/SW7405-3774/3   3/18/10 9:34p bandrews
* SW7405-4042: rework delay estimation so that imprecise lipsync works
* correctly
* 
* Hydra_Software_Devel/SW7405-3774/2   3/18/10 8:20p bandrews
* SW7405-4042: imprecise lipsync needs to assume 1/2 frame delay
* 
* Hydra_Software_Devel/SW7405-3774/1   2/17/10 6:17p bandrews
* SW7405-3774: attempt to fix early audio unmute then mute
* 
* Hydra_Software_Devel/9   2/12/10 8:46p bandrews
* SW7405-3912: assert all inputs are not null
* 
* Hydra_Software_Devel/8   2/1/10 8:01p bandrews
* SW7405-3774: only mute/unmute synchronized sources
* 
* Hydra_Software_Devel/7   1/26/10 9:00p bandrews
* SW7405-3774: added mute control support
* 
* Hydra_Software_Devel/6   12/10/09 9:18p bandrews
* SW7401-3634: adding PWC (now JTI) support to synclib
* 
* Hydra_Software_Devel/5   8/4/09 4:56p bandrews
* PR52812: added improved rmd for dmv2
* 
* Hydra_Software_Devel/4   7/24/09 4:14p bandrews
* PR52812: Added any-time source or display rate change support
* 
* Hydra_Software_Devel/3   6/16/08 5:05p bandrews
* PR43010: Units conversion not from or to 27MHz needs 2 steps
* 
* Hydra_Software_Devel/2   4/3/08 2:33p bandrews
* PR40090: synclib needs to have different contexts for syslib callbacks
* and synclib callbacks
* 
* Hydra_Software_Devel/1   3/24/08 3:10p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/8   2/26/08 10:32p bandrews
* PR37951: Fixed validation of delay in low precision mode
* 
* Hydra_Software_Devel/7   2/26/08 10:21p bandrews
* PR37951: Fixed units.  Implemented static rate mismatch detection.
* 
* Hydra_Software_Devel/6   2/26/08 3:03p bandrews
* PR37951: Added video delay requantization
* 
* Hydra_Software_Devel/5   2/22/08 8:28p bandrews
* PR37951: Fixed bug in units conversion
* 
* Hydra_Software_Devel/4   2/21/08 6:02p bandrews
* PR37951: Default video source numbers to 45KHz domain.  Improved
* handling of unsynchronized elements
* 
* Hydra_Software_Devel/3   2/20/08 10:03p bandrews
* PR37951: Updated based on feedback from usage
* 
* Hydra_Software_Devel/2   1/3/08 5:17p bandrews
* PR37951: Updated based on initial feedback
* 
* Hydra_Software_Devel/1   12/12/07 2:54p bandrews
* PR37951: Initial check-in
***************************************************************************/

#include "bstd.h"
#include "bsynclib_priv.h"
#include "bsynclib_video_source.h"
#include "bsynclib_channel_priv.h"
#include "bsynclib_mute_control.h"

BDBG_MODULE(synclib);

#define BSYNCLIB_P_VIDEO_SOURCE_DEFAULT_UNITS BSYNClib_Units_e45KhzTicks
#define BSYNCLIB_P_VIDEO_SOURCE_DEFAULT_THRESHOLD_VALUE 90 /* 2 ms @ 45 KHz */
#define BSYNCLIB_P_VIDEO_SOURCE_DEFAULT_JITTER_TOLERANCE_IMPROVEMENT_THRESHOLD_VALUE 1 /* 1 ms */

BSYNClib_VideoSource * BSYNClib_VideoSource_Create(void)
{
	BSYNClib_VideoSource * psSource = NULL;

	BDBG_ENTER(BSYNClib_VideoSource_Create);

	psSource = (BSYNClib_VideoSource *)BKNI_Malloc(sizeof(BSYNClib_VideoSource));

	if (psSource)
	{
		BKNI_Memset(psSource, 0, sizeof(BSYNClib_VideoSource));
		BSYNClib_DelayElement_Init(&psSource->sElement);
		BSYNClib_VideoFormat_Init(&psSource->sFormat);
		BSYNClib_VideoSource_GetDefaultConfig(&psSource->sConfig);
		BSYNClib_VideoSource_P_GetDefaultStatus(&psSource->sStatus);
		psSource->sElement.sDelay.sData.eOriginalUnits = BSYNCLIB_P_VIDEO_SOURCE_DEFAULT_UNITS;
		psSource->sElement.sDelay.sSnapshot.eOriginalUnits = BSYNCLIB_P_VIDEO_SOURCE_DEFAULT_UNITS;
		psSource->sElement.sNotification.sResults.sThreshold.eUnits = BSYNCLIB_P_VIDEO_SOURCE_DEFAULT_UNITS;
		psSource->sElement.sNotification.sResults.sThreshold.uiValue = BSYNCLIB_P_VIDEO_SOURCE_DEFAULT_THRESHOLD_VALUE; 
	}

	BDBG_LEAVE(BSYNClib_VideoSource_Create);
	return psSource;
}

void BSYNClib_VideoSource_Destroy(BSYNClib_VideoSource * psSource)
{
	BDBG_ENTER(BSYNClib_VideoSource_Destroy);

	BDBG_ASSERT(psSource);

	BKNI_Free(psSource);

	BDBG_LEAVE(BSYNClib_VideoSource_Destroy);
}

bool BSYNClib_VideoSource_SyncCheck(BSYNClib_VideoSource * psSource)
{
	bool bPass = false;
	
	BDBG_ENTER(BSYNClib_VideoSource_SyncCheck);

	BDBG_ASSERT(psSource);

	bPass = !psSource->sElement.sSnapshot.bSynchronize /* if we must not synchronize this element */
		|| (psSource->sElement.sSnapshot.bStarted /* must be started */
		&& psSource->sFormat.sSnapshot.bValid /* have a valid format */
		&& psSource->sElement.sDelay.sSnapshot.bValid /* have a valid measured delay */
		&& psSource->sElement.sDelay.sResults.bAccepted); /* and be accepted */

	BDBG_MSG(("[%d] Video source %u sync check:", psSource->sElement.hParent->iIndex, psSource->sElement.uiIndex));
	BDBG_MSG(("[%d]  %s", psSource->sElement.hParent->iIndex, psSource->sElement.sSnapshot.bSynchronize ? "synchronized" : "ignored"));
	BDBG_MSG(("[%d]  %s", psSource->sElement.hParent->iIndex, psSource->sElement.sSnapshot.bStarted ? "started" : "stopped"));
	BDBG_MSG(("[%d]  format %s", psSource->sElement.hParent->iIndex, psSource->sFormat.sSnapshot.bValid ? "valid" : "invalid"));
	if (!psSource->sElement.sDelay.sResults.bEstimated)
	{
		BDBG_MSG(("[%d]  delay %s, %s", psSource->sElement.hParent->iIndex, psSource->sElement.sDelay.sSnapshot.bValid ? "valid" : "invalid", 
			psSource->sElement.sDelay.sResults.bAccepted ? "accepted" : "unaccepted"));
	}
	else
	{
		BDBG_MSG(("[%d]  delay %s, estimated", psSource->sElement.hParent->iIndex, psSource->sElement.sDelay.sSnapshot.bValid ? "valid" : "invalid"));
	}

	BDBG_LEAVE(BSYNClib_VideoSource_SyncCheck);
	return bPass;
}

void BSYNClib_VideoSource_Reset_isr(BSYNClib_VideoSource * psSource)
{
	BDBG_ENTER(BSYNClib_VideoSource_Reset_isr);

	BDBG_ASSERT(psSource);

	BSYNClib_Timer_Reset_isr(psSource->psTsmLockTimer);

	psSource->sData.bDigital = true;
	psSource->sData.bLastPictureHeld = false;
	psSource->sResults.bDelaySaved = false;
	psSource->sResults.sJtiFactor.bAdjusted = false;
	psSource->sResults.sJtiFactor.iValue = 0;

	BDBG_LEAVE(BSYNClib_VideoSource_Reset_isr);
}

BERR_Code BSYNClib_VideoSource_TsmLockTimerExpired(void * pvParm1, int iParm2, BSYSlib_Timer_Handle hTimer)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_VideoSource * psSource = pvParm1;
	
	BDBG_ENTER(BSYNClib_VideoSource_TsmLockTimerExpired);

	BSTD_UNUSED(iParm2);

	BDBG_ASSERT(psSource);
	BDBG_ASSERT(hTimer);

	BDBG_MSG(("[%d] Video source %u tsm lock timer expired", psSource->sElement.hParent->iIndex, psSource->sElement.uiIndex));

	psSource->sElement.sDelay.sResults.bAccepted = true;

	rc = BSYNClib_Channel_P_TimerExpired(psSource->sElement.hParent, hTimer);
	if (rc) goto end;

	BSYNClib_Channel_P_Process(psSource->sElement.hParent, 0);

end:

	BDBG_LEAVE(BSYNClib_VideoSource_TsmLockTimerExpired);
	return rc;
}

BERR_Code BSYNClib_VideoSource_SetMute(BSYNClib_VideoSource * psSource, bool bMute)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_Channel_Handle hChn;
	BSYNClib_Channel_MuteCallback * pcbMute;

	BDBG_ENTER(BSYNClib_VideoSource_SetMute);
	
	BDBG_ASSERT(psSource);
	
	hChn = psSource->sElement.hParent;
	pcbMute = &hChn->sSettings.sVideo.sSource.cbMute;

	BKNI_EnterCriticalSection();
	psSource->sResults.bMutePending = false;
	
	/* cancel any pending unmute timer */
	rc = BSYNClib_Channel_P_CancelTimer_isr(hChn, psSource->psUnmuteTimer);
	BKNI_LeaveCriticalSection();
	if (rc) goto end;

	/* cancel any pending unconditional unmute timer */
	if (!bMute)
	{
		BKNI_EnterCriticalSection();
		rc = BSYNClib_Channel_P_CancelTimer_isr(hChn, psSource->psUnconditionalUnmuteTimer);
		BKNI_LeaveCriticalSection();
		if (rc) goto end;
	}

	/* handling of video channel change mode is in upper layer software, as what constitutes a 
	mute depends on that mode. May want to revisit inclusion in synclib later. 
	The logic should be something like:
		if (sync wants to mute)
		{
			switch (channel change mode)
			{
				case BXVD_ChannelChangeMode_eMute:
					BXVD_EnableMute(true);
					break;
				case BXVD_ChannelChangeMode_eLastFramePreviousChannel:
				case BXVD_ChannelChangeMode_eMuteWithFirstPicturePreview:
				case BXVD_ChannelChangeMode_eLastFramePreviousWithFirstPicturePreview:
					BXVD_EnableVideoFreeze();
					break;
				default:
					break;
			}
		}
		else if (sync wants to unmute)
		{
			switch (channel change mode)
			{
				case BXVD_ChannelChangeMode_eMute:
					BXVD_EnableMute(false);
					break;
				case BXVD_ChannelChangeMode_eLastFramePreviousChannel:
				case BXVD_ChannelChangeMode_eMuteWithFirstPicturePreview:
				case BXVD_ChannelChangeMode_eLastFramePreviousWithFirstPicturePreview:
					BXVD_DisableVideoFreeze();
					break;
				default:
					break;
			}
		}
	This assumes that the FPP modes already show the first picture and freeze until the next
	picture matures.  In this case, sync will just extend that freeze time a little bit.
	*/
	if (psSource->sConfig.bSynchronize && pcbMute->pfSetMute)/* at mute time, no snapshot has been made */
	{
		rc = pcbMute->pfSetMute(pcbMute->pvParm1, pcbMute->iParm2, psSource->sElement.uiIndex, bMute);
		if(rc) goto end;
		psSource->sStatus.bMuted = bMute;
	}

end:
	BDBG_LEAVE(BSYNClib_VideoSource_SetMute);
	return rc;
}

static void BSYNClib_VideoSource_RestoreDelay_isr(BSYNClib_VideoSource * psSource)
{
	BDBG_ENTER(BSYNClib_VideoSource_RestoreDelay_isr);

	BDBG_ASSERT(psSource);

	if (psSource->sResults.bDelaySaved)
	{
		psSource->sElement.sDelay = psSource->sResults.sSavedDelay;
		BDBG_MSG(("[%d]  restored delay state: accepted = %s, diff = %u ms", psSource->sElement.hParent->iIndex, psSource->sElement.sDelay.sResults.bAccepted ? "true" : "false", BSYNClib_P_Convert_isr(psSource->sElement.sDelay.sData.uiMeasured, BSYNClib_Units_e27MhzTicks, BSYNClib_Units_eMilliseconds)));
		psSource->sResults.bDelaySaved = false;
		psSource->sElement.sDelay.sResults.bEstimated = false;
	}

	BDBG_LEAVE(BSYNClib_VideoSource_RestoreDelay_isr);
}

static void BSYNClib_VideoSource_SaveDelay_isr(BSYNClib_VideoSource * psSource)
{
	BDBG_ENTER(BSYNClib_VideoSource_SaveDelay_isr);

	BDBG_ASSERT(psSource);

	if (psSource->sElement.sDelay.sData.bValid && !psSource->sElement.sDelay.sResults.bEstimated)
	{
		psSource->sResults.sSavedDelay = psSource->sElement.sDelay;
		psSource->sResults.bDelaySaved = true;
		BDBG_MSG(("[%d]  saved delay state: accepted = %s, diff = %u ms", psSource->sElement.hParent->iIndex, psSource->sElement.sDelay.sResults.bAccepted ? "true" : "false", BSYNClib_P_Convert_isr(psSource->sElement.sDelay.sData.uiMeasured, BSYNClib_Units_e27MhzTicks, BSYNClib_Units_eMilliseconds)));
	}
	
	BDBG_LEAVE(BSYNClib_VideoSource_SaveDelay_isr);
}

bool BSYNClib_VideoSource_EstimateDelay_isr(BSYNClib_VideoSource * psSource)
{
	bool bChanged = false;
	unsigned int uiOldMeasured;
	
	BDBG_ENTER(BSYNClib_VideoSource_EstimateDelay_isr);

	BDBG_ASSERT(psSource);

	psSource->sElement.sDelay.sData.bValid = true;
	psSource->sElement.sDelay.sResults.bEstimated = true;
	
	if (psSource->sFormat.sData.bValid)
	{
		/* only accept if we actually have a value from receipt of format info */
		psSource->sElement.sDelay.sResults.bAccepted = true;

		uiOldMeasured = psSource->sElement.sDelay.sData.uiMeasured;
		
		/* assume we are centered in pass window */
		psSource->sElement.sDelay.sData.eOriginalUnits = BSYNClib_Units_e27MhzTicks;
		psSource->sElement.sDelay.sData.uiMeasured = BSYNClib_VideoFormat_P_GetFramePeriod_isr(&psSource->sFormat) / 2;

		if (uiOldMeasured != psSource->sElement.sDelay.sData.uiMeasured)
		{
			bChanged = true;
		}
	}
	
	BDBG_LEAVE(BSYNClib_VideoSource_EstimateDelay_isr);
	return bChanged;
}

BERR_Code BSYNClib_VideoSource_RateMismatchDetected(BSYNClib_VideoSource * psSource)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_Channel_Handle hChn;
	
	BDBG_ENTER(BSYNClib_VideoSource_RateMismatchDetected);

	BDBG_ASSERT(psSource);

	hChn = psSource->sElement.hParent;

	BDBG_WRN(("[%d] Video source %u rate mismatch detected!  Disabling precision lipsync.", hChn->iIndex, psSource->sElement.uiIndex));
	psSource->sElement.sNotification.sResults.bEnabled = false;

	BKNI_EnterCriticalSection();
	BSYNClib_VideoSource_SaveDelay_isr(psSource);
	BSYNClib_VideoSource_EstimateDelay_isr(psSource);
	BKNI_LeaveCriticalSection();

	if (hChn->sSettings.sVideo.sSource.cbDelay.pfSetDelayNotification)
	{
		rc = hChn->sSettings.sVideo.sSource.cbDelay.pfSetDelayNotification(
			hChn->sSettings.sVideo.sSource.cbDelay.pvParm1, 
			hChn->sSettings.sVideo.sSource.cbDelay.iParm2, 
			psSource->sElement.uiIndex, 
			psSource->sElement.sNotification.sResults.bEnabled, 
			&psSource->sElement.sNotification.sResults.sThreshold);
	}
	
	BDBG_LEAVE(BSYNClib_VideoSource_RateMismatchDetected);
	return rc;
}

BERR_Code BSYNClib_VideoSource_RateRematchDetected(BSYNClib_VideoSource * psSource)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_Channel_Handle hChn;

	BDBG_ENTER(BSYNClib_VideoSource_RateRematchDetected);

	BDBG_ASSERT(psSource);

	hChn = psSource->sElement.hParent;

	BDBG_WRN(("[%d] Video source %u rate appears to be matched.  Precision lipsync enabled.", hChn->iIndex, psSource->sElement.uiIndex));
	psSource->sElement.sNotification.sResults.bEnabled = true;

	BKNI_EnterCriticalSection();
	BSYNClib_VideoSource_RestoreDelay_isr(psSource);
	BKNI_LeaveCriticalSection();

	/* TODO: make this so we don't need hChn to do this */
	if (hChn->sSettings.sVideo.sSource.cbDelay.pfSetDelayNotification)
	{
		rc = hChn->sSettings.sVideo.sSource.cbDelay.pfSetDelayNotification(
			hChn->sSettings.sVideo.sSource.cbDelay.pvParm1, 
			hChn->sSettings.sVideo.sSource.cbDelay.iParm2, 
			psSource->sElement.uiIndex, 
			psSource->sElement.sNotification.sResults.bEnabled, 
			&psSource->sElement.sNotification.sResults.sThreshold);
	}
	
	BDBG_LEAVE(BSYNClib_VideoSource_RateRematchDetected);
	return rc;
}

void BSYNClib_VideoSource_GetDefaultConfig(BSYNClib_VideoSource_Config * psConfig)
{
	BDBG_ENTER(BSYNClib_VideoSource_GetDefaultConfig);

	BDBG_ASSERT(psConfig);
	
	BKNI_Memset(psConfig, 0, sizeof(BSYNClib_VideoSource_Config));

	psConfig->bDigital = true;
	psConfig->bLastPictureHeld = true;
	psConfig->sFormat.bInterlaced = true;
	psConfig->sFormat.eFrameRate = BAVC_FrameRateCode_eUnknown;
	psConfig->sDelay.sMeasured.eUnits = BSYNClib_Units_e45KhzTicks;
	psConfig->sJitterToleranceImprovementThreshold.uiValue = BSYNCLIB_P_VIDEO_SOURCE_DEFAULT_JITTER_TOLERANCE_IMPROVEMENT_THRESHOLD_VALUE;
	psConfig->sJitterToleranceImprovementThreshold.eUnits = BSYNClib_Units_eMilliseconds;

	BDBG_LEAVE(BSYNClib_VideoSource_GetDefaultConfig);
}

void BSYNClib_VideoSource_P_SelfClearConfig_isr(BSYNClib_VideoSource * psSource)
{
	BDBG_ENTER(BSYNClib_VideoSource_P_SelfClearConfig_isr);

	BDBG_ASSERT(psSource);
	
	psSource->sConfig.sDelay.bReceived = false;
	psSource->sConfig.sFormat.bReceived = false;

	BDBG_LEAVE(BSYNClib_VideoSource_P_SelfClearConfig_isr);
}

BERR_Code BSYNClib_VideoSource_P_ProcessConfig_isr(BSYNClib_VideoSource * psSource)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_VideoSource_Config * psConfig;
	BSYNClib_Channel_Handle hChn;
	bool bChanged = false;
	bool bScheduleMuteTask = false;
	BSYNClib_DelayElement sDesired;
	BSYNClib_DelayElement * psCurrent;
	BSYNClib_DelayElement_DiffResults sElementDiffResults;
	BSYNClib_VideoFormat sFormat;
	bool bFormatReceived;
	BSYNClib_VideoFormat_DiffResults sFormatDiffResults;

	BDBG_ENTER(BSYNClib_VideoSource_P_ProcessConfig_isr);
	
	BDBG_ASSERT(psSource);

	hChn = psSource->sElement.hParent;
	psConfig = &psSource->sConfig;
	psCurrent = &psSource->sElement;

	BKNI_Memset(&sElementDiffResults, 0, sizeof(BSYNClib_DelayElement_DiffResults));

	/* check lifecycle first */
	BSYNClib_DelayElement_CheckLifecycle_isr(psConfig->bStarted, psCurrent, &sElementDiffResults);
	if (sElementDiffResults.eLifecycleEvent == BSYNClib_DelayElement_LifecycleEvent_eStarted)
	{
		BSYNClib_DelayElement_Reset_isr(psCurrent);
	}

	/* create "desired" delay element config from current plus changes */
	sDesired = *psCurrent;
	sDesired.sData.bStarted = psConfig->bStarted;
	sDesired.sData.bSynchronize = psConfig->bSynchronize;
	/* if we estimated the delay previously, we shouldn't overwrite it with the measured delay, unless
	we've really received it this time */
	if (psConfig->sDelay.bReceived)
	{
		sDesired.sDelay.sData.eOriginalUnits = psConfig->sDelay.sMeasured.eUnits;
		sDesired.sDelay.sData.uiMeasured = BSYNClib_P_Convert_isr(psConfig->sDelay.sMeasured.uiValue, psConfig->sDelay.sMeasured.eUnits, BSYNClib_Units_e27MhzTicks);
	}
	sDesired.sDelay.sData.uiCustom = BSYNClib_P_Convert_isr(psConfig->sDelay.sCustom.uiValue, psConfig->sDelay.sCustom.eUnits, BSYNClib_Units_e27MhzTicks);
	sDesired.sNotification.sData.bReceived = psConfig->sDelay.bReceived;
	/* threshold will always be converted to preferred units */
#if 0
	/* convert status units to user's measurement units */
	psSource->sStatus.sDelayNotification.sThreshold.uiValue = BSYNClib_P_Convert_isr(psSource->sStatus.sDelayNotification.sThreshold.uiValue, psSource->sStatus.sDelayNotification.sThreshold.eUnits, BSYNClib_Units_e27MhzTicks);
	/* 20080616 bandrews - conversion requires 2 steps */
	psSource->sStatus.sDelayNotification.sThreshold.uiValue = BSYNClib_P_Convert_isr(psSource->sStatus.sDelayNotification.sThreshold.uiValue, BSYNClib_Units_e27MhzTicks, psConfig->sDelay.sMeasured.eUnits);
	psSource->sStatus.sDelayNotification.sThreshold.eUnits = psConfig->sDelay.sMeasured.eUnits;
#endif

	BSYNClib_DelayElement_Diff_isr(&sDesired, psCurrent, &sElementDiffResults);

	if (sElementDiffResults.bChanged)
	{
		bChanged = true;
		BSYNClib_DelayElement_Patch_isr(&sDesired, psCurrent, &sElementDiffResults);
	}
	
	/* if we want to synchronize this source, tell mute control */
	/* must do this on sync change, not on start, otherwise it's too late */
	switch (sElementDiffResults.eSynchronizationEvent)
	{
		case BSYNClib_DelayElement_SynchronizationEvent_eSynchronized:
			/* TODO: actually, we want to re-mute if already muted to converge mute states */
			/* SW7420-2314: do not mute if synchronizing but already started */
			if (!psSource->sElement.sData.bStarted)
			{
				psSource->sResults.bMutePending = true;
				bScheduleMuteTask = true;
			}
			else
			{
				psSource->sResults.bMutePending = false;
			}
			break;
		case BSYNClib_DelayElement_SynchronizationEvent_eIgnored:
			psSource->sResults.bMutePending = false;
			break;
		case BSYNClib_DelayElement_SynchronizationEvent_eNone:
		default:
			break;
	}
	
	if (sElementDiffResults.eLifecycleEvent == BSYNClib_DelayElement_LifecycleEvent_eStarted)
	{
		/* reset source data */
		BSYNClib_VideoSource_Reset_isr(psSource);
		BSYNClib_VideoFormat_Reset_isr(&psSource->sFormat);

		/* reset rmd data */
		rc = BSYNClib_RateMismatchDetector_ResetSourceData_isr(hChn->hDetector, psSource->sElement.uiIndex);
		if (rc) goto error;

		/* note: side effect of copying last picture held and digital here is
		that they only apply at start time.  They cannot be changed on the fly */
		
		/* copy last picture held for mute controller */
		psSource->sData.bLastPictureHeld = psConfig->bLastPictureHeld;

		/* source digital or analog */
		if (psConfig->bDigital != psSource->sData.bDigital)
		{
			bChanged = true;
			psSource->sData.bDigital = psConfig->bDigital;
		}

		/* fire setdelay with zero */
		psSource->sElement.sDelay.sResults.bGenerateCallback = true;

		if (psConfig->bSynchronize)
		{
		    /* we expect to start muted via sending the mute command once already on synchronization connection, but
		    this will ensure the state after stop/start is correct */
			psSource->sResults.bMutePending = true;
			/* notify mute control of the start of this synchronized source */
			bScheduleMuteTask = true;
		}
	}

	/* received a delay notification? */
	if (sElementDiffResults.bDelayReceived)
	{
		BSYNClib_RateMismatchDetector_DelayNotificationInfo sInfo;

		BDBG_MSG(("[%d] Video source %u delay received", hChn->iIndex, psSource->sElement.uiIndex));

		/* inform rmd of delay timing */
		if (hChn->sSettings.cbTime.pfGetTime_isr)
		{
			unsigned long ulTime;
			
			rc = hChn->sSettings.cbTime.pfGetTime_isr(hChn->sSettings.cbTime.pvParm1, hChn->sSettings.cbTime.iParm2, &ulTime);
			if (rc) goto error;
			
			sInfo.ulDelayNotificationTime = ulTime;
			
			rc = BSYNClib_RateMismatchDetector_DelayNotificationHandler_isr(hChn->hDetector, psSource->sElement.uiIndex, &sInfo);
			if (rc) goto error;
		}

		/* need to cancel any pending unmute timers */
		BSYNClib_MuteControl_CancelUnmuteTimers_isr(hChn);
		
		/* start video tsm lock timer */
		BDBG_MSG(("[%d]  tsm lock timer started", hChn->iIndex));
		rc = BSYNClib_Channel_P_StartTimer_isr(
			hChn, 
			psSource->psTsmLockTimer, 
			hChn->hParent->sSettings.sVideo.uiTsmLockTimeout,
			&BSYNClib_VideoSource_TsmLockTimerExpired,
			psSource,
			0
		);
		if (rc) goto error;

		/* TODO: cancel audio unconditional unmute timer */
	}

	bFormatReceived = psConfig->sFormat.bReceived;

	/* received source format info */
	if (bFormatReceived)
	{
		/* create "desired" video format config */
		sFormat.sData.bInterlaced = psConfig->sFormat.bInterlaced;
		sFormat.sData.uiHeight = psConfig->sFormat.uiHeight;
		sFormat.sData.eFrameRate = psConfig->sFormat.eFrameRate;

		BSYNClib_VideoFormat_Diff_isr(&sFormat, &psSource->sFormat, &sFormatDiffResults);

		if (sFormatDiffResults.bChanged)
		{
			bChanged = true;
			BSYNClib_VideoFormat_Patch_isr(&sFormat, &psSource->sFormat, &sFormatDiffResults);
		}
		
		/* set rmd source format */
		rc = BSYNClib_RateMismatchDetector_SetVideoSourceFormat_isr(hChn->hDetector, psSource->sElement.uiIndex, &sFormat);
		if (rc) goto error;
	}

	/* SW7405-4042 imprecise lipsync needs to assume 1/2 frame delay on each config change */
	/* or if we were estimated before, force re-estimation now */
	if (!hChn->bPrecisionLipsyncEnabled || psSource->sElement.sDelay.sResults.bEstimated)
	{
		if (BSYNClib_VideoSource_EstimateDelay_isr(psSource))
		{
			bChanged = true;
		}
	}

	/* JTI */
	{
		unsigned int uiJtiThreshold = BSYNClib_P_Convert_isr(psConfig->sJitterToleranceImprovementThreshold.uiValue, 
				psConfig->sJitterToleranceImprovementThreshold.eUnits, BSYNClib_Units_e27MhzTicks);

		if (uiJtiThreshold != psSource->sData.uiJitterToleranceImprovementThreshold)
		{
			bChanged = true;
			psSource->sData.uiJitterToleranceImprovementThreshold = uiJtiThreshold;
		}
	}

	if (bChanged)
	{
		BDBG_MSG(("[%d] Video source %u properties changed:", hChn->iIndex, psSource->sElement.uiIndex));
		if (sElementDiffResults.eSynchronizationEvent != BSYNClib_DelayElement_SynchronizationEvent_eNone)
		{
			BDBG_MSG(("[%d]  %s", hChn->iIndex, BSYNClib_DelayElement_SynchronizationEventNames[sElementDiffResults.eSynchronizationEvent]));
		}
		if (sElementDiffResults.eLifecycleEvent != BSYNClib_DelayElement_LifecycleEvent_eNone)
		{
			BDBG_MSG(("[%d]  %s", hChn->iIndex, BSYNClib_DelayElement_LifecycleEventNames[sElementDiffResults.eLifecycleEvent]));
		}
		if (psSource->sElement.sDelay.sResults.bEstimated)
		{
			BDBG_MSG(("[%d]  estimated delay %u ms", hChn->iIndex, BSYNClib_P_Convert_isr(psSource->sElement.sDelay.sData.uiMeasured, BSYNClib_Units_e27MhzTicks, BSYNClib_Units_eMilliseconds)));
		}
		else
		{
			BDBG_MSG(("[%d]  measured delay %u ms", hChn->iIndex, BSYNClib_P_Convert_isr(psSource->sElement.sDelay.sData.uiMeasured, BSYNClib_Units_e27MhzTicks, BSYNClib_Units_eMilliseconds)));
		}
		BDBG_MSG(("[%d]  custom delay %u ms", hChn->iIndex, BSYNClib_P_Convert_isr(psSource->sElement.sDelay.sData.uiCustom, BSYNClib_Units_e27MhzTicks, BSYNClib_Units_eMilliseconds)));
		BDBG_MSG(("[%d]  jti threshold %u ms", hChn->iIndex, BSYNClib_P_Convert_isr(psSource->sData.uiJitterToleranceImprovementThreshold, BSYNClib_Units_e27MhzTicks, BSYNClib_Units_eMilliseconds)));
		BDBG_MSG(("[%d]  %s", hChn->iIndex, psSource->sData.bDigital ? "digital" : "analog"));
		BDBG_MSG(("[%d]  %s on stop", hChn->iIndex, psSource->sData.bLastPictureHeld ? "last picture held" : "blank"));
		BDBG_MSG(("[%d]  format:", hChn->iIndex));
		BDBG_MSG(("[%d]    %s", hChn->iIndex, psSource->sFormat.sData.bInterlaced ? "interlaced" : "progressive"));
		BDBG_MSG(("[%d]    height %u", hChn->iIndex, psSource->sFormat.sData.uiHeight));
		BDBG_MSG(("[%d]    frame rate %s", hChn->iIndex, BSYNClib_VideoFormat_P_GetFrameRateName_isr(psSource->sFormat.sData.eFrameRate)));
		if (psSource->sResults.bMutePending)
		{
				BDBG_MSG(("[%d]  mute pending", hChn->iIndex));
		}
	}

	if (bScheduleMuteTask)
	{
		/* TODO: probably should do this as a dynamic bind so we don't depend upwards, but ... */
		BSYNClib_MuteControl_ScheduleTask_isr(hChn);
	}
	
	if (BSYNClib_Channel_P_Enabled_isr(hChn))
	{
		/* if anything changed in the config, reprocess based on current state */
		if (bChanged)
		{
			BSYNClib_Channel_P_ScheduleTask_isr(hChn);
		}
	}
	
	goto end;
	
error:

end:

	BDBG_LEAVE(BSYNClib_VideoSource_P_ProcessConfig_isr);
	return rc;
}

void BSYNClib_VideoSource_Snapshot_isr(BSYNClib_VideoSource * psSource)
{
	BDBG_ENTER(BSYNClib_VideoSource_Snapshot_isr);

	BDBG_ASSERT(psSource);

	psSource->sSnapshot = psSource->sData;
	
	BSYNClib_DelayElement_Snapshot_isr(&psSource->sElement);
	BSYNClib_VideoFormat_Snapshot_isr(&psSource->sFormat);

	BDBG_LEAVE(BSYNClib_VideoSource_Snapshot_isr);
}

void BSYNClib_VideoSource_P_GetDefaultStatus(
	BSYNClib_Source_Status * psStatus
)
{
	BDBG_ENTER(BSYNClib_VideoSource_P_GetDefaultStatus);

	BDBG_ASSERT(psStatus);

	psStatus->bMuted = false;
	psStatus->sDelayNotification.bEnabled = true;
	psStatus->sDelayNotification.sThreshold.uiValue = BSYNCLIB_P_VIDEO_SOURCE_DEFAULT_THRESHOLD_VALUE;
	psStatus->sDelayNotification.sThreshold.eUnits = BSYNCLIB_P_VIDEO_SOURCE_DEFAULT_UNITS;
	psStatus->sAppliedDelay.uiValue = 0;
	psStatus->sAppliedDelay.eUnits = BSYNCLIB_P_VIDEO_SOURCE_DEFAULT_UNITS;

	BDBG_LEAVE(BSYNClib_VideoSource_P_GetDefaultStatus);
}

