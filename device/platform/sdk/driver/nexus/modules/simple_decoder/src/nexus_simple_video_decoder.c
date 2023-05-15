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
 * $brcm_Workfile: nexus_simple_video_decoder.c $
 * $brcm_Revision: 32 $
 * $brcm_Date: 10/16/12 5:19p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/simple_decoder/src/nexus_simple_video_decoder.c $
 * 
 * 32   10/16/12 5:19p erickson
 * SW7425-1792: remove wrong assert
 * 
 * 31   9/17/12 3:05p erickson
 * SW7435-344: remove manual NEXUS_OBJECT_REGISTER/UNREGISTER
 * 
 * 30   8/22/12 3:16p erickson
 * SW7420-1135: fix warning
 * 
 * 29   8/22/12 1:38p erickson
 * SW7420-1135: don't require a window is passed to simple decoder. the
 *  app may have connected it or is bypassing the MFD.
 * 
 * 28   8/9/12 5:27p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 27   8/6/12 4:36p erickson
 * SW7425-3572: add userdata capture
 * 
 * 26   7/23/12 11:36a vsilyaev
 * SW7420-2328: Use per-thread storage to keep information about the
 *  current client, remove per-module client information
 * 
 * 25   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 24   4/24/12 5:02p erickson
 * SW7420-1148: check return code of nexus_register_xxx calls
 * 
 * 23   3/13/12 4:50p erickson
 * SW7425-2607: use unvarying macros in API for binary compatibility
 * 
 * 22   2/9/12 12:38p erickson
 * SW7420-2237: remove NEXUS_SimpleVideoDecoder_SetWindowSettings
 * 
 * 21   12/8/11 9:18a erickson
 * SW7231-521: some settings require a default that's preserved from the
 *  initial state of the incoming regular decoder
 * 
 * 20   11/10/11 4:55p erickson
 * SW7420-1135: fix window array bounds
 * 
 * 19   11/2/11 4:21p erickson
 * SW7420-1992: add Settings, ExtendedSettings, SetStartPts
 *
 * 18   10/26/11 3:45p erickson
 * SW7420-1135: implement SetServerSettings that allows resources to by
 *  removed and restored while client has acquired decoder
 *
 * 17   10/11/11 8:26a erickson
 * SW7420-1992: use regular-decoder start settings, client allocates stc
 *  channel, add disable mode enum
 *
 * 16   10/5/11 3:14p erickson
 * SW7420-1135: enable NEXUS_VideoDecoderPlaybackSettings callbacks
 *
 * 15   10/5/11 1:06p erickson
 * SW7420-1148: add explicit register/unregister when server
 *  creates/destroys clients
 *
 * 14   10/3/11 9:51a erickson
 * SW7420-1148: add HD/SD simul support
 *
 * 13   9/22/11 4:43p erickson
 * SW7420-1965: add simple decoder support to nexus playback module
 *
 * 12   6/1/11 2:49p erickson
 * SW7425-614: add
 *  NEXUS_SimpleVideoDecoderStartSettings.enhancementPidChannel
 *
 * 11   3/2/11 12:18p erickson
 * SW7420-1560: change NEXUS_CalculateVideoWindowPosition (not backward
 *  compatible) so that NEXUS_VideoWindowSettings param is not [in/out]
 *
 * 10   2/28/11 1:51p erickson
 * SW7405-5105: allow playback to start before audio decode, but still use
 *  audio's STC
 *
 * 9   1/24/11 3:58p erickson
 * SW7405-5105: add simple audio playback
 *
 * 8   12/2/10 3:56p erickson
 * SW7420-1227: allow virtual (offscreen) window coordinates
 *
 * 7   11/17/10 3:49p erickson
 * SW7420-1135: add NEXUS_VideoInput_Shutdown. stop an unacquired decoder
 *  on destroy.
 *
 * 6   11/10/10 10:38a erickson
 * SW7420-1135: fix cleanup
 *
 * 5   11/1/10 2:29p erickson
 * SW7420-1135: add TSM support
 *
 * 4   10/29/10 4:33p erickson
 * SW7420-1135: update
 *
 * 3   10/29/10 2:59p erickson
 * SW7420-1135: defer window connection until start time
 *
 * 2   10/22/10 11:17a erickson
 * SW7420-1135: allow runtime changes of maxWidth/maxHeight for mosaic
 *  decodes
 *
 * 1   10/13/10 10:32a erickson
 * SW7420-1135: add
 *
 **************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#include "nexus_simple_decoder_module.h"
