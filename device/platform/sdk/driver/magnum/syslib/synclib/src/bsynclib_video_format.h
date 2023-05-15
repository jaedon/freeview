/***************************************************************************
*     Copyright (c) 2004-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_video_format.h $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 6/30/10 3:50p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_video_format.h $
* 
* Hydra_Software_Devel/4   6/30/10 3:50p bandrews
* SW7335-781: check in private function protos
* 
* Hydra_Software_Devel/3   8/4/09 4:56p bandrews
* PR52812: added improved rmd for dmv2
* 
* Hydra_Software_Devel/2   7/29/09 3:42p bandrews
* PR50988: Add support for 7 and 14 DIVX rates
* 
* Hydra_Software_Devel/1   3/24/08 3:10p bandrews
* PR40865: Fixed
* 
* Hydra_Software_Devel/2   2/26/08 10:21p bandrews
* PR37951: Fixed units.  Implemented static rate mismatch detection.
* 
* Hydra_Software_Devel/1   1/3/08 6:02p bandrews
* PR37951: Updated based on initial feedback
***************************************************************************/

#include "bstd.h"
#include "bavc.h"

#ifndef BSYNCLIB_VIDEO_FORMAT_H__
#define BSYNCLIB_VIDEO_FORMAT_H__

typedef struct
{
	bool bValid;
	unsigned int uiHeight; /* height of format, required to predict VDC MAD state changes */
	bool bInterlaced; /* whether the format is interlaced */
	BAVC_FrameRateCode eFrameRate; /* the frame rate of the format */
} BSYNClib_VideoFormat_Data;

typedef struct
{
	bool bChanged;
} BSYNClib_VideoFormat_DiffResults;

typedef struct
{
	BSYNClib_VideoFormat_Data sData;
	BSYNClib_VideoFormat_Data sSnapshot;
} BSYNClib_VideoFormat;

void BSYNClib_VideoFormat_Init(BSYNClib_VideoFormat * psFormat);

void BSYNClib_VideoFormat_Reset_isr(BSYNClib_VideoFormat * psFormat);

void BSYNClib_VideoFormat_Diff_isr(
	BSYNClib_VideoFormat * psDesired,
	BSYNClib_VideoFormat * psCurrent,
	BSYNClib_VideoFormat_DiffResults * psResults
);

void BSYNClib_VideoFormat_Patch_isr(
	BSYNClib_VideoFormat * psDesired,
	BSYNClib_VideoFormat * psCurrent,
	BSYNClib_VideoFormat_DiffResults * psResults
);

void BSYNClib_VideoFormat_Snapshot_isr(BSYNClib_VideoFormat * psFormat);

unsigned int BSYNClib_VideoFormat_P_GetDownconvertedFramePeriod(BSYNClib_VideoFormat * psFormat);
unsigned int BSYNClib_VideoFormat_P_GetDownconvertedFramePeriod_isr(BSYNClib_VideoFormat * psFormat);

unsigned int BSYNClib_VideoFormat_P_GetUpconvertedFramePeriod(BSYNClib_VideoFormat * psFormat);
unsigned int BSYNClib_VideoFormat_P_GetUpconvertedFramePeriod_isr(BSYNClib_VideoFormat * psFormat);

unsigned int BSYNClib_VideoFormat_P_GetFramePeriod(BSYNClib_VideoFormat * psFormat);
unsigned int BSYNClib_VideoFormat_P_GetFramePeriod_isr(BSYNClib_VideoFormat * psFormat);

unsigned int BSYNClib_VideoFormat_P_GetVsyncPeriod(BSYNClib_VideoFormat * psFormat);
unsigned int BSYNClib_VideoFormat_P_GetVsyncPeriod_isr(BSYNClib_VideoFormat * psFormat);

bool BSYNClib_VideoFormat_P_IsFrameRateSupported(BAVC_FrameRateCode eRate);
bool BSYNClib_VideoFormat_P_IsFrameRateSupported_isr(BAVC_FrameRateCode eRate);

#ifdef BDBG_DEBUG_BUILD
const char * BSYNClib_VideoFormat_P_GetFrameRateName(BAVC_FrameRateCode eRate);
const char * BSYNClib_VideoFormat_P_GetFrameRateName_isr(BAVC_FrameRateCode eRate);
#endif

#endif /* BSYNCLIB_VIDEO_FORMAT_H__ */

