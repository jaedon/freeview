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
 * $brcm_Workfile: nexus_simple_audio_priv.h $
 * $brcm_Revision: 6 $
 * $brcm_Date: 6/19/12 12:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/simple_decoder/src/nexus_simple_audio_priv.h $
 * 
 * 6   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 5   6/6/12 11:41a erickson
 * SW7425-2981: rework simple audio decoder input[] configuration
 * 
 * 4   1/12/12 1:01p vsilyaev
 * SW7425-2071:  Added extended functionality
 * 
 * 3   10/26/11 3:45p erickson
 * SW7420-1135: implement SetServerSettings that allows resources to by
 *  removed and restored while client has acquired decoder
 * 
 * 2   6/2/11 5:33p erickson
 * SW7420-1889: rework SimpleAudioPlayback to have separate
 *  Acquire/Release functions
 * 
 * 1   1/24/11 3:58p erickson
 * SW7405-5105: add simple audio playback
 *
 **************************************************************************/
#ifndef NEXUS_SIMPLE_AUDIO_PRIV_H__
#define NEXUS_SIMPLE_AUDIO_PRIV_H__

#ifdef __cplusplus
#error
#endif

struct NEXUS_SimpleAudioDecoder_P_CodecSettings {
    NEXUS_AudioDecoderCodecSettings codecSettings[NEXUS_AudioCodec_eMax];
};

struct NEXUS_SimpleAudioDecoder_P_CodecSettingsDecoder {
    struct NEXUS_SimpleAudioDecoder_P_CodecSettings primary;
    struct NEXUS_SimpleAudioDecoder_P_CodecSettings secondary;
};

struct NEXUS_SimpleAudioDecoder
{
    NEXUS_OBJECT(NEXUS_SimpleAudioDecoder);
    BLST_S_ENTRY(NEXUS_SimpleAudioDecoder) link;
    unsigned index;
    bool acquired;
    bool primaryStarted;
    bool secondaryStarted;
    bool clientStarted;
    bool decoderSettingsSet;
    bool defaultSettingsAcquired;
    NEXUS_SimpleAudioDecoderServerSettings serverSettings;
    struct {
        NEXUS_SimpleAudioDecoderSettings  decoderSettings;
        struct NEXUS_SimpleAudioDecoder_P_CodecSettingsDecoder codecSettings;
    } defaultSettings;
    NEXUS_SimpleAudioDecoderSettings settings;
    struct NEXUS_SimpleAudioDecoder_P_CodecSettingsDecoder codecSettings;
    NEXUS_SimpleAudioDecoderStartSettings startSettings;
    NEXUS_AudioDecoderTrickState trickState;
    BLST_S_HEAD(playback_list, NEXUS_SimpleAudioPlayback) audioPlaybacks;
    NEXUS_AudioInput currentSpdifInput, currentHdmiInput;
};

BDBG_OBJECT_ID_DECLARE(NEXUS_SimpleAudioPlayback);
struct NEXUS_SimpleAudioPlayback
{
    NEXUS_OBJECT(NEXUS_SimpleAudioPlayback);
    BLST_S_ENTRY(NEXUS_SimpleAudioPlayback) link; /* global list */
    BLST_S_ENTRY(NEXUS_SimpleAudioPlayback) decoderLink; /* per decoder list */
    unsigned index;
    bool acquired;
    bool started;
    bool clientStarted;
    NEXUS_SimpleAudioPlaybackServerSettings serverSettings;
    NEXUS_SimpleAudioPlaybackSettings settings;
    NEXUS_SimpleAudioPlaybackStartSettings startSettings;
};

void nexus_simpleaudiodecoder_p_add_playback(NEXUS_SimpleAudioDecoderHandle audioDecoder, NEXUS_SimpleAudioPlaybackHandle audioPlayback);
void nexus_simpleaudiodecoder_p_remove_playback(NEXUS_SimpleAudioDecoderHandle audioDecoder, NEXUS_SimpleAudioPlaybackHandle audioPlayback);

NEXUS_Error nexus_simpleaudioplayback_p_internal_start(NEXUS_SimpleAudioPlaybackHandle handle);
void nexus_simpleaudioplayback_p_internal_stop(NEXUS_SimpleAudioPlaybackHandle handle);

#endif
