/**
	@file     port_itk_bitmap.c
	@brief    ITK Host Display 중에서 DFB에서 사용하는 bitmap을 위한 모듈.

	Description: S&T에서 제공한 모듈임.	\n
	Module: mw/mheg/itk												\n
	Copyright (c) 2008 HUMAX Co., Ltd.									\n
	All rights reserved.													\n
*/
#define __________________________JPEG____________________________________________
/*
 * Copyright (c) 2008 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 *
 */

/**
 * Example implementation of code to create a itk_bitmap_t from JPEG source
 * using the reference JPEG decoder library
 *
 * Note that the code may not be suitable for use without adaptation. In
 * particular note that the code uses the malloc family of heap allocators
 * which may need to be adapted for use on the target platform.
 */

/** JPEG support layer. See file 'example.c' in JPEG source distribution. */

#include <stdio.h> /* libjpeg header requires this for e.g. FILE, which may cause
                      problems on embedded platforms */
//#include <setjmp.h>
#include <png.h>
#include <jpeglib.h>
#include <jerror.h>
#include <itk_console.h>

#include "port_itk_main.h"
#include "port_itk_display.h"

struct jjpeg_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */

	jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct jjpeg_error_mgr * jjpeg_error_ptr;

static void jjpeg_error_exit(j_common_ptr jinfo)
{
	/* jinfo->err really points to a jjpeg_error_mgr struct, so coerce pointer */
	jjpeg_error_ptr jerr = (jjpeg_error_ptr)jinfo->err;
	/* We could postpone this until after returning, if we chose. */
	(*jinfo->err->output_message)(jinfo);
	/* Return control to the setjmp point */
	longjmp(jerr->setjmp_buffer, 1);
}

/** Custom error handling. */
static void jjpeg_output_message(j_common_ptr jinfo)
{
	char buffer[JMSG_LENGTH_MAX];
	/* jinfo->err really points to a jjpeg_error_mgr struct, so coerce pointer */
	jjpeg_error_ptr jerr = (jjpeg_error_ptr)jinfo->err;

	/* Create the error message */
	(*jerr->pub.format_message)(jinfo, buffer);
	/* Output a diagnostic error */
	//WARNING_LOG(jerr->diagnostic, "\t%s\n", buffer);
}


static void jjpeg_manage_error(j_decompress_ptr jinfo, jjpeg_error_ptr jerr)
{
	/* Set up default error handling, then override the output message and
	error exit handling. */
	jinfo->err = jpeg_std_error(&jerr->pub);
	jerr->pub.output_message = jjpeg_output_message;
	jerr->pub.error_exit = jjpeg_error_exit;
}

/** Custom in-memory data source.
    Googling jpeg "fill_input_buffer" found me this at
    http://www.smalleranimals.com/vforum/archive/index.php?t-1922.html
 */
/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */
static void jjpeg_init_source (j_decompress_ptr jinfo)
{
	/* No work, since jjpeg_memory_src set up the buffer pointer and count.
	 * Indeed, if we want to read multiple JPEG images from one buffer,
	 * this *must* not do anything to the pointer.
	 */
	(void)(jinfo);
}

/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 * For our in-memory data source, this function gets called if the
 * decompressor has overrun the end of the input buffer, implying we supplied
 * an incomplete or corrupt JPEG datastream.
 *
 * Note that an alternative strategy, to supply some sort of output image
 * no matter how corrupted, would also be possible. Generally, if there are
 * problems, we prefer to show nothing to the user.
 */
static boolean jjpeg_fill_input_buffer (j_decompress_ptr jinfo)
{
	jjpeg_error_exit((j_common_ptr)jinfo);

	return 0; /* Shouldn't get here! */
}

/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 * If we overrun the end of the buffer, we let fill_input_buffer deal with
 * it.  An extremely large skip could cause some time-wasting here, but
 * it really isn't supposed to happen ... and the decompressor will never
 * skip more than 64K anyway.
 */
