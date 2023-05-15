/***************************************************************************
*     (c)2004-2013 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_encoder.c $
* $brcm_Revision: SW7425-4386/2 $
* $brcm_Date: 5/16/13 5:52p $
*
* API Description:
*   API name: DtsEncode
*    Specific APIs related to DTS Audio Encoding
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/ape_raaga/nexus_audio_encoder.c $
* 
* SW7425-4386/2   5/16/13 5:52p mpeteri
* SW7425-4386: revisions per NEXUS API CR
* 
* SW7425-4386/1   5/14/13 4:52p mpeteri
* SW7425-4386: initial changes for MP3/AAC mono encode
* 
* 21   9/11/12 11:28a jgarrett
* SW7425-3795: Converting complexity to enum
* 
* 20   9/4/12 10:48a jgarrett
* SW7425-3795: Exposing AAC complexity setting
* 
* 19   7/25/12 5:10p jgarrett
* SW7425-3125: Propagating sample rate setting in
*  NEXUS_AudioEncoder_SetSettings
* 
* 18   7/20/12 4:59p jgarrett
* SW7425-3115: Adding output sample rate selection
* 
* 17   6/19/12 12:41p vsilyaev
* SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
* 
* 16   2/13/12 11:43a jgarrett
* SW7231-654: Coverity CID 36800
* 
* 15   2/1/12 3:10p jgarrett
* SW7425-2268: Adding initial voice conferencing support
* 
* 14   12/20/11 4:18p jgarrett
* SW7425-1018: Propagating loudness equivalence setting to APE
* 
* 13   12/20/11 4:13p jgarrett
* SW7425-1018: Adding initial A/85 implementation
* 
* 12   12/9/11 11:18a jgarrett
* SW7425-1478: Adding WMA encoding
* 
* 11   8/15/11 2:03p jgarrett
* SW7346-443: Coverity CID 34599
* 
* 10   7/29/11 5:38p jgarrett
* SW7346-357: coverity cid 33741
* 
* 9   6/16/11 5:26p jgarrett
* SW7425-741: Adding MP3 encoder
* 
* 8   4/26/11 11:35a jgarrett
* SW7425-437: Resolving kernel mode shutdown issues
* 
* 7   4/25/11 9:45p jgarrett
* SW7425-437: Fixing shutdown crash if still connected or running
* 
* 6   3/28/11 5:34a jgarrett
* SW7422-146:  Fixing RemoveAllInputs
* 
* 5   3/21/11 7:04p jgarrett
* SW7422-355: Adding AAC encoders
* 
* 4   3/1/11 7:25p jgarrett
* SW7422-146: Adding DTS/AC3 encoders
* 
* 3   2/22/11 5:44p jgarrett
* SW7422-146: Implemented type renaming based on filter graph review
*  comments
* 
* 2   1/13/11 5:25p jgarrett
* SW7422-146: Adding encoder hooks
* 
* 1   1/10/11 3:26p jgarrett
* SW7422-146: Adding full nexus API set
* 
***************************************************************************/

#include "nexus_audio_module.h"

BDBG_MODULE(nexus_audio_encoder);

/***************************************************************************
Summary:
    Get default settings for an Audio Encoder stage
***************************************************************************/
void NEXUS_AudioEncoder_GetDefaultSettings(
    NEXUS_AudioEncoderSettings *pSettings   /* [out] default settings */
    )
{
    BAPE_EncoderSettings encoderSettings;

    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));

    BAPE_Encoder_GetDefaultSettings(&encoderSettings);
    pSettings->codec = NEXUS_Audio_P_MagnumToCodec(encoderSettings.codec);
    pSettings->loudnessEquivalenceEnabled = encoderSettings.loudnessEquivalenceEnabled;
}

