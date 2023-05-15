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
* $brcm_Workfile: bape_dolby_digital_reencode.c $
* $brcm_Revision: Hydra_Software_Devel/16 $
* $brcm_Date: 10/9/12 4:37a $
*
* API Description:
*   API name: DolbyDigitalReencode
*    Specific APIs related to Dolby Digital Reencoding used in Dolby MS11
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_dolby_digital_reencode.c $
* 
* Hydra_Software_Devel/16   10/9/12 4:37a jgarrett
* SW7425-4042: Adding additional external pcm mode settings
* 
* Hydra_Software_Devel/15   7/12/12 11:20a jgarrett
* SW7425-3476: Allowing DDRE to run in non-externalPcmMode for PCM-WAV
* 
* Hydra_Software_Devel/14   5/3/12 5:06p jgarrett
* SW7425-2075: Merge to main branch
* 
* Hydra_Software_Devel/SW7425-2075/2   5/3/12 3:45p jgarrett
* SW7425-2075: Merging latest mainline changes
* 
* Hydra_Software_Devel/13   4/10/12 10:57a jgarrett
* SW7231-744: Forcing external PCM mode if input is stereo
* 
* Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:51p jgarrett
* SW7425-2075: Adding new FMT interfaces
* 
* Hydra_Software_Devel/12   1/31/12 6:17p jgarrett
* SW7425-2268: Adding initial voice conferencing support
* 
* Hydra_Software_Devel/11   12/20/11 5:25p jgarrett
* SW7425-1018: Switching to RF mode for all PCM outputs
* 
* Hydra_Software_Devel/10   12/19/11 4:27p jgarrett
* SW7425-1018: Adding initial A/85 implementation
* 
* Hydra_Software_Devel/9   6/9/11 3:42p jgarrett
* SW7425-406: Cleaning up external PCM mode setting and not treating DD
* Transcode as a multichannel consumer
* 
* Hydra_Software_Devel/8   5/28/11 7:03a jgarrett
* SW7425-408: Coverity CID 420
* 
* Hydra_Software_Devel/7   5/27/11 4:56p jgarrett
* SW7425-408: Propagating decoder settings for AAC/AC3 to DDRE in MS11
* usage modes
* 
* Hydra_Software_Devel/6   5/27/11 12:28p jgarrett
* SW7425-408: Fixing DDRE output rate to 48k
* 
* Hydra_Software_Devel/5   5/25/11 6:16p jgarrett
* SW7425-408: Merge to main branch
* 
* Hydra_Software_Devel/SW7425-408/2   5/25/11 5:14p jgarrett
* SW7425-408: Fixed CIT and userconfig errors
* 
* Hydra_Software_Devel/SW7425-408/1   5/19/11 6:10p jgarrett
* SW7425-408: Moving MS11 to a branch
* 
* Hydra_Software_Devel/3   5/18/11 6:50p jgarrett
* SW7425-408: Adding preliminary DDRE support
* 
* Hydra_Software_Devel/2   2/22/11 5:44p jgarrett
* SW7422-146: Implemented type renaming based on filter graph review
* comments
* 
* Hydra_Software_Devel/1   1/12/11 4:24p jgarrett
* SW7422-146: Adding additional APIs
* 
***************************************************************************/

#include "bape.h"
#include "bape_priv.h"
#include "bdsp_raaga.h"

BDBG_MODULE(bape_ddre);

BDBG_OBJECT_ID(BAPE_DolbyDigitalReencode);

typedef struct BAPE_DolbyDigitalReencode
{
    BDBG_OBJECT(BAPE_DolbyDigitalReencode)
    BAPE_PathNode node;
    BAPE_DolbyDigitalReencodeSettings settings;
    BAPE_PathNodeOutputStatus outputStatus;
    BAPE_Connector input;
    BAPE_DecoderHandle masterDecoder;
} BAPE_DolbyDigitalReencode;

static BERR_Code BAPE_DolbyDigitalReencode_P_InputFormatChange(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, const BAPE_FMT_Descriptor *pNewFormat);
static BERR_Code BAPE_DolbyDigitalReencode_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_DolbyDigitalReencode_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_DolbyDigitalReencode_P_ApplyDspSettings(BAPE_DolbyDigitalReencodeHandle handle, BDSP_TaskHandle task, unsigned ddreBranchId, unsigned ddreStageId, unsigned encBranchId, unsigned encStageId);
static void BAPE_DolbyDigitalReencode_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_DolbyDigitalReencode_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);
static void BAPE_DolbyDigitalReencode_P_InputSampleRateChange_isr(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, unsigned newSampleRate);

static bool BAPE_DolbyDigitalReencode_P_HasCompressedOutput(BAPE_DolbyDigitalReencodeHandle handle)
{
    return (handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eCompressed].directConnections) > 0 ? true : false;
}

static bool BAPE_DolbyDigitalReencode_P_HasMultichannelOutput(BAPE_DolbyDigitalReencodeHandle handle)
{
    return (handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMultichannel].directConnections) > 0 ? true : false;
}