#include "nexus_class_verification.h"

/* internal apis */
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_extra.h"
#include "nexus_video_decoder_trick.h"
#include "nexus_mosaic_video_decoder.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_core_utils.h"
#include "nexus_client_resources.h"

BDBG_MODULE(nexus_simple_video_decoder);

#define BDBG_MSG_TRACE(X)

struct NEXUS_SimpleVideoDecoder
{
    NEXUS_OBJECT(NEXUS_SimpleVideoDecoder);
    BLST_S_ENTRY(NEXUS_SimpleVideoDecoder) link;
    unsigned index;
    bool acquired;
    bool started; /* actually started. if started, must be connected. may or may not be acquired. */
    bool clientStarted; /* user has started. if resource present, started will also match. */
    bool connected; /* to VideoWindow */
    NEXUS_SimpleVideoDecoderServerSettings serverSettings;
    NEXUS_SimpleVideoDecoderStartSettings startSettings;
    NEXUS_VideoDecoderTrickState trickSettings; /* default is normal play */
    
    /* the following settings require a default that's preserved from the initial state of the incoming regular decoder */
    struct {
        NEXUS_VideoDecoderPlaybackSettings playbackSettings;
        NEXUS_VideoDecoderExtendedSettings extendedSettings;
        NEXUS_VideoDecoderSettings settings;
    } currentSettings, defaultSettings;
};

static BLST_S_HEAD(NEXUS_SimpleVideoDecoder_P_List, NEXUS_SimpleVideoDecoder) g_videoDecoders;

static NEXUS_Error nexus_simplevideodecoder_p_setdecodersettings(NEXUS_SimpleVideoDecoderHandle handle, bool currentSettings);

/**
server functions
**/

void NEXUS_SimpleVideoDecoder_GetDefaultServerSettings( NEXUS_SimpleVideoDecoderServerSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->enabled = true;
}

