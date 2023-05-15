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
 * $brcm_Workfile: nexus_picture_decoder.c $
 * $brcm_Revision: 51 $
 * $brcm_Date: 8/20/12 4:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/picture_decoder/src/nexus_picture_decoder.c $
 * 
 * 51   8/20/12 4:44p ahulse
 * SW7425-3775: Segmented output decode of rotated or multiscan jpeg not
 *  supported
 * 
 * 50   8/9/12 5:27p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 49   7/31/12 5:01p jtna
 * SW7425-2449: push power management down to BSID
 * 
 * 48   7/9/12 1:58p erickson
 * SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
 * 
 * 47   6/20/12 10:15a ahulse
 * SW7346-859: fix coverity, honor NEXUS_ANY_ID, reject index exceeding hw
 *  picture decoders
 * 
 * 46   6/19/12 12:43p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 45   5/15/12 10:30a ahulse
 * SW7425-3055: Coverity Defect ID:37729 MISSING_BREAK
 * 
 * 44   4/20/12 11:06a ahulse
 * SW7425-1619: deprecate Rle support
 * 
 * 43   4/16/12 5:42p ahulse
 * SW7425-1619,SW7231-771,SW7435-110: fix index,coverity issues 36967 &
 *  37275
 * 
 * 42   3/20/12 9:16a ahulse
 * SW7425-1382,SW7425-2547: fix coverity issue 36605, 36964
 * 
 * 41   3/5/12 1:06p ahulse
 * SW7425-2523: Set lastdma when buffer holds pic
 * 
 * 40   2/28/12 1:25p gmohile
 * SW7425-2401 : Picture decoder standby support
 * 
 * 39   2/27/12 2:28p ahulse
 * SW7425-1619: Limit open decoders to available SID channels
 * 
 * 38   2/27/12 12:00p ahulse
 * SW7425-2112: fix structure types, immediate stop
 * 
 * 37   2/21/12 5:59p gmohile
 * SW7425-2401 : Add SID SRAM power management
 * 
 * 36   2/21/12 3:14p ahulse
 * SW7425-2433: fix state race and sidch err
 * 
 * 35   2/21/12 11:15a ahulse
 * SW7425-2112: merge SID mjpeg changes to main
 *
 * SW7425-2112/2   2/16/12 4:52p fbasso
 * SW7425-2112: updated nexus picture decoder to reflect changes to
 *  bsid.h.
 *
 * SW7425-2112/1   2/10/12 10:51a fbasso
 * SW7425-2112: SID PI integration for motion jpeg video decode.
 * 
 * 34   2/15/12 4:11p gmohile
 * SW7425-2401 : Enable SID power before open channel
 * 
 * 33   1/26/12 2:27p vsilyaev
 * SW7425-2258: Moved NEXUS_PictureDecoder_P_HWState into the external
 *  symbol
 * 
 * 32   1/24/12 2:44p vsilyaev
 * SW7425-2258: Added Nexus video decoder extension to decode MJPEG
 * 
 * 31   1/20/12 4:42p ahulse
 * SW7425-1619: merge SID multi ch support into nexus mainline
 * 
 * SW7425-1619/4   1/12/12 1:48p ahulse
 * SW7425-1619: Allow open picture decodes to exceed SID channels
 * 
 * SW7425-1619/3   12/29/11 3:16p ahulse
 * SW7425-1619: Fix segmented output decode - preserve active across
 *  segmented decodes
 * 
 * SW7425-1619/2   12/28/11 5:54p ahulse
 * SW7425-1619: updated to work with latest SID multi ch fw
 * 
 * SW7425-1619/1   12/9/11 2:48p ahulse
 * SW7425-1619: changes to start using new SID fw
 * 
 * 30   12/21/11 2:32p erickson
 * SW7425-1795: SID buffers must be cpu accessible in the driver
 * 
 * 28   10/27/11 3:09p gmohile
 * SW7231-426 : Acquire SID power before sid close
 * 
 * 27   9/26/11 9:34a erickson
 * SW7425-948: add temporary virtualization with single context SID. based
 *  on implicit acquire on first stateful call, release on stop.
 *
 * 26   9/8/11 4:27p erickson
 * SW7425-948: clarify and add TODO's for future refactoring
 *
 * 25   8/25/11 2:37p erickson
 * SWDTV-8264: add transparency status to NEXUS_PictureDecoderStatus
 *
 * 24   7/25/11 2:29p ahulse
 * SW7422-256,SWDTV-7693: Set correct size when decoding header
 *
 * 23   6/24/11 11:56a ahulse
 * SW7422-256: merge branch to mainline to use new SID f/w
 *
 * SW7420-256/3   6/2/11 10:24a ahulse
 * SW7422-256,SWDTV-7290,SWDTV-7326:
 *
 * SW7420-256/2   5/26/11 2:41p ahulse
 * SW7422-256: Use updated SID fw using a divided CDB
 *
 * SW7420-256/1   3/7/11 3:54p ahulse
 * SW7422-256: Enable decode of jpeg image with a buffer (compressed data
 *  buffer ) that is smaller than jpeg image filesize
 *
 * 22   11/11/10 6:00p vsilyaev
 * SW7405-4995: Use BKNI_Memcmp to preinitialize structure used in
 *  GetDefaultSettings
 *
 * 21   8/9/10 11:09a vsilyaev
 * SW3548-126: Promoted error handling into the separate function
 *
 * 20   8/6/10 4:36p vsilyaev
 * SW3548-126: If decoding failed print information known about decoded
 *  picture
 *
 * 19   5/4/10 1:59p vsilyaev
 * SW3548-2919: Added configuration for the user supplied buffers
 *
 * 18   12/18/09 6:51p mphillip
 * SW3548-2663: Allow user to configure heap used by picture decoder
 *
 * 17   11/2/09 4:45p erickson
 * SW3548-2579: add required connection for mpeg
 *
 * 16   11/2/09 2:50p erickson
 * SW3548-2579: add initial MPEG support
 *
 * 15   6/25/09 10:22a erickson
 * PR55968: add power management
 *
 * 14   12/30/08 11:44a jgarrett
 * PR 50001: Adding NEXUS_NUM_PICTURE_DECODERS for apps to select SW vs HW
 *  decoding
 *
 * 13   10/10/08 9:36a vsilyaev
 * PR46142: Fix bug that cause multiScanBuffer is not free.
 *
 * PR46142/1   10/10/08 4:24p chengs
 * PR46142: Fix bug that cause multiScanBuffer is not free.
 *
 * 12   10/8/08 1:01p vsilyaev
 * PR 46142: Added 'rotated' bit into the picture header
 *
 * 11   9/30/08 4:14p vsilyaev
 * PR 46142: Use BSID_DecodeStillImage if destination surface matches
 *  image surface
 *
 * 10   9/12/08 8:27a vsilyaev
 * PR 46142: Removed deprecated option
 *
 * 9   9/8/08 4:10p vsilyaev
 * PR 46142: Added option to specify extra buffer for the multi scan
 *  decode
 *
 * 8   8/5/08 12:55p erickson
 * PR45361: fix c89 build
 *
 * 7   6/19/08 5:39p vsilyaev
 * PR 40870: Fixed segmented decode
 *
 * PR40395/1   6/20/08 7:32a chengs
 * PR40395: Using HW JPEG decode.
 *
 * 6   6/13/08 2:07p erickson
 * PR40870: fix memset, move reset
 *
 * 5   6/12/08 7:01p vsilyaev
 * PR 40670: Fixed to work on a real HW
 *
 * 4   4/29/08 3:40p vsilyaev
 * PR 40870: Improved description
 *
 * 3   4/16/08 11:52a vsilyaev
 * PR 40870: Improved implementation
 *
 * 2   4/11/08 7:11p vsilyaev
 * PR 40870: Added stub implementation
 *
 * 1   4/11/08 5:51p vsilyaev
 * PR 40870: Hardware JPEG decoder
 *
 **************************************************************************/
#include "nexus_picture_decoder_module.h"
#include "nexus_picture_decoder_init.h"
#include "nexus_power_management.h"
#include "priv/nexus_core.h"
#include "nexus_client_resources.h"

BDBG_MODULE(nexus_picture_decoder);
#define BDBG_MSG_TRACE(x) /* BDBG_MSG(x) */

#define BUF_DIV_CHUNKS      2                  /* Number of chunks in which to divide up user buffer passed in */

typedef enum {
  ebufFree,
  ebufAllocd,
  ebufFilled
} sidBuf_eState;


typedef struct NEXUS_PictureDecoder_P_SidQue {
          BSID_DmaChunkInfo      chunkInfo[BUF_DIV_CHUNKS];
          int                    stage[BUF_DIV_CHUNKS];     /* empty,load,readC,procd */
} NEXUS_PictureDecoder_P_SidQue;

