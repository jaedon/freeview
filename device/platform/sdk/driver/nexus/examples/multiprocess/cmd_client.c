/***************************************************************************
 *     (c)2011-2012 Broadcom Corporation
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
 * $brcm_Workfile: cmd_client.c $
 * $brcm_Revision: 13 $
 * $brcm_Date: 11/27/12 1:40p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/multiprocess/cmd_client.c $
 * 
 * 13   11/27/12 1:40p erickson
 * SW7435-421: undo previous change. IPC comes up before nexus join and
 *  comes down after nexus uninit.
 * 
 * 12   11/20/12 12:26p erickson
 * SW7425-1792: fix warning
 * 
 * 11   11/20/12 10:17a erickson
 * SW7425-1792: fix platforms without hdmi_input
 * 
 * 10   10/16/12 5:21p erickson
 * SW7425-1792: add client hdmi input, protect with
 *  NEXUS_Platform_AcquireObject
 * 
 * 9   8/1/12 4:00p erickson
 * SW7425-2734: allow server to limit resources for untrusted clients
 * 
 * 8   4/20/12 10:08a erickson
 * SW7425-2882: create fully mapped heap and notify using app ipc
 * 
 * 7   12/7/11 10:42a erickson
 * SW7420-2141: remove unnecessary NEXUS_VideoInput_Shutdown
 * 
 * 6   11/29/11 11:40a erickson
 * SW7420-1148: fix warning
 * 
 * 5   11/22/11 1:40p vsilyaev
 * SW7420-2148: Use proper arguments for NEXUS_SurfaceClient_UpdateSurface
 * 
 * 4   11/11/11 11:28a erickson
 * SW7420-1135: add regular decoder and mosaic support
 * 
 * 3   11/10/11 2:18p erickson
 * SW7420-1135: add regular decoder support to cmd_client and refsw_server
 *
 * 2   10/27/11 2:37p erickson
 * SW7425-1364: rename to brc
 *
 * 1   10/26/11 3:56p erickson
 * SW7425-1364: extend app ipc
 *
 **************************************************************************/

#include "nexus_platform_client.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_memory.h"
#include "nexus_base_mmap.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_parser_band.h"
#include "nexus_platform.h"
#include "nexus_video_input.h"
#if NEXUS_HAS_SURFACE_COMPOSITOR
#include "nexus_surface_client.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "refsw_session_simple_client.h"

BDBG_MODULE(blit_client);

static void draw_surface(NEXUS_SurfaceHandle surface, NEXUS_Graphics2DHandle gfx, BKNI_EventHandle packetSpaceAvailableEvent);

#if NEXUS_HAS_SURFACE_COMPOSITOR
static void complete(void *data, int unused)
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

static int b_start_live_decode(void);
static void b_stop_live_decode(void);
static int b_set_hdmi_input(bool enabled);

