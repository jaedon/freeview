/***************************************************************************
*     (c)2004-2011 Broadcom Corporation
*  
*  This program is the proprietary software of Broadcom Corporation and/or its licensors,
*  and may only be used, duplicated, modified or distributed pursuant to the terms and
*  conditions of a separate, written license agreement executed between you and Broadcom
*  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
*  no license (express or implied), right to use, or waiver of any kind with respect to the
*  Software, and Broadcom expressly reserves all rights in and to the Software and all
*  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
*  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
*  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
*   
*  Except as expressly set forth in the Authorized License,
*   
*  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
*  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
*  and to use this information only in connection with your use of Broadcom integrated circuit products.
*   
*  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
*  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
*  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
*  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
*  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
*  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
*  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
*  USE OR PERFORMANCE OF THE SOFTWARE.
*  
*  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
*  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
*  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
*  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
*  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
*  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
*  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
*  ANY LIMITED REMEDY.
* 
* $brcm_Workfile: bape_custom_processing.c $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 5/3/12 5:06p $
*
* API Description:
*   API name: CustomProcessing
*    Specific APIs related to custom audio post-processing
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/7422/bape_custom_processing.c $
* 
* Hydra_Software_Devel/4   5/3/12 5:06p jgarrett
* SW7425-2075: Merge to main branch
* 
* Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:52p jgarrett
* SW7425-2075: Adding new FMT interfaces
* 
* Hydra_Software_Devel/3   5/13/11 12:50p jgarrett
* SW7422-410: Fixing coverity defect
* 
* Hydra_Software_Devel/2   5/12/11 9:51a jgarrett
* SW7422-410: Fixing initial setting state
* 
* Hydra_Software_Devel/1   5/11/11 7:04p jgarrett
* SW7422-410: Adding CustomProcessing
* 
***************************************************************************/

#include "bape.h"
#include "bape_priv.h"
#include "bdsp_raaga.h"

BDBG_MODULE(bape_custom_processing);

BDBG_OBJECT_ID(BAPE_CustomProcessing);

typedef struct BAPE_CustomProcessing
{
    BDBG_OBJECT(BAPE_CustomProcessing)
    BAPE_PathNode node;
    BAPE_CustomProcessingSettings settings;
    void *pSettings;
    BAPE_Connector input;
} BAPE_CustomProcessing;

static BERR_Code BAPE_CustomProcessing_P_InputFormatChange(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, const BAPE_FMT_Descriptor *pNewFormat);
static BERR_Code BAPE_CustomProcessing_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_CustomProcessing_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_CustomProcessing_P_ApplyDspSettings(BAPE_CustomProcessingHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId);
static void BAPE_CustomProcessing_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_CustomProcessing_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);

void BAPE_CustomProcessing_GetDefaultSettings(
    BAPE_CustomProcessingSettings *pSettings   /* [out] default settings */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->algorithm = BDSP_AudioProcessing_eMax;
    pSettings->outputFormat = BAPE_MultichannelFormat_e2_0;
}

