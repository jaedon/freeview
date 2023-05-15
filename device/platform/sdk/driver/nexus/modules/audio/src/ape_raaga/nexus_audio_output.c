/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_output.c $
* $brcm_Revision: 23 $
* $brcm_Date: 10/19/12 4:09p $
*
* API Description:
*   Private routines internal to the audio module
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/ape_raaga/nexus_audio_output.c $
* 
* 23   10/19/12 4:09p mpeteri
* SW7358-417: merge to main
* 
* SW7358-417/1   10/3/12 12:21p mpeteri
* SW7358-417: Changes to allow NEXUS to configure HDMI audio burst
*  settings.
* 
* 22   5/31/12 10:34a jgarrett
* SW7425-3133: Adding no_independent_delay option
* 
* 21   2/9/12 1:21p jgarrett
* SW7425-2074: Supporting 7.1 PCM
* 
* 20   1/27/12 4:48p jgarrett
* SW7429-55: Using APE Mixer defaults for NCO default in nexus
* 
* 19   11/18/11 2:47p jgarrett
* SW7425-1757: Propagating channel status information to APE
* 
* 18   9/8/11 8:45a jgarrett
* SWDTV-6627: Adding nexus EQ support
* 
* 17   9/1/11 9:32a ialauder
* SWDTV-8471: NEXUS_AudioReturnChannel_P_SetMaster was called with params
*  in reversed order.  Also need to prevent setting volume if we have a
*  slave output.
* 
* 16   7/13/11 12:15p gskerl
* SW7425-884: Re-checked in: Added support for NCO/Mclkgen audio clock
*  sources
* 
* 15   7/10/11 10:24p gskerl
* SW7552-72: Reverted previous checkin (waiting until APE label is moved)
* 
* 14   7/8/11 4:32p gskerl
* SW7552-72: Added support for NCO/Mclkgen audio clock sources
* 
* 13   7/6/11 6:41p jgarrett
* SW7552-55: Mapping channelMode to stereoMode
* 
* 12   6/30/11 6:24p jgarrett
* SW7346-206: Caching HDMI sample rate from last sample rate callback and
*  not resetting to default on disconnection
* 
* 11   5/25/11 5:16p jgarrett
* SW7425-408: Adding BDBG_OBJECT to input/output types and MS11 features
* 
* 10   5/13/11 2:06p gskerl
* SW7422-354: Added APE support for Audio Return Channel
* 
* 9   4/26/11 11:35a jgarrett
* SW7425-437: Resolving kernel mode shutdown issues
* 
* 8   4/6/11 11:39a jgarrett
* SW35330-35: Stubbing out ARC and making HDMI output optional
* 
* 7   3/21/11 7:05p jgarrett
* SW7422-356: Adding MuxOutput support in APE/Nexus
* 
* 6   3/3/11 8:09p jgarrett
* SW7422-146: Fixing DDP passthrough sample rate issue
* 
* 5   3/3/11 6:32p jgarrett
* SW7422-146: Fixing issue with HDMI sample rate being stuck at a
*  previous value
* 
* 4   2/22/11 5:44p jgarrett
* SW7422-146: Implemented type renaming based on filter graph review
*  comments
* 
* 3   1/27/11 5:47p jgarrett
* SW7422-146: Removing warning of bad hdmi sample rate
* 
* 2   1/21/11 10:58a jgarrett
* SW7422-146: Fixing HDMI sample rate
* 
* 1   12/17/10 3:56p jgarrett
* SW7422-146: Adding initial nexus on APE for 7422
* 
***************************************************************************/
#include "nexus_audio_module.h"
#include "priv/nexus_audio_output_priv.h"
#if NEXUS_NUM_HDMI_OUTPUTS
#include "nexus_hdmi_output.h"
#include "priv/nexus_hdmi_output_priv.h"
static struct
{
    NEXUS_AudioOutput output;
    BAPE_MaiOutputHandle handle;
    BKNI_EventHandle settingsChangedEvent;
    NEXUS_EventCallbackHandle settingsChangedCallback;
} g_hdmiMapping[NEXUS_NUM_HDMI_OUTPUTS];

static void NEXUS_AudioOutput_P_HdmiSettingsChanged(void *pOutput);
static void NEXUS_AudioOutput_P_HdmiSampleRateChange_isr(void *, int, unsigned);
#define NEXUS_AudioOutput_P_GetSampleRateEnum NEXUS_AudioOutput_P_GetSampleRateEnum_isr
static BAVC_AudioSamplingRate NEXUS_AudioOutput_P_GetSampleRateEnum_isr(unsigned sampleRate)
{
    switch ( sampleRate )
    {
    case 32000:
        return BAVC_AudioSamplingRate_e32k;    /* 32K Sample rate */
    case 44100:
        return BAVC_AudioSamplingRate_e44_1k;    /* 44.1K Sample rate */
    case 48000:
        return BAVC_AudioSamplingRate_e48k;      /* 48K Sample rate */
    case 96000:
        return BAVC_AudioSamplingRate_e96k;      /* 96K Sample rate */
    case 16000:
        return BAVC_AudioSamplingRate_e16k;      /* 16K Sample rate */
    case 22050:
        return BAVC_AudioSamplingRate_e22_05k;   /* 22.05K Sample rate */
    case 24000:
        return BAVC_AudioSamplingRate_e24k;      /* 24K Sample rate */
    case 64000:
        return BAVC_AudioSamplingRate_e64k;      /* 64K Sample rate */
    case 88200:
        return BAVC_AudioSamplingRate_e88_2k;    /* 88.2K Sample rate */
    case 128000:
        return BAVC_AudioSamplingRate_e128k;     /* 128K Sample rate */
    case 176400:
        return BAVC_AudioSamplingRate_e176_4k;   /* 176.4K Sample rate */
    case 192000:
        return BAVC_AudioSamplingRate_e192k;     /* 192K Sample rate */
    case 8000:
        return BAVC_AudioSamplingRate_e8k;       /* 8K Sample rate */
    case 12000:
        return BAVC_AudioSamplingRate_e12k;      /* 12K Sample rate */
    case 11025:
        return BAVC_AudioSamplingRate_e11_025k;  /* 11.025K Sample rate */
    default:
        return 0;
    }
}
#endif
#if NEXUS_NUM_RFM_OUTPUTS
#include "priv/nexus_rfm_priv.h"
static struct
{
    NEXUS_AudioOutput output;
    BAPE_RfModHandle handle;
} g_rfmMapping[NEXUS_NUM_RFM_OUTPUTS];
#endif
#if 0 /* TODO */
#include "bchp_vcxo_0_rm.h"
#include "bchp_vcxo_ctl_misc.h"
#endif

