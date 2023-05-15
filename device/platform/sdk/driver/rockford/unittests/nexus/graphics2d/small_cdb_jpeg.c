/******************************************************************************
 *    (c)2008-2009 Broadcom Corporation
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
 * $brcm_Workfile: small_cdb_jpeg.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 3/7/11 3:17p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/small_cdb_jpeg.c $
 * 
 * Hydra_Software_Devel/1   3/7/11 3:17p ahulse
 * SW7422-256: Test app to show decode of a large jpeg with a compressed
 * data buffer smaller than jpeg file size
 *
 *
 *****************************************************************************/

/* Nexus example app: small_cdb_jpeg <cdb_buffer_size> <imagefile>
   example app to show how to use a small compressed data buffer size with
   a large jpeg file.

   nexus small_cdb_jpeg 128 myPicture.jpg - decodes pic myPicture.jpg with 128k cdb
*/

#include "nexus_platform.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#endif
#include "nexus_core_utils.h"
#include "nexus_picture_decoder.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#if 0
#include "btrc.h"
BTRC_MODULE(frame,ENABLE);
BTRC_MODULE(decode,ENABLE);
BTRC_MODULE(scale,ENABLE);
#else
#define BTRC_TRACE(x,y)
#define BTRC_Module_Report(x)
#define BTRC_Module_Reset(x)
#endif

BDBG_MODULE(small_cdb_jpeg);

struct app_context {
    NEXUS_DisplayHandle        display;
    NEXUS_Graphics2DHandle     gfx;
    NEXUS_PictureDecoderHandle pictureDecoder;
    NEXUS_PictureDecoderStatus pictureStatus;
    BKNI_EventHandle           graphicsSync;    /* event from graphics core to tell us graphics op has completed */
    BKNI_EventHandle           decoderStep;     /* event from single image decoder to tell us a segment has been decoded */
    BKNI_EventHandle           bufferFlip;      /* event from display to tell us set surface is now active */
};

static void buffer_flip_complete(void *context_, int unused)
{
    const struct app_context *context = context_;
    unused = unused;
    BKNI_SetEvent(context->bufferFlip);
    return;
}

static void graphics_sync_complete(void *context_, int unused)
{
    const struct app_context *context = context_;
    unused = unused;
    BKNI_SetEvent(context->graphicsSync);
    return;
}

static void graphics_sync_mark(const struct app_context *context)
{
    NEXUS_CallbackDesc callback = NEXUS_CALLBACKDESC_INITIALIZER();
    callback.callback = graphics_sync_complete;
    callback.context = (void *)context;
    NEXUS_Graphics2D_Checkpoint(context->gfx, &callback);
    return;
}

static void graphics_sync_check(const struct app_context *context)
{
    BKNI_WaitForEvent(context->graphicsSync, BKNI_INFINITE);
    return;
}

static void graphics_sync(const struct app_context *context)
{
    graphics_sync_mark(context);
    graphics_sync_check(context);
    return;
}

/* Callback called whenever image decoder has completed it' task ( ie segmented or full image decode etc ) */
static void picture_decoder_step(void *context_, int unused)
{
    const struct app_context *context = context_;
    unused = unused;
    BKNI_SetEvent(context->decoderStep);
    return;
}


