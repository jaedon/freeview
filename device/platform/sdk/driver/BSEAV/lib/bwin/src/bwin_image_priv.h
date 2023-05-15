/***************************************************************************
 *     Copyright (c) 2004-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_image_priv.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 12/5/11 12:22p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_image_priv.h $
 * 
 * 4   12/5/11 12:22p erickson
 * SW7405-5560: extend bwin to support no-blend image loading
 * 
 * 3   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 * 
 * SanDiego_Brutus_Skin/3   5/16/06 10:17a tokushig
 * Merge from main
 * 
 * SanDiego_Brutus_Skin/2   5/4/06 2:34p tokushig
 * added filename to bwin image structure in support of libexif
 * 
 * 2   4/17/06 5:08p jgarrett
 * PR 20951: Merging skinned UI into mainline
 * 
 * SanDiego_Brutus_Skin/1   3/28/06 3:13p tokushig
 * added file pointer for bwin images.  this is used for image types that
 * should be read directly from the file (by the decompressor).
 * 
 * 1   2/7/05 9:05p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/11   1/10/05 4:09p erickson
 * PR13386: rework for image-rendering changs
 * 
 * Irvine_BSEAVSW_Devel/10   1/6/05 4:11p erickson
 * PR13639: moved shared rendering code into bwin_image and fixed some
 * clipping problems
 * 
 * Irvine_BSEAVSW_Devel/9   1/5/05 4:27p erickson
 * PR13639: added const
 * 
 * Irvine_BSEAVSW_Devel/8   1/5/05 3:01p erickson
 * PR13639: removed unneeded code
 * 
 * Irvine_BSEAVSW_Devel/7   12/22/04 5:33p erickson
 * PR13639: image pitch is a private data item, not to be exposed
 * 
 * Irvine_BSEAVSW_Devel/6   12/22/04 5:05p erickson
 * PR13639: reworked image handling internals in preparation for adding
 * jpeg
 * 
 * Irvine_BSEAVSW_Devel/5   2/25/04 11:37a bandrews
 * PR9882: Checked the return codes while trying to read from a file. PNG
 * ignores these, but we don't have to.
 * 
 * Irvine_BSEAVSW_Devel/4   2/23/04 11:35a bandrews
 * PR9685: Refactored software blt and scale.
 * 
 * Irvine_BSEAVSW_Devel/3   2/17/04 7:16p bandrews
 * PR9723: Added 1555 support to bwin_image api
 * 
 * Irvine_BSEAVSW_Devel/2   2/3/04 7:53p bandrews
 * PR9107: Moved rendering to here, since most of rendering isn't format-
 * specific.
 * 
 * Irvine_BSEAVSW_Devel/1   1/30/04 4:42p bandrews
 * PR9107: BWIN Image API
 *
 ****************************************************************/

#ifndef BWIN_IMAGE_PRIV_H__
#define BWIN_IMAGE_PRIV_H__

#include "bwin.h"
#include "bwin_priv.h"
#include "bwin_image.h"
#include "bwin_transform.h"

/* Memory scheme
1. bwin_image_load_file loads compressed image into memory, then calls bwin_image_load_memory
2. bwin_image_load_memory reads format information
3. bwin_image_render handles all clipping, tiling/stretch modes, and conversion to global
   coordinates. Then it calls the image-specific render function.
4. The image-specific render function uncompressed the image line-by-line
    and calls the line renderer.
3. close will free memory
*/

/**
Summary:
Opaque type-specific data pointer.  Each implementation of an image format
will have its own private struct defining this.
**/
typedef struct bwin_image_data *bwin_image_data_t;

/**
Summary:
A private implementation of the image structure for bwin.
**/
struct bwin_image
{
    bwin_engine_t win;                          /* pointer to bwin engine */
    bwin_image_settings settings;               /* image settings */

    const unsigned char *buffer;                /* compressed buffer */
    int size;                                   /* size of buffer */
    int pos;                                    /* current read position in buffer */
    bool allocated;                             /* if true, then we must free buffer */

    const unsigned char *srcrow_buffer;         /* current uncompressed row */
    int srcrow;                                 /* index for srcrow_buffer, if -1,
                        then srcrow_buffer is invalid. */
    int pitch;                                  /* pitch of uncompressed row */

    bwin_image_data_t data;                     /* type-specific data pointer */

    FILE * fpImage;                             /* if valid, renderer is to access image file directly */
    char filename[256];                         /* only valid if fpImage is valid */
    bwin_image_render_mode render_mode;         /* current render mode */

    /* loads settings */
    bwin_result (*init)(bwin_image_t image);

    /* image-specific renderer */
    void (*render)(bwin_framebuffer_t fb,
        bwin_image_t image,
        const bwin_rect *destrect,
        const bwin_rect *srcrect,
        const bwin_rect *clipdestrect);

    /* clean up after init and possible render */
    void (*finalize)(bwin_image_t image);
};

/**
Callback from bwin_image_p_render_lines for retrieving a line of src
**/
typedef int (*bwin_image_get_src_line_func)(
    bwin_image_t image, 
    int srcrow /* the line to retrieve */
    );

/**
 A generic line-based rendering algorithm. This must be called from
each modules render rountine.
**/
void bwin_image_p_render_lines(
    bwin_framebuffer_t fb,
    bwin_image_t image,
    const bwin_rect *destrect,
    const bwin_rect *srcrect,
    const bwin_rect *clipdestrect,
    const bwin_image_transform_settings *transform_settings, 
    bwin_image_get_src_line_func get_src_line
    );

#endif /* BWIN_IMAGE_PRIV_H__ */
