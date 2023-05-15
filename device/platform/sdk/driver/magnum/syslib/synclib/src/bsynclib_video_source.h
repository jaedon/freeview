/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_video_source.h $
* $brcm_Revision: Hydra_Software_Devel/6 $
* $brcm_Date: 1/16/12 8:26p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_video_source.h $
* 
* Hydra_Software_Devel/6   1/16/12 8:26p bandrews
* SW7346-640: preserve delay estimation when callbacks come in an
* unexpected order
* 
* Hydra_Software_Devel/5   3/22/10 5:39p bandrews
* SW7408-83: remove undefined functions; add unconditional video unmute
* 
* Hydra_Software_Devel/4   3/18/10 9:34p bandrews
* SW7405-4042: rework delay estimation so that imprecise lipsync works
* correctly
* 
* Hydra_Software_Devel/3   1/26/10 9:00p bandrews
* SW7405-3774: added mute control support
* 
* Hydra_Software_Devel/2   12/10/09 9:18p bandrews
* SW7401-3634: adding PWC (now JTI) support to synclib
* 
* Hydra_Software_Devel/1   3/24/08 3:10p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/4   2/25/08 9:33p bandrews
* PR37951: Fixed various bugs
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
#include "bsyslib.h"
#include "bsynclib_timer.h"
#include "bsynclib_video_format.h"
#include "bsynclib_delay_element.h"
#include "bsynclib_priv.h"

#ifndef BSYNCLIB_VIDEO_SOURCE_H__
#define BSYNCLIB_VIDEO_SOURCE_H__

typedef struct BSYNClib_VideoSource_Data
{
	bool bDigital;
	bool bLastPictureHeld; /* is the decoder holding the last picture or blanking? */
	unsigned int uiJitterToleranceImprovementThreshold;
} BSYNClib_VideoSource_Data;

typedef struct
{
	bool bAdjusted;
	int iValue;
} BSYNClib_VideoSource_JitterToleranceImprovementFactor;

typedef struct BSYNClib_VideoSource_Results
{
	bool bDelaySaved;
	BSYNClib_Delay sSavedDelay;
	bool bMutePending;
	
	BSYNClib_VideoSource_JitterToleranceImprovementFactor sJtiFactor;
} BSYNClib_VideoSource_Results;

typedef struct BSYNClib_VideoSource
{
	BSYNClib_DelayElement sElement;
	BSYNClib_VideoFormat sFormat;

	BSYNClib_Timer * psUnmuteTimer;
	BSYNClib_Timer * psUnconditionalUnmuteTimer;
	BSYNClib_Timer * psTsmLockTimer;

	BSYNClib_VideoSource_Data sData;
	BSYNClib_VideoSource_Data sSnapshot;
	BSYNClib_VideoSource_Results sResults;
	BSYNClib_VideoSource_Config sConfig;
	BSYNClib_Source_Status sStatus;
} BSYNClib_VideoSource;

BSYNClib_VideoSource * BSYNClib_VideoSource_Create(void);

void BSYNClib_VideoSource_Destroy(BSYNClib_VideoSource * psSource);

bool BSYNClib_VideoSource_SyncCheck(BSYNClib_VideoSource * psSource);

void BSYNClib_VideoSource_Reset_isr(BSYNClib_VideoSource * psSource);

BERR_Code BSYNClib_VideoSource_TsmLockTimerExpired(void * pvParm1, int iParm2, BSYSlib_Timer_Handle hTimer);

BERR_Code BSYNClib_VideoSource_SetMute(BSYNClib_VideoSource * psSource, bool bMute);

void BSYNClib_VideoSource_GetDefaultConfig(BSYNClib_VideoSource_Config * psConfig);

void BSYNClib_VideoSource_P_SelfClearConfig_isr(BSYNClib_VideoSource * psSource);

BERR_Code BSYNClib_VideoSource_P_ProcessConfig_isr(BSYNClib_VideoSource * psSource);

void BSYNClib_VideoSource_Snapshot_isr(BSYNClib_VideoSource * psSource);

void BSYNClib_VideoSource_P_GetDefaultStatus(BSYNClib_Source_Status * psStatus);

bool BSYNClib_VideoSource_EstimateDelay_isr(BSYNClib_VideoSource * psSource);

BERR_Code BSYNClib_VideoSource_RateMismatchDetected(BSYNClib_VideoSource * psSource);

BERR_Code BSYNClib_VideoSource_RateRematchDetected(BSYNClib_VideoSource * psSource);

#endif /* BSYNCLIB_VIDEO_SOURCE_H__ */

