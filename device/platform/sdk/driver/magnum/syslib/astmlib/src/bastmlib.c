/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bastmlib.c $
* $brcm_Revision: Hydra_Software_Devel/8 $
* $brcm_Date: 11/2/10 4:54p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/astmlib/noarch/bastmlib.c $
*
* Hydra_Software_Devel/8   11/2/10 4:54p bandrews
* SW3548-1159: updated clock coupling recovery to use a second timeout
* value
*
* Hydra_Software_Devel/7   7/14/10 7:44p bandrews
* SW3548-1161: expose default configs as public
*
* Hydra_Software_Devel/6   7/17/09 6:56p bandrews
* PR49215: playback support
*
* Hydra_Software_Devel/5   11/20/08 6:16p bandrews
* PR49489: Add debug messages; ensure no events are recorded if stopped;
* clear event queue on start; only schedule settling timers if started
*
* Hydra_Software_Devel/4   11/19/08 7:22p bandrews
* PR49489: Add state reset on stop; Ensure state reset generates
* callbacks
*
* Hydra_Software_Devel/3   4/3/08 2:33p bandrews
* PR40090: synclib needs to have different contexts for syslib callbacks
* and synclib callbacks
*
* Hydra_Software_Devel/2   3/27/08 5:48p bandrews
* PR40090: Fixed display and storage of presenter names for debug
*
* Hydra_Software_Devel/1   3/24/08 3:08p bandrews
* PR40865: Fixed
*
* Hydra_Software_Devel/3   2/15/08 10:00p bandrews
* PR36148: Updated ASTM based on reviews
*
* Hydra_Software_Devel/2   1/25/08 9:21p bandrews
* PR36148: Updated based on simulation
*
* Hydra_Software_Devel/1   10/19/07 9:30p bandrews
* PR36148: Initial implementation
***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bsyslib.h"
#include "bastmlib.h"
#include "bastmlib_priv.h"
#include "bastmlib_clock_reference.h"
#include "bastmlib_clock_reference_priv.h"
#include "bastmlib_clock_coupling_task.h"
#include "bastmlib_presenter.h"
#include "bastmlib_presenter_priv.h"
#include "bastmlib_presentation_task.h"

BDBG_MODULE(astmlib);

void BASTMlib_GetDefaultSettings(
	BASTMlib_Settings * psSettings /* default instance settings [out] */
)
{
	BDBG_ENTER(BASTMlib_GetDefaultSettings);

	BDBG_ASSERT(psSettings);

	psSettings->cbTimer.pvParm1 = NULL;
	psSettings->cbTimer.iParm2 = 0;
	psSettings->cbTimer.pfCreate = NULL;
	psSettings->cbTimer.pfDestroy = NULL;
	psSettings->cbTimer.pfStart_isr = NULL;
	psSettings->cbTimer.pfCancel_isr = NULL;
	BSYSlib_Callback_Init(&psSettings->sClockCoupling.cbStateChange);
	BSYSlib_Callback_Init(&psSettings->sPresentation.cbStateChange);

	BDBG_LEAVE(BASTMlib_GetDefaultSettings);
}

