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
 * $brcm_Workfile: nexus_simple_audio_decoder.c $
 * $brcm_Revision: 33 $
 * $brcm_Date: 9/17/12 3:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/simple_decoder/src/nexus_simple_audio_decoder.c $
 * 
 * 33   9/17/12 3:05p erickson
 * SW7435-344: remove manual NEXUS_OBJECT_REGISTER/UNREGISTER
 * 
 * 32   9/13/12 2:14p erickson
 * SW7420-1135: fix SetSettings
 * 
 * 31   8/9/12 5:27p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 30   7/23/12 11:35a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 29   6/27/12 3:10p jgarrett
 * SW7231-872: Merge to main branch
 * 
 * SW7231-872/1   6/25/12 4:26p marcusk
 * SW7231-872: Default behavior is to passthrough AC3 and DTS streams to
 *  SPDIF output.
 * 
 * 28   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 27   6/11/12 10:22a erickson
 * SW7425-2981: not all NEXUS_AudioDecoderConnectorType values are
 *  supported on every platform
 * 
 * 26   6/6/12 11:41a erickson
 * SW7425-2981: rework simple audio decoder input[] configuration
 * 
 * 25   5/2/12 5:10p erickson
 * SW7425-2981: fix secondary decoder compressed passthrough
 * 
 * 24   4/24/12 5:02p erickson
 * SW7420-1148: check return code of nexus_register_xxx calls
 * 
 * 23   3/13/12 4:50p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 22   1/17/12 12:49p vsilyaev
 * SW7231-602: Fixed typo
 * 
 * 21   1/12/12 1:01p vsilyaev
 * SW7425-2071:  Added extended functionality
 * 
 * 20   12/19/11 11:53a erickson
 * SW7231-562: fix default of
 *  NEXUS_SimpleAudioDecoderStartSettings.secondary.secondaryDecoder
 * 
 * 19   10/27/11 5:22p bandrews
 * SW7231-427: check for pid channel moved earlier in Start and added to
 *  Stop
 * 
 * 18   10/26/11 3:45p erickson
 * SW7420-1135: implement SetServerSettings that allows resources to by
 *  removed and restored while client has acquired decoder
 * 
 * 17   10/11/11 8:26a erickson
 * SW7420-1992: use regular-decoder start settings, client allocates stc
 *  channel, add disable mode enum
 * 
 * 16   10/5/11 1:51p erickson
 * SW7420-1148: add explicit register/unregister
 * 
 * 15   9/22/11 4:43p erickson
 * SW7420-1965: add simple decoder support to nexus playback module
 * 
 * 14   6/16/11 12:14p erickson
 * SW7425-458: add error message if tsm.audioOnly.pcr pid channel is
 *  missing
 * 
 * 13   6/2/11 5:33p erickson
 * SW7420-1889: rework SimpleAudioPlayback to have separate
 *  Acquire/Release functions
 * 
 * 12   2/28/11 1:51p erickson
 * SW7405-5105: allow playback to start before audio decode, but still use
 *  audio's STC
 *
 * 11   2/15/11 2:23p erickson
 * SW7420-1135: use nexus_audio_decoder_types.h
 *
 * 10   2/9/11 11:45a erickson
 * SW7405-5105: refactor SimpleAudioDecoder to support external, static
 *  configuration of mixed PCM. this allows decode and playback to start
 *  independently.
 *
 * 9   1/27/11 3:59p erickson
 * SW7420-1135: fix support for no HDMI and no SPDIF
 *
 * 8   1/26/11 3:16p erickson
 * SW7420-1135: add HDMI output support
 *
 * 7   1/24/11 3:58p erickson
 * SW7405-5105: add simple audio playback
 *
 * 6   11/17/10 3:48p erickson
 * SW7420-1135: fix possible uninitialized access
 *
 * 5   11/1/10 2:29p erickson
 * SW7420-1135: add TSM support
 *
 * 4   10/29/10 4:33p erickson
 * SW7420-1135: update
 *
 * 3   10/22/10 11:17a erickson
 * SW7420-1135: remove BERR_TRACE
 *
 * 2   10/20/10 4:05p erickson
 * SW7420-1135: initial audio impl
 *
 * 1   10/13/10 10:32a erickson
 * SW7420-1135: add
 *
 **************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#include "nexus_simple_decoder_module.h"