/***************************************************************************
Summary:
    Open an Audio Encoder stage
***************************************************************************/
NEXUS_AudioEncoderHandle NEXUS_AudioEncoder_Open( /* attr{destructor=NEXUS_AudioEncoder_Close}  */
    const NEXUS_AudioEncoderSettings *pSettings     /* Pass NULL for default settings */
    )
{
    NEXUS_Error errCode;
    NEXUS_AudioEncoderHandle handle;
    BAPE_EncoderSettings encoderSettings;
    BAPE_Connector path;
    NEXUS_AudioEncoderSettings defaults;
    
    if ( NULL == pSettings )
    {
        NEXUS_AudioEncoder_GetDefaultSettings(&defaults);
        pSettings = &defaults;
    }

    handle = BKNI_Malloc(sizeof(NEXUS_AudioEncoder));
    if ( NULL == handle )
    {
        (void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_malloc;
    }

    NEXUS_OBJECT_INIT(NEXUS_AudioEncoder, handle);
    
    BAPE_Encoder_GetDefaultSettings(&encoderSettings);
    encoderSettings.codec = NEXUS_Audio_P_CodecToMagnum(pSettings->codec);
    errCode = BAPE_Encoder_Create(NEXUS_AUDIO_DEVICE_HANDLE,
                                  &encoderSettings,
                                  &handle->encoder);
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        goto err_create;
    }

    NEXUS_AUDIO_INPUT_INIT(&handle->connector, NEXUS_AudioInputType_eEncoder, handle);
    handle->connector.format = NEXUS_AudioInputFormat_eCompressed;
    BAPE_Encoder_GetConnector(handle->encoder, &path);
    handle->connector.port = (uint32_t)path;
    
    /* success */
    return handle;

err_create:
    NEXUS_OBJECT_DESTROY(NEXUS_AudioEncoder, handle);
    BKNI_Free(handle);
err_malloc:
    return NULL;
}

/***************************************************************************
Summary:
    Close an DTS Encode stage
    
Description:
    Input to the stage must be removed prior to closing.
***************************************************************************/
static void NEXUS_AudioEncoder_P_Finalizer(
    NEXUS_AudioEncoderHandle handle
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_AudioEncoder, handle);
    NEXUS_AudioInput_Shutdown(&handle->connector);
    BAPE_Encoder_Destroy(handle->encoder);
    NEXUS_OBJECT_DESTROY(NEXUS_AudioEncoder, handle);
    BKNI_Free(handle);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_AudioEncoder, NEXUS_AudioEncoder_Close);

/***************************************************************************
Summary:
    Get Settings for an DTS Encode stage
***************************************************************************/
void NEXUS_AudioEncoder_GetSettings(
    NEXUS_AudioEncoderHandle handle,
    NEXUS_AudioEncoderSettings *pSettings    /* [out] Settings */
    )
{
    BAPE_EncoderSettings settings;
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEncoder);
    BDBG_ASSERT(NULL != pSettings);

    BAPE_Encoder_GetSettings(handle->encoder, &settings);
    pSettings->codec = NEXUS_Audio_P_MagnumToCodec(settings.codec);
    pSettings->loudnessEquivalenceEnabled = settings.loudnessEquivalenceEnabled;
}


