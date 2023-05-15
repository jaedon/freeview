/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_video_decoder_dispatch.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 10/19/12 4:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/src/nexus_video_decoder_dispatch.c $
 * 
 * 7   10/19/12 4:21p erickson
 * SW7425-1792: unregister videoInput in _P_Release
 * 
 * 6   6/19/12 1:02p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 5   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 4   6/13/12 3:43p erickson
 * SW7425-3170: refactor VideoDecoderPrimer api
 * 
 * 3   6/11/12 2:23p erickson
 * SW7425-3170: refactor VideoDecoderPrimer api
 * 
 * 2   6/8/12 2:15p erickson
 * SW7425-3170: promote NEXUS_VideoDecoderPrimerHandle to a tracked
 *  handle, refactor API with backward compatible macros
 * 
 * 1   9/8/10 12:04p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 * 
 * SW7468-129/3   3/5/10 7:30p vsilyaev
 * SW7468-129: Added hooks for soft video decoder
 * 
 * SW7468-129/2   3/1/10 7:09p vsilyaev
 * SW7468-129: Added private interfaces for ZSP video decoder
 * 
 * SW7468-129/1   2/26/10 7:09p vsilyaev
 * SW7468-129: Added ZSP video decoder stub
 * 
 **************************************************************************/
#include "nexus_video_decoder_module.h"
#include "nexus_still_decoder_impl.h"
BDBG_MODULE(nexus_video_decoder_dispatch);

static void NEXUS_VideoDecoder_P_Finalizer( NEXUS_VideoDecoderHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->Close) {
        handle->interface->Close(handle);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

static void NEXUS_VideoDecoder_P_Release(NEXUS_VideoDecoderHandle videoDecoder)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_OBJECT_UNREGISTER(NEXUS_VideoInput, &videoDecoder->input, Close);
}

NEXUS_OBJECT_CLASS_MAKE_WITH_RELEASE(NEXUS_VideoDecoder, NEXUS_VideoDecoder_Close);

