/******************************************************************************
 *    (c)2008-2012 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: picture_decoder.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 9/13/12 5:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/picture_decoder.c $
 * 
 * Hydra_Software_Devel/7   9/13/12 5:01p ahulse
 * SW7425-2678: Update golden crc values
 * 
 * Hydra_Software_Devel/6   8/2/12 3:27p ahulse
 * SW7425-2678: update golden crc values as a result of change in SW7425-
 * 3321
 * 
 * Hydra_Software_Devel/5   6/27/12 9:20a ahulse
 * SW7425-2678: move displayCrc to vdcCrc.h, refactor handle passing
 * context
 * 
 * Hydra_Software_Devel/4   6/20/12 11:24a ahulse
 * SW7425-2678: use generic NEXUS_ANY_ID, for picture decoder resource
 * management
 * 
 * Hydra_Software_Devel/3   4/18/12 8:48a ahulse
 * SW7425-2678: init pic decoder handles array
 * 
 * Hydra_Software_Devel/2   4/17/12 9:10a ahulse
 * SW7425-2678: set directory to load test files from
 * 
 * Hydra_Software_Devel/1   4/16/12 6:18p ahulse
 * SW7425-2678: add picture_decoder (SID) unittest app
 *
 *
******************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "nexus_types.h"
#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_video_output.h"
#include "nexus_video_window.h"
#include "nexus_core_utils.h"
#include <stdio.h>
#include <string.h>

#if NEXUS_HAS_PICTURE_DECODER
#include "nexus_picture_decoder.h"
#endif
#include "nexus_graphics2d.h"
#include <unistd.h>

BDBG_MODULE(picture_decoder);

#include "../common/vdcCrc.h"

#if NEXUS_HAS_PICTURE_DECODER

#define SID_CHANNELS   16  /* see SID_DECODE_CHANNELS nexus_picture_decoder_module.h */

#define TEST_DBG()  /* do { printf("press a key to continue\n"); getchar(); } while(0)  */

/* Structure to simplify passing handles around */
typedef struct app_context {
    crc_context                crcCtxt;
    BKNI_EventHandle           decoderStep;
    NEXUS_PictureDecoderHandle pictureDecoder;
    NEXUS_PictureDecoderStatus pictureStatus;
    NEXUS_HeapHandle           heap;
} app_context;

typedef struct pic_context {
    NEXUS_SurfaceHandle picSurface;
    NEXUS_Rect          rect;
    const char          *filename;
    size_t              fileSize;
    size_t              bytesRemain;
    size_t              bufSize;
    unsigned            block;
} pic_context;

typedef struct {
        const char imageName[61];
        NEXUS_DisplayCrcData data;
} testVectors;

testVectors goldenCrcVectors[] = {
        { "videos/stills/Sunset.jpg"      , {{0x3b0d, 0x83ba, 0xa5f3 }} }, 
        { "videos/stills/06_step06.png"   , {{0xd634, 0x5867, 0x0398 }} },
        { "videos/stills/9-1.gif"         , {{0xec2a, 0xcf1d, 0x6154 }} },
/* TODO : decode rle headers
        { "pict_1200_90.rle_bluray"         , {{0xa2a8, 0x8884, 0x9e82}} },
        { "chamber17.rle_dvd"         , {{0xa2a8, 0x8884, 0x9e82}} }
*/

};

#ifdef UNIQUE_JPEGS
/* Using these jpegs it's easier to see individual decodes */
char jpegSamples[][64] = { "01_TN_one.jpg"     , "02_TN_two.jpg"      , "03_TN_three.jpg"  , "04_TN_four.jpg",
                           "05_TN_five.jpg"    , "06_TN_six.jpg"      , "07_TN_seven.jpg"  , "08_TN_eight.jpg" ,
                           "09_TN_nine.jpg"    , "10_TN_ten.jpg"      , "11_TN_eleven.jpg" , "12_TN_twelve.jpg",
                           "13_TN_thirteen.jpg", "14_TN_fourteen.jpg" , "15_TN_fifteen.jpg",
                           "TN_number_1e.jpg"  , "TN_number_2e.jpg"   , "TN_number_3e.jpg" , "TN_number_4e.jpg",
                           "TN_number_5e.jpg"  , "TN_number_6e.jpg"   , "TN_number_7e.jpg" , "TN_number_8e.jpg"
                        };
#else
char jpegSamples[][64] = { "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg",
                           "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg" ,
                           "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg" ,
                           "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg" ,
                           "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg" , "videos/stills/Sunset.jpg" ,
                        };
#endif

void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

/* Callback called whenever image decoder has completed it's task ( ie segmented or full image decode etc ) */
static void picture_decoder_step(void *context_, int unused)
{
    const struct app_context *context = context_;
    unused = unused;
    BKNI_SetEvent(context->decoderStep);
    return;
}

