/***************************************************************************
 *     Copyright (c) 2004-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_draw.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 2/6/07 3:19p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_draw.c $
 * 
 * 5   2/6/07 3:19p erickson
 * PR27587: check bounds on fill/blit after conversion to global.
 * 
 * 4   12/19/06 1:30p erickson
 * PR26643: added fill_rect_threshold
 * 
 * 3   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/1   5/9/06 12:49p tokushig
 * Added 'blit' drawop to allow for hardware accelerated blit operations.
 * 
 * 2   2/15/05 5:23p erickson
 * PR13387: added palette8 support
 * 
 * 1   2/7/05 9:04p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/12   1/14/05 4:55p erickson
 * PR13829: implemented clipping in bwin_copy_rect
 * 
 * Irvine_BSEAVSW_Devel/11   9/9/04 1:52p erickson
 * PR12608: convert dbg from private impl to magnum impl
 * 
 * Irvine_BSEAVSW_Devel/10   3/19/04 5:18p erickson
 * PR10166: added generic_color to the drawing primitive callbacks so that
 * hardware can use either pixelformat specific or generic ARGB8888 color
 * 
 * Irvine_BSEAVSW_Devel/9   2/19/04 3:40p bandrews
 * PR9723: Changed pixel conversion functions to use 32-bit masks
 * 
 * Irvine_BSEAVSW_Devel/8   2/19/04 1:31p erickson
 * PR9723: fixed compilation error for ansi
 * 
 * Irvine_BSEAVSW_Devel/7   2/17/04 7:17p bandrews
 * PR9723: Added 1555 support.
 * 
 * Irvine_BSEAVSW_Devel/6   2/5/04 11:52a erickson
 * PR9107: enable debug
 * 
 * Irvine_BSEAVSW_Devel/5   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/4   1/19/04 9:02a erickson
 * PR9107: getting ready for BRUTUS_PR9107 branch
 * 
 * Irvine_BSEAVSW_Devel/3   1/14/04 11:36p erickson
 * PR9107: freetype support added to iolaus and still doing initial
 * development
 *
 * Irvine_BSEAVSW_Devel/2   1/13/04 2:01p erickson
 * PR9107: iolaus drawing, clipping, event queue working
 *
 * Irvine_BSEAVSW_Devel/1   1/13/04 12:32a erickson
 * PR9107: initial impl of iolaus
 *
 ****************************************************************/
#include "bwin.h"
#include "bwin_priv.h"
#include <assert.h>
#include <string.h>
BDBG_MODULE(bwin_draw);

/* runtime support of all pixel formats. converts from argb8888 to bwin_pixel_format */
uint32_t bwin_p_convert_color(bwin_framebuffer_t fb, uint32_t color)
{
    uint32_t c;
    switch (fb->settings.pixel_format) {
    case bwin_pixel_format_r5_g6_b5:
        c = ((color & 0x00F80000) >> 8) | 
            ((color & 0x0000FC00) >> 5) | 
            ((color & 0x000000F8) >> 3);
        break;
    case bwin_pixel_format_a1_r5_g5_b5:
        c = ((color & 0x80000000) >> 16) | 
            ((color & 0x00F80000) >> 9) | 
            ((color & 0x0000F800) >> 6) |
            ((color & 0x000000F8) >> 3);
        break;
    default: /* argb8888 or p8 - no transform */
        c = color;
        break;
    }
    return c;
}

#define OPS(win) ((win)->fb->settings.drawops)

void bwin_draw_point(bwin_t win, int x, int y, uint32_t color,
    const bwin_rect *cliprect)
{
    if (!cliprect) cliprect = &win->settings.rect;
    if (BWIN_POINT_IN_RECT(x,y,cliprect)) {
        int gx, gy;
        bwin_p_convert_to_global(win, x, y, &gx, &gy);

        if (BWIN_POINT_IN_RECT(gx,gy,&win->fb->rect)) {
            (OPS(win).draw_point)(win->fb, gx, gy, bwin_p_convert_color(win->fb, color), color);
        }
    }
}

