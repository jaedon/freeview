/******************************************************************************
 *    (c)2012 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: b_simpledecoder.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 7/20/12 8:42a $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/examples/multiprocess/b_simpledecoder.c $
 * 
 * 4   7/20/12 8:42a erickson
 * SW7425-3515: add syncchannel support to refsw_server
 * 
 * 3   7/9/12 8:46a jgarrett
 * SW7231-872: Merge to main branch
 * 
 * SW7231-872/4   7/2/12 2:17p marcusk
 * SW7231-872: Added debug output and fixed HDMI initialization
 * 
 * SW7231-872/3   7/2/12 11:37a marcusk
 * SW7231-872: update with a few fixes and improvements.  Supports multi-
 *  channel PCM output over HDMI
 * 
 * SW7231-872/2   6/26/12 4:48p marcusk
 * SW7231-872: Fixed some minor bugs and added better defaults for HDMI
 * 
 * SW7231-872/1   6/25/12 4:26p marcusk
 * SW7231-872: Default behavior is to passthrough AC3 and DTS streams to
 *  SPDIF output.
 * 
 * 1   6/13/12 3:07p erickson
 * SW7425-2981: refactor into b_simpledecoder wrapper library
 * 
 *****************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "b_simpledecoder.h"
#include "nexus_simple_video_decoder_server.h"
#include "nexus_simple_audio_decoder_server.h"
#include "nexus_audio_output.h"
#include "nexus_audio_dac.h"
#include "nexus_platform.h"
#include "nexus_mosaic_video_decoder.h"

BDBG_MODULE(b_simpledecoder);

int b_simpledecoder_init(b_simpledecoder_state *state, unsigned id)
{
    NEXUS_SimpleVideoDecoderServerSettings videoSettings;
    NEXUS_SimpleAudioDecoderServerSettings audioSettings;
    NEXUS_SimpleAudioPlaybackServerSettings audioPlaybackSettings;
    unsigned i;
    int rc;
    
    BKNI_Memset(state, 0, sizeof(*state));

    NEXUS_SimpleVideoDecoder_GetDefaultServerSettings(&videoSettings);
    for (i=0;i<B_MAX_VIDEO_WINDOWS;i++) {
        state->id.simpleVideoDecoder[i] = id+i;
        state->simpleVideoDecoder[i] = NEXUS_SimpleVideoDecoder_Create(state->id.simpleVideoDecoder[i], &videoSettings);
        if (!state->simpleVideoDecoder[i]) {
            rc = BERR_TRACE(NEXUS_UNKNOWN);
            goto error;
        }
    }

    NEXUS_SimpleAudioDecoder_GetDefaultServerSettings(&audioSettings);
    state->id.simpleAudioDecoder = id;
    state->simpleAudioDecoder = NEXUS_SimpleAudioDecoder_Create(state->id.simpleAudioDecoder, &audioSettings);
    if (!state->simpleAudioDecoder) {
        rc = BERR_TRACE(NEXUS_UNKNOWN);
        goto error;
    }

    NEXUS_SimpleAudioPlayback_GetDefaultServerSettings(&audioPlaybackSettings);
    for (i=0;i<B_MAX_AUDIO_PLAYBACKS;i++) {
        /* always open B_MAX_AUDIO_PLAYBACKS for now */
        audioPlaybackSettings.decoder = state->simpleAudioDecoder;
        state->id.simpleAudioPlayback[i] = id+i;
        state->simpleAudioPlayback[i] = NEXUS_SimpleAudioPlayback_Create(state->id.simpleAudioPlayback[i], &audioPlaybackSettings);
    }
    
    /* ID's id through id+B_MAX_VIDEO_WINDOWS-1 are used */
    
    return 0;
    
error:
    b_simpledecoder_uninit(state);
    return rc;
}

