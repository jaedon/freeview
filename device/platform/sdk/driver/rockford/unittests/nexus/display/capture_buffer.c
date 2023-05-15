/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: capture_buffer.c $
* $brcm_Revision: Hydra_Software_Devel/5 $
* $brcm_Date: 10/19/12 9:56a $
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/nexus/display/capture_buffer.c $
* 
* Hydra_Software_Devel/5   10/19/12 9:56a erickson
* SW7435-426: fix capture buffer for stb
* 
* Hydra_Software_Devel/4   10/25/10 5:35p randyjew
* SW7468-6: Modifed for chips that do not have Component.
* 
* Hydra_Software_Devel/3   4/6/09 2:20p erickson
* PR42679: update
*
* Hydra_Software_Devel/2   3/24/09 9:34a erickson
* PR47583: added forceCapture. allow retries if
* NEXUS_VideoWindow_CaptureVideoBuffer returns no surface, which is
* normal for a disconnected input.
*
* Hydra_Software_Devel/1   10/5/08 8:26p erickson
* PR47583: added NEXUS_VideoWindow_CaptureVideoBuffer test
*
***************************************************************************/

#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_video_input.h"
#include "nexus_video_decoder.h"
#include "nexus_video_window.h"
#include "nexus_core_utils.h"
#include "nexus_graphics2d.h"
#include "nexus_parser_band.h"
#include "nexus_stc_channel.h"

#include "bstd.h"
#include "bkni.h"
#include <stdio.h>
#include <stdlib.h>

void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}

/* the following define the input file and its characteristics -- these will vary by input file */
#define TRANSPORT_TYPE NEXUS_TransportType_eTs
#define VIDEO_CODEC NEXUS_VideoCodec_eMpeg2
#define VIDEO_PID 0x11

