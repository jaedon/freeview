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
 * $brcm_Workfile: image_input.c $
 * $brcm_Revision: 17 $
 * $brcm_Date: 9/11/12 3:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/graphics/image_input.c $
 * 
 * 17   9/11/12 3:01p jtna
 * SW7425-2172: if surface in use, wait for recycled surface
 * 
 * 16   2/24/12 12:02p erickson
 * SW7425-1511: merge
 * 
 * 15   1/25/12 5:22p rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 14   12/21/11 4:05p erickson
 * SWDEPRECATED-2425: write reusable function to determine heap for MFD
 *  access
 * 
 * 13   2/18/11 4:09p erickson
 * SW7422-189: use mfdMapping[] to learn the heap for MFD0, which is not
 *  necessarily AVD0
 *
 * 12   2/17/11 2:44p erickson
 * SW7420-1380: update for NEXUS_VideoImageInput_SetNextSurface
 *
 * 11   1/3/11 2:13p erickson
 * SW7420-1327: add non-UMA support
 *
 * 10   11/29/10 2:57p erickson
 * SW7420-1078: add NEXUS_Platform_GetFramebufferHeap
 *
 * 9   8/6/10 1:11p erickson
 * SW7420-703: remove unnecessary BCHP_CHIP and SCART logic
 *
 * 8   9/21/09 2:00p mphillip
 * SW7420-337: Fix for 7420's heaps
 *
 * 7   1/16/09 10:27a erickson
 * PR39986: fix shutdown
 *
 * 6   1/16/09 10:24a erickson
 * PR39986: add panel support, add comments
 *
 * 5   7/9/08 5:22p vsilyaev
 * PR 40921: Don't used component output if it's undefined
 *
 * 4   6/19/08 2:33p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 *
 * 3   4/15/08 4:31p vsilyaev
 * PR 39986: Fixed display initialization for 3563
 *
 * 2   3/21/08 12:50p vsilyaev
 * PR 39986: Fixed teardown sequence
 *
 * 1   3/21/08 12:09p vsilyaev
 * PR 39986: Added VideoImageInput
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
#include "nexus_graphics2d.h"
#include "nexus_video_output.h"
#include "nexus_video_window.h"
#include "nexus_video_image_input.h"
#include "nexus_video_input.h"

#include <stdio.h>

BDBG_MODULE(image_input);

#define NUM_SURFACES 3  /* triple buffering for image update every FRAME and a full queue */

struct {
    NEXUS_SurfaceHandle handle;
    bool submitted;
} g_surface[NUM_SURFACES];

static void
simple_fill(const NEXUS_SurfaceMemory *mem, const NEXUS_Rect *rect, NEXUS_Pixel pixel)
{
    unsigned x,y;
    unsigned width = rect->width;
    for(y=0;y<rect->height;y++) {
        uint32_t *buf = (uint32_t *)((uint8_t *)mem->buffer + mem->pitch*(y+rect->y))+rect->x;
        for(x=0;x<width;x++) {
            buf[x] = pixel;
        }
    }
}

static void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void imageBufferCallback( void *context, int param )
{
    BSTD_UNUSED(param);
    BKNI_SetEvent((BKNI_EventHandle)context);
}

static NEXUS_HeapHandle b_get_image_input_heap(unsigned imageInputIndex)
{
    NEXUS_PlatformSettings platformSettings;
    unsigned videoDecoderIndex, heapIndex;
    NEXUS_PlatformConfiguration platformConfig;
    
    /* The imageInput index is the same as the MPEG feeder (MFD) index. we need to find a heap that is RTS-accessible by this MFD. 
    This can be learned from the VideoDecoderModule because the MFD normally reads from AVD picture buffers. */
    
    /* first, get platform settings, which gives us VideoDecoderModuleSettings */
    NEXUS_Platform_GetSettings(&platformSettings);
    
    /* next, find the VideoDecoder index that uses this MFD */
    for (videoDecoderIndex=0;videoDecoderIndex<NEXUS_NUM_VIDEO_DECODERS;videoDecoderIndex++) {
        if (platformSettings.videoDecoderModuleSettings.mfdMapping[videoDecoderIndex] == imageInputIndex) {
            /* we've found a video decoder that uses this MFD */
            break;
        }
    }
    if (videoDecoderIndex == NEXUS_NUM_VIDEO_DECODERS) {
        /* this MFD is unused by VideoDecoder, so we can't know the heap */
        return NULL;
    }
    
    /* now, get the heap index for this video decoder. the MFD must be able to read from this heap 
    if it's able to read AVD decoded pictures placed into this heap. */
    heapIndex = platformSettings.videoDecoderModuleSettings.avdHeapIndex[videoDecoderIndex];
    
    /* then return the heap */
    NEXUS_Platform_GetConfiguration(&platformConfig);
    return platformConfig.heap[heapIndex];
}

