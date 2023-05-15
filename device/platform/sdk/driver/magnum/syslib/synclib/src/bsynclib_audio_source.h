/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_audio_source.h $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 3/22/10 5:39p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_audio_source.h $
* 
* Hydra_Software_Devel/3   3/22/10 5:39p bandrews
* SW7408-83: remove undefined functions; add unconditional video unmute
* 
* Hydra_Software_Devel/2   1/26/10 9:00p bandrews
* SW7405-3774: added mute control support
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
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
#include "bsynclib_timer.h"

#ifndef BSYNCLIB_AUDIO_SOURCE_H__
#define BSYNCLIB_AUDIO_SOURCE_H__

typedef struct BSYNClib_AudioSource_Data
{
	bool bDigital;
	bool bSamplingRateReceived;
} BSYNClib_AudioSource_Data;

typedef struct BSYNClib_AudioSource_Results
{
	bool bMutePending;
} BSYNClib_AudioSource_Results;

typedef struct BSYNClib_AudioSource
{
	BSYNClib_DelayElement sElement;

	BSYNClib_Timer * psUnmuteTimer;
	BSYNClib_Timer * psUnconditionalUnmuteTimer;

	BSYNClib_AudioSource_Data sData;
	BSYNClib_AudioSource_Data sSnapshot;
	BSYNClib_AudioSource_Results sResults;
	BSYNClib_AudioSource_Config sConfig;
	BSYNClib_Source_Status sStatus;
} BSYNClib_AudioSource;

BSYNClib_AudioSource * BSYNClib_AudioSource_Create(void);

void BSYNClib_AudioSource_Destroy(BSYNClib_AudioSource * psSource);

bool BSYNClib_AudioSource_SyncCheck(BSYNClib_AudioSource * psSource);

void BSYNClib_AudioSource_Reset_isr(BSYNClib_AudioSource * psSource);

void BSYNClib_AudioSource_GetDefaultConfig(BSYNClib_AudioSource_Config * psConfig);

void BSYNClib_AudioSource_P_SelfClearConfig_isr(BSYNClib_AudioSource * psSource);

BERR_Code BSYNClib_AudioSource_P_ProcessConfig_isr(BSYNClib_AudioSource * psSource);

void BSYNClib_AudioSource_Snapshot_isr(BSYNClib_AudioSource * psSource);

void BSYNClib_AudioSource_P_GetDefaultStatus(BSYNClib_Source_Status * psStatus);

BERR_Code BSYNClib_AudioSource_SetMute(BSYNClib_AudioSource * psSource, bool bMute);

#endif /* BSYNCLIB_AUDIO_SOURCE_H__ */

