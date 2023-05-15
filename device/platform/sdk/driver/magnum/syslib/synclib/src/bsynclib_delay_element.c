/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_delay_element.c $
* $brcm_Revision: Hydra_Software_Devel/6 $
* $brcm_Date: 3/19/10 2:44p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_delay_element.c $
* 
* Hydra_Software_Devel/6   3/19/10 2:44p bandrews
* SW7405-3774: merge mute control
* 
* Hydra_Software_Devel/SW7405-3774/2   3/18/10 9:34p bandrews
* SW7405-4042: rework delay estimation so that imprecise lipsync works
* correctly
* 
* Hydra_Software_Devel/SW7405-3774/1   2/17/10 6:17p bandrews
* SW7405-3774: attempt to fix early audio unmute then mute
* 
* Hydra_Software_Devel/5   12/10/09 9:18p bandrews
* SW7401-3634: adding PWC (now JTI) support to synclib
* 
* Hydra_Software_Devel/4   7/24/09 4:14p bandrews
* PR52812: Added any-time source or display rate change support
* 
* Hydra_Software_Devel/3   2/25/09 7:53p bandrews
* PR52514: needed another const
* 
* Hydra_Software_Devel/2   10/6/08 8:07p bandrews
* PR44510: Implement delay capacity for video sinks
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/9   2/27/08 8:33p bandrews
* PR37951: Fixed units conversion
* 
* Hydra_Software_Devel/8   2/26/08 3:19p bandrews
* PR37951: Fixed expiry of timers
* 
* Hydra_Software_Devel/7   2/25/08 9:33p bandrews
* PR37951: Fixed various bugs
* 
* Hydra_Software_Devel/6   2/22/08 9:20p bandrews
* PR37951: Added RMD support
* 
* Hydra_Software_Devel/5   2/22/08 8:28p bandrews
* PR37951: Fixed bug in units conversion
* 
* Hydra_Software_Devel/4   2/22/08 3:06p bandrews
* PR37951: Debugging callbacks
* 
* Hydra_Software_Devel/3   2/20/08 10:03p bandrews
* PR37951: Updated based on feedback from usage
* 
* Hydra_Software_Devel/2   1/3/08 5:17p bandrews
* PR37951: Updated based on initial feedback
* 
* Hydra_Software_Devel/1   12/12/07 2:53p bandrews
* PR37951: Initial check-in
***************************************************************************/

#include "bstd.h"
#include "bsynclib_delay_element.h"

BDBG_MODULE(synclib);

#ifdef BDBG_DEBUG_BUILD
const char * const BSYNClib_DelayElement_LifecycleEventNames[] = 
{
	"none",
	"started",
	"stopped"
};
const char * const BSYNClib_DelayElement_SynchronizationEventNames[] = 
{
	"none",
	"synchronized",
	"ignored"
};
#endif

void BSYNClib_DelayElement_Init(BSYNClib_DelayElement * psElement)
{
	BDBG_ENTER(BSYNClib_DelayElement_Init);

	BDBG_ASSERT(psElement);

	BKNI_Memset(psElement, 0, sizeof(BSYNClib_DelayElement));

	BSYNClib_Delay_Init(&psElement->sDelay);
	BSYNClib_DelayNotification_Init(&psElement->sNotification);

	BDBG_LEAVE(BSYNClib_DelayElement_Init);
}

void BSYNClib_DelayElement_Reset_isr(BSYNClib_DelayElement * psElement)
{
	BDBG_ENTER(BSYNClib_DelayElement_Reset_isr);

	BDBG_ASSERT(psElement);

	psElement->sData.bStarted = false;

	BSYNClib_Delay_Reset_isr(&psElement->sDelay);
	BSYNClib_DelayNotification_Reset_isr(&psElement->sNotification);

	BDBG_LEAVE(BSYNClib_DelayElement_Reset_isr);
}

void BSYNClib_DelayElement_Snapshot_isr(BSYNClib_DelayElement * psElement)
{
	BDBG_ENTER(BSYNClib_DelayElement_Snapshot_isr);
	
	BDBG_ASSERT(psElement);

	psElement->sSnapshot = psElement->sData;

	BSYNClib_Delay_Snapshot_isr(&psElement->sDelay);
	BSYNClib_DelayNotification_Snapshot_isr(&psElement->sNotification);
	
	BDBG_LEAVE(BSYNClib_DelayElement_Snapshot_isr);
}