/* simple func to find a heap that is not heap[0] ( default ) 
   and is greater than X bytes.
*/
NEXUS_HeapHandle pick_eFull_Heap( unsigned minHeapSize ) {

    NEXUS_HeapHandle             stdHeap=NULL,fullyMappedHeap=NULL;
    NEXUS_PlatformConfiguration  platformConfig;
    int i,rc;

    NEXUS_Platform_GetConfiguration(&platformConfig);
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        NEXUS_MemoryStatus status;
        NEXUS_HeapHandle heap;

        heap = platformConfig.heap[i];
        if (!heap) continue;
        rc = NEXUS_Heap_GetStatus(heap, &status);
        BDBG_ASSERT(!rc);
        BDBG_MSG(("platform heap[%d]: MEMC %d, offset %#x, addr %p, size %d (%#x), alignment %d\n",
            i, status.memcIndex, status.offset, status.addr, status.size, status.size, status.alignment));
        /* pick an eFull heap, but not first and one > 4M */
        if ( i == 0 ) {
            /* if we find no other, have to use this, which is what pic decoder uses anyway */
            stdHeap = heap;
            BDBG_MSG(("std heap i=%d heap=%p size=%d\n", i, (void *)heap, status.size ));
        }
        else if ( status.size > minHeapSize && status.memoryType == NEXUS_MemoryType_eFull) {
            BDBG_MSG(("picked heap i=%d heap=%p size=%d\n", i, (void *)heap, status.size ));
            /* force default heap selection , uncomment to revive operation 
               also to see heap mappings run with msg_modules=nexus_platform_settings
            */
            /* fullyMappedHeap = heap; */
            break; /* don't check anymore */
        }
    }

    if ( fullyMappedHeap==NULL ) {
        BDBG_MSG(("Using default heap!"));
        fullyMappedHeap = stdHeap;
    }

    return fullyMappedHeap;
}

void blankGraphics( app_context *context ) {

    NEXUS_GraphicsSettings       graphicsSettings;

   /* Turn off graphics until next pic is decoded */
   NEXUS_Display_GetGraphicsSettings( context->crcCtxt.display, &graphicsSettings);
   graphicsSettings.enabled = false;
   NEXUS_Display_SetGraphicsSettings( context->crcCtxt.display, &graphicsSettings);

    BKNI_Sleep(500);

   /* Turn on graphics  */
   NEXUS_Display_GetGraphicsSettings( context->crcCtxt.display, &graphicsSettings);
   graphicsSettings.enabled = true;
   NEXUS_Display_SetGraphicsSettings( context->crcCtxt.display, &graphicsSettings);
}