BERR_Code BAPE_CustomProcessing_Create(
    BAPE_Handle deviceHandle,
    const BAPE_CustomProcessingSettings *pSettings,
    BAPE_CustomProcessingHandle *pHandle
    )
{
    BAPE_CustomProcessingHandle handle;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pHandle);

    handle = BKNI_Malloc(sizeof(BAPE_CustomProcessing));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(handle, 0, sizeof(BAPE_CustomProcessing));
    BDBG_OBJECT_SET(handle, BAPE_CustomProcessing);    
    BAPE_P_InitPathNode(&handle->node, BAPE_PathNodeType_ePostProcessor, BAPE_PostProcessorType_eCustom, 1, deviceHandle, handle);
    handle->node.pName = "CustomProcessing";

    /* Initialize to stereo.  SetSettings may change this. */
    handle->node.connectors[0].useBufferPool = true;
    BAPE_Connector_P_GetFormat(&handle->node.connectors[0], &format);
    format.source = BAPE_DataSource_eDspBuffer;
    format.type = BAPE_DataType_ePcmStereo;
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[0], &format);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_set_format;
    }

    /* We can accept 2.0 input from the DSP */
    BAPE_PathNode_P_GetInputCapabilities(&handle->node, &caps);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcmStereo);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eDspBuffer);
    errCode = BAPE_PathNode_P_SetInputCapabilities(&handle->node, &caps);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_set_caps;
    }

    /* Generic Routines */
    handle->node.allocatePathToOutput = BAPE_DSP_P_AllocatePathToOutput;
    handle->node.configPathToOutput = BAPE_DSP_P_ConfigPathToOutput;
    handle->node.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;
    handle->node.startPathToOutput = BAPE_DSP_P_StartPathToOutput;
    handle->node.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;

    /* CustomProcessing Specifics */
    handle->node.inputFormatChange = BAPE_CustomProcessing_P_InputFormatChange;
    handle->node.allocatePathFromInput = BAPE_CustomProcessing_P_AllocatePathFromInput;
    handle->node.configPathFromInput = BAPE_CustomProcessing_P_ConfigPathFromInput;
    handle->node.stopPathFromInput = BAPE_CustomProcessing_P_StopPathFromInput;
    handle->node.removeInput = BAPE_CustomProcessing_P_RemoveInputCallback;

    /* Apply Settings */
    errCode = BAPE_CustomProcessing_SetSettings(handle, pSettings);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_set_settings;
    }

    *pHandle = handle;
    return BERR_SUCCESS;

err_set_settings:
err_set_caps:
err_set_format:
    BAPE_CustomProcessing_Destroy(handle);
    return errCode;
}


void BAPE_CustomProcessing_Destroy(
    BAPE_CustomProcessingHandle handle
    )
{
    bool running;
    BDBG_OBJECT_ASSERT(handle, BAPE_CustomProcessing);
    running = (handle->node.connectors[0].task != NULL)?true:false;
    BDBG_ASSERT(false == running);
    BDBG_ASSERT(NULL == handle->input);
    if ( handle->pSettings )
    {
        BKNI_Free(handle->pSettings);
        handle->pSettings = NULL;
    }
    BDBG_OBJECT_DESTROY(handle, BAPE_CustomProcessing);
    BKNI_Free(handle);
}

void BAPE_CustomProcessing_GetSettings(
    BAPE_CustomProcessingHandle handle,
    BAPE_CustomProcessingSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_CustomProcessing);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}


