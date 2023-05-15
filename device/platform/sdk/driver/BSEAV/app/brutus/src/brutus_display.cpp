/***************************************************************************
 *     Copyright (c) 2004-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brutus_display.cpp $
 * $brcm_Revision: 96 $
 * $brcm_Date: 1/19/12 4:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/app/brutus/src/brutus_display.cpp $
 * 
 * 96   1/19/12 4:09p gmohile
 * SW7425-2213 : Fix S1 standby
 * 
 * 95   12/21/11 3:53p mward
 * SW7435-7:  Support Brutus build on (new, 7435-based) platform 97435.
 * 
 * 94   11/16/11 9:41a erickson
 * SW7425-1747: remove old code
 * 
 * 93   6/3/11 5:39p jtna
 * SW7552-26: add RF to B_TWO_DISPLAY_OUTPUT_ISSUE hack
 * 
 * 92   4/5/11 2:29p jtna
 * SW7425-234: use B_HAS_GFD_VERTICAL_UPSCALER to limit display width
 * 
 * 91   2/11/11 2:46p jrubio
 * SW7346-24: fix composite disappearing.
 * 
 * 90   1/28/11 5:31p jtna
 * SW7405-5119: fix 3DTV top-bottom mode
 * 
 * 89   12/28/10 3:15p hongtaoz
 * SW7425-49: add 7425 support;
 * 
 * 88   11/30/10 2:07p VISHK
 * SW7208-114: "Unable to set HD path display settings" if building with
 * MACROVISION ON
 * 
 * 87   11/22/10 1:01p spothana
 * SW7420-1078: Use get frame buffer heap index
 * 
 * 86   11/16/10 12:16p jtna
 * SW7405-4998: more 3D refactoring
 * 
 * 85   10/21/10 4:56p nickh
 * SW7422-80: Add 7422 support for triple display and graphics heap
 * indices for the 3 displays
 * 
 * 84   8/4/10 7:35p spothana
 * SW7420-886: Correcting the 7410 UMA env variable name.
 * 
 * 83   8/4/10 4:33p spothana
 * SW7420-886: Add support for 97410 UMA mode using MEMC0 only.
 * 
 * 82   7/9/10 12:15p jtna
 * SW7405-3999: warning for unrecognized 3DTV string
 * 
 * 81   5/24/10 11:53a jtna
 * SW7405-4379: disabled 3DTV 2D->3D coversion
 * 
 * 80   5/12/10 5:52p jtna
 * SW7405-3999: added 3DTV auto mode
 * 
 * 79   4/9/10 6:40a erickson
 * SW7405-3625: fix ou znorm, add 7420-specific 3dtv width limitation
 *
 * 78   4/5/10 11:30a erickson
 * SW7405-3625: allow 3DTV sbs memory to be allocated, even if it starts
 * in ou mode
 *
 * 77   3/29/10 12:15p erickson
 * SW7405-3625: remove off-by-one error
 *
 * 76   3/2/10 2:37p erickson
 * SW7550-272: MERGE
 *
 * 75   3/2/10 11:15a erickson
 * SW7405-3625: fill fb with black if there's a 3D znorm
 *
 * 74   3/1/10 10:57a erickson
 * SW7550-272: add graphics options for 7550
 *
 * 73   2/26/10 5:41p erickson
 * SW7405-3625: 3DTV - don't use znorm when in PIG mode
 *
 * 72   1/22/10 2:57p erickson
 * SW7550-210: refactor BCHP_CHIP lists
 *
 * 71   12/26/09 2:31p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 70   12/17/09 5:41p randyjew
 * SW7468-6:Add 7468 Support for HDSD_SINGLE
 *
 * 69   12/9/09 12:55p rjain
 * SW7550-112: Merging 7550 to  main branch.
 *
 * Refsw_7550/3   11/18/09 8:25p chengli
 * SW7550-64 : fix framebuffer size to 480 for 7550
 *
 * 68   11/18/09 3:57p erickson
 * SW7405-3106: don't assume bsurface_copy can handle NULL src_rect. fix
 * 3D TV ou znorm src rect.
 *
 * 67   11/10/09 1:22p nickh
 * SW7420-166:  Add support for 3rd display and DUAL_OUTPUT
 *
 * 66   10/20/09 1:53p erickson
 * SW7405-3106: add support for negative znorm values
 *
 * 65   10/13/09 6:27p jrubio
 * SW7342-48: remove outputs from Display1 before opening display 2
 *
 * 64   10/5/09 10:08a erickson
 * SW7405-3106: fix manual framebuffer composition. use cliprect, not
 * position.
 *
 * 63   10/1/09 12:47p erickson
 * SW7405-3106: add OUTPUT_3D_ZNORM. renamed OUTPUT_3D_MODE values to sbs
 * and ou.
 *
 * 62   9/30/09 10:38a erickson
 * SW7405-3106: add OUTPUT_3D_MODE
 *
 * 61   9/29/09 3:58p erickson
 * SW7405-3106: remove bgraphics_sync. use manual bsurface_blit.
 *
 * 60   5/26/09 9:18a erickson
 * PR44368: remove temp surface code. there was no explanation for why
 * this code was needed. if the reason is discovered in the future,
 * please re-add the code along with an explanation of the problem that's
 * being solved.
 *
 * 59   4/16/09 4:42p erickson
 * PR54270: use GFD vertical upscale in 7420 brutus
 *
 * 58   2/11/09 3:11p erickson
 * PR52032: don't open second display in brutus by default for 3548/3556
 *
 * 57   1/16/09 5:20p mward
 * PR51225:  Disable graphics on init until fb is drawn, also for SD in
 * HDSD_SINGLE.
 *
 * 56   10/20/08 3:49p erickson
 * PR47994: don't set SD outputs for HD displays
 *
 * 55   8/15/08 10:29a erickson
 * PR42789: disable graphics on init until fb is drawn
 *
 * 54   6/13/08 10:57a erickson
 * PR43651: improve ERR message
 *
 * 53   4/7/08 2:44p bandrews
 * PR41434: Fixed
 *
 * 52   3/20/08 12:04p erickson
 * PR40307: cleanup old code
 *
 * 52   3/20/08 11:55a erickson
 * PR40307: cleanup old code
 *
 * 51   3/17/08 3:17p erickson
 * PR40307: remove old, unused code before adding new
 *
 * 50   2/15/08 3:35p gmohile
 * PR 39627 : Added 1080p pass-thru support
 *
 * 49   2/13/08 4:52p tokushig
 * PR39534: added bluetooth remote capabilities to brutus
 *
 * 48   12/18/07 7:51p katrep
 * PR37217: Updated debug info
 *
 * 47   12/4/07 10:12a erickson
 * PR36068: only create 720x576 framebuffer for SD display
 *
 * 46   10/31/07 11:48a katrep
 * PR36600: Enable DUAL OUTPUT mode for 7405. This is required for Allegro
 * testing. This is not a 7405 feature.
 *
 * 45   10/16/07 1:16p erickson
 * PR36068: bad typecast
 *
 *******************************************************/
