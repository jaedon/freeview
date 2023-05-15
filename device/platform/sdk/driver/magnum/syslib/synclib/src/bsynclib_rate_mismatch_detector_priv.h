/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_rate_mismatch_detector_priv.h $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 1/20/12 7:53p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_rate_mismatch_detector_priv.h $
* 
* Hydra_Software_Devel/3   1/20/12 7:53p bandrews
* SW7358-219: ensure that synclib never tries to apply BVN offsets
* between displays that don't have matching refresh rates
* 
* Hydra_Software_Devel/2   8/4/09 4:56p bandrews
* PR52812: added improved rmd for dmv2
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
* Hydra_Software_Devel/1   12/12/07 2:54p bandrews
* PR37951: Initial check-in
***************************************************************************/

#include "bsyslib.h"
#include "bsyslib_list.h"
#include "bsynclib_priv.h"
#include "bsynclib_rate_mismatch_detector.h"
#include "bsynclib_timer.h"

#ifndef BSYNCLIB_RATE_MISMATCH_DETECTOR_PRIV_H__
#define BSYNCLIB_RATE_MISMATCH_DETECTOR_PRIV_H__

typedef struct BSYNClib_RateMismatchDetector_Sink
{
	unsigned int uiIndex;
	BSYNClib_VideoFormat sFormat;
	bool bMasterFrameRateEnabled;
} BSYNClib_RateMismatchDetector_Sink;

typedef struct BSYNClib_RateMismatchDetector_Source
{
	unsigned int uiIndex;
	BSYNClib_Timer * psTimer;

	BSYNClib_VideoFormat sFormat;

	unsigned int uiDelayCallbackCount;
	long lMeanTimeBetweenCallbacks;
	unsigned long ulLastCallbackTime;
} BSYNClib_RateMismatchDetector_Source;

typedef struct
{
	BSYSlib_List_Handle hSources;
	unsigned int uiSourceCount;
	
	BSYSlib_List_Handle hSinks;
	unsigned int uiSinkCount;
} BSYNClib_RateMismatchDetector_Data;

/*
Summary:
*/
struct BSYNClib_RateMismatchDetector_Impl
{
	/* TODO: change this so that I can schedule tasks instead of having a timer for each one */
	BSYNClib_Timer * psStaticRateMatchTimer;
	BSYNClib_Timer * psStaticRateMismatchTimer;
	BSYNClib_Channel_Handle hParent;
	BSYNClib_RateMismatchDetector_Settings sSettings;
	BSYNClib_RateMismatchDetector_Config sConfig;
	BSYNClib_RateMismatchDetector_Data sData;
	BSYNClib_RateMismatchDetector_Status sStatus;
};

BERR_Code BSYNClib_RateMismatchDetector_P_StaticRateMatchTimerExpired(void * pvParm1, int iParm2, BSYSlib_Timer_Handle hTimer);
BERR_Code BSYNClib_RateMismatchDetector_P_StaticRateMismatchTimerExpired(void * pvParm1, int iParm2, BSYSlib_Timer_Handle hTimer);
BERR_Code BSYNClib_RateMismatchDetector_MismatchTimerExpired(void * pvParm1, int iParm2, BSYSlib_Timer_Handle hTimer);
BERR_Code BSYNClib_RateMismatchDetector_RematchTimerExpired(void * pvParm1, int iParm2, BSYSlib_Timer_Handle hTimer);
BERR_Code BSYNClib_RateMismatchDetector_P_ProcessConfig(BSYNClib_RateMismatchDetector_Handle hDetector);

void BSYNClib_RateMismatchDetector_P_TestSourceAndSinkFormats_isr(
	BSYNClib_RateMismatchDetector_Handle hDetector, 
	BSYNClib_RateMismatchDetector_Source * psSource, 
	BSYNClib_RateMismatchDetector_Sink * psSink
);

void BSYNClib_RateMismatchDetector_P_CompareSinkFormats_isr(
	BSYNClib_RateMismatchDetector_Handle hDetector
);

bool BSYNClib_RateMismatchDetector_P_LooseRateMatchCheck_isr(
	BSYNClib_VideoFormat * psFormatA, 
	BSYNClib_VideoFormat * psFormatB
);

bool BSYNClib_RateMismatchDetector_P_StrictRateMatchCheck_isr(
	BSYNClib_VideoFormat * psFormatA, 
	BSYNClib_VideoFormat * psFormatB,
	bool bMasterFrameRateEnabled
);

#endif /* BSYNCLIB_RATE_MISMATCH_DETECTOR_PRIV_H__ */

