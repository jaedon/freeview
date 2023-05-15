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
 * $brcm_Workfile: still_decoder.c $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 3/11/09 9:20a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/nexus/graphics2d/still_decoder.c $
 * 
 * Hydra_Software_Devel/6   3/11/09 9:20a erickson
 * PR53076: destripe to RGB is now supported
 *
 * Hydra_Software_Devel/5   3/10/09 2:36p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/3   10/23/08 3:59p erickson
 * PR42679: update
 *
 * Hydra_Software_Devel/2   9/8/08 4:11p erickson
 * PR46430: test NEXUS_Graphics2D_DestripeToSurface
 *
 * Hydra_Software_Devel/1   6/24/08 8:39a erickson
 * PR43693: add sw ycrcb to rgb conversion app for comparison with hw
 *
 * 6   6/23/08 4:36p erickson
 * PR43693: fix warning
 *
 * 5   6/23/08 4:33p erickson
 * PR43693: adapt to 3548/3556
 *
 * 4   3/10/08 3:43p erickson
 * PR40307: added -avc param
 *
 * 3   2/26/08 2:42p erickson
 * PR39786: added Stop
 *
 * 2   2/25/08 3:04p erickson
 * PR39786: update
 *
 * 1   2/21/08 1:56p erickson
 * PR39786: add still_decoder example
 *
 *****************************************************************************/
#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_core_utils.h"
#include "nexus_core_utils.h"
#if NEXUS_DTV_PLATFORM
#include "nexus_panel_output.h"
#include "nexus_platform_boardcfg.h"
#else
#include "nexus_component_output.h"
#endif
#include "nexus_video_decoder.h"
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

#if NEXUS_DTV_PLATFORM
#include "nexus_platform_boardcfg.h"
#endif
extern bool is_panel_output(void); /* examples_lib */

void still_picture_ready(void *context, int unused)
{
    BSTD_UNUSED(unused);
    printf("still_picture_ready\n");
    BKNI_SetEvent((BKNI_EventHandle)context);
}

/**
SW YCrCb to RGB conversion. Use this to compare with GRC conversion.
TODO: this algo works, but not perfectly. some values need to be clamped. We see some red artifacts.
**/
void ycrcb_to_rgb(uint32_t ycrcb_pixel, uint32_t *rgb_pixel1, uint32_t *rgb_pixel2)
{
    uint8_t y,u,v;
    uint8_t r,g,b;

    y = (ycrcb_pixel >> 24);
    u = (ycrcb_pixel >> 16) & 0xFF;
    v = (ycrcb_pixel) & 0xFF;

    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));

    *rgb_pixel1 = 0xFF000000 | (r<<16) | (g<<8) | b;

    y = (ycrcb_pixel >> 8) & 0xFF;

    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));

    *rgb_pixel2 = 0xFF000000 | (r<<16) | (g<<8) | b;
}

