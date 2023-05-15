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
* $brcm_Workfile: bape_encoder.c $
* $brcm_Revision: Hydra_Software_Devel/SW7425-4386/2 $
* $brcm_Date: 5/16/13 5:53p $
*
* API Description:
*   API name: Encoder
*    Specific APIs related to Audio Encoding
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_encoder.c $
* 
* Hydra_Software_Devel/SW7425-4386/2   5/16/13 5:53p mpeteri
* SW7425-4386: revisions per NEXUS API CR
* 
* Hydra_Software_Devel/SW7425-4386/1   5/14/13 4:53p mpeteri
* SW7425-4386: initial changes for MP3/AAC mono encode
* 
* Hydra_Software_Devel/32   1/17/13 3:50p jgarrett
* SW7425-4433: Merge to main branch
* 
* Hydra_Software_Devel/SW7425_4433/1   1/16/13 6:25p srajapur
* SW7425-4433: Changing AAC HE encoder max bit rate to 52kbps
* 
* Hydra_Software_Devel/31   9/10/12 4:19p jgarrett
* SW7425-3858: Using integer bitrates instead of enumerated values
* 
* Hydra_Software_Devel/30   8/31/12 4:13p jgarrett
* SW7425-3795: Exposing AAC complexity setting
* 
* Hydra_Software_Devel/29   7/3/12 2:12p jgarrett
* SW7425-3115: Adding output sample rate conversion for aac encoder
* 
* Hydra_Software_Devel/28   7/2/12 4:06p jgarrett
* SW7425-3326: Propagating output codec across input format changes
* 
* Hydra_Software_Devel/27   6/6/12 4:33p jgarrett
* SW7435-219: Report error if attempting to encode AC3 from a node other
* than decoder
* 
* Hydra_Software_Devel/26   5/3/12 5:05p jgarrett
* SW7425-2075: Merge to main branch
* 
* Hydra_Software_Devel/SW7425-2075/3   5/3/12 3:45p jgarrett
* SW7425-2075: Merging latest mainline changes
* 
* Hydra_Software_Devel/25   4/27/12 1:37p jgarrett
* SW7425-2802: Adding runtime capabilities routine
* 
* Hydra_Software_Devel/SW7425-2075/2   4/6/12 4:15p jgarrett
* SW7425-2075: Correcting output format handling
* 
* Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:50p jgarrett
* SW7425-2075: Adding new FMT interfaces
* 
* Hydra_Software_Devel/24   2/3/12 4:29p jgarrett
* SW7425-2268: Initial bringup of voice conferencing support
* 
* Hydra_Software_Devel/23   2/2/12 4:49p jgarrett
* SW7425-2268: Enabling mono input
* 
* Hydra_Software_Devel/22   1/31/12 6:17p jgarrett
* SW7425-2268: Adding initial voice conferencing support
* 
* Hydra_Software_Devel/21   12/19/11 4:27p jgarrett
* SW7425-1018: Adding initial A/85 implementation
* 
* Hydra_Software_Devel/20   12/8/11 2:21p jgarrett
* SW7425-1478: Adding WMA Std settings interface
* 
* Hydra_Software_Devel/19   12/7/11 5:44p jgarrett
* SW7425-1478: Adding WMA encoder interface
* 
* Hydra_Software_Devel/18   9/8/11 10:42a jgarrett
* SWDTV-6627: Adding BAPE_Equalizer_GetDefaultSettings stub for chips
* w/out EQ support
* 
* Hydra_Software_Devel/17   6/28/11 8:59a jgarrett
* SW7231-97: Coverity CID 444
* 
* Hydra_Software_Devel/16   6/27/11 5:41p jgarrett
* SW7231-97: Refactoring SPDIF/HDMI enable mechanisms to handle older
* Onkyo receiver DTS->PCM switching requirements
* 
* Hydra_Software_Devel/15   6/17/11 10:21a jgarrett
* SW7425-741: Adding missing GetStageSettings for MP3 encode settings
* 
* Hydra_Software_Devel/14   6/16/11 3:27p jgarrett
* SW7425-741: Adding MP3 encoder
* 
* Hydra_Software_Devel/13   6/14/11 3:37p jgarrett
* SW7425-716: Updating AAC bitrate to enum value
* 
* Hydra_Software_Devel/12   5/26/11 6:10p jgarrett
* SW7425-408: Changing connection format between dolby pulse and DD
* transcode
* 
* Hydra_Software_Devel/11   5/18/11 6:50p jgarrett
* SW7425-408: Adding preliminary DDRE support
* 
* Hydra_Software_Devel/10   4/29/11 11:00a jgarrett
* SW7422-368: Switching to new GetDefaultSettings routines
* 
* Hydra_Software_Devel/9   4/18/11 10:09p jgarrett
* SW7425-361: Refactoring DSP branch decisions
* 
* Hydra_Software_Devel/8   4/17/11 1:55p jgarrett
* SW7425-288: Adding audio codec to metadata
* 
* Hydra_Software_Devel/7   4/16/11 12:15p jgarrett
* SW7425-371: Removing tab characters
* 
* Hydra_Software_Devel/6   4/14/11 3:25p jgarrett
* SW7425-360: Fixing shutdown crash if resources are leaked in the app
* 
* Hydra_Software_Devel/5   3/21/11 7:07p jgarrett
* SW7422-355: Adding encoder updates
* 
* Hydra_Software_Devel/4   3/1/11 7:26p jgarrett
* SW7422-146: Adding encoders
* 
* Hydra_Software_Devel/3   2/22/11 5:43p jgarrett
* SW7422-146: Implemented type renaming based on filter graph review
* comments
* 
* Hydra_Software_Devel/2   1/13/11 5:26p jgarrett
* SW7422-146: Renaming encoder codec settings
* 
* Hydra_Software_Devel/1   1/12/11 4:24p jgarrett
* SW7422-146: Adding additional APIs
* 
***************************************************************************/

#include "bape.h"
#include "bape_priv.h"
#include "bdsp_raaga.h"

BDBG_MODULE(bape_encoder);

BDBG_OBJECT_ID(BAPE_Encoder);
typedef struct BAPE_Encoder
{
    BDBG_OBJECT(BAPE_Encoder)
    BAPE_PathNode node;
    BAPE_EncoderSettings settings;
    BAPE_Ac3EncodeSettings ac3Settings;
    BAPE_DtsEncodeSettings dtsSettings;
    BAPE_AacEncodeSettings aacSettings, aacPlusSettings;
    BAPE_MpegEncodeSettings mp3Settings;
    BAPE_WmaEncodeSettings wmaSettings;
    BAPE_G711EncodeSettings g711Settings;
    BAPE_G726EncodeSettings g726Settings;
    BAPE_G729EncodeSettings g729Settings;
    BAPE_G723_1EncodeSettings g723_1Settings;
    BAPE_Connector input;
} BAPE_Encoder;