/* Decode any supported image in one shot ( non segmented decode ) */
int decodeImage( const char *filename, NEXUS_SurfaceHandle *picSurface, NEXUS_PictureDecoderStatus *imgStatus,
                NEXUS_PictureDecoderHandle  *pictureDecoder , unsigned bufSize ) {

    NEXUS_PictureDecoderStartSettings pictureSettings;
    NEXUS_PictureDecoderOpenSettings  decoderSettings;
    NEXUS_SurfaceCreateSettings       imgSurfaceSettings;

    int rc;
    FILE *fin ;
    void *buffer;
    size_t size;

    fin = fopen(filename,"rb");
    if(!fin) {
        perror(filename);
        return 1;
    }
    fseek(fin, 0, SEEK_END); /* seek to end of file so we can get filesize */

    if ( *pictureDecoder == NULL ) {
        /* doesn't exist so open picture decoder */
        NEXUS_PictureDecoder_GetDefaultOpenSettings(&decoderSettings);
        decoderSettings.multiScanBufferSize = 128*1024;
        if ( bufSize ) {
            decoderSettings.bufferSize = bufSize;
        }
        else {
            /* If not set, set to actual size of file */
            decoderSettings.bufferSize = ftell(fin);
        }
        *pictureDecoder = NEXUS_PictureDecoder_Open(NEXUS_ANY_ID, &decoderSettings);
        if ( *pictureDecoder == NULL ) {
            BDBG_ERR(("Failed to open picture decoder !!"));
            goto error;
        }
    }
    fseek(fin, 0, SEEK_SET);

    NEXUS_PictureDecoder_GetBuffer(*pictureDecoder, &buffer, &size); 
    rc = fread(buffer, 1, size, fin); 
    if(rc<0) {
        perror(filename);
        goto error;
    }
    NEXUS_PictureDecoder_ReadComplete(*pictureDecoder, 0, rc); 

    NEXUS_PictureDecoder_GetDefaultStartSettings(&pictureSettings);

    /* Determine image format and tell decoder so it can decode correctly */
    if (strcasestr(filename, ".mpg") || strcasestr(filename, ".es")) {
        pictureSettings.format = NEXUS_PictureFormat_eMpeg;
    }
    else if (strcasestr(filename, ".png")) {
        pictureSettings.format = NEXUS_PictureFormat_ePng;
    }
    else if (strcasestr(filename, ".gif")) {
        pictureSettings.format = NEXUS_PictureFormat_eGif;
    }
    else if (strcasestr(filename, ".rle_bluray")) {
        pictureSettings.format = NEXUS_PictureFormat_eRleBluRay;
        BDBG_ERR(("RleBluRay not supported yet"));
        goto error;
    }
    else if (strcasestr(filename, ".rle_dvd")) {
        pictureSettings.format = NEXUS_PictureFormat_eRleDvd;
        BDBG_ERR(("RleDvd not supported yet"));
        goto error;
    }
    else if (strcasestr(filename, ".bmp")) {
        BDBG_ERR(("BMP is not supported"));
        goto error;
    }
    else {
        pictureSettings.format = NEXUS_PictureFormat_eJpeg;
    }

    NEXUS_PictureDecoder_Start(*pictureDecoder, &pictureSettings);

    do {
        NEXUS_PictureDecoder_GetStatus(*pictureDecoder, imgStatus);
        if( imgStatus->state==NEXUS_PictureDecoderState_eError) {
            fprintf(stderr, "decoding failed\n");
            return 1;
        }
        usleep(1000);
    } while(!imgStatus->headerValid); /* wait for picture dimensions */

    /* create surface that can handle complete picture */
    NEXUS_Surface_GetDefaultCreateSettings(&imgSurfaceSettings);
    imgSurfaceSettings.pixelFormat = imgStatus->header.format;
    imgSurfaceSettings.width       = imgStatus->header.surface.width;
    imgSurfaceSettings.height      = imgStatus->header.surface.height;

    imgSurfaceSettings.alignment = 2;

    switch (imgSurfaceSettings.pixelFormat) {
        case NEXUS_PixelFormat_ePalette8:
        case NEXUS_PixelFormat_eA8:
            imgSurfaceSettings.pitch = imgSurfaceSettings.width;
            break;
        case NEXUS_PixelFormat_ePalette4:
            imgSurfaceSettings.pitch = (imgSurfaceSettings.width + 1) >> 1;
            break;
        case NEXUS_PixelFormat_ePalette2:
            imgSurfaceSettings.pitch = (imgSurfaceSettings.width + 3) >> 2;
            break;
        case NEXUS_PixelFormat_ePalette1:
            imgSurfaceSettings.pitch = (imgSurfaceSettings.width + 7) >> 3;
            break;
        case NEXUS_PixelFormat_eA8_Palette8:
            imgSurfaceSettings.pitch = imgSurfaceSettings.width << 1;
            break;
        default:
            break;
    }
    imgSurfaceSettings.pitch = ((imgSurfaceSettings.pitch + 3) & ~3);
    BDBG_MSG(("creating surface: format %d, %dx%d, pitch %d", imgSurfaceSettings.pixelFormat, imgSurfaceSettings.width, imgSurfaceSettings.height, imgSurfaceSettings.pitch));

    *picSurface = NEXUS_Surface_Create(&imgSurfaceSettings);

    /* if uses a palette, copy it */
    if (NEXUS_PIXEL_FORMAT_IS_PALETTE(imgSurfaceSettings.pixelFormat)) {
        NEXUS_PictureDecoderPalette sidPalette;
        NEXUS_SurfaceMemory mem;

        rc = NEXUS_PictureDecoder_GetPalette(*pictureDecoder, &sidPalette);
        BDBG_ASSERT(!rc);
        NEXUS_Surface_GetMemory(*picSurface, &mem),
        BKNI_Memcpy(mem.palette, sidPalette.palette, mem.numPaletteEntries*sizeof(NEXUS_PixelFormat));
        NEXUS_Surface_Flush(*picSurface);
    }

    /* start decoding */
    NEXUS_PictureDecoder_DecodeSegment(*pictureDecoder, *picSurface, NULL);
    do {
        NEXUS_PictureDecoder_GetStatus(*pictureDecoder, imgStatus);
        if( imgStatus->state == NEXUS_PictureDecoderState_eError) {
            fprintf(stderr, "decoding failed\n");
            goto error;
        }
        usleep(1000);
    } while( imgStatus->state!=NEXUS_PictureDecoderState_eSegmentDone); /* wait for picture to decode */

    NEXUS_PictureDecoder_Stop(*pictureDecoder);

    fclose( fin );
    return 0;
error:
    fclose( fin );
    return 1;
}

