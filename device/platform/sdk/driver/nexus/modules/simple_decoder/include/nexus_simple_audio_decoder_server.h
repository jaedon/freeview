/***************************************************************************
 *     (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_simple_audio_decoder_server.h $
 * $brcm_Revision: 14 $
 * $brcm_Date: 6/6/12 11:41a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/simple_decoder/include/nexus_simple_audio_decoder_server.h $
 * 
 * 14   6/6/12 11:41a erickson
 * SW7425-2981: rework simple audio decoder input[] configuration
 * 
 * 13   4/30/12 2:20p erickson
 * SW7231-786: clarify mixed/unmixed PCM configuration
 * 
 * 12   3/13/12 4:50p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 11   10/26/11 3:45p erickson
 * SW7420-1135: implement SetServerSettings that allows resources to by
 *  removed and restored while client has acquired decoder
 * 
 * 10   10/11/11 8:26a erickson
 * SW7420-1992: use regular-decoder start settings, client allocates stc
 *  channel, add disable mode enum
 * 
 * 9   6/2/11 5:33p erickson
 * SW7420-1889: rework SimpleAudioPlayback to have separate
 *  Acquire/Release functions
 * 
 * 8   2/15/11 2:23p erickson
 * SW7420-1135: use nexus_audio_decoder_types.h
 *
 * 7   2/9/11 11:45a erickson
 * SW7405-5105: refactor SimpleAudioDecoder to support external, static
 *  configuration of mixed PCM. this allows decode and playback to start
 *  independently.
 *
 * 6   1/27/11 3:59p erickson
 * SW7420-1135: fix support for no HDMI and no SPDIF
 *
 * 5   1/26/11 3:16p erickson
 * SW7420-1135: add HDMI output support
 *
 * 4   1/24/11 3:58p erickson
 * SW7405-5105: add simple audio playback
 *
 * 3   12/20/10 10:34a erickson
 * SW7420-1148: add #if NEXUS_NUM_HDMI_OUTPUTS
 *
 * 2   10/20/10 4:05p erickson
 * SW7420-1135: initial audio impl
 *
 * 1   10/13/10 10:32a erickson
 * SW7420-1135: add
 *
 **************************************************************************/
#ifndef NEXUS_SIMPLE_AUDIO_DECODER_SERVER_H__
#define NEXUS_SIMPLE_AUDIO_DECODER_SERVER_H__

#include "nexus_types.h"
#include "nexus_simple_audio_decoder.h"
#include "nexus_simple_audio_playback.h"
#include "nexus_stc_channel.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_playback.h"
#include "nexus_spdif_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define NEXUS_MAX_SIMPLE_DECODER_SPDIF_OUTPUTS 2
#define NEXUS_MAX_SIMPLE_DECODER_HDMI_OUTPUTS 1

/**
Summary:
Settings for Create and SetServerSettings. They can be modified before or after the client starts decode.
For many changes, restarting audio decode will be required. But the SimpleAudioDecoder is allowed to do this transparently.
**/
typedef struct NEXUS_SimpleAudioDecoderServerSettings
{
    bool enabled;
    NEXUS_SimpleDecoderDisableMode disableMode;
    NEXUS_AudioDecoderHandle primary;   /* for decode and simul */
    NEXUS_AudioDecoderHandle secondary; /* for compressed passthrough */

    struct {
        NEXUS_SpdifOutputHandle outputs[NEXUS_MAX_SIMPLE_DECODER_SPDIF_OUTPUTS];
        NEXUS_AudioInput input[NEXUS_AudioCodec_eMax]; /* per codec, specify the final stage to be connected to spdif. 
            use NEXUS_AudioCodec_eUnknown to specify default configuration for playback-only. */
    } spdif;
#if NEXUS_HAS_HDMI_OUTPUT
    struct {
        NEXUS_HdmiOutputHandle outputs[NEXUS_MAX_SIMPLE_DECODER_HDMI_OUTPUTS];
        NEXUS_AudioInput input[NEXUS_AudioCodec_eMax]; /* per codec, specify the final stage to be connected to hdmi. 
            use NEXUS_AudioCodec_eUnknown to specify default configuration for playback-only. */
    } hdmi;
#endif
} NEXUS_SimpleAudioDecoderServerSettings;

/**
Summary:
**/
void NEXUS_SimpleAudioDecoder_GetDefaultServerSettings(
    NEXUS_SimpleAudioDecoderServerSettings *pSettings /* [out] */
    );

/**
Summary:
**/
NEXUS_SimpleAudioDecoderHandle NEXUS_SimpleAudioDecoder_Create( /* attr{destructor=NEXUS_SimpleAudioDecoder_Destroy}  */
    unsigned index,
    const NEXUS_SimpleAudioDecoderServerSettings *pSettings
    );

/**
Summary:
Server is responsible for ensuring that all clients have called NEXUS_SimpleAudioDecoder_Release.
If any handle is not released beforehand, there will likely be system failure.
If a client will not release its handle, the app should forcibly terminate the client process before calling Destroy.
**/
void NEXUS_SimpleAudioDecoder_Destroy(
    NEXUS_SimpleAudioDecoderHandle handle
    );

/**
Summary:
**/
void NEXUS_SimpleAudioDecoder_GetServerSettings(
    NEXUS_SimpleAudioDecoderHandle handle,
    NEXUS_SimpleAudioDecoderServerSettings *pSettings /* [out] */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleAudioDecoder_SetServerSettings(
    NEXUS_SimpleAudioDecoderHandle handle,
    const NEXUS_SimpleAudioDecoderServerSettings *pSettings
    );

/**
Summary:
**/
typedef struct NEXUS_SimpleAudioPlaybackServerSettings
{
    NEXUS_SimpleAudioDecoderHandle decoder;
    NEXUS_AudioPlaybackHandle playback;
} NEXUS_SimpleAudioPlaybackServerSettings;

/**
Summary:
**/
void NEXUS_SimpleAudioPlayback_GetDefaultServerSettings(
    NEXUS_SimpleAudioPlaybackServerSettings *pSettings /* [out] */
    );
    
/**
Summary:
Audio playback
**/
NEXUS_SimpleAudioPlaybackHandle NEXUS_SimpleAudioPlayback_Create( /* attr{destructor=NEXUS_SimpleAudioPlayback_Destroy}  */
    unsigned index,
    const NEXUS_SimpleAudioPlaybackServerSettings *pSettings
    );

/**
Summary:
**/
void NEXUS_SimpleAudioPlayback_Destroy(
    NEXUS_SimpleAudioPlaybackHandle handle
    );
    
/**
Summary:
**/
void NEXUS_SimpleAudioPlayback_GetServerSettings(
    NEXUS_SimpleAudioPlaybackHandle handle,
    NEXUS_SimpleAudioPlaybackServerSettings *pSettings /* [out] */
    );

/**
Summary:
**/
NEXUS_Error NEXUS_SimpleAudioPlayback_SetServerSettings(
    NEXUS_SimpleAudioPlaybackHandle handle,
    const NEXUS_SimpleAudioPlaybackServerSettings *pSettings
    );    
    
#ifdef __cplusplus
}
#endif

#endif