NEXUS_SimpleVideoDecoderHandle NEXUS_SimpleVideoDecoder_Create( unsigned index, const NEXUS_SimpleVideoDecoderServerSettings *pSettings )
{
    NEXUS_SimpleVideoDecoderHandle handle;
    NEXUS_Error rc;

    /* find dup */
    for (handle=BLST_S_FIRST(&g_videoDecoders); handle; handle=BLST_S_NEXT(handle, link)) {
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
    NEXUS_OBJECT_INIT(NEXUS_SimpleVideoDecoder, handle);
    handle->index = index;

    /* insert in order. not required, but makes debug easier */
    if (!BLST_S_FIRST(&g_videoDecoders)) {
        BLST_S_INSERT_HEAD(&g_videoDecoders, handle, link);
    }
    else {
        NEXUS_SimpleVideoDecoderHandle prev;
        for (prev=BLST_S_FIRST(&g_videoDecoders);;prev=BLST_S_NEXT(prev, link)) {
            if (!BLST_S_NEXT(prev, link)) {
                BLST_S_INSERT_AFTER(&g_videoDecoders, prev, handle, link);
                break;
            }
        }
    }
    /* now a valid object */

    rc = NEXUS_SimpleVideoDecoder_SetServerSettings(handle, pSettings);
    if (rc) { rc = BERR_TRACE(rc); goto error; }

    return handle;

error:
    NEXUS_SimpleVideoDecoder_Destroy(handle);
    return NULL;
}

static void NEXUS_SimpleVideoDecoder_P_Release( NEXUS_SimpleVideoDecoderHandle handle )
{
    NEXUS_OBJECT_ASSERT(NEXUS_SimpleVideoDecoder, handle);
    if (handle->acquired) {
        NEXUS_SimpleVideoDecoder_Release(handle);
    }
    if (handle->connected) {
        NEXUS_SimpleVideoDecoder_StopAndFree(handle);
    }
    BDBG_ASSERT(!handle->acquired);
    BDBG_ASSERT(!handle->connected);
    BDBG_ASSERT(!handle->started);
    return;
}

static void NEXUS_SimpleVideoDecoder_P_Finalizer( NEXUS_SimpleVideoDecoderHandle handle )
{
    NEXUS_OBJECT_ASSERT(NEXUS_SimpleVideoDecoder, handle);
    BLST_S_REMOVE(&g_videoDecoders, handle, NEXUS_SimpleVideoDecoder, link);
    NEXUS_OBJECT_DESTROY(NEXUS_SimpleVideoDecoder, handle);
    BKNI_Free(handle);
    return;
}

NEXUS_OBJECT_CLASS_MAKE_WITH_RELEASE(NEXUS_SimpleVideoDecoder, NEXUS_SimpleVideoDecoder_Destroy);

void NEXUS_SimpleVideoDecoder_GetServerSettings( NEXUS_SimpleVideoDecoderHandle handle, NEXUS_SimpleVideoDecoderServerSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    *pSettings = handle->serverSettings;
}

NEXUS_Error NEXUS_SimpleVideoDecoder_SetServerSettings( NEXUS_SimpleVideoDecoderHandle handle, const NEXUS_SimpleVideoDecoderServerSettings *pSettings )
{
    NEXUS_Error rc;
    bool decoder_change;

    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);

    decoder_change = (handle->serverSettings.videoDecoder != pSettings->videoDecoder);
    
    /* we might be losing our decoders so, disconnect */
    if (handle->connected && decoder_change) {
        bool clientStarted = handle->clientStarted;
        NEXUS_SimpleVideoDecoder_StopAndFree(handle);
        handle->clientStarted = clientStarted; /* restore client's desired state */
    }

    handle->serverSettings = *pSettings;

    /* remember its initial settings so they can be reapplied */
    if (decoder_change && handle->serverSettings.videoDecoder) {
        NEXUS_CallbackDesc nullCallback = NEXUS_CALLBACKDESC_INITIALIZER();
        
        NEXUS_VideoDecoder_GetPlaybackSettings(handle->serverSettings.videoDecoder, &handle->defaultSettings.playbackSettings);
        NEXUS_VideoDecoder_GetSettings(handle->serverSettings.videoDecoder, &handle->defaultSettings.settings);
        NEXUS_VideoDecoder_GetExtendedSettings(handle->serverSettings.videoDecoder, &handle->defaultSettings.extendedSettings);
        
        /* don't inherit any callbacks */
        handle->defaultSettings.playbackSettings.firstPts = nullCallback;
        handle->defaultSettings.playbackSettings.firstPtsPassed = nullCallback;
        handle->defaultSettings.settings.sourceChanged = nullCallback;
        handle->defaultSettings.settings.streamChanged = nullCallback;
        handle->defaultSettings.settings.appUserDataReady = nullCallback;
        handle->defaultSettings.settings.ptsError = nullCallback;
        handle->defaultSettings.settings.firstPts = nullCallback;
        handle->defaultSettings.settings.firstPtsPassed = nullCallback;
        handle->defaultSettings.settings.fifoEmpty = nullCallback;
        handle->defaultSettings.settings.afdChanged = nullCallback;
        handle->defaultSettings.settings.decodeError = nullCallback;
        handle->defaultSettings.extendedSettings.dataReadyCallback = nullCallback;
        handle->defaultSettings.extendedSettings.s3DTVStatusChanged = nullCallback;
        
        /* if already acquired, then stick with the current settings */
        if (!handle->acquired) {
            handle->currentSettings = handle->defaultSettings;
        }
    }
    
    if (handle->serverSettings.videoDecoder && handle->serverSettings.enabled) {
        if (handle->clientStarted) {
            handle->clientStarted = false; /* Start will set true */
            rc = NEXUS_SimpleVideoDecoder_Start(handle, &handle->startSettings);
            if (rc) {rc = BERR_TRACE(rc);} /* fall through */

            if (!rc) {
                rc = NEXUS_SimpleVideoDecoder_SetTrickState(handle, &handle->trickSettings);
                if (rc) {rc = BERR_TRACE(rc);} /* fall through */
            }
        }
    }

    return 0;
}

