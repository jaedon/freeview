/******************************************************************************
 *    (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: smooth_pig.c $
 * $brcm_Revision: 20 $
 * $brcm_Date: 11/27/12 1:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/smooth_pig.c $
 * 
 * 20   11/27/12 1:40p erickson
 * SW7435-421: add shutdown code
 * 
 * 19   8/16/12 2:23p erickson
 * SW7425-2734: back out test code
 * 
 * 18   8/10/12 10:25a erickson
 * SW7425-2734: rework examples to use NEXUS_ClientResources for decoder
 *  ids
 * 
 * 17   8/6/12 1:29p erickson
 * SW7425-3565: add -full_screen option
 * 
 * 16   8/1/12 4:00p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 15   7/9/12 1:57p erickson
 * SW7231-844: allow server to pick default NEXUS_MemoryType_eFull heap
 * 
 * 14   4/20/12 10:08a erickson
 * SW7425-2882: create fully mapped heap and notify using app ipc
 * 
 * 13   12/21/11 3:48p erickson
 * SW7420-1148: fix UpdateSurface rectangle calculation
 * 
 * 12   12/19/11 10:09a erickson
 * SW7425-1795: use eFull heap for graphics2d packet fifo or playpump fifo
 * 
 * 11   11/29/11 11:41a erickson
 * SW7420-2129: use NULL for default settings now that default heap can be
 *  obtained via NEXUS_P_DefaultHeap
 * 
 * 10   11/11/11 11:28a erickson
 * SW7420-1135: add regular decoder and mosaic support
 * 
 * 9   10/26/11 3:54p erickson
 * SW7420-1992: improve DBG
 * 
 * 8   10/18/11 12:32p vsilyaev
 * SW7420-1992: Moved virtual display into the composition settings
 * 
 * 7   10/13/11 2:15p vsilyaev
 * SW7425-1364: Added support for simple decoder
 * 
 * 6   10/11/11 8:26a erickson
 * SW7420-1992: use regular-decoder start settings, client allocates stc
 *  channel, add disable mode enum
 * 
 * 5   10/4/11 4:33p erickson
 * SW7420-1148: add -move off option
 * 
 * 4   10/3/11 9:26a erickson
 * SW7420-1148: rewrite smooth_pig as surfacecmp and simple_decoder client
 * 
 * 3   9/7/11 12:55p erickson
 * SW7420-1009: use NEXUS_ANY_ID for NEXUS_Graphics2D_Open
 * 
 * 2   1/4/11 10:25a erickson
 * SW7420-1227: add double and triple buffering option, test w/ and w/o
 *  video
 *
 * 1   12/2/10 3:55p erickson
 * SW7420-1227: add smooth_pig example
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_surface_client.h"
#include "nexus_simple_video_decoder.h"
#include "nexus_graphics2d.h"
#include "nexus_playback.h"
#include "nexus_timebase.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "bstd.h"
#include "bkni.h"

BDBG_MODULE(smooth_pig);

#define SESSION_NEED_DECODER    1

#include "refsw_session_simple_client.h"

struct decode_context {
    NEXUS_SimpleVideoDecoderHandle videoDecoder;
    bool mosaic;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaybackHandle playback;
    NEXUS_FilePlayHandle file;
    NEXUS_StcChannelHandle stcChannel;
};

static int start_decode(struct decode_context *context);
static void stop_decode(struct decode_context *context);

static void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void checkpoint(NEXUS_Graphics2DHandle gfx, BKNI_EventHandle event)
{
    int rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
    if (rc == NEXUS_GRAPHICS2D_QUEUED) {
        BKNI_WaitForEvent(event, 0xffffffff);
    }
}


#undef min
#define min(A,B) ((A)<(B)?(A):(B))
#undef max
#define max(A,B) ((A)>(B)?(A):(B))

static void
b_union_rect(NEXUS_Rect *dest, const NEXUS_Rect *s1, const NEXUS_Rect *s2)
{
    if (s1->width == 0 || s1->height == 0) {
        *dest = *s2;
    }
    else if (s2->width == 0 || s2->height == 0) {
        *dest = *s1;
    }
    else {
        int r,b,x,y;
        x = min(s1->x,s2->x);
        y = min(s1->y,s2->y);
        r = max(s1->x + s1->width, s2->x + s2->width);
        b = max(s1->y + s1->height, s2->y + s2->height);
        dest->x = x;
        dest->y = y;
        dest->width = r - x;
        dest->height = b - y;
    }
}

void
b_intersect_rect(NEXUS_Rect *dest, const NEXUS_Rect *s1, const NEXUS_Rect *s2)
{
    int r,b,x,y;
    x = max(s1->x, s2->x);
    y = max(s1->y, s2->y);
    r = min(s1->x + (int)s1->width, s2->x + (int)s2->width);
    b = min(s1->y + (int)s1->height, s2->y + (int)s2->height);
    if (r >= x && b >= y) {
        dest->x = x;
        dest->y = y;
        dest->width = r - x;
        dest->height = b - y;
    }
    else {
        dest->x = 0;
        dest->y = 0;
        dest->width = 0;
        dest->height = 0;
    }
}

int main(int argc, const char * const argv[])
{
    NEXUS_ClientConfiguration clientConfig;
    NEXUS_Error rc;
    BKNI_EventHandle displayedEvent, checkpointEvent;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Rect graphicsRect = {0,0,720,480};
    NEXUS_Rect oldGraphicsRect = graphicsRect;
    int gfx_client_id = -1;
#define MAX_WINDOWS 8
    NEXUS_Rect windowRect[MAX_WINDOWS];
    NEXUS_SurfaceClientHandle blit_client, video_client[MAX_WINDOWS];
    struct decode_context decode_context[MAX_WINDOWS];
    NEXUS_SurfaceClientSettings client_settings;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_SurfaceHandle surface;
    NEXUS_ClientAuthenticationSettings authSettings;
    int curarg = 1;
    bool move = true;
    unsigned num_windows = 1;
    unsigned i;
    bool full_screen = false;

    while (curarg < argc) {
        if (!strcmp(argv[curarg], "-move") && curarg+1<argc) {
            curarg++;
            if (!strcmp(argv[curarg], "off")) {
                move = false;
            }
        }
        else if (!strcmp(argv[curarg], "-n") && curarg+1<argc) {
            curarg++;
            num_windows = atoi(argv[curarg]);
            if (num_windows > MAX_WINDOWS) {
                num_windows = MAX_WINDOWS;
            }
        }
        else if (!strcmp(argv[curarg], "-full_screen")) {
            full_screen = true;
        }
        else {
            break;
        }
        curarg++;
    }
    if (full_screen) {
        num_windows = 1;
        move = false;
    }
    rc = simple_client_init(argv[0], &authSettings);
    printf("using certificate:'%s'\n", (char *)authSettings.certificate.data);
    rc = NEXUS_Platform_AuthenticatedJoin(&authSettings);
    if (rc) return -1;
    
    BKNI_CreateEvent(&displayedEvent);
    BKNI_CreateEvent(&checkpointEvent);

    NEXUS_Platform_GetClientConfiguration(&clientConfig);
    if (gfx_client_id == -1 && clientConfig.resources.surfaceClient.total) {
        gfx_client_id = clientConfig.resources.surfaceClient.id[0];
    }
    blit_client = NEXUS_SurfaceClient_Acquire(gfx_client_id);
    if (!blit_client) {
        BDBG_ERR(("NEXUS_SurfaceClient_Acquire %d failed", gfx_client_id));
        return -1;
    }
    
    NEXUS_SurfaceClient_GetSettings(blit_client, &client_settings);
    client_settings.displayed.callback = complete;
    client_settings.displayed.context = displayedEvent;
    client_settings.composition.virtualDisplay.width = graphicsRect.width;
    client_settings.composition.virtualDisplay.height = graphicsRect.height;
    rc = NEXUS_SurfaceClient_SetSettings(blit_client, &client_settings);
    BDBG_ASSERT(!rc);
    
    if (num_windows > 1) {
        /* TODO: server must revert to non-mosaic when the app quits */
        brc_client_set_simple_decoder_mosaic_mode(simple_client_state.rca_client, true);
    }
    
    if (num_windows > clientConfig.resources.simpleVideoDecoder.total) {
        num_windows = clientConfig.resources.simpleVideoDecoder.total;
    }
    for (i=0;i<num_windows;i++) {
        decode_context[i].videoDecoder = NEXUS_SimpleVideoDecoder_Acquire(clientConfig.resources.simpleVideoDecoder.id[i]);
        if (!decode_context[i].videoDecoder) {
            BDBG_ERR(("NEXUS_SimpleVideoDecoder_Acquire %d failed", clientConfig.resources.simpleVideoDecoder.id[i]));
            return -1;
        }
    
        video_client[i] = NEXUS_SurfaceClient_AcquireVideoWindow(blit_client, i);
        if (!video_client[i]) {
            BDBG_ERR(("NEXUS_SurfaceClient_AcquireVideoWindow %d failed", i));
            return -1;
        }
        
        if (full_screen) {
            windowRect[i].x = 0;
            windowRect[i].y = 0;
            windowRect[i].width = client_settings.composition.virtualDisplay.width;
            windowRect[i].height = client_settings.composition.virtualDisplay.height;
        }
        else {
            windowRect[i].x = 100+i*10;
            windowRect[i].y = 100+i*10;
            windowRect[i].width = 200+i*10;
            windowRect[i].height = 200+i*10;
        }
        
        NEXUS_SurfaceClient_GetSettings(video_client[i], &client_settings);
        client_settings.composition.position = windowRect[i];
        client_settings.composition.visible = true;
        client_settings.composition.contentMode = NEXUS_VideoWindowContentMode_eBox;
        rc = NEXUS_SurfaceClient_SetSettings(video_client[i], &client_settings);
        BDBG_ASSERT(!rc);
    }

    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = graphicsRect.width;
    createSettings.height = graphicsRect.height;
    surface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.color = 0xFF0000FF;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.color = 0;
    for (i=0;i<num_windows;i++) {
        fillSettings.rect = windowRect[i];
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        BDBG_ASSERT(!rc);
    }
    checkpoint(gfx, checkpointEvent);
    rc = NEXUS_SurfaceClient_SetSurface(blit_client, surface);
    BDBG_ASSERT(!rc);
    
    BKNI_WaitForEvent(displayedEvent, BKNI_INFINITE);
    
    for (i=0;i<num_windows;i++) {
        decode_context[i].mosaic = num_windows>1;
        rc = start_decode(&decode_context[i]);
        if (rc) return -1;
    }

    if (!move) {
        getchar();
    }
    else    
    while (1) {
        NEXUS_Rect newGraphicsRect;
        NEXUS_Rect allWindows = {0,0,0,0};
        NEXUS_Rect updateRect = oldGraphicsRect;
        
        /* fill whole screen with blue */
        NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
        fillSettings.surface = surface;
        fillSettings.color = 0xFF0000FF;
        fillSettings.rect = oldGraphicsRect;
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        BDBG_ASSERT(!rc);
        
        for (i=0;i<num_windows;i++) {
            /* move the window(s) */
            windowRect[i].x += (i%3)-1;
            windowRect[i].y += (i%3)-1;
            if (windowRect[i].x >= (int)graphicsRect.width+20) {
                windowRect[i].x = -220;
            }
            else if (windowRect[i].x+windowRect[i].width < 0) {
                windowRect[i].x = graphicsRect.width;
            }
            if (windowRect[i].y >= (int)graphicsRect.height+20) {
                windowRect[i].y = -220;
            }
            else if (windowRect[i].y+windowRect[i].height < 0) {
                windowRect[i].y = graphicsRect.height;
            }
            
            /* because blitting is local, we must clip the blit here. */
            b_intersect_rect(&newGraphicsRect, &windowRect[i], &graphicsRect);
            
            if (newGraphicsRect.width && newGraphicsRect.height) {
                /* draw the new PIG hole */
                fillSettings.surface = surface;
                fillSettings.color = 0;
                fillSettings.rect = newGraphicsRect;
                rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
                BDBG_ASSERT(!rc);
            }
            /* increment the update region */
            b_union_rect(&updateRect, &updateRect, &newGraphicsRect);
        }
        
        checkpoint(gfx, checkpointEvent);
        
        for (i=0;i<num_windows;i++) {
            /* new video window settings won't be applied until next vsync, so set them first. 
            if there is some system delay between now and the framebuffer composition, you may see an
            artifact on the screen. but that's unavoidable. */
            NEXUS_SurfaceClient_GetSettings(video_client[i], &client_settings);
            client_settings.composition.position = windowRect[i];
            client_settings.composition.visible = true;
            rc = NEXUS_SurfaceClient_SetSettings(video_client[i], &client_settings);
            BDBG_ASSERT(!rc);
            BDBG_MSG(("window %d: %d,%d,%d,%d", i, windowRect[i].x, windowRect[i].y, windowRect[i].width, windowRect[i].height));
            
            b_union_rect(&allWindows, &allWindows, &windowRect[i]);
        }
        b_intersect_rect(&allWindows, &allWindows, &graphicsRect);

        BDBG_MSG(("update %d,%d,%d,%d", updateRect.x, updateRect.y, updateRect.width, updateRect.height));
        rc = NEXUS_SurfaceClient_UpdateSurface(blit_client, &updateRect);
        BDBG_ASSERT(!rc);
        
        BKNI_WaitForEvent(displayedEvent, BKNI_INFINITE);
        
        /* remember the old pig hole so we can minimize the blit */
        oldGraphicsRect = allWindows;
    }
    
    NEXUS_SurfaceClient_Release(blit_client);
    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Surface_Destroy(surface);
    for (i=0;i<num_windows;i++) {
        if (decode_context[i].videoDecoder) {
            stop_decode(&decode_context[i]);
            NEXUS_SimpleVideoDecoder_Release(decode_context[i].videoDecoder);
        }
    }
    BKNI_DestroyEvent(displayedEvent);
    BKNI_DestroyEvent(checkpointEvent);
    NEXUS_Platform_Uninit();
    simple_client_shutdown();

    return 0;
}

