/***************************************************************************
 *     (c)2009-2012 Broadcom Corporation
 *
 *  This program is the proprietary decoder.software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  .  Except as set forth in an Authorized License, Broadcom grants
 *  no license , right to use, or waiver of any kind with respect to the
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
 *  LICENSORS BE LIABLE FOR  CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR  ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_video_decoder_soft.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 9/7/12 11:56a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/softdecode_ext/src/nexus_video_decoder_soft.c $
 * 
 * 3   9/7/12 11:56a erickson
 * SW7435-349: add standard header
 * 
 * 2   5/13/11 4:13p erickson
 * SWDTV-6386: add "nx_" namespace prefix for internal nexus threads
 * 
 * 1   9/8/10 11:47a vsilyaev
 * SW7468-129: Added video decoder on ZSP
 * 
 * SW7468-129/5   3/9/10 8:01p vsilyaev
 * SW7468-129: Used static functions where aprorpiate
 * 
 * SW7468-129/4   3/9/10 7:14p vsilyaev
 * SW7468-129: Added functional software video decoder
 * 
 * SW7468-129/3   3/8/10 6:33p vsilyaev
 * SW7468-129: Added basic implementation for video audio decoder
 * 
 * SW7468-129/2   3/8/10 12:49p vsilyaev
 * SW7468-129: Added private interface between audio and video decoders
 * 
 * SW7468-129/1   3/5/10 7:30p vsilyaev
 * SW7468-129: Added hooks for soft video decoder
 * 
 **************************************************************************/
#include "nexus_video_decoder_module.h"
#include "nexus_still_decoder_impl.h"
#include "priv/nexus_core.h"

BDBG_MODULE(nexus_video_decoder_soft);
BDBG_OBJECT_ID(NEXUS_VideoDecoder_Soft);

#define BDBG_MSG_TRACE(x) BDBG_MSG(x)

#define NOT_SUPPORTED(x) NULL
static const NEXUS_VideoDecoder_P_Interface NEXUS_VideoDecoder_P_Interface_Soft;

extern b_video_softdecode_t bffmpeg_video_decode_create(void);


static NEXUS_Error NEXUS_VideoDecoder_P_SetSettings_Soft( NEXUS_VideoDecoderHandle decoder, const NEXUS_VideoDecoderSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

static NEXUS_Error NEXUS_VideoDecoder_P_Start_Soft( NEXUS_VideoDecoderHandle decoder, const NEXUS_VideoDecoderStartSettings *pSettings)
{
    NEXUS_Error  rc;
    NEXUS_StcChannelCallbacks callbacks;
    BAVC_VideoCompressionStd videoCmprStd;
    NEXUS_RaveStatus raveStatus;
    bool playback = false;
    unsigned stcChannelIndex;

    BDBG_MSG(("NEXUS_VideoDecoder_P_Start_Soft:%#x", (unsigned)decoder));
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);


    rc = NEXUS_VideoDecoder_P_Start_Generic_Prologue(decoder, pSettings);
    if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}


    LOCK_TRANSPORT();
    NEXUS_StcChannel_GetDefaultCallbacks_priv(&callbacks);
    callbacks.pDevContext = decoder;
    UNLOCK_TRANSPORT();

    rc = NEXUS_VideoDecoder_P_Start_Generic_Body(decoder, pSettings, false, &videoCmprStd, &raveStatus, &callbacks, &playback, &stcChannelIndex);
    if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}

    rc = NEXUS_VideoDecoder_P_Xdm_Start(decoder);
    if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}

    decoder->decoder.soft.data.frame_cnt = 0;
    decoder->decoder.soft.data.fin = fopen("soft.dat", "rb");
    if(!decoder->decoder.soft.data.fin) { return BERR_TRACE(NEXUS_NOT_SUPPORTED);}

    rc = decoder->decoder.soft.soft_decode->methods->start(decoder->decoder.soft.soft_decode, pSettings->codec);
    if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}

    BKNI_AcquireMutex(decoder->decoder.soft.lock);
    decoder->decoder.soft.paused = false;
    BKNI_ReleaseMutex(decoder->decoder.soft.lock);

    rc = NEXUS_VideoDecoder_P_Start_Generic_Epilogue(decoder, pSettings);
    if(rc!=NEXUS_SUCCESS) {return BERR_TRACE(rc);}

    return NEXUS_SUCCESS;
}

static void NEXUS_VideoDecoder_P_Stop_Soft( NEXUS_VideoDecoderHandle decoder)
{
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);
}

static void NEXUS_VideoDecoder_P_Flush_Soft( NEXUS_VideoDecoderHandle decoder)
{
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);
}


static void NEXUS_VideoDecoder_P_FrameQueue_Init(NEXUS_VideoDecoder_P_FrameQueue *fq)
{
    BLST_SQ_INIT(&fq->list);
    fq->count = 0;
    return;
}

static void NEXUS_VideoDecoder_P_FrameQueue_Add(NEXUS_VideoDecoder_P_FrameQueue *fq, NEXUS_VideoDecoder_P_FrameElement *f)
{
    BLST_SQ_INSERT_TAIL(&fq->list, f, link);
    fq->count++;
    return;
}

