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
 * $brcm_Workfile: nexus_still_decoder.c $
 * $brcm_Revision: 40 $
 * $brcm_Date: 7/13/12 11:58a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/7400/src/nexus_still_decoder.c $
 * 
 * 40   7/13/12 11:58a erickson
 * SW7552-303: use OS-reported max_dcache_line_size instead of nexus macro
 * 
 * 39   6/19/12 1:05p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 38   6/19/12 1:02p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 37   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 36   3/14/12 12:34p erickson
 * SW7401-4517: remove old code
 * 
 * 35   11/11/11 2:19p mward
 * SW7125-1155:  nexus_stilldecoder_p_openchannel() use default values if
 *  !pSettings.
 * 
 * 34   11/3/11 10:58a erickson
 * SW7346-486: add open/start time codec and buffer control, add
 *  NEXUS_StillDecoder_RequestMemorySize
 *
 * 33   8/25/11 3:29p erickson
 * SW7346-454: add NEXUS_StillDecoderStatus.rave.mainIndex
 *
 * 32   8/8/11 11:30a erickson
 * SW7425-1066: add NEXUS_StillDecoderOpenSettings.heap
 *
 * 31   3/9/11 12:11p erickson
 * SW7425-92: avoid warning for new ITB alignment requirement
 *
 * 30   12/22/10 3:48p erickson
 * SW7125-778: use BXVD_DecodeStillPictureReset which is preferred
 *
 * 29   12/7/10 2:36p jtna
 * SW7408-190: remove NEXUS_PowerManagement_SetCoreState calls. XVD power
 *  management is handled in magnum
 *
 * 28   9/8/10 12:25p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 *
 * 27   6/23/10 12:11p erickson
 * SW7550-453: refactor NEXUS_StripedSurface to use
 *  nexus_striped_surface.h API
 *
 * 26   3/10/10 3:00p erickson
 * SW7405-3965: deprecrate NEXUS_VideoDecoderModuleSettings.numDecodes,
 *  etc. clean up avd/xvd channel/mfd index assignment in
 *  NEXUS_VideoDecoder_P_Open.
 *
 * 25   2/10/10 10:36a erickson
 * SW7405-3891: rename macro to B_USE_NEW_XVD
 *
 * 24   8/10/09 1:57p gmohile
 * PR 56512 : Fix still decoder for non mosaic decode
 *
 * 23   8/7/09 10:17a gmohile
 * PR 57420 : Support still decode for non mosaic apis
 *
 * 22   6/25/09 2:56p erickson
 * PR45902: add nexus-based dynamic power management for video decoder
 *
 * 21   6/9/09 2:24p jtna
 * PR55706: added NEXUS_StillDecoderStatus.avdStatusBlock
 *
 * 20   1/27/09 9:56a erickson
 * PR51468: refactor to reduce exposed globals
 *
 * 19   1/26/09 10:54a erickson
 * PR51468: global variable naming convention
 *
 * 18   12/11/08 1:44p erickson
 * PR50171: fix HD test
 *
 * 17   9/2/08 7:48p katrep
 * PR46457: Configure nexus features at the run time based on board strap
 *  options
 *
 * 16   8/14/08 5:26p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 15   8/7/08 1:26p erickson
 * PR45521: prevent misleading XVD error message if seqend was never found
 *  in ITB
 *
 * 14   7/23/08 10:12a erickson
 * PR45071: fix static buffer overrun
 *
 * 13   6/17/08 7:00a erickson
 * PR43222: add divx support
 *
 * 12   4/11/08 9:53a erickson
 * PR41246: convert BDBG_OBJECT_UNSET to BDBG_OBJECT_DESTROY if freeing
 *  memory
 *
 * 11   3/31/08 12:33p erickson
 * PR41073: check result of malloc and fail graciously
 *
 * 10   3/22/08 1:37a erickson
 * PR40831: call BXVD_StopDecode if still decode timed out
 *
 * 9   3/19/08 4:06p erickson
 * PR40307: fix XVD index
 *
 * 8   3/18/08 4:45p erickson
 * PR40307: missing BKNI_Memset
 *
 * 7   3/8/08 7:08a erickson
 * PR40103: convert to TaskCallback and IsrCallback
 *
 * 6   3/5/08 2:41p erickson
 * PR40307: fix StillDecoder Close. add AVC/VC1 stills.
 *
 * 5   2/26/08 2:42p erickson
 * PR39786: added Stop
 *
 * 4   2/25/08 3:01p erickson
 * PR39786: update
 *
 * 3   2/21/08 1:49p erickson
 * PR39786: added NEXUS_Rave_Disable_priv
 *
 * 2   2/21/08 9:13a erickson
 * PR39786: 7401 compilation
 *
 * 1   2/20/08 4:57p erickson
 * PR39786: initial impl
 *
 **************************************************************************/
