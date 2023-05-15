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
* $brcm_Workfile: bape_auto_volume_level.c $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 5/3/12 5:06p $
*
* API Description:
*   API name: AutoVolumeLevel
*    Specific APIs related to Broadcom Auto Volume Levelling (AVL) processing
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/7422/bape_auto_volume_level.c $
* 
* Hydra_Software_Devel/4   5/3/12 5:06p jgarrett
* SW7425-2075: Merge to main branch
* 
* Hydra_Software_Devel/SW7425-2075/2   5/3/12 3:46p jgarrett
* SW7425-2075: Merging latest mainline changes
* 
* Hydra_Software_Devel/3   4/19/12 11:59a jgarrett
* SW7425-2905: Adding AVL 2.0
* 
* Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:53p jgarrett
* SW7425-2075: Adding new FMT interfaces
* 
* Hydra_Software_Devel/2   5/26/11 4:13p jgarrett
* SW7425-588: Using defaults from BDSP
* 
* Hydra_Software_Devel/1   5/14/11 9:53a jgarrett
* SW7425-410: Adding AVL
* 
***************************************************************************/

#include "bape.h"
#include "bape_priv.h"
#include "bdsp_raaga.h"

BDBG_MODULE(bape_auto_volume_level);

BDBG_OBJECT_ID(BAPE_AutoVolumeLevel);

typedef struct BAPE_AutoVolumeLevel
{
    BDBG_OBJECT(BAPE_AutoVolumeLevel)
    BAPE_PathNode node;
    BAPE_AutoVolumeLevelSettings settings;
    BAPE_Connector input;
} BAPE_AutoVolumeLevel;

static BERR_Code BAPE_AutoVolumeLevel_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_AutoVolumeLevel_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_AutoVolumeLevel_P_ApplyDspSettings(BAPE_AutoVolumeLevelHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId);
static void BAPE_AutoVolumeLevel_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_AutoVolumeLevel_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);

void BAPE_AutoVolumeLevel_GetDefaultSettings(
    BAPE_AutoVolumeLevelSettings *pSettings   /* [out] default settings */
    )
{
    BDSP_Raaga_Audio_AVLConfigParams dspSettings;

    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    BDSP_Raaga_GetDefaultAudioProcessingSettings(BDSP_AudioProcessing_eAVL, (void *)&dspSettings, sizeof(dspSettings));

    pSettings->enabled = dspSettings.ui32AVLEnableFlag?true:false;    
    pSettings->loudnessEqualization = dspSettings.i32LoudnessLevelEq ? true : false;   
    pSettings->target = (dspSettings.i32Target * 100)/32768;
    pSettings->lowerBound = (dspSettings.i32LowerBound * 100)/32768;
    pSettings->fixedBoost = (dspSettings.i32FixedBoost * 100)/32768;
    pSettings->ref = dspSettings.i32RefLevel;
    pSettings->alpha = dspSettings.i32Alpha;
    pSettings->beta = dspSettings.i32Beta;
    pSettings->threshold = 3; /*(dspSettings.i32ActiveThreshold * 10)/32768; Not enough precision to work this backward. */
    BDBG_ASSERT(dspSettings.i32ActiveThreshold==9830);
    pSettings->dtfPercent = dspSettings.i32DTFPCNT;
    pSettings->alpha2 = dspSettings.i32Alpha2;
    pSettings->fastGainRamp = 5; /*(dspSettings.i32NSFGR_SEC * 10)/32768; Not enough precision to work this backward. */
    BDBG_ASSERT(dspSettings.i32NSFGR_SEC==16384);
    pSettings->dtf = 3; /*(dspSettings.i32DTF * 10)/32768; Not enough precision to work this backward */
    BDBG_ASSERT(dspSettings.i32DTF==9830);
}

/***************************************************************************
Summary:
    Open an SRS AutoVolumeLevel stage
***************************************************************************/
BERR_Code BAPE_AutoVolumeLevel_Create(
                               BAPE_Handle deviceHandle,
                               const BAPE_AutoVolumeLevelSettings *pSettings,
                               BAPE_AutoVolumeLevelHandle *pHandle
                               )
{
    BAPE_AutoVolumeLevelHandle handle;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pHandle);


    handle = BKNI_Malloc(sizeof(BAPE_AutoVolumeLevel));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(handle, 0, sizeof(BAPE_AutoVolumeLevel));
    BDBG_OBJECT_SET(handle, BAPE_AutoVolumeLevel);
    handle->settings = *pSettings;
    BAPE_P_InitPathNode(&handle->node, BAPE_PathNodeType_ePostProcessor, BAPE_PostProcessorType_eAutoVolumeLevel, 1, deviceHandle, handle);
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

    /* AutoVolumeLevel Specifics */
    handle->node.allocatePathFromInput = BAPE_AutoVolumeLevel_P_AllocatePathFromInput;
    handle->node.configPathFromInput = BAPE_AutoVolumeLevel_P_ConfigPathFromInput;
    handle->node.stopPathFromInput = BAPE_AutoVolumeLevel_P_StopPathFromInput;
    handle->node.removeInput = BAPE_AutoVolumeLevel_P_RemoveInputCallback;

    *pHandle = handle;
    return BERR_SUCCESS;

