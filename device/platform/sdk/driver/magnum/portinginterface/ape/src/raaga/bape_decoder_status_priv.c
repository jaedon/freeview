/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_decoder_status_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 10/18/12 3:59p $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_decoder_status_priv.c $
 * 
 * Hydra_Software_Devel/11   10/18/12 3:59p jgarrett
 * SW7425-4090: Adding frame count status for all algorithms
 * 
 * Hydra_Software_Devel/10   6/29/12 6:31p jgarrett
 * SW7425-3402: Converting user buffers to static to avoid stack issues
 * 
 * Hydra_Software_Devel/9   5/30/12 9:50a jgarrett
 * SW7425-3129: Allowing WMA/AAC status to read from stream info if the
 * mode change interrupt has not fired
 * 
 * Hydra_Software_Devel/8   1/6/12 2:38p jgarrett
 * SW7435-24: Merge to main branch
 * 
 * Hydra_Software_Devel/7   12/20/11 5:35p jgarrett
 * SW7425-1671: Adding Dependent Frame Channel Map for DDP
 * 
 * Hydra_Software_Devel/6   9/14/11 6:58p jgarrett
 * SW7425-1045: Adding unmapped settings and status values for some
 * decoder algorithms
 * 
 * Hydra_Software_Devel/5   5/14/11 12:23p jgarrett
 * SW7425-408: Adding DDRE APIs
 * 
 * Hydra_Software_Devel/4   2/15/11 4:08p jgarrett
 * SW7422-146: Adding additional codec settings and types
 * 
 * Hydra_Software_Devel/3   2/7/11 11:30a jgarrett
 * SW7422-146: Implementing DDP -> AC3 conversion and status for
 * MPEG/AAC/AC3
 * 
 * Hydra_Software_Devel/2   2/2/11 2:46p jgarrett
 * SW7422-146: Adding status
 * 
 * Hydra_Software_Devel/1   2/2/11 2:17p jgarrett
 * SW7422-146: Adding decoder status placeholder
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bape.h"
#include "bape_priv.h"
#include "bdsp_raaga.h"

BDBG_MODULE(bape_decoder_status);

static BAPE_ChannelMode BAPE_Decoder_P_ChannelModeFromDsp(uint32_t dspMode)
{
    switch ( dspMode )
    {
    case 0:
        return BAPE_ChannelMode_e1_1;
    case 1:
        return BAPE_ChannelMode_e1_0;
    case 2:
        return BAPE_ChannelMode_e2_0;
    case 3:
        return BAPE_ChannelMode_e3_0;
    case 4:
        return BAPE_ChannelMode_e2_1;
    case 5:
        return BAPE_ChannelMode_e3_1;
    case 6:
        return BAPE_ChannelMode_e2_2;
    case 7:
        return BAPE_ChannelMode_e3_2;
    default:
        return BAPE_ChannelMode_eMax;
    }
}

