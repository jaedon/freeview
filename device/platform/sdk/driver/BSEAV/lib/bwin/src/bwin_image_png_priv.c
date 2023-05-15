/***************************************************************************
 *     Copyright (c) 2004-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_image_png_priv.c $
 * $brcm_Revision: 4 $
 * $brcm_Date: 7/30/09 12:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_image_png_priv.c $
 * 
 * 4   7/30/09 12:15p vsilyaev
 * PR 57223: Fixed bad use of pointers
 * 
 * 3   2/23/07 1:47p rjlewis
 * PR25895: fix memory leak on out of memory.
 * 
 * 2   8/24/05 6:16p arbisman
 * PR16678: Add support for WinCE
 * 
 * 1   2/7/05 9:05p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/16   2/7/05 12:29p erickson
 * PR13639: palette conversion should to be ARGB, not RGB
 * 
 * Irvine_BSEAVSW_Devel/15   2/7/05 12:07p erickson
 * PR13639: fix palette support by remembering to convert the color_type
 * variable
 * 
 * Irvine_BSEAVSW_Devel/14   1/10/05 5:08p erickson
 * PR13386: don't add alpha channel to png if it's not there, no need.
 * 
 * Irvine_BSEAVSW_Devel/13   1/5/05 4:29p erickson
 * PR13639: removed unneeded code
 * 
 * Irvine_BSEAVSW_Devel/12   1/5/05 3:02p erickson
 * PR13639: made uncompressed_buffer a png-specific param
 * 
 * Irvine_BSEAVSW_Devel/11   12/22/04 5:32p erickson
 * PR13639: image pitch is a private data item, not to be exposed
 * 
 * Irvine_BSEAVSW_Devel/10   12/22/04 5:05p erickson
 * PR13639: reworked image handling internals in preparation for adding
 * jpeg
 * 
 * Irvine_BSEAVSW_Devel/9   12/13/04 12:27p erickson
 * PR13495: added debug
 * 
 * Irvine_BSEAVSW_Devel/8   12/13/04 11:32a erickson
 * PR13495: removed one level of indirection in destroy, added comments
 * related to pixel format and fixed pixel format for palettized formats
 * 
 * Irvine_BSEAVSW_Devel/7   9/9/04 1:53p erickson
 * PR12608: convert dbg from private impl to magnum impl
 * 
 * Irvine_BSEAVSW_Devel/6   2/23/04 11:35a bandrews
 * PR9685: Refactored software blt and scale.
 * 
 * Irvine_BSEAVSW_Devel/5   2/18/04 12:20p erickson
 * PR9685: cleaned up compiler warnings and one minor build error
 * 
 * Irvine_BSEAVSW_Devel/4   2/17/04 7:16p bandrews
 * PR9723: Added 1555 support to bwin_image api
 *
 * Irvine_BSEAVSW_Devel/3   2/6/04 1:45p erickson
 * PR9107: png_malloc/png_free had a memory leak, so I converted to
 * malloc/free, and do a single malloc for all rows instead of a malloc
 * per row
 *
 * Irvine_BSEAVSW_Devel/2   2/3/04 7:56p bandrews
 * PR9107: Added support for pixel formats.
 *
 * Irvine_BSEAVSW_Devel/1   1/30/04 4:42p bandrews
 * PR9107: BWIN Image API
 *
 ****************************************************************/

#include <png.h>
#include "bwin.h"
#include "bwin_image.h"
#include "bwin_image_priv.h"
#include "bwin_image_png_priv.h"
#include <bstd.h>
#include <bkni.h>

#include "bwin_image_debug.h"
BDBG_MODULE(bwin_image_png_priv);

/* png's io read function */
static void bwin_p_png_read(png_structp png_ptr, png_bytep destptr, png_size_t size)
{
	bwin_image_t image = png_get_io_ptr(png_ptr);
	BDBG_ASSERT(image->data->png_ptr == png_ptr);

	BDBG_MSG(("bwin_p_png_read %p %p %d %d", destptr, &image->buffer[image->pos],
		size, image->pos));
	if (image->pos + (int)size > image->size) {
		BDBG_ERR(("read overflow"));
		size = image->size - image->pos;
	}
	/* read from the mem buffer */
	BKNI_Memcpy(destptr, &image->buffer[image->pos], size);
	image->pos += size;
}