int main(int argc, const char * const argv[])
{
    NEXUS_ClientConfiguration clientConfig;
    NEXUS_SurfaceHandle surface;
    NEXUS_SurfaceMemory mem;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle checkpointEvent, packetSpaceAvailableEvent, displayedEvent;
    int i;
    NEXUS_Error rc;
    NEXUS_ClientAuthenticationSettings authSettings;
    NEXUS_SurfaceClientHandle blit_client;
    NEXUS_SurfaceClientSettings client_settings;
    NEXUS_Graphics2DBlitSettings blitSettings;
    int gfx_client_id = -1;

    BSTD_UNUSED(argc);
    
    rc = simple_client_init(argv[0], &authSettings);
    printf("using certificate:'%s'\n", (char *)authSettings.certificate.data);
    rc = NEXUS_Platform_AuthenticatedJoin(&authSettings);
    if (rc) {
        printf("cannot join: %d\n", rc);
        return -1;
    }

    NEXUS_Platform_GetClientConfiguration(&clientConfig);
    /* print client heaps */
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        NEXUS_MemoryStatus status;
        NEXUS_HeapHandle heap;

        heap = clientConfig.heap[i];
        if (!heap) break;
        rc = NEXUS_Heap_GetStatus(heap, &status);
        BDBG_ASSERT(!rc);
        printf("client heap[%d]: MEMC %d, offset %#x, addr %p, size %d (%#x), alignment %d\n",
            i, status.memcIndex, status.offset, status.addr, status.size, status.size, status.alignment);
    }

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 720;
    createSettings.height = 480;
    /* createSettings.heap is NULL. proxy will populate. */
    surface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(surface, &mem);

    BKNI_CreateEvent(&checkpointEvent);
    BKNI_CreateEvent(&packetSpaceAvailableEvent);
    BKNI_CreateEvent(&displayedEvent);

    gfx = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    gfxSettings.packetSpaceAvailable.callback = complete;
    gfxSettings.packetSpaceAvailable.context = packetSpaceAvailableEvent;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);

    draw_surface(surface, gfx, packetSpaceAvailableEvent);
    checkpoint(gfx, checkpointEvent);

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
    rc = NEXUS_SurfaceClient_SetSettings(blit_client, &client_settings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_SurfaceClient_SetSurface(blit_client, surface);
    BDBG_ASSERT(!rc);
    rc = BKNI_WaitForEvent(displayedEvent, 5000);
    BDBG_ASSERT(!rc);

    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);

    /* blit from left-hand gradient into the rest of the framebuffer */
    BDBG_WRN(("start"));
    for(i=0;;) {
        char buf[256];
        printf("brc_test_client>");
        fflush(stdout);
        fgets(buf, 256, stdin);

        if(feof(stdin)) {
            break;
        }

        buf[strlen(buf)-1] = 0;

        if (!strcmp(buf, "?")) {
            printf(
            "? - this help screen\n"
            "q - quit\n"
            "480i,1080i,720p,480p - change display format\n"
            "request_decode - request regular decoder from server and start live decode\n"
            "return_decode -  stop regular decoder and return to server\n"
            "ENTER - redraw\n"
            );
        }
        else if (!buf[0]) {
            draw_surface(surface, gfx, packetSpaceAvailableEvent);
            checkpoint(gfx, checkpointEvent);
            rc = NEXUS_SurfaceClient_UpdateSurface(blit_client, NULL);
            BDBG_ASSERT(!rc);
            rc = BKNI_WaitForEvent(displayedEvent, 5000);
            BDBG_ASSERT(!rc);
        }
        else if (!strcmp(buf, "q")) {
            break;
        }
        else if (!strcmp(buf, "1080i")) {
            brc_client_set_display_format(simple_client_state.rca_client, NEXUS_VideoFormat_e1080i);
        }
        else if (!strcmp(buf, "480i")) {
            brc_client_set_display_format(simple_client_state.rca_client, NEXUS_VideoFormat_eNtsc);
        }
        else if (!strcmp(buf, "720p")) {
            brc_client_set_display_format(simple_client_state.rca_client, NEXUS_VideoFormat_e720p);
        }
        else if (!strcmp(buf, "480p")) {
            brc_client_set_display_format(simple_client_state.rca_client, NEXUS_VideoFormat_e480p);
        }
        else if (!strcmp(buf, "request_decode")) {
            brc_client_request_video_decoder(simple_client_state.rca_client, 0);
            b_start_live_decode();
        }
        else if (!strcmp(buf, "return_decode")) {
            b_stop_live_decode();
            brc_client_return_video_decoder(simple_client_state.rca_client, 0);
        }
        else if (!strcmp(buf, "hdmi(on)")) {
            b_set_hdmi_input(true);
        }
        else if (!strcmp(buf, "hdmi(off)")) {
            b_set_hdmi_input(false);
        }
        else if (strstr(buf, "mosaic(") == buf) {
            brc_client_set_simple_decoder_mosaic_mode(simple_client_state.rca_client, atoi(buf+7));
        }
        else if (strstr(buf, "move(") == buf) {
            int x, y, width, height, n;
            n = sscanf(buf+5, "%d,%d,%d,%d", &x, &y, &width, &height);
            if (n == 4) {
                NEXUS_SurfaceComposition composition;
                brc_client_get_composition(simple_client_state.rca_client, &composition);
                composition.position.x = x;
                composition.position.y = y;
                composition.position.width = width;
                composition.position.height = height;
                brc_client_set_composition(simple_client_state.rca_client, &composition);
            }
        }
        else {
            printf("unknown command '%s'\n", buf);
        }
    }

    NEXUS_SurfaceClient_Release(blit_client);
    BKNI_DestroyEvent(displayedEvent);
    BKNI_DestroyEvent(checkpointEvent);
    BKNI_DestroyEvent(packetSpaceAvailableEvent);
    NEXUS_Surface_Destroy(surface);
    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Platform_Uninit();
    simple_client_shutdown();
    return 0;
}
#else
int main(void)
{
    printf("ERROR: NEXUS_SurfaceCompositor  not supported\n");
    return -1;
}
#endif