static void NEXUS_VideoDecoder_P_FrameQueue_Remove(NEXUS_VideoDecoder_P_FrameQueue *fq, NEXUS_VideoDecoder_P_FrameElement *f)
{
    BDBG_ASSERT(fq->count>0);
    BLST_SQ_REMOVE(&fq->list, f, NEXUS_VideoDecoder_P_FrameElement, link);
    fq->count--;
    return;
}

static void * NEXUS_VideoDecoder_P_Soft_Cached2Uncached(NEXUS_VideoDecoderHandle decoder, void *addr)
{
    return (uint8_t *)decoder->decoder.soft.frameMemory + ((uint8_t *)addr - (uint8_t *)decoder->decoder.soft.frameMemoryCached);
}
#define STRIPE_WIDTH_128    (128)

struct NEXUS_VideoDecoder_P_BufferInfo {
    unsigned lumaSize;
    unsigned chromaSize;
   	uint32_t                 ulLuminanceNMBY;
	uint32_t                 ulChrominanceNMBY;
	uint32_t                 ulStripeWidth;
	unsigned long ulNumOfStripes, ulTotalByteWidth, ulTotalLumaHeight, ulTotalChromaHeight;
};

static void NEXUS_VideoDecoder_P_GetBufferInformation(unsigned width, unsigned height, struct NEXUS_VideoDecoder_P_BufferInfo *info)
{
    /* from ../../rockford/unittests/portinginterface/vdc/static_buffer.c */
	unsigned long ulNumOfStripes, ulTotalByteWidth, ulTotalLumaHeight, ulTotalChromaHeight;
	unsigned long             ulWidth = width;
	unsigned long             ulHeight = height;
	uint32_t                  ulStripeWidth = 64;
	unsigned long ulBankHeight, ulBankRemainder, ulBankAdd;
	unsigned long ulLumaBufSize, ulChromaBufSize;
   	uint32_t                 ulLuminanceNMBY;
	uint32_t                 ulChrominanceNMBY;
	bool                      bChroma422 = false;

    {
		/* Determine number of vertical stripes for both luma and chroma
		   buffers. ulStripeWidth is the width of the stripe in bytes */
		ulNumOfStripes = (ulWidth + (ulStripeWidth-1)) / ulStripeWidth;
		ulTotalByteWidth = ulStripeWidth * ulNumOfStripes;

		/* Determine the height of the luma stripe. Height is broken down
		   into banks of 16 pixels high. */
		ulBankHeight = (ulHeight + 15)/16;
#if (BCHP_CHIP == 7405) || (BCHP_CHIP == 7325) || (BCHP_CHIP == 7335) || \
	(BCHP_CHIP == 7336) || (BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)
		if(ulStripeWidth == STRIPE_WIDTH_128)
		{
			/* NOTE: NMBY must be 16 * N + 8; */
			ulBankRemainder = ulBankHeight & 15;
			/* 0 + 4; 1 + 3; 2 + 2; 3 + 1; 4 + 0; 5 + 7; 6 + 6; 7 + 5*/
			ulBankAdd = (8 - ulBankRemainder) & 15;
		}
		else
		{
			/* NOTE: NMBY must be 8 * N + 4; */
			ulBankRemainder = ulBankHeight & 7;
			/* 0 + 4; 1 + 3; 2 + 2; 3 + 1; 4 + 0; 5 + 7; 6 + 6; 7 + 5*/
			ulBankAdd = (4 - ulBankRemainder) & 7;
		}
#elif (BCHP_CHIP == 7420)
	/* NOTE: NMBY must be 8 * N + 4; */
	ulBankRemainder = ulBankHeight & 7;
	/* 0 + 4; 1 + 3; 2 + 2; 3 + 1; 4 + 0; 5 + 7; 6 + 6; 7 + 5*/
	ulBankAdd = (4 - ulBankRemainder) & 7;
#else
		/* NOTE: NMBY must be 4 * N + 2; */
		ulBankRemainder = ulBankHeight & 3;
		/* 0 + 2; 1 + 1; 2 + 0; 3 + 3; */
		ulBankAdd = (2 - ulBankRemainder) & 3;
#endif
		ulTotalLumaHeight = (ulBankHeight + ulBankAdd) * 16;
		ulLuminanceNMBY = (ulBankHeight + ulBankAdd);

		/* Determine the height of the chroma stript. When format is
		   4:2:0 then chroma is based on 1/2 pixel height. When format is
		   4:2:2 then chroma is based on pixel height */
		ulBankHeight = ((ulHeight >> (1 - bChroma422)) + 15) / 16;
#if (BCHP_CHIP == 7405) || (BCHP_CHIP == 7325) || (BCHP_CHIP == 7335) || \
	(BCHP_CHIP == 7336) || (BCHP_CHIP == 3548) || (BCHP_CHIP == 3556)
		if(ulStripeWidth == STRIPE_WIDTH_128)
		{
			ulBankRemainder = ulBankHeight & 15;
			ulBankAdd = (8 - ulBankRemainder) & 15;
		}
		else
		{
			ulBankRemainder = ulBankHeight & 7;
			ulBankAdd = (4 - ulBankRemainder) & 7;
		}
#elif (BCHP_CHIP == 7420)
		ulBankRemainder = ulBankHeight & 7;
		ulBankAdd = (4 - ulBankRemainder) & 7;
#else
		ulBankRemainder = ulBankHeight & 3;
		ulBankAdd = (2 - ulBankRemainder) & 3;
#endif
		ulTotalChromaHeight = (ulBankHeight + ulBankAdd) * 16;
		ulChrominanceNMBY = (ulBankHeight + ulBankAdd);

		/* Calculate size of luma and chroma buffers in bytes */
		ulLumaBufSize   = ulTotalByteWidth*ulTotalLumaHeight;
		ulChromaBufSize = ulTotalByteWidth*ulTotalChromaHeight;
    }

    info->lumaSize = ulLumaBufSize;
    info->chromaSize = ulChromaBufSize;
    info->ulLuminanceNMBY = ulLuminanceNMBY;
    info->ulChrominanceNMBY = ulChrominanceNMBY;
    info->ulStripeWidth = ulStripeWidth;
    info->ulNumOfStripes = ulNumOfStripes;
    info->ulTotalByteWidth = ulTotalByteWidth;
    info->ulTotalLumaHeight = ulTotalLumaHeight;
    info->ulTotalChromaHeight = ulTotalChromaHeight;

    return;
}