#include "nexus_simple_audio_priv.h"
#include "nexus_audio.h"
#include "nexus_audio_decoder.h"
#include "nexus_audio_decoder_trick.h"
#include "nexus_audio_output.h"
#include "nexus_audio_input.h"
#include "nexus_class_verification.h"
#include "nexus_client_resources.h"

BDBG_MODULE(nexus_simple_audio_decoder);

static BLST_S_HEAD(NEXUS_SimpleAudioDecoder_P_List, NEXUS_SimpleAudioDecoder) g_audioDecoders;

static NEXUS_Error NEXUS_SimpleAudioDecoder_P_AddOutputs( NEXUS_SimpleAudioDecoderHandle handle, NEXUS_AudioCodec primaryCodec, NEXUS_AudioCodec secondaryCodec);
static void NEXUS_SimpleAudioDecoder_P_RemoveOutputs( NEXUS_SimpleAudioDecoderHandle handle );

/**
server functions
**/

void NEXUS_SimpleAudioDecoder_GetDefaultServerSettings( NEXUS_SimpleAudioDecoderServerSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->enabled = true;
}

NEXUS_SimpleAudioDecoderHandle NEXUS_SimpleAudioDecoder_Create( unsigned index, const NEXUS_SimpleAudioDecoderServerSettings *pSettings )
{
    NEXUS_SimpleAudioDecoderHandle handle;
    NEXUS_Error rc;

    /* verify API macros aren't below nexus_platform_features.h */
#if NEXUS_NUM_SPDIF_OUTPUTS
    BDBG_CASSERT(NEXUS_MAX_SIMPLE_DECODER_SPDIF_OUTPUTS >= NEXUS_NUM_SPDIF_OUTPUTS);
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    BDBG_CASSERT(NEXUS_MAX_SIMPLE_DECODER_HDMI_OUTPUTS >= NEXUS_NUM_HDMI_OUTPUTS);
#endif
    
    /* find dup */
    for (handle=BLST_S_FIRST(&g_audioDecoders); handle; handle=BLST_S_NEXT(handle, link)) {
        if (handle->index == index) {
            BERR_TRACE(NEXUS_INVALID_PARAMETER);
            return NULL;
        }
    }

    handle = BKNI_Malloc(sizeof(*handle));
    if (!handle) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_SimpleAudioDecoder, handle);
    handle->index = index;
    handle->decoderSettingsSet = false;
    handle->defaultSettingsAcquired = false;
    BLST_S_INSERT_HEAD(&g_audioDecoders, handle, link);
    /* now a valid object */

    rc = NEXUS_SimpleAudioDecoder_SetServerSettings(handle, pSettings);
    if (rc) { rc = BERR_TRACE(rc); goto error; }
    
    return handle;
    
error:
    NEXUS_SimpleAudioDecoder_Destroy(handle);
    return NULL;
}

static void NEXUS_SimpleAudioDecoder_P_Release( NEXUS_SimpleAudioDecoderHandle handle )
{
    NEXUS_OBJECT_ASSERT(NEXUS_SimpleAudioDecoder, handle);
    if (handle->acquired) {
        NEXUS_SimpleAudioDecoder_Release(handle);
    }
    return;
}


static void NEXUS_SimpleAudioDecoder_P_Finalizer( NEXUS_SimpleAudioDecoderHandle handle )
{
    NEXUS_OBJECT_ASSERT(NEXUS_SimpleAudioDecoder, handle);

    BLST_S_REMOVE(&g_audioDecoders, handle, NEXUS_SimpleAudioDecoder, link);
    NEXUS_OBJECT_DESTROY(NEXUS_SimpleAudioDecoder, handle);
    BKNI_Free(handle);
    return;
}

NEXUS_OBJECT_CLASS_MAKE_WITH_RELEASE(NEXUS_SimpleAudioDecoder, NEXUS_SimpleAudioDecoder_Destroy);

void NEXUS_SimpleAudioDecoder_GetServerSettings( NEXUS_SimpleAudioDecoderHandle handle, NEXUS_SimpleAudioDecoderServerSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);
    *pSettings = handle->serverSettings;
}

