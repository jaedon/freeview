/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_task.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 2/6/13 6:50p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/bdsp_task.c $
 * 
 * Hydra_Software_Devel/6   2/6/13 6:50p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4216/3   12/4/12 5:07p gprasad
 * SW7425-4216: Implementing code review comments
 * 
 * Hydra_Software_Devel/SW7425-4216/2   11/28/12 4:25p gprasad
 * SW7425-4216: Added capture create and destroy apis for configuring
 * capture
 * 
 * Hydra_Software_Devel/5   6/15/11 4:20p gautamk
 * SW7422-357 : [7425] Adding support for new API for getStageInputIndex
 * 
 * Hydra_Software_Devel/4   6/14/11 3:14p gautamk
 * SW7422-357 : [7425] Merging to mainline
 * 
 * Hydra_Software_Devel/3   6/3/11 11:59a srajapur
 * SW7422-357 : [7425] Added support for MS11
 * 
 * Hydra_Software_Devel/2   6/2/11 7:56p srajapur
 * SW7422-358 : [7425] Added support for MS11
 * 
 * Hydra_Software_Devel/1   4/6/11 2:16p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/3   1/13/11 6:00a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 * Hydra_Software_Devel/2   12/15/10 6:47p jgarrett
 * SW7422-146: Initial compileable prototype
 * 
 * Hydra_Software_Devel/1   12/15/10 2:00p jgarrett
 * SW7422-146: Adding initial BDSP files
 * 
 ***************************************************************************/

#include "bdsp.h"
#include "bdsp_priv.h"

BDBG_MODULE(bdsp_task);

BDBG_OBJECT_ID(BDSP_TaskStageInput);
BDBG_OBJECT_ID(BDSP_AudioCapture);


void BDSP_Task_GetDefaultCreateSettings(
    BDSP_ContextHandle context,
    BDSP_TaskCreateSettings *pSettings     /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(context, BDSP_Context);
    BDBG_ASSERT(NULL != pSettings);

    if ( context->getDefaultTaskSettings ) 
    {
        context->getDefaultTaskSettings(context->pContextHandle, pSettings);
    }
    else
    {
        BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    }
}

BERR_Code BDSP_Task_Create(
    BDSP_ContextHandle context,
    const BDSP_TaskCreateSettings *pSettings,
    BDSP_TaskHandle *pTask    /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(context, BDSP_Context);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pTask);

    if ( context->createTask ) 
    {
        return context->createTask(context->pContextHandle, pSettings, pTask);
    }
    else
    {
        *pTask = NULL;
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void BDSP_Task_Destroy(
    BDSP_TaskHandle task
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != task->destroy);
    task->destroy(task->pTaskHandle);
}

BERR_Code BDSP_Task_Start(
    BDSP_TaskHandle task
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    if ( task->start ) 
    {
        return task->start(task->pTaskHandle);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

BERR_Code BDSP_Task_Stop(
    BDSP_TaskHandle task
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    if ( task->stop ) 
    {
        return task->stop(task->pTaskHandle);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }    
}

BERR_Code BDSP_Task_GetStageSettings(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    void *pSettingsBuffer,        /* [out] */
    size_t settingsBufferSize
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pSettingsBuffer);
    BDBG_ASSERT(settingsBufferSize > 0);

    if ( task->getStageSettings ) 
    {
        return task->getStageSettings(task->pTaskHandle, branchId, stageId, pSettingsBuffer, settingsBufferSize);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }        
}

BERR_Code BDSP_Task_SetStageSettings(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const void *pSettingsBuffer,
    size_t settingsBufferSize
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pSettingsBuffer);
    BDBG_ASSERT(settingsBufferSize > 0);

    if ( task->setStageSettings ) 
    {
        return task->setStageSettings(task->pTaskHandle, branchId, stageId, pSettingsBuffer, settingsBufferSize);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }        
}

BERR_Code BDSP_Task_GetStageStatus(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    void *pStatusBuffer,        /* [out] */
    size_t statusBufferSize
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pStatusBuffer);
    BDBG_ASSERT(statusBufferSize > 0);

    if ( task->getStageStatus ) 
    {
        return task->getStageStatus(task->pTaskHandle, branchId, stageId, pStatusBuffer, statusBufferSize);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }        
}