#define B_SWAP_ADDR_LUMA(x) (((x) & (~3)) + (3 - ((x) & 0x3)))
#define B_SWAP_ADDR_CHROMA(x) (((x) & (~2)) + (2 - ((x) & 0x2)))

static void NEXUS_VideoDecoder_P_StripeLuma(uint8_t *dest, const b_avcodec_plane *src, unsigned width, unsigned height,const struct NEXUS_VideoDecoder_P_BufferInfo *info)
{
    unsigned y;
    unsigned ulTotalLumaHeight = info->ulTotalLumaHeight;

    for(y=0;y<height;y++) {
        const uint8_t *line = (uint8_t *)src->buf + y*src->stride;
        unsigned x;
        for(x=0;x<width;x++) {
	        const unsigned ulStripeWidth = 64;
            const unsigned ulStripeNum = x / ulStripeWidth;
            unsigned offset = (x % ulStripeWidth) +
						(y * ulStripeWidth) +
						(ulStripeNum * ulStripeWidth * ulTotalLumaHeight);
            dest[B_SWAP_ADDR_LUMA(offset)] = line[x];
        }
    }
    return;
}

static void NEXUS_VideoDecoder_P_StripeChroma(uint8_t *dest, const b_avcodec_plane *u, const b_avcodec_plane *v, unsigned width, unsigned height, const struct NEXUS_VideoDecoder_P_BufferInfo *info)
{
    unsigned y;
    unsigned ulTotalChromaHeight = info->ulTotalChromaHeight;

    width/=2;
    height/=2;
    for(y=0;y<height;y++) {
        unsigned x;
        const uint8_t *line_u = (uint8_t *)u->buf + y*u->stride;
        const uint8_t *line_v = (uint8_t *)v->buf + y*v->stride;
        for(x=0;x<width;x++) {
	        const unsigned ulStripeWidth = 64;
            const unsigned ulStripeNum = (2*x) / ulStripeWidth;
            unsigned offset= ((2*x) % ulStripeWidth) +
							 (y  * ulStripeWidth) +
							(ulStripeNum * ulStripeWidth * ulTotalChromaHeight);

            dest[B_SWAP_ADDR_CHROMA(offset)] = line_v[x];
            dest[B_SWAP_ADDR_CHROMA(offset)+1] = line_u[x]; 
        }
    }
    return;
}
 
