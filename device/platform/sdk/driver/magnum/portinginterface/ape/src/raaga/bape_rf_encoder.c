/***************************************************************************
*     (c)2004-2010 Broadcom Corporation
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
* $brcm_Workfile: bape_rf_encoder.c $
* $brcm_Revision: Hydra_Software_Devel/7 $
* $brcm_Date: 6/14/12 2:54p $
*
* API Description:
*   API name: RfEncoder
*    Specific APIs related to RF Audio Encoding
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/7422/bape_rf_encoder.c $
* 
* Hydra_Software_Devel/7   6/14/12 2:54p jgarrett
* SW7425-3248: Adding mono option
* 
* Hydra_Software_Devel/6   5/16/12 5:06p jgarrett
* SW7425-2075: Ensuring output format is always PcmRf
* 
* Hydra_Software_Devel/5   5/14/12 3:03p jgarrett
* SW7425-724: Changing units for attenuation variable
* 
* Hydra_Software_Devel/4   5/3/12 5:06p jgarrett
* SW7425-2075: Merge to main branch
* 
* Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:53p jgarrett
* SW7425-2075: Adding new FMT interfaces
* 
* Hydra_Software_Devel/3   8/29/11 4:24p jgarrett
* SW7425-724: Changing SFIFO configuration to mono 32-bit instead of
* stereo 16-bit
* 
* Hydra_Software_Devel/2   8/26/11 11:41a jgarrett
* SW7425-742: Incorporating API review feedback
* 
* Hydra_Software_Devel/1   8/24/11 11:53a jgarrett
* SW7425-724: Adding RF Audio Encoder
* 
***************************************************************************/

#include "bape.h"
#include "bape_priv.h"
#include "bdsp_raaga.h"

BDBG_MODULE(bape_rf_encoder);

BDBG_OBJECT_ID(BAPE_RfEncoder);
typedef struct BAPE_RfEncoder
{
    BDBG_OBJECT(BAPE_RfEncoder)
    BAPE_PathNode node;    
    BAPE_RfEncoderSettings  settings;
    BAPE_BtscEncodeSettings btscSettings;
    BAPE_Connector input;
} BAPE_RfEncoder;

static const char *BAPE_RfEncoder_P_GetName(BAPE_RfAudioEncoding encoding);
static BDSP_AudioProcessing BAPE_RfEncoder_P_GetType(BAPE_RfAudioEncoding encoding);
static BERR_Code BAPE_RfEncoder_P_InputFormatChange(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, const BAPE_FMT_Descriptor *pNewFormat);
static BERR_Code BAPE_RfEncoder_P_ApplyDspSettings(BAPE_RfEncoderHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId);
static BERR_Code BAPE_RfEncoder_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_RfEncoder_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_RfEncoder_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_RfEncoder_P_GetDefaultEncodingSettings(BAPE_RfEncoderHandle handle);
static void BAPE_RfEncoder_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);
static void BAPE_RfEncoder_P_InputSampleRateChange_isr(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, unsigned newSampleRate);
static BERR_Code BAPE_RfEncoder_P_ConfigPathToOutput(BAPE_PathNode *pNode, BAPE_PathConnection *pConnection);

/***************************************************************************
Summary:
    Get default settings for an Audio Encoder stage
***************************************************************************/
void BAPE_RfEncoder_GetDefaultSettings(
    BAPE_RfEncoderSettings *pSettings   /* [out] default settings */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    pSettings->encoding = BAPE_RfAudioEncoding_eBtsc;
}

