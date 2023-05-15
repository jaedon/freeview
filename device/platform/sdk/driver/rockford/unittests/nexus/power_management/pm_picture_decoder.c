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
 * $brcm_Workfile: pm_picture_decoder.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 8/8/12 4:30p $
 *
 * Module Description: Test power management works with SID block
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/power_management/pm_picture_decoder.c $
 * 
 * Hydra_Software_Devel/5   8/8/12 4:30p ahulse
 * SW7425-3613: demote S3 temporarily
 * 
 * Hydra_Software_Devel/4   8/3/12 1:25p ahulse
 * SW7425-3613: update golden crc values as a result of change in SW7425-
 * 3321
 * 
 * Hydra_Software_Devel/3   8/2/12 2:22p ahulse
 * SW7425-3613: re-factor to use pmCommon.h which provides power
 * management wrapper functions
 * 
 * Hydra_Software_Devel/2   8/1/12 5:56p ahulse
 * SW7425-3613: set S3 support according to platform
 * 
 * Hydra_Software_Devel/1   8/1/12 2:12p ahulse
 * SW7425-3613: add
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

BDBG_MODULE(pm_picture_decoder);

#include "../common/vdcCrc.h"
#include "../common/pmCommon.h"

#undef S3_STANDBY_SUPPORT  /* demote S3 support */

#if NEXUS_HAS_PICTURE_DECODER

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
        { "videos/stills/Sunset.jpg"      , {{0xd362, 0x3b2e, 0xd8a6 }} },
        { "videos/stills/06_step06.png"   , {{0xc594, 0x99d6, 0x858d }} },
        { "videos/stills/9-1.gif"         , {{0xa2a8, 0x9e82, 0x8884 }} },
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
#endif

#if NEXUS_HAS_PICTURE_DECODER

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

#endif  /* HAS_PICTURE_DECODER */

/*
  Test single image with picture decoder (SID) , decode image
  and check veracity of decoded picture against archive crc signature.
  Fail decode if it doesn't match.
*/
int pmImageCrcTest( app_context *context ) {
    int i,rc;
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

    for ( i=0; i < 1 /* (int)( sizeof(goldenCrcVectors) / sizeof(testVectors) )*/ ; i++ ) {
 
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
        }

        /* Destroy image surface, because next one COULD be different size */
        NEXUS_Surface_Destroy(picSurface);
        picSurface = NULL;
        TEST_DBG();

        /* Close decoder each loop because it allocates buffer size at opening */
        NEXUS_PictureDecoder_Close( context->pictureDecoder );
        context->pictureDecoder = NULL;
    } /* end of for */

    return 0;
}

int main(int argc, char **argv)
{
#if NEXUS_HAS_PICTURE_DECODER && NEXUS_POWER_MANAGEMENT
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
    int i,rc;

    pmlib_state_t pmlib_state;
    unsigned sleepTime = 5; /* seconds */
    unsigned loops = 1;

    for (i=1; i<argc; i++) {
        if (!strcmp(argv[i], "--help")) {
            BKNI_Printf("Usage: nexus %s (options)\n", argv[0]);
            BKNI_Printf(" -loops #  number of transitions to test. defaults to %u\n", loops);
            return 0;
        }
        else if (!strcmp(argv[i], "-loops")){
            loops = strtoul(argv[++i], NULL, 0);
        }
    }

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;

    g_power_state.brcm_pm_ctx = brcm_pm_init();
    /* when app is first run, enet and moca are already running */
    g_power_state.g_state.enet = true;
    g_power_state.g_state.moca = true;

    get_pmlib_state(&pmlib_state);
    pmlib_state.usb = true;
    pmlib_state.enet = true;
    pmlib_state.moca = false; /* TODO: BSEAV/app/standby/standby.c had "true" commented out */
    pmlib_state.tp1 = true;
    pmlib_state.cpu = true;
    pmlib_state.ddr = true;
    pmlib_state.memc1 = true;
    set_pmlib_state(&pmlib_state);

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

#if S3_STANDBY_SUPPORT
    #define MODES 4
#else
    #define MODES 3
#endif

    while (loops--) {
        int tests = 1; /* skip S1 mode */
        while ( tests++ < MODES ) {
            BDBG_WRN(("loop %d: mode S%d ", loops, tests ));
            rc = resume();
            rc = pmImageCrcTest( &context );
            BKNI_Sleep(sleepTime*1000);

            switch (tests) {
                case 1:
                    rc = active_standby(sleepTime);
                    if (rc) {
                        BDBG_ERR(("Active standby failure"));
                        goto end;
                    }
                    break;
                case 2:
                    rc = passive_standby(sleepTime);
                    if (rc) {
                        BDBG_ERR(("Passive standby failure"));
                        goto end;
                    }
                    break;
#if S3_STANDBY_SUPPORT
/* TODO: S3 support is not across the board and even on chips supporting it depends
         on CFE version, kernel version, board configuration etc
*/
                case 3:
                    rc = deep_sleep(sleepTime);
                    if (rc) {
                        BDBG_ERR(("Deep sleep failure"));
                        goto end;
                    }
                    break;
#else
                case 3:
                    BDBG_WRN(("NOT DOING S3 Test !"));
                    break;
#endif
            }

            BDBG_WRN(("End of loop %d: mode S%d ", loops, tests ));
        } /* tests */

    }
end:
    resume(); /* must resume in order to uninit */

    if ( context.pictureDecoder )  NEXUS_PictureDecoder_Close( context.pictureDecoder );
    NEXUS_Display_RemoveAllOutputs(context.crcCtxt.display);
    NEXUS_Graphics2D_Close(context.crcCtxt.gfx);

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