static void draw_surface(NEXUS_SurfaceHandle surface, NEXUS_Graphics2DHandle gfx, BKNI_EventHandle packetSpaceAvailableEvent)
{
    NEXUS_SurfaceCreateSettings createSettings;
    int rc;
    NEXUS_Graphics2DFillSettings fillSettings;
    unsigned i;

    NEXUS_Surface_GetCreateSettings(surface, &createSettings);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = surface;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    for (i=0;i<(int)createSettings.height;i+=2) {
        fillSettings.rect.width = rand()%(createSettings.width-1);
        fillSettings.rect.height = rand()%(createSettings.height-1);
        fillSettings.rect.x = rand() % (createSettings.width - fillSettings.rect.width);
        fillSettings.rect.y = rand() % (createSettings.height - fillSettings.rect.height);
        fillSettings.color = (0xFF << 24) | rand();
        do {
        rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
        if (rc) {
                /* if blit fails, must checkpoint and resubmit */
                BKNI_WaitForEvent(packetSpaceAvailableEvent, BKNI_INFINITE);
            }
        } while (rc);
    }
}

static struct b_live_decode
{
    NEXUS_PidChannelHandle videoPidChannel, audioPidChannel;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_ParserBand parserBand;
} g_live_decode;

static int b_start_live_decode(void)
{
    NEXUS_InputBand inputBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_StcChannelSettings stcSettings;
    int rc;

    if (g_live_decode.videoDecoder) {
        BDBG_ERR(("live decode already started"));
        return -1;
    }

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    g_live_decode.parserBand = NEXUS_ParserBand_Open(NEXUS_ANY_ID);
    if (!g_live_decode.parserBand) {
        BDBG_ERR(("unable to open parser band"));
        goto error;
    }

    NEXUS_ParserBand_GetSettings(g_live_decode.parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    rc = NEXUS_ParserBand_SetSettings(g_live_decode.parserBand, &parserBandSettings);
    BDBG_ASSERT(!rc);

    g_live_decode.videoPidChannel = NEXUS_PidChannel_Open(g_live_decode.parserBand, 0x11, NULL);

    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.mode = NEXUS_StcChannelMode_ePcr;
    stcSettings.modeSettings.pcr.pidChannel = g_live_decode.videoPidChannel;
    g_live_decode.stcChannel = NEXUS_StcChannel_Open(NEXUS_ANY_ID, &stcSettings);
    if (!g_live_decode.stcChannel) {
        BDBG_ERR(("unable to open stc channel"));
        goto error;
    }

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = g_live_decode.videoPidChannel;
    videoProgram.stcChannel = g_live_decode.stcChannel;

    g_live_decode.videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    if (!g_live_decode.videoDecoder) {
        BDBG_ERR(("unable to open live decoder"));
        goto error;
    }
    
    rc = brc_client_add_video_decoder_input(simple_client_state.rca_client, 0, g_live_decode.videoDecoder);
    if (rc) {
        BDBG_ERR(("unable to connect to window"));
        goto error;
    }
    
    rc = NEXUS_VideoDecoder_Start(g_live_decode.videoDecoder, &videoProgram);
    if (rc) {
        BDBG_ERR(("unable to start decode"));
        goto error;
    }
    
    return 0;
    
error:
    b_stop_live_decode();
    return -1;
}

static void b_stop_live_decode(void)
{
    if (g_live_decode.videoDecoder) {
        NEXUS_VideoDecoder_Stop(g_live_decode.videoDecoder);
        brc_client_remove_window_input(simple_client_state.rca_client, 0);
        NEXUS_VideoDecoder_Close(g_live_decode.videoDecoder);
        g_live_decode.videoDecoder = NULL;
    }
    if (g_live_decode.stcChannel) {
        NEXUS_StcChannel_Close(g_live_decode.stcChannel);
        g_live_decode.stcChannel = NULL;
    }
    if (g_live_decode.videoPidChannel) {
        NEXUS_PidChannel_Close(g_live_decode.videoPidChannel);
        g_live_decode.videoPidChannel = NULL;
    }
    if (g_live_decode.parserBand) {
        NEXUS_ParserBand_Close(g_live_decode.parserBand);
        g_live_decode.parserBand = 0;
    }
}

#if NEXUS_HAS_HDMI_INPUT
#include "nexus_hdmi_input.h"

static uint8_t SampleEDID[] = 
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x08, 0x6D, 0x74, 0x22, 0x05, 0x01, 0x11, 0x20,
    0x00, 0x14, 0x01, 0x03, 0x80, 0x00, 0x00, 0x78, 0x0A, 0xDA, 0xFF, 0xA3, 0x58, 0x4A, 0xA2, 0x29,
    0x17, 0x49, 0x4B, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
    0x45, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0x18, 0x71, 0x1C, 0x16, 0x20,
    0x58, 0x2C, 0x25, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x42,
    0x43, 0x4D, 0x37, 0x34, 0x32, 0x32, 0x2F, 0x37, 0x34, 0x32, 0x35, 0x0A, 0x00, 0x00, 0x00, 0xFD,
    0x00, 0x17, 0x3D, 0x0F, 0x44, 0x0F, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0x89,

    0x02, 0x03, 0x3C, 0x71, 0x7F, 0x03, 0x0C, 0x00, 0x40, 0x00, 0xB8, 0x2D, 0x2F, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xE3, 0x05, 0x1F, 0x01, 0x49, 0x90, 0x05, 0x20, 0x04, 0x03, 0x02, 0x07,
    0x06, 0x01, 0x29, 0x09, 0x07, 0x01, 0x11, 0x07, 0x00, 0x15, 0x07, 0x00, 0x01, 0x1D, 0x00, 0x72,
    0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x8C, 0x0A,
    0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x18,
    0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0x0B, 0x88, 0x21, 0x00,
    0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9D
};