struct {
    const char *filename, *indexfilename;
    NEXUS_VideoCodec videoCodec;
    unsigned short videoPid;
    NEXUS_AudioCodec audioCodec;
    unsigned short audioPid;
} g_pvrfiles[] = {
    {"videos/cnnticker.mpg", "videos/cnnticker.nav", NEXUS_VideoCodec_eMpeg2, 0x21, NEXUS_AudioCodec_eMpeg, 0x22},
    {"videos/spider_cc.mpg", "videos/spider_cc.nav", NEXUS_VideoCodec_eMpeg2, 0x11, NEXUS_AudioCodec_eAc3,  0x14}
};

NEXUS_Timebase timebase = 0;
    
static int start_decode(struct decode_context *context)
{
    NEXUS_PlaypumpOpenSettings openPlaypumpSettings;
    NEXUS_PlaybackSettings playbackSettings;
    NEXUS_SimpleVideoDecoderStartSettings videoProgram;
    int rc;
    unsigned index = 0;
    NEXUS_PlaybackPidChannelSettings playbackPidSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_ClientConfiguration clientConfig;
    
    NEXUS_Platform_GetClientConfiguration(&clientConfig);
    
    NEXUS_Playpump_GetDefaultOpenSettings(&openPlaypumpSettings);
    context->playpump = NEXUS_Playpump_Open(NEXUS_ANY_ID, &openPlaypumpSettings);
    BDBG_ASSERT(context->playpump);
    context->playback = NEXUS_Playback_Create();
    BDBG_ASSERT(context->playback);

    context->file = NEXUS_FilePlay_OpenPosix(g_pvrfiles[index].filename, g_pvrfiles[index].indexfilename);
    if (!context->file) {
        BDBG_ERR(("can't open files: %s %s", g_pvrfiles[index].filename, g_pvrfiles[index].indexfilename));
        return -1;
    }
    
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.mode = NEXUS_StcChannelMode_eAuto; /* PVR */
    if (context->mosaic) {
        if (!timebase) {
            timebase = NEXUS_Timebase_Open(NEXUS_ANY_ID);
            if (!timebase) {
                return -1;
            }
        }
    
        stcSettings.timebase = timebase;
        stcSettings.autoConfigTimebase = false;
        stcSettings.stcIndex = 0;
    }
    context->stcChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
    BDBG_ASSERT(context->stcChannel);
    
    NEXUS_Playback_GetSettings(context->playback, &playbackSettings);
    playbackSettings.playpump = context->playpump;
    /* set a stream format, it could be any audio video transport type or file format, i.e NEXUS_TransportType_eMp4, NEXUS_TransportType_eAvi ... */
    playbackSettings.playpumpSettings.transportType = NEXUS_TransportType_eTs;
    playbackSettings.stcChannel = context->stcChannel;
    playbackSettings.stcTrick = true;
    rc = NEXUS_Playback_SetSettings(context->playback, &playbackSettings);
    BDBG_ASSERT(!rc);
    
    NEXUS_SimpleVideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.settings.codec = NEXUS_VideoCodec_eMpeg2;
    
    NEXUS_Playback_GetDefaultPidChannelSettings(&playbackPidSettings);
    playbackPidSettings.pidSettings.pidType = NEXUS_PidType_eVideo;
    playbackPidSettings.pidTypeSettings.video.codec = g_pvrfiles[index].videoCodec; /* must be told codec for correct handling */
    playbackPidSettings.pidTypeSettings.video.index = true;
    playbackPidSettings.pidTypeSettings.video.simpleDecoder = context->videoDecoder;
    videoProgram.settings.pidChannel = NEXUS_Playback_OpenPidChannel(context->playback, g_pvrfiles[index].videoPid, &playbackPidSettings);
    
    rc = NEXUS_Playback_Start(context->playback, context->file, NULL);
    BDBG_ASSERT(!rc);
        
    videoProgram.maxWidth = 720;
    videoProgram.maxHeight = 480;
    rc = NEXUS_SimpleVideoDecoder_Start(context->videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);
    
    return 0;
}

static void stop_decode(struct decode_context *context)
{
    NEXUS_Playback_Destroy(context->playback);
    NEXUS_StcChannel_Close(context->stcChannel);
    NEXUS_Playpump_Close(context->playpump);
    NEXUS_FilePlay_Close(context->file);
}
