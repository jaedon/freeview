/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bape_decoder_settings_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/47 $
 * $brcm_Date: 10/8/12 2:02a $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ape/src/raaga/bape_decoder_settings_priv.c $
 * 
 * Hydra_Software_Devel/47   10/8/12 2:02a jgarrett
 * SW7346-914: Adding enable for ancillary data
 * 
 * Hydra_Software_Devel/46   10/7/12 8:19p jgarrett
 * SW7346-914: Merge to main branch
 * 
 * Hydra_Software_Devel/45   10/3/12 10:24p jgarrett
 * SW7346-914: Undo checkin
 * 
 * Hydra_Software_Devel/SW7346-914/1   10/3/12 10:51p jgarrett
 * SW7346-914: Adding ancillary data user configuration
 * 
 * Hydra_Software_Devel/43   7/12/12 11:20a jgarrett
 * SW7425-3476: Allowing DDRE to run in non-externalPcmMode for PCM-WAV
 * 
 * Hydra_Software_Devel/42   7/6/12 3:54p jgarrett
 * SW7425-455: Detecting and handling orphan nodes that will not produce
 * output
 * 
 * Hydra_Software_Devel/41   7/6/12 12:47p jgarrett
 * SW7425-3441: Adjusting AAC downmix programming to match updated FW
 * interface
 * 
 * Hydra_Software_Devel/40   6/29/12 6:31p jgarrett
 * SW7425-3402: Converting user buffers to static to avoid stack issues
 * 
 * Hydra_Software_Devel/39   6/26/12 3:45p jgarrett
 * SW7425-2911: Adding support for multichannel FLAC
 * 
 * Hydra_Software_Devel/38   6/26/12 3:40p jgarrett
 * SW7552-266: Enabling multichannel for LPCM variants
 * 
 * Hydra_Software_Devel/37   6/6/12 3:28p mpeteri
 * SW7425-2889: Add support for DTS-CD
 * 
 * Hydra_Software_Devel/37   6/6/12 2:44p mpeteri
 * SW7425-2889: Changes for DTS-CD support
 * 
 * Hydra_Software_Devel/SW7425-2889/1   6/5/12 4:30p mpeteri
 * SW7425-2889: Add support for DTS-CD
 * 
 * Hydra_Software_Devel/36   5/3/12 5:06p jgarrett
 * SW7425-2075: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-2075/2   5/3/12 3:45p jgarrett
 * SW7425-2075: Merging latest mainline changes
 * 
 * Hydra_Software_Devel/35   4/10/12 10:56a jgarrett
 * SW7231-744: Forcing MS10 mode for decoders outputting stereo data
 * 
 * Hydra_Software_Devel/SW7425-2075/1   4/6/12 12:52p jgarrett
 * SW7425-2075: Adding new FMT interfaces
 * 
 * Hydra_Software_Devel/34   2/13/12 9:10a jgarrett
 * SW7425-2268: Refactoring mono output programming to use a single buffer
 * 
 * Hydra_Software_Devel/33   1/31/12 6:17p jgarrett
 * SW7425-2268: Adding initial voice conferencing support
 * 
 * Hydra_Software_Devel/32   1/25/12 5:10p jgarrett
 * SW7425-2269: Applying DSP settings for DTS-Legacy as well as other DTS
 * formats
 * 
 * Hydra_Software_Devel/31   1/6/12 11:14a jgarrett
 * SW7231-524: Handling updated pcmwav configuration
 * 
 * Hydra_Software_Devel/29   12/21/11 12:40p jgarrett
 * SW7425-1018: Setting DRC target level for A/85 using AAC-HE
 * 
 * Hydra_Software_Devel/28   12/20/11 5:25p jgarrett
 * SW7425-1018: Switching to RF mode for all PCM outputs and adding debug
 * 
 * Hydra_Software_Devel/27   12/19/11 4:27p jgarrett
 * SW7425-1018: Adding initial A/85 implementation
 * 
 * Hydra_Software_Devel/26   12/15/11 3:22p jgarrett
 * SW7231-500: Converting framesync settings from struct to union
 * 
 * Hydra_Software_Devel/25   12/2/11 5:20p jgarrett
 * SW7425-1893: Adding MPEG conformance mode for Dolby Pulse
 * 
 * Hydra_Software_Devel/24   11/16/11 5:15p jgarrett
 * SW7425-1519: Adding external PCM mixing option
 * 
 * Hydra_Software_Devel/23   10/11/11 5:22p jgarrett
 * SW7425-1444: Setting configuration for PCMWAV in 5.1 mode
 * 
 * Hydra_Software_Devel/22   9/14/11 6:58p jgarrett
 * SW7425-1045: Adding unmapped settings and status values for some
 * decoder algorithms
 * 
 * Hydra_Software_Devel/21   6/29/11 5:00p jgarrett
 * SW7425-620: Allowing multichannel/compressed outputs from a decoder to
 * override to PCM if the codec does not support multichannel/compressed
 * output
 * 
 * Hydra_Software_Devel/20   6/22/11 2:18p jgarrett
 * SW7425-756: Adding multistream support for MPEG/AD
 * 
 * Hydra_Software_Devel/19   6/21/11 11:13a jgarrett
 * SW7425-748: Not allowing multichannel AAC output if only consumer is DD
 * Transcode
 * 
 * Hydra_Software_Devel/18   6/21/11 10:24a jgarrett
 * SW7425-754: Remapping dual mono options for MPEG to match RAP.
 * 
 * Hydra_Software_Devel/17   5/27/11 4:56p jgarrett
 * SW7425-408: Propagating decoder settings for AAC/AC3 to DDRE in MS11
 * usage modes
 * 
 * Hydra_Software_Devel/16   5/25/11 6:16p jgarrett
 * SW7425-408: Merge to main branch
 * 
 * Hydra_Software_Devel/SW7425-408/1   5/19/11 6:10p jgarrett
 * SW7425-408: Moving MS11 to a branch
 * 
 * Hydra_Software_Devel/14   5/18/11 6:50p jgarrett
 * SW7425-408: Adding preliminary DDRE support
 * 
 * Hydra_Software_Devel/13   4/29/11 11:00a jgarrett
 * SW7422-368: Switching to new GetDefaultSettings routines
 * 
 * Hydra_Software_Devel/12   4/19/11 5:33p jgarrett
 * SW7422-408: Adding additional codecs
 * 
 * Hydra_Software_Devel/11   3/21/11 7:08p jgarrett
 * SW7422-356: Adding MuxOutput
 * 
 * Hydra_Software_Devel/10   3/8/11 4:09p jgarrett
 * SW7422-146: Adding MS10 configuration for pulse
 * 
 * Hydra_Software_Devel/9   3/4/11 2:18p jgarrett
 * SW7422-146: Removing warning message
 * 
 * Hydra_Software_Devel/8   3/3/11 6:31p jgarrett
 * SW7422-146: Adding SRC and DSOLA and path review feedback
 * 
 * Hydra_Software_Devel/7   2/16/11 2:59p jgarrett
 * SW7422-146: Coverity CID 260
 * 
 * Hydra_Software_Devel/6   2/16/11 2:58p jgarrett
 * SW7422-146: Coverity CID 261
 * 
 * Hydra_Software_Devel/5   2/15/11 4:08p jgarrett
 * SW7422-146: Adding additional codec settings and types
 * 
 * Hydra_Software_Devel/4   2/7/11 11:30a jgarrett
 * SW7422-146: Implementing DDP -> AC3 conversion and status for
 * MPEG/AAC/AC3
 * 
 * Hydra_Software_Devel/3   2/1/11 5:41p jgarrett
 * SW7422-146: Added MPEG support
 * 
 * Hydra_Software_Devel/2   2/1/11 5:01p jgarrett
 * SW7422-146: Adding AAC and Generic Passthrough
 * 
 * Hydra_Software_Devel/1   1/27/11 5:47p jgarrett
 * SW7422-146: Adding support for multichannel and ac3 user parameters
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bape.h"
#include "bape_priv.h"
#include "bdsp_raaga.h"

BDBG_MODULE(bape_decoder_settings);

static bool BAPE_Decoder_P_HasMultichannelOutput(
    BAPE_DecoderHandle handle
    )
{
    if ( handle->stereoOnMultichannel )
    {
        return false;
    }
    else
    {
        return handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMultichannel].numValidOutputs > 0 ? true : false;
    }
}

static bool BAPE_Decoder_P_HasStereoOutput(
    BAPE_DecoderHandle handle
    )
{
    if ( handle->stereoOnMultichannel || handle->stereoOnCompressed )
    {
        return true;
    }
    else
    {
        return handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eStereo].numValidOutputs > 0 ? true : false;
    }
}

