/******************************************************************************
 *    (c)2008 Broadcom Corporation
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
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 1/7/11 3:32p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/decode_video_via_graphics.c $
 * 
 * Hydra_Software_Devel/10   1/7/11 3:32p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 * PTS - STC difference
 * 
 * Hydra_Software_Devel/9   11/8/10 3:32p jtna
 * SW7405-4702: add gfx checkpoint call
 * 
 * Hydra_Software_Devel/8   8/16/10 2:57p jtna
 * SW7405-4702: NEXUS_VideoDecoder_CreateStripedSurface() returns NULL on
 * repeat surfaces
 * 
 * Hydra_Software_Devel/7   7/29/10 11:33a calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/6   5/6/09 11:16a erickson
 * PR42679: lengthen test
 *
 * Hydra_Software_Devel/5   4/6/09 2:20p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/4   1/12/09 12:30p erickson
 * PR50979: rename NEXUS_Display_ConnectVideoInput api
 *
 * Hydra_Software_Devel/3   10/23/08 3:59p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/2   9/9/08 4:41p erickson
 * PR46527: use NEXUS_Graphics2D_Blit so that proper YCrCb->RGB conversion
 * happens
 *
 * Hydra_Software_Devel/1   9/8/08 2:58p erickson
 * PR46527: initial checkin
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
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif
#include "nexus_graphics2d.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

int main(void)
{
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
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Error rc;
    unsigned loops = 30 * 60; /* approx 60 seconds */
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=NEXUS_ParserBand_e0;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    /* Open the pid channels */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);

    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
#ifdef IP_STREAMER_SUBST
    IpsAddStcSettings(ips, &stcSettings);
#endif
    videoProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* bring up display and graphics */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
#if NEXUS_DTV_PLATFORM
    display = NEXUS_Display_Open(0, &displaySettings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    BDBG_ASSERT(!rc);
    rc = NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    BDBG_ASSERT(!rc);
#else
    display = NEXUS_Display_Open(0, &displaySettings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &formatInfo);
#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = NULL;
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

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
    framebuffer = NEXUS_Surface_Create(&surfaceCreateSettings);

    /* bring up video decoder */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    vdecode = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);

    /* Tell Display module to connect to the VideoDecoder module and supply the
    L1 INT id's from BVDC_Display_GetInterrupt. Display will not register for the data ready ISR callback. */
    NEXUS_Display_ConnectVideoInput(display, NEXUS_VideoDecoder_GetConnector(vdecode));

#define POLLING_BASED 1
    /* If we don't want a polling based system, there must be an internal connection between VideoDecoder and Graphics2D.
    For a polling based system, no internal connection is required. */
#if !POLLING_BASED
    /* In this mode, the Graphics2D module must register for the VideoDecoder data ready ISR callback.
    Then, the Graphics2D module must have a surface into which it copied the video data. */
    NEXUS_Graphics2D_ConnectVideoInput(gfx, NEXUS_VideoDecoder_GetConnector(vdecode));
    NEXUS_Graphics2D_SetVideoInputSurface(gfx, framebuffer);
#endif

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = vdecode;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    /* Start decode */
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);

    /* Monitor status */
    while (loops--) {
#if POLLING_BASED
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
        }
        else {
            loops++;
            BKNI_Sleep(1);
        }
#else
        /* if !POLLING_BASED, there is no required work. so just monitor status. */

        NEXUS_VideoDecoderStatus status;
        uint32_t stc;
        NEXUS_VideoDecoder_GetStatus(vdecode, &status);
        NEXUS_StcChannel_GetStc(videoProgram.stcChannel, &stc);
        printf("decode %dx%d, pts %#x, stc %#x (diff %d)\n",
            status.source.width, status.source.height, status.pts, stc, status.ptsStcDifference);
        BKNI_Sleep(1000);
#endif
    }

    /* TODO: shutdown */
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif

    return 0;
}