static const char *BAPE_Encoder_P_GetName(BAVC_AudioCompressionStd codec);
static void BAPE_Encoder_P_InputSampleRateChange_isr(BAPE_PathNode *pNode, BAPE_PathConnection *pConnection, unsigned sampleRate);
static BERR_Code BAPE_Encoder_P_InputFormatChange(BAPE_PathNode *pNode, BAPE_PathConnection *pConnection, const BAPE_FMT_Descriptor *pNewFormat);
static BERR_Code BAPE_Encoder_P_ApplyDspSettings(BAPE_EncoderHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId);
static BERR_Code BAPE_Encoder_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static BERR_Code BAPE_Encoder_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_Encoder_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection);
static void BAPE_Encoder_P_GetDefaultCodecSettings(BAPE_EncoderHandle handle);
static void BAPE_Encoder_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector);

/***************************************************************************
Summary:
    Get default settings for an Audio Encoder stage
***************************************************************************/
void BAPE_Encoder_GetDefaultSettings(
    BAPE_EncoderSettings *pSettings   /* [out] default settings */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    pSettings->codec = BAVC_AudioCompressionStd_eAc3;
    pSettings->loudnessEquivalenceEnabled = true;
}

/***************************************************************************
Summary:
    Open an Audio Encoder stage
***************************************************************************/
BERR_Code BAPE_Encoder_Create(
    BAPE_Handle deviceHandle,
    const BAPE_EncoderSettings *pSettings,
    BAPE_EncoderHandle *pHandle             /* [out] */
    )
{
    BAPE_EncoderHandle handle;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(deviceHandle, BAPE_Device);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pHandle);

    handle = BKNI_Malloc(sizeof(BAPE_Encoder));
    if ( NULL == handle )
    {
        return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
    }
    BKNI_Memset(handle, 0, sizeof(BAPE_Encoder));
    BDBG_OBJECT_SET(handle, BAPE_Encoder);
    BAPE_P_InitPathNode(&handle->node, BAPE_PathNodeType_eEncoder, pSettings->codec, 1, deviceHandle, handle);
    handle->node.pName = BAPE_Encoder_P_GetName(pSettings->codec);
    handle->node.connectors[0].useBufferPool = true;    

    /* Format and capabilities are set in SetSettings per-codec */

    /* Generic Routines */
    handle->node.allocatePathToOutput = BAPE_DSP_P_AllocatePathToOutput;
    handle->node.configPathToOutput = BAPE_DSP_P_ConfigPathToOutput;
    handle->node.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;
    handle->node.startPathToOutput = BAPE_DSP_P_StartPathToOutput;
    handle->node.stopPathToOutput = BAPE_DSP_P_StopPathToOutput;

    /* Encoder Specifics */
    handle->node.inputSampleRateChange_isr = BAPE_Encoder_P_InputSampleRateChange_isr;
    handle->node.inputFormatChange = BAPE_Encoder_P_InputFormatChange;
    handle->node.allocatePathFromInput = BAPE_Encoder_P_AllocatePathFromInput;
    handle->node.configPathFromInput = BAPE_Encoder_P_ConfigPathFromInput;
    handle->node.stopPathFromInput = BAPE_Encoder_P_StopPathFromInput;
    handle->node.removeInput = BAPE_Encoder_P_RemoveInputCallback;

    /* Init codec settings */
    BAPE_Encoder_P_GetDefaultCodecSettings(handle);
    errCode = BAPE_Encoder_SetSettings(handle, pSettings);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
        goto err_settings;
    }

    *pHandle = handle;
    return BERR_SUCCESS;

err_settings:
    BAPE_Encoder_Destroy(handle);
    return errCode;
}

/***************************************************************************
Summary:
    Close an Audio Encoder stage
    
Description:
    Input to the stage must be removed prior to closing.
***************************************************************************/
void BAPE_Encoder_Destroy(
    BAPE_EncoderHandle handle
    )
{
    bool running;
    BDBG_OBJECT_ASSERT(handle, BAPE_Encoder);
    running = (handle->node.connectors[0].task != NULL)?true:false;
    BDBG_ASSERT(false == running);
    BDBG_ASSERT(NULL == handle->input);
    BDBG_OBJECT_DESTROY(handle, BAPE_Encoder);
    BKNI_Free(handle);
}


/***************************************************************************
Summary:
    Get Settings for an Audio Encoder stage
***************************************************************************/
void BAPE_Encoder_GetSettings(
    BAPE_EncoderHandle handle,
    BAPE_EncoderSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Encoder);
    BDBG_ASSERT(NULL != pSettings);
    *pSettings = handle->settings;
}