static void jjpeg_skip_input_data (j_decompress_ptr jinfo, long num_bytes)
{
	struct jpeg_source_mgr * src = jinfo->src;

	if (num_bytes > 0)
	{
		long const can_skip = (long)src->bytes_in_buffer;
		long const skip = (num_bytes <= can_skip) ? num_bytes : can_skip ;
		src->next_input_byte += (size_t) skip;
		src->bytes_in_buffer -= (size_t) skip;
	}
}

/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */
static void jjpeg_term_source (j_decompress_ptr jinfo)
{
	/* no work necessary here */
	(void)(jinfo);
}

/*
 * Prepare for input from a memory buffer.
 */

static void jjpeg_memory_src(j_decompress_ptr jinfo, JOCTET *buffer, size_t bytes)
{
	struct jpeg_source_mgr * src;
	jpeg_stdio_src(jinfo, NULL);

	/* We set up the normal JPEG source routines. */
	src = jinfo->src;

	src->init_source = jjpeg_init_source;
	src->fill_input_buffer = jjpeg_fill_input_buffer;
	src->skip_input_data = jjpeg_skip_input_data;
	src->resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->term_source = jjpeg_term_source;

	src->next_input_byte = buffer;
	src->bytes_in_buffer = bytes;
}

/**
 * Sets the bitmap resolution as defined in the FreeSAT graphics model.
 */
static void jjpeg_set_resolution(j_decompress_ptr jinfo, itk_bitmap_t *bitmap)
{
	enum { dots_per_inch = 1, dots_per_cm = 2 };

	// Set for default Standard Def
	bitmap->pixel_resolution_w = 720;
	bitmap->pixel_resolution_h = 576;

	// Then overwrite if it matches one of the valid Hi-Def densities
	if (jinfo->saw_JFIF_marker)
	{
		UINT8 const units = jinfo->density_unit;
		UINT16 const xd = jinfo->X_density;
		UINT16 const yd = jinfo->Y_density;

		if ((units == dots_per_inch && xd == 90 && yd == 90) ||
			(units == dots_per_cm && xd == 35 && yd == 35))
		{
			bitmap->pixel_resolution_w = 1280;
			bitmap->pixel_resolution_h = 720;
		}
		else if ((units == dots_per_inch && xd == 135 && yd == 135) ||
			(units == dots_per_cm && xd == 53 && yd == 53))
		{
			bitmap->pixel_resolution_w = 1920;
			bitmap->pixel_resolution_h = 1080;
		}
	}
}

/**
 * Allocates workspace for the in memory version of the decompressed JPEG.
 */
static boolean jjpeg_alloc_workspace(j_decompress_ptr jinfo,
					itk_pixel_format_t format,
					itk_bitmap_t **bitmap,
					itk_uint8_t **scanline)
{
	boolean rc = 1;
	ITK_bitmap_funcs_t * f = (ITK_bitmap_funcs_t *)jinfo->client_data;
	*bitmap = f->createBitmap
		(f, jinfo->output_width, jinfo->output_height, format);
	*scanline = (itk_uint8_t *)f->alloc
		(f, jinfo->output_width * jinfo->output_components);

	if (*bitmap == NULL || *scanline == NULL)
	{
		rc = 0;
	}
	else
	{
		jjpeg_set_resolution(jinfo, *bitmap);
	}

	return rc;
}

/**
 * Unpacks a single JPEG scanline into our internal picture buffer.
 */