int openFileDecodeHeader( FILE **fin, app_context *context, pic_context *picDecode ) {

    NEXUS_PictureDecoderStartSettings pictureSettings;
    NEXUS_PictureDecoderOpenSettings  decoderSettings;
    NEXUS_SurfaceCreateSettings       jpegSurfaceSettings; 
    NEXUS_PictureDecoderStatus        *jpegStatus;
    NEXUS_Rect                        rect;

    int    rc;
    void   *buffer;
    size_t size,fileSize,readSize;

    jpegStatus = &context->pictureStatus;

    BDBG_MSG(("Opening file %s fin=%p" , picDecode->filename , *fin ));
    *fin = fopen(picDecode->filename,"rb");
    if(!*fin) {
        perror(picDecode->filename);
        return 1;
    }
    fseek(*fin, 0, SEEK_END);
    fileSize = ftell(*fin);
    fseek(*fin, 0, SEEK_SET);

    if ( context->pictureDecoder == NULL ) {
        /* pic decoder not open yet, so open */
        NEXUS_PictureDecoder_GetDefaultOpenSettings(&decoderSettings);
        decoderSettings.multiScanBufferSize = 1*1024*1024;
        BDBG_MSG(("DecodeHeader bufSize=%d" , picDecode->bufSize ));
        if ( picDecode->bufSize ) {
            decoderSettings.bufferSize = picDecode->bufSize;
        }
        else {
            /* If not set, set to actual size of file */
            decoderSettings.bufferSize = fileSize;
        }
        if ( context->heap ) {
            decoderSettings.heap = context->heap;
            BDBG_MSG(("ctxt->heap=%p decoderSettings.heap=%p" , context->heap, decoderSettings.heap ));
        }
        context->pictureDecoder = NEXUS_PictureDecoder_Open(NEXUS_ANY_ID, &decoderSettings);
        if ( context->pictureDecoder == NULL ) {
            fclose( *fin );
            BDBG_ERR(("Failed to open Picture Decoder" ));
            goto error;
        }
    }

    NEXUS_PictureDecoder_GetBuffer(context->pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
    BDBG_MSG(("DecodeHeader read in %d bytes" , size ));
    rc = fread(buffer, 1, size, *fin); /* read file into the decoder's buffer */
    if(rc<0) {
        perror(picDecode->filename);
        goto error;
    } 
    NEXUS_PictureDecoder_ReadComplete(context->pictureDecoder, 0, rc); /* commit data to the decoder */

    /* Get Default settings and then customize with our own */
    NEXUS_PictureDecoder_GetDefaultStartSettings(&pictureSettings);
    pictureSettings.format                  = NEXUS_PictureFormat_eJpeg;
    pictureSettings.pictureParsed.callback  = picture_decoder_step;
    pictureSettings.pictureParsed.context   = &(*context);
    pictureSettings.segmentDecoded.callback = picture_decoder_step;
    pictureSettings.segmentDecoded.context  = &(*context);
    if( fileSize > decoderSettings.bufferSize ) {
        pictureSettings.imageSize           = fileSize;
    }

    /* Start the image decode process */
    NEXUS_PictureDecoder_Start(context->pictureDecoder, &pictureSettings);
    BKNI_WaitForEvent(context->decoderStep, BKNI_INFINITE);

    NEXUS_PictureDecoder_GetStatus(context->pictureDecoder, jpegStatus);
    if(jpegStatus->state==NEXUS_PictureDecoderState_eError) {
        fprintf(stderr, "header decoding failed\n");
        goto error;
    } else if ( jpegStatus->state==NEXUS_PictureDecoderState_eMoreData ) {
        /* Get a new buffer full */
        do {
            NEXUS_PictureDecoder_GetBuffer(context->pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
            readSize = fread(buffer, 1, size, *fin); /* read file into the decoder's buffer */

            if(readSize) {
                NEXUS_PictureDecoder_ReadComplete(context->pictureDecoder, 0, readSize); /* commit data to the decoder */
                BKNI_WaitForEvent(context->decoderStep, BKNI_INFINITE);
                NEXUS_PictureDecoder_GetStatus(context->pictureDecoder, jpegStatus);
                if ( jpegStatus->headerValid ) {
                    /* SID has signalled it has enough data to decode image header, so stop reading further data */
                    break;
                }
            }
        } while ( readSize );
    }
    BDBG_ASSERT(jpegStatus->headerValid);

    if ( picDecode->picSurface==NULL ) {

        rect.x = 0;
        rect.width = jpegStatus->header.surface.width;
        rect.y = 0;
        rect.height = jpegStatus->header.surface.height;
        if ( picDecode->block ) {
            if(rect.y + picDecode->block <= jpegStatus->header.surface.height) {
                rect.height = picDecode->block;
            } else {
                rect.height = jpegStatus->header.surface.height - rect.y;
            }
        }

        /* create picture that could handle complete picture */
        NEXUS_Surface_GetDefaultCreateSettings(&jpegSurfaceSettings);
        jpegSurfaceSettings.pixelFormat = jpegStatus->header.format;
        jpegSurfaceSettings.width       = rect.width;
        jpegSurfaceSettings.height      = rect.height;
        picDecode->picSurface = NEXUS_Surface_Create(&jpegSurfaceSettings);
        BDBG_MSG(("Created jpegSurface w=%d h=%d , jpeg w=%d h=%d" , rect.width, rect.height ,
                                                                     jpegStatus->header.surface.width, jpegStatus->header.surface.height ));
    }


    fseek(*fin, 0, SEEK_SET); /* rewind to begining of file */

    picDecode->fileSize    = fileSize;
    picDecode->bytesRemain = fileSize;

    BDBG_MSG(("Decoded Header for image %s OK fileSize=%d", picDecode->filename , picDecode->fileSize ));

    return 0;
error:
    fclose( *fin );
    return 1;
}

int decodeRow( FILE *fin, app_context *context, pic_context *picDecode ) {

    NEXUS_PictureDecoderStatus        *jpegStatus;
    NEXUS_Rect                        rect;

    void *buffer;
    size_t size, readSize;

    jpegStatus = &context->pictureStatus;

    BDBG_MSG(("decodeRow: Ctxt image w=%d h=%d surf.w=%d surf.h=%d format=%d\n\n",
                context->pictureStatus.header.width, context->pictureStatus.header.height,
                context->pictureStatus.header.surface.width, context->pictureStatus.header.surface.height,
                context->pictureStatus.header.format ));

    rect = picDecode->rect;

    if ( picDecode->block ) {
        if(rect.y + picDecode->block <= jpegStatus->header.surface.height) {
            rect.height = picDecode->block;
        } else {
            rect.height = jpegStatus->header.surface.height - rect.y;
        }
    }
    picDecode->rect = rect;

    if ( picDecode->picSurface ) {

        if ( rect.y == 0 ) {
            /* For first segment to decode, read in data BEFORE calling DecodeSegment, otherwise data already in buffer */
            NEXUS_PictureDecoder_GetBuffer(context->pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
            readSize = fread(buffer, 1, size, fin); /* read file into the decoder's buffer */

            if(readSize) {
                NEXUS_PictureDecoder_ReadComplete(context->pictureDecoder, 0, readSize); /* commit data to the decoder */
                picDecode->bytesRemain -= readSize;
            }
        }

        if ( picDecode->block ) {
            NEXUS_PictureDecoder_DecodeSegment(context->pictureDecoder, picDecode->picSurface, &rect);
        }
        else {
            NEXUS_PictureDecoder_DecodeSegment(context->pictureDecoder, picDecode->picSurface, NULL);
        }
        BKNI_WaitForEvent(context->decoderStep, BKNI_INFINITE);
        NEXUS_PictureDecoder_GetStatus(context->pictureDecoder, jpegStatus);
        if(jpegStatus->state==NEXUS_PictureDecoderState_eError) {
                fprintf(stderr, "image decoding failed\n");
                NEXUS_Surface_Destroy(picDecode->picSurface);
                return 1;
        } else if ( jpegStatus->state==NEXUS_PictureDecoderState_eMoreData ) {
            /* Get a new buffer full */
            do {
                NEXUS_PictureDecoder_GetBuffer(context->pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
                if( picDecode->bytesRemain < size ) size = picDecode->bytesRemain;
                readSize = fread(buffer, 1, size, fin); /* read file into the decoder's buffer */
                if(readSize) {
                    NEXUS_PictureDecoder_ReadComplete(context->pictureDecoder, 0, readSize); /* commit data to the decoder */
                    picDecode->bytesRemain -= readSize;
                    BKNI_WaitForEvent(context->decoderStep, BKNI_INFINITE);
                    NEXUS_PictureDecoder_GetStatus(context->pictureDecoder, jpegStatus);
                    if ( jpegStatus->state==NEXUS_PictureDecoderState_eSegmentDone ) break;
                }
            } while ( readSize );
            NEXUS_PictureDecoder_GetStatus(context->pictureDecoder, jpegStatus);
        }

    }

    return 0;
}

/*
  Test all picture decoder (SID) supported image formats, decode them
  and check veracity of decoded picture against archive crc signature.
  Fail decode if they don't match.
*/
int allImageCrcTest( app_context *context ) {
    int i,rc,imgFail;
    NEXUS_SurfaceHandle          picSurface;
    NEXUS_PictureDecoderStatus   pictureStatus;
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_VideoFormatInfo        videoFormatInfo;
    NEXUS_DisplaySettings        displaySettings;
    NEXUS_GraphicsSettings       graphicsSettings;
    NEXUS_DisplayCrcData         data[16];

    NEXUS_Display_GetSettings(context->crcCtxt.display, &displaySettings);
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);

    BKNI_Memset( data, sizeof(data) , 0 );

    imgFail = 0;
    for ( i=0; i < (int)( sizeof(goldenCrcVectors) / sizeof(testVectors) ) ; i++ ) {
 
        /* use graphics to fit image into the imageinput surface */
        rc = decodeImage( goldenCrcVectors[i].imageName , &picSurface, &pictureStatus, &(context->pictureDecoder), 0 );
        if (rc) {
            BDBG_ERR(("Image %s failed to decode !" , goldenCrcVectors[i].imageName ));
            return -1;
        }

        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface     = picSurface;
        blitSettings.source.rect.x      = 0;
        blitSettings.source.rect.y      = 0;
        blitSettings.source.rect.width  = pictureStatus.header.width;
        blitSettings.source.rect.height = pictureStatus.header.height;
        blitSettings.output.surface     = context->crcCtxt.framebuffer;
        blitSettings.output.rect.width  = videoFormatInfo.width; /* fill to fit entire screen */
        blitSettings.output.rect.height = videoFormatInfo.height;

        NEXUS_Graphics2D_Blit(context->crcCtxt.gfx, &blitSettings);   /* don't wait for blit to complete */
        rc = NEXUS_Graphics2D_Checkpoint(context->crcCtxt.gfx, NULL); /* require to execute queue */
        if (rc == NEXUS_GRAPHICS2D_QUEUED) {
            BKNI_WaitForEvent(context->crcCtxt.checkpointEvent, BKNI_INFINITE);
        }

        NEXUS_Display_GetGraphicsSettings(context->crcCtxt.display, &graphicsSettings);
        graphicsSettings.enabled = true;
        NEXUS_Display_SetGraphicsSettings(context->crcCtxt.display, &graphicsSettings);
        NEXUS_Display_SetGraphicsFramebuffer(context->crcCtxt.display, context->crcCtxt.framebuffer);

        if ( displayVdcCrc( &context->crcCtxt, goldenCrcVectors[i].data , goldenCrcVectors[i].imageName ) ) {
            BDBG_ERR(("!!! Image %s failed CRC check !!!" , goldenCrcVectors[i].imageName ));
            imgFail |= 1<<i;
        }

        /* Destroy image surface, because next one COULD be different size */
        NEXUS_Surface_Destroy(picSurface);
        picSurface = NULL;
        TEST_DBG();

        /* Turn off graphics until next pic is decoded */
        NEXUS_Display_GetGraphicsSettings( context->crcCtxt.display, &graphicsSettings);
        graphicsSettings.enabled = false;
        NEXUS_Display_SetGraphicsSettings( context->crcCtxt.display, &graphicsSettings);

        /* Close decoder each loop because it allocates buffer size at opening */
        NEXUS_PictureDecoder_Close( context->pictureDecoder );
        context->pictureDecoder = NULL;
    } /* end of for */

    return imgFail;
}

/* Test opening max sid channels +1 - should fail to open on max chan+1 open */
int multiDecodeTest( app_context *context ) {
    int i,rc;
    NEXUS_SurfaceHandle          picSurface;
    NEXUS_PictureDecoderStatus   pictureStatus;
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_VideoFormatInfo        videoFormatInfo;
    NEXUS_DisplaySettings        displaySettings;
    NEXUS_GraphicsSettings       graphicsSettings;
    NEXUS_PictureDecoderHandle   picDec[SID_CHANNELS+1];

    BKNI_Memset( &picDec[0], 0, sizeof(NEXUS_PictureDecoderHandle)*(SID_CHANNELS+1) );

    NEXUS_Display_GetSettings(context->crcCtxt.display, &displaySettings);
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);

    for ( i=0; i < SID_CHANNELS+1 ; i++ ) {

        picDec[i] = NULL;
        /* use graphics to fit image into the imageinput surface */
        BDBG_MSG(("Image %2d = %s" , i+1, jpegSamples[i] ));
        if ( i == SID_CHANNELS ) {
            fprintf( stderr, "Test opening SID MAX_CHAN+1 (expect fail)\n" );
        }
        rc = decodeImage(  jpegSamples[i], &picSurface, &pictureStatus, &picDec[i], 0 );
        if (rc) {
            BDBG_ERR(("Image %d %s failed to decode !" , i, jpegSamples[i] ));
            continue;
        }

        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface     = picSurface;
        blitSettings.source.rect.x      = 0;
        blitSettings.source.rect.y      = 0;
        blitSettings.source.rect.width  = pictureStatus.header.width;
        blitSettings.source.rect.height = pictureStatus.header.height;

        blitSettings.output.surface     = context->crcCtxt.framebuffer;
        /* TODO: divide 720 / 120 = 6 pics per row, 720 from 480p reolution */
        blitSettings.output.rect.x      = ( i % 6 ) * 120;
        blitSettings.output.rect.y      = ( i / 6 ) * 120;
        blitSettings.output.rect.width  = 120; 
        blitSettings.output.rect.height = 120;

        NEXUS_Graphics2D_Blit(context->crcCtxt.gfx, &blitSettings);  /* don't wait for blit to complete */
        rc = NEXUS_Graphics2D_Checkpoint(context->crcCtxt.gfx, NULL); /* require to execute queue */
        if (rc == NEXUS_GRAPHICS2D_QUEUED) {
            BKNI_WaitForEvent(context->crcCtxt.checkpointEvent, BKNI_INFINITE);
        }

        NEXUS_Display_GetGraphicsSettings(context->crcCtxt.display, &graphicsSettings);
        graphicsSettings.enabled = true;
        NEXUS_Display_SetGraphicsSettings(context->crcCtxt.display, &graphicsSettings);
        NEXUS_Display_SetGraphicsFramebuffer(context->crcCtxt.display, context->crcCtxt.framebuffer);

        /* Destroy image surface, because next one COULD be different size */
        NEXUS_Surface_Destroy(picSurface);
        picSurface = NULL;

    } /* end of for */
    TEST_DBG();

    for ( i=0; i < SID_CHANNELS ; i++ ) {
        /* close any decoders that are open */
        if ( picDec[i] ) NEXUS_PictureDecoder_Close( picDec[i] );
    }

    return 0;
}

int jpegRowDecodeTest( app_context *context , int bufrSize ) {
    int rc;
    unsigned num,inc_y;
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_VideoFormatInfo        videoFormatInfo;
    NEXUS_DisplaySettings        displaySettings;
    NEXUS_GraphicsSettings       graphicsSettings;
    NEXUS_DisplayCrcData         data;
    FILE *pfin=NULL ; 
    pic_context picDecode;

    NEXUS_Display_GetSettings(context->crcCtxt.display, &displaySettings);
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);

    BKNI_Memset( &data, sizeof(data) , 0 );

    picDecode.filename = (const char *)goldenCrcVectors[0].imageName;
    BDBG_MSG(("picDecode.filename=%s" , picDecode.filename ));
    picDecode.picSurface = NULL;
    picDecode.bufSize    = 0;
    if ( bufrSize ) 
        picDecode.bufSize    = bufrSize;

    picDecode.block      = 64;  /* this needs to be a multiple of 16 for jpeg segmented decode !! */

    rc = openFileDecodeHeader( &pfin , &(*context), &picDecode );
    if (rc) {
        BDBG_ERR(("Image %s failed to decode ! L%d" , picDecode.filename, __LINE__ ));
        return -1;
    }
    BDBG_MSG(("jpegRowDecodeTest: cntx->picStat hdr.w=%d hdr.h=%d fin=%p" , context->pictureStatus.header.width, context->pictureStatus.header.height , pfin ));

    picDecode.rect.x = 0;
    picDecode.rect.width = context->pictureStatus.header.width;
    picDecode.rect.y = 0;
    picDecode.rect.height = picDecode.block;

    inc_y = 0;
    while ( inc_y < videoFormatInfo.height ) {

        rc = decodeRow( pfin , &(*context), &picDecode );

        num = 0;

        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface     = picDecode.picSurface;
        blitSettings.source.rect.x      = 0;
        blitSettings.source.rect.y      = 0;
        blitSettings.source.rect.width  = context->pictureStatus.header.width;
        blitSettings.source.rect.height = picDecode.rect.height /* context->pictureStatus.header.height */;

        blitSettings.output.surface     = context->crcCtxt.framebuffer;
        blitSettings.output.rect.width  = videoFormatInfo.width; /* fill to fit entire screen width */
        blitSettings.output.rect.y      = inc_y;
        blitSettings.output.rect.height = ((picDecode.rect.y+blitSettings.source.rect.height)*videoFormatInfo.height)/context->pictureStatus.header.height - inc_y;
/*
        printf("scale %.3f(%.3f)\n", picDecode.rect.height/(float)blitSettings.output.rect.height, 
                context->pictureStatus.header.surface.height/(float)videoFormatInfo.height); 
*/
        BDBG_MSG(("blit from x=%d y=%d w=%d h=%d" , blitSettings.source.rect.x, blitSettings.source.rect.y,
                                                          blitSettings.source.rect.width, blitSettings.source.rect.height ));
        BDBG_MSG(("blit to   x=%d y=%d w=%d h=%d\n" , blitSettings.output.rect.x, blitSettings.output.rect.y,
                                                     blitSettings.output.rect.width, blitSettings.output.rect.height ));

        inc_y += blitSettings.output.rect.height;

        NEXUS_Graphics2D_Blit(context->crcCtxt.gfx, &blitSettings);  /* don't wait for blit to complete */
        rc = NEXUS_Graphics2D_Checkpoint(context->crcCtxt.gfx, NULL); /* required to execute queue */
        if (rc == NEXUS_GRAPHICS2D_QUEUED) {
            BKNI_WaitForEvent(context->crcCtxt.checkpointEvent, BKNI_INFINITE);
        }

        NEXUS_Display_GetGraphicsSettings(context->crcCtxt.display, &graphicsSettings);
        graphicsSettings.enabled = true;
        NEXUS_Display_SetGraphicsSettings(context->crcCtxt.display, &graphicsSettings);
        NEXUS_Display_SetGraphicsFramebuffer(context->crcCtxt.display, context->crcCtxt.framebuffer);

        if ( picDecode.block ) picDecode.rect.y += picDecode.block;
    } /* end of for */

    NEXUS_PictureDecoder_Stop(context->pictureDecoder);

    /* displayVdcCrc( &context->crcCtxt, data , picDecode.filename); */

    /* Destroy image surface, because next one COULD be different size */
    NEXUS_Surface_Destroy(picDecode.picSurface);
    picDecode.picSurface = NULL;

    /* Close decoder because next image decoded may need a different decode buffer size */
    NEXUS_PictureDecoder_Close( context->pictureDecoder );
    context->pictureDecoder = NULL;

    fclose( pfin );

    return 0;
}


#endif

int main(int argc, char **argv)
{
#if NEXUS_HAS_PICTURE_DECODER
    NEXUS_SurfaceCreateSettings  createSettings;
    NEXUS_Graphics2DSettings     gfxSettings;
    NEXUS_PlatformSettings       platformSettings;
    NEXUS_PlatformConfiguration  platformConfig;
    NEXUS_DisplaySettings        displaySettings;
    NEXUS_VideoFormatInfo        videoFormatInfo;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus       hdmiStatus;
#endif
    app_context context;
    int rc;
    const char *filename = "Sunset.jpg"; /* change this to a jpeg of your choice */

    if (argc > 1) {
        filename = argv[1];
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;

    NEXUS_Platform_GetConfiguration(&platformConfig);
    BKNI_CreateEvent(&context.crcCtxt.checkpointEvent);
    BKNI_CreateEvent(&context.decoderStep);
    context.heap = NULL;
    
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e480p /* use 480p to avoid different crc's for each field */;
    context.crcCtxt.display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(context.crcCtxt.display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    if ( displaySettings.format == NEXUS_VideoFormat_eNtsc )
        NEXUS_Display_AddOutput(context.crcCtxt.display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(context.crcCtxt.display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format.
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(context.crcCtxt.display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(context.crcCtxt.display, &displaySettings);
        }
    }
#endif
    rc = 0;
    /* allocate framebuffer */
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = videoFormatInfo.width;
    createSettings.height = videoFormatInfo.height;
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    context.crcCtxt.framebuffer = NEXUS_Surface_Create(&createSettings);

    context.crcCtxt.gfx = NEXUS_Graphics2D_Open(0, NULL);
    NEXUS_Graphics2D_GetSettings(context.crcCtxt.gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = context.crcCtxt.checkpointEvent;
    NEXUS_Graphics2D_SetSettings(context.crcCtxt.gfx, &gfxSettings);

    context.pictureDecoder = NULL;

    fprintf(stderr, "\nDecode All images test\n" );
    rc = allImageCrcTest( &context );
    blankGraphics( &context );

    fprintf(stderr, "\nDecode jpeg with buffer = filesize\n");
    jpegRowDecodeTest( &context , 0 /* use same size buffer as filesize */ );
    blankGraphics( &context );

    fprintf(stderr, "\nDecode jpeg with buffer < filesize\n");
    jpegRowDecodeTest( &context , 36*1024 /* buffer is smaller than filesize */ );
    blankGraphics( &context );

    fprintf(stderr, "\nDecode jpeg with specified heap \n");
    context.heap = pick_eFull_Heap( 3*1024*1024 );
    jpegRowDecodeTest( &context , 2*1024*1024 /* too big for heap ?? */ );
    blankGraphics( &context );

    fprintf(stderr, "\nOpen and decode with MAX sid channels +1\n\n");
    multiDecodeTest( &context ); 

    if ( context.pictureDecoder )  NEXUS_PictureDecoder_Close( context.pictureDecoder );
    NEXUS_Display_RemoveAllOutputs(context.crcCtxt.display);
    NEXUS_Graphics2D_Close(context.crcCtxt.gfx);

    if ( rc ) {
        BDBG_WRN(("CRC check failed, see \"failed CRC check\" warnings above" ));
        BDBG_ASSERT(0); /* assert so error will be picked up by autotest system */
    }

    NEXUS_Display_Close(context.crcCtxt.display);
    NEXUS_Surface_Destroy(context.crcCtxt.framebuffer);
    BKNI_DestroyEvent(context.decoderStep);
    BKNI_DestroyEvent(context.crcCtxt.checkpointEvent);
    NEXUS_Platform_Uninit();
#else
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);
    printf("This platform has no picture decoder, application not supported on this platform!\n");
#endif

    return 0;
}

