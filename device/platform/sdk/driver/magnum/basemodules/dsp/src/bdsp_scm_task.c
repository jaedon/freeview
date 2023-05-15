/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_scm_task.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/27/12 2:52p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 *  
 ***************************************************************************/
#include "bdsp.h"
#include "bdsp_priv.h"
BERR_Code BDSP_Scm_sendScmCommand(	  
													BDSP_TaskHandle task,	  
													BDSP_Raaga_P_SCM_CmdPayload *pScmCmdPayload
													)
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
	if ( task->sendScmCommand) 
    {
        return (task->sendScmCommand(task->pTaskHandle, pScmCmdPayload));
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    } 
}