static void NEXUS_VideoDecoder_P_ConvertFrame(NEXUS_VideoDecoderHandle decoder, NEXUS_VideoDecoder_P_FrameElement *frame, const b_avcodec_frame *soft_frame)
{
    struct NEXUS_VideoDecoder_P_BufferInfo bufferInfo;
    size_t luma_size, chroma_size;

    NEXUS_VideoDecoder_P_GetBufferInformation(soft_frame->width, soft_frame->height, &bufferInfo);
    luma_size = bufferInfo.lumaSize;
    chroma_size = bufferInfo.chromaSize;

    frame->picture.stBufferInfo.pLumaBufferVirtualAddress = frame->buffer.buf;
    frame->picture.stBufferInfo.pChromaBufferVirtualAddress = (uint8_t *)frame->buffer.buf + luma_size;
    frame->picture.stBufferInfo.hHeap = g_pCoreHandles->heap[g_NEXUS_videoDecoderModuleSettings.hostAccessibleHeapIndex];
    frame->picture.stBufferInfo.stSource.uiWidth = soft_frame->width;
    frame->picture.stBufferInfo.stSource.uiHeight = soft_frame->height;
    frame->picture.stBufferInfo.stSource.bValid = true;
    frame->picture.stBufferInfo.stDisplay = frame->picture.stBufferInfo.stSource;
    frame->picture.stBufferInfo.eStripeWidth = BAVC_StripeWidth_e64Byte;
    frame->picture.stBufferInfo.uiLumaStripeHeight = bufferInfo.ulLuminanceNMBY << 4;
    frame->picture.stBufferInfo.uiChromaStripeHeight = bufferInfo.ulChrominanceNMBY << 4;
    frame->picture.stBufferInfo.stChromaLocation[BAVC_Polarity_eFrame].bValid = true;
    frame->picture.stBufferInfo.stChromaLocation[BAVC_Polarity_eFrame].eMpegType = BAVC_MpegType_eMpeg2;
    frame->picture.stBufferInfo.stChromaLocation[BAVC_Polarity_eTopField].bValid = true;
    frame->picture.stBufferInfo.stChromaLocation[BAVC_Polarity_eTopField].eMpegType= BAVC_MpegType_eMpeg2;
    frame->picture.stBufferInfo.stChromaLocation[BAVC_Polarity_eBotField].bValid = true;
    frame->picture.stBufferInfo.stChromaLocation[BAVC_Polarity_eBotField].eMpegType= BAVC_MpegType_eMpeg2;
    frame->picture.stBufferInfo.eYCbCrType = BAVC_YCbCrType_e4_2_0;
    frame->picture.stBufferInfo.ePulldown = BXDM_Picture_PullDown_eFrameX1;
    frame->picture.stBufferInfo.eSourceFormat = BXDM_Picture_SourceFormat_eProgressive;
    frame->picture.stBufferInfo.bValid = true;
    frame->picture.stFrameRate.eType = BXDM_Picture_FrameRateType_eFixed;
    frame->picture.stFrameRate.stRate.uiNumerator = 25000;
    frame->picture.stFrameRate.stRate.uiDenominator = 1000;
    frame->picture.stFrameRate.bValid = true;

    NEXUS_VideoDecoder_P_StripeLuma(frame->picture.stBufferInfo.pLumaBufferVirtualAddress, &soft_frame->y, soft_frame->width, soft_frame->height, &bufferInfo);
    NEXUS_VideoDecoder_P_StripeChroma(frame->picture.stBufferInfo.pChromaBufferVirtualAddress, &soft_frame->u, &soft_frame->v, soft_frame->width, soft_frame->height, &bufferInfo);
    NEXUS_FlushCache(frame->buffer.buf, luma_size+chroma_size);
    frame->picture.stBufferInfo.pLumaBufferVirtualAddress = NEXUS_VideoDecoder_P_Soft_Cached2Uncached(decoder, frame->picture.stBufferInfo.pLumaBufferVirtualAddress);
    frame->picture.stBufferInfo.pChromaBufferVirtualAddress = NEXUS_VideoDecoder_P_Soft_Cached2Uncached(decoder, frame->picture.stBufferInfo.pChromaBufferVirtualAddress);
    return;

}

static void NEXUS_VideoDecoder_P_Soft_DecoderThread(void *context)
{
    NEXUS_VideoDecoderHandle decoder = context;

    BDBG_MSG(("NEXUS_VideoDecoder_P_Soft_DecoderThread:%#x", (unsigned)decoder));
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);
    for(;;) {
        NEXUS_VideoDecoder_P_FrameElement *frame;
        BKNI_AcquireMutex(decoder->decoder.soft.lock);
        if(decoder->decoder.soft.stop) {
            BKNI_ReleaseMutex(decoder->decoder.soft.lock);
            break;
        }
        if(decoder->decoder.soft.paused) {
            BKNI_ReleaseMutex(decoder->decoder.soft.lock);
            goto wait;
        }
        BKNI_ReleaseMutex(decoder->decoder.soft.lock);

        BKNI_EnterCriticalSection();
        frame = BLST_SQ_FIRST(&decoder->decoder.soft.free);
        if(frame) {
            BLST_SQ_REMOVE_HEAD(&decoder->decoder.soft.free, link);
        }
        BKNI_LeaveCriticalSection();
        if(frame==NULL) {
            goto wait;
        }
        {
            uint32_t frame_size;
            uint8_t frame_data[32768];
            int read_rc;
            b_avcodec_frame soft_frame;
            NEXUS_Error rc;
            b_avcodec_compressed_frame compressed;
            NEXUS_VideoDecoder_P_FrameElement *temp_frame = &decoder->decoder.soft.frames[0]; /* always use frame 0 as a temporary for the  linear scan soft decoders output */

            read_rc = fread(&frame_size, sizeof(frame_size), 1, decoder->decoder.soft.data.fin);
            if(read_rc<0) { goto recycle_and_wait;}
            BDBG_MSG(("NEXUS_VideoDecoder_P_Soft_DecoderThread:%#x frame_size:%u", (unsigned)decoder, frame_size));
            BDBG_ASSERT(frame_size<=sizeof(frame_data));
            read_rc = fread(frame_data, 1, frame_size, decoder->decoder.soft.data.fin);
            if(read_rc!=(int)frame_size) { goto wait;}
            compressed.buf = frame_data;
            compressed.size = frame_size;
            compressed.pts_valid = true;
            compressed.pts = 45 * (decoder->decoder.soft.data.frame_cnt * 33);
            rc = decoder->decoder.soft.soft_decode->methods->decode(decoder->decoder.soft.soft_decode, &compressed, &temp_frame->buffer, &soft_frame);
            if(rc!=NEXUS_SUCCESS) {
                rc=BERR_TRACE(rc);
                goto recycle_and_wait;
            }
            BDBG_MSG(("NEXUS_VideoDecoder_P_Soft_DecoderThread:%#x %u frame:%uX%u %#x", (unsigned)decoder, frame - decoder->decoder.soft.frames, soft_frame.width, soft_frame.height, frame->picture.stBufferInfo.pLumaBufferVirtualAddress));
            NEXUS_VideoDecoder_P_ConvertFrame(decoder, frame, &soft_frame);
            BKNI_EnterCriticalSection();
            NEXUS_VideoDecoder_P_FrameQueue_Add(&decoder->decoder.soft.ready, frame);
            BKNI_LeaveCriticalSection();
            continue;
        }
recycle_and_wait:
        BKNI_EnterCriticalSection();
        BLST_SQ_INSERT_HEAD(&decoder->decoder.soft.free, frame, link);
        BKNI_LeaveCriticalSection();
wait:
        BKNI_WaitForEvent(decoder->decoder.soft.event, 100);
        continue;
    }
    return;
}