static NEXUS_Error NEXUS_SimpleAudioDecoder_P_ApplyCodecSettings(NEXUS_AudioDecoderHandle decoder, const struct NEXUS_SimpleAudioDecoder_P_CodecSettings *codecSettings)
{
    NEXUS_Error rc;
    unsigned i;

    for(i=0;i<NEXUS_AudioCodec_eMax;i++) {
        if(codecSettings->codecSettings[i].codec!=NEXUS_AudioCodec_eUnknown) {
            rc = NEXUS_AudioDecoder_SetCodecSettings(decoder, &codecSettings->codecSettings[i]);
            if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}
        }
    }
    return NEXUS_SUCCESS;
}

static void NEXUS_SimpleAudioDecoder_P_RestoreCodecSettings(NEXUS_AudioDecoderHandle decoder, const struct NEXUS_SimpleAudioDecoder_P_CodecSettings *codecSettings, const struct NEXUS_SimpleAudioDecoder_P_CodecSettings *defaultCodecSettings)
{
    NEXUS_Error rc;
    unsigned i;

    for(i=0;i<NEXUS_AudioCodec_eMax;i++) {
        if(codecSettings->codecSettings[i].codec!=NEXUS_AudioCodec_eUnknown) {
            rc = NEXUS_AudioDecoder_SetCodecSettings(decoder, &defaultCodecSettings->codecSettings[i]);
            if(rc!=NEXUS_SUCCESS) {BERR_TRACE(rc);}
        }
    }
    return;
}

static void NEXUS_SimpleAudioDecoder_P_RestorePrimaryDecoder(NEXUS_SimpleAudioDecoderHandle handle)
{
    NEXUS_Error rc;

    if(handle->serverSettings.primary) {
        NEXUS_SimpleAudioDecoder_P_RestoreCodecSettings(handle->serverSettings.primary, &handle->codecSettings.primary, &handle->defaultSettings.codecSettings.primary);
        if(handle->decoderSettingsSet && handle->defaultSettingsAcquired) { /* restore back default configuration */
            rc = NEXUS_AudioDecoder_SetSettings(handle->serverSettings.primary, &handle->defaultSettings.decoderSettings.primary);
            if (rc!=NEXUS_SUCCESS) {rc = BERR_TRACE(rc); }
        }
    }
    return;
}

static void NEXUS_SimpleAudioDecoder_P_RestoreSecondaryDecoder(NEXUS_SimpleAudioDecoderHandle handle)
{
    NEXUS_Error rc;

    if(handle->serverSettings.secondary) {
        NEXUS_SimpleAudioDecoder_P_RestoreCodecSettings(handle->serverSettings.secondary, &handle->codecSettings.secondary, &handle->defaultSettings.codecSettings.secondary);
        if(handle->decoderSettingsSet && handle->defaultSettingsAcquired) { /* restore back default configuration */
            rc = NEXUS_AudioDecoder_SetSettings(handle->serverSettings.secondary, &handle->defaultSettings.decoderSettings.secondary);
            if (rc!=NEXUS_SUCCESS) {rc = BERR_TRACE(rc); }
        }
    }
    return;
}