static BERR_Code BAPE_Decoder_P_GetAc3Status(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    BERR_Code errCode;
    static BDSP_Raaga_Audio_MultiStreamDDPStreamInfo streamInfo;

    pStatus->codecStatus.ac3.acmod = mode;  /* Use the value from the interrupt.  The streamInfo value is last decoded frame not last parsed. */
    pStatus->codecStatus.ac3.channelMode = BAPE_Decoder_P_ChannelModeFromDsp(mode);
    pStatus->codecStatus.ac3.bitrate = pBitRateInfo->ui32BitRate/1024; /* FW reports in bps */

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }
    
        pStatus->codecStatus.ac3.sampleRateCode = streamInfo.ui32SamplingFrequency;
        pStatus->codecStatus.ac3.bitstreamId = streamInfo.ui32BitStreamIdentification;
        /* This enum is defined to match the AC3 spec */
        pStatus->codecStatus.ac3.bsmod = streamInfo.ui32BsmodValue;
        /* This enum is defined to match the AC3 spec */
        switch ( streamInfo.ui32DsurmodValue )
        {
        case 0:
            pStatus->codecStatus.ac3.dolbySurround = BAPE_Ac3DolbySurround_eNotIndicated;
            break;
        case 1:
            pStatus->codecStatus.ac3.dolbySurround = BAPE_Ac3DolbySurround_eNotEncoded;
            break;
        case 2:
            pStatus->codecStatus.ac3.dolbySurround = BAPE_Ac3DolbySurround_eEncoded;
            break;
        default:
            pStatus->codecStatus.ac3.dolbySurround = BAPE_Ac3DolbySurround_eReserved;
            break;
        }
        /* This enum is defined to match the AC3 spec */
        switch ( streamInfo.ui32CmixLevel )
        {
        case 0:
            pStatus->codecStatus.ac3.centerMixLevel = BAPE_Ac3CenterMixLevel_e3;
            break;
        case 1:
            pStatus->codecStatus.ac3.centerMixLevel = BAPE_Ac3CenterMixLevel_e4_5;
            break;
        case 2:
            pStatus->codecStatus.ac3.centerMixLevel = BAPE_Ac3CenterMixLevel_e6;
            break;
        default:
            pStatus->codecStatus.ac3.centerMixLevel = BAPE_Ac3CenterMixLevel_eReserved;
            break;
        }
        /* This enum is defined to match the AC3 spec */
        switch ( streamInfo.ui32SurmixLevel )
        {
        case 0:
            pStatus->codecStatus.ac3.centerMixLevel = BAPE_Ac3SurroundMixLevel_e3;
            break;
        case 1:
            pStatus->codecStatus.ac3.centerMixLevel = BAPE_Ac3SurroundMixLevel_e6;
            break;
        case 2:
            pStatus->codecStatus.ac3.centerMixLevel = BAPE_Ac3SurroundMixLevel_e0;
            break;
        default:
            pStatus->codecStatus.ac3.centerMixLevel = BAPE_Ac3SurroundMixLevel_eReserved;
            break;
        }
        /* TODO: Doesn't this have the same race condition as acmod? */
        pStatus->codecStatus.ac3.lfe = streamInfo.ui32LfeOn?true:false;
        pStatus->codecStatus.ac3.copyright = streamInfo.ui32CopyrightBit?true:false;
        pStatus->codecStatus.ac3.original = streamInfo.ui32OriginalBitStream?true:false;
        pStatus->codecStatus.ac3.frameSizeCode = streamInfo.ui32FrmSizeCod;
        pStatus->codecStatus.ac3.dependentFrameChannelMap = streamInfo.ui32DepFrameChanmapMode;
        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetMpegStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    BERR_Code errCode;
    static BDSP_Raaga_Audio_MpegStreamInfo streamInfo;

    pStatus->codecStatus.mpeg.bitRate = pBitRateInfo->ui32BitRate/1024; /* FW reports in bps */
    switch ( mode )
    {
    default:
        BDBG_MSG(("Unsupported MPEG channel mode value %u", mode));
        /* fall through */
    case 0:
    case 1:
        pStatus->codecStatus.mpeg.channelMode = BAPE_ChannelMode_e2_0;
        break;
    case 2:
        pStatus->codecStatus.mpeg.channelMode = BAPE_ChannelMode_e1_1;
        break;
    case 3:
        pStatus->codecStatus.mpeg.channelMode = BAPE_ChannelMode_e1_0;
        break;
    }
    pStatus->codecStatus.mpeg.mpegChannelMode = mode;                   /* This enum is defined to match the MPEG spec */

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }
    
        pStatus->codecStatus.mpeg.original = streamInfo.ui32OriginalCopy?true:false;
        pStatus->codecStatus.mpeg.copyright = streamInfo.ui32Copyright?true:false;
        pStatus->codecStatus.mpeg.crcPresent = streamInfo.ui32CrcPresent?true:false;
        pStatus->codecStatus.mpeg.sampleRateCode = streamInfo.ui32SamplingFreq;
        switch ( streamInfo.ui32MpegLayer )
        {
        case 3:
            pStatus->codecStatus.mpeg.layer = 1;
            break;
        case 2:
            pStatus->codecStatus.mpeg.layer = 2;
            break;
        case 1:
            pStatus->codecStatus.mpeg.layer = 3;
            break;
        default:
            BDBG_WRN(("Unrecognized MPEG layer code %u", streamInfo.ui32MpegLayer));
            pStatus->codecStatus.mpeg.layer = 0;
            break;
        }
    
        pStatus->codecStatus.mpeg.emphasisMode = streamInfo.ui32Emphasis;   /* This enum is defined to match the MPEG spec */
        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }
    
    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetAacStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    BERR_Code errCode;
    static BDSP_Raaga_Audio_AacheStreamInfo streamInfo;

    pStatus->codecStatus.aac.channelMode = BAPE_Decoder_P_ChannelModeFromDsp(mode);
    pStatus->codecStatus.aac.bitRate = pBitRateInfo->ui32BitRate/1024; /* FW reports in bps */

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }
        
        switch ( streamInfo.ui32SamplingFreq )
        {
        case 0:
            pStatus->codecStatus.aac.originalSampleRate = 96000;
            break;
        case 1:
            pStatus->codecStatus.aac.originalSampleRate = 88200;
            break;
        case 2:
            pStatus->codecStatus.aac.originalSampleRate = 64000;
            break;
        case 3:
            pStatus->codecStatus.aac.originalSampleRate = 48000;
            break;
        case 4:
            pStatus->codecStatus.aac.originalSampleRate = 44100;
            break;
        case 5:
            pStatus->codecStatus.aac.originalSampleRate = 32000;
            break;
        case 6:
            pStatus->codecStatus.aac.originalSampleRate = 24000;
            break;
        case 7:
            pStatus->codecStatus.aac.originalSampleRate = 22050;
            break;
        case 8:
            pStatus->codecStatus.aac.originalSampleRate = 16000;
            break;
        case 9:
            pStatus->codecStatus.aac.originalSampleRate = 12000;
            break;
        case 10:
            pStatus->codecStatus.aac.originalSampleRate = 11025;
            break;
        case 11:
            pStatus->codecStatus.aac.originalSampleRate = 8000;
            break;
        default:
            BDBG_WRN(("Unsupported AAC sample rate code %u", streamInfo.ui32SamplingFreq));
            pStatus->codecStatus.aac.originalSampleRate = 0;
            break;
        }
        
        pStatus->codecStatus.aac.profile = streamInfo.ui32Profile;  /* This enum is defined to match the spec */
        pStatus->codecStatus.aac.lfe = streamInfo.ui32LfeOn?true:false;
        pStatus->codecStatus.aac.pseudoSurround = streamInfo.ui32PseudoSurroundEnable?true:false;
        pStatus->codecStatus.aac.drc = streamInfo.ui32DrcPresent?true:false;
        if ( pStatus->codecStatus.aac.channelMode == BAPE_ChannelMode_eMax )
        {
            /* The mode change interrupt has not been received, parse from stream information */
            pStatus->codecStatus.aac.channelMode = BAPE_Decoder_P_ChannelModeFromDsp(streamInfo.ui32AcmodValue);
        }
        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetWmaStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_Audio_WmaStreamInfo streamInfo;
    BERR_Code errCode;

    pStatus->codecStatus.wma.channelMode = mode == 0 ? BAPE_ChannelMode_e1_0 : BAPE_ChannelMode_e2_0;
    pStatus->codecStatus.wma.bitRate = pBitRateInfo->ui32BitRate/1024; /* FW reports in bps */

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->codecStatus.wma.copyright = streamInfo.ui32Copyright ? true : false;
        pStatus->codecStatus.wma.original = streamInfo.ui32OriginalCopy ? true : false;
        pStatus->codecStatus.wma.crc = streamInfo.ui32CrcPresent ? true : false;
        pStatus->codecStatus.wma.version = streamInfo.ui32Version + 1;
        if ( pStatus->codecStatus.wma.channelMode == BAPE_ChannelMode_eMax )
        {
            pStatus->codecStatus.wma.channelMode = streamInfo.ui32Acmod;
        }
        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetWmaProStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_Audio_WmaProStreamInfo streamInfo;
    BERR_Code errCode;

    pStatus->codecStatus.wmaPro.channelMode = BAPE_Decoder_P_ChannelModeFromDsp(mode);
    pStatus->codecStatus.wmaPro.bitRate = pBitRateInfo->ui32BitRate/1024; /* FW reports in bps */

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->codecStatus.wmaPro.copyright = streamInfo.ui32Copyright ? true : false;
        pStatus->codecStatus.wmaPro.original = streamInfo.ui32OriginalCopy ? true : false;
        pStatus->codecStatus.wmaPro.crc = streamInfo.ui32CrcPresent ? true : false;
        pStatus->codecStatus.wmaPro.lfe = streamInfo.ui32LfeOn ? true : false;
        pStatus->codecStatus.wmaPro.version = streamInfo.ui32Version + 1;

        switch ( streamInfo.ui32Mode )
        {
        case 0:
            pStatus->codecStatus.wmaPro.stereoMode = BAPE_WmaProStereoMode_eAuto;
            break;
        case 1:
            pStatus->codecStatus.wmaPro.stereoMode = BAPE_WmaProStereoMode_eLtRt;
            break;
        case 2:
            pStatus->codecStatus.wmaPro.stereoMode = BAPE_WmaProStereoMode_eLoRo;
            break;
        default:
            pStatus->codecStatus.wmaPro.stereoMode = BAPE_WmaProStereoMode_eMax;
            break;
        }
        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetDtsStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_Audio_DtsHdStreamInfo streamInfo;
    BERR_Code errCode;

    pStatus->codecStatus.dts.channelMode = BAPE_Decoder_P_ChannelModeFromDsp(mode);
    pStatus->codecStatus.dts.bitRate = pBitRateInfo->ui32BitRate/1024; /* FW reports in bps */

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->codecStatus.dts.amode = streamInfo.ui32Amode;
        pStatus->codecStatus.dts.pcmResolution = streamInfo.ui32PcmResolution;
        pStatus->codecStatus.dts.copyHistory = streamInfo.ui32CopyHist;
        pStatus->codecStatus.dts.extensionDescriptor = streamInfo.ui32ExtAudioId;
        pStatus->codecStatus.dts.version = streamInfo.ui32VerNum;
        pStatus->codecStatus.dts.esFormat = streamInfo.sDtsFrameInfo.ui32EsFlag ? true : false;
        pStatus->codecStatus.dts.lfe = streamInfo.sDtsFrameInfo.ui32LFEPresent ? true : false;
        pStatus->codecStatus.dts.extensionPresent = streamInfo.ui32ExtAudioFlag ? true : false;
        pStatus->codecStatus.dts.crc = streamInfo.ui32CrcFlag ? true : false;
        pStatus->codecStatus.dts.hdcdFormat = streamInfo.ui32HdcdFormat ? true : false;
        pStatus->codecStatus.dts.drc = streamInfo.ui32DynRangeCoeff;
        pStatus->codecStatus.dts.downmixCoefficients = streamInfo.ui32DownMix;
        pStatus->codecStatus.dts.neo = streamInfo.sDtsFrameInfo.ui32DTSNeoEnable;
        pStatus->codecStatus.dts.frameSize = streamInfo.ui32Fsize;
        pStatus->codecStatus.dts.numChannels = streamInfo.sDtsFrameInfo.ui32NumOfChannels;
        pStatus->codecStatus.dts.pcmFrameSize = streamInfo.sDtsFrameInfo.ui32PCMFrameSize;
        pStatus->codecStatus.dts.numPcmBlocks = streamInfo.ui32NBlocks;
        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetPcmWavStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_Audio_PcmWavStreamInfo streamInfo;
    BERR_Code errCode;

    BSTD_UNUSED(mode);
    BSTD_UNUSED(pBitRateInfo);

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->codecStatus.pcmWav.numChannels = streamInfo.ui32NumChannels;
        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetAmrStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_Audio_AmrStreamInfo streamInfo;
    BERR_Code errCode;

    BSTD_UNUSED(mode);
    BSTD_UNUSED(pBitRateInfo);

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->codecStatus.amr.bitRate = streamInfo.ui32BitRate/1024; /* FW reports in bps */
        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetDraStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_Audio_DraStreamInfo streamInfo;
    BERR_Code errCode;

    pStatus->codecStatus.dra.channelMode = BAPE_Decoder_P_ChannelModeFromDsp(mode);
    BSTD_UNUSED(pBitRateInfo);

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->codecStatus.dra.frameSize = streamInfo.ui32FrameSize;
        pStatus->codecStatus.dra.numBlocks = streamInfo.ui32NumBlocks;
        pStatus->codecStatus.dra.acmod = streamInfo.ui32AcmodValue;
        pStatus->codecStatus.dra.lfe = streamInfo.ui32LFEOn ? true : false;
        switch ( streamInfo.ui32OutputMode )
        {
        case 1:
            pStatus->codecStatus.dra.stereoMode = BAPE_DraStereoMode_eLoRo;
            break;
        case 2:
            pStatus->codecStatus.dra.stereoMode = BAPE_DraStereoMode_eLtRt;
            break;
        default:
            pStatus->codecStatus.dra.stereoMode = BAPE_DraStereoMode_eMax;
            break;
        }
        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetCookStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_Audio_RalbrStreamInfo streamInfo;
    BERR_Code errCode;

    BSTD_UNUSED(mode);
    BSTD_UNUSED(pBitRateInfo);

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->codecStatus.cook.stereo = streamInfo.ui32StreamAcmod == 2 ? true : false;
        pStatus->codecStatus.cook.frameSize = streamInfo.ui32FrameSize;
        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetLpcmStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_Audio_LpcmStreamInfo streamInfo;
    BERR_Code errCode;

    BSTD_UNUSED(mode);
    BSTD_UNUSED(pBitRateInfo);

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetMlpStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_Audio_MlpStreamInfo streamInfo;
    BERR_Code errCode;

    BSTD_UNUSED(mode);
    BSTD_UNUSED(pBitRateInfo);

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetAdpcmStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_Audio_AdpcmStreamInfo streamInfo;
    BERR_Code errCode;

    BSTD_UNUSED(mode);
    BSTD_UNUSED(pBitRateInfo);

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetG711G726Status(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_Audio_G726StreamInfo streamInfo;
    BERR_Code errCode;

    BSTD_UNUSED(mode);
    BSTD_UNUSED(pBitRateInfo);

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetG729Status(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_Audio_G729DecStreamInfo streamInfo;
    BERR_Code errCode;

    BSTD_UNUSED(mode);
    BSTD_UNUSED(pBitRateInfo);

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetG723_1Status(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_Audio_G723_1_StreamInfo streamInfo;
    BERR_Code errCode;

    BSTD_UNUSED(mode);
    BSTD_UNUSED(pBitRateInfo);

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetVorbisStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_VorbisStatus streamInfo;
    BERR_Code errCode;

    BSTD_UNUSED(mode);
    BSTD_UNUSED(pBitRateInfo);

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetApeStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_MacStatus streamInfo;
    BERR_Code errCode;

    BSTD_UNUSED(mode);
    BSTD_UNUSED(pBitRateInfo);

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetFlacStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_FlacStatus streamInfo;
    BERR_Code errCode;

    BSTD_UNUSED(mode);
    BSTD_UNUSED(pBitRateInfo);

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_GetGenericStatus(
    BAPE_DecoderHandle handle, 
    unsigned mode, 
    const BDSP_AudioBitRateChangeInfo *pBitRateInfo, 
    BAPE_DecoderStatus *pStatus
    )
{
    static BDSP_Raaga_Audio_DecodeStreamInfo streamInfo;
    BERR_Code errCode;

    BSTD_UNUSED(mode);
    BSTD_UNUSED(pBitRateInfo);

    if ( !handle->passthrough )
    {
        errCode = BDSP_Task_GetStageStatus(handle->task, 0, 0, &streamInfo, sizeof(streamInfo));
        if ( errCode )
        {
            return errCode;
        }

        pStatus->framesDecoded = streamInfo.ui32TotalFramesDecoded;
        pStatus->frameErrors = streamInfo.ui32TotalFramesInError;
        pStatus->dummyFrames = streamInfo.ui32TotalFramesDummy;
    }

    return BERR_SUCCESS;
}

BERR_Code BAPE_Decoder_P_GetCodecStatus(BAPE_DecoderHandle handle, BAPE_DecoderStatus *pStatus)
{
    unsigned mode;
    BDSP_AudioBitRateChangeInfo bitRateInfo;

    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    BDBG_ASSERT(NULL != pStatus);

    if ( NULL == handle->task )
    {
        return BERR_SUCCESS;
    }

    /* Atomically save params updated at isr time */
    BKNI_EnterCriticalSection();
    mode = handle->mode;
    BKNI_Memcpy(&bitRateInfo, &handle->bitRateInfo, sizeof(bitRateInfo));
    BKNI_LeaveCriticalSection();

    switch ( handle->startSettings.codec )
    {
    case BAVC_AudioCompressionStd_eMpegL1:
    case BAVC_AudioCompressionStd_eMpegL2:
    case BAVC_AudioCompressionStd_eMpegL3:
        return BAPE_Decoder_P_GetMpegStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eAc3:
    case BAVC_AudioCompressionStd_eAc3Plus:
        return BAPE_Decoder_P_GetAc3Status(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eAacAdts:
    case BAVC_AudioCompressionStd_eAacLoas:
    case BAVC_AudioCompressionStd_eAacPlusAdts:
    case BAVC_AudioCompressionStd_eAacPlusLoas:
        return BAPE_Decoder_P_GetAacStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eWmaStd:
    case BAVC_AudioCompressionStd_eWmaStdTs:
        return BAPE_Decoder_P_GetWmaStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eWmaPro:
        return BAPE_Decoder_P_GetWmaProStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eDts:
    case BAVC_AudioCompressionStd_eDtshd:
    case BAVC_AudioCompressionStd_eDtsLegacy:
        return BAPE_Decoder_P_GetDtsStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_ePcmWav:
        return BAPE_Decoder_P_GetPcmWavStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eAmr:
        return BAPE_Decoder_P_GetAmrStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eDra:
        return BAPE_Decoder_P_GetDraStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eCook:
        return BAPE_Decoder_P_GetCookStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eLpcmDvd:
    case BAVC_AudioCompressionStd_eLpcmBd:
    case BAVC_AudioCompressionStd_eLpcm1394:
        return BAPE_Decoder_P_GetLpcmStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eAdpcm:
        return BAPE_Decoder_P_GetAdpcmStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eMlp:
        return BAPE_Decoder_P_GetMlpStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eG711:
    case BAVC_AudioCompressionStd_eG726:
        return BAPE_Decoder_P_GetG711G726Status(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eG729:
        return BAPE_Decoder_P_GetG729Status(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eG723_1:
        return BAPE_Decoder_P_GetG723_1Status(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eVorbis:
        return BAPE_Decoder_P_GetVorbisStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eApe:
        return BAPE_Decoder_P_GetApeStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_eFlac:
        return BAPE_Decoder_P_GetFlacStatus(handle, mode, &bitRateInfo, pStatus);
    case BAVC_AudioCompressionStd_ePcm:
        return BAPE_Decoder_P_GetGenericStatus(handle, mode, &bitRateInfo, pStatus);
    default:
        return BERR_SUCCESS;
    }
}