struct NEXUS_PictureDecoder {
    NEXUS_OBJECT(NEXUS_PictureDecoder);
    BSID_ChannelHandle  sid; 
    int index;
    void *fifo; /* cached pointer */
    unsigned wr_offset;
    enum {
        NEXUS_PictureDecoder_P_eStopped,
        NEXUS_PictureDecoder_P_eWaitingData,
        NEXUS_PictureDecoder_P_eStreamInfo,       /* BSID_GetStreamInfo called */
        NEXUS_PictureDecoder_P_eStreamInfoDone ,  /* BSID_GetStreamInfo completed */
        NEXUS_PictureDecoder_P_eDecoding,
        NEXUS_PictureDecoder_P_eSegmentDone,
        NEXUS_PictureDecoder_P_eFailed,
        NEXUS_PictureDecoder_P_eMoreInfoData,
        NEXUS_PictureDecoder_P_eMoreImageData
    } state;
    bool busy;   /* set to true if waiting for operation to complete */
    bool cancel; /* set to true if want to cancel current operation */
    NEXUS_PictureDecoderOpenSettings openSettings;
    NEXUS_PictureDecoderStartSettings startSettings;
    BKNI_EventHandle event;
    BKNI_EventHandle eventCancel;
    NEXUS_EventCallbackHandle eventHandler;
    NEXUS_TaskCallbackHandle pictureParsedCallback;
    NEXUS_TaskCallbackHandle segmentDecodedCallback;

    struct {
        BSID_LinDataMap dataMap;
        BSID_ImageFormat imageFormat;
        struct {
            BSID_NotificationEvent eventType;
            BSID_ResourceStatus resourceStatus;
        } event;
        NEXUS_PictureDecoderStatus status;
        unsigned nextRow;
        struct {
            NEXUS_Rect rect;
            NEXUS_SurfaceHandle surface;
        } segment;
    } decodeState; /* SID PI could write to user supplied pointers after function have returned */
    void *memFifo; /* pointer from bmem */
    void *multiScanBuffer;
    uint32_t multiScanBufOffset;
    BMEM_Heap_Handle heap;   /* which heap this decoder allocates from */
    unsigned remainingBytes; /* When using small cdb, shows remaining bytes to decode */
    unsigned bufferDivide;
    bool                          bufFlip;
    BSID_OpenChannelSettings      sidChOpenSettings;
    NEXUS_PictureDecoder_P_SidQue sidQue;
    int                           active;        /* which one is currently active */
    int                           chBusy;        /* bufr being processed, set when loaded, zerod after isr received */
                                                 /* Read complete uses it to know when it can start chain again */
    BSID_ImageHeader           imageHeader;
};

NEXUS_PictureDecoder_P_ModuleState g_NEXUS_PictureDecoder_P_ModuleState;

void
NEXUS_PictureDecoderModule_GetDefaultSettings(NEXUS_PictureDecoderModuleSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->surface = NULL;
    return;
}

NEXUS_ModuleHandle
NEXUS_PictureDecoderModule_Init(const NEXUS_PictureDecoderModuleSettings *pSettings)
{
    NEXUS_Error rc;
    NEXUS_ModuleSettings moduleSettings;
    struct NEXUS_PictureDecoder_P_HWState *hwState = &g_NEXUS_PictureDecoder_P_ModuleState.hwState;
    BSID_Settings sidSettings;
    BSID_RevisionInfo st_RevisionInfo;


    BDBG_ASSERT(pSettings);
    BDBG_ASSERT(pSettings->surface);
    g_NEXUS_PictureDecoder_P_ModuleState.settings = *pSettings;
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLow; /* picture decoder interface is slow */
    g_NEXUS_PictureDecoder_P_ModuleState.module = NEXUS_Module_Create("picture_decoder", &moduleSettings);
    if(!g_NEXUS_PictureDecoder_P_ModuleState.module) { rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);goto err_module;}

    BKNI_Memset(hwState, 0, sizeof(*hwState));

    (void)BSID_GetDefaultSettings(&sidSettings);
    sidSettings.ui32_SidInstance = 0;

    rc = BSID_Open(&hwState->sid, g_pCoreHandles->chp, g_pCoreHandles->reg, g_pCoreHandles->heap[0], g_pCoreHandles->bint, &sidSettings);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_sid;}
    

    rc = BSID_GetRevisionInfo( hwState->sid , &st_RevisionInfo );
        
    BDBG_MSG(("SID PI version=%d FW version=%d" , st_RevisionInfo.ui32_PIVersion , st_RevisionInfo.ui32_FWVersion ));
    BDBG_MSG(("    RleLegacyMaxWidth     =%4d RleHdDvdRleMaxWidth=%4d" , st_RevisionInfo.ui32_RleLegacyMaxWidth  ,st_RevisionInfo.ui32_RleHdDvdRleMaxWidth ));
    BDBG_MSG(("    RleBdMaxWidth         =%4d JpegMaxWidth       =%4d" , st_RevisionInfo.ui32_RleBdMaxWidth      ,st_RevisionInfo.ui32_JpegMaxWidth ));
    BDBG_MSG(("    PngPaletteMaxWidth    =%4d PngRgbaMaxWidth    =%4d" , st_RevisionInfo.ui32_PngPaletteMaxWidth ,st_RevisionInfo.ui32_PngRgbaMaxWidth ));
    BDBG_MSG(("    Png16bppMaxWidth      =%4d GifMaxWidth        =%4d" , st_RevisionInfo.ui32_Png16BppMaxWidth   ,st_RevisionInfo.ui32_GifMaxWidth ));

    return g_NEXUS_PictureDecoder_P_ModuleState.module;

err_sid:
    NEXUS_Module_Destroy(g_NEXUS_PictureDecoder_P_ModuleState.module);
    g_NEXUS_PictureDecoder_P_ModuleState.module = NULL;
err_module:
    return NULL;
}

void
NEXUS_PictureDecoderModule_Uninit(void)
{
    struct NEXUS_PictureDecoder_P_HWState *hwState = &g_NEXUS_PictureDecoder_P_ModuleState.hwState;

    BDBG_ASSERT(g_NEXUS_PictureDecoder_P_ModuleState.module);

    (void)BSID_Close(hwState->sid);
    NEXUS_Module_Destroy(g_NEXUS_PictureDecoder_P_ModuleState.module);
    g_NEXUS_PictureDecoder_P_ModuleState.module = NULL;
    return;
}

void
NEXUS_PictureDecoder_GetDefaultStartSettings(NEXUS_PictureDecoderStartSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->format = NEXUS_PictureFormat_eJpeg;
    NEXUS_CallbackDesc_Init(&pSettings->pictureParsed);
    NEXUS_CallbackDesc_Init(&pSettings->segmentDecoded);
    return;
}

void
NEXUS_PictureDecoder_GetDefaultOpenSettings(NEXUS_PictureDecoderOpenSettings *pSettings)
{
    BERR_Code rc;
    BSID_Settings sidSettings;
    struct NEXUS_PictureDecoder_P_HWState *hwState = &g_NEXUS_PictureDecoder_P_ModuleState.hwState;
    BSID_RevisionInfo st_RevisionInfo;

    BDBG_ASSERT(pSettings);

    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    BSID_GetDefaultSettings(&sidSettings);

    rc = BSID_GetRevisionInfo( hwState->sid , &st_RevisionInfo );

    pSettings->jpeg.maxWidth              = st_RevisionInfo.ui32_JpegMaxWidth;
    pSettings->jpeg.chromaVerticalShift   = 0;
    pSettings->jpeg.chromaHorizontalShift = 0;
    pSettings->png.maxWidthRgb            = st_RevisionInfo.ui32_PngRgbaMaxWidth;
    pSettings->png.maxWidthPalette        = st_RevisionInfo.ui32_PngPaletteMaxWidth;
    pSettings->png.maxWidth16Bpp          = st_RevisionInfo.ui32_Png16BppMaxWidth;
    pSettings->gif.maxWidth               = st_RevisionInfo.ui32_GifMaxWidth;
    pSettings->rle.maxWidthDvd            = st_RevisionInfo.ui32_RleLegacyMaxWidth;
    pSettings->rle.maxWidthBluRay         = st_RevisionInfo.ui32_RleBdMaxWidth;
    pSettings->bufferSize                 = 2*1024*1024;
    pSettings->multiScanBufferSize        = 0;
    pSettings->bufferPtr                  = NULL;
    pSettings->multiScanBufferPtr         = NULL;

    return;
}

static void
NEXUS_PictureDecoder_P_Reset(NEXUS_PictureDecoderHandle decoder)
{
    int i;

    decoder->wr_offset = 0;
    decoder->busy   = false;
    decoder->chBusy = 0;
    decoder->cancel = false;
    decoder->state = NEXUS_PictureDecoder_P_eStopped;
    BKNI_Memset(&decoder->decodeState, 0, sizeof(decoder->decodeState));
    decoder->decodeState.status.state = NEXUS_PictureDecoderState_eIdle;
    decoder->decodeState.status.headerValid = false;
    decoder->decodeState.segment.surface = NULL;
    decoder->bufFlip = false;
    decoder->active  = 0;
    for ( i=0 ; i < BUF_DIV_CHUNKS ; i++ ) {
        decoder->sidQue.stage[i] = ebufFree; 
    }
    decoder->startSettings.imageSize = 0;  /* Need to reset before every GetBuffer / Decoder_Start */
    
    return;
}