static bool BAPE_DolbyDigitalReencode_P_HasStereoOutput(BAPE_DolbyDigitalReencodeHandle handle)
{
    return handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eStereo].directConnections > 0 ? true : false;
}

void BAPE_DolbyDigitalReencode_GetDefaultSettings(
    BAPE_DolbyDigitalReencodeSettings *pSettings   /* [out] default settings */
    )
{
    BDSP_Raaga_Audio_DDReencodeConfigParams ddreSettings;
    BDSP_Raaga_Audio_DDTranscodeConfigParams encodeSettings;

    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    BDSP_Raaga_GetDefaultAudioProcessingSettings(BDSP_AudioProcessing_eDdre, (void *)&ddreSettings, sizeof(ddreSettings));
    pSettings->loudnessEquivalenceEnabled = true;
    pSettings->externalPcmMode = (ddreSettings.ui32ExternalPcmEnabled)?true:false;
    pSettings->profile = (BAPE_DolbyDigitalReencodeProfile)ddreSettings.ui32CompProfile;
    pSettings->centerMixLevel = (BAPE_Ac3CenterMixLevel)ddreSettings.ui32CmixLev;
    pSettings->surroundMixLevel = (BAPE_Ac3SurroundMixLevel)ddreSettings.ui32SurmixLev;
    pSettings->dolbySurround = (BAPE_Ac3DolbySurround)ddreSettings.ui32DsurMod;
    pSettings->dialogLevel = ddreSettings.ui32DialNorm;
    pSettings->drcMode = BAPE_DolbyDigitalReencodeDrcMode_eLine;
    pSettings->drcModeDownmix = BAPE_DolbyDigitalReencodeDrcMode_eLine;
    pSettings->drcScaleHi = 100;
    pSettings->drcScaleLow = 100;
    pSettings->drcScaleHiDownmix = 100;
    pSettings->drcScaleLowDownmix = 100;
    pSettings->stereoMode = BAPE_DolbyDigitalReencodeStereoMode_eLtRt;
    pSettings->dualMonoMode = BAPE_DualMonoMode_eStereo;

    BDSP_Raaga_GetDefaultAudioEncodeSettings(BDSP_AudioEncode_eAc3, (void *)&encodeSettings, sizeof(encodeSettings));
    pSettings->encodeSettings.certificationMode = (encodeSettings.eTranscodeEnable)?false:true;
    pSettings->encodeSettings.spdifHeaderEnabled = (encodeSettings.eSpdifHeaderEnable)?true:false;
}

/***************************************************************************
Summary:
    Open an SRS DolbyDigitalReencode stage
***************************************************************************/
BERR_Code BAPE_DolbyDigitalReencode_Create(
    BAPE_Handle deviceHandle,
    const BAPE_DolbyDigitalReencodeSettings *pSettings,
    BAPE_DolbyDigitalReencodeHandle *pHandle
    )
{
    BAPE_DolbyDigitalReencodeHandle handle;
    BAPE_DolbyDigitalReencodeSettings defaults;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pHandle);

    if ( NULL == pSettings )
    {
        pSettings = &defaults;
        BAPE_DolbyDigitalReencode_GetDefaultSettings(&defaults);
    }
    
    handle = BKNI_Malloc(sizeof(BAPE_DolbyDigitalReencode));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(handle, 0, sizeof(BAPE_DolbyDigitalReencode));
    BDBG_OBJECT_SET(handle, BAPE_DolbyDigitalReencode);
    handle->settings = *pSettings;
    BAPE_P_InitPathNode(&handle->node, BAPE_PathNodeType_ePostProcessor, BAPE_PostProcessorType_eDdre, 3, deviceHandle, handle);
    handle->node.pName = "DDRE";
    handle->node.connectors[BAPE_ConnectorFormat_eStereo].pName = "stereo";
    handle->node.connectors[BAPE_ConnectorFormat_eStereo].useBufferPool = true;
    handle->node.connectors[BAPE_ConnectorFormat_eMultichannel].pName = "multichannel";
    handle->node.connectors[BAPE_ConnectorFormat_eMultichannel].useBufferPool = true;
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed].pName = "compressed";
    handle->node.connectors[BAPE_ConnectorFormat_eCompressed].useBufferPool = true;    

    BAPE_FMT_P_InitDescriptor(&format);
    format.source = BAPE_DataSource_eDspBuffer;
    format.type = BAPE_DataType_ePcmStereo;
    format.sampleRate = 48000;              /* DDRE output is fixed @ 48k */
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eStereo], &format);
    if ( errCode ) { (void)BERR_TRACE(errCode); goto err_connector_format; }

    format.type = BAPE_DataType_ePcm5_1;
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eMultichannel], &format);
    if ( errCode ) { (void)BERR_TRACE(errCode); goto err_connector_format; }

    format.type = BAPE_DataType_eIec61937;
    BAPE_FMT_P_SetAudioCompressionStd(&format, BAVC_AudioCompressionStd_eAc3);
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[BAPE_ConnectorFormat_eCompressed], &format);
    if ( errCode ) { (void)BERR_TRACE(errCode); goto err_connector_format; }

    /* DDRE only works with 5.1 source content from the DSP */
    BAPE_PathNode_P_GetInputCapabilities(&handle->node, &caps);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eDspBuffer);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcmStereo);
    BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcm5_1);
    errCode = BAPE_PathNode_P_SetInputCapabilities(&handle->node, &caps);
    if ( errCode ) { (void)BERR_TRACE(errCode); goto err_caps; }

    /* Generic Routines */
    handle->node.allocatePathToOutput = BAPE_DSP_P_AllocatePathToOutput;
    handle->node.configPathToOutput = BAPE_DSP_P_ConfigPathToOutput;
    handle->node.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;
    handle->node.startPathToOutput = BAPE_DSP_P_StartPathToOutput;
    handle->node.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;

    /* DolbyDigitalReencode Specifics */
    handle->node.inputFormatChange = BAPE_DolbyDigitalReencode_P_InputFormatChange;
    handle->node.allocatePathFromInput = BAPE_DolbyDigitalReencode_P_AllocatePathFromInput;
    handle->node.configPathFromInput = BAPE_DolbyDigitalReencode_P_ConfigPathFromInput;
    handle->node.stopPathFromInput = BAPE_DolbyDigitalReencode_P_StopPathFromInput;
    handle->node.removeInput = BAPE_DolbyDigitalReencode_P_RemoveInputCallback;
    handle->node.inputSampleRateChange_isr = BAPE_DolbyDigitalReencode_P_InputSampleRateChange_isr;

    *pHandle = handle;
    return BERR_SUCCESS;