static BERR_Code NEXUS_VideoDecoder_P_Soft_GetPictureCount_isr(void *context, uint32_t *pictureCount)
{
    NEXUS_VideoDecoderHandle decoder = context;
    BDBG_MSG_TRACE(("NEXUS_VideoDecoder_P_Soft_GetPictureCount_isr>:%#x", (unsigned)decoder));
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);
    *pictureCount = decoder->decoder.soft.ready.count;
    BDBG_MSG_TRACE(("NEXUS_VideoDecoder_P_Soft_GetPictureCount_isr<:%#x %u", (unsigned)decoder, *pictureCount));
    return BERR_SUCCESS;
}

static BERR_Code NEXUS_VideoDecoder_P_Soft_PeekAtPicture_isr(void *context, uint32_t index, const BXDM_Picture **pUnifiedPicture)
{
    NEXUS_VideoDecoderHandle decoder = context;
    const NEXUS_VideoDecoder_P_FrameElement *frame;
    BDBG_MSG_TRACE(("NEXUS_VideoDecoder_P_Soft_PeekAtPicture_isr:%#x %u", (unsigned)decoder, index));
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);
    for(frame=BLST_SQ_FIRST(&decoder->decoder.soft.ready.list);frame && index>0;index--) {
        frame = BLST_SQ_NEXT(frame, link);
    }
    if(frame==NULL) {
        *pUnifiedPicture = NULL;
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    *pUnifiedPicture = &frame->picture;
    return BERR_SUCCESS;
}

static BERR_Code NEXUS_VideoDecoder_P_Soft_GetNextPicture_isr(void *context, const BXDM_Picture **pUnifiedPicture)
{
    NEXUS_VideoDecoderHandle decoder = context;
    NEXUS_VideoDecoder_P_FrameElement *frame;

    BDBG_MSG_TRACE(("NEXUS_VideoDecoder_P_Soft_GetNextPicture_isr>:%#x", (unsigned)decoder));
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);
    while(NULL!=(frame=BLST_SQ_FIRST(&decoder->decoder.soft.recycled))) {
        BLST_SQ_REMOVE_HEAD(&decoder->decoder.soft.recycled, link);
        BLST_SQ_INSERT_TAIL(&decoder->decoder.soft.free, frame, link);
    }
    frame=BLST_SQ_FIRST(&decoder->decoder.soft.ready.list);
    if(frame==NULL) {
        *pUnifiedPicture = NULL;
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    NEXUS_VideoDecoder_P_FrameQueue_Remove(&decoder->decoder.soft.ready, frame);
    BLST_SQ_INSERT_TAIL(&decoder->decoder.soft.display, frame, link);
    *pUnifiedPicture = &frame->picture;
    BDBG_MSG_TRACE(("NEXUS_VideoDecoder_P_Soft_GetNextPicture_isr<:%#x %#x %#x[%u]", (unsigned)decoder, (unsigned)frame, (unsigned)frame->picture.stBufferInfo.pLumaBufferVirtualAddress, frame - decoder->decoder.soft.frames));
    return BERR_SUCCESS;
}

static BERR_Code NEXUS_VideoDecoder_P_Soft_ReleasePicture_isr(void *context, const BXDM_Picture *pUnifiedPicture, const BXDM_Decoder_ReleasePictureInfo *pReleasePictureInfo)
{
    NEXUS_VideoDecoderHandle decoder = context;
    NEXUS_VideoDecoder_P_FrameElement *frame = (void *)pUnifiedPicture;
    BDBG_MSG_TRACE(("NEXUS_VideoDecoder_P_Soft_ReleasePicture_isr>:%#x %#x %#x", (unsigned)decoder, (unsigned)pUnifiedPicture, (unsigned)pReleasePictureInfo));
    BSTD_UNUSED(pReleasePictureInfo);
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);
    BLST_SQ_REMOVE(&decoder->decoder.soft.display, frame, NEXUS_VideoDecoder_P_FrameElement, link);
    BLST_SQ_INSERT_TAIL(&decoder->decoder.soft.recycled, frame, link); 
    BDBG_MSG_TRACE(("NEXUS_VideoDecoder_P_Soft_ReleasePicture_isr<:%#x %#x %#x", (unsigned)decoder, (unsigned)pUnifiedPicture, (unsigned)pReleasePictureInfo));
    return BERR_SUCCESS;
}

static const BXDM_Decoder_Interface NEXUS_VideoDecoder_P_Soft_DecoderInterface = {
    NEXUS_VideoDecoder_P_Soft_GetPictureCount_isr,
    NEXUS_VideoDecoder_P_Soft_PeekAtPicture_isr,
    NEXUS_VideoDecoder_P_Soft_GetNextPicture_isr,
    NEXUS_VideoDecoder_P_Soft_ReleasePicture_isr,
    NULL,
    NULL,
    NULL
};