#include "brutus_app.h"
#include "control.h"
#include <string.h>
#include "bstd.h"
#include "cfgsettings.h"
BDBG_MODULE(brutus_display);

/*
Test options:
#define DONT_DOUBLE_BUFFER 1
#define DRAW_DIRECTLY_TO_FRAMEBUFFER 1

If you draw directly to the framebuffer, you will bypass the M2MC scaling needed to support different display format sizes.
    If your chip only supports GFD horizontal upscale, this means you'll get full width but only 480 height graphics.
*/

#if defined DRAW_DIRECTLY_TO_FRAMEBUFFER && !defined DONT_DOUBLE_BUFFER
/* Brutus doesn't support directly drawing into a double-buffered framebuffer. Brutus assumes it can
make incremental updates to mainsurface. */
#error Invalid Brutus graphics configuration
#endif

#if BCHP_CHIP !=7400
/* TODO: please rename this #define when the underlying cause of this issue is determined. This defaults to True for all platforms  */
#define B_TWO_DISPLAY_OUTPUT_ISSUE 1
#endif

bool isStandby(Brutus * pBrutus)
{    
    Control *control = pBrutus->control();
    
    if(!control) return false;

    if(control->getStandbyState() != brutus_standby_state_eOn) 
	return true;
    else 
	return false;
}

/**
Process sync calls from bwin after painting has completed.
This allows us to blit from brutus's fixed size drawing surface (mainsurface) to the framebuffer(s).
**/
void brutus_fb_sync(bwin_framebuffer_t fb)
{
    bwin_framebuffer_settings settings;
    brutus_fb_data *fb_data;
    bsettop_rect dest_rect;
    bsettop_rect src_rect;
    bsurface_settings surface_settings;
    BrutusDisplay *brutusDisplay;    

    /* get the display from the user data */
    bwin_get_framebuffer_settings(fb, &settings);
    fb_data = (brutus_fb_data *)settings.data;
    BDBG_ASSERT(fb_data);
    brutusDisplay = fb_data->pBrutus->display();
    
    if(isStandby(fb_data->pBrutus)) return;
    
    /* manually blit from the mainsurface to the framebuffer.
    we do not use bgraphics_sync of bsurface_settings.visible = true. Instead, manually blit so that we get the exact
    surface composition that we want. */
#if !defined DRAW_DIRECTLY_TO_FRAMEBUFFER
    bsurface_get(brutusDisplay->actual_framebuffer, &surface_settings);
    dest_rect.x = 0;
    dest_rect.y = 0;
    dest_rect.width = surface_settings.cliprect.width;
    dest_rect.height = surface_settings.cliprect.height;
    bsurface_get(brutusDisplay->mainsurface, &surface_settings);
    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.width = surface_settings.position.width;
    src_rect.height = surface_settings.position.height;
    if (brutusDisplay->s3dTv.output > Brutus3DTVMode_eOff
#if 1 /* disable 2D->3D conversion for current-gen platforms */
        && brutusDisplay->s3dTv.source > Brutus3DTVMode_eOff
#endif
        ) {
        bsettop_rect temp_dest = dest_rect;
        bsettop_rect temp_src = src_rect;
        int znorm = 0;

        if (fb_data->pBrutus->control() && fb_data->pBrutus->control()->currentScreen() == Control::eTV) {
            znorm = brutusDisplay->s3dTv.znorm;
        }

        if (brutusDisplay->s3dTv.znorm) {
            bsurface_fill(brutusDisplay->actual_framebuffer, NULL, 0);
        }

        if (brutusDisplay->s3dTv.output == Brutus3DTVMode_eSBS) {
            temp_dest = dest_rect;
            temp_src = src_rect;
            if (znorm >= 0) {
                temp_dest.x = znorm;
                temp_dest.width = dest_rect.width / 2;
            }
            else {
                temp_src.x = -znorm;
                temp_src.width -= temp_src.x;
                temp_dest.x = 0;
                temp_dest.width = (dest_rect.width-temp_src.x) / 2;
            }
            bsurface_copy(brutusDisplay->actual_framebuffer, &temp_dest, brutusDisplay->mainsurface, &temp_src);

            temp_dest = dest_rect;
            temp_src = src_rect;
            if (znorm >= 0) {
                temp_dest.x = dest_rect.width / 2 - znorm;
                temp_dest.width = dest_rect.width / 2;
            }
            else {
                temp_src.x = 0;
                temp_src.width += znorm;
                temp_dest.x = (dest_rect.width - znorm) / 2;
                temp_dest.width = (dest_rect.width + znorm) / 2;
            }
            bsurface_copy(brutusDisplay->actual_framebuffer, &temp_dest, brutusDisplay->mainsurface, &temp_src);
        }
        else {
            temp_dest = dest_rect;
            temp_src = src_rect;
            if (znorm >= 0) {
                temp_dest.x = znorm;
                temp_src.width -= znorm;
                temp_dest.width -= znorm;
                temp_dest.height = dest_rect.height / 2;
            }
            else {
                temp_src.x = -znorm;
                temp_src.width += znorm;
                temp_dest.width += znorm;
                temp_dest.height = dest_rect.height / 2;
            }
            bsurface_copy(brutusDisplay->actual_framebuffer, &temp_dest, brutusDisplay->mainsurface, &temp_src);

            temp_dest = dest_rect;
            temp_src = src_rect;
            if (znorm >= 0) {
                temp_src.x = znorm;
                temp_src.width -= znorm;
                temp_dest.x = 0;
                temp_dest.width -= znorm;
                temp_dest.y = dest_rect.height / 2;
                temp_dest.height = dest_rect.height / 2;
            }
            else {
                temp_src.width += znorm;
                temp_dest.x = -znorm;
                temp_dest.width += znorm;
                temp_dest.y = dest_rect.height / 2;
                temp_dest.height = dest_rect.height / 2;
            }
            bsurface_copy(brutusDisplay->actual_framebuffer, &temp_dest, brutusDisplay->mainsurface, &temp_src);
        }
    }
    else {
        bsurface_copy(brutusDisplay->actual_framebuffer, &dest_rect, brutusDisplay->mainsurface, &src_rect);
    }
#endif
#if !defined DONT_DOUBLE_BUFFER
    bgraphics_flip(brutusDisplay->graphics);
#endif

    /* if HDSD_SINGLE is true, then we need to blit the fb
    to the other graphics engine as well */
    if (brutusDisplay->graphics2) {
        bsurface_get(brutusDisplay->actual_framebuffer2, &surface_settings);
        dest_rect.x = 0;
        dest_rect.y = 0;
        dest_rect.width = surface_settings.cliprect.width;
        dest_rect.height = surface_settings.cliprect.height;
        bsurface_get(brutusDisplay->mainsurface, &surface_settings);
        src_rect.x = 0;
        src_rect.y = 0;
        src_rect.width = surface_settings.position.width;
        src_rect.height = surface_settings.position.height;

        bsurface_copy(brutusDisplay->actual_framebuffer2, &dest_rect, brutusDisplay->mainsurface, &src_rect);
#if !defined DONT_DOUBLE_BUFFER
        bgraphics_flip(brutusDisplay->graphics2);
#endif
    }

    if (fb_data->pBrutus->control()) {
        fb_data->pBrutus->control()->flipDone();
    }
}