err_connector_format:
err_caps:
    BAPE_DolbyDigitalReencode_Destroy(handle);
    return errCode;
}

void BAPE_DolbyDigitalReencode_Destroy(
    BAPE_DolbyDigitalReencodeHandle handle
    )
{
    bool running;
    BDBG_OBJECT_ASSERT(handle, BAPE_DolbyDigitalReencode);
    running = (handle->node.connectors[BAPE_ConnectorFormat_eStereo].task != NULL)?true:false;
    BDBG_ASSERT(false == running);
    BDBG_ASSERT(NULL == handle->input);
    BDBG_OBJECT_DESTROY(handle, BAPE_DolbyDigitalReencode);
    BKNI_Free(handle);
}

void BAPE_DolbyDigitalReencode_GetSettings(
    BAPE_DolbyDigitalReencodeHandle handle,
    BAPE_DolbyDigitalReencodeSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_DolbyDigitalReencode);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}

BERR_Code BAPE_DolbyDigitalReencode_SetSettings(
    BAPE_DolbyDigitalReencodeHandle handle,
    const BAPE_DolbyDigitalReencodeSettings *pSettings
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_DolbyDigitalReencode);
    BDBG_ASSERT(NULL != pSettings);

    if ( pSettings != &handle->settings )
    {
        handle->settings = *pSettings;
    }

    if ( handle->node.connectors[BAPE_ConnectorFormat_eStereo].task != NULL )
    {
        errCode = BAPE_DolbyDigitalReencode_P_ApplyDspSettings(handle, 
                                                               handle->node.connectors[BAPE_ConnectorFormat_eStereo].task,
                                                               handle->node.connectors[BAPE_ConnectorFormat_eStereo].branchId, 
                                                               handle->node.connectors[BAPE_ConnectorFormat_eStereo].stageId,
                                                               handle->node.connectors[BAPE_ConnectorFormat_eCompressed].branchId, 
                                                               handle->node.connectors[BAPE_ConnectorFormat_eCompressed].stageId);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}


void BAPE_DolbyDigitalReencode_GetConnector(
    BAPE_DolbyDigitalReencodeHandle handle,
    BAPE_ConnectorFormat format,
    BAPE_Connector *pConnector
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_DolbyDigitalReencode);
    BDBG_ASSERT(NULL != pConnector);
    switch ( format )
    {
    case BAPE_ConnectorFormat_eStereo:
    case BAPE_ConnectorFormat_eMultichannel:
    case BAPE_ConnectorFormat_eCompressed:
        /* Valid */
        break;
    default:
        BDBG_ERR(("Connector format %u not supported by BAPE_DolbyDigitalReencode", format));
        *pConnector = NULL;
        return;
    }
    *pConnector = &handle->node.connectors[format];
}


BERR_Code BAPE_DolbyDigitalReencode_AddInput(
    BAPE_DolbyDigitalReencodeHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_DolbyDigitalReencode);
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


BERR_Code BAPE_DolbyDigitalReencode_RemoveInput(
    BAPE_DolbyDigitalReencodeHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_DolbyDigitalReencode);
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