/**
client functions
**/

static NEXUS_Error nexus_simplevideodecoder_has_resource(NEXUS_SimpleVideoDecoderHandle handle, NEXUS_Error *pResult)
{
    if (handle->serverSettings.videoDecoder && handle->serverSettings.enabled) {
        *pResult = 0;
        return 0;
    }
    switch (handle->serverSettings.disableMode) {
    case NEXUS_SimpleDecoderDisableMode_eSuccess:
    case NEXUS_SimpleDecoderDisableMode_eFailOnStart:
        *pResult = 0;
        break;
    default:
        *pResult = NEXUS_NOT_AVAILABLE;
        break;
    }
    return NEXUS_NOT_AVAILABLE;
}

NEXUS_SimpleVideoDecoderHandle NEXUS_SimpleVideoDecoder_Acquire( unsigned index )
{
    NEXUS_Error rc;
    NEXUS_SimpleVideoDecoderHandle handle;

    for (handle=BLST_S_FIRST(&g_videoDecoders); handle; handle = BLST_S_NEXT(handle, link)) {
        BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
        if ((index == NEXUS_ANY_ID && !handle->acquired) || (handle->index == index)) {
            if (handle->acquired) {
                BERR_TRACE(NEXUS_NOT_AVAILABLE);
                return NULL;
            }
            rc = NEXUS_CLIENT_RESOURCES_ACQUIRE(simpleVideoDecoder,IdList,index);
            if (rc) { rc = BERR_TRACE(rc); return NULL; }
            handle->acquired = true;
            return handle;
        }
    }
    return NULL;
}

void NEXUS_SimpleVideoDecoder_Release( NEXUS_SimpleVideoDecoderHandle handle )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    /* IPC handle validation will only allow this call if handle is owned by client.
    For non-IPC used, acquiring is not required, so acquired boolean is not checked in any other API. */
    if (!handle->acquired) {
        BERR_TRACE(NEXUS_NOT_AVAILABLE);
        return;
    }
    if (handle->connected) {
        NEXUS_SimpleVideoDecoder_StopAndFree(handle);
    }

    /* go back to default settings in the simple decoder and the regular decoder */
    handle->currentSettings = handle->defaultSettings;
    
    NEXUS_CLIENT_RESOURCES_RELEASE(simpleVideoDecoder,IdList,handle->index);
    handle->acquired = false;
}

void NEXUS_SimpleVideoDecoder_GetDefaultStartSettings( NEXUS_SimpleVideoDecoderStartSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    NEXUS_VideoDecoder_GetDefaultStartSettings(&pSettings->settings);
    pSettings->maxWidth = 1920;
    pSettings->maxHeight = 1080;
}

static void nexus_simplevideodecoder_disconnect(NEXUS_SimpleVideoDecoderHandle handle);

static NEXUS_Error nexus_simplevideodecoder_connect (NEXUS_SimpleVideoDecoderHandle handle)
{
    unsigned i;
    NEXUS_Error rc;

    rc = nexus_simplevideodecoder_p_setdecodersettings(handle, true);
    if (rc) return BERR_TRACE(rc);
    
    for (i=0;i<NEXUS_MAX_DISPLAYS;i++) {
        if (handle->serverSettings.window[i]) {
            rc = NEXUS_VideoWindow_AddInput(handle->serverSettings.window[i], NEXUS_VideoDecoder_GetConnector(handle->serverSettings.videoDecoder));
            if (rc) {rc = BERR_TRACE(rc); goto error;}
        }
    }

    handle->connected = true;
    return 0;

error:
    nexus_simplevideodecoder_disconnect(handle);
    return rc;
}

