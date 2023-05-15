/***************************************************************************
 *     Copyright (c) 2004-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_image_png.c $
 * $brcm_Revision: 10 $
 * $brcm_Date: 4/1/09 6:23p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_image_png.c $
 * 
 * 10   4/1/09 6:23p mward
 * PR47980: Eliminate compiler warning for signed, unsigned compare.
 * 
 * 9   12/19/08 2:52p erickson
 * PR47980: protect png from buffer overrun
 *
 * 8   2/28/07 11:22a gmohile
 * PR 25109: fix logic which checks if bwin_load failed.
 *
 * 6   6/30/06 2:51p erickson
 * PR21941: fix warning
 *
 * 5   5/31/06 4:19p tokushig
 * PR21891: Merging updated picture viewer into mainline.
 *
 * SanDiego_Brutus_Skin/2   5/16/06 11:33a tokushig
 * merge from main
 *
 * 3   4/17/06 5:08p jgarrett
 * PR 20951: Merging skinned UI into mainline
 *
 * SanDiego_Brutus_Skin/1   3/28/06 2:48p tokushig
 * when determining if a given image is a png, we must check to determine
 * whether the given image resides in a memory buffer or in a file.
 *
 * 2   3/25/05 3:10p erickson
 * PR14593: added DECOMPRESS_ONCE option for performance increase at cost
 * of memory usage
 *
 * 1   2/7/05 9:05p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 *
 * Irvine_BSEAVSW_Devel/12   1/10/05 5:08p erickson
 * PR13386: don't add alpha channel to png if it's not there, no need.
 *
 * Irvine_BSEAVSW_Devel/11   1/10/05 4:09p erickson
 * PR13386: rework for image-rendering changs
 *
 * Irvine_BSEAVSW_Devel/10   1/6/05 4:11p erickson
 * PR13639: moved shared rendering code into bwin_image and fixed some
 * clipping problems
 *
 * Irvine_BSEAVSW_Devel/9   1/5/05 4:29p erickson
 * PR13639: removed unneeded code
 *
 * Irvine_BSEAVSW_Devel/8   1/5/05 3:02p erickson
 * PR13639: converted png render to use new line-based logic
 *
 * Irvine_BSEAVSW_Devel/7   12/22/04 5:32p erickson
 * PR13639: image pitch is a private data item, not to be exposed
 *
 * Irvine_BSEAVSW_Devel/6   12/22/04 5:05p erickson
 * PR13639: reworked image handling internals in preparation for adding
 * jpeg
 *
 * Irvine_BSEAVSW_Devel/5   12/13/04 11:31a erickson
 * PR13495: added io funcs
 *
 * Irvine_BSEAVSW_Devel/4   9/9/04 1:53p erickson
 * PR12608: convert dbg from private impl to magnum impl
 *
 * Irvine_BSEAVSW_Devel/3   4/1/04 7:02p bandrews
 * PR10422: Mostly removed c++ style comments
 *
 * Irvine_BSEAVSW_Devel/2   2/3/04 7:55p bandrews
 * PR9107: Moved rendering to another file. Added pixel_at.
 *
 * Irvine_BSEAVSW_Devel/1   1/30/04 4:42p bandrews
 * PR9107: BWIN Image API
 *
 ****************************************************************/

#include <png.h>

#include "bwin.h"
#include "bwin_priv.h"
#include "bwin_image.h"
#include "bwin_image_priv.h"
#include "bwin_image_png_priv.h"

#include "bwin_image_debug.h"
BDBG_MODULE(bwin_image_png);

static bwin_result bwin_image_png_init(bwin_image_t image)
{
	struct bwin_image_data *png;
    bwin_result rc = 0;

    DBG_ENTER(bwin_image_png_init);

	/* allocate png-specific data structure memory */
	png = (struct bwin_image_data *)BKNI_Malloc(sizeof(struct bwin_image_data));
	if (!png)
		return bwin_result_allocation_error;
	memset(png, 0, sizeof(struct bwin_image_data));
	image->data = png;

    /* need these here for getting image info before we render, without
    loading the image data itself due to memory constraints */

    /* setup png stuff */
    rc = bwin_image_png_setup(image);
    if (!rc) {
        /* read the info and then discard everything */
        bwin_image_png_read_info(image);
        bwin_image_png_teardown(image);
    }

    DBG_LEAVE(bwin_image_png_init);
    return rc;
}

static void bwin_image_png_finalize(bwin_image_t image)
{
    BKNI_Free(image->data);
    image->data = NULL;
}

