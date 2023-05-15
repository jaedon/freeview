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
 * $brcm_Workfile: nexus_video_decoder_extra.c $
 * $brcm_Revision: 27 $
 * $brcm_Date: 8/13/12 1:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/src/nexus_video_decoder_extra.c $
 * 
 * 27   8/13/12 1:18p vsilyaev
 * SW7425-3729: Fixed typo
 * 
 * 26   6/8/12 10:52a vsilyaev
 * SW7425-3176: Added 'stub' implementation for Get/SetExtendedSettings
 * 
 * 25   5/14/12 7:42p bandrews
 * SW7425-2264: merge to main
 * 
 * SW7425-2264/1   5/4/12 9:50p bandrews
 * SW7425-2264: first stab at low latency TSM mode impl
 * 
 * 24   3/14/12 12:34p erickson
 * SW7401-4517: remove old code
 * 
 * 23   1/16/12 11:23a erickson
 * SW7346-629: add NEXUS_VideoDecoder_GetCrcData
 * 
 * 22   8/19/11 9:09a dlwin
 * SW7125-1084: Resolved coverity: 25911
 * 
 * 21   4/14/11 3:32p jtna
 * SWDTV-6321: fix build warnings for 2.6.37 kernel
 * 
 * 20   4/6/11 2:56p erickson
 * SW7342-340: add HW rave indices to NEXUS_VideoDecoderExtendedStatus
 *
 * 19   11/16/10 5:35p jtna
 * SW7405-4558: enable userdata parsing based on
 *  NEXUS_VideoDecoderExtendedSettings.s3DTVStatusEnabled
 *
 * 18   10/14/10 6:40p jtna
 * SW7405-4817: rename struct to s3DTVChangedCallback
 *
 * 17   9/9/10 11:32a jtna
 * SW7405-4702: NEXUS_VideoDecoder_CreateStripedMosaicSurfaces() now
 *  returns NULL on repeat surfaces
 *
 * 16   9/8/10 12:25p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 *
 * 15   8/17/10 4:22p erickson
 * SW7420-928: use NEXUS_VideoBufferType for striped surface
 *
 * 14   8/16/10 2:57p jtna
 * SW7405-4702: NEXUS_VideoDecoder_CreateStripedSurface() returns NULL on
 *  repeat surfaces
 *
 * 13   7/30/10 1:37p erickson
 * SW7400-2842: revise negative-logic BCHP_CHIP list
 *
 * 12   7/7/10 11:45a mward
 * SW7400-2842: Add an option NEXUS_VIDEO_DECODER_SINGLEDECODE=y for XVD
 *  singledecode on 97400.
 *
 * 11   6/29/10 2:27p jtna
 * SW7405-3998: update NEXUS_VideoDecoder_Get3DTVStatus to return codec-
 *  generic 3DTV status
 *
 * 10   6/23/10 12:11p erickson
 * SW7550-453: refactor NEXUS_StripedSurface to use
 *  nexus_striped_surface.h API
 *
 * 9   5/12/10 12:29p jtna
 * SW7405-3998: merge
 *
 * SW7405-3998/2   5/12/10 12:18p jtna
 * SW7405-3998: update SEI function/variable naming
 *
 * SW7405-3998/1   5/4/10 5:14p jtna
 * SW7405-3998: added SEI frame packing arrangement message callback
 *
 * 8   7/31/09 4:51p gmohile
 * PR 56512 : Add 7401/03 support
 *
 * 7   1/16/09 1:41p erickson
 * PR50905: added NEXUS_VideoDecoder_GetMostRecentPts
 *
 * 6   10/20/08 9:56a erickson
 * PR46527: add NEXUS_VideoDecoder_CreateStripedMosaicSurfaces and related
 *  code
 *
 * 5   9/8/08 3:03p erickson
 * PR46527: intial checkin of test code to route from AVD->M2MC->GFD
 *
 * 4   7/28/08 11:04a jtna
 * PR45093: Move ChannelChangeReport() from public API to private
 *
 * 3   7/11/08 5:07p jtna
 * PR42924: Add channel change measurements functionality
 *
 * 2   7/7/08 4:46p erickson
 * PR42924: added NEXUS_VideoDecoder_ChannelChangeReport for test
 *
 * 1   5/20/08 3:42p erickson
 * PR42128: added dataReadyCallback to ExtendedSettings
 *
 **************************************************************************/