BDBG_MODULE(nexus_audio_output);

BDBG_OBJECT_ID(NEXUS_AudioOutputData);


/******************************************************************************
The array to represent the value of volume in hex corresponding to the value
in DB. The application inputs the volume in terms of DB and the Corresponding
HEX value is mentioned here. The formula used for the same is:

    HEX = (2^23) * 10^(DB/20)

Note: 23 is the number of bits in the volume control field.

The volume can range from 0-1. 0 in hex corresponds to the 139 DB from the above
Formula. If application enters more than this value, it is forced to 0.
******************************************************************************/
static const int32_t g_db2linear[] =
{
    0x800000,   0x721482,   0x65AC8C,   0x5A9DF7,   0x50C335,
    0x47FACC,   0x4026E7,   0x392CED,   0x32F52C,   0x2D6A86,
    0x287A26,   0x241346,   0x2026F3,   0x1CA7D7,   0x198A13,
    0x16C310,   0x144960,   0x12149A,   0x101D3F,   0xE5CA1,
    0xCCCCC,    0xB6873,    0xA2ADA,    0x90FCB,    0x81385,
    0x732AE,    0x66A4A,    0x5B7B1,    0x51884,    0x48AA7,
    0x40C37,    0x39B87,    0x33718,    0x2DD95,    0x28DCE,
    0x246B4,    0x20756,    0x1CEDC,    0x19C86,    0x16FA9,
    0x147AE,    0x1240B,    0x10449,    0xE7FA,     0xCEC0,
    0xB844,     0xA43A,     0x925E,     0x8273,     0x7443,
    0x679F,     0x5C5A,     0x524F,     0x495B,     0x4161,
    0x3A45,     0x33EF,     0x2E49,     0x2940,     0x24C4,
    0x20C4,     0x1D34,     0x1A07,     0x1732,     0x14AC,
    0x126D,     0x106C,     0xEA3,      0xD0B,      0xBA0,
    0xA5C,      0x93C,      0x83B,      0x755,      0x689,
    0x5D3,      0x531,      0x4A0,      0x420,      0x3AD,
    0x346,      0x2EB,      0x29A,      0x251,      0x211,
    0x1D7,      0x1A4,      0x176,      0x14D,      0x129,
    0xEC,       0xD2,       0xA7,       0x95,       0x84,
    0x76,       0x69,       0x5E,       0x53,       0x4A,
    0x42,       0x3B,       0x34,       0x2F,       0x2A,
    0x25,       0x21,       0x1D,       0x1A,       0x17,
    0x15,       0x12,       0x10,       0xE,        0xD,
    0xB,        0xA,        0x9,        0x8,        0x7,
    0x6,        0x5,        0x5,        0x4,        0x4,
    0x3,        0x3,        0x2,        0x2,        0x2,
    0x2,        0x1,        0x1,        0x1,        0x1,
    0x1,        0x1,        0x1,        0x0
};

static int32_t NEXUS_AudioOutput_P_Vol2Magnum(NEXUS_AudioVolumeType type, int32_t volume)
{
    if ( type == NEXUS_AudioVolumeType_eDecibel )
    {
        int32_t linearVol;
        int volumeIndex;
        if ( volume > 0 )
        {
            BDBG_ERR(("Currently, amplification is not supported for dB values.  Clipping to 0 dB"));
            return 0;
        }
        else if ( volume < NEXUS_AUDIO_VOLUME_DB_MIN )
        {
            BDBG_ERR(("Clipping out of range volume to minimum"));
            return NEXUS_AUDIO_VOLUME_LINEAR_MIN;
        }
        else if ( volume == NEXUS_AUDIO_VOLUME_DB_MIN )
        {
            return NEXUS_AUDIO_VOLUME_LINEAR_MIN;
        }

        /* Convert dB attenuation to linear gain.  Start by Flipping the sign */
        volume = -volume;

        /* Grab integer portion */
        volumeIndex = volume/100;
        linearVol = g_db2linear[volumeIndex];
        /* Grab fractional portion */
        volume = volume % 100;
        /* Linearly interpolate between the two levels */
        linearVol -= ((g_db2linear[volumeIndex]-g_db2linear[volumeIndex+1])*volume)/100;

        return linearVol;
    }
    else
    {
        if ( volume < NEXUS_AUDIO_VOLUME_LINEAR_MIN )
        {
            BDBG_ERR(("Clipping out of range volume to minimum"));
            return NEXUS_AUDIO_VOLUME_LINEAR_MIN;
        }
#if 0 /* Permitting amplification */
        /* The destination APIs are linear now.  No conversion required!! */
        else if ( volume > NEXUS_AUDIO_VOLUME_LINEAR_NORMAL )
        {
            BDBG_ERR(("This platform does not support amplification.  Volume will be set to normal"));
            return NEXUS_AUDIO_VOLUME_LINEAR_NORMAL;
        }
#endif

        return volume;
    }
}

