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
 * $brcm_Workfile: nexus_simple_audio_playback.c $
 * $brcm_Revision: 17 $
 * $brcm_Date: 9/17/12 3:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/simple_decoder/src/nexus_simple_audio_playback.c $
 * 
 * 17   9/17/12 3:05p erickson
 * SW7435-344: remove manual NEXUS_OBJECT_REGISTER/UNREGISTER
 * 
 * 16   8/10/12 10:22a erickson
 * SW7425-2734: simpleAudioPlayback uses Count, not IdList
 * 
 * 15   8/9/12 5:27p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 14   7/23/12 11:36a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 13   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 12   6/6/12 11:41a erickson
 * SW7425-2981: rework simple audio decoder input[] configuration
 * 
 * 11   4/24/12 5:02p erickson
 * SW7420-1148: check return code of nexus_register_xxx calls
 * 
 * 10   10/31/11 6:26p bandrews
 * SW7231-391: remove dependency on stc channel from decoder start
 *  settings by exposing timebase directly
 * 
 * SW7420-2078/2   10/28/11 5:49p bandrews
 * SW7231-391: expose timebase directly
 * 
 * SW7420-2078/1   10/28/11 4:51p bandrews
 * SW7231-391: get timebase from stc channel private interface
 * 
 * 9   10/26/11 3:45p erickson
 * SW7420-1135: implement SetServerSettings that allows resources to by
 *  removed and restored while client has acquired decoder
 *
 * 8   10/11/11 8:26a erickson
 * SW7420-1992: use regular-decoder start settings, client allocates stc
 *  channel, add disable mode enum
 *
 * 7   10/5/11 1:51p erickson
 * SW7420-1148: add explicit register/unregister
 *
 * 6   7/25/11 2:31p erickson
 * SWNOOS-476: Fixed compilation errors when using the SDE toolchain.
 *
 * 5   6/2/11 5:33p erickson
 * SW7420-1889: rework SimpleAudioPlayback to have separate
 *  Acquire/Release functions
 *
 * 4   2/28/11 3:04p erickson
 * SW7420-1123: forced cleanup
 *
 * 3   2/28/11 1:51p erickson
 * SW7405-5105: allow playback to start before audio decode, but still use
 *  audio's STC
 *
 * 2   1/24/11 4:03p erickson
 * SW7405-5105: fix callback
 *
 * 1   1/24/11 3:58p erickson
 * SW7405-5105: add simple audio playback
 *
 **************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "nexus_simple_decoder_module.h"
#include "nexus_simple_audio_playback.h"
#include "nexus_simple_audio_priv.h"
#include "nexus_audio_playback.h"
#include "nexus_audio_input.h"
#include "nexus_class_verification.h"
#include "nexus_client_resources.h"

BDBG_MODULE(nexus_simple_audio_playback);

static BLST_S_HEAD(NEXUS_SimpleAudioPlayback_P_List, NEXUS_SimpleAudioPlayback) g_audioPlaybacks;