static void
NEXUS_PictureDecoder_P_Callback_isr(BSID_ChannelHandle hSid, BSID_NotificationEvent eEventType, void *pCallbackData)
{
    NEXUS_PictureDecoderHandle decoder = pCallbackData;
    BDBG_MSG(("xx_isr:(%#lx) idx=%d %#lx event:%#x ", (unsigned long)decoder, decoder->index, (unsigned long)hSid, (unsigned)eEventType ));

    BDBG_OBJECT_ASSERT(decoder, NEXUS_PictureDecoder);
    BDBG_ASSERT(decoder->sid == hSid);
    decoder->decodeState.event.eventType |= eEventType; /* eventType is really a bitmask */
    /* since event handler is serialized with the module API, cancel shall be handled inside ISR callback */
 
    decoder->chBusy   = 0;  /* clear so readComp can start dma */

    if ( eEventType == BSID_NotificationEvent_eWaitingForMoreInputData ) {
        /* Convert state to data waiting type */
        switch( decoder->state ) {
            case NEXUS_PictureDecoder_P_eStreamInfo:
                decoder->state = NEXUS_PictureDecoder_P_eMoreInfoData;
                break;
            case NEXUS_PictureDecoder_P_eMoreImageData: 
            case NEXUS_PictureDecoder_P_eDecoding:
                decoder->state = NEXUS_PictureDecoder_P_eMoreImageData; 
                break;
            default:
                /* Everything else don't change state */
                break;
        }
    }
    else if ( decoder->state == NEXUS_PictureDecoder_P_eMoreInfoData ) {
        decoder->state = NEXUS_PictureDecoder_P_eStreamInfo;
        decoder->remainingBytes = decoder->startSettings.imageSize; /* reset remainingBytes, for data run */
        decoder->bufFlip = false;
    }
    else if ( decoder->state == NEXUS_PictureDecoder_P_eMoreImageData ) {
        decoder->state = NEXUS_PictureDecoder_P_eSegmentDone;
    }
    else if ( decoder->state == NEXUS_PictureDecoder_P_eDecoding && eEventType == BSID_NotificationEvent_eDecodeDone ) {
            decoder->state = NEXUS_PictureDecoder_P_eSegmentDone;
    }

    decoder->busy = false;
    /* see NEXUS_PictureDecoder_P_EventHandler */
    if(!decoder->cancel) {
        BKNI_SetEvent(decoder->event);
    } else {
        BKNI_SetEvent(decoder->eventCancel);
    }
    return;
}

static void
NEXUS_PictureDecoder_P_Failed(NEXUS_PictureDecoderHandle decoder)
{
    int i;
    unsigned multiScanBufferSize = decoder->decodeState.status.header.width * decoder->decodeState.status.header.height * 4;
    decoder->state = NEXUS_PictureDecoder_P_eFailed;
    for ( i=0; i < BUF_DIV_CHUNKS; i++ ){
        decoder->sidQue.stage[i] = ebufFree; /* clear queues */
    }
    decoder->chBusy = 0;
    BDBG_WRN(("%#lx decode failed stream: %ux%u format:%u surface:%ux%u %s %s %s multiScanBuffer:%u", (unsigned long)decoder, (unsigned)decoder->decodeState.status.header.width, (unsigned)decoder->decodeState.status.header.height, (unsigned)decoder->decodeState.status.header.format, (unsigned)decoder->decodeState.status.header.surface.width, (unsigned)decoder->decodeState.status.header.surface.height, decoder->decodeState.status.header.multiscan?"multiscan":"", decoder->decodeState.status.header.grayscale?"grayscale":"", decoder->decodeState.status.header.rotated?"rotated":"", (unsigned)decoder->openSettings.multiScanBufferSize ));
    if( (decoder->decodeState.status.header.multiscan || decoder->decodeState.status.header.rotated)  && decoder->openSettings.multiScanBufferSize<multiScanBufferSize) {
        BDBG_WRN(("%#lx stream: requires multiscan buffer at least %uKBytes", (unsigned long)decoder, (unsigned)multiScanBufferSize/1024));
    }
    return;
}


static NEXUS_Error
NEXUS_PictureDecoder_P_DecodeSegment(NEXUS_PictureDecoderHandle decoder)
{
    BERR_Code rc;
    NEXUS_SurfaceMemory memory;
    NEXUS_SurfaceCreateSettings surfaceSettings;
    BDBG_ASSERT(decoder->decodeState.segment.surface);

    NEXUS_Surface_GetMemory(decoder->decodeState.segment.surface, &memory);
    NEXUS_Surface_GetCreateSettings(decoder->decodeState.segment.surface, &surfaceSettings);

    if(decoder->decodeState.segment.rect.width == 0 && decoder->decodeState.segment.rect.height==0) {
        decoder->decodeState.segment.rect.width = decoder->decodeState.status.header.surface.width;
        decoder->decodeState.segment.rect.height = decoder->decodeState.status.header.surface.height;
    } else if(decoder->decodeState.segment.rect.width != decoder->decodeState.status.header.surface.width ||
              (unsigned)decoder->decodeState.segment.rect.y != decoder->decodeState.nextRow ||
              decoder->decodeState.segment.rect.y + decoder->decodeState.segment.rect.height > decoder->decodeState.status.header.surface.height) {
        /* only sequential horizontal stripes are supported */
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    if ( decoder->startSettings.imageSize > decoder->openSettings.bufferSize ) {
        decoder->decodeState.dataMap.ui32_Size = decoder->bufferDivide;
    }
    else {
        decoder->decodeState.dataMap.ui32_Size = decoder->wr_offset;
    }
    decoder->decodeState.dataMap.ui32_Offset = NEXUS_AddrToOffset(decoder->fifo);

    if(decoder->decodeState.segment.rect.width == decoder->decodeState.status.header.surface.width &&
        decoder->decodeState.segment.rect.height == decoder->decodeState.status.header.surface.height &&
        decoder->decodeState.segment.rect.x == 0 &&  decoder->decodeState.segment.rect.y == 0) {

        /* Full decode of image */
        BSID_DecodeImage decodeImage;
        BDBG_MSG(("Full image decode=%p idx=%d" , decoder, decoder->index ));

        BKNI_Memset(&decodeImage, 0, sizeof(decodeImage));
        decodeImage.s_ImageBuffer.ui32_Pitch   = memory.pitch;
        decodeImage.s_ImageBuffer.ui32_Width   = surfaceSettings.width;
        decodeImage.s_ImageBuffer.ui32_Height  = surfaceSettings.height;
        decodeImage.s_ImageBuffer.ui32_Offset  = NEXUS_AddrToOffset(memory.buffer);
        decodeImage.e_ImageFormat   = decoder->decodeState.imageFormat;
        decodeImage.e_DataMapType   = BSID_DataMapType_eLinear;
        decodeImage.pv_InputDataMap = &decoder->decodeState.dataMap;
        decodeImage.ui32_MultiScanBufSize = decoder->openSettings.multiScanBufferSize;
        decodeImage.ui32_MultiScanBufAddr = decoder->multiScanBufOffset;

        decoder->state  = NEXUS_PictureDecoder_P_eDecoding;
        decoder->busy   = true;
        decoder->active = 0; 
        decoder->chBusy = 1; 
        if ( decoder->startSettings.imageSize < decoder->openSettings.bufferSize ) {
            /* Using cdb large enough to hold image, so flag this to SID */
            decodeImage.b_LastDmaChunk = 1;
        }
        rc = BSID_DecodeStillImage( decoder->sid, &decodeImage);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
    } else {
        /* Segmented decode of image */
        BSID_DecodeSegment decodeSegment;
        BDBG_MSG(("Segmented decode=%p idx=%d of image " , decoder, decoder->index));
        BKNI_Memset(&decodeSegment, 0, sizeof(decodeSegment));
        decodeSegment.s_ImageBuffer.ui32_Pitch   = memory.pitch;
        decodeSegment.s_ImageBuffer.ui32_Width   = surfaceSettings.width;
        decodeSegment.s_ImageBuffer.ui32_Height  = surfaceSettings.height;
        decodeSegment.s_ImageBuffer.ui32_Offset  = NEXUS_AddrToOffset(memory.buffer);
        decodeSegment.e_ImageFormat   = decoder->decodeState.imageFormat;
        decodeSegment.e_DataMapType   = BSID_DataMapType_eLinear;
        decodeSegment.pv_InputDataMap = &decoder->decodeState.dataMap;
        decodeSegment.b_FirstSegment  = decoder->decodeState.segment.rect.y == 0;
        decodeSegment.ui32_SegmentHeight    = decoder->decodeState.segment.rect.height;
        decodeSegment.ui32_MultiScanBufSize = decoder->openSettings.multiScanBufferSize;
        decodeSegment.ui32_MultiScanBufAddr = decoder->multiScanBufOffset;

        if ( decoder->decodeState.status.header.multiscan  || decoder->decodeState.status.header.rotated ) {
            BDBG_ERR(("Segmented output decode of multiscan/rotated jpeg not supported!"));
            /* purposefully disable multiscan, to generate error and abort process */
            /* as Segmented output decode of multiscan/rotated jpeg not supported  */
            decodeSegment.ui32_MultiScanBufSize = 0;
            decodeSegment.ui32_MultiScanBufAddr = 0;
        }

        decoder->decodeState.nextRow += decoder->decodeState.segment.rect.height;
        decoder->state = NEXUS_PictureDecoder_P_eDecoding;
        decoder->busy = true;
        decoder->chBusy  = 1; 
        if ( decoder->startSettings.imageSize < decoder->openSettings.bufferSize ) {
            /* buffer is large enough to hold image, so flag this to SID */
            decodeSegment.b_LastDmaChunk = 1;
        }
        BDBG_MSG(("NEXUS_PictureDecoder_P_DecodeSegment: %#lx %u %s", (unsigned long)decoder, (unsigned)decodeSegment.ui32_SegmentHeight, decodeSegment.b_FirstSegment?"first":""));
        rc = BSID_DecodeImageSegment(decoder->sid, &decodeSegment);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
    }

    return NEXUS_SUCCESS;

error:
    NEXUS_PictureDecoder_P_Failed(decoder);
    decoder->busy   = false;
    decoder->active = 0;
    decoder->chBusy = 1; 
    return rc;
}

static void
NEXUS_PictureDecoder_P_EventHandler(void *cntx)
{
    NEXUS_PictureDecoderHandle decoder = cntx;
    BERR_Code rc;
    unsigned eventType;
    int i;

    BDBG_OBJECT_ASSERT(decoder, NEXUS_PictureDecoder);
    BDBG_MSG(("event:(%#lx) idx=%d state:%d event:%#x resource:%u", (unsigned long)decoder, decoder->index, decoder->state, (unsigned)decoder->decodeState.event.eventType, decoder->decodeState.event.resourceStatus));
    BKNI_EnterCriticalSection();
    eventType = decoder->decodeState.event.eventType;
    decoder->decodeState.event.eventType = 0;
    BKNI_LeaveCriticalSection();
    if(decoder->state==NEXUS_PictureDecoder_P_eStopped) {
        goto done;
    }
    if( (eventType&(BSID_NotificationEvent_eDecodeError | BSID_NotificationEvent_eGetInfoError))!=0) {
        BDBG_WRN(("NEXUS_PictureDecoder_P_EventHandler: %#lx notification about error %x", (unsigned long)decoder, eventType));
        goto error;
    }
#if 0
    {
    BSID_Status status;
    rc = BSID_GetStatus(decoder->sid, &status);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc);goto error; }
    if(status.eDecodeResult == BSID_DecodeResult_eFailed) {
        BDBG_WRN(("NEXUS_PictureDecoder_P_EventHandler: %#lx decoding failed", (unsigned long)decoder));
        goto error;
    }
    }