NEXUS_Error NEXUS_SimpleAudioDecoder_SetServerSettings( NEXUS_SimpleAudioDecoderHandle handle, const NEXUS_SimpleAudioDecoderServerSettings *pSettings )
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);

    if (handle->primaryStarted || handle->secondaryStarted) {
        bool clientStarted = handle->clientStarted;
        NEXUS_SimpleAudioDecoder_Stop(handle);
        handle->clientStarted = clientStarted; /* restore client's desired state */
    }

    if(!handle->defaultSettingsAcquired) {
        if (pSettings->primary) {
            unsigned i;
            handle->defaultSettingsAcquired = true;
            NEXUS_AudioDecoder_GetSettings(pSettings->primary, &handle->defaultSettings.decoderSettings.primary);
            if(pSettings->secondary) {
                NEXUS_AudioDecoder_GetSettings(pSettings->secondary, &handle->defaultSettings.decoderSettings.secondary);
            } else {
                handle->defaultSettings.decoderSettings.secondary = handle->defaultSettings.decoderSettings.primary;
            }
            for(i=0;i<NEXUS_AudioCodec_eMax;i++) {
                NEXUS_AudioDecoder_GetCodecSettings(pSettings->primary, i, &handle->defaultSettings.codecSettings.primary.codecSettings[i]);
                if(pSettings->secondary) {
                    NEXUS_AudioDecoder_GetCodecSettings(pSettings->secondary, i, &handle->defaultSettings.codecSettings.secondary.codecSettings[i]);
                } else {
                    handle->defaultSettings.codecSettings.secondary.codecSettings[i] = handle->defaultSettings.codecSettings.primary.codecSettings[i];
                }
            }
        }
    }
    if(!pSettings->enabled || pSettings->secondary==NULL || pSettings->primary==NULL ) {
        if(!pSettings->enabled || pSettings->primary==NULL) {
            NEXUS_SimpleAudioDecoder_P_RestorePrimaryDecoder(handle);
        }
        if(!pSettings->enabled || pSettings->secondary==NULL) {
            NEXUS_SimpleAudioDecoder_P_RestoreSecondaryDecoder(handle);
        }
    }

    handle->serverSettings = *pSettings;
    handle->currentSpdifInput = handle->currentHdmiInput = NULL; /* force reconfig */
    

    if (pSettings->enabled) {
        if (handle->clientStarted) {
            handle->clientStarted = false; /* Start will set true */
            rc = NEXUS_SimpleAudioDecoder_Start(handle, &handle->startSettings);
            if (rc) return BERR_TRACE(rc);
        }
        else {
            /* set up default outputs for playback-only */
            rc = NEXUS_SimpleAudioDecoder_P_AddOutputs(handle, NEXUS_AudioCodec_eUnknown, NEXUS_AudioCodec_eUnknown);
            if (rc) return BERR_TRACE(rc);
        }
    } 

    return 0;
}

/**
client functions
**/

NEXUS_SimpleAudioDecoderHandle NEXUS_SimpleAudioDecoder_Acquire( unsigned index )
{
    NEXUS_SimpleAudioDecoderHandle handle;
    NEXUS_Error rc;

    for (handle=BLST_S_FIRST(&g_audioDecoders); handle; handle = BLST_S_NEXT(handle, link)) {
        BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);
        if ((index == NEXUS_ANY_ID && !handle->acquired) || (handle->index == index)) {
            unsigned i;
            if (handle->acquired) {
                BERR_TRACE(NEXUS_NOT_AVAILABLE);
                return NULL;
            }
            
            rc = NEXUS_CLIENT_RESOURCES_ACQUIRE(simpleAudioDecoder,IdList,index);
            if (rc) { rc = BERR_TRACE(rc); return NULL; }
            
            handle->acquired = true;
            handle->decoderSettingsSet = false;
            for(i=0;i<NEXUS_AudioCodec_eMax;i++) {
                handle->codecSettings.primary.codecSettings[i].codec = NEXUS_AudioCodec_eUnknown;
                handle->codecSettings.secondary.codecSettings[i].codec = NEXUS_AudioCodec_eUnknown;
            }
            return handle;
        }
    }
    return NULL;
}

void NEXUS_SimpleAudioDecoder_Release( NEXUS_SimpleAudioDecoderHandle handle )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);
    /* IPC handle validation will only allow this call if handle is owned by client.
    For non-IPC used, acquiring is not required, so acquired boolean is not checked in any other API. */
    if (!handle->acquired) {
        BERR_TRACE(NEXUS_NOT_AVAILABLE);
        return;
    }
    if (handle->primaryStarted || handle->secondaryStarted) {
        NEXUS_SimpleAudioDecoder_Stop(handle);
    }
    NEXUS_SimpleAudioDecoder_P_RestorePrimaryDecoder(handle);
    NEXUS_SimpleAudioDecoder_P_RestoreSecondaryDecoder(handle);

    NEXUS_CLIENT_RESOURCES_RELEASE(simpleAudioDecoder,IdList,handle->index);
    handle->acquired = false;
    return;
}

void NEXUS_SimpleAudioDecoder_GetDefaultStartSettings( NEXUS_SimpleAudioDecoderStartSettings *pSettings )
{
    NEXUS_AudioDecoder_GetDefaultStartSettings(&pSettings->primary);
    NEXUS_AudioDecoder_GetDefaultStartSettings(&pSettings->secondary);
    pSettings->secondary.secondaryDecoder = true;
}

