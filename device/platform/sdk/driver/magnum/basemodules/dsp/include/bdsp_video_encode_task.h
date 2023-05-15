/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_video_encode_task.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 6/19/12 3:16a $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: 
 * 
 ***************************************************************************/
 
#ifndef BDSP_VIDEO_ENCODE_TASK_H_
#define BDSP_VIDEO_ENCODE_TASK_H_
 
#include "bchp.h"
#include "bint.h"
#include "bmem.h"
#include "breg_mem.h"
#include "btmr.h"
#include "bdsp_task.h"
#include "bdsp_raaga_fw_settings.h"


/***************************************************************************
Summary:
Get Datasync settings for a particular task of video Encode type
***************************************************************************/
BERR_Code BDSP_VideoEncodeTask_GetDatasyncSettings(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    BDSP_VideoEncodeTaskDatasyncSettings *pDataSyncSettings         /* [out] */
    );

/***************************************************************************
Summary:
Set Datasync settings for a particular task of video Encode type
***************************************************************************/
BERR_Code BDSP_VideoEncodeTask_SetDatasyncSettings(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const BDSP_VideoEncodeTaskDatasyncSettings *pDataSyncSettings
    );

/* Function for getting a free picture buffer. Returns a NULL address if no picture buffer is available */
BERR_Code BDSP_VideoEncode_getPictureBuffer_isr(	  
													BDSP_TaskHandle task,	  
													BVENC_VF_sPicParamBuff **pPictureParmBuf
													);

BERR_Code BDSP_VideoEncode_putPicture_isr(
												BDSP_TaskHandle 	task,
												uint32_t			ui32PPBAddress
												);


#endif /* BDSP_VIDEO_ENCODE_TASK_H_ */