void b_simpledecoder_uninit(b_simpledecoder_state *state)
{
    unsigned i;
    
    for (i=0;i<B_MAX_VIDEO_WINDOWS;i++) {
        if (state->simpleVideoDecoder[i]) {
            NEXUS_SimpleVideoDecoder_Destroy(state->simpleVideoDecoder[i]);
        }
    }
    for (i=0;i<B_MAX_AUDIO_PLAYBACKS;i++) {
        NEXUS_SimpleAudioPlayback_Destroy(state->simpleAudioPlayback[i]);
    }
    if (state->simpleAudioDecoder) {
        NEXUS_SimpleAudioDecoder_Destroy(state->simpleAudioDecoder);
    }
    BKNI_Memset(state, 0, sizeof(*state));
}

void b_simpledecoder_get_default_config(b_simpledecoder_config *config)
{
    /* Default to mixed PCM */
    BKNI_Memset(config, 0, sizeof(*config));
    /* We are assuming spdif can decode AC3 and DTS audio */
    /* TODO: Add transcode support for DtsLegacy */
    config->spdif.audioCodecOutput[NEXUS_AudioCodec_eAacAdts] =
    config->spdif.audioCodecOutput[NEXUS_AudioCodec_eAacLoas] =
    config->spdif.audioCodecOutput[NEXUS_AudioCodec_eAacPlusLoas] =
    config->spdif.audioCodecOutput[NEXUS_AudioCodec_eAacPlusAdts] = b_audio_output_transcode;
    config->spdif.audioCodecOutput[NEXUS_AudioCodec_eAc3] = b_audio_output_passthrough;
    config->spdif.audioCodecOutput[NEXUS_AudioCodec_eDts] = b_audio_output_passthrough;
    /* Don't worry, DTS is layered and we'll only send the base out SPDIF */
    config->spdif.audioCodecOutput[NEXUS_AudioCodec_eDtsHd] = b_audio_output_passthrough;

}

