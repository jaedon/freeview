/***************************************************************************
 *     Copyright (c) 2003-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: graphics_multires.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 12/21/05 12:05p $
 *
 * Module Description:
 * This module implements a graphics test that will drive the same virtual
 * framebuffer on two separate displays.  The framebuffer will be scaled
 * independently for each display resolution.  The resolutions used can be
 * controlled via the constants at the top of this file.
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/examples/graphics_multires.c $
 * 
 * 1   12/21/05 12:05p jgarrett
 * PR 18758: Adding multi-resolution graphics example
 *
***************************************************************************/

#include "bsettop.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* Constants */

/* HD graphics output settings */
#define GFX_HD_WIDTH  (960)      /* Graphics horizontal resolution for HD outputs */
#define GFX_HD_HEIGHT (540)      /* Graphics vertical resolution for HD outputs */

/* SD graphics output settings */
#define GFX_SD_WIDTH  (720)      /* Graphics horizontal resolution for HD outputs */
#define GFX_SD_HEIGHT (480)      /* Graphics vertical resolution for HD outputs */

/* The 'Real' UI Graphics Resolution, irrespective of output format */
#define GFX_UI_WIDTH  (960)
#define GFX_UI_HEIGHT (540)

/* The UI graphics format */
#define GFX_UI_FORMAT (bgraphics_pixel_format_a8_r8_g8_b8)

/* The framebuffer graphics formats */
#define GFX_HD_FRAMEBUFFER_FORMAT (bgraphics_pixel_format_a1_r5_g5_b5)
#define GFX_SD_FRAMEBUFFER_FORMAT (bgraphics_pixel_format_a1_r5_g5_b5)

/* Framebuffer HD Resolution -- Must use maximum video resolution vertically, HW horizontally scales up */
#define GFX_HD_FB_WIDTH (GFX_HD_WIDTH)  
#define GFX_HD_FB_HEIGHT (1080)

/* Framebuffer SD Resolution -- Must use maximum video resolution vertically, HW horizontally scales up */
#define GFX_SD_FB_WIDTH (GFX_SD_WIDTH)
#define GFX_SD_FB_HEIGHT (480)

/* The video output resolutions for HD and SD */
#define HD_OUTPUT_MODE (bvideo_format_1080i)  /* HD Display output resolution */
#define SD_OUTPUT_MODE (bvideo_format_ntsc)  /* SD Display output resolution */

/* Preserve Aspect Ratio or not?  */
#define GFX_PRESERVE_ASPECT (true)

/* Local Routines */
static void PromptUser(const char *pszMsg);
static bresult Initialize(void);
static void Finalize(void);
static void DisplayUpdate(void);
static bsurface_t SurfaceCreate(int width, int height);
static void SurfaceDestroy(bsurface_t surface);
static void GradientCreate(bsurface_t surface, int width, int height, unsigned long colorMask, bool isVertical);
static void DisplayCalcAspectRatios(void);

/* Local Variables */
static bdisplay_t g_displayHD;
static bdisplay_t g_displaySD;
static bgraphics_t g_graphicsHD;
static bgraphics_t g_graphicsSD;
static bsurface_t g_framebufferHD;
static bsurface_t g_framebufferSD;
static bgraphics_settings g_graphicsSettingsHD;
static bgraphics_settings g_graphicsSettingsSD;

/* 
 * This surface will only be created on the HD display
 * Do not mark it as visible.
 */
static bsurface_t g_displaySurfaceUI;

/* Framebuffer visible height & width may change with video resolution */
static unsigned g_fbWidthHD, g_fbHeightHD;
static unsigned g_fbWidthSD, g_fbHeightSD;
static bsettop_rect g_fbRectHD;
static bsettop_rect g_fbRectSD;
static bvideo_format_settings g_formatSettingsHD;
static bvideo_format_settings g_formatSettingsSD;