static void jjpeg_unpack_scanline(itk_int32_t width,
					itk_pixel_format_t format,
					itk_uint8_t const * scan,
					itk_uint8_t * pixels)
{
	switch (format)
	{
	case ITK_FB_LUT8:
		/* we can not handle palette based bitmaps */
	default:
		break;
	case ITK_FB_RGBA4444:
		{
			itk_uint16_t *pixels16 = (itk_uint16_t *)pixels;

			for ( ; width-- != 0 ; scan += 3)
			{
				*pixels16 = (scan[0] & 0xf0) << 8 | (scan[1] & 0xf0) << 4 | (scan[2] & 0xf0) | 0x0f;
				pixels16++;
			}
		}
		break;
	case ITK_FB_ARGB4444:
		{
			itk_uint16_t *pixels16 = (itk_uint16_t *)pixels;

			for ( ; width-- != 0 ; scan += 3)
			{
				*pixels16 = (0xf0 << 8) | (scan[0] & 0xf0) << 4 | (scan[1] & 0xf0) | (scan[2] >> 4);
				pixels16++;
			}
		}
		break;
	case ITK_FB_RGBA8888:
		{
			itk_uint32_t *pixels32 = (itk_uint32_t *)pixels;

			while (width-- != 0)
			{
				*pixels32 = (scan[0] << 24) | (scan[1] << 16) | (scan[2] << 8) | 0xff;
				pixels32++;

				scan += 3;
			}
		}
		break;
	case ITK_FB_ARGB8888:
		{
			itk_uint32_t *pixels32 = (itk_uint32_t *)pixels;

			while (width-- != 0)
			{
				*pixels32 = (0xff << 24) | (scan[0] << 16) | (scan[1] << 8) | scan[2];
				pixels32++;

				scan += 3;
			}
		}
		break;
	}
}

/**
 * Decompress the JPEG saving pixels to the picture buffer.
 */
static void jjpeg_decompress_jpeg(j_decompress_ptr jinfo, itk_bitmap_t *bitmap, itk_uint8_t *scanline)
{
	itk_uint8_t *pixels = bitmap->bitmap;	// rjlee : pixels가 surface의 buffer 이다
	itk_int32_t const width = bitmap->width;
	itk_int32_t const pitch = bitmap->pitch;
	itk_pixel_format_t format = bitmap->format;
	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/
	while (jinfo->output_scanline < jinfo->output_height)
	{
		/* We can ignore return values since suspension is not possible with
		   our in-memory data source. */
		jpeg_read_scanlines(jinfo, &scanline, 1);

		jjpeg_unpack_scanline(width, format, scanline, pixels);

		pixels += pitch;
	}
}
#if 0


/** Custom memory management.
    There don't seem to be any hooks into libjpeg, so we don't compile in
    any of their memory managers, and instead supply our own definitions of
    the required functions.
 */
extern void *
jpeg_get_large(j_common_ptr jinfo, size_t size)
{
    ITK_bitmap_funcs_t *f = (ITK_bitmap_funcs_t *)(jinfo->client_data);
    return f->alloc(f, size);
}

extern void
jpeg_free_large(j_common_ptr jinfo, void * object, size_t size)
{
    ITK_bitmap_funcs_t *f = (ITK_bitmap_funcs_t *)(jinfo->client_data);
    PARAM_UNUSED(size);
    f->free(f, object);
}

extern void *
jpeg_get_small(j_common_ptr jinfo, size_t size)
{
    return jpeg_get_large(jinfo, size);
}

extern void
jpeg_free_small(j_common_ptr jinfo, void * object, size_t size)
{
    jpeg_free_large(jinfo, object, size);
}


extern long
jpeg_mem_available (j_common_ptr jinfo, long min_bytes_needed,
                    long max_bytes_needed, long already_allocated)
{
    PARAM_UNUSED(jinfo);
    PARAM_UNUSED(min_bytes_needed);
    PARAM_UNUSED(already_allocated);
    return max_bytes_needed;
}

extern long
jpeg_mem_init (j_common_ptr jinfo)
{
    PARAM_UNUSED(jinfo);
    return 0; /* just set max_memory_to_use to 0 */
}

extern void
jpeg_mem_term (j_common_ptr jinfo)
{
    PARAM_UNUSED(jinfo);
}

/*
 * Backing store (temporary file) management.
 * Since jpeg_mem_available always promised the moon,
 * this should never be called and we can just error out.
 */