void brutus_fill_rect(bwin_framebuffer_t fb, const bwin_rect *rect,
    uint32_t color, uint32_t generic_color)
{
    bwin_framebuffer_settings settings;
    bsettop_rect surfacerect;
    brutus_fb_data            * fb_data      = NULL;
    Brutus                    * pBrutus      = NULL;    
   
    /* get the display from the user data */
    bwin_get_framebuffer_settings(fb, &settings);
    fb_data = (brutus_fb_data *)settings.data;
    BDBG_ASSERT(fb_data);

    /* get reference to Brutus app */
    if (!fb_data)
    {
        /* cannot copy rect without reference to brutus app */
        BDBG_WRN(("brutus_fill_rect(): cannot copy rect without a reference to Brutus app"));
        return;
    }
    pBrutus = fb_data->pBrutus;
    
    if(isStandby(pBrutus)) return;

    /* translate from viewable area coordinates to framebuffer coordinates */
    surfacerect.x      = rect->x;
    surfacerect.y      = rect->y;
    surfacerect.width = rect->width;
    surfacerect.height = rect->height;

    /* adjust dest rect if it refers to the actual main surface
     * (must adjust for viewable area)
     */
    if (fb_data->surface == pBrutus->display()->mainsurface)
    {
        BDBG_MSG(("drawing to mainsurface so adjust for proper viewing rect"));
        surfacerect.x = rect->x + pBrutus->display()->view_rect.x;
        surfacerect.y = rect->y + pBrutus->display()->view_rect.y;
    }

    BDBG_MSG(("filling surface:"));
    BDBG_MSG(("rect x:%d y:%d w:%d h:%d", rect->x, rect->y, rect->width, rect->height));

    /* Now we can update the graphics engine */
    bsurface_fill(fb_data->surface, &surfacerect, generic_color);
}

void brutus_device_bitmap_create(bwin_framebuffer_t fb, void ** new_buffer_ptr, void ** new_data_ptr)
{
    bwin_framebuffer_settings  settings;
    bsurface_create_settings   surface_create_settings;
    bsurface_memory            new_surface_mem;
    brutus_fb_data           * fb_data_old;
    brutus_fb_data           * fb_data_new;       

    if (!fb)
    {
        return;
    }

    /* get the display from the user data
     * note that this fb_data_old is actually from another framebuffer.  we are
     * only using it so we can have access to the pBrutus pointer.  we will
     * malloc a new version of this fb_data_old to go along with the new bsurface
     * we are creating here.
     */
    bwin_get_framebuffer_settings(fb, &settings);
    fb_data_old = (brutus_fb_data *)settings.data;
    BDBG_ASSERT(fb_data_old);

    if(isStandby(fb_data_old->pBrutus)) return;

    bsurface_create_settings_init(&surface_create_settings,
                                  fb_data_old->pBrutus->display()->graphics);
    surface_create_settings.width  = settings.width;
    surface_create_settings.height = settings.height;

    switch (settings.pixel_format)
    {
#if B_DEFAULT_TO_ARGB8888_PIXEL
    default:
#endif
    case bwin_pixel_format_a8_r8_g8_b8:
        surface_create_settings.pixel_format = bgraphics_pixel_format_a8_r8_g8_b8;
        break;

    case bwin_pixel_format_a1_r5_g5_b5:
        surface_create_settings.pixel_format = bgraphics_pixel_format_a1_r5_g5_b5;
        break;

    case bwin_pixel_format_r5_g6_b5:
#if B_DEFAULT_TO_ARGB8888_PIXEL
#else
    default:
#endif
        surface_create_settings.pixel_format = bgraphics_pixel_format_r5_g6_b5;
        break;
    }

    /* malloc new version of fb_data. (must be new copy since surface pointer
     * is new and we do not want to overwrite the current pointer (which belongs
     * to a different bwin framebuffer
     */
    fb_data_new = (brutus_fb_data *)malloc(sizeof(brutus_fb_data));

    /* copy old fb data but then save the newly created bsurface pointer */
    *fb_data_new         = *fb_data_old;
    fb_data_new->surface = bsurface_create(fb_data_new->pBrutus->display()->graphics,
                                           &surface_create_settings);
    assert(fb_data_new->surface);

    /* save new surface mem and fb data in arg pointers */
    *new_data_ptr = fb_data_new;
    bsurface_get_memory(fb_data_new->surface, &new_surface_mem);
    *new_buffer_ptr = new_surface_mem.buffer;
}

void brutus_device_bitmap_destroy(bwin_framebuffer_t fb)
{
    bwin_framebuffer_settings  settings;
    brutus_fb_data           * fb_data;           

    /* get the display from the user data */
    bwin_get_framebuffer_settings(fb, &settings);
    fb_data = (brutus_fb_data *)settings.data;
    BDBG_ASSERT(fb_data);

    if(isStandby(fb_data->pBrutus)) return;

    if (fb_data)
    {
        if (fb_data->surface)
        {
            bsurface_destroy(fb_data->surface);
        }
        else
        {
            BDBG_ERR(("memory leak: invalid bsurface pointer - cannot destroy it!"));
        }

        free(fb_data);
    }
}