/***************************************************************************
Summary:
    Set Settings for an DTS Encode stage
***************************************************************************/
NEXUS_Error NEXUS_AudioEncoder_SetSettings(
    NEXUS_AudioEncoderHandle handle,
    const NEXUS_AudioEncoderSettings *pSettings
    )
{
    BAPE_EncoderSettings settings;
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEncoder);
    BDBG_ASSERT(NULL != pSettings);

    BAPE_Encoder_GetSettings(handle->encoder, &settings);
    settings.codec = NEXUS_Audio_P_CodecToMagnum(pSettings->codec);
    settings.loudnessEquivalenceEnabled = pSettings->loudnessEquivalenceEnabled;
    errCode = BAPE_Encoder_SetSettings(handle->encoder, &settings);
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
void NEXUS_AudioEncoder_GetCodecSettings(
    NEXUS_AudioEncoderHandle handle,
    NEXUS_AudioCodec codec, /* the codec for which you are retrieving settings. */
    NEXUS_AudioEncoderCodecSettings *pSettings    /* [out] Settings */
    )
{
    BAVC_AudioCompressionStd magnumCodec;
    BAPE_EncoderCodecSettings codecSettings;
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEncoder);
    BDBG_ASSERT(NULL != pSettings);
    pSettings->codec = codec;
    magnumCodec = NEXUS_Audio_P_CodecToMagnum(codec);
    BAPE_Encoder_GetCodecSettings(handle->encoder, magnumCodec, &codecSettings);
    switch ( codec )
    {
    case NEXUS_AudioCodec_eAc3:
        pSettings->codecSettings.ac3.spdifHeaderEnabled = codecSettings.codecSettings.ac3.spdifHeaderEnabled;
        break;
    case NEXUS_AudioCodec_eDts:
        pSettings->codecSettings.dts.spdifHeaderEnabled = codecSettings.codecSettings.dts.spdifHeaderEnabled;
        break;
    case NEXUS_AudioCodec_eAacAdts:
    case NEXUS_AudioCodec_eAacLoas:
        pSettings->codecSettings.aac.bitRate = codecSettings.codecSettings.aac.bitRate;
        pSettings->codecSettings.aac.outputMode = (codecSettings.codecSettings.aac.channelMode == BAPE_ChannelMode_e1_0)?NEXUS_AudioMode_e1_0:(codecSettings.codecSettings.aac.channelMode == BAPE_ChannelMode_e1_1)?NEXUS_AudioMode_e1_1:NEXUS_AudioMode_e2_0;
        pSettings->codecSettings.aac.monoMode = (NEXUS_AudioMonoChannelMode)codecSettings.codecSettings.aac.monoMode;
        pSettings->codecSettings.aac.sampleRate = codecSettings.codecSettings.aac.sampleRate;
        pSettings->codecSettings.aac.complexity = (NEXUS_AacEncodeComplexity)codecSettings.codecSettings.aac.complexity;
        break;
    case NEXUS_AudioCodec_eAacPlusAdts:
    case NEXUS_AudioCodec_eAacPlusLoas:
        pSettings->codecSettings.aacPlus.bitRate = codecSettings.codecSettings.aacPlus.bitRate;
        pSettings->codecSettings.aacPlus.outputMode = (codecSettings.codecSettings.aacPlus.channelMode == BAPE_ChannelMode_e1_0)?NEXUS_AudioMode_e1_0:(codecSettings.codecSettings.aacPlus.channelMode == BAPE_ChannelMode_e1_1)?NEXUS_AudioMode_e1_1:NEXUS_AudioMode_e2_0;
        pSettings->codecSettings.aacPlus.monoMode = (NEXUS_AudioMonoChannelMode)codecSettings.codecSettings.aacPlus.monoMode;
        BDBG_CASSERT((int)NEXUS_AudioMonoChannelMode_eLeft==(int)BAPE_MonoChannelMode_eLeft);
        BDBG_CASSERT((int)NEXUS_AudioMonoChannelMode_eRight==(int)BAPE_MonoChannelMode_eRight);
        BDBG_CASSERT((int)NEXUS_AudioMonoChannelMode_eMix==(int)BAPE_MonoChannelMode_eMix);
        pSettings->codecSettings.aacPlus.sampleRate = codecSettings.codecSettings.aacPlus.sampleRate;
        BDBG_CASSERT((int)NEXUS_AacEncodeComplexity_eLowest == (int)BAPE_AacEncodeComplexity_eLowest);
        BDBG_CASSERT((int)NEXUS_AacEncodeComplexity_eMediumLow == (int)BAPE_AacEncodeComplexity_eMediumLow);
        BDBG_CASSERT((int)NEXUS_AacEncodeComplexity_eMediumHigh == (int)BAPE_AacEncodeComplexity_eMediumHigh);
        BDBG_CASSERT((int)NEXUS_AacEncodeComplexity_eHighest == (int)BAPE_AacEncodeComplexity_eHighest);
        BDBG_CASSERT((int)NEXUS_AacEncodeComplexity_eMax == (int)BAPE_AacEncodeComplexity_eMax);
        pSettings->codecSettings.aacPlus.complexity = (NEXUS_AacEncodeComplexity)codecSettings.codecSettings.aacPlus.complexity;
        break;
    case NEXUS_AudioCodec_eMp3:
        pSettings->codecSettings.mp3.bitRate = codecSettings.codecSettings.mp3.bitRate;
        pSettings->codecSettings.mp3.privateBit = codecSettings.codecSettings.mp3.privateBit;
        pSettings->codecSettings.mp3.copyrightBit = codecSettings.codecSettings.mp3.copyrightBit;
        pSettings->codecSettings.mp3.originalBit = codecSettings.codecSettings.mp3.originalBit;
        pSettings->codecSettings.mp3.emphasis = (NEXUS_AudioMpegEmphasis)codecSettings.codecSettings.mp3.emphasisMode;
        BDBG_CASSERT((int)NEXUS_AudioMpegEmphasis_eMax==(int)BAPE_MpegEmphasisMode_eMax);
        pSettings->codecSettings.mp3.outputMode = (codecSettings.codecSettings.mp3.channelMode == BAPE_ChannelMode_e1_0)?NEXUS_AudioMode_e1_0:(codecSettings.codecSettings.mp3.channelMode == BAPE_ChannelMode_e1_1)?NEXUS_AudioMode_e1_1:NEXUS_AudioMode_e2_0;
        pSettings->codecSettings.mp3.monoMode = (NEXUS_AudioMonoChannelMode)codecSettings.codecSettings.mp3.monoMode;
        break;
    case NEXUS_AudioCodec_eWmaStd:
        pSettings->codecSettings.wmaStd.bitRate = codecSettings.codecSettings.wmaStd.bitRate;
        pSettings->codecSettings.wmaStd.monoEncoding = (codecSettings.codecSettings.wmaStd.channelMode == BAPE_ChannelMode_e1_0)?true:false;
        break;
    case NEXUS_AudioCodec_eG711:
        pSettings->codecSettings.g711.compressionMode = (NEXUS_G711G726CompressionMode)codecSettings.codecSettings.g711.compressionMode;
        BDBG_CASSERT(NEXUS_G711G726CompressionMode_eUlaw == (int)BAPE_G711G726CompressionMode_eUlaw);
        BDBG_CASSERT(NEXUS_G711G726CompressionMode_eAlaw == (int)BAPE_G711G726CompressionMode_eAlaw);
        BDBG_CASSERT(NEXUS_G711G726CompressionMode_eMax == (int)BAPE_G711G726CompressionMode_eMax);
        break;
    case NEXUS_AudioCodec_eG726:
        pSettings->codecSettings.g726.compressionMode = (NEXUS_G711G726CompressionMode)codecSettings.codecSettings.g726.compressionMode;
        pSettings->codecSettings.g726.bitRate = codecSettings.codecSettings.g726.bitRate;
        break;
    case NEXUS_AudioCodec_eG729:
        pSettings->codecSettings.g729.dtxEnabled = codecSettings.codecSettings.g729.dtxEnabled;
        pSettings->codecSettings.g729.bitRate = codecSettings.codecSettings.g729.bitRate;
        break;
    case NEXUS_AudioCodec_eG723_1:
        pSettings->codecSettings.g723_1.vadEnabled = codecSettings.codecSettings.g723_1.vadEnabled;
        pSettings->codecSettings.g723_1.hpfEnabled = codecSettings.codecSettings.g723_1.hpfEnabled;
        pSettings->codecSettings.g723_1.bitRate = codecSettings.codecSettings.g723_1.bitRate;
        break;
    default:
        break;
    }
}