static void nexus_simplevideodecoder_disconnect(NEXUS_SimpleVideoDecoderHandle handle)
{
    unsigned i;
    for (i=0;i<NEXUS_MAX_DISPLAYS;i++) {
        if (handle->serverSettings.window[i]) {
            NEXUS_VideoWindow_RemoveAllInputs(handle->serverSettings.window[i]);
        }
    }
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(handle->serverSettings.videoDecoder));
    nexus_simplevideodecoder_p_setdecodersettings(handle, false);
    handle->connected = false;
}

NEXUS_Error NEXUS_SimpleVideoDecoder_Start( NEXUS_SimpleVideoDecoderHandle handle, const NEXUS_SimpleVideoDecoderStartSettings *pSettings )
{
    NEXUS_Error rc;
    NEXUS_VideoDecoderMosaicSettings mosaicSettings;

    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);

    if (handle->clientStarted) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }

    if (!(handle->serverSettings.videoDecoder && handle->serverSettings.enabled)) {
        if (handle->serverSettings.disableMode == NEXUS_SimpleDecoderDisableMode_eSuccess) {
            handle->clientStarted = true;
            return 0;
        }
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }

    /* if we can change them, and if the user wants to set them, then do so. */
    NEXUS_VideoDecoder_GetMosaicSettings(handle->serverSettings.videoDecoder, &mosaicSettings);
    if (pSettings->maxHeight != mosaicSettings.maxHeight || pSettings->maxWidth != mosaicSettings.maxWidth) {
        if (handle->connected) {
            nexus_simplevideodecoder_disconnect(handle);
        }

        mosaicSettings.maxWidth = pSettings->maxWidth;
        mosaicSettings.maxHeight = pSettings->maxHeight;
        rc = NEXUS_VideoDecoder_SetMosaicSettings(handle->serverSettings.videoDecoder, &mosaicSettings);
        if (rc) {rc = BERR_TRACE(rc);} /* fall through. try to keep decoding with previous settings. */
    }

    if (!handle->connected) {
        nexus_simplevideodecoder_connect(handle);
    }

    rc = NEXUS_VideoDecoder_Start(handle->serverSettings.videoDecoder, &pSettings->settings);
    if (rc) return BERR_TRACE(rc);

    handle->clientStarted = true;
    handle->started = true;
    handle->startSettings = *pSettings;
    return 0;
}

void NEXUS_SimpleVideoDecoder_Stop( NEXUS_SimpleVideoDecoderHandle handle )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    handle->clientStarted = false;
    if (handle->started) {
        NEXUS_VideoDecoder_Stop(handle->serverSettings.videoDecoder);
        handle->started = false;
    }
}

void NEXUS_SimpleVideoDecoder_StopAndFree( NEXUS_SimpleVideoDecoderHandle handle )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);

    if (handle->clientStarted) {
        NEXUS_SimpleVideoDecoder_Stop(handle);
    }
    if (handle->connected) {
        nexus_simplevideodecoder_disconnect(handle);
    }
}

NEXUS_Error NEXUS_SimpleVideoDecoder_GetStatus( NEXUS_SimpleVideoDecoderHandle handle, NEXUS_VideoDecoderStatus *pStatus )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);

    if (nexus_simplevideodecoder_has_resource(handle, &rc)) {
        BKNI_Memset(pStatus, 0, sizeof(*pStatus));
        return rc; /* no BERR_TRACE */
    }
    return NEXUS_VideoDecoder_GetStatus(handle->serverSettings.videoDecoder, pStatus);
}

void NEXUS_SimpleVideoDecoder_Flush( NEXUS_SimpleVideoDecoderHandle handle )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    if (nexus_simplevideodecoder_has_resource(handle, &rc)) return;
    NEXUS_VideoDecoder_Flush(handle->serverSettings.videoDecoder);
}

void NEXUS_SimpleVideoDecoder_GetTrickState( NEXUS_SimpleVideoDecoderHandle handle, NEXUS_VideoDecoderTrickState *pSettings )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    *pSettings = handle->trickSettings;
}