static int b_set_hdmi_input(bool enabled)
{
    static NEXUS_HdmiInputHandle hdmiInput = NULL;
    int rc;
    
    if (enabled) {
        if (!hdmiInput) {
            NEXUS_HdmiInputSettings hdmiInputSettings;
            NEXUS_HdmiInput_GetDefaultSettings(&hdmiInputSettings);
            hdmiInputSettings.timebase = NEXUS_Timebase_e0;
            hdmiInputSettings.frontend.hpdDisconnected = false;
            hdmiInputSettings.useInternalEdid = true;
            hdmiInput = NEXUS_HdmiInput_OpenWithEdid(0, &hdmiInputSettings, 
                &SampleEDID[0], (uint16_t) sizeof(SampleEDID));
            if (!hdmiInput) return BERR_TRACE(NEXUS_NOT_AVAILABLE);
        }
        
        rc = brc_client_add_hdmi_input(simple_client_state.rca_client, 0, hdmiInput);
        if (rc) {
            BDBG_ERR(("unable to connect to window"));
            return BERR_TRACE(rc);
        }
    }
    else {
        brc_client_remove_window_input(simple_client_state.rca_client, 0);
    }
    return 0;
}
#else
static int b_set_hdmi_input(bool enabled)
{
    BSTD_UNUSED(enabled);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}
#endif