static NEXUS_Error NEXUS_SimpleAudioDecoder_P_AddOutputs( NEXUS_SimpleAudioDecoderHandle handle, NEXUS_AudioCodec primaryCodec, NEXUS_AudioCodec secondaryCodec)
{
    NEXUS_AudioInput spdifInput, hdmiInput;
    unsigned i;
    NEXUS_Error rc;
    NEXUS_SimpleAudioPlaybackHandle audioPlayback;

    /* determine inputs */    
    spdifInput = NULL;
    if (secondaryCodec != NEXUS_AudioCodec_eUnknown) {
        spdifInput = handle->serverSettings.spdif.input[secondaryCodec];
    }
    if (!spdifInput) {
        spdifInput = handle->serverSettings.spdif.input[primaryCodec];
    }
    hdmiInput = NULL;
#if NEXUS_HAS_HDMI_OUTPUT
    if (secondaryCodec != NEXUS_AudioCodec_eUnknown) {
        hdmiInput = handle->serverSettings.hdmi.input[secondaryCodec];
    }
    if (!hdmiInput) {
        hdmiInput = handle->serverSettings.hdmi.input[primaryCodec];
    }
#endif

    /* if outputs are the same, don't change. this avoids needless glitches when starting/stopping decode */
    if (handle->currentSpdifInput == spdifInput && handle->currentHdmiInput == hdmiInput) {
        return 0;
    }
    
    /* playbacks must be stopped and restarted when reconfiguring outputs. TODO: audio module could be extended to avoid the flush of pcm data on this transition. */
    for (audioPlayback = BLST_S_FIRST(&handle->audioPlaybacks); audioPlayback; audioPlayback = BLST_S_NEXT(audioPlayback, decoderLink)) {
        nexus_simpleaudioplayback_p_internal_stop(audioPlayback);
    }
    
    /* always remove because of default configuration */
    NEXUS_SimpleAudioDecoder_P_RemoveOutputs(handle);
    
    if (spdifInput) {
        for (i=0;i<NEXUS_MAX_SIMPLE_DECODER_SPDIF_OUTPUTS;i++) {
            if (handle->serverSettings.spdif.outputs[i]) {
                rc = NEXUS_AudioOutput_AddInput(NEXUS_SpdifOutput_GetConnector(handle->serverSettings.spdif.outputs[i]), spdifInput);
                if (rc) { rc = BERR_TRACE(rc); goto error_start;}
                BDBG_MSG(("connect input %p -> spdif%d", spdifInput, i));
            }
        }
    }
#if NEXUS_HAS_HDMI_OUTPUT
    if (hdmiInput) {
        for (i=0;i<NEXUS_MAX_SIMPLE_DECODER_HDMI_OUTPUTS;i++) {
            if (handle->serverSettings.hdmi.outputs[i]) {
                rc = NEXUS_AudioOutput_AddInput(NEXUS_HdmiOutput_GetAudioConnector(handle->serverSettings.hdmi.outputs[i]), hdmiInput);
                if (rc) { rc = BERR_TRACE(rc); goto error_start;}
                BDBG_MSG(("connect input %p -> hdmi%d", hdmiInput, i));
            }
        }
    }
#endif

    for (audioPlayback = BLST_S_FIRST(&handle->audioPlaybacks); audioPlayback; audioPlayback = BLST_S_NEXT(audioPlayback, decoderLink)) {
        nexus_simpleaudioplayback_p_internal_start(audioPlayback);
    }

    handle->currentSpdifInput = spdifInput;
    handle->currentHdmiInput = hdmiInput;
    return 0;
    
error_start:
    /* don't remove outputs here */
    return rc;
}

static void NEXUS_SimpleAudioDecoder_P_RemoveOutputs( NEXUS_SimpleAudioDecoderHandle handle )
{
    unsigned i;
    for (i=0;i<NEXUS_MAX_SIMPLE_DECODER_SPDIF_OUTPUTS;i++) {
        if (handle->serverSettings.spdif.outputs[i]) {
            NEXUS_AudioOutput_RemoveAllInputs(NEXUS_SpdifOutput_GetConnector(handle->serverSettings.spdif.outputs[i]));
        }
    }
#if NEXUS_HAS_HDMI_OUTPUT
    for (i=0;i<NEXUS_MAX_SIMPLE_DECODER_HDMI_OUTPUTS;i++) {
        if (handle->serverSettings.hdmi.outputs[i]) {
            NEXUS_AudioOutput_RemoveAllInputs(NEXUS_HdmiOutput_GetAudioConnector(handle->serverSettings.hdmi.outputs[i]));
        }
    }
#endif

    /* can't call NEXUS_AudioInput_Shutdown on handle->serverSettings.spdif/hdmi.input[] because it could be stereo connector */
}