NEXUS_Error NEXUS_SimpleVideoDecoder_SetTrickState( NEXUS_SimpleVideoDecoderHandle handle, const NEXUS_VideoDecoderTrickState *pSettings )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    handle->trickSettings = *pSettings;
    if (nexus_simplevideodecoder_has_resource(handle, &rc)) return rc; /* no BERR_TRACE */
    rc = NEXUS_VideoDecoder_SetTrickState(handle->serverSettings.videoDecoder, pSettings);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

NEXUS_Error NEXUS_SimpleVideoDecoder_FrameAdvance( NEXUS_SimpleVideoDecoderHandle handle )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    if (nexus_simplevideodecoder_has_resource(handle, &rc)) return rc; /* no BERR_TRACE */
    rc = NEXUS_VideoDecoder_FrameAdvance(handle->serverSettings.videoDecoder);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

void NEXUS_SimpleVideoDecoder_GetPlaybackSettings( NEXUS_SimpleVideoDecoderHandle handle, NEXUS_VideoDecoderPlaybackSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    *pSettings = handle->currentSettings.playbackSettings;
}

NEXUS_Error NEXUS_SimpleVideoDecoder_SetPlaybackSettings( NEXUS_SimpleVideoDecoderHandle handle, const NEXUS_VideoDecoderPlaybackSettings *pSettings )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    handle->currentSettings.playbackSettings = *pSettings;
    if (nexus_simplevideodecoder_has_resource(handle, &rc)) return rc; /* no BERR_TRACE */
    rc = NEXUS_VideoDecoder_SetPlaybackSettings(handle->serverSettings.videoDecoder, pSettings);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

NEXUS_Error NEXUS_SimpleVideoDecoder_GetClientStatus( NEXUS_SimpleVideoDecoderHandle handle, NEXUS_SimpleVideoDecoderClientStatus *pStatus )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    BKNI_Memset(pStatus, 0, sizeof(*pStatus));
    pStatus->enabled = handle->serverSettings.videoDecoder && handle->serverSettings.enabled;
    return 0;
}

void NEXUS_SimpleVideoDecoder_GetSettings( NEXUS_SimpleVideoDecoderHandle handle, NEXUS_VideoDecoderSettings *pSettings )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    if (nexus_simplevideodecoder_has_resource(handle, &rc)) {
        /* used cached value if no resource, but prefer getting from actual resource */
        *pSettings = handle->currentSettings.settings;
    }
    else {
        NEXUS_VideoDecoder_GetSettings(handle->serverSettings.videoDecoder, pSettings);
        handle->currentSettings.settings = *pSettings;
    }}

NEXUS_Error NEXUS_SimpleVideoDecoder_SetSettings( NEXUS_SimpleVideoDecoderHandle handle, const NEXUS_VideoDecoderSettings *pSettings )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    handle->currentSettings.settings = *pSettings;
    if (nexus_simplevideodecoder_has_resource(handle, &rc)) return rc; /* no BERR_TRACE */
    rc = NEXUS_VideoDecoder_SetSettings(handle->serverSettings.videoDecoder, pSettings);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

void NEXUS_SimpleVideoDecoder_GetExtendedSettings( NEXUS_SimpleVideoDecoderHandle handle, NEXUS_VideoDecoderExtendedSettings *pSettings )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    if (nexus_simplevideodecoder_has_resource(handle, &rc)) {
        /* used cached value if no resource, but prefer getting from actual resource */
        *pSettings = handle->currentSettings.extendedSettings;
    }
    else {
        NEXUS_VideoDecoder_GetExtendedSettings(handle->serverSettings.videoDecoder, pSettings);
        handle->currentSettings.extendedSettings = *pSettings;
    }
}

NEXUS_Error NEXUS_SimpleVideoDecoder_SetExtendedSettings( NEXUS_SimpleVideoDecoderHandle handle, const NEXUS_VideoDecoderExtendedSettings *pSettings )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    handle->currentSettings.extendedSettings = *pSettings;
    if (nexus_simplevideodecoder_has_resource(handle, &rc)) return rc; /* no BERR_TRACE */
    rc = NEXUS_VideoDecoder_SetExtendedSettings(handle->serverSettings.videoDecoder, pSettings);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

