/***************************************************************************
*     Copyright (c) 2004-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib.c $
* $brcm_Revision: Hydra_Software_Devel/9 $
* $brcm_Date: 7/8/11 2:40p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib.c $
* 
* Hydra_Software_Devel/9   7/8/11 2:40p bandrews
* SW7405-3884: punch RMD settings through properly, hParent wasn't set
* during create
* 
* Hydra_Software_Devel/8   7/2/10 6:04p bandrews
* SW7405-4436: added channel index to algo debug messages
* 
* Hydra_Software_Devel/7   5/28/10 6:31p bandrews
* SW7405-4436: printing channel index in dbg messages
* 
* Hydra_Software_Devel/6   3/22/10 5:44p bandrews
* sw7408-83: add unconditional video unmute
* 
* Hydra_Software_Devel/5   3/5/10 8:04p bandrews
* SW7325-688: default full screen required to false
* 
* Hydra_Software_Devel/4   7/24/09 4:15p bandrews
* PR48566: support preferred units
* 
* Hydra_Software_Devel/3   10/30/08 10:34p bandrews
* PR48566: Added support for preferred callback units
* 
* Hydra_Software_Devel/2   4/3/08 2:33p bandrews
* PR40090: synclib needs to have different contexts for syslib callbacks
* and synclib callbacks
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/2   1/3/08 5:17p bandrews
* PR37951: Updated based on initial feedback
* 
* Hydra_Software_Devel/1   12/12/07 2:53p bandrews
* PR37951: Initial check-in
***************************************************************************/

#include "bstd.h"
#include "bsyslib_list.h"
#include "bsynclib.h"
#include "bsynclib_priv.h"
#include "bsynclib_channel_priv.h"

BDBG_MODULE(synclib);

/*
Summary:
Returns the default global SYNC lib module settings
Description:
*/
void BSYNClib_GetDefaultSettings(
	BSYNClib_Settings * psSettings /* [out] */
)
{
	BDBG_ENTER(BSYNClib_GetDefaultSettings);

	BDBG_ASSERT(psSettings);

	psSettings->bEnabled = true;
	psSettings->sVideo.bRequireFullScreen = false;
	psSettings->sVideo.uiTsmLockTimeout = BSYNCLIB_VIDEO_TSM_LOCK_TIMER_DEFAULT_TIMEOUT;
	psSettings->sVideo.uiUnmuteTimeout = BSYNCLIB_VIDEO_UNMUTE_DEFAULT_TIMEOUT;
	psSettings->sVideo.uiUnconditionalUnmuteTimeout = BSYNCLIB_VIDEO_UNCONDITIONAL_UNMUTE_DEFAULT_TIMEOUT;
	psSettings->sVideo.sRateMismatchDetection.uiTimeout = BSYNCLIB_VIDEO_RATE_MISMATCH_DETECTION_TIMER_DEFAULT_TIMEOUT;
	psSettings->sVideo.sRateMismatchDetection.uiAcceptableMtbcLower = BSYNCLIB_VIDEO_RATE_MISMATCH_DETECTION_DEFAULT_ACCEPTABLE_MTBC_LOWER;
	psSettings->sVideo.sRateMismatchDetection.uiAcceptableMtbcUpper = BSYNCLIB_VIDEO_RATE_MISMATCH_DETECTION_DEFAULT_ACCEPTABLE_MTBC_UPPER;
	psSettings->sVideo.sRateMismatchDetection.uiAcceptableTtlc = BSYNCLIB_VIDEO_RATE_MISMATCH_DETECTION_DEFAULT_ACCEPTABLE_TTLC;
	psSettings->sVideo.sRateRematchDetection.uiTimeout = BSYNCLIB_VIDEO_RATE_REMATCH_DETECTION_TIMER_DEFAULT_TIMEOUT;
	psSettings->sVideo.sRateRematchDetection.uiAcceptableTtlc = BSYNCLIB_VIDEO_RATE_REMATCH_DETECTION_DEFAULT_ACCEPTABLE_TTLC;
	psSettings->sAudio.uiUnmuteTimeout = BSYNCLIB_AUDIO_UNMUTE_DEFAULT_TIMEOUT;
	psSettings->sAudio.uiReceiverDelayCompensation = BSYNCLIB_AUDIO_RECEIVER_DELAY_COMPENSATION_DEFAULT;
	psSettings->sAudio.uiUnconditionalUnmuteTimeout = BSYNCLIB_AUDIO_UNCONDITIONAL_UNMUTE_DEFAULT_TIMEOUT;

	BDBG_LEAVE(BSYNClib_GetDefaultSettings);
}