int b_simpledecoder_set_resources(b_simpledecoder_state *state, bool enabled, const b_simpledecoder_resources *resources)
{
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_SimpleVideoDecoderServerSettings videoSettings[B_MAX_VIDEO_WINDOWS];
    NEXUS_SimpleAudioDecoderServerSettings audioSettings;
    NEXUS_SimpleAudioPlaybackServerSettings audioPlaybackSettings[B_MAX_AUDIO_PLAYBACKS];
    int rc;
    unsigned i, j;
    
    for (i=0;i<B_MAX_VIDEO_WINDOWS;i++) {
        if (state->simpleVideoDecoder[i]) {
            NEXUS_SimpleVideoDecoder_GetServerSettings(state->simpleVideoDecoder[i], &videoSettings[i]);
        }
    }
    NEXUS_SimpleAudioDecoder_GetServerSettings(state->simpleAudioDecoder, &audioSettings);
    for (i=0;i<B_MAX_AUDIO_PLAYBACKS;i++) {
        NEXUS_SimpleAudioPlayback_GetServerSettings(state->simpleAudioPlayback[i], &audioPlaybackSettings[i]);
    }
    
    if (enabled == state->enabled) {
        return 0;
    }
    
    NEXUS_Platform_GetConfiguration(&platformConfig);
    
    if (enabled) {
        for (i=0;i<B_MAX_VIDEO_WINDOWS;i++) {
            videoSettings[i].videoDecoder = resources->videoDecoder[i];
            for (j=0;j<NEXUS_NUM_DISPLAYS;j++) {
                videoSettings[i].window[j] = resources->display[j].window[i].window;
            }
        }
        
        if (resources->ac3Encoder) {
            NEXUS_Ac3Encode_AddInput(resources->ac3Encoder,
                NEXUS_AudioDecoder_GetConnector(resources->audioDecoder0, NEXUS_AudioConnectorType_eMultichannel));
        }
        
        audioSettings.primary = resources->audioDecoder0;
        audioSettings.secondary = resources->audioDecoder1;

        if (resources->audioMixer) {
            rc = NEXUS_AudioMixer_AddInput(resources->audioMixer, NEXUS_AudioDecoder_GetConnector(resources->audioDecoder0, NEXUS_AudioConnectorType_eStereo));
            BDBG_ASSERT(!rc);
            for (i=0;i<B_MAX_AUDIO_PLAYBACKS;i++) {
                rc = NEXUS_AudioMixer_AddInput(resources->audioMixer, NEXUS_AudioPlayback_GetConnector(resources->audioPlayback[i]));
                BDBG_ASSERT(!rc);
            }
            rc = NEXUS_AudioOutput_AddInput(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]), NEXUS_AudioMixer_GetConnector(resources->audioMixer));
            BDBG_ASSERT(!rc);
        }

        for (i=0;i<B_MAX_AUDIO_PLAYBACKS;i++) {
            audioPlaybackSettings[i].decoder = state->simpleAudioDecoder;
            audioPlaybackSettings[i].playback = resources->audioPlayback[i];
        }
    }
    else {
        /* unassign from structs, but don't close here */
        for (i=0;i<B_MAX_VIDEO_WINDOWS;i++) {
            videoSettings[i].videoDecoder = NULL;
            for (j=0;j<NEXUS_NUM_DISPLAYS;j++) {
                videoSettings[i].window[j] = NULL;
            }
        }
        
        audioSettings.primary = NULL;
        audioSettings.secondary = NULL;
        BKNI_Memset(&audioSettings.spdif, 0, sizeof(audioSettings.spdif));
        BKNI_Memset(&audioSettings.hdmi, 0, sizeof(audioSettings.hdmi));
        for (i=0;i<B_MAX_AUDIO_PLAYBACKS;i++) {
            audioPlaybackSettings[i].playback = NULL;
        }
    }

    for (i=0;i<B_MAX_VIDEO_WINDOWS;i++) {
        if (state->simpleVideoDecoder[i]) {
            rc = NEXUS_SimpleVideoDecoder_SetServerSettings(state->simpleVideoDecoder[i], &videoSettings[i]);
            if (rc) return BERR_TRACE(rc);
        }
    }
    rc = NEXUS_SimpleAudioDecoder_SetServerSettings(state->simpleAudioDecoder, &audioSettings);
    if (rc) return BERR_TRACE(rc);
    for (i=0;i<B_MAX_AUDIO_PLAYBACKS;i++) {
        rc = NEXUS_SimpleAudioPlayback_SetServerSettings(state->simpleAudioPlayback[i], &audioPlaybackSettings[i]);
        if (rc) return BERR_TRACE(rc);
    }
    
    if (!enabled) {    
        if (resources->ac3Encoder) {
            NEXUS_Ac3Encode_RemoveAllInputs(resources->ac3Encoder);
        }
        if (resources->audioMixer) {
            NEXUS_AudioMixer_RemoveAllInputs(resources->audioMixer);
            NEXUS_AudioOutput_RemoveAllInputs(NEXUS_AudioDac_GetConnector(platformConfig.outputs.audioDacs[0]));
        }
    }
    
    state->enabled = enabled;

    return 0;
}