BERR_Code BAPE_DolbyDigitalReencode_RemoveAllInputs(
    BAPE_DolbyDigitalReencodeHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_DolbyDigitalReencode);
    if ( handle->input )
    {
        return BAPE_DolbyDigitalReencode_RemoveInput(handle, handle->input);
    }
    return BERR_SUCCESS;
}

static BERR_Code BAPE_DolbyDigitalReencode_P_InputFormatChange(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, const BAPE_FMT_Descriptor *pNewFormat)
{
    BAPE_FMT_Descriptor outputFormat;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pNewFormat);

    /* DDRE output formats are fixed.  All other fields are passthrough. */
    outputFormat = *pNewFormat;
    outputFormat.type = BAPE_DataType_ePcmStereo;
    outputFormat.sampleRate = 48000;              /* DDRE output is fixed @ 48k */
    errCode = BAPE_Connector_P_SetFormat(&pNode->connectors[BAPE_ConnectorFormat_eStereo], &outputFormat);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    outputFormat = *pNewFormat;
    outputFormat.type = BAPE_DataType_ePcm5_1;
    outputFormat.sampleRate = 48000;              /* DDRE output is fixed @ 48k */
    errCode = BAPE_Connector_P_SetFormat(&pNode->connectors[BAPE_ConnectorFormat_eMultichannel], &outputFormat);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    outputFormat = *pNewFormat;
    outputFormat.type = BAPE_DataType_eIec61937;
    outputFormat.sampleRate = 48000;              /* DDRE output is fixed @ 48k */
    BAPE_FMT_P_SetAudioCompressionStd(&outputFormat, BAVC_AudioCompressionStd_eAc3);
    errCode = BAPE_Connector_P_SetFormat(&pNode->connectors[BAPE_ConnectorFormat_eCompressed], &outputFormat);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_DolbyDigitalReencode_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_DolbyDigitalReencodeHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->pTaskCreateSettings);
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_DolbyDigitalReencode);
    /* Add stage to CIT and propagate task settings */
    errCode = BAPE_DSP_P_AddProcessingStage(pConnection->pSource->pTaskCreateSettings, pConnection->pSource->branchId, pConnection->pSource->stageId, 
                                            BAPE_DSP_P_GetDataTypeFromConnector(handle->input), 
                                            BDSP_AudioProcessing_eDdre,
                                            false,
                                            &pNode->connectors[BAPE_ConnectorFormat_eStereo].branchId, 
                                            &pNode->connectors[BAPE_ConnectorFormat_eStereo].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    pNode->connectors[BAPE_ConnectorFormat_eStereo].pTaskCreateSettings = pConnection->pSource->pTaskCreateSettings;
    pNode->connectors[BAPE_ConnectorFormat_eMultichannel].branchId = pNode->connectors[BAPE_ConnectorFormat_eStereo].branchId;
    pNode->connectors[BAPE_ConnectorFormat_eMultichannel].stageId = pNode->connectors[BAPE_ConnectorFormat_eStereo].stageId;
    pNode->connectors[BAPE_ConnectorFormat_eMultichannel].pTaskCreateSettings = pConnection->pSource->pTaskCreateSettings;
    pNode->connectors[BAPE_ConnectorFormat_eCompressed].pTaskCreateSettings = pConnection->pSource->pTaskCreateSettings;
    
    /* Determine number of outputs on compressed connector */
    BAPE_PathNode_P_GetOutputStatus(pNode, &handle->outputStatus);
    /* If we have at least one compressed consumer, add the transcoder */
    if ( handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eCompressed].totalConnections > 0 )
    {
        bool forceBranch;
        /* Force a branch for transcode if we have any PCM consumers as well */
        forceBranch = ((handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eStereo].totalConnections + 
                        handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMultichannel].totalConnections) > 0)?true:false;
        errCode = BAPE_DSP_P_AddEncodeStage(pConnection->pSource->pTaskCreateSettings, 
                                            pNode->connectors[BAPE_ConnectorFormat_eStereo].branchId, 
                                            pNode->connectors[BAPE_ConnectorFormat_eStereo].stageId, 
                                            BDSP_AF_P_DistinctOpType_eDolbyReEncodeAuxDataOut, 
                                            BDSP_AudioEncode_eAc3, BDSP_EncodeMode_eRealtime,
                                            forceBranch,
                                            &pNode->connectors[BAPE_ConnectorFormat_eCompressed].branchId, 
                                            &pNode->connectors[BAPE_ConnectorFormat_eCompressed].stageId);        
    }
    return BERR_SUCCESS;
}

