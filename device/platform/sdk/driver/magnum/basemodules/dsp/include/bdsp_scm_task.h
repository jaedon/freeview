/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_scm_task.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 11/26/12 1:23p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: 
 * 
 ***************************************************************************/
 
#ifndef BDSP_SCM_TASK_H_
#define BDSP_SCM_TASK_H_

#include "bchp.h"
#include "bint.h"
#include "bmem.h"
#include "breg_mem.h"
#include "btmr.h"
#include "bdsp_task.h"
#include "bdsp_raaga_fw_settings.h"
#include "bdsp_raaga_fw_status.h"
#include "../src/raaga/bdsp_raaga_cmdresp_priv.h" 
 
/* API for Host to route SCM commands into Firmware */
BERR_Code BDSP_Scm_sendScmCommand(	  
													BDSP_TaskHandle task,	  
													BDSP_Raaga_P_SCM_CmdPayload *pScmCmdPayload
													);



#endif /* BDSP_SCM_TASK_H_ */
