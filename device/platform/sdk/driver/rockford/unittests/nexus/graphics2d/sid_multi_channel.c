/******************************************************************************
 *    (c)2008-2011 Broadcom Corporation
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
 * $brcm_Workfile: sid_multi_channel.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 1/12/12 1:54p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/sid_multi_channel.c $
 * 
 * Hydra_Software_Devel/2   1/12/12 1:54p ahulse
 * SW7425-1619: Allow app to use more picture decodes than available SID
 * decode channels
 * 
 * Hydra_Software_Devel/1   12/9/11 10:44a ahulse
 * SW7425-1619: add sid multi channel test app
 * 
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_surface.h"
#if NEXUS_HAS_GRAPHICS2D
#include "nexus_graphics2d.h"
#endif

#if NEXUS_HAS_PICTURE_DECODER
#include "nexus_picture_decoder.h"
#endif

#include "nexus_display.h"
#include "nexus_composite_output.h"
#include "nexus_component_output.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

BDBG_MODULE(twoSidCh);

#define MY_BDBG_WRN /* BDBG_WRN */

#if NEXUS_HAS_PICTURE_DECODER /* NEXUS_HAS_GRAPHICS2D */
/* this data is setup by the main thread, but used by each blit thread */
struct app_data {
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_SurfaceHandle surface;
    int  start; /* used to start threads cleanly */
    bool done;  /* used to terminate threads cleanly */
    char picFilename[128];
    int bufferSize;        /* size of picture decode buffer, can be less than picture file size */
    int threadNr;
} g_data;

void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void checkpoint(NEXUS_Graphics2DHandle gfx, BKNI_EventHandle checkpointEvent)
{
    int rc;
    rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
        BKNI_WaitForEvent(checkpointEvent, BKNI_INFINITE);
    }
}

