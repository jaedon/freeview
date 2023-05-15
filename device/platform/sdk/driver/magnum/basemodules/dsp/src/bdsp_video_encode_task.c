/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_video_encode_task.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 1/12/12 8:07p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/common/bdsp_video_encode_task.c $
 * 
 * Hydra_Software_Devel/3   1/12/12 8:07p ashoky
 * FWRAA-390: Adding userconfig for datasync module for dsp video encoder.
 * 
 * Hydra_Software_Devel/2   10/13/11 7:20p ashoky
 * SW7231-344: Adding changes in the interface after discussion.
 * 
 * Hydra_Software_Devel/1   10/4/11 7:48p ashoky
 * SW7231-344: Adding changes in the interface after discussion.
 * 
 ***************************************************************************/
#include "bdsp.h"
#include "bdsp_priv.h"

/* Retruns the datasync settings for video encoder */
BERR_Code BDSP_VideoEncodeTask_GetDatasyncSettings(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    BDSP_VideoEncodeTaskDatasyncSettings *pDatasyncSettings         /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pDatasyncSettings);

    if ( task->getVideoEncodeDatasyncSettings )
    {
        return task->getVideoEncodeDatasyncSettings(task->pTaskHandle, branchId, stageId, pDatasyncSettings);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

/* Sets datasync settings for dsp video encoder */
BERR_Code BDSP_VideoEncodeTask_SetDatasyncSettings(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const BDSP_VideoEncodeTaskDatasyncSettings *pDatasyncSettings
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pDatasyncSettings);

    if ( task->setVideoEncodeDatasyncSettings )
    {
        return task->setVideoEncodeDatasyncSettings(task->pTaskHandle, branchId, stageId, pDatasyncSettings);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

/* Function for getting a free picture buffer. Returns a NULL address if no picture buffer is available */
BERR_Code BDSP_VideoEncode_getPictureBuffer_isr(	  
													BDSP_TaskHandle task,	  
													BVENC_VF_sPicParamBuff **pPictureParmBuf
													)
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
	if ( task->getPictureBuffer_isr) 
    {
        return task->getPictureBuffer_isr(task->pTaskHandle, pPictureParmBuf);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    } 
}

BERR_Code BDSP_VideoEncode_putPicture_isr(
												BDSP_TaskHandle 	task,
												uint32_t			ui32PPBAddress
												)
{
	BDBG_OBJECT_ASSERT(task, BDSP_Task);
	if ( task->putPicture_isr)
    {
		return task->putPicture_isr(task->pTaskHandle, ui32PPBAddress);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    } 
}