/***************************************************************************
Summary:
    Open an Audio Encoder stage
***************************************************************************/
BERR_Code BAPE_RfEncoder_Create(
    BAPE_Handle deviceHandle,
    const BAPE_RfEncoderSettings *pSettings,
    BAPE_RfEncoderHandle *pHandle             /* [out] */
    )
{
    BAPE_RfEncoderHandle handle;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pHandle);

    handle = BKNI_Malloc(sizeof(BAPE_RfEncoder));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(handle, 0, sizeof(BAPE_RfEncoder));
    BDBG_OBJECT_SET(handle, BAPE_RfEncoder);
    BAPE_P_InitPathNode(&handle->node, BAPE_PathNodeType_eRfEncoder, pSettings->encoding, 1, deviceHandle, handle);
    handle->node.pName = BAPE_RfEncoder_P_GetName(pSettings->encoding);
    handle->node.connectors[0].useBufferPool = true;    

    BAPE_Connector_P_GetFormat(&handle->node.connectors[0], &format);
    format.source = BAPE_DataSource_eDspBuffer;
    format.type = BAPE_DataType_ePcmRf;
    BAPE_FMT_P_SetRfAudioEncoding(&format, BAPE_RfAudioEncoding_eBtsc);   /* We only support BTSC currently */
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[0], &format);
    if ( errCode ) { errCode = BERR_TRACE(errCode); goto err_format; }

    BAPE_PathNode_P_GetInputCapabilities(&handle->node, &caps);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eDspBuffer);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcmStereo);
    errCode = BAPE_PathNode_P_SetInputCapabilities(&handle->node, &caps);
    if ( errCode ) { errCode = BERR_TRACE(errCode); goto err_caps; }


    /* Generic Routines */
    handle->node.allocatePathToOutput = BAPE_DSP_P_AllocatePathToOutput;
    handle->node.configPathToOutput = BAPE_RfEncoder_P_ConfigPathToOutput;
    handle->node.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;
    handle->node.startPathToOutput = BAPE_DSP_P_StartPathToOutput;
    handle->node.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;

    /* RF Encoder Specifics */
    handle->node.inputFormatChange = BAPE_RfEncoder_P_InputFormatChange;
    handle->node.allocatePathFromInput = BAPE_RfEncoder_P_AllocatePathFromInput;
    handle->node.configPathFromInput = BAPE_RfEncoder_P_ConfigPathFromInput;
    handle->node.stopPathFromInput = BAPE_RfEncoder_P_StopPathFromInput;
    handle->node.removeInput = BAPE_RfEncoder_P_RemoveInputCallback;
    handle->node.inputSampleRateChange_isr = BAPE_RfEncoder_P_InputSampleRateChange_isr;

    /* Init encoding settings */
    BKNI_Memcpy(&handle->settings, pSettings, sizeof(BAPE_RfEncoderSettings));
    BAPE_RfEncoder_P_GetDefaultEncodingSettings(handle);

    *pHandle = handle;
    return BERR_SUCCESS;

err_caps:
err_format:
    BAPE_RfEncoder_Destroy(handle);
    return errCode;
}

/***************************************************************************
Summary:
    Close an RF Audio Encoder stage
    
Description:
    Input to the stage must be removed prior to closing.
***************************************************************************/
void BAPE_RfEncoder_Destroy(
    BAPE_RfEncoderHandle handle
    )
{
    bool running;
    BDBG_OBJECT_ASSERT(handle, BAPE_RfEncoder);
    running = (handle->node.connectors[0].task != NULL)?true:false;
    BDBG_ASSERT(false == running);
    BDBG_ASSERT(NULL == handle->input);
    BDBG_OBJECT_DESTROY(handle, BAPE_RfEncoder);
    BKNI_Free(handle);
}


/***************************************************************************
Summary:
    Get Settings for an Audio Encoder stage
***************************************************************************/
void BAPE_RfEncoder_GetSettings(
    BAPE_RfEncoderHandle handle,
    BAPE_RfEncoderSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_RfEncoder);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}


