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
 * $brcm_Revision: 28 $
 * $brcm_Date: 2/29/12 4:46p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/graphics/picture_decoder.c $
 * 
 * 28   2/29/12 4:46p erickson
 * SW7405-3671: always WaitForEvent after Checkpoint
 * 
 * 27   1/25/12 5:22p rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 26   1/13/12 7:58p erickson
 * SW7425-1136: add HDMI output, remove unused code
 * 
 * 25   10/20/11 12:18p erickson
 * SW7231-199: add hdmi output
 * 
 * 24   6/27/11 2:49p erickson
 * SW7405-5356: add required checkpoint
 * 
 * 23   6/24/11 11:23a erickson
 * SW7420-256: picture decoder to use smaller than filesize compressed
 *  data buffer for picture decoding with improved SID firmware
 * 
 * 22   4/8/11 1:50p erickson
 * SW7335-704: update examples for settops that have SID
 *
 * 21   2/22/11 4:16p erickson
 * SWDTV-5278: BMP is not supported by SID
 *
 * 20   2/22/11 4:08p erickson
 * SWDTV-5278: prevent infinite loop for short file
 *
 * 19   2/22/11 3:51p erickson
 * SWDTV-5278: transfer palette from SID to nexus surface using
 *  NEXUS_PictureDecoder_GetPalette
 *
 * 18   2/22/11 2:42p erickson
 * SWDTV-5278: use strcasestr. set required pitch and alignment for
 *  palettized surfaces.
 *
 * 17   2/18/11 4:27p erickson
 * SW35230-2905: fix fullscreen blit, fix ARC violation on quit, add
 *  composite output
 *
 * 16   2/9/11 4:55p erickson
 * SW35230-2905: update app to process list of images
 *
 * 15   11/29/10 2:58p erickson
 * SW7420-1078: add NEXUS_Platform_GetFramebufferHeap
 *
 * 14   8/17/10 4:05p erickson
 * SW7420-703: update
 *
 * 13   5/4/10 3:45p dlwin
 * SW3548-2919: Added sample code use application provided buffer for
 *  PictureDecoder.  Also added code to cleanup on exit.
 *
 * 12   11/2/09 4:44p erickson
 * SW3548-2579: support .es and larger pictures
 *
 * 11   11/2/09 2:49p erickson
 * SW3548-2579: convert app to generic NEXUS_PictureDecoder example
 *
 * 10   8/24/09 5:42p vsilyaev
 * SW3548-1911: Round-up witdh for YCrCB pixels
 *
 * 9   2/20/09 2:58p mphillip
 * PR52359: Added panel initialization
 *
 * 8   10/8/08 1:01p vsilyaev
 * PR 46142: Added 'rotated' bit into the picture header
 *
 * 7   9/30/08 4:36p vsilyaev
 * PR 46142: Allocate multiscan buffer and increase size of compressed
 *  buffer
 *
 * 6   8/20/08 4:50p erickson
 * PR45866: update NEXUS_Graphics2D_Open
 *
 * 5   7/2/08 6:14p vsilyaev
 * PR 44481: Added code to detect oversize JPEG files
 *
 * 4   6/27/08 2:42p vsilyaev
 * PR 40870: Removed debug code
 *
 * 3   6/19/08 2:32p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 *
 * 2   6/12/08 7:01p vsilyaev
 * PR 40670: Fixed to work on a real HW
 *
 * 1   4/25/08 4:09p vsilyaev
 * PR 40870: Sample application to use HW JPEG decoder
 *
 *****************************************************************************/
/* Nexus example app: show jpeg image */
#include "nexus_platform.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_core_utils.h"
#if NEXUS_HAS_PICTURE_DECODER
#include "nexus_picture_decoder.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

BDBG_MODULE(picture_decoder);

#define USE_APPLICATION_BUFFER 0

#if NEXUS_HAS_PICTURE_DECODER
static void complete(void *context, int param)
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

