/***************************************************************************
 *     Copyright (c) 2004-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_image.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 12/5/11 12:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/include/bwin_image.h $
 * 
 * 5   12/5/11 12:22p erickson
 * SW7405-5560: extend bwin to support no-blend image loading
 * 
 * 4   8/25/06 1:26p tokushig
 * PR22347: in non-skinned brutus, images w/o embedded thumbnails will
 * display full image as thumb.  if using a skin, small images w/o
 * embedded thumbnails will display full image as thumb but large images
 * w/o embedded thumbnails will display a placeholder icon instead
 * (images are currently considered large if they have more than 640 *
 * 480 = 307200pixels.)  thumbnail please wait popup was replaced by
 * sequential drawing of thumbs which provides better feedback to users.
 * implemented image caching on thumbs (default: last 30) and for full
 * screen image view (default: last 3).
 * 
 * SanDiego_Brutus_Skin/5   8/23/06 5:05p tokushig
 * added return code to bwin_image_render_thumb()
 * 
 * SanDiego_Brutus_Skin/4   5/16/06 10:17a tokushig
 * Merge from main
 * 
 * SanDiego_Brutus_Skin/3   5/4/06 11:51a tokushig
 * added functions to draw image thumbnails using EXIF data if available
 * 
 * 2   4/17/06 5:08p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/2   4/4/06 11:13a tokushig
 * add function to only open file pointer during image loading.  the
 * entire compressed image is not saved in memory.  this will require an
 * image decompressor that can access the file directly.
 * 
 * SanDiego_Brutus_Skin/1   3/7/06 2:28p tokushig
 * added proportional maximizing, and proportional downscale only for
 * images
 * 
 * 1   2/7/05 9:03p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/10   1/5/05 2:59p erickson
 * PR13639: removed bwin_result from public api
 * 
 * Irvine_BSEAVSW_Devel/9   12/22/04 5:31p erickson
 * PR13639: removed pitch from bwin_image_settings
 * 
 * Irvine_BSEAVSW_Devel/8   12/22/04 5:04p erickson
 * PR13639: added bwin_image_close
 * 
 * Irvine_BSEAVSW_Devel/7   2/25/04 11:35a bandrews
 * PR9882: Added invalid argument return code.
 * 
 * Irvine_BSEAVSW_Devel/6   2/17/04 7:15p bandrews
 * PR9723: Added 1555 support to bwin_image api
 * 
 * Irvine_BSEAVSW_Devel/5   2/3/04 10:30a bandrews
 * PR9107: Fixed api calls to bwin_image_*.
 * 
 * Irvine_BSEAVSW_Devel/4   1/30/04 6:58p bandrews
 * PR9107: BWIN Image API
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
#ifndef BWIN_IMAGE_H__
#define BWIN_IMAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <bstd.h>
#include <bkni.h>

/**
Summary:
An opaque handle for an image.
**/
typedef struct bwin_image *bwin_image_t;

/**
Summary:
Load an image from a file.
Description:
This loads the entire compressed file into memory until bwin_image_close
is called.
**/
bwin_image_t bwin_image_load(
    bwin_engine_t win,
    const char *filename);      /* the name of the file to load */

/**
Summary:
Load an image from a file.
Description:
This opens the image file and saves the file pointer.  Unlike bwin_image_load(),
the image is not read into memory.  This function is useful when a 
particular image decompressor accesses the file directly.
**/
bwin_image_t bwin_image_file_io(
    bwin_engine_t win,
    const char *filename);      /* the name of the file to load */

/**
Summary:
Load an image from memory.
Description:
This does not copy the memory but assumes it is available until bwin_image_close
is called.
**/
bwin_image_t bwin_image_load_memory(
    bwin_engine_t win,
    const void *mem,            /* a pointer to the memory from stores the
                                compressed image */
    unsigned length);           /* the length of the image in memory */


/**
Summary:
Close an image and invalidate the handle.
Description:
This frees any memory allocated by bwin_image_load.
**/
void bwin_image_close(bwin_image_t image);

/* Rendering modes:
   Single: draw a single image in the dest rect.
   Stretch: stretch a single image to fit the dest rect
   Tile: tile a single image multiple times horizontally and vertically
    to fit the dest rect. */
typedef enum
{
    bwin_image_render_mode_single, /* no stretch, but blend using alpha-per-pixel */
    bwin_image_render_mode_stretch,
    bwin_image_render_mode_tile,
    bwin_image_render_mode_maximize,
    bwin_image_render_mode_maximize_down,
    bwin_image_render_mode_single_load /* like single, but don't blend using alpha-per-pixel */
} bwin_image_render_mode;

/**
Summary:
Draw an image into a window.
**/
void bwin_image_render(
    bwin_t window,
    bwin_image_t image,
    bwin_image_render_mode mode,/* rendering mode: single, stretch, tile */
    const bwin_rect *destrect,  /* Optional window-coordinates for the image.
                                If NULL, draw at 0,0 using the whole window. */
    const bwin_rect *srcrect,   /* Optional image-coordinates for the source.
                                If NULL, draw the entire image. */
    const bwin_rect *cliprect   /* Optional clip rect */
    );

/**
Summary:
Draw a thumbnail image into a window.
 returns 1 on success, 0 otherwise
**/
int bwin_image_render_thumb(
    bwin_t window, bwin_image_t image,
    bwin_image_render_mode mode,/* rendering mode: single, stretch, tile */
    const bwin_rect *destrect,  /* Optional window-coordinates for the image.
                                If NULL, draw at 0,0 using the whole window. */
    const bwin_rect *cliprect   /* Optional clip rect */
    );

/**
Summary:
A data type for retrieving information about the image
**/
typedef struct
{
    unsigned width; /* width of uncompressed, unscaled image */
    unsigned height; /* height of uncompressed, unscaled image */
    unsigned thumb_width; /* width of uncompress, unscaled thumbnail image */
    unsigned thumb_height; /* height of uncompressed, unscaled thumbnail image */
} bwin_image_settings;

/**
Summary:
Get the image settings from the image.
**/
void bwin_image_get_settings(bwin_image_t image,
    bwin_image_settings *settings);

#ifdef __cplusplus
}
#endif

#endif /* BWIN_IMAGE_H__ */