NEXUS_Error NEXUS_SimpleVideoDecoder_SetStartPts( NEXUS_SimpleVideoDecoderHandle handle, uint32_t pts )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    if (nexus_simplevideodecoder_has_resource(handle, &rc)) return rc; /* no BERR_TRACE */
    rc = NEXUS_VideoDecoder_SetStartPts(handle->serverSettings.videoDecoder, pts);
    if (rc) return BERR_TRACE(rc);
    return 0;
}

static NEXUS_Error nexus_simplevideodecoder_p_setdecodersettings(NEXUS_SimpleVideoDecoderHandle handle, bool currentSettings)
{
    NEXUS_Error rc;
    if (currentSettings) {
        /* apply the current simpledecoder settings back to the regular decoder */
        rc = NEXUS_VideoDecoder_SetPlaybackSettings(handle->serverSettings.videoDecoder, &handle->currentSettings.playbackSettings);
        if (rc) return BERR_TRACE(rc);
        rc = NEXUS_VideoDecoder_SetSettings(handle->serverSettings.videoDecoder, &handle->currentSettings.settings);
        if (rc) return BERR_TRACE(rc);
        rc = NEXUS_VideoDecoder_SetExtendedSettings(handle->serverSettings.videoDecoder, &handle->currentSettings.extendedSettings);
        if (rc) return BERR_TRACE(rc);
    }
    else {
        /* restore the regular decoder's initial settings */
        rc = NEXUS_VideoDecoder_SetPlaybackSettings(handle->serverSettings.videoDecoder, &handle->defaultSettings.playbackSettings);
        if (rc) return BERR_TRACE(rc);
        rc = NEXUS_VideoDecoder_SetSettings(handle->serverSettings.videoDecoder, &handle->defaultSettings.settings);
        if (rc) return BERR_TRACE(rc);
        rc = NEXUS_VideoDecoder_SetExtendedSettings(handle->serverSettings.videoDecoder, &handle->defaultSettings.extendedSettings);
        if (rc) return BERR_TRACE(rc);
    }
    return 0;
}

NEXUS_Error NEXUS_SimpleVideoDecoder_ReadUserDataBuffer(NEXUS_SimpleVideoDecoderHandle handle, void *pBuffer, size_t bufferSize, size_t *pBytesCopied )
{
    NEXUS_Error rc;
    void *internalBuffer;
    unsigned internalSize;
    const NEXUS_UserDataHeader *pHeader;
    unsigned total;
    
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    *pBytesCopied = 0;
    if (nexus_simplevideodecoder_has_resource(handle, &rc)) return rc; /* no BERR_TRACE */
    
    rc = NEXUS_VideoDecoder_GetUserDataBuffer(handle->serverSettings.videoDecoder, &internalBuffer, &internalSize);
    if (rc) return BERR_TRACE(rc);

    /* mempcy is essential for simple decoder because untrusted clients should not have direct access to video memory.
    first, limit it by function params.
    second, limit it by NEXUS_UserDataHeader.blockSize. this makes app logic much easier */
    if (internalSize > bufferSize) {
        internalSize = bufferSize;
    }
    
    /* read as many whole blocks (header + payload) as possible */
    total = 0;
    while (total < internalSize) {
        if (total + sizeof(*pHeader) > internalSize) {
            break;
        }
        pHeader = (const NEXUS_UserDataHeader *)((uint8_t*)internalBuffer + total);
        if (total + pHeader->blockSize > internalSize) {
            break;
        }
        total += pHeader->blockSize;
    }
    if (total) {
        BKNI_Memcpy(pBuffer, internalBuffer, total);
        
        NEXUS_VideoDecoder_UserDataReadComplete(handle->serverSettings.videoDecoder, total);
        *pBytesCopied = total;
    }
    
    return 0;
}

void NEXUS_SimpleVideoDecoder_FlushUserData( NEXUS_SimpleVideoDecoderHandle handle )
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(handle, NEXUS_SimpleVideoDecoder);
    if (nexus_simplevideodecoder_has_resource(handle, &rc)) return;
    NEXUS_VideoDecoder_FlushUserData(handle->serverSettings.videoDecoder);
    return;
}
