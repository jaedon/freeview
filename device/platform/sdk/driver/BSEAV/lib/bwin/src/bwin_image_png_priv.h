/***************************************************************************
 *     Copyright (c) 2004-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_image_png_priv.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 3/25/05 3:10p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_image_png_priv.h $
 * 
 * 2   3/25/05 3:10p erickson
 * PR14593: added DECOMPRESS_ONCE option for performance increase at cost
 * of memory usage
 * 
 * 1   2/7/05 9:05p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/3   1/10/05 5:08p erickson
 * PR13386: don't add alpha channel to png if it's not there, no need.
 * 
 * Irvine_BSEAVSW_Devel/2   1/5/05 3:02p erickson
 * PR13639: made uncompressed_buffer a png-specific param
 *
 * Irvine_BSEAVSW_Devel/1   1/30/04 4:42p bandrews
 * PR9107: BWIN Image API
 *
 ****************************************************************/

#ifndef BWIN_IMAGE_PNG_PRIV_H__
#define BWIN_IMAGE_PNG_PRIV_H__

#include <png.h>

#include "bwin_image.h"

/**
Summary:
The private image data structure for PNG-format graphics.
**/
struct bwin_image_data
{
	png_structp png_ptr;
	png_infop info_ptr;
	int bit_depth;
	int color_type;
	png_color_8p sig_bit;
	png_color_16p bgcolor;
	int row_bytes;
	png_bytepp row_pointers;
	unsigned char *uncompressed_buffer;			/* see image->pitch for pitch */
	
/* DECOMPRESS_ONCE is a hack for platforms that don't have the cpu guts to
decompress the image quickly. Instead, chew up memory by storing the 
decompress image after the first render. */
#if 0
#define DECOMPRESS_ONCE
#endif

#ifdef DECOMPRESS_ONCE
	bwin_pixel_format stored_pixel_format;
#endif
};

/**
Summary:
Set up the PNG library interface.
Returns:
Zero if the setup succeeded, non-zero otherwise.
**/
bwin_result bwin_image_png_setup(bwin_image_t image);

/**
Summary:
Tear down the PNG library interface.
**/
void bwin_image_png_teardown(bwin_image_t image);

/**
Summary:
Read the info chunk from a PNG-format image.
**/
void bwin_image_png_read_info(bwin_image_t image);

/**
Summary:
Set the transformations we want libpng to perform during the
read of the image.
**/
void bwin_image_png_set_read_transforms(bwin_image_t image,
	bwin_pixel_format *pixel_format);

/**
Summary:
Create the PNG library interface structures.
Returns:
Zero if the allocation succeeded, non-zero otherwise. 
**/
bwin_result bwin_image_png_create_struct(bwin_image_t image);

/**
Summary:
Destroy the PNG library interface structures.
**/
void bwin_image_png_destroy_struct(bwin_image_t image);

/**
Summary:
Create the buffer for the specified PNG image.
Returns:
Zero if the allocation succeeded, non-zero otherwise. 
**/
bwin_result bwin_image_png_create_buffer(bwin_image_t image);

/**
Summary:
Destroy the buffer for the specified PNG image.
**/
void bwin_image_png_destroy_buffer(bwin_image_t image);

#endif /* BWIN_IMAGE_PNG_PRIV_H__ */

