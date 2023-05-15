/***************************************************************************
 *  Copyright (c) 2003-2012, Broadcom Corporation
 *  All Rights Reserved
 *  Confidential Property of Broadcom Corporation
 *
 * THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 * AGREEMENT BETWEEN THE USER AND BROADCOM. YOU HAVE NO RIGHT TO USE OR
 * EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_graphics.c $
 * $brcm_Revision: 64 $
 * $brcm_Date: 5/18/12 5:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_graphics.c $
 * 
 * 64   5/18/12 5:27p mward
 * SW7435-67:  For 7435A0, M2MC1 (NEXUS_Graphics2D(1)) is more reliable.
 * 
 * 63   4/26/12 2:23p jrubio
 * SW7429-1: add NEXUS_HAS_NEXUS_HAS_GFD_VERTICAL_UPSCALE
 * 
 * 62   4/26/12 11:23a jrubio
 * SW7429-1: reverse logic so new chips will not need to be in the long
 * list of BCHP_CHIP's for scaling
 * 
 * 61   4/24/12 6:07p katrep
 * SW7429-1:fixed issue with incorrect scaling
 * 
 * 60   3/29/12 11:00a jessem
 * SW7425-2790: Fixed coverity issue 37108.
 *
 * 59   2/13/12 1:36p jessem
 * SW7425-2390: Fixed coverity issues.
 *
 * 58   12/21/11 3:48p mward
 * SW7435-7:  Support Brutus build on (new, 7435-based) platform 97435.
 *
 * 57   9/7/11 12:48p erickson
 * SW7420-1009: use NEXUS_ANY_ID for NEXUS_Graphics2D_Open, open one
 * blitter per bgraphics_t (which removes some commented assumptions and
 * fixes a dual-M2MC/single-checkpoint latent bug)
 *
 * 56   2/15/11 7:15p xhuang
 * SW7358-34: Add 7358/7552 support
 *
 * 55   1/28/11 4:36p nitinb
 * SW7550-496: Enable CSC for secondary display (GFD1) on 7550
 *
 * 54   12/28/10 3:03p hongtaoz
 * SW7425-49: add 7425 brutus support;
 *
 * 53   12/15/10 11:01a erickson
 * SW7420-941: fix BLST_Q_REMOVE
 *
 * 52   12/15/10 10:15a jrubio
 * SW7344-9: add 7344/7346 support
 *
 * 51   12/6/10 3:21p katrep
 * SW7231-4:add initial support for 7231
 *
 * 50   11/22/10 1:01p spothana
 * SW7420-1078: Use get frame buffer heap index
 *
 * SW7420-1078/1   11/12/10 4:45p spothana
 * SW7420-1078: Use get frame buffer heap index
 *
 * 49   10/26/10 10:20p agin
 * SWNOOS-429:  Changed NO_OS to NO_OS_DIAGS.
 *
 * 48   10/20/10 4:33p nickh
 * SW7422-80: Allow heap settings to be provided for each GFD by the
 * application
 *
 * 47   7/14/10 12:48p erickson
 * SW7405-3787: fix alignment units
 *
 * 46   4/4/10 8:07a agin
 * SWNOOS-402:  Fix SDE compiler error.
 *
 * 45   3/1/10 10:56a erickson
 * SW7550-272: add bgraphics_pixel_format_a8_y8_cb8_cr8
 *
 * 44   2/11/10 2:37p erickson
 * SW7405-3903: use NEXUS_Graphics2D_Checkpoint correctly
 *
 * 43   1/18/10 3:07p erickson
 * SW7405-3787: default settop api surface allocations to 4K alignment to
 * avoid stale data in the RAC
 *
 * 42   12/11/09 4:30p mphillip
 * SW7550-112: Merge 7550 changes to main branch
 *
 * Refsw_7550/6   12/11/09 2:25p rjain
 * SW7550-112: Merging LATEST FROM  main branch.
 *
 * Refsw_7550/5   11/25/09 3:35p chengli
 * SW7550-64: remove compile warning
 *
 * Refsw_7550/4   11/18/09 8:29p chengli
 * SW7550-64 : GFD of 7550 should do the vertical scaling
 *
 * 41   11/10/09 1:42p nickh
 * SW7420-166:  Add support for 3rd display and DUAL_OUTPUT
 *
 * Refsw_7550/3   11/5/09 11:25a nitinb
 * sw7550-64: Initial brutus build for 7550
 *
 * 40   9/30/09 10:38a erickson
 * SW7405-3106: don't overwrite user's bsurface_settings.position for
 * framebuffers. only update the cliprect.
 *
 * 39   8/19/09 11:34a mphillip
 * PR57626: Add zorder support to shim
 *
 * 38   8/14/09 3:10p erickson
 * PR55994: allow for default NULL rect
 *
 * 37   7/10/09 2:41p agin
 * PR56698: Fix graphics test for no-os.
 *
 * 36   6/5/09 2:07p vishk
 * PR 55748: (NEXUS) Clipping problem with bsettop Nexus shim
 *
 * 35   4/17/09 9:25a erickson
 * PR54270: fix non-7420
 *
 * 34   4/16/09 4:42p erickson
 * PR54270: use GFD vertical upscale in 7420 brutus
 *
 * 33   4/2/09 3:22p erickson
 * PR53698: implement some (not all) of the bsurface_blit operations
 *
 * 32   2/26/09 1:26p erickson
 * PR52363: bsurface_get_display should create a single-buffered
 * framebuffer to be backward compatible
 *
 * 31   2/20/09 3:58p erickson
 * PR52363: initialize memory in bgraphics_create_framebuffer
 *
 * 30   1/5/09 10:33a erickson
 * PR49688: downgrade ERR to MSG
 *
 * 29   1/2/09 6:01p katrep
 * PR49688: Updated bgraphics_flip and bgraphics_flip_sync
 *
 * 28   8/21/08 4:15p vishk
 * PR 45550: Invoking graphics results in loss of decode video.
 *
 * 27   8/19/08 4:38p erickson
 * PR45866: add NEXUS_Graphics2DOpenSettings
 *
 * 26   8/5/08 2:59p erickson
 * PR44958: bsurface_create should implicitly create fb if needed
 * (backward compatible)
 *
 * 25   6/26/08 1:35p jgarrett
 * PR 36285: Updating blit to match new api spec
 *
 * 24   6/18/08 10:13p erickson
 * PR36068: don't create a fb on first bgraphics_set call
 *
 * 23   5/5/08 3:50p erickson
 * PR36068: set graphics colorkey
 *
 * 22   4/15/08 4:01p erickson
 * PR41718: don't use FastBlit unless we properly screen out all
 * exceptions
 *
 * 21   4/14/08 3:15p erickson
 * PR41738: use FastBlit when able
 *
 * 20   4/10/08 9:56a erickson
 * PR36068: coverity fixes
 *
 * 19   4/3/08 5:41p jgarrett
 * PR 41312: Revising callback logic
 *
 * 18   3/19/08 9:23p erickson
 * PR36068: convert bgraphics.surfaces to lst
 *
 * 17   2/25/08 9:23a erickson
 * PR36068: impl more functions
 *
 * 16   1/7/08 10:15a erickson
 * PR36068: close graphics
 *
 * 15   12/4/07 10:12a erickson
 * PR36068: fix double-buffered framebuffer support
 *
 * 14   12/3/07 4:58p erickson
 * PR36068: use B_N_GRC to limit number of Graphics2D interfaces opened
 *
 * 13   11/27/07 1:44p erickson
 * PR37590: added BDBG_OBJECT code
 *
 * 12   11/14/07 3:23p erickson
 * PR36068: must flush surfaces before calling NEXUS_Graphics2D fill or
 * blit
 *
 * 11   11/14/07 2:21p erickson
 * PR36068: fix compile error
 *
 * 10   11/14/07 1:28p erickson
 * PR36068: convert to cpu-based blit/fill until Nexus fill problems
 * debugged
 *
 * 9   11/12/07 3:57p erickson
 * PR36068: update
 *
 * 8   11/12/07 2:34p erickson
 * PR36068: update
 *
 * 7   11/2/07 1:25p erickson
 * PR36633: update nexus enums
 *
 * 6   10/19/07 12:26p erickson
 * PR36068: added blitter
 *
 * 5   10/17/07 10:09a erickson
 * PR36068: default and check destination/source width/height
 *
 * 4   10/17/07 9:45a erickson
 * PR36068: there should be one bsurface_settings for framebuffers
 *
 * 3   10/16/07 1:17p erickson
 * PR36068: update
 *
 * 2   10/16/07 12:35p erickson
 * PR36068: brutus up over settop api/nexus
 *
 * 1   10/15/07 2:36p erickson
 * PR36068: initial
 *
 ***************************************************************************/