/***************************************************************************
Summary:
    Set Codec-Specific Settings for an Audio Encoder stage
***************************************************************************/
NEXUS_Error NEXUS_AudioEncoder_SetCodecSettings(
    NEXUS_AudioEncoderHandle handle,
    const NEXUS_AudioEncoderCodecSettings *pSettings
    )
{
    BAVC_AudioCompressionStd magnumCodec;
    BAPE_EncoderCodecSettings codecSettings;
    BERR_Code errCode;
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEncoder);
    BDBG_ASSERT(NULL != pSettings);
    magnumCodec = NEXUS_Audio_P_CodecToMagnum(pSettings->codec);
    BAPE_Encoder_GetCodecSettings(handle->encoder, magnumCodec, &codecSettings);
    switch ( pSettings->codec )
    {
    case NEXUS_AudioCodec_eAc3:
        codecSettings.codecSettings.ac3.spdifHeaderEnabled = pSettings->codecSettings.ac3.spdifHeaderEnabled;
        break;
    case NEXUS_AudioCodec_eDts:
        codecSettings.codecSettings.dts.spdifHeaderEnabled = pSettings->codecSettings.dts.spdifHeaderEnabled;
        break;
    case NEXUS_AudioCodec_eAacAdts:
    case NEXUS_AudioCodec_eAacLoas:
        codecSettings.codecSettings.aac.bitRate = pSettings->codecSettings.aac.bitRate;
        codecSettings.codecSettings.aac.channelMode = (pSettings->codecSettings.aac.outputMode == NEXUS_AudioMode_e1_0)?BAPE_ChannelMode_e1_0:(pSettings->codecSettings.aac.outputMode == NEXUS_AudioMode_e1_1)?BAPE_ChannelMode_e1_1:BAPE_ChannelMode_e2_0;
        codecSettings.codecSettings.aac.monoMode = (BAPE_MonoChannelMode)pSettings->codecSettings.aac.monoMode;
        codecSettings.codecSettings.aac.sampleRate = pSettings->codecSettings.aac.sampleRate;
        codecSettings.codecSettings.aac.complexity = (BAPE_AacEncodeComplexity)pSettings->codecSettings.aac.complexity;
        break;
    case NEXUS_AudioCodec_eAacPlusAdts:
    case NEXUS_AudioCodec_eAacPlusLoas:
        codecSettings.codecSettings.aacPlus.bitRate = pSettings->codecSettings.aacPlus.bitRate;
        codecSettings.codecSettings.aacPlus.channelMode = (pSettings->codecSettings.aacPlus.outputMode == NEXUS_AudioMode_e1_0)?BAPE_ChannelMode_e1_0:(pSettings->codecSettings.aacPlus.outputMode == NEXUS_AudioMode_e1_1)?BAPE_ChannelMode_e1_1:BAPE_ChannelMode_e2_0;
        codecSettings.codecSettings.aacPlus.monoMode = (BAPE_MonoChannelMode)pSettings->codecSettings.aacPlus.monoMode;
        codecSettings.codecSettings.aacPlus.sampleRate = pSettings->codecSettings.aacPlus.sampleRate;
        codecSettings.codecSettings.aacPlus.complexity = (BAPE_AacEncodeComplexity)pSettings->codecSettings.aacPlus.complexity;
        break;
    case NEXUS_AudioCodec_eMp3:
        codecSettings.codecSettings.mp3.bitRate = pSettings->codecSettings.mp3.bitRate;
        codecSettings.codecSettings.mp3.privateBit = pSettings->codecSettings.mp3.privateBit;
        codecSettings.codecSettings.mp3.copyrightBit = pSettings->codecSettings.mp3.copyrightBit;
        codecSettings.codecSettings.mp3.originalBit = pSettings->codecSettings.mp3.originalBit;
        codecSettings.codecSettings.mp3.emphasisMode = (BAPE_MpegEmphasisMode)pSettings->codecSettings.mp3.emphasis;
        codecSettings.codecSettings.mp3.channelMode = (pSettings->codecSettings.mp3.outputMode == NEXUS_AudioMode_e1_0)?BAPE_ChannelMode_e1_0:(pSettings->codecSettings.mp3.outputMode == NEXUS_AudioMode_e1_1)?BAPE_ChannelMode_e1_1:BAPE_ChannelMode_e2_0;
        codecSettings.codecSettings.mp3.monoMode = (BAPE_MonoChannelMode)pSettings->codecSettings.mp3.monoMode;
        break;
    case NEXUS_AudioCodec_eWmaStd:
        codecSettings.codecSettings.wmaStd.bitRate = pSettings->codecSettings.wmaStd.bitRate;
        codecSettings.codecSettings.wmaStd.channelMode = pSettings->codecSettings.wmaStd.monoEncoding ? BAPE_ChannelMode_e1_0 : BAPE_ChannelMode_e2_0;
        break;
    case NEXUS_AudioCodec_eG711:
        codecSettings.codecSettings.g711.compressionMode = (BAPE_G711G726CompressionMode)pSettings->codecSettings.g711.compressionMode;
        break;
    case NEXUS_AudioCodec_eG726:
        codecSettings.codecSettings.g726.compressionMode = (BAPE_G711G726CompressionMode)pSettings->codecSettings.g726.compressionMode;
        codecSettings.codecSettings.g726.bitRate = pSettings->codecSettings.g726.bitRate;
        break;
    case NEXUS_AudioCodec_eG729:
        codecSettings.codecSettings.g729.dtxEnabled = pSettings->codecSettings.g729.dtxEnabled;
        codecSettings.codecSettings.g729.bitRate = pSettings->codecSettings.g729.bitRate;
        break;
    case NEXUS_AudioCodec_eG723_1:
        codecSettings.codecSettings.g723_1.vadEnabled = pSettings->codecSettings.g723_1.vadEnabled;
        codecSettings.codecSettings.g723_1.hpfEnabled = pSettings->codecSettings.g723_1.hpfEnabled;
        codecSettings.codecSettings.g723_1.bitRate = pSettings->codecSettings.g723_1.bitRate;
        break;
    default:
        return BERR_SUCCESS;
    }
    errCode = BAPE_Encoder_SetCodecSettings(handle->encoder, &codecSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Get the audio connector for an Audio Encoder stage

Description:
This is used for a direct connection to SPDIF, as follows:

    NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(spdif), NEXUS_AudioEncoder_GetConnector(audioEncoder));