void
jpeg_open_backing_store (j_common_ptr jinfo, backing_store_ptr info,
                         long total_bytes_needed)
{
    PARAM_UNUSED(info);
    PARAM_UNUSED(total_bytes_needed);
    ERREXIT(jinfo, JERR_NO_BACKING_STORE);
}



#endif

/**
 * Decompresses the JPEG data and attach it to our internal picture
 * representation.
 */
itk_bitmap_t *port_itk_bitmap_jpeg(itk_uint8_t const *jpeg_data,
					size_t jpeg_data_size,
					itk_pixel_format_t format,
					ITK_bitmap_funcs_t *funcs)
{
	struct jpeg_decompress_struct jinfo_s;
	j_decompress_ptr jinfo = &jinfo_s;
	struct jjpeg_error_mgr jerr_s;
	jjpeg_error_ptr jerr = &jerr_s;
	itk_bitmap_t *bitmap = NULL;
	itk_uint8_t *scanline = NULL;
	jjpeg_manage_error(jinfo, jerr);

	jinfo->client_data = funcs;

	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr->setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object and return.
		 */
		jpeg_destroy_decompress(jinfo);

		return NULL;
	}
	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(jinfo);

	/* Specify data source */
	jjpeg_memory_src(jinfo, (JOCTET *)jpeg_data, jpeg_data_size);

	/* We can ignore return values since suspension is not possible with our
	in-memory data source. */
	(void)jpeg_read_header(jinfo, TRUE /* require image */);

	/* In this function, we need to change any of the defaults set by
	* jpeg_read_header(), so we set the output color space to JCS_RGB.
	*/
	jinfo->out_color_space = JCS_RGB;

	/* Start decompressor */
	(void)jpeg_start_decompress(jinfo);

	/* Create our own in memory version of the JPEG */
	if (jjpeg_alloc_workspace(jinfo, format, &bitmap, &scanline))
	{
		/* We may need to do some setup of our own at this point before reading
		* the data.  After jpeg_start_decompress() we have the correct scaled
		* output image dimensions available, as well as the output colormap
		* if we asked for color quantization.
		* In this function, we need to make an output work buffer of the right size.
		*/
		jjpeg_decompress_jpeg(jinfo, bitmap, scanline);
	}
	(void)jpeg_finish_decompress(jinfo);
	jpeg_destroy_decompress(jinfo);
	if (NULL != scanline)
	{
		funcs->free(funcs, scanline);
	}
	return bitmap;
}
#define __________________________PNG_____________________________________________
/*
 * Copyright (c) 2008 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 *
 */

/**
 * Example implementation of code to create a itk_bitmap_t from PNG source
 * using the libPNG decoder library
 *
 * Note that the code may not be suitable for use without adaptation. In
 * particular note that the code uses the malloc family of heap allocators
 * which may need to be adapted for use on the target platform.
 */

/**
 * PNG callback info structure
 */
typedef struct png_callback_t
{
    ITK_bitmap_funcs_t *funcs;
    itk_pixel_format_t format;
    itk_bitmap_t *bitmap;
    itk_int8_t interlace;
    size_t row_size;
    itk_uint8_t *png_data;
    itk_palette_t *palette;
} png_callback_t;

typedef png_callback_t FAR * png_callback_tp;
typedef png_callback_t FAR * FAR * png_callback_tpp;

static void ppngInfoCallback(png_structp png_p, png_infop info_p);
static void ppngRowCallback(png_structp png_p, png_bytep new_row, png_uint_32 row_num, int pass);
static void ppngEndCallback(png_structp png_p, png_infop info_p);

// Custom PNG memory management
static void * pngAlloc(png_structp png_p, png_size_t size)
{
    ITK_bitmap_funcs_t * f = (ITK_bitmap_funcs_t *)png_get_mem_ptr(png_p);
    return f->alloc(f, size);
}