/***************************************************************************
Summary:
    Set Settings for an Audio Encoder stage
***************************************************************************/
BERR_Code BAPE_Encoder_SetSettings(
    BAPE_EncoderHandle handle,
    const BAPE_EncoderSettings *pSettings
    )
{
    bool running;
    BERR_Code errCode;
    BAPE_FMT_Descriptor format;
    BAPE_FMT_Capabilities caps;

    BDBG_OBJECT_ASSERT(handle, BAPE_Encoder);
    BDBG_ASSERT(NULL != pSettings);
    running = (handle->node.connectors[0].task != NULL)?true:false;
    if ( running && pSettings->codec != handle->settings.codec )
    {
        BDBG_ERR(("Can not change codec while running."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    handle->settings = *pSettings;
    BAPE_Connector_P_GetFormat(&handle->node.connectors[0], &format);
    format.source = BAPE_DataSource_eDspBuffer;
    BAPE_FMT_P_SetAudioCompressionStd(&format, pSettings->codec);
    switch ( pSettings->codec )
    {
    case BAVC_AudioCompressionStd_eAc3:
    case BAVC_AudioCompressionStd_eDts:
        /* AC3/DTS can be sent to SPDIF/HDMI */
        format.type = BAPE_DataType_eIec61937;
        break;
    default:
        /* Other codecs can only be sent to MuxOutput */
        format.type = BAPE_DataType_eCompressedRaw;
        break;
    }
    errCode = BAPE_Connector_P_SetFormat(&handle->node.connectors[0], &format);
    if ( errCode ) 
    {
        return BERR_TRACE(errCode);
    }

    /* Voice Conferencing codecs only support mono.  DTS/AC3 support 5.1/2.0.  Others are 2.0 only */
    BAPE_FMT_P_InitCapabilities(&caps, NULL, NULL);
    BAPE_FMT_P_EnableSource(&caps, BAPE_DataSource_eDspBuffer);
    switch ( pSettings->codec )
    {
    case BAVC_AudioCompressionStd_eG711:
    case BAVC_AudioCompressionStd_eG726:
    case BAVC_AudioCompressionStd_eG723_1:
    case BAVC_AudioCompressionStd_eG729:
        BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcmMono);
        break;
    case BAVC_AudioCompressionStd_eAc3:
    case BAVC_AudioCompressionStd_eDts:
        BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcm5_1);
        /* fall-through */
    default:
        BAPE_FMT_P_EnableType(&caps, BAPE_DataType_ePcmStereo);
        break;
    }
    errCode = BAPE_PathNode_P_SetInputCapabilities(&handle->node, &caps);
    if ( errCode ) 
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Get Codec-Specific Settings for an Audio Encoder stage
***************************************************************************/
void BAPE_Encoder_GetCodecSettings(
    BAPE_EncoderHandle handle,
    BAVC_AudioCompressionStd codec,              /* the codec for which you are retrieving settings. */
    BAPE_EncoderCodecSettings *pSettings    /* [out] Settings */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Encoder);
    BDBG_ASSERT(NULL != pSettings);
    
    pSettings->codec = codec;
    switch ( codec )
    {
    case BAVC_AudioCompressionStd_eAc3:
        pSettings->codecSettings.ac3 = handle->ac3Settings;
        break;
    case BAVC_AudioCompressionStd_eDts:
        pSettings->codecSettings.dts = handle->dtsSettings;
        break;
    case BAVC_AudioCompressionStd_eAacAdts:
    case BAVC_AudioCompressionStd_eAacLoas:
        pSettings->codecSettings.aac = handle->aacSettings;
        break;
    case BAVC_AudioCompressionStd_eAacPlusAdts:
    case BAVC_AudioCompressionStd_eAacPlusLoas:
        pSettings->codecSettings.aac = handle->aacPlusSettings;
        break;
    case BAVC_AudioCompressionStd_eMpegL3:
        pSettings->codecSettings.mp3 = handle->mp3Settings;
        break;
    case BAVC_AudioCompressionStd_eWmaStd:
        pSettings->codecSettings.wmaStd = handle->wmaSettings;
        break;
    default:
        break;
    }
}

/***************************************************************************
Summary:
    Set Codec-Specific Settings for an Audio Encoder stage
***************************************************************************/
BERR_Code BAPE_Encoder_SetCodecSettings(
    BAPE_EncoderHandle handle,
    const BAPE_EncoderCodecSettings *pSettings
    )
{
    bool running, codecsEqual = false;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, BAPE_Encoder);
    BDBG_ASSERT(NULL != pSettings);
    running = (handle->node.connectors[0].task != NULL)?true:false;

    if ( pSettings->codec == handle->settings.codec )
    {
        codecsEqual = true;
    }
    switch ( pSettings->codec )
    {
    case BAVC_AudioCompressionStd_eAc3:
        handle->ac3Settings = pSettings->codecSettings.ac3;
        break;
    case BAVC_AudioCompressionStd_eDts:
        handle->dtsSettings = pSettings->codecSettings.dts;
        break;
    case BAVC_AudioCompressionStd_eAacAdts:
    case BAVC_AudioCompressionStd_eAacLoas:
        handle->aacSettings = pSettings->codecSettings.aac;
        switch ( handle->settings.codec )
        {
        case BAVC_AudioCompressionStd_eAacAdts:
        case BAVC_AudioCompressionStd_eAacLoas:
            codecsEqual=true;
            break;
        default:
            break;
        }
        break;
    case BAVC_AudioCompressionStd_eAacPlusAdts:
    case BAVC_AudioCompressionStd_eAacPlusLoas:
        handle->aacPlusSettings = pSettings->codecSettings.aacPlus;
        switch ( handle->settings.codec )
        {
        case BAVC_AudioCompressionStd_eAacPlusAdts:
        case BAVC_AudioCompressionStd_eAacPlusLoas:
            codecsEqual=true;
            break;
        default:
            break;
        }
        break;
    case BAVC_AudioCompressionStd_eMpegL1:
    case BAVC_AudioCompressionStd_eMpegL2:        
        switch ( handle->settings.codec )
        {
        case BAVC_AudioCompressionStd_eMpegL1:
        case BAVC_AudioCompressionStd_eMpegL2:
            codecsEqual=true;
            break;
        default:
            break;
        }
        break;
    case BAVC_AudioCompressionStd_eMpegL3:
        handle->mp3Settings = pSettings->codecSettings.mp3;
        break;
    case BAVC_AudioCompressionStd_eSbc:
        break;
    case BAVC_AudioCompressionStd_eWmaStd:
        handle->wmaSettings = pSettings->codecSettings.wmaStd;
        break;
    case BAVC_AudioCompressionStd_eG711:
        handle->g711Settings = pSettings->codecSettings.g711;
        break;
    case BAVC_AudioCompressionStd_eG726:
        handle->g726Settings = pSettings->codecSettings.g726;
        break;
    case BAVC_AudioCompressionStd_eG723_1:
        handle->g723_1Settings = pSettings->codecSettings.g723_1;
        break;
    case BAVC_AudioCompressionStd_eG729:
        handle->g729Settings = pSettings->codecSettings.g729;
        break;
    default:
        break;
    }

    if ( running && codecsEqual )
    {
        errCode = BAPE_Encoder_P_ApplyDspSettings(handle, 
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
void BAPE_Encoder_GetConnector(
    BAPE_EncoderHandle handle,
    BAPE_Connector *pConnector   /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Encoder);
    BDBG_ASSERT(NULL != pConnector);
    *pConnector = &handle->node.connectors[0];
}

/***************************************************************************
Summary:
Add an input to this processing stage
***************************************************************************/
BERR_Code BAPE_Encoder_AddInput(
    BAPE_EncoderHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_Encoder);
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
BERR_Code BAPE_Encoder_RemoveInput(
    BAPE_EncoderHandle handle,
    BAPE_Connector input
    )
{
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, BAPE_Encoder);
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
BERR_Code BAPE_Encoder_RemoveAllInputs(
    BAPE_EncoderHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Encoder);
    if ( handle->input )
    {
        return BAPE_Encoder_RemoveInput(handle, handle->input);
    }
    return BERR_SUCCESS;
}

static const char *BAPE_Encoder_P_GetName(BAVC_AudioCompressionStd codec)
{
    switch ( codec )
    {
    case BAVC_AudioCompressionStd_eDts:
        return "DTS Encoder";
    case BAVC_AudioCompressionStd_eAc3:
        return "AC3 Encoder";
    case BAVC_AudioCompressionStd_eAacAdts:
    case BAVC_AudioCompressionStd_eAacLoas:
        return "AAC Encoder";
    case BAVC_AudioCompressionStd_eAacPlusAdts:
    case BAVC_AudioCompressionStd_eAacPlusLoas:
        return "AAC-HE Encoder";
    case BAVC_AudioCompressionStd_eMpegL1:
    case BAVC_AudioCompressionStd_eMpegL2:
        return "MPEG Encoder";
    case BAVC_AudioCompressionStd_eMpegL3:
        return "MP3 Encoder";
    case BAVC_AudioCompressionStd_eSbc:
        return "SBC Encoder";
    case BAVC_AudioCompressionStd_eWmaStd:
        return "WMA Encoder";
    default:
        return "Encoder";
    }
}

static void BAPE_Encoder_P_InputSampleRateChange_isr(BAPE_PathNode *pNode, BAPE_PathConnection *pConnection, unsigned sampleRate)
{
    BAPE_EncoderHandle hEncoder;
    BAVC_AudioCompressionStd codec;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);

    hEncoder = pNode->pHandle;
    BDBG_OBJECT_ASSERT(hEncoder, BAPE_Encoder);

    codec = hEncoder->settings.codec;

    if ( sampleRate == 48000 )
    {
        switch ( codec )
        {
        case BAVC_AudioCompressionStd_eAacAdts:
        case BAVC_AudioCompressionStd_eAacLoas:
            if ( hEncoder->aacSettings.sampleRate == 32000 )
            {
                sampleRate = 32000;
            }
            break;
        case BAVC_AudioCompressionStd_eAacPlusAdts:
        case BAVC_AudioCompressionStd_eAacPlusLoas:
            if ( hEncoder->aacSettings.sampleRate == 32000 )
            {
                sampleRate = 32000;
            }
            break;
        default:
            break;
        }
    }
    BAPE_Connector_P_SetSampleRate_isr(&pNode->connectors[0], sampleRate);
}