void brutus_copy_rect(bwin_framebuffer_t destfb, const bwin_rect *destrect,
                      bwin_framebuffer_t srcfb, const bwin_rect *srcrect)
{
    bwin_framebuffer_settings dest_fb_settings;
    bwin_framebuffer_settings src_fb_settings;
    bsettop_rect dest_surfacerect;
    bsettop_rect src_surfacerect;
    brutus_fb_data            * src_fb_data          = NULL;
    brutus_fb_data            * dest_fb_data         = NULL;
    Brutus                    * pBrutus              = NULL;
    bsurface_t                  src_surface          = NULL;
    bsurface_t                  dest_surface         = NULL;    

    if (!(srcrect->width && srcrect->height &&
          destrect->width && destrect->height))
    {
        /* dest or src rect has 0 area so do nothing */
        BDBG_WRN(("brutus_copy_rect(): src or dest rect has area = 0 so do nothing!"));
        return;
    }

    /* we will use the hardware to do the actual copying since it does a better
     * (and faster) job with image scaling than we can do in software.  to
     * accomplish this, we will convert the srcfb and destfb to bsurfaces unless
     * they already contain associated bsurfaces.
     */

    /* get the display from the user data */
    bwin_get_framebuffer_settings(destfb, &dest_fb_settings);
    dest_fb_data = (brutus_fb_data *)dest_fb_settings.data;
    BDBG_ASSERT(dest_fb_data);
    bwin_get_framebuffer_settings(srcfb, &src_fb_settings);
    src_fb_data = (brutus_fb_data *)src_fb_settings.data;

    /* get reference to Brutus app */
    if (!dest_fb_data && !src_fb_data)
    {
        /* cannot copy rect without reference to brutus app */
        BDBG_WRN(("brutus_copy_rect(): cannot copy rect without a reference to Brutus app"));
        return;
    }
    else
    if (dest_fb_data)
    {
        BDBG_MSG(("brutus reference found in dest bwin framebuffer"));
        pBrutus = dest_fb_data->pBrutus;
    }
    else
    {
        BDBG_MSG(("brutus reference found in src bwin framebuffer"));
        pBrutus = src_fb_data->pBrutus;
    }
    
    if(isStandby(pBrutus)) return;

    dest_surface            = dest_fb_data->surface;
    dest_surfacerect.x      = destrect->x;
    dest_surfacerect.y      = destrect->y;
    dest_surfacerect.width  = destrect->width;
    dest_surfacerect.height = destrect->height;
    src_surface             = src_fb_data->surface;
    src_surfacerect.x       = srcrect->x;
    src_surfacerect.y       = srcrect->y;
    src_surfacerect.width   = srcrect->width;
    src_surfacerect.height  = srcrect->height;

    /* adjust dest rect if it refers to the actual main surface
     * (must adjust for viewable area)
     */
    if (dest_surface == pBrutus->display()->mainsurface)
    {
        BDBG_MSG(("drawing to mainsurface so adjust for proper viewing rect"));
        dest_surfacerect.x  = destrect->x + pBrutus->display()->view_rect.x;
        dest_surfacerect.y  = destrect->y + pBrutus->display()->view_rect.y;
    }

    bsurface_copy(dest_surface, &dest_surfacerect, src_surface, &src_surfacerect);
}

void brutus_blit(bwin_framebuffer_t destfb, const bwin_rect *destrect,
                 const uint32_t operation,
                 bwin_framebuffer_t srcfb1, const bwin_rect *srcrect1,
                 bwin_framebuffer_t srcfb2, const bwin_rect *srcrect2,
                 const uint32_t pixel1, const uint32_t pixel2)
{
    bwin_framebuffer_settings   dest_fb_settings;
    bwin_framebuffer_settings   src_fb_settings1;
    bwin_framebuffer_settings   src_fb_settings2;
    bsettop_rect                dest_surfacerect;
    bsettop_rect                src_surfacerect1;
    bsettop_rect                src_surfacerect2;

    brutus_fb_data            * dest_fb_data         = NULL;
    brutus_fb_data            * src_fb_data1         = NULL;
    brutus_fb_data            * src_fb_data2         = NULL;

    Brutus                    * pBrutus              = NULL;

    bsurface_t                  src_surface1         = NULL;
    bsurface_t                  src_surface2         = NULL;
    bsurface_t                  dest_surface         = NULL;       

    if (!(srcrect1->width && srcrect1->height &&
          srcrect2->width && srcrect2->height &&
          destrect->width && destrect->height))
    {
        /* dest or src rect has 0 area so do nothing */
        BDBG_WRN(("brutus_blit(): src or dest rect has area = 0 so do nothing!"));
        return;
    }

    /* we will use the hardware to do the actual copying since it does a better
     * (and faster) job with image scaling than we can do in software.  to
     * accomplish this, we will convert the srcfb and destfb to bsurfaces unless
     * they already contain associated bsurfaces.
     */

    /* get the display from the user data */
    bwin_get_framebuffer_settings(destfb, &dest_fb_settings);
    dest_fb_data = (brutus_fb_data *)dest_fb_settings.data;
    BDBG_ASSERT(dest_fb_data);
    bwin_get_framebuffer_settings(srcfb1, &src_fb_settings1);
    src_fb_data1 = (brutus_fb_data *)src_fb_settings1.data;
    bwin_get_framebuffer_settings(srcfb2, &src_fb_settings2);
    src_fb_data2 = (brutus_fb_data *)src_fb_settings2.data;

    /* get reference to Brutus app */
    if (!dest_fb_data && !src_fb_data1 && !src_fb_data2)
    {
        /* cannot copy rect without reference to brutus app */
        BDBG_WRN(("brutus_copy_rect(): cannot copy rect without a reference to Brutus app"));
        return;
    }
    else if (dest_fb_data)
    {
        BDBG_MSG(("brutus reference found in dest bwin framebuffer"));
        pBrutus = dest_fb_data->pBrutus;
    }
    else if (src_fb_data1)
    {
        BDBG_MSG(("brutus reference found in src1 bwin framebuffer"));
        pBrutus = src_fb_data1->pBrutus;
    }
    else if (src_fb_data2)
    {
        BDBG_MSG(("brutus reference found in src2 bwin framebuffer"));
        pBrutus = src_fb_data2->pBrutus;
    }

    if(isStandby(pBrutus)) return;

    dest_surface             = dest_fb_data->surface;
    dest_surfacerect.x       = destrect->x;
    dest_surfacerect.y       = destrect->y;
    dest_surfacerect.width   = destrect->width;
    dest_surfacerect.height  = destrect->height;
    src_surface1             = src_fb_data1->surface;
    src_surfacerect1.x       = srcrect1->x;
    src_surfacerect1.y       = srcrect1->y;
    src_surfacerect1.width   = srcrect1->width;
    src_surfacerect1.height  = srcrect1->height;
    src_surface2             = src_fb_data2->surface;
    src_surfacerect2.x       = srcrect2->x;
    src_surfacerect2.y       = srcrect2->y;
    src_surfacerect2.width   = srcrect2->width;
    src_surfacerect2.height  = srcrect2->height;

    /* adjust dest rect if it refers to the actual main surface
     * (must adjust for viewable area)
     */
    if (dest_surface == pBrutus->display()->mainsurface)
    {
        BDBG_MSG(("drawing to mainsurface so adjust for proper viewing rect"));
        dest_surfacerect.x  = destrect->x + pBrutus->display()->view_rect.x;
        dest_surfacerect.y  = destrect->y + pBrutus->display()->view_rect.y;
    }

    bsurface_blit(dest_surface, &dest_surfacerect,
                  operation,
                  src_surface1, &src_surfacerect1,
                  src_surface2, &src_surfacerect2,
                  pixel1, pixel2);
}