#include "nexus_video_decoder_module.h"
#include "nexus_still_decoder_impl.h"
#include "nexus_power_management.h"
#include "priv/nexus_core.h"

BDBG_MODULE(nexus_still_decoder);

/* TODO: move this to nexus_video_decoder.c for general use */
/* convert from nexus types to XVD types */
void NEXUS_VideoDecoder_P_SetXvdChannelCapabilities(BXVD_ChannelSettings *pSettings, unsigned maxCodecEntries, NEXUS_VideoCodec *pSupportedCodecs, unsigned width, unsigned height)
{
    unsigned i;
    unsigned total = 0;

    /* maxCodecEntries is the size of pSettings->peVideoCmprStdList[] */
    for (i=0; i<NEXUS_VideoCodec_eMax && total<maxCodecEntries; i++) {
        if (!pSupportedCodecs[i]) continue;
        pSettings->peVideoCmprStdList[total] = NEXUS_P_VideoCodec_ToMagnum(pSupportedCodecs[i], NEXUS_TransportType_eTs);
        total++;
    }
    pSettings->uiVideoCmprCount = total;

    if (height == 0 || width == 0 || height > 1088 || width > 1920 ) {
        BDBG_ERR(("Invalid resolution %dx%d specified", width,height));
        pSettings->eDecodeResolution = BXVD_DecodeResolution_eHD;
    }
    if (height <= 144 && width <= 176) {
        pSettings->eDecodeResolution = BXVD_DecodeResolution_eQCIF;
    }
    else if (height <= 288 && width <= 352) {
        pSettings->eDecodeResolution = BXVD_DecodeResolution_eCIF;
    }
    else if (height <= 576 && width <= 720) {
        pSettings->eDecodeResolution = BXVD_DecodeResolution_eSD;
    }
    else {
        pSettings->eDecodeResolution = BXVD_DecodeResolution_eHD;
    }
}

void NEXUS_StillDecoder_GetDefaultOpenSettings( NEXUS_StillDecoderOpenSettings *pSettings )
{
    NEXUS_VideoFormatInfo info;
    NEXUS_VideoCodec total, i;

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->fifoSize = 1 * 1024 * 1024;

    NEXUS_VideoFormat_GetInfo(g_NEXUS_videoDecoderModuleSettings.maxStillDecodeFormat, &info);
    pSettings->maxWidth = info.width;
    pSettings->maxHeight = info.height;
    
    /* copy over default codec support appropriate to stilldecoder */
    for (total=0,i=0;i<NEXUS_VideoCodec_eMax;i++) {
        switch (g_NEXUS_videoDecoderModuleSettings.supportedCodecs[i]) {
        case NEXUS_VideoCodec_eNone:
        case NEXUS_VideoCodec_eH264_Svc: /* not supported by stilldecoder */
        case NEXUS_VideoCodec_eH264_Mvc: /* not supported by stilldecoder */
            break;
        default:    
            pSettings->supportedCodecs[total++] = g_NEXUS_videoDecoderModuleSettings.supportedCodecs[i];
            break;
        }
    }
}

