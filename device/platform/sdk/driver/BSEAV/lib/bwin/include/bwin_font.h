/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_font.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 6/11/10 11:53a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/include/bwin_font.h $
 * 
 * 4   6/11/10 11:53a gmohile
 * SW3548-2979 : Add support for variable length utf8 and opening freetype
 * fonts from memory
 * 
 * 3   2/17/10 5:42p erickson
 * SW3548-2788: add bwin_open_rendered_font for compiled-in font support
 *
 * 2   2/16/05 9:14a erickson
 * PR13387: added notes on how to do antialiased fonts with palettized
 * color
 *
 * 1   2/7/05 9:03p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/3   1/29/04 2:27p erickson
 * PR9107: merging back from BRUTUS_PR9107 branch for bwin development
 *
 * Irvine_BSEAVSW_Devel/2   1/19/04 8:59a erickson
 * PR9107: switching to BRUTUS_PR9107 branch
 *
 * Irvine_BSEAVSW_Devel/1   1/14/04 11:36p erickson
 * PR9107: freetype support added to iolaus and still doing initial
 * development
 *
 ****************************************************************/
#ifndef BWIN_FONT_H__
#define BWIN_FONT_H__

#include "bwin.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bwin_font *bwin_font_t;

/**
Summary:
**/
bwin_font_t bwin_open_font(
    bwin_engine_t win,
    const char *filename,   /* Filename of the font. */
    int size, /* point size. If -1, then this is a rendered font, not a truetype font. */
    bool antialiased    /* if true, blend the edges with the background.
                            If size is -1, then this is ignored. */
    );

/**
Summary:
Save a rendered font into a bwin format.
**/
int bwin_save_rendered_font(bwin_font_t font, const char *filename);

/**
Summary:
You must close any font which you open.
**/
void bwin_close_font(bwin_font_t font);

/**
Summary:
Get the recommended baseline-to-baseline height for spacing text.
**/
void bwin_get_font_height(bwin_font_t font, unsigned *height);

/**
Summary:
Calculate how large text would be if rendered.
Return Values:
0 is succesful.
-1 is a character cannot be loaded.
**/
int bwin_measure_text(bwin_font_t font, const char *text,
    int len, /* if -1, use strlen(text) */
    int *width,     /* total width in pixels of the text */
    int *height,    /* total height in pixels of the text */
    int *base       /* # of pixels from the top to the baseline (e.g. ascender) */
    );

/**
Summary:
Draw text into a window.

Description:
If you are using a palettized pixel format, you can still do antialiased fonts,
but it requires a special technique and a code modification. The recommended method
is that the colors following the text color be arranged to blend with the background.
As an example, let's say that your text color blue is index 5 and you want to
anti-alias with a white background. Then, the following should appear in your palette:
o index 5 = blue
o index 6 = 2/3 blue + 1/3 white
o index 7 = 1/3 blue + 2/3 white
o index 8 = white
To make this work, please see antialias_palette8_pixels in bwin_font.c.
**/
int bwin_draw_text(bwin_t window, bwin_font_t font,
    int x, int y,
    const char *text,
    int len, /* if -1, use strlen(text) */
    uint32_t color,
    const bwin_rect *cliprect   /* optional cliprect, in window coordinates */
    );

typedef size_t (*bwin_readfn_t)(void *buffer, size_t size, size_t number, void *handle);

bwin_font_t bwin_open_rendered_font(
    bwin_engine_t win,
    bwin_readfn_t readfn,
    void *context
    );

typedef struct  {
	  void *context;
	  bwin_readfn_t readfn;
	  int size;
	  bool antialiased;
	  unsigned buf_len;
} bwin_font_settings;

void bwin_get_default_font_settings(bwin_font_settings *settings);
bwin_font_t bwin_open_font_generic(bwin_engine_t win, bwin_font_settings *settings);
#ifdef __cplusplus
}
#endif

#endif /* BWIN_FONT_H__*/