#include "nexus_video_decoder_module.h"
#include "priv/nexus_core.h"
#include "priv/nexus_rave_priv.h"

BDBG_MODULE(nexus_video_decoder_extra);

NEXUS_Error NEXUS_VideoDecoder_P_GetExtendedStatus_Avd( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderExtendedStatus *pStatus )
{
    NEXUS_Error rc;
    NEXUS_RaveStatus raveStatus;
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    BDBG_CASSERT(BAVC_Polarity_eFrame == (BAVC_Polarity)NEXUS_PicturePolarity_eFrame);
    pStatus->interruptPolarity = videoDecoder->last_field.eInterruptPolarity;
    pStatus->sourcePolarity = videoDecoder->last_field.eSourcePolarity;

    pStatus->isMpeg1 = videoDecoder->last_field.eMpegType == BAVC_MpegType_eMpeg1;
    pStatus->isYCbCr422 = videoDecoder->last_field.eYCbCrType == BAVC_YCbCrType_e4_2_2;
    pStatus->fieldChrominanceInterpolationMode = videoDecoder->last_field.eChrominanceInterpolationMode == BAVC_InterpolationMode_eField;
    pStatus->adjQp = videoDecoder->last_field.ulAdjQp;
    pStatus->pictureRepeatFlag = videoDecoder->last_field.bPictureRepeatFlag;
    pStatus->luminanceNMBY = videoDecoder->last_field.ulLuminanceNMBY;
    pStatus->chrominanceNMBY = videoDecoder->last_field.ulChrominanceNMBY;
    switch (videoDecoder->last_field.eStripeWidth) {
    default:
    case BAVC_StripeWidth_e64Byte: pStatus->stripeWidth = 64; break;
    case BAVC_StripeWidth_e128Byte: pStatus->stripeWidth = 128; break;
    case BAVC_StripeWidth_e256Byte: pStatus->stripeWidth = 256; break;
    }
    pStatus->captureCrc = videoDecoder->last_field.bCaptureCrc;
    pStatus->idrPicID = videoDecoder->last_field.ulIdrPicID;
    pStatus->picOrderCnt = videoDecoder->last_field.int32_PicOrderCnt;
    pStatus->sourceClipTop = videoDecoder->last_field.ulSourceClipTop;
    pStatus->sourceClipLeft = videoDecoder->last_field.ulSourceClipLeft;
    pStatus->lumaRangeRemapping = videoDecoder->last_field.ulLumaRangeRemapping;
    pStatus->chromaRangeRemapping = videoDecoder->last_field.ulChromaRangeRemapping;
    pStatus->dataReadyCount = videoDecoder->dataReadyCount;

    pStatus->rave.mainIndex = -1;
    pStatus->rave.mainSwRaveIndex = -1;
    pStatus->rave.enhancementIndex = -1;
    pStatus->rave.enhancementSwRaveIndex = -1;
    LOCK_TRANSPORT();
    if (videoDecoder->rave) {
        rc = NEXUS_Rave_GetStatus_priv(videoDecoder->rave, &raveStatus);
        if (!rc) {
            pStatus->rave.mainIndex = raveStatus.index;
            pStatus->rave.mainSwRaveIndex = raveStatus.swRaveIndex;
        }
    }
    if (videoDecoder->enhancementRave) {
        rc = NEXUS_Rave_GetStatus_priv(videoDecoder->enhancementRave, &raveStatus);
        if (!rc) {
            pStatus->rave.enhancementIndex = raveStatus.index;
            pStatus->rave.enhancementSwRaveIndex = raveStatus.swRaveIndex;
        }
    }
    UNLOCK_TRANSPORT();

    return 0;
}