static BERR_Code BAPE_DolbyDigitalReencode_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_DolbyDigitalReencodeHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->task);

    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_DolbyDigitalReencode);
    pNode->connectors[BAPE_ConnectorFormat_eStereo].task = pConnection->pSource->task;
    pNode->connectors[BAPE_ConnectorFormat_eMultichannel].task = pConnection->pSource->task;
    pNode->connectors[BAPE_ConnectorFormat_eCompressed].task = pConnection->pSource->task;
    errCode = BAPE_DolbyDigitalReencode_P_ApplyDspSettings(handle, 
                                                           handle->node.connectors[BAPE_ConnectorFormat_eStereo].task,
                                                           handle->node.connectors[BAPE_ConnectorFormat_eStereo].branchId, 
                                                           handle->node.connectors[BAPE_ConnectorFormat_eStereo].stageId,
                                                           handle->node.connectors[BAPE_ConnectorFormat_eCompressed].branchId, 
                                                           handle->node.connectors[BAPE_ConnectorFormat_eCompressed].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;    
}

static void BAPE_DolbyDigitalReencode_P_ApplyAc3DecoderSettings(
    BAPE_DolbyDigitalReencodeHandle handle,
    BDSP_Raaga_Audio_DDReencodeConfigParams *pUserConfig, 
    const BAPE_Ac3Settings *pSettings)
{
    BDBG_ASSERT(NULL != pUserConfig);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_MSG(("Applying master decoder AC3 settings"));
    switch ( pSettings->drcMode )
    {
    default:
    case BAPE_Ac3DrcMode_eLine:
        pUserConfig->sUserOutputCfg[0].ui32CompMode = 0;
        break;
    case BAPE_Ac3DrcMode_eRf:
        pUserConfig->sUserOutputCfg[0].ui32CompMode = 1;
        break;
    }
    switch ( pSettings->drcModeDownmix )
    {
    default:
    case BAPE_Ac3DrcMode_eLine:
        pUserConfig->sUserOutputCfg[1].ui32CompMode = 0;
        break;
    case BAPE_Ac3DrcMode_eRf:
        pUserConfig->sUserOutputCfg[1].ui32CompMode = 1;
        break;
    }
    switch ( handle->masterDecoder->settings.dualMonoMode )
    {
    default:
    case BAPE_DualMonoMode_eStereo:
        pUserConfig->sUserOutputCfg[0].ui32DualMode = 0;
        pUserConfig->sUserOutputCfg[1].ui32DualMode = 0;
        break;
    case BAPE_DualMonoMode_eLeft:
        pUserConfig->sUserOutputCfg[0].ui32DualMode = 1;
        pUserConfig->sUserOutputCfg[1].ui32DualMode = 1;
        break;
    case BAPE_DualMonoMode_eRight:
        pUserConfig->sUserOutputCfg[0].ui32DualMode = 2;
        pUserConfig->sUserOutputCfg[1].ui32DualMode = 2;
        break;
    case BAPE_DualMonoMode_eMix:
        pUserConfig->sUserOutputCfg[0].ui32DualMode = 3;
        pUserConfig->sUserOutputCfg[1].ui32DualMode = 3;
        break;
    }
    switch ( pSettings->stereoMode )
    {
    default:
    case BAPE_Ac3StereoMode_eLtRt:
        pUserConfig->sUserOutputCfg[0].ui32StereoMode = 0;
        pUserConfig->sUserOutputCfg[1].ui32StereoMode = 0;
        break;
    case BAPE_Ac3StereoMode_eLoRo:
        pUserConfig->sUserOutputCfg[0].ui32StereoMode = 1;
        pUserConfig->sUserOutputCfg[1].ui32StereoMode = 1;
        break;
    }
    pUserConfig->sUserOutputCfg[0].ui32DrcCutFac = BAPE_P_FloatToQ131(pSettings->drcScaleHi, 100);
    pUserConfig->sUserOutputCfg[0].ui32DrcBoostFac = BAPE_P_FloatToQ131(pSettings->drcScaleLow, 100);
    pUserConfig->sUserOutputCfg[1].ui32DrcCutFac = BAPE_P_FloatToQ131(pSettings->drcScaleHiDownmix, 100);
    pUserConfig->sUserOutputCfg[1].ui32DrcBoostFac = BAPE_P_FloatToQ131(pSettings->drcScaleLowDownmix, 100);    
}

