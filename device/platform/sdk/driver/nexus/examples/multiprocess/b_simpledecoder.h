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
 * $brcm_Workfile: b_simpledecoder.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 7/20/12 8:42a $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /nexus/examples/multiprocess/b_simpledecoder.h $
 * 
 * 4   7/20/12 8:42a erickson
 * SW7425-3515: add syncchannel support to refsw_server
 * 
 * 3   7/9/12 8:46a jgarrett
 * SW7231-872: Merge to main branch
 * 
 * SW7231-872/2   7/2/12 11:37a marcusk
 * SW7231-872: update with a few fixes and improvements.  Supports multi-
 *  channel PCM output over HDMI
 * 
 * 2   6/27/12 3:25p jgarrett
 * SW7231-872: Merge to main branch
 * 
 * SW7231-872/1   6/26/12 4:48p marcusk
 * SW7231-872: Fixed some minor bugs and added better defaults for HDMI
 * 
 * 1   6/13/12 3:07p erickson
 * SW7425-2981: refactor into b_simpledecoder wrapper library
 * 
 *****************************************************************************/
#ifndef B_SIMPLEDECODER_H__
#define B_SIMPLEDECODER_H__

/*****************************
* 
* b_simpledecoder is a wrapper library which configures a set of simpledecoder
* resources for a typical server app. 
* you can use the code directly or just as a reference.
* 
*****************************/

#include "nexus_memory.h"
#include "nexus_simple_video_decoder.h"
#include "nexus_simple_audio_decoder.h"
#include "nexus_simple_audio_playback.h"
#include "nexus_video_window.h"
#include "nexus_video_decoder.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_playback.h"
#include "nexus_audio_mixer.h"
#include "nexus_ac3_encode.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_spdif_output.h"
#include "nexus_sync_channel.h"

#ifdef __cplusplus
extern "C" {
#endif

#define B_MAX_AUDIO_PLAYBACKS 2
#define B_MAX_VIDEO_WINDOWS 16

/*****************************
* 
* b_simpledecoder_resources holds all of the nexus handles needed by b_simpledecoder
* to configure decode.
* 
* you can open the resources yourself and populate b_simpledecoder_resources, or call b_simpledecoder_resources_open.
* 
*****************************/

typedef struct b_simpledecoder_resources_config {
    unsigned numDisplays;
    unsigned numVideoDecoders; /* if numVideoDecoders > NEXUS_NUM_VIDEO_WINDOWS, use mosaic */
    NEXUS_HeapHandle heap; /* client accessible heap with eFull mapping for audio playback buffers */
} b_simpledecoder_resources_config;

/* transparent struct used by b_simpledecoder. 
you can manually populate, or call b_simpledecoder_resources_open to open in a default config */
typedef struct b_simpledecoder_resources {
    /* opened by b_simpledecoder_resources_open */
    NEXUS_VideoDecoderHandle videoDecoder[B_MAX_VIDEO_WINDOWS];
    NEXUS_AudioDecoderHandle audioDecoder0, audioDecoder1;
    NEXUS_AudioMixerHandle audioMixer;
    NEXUS_AudioMixerHandle audioMultiChannelMixer;
    NEXUS_Ac3EncodeHandle ac3Encoder;
    NEXUS_AudioPlaybackHandle audioPlayback[B_MAX_AUDIO_PLAYBACKS];
    NEXUS_SyncChannelHandle syncChannel;
    
    /* the following display resources must be opened by the app and assigned after b_simpledecoder_resources_open */
    struct {
        struct {
            NEXUS_VideoWindowHandle window;
        } window[B_MAX_VIDEO_WINDOWS];
    } display[NEXUS_MAX_DISPLAYS];
} b_simpledecoder_resources;

void b_simpledecoder_get_default_resources_config(b_simpledecoder_resources_config *config);
int b_simpledecoder_resources_open(b_simpledecoder_resources *resources, const b_simpledecoder_resources_config *config);
void b_simpledecoder_resources_close(b_simpledecoder_resources *resources);

/*****************************
* 
* b_simpledecoder_state holds a set of simple decoder resources
* 
*****************************/

typedef struct b_simpledecoder_state
{
    /* do not modify anything directly in this struct */
    
    NEXUS_SimpleVideoDecoderHandle simpleVideoDecoder[B_MAX_VIDEO_WINDOWS];
    NEXUS_SimpleAudioDecoderHandle simpleAudioDecoder;
    NEXUS_SimpleAudioPlaybackHandle simpleAudioPlayback[B_MAX_AUDIO_PLAYBACKS];
    
    bool enabled;
    
    struct {
        unsigned simpleVideoDecoder[B_MAX_VIDEO_WINDOWS];
        unsigned simpleAudioDecoder;
        unsigned simpleAudioPlayback[B_MAX_AUDIO_PLAYBACKS];
    } id;
} b_simpledecoder_state;

/* 
open b_simpledecoder wrapper
*/
int b_simpledecoder_init(b_simpledecoder_state *state, unsigned id);

/* 
close b_simpledecoder wrapper
*/
void b_simpledecoder_uninit(b_simpledecoder_state *state);

/*
connect simple decoder to the underlying resources
*/
int b_simpledecoder_set_resources(b_simpledecoder_state *state, bool enabled, const b_simpledecoder_resources *resources);

/*
configure simple decoder
*/
typedef enum b_audio_output {
    b_audio_output_mixed_pcm, /* 2 channel with sound effects */
    b_audio_output_pcm,       /* 2 channel no sound effects */
    b_audio_output_multi_pcm, /* multi-channel no sound effects */
    b_audio_output_passthrough,
    b_audio_output_transcode
} b_audio_output;

typedef struct b_simpledecoder_config
{
    struct {
        b_audio_output audioCodecOutput[NEXUS_AudioCodec_eMax];
    } hdmi, spdif;
} b_simpledecoder_config;
void b_simpledecoder_get_default_config(b_simpledecoder_config *config);
int b_simpledecoder_set_config(b_simpledecoder_state *state, const b_simpledecoder_config *config, const b_simpledecoder_resources *resources);

/* 
modify config->hdmi and config->spdif based on HDMI EDID capabilities
*/
#if NEXUS_HAS_HDMI_OUTPUT
int b_simpledecoder_hdmi_edid_config(b_simpledecoder_config *config, const NEXUS_HdmiOutputStatus *pStatus, const b_simpledecoder_resources *resources);
#endif

#ifdef __cplusplus
}
#endif
#endif
