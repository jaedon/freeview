/***************************************************************************
 *     Copyright (c) 2004-2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_priv.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 9/24/07 11:28a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_priv.h $
 * 
 * 4   9/24/07 11:28a vishk
 * PR 35292: SettopAPI-Coverity (CID 295): UFFER_SIZE_WARNING, bwin_font.c
 * 
 * 2   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/2   5/9/06 12:49p tokushig
 * Added 'blit' drawop to allow for hardware accelerated blit operations.
 * 
 * SanDiego_Brutus_Skin/1   5/3/06 12:23p tokushig
 * added support for hardware accelerated offscreen buffers to bwin
 * 
 * 1   2/7/05 9:06p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/16   1/5/05 3:00p erickson
 * PR13639: moved bwin_result from public to private
 * 
 * Irvine_BSEAVSW_Devel/15   7/26/04 3:12p erickson
 * PR11674: implemented optional double-buffering in bwin
 * 
 * Irvine_BSEAVSW_Devel/14   7/6/04 3:55p erickson
 * PR11771: settop api dataflow redesign
 * 
 * Irvine_BSEAVSW_Devel/13   3/23/04 5:34p erickson
 * PR9753: implemented zorder support by reworking the repaint algorithm
 * 
 * Irvine_BSEAVSW_Devel/12   3/19/04 5:19p erickson
 * PR10166: added generic_color to the drawing primitive callbacks so that
 * hardware can use either pixelformat specific or generic ARGB8888 color
 * 
 * Irvine_BSEAVSW_Devel/11   3/8/04 11:30a erickson
 * PR9883: use bwin member variable instead of static to support dual
 * output/single app configuration
 * 
 * Irvine_BSEAVSW_Devel/10   2/10/04 4:05p erickson
 * PR9691: added sync callback
 * 
 * Irvine_BSEAVSW_Devel/9   2/4/04 2:11p erickson
 * PR9107: implemented device io system
 * 
 * Irvine_BSEAVSW_Devel/8   2/3/04 10:05a erickson
 * PR9497: initial vxworks support
 * 
 * Irvine_BSEAVSW_Devel/7   1/30/04 4:36p erickson
 * PR9107: added quasi-mono mode because some fonts appear to be
 * antialiased only
 * 
 * Irvine_BSEAVSW_Devel/6   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 * 
 * Irvine_BSEAVSW_Devel/5   1/19/04 9:02a erickson
 * PR9107: getting ready for BRUTUS_PR9107 branch
 * 
 * Irvine_BSEAVSW_Devel/4   1/15/04 10:14a erickson
 * PR9107: continued initial development, ready for mlibs port
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
#ifndef BWIN_PRIV_H__
#define BWIN_PRIV_H__

#include <blst_list.h>
#include <blst_queue.h>
#include <stdio.h>
#ifdef FREETYPE_SUPPORT
#include <ft2build.h>
#include FT_FREETYPE_H
#endif
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#undef min
#define min(A,B) ((A)<(B)?(A):(B))
#undef max
#define max(A,B) ((A)>(B)?(A):(B))

#define XBDBG_MSG(X) do {printf X; printf("\n");}while(0)

/**
Internal event structure. It contains the external event and some internal
bookkeeping stuff.
**/
typedef struct bwin_p_event {
    bwin_event ev; /* external event */
    int magic;
    BLST_Q_ENTRY(bwin_p_event) link;
} bwin_p_event;

void bwin_p_event_init(bwin_p_event *event);
void bwin_p_add_event(bwin_engine_t win, const bwin_p_event *event);
void bwin_p_process_events(bwin_engine_t win);
void bwin_p_create_paint_event(bwin_t window, const bwin_rect *repaintrect);

/* Convert from window-relative coordintes to global coordinates (framebuffer relative) */
void bwin_p_convert_to_global(bwin_t window, int x, int y, int *globalx, int *globaly);
void bwin_p_convert_from_global(bwin_t window, int globalx, int globaly, int *x, int *y);

#define BWIN_FONT_VERSION 1

typedef uint32_t bwin_char; /* unicode character */

/* WARNING: if this structure changes, increment the BWIN_FONT_VERSION number */
typedef struct {
    int top;
    int left;
    unsigned width; /* in pixels */
    unsigned height; /* in pixels */
    unsigned pitch; /* linesize, in bytes */
    unsigned advance; /* in pixels, x axis */
    unsigned char *mem; /* pointer into bwin_font.glyph_mem */
} bwin_font_glyph;

