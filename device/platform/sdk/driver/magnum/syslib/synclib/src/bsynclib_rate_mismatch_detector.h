/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_rate_mismatch_detector.h $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 1/20/12 7:52p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_rate_mismatch_detector.h $
* 
* Hydra_Software_Devel/3   1/20/12 7:52p bandrews
* SW7358-219: ensure that synclib never tries to apply BVN offsets
* between displays that don't have matching refresh rates
* 
* Hydra_Software_Devel/2   4/28/09 10:43p bandrews
* PR54526: Fix format change call into sync; fix static rate mismatch
* detection
* 
* Hydra_Software_Devel/1   3/24/08 3:09p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/4   2/26/08 10:21p bandrews
* PR37951: Fixed units.  Implemented static rate mismatch detection.
* 
* Hydra_Software_Devel/3   1/31/08 3:09p bandrews
* PR37951: Fixed references to syslib callbacks
* 
* Hydra_Software_Devel/2   1/3/08 5:17p bandrews
* PR37951: Updated based on initial feedback
* 
* Hydra_Software_Devel/1   12/12/07 2:54p bandrews
* PR37951: Initial check-in
***************************************************************************/

#include "bsyslib.h"
#include "bsyslib_callback.h"
#include "bsynclib_video_format.h"

#ifndef BSYNCLIB_RATE_MISMATCH_DETECTOR_H__
#define BSYNCLIB_RATE_MISMATCH_DETECTOR_H__

/*
Summary:
*/
typedef struct BSYNClib_RateMismatchDetector_Impl * BSYNClib_RateMismatchDetector_Handle;

/*
Summary:
*/
typedef struct
{
	bool bEnabled;

	BSYSlib_Callback cbStateChange;
	
	struct 
	{
		unsigned int uiTimeout; /* time to wait before checking the number of callbacks received vs the acceptable callback count for rate mismatch */
		unsigned int uiAcceptableMtbcLower; /* acceptable mean time between callbacks lower bound */
		unsigned int uiAcceptableMtbcUpper; /* acceptable mean time between callbacks upper bound */
		unsigned int uiAcceptableTtlc; /* acceptable time to last callback for mismatches */
	} sMismatch;
	struct
	{
		unsigned int uiTimeout; /* time to wait before checking the number of callbacks received vs the acceptable callback count for rate rematch */
		unsigned int uiAcceptableTtlc; /* acceptable time to last callback for rematches */
	} sRematch;
} BSYNClib_RateMismatchDetector_Settings;

/*
Summary:
*/
typedef struct
{
	unsigned long ulDelayNotificationTime; /* when was the notification received in ms */
} BSYNClib_RateMismatchDetector_DelayNotificationInfo;

/*
Summary:
*/
typedef struct
{
	bool bSourceSinkMatched; /* source 0 matches sink 0 strictly */
	bool bSinkSinkMatched; /* all sinks match each other loosely */
} BSYNClib_RateMismatchDetector_Status;

typedef struct
{
	unsigned int uiVideoSourceCount;
	unsigned int uiVideoSinkCount;
} BSYNClib_RateMismatchDetector_Config;

/*
Summary:
Gets the default settings
*/
void BSYNClib_RateMismatchDetector_GetDefaultSettings(
	BSYNClib_RateMismatchDetector_Settings * psSettings /* [out] */
);

/*
Summary:
Opens a state machine handle
*/
BERR_Code BSYNClib_RateMismatchDetector_Open(
	BSYNClib_Channel_Handle hParent,
	const BSYNClib_RateMismatchDetector_Settings * psSettings,
	BSYNClib_RateMismatchDetector_Handle * phDetector
);

/*
Summary:
Closes a state machine handle
*/
void BSYNClib_RateMismatchDetector_Close(
	BSYNClib_RateMismatchDetector_Handle hDetector
);

/*
Summary:
*/
BERR_Code BSYNClib_RateMismatchDetector_DelayNotificationHandler_isr(
	BSYNClib_RateMismatchDetector_Handle hDetector,
	unsigned int uiSource,
	const BSYNClib_RateMismatchDetector_DelayNotificationInfo * psInfo
);

/*
Summary:
*/
BERR_Code BSYNClib_RateMismatchDetector_ResetSourceMeasurements_isr(
	BSYNClib_RateMismatchDetector_Handle hDetector,
	unsigned int uiSource
);

/*
Summary:
*/
BERR_Code BSYNClib_RateMismatchDetector_ResetSourceData_isr(
	BSYNClib_RateMismatchDetector_Handle hDetector,
	unsigned int uiSource
);

/*
Summary:
*/
BERR_Code BSYNClib_RateMismatchDetector_SetVideoSourceFormat_isr(
	BSYNClib_RateMismatchDetector_Handle hSync,
	unsigned int uiSource,
	const BSYNClib_VideoFormat * psFormat
);

/*
Summary:
*/
BERR_Code BSYNClib_RateMismatchDetector_SetVideoSinkFormat_isr(
	BSYNClib_RateMismatchDetector_Handle hSync,
	unsigned int uiSink,
	const BSYNClib_VideoFormat * psFormat,
	bool bMasterFrameRateEnabled
);

/*
Summary:
*/
BERR_Code BSYNClib_RateMismatchDetector_GetStatus(
	BSYNClib_RateMismatchDetector_Handle hDetector,
	BSYNClib_RateMismatchDetector_Status * psStatus
);

void BSYNClib_RateMismatchDetector_GetConfig(
	BSYNClib_RateMismatchDetector_Handle hDetector,
	BSYNClib_RateMismatchDetector_Config * psConfig
);

BERR_Code BSYNClib_RateMismatchDetector_SetConfig(
	BSYNClib_RateMismatchDetector_Handle hDetector,
	const BSYNClib_RateMismatchDetector_Config * psConfig
);

#endif /* BSYNCLIB_RATE_MISMATCH_DETECTOR_H__ */

