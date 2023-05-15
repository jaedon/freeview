/***************************************************************************
 *     Copyright (c) 2004-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: showimage.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 12/5/11 12:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/utils/showimage.c $
 * 
 * 7   12/5/11 12:22p erickson
 * SW7405-5560: extend bwin to support no-blend image loading
 * 
 * 6   7/26/11 1:33p erickson
 * SW7420-1992: add surfacecmp support
 * 
 * 5   6/30/11 4:56p erickson
 * SW7405-5356: add required checkpoint
 * 
 * 4   10/17/08 5:40p jtna
 * PR46852: removed sleep
 * 
 * 3   9/12/08 12:15p erickson
 * PR46852: added temp sleep
 * 
 * 2   9/12/08 10:50a erickson
 * PR46295: add hdmi output
 *
 * 1   9/9/08 2:01p erickson
 * PR46295: create nexus variety of showimage
 *
 ****************************************************************/
#include "bwin.h"
#include "nexus_platform.h"
#include "nexus_display.h"
#include "nexus_hdmi_output.h"
#include "nexus_surface.h"
#include "nexus_core_utils.h"
#include "nexus_graphics2d.h"
#if NEXUS_HAS_SURFACECMP
#include "nexus_surfacecmp_client.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

BDBG_MODULE(showimage);

struct {
    const char *name;
    bwin_image_render_mode render_mode;
} g_render_modes[] = {
    {"single", bwin_image_render_mode_single},
    {"stretch", bwin_image_render_mode_stretch},
    {"tile", bwin_image_render_mode_tile},
    {"maximize", bwin_image_render_mode_maximize},
    {"maximize_down", bwin_image_render_mode_maximize_down},
    {"single_load", bwin_image_render_mode_single_load},
    {NULL, 0}
};

void printUsage(void)
{
    printf(
    "Usage: showimage OPTIONS FILE1 [FILE2 ...]\n"
    "  OPTIONS are:\n"
    "    -fmt DISPLAYFORMAT    Values are 480i,480p,720p,1080i\n"
    "    -pixelfmt PIXELFORMAT Values are rgb565,argb1555,argb8888\n"
    "    -loop                 After rendering all pictures, start again.\n"
    );
    printf(
    "    -timeout SECONDS      Number of seconds to display (default 2). 0 for user input. -1 for immediate exit.\n"
    "    -rect X Y WIDTH HEIGHT specify dimensions\n"
    "    -mode {single|stretch|tile|maximize|maximize_down}\n"
#if NEXUS_HAS_SURFACECMP
    "    -client X             Run as nexus client with surfacecmp\n"  
#endif
    "    -render_loops X       Render each image X times (default 1, change for performance testing)\n"
    "    -background X         Background color (0xAARRGGBB)\n"
    );
}

#define MAX_DISPFORMAT 4
struct {
    const char *name;
    NEXUS_VideoFormat format;
} g_dispformat[MAX_DISPFORMAT] = {
    {"480i",NEXUS_VideoFormat_eNtsc},
    {"480p",NEXUS_VideoFormat_e480p},
    {"720p",NEXUS_VideoFormat_e720p},
    {"1080i",NEXUS_VideoFormat_e1080i}
};

#define MAX_PIXELFORMAT 3
struct {
    const char *name;
    NEXUS_PixelFormat format;
    bwin_pixel_format bwin_format;
} g_pixelformat[MAX_PIXELFORMAT] = {
    {"rgb565",  NEXUS_PixelFormat_eR5_G6_B5,    bwin_pixel_format_r5_g6_b5},
    {"argb1555",NEXUS_PixelFormat_eA1_R5_G5_B5, bwin_pixel_format_a1_r5_g5_b5},
    {"argb8888",NEXUS_PixelFormat_eA8_R8_G8_B8, bwin_pixel_format_a8_r8_g8_b8}
};

#if NEXUS_HAS_SURFACECMP
static void complete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    BKNI_SetEvent((BKNI_EventHandle)data);
}
#endif