static void pngFree(png_structp png_p, void * p)
{
    ITK_bitmap_funcs_t * f = (ITK_bitmap_funcs_t *)png_get_mem_ptr(png_p);
    f->free(f, p);
}


// Creates space for picture data and row callback function.
// If resources cannot be found, exit the PNG decoder (by calling png_error).
static void
allocateWorkspace(png_structp png_p,
                  png_callback_t * pic_cb, png_uint_32 w, png_uint_32 h)
{
    ITK_bitmap_funcs_t *f = pic_cb->funcs;

    // Space to decode a row
    itk_uint8_t * png_data = (itk_uint8_t *)f->zalloc(f, h * pic_cb->row_size);

    // Space for the fully decoded bitmap
    itk_bitmap_t * bitmap = f->createBitmap(f, w, h, pic_cb->format);

    if (png_data == NULL || bitmap == NULL)
    {
        f->free(f, png_data);
        f->freeBitmap(f, bitmap);
        png_error(png_p, "Failed to allocate workspace");
    }
    else
    {
        pic_cb->png_data = png_data;
        pic_cb->bitmap = bitmap;
    }
}

static void setPixelResolution(png_structp png_p, png_infop info_p, png_callback_tp pic_cb)
{
	itk_bitmap_t * bitmap = pic_cb->bitmap;
	png_uint_32 res_x, res_y;
	int unit_type;

	// Set for default Standard Def
	bitmap->pixel_resolution_w = 720;
	bitmap->pixel_resolution_h = 576;

	// Then overwrite if it matches one of the valid Hi-Def densities
	if (png_get_pHYs(png_p, info_p, &res_x, &res_y, &unit_type)
		&& unit_type == 1)
	{
		// (See FreeSAT High Definition Graphics model for details)
		if (res_x == 3543 && res_y == 3543)
		{
			bitmap->pixel_resolution_w = 1280;
			bitmap->pixel_resolution_h = 720;
		}
		else if (res_x == 5315 && res_y == 5315)
		{
			bitmap->pixel_resolution_w = 1920;
			bitmap->pixel_resolution_h = 1080;
		}
	}
}

#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr)   ((png_ptr)->jmpbuf)
#endif

itk_bitmap_t *port_itk_bitmap_png(itk_uint8_t const *png_data,
					size_t png_data_size,
					itk_pixel_format_t format,
					ITK_bitmap_funcs_t *funcs)
{
	png_structp png_p = NULL;
	png_infop info_p = NULL;

	png_callback_t pic_cb = { NULL, 0, NULL, 0, 0, NULL, NULL };
	size_t offset = 0;

	// compare the signature bytes to check if valid PNG file
	if (png_sig_cmp( (itk_uint8_t *)png_data, 0, 8 ) != 0)
	{
		return NULL;
	}

	pic_cb.funcs = funcs;

	// create and initialize the png_struct (REQUIRED)
	png_p = png_create_read_struct_2(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL, // default error handling
		funcs, pngAlloc, pngFree); // custom heap use
	if (png_p == NULL)
	{
		return NULL;
	}
	// allocate/initialize the memory for image information (REQUIRED)
	info_p = png_create_info_struct(png_p);
	if (info_p == NULL)
	{
		png_destroy_read_struct(&png_p, NULL, NULL);
		return NULL;
	}
	// set up error handling (REQUIRED unless you set up your own error handlers)
//	if (setjmp(png_p->jmpbuf))
	if (setjmp(png_jmpbuf(png_p)))
	{
		png_destroy_read_struct(&png_p, &info_p, NULL);
		return NULL;
	}

	pic_cb.format = format;

	png_set_progressive_read_fn(png_p, (void *)&pic_cb,
		ppngInfoCallback,
		ppngRowCallback,
		ppngEndCallback);

	for (offset = 0; offset < png_data_size; offset++)
	{
		png_process_data(png_p, info_p, (itk_uint8_t *)png_data + offset, 1);
	}

	png_destroy_read_struct(&png_p, &info_p, NULL);

	return pic_cb.bitmap;
}