#include "bsettop_impl.h"

BDBG_MODULE(graphics);

BDBG_OBJECT_ID(bgraphics);
BDBG_OBJECT_ID(bsurface);

struct bgraphics g_graphics[B_N_GRAPHICS];
static int g_refCount=0;
static B_MutexHandle g_checkpointMutex;

#define COPY_RECT(pNexus, pSettop) \
    do { \
        if (pSettop) { \
            (pNexus)->x = (pSettop)->x; \
            (pNexus)->y = (pSettop)->y; \
            (pNexus)->width = (pSettop)->width; \
            (pNexus)->height = (pSettop)->height; \
        } \
        else {(pNexus)->width = (pNexus)->height = 0;} \
    } while (0)

/**
Meta-data for each pixel format.
This structure must be kept up-to-date with order of enum.
**/
struct pixel_format_info {
    bgraphics_pixel_format pixel_format;
    bool rgb;
    bool palette; /* NOTE: All palettes are RGB. */
    bool alpha_per_pixel;
    int bpp; /* bits per pixel */
} g_pixel_format_info[] = {
    {bgraphics_pixel_format_r5_g6_b5, true, false, false, 16},
    {bgraphics_pixel_format_a1_r5_g5_b5, true, false, true, 16},
    {bgraphics_pixel_format_r5_g5_b5_a1, true, false, true, 16},
    {bgraphics_pixel_format_a4_r4_g4_b4, true, false, true, 16},
    {bgraphics_pixel_format_r4_g4_b4_a4, true, false, true, 16},
    {bgraphics_pixel_format_a8_r8_g8_b8, true, false, true, 32},
    {bgraphics_pixel_format_palette8, true, true, false, 8},
    {bgraphics_pixel_format_a8_palette8, true, true, true, 16},
    {bgraphics_pixel_format_a0, false, false, false, 0},
    {bgraphics_pixel_format_a8, true, false, true, 8},
    {bgraphics_pixel_format_y08_cb8_y18_cr8, false, false, false, 32},
    {bgraphics_pixel_format_cr8_y18_cb8_y08, false, false, false, 32},
    {bgraphics_pixel_format_cr8_y18_cb8_y08, false, false, false, 32},
    {bgraphics_pixel_format_video_tunnel, false, false, false, 0},
    {bgraphics_pixel_format_palette2, true, true, false, 2},
    {bgraphics_pixel_format_palette4, true, true, false, 4}
};

#if BCHP_CHIP == 7550
/* Color-Conversion Matrices for 7550. 7550 GFD1 doesn't have
   color conversion capability. */
/*
ITU-R BT.601:
        Y  =  (0.257 * R) + (0.504 * G) + (0.098 * B) + 16
        Cb = -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128
        Cr =  (0.439 * R) - (0.368 * G) - (0.071 * B) + 128
*/
static int32_t g_ai32_Matrix_RGBtoYCbCr_BT_601[20] =
{
    (int32_t) ( 0.257f * (1 << 8)),   /*  R factor for Y */
    (int32_t) ( 0.504f * (1 << 8)),   /*  G factor for Y */
    (int32_t) ( 0.098f * (1 << 8)),   /*  B factor for Y */
    (int32_t) 0,                      /*  A factor for Y */
    (int32_t) (16 * (1 << 8)),        /* Increment for Y */
    (int32_t) (-0.148f * (1 << 8)),   /*  R factor for Cb */
    (int32_t) (-0.291f * (1 << 8)),   /*  G factor for Cb */
    (int32_t) ( 0.439f * (1 << 8)),   /*  B factor for Cb */
    (int32_t) 0,                      /*  A factor for Cb */
    (int32_t) (128 * (1 << 8)),      /* Increment for Cb */
    (int32_t) ( 0.439f * (1 << 8)),   /*  R factor for Cr */
    (int32_t) (-0.368f * (1 << 8)),   /*  G factor for Cr */
    (int32_t) (-0.071f * (1 << 8)),   /*  B factor for Cr */
    (int32_t) 0,                      /*  A factor for Cr */
    (int32_t) (128 * (1 << 8)),       /* Increment for Cr */
    (int32_t) 0,                      /*  R factor for A */
    (int32_t) 0,                      /*  G factor for A */
    (int32_t) 0,                      /*  B factor for A */
    (int32_t) (1 << 8),               /*  A factor for A */
    (int32_t) 0,                      /* Increment for A */
};

/*
ITU-R BT.709:
        Y  =  (0.183 * R) + (0.614 * G) + (0.062 * B) + 16
        Cb = -(0.101 * R) - (0.339 * G) + (0.439 * B) + 128
        Cr =  (0.439 * R) - (0.339 * G) - (0.040 * B) + 128
*/
static int32_t g_ai32_Matrix_RGBtoYCbCr_BT_709[20] =
{
    (int32_t) ( 0.183f * (1 << 8)),   /*  R factor for Y */
    (int32_t) ( 0.614f * (1 << 8)),   /*  G factor for Y */
    (int32_t) ( 0.062f * (1 << 8)),   /*  B factor for Y */
    (int32_t) 0,                      /*  A factor for Y */
    (int32_t) (16 * (1 << 8)),        /* Increment for Y */
    (int32_t) (-0.101f * (1 << 8)),   /*  R factor for Cb */
    (int32_t) (-0.339f * (1 << 8)),   /*  G factor for Cb */
    (int32_t) ( 0.439f * (1 << 8)),   /*  B factor for Cb */
    (int32_t) 0,                      /*  A factor for Cb */
    (int32_t) (128 * (1 << 8)),       /* Increment for Cb */
    (int32_t) ( 0.439f * (1 << 8)),   /*  R factor for Cr */
    (int32_t) (-0.339f * (1 << 8)),   /*  G factor for Cr */
    (int32_t) (-0.040f * (1 << 8)),   /*  B factor for Cr */
    (int32_t) 0,                      /*  A factor for Cr */
    (int32_t) (128 * (1 << 8)),       /* Increment for Cr */
    (int32_t) 0,                      /*  R factor for A */
    (int32_t) 0,                      /*  G factor for A */
    (int32_t) 0,                      /*  B factor for A */
    (int32_t) (1 << 8),               /*  A factor for A */
    (int32_t) 0,                      /* Increment for A */
};

#endif

#if B_N_GRC
static void bgraphics_p_checkpoint_callback(void *data, int unused)
{
    bgraphics_t graphics = (bgraphics_t)data;
    BSTD_UNUSED(unused);
    BKNI_SetEvent(graphics->gEvent);
}

