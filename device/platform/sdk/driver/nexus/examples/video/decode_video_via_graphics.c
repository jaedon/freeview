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
 * $brcm_Workfile: decode_video_via_graphics.c $
 * $brcm_Revision: 8 $
 * $brcm_Date: 7/9/12 2:59p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/video/decode_video_via_graphics.c $
 * 
 * 8   7/9/12 2:59p erickson
 * SW7425-1136: remove preferred HDMI format. example hardcoded for
 *  720x480.
 * 
 * 7   1/26/12 11:23a rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 6   11/29/10 3:10p erickson
 * SW7420-1078: add NEXUS_Platform_GetFramebufferHeap
 * 
 * 5   11/8/10 3:32p jtna
 * SW7405-4702: add gfx checkpoint call
 * 
 * 4   8/17/10 10:59a erickson
 * SW7405-4702: merge
 *
 * SW7405-4680/1   8/16/10 2:56p jtna
 * SW7405-4702: NEXUS_VideoDecoder_CreateStripedSurface() returns NULL on
 *  repeat surfaces
 *
 * 3   3/22/10 2:39a randyjew
 * SW7468-143: Support for boards without component and/or composite
 *
 * 2   2/25/10 10:57a mphillip
 * SW7550-277: Guard video examples for platforms lacking graphics2d
 *
 * 1   1/12/09 12:40p erickson
 * PR50979: example of NEXUS_VideoDecoder_CreateStripedSurface and
 *  NEXUS_Display_ConnectVideoInput
 *
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_video_decoder_extra.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_core_utils.h"
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#if NEXUS_HAS_GRAPHICS2D
#include "nexus_graphics2d.h"
#endif
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

/* the following define the input and its characteristics -- these will vary by input */
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define VIDEO_PID 0x11

void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

int main(int argc, char **argv)
{
#if NEXUS_HAS_GRAPHICS2D
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoFormatInfo formatInfo;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle event;
    NEXUS_SurfaceCreateSettings surfaceCreateSettings;
    NEXUS_SurfaceHandle framebuffer;
    NEXUS_SurfaceMemory mem;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Error rc;
    unsigned loops = 10000;

    if (argc > 1) loops = atoi(argv[1]);

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the pid channels */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, VIDEO_PID, NULL);

    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
    videoProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* bring up display and graphics */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
#endif

    NEXUS_VideoFormat_GetInfo(displaySettings.format, &formatInfo);

    BKNI_CreateEvent(&event);

    gfx = NEXUS_Graphics2D_Open(0, NULL);
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = event;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);

    /* create the framebuffer */
    NEXUS_Surface_GetDefaultCreateSettings(&surfaceCreateSettings);
    surfaceCreateSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    surfaceCreateSettings.width = 720;
    surfaceCreateSettings.height = 480;
    surfaceCreateSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    framebuffer = NEXUS_Surface_Create(&surfaceCreateSettings);
    NEXUS_Surface_GetMemory(framebuffer, &mem);
    BKNI_Memset(mem.buffer, 0, mem.pitch * surfaceCreateSettings.height);

    /* bring up video decoder */
    vdecode = NEXUS_VideoDecoder_Open(0, NULL); /* take default capabilities */

    /* Tell Display module to connect to the VideoDecoder module and supply the
    L1 INT id's from BVDC_Display_GetInterrupt. Display will not register for the data ready ISR callback. */
    NEXUS_Display_ConnectVideoInput(display, NEXUS_VideoDecoder_GetConnector(vdecode));

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);

    /* Start decode */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);

    /* Monitor status */
    while (1) {
        NEXUS_StripedSurfaceHandle stripedSurface;

        /* Create a Nexus striped surface for the most recent picture reported by VideoDecoder */
        stripedSurface = NEXUS_VideoDecoder_CreateStripedSurface(vdecode);

        if (stripedSurface) {
            NEXUS_Graphics2DBlitSettings blitSettings;
            NEXUS_SurfaceHandle surface;
            surface = NEXUS_Graphics2D_Destripe(gfx, stripedSurface);

            /* blit as a full screen "video window". The Blit will do a YCrCb->RGB conversion. */
            NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
            blitSettings.source.surface = surface;
            blitSettings.output.surface = framebuffer;
            rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
            BDBG_ASSERT(!rc);

            rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
            if (rc==NEXUS_GRAPHICS2D_QUEUED) {
                rc = BKNI_WaitForEvent(event, BKNI_INFINITE);
                BDBG_ASSERT(!rc);
            }

            NEXUS_Surface_Destroy(surface);
            NEXUS_VideoDecoder_DestroyStripedSurface(vdecode, stripedSurface);

            loops--;
        }
        else {
            BKNI_Sleep(1);
        }

        if (!loops) {
            break;
        }
    }

    NEXUS_VideoDecoder_Stop(vdecode);
    NEXUS_Display_DisconnectVideoInput(display, NEXUS_VideoDecoder_GetConnector(vdecode));
    NEXUS_VideoDecoder_Close(vdecode);
    NEXUS_Surface_Destroy(framebuffer);
    NEXUS_PidChannel_Close(videoProgram.pidChannel);
    NEXUS_StcChannel_Close(videoProgram.stcChannel);
    NEXUS_Display_Close(display);
    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Platform_Uninit();

    BKNI_Sleep(10000);
#else
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);
#endif
    return 0;
}
