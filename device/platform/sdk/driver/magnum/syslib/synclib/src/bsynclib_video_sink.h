/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_video_sink.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 3/24/08 3:10p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_video_sink.h $
* 
* Hydra_Software_Devel/1   3/24/08 3:10p bandrews
* PR40865: Fixed
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
#include "bsynclib_video_format.h"
#include "bsynclib_delay_element.h"

#ifndef BSYNCLIB_VIDEO_SINK_H__
#define BSYNCLIB_VIDEO_SINK_H__

typedef struct BSYNClib_VideoSink_Data
{
	unsigned int uiMinDelay; /* TODO: needed? */
	
	bool bForcedCaptureEnabled; /* is forced capture enabled on this window */
	bool bMasterFrameRateEnabled; /* is master frame rate enabled on the main window for this display */
	bool bFullScreen; /* does window rect match display rect? */
	bool bVisible; /* is this window visible? */
} BSYNClib_VideoSink_Data;

typedef struct BSYNClib_VideoSink_Results
{
	bool bValidated; /* TODO: needed? */
} BSYNClib_VideoSink_Results;

/*
Summary:
*/
typedef struct BSYNClib_VideoSink
{
	BSYNClib_DelayElement sElement;

	BSYNClib_VideoFormat sFormat;

	BSYNClib_VideoSink_Data sData;
	BSYNClib_VideoSink_Data sSnapshot;
	BSYNClib_VideoSink_Results sResults;
	BSYNClib_VideoSink_Config sConfig;
	BSYNClib_Sink_Status sStatus;
} BSYNClib_VideoSink;

BSYNClib_VideoSink * BSYNClib_VideoSink_Create(void);

void BSYNClib_VideoSink_Destroy(BSYNClib_VideoSink * psSink);

bool BSYNClib_VideoSink_SyncCheck(BSYNClib_VideoSink * psSink);

void BSYNClib_VideoSink_Reset_isr(BSYNClib_VideoSink * psSink);

void BSYNClib_VideoSink_GetDefaultConfig(BSYNClib_VideoSink_Config * psConfig);

void BSYNClib_VideoSink_P_SelfClearConfig_isr(BSYNClib_VideoSink * psSink);

BERR_Code BSYNClib_VideoSink_P_ProcessConfig_isr(BSYNClib_VideoSink * psSink);

void BSYNClib_VideoSink_Snapshot_isr(BSYNClib_VideoSink * psSink);

void BSYNClib_VideoSink_P_GetDefaultStatus(BSYNClib_Sink_Status * psStatus);

#endif /* BSYNCLIB_VIDEO_SINK_H__ */