static void bgraphics_p_checkpoint(bgraphics_t graphics)
{
#ifdef NO_OS_DIAGS
    BSTD_UNUSED(graphics);
    BKNI_Sleep(30);
#else
    BERR_Code errCode;

    b_unlock();
    B_Mutex_Lock(g_checkpointMutex);
    /* Unlock to avoid overflow when UI is busy */
    errCode = NEXUS_Graphics2D_Checkpoint(graphics->graphics2d, NULL);
    if (errCode == NEXUS_GRAPHICS2D_QUEUED) {
        errCode = BKNI_WaitForEvent(graphics->gEvent, 5000);
    }
    B_Mutex_Unlock(g_checkpointMutex);
    b_lock();
    if ( errCode )
    {
        BDBG_ERR(("Graphics Checkpoint Timeout"));
    }
#endif
}
#endif

bgraphics_t bgraphics_open(bobject_t id, bdisplay_t display)
{
    unsigned index = B_ID_GET_INDEX(id);
    bgraphics_t graphics;

    if (index >= B_N_GRAPHICS) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }
    graphics = &g_graphics[index];
    BDBG_OBJECT_SET(graphics, bgraphics);
    graphics->index = index;
    if (graphics->display || !display || display->graphics) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }

#if B_N_GRC
    if (BKNI_CreateEvent(&graphics->gEvent) != BERR_SUCCESS) {
        return NULL;
    }

    #if (BCHP_CHIP == 7435) && (BCHP_VER == BCHP_VER_A0)
    graphics->graphics2d = NEXUS_Graphics2D_Open(1, NULL);
    #else
    graphics->graphics2d = NEXUS_Graphics2D_Open(NEXUS_ANY_ID, NULL);
    #endif
    if (!graphics->graphics2d) {
        BSETTOP_ERROR(berr_not_available);
        return NULL;
    }

    {
        NEXUS_Graphics2DSettings settings;
        NEXUS_Error rc;

        NEXUS_Graphics2D_GetSettings(graphics->graphics2d, &settings);
        settings.checkpointCallback.callback = bgraphics_p_checkpoint_callback;
        settings.checkpointCallback.context = graphics;
        rc = NEXUS_Graphics2D_SetSettings(graphics->graphics2d, &settings);
        if (rc) {
            NEXUS_Graphics2D_Close(graphics->graphics2d);
            graphics->graphics2d = NULL;
            BSETTOP_ERROR(berr_not_available);
            return NULL;
        }
    }
#endif

    graphics->display = display;
    display->graphics = graphics;
    graphics->settings.destination_width = 720;
    graphics->settings.destination_height = 480;
    graphics->settings.source_width = 720;
    graphics->settings.source_height = 480;
    graphics->settings.enabled = true;
    graphics->settings.chromakey_enabled = true;
    BLST_Q_INIT(&graphics->surfaces);

    if ( 0 == g_refCount++ )
    {
        g_checkpointMutex = B_Mutex_Create(NULL);
        BDBG_ASSERT(NULL != g_checkpointMutex);
    }

    return graphics;
}

void bgraphics_close(bgraphics_t graphics)
{
    unsigned i;
    NEXUS_Error rc;
    NEXUS_GraphicsSettings graphicsSettings;

    NEXUS_Display_GetGraphicsSettings(graphics->display->nDisplay, &graphicsSettings);
    graphicsSettings.enabled = false;
    rc = NEXUS_Display_SetGraphicsSettings(graphics->display->nDisplay, &graphicsSettings);
    if (rc)
    {
        rc = BERR_TRACE(rc);
        return;
    }
    graphics->closing = true;

    for (i=0;i<MAX_GRAPHICS_FBS;i++) {
        if (graphics->fb[i]) {
            bsurface_destroy(graphics->fb[i]);
            graphics->fb[i] = NULL;
        }
    }
    graphics->display->graphics = NULL;
    graphics->display = NULL;
#if B_N_GRC
    NEXUS_Graphics2D_Close(graphics->graphics2d);
    BKNI_DestroyEvent(graphics->gEvent);
#endif
    BDBG_OBJECT_UNSET(graphics, bgraphics);

    if ( 0 == --g_refCount )
    {
        B_Mutex_Destroy(g_checkpointMutex);
        g_checkpointMutex = NULL;
    }
}

/**
bgraphics_p_used_framebuffer determines the actual width of the framebuffer
that will be used. This results in horizontal upscaling if needed.

If you must handle all output formats, we recommend that you create a
framebuffer that is 1280x1080. Three main cases result:

case 1) 1080i output
Because integer scales are preferred, the used portion of the framebuffer will
be 960. If you're using a 720x480 offscreen surface, you'll have two
upscales, one with GRC and other with CMP (720->960->1080).

case 2) 480i output
If your framebuffer is 960 wide and destination width is 720, clip = 240 and
there's no upscale.

case 3) 720p output
You will use the entire framebuffer.
If you're using a 720x480 offscreen surface, you'll have one upscale with the
GRC (720->1280->1280).
**/
static void bgraphics_p_used_framebuffer(bgraphics_t g,
    const bgraphics_settings *settings, uint16_t *pWidth, uint16_t *pHeight)
{
    bsurface_t fb = g->fb[g->cur_fb];
    BDBG_ASSERT(fb);

    if (!settings)
        settings = &g->settings;

    if( fb->settings.position.width >= settings->destination_width ) {
        /* No CMP scaling required */
        *pWidth = settings->destination_width;
    }
    else {
        *pWidth = fb->settings.position.width;
    }

#if NEXUS_HAS_NEXUS_HAS_GFD_VERTICAL_UPSCALE 
    *pHeight = g->settings.destination_height;
#else
    if( fb->settings.position.height >= g->settings.destination_height ) {
        /* No CMP scaling required */
        *pHeight = g->settings.destination_height;
    }
    else {
        *pHeight = fb->settings.position.height;
    }
#endif
}

static bresult bgraphics_p_set_framebuffer(bgraphics_t graphics)
{
    NEXUS_Error rc;
    NEXUS_GraphicsSettings graphicsSettings;
    bsurface_t fb;
    unsigned i;

    BDBG_OBJECT_ASSERT(graphics, bgraphics);

    fb = graphics->fb[graphics->cur_fb]; /* the current */
    if (!fb) return BSETTOP_ERROR(berr_not_available);

    bsurface_sync(fb);

    /* only use fb[0] for settings */
    BDBG_OBJECT_ASSERT(graphics->display, bdisplay);
    NEXUS_Display_GetGraphicsSettings(graphics->display->nDisplay, &graphicsSettings);
    graphicsSettings.enabled = graphics->settings.enabled;
    graphicsSettings.alpha = fb->settings.alpha;
    graphicsSettings.position.width = graphics->settings.destination_width;
    graphicsSettings.position.height = graphics->settings.destination_height;
    graphicsSettings.chromakeyEnabled = graphics->settings.chromakey_enabled;
    graphicsSettings.lowerChromakey = graphics->settings.lower_chromakey;
    graphicsSettings.upperChromakey = graphics->settings.upper_chromakey;

    /* calc framebuffer cliprect based on display format */
    bgraphics_p_used_framebuffer(graphics, &graphics->settings, &graphicsSettings.clip.width, &graphicsSettings.clip.height);

    /* apply cliprect update to all framebuffer surfaces */
    for (i=0;i<graphics->num_fbs;i++) {
        graphics->fb[i]->settings.cliprect.width = graphicsSettings.clip.width;
        graphics->fb[i]->settings.cliprect.height = graphicsSettings.clip.height;
    }

    BDBG_MSG(("fb position %dx%d, clip %dx%d, enabled=%d",
        graphicsSettings.position.width, graphicsSettings.position.height,
        graphicsSettings.clip.width, graphicsSettings.clip.height,
        graphics->settings.enabled));

    rc = NEXUS_Display_SetGraphicsSettings(graphics->display->nDisplay, &graphicsSettings);
    if (rc) return BSETTOP_ERROR(berr_external_error);

    rc = NEXUS_Display_SetGraphicsFramebuffer(graphics->display->nDisplay, fb->nSurface);
    if (rc) return BSETTOP_ERROR(berr_external_error);

    return 0;
}