static BERR_Code BAPE_Encoder_P_InputFormatChange(BAPE_PathNode *pNode, BAPE_PathConnection *pConnection, const BAPE_FMT_Descriptor *pNewFormat)
{
    BAPE_FMT_Descriptor outputFormat;
    BAVC_AudioCompressionStd codec;
    BAPE_EncoderHandle hEncoder;
    BAPE_DataType dataType;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pNewFormat);

    hEncoder = pNode->pHandle;
    BDBG_OBJECT_ASSERT(hEncoder, BAPE_Encoder);

    /* This module will convert the input to stereo.  All other fields are passthrough. */
    BAPE_Connector_P_GetFormat(&pNode->connectors[0], &outputFormat);
    dataType = outputFormat.type;
    codec = BAPE_FMT_P_GetAudioCompressionStd(&outputFormat);
    outputFormat = *pNewFormat;
    outputFormat.type = dataType;
    BAPE_FMT_P_SetAudioCompressionStd(&outputFormat, codec);
    if ( pNewFormat->sampleRate == 48000 )
    {
        switch ( codec )
        {
        case BAVC_AudioCompressionStd_eAacAdts:
        case BAVC_AudioCompressionStd_eAacLoas:
            if ( hEncoder->aacSettings.sampleRate == 32000 )
            {
                outputFormat.sampleRate = 32000;
            }
            break;
        case BAVC_AudioCompressionStd_eAacPlusAdts:
        case BAVC_AudioCompressionStd_eAacPlusLoas:
            if ( hEncoder->aacSettings.sampleRate == 32000 )
            {
                outputFormat.sampleRate = 32000;
            }
            break;
        default:
            break;
        }
    }
    errCode = BAPE_Connector_P_SetFormat(&pNode->connectors[0], &outputFormat);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;    
}