void BSYNClib_Delay_Init(BSYNClib_Delay * psDelay)
{
	BDBG_ENTER(BSYNClib_Delay_Init);

	BDBG_ASSERT(psDelay);

	BKNI_Memset(psDelay, 0, sizeof(BSYNClib_Delay));

	BDBG_LEAVE(BSYNClib_Delay_Init);
}

void BSYNClib_Delay_Reset_isr(BSYNClib_Delay * psDelay)
{
	BDBG_ENTER(BSYNClib_Delay_Reset_isr);

	BDBG_ASSERT(psDelay);

	psDelay->sData.bValid = false;
	psDelay->sData.uiCustom = 0;
	psDelay->sData.uiMeasured = 0;

	psDelay->sSnapshot.bValid = false;
	psDelay->sSnapshot.uiCustom = 0;
	psDelay->sSnapshot.uiMeasured = 0;

	psDelay->sResults.bAccepted = false;
	psDelay->sResults.bEstimated = false;
	psDelay->sResults.uiApplied = 0;
	psDelay->sResults.uiDesired = 0;

	BDBG_LEAVE(BSYNClib_Delay_Reset_isr);
}

void BSYNClib_Delay_Snapshot_isr(BSYNClib_Delay * psDelay)
{
	BDBG_ENTER(BSYNClib_Delay_Snapshot_isr);

	BDBG_ASSERT(psDelay);

	psDelay->sSnapshot = psDelay->sData;
	BDBG_LEAVE(BSYNClib_Delay_Snapshot_isr);
}

void BSYNClib_DelayNotification_Init(BSYNClib_DelayNotification * psNotification)
{
	BDBG_ENTER(BSYNClib_DelayNotification_Init);

	BDBG_ASSERT(psNotification);

	BKNI_Memset(psNotification, 0, sizeof(BSYNClib_DelayNotification));
	psNotification->sData.bEnabled = true;
	psNotification->sSnapshot.bEnabled = true;
	psNotification->sResults.bEnabled = true;

	BDBG_LEAVE(BSYNClib_DelayNotification_Init);
}

void BSYNClib_DelayNotification_Reset_isr(BSYNClib_DelayNotification * psNotification)
{
	BDBG_ENTER(BSYNClib_DelayNotification_Reset_isr);
	
	BDBG_ASSERT(psNotification);

	psNotification->sData.bEnabled = true; /* TODO: should be based on callback installed */
	psNotification->sData.bReceived = false;

	psNotification->sSnapshot.bEnabled = true;
	psNotification->sSnapshot.bReceived = false;

	psNotification->sResults.bEnabled = true;
	psNotification->sResults.bGenerateCallback = false;
	
	BDBG_LEAVE(BSYNClib_DelayNotification_Reset_isr);
}

void BSYNClib_DelayNotification_Snapshot_isr(BSYNClib_DelayNotification * psNotification)
{
	BDBG_ENTER(BSYNClib_DelayNotification_Snapshot_isr);
	
	BDBG_ASSERT(psNotification);

	psNotification->sSnapshot = psNotification->sData;
	
	BDBG_LEAVE(BSYNClib_DelayNotification_Snapshot_isr);
}

void BSYNClib_DelayElement_CheckLifecycle_isr(
	bool bStarted,
	const BSYNClib_DelayElement * psCurrent,
	BSYNClib_DelayElement_DiffResults * psResults
)
{
	BDBG_ENTER(BSYNClib_DelayElement_CheckLifecycle_isr);

	BDBG_ASSERT(psCurrent);
	BDBG_ASSERT(psResults);

	if (bStarted && !psCurrent->sData.bStarted)
	{
		psResults->bChanged = true;
		psResults->eLifecycleEvent = BSYNClib_DelayElement_LifecycleEvent_eStarted;
	}
	else if (!bStarted && psCurrent->sData.bStarted)
	{
		psResults->bChanged = true;
		psResults->eLifecycleEvent = BSYNClib_DelayElement_LifecycleEvent_eStopped;
	}
	else
	{
		psResults->eLifecycleEvent = BSYNClib_DelayElement_LifecycleEvent_eNone;
	}

	BDBG_LEAVE(BSYNClib_DelayElement_CheckLifecycle_isr);
}
	