static bool nexus_p_is_decoder_connected(NEXUS_AudioDecoderHandle audioDecoder)
{
    NEXUS_AudioDecoderConnectorType i;
    for (i=0;i<NEXUS_AudioDecoderConnectorType_eMax;i++) {
        bool connected;
        NEXUS_AudioInput audioInput = NEXUS_AudioDecoder_GetConnector(audioDecoder, i);
        if (audioInput) {
            NEXUS_AudioInput_HasConnectedOutputs(audioInput, &connected);
            if (connected) {
                return true;
            }
        }
    }
    return false;
}

NEXUS_Error NEXUS_SimpleAudioDecoder_Start( NEXUS_SimpleAudioDecoderHandle handle, const NEXUS_SimpleAudioDecoderStartSettings *pSettings )
{
    NEXUS_Error rc;
    NEXUS_AudioCapabilities caps;

    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);
    
    if (!handle->serverSettings.enabled) {
        if (handle->serverSettings.disableMode == NEXUS_SimpleDecoderDisableMode_eSuccess) {
            goto done;
        }
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }

    BDBG_MSG(("start settings primary[pidch %p, codec %d, decoder %p] secondary[pidch %p, codec %d, decoder %p]", 
        pSettings->primary.pidChannel, pSettings->primary.codec, handle->serverSettings.primary, 
        pSettings->secondary.pidChannel, pSettings->secondary.codec, handle->serverSettings.secondary));
        
    rc = NEXUS_SimpleAudioDecoder_P_AddOutputs( handle, 
        pSettings->primary.codec, 
        handle->serverSettings.secondary && pSettings->secondary.pidChannel ? pSettings->secondary.codec : NEXUS_AudioCodec_eUnknown);
    if (rc) return BERR_TRACE(rc);

    if (handle->serverSettings.primary && nexus_p_is_decoder_connected(handle->serverSettings.primary)) {
        if(handle->decoderSettingsSet) {
            rc = NEXUS_AudioDecoder_SetSettings(handle->serverSettings.primary, &handle->settings.primary);
            if (rc) { rc = BERR_TRACE(rc); goto error_start;}
        }
        rc = NEXUS_SimpleAudioDecoder_P_ApplyCodecSettings(handle->serverSettings.primary, &handle->codecSettings.primary);
        if (rc) { rc = BERR_TRACE(rc); goto error_start;}

        NEXUS_GetAudioCapabilities(&caps);
        if (caps.dsp.codecs[pSettings->primary.codec].decode) {
            rc = NEXUS_AudioDecoder_Start(handle->serverSettings.primary, &pSettings->primary);
            if (rc) { rc = BERR_TRACE(rc); goto error_start;}

            handle->primaryStarted = true;
            
            NEXUS_AudioDecoder_GetTrickState(handle->serverSettings.primary, &handle->trickState);
        }
    }
    
    if (handle->serverSettings.secondary && nexus_p_is_decoder_connected(handle->serverSettings.secondary)) {
        if(handle->decoderSettingsSet) {
            rc = NEXUS_AudioDecoder_SetSettings(handle->serverSettings.secondary, &handle->settings.secondary);
            if (rc) { rc = BERR_TRACE(rc); goto error_start;}
        }
        rc = NEXUS_SimpleAudioDecoder_P_ApplyCodecSettings(handle->serverSettings.secondary, &handle->codecSettings.secondary);
        if (rc) { rc = BERR_TRACE(rc); goto error_start;}

        BDBG_WRN(("start secondary decoder"));
        rc = NEXUS_AudioDecoder_Start(handle->serverSettings.secondary, pSettings->secondary.pidChannel?&pSettings->secondary:&pSettings->primary);
        if (rc) { rc = BERR_TRACE(rc); goto error_start;}

        handle->secondaryStarted = true;
        
        if (!handle->primaryStarted) {
            NEXUS_AudioDecoder_GetTrickState(handle->serverSettings.secondary, &handle->trickState);
        }
    }
    
