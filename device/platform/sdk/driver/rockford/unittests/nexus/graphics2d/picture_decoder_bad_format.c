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
 * $brcm_Workfile: picture_decoder_bad_format.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 6/20/12 11:22a $
 *
 * Module Description: Test SID doesn't hang or crash when attempting decode of 
 *                     image with bad (incorrect) format specified.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/picture_decoder_bad_format.c $
 * 
 * Hydra_Software_Devel/4   6/20/12 11:22a ahulse
 * SW7425-2679: use generic NEXUS_ANY_ID, for picture decoder resource
 * management
 * 
 * Hydra_Software_Devel/3   5/24/12 3:05p mward
 * SW7425-2679: no argv, argc used.  Compile error for chips without  SID.
 * 
 * Hydra_Software_Devel/2   4/26/12 1:55p ahulse
 * SW7425-2679: disable TEST_DBG macro
 * 
 * Hydra_Software_Devel/1   4/24/12 10:03a ahulse
 * SW7425-2679: add picture_decoder (SID) bad format test app
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

BDBG_MODULE(picture_decoder_bad_format);

#if NEXUS_HAS_PICTURE_DECODER

#define CRC_QUEUE_SIZE 600
#define SID_CHANNELS   16  /* see SID_DECODE_CHANNELS nexus_picture_decoder_module.h */

#define TEST_DBG()  /* do { printf("press a key to continue\n"); getchar(); } while(0) */

/* Structure to simplify passing handles around */
typedef struct app_context {
    NEXUS_Graphics2DHandle     gfx;
    NEXUS_DisplayHandle        display;
    NEXUS_PictureDecoderHandle pictureDecoder;
    NEXUS_PictureDecoderStatus pictureStatus;
    BKNI_EventHandle           checkpointEvent; 
    BKNI_EventHandle           decoderStep;
    NEXUS_SurfaceHandle        framebuffer;
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
        NEXUS_PictureFormat  imgFormat;
} testVectors;

testVectors noLockUpTest[] = {
        { "videos/stills/Sunset.jpg"      , {{0xd362, 0xd8a6, 0x3b2e }}, NEXUS_PictureFormat_eJpeg },
        { "videos/stills/06_step06.png"   , {{0xc594, 0x858d, 0x99d6 }}, NEXUS_PictureFormat_ePng  },
        { "videos/stills/9-1.gif"         , {{0xa2a8, 0x8884, 0x9e82 }}, NEXUS_PictureFormat_eGif  },
        { "videos/stills/still_mpeg2_bikini_03.es"     , {{0xa2a8, 0x8884, 0x9e82 }}, NEXUS_PictureFormat_eMpeg },
/*
 TODO : decode rle headers
        { "pict_1200_90.rle_bluray"         , {{0xa2a8, 0x8884, 0x9e82}}, NEXUS_PictureFormat_eRleBluRay },
        { "chamber17.rle_dvd"         , {{0xa2a8, 0x8884, 0x9e82}}, NEXUS_PictureFormat_eRleDvd }
*/

};

void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

void blankGraphics( app_context *context ) {

    NEXUS_GraphicsSettings       graphicsSettings;

   /* Turn off graphics until next pic is decoded */
   NEXUS_Display_GetGraphicsSettings( context->display, &graphicsSettings);
   graphicsSettings.enabled = false;
   NEXUS_Display_SetGraphicsSettings( context->display, &graphicsSettings);

    BKNI_Sleep(500);

   /* Turn on graphics  */
   NEXUS_Display_GetGraphicsSettings( context->display, &graphicsSettings);
   graphicsSettings.enabled = true;
   NEXUS_Display_SetGraphicsSettings( context->display, &graphicsSettings);
}