static BERR_Code BAPE_Encoder_P_AllocatePathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_EncoderHandle handle;
    BDSP_AF_P_DistinctOpType opType;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->pTaskCreateSettings);
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Encoder);

    opType = BAPE_DSP_P_GetDataTypeFromConnector(handle->input);
    /* Dolby transcoder only supports pulse or DDRE as it's source.  Check for pulse here, DDRE is handled in bape_dolby_digital_reencode.c */
    if ( handle->settings.codec == BAVC_AudioCompressionStd_eAc3 )
    {
        if ( pConnection->pSource->pParent->type == BAPE_PathNodeType_eDecoder )
        {
            BAPE_DecoderHandle decoder = pConnection->pSource->pParent->pHandle;
            switch ( decoder->startSettings.codec )
            {
            case BAVC_AudioCompressionStd_eAacAdts:
            case BAVC_AudioCompressionStd_eAacLoas:
            case BAVC_AudioCompressionStd_eAacPlusAdts:
            case BAVC_AudioCompressionStd_eAacPlusLoas:
#if BDSP_MS10_SUPPORT
                opType = BDSP_AF_P_DistinctOpType_eDolbyReEncodeAuxDataOut;
#endif
                break;
            default:
                BDBG_ERR(("AC3 transcoder can only be used for AAC source material"));
                return BERR_TRACE(BERR_NOT_SUPPORTED);
            }
        }
        else
        {
            BDBG_ERR(("AC3 encoding is only supported when directly connected to an AAC decoder."));
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    /* Add stage to CIT and propagate task settings */
    errCode = BAPE_DSP_P_AddEncodeStage(pConnection->pSource->pTaskCreateSettings, pConnection->pSource->branchId, pConnection->pSource->stageId, 
                                        opType,
                                        BAPE_P_GetCodecAudioEncode(handle->settings.codec), BDSP_EncodeMode_eRealtime,
                                        false,
                                        &pNode->connectors[0].branchId, &pNode->connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    pNode->connectors[0].pTaskCreateSettings = pConnection->pSource->pTaskCreateSettings;
    return BERR_SUCCESS;
}

static BERR_Code BAPE_Encoder_P_ConfigPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    BERR_Code errCode;
    BAPE_EncoderHandle handle;
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BDBG_OBJECT_ASSERT(pConnection, BAPE_PathConnection);
    BDBG_ASSERT(NULL != pConnection->pSource->task);
    handle = pNode->pHandle;
    BDBG_OBJECT_ASSERT(handle, BAPE_Encoder);
    pNode->connectors[0].task = pConnection->pSource->task;
    errCode = BAPE_Encoder_P_ApplyDspSettings(handle, 
                                              handle->node.connectors[0].task, 
                                              handle->node.connectors[0].branchId, 
                                              handle->node.connectors[0].stageId);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;    
}

static void BAPE_Encoder_P_StopPathFromInput(struct BAPE_PathNode *pNode, struct BAPE_PathConnection *pConnection)
{
    /* Invalidate task handle */
    BDBG_OBJECT_ASSERT(pNode, BAPE_PathNode);
    BSTD_UNUSED(pConnection);
    pNode->connectors[0].task = NULL;
}

static BERR_Code BAPE_Encoder_P_ApplyAc3Settings(BAPE_EncoderHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;
    BDSP_Raaga_Audio_DDTranscodeConfigParams userConfig;

    errCode = BDSP_Task_GetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    userConfig.eSpdifHeaderEnable = handle->ac3Settings.spdifHeaderEnabled?BDSP_AF_P_eEnable:BDSP_AF_P_eDisable;

    errCode = BDSP_Task_SetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_Encoder_P_GetDefaultAc3Settings(BAPE_EncoderHandle handle)
{
    BDSP_Raaga_Audio_DDTranscodeConfigParams userConfig;

    BERR_TRACE(BDSP_Raaga_GetDefaultAudioEncodeSettings(BDSP_AudioEncode_eAc3, &userConfig, sizeof(userConfig)));
    handle->ac3Settings.spdifHeaderEnabled = (userConfig.eSpdifHeaderEnable == BDSP_AF_P_eEnable)?true:false;
}

static BERR_Code BAPE_Encoder_P_ApplyDtsSettings(BAPE_EncoderHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;
    BDSP_Raaga_Audio_DtsBroadcastEncConfigParams userConfig;

    errCode = BDSP_Task_GetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    userConfig.ui32SpdifHeaderEnable = handle->dtsSettings.spdifHeaderEnabled?BDSP_AF_P_eEnable:BDSP_AF_P_eDisable;

    errCode = BDSP_Task_SetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_Encoder_P_GetDefaultDtsSettings(BAPE_EncoderHandle handle)
{
    BDSP_Raaga_Audio_DtsBroadcastEncConfigParams userConfig;

    BERR_TRACE(BDSP_Raaga_GetDefaultAudioEncodeSettings(BDSP_AudioEncode_eDTS, &userConfig, sizeof(userConfig)));
    handle->dtsSettings.spdifHeaderEnabled = (userConfig.ui32SpdifHeaderEnable == 0)?false:true;
}

static BERR_Code BAPE_Encoder_P_ApplyAacSettings(BAPE_EncoderHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;
    BDSP_Raaga_Audio_AacheEncConfigParams userConfig;
    const BAPE_AacEncodeSettings *pSettings;
    unsigned sbr;
    unsigned format;
    unsigned maxBitRate;

    switch ( handle->settings.codec )
    {
    case BAVC_AudioCompressionStd_eAacAdts:
    case BAVC_AudioCompressionStd_eAacLoas:
        pSettings = &handle->aacSettings;
        sbr=0;
        format = (handle->settings.codec == BAVC_AudioCompressionStd_eAacAdts)?1:2;
        maxBitRate = 320000;
        break;
    default:
    case BAVC_AudioCompressionStd_eAacPlusAdts:
    case BAVC_AudioCompressionStd_eAacPlusLoas:
        pSettings = &handle->aacPlusSettings;
        sbr=1;
        format = (handle->settings.codec == BAVC_AudioCompressionStd_eAacPlusAdts)?1:2;
        maxBitRate = 52000;
        break;
    }

    errCode = BDSP_Task_GetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    if ( pSettings->bitRate < 16000 || pSettings->bitRate > maxBitRate )
    {
        BDBG_ERR(("Bit Rate for codec %u (%s) must be between 16000 and %u bps.", handle->settings.codec, BAPE_P_GetCodecName(handle->settings.codec), maxBitRate));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* TODO: This will be converted to an integer in an upcoming release */
    BAPE_DSP_P_SET_VARIABLE(userConfig, eAacEncodeBitRate, (BDSP_Raaga_Audio_AacEncodeBitRate)pSettings->bitRate);

    if ( pSettings->channelMode == BAPE_ChannelMode_e1_0 ) 
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, ui32bEncodeMono, 1);
        BAPE_DSP_P_SET_VARIABLE(userConfig, eAacEncodeMonoChSelcect, (BDSP_Raaga_Audio_AacEncodeMonoChannelSelect)pSettings->monoMode);
    }
    else if ( pSettings->channelMode == BAPE_ChannelMode_e1_1 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, ui32bEncodeMono, 1);
        BAPE_DSP_P_SET_VARIABLE(userConfig, eAacEncodeMonoChSelcect, BDSP_Raaga_Audio_AacEncodeMono_DualMono);
    }
    else
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, ui32bEncodeMono, 0);
        BAPE_DSP_P_SET_VARIABLE(userConfig, eAacEncodeMonoChSelcect, BDSP_Raaga_Audio_AacEncodeMono_Mix);
    }
    BDBG_ERR(("%s: ui32bEncodeMono %d, eAacEncodeMonoChSelcect %d", __FUNCTION__, userConfig.ui32bEncodeMono, userConfig.eAacEncodeMonoChSelcect));

    BAPE_DSP_P_SET_VARIABLE(userConfig, ui32EnableSBR, sbr);
    BAPE_DSP_P_SET_VARIABLE(userConfig, ui32EncodeFormatType, format);
    BAPE_DSP_P_SET_VARIABLE(userConfig, ui3248SRC32Enable, pSettings->sampleRate == 32000 ? 1 : 0);
    if ( pSettings->complexity > (unsigned)BDSP_Raaga_Audio_AacEncodeComplexity_High )
    {
        BDBG_WRN(("AAC Complexity setting out of range.  Using highest complexity."));
        BAPE_DSP_P_SET_VARIABLE(userConfig, eAacEncodeComplexity, BDSP_Raaga_Audio_AacEncodeComplexity_High);
    }
    else
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eAacEncodeComplexity, pSettings->complexity);
    }

    /* In the case of AAC-LC set the Encode type to MPEG 2 due to compatability with certain receivers/TVs */
    switch ( handle->settings.codec )
    {
        case BAVC_AudioCompressionStd_eAacAdts:  
            BAPE_DSP_P_SET_VARIABLE(userConfig, eAacEncodeAdtsMpegType, BDSP_Raaga_Audio_AacEncodeAdtsMpeg2);
            break;    
        default:
        case BAVC_AudioCompressionStd_eAacLoas:
        case BAVC_AudioCompressionStd_eAacPlusAdts:
        case BAVC_AudioCompressionStd_eAacPlusLoas:
            BAPE_DSP_P_SET_VARIABLE(userConfig, eAacEncodeAdtsMpegType, BDSP_Raaga_Audio_AacEncodeAdtsMpeg4);
            break;
    } 

    if ( handle->settings.loudnessEquivalenceEnabled )
    {
        bool dolbySource=false;
        switch ( handle->node.deviceHandle->settings.loudnessMode )
        {
        case BAPE_LoudnessEquivalenceMode_eAtscA85:
            /* A/85 is consistent with 20dB stereo output from the decoder */
            BAPE_DSP_P_SET_VARIABLE(userConfig, i32InputVolLevel, -20);
            BAPE_DSP_P_SET_VARIABLE(userConfig, i32OutputVolLevel, -31);
            break;
        case BAPE_LoudnessEquivalenceMode_eEbuR128:
            /* EBU-R128 varies based on the input codec.  For Dolby codecs it's set to -20dB and not -23.  For MPEG/AAC-HE it's -23. */
            if ( handle->input->pParent->type == BAPE_PathNodeType_ePostProcessor &&
                 handle->input->pParent->subtype == BAPE_PostProcessorType_eDdre )
            {
                dolbySource = true;
            }
            else if ( handle->input->pParent->type == BAPE_PathNodeType_eDecoder )
            {
                BAPE_DecoderHandle hDecoder = (BAPE_DecoderHandle)handle->input->pParent->pHandle;
                switch ( hDecoder->startSettings.codec )
                {
                case BAVC_AudioCompressionStd_eAc3:
                case BAVC_AudioCompressionStd_eAc3Plus:
                #if BDSP_MS10_SUPPORT
                case BAVC_AudioCompressionStd_eAacAdts:
                case BAVC_AudioCompressionStd_eAacLoas:
                case BAVC_AudioCompressionStd_eAacPlusAdts:
                case BAVC_AudioCompressionStd_eAacPlusLoas:
                #endif
                    dolbySource = true;
                    break;
                default:
                    break;
                }
            }
            BAPE_DSP_P_SET_VARIABLE(userConfig, i32InputVolLevel, dolbySource?-20:-23);
            BAPE_DSP_P_SET_VARIABLE(userConfig, i32OutputVolLevel, -31);
            break;
        default:
            /* Use default config from FW */
            break;
        }
    }

    errCode = BDSP_Task_SetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_Encoder_P_GetDefaultAacSettings(BAPE_EncoderHandle handle)
{
    BDSP_Raaga_Audio_AacheEncConfigParams userConfig;

    BERR_TRACE(BDSP_Raaga_GetDefaultAudioEncodeSettings(BDSP_AudioEncode_eAacHe, &userConfig, sizeof(userConfig)));
    handle->aacSettings.bitRate = (unsigned)userConfig.eAacEncodeBitRate;
    BDBG_CASSERT(BDSP_Raaga_Audio_AacEncodeBitRate_e16kbps == 16000);  /* Just in case they ever change the rate enum */
    if ( userConfig.ui32bEncodeMono )
    {
        if ( userConfig.eAacEncodeMonoChSelcect == BDSP_Raaga_Audio_AacEncodeMono_DualMono )
        {
            handle->aacSettings.channelMode = BAPE_ChannelMode_e1_1;
            handle->aacSettings.monoMode = BAPE_MonoChannelMode_eMix;
        }
        else
        {
            handle->aacSettings.channelMode = BAPE_ChannelMode_e1_0;
            handle->aacSettings.monoMode = (BAPE_MonoChannelMode)userConfig.eAacEncodeMonoChSelcect;
        }        
    }
    else
    {
        handle->aacSettings.channelMode = BAPE_ChannelMode_e2_0;
        handle->aacSettings.monoMode = BAPE_MonoChannelMode_eMix;
    }
    BDBG_CASSERT((int)BDSP_Raaga_Audio_AacEncodeMono_Left==(int)BAPE_MonoChannelMode_eLeft);
    BDBG_CASSERT((int)BDSP_Raaga_Audio_AacEncodeMono_Right==(int)BAPE_MonoChannelMode_eRight);
    BDBG_CASSERT((int)BDSP_Raaga_Audio_AacEncodeMono_Mix==(int)BAPE_MonoChannelMode_eMix);
    handle->aacSettings.sampleRate = 0;
    handle->aacSettings.complexity = userConfig.eAacEncodeComplexity;
    handle->aacPlusSettings = handle->aacSettings;
    handle->aacSettings.bitRate = 128000;   /* Default above is only usable for AAC-HE.  This is suitable for LC. */
}