/***************************************************************************
Summary:
    Get default settings for an audio output
See Also:
    NEXUS_AudioOutput_Open
 ***************************************************************************/
void NEXUS_AudioOutput_GetDefaultSettings(
    NEXUS_AudioOutputSettings *pSettings      /* [out] Settings */
    )
{
    BAPE_MixerSettings mixerSettings;

    BAPE_Mixer_GetDefaultSettings(&mixerSettings);

    BKNI_Memset(pSettings, 0, sizeof(NEXUS_AudioOutputSettings));
    pSettings->defaultSampleRate = 48000;
    pSettings->pll = g_NEXUS_audioModuleData.settings.defaultPll;
    pSettings->nco = mixerSettings.outputNco;
    pSettings->leftVolume = NEXUS_AUDIO_VOLUME_DB_NORMAL;
    pSettings->rightVolume = NEXUS_AUDIO_VOLUME_DB_NORMAL;
}

/***************************************************************************
Summary:
    Get settings of an audio output
See Also:
    NEXUS_AudioOutput_SetSettings
 ***************************************************************************/
void NEXUS_AudioOutput_GetSettings(
    NEXUS_AudioOutput output,
    NEXUS_AudioOutputSettings *pSettings    /* [out] Current Settings */
    )
{
    NEXUS_AudioOutputData *pData;

    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);
    BDBG_ASSERT(NULL != pSettings);

    if ( NULL == output->pMixerData )
    {
        NEXUS_AudioOutput_GetDefaultSettings(pSettings);
        return;
    }

    pData = output->pMixerData;
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
    *pSettings = pData->settings;

    /* Success */
}