#endif
    if ( !(eventType&(BSID_NotificationEvent_eGetInfoDone)) && eventType&(BSID_NotificationEvent_eWaitingForMoreInputData) ) {
        /* fire callback to get more data */
        switch( decoder->state ) {
            case NEXUS_PictureDecoder_P_eStreamInfo:
            case NEXUS_PictureDecoder_P_eMoreInfoData:
                for ( i=0 ; i < BUF_DIV_CHUNKS; i++ ) {
                    BDBG_MSG_TRACE(("_EvntHdlr active=%d stage[%d]=%d" , decoder->active, i,
                                                              decoder->sidQue.stage[i] ));

                    if ( decoder->active != i && decoder->sidQue.stage[i] == ebufFilled ) {
                        BKNI_EnterCriticalSection();
                        decoder->active = i;
                        decoder->chBusy   = 1;       /* signal we are processing so readComp doesnt start dma */
                        BKNI_LeaveCriticalSection();
                        BSID_SetDmaChunkInfo( decoder->sid, &decoder->sidQue.chunkInfo[i] );
                    }
                }
                if ( decoder->chBusy > 1 ) {
                    BKNI_EnterCriticalSection();
                    decoder->chBusy = 0;
                    BKNI_LeaveCriticalSection();
                }
                break;
            case NEXUS_PictureDecoder_P_eMoreImageData:
            case NEXUS_PictureDecoder_P_eDecoding:
                decoder->state = NEXUS_PictureDecoder_P_eMoreImageData;

                decoder->sidQue.stage[ decoder->active ] = ebufFree; /* can be used again */
                for ( i=0 ; i < BUF_DIV_CHUNKS; i++ ) {
                    if ( decoder->active != i && decoder->sidQue.stage[i] == ebufFilled ) {
                        BKNI_EnterCriticalSection();
                        decoder->active = i;
                        decoder->chBusy = 1;         /* signal we are processing so readComp doesnt start dma */
                        decoder->state  = NEXUS_PictureDecoder_P_eDecoding;
                        BKNI_LeaveCriticalSection();
                        BSID_SetDmaChunkInfo( decoder->sid, &decoder->sidQue.chunkInfo[i] );
                    }
                }
                if ( decoder->chBusy > 1 ) {
                    BKNI_EnterCriticalSection();
                    decoder->chBusy = 0;
                    BKNI_LeaveCriticalSection();
                }
                break;
            default:
                /* Everything else don't change state */
                break;
        } /* end of switch */
        NEXUS_TaskCallback_Fire(decoder->segmentDecodedCallback);
        goto done;
    }

    switch(decoder->state) {
    case NEXUS_PictureDecoder_P_eStreamInfo:
        if( (eventType&BSID_NotificationEvent_eGetInfoDone)!=BSID_NotificationEvent_eGetInfoDone) {
            break;
        }
        decoder->decodeState.status.header.format = NEXUS_P_PixelFormat_FromMagnum(decoder->imageHeader.e_PixelFormat);
        decoder->decodeState.status.header.width  = decoder->imageHeader.ui32_ImgWidth;
        decoder->decodeState.status.header.height = decoder->imageHeader.ui32_ImgHeight;
        decoder->decodeState.status.header.multiscan = decoder->imageHeader.ui32_MultiScan != 0;
        decoder->decodeState.status.header.grayscale = decoder->imageHeader.ui32_GrayScale != 0;
        decoder->decodeState.status.header.palette = decoder->imageHeader.ui32_NumClutEntries!=0;
        decoder->decodeState.status.header.rotated = decoder->imageHeader.e_JpegSubtype == BSID_JpegSubType_e422r;
        decoder->decodeState.status.header.hasTransparentData = decoder->imageHeader.ui32_HasTransparentData;
        decoder->decodeState.status.header.transparentColorIndex = decoder->imageHeader.ui32_TransparentColorIndex;
        decoder->decodeState.status.header.transparentColorRgb = decoder->imageHeader.ui32_TransparentColorRGB;
        decoder->decodeState.status.header.surface.x = 0;
        decoder->decodeState.status.header.surface.y = 0;
        decoder->decodeState.status.header.surface.width = decoder->imageHeader.ui32_SurWidth;
        decoder->decodeState.status.header.surface.height = decoder->imageHeader.ui32_SurHeight;
        decoder->decodeState.status.headerValid = true;
        decoder->state = NEXUS_PictureDecoder_P_eStreamInfoDone;
        NEXUS_TaskCallback_Fire(decoder->pictureParsedCallback);
        BDBG_MSG(("NEXUS_PictureDecoder_P_EventHandler: %#lx stream: %ux%u format:%u surface:%ux%u %s %s %s", (unsigned long)decoder, (unsigned)decoder->decodeState.status.header.width, (unsigned)decoder->decodeState.status.header.height, (unsigned)decoder->decodeState.status.header.format, (unsigned)decoder->decodeState.status.header.surface.width, (unsigned)decoder->decodeState.status.header.surface.height, decoder->decodeState.status.header.multiscan?"multiscan":"", decoder->decodeState.status.header.grayscale?"grayscale":"", decoder->decodeState.status.header.rotated?"rotated":""));

        BKNI_EnterCriticalSection();
        /* clear any buffer state, before starting image part of decode */
        for ( i=0 ; i < BUF_DIV_CHUNKS; i++ ) {
            decoder->sidQue.stage[i] = ebufFree; /* can be used again */
        }
        decoder->active = 0;
        decoder->chBusy = 0;  /* signal we are processing so readComp doesnt start dma */
        BKNI_LeaveCriticalSection();

        if(decoder->decodeState.segment.surface) {
            rc = NEXUS_PictureDecoder_P_DecodeSegment(decoder);
            if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc); goto error;}
        }
        break;
    case NEXUS_PictureDecoder_P_eDecoding:
        decoder->state = NEXUS_PictureDecoder_P_eSegmentDone;
        NEXUS_TaskCallback_Fire(decoder->segmentDecodedCallback);
        break;
    case NEXUS_PictureDecoder_P_eSegmentDone:
        decoder->state = NEXUS_PictureDecoder_P_eSegmentDone;
        NEXUS_TaskCallback_Fire(decoder->segmentDecodedCallback);
        break;
    default:
        break;
    }
    return;
error:
    NEXUS_PictureDecoder_P_Failed(decoder);
    NEXUS_TaskCallback_Fire(decoder->pictureParsedCallback);
    NEXUS_TaskCallback_Fire(decoder->segmentDecodedCallback);
done:
    return;
}