done:
    handle->startSettings = *pSettings;
    handle->clientStarted = true;
    return 0;
    
error_start:
    NEXUS_SimpleAudioDecoder_P_RemoveOutputs(handle);
    return rc;
}

void NEXUS_SimpleAudioDecoder_Stop( NEXUS_SimpleAudioDecoderHandle handle )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);

    handle->clientStarted = false;
    
    /* first, stop decode */
    if (handle->primaryStarted) {
        NEXUS_AudioDecoder_Stop(handle->serverSettings.primary);
        handle->primaryStarted = false;
    }
    if (handle->secondaryStarted) {
        NEXUS_AudioDecoder_Stop(handle->serverSettings.secondary);
        handle->secondaryStarted = false;
    }

    /* revert to default config. if decoder output was compressed, this allows pcm playback to be restored. */
    (void)NEXUS_SimpleAudioDecoder_P_AddOutputs(handle, NEXUS_AudioCodec_eUnknown, NEXUS_AudioCodec_eUnknown);
}

void NEXUS_SimpleAudioDecoder_GetSettings( NEXUS_SimpleAudioDecoderHandle handle, NEXUS_SimpleAudioDecoderSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);
    if(handle->decoderSettingsSet)  {
        *pSettings = handle->settings;
    } else {
        *pSettings = handle->defaultSettings.decoderSettings;
    } 
    return;
}

NEXUS_Error NEXUS_SimpleAudioDecoder_SetSettings( NEXUS_SimpleAudioDecoderHandle handle, const NEXUS_SimpleAudioDecoderSettings *pSettings )
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);
    if (!handle->serverSettings.enabled && handle->serverSettings.disableMode == NEXUS_SimpleDecoderDisableMode_eFail) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    if (handle->primaryStarted) {
        rc = NEXUS_AudioDecoder_SetSettings(handle->serverSettings.primary, &pSettings->primary);
        if (rc) return BERR_TRACE(rc);
    }
    if (handle->secondaryStarted) {
        rc = NEXUS_AudioDecoder_SetSettings(handle->serverSettings.secondary, &pSettings->secondary);
        if (rc) return BERR_TRACE(rc);
    }

    handle->settings = *pSettings;
    handle->decoderSettingsSet = true;
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_SimpleAudioDecoder_GetStatus( NEXUS_SimpleAudioDecoderHandle handle, NEXUS_AudioDecoderStatus *pStatus )
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);
    if (!handle->serverSettings.enabled && handle->serverSettings.disableMode == NEXUS_SimpleDecoderDisableMode_eFail) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    if (handle->primaryStarted) {
        rc = NEXUS_AudioDecoder_GetStatus(handle->serverSettings.primary, pStatus);
        if (rc) return BERR_TRACE(rc);
    }
    else if (handle->secondaryStarted) {
        rc = NEXUS_AudioDecoder_GetStatus(handle->serverSettings.secondary, pStatus);
        if (rc) return BERR_TRACE(rc);
    }
    else {
        BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    }
    return 0;
}

void NEXUS_SimpleAudioDecoder_Flush( NEXUS_SimpleAudioDecoderHandle handle )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);
    if (handle->primaryStarted) {
        NEXUS_AudioDecoder_Flush(handle->serverSettings.primary);
    }
    if (handle->secondaryStarted) {
        NEXUS_AudioDecoder_Flush(handle->serverSettings.secondary);
    }
}

void NEXUS_SimpleAudioDecoder_GetTrickState( NEXUS_SimpleAudioDecoderHandle handle, NEXUS_AudioDecoderTrickState *pSettings )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);
    *pSettings = handle->trickState;
}

NEXUS_Error NEXUS_SimpleAudioDecoder_SetTrickState( NEXUS_SimpleAudioDecoderHandle handle, const NEXUS_AudioDecoderTrickState *pSettings )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);
    if (!handle->serverSettings.enabled && handle->serverSettings.disableMode == NEXUS_SimpleDecoderDisableMode_eFail) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    if (handle->primaryStarted) {
        rc = NEXUS_AudioDecoder_SetTrickState(handle->serverSettings.primary, pSettings);
        if (rc) return BERR_TRACE(rc);
    }
    if (handle->secondaryStarted) {
        rc = NEXUS_AudioDecoder_SetTrickState(handle->serverSettings.secondary, pSettings);
        if (rc) return BERR_TRACE(rc);
    }
    handle->trickState = *pSettings;
    return 0;
}