/* code adapted from nexus/examples/graphics/picture_decoder.c */
static int decode_picture(NEXUS_PictureDecoderHandle pictureDecoder, unsigned bufferSize, NEXUS_SurfaceHandle destSurface, const char *pictureFilename, NEXUS_Graphics2DHandle gfx, BKNI_EventHandle checkpointEvent , NEXUS_Graphics2DFillSettings fillSettings, int threadNr )
{
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_PictureDecoderStartSettings pictureSettings;
    NEXUS_PictureDecoderStatus pictureStatus;
    NEXUS_Graphics2DBlitSettings blitSettings;
    void *buffer;
    size_t size,file_size;
    int rc = -1;
    FILE *fin;
    NEXUS_SurfaceHandle picture = NULL;
    unsigned cnt;
    size_t bytesRemain = 0;

    BDBG_WRN(("decoding dec=%p %s...\n\n", pictureDecoder, pictureFilename));
    fin = fopen(pictureFilename,"rb");
    if(!fin) {
        perror(pictureFilename);
        goto error;
    }

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

    while (1) {
        /* this while(1) loop is temporarily necessary for a simple virtualization. if this function fails with NEXUS_NOT_AVAILABLE,
        the underlying resource is being used. When the SID FW is rewritten for multicontext support, this loop can be removed. */
        rc = NEXUS_PictureDecoder_GetBuffer(pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
        if (!rc) break; /* we have it */
        if (rc == NEXUS_NOT_AVAILABLE) {
            BDBG_WRN(("decoder=%p threadNr=%d can't get a decoder slot, waiting.." , pictureDecoder, threadNr ));
            BKNI_Sleep(1);
        }
        else {
            BDBG_WRN(("\n\n\n\t\tjmp to err from L%d\n\n\n" , __LINE__ ));
            goto error;
        }
    }

    rc = fread(buffer, 1, size, fin);                               /* read file into the decoder's buffer */
    if(rc<0) {
        perror(pictureFilename);
        BDBG_WRN(("\n\n\n\t\tjmp to err from L%d\n\n\n" , __LINE__ ));
        goto error;
    }
    else if(rc==(int)size) {
        if( (unsigned)file_size > size && pictureSettings.format == NEXUS_PictureFormat_eMpeg ) {
            fprintf(stderr, "Picture file size %u is larger then buffer size %u, not supported for MPEG still frames\n", (unsigned)file_size, (unsigned)size);
            goto error;
        }
    }
    rc = NEXUS_PictureDecoder_ReadComplete(pictureDecoder, 0, rc); /* commit data to the decoder */
    BDBG_ASSERT(!rc);
    bytesRemain = pictureSettings.imageSize - size ;

    rc = NEXUS_PictureDecoder_Start(pictureDecoder, &pictureSettings);
    if ( rc ) BDBG_WRN(("Unable to start picture decoder"));
    BDBG_ASSERT(!rc);

    cnt = 0;
    do {
        rc = NEXUS_PictureDecoder_GetStatus(pictureDecoder, &pictureStatus);
        BDBG_ASSERT(!rc);
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
                    BDBG_MSG(( "L%d COULDN'T READ IN any more data !!! rc=%d bytesRemain=%d\n" , __LINE__ , rc, bytesRemain  ));
                    cnt = 999;
                }
            }
        }
        usleep(1000);
        if (++cnt == 1000) { /* 1 second */
            BDBG_ERR(("unable to read header (state %d). aborting.", pictureStatus.state));
            goto error;
        }
    } while(!pictureStatus.headerValid); /* wait for picture dimensions */

    BDBG_MSG(("decoded header of %s...\n\n", pictureFilename));

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

    if ( picture == NULL ) {
        BDBG_WRN(("\n\n\n\t\tCan't alloc surface mem for dimensions w=%d h=%d, abort decode !!!\n\n\n" , createSettings.width, createSettings.height ));
        BDBG_ASSERT(picture); 
        goto error;
    }

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

    BDBG_MSG(("Header Decoded, now decode image, need to re-read data from start" ));
    bytesRemain = pictureSettings.imageSize;
    fseek(fin, 0, SEEK_SET);
    BDBG_MSG(("file pos =%d bytesRemain=%d " , ftell(fin), bytesRemain ));

    NEXUS_PictureDecoder_GetBuffer(pictureDecoder, &buffer, &size); /* get location and size of the decoder's buffer */
    BDBG_MSG(("L%d bufr=%p size=%d" , __LINE__ , buffer, size ));
    rc = fread(buffer, 1, size, fin);                               /* read file into the decoder's buffer */
    if(rc) {
        NEXUS_PictureDecoder_ReadComplete(pictureDecoder, 0, rc); /* commit data to the decoder */
        bytesRemain -= size;
    }
    else {
        fprintf(stderr, "L%d failed to read in data for %s rc=%d size=%d\n" , __LINE__ , pictureFilename , rc, size );
        goto error;
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
                BDBG_MSG(("L%d MORE DATA...bytesRemain=%d rc=%d bufr=%p size=%d" , __LINE__ , bytesRemain, rc, buffer, size ));
                if ( rc == NEXUS_SUCCESS ) {
                    rc = fread(buffer, 1, size, fin);                             /* read file into the decoder's buffer */
                    if ( size > bytesRemain ) size = bytesRemain;

                }
                else {
                    BDBG_MSG(("L%d failed to get buffer to read data into\n" , __LINE__ ));
                }

                /* ReadComplete can happen independently of getBuffer */
                if (rc) {
                    NEXUS_PictureDecoder_ReadComplete(pictureDecoder, 0, rc); /* commit data to the decoder */
                    MY_BDBG_WRN(("L%d bytesRemain = %d ( bytesremain=%d - size=%d) \n" , __LINE__ , (bytesRemain - size ) , bytesRemain, size ));
                    bytesRemain -= size;
                }
                else {
                    BDBG_MSG(("L%d NO MORE DATA to read in /read complete , wait for eSegmentDone\n" , __LINE__ ));
                }
           /*  } */
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

    blitSettings.output.surface = destSurface;

    blitSettings.output.rect.x      = fillSettings.rect.x;
    blitSettings.output.rect.y      = fillSettings.rect.y; 
    blitSettings.output.rect.width  = fillSettings.rect.width;
    blitSettings.output.rect.height = fillSettings.rect.height;

    NEXUS_Graphics2D_Blit(gfx, &blitSettings);

    /* must checkpoint before destroying 'picture' surface */
    checkpoint(gfx, checkpointEvent);
    rc = 0;
    if (picture) {
        NEXUS_Surface_Destroy(picture);
    }
    fclose(fin);
    return rc;

error:
    fclose(fin);
    /* always call stop, even if we only called GetBuffer. this releases the internal resource */
    BDBG_WRN(("ERROR: call pic_Stop after error/end of decode_picture decoder=%p" , pictureDecoder ));
    NEXUS_PictureDecoder_Stop(pictureDecoder);
    if (picture) {
        NEXUS_Surface_Destroy(picture);
    }
    return rc;
}