static bool BAPE_Decoder_P_HasMonoOutput(
    BAPE_DecoderHandle handle
    )
{
    return handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMono].numValidOutputs > 0 ? true : false;
}

static BAPE_ChannelMode BAPE_Decoder_P_GetChannelMode(BAPE_DecoderHandle handle)
{
    return BAPE_DSP_P_GetChannelMode(handle->startSettings.codec, handle->settings.outputMode, BAPE_Decoder_P_HasMultichannelOutput(handle), handle->settings.multichannelFormat);
}

static void BAPE_Decoder_P_GetAFDecoderType(BAPE_DecoderHandle handle, BDSP_AF_P_DecoderType *pType)
{
    if ( handle->fwMixerMaster )
    {
        *pType = BDSP_AF_P_DecoderType_ePrimary;
    }
    else
    {
        switch ( handle->startSettings.mixingMode )
        {
        default:
        case BAPE_DecoderMixingMode_eDescription:
            *pType = BDSP_AF_P_DecoderType_eSecondary;
            break;
        case BAPE_DecoderMixingMode_eSoundEffects:
            *pType = BDSP_AF_P_DecoderType_eSoundEffects;
            break;
        }
    }

}

static void BAPE_Decoder_P_GetDefaultAc3Settings(BAPE_DecoderHandle handle)
{
    static BDSP_Raaga_Audio_DDPMultiStreamConfigParams userConfig;

    BDSP_Raaga_GetDefaultAudioTypeSettings(BDSP_AudioType_eAc3, &userConfig, sizeof(userConfig));

    handle->ac3Settings.codec = BAVC_AudioCompressionStd_eAc3;
    handle->ac3Settings.codecSettings.ac3.substreamId = userConfig.ui32SubstreamIDToDecode;
    handle->ac3Settings.codecSettings.ac3.dialogNormalization = userConfig.i32StreamDialNormEnable?true:false;
    handle->ac3Settings.codecSettings.ac3.dialogNormalizationValue = userConfig.i32UserDialNormVal;
    switch ( userConfig.sUserOutputCfg[0].i32CompMode )
    {
    case 0:
        handle->ac3Settings.codecSettings.ac3.drcMode = BAPE_Ac3DrcMode_eCustomA;
        break;
    case 1:
        handle->ac3Settings.codecSettings.ac3.drcMode = BAPE_Ac3DrcMode_eCustomD;
        break;
    default:
    case 2:
        handle->ac3Settings.codecSettings.ac3.drcMode = BAPE_Ac3DrcMode_eLine;
        break;
    case 3:
        handle->ac3Settings.codecSettings.ac3.drcMode = BAPE_Ac3DrcMode_eRf;
        break;
    case 4:
        handle->ac3Settings.codecSettings.ac3.drcMode = BAPE_Ac3DrcMode_eDisabled;
        break;
    }
    switch ( userConfig.sUserOutputCfg[1].i32CompMode )
    {
    case 0:
        handle->ac3Settings.codecSettings.ac3.drcModeDownmix = BAPE_Ac3DrcMode_eCustomA;
        break;
    case 1:
        handle->ac3Settings.codecSettings.ac3.drcModeDownmix = BAPE_Ac3DrcMode_eCustomD;
        break;
    default:
    case 2:
        handle->ac3Settings.codecSettings.ac3.drcModeDownmix = BAPE_Ac3DrcMode_eLine;
        break;
    case 3:
        handle->ac3Settings.codecSettings.ac3.drcModeDownmix = BAPE_Ac3DrcMode_eRf;
        break;
    case 4:
        handle->ac3Settings.codecSettings.ac3.drcModeDownmix = BAPE_Ac3DrcMode_eDisabled;
        break;
    }

    handle->ac3Settings.codecSettings.ac3.scale = 100;
    handle->ac3Settings.codecSettings.ac3.scaleDownmix = 100;
    handle->ac3Settings.codecSettings.ac3.drcScaleHi = 100;
    handle->ac3Settings.codecSettings.ac3.drcScaleLow = 100;
    handle->ac3Settings.codecSettings.ac3.drcScaleHiDownmix = 100;
    handle->ac3Settings.codecSettings.ac3.drcScaleLowDownmix = 100;
    
    BDBG_ASSERT(0x7fffffff == userConfig.sUserOutputCfg[0].i32PcmScale);
    BDBG_ASSERT(0x7fffffff == userConfig.sUserOutputCfg[1].i32PcmScale);
    BDBG_ASSERT(0x7fffffff == userConfig.sUserOutputCfg[0].i32DynScaleHigh);
    BDBG_ASSERT(0x7fffffff == userConfig.sUserOutputCfg[1].i32DynScaleHigh);
    BDBG_ASSERT(0x7fffffff == userConfig.sUserOutputCfg[0].i32DynScaleLow);
    BDBG_ASSERT(0x7fffffff == userConfig.sUserOutputCfg[1].i32DynScaleLow);
    
    BKNI_Memcpy(&handle->ac3PlusSettings, &handle->ac3Settings, sizeof(BAPE_DecoderCodecSettings));
    handle->ac3PlusSettings.codec = BAVC_AudioCompressionStd_eAc3Plus;
}

static void BAPE_Decoder_P_GetDefaultAacSettings(BAPE_DecoderHandle handle)
{
#if BDSP_MS10_SUPPORT
    static  BDSP_Raaga_Audio_DolbyPulseUserConfig userConfig;

    BDSP_Raaga_GetDefaultAudioTypeSettings(BDSP_AudioType_eAacAdts, &userConfig, sizeof(userConfig));
    handle->aacSettings.codec = BAVC_AudioCompressionStd_eAac;
    handle->aacPlusSettings.codec = BAVC_AudioCompressionStd_eAacPlus;
    handle->aacSettings.codecSettings.aac.drcTargetLevel = userConfig.ui32RefDialnormLevel;
    handle->aacSettings.codecSettings.aac.drcDefaultLevel = userConfig.ui32DefDialnormLevel;
    handle->aacSettings.codecSettings.aac.drcScaleHi = userConfig.sOutPortCfg[0].ui32DrcCut;
    handle->aacSettings.codecSettings.aac.drcScaleLow = userConfig.sOutPortCfg[0].ui32DrcBoost;
    handle->aacSettings.codecSettings.aac.downmixMode = userConfig.sOutPortCfg[0].ui32LoRoDownmix?BAPE_AacStereoMode_eLoRo:BAPE_AacStereoMode_eLtRt;
    handle->aacSettings.codecSettings.aac.enableGainFactor = userConfig.sOutPortCfg[0].ui32ApplyGain?true:false;
    handle->aacSettings.codecSettings.aac.gainFactor = userConfig.sOutPortCfg[0].i32GainFactor;
    handle->aacSettings.codecSettings.aac.drcMode = userConfig.sOutPortCfg[0].ui32RfMode?BAPE_DolbyPulseDrcMode_eRf:BAPE_DolbyPulseDrcMode_eLine;
    BKNI_Memcpy(&handle->aacPlusSettings.codecSettings, &handle->aacSettings.codecSettings, sizeof(handle->aacSettings.codecSettings.aac));
#else
    static BDSP_Raaga_Audio_AacheConfigParams userConfig;
    unsigned i,j;

    BDSP_Raaga_GetDefaultAudioTypeSettings(BDSP_AudioType_eAacAdts, &userConfig, sizeof(userConfig));
    handle->aacSettings.codec = BAVC_AudioCompressionStd_eAac;
    handle->aacPlusSettings.codec = BAVC_AudioCompressionStd_eAacPlus;
    handle->aacSettings.codecSettings.aac.drcScaleHi = 100;
    handle->aacSettings.codecSettings.aac.drcScaleLow = 100;
    BDBG_ASSERT(userConfig.ui32DrcGainControlCompress == 0x40000000);
    BDBG_ASSERT(userConfig.ui32DrcGainControlBoost == 0x40000000);
    handle->aacSettings.codecSettings.aac.drcTargetLevel = userConfig.ui32DrcTargetLevel;
    handle->aacSettings.codecSettings.aac.downmixMode = (userConfig.i32DownmixType==0)?BAPE_AacStereoMode_eMatrix:BAPE_AacStereoMode_eArib;
    handle->aacSettings.codecSettings.aac.enableDownmixCoefficients = userConfig.sUserOutputCfg[0].i32ExtDnmixEnabled?true:false;
    for ( i = 0; i < 6; i++ )
    {
        for ( j = 0; j < 6; j++ )
        {
            handle->aacSettings.codecSettings.aac.downmixCoefficients[i][j] = userConfig.sUserOutputCfg[0].i32ExtDnmixTab[i][j];
        }
    }
    BKNI_Memcpy(&handle->aacPlusSettings.codecSettings, &handle->aacSettings.codecSettings, sizeof(handle->aacSettings.codecSettings.aac));
#endif
}