void NEXUS_VideoDecoder_P_GetExtendedSettings_Avd( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderExtendedSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    *pSettings = videoDecoder->extendedSettings;
}

NEXUS_Error NEXUS_VideoDecoder_P_SetExtendedSettings_Avd( NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderExtendedSettings *pSettings )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    bool setUserdata = false;
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    if (pSettings->s3DTVStatusEnabled != videoDecoder->extendedSettings.s3DTVStatusEnabled) {
        setUserdata = true;
    }
    videoDecoder->extendedSettings = *pSettings;
    NEXUS_IsrCallback_Set(videoDecoder->dataReadyCallback, &pSettings->dataReadyCallback);
    NEXUS_IsrCallback_Set(videoDecoder->s3DTVChangedCallback, &pSettings->s3DTVStatusChanged);

    rc = NEXUS_VideoDecoder_P_SetCrcFifoSize(videoDecoder, false);
    if (rc) return BERR_TRACE(rc);
        
    if (setUserdata) {
        rc = NEXUS_VideoDecoder_P_SetUserdata(videoDecoder);
        if (rc) return BERR_TRACE(rc);
    }

    rc = NEXUS_VideoDecoder_P_SetLowLatencySettings(videoDecoder);
    if (rc) return BERR_TRACE(rc);
    
    return 0;
}

NEXUS_Error NEXUS_VideoDecoder_Get3DTVStatus( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoder3DTVStatus *pStatus )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    *pStatus = videoDecoder->s3DTVStatus;
    return 0;
}

static NEXUS_StripedSurfaceHandle NEXUS_VideoDecoder_CreateStripedSurface_priv( NEXUS_VideoDecoderHandle videoDecoder, const BAVC_MFD_Picture *pPicture )
{
    NEXUS_StripedSurfaceCreateSettings createSettings;

    if( pPicture->bMute )
        return NULL;
    if( (!pPicture->ulDisplayHorizontalSize) || (!pPicture->ulDisplayVerticalSize) )
        return NULL;

    if (pPicture->bPictureRepeatFlag || !videoDecoder->last_field_new) {
        return NULL;
    }
    videoDecoder->last_field_new = false;
    /* TODO: for progressive encoded pictures, we could reduce the number of blits by overriding the CDT interlaced scanout */

    NEXUS_StripedSurface_GetDefaultCreateSettings(&createSettings);

    createSettings.heap = NEXUS_Heap_GetHeapHandle(videoDecoder->device->mem);
    createSettings.picMemIndex = 0; /* TODO: store index used in NEXUS_VideoDecoderModule_Init */

    createSettings.imageWidth = pPicture->ulDisplayHorizontalSize;
    createSettings.imageHeight = pPicture->ulDisplayVerticalSize;

    createSettings.pLumaBuffer = pPicture->pLuminanceFrameBufferAddress;
    createSettings.pChromaBuffer = pPicture->pChrominanceFrameBufferAddress;

    switch (pPicture->eStripeWidth) {
    case BAVC_StripeWidth_e64Byte: createSettings.pitch = 64; break;
    case BAVC_StripeWidth_e128Byte: createSettings.pitch = 128; break;
    case BAVC_StripeWidth_e256Byte: createSettings.pitch = 256; break;
    }

    createSettings.stripedWidth = createSettings.pitch;
    createSettings.lumaStripedHeight = pPicture->ulLuminanceNMBY * 16;
    createSettings.chromaStripedHeight = pPicture->ulChrominanceNMBY * 16;

    BDBG_CASSERT(BXVD_BufferType_eMax == (BXVD_BufferType)NEXUS_VideoBufferType_eMax);
    switch (pPicture->eSourcePolarity) {
    case BAVC_Polarity_eFrame: createSettings.bufferType = BXVD_BufferType_eFrame; break;
    case BAVC_Polarity_eTopField: createSettings.bufferType = BXVD_BufferType_eTopField; break;
    case BAVC_Polarity_eBotField: createSettings.bufferType = BXVD_BufferType_eBotField; break;
    }

    return NEXUS_StripedSurface_Create(&createSettings);
}