/* continually blit from a dedicated thread.
   each thread gets its own NEXUS_Graphics2D instance and event 
*/
void *blit_thread(void *context)
{
    NEXUS_Error rc;
    struct app_data *app_data = context;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle event;
    unsigned id = pthread_self();
    unsigned cnt = 0;
    int threadNr = app_data->threadNr; /* copy thread id, cos will change */

#ifndef BLIT_ONLY
    char picName[128];
    NEXUS_PictureDecoderOpenSettings decoderSettings;
    NEXUS_PictureDecoderHandle pictureDecoder;
#endif

    strcpy( picName , app_data->picFilename );
    rc = BKNI_CreateEvent(&event);
    BDBG_ASSERT(!rc);
    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);

    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = event;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

#ifndef BLIT_ONLY

    NEXUS_PictureDecoder_GetDefaultOpenSettings(&decoderSettings);
    decoderSettings.multiScanBufferSize = 4*1024*1024; /* intentionally small because blit_server only gives a small heap */
    decoderSettings.bufferSize = 128*1024;
    pictureDecoder = NEXUS_PictureDecoder_Open(0, &decoderSettings);
    BDBG_ASSERT(pictureDecoder);
    BKNI_Sleep(100);
    BDBG_WRN(("picture name is %s" , picName ));
#endif

    while ( app_data->start < threadNr ) {
        BKNI_Sleep( 10*(threadNr*5 /* make sure start in order !!*/ ) );
    }
    app_data->start++;

    BDBG_WRN(("Now starting decode on Picname %s id=%d" , picName, id ));

    while (!app_data->done) {
        NEXUS_Graphics2DFillSettings fillSettings;

        BDBG_WRN(("Picname is %s id=%d" , picName, id ));

        if (++cnt % 10000 == 0) BKNI_Printf("blit_thread(%d): %d\n", id, cnt);

        NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
        fillSettings.surface = app_data->surface;
#ifdef RANDOM
        fillSettings.rect.x = rand() % (app_data->createSettings.width-200);
        fillSettings.rect.y = rand() % (app_data->createSettings.height-200);
#else
        fillSettings.rect.x = ( (threadNr-1) % 16 ) * 80;
        fillSettings.rect.y = ( (threadNr-1) / 16 ) * 80;
#endif
        fillSettings.rect.width = 80;
        fillSettings.rect.height = 80;
        fillSettings.colorOp = NEXUS_FillOp_eCopy;
        fillSettings.alphaOp = NEXUS_FillOp_eCopy;

#ifdef BLIT_ONLY
        fillSettings.color = (rand()%0xFFFFFF) | ((rand()%0xFF)<<24);
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        if (rc) continue;
        checkpoint( gfx , event );
#else /* image decode */
        decode_picture(pictureDecoder, app_data->bufferSize, app_data->surface, picName, gfx, event , fillSettings , threadNr );
#endif

        if ( !app_data->done ) BKNI_Sleep(700);

        /* Now undraw it */
        fillSettings.colorOp = NEXUS_FillOp_eCopy;
        fillSettings.alphaOp = NEXUS_FillOp_eCopy;

        fillSettings.color = (0x000000) | ((0xFF)<<24);

        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        if (rc) continue;

        checkpoint( gfx , event );
        if ( !app_data->done ) BKNI_Sleep(160+(rand()%40));
    }

#ifndef BLIT_ONLY
    BDBG_WRN(("Attempting to close Thread id=%d (pic=%s) dec=%p" , id, picName , pictureDecoder ));
    NEXUS_PictureDecoder_Close(pictureDecoder);
    BDBG_WRN(("Closed Thread id=%d (pic=%s)\n" , id, picName ));
#endif

    NEXUS_Graphics2D_Close(gfx);
    BKNI_DestroyEvent(event);
    return NULL;
}
#endif  /* end of NEXUS_HAS_PICTURE_DECODER */

#define TEST_NR_DECODES 
/* #define TEST_GENERIC_JPEGS */