static void BAPE_Decoder_P_GetDefaultWmaProSettings(
    BAPE_DecoderHandle handle
    )
{
    static BDSP_Raaga_Audio_WmaProConfigParams userConfig;

    BDSP_Raaga_GetDefaultAudioTypeSettings(BDSP_AudioType_eWmaPro, &userConfig, sizeof(userConfig));
    
    handle->wmaProSettings.codec = BAVC_AudioCompressionStd_eWmaPro;

    if ( userConfig.sOutputCfg[0].ui32DRCEnable )
    {
        switch ( userConfig.sOutputCfg[0].eDRCSetting )
        {
        default:
            BDBG_WRN(("Unrecognized WMA Pro DRC mode %u", userConfig.sOutputCfg[0].eDRCSetting));
            /* Fall through */
        case BDSP_Raaga_Audio_eDrcSetting_High:
            handle->wmaProSettings.codecSettings.wmaPro.drcMode = BAPE_WmaProDrcMode_eHigh;
            break;
        case BDSP_Raaga_Audio_eDrcSetting_Med:
            handle->wmaProSettings.codecSettings.wmaPro.drcMode = BAPE_WmaProDrcMode_eMedium;
            break;
        case BDSP_Raaga_Audio_eDrcSetting_Low:
            handle->wmaProSettings.codecSettings.wmaPro.drcMode = BAPE_WmaProDrcMode_eLow;
            break;
        }
    }
    else
    {
        handle->wmaProSettings.codecSettings.wmaPro.drcMode = BAPE_WmaProDrcMode_eDisabled;
    }

    handle->wmaProSettings.codecSettings.wmaPro.rmsAmplitudeReference = userConfig.sOutputCfg[0].i32RmsAmplitudeRef;
    handle->wmaProSettings.codecSettings.wmaPro.peakAmplitudeReference = userConfig.sOutputCfg[0].i32PeakAmplitudeRef;
    handle->wmaProSettings.codecSettings.wmaPro.desiredRms = userConfig.sOutputCfg[0].i32DesiredRms;
    handle->wmaProSettings.codecSettings.wmaPro.desiredPeak = userConfig.sOutputCfg[0].i32DesiredPeak;
    handle->wmaProSettings.codecSettings.wmaPro.stereoMode = userConfig.sOutputCfg[0].ui32Stereomode;
    BDBG_ASSERT(handle->wmaProSettings.codecSettings.wmaPro.stereoMode < BAPE_WmaProStereoMode_eMax);
}

static void BAPE_Decoder_P_GetDefaultDtsSettings(
    BAPE_DecoderHandle handle
    )
{
    static BDSP_Raaga_DtshdSettings userConfig;

    BDSP_Raaga_GetDefaultAudioTypeSettings(BDSP_AudioType_eDtshd, &userConfig, sizeof(userConfig));

    handle->dtsSettings.codec = BAVC_AudioCompressionStd_eDts;

    handle->dtsSettings.codecSettings.dts.littleEndian = false;
    handle->dtsSettings.codecSettings.dts.drcMode = (BAPE_DtsDrcMode)userConfig.sUserOutputCfg[0].i32UserDRCFlag;
    handle->dtsSettings.codecSettings.dts.drcScaleHi = 100;
    BDBG_ASSERT(userConfig.sUserOutputCfg[0].i32DynScaleHigh == 0x7fffffff);
    handle->dtsSettings.codecSettings.dts.drcScaleLow = 100;
    BDBG_ASSERT(userConfig.sUserOutputCfg[0].i32DynScaleLow == 0x7fffffff);
    handle->dtsSettings.codecSettings.dts.stereoMode = userConfig.sUserOutputCfg[0].ui32StereoMode;
    handle->dtsSettings.codecSettings.dts.mixLfeToPrimary = userConfig.ui32MixLFE2Primary ? true : false;
}

static void BAPE_Decoder_P_GetDefaultAdpcmSettings(
    BAPE_DecoderHandle handle
    )
{
    static BDSP_Raaga_AdpcmSettings userConfig;

    BDSP_Raaga_GetDefaultAudioTypeSettings(BDSP_AudioType_eAdpcm, &userConfig, sizeof(userConfig));

    handle->adpcmSettings.codec = BAVC_AudioCompressionStd_eAdpcm;

    handle->adpcmSettings.codecSettings.adpcm.gain.enabled = userConfig.sUsrOutputCfg[0].ui32ApplyGain ? true : false;
    handle->adpcmSettings.codecSettings.adpcm.gain.factor = userConfig.sUsrOutputCfg[0].i32GainFactor;
}

void BAPE_Decoder_P_GetDefaultCodecSettings(BAPE_DecoderHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);
    
    BAPE_Decoder_P_GetDefaultAc3Settings(handle);
    BAPE_Decoder_P_GetDefaultAacSettings(handle);
    BAPE_Decoder_P_GetDefaultWmaProSettings(handle);
    BAPE_Decoder_P_GetDefaultDtsSettings(handle);
    BAPE_Decoder_P_GetDefaultAdpcmSettings(handle);
}