#define NEXUS_VIDEO_DECODER_P_SOFT_DEFAULT_WIDTH    720
#define NEXUS_VIDEO_DECODER_P_SOFT_DEFAULT_HEIGHT   480


#if 0
static void 
NEXUS_VideoDecoder_P_Soft_BuildBlackFrame(NEXUS_VideoDecoderHandle decoder,NEXUS_VideoDecoder_P_FrameElement *frame, unsigned luma, unsigned chroma)
{
    struct NEXUS_VideoDecoder_P_BufferInfo bufferInfo;
    size_t luma_size, chroma_size;

    NEXUS_VideoDecoder_P_GetBufferInformation(NEXUS_VIDEO_DECODER_P_SOFT_DEFAULT_WIDTH, NEXUS_VIDEO_DECODER_P_SOFT_DEFAULT_HEIGHT, &bufferInfo);
    luma_size = bufferInfo.lumaSize;
    chroma_size = bufferInfo.chromaSize;
    frame->picture.stBufferInfo.pLumaBufferVirtualAddress = frame->buffer.buf;
    frame->picture.stBufferInfo.pChromaBufferVirtualAddress = (uint8_t *)frame->buffer.buf + luma_size;
    frame->picture.stBufferInfo.hHeap = g_pCoreHandles->heap[g_NEXUS_videoDecoderModuleSettings.hostAccessibleHeapIndex];
    frame->picture.stBufferInfo.stSource.uiWidth = NEXUS_VIDEO_DECODER_P_SOFT_DEFAULT_WIDTH;
    frame->picture.stBufferInfo.stSource.uiHeight = NEXUS_VIDEO_DECODER_P_SOFT_DEFAULT_HEIGHT;
    frame->picture.stBufferInfo.stSource.bValid = true;
    frame->picture.stBufferInfo.stDisplay = frame->picture.stBufferInfo.stSource;
    frame->picture.stBufferInfo.eStripeWidth = BAVC_StripeWidth_e64Byte;
    frame->picture.stBufferInfo.uiLumaStripeHeight = bufferInfo.ulLuminanceNMBY << 4;
    frame->picture.stBufferInfo.uiChromaStripeHeight = bufferInfo.ulChrominanceNMBY << 4;
    frame->picture.stBufferInfo.stChromaLocation[BAVC_Polarity_eFrame].bValid = true;
    frame->picture.stBufferInfo.stChromaLocation[BAVC_Polarity_eFrame].eMpegType = BAVC_MpegType_eMpeg2;
    frame->picture.stBufferInfo.stChromaLocation[BAVC_Polarity_eTopField].bValid = true;
    frame->picture.stBufferInfo.stChromaLocation[BAVC_Polarity_eTopField].eMpegType= BAVC_MpegType_eMpeg2;
    frame->picture.stBufferInfo.stChromaLocation[BAVC_Polarity_eBotField].bValid = true;
    frame->picture.stBufferInfo.stChromaLocation[BAVC_Polarity_eBotField].eMpegType= BAVC_MpegType_eMpeg2;
    frame->picture.stBufferInfo.eYCbCrType = BAVC_YCbCrType_e4_2_0;
    frame->picture.stBufferInfo.ePulldown = BXDM_Picture_PullDown_eFrameX1;
    frame->picture.stBufferInfo.eSourceFormat = BXDM_Picture_SourceFormat_eProgressive;
    frame->picture.stBufferInfo.bValid = true;
    frame->picture.stFrameRate.eType = BXDM_Picture_FrameRateType_eFixed;
    frame->picture.stFrameRate.stRate.uiNumerator = 25000;
    frame->picture.stFrameRate.stRate.uiDenominator = 1000;
    frame->picture.stFrameRate.bValid = true;

    BKNI_Memset(frame->picture.stBufferInfo.pLumaBufferVirtualAddress, luma, luma_size);
    BKNI_Memset(frame->picture.stBufferInfo.pChromaBufferVirtualAddress, chroma, chroma_size);
    NEXUS_FlushCache(frame->buffer.buf, luma_size+chroma_size);
    frame->picture.stBufferInfo.pLumaBufferVirtualAddress = NEXUS_VideoDecoder_P_Soft_Cached2Uncached(decoder, frame->picture.stBufferInfo.pLumaBufferVirtualAddress);
    frame->picture.stBufferInfo.pChromaBufferVirtualAddress = NEXUS_VideoDecoder_P_Soft_Cached2Uncached(decoder, frame->picture.stBufferInfo.pChromaBufferVirtualAddress);
    return;
}
#endif