static void BAPE_DolbyDigitalReencode_P_ApplyAacDecoderSettings(
    BAPE_DolbyDigitalReencodeHandle handle,
    BDSP_Raaga_Audio_DDReencodeConfigParams *pUserConfig, 
    const BAPE_AacSettings *pSettings)
{
    BDBG_ASSERT(NULL != pUserConfig);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_MSG(("Applying master decoder AAC settings"));
    switch ( pSettings->drcMode )
    {
    default:
    case BAPE_DolbyPulseDrcMode_eLine:
        pUserConfig->sUserOutputCfg[0].ui32CompMode = 0;
        pUserConfig->sUserOutputCfg[1].ui32CompMode = 0;
        break;
    case BAPE_DolbyPulseDrcMode_eRf:
        pUserConfig->sUserOutputCfg[0].ui32CompMode = 1;
        pUserConfig->sUserOutputCfg[1].ui32CompMode = 1;
        break;
    }
    switch ( handle->masterDecoder->settings.dualMonoMode )
    {
    default:
    case BAPE_DualMonoMode_eStereo:
        pUserConfig->sUserOutputCfg[0].ui32DualMode = 0;
        pUserConfig->sUserOutputCfg[1].ui32DualMode = 0;
        break;
    case BAPE_DualMonoMode_eLeft:
        pUserConfig->sUserOutputCfg[0].ui32DualMode = 1;
        pUserConfig->sUserOutputCfg[1].ui32DualMode = 1;
        break;
    case BAPE_DualMonoMode_eRight:
        pUserConfig->sUserOutputCfg[0].ui32DualMode = 2;
        pUserConfig->sUserOutputCfg[1].ui32DualMode = 2;
        break;
    case BAPE_DualMonoMode_eMix:
        pUserConfig->sUserOutputCfg[0].ui32DualMode = 3;
        pUserConfig->sUserOutputCfg[1].ui32DualMode = 3;
        break;
    }
    switch ( pSettings->downmixMode )
    {
    default:
    case BAPE_AacStereoMode_eLtRt:
        pUserConfig->sUserOutputCfg[0].ui32StereoMode = 0;
        pUserConfig->sUserOutputCfg[1].ui32StereoMode = 0;
        break;
    case BAPE_AacStereoMode_eLoRo:
        pUserConfig->sUserOutputCfg[0].ui32StereoMode = 1;
        pUserConfig->sUserOutputCfg[1].ui32StereoMode = 1;
        break;
    case BAPE_AacStereoMode_eArib:
        pUserConfig->sUserOutputCfg[0].ui32StereoMode = 2;
        pUserConfig->sUserOutputCfg[1].ui32StereoMode = 2;
        break;
    }
    pUserConfig->sUserOutputCfg[0].ui32DrcCutFac = BAPE_P_FloatToQ131(pSettings->drcScaleHi, 100);
    pUserConfig->sUserOutputCfg[0].ui32DrcBoostFac = BAPE_P_FloatToQ131(pSettings->drcScaleLow, 100);
    pUserConfig->sUserOutputCfg[1].ui32DrcCutFac = BAPE_P_FloatToQ131(pSettings->drcScaleHi, 100);
    pUserConfig->sUserOutputCfg[1].ui32DrcBoostFac = BAPE_P_FloatToQ131(pSettings->drcScaleLow, 100);    
}

