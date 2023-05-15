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
 * $brcm_Workfile: 3dsettop_pig.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 1/7/11 3:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/3dtv/3dsettop_pig.c $
 * 
 * Hydra_Software_Devel/5   1/7/11 3:31p jtna
 * SW7422-165: use NEXUS_VideoDecoderStatus.ptsStcDifference to calculate
 * PTS - STC difference
 * 
 * Hydra_Software_Devel/4   7/28/10 5:30p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/3   2/22/10 1:31p erickson
 * SW7405-3625: center the video for any display format
 *
 * Hydra_Software_Devel/2   2/22/10 12:50p erickson
 * SW7405-3625: add sbs/ou option
 *
 * Hydra_Software_Devel/1   2/19/10 1:24p erickson
 * SW7405-3625: add example for 3DTV on a settop using backend mosaic for
 * PIG
 *
 *****************************************************************************/

#include "nexus_platform.h"
#include "nexus_pid_channel.h"
#include "nexus_parser_band.h"
#include "nexus_video_decoder.h"
#include "nexus_stc_channel.h"
#include "nexus_core_utils.h"
#include "nexus_hdmi_output.h"
#include "nexus_display.h"
#include "nexus_mosaic_display.h"
#include "nexus_graphics2d.h"
#include "nexus_component_output.h"
#include "nexus_video_window.h"
#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <string.h>

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

int main(int argc, char **argv)
{
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_ParserBand parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_DisplayHandle display;
    NEXUS_VideoWindowHandle parentWindow;
    NEXUS_VideoWindowHandle window[2]; /* mosaics */
    NEXUS_VideoWindowSettings windowSettings[2];
    NEXUS_VideoWindowMosaicSettings mosaicWindowSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoFormatInfo displayFormatInfo;
    NEXUS_SurfaceHandle framebuffer;
    NEXUS_SurfaceCreateSettings createSurfaceSettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_HdmiOutputHandle hdmiOutput;
    NEXUS_Error rc;
    bool sbs = true; /* side-by-side or over-under */
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    if (argc > 1 && !strcmp(argv[1], "ou")) {
        sbs = false;
    }

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
    videoProgram.codec = NEXUS_VideoCodec_eH264;
    videoProgram.pidChannel = NEXUS_PidChannel_Open(parserBand, 0x1e1, NULL);

    /* By default, StcChannel will configure NEXUS_Timebase with the info it has */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoProgram.pidChannel; /* PCR on video pid */
#ifdef IP_STREAMER_SUBST
    IpsAddStcSettings(ips, &stcSettings);
#endif
    videoProgram.stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* bring up display */
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.format = NEXUS_VideoFormat_e1080i;
    displaySettings.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displaySettings);
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
    hdmiOutput = platformConfig.outputs.hdmi[0];
    if (hdmiOutput) {
        rc = NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(hdmiOutput));
        BDBG_ASSERT(!rc);
    }
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &displayFormatInfo);
#ifdef IP_STREAMER_SUBST
    ipsTimebaseSettings.timebaseStc = NEXUS_Timebase_e0;
    ipsTimebaseSettings.timebaseDecouple = NEXUS_Timebase_e1;
    ipsTimebaseSettings.audioDac = platformConfig.outputs.audioDacs[0];
    ipsTimebaseSettings.display = display;
    IpsConfigureTimebase(ips, &ipsTimebaseSettings);