NEXUS_VideoDecoderHandle 
NEXUS_VideoDecoder_P_Open_Soft( unsigned index, const NEXUS_VideoDecoderOpenSettings *pOpenSettings)
{
    NEXUS_VideoDecoderHandle decoder;
    NEXUS_VideoDecoderOpenMosaicSettings mosaicSettings;
    NEXUS_Error rc;
    NEXUS_RaveOpenSettings raveSettings;
    BMEM_Heap_Handle heap = g_pCoreHandles->heap[g_NEXUS_videoDecoderModuleSettings.hostAccessibleHeapIndex];
    struct NEXUS_VideoDecoder_P_BufferInfo bufferInfo;
    size_t frame_size;
    unsigned i;
    NEXUS_ThreadSettings threadSettings;

    BDBG_MSG(("NEXUS_VideoDecoder_P_Open_Soft:%u", index));

    BSTD_UNUSED(index);
    decoder = BKNI_Malloc(sizeof(*decoder));
    if(!decoder) { (void)BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
    BKNI_Memset(decoder, 0, sizeof(*decoder));
    BDBG_OBJECT_SET(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);
    decoder->interface = &NEXUS_VideoDecoder_P_Interface_Soft;

    NEXUS_VideoDecoder_P_GetMosaicOpenSettings(&mosaicSettings, pOpenSettings);
    LOCK_TRANSPORT();
    NEXUS_Rave_GetDefaultOpenSettings_priv(&raveSettings);
    UNLOCK_TRANSPORT();
    raveSettings.config.Cdb.Length = (3*1024*1024)/2;
    raveSettings.config.Itb.Length = (512*1024);

    rc = NEXUS_VideoDecoder_P_Init_Generic(decoder, &raveSettings, &mosaicSettings);
    if(rc!=NEXUS_SUCCESS) {rc=BERR_TRACE(rc);goto error;}

    rc = BKNI_CreateMutex(&decoder->decoder.soft.lock);
    if(rc!=BERR_SUCCESS){(void)BERR_TRACE(rc);goto error;}
    rc = BKNI_CreateEvent(&decoder->decoder.soft.event);
    if(rc!=BERR_SUCCESS){(void)BERR_TRACE(rc);goto error;}

    decoder->decoder.soft.paused = true;
    decoder->decoder.soft.stop = false;
    decoder->decoder.soft.soft_decode = bffmpeg_video_decode_create();
    if(!decoder->decoder.soft.soft_decode) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto error;}

    NEXUS_VideoDecoder_P_GetBufferInformation(NEXUS_VIDEO_DECODER_P_SOFT_DEFAULT_WIDTH, NEXUS_VIDEO_DECODER_P_SOFT_DEFAULT_HEIGHT, &bufferInfo);
    frame_size = bufferInfo.lumaSize + bufferInfo.chromaSize;
    decoder->decoder.soft.frameMemory = BMEM_Alloc(heap, frame_size*(NEXUS_VIDEO_DECODER_P_SOFTDECODE_N_FRAMES));
    if(!decoder->decoder.soft.frameMemory) { rc=BERR_TRACE(NEXUS_OUT_OF_DEVICE_MEMORY);goto error;}
    rc = BMEM_Heap_ConvertAddressToCached(heap, decoder->decoder.soft.frameMemory, &decoder->decoder.soft.frameMemoryCached);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}

    rc = NEXUS_VideoDecoder_P_Xdm_Initialize(decoder, &NEXUS_VideoDecoder_P_Soft_DecoderInterface, decoder);
    if(rc!=BERR_SUCCESS) {rc=BERR_TRACE(rc);goto error;}

    BLST_SQ_INIT(&decoder->decoder.soft.display);
    BLST_SQ_INIT(&decoder->decoder.soft.recycled);
    BLST_SQ_INIT(&decoder->decoder.soft.free);
    NEXUS_VideoDecoder_P_FrameQueue_Init(&decoder->decoder.soft.ready);
    for(i=0;i<NEXUS_VIDEO_DECODER_P_SOFTDECODE_N_FRAMES;i++) {
        decoder->decoder.soft.frames[i].buffer.buf = (uint8_t *)decoder->decoder.soft.frameMemoryCached + i*frame_size;
        decoder->decoder.soft.frames[i].buffer.size = frame_size;
    }
    i=0;
#if 0
    for(i=0;i<0;i++) {
        NEXUS_VideoDecoder_P_Soft_BuildBlackFrame(decoder, &decoder->decoder.soft.frames[i], 0x60+0x20*i, 0x80);
        NEXUS_VideoDecoder_P_FrameQueue_Add(&decoder->decoder.soft.ready, &decoder->decoder.soft.frames[i]);
    }
#endif

    for(i++;i<NEXUS_VIDEO_DECODER_P_SOFTDECODE_N_FRAMES;i++) {
        BLST_SQ_INSERT_TAIL(&decoder->decoder.soft.free, &decoder->decoder.soft.frames[i], link);
    }

    NEXUS_Thread_GetDefaultSettings(&threadSettings);
    decoder->decoder.soft.thread = NEXUS_Thread_Create("nx_soft_video_decode", NEXUS_VideoDecoder_P_Soft_DecoderThread, decoder, &threadSettings);
    if(!decoder->decoder.soft.thread) {rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);goto error;}

    return decoder;
error:
    NEXUS_VideoDecoder_Close(decoder);
err_alloc:
    return NULL;
}