static BERR_Code BAPE_Decoder_P_ApplyAc3Settings(BAPE_DecoderHandle handle, BAPE_Ac3Settings *pSettings)
{
    bool lfe;
    bool forceDrcModes=false;
    BERR_Code errCode;
    BAPE_ChannelMode channelMode;
    static BDSP_Raaga_Audio_DDPMultiStreamConfigParams userConfig;

    if ( handle->settings.loudnessEquivalenceEnabled &&
         handle->deviceHandle->settings.loudnessMode != BAPE_LoudnessEquivalenceMode_eNone )
    {
        forceDrcModes = true;
    }

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Determine output mode */
    lfe = handle->settings.outputLfe;
    channelMode = BAPE_Decoder_P_GetChannelMode(handle);
    if ( !BAPE_DSP_P_IsLfePermitted(channelMode) )
    {
        lfe = false;
    }   

    /* Setup global parameters */

    if ( handle->ddre && BAPE_Decoder_P_HasMultichannelOutput(handle) )
    {
        userConfig.eDdpUsageMode = BDSP_Raaga_Audio_eMS11DecodeMode;        
    }
    else if ( handle->fwMixer )
    {
        userConfig.eDdpUsageMode = BDSP_Raaga_Audio_eMS10DecodeMode;        
    }
    else
    {
        userConfig.eDdpUsageMode = BDSP_Raaga_Audio_eSingleDecodeMode;
    }
    
    BAPE_Decoder_P_GetAFDecoderType(handle, &userConfig.eDecoderType);

    userConfig.ui32SubstreamIDToDecode = pSettings->substreamId;
    if ( BAPE_Decoder_P_HasStereoOutput(handle) && BAPE_Decoder_P_HasMultichannelOutput(handle) )
    {
        userConfig.i32NumOutPorts = 2;
        userConfig.sUserOutputCfg[1].i32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e2_0);    /* Stereo */
        userConfig.sUserOutputCfg[1].i32OutLfe = 0;
    }
    else
    {
        userConfig.i32NumOutPorts = 1;
    }
    userConfig.i32StreamDialNormEnable = pSettings->dialogNormalization?1:0;
    userConfig.i32UserDialNormVal = pSettings->dialogNormalizationValue;
    userConfig.sUserOutputCfg[0].i32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);    /* Stereo */
    userConfig.sUserOutputCfg[0].i32OutLfe = lfe?1:0;
    BAPE_DSP_P_GetChannelMatrix(channelMode, lfe, userConfig.sUserOutputCfg[0].ui32OutputChannelMatrix);

    if ( forceDrcModes )
    {
        BDBG_MSG(("Loudness Management Active for AC3/DDP"));
        userConfig.sUserOutputCfg[0].i32CompMode = 3;   /* RF mode for stereo (-20dB) */
        userConfig.sUserOutputCfg[1].i32CompMode = 3;   /* RF mode for stereo (-20dB) */
    }
    else
    {
        BDBG_MSG(("Loudness Management NOT Active for AC3/DDP"));
        switch ( pSettings->drcMode )
        {
        case BAPE_Ac3DrcMode_eCustomA:
            userConfig.sUserOutputCfg[0].i32CompMode = 0;
            break;
        case BAPE_Ac3DrcMode_eCustomD:
            userConfig.sUserOutputCfg[0].i32CompMode = 1;
            break;
        case BAPE_Ac3DrcMode_eLine:
            userConfig.sUserOutputCfg[0].i32CompMode = 2;
            break;
        case BAPE_Ac3DrcMode_eRf:
            userConfig.sUserOutputCfg[0].i32CompMode = 3;
            break;
        default:
        case BAPE_Ac3DrcMode_eDisabled:
            userConfig.sUserOutputCfg[0].i32CompMode = 4;
            break;
        }
        switch ( pSettings->drcModeDownmix )
        {
        case BAPE_Ac3DrcMode_eCustomA:
            userConfig.sUserOutputCfg[1].i32CompMode = 0;
            break;
        case BAPE_Ac3DrcMode_eCustomD:
            userConfig.sUserOutputCfg[1].i32CompMode = 1;
            break;
        case BAPE_Ac3DrcMode_eLine:
            userConfig.sUserOutputCfg[1].i32CompMode = 2;
            break;
        case BAPE_Ac3DrcMode_eRf:
            userConfig.sUserOutputCfg[1].i32CompMode = 3;
            break;
        default:
        case BAPE_Ac3DrcMode_eDisabled:
            userConfig.sUserOutputCfg[1].i32CompMode = 4;
            break;
        }
    }

    switch ( handle->settings.dualMonoMode )
    {
    default:
    case BAPE_DualMonoMode_eStereo:
        userConfig.sUserOutputCfg[0].i32DualMode = 0;
        userConfig.sUserOutputCfg[1].i32DualMode = 0;
        break;
    case BAPE_DualMonoMode_eLeft:
        userConfig.sUserOutputCfg[0].i32DualMode = 1;
        userConfig.sUserOutputCfg[1].i32DualMode = 1;
        break;
    case BAPE_DualMonoMode_eRight:
        userConfig.sUserOutputCfg[0].i32DualMode = 2;
        userConfig.sUserOutputCfg[1].i32DualMode = 2;
        break;
    case BAPE_DualMonoMode_eMix:
        userConfig.sUserOutputCfg[0].i32DualMode = 3;
        userConfig.sUserOutputCfg[1].i32DualMode = 3;
        break;
    }
    switch ( pSettings->stereoMode )
    {
    default:
    case BAPE_Ac3StereoMode_eAuto:
        userConfig.sUserOutputCfg[0].i32StereoMode = 0;
        userConfig.sUserOutputCfg[1].i32StereoMode = 0;
        break;
    case BAPE_Ac3StereoMode_eLtRt:
        userConfig.sUserOutputCfg[0].i32StereoMode = 1;
        userConfig.sUserOutputCfg[1].i32StereoMode = 1;
        break;
    case BAPE_Ac3StereoMode_eLoRo:
        userConfig.sUserOutputCfg[0].i32StereoMode = 2;
        userConfig.sUserOutputCfg[1].i32StereoMode = 2;
        break;
    }

    userConfig.sUserOutputCfg[0].i32PcmScale = BAPE_P_FloatToQ131(pSettings->scale, 100);
    userConfig.sUserOutputCfg[1].i32PcmScale = BAPE_P_FloatToQ131(pSettings->scaleDownmix, 100);
    userConfig.sUserOutputCfg[0].i32DynScaleHigh = BAPE_P_FloatToQ131(pSettings->drcScaleHi, 100);
    userConfig.sUserOutputCfg[0].i32DynScaleLow = BAPE_P_FloatToQ131(pSettings->drcScaleLow, 100);
    userConfig.sUserOutputCfg[1].i32DynScaleHigh = BAPE_P_FloatToQ131(pSettings->drcScaleHiDownmix, 100);
    userConfig.sUserOutputCfg[1].i32DynScaleLow = BAPE_P_FloatToQ131(pSettings->drcScaleLowDownmix, 100);    

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    if ( handle->ddre && (NULL == handle->fwMixer || handle->fwMixerMaster == true) )
    {
        errCode = BAPE_DolbyDigitalReencode_P_SettingsChanged(handle->ddre, handle);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyAacSettings(
    BAPE_DecoderHandle handle, 
    const BAPE_AacSettings *pSettings
    )
{
#if BDSP_MS10_SUPPORT
    BERR_Code errCode;
    static BDSP_Raaga_Audio_DolbyPulseUserConfig userConfig;
    bool lfe, multichannel;
    bool forceDrcModes=false;
    BAPE_ChannelMode channelMode;

    if ( handle->settings.loudnessEquivalenceEnabled &&
         handle->deviceHandle->settings.loudnessMode != BAPE_LoudnessEquivalenceMode_eNone )
    {
        forceDrcModes = true;
    }

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    if ( handle->ddre && BAPE_Decoder_P_HasMultichannelOutput(handle) )
    {
        userConfig.eDolbyPulseUsageMode = BDSP_AF_P_DolbyMsUsageMode_eMS11DecodeMode;        
    }
    else if ( handle->fwMixer )
    {
        userConfig.eDolbyPulseUsageMode = BDSP_AF_P_DolbyMsUsageMode_eMS10DecodeMode;        
    }
    else
    {
        if ( pSettings->mpegConformanceMode )
        {
            userConfig.eDolbyPulseUsageMode = BDSP_AF_P_DolbyMsUsageMode_eMpegConformanceMode;
        }
        else
        {
            userConfig.eDolbyPulseUsageMode = BDSP_AF_P_DolbyMsUsageMode_eSingleDecodeMode;
        }
    }

    BAPE_Decoder_P_GetAFDecoderType(handle, &userConfig.eDecoderType);

    BAPE_DSP_P_SET_VARIABLE(userConfig, ui32RefDialnormLevel, pSettings->drcTargetLevel);
    BAPE_DSP_P_SET_VARIABLE(userConfig, ui32DefDialnormLevel, pSettings->drcDefaultLevel);
    userConfig.sOutPortCfg[0].ui32DrcCut = pSettings->drcScaleHi;
    userConfig.sOutPortCfg[0].ui32DrcBoost = pSettings->drcScaleLow;
    if ( NULL == handle->fwMixer )
    {
        switch ( pSettings->downmixMode )
        {
        default:
        case BAPE_AacStereoMode_eLtRt:
            userConfig.sOutPortCfg[0].ui32LoRoDownmix = 0;
            break;
        case BAPE_AacStereoMode_eLoRo:
            userConfig.sOutPortCfg[0].ui32LoRoDownmix = 1;
            break;
        case BAPE_AacStereoMode_eArib:
            userConfig.sOutPortCfg[0].ui32LoRoDownmix = 2;
            break;
        }
    }
    else
    {
        /* In multi-stream modes, force LoRo always */
        userConfig.sOutPortCfg[0].ui32LoRoDownmix = 1;
    }
    userConfig.sOutPortCfg[0].ui32ApplyGain = (pSettings->enableGainFactor == true) ? 1 : 0;
    userConfig.sOutPortCfg[0].i32GainFactor = pSettings->gainFactor;
    userConfig.sOutPortCfg[0].ui32RfMode = (pSettings->drcMode == BAPE_DolbyPulseDrcMode_eRf)?1:0;
    switch ( handle->settings.dualMonoMode )
    {
    default:
    case BAPE_DualMonoMode_eStereo:
        /* TODO: This is not documented, assume it's the same as others */
        userConfig.sOutPortCfg[0].ui32DualMode = 0;
        break;
    case BAPE_DualMonoMode_eLeft:
        userConfig.sOutPortCfg[0].ui32DualMode = 1;
        break;
    case BAPE_DualMonoMode_eRight:
        userConfig.sOutPortCfg[0].ui32DualMode = 2;
        break;
    case BAPE_DualMonoMode_eMix:
        userConfig.sOutPortCfg[0].ui32DualMode = 3;
        break;
    }
    userConfig.sOutPortCfg[1] = userConfig.sOutPortCfg[0];  /* Duplicate settings to both outputs for now */

    if ( forceDrcModes )
    {
        /* Force RF mode for loudness equivalence.  This will normalize output level at -20dB.  */
        userConfig.sOutPortCfg[0].ui32RfMode = 1;   /* RF mode for stereo (-20dB) */
        userConfig.sOutPortCfg[1].ui32RfMode = 1;   /* RF mode for stereo (-20dB) */
        BDBG_MSG(("Loudness Management Active for Dolby Pulse"));

#if 0   /* JDG - This seems to have been missed, need to handle streams without DRC coding... */
        if ( handle->deviceHandle->settings.loudnessMode == BAPE_LoudnessEquivalenceMode_eEbuR128 )
        {
            userConfig.i32InputVolLevel = -23;   /* -23dB for EBU */
        }
        else
        {
            userConfig.i32InputVolLevel = -31;   /* -31.75dB for ATSC */
        }
#endif
    }
    else
    {
        BDBG_MSG(("Loudness Equivalence not active for Dolby Pulse"));
    }

    multichannel = BAPE_Decoder_P_HasMultichannelOutput(handle);
    /* Determine if the only thing connected to multichannel is the transcoder.  If so, pretend we don't have any multichannel outputs */
    if ( multichannel && handle->outputStatus.connectorStatus[BAPE_ConnectorFormat_eMultichannel].directConnections == 1 )
    {
        BAPE_PathConnection *pConnection;
        pConnection = BLST_SQ_FIRST(&handle->node.connectors[BAPE_ConnectorFormat_eMultichannel].connectionList);
        BDBG_ASSERT(NULL != pConnection);
        if ( pConnection->pSink->type == BAPE_PathNodeType_eEncoder && pConnection->pSink->subtype == BAVC_AudioCompressionStd_eAc3 )
        {
            /* Only connected node is dolby transcoder.  Don't output multichannel. */
            multichannel = false;
        }
    }
    if ( BAPE_Decoder_P_HasStereoOutput(handle) && multichannel )
    {
        userConfig.ui32NumOutPorts = 2;
        userConfig.sOutPortCfg[1].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e2_0);    /* Stereo */
        userConfig.sOutPortCfg[1].ui32OutLfe = 0;
    }
    else
    {
        userConfig.ui32NumOutPorts = 1;
    }

    /* Determine output mode */
    lfe = handle->settings.outputLfe;
    channelMode = BAPE_DSP_P_GetChannelMode(handle->startSettings.codec, handle->settings.outputMode, multichannel, handle->settings.multichannelFormat);
    if ( !BAPE_DSP_P_IsLfePermitted(channelMode) )
    {
        lfe = false;
    }   

    userConfig.sOutPortCfg[0].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);    /* Multichannel */
    userConfig.sOutPortCfg[0].ui32OutLfe = lfe?1:0;
    BAPE_DSP_P_GetChannelMatrix(channelMode, lfe, userConfig.sOutPortCfg[0].ui32OutputChannelMatrix);

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    if ( handle->ddre && (NULL == handle->fwMixer || handle->fwMixerMaster == true) )
    {
        errCode = BAPE_DolbyDigitalReencode_P_SettingsChanged(handle->ddre, handle);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
#else
    static BDSP_Raaga_Audio_AacheConfigParams userConfig;
    BERR_Code errCode;
    unsigned i,j;
    bool lfe;
    BAPE_ChannelMode channelMode;

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    
    userConfig.ui32DrcGainControlCompress = BAPE_P_FloatToQ230(pSettings->drcScaleHi);
    userConfig.ui32DrcGainControlBoost = BAPE_P_FloatToQ230(pSettings->drcScaleLow);
    userConfig.ui32DrcTargetLevel = pSettings->drcTargetLevel;
    userConfig.i32DownmixType = (pSettings->downmixMode == BAPE_AacStereoMode_eMatrix)?0:1;    

    if ( BAPE_Decoder_P_HasStereoOutput(handle) && BAPE_Decoder_P_HasMultichannelOutput(handle) )
    {
        userConfig.i32NumOutPorts = 2;
        userConfig.sUserOutputCfg[1].i32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e2_0);    /* Stereo */
        userConfig.sUserOutputCfg[1].i32OutLfe = 0;
    }
    else
    {
        userConfig.i32NumOutPorts = 1;
    }
    
    /* Determine output mode */
    lfe = handle->settings.outputLfe;
    channelMode = BAPE_Decoder_P_GetChannelMode(handle);
    if ( !BAPE_DSP_P_IsLfePermitted(channelMode) )
    {
        lfe = false;
    }   

    userConfig.sUserOutputCfg[0].i32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);    /* Multichannel */
    userConfig.sUserOutputCfg[0].i32OutLfe = lfe?1:0;
    BAPE_DSP_P_GetChannelMatrix(channelMode, lfe, userConfig.sUserOutputCfg[0].ui32OutputChannelMatrix);

    switch ( handle->settings.dualMonoMode )
    {
    default:
    case BAPE_DualMonoMode_eStereo:
        userConfig.sUserOutputCfg[0].i32DualMode = 0;
        userConfig.sUserOutputCfg[1].i32DualMode = 0;
        break;
    case BAPE_DualMonoMode_eLeft:
        userConfig.sUserOutputCfg[0].i32DualMode = 1;
        userConfig.sUserOutputCfg[1].i32DualMode = 1;
        break;
    case BAPE_DualMonoMode_eRight:
        userConfig.sUserOutputCfg[0].i32DualMode = 2;
        userConfig.sUserOutputCfg[1].i32DualMode = 2;
        break;
    case BAPE_DualMonoMode_eMix:
        userConfig.sUserOutputCfg[0].i32DualMode = 3;
        userConfig.sUserOutputCfg[1].i32DualMode = 3;
        break;
    }

    userConfig.sUserOutputCfg[0].i32ExtDnmixEnabled = (pSettings->enableDownmixCoefficients)?1:0;
    userConfig.sUserOutputCfg[1].i32ExtDnmixEnabled = (pSettings->enableDownmixCoefficients)?1:0;
    for ( i = 0; i < 6; i++ )
    {
        for ( j = 0; j < 6; j++ )
        {
            userConfig.sUserOutputCfg[0].i32ExtDnmixTab[i][j] = pSettings->downmixCoefficients[i][j];
            userConfig.sUserOutputCfg[1].i32ExtDnmixTab[i][j] = pSettings->downmixCoefficients[i][j];
        }
    }

    if ( handle->settings.loudnessEquivalenceEnabled )
    {
        switch ( handle->deviceHandle->settings.loudnessMode )
        {
        case BAPE_LoudnessEquivalenceMode_eAtscA85:
            BDBG_MSG(("A/85 Loudness Equivalence Enabled for AAC-HE"));
            userConfig.i32InputVolLevel = -31;
            userConfig.i32OutputVolLevel = -31;
            userConfig.ui32DrcTargetLevel = 80;   /* Config for -20dB (it's in 0.25dB units) */
            break;
        case BAPE_LoudnessEquivalenceMode_eEbuR128:
            BDBG_MSG(("EBU-R128 Loudness Equivalence Enabled for AAC-HE"));
            userConfig.i32InputVolLevel = -23;
            userConfig.i32OutputVolLevel = -23;
            userConfig.ui32DrcTargetLevel = 80;   /* Config for -20dB (it's in 0.25dB units) */
            break;
        default:
            BDBG_MSG(("Loudness Equivalence Disabled for AAC-HE"));
            break;
        }
    }

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
#endif    
    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyMpegSettings(BAPE_DecoderHandle handle)
{
    static BDSP_Raaga_Audio_MpegConfigParams userConfig;
    BAPE_ChannelMode channelMode;
    BERR_Code errCode;

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    channelMode = BAPE_Decoder_P_GetChannelMode(handle);
    userConfig.ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);
    switch ( handle->settings.dualMonoMode )
    {
    case BAPE_DualMonoMode_eLeft:
        userConfig.ui32DualMonoMode = 0;
        break;
    case BAPE_DualMonoMode_eRight:
        userConfig.ui32DualMonoMode = 1;
        break;
    default:
    case BAPE_DualMonoMode_eStereo:
        userConfig.ui32DualMonoMode = 2;
        break;
    case BAPE_DualMonoMode_eMix:
        userConfig.ui32DualMonoMode = 3;
        break;
    }

    BAPE_Decoder_P_GetAFDecoderType(handle, &userConfig.eDecoderType);

    if ( handle->settings.loudnessEquivalenceEnabled )
    {
        switch ( handle->deviceHandle->settings.loudnessMode )
        {
        case BAPE_LoudnessEquivalenceMode_eAtscA85:
            BDBG_MSG(("ATSC A/85 Loudness Equivalence Enabled for MPEG"));
            userConfig.i32InputVolLevel = -14;
            userConfig.i32OutputVolLevel = -20;
            break;
        case BAPE_LoudnessEquivalenceMode_eEbuR128:
            BDBG_MSG(("EBU-R128 Loudness Equivalence Enabled for MPEG"));
            userConfig.i32InputVolLevel = -23;
            userConfig.i32OutputVolLevel = -23;
            break;
        default:
            BDBG_MSG(("Loudness Equivalence Disabled for MPEG"));
            /* Use default config from FW */
            break;
        }
    }

    if ( handle->pAncDataBufferDescriptor )
    {
        errCode = BMEM_Heap_ConvertAddressToOffset(handle->deviceHandle->memHandle, handle->pAncDataBufferDescriptor, &userConfig.ui32AncDataDramCircAddr);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        userConfig.ui32AncDataParseEnable = handle->settings.ancillaryDataEnabled?1:0;
    }

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyWmaStdSettings(BAPE_DecoderHandle handle)
{
    static BDSP_AudioTaskDatasyncSettings datasyncSettings;
    static BDSP_Raaga_Audio_WmaConfigParams userConfig;
    BAPE_ChannelMode channelMode;
    BERR_Code errCode;

    /* WMA is set in two parts.  The first is in framesync to handle TS vs ASF */
    errCode = BDSP_AudioTask_GetDatasyncSettings(handle->task, 0, 0, 
                                                 &datasyncSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Setup datasync for ASF vs. TS */
    datasyncSettings.uAlgoSpecConfigStruct.sWmaConfig.eWMAIpType = 
        (handle->startSettings.codec == BAVC_AudioCompressionStd_eWmaStdTs)?
        BDSP_Raaga_Audio_WMAIpType_eTS:BDSP_Raaga_Audio_WMAIpType_eASF;

    errCode = BDSP_AudioTask_SetDatasyncSettings(handle->task, 0, 0, 
                                                 &datasyncSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }
    
    /* Set remaining codec settings */
    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    channelMode = BAPE_Decoder_P_GetChannelMode(handle);
    userConfig.ui32OutputMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);
    
    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyWmaProSettings(BAPE_DecoderHandle handle)
{
    static BDSP_Raaga_Audio_WmaProConfigParams userConfig;   
    BAPE_ChannelMode channelMode;
    BERR_Code errCode;
    bool lfe;

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    switch ( handle->wmaProSettings.codecSettings.wmaPro.drcMode )
    {
    case BAPE_WmaProDrcMode_eHigh:
        userConfig.sOutputCfg[0].ui32DRCEnable = 1;
        userConfig.sOutputCfg[0].eDRCSetting = BDSP_Raaga_Audio_eDrcSetting_High;
        userConfig.sOutputCfg[1].ui32DRCEnable = 1;
        userConfig.sOutputCfg[1].eDRCSetting = BDSP_Raaga_Audio_eDrcSetting_High;
        break;
    case BAPE_WmaProDrcMode_eMedium:
        userConfig.sOutputCfg[0].ui32DRCEnable = 1;
        userConfig.sOutputCfg[0].eDRCSetting = BDSP_Raaga_Audio_eDrcSetting_Med;
        userConfig.sOutputCfg[1].ui32DRCEnable = 1;
        userConfig.sOutputCfg[1].eDRCSetting = BDSP_Raaga_Audio_eDrcSetting_Med;
        break;
    case BAPE_WmaProDrcMode_eLow:
        userConfig.sOutputCfg[0].ui32DRCEnable = 1;
        userConfig.sOutputCfg[0].eDRCSetting = BDSP_Raaga_Audio_eDrcSetting_Low;
        userConfig.sOutputCfg[1].ui32DRCEnable = 1;
        userConfig.sOutputCfg[1].eDRCSetting = BDSP_Raaga_Audio_eDrcSetting_Low;
        break;
    default:
        BDBG_WRN(("Unsupported WMA Pro DRC Mode %u", handle->wmaProSettings.codecSettings.wmaPro.drcMode));
        /* Fall through */
    case BAPE_WmaProDrcMode_eDisabled:
        userConfig.sOutputCfg[0].ui32DRCEnable = 0;
        userConfig.sOutputCfg[1].ui32DRCEnable = 0;
        break;
    }

    userConfig.sOutputCfg[0].i32RmsAmplitudeRef = handle->wmaProSettings.codecSettings.wmaPro.rmsAmplitudeReference;
    userConfig.sOutputCfg[1].i32RmsAmplitudeRef = handle->wmaProSettings.codecSettings.wmaPro.rmsAmplitudeReference;
    userConfig.sOutputCfg[0].i32PeakAmplitudeRef = handle->wmaProSettings.codecSettings.wmaPro.peakAmplitudeReference;
    userConfig.sOutputCfg[1].i32PeakAmplitudeRef = handle->wmaProSettings.codecSettings.wmaPro.peakAmplitudeReference;
    userConfig.sOutputCfg[0].i32DesiredRms = handle->wmaProSettings.codecSettings.wmaPro.desiredRms;
    userConfig.sOutputCfg[1].i32DesiredRms = handle->wmaProSettings.codecSettings.wmaPro.desiredRms;
    userConfig.sOutputCfg[0].i32DesiredPeak = handle->wmaProSettings.codecSettings.wmaPro.desiredPeak;
    userConfig.sOutputCfg[1].i32DesiredPeak = handle->wmaProSettings.codecSettings.wmaPro.desiredPeak;
    userConfig.sOutputCfg[0].ui32Stereomode = handle->wmaProSettings.codecSettings.wmaPro.stereoMode;
    userConfig.sOutputCfg[1].ui32Stereomode = handle->wmaProSettings.codecSettings.wmaPro.stereoMode;

    lfe = handle->settings.outputLfe;
    channelMode = BAPE_Decoder_P_GetChannelMode(handle);
    if ( !BAPE_DSP_P_IsLfePermitted(channelMode) )
    {
        lfe = false;
    }   
    userConfig.sOutputCfg[0].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);
    userConfig.sOutputCfg[0].ui32OutLfe = lfe?1:0;
    BAPE_DSP_P_GetChannelMatrix(channelMode, lfe, userConfig.sOutputCfg[0].ui32OutputChannelMatrix);
    userConfig.sOutputCfg[1].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e2_0);
    userConfig.sOutputCfg[1].ui32OutLfe = 0;
    userConfig.ui32NumOutports = (BAPE_Decoder_P_HasStereoOutput(handle) && BAPE_Decoder_P_HasMultichannelOutput(handle))?2:1;
    userConfig.ui32UsageMode = (handle->simul)? 2 /* Simul */ : 0 /* Decode */;

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyDtsSettings(BAPE_DecoderHandle handle)
{
    static BDSP_AudioTaskDatasyncSettings datasyncSettings;
    static BDSP_Raaga_Audio_DtsHdConfigParams userConfig;   
    BAPE_ChannelMode channelMode;
    BERR_Code errCode;
    bool lfe;

    errCode = BDSP_AudioTask_GetDatasyncSettings(handle->task, 0, 0, 
                                                 &datasyncSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Set endian of compressed input data */
    datasyncSettings.uAlgoSpecConfigStruct.sDtsConfig.eDtsEndianType = 
        (handle->dtsSettings.codecSettings.dts.littleEndian)?
        BDSP_Raaga_Audio_DtsEndianType_eLITTLE_ENDIAN:BDSP_Raaga_Audio_DtsEndianType_eBIG_ENDIAN;

    errCode = BDSP_AudioTask_SetDatasyncSettings(handle->task, 0, 0, 
                                                 &datasyncSettings);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    lfe = handle->settings.outputLfe;
    channelMode = BAPE_Decoder_P_GetChannelMode(handle);
    if ( !BAPE_DSP_P_IsLfePermitted(channelMode) )
    {
        lfe = false;
    }   
    userConfig.sUserOutputCfg[0].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);
    userConfig.sUserOutputCfg[0].ui32OutLfe = lfe?1:0;
    BAPE_DSP_P_GetChannelMatrix(channelMode, lfe, userConfig.sUserOutputCfg[0].ui32OutputChannelMatrix);
    userConfig.sUserOutputCfg[1].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e2_0);
    userConfig.sUserOutputCfg[1].ui32OutLfe = 0;
    userConfig.i32NumOutPorts = (BAPE_Decoder_P_HasStereoOutput(handle) && BAPE_Decoder_P_HasMultichannelOutput(handle))?2:1;    

    switch ( handle->settings.dualMonoMode )
    {
    default:
    case BAPE_DualMonoMode_eStereo:
        userConfig.sUserOutputCfg[0].ui32DualMode = 2;
        break;
    case BAPE_DualMonoMode_eLeft:
        userConfig.sUserOutputCfg[0].ui32DualMode = 0;
        break;
    case BAPE_DualMonoMode_eRight:
        userConfig.sUserOutputCfg[0].ui32DualMode = 1;
        break;
    case BAPE_DualMonoMode_eMix:
        userConfig.sUserOutputCfg[0].ui32DualMode = 3;
        break;
    }
    userConfig.sUserOutputCfg[1].ui32DualMode = userConfig.sUserOutputCfg[0].ui32DualMode;

    userConfig.ui32MixLFE2Primary = handle->dtsSettings.codecSettings.dts.mixLfeToPrimary ? 1 : 0;
    userConfig.sUserOutputCfg[0].i32UserDRCFlag = handle->dtsSettings.codecSettings.dts.drcMode == BAPE_DtsDrcMode_eDisabled ? 0 : 1;
    userConfig.sUserOutputCfg[1].i32UserDRCFlag = userConfig.sUserOutputCfg[0].i32UserDRCFlag;

    userConfig.sUserOutputCfg[0].i32DynScaleHigh = BAPE_P_FloatToQ131(handle->dtsSettings.codecSettings.dts.drcScaleHi, 100);
    userConfig.sUserOutputCfg[0].i32DynScaleLow = BAPE_P_FloatToQ131(handle->dtsSettings.codecSettings.dts.drcScaleLow, 100);
    userConfig.sUserOutputCfg[1].i32DynScaleHigh = userConfig.sUserOutputCfg[0].i32DynScaleHigh;
    userConfig.sUserOutputCfg[1].i32DynScaleLow = userConfig.sUserOutputCfg[0].i32DynScaleLow;

    userConfig.sUserOutputCfg[0].ui32StereoMode = (uint32_t)handle->dtsSettings.codecSettings.dts.stereoMode;
    userConfig.sUserOutputCfg[1].ui32StereoMode = userConfig.sUserOutputCfg[0].ui32StereoMode;

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyAmrSettings(BAPE_DecoderHandle handle)
{
    static BDSP_Raaga_Audio_AmrConfigParams userConfig;   
    BAPE_ChannelMode channelMode;
    BERR_Code errCode;

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* AMR is a mono codec.  We can ignore the requested channel mode and force 1_0 */
    channelMode = BAPE_ChannelMode_e1_0;
    userConfig.sUsrOutputCfg[0].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);
    BAPE_DSP_P_GetChannelMatrix(channelMode, false, userConfig.sUsrOutputCfg[0].ui32OutputChannelMatrix);
    userConfig.sUsrOutputCfg[1].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);
    userConfig.ui32NumOutPorts = (BAPE_Decoder_P_HasStereoOutput(handle) && BAPE_Decoder_P_HasMultichannelOutput(handle))?2:1;    

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyDraSettings(BAPE_DecoderHandle handle)
{
    static BDSP_Raaga_Audio_DraConfigParams userConfig;   
    BAPE_ChannelMode channelMode;
    BERR_Code errCode;
    bool lfe;

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    lfe = handle->settings.outputLfe;
    channelMode = BAPE_Decoder_P_GetChannelMode(handle);
    if ( !BAPE_DSP_P_IsLfePermitted(channelMode) )
    {
        lfe = false;
    }   
    userConfig.sUserOutputCfg[0].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);
    userConfig.sUserOutputCfg[0].ui32OutLfe = lfe?1:0;
    BAPE_DSP_P_GetChannelMatrix(channelMode, lfe, userConfig.sUserOutputCfg[0].ui32OutputChannelMatrix);
    userConfig.sUserOutputCfg[1].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e2_0);
    userConfig.sUserOutputCfg[1].ui32OutLfe = 0;
    userConfig.ui32NumOutPorts = (BAPE_Decoder_P_HasStereoOutput(handle) && BAPE_Decoder_P_HasMultichannelOutput(handle))?2:1;    

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyCookSettings(BAPE_DecoderHandle handle)
{
    static BDSP_Raaga_Audio_RalbrConfigParams userConfig;   
    BAPE_ChannelMode channelMode;
    BERR_Code errCode;

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* Cook is a stereo codec.  No LFE support. */
    channelMode = BAPE_Decoder_P_GetChannelMode(handle);
    userConfig.sUserOutputCfg[0].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);
    BAPE_DSP_P_GetChannelMatrix(channelMode, false, userConfig.sUserOutputCfg[0].ui32OutputChannelMatrix);
    userConfig.sUserOutputCfg[1].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);
    userConfig.ui32NumOutPorts = (BAPE_Decoder_P_HasStereoOutput(handle) && BAPE_Decoder_P_HasMultichannelOutput(handle))?2:1;    

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyPcmWavSettings(BAPE_DecoderHandle handle)
{
    static BDSP_Raaga_Audio_PcmWavConfigParams userConfig;
    BAPE_ChannelMode channelMode;
    BERR_Code errCode;
    bool lfe;

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    lfe = handle->settings.outputLfe;
    channelMode = BAPE_Decoder_P_GetChannelMode(handle);
    if ( !BAPE_DSP_P_IsLfePermitted(channelMode) )
    {
        lfe = false;
    }
    
    userConfig.sUserOutputCfg[0].i32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);
    BAPE_DSP_P_GetChannelMatrix(channelMode, lfe, userConfig.sUserOutputCfg[0].ui32OutputChannelMatrix);

    userConfig.ui32NumOutputPorts = 0;
    if ( BAPE_Decoder_P_HasStereoOutput(handle) )
    {
        userConfig.ui32NumOutputPorts++;
    }
    if ( BAPE_Decoder_P_HasMultichannelOutput(handle) )
    {
        userConfig.ui32NumOutputPorts++;
    }
    if ( BAPE_Decoder_P_HasMonoOutput(handle) )
    {
        unsigned monoPathId;

        /* Sanity check - this will be ensured in BAPE_Decoder_P_ValidateSettings */
        BDBG_ASSERT(userConfig.ui32NumOutputPorts < 2);

        userConfig.ui32NumOutputPorts++;
        monoPathId = ( userConfig.ui32NumOutputPorts == 1 ) ? 0 : 1;
        userConfig.sUserOutputCfg[monoPathId].i32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e1_0);
        BAPE_DSP_P_GetMonoChannelMatrix(userConfig.sUserOutputCfg[monoPathId].ui32OutputChannelMatrix);
    }     
        
    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    if ( handle->ddre && (NULL == handle->fwMixer || handle->fwMixerMaster == true) )
    {
        errCode = BAPE_DolbyDigitalReencode_P_SettingsChanged(handle->ddre, handle);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyG711G726Settings(BAPE_DecoderHandle handle)
{
    static BDSP_Raaga_G711G726Settings userConfig;
    BERR_Code errCode;

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    userConfig.ui32NumOutPorts = 0;
    if ( BAPE_Decoder_P_HasStereoOutput(handle) )
    {
        userConfig.ui32NumOutPorts++;
        userConfig.sUsrOutputCfg[0].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e2_0);
        BAPE_DSP_P_GetChannelMatrix(BAPE_ChannelMode_e2_0, false, userConfig.sUsrOutputCfg[0].ui32OutputChannelMatrix);
    }
    if ( BAPE_Decoder_P_HasMonoOutput(handle) )
    {
        unsigned monoPathId;

        /* Sanity check - this will be ensured in BAPE_Decoder_P_ValidateSettings */
        BDBG_ASSERT(userConfig.ui32NumOutPorts < 2);

        userConfig.ui32NumOutPorts++;
        monoPathId = ( userConfig.ui32NumOutPorts == 1 ) ? 0 : 1;
        userConfig.sUsrOutputCfg[monoPathId].ui32ApplyGain = 0;
        userConfig.sUsrOutputCfg[monoPathId].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e1_0);
        BAPE_DSP_P_GetMonoChannelMatrix(userConfig.sUsrOutputCfg[monoPathId].ui32OutputChannelMatrix);
    }     

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyG723_1Settings(BAPE_DecoderHandle handle)
{
    static BDSP_Raaga_G723_1Settings userConfig;
    BERR_Code errCode;

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    userConfig.ui32NumOutPorts = 0;
    if ( BAPE_Decoder_P_HasStereoOutput(handle) )
    {
        userConfig.ui32NumOutPorts++;
        userConfig.sUsrOutputCfg[0].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e2_0);
        BAPE_DSP_P_GetChannelMatrix(BAPE_ChannelMode_e2_0, false, userConfig.sUsrOutputCfg[0].ui32OutputChannelMatrix);
    }
    if ( BAPE_Decoder_P_HasMonoOutput(handle) )
    {
        unsigned monoPathId;

        /* Sanity check - this will be ensured in BAPE_Decoder_P_ValidateSettings */
        BDBG_ASSERT(userConfig.ui32NumOutPorts < 2);

        userConfig.ui32NumOutPorts++;
        monoPathId = ( userConfig.ui32NumOutPorts == 1 ) ? 0 : 1;
        userConfig.sUsrOutputCfg[monoPathId].ui32ScaleOp = 0;
        userConfig.sUsrOutputCfg[monoPathId].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e1_0);
        BAPE_DSP_P_GetMonoChannelMatrix(userConfig.sUsrOutputCfg[monoPathId].ui32OutputChannelMatrix);
    }     

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyG729Settings(BAPE_DecoderHandle handle)
{
    static BDSP_Raaga_G729Settings userConfig;
    BERR_Code errCode;

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    userConfig.ui32NumOutPorts = 0;
    if ( BAPE_Decoder_P_HasStereoOutput(handle) )
    {
        userConfig.ui32NumOutPorts++;
        userConfig.sUsrOutputCfg[0].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e2_0);
        BAPE_DSP_P_GetChannelMatrix(BAPE_ChannelMode_e2_0, false, userConfig.sUsrOutputCfg[0].ui32OutputChannelMatrix);
    }
    if ( BAPE_Decoder_P_HasMonoOutput(handle) )
    {
        unsigned monoPathId;

        /* Sanity check - this will be ensured in BAPE_Decoder_P_ValidateSettings */
        BDBG_ASSERT(userConfig.ui32NumOutPorts < 2);

        userConfig.ui32NumOutPorts++;
        monoPathId = ( userConfig.ui32NumOutPorts == 1 ) ? 0 : 1;
        userConfig.sUsrOutputCfg[monoPathId].ui32ScaleOp = 0;
        userConfig.sUsrOutputCfg[monoPathId].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e1_0);
        BAPE_DSP_P_GetMonoChannelMatrix(userConfig.sUsrOutputCfg[monoPathId].ui32OutputChannelMatrix);
    }     

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyAdpcmSettings(BAPE_DecoderHandle handle)
{
    static BDSP_Raaga_Audio_AdpcmConfigParams userConfig;   
    BAPE_ChannelMode channelMode;
    BERR_Code errCode;

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    /* ADPCM is a stereo codec.  No LFE support. */
    channelMode = BAPE_Decoder_P_GetChannelMode(handle);
    userConfig.sUsrOutputCfg[0].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);
    BAPE_DSP_P_GetChannelMatrix(channelMode, false, userConfig.sUsrOutputCfg[0].ui32OutputChannelMatrix);
    userConfig.sUsrOutputCfg[1].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);
    userConfig.ui32NumOutPorts = (BAPE_Decoder_P_HasStereoOutput(handle) && BAPE_Decoder_P_HasMultichannelOutput(handle))?2:1;    
    userConfig.sUsrOutputCfg[0].ui32ApplyGain = handle->adpcmSettings.codecSettings.adpcm.gain.enabled ? 1 : 0;
    userConfig.sUsrOutputCfg[0].i32GainFactor = handle->adpcmSettings.codecSettings.adpcm.gain.factor;
    userConfig.sUsrOutputCfg[1].ui32ApplyGain = userConfig.sUsrOutputCfg[0].ui32ApplyGain;
    userConfig.sUsrOutputCfg[1].i32GainFactor = userConfig.sUsrOutputCfg[0].i32GainFactor;

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyDvdLpcmSettings(BAPE_DecoderHandle handle)
{
    static BDSP_Raaga_Audio_LpcmUserConfig userConfig;   
    BAPE_ChannelMode channelMode;
    BERR_Code errCode;
    bool lfe;

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    lfe = handle->settings.outputLfe;
    channelMode = BAPE_Decoder_P_GetChannelMode(handle);
    if ( !BAPE_DSP_P_IsLfePermitted(channelMode) )
    {
        lfe = false;
    }   
    userConfig.sOutputConfig[0].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);
    userConfig.sOutputConfig[0].ui32LfeOnFlag = lfe?1:0;
    BAPE_DSP_P_GetChannelMatrix(channelMode, lfe, userConfig.sOutputConfig[0].ui32OutputChannelMatrix);
    userConfig.sOutputConfig[1].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e2_0);
    userConfig.sOutputConfig[1].ui32LfeOnFlag = 0;
    userConfig.ui32NumOutputPorts = (BAPE_Decoder_P_HasStereoOutput(handle) && BAPE_Decoder_P_HasMultichannelOutput(handle))?2:1;    

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