/* Program entry point */
int main(void)
{
    bsurface_t gradientSurface;
    bsurface_t greenSurface;
    bsurface_t blueSurface;
    bsettop_rect srcRect, dstRect;
    int greenHeight, greenWidth;
    int blueHeight, blueWidth;

    /* Initialize everything */
    (void)Initialize();

    /* Create surfaces */
    gradientSurface = SurfaceCreate(GFX_UI_WIDTH, GFX_UI_HEIGHT);
    assert(NULL != gradientSurface);
    /* Fill surface with grayscale gradient */
    GradientCreate(gradientSurface, GFX_UI_WIDTH, GFX_UI_HEIGHT, 0xFFFFFFFFUL, true);

    /* Create the green surface to be 4x3 */
    greenHeight = 220;
    greenWidth = (greenHeight<<2)/3;
    greenSurface = SurfaceCreate(greenWidth, greenHeight);
    assert(NULL != greenSurface);

    GradientCreate(greenSurface, greenWidth, greenHeight, 0xFF00FF00, false);

    /* Create the blue surface to be 16x9 */
    blueWidth = 320;
    blueHeight = (9*blueWidth)>>4;
    blueSurface = SurfaceCreate(blueWidth, blueHeight);
    assert(NULL != blueSurface);

    GradientCreate(blueSurface, blueWidth, blueHeight, 0xFF0000FF, false);

    /* Composite the screen.  Draw the gradient as the background */
    srcRect.x = srcRect.y = dstRect.x = dstRect.y = 0;
    srcRect.width = dstRect.width = GFX_UI_WIDTH;
    srcRect.height = dstRect.height = GFX_UI_HEIGHT;    
    bsurface_copy(g_displaySurfaceUI, &dstRect, gradientSurface, &srcRect);

    DisplayUpdate();
    PromptUser("Full Screen Gradient -- Press enter to continue");

    /* Mix the green rectangle in the upper-left corner */
    srcRect.width = dstRect.width = greenWidth;
    srcRect.height = dstRect.height = greenHeight;
    dstRect.x = 70;
    dstRect.y = 50;
    bsurface_blit(g_displaySurfaceUI, &dstRect, 
                  BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_SRC2_ALPHA, 
                  greenSurface, &srcRect, 
                  g_displaySurfaceUI, &dstRect, 0, 0);

    DisplayUpdate();
    PromptUser("Added green 4x3 rectangle -- Press enter to continue");

    /* Mix the blue rectangle in the lower-right */
    srcRect.width = dstRect.width = blueWidth;
    srcRect.height = dstRect.height = blueHeight;
    dstRect.x = GFX_UI_WIDTH - (70+blueWidth);
    dstRect.y = greenHeight + 60;
    bsurface_blit(g_displaySurfaceUI, &dstRect, 
                  BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_SRC2_ALPHA, 
                  blueSurface, &srcRect, 
                  g_displaySurfaceUI, &dstRect, 0, 0);

    DisplayUpdate();
    PromptUser("Added blue 16x9 rectangle -- Press enter to continue");

    /* Clean up */
    SurfaceDestroy(blueSurface);
    SurfaceDestroy(greenSurface);
    SurfaceDestroy(gradientSurface);

    PromptUser("Complete -- press enter to exit");
    exit(0);
}

