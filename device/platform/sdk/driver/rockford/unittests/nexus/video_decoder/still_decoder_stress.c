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
 * $brcm_Workfile: still_decoder_stress.c $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 7/28/10 5:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/video_decoder/still_decoder_stress.c $
 * 
 * Hydra_Software_Devel/4   7/28/10 5:22p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/4   7/28/10 5:07p calvinho
 * SW7405-4653: Added Ip Streamer input support
 * 
 * Hydra_Software_Devel/3   5/10/10 7:02p mward
 * SW7125-253: Allow compilation with C++, eliminate "invalid conversion"
 * and other errors and warnings.
 * 
 * Hydra_Software_Devel/2   5/4/09 5:02p erickson
 * PR42679: allow test to pass with no stills
 *
 * Hydra_Software_Devel/1   1/21/09 4:27p erickson
 * PR51314: added still_decoder_stress
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_core_utils.h"
#include "nexus_component_output.h"
#include "nexus_video_decoder.h"
#include "nexus_video_window.h"
#include "nexus_video_input.h"
#include "nexus_parser_band.h"
#include "nexus_still_decoder.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_playpump.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef IP_STREAMER_SUBST
#include "ip_streamer_subst.h"
#endif

#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif

static bool g_stillReady = false;
void start_still_decode(unsigned index);
void stop_still_decode(void);

#define MAX_STILLS 100
struct stills_t {
    const char *filename;
    NEXUS_VideoCodec codec;
} g_stills[MAX_STILLS];

NEXUS_StillDecoderHandle stillDecoder;
NEXUS_PlaypumpHandle playpump;
NEXUS_Graphics2DHandle blitter;
NEXUS_SurfaceHandle framebufferSurface;
NEXUS_VideoDecoderHandle videoDecoder;

int main(int argc, char **argv)
{
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoFormatInfo videoFormatInfo;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_PlaypumpSettings playpumpSettings;
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    int i;
    NEXUS_Error rc;
    unsigned total_stills = 0;
    unsigned loops = 1000;
    bool still_started = false;
    NEXUS_VideoDecoderOpenSettings videoDecoderOpenSettings;
#ifdef IP_STREAMER_SUBST
    IpsHandle ips;
    IpsOpenSettings ipsOpenSettings;
    IpsStartSettings ipsStartSettings;
    IpsTimebaseSettings ipsTimebaseSettings;
#endif

    g_stills[0].codec = NEXUS_VideoCodec_eMpeg2;
    for (i=1;i<argc && total_stills < MAX_STILLS;i++) {
        if (!strcmp(argv[i], "-avc")) {
            g_stills[total_stills].codec = NEXUS_VideoCodec_eH264;
            continue;
        }
        else if (!strcmp(argv[i], "-mpeg")) {
            g_stills[total_stills].codec = NEXUS_VideoCodec_eMpeg2;
            continue;
        }
        else if (!strcmp(argv[i], "-vc1")) {
            g_stills[total_stills].codec = NEXUS_VideoCodec_eVc1;
            continue;
        }
        else {
            g_stills[total_stills++].filename = argv[i];
            g_stills[total_stills].codec = NEXUS_VideoCodec_eMpeg2;
        }
    }

    if (!total_stills) {
        return 0;
    }

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

#if NEXUS_DTV_PLATFORM
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_PanelOutput_GetConnector(platformConfig.outputs.panel[0]));
    NEXUS_BoardCfg_ConfigurePanel(true, true, true);
    fprintf(stderr, "Panel output ready\n");
#else
    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_e720p;
    display = NEXUS_Display_Open(0, &displaySettings);
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif

    window = NEXUS_VideoWindow_Open(display, 0);
    blitter = NEXUS_Graphics2D_Open(0, NULL);

    /* Get the streamer input band from Platform. Platform has already configured the FPGA with a default streamer routing */
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);

    /* Map a parser band to the streamer input band. */
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = NEXUS_TransportType_eTs;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
#ifdef IP_STREAMER_SUBST
    ipsOpenSettings.parserBand=parserBand;
    ipsOpenSettings.playpumpIndex=1;
    ipsOpenSettings.remuxIndex=1;
    ips = IpsOpen(&ipsOpenSettings);
#endif

    videoPidChannel = NEXUS_PidChannel_Open(parserBand, 0x11, NULL);

    /* Open the StcChannel to do lipsync with the PCR */
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR on video pid */
#ifdef IP_STREAMER_SUBST
    IpsAddStcSettings(ips, &stcSettings);
#endif
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    /* configure transport and still decoder */
    playpump = NEXUS_Playpump_Open(0, NULL);
    BDBG_ASSERT(playpump);
    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.transportType = NEXUS_TransportType_eEs;
    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);

    NEXUS_VideoDecoder_GetDefaultOpenSettings(&videoDecoderOpenSettings);
#ifdef IP_STREAMER_SUBST
    IpsAddVideoDecoderOpenSettings(ips, &videoDecoderOpenSettings);
#endif
    videoDecoder = NEXUS_VideoDecoder_Open(0, &videoDecoderOpenSettings);

    NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.codec = NEXUS_VideoCodec_eMpeg2;
    videoProgram.stcChannel = stcChannel;

    /* create graphics framebuffer */
    NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    createSettings.width = 720;
    createSettings.height = videoFormatInfo.height;
    framebufferSurface = NEXUS_Surface_Create(&createSettings);

    /* fill black before the first still */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebufferSurface;
    fillSettings.color = 0x00000000; /* transparent */
    rc = NEXUS_Graphics2D_Fill(blitter, &fillSettings);
    BDBG_ASSERT(!rc);

    /* force full screen framebuffer. no horizontal upscale. */
    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.clip.width = createSettings.width;
    graphicsSettings.clip.height = createSettings.height;
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(display, framebufferSurface);