void bwin_draw_line(bwin_t win, int x1, int y1, int x2, int y2, uint32_t color,
    const bwin_rect *cliprect)
{
    bwin_rect intersect;

    /* normalize the line */
    if (x1 > x2) {
        int temp = x1;
        x1 = x2;
        x2 = temp;
    }
    if (y1 > y2) {
        int temp = y1;
        y1 = y2;
        y2 = temp;
    }

    /* find the intersection with the cliprect by converting to a rectangle. */
    BWIN_SET_RECT(&intersect, x1,y1,x2-x1,y2-y1);

    /* clip to cliprect (default to window) and framebuffer */
    if (!cliprect) cliprect = &win->settings.rect;
    bwin_intersect_rect(&intersect, &intersect, cliprect);
    if (!intersect.width && !intersect.height) return;

    /* handle boundary case */
    if (!intersect.width && intersect.x == cliprect->x+(int)cliprect->width)
        return;
    if (!intersect.height && intersect.y == cliprect->y+(int)cliprect->height)
        return;

    /* convert to global coordinates */
    bwin_p_convert_to_global(win, intersect.x, intersect.y, &intersect.x, &intersect.y);

    /* clip to framebuffer */
    cliprect = &win->fb->rect;
    bwin_intersect_rect(&intersect, &intersect, cliprect);
    if (!intersect.width && !intersect.height) return;

    /* handle boundary case */
    if (!intersect.width && intersect.x == cliprect->x+(int)cliprect->width)
        return;
    if (!intersect.height && intersect.y == cliprect->y+(int)cliprect->height)
        return;

    BDBG_MSG(("draw_line 0x%06x: %d,%d,%d,%d", color, intersect.x, intersect.y,
        intersect.x+intersect.width, intersect.y+intersect.height));

    (OPS(win).draw_line)(win->fb, intersect.x, intersect.y,
        intersect.x+intersect.width, intersect.y+intersect.height,
        bwin_p_convert_color(win->fb, color), color);
}

void bwin_fill_rect(bwin_t win, const bwin_rect *rect, uint32_t color,
    const bwin_rect *cliprect)
{
    bwin_rect intersect, grect;

    /* clip to cliprect (default to window) */
    if (!cliprect) cliprect = &win->settings.rect;
    bwin_intersect_rect(&intersect, rect, cliprect);

    /* convert to global coordinates */
    grect = intersect;
    bwin_p_convert_to_global(win, intersect.x, intersect.y, &grect.x, &grect.y);

    BDBG_MSG(("fill_rect %d,%d,%d,%d", grect.x,grect.y,grect.width,grect.height));
    BDBG_MSG(("          %d,%d,%d,%d", win->fb->rect.x,win->fb->rect.y,win->fb->rect.width,win->fb->rect.height));

    /* clip to framebuffer */
    bwin_intersect_rect(&grect, &grect, &win->fb->rect);

    if (grect.width && grect.height) {
	    if (grect.width * grect.height >= OPS(win).fill_rect_threshold) {
	        (OPS(win).fill_rect)(win->fb, &grect, bwin_p_convert_color(win->fb, color), color);
        }
        else {
	        bwin_p_fill_rect(win->fb, &grect, bwin_p_convert_color(win->fb, color), color);
        }
    }
}

void bwin_copy_rect(bwin_t destwin, const bwin_rect *destrect,
    bwin_t srcwin, const bwin_rect *srcrect,
    const bwin_rect *cliprect)
{
    bwin_rect g_destrect = *destrect;
    bwin_rect g_srcrect = *srcrect;

    /* If there's a cliprect, clip the dest, then clip the src by a proportional
    amount */   
	BDBG_MSG(("copy_rect %x,%x,%x,%x -> %x,%x,%x,%x (%x,%x,%x,%x)",
		srcrect->x,  srcrect->y,  srcrect->width,  srcrect->height,
		destrect->x, destrect->y, destrect->width, destrect->height,
		cliprect->x, cliprect->y, cliprect->width, cliprect->height));
    if (cliprect) {
        bwin_rect new_destrect;
        bwin_rect new_srcrect;
        bwin_intersect_rect(&new_destrect, destrect, cliprect);
        
        new_srcrect.x = srcrect->x + 
            (new_destrect.x - destrect->x) *
            srcrect->width / destrect->width;
        new_srcrect.y = srcrect->y + 
            (new_destrect.y - destrect->y) *
            srcrect->height / destrect->height;
        new_srcrect.width = new_destrect.width *
            srcrect->width / destrect->width;
        new_srcrect.height = new_destrect.height *
            srcrect->height / destrect->height;
        
        g_destrect = new_destrect;
        g_srcrect = new_srcrect;
    }
    
    bwin_p_convert_to_global(destwin, g_destrect.x, g_destrect.y, &g_destrect.x, &g_destrect.y);
    bwin_p_convert_to_global(srcwin, g_srcrect.x, g_srcrect.y, &g_srcrect.x, &g_srcrect.y);
    
    /* intersect with framebuffer */
    bwin_intersect_rect(&g_destrect, &g_destrect, &destwin->fb->rect);
    if (!g_destrect.width || !g_destrect.height) return;
    
	BDBG_MSG(("copy_rect %x,%x,%x,%x -> %x,%x,%x,%x",
		g_srcrect.x, g_srcrect.y, g_srcrect.width, g_srcrect.height,
		g_destrect.x, g_destrect.y, g_destrect.width, g_destrect.height));
    (OPS(destwin).copy_rect)(destwin->fb, &g_destrect, srcwin->fb, &g_srcrect);
}