NEXUS_Error NEXUS_SimpleAudioDecoder_Advance( NEXUS_SimpleAudioDecoderHandle handle, uint32_t pts )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);
    if (!handle->serverSettings.enabled && handle->serverSettings.disableMode == NEXUS_SimpleDecoderDisableMode_eFail) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    if (handle->primaryStarted) {
        rc = NEXUS_AudioDecoder_Advance(handle->serverSettings.primary, pts);
        if (rc) return BERR_TRACE(rc);
    }
    if (handle->secondaryStarted) {
        rc = NEXUS_AudioDecoder_Advance(handle->serverSettings.secondary, pts);
        if (rc) return BERR_TRACE(rc);
    }
    return 0;
}

NEXUS_Error NEXUS_SimpleAudioDecoder_SetCodecSettings( NEXUS_SimpleAudioDecoderHandle handle, NEXUS_SimpleAudioDecoderSelector selector, const NEXUS_AudioDecoderCodecSettings *pSettings)
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);
    BDBG_ASSERT(pSettings);
    if (!handle->serverSettings.enabled && handle->serverSettings.disableMode == NEXUS_SimpleDecoderDisableMode_eFail) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    if(pSettings->codec >= NEXUS_AudioCodec_eMax) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if(selector >= NEXUS_SimpleAudioDecoderSelector_eMax) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if(selector == NEXUS_SimpleAudioDecoderSelector_ePrimary) {
        if (handle->primaryStarted) {
            rc = NEXUS_AudioDecoder_SetCodecSettings(handle->serverSettings.primary, pSettings);
            if (rc) return BERR_TRACE(rc);
        }
        handle->codecSettings.primary.codecSettings[pSettings->codec] = *pSettings;
    } else if (selector == NEXUS_SimpleAudioDecoderSelector_eSecondary) {
        if (handle->secondaryStarted) {
            rc = NEXUS_AudioDecoder_SetCodecSettings(handle->serverSettings.secondary, pSettings);
            if (rc) return BERR_TRACE(rc);
        }
        handle->codecSettings.primary.codecSettings[pSettings->codec] = *pSettings;
    }

    return NEXUS_SUCCESS;
}

void NEXUS_SimpleAudioDecoder_GetCodecSettings( NEXUS_SimpleAudioDecoderHandle handle, NEXUS_SimpleAudioDecoderSelector selector, NEXUS_AudioCodec codec, NEXUS_AudioDecoderCodecSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioDecoder);
    BDBG_ASSERT(pSettings);

    if(codec >= NEXUS_AudioCodec_eMax) {
        (void)BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }
    if(selector >= NEXUS_SimpleAudioDecoderSelector_eMax) {
        (void)BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return;
    }

    if(selector == NEXUS_SimpleAudioDecoderSelector_ePrimary) {
        if(handle->codecSettings.primary.codecSettings[codec].codec != NEXUS_AudioCodec_eUnknown) {
            *pSettings = handle->codecSettings.primary.codecSettings[codec];
        } else {
            *pSettings = handle->defaultSettings.codecSettings.primary.codecSettings[codec];
        }
    } else if(selector == NEXUS_SimpleAudioDecoderSelector_eSecondary) {
        if(handle->codecSettings.secondary.codecSettings[codec].codec != NEXUS_AudioCodec_eUnknown) {
            *pSettings = handle->codecSettings.secondary.codecSettings[codec];
        } else {
            *pSettings = handle->defaultSettings.codecSettings.secondary.codecSettings[codec];
        }
    }
    return;
}

void nexus_simpleaudiodecoder_p_add_playback(NEXUS_SimpleAudioDecoderHandle handle, NEXUS_SimpleAudioPlaybackHandle audioPlayback)
{
    BLST_S_INSERT_HEAD(&handle->audioPlaybacks, audioPlayback, decoderLink);
}

void nexus_simpleaudiodecoder_p_remove_playback(NEXUS_SimpleAudioDecoderHandle handle, NEXUS_SimpleAudioPlaybackHandle audioPlayback)
{
    BLST_S_REMOVE(&handle->audioPlaybacks, audioPlayback, NEXUS_SimpleAudioPlayback, decoderLink);
}
