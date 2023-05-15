/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_audio_task.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 2/6/13 6:45p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/bdsp_audio_task.c $
 * 
 * Hydra_Software_Devel/7   2/6/13 6:45p nihar
 * SW7425-4216: Merging to mainline.
 * 
 * Hydra_Software_Devel/SW7425-4216/1   12/4/12 5:06p gprasad
 * SW7425-4216: Implementing code review comments
 * 
 * Hydra_Software_Devel/6   6/19/12 3:16a gprasad
 * SW7425-2952: DSP directory structure flattening
 * 
 * Hydra_Software_Devel/5   2/15/12 3:02p ashoky
 * SW7425-2308: Adding interface to put audio firmware in audio gap fill
 * mode.
 * 
 * Hydra_Software_Devel/4   6/9/11 8:11p gautamk
 * SW7422-357 : [7425] Modifying API prtotype for Add/Remove stage input
 * 
 * Hydra_Software_Devel/3   4/29/11 11:42a srajapur
 * SW7422-368 : [7422] BDSP_AudioTask_GetDefaultXxxSettings is Raaga-
 * specific ---------> Moved raaga specific API from common files to
 * raaga files and renamed the API's accordingly.
 * 
 * Hydra_Software_Devel/2   4/20/11 1:10p jgarrett
 * SW7425-392: Removing BDSP_ProcessingType enum
 * 
 * Hydra_Software_Devel/1   4/6/11 2:15p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/12   3/15/11 11:58a gskerl
 * SW7422-146: Moved external declarations from other .c files into
 * bdsp_raaga_priv.h
 * 
 * Hydra_Software_Devel/11   2/26/11 12:24a srajapur
 * SW7422-293:[7422] Adding support for encode algorithms
 * 
 * Hydra_Software_Devel/10   2/16/11 5:37a srajapur
 * SW7422-247 : [7422] Adding all audio post processing algorithms.
 * 
 * Hydra_Software_Devel/9   2/15/11 3:57p jgarrett
 * SW7422-146: Coverity CID 235
 * 
 * Hydra_Software_Devel/8   2/15/11 3:53p jgarrett
 * SW7422-146: Coverity CID 236
 * 
 * Hydra_Software_Devel/7   2/5/11 4:07a srajapur
 * SW7422-247 : [7422] Adding Generic passthrul audio processing support.
 * 
 * Hydra_Software_Devel/6   1/19/11 7:23a gautamk
 * SW7422-191:Adding API for Default settings.
 * 
 * Hydra_Software_Devel/5   1/18/11 4:57a gautamk
 * SW7422-191: Implementing review comments.
 * 
 * Hydra_Software_Devel/4   1/18/11 4:25a gautamk
 * SW7422-191: Implementing review comments.
 * 
 * Hydra_Software_Devel/3   1/13/11 5:57a gautamk
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
#include "../src/raaga/bdsp_raaga_priv.h" /* FIXME : Need to remove _priv dependency */
#include "bdsp_raaga_fw_settings.h"


BDBG_MODULE(bdsp_audio_task);


BERR_Code BDSP_AudioTask_Pause(
    BDSP_TaskHandle task
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    if ( task->pause ) 
    {
        return task->pause(task->pTaskHandle);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }    
}

BERR_Code BDSP_AudioTask_Resume(
    BDSP_TaskHandle task
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    if ( task->resume ) 
    {
        return task->resume(task->pTaskHandle);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }    
}

BERR_Code BDSP_AudioTask_Advance(
    BDSP_TaskHandle task,
    unsigned ms
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    if ( task->advance )
    {
        return task->advance(task->pTaskHandle, ms);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }    
}