/**
Calculate the main drawing area and border areas based on display configuration.
**/
void Brutus::getBorderRects(bwin_rect borderRect[]) /* [out] */
{
    int right = _displayConfig.view_rect.x + _displayConfig.view_rect.width - 1;
    int bottom = _displayConfig.view_rect.y + _displayConfig.view_rect.height - 1;

    /* left */
    BWIN_SET_RECT(&borderRect[0], 0, 0, _displayConfig.view_rect.x, _displayConfig.height);

    /* top */
    BWIN_SET_RECT(&borderRect[1], _displayConfig.view_rect.x, 0, _displayConfig.view_rect.width, _displayConfig.view_rect.y);

    /* right */
    BWIN_SET_RECT(&borderRect[2], right, 0, _displayConfig.width - right, _displayConfig.height);

    /* bottom */
    BWIN_SET_RECT(&borderRect[3], _displayConfig.view_rect.x, bottom, _displayConfig.view_rect.width, _displayConfig.height - bottom);
}

int Brutus::openDisplay(Config *cfg)
{
    int bpp = 0; // bits per pixel
    int index = displayindex();
    unsigned extra_framebuffers;
    bsurface_create_settings surface_create_settings;
    bsurface_settings surface_settings;
    bsurface_memory mem;
    bwin_rect borderRect[4];
    bwin_framebuffer_settings fb_settings;
    bool alphaperpixel = false;
    bsurface_create_settings create_settings;
    bool need_second_display = false;
    BrutusDisplayConfiguration *config = &_displayConfig;
    const char *str;

    memset(&_display, 0, sizeof(_display));
    _display.view_rect = config->view_rect;
    getBorderRects(borderRect);

    str = cfg->get(Config::OUTPUT_3D_MODE);
    if (str) {
        if (!strcmp(str, "sbs")) {
            _display.s3dTv.output = Brutus3DTVMode_eSBS;
        }
        else if (!strcmp(str, "ou")) {
            _display.s3dTv.output = Brutus3DTVMode_eOU;
        }
        else if (!strcmp(str, "auto")) {
            _display.s3dTv.output = Brutus3DTVMode_eOff;
            _display.s3dTv.automode = true;
        }
        else {
            BDBG_WRN(("Unrecognized OUTPUT_3D_MODE string: %s", str));
        }
        _display.s3dTv.znorm = cfg->getInt(Config::OUTPUT_3D_ZNORM);
    }

    BDBG_MSG(("Opening bdisplay(%d)", index));
    _display.display = bdisplay_open(B_ID(index));
    if (!_display.display) goto error;

    if (cfg->getBool(Config::HDSD_SINGLE)) {
        BDBG_MSG(("Opening bdisplay(1)"));
#if B_TWO_DISPLAY_OUTPUT_ISSUE
        /* We need to remove resources  from Display 1 before opening Display 2*/
         bdisplay_settings s;
         bdisplay_get(_display.display, &s);
         s.svideo = NULL;
         s.composite = NULL;
         s.rf = NULL;
         bdisplay_set(_display.display, &s);
#endif
        _display.display2 = bdisplay_open(B_ID(1));
    }
    else {
        _display.display2 = NULL;
    }

    BDBG_MSG(("Opening bgraphics(%d)", index));
    _display.graphics = bgraphics_open(B_ID(index), _display.display);
    if (!_display.graphics) goto error;
    if (cfg->getBool(Config::HDSD_SINGLE)) {
        BDBG_ASSERT(index == 0);
        BDBG_ASSERT(_display.display2);
        BDBG_MSG(("Opening bgraphics(1)"));
        _display.graphics2 = bgraphics_open(B_ID(1), _display.display2);
        if (!_display.graphics2) goto error;
    }

    bwin_framebuffer_settings_init(&fb_settings);
    switch (config->pixel_format) {
    case bgraphics_pixel_format_r5_g6_b5:
        fb_settings.pixel_format = bwin_pixel_format_r5_g6_b5;
        bpp = 16;
        break;

    case bgraphics_pixel_format_a1_r5_g5_b5:
        fb_settings.pixel_format = bwin_pixel_format_a1_r5_g5_b5;
        alphaperpixel = true;
        bpp = 16;
        break;

    case bgraphics_pixel_format_a8_r8_g8_b8:
        fb_settings.pixel_format = bwin_pixel_format_a8_r8_g8_b8;
        alphaperpixel = true;
        bpp = 32;
        break;

    case bgraphics_pixel_format_palette8:
        fb_settings.pixel_format = bwin_pixel_format_palette8;
        alphaperpixel = true;
        bpp = 8;
        break;

    default:
        return -1;
    }

    /* Create a double-buffered framebuffer */
    bsurface_create_settings_init(&create_settings, _display.graphics);
    if (_display.s3dTv.output > Brutus3DTVMode_eOff) {
#if B_HAS_GFD_VERTICAL_UPSCALER
        /* gfx rts is limited to 1280. this causes downscale for 1080i/p outputs. */
        create_settings.width = 1280;
#else
        create_settings.width = 1440;
#endif
    }
    else {
        create_settings.width = 720;
    }
    
#if B_HAS_GFD_VERTICAL_UPSCALER
    create_settings.height = 480;
#else
    create_settings.height = 1080;
#endif
    create_settings.pixel_format = config->pixel_format;
#if BCHP_CHIP == 7400
    if (cfg->getBool(Config::SECOND_DISPLAY)) {
        /* 2nd app is RTS limited to 16 bit color */
        if (bpp > 16) {
            create_settings.pixel_format = bgraphics_pixel_format_a1_r5_g5_b5;
        }
    }
#endif
#if defined DONT_DOUBLE_BUFFER
    extra_framebuffers = 0;
#else
    extra_framebuffers = 1;
#endif
    BDBG_MSG(("Creating graphics framebuffer %dx%d, pixel_format=%d, fbs=%d", create_settings.width, create_settings.height, create_settings.pixel_format, extra_framebuffers+1));
    _display.actual_framebuffer = bgraphics_create_framebuffer(_display.graphics, &create_settings, extra_framebuffers);
    if (!_display.actual_framebuffer)
        goto error;

    if (_display.graphics2) {
        /* max resolution on SD display is PAL */
        create_settings.width = 720;
        create_settings.height = 576;

#if B_GFD_1_YCRCB_ONLY
        /* Create YCrCb frame buffer. The bsurface_copy in brutus_fb_sync will do the conversion from RGB to YCrCb. */
        create_settings.pixel_format = bgraphics_pixel_format_a8_y8_cb8_cr8;
#endif

#if B_DEFAULT_TO_ARGB8888_PIXEL
#else
        if (bpp > 16) {
            /* 2nd framebuffer is limited to 16 bit color on these platforms */
            create_settings.pixel_format = bgraphics_pixel_format_a1_r5_g5_b5;
        }
#endif
        _display.actual_framebuffer2 = bgraphics_create_framebuffer(_display.graphics2,
            &create_settings, true /* double_buffered */);
        if (!_display.actual_framebuffer2)
            goto error;
    }

#if !defined DRAW_DIRECTLY_TO_FRAMEBUFFER
    /* we'll never to cpu draws to framebuffer, so disable needless cache flushes */
    bsurface_get(_display.actual_framebuffer, &surface_settings);
    surface_settings.disable_automatic_cache_flush = true;
    bsurface_set(_display.actual_framebuffer, &surface_settings);
    if (_display.actual_framebuffer2) {
        bsurface_get(_display.actual_framebuffer2, &surface_settings);
        surface_settings.disable_automatic_cache_flush = true;
        bsurface_set(_display.actual_framebuffer2, &surface_settings);
    }
#endif

    BDBG_MSG(("Set graphics settings"));
    bgraphics_settings graphics_settings;
    bgraphics_get(_display.graphics, &graphics_settings);
    graphics_settings.enabled = false;
    graphics_settings.chromakey_enabled = !alphaperpixel;
    graphics_settings.lower_chromakey = config->chromakeyColor;
    graphics_settings.upper_chromakey = config->chromakeyColor;
    graphics_settings.offscreen_framebuffer = true;

    bdisplay_settings display_settings;
    bvideo_format_settings fmt_settings;
    bdisplay_get(_display.display, &display_settings);
    bdisplay_get_video_format_settings(&display_settings, &fmt_settings);
    graphics_settings.destination_width = fmt_settings.width;
    graphics_settings.destination_height = fmt_settings.height;
    if (bgraphics_set(_display.graphics, &graphics_settings))
        goto error;

    if (_display.graphics2) {
        /* set graphics for other display if needed. don't set source/dest width/height */
        bgraphics_get(_display.graphics2, &graphics_settings);
        graphics_settings.enabled = false;
        graphics_settings.chromakey_enabled = !alphaperpixel;
        graphics_settings.lower_chromakey = config->chromakeyColor;
        graphics_settings.upper_chromakey = config->chromakeyColor;
        graphics_settings.offscreen_framebuffer = true;
        if (bgraphics_set(_display.graphics2, &graphics_settings))
            goto error;
    }

    /* bring up settop api graphics */
    bsurface_create_settings_init(&surface_create_settings, _display.graphics);
    surface_create_settings.pixel_format = config->pixel_format;

#if !defined DRAW_DIRECTLY_TO_FRAMEBUFFER
    /* Create an off-screen surface. Even with double-buffering this is essential
    because we use M2MC to scale the graphics from a fixed 720x480 up to the
    framebuffer which must match the output format in the vertical axis. */
    surface_create_settings.width = config->width;
    surface_create_settings.height = config->height;

#if BCHP_CHIP == 7420
    #ifdef NEXUS_PLATFORM_97410_UMA_32b
     /*
      * In this config, only MEMC0 is active and is associated with heap0
      */
     create_settings.heap_index =0;
    #else
    /* Set heap index to 0 for 3rd display */
     create_settings.heap_index = (index == 2)?0:1;
    #endif
#endif

#if BCHP_CHIP == 7422
    /* Set heap index to 1 for 3rd graphics display */
     create_settings.heap_index = (index == 2)?1:0;
#elif BCHP_CHIP == 7425 || BCHP_CHIP == 7435
    /* Set heap index to 0 for 3rd graphics display */
     create_settings.heap_index = (index == 2)?0:1;
#endif

    _display.mainsurface = bsurface_create(_display.graphics, &surface_create_settings);
    bsurface_get_memory(_display.mainsurface, &mem);
    memset(mem.buffer, 0, surface_create_settings.height * mem.pitch);

    /* bring up bwin framebuffer for that surface */
    fb_settings.width = config->view_rect.width;
    fb_settings.height = config->view_rect.height;
    fb_settings.pitch = mem.pitch;
    /* offset into the buffer to define the drawable area */
    fb_settings.buffer = (void*)((unsigned)mem.buffer +
        config->view_rect.x*(bpp/8) + config->view_rect.y*mem.pitch);

    /* enable callback to bgraphics_sync to support blitting to framebuffer */
    fb_settings.drawops.sync = brutus_fb_sync;
    fb_settings.drawops.fill_rect = brutus_fill_rect;
    fb_settings.drawops.copy_rect = brutus_copy_rect;
    fb_settings.drawops.blit                  = brutus_blit;
    fb_settings.drawops.device_bitmap_create  = brutus_device_bitmap_create;
    fb_settings.drawops.device_bitmap_destroy = brutus_device_bitmap_destroy;

    fb_data.pBrutus = this;
    fb_data.surface = _display.mainsurface;
    fb_settings.data = (void*)&fb_data;

    // start bwin
    bwin_engine_settings win_engine_settings;
    bwin_engine_settings_init(&win_engine_settings);
    _display.win_engine = bwin_open_engine(&win_engine_settings);
    if (!_display.win_engine)
        goto error;

    _display.framebuffer = bwin_open_framebuffer(_display.win_engine, &fb_settings);
    if (!_display.framebuffer) goto error;

    /**
    7038 DUAL_OUTPUT and SECOND_DISPLAY doesn't rely on settop api defaults.
    **/
    if (cfg->getBool(Config::DUAL_OUTPUT) && !cfg->getBool(Config::HDSD_SINGLE)) {
        bdisplay_settings s;
        bdisplay_get(_display.display, &s);
        /* On 7038 DUAL_OUTPUT, app0 is always component, whether SD or HD.
        app1 is always composite/svideo and is therefore always SD. No swapping. */
        #if B_TRIPLE_VEC_SYSTEM
        if (index == 0) {			
            s.component = boutput_component_open(B_ID(0));
            s.svideo = boutput_svideo_open(B_ID(0));
            s.composite = boutput_composite_open(B_ID(0));
            s.rf = NULL;
        }
        else {
            s.component = NULL;
            s.svideo = NULL;
            s.composite = boutput_composite_open(B_ID(1));
            #ifdef MACROVISION_SUPPORT
            s.macrovision_type=bmacrovision_type_agc_only;
            #endif
            s.rf = boutput_rf_open(B_ID(0));
        }
        #elif B_TRIPLE_DISPLAY_DUAL_VEC_SYSTEM
        if (index == 0) {
            s.component = NULL;
            s.svideo = NULL;
            s.composite = boutput_composite_open(B_ID(0));
            s.rf = NULL;
            #ifdef MACROVISION_SUPPORT
            s.macrovision_type=bmacrovision_type_agc_only;
            #endif
        }
        else {
            s.component = boutput_component_open(B_ID(0));
            s.svideo = NULL;
            s.composite = NULL;
            s.rf = NULL;
        }
        #else
        if (index == 0) {
            s.component = boutput_component_open(B_ID(0));
            s.svideo = NULL;
            s.composite = NULL;
            s.rf = NULL;
        }
        else {
            s.component = NULL;
            s.svideo = boutput_svideo_open(B_ID(0));
            s.composite = boutput_composite_open(B_ID(0));
            s.rf = boutput_rf_open(B_ID(0));
        }
        #endif
        bdisplay_set(_display.display, &s);
    }
    else if (cfg->getBool(Config::HDSD_SINGLE)) {
        bdisplay_settings s;

        /* For now, HDSD_SIMUL is a init mode. We may make it dynamic in the future. */
        #if BCHP_CHIP != 7400
        BDBG_ASSERT(index == 0); // only one app
        #endif
        BDBG_ASSERT(_display.display2);

        /* Change outputs for HD/SD simul. Do not change display format. This
        happens in control.cpp. */
        bdisplay_get(_display.display, &s);
        s.svideo = NULL;
        s.composite = NULL;
        s.rf = NULL;
        #ifdef MACROVISION_SUPPORT
        s.macrovision_type=bmacrovision_type_agc_only;
        #endif
        bdisplay_set(_display.display, &s);

        bdisplay_get(_display.display2, &s);
        s.svideo = boutput_svideo_open(B_ID(0));
        s.composite = boutput_composite_open(B_ID(0));
        #ifdef MACROVISION_SUPPORT
        s.macrovision_type=bmacrovision_type_agc_only;
        #endif
        /* In 3-display mode, RF will go to the second UI */
        if ( cfg->getBool(Config::DUAL_OUTPUT) )
            s.rf = NULL;
        else
        {
            s.rf = boutput_rf_open(B_ID(0));
            #if B_TRIPLE_VEC_SYSTEM
            s.composite2 = boutput_composite_open(B_ID(1));
            #ifdef MACROVISION_SUPPORT
            s.macrovision_type=bmacrovision_type_agc_only;
            #endif
            #endif
        }

        bdisplay_set(_display.display2, &s);

        /* In control.cpp, we have to clone decode window 2 from 0 to enable
        the simul. */
    }
    else if (!cfg->getBool(Config::DUAL_OUTPUT) && cfg->getBool(Config::SECOND_DISPLAY)) {
        /* If we want to run 2nd output only (secondary VEC) on a 7038 system, we
        can do that, but there are limitations on the number of DACs that can be
        used. I'm choosing to run all SD outputs, no components.
        This mode is not really supported. */
        bdisplay_settings s;

        bdisplay_t d0 = bdisplay_open(B_ID(0));
        bdisplay_get(d0, &s);
        s.component = NULL;
        s.svideo = NULL;
        s.rf = NULL;
        s.composite = NULL;
        bdisplay_set(d0, &s);
        bdisplay_close(d0);

        bdisplay_get(_display.display, &s);
        //s.component = boutput_component_open(B_ID(0));
        s.svideo = boutput_svideo_open(B_ID(0));
        s.composite = boutput_composite_open(B_ID(0));
        s.rf = boutput_rf_open(B_ID(0));
        bdisplay_set(_display.display, &s);
    }
    else {
        /* 1st display */
        bdisplay_settings s;
        bdisplay_t d1 = NULL;

        if (_display.display2)
            d1 = _display.display2;
            else if (bconfig->resources.display.total > 1) {
#if B_TWO_DISPLAY_OUTPUT_ISSUE
                /* We need to remove resources  from Display 1 before opening Display 2*/
                bdisplay_get(_display.display, &s);
                s.svideo = NULL;
                s.composite = NULL;
                s.rf = NULL;
                bdisplay_set(_display.display, &s);
#endif
                d1 = bdisplay_open(B_ID(1));
            }
        if (d1) {
            bdisplay_get(d1, &s);
            s.component = NULL;
            s.svideo = NULL;
            s.composite = NULL;
            s.rf = NULL;
            bdisplay_set(d1, &s);
            bdisplay_close(d1);
        }

        bdisplay_get(_display.display, &s);
#if B_TWO_DISPLAY_OUTPUT_ISSUE
        s.svideo =  boutput_svideo_open(B_ID(0));
        s.composite = boutput_composite_open(B_ID(0));
        s.rf = boutput_rf_open(B_ID(0));
#endif

        s.component = boutput_component_open(B_ID(0));
        if (fmt_settings.height <= 576) {
            s.svideo = boutput_svideo_open(B_ID(0));
            s.composite = boutput_composite_open(B_ID(0));
            s.rf = boutput_rf_open(B_ID(0));
        }
        bdisplay_set(_display.display, &s);
    }

#else
    /* Legacy platforms where we draw directly to the frame buffer */

    /* There is no difference between these for legacy */
    _display.mainsurface = _display.actual_framebuffer;

    if (bsurface_get_memory(_display.mainsurface, &mem))
        return -1;
    memset(mem.buffer, 0, surface_create_settings.height * mem.pitch);

    /* bring up framebuffer for that surface */
    fb_settings.drawops.sync = brutus_fb_sync;

    fb_data.pBrutus = this;
    fb_data.surface = _display.mainsurface;
    fb_settings.data = (void*)&fb_data;

    fb_settings.width = config->view_rect.width;
    fb_settings.height = config->view_rect.height;
    fb_settings.pitch = mem.pitch;
#ifndef DONT_DOUBLE_BUFFER
    /* offset into the buffer to define the drawable area */
    fb_settings.second_buffer = (void*)((unsigned)mem.buffer + config->view_rect.x*2 + config->view_rect.y*mem.pitch);

    /* we have to flip once in order to get the buffer pointer for the 2nd
    surface. bwin will start drawing here to the results of this bsurface_get_memory. */
    bgraphics_flip(_display.graphics);
    bsurface_get_memory(_display.mainsurface, &mem);
#endif
    fb_settings.buffer = (void*)((unsigned)mem.buffer + config->view_rect.x*2 + config->view_rect.y*mem.pitch);

    // start bwin
    BDBG_MSG(("Opening bwin"));
    bwin_engine_settings win_engine_settings;
    bwin_engine_settings_init(&win_engine_settings);
    _display.win_engine = bwin_open_engine(&win_engine_settings);
    if (!_display.win_engine)
        goto error;

    _display.framebuffer = bwin_open_framebuffer(_display.win_engine, &fb_settings);
    if (!_display.framebuffer) goto error;
#endif

    return 0;
error:
    BDBG_ERR(("Unable to open display %d", index));
    return -1;
}