int main(int argc, char **argv)
{
    int i;
    unsigned render_loops = 1;
    int timeout = 2;
    bool loop = false;
    unsigned initial_curarg;
    bwin_engine_t win_engine;
    bwin_framebuffer_t fb = NULL;
    bwin_framebuffer_settings fb_settings;
    bwin_engine_settings win_engine_settings;
    const char *filename;
    NEXUS_Rect destrect = {0,0,0,0};
    bwin_rect imagerect = {0,0,0,0};
    int render_mode = bwin_image_render_mode_stretch;
    bwin_image_t image;
    bwin_image_settings image_settings;
    int curarg = 1;
    bwin_pixel_format bwin_pformat;
    NEXUS_PixelFormat pixelFormat;
    NEXUS_DisplaySettings displaySettings;
    NEXUS_VideoFormatInfo videoFormatInfo;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_DisplayHandle display;
    NEXUS_GraphicsSettings graphicsSettings;
    NEXUS_SurfaceHandle framebuffer;
    NEXUS_SurfaceHandle offscreen_surface = NULL;
    NEXUS_SurfaceMemory mem;
    NEXUS_SurfaceCreateSettings surfaceCreateSettings;
    NEXUS_Graphics2DHandle gfx;
    NEXUS_Error rc;
    NEXUS_VideoFormat displayFormat;
    NEXUS_Graphics2DFillSettings fillSettings;
    int client_id = -1; /* default to server */
#if NEXUS_HAS_SURFACECMP
    NEXUS_SurfaceClientHandle blit_client;
    BKNI_EventHandle displayedEvent;
#endif
    unsigned backgroundColor = 0xFF000000;

    /* app defaults */
    displayFormat = NEXUS_VideoFormat_eNtsc;
    pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
    bwin_pformat = bwin_pixel_format_a8_r8_g8_b8;

    while (curarg < argc) {
        if (!strcmp(argv[curarg],"-fmt") && curarg+1 < argc) {
            curarg++;
            for (i=0;i<MAX_DISPFORMAT;i++) {
                if (!strcmp(argv[curarg],g_dispformat[i].name)) {
                    displayFormat = g_dispformat[i].format;
                    break;
                }
            }
        }
        else if (!strcmp(argv[curarg],"-pixelfmt") && curarg+1 < argc) {
            curarg++;
            for (i=0;i<MAX_PIXELFORMAT;i++) {
                if (!strcmp(argv[curarg],g_pixelformat[i].name)) {
                    pixelFormat = g_pixelformat[i].format;
                    bwin_pformat = g_pixelformat[i].bwin_format;
                    break;
                }
            }
        }
#if NEXUS_HAS_SURFACECMP
        else if (!strcmp(argv[curarg],"-client") && curarg+1 < argc) {
            curarg++;
            client_id = atoi(argv[curarg]);
        }
#endif
        else if (!strcmp(argv[curarg],"-rect") && curarg+4 < argc) {
            destrect.x = atoi(argv[++curarg]);
            destrect.y = atoi(argv[++curarg]);
            destrect.width = atoi(argv[++curarg]);
            destrect.height = atoi(argv[++curarg]);
        }
        else if (!strcmp(argv[curarg],"-mode") && curarg+1 < argc) {
            ++curarg;
            for (i=0;g_render_modes[i].name;i++) {
                if (!strcmp(argv[curarg], g_render_modes[i].name)) {
                    render_mode = g_render_modes[i].render_mode;
                    break;
                }
            }
        }
        else if (!strcmp(argv[curarg],"-loop")) {
            loop = true;
        }
        else if (!strcmp(argv[curarg],"-render_loops") && curarg+1 < argc) {
            curarg++;
            render_loops = atoi(argv[curarg]);
        }
        else if (!strcmp(argv[curarg],"-timeout") && curarg+1 < argc) {
            timeout = atoi(argv[++curarg]);
        }
        else if (!strcmp(argv[curarg],"-background") && curarg+1 < argc) {
            backgroundColor = strtoul(argv[++curarg], NULL, 0);
        }
        else
            break;
        curarg++;
    }

    if (curarg >= argc) {
        printUsage();
        return -1;
    }

    /* Init nexus */
    if (client_id == -1) {
        NEXUS_PlatformSettings settings;
        NEXUS_Platform_GetDefaultSettings(&settings);
        settings.openFrontend = false;
        NEXUS_Platform_Init(&settings);
        NEXUS_Platform_GetConfiguration(&platformConfig);
    
        /* Bring up Nexus display */
        NEXUS_Display_GetDefaultSettings(&displaySettings);
        displaySettings.format = displayFormat;
        display = NEXUS_Display_Open(0, &displaySettings);
        NEXUS_Display_AddOutput(display, NEXUS_ComponentOutput_GetConnector(platformConfig.outputs.component[0]));
        NEXUS_Display_AddOutput(display, NEXUS_HdmiOutput_GetVideoConnector(platformConfig.outputs.hdmi[0]));
        NEXUS_VideoFormat_GetInfo(displaySettings.format, &videoFormatInfo);
    }
#if NEXUS_HAS_SURFACECMP
    else {
        NEXUS_SurfaceClientSettings client_settings;
        
        NEXUS_Platform_Join();
        blit_client = NEXUS_SurfaceClient_Acquire(client_id);
        if (!blit_client) {
            BDBG_ERR(("NEXUS_SurfaceClient_Acquire %d failed, client_id"));
            return -1;
        }
    
        NEXUS_VideoFormat_GetInfo(NEXUS_VideoFormat_eNtsc, &videoFormatInfo);
        BKNI_CreateEvent(&displayedEvent);
        
        NEXUS_SurfaceClient_GetSettings(blit_client, &client_settings);
        client_settings.displayed.callback = complete;
        client_settings.displayed.context = displayedEvent;
        rc = NEXUS_SurfaceClient_SetSettings(blit_client, &client_settings);
        BDBG_ASSERT(!rc);
    }
#endif

    /* Bring up Nexus graphics */
    gfx = NEXUS_Graphics2D_Open(0, NULL);
    NEXUS_Surface_GetDefaultCreateSettings(&surfaceCreateSettings);
    surfaceCreateSettings.width = videoFormatInfo.width;
    surfaceCreateSettings.height = videoFormatInfo.height;
    surfaceCreateSettings.pixelFormat = pixelFormat;
    framebuffer = NEXUS_Surface_Create(&surfaceCreateSettings);
    
    /* fill the entire framebuffer with black */
    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);
    fillSettings.surface = framebuffer;
    fillSettings.color = backgroundColor;
    rc = NEXUS_Graphics2D_Fill(gfx, &fillSettings);
    BDBG_ASSERT(!rc);
    NEXUS_Graphics2D_Checkpoint(gfx, NULL);
    
    if (client_id == -1) {
        /* show the framebuffer */
        NEXUS_Display_GetGraphicsSettings(display, &graphicsSettings);
        graphicsSettings.enabled = true;
        graphicsSettings.clip.width = videoFormatInfo.width; /* no GFD upscale */
        rc = NEXUS_Display_SetGraphicsSettings(display, &graphicsSettings);
        BDBG_ASSERT(!rc);
        rc = NEXUS_Display_SetGraphicsFramebuffer(display, framebuffer);
        BDBG_ASSERT(!rc);
    }