/***************************************************************************
Summary:
    Set Settings for an Audio Encoder stage
***************************************************************************/
BERR_Code BAPE_RfEncoder_SetSettings(
    BAPE_RfEncoderHandle handle,
    const BAPE_RfEncoderSettings *pSettings
    )
{
    bool running;
    BDBG_OBJECT_ASSERT(handle, BAPE_RfEncoder);
    BDBG_ASSERT(NULL != pSettings);
    running = (handle->node.connectors[0].task != NULL)?true:false;
    if ( running && pSettings->encoding != handle->settings.encoding )
    {
        BDBG_ERR(("Can not change encoding while running."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    handle->settings = *pSettings;
    handle->node.subtype = pSettings->encoding;
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Get Encoding-Specific Settings for an Audio Encoder stage
***************************************************************************/
void BAPE_RfEncoder_GetEncodingSettings(
    BAPE_RfEncoderHandle handle,
    BAPE_RfAudioEncoding encoding,               /* The encoding for which you are retrieving settings. */
    BAPE_RfEncoderEncodingSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_RfEncoder);
    BDBG_ASSERT(NULL != pSettings);
    
    pSettings->encoding = encoding;
    switch ( encoding )
    {
    case BAPE_RfAudioEncoding_eBtsc:
        pSettings->encodingSettings.btsc = handle->btscSettings;
        break;
    default:
        break;
    }
}

/***************************************************************************
Summary:
    Set Encoding-Specific Settings for an Audio Encoder stage
***************************************************************************/
BERR_Code BAPE_RfEncoder_SetEncodingSettings(
    BAPE_RfEncoderHandle handle,
    const BAPE_RfEncoderEncodingSettings *pSettings
    )
{
    bool running, encodingsEqual = false;
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_RfEncoder);
    BDBG_ASSERT(NULL != pSettings);
    running = (handle->node.connectors[0].task != NULL)?true:false;

    if ( pSettings->encoding == handle->settings.encoding )
    {
        encodingsEqual = true;
    }
    switch ( pSettings->encoding )
    {
    case BAPE_RfAudioEncoding_eBtsc:
        handle->btscSettings = pSettings->encodingSettings.btsc;
        break;
    default:
        BDBG_ERR(("Unsupported RF audio encoding %u", pSettings->encoding));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
        break;
    }

    if ( running && encodingsEqual )
    {
        errCode = BAPE_RfEncoder_P_ApplyDspSettings(handle, 
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

/***************************************************************************
Summary:
    Get the audio connector for an Audio Encoder stage
***************************************************************************/
void BAPE_RfEncoder_GetConnector(
    BAPE_RfEncoderHandle handle,
    BAPE_Connector *pConnector   /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_RfEncoder);
    BDBG_ASSERT(NULL != pConnector);
    *pConnector = &handle->node.connectors[0];
}

/***************************************************************************
Summary:
Add an input to this processing stage
***************************************************************************/
BERR_Code BAPE_RfEncoder_AddInput(
    BAPE_RfEncoderHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_RfEncoder);
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

/***************************************************************************
Summary:
Remove an input from this processing stage
***************************************************************************/
BERR_Code BAPE_RfEncoder_RemoveInput(
    BAPE_RfEncoderHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_RfEncoder);
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

/***************************************************************************
Summary:
Remove all inputs from this processing stage
***************************************************************************/
BERR_Code BAPE_RfEncoder_RemoveAllInputs(
    BAPE_RfEncoderHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_RfEncoder);
    if ( handle->input )
    {
        return BAPE_RfEncoder_RemoveInput(handle, handle->input);
    }
    return BERR_SUCCESS;
}

static const char *BAPE_RfEncoder_P_GetName(BAPE_RfAudioEncoding encoding)
{
    switch ( encoding )
    {
    case BAPE_RfAudioEncoding_eBtsc:
        return "BTSC";
    default:
        return "Unknown";
    }
}

static BDSP_AudioProcessing BAPE_RfEncoder_P_GetType(BAPE_RfAudioEncoding encoding)
{
    switch ( encoding )
    {
    case BAPE_RfAudioEncoding_eBtsc:
        return BDSP_AudioProcessing_eBtscEncoder;
    default:
        return BDSP_AudioProcessing_eMax;
    }
}

static BERR_Code BAPE_RfEncoder_P_InputFormatChange(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, const BAPE_FMT_Descriptor *pNewFormat)
{
    BAPE_FMT_Descriptor outputFormat;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pNewFormat);

    /* This module will convert the input to RF.  All other fields are passthrough. */
    outputFormat = *pNewFormat;
    outputFormat.type = BAPE_DataType_ePcmRf;
    errCode = BAPE_Connector_P_SetFormat(&pNode->connectors[0], &outputFormat);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}

static BERR_Code BAPE_RfEncoder_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_RfEncoderHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->pTaskCreateSettings);
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_RfEncoder);

    /* Add stage to CIT and propagate task settings */
    errCode = BAPE_DSP_P_AddProcessingStage(pConnection->pSource->pTaskCreateSettings, pConnection->pSource->branchId, pConnection->pSource->stageId, 
                                            BAPE_DSP_P_GetDataTypeFromConnector(handle->input),
                                            BAPE_RfEncoder_P_GetType(handle->settings.encoding),
                                            false,
                                            &pNode->connectors[0].branchId, &pNode->connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    pNode->connectors[0].pTaskCreateSettings = pConnection->pSource->pTaskCreateSettings;
    return BERR_SUCCESS;
}