/*
Summary:
Opens the global SYNC lib module
Description:
*/
BERR_Code BSYNClib_Open(
	const BSYNClib_Settings * psSettings,
	BSYNClib_Handle * phSync /* [out] */
)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_Handle hSync = NULL;

	BDBG_ENTER(BSYNClib_Open);

	BDBG_ASSERT(phSync);

	hSync = (BSYNClib_Handle)BKNI_Malloc(sizeof(struct BSYNClib_Impl));
	if (!hSync)
	{
		rc = BERR_OUT_OF_SYSTEM_MEMORY;
		*phSync = NULL;
		goto end;
	}

	BKNI_Memset(hSync, 0, sizeof(struct BSYNClib_Impl));

	if (psSettings)
	{
		hSync->sSettings = *psSettings;
	}
	else
	{
		BSYNClib_GetDefaultSettings(&hSync->sSettings);
	}

	if (BSYNClib_P_Enabled(hSync))
	{
		hSync->hChannels = BSYSlib_List_Create();

	    BDBG_MSG(("Settings Summary:"));
	    BDBG_MSG(("  sync correction: %s", hSync->sSettings.bEnabled ? "enabled" : "disabled"));
	    BDBG_MSG(("  video"));
	    BDBG_MSG(("    full screen: %s", hSync->sSettings.sVideo.bRequireFullScreen ? "required" : "not required"));
	    BDBG_MSG(("    TSM lock timeout: %d milliseconds", hSync->sSettings.sVideo.uiTsmLockTimeout));
	    BDBG_MSG(("    unmute timeout: %d milliseconds", hSync->sSettings.sVideo.uiUnmuteTimeout));
	    BDBG_MSG(("    unconditional unmute timeout: %d milliseconds", hSync->sSettings.sVideo.uiUnconditionalUnmuteTimeout));
	    BDBG_MSG(("    rate mismatch detection"));
	    BDBG_MSG(("      timeout: %d milliseconds", hSync->sSettings.sVideo.sRateMismatchDetection.uiTimeout));
	    BDBG_MSG(("      acceptable mtbc lower bound: %d milliseconds", hSync->sSettings.sVideo.sRateMismatchDetection.uiAcceptableMtbcLower));
	    BDBG_MSG(("      acceptable mtbc upper bound: %d milliseconds", hSync->sSettings.sVideo.sRateMismatchDetection.uiAcceptableMtbcUpper));
	    BDBG_MSG(("      acceptable ttlc: %d milliseconds", hSync->sSettings.sVideo.sRateMismatchDetection.uiAcceptableTtlc));
	    BDBG_MSG(("    rate rematch detection"));
	    BDBG_MSG(("      timeout: %d milliseconds", hSync->sSettings.sVideo.sRateRematchDetection.uiTimeout));
	    BDBG_MSG(("      acceptable ttlc: %d milliseconds", hSync->sSettings.sVideo.sRateRematchDetection.uiAcceptableTtlc));
	    BDBG_MSG(("  audio"));
	    BDBG_MSG(("    unmute timeout: %d milliseconds", hSync->sSettings.sAudio.uiUnmuteTimeout));
	    BDBG_MSG(("    unconditional unmute timeout: %d milliseconds", hSync->sSettings.sAudio.uiUnconditionalUnmuteTimeout));
	    BDBG_MSG(("    receiver delay compensation: %d milliseconds", hSync->sSettings.sAudio.uiReceiverDelayCompensation));
	}
	else
	{
	    BDBG_MSG(("Settings Summary:"));
	    BDBG_MSG(("  sync correction: %s", BSYNClib_P_Enabled(hSync) ? "enabled" : "disabled"));
	}

	*phSync = hSync;

end:

	BDBG_LEAVE(BSYNClib_Open);
	return rc;
}


/*
Summary:
Closes the global SYNC lib module
Description:
*/
void BSYNClib_Close(
	BSYNClib_Handle hSync
)
{
	BSYNClib_Channel_Handle hChn;
	BSYSlib_List_IteratorHandle hIterator;

	BDBG_ENTER(BSYNClib_Close);

	BDBG_ASSERT(hSync);

	if (BSYNClib_P_Enabled(hSync))
	{
		hIterator = BSYSlib_List_AcquireIterator(hSync->hChannels);

		while (BSYSlib_List_HasNext(hIterator))
		{
			hChn = (BSYNClib_Channel_Handle)BSYSlib_List_Next(hIterator);
			BSYNClib_DestroyChannel(hSync, hChn);
		}

		BSYSlib_List_ReleaseIterator(hIterator);

		BSYSlib_List_Destroy(hSync->hChannels);
	}

	/* free me */
	BKNI_Free(hSync);

	BDBG_LEAVE(BSYNClib_Close);
}