void bgraphics_get(bgraphics_t graphics, bgraphics_settings *settings)
{
    *settings = graphics->settings;
}

bresult bgraphics_set(bgraphics_t graphics, const bgraphics_settings *settings)
{
    BDBG_OBJECT_ASSERT(graphics, bgraphics);
    graphics->settings = *settings;
    if (graphics->fb[0]) {
        bgraphics_p_set_framebuffer(graphics);
        return bgraphics_sync(graphics);
    }
    else {
        return 0;
    }
}

static void bgraphics_p_scale_rect(bgraphics_t g, const bsettop_rect *rect, bsettop_rect *destrect)
{
    uint16_t dest_width, dest_height;

    /* destrect is calculated using graphics_settings dest==>src ratio. This means we
    don't support per-surface scaling. But this is typical for UI's and makes
    display format changes much easier. */

    /* This bit of calculation handles the upscaled framebuffer. It's recalculating
    what was done in bgraphics_set. */
    bgraphics_p_used_framebuffer(g, NULL, &dest_width, &dest_height);

    if (g->settings.source_width && g->settings.source_height) {
        destrect->x = rect->x * dest_width / g->settings.source_width;
        destrect->y = rect->y * g->settings.destination_height / g->settings.source_height;
        destrect->width = rect->width * dest_width / g->settings.source_width;
        destrect->height = rect->height * dest_height / g->settings.source_height;
    }
    else {
        BDBG_ERR(("bgraphics_p_scale_rect failed because of bad source dim %dx%d", g->settings.source_width, g->settings.source_height));
    }
}

#define NEXT_FB_INDEX(g) ((g)->cur_fb+1>=(g)->num_fbs?0:(g)->cur_fb+1)

bresult bgraphics_sync(bgraphics_t g)
{
    bsurface_t surface, fb;
    bresult result = b_ok;

    BDBG_OBJECT_ASSERT(g, bgraphics);

    fb = g->fb[NEXT_FB_INDEX(g)]; /* get next framebuffer */

    if (!fb) {
        return BSETTOP_ERROR(berr_not_available);
    }

    /* flush cached memory for the framebuffer in case an old CPU write is still in cache */
    NEXUS_Surface_Flush(fb->nSurface);

    /* If at least one surface is marked visible and if offscreen_framebuffer is not set, clear the framebuffer.
    After this, we'll redraw all visible surfaces anyway. This takes care of deleting
    and moving visible surfaces and gives a blank slate if we are going to be blending.
    All direct draws to the framebuffer are wiped out. */
    if (!g->settings.offscreen_framebuffer) {
        for (surface = BLST_Q_FIRST(&g->surfaces);surface;surface=BLST_Q_NEXT(surface, link)) {
            if (surface->settings.visible)
                break;
        }
        if (surface) {
            /* Show video through the framebuffer by default. If we don't have alpha per
            pixel and chromakey isn't enabled, you're going to get black. */
            bgraphics_pixel color = g->settings.chromakey_enabled ?
                g->settings.lower_chromakey : 0x00000000;
            bsurface_fill(g->fb[0], &fb->settings.cliprect, color);
        }
    }

    /* Process all visible, non-framebuffer surfaces */
    for (surface = BLST_Q_FIRST(&g->surfaces);surface;surface=BLST_Q_NEXT(surface, link)) {
        if (surface->settings.visible) {
            bsettop_rect destrect, srcrect;

            NEXUS_Surface_Flush(surface->nSurface);

            bgraphics_p_scale_rect(g, &surface->settings.position, &destrect);

            /* take into account cliprect in blit */
            srcrect.x = surface->settings.cliprect.x;
            srcrect.y = surface->settings.cliprect.y;
            srcrect.width = surface->settings.cliprect.width;
            srcrect.height = surface->settings.cliprect.height;
            BDBG_MSG(("bgraphics_sync[%d], %d,%d,%d,%d -> %d,%d,%d,%d",
                g->index,
                srcrect.x,srcrect.y,srcrect.width,srcrect.height,
                destrect.x,destrect.y,destrect.width,destrect.height
                ));

            #if B_N_GRC
            /* blit to the framebuffer */
            if ((surface->settings.alpha < 0xFF) || g->settings.offscreen_framebuffer) {
                /* In this case, we're going to copy surface's alpha channel. We may
                or may not blend with dest depending on constant alpha. */
                result = bsurface_blit(fb, &destrect,
                    BSURFACE_BLEND_WITH_PIXEL1|BSURFACE_SET_DEST_ALPHA_WITH_SRC1_ALPHA,
                    surface, &srcrect,
                    fb, &destrect,
                    surface->settings.alpha, 0);
            }
            else {
                /* In this case, blend surface with dest, both color and alpha channels,
                using surface's alpha channel. */
                result = bsurface_blit(fb, &destrect,
                    BSURFACE_BLEND_WITH_SRC1_ALPHA|BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA,
                    surface, &srcrect,
                    fb, &destrect,
                    0, 0);
            }
            #else
            {
                result = bsurface_copy(fb, &destrect, surface, &srcrect);
            }
            #endif
            if (result) goto done;
        }
    }


#if B_N_GRC
    /* Now wait for all of it to complete */
    bgraphics_p_checkpoint(g);
#endif

done:
    return result;
}

bresult bsurface_sync(bsurface_t surface)
{
    BDBG_OBJECT_ASSERT(surface, bsurface);
    NEXUS_Surface_Flush(surface->nSurface);
    if (surface->is_fb) {
        bgraphics_sync(surface->graphics);
    }
    else {
#if B_N_GRC
        bgraphics_p_checkpoint(surface->graphics);
#endif
    }
    return 0;
}

bresult bgraphics_flip(bgraphics_t graphics)
{
    bresult result;
    bsettop_rect rect;
    bsurface_t fb1, fb2;

    BDBG_OBJECT_ASSERT(graphics, bgraphics);
    /* always an implicit sync, even if not double or triple buffered */
    result = bgraphics_sync(graphics);
    if (result) return result;

    fb1 = graphics->fb[graphics->cur_fb]; /* the current */
    fb2 = graphics->fb[NEXT_FB_INDEX(graphics)]; /* the current */


    rect = fb2->settings.cliprect;
    BDBG_MSG(("fb %p, x %d,y %d, width %d, height %d",
          fb2,
          rect.x,rect.y,rect.width,rect.height
          ));
    /* This will trigger a special case, copying from one buffer to another */
    if (!graphics->settings.offscreen_framebuffer) {
        BDBG_ERR((" apply bsurface_copy"));
        result = bsurface_copy(fb1, &rect, fb2, &rect);
    }
    graphics->cur_fb = NEXT_FB_INDEX(graphics);
    result = bgraphics_p_set_framebuffer(graphics);

    return result;
}

bresult bgraphics_flip_sync(bgraphics_t graphics)
{
    BDBG_OBJECT_ASSERT(graphics, bgraphics);

    return 0;
}

void bsurface_create_settings_init(bsurface_create_settings *create_settings, bgraphics_t graphics)
{
    BDBG_OBJECT_ASSERT(graphics, bgraphics);
    BKNI_Memset(create_settings, 0, sizeof(*create_settings));
    create_settings->pixel_format = bgraphics_pixel_format_r5_g6_b5;
    create_settings->width = 720;
    create_settings->height = 480;
    create_settings->alignment = 12; /* 2^12 = 4K alignment needed for RAC. see nexus_dma.h for detailed description of cache coherency requirements. */
    create_settings->pitch = 0;
}