BERR_Code BASTMlib_Create(
	BASTMlib_Handle * phAstm,
	const BASTMlib_Settings * psSettings
)
{
	BERR_Code rc = BERR_SUCCESS;
	BASTMlib_Handle hAstm = NULL;

	BDBG_ENTER(BASTMlib_Create);

	BDBG_ASSERT(phAstm);

	hAstm = (BASTMlib_Handle)BKNI_Malloc(sizeof(struct BASTMlib_Impl));
	BDBG_ASSERT(hAstm);
	if (!hAstm)
	{
		rc = BERR_OUT_OF_SYSTEM_MEMORY;
		goto error;
	}

	BKNI_Memset(hAstm, 0, sizeof(struct BASTMlib_Impl));

	if (psSettings)
	{
		hAstm->sSettings = *psSettings;
	}
	else
	{
		BASTMlib_GetDefaultSettings(&hAstm->sSettings);
	}

	/* allocate timers */
	if (hAstm->sSettings.cbTimer.pfCreate)
	{
		void * pvParm1 = hAstm->sSettings.cbTimer.pvParm1;
		int iParm2 = hAstm->sSettings.cbTimer.iParm2;

		rc = hAstm->sSettings.cbTimer.pfCreate(pvParm1, iParm2, &hAstm->sClockCoupling.hTimer);
		BDBG_ASSERT(hAstm->sClockCoupling.hTimer);
		if (rc) goto error;

		rc = hAstm->sSettings.cbTimer.pfCreate(pvParm1, iParm2, &hAstm->sPresentation.hTimer);
		BDBG_ASSERT(hAstm->sPresentation.hTimer);
		if (rc) goto error;

		rc = hAstm->sSettings.cbTimer.pfCreate(pvParm1, iParm2, &hAstm->sPresentation.hWatchdogTimer);
		BDBG_ASSERT(hAstm->sPresentation.hWatchdogTimer);
		if (rc) goto error;
	}

	/* create clock reference */
	BASTMlib_ClockReference_Create(&hAstm->sClockCoupling.hReference);
	hAstm->sClockCoupling.hReference->hAstm = hAstm;

	/* create presenter list */
	hAstm->sPresentation.hPresenters = BSYSlib_List_Create();
	BDBG_ASSERT(hAstm->sPresentation.hPresenters);
	hAstm->sPresentation.uiPresenterCount = 0;

	/* set default config */
	BASTMlib_P_GetDefaultConfig(&hAstm->sConfig);

	BDBG_MSG(("module initial configuration:"));
	BDBG_MSG(("  enabled: %s", hAstm->sConfig.bEnabled? "true" : "false"));
	BDBG_MSG(("  stc rate: %u Hz", hAstm->sConfig.eStcRate));
	BDBG_MSG(("  clock coupling:"));
	BDBG_MSG(("    initial acquisition time: %u ms", hAstm->sConfig.sClockCoupling.uiInitialAcquisitionTime));
	BDBG_MSG(("    processing frequency: %u ms", hAstm->sConfig.sClockCoupling.uiProcessingFrequency));
	BDBG_MSG(("    settling time: %u ms", hAstm->sConfig.sClockCoupling.uiSettlingTime));
	BDBG_MSG(("    preferred clock coupling: %u", hAstm->sConfig.sClockCoupling.ePreferredClockCoupling));
	BDBG_MSG(("  presentation:"));
	BDBG_MSG(("    preferred stc source: %u", hAstm->sConfig.sPresentation.ePreferredStcSource));
	BDBG_MSG(("    preferred presenter: %s", hAstm->sConfig.sPresentation.hPreferredPresenter ? hAstm->sConfig.sPresentation.hPreferredPresenter->pcName : "FCFS"));
	BDBG_MSG(("    initial acquisition time: %u ms", hAstm->sConfig.sPresentation.uiInitialAcquisitionTime));
	BDBG_MSG(("    processing frequency: %u ms", hAstm->sConfig.sPresentation.uiProcessingFrequency));
	BDBG_MSG(("    TSM-disabled watchdog timeout: %u ms", hAstm->sConfig.sPresentation.uiTsmDisabledWatchdogTimeout));
	BDBG_MSG(("    settling time: %u ms", hAstm->sConfig.sPresentation.uiSettlingTime));
	BDBG_MSG(("    preferred presentation rate control: %u", hAstm->sConfig.sPresentation.ePreferredPresentationRateControl));

	/* create clock reference event queue */
	BASTMlib_P_ClockCoupling_GetMaxAcquisitionTime(hAstm, &hAstm->sClockCoupling.hReference->uiMaximumAcquisitionTime);
	BASTMlib_ClockReference_ResizeEventQueue(hAstm->sClockCoupling.hReference);

	*phAstm = hAstm;

	goto end;

error:

	if (hAstm)
	{
		BASTMlib_Destroy(hAstm);
	}

end:
	BDBG_LEAVE(BASTMlib_Create);
	return rc;
}