NEXUS_Error
NEXUS_PictureDecoder_P_ParseHeader(NEXUS_PictureDecoderHandle decoder)
{
    BSID_StreamInfo streamInfo;
    BERR_Code rc;

    BKNI_Memset(&streamInfo, 0, sizeof(streamInfo));

    decoder->decodeState.dataMap.ui32_Size   = decoder->wr_offset;
    decoder->decodeState.dataMap.ui32_Offset = NEXUS_AddrToOffset(decoder->fifo);

    streamInfo.e_ImageFormat   = decoder->decodeState.imageFormat;
    streamInfo.e_DataMapType   = BSID_DataMapType_eLinear;
    streamInfo.pv_InputDataMap = &decoder->decodeState.dataMap;
    streamInfo.ps_OutImageInfo = &decoder->imageHeader;
    decoder->state  = NEXUS_PictureDecoder_P_eStreamInfo;
    decoder->busy   = true;
    BKNI_EnterCriticalSection();
    decoder->active = 0;
    decoder->chBusy = 1;
    BKNI_LeaveCriticalSection();

    rc = BSID_GetStreamInfo( decoder->sid, &streamInfo);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}
    return NEXUS_SUCCESS;

error:
    decoder->busy = false;
    decoder->active  = 0;
    decoder->chBusy  = 0;
    decoder->state = NEXUS_PictureDecoder_P_eFailed;
    return rc;
}

static NEXUS_Error nexus_picturedecoder_verify(NEXUS_PictureDecoderHandle decoder, bool checkOnly )
{
    struct NEXUS_PictureDecoder_P_HWState *hwState = &g_NEXUS_PictureDecoder_P_ModuleState.hwState;
    int i;


    if ( hwState->nrSidChOpen > SID_DECODE_CHANNELS ) {
        int active=0;

        BDBG_MSG(("Open decoders (%d) > SID channels (%d) , check for free channel" , hwState->nrSidChOpen, SID_DECODE_CHANNELS ));

        for ( i=0; i < SID_DECODE_CHANNELS; i++ ) {
            if ( decoder == hwState->acquired[i] ) {
                active++;  
             }
        }
        if ( active ) {
            if ( active > 1 ) {
                BDBG_WRN(("More than one in list active=%d!!!" ));
                BDBG_ASSERT( !active );  /* shouldn't be any duplicates */
            }
            return 0;  /* we are currently active */
        }

        /* passed in decoder may not be active, but check if SID channel is active */
        for ( i=0; i < SID_DECODE_CHANNELS; i++ ) {
            if ( hwState->acquired[i] && decoder->sid == hwState->acquired[i]->sid ) {
                BDBG_MSG(("dec->sid=%p is already in use by slot %d" , decoder->sid , i ));
                return NEXUS_NOT_AVAILABLE;
            }
        }


        for ( i=0; i < SID_DECODE_CHANNELS; i++ ) {
            if ( !hwState->acquired[i] ) {
                if ( !checkOnly ) { 
                    BDBG_MSG(("%p acquired slot=%d idx=%d", decoder , i, decoder->index));
                    hwState->acquired[i] = decoder;
                    goto free_slot;  /* break out of for loop, we have a free decoder slot */
                }
            }
        }
        return NEXUS_NOT_AVAILABLE;
free_slot:
        BDBG_MSG(("free slot was %d" , i ));
    }
    return 0;
}

NEXUS_Error
NEXUS_PictureDecoder_Start(NEXUS_PictureDecoderHandle decoder, const NEXUS_PictureDecoderStartSettings *pSettings)
{
    NEXUS_Error rc;
    NEXUS_PictureDecoderStartSettings settings;
    BSID_ChannelSettings sidChSettings;
    BDBG_OBJECT_ASSERT(decoder, NEXUS_PictureDecoder);

    if (nexus_picturedecoder_verify(decoder,false)) {
        return NEXUS_NOT_AVAILABLE; /* currently, not an error. */
    }

    if(decoder->state != NEXUS_PictureDecoder_P_eStopped) { return BERR_TRACE(BERR_NOT_SUPPORTED); }

    if(!pSettings) {
        NEXUS_PictureDecoder_GetDefaultStartSettings(&settings);
        pSettings = &settings;
    }

    switch(pSettings->format) {
#if 0
    case NEXUS_PictureFormat_eRleDvd:
        decoder->decodeState.imageFormat = BSID_ImageFormat_eRLE_Legacy_DVD;
        break;
    case NEXUS_PictureFormat_eRleBluRay:
        decoder->decodeState.imageFormat = BSID_ImageFormat_eRLE_BD;
        break;
#endif
    case NEXUS_PictureFormat_eJpeg:
        decoder->decodeState.imageFormat = BSID_ImageFormat_eJPEG;
        break;
    case NEXUS_PictureFormat_ePng:
        decoder->decodeState.imageFormat = BSID_ImageFormat_ePNG;
        break;
    case NEXUS_PictureFormat_eGif:
        decoder->decodeState.imageFormat = BSID_ImageFormat_eGIF;
        break;
    case NEXUS_PictureFormat_eMpeg:
        decoder->decodeState.imageFormat = BSID_ImageFormat_eMPEG_IFRAME;
        break;
    case NEXUS_PictureFormat_eRleDvd:
        /* coverity[unterminated_case] */
    case NEXUS_PictureFormat_eRleBluRay:
        decoder->state = NEXUS_PictureDecoder_P_eFailed; 
        /* coverity[fallthrough] */
    default:
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }


    decoder->startSettings = *pSettings;
    if ( decoder->startSettings.imageSize > decoder->openSettings.bufferSize ) {
        decoder->remainingBytes = decoder->startSettings.imageSize;
        decoder->bufferDivide   = decoder->openSettings.bufferSize/2;
        BDBG_MSG(("imageSize(%d) > bufferSize(%d), small buffer decode enabled" , decoder->startSettings.imageSize, decoder->openSettings.bufferSize ));
    }

    NEXUS_Module_TaskCallback_Set(decoder->pictureParsedCallback, &decoder->startSettings.pictureParsed);
    NEXUS_Module_TaskCallback_Set(decoder->segmentDecodedCallback, &decoder->startSettings.segmentDecoded);
    decoder->state = NEXUS_PictureDecoder_P_eWaitingData;

    BSID_GetChannelSettings( decoder->sid, &sidChSettings );

    sidChSettings.u_ChannelSpecific.still.p_CallbackFunc_Isr = decoder->sidChOpenSettings.u_ChannelSpecific.still.p_CallbackFunc_Isr;
    sidChSettings.u_ChannelSpecific.still.ui32_CallbackEvents = decoder->sidChOpenSettings.u_ChannelSpecific.still.ui32_CallbackEvents;
    sidChSettings.u_ChannelSpecific.still.pv_CallbackData = decoder->sidChOpenSettings.u_ChannelSpecific.still.pv_CallbackData;

    /* Set callbackData */
    BSID_SetChannelSettings( decoder->sid, &sidChSettings );

    if(decoder->wr_offset>0) {
        rc = NEXUS_PictureDecoder_P_ParseHeader(decoder);
        if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}
    }
    return NEXUS_SUCCESS;
}

void
NEXUS_PictureDecoder_P_Stop(NEXUS_PictureDecoderHandle decoder)
{
    unsigned i;
    struct NEXUS_PictureDecoder_P_HWState *hwState = &g_NEXUS_PictureDecoder_P_ModuleState.hwState;

    BDBG_OBJECT_ASSERT(decoder, NEXUS_PictureDecoder);

    NEXUS_Module_TaskCallback_Set(decoder->pictureParsedCallback, NULL);
    NEXUS_Module_TaskCallback_Set(decoder->segmentDecodedCallback, NULL);

    if ( decoder->chBusy || (decoder->state == NEXUS_PictureDecoder_P_eMoreInfoData) || (decoder->state == NEXUS_PictureDecoder_P_eMoreImageData) ){
        /* If in anyway busy, send abort DMA to SID to shut it down and wait for Isr event that it has done so */
        BSID_DmaChunkInfo DMAChunkInfo;

        BKNI_Memset( &DMAChunkInfo , 0 , sizeof( BSID_DmaChunkInfo ) );
        DMAChunkInfo.ui32_AbortDma = true;

        BKNI_EnterCriticalSection();
        decoder->chBusy = true;
        decoder->cancel = true;
        BKNI_LeaveCriticalSection();
        BSID_SetDmaChunkInfo( decoder->sid, &DMAChunkInfo);
    }

    for(i=0;i<10;i++) {
        bool busy=false;
        BERR_Code err;

        BKNI_EnterCriticalSection();
        decoder->cancel = true;
        busy = decoder->chBusy;
        BKNI_LeaveCriticalSection();
        if(!busy) {
            break;
        }
        /* we activated decoder, and need to wait for decoder to either report success/or failure */
        err = BKNI_WaitForEvent(decoder->eventCancel, 100/*ms*/);
        if ( BERR_TIMEOUT == err ) {
            BDBG_WRN(("NEXUS_PictureDecoder_P_Stop: %#lx waiting decoder Idx=%d to respond [%u]", (unsigned long)decoder, decoder->index,  i));
        }
    }

    NEXUS_PictureDecoder_P_Reset(decoder);

    for ( i=0; i < SID_DECODE_CHANNELS; i++ ) {
        if (hwState->acquired[i] == decoder) {
            BDBG_MSG_TRACE(("x_P_Stop dec=%p released dec->idx=%d ", decoder , decoder->index ));
            hwState->acquired[i] = NULL;
            break;
        }
    }

    return ;

}