void bwin_blit(bwin_t destwin, const bwin_rect *destrect,
               const unsigned int operation,
               bwin_t srcwin1, const bwin_rect *srcrect1,
               bwin_t srcwin2, const bwin_rect *srcrect2,
               const unsigned int pixel1, const unsigned int pixel2)
{
    bwin_rect g_destrect = *destrect;
    bwin_rect g_srcrect1 = *srcrect1;
    bwin_rect g_srcrect2 = *srcrect2;

    bwin_p_convert_to_global(destwin, g_destrect.x, g_destrect.y, &g_destrect.x, &g_destrect.y);
    bwin_p_convert_to_global(srcwin1, g_srcrect1.x, g_srcrect1.y, &g_srcrect1.x, &g_srcrect1.y);
    bwin_p_convert_to_global(srcwin2, g_srcrect2.x, g_srcrect2.y, &g_srcrect2.x, &g_srcrect2.y);

    /* intersect with framebuffer */
    bwin_intersect_rect(&g_destrect, &g_destrect, &destwin->fb->rect);
    if (!g_destrect.width || !g_destrect.height) return;
    
    (OPS(destwin).blit)(destwin->fb, &g_destrect, 
                        operation,
                        srcwin1->fb, &g_srcrect1, srcwin2->fb, &g_srcrect2,
                        pixel1, pixel2);
}

void bwin_draw_ellipse(bwin_t win, int x, int y,
    unsigned rx, unsigned ry, int color, const bwin_rect *cliprect)
{
    bwin_rect rect;

    /* set window cliprect */
    if (!cliprect)
        rect = win->settings.rect;
    else {
        rect = *cliprect;
        bwin_intersect_rect(&rect, &rect, &win->settings.rect);
    }

    /* convert to global coordinates */
    bwin_p_convert_to_global(win, rect.x, rect.y, &rect.x, &rect.y);
    bwin_p_convert_to_global(win, x, y, &x, &y);

    /* intersect with framebuffer */
    bwin_intersect_rect(&rect, &rect, &win->fb->rect);

    /* note that we don't actually clip the ellipse. this must be done at draw time. */
    if (rect.width && rect.height)
        (OPS(win).draw_ellipse)(win->fb, x, y, rx, ry, bwin_p_convert_color(win->fb, color), color, &rect);
}

void bwin_fill_ellipse(bwin_t win, int x, int y,
    unsigned rx, unsigned ry, int color, const bwin_rect *cliprect)
{
    bwin_rect rect;

    /* set window cliprect */
    if (!cliprect)
        rect = win->settings.rect;
    else {
        rect = *cliprect;
        bwin_intersect_rect(&rect, &rect, &win->settings.rect);
    }

    /* convert to global coordinates */
    bwin_p_convert_to_global(win, rect.x, rect.y, &rect.x, &rect.y);
    bwin_p_convert_to_global(win, x, y, &x, &y);

    /* intersect with framebuffer */
    bwin_intersect_rect(&rect, &rect, &win->fb->rect);

    /* note that we don't actually clip the ellipse. this must be done at draw time. */
    if (rect.width && rect.height)
        (OPS(win).fill_ellipse)(win->fb, x, y, rx, ry, bwin_p_convert_color(win->fb, color), color, &rect);
}
