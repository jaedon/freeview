/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsynclib_audio_sink.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 3/24/08 3:09p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/synclib/noarch/bsynclib_audio_sink.h $
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

#include "bsynclib_delay_element.h"

#ifndef BSYNCLIB_AUDIO_SINK_H__
#define BSYNCLIB_AUDIO_SINK_H__

typedef struct
{
	bool bCompressed;
	unsigned int uiSamplingRate;
} BSYNClib_AudioSink_Data;

typedef struct
{
	BSYNClib_DelayElement sElement;

	BSYNClib_AudioSink_Data sData;
	BSYNClib_AudioSink_Data sSnapshot;
	BSYNClib_AudioSink_Config sConfig;
	BSYNClib_Sink_Status sStatus;
} BSYNClib_AudioSink;

BSYNClib_AudioSink * BSYNClib_AudioSink_Create(void);

void BSYNClib_AudioSink_Destroy(BSYNClib_AudioSink * psSink);

bool BSYNClib_AudioSink_SyncCheck(BSYNClib_AudioSink * psSink);

void BSYNClib_AudioSink_Reset_isr(BSYNClib_AudioSink * psSink);

void BSYNClib_AudioSink_GetDefaultConfig(BSYNClib_AudioSink_Config * psConfig);

void BSYNClib_AudioSink_P_SelfClearConfig_isr(BSYNClib_AudioSink * psSink);

BERR_Code BSYNClib_AudioSink_P_ProcessConfig_isr(BSYNClib_AudioSink * psSink);

void BSYNClib_AudioSink_Snapshot_isr(BSYNClib_AudioSink * psSink);

void BSYNClib_AudioSink_P_GetDefaultStatus(BSYNClib_Sink_Status * psStatus);

#endif /* BSYNCLIB_AUDIO_SINK_H__ */