/* WARNING: if this structure changes, increment the BWIN_FONT_VERSION number */
struct bwin_font {
    int version;
    int refcnt; /* for the bwin_engine cache */
    bwin_engine_t win;
    unsigned size;
    int antialiased; /* 0 is mono, 1 is antialiased,
        2 is quasi-mono. see bwin_font.c for an explanation of this. */
    unsigned height; /* max height, in pixels */
#ifdef FREETYPE_SUPPORT
    FT_Face face;
#else
    void *face; /* must be same size as FT_Face (which is a pointer) */
#endif
    int ascender; /* in pixels */
    int descender; /* in pixels */
    int cache_size; /* number of glyphs and chars in glyph_cache and char_map */
    int cache_maxsize; /* maximum number of entries in glyph_cache and char_map. corresponds to mem allocated */
    bwin_font_glyph *glyph_cache; /* points to array of size cache_size */
    bwin_char *char_map; /* points to array of size cache_size */
};

struct bwin_engine {
    bwin_engine_settings settings;
#ifdef FREETYPE_SUPPORT
    FT_Library freetype;
    bool freetype_init;
#endif

    bool valid_ev_list; /* The ev_list is built at get/peek event time. It is invalidated
                            whenever a new event is added. */
    BLST_Q_HEAD(evhead, bwin_p_event) ev_list;

#define BWIN_IO_HANDLES 20
    struct {
        bwin_io_handle handle;
        bool pending;
    } io[BWIN_IO_HANDLES];
    BKNI_EventHandle event;

    BLST_D_HEAD(fbhead,bwin_framebuffer) fb_list;

#define MAX_FONT_CACHE 15
#define BWIN_MAX_FILENAME 60
    struct {
        char filename[BWIN_MAX_FILENAME + 1]; /* +1 is for string null termination */
        bwin_font_t font;
    } fontcache[MAX_FONT_CACHE];

    int input_round_robin; /* This is a hack for round robinning the input devices
                                until settop api matures. */
};

struct bwin_framebuffer {
    bwin_engine_t win_engine;
    bwin_framebuffer_settings settings;
    bwin_rect rect; /* useful for clipping to framebuffer */
    int bpp;
    bool had_paint; /* keeps track if sync is needed in bwin_get_event */
    void *buffer; /* current buffer. this will change on bwin_sync
                    if you have double buffering enabled (settings.buffer2 != NULL) */
    bool is_buf_device_dependent;
    bool is_buf_internal_alloc;  /* if true, then buffer must be freed when the bwin framebuffer is closed */
    BLST_D_ENTRY(bwin_framebuffer) link; /* see bwin_engine.fb_list */
};

struct bwin {
    int magic;
    bwin_settings settings;
    bwin_framebuffer_t fb; /* cached from toplevel */

    bwin_rect repaint; /* client area currently scheduled for repainting */

    BLST_Q_ENTRY(bwin) link; /* see bwin.win_list. */
    BLST_Q_HEAD(winhead, bwin) win_list; /* doubly linked list of child windows */
};

/* default drawops. see bwin_default_drawops.c. */
void bwin_p_draw_point(bwin_framebuffer_t fb, int x, int y, uint32_t color, uint32_t generic_color);
void bwin_p_draw_line(bwin_framebuffer_t fb, int x1, int y1, int x2, int y2, uint32_t color, uint32_t generic_color);
void bwin_p_fill_rect(bwin_framebuffer_t fb, const bwin_rect *rect, uint32_t color, uint32_t generic_color);
void bwin_p_copy_rect(bwin_framebuffer_t destfb, const bwin_rect *destrect,
    bwin_framebuffer_t srcfb, const bwin_rect *srcrect);
void bwin_p_blit(bwin_framebuffer_t destfb, const bwin_rect *destrect,
                 const uint32_t operation,
                 bwin_framebuffer_t srcfb1, const bwin_rect *srcrect1,
                 bwin_framebuffer_t srcfb2, const bwin_rect *srcrect2,
                 const uint32_t pixel1, const uint32_t pixel2);
void bwin_p_draw_ellipse(bwin_framebuffer_t fb, int x, int y,
    unsigned rx, unsigned ry, uint32_t color, uint32_t generic_color, const bwin_rect *cliprect);
void bwin_p_fill_ellipse(bwin_framebuffer_t fb, int x, int y,
    unsigned rx, unsigned ry, uint32_t color, uint32_t generic_color, const bwin_rect *cliprect);

uint32_t bwin_p_convert_color(bwin_framebuffer_t fb, uint32_t color);

void bwin_p_add_font_to_cache(const char *filename, bwin_font_t font);
bwin_font_t bwin_p_get_font_from_cache(bwin_engine_t win, const char *filename,
    int size, bool antialiased);
void bwin_p_remove_font(bwin_font_t font);

/**
Summary:
An enumeration for types of result codes returned by the bwin image api.
**/
typedef enum bwin_result
{
    bwin_result_success = 0,
    bwin_result_file_error,
    bwin_result_allocation_error,
    bwin_result_invalid_argument_error
} bwin_result;

#endif /* BWIN_PRIV_H__ */