static BERR_Code BAPE_RfEncoder_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_RfEncoderHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->task);
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_RfEncoder);
    pNode->connectors[0].task = pConnection->pSource->task;
    errCode = BAPE_RfEncoder_P_ApplyDspSettings(handle, 
                                                handle->node.connectors[0].task, 
                                                handle->node.connectors[0].branchId, 
                                                handle->node.connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;    
}

static void BAPE_RfEncoder_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    /* Invalidate task handle */
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BSTD_UNUSED(pConnection);
    pNode->connectors[0].task = NULL;
}

static BERR_Code BAPE_RfEncoder_P_ApplyBtscSettings(BAPE_RfEncoderHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;
    BDSP_Raaga_BtscEncoderSettings userConfig;

    errCode = BDSP_Task_GetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_DSP_P_SET_VARIABLE(userConfig, eUseDeEmphasizedSourceSignals, handle->btscSettings.useDeEmphasizedSourceSignals ? BDSP_AF_P_eEnable : BDSP_AF_P_eDisable);
    BAPE_DSP_P_SET_VARIABLE(userConfig, eEquivalenMode75MicroSec, handle->btscSettings.equivalentMode75us ? BDSP_AF_P_eEnable : BDSP_AF_P_eDisable);
    BAPE_DSP_P_SET_VARIABLE(userConfig, eOpFreqDevCtrl, handle->btscSettings.fmDeviationControl ? BDSP_AF_P_eEnable : BDSP_AF_P_eDisable);

    if ( handle->btscSettings.volume == BAPE_VOLUME_NORMAL )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, ui32AttenuationFactor, 0x20000000);
    }
    else
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, ui32AttenuationFactor, BAPE_P_FloatToQ329(handle->btscSettings.volume, BAPE_VOLUME_NORMAL));
    }
    BAPE_DSP_P_SET_VARIABLE(userConfig, ui32SRDCalibrationFactor, handle->btscSettings.srdCalibrationFactor);
    
    BAPE_DSP_P_SET_VARIABLE(userConfig, eBTSCMonoModeOn, handle->btscSettings.encodingMode == BAPE_BtscEncodingMode_eMono ? BDSP_AF_P_eEnable : BDSP_AF_P_eDisable);

    BAPE_DSP_P_SET_VARIABLE(userConfig, eMainChannelPreEmphasisOn, handle->btscSettings.mainChannel.preemphasis ? BDSP_AF_P_eEnable : BDSP_AF_P_eDisable);
    BAPE_DSP_P_SET_VARIABLE(userConfig, eMainChannelPreEmphasisOn, handle->btscSettings.mainChannel.fmDeviationControl ? BDSP_AF_P_eEnable : BDSP_AF_P_eDisable);

    BAPE_DSP_P_SET_VARIABLE(userConfig, eClipDiffChannelData, handle->btscSettings.diffChannel.clipping ? BDSP_AF_P_eEnable : BDSP_AF_P_eDisable);
    BAPE_DSP_P_SET_VARIABLE(userConfig, eDiffChannelPreEmphasisOn, handle->btscSettings.diffChannel.preemphasis ? BDSP_AF_P_eEnable : BDSP_AF_P_eDisable);
    BAPE_DSP_P_SET_VARIABLE(userConfig, eDiffChanFreqDevCtrl, handle->btscSettings.diffChannel.fmDeviationControl ? BDSP_AF_P_eEnable : BDSP_AF_P_eDisable);

    BAPE_DSP_P_SET_VARIABLE(userConfig, eFreqGeneration, handle->btscSettings.testTone.enabled ? BDSP_AF_P_eEnable : BDSP_AF_P_eDisable);
    BAPE_DSP_P_SET_VARIABLE(userConfig, i32Frequency, handle->btscSettings.testTone.frequency);
    
    errCode = BDSP_Task_SetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_RfEncoder_P_GetDefaultBtscSettings(BAPE_RfEncoderHandle handle)
{
    BDSP_Raaga_BtscEncoderSettings userConfig;

    BERR_TRACE(BDSP_Raaga_GetDefaultAudioProcessingSettings(BDSP_AudioProcessing_eBtscEncoder, &userConfig, sizeof(userConfig)));
    
    handle->btscSettings.useDeEmphasizedSourceSignals = (userConfig.eUseDeEmphasizedSourceSignals == BDSP_AF_P_eEnable) ? true : false;
    handle->btscSettings.equivalentMode75us = (userConfig.eEquivalenMode75MicroSec == BDSP_AF_P_eEnable) ? true : false;
    handle->btscSettings.fmDeviationControl = (userConfig.eOpFreqDevCtrl == BDSP_AF_P_eEnable) ? true : false;
    
    handle->btscSettings.volume = BAPE_VOLUME_NORMAL;
    BDBG_ASSERT(userConfig.ui32AttenuationFactor == 0x20000000);
    handle->btscSettings.srdCalibrationFactor = userConfig.ui32SRDCalibrationFactor;
    
    handle->btscSettings.encodingMode = userConfig.eBTSCMonoModeOn ? BAPE_BtscEncodingMode_eMono : BAPE_BtscEncodingMode_eStereo;

    handle->btscSettings.mainChannel.preemphasis = (userConfig.eMainChannelPreEmphasisOn == BDSP_AF_P_eEnable) ? true : false;
    handle->btscSettings.mainChannel.fmDeviationControl = (userConfig.eSumChanFreqDevCtrl == BDSP_AF_P_eEnable) ? true : false;

    handle->btscSettings.diffChannel.clipping = (userConfig.eClipDiffChannelData == BDSP_AF_P_eEnable) ? true : false;
    handle->btscSettings.diffChannel.preemphasis = (userConfig.eDiffChannelPreEmphasisOn == BDSP_AF_P_eEnable) ? true : false;
    handle->btscSettings.diffChannel.fmDeviationControl = (userConfig.eDiffChanFreqDevCtrl == BDSP_AF_P_eEnable) ? true : false;
    
    handle->btscSettings.testTone.enabled = (userConfig.eFreqGeneration == BDSP_AF_P_eEnable) ? true : false;
    handle->btscSettings.testTone.frequency = (unsigned)userConfig.i32Frequency;
}