void NEXUS_SimpleAudioPlayback_GetDefaultServerSettings( NEXUS_SimpleAudioPlaybackServerSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_SimpleAudioPlaybackHandle NEXUS_SimpleAudioPlayback_Create( unsigned index, const NEXUS_SimpleAudioPlaybackServerSettings *pSettings )
{
    NEXUS_SimpleAudioPlaybackHandle handle;
    NEXUS_Error rc;

    if (!pSettings->decoder) {
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }
    
    /* find dup */
    for (handle=BLST_S_FIRST(&g_audioPlaybacks); handle; handle=BLST_S_NEXT(handle, link)) {
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
    NEXUS_OBJECT_INIT(NEXUS_SimpleAudioPlayback, handle);
    handle->index = index;
    BLST_S_INSERT_HEAD(&g_audioPlaybacks, handle, link);
    nexus_simpleaudiodecoder_p_add_playback(pSettings->decoder, handle);
    /* now a valid object */

    rc = NEXUS_SimpleAudioPlayback_SetServerSettings(handle, pSettings);
    if (rc) {rc = BERR_TRACE(rc); goto error;}

    return handle;
    
error:
    NEXUS_SimpleAudioPlayback_Destroy(handle);
    return NULL;
}

static void NEXUS_SimpleAudioPlayback_P_Release( NEXUS_SimpleAudioPlaybackHandle handle )
{
    NEXUS_OBJECT_ASSERT(NEXUS_SimpleAudioPlayback, handle);

    nexus_simpleaudiodecoder_p_remove_playback(handle->serverSettings.decoder, handle);
    return;
}

static void NEXUS_SimpleAudioPlayback_P_Finalizer( NEXUS_SimpleAudioPlaybackHandle handle )
{
    NEXUS_OBJECT_ASSERT(NEXUS_SimpleAudioPlayback, handle);

    BLST_S_REMOVE(&g_audioPlaybacks, handle, NEXUS_SimpleAudioPlayback, link);
    NEXUS_OBJECT_DESTROY(NEXUS_SimpleAudioPlayback, handle);
    BKNI_Free(handle);
    return;
}

NEXUS_OBJECT_CLASS_MAKE_WITH_RELEASE(NEXUS_SimpleAudioPlayback, NEXUS_SimpleAudioPlayback_Destroy);

NEXUS_SimpleAudioPlaybackHandle NEXUS_SimpleAudioPlayback_Acquire(unsigned index)
{
    NEXUS_SimpleAudioPlaybackHandle handle;
    NEXUS_Error rc;

    for (handle=BLST_S_FIRST(&g_audioPlaybacks); handle; handle = BLST_S_NEXT(handle, link)) {
        BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioPlayback);
        if ((index == NEXUS_ANY_ID && !handle->acquired) || (handle->index == index)) {
            if (handle->acquired) {
                BERR_TRACE(NEXUS_NOT_AVAILABLE);
                return NULL;
            }
            rc = NEXUS_CLIENT_RESOURCES_ACQUIRE(simpleAudioPlayback,Count,NEXUS_ANY_ID);
            if (rc) { rc = BERR_TRACE(rc); return NULL; }
            
            handle->acquired = true;
            return handle;
        }
    }
    return NULL;
}

void NEXUS_SimpleAudioPlayback_Release( NEXUS_SimpleAudioPlaybackHandle handle )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioPlayback);
    /* IPC handle validation will only allow this call if handle is owned by client.
    For non-IPC used, acquiring is not required, so acquired boolean is not checked in any other API. */
    if (!handle->acquired) {
        BERR_TRACE(NEXUS_NOT_AVAILABLE);
        return;
    }
    if (handle->started) {
        NEXUS_SimpleAudioPlayback_Stop(handle);
    }
    NEXUS_CLIENT_RESOURCES_RELEASE(simpleAudioPlayback,Count,NEXUS_ANY_ID);
    handle->acquired = false;
}

void NEXUS_SimpleAudioPlayback_GetDefaultStartSettings( NEXUS_SimpleAudioPlaybackStartSettings *pSettings )
{
    NEXUS_AudioPlaybackStartSettings start;

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    NEXUS_AudioPlayback_GetDefaultStartSettings(&start);

    /* simple decoder can't use NEXUS_AudioPlaybackStartSettings directly because it has Timebase. */
    pSettings->sampleRate = start.sampleRate;
    pSettings->bitsPerSample = start.bitsPerSample;
    pSettings->startThreshold = start.startThreshold;
    pSettings->stereo = start.stereo;
    pSettings->signedData = start.signedData;
    pSettings->loopAround = start.loopAround;
    pSettings->timebase = start.timebase;
}

NEXUS_Error nexus_simpleaudioplayback_p_internal_start(NEXUS_SimpleAudioPlaybackHandle handle)
{
    if (handle->clientStarted && !handle->started) {
        const NEXUS_SimpleAudioPlaybackStartSettings *pSettings = &handle->startSettings;
        NEXUS_AudioPlaybackStartSettings start;
        NEXUS_Error rc;
        
        NEXUS_AudioPlayback_GetDefaultStartSettings(&start);
        start.sampleRate = pSettings->sampleRate;
        start.bitsPerSample = pSettings->bitsPerSample;
        start.startThreshold = pSettings->startThreshold;
        start.stereo = pSettings->stereo;
        start.signedData = pSettings->signedData;
        start.loopAround = pSettings->loopAround;
        start.dataCallback = pSettings->dataCallback;
        start.timebase = pSettings->timebase;
    
        rc = NEXUS_AudioPlayback_Start(handle->serverSettings.playback, &start);
        if (rc) return BERR_TRACE(rc);
        
        handle->started = true;
    }
    return 0;
}

void nexus_simpleaudioplayback_p_internal_stop(NEXUS_SimpleAudioPlaybackHandle handle)
{
    if (handle->started) {
        NEXUS_AudioPlayback_Stop(handle->serverSettings.playback);
        handle->started = false;
    }
}

