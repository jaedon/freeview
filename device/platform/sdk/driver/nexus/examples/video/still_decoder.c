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
 * $brcm_Workfile: still_decoder.c $
 * $brcm_Revision: 20 $
 * $brcm_Date: 1/26/12 11:23a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/examples/video/still_decoder.c $
 * 
 * 20   1/26/12 11:23a rjlewis
 * SW7425-1136: Added HDMI Support.
 * 
 * 19   10/7/11 11:02a erickson
 * SW7346-486: general app improvements
 * 
 * 18   6/24/11 10:57a erickson
 * SW7231-199: add hdmi output
 * 
 * 17   5/23/11 4:21p erickson
 * SW7405-3671: NEXUS_Graphics2D_Checkpoint is now required
 * 
 * 16   11/29/10 3:10p erickson
 * SW7420-1078: add NEXUS_Platform_GetFramebufferHeap
 * 
 * 15   9/16/10 10:36a erickson
 * SWDEPRECATED-2425: add comment
 *
 * 14   8/10/10 3:34p erickson
 * SW7420-934: rename NEXUS_Playpump_ReadComplete to
 *  NEXUS_Playpump_WriteComplete
 *
 * 13   7/29/10 4:01p erickson
 * SW7208-24: fix composite output
 *
 * 12   3/22/10 2:41a randyjew
 * SW7468-143: Support for boards without component and/or composite
 *
 * 11   2/25/10 10:57a mphillip
 * SW7550-277: Guard video examples for platforms lacking graphics2d
 *
 * 10   8/10/09 5:59p vsilyaev
 * PR 57552: Use checkpoint API to wait for blit to complete
 *
 * 9   2/4/09 3:50p erickson
 * PR51841: refactor
 *
 * 8   8/21/08 5:13p erickson
 * PR45941: added call to NEXUS_StripedSurface_GetStatus
 *
 * 7   8/20/08 4:50p erickson
 * PR45866: update NEXUS_Graphics2D_Open
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
#include "nexus_component_output.h"
#include "nexus_composite_output.h"
#if NEXUS_HAS_HDMI_OUTPUT
#include "nexus_hdmi_output.h"
#endif
#include "nexus_video_decoder.h"
#include "nexus_still_decoder.h"
#include "nexus_surface.h"
#if NEXUS_HAS_GRAPHICS2D
#include "nexus_graphics2d.h"
#endif
#include "nexus_playpump.h"
#include "bstd.h"
#include "bkni.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>

#if NEXUS_HAS_GRAPHICS2D
void still_picture_ready(void *context, int unused)
{
    BSTD_UNUSED(unused);
    printf("still_picture_ready\n");
    BKNI_SetEvent((BKNI_EventHandle)context);
}

void blitter_done(void *context, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)context);
}
#endif