/* add a surface according to zorder */
static void bgraphics_p_add_surface(bgraphics_t g, bsurface_t surface)
{
    bsurface_t s;
    for (s = BLST_Q_FIRST(&g->surfaces);s;s = BLST_Q_NEXT(s, link)) {
        if (surface->settings.zorder <= s->settings.zorder) {
            BLST_Q_INSERT_BEFORE(&g->surfaces, s, surface, link);
            return;
        }
    }

    /* just put it at the end */
    BLST_Q_INSERT_TAIL(&g->surfaces, surface, link);
}

/* complement of bsurface_p_add_surface */
static void bgraphics_p_remove_surface(bgraphics_t g, bsurface_t surface)
{
    BLST_Q_REMOVE(&g->surfaces, surface, link);
}

static bsurface_t bsurface_p_create(bgraphics_t graphics, const bsurface_create_settings *create_settings, bool is_fb)
{
    bsurface_t surface;
    NEXUS_SurfaceCreateSettings createSettings;
    NEXUS_SurfaceMemory mem;

    BDBG_OBJECT_ASSERT(graphics, bgraphics);
    surface = BKNI_Malloc(sizeof(*surface));
    BKNI_Memset(surface, 0, sizeof(*surface));

    NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
    createSettings.width = create_settings->width;
    createSettings.height = create_settings->height;
    createSettings.alignment = create_settings->alignment;
    createSettings.pitch = create_settings->pitch;
    createSettings.pixelFormat = b_graphicspixelformat2nexus(create_settings->pixel_format);
    createSettings.heap = NEXUS_Platform_GetFramebufferHeap(graphics->display->index);;
    surface->nSurface = NEXUS_Surface_Create(&createSettings);
    if (!surface->nSurface) {
        BSETTOP_ERROR(berr_external_error);
        BKNI_Free(surface);
        return NULL;
    }

    NEXUS_Surface_GetMemory(surface->nSurface, &mem);
    surface->mem.buffer = mem.buffer;
    surface->mem.pitch = mem.pitch;
    surface->graphics = graphics;
    surface->is_fb = is_fb;
    surface->create_settings = *create_settings;
    surface->settings.position.width = create_settings->width;
    surface->settings.position.height = create_settings->height;
    surface->settings.cliprect = surface->settings.position;
    surface->settings.color = 0xFF000000;
    surface->settings.alpha = 0xFF; /* matches on in bsettop_base_video */

    if (!is_fb) {
        /* non-framebuffer surfaces are stored for bgraphics_sync */
        bgraphics_p_add_surface(graphics, surface);
    }

    BDBG_OBJECT_SET(surface, bsurface);
    return surface;
}

bsurface_t bsurface_create(bgraphics_t graphics, const bsurface_create_settings *create_settings)
{
    BDBG_OBJECT_ASSERT(graphics, bgraphics);

    /* Implicitly create a fullscreen framebuffer */
    if (!graphics->fb[0]) {
        bvideo_format_settings vs;
        bsurface_create_settings settings = *create_settings;

        BDBG_WRN(("Implicitly creating framebuffer using first surface settings. "
            "Call bgraphics_create_framebuffer to be explicit."));

        /* make sure that g_pixel_format_info[] has every enum in the correct order */
        BDBG_ASSERT(g_pixel_format_info[create_settings->pixel_format].pixel_format == create_settings->pixel_format);

        if (g_pixel_format_info[create_settings->pixel_format].palette) {
            /* This is because no one's going to want an implicit palette to go
            along with it. */
            BDBG_ERR(("Will not implicitly create palettized framebuffer. "
                "Converting to r5_g6_b5."));
            settings.pixel_format = bgraphics_pixel_format_r5_g6_b5;
        }

        bdisplay_get_video_format_settings(&graphics->display->settings, &vs);
        settings.width = vs.width;
        settings.height = vs.height;
        if (!bgraphics_create_framebuffer(graphics, &settings, false))
            return NULL;
    }

    return bsurface_p_create(graphics, create_settings, false);
}

bsurface_t bgraphics_create_framebuffer(bgraphics_t graphics, const bsurface_create_settings *create_settings, unsigned int back_buffer_count)
{
    unsigned i;

    BDBG_OBJECT_ASSERT(graphics, bgraphics);
    if (graphics->fb[0]) {
        BDBG_WRN(("already created"));
        return graphics->fb[0];
    }

    if (back_buffer_count >= MAX_GRAPHICS_FBS) {
        BSETTOP_ERROR(berr_invalid_parameter);
        back_buffer_count = 2;
    }

    graphics->fb[0] = bsurface_p_create(graphics, create_settings, true);
    if (back_buffer_count > 0) {
        graphics->fb[1] = bsurface_p_create(graphics, create_settings, true);
        if (back_buffer_count > 1) {
            graphics->fb[2] = bsurface_p_create(graphics, create_settings, true);
        }
    }
    graphics->cur_fb = 0;
    graphics->num_fbs = back_buffer_count + 1;

    for (i=0;i<graphics->num_fbs;i++) {
        if (graphics->fb[i]) {
            BKNI_Memset(graphics->fb[i]->mem.buffer, 0, graphics->fb[i]->mem.pitch * create_settings->height);
        }
    }

    bgraphics_p_set_framebuffer(graphics);

    return graphics->fb[0];
}

bsurface_t bsurface_get_display(bgraphics_t graphics, const bsurface_create_settings *create_settings)
{
    /* create a single buffered framebuffer */
    return bgraphics_create_framebuffer(graphics, create_settings, 0);
}

void bsurface_destroy(bsurface_t surface)
{
    BDBG_OBJECT_ASSERT(surface, bsurface);

    if (surface->is_fb && !surface->graphics->closing) {
        BDBG_ERR(("cannot destroy framebuffer"));
        return;
    }

    if (!surface->is_fb) {
        bgraphics_p_remove_surface(surface->graphics, surface);
    }

    if (surface->nSurface) {
        NEXUS_Surface_Destroy(surface->nSurface);
    }
    BDBG_OBJECT_UNSET(surface, bsurface);
    BKNI_Free(surface);
}

bresult bsurface_get_memory(bsurface_t surface, bsurface_memory *memory)
{
    if (surface->is_fb) {
        surface = surface->graphics->fb[NEXT_FB_INDEX(surface->graphics)];
    }
    *memory = surface->mem;
    return 0;
}

bresult bsurface_set(bsurface_t surface, const bsurface_settings *settings)
{
    /* store fb settings in all fbs */
    if (surface->is_fb) {
        unsigned i;
        for (i=0;i<surface->graphics->num_fbs;i++) {
            surface->graphics->fb[i]->settings = *settings;
        }
        /* reset the frame buffer to reapply settings */
        bgraphics_p_set_framebuffer(surface->graphics);
    }
    else {
        bool reorder = surface->settings.zorder != settings->zorder;
        surface->settings = *settings;
        /* remove and readd in order to pick up zorder changes */
        if (reorder) {
            bgraphics_p_remove_surface(surface->graphics, surface);
            bgraphics_p_add_surface(surface->graphics, surface);
        }
    }
    return 0;
}

void bsurface_get(bsurface_t surface, bsurface_settings *settings)
{
    BDBG_OBJECT_ASSERT(surface, bsurface);
    *settings = surface->settings;
}