#define BSYNCLIB_P_DEFAULT_SOURCE_PREFERRED_UNITS BSYNClib_Units_e45KhzTicks
#define BSYNCLIB_P_DEFAULT_VIDEO_SINK_PREFERRED_UNITS BSYNClib_Units_e60HzVsyncs
#define BSYNCLIB_P_DEFAULT_AUDIO_SINK_PREFERRED_UNITS BSYNClib_Units_eMilliseconds

/*
Summary:
Returns the default settings for the specified channel index
Description:
*/
void BSYNClib_GetChannelDefaultSettings(
	BSYNClib_Channel_Settings * psSettings /* default channel settings [out] */
)
{
	BDBG_ENTER(BSYNClib_GetChannelDefaultSettings);

	BDBG_ASSERT(psSettings);

	BKNI_Memset(psSettings, 0, sizeof(BSYNClib_Channel_Settings));

	psSettings->sVideo.sSource.cbDelay.preferredDelayUnits = BSYNCLIB_P_DEFAULT_SOURCE_PREFERRED_UNITS;
	psSettings->sVideo.sSource.cbDelay.preferredNotificationThresholdUnits = BSYNCLIB_P_DEFAULT_SOURCE_PREFERRED_UNITS;
	psSettings->sVideo.sSink.cbDelay.preferredDelayUnits = BSYNCLIB_P_DEFAULT_VIDEO_SINK_PREFERRED_UNITS;
	psSettings->sVideo.sSink.cbDelay.preferredNotificationThresholdUnits = BSYNCLIB_P_DEFAULT_VIDEO_SINK_PREFERRED_UNITS;
	psSettings->sAudio.sSource.cbDelay.preferredDelayUnits = BSYNCLIB_P_DEFAULT_SOURCE_PREFERRED_UNITS;
	psSettings->sAudio.sSource.cbDelay.preferredNotificationThresholdUnits = BSYNCLIB_P_DEFAULT_SOURCE_PREFERRED_UNITS;
	psSettings->sAudio.sSink.cbDelay.preferredDelayUnits = BSYNCLIB_P_DEFAULT_AUDIO_SINK_PREFERRED_UNITS;
	psSettings->sAudio.sSink.cbDelay.preferredNotificationThresholdUnits = BSYNCLIB_P_DEFAULT_AUDIO_SINK_PREFERRED_UNITS;

	BDBG_LEAVE(BSYNClib_GetChannelDefaultSettings);
}

/*
Summary:
Creates a SYNC lib channel
Description:
*/
BERR_Code BSYNClib_CreateChannel(
	BSYNClib_Handle hSync,
	const BSYNClib_Channel_Settings * psSettings,
	BSYNClib_Channel_Handle * phChn /* [out] */
)
{
	BERR_Code rc = BERR_SUCCESS;
	BSYNClib_Channel_Handle hChn = NULL;
	BSYNClib_Channel_Settings sSettings;

	BDBG_ENTER(BSYNClib_CreateChannel);

	BDBG_ASSERT(hSync);
	BDBG_ASSERT(phChn);

	if (BSYNClib_P_Enabled(hSync))
	{
		if (!psSettings)
		{
			BSYNClib_GetChannelDefaultSettings(&sSettings);
			psSettings = &sSettings;
		}

		rc = BSYNClib_Channel_P_Create(&hSync->sSettings, psSettings, &hChn);
		if (rc) goto end;

		hChn->hParent = hSync;

		BSYSlib_List_AddElement(hSync->hChannels, hChn);
		hChn->iIndex = BSYSlib_List_IndexOf(hSync->hChannels, hChn);
		BDBG_MSG(("chn %p is index %d", hChn, hChn->iIndex));
		hChn->sVideo.hChn = hChn;
		hChn->sAudio.hChn = hChn;
	}

	*phChn = hChn;

end:

	BDBG_LEAVE(BSYNClib_CreateChannel);
	return rc;
}

/*
Summary:
Closes a SYNC lib channel
Description:
*/
void BSYNClib_DestroyChannel(
	BSYNClib_Handle hSync,
	BSYNClib_Channel_Handle hChn
)
{
	BDBG_ENTER(BSYNClib_DestroyChannel);

	if (BSYNClib_P_Enabled(hSync))
	{
		BDBG_ASSERT(hSync);
		BDBG_ASSERT(hChn);

		BSYSlib_List_RemoveElement(hSync->hChannels, hChn);

		BSYNClib_Channel_P_Destroy(hChn);
	}

	BDBG_LEAVE(BSYNClib_DestroyChannel);
}