#if NEXUS_HAS_SURFACECMP
    else {
        rc = NEXUS_SurfaceClient_SetSurface(blit_client, framebuffer);
        BDBG_ASSERT(!rc);
        rc = BKNI_WaitForEvent(displayedEvent, 5000);
        BDBG_ASSERT(!rc);
    }
#endif
    
    /* bring up bwin with a single framebuffer */
    bwin_engine_settings_init(&win_engine_settings);
    win_engine = bwin_open_engine(&win_engine_settings);
    BDBG_ASSERT(win_engine);

    initial_curarg = curarg;
    for (;curarg<argc;curarg++) {
        unsigned i;
        NEXUS_Graphics2DBlitSettings blitSettings;
        
        filename = argv[curarg];
    
        image = bwin_image_load(win_engine, filename);
        if (!image) {
            printf("Unable to load image %s\n", filename);
            goto done;
        }
    
        bwin_image_get_settings(image, &image_settings);
        imagerect.width = image_settings.width;
        imagerect.height = image_settings.height;
    
        /* bwin will render into a surface created to be the same size as the image */
        NEXUS_Surface_GetDefaultCreateSettings(&surfaceCreateSettings);
        surfaceCreateSettings.width = imagerect.width;
        surfaceCreateSettings.height = imagerect.height;
        surfaceCreateSettings.pixelFormat = pixelFormat;
        offscreen_surface = NEXUS_Surface_Create(&surfaceCreateSettings);
        NEXUS_Surface_GetMemory(offscreen_surface, &mem);
    
        bwin_framebuffer_settings_init(&fb_settings);
        fb_settings.width = imagerect.width;
        fb_settings.height = imagerect.height;
        fb_settings.pitch = mem.pitch;
        fb_settings.buffer = mem.buffer;
        fb_settings.pixel_format = bwin_pformat;
        fb = bwin_open_framebuffer(win_engine, &fb_settings);
        BDBG_ASSERT(fb);
    
        /* get the toplevel window */
        bwin_get_framebuffer_settings(fb, &fb_settings);
    
        /* this provides a timestamp on the console for timing */
        BDBG_WRN(("begin bwin processing..."));
    
        if (render_mode != bwin_image_render_mode_single_load) {
            /* if bwin is going to blend, we need to initialize the surface before rendering.
            if bwin will only load (no blend), then we don't. */
            bwin_rect fillrect = {0,0,0,0};
            fillrect.width = imagerect.width;
            fillrect.height = imagerect.height;
            bwin_fill_rect(fb_settings.window, &fillrect, backgroundColor, NULL);
        }
    
        /* make sure dest rect doesn't exceed the framebuffer */
        if ((destrect.x + destrect.width > (int)videoFormatInfo.width) ||
            (destrect.y + destrect.height > (int)videoFormatInfo.height))
        {
            destrect.width = videoFormatInfo.width - destrect.x;
            destrect.height = videoFormatInfo.height - destrect.y;
        }
    
        /* bwin will render unscaled */
        for (i=0;i<render_loops;i++) {
            bwin_image_render(fb_settings.window, image,
                render_mode, &imagerect, NULL, NULL);
        }
        /* this provides a timestamp on the console for timing */
        BDBG_WRN(("end bwin processing"));
    
        /* must flush before blit, otherwise M2MC might copy from invalid memory */
        NEXUS_Surface_Flush(offscreen_surface);
    
        /* blit from offscreen to the framebuffer */
        NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
        blitSettings.source.surface = offscreen_surface;
        blitSettings.output.surface = framebuffer;
        blitSettings.output.rect = destrect;
        blitSettings.dest.surface = framebuffer;
        blitSettings.dest.rect = destrect;
        if (render_mode != bwin_image_render_mode_single_load) {
            /* bwin has already blended, so just copy and scale */
            blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
            blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
        }   
        else {
            /* bwin did not blend. M2MC can now blend. */
            blitSettings.colorOp = NEXUS_BlitColorOp_eUseSourceAlpha;
            blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyDest;
        }
        rc = NEXUS_Graphics2D_Blit(gfx, &blitSettings);
        BDBG_ASSERT(!rc);
        NEXUS_Graphics2D_Checkpoint(gfx, NULL);
        
        if (client_id == -1) {
            /* TODO: double buffer and flip */
        }
#if NEXUS_HAS_SURFACECMP
        else {
            rc = NEXUS_SurfaceClient_UpdateSurface(blit_client, NULL);
            BDBG_ASSERT(!rc);
            rc = BKNI_WaitForEvent(displayedEvent, 5000);
            BDBG_ASSERT(!rc);
        }
#endif    

        if (timeout == 0) {
            printf("press Enter for next\n");
            getchar();
        }
        else if (timeout != -1) {
            printf("sleeping %d second(s)...\n", timeout);
#if 0
            sleep(timeout);
#else
            /* linux scheduler seems to require steady activity. if we sleep for 2 seconds, then it allocates too large of a timeslice
            when we start SW decoding. this leads to glitches in the compositor. 
            so we use BKNI_Delay busy loop. */
            BKNI_Delay(timeout * 1000 * 1000);
#endif
        }
        
        bwin_image_close(image);
        bwin_close_framebuffer(fb);
        if (offscreen_surface) {
            NEXUS_Surface_Destroy(offscreen_surface);
        }
        
        if (loop && curarg+1 == argc) {
            curarg = initial_curarg-1;
        }
    }

done:
    bwin_close_engine(win_engine);

    return 0;
}