void BASTMlib_Destroy(
	BASTMlib_Handle hAstm
)
{
	BDBG_ENTER(BASTMlib_Destroy);

	BDBG_ASSERT(hAstm);

	BASTMlib_Stop(hAstm);

	/* destroy clock reference */
	BASTMlib_ClockReference_Destroy(hAstm->sClockCoupling.hReference);
	hAstm->sClockCoupling.hReference = NULL;

	/* destroy the presenter list */
	BSYSlib_List_Destroy(hAstm->sPresentation.hPresenters);
	hAstm->sPresentation.hPresenters = NULL;

	/* deallocate timers */
	if (hAstm->sSettings.cbTimer.pfDestroy)
	{
		void * pvParm1 = hAstm->sSettings.cbTimer.pvParm1;
		int iParm2 = hAstm->sSettings.cbTimer.iParm2;
		BSYSlib_Timer_Handle hTimer;

		hTimer = hAstm->sPresentation.hTimer;
		if (hTimer)
		{
			hAstm->sSettings.cbTimer.pfDestroy(pvParm1, iParm2, hTimer);
		}
		hAstm->sPresentation.hTimer = NULL;

		hTimer = hAstm->sPresentation.hWatchdogTimer;
		if (hTimer)
		{
			hAstm->sSettings.cbTimer.pfDestroy(pvParm1, iParm2, hTimer);
		}
		hAstm->sPresentation.hWatchdogTimer = NULL;

		hTimer = hAstm->sClockCoupling.hTimer;
		if (hTimer)
		{
			hAstm->sSettings.cbTimer.pfDestroy(pvParm1, iParm2, hTimer);
		}
		hAstm->sClockCoupling.hTimer = NULL;
	}

	/* free me */
	BKNI_Free(hAstm);

	BDBG_LEAVE(BASTMlib_Destroy);
}

BERR_Code BASTMlib_Start(
	BASTMlib_Handle hAstm
)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYSlib_List_IteratorHandle hIterator;

	BDBG_ENTER(BASTMlib_Start);

	BDBG_ASSERT(hAstm);

    BKNI_EnterCriticalSection();
    /* flush event queue on start */
    BASTMlib_ClockReference_Flush_isr(hAstm->sClockCoupling.hReference);
    /* reset counters on start */
    BASTMlib_ClockReference_Reset_isr(hAstm->sClockCoupling.hReference);
    BKNI_LeaveCriticalSection();

	/* resize queues on start, since we may have tried to resize while
	already running. If same size as last time, nothing happens. */
	BASTMlib_ClockReference_ResizeEventQueue(hAstm->sClockCoupling.hReference);

	hIterator = BSYSlib_List_AcquireIterator(hAstm->sPresentation.hPresenters);
	while (BSYSlib_List_HasNext(hIterator))
	{
		BASTMlib_Presenter_Handle hPresenter = (BASTMlib_Presenter_Handle)BSYSlib_List_Next(hIterator);

		BKNI_EnterCriticalSection();
        /* flush event queue on start */
        BASTMlib_Presenter_Flush_isr(hPresenter);
        /* reset counters on start */
		BASTMlib_Presenter_Reset_isr(hPresenter);
		BKNI_LeaveCriticalSection();

        /* resize queues on start, since we may have tried to resize while
        already running. If same size as last time, nothing happens. */
		BASTMlib_Presenter_ResizeEventQueue(hPresenter);
	}
	BSYSlib_List_ReleaseIterator(hIterator);

	BASTMlib_P_ClockCoupling_StateMachine_SendSignal(hAstm,
		BASTMlib_ClockCoupling_StateMachineSignal_eReset);

	/* only schedule clock coupling task if we prefer the input clock */
	if (hAstm->sConfig.sClockCoupling.ePreferredClockCoupling == BASTMlib_ClockCoupling_eInputClock)
	{
		BDBG_MSG(("Scheduling initial clock coupling acquisition timer"));
		BKNI_EnterCriticalSection();
		hAstm->sClockCoupling.bAcquire = true;
		rc = BASTMlib_P_StartTimer_isr(hAstm,
			hAstm->sClockCoupling.hTimer,
			hAstm->sConfig.sClockCoupling.uiInitialAcquisitionTime,
			&BASTMlib_P_ClockCoupling_TimerExpired, hAstm, 0);
		BKNI_LeaveCriticalSection();
		if (rc) goto error;
	}
	else
	{
		/* else disable clock coupling event acquisition */
		hAstm->sClockCoupling.bAcquire = false;
	}

	BASTMlib_P_Presentation_RateControlStateMachine_SendSignal(hAstm,
		BASTMlib_Presentation_StateMachineSignal_eReset);

	BASTMlib_P_Presentation_StcSourceStateMachine_SendSignal(hAstm,
		BASTMlib_Presentation_StateMachineSignal_eReset);

	BDBG_MSG(("Scheduling initial presentation acquisition timer"));
	BKNI_EnterCriticalSection();
	hAstm->sPresentation.bAcquire = false; /* initially, we ignore pts errors */
	rc = BASTMlib_P_StartTimer_isr(hAstm,
		hAstm->sPresentation.hTimer,
		hAstm->sConfig.sPresentation.uiInitialAcquisitionTime,
		&BASTMlib_P_Presentation_TimerExpired, hAstm, 0);
	BKNI_LeaveCriticalSection();
	if (rc) goto error;

	hAstm->bStarted = true;

	goto end;