static BERR_Code BAPE_RfEncoder_P_ApplyDspSettings(BAPE_RfEncoderHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BDSP_AudioProcessing encodeType;
    BDBG_OBJECT_ASSERT(handle, BAPE_RfEncoder);
    BDBG_ASSERT(NULL != task);
    encodeType = BAPE_RfEncoder_P_GetType(handle->settings.encoding);
    switch ( encodeType )
    {
    case BDSP_AudioProcessing_eBtscEncoder:
        return BAPE_RfEncoder_P_ApplyBtscSettings(handle, task, branchId, stageId);
    default:
        return BERR_SUCCESS;
    }
}

static void BAPE_RfEncoder_P_GetDefaultEncodingSettings(BAPE_RfEncoderHandle handle)
{
    BAPE_RfEncoder_P_GetDefaultBtscSettings(handle);
}

static void BAPE_RfEncoder_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector)
{
    (void)BAPE_RfEncoder_RemoveInput(pNode->pHandle, pConnector);
}

static void BAPE_RfEncoder_P_InputSampleRateChange_isr(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, unsigned newSampleRate)
{
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
        
    BDBG_MSG(("Input sample rate changed to %u Hz.  Output rate is %u Hz.", newSampleRate, 4*newSampleRate));

    BAPE_Connector_P_SetSampleRate_isr(&pNode->connectors[0], 4*newSampleRate);    
}

