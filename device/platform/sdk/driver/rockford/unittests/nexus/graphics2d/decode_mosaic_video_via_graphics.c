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
 * $brcm_Workfile: decode_mosaic_video_via_graphics.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 9/9/10 11:32a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/decode_mosaic_video_via_graphics.c $
 * 
 * Hydra_Software_Devel/5   9/9/10 11:32a jtna
 * SW7405-4702: fix broken app
 * 
 * Hydra_Software_Devel/4   5/6/09 11:16a erickson
 * PR42679: shorten test
 *
 * Hydra_Software_Devel/3   1/12/09 12:30p erickson
 * PR50979: rename NEXUS_Display_ConnectVideoInput api
 *
 * Hydra_Software_Devel/2   1/6/09 7:39p erickson
 * PR50749: added shutdown code
 *
 * Hydra_Software_Devel/1   12/29/08 3:04p erickson
 * PR50749: added decode_mosaic_video_via_graphics.c
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
#include "nexus_mosaic_video_decoder.h"
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

#define NUM_MOSAICS 4

int main(void)
{
    NEXUS_VideoDecoderHandle vdecode[NUM_MOSAICS];
    NEXUS_VideoDecoderStartSettings videoProgram[NUM_MOSAICS];
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoFormatInfo formatInfo;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_SurfaceCreateSettings surfaceCreateSettings;
    NEXUS_SurfaceHandle framebuffer;
    NEXUS_SurfaceMemory mem;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Error rc;
    unsigned i;
    unsigned loops = 5000; /* about 30 seconds */

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

    /* bring up decoder and connect to display */
    for (i=0;i<NUM_MOSAICS;i++) {
        NEXUS_VideoDecoderOpenMosaicSettings openSettings;

        NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram[i]);
        videoProgram[i].codec = NEXUS_VideoCodec_eH264;
        videoProgram[i].pidChannel = NEXUS_PidChannel_Open(parserBand, 0x1001 + i, NULL);

        NEXUS_VideoDecoder_GetDefaultOpenMosaicSettings(&openSettings);
        openSettings.maxWidth = 352; /* CIF */
        openSettings.maxHeight = 288; /* CIF */
        vdecode[i] = NEXUS_VideoDecoder_OpenMosaic(0, i, &openSettings);
    }

    /* to keep app simple, no stc channel */

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

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    /* create the framebuffer */
    NEXUS_Surface_GetDefaultCreateSettings(&surfaceCreateSettings);
    surfaceCreateSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    surfaceCreateSettings.width = 720;
    surfaceCreateSettings.height = 480;
    framebuffer = NEXUS_Surface_Create(&surfaceCreateSettings);
    NEXUS_Surface_GetMemory(framebuffer, &mem);
    BKNI_Memset(mem.buffer, 0, mem.pitch * surfaceCreateSettings.height);

    /* Tell Display module to connect to the VideoDecoder module and supply the
    L1 INT id's from BVDC_Display_GetInterrupt. Display will not register for the data ready ISR callback. */
    NEXUS_Display_ConnectVideoInput(display, NEXUS_VideoDecoder_GetConnector(vdecode[0]));

    /* For a main video-as-graphics window, we call NEXUS_Display_ConnectVideoInput to connect
       display back to video decoder. However, for any secondary window (e.g. PIP, second mosaic),
       we cannot call NEXUS_Display_ConnectVideoInput. The main window will have already made the
       connection. Instead, call NEXUS_VideoDecoder_SetPowerState with true so that the XVD
       channel is created */
    for (i=1; i<NUM_MOSAICS; i++) {
        rc = NEXUS_VideoDecoder_SetPowerState(vdecode[i], true);
        BDBG_ASSERT(!rc);
    }

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);

    /* Start Decoders */
    for (i=0;i<NUM_MOSAICS;i++) {
        NEXUS_VideoDecoder_Start(vdecode[i], &videoProgram[i]);
    }

    /* Monitor status */
    while (1) {
        NEXUS_StripedSurfaceHandle stripedSurfaces[NUM_MOSAICS];
        unsigned num, i;
        bool sleep = true;

        /* Retrieve mosaics only from the first mosaic video decoder */
        NEXUS_VideoDecoder_CreateStripedMosaicSurfaces(vdecode[0], stripedSurfaces, NUM_MOSAICS, &num);

        for (i=0;i<num;i++) {
            NEXUS_Graphics2DBlitSettings blitSettings;
            NEXUS_SurfaceHandle surface;

            if (stripedSurfaces[i] == NULL) {
                continue;
            }
            sleep = false;
            surface = NEXUS_Graphics2D_Destripe(gfx, stripedSurfaces[i]);

            /* blit as a set of mosaic video windows. The Blit will do a YCrCb->RGB conversion. */
            NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
            blitSettings.source.surface = surface;
            blitSettings.output.surface = framebuffer;
            blitSettings.output.rect.x = 50 + (120 * i);
            blitSettings.output.rect.y = 50;
            blitSettings.output.rect.width = 100;
            blitSettings.output.rect.height = 100;
            rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
            BDBG_ASSERT(!rc);

            NEXUS_Surface_Destroy(surface);
        }

        /* sleep if all surfaces are NULL */
        if (sleep) {
            BKNI_Sleep(1);
        }
        else {
            loops--;
        }

        NEXUS_VideoDecoder_DestroyStripedMosaicSurfaces(vdecode[0], stripedSurfaces, num);

        if (!loops)
            break;
    }

    /* shutdown */
    for (i=0;i<NUM_MOSAICS;i++) {
        NEXUS_VideoDecoder_Stop(vdecode[i]);
        NEXUS_PidChannel_Close(videoProgram[i].pidChannel);
    }
    NEXUS_Display_DisconnectVideoInput(display, NEXUS_VideoDecoder_GetConnector(vdecode[0]));
    for (i=1; i<NUM_MOSAICS; i++) {
        rc = NEXUS_VideoDecoder_SetPowerState(vdecode[i], false);
        BDBG_ASSERT(!rc);
    }
    for (i=0;i<NUM_MOSAICS;i++) {
        NEXUS_VideoDecoder_Close(vdecode[i]);
    }

    NEXUS_Surface_Destroy(framebuffer);
    NEXUS_Graphics2D_Close(gfx);
    NEXUS_Display_Close(display);
    NEXUS_Platform_Uninit();

    return 0;
}