/* Decode any supported image in one shot ( non segmented decode ) */
int decodeImage( const char *filename, NEXUS_SurfaceHandle *picSurface, NEXUS_PictureDecoderStatus *imgStatus,
                NEXUS_PictureDecoderHandle  *pictureDecoder , unsigned bufSize, NEXUS_PictureFormat testFormat ) {

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

    /* Blindly follow specified format */
    pictureSettings.format = testFormat;

    NEXUS_PictureDecoder_Start(*pictureDecoder, &pictureSettings);

    do {
        NEXUS_PictureDecoder_GetStatus(*pictureDecoder, imgStatus);
        if( imgStatus->state==NEXUS_PictureDecoderState_eError) {
            NEXUS_PictureDecoder_Stop(*pictureDecoder);
            fprintf(stderr, "Header decoding failed for image %s \n" , filename );
            goto error;
            /* return 1; */
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

    /* start decoding image data */
    NEXUS_PictureDecoder_DecodeSegment(*pictureDecoder, *picSurface, NULL);
    do {
        NEXUS_PictureDecoder_GetStatus(*pictureDecoder, imgStatus);
        if( imgStatus->state == NEXUS_PictureDecoderState_eError) {
            fprintf(stderr, "Image decoding failed for image %s \n" , filename );
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

char imageString[][13] = { "rleDvd" , "rleBluRay" , "jpeg" , "gif" , "png" , "mpeg" }; /* see nexus_picture_decoder.h */

/*
  Test picture decoder (SID) doesn't hang when given an image with incorrect format specified
*/
int incorrectFormatTest( app_context *context ) {
    int i,rc;
    NEXUS_SurfaceHandle          picSurface;
    NEXUS_PictureDecoderStatus   pictureStatus;
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_VideoFormatInfo        videoFormatInfo;
    NEXUS_DisplaySettings        displaySettings;
    NEXUS_GraphicsSettings       graphicsSettings;
    int                          fileFormat;

    NEXUS_Display_GetSettings(context->display, &displaySettings);
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);

    for ( i=0; i < (int)( sizeof(noLockUpTest) / sizeof(testVectors) ) ; i++ ) {
 
        for ( fileFormat=(int)NEXUS_PictureFormat_eJpeg; fileFormat < NEXUS_PictureFormat_eMax ; fileFormat++ ) {
            if ( noLockUpTest[i].imgFormat != (NEXUS_PictureFormat)fileFormat ) {
                fprintf( stderr, "Try decode of %s with %s image type ( expect FAIL )\n", noLockUpTest[i].imageName , imageString[fileFormat] );
                rc = decodeImage( noLockUpTest[i].imageName , &picSurface, &pictureStatus, &(context->pictureDecoder), 0 , (NEXUS_PictureFormat)fileFormat );
                if (rc) {
                    BDBG_ERR(("Image %s failed to decode !" , noLockUpTest[i].imageName ));
                    TEST_DBG();
                    continue; /* Don't expect them to decode !! */
                }
            }
            else continue;

            /* Should never make it to here, but if we do display whatever decode attempt was */
            NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
            blitSettings.source.surface     = picSurface;
            blitSettings.source.rect.x      = 0;
            blitSettings.source.rect.y      = 0;
            blitSettings.source.rect.width  = pictureStatus.header.width;
            blitSettings.source.rect.height = pictureStatus.header.height;
            blitSettings.output.surface     = context->framebuffer;
            blitSettings.output.rect.width  = videoFormatInfo.width; /* fill to fit entire screen */
            blitSettings.output.rect.height = videoFormatInfo.height;

            NEXUS_Graphics2D_Blit(context->gfx, &blitSettings);   /* don't wait for blit to complete */
            rc = NEXUS_Graphics2D_Checkpoint(context->gfx, NULL); /* require to execute queue */
            if (rc == NEXUS_GRAPHICS2D_QUEUED) {
                BKNI_WaitForEvent(context->checkpointEvent, BKNI_INFINITE);
            }

            NEXUS_Display_GetGraphicsSettings(context->display, &graphicsSettings);
            graphicsSettings.enabled = true;
            NEXUS_Display_SetGraphicsSettings(context->display, &graphicsSettings);
            NEXUS_Display_SetGraphicsFramebuffer(context->display, context->framebuffer);

            /* Destroy image surface, because next one COULD be different size */
            NEXUS_Surface_Destroy(picSurface);
            picSurface = NULL;
            TEST_DBG();

            /* Turn off graphics until next pic is decoded */
            NEXUS_Display_GetGraphicsSettings( context->display, &graphicsSettings);
            graphicsSettings.enabled = false;
            NEXUS_Display_SetGraphicsSettings( context->display, &graphicsSettings);

            /* Close decoder each loop because it allocates buffer size at opening */
            NEXUS_PictureDecoder_Close( context->pictureDecoder );
            context->pictureDecoder = NULL;
        } 

    } 


    return 0;
}

#endif

int main(void)
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

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;

    NEXUS_Platform_GetConfiguration(&platformConfig);
    BKNI_CreateEvent(&context.checkpointEvent);
    BKNI_CreateEvent(&context.decoderStep);
    context.heap = NULL;
    
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e480p /* use 480p to avoid different crc's for each field */;
    context.display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(context.display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    if ( displaySettings.format == NEXUS_VideoFormat_eNtsc )
        NEXUS_Display_AddOutput(context.display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(context.display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format.
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(context.display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(context.display, &displaySettings);
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
    context.framebuffer = NEXUS_Surface_Create(&createSettings);

    context.gfx = NEXUS_Graphics2D_Open(0, NULL);
    NEXUS_Graphics2D_GetSettings(context.gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = context.checkpointEvent;
    NEXUS_Graphics2D_SetSettings(context.gfx, &gfxSettings);

    context.pictureDecoder = NULL;

    fprintf(stderr, "\nincorrect Format test\n" );
    incorrectFormatTest( &context );
    blankGraphics( &context );

    if ( context.pictureDecoder )  NEXUS_PictureDecoder_Close( context.pictureDecoder );
    NEXUS_Display_RemoveAllOutputs(context.display);
    NEXUS_Graphics2D_Close(context.gfx);

    NEXUS_Display_Close(context.display);
    NEXUS_Surface_Destroy(context.framebuffer);
    BKNI_DestroyEvent(context.decoderStep);
    BKNI_DestroyEvent(context.checkpointEvent);
    NEXUS_Platform_Uninit();
#else
    printf("This platform has no picture decoder, application not supported on this platform!\n");
#endif

    return 0;
}