static void 
NEXUS_VideoDecoder_P_Close_Soft( NEXUS_VideoDecoderHandle decoder)
{
    BMEM_Heap_Handle heap = g_pCoreHandles->heap[g_NEXUS_videoDecoderModuleSettings.hostAccessibleHeapIndex];

    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);
    NEXUS_VideoDecoder_P_Close_Generic(decoder);
    if(decoder->decoder.soft.thread) {
        BKNI_AcquireMutex(decoder->decoder.soft.lock);
        decoder->decoder.soft.stop = true;
        BKNI_ReleaseMutex(decoder->decoder.soft.lock);
        BKNI_SetEvent(decoder->decoder.soft.event);
        NEXUS_Thread_Destroy(decoder->decoder.soft.thread);
    }

    NEXUS_VideoDecoder_P_Xdm_Shutdown(decoder);

    if(decoder->decoder.soft.frameMemory) {
        BMEM_Free(heap, decoder->decoder.soft.frameMemory);
    }
    if(decoder->decoder.soft.lock) {
        BKNI_DestroyMutex(decoder->decoder.soft.lock);
    }
    if(decoder->decoder.soft.event) {
        BKNI_DestroyEvent(decoder->decoder.soft.event);
    }
    if(decoder->decoder.soft.soft_decode) {
        decoder->decoder.soft.soft_decode->methods->destroy(decoder->decoder.soft.soft_decode);
    }
    BDBG_OBJECT_DESTROY(decoder, NEXUS_VideoDecoder);
    BKNI_Free(decoder);
    return;
}

NEXUS_Error
NEXUS_VideoDecoderModule_P_Init_Soft(const NEXUS_VideoDecoderModuleSettings *pSettings)
{
    if(pSettings->audio==NULL) {
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }
    return BERR_SUCCESS;
}

void 
NEXUS_VideoDecoderModule_P_Uninit_Soft(void)
{
    return;
}

static void NEXUS_VideoDecoder_GetSourceId_priv_Soft(NEXUS_VideoDecoderHandle decoder, BAVC_SourceId *pSource)
{
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);
    NEXUS_ASSERT_MODULE();
    *pSource = BAVC_SourceId_eMpeg0;
    return;
}

static void NEXUS_VideoDecoder_GetDisplayConnection_priv_Soft(NEXUS_VideoDecoderHandle decoder, NEXUS_VideoDecoderDisplayConnection *pConnection)
{
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);
    NEXUS_ASSERT_MODULE();
    *pConnection = decoder->displayConnection;
}

static NEXUS_Error NEXUS_VideoDecoder_P_GetStatus_Soft( NEXUS_VideoDecoderHandle decoder, NEXUS_VideoDecoderStatus *pStatus)
{
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);
    NEXUS_VideoDecoder_P_GetStatus_Generic(decoder, pStatus);
    return NEXUS_VideoDecoder_P_GetStatus_Generic_Xdm(decoder, pStatus);
}


static NEXUS_Error NEXUS_VideoDecoder_P_SetTrickState_Soft( NEXUS_VideoDecoderHandle decoder, const NEXUS_VideoDecoderTrickState *pState)
{
    BDBG_OBJECT_ASSERT(decoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(&decoder->decoder.soft, NEXUS_VideoDecoder_Soft);

    decoder->trickState = *pState;
    return BERR_SUCCESS;
}



static const NEXUS_VideoDecoder_P_Interface NEXUS_VideoDecoder_P_Interface_Soft = {
    NEXUS_VideoDecoder_P_Close_Soft,
    NEXUS_VideoDecoder_P_GetOpenSettings_Common,
    NEXUS_VideoDecoder_P_GetSettings_Common,
    NEXUS_VideoDecoder_P_SetSettings_Soft,
    NEXUS_VideoDecoder_P_Start_Soft,
    NEXUS_VideoDecoder_P_Stop_Soft,
    NEXUS_VideoDecoder_P_Flush_Soft,
    NEXUS_VideoDecoder_P_GetStatus_Soft,
    NEXUS_VideoDecoder_P_GetConnector_Common,
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_GetStreamInformation_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_SetStartPts_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_IsCodecSupported_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_SetPowerState_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_Reset_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_GetExtendedStatus_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_GetExtendedSettings_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_SetExtendedSettings_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_CreateStripedSurface_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_DestroyStripedSurface_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_CreateStripedMosaicSurfaces_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_DestroyStripedMosaicSurfaces_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_GetMostRecentPts_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_OpenPrimer_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_ClosePrimer_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_StartPrimer_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_StopPrimer_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_StartDecodeWithPrimer_Soft),
    NEXUS_VideoDecoder_P_GetTrickState_Common,
    NEXUS_VideoDecoder_P_SetTrickState_Soft,
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_FrameAdvance_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_GetNextPts_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_GetPlaybackSettings_Soft),
    NOT_SUPPORTED(NEXUS_VideoDecoder_P_SetPlaybackSettings_Soft),
    NOT_SUPPORTED(NEXUS_StillDecoder_P_Open_Soft),

#if NEXUS_HAS_ASTM
    NEXUS_VideoDecoder_GetAstmSettings_priv_Common,
    NOT_SUPPORTED(NEXUS_VideoDecoder_SetAstmSettings_priv_Soft),
    NEXUS_VideoDecoder_GetAstmStatus_isr_Common,
#endif

    NEXUS_VideoDecoder_GetDisplayConnection_priv_Soft,
    NEXUS_VideoDecoder_SetDisplayConnection_priv_Xdm,
    NEXUS_VideoDecoder_GetSourceId_priv_Soft,
    NEXUS_VideoDecoder_GetHeap_priv_Common,
    NEXUS_VideoDecoder_GetSyncSettings_priv_Common,
    NOT_SUPPORTED(NEXUS_VideoDecoder_SetSyncSettings_priv_Soft),
    NEXUS_VideoDecoder_GetSyncStatus_isr_Common,
    NEXUS_VideoDecoder_UpdateDisplayInformation_priv_Xdm
};