#if !B_N_GRC
static bresult bsurface_p_cpu_scalecopy(
    bsurface_t destsurface, const bsettop_rect *destrect,
    bsurface_t srcsurface, const bsettop_rect *srcrect)
{
#if B_HAS_SOFT_GFX
    B_SoftGfx_BlitSettings sBlitSettings;
    B_SoftGfxHandle handle = NULL;

    handle = B_SoftGfx_Open(NULL);

    B_SoftGfx_GetDefaultBlitSettings(&sBlitSettings);

    sBlitSettings.srcsurface = srcsurface->nSurface;
    sBlitSettings.outsurface = destsurface->nSurface;

    sBlitSettings.srcrect.x     = (int16_t)srcrect->x;
    sBlitSettings.srcrect.y     = (int16_t)srcrect->y;
    sBlitSettings.srcrect.width = (uint16_t)srcrect->width;
    sBlitSettings.srcrect.height= (uint16_t)srcrect->height;

    sBlitSettings.outx = (int16_t)destrect->x;
    sBlitSettings.outy = (int16_t)destrect->y;

    sBlitSettings.colorOp = B_SoftGfx_BlitColorOp_eCopySource;
    sBlitSettings.alphaOp = B_SoftGfx_BlitAlphaOp_eCopySource;

#if BCHP_CHIP == 7550
    /* Color conversion for surfaces going to GFD1.*/
    if ((destsurface->create_settings.pixel_format == bgraphics_pixel_format_a8_y8_cb8_cr8)
        &&( destsurface->graphics->display->index == 1))
    {
        int i;
        sBlitSettings.conversionMatrixEnabled = true;

        for(i=0; i<20; i++)
        {
            sBlitSettings.conversionMatrix.coeffMatrix[i] =
            (destsurface->graphics->display->settings.format == NEXUS_VideoFormat_ePalD) ?
                g_ai32_Matrix_RGBtoYCbCr_BT_601[i]: g_ai32_Matrix_RGBtoYCbCr_BT_709[i];
        }
        sBlitSettings.conversionMatrix.shift = 8;
    }

#endif

    B_SoftGfx_Blit(handle, &sBlitSettings);

    B_SoftGfx_Close(handle);
    return b_ok;
#else

    int h,w,y,bpp,w_bytes,srch;
    unsigned char *pSrc, *pDst;

    if ( destsurface->create_settings.pixel_format !=
         srcsurface->create_settings.pixel_format )
    {
        BDBG_WRN(("Unable to software copy when pixel formats don't match!"));
        return b_ok;
    }

    /* Set starting memory addresses based upon rectangles */
    pSrc = (unsigned char *)srcsurface->mem.buffer;
    pDst = (unsigned char *)destsurface->mem.buffer;
    pSrc += srcsurface->mem.pitch*srcrect->y;
    pDst += destsurface->mem.pitch*destrect->y;

    /* No horizontal scaling, use the smaller of the two widths */
    if ( srcrect->width > destrect->width )
        w = destrect->width;
    else
        w = srcrect->width;

    /* No vertical downscaling, use smaller of the two heights */
    if ( srcrect->height > destrect->height )
        srch = destrect->height;
    else
        srch = srcrect->height;
    h = destrect->height;

    /* Need to calculate #bytes to copy per scanline */
    bpp = g_pixel_format_info[srcsurface->create_settings.pixel_format].bpp;
    w_bytes = (w*bpp)/8;    /* Not doing partial byte copies for indexed formats */

    /* Add in horizontal offsets */
    pDst += (destrect->x*bpp)/8;
    pSrc += (srcrect->x*bpp)/8;

    /* Simple line-doubling for now... */
    for ( y = 0; y < h; y++ )
    {
        BKNI_Memcpy(pDst, pSrc, w_bytes);
        pDst += destsurface->mem.pitch;

        if ( ((y * srch)/h) !=
             (((y+1) * srch)/h) )
        {
            pSrc += srcsurface->mem.pitch;
        }
    }

    if (!destsurface->settings.disable_automatic_cache_flush) {
        NEXUS_Surface_Flush(destsurface->nSurface);
    }

    return b_ok;
#endif /* B_HAS_SOFT_GFX */
}

#if !B_HAS_SOFT_GFX
static bgraphics_pixel bgraphics_p_make_pixel(bgraphics_pixel_format pixel_format,
    uint32_t a, uint32_t r, uint32_t g, uint32_t b)
{
    switch (pixel_format) {
    case bgraphics_pixel_format_r5_g6_b5:
        return (r&0xf8)<<8 | (g&0xfc)<<3 | (b&0xf8)>>3;
    case bgraphics_pixel_format_a1_r5_g5_b5:
        return (a?1:0)<<15 | (r&0xf8)<<7 | (g&0xf8)<<2 | (b&0xf8)>>3;
    case bgraphics_pixel_format_r5_g5_b5_a1:
        return (r&0xf8)<<8 | (g&0xf8)<<3 | (b&0xf8)>>2 | (a?1:0)<<0;
    case bgraphics_pixel_format_a4_r4_g4_b4:
        return (a&0xf0)<<8 | (r&0xf0)<<4 | (g&0xf0)<<0 | (b&0xf0)>>4;
    case bgraphics_pixel_format_r4_g4_b4_a4:
        return (r&0xf0)<<8 | (g&0xf0)<<4 | (b&0xf0)<<0 | (a&0xf0)>>4;

    case bgraphics_pixel_format_a8_r8_g8_b8:
        return (a)<<24 | (r)<<16 | (g)<<8 | (b)<<0;

    case bgraphics_pixel_format_y08_cb8_y18_cr8:
    case bgraphics_pixel_format_cr8_y18_cb8_y08:
        BDBG_WRN(("not supported yet"));
        return 0;

    case bgraphics_pixel_format_palette8:
    case bgraphics_pixel_format_a8_palette8:
    case bgraphics_pixel_format_palette2:
    case bgraphics_pixel_format_palette4:
    case bgraphics_pixel_format_a0:
    case bgraphics_pixel_format_a8:
    case bgraphics_pixel_format_video_tunnel:
        break;
    }
    return 0; /* no conversion */
}
#endif