static BERR_Code BAPE_Decoder_P_ApplyFlacSettings(BAPE_DecoderHandle handle)
{
    static BDSP_Raaga_Audio_FlacDecConfigParams userConfig;   
    BAPE_ChannelMode channelMode;
    BERR_Code errCode;
    bool lfe;

    errCode = BDSP_Task_GetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    lfe = handle->settings.outputLfe;
    channelMode = BAPE_Decoder_P_GetChannelMode(handle);
    if ( !BAPE_DSP_P_IsLfePermitted(channelMode) )
    {
        lfe = false;
    }   
    userConfig.sUsrOutputCfg[0].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(channelMode);
    userConfig.sUsrOutputCfg[0].ui32OutLfe = lfe?1:0;
    BAPE_DSP_P_GetChannelMatrix(channelMode, lfe, userConfig.sUsrOutputCfg[0].ui32OutputChannelMatrix);
    userConfig.sUsrOutputCfg[1].ui32OutMode = BAPE_DSP_P_ChannelModeToDsp(BAPE_ChannelMode_e2_0);
    userConfig.sUsrOutputCfg[1].ui32OutLfe = 0;
    userConfig.ui32NumOutPorts = (BAPE_Decoder_P_HasStereoOutput(handle) && BAPE_Decoder_P_HasMultichannelOutput(handle))?2:1;    

    errCode = BDSP_Task_SetStageSettings(handle->task, 0, 0, &userConfig, sizeof(userConfig));
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    return BERR_SUCCESS;
}