void BSYNClib_DelayElement_Diff_isr(
	const BSYNClib_DelayElement * psDesired,
	const BSYNClib_DelayElement * psCurrent,
	BSYNClib_DelayElement_DiffResults * psResults
)
{
	BDBG_ENTER(BSYNClib_DelayElement_Diff_isr);

	BDBG_ASSERT(psDesired);
	BDBG_ASSERT(psCurrent);
	BDBG_ASSERT(psResults);

	/* check if synchronization for this element was just enabled */
	if (psDesired->sData.bSynchronize && !psCurrent->sData.bSynchronize)
	{
		psResults->bChanged = true;
		psResults->bGenerateDelayCallback = true;
		psResults->bGenerateNotificationCallback = true;
		psResults->eSynchronizationEvent = BSYNClib_DelayElement_SynchronizationEvent_eSynchronized;
	}
	else if (!psDesired->sData.bSynchronize && psCurrent->sData.bSynchronize)
	{
		/* or disabled */
		psResults->bChanged = true;
		psResults->bGenerateDelayCallback = false;
		psResults->bGenerateNotificationCallback = false;
		psResults->eSynchronizationEvent = BSYNClib_DelayElement_SynchronizationEvent_eIgnored;
	}
	else
	{
		psResults->eSynchronizationEvent = BSYNClib_DelayElement_SynchronizationEvent_eNone;
	}

	/* copy user custom delay */
	if (psCurrent->sDelay.sData.uiCustom != psDesired->sDelay.sData.uiCustom)
	{
		psResults->bChanged = true;
	}

	/* if delay notification is being enabled (or we are starting with 
	delay notification enabled), generate callback for threshold value */
	if ((!psCurrent->sNotification.sData.bEnabled && psDesired->sNotification.sData.bEnabled)
		|| (psDesired->sNotification.sData.bEnabled && psResults->eLifecycleEvent == BSYNClib_DelayElement_LifecycleEvent_eStarted))
	{
		psResults->bGenerateNotificationCallback = true;
		psResults->bChanged = true;
	}

	/* if delay notification is enabled, check that we received one, otherwise, copy delay level and validate */
	if (psCurrent->sNotification.sData.bEnabled)
	{
		/* received a delay notification? */
		if (psDesired->sNotification.sData.bReceived)
		{
			psResults->bDelayReceived = true;
			psResults->bChanged = true;
		}
	}
	else
	{
		if (psCurrent->sDelay.sData.uiMeasured != psDesired->sDelay.sData.uiMeasured)
		{
			psResults->bChanged = true;
		}
	}

	if (psCurrent->sDelay.sData.uiCapacity != psDesired->sDelay.sData.uiCapacity)
	{
		psResults->bChanged = true;
	}

	BDBG_LEAVE(BSYNClib_DelayElement_Diff_isr);
}

void BSYNClib_DelayElement_Patch_isr(
	const BSYNClib_DelayElement * psDesired,
	BSYNClib_DelayElement * psCurrent,
	BSYNClib_DelayElement_DiffResults * psResults
)
{
	BDBG_ENTER(BSYNClib_DelayElement_Patch_isr);

	BDBG_ASSERT(psCurrent);
	BDBG_ASSERT(psDesired);
	BDBG_ASSERT(psResults);

	if (psResults->bChanged)
	{
		psCurrent->sData = psDesired->sData;
		psCurrent->sDelay.sData = psDesired->sDelay.sData;
		psCurrent->sNotification.sData = psDesired->sNotification.sData;

		if (psResults->bDelayReceived)
		{
			/* validate */
			psCurrent->sDelay.sData.bValid = true;
			/* accept only after timer expires */
			psCurrent->sDelay.sResults.bAccepted = false;
		}

		/* Accept and validate the delay if notification was disabled */
		if (!psCurrent->sNotification.sData.bEnabled)
		{
			psCurrent->sDelay.sData.bValid = true;
			/* TODO: do elsewhere? */
#if 0
			psCurrent->sDelay.sResults.bAccepted = true;
#endif
			/* TODO: assume avg delay? */
		}

		if (psResults->bGenerateDelayCallback)
		{
			psCurrent->sDelay.sResults.bGenerateCallback = true;
		}

		if (psResults->bGenerateNotificationCallback)
		{
			psCurrent->sNotification.sResults.bGenerateCallback = true;
		}
	}

	BDBG_LEAVE(BSYNClib_DelayElement_Patch_isr);
}