#define STILL_DECODER_INDEX(stillDecoder) (NEXUS_NUM_VIDEO_DECODERS + (stillDecoder)->index)

static NEXUS_Error nexus_stilldecoder_p_openchannel(NEXUS_StillDecoderHandle stillDecoder, const NEXUS_StillDecoderStartSettings *pSettings)
{
    NEXUS_Error rc;
    unsigned index;
    BXVD_ChannelSettings xvdChannelSettings;
    BAVC_VideoCompressionStd stVideoCompressionList[6];

    index = STILL_DECODER_INDEX(stillDecoder);

    rc = BXVD_GetChannelDefaultSettings(stillDecoder->parentDecoder->device->xvd, index, &xvdChannelSettings);
    if (rc) return BERR_TRACE(rc);

    xvdChannelSettings.peVideoCmprStdList = stVideoCompressionList;
    if (stillDecoder->buffer.heap && pSettings) {
        NEXUS_VideoCodec supportedCodecs[NEXUS_VideoCodec_eMax];
        BKNI_Memset(supportedCodecs, 0, sizeof(supportedCodecs));
        supportedCodecs[0] = pSettings->codec;
        NEXUS_VideoDecoder_P_SetXvdChannelCapabilities(&xvdChannelSettings, 6, supportedCodecs, pSettings->output.maxWidth, pSettings->output.maxHeight);
    }
    else {
        NEXUS_VideoDecoder_P_SetXvdChannelCapabilities(&xvdChannelSettings, 6, stillDecoder->openSettings.supportedCodecs, stillDecoder->openSettings.maxWidth, stillDecoder->openSettings.maxHeight);
    }
    xvdChannelSettings.eChannelMode = BXVD_ChannelMode_eStill;

    if (stillDecoder->buffer.heap) {
        xvdChannelSettings.hChannelPictureHeap = stillDecoder->buffer.heap;
    }
    else if (stillDecoder->openSettings.heap) {
        xvdChannelSettings.hChannelPictureHeap = NEXUS_Heap_GetMemHandle(stillDecoder->openSettings.heap);
    }
    /* else, xvd will use its default heap */

    BDBG_MSG(("creating xvd channel %d for still decoder %d", index, stillDecoder->index));
    /* The index of the still channel is one more than the regular. I don't believe adding index but it's an added check that index == 0. */
    rc = BXVD_OpenChannel(stillDecoder->parentDecoder->device->xvd, &stillDecoder->xvdChannel, index, &xvdChannelSettings);
    if (rc) return BERR_TRACE(rc);

    return 0;
}

static void nexus_stilldecoder_p_closechannel(NEXUS_StillDecoderHandle stillDecoder)
{
    BDBG_ASSERT(stillDecoder->xvdChannel);
    BDBG_MSG(("closing xvd channel for still decoder %d", stillDecoder->index));
    BXVD_CloseChannel(stillDecoder->xvdChannel);
    stillDecoder->xvdChannel = NULL;
}