BERR_Code BAPE_Decoder_P_ApplyCodecSettings(BAPE_DecoderHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, BAPE_Decoder);

    if ( handle->task )
    {
        if ( !handle->passthrough )
        {
            switch ( handle->startSettings.codec )
            {
            case BAVC_AudioCompressionStd_eAc3:
                return BAPE_Decoder_P_ApplyAc3Settings(handle, &handle->ac3Settings.codecSettings.ac3);
            case BAVC_AudioCompressionStd_eAc3Plus:
                return BAPE_Decoder_P_ApplyAc3Settings(handle, &handle->ac3PlusSettings.codecSettings.ac3Plus);
            case BAVC_AudioCompressionStd_eAacAdts:
            case BAVC_AudioCompressionStd_eAacLoas:
                return BAPE_Decoder_P_ApplyAacSettings(handle, &handle->aacSettings.codecSettings.aac);
            case BAVC_AudioCompressionStd_eAacPlusAdts:
            case BAVC_AudioCompressionStd_eAacPlusLoas:
                return BAPE_Decoder_P_ApplyAacSettings(handle, &handle->aacPlusSettings.codecSettings.aacPlus);
            case BAVC_AudioCompressionStd_eMpegL1:
            case BAVC_AudioCompressionStd_eMpegL2:
            case BAVC_AudioCompressionStd_eMpegL3:
                return BAPE_Decoder_P_ApplyMpegSettings(handle);
            case BAVC_AudioCompressionStd_eWmaStd:
            case BAVC_AudioCompressionStd_eWmaStdTs:
                return BAPE_Decoder_P_ApplyWmaStdSettings(handle);
            case BAVC_AudioCompressionStd_eWmaPro:
                return BAPE_Decoder_P_ApplyWmaProSettings(handle);
            case BAVC_AudioCompressionStd_eDts:
            case BAVC_AudioCompressionStd_eDtshd:
            case BAVC_AudioCompressionStd_eDtsLegacy:
                return BAPE_Decoder_P_ApplyDtsSettings(handle);
            case BAVC_AudioCompressionStd_eAmr:
                return BAPE_Decoder_P_ApplyAmrSettings(handle);
            case BAVC_AudioCompressionStd_eDra:
                return BAPE_Decoder_P_ApplyDraSettings(handle);
            case BAVC_AudioCompressionStd_eCook:
                return BAPE_Decoder_P_ApplyCookSettings(handle);
            case BAVC_AudioCompressionStd_eAdpcm:
                return BAPE_Decoder_P_ApplyAdpcmSettings(handle);
            case BAVC_AudioCompressionStd_ePcmWav:
                return BAPE_Decoder_P_ApplyPcmWavSettings(handle);
            case BAVC_AudioCompressionStd_eG711:
            case BAVC_AudioCompressionStd_eG726:
                return BAPE_Decoder_P_ApplyG711G726Settings(handle);
            case BAVC_AudioCompressionStd_eG723_1:
                return BAPE_Decoder_P_ApplyG723_1Settings(handle);
            case BAVC_AudioCompressionStd_eG729:
                return BAPE_Decoder_P_ApplyG729Settings(handle);
            case BAVC_AudioCompressionStd_eLpcmDvd:
            case BAVC_AudioCompressionStd_eLpcmBd:
            case BAVC_AudioCompressionStd_eLpcm1394:
                return BAPE_Decoder_P_ApplyDvdLpcmSettings(handle);
            case BAVC_AudioCompressionStd_eFlac:
                return BAPE_Decoder_P_ApplyFlacSettings(handle);                
            default:
                BDBG_MSG(("DSP Codec %u (%s) does not have settings", handle->startSettings.codec, BAPE_P_GetCodecName(handle->startSettings.codec)));
                break;
            }
        }
    }

    return BERR_SUCCESS;
}

