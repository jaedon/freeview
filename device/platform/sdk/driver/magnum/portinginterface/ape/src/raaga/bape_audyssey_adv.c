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
* $brcm_Workfile: bape_audyssey_adv.c $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 5/3/12 5:06p $
*
* API Description:
*   API name: AudysseyAdv
*    Specific APIs related to Audyssey ADV Audio Processing
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/7422/bape_audyssey_adv.c $
* 
* Hydra_Software_Devel/2   5/3/12 5:06p jgarrett
* SW7425-2075: Merge to main branch
* 
* Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:53p jgarrett
* SW7425-2075: Adding new FMT interfaces
* 
* Hydra_Software_Devel/1   5/14/11 11:22a jgarrett
* SWDTV-6762: Adding Audyssey ADV/ABX
* 
***************************************************************************/

#include "bape.h"
#include "bape_priv.h"
#include "bdsp_raaga.h"

BDBG_MODULE(bape_audyssey_adv);

BDBG_OBJECT_ID(BAPE_AudysseyAdv);

typedef struct BAPE_AudysseyAdv
{
    BDBG_OBJECT(BAPE_AudysseyAdv)
    BAPE_PathNode node;
    BAPE_AudysseyAdvSettings settings;
    BAPE_Connector input;
} BAPE_AudysseyAdv;

static BERR_Code BAPE_AudysseyAdv_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_AudysseyAdv_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_AudysseyAdv_P_ApplyDspSettings(BAPE_AudysseyAdvHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId);
static void BAPE_AudysseyAdv_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_AudysseyAdv_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);

void BAPE_AudysseyAdv_GetDefaultSettings(
    BAPE_AudysseyAdvSettings *pSettings   /* [out] default settings */
    )
{
    BDSP_Raaga_Audio_AudysseyVolUserConfig dspSettings;
    unsigned i;

    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    BDSP_Raaga_GetDefaultAudioProcessingSettings(BDSP_AudioProcessing_eAudysseyVolume, (void *)&dspSettings, sizeof(dspSettings));

    pSettings->enabled = (dspSettings.ui32AudysseyVolBypass == 1) ? false : true;
    pSettings->channelMask = (uint8_t)dspSettings.i32ChannelMask;
    pSettings->reInitialize = 0;
    pSettings->volumeScalingEnabled = dspSettings.i32SwAudysseyVol;
    pSettings->dynamicSurroundGainEnabled = dspSettings.i32SwDynSurrGain;
    pSettings->hiLoCompress = dspSettings.i32SwHiLoCmpress;
    pSettings->dynamicEqMode = (BAPE_AudysseyAdvDynamicEqMode)dspSettings.i32SwDynEQ;
    pSettings->volume = 0;
    BDBG_ASSERT(0 == dspSettings.i32dBVolSetting);
    pSettings->gainCompression = dspSettings.i32GCF;
    pSettings->headroomOffset = 0;
    BDBG_ASSERT(0 == dspSettings.i32HeadroomOffset);
    for ( i = 0; i < 8; i++ )
    {
        pSettings->chCalbGain[i] = 0;
        BDBG_ASSERT(0==dspSettings.i32chCalbGain[i]);
    }
}

/***************************************************************************
Summary:
    Open an SRS AudysseyAdv stage
***************************************************************************/
BERR_Code BAPE_AudysseyAdv_Create(
    BAPE_Handle deviceHandle,
    const BAPE_AudysseyAdvSettings *pSettings,
    BAPE_AudysseyAdvHandle *pHandle
    )
{
    BAPE_AudysseyAdvHandle handle;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pHandle);

    handle = BKNI_Malloc(sizeof(BAPE_AudysseyAdv));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(handle, 0, sizeof(BAPE_AudysseyAdv));
    BDBG_OBJECT_SET(handle, BAPE_AudysseyAdv);
    handle->settings = *pSettings;
    BAPE_P_InitPathNode(&handle->node, BAPE_PathNodeType_ePostProcessor, BAPE_PostProcessorType_eAudysseyAdv, 1, deviceHandle, handle);
    handle->node.pName = "AVL";

    /* Output format is fixed to PCM stereo */
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

    /* AudysseyAdv Specifics */
    handle->node.allocatePathFromInput = BAPE_AudysseyAdv_P_AllocatePathFromInput;
    handle->node.configPathFromInput = BAPE_AudysseyAdv_P_ConfigPathFromInput;
    handle->node.stopPathFromInput = BAPE_AudysseyAdv_P_StopPathFromInput;
    handle->node.removeInput = BAPE_AudysseyAdv_P_RemoveInputCallback;

    *pHandle = handle;
    return BERR_SUCCESS;

err_set_caps:
err_set_format:
    BAPE_AudysseyAdv_Destroy(handle);
    return errCode;    
}

void BAPE_AudysseyAdv_Destroy(
    BAPE_AudysseyAdvHandle handle
    )
{
    bool running;
    BDBG_OBJECT_ASSERT(handle, BAPE_AudysseyAdv);
    running = (handle->node.connectors[0].task != NULL)?true:false;
    BDBG_ASSERT(false == running);
    BDBG_ASSERT(NULL == handle->input);
    BDBG_OBJECT_DESTROY(handle, BAPE_AudysseyAdv);
    BKNI_Free(handle);
}

void BAPE_AudysseyAdv_GetSettings(
    BAPE_AudysseyAdvHandle handle,
    BAPE_AudysseyAdvSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_AudysseyAdv);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}

