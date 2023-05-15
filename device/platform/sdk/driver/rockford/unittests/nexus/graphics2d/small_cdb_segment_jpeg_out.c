/******************************************************************************
 *    (c)2008-2010 Broadcom Corporation
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
 * $brcm_Workfile: small_cdb_segment_jpeg_out.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 5/26/11 4:05p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/small_cdb_segment_jpeg_out.c $
 * 
 * Hydra_Software_Devel/2   5/26/11 4:05p ahulse
 * SW7422-256: Update app to use new SID fw
 * 
 * Hydra_Software_Devel/1   3/8/11 2:37p ahulse
 * SW7422-256: AAdd small cdb segmented output jpeg decode
 * 
 *****************************************************************************/
/* Nexus example app: show use of a small compressed data buffer and segmented jpeg decode  */

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

BDBG_MODULE(small_cdb_segment_out);

struct app_context {
    NEXUS_DisplayHandle        display;
    NEXUS_Graphics2DHandle     gfx;
    NEXUS_PictureDecoderHandle pictureDecoder;
    NEXUS_PictureDecoderStatus pictureStatus;
    BKNI_EventHandle           graphicsSync;
    BKNI_EventHandle           decoderStep;
    BKNI_EventHandle           bufferFlip;
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

static void picture_decoder_step(void *context_, int unused)
{
    const struct app_context *context = context_;
    unused = unused;
    BKNI_SetEvent(context->decoderStep);
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

int main(int argc, const char *argv[])
{
    NEXUS_SurfaceHandle          framebuffer, row[2];
    NEXUS_SurfaceCreateSettings  screenSurfaceSettings,rowSurfaceSettings;
    NEXUS_DisplaySettings        displaySettings;
    NEXUS_GraphicsSettings       graphicsSettings;
    NEXUS_PlatformSettings       platformSettings;
    NEXUS_PlatformConfiguration  platformConfig;
    NEXUS_PictureDecoderStatus   pictureStatus;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DBlitSettings rowBlitSettings;
    NEXUS_PictureDecoderStartSettings pictureSettings;
    NEXUS_PictureDecoderOpenSettings  pictureDecoderOpenSettings;
    unsigned block = 64;   /* segmented decode size needs to be a multiple of 16 for jpegs */
    bool readDataFromStart=false;
    unsigned y;
    unsigned dst_y;
    int prev_row,cur_row,bufr=128;

    struct app_context context;
    const char *jpeg = "videos/picture.jpg";
    FILE *fin ;
    void *buffer;
    size_t size;
    unsigned file_size,read_size, remainingBytes;
    NEXUS_Rect dest;

    if(argc>1) {
        jpeg = argv[1];
    }
    if(argc>2) {
        bufr = atoi(argv[2]);
    }
    if(argc>3) {
        block = atoi(argv[3]);
        if ( block%16 ) {
            printf( "Block=%d is not multiple of 16, recalc to %d\n" , block, (block/16)*16 );
            block = (block/16)*16;
            if ( block < 16 ) block = 16;
        }
    }

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
    displaySettings.format = NEXUS_VideoFormat_e720p;         /* Choose 720p so component only */
    context.display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(context.display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#endif

    fin = fopen(jpeg,"rb");
    if(!fin) {
        perror(jpeg);
        return 1;
    }

    NEXUS_PictureDecoder_GetDefaultOpenSettings(&pictureDecoderOpenSettings);
    pictureDecoderOpenSettings.bufferSize  = bufr*1024;  /* use a 128k CDB buffer if user doesn't set. */
    BDBG_MSG(("set CDB to %d kBytes" , pictureDecoderOpenSettings.bufferSize ));
    context.pictureDecoder = NEXUS_PictureDecoder_Open(0, &pictureDecoderOpenSettings); 
    fseek(fin, 0, SEEK_END);
    file_size = ftell(fin);
    fseek(fin, 0, SEEK_SET);
    remainingBytes = file_size;

    BKNI_CreateEvent(&context.graphicsSync);
    BKNI_CreateEvent(&context.decoderStep);
    BKNI_CreateEvent(&context.bufferFlip);

    context.gfx = NEXUS_Graphics2D_Open(0, NULL);

    /* allocate framebuffer */
    NEXUS_Surface_GetDefaultCreateSettings(&screenSurfaceSettings);
    screenSurfaceSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    if(displaySettings.format == NEXUS_VideoFormat_e720p) {
        screenSurfaceSettings.width  = 1280;
        screenSurfaceSettings.height = 720;
    } else if(displaySettings.format == NEXUS_VideoFormat_e1080i) {
        screenSurfaceSettings.width  = 1920;
        screenSurfaceSettings.height = 1080;
    } else {
        screenSurfaceSettings.width  = 720;
        screenSurfaceSettings.height = 480;
    }
    screenSurfaceSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    framebuffer = NEXUS_Surface_Create(&screenSurfaceSettings);

    NEXUS_Display_GetGraphicsSettings(context.display, &graphicsSettings);
    graphicsSettings.enabled                      = true;
    graphicsSettings.frameBufferCallback.callback = buffer_flip_complete;
    graphicsSettings.frameBufferCallback.context  = &context;
    graphicsSettings.position.width               = screenSurfaceSettings.width;
    graphicsSettings.position.height              = screenSurfaceSettings.height;
    graphicsSettings.clip.width                   = screenSurfaceSettings.width;
    graphicsSettings.clip.height                  = screenSurfaceSettings.height;
    NEXUS_Display_SetGraphicsSettings(context.display, &graphicsSettings);


    NEXUS_PictureDecoder_GetBuffer(context.pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
    read_size = fread(buffer, 1, size, fin);                                /* read file into the decoder's buffer */
    if(!read_size) {
        perror(jpeg);
        return 1;
    }

    NEXUS_PictureDecoder_ReadComplete(context.pictureDecoder, 0, read_size); /* commit data to the decoder */

    /* Draw grey background so we can clearly see picture being drawn */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface     = framebuffer;
    fillSettings.color       = 0x80808080;
    fillSettings.rect.x      = 0;
    fillSettings.rect.y      = 0;
    fillSettings.rect.width  = screenSurfaceSettings.width;
    fillSettings.rect.height = screenSurfaceSettings.height;
    fillSettings.colorOp     = NEXUS_FillOp_eCopy;
    fillSettings.alphaOp     = NEXUS_FillOp_eCopy;
    NEXUS_Graphics2D_Fill(context.gfx, &fillSettings);

    NEXUS_PictureDecoder_GetDefaultStartSettings(&pictureSettings);
    pictureSettings.format                  = NEXUS_PictureFormat_eJpeg;
    pictureSettings.pictureParsed.callback  = picture_decoder_step;
    pictureSettings.pictureParsed.context   = &context;
    pictureSettings.segmentDecoded.callback = picture_decoder_step;
    pictureSettings.segmentDecoded.context  = &context;
    if( file_size > pictureDecoderOpenSettings.bufferSize ) {
        pictureSettings.imageSize               = file_size;
    }

    /* Start the image decode process , by decoding jpeg header */
    NEXUS_PictureDecoder_Start(context.pictureDecoder, &pictureSettings);
    BKNI_WaitForEvent(context.decoderStep, BKNI_INFINITE);

    NEXUS_PictureDecoder_GetStatus(context.pictureDecoder, &pictureStatus);
    if(pictureStatus.state==NEXUS_PictureDecoderState_eError) {
        fprintf(stderr, "decoding failed\n");
        return 1;
    } else if ( pictureStatus.state==NEXUS_PictureDecoderState_eMoreData ) {
        readDataFromStart = true;
        /* If cdb size is less than required to decode header, read in until header is decoded */
        do {
           NEXUS_PictureDecoder_GetBuffer(context.pictureDecoder, &buffer, &size); 
           read_size = fread(buffer, 1, size, fin); 

           if(read_size) {
               NEXUS_PictureDecoder_ReadComplete(context.pictureDecoder, 0, read_size); 
               BKNI_WaitForEvent(context.decoderStep, BKNI_INFINITE);
               NEXUS_PictureDecoder_GetStatus(context.pictureDecoder, &pictureStatus);
               if ( pictureStatus.headerValid ) {
                    BDBG_MSG(("Read in enough data to decode header, break loop !" ));
                break;
               }
           }
        } while ( read_size );
    }
    readDataFromStart = true;

    BDBG_ASSERT(pictureStatus.headerValid);
    context.pictureStatus = pictureStatus;
    BDBG_WRN(("\n\n\t\tpic w=%d h=%d surf.w=%d surf.h=%d ..picStatus.state= %d format=%d\n\n",
        pictureStatus.header.width, pictureStatus.header.height,
        pictureStatus.header.surface.width, pictureStatus.header.surface.height,
        context.pictureStatus, context.pictureStatus.header.format ));

    /* create shrink picture into screen effect */
    dest.x      = 0;
    dest.y      = 0;
    dest.width  = screenSurfaceSettings.width; 
    dest.height = screenSurfaceSettings.height;

    BTRC_TRACE(frame, START);

    /* create picture that could handle complete picture */
    NEXUS_Surface_GetDefaultCreateSettings(&rowSurfaceSettings);
    rowSurfaceSettings.pixelFormat = context.pictureStatus.header.format;
    rowSurfaceSettings.width       = context.pictureStatus.header.surface.width;
    rowSurfaceSettings.height      = block;
    row[0] = NEXUS_Surface_Create(&rowSurfaceSettings);
    row[1] = NEXUS_Surface_Create(&rowSurfaceSettings);

    if ( readDataFromStart ) {
        fseek(fin, 0, SEEK_SET);
        remainingBytes = file_size;
        NEXUS_PictureDecoder_GetBuffer(context.pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
        read_size = fread(buffer, 1, size, fin); /* read file into the decoder's buffer */

        if(read_size) {
            NEXUS_PictureDecoder_ReadComplete(context.pictureDecoder, 0, read_size); /* commit data to the decoder */
            remainingBytes -= read_size;
        }
    }

    for(prev_row=-1,cur_row=0, dst_y=y=0; y<context.pictureStatus.header.height;y+=block)  {
        NEXUS_Rect rect;
        rect.x = 0;
        rect.width = context.pictureStatus.header.surface.width;
        rect.y = y;
        if(rect.y + block <= context.pictureStatus.header.surface.height) {
            rect.height = block;
        } else {
            rect.height = context.pictureStatus.header.surface.height - rect.y;
        }

        /* start decoding */
        BTRC_TRACE(decode, START);
        NEXUS_PictureDecoder_DecodeSegment(context.pictureDecoder, row[cur_row], &rect); 
        BKNI_WaitForEvent(context.decoderStep, BKNI_INFINITE);
        BTRC_TRACE(decode, STOP);
        NEXUS_PictureDecoder_GetStatus(context.pictureDecoder, &pictureStatus);
        if(pictureStatus.state==NEXUS_PictureDecoderState_eError) {
            fprintf(stderr, "decoding failed\n");
            NEXUS_Surface_Destroy(row[0]); 
            NEXUS_Surface_Destroy(row[1]); 
           return 1;
        } else if ( pictureStatus.state==NEXUS_PictureDecoderState_eMoreData ) {
            /* Get a new buffer full */
            do {
                NEXUS_PictureDecoder_GetBuffer(context.pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
                if( remainingBytes < size ) size = remainingBytes;
                read_size = fread(buffer, 1, size, fin); /* read file into the decoder's buffer */

                if(read_size) {
                    NEXUS_PictureDecoder_ReadComplete(context.pictureDecoder, 0, read_size); /* commit data to the decoder */
                    remainingBytes -= read_size;
                    BKNI_WaitForEvent(context.decoderStep, BKNI_INFINITE);
                    NEXUS_PictureDecoder_GetStatus(context.pictureDecoder, &pictureStatus);
                    if ( pictureStatus.state==NEXUS_PictureDecoderState_eSegmentDone ) { 
                        break;
                   }
                }
            } while ( read_size );
            
            if ( !read_size ) {
                BKNI_WaitForEvent(context.decoderStep, BKNI_INFINITE);
                NEXUS_PictureDecoder_GetStatus(context.pictureDecoder, &pictureStatus);
            }
        }


        BDBG_ASSERT(pictureStatus.state==NEXUS_PictureDecoderState_eSegmentDone);
        if(prev_row!=-1) {
            BTRC_TRACE(scale, STOP);
            graphics_sync(&context);
        }
        NEXUS_Graphics2D_GetDefaultBlitSettings(&rowBlitSettings);
        rowBlitSettings.source.surface    = row[cur_row];
        rowBlitSettings.source.rect.x     = 0;
        rowBlitSettings.source.rect.y     = 0;
        rowBlitSettings.source.rect.width = context.pictureStatus.header.width;
        if(NEXUS_PIXEL_FORMAT_IS_YCRCB(context.pictureStatus.header.format)) {
            rowBlitSettings.source.rect.width += rowBlitSettings.source.rect.width%2; /* YCrCb single pixel has width of 2 */
        }
        if(rect.y + block <= context.pictureStatus.header.surface.height) {
            rowBlitSettings.source.rect.height = block;
        } else {
            rowBlitSettings.source.rect.height = context.pictureStatus.header.height - rect.y;
        }
        rowBlitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
        rowBlitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;

        rowBlitSettings.output.surface     = framebuffer;
        rowBlitSettings.output.rect.x      = dest.x;
        rowBlitSettings.output.rect.width  = dest.width;
        rowBlitSettings.output.rect.y      = dest.y + dst_y;
        rowBlitSettings.output.rect.height = ((rect.y+rowBlitSettings.source.rect.height)*dest.height)/context.pictureStatus.header.height - dst_y;

        BDBG_WRN(("blit from x=%3d y=%3d w=%3d h=%3d" , rowBlitSettings.source.rect.x, rowBlitSettings.source.rect.y,
                                                        rowBlitSettings.source.rect.width, rowBlitSettings.source.rect.height ));

        BDBG_WRN(("blit to   x=%3d y=%3d w=%3d h=%3d" , rowBlitSettings.output.rect.x, rowBlitSettings.output.rect.y,
                                                        rowBlitSettings.output.rect.width, rowBlitSettings.output.rect.height ));

        dst_y += rowBlitSettings.output.rect.height;
        BTRC_TRACE(scale, START);
        NEXUS_Graphics2D_Blit(context.gfx, &rowBlitSettings);  
        graphics_sync_mark(&context);
        prev_row = cur_row;
        cur_row  = (cur_row+1)%2;
        NEXUS_Display_SetGraphicsFramebuffer(context.display, framebuffer );

        getchar(); /* re-enable this to "step" row by row decoding and display of image */
    }
    BTRC_TRACE(scale, STOP);
    graphics_sync_check(&context);
    NEXUS_Surface_Destroy(row[0]); 
    NEXUS_Surface_Destroy(row[1]); 

    BTRC_TRACE(frame, STOP);
    NEXUS_PictureDecoder_Stop(context.pictureDecoder);

    NEXUS_Display_SetGraphicsFramebuffer(context.display, framebuffer);
    BKNI_WaitForEvent(context.bufferFlip, BKNI_INFINITE);
    BTRC_Module_Report(BTRC_MODULE_HANDLE(frame));
    BTRC_Module_Report(BTRC_MODULE_HANDLE(decode));
    BTRC_Module_Report(BTRC_MODULE_HANDLE(scale));
    BTRC_Module_Reset(BTRC_MODULE_HANDLE(decode));
    BTRC_Module_Reset(BTRC_MODULE_HANDLE(scale));

    printf( "\nPress Enter to end program\n" );
    getchar();

    return 0;
}