static BERR_Code BAPE_Encoder_P_ApplyMp3Settings(BAPE_EncoderHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;
    BDSP_Raaga_Audio_Mpeg1L3EncConfigParams userConfig;

    errCode = BDSP_Task_GetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    if ( handle->mp3Settings.bitRate < 32000 || handle->mp3Settings.bitRate > 320000 )
    {
        BDBG_ERR(("Bit Rate for codec %u (%s) must be between 32000 and 320000 bps.", handle->settings.codec, BAPE_P_GetCodecName(handle->settings.codec)));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Make the bitrate an appropriate value */
    if ( handle->mp3Settings.bitRate < 40000 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeBitRate, BDSP_Raaga_Audio_Mp3EncodeBitRate_e32kbps);
    }
    else if ( handle->mp3Settings.bitRate < 48000 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeBitRate, BDSP_Raaga_Audio_Mp3EncodeBitRate_e40kbps);
    }
    else if ( handle->mp3Settings.bitRate < 56000 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeBitRate, BDSP_Raaga_Audio_Mp3EncodeBitRate_e48kbps);
    }
    else if ( handle->mp3Settings.bitRate < 64000 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeBitRate, BDSP_Raaga_Audio_Mp3EncodeBitRate_e56kbps);
    }
    else if ( handle->mp3Settings.bitRate < 80000 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeBitRate, BDSP_Raaga_Audio_Mp3EncodeBitRate_e64kbps);
    }
    else if ( handle->mp3Settings.bitRate < 96000 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeBitRate, BDSP_Raaga_Audio_Mp3EncodeBitRate_e80kbps);
    }
    else if ( handle->mp3Settings.bitRate < 112000 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeBitRate, BDSP_Raaga_Audio_Mp3EncodeBitRate_e96kbps);
    }
    else if ( handle->mp3Settings.bitRate < 128000 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeBitRate, BDSP_Raaga_Audio_Mp3EncodeBitRate_e112kbps);
    }
    else if ( handle->mp3Settings.bitRate < 160000 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeBitRate, BDSP_Raaga_Audio_Mp3EncodeBitRate_e128kbps);
    }
    else if ( handle->mp3Settings.bitRate < 192000 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeBitRate, BDSP_Raaga_Audio_Mp3EncodeBitRate_e160kbps);
    }
    else if ( handle->mp3Settings.bitRate < 224000 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeBitRate, BDSP_Raaga_Audio_Mp3EncodeBitRate_e192kbps);
    }
    else if ( handle->mp3Settings.bitRate < 256000 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeBitRate, BDSP_Raaga_Audio_Mp3EncodeBitRate_e224kbps);
    }
    else if ( handle->mp3Settings.bitRate < 320000 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeBitRate, BDSP_Raaga_Audio_Mp3EncodeBitRate_e256kbps);
    }
    else
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeBitRate, BDSP_Raaga_Audio_Mp3EncodeBitRate_e320kbps);
    }

    BAPE_DSP_P_SET_VARIABLE(userConfig, ePrivateBit, handle->mp3Settings.privateBit?BDSP_AF_P_eEnable:BDSP_AF_P_eDisable);
    BAPE_DSP_P_SET_VARIABLE(userConfig, eCopyright, handle->mp3Settings.copyrightBit?BDSP_AF_P_eEnable:BDSP_AF_P_eDisable);
    BAPE_DSP_P_SET_VARIABLE(userConfig, eOriginal, handle->mp3Settings.originalBit?BDSP_AF_P_eEnable:BDSP_AF_P_eDisable);
    BAPE_DSP_P_SET_VARIABLE(userConfig, eEmphasisType, (BDSP_Raaga_Audio_Mp3EncodeEmphasisType)handle->mp3Settings.emphasisMode);
    if ( handle->mp3Settings.channelMode == BAPE_ChannelMode_e1_0 ) 
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, ui32bEncodeMono, 1);
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeMonoChannelSelect, (BDSP_Raaga_Audio_Mp3EncodeMonoChannelSelect)handle->mp3Settings.monoMode);
    }
    else if ( handle->mp3Settings.channelMode == BAPE_ChannelMode_e1_1 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, ui32bEncodeMono, 1);
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeMonoChannelSelect, BDSP_Raaga_Audio_Mp3EncodeMono_DualMono);
    }
    else
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, ui32bEncodeMono, 0);
        BAPE_DSP_P_SET_VARIABLE(userConfig, eMp3EncodeMonoChannelSelect, BDSP_Raaga_Audio_Mp3EncodeMono_Mix);
    }

    BDBG_ERR(("%s: ui32bEncodeMono %d, eMp3EncodeMonoChannelSelect %d", __FUNCTION__, userConfig.ui32bEncodeMono, userConfig.eMp3EncodeMonoChannelSelect));

    errCode = BDSP_Task_SetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_Encoder_P_GetDefaultMp3Settings(BAPE_EncoderHandle handle)
{
    BDSP_Raaga_Audio_Mpeg1L3EncConfigParams userConfig;

    BERR_TRACE(BDSP_Raaga_GetDefaultAudioEncodeSettings(BDSP_AudioEncode_eMpeg1Layer3, &userConfig, sizeof(userConfig)));
    handle->mp3Settings.bitRate = (unsigned)userConfig.eMp3EncodeBitRate * 1000;
    BDBG_CASSERT(BDSP_Raaga_Audio_Mp3EncodeBitRate_e32kbps == 32);  /* Just in case they ever change the rate enum */
    handle->mp3Settings.privateBit = (userConfig.ePrivateBit == BDSP_AF_P_eEnable)?true:false;
    handle->mp3Settings.copyrightBit = (userConfig.eCopyright == BDSP_AF_P_eEnable)?true:false;
    handle->mp3Settings.originalBit = (userConfig.eOriginal == BDSP_AF_P_eEnable)?true:false;
    handle->mp3Settings.emphasisMode = (BAPE_MpegEmphasisMode)userConfig.eEmphasisType;
    BDBG_CASSERT((int)BDSP_Raaga_Audio_Mp3EncodeEmphasisType_eNone==(int)BAPE_MpegEmphasisMode_eNone);
    BDBG_CASSERT((int)BDSP_Raaga_Audio_Mp3EncodeEmphasisType_e50_15uSeconds==(int)BAPE_MpegEmphasisMode_e50_15ms);
    BDBG_CASSERT((int)BDSP_Raaga_Audio_Mp3EncodeEmphasisType_eReserved==(int)BAPE_MpegEmphasisMode_eReserved);
    BDBG_CASSERT((int)BDSP_Raaga_Audio_Mp3EncodeEmphasisType_eCCITTJ_17==(int)BAPE_MpegEmphasisMode_eCcit_J17);
    if ( userConfig.ui32bEncodeMono )
    {
        if ( userConfig.eMp3EncodeMonoChannelSelect == BDSP_Raaga_Audio_Mp3EncodeMono_DualMono )
        {
            handle->mp3Settings.channelMode = BAPE_ChannelMode_e1_1;
            handle->mp3Settings.monoMode = BAPE_MonoChannelMode_eMix; /* unused in this mode, set to default */
        }
        else
        {
            handle->mp3Settings.channelMode = BAPE_ChannelMode_e1_0;
            handle->mp3Settings.monoMode = (BAPE_MonoChannelMode)userConfig.eMp3EncodeMonoChannelSelect;
        }        
    }
    else
    {
        handle->aacSettings.channelMode = BAPE_ChannelMode_e2_0;
        handle->aacSettings.monoMode = BAPE_MonoChannelMode_eMix;
    }
    BDBG_CASSERT((int)BDSP_Raaga_Audio_Mp3EncodeMono_Left==(int)BAPE_MonoChannelMode_eLeft);
    BDBG_CASSERT((int)BDSP_Raaga_Audio_Mp3EncodeMono_Right==(int)BAPE_MonoChannelMode_eRight);
    BDBG_CASSERT((int)BDSP_Raaga_Audio_Mp3EncodeMono_Mix==(int)BAPE_MonoChannelMode_eMix);

    if ( userConfig.ui32bEncodeMono )
    {
        handle->mp3Settings.channelMode = (userConfig.eMp3EncodeMonoChannelSelect == BDSP_Raaga_Audio_Mp3EncodeMono_DualMono)? BAPE_ChannelMode_e1_1:BAPE_ChannelMode_e1_0;
        handle->mp3Settings.monoMode = (userConfig.eMp3EncodeMonoChannelSelect == BDSP_Raaga_Audio_Mp3EncodeMono_DualMono)?BAPE_MonoChannelMode_eMix:(BAPE_MonoChannelMode)userConfig.eMp3EncodeMonoChannelSelect;
    }
    else
    {
        handle->mp3Settings.channelMode = BAPE_ChannelMode_e2_0;
        handle->mp3Settings.monoMode = BAPE_MonoChannelMode_eMix;
    }
}