int main(int argc, char **argv)
{
    NEXUS_SurfaceHandle framebufferSurface;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_DisplayHandle display;
    NEXUS_Graphics2DHandle blitter;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoFormatInfo videoFormatInfo;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_StillDecoderHandle stillDecoder;
    NEXUS_PlaypumpHandle playpump;
    NEXUS_PlaypumpSettings playpumpSettings;
    BKNI_EventHandle event;
    NEXUS_Graphics2DFillSettings fillSettings;
    int i;
    NEXUS_Error rc;
    NEXUS_VideoCodec codec = NEXUS_VideoCodec_eMpeg2;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&event);

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
    blitter = NEXUS_Graphics2D_Open(0, NULL);

    /* configure transport and still decoder */
    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);
    NEXUS_Playpump_GetSettings(playpump, &playpumpSettings);
    playpumpSettings.transportType = NEXUS_TransportType_eEs;
    NEXUS_Playpump_SetSettings(playpump, &playpumpSettings);
    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    stillDecoder = NEXUS_StillDecoder_Open(videoDecoder, 0, NULL);

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
    fillSettings.color = 0xFF000000; /* black */
    rc = NEXUS_Graphics2D_Fill(blitter, &fillSettings);
    assert(!rc);

    /* force full screen framebuffer. no horizontal upscale. */
    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.clip.width = createSettings.width;
    graphicsSettings.clip.height = createSettings.height;
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(display, framebufferSurface);

    /* decode and display each still on the command line */
    for (i=1;i<argc;i++) {
        NEXUS_Graphics2DBlitSettings blitSettings;
        NEXUS_StillDecoderStartSettings stillDecoderSettings;
        NEXUS_StripedSurfaceHandle stripedSurface;
        NEXUS_SurfaceHandle stillSurface = NULL;
        FILE *file;
        unsigned feed_zeros = 256;
        bool feed_file = true;

        if (!strcmp(argv[i], "-avc")) {
            codec = NEXUS_VideoCodec_eH264;
            continue;
        }
        else if (!strcmp(argv[i], "-mpeg")) {
            codec = NEXUS_VideoCodec_eMpeg2;
            continue;
        }
        else if (!strcmp(argv[i], "-vc1")) {
            codec = NEXUS_VideoCodec_eVc1;
            continue;
        }

        printf("Decoding %s...\n", argv[i]);
        file = fopen(argv[i], "r");
        if (!file) {
            fprintf(stderr, "can't open file:%s\n", argv[i]);
            continue;
        }

        BKNI_ResetEvent(event);

        NEXUS_StillDecoder_GetDefaultStartSettings(&stillDecoderSettings);
        stillDecoderSettings.pidChannel = NEXUS_Playpump_OpenPidChannel(playpump, 0x1, NULL);
        assert(stillDecoderSettings.pidChannel);
        stillDecoderSettings.stillPictureReady.callback = still_picture_ready;
        stillDecoderSettings.stillPictureReady.context = event;
        stillDecoderSettings.codec = codec;
        rc = NEXUS_StillDecoder_Start(stillDecoder, &stillDecoderSettings);
        assert(!rc);

        rc = NEXUS_Playpump_Start(playpump);
        assert(!rc);

        /* feeding 256 bytes of zeros is required to flush the last packet of data through transport */
        while (feed_file || feed_zeros) {
            size_t size;
            void *buffer;

            rc = NEXUS_Playpump_GetBuffer(playpump, &buffer, &size);
            assert(!rc);
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
            assert(!rc);
        }

        /* now we can decode the still */
        rc = BKNI_WaitForEvent(event, 3000);
        if (rc) {
            printf("Unable to decode still\n");
            goto done;
        }

        rc = NEXUS_StillDecoder_GetStripedSurface(stillDecoder, &stripedSurface);
        assert(!rc);

#if 0
        /* have destripe allocate a surface */
        stillSurface = NEXUS_Graphics2D_Destripe(blitter, stripedSurface);
        assert(stillSurface);
#else
        /* create a large surface and destripe into it */
        NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
        createSettings.width = 1920; /* larger than we need */
        createSettings.height = 1080;
        createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8; /* conversion to RGB is supported for one-pass destripe */
        stillSurface = NEXUS_Surface_Create(&createSettings);
        assert(stillSurface);
        rc = NEXUS_Graphics2D_DestripeToSurface(blitter, stripedSurface, stillSurface, NULL);
        assert(!rc);
#endif

#if 1
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = stillSurface;
        blitSettings.output.surface = framebufferSurface;
        blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
        blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyConstant; /* YCrCb has no alpha, so we must set 0xFF */
        blitSettings.constantColor = 0xFF000000; /* alpha is opaque */
        rc = NEXUS_Graphics2D_Blit(blitter, &blitSettings);
        assert(!rc);
#else
/* TODO: simplistic algo, no scaling. requires SD-sized still. */
        {
            unsigned x,y;
            NEXUS_SurfaceCreateSettings cs;
            NEXUS_SurfaceMemory mem1, mem2;
            NEXUS_Surface_GetCreateSettings(stillSurface, &cs);
            NEXUS_Surface_GetMemory(stillSurface, &mem1);
            NEXUS_Surface_GetMemory(framebufferSurface, &mem2);
            for (y=0;y<cs.height;y++) {
                for (x=0;x<cs.width;x+=2) {
                    ycrcb_to_rgb(
                        *(uint32_t*)(((uint8_t*)mem1.buffer+(y*mem1.pitch))+x*2),
                        (uint32_t*)(((uint8_t*)mem2.buffer+(y*mem2.pitch))+x*4),
                        (uint32_t*)(((uint8_t*)mem2.buffer+(y*mem2.pitch))+(x+1)*4));
                }
            }
            NEXUS_Surface_Flush(framebufferSurface);
        }
#endif

done:
        NEXUS_StillDecoder_Stop(stillDecoder);
        NEXUS_Playpump_Stop(playpump);

        BKNI_Sleep(10000);

        fclose(file);
        NEXUS_Playpump_ClosePidChannel(playpump, stillDecoderSettings.pidChannel);
        if (stillSurface) {
            NEXUS_Surface_Destroy(stillSurface);
        }
    }

    return 0;
}