/***************************************************************************
Summary:
    Set settings of an audio output
See Also:
    NEXUS_AudioOutput_GetSettings
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_SetSettings(
    NEXUS_AudioOutput output,
    const NEXUS_AudioOutputSettings *pSettings
    )
{
    BAPE_OutputVolume volume;
    NEXUS_Error errCode;
    NEXUS_AudioOutputSettings defaultSettings;
    NEXUS_AudioOutputData *pData;
    int i;
    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);

    if ( NULL == pSettings )
    {
        NEXUS_AudioOutput_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    if ( NULL == output->pMixerData )
    {
        pData = NEXUS_AudioOutput_P_CreateData(output);
        if ( NULL == pData )
        {
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }
    }

    pData = output->pMixerData;
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);

    if ( !(NEXUS_AudioOutput_P_IsDacSlave(output) || NEXUS_AudioOutput_P_IsSpdifSlave(output)) )
    {
        /* We can apply volume immediately.  Do that here. */
        BAPE_GetOutputVolume((BAPE_OutputPort)output->port, &volume);
        for ( i = 0; i < BAPE_ChannelPair_eMax; i++ )
        {
            volume.volume[i*2] = NEXUS_AudioOutput_P_Vol2Magnum(pSettings->volumeType, pSettings->leftVolume);
            volume.volume[(i*2)+1] = NEXUS_AudioOutput_P_Vol2Magnum(pSettings->volumeType, pSettings->rightVolume);
        }
        volume.muted = pSettings->muted || pData->trickMute;
        errCode = BAPE_SetOutputVolume((BAPE_OutputPort)output->port, &volume);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    /* Apply channel mode if required */
    if ( output->setChannelMode )
    {
        errCode = output->setChannelMode(output->pObjectHandle, pSettings->channelMode);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
#if NEXUS_NUM_HDMI_OUTPUTS > 0
    else if ( output->objectType == NEXUS_AudioOutputType_eHdmi )
    {
        for ( i = 0; i < NEXUS_NUM_HDMI_OUTPUTS; i++ )
        {
            if ( g_hdmiMapping[i].output == output )
            {
                BAPE_MaiOutputSettings maiOutputSettings;
                NEXUS_HdmiOutputSettings hdmiSettings;
                BAPE_MaiOutput_GetSettings(g_hdmiMapping[i].handle, &maiOutputSettings);
                switch ( pSettings->channelMode )
                {
                default:
                case NEXUS_AudioChannelMode_eStereo:
                    maiOutputSettings.stereoMode = BAPE_StereoMode_eLeftRight;
                    break;
                case NEXUS_AudioChannelMode_eLeft:
                    maiOutputSettings.stereoMode = BAPE_StereoMode_eLeftLeft;
                    break;
                case NEXUS_AudioChannelMode_eRight:
                    maiOutputSettings.stereoMode = BAPE_StereoMode_eRightRight;
                    break;
                case NEXUS_AudioChannelMode_eSwapped:
                    maiOutputSettings.stereoMode = BAPE_StereoMode_eRightLeft;
                    break;
                }

                /* Remaining settings need to be pulled from the HDMI output */
                NEXUS_HdmiOutput_GetSettings(output->pObjectHandle, &hdmiSettings);
                maiOutputSettings.channelStatus.professional = hdmiSettings.audioChannelStatusInfo.professionalMode;
                maiOutputSettings.channelStatus.copyright = hdmiSettings.audioChannelStatusInfo.swCopyRight;
                maiOutputSettings.channelStatus.categoryCode = hdmiSettings.audioChannelStatusInfo.categoryCode;
                maiOutputSettings.channelStatus.clockAccuracy = hdmiSettings.audioChannelStatusInfo.clockAccuracy;
                maiOutputSettings.channelStatus.separateLeftRight = hdmiSettings.audioChannelStatusInfo.separateLRChanNum;
                maiOutputSettings.ditherEnabled = hdmiSettings.audioDitherEnabled;
                maiOutputSettings.underflowBurst = hdmiSettings.audioBurstType;

                errCode = BAPE_MaiOutput_SetSettings(g_hdmiMapping[i].handle, &maiOutputSettings);
                if ( errCode )
                {
                    return BERR_TRACE(errCode);
                }
                break;
            }
        }
    }
#endif

    /* Store settings */
    pData->settings = *pSettings;

    /* Applying remaining settings should be deferred until the next start call if we're running. */
    if ( pData->input )
    {
        NEXUS_AudioInput_P_OutputSettingsChanged(pData->input, output);
    }

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Add an input to this output
See Also:
    NEXUS_AudioOutput_RemoveInput
    NEXUS_AudioOutput_RemoveAllInputs
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_AddInput(
    NEXUS_AudioOutput output,
    NEXUS_AudioInput input
    )
{
    NEXUS_Error errCode;
    NEXUS_AudioOutputData *pData;

    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);
    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);

    if ( NULL == output->pMixerData )
    {
        pData = NEXUS_AudioOutput_P_CreateData(output);
        if ( NULL == pData )
        {
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }
    }

    pData = output->pMixerData;
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);

    if ( pData->input != NULL )
    {
        BDBG_WRN(("Input %p is already connected to this output", output->pMixerData));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( NEXUS_AudioInput_P_IsRunning(input) )
    {
        BDBG_WRN(("Input %p is running.  Please stop first.", input));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    errCode = NEXUS_AudioInput_P_ConnectOutput(input, output);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    pData->input = input;

    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Add an input to this output
See Also:
    NEXUS_AudioOutput_AddInput
    NEXUS_AudioOutput_RemoveAllInputs
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_RemoveInput(
    NEXUS_AudioOutput output,
    NEXUS_AudioInput input
    )
{
    NEXUS_Error errCode;
    NEXUS_AudioOutputData *pData;

    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);
    BDBG_OBJECT_ASSERT(input, NEXUS_AudioInput);

    pData = output->pMixerData;

    if ( NULL == pData || input != pData->input )
    {
        BDBG_ERR(("Input %p is not connected to this output.", input));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);

    if ( NEXUS_AudioInput_P_IsRunning(input) )
    {
        BDBG_WRN(("Forcing input %p to stop on output %p shutdown", input, output));
        NEXUS_AudioInput_P_ForceStop(input);
    }
    
    errCode = NEXUS_AudioInput_P_DisconnectOutput(input, output);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    pData->input = NULL;
    return BERR_SUCCESS;
}

/***************************************************************************
Summary:
    Add an input to this output
See Also:
    NEXUS_AudioOutput_AddInput
    NEXUS_AudioOutput_RemoveInput
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_RemoveAllInputs(
    NEXUS_AudioOutput output
    )
{
    NEXUS_AudioOutputData *pData;

    BDBG_ASSERT(NULL != output);

    pData = output->pMixerData;
    if ( pData && NULL != pData->input )
    {
        NEXUS_Error errCode;

        BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
        errCode = NEXUS_AudioOutput_RemoveInput(output, pData->input);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }

    return BERR_SUCCESS;;
}

/***************************************************************************
Summary:
    Shutdown this output handle
Description:
    This routine should be called before the specific output object
    (e.g. AudioDac) is closed.
See Also:
    NEXUS_AudioOutput_AddInput
    NEXUS_AudioOutput_RemoveInput
 ***************************************************************************/
void NEXUS_AudioOutput_Shutdown(
    NEXUS_AudioOutput output
    )
{
    NEXUS_AudioOutputData *pData;

    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);

    pData = output->pMixerData;
    if ( pData )
    {
        NEXUS_Error rc;

        BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);

        rc = NEXUS_AudioOutput_RemoveAllInputs(output);
        if (rc) {
            /* we cannot complete the shutdown */
            return;
        }
        if ( pData->equalizer )
        {
            rc = NEXUS_AudioOutput_SetEqualizer(output, NULL);
            if (rc) {
                /* we cannot complete the shutdown */
                return;
            }
        }

        BDBG_OBJECT_DESTROY(pData, NEXUS_AudioOutputData);
        BKNI_Free(pData);

        /* TODO: Handle RFM object handles */
        switch ( output->objectType )
        {
#if NEXUS_NUM_HDMI_OUTPUTS
        case NEXUS_AudioOutputType_eHdmi:
            {
                int i;
                for ( i = 0; i < NEXUS_NUM_HDMI_OUTPUTS; i++ )
                {
                    if ( g_hdmiMapping[i].output == output )
                    {
                        output->port = 0;
                        BDBG_MSG(("Closing MAI output"));
                        BAPE_MaiOutput_Close(g_hdmiMapping[i].handle);

                        NEXUS_Module_Lock(g_NEXUS_audioModuleData.settings.modules.hdmiOutput);
                        NEXUS_HdmiOutput_SetNotifyAudioEvent_priv(output->pObjectHandle, NULL);
                        NEXUS_Module_Unlock(g_NEXUS_audioModuleData.settings.modules.hdmiOutput);

                        NEXUS_UnregisterEvent(g_hdmiMapping[i].settingsChangedCallback);
                        BKNI_DestroyEvent(g_hdmiMapping[i].settingsChangedEvent);
                        BKNI_Memset(&g_hdmiMapping[i], 0, sizeof(g_hdmiMapping[i]));
                        break;
                    }
                }
                BDBG_ASSERT(i < NEXUS_NUM_HDMI_OUTPUTS);
            }
            break;
#endif
#if NEXUS_NUM_RFM_OUTPUTS
        case NEXUS_AudioOutputType_eRfm:
            {
                int i;
                for ( i = 0; i < NEXUS_NUM_RFM_OUTPUTS; i++ )
                {
                    if ( g_rfmMapping[i].output == output )
                    {
                        output->port = 0;
                        BDBG_MSG(("Closing RFM output"));
                        BAPE_RfMod_Close(g_rfmMapping[i].handle);
                        g_rfmMapping[i].output = NULL;
                        g_rfmMapping[i].handle = NULL;
                        break;
                    }
                }
                BDBG_ASSERT(i < NEXUS_NUM_RFM_OUTPUTS);
            }
            break;
#endif
        default:
            break;
        }
    }
    output->pMixerData = NULL;

    return ;
}

