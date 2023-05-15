/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_stubs.c $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 3/24/08 3:09p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_stubs.c $
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/5   1/3/08 5:30p bandrews
* PR37951: Updated with API changes
* 
* Hydra_Software_Devel/4   12/17/07 6:58p bandrews
* PR37951: Stubs compile errors
* 
* Hydra_Software_Devel/3   12/17/07 3:44p bandrews
* PR37951: Moved test stubs to test dir
* 
* Hydra_Software_Devel/2   12/7/07 3:09p bandrews
* PR37951: Fixed timer references
* 
* Hydra_Software_Devel/1   12/5/07 10:59p bandrews
* PR37951: Check in stubs for external work
***************************************************************************/

#include "bstd.h"
#include "bsynclib.h"

BDBG_MODULE(synclib_stubs);

void BSYNClib_GetDefaultSettings
(
	BSYNClib_Settings * psSettings /* [out] */
)
{
	BSTD_UNUSED(psSettings);
}

BERR_Code BSYNClib_Open
(
	const BSYNClib_Settings * psSettings,
	BSYNClib_Handle * phSync /* [out] */
)
{
	BSTD_UNUSED(psSettings);
	BSTD_UNUSED(phSync);
	return BERR_SUCCESS;
}

void BSYNClib_Close
(
	BSYNClib_Handle hSync
)
{
	BSTD_UNUSED(hSync);
}

void BSYNClib_GetChannelDefaultSettings
(
	BSYNClib_Channel_Settings * psSettings /* [out] */
)
{
	BSTD_UNUSED(psSettings);
}

BERR_Code BSYNClib_CreateChannel
(
	BSYNClib_Handle hSync, 
	const BSYNClib_Channel_Settings * psSettings, 
	BSYNClib_Channel_Handle * phChn /* [out] */
)
{
	BSTD_UNUSED(hSync);
	BSTD_UNUSED(psSettings);
	BSTD_UNUSED(phChn);
	return BERR_SUCCESS;
}

void BSYNClib_DestroyChannel
(
	BSYNClib_Handle hSync, 
	BSYNClib_Channel_Handle hChn
)
{
	BSTD_UNUSED(hSync);
	BSTD_UNUSED(hChn);
}

void BSYNClib_Channel_GetSettings
(
	const BSYNClib_Channel_Handle hChn,
	BSYNClib_Channel_Settings * psSettings /* [out] */
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(psSettings);
}

BERR_Code BSYNClib_Channel_SetSettings
(
	BSYNClib_Channel_Handle hChn,
	const BSYNClib_Channel_Settings * psSettings
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(psSettings);
	return BERR_SUCCESS;
}

void BSYNClib_Channel_GetConfig
(
	const BSYNClib_Channel_Handle hChn,
	BSYNClib_Channel_Config * psConfig /* [out] */
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(psConfig);
}

BERR_Code BSYNClib_Channel_SetConfig
(
	BSYNClib_Channel_Handle hChn,
	const BSYNClib_Channel_Config * psConfig
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(psConfig);
	return BERR_SUCCESS;
}

void BSYNClib_Channel_GetStatus
(
	const BSYNClib_Channel_Handle hChn, 
	BSYNClib_Channel_Status * psStatus /* [out] */
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(psStatus);
}

void BSYNClib_Channel_GetVideoSourceConfig_isr
(
	const BSYNClib_Channel_Handle hChn,
	unsigned int uiSource,
	BSYNClib_VideoSource_Config * psConfig /* [out] */
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(uiSource);
	BSTD_UNUSED(psConfig);
}

BERR_Code BSYNClib_Channel_SetVideoSourceConfig_isr
(
	BSYNClib_Channel_Handle hChn,
	unsigned int uiSource,
	const BSYNClib_VideoSource_Config * psConfig
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(uiSource);
	BSTD_UNUSED(psConfig);
	return BERR_SUCCESS;
}

void BSYNClib_Channel_GetVideoSinkConfig_isr
(
	const BSYNClib_Channel_Handle hChn,
	unsigned int uiSink,
	BSYNClib_VideoSink_Config * psConfig /* [out] */
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(uiSink);
	BSTD_UNUSED(psConfig);
}

BERR_Code BSYNClib_Channel_SetVideoSinkConfig_isr
(
	BSYNClib_Channel_Handle hChn,
	unsigned int uiSink,
	const BSYNClib_VideoSink_Config * psConfig
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(uiSink);
	BSTD_UNUSED(psConfig);
	return BERR_SUCCESS;
}

void BSYNClib_Channel_GetAudioSourceConfig_isr
(
	const BSYNClib_Channel_Handle hChn,
	unsigned int uiSource,
	BSYNClib_AudioSource_Config * psConfig /* [out] */
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(uiSource);
	BSTD_UNUSED(psConfig);
}

BERR_Code BSYNClib_Channel_SetAudioSourceConfig_isr
(
	BSYNClib_Channel_Handle hChn,
	unsigned int uiSource,
	const BSYNClib_AudioSource_Config * psConfig
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(uiSource);
	BSTD_UNUSED(psConfig);
	return BERR_SUCCESS;
}

void BSYNClib_Channel_GetAudioSinkConfig_isr
(
	const BSYNClib_Channel_Handle hChn,
	unsigned int uiSink,
	BSYNClib_AudioSink_Config * psConfig /* [out] */
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(uiSink);
	BSTD_UNUSED(psConfig);
}

BERR_Code BSYNClib_Channel_SetAudioSinkConfig_isr
(
	BSYNClib_Channel_Handle hChn,
	unsigned int uiSink,
	const BSYNClib_AudioSink_Config * psConfig
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(uiSink);
	BSTD_UNUSED(psConfig);
	return BERR_SUCCESS;
}

void BSYNClib_Channel_GetVideoSourceStatus
(
	const BSYNClib_Channel_Handle hChn,
	unsigned int uiSource,
	BSYNClib_Source_Status * psStatus /* [out] */
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(uiSource);
	BSTD_UNUSED(psStatus);
}

void BSYNClib_Channel_GetVideoSinkStatus
(
	const BSYNClib_Channel_Handle hChn,
	unsigned int uiSink,
	BSYNClib_Sink_Status * psStatus /* [out] */
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(uiSink);
	BSTD_UNUSED(psStatus);
}

void BSYNClib_Channel_GetAudioSourceStatus
(
	const BSYNClib_Channel_Handle hChn,
	unsigned int uiSource,
	BSYNClib_Source_Status * psStatus /* [out] */
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(uiSource);
	BSTD_UNUSED(psStatus);
}

void BSYNClib_Channel_GetAudioSinkStatus
(
	const BSYNClib_Channel_Handle hChn,
	unsigned int uiSink,
	BSYNClib_Sink_Status * psStatus /* [out] */
)
{
	BSTD_UNUSED(hChn);
	BSTD_UNUSED(uiSink);
	BSTD_UNUSED(psStatus);
}