BERR_Code BAPE_CustomProcessing_SetSettings(
    BAPE_CustomProcessingHandle handle,
    const BAPE_CustomProcessingSettings *pSettings
    )
{
    BERR_Code errCode;
    bool running;
    bool algoChanged;
    bool formatChanged;
    BAPE_FMT_Descriptor format;

    BDBG_OBJECT_ASSERT(handle, BAPE_CustomProcessing);
    BDBG_ASSERT(NULL != pSettings);

    /* Check if we are running */
    running = (handle->node.connectors[0].task == NULL) ? false : true;
    /* Check if the algorithm is changing */
    algoChanged = (pSettings->algorithm == handle->settings.algorithm) ? false : true;

    BAPE_Connector_P_GetFormat(&handle->node.connectors[0], &format);
    switch ( pSettings->outputFormat )
    {
    case BAPE_MultichannelFormat_e2_0:
        format.type = BAPE_DataType_ePcmStereo;
        break;
    case BAPE_MultichannelFormat_e5_1:
        format.type = BAPE_DataType_ePcm5_1;
        break;
    case BAPE_MultichannelFormat_e7_1:
        format.type = BAPE_DataType_ePcm7_1;
        break;
    default:
        BDBG_ERR(("Invalid multichannel format"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Check if format changed */
    formatChanged = BAPE_FMT_P_IsEqual(&format, &handle->node.connectors[0].format);

    /* Only allow some changes if we are stopped */
    if ( running )
    {
        if ( algoChanged )
        {
            BDBG_ERR(("Can not change custom processing algorithm while running."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
        if ( formatChanged )
        {
            BDBG_ERR(("Can not change output format while running."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
  
    if ( formatChanged )
    {
        /* Propagate format change first */
        errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[0], &format);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    /* Check if algorithm changed */
    if ( algoChanged )
    {
        /* Check if this algorithm is supported */
        if ( !BDSP_Raaga_IsAudioProcessingSupported(pSettings->algorithm) )
        {
            BDBG_ERR(("Audio processing algorithm %u is not supported", pSettings->algorithm));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }

        /* Release old settings buffer if needed */
        if ( handle->pSettings )
        {
            BKNI_Free(handle->pSettings);
            handle->pSettings = NULL;
        }

        /* Alloc new settings buffer */
        if ( pSettings->algorithmSettingsSize > 0 )
        {
            handle->pSettings = BKNI_Malloc(pSettings->algorithmSettingsSize);
            if ( NULL == handle->pSettings )
            {
                return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            }

            /* Retrieve defaults for this algorithm into our buffer */
            errCode = BDSP_Raaga_GetDefaultAudioProcessingSettings(pSettings->algorithm, handle->pSettings, pSettings->algorithmSettingsSize);
            if ( errCode )
            {
                return BERR_TRACE(BERR_INVALID_PARAMETER);
            }
        }
    }

    /* Save settings */
    BKNI_Memcpy(&handle->settings, pSettings, sizeof(BAPE_CustomProcessingSettings));

    return BERR_SUCCESS;
}

void BAPE_CustomProcessing_GetAlgorithmSettings(
    BAPE_CustomProcessingHandle handle,
    void *pSettings,        /* [out] Should be defined as the correct data type for this custom algorithm */
    size_t settingsSize     /* Size of the settings structure in bytes */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_CustomProcessing);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(settingsSize > 0);
    BDBG_ASSERT(settingsSize <= handle->settings.algorithmSettingsSize);
    if ( NULL != handle->pSettings )
    {
        BKNI_Memcpy(pSettings, handle->pSettings, settingsSize);
    }
    else
    {
        BKNI_Memset(pSettings, 0, settingsSize);
    }
}

BERR_Code BAPE_CustomProcessing_SetAlgorithmSettings(
    BAPE_CustomProcessingHandle handle,
    const void *pSettings,  /* Should be defined as the correct data type for this custom algorithm */
    size_t settingsSize     /* Size of the settings structure in bytes */
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_CustomProcessing);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(settingsSize > 0);
    if ( settingsSize > handle->settings.algorithmSettingsSize )
    {
        BDBG_ERR(("Custom algorithm size is greater than specified in BAPE_CustomProcessingSettings.algorithmSettingsSize"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    if ( NULL != handle->pSettings )
    {
        BKNI_Memcpy(handle->pSettings, pSettings, settingsSize);
    }
    if ( handle->node.connectors[0].task != NULL )
    {
        errCode = BAPE_CustomProcessing_P_ApplyDspSettings(handle, 
                                                           handle->node.connectors[0].task, 
                                                           handle->node.connectors[0].branchId, 
                                                           handle->node.connectors[0].stageId);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}

void BAPE_CustomProcessing_GetConnector(
    BAPE_CustomProcessingHandle handle,
    BAPE_Connector *pConnector
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_CustomProcessing);
    BDBG_ASSERT(NULL != pConnector);
    *pConnector = &handle->node.connectors[0];
}


BERR_Code BAPE_CustomProcessing_AddInput(
    BAPE_CustomProcessingHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_CustomProcessing);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    if ( NULL != handle->input )
    {
        BDBG_ERR(("Can not have more than one input"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    errCode = BAPE_PathNode_P_AddInput(&handle->node, input);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    handle->input = input;
    return BERR_SUCCESS;
}

BERR_Code BAPE_CustomProcessing_RemoveInput(
    BAPE_CustomProcessingHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_CustomProcessing);
    BDBG_OBJECT_ASSERT(input, BAPE_PathConnector);
    if ( input != handle->input )
    {
        BDBG_ERR(("Input %s %s (%#x) is not connected", input->pParent->pName, input->pName, input));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    errCode = BAPE_PathNode_P_RemoveInput(&handle->node, input);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    handle->input = NULL;
    return BERR_SUCCESS;
}

BERR_Code BAPE_CustomProcessing_RemoveAllInputs(
    BAPE_CustomProcessingHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_CustomProcessing);
    if ( handle->input )
    {
        return BAPE_CustomProcessing_RemoveInput(handle, handle->input);
    }
    return BERR_SUCCESS;
}

BERR_Code BAPE_CustomProcessing_GetAlgorithmStatus(
    BAPE_CustomProcessingHandle handle,
    void *pStatus,      /* [out] Should be defined as the correct data type for this custom algorithm */
    size_t statusSize   /* Size of the status structure in bytes */
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_CustomProcessing);
    BDBG_ASSERT(NULL != pStatus);
    BDBG_ASSERT(statusSize > 0);
    if ( NULL != handle->node.connectors[0].task )
    {
        errCode = BDSP_Task_GetStageStatus(handle->node.connectors[0].task, 
                                           handle->node.connectors[0].branchId,
                                           handle->node.connectors[0].stageId,
                                           pStatus,
                                           statusSize);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    else
    {
        BDBG_ERR(("Processing Status not available while stopped."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    return BERR_SUCCESS;
}


static BERR_Code BAPE_CustomProcessing_P_InputFormatChange(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, const BAPE_FMT_Descriptor *pNewFormat)
{
    BAPE_FMT_Descriptor outputFormat;
    BAPE_DataType dataType;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pNewFormat);

    /* Set our output format but pass all other fields through */
    BAPE_Connector_P_GetFormat(&pNode->connectors[0], &outputFormat);
    dataType = outputFormat.type;
    outputFormat = *pNewFormat;
    outputFormat.type = dataType;
    errCode = BAPE_Connector_P_SetFormat(&pNode->connectors[0], &outputFormat);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}

static BERR_Code BAPE_CustomProcessing_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_CustomProcessingHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->pTaskCreateSettings);
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_CustomProcessing);
    /* Add stage to CIT and propagate task settings */
    errCode = BAPE_DSP_P_AddProcessingStage(pConnection->pSource->pTaskCreateSettings, pConnection->pSource->branchId, pConnection->pSource->stageId, 
                                            BAPE_DSP_P_GetDataTypeFromConnector(handle->input), 
                                            handle->settings.algorithm,
                                            false,
                                            &pNode->connectors[0].branchId, &pNode->connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    pNode->connectors[0].pTaskCreateSettings = pConnection->pSource->pTaskCreateSettings;
    return BERR_SUCCESS;
}

static BERR_Code BAPE_CustomProcessing_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_CustomProcessingHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->task);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_CustomProcessing);
    pNode->connectors[0].task = pConnection->pSource->task;
    errCode = BAPE_CustomProcessing_P_ApplyDspSettings(handle, 
                                                       handle->node.connectors[0].task, 
                                                       handle->node.connectors[0].branchId, 
                                                       handle->node.connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;    
}

static BERR_Code BAPE_CustomProcessing_P_ApplyDspSettings(BAPE_CustomProcessingHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;

    if ( NULL == handle->pSettings )
    {
        /* NULL means just use the defaults */
        return BERR_SUCCESS;
    }

    errCode = BDSP_Task_SetStageSettings(task, branchId, stageId, handle->pSettings, handle->settings.algorithmSettingsSize);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_CustomProcessing_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    /* Invalidate task handle */
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BSTD_UNUSED(pConnection);
    pNode->connectors[0].task = NULL;
}

static void BAPE_CustomProcessing_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector)
{
    (void)BAPE_CustomProcessing_RemoveInput(pNode->pHandle, pConnector);
}

