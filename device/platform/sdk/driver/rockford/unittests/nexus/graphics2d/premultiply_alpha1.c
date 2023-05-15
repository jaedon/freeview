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
 * $brcm_Workfile: premultiply_alpha1.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 2/21/12 12:53p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/premultiply_alpha1.c $
 * 
 * Hydra_Software_Devel/9   2/21/12 12:53p erickson
 * SWDEPRECATED-3044: fix checkpoint
 * 
 * Hydra_Software_Devel/8   10/25/10 5:37p randyjew
 * SW7468-6: Modifed for chips that do not have Component.
 * 
 * Hydra_Software_Devel/7   7/29/10 11:42a calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/6   7/15/10 11:00a erickson
 * SWDEPRECATED-3340: improve comments on using a pre-multiplied alpha
 * framebuffer
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

static void checkpoint(NEXUS_Graphics2DHandle gfx)
{
    NEXUS_Graphics2DSettings gfxSettings;
    int rc;
    
    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.pollingCheckpoint = true;
    rc = NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    BDBG_ASSERT(!rc);
    do {
        rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL);
        if (rc == NEXUS_GRAPHICS2D_BUSY) {
            BKNI_Sleep(1);
            continue;
        }
        BDBG_ASSERT(!rc);
        break;
    } while (1);
}