BERR_Code BDSP_Task_AddStageInput(
    BDSP_TaskHandle task,
    const BDSP_TaskAddStageInputSettings *pSettings,
    BDSP_TaskStageInputHandle *pHandle /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pSettings);    
    BDBG_ASSERT(NULL != pHandle);        
    if ( task->addStageInput) 
    {
        return task->addStageInput(task->pTaskHandle,pSettings,pHandle);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }        
}

BERR_Code BDSP_Task_RemoveStageInput(
    BDSP_TaskStageInputHandle input
    )
{
    BDBG_OBJECT_ASSERT(input, BDSP_TaskStageInput);
    
    if ( input->task->removeStageInput) 
    {
        return input->task->removeStageInput(input->pStageInputHandle);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }        
}

BERR_Code BDSP_Task_GetStageInputIndex(
    BDSP_TaskStageInputHandle input,
    unsigned    *inputIndex
    )
{
    BDBG_OBJECT_ASSERT(input, BDSP_TaskStageInput);
    BDBG_ASSERT(inputIndex);
    
    if ( input->task->getStageInputIndex) 
    {
        return input->task->getStageInputIndex(input->pStageInputHandle,inputIndex);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }        
}    


void BDSP_Task_GetDefaultAddStageInputSettings(
    BDSP_CIT_P_FwStgSrcDstType  inputType,
    BDSP_TaskAddStageInputSettings *pSettings /*[out] */
    )
{
    unsigned i=0;
    BDBG_ASSERT(pSettings);
    
    BKNI_Memset((void *)pSettings,0,sizeof(BDSP_TaskAddStageInputSettings));
    pSettings->sourceDetails.eType = inputType;
    
    if(inputType >= BDSP_CIT_P_FwStgSrcDstType_eMax)
    {
        BDBG_MSG(("Not a valid input type %d",inputType));
        goto end;
    }
        
    if(inputType == BDSP_CIT_P_FwStgSrcDstType_eFMMBuf)
    {
        pSettings->sourceDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32NumBuffers =0;
        pSettings->sourceDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffId.ui32NumBuffers =0;
                
        pSettings->sourceDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffId.eBufferType=BDSP_AF_P_BufferType_eFMM;        
        pSettings->sourceDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffId.eBufferType = BDSP_AF_P_BufferType_eInvalid;
        
        for(i = 0 ; i < BDSP_AF_P_MAX_CHANNEL_PAIR; i++)
        {     
            pSettings->sourceDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32AdaptRateCtrlIds[i]=BDSP_INVALID_INDEX;     
            pSettings->sourceDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffId.ui32AdaptRateCtrlIds[i]=BDSP_INVALID_INDEX;                             
        }

        for(i = 0 ; i < BDSP_AF_P_MAX_CHANNELS; i++)        
        {
            pSettings->sourceDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffId.ui32BufferId[i]=BDSP_INVALID_INDEX;   
            pSettings->sourceDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffId.ui32BufferId[i]=BDSP_INVALID_INDEX;                          
        } 
    }
    else if(inputType == BDSP_CIT_P_FwStgSrcDstType_eFwStg)
    {
        pSettings->sourceDetails.uDetails.sFwStage.ui32BranchId = BDSP_INVALID_INDEX;
        pSettings->sourceDetails.uDetails.sFwStage.ui32StageId= BDSP_INVALID_INDEX;        
    }
    else if(inputType == BDSP_CIT_P_FwStgSrcDstType_eDRAMBuf)
    {
        pSettings->sourceDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffer.ui32NumBuffers=0;
        pSettings->sourceDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.ui32NumBuffers =0;
                
        pSettings->sourceDetails.uDetails.sIoBuf.uIoBuffer.sIoBuffer.eBufferType=BDSP_AF_P_BufferType_eDRAM;        
        pSettings->sourceDetails.uDetails.sIoBuf.uIoGenericBuffer.sIoGenericBuffer.eBufferType = BDSP_AF_P_BufferType_eDRAM;        
    }    
end:
    return;
}