error:

end:
	BDBG_LEAVE(BASTMlib_Start);
	return rc;
}

BERR_Code BASTMlib_Stop(
	BASTMlib_Handle hAstm
)
{
	BERR_Code rc = BERR_SUCCESS;
	void * pvParm1;
	int iParm2;
	BSYSlib_Timer_Cancel_isr pfCancel_isr;

	BDBG_ENTER(BASTMlib_Stop);

	BDBG_ASSERT(hAstm);

	hAstm->bStarted = false;

	pfCancel_isr = hAstm->sSettings.cbTimer.pfCancel_isr;
	pvParm1 = hAstm->sSettings.cbTimer.pvParm1;
	iParm2 = hAstm->sSettings.cbTimer.iParm2;

	if (pfCancel_isr)
	{
		BKNI_EnterCriticalSection();
		hAstm->sClockCoupling.bAcquire = false;
		rc = pfCancel_isr(pvParm1, iParm2, hAstm->sClockCoupling.hTimer);
		BKNI_LeaveCriticalSection();
		if (rc) goto error;

		BKNI_EnterCriticalSection();
		hAstm->sPresentation.bAcquire = false;
		rc = pfCancel_isr(pvParm1, iParm2, hAstm->sPresentation.hTimer);
		BKNI_LeaveCriticalSection();
		if (rc) goto error;

		/* PR:50051 need to stop watchdog timer, too */
		BKNI_EnterCriticalSection();
		rc = pfCancel_isr(pvParm1, iParm2, hAstm->sPresentation.hWatchdogTimer);
		BKNI_LeaveCriticalSection();
		if (rc) goto error;
	}

	BASTMlib_P_ClockCoupling_StateMachine_SendSignal(hAstm,
		BASTMlib_ClockCoupling_StateMachineSignal_eReset);

	BASTMlib_P_Presentation_RateControlStateMachine_SendSignal(hAstm,
		BASTMlib_Presentation_StateMachineSignal_eReset);

	BASTMlib_P_Presentation_StcSourceStateMachine_SendSignal(hAstm,
		BASTMlib_Presentation_StateMachineSignal_eReset);

	goto end;

error:

end:
	BDBG_LEAVE(BASTMlib_Stop);
	return rc;
}

void BASTMlib_GetDefaultConfig(
	BASTMlib_Config * psConfig
)
{
	BASTMlib_P_GetDefaultConfig(psConfig);
}

void BASTMlib_GetConfig(
	const BASTMlib_Handle hAstm,
	BASTMlib_Config * psConfig
)
{
	BDBG_ENTER(BASTMlib_GetConfig);

	BDBG_ASSERT(psConfig);

	*psConfig = hAstm->sConfig;

	BDBG_LEAVE(BASTMlib_GetConfig);
}