static BERR_Code BAPE_DolbyDigitalReencode_P_ApplyDspSettings(BAPE_DolbyDigitalReencodeHandle handle, BDSP_TaskHandle task, 
                                                              unsigned ddreBranchId, unsigned ddreStageId, unsigned encBranchId, unsigned encStageId)
{
    bool lfe;
    BERR_Code errCode;
    BAPE_ChannelMode channelMode;
    bool externalPcmMode;
    BDSP_Raaga_Audio_DDReencodeConfigParams ddreSettings;
    BDSP_Raaga_Audio_DDTranscodeConfigParams encodeSettings;
    bool forceDrcModes = false;

    /* Determine output mode */
    lfe = true;
    channelMode = BAPE_DSP_P_GetChannelMode(BAVC_AudioCompressionStd_eAc3, BAPE_ChannelMode_e3_2, BAPE_DolbyDigitalReencode_P_HasMultichannelOutput(handle), BAPE_MultichannelFormat_e5_1);
    if ( !BAPE_DSP_P_IsLfePermitted(channelMode) )
    {
        lfe = false;
    }   

    errCode = BDSP_Task_GetStageSettings(task, ddreBranchId, ddreStageId, &ddreSettings, sizeof(ddreSettings));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    externalPcmMode = handle->settings.externalPcmMode;
    if ( NULL == handle->masterDecoder )
    {
        if ( !externalPcmMode )
        {
            BDBG_MSG(("No master input detected.  Forcing external PCM mode."));
            externalPcmMode = true;
        }
    }
    else 
    {
        bool msDecoder=false;
        BAVC_AudioCompressionStd codec;

        codec = handle->masterDecoder->startSettings.codec;
        switch ( codec )
        {
        case BAVC_AudioCompressionStd_eAc3:
        case BAVC_AudioCompressionStd_eAc3Plus:
        case BAVC_AudioCompressionStd_eAacAdts:
        case BAVC_AudioCompressionStd_eAacLoas:
        case BAVC_AudioCompressionStd_eAacPlusAdts:
        case BAVC_AudioCompressionStd_eAacPlusLoas:
            BDBG_MSG(("Dolby Multistream Input Detected."));
            msDecoder = true;
            if ( externalPcmMode )
            {
                BDBG_WRN(("External PCM Mode enabled, but master input is a dolby multi-stream codec (%s).  Using External PCM Mode.",
                          BAPE_P_GetCodecName(codec)));
            }
            break;
        case BAVC_AudioCompressionStd_ePcmWav:
            if ( !externalPcmMode )
            {
                /* This is permitted for certification purposes, but not recommended for general usage. */
                BDBG_WRN(("PCM data is being received, but externalPcmMode is not active."));
                BDBG_WRN(("BAPE_DolbyDigitalReencodeSettings.externalPcmMode = true is recommended for PCM."));
            }
            break;
        default:
            if ( !externalPcmMode )
            {
                BDBG_WRN(("Master input detected, but codec is not a dolby multi-stream codec (%s).  Forcing external PCM mode.",
                          BAPE_P_GetCodecName(codec)));
                externalPcmMode = true;
            }
            break;
        }
        if ( handle->input->format.type == BAPE_DataType_ePcmStereo && !externalPcmMode )
        {
            BDBG_MSG(("Stereo input detected.  Forcing external PCM mode."));
            externalPcmMode = true;
        }
        if ( msDecoder )
        {
            BAPE_DecoderCodecSettings decoderSettings;
            BAPE_Decoder_GetCodecSettings(handle->masterDecoder, handle->masterDecoder->startSettings.codec, &decoderSettings);
            /* Would prefer a switch statement but coverity will flag dead code based on the msdecoder flag. */
            if ( codec == BAVC_AudioCompressionStd_eAc3 )
            {
                BAPE_DolbyDigitalReencode_P_ApplyAc3DecoderSettings(handle, &ddreSettings, &decoderSettings.codecSettings.ac3);
            }
            else if ( codec == BAVC_AudioCompressionStd_eAc3Plus )
            {
                BAPE_DolbyDigitalReencode_P_ApplyAc3DecoderSettings(handle, &ddreSettings, &decoderSettings.codecSettings.ac3Plus);
            }
            else if ( codec == BAVC_AudioCompressionStd_eAacAdts ||
                      codec == BAVC_AudioCompressionStd_eAacLoas )
            {
                BAPE_DolbyDigitalReencode_P_ApplyAacDecoderSettings(handle, &ddreSettings, &decoderSettings.codecSettings.aac);
            }
            else  /* AAC-Plus */
            {
                BAPE_DolbyDigitalReencode_P_ApplyAacDecoderSettings(handle, &ddreSettings, &decoderSettings.codecSettings.aacPlus);
            }
        }
    }

    /* Handle external PCM mode DRC/downmix settings */
    if ( externalPcmMode )
    {
        switch ( handle->settings.drcMode )
        {
        default:
        case BAPE_DolbyDigitalReencodeDrcMode_eLine:
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[0].ui32CompMode, 0);
            break;
        case BAPE_DolbyDigitalReencodeDrcMode_eRf:
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[0].ui32CompMode, 1);
            break;
        }
        switch ( handle->settings.drcModeDownmix )
        {
        default:
        case BAPE_DolbyDigitalReencodeDrcMode_eLine:
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[1].ui32CompMode, 0);
            break;
        case BAPE_DolbyDigitalReencodeDrcMode_eRf:
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[1].ui32CompMode, 1);
            break;
        }
        switch ( handle->settings.dualMonoMode )
        {
        default:
        case BAPE_DualMonoMode_eStereo:
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[0].ui32DualMode, 0);
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[1].ui32DualMode, 0);
            break;
        case BAPE_DualMonoMode_eLeft:
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[0].ui32DualMode, 1);
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[1].ui32DualMode, 1);
            break;
        case BAPE_DualMonoMode_eRight:
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[0].ui32DualMode, 2);
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[1].ui32DualMode, 2);
            break;
        case BAPE_DualMonoMode_eMix:
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[0].ui32DualMode, 3);
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[1].ui32DualMode, 3);
            break;
        }
        switch ( handle->settings.stereoMode )
        {
        default:
        case BAPE_DolbyDigitalReencodeStereoMode_eLtRt:
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[0].ui32StereoMode, 0);
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[1].ui32StereoMode, 0);
            break;
        case BAPE_DolbyDigitalReencodeStereoMode_eLoRo:
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[0].ui32StereoMode, 1);
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[1].ui32StereoMode, 1);
            break;
        case BAPE_DolbyDigitalReencodeStereoMode_eArib:
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[0].ui32StereoMode, 2);
            BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[1].ui32StereoMode, 2);
            break;
        }
        BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[0].ui32DrcCutFac, BAPE_P_FloatToQ131(handle->settings.drcScaleHi, 100));
        BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[0].ui32DrcBoostFac, BAPE_P_FloatToQ131(handle->settings.drcScaleLow, 100));
        BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[1].ui32DrcCutFac, BAPE_P_FloatToQ131(handle->settings.drcScaleHi, 100));
        BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[1].ui32DrcBoostFac, BAPE_P_FloatToQ131(handle->settings.drcScaleLow, 100));    
    }

    /* Handle loudness equivalence. */
    if ( handle->settings.loudnessEquivalenceEnabled &&
         handle->node.deviceHandle->settings.loudnessMode != BAPE_LoudnessEquivalenceMode_eNone )
    {
        forceDrcModes = true;
    }

    BAPE_DSP_P_SET_VARIABLE(ddreSettings, ui32ExternalPcmEnabled, externalPcmMode?1:0);
    BAPE_DSP_P_SET_VARIABLE(ddreSettings, ui32CompProfile, handle->settings.profile);
    BAPE_DSP_P_SET_VARIABLE(ddreSettings, ui32CmixLev, handle->settings.centerMixLevel);
    BAPE_DSP_P_SET_VARIABLE(ddreSettings, ui32SurmixLev, handle->settings.surroundMixLevel);
    BAPE_DSP_P_SET_VARIABLE(ddreSettings, ui32DsurMod, handle->settings.dolbySurround);
    BAPE_DSP_P_SET_VARIABLE(ddreSettings, ui32DialNorm, handle->settings.dialogLevel);

    /* Setup correct number of outputs and derive parameters from master decoder if required */
    if ( BAPE_DolbyDigitalReencode_P_HasStereoOutput(handle) > 0 &&
         BAPE_DolbyDigitalReencode_P_HasMultichannelOutput(handle) )
    {
        BAPE_DSP_P_SET_VARIABLE(ddreSettings, ui32NumOutPorts, 2);
        BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[1].ui32OutMode, BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e2_0));    /* Stereo */
        BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[1].ui32OutLfe, 0);
    }
    else
    {
        BAPE_DSP_P_SET_VARIABLE(ddreSettings, ui32NumOutPorts, 1);            
    }

    if ( forceDrcModes )
    {
        BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[0].ui32CompMode, 1);   /* RF (-20dB) */
        BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[1].ui32CompMode, 1);   /* RF (-20dB) */
    }
 
    BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[0].ui32OutMode, BAPE_DSP_P_ChannelModeToDsp(channelMode));
    BAPE_DSP_P_SET_VARIABLE(ddreSettings, sUserOutputCfg[0].ui32OutLfe, lfe?1:0);
    BAPE_DSP_P_GetChannelMatrix(channelMode, lfe, ddreSettings.sUserOutputCfg[0].ui32OutputChannelMatrix);
    
    errCode = BDSP_Task_SetStageSettings(task, ddreBranchId, ddreStageId, &ddreSettings, sizeof(ddreSettings));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    
    if ( BAPE_DolbyDigitalReencode_P_HasCompressedOutput(handle) )
    {
        errCode = BDSP_Task_GetStageSettings(task, encBranchId, encStageId, &encodeSettings, sizeof(encodeSettings));
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        BAPE_DSP_P_SET_VARIABLE(encodeSettings, eTranscodeEnable, handle->settings.encodeSettings.certificationMode?BDSP_AF_P_eDisable:BDSP_AF_P_eEnable);
        BAPE_DSP_P_SET_VARIABLE(encodeSettings, eSpdifHeaderEnable, handle->settings.encodeSettings.spdifHeaderEnabled?BDSP_AF_P_eEnable:BDSP_AF_P_eDisable);
    
        errCode = BDSP_Task_SetStageSettings(task, encBranchId, encStageId, &encodeSettings, sizeof(encodeSettings));
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}

