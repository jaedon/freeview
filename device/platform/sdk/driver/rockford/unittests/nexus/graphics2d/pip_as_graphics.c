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
 * $brcm_Workfile: pip_as_graphics.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 11/8/10 3:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/pip_as_graphics.c $
 * 
 * Hydra_Software_Devel/4   11/8/10 3:01p jtna
 * SW7405-4680: add gfx checkpoint call
 * 
 * Hydra_Software_Devel/3   8/17/10 4:22p erickson
 * SW7420-928: use NEXUS_VideoBufferType for striped surface
 *
 * Hydra_Software_Devel/2   8/16/10 2:58p jtna
 * SW7405-4702: update
 *
 * Hydra_Software_Devel/1   7/29/10 2:06p jtna
 * SW7405-4680: added pip_as_graphics.c
 *
******************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_mosaic_video_decoder.h"
#include "nexus_video_decoder_extra.h"
#include "nexus_stc_channel.h"
#include "nexus_display.h"
#include "nexus_video_window.h"
#include "nexus_core_utils.h"
#include "nexus_component_output.h"
#include "nexus_graphics2d.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>

BDBG_MODULE(pip_as_graphics);

void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

int main(void)
{
    NEXUS_VideoDecoderHandle vdecode[2];
    NEXUS_VideoDecoderOpenMosaicSettings openSettings;
    NEXUS_VideoDecoderStartSettings videoProgram[2];
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoFormatInfo formatInfo;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle event;
    NEXUS_SurfaceCreateSettings surfaceCreateSettings;
    NEXUS_SurfaceHandle framebuffer[2];
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Error rc;
    unsigned current_fb = 0;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
#if 0
    platformSettings.videoDecoderModuleSettings.heapSize[0].general = 60 * 1024 * 1024; /* your platform may require larger heapsize */
#endif
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    /* Map input band and parser band. This could be eliminated because a straight mapping and TS config is a good default. */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);

    /* Open the pid channels */
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram[0]);
    videoProgram[0].codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram[0].pidChannel = NEXUS_PidChannel_Open(parserBand, 0x31, NULL); /* HD on main */

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram[1]);
    videoProgram[1].codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram[1].pidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL); /* SD on PIP */

    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram[0].pidChannel; /* PCR on video pid */
    videoProgram[0].stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* bring up display and graphics */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e720p;
    displaySettings.format = NEXUS_VideoFormat_e1080i;
    display = NEXUS_Display_Open(0, &displaySettings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
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
    surfaceCreateSettings.width = formatInfo.width;
    surfaceCreateSettings.height = formatInfo.height;
    surfaceCreateSettings.heap = platformConfig.heap[1];
    framebuffer[0] = NEXUS_Surface_Create(&surfaceCreateSettings);
    framebuffer[1] = NEXUS_Surface_Create(&surfaceCreateSettings);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer[0];
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer[1];
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);

    NEXUS_VideoDecoder_GetDefaultOpenMosaicSettings(&openSettings);
    openSettings.maxWidth = 1920;
    openSettings.maxHeight = 1080;
    vdecode[0] = NEXUS_VideoDecoder_OpenMosaic(0, 0, &openSettings);
    NEXUS_VideoDecoder_GetDefaultOpenMosaicSettings(&openSettings);
    openSettings.maxWidth = 720;
    openSettings.maxHeight = 480;
    vdecode[1] = NEXUS_VideoDecoder_OpenMosaic(0, 1, &openSettings);

    window = NEXUS_VideoWindow_Open(display, 0);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode[0]));
    BDBG_ASSERT(!rc);
    /* For a main video-as-graphics window, we call NEXUS_Display_ConnectVideoInput to connect
       display back to video decoder. However, for any secondary window (e.g. PIP, second mosaic),
       we cannot call NEXUS_Display_ConnectVideoInput. The main window will have already made the
       connection. Instead, call NEXUS_VideoDecoder_SetPowerState with true so that the XVD
       channel is created */
    rc = NEXUS_VideoDecoder_SetPowerState(vdecode[1], true);
    BDBG_ASSERT(!rc);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer[current_fb]);
    BDBG_ASSERT(!rc);
    current_fb = 1-current_fb;

    /* Start decode */
    rc = NEXUS_VideoDecoder_Start(vdecode[0], &videoProgram[0]);
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoDecoder_Start(vdecode[1], &videoProgram[1]);
    BDBG_ASSERT(!rc);

    /* Monitor status */
    {
        unsigned cnt[3];
        BKNI_Memset(cnt, 0, sizeof(cnt));
        while (1) {
            NEXUS_StripedSurfaceHandle stripedSurface;

            /* Create a Nexus striped surface for the most recent picture reported by VideoDecoder */
            stripedSurface = NEXUS_VideoDecoder_CreateStripedSurface(vdecode[1]);

            if (stripedSurface) {
                NEXUS_Graphics2DBlitSettings blitSettings;
                NEXUS_SurfaceHandle surface;
                NEXUS_StripedSurfaceCreateSettings createSettings;

                NEXUS_StripedSurface_GetCreateSettings(stripedSurface, &createSettings);

                surface = NEXUS_Graphics2D_Destripe(gfx, stripedSurface);

                /* blit as a full screen "video window". The Blit will do a YCrCb->RGB conversion. */
                NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
                blitSettings.source.surface = surface;
                blitSettings.output.surface = framebuffer[current_fb];
                blitSettings.output.rect.width = 400;
                blitSettings.output.rect.height = 300;
                blitSettings.output.rect.x = formatInfo.width - blitSettings.output.rect.width - 100;
                blitSettings.output.rect.y = 50;
                switch (createSettings.bufferType) {
                case NEXUS_VideoBufferType_eTopField:
#if 0 /* the phase adjustment Nexus API was not checked-in, as we weren't sure if it was doing anything */
                    blitSettings.phaseAdjustment.horizontal = 0;
                    blitSettings.phaseAdjustment.vertical = -1;
                    blitSettings.phaseAdjustment.shift = 2;
#endif
                    cnt[0]++;
                    break;
                case NEXUS_VideoBufferType_eBotField:
#if 0
                    blitSettings.phaseAdjustment.horizontal = 0;
                    blitSettings.phaseAdjustment.vertical = 1;
                    blitSettings.phaseAdjustment.shift = 2;
#endif
                    cnt[1]++;
                    break;
                default: /* frame */
#if 0
                    blitSettings.phaseAdjustment.horizontal = 0;
                    blitSettings.phaseAdjustment.vertical = 0;
                    blitSettings.phaseAdjustment.shift = 0;
#endif
                    cnt[2]++;
                    break;
                }
                rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
                BDBG_ASSERT(!rc);
                rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
                if (rc == NEXUS_GRAPHICS2D_QUEUED) {
                    BKNI_WaitForEvent(event, 0xffffffff);
                }

                NEXUS_StripedSurface_Destroy(stripedSurface);
                NEXUS_Surface_Destroy(surface);

                rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer[current_fb]);

                BDBG_ASSERT(!rc);
                current_fb = 1-current_fb;

                if ((cnt[0]+cnt[1]+cnt[2]) % 100 == 0) {
                    BDBG_WRN(("PIP pictures: top %d, bottom %d, frame %d", cnt[0], cnt[1], cnt[2]));
                }
            }
            else {
                BKNI_Sleep(1);
            }
        }
    }

    /* TODO: shutdown */

    return 0;
}