void BASTMlib_SetConfig(
	BASTMlib_Handle hAstm,
	const BASTMlib_Config * psConfig
)
{
	BDBG_ENTER(BASTMlib_SetConfig);

	BDBG_ASSERT(hAstm);

	if (psConfig)
	{
		/*
			process config changes:
			if current state is higher than preferred state, reset to preference
			immediately. However, if current state is lower than preferred
			state, just save for later
		*/
		if (hAstm->sConfig.sClockCoupling.ePreferredClockCoupling == BASTMlib_ClockCoupling_eInputClock
			&& psConfig->sClockCoupling.ePreferredClockCoupling == BASTMlib_ClockCoupling_eInternalClock)
		{
			BASTMlib_P_ClockCoupling_StateMachine_SendSignal(hAstm,
				BASTMlib_ClockCoupling_StateMachineSignal_eReset);
		}

		if (hAstm->sConfig.sPresentation.ePreferredStcSource == BASTMlib_StcSource_eClockReference
			&& psConfig->sPresentation.ePreferredStcSource == BASTMlib_StcSource_ePresenter)
		{
			BASTMlib_P_Presentation_StcSourceStateMachine_SendSignal(hAstm,
				BASTMlib_Presentation_StateMachineSignal_eReset);
		}

		if (hAstm->sConfig.sPresentation.ePreferredPresentationRateControl == BASTMlib_PresentationRateControl_eTimeStamp
			&& psConfig->sPresentation.ePreferredPresentationRateControl == BASTMlib_PresentationRateControl_eOutputClock)
		{
			BASTMlib_P_Presentation_RateControlStateMachine_SendSignal(hAstm,
				BASTMlib_Presentation_StateMachineSignal_eReset);
		}

		/* copy config */
		hAstm->sConfig = *psConfig;

		BDBG_MSG(("module reconfigured:"));
		BDBG_MSG(("  enabled: %s", hAstm->sConfig.bEnabled? "true" : "false"));
		BDBG_MSG(("  stc rate: %u Hz", hAstm->sConfig.eStcRate));
		BDBG_MSG(("  clock coupling:"));
		BDBG_MSG(("    initial acquisition time: %u ms", hAstm->sConfig.sClockCoupling.uiInitialAcquisitionTime));
		BDBG_MSG(("    processing frequency: %u ms", hAstm->sConfig.sClockCoupling.uiProcessingFrequency));
		BDBG_MSG(("    ideal processing frequency: %u ms", hAstm->sConfig.sClockCoupling.uiIdealProcessingFrequency));
		BDBG_MSG(("    settling time: %u ms", hAstm->sConfig.sClockCoupling.uiSettlingTime));
		BDBG_MSG(("    preferred clock coupling: %u", hAstm->sConfig.sClockCoupling.ePreferredClockCoupling));
		BDBG_MSG(("  presentation:"));
		BDBG_MSG(("    preferred stc source: %u", hAstm->sConfig.sPresentation.ePreferredStcSource));
		BDBG_MSG(("    preferred presenter: %s", hAstm->sConfig.sPresentation.hPreferredPresenter ? hAstm->sConfig.sPresentation.hPreferredPresenter->pcName : "FCFS"));
		BDBG_MSG(("    initial acquisition time: %u ms", hAstm->sConfig.sPresentation.uiInitialAcquisitionTime));
		BDBG_MSG(("    processing frequency: %u ms", hAstm->sConfig.sPresentation.uiProcessingFrequency));
		BDBG_MSG(("    TSM-disabled watchdog timeout: %u ms", hAstm->sConfig.sPresentation.uiTsmDisabledWatchdogTimeout));
		BDBG_MSG(("    settling time: %u ms", hAstm->sConfig.sPresentation.uiSettlingTime));
		BDBG_MSG(("    preferred presentation rate control: %u", hAstm->sConfig.sPresentation.ePreferredPresentationRateControl));

		if (hAstm->sClockCoupling.hReference)
		{
			BASTMlib_ClockReference_Config sReferenceConfig;

			sReferenceConfig.uiMinimumTimeBetweenEvents = psConfig->sClockCoupling.uiMinimumTimeBetweenEvents;
			sReferenceConfig.eClockReferenceDomain = psConfig->sClockCoupling.eClockReferenceDomain;
			sReferenceConfig.uiDeviationThreshold = psConfig->sClockCoupling.uiDeviationThreshold;
			sReferenceConfig.uiDeviantCountThreshold = psConfig->sClockCoupling.uiDeviantCountThreshold;
			sReferenceConfig.uiIdealCountThreshold = psConfig->sClockCoupling.uiIdealCountThreshold;

			BASTMlib_P_ClockCoupling_GetMaxAcquisitionTime(hAstm, &hAstm->sClockCoupling.hReference->uiMaximumAcquisitionTime);
			BASTMlib_ClockReference_SetConfig(hAstm->sClockCoupling.hReference,
				&sReferenceConfig);
		}
	}

	BDBG_LEAVE(BASTMlib_SetConfig);
}

