/***************************************************************************
 *     Copyright (c) 2004-2006, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_default_drawops.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 6/13/06 3:35p $
 *
 * Module Description: cpu-based drawing primitives. These are the defaults
 * used in bwin_framebuffer_settings.drawops.
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_default_drawops.c $
 * 
 * 6   6/13/06 3:35p mphillip
 * PR14698: Flagged several unused variables
 * 
 * 5   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/1   5/9/06 12:49p tokushig
 * Added 'blit' drawop to allow for hardware accelerated blit operations.
 * 
 * 4   4/17/06 7:47p jgarrett
 * PR 20951: Added support for both emulated and native FP
 * 
 * 3   8/4/05 4:42p erickson
 * PR15139: fixed -pedantic warnings
 * 
 * 2   2/15/05 5:23p erickson
 * PR13387: added palette8 support
 * 
 * 1   2/7/05 9:04p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/8   7/26/04 3:11p erickson
 * PR11674: implemented optional double-buffering in bwin
 * 
 * Irvine_BSEAVSW_Devel/7   3/19/04 5:18p erickson
 * PR10166: added generic_color to the drawing primitive callbacks so that
 * hardware can use either pixelformat specific or generic ARGB8888 color
 * 
 * Irvine_BSEAVSW_Devel/6   2/5/04 11:52a erickson
 * PR9107: enable debug
 * 
 * Irvine_BSEAVSW_Devel/5   1/29/04 4:40p erickson
 * PR9107: precalc some of the circle values for mwidgets
 * 
 * Irvine_BSEAVSW_Devel/4   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/3   1/19/04 9:02a erickson
 * PR9107: getting ready for BRUTUS_PR9107 branch
 *
 * Irvine_BSEAVSW_Devel/2   1/14/04 11:36p erickson
 * PR9107: freetype support added to iolaus and still doing initial
 * development
 *
 * Irvine_BSEAVSW_Devel/1   1/13/04 2:01p erickson
 * PR9107: iolaus drawing, clipping, event queue working
 *
 ****************************************************************/
#include "bwin.h"
#include "bwin_priv.h"
#include <assert.h>
#include <string.h>
#if defined FLOAT_SUPPORT || defined EMULATED_FLOAT_SUPPORT
#include <math.h>
#endif
BDBG_MODULE(bwin_default_drawops);

/**
NOTE:
Color is already modified for the pixel_format.
Coordinates are all global (relative to the framebuffer, not a window).
You must convert the frambuffer->buffer pointer to the right type. SET_PIXEL
makes this easier.
**/

#define SET_PIXEL(PTR, FB, COLOR) \
    switch ((FB)->bpp) { \
    case 8:  *(unsigned char *)(PTR) = (COLOR); break; \
    case 16: *(unsigned short *)(PTR) = (COLOR); break; \
    case 32: *(unsigned long *)(PTR) = (COLOR); break; \
    }

#define GET_BUFFER_PTR(FB, X, Y) \
    ((unsigned char *)(FB)->buffer + ((FB)->settings.pitch * (Y)) + ((X) * ((FB)->bpp/8))) 

void bwin_p_draw_point(bwin_framebuffer_t fb, int x, int y, uint32_t color,
    uint32_t generic_color)
{
    unsigned char *b = GET_BUFFER_PTR(fb, x, y);
    BSTD_UNUSED(generic_color);
    SET_PIXEL(b, fb, color);
}

void bwin_p_draw_line(bwin_framebuffer_t fb, int x1, int y1, int x2, int y2, uint32_t color,
    uint32_t generic_color)
{
    int i;
    unsigned char *b;

    b = GET_BUFFER_PTR(fb, x1, y1);
    BSTD_UNUSED(generic_color);

    BDBG_MSG(("draw_line 0x%06x: %d,%d,%d,%d", color, x1, y1, x2, y2));
    if (x1 == x2) {
        for (i=y1;i<y2;i++) {
            SET_PIXEL(b, fb, color);
            b += fb->settings.pitch;
        }
    }
    else if (y1 == y2) {
        /* TODO: if every byte in color is the same, we can memset */
        int inc = fb->bpp/8;
        for (i=x1;i<x2;i++) {
            SET_PIXEL(b, fb, color);
            b += inc;
        }
    }
    else {
        BDBG_ERR(("No diagonal line support: %d,%d,%d,%d", x1,y1,x2,y2));
    }
}