static void ppngInfoCallback(png_structp png_p, png_infop info_p)
{
	png_callback_tp pic_cb = png_get_progressive_ptr(png_p);
	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;
	int interlace_type;
	int compression_type;
	int filter_type;

	// read IHDR chunk
	png_get_IHDR(png_p, info_p, &width, &height,
		&bit_depth, &color_type, &interlace_type,
		&compression_type, &filter_type);

	// set up the data transformations
	// expand grey color to RGB
	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		png_set_gray_to_rgb(png_p);
	}

	// expand to RGBA quartets
	png_set_expand(png_p);

	// convert 16-bit to 8-bit
	if (bit_depth == 16)
	{
		png_set_strip_16(png_p);
	}

	// convert alpha to transparency
	png_set_invert_alpha(png_p);

	// set filler for non-alpha color types
	png_set_filler(png_p, 0x00, PNG_FILLER_AFTER);

	// turn on interlace handling
	pic_cb->interlace = (itk_int8_t)png_set_interlace_handling(png_p);

	// update the palette transformation
	png_read_update_info(png_p, info_p);

	pic_cb->row_size = png_get_rowbytes(png_p, info_p);

	pic_cb->palette = NULL;

	allocateWorkspace(png_p, pic_cb, width, height);
	setPixelResolution(png_p, info_p, pic_cb);
}

static void ppngRowCallback(png_structp png_p, png_bytep new_row, png_uint_32 row_num, int pass)
{
	png_callback_tp pic_cb = png_get_progressive_ptr(png_p);
	itk_bitmap_t * bitmap = pic_cb->bitmap;
	itk_pixel_format_t const format = bitmap->format;
	itk_uint8_t * bitmap_bits = NULL;
	itk_int32_t width = 0;
	itk_colour_t * row_data = NULL;
	void * colour = NULL;
	itk_uint8_t * scan = NULL;
	itk_uint32_t * pixels32 = NULL;
	itk_uint16_t * pixels16 = NULL;
	itk_uint32_t alpha = 0;

	// combine row data
	png_progressive_combine_row(png_p,
		pic_cb->png_data + (row_num * pic_cb->row_size),
		new_row);

	// convert to actual format after the last pass
	if (pass == (pic_cb->interlace - 1))
	{
		if (pic_cb->palette == NULL)
		{
			bitmap_bits = bitmap->bitmap + (row_num * bitmap->pitch);
			row_data = (itk_colour_t *)
				(pic_cb->png_data + (row_num * pic_cb->row_size));
			width = bitmap->width;
			switch (format)
			{
			case ITK_FB_LUT8:
				/* we can not handle palette based bitmaps */
				ITK_fatalError("Palette not supported at this time!\n");
			default:
				return;
			case ITK_FB_RGBA4444:
				while (width--)
				{
					colour = row_data;

					pixels16 = (itk_uint16_t *)bitmap_bits;
					scan = (itk_uint8_t *)colour;
					alpha = 0xff - scan[3];
					*pixels16 = ((itk_uint8_t)(scan[0] * alpha / 0xff) & 0xf0) << 8 | ((itk_uint8_t)(scan[1] * alpha / 0xff) & 0xf0) << 4 | ((itk_uint8_t)(scan[2] * alpha / 0xff) & 0xf0) | ((itk_uint8_t)(alpha) >> 4);

					row_data++;
					bitmap_bits += 2;
				}
				break;
			case ITK_FB_ARGB4444:
				while (width--)
				{
					colour = row_data;

					pixels16 = (itk_uint16_t *)bitmap_bits;
					scan = (itk_uint8_t *)colour;
					alpha = 0xff - scan[3];
					*pixels16 = ((itk_uint8_t)(alpha) & 0xf0) << 8 | ((itk_uint8_t)(scan[0] * alpha / 0xff) & 0xf0) << 4 | ((itk_uint8_t)(scan[1] * alpha / 0xff) & 0xf0) | ((itk_uint8_t)(scan[2] * alpha / 0xff) >> 4);

					row_data++;
					bitmap_bits += 2;
				}
				break;
			case ITK_FB_RGBA8888:
				while (width--)
				{
					colour = row_data;

					pixels32 = (itk_uint32_t *)bitmap_bits;
					scan = (itk_uint8_t *)colour;
					alpha = 0xff - scan[3];
					*pixels32 = ((itk_uint8_t)(scan[0] * alpha / 0xff) << 24) | ((itk_uint8_t)(scan[1] * alpha / 0xff) << 16) | ((itk_uint8_t)(scan[2] * alpha / 0xff) << 8) | (itk_uint8_t)(alpha);

					row_data++;
					bitmap_bits += 4;
				}
				break;
			case ITK_FB_ARGB8888:
				while (width--)
				{
					colour = row_data;

					pixels32 = (itk_uint32_t *)bitmap_bits;
					scan = (itk_uint8_t *)colour;
					alpha = 0xff - scan[3];
					*pixels32 = ((itk_uint8_t)(alpha) << 24) | ((itk_uint8_t)(scan[0] * alpha / 0xff) << 16) | ((itk_uint8_t)(scan[1] * alpha / 0xff) << 8) | (itk_uint8_t)(scan[2] * alpha / 0xff);

					row_data++;
					bitmap_bits += 4;
				}
				break;
			}
		}
		else
		{
			/* we can not handle palette based bitmaps */
			ITK_fatalError("Palette not supported at this time!\n");
			return;
		}
	}
}