void
NEXUS_PictureDecoder_Stop(NEXUS_PictureDecoderHandle decoder)
{
    unsigned i;
    struct NEXUS_PictureDecoder_P_HWState *hwState = &g_NEXUS_PictureDecoder_P_ModuleState.hwState;

    BDBG_OBJECT_ASSERT(decoder, NEXUS_PictureDecoder);

    if (nexus_picturedecoder_verify(decoder,true)) {
        BDBG_WRN(("nexus_picturedecoder_verify rtnd a val, CAN'T STOP Decoder(%p)" , decoder ));
        return; /* currently, not an error. */
    }

    if (decoder->state == NEXUS_PictureDecoder_P_eStopped) {
        return;
    }
    NEXUS_Module_TaskCallback_Set(decoder->pictureParsedCallback, NULL);
    NEXUS_Module_TaskCallback_Set(decoder->segmentDecodedCallback, NULL);

    if ( decoder->chBusy || (decoder->state == NEXUS_PictureDecoder_P_eMoreInfoData) || (decoder->state == NEXUS_PictureDecoder_P_eMoreImageData) ){
        /* If in anyway busy, send abort DMA to SID to shut it down and wait for Isr event that it has done so */
        BSID_DmaChunkInfo DMAChunkInfo;

        BKNI_Memset( &DMAChunkInfo , 0 , sizeof( BSID_DmaChunkInfo ) );
        DMAChunkInfo.ui32_AbortDma = true;

        BKNI_EnterCriticalSection();
        decoder->chBusy = true;
        decoder->cancel = true;
        BKNI_LeaveCriticalSection();
        BSID_SetDmaChunkInfo( decoder->sid, &DMAChunkInfo);
    }

    for(i=0;i<10;i++) {
        bool busy=false;
        BERR_Code err;

        BKNI_EnterCriticalSection();
        decoder->cancel = true;
        busy = decoder->chBusy; 
        BKNI_LeaveCriticalSection();
        if(!busy) {
            break;
        }
        /* we activated decoder, and need to wait for decoder to either report success/or failure */
        err = BKNI_WaitForEvent(decoder->eventCancel, 100/*ms*/);
        if ( BERR_TIMEOUT == err ) {
            BDBG_WRN(("NEXUS_PictureDecoder_Stop: %#lx waiting decoder Idx=%d to respond [%u]", (unsigned long)decoder, decoder->index,  i));
        }
    }

    NEXUS_PictureDecoder_P_Reset(decoder);
    for ( i=0; i < SID_DECODE_CHANNELS; i++ ) {
        if (hwState->acquired[i] == decoder) {
            BDBG_MSG(("x_Stop dec=%p released slot=%d dec->idx=%d ", decoder , i, decoder->index ));
            hwState->acquired[i] = NULL;
            break;
        }
    }

    return ;
}