int main(void)
{
    NEXUS_SurfaceHandle stillplane, osd, framebuffer, fullscreen_osd, tempbuffer;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoFormatInfo formatInfo;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
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
    NEXUS_Rect osd_location = {100,100,720/2,480/2};
    NEXUS_Rect video_hole = {200,200,720/2,480/2};
    int i,j;
    int loops = 256 * 10;
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
#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif	
#endif
    window = NEXUS_VideoWindow_Open(display, 0);
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &formatInfo);
#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = platformConfig.outputs.audioDacs[0];
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif
    
    /* start decode */
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
    NEXUS_VideoDecoder_Start(vdecode, &videoProgram);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    /* no alpha per pixel in the stillplane */
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eR5_G6_B5;
    createSettings.width = formatInfo.width;
    createSettings.height = formatInfo.height;
    stillplane = NEXUS_Surface_Create(&createSettings);

    /* osd is a part of an OSD. a real app may have multiple OSD surfaces */
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = osd_location.width;
    createSettings.height = osd_location.height;
    osd = NEXUS_Surface_Create(&createSettings);

    /* fullscreen_osd is summation of all osd surfaces */
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = formatInfo.width;
    createSettings.height = formatInfo.height;
    fullscreen_osd = NEXUS_Surface_Create(&createSettings);

    /* framebuffer is the final graphics */
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = formatInfo.width;
    createSettings.height = formatInfo.height;
    framebuffer = NEXUS_Surface_Create(&createSettings);

    /*
    tempbuffer is needed for a couple reasons:
    1) if you want to composite your stillplane and video tunnel less frequently than the osd
    2) this app lacks double buffering. this tempbuffer performs a quasi-double buffering for this app.
    you can just set tempbuffer = framebuffer and it will work, but you will see transient artifacts
    */
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = formatInfo.width;
    createSettings.height = formatInfo.height;
    tempbuffer = NEXUS_Surface_Create(&createSettings); /* TODO: needed? */

    /* draw blue boxes for the OSD */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = osd;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = osd;
    for (i=0;i<osd_location.width-20;i+=40) {
        for (j=0;j<osd_location.height-20;j+=40) {
            fillSettings.rect.x = i;
            fillSettings.rect.width = 20;
            fillSettings.rect.y = j;
            fillSettings.rect.height = 20;
            fillSettings.color = 0x800000FF; /* 50%alpha blue for all boxes */
            rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
            BDBG_ASSERT(!rc);
        }
    }

    /* composite the fullscreen_osd. in a real app, this is more complicated.*/
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = fullscreen_osd;
    fillSettings.color = 0;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Graphics2D_FastBlit(gfx, fullscreen_osd, &osd_location, osd, NULL, NEXUS_BlitColorOp_eCopySource, NEXUS_BlitAlphaOp_eCopySource, 0);
    BDBG_ASSERT(!rc);

    /* stillplane is solid red */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = stillplane;
    fillSettings.color = 0xFFFF0000; /* red */
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    
    checkpoint(gfx);

    /*
    All alpha premultiply formulas can be found at
    http://twiki-01.broadcom.com/bin/view/Arch/GraphicsLayerReduction (internal broadcom twiki)
    OpenTV_OSD_ImageLayer_AnalysisAndDesign_for_7401.pdf (internal broadcom design doc)

    The number of blits in this app are minimal. However, there are ways to optimize this app in terms of memory bandwidth.
    For instance, the screen can be divded into 5 regions: the video hole and four regions around the video hole.
    The four non-video hole regions can be handles with a single blit combining stillsurface and osd directly to the framebuffer.
    The video hole region must be handled with multiple blits (if video is translucent) but the total memory bandwidth is less.
    The video hole region can be handled with a single blit if video is opaque.
    */

    /*
    Run a loop which fades the video window in and out, under an OSD, on top of a still plane (i.e. vary Av)
    */
    for (i=0;i<loops;i++) {
        unsigned alpha = i%256;

        /* copy the stillplane to a temp buffer. punch an alpha video hole and premultiply color.
            At = Av
            Bt = (1 - Av) * B2

        This is one with two blits:
            blit 1: copy B2 to Bt and write 0 to At
            blit 2: write Av and premultiply the color in the video hole

        NOTE: this example does not use A2. it is assumed that the stillplane is opaque.
        */
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = stillplane; /* B2 */
        blitSettings.output.surface = tempbuffer; /* Bt and At */
        blitSettings.constantColor = 0; /* Av (expect for the video hole) */
        blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource; /* copy B2 to Bt */
        blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyConstant; /* write 0 to At */
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);

        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = tempbuffer;
        blitSettings.source.rect = video_hole;
        blitSettings.output.surface = tempbuffer;
        blitSettings.output.rect = video_hole;
        blitSettings.constantColor = alpha<<24; /* Av (for the video hole) */
        blitSettings.alphaOp = NEXUS_BlitAlphaOp_eUseBlendEquation;
        blitSettings.alphaBlend.a = NEXUS_BlendFactor_eConstantAlpha;
        blitSettings.alphaBlend.b = NEXUS_BlendFactor_eOne;
        blitSettings.colorOp = NEXUS_BlitColorOp_eUseBlendEquation;
        blitSettings.colorBlend.a = NEXUS_BlendFactor_eInverseConstantAlpha;
        blitSettings.colorBlend.b = NEXUS_BlendFactor_eSourceColor;
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);

        /* combine the OSD w/ the temp buffer into the framebuffer
            Ag = 1 - (1 - A1) * At
            Bg = A1 * B1 + (1 - A1) * Bt
        */
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = fullscreen_osd; /* B1 and A1 */
        blitSettings.dest.surface = tempbuffer; /* Bt and At */
        blitSettings.output.surface = framebuffer; /* Bg and Ag */

        blitSettings.alphaOp = NEXUS_BlitAlphaOp_eUseBlendEquation;
        blitSettings.alphaBlend.a = NEXUS_BlendFactor_eOne;
        blitSettings.alphaBlend.b = NEXUS_BlendFactor_eOne;
        blitSettings.alphaBlend.subtract_cd = true;
        blitSettings.alphaBlend.c = NEXUS_BlendFactor_eInverseSourceAlpha;
        blitSettings.alphaBlend.d = NEXUS_BlendFactor_eDestinationAlpha;
        blitSettings.alphaBlend.subtract_e = false;
        blitSettings.alphaBlend.e = NEXUS_BlendFactor_eZero;

        blitSettings.colorOp = NEXUS_BlitColorOp_eUseBlendEquation;
        blitSettings.colorBlend.a = NEXUS_BlendFactor_eSourceColor;
        blitSettings.colorBlend.b = NEXUS_BlendFactor_eSourceAlpha;
        blitSettings.colorBlend.subtract_cd = false;
        blitSettings.colorBlend.c = NEXUS_BlendFactor_eInverseSourceAlpha;
        blitSettings.colorBlend.d = NEXUS_BlendFactor_eDestinationColor;
        blitSettings.colorBlend.subtract_e = false;
        blitSettings.colorBlend.e = NEXUS_BlendFactor_eZero;

        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        
        checkpoint(gfx);

        /* final GFD equation:
            1 * Bg + (1 - Ag) * Bv

        This configures the GFD to use a pre-multiplied alpha framebuffer.
            Ag = graphics alpha plane
            Bg = graphics color plane
            Av = doesn't exist. video has no alpha.
            Bv = video color plane

        If it wasn't pre-multiplied, the equation would be Ag * Bg + (1 - Ag) * Bv.
        The essence of pre-multiplied alpha is that Bg already had Ag applied to it prior to GFD.
        */
        NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
        graphicsSettings.enabled = true;
        graphicsSettings.clip.width = formatInfo.width; /* no GFD upscale */
        graphicsSettings.sourceBlendFactor = NEXUS_CompositorBlendFactor_eConstantAlpha; /* graphics is the source */
        graphicsSettings.destBlendFactor = NEXUS_CompositorBlendFactor_eInverseSourceAlpha; /* video is the dest */
        graphicsSettings.constantAlpha = 0xFF; /* this is "1" in the equation */
        rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
        BDBG_ASSERT(!rc);

        if (alpha == 0) {
            printf("video is transparent\n");
            BKNI_Sleep(2000);
        }
        else if (alpha == 255) {
            /* NOTE: must have active video source to have CMP_0_V0 visible. */
            printf("video is opaque\n");
            BKNI_Sleep(2000);
        }
        else {
            if (alpha == 1) {
                printf("video is fading in...\n");
            }
            BKNI_Sleep(10);
        }

        if (alpha == 128) {
            printf("at the midpoint of the video fade, change the compositor background color. there should be no visible change.\n");
            NEXUS_Display_GetSettings(display, &displaySettings);
            displaySettings.background = 0xFF00FF00;
            rc = NEXUS_Display_SetSettings(display, &displaySettings);
            BDBG_ASSERT(!rc);
            BKNI_Sleep(1000);
            NEXUS_Display_GetSettings(display, &displaySettings);
            displaySettings.background = 0xFFFF0000;
            rc = NEXUS_Display_SetSettings(display, &displaySettings);
            BDBG_ASSERT(!rc);
            BKNI_Sleep(1000);
            NEXUS_Display_GetSettings(display, &displaySettings);
            displaySettings.background = 0xFF00FFFF;
            rc = NEXUS_Display_SetSettings(display, &displaySettings);
            BDBG_ASSERT(!rc);
            BKNI_Sleep(1000);
            NEXUS_Display_GetSettings(display, &displaySettings);
            displaySettings.background = 0xFF0000FF;
            rc = NEXUS_Display_SetSettings(display, &displaySettings);
            BDBG_ASSERT(!rc);
            BKNI_Sleep(1000);
        }
    }

#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    return 0;
}