int main(int argc, const char *argv[])
{
    NEXUS_SurfaceHandle framebuffer[2];
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PictureDecoderStartSettings pictureSettings;
    NEXUS_PictureDecoderOpenSettings pictureDecoderOpenSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    struct app_context context;
    FILE *fin ;
    void *buffer;
    size_t size;
    int file_size,read_size, i;
    NEXUS_Rect dest;
    unsigned width,height;
    unsigned bufr=128;          /* default buffer size to 128k bytes */
    int clipWidth, clipHeight;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    fprintf(stderr, "Panel output ready\n");
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_e720p;
    context.display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(context.display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#endif

    BKNI_CreateEvent(&context.graphicsSync);
    BKNI_CreateEvent(&context.bufferFlip);
    BKNI_CreateEvent(&context.decoderStep);

    /* use graphics to fit image to the display framebuffer */
    context.gfx = NEXUS_Graphics2D_Open(0, NULL);

    /* allocate framebuffer */
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    if(displaySettings.format == NEXUS_VideoFormat_e720p) {
        createSettings.width = 1280;
        createSettings.height = 720;
    } else if(displaySettings.format == NEXUS_VideoFormat_e1080i) {
        createSettings.width = 1920;
        createSettings.height = 1080;
    } else {
        createSettings.width = 720;
        createSettings.height = 480;
    }
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    framebuffer[0] = NEXUS_Surface_Create(&createSettings);
    framebuffer[1] = NEXUS_Surface_Create(&createSettings);

    NEXUS_Display_GetGraphicsSettings(context.display, &graphicsSettings);
    graphicsSettings.enabled                      = true;
    graphicsSettings.frameBufferCallback.callback = buffer_flip_complete;
    graphicsSettings.frameBufferCallback.context  = &context;
    graphicsSettings.position.width               = createSettings.width;
    graphicsSettings.position.height              = createSettings.height;
    graphicsSettings.clip.width                   = createSettings.width;
    graphicsSettings.clip.height                  = createSettings.height;
    NEXUS_Display_SetGraphicsSettings(context.display, &graphicsSettings);

    NEXUS_PictureDecoder_GetDefaultOpenSettings(&pictureDecoderOpenSettings);

    if(argc>1) {
        /* First argument has to be buffer size */
        bufr = atoi(argv[1]);
        if ( bufr == 0 ) {
            fprintf( stderr, "\nNeed to specify compressed data buffer size in kiloBytes for image decode\n" );
            return 1;
        }
    }
    pictureDecoderOpenSettings.bufferSize  = bufr*1024;
    pictureDecoderOpenSettings.smallBuffer = true;
    context.pictureDecoder = NEXUS_PictureDecoder_Open(0, &pictureDecoderOpenSettings);

    for(i=1;;) {
        char jpeg[128];
        NEXUS_SurfaceCreateSettings jpegSurfSettings;
        NEXUS_SurfaceHandle row;
        NEXUS_PictureDecoderStatus pictureStatus;
        NEXUS_Graphics2DBlitSettings blitSettings;
        bool readDataFromStart=false;              /* If header info of jpeg exceeds cdb size */

        if (argc > 2) {
            /* run as: nexus slide_jpeg *.jpg */
            if (i+1 == argc) {
                goto doneAllFiles;
                i = 1;
            }
            BKNI_Snprintf(jpeg, sizeof(jpeg), "%s", argv[i+1]);
        }
        else {
            BKNI_Snprintf(jpeg, sizeof(jpeg), "videos/picture%u.jpg", i);
        }
        printf("Opening file %s\n" , jpeg );
        fin = fopen(jpeg,"rb");
        if(!fin) {
            perror(jpeg);
            if(i==0) {
                break;
            }
            i=0;
            continue;
        }
        i++;


        fseek(fin, 0, SEEK_END);
        file_size = ftell(fin);
        fseek(fin, 0, SEEK_SET);

        NEXUS_PictureDecoder_GetBuffer(context.pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
        read_size = fread(buffer, 1, size, fin); /* read file into the decoder's buffer */
        if(read_size<0) {
            perror(jpeg);
            return 1;
        }

        /* Get buffer to read image data into upto cdb size */
        NEXUS_PictureDecoder_ReadComplete(context.pictureDecoder, 0, read_size); /* commit data to the decoder */

        /* Fill screen with grey so can see lengthy decode times when using small CDB buffer */
        NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
        fillSettings.surface     = framebuffer[0];
        fillSettings.color       = 0x80808080; /* grey */
        fillSettings.rect.x      = 0;
        fillSettings.rect.y      = 0;
        fillSettings.rect.width  = createSettings.width;
        fillSettings.rect.height = createSettings.height;
        fillSettings.colorOp     = NEXUS_FillOp_eCopy;
        fillSettings.alphaOp     = NEXUS_FillOp_eCopy;
        NEXUS_Graphics2D_Fill(context.gfx, &fillSettings);
        NEXUS_Display_SetGraphicsFramebuffer(context.display, framebuffer[0]);

        /* Get Default settings and then customize with our own */
        NEXUS_PictureDecoder_GetDefaultStartSettings(&pictureSettings);
            pictureSettings.format                  = NEXUS_PictureFormat_eJpeg;
            pictureSettings.pictureParsed.callback  = picture_decoder_step;
            pictureSettings.pictureParsed.context   = &context;
            pictureSettings.segmentDecoded.callback = picture_decoder_step;
            pictureSettings.segmentDecoded.context  = &context;
            pictureSettings.imageSize               = file_size;

        /* Start the image decode process */
        NEXUS_PictureDecoder_Start(context.pictureDecoder, &pictureSettings);
        BKNI_WaitForEvent(context.decoderStep, BKNI_INFINITE);

        NEXUS_PictureDecoder_GetStatus(context.pictureDecoder, &pictureStatus);
        if(pictureStatus.state==NEXUS_PictureDecoderState_eError) {
            fprintf(stderr, "decoding failed\n");
            return 1;
        } else if ( pictureStatus.state==NEXUS_PictureDecoderState_eMoreData ) {
            readDataFromStart = true;
            /* Get a new buffer full */
            do {
               NEXUS_PictureDecoder_GetBuffer(context.pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
               read_size = fread(buffer, 1, size, fin); /* read file into the decoder's buffer */

               if(read_size) {
                   NEXUS_PictureDecoder_ReadComplete(context.pictureDecoder, 0, read_size); /* commit data to the decoder */
                   BKNI_WaitForEvent(context.decoderStep, BKNI_INFINITE);
                   NEXUS_PictureDecoder_GetStatus(context.pictureDecoder, &pictureStatus);
                   if ( pictureStatus.headerValid ) {
                        /* SID has signalled it has enough data to decode image header, so stop reading further data */
                        break;
                   }
               }
            } while ( read_size );
        }

        BDBG_ASSERT(pictureStatus.headerValid);
        context.pictureStatus = pictureStatus;
        BDBG_MSG(("\n\n\t image w=%d h=%d surf.w=%d surf.h=%d ..picStatus.state= %d format=%d\n\n",
                pictureStatus.header.width, pictureStatus.header.height,
                pictureStatus.header.surface.width, pictureStatus.header.surface.height, 
                context.pictureStatus, context.pictureStatus.header.format ));

        width=createSettings.width;
        height=createSettings.height;

        dest.x = dest.y = 0;
        dest.width  = width; /* fill to fit entire screen */
        dest.height = height;

        /* create picture that could handle complete picture */
        NEXUS_Surface_GetDefaultCreateSettings(&jpegSurfSettings);
        jpegSurfSettings.pixelFormat = pictureStatus.header.format;
        jpegSurfSettings.width       = pictureStatus.header.surface.width;
        jpegSurfSettings.height      = pictureStatus.header.surface.height;
        jpegSurfSettings.alignment   = 2;
        switch (jpegSurfSettings.pixelFormat) {
        case NEXUS_PixelFormat_ePalette8:
        case NEXUS_PixelFormat_eA8:
            jpegSurfSettings.pitch = jpegSurfSettings.width;
            break;
        case NEXUS_PixelFormat_ePalette4:
            jpegSurfSettings.pitch = (jpegSurfSettings.width + 1) >> 1;
            break;
        case NEXUS_PixelFormat_ePalette2:
            jpegSurfSettings.pitch = (jpegSurfSettings.width + 3) >> 2;
            break;
        case NEXUS_PixelFormat_ePalette1:
            jpegSurfSettings.pitch = (jpegSurfSettings.width + 7) >> 3;
            break;
        case NEXUS_PixelFormat_eA8_Palette8:
            jpegSurfSettings.pitch = jpegSurfSettings.width << 1;
            break;
        default:
            break;
        }
        jpegSurfSettings.pitch = ((jpegSurfSettings.pitch + 3) & ~3);

        row = NEXUS_Surface_Create(&jpegSurfSettings);

        if ( readDataFromStart ) {
            fseek(fin, 0, SEEK_SET);
            NEXUS_PictureDecoder_GetBuffer(context.pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
            read_size = fread(buffer, 1, size, fin); /* read file into the decoder's buffer */

            if(read_size) {
                   NEXUS_PictureDecoder_ReadComplete(context.pictureDecoder, 0, read_size); /* commit data to the decoder */
            }
        }
        /* start decoding */
        BTRC_TRACE(decode, START);
        NEXUS_PictureDecoder_DecodeSegment(context.pictureDecoder, row, NULL);
        BKNI_WaitForEvent(context.decoderStep, BKNI_INFINITE);
        BTRC_TRACE(decode, STOP);
        NEXUS_PictureDecoder_GetStatus(context.pictureDecoder, &pictureStatus);
        if(pictureStatus.state==NEXUS_PictureDecoderState_eError) {
            fprintf(stderr, "decoding failed\n");
            NEXUS_Surface_Destroy(row);
            return 1;
        } else if ( pictureStatus.state==NEXUS_PictureDecoderState_eMoreData ) {
            /* Get a new buffer full */
            do { 
               NEXUS_PictureDecoder_GetBuffer(context.pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
               read_size = fread(buffer, 1, size, fin); /* read file into the decoder's buffer */
                            
               if(read_size) {
                   NEXUS_PictureDecoder_ReadComplete(context.pictureDecoder, 0, read_size); /* commit data to the decoder */
                   BKNI_WaitForEvent(context.decoderStep, BKNI_INFINITE);
                   NEXUS_PictureDecoder_GetStatus(context.pictureDecoder, &pictureStatus);
               }
            } while ( read_size );
            BKNI_WaitForEvent(context.decoderStep, BKNI_INFINITE); 
            NEXUS_PictureDecoder_GetStatus(context.pictureDecoder, &pictureStatus);
        }

        clipWidth  = pictureStatus.header.width;
        clipHeight = pictureStatus.header.height;


        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface     = row;
        blitSettings.source.rect.x      = 0;
        blitSettings.source.rect.y      = 0;
        blitSettings.source.rect.width  = clipWidth;
        blitSettings.source.rect.height = clipHeight;
        blitSettings.colorOp            = NEXUS_BlitColorOp_eCopySource;
        blitSettings.alphaOp            = NEXUS_BlitAlphaOp_eCopySource;

        NEXUS_Surface_GetCreateSettings(framebuffer[0], &createSettings);

        blitSettings.output.surface     = framebuffer[0];
        blitSettings.output.rect.x      = 0;
        blitSettings.output.rect.y      = 0;
        blitSettings.output.rect.width  = createSettings.width; /* fill to fit entire screen */
        blitSettings.output.rect.height = createSettings.height;

        NEXUS_Graphics2D_Blit(context.gfx, &blitSettings);  /* don't wait for blit to complete */
        NEXUS_Display_GetGraphicsSettings(context.display, &graphicsSettings);
        graphicsSettings.enabled = true;
        NEXUS_Display_SetGraphicsSettings(context.display, &graphicsSettings);
        NEXUS_Display_SetGraphicsFramebuffer(context.display, framebuffer[0]);

        printf("%s (%dx%d filesize=%d bufSize=%d) decoded \nPress a key to continue\n", 
            jpeg, pictureStatus.header.width, pictureStatus.header.height, file_size, bufr*1024 );
        getchar();   
        NEXUS_Surface_Destroy(row);

        /* Shutdown in the reverse order we opened things */
        fclose( fin );
        NEXUS_PictureDecoder_Stop(context.pictureDecoder);
    } /* end of for loop */

doneAllFiles:

    NEXUS_Surface_Destroy(framebuffer[1]);
    NEXUS_Surface_Destroy(framebuffer[0]);
    NEXUS_Graphics2D_Close(context.gfx);
    NEXUS_PictureDecoder_Close(context.pictureDecoder);

    BKNI_DestroyEvent(context.decoderStep);
    BKNI_DestroyEvent(context.bufferFlip);
    BKNI_DestroyEvent(context.graphicsSync);

    NEXUS_Display_Close(context.display);

    NEXUS_Platform_Uninit();

    return 0;
} /* end of main */