void NEXUS_VideoDecoder_GetOpenSettings( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderOpenSettings *pOpenSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetOpenSettings) {
        handle->interface->GetOpenSettings(handle, pOpenSettings);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

void NEXUS_VideoDecoder_GetSettings( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetSettings) {
        handle->interface->GetSettings(handle, pSettings);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_SetSettings( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->SetSettings) {
        return handle->interface->SetSettings(handle, pSettings);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_Start( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderStartSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->Start) {
        return handle->interface->Start( handle, pSettings);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_VideoDecoder_Stop( NEXUS_VideoDecoderHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->Stop) {
        handle->interface->Stop( handle);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

void NEXUS_VideoDecoder_Flush( NEXUS_VideoDecoderHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->Flush) {
        handle->interface->Flush(handle);
    } else {
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_GetStatus( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderStatus *pStatus)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetStatus) {
        return handle->interface->GetStatus(handle, pStatus);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_VideoInput NEXUS_VideoDecoder_GetConnector(NEXUS_VideoDecoderHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetConnector) {
        return handle->interface->GetConnector(handle);
    } else {
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
        return NULL;
    }
}

NEXUS_Error NEXUS_VideoDecoder_GetStreamInformation(NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderStreamInformation *pStreamInformation)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetStreamInformation) {
       return handle->interface->GetStreamInformation(handle, pStreamInformation);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_SetStartPts( NEXUS_VideoDecoderHandle handle, uint32_t pts)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->SetStartPts) {
       return handle->interface->SetStartPts( handle, pts);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_VideoDecoder_IsCodecSupported( NEXUS_VideoDecoderHandle handle, NEXUS_VideoCodec codec, bool *pSupported)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->IsCodecSupported) {
        handle->interface->IsCodecSupported( handle, codec, pSupported);
    } else {
        *pSupported = false;
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_SetPowerState( NEXUS_VideoDecoderHandle handle, bool powerUp)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->SetPowerState ) {
        return handle->interface->SetPowerState( handle, powerUp);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}


void NEXUS_VideoDecoder_Reset( NEXUS_VideoDecoderHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->Reset) {
        handle->interface->Reset(handle);
    } else {
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
        return;
    }
}

NEXUS_Error NEXUS_VideoDecoder_GetExtendedStatus( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderExtendedStatus *pStatus)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetExtendedStatus) {
        return handle->interface->GetExtendedStatus( handle, pStatus);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_VideoDecoder_GetExtendedSettings( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderExtendedSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetExtendedSettings) {
        handle->interface->GetExtendedSettings( handle, pSettings);
    } else {
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_SetExtendedSettings( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderExtendedSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->SetExtendedSettings) {
        return handle->interface->SetExtendedSettings( handle, pSettings);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_StripedSurfaceHandle NEXUS_VideoDecoder_CreateStripedSurface( NEXUS_VideoDecoderHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->CreateStripedSurface) {
        return handle->interface->CreateStripedSurface(handle);
    } else {
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
        return NULL;
    }
}

void NEXUS_VideoDecoder_DestroyStripedSurface( NEXUS_VideoDecoderHandle handle, NEXUS_StripedSurfaceHandle stripedSurface)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->DestroyStripedSurface) {
        handle->interface->DestroyStripedSurface( handle, stripedSurface);
    } else {
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_CreateStripedMosaicSurfaces( NEXUS_VideoDecoderHandle handle, NEXUS_StripedSurfaceHandle *pStripedSurfaces, unsigned int maxSurfaces, unsigned int *pSurfaceCount)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->CreateStripedMosaicSurfaces) {
        return handle->interface->CreateStripedMosaicSurfaces( handle, pStripedSurfaces, maxSurfaces, pSurfaceCount);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_VideoDecoder_DestroyStripedMosaicSurfaces( NEXUS_VideoDecoderHandle handle, const NEXUS_StripedSurfaceHandle *pStripedSurfaces, unsigned int surfaceCount)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->DestroyStripedMosaicSurfaces) {
        handle->interface->DestroyStripedMosaicSurfaces( handle, pStripedSurfaces, surfaceCount);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_GetMostRecentPts( NEXUS_VideoDecoderHandle handle, uint32_t *pPts)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetMostRecentPts) {
        return handle->interface->GetMostRecentPts( handle, pPts);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_VideoDecoderPrimerHandle NEXUS_VideoDecoderPrimer_Open( NEXUS_VideoDecoderHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->OpenPrimer) {
        return handle->interface->OpenPrimer(handle);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return NULL;
    }
}

void NEXUS_VideoDecoderPrimer_Close( NEXUS_VideoDecoderPrimerHandle primer)
{
    NEXUS_VideoDecoderPrimer_P_Close_Avd(primer);
}

NEXUS_Error NEXUS_VideoDecoderPrimer_Start(NEXUS_VideoDecoderPrimerHandle primer, const NEXUS_VideoDecoderStartSettings *pStartSettings)
{
    NEXUS_VideoDecoderHandle handle = nexus_videodecoderprimer_p_getdecoder(primer);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->StartPrimer) {
        return handle->interface->StartPrimer(primer, pStartSettings);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_VideoDecoderPrimer_Stop(NEXUS_VideoDecoderPrimerHandle primer)
{
    NEXUS_VideoDecoderHandle handle = nexus_videodecoderprimer_p_getdecoder(primer);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->StopPrimer) {
        handle->interface->StopPrimer(primer);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoderPrimer_StopPrimerAndStartDecode(NEXUS_VideoDecoderPrimerHandle primer, NEXUS_VideoDecoderHandle handle)
{
    NEXUS_VideoDecoderHandle check_handle = nexus_videodecoderprimer_p_getdecoder(primer);    
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    if (check_handle != handle) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    BDBG_ASSERT(handle->interface);
    if(handle->interface->StopPrimerAndStartDecode) {
        return handle->interface->StopPrimerAndStartDecode(primer, handle);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_VideoDecoder_GetTrickState( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderTrickState *pState)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetTrickState) {
        handle->interface->GetTrickState( handle, pState);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_SetTrickState( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderTrickState *pState)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->SetTrickState) {
        return handle->interface->SetTrickState( handle, pState);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_FrameAdvance(NEXUS_VideoDecoderHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->FrameAdvance) {
        return handle->interface->FrameAdvance(handle);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_GetNextPts( NEXUS_VideoDecoderHandle handle, uint32_t *pNextPts)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetNextPts) {
        return handle->interface->GetNextPts( handle, pNextPts);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_VideoDecoder_GetPlaybackSettings( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderPlaybackSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetPlaybackSettings) {
        handle->interface->GetPlaybackSettings( handle, pSettings);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_SetPlaybackSettings( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderPlaybackSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->SetPlaybackSettings) {
        return handle->interface->SetPlaybackSettings( handle, pSettings);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_VideoDecoder_GetDisplayConnection_priv( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderDisplayConnection *pConnection)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetDisplayConnection_priv) {
        handle->interface->GetDisplayConnection_priv( handle, pConnection);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_SetDisplayConnection_priv( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderDisplayConnection *pConnection)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->SetDisplayConnection_priv) {
        return handle->interface->SetDisplayConnection_priv( handle, pConnection);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_VideoDecoder_GetSourceId_priv( NEXUS_VideoDecoderHandle handle, BAVC_SourceId *pSource)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetSourceId_priv) {
        handle->interface->GetSourceId_priv( handle, pSource);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

void NEXUS_VideoDecoder_GetHeap_priv( NEXUS_VideoDecoderHandle handle, NEXUS_HeapHandle *pHeap)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetHeap_priv) {
        handle->interface->GetHeap_priv( handle, pHeap);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

void NEXUS_VideoDecoder_GetSyncSettings_priv(NEXUS_VideoDecoderHandle handle, NEXUS_VideoInputSyncSettings *pSyncSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetSyncSettings_priv) {
        handle->interface->GetSyncSettings_priv( handle, pSyncSettings);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_SetSyncSettings_priv(NEXUS_VideoDecoderHandle handle, const NEXUS_VideoInputSyncSettings *pSyncSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->SetSyncSettings_priv) {
        return handle->interface->SetSyncSettings_priv( handle, pSyncSettings);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_GetSyncStatus_isr(NEXUS_VideoDecoderHandle handle, NEXUS_VideoInputSyncStatus *pSyncStatus )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetSyncStatus_isr) {
        return handle->interface->GetSyncStatus_isr( handle, pSyncStatus);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_VideoDecoder_UpdateDisplayInformation_priv( NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoder_DisplayInformation *displayInformation)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->UpdateDisplayInformation_priv) {
        handle->interface->UpdateDisplayInformation_priv( handle, displayInformation);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

#if NEXUS_HAS_ASTM
void NEXUS_VideoDecoder_GetAstmSettings_priv( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderAstmSettings * pAstmSettings)
{
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetAstmSettings_priv) {
        handle->interface->GetAstmSettings_priv( handle, pAstmSettings);
    } else {
        (void)BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_SetAstmSettings_priv(NEXUS_VideoDecoderHandle handle, const NEXUS_VideoDecoderAstmSettings * pAstmSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->SetAstmSettings_priv) {
        return handle->interface->SetAstmSettings_priv( handle, pAstmSettings);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_VideoDecoder_GetAstmStatus_isr( NEXUS_VideoDecoderHandle handle, NEXUS_VideoDecoderAstmStatus * pAstmStatus)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetAstmStatus_isr) {
        return handle->interface->GetAstmStatus_isr( handle, pAstmStatus);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}
#endif /* NEXUS_HAS_ASTM */


NEXUS_StillDecoderHandle NEXUS_StillDecoder_Open( NEXUS_VideoDecoderHandle handle, unsigned index, const NEXUS_StillDecoderOpenSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_VideoDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->StillDecoder_Open) {
        NEXUS_StillDecoderHandle stillDecoder = handle->interface->StillDecoder_Open( handle, index, pSettings);
        if(stillDecoder) {
            NEXUS_OBJECT_SET(NEXUS_StillDecoder, stillDecoder);
        }
        return stillDecoder;
    } else {
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
        return NULL;
    }
}

static void NEXUS_StillDecoder_P_Finalizer( NEXUS_StillDecoderHandle handle)
{
    BDBG_ASSERT(handle->interface);
    if(handle->interface->Close) {
        handle->interface->Close(handle);
    } else {
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_StillDecoder, NEXUS_StillDecoder_Close);

NEXUS_Error NEXUS_StillDecoder_Start( NEXUS_StillDecoderHandle handle, const NEXUS_StillDecoderStartSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_StillDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->Start) {
        return handle->interface->Start( handle, pSettings);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

void NEXUS_StillDecoder_Stop( NEXUS_StillDecoderHandle handle)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_StillDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->Stop) {
        handle->interface->Stop(handle);
    } else {
        (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_StillDecoder_GetStatus( NEXUS_StillDecoderHandle handle, NEXUS_StillDecoderStatus *pStatus )
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_StillDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetStatus) {
        return handle->interface->GetStatus( handle, pStatus );
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

NEXUS_Error NEXUS_StillDecoder_GetStripedSurface( NEXUS_StillDecoderHandle handle, NEXUS_StripedSurfaceHandle *pStripedSurface)
{
    BDBG_OBJECT_ASSERT(handle, NEXUS_StillDecoder);
    BDBG_ASSERT(handle->interface);
    if(handle->interface->GetStripedSurface) {
        return handle->interface->GetStripedSurface( handle, pStripedSurface);
    } else {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
}