int main(void)
{
    NEXUS_Error rc;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoWindowSettings windowCfg;
    NEXUS_GraphicsSettings graphicsCfg;
    NEXUS_SurfaceHandle frameBuffer;
    NEXUS_SurfaceMemory mem;
    NEXUS_SurfaceCreateSettings surfaceCfg;
    NEXUS_Rect fillRect;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_VideoImageInputHandle imageInput;
    NEXUS_VideoInput videoInput;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle event,imageEvent;
    NEXUS_VideoImageInputSettings  imageInputSetting;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
#endif
    unsigned i;
    bool moveWindow=false;
    unsigned submitIdx,releaseIdx;
    unsigned num_entries;


    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format      = NEXUS_VideoFormat_eNtsc; /* change to xx_e720p to observe faster draw rate */
    display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
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

    window = NEXUS_VideoWindow_Open(display, 0);
    BDBG_ASSERT(window);

    NEXUS_VideoWindow_GetSettings(window, &windowCfg);
    if ( !moveWindow ) {
        windowCfg.position.x = windowCfg.position.width/4;
        windowCfg.position.y = windowCfg.position.height/4;
    }
    windowCfg.position.width/=2;
    windowCfg.position.height/=2;
    windowCfg.visible = true;
    rc = NEXUS_VideoWindow_SetSettings(window, &windowCfg);
    BDBG_ASSERT(!rc);

    BKNI_CreateEvent(&event);
    gfx = NEXUS_Graphics2D_Open(0, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = event;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);

    /* Create a surface that will be used as a graphics framebuffer.
    This is used to show compositing the graphics (img[]) being fed through the video window
    with graphics (framebuffer) being fed through the graphics feeder. */
    NEXUS_Display_GetGraphicsSettings(display, &graphicsCfg);
    NEXUS_Surface_GetDefaultCreateSettings(&surfaceCfg);
    surfaceCfg.width  = graphicsCfg.clip.width;
    surfaceCfg.height = graphicsCfg.clip.height;
    surfaceCfg.heap   = NEXUS_Platform_GetFramebufferHeap(0);
    frameBuffer = NEXUS_Surface_Create(&surfaceCfg);

    if( !frameBuffer ) {
        fprintf(stderr, "Can't create framebuffer\n");
        return -1;
    }

    /* Create surfaces that will be used with VideoImageInput */
    NEXUS_Surface_GetDefaultCreateSettings(&surfaceCfg);
    surfaceCfg.width  = 720;
    surfaceCfg.height = 480;
    surfaceCfg.pixelFormat = NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08;
    surfaceCfg.heap = b_get_image_input_heap(0);

    for( i=0; i < NUM_SURFACES ; i++ ) {
        g_surface[i].handle = NEXUS_Surface_Create(&surfaceCfg);
        if ( !g_surface[i].handle ) {
            fprintf(stderr, "Can't create g_surface[%d].handle \nTerminate program\n",i);
            return -1;
        }
        g_surface[i].submitted = false;
    }

    imageInput = NEXUS_VideoImageInput_Open(0, NULL);
    BDBG_ASSERT(imageInput);

    videoInput = NEXUS_VideoImageInput_GetConnector(imageInput);
    BDBG_ASSERT(videoInput);

    /* Draw a simple pattern as background in the graphics plane */
    NEXUS_Surface_GetMemory(frameBuffer, &mem);
    fillRect = graphicsCfg.clip;
    simple_fill(&mem, &fillRect, 0);
    fillRect.height /= 2;
    simple_fill(&mem, &fillRect, 0x1F00FF00);  /* Draw top half of screen in green */
    fillRect = graphicsCfg.clip;
    fillRect.y = fillRect.height/2;
    fillRect.height /= 2;
    fillRect.width  /= 2;
    simple_fill(&mem, &fillRect, 0x7F800000);  /* Draw bottom left quadrant in red */
    fillRect = graphicsCfg.clip;
    fillRect.y = fillRect.height/2-30;
    fillRect.x = fillRect.width/2-40;
    fillRect.height = 60;
    fillRect.width  = 80;
    simple_fill(&mem, &fillRect, 0xFF000000);  /* opaque black hole in the middle */

    /* memset(mem.buffer, 0xFF, mem.pitch*graphicsCfg.displayRect.height); */
    NEXUS_Surface_Flush(frameBuffer);
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, frameBuffer);

    NEXUS_Surface_GetCreateSettings( g_surface[0].handle, &surfaceCfg);
    fillRect.width  = surfaceCfg.width/2;
    fillRect.height = surfaceCfg.height;
    fillRect.x = 0;
    fillRect.y = 0;

    rc = NEXUS_VideoWindow_AddInput(window, videoInput);
    BDBG_ASSERT(!rc);

    BKNI_CreateEvent(&imageEvent);
    NEXUS_VideoImageInput_GetSettings( imageInput, &imageInputSetting );
    imageInputSetting.imageCallback.callback = imageBufferCallback;
    imageInputSetting.imageCallback.context  = imageEvent;

    NEXUS_VideoImageInput_SetSettings( imageInput, &imageInputSetting );

    submitIdx=0;
    releaseIdx=0;

    for(i=0;i<1000;i++) {
        NEXUS_Graphics2DFillSettings fillSettings;
        NEXUS_SurfaceHandle freeSurface=NULL;
        NEXUS_VideoImageInputSurfaceSettings surfaceSettings;
        NEXUS_Pixel luma=(i*8)&0xFF;
        NEXUS_SurfaceHandle pic;

        num_entries=0;
        /* Make sure image surface is not in use by Video Output (VDC) */
        do {
            if ( g_surface[submitIdx].submitted ) {
                /* our queue is all used up, need to wait until VideoImageInput returns */
                /* a surface after it has been displayed                                */
                BDBG_MSG(("g_surface[submitIdx=%d].submitted in use, wait for recycle" , submitIdx ));
                BKNI_WaitForEvent( imageEvent, BKNI_INFINITE);
            }

            rc=NEXUS_VideoImageInput_RecycleSurface( imageInput, &freeSurface , 1, &num_entries );
            BDBG_ASSERT(!rc);
            if ( num_entries ) {
                /* our surface has been displayed, we can now re-use and re-queue it */
                BDBG_MSG(("g_surface[releaseIdx=%d].handle=%p  recycSurface=%p" , releaseIdx, g_surface[releaseIdx].handle , freeSurface ));
                BDBG_ASSERT( g_surface[releaseIdx].handle == freeSurface );
                g_surface[releaseIdx].submitted = false;
                if ( ++releaseIdx == NUM_SURFACES ) releaseIdx=0;
            }

        } while ( num_entries || g_surface[submitIdx].submitted );

        g_surface[submitIdx].submitted = true; /* mark as inuse */
        pic = g_surface[submitIdx].handle;
        BDBG_MSG(("pic=%p" , pic ));
        if ( ++submitIdx == NUM_SURFACES ) submitIdx=0;

        /* must do M2MC fill. CPU may not have access to this surface on some non-UMA systems. */
        NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
        fillSettings.surface = pic;
        fillSettings.color = 0xFF008080 | (luma<<16); /* Draw box in gray in AYCrCb, with increasing luma  */
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
        if (rc == NEXUS_GRAPHICS2D_QUEUED) {
            BKNI_WaitForEvent(event, 0xffffffff);
        }

        NEXUS_VideoImageInput_GetDefaultSurfaceSettings( &surfaceSettings );
        /* Submit surface to VideoImageInput, wait if queue to feed VDC is already full */
        do {
            rc = NEXUS_VideoImageInput_PushSurface(imageInput, pic , &surfaceSettings );
            if(rc==NEXUS_IMAGEINPUT_QUEUE_FULL) {
                BKNI_WaitForEvent(imageEvent, BKNI_INFINITE);
            }
        } while ( rc );

        if ( moveWindow ) {
            /* Moving the window slows system throughput, set moveWindow to false to see higher throughput */
            NEXUS_VideoWindow_GetSettings(window, &windowCfg);
            windowCfg.position.x=(i*8)%windowCfg.position.width;
            windowCfg.position.y=(i*4)%windowCfg.position.height;
            rc = NEXUS_VideoWindow_SetSettings(window, &windowCfg);
            BDBG_ASSERT(!rc);
        }
    }

    fprintf(stderr, "done. press enter to exit.\n");
    getchar();

    /* All work is done, but we must free any image displayed by passing NULL in to flush the pipeline */
    NEXUS_VideoImageInput_SetSurface(imageInput, NULL);
    NEXUS_VideoWindow_RemoveAllInputs(window);
    NEXUS_VideoInput_Shutdown(videoInput);
    NEXUS_VideoImageInput_Close(imageInput);

    BKNI_DestroyEvent(imageEvent);
    BKNI_DestroyEvent(event);

    if ( gfx ) NEXUS_Graphics2D_Close( gfx );
    NEXUS_VideoWindow_Close(window);
    NEXUS_Display_Close(display);
    for( i=NUM_SURFACES; i > 0 ; i-- ) {
        NEXUS_Surface_Destroy(g_surface[i-1].handle);
    }
    NEXUS_Surface_Destroy(frameBuffer);
    NEXUS_Platform_Uninit();

    return 0;
}