err_set_caps:
err_set_format:
    BAPE_AutoVolumeLevel_Destroy(handle);
    return errCode;    
}

void BAPE_AutoVolumeLevel_Destroy(
    BAPE_AutoVolumeLevelHandle handle
    )
{
    bool running;
    BDBG_OBJECT_ASSERT(handle, BAPE_AutoVolumeLevel);
    running = (handle->node.connectors[0].task != NULL)?true:false;
    BDBG_ASSERT(false == running);
    BDBG_ASSERT(NULL == handle->input);
    BDBG_OBJECT_DESTROY(handle, BAPE_AutoVolumeLevel);
    BKNI_Free(handle);
}

void BAPE_AutoVolumeLevel_GetSettings(
    BAPE_AutoVolumeLevelHandle handle,
    BAPE_AutoVolumeLevelSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_AutoVolumeLevel);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}

BERR_Code BAPE_AutoVolumeLevel_SetSettings(
    BAPE_AutoVolumeLevelHandle handle,
    const BAPE_AutoVolumeLevelSettings *pSettings
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_AutoVolumeLevel);
    BDBG_ASSERT(NULL != pSettings);

    if ( pSettings != &handle->settings )
    {
        handle->settings = *pSettings;
    }

    if ( handle->node.connectors[0].task != NULL )
    {
        errCode = BAPE_AutoVolumeLevel_P_ApplyDspSettings(handle, 
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


void BAPE_AutoVolumeLevel_GetConnector(
    BAPE_AutoVolumeLevelHandle handle,
    BAPE_Connector *pConnector
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_AutoVolumeLevel);
    BDBG_ASSERT(NULL != pConnector);
    *pConnector = &handle->node.connectors[0];
}


BERR_Code BAPE_AutoVolumeLevel_AddInput(
    BAPE_AutoVolumeLevelHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_AutoVolumeLevel);
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


BERR_Code BAPE_AutoVolumeLevel_RemoveInput(
    BAPE_AutoVolumeLevelHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_AutoVolumeLevel);
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


BERR_Code BAPE_AutoVolumeLevel_RemoveAllInputs(
    BAPE_AutoVolumeLevelHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_AutoVolumeLevel);
    if ( handle->input )
    {
        return BAPE_AutoVolumeLevel_RemoveInput(handle, handle->input);
    }
    return BERR_SUCCESS;
}

static BERR_Code BAPE_AutoVolumeLevel_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_AutoVolumeLevelHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->pTaskCreateSettings);
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_AutoVolumeLevel);
    /* Add stage to CIT and propagate task settings */
    errCode = BAPE_DSP_P_AddProcessingStage(pConnection->pSource->pTaskCreateSettings, pConnection->pSource->branchId, pConnection->pSource->stageId, 
                                            BAPE_DSP_P_GetDataTypeFromConnector(handle->input), 
                                            BDSP_AudioProcessing_eAVL,
                                            false,
                                            &pNode->connectors[0].branchId, &pNode->connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    pNode->connectors[0].pTaskCreateSettings = pConnection->pSource->pTaskCreateSettings;
    return BERR_SUCCESS;
}

static BERR_Code BAPE_AutoVolumeLevel_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_AutoVolumeLevelHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->task);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_AutoVolumeLevel);
    pNode->connectors[0].task = pConnection->pSource->task;
    errCode = BAPE_AutoVolumeLevel_P_ApplyDspSettings(handle, 
                                                      handle->node.connectors[0].task, 
                                                      handle->node.connectors[0].branchId, 
                                                      handle->node.connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;    
}

static BERR_Code BAPE_AutoVolumeLevel_P_ApplyDspSettings(BAPE_AutoVolumeLevelHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;
    BDSP_Raaga_Audio_AVLConfigParams userConfig;

    errCode = BDSP_Task_GetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_DSP_P_SET_VARIABLE(userConfig, ui32AVLEnableFlag, handle->settings.enabled);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32LoudnessLevelEq, handle->settings.loudnessEqualization);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32Target, (handle->settings.target*32768)/100);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32LowerBound, (handle->settings.lowerBound*32768)/100);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32FixedBoost, (handle->settings.fixedBoost*32768)/100);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32RefLevel, handle->settings.ref);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32Alpha, handle->settings.alpha);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32Beta, handle->settings.beta);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32ActiveThreshold, (handle->settings.threshold*32768)/10);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32DTFPCNT, handle->settings.dtfPercent);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32Alpha2, handle->settings.alpha2);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32NSFGR_SEC, (handle->settings.fastGainRamp*32768)/10);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32DTF, (handle->settings.dtf*32768)/10);

    errCode = BDSP_Task_SetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_AutoVolumeLevel_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    /* Invalidate task handle */
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BSTD_UNUSED(pConnection);
    pNode->connectors[0].task = NULL;
}

static void BAPE_AutoVolumeLevel_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector)
{
    (void)BAPE_AutoVolumeLevel_RemoveInput(pNode->pHandle, pConnector);
}