/* Initialize the Settop API and required components */
static bresult Initialize(void)
{
    bresult rc;
    bsurface_create_settings surfaceCreateSettings;
    bdisplay_settings        displaySettings;
    const bsettop_rect       uiRect = {0, 0, GFX_UI_WIDTH, GFX_UI_HEIGHT};

    /* Init the settop API */
    rc = bsettop_init(BSETTOP_VERSION);
    if ( rc )
        return rc;

    /* Register the finalize routine to cleanup on exit */
    atexit(Finalize);

    /* Open both displays */
    g_displayHD = bdisplay_open(B_ID(0));
    assert(NULL != g_displayHD);
    g_displaySD = bdisplay_open(B_ID(1));
    assert(NULL != g_displaySD);

    /* Open Both Graphics Compositors */
    g_graphicsHD = bgraphics_open(B_ID(0), g_displayHD);
    assert(NULL != g_graphicsHD);
    g_graphicsSD = bgraphics_open(B_ID(1), g_displaySD);
    assert(NULL != g_graphicsSD);

    /* Create Framebuffers for both resolutions */
    bsurface_create_settings_init(&surfaceCreateSettings, g_graphicsHD);
    surfaceCreateSettings.height = GFX_HD_FB_HEIGHT;
    surfaceCreateSettings.width = GFX_HD_FB_WIDTH;
    surfaceCreateSettings.pixel_format = GFX_HD_FRAMEBUFFER_FORMAT;
    g_framebufferHD = bgraphics_create_framebuffer(g_graphicsHD, &surfaceCreateSettings, true);
    assert(NULL != g_framebufferHD);

    bsurface_create_settings_init(&surfaceCreateSettings, g_graphicsSD);
    surfaceCreateSettings.height = GFX_SD_FB_HEIGHT;
    surfaceCreateSettings.width = GFX_SD_FB_WIDTH;
    surfaceCreateSettings.pixel_format = GFX_SD_FRAMEBUFFER_FORMAT;
    g_framebufferSD = bgraphics_create_framebuffer(g_graphicsSD, &surfaceCreateSettings, true);
    assert(NULL != g_framebufferSD);

    /* Configure both displays and graphics scalers for the correct output mode */
    /* HD Display */
    bdisplay_get(g_displayHD, &displaySettings);
    displaySettings.format = HD_OUTPUT_MODE;
    displaySettings.component = boutput_component_open(B_ID(0));
    displaySettings.dvi = boutput_dvi_open(B_ID(0));
    displaySettings.rf = NULL;
    displaySettings.svideo = NULL;
    displaySettings.composite = NULL;
    bdisplay_set(g_displayHD, &displaySettings);
    bdisplay_get_video_format_settings(&displaySettings, &g_formatSettingsHD);
    /* HD Graphics */
    bgraphics_get(g_graphicsHD, &g_graphicsSettingsHD);
    g_graphicsSettingsHD.destination_height = g_formatSettingsHD.height;
    g_graphicsSettingsHD.destination_width = g_formatSettingsHD.width;
    /* 
       The horizontal upscaler can only convert up, not down.  Must use the
       correct (smallest) width here 
    */
    g_fbWidthHD = g_graphicsSettingsHD.destination_width;
    if ( GFX_HD_WIDTH < g_fbWidthHD )
        g_fbWidthHD = GFX_HD_WIDTH;
    /* 
       The source height should match the output height always when
       directly rendering to the framebuffer.  The hardware does not
       scale vertically
    */
    g_fbHeightHD = g_graphicsSettingsHD.destination_height;
    g_graphicsSettingsHD.source_height = g_fbHeightHD;
    g_graphicsSettingsHD.source_width = g_fbWidthHD;
    g_graphicsSettingsHD.enabled = true;
    g_graphicsSettingsHD.async_blits = true;
    bgraphics_set(g_graphicsHD, &g_graphicsSettingsHD);

    /* SD Display */
    bdisplay_get(g_displaySD, &displaySettings);
    displaySettings.format = SD_OUTPUT_MODE;
    displaySettings.component = NULL;
    displaySettings.dvi = NULL;
    displaySettings.rf = boutput_rf_open(B_ID(0));
    displaySettings.svideo = boutput_svideo_open(B_ID(0));
    displaySettings.composite = boutput_composite_open(B_ID(0));
    bdisplay_set(g_displaySD, &displaySettings);
    bdisplay_get_video_format_settings(&displaySettings, &g_formatSettingsSD);
    /* SD Graphics */
    bgraphics_get(g_graphicsSD, &g_graphicsSettingsSD);
    g_graphicsSettingsSD.destination_height = g_formatSettingsSD.height;
    g_graphicsSettingsSD.destination_width = g_formatSettingsSD.width;
    /* 
       The horizontal upscaler can only convert up, not down.  Must use the
       correct (smallest) width here 
    */
    g_fbWidthSD = g_graphicsSettingsSD.destination_width;
    if ( GFX_SD_WIDTH < g_fbWidthSD )
        g_fbWidthSD = GFX_SD_WIDTH;
    /* 
       The source height should match the output height always when
       directly rendering to the framebuffer.  The hardware does not
       scale vertically
    */
    g_fbHeightSD = g_graphicsSettingsSD.destination_height;
    g_graphicsSettingsSD.source_height = g_fbHeightSD;
    g_graphicsSettingsSD.source_width = g_fbWidthSD;
    g_graphicsSettingsSD.enabled = true;
    g_graphicsSettingsSD.async_blits = true;
    bgraphics_set(g_graphicsSD, &g_graphicsSettingsSD);

    /* Create the UI framebuffer.  This will be copied to each display */
    bsurface_create_settings_init(&surfaceCreateSettings, g_graphicsHD);
    surfaceCreateSettings.height = GFX_UI_HEIGHT;
    surfaceCreateSettings.width = GFX_UI_WIDTH;
    surfaceCreateSettings.pixel_format = GFX_UI_FORMAT;
    g_displaySurfaceUI = bsurface_create(g_graphicsHD, &surfaceCreateSettings);
    assert(NULL != g_displaySurfaceUI);

    /* Fill the UI framebuffer and both destination framebuffers with transparent */
    bsurface_fill(g_displaySurfaceUI, &uiRect, 0x0UL);

    /* Initially, do not preserve aspect.  This will force both displays to become transparent */
    g_fbRectHD.height = g_fbHeightHD;
    g_fbRectHD.width = g_fbWidthHD;
    g_fbRectSD.height = g_fbHeightSD;
    g_fbRectSD.width = g_fbWidthSD;

    /* Update the display -- copies transparent color -> both displays */
    DisplayUpdate();

    /* Now, set the aspect correctly */
    DisplayCalcAspectRatios();

    return rc;
}