void BASTMlib_GetStatus(
	BASTMlib_Handle hAstm,
	BASTMlib_Status * psStatus /* [out] */
)
{
	BDBG_ENTER(BASTMlib_GetStatus);

	BDBG_ASSERT(psStatus);

	*psStatus = hAstm->sStatus;

	BDBG_LEAVE(BASTMlib_GetStatus);
}

BERR_Code BASTMlib_AddPresenter(
	BASTMlib_Handle hAstm,
	BASTMlib_Presenter_Handle hPresenter
)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYSlib_List_IteratorHandle hIterator;
	bool bFound = false;

	BDBG_ENTER(BASTMlib_AddPresenter);

	BDBG_ASSERT(hAstm);
	BDBG_ASSERT(hPresenter);

	/* look for presenter already added to list */
	hIterator = BSYSlib_List_AcquireIterator(hAstm->sPresentation.hPresenters);

	while (BSYSlib_List_HasNext(hIterator))
	{
		BASTMlib_Presenter_Handle hTemp;

		hTemp = (BASTMlib_Presenter_Handle)BSYSlib_List_Next(hIterator);

		if (hTemp == hPresenter)
		{
			bFound = true;
			break;
		}
	}

	BSYSlib_List_ReleaseIterator(hIterator);

	if (!bFound)
	{
		BSYSlib_List_AddElement(hAstm->sPresentation.hPresenters, hPresenter);
		hPresenter->hAstm = hAstm;
		hAstm->sPresentation.uiPresenterCount++;

		BASTMlib_P_Presentation_GetMaxAcquisitionTime(hAstm, &hPresenter->uiMaximumAcquisitionTime);

		/* create queue */
		BASTMlib_Presenter_ResizeEventQueue(hPresenter);
	}
	else
	{
		/* already added */
		BDBG_WRN(("Presenter %p already added to ASTM instance %p", hPresenter, hAstm));
		rc = BERR_INVALID_PARAMETER;
		goto error;
	}

	goto end;

error:

end:

	BDBG_LEAVE(BASTMlib_AddPresenter);
	return rc;
}

void BASTMlib_RemovePresenter(
	BASTMlib_Handle hAstm,
	BASTMlib_Presenter_Handle hPresenter
)
{
	BSYSlib_List_IteratorHandle hIterator;
	bool bFound = false;

	BDBG_ENTER(BASTMlib_RemovePresenter);

	BDBG_ASSERT(hAstm);
	BDBG_ASSERT(hPresenter);

	/* look for presenter in list */
	hIterator = BSYSlib_List_AcquireIterator(hAstm->sPresentation.hPresenters);

	while (BSYSlib_List_HasNext(hIterator))
	{
		BASTMlib_Presenter_Handle hTemp;

		hTemp = (BASTMlib_Presenter_Handle)BSYSlib_List_Next(hIterator);

		if (hTemp == hPresenter)
		{
			bFound = true;
			break;
		}
	}

	BSYSlib_List_ReleaseIterator(hIterator);

	if (bFound)
	{
		hAstm->sPresentation.uiPresenterCount--;
		hPresenter->hAstm = hAstm;
		BSYSlib_List_RemoveElement(hAstm->sPresentation.hPresenters, hPresenter);
	}
	else
	{
		/* already added */
		BDBG_MSG(("Presenter %p wasn't added to ASTM instance %p, so not removed", hPresenter, hAstm));
	}

	BDBG_LEAVE(BASTMlib_RemovePresenter);
}

void BASTMlib_ClockReferenceEventHandler_isr(
	BASTMlib_Handle hAstm,
	const BASTMlib_ClockReference_Event * psEvent

)
{
	BASTMlib_ClockReference_Handle hReference;

	BDBG_ENTER(BASTMlib_ClockReferenceEventHandler_isr);

	BDBG_ASSERT(hAstm);
	BDBG_ASSERT(psEvent);

	hReference = hAstm->sClockCoupling.hReference;

	BASTMlib_ClockReference_EventHandler_isr(hReference, psEvent);

	BDBG_LEAVE(BASTMlib_ClockReferenceEventHandler_isr);
}