/***************************************************************************
Summary:
    Determine if an output must be tied to a DAC
Description:
    Some outputs (i.e. RFM) are slaves to a DAC.
 ***************************************************************************/
bool NEXUS_AudioOutput_P_IsDacSlave(
    NEXUS_AudioOutput output
    )
{
    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);
    return (output->objectType == NEXUS_AudioOutputType_eRfm) ? true : false;
}

/***************************************************************************
Summary:
    Determine if an output must be tied to a SPDIF port
Description:
    Some outputs (i.e. ARC) are slaves to a SPDIF port.
 ***************************************************************************/
bool NEXUS_AudioOutput_P_IsSpdifSlave(
    NEXUS_AudioOutput output
    )
{
    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);
    return (output->objectType == NEXUS_AudioOutputType_eArc) ? true : false;
}

/***************************************************************************
Summary:
    For slave outputs, set the DAC source
Description:
    Some outputs (i.e. RFM) are slaves to a DAC.  Mixer will find the proper
    DAC to bind the output to and provide it here.
 ***************************************************************************/
NEXUS_Error NEXUS_AudioOutput_P_SetSlaveSource(
    NEXUS_AudioOutput slaveHandle,
    NEXUS_AudioOutput sourceHandle)
{
    BDBG_OBJECT_ASSERT(slaveHandle, NEXUS_AudioOutput);
    BSTD_UNUSED(sourceHandle);  /* In case neither of the cases below are defined */

    if ( NEXUS_AudioOutput_P_IsDacSlave(slaveHandle) )
    {
        #if NEXUS_NUM_RFM_OUTPUTS
        NEXUS_RfmConnectionSettings rfmConnectionSettings;
        bool enabled=false;
        NEXUS_Error errCode;
        BAPE_RfModHandle apeRfmHandle=NULL;
        BAPE_RfModSettings apeRfmSettings;
        int i;

        if ( NULL == g_NEXUS_audioModuleData.settings.modules.rfm )
        {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }

        for ( i = 0; i < NEXUS_NUM_RFM_OUTPUTS; i++ )
        {
            if ( g_rfmMapping[i].output == slaveHandle )
            {
                apeRfmHandle = g_rfmMapping[i].handle;
                break;
            }
        }

        BDBG_ASSERT(NULL != apeRfmHandle);

        BAPE_RfMod_GetSettings(apeRfmHandle, &apeRfmSettings);

        if ( NULL == sourceHandle )
        {
            apeRfmSettings.master = NULL;
            apeRfmSettings.muted = true;
        }
        else
        {
            BDBG_ASSERT(sourceHandle->objectType == NEXUS_AudioOutputType_eDac);
            enabled = true;
            apeRfmSettings.master = (BAPE_OutputPort)sourceHandle->port;
            apeRfmSettings.muted = false;
        }

        errCode = BAPE_RfMod_SetSettings(apeRfmHandle, &apeRfmSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        /* Tell RFM to mute or unmute appropriately */
        NEXUS_Module_Lock(g_NEXUS_audioModuleData.settings.modules.rfm);
        NEXUS_Rfm_GetConnectionSettings_priv(slaveHandle->pObjectHandle, &rfmConnectionSettings);
        rfmConnectionSettings.audioEnabled = enabled;
        errCode = NEXUS_Rfm_SetConnectionSettings_priv(slaveHandle->pObjectHandle, &rfmConnectionSettings);
        NEXUS_Module_Unlock(g_NEXUS_audioModuleData.settings.modules.rfm);

        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
        #endif
    }
    else if ( NEXUS_AudioOutput_P_IsSpdifSlave(slaveHandle) )
    {
        #if NEXUS_NUM_AUDIO_RETURN_CHANNEL
            NEXUS_Error errCode;
    
            errCode = NEXUS_AudioReturnChannel_P_SetMaster( slaveHandle, sourceHandle );
                
            if ( errCode )
            {
                return BERR_TRACE(errCode);
            }        
        #endif
    }

    return BERR_SUCCESS;
}

NEXUS_AudioOutputData *NEXUS_AudioOutput_P_CreateData(NEXUS_AudioOutput output)
{
    NEXUS_AudioOutputData *pData;
    if ( NULL == output->pMixerData )
    {
        output->pMixerData = pData = BKNI_Malloc(sizeof(NEXUS_AudioOutputData));
        if ( pData )
        {
            BKNI_Memset(pData, 0, sizeof(NEXUS_AudioOutputData));
            BDBG_OBJECT_SET(pData, NEXUS_AudioOutputData);
            NEXUS_AudioOutput_GetDefaultSettings( &pData->settings);
            pData->sampleRate = BAVC_AudioSamplingRate_eUnknown;
            switch ( output->objectType )
            {
            #if NEXUS_NUM_HDMI_OUTPUTS  /* TODO */
            case NEXUS_AudioOutputType_eHdmi:
                #if NEXUS_NUM_HDMI_OUTPUTS != 1
                #error More than one HDMI output is not supported
                #endif
                BDBG_ASSERT(g_hdmiMapping[0].output == NULL);
                {
                    BERR_Code errCode;
                    BAPE_OutputPort mixerOutput;
                    BAPE_MaiOutputInterruptHandlers interrupts;

                    BDBG_MSG(("Creating MAI handle"));
                    errCode = BAPE_MaiOutput_Open(NEXUS_AUDIO_DEVICE_HANDLE, 0, NULL, &g_hdmiMapping[0].handle);
                    if ( errCode )
                    {
                        BDBG_ERR(("Unable to open MAI Output"));
                        BKNI_Free(output->pMixerData);
                        output->pMixerData = NULL;
                        return NULL;
                    }

                    errCode = BKNI_CreateEvent(&g_hdmiMapping[0].settingsChangedEvent);
                    if ( errCode )
                    {
                        errCode = BERR_TRACE(errCode);
                        BAPE_MaiOutput_Close(g_hdmiMapping[0].handle);
                        BKNI_Free(output->pMixerData);
                        output->pMixerData = NULL;
                        return NULL;
                    }

                    g_hdmiMapping[0].settingsChangedCallback = NEXUS_RegisterEvent(g_hdmiMapping[0].settingsChangedEvent, NEXUS_AudioOutput_P_HdmiSettingsChanged, output);
                    if ( NULL == g_hdmiMapping[0].settingsChangedCallback )
                    {
                        errCode = BERR_TRACE(errCode);
                        BKNI_DestroyEvent(g_hdmiMapping[0].settingsChangedEvent);
                        BAPE_MaiOutput_Close(g_hdmiMapping[0].handle);
                        BKNI_Free(output->pMixerData);
                        output->pMixerData = NULL;
                        return NULL;
                    }

                    BAPE_MaiOutput_GetInterruptHandlers(g_hdmiMapping[0].handle, &interrupts);
                    interrupts.sampleRate.pCallback_isr = NEXUS_AudioOutput_P_HdmiSampleRateChange_isr;
                    interrupts.sampleRate.pParam1 = output->pObjectHandle;
                    interrupts.sampleRate.param2 = (int)output;
                    errCode = BAPE_MaiOutput_SetInterruptHandlers(g_hdmiMapping[0].handle, &interrupts);
                    if ( errCode )
                    {
                        BDBG_ERR(("Unable to register sample rate interrupt"));
                        NEXUS_UnregisterEvent(g_hdmiMapping[0].settingsChangedCallback);
                        BKNI_DestroyEvent(g_hdmiMapping[0].settingsChangedEvent);
                        BAPE_MaiOutput_Close(g_hdmiMapping[0].handle);
                        BKNI_Free(output->pMixerData);
                        output->pMixerData = NULL;
                        return NULL;
                    }                    
                    g_hdmiMapping[0].output = output;
                    BAPE_MaiOutput_GetOutputPort(g_hdmiMapping[0].handle, &mixerOutput);
                    output->port = (uint32_t)mixerOutput;

                    NEXUS_Module_Lock(g_NEXUS_audioModuleData.settings.modules.hdmiOutput);
                    NEXUS_HdmiOutput_SetNotifyAudioEvent_priv(output->pObjectHandle, g_hdmiMapping[0].settingsChangedEvent);
                    NEXUS_Module_Unlock(g_NEXUS_audioModuleData.settings.modules.hdmiOutput);
                }
                break;
            #endif
            #if NEXUS_NUM_RFM_OUTPUTS
            case NEXUS_AudioOutputType_eRfm:
                #if NEXUS_NUM_RFM_OUTPUTS != 1
                #error More than one RFM output is not supported
                #endif
                BDBG_ASSERT(g_rfmMapping[0].output == NULL);
                {
                    BERR_Code errCode;
                    BDBG_MSG(("Creating RFM handle"));
                    errCode = BAPE_RfMod_Open(NEXUS_AUDIO_DEVICE_HANDLE, 0, NULL, &g_rfmMapping[0].handle);
                    if ( errCode )
                    {
                        BDBG_ERR(("Unable to open MAI Output"));
                        BKNI_Free(output->pMixerData);
                        output->pMixerData = NULL;
                        return NULL;
                    }
                    g_rfmMapping[0].output = output;
                    output->port = 0;
                }
                break;
            #endif
            default:
                break;
            }
        }
        else
        {
            NEXUS_Error errCode;
            errCode = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            return NULL;
        }
    }

    return output->pMixerData;
}

void NEXUS_AudioOutput_GetSyncSettings_priv( NEXUS_AudioOutput audioOutput, NEXUS_AudioOutputSyncSettings *pSyncSettings )
{
    NEXUS_AudioOutputData *pData = audioOutput->pMixerData;
    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
    *pSyncSettings = pData->syncSettings;
}

NEXUS_Error NEXUS_AudioOutput_SetSyncSettings_priv( NEXUS_AudioOutput audioOutput, const NEXUS_AudioOutputSyncSettings *pSyncSettings )
{
    NEXUS_AudioOutputData *pData = audioOutput->pMixerData;

    NEXUS_ASSERT_MODULE();
    if (!pData) return 0; /* TODO */

    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
    pData->syncSettings = *pSyncSettings;

    BKNI_EnterCriticalSection();
    /* fire right away. always "on". */
    if (pSyncSettings->startCallback_isr) {
        (*pSyncSettings->startCallback_isr)(pSyncSettings->callbackContext, 0);
    }
    BKNI_LeaveCriticalSection();

    /* set delay + additionalDelay */
    if ( !g_NEXUS_audioModuleData.settings.independentDelay && 
         (pData->settings.additionalDelay || pData->syncSettings.delay) )
    {
        BDBG_ERR(("Independent output delay is not enabled, ignoring delay.  Please check NEXUS_AudioModuleSettings.independentDelay"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    /* Applying remaining settings should be deferred until the next start call if we're running. */
    if ( pData->input )
    {
        NEXUS_AudioInput_P_OutputSettingsChanged(pData->input, audioOutput);
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_AudioOutput_GetSyncStatus_isr( NEXUS_AudioOutput audioOutput, NEXUS_AudioOutputSyncStatus *pSyncStatus )
{

    BDBG_ASSERT(NULL != audioOutput);
    BDBG_ASSERT(NULL != pSyncStatus);

    /* Default to PCM */
    pSyncStatus->compressed = false;

    /* Look for connected input.  If found, determine format from AudioInput. */
    if ( NULL != audioOutput->pMixerData )
    {
        NEXUS_AudioOutputData *pData;

        pData = audioOutput->pMixerData;
        BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
        if ( pData->input != NULL )
        {
            NEXUS_AudioInputFormat format = NEXUS_AudioInput_P_GetFormat(pData->input);

            if ( format == NEXUS_AudioInputFormat_eCompressed )
            {
                pSyncStatus->compressed = true;
            }
        }
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_AudioOutput_GetStatus( NEXUS_AudioOutput audioOutput, NEXUS_AudioOutputStatus *pStatus )
{
    NEXUS_AudioOutputData *pData = audioOutput->pMixerData;
    if (!pData) {
        BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    }
    else {
        BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
        *pStatus = pData->status;
    }
    return 0;
}

void NEXUS_AudioOutput_P_SetVcxo(
    unsigned outputTimebase
    )
{
#if 0 /* TODO */
    uint32_t reg;
    BREG_Handle hRegister = g_pCoreHandles->reg;

    /* Hack since we lack a PI for setting Vcxo */
    /* set the VCXO Timebase for Rate Manager 0 */
    /* TODO: what about chips w/ 2nd Rate Managers, do we need to use its VCXO */
    BKNI_EnterCriticalSection();
    reg = BREG_Read32(hRegister, BCHP_VCXO_0_RM_CONTROL);
    reg &= ~(BCHP_MASK(VCXO_0_RM_CONTROL,TIMEBASE));
    reg |= BCHP_FIELD_DATA(VCXO_0_RM_CONTROL,TIMEBASE, outputTimebase);
    BREG_Write32(hRegister, BCHP_VCXO_0_RM_CONTROL, reg);
    BKNI_LeaveCriticalSection();
    BDBG_MSG(("%s: Set Rate Manager 0's VCXO to sync with Timebase %d", __FUNCTION__, outputTimebase));
#else
    BSTD_UNUSED(outputTimebase);
#endif
}

#if 0 /* TODO */
NEXUS_Error NEXUS_AudioOutput_P_SetHbrMode(
    NEXUS_AudioOutput output,
    bool bEnableHbr
    )
{
    NEXUS_Error errCode;

    BRAP_OutputPortConfig outputSettings;
    BDBG_ASSERT(NULL != output);

    if ( output->objectType == NEXUS_AudioOutputType_eHdmi ||
         output->objectType == NEXUS_AudioOutputType_eSpdif )
    {
        errCode = BRAP_GetCurrentOutputConfig(g_NEXUS_audioModuleData.hRap,
                                              output->port,
                                              &outputSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }

        outputSettings.bHbrEnable = bEnableHbr;

        errCode = BRAP_SetOutputConfig(g_NEXUS_audioModuleData.hRap,
                                       &outputSettings);
        if ( errCode )
        {
            return BERR_TRACE(errCode);
        }
    }
    else if ( bEnableHbr )
    {
        BDBG_ERR(("Can not enable HBR mode for outputs other than HDMI or SPDIF."));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    return BERR_SUCCESS;
}
#endif

NEXUS_Error NEXUS_AudioOutput_P_SetTrickMute(
    NEXUS_AudioOutput output,
    bool mute
    )
{
    BAPE_OutputVolume volume;
    NEXUS_AudioOutputData *pData;
    NEXUS_Error errCode;

    if ( NEXUS_AudioOutput_P_IsDacSlave(output) ||
         NEXUS_AudioOutput_P_IsSpdifSlave(output) )
    {
        return NEXUS_SUCCESS;
    }

    if ( NULL == output->pMixerData )
    {
        pData = NEXUS_AudioOutput_P_CreateData(output);
        if ( NULL == pData )
        {
            return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        }
    }

    pData = output->pMixerData;
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);

    BDBG_MSG(("Setting output mute to %d for trick", pData->settings.muted || mute));
    BAPE_GetOutputVolume((BAPE_OutputPort)output->port, &volume);
    volume.muted = pData->settings.muted || mute;
    errCode = BAPE_SetOutputVolume((BAPE_OutputPort)output->port, &volume);
    if ( errCode )
    {
        return BERR_TRACE(errCode);
    }

    pData->trickMute = mute;
    return NEXUS_SUCCESS;
}

/***************************************************************************
Summary:
    Determine if an output is connected to any inputs
 ***************************************************************************/
bool NEXUS_AudioOutput_P_IsConnected(
    NEXUS_AudioOutput output
    )
{
    NEXUS_AudioOutputData *pData;

    BDBG_ASSERT(NULL != output);

    pData = output->pMixerData;
    if ( NULL != pData && NULL != pData->input )
    {
        BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
        return true;
    }

    return false;
}

#if NEXUS_NUM_HDMI_OUTPUTS
static void NEXUS_AudioOutput_P_HdmiSampleRateChange_isr(void *pParam1, int param2, unsigned sampleRate)
{
    BAVC_AudioSamplingRate avcRate;
    NEXUS_AudioOutput output;
    NEXUS_AudioOutputData *pData;

    BSTD_UNUSED(pParam1);

    BDBG_MSG(("HDMI Sample Rate Change %u", sampleRate));

    avcRate = NEXUS_AudioOutput_P_GetSampleRateEnum_isr(sampleRate);
    output=(NEXUS_AudioOutput)param2;
    pData = output->pMixerData;
    BDBG_ASSERT(NULL != pData);
    pData->sampleRate = avcRate;

    NEXUS_HdmiOutput_AudioSampleRateChange_isr(output->pObjectHandle, avcRate);
}

static void NEXUS_AudioOutput_P_HdmiSettingsChanged(void *pOutput)
{
    NEXUS_AudioOutput output = (NEXUS_AudioOutput)pOutput;
    NEXUS_AudioOutputSettings outputSettings;

    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);
    BDBG_ASSERT(output->objectType == NEXUS_AudioOutputType_eHdmi);
    
    BDBG_MSG(("HDMI audio settings have changed.  Updating."));

    /* this will force the settings to be applied to the PI */
    NEXUS_AudioOutput_GetSettings(output, &outputSettings);
    NEXUS_AudioOutput_SetSettings(output, &outputSettings);
}
#endif

void NEXUS_AudioOutput_P_GetMixerSettings(NEXUS_AudioOutput output, BAPE_MixerSettings *pSettings /* [out] */)
{
    NEXUS_AudioOutputData *pData;
    BDBG_ASSERT(NULL != output);
    pData = output->pMixerData;
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
    BAPE_Mixer_GetDefaultSettings(pSettings);
    pSettings->defaultSampleRate = pData->settings.defaultSampleRate;
    pSettings->outputPll = pData->settings.pll;
    pSettings->outputNco = pData->settings.nco;
    pSettings->outputTimebase = pData->settings.timebase;
    if ( NULL == NEXUS_GetEnv("no_independent_delay") )
    {
        pSettings->outputDelay = pData->settings.additionalDelay;
    }
}

void NEXUS_AudioOutput_P_SetOutputFormat(NEXUS_AudioOutput output, NEXUS_AudioInputFormat format)
{
    BDBG_ASSERT(NULL != output);
#if NEXUS_NUM_HDMI_OUTPUTS
    if ( output->objectType == NEXUS_AudioOutputType_eHdmi )
    {
        NEXUS_Module_Lock(g_NEXUS_audioModuleData.settings.modules.hdmiOutput);
        {
            BAVC_AudioSamplingRate sampleRate = BAVC_AudioSamplingRate_e48k;
            NEXUS_AudioOutputData *pData = output->pMixerData;
            unsigned numberOfChannels;

            switch ( format )
            {
            case NEXUS_AudioInputFormat_ePcm5_1:
                numberOfChannels = 6;
                break;
            case NEXUS_AudioInputFormat_ePcm7_1:
                numberOfChannels = 8;
                break;
            default:
                numberOfChannels = 2;
                break;
            }

            if ( pData )
            {
                if ( pData->sampleRate == BAVC_AudioSamplingRate_eUnknown )
                {
                sampleRate = NEXUS_AudioOutput_P_GetSampleRateEnum(pData->settings.defaultSampleRate);
                }
                else
                {
                    sampleRate = pData->sampleRate;
                }
            }
            BDBG_MSG(("Set HDMI audio format to %s - %u channels", format==NEXUS_AudioInputFormat_eCompressed?"Compressed":"PCM",numberOfChannels));
            /* Eventually, audio decoder will need to set the proper compressed format */
            NEXUS_HdmiOutput_SetAudioParams_priv(output->pObjectHandle, BAVC_AudioBits_e16, sampleRate,
                                                 format == NEXUS_AudioInputFormat_eCompressed?BAVC_AudioFormat_eAC3:BAVC_AudioFormat_ePCM,numberOfChannels);
        }
        NEXUS_Module_Unlock(g_NEXUS_audioModuleData.settings.modules.hdmiOutput);
    }
#else
    BSTD_UNUSED(output);
    BSTD_UNUSED(format);
#endif
}

bool NEXUS_AudioOutput_P_IsRunning(
    NEXUS_AudioOutput output
    )
{
    NEXUS_AudioOutputData *pData;
    
    BDBG_OBJECT_ASSERT(output, NEXUS_AudioOutput);
    
    pData = output->pMixerData;
    if ( NULL == pData )
    {
        /* No connection can have been made, so we can't be running */
        return false;
    }
    BDBG_OBJECT_ASSERT(pData, NEXUS_AudioOutputData);
    if ( NULL == pData->input )
    {
        /* There is no input, can't be running */
        return false;
    }
    
    /* We're running if our input is running. */
    return NEXUS_AudioInput_P_IsRunning(pData->input);
}