int main(int argc, char **argv)
{
#if NEXUS_HAS_GRAPHICS2D
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
    BKNI_EventHandle stillPictureReadyEvent,checkpointEvent;
    NEXUS_Graphics2DFillSettings fillSettings;
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_HdmiOutputStatus hdmiStatus;
#endif
    int i;
    NEXUS_Error rc;
    NEXUS_VideoCodec codec = NEXUS_VideoCodec_eMpeg2;
    NEXUS_StripedSurfaceStatus stripedSurfaceStatus;
    NEXUS_Graphics2DSettings graphics2DSettings;

    /* Bring up all modules for a platform in a default configuraiton for this platform */
    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    NEXUS_Platform_Init(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);

    BKNI_CreateEvent(&stillPictureReadyEvent);
    BKNI_CreateEvent(&checkpointEvent);

    NEXUS_Display_GetDefaultSettings(&displaySettings);
    displaySettings.displayType = NEXUS_DisplayType_eAuto;
    displaySettings.format = NEXUS_VideoFormat_eNtsc;
    display = NEXUS_Display_Open(0, &displaySettings);

#if NEXUS_NUM_COMPONENT_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
#endif
#if NEXUS_NUM_HDMI_OUTPUTS
    NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
    rc = NEXUS_HdmiOutput_GetStatus(platformConfig.outputs.hdmi[0], &hdmiStatus);
    if ( !rc && hdmiStatus.connected )
    {
        /* If current display format is not supported by monitor, switch to monitor's preferred format. 
           If other connected outputs do not support the preferred format, a harmless error will occur. */
        NEXUS_Display_GetSettings(display, &displaySettings);
        if ( !hdmiStatus.videoFormatSupported[displaySettings.format] ) {
            displaySettings.format = hdmiStatus.preferredVideoFormat;
            NEXUS_Display_SetSettings(display, &displaySettings);
		}
    }
#endif

    blitter = NEXUS_Graphics2D_Open(0, NULL);

    NEXUS_Graphics2D_GetSettings(blitter, &graphics2DSettings);
    graphics2DSettings.checkpointCallback.callback = blitter_done;
    graphics2DSettings.checkpointCallback.context = checkpointEvent;
    NEXUS_Graphics2D_SetSettings(blitter, &graphics2DSettings);

    /* configure transport and still decoder */
    playpump = NEXUS_Playpump_Open(0, NULL);
    assert(playpump);

    /* This simple example is hardcoded for MPEG ES streams. It can be easily converted to other
    ES formats, but cannot be easily converted to extracting from any stream format other than PES and TS.
    See BSEAV/app/thumbnail and BSEAV/lib/thumbnail for a more sophisticated example which extracts from
    a variety of containers like ASF, AVI, MKV and MP4. Extracting from these containers
    requires bmedia_player, which is used by BSEAV/lib/thumbnail/bthumbnail_extractor.c. */
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
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(0);
    framebufferSurface = NEXUS_Surface_Create(&createSettings);

    /* fill black before the first still */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebufferSurface;
    fillSettings.color = 0xFF000000; /* black */
    rc = NEXUS_Graphics2D_Fill(blitter, &fillSettings);
    assert(!rc);
    
    NEXUS_Graphics2D_Checkpoint(blitter, NULL);

    /* force full screen framebuffer. no horizontal upscale. */
    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.clip.width = createSettings.width;
    graphicsSettings.clip.height = createSettings.height;
    graphicsSettings.enabled = true;
    NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    NEXUS_Display_SetGraphicsFramebuffer(display, framebufferSurface);

    /* decode and display each still on the command line */
    for (i=1;i<argc;i++) 
    {
        NEXUS_Graphics2DBlitSettings blitSettings;
        NEXUS_StillDecoderStartSettings stillDecoderSettings;
        NEXUS_StripedSurfaceHandle stripedSurface;
        NEXUS_SurfaceHandle stillSurface = NULL;
        FILE *file = NULL;
        struct stat st;
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

        rc = stat(argv[i], &st);
        if (!rc) {
            if (S_ISDIR(st.st_mode)) {
                fprintf(stderr, "skipping directory %s\n", argv[i]);
                continue;
            }
            file = fopen(argv[i], "r");
        }
        if (!file) {
            fprintf(stderr, "can't open file:%s\n", argv[i]);
            continue;
        }

        printf("decoding %s...\n", argv[i]);
        BKNI_ResetEvent(stillPictureReadyEvent);

        NEXUS_StillDecoder_GetDefaultStartSettings(&stillDecoderSettings);
        stillDecoderSettings.pidChannel = NEXUS_Playpump_OpenPidChannel(playpump, 0x1, NULL);
        assert(stillDecoderSettings.pidChannel);
        stillDecoderSettings.stillPictureReady.callback = still_picture_ready;
        stillDecoderSettings.stillPictureReady.context = stillPictureReadyEvent;
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
            rc = NEXUS_Playpump_WriteComplete(playpump, 0, size);
            assert(!rc);
        }

        /* now we can decode the still */
        rc = BKNI_WaitForEvent(stillPictureReadyEvent, 3000);
        if (rc) {
            printf("Unable to decode still\n");
            goto done;
        }

        rc = NEXUS_StillDecoder_GetStripedSurface(stillDecoder, &stripedSurface);
        assert(!rc);

        NEXUS_StripedSurface_GetStatus(stripedSurface, &stripedSurfaceStatus);
        printf("surface will be %dx%d\n", stripedSurfaceStatus.width, stripedSurfaceStatus.height);

        stillSurface = NEXUS_Graphics2D_Destripe(blitter, stripedSurface);
        assert(stillSurface);

        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = stillSurface;
        blitSettings.output.surface = framebufferSurface;
        blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
        blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyConstant; /* YCrCb has no alpha, so we must set 0xFF */
        blitSettings.constantColor = 0xFF000000; /* alpha is opaque */
        rc = NEXUS_Graphics2D_Blit(blitter, &blitSettings);
        assert(!rc);
        rc = NEXUS_Graphics2D_Checkpoint(blitter, NULL);
        assert(rc==NEXUS_SUCCESS || rc==NEXUS_GRAPHICS2D_QUEUED);
        if (rc == NEXUS_GRAPHICS2D_QUEUED) {
            rc = BKNI_WaitForEvent(checkpointEvent, BKNI_INFINITE);
            assert(!rc);
        }
        
done:
        NEXUS_StillDecoder_Stop(stillDecoder);
        NEXUS_Playpump_Stop(playpump);

        printf("Press ENTER for next still\n");
        getchar();

        fclose(file);
        NEXUS_Playpump_ClosePidChannel(playpump, stillDecoderSettings.pidChannel);
        if (stillSurface) {
            NEXUS_Surface_Destroy(stillSurface);
        }
    }
    
    NEXUS_StillDecoder_Close(stillDecoder);
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_Playpump_Close(playpump);
    NEXUS_Display_Close(display);
    NEXUS_Graphics2D_Close(blitter);
    BKNI_DestroyEvent(stillPictureReadyEvent);
    BKNI_DestroyEvent(checkpointEvent);
    NEXUS_Surface_Destroy(framebufferSurface);
    NEXUS_Platform_Uninit();
#else
    BSTD_UNUSED(argc);
    BSTD_UNUSED(argv);
	printf("This application is not supported on this platform!\n");
#endif

    return 0;
}