static bresult bsurface_p_cpu_fill(bsurface_t surface, const bsettop_rect *rect, bgraphics_pixel pixel)
{
#if B_HAS_SOFT_GFX
    B_SoftGfx_FillSettings sFillSettings;
    B_SoftGfxHandle handle = NULL;

    handle = B_SoftGfx_Open(NULL);

    B_SoftGfx_GetDefaultFillSettings(&sFillSettings);

    sFillSettings.surface = surface->nSurface;
    sFillSettings.color = (NEXUS_Pixel) pixel;
    sFillSettings.rect.x     = (int16_t)rect->x;
    sFillSettings.rect.y     = (int16_t)rect->y;
    sFillSettings.rect.width = (uint16_t)rect->width;
    sFillSettings.rect.height= (uint16_t)rect->height;
    sFillSettings.colorOp = B_SoftGfx_FillOp_eCopy;
    sFillSettings.alphaOp = B_SoftGfx_FillOp_eCopy;

    B_SoftGfx_Fill(handle, &sFillSettings);

    B_SoftGfx_Close(handle);
    return b_ok;
#else

    int h,w,y,bpp,w_bytes;
    unsigned char *pDst;
    unsigned long mask=0;

    /* Set starting memory addresses based upon rectangles */
    pDst = (unsigned char *)surface->mem.buffer;
    pDst += surface->mem.pitch*rect->y;
    h=rect->height;
    w=rect->width;

    switch (surface->create_settings.pixel_format) {
    case bgraphics_pixel_format_palette2:
        pixel = (pixel & 0x3) << 6;
        pixel |= (pixel>>4) | (pixel>>2) | (pixel>>6);
        mask=0xff;
        break;
    case bgraphics_pixel_format_palette4:
        pixel = (pixel & 0xF) << 4;
        pixel |= (pixel>>4);
        mask=0xff;
        break;
    case bgraphics_pixel_format_a8_palette8:
        pixel = ((pixel & 0xFF00) << 16) | (pixel & 0xFF);
        mask=0xff;
        break;
    case bgraphics_pixel_format_a8:
        pixel = (pixel & 0xFF) << 24;
        mask=0xff000000;
        break;
#if 0
/* TODO */
    case bgraphics_pixel_format_y08_cb8_y18_cr8:
        {
        unsigned int aycrcb_pixel;
        NEXUS_ConvertPixel_RGBtoYCbCr(BPXL_eA8_Y8_Cb8_Cr8, BPXL_eA8_R8_G8_B8, pixel, &aycrcb_pixel);
        BDBG_MSG(("convert pixel on fill %08x => %08x", pixel, aycrcb_pixel));
        pixel = aycrcb_pixel;
        mask=0xffffffff;
        }
        break;
#endif
    case bgraphics_pixel_format_r5_g6_b5:         /* 16-bit, no per-pixel alpha */
    case bgraphics_pixel_format_a1_r5_g5_b5:      /* 16-bit */
    case bgraphics_pixel_format_r5_g5_b5_a1:      /* 16-bit */
    case bgraphics_pixel_format_a4_r4_g4_b4:      /* 16-bit */
    case bgraphics_pixel_format_r4_g4_b4_a4:      /* 16-bit */
        mask=0xffff;
        break;
    case bgraphics_pixel_format_a8_r8_g8_b8:      /* 32-bit */
        mask=0xffffffff;
        break;
    default:
        BDBG_ERR(("Unsupported software fill pixel format"));
        return b_ok;
    }

    bpp = g_pixel_format_info[surface->create_settings.pixel_format].bpp;
    w_bytes = (w*bpp)/8;    /* Not doing partial byte copies for indexed formats */

    /* Add in horizontal offset */
    pDst += (rect->x*bpp)/8;

    if ( mask == 0xff )
    {
        /* 8-bit memset */
        for ( y = 0; y < h; y++ )
        {
            BKNI_Memset(pDst, pixel, w_bytes);
            pDst += surface->mem.pitch;
        }
    }
    else if ( mask == 0xffff )
    {
        /* 16-bit copy */
        int x;
        pixel = bgraphics_p_make_pixel(surface->create_settings.pixel_format,
                                pixel>>24,
                                (pixel>>16)&0xff,
                                (pixel>>8)&0xff,
                                pixel&0xff);
        for ( y = 0; y < h; y++ )
        {
            unsigned short *pDst16=(unsigned short *)pDst;
            for (x = 0; x < w; x++ )
            {
                pDst16[x] = (unsigned short)pixel;
            }
            pDst += surface->mem.pitch;
        }
    }
    else
    {
        /* 32-bit copy w/mask */
        int x;
        for ( y = 0; y < h; y++ )
        {
            unsigned long *pDst32=(unsigned long *)pDst;
            for (x = 0; x < w; x++ )
            {
                pDst32[x] = (unsigned long)pixel | (pDst32[x] & ~mask);
            }
            pDst += surface->mem.pitch;
        }
    }
    return b_ok;
#endif /* B_HAS_SOFT_GFX */
}
#endif


bresult bsurface_fill(bsurface_t surface, const bsettop_rect *rect, bgraphics_pixel pixel)
{
    BDBG_OBJECT_ASSERT(surface, bsurface);
    return bsurface_blended_fill( surface, rect, pixel, BSURFACE_FILL_COPY_COLOR );
}

bresult bsurface_copy(bsurface_t destsurface, const bsettop_rect *destrect, bsurface_t srcsurface, const bsettop_rect *srcrect)
{
    BDBG_MSG(("bsurface_copy from %p (%d,%d,%dx%d) to %p (%d,%d,%dx%d)",
        srcsurface, srcrect?srcrect->x:0, srcrect?srcrect->y:0, srcrect?srcrect->width:0, srcrect?srcrect->height:0,
        destsurface, destrect?destrect->x:0, destrect?destrect->y:0, destrect?destrect->width:0, destrect?destrect->height:0));

#if B_N_GRC
    /* This copies the source and sets the destinations alpha channel to opaque. */
    return bsurface_blit(destsurface, destrect,
        BSURFACE_BLEND_WITH_PIXEL1|BSURFACE_SET_DEST_ALPHA_WITH_SRC1_ALPHA,
        srcsurface, srcrect, NULL, NULL, 0xFF, 0xFF);
#else
    if (destsurface->is_fb) {
        destsurface = destsurface->graphics->fb[NEXT_FB_INDEX(destsurface->graphics)];
    }
    if (srcsurface->is_fb) {
        srcsurface = destsurface->graphics->fb[NEXT_FB_INDEX(srcsurface->graphics)];
    }
    bsurface_p_cpu_scalecopy(destsurface, destrect, srcsurface, srcrect);
    if (!destsurface->settings.disable_automatic_cache_flush) {
        NEXUS_Surface_Flush(destsurface->nSurface);
    }
    return 0;
#endif
}

bresult bsurface_blit(bsurface_t destsurface, const bsettop_rect *destrect, uint32_t operation,
 bsurface_t src1, const bsettop_rect *src1_rect, bsurface_t src2, const bsettop_rect *src2_rect, bgraphics_pixel pixel1,
 bgraphics_pixel pixel2)
{
#if B_N_GRC
    bsurface_blit_params params;

    if (!destsurface) {
        return BSETTOP_ERROR(berr_invalid_parameter);
    }

#if 0
/* TODO: in order to use FastBlit, we have to screen out a variety of issues including YCrCb/RGB conversion. */
    if (src2) {
#endif
        bsurface_blit_params_init(&params);
        params.operation = operation;
        params.src1 = src1;
        if (src1_rect) {
            params.src1_rect = *src1_rect;
        }
        params.src2 = src2;
        if ( src2_rect )
        {
            params.src2_rect = *src2_rect;
        }
        params.pixel1 = pixel1;
        params.pixel2 = pixel2;
        return bsurface_extended_blit(destsurface, destrect, &params);
#if 0
    }
    else {
        NEXUS_Rect outputRect, sourceRect;
        NEXUS_Error rc;

        COPY_RECT(&sourceRect, src1_rect);
        COPY_RECT(&outputRect, destrect);

        NEXUS_Surface_Flush(destsurface->nSurface);
        NEXUS_Surface_Flush(src1->nSurface);

        rc = NEXUS_Graphics2D_FastBlit(destsurface->graphics->graphics2d,
            destsurface->nSurface,
            &outputRect,
            src1->nSurface,
            &sourceRect,
            NEXUS_BlitColorOp_eCopySource, /* TODO */
            NEXUS_BlitAlphaOp_eCopySource, /* TODO */
            pixel1);
        if (rc) return BSETTOP_ERROR(rc);

        if (!destsurface->graphics->settings.async_blits) {
            bgraphics_p_checkpoint(destsurface->graphics);
        }

        return 0;
    }
#endif
#else
    BSTD_UNUSED(destsurface);
    BSTD_UNUSED(destrect);
    BSTD_UNUSED(operation);
    BSTD_UNUSED(src1);
    BSTD_UNUSED(src2);
    BSTD_UNUSED(src1_rect);
    BSTD_UNUSED(src2_rect);
    BSTD_UNUSED(pixel1);
    BSTD_UNUSED(pixel2);
    BDBG_WRN(("Dropping software blit request"));
    return 0;
#endif
}


void bsurface_blit_params_init(bsurface_blit_params *params)
{
    BKNI_Memset(params, 0, sizeof(*params));
}