int Brutus::closeDisplay()
{
    if (_display.framebuffer)
        bwin_close_framebuffer(_display.framebuffer);
    if (_display.win_engine)
        bwin_close_engine(_display.win_engine);
    if (_display.mainsurface)
        bsurface_destroy(_display.mainsurface);
    if (_display.graphics)
        bgraphics_close(_display.graphics);
    if (_display.graphics2)
        bgraphics_close(_display.graphics2);
    return 0;
}

void Brutus::setFullscreenGraphics(bool enabled)
{
    setFullscreenGraphics(enabled ? _displayConfig.backgroundColor : _displayConfig.chromakeyColor);
}

void Brutus::setFullscreenGraphics(uint32_t pixel)
{
    bwin_rect borderRect[4];

    getBorderRects(borderRect);
    bsurface_fill(_display.mainsurface, (const bsettop_rect*)&borderRect[0], pixel);
    bsurface_fill(_display.mainsurface, (const bsettop_rect*)&borderRect[1], pixel);
    bsurface_fill(_display.mainsurface, (const bsettop_rect*)&borderRect[2], pixel);
    bsurface_fill(_display.mainsurface, (const bsettop_rect*)&borderRect[3], pixel);
}

int Brutus::screenCapture(const char *filename)
{
    bsurface_settings settings;
    bsurface_memory mem;
    FILE *f;

    f = fopen(filename, "wb+");
    if (!f) {
        BDBG_ERR(("Unable to create %s", filename));
        return -1;
    }

    bsurface_get(_display.mainsurface, &settings);
    bsurface_get_memory(_display.mainsurface, &mem);
    for (unsigned i=0;i<settings.position.height;i++) {
        void *ptr = (void*)((unsigned long)mem.buffer + i*mem.pitch);
        fwrite(ptr, mem.pitch, 1, f);
    }
    fclose(f);
    return 0;
}