NEXUS_Error
NEXUS_PictureDecoder_GetBuffer(NEXUS_PictureDecoderHandle decoder, void **pBuffer, size_t *pSize)
{
    BDBG_OBJECT_ASSERT(decoder, NEXUS_PictureDecoder);
    BDBG_ASSERT(pBuffer);
    BDBG_ASSERT(pSize);

    if (nexus_picturedecoder_verify(decoder,false)) {
        return NEXUS_NOT_AVAILABLE; /* currently, not an error. */
    }

    switch(decoder->state) {
    case NEXUS_PictureDecoder_P_eStopped:
    case NEXUS_PictureDecoder_P_eWaitingData:
    case NEXUS_PictureDecoder_P_eMoreInfoData:
    case NEXUS_PictureDecoder_P_eMoreImageData:
    case NEXUS_PictureDecoder_P_eStreamInfoDone:
    case NEXUS_PictureDecoder_P_eSegmentDone:
    case NEXUS_PictureDecoder_P_eDecoding:
        break;
    default:
        *pBuffer = NULL;
        *pSize = 0;
        BDBG_WRN(("Dec->state=%d " , decoder->state ));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }

    if ( decoder->startSettings.imageSize > decoder->openSettings.bufferSize ) {
        /* Small buffer mode */
        if ( decoder->sidQue.stage[0] && decoder->sidQue.stage[1] ) { /* either non zero means processing */
            *pBuffer = NULL;
            *pSize = 0;
            return BERR_UNKNOWN;  /* SID is still processing data, so can't return buffer until it has finished */
        }
        *pSize = decoder->openSettings.bufferSize/2;
        if( decoder->bufFlip ) {
            /* Read into mid point where SID expects it */
            *pBuffer = (uint8_t *)decoder->fifo+(decoder->bufferDivide);
            BDBG_MSG_TRACE(("Back dec->fifo=0x%p physDecFifo=0x%p Buffer=0x%8x pSize=%d dec->bufDiv=%d" , decoder->fifo , NEXUS_AddrToOffset(*pBuffer),
                                                                                                         *pBuffer, *pSize, decoder->bufferDivide ));
            decoder->sidQue.stage[1] = ebufAllocd;
            decoder->bufFlip = false;
        }
        else {
            *pBuffer = (uint8_t *)decoder->fifo;
            BDBG_MSG_TRACE(("Frnt dec->fifo=0x%p physDecFifo=0x%p pBuffer=0x%8x pSize=%d dec->bufDiv=%d" , decoder->fifo ,NEXUS_AddrToOffset(*pBuffer),
                                                                                                        *pBuffer, *pSize, decoder->bufferDivide ));
            decoder->sidQue.stage[0] = ebufAllocd ;
            decoder->bufFlip = true;
        }
    }
    else {
        /* full size buffer mode */
        *pBuffer = (uint8_t *)decoder->fifo + decoder->wr_offset;
        *pSize = decoder->openSettings.bufferSize - decoder->wr_offset;
         BDBG_MSG(("FULL dec->fifo=0x%p pBuffer=0x%8x pSize=%d " , decoder->fifo ,*pBuffer, *pSize ));
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error
NEXUS_PictureDecoder_ReadComplete(NEXUS_PictureDecoderHandle decoder, size_t skip, size_t amountUsed)
{
    NEXUS_Error rc;
    int i;
    BDBG_OBJECT_ASSERT(decoder, NEXUS_PictureDecoder);

    if (nexus_picturedecoder_verify(decoder,false)) {
        return NEXUS_NOT_AVAILABLE; /* currently, not an error. */
    }

    switch(decoder->state) {
    case NEXUS_PictureDecoder_P_eMoreInfoData:
    case NEXUS_PictureDecoder_P_eMoreImageData:
        break;
    case NEXUS_PictureDecoder_P_eStopped:
    case NEXUS_PictureDecoder_P_eWaitingData:
        break;
    case NEXUS_PictureDecoder_P_eStreamInfoDone:
    case NEXUS_PictureDecoder_P_eSegmentDone:
        break;
    case NEXUS_PictureDecoder_P_eFailed:
        return NEXUS_NOT_AVAILABLE; /* decode failed, there's nothing to be done */ 
        break;
    case NEXUS_PictureDecoder_P_eStreamInfo:
        BDBG_WRN(("SID decode buffer(=%d kByte) is probably too small to decode header, increase buffer size, decoder->state=%d" , 
            (decoder->openSettings.bufferSize/1024), decoder->state ));
        break;
    default:
        BDBG_WRN(("decoder->state=%d Idx=%d" , decoder->state, decoder->index ));
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    if(skip!=0) { return BERR_TRACE(BERR_NOT_SUPPORTED);}
    if( !decoder->startSettings.imageSize && amountUsed > (decoder->openSettings.bufferSize - decoder->wr_offset)) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if ( decoder->startSettings.imageSize > decoder->openSettings.bufferSize ) {
        for ( i=0 ;i < BUF_DIV_CHUNKS; i++ ) {
            if ( decoder->sidQue.stage[i] == ebufAllocd ) {
                BSID_DmaChunkInfo DMAChunkInfo;

                if ( decoder->remainingBytes > decoder->bufferDivide ) {
                    DMAChunkInfo.ui32_Size = decoder->bufferDivide;
                }
                else {
                    DMAChunkInfo.ui32_Size = decoder->remainingBytes;
                }
                decoder->remainingBytes   -= DMAChunkInfo.ui32_Size;

                if ( i == 0 ) {
                    DMAChunkInfo.ui32_Offset = NEXUS_AddrToOffset(decoder->fifo);
                    NEXUS_FlushCache((uint8_t *)decoder->fifo, decoder->openSettings.bufferSize ); /* just flush part of buffer */
                }
                else {
                    DMAChunkInfo.ui32_Offset = NEXUS_AddrToOffset((uint8_t *)decoder->fifo+(decoder->bufferDivide));
                    NEXUS_FlushCache((uint8_t *)decoder->fifo+(decoder->bufferDivide), decoder->openSettings.bufferSize ); /* just flush part of buffer */
                }

                DMAChunkInfo.ui32_LastDmaChunk = (decoder->remainingBytes<= 0); 
                DMAChunkInfo.ui32_AbortDma     = 0x0;

                decoder->sidQue.chunkInfo[i].ui32_Offset       = DMAChunkInfo.ui32_Offset;
                decoder->sidQue.chunkInfo[i].ui32_Size         = DMAChunkInfo.ui32_Size;
                decoder->sidQue.chunkInfo[i].ui32_AbortDma     = DMAChunkInfo.ui32_AbortDma;
                decoder->sidQue.chunkInfo[i].ui32_LastDmaChunk = DMAChunkInfo.ui32_LastDmaChunk;

                BKNI_EnterCriticalSection();
                decoder->sidQue.stage[i] = ebufFilled;
                BKNI_LeaveCriticalSection();

                BKNI_EnterCriticalSection();
                if ( decoder->chBusy == ebufFree && decoder->state != NEXUS_PictureDecoder_P_eSegmentDone && decoder->state != NEXUS_PictureDecoder_P_eStreamInfoDone ) {
                    decoder->chBusy  = ebufAllocd;
                    decoder->active  = i;
                    decoder->state   = NEXUS_PictureDecoder_P_eDecoding;
                    BKNI_LeaveCriticalSection();
                    /* Start SID now */
                    BSID_SetDmaChunkInfo( decoder->sid, &DMAChunkInfo);
                }
                else {
                    BKNI_LeaveCriticalSection();

                    if ( decoder->state == NEXUS_PictureDecoder_P_eSegmentDone ) {
                        BKNI_EnterCriticalSection();
                        decoder->sidQue.stage[i] = ebufFree;
                        BKNI_LeaveCriticalSection();
                        return NEXUS_NOT_AVAILABLE;
                    }
                }
            } 

        } 

    } /* end of small buffer model */
    else {
        /* large buffer model */
        NEXUS_FlushCache((uint8_t *)decoder->fifo, amountUsed );
    }
    decoder->wr_offset += amountUsed;

    if(decoder->state == NEXUS_PictureDecoder_P_eWaitingData && decoder->wr_offset>0) {
        rc = NEXUS_PictureDecoder_P_ParseHeader(decoder);
        if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error
NEXUS_PictureDecoder_GetStatus(NEXUS_PictureDecoderHandle decoder, NEXUS_PictureDecoderStatus *pStatus)
{
    BDBG_OBJECT_ASSERT(decoder, NEXUS_PictureDecoder);
    BDBG_ASSERT(pStatus);

    if (nexus_picturedecoder_verify(decoder,false)) {
        return NEXUS_NOT_AVAILABLE; /* currently, not an error. */
    }

    *pStatus = decoder->decodeState.status;
    switch(decoder->state) {
    case NEXUS_PictureDecoder_P_eSegmentDone:
        pStatus->state = NEXUS_PictureDecoderState_eSegmentDone;
        break;
    case NEXUS_PictureDecoder_P_eStopped:
        pStatus->state = NEXUS_PictureDecoderState_eIdle;
        break;
    case NEXUS_PictureDecoder_P_eFailed:
        pStatus->state = NEXUS_PictureDecoderState_eError;
        break;
    case NEXUS_PictureDecoder_P_eMoreInfoData:
        pStatus->state = NEXUS_PictureDecoderState_eMoreData;
        break;
    case NEXUS_PictureDecoder_P_eMoreImageData:
        pStatus->state = NEXUS_PictureDecoderState_eMoreData;
        break;
    default:
        pStatus->state = NEXUS_PictureDecoderState_eDecoding;
        break;
    }
    BDBG_MSG(("NEXUS_PictureDecoder_GetStatus:%#lx index=%d state:%u", (unsigned long)decoder, decoder->index , (unsigned)pStatus->state));
    return NEXUS_SUCCESS;
}

NEXUS_Error
NEXUS_PictureDecoder_GetPalette(NEXUS_PictureDecoderHandle decoder, NEXUS_PictureDecoderPalette *pPalette)
{
    BDBG_OBJECT_ASSERT(decoder, NEXUS_PictureDecoder);
    BDBG_ASSERT(pPalette);
    if (nexus_picturedecoder_verify(decoder,false)) {
        return NEXUS_NOT_AVAILABLE; /* currently, not an error. */
    }
    if(!decoder->decodeState.status.headerValid) {
        return BERR_TRACE(BERR_NOT_SUPPORTED);
    }
    pPalette->numPaletteEntries = decoder->imageHeader.ui32_NumClutEntries;

    BDBG_CASSERT(sizeof(pPalette->palette) == sizeof(decoder->imageHeader.aui32_Clut));
    BKNI_Memcpy(pPalette->palette, decoder->imageHeader.aui32_Clut, sizeof(pPalette->palette));
    return NEXUS_SUCCESS;
}

NEXUS_Error
NEXUS_PictureDecoder_DecodeSegment(NEXUS_PictureDecoderHandle decoder, NEXUS_SurfaceHandle destination, const NEXUS_Rect *segment)
{
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(decoder, NEXUS_PictureDecoder);
    BDBG_ASSERT(destination);

    if (nexus_picturedecoder_verify(decoder,false)) {
        return NEXUS_NOT_AVAILABLE; /* currently, not an error. */
    }
    decoder->decodeState.segment.surface = destination;
    if(segment) {
        if(segment->x != 0  || segment->width <= 0 || segment->height<=0 || segment->y<0) {
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
        decoder->decodeState.segment.rect = *segment;
    } else {
        decoder->decodeState.segment.rect.x = 0;
        decoder->decodeState.segment.rect.y = 0;
        decoder->decodeState.segment.rect.width = 0;
        decoder->decodeState.segment.rect.height = 0;
    }

    if(decoder->state == NEXUS_PictureDecoder_P_eStreamInfoDone || decoder->state == NEXUS_PictureDecoder_P_eSegmentDone)
    {
        rc = NEXUS_PictureDecoder_P_DecodeSegment(decoder);
        if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}
    }

    return NEXUS_SUCCESS;
}

NEXUS_PictureDecoderHandle
NEXUS_PictureDecoder_Open(unsigned index, const NEXUS_PictureDecoderOpenSettings *pSettings)
{
    NEXUS_PictureDecoderHandle decoder;
    NEXUS_PictureDecoderOpenSettings settings;
    BERR_Code rc;
    NEXUS_HeapHandle heap;
    struct NEXUS_PictureDecoder_P_HWState *hwState = &g_NEXUS_PictureDecoder_P_ModuleState.hwState;

    if(!pSettings) {
        NEXUS_PictureDecoder_GetDefaultOpenSettings(&settings);
        pSettings = &settings;
    }
    if ( index != NEXUS_ANY_ID && index >= NEXUS_NUM_PICTURE_DECODERS) {
        BDBG_ERR(("index=%d Number HW pic decoders=%d" , index, NEXUS_NUM_PICTURE_DECODERS  ));
        BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }
    else {
        /* TODO: handle multi sid decoder plus multi sid channels */
        unsigned i;
        for (i=0;i<SID_DECODE_CHANNELS;i++) {
            if (!hwState->opened[i]) {
                index = i;
                break;
            }
        }
        if (i == SID_DECODE_CHANNELS) {
            BDBG_WRN(("Can't open additional channel. %d out of %d (MAX) already open." , hwState->nrSidChOpen, SID_DECODE_CHANNELS ));
            rc = BERR_TRACE(BERR_NOT_SUPPORTED);
            return NULL;
        }
    }
    
    heap = NEXUS_P_DefaultHeap(pSettings->heap, NEXUS_DefaultHeapType_eFull);
    if (!heap) heap = g_pCoreHandles->nexusHeap[0];
    if (!NEXUS_P_CpuAccessibleHeap(heap)) {
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        return NULL;
    }
    if (pSettings->bufferPtr) {
        if (!NEXUS_P_CpuAccessibleAddress(pSettings->bufferPtr)) {
            rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
            return NULL;
        }
    }
    
    rc = NEXUS_CLIENT_RESOURCES_ACQUIRE(pictureDecoder,Count,NEXUS_ANY_ID);
    if (rc) { rc = BERR_TRACE(rc); return NULL; }

    decoder = BKNI_Malloc(sizeof(*decoder));
    if(!decoder) {
        NEXUS_CLIENT_RESOURCES_RELEASE(pictureDecoder,Count,NEXUS_ANY_ID);
        rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    NEXUS_OBJECT_INIT(NEXUS_PictureDecoder, decoder);
    decoder->heap = NEXUS_Heap_GetMemHandle(heap);
    decoder->index = index;
    decoder->openSettings = *pSettings;
    NEXUS_PictureDecoder_GetDefaultStartSettings(&decoder->startSettings);

    /* alloc multiscan buffer */
    decoder->multiScanBuffer = NULL;
    decoder->multiScanBufOffset = 0;
    if(pSettings->multiScanBufferSize) {
        if(pSettings->multiScanBufferPtr==NULL) {
            decoder->multiScanBuffer = BMEM_Alloc(decoder->heap, pSettings->multiScanBufferSize);
            if(!decoder->multiScanBuffer) {rc=BERR_TRACE(NEXUS_OUT_OF_DEVICE_MEMORY);goto err_multiscanbuffer;}
            decoder->multiScanBufOffset = NEXUS_AddrToOffset(decoder->multiScanBuffer);
        } else {
            decoder->multiScanBufOffset = NEXUS_AddrToOffset(pSettings->multiScanBufferPtr);
            if(decoder->multiScanBufOffset==0) {rc=BERR_TRACE(NEXUS_INVALID_PARAMETER);goto err_multiscanbuffer;}
            decoder->multiScanBuffer = NEXUS_OffsetToUncachedAddr(decoder->multiScanBufOffset);
        }
    }
    decoder->remainingBytes = 0;

    if(pSettings->bufferPtr==NULL) {
        decoder->memFifo = BMEM_Alloc(decoder->heap, pSettings->bufferSize);
        if(!decoder->memFifo) {rc=BERR_TRACE(NEXUS_OUT_OF_DEVICE_MEMORY);goto err_fifo;}
        rc = BMEM_Heap_ConvertAddressToCached(decoder->heap, decoder->memFifo, &decoder->fifo);
        if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_fifo_cached;}
    } else {
        decoder->memFifo = NULL;
        decoder->fifo = pSettings->bufferPtr;
    }

    hwState->nrSidChOpen++;

    rc = BKNI_CreateEvent(&decoder->event);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_event;}
    decoder->eventHandler = NEXUS_RegisterEvent(decoder->event, NEXUS_PictureDecoder_P_EventHandler, decoder);
    if(!decoder->eventHandler) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_event_handler;}

    rc = BKNI_CreateEvent(&decoder->eventCancel);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto err_event_cancel;}

    decoder->pictureParsedCallback = NEXUS_TaskCallback_Create(decoder, NULL);
    if(!decoder->pictureParsedCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_picture_header_callback;}

    decoder->segmentDecodedCallback = NEXUS_TaskCallback_Create(decoder, NULL);
    if(!decoder->segmentDecodedCallback) {rc=BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_segment_decoded_callback;}

    NEXUS_PictureDecoder_P_Reset(decoder);

    if ( index < SID_DECODE_CHANNELS ) {
        /* decoder handle gets a unique sid channel */
        BSID_GetOpenChannelSettings( BSID_ChannelType_eStill, &decoder->sidChOpenSettings  );

        decoder->sidChOpenSettings.e_ChannelType = BSID_ChannelType_eStill;
        decoder->sidChOpenSettings.ui32_ChannelPriority = 0;
        decoder->sidChOpenSettings.u_ChannelSpecific.still.ui32_QueueDepth = 5;
        decoder->sidChOpenSettings.u_ChannelSpecific.still.p_CallbackFunc_Isr  = (BSID_CallbackFunc)NEXUS_PictureDecoder_P_Callback_isr;
        decoder->sidChOpenSettings.u_ChannelSpecific.still.ui32_CallbackEvents =   (BSID_NotificationEvent_eGetInfoDone |
                                                          BSID_NotificationEvent_eDecodeDone |
                                                          BSID_NotificationEvent_eGetInfoError |
                                                          BSID_NotificationEvent_eDecodeError |
                                                          BSID_NotificationEvent_eWaitingForMoreInputData);
        decoder->sidChOpenSettings.u_ChannelSpecific.still.pv_CallbackData     = decoder;
        /* Setting these means no memory bounds check is performed */
        decoder->sidChOpenSettings.u_ChannelSpecific.still.s_MemoryBounds.ui32_Offset = 0x0;
        decoder->sidChOpenSettings.u_ChannelSpecific.still.s_MemoryBounds.ui32_Size   = 0x0;

        /* open sid channel*/
        rc = BSID_OpenChannel( hwState->sid, &decoder->sid, index , &decoder->sidChOpenSettings);
        if (rc != BERR_SUCCESS) {
            BDBG_WRN(("app_main: BSID_OpenChannel decoder=%p idx=%d failed err=0x%x\n", decoder, index, rc));
            goto err_segment_decoded_callback;    
        }
        hwState->sidCh = decoder->sid;
        hwState->opened[index] = decoder;
        BKNI_Memcpy( &hwState->sidChOpenSettings , &decoder->sidChOpenSettings, sizeof(BSID_OpenChannelSettings) );
        BKNI_Memset( &decoder->sidQue, 0, sizeof(NEXUS_PictureDecoder_P_SidQue) );
        decoder->active = 0;
    }
    else { 
        /* decoder handle shares sid channel, with another open decoder handle */
        BKNI_Memcpy( &decoder->sidChOpenSettings , &hwState->sidChOpenSettings , sizeof(BSID_OpenChannelSettings) );
        decoder->sidChOpenSettings.u_ChannelSpecific.still.pv_CallbackData = decoder;
        decoder->sid                              = hwState->sidCh;         /* copy sidChannel handle */
        decoder->index                            = hwState->nrSidChOpen;   /* get index count        */
        BKNI_Memset( &decoder->sidQue, 0, sizeof(NEXUS_PictureDecoder_P_SidQue) );
        decoder->active = 0;
    }
    return decoder;

err_segment_decoded_callback:
    NEXUS_TaskCallback_Destroy(decoder->pictureParsedCallback);
err_picture_header_callback:
    BKNI_DestroyEvent(decoder->eventCancel);
err_event_cancel:
    NEXUS_UnregisterEvent(decoder->eventHandler);
err_event_handler:
    BKNI_DestroyEvent(decoder->event);
err_event:
err_fifo_cached:
    if(pSettings->bufferPtr==NULL) {
        BMEM_Free(decoder->heap, decoder->memFifo);
    }
err_fifo:
    if(pSettings->multiScanBufferPtr==NULL && decoder->multiScanBuffer) {
        BMEM_Free(decoder->heap, decoder->multiScanBuffer);
    }
err_multiscanbuffer:
    NEXUS_CLIENT_RESOURCES_RELEASE(pictureDecoder,Count,NEXUS_ANY_ID);
    BDBG_OBJECT_DESTROY(decoder, NEXUS_PictureDecoder);
    BKNI_Free(decoder);
    return NULL;
}

static void
NEXUS_PictureDecoder_P_Finalizer( NEXUS_PictureDecoderHandle decoder)
{
    bool okToCloseSID = true;
    struct NEXUS_PictureDecoder_P_HWState *hwState = &g_NEXUS_PictureDecoder_P_ModuleState.hwState;
    unsigned i;

    NEXUS_OBJECT_ASSERT(NEXUS_PictureDecoder, decoder);

    if ( nexus_picturedecoder_verify(decoder,true) ) {
        NEXUS_PictureDecoder_P_Stop(decoder);
        okToCloseSID = false;  /* if decoder is sharing sid channel, we can't close sid channel now */
    }
    else {
        /* may have acquired, but not started, so always stop */
        NEXUS_PictureDecoder_Stop(decoder);
    }

    if(decoder->openSettings.multiScanBufferPtr==NULL && decoder->multiScanBuffer) {
        BMEM_Free(decoder->heap, decoder->multiScanBuffer);
    }
    if(decoder->openSettings.bufferPtr==NULL) {
        BMEM_Free(decoder->heap, decoder->memFifo);
    }

    /* close sid channel */
    if ( okToCloseSID ) {
        if ( decoder->sid ) {
            BSID_CloseChannel(decoder->sid);
    }
        else {
            BDBG_WRN(("\t\t SID chan=%p for decoder=%p idx=%d ALREADY closed !!" , decoder->sid, decoder, decoder->index ));
            BDBG_ASSERT(!decoder->sid);
        }
    }

    if ( hwState->nrSidChOpen ) {
        hwState->nrSidChOpen--;
    }

    NEXUS_TaskCallback_Destroy(decoder->segmentDecodedCallback);
    NEXUS_TaskCallback_Destroy(decoder->pictureParsedCallback);
    BKNI_DestroyEvent(decoder->eventCancel);
    NEXUS_UnregisterEvent(decoder->eventHandler);
    BKNI_DestroyEvent(decoder->event);

    for(i=0; i<SID_DECODE_CHANNELS; i++) {
    if (hwState->opened[i] == decoder) 
        hwState->opened[i] = NULL;
    }

    NEXUS_CLIENT_RESOURCES_RELEASE(pictureDecoder,Count,NEXUS_ANY_ID);
    NEXUS_OBJECT_DESTROY(NEXUS_PictureDecoder, decoder);
    BKNI_Free(decoder);

    return;
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_PictureDecoder, NEXUS_PictureDecoder_Close);

NEXUS_Error NEXUS_PictureDecoderModule_Standby_priv(bool enabled, const NEXUS_StandbySettings *pSettings)
{
#if NEXUS_POWER_MANAGEMENT
    unsigned i;
    NEXUS_Error rc = NEXUS_SUCCESS;
    struct NEXUS_PictureDecoder_P_HWState *hwState = &g_NEXUS_PictureDecoder_P_ModuleState.hwState;
    NEXUS_PictureDecoderHandle decoder;
    BSTD_UNUSED(pSettings);

    if (enabled) {
        for (i=0; i<SID_DECODE_CHANNELS; i++) {
            decoder = hwState->opened[i];
            if(!decoder) { continue; }

            /* If all channels are stopped, that ensures that the SID clock is released */
            if(decoder->state != NEXUS_PictureDecoder_P_eStopped) {
                BDBG_WRN(("Picture decoder %d:%08x needs to be stopped before entering standby", i, decoder));
                return NEXUS_NOT_SUPPORTED;
            }
        }
        rc = BSID_Standby(hwState->sid);
        if (rc!=BERR_SUCCESS) {
            rc = BERR_TRACE(rc);
        }
    } 
    else {
        rc = BSID_Resume(hwState->sid);
        if (rc!=BERR_SUCCESS) {
            rc = BERR_TRACE(rc);
        }
    }
    return rc;
#else
    BSTD_UNUSED(enabled);
    BSTD_UNUSED(pSettings);

    return NEXUS_SUCCESS;
#endif    
}    