bresult bsurface_extended_blit(bsurface_t destsurface, const bsettop_rect *destrect, const bsurface_blit_params *params)
{
#if B_N_GRC
    NEXUS_Graphics2DBlitSettings blitSettings;
    NEXUS_Error rc;
    bsurface_t src1 = params->src1;

    if (!src1 || !destsurface) {
        return BSETTOP_ERROR(berr_invalid_parameter);
    }
    if (destsurface->is_fb &&
        src1->is_fb &&
        !params->src2 &&
        src1->graphics == destsurface->graphics &&
        params->operation == (BSURFACE_BLEND_WITH_PIXEL1|BSURFACE_SET_DEST_ALPHA_WITH_SRC1_ALPHA))
    {
        /* detect a special case for handling double buffering */
        BDBG_ERR(("double buffer"));
    } else {
        if (destsurface->is_fb) {
            destsurface = destsurface->graphics->fb[NEXT_FB_INDEX(destsurface->graphics)];
        }
        if (src1->is_fb) {
            src1 = src1->graphics->fb[NEXT_FB_INDEX(src1->graphics)];
        }
    }

    NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);

    blitSettings.source.surface = src1->nSurface;
    if ( 0 == params->src1_rect.width && 0 == params->src1_rect.height ) {
        blitSettings.dest.rect.width = params->src1->create_settings.width;
        blitSettings.dest.rect.height = params->src1->create_settings.height;
    }
    else {
        COPY_RECT(&blitSettings.source.rect, &params->src1_rect);
    }
    if (params->src2) {
        blitSettings.dest.surface = params->src2->nSurface;
        if ( 0 == params->src2_rect.width && 0 == params->src2_rect.height ) {
            blitSettings.dest.rect.width = params->src2->create_settings.width;
            blitSettings.dest.rect.height = params->src2->create_settings.height;
        }
        else {
            COPY_RECT(&blitSettings.dest.rect, &params->src2_rect);
        }
    }
    blitSettings.output.surface = destsurface->nSurface;
    COPY_RECT(&blitSettings.output.rect, destrect);

    /* TODO: support every params->operation */
    if (params->operation & BSURFACE_BLEND_WITH_SRC1_ALPHA) {
        blitSettings.colorOp = NEXUS_BlitColorOp_eUseSourceAlpha;
    }
    else if (params->operation & BSURFACE_BLEND_WITH_SRC2_ALPHA) {
        blitSettings.colorOp = NEXUS_BlitColorOp_eUseDestAlpha;
    }
    else if (params->pixel1 != 0xFF) {
        blitSettings.colorOp = NEXUS_BlitColorOp_eUseConstantAlpha;
        blitSettings.constantColor = params->pixel1;
    }
    else {
        blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
    }

    if (params->operation & BSURFACE_SET_DEST_ALPHA_WITH_SRC1_ALPHA) {
        blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopySource;
    }
    else if (params->operation & BSURFACE_SET_DEST_ALPHA_WITH_SRC2_ALPHA) {
        blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyDest;
    }
    else if (params->operation & BSURFACE_SET_DEST_ALPHA_WITH_AVG_ALPHA) {
        blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCombine;
    }
    else {
        blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyConstant;
        blitSettings.constantColor |= params->pixel1 & 0xFF000000;
    }

    NEXUS_Surface_Flush(destsurface->nSurface);
    NEXUS_Surface_Flush(src1->nSurface);
    if (params->src2) {
        NEXUS_Surface_Flush(params->src2->nSurface);
    }

#if BCHP_CHIP == 7550
    /* Color conversion for surfaces going to GFD1.*/
    if ((destsurface->create_settings.pixel_format == bgraphics_pixel_format_a8_y8_cb8_cr8)
        &&( destsurface->graphics->display->index == 1))
    {
        int i;
        blitSettings.conversionMatrixEnabled = true;

        for(i=0; i<20; i++)
        {
            blitSettings.conversionMatrix.coeffMatrix[i] =
            (destsurface->graphics->display->settings.format == NEXUS_VideoFormat_ePalD) ?
                g_ai32_Matrix_RGBtoYCbCr_BT_601[i]: g_ai32_Matrix_RGBtoYCbCr_BT_709[i];
        }
        blitSettings.conversionMatrix.shift = 8;
    }

#endif

    rc = NEXUS_Graphics2D_Blit(destsurface->graphics->graphics2d, &blitSettings);
    if (rc) {
        BKNI_Fail();
        return BSETTOP_ERROR(berr_external_error);
    }

    if (!destsurface->graphics->settings.async_blits) {
        bgraphics_p_checkpoint(destsurface->graphics);
    }
#else
    bsurface_t src1 = params->src1;

    if (!src1 || !destsurface) {
        return BSETTOP_ERROR(berr_invalid_parameter);
    }

    if (destsurface->is_fb) {
        destsurface = destsurface->graphics->fb[NEXT_FB_INDEX(destsurface->graphics)];
    }
    if (src1 && src1->is_fb) {
        src1 = src1->graphics->fb[NEXT_FB_INDEX(src1->graphics)];
    }

    BSTD_UNUSED(destrect);

    BDBG_WRN(("Dropping software blit request"));
#endif

    return b_ok;
}

bresult
bsurface_blended_fill(bsurface_t surface, const bsettop_rect *rect, bgraphics_pixel pixel, uint32_t operation)
{
#if B_N_GRC
    NEXUS_Graphics2DFillSettings fillSettings;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(surface, bsurface);
    if (surface->is_fb) {
        surface = surface->graphics->fb[NEXT_FB_INDEX(surface->graphics)];
    }

    BDBG_MSG(("bsurface_blended_fill %p %d,%d,%dx%d with %#x",
        surface, rect?rect->x:0, rect?rect->y:0, rect?rect->width:0, rect?rect->height:0,
        pixel));
    BSTD_UNUSED(operation);

    NEXUS_Graphics2D_GetDefaultFillSettings(&fillSettings);

    fillSettings.surface = surface->nSurface;
    COPY_RECT(&fillSettings.rect, rect);

    /* TODO */
    fillSettings.colorOp = NEXUS_FillOp_eCopy;
    fillSettings.alphaOp = NEXUS_FillOp_eCopy;
    fillSettings.color = pixel;

    NEXUS_Surface_Flush(surface->nSurface);
    rc = NEXUS_Graphics2D_Fill(surface->graphics->graphics2d, &fillSettings);
    if (rc) return BSETTOP_ERROR(berr_external_error);

    if (!surface->graphics->settings.async_blits) {
        bgraphics_p_checkpoint(surface->graphics);
    }
    return 0;
#else
    BDBG_OBJECT_ASSERT(surface, bsurface);
    if (surface->is_fb) {
        surface = surface->graphics->fb[NEXT_FB_INDEX(surface->graphics)];
    }

    BDBG_MSG(("bsurface_blended_fill %p %d,%d,%dx%d with %#x",
        surface, rect?rect->x:0, rect?rect->y:0, rect?rect->width:0, rect?rect->height:0,
        pixel));
    if( (operation&0xF) == BSURFACE_FILL_COPY_COLOR ) {
        bsurface_p_cpu_fill(surface, rect, pixel);
        if (!surface->settings.disable_automatic_cache_flush) {
            NEXUS_Surface_Flush(surface->nSurface);
        }
        return b_ok;
    }
    else
        return BSETTOP_ERROR(berr_external_error);
#endif
}

bresult bsurface_load_clut(bsurface_t s, bgraphics_palette palette)
{
    NEXUS_SurfaceMemory mem;
    unsigned n;

    NEXUS_Surface_GetMemory(s->nSurface, &mem);
    n = palette.length;
    if (n > mem.numPaletteEntries) {
        n = mem.numPaletteEntries;
    }

    BDBG_ASSERT(sizeof(bgraphics_pixel) == sizeof(NEXUS_Pixel));
    BKNI_Memcpy(mem.palette, palette.palette, n * sizeof(bgraphics_pixel));

    NEXUS_Surface_Flush(s->nSurface);

    return 0;
}