/* create heap for NEXUS_StillDecoderStartSettings.buffer */
static NEXUS_Error nexus_stilldecoder_p_outputheap(NEXUS_StillDecoderHandle stillDecoder, const NEXUS_StillDecoderStartSettings *pSettings)
{
    NEXUS_Error rc;
    void *buffer = pSettings?pSettings->output.buffer:NULL;
    unsigned bufferSize = pSettings?pSettings->output.size:0;
    unsigned maxHeight = pSettings?pSettings->output.maxHeight:0;

    if (buffer && !bufferSize) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
    if (!buffer && bufferSize) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    /* if no change needed, short circuit */
    if ((!buffer && !stillDecoder->buffer.heap) ||
        (buffer == stillDecoder->buffer.mem && bufferSize == stillDecoder->buffer.size && maxHeight == stillDecoder->buffer.maxHeight)) {
        /* Knowing the height is necessary in the following scenario:
           User supplied buffer/size that is big enough for HD but a WxH that is SD.
           We open XVD in SD mode. Next call uses the same buffer/size but is for
           HD decode. XVD must be closed/re-opened in HD mode.
        */
        return 0;
    }

    if (stillDecoder->xvdChannel) {
        nexus_stilldecoder_p_closechannel(stillDecoder);
    }

    /* free an existing heap if different buffer or size */
    if (buffer && stillDecoder->buffer.heap) {
        BDBG_MSG(("destroying user buffer heap %p", stillDecoder->buffer.heap));
        BMEM_Heap_Destroy(stillDecoder->buffer.heap);
        stillDecoder->buffer.heap = NULL;
    }

    if (buffer && !stillDecoder->buffer.heap) {
        unsigned offset;
        BMEM_Heap_Settings mem_heap_settings;
        void *uncachedBuffer;
        BMEM_HeapInfo info;
        
        offset = NEXUS_AddrToOffset(buffer);
        if (!offset) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }

        /* must flush before HW write because there could be stale data in the cache */
        NEXUS_FlushCache(buffer, bufferSize);

        uncachedBuffer = NEXUS_OffsetToUncachedAddr(offset);
        if (!uncachedBuffer) {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
        
        /* use alignment from heap[0]. this will give minimum required dcache alignment */
        BMEM_Heap_GetInfo(g_pCoreHandles->heap[0], &info);

        /* can't afford any overhead in the heap, so eSystem for OS metadata and eFastest for no guard banding. */
        (void)BMEM_Heap_GetDefaultSettings(&mem_heap_settings);
        mem_heap_settings.eSafetyConfig = BMEM_SafetyConfig_eFastest;
        mem_heap_settings.eBookKeeping = BMEM_BookKeeping_eSystem;
        mem_heap_settings.uiAlignment = info.uiAlignment;
        mem_heap_settings.pCachedAddress = buffer;
        mem_heap_settings.flush = NEXUS_FlushCache;
        mem_heap_settings.flush_isr = NEXUS_FlushCache_isr;
        rc = BMEM_Heap_Create(g_pCoreHandles->mem, uncachedBuffer, offset, bufferSize, &mem_heap_settings, &stillDecoder->buffer.heap);
        if (rc) return BERR_TRACE(rc);
        BDBG_ASSERT(stillDecoder->buffer.heap);
        stillDecoder->buffer.mem = buffer;
        stillDecoder->buffer.size = bufferSize;
        stillDecoder->buffer.maxHeight = maxHeight;
        BDBG_MSG(("creating heap %p for user buffer %p %d", stillDecoder->buffer.heap, stillDecoder->buffer.mem, stillDecoder->buffer.size));
    }

    /* stillDecoder->rave is NULL, we are closing */
    if (stillDecoder->rave) {
        rc = nexus_stilldecoder_p_openchannel(stillDecoder, pSettings);
        if (rc) return BERR_TRACE(rc);
    }

    return 0;
}