/* Cleanup the Settop API and required components */
static void Finalize(void)
{
    /* Free the surfaces */
    if ( g_displaySurfaceUI )
        bsurface_destroy(g_displaySurfaceUI);
    /* Close the compositors */
    if ( g_graphicsHD )
        bgraphics_close(g_graphicsHD);
    if ( g_graphicsSD )
        bgraphics_close(g_graphicsSD);
    /* Close the displays */
    if ( g_displayHD )
        bdisplay_close(g_displayHD);
    if ( g_displaySD )
        bdisplay_close(g_displaySD);
    /* Cleanup the settop API */
    bsettop_uninit();
}

/* Update the graphics displays with new source data.             */
static void DisplayUpdate(void)
{
    static const bsettop_rect srcRect = 
    {
        0, 0,
        GFX_UI_WIDTH, GFX_UI_HEIGHT,
    };

    /* Copy the UI surface to the HD display */
    bsurface_copy(g_framebufferHD, &g_fbRectHD, g_displaySurfaceUI, &srcRect);
    /* Copy the UI surface to the SD display */
    bsurface_copy(g_framebufferSD, &g_fbRectSD, g_displaySurfaceUI, &srcRect);

#if 0
    printf("Updated HD at %dx%d SD at %dx%d\n", g_fbRectHD.width, g_fbRectHD.height,
           g_fbRectSD.width, g_fbRectSD.height);
#endif

    /* Update both displays */
    bgraphics_flip(g_graphicsHD);
    bgraphics_flip(g_graphicsSD);
}