static bwin_result bwin_image_png_load(bwin_image_t image,
    bwin_pixel_format *pixel_format)
{
    bwin_result rc = 0;

    /* set up the data structures and source pointers */
    rc = bwin_image_png_setup(image);
    if (rc) return rc;

    /* read the image info */
    bwin_image_png_read_info(image);

    /* set up read transformations   */
    bwin_image_png_set_read_transforms(image, pixel_format);

    /* create a buffer to store the image */
    rc = bwin_image_png_create_buffer(image);

    /* check if we got a buffer or ran out of heap space */
    if (!rc) /* no problem with buffer */
    {
        /* read the image into the buffer */
        DBG_CALL(png_read_image);
        png_read_image(image->data->png_ptr, image->data->row_pointers);

        /* finish reading, don't care about end data */
        DBG_CALL(png_read_end);
        png_read_end(image->data->png_ptr, NULL);
    }
    else /* problem with allocation */
    {
        /* make sure the image doesn't try to render */
        image->render = NULL;
    }

    /* kill the data structures and source pointers */
    /* since we don't need them after we've copied the image */
    /* to the buffer */
    bwin_image_png_teardown(image);

    DBG_LEAVE(bwin_image_png_load);
    return rc;
}

static bwin_result bwin_image_png_unload(bwin_image_t image)
{
    bwin_image_png_destroy_buffer(image);
    return 0;
}


static int get_png_src_line(bwin_image_t image, int srcrow)
{
    if (image->srcrow != srcrow) {
        image->srcrow = srcrow;
        image->srcrow_buffer = image->data->uncompressed_buffer + (image->pitch * image->srcrow);

        /* ensure here that we aren't going to overrun because of bad metadata */
        if (image->srcrow_buffer + image->pitch - image->data->uncompressed_buffer > (int)(image->pitch * image->settings.height)) {
            image->srcrow_buffer = image->data->uncompressed_buffer;
        }
    }
    return 0;
}

/* render from the compressed buffer to the framebuffer */
static void bwin_image_png_render(
    bwin_framebuffer_t fb,
    bwin_image_t image,
    const bwin_rect *destrect,
    const bwin_rect *srcrect,
    const bwin_rect *clipdestrect /* guaranteed to be inside destrect */
    )
{
    bwin_image_transform_settings transform_settings;
    bwin_pixel_format pixel_format;

    /* This loads the entire image into an uncompressed buffer. We could do it line-based,
    but that would require we handle interlacing. I don't know how many interlaced
    png's are out there, but that's the issue. */
#ifdef DECOMPRESS_ONCE
    if (image->data->row_pointers) {
        pixel_format = image->data->stored_pixel_format;
    }
    else {
        if (bwin_image_png_load(image, &pixel_format)) goto error;
        image->data->stored_pixel_format = pixel_format;
    }
#else
    if (bwin_image_png_load(image, &pixel_format)) goto error;
#endif

    bwin_image_transform_settings_init(&transform_settings,
        pixel_format, fb->settings.pixel_format);

    bwin_image_p_render_lines(fb, image, destrect, srcrect, clipdestrect,
        &transform_settings, get_png_src_line);

error:
#ifndef DECOMPRESS_ONCE
    bwin_image_png_unload(image);
#endif
}

int bwin_image_p_is_png(bwin_image_t image)
{
    int rc = 0;
#define PNG_BYTES_TO_CHECK 4
    if (image->size < PNG_BYTES_TO_CHECK)
        return -1;

    if (image->fpImage)
    {
        /* decompressor will read image directly from file
           so use file handle to determine if it is a jpeg */
        char buf[PNG_BYTES_TO_CHECK];
        fpos_t posOrig;
        BKNI_Memset(buf, 0, sizeof(buf));
        fgetpos(image->fpImage, &posOrig);
        rewind(image->fpImage);
        fread(buf, sizeof(*buf), sizeof(buf), image->fpImage);
        rc = png_sig_cmp((void*)buf, 0, PNG_BYTES_TO_CHECK);
        fsetpos(image->fpImage, &posOrig);
    }
    else
    if (image->buffer)
    {
        /* decompressor will read image from memory
           so use buffer to determine if it is a jpeg */
        rc = png_sig_cmp((void*)image->buffer, 0, PNG_BYTES_TO_CHECK);
    }

    if (!rc) {
        image->init = bwin_image_png_init;
        image->render = bwin_image_png_render;
        image->finalize = bwin_image_png_finalize;
    }
    return rc;
}