int main(int argc, const char *argv[])
{
    NEXUS_SurfaceHandle framebuffer;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle checkpointEvent;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PictureDecoderStartSettings pictureSettings;
    NEXUS_PictureDecoderStatus pictureStatus;
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_PictureDecoderOpenSettings decoderSettings;
    NEXUS_VideoFormatInfo videoFormatInfo;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
    NEXUS_Error errCode;
#endif
    int pictureBufferSize;
    int multiScanBufferSize;
    void *pPictureBuffer;
    void *pMultiScanBuffer;
#if USE_APPLICATION_BUFFER
    NEXUS_MemoryAllocationSettings allocationSettings;
#endif
    int i;
    void *buffer;
    size_t size,file_size;
    int rc;

    if(argc < 2) {
        argv[1] = "videos/picture.jpg";
        argc = 2;
    }

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);


    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    errCode = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !errCode && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format. 
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
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
    framebuffer = NEXUS_Surface_Create(&createSettings);

    /* use graphics to fit image into the display framebuffer */
    gfx = NEXUS_Graphics2D_Open(0, NULL);
    BKNI_CreateEvent(&checkpointEvent);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);

    for (i=1;i<argc;i++) {
        FILE *fin;
        const char *pictureFilename = argv[i];
        NEXUS_PictureDecoderHandle pictureDecoder = NULL;
        NEXUS_SurfaceHandle picture = NULL;
        unsigned cnt;
        size_t bytesRemain =0;

        BDBG_WRN(("decoding %s...", pictureFilename));
        fin = fopen(pictureFilename,"rb");
        if(!fin) {
            perror(pictureFilename);
            continue;
        }

        multiScanBufferSize = 16*1024*1024;
        pictureBufferSize = 128*1024;
    #if USE_APPLICATION_BUFFER
        printf(" Using application provided buffers for PictureDecoder...\n");
        /* Get heap from system */
        /* Allocate memory from heap, this memory will be used by Picture Decoder */
        NEXUS_Memory_GetDefaultAllocationSettings(&allocationSettings);
        /* Use the default heap to allocate memory from */
        rc = NEXUS_Memory_Allocate(multiScanBufferSize, &allocationSettings, &pMultiScanBuffer);
        BDBG_ASSERT(!rc);
        /* Reuse alloctionSettings from available */
        rc = NEXUS_Memory_Allocate(pictureBufferSize, &allocationSettings, &pPictureBuffer);
        BDBG_ASSERT(!rc);
    #else
        /* Let PictuerDecoder allocation memory */
        printf(" Using PictureDecoder provided buffers...\n");
        pMultiScanBuffer = NULL;
        pPictureBuffer = NULL;
    #endif

        NEXUS_PictureDecoder_GetDefaultOpenSettings(&decoderSettings);
        decoderSettings.multiScanBufferSize = multiScanBufferSize;
        decoderSettings.bufferSize = pictureBufferSize;
        decoderSettings.multiScanBufferPtr = pMultiScanBuffer;
        decoderSettings.bufferPtr = pPictureBuffer;
        pictureDecoder = NEXUS_PictureDecoder_Open(0, &decoderSettings);

        /* Determine file size of image to display */
        fseek(fin, 0, SEEK_END);
        file_size = ftell(fin);
        fseek(fin, 0, SEEK_SET);

        NEXUS_PictureDecoder_GetDefaultStartSettings(&pictureSettings);
        if (strcasestr(pictureFilename, ".mpg") || strcasestr(pictureFilename, ".es")) {
            pictureSettings.format = NEXUS_PictureFormat_eMpeg;
        }
        else if (strcasestr(pictureFilename, ".png")) {
            pictureSettings.format = NEXUS_PictureFormat_ePng;
        }
        else if (strcasestr(pictureFilename, ".gif")) {
            pictureSettings.format = NEXUS_PictureFormat_eGif;
        }
        else if (strcasestr(pictureFilename, ".bmp")) {
            BDBG_ERR(("BMP is not supported"));
            goto error;
        }
        else {
            pictureSettings.format = NEXUS_PictureFormat_eJpeg;
        }
        pictureSettings.imageSize = file_size;

        NEXUS_PictureDecoder_GetBuffer(pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
        rc = fread(buffer, 1, size, fin);                               /* read file into the decoder's buffer */
        if(rc<0) {
            perror(pictureFilename);
            goto error;
        }
        else if(rc==(int)size) {
            if( (unsigned)file_size > size && pictureSettings.format == NEXUS_PictureFormat_eMpeg ) {
                fprintf(stderr, "Picture file size %u is larger then buffer size %u, not supported for MPEG still frames\n", (unsigned)file_size, (unsigned)size);
                goto error;
            }
        }
        NEXUS_PictureDecoder_ReadComplete(pictureDecoder, 0, rc); /* commit data to the decoder */
        bytesRemain = pictureSettings.imageSize - size ;

        NEXUS_PictureDecoder_Start(pictureDecoder, &pictureSettings);

        cnt = 0;
        do {
            NEXUS_PictureDecoder_GetStatus(pictureDecoder, &pictureStatus);
            if(pictureStatus.state==NEXUS_PictureDecoderState_eError) {
                fprintf(stderr, "decoding failed\n");
                goto error;
            } else if ( pictureStatus.state==NEXUS_PictureDecoderState_eMoreData ) {
                rc = NEXUS_PictureDecoder_GetBuffer(pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
                if ( size > bytesRemain ) size = bytesRemain;

                if ( rc == NEXUS_SUCCESS ) {
                    rc = fread(buffer, 1, size, fin);                             /* read file into the decoder's buffer */

                    if(rc) {
                        NEXUS_PictureDecoder_ReadComplete(pictureDecoder, 0, rc); /* commit data to the decoder */
                        bytesRemain -= size;
                    }
                    else {
                        BDBG_MSG(( "COULDN'T READ IN any more data !!! rc=%d bytesRemain=%d\n" , rc, bytesRemain  ));
                        cnt = 999;
                    }
                }
            }
            usleep(1000);
            if (++cnt == 1000) { /* 1 second */
                BDBG_ERR(("unable to read header. aborting."));
                goto error;
            }
        } while(!pictureStatus.headerValid); /* wait for picture dimensions */

        /* create picture that could handle complete picture */
        NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
        createSettings.pixelFormat = pictureStatus.header.format;
        createSettings.width = pictureStatus.header.surface.width;
        createSettings.height = pictureStatus.header.surface.height;
        createSettings.alignment = 2;
        switch (createSettings.pixelFormat) {
        case NEXUS_PixelFormat_ePalette8:
        case NEXUS_PixelFormat_eA8:
            createSettings.pitch = createSettings.width;
            break;
        case NEXUS_PixelFormat_ePalette4:
            createSettings.pitch = (createSettings.width + 1) >> 1;
            break;
        case NEXUS_PixelFormat_ePalette2:
            createSettings.pitch = (createSettings.width + 3) >> 2;
            break;
        case NEXUS_PixelFormat_ePalette1:
            createSettings.pitch = (createSettings.width + 7) >> 3;
            break;
        case NEXUS_PixelFormat_eA8_Palette8:
            createSettings.pitch = createSettings.width << 1;
            break;
        default:
            break;
        }
        createSettings.pitch = ((createSettings.pitch + 3) & ~3);
        picture = NEXUS_Surface_Create(&createSettings);
        BDBG_WRN(("creating surface: format %d, %dx%d, pitch %d", createSettings.pixelFormat, createSettings.width, createSettings.height, createSettings.pitch));

        /* copy palette if needed */
        if (NEXUS_PIXEL_FORMAT_IS_PALETTE(createSettings.pixelFormat)) {
            NEXUS_PictureDecoderPalette sidPalette;
            NEXUS_SurfaceMemory mem;

            rc = NEXUS_PictureDecoder_GetPalette(pictureDecoder, &sidPalette);
            BDBG_ASSERT(!rc);
            NEXUS_Surface_GetMemory(picture, &mem),
            BKNI_Memcpy(mem.palette, sidPalette.palette, mem.numPaletteEntries*sizeof(NEXUS_PixelFormat));
            NEXUS_Surface_Flush(picture);
        }

        if ( file_size > (unsigned)pictureBufferSize ) {
            fprintf(stderr, "File requires multi part read, SID fw requires read from start\n");
            fseek(fin, 0, SEEK_SET);
            NEXUS_PictureDecoder_GetBuffer(pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
            rc = fread(buffer, 1, size, fin);                               /* read file into the decoder's buffer */

            if(rc) {
                   NEXUS_PictureDecoder_ReadComplete(pictureDecoder, 0, rc); /* commit data to the decoder */
            }
            else {
                fprintf(stderr, "failed to read in data for %s \n" , pictureFilename );
                goto error;
            }
        }

        /* start decoding */
        NEXUS_PictureDecoder_DecodeSegment(pictureDecoder, picture, NULL);
        do {
            NEXUS_PictureDecoder_GetStatus(pictureDecoder, &pictureStatus);
            if(pictureStatus.state==NEXUS_PictureDecoderState_eError) {
                fprintf(stderr, "decoding failed\n");
                goto error;
            } else if ( pictureStatus.state==NEXUS_PictureDecoderState_eMoreData ) {
                rc = NEXUS_PictureDecoder_GetBuffer(pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
                if ( rc == NEXUS_SUCCESS ) {
                    rc = fread(buffer, 1, size, fin);                             /* read file into the decoder's buffer */
                    if ( size > bytesRemain ) size = bytesRemain;

                    if(rc) {
                        NEXUS_PictureDecoder_ReadComplete(pictureDecoder, 0, rc); /* commit data to the decoder */
                        bytesRemain -= size;
                    }
                    else {
                        fprintf( stderr, "rc=%d size=%d ..COULDN'T READ IN any more data !!! \n", rc, size );
                        goto error;
                    }
                }
                else {
                    BDBG_MSG(("failed to get buffer to read data into\n" ));
                }
            }
            usleep(1000);
        } while(pictureStatus.state!=NEXUS_PictureDecoderState_eSegmentDone);   /* wait for picture to decode */

        NEXUS_PictureDecoder_Stop(pictureDecoder);

        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = picture;
        blitSettings.source.rect.x = 0;
        blitSettings.source.rect.y = 0;
        blitSettings.source.rect.width = pictureStatus.header.width;
        if(NEXUS_PIXEL_FORMAT_IS_YCRCB(pictureStatus.header.format)) {
            blitSettings.source.rect.width += blitSettings.source.rect.width%2; /* YCrCb single pixel has width of 2 */
        }

        blitSettings.source.rect.height = pictureStatus.header.height;
        blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
        blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;

        blitSettings.output.surface = framebuffer;
        blitSettings.output.rect.x = 0;
        blitSettings.output.rect.y = 0;
        blitSettings.output.rect.width = videoFormatInfo.width; /* fill to fit entire screen */
        blitSettings.output.rect.height = videoFormatInfo.height;

        NEXUS_Graphics2D_Blit(gfx, &blitSettings);              /* don't wait for blit to complete */
        rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
        if (rc == NEXUS_GRAPHICS2D_QUEUED) {
            rc = BKNI_WaitForEvent(checkpointEvent, BKNI_INFINITE);
        }
        BDBG_ASSERT(!rc);

        NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
        graphicsSettings.enabled = true;
        NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
        NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);

error:
        fprintf(stderr, "Press ENTER for next picture\n");
        getchar();

        /* Now clean up in reverse order */
        if (picture) {
            NEXUS_Surface_Destroy(picture);
        }
        if (pictureDecoder) {
            NEXUS_PictureDecoder_Close(pictureDecoder);
        }
        if (pPictureBuffer) {
            NEXUS_Memory_Free(pPictureBuffer);
        }
        if (pMultiScanBuffer) {
            NEXUS_Memory_Free(pMultiScanBuffer);
        }
    }

    NEXUS_Display_Close(display);
    NEXUS_Surface_Destroy(framebuffer);
    NEXUS_Graphics2D_Close(gfx);
    BKNI_DestroyEvent(checkpointEvent);
    NEXUS_Platform_Uninit();

    return 0;
}
#else
int main(void) {printf("picture decoder not supported\n");return -1;}
#endif