#endif

    parentWindow = NEXUS_VideoWindow_Open(display, 0);

    window[0] = NEXUS_VideoWindow_OpenMosaic(parentWindow, 0);
    NEXUS_VideoWindow_GetSettings(window[0], &windowSettings[0]);
    if (sbs) {
        windowSettings[0].position.x = 50;
        windowSettings[0].position.y = 100;
        windowSettings[0].position.width = displayFormatInfo.width/3;
        windowSettings[0].position.height = displayFormatInfo.height*2/3;
    }
    else {
        windowSettings[0].position.x = 100;
        windowSettings[0].position.y = 50;
        windowSettings[0].position.width = displayFormatInfo.width*2/3;
        windowSettings[0].position.height = displayFormatInfo.height/3;
    }
    windowSettings[0].alpha = 0xFF;
    rc = NEXUS_VideoWindow_SetSettings(window[0], &windowSettings[0]);
    BDBG_ASSERT(!rc);

    /* mosaic 0 uses the left half of the video */
    NEXUS_VideoWindow_GetMosaicSettings(window[0], &mosaicWindowSettings);
    mosaicWindowSettings.backendMosaic.enabled = true;
    mosaicWindowSettings.backendMosaic.clipRect.x = 0;
    mosaicWindowSettings.backendMosaic.clipRect.y = 0;
    mosaicWindowSettings.backendMosaic.clipRect.width = 1920/2;
    mosaicWindowSettings.backendMosaic.clipRect.height = 1080;
    mosaicWindowSettings.backendMosaic.clipBase.width = 1920;
    mosaicWindowSettings.backendMosaic.clipBase.height = 1080;
    rc = NEXUS_VideoWindow_SetMosaicSettings(window[0], &mosaicWindowSettings);
    BDBG_ASSERT(!rc);

    window[1] = NEXUS_VideoWindow_OpenMosaic(parentWindow, 1);
    NEXUS_VideoWindow_GetSettings(window[1], &windowSettings[1]);
    windowSettings[1].position = windowSettings[0].position;
    if (sbs) {
        windowSettings[1].position.x = windowSettings[1].position.x + displayFormatInfo.width / 2;
    }
    else {
        windowSettings[1].position.y = windowSettings[1].position.y + displayFormatInfo.height / 2;
    }
    windowSettings[1].alpha = 0xFF;
    rc = NEXUS_VideoWindow_SetSettings(window[1], &windowSettings[1]);
    BDBG_ASSERT(!rc);

    /* mosaic 1 uses the right half of the video */
    NEXUS_VideoWindow_GetMosaicSettings(window[1], &mosaicWindowSettings);
    mosaicWindowSettings.backendMosaic.enabled = true;
    mosaicWindowSettings.backendMosaic.clipRect.x = 1920/2;
    mosaicWindowSettings.backendMosaic.clipRect.y = 0;
    mosaicWindowSettings.backendMosaic.clipRect.width = 1920/2;
    mosaicWindowSettings.backendMosaic.clipRect.height = 1080;
    mosaicWindowSettings.backendMosaic.clipBase.width = 1920;
    mosaicWindowSettings.backendMosaic.clipBase.height = 1080;
    rc = NEXUS_VideoWindow_SetMosaicSettings(window[1], &mosaicWindowSettings);
    BDBG_ASSERT(!rc);

    /* bring up decoder and connect to display via two windows */
    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);

    rc = NEXUS_VideoWindow_AddInput(window[0], NEXUS_VideoDecoder_GetConnector(videoDecoder));
    BDBG_ASSERT(!rc);
    rc = NEXUS_VideoWindow_AddInput(window[1], NEXUS_VideoDecoder_GetConnector(videoDecoder));
    BDBG_ASSERT(!rc);

    /* bring up graphics */
    NEXUS_Surface_GetDefaultCreateSettings(&createSurfaceSettings);
    createSurfaceSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSurfaceSettings.width = displayFormatInfo.width;
    createSurfaceSettings.height = displayFormatInfo.height;
    framebuffer = NEXUS_Surface_Create(&createSurfaceSettings);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    /* fill whole screen with blue */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.color = 0xFF104080;
    NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    /* punch through the picture-in-graphics hole for first window */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.rect = windowSettings[0].position;
    fillSettings.color = 0x00cacaca;
    NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    /* second window */
    fillSettings.rect = windowSettings[1].position;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    graphicsSettings.clip.width = displayFormatInfo.width;
    graphicsSettings.clip.height = displayFormatInfo.height;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);
    rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
    BDBG_ASSERT(!rc);

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = videoDecoder;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = NULL;
    IpsStart(ips, &ipsStartSettings);
#endif

    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);

    for (;;) {
        NEXUS_VideoDecoderStatus status;
        uint32_t stc;

        NEXUS_VideoDecoder_GetStatus(videoDecoder, &status);
        NEXUS_StcChannel_GetStc(videoProgram.stcChannel, &stc);
        printf("decode %dx%d, pts %#x, stc %#x (diff %d)\n",
            status.source.width, status.source.height, status.pts, stc, status.ptsStcDifference);
        BKNI_Sleep(1000);
    }

    getchar();
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif
    return 0;
}