***************************************************************************/
NEXUS_AudioInput NEXUS_AudioEncoder_GetConnector( /* attr{shutdown=NEXUS_AudioInput_Shutdown} */
    NEXUS_AudioEncoderHandle handle
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEncoder);
    return &handle->connector;
}

/***************************************************************************
Summary:
Add an input to this processing stage
***************************************************************************/
NEXUS_Error NEXUS_AudioEncoder_AddInput(
    NEXUS_AudioEncoderHandle handle,
    NEXUS_AudioInput input
    )
{
    BERR_Code errCode;

    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEncoder);
    BDBG_ASSERT(NULL != input);

    errCode = BAPE_Encoder_AddInput(handle->encoder, (BAPE_Connector)input->port);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    errCode = NEXUS_AudioInput_P_AddInput(&handle->connector, input);
    if ( errCode )
    {
        (void)BAPE_Encoder_RemoveInput(handle->encoder, (BAPE_Connector)input->port);
        return BERR_TRACE(errCode);
    }

    handle->input = input;
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
Remove an input from this processing stage
***************************************************************************/
NEXUS_Error NEXUS_AudioEncoder_RemoveInput(
    NEXUS_AudioEncoderHandle handle,
    NEXUS_AudioInput input
    )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_AudioEncoder);
    BDBG_ASSERT(NULL != input);
    if ( input != handle->input )
    {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    (void)BAPE_Encoder_RemoveInput(handle->encoder, (BAPE_Connector)input->port);
    handle->input = NULL;
    return NEXUS_AudioInput_P_RemoveInput(&handle->connector, input);
}

/***************************************************************************
Summary:
Remove all inputs from this processing stage
***************************************************************************/
NEXUS_Error NEXUS_AudioEncoder_RemoveAllInputs(
    NEXUS_AudioEncoderHandle handle
    )
{
    if ( handle->input ) 
    {
        return NEXUS_AudioEncoder_RemoveInput(handle, handle->input);
    }
    return NEXUS_SUCCESS;
}