int b_simpledecoder_set_config(b_simpledecoder_state *state, const b_simpledecoder_config *config, const b_simpledecoder_resources *resources)
{
    NEXUS_SimpleAudioDecoderServerSettings audioSettings;
    NEXUS_PlatformConfiguration platformConfig;
    unsigned i;
    int rc;
    
    NEXUS_Platform_GetConfiguration(&platformConfig);
    NEXUS_SimpleAudioDecoder_GetServerSettings(state->simpleAudioDecoder, &audioSettings);
    
    /* set up per codec outputs for HDMI and SPDIF */
    audioSettings.hdmi.outputs[0] = platformConfig.outputs.hdmi[0];
    if (platformConfig.outputs.hdmi[0]) {
        BDBG_MSG(("HDMI output config:"));
        for (i=0;i<NEXUS_AudioCodec_eMax;i++) {
            BDBG_MSG(("%d: %d", i, config->hdmi.audioCodecOutput[i]));
            if (resources->audioMixer) {
                audioSettings.hdmi.input[i] = NEXUS_AudioMixer_GetConnector(resources->audioMixer);
            }
            else {
                audioSettings.hdmi.input[i] = NEXUS_AudioDecoder_GetConnector(resources->audioDecoder0, NEXUS_AudioConnectorType_eStereo);
            }
            switch (config->hdmi.audioCodecOutput[i]) {
            case b_audio_output_transcode:
                if (resources->ac3Encoder) {
                    audioSettings.hdmi.input[i] = NEXUS_Ac3Encode_GetConnector(resources->ac3Encoder);
                }
                break;
            case b_audio_output_passthrough:
                if (resources->audioDecoder1) {
                    /* TODO: some receivers may have an issue with all DTSHD formats being sent with
                     * eCompressed16x (although the spec says it should be supported). */
                    if (i==NEXUS_AudioCodec_eDtsHd || i==NEXUS_AudioCodec_eMlp) {
                    audioSettings.hdmi.input[i] = NEXUS_AudioDecoder_GetConnector(resources->audioDecoder1, NEXUS_AudioConnectorType_eCompressed16x);
                        if (audioSettings.hdmi.input[i] == NULL) {
                            /* Some older chips do not support eCompressed16x HBR passthrough. Revert
                             * to standard compressed output and the audio block with automatically
                             * send what is possible. */
                            audioSettings.hdmi.input[i] = NEXUS_AudioDecoder_GetConnector(resources->audioDecoder1, NEXUS_AudioConnectorType_eCompressed);
                        }
                    }
                    else {
                        audioSettings.hdmi.input[i] = NEXUS_AudioDecoder_GetConnector(resources->audioDecoder1, NEXUS_AudioConnectorType_eCompressed);
                    }
                }
                break;
            case b_audio_output_pcm:
                audioSettings.hdmi.input[i] = NEXUS_AudioDecoder_GetConnector(resources->audioDecoder0, NEXUS_AudioConnectorType_eStereo);
                break;
            case b_audio_output_multi_pcm:
                audioSettings.hdmi.input[i] = NEXUS_AudioDecoder_GetConnector(resources->audioDecoder0, NEXUS_AudioConnectorType_eMultichannel);
                break;
            default:
                break;
            }
        }
    }
      
    audioSettings.spdif.outputs[0] = platformConfig.outputs.spdif[0];
    if (platformConfig.outputs.spdif[0]) {
        BDBG_MSG(("SPDIF output config:"));
        for (i=0;i<NEXUS_AudioCodec_eMax;i++) {
            BDBG_MSG(("%d: %d", i, config->spdif.audioCodecOutput[i]));
            if (resources->audioMixer) {
                audioSettings.spdif.input[i] = NEXUS_AudioMixer_GetConnector(resources->audioMixer);
            }
            else {
                audioSettings.spdif.input[i] = NEXUS_AudioDecoder_GetConnector(resources->audioDecoder0, NEXUS_AudioConnectorType_eStereo);
            }
            switch (config->spdif.audioCodecOutput[i]) {
            case b_audio_output_transcode:
                if (resources->ac3Encoder) {
                    audioSettings.spdif.input[i] = NEXUS_Ac3Encode_GetConnector(resources->ac3Encoder);
                }
                break;
            case b_audio_output_passthrough:
                if (resources->audioDecoder1) {
                    audioSettings.spdif.input[i] = NEXUS_AudioDecoder_GetConnector(resources->audioDecoder1, NEXUS_AudioConnectorType_eCompressed);
                }
                break;
            case b_audio_output_pcm:
                audioSettings.spdif.input[i] = NEXUS_AudioDecoder_GetConnector(resources->audioDecoder0, NEXUS_AudioConnectorType_eStereo);
                break;
            default:
                break;
            }
        }
    }
    rc = NEXUS_SimpleAudioDecoder_SetServerSettings(state->simpleAudioDecoder, &audioSettings);
    if (rc) return BERR_TRACE(rc);
    
    return 0;
}