/* Set the graphics scaling based on current display format,
and handle any graphics resets as needed. */
int Brutus::resetGraphics()
{
    bdisplay_settings display_settings;
    bvideo_format_settings video_format_settings;
    bgraphics_settings graphics_settings;

    bdisplay_get(_display.display, &display_settings);
    bdisplay_get_video_format_settings(&display_settings, &video_format_settings);

    // reenable the graphics with proper scaling and other settings
    bgraphics_get(_display.graphics, &graphics_settings);
    graphics_settings.destination_width = video_format_settings.width;
    graphics_settings.destination_height = video_format_settings.height;
    graphics_settings.enabled = true;
    if (bgraphics_set(_display.graphics, &graphics_settings)) {
        BDBG_ERR(("unable to reset graphics to %dx%d", graphics_settings.destination_width, graphics_settings.destination_height));
        return -1;
    }

    if (_display.graphics2) { /* HDSD_SINGLE */
        bool is_pal; /* is 50/25 hertz, which applies to PAL and SECAM */

        /* if display2 exists, use it's settings instead of display1's for graphics2 */
        if (_display.display2)
        {
            bdisplay_get(_display.display2, &display_settings);
            bdisplay_get_video_format_settings(&display_settings, &video_format_settings);
        }

        /* Determine if this is PAL (SD or HD) */
        is_pal = video_format_settings.refresh_rate != 24 &&
            video_format_settings.refresh_rate != 30 &&
            video_format_settings.refresh_rate != 60;

        bgraphics_get(_display.graphics2, &graphics_settings);
        graphics_settings.destination_width = 720;
        graphics_settings.destination_height = is_pal ? 576 : 480;
        graphics_settings.enabled = true;
        if (bgraphics_set(_display.graphics2, &graphics_settings)) {
            BDBG_ERR(("unable to reset graphics2"));
            return -1;
        }
    }

    /* flip bgraphics (1st and possibly 2nd displays) */
    brutus_fb_sync(_display.framebuffer);
    return 0;
}