#ifdef TEST_NR_DECODES
/* 
   15 pool ball images numbered 1 to 15 downloaded from http://classroomclipart.com/clipart-search/page-36/numbers/
   used to test if x number of decodes can be completed by SID decoder
*/

char jpegSamples[][64] = { "01_TN_one.jpg"     , "02_TN_two.jpg"      , "03_TN_three.jpg"  , "04_TN_four.jpg",
                           "05_TN_five.jpg"    , "06_TN_six.jpg"      , "07_TN_seven.jpg"  , "08_TN_eight.jpg" , 
                           "09_TN_nine.jpg"    , "10_TN_ten.jpg"      , "11_TN_eleven.jpg" , "12_TN_twelve.jpg",
                           "13_TN_thirteen.jpg", "14_TN_fourteen.jpg" , "15_TN_fifteen.jpg",
                           "TN_number_1e.jpg"  , "TN_number_2e.jpg"   , "TN_number_3e.jpg" , "TN_number_4e.jpg",
                           "TN_number_5e.jpg"  , "TN_number_6e.jpg"   , "TN_number_7e.jpg" , "TN_number_8e.jpg"
                        };
#endif

#ifdef TEST_GENERIC_JPEGS
char jpegSamples[][64] = { "AA1033.jpg"     , "AA1100.jpg"      , "AA1053.jpg"  , "AA1164.jpg",
                           "wood_andi.jpg"  , "House.jpg"       , "Sunset.jpg"  , "bg.jpg"   ,
                           "Sunset.jpg"
                         };

#endif

int main(int argc, const char *argv[])
{
#if NEXUS_HAS_PICTURE_DECODER /* NEXUS_HAS_GRAPHICS2D */
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Graphics2DHandle gfx;
    int i;
    int nrSidCh = 4;  /* default to 4 SID channels */

#define MAX_THREADS 128 
    pthread_t thread[MAX_THREADS];
    struct app_data app_data;
    NEXUS_Error rc;


    if(argc == 2) { 
        /* user overiding nr of default SID channels */
        nrSidCh = atoi(argv[1]);
        if ( nrSidCh > MAX_THREADS ) nrSidCh = MAX_THREADS;
    }

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_Memset(&app_data, 0, sizeof(app_data));

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_e720p /* NEXUS_VideoFormat_eNtsc */; 
    display = NEXUS_Display_Open(0, &displaySettings);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
#endif

    NEXUS_Surface_GetDefaultCreateSettings(&app_data.createSettings);
    app_data.createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    app_data.createSettings.width = 1280 /*720 (NTSC) */;
    app_data.createSettings.height = 720 /*480 (NTSC) */;
    app_data.createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    app_data.surface = NEXUS_Surface_Create(&app_data.createSettings);

    /* fill with black. this instance of the blitter is not used in the threads. */
    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = app_data.surface;
    fillSettings.color = 0;

    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
    BKNI_Sleep(70);

    NEXUS_Graphics2D_Close(gfx);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, app_data.surface);
    BDBG_ASSERT(!rc);

    app_data.bufferSize = 128*1024;

    /* fill random boxes from multiple threads*/
    for (i=0;i<nrSidCh;i++) {
        app_data.threadNr = i+1;
#ifdef TEST_GENERIC_JPEGS
            strcpy( app_data.picFilename, jpegSamples[i%9] );
#endif

#ifdef TEST_NR_DECODES
            strcpy( app_data.picFilename, "numberJpegs/" );
            strcat( app_data.picFilename, jpegSamples[i%23] );
#endif
        rc = pthread_create(&thread[i], NULL, blit_thread, &app_data);
        BDBG_ASSERT(!rc);
        BKNI_Sleep(100+( (app_data.threadNr-1) *2)); /* give threads enough time to open sid channels etc */
    }
    BKNI_Sleep(200);     
    fprintf( stderr, "Now press a key to start ALL decodes\n" );
    getchar();
    app_data.start = true;

    printf("press ENTER to quit\n");
    getchar();

    /* take down all the threads */
    app_data.done = true;
    for (i=0;i<nrSidCh;i++) {
        BDBG_WRN(("joining thread %d out of %d to CLOSE IT" , i , nrSidCh )); 
        pthread_join(thread[i], NULL);
    }

    /* take down the system */
    NEXUS_Surface_Destroy(app_data.surface);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();
#endif

    return 0;
}
