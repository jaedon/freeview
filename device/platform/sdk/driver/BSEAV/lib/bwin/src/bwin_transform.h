/***************************************************************************
 *     Copyright (c) 2004-2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bwin_transform.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 2/7/05 9:06p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/bwin/src/bwin_transform.h $
 * 
 * 1   2/7/05 9:06p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/7   2/7/05 12:07p erickson
 * PR13639: added b8_g8_r8 for bmp support
 * 
 * Irvine_BSEAVSW_Devel/6   1/10/05 5:07p erickson
 * PR13386: fixed COMPOSE/DECOMPOSE macros to work with 8 bit values where
 * data is shifted to most-significant bit
 * 
 * Irvine_BSEAVSW_Devel/5   1/10/05 4:09p erickson
 * PR13386: reworked transform code using byte-per-color-channel
 * assumption which makes code more compact and efficient
 * 
 * Irvine_BSEAVSW_Devel/4   1/5/05 3:01p erickson
 * PR13639: added RGB888 support for jpeg, and refactored
 *
 * Irvine_BSEAVSW_Devel/3   6/24/04 12:24p bandrews
 * PR10889: Added code to not swap pixel bytes for big endian platforms
 *
 * Irvine_BSEAVSW_Devel/2   3/9/04 2:11p bandrews
 * PR10025: The images now seem to have the correct rendering procedure by
 * not rendering pixels that have a zero alpha.
 *
 * Irvine_BSEAVSW_Devel/1   2/23/04 11:35a bandrews
 * PR9685: Refactored software blt and scale.
 *
 ****************************************************************/

#ifndef BWIN_TRANSFORM_H__
#define BWIN_TRANSFORM_H__

/**
Summary:
This is a continuation of the public bwin_pixel_format for internal
use only.
**/
typedef enum
{
	bwin_pixel_format_r8_g8_b8 = 100, /* used by JPEG and non-alpha PNG's */
	bwin_pixel_format_b8_g8_r8 = 101 /* used by BMP */
	/* In the future, there might be more */
} bwin_pixel_format_extension;

typedef struct bwin_image_transform_settings bwin_image_transform_settings;

void bwin_image_p_render_line(bwin_framebuffer_t fb,
	const bwin_rect *destrect,
	const bwin_rect *clipdestrect,
	int destrow, /* row in fb */
	bwin_image_t image,
	const bwin_rect *srcrect,
	const bwin_image_transform_settings *transform_settings);

/**
Summary: Transformation settings for the software scaler and blter
**/
struct bwin_image_transform_settings
{
	int red_index, green_index, blue_index, alpha_index;
	bool src_alpha;
	bwin_pixel_format dest_format;
	int src_bpp, dest_bpp;
};

/**
Summary: A function for initializing the transform settings
**/
void bwin_image_transform_settings_init
(
	bwin_image_transform_settings* p_transform_settings,
	bwin_pixel_format src_format,
	bwin_pixel_format dest_format
);

/**
The following DECOMPOSE/COMPOSE macros convert packed pixel
formats to and from one byte per color channel.
**/

#define DECOMPOSE_RGB565(pixel,r,g,b) do {\
	r = ((pixel) & 0xF800) >> 8; \
	g = ((pixel) & 0x07E0) >> 3; \
	b = ((pixel) & 0x001F) << 3; \
	} while(0)

#define COMPOSE_RGB565(r,g,b) (\
	((unsigned short)(r) & 0xF8) << 8 | \
	((unsigned short)(g) & 0xFC) << 3 | \
	((unsigned short)(b) & 0xF8) >> 3 \
	)

#define DECOMPOSE_ARGB1555(pixel,a,r,g,b) do {\
	a = ((pixel) & 0x8000)?0xFF:00; \
	r = ((pixel) & 0x7C00) >> 7; \
	g = ((pixel) & 0x03E0) >> 2; \
	b = ((pixel) & 0x001F) << 3; \
	} while(0)

#define COMPOSE_ARGB1555(a,r,g,b) (\
	((a)?0x8000:0x0000) | \
	((unsigned short)(r) & 0xF8) << 7 | \
	((unsigned short)(g) & 0xF8) << 2 | \
	((unsigned short)(b) & 0xF8) >> 3 \
	)

#define DECOMPOSE_ARGB8888(pixel,a,r,g,b) do {\
	a = ((pixel) & 0xFF000000) >> 24; \
	r = ((pixel) & 0x00FF0000) >> 16; \
	g = ((pixel) & 0x0000FF00) >> 8; \
	b = ((pixel) & 0x000000FF); \
	} while(0)

#define COMPOSE_ARGB8888(a,r,g,b) (\
	((unsigned long)(a) & 0xFF) << 24 | \
	((unsigned long)(r) & 0xFF) << 16 | \
	((unsigned long)(g) & 0xFF) << 8 | \
	((unsigned long)(b) & 0xFF) \
	)

#define BLEND(DEST,SRC,ALPHA) \
	(DEST) = (((SRC)-(DEST))*(ALPHA)/255) + (DEST)

#endif /* BWIN_TRANSFORM_H__ */