void bwin_p_fill_rect(bwin_framebuffer_t fb, const bwin_rect *rect, uint32_t color, 
    uint32_t generic_color)
{
    unsigned i;
    BSTD_UNUSED(generic_color);
    BDBG_MSG(("fill_rect 0x%06x: %d,%d,%d,%d", color, rect->x,rect->y,rect->width,rect->height));
    for (i=0;i<rect->height;i++)
        bwin_p_draw_line(fb, rect->x, rect->y+i, rect->x+rect->width, rect->y+i, color, generic_color);
}

void bwin_p_copy_rect(bwin_framebuffer_t destfb, const bwin_rect *destrect,
    bwin_framebuffer_t srcfb, const bwin_rect *srcrect)
{
    unsigned char *d, *s;
    unsigned cpywidth,i;

    /* scaling not supported yet */
    assert(destrect->width == srcrect->width && destrect->height == srcrect->height);
    /* pixel conversion not supported yet */
    assert(destfb->settings.pixel_format == srcfb->settings.pixel_format);

    d = GET_BUFFER_PTR(destfb, destrect->x, destrect->y);
    s = GET_BUFFER_PTR(srcfb, srcrect->x, srcrect->y);
    cpywidth = destrect->width * destfb->bpp/8;

    for (i=0;i<destrect->height;i++) {
        memcpy(d, s, cpywidth);
        d += destfb->settings.pitch;
        s += srcfb->settings.pitch;
    }
}

void bwin_p_blit(bwin_framebuffer_t destfb, const bwin_rect *destrect,
                 const uint32_t operation,
                 bwin_framebuffer_t srcfb1, const bwin_rect *srcrect1,
                 bwin_framebuffer_t srcfb2, const bwin_rect *srcrect2,
                 const uint32_t pixel1, const uint32_t pixel2)
{
    BSTD_UNUSED(operation);
    BSTD_UNUSED(srcfb2);
    BSTD_UNUSED(srcrect2);
    BSTD_UNUSED(pixel1);
    BSTD_UNUSED(pixel2);
    /* no software blit implementation so just do copy_rect */
    bwin_p_copy_rect(destfb, destrect, srcfb1, srcrect1);
}

#if defined FLOAT_SUPPORT || defined EMULATED_FLOAT_SUPPORT

/* This is currently optimized for mwidget's radio buttons. */
struct circ_precalc {
#define MAX_PRECALC_RADIUS 10
    int values[MAX_PRECALC_RADIUS];
} g_circ_precalc[] = {
{{0}},
{{0}},
{{0}},
{{0}},
{{0}},
{{
/* r=5,y=0 */ 5,
/* r=5,y=1 */ 4,
/* r=5,y=2 */ 4,
/* r=5,y=3 */ 3,
/* r=5,y=4 */ 2,
/* r=7,y=0 */ 7
}},
{{0}},
{{
/* r=7,y=1 */ 6,
/* r=7,y=2 */ 6,
/* r=7,y=3 */ 6,
/* r=7,y=4 */ 5,
/* r=7,y=5 */ 4,
/* r=7,y=6 */ 3
}},
{{
/* r=8,y=0 */ 8,
/* r=8,y=1 */ 7,
/* r=8,y=2 */ 7,
/* r=8,y=3 */ 7,
/* r=8,y=4 */ 6,
/* r=8,y=5 */ 6,
/* r=8,y=6 */ 5,
/* r=8,y=7 */ 3
}},
{{
/* r=9,y=0 */ 9,
/* r=9,y=1 */ 8,
/* r=9,y=2 */ 8,
/* r=9,y=3 */ 8,
/* r=9,y=4 */ 8,
/* r=9,y=5 */ 7,
/* r=9,y=6 */ 6,
/* r=9,y=7 */ 5,
/* r=9,y=8 */ 4
}},
{{
/* r=10,y=0 */ 10,
/* r=10,y=1 */ 9,
/* r=10,y=2 */ 9,
/* r=10,y=3 */ 9,
/* r=10,y=4 */ 9,
/* r=10,y=5 */ 8,
/* r=10,y=6 */ 7,
/* r=10,y=7 */ 7,
/* r=10,y=8 */ 5,
/* r=10,y=9 */ 4
}}};