#if NEXUS_HAS_HDMI_OUTPUT
int b_simpledecoder_hdmi_edid_config(b_simpledecoder_config *config, const NEXUS_HdmiOutputStatus *pStatus, const b_simpledecoder_resources *resources )
{
    unsigned i;
    b_audio_output defaultAO;

    if (pStatus->maxAudioPcmChannels > 2) {
        defaultAO = b_audio_output_multi_pcm;
    }
    else {
        defaultAO = b_audio_output_mixed_pcm;
    }

    for (i=0;i<NEXUS_AudioCodec_eMax;i++) {
        config->hdmi.audioCodecOutput[i] = defaultAO;
        if (pStatus->audioCodecSupported[i]) {
            switch (i) {
            case NEXUS_AudioCodec_eAacAdts:
            case NEXUS_AudioCodec_eAacLoas:
            case NEXUS_AudioCodec_eAacPlusAdts:
            case NEXUS_AudioCodec_eAacPlusLoas:
                /* These codecs support AC3 encoding.  If we can't send them out
                 * as multi-channel PCM, lets transcode to AC3 to allow 5.1 output */
                if (resources->ac3Encoder && pStatus->maxAudioPcmChannels <= 2) {
                    config->hdmi.audioCodecOutput[i] = b_audio_output_transcode;
                }
                break;
            default:    
                config->hdmi.audioCodecOutput[i] = b_audio_output_passthrough;
                break;
            }
        }
        else {
            /* Special cases when native codec is not supported */
            switch(i) {
            case NEXUS_AudioCodec_eAc3Plus:
                /* transcode DDP to AC3 if receiver only supports the latter */
                if (resources->ac3Encoder && pStatus->audioCodecSupported[NEXUS_AudioCodec_eAc3]) {
                    config->hdmi.audioCodecOutput[i] = b_audio_output_transcode;
                }
                break;
            case NEXUS_AudioCodec_eDtsHd:
                /* DTS is layered so the RX should be able to handle just the baselayer out HDMI */
                if (pStatus->audioCodecSupported[NEXUS_AudioCodec_eDts]) {
                    config->hdmi.audioCodecOutput[i] = b_audio_output_passthrough;
                }
                break;
            default:
                break;
            }
        }
    }
    return 0;
}
#endif

void b_simpledecoder_get_default_resources_config(b_simpledecoder_resources_config *config)
{
    BKNI_Memset(config, 0, sizeof(*config));
    config->numDisplays = NEXUS_NUM_DISPLAYS>1?2:1; /* avoid transcode displays */
    config->numVideoDecoders = NEXUS_NUM_VIDEO_DECODERS; /* 1 or 2 (non-mosaic) */
}

