/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_channel.c $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 10/16/12 3:15p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/src/bsynclib_channel.c $
* 
* Hydra_Software_Devel/4   10/16/12 3:15p bandrews
* SW7425-4022: add delay received status to sink, which allows for delay
* values of zero to be marked as received
* 
* Hydra_Software_Devel/3   2/12/10 8:46p bandrews
* SW7405-3912: assert all inputs are not null
* 
* Hydra_Software_Devel/2   10/30/08 10:34p bandrews
* PR48566: Added support for preferred callback units
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/3   2/26/08 10:21p bandrews
* PR37951: Fixed units.  Implemented static rate mismatch detection.
* 
* Hydra_Software_Devel/2   1/3/08 5:17p bandrews
* PR37951: Updated based on initial feedback
* 
* Hydra_Software_Devel/1   12/12/07 2:53p bandrews
* PR37951: Initial check-in
***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bsyslib.h"
#include "bsynclib.h"
#include "bsynclib_priv.h"
#include "bsynclib_channel_priv.h"

BDBG_MODULE(synclib);

void BSYNClib_Channel_GetSettings
(
	const BSYNClib_Channel_Handle hChn,
	BSYNClib_Channel_Settings * psSettings /* [out] */
)
{
	BDBG_ENTER(BSYNClib_Channel_GetSettings);

	BDBG_ASSERT(hChn);
	BDBG_ASSERT(psSettings);

	*psSettings = hChn->sSettings;

	BDBG_LEAVE(BSYNClib_Channel_GetSettings);
}

BERR_Code BSYNClib_Channel_SetSettings
(
	BSYNClib_Channel_Handle hChn,
	const BSYNClib_Channel_Settings * psSettings
)
{
	BERR_Code rc = BERR_SUCCESS;

	BDBG_ENTER(BSYNClib_Channel_SetSettings);

	BDBG_ASSERT(hChn);
	BDBG_ASSERT(psSettings);

	hChn->sSettings = *psSettings;

	BDBG_LEAVE(BSYNClib_Channel_SetSettings);

	return rc;
}


void BSYNClib_Channel_GetStatus(
	BSYNClib_Channel_Handle hChn,
	BSYNClib_Channel_Status * psStatus /* [out] */
)
{
	BDBG_ENTER(BSYNClib_Channel_GetStatus);
	
	BDBG_ASSERT(hChn);

	if (BSYNClib_P_Enabled(hChn->hParent))
	{
		BDBG_ASSERT(psStatus);

		*psStatus = hChn->sStatus;
	}
	
	BDBG_LEAVE(BSYNClib_Channel_GetStatus);
}

void BSYNClib_Channel_GetVideoSourceStatus
(
	const BSYNClib_Channel_Handle hChn,
	unsigned int uiSource,
	BSYNClib_Source_Status * psStatus /* [out] */
)
{
	BDBG_ENTER(BSYNClib_Channel_GetVideoSourceStatus);

	BDBG_ASSERT(hChn);

	if (BSYNClib_P_Enabled(hChn->hParent))
	{
		BSYNClib_VideoSource * psSource;

		psSource = (BSYNClib_VideoSource *)BSYSlib_List_GetByIndex(hChn->sVideo.hSources, uiSource);

		if (psSource)
		{
			BDBG_ASSERT(psStatus);
			*psStatus = psSource->sStatus;
		}
	}

	BDBG_LEAVE(BSYNClib_Channel_GetVideoSourceStatus);
}

void BSYNClib_Channel_GetVideoSinkStatus_isr
(
	const BSYNClib_Channel_Handle hChn,
	unsigned int uiSink,
	BSYNClib_Sink_Status * psStatus /* [out] */
)
{
	BSYNClib_VideoSink * psSink;
	BDBG_ENTER(BSYNClib_Channel_GetVideoSinkStatus);

	BDBG_ASSERT(hChn);
	BDBG_ASSERT(psStatus);

	psSink = (BSYNClib_VideoSink *)BSYSlib_List_GetByIndex_isr(hChn->sVideo.hSinks, uiSink);

	if (psSink)
	{
	    BKNI_Memcpy_isr(psStatus, &psSink->sStatus, sizeof(BSYNClib_Sink_Status));
	}

	BDBG_LEAVE(BSYNClib_Channel_GetVideoSinkStatus);
}

void BSYNClib_Channel_GetAudioSourceStatus
(
	const BSYNClib_Channel_Handle hChn,
	unsigned int uiSource,
	BSYNClib_Source_Status * psStatus /* [out] */
)
{
	BDBG_ENTER(BSYNClib_Channel_GetAudioSourceStatus);

	BDBG_ASSERT(hChn);

	if (BSYNClib_P_Enabled(hChn->hParent))
	{
		BSYNClib_AudioSource * psSource;

		psSource = (BSYNClib_AudioSource *)BSYSlib_List_GetByIndex(hChn->sAudio.hSources, uiSource);

		if (psSource)
		{
			BDBG_ASSERT(psStatus);
			*psStatus = psSource->sStatus;
		}
	}

	BDBG_LEAVE(BSYNClib_Channel_GetAudioSourceStatus);
}

void BSYNClib_Channel_GetAudioSinkStatus
(
	const BSYNClib_Channel_Handle hChn,
	unsigned int uiSink,
	BSYNClib_Sink_Status * psStatus /* [out] */
)
{
	BDBG_ENTER(BSYNClib_Channel_GetAudioSinkStatus);

	BDBG_ASSERT(hChn);

	if (BSYNClib_P_Enabled(hChn->hParent))
	{
		BSYNClib_AudioSink * psSink;

		psSink = (BSYNClib_AudioSink *)BSYSlib_List_GetByIndex(hChn->sAudio.hSinks, uiSink);

		if (psSink)
		{
			BDBG_ASSERT(psStatus);
			*psStatus = psSink->sStatus;
		}
	}

	BDBG_LEAVE(BSYNClib_Channel_GetAudioSinkStatus);
}

