/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_video_task.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 6/8/11 4:50p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/common/bdsp_video_task.h $
 * 
 * Hydra_Software_Devel/2   6/8/11 4:50p gautamk
 * SW7425-572: [7425] Adding DM related API support for VP6  in BDSP
 * 
 * Hydra_Software_Devel/1   4/6/11 2:17p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/1   12/15/10 2:00p jgarrett
 * SW7422-146: Adding initial BDSP files
 * 
 ***************************************************************************/
 
#ifndef BDSP_VIDEO_TASK_H_
#define BDSP_VIDEO_TASK_H_
 
#include "bchp.h"
#include "bint.h"
#include "bmem.h"
#include "breg_mem.h"
#include "btmr.h"
#include "bdsp_task.h"
#include "bxdm_picture.h"


BERR_Code BDSP_Video_GetPictureCount_isr(
    BDSP_TaskHandle task,
    unsigned *pPictureCount
    );

BERR_Code BDSP_Video_PeekAtPicture_isr(
    BDSP_TaskHandle task,
    unsigned index,
    BXDM_Picture **pUnifiedPicture    
    );

BERR_Code BDSP_Video_GetNextPicture_isr(
    BDSP_TaskHandle task,
    BXDM_Picture **pUnifiedPicture    
    );

BERR_Code BDSP_Video_ReleasePicture_isr(
    BDSP_TaskHandle task,
    BXDM_Picture *pUnifiedPicture    
    );

BERR_Code BDSP_Video_GetPictureDropPendingCount_isr(
    BDSP_TaskHandle task,
    unsigned *pPictureDropPendingCount
    );

BERR_Code BDSP_Video_RequestPictureDrop_isr(
    BDSP_TaskHandle task,
    unsigned *pPictureDropRequestCount
    );

BERR_Code BDSP_Video_DisplayInterruptEvent_isr(
    BDSP_TaskHandle task
    );

#endif