bwin_result bwin_image_png_setup(bwin_image_t image)
{
	DBG_ENTER(bwin_image_png_create_struct);

	/* png init calls to get the structure pointers */
	DBG_CALL(png_create_read_struct);
	image->data->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL);
	if (!image->data->png_ptr)
		return bwin_result_allocation_error;

	DBG_CALL(png_create_info_struct);
	image->data->info_ptr = png_create_info_struct(image->data->png_ptr);
	if (!image->data->info_ptr)
	{
		DBG_CALL(png_destroy_read_struct);
		png_destroy_read_struct(&(image->data->png_ptr),
			(png_infopp)NULL, (png_infopp)NULL);
		return bwin_result_allocation_error;
	}

	image->pos = 0;
	png_set_read_fn(image->data->png_ptr, image, bwin_p_png_read);
	return 0;
}

void bwin_image_png_teardown(bwin_image_t image)
{
	DBG_ENTER(bwin_image_png_teardown);

	/* destroy the image data structures */
	png_destroy_read_struct(
		&(image->data->png_ptr),
		&(image->data->info_ptr),
		NULL);
	image->data->png_ptr = NULL;
	image->data->info_ptr = NULL;

	DBG_LEAVE(bwin_image_png_teardown);
}

void bwin_image_png_read_info(bwin_image_t image)
{
    png_uint_32 width, height;
	DBG_ENTER(bwin_image_png_read_info);

	/* load the info structure */
	DBG_CALL(png_read_info);
	png_read_info(image->data->png_ptr, image->data->info_ptr);

	/* get info header */
	DBG_CALL(png_get_IHDR);
	png_get_IHDR(
		image->data->png_ptr,
		image->data->info_ptr,
		&width,
		&height,
		&(image->data->bit_depth),
		&(image->data->color_type),
		NULL, NULL, NULL);

	image->settings.width = width;
	image->settings.height = height;
	BDBG_MSG(("image %dx%x, %d bpp, type %d",
		image->settings.width, image->settings.height,
		image->data->bit_depth, image->data->color_type));

	DBG_LEAVE(bwin_image_png_read_info);
}

void bwin_image_png_set_read_transforms(bwin_image_t image,
	bwin_pixel_format *pixel_format)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_color_8p sig_bit;
	int color_type;
	int bit_depth;

#ifdef FLOAT_SUPPORT
	double  gamma;
#endif

	DBG_ENTER(bwin_image_png_set_read_transforms);

	/* set locals for speed of access */
	png_ptr = image->data->png_ptr;
	info_ptr = image->data->info_ptr;
	color_type = image->data->color_type;
	bit_depth = image->data->bit_depth;
	sig_bit = image->data->sig_bit;

	DBG_TRACE(("Bit depth is %d.", bit_depth));

	/* Transforms are numbered and performed in the order recommended by
	   the libpng maintainers */

	/* 1. convert from palettized to RGB */
	if (color_type == PNG_COLOR_TYPE_PALETTE)
	{
		DBG_TRACE(("Color type is PALETTE, converting to RGB."));
		DBG_CALL(png_set_palette_to_rgb);
		png_set_palette_to_rgb(png_ptr);
		color_type = PNG_COLOR_TYPE_RGB_ALPHA;
	}

	/* 2. convert from gray 1,2,4-bit to 8-bit */
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
	{
		DBG_TRACE(("Color type is GRAY and bit_depth < 8, converting to 8-bit GRAY."));
		DBG_CALL(png_set_gray_1_2_4_to_8);
		png_set_gray_1_2_4_to_8(png_ptr);
	}

	/* 9. if grayscale, convert to rgb */
	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		DBG_TRACE(("Color type is GRAY or GRAY_ALPHA, converting to RGB."));
		png_set_gray_to_rgb(png_ptr);
		color_type = PNG_COLOR_TYPE_RGB;
	}

	/* 3. if a transparency chunk exists, convert to alpha channel */
	DBG_CALL(png_get_valid);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
	{
		DBG_TRACE(("Converting tRNS chunk into alpha channel."));
		DBG_CALL(png_set_tRNS_to_alpha);
		png_set_tRNS_to_alpha(png_ptr);
	}

	/* 4. if bit depth is 16, convert to 8 */
	if (bit_depth == 16)
	{
		DBG_TRACE(("Stripping depth from 16-bits to 8-bits."));
		DBG_CALL(png_set_strip_16);
		png_set_strip_16(png_ptr);
	}

	/* 5. if bit depth is less than 8, pack to 8 */
	if (bit_depth < 8)
	{
		DBG_TRACE(("Setting packing to 8 bits."));
		DBG_CALL(png_set_packing);
		png_set_packing(png_ptr);
	}

	/* 6. convert to original bitness, i.e. 5-6-5 */
	DBG_CALL(png_get_sBIT);
	if (png_get_sBIT(png_ptr, info_ptr, &image->data->sig_bit))
	{
		DBG_TRACE(("Significant index/color/gray bits: (%d, %d, %d, %d)",
			sig_bit->red, sig_bit->green, sig_bit->blue,
			sig_bit->gray));
		DBG_TRACE(("Shifting data to original bitness"));
		DBG_CALL(png_set_shift);
		png_set_shift(png_ptr, image->data->sig_bit);
	}

	/* 7. select the resulting pixel format, either RGB or ARGB. 
	don't add alpha because that adds more processing time. */
    if (color_type == PNG_COLOR_TYPE_RGB)
    {
		*pixel_format = bwin_pixel_format_r8_g8_b8;
    }
	else {
		*pixel_format = bwin_pixel_format_a8_r8_g8_b8;
	}

	/* 8. if RGBA, convert to ARGB */
	BDBG_MSG(("color_type == %d", color_type));
	if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
	{
		DBG_TRACE(("Color type is RGBA, converting to ARGB."));
		DBG_CALL(png_set_swap_alpha);
		png_set_swap_alpha(png_ptr);
	}

