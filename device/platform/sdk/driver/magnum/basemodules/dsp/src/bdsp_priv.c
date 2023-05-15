/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 2/22/12 7:21p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/common/bdsp_priv.c $
 * 
 * Hydra_Software_Devel/3   2/22/12 7:21p ashoky
 * SW7231-585: Adding APIs to expose interrupt bits to interrupt DSP.
 * 
 * Hydra_Software_Devel/1   4/6/11 2:16p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/1   12/15/10 6:47p jgarrett
 * SW7422-146: Initial compileable prototype
 * 
 * Hydra_Software_Devel/1   12/15/10 2:31p jgarrett
 * SW7422-146: Adding initial BDSP files
 * 
 ***************************************************************************/

#include "bdsp.h"
#include "bdsp_priv.h"

BDBG_MODULE(bdsp_priv);

BDBG_OBJECT_ID(BDSP_Device);
BDBG_OBJECT_ID(BDSP_Context);
BDBG_OBJECT_ID(BDSP_Task);
BDBG_OBJECT_ID(BDSP_ExternalInterrupt);
BDBG_OBJECT_ID(BDSP_RdbRegister);


void BDSP_P_InitDevice(
    BDSP_Device *pDevice,
    void *pDeviceHandle
    )
{
    BDBG_ASSERT(NULL != pDevice);
    BDBG_ASSERT(NULL != pDeviceHandle);
    BKNI_Memset(pDevice, 0, sizeof(BDSP_Device));
    BDBG_OBJECT_SET(pDevice, BDSP_Device);
    pDevice->pDeviceHandle = pDeviceHandle;
}

void BDSP_P_InitContext(
    BDSP_Context *pContext,
    void *pContextHandle
    )
{
    BDBG_ASSERT(NULL != pContext);
    BDBG_ASSERT(NULL != pContextHandle);
    BKNI_Memset(pContext, 0, sizeof(BDSP_Context));
    BDBG_OBJECT_SET(pContext, BDSP_Context);
    pContext->pContextHandle = pContextHandle;
}

void BDSP_P_InitTask(
    BDSP_Task *pTask,
    void *pTaskHandle
    )
{
    BDBG_ASSERT(NULL != pTask);
    BDBG_ASSERT(NULL != pTaskHandle);
    BKNI_Memset(pTask, 0, sizeof(BDSP_Task));
    BDBG_OBJECT_SET(pTask, BDSP_Task);
    pTask->pTaskHandle = pTaskHandle;
}