static void ppngEndCallback(png_structp png_p, png_infop info_p)
{
	png_callback_tp pic_cb = png_get_progressive_ptr(png_p);
	pic_cb->funcs->free(pic_cb->funcs, pic_cb->png_data);
	pic_cb->png_data = NULL;
	(void)(info_p);
}

#define	__PUBLIC_INTERNAL_FUNCTION__
/**
 * Get bitmap data
 *
 * @author swkim (2011-08-10)
 *
 * @param data [in]
 * @param data_size [in]
 * @param picture_format [in]
 * @param pixel_format [in]
 * @param funcs [in]
 *
 * @return itk_bitmap_t*
 */
itk_bitmap_t *port_itk_bitmap_getPictureData(itk_uint8_t const * data,
											size_t data_size,
											itk_picture_format_t picture_format,
											itk_pixel_format_t pixel_format,
											ITK_bitmap_funcs_t * funcs)
{
	itk_bitmap_t * bitmap = NULL;

	if (data == NULL || data_size == 0)
	{
		return NULL;
	}

	switch (picture_format)
	{
		case ITK_PICTURE_PNG:
			bitmap = port_itk_bitmap_png(data, data_size, pixel_format, funcs);
			if (bitmap == NULL)
			{
				bitmap = port_itk_bitmap_jpeg(data, data_size, pixel_format, funcs);
			}
			break;
		case ITK_PICTURE_JPEG:
			bitmap = port_itk_bitmap_jpeg(data, data_size, pixel_format, funcs);
			if (bitmap == NULL)
			{
				bitmap = port_itk_bitmap_png(data, data_size, pixel_format, funcs);
			}
			break;
		default:
			bitmap = NULL;
	}

	HxLOG_Info(" +++ bitmap(%p) picture format(%d) pixel format(%d)\n", bitmap, picture_format, pixel_format);

	return bitmap;
}

#define __________________________ETC_____________________________________________

itk_int32_t bitmapGetPixelSize(itk_pixel_format_t format)
{
    switch (format)
    {
    case ITK_FB_LUT8:
        return 1;
    case ITK_FB_RGBA4444:
    case ITK_FB_ARGB4444:
        return 2;
    case ITK_FB_RGBA8888:
    case ITK_FB_ARGB8888:
        return 4;
    default:
        return 0;
    }
}