#ifdef FLOAT_SUPPORT
	/* 10. Setting gamma to average values */
	DBG_TRACE(("Setting gamma."));
	DBG_CALL(png_get_gAMA);
	if (png_get_gAMA(png_ptr, info_ptr, &gamma))
	{
		DBG_TRACE(("Using file-supplied gamma."));
		DBG_CALL(png_set_gamma);
		png_set_gamma(png_ptr, 2.2, gamma);
	}
	else
	{
		DBG_TRACE(("Using average gamma."));
		DBG_CALL(png_set_gamma);
		png_set_gamma(png_ptr, 2.2, 0.45455);
	}
#endif

	/* update info struct for correct row bytes, etc. */
	DBG_CALL(png_read_update_info);
	png_read_update_info(png_ptr, info_ptr);

	/* get row bytes for allocation */
	DBG_CALL(png_get_rowbytes);
	image->data->row_bytes = png_get_rowbytes(png_ptr, image->data->info_ptr);

	image->pitch = image->data->row_bytes;

	DBG_LEAVE(bwin_image_png_set_read_transforms);
}

bwin_result bwin_image_png_create_buffer(bwin_image_t image)
{
	unsigned int i;
	png_structp png_ptr;
	bwin_result rc = bwin_result_success;

	DBG_ENTER(bwin_image_png_create_buffer);

	/* make local png_ptr for repeated access */
	png_ptr = image->data->png_ptr;

	DBG_TRACE(("Image pitch = %d.", image->pitch));

	/* allocate row_pointers */
	BDBG_ASSERT(!image->data->row_pointers);
	image->data->row_pointers = BKNI_Malloc(image->settings.height * sizeof(png_bytep));
	if (!image->data->row_pointers) {
		goto error;
	}
	BDBG_MSG(("image->data->row_pointers %p %d", image->data->row_pointers, image->settings.height * sizeof(png_bytep)));

	/* allocate entire buffer at once */
	BDBG_ASSERT(!image->data->uncompressed_buffer);
	image->data->uncompressed_buffer = BKNI_Malloc(image->pitch * image->settings.height);
	if (!image->data->uncompressed_buffer) {
		BKNI_Free(image->data->row_pointers);
		image->data->row_pointers = NULL;
		goto error;
	}
	BDBG_MSG(("uncompressed_buffer %p %d", image->data->uncompressed_buffer, image->pitch * image->settings.height));

	/* fill out the row_pointers structure */
	for (i = 0; i < image->settings.height; i++)
		image->data->row_pointers[i] = &image->data->uncompressed_buffer[image->pitch * i];

	DBG_LEAVE(bwin_image_png_create_buffer);
	return rc;
error:
	return bwin_result_allocation_error;
}

void bwin_image_png_destroy_buffer(bwin_image_t image)
{
	DBG_ENTER(bwin_image_png_destroy_buffer);

	if (image->data->row_pointers)
	{
		BKNI_Free(image->data->row_pointers);
		image->data->row_pointers = NULL;
	}

	if (image->data->uncompressed_buffer)
	{
		BKNI_Free(image->data->uncompressed_buffer);
		image->data->uncompressed_buffer = NULL;
	}

	DBG_LEAVE(bwin_image_png_destroy_buffer);
}