static BERR_Code BAPE_RfEncoder_P_ConfigPathToOutput(
    BAPE_PathNode *pNode,
    BAPE_PathConnection *pConnection
    )
{
    BAPE_PathConnector *pSource;
    BAPE_Handle deviceHandle;
    BAPE_PathNode *pSink;
    BERR_Code errCode;
    unsigned i;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_OBJECT_ASSERT(pConnection->pSource, BAPE_PathConnector);
    BDBG_OBJECT_ASSERT(pConnection->pSink, BAPE_PathNode);

    deviceHandle = pNode->deviceHandle;
    pSource = pConnection->pSource;
    pSink = pConnection->pSink;

    if ( pSink->type == BAPE_PathNodeType_eMixer && pSink->subtype == BAPE_MixerType_eDsp )
    {
        BDBG_ERR(("RF Encoder can not be connected to a DSP mixer."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    else if ( pSink->type == BAPE_PathNodeType_eEqualizer )
    {
        BDBG_ERR(("RF Encoder can not be connected to an equalizer."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    else if ( pSink->type == BAPE_PathNodeType_eMixer )
    {
        BAPE_SfifoGroupSettings sfifoSettings;
        /* Dropping into the FMM - need to config SFIFO's.  DSP does not use master/slave, it will instead reuse ringbuffers in multiple
           master SFIFOs */
        BAPE_SfifoGroup_P_GetSettings(pConnection->sfifoGroup, &sfifoSettings);
        sfifoSettings.highPriority = false; /* TODO: How to handle this on the fly? */
        sfifoSettings.reverseEndian = false;
        sfifoSettings.signedData = true;
        sfifoSettings.wrpointEnabled = true;
        sfifoSettings.stereoData = false;       /* RF Encoded data is output as mono to the SFIFO and not stereo */
        sfifoSettings.sampleRepeatEnabled = false;
        sfifoSettings.interleaveData = true;
        sfifoSettings.dataWidth = 32;
        /* Setup buffers from pool */
        for ( i = 0; i < BAPE_FMT_P_GetNumChannelPairs(&pSource->format); i++ )
        {
            unsigned bufferId = 2*i;
            BAPE_BufferNode *pBuffer = pSource->pBuffers[i];
            BDBG_ASSERT(NULL != pBuffer);
            sfifoSettings.bufferInfo[bufferId].base = pBuffer->offset;
            sfifoSettings.bufferInfo[bufferId].length = pBuffer->bufferSize;
            sfifoSettings.bufferInfo[bufferId].wrpoint = pBuffer->offset+pBuffer->bufferSize-1;
            bufferId++;
            sfifoSettings.bufferInfo[bufferId].base = 0;
            sfifoSettings.bufferInfo[bufferId].length = 0;
            sfifoSettings.bufferInfo[bufferId].wrpoint = 0;
        }

        errCode = BAPE_SfifoGroup_P_SetSettings(pConnection->sfifoGroup, &sfifoSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}