int b_simpledecoder_resources_open(b_simpledecoder_resources *resources, const b_simpledecoder_resources_config *config)
{
    b_simpledecoder_resources_config default_config;
    NEXUS_PlatformConfiguration platformConfig;
    unsigned i;
    int rc;
    
    if (!config) {
        b_simpledecoder_get_default_resources_config(&default_config);
        config = &default_config;
    }
    
    if (config->numDisplays == 0 || config->numDisplays > NEXUS_MAX_DISPLAYS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (config->numVideoDecoders == 0 || config->numVideoDecoders > B_MAX_VIDEO_WINDOWS) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_Memset(resources, 0, sizeof(*resources));

    for (i=0;i<config->numVideoDecoders;i++) {
        NEXUS_VideoDecoderHandle videoDecoder;
        if (config->numVideoDecoders > NEXUS_NUM_VIDEO_WINDOWS) {
            NEXUS_VideoDecoderOpenMosaicSettings openSettings;
            NEXUS_VideoDecoder_GetDefaultOpenMosaicSettings(&openSettings);

            BDBG_MSG(("NEXUS_VideoDecoder_OpenMosaic 0, %d", i));
            videoDecoder = NEXUS_VideoDecoder_OpenMosaic(0, i, &openSettings);
            if (!videoDecoder) {
                rc = BERR_TRACE(NEXUS_UNKNOWN);
                goto error;
            }
        }
        else {
            BDBG_MSG(("NEXUS_VideoDecoder_Open %d", i));
            videoDecoder = NEXUS_VideoDecoder_Open(i, NULL);
            if (!videoDecoder) {
                rc = BERR_TRACE(NEXUS_UNKNOWN);
                goto error;
            }
        }
        resources->videoDecoder[i] = videoDecoder;
    }

    resources->audioDecoder0 = NEXUS_AudioDecoder_Open(0, NULL);
    if (!resources->audioDecoder0) {
        rc = BERR_TRACE(NEXUS_UNKNOWN);
        goto error;
    }
    resources->audioDecoder1 = NEXUS_AudioDecoder_Open(1, NULL);
    /* allow this to fail */
    
    resources->audioMixer = NEXUS_AudioMixer_Open(NULL);
    if (!resources->audioMixer) {
        rc = BERR_TRACE(NEXUS_UNKNOWN);
        goto error;
    }
    if (resources->audioMixer) {
        /* without a mixer, we can't add pcm playback */
        for (i=0;i<B_MAX_AUDIO_PLAYBACKS;i++) {
            NEXUS_AudioPlaybackOpenSettings openSettings;
            NEXUS_AudioPlayback_GetDefaultOpenSettings(&openSettings);
            openSettings.heap = config->heap;
            resources->audioPlayback[i] = NEXUS_AudioPlayback_Open(NEXUS_ANY_ID, &openSettings);
        }
    }
    
#if 0
/* TODO: not supported because dangling ac3Encoder causes problems */
    resources->ac3Encoder = NEXUS_Ac3Encode_Open(NULL);
#endif

    if (!NEXUS_GetEnv("sync_disabled")) { /* debug env variable from brutus */
        NEXUS_SyncChannelSettings syncChannelSettings;
        NEXUS_SyncChannel_GetDefaultSettings(&syncChannelSettings);
        resources->syncChannel = NEXUS_SyncChannel_Create(&syncChannelSettings);
        if (resources->syncChannel) {
            NEXUS_SyncChannel_GetSettings(resources->syncChannel, &syncChannelSettings);
            syncChannelSettings.videoInput = NEXUS_VideoDecoder_GetConnector(resources->videoDecoder[0]);
            syncChannelSettings.audioInput[0] = NEXUS_AudioDecoder_GetConnector(resources->audioDecoder0, NEXUS_AudioDecoderConnectorType_eStereo);
            if (resources->audioDecoder1) {
                syncChannelSettings.audioInput[1] = NEXUS_AudioDecoder_GetConnector(resources->audioDecoder1, NEXUS_AudioDecoderConnectorType_eCompressed);
            }
            NEXUS_SyncChannel_SetSettings(resources->syncChannel, &syncChannelSettings);
        }
    }

    return 0;
    
error:
    b_simpledecoder_resources_close(resources);
    return rc;
}

void b_simpledecoder_resources_close(b_simpledecoder_resources *resources)
{
    unsigned i;
    
    if (resources->syncChannel) {
        NEXUS_SyncChannel_Destroy(resources->syncChannel);
        resources->syncChannel = NULL;
    }
    for (i=0;i<B_MAX_VIDEO_WINDOWS;i++) {
        if (resources->videoDecoder[i]) {
            NEXUS_VideoDecoder_Close(resources->videoDecoder[i]);
        }
    }
    if (resources->ac3Encoder) {
        NEXUS_Ac3Encode_Close(resources->ac3Encoder);
    }
    for (i=0;i<B_MAX_AUDIO_PLAYBACKS;i++) {
        if (resources->audioPlayback[i]) {
            NEXUS_AudioPlayback_Close(resources->audioPlayback[i]);
        }
    }
    if (resources->audioMixer) {
        NEXUS_AudioMixer_Close(resources->audioMixer);
    }
    if (resources->audioDecoder1) {
        NEXUS_AudioDecoder_Close(resources->audioDecoder1);
    }
    if (resources->audioDecoder0) {
        NEXUS_AudioDecoder_Close(resources->audioDecoder0);
    }
    BKNI_Memset(resources, 0, sizeof(*resources));
}