static void BAPE_DolbyDigitalReencode_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    /* Invalidate task handle */
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BSTD_UNUSED(pConnection);
    pNode->connectors[BAPE_ConnectorFormat_eStereo].task = NULL;
}

static void BAPE_DolbyDigitalReencode_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector)
{
    (void)BAPE_DolbyDigitalReencode_RemoveInput(pNode->pHandle, pConnector);
}

static void BAPE_DolbyDigitalReencode_P_InputSampleRateChange_isr(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection, unsigned newSampleRate)
{
    BSTD_UNUSED(pNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_MSG(("Input sample rate changed to %u Hz.  Overriding to 48kHz output rate.", newSampleRate));
}

BERR_Code BAPE_DolbyDigitalReencode_P_SettingsChanged(
    BAPE_DolbyDigitalReencodeHandle handle, 
    BAPE_DecoderHandle decoder
    )
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BAPE_DolbyDigitalReencode);
    BDBG_OBJECT_ASSERT(decoder, BAPE_Decoder);
    BDBG_ASSERT(decoder->ddre == handle);
    if ( decoder->fwMixer && !decoder->fwMixerMaster )
    {
        /* We only care about the master's settings */
        return BERR_SUCCESS;
    }
    BDBG_MSG(("Master decoder input settings have changed"));
    handle->masterDecoder = decoder;
    if ( handle->node.connectors[BAPE_ConnectorFormat_eStereo].task )
    {
        errCode = BAPE_DolbyDigitalReencode_P_ApplyDspSettings(handle, 
                                                               handle->node.connectors[BAPE_ConnectorFormat_eStereo].task,
                                                               handle->node.connectors[BAPE_ConnectorFormat_eStereo].branchId, 
                                                               handle->node.connectors[BAPE_ConnectorFormat_eStereo].stageId,
                                                               handle->node.connectors[BAPE_ConnectorFormat_eCompressed].branchId, 
                                                               handle->node.connectors[BAPE_ConnectorFormat_eCompressed].stageId);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    return BERR_SUCCESS;
}