static void DisplayCalcAspectRatios(void)
{
#if GFX_PRESERVE_ASPECT
    if ( g_formatSettingsHD.width > 720 )
    {
        /* HD Output is 16x9 */
    #if GFX_SD_WIDTH >= GFX_UI_WIDTH
        /* UI is 4x3 -- pillarbox on HD, do nothing on SD */
        /* First, compute ideal destination width, ignoring horizontal scaler */
        g_fbRectHD.width = (g_fbWidthSD * g_fbHeightHD)/g_fbHeightSD;
        /* Now, account for horizontal scaling */
        g_fbRectHD.width = (g_fbRectHD.width * g_graphicsSettingsHD.source_width) / g_graphicsSettingsHD.destination_width;
        g_fbRectHD.x = (g_graphicsSettingsHD.source_width - g_fbRectHD.width) >> 1;
        printf("*** Pillarboxing 4x3 UI onto 16x9 HD Output -- target resolution is %dx%d (offset=%d,%d)\n", g_fbRectHD.width, g_fbRectHD.height, g_fbRectHD.x, g_fbRectHD.y);
    #else
        /* Assume 16x9 -- letterbox on SD, do nothing on HD */        
        g_fbRectSD.height = (GFX_UI_HEIGHT * g_fbWidthSD)/GFX_UI_WIDTH;
        g_fbRectSD.y = (g_graphicsSettingsSD.source_height - g_fbRectSD.height) >> 1;
        printf("*** Letterboxing 16x9 UI onto 4x3 SD Output -- target resolution is %dx%d\n", g_fbRectSD.width, g_fbRectSD.height);
    #endif
    }
    else
    {
        /* HD Output is 4x3 */
    #if GFX_SD_WIDTH >= GFX_UI_WIDTH
        /* UI is 4x3 -- Do nothing */
    #else
        /* UI is 16x9 -- letterbox on both outputs */
        g_fbRectHD.height = g_fbRectSD.height = (GFX_UI_HEIGHT * g_fbHeightSD)/GFX_UI_WIDTH;
        g_fbRectHD.y = g_fbRectSD.y = (g_graphicsSettingsSD.source_height - g_fbRectSD.height) >> 1;
        printf("*** Letterboxing 16x9 UI onto 4x3 HD & SD Output -- target resolution is %dx%d\n", g_fbRectSD.width, g_fbRectSD.height);
    #endif
    }
#endif
}

/* Create an invisible surface */
static bsurface_t SurfaceCreate(int width, int height)
{
    bsurface_create_settings surfaceSettings;
    bsurface_create_settings_init(&surfaceSettings, g_graphicsHD);
    surfaceSettings.height = height;
    surfaceSettings.width = width;
    surfaceSettings.pixel_format = GFX_UI_FORMAT;
    return bsurface_create(g_graphicsHD, &surfaceSettings);
}

/* Destroy an invisible surface */
static void SurfaceDestroy(bsurface_t surface)
{
    bsurface_destroy(surface);
}

static void GradientCreate(bsurface_t surface, int width, int height, unsigned long colorMask, bool isVertical)
{
    const unsigned long startColor = 0x10;
    const unsigned long endColor = 0xf0;
    bsettop_rect rect;

    if ( isVertical )
    {
        rect.width = width;
        rect.height = 1;
        rect.x = 0;

        /* Create a vertical grayscale gradient by filling each scanline with a different color */
        for ( rect.y = 0; rect.y < height; rect.y++ )
        {
            unsigned long pixel = 0xFF000000UL; /* Alpha Opaque */
            unsigned long color;

            color = 2*((endColor - startColor) * rect.y)/height;

            if ( color > endColor )
                color = endColor - (color-endColor);

            pixel = pixel | (color) | (color<<16) | (color<<8);
            pixel &= colorMask;

            bsurface_fill(surface, &rect, pixel);
        }
    }
    else
    {
        rect.width = 1;
        rect.height = height;
        rect.y = 0;

        /* Create a horizontal grayscale gradient */
        for ( rect.x = 0; rect.x < (width); rect.x+=rect.width )
        {
            unsigned long pixel = 0xFF000000UL; /* Alpha Opaque */
            unsigned long color;

            color = 2*((endColor - startColor) * rect.x)/width;

            if ( color > endColor )
                color = endColor - (color-endColor);

            pixel = pixel | (color) | (color<<16) | (color<<8);
            pixel &= colorMask;

            bsurface_fill(surface, &rect, pixel);
        }
    }
}

static void PromptUser(const char *pszMsg)
{
    printf("%s\n", pszMsg);
    getchar();
}