unsigned bwin_p_calc_circle(unsigned y, unsigned r)
{
    if (y > r)
        return r;
    else {
        if (r <= MAX_PRECALC_RADIUS && g_circ_precalc[r].values[0]) {
            return g_circ_precalc[r].values[y];
        }
        else {
            float yy = (float)y/r;
#if 0
/* Use this to populate the g_circ_precalc table. */
            printf("/* r=%d,y=%d */ %d,\n", r, y, (int)(sin(acos(yy)) * r));
#endif
            return (int)(sin(acos(yy)) * r);
        }
    }
}
#endif

void bwin_p_draw_ellipse(bwin_framebuffer_t fb, int x, int y,
    unsigned rx, unsigned ry, uint32_t color, uint32_t generic_color, const bwin_rect *cliprect)
{
#if defined FLOAT_SUPPORT || defined EMULATED_FLOAT_SUPPORT
    int j;

    BSTD_UNUSED(generic_color);
    ry = rx; /* force it to be a circle */

    for (j=0;j<(int)ry;j++) {
        bwin_rect rect;
        int i = bwin_p_calc_circle(j,ry);

        if (j == (int)ry-1) {
            BWIN_SET_RECT(&rect, x-i, (int)y-j, i*2, 0);
            bwin_intersect_rect(&rect, &rect, cliprect);
            if (rect.width || rect.height)
                bwin_p_draw_line(fb, rect.x, rect.y, rect.x+rect.width, rect.y+rect.height, color, generic_color);

            BWIN_SET_RECT(&rect, x-i, (int)y+j, i*2, 0);
            bwin_intersect_rect(&rect, &rect, cliprect);
            if (rect.width || rect.height)
                bwin_p_draw_line(fb, rect.x, rect.y, rect.x+rect.width, rect.y+rect.height, color, generic_color);
        }
        else {
            if (BWIN_POINT_IN_RECT(x-i, (int)y-j, cliprect))
                bwin_p_draw_point(fb, x-i, (int)y-j, color, generic_color);
            if (BWIN_POINT_IN_RECT(x+i, (int)y-j, cliprect))
                bwin_p_draw_point(fb, x+i, (int)y-j, color, generic_color);
            if (BWIN_POINT_IN_RECT(x-i, (int)y+j, cliprect))
                bwin_p_draw_point(fb, x-i, (int)y+j, color, generic_color);
            if (BWIN_POINT_IN_RECT(x+i, (int)y+j, cliprect))
                bwin_p_draw_point(fb, x+i, (int)y+j, color, generic_color);
        }
    }
#endif
}

void bwin_p_fill_ellipse(bwin_framebuffer_t fb, int x, int y,
    unsigned rx, unsigned ry, uint32_t color, uint32_t generic_color, const bwin_rect *cliprect)
{
#if defined FLOAT_SUPPORT || defined EMULATED_FLOAT_SUPPORT
    int j;

    BSTD_UNUSED(generic_color);
    ry = rx; /* force it to be a circle */

    for (j=0;j<(int)ry;j++) {
        bwin_rect rect;
        int i = bwin_p_calc_circle(j,ry);

        BWIN_SET_RECT(&rect, x-i, (int)y-j, i*2, 0);
        bwin_intersect_rect(&rect, &rect, cliprect);
        if (rect.width || rect.height)
            bwin_p_draw_line(fb, rect.x, rect.y, rect.x+rect.width, rect.y+rect.height, color, generic_color);

        BWIN_SET_RECT(&rect, x-i, (int)y+j, i*2, 0);
        bwin_intersect_rect(&rect, &rect, cliprect);
        if (rect.width || rect.height)
            bwin_p_draw_line(fb, rect.x, rect.y, rect.x+rect.width, rect.y+rect.height, color, generic_color);
    }
#endif
}