static BERR_Code BAPE_Encoder_P_ApplyWmaSettings(BAPE_EncoderHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;
    BDSP_Raaga_Audio_WMAEncoderConfigParams userConfig;

    errCode = BDSP_Task_GetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_DSP_P_SET_VARIABLE(userConfig, ui32Bitrate, handle->wmaSettings.bitRate);
    BAPE_DSP_P_SET_VARIABLE(userConfig, ui32DownMixToMono, (handle->wmaSettings.channelMode == BAPE_ChannelMode_e1_0) ? 1 : 0);

    errCode = BDSP_Task_SetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_Encoder_P_GetDefaultWmaSettings(BAPE_EncoderHandle handle)
{
    BDSP_Raaga_Audio_WMAEncoderConfigParams userConfig;

    BERR_TRACE(BDSP_Raaga_GetDefaultAudioEncodeSettings(BDSP_AudioEncode_eWma, &userConfig, sizeof(userConfig)));
    handle->wmaSettings.bitRate = userConfig.ui32Bitrate;
    handle->wmaSettings.channelMode = (userConfig.ui32DownMixToMono)?BAPE_ChannelMode_e1_0:BAPE_ChannelMode_e2_0;
}

static BERR_Code BAPE_Encoder_P_ApplyG711G726Settings(BAPE_EncoderHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;
    BDSP_Raaga_Audio_G711_G726EncConfigParams userConfig;

    errCode = BDSP_Task_GetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    if ( handle->settings.codec == BAVC_AudioCompressionStd_eG711 )
    {
        /* G.711 */
        BAPE_DSP_P_SET_VARIABLE(userConfig, eCompressionType, (handle->g711Settings.compressionMode == BAPE_G711G726CompressionMode_eUlaw)?BDSP_Raaga_Audio_eCompressionType_uLaw_disableG726:BDSP_Raaga_Audio_eCompressionType_aLaw_disableG726);
        BAPE_DSP_P_SET_VARIABLE(userConfig, eBitRate, BDSP_Raaga_Audio_eBitRate_64kbps);
    }
    else
    {
        /* G.726 */
        BAPE_DSP_P_SET_VARIABLE(userConfig, eCompressionType, (handle->g726Settings.compressionMode == BAPE_G711G726CompressionMode_eUlaw)?BDSP_Raaga_Audio_eCompressionType_uLaw_G726:BDSP_Raaga_Audio_eCompressionType_aLaw_G726);
        if ( handle->g726Settings.bitRate <= 16*1024 )
        {
            BAPE_DSP_P_SET_VARIABLE(userConfig, eBitRate, BDSP_Raaga_Audio_eBitRate_16kbps);
        }
        else if ( handle->g726Settings.bitRate <= 24*1024 )
        {
            BAPE_DSP_P_SET_VARIABLE(userConfig, eBitRate, BDSP_Raaga_Audio_eBitRate_24kbps);
        }
        else if ( handle->g726Settings.bitRate <= 32*1024 )
        {
            BAPE_DSP_P_SET_VARIABLE(userConfig, eBitRate, BDSP_Raaga_Audio_eBitRate_32kbps);
        }
        else
        {
            BAPE_DSP_P_SET_VARIABLE(userConfig, eBitRate, BDSP_Raaga_Audio_eBitRate_40kbps);
        }
    }

    errCode = BDSP_Task_SetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_Encoder_P_GetDefaultG711G726Settings(BAPE_EncoderHandle handle)
{
    BDSP_Raaga_Audio_G711_G726EncConfigParams userConfig;

    BERR_TRACE(BDSP_Raaga_GetDefaultAudioEncodeSettings(BDSP_AudioEncode_eG711G726, &userConfig, sizeof(userConfig)));

    switch ( userConfig.eCompressionType )
    {
    case BDSP_Raaga_Audio_eCompressionType_uLaw_G726:
    case BDSP_Raaga_Audio_eCompressionType_uLaw_ext:
    case BDSP_Raaga_Audio_eCompressionType_uLaw_disableG726:
        handle->g711Settings.compressionMode = BAPE_G711G726CompressionMode_eUlaw;
        break;
    default:
        handle->g711Settings.compressionMode = BAPE_G711G726CompressionMode_eAlaw;
        break;
    }
    handle->g726Settings.compressionMode = handle->g711Settings.compressionMode;

    switch ( userConfig.eBitRate )
    {
    case BDSP_Raaga_Audio_eBitRate_16kbps:
        handle->g726Settings.bitRate = 16000;
        break;
    case BDSP_Raaga_Audio_eBitRate_24kbps:
        handle->g726Settings.bitRate = 24000;
        break;
    default:
    case BDSP_Raaga_Audio_eBitRate_32kbps:
        handle->g726Settings.bitRate = 24000;
        break;
    case BDSP_Raaga_Audio_eBitRate_40kbps:
        handle->g726Settings.bitRate = 24000;
        break;
    }
}

static BERR_Code BAPE_Encoder_P_ApplyG729Settings(BAPE_EncoderHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;
    BDSP_Raaga_Audio_G729EncoderUserConfig userConfig;

    errCode = BDSP_Task_GetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_DSP_P_SET_VARIABLE(userConfig, ui32DtxEnable, handle->g729Settings.dtxEnabled ? 1 : 0);
    if ( handle->g729Settings.bitRate <= 6554 ) /* 6.4 * 1024 */
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, ui32Bitrate, 0);
    }
    else if ( handle->g729Settings.bitRate <= 8*1024 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, ui32Bitrate, 1);
    }
    else
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, ui32Bitrate, 2);
    }

    errCode = BDSP_Task_SetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_Encoder_P_GetDefaultG729Settings(BAPE_EncoderHandle handle)
{
    BDSP_Raaga_Audio_G729EncoderUserConfig userConfig;

    BERR_TRACE(BDSP_Raaga_GetDefaultAudioEncodeSettings(BDSP_AudioEncode_eG729, &userConfig, sizeof(userConfig)));

    handle->g729Settings.dtxEnabled = userConfig.ui32DtxEnable ? true : false;
    switch ( userConfig.ui32Bitrate )
    {
    default:
    case 0:
        handle->g729Settings.bitRate = 6400;
        break;
    case 1:
        handle->g729Settings.bitRate = 8000;
        break;
    case 2:
        handle->g729Settings.bitRate = 11800;
        break;
    }
}