NEXUS_StripedSurfaceHandle NEXUS_VideoDecoder_P_CreateStripedSurface_Avd( NEXUS_VideoDecoderHandle videoDecoder )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    return NEXUS_VideoDecoder_CreateStripedSurface_priv( videoDecoder, &videoDecoder->last_field );
}

NEXUS_Error NEXUS_VideoDecoder_P_CreateStripedMosaicSurfaces_Avd( NEXUS_VideoDecoderHandle videoDecoder,
    NEXUS_StripedSurfaceHandle *pStripedSurfaces, unsigned surfaceMax, unsigned int *pSurfaceCount )
{
    NEXUS_VideoDecoderHandle v;
    unsigned channelIdx, i;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    /* Coverity: 25911 */
    if (pStripedSurfaces == NULL) {
    	return BERR_TRACE(NEXUS_NOT_INITIALIZED);
    }

    *pSurfaceCount = 0;
    channelIdx = 0, i = 0;

    while ((i<surfaceMax) && (channelIdx<NEXUS_NUM_XVD_CHANNELS)) {
        v = &videoDecoder->device->channel[channelIdx];
        if (v->dec && v->mosaicMode) {
            /* some stripedSurfaces may be NULL, while others may not be. there is no guarantee that
               all mosaic surfaces will have the same bPictureRepeatFlag at the same time */
            pStripedSurfaces[i++] = NEXUS_VideoDecoder_CreateStripedSurface_priv(v, &v->last_field);
        }
        channelIdx++;
    }

    *pSurfaceCount = i;
    return 0;
}

void NEXUS_VideoDecoder_P_DestroyStripedSurface_Avd( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_StripedSurfaceHandle stripedSurface )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    NEXUS_StripedSurface_Destroy(stripedSurface);
}

void NEXUS_VideoDecoder_P_DestroyStripedMosaicSurfaces_Avd( NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_StripedSurfaceHandle *pStripedSurfaces, unsigned int surfaceCount )
{
    unsigned int ii;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_ASSERT(pStripedSurfaces);

    for( ii = 0; ii < surfaceCount; ++ii )
    {
        if( pStripedSurfaces[ii] )
            NEXUS_VideoDecoder_DestroyStripedSurface( videoDecoder, pStripedSurfaces[ii] );
    }
}

NEXUS_Error NEXUS_VideoDecoder_P_GetMostRecentPts_Avd( NEXUS_VideoDecoderHandle videoDecoder, uint32_t *pPts )
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    LOCK_TRANSPORT();
    rc = NEXUS_Rave_GetMostRecentPts_priv(videoDecoder->rave, pPts);
    UNLOCK_TRANSPORT();
    return rc; /* don't use BERR_TRACE. failure can be normal. */
}

void NEXUS_VideoDecoder_P_GetExtendedSettings_NotImplemented( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderExtendedSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BKNI_Memcpy(pSettings, &videoDecoder->extendedSettings, sizeof(*pSettings));
    return;
}

NEXUS_Error NEXUS_VideoDecoder_P_SetExtendedSettings_NotImplemented( NEXUS_VideoDecoderHandle videoDecoder, const NEXUS_VideoDecoderExtendedSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_ASSERT(pSettings);
    if(BKNI_Memcmp(pSettings, &videoDecoder->extendedSettings, sizeof(*pSettings))!=0) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    return NEXUS_SUCCESS;
}