BERR_Code BAPE_AudysseyAdv_SetSettings(
    BAPE_AudysseyAdvHandle handle,
    const BAPE_AudysseyAdvSettings *pSettings
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_AudysseyAdv);
    BDBG_ASSERT(NULL != pSettings);

    if ( pSettings != &handle->settings )
    {
        handle->settings = *pSettings;
    }

    if ( handle->node.connectors[0].task != NULL )
    {
        errCode = BAPE_AudysseyAdv_P_ApplyDspSettings(handle, 
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


void BAPE_AudysseyAdv_GetConnector(
    BAPE_AudysseyAdvHandle handle,
    BAPE_Connector *pConnector
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_AudysseyAdv);
    BDBG_ASSERT(NULL != pConnector);
    *pConnector = &handle->node.connectors[0];
}


BERR_Code BAPE_AudysseyAdv_AddInput(
    BAPE_AudysseyAdvHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_AudysseyAdv);
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


BERR_Code BAPE_AudysseyAdv_RemoveInput(
    BAPE_AudysseyAdvHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_AudysseyAdv);
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


BERR_Code BAPE_AudysseyAdv_RemoveAllInputs(
    BAPE_AudysseyAdvHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_AudysseyAdv);
    if ( handle->input )
    {
        return BAPE_AudysseyAdv_RemoveInput(handle, handle->input);
    }
    return BERR_SUCCESS;
}

static BERR_Code BAPE_AudysseyAdv_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_AudysseyAdvHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->pTaskCreateSettings);
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_AudysseyAdv);
    /* Add stage to CIT and propagate task settings */
    errCode = BAPE_DSP_P_AddProcessingStage(pConnection->pSource->pTaskCreateSettings, pConnection->pSource->branchId, pConnection->pSource->stageId, 
                                            BAPE_DSP_P_GetDataTypeFromConnector(handle->input), 
                                            BDSP_AudioProcessing_eAudysseyVolume,
                                            false,
                                            &pNode->connectors[0].branchId, &pNode->connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    pNode->connectors[0].pTaskCreateSettings = pConnection->pSource->pTaskCreateSettings;
    return BERR_SUCCESS;
}

static BERR_Code BAPE_AudysseyAdv_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_AudysseyAdvHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->task);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_AudysseyAdv);
    pNode->connectors[0].task = pConnection->pSource->task;
    errCode = BAPE_AudysseyAdv_P_ApplyDspSettings(handle, 
                                                      handle->node.connectors[0].task, 
                                                      handle->node.connectors[0].branchId, 
                                                      handle->node.connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;    
}

static BERR_Code BAPE_AudysseyAdv_P_ApplyDspSettings(BAPE_AudysseyAdvHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;
    BDSP_Raaga_Audio_AudysseyVolUserConfig userConfig;
    unsigned i;

    errCode = BDSP_Task_GetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_DSP_P_SET_VARIABLE(userConfig, ui32AudysseyVolBypass, handle->settings.enabled?0:1);
    BAPE_DSP_P_SET_VARIABLE(userConfig, ui32NumChannels, 2);    /* Stereo only */
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32ChannelMask, handle->settings.channelMask);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32AudysseyVolInit, handle->settings.reInitialize);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32SwAudysseyVol, handle->settings.volumeScalingEnabled?1:0);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32SwDynSurrGain, handle->settings.dynamicSurroundGainEnabled?1:0);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32SwHiLoCmpress, handle->settings.hiLoCompress?1:0);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32SwDynEQ, (int32_t)handle->settings.dynamicEqMode);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32dBVolSetting, BAPE_P_FloatToQ1022(handle->settings.volume, 10));
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32GCF, handle->settings.gainCompression);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32dBVolSetting, BAPE_P_FloatToQ1022(handle->settings.headroomOffset, 100));
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32dBVolSetting, BAPE_P_FloatToQ1022(handle->settings.headroomOffset, 100));
    for ( i = 0; i < 8; i++ )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, i32chCalbGain[i], BAPE_P_FloatToQ1022(handle->settings.chCalbGain[i], 100));
    }

    errCode = BDSP_Task_SetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_AudysseyAdv_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    /* Invalidate task handle */
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BSTD_UNUSED(pConnection);
    pNode->connectors[0].task = NULL;
}

static void BAPE_AudysseyAdv_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector)
{
    (void)BAPE_AudysseyAdv_RemoveInput(pNode->pHandle, pConnector);
}

BERR_Code BAPE_AudysseyAdv_GetStatus(
    BAPE_AudysseyAdvHandle handle,
    BAPE_AudysseyAdvStatus *pStatus    /* [out] Status */
    )
{
    BDSP_Raaga_Audio_AudysseyVolStatusInfo statusInfo;
    unsigned i;
    BERR_Code errCode;

    if ( NULL == handle->node.connectors[0].task )
    {
        BDBG_ERR(("Not started, status not available"));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    errCode = BDSP_Task_GetStageStatus(handle->node.connectors[0].task,
                                       handle->node.connectors[0].branchId,
                                       handle->node.connectors[0].stageId,
                                       &statusInfo, sizeof(statusInfo));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    pStatus->volumeApplied = statusInfo.i32Vol_Applied_dB;
    pStatus->numChannels = statusInfo.ui32NumChannels;
    pStatus->dynamicEqMode = statusInfo.i32SwDynEQ;
    for ( i = 0; i < 8; i++ )
    {
        pStatus->chCalcLevel[i] = statusInfo.i32chCalcLevel[i];
    }

    return BERR_SUCCESS;
}