NEXUS_Error NEXUS_SimpleAudioPlayback_Start( NEXUS_SimpleAudioPlaybackHandle handle, const NEXUS_SimpleAudioPlaybackStartSettings *pSettings )
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioPlayback);
    if (handle->clientStarted) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    handle->startSettings = *pSettings;
    handle->clientStarted = true;
    rc = nexus_simpleaudioplayback_p_internal_start(handle);
    if (rc) {
        handle->clientStarted = false;
        return BERR_TRACE(rc);
    }
    return 0;
}

void NEXUS_SimpleAudioPlayback_Stop( NEXUS_SimpleAudioPlaybackHandle handle )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioPlayback);
    handle->clientStarted = false;
    nexus_simpleaudioplayback_p_internal_stop(handle);
}

void NEXUS_SimpleAudioPlayback_GetSettings( NEXUS_SimpleAudioPlaybackHandle playback, NEXUS_SimpleAudioPlaybackSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(playback, NEXUS_SimpleAudioPlayback);
    *pSettings = playback->settings;
}

NEXUS_Error NEXUS_SimpleAudioPlayback_SetSettings( NEXUS_SimpleAudioPlaybackHandle playback, const NEXUS_SimpleAudioPlaybackSettings *pSettings )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(playback, NEXUS_SimpleAudioPlayback);

    BDBG_CASSERT(sizeof(NEXUS_SimpleAudioPlaybackSettings) == sizeof(NEXUS_AudioPlaybackSettings));
    rc = NEXUS_AudioPlayback_SetSettings(playback->serverSettings.playback, (const NEXUS_AudioPlaybackSettings *)pSettings);
    if (rc) return BERR_TRACE(rc);

    playback->settings = *pSettings;
    return 0;
}

NEXUS_Error NEXUS_SimpleAudioPlayback_GetStatus( NEXUS_SimpleAudioPlaybackHandle handle, NEXUS_SimpleAudioPlaybackStatus *pStatus )
{
    NEXUS_AudioPlaybackStatus status;

    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioPlayback);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));

    NEXUS_AudioPlayback_GetStatus(handle->serverSettings.playback, &status);

    pStatus->started = status.started;
    pStatus->queuedBytes = status.queuedBytes;
    pStatus->fifoSize = status.fifoSize;
    pStatus->playedBytes = status.playedBytes;

    return 0;
}

NEXUS_Error NEXUS_SimpleAudioPlayback_GetBuffer( NEXUS_SimpleAudioPlaybackHandle handle, void **pBuffer, size_t *pSize )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioPlayback);
    return NEXUS_AudioPlayback_GetBuffer(handle->serverSettings.playback, pBuffer, pSize);
}

NEXUS_Error NEXUS_SimpleAudioPlayback_WriteComplete( NEXUS_SimpleAudioPlaybackHandle handle, size_t amountWritten )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioPlayback);
    return NEXUS_AudioPlayback_WriteComplete(handle->serverSettings.playback, amountWritten);
}

void NEXUS_SimpleAudioPlayback_Flush( NEXUS_SimpleAudioPlaybackHandle handle )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioPlayback);
    NEXUS_AudioPlayback_Flush(handle->serverSettings.playback);
}

void NEXUS_SimpleAudioPlayback_GetServerSettings( NEXUS_SimpleAudioPlaybackHandle handle, NEXUS_SimpleAudioPlaybackServerSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioPlayback);
    *pSettings = handle->serverSettings;
}

NEXUS_Error NEXUS_SimpleAudioPlayback_SetServerSettings( NEXUS_SimpleAudioPlaybackHandle handle, const NEXUS_SimpleAudioPlaybackServerSettings *pSettings )
{
    NEXUS_Error rc;
    
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleAudioPlayback);
    
    if (handle->started && (pSettings->playback != handle->serverSettings.playback || pSettings->decoder != handle->serverSettings.decoder)) {
        bool clientStarted = handle->clientStarted;
        NEXUS_SimpleAudioPlayback_Stop(handle);
        handle->clientStarted = clientStarted; /* restore client's desired state */
    }
    
    handle->serverSettings = *pSettings;
    
    if (handle->serverSettings.playback) {
        if (handle->clientStarted) {
            handle->clientStarted = false; /* Start will set true */
            rc = NEXUS_SimpleAudioPlayback_Start(handle, &handle->startSettings);
            if (rc) {rc = BERR_TRACE(rc);} /* fall through */
        }
    }
    
    return 0;
}