BERR_Code BDSP_AudioTask_GetTsmSettings_isr(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    BDSP_AudioTaskTsmSettings *pTsmSettings         /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pTsmSettings);

    BKNI_ASSERT_ISR_CONTEXT();

    if ( task->getTsmSettings_isr )
    {
        return task->getTsmSettings_isr(task->pTaskHandle, branchId, stageId, pTsmSettings);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

BERR_Code BDSP_AudioTask_SetTsmSettings_isr(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const BDSP_AudioTaskTsmSettings *pTsmSettings
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pTsmSettings);

    BKNI_ASSERT_ISR_CONTEXT();

    if ( task->setTsmSettings_isr )
    {
        return task->setTsmSettings_isr(task->pTaskHandle, branchId, stageId, pTsmSettings);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

BERR_Code BDSP_AudioTask_GetTsmStatus_isr(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    BDSP_AudioTaskTsmStatus *pTsmStatus         /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pTsmStatus);

    BKNI_ASSERT_ISR_CONTEXT();

    if ( task->getTsmStatus_isr )
    {
        return task->getTsmStatus_isr(task->pTaskHandle, branchId, stageId, pTsmStatus);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

BERR_Code BDSP_AudioTask_GetDatasyncSettings(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    BDSP_AudioTaskDatasyncSettings *pDatasyncSettings         /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pDatasyncSettings);

    if ( task->getDatasyncSettings )
    {
        return task->getDatasyncSettings(task->pTaskHandle, branchId, stageId, pDatasyncSettings);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

BERR_Code BDSP_AudioTask_SetDatasyncSettings(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const BDSP_AudioTaskDatasyncSettings *pDatasyncSettings
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pDatasyncSettings);

    if ( task->setDatasyncSettings )
    {
        return task->setDatasyncSettings(task->pTaskHandle, branchId, stageId, pDatasyncSettings);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

BERR_Code BDSP_AudioTask_GetDatasyncStatus(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    BDSP_AudioTaskDatasyncStatus *pDatasyncStatus         /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pDatasyncStatus);

    if ( task->getDatasyncStatus )
    {
        return task->getDatasyncStatus(task->pTaskHandle, branchId, stageId, pDatasyncStatus);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}
#if 0
BERR_Code BDSP_AudioTask_AddStageInput(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const BDSP_CIT_P_FwStgSrcDstDetails *pInputInfo
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pInputInfo);

    if ( task->addStageInput )
    {
        return task->addStageInput(task->pTaskHandle, branchId, stageId, pInputInfo);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

BERR_Code BDSP_AudioTask_RemoveStageInput(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const BDSP_CIT_P_FwStgSrcDstDetails *pInputInfo
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pInputInfo);

    if ( task->removeStageInput )
    {
        return task->removeStageInput(task->pTaskHandle, branchId, stageId, pInputInfo);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

BERR_Code BDSP_AudioTask_AddStageOutput(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const BDSP_CIT_P_FwStgSrcDstDetails *pOutputInfo
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pOutputInfo);

    if ( task->addStageOutput )
    {
        return task->addStageOutput(task->pTaskHandle, branchId, stageId, pOutputInfo);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

BERR_Code BDSP_AudioTask_RemoveStageOutput(
    BDSP_TaskHandle task,
    unsigned branchId,
    unsigned stageId,
    const BDSP_CIT_P_FwStgSrcDstDetails *pOutputInfo
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pOutputInfo);

    if ( task->removeStageOutput )
    {
        return task->removeStageOutput(task->pTaskHandle, branchId, stageId, pOutputInfo);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

#endif
/***************************************************************************
Summary: 
Get Current Interrupt Handlers for a task 
***************************************************************************/
void BDSP_AudioTask_GetInterruptHandlers_isr(
    BDSP_TaskHandle task,
    BDSP_AudioInterruptHandlers *pHandlers   /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pHandlers);

    BKNI_ASSERT_ISR_CONTEXT();

    if ( task->getAudioInterruptHandlers_isr)
    {
         task->getAudioInterruptHandlers_isr(task->pTaskHandle,pHandlers);
    }
}

/***************************************************************************
Summary: 
Set Current Interrupt Handlers for a task 
***************************************************************************/
BERR_Code BDSP_AudioTask_SetInterruptHandlers_isr(
    BDSP_TaskHandle task,
    const BDSP_AudioInterruptHandlers *pHandlers   /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);
    BDBG_ASSERT(NULL != pHandlers);

    BKNI_ASSERT_ISR_CONTEXT();

    if ( task->setAudioInterruptHandlers_isr)
    {
        return task->setAudioInterruptHandlers_isr(task->pTaskHandle,pHandlers);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

BERR_Code BDSP_AudioTask_GetDefaultDatasyncSettings(
        void *pSettingsBuffer,        /* [out] */
        size_t settingsBufferSize   /*[In]*/
    )
{    
    if(sizeof(BDSP_AudioTaskDatasyncSettings) != settingsBufferSize)
    {
        BDBG_ERR(("settingsBufferSize (%d) is not equal to Config size (%d) of DataSync ",
            settingsBufferSize,sizeof(BDSP_AudioTaskDatasyncSettings)));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BKNI_Memcpy((void *)(volatile void *)pSettingsBuffer,(void *)&(BDSP_sDefaultFrameSyncTsmSettings.sFrameSyncConfigParams),settingsBufferSize);     

    return BERR_SUCCESS;
}

BERR_Code BDSP_AudioTask_GetDefaultTsmSettings(
        void *pSettingsBuffer,        /* [out] */
        size_t settingsBufferSize   /*[In]*/
    )
{    
    if(sizeof(BDSP_AudioTaskTsmSettings) != settingsBufferSize)
    {
        BDBG_ERR(("settingsBufferSize (%d) is not equal to Config size (%d) of DataSync ",
            settingsBufferSize,sizeof(BDSP_AudioTaskTsmSettings)));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BKNI_Memcpy((void *)(volatile void *)pSettingsBuffer,(void *)&(BDSP_sDefaultFrameSyncTsmSettings.sTsmConfigParams),settingsBufferSize);     

    return BERR_SUCCESS;
}


BERR_Code BDSP_AudioTask_AudioGapFill(
    BDSP_TaskHandle task
    )
{
    BDBG_OBJECT_ASSERT(task, BDSP_Task);

    if ( task->audioGapFill )
    {
        return task->audioGapFill(task->pTaskHandle);
    }
    else
    {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void BDSP_AudioCapture_GetDefaultCreateSettings(
    BDSP_AudioCaptureCreateSettings *pSettings  /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    pSettings->maxChannels = 2;
    pSettings->channelBufferSize = 1536*1024;
    pSettings->hHeap = NULL;
}

BERR_Code BDSP_AudioCapture_Create(
    BDSP_ContextHandle hContext,
    const BDSP_AudioCaptureCreateSettings *pSettings,
    BDSP_AudioCaptureHandle *pCapture   /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(hContext, BDSP_Context);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pCapture);

    if ( hContext->createCapture ) 
    {
        return hContext->createCapture(hContext->pContextHandle, pSettings, pCapture);
    }
    else
    {
        *pCapture = NULL;
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void BDSP_AudioCapture_Destroy(
    BDSP_AudioCaptureHandle hCapture
    )
{
    BDBG_ASSERT(NULL != hCapture);
    BDBG_ASSERT(NULL != hCapture->destroy);
    hCapture->destroy(hCapture->pCapHandle);
}

void BDSP_AudioTask_GetDefaultAddCaptureSettings(
    BDSP_AudioTaskAddCaptureSettings *pSettings /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->updateRead = false;
    pSettings->eOpType = BDSP_AF_P_DistinctOpType_eStereo_PCM;
}

BERR_Code BDSP_AudioTask_AddCapture(
    BDSP_TaskHandle hTask,
    BDSP_AudioCaptureHandle hCapture,
    const BDSP_AudioTaskAddCaptureSettings *pSettings
    )
{
    BDBG_ASSERT(NULL != hCapture);
    BDBG_ASSERT(NULL != hCapture->add);
    return hCapture->add(hCapture->pCapHandle, hTask->pTaskHandle, pSettings);
}

void BDSP_AudioTask_RemoveCapture(
    BDSP_TaskHandle hTask,
    BDSP_AudioCaptureHandle hCapture
    )
{
    BDBG_ASSERT(NULL != hCapture);
    BDBG_ASSERT(NULL != hCapture->remove);
    hCapture->remove(hCapture->pCapHandle, hTask->pTaskHandle);
}

BERR_Code BDSP_AudioCapture_GetBuffer(
    BDSP_AudioCaptureHandle hCapture,
    BDSP_BufferDescriptor *pBuffers /* [out] */
    )
{
    BDBG_ASSERT(NULL != hCapture);
    BDBG_ASSERT(NULL != pBuffers);
    BDBG_ASSERT(NULL != hCapture->getBuffer);
    return hCapture->getBuffer(hCapture->pCapHandle, pBuffers);
}

BERR_Code BDSP_AudioCapture_ConsumeData(
    BDSP_AudioCaptureHandle hCapture,
    unsigned numBytes                   /* Number of bytes read from each buffer */
    )
{
    BDBG_ASSERT(NULL != hCapture);
    BDBG_ASSERT(NULL != hCapture->consumeData);
    return hCapture->consumeData(hCapture->pCapHandle, numBytes);
}
