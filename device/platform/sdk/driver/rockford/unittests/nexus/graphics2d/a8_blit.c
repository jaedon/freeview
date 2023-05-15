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
 * $brcm_Workfile: a8_blit.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 7/29/10 11:28a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/a8_blit.c $
 * 
 * Hydra_Software_Devel/2   7/29/10 11:28a calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/1   3/28/09 11:05a erickson
 * PR53661: added a8_blit test
 *
 * Hydra_Software_Devel/5   11/3/08 11:30a erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/4   10/23/08 3:59p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/3   9/19/08 11:53a erickson
 * PR47014: update
 *
 * Hydra_Software_Devel/2   9/19/08 9:11a erickson
 * PR47014: made fixes. premultiplied alpha with translucent video window
 * is now working.
 *
 * Hydra_Software_Devel/1   9/18/08 11:53a erickson
 * PR47014: update
 *
 * Hydra_Software_Devel/1   9/17/08 10:32a erickson
 * PR47014: add blit test app
 *
 * 4   8/20/08 1:47p jgarrett
 * PR45866: added NEXUS_Graphics2DOpenSettings to NEXUS_Graphics2D_Open
 *
 * 3   6/19/08 2:33p vsilyaev
 * PR 40921: There is no component output on the 93556 platform
 *
 *****************************************************************************/
/* Nexus example app: show graphics framebuffer with CPU drawing */

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"
#include "nexus_core_utils.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif
#include "nexus_parser_band.h"
#include "nexus_stc_channel.h"
#include "nexus_video_decoder.h"
#include "nexus_video_window.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

int main(void)
{
    NEXUS_SurfaceHandle framebuffer, surface;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_SurfaceMemory mem;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoFormatInfo formatInfo;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoDecoderHandle vdecode;
    NEXUS_Error rc;
    unsigned y;
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
    window = NEXUS_VideoWindow_Open(display, 0);
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &formatInfo);

    /* start decode */
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    NEXUS_Platform_GetStreamerInputBand(0, &parserBandSettings.sourceTypeSettings.inputBand);
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex = 0;
    ipsOpenSettings.remuxIndex = 0;
    ips = IpsOpen(&ipsOpenSettings);
#endif
    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr;
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel;
#ifdef IP_STREAMER_SUBST
    IpsAddStcSettings(ips, &stcSettings);
#endif
    videoProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings); 
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    vdecode = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);    
    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(vdecode));
#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = vdecode;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

printf("%u\n", __LINE__);
#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = NULL;
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = formatInfo.width;
    createSettings.height = formatInfo.height;
    framebuffer = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(framebuffer, &mem);
    BKNI_Memset(mem.buffer, 0xFF, mem.pitch * createSettings.height); /* white */
    NEXUS_Surface_Flush(framebuffer);

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8;
    createSettings.width = 200;
    createSettings.height = 200;
    surface = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(surface, &mem);
    for (y=0;y<createSettings.height;y++) {
        BKNI_Memset((uint8_t*)mem.buffer+(mem.pitch*y), y, mem.pitch);
    }
    NEXUS_Surface_Flush(surface);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    graphicsSettings.clip.width = formatInfo.width; /* no GFD upscale */
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
    BDBG_ASSERT(!rc);

    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = surface;
    blitSettings.output.surface = framebuffer;
    blitSettings.output.rect.x = 150;
    blitSettings.output.rect.y = 150;
    blitSettings.output.rect.width = 200;
    blitSettings.output.rect.height = 200;
    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
    blitSettings.colorOp = NEXUS_BlitColorOp_eUseBlendEquation;
    blitSettings.colorBlend.a = NEXUS_BlendFactor_eConstantColor;
    blitSettings.colorBlend.b = NEXUS_BlendFactor_eOne;
    blitSettings.constantColor = 0xFFFF0000; /* red */
    rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
    BDBG_ASSERT(!rc);

    BKNI_Sleep(10000);

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    return 0;
}