static BERR_Code BAPE_Encoder_P_ApplyG723_1Settings(BAPE_EncoderHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BERR_Code errCode;
    BDSP_Raaga_Audio_G723EncoderUserConfig userConfig;

    errCode = BDSP_Task_GetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    BAPE_DSP_P_SET_VARIABLE(userConfig, UseVx, handle->g723_1Settings.vadEnabled ? 1 : 0);
    BAPE_DSP_P_SET_VARIABLE(userConfig, UseHp, handle->g723_1Settings.hpfEnabled ? 1 : 0);
    if ( handle->g729Settings.bitRate < 6300 )
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, WrkRate, 0);
    }
    else
    {
        BAPE_DSP_P_SET_VARIABLE(userConfig, WrkRate, 1);
    }

    errCode = BDSP_Task_SetStageSettings(task, branchId, stageId, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static void BAPE_Encoder_P_GetDefaultG723_1Settings(BAPE_EncoderHandle handle)
{
    BDSP_Raaga_Audio_G723EncoderUserConfig userConfig;

    BERR_TRACE(BDSP_Raaga_GetDefaultAudioEncodeSettings(BDSP_AudioEncode_eG723_1, &userConfig, sizeof(userConfig)));

    handle->g723_1Settings.vadEnabled = userConfig.UseVx ? true : false;
    handle->g723_1Settings.hpfEnabled = userConfig.UseHp ? true : false;
    switch ( userConfig.WrkRate )
    {
    default:
    case 0:
        handle->g723_1Settings.bitRate = 6300;
        break;
    case 1:
        handle->g723_1Settings.bitRate = 5300;
        break;
    }
}

static BERR_Code BAPE_Encoder_P_ApplyDspSettings(BAPE_EncoderHandle handle, BDSP_TaskHandle task, unsigned branchId, unsigned stageId)
{
    BDSP_AudioEncode encodeType;
    BDBG_OBJECT_ASSERT(handle, BAPE_Encoder);
    BDBG_ASSERT(NULL != task);
    encodeType = BAPE_P_GetCodecAudioEncode(handle->settings.codec);
    switch ( encodeType )
    {
    case BDSP_AudioEncode_eAc3:
        return BAPE_Encoder_P_ApplyAc3Settings(handle, task, branchId, stageId);
    case BDSP_AudioEncode_eDTS:
        return BAPE_Encoder_P_ApplyDtsSettings(handle, task, branchId, stageId);
    case BDSP_AudioEncode_eAacHe:
        return BAPE_Encoder_P_ApplyAacSettings(handle, task, branchId, stageId);
    case BDSP_AudioEncode_eMpeg1Layer3:
        return BAPE_Encoder_P_ApplyMp3Settings(handle, task, branchId, stageId);
    case BDSP_AudioEncode_eWma:
        return BAPE_Encoder_P_ApplyWmaSettings(handle, task, branchId, stageId);
    case BDSP_AudioEncode_eG711G726:
        return BAPE_Encoder_P_ApplyG711G726Settings(handle, task, branchId, stageId);
    case BDSP_AudioEncode_eG729:
        return BAPE_Encoder_P_ApplyG729Settings(handle, task, branchId, stageId);
    case BDSP_AudioEncode_eG723_1:
        return BAPE_Encoder_P_ApplyG723_1Settings(handle, task, branchId, stageId);
    default:
        return BERR_SUCCESS;
    }
}

static void BAPE_Encoder_P_GetDefaultCodecSettings(BAPE_EncoderHandle handle)
{
    BAPE_Encoder_P_GetDefaultAc3Settings(handle);
    BAPE_Encoder_P_GetDefaultDtsSettings(handle);
    BAPE_Encoder_P_GetDefaultAacSettings(handle);
    BAPE_Encoder_P_GetDefaultMp3Settings(handle);
    BAPE_Encoder_P_GetDefaultWmaSettings(handle);
    BAPE_Encoder_P_GetDefaultG711G726Settings(handle);
    BAPE_Encoder_P_GetDefaultG729Settings(handle);
    BAPE_Encoder_P_GetDefaultG723_1Settings(handle);
}

static void BAPE_Encoder_P_RemoveInputCallback(struct BAPE_PathNode *pNode, BAPE_PathConnector *pConnector)
{
    (void)BAPE_Encoder_RemoveInput(pNode->pHandle, pConnector);
}