int main(void)
{
    NEXUS_DisplayHandle display;
    NEXUS_DisplaySettings displayCfg;
    NEXUS_VideoWindowHandle window;
    NEXUS_VideoWindowSettings windowSettings;
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings videoProgram;
    NEXUS_InputBand inputBand;
    NEXUS_ParserBand parserBand;
    NEXUS_ParserBandSettings parserBandSettings;
    NEXUS_StcChannelHandle stcChannel;
    NEXUS_StcChannelSettings stcSettings;
    NEXUS_PidChannelHandle videoPidChannel;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_PlatformSettings platformSettings;
    NEXUS_SurfaceHandle framebuffer, capture_surface;
    NEXUS_SurfaceMemory mem;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_VideoFormatInfo videoInfo;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Graphics2DSettings gfxSettings;
    BKNI_EventHandle checkpointEvent;
    NEXUS_Error rc;
    unsigned loops = 50;

    NEXUS_Platform_GetDefaultSettings(&platformSettings);
    platformSettings.openFrontend = false;
    rc = NEXUS_Platform_Init(&platformSettings);
    if (rc) return -1;
    BKNI_CreateEvent(&checkpointEvent);
    NEXUS_Platform_GetConfiguration(&platformConfig);
    
    NEXUS_Platform_GetStreamerInputBand(0, &inputBand);
    parserBand = NEXUS_ParserBand_e0;
    NEXUS_ParserBand_GetSettings(parserBand, &parserBandSettings);
    parserBandSettings.sourceType = NEXUS_ParserBandSourceType_eInputBand;
    parserBandSettings.sourceTypeSettings.inputBand = inputBand;
    parserBandSettings.transportType = TRANSPORT_TYPE;
    NEXUS_ParserBand_SetSettings(parserBand, &parserBandSettings);
    
    videoPidChannel = NEXUS_PidChannel_Open(parserBand, VIDEO_PID, NULL);
    
    NEXUS_StcChannel_GetDefaultSettings(0, &stcSettings);
    stcSettings.timebase = NEXUS_Timebase_e0;
    stcSettings.mode = NEXUS_StcChannelMode_ePcr; /* live */
    stcSettings.modeSettings.pcr.pidChannel = videoPidChannel; /* PCR happens to be on video pid */
    stcChannel = NEXUS_StcChannel_Open(0, &stcSettings);

    NEXUS_VideoDecoder_GetDefaultStartSettings(&videoProgram);
    videoProgram.codec = VIDEO_CODEC;
    videoProgram.pidChannel = videoPidChannel;
    videoProgram.stcChannel = stcChannel;
    
    NEXUS_Display_GetDefaultSettings(&displayCfg);
    display = NEXUS_Display_Open(0, &displayCfg);
#if NEXUS_NUM_COMPONENT_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
    BDBG_ASSERT(!rc);
#endif
#if NEXUS_NUM_COMPOSITE_OUTPUTS
    rc = NEXUS_Display_AddOutput(display, NEXUS_CompositeOutput_GetConnector(platformConfig.outputs.composite[0]));
    BDBG_ASSERT(!rc);
#endif  
    window = NEXUS_VideoWindow_Open(display, 0);

    /* capture is always used for analog, but forceCapture may be needed for digital sources.
    this is added to show how to do it. */
    NEXUS_VideoWindow_GetSettings(window, &windowSettings);
    windowSettings.forceCapture = true;
    rc = NEXUS_VideoWindow_SetSettings(window, &windowSettings);
    BDBG_ASSERT(!rc);

    videoDecoder = NEXUS_VideoDecoder_Open(0, NULL);
    rc = NEXUS_VideoWindow_AddInput(window, NEXUS_VideoDecoder_GetConnector(videoDecoder));
    BDBG_ASSERT(!rc);

    NEXUS_VideoDecoder_Start(videoDecoder, &videoProgram);
    
    NEXUS_VideoFormat_GetInfo(displayCfg.format, &videoInfo);
    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.width = videoInfo.width;
    createSettings.height = videoInfo.height;
    framebuffer = NEXUS_Surface_Create(&createSettings);
    NEXUS_Surface_GetMemory(framebuffer, &mem);
    BKNI_Memset(mem.buffer, 0, createSettings.height * mem.pitch);

    NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
    graphicsSettings.enabled = true;
    graphicsSettings.clip.width = createSettings.width;
    graphicsSettings.clip.height = createSettings.height;
    rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
    BDBG_ASSERT(!rc);

    rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
    BDBG_ASSERT(!rc);

    gfx = NEXUS_Graphics2D_Open(0, NULL);

    NEXUS_Graphics2D_GetSettings(gfx, &gfxSettings);
    gfxSettings.checkpointCallback.callback = complete;
    gfxSettings.checkpointCallback.context = checkpointEvent;
    NEXUS_Graphics2D_SetSettings(gfx, &gfxSettings);
    
    /* an initial sleep is required to make sure video is flowing. otherwise, the first call to NEXUS_VideoWindow_CaptureVideoBuffer might fail. */
    BKNI_Sleep(2000);

    capture_surface = NULL;
    while (loops--) {
        NEXUS_Rect rect = {100,100,200,200*3/4};
        NEXUS_Graphics2DBlitSettings blitSettings;
        unsigned tries = 50;

        printf("Capturing buffer...\n");

        if (capture_surface) {
            rc = NEXUS_VideoWindow_ReleaseVideoBuffer(window, capture_surface);
            BDBG_ASSERT(!rc);
            capture_surface = NULL;
        }

        while (tries--) {
            capture_surface = NEXUS_VideoWindow_CaptureVideoBuffer(window);
            if (!capture_surface) {
                printf("Unable to get buffer. Do you have composite 0 video input connected?\n");
                BKNI_Sleep(1000);
            }
            else {
                break;
            }
        }
        if (!capture_surface) break;

        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = capture_surface;
        blitSettings.output.surface = framebuffer;
        blitSettings.output.rect = rect;
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        
        rc = NEXUS_Graphics2D_Checkpoint(gfx, NULL); /* require to execute queue */
        if (rc == NEXUS_GRAPHICS2D_QUEUED) {
            rc = BKNI_WaitForEvent(checkpointEvent, BKNI_INFINITE);
        }
        BDBG_ASSERT(!rc);
        
        BKNI_Sleep(2000);
    }

    if (capture_surface) {
        rc = NEXUS_VideoWindow_ReleaseVideoBuffer(window, capture_surface);
        BDBG_ASSERT(!rc);
        capture_surface = NULL;
    }

    /* shutdown */
    NEXUS_Graphics2D_Close(gfx);
    NEXUS_VideoWindow_Close(window);
    NEXUS_VideoInput_Shutdown(NEXUS_VideoDecoder_GetConnector(videoDecoder));
    NEXUS_VideoDecoder_Close(videoDecoder);
    NEXUS_Display_Close(display);
    NEXUS_Surface_Destroy(framebuffer);
    BKNI_DestroyEvent(checkpointEvent);
    NEXUS_Platform_Uninit();

    return 0;
}