NEXUS_StillDecoderHandle NEXUS_StillDecoder_P_Open_Avd( NEXUS_VideoDecoderHandle parentDecoder, unsigned index, const NEXUS_StillDecoderOpenSettings *pSettings )
{
    NEXUS_StillDecoderHandle stillDecoder;
    NEXUS_StillDecoderOpenSettings defaultSettings;
    NEXUS_RaveOpenSettings raveSettings;
    BERR_Code rc;
    int32_t pic_buf_length;

    if (!pSettings) {
        NEXUS_StillDecoder_GetDefaultOpenSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    stillDecoder = BKNI_Malloc(sizeof(*stillDecoder));
    if (!stillDecoder) {
        rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_StillDecoder, stillDecoder);
    stillDecoder->interface = &NEXUS_StillDecoder_P_Interface_Avd;
    stillDecoder->openSettings = *pSettings;
    stillDecoder->parentDecoder = parentDecoder;
    stillDecoder->index = index;

    stillDecoder->endCodeFoundCallback = NEXUS_TaskCallback_Create(stillDecoder, NULL);
    stillDecoder->stillPictureReadyCallback = NEXUS_IsrCallback_Create(stillDecoder, NULL);

    NEXUS_StripedSurface_GetDefaultCreateSettings(&stillDecoder->stripedSurface.createSettings);

    if (pSettings->maxWidth && pSettings->maxHeight) {
        rc = nexus_stilldecoder_p_openchannel(stillDecoder, NULL);
        if (rc) {rc = BERR_TRACE(rc); goto error;}
    }

    LOCK_TRANSPORT();
    NEXUS_Rave_GetDefaultOpenSettings_priv(&raveSettings);
    rc = BXVD_GetBufferConfig(stillDecoder->parentDecoder->device->xvd, &raveSettings.config, &pic_buf_length);
    raveSettings.config.Cdb.Length = pSettings->fifoSize;
    raveSettings.config.Itb.Length = pSettings->fifoSize / 10;
    if (raveSettings.config.Itb.Length % 16) {
        raveSettings.config.Itb.Length -= raveSettings.config.Itb.Length % 16;
    }
    raveSettings.allocSoftRave = true;
    stillDecoder->rave = NEXUS_Rave_Open_priv(&raveSettings);
    UNLOCK_TRANSPORT();
    if (rc || !stillDecoder->rave) {rc = BERR_TRACE(NEXUS_UNKNOWN); goto error;}

    /* TODO: per device, not per channel */
    rc = BXVD_InstallDeviceInterruptCallback(stillDecoder->parentDecoder->device->xvd, BXVD_DeviceInterrupt_eDecodedStillBufferReady, NEXUS_VideoDecoder_P_StillReady_isr, stillDecoder, 0);
    if (rc) {rc=BERR_TRACE(rc); goto error;}

    return stillDecoder;

error:
    NEXUS_StillDecoder_Close(stillDecoder);
    return NULL;
}

void NEXUS_StillDecoder_P_Close_Avd( NEXUS_StillDecoderHandle stillDecoder )
{
    /* just in case there's a timer firing */
    NEXUS_StillDecoder_Stop(stillDecoder);

    BXVD_UnInstallDeviceInterruptCallback(stillDecoder->parentDecoder->device->xvd, BXVD_DeviceInterrupt_eDecodedStillBufferReady);
    if (stillDecoder->rave) {
        LOCK_TRANSPORT();
        NEXUS_Rave_Close_priv(stillDecoder->rave);
        UNLOCK_TRANSPORT();
        stillDecoder->rave = NULL;
    }
    if (stillDecoder->xvdChannel) {
        nexus_stilldecoder_p_closechannel(stillDecoder);
    }
    (void)nexus_stilldecoder_p_outputheap(stillDecoder, NULL);

    if (stillDecoder->stripedSurface.handle) {
        NEXUS_StripedSurface_Destroy(stillDecoder->stripedSurface.handle);
        stillDecoder->stripedSurface.handle = NULL;
    }

    NEXUS_TaskCallback_Destroy(stillDecoder->endCodeFoundCallback);
    NEXUS_IsrCallback_Destroy(stillDecoder->stillPictureReadyCallback);

    BDBG_OBJECT_DESTROY(stillDecoder, NEXUS_StillDecoder);
    BKNI_Free(stillDecoder);
}

void NEXUS_StillDecoder_GetDefaultStartSettings( NEXUS_StillDecoderStartSettings *pSettings )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_Error NEXUS_StillDecoder_P_Start_Avd( NEXUS_StillDecoderHandle stillDecoder, const NEXUS_StillDecoderStartSettings *pSettings )
{
    NEXUS_RaveSettings raveSettings;
    NEXUS_PidChannelStatus pidChannelStatus;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(stillDecoder, NEXUS_StillDecoder);

    if (stillDecoder->started) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    /* Start not valid with NULL settings */
    if (!pSettings) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    rc = nexus_stilldecoder_p_outputheap(stillDecoder, pSettings);
    if (rc) {
        return BERR_TRACE(rc);
    }

    NEXUS_PidChannel_GetStatus(pSettings->pidChannel, &pidChannelStatus);

    LOCK_TRANSPORT();
    NEXUS_Rave_Disable_priv(stillDecoder->rave); /* must disable before reconfigure */
    NEXUS_Rave_Flush_priv(stillDecoder->rave);
    NEXUS_Rave_GetDefaultSettings_priv(&raveSettings);
    raveSettings.pidChannel = pSettings->pidChannel;
    raveSettings.bandHold = pidChannelStatus.playback;
    raveSettings.continuityCountEnabled = !pidChannelStatus.playback;
    rc = NEXUS_Rave_ConfigureVideo_priv(stillDecoder->rave, pSettings->codec, &raveSettings);
    NEXUS_Rave_Enable_priv(stillDecoder->rave);
    UNLOCK_TRANSPORT();
    if (rc) {return BERR_TRACE(rc);}

    NEXUS_PidChannel_ConsumerStarted(pSettings->pidChannel); /* needed to unpause playback & stuff like that */

    stillDecoder->settings = *pSettings;
    stillDecoder->status.endCodeFound = false;
    stillDecoder->status.stillPictureReady = false;
    if (stillDecoder->stripedSurface.handle) {
        NEXUS_StripedSurface_Destroy(stillDecoder->stripedSurface.handle);
        stillDecoder->stripedSurface.handle = NULL;
    }

    NEXUS_TaskCallback_Set(stillDecoder->endCodeFoundCallback, &pSettings->endCodeFound);
    NEXUS_IsrCallback_Set(stillDecoder->stillPictureReadyCallback, &pSettings->stillPictureReady);

    NEXUS_Time_Get(&stillDecoder->startTime);

    /* start to poll on the ITB, looking for the endCode */
    if (!stillDecoder->timer) {
        stillDecoder->timer = NEXUS_ScheduleTimer(10, NEXUS_StillDecoder_P_CheckForEndCode, stillDecoder);
    }

    stillDecoder->started = true;

    return 0;
}

void NEXUS_StillDecoder_P_Stop_Avd(NEXUS_StillDecoderHandle stillDecoder)
{
    bool never_started = false;
    BDBG_OBJECT_ASSERT(stillDecoder, NEXUS_StillDecoder);

    if (stillDecoder->timer) {
        NEXUS_CancelTimer(stillDecoder->timer);
        stillDecoder->timer = NULL;
        never_started = true;
    }

    if (stillDecoder->started) {
        LOCK_TRANSPORT();
        NEXUS_Rave_Disable_priv(stillDecoder->rave);
        NEXUS_Rave_Flush_priv(stillDecoder->rave);
        NEXUS_Rave_RemovePidChannel_priv(stillDecoder->rave);
        UNLOCK_TRANSPORT();

        /* if we never received a still, we should reset the XVD channel */
        if (!stillDecoder->stripedSurface.handle && !never_started) {
            BXVD_DecodeStillPictureReset(stillDecoder->xvdChannel);
        }
        stillDecoder->started = false;
    }
}

NEXUS_Error NEXUS_StillDecoder_P_GetStatus_Avd( NEXUS_StillDecoderHandle stillDecoder, NEXUS_StillDecoderStatus *pStatus )
{
    BERR_Code rc;
    BXVD_ChannelStatus channelStatus;

    BDBG_OBJECT_ASSERT(stillDecoder, NEXUS_StillDecoder);
    *pStatus = stillDecoder->status;

    if (stillDecoder->xvdChannel) {
        rc = BXVD_GetChannelStatus(stillDecoder->xvdChannel, &channelStatus);
        if (rc) {return BERR_TRACE(rc);}
        pStatus->avdStatusBlock = channelStatus.uiAVDStatusBlock;
    }

    pStatus->rave.mainIndex = -1;
    pStatus->rave.mainSwRaveIndex = -1;
    if (stillDecoder->rave) {
        NEXUS_RaveStatus raveStatus;
        LOCK_TRANSPORT();
        rc = NEXUS_Rave_GetStatus_priv(stillDecoder->rave, &raveStatus);
        UNLOCK_TRANSPORT();
        if (!rc) {
            pStatus->rave.mainIndex = raveStatus.index;
            pStatus->rave.mainSwRaveIndex = raveStatus.swRaveIndex;
        }
    }


    return 0;
}

NEXUS_Error NEXUS_StillDecoder_P_GetStripedSurface_Avd( NEXUS_StillDecoderHandle stillDecoder, NEXUS_StripedSurfaceHandle *pStripedSurface )
{
    BDBG_OBJECT_ASSERT(stillDecoder, NEXUS_StillDecoder);
    if (stillDecoder->status.stillPictureReady) {
        if (stillDecoder->stripedSurface.recreate) {
            NEXUS_StripedSurfaceCreateSettings createSettings;
            BMEM_Heap_Handle mem;

            if (stillDecoder->stripedSurface.handle) {
                NEXUS_StripedSurface_Destroy(stillDecoder->stripedSurface.handle);
                stillDecoder->stripedSurface.handle = NULL;
            }

            /* get data out of critical section */
            BKNI_EnterCriticalSection();
            createSettings = stillDecoder->stripedSurface.createSettings;
            mem = stillDecoder->stripedSurface.mem;
            stillDecoder->stripedSurface.recreate = false;
            BKNI_LeaveCriticalSection();

            createSettings.heap = NEXUS_Heap_GetHeapHandle(mem);
            stillDecoder->stripedSurface.handle = NEXUS_StripedSurface_Create(&createSettings);
        }

        *pStripedSurface = stillDecoder->stripedSurface.handle;
        return 0;
    }
    return BERR_TRACE(NEXUS_UNKNOWN);
}

void NEXUS_StillDecoder_GetDefaultMemoryRequest( NEXUS_StillDecoderMemoryRequest *pRequest )
{
    BKNI_Memset(pRequest, 0, sizeof(*pRequest));
    pRequest->width = 720;
    pRequest->height = 480;
    pRequest->codec = NEXUS_VideoCodec_eMpeg2;
}

NEXUS_Error NEXUS_StillDecoder_RequestMemorySize( NEXUS_StillDecoderHandle stillDecoder, const NEXUS_StillDecoderMemoryRequest *pRequest, unsigned *pSize )
{
    int rc;
    BXVD_ChannelSettings settings;
    BXVD_FWMemConfig config;
    BAVC_VideoCompressionStd stVideoCompressionList[1];

    BDBG_OBJECT_ASSERT(stillDecoder, NEXUS_StillDecoder);

    rc = BXVD_GetChannelDefaultSettings(stillDecoder->parentDecoder->device->xvd, STILL_DECODER_INDEX(stillDecoder), &settings);
    if (rc) return BERR_TRACE(rc);

    if (pRequest->width > 720 || pRequest->height > 480) {
        settings.eDecodeResolution = BXVD_DecodeResolution_eHD;
    }
    else {
        settings.eDecodeResolution = BXVD_DecodeResolution_eSD;
    }
    /* hardcode NEXUS_TransportType_eTs because only DSS matters */
    stVideoCompressionList[0] = NEXUS_P_VideoCodec_ToMagnum(pRequest->codec, NEXUS_TransportType_eTs);
    settings.peVideoCmprStdList = stVideoCompressionList;
    settings.uiVideoCmprCount = 1;
    settings.eChannelMode = BXVD_ChannelMode_eStill;

    rc = BXVD_GetChannelMemoryConfig(stillDecoder->parentDecoder->device->xvd, &settings, &config);
    if (rc) return BERR_TRACE(rc);

    *pSize = config.uiPictureHeapSize;
    return 0;
}