#ifdef IP_STREAMER_SUBST
    ipsStartSettings.vDecoder = videoDecoder;
    ipsStartSettings.aDecoder = NULL;
    ipsStartSettings.stcChannel = stcChannel;
    IpsStart(ips, &ipsStartSettings);
#endif

    rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    BDBG_ASSERT(!rc);

    BKNI_Sleep(3000);

    while (loops--) {
        switch (rand() % 3) {
        case 0:
            NEXUS_VideoDecoder_Stop(videoDecoder);
#if 0
            if (rand() % 2) {
                NEXUS_VideoWindow_RemoveAllInputs(window);
                NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
                NEXUS_VideoDecoder_Close(videoDecoder);
                videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
                NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
            }
#endif
            rc = NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
            BDBG_ASSERT(!rc);
            BKNI_Sleep(rand() % 3000);
            break;
        case 1:
            if (!still_started) {
                start_still_decode(rand() % total_stills);
                still_started = true;
                BKNI_Sleep(rand() % 1000);
            }
            break;
        case 2:
            /* don't check still_started */
            stop_still_decode();
            still_started = false;
            BKNI_Sleep(rand() % 1000);
            break;
        }
    }
#ifdef IP_STREAMER_SUBST
    IpsClose(ips);
#endif

    return 0;
}

static void still_picture_ready(void *context, int unused)
{
    BSTD_UNUSED(context);
    BSTD_UNUSED(unused);
    printf("still_picture_ready\n");
    g_stillReady = true;
}

void start_still_decode(unsigned index)
{
    NEXUS_StillDecoderStartSettings stillDecoderSettings;
    FILE *file;
    unsigned feed_zeros = 256;
    bool feed_file = true;
    NEXUS_Error rc;

    printf("Decoding %s...\n", g_stills[index].filename);
    file = fopen(g_stills[index].filename, "r");
    if (!file) {
        fprintf(stderr, "can't open file:%s\n", g_stills[index].filename);
        return;
    }

    g_stillReady = false;

    if (!stillDecoder) {
        stillDecoder = NEXUS_StillDecoder_Open(videoDecoder, 0, NULL);
    }
    NEXUS_StillDecoder_GetDefaultStartSettings(&stillDecoderSettings);
    stillDecoderSettings.pidChannel = NEXUS_Playpump_OpenPidChannel(playpump, 0x1, NULL);
    BDBG_ASSERT(stillDecoderSettings.pidChannel);
    stillDecoderSettings.stillPictureReady.callback = still_picture_ready;
    stillDecoderSettings.codec = g_stills[index].codec;
    rc = NEXUS_StillDecoder_Start(stillDecoder, &stillDecoderSettings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Playpump_Start(playpump);
    BDBG_ASSERT(!rc);

    /* feeding 256 bytes of zeros is required to flush the last packet of data through transport */
    while (feed_file || feed_zeros) {
        size_t size;
        void *buffer;

        rc = NEXUS_Playpump_GetBuffer(playpump, &buffer, &size);
        BDBG_ASSERT(!rc);
        if (!size) {
            BKNI_Sleep(10);
            continue;
        }

        if (feed_file) {
            size = fread(buffer, 1, size, file);
            if (size <= 0) {
                /* done reading file. now feed some zeros to flush transport. */
                feed_file = false;
                continue;
            }
            printf("read %d from file\n", size);
        }
        else if (!feed_file && feed_zeros) {
            if (size > feed_zeros) size = feed_zeros;
            BKNI_Memset(buffer, 0, size);
            feed_zeros -= size;
        }
        rc = NEXUS_Playpump_ReadComplete(playpump, 0, size);
        BDBG_ASSERT(!rc);
    }

    BKNI_Sleep(100); /* TODO */
    NEXUS_Playpump_Stop(playpump);
    NEXUS_Playpump_ClosePidChannel(playpump, stillDecoderSettings.pidChannel);
    fclose(file);
}

void stop_still_decode(void)
{
    NEXUS_StripedSurfaceHandle stripedSurface;
    NEXUS_SurfaceHandle stillSurface = NULL;
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_Error rc;
    NEXUS_StripedSurfaceStatus stripedSurfaceStatus;

    if (!g_stillReady) goto done;

    rc = NEXUS_StillDecoder_GetStripedSurface(stillDecoder, &stripedSurface);
    if (rc) {goto done;}

    NEXUS_StripedSurface_GetStatus(stripedSurface, &stripedSurfaceStatus);
    printf("got still %dx%d\n", stripedSurfaceStatus.width, stripedSurfaceStatus.height);

    stillSurface = NEXUS_Graphics2D_Destripe(blitter, stripedSurface);
    if (!stillSurface) {goto done;}

    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
    blitSettings.source.surface = stillSurface;
    blitSettings.output.surface = framebufferSurface;
    blitSettings.output.rect.width = 300;
    blitSettings.output.rect.height = 300;
    blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
    blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyConstant; /* YCrCb has no alpha, so we must set 0xFF */
    blitSettings.constantColor = 0xFF000000; /* alpha is opaque */
    rc = NEXUS_Graphics2D_Blit(blitter, &blitSettings);
    BDBG_ASSERT(!rc);

done:
    g_stillReady = false;
    if (stillSurface) {
        NEXUS_Surface_Destroy(stillSurface);
    }
    if (stillDecoder) {
        NEXUS_StillDecoder_Stop(stillDecoder);
        if (rand() % 2) {
            NEXUS_StillDecoder_Close(stillDecoder);
            stillDecoder = NULL;
        }
    }
}

