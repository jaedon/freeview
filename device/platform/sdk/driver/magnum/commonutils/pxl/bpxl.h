/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpxl.h $
 * $brcm_Revision: Hydra_Software_Devel/51 $
 * $brcm_Date: 9/23/11 7:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/pxl/bpxl.h $
 * 
 * Hydra_Software_Devel/51   9/23/11 7:21p nissen
 * SW7425-1078: Added A0 and P0 pixel formats.
 * 
 * Hydra_Software_Devel/50   4/25/11 5:06p nissen
 * SW7630-104: Added Cr and Cb format.
 * 
 * Hydra_Software_Devel/49   4/20/11 1:19p nissen
 * SW7630-104: Added A8_Y8 pixel format.
 * 
 * Hydra_Software_Devel/48   1/17/11 7:24p nissen
 * SW7405-3671: Fixed luma format.
 * 
 * Hydra_Software_Devel/47   1/17/11 6:51p nissen
 * SW7405-3671: Removed luma format.
 * 
 * Hydra_Software_Devel/46   1/17/11 5:57p nissen
 * SW7405-3671: Defined more luma formats.
 * 
 * Hydra_Software_Devel/45   1/19/10 12:59p nissen
 * SW7405-3671: Fixed CbCr format defines.
 * 
 * Hydra_Software_Devel/44   12/14/09 11:14a nissen
 * SW7405-3590: Fixed converting RGB to 10-bit YCbCr.
 * 
 * Hydra_Software_Devel/43   12/14/09 10:46a nissen
 * SW7405-3590: Fixed 24-bit packed RGB definitions.
 * 
 * Hydra_Software_Devel/42   10/1/08 5:23p nissen
 * PR 47303: Added macro to determine if format is YCbCr444.
 * 
 * Hydra_Software_Devel/41   4/4/08 10:11a nissen
 * PR 40122: Added more support for 10-bit 422 40-bit packed formats.
 * 
 * Hydra_Software_Devel/40   3/27/08 4:49p nissen
 * PR 40122: Added 10-bit 422 40-bit packed formats.
 * 
 * Hydra_Software_Devel/39   9/14/07 10:57a syang
 * PR 33575: merge from 7401
 * 
 * Hydra_Software_Devel/Refsw_Dedicated_vdc_7401/1   9/7/07 5:13p jessem
 * PR 34592: Added BPXL_P_Format_Info  table and BPXL_ConvertFmtToStr to
 * help convert pixel format to a string literal.
 * 
 * Hydra_Software_Devel/36   11/29/06 10:42a nissen
 * PR 26250: Added more YCbCr 444 formats.
 *
 * Hydra_Software_Devel/35   10/24/06 7:53p nissen
 * PR 24648: Fixed macro that returns bits per pixel for YCbCr 420 format.
 *
 * Hydra_Software_Devel/34   8/25/06 10:43a nissen
 * PR 20763: Added support for 10-bit versions of YCbCr 444 and 422.
 *
 * Hydra_Software_Devel/33   3/15/06 12:00p nissen
 * PR 15840: Added support for YCbCr 420 formats.
 *
 * Hydra_Software_Devel/32   11/28/05 3:44p syang
 * PR 12762: separate little endian and big endian when gfx sur goes to
 * mgd; mute for bad gfx sur; add 4 more pxl fmts to fully use HW
 *
 * Hydra_Software_Devel/31   3/16/05 12:16p nissen
 * PR 14329: Added Y8 and YP88 formats.
 *
 * Hydra_Software_Devel/30   9/29/04 1:06p nissen
 * PR 12728: Eliminated -pendantic errors.
 *
 * Hydra_Software_Devel/29   9/24/04 4:34p nissen
 * PR 12598: Added macro to determine if format is YCbCr422.
 *
 * Hydra_Software_Devel/28   7/2/04 12:45p nissen
 * PR 11789: Fixed definition of A8_P8 format.
 *
 * Hydra_Software_Devel/27   6/17/04 11:19a nissen
 * PR 11576: Reversed channel location define for CrCbYA.
 *
 * Hydra_Software_Devel/26   5/3/04 10:42p nissen
 * PR 9635: Fixed macro that checks if format has masked alpha.
 *
 * Hydra_Software_Devel/25   2/20/04 4:49p nissen
 * PR 9635: Added masked alpha "X" formats which are supported by M2MC B0.
 *
 * Hydra_Software_Devel/24   1/21/04 1:40p nissen
 * PR 9132: Added macros to check for luma and depth surfaces used by P3D
 * module.
 *
 * Hydra_Software_Devel/23   12/21/03 8:43p nissen
 * PR 9034: Fixed 24 bpp RGB pixel format enums.
 *
 * Hydra_Software_Devel/22   9/26/03 1:22p jasonh
 * Fixed documentation with in/out tags and in-line argument description.
 *
 * Hydra_Software_Devel/21   8/26/03 4:19p nissen
 * Renamed YCrCb to YCbCr.  Removed X formats.
 *
 * Hydra_Software_Devel/20   8/19/03 10:02a nissen
 * Added YCbCr enums.
 *
 * Hydra_Software_Devel/19   8/12/03 12:38p nissen
 * Updated macros that set component positions.
 *
 * Hydra_Software_Devel/18   8/12/03 11:09a syang
 * corrected the error in the definition of BPXL_P_LOC_AYCrCb (loc of
 * component Cb and Cr were swaped)
 *
 * Hydra_Software_Devel/17   8/12/03 11:01a syang
 * changed the parameter name of BPXL_P_LOC, BPXL_P_COMP_LOC, and
 * BPXL_P_COMP_SIZE for better readability
 *
 * Hydra_Software_Devel/16   8/12/03 10:26a nissen
 * Added enums for W1 and WRGB1555 formats.
 *
 * Hydra_Software_Devel/15   8/6/03 2:18p nissen
 * Fixed problem with YCrCb and Palette pixel component positions.
 *
 * Hydra_Software_Devel/14   6/9/03 3:30p nissen
 * Fixed problem with macro parameter.
 *
 * Hydra_Software_Devel/13   6/9/03 3:02p nissen
 * Added pixel and component conversion functions.
 *
 * Hydra_Software_Devel/12   6/5/03 11:33a nissen
 * Fixed and added pixel manipulation macros.
 *
 * Hydra_Software_Devel/11   5/29/03 10:07a nissen
 * Updated comments.
 *
 * Hydra_Software_Devel/10   5/28/03 6:57p nissen
 * Update comments.
 *
 * Hydra_Software_Devel/9   5/28/03 4:31p nissen
 * Fixed comment for macro.
 *
 * Hydra_Software_Devel/8   5/28/03 3:35p nissen
 * Modified internal definition of pixel formats. Added macros for
 * converting pixel components. Added function definitions for converting
 * pixels.
 *
 * Hydra_Software_Devel/7   4/14/03 8:32a jasonh
 * Added function to get the number of bytes per row of pixels.
 *
 * Hydra_Software_Devel/6   4/4/03 2:38p jasonh
 * Added BPXL_eA8_Y8_Cr8_Cb8 to support palette type.
 *
 * Hydra_Software_Devel/5   3/21/03 1:15p jasonh
 * Fixed module overview section of documentation.
 *
 * Hydra_Software_Devel/4   3/19/03 5:22p jasonh
 * Completed list of changes from initial internal code review.
 *
 * Hydra_Software_Devel/3   3/17/03 3:43p jasonh
 * Turned bit fields into private. Created BGR and num palette entries
 * field. Added comment on private fields. Reformatted to fit 80 columns.
 * Turned num palette entries and bits per pixel into macro.
 *
 ***************************************************************************/
#ifndef BPXL_H__
#define BPXL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=Module Overview: ********************************************************
This module provides a list of arbitrary pixel formats usable by multiple
kinds of modules. This module will provide information about the listed
formats through macros and functions. This module will also contain utility
functions used to convert from one format to another.

Supported features:
o TODO: Complete list of specific features that are supported and unit
  tested for this module.
****************************************************************************/

/***************************************************************************
 * Header bits:
 * Combined together, these bits create unique pixel values.
 * When extracted, these bits can be used to determine
 * basic information about a pixel, allowing for quick
 * processing.
 ***************************************************************************/

/* Private pixel format types */
#define BPXL_P_ALPHA           0x01000000
#define BPXL_P_COLOR           0x02000000
#define BPXL_P_RGB             0x04000000
#define BPXL_P_YCbCr           0x08000000
#define BPXL_P_PALETTE         0x10000000
#define BPXL_P_SPECIAL         0x20000000
#define BPXL_P_WINDOW          0x40000000
#define BPXL_P_TYPE_MASK       0x7F000000

/* Private pixel component information */
#define BPXL_P_COMP_SIZE_MASK  0xF
#define BPXL_P_COMP_SIZE_SHIFT 4
#define BPXL_P_COMP_LOC_MASK   0x3
#define BPXL_P_COMP_LOC_SHIFT  16

/* Private pixel component locations */
#define BPXL_P_LOC_A(p)        ((p) << (BPXL_P_COMP_LOC_SHIFT + 3 * 2))
#define BPXL_P_LOC_R(p)        ((p) << (BPXL_P_COMP_LOC_SHIFT + 2 * 2))
#define BPXL_P_LOC_G(p)        ((p) << (BPXL_P_COMP_LOC_SHIFT + 1 * 2))
#define BPXL_P_LOC_B(p)        ((p) << (BPXL_P_COMP_LOC_SHIFT + 0 * 2))
#define BPXL_P_LOC_Y0(p)       ((p) << (BPXL_P_COMP_LOC_SHIFT + 3 * 2))
#define BPXL_P_LOC_Y1(p)       ((p) << (BPXL_P_COMP_LOC_SHIFT + 2 * 2))
#define BPXL_P_LOC_Y(p)        ((p) << (BPXL_P_COMP_LOC_SHIFT + 2 * 2))
#define BPXL_P_LOC_Cb(p)       ((p) << (BPXL_P_COMP_LOC_SHIFT + 1 * 2))
#define BPXL_P_LOC_Cr(p)       ((p) << (BPXL_P_COMP_LOC_SHIFT + 0 * 2))
#define BPXL_P_LOC_P(p)        ((p) << (BPXL_P_COMP_LOC_SHIFT + 0 * 2))
#define BPXL_P_LOC_L(p)        ((p) << (BPXL_P_COMP_LOC_SHIFT + 0 * 2))

#define BPXL_P_LOC_ARGB        (BPXL_P_LOC_A(3)  | BPXL_P_LOC_R(2)  | BPXL_P_LOC_G(1)  | BPXL_P_LOC_B(0))
#define BPXL_P_LOC_ABGR        (BPXL_P_LOC_A(3)  | BPXL_P_LOC_R(0)  | BPXL_P_LOC_G(1)  | BPXL_P_LOC_B(2))
#define BPXL_P_LOC_RGBA        (BPXL_P_LOC_A(0)  | BPXL_P_LOC_R(3)  | BPXL_P_LOC_G(2)  | BPXL_P_LOC_B(1))
#define BPXL_P_LOC_BGRA        (BPXL_P_LOC_A(0)  | BPXL_P_LOC_R(1)  | BPXL_P_LOC_G(2)  | BPXL_P_LOC_B(3))
#define BPXL_P_LOC_RGB         (                   BPXL_P_LOC_R(2)  | BPXL_P_LOC_G(1)  | BPXL_P_LOC_B(0))
#define BPXL_P_LOC_BGR         (                   BPXL_P_LOC_R(0)  | BPXL_P_LOC_G(1)  | BPXL_P_LOC_B(2))
#define BPXL_P_LOC_AYCbCr      (BPXL_P_LOC_A(3)  | BPXL_P_LOC_Y(2)  | BPXL_P_LOC_Cb(1) | BPXL_P_LOC_Cr(0))
#define BPXL_P_LOC_CrCbYA      (BPXL_P_LOC_A(0)  | BPXL_P_LOC_Y(1)  | BPXL_P_LOC_Cb(2) | BPXL_P_LOC_Cr(3))
#define BPXL_P_LOC_YCbCrA      (BPXL_P_LOC_A(0)  | BPXL_P_LOC_Y(3)  | BPXL_P_LOC_Cb(2) | BPXL_P_LOC_Cr(1))
#define BPXL_P_LOC_ACrCbY      (BPXL_P_LOC_A(3)  | BPXL_P_LOC_Y(0)  | BPXL_P_LOC_Cb(1) | BPXL_P_LOC_Cr(2))
#define BPXL_P_LOC_CrY1CbY0    (BPXL_P_LOC_Y0(0) | BPXL_P_LOC_Y1(2) | BPXL_P_LOC_Cb(1) | BPXL_P_LOC_Cr(3))
#define BPXL_P_LOC_Y1CrY0Cb    (BPXL_P_LOC_Y0(1) | BPXL_P_LOC_Y1(3) | BPXL_P_LOC_Cb(0) | BPXL_P_LOC_Cr(2))
#define BPXL_P_LOC_Y0CbY1Cr    (BPXL_P_LOC_Y0(3) | BPXL_P_LOC_Y1(1) | BPXL_P_LOC_Cb(2) | BPXL_P_LOC_Cr(0))
#define BPXL_P_LOC_CbY0CrY1    (BPXL_P_LOC_Y0(2) | BPXL_P_LOC_Y1(0) | BPXL_P_LOC_Cb(3) | BPXL_P_LOC_Cr(1))
#define BPXL_P_LOC_CbY1CrY0    (BPXL_P_LOC_Y0(0) | BPXL_P_LOC_Y1(2) | BPXL_P_LOC_Cb(3) | BPXL_P_LOC_Cr(1))
#define BPXL_P_LOC_Y1CbY0Cr    (BPXL_P_LOC_Y0(1) | BPXL_P_LOC_Y1(3) | BPXL_P_LOC_Cb(2) | BPXL_P_LOC_Cr(0))
#define BPXL_P_LOC_Y0CrY1Cb    (BPXL_P_LOC_Y0(3) | BPXL_P_LOC_Y1(1) | BPXL_P_LOC_Cb(0) | BPXL_P_LOC_Cr(2))
#define BPXL_P_LOC_CrY0CbY1    (BPXL_P_LOC_Y0(2) | BPXL_P_LOC_Y1(0) | BPXL_P_LOC_Cb(1) | BPXL_P_LOC_Cr(3))
#define BPXL_P_LOC_CbCr        (BPXL_P_LOC_A(3)  | BPXL_P_LOC_Y(2)  | BPXL_P_LOC_Cb(1) | BPXL_P_LOC_Cr(0))
#define BPXL_P_LOC_CrCb        (BPXL_P_LOC_A(3)  | BPXL_P_LOC_Y(2)  | BPXL_P_LOC_Cb(0) | BPXL_P_LOC_Cr(1))
#define BPXL_P_LOC_ACrYCb      (BPXL_P_LOC_A(3)  | BPXL_P_LOC_Y(1)  | BPXL_P_LOC_Cb(0) | BPXL_P_LOC_Cr(2))
#define BPXL_P_LOC_AP          (BPXL_P_LOC_A(1)  |                                       BPXL_P_LOC_P(0))
#define BPXL_P_LOC_LA          (BPXL_P_LOC_A(0)  |                                       BPXL_P_LOC_L(1))

/* Private macro to retrieve location of component */
#define BPXL_P_COMP_LOC(f,c) \
	(((f) >> (BPXL_P_COMP_LOC_SHIFT + (c) * 2)) & BPXL_P_COMP_LOC_MASK)

/* Private macro to retrieve size of component */
#define BPXL_P_COMP_SIZE(f,c) \
	(((f) >> (c * BPXL_P_COMP_SIZE_SHIFT)) & BPXL_P_COMP_SIZE_MASK)

/***************************************************************************
Summary:
	List of all possible pixel formats.

Description:
	A pixel may contain one or more of the following components:

	A -  Alpha component.
	B -  Blue component
	Cb - Color component for YCbCr
	Cr - Color component for YCbCr
	G -  Green component
	L -  Luma component (where red, green, and blue = L) (used in 3D core)
	P -  Palette index
	R -  Red component
	W -  Special window alpha
	X -  Used to block out unused components in a format
	Y -  Luma component for YUV and YCbCr
	Z -  Fixed point Z-value (used for 3D)

	Each format identifier will contain one or more of those components
	combined with the number of bits used for that component. The order
	specified will describe what order the components take with respect
	to each other. For example:

	o  BPXL_eR5_G6_B5:  16 bit value. 5 bits of red, 6 bits of green,
	   and 5 bits of blue. High bits are red. Low bits are green.
	o  BPXL_eX8_R8_G8_B8:  32 bit value. 8 bits of red, 8 bits of green,
	   8 bits of blue. Top 8 bits are unused. Next bits are red, then
	   green, then blue in the low bits.
	o  BPXL_eZ16:  16 bit fixed point Z-value
	o  BPXL_eY08_Cb8_Y18_Cr8:  32 bit value. 4:2:2 format.

	In order to keep the component order consistent on little and
	big endian systems, access to the pixels should be done with a
	unsigned type that contains the same number of bits as the pixel.
	For 32 bit values, use uint32_t, for 16 bit value use uint16_t,
	for 8 bit values use uint8_t. For those pixels that are packed,
	such as 24 bit pixels, access will be done using bytes (uint8_t)
	and the first component listed will be the first byte found. For
	example BPXL_eR8_G8_B8 (24 bit packed value) will be accessed with a
	uint8_t and the bytes will be ordered red, then green, then blue.

	Private values used to build up actual format values are private
	and should not be used directly since they may change at any time.

	This is not a fixed list. Many more formats are available and
	can be enumerated on request.
****************************************************************************/

typedef enum BPXL_Format
{
	/* YCbCr 444 */
	BPXL_eA8_Y8_Cb8_Cr8    = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_AYCbCr | 0x8888,
	BPXL_eCr8_Cb8_Y8_A8    = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_CrCbYA | 0x8888,
	BPXL_eY8_Cb8_Cr8_A8    = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_YCbCrA | 0x8888,
	BPXL_eA8_Cr8_Cb8_Y8    = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_ACrCbY | 0x8888,

	/* YCbCr 422 */
	BPXL_eCr8_Y18_Cb8_Y08  = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_CrY1CbY0 | 0x8888,
	BPXL_eY18_Cr8_Y08_Cb8  = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_Y1CrY0Cb | 0x8888,
	BPXL_eY08_Cb8_Y18_Cr8  = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_Y0CbY1Cr | 0x8888,
	BPXL_eCb8_Y08_Cr8_Y18  = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_CbY0CrY1 | 0x8888,
	BPXL_eCb8_Y18_Cr8_Y08  = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_CbY1CrY0 | 0x8888,
	BPXL_eY18_Cb8_Y08_Cr8  = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_Y1CbY0Cr | 0x8888,
	BPXL_eY08_Cr8_Y18_Cb8  = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_Y0CrY1Cb | 0x8888,
	BPXL_eCr8_Y08_Cb8_Y18  = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_CrY0CbY1 | 0x8888,

	/* YCbCr 420 */
	BPXL_eY8               = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_ARGB | 0x0800,
	BPXL_eA8_Y8            = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_ARGB | 0x8800,
	BPXL_eCb8_Cr8          = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_CbCr | 0x0088,
	BPXL_eCr8_Cb8          = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_CrCb | 0x0088,
	BPXL_eCb8              = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_CbCr | 0x0080,
	BPXL_eCr8              = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_CbCr | 0x0008,

	/* YCbCr 444 10-bit */
	BPXL_eX2_Cr10_Y10_Cb10 = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_ACrYCb | 0x2AAA,

	/* YCbCr 422 10-bit (Special) */
	BPXL_eX2_Y010_Cb10_Y110_X2_Cr10_Y010_Cb10_X2_Y110_Cr10_Y010_X2_Cb10_Y110_Cr10 =
	                         BPXL_P_SPECIAL | BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_Y0CbY1Cr | 0xAAAA,

	/* YCbCr 422 10-bit (Packed 40-bit pixel)*/
	BPXL_eCr10_Y110_Cb10_Y010 = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_CrY1CbY0 | 0xAAAA,
	BPXL_eY110_Cr10_Y010_Cb10 = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_Y1CrY0Cb | 0xAAAA,
	BPXL_eY010_Cb10_Y110_Cr10 = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_Y0CbY1Cr | 0xAAAA,
	BPXL_eCb10_Y010_Cr10_Y110 = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_CbY0CrY1 | 0xAAAA,
	BPXL_eCb10_Y110_Cr10_Y010 = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_CbY1CrY0 | 0xAAAA,
	BPXL_eY110_Cb10_Y010_Cr10 = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_Y1CbY0Cr | 0xAAAA,
	BPXL_eY010_Cr10_Y110_Cb10 = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_Y0CrY1Cb | 0xAAAA,
	BPXL_eCr10_Y010_Cb10_Y110 = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_LOC_CrY0CbY1 | 0xAAAA,

	/* RGB */
	BPXL_eA8_R8_G8_B8      = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ARGB | 0x8888,
	BPXL_eA8_B8_G8_R8      = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ABGR | 0x8888,
	BPXL_eR8_G8_B8_A8      = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_RGBA | 0x8888,
	BPXL_eB8_G8_R8_A8      = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_BGRA | 0x8888,

	BPXL_eX8_R8_G8_B8      = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ARGB | 0x8888,
	BPXL_eX8_B8_G8_R8      = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ABGR | 0x8888,
	BPXL_eR8_G8_B8_X8      = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_RGBA | 0x8888,
	BPXL_eB8_G8_R8_X8      = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_BGRA | 0x8888,

	BPXL_eR8_G8_B8         = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_RGB | 0x0888,
	BPXL_eB8_G8_R8         = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_BGR | 0x0888,

	BPXL_eR5_G6_B5         = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ARGB | 0x0565,
	BPXL_eB5_G6_R5         = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ABGR | 0x0565,

	BPXL_eA1_R5_G5_B5      = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ARGB | 0x1555,
	BPXL_eA1_B5_G5_R5      = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ABGR | 0x1555,
	BPXL_eR5_G5_B5_A1      = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_RGBA | 0x5551,
	BPXL_eB5_G5_R5_A1      = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_BGRA | 0x5551,

	BPXL_eX1_R5_G5_B5      = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ARGB | 0x1555,
	BPXL_eX1_B5_G5_R5      = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ABGR | 0x1555,
	BPXL_eR5_G5_B5_X1      = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_RGBA | 0x5551,
	BPXL_eB5_G5_R5_X1      = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_BGRA | 0x5551,

	BPXL_eW1_R5_G5_B5      = BPXL_P_WINDOW | BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ARGB | 0x1555,
	BPXL_eW1_B5_G5_R5      = BPXL_P_WINDOW | BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ABGR | 0x1555,
	BPXL_eR5_G5_B5_W1      = BPXL_P_WINDOW | BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_RGBA | 0x5551,
	BPXL_eB5_G5_R5_W1      = BPXL_P_WINDOW | BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_BGRA | 0x5551,

	BPXL_eA4_R4_G4_B4      = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ARGB | 0x4444,
	BPXL_eA4_B4_G4_R4      = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ABGR | 0x4444,
	BPXL_eR4_G4_B4_A4      = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_RGBA | 0x4444,
	BPXL_eB4_G4_R4_A4      = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_BGRA | 0x4444,

	BPXL_eX4_R4_G4_B4      = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ARGB | 0x4444,
	BPXL_eX4_B4_G4_R4      = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_ABGR | 0x4444,
	BPXL_eR4_G4_B4_X4      = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_RGBA | 0x4444,
	BPXL_eB4_G4_R4_X4      = BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_BGRA | 0x4444,

	/* Palette */
	BPXL_eP0               = BPXL_P_COLOR | BPXL_P_PALETTE | BPXL_P_LOC_ARGB | 0x0000,
	BPXL_eP1               = BPXL_P_COLOR | BPXL_P_PALETTE | BPXL_P_LOC_ARGB | 0x0001,
	BPXL_eP2               = BPXL_P_COLOR | BPXL_P_PALETTE | BPXL_P_LOC_ARGB | 0x0002,
	BPXL_eP4               = BPXL_P_COLOR | BPXL_P_PALETTE | BPXL_P_LOC_ARGB | 0x0004,
	BPXL_eP8               = BPXL_P_COLOR | BPXL_P_PALETTE | BPXL_P_LOC_ARGB | 0x0008,
	BPXL_eA8_P8            = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_PALETTE | BPXL_P_LOC_ARGB | 0x8008,
	BPXL_eY8_P8            = BPXL_P_COLOR | BPXL_P_YCbCr | BPXL_P_PALETTE | BPXL_P_LOC_ARGB | 0x0808,

	/* Alpha */
	BPXL_eA0               = BPXL_P_ALPHA | BPXL_P_LOC_RGBA | 0x0000,
	BPXL_eA1               = BPXL_P_ALPHA | BPXL_P_LOC_RGBA | 0x0001,
	BPXL_eA2               = BPXL_P_ALPHA | BPXL_P_LOC_RGBA | 0x0002,
	BPXL_eA4               = BPXL_P_ALPHA | BPXL_P_LOC_RGBA | 0x0004,
	BPXL_eA8               = BPXL_P_ALPHA | BPXL_P_LOC_RGBA | 0x0008,
	BPXL_eW1               = BPXL_P_WINDOW | BPXL_P_ALPHA | BPXL_P_LOC_RGBA | 0x0001,

	/* 3D (Special) */
	BPXL_eL8               = BPXL_P_COLOR | BPXL_P_SPECIAL | BPXL_P_LOC_ARGB | 0x0008,
	BPXL_eL8_A8            = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_RGBA | 0x0088,
	BPXL_eZ16              = BPXL_P_SPECIAL | 0x0088,

	BPXL_eL4_A4            = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_RGBA | 0x0044,
	BPXL_eL15_L05_A6       = BPXL_P_ALPHA | BPXL_P_COLOR | BPXL_P_RGB | BPXL_P_LOC_RGBA | 0x0556,

	BPXL_INVALID           = 0
}

BPXL_Format;

/***************************************************************************
Summary:
	Determines if a format contains an alpha component.

Description:
	This macro will determine if the specified format has a alpha component.

Input:
	x - Pixel format.

Returns:
	0 - Format has no alpha component.
	1 - Format has alpha component.
****************************************************************************/
#define BPXL_HAS_ALPHA(x) \
	((((uint32_t)(x)) & BPXL_P_ALPHA) ? 1 : 0)

/***************************************************************************
Summary:
	Determines if a format contains a masked alpha component.

Description:
	This macro will determine if the specified format has a masked
	alpha component. eg. BPXL_eX8_R8_G8_B8

Input:
	x - Pixel format.

Returns:
	0 - Format has no masked alpha component.
	1 - Format has masked alpha component.
****************************************************************************/
#define BPXL_HAS_MASKED_ALPHA(x) \
	(BPXL_IS_RGB_FORMAT(x) && (BPXL_HAS_ALPHA(x) == 0) && (BPXL_COMPONENT_SIZE(x, 3)) ? 1 : 0)

/***************************************************************************
Summary:
	Determines if a format contains color components.

Description:
	This macro will determine if the specified format has one or more
	color components.

Input:
	x - Pixel format.

Returns:
	0 - Format has no color components.
	1 - Format has one or more components.
****************************************************************************/
#define BPXL_HAS_COLOR(x) \
	((((uint32_t)(x)) & BPXL_P_COLOR) ? 1 : 0)

/***************************************************************************
Summary:
	Determines if a format only has an alpha component.

Description:
	This macro will determine if the specified format only has an
	alpha component.

Input:
	x - Pixel format.

Returns:
	0 - Format is not alpha only.
	1 - Format is alpha only.
****************************************************************************/
#define BPXL_IS_ALPHA_ONLY_FORMAT(x) \
	(((((uint32_t)(x)) & BPXL_P_TYPE_MASK) == BPXL_P_ALPHA) ? 1 : 0)

/***************************************************************************
Summary:
	Determines if a format is RGB.

Description:
	This macro will determine if the specified format is an RGB format.

Input:
	x - Pixel format.

Returns:
	0 - Format is not RGB.
	1 - Format is RGB.
****************************************************************************/
#define BPXL_IS_RGB_FORMAT(x) \
	((((uint32_t)(x)) & BPXL_P_RGB) ? 1 : 0)

/***************************************************************************
Summary:
	Determines if a format is YCbCr.

Description:
	This macro will determine if the specified format is a YCbCr format.

Input:
	x - Pixel format.

Returns:
	0 - Format is not YCbCr.
	1 - Format is YCbCr.
****************************************************************************/
#define BPXL_IS_YCbCr_FORMAT(x) \
	((((uint32_t)(x)) & BPXL_P_YCbCr) ? 1 : 0)

/***************************************************************************
Summary:
	Determines if a format is special.

Description:
	This macro will determine if the specified format is a special format.

Input:
	x - Pixel format.

Returns:
	0 - Format is not special.
	1 - Format is special.
****************************************************************************/
#define BPXL_IS_SPECIAL_FORMAT(x) \
	((((uint32_t)(x)) & BPXL_P_SPECIAL) ? 1 : 0)

/***************************************************************************
Summary:
	Determines if a format is YCbCr422, YCbCr420 or 10 bit YCbCr.

Description:
	This macro will determine if the specified format is a YCbCr42x format.

Input:
	x - Pixel format.

Returns:
	0 - Format is not YCbCr42x.
	1 - Format is YCbCr42x.
****************************************************************************/
#define BPXL_IS_YCbCr422_FORMAT(x) \
	((BPXL_IS_YCbCr_FORMAT(x) && (BPXL_HAS_ALPHA(x) == 0) && (((x) & 0xFFFF) == 0x8888)) ? 1 : 0)

#define BPXL_IS_YCbCr420_LUMA_FORMAT(x) \
	((BPXL_IS_YCbCr_FORMAT(x) && ((((x) & 0xFFFF) == 0x0800) || (((x) & 0xFFFF) == 0x8800))) ? 1 : 0)

#define BPXL_IS_YCbCr420_CHROMA_FORMAT(x) \
	((BPXL_IS_YCbCr_FORMAT(x) && (((x) & 0xFFFF) == 0x0088)) ? 1 : 0)

#define BPXL_IS_YCbCr420_FORMAT(x) \
	((BPXL_IS_YCbCr420_LUMA_FORMAT(x) || BPXL_IS_YCbCr420_CHROMA_FORMAT(x)) ? 1 : 0)

#define BPXL_IS_YCbCr444_FORMAT(x) \
	((BPXL_IS_YCbCr_FORMAT(x) && BPXL_HAS_ALPHA(x)) ? 1 : 0)

#define BPXL_IS_YCbCr444_10BIT_FORMAT(x) \
	((BPXL_IS_YCbCr_FORMAT(x) && (((x) & 0xFFFF) == 0x2AAA)) ? 1 : 0)

#define BPXL_IS_YCbCr422_10BIT_FORMAT(x) \
	((BPXL_IS_SPECIAL_FORMAT(x) && BPXL_IS_YCbCr_FORMAT(x) && (((x) & 0xFFFF) == 0xAAAA)) ? 1 : 0)

#define BPXL_IS_YCbCr422_10BIT_PACKED_FORMAT(x) \
	((BPXL_IS_YCbCr_FORMAT(x) && (((x) & 0xFFFF) == 0xAAAA)) ? 1 : 0)

/***************************************************************************
Summary:
	Determines if a format is a palette.

Description:
	This macro will determine if the specified format is a palette format.

Input:
	x - Pixel format.

Returns:
	0 - Format is not palette.
	1 - Format is palette.
****************************************************************************/
#define BPXL_IS_PALETTE_FORMAT(x) \
	((((uint32_t)(x)) & BPXL_P_PALETTE) ? 1 : 0)

/***************************************************************************
Summary:
	Determines if a format contains a window alpha.

Description:
	This macro will determine if the specified format has a window alpha.

Input:
	x - Pixel format.

Returns:
	0 - Format has no window alpha.
	1 - Format has window alpha.
****************************************************************************/
#define BPXL_IS_WINDOW_FORMAT(x) \
	((((uint32_t)(x)) & BPXL_P_WINDOW) ? 1 : 0)

/***************************************************************************
Summary:
	Determines if a format is luma.

Description:
	This macro will determine if the specified format is a luma format.
	Luma formats have one color channel which is replicated across the RGB
	color channels. These formats are supported by PX3D device.

Input:
	x - Pixel format.

Returns:
	0 - Format is not luma.
	1 - Format is luma.
****************************************************************************/
#define BPXL_IS_LUMA_FORMAT(x) \
	((BPXL_IS_SPECIAL_FORMAT(x) && BPXL_HAS_COLOR(x) && (BPXL_IS_RGB_FORMAT(x) == 0) && (BPXL_IS_YCbCr_FORMAT(x) == 0)) ? 1 : 0)

/***************************************************************************
Summary:
	Determines if a format is depth.

Description:
	This macro will determine if the specified format is a depth format
	which is supported by the PX3D device.

Input:
	x - Pixel format.

Returns:
	0 - Format is not depth.
	1 - Format is depth.
****************************************************************************/
#define BPXL_IS_DEPTH_FORMAT(x) \
	((BPXL_IS_SPECIAL_FORMAT(x) && (BPXL_HAS_ALPHA(x) == 0) && (BPXL_HAS_COLOR(x) == 0)) ? 1 : 0)

/***************************************************************************
Summary:
	Returns the format's bits per pixel.

Description:
	This macro will retrieve the bits per pixel for a specified format,
	except for the YCbCr 422 10-bit special format.

Input:
	x - Pixel format.

Returns:
	Format's bits per pixel.
****************************************************************************/
#define BPXL_BITS_PER_PIXEL(x) ((\
	(((x) & BPXL_P_COMP_SIZE_MASK) + \
	(((x) >> BPXL_P_COMP_SIZE_SHIFT) & BPXL_P_COMP_SIZE_MASK) + \
	(((x) >> (BPXL_P_COMP_SIZE_SHIFT * 2)) & BPXL_P_COMP_SIZE_MASK) + \
	(((x) >> (BPXL_P_COMP_SIZE_SHIFT * 3)) & BPXL_P_COMP_SIZE_MASK))) / \
	(BPXL_IS_YCbCr422_FORMAT(x) ? 2 : 1))

/***************************************************************************
Summary:
	Returns a format's pixel mask.

Description:
	This macro will retrieve the pixel mask for a specified format.

Input:
	x - Pixel format.

Returns:
	Format's pixel mask.
****************************************************************************/
#define BPXL_PIXEL_MASK(x) \
	((1 << BPXL_BITS_PER_PIXEL(x)) - 1)

/***************************************************************************
Summary:
	Returns the format's number of palette entries.

Description:
	This macro will retrieve the number of palette entries for the
	specified format.

Input:
	x - Pixel format.

Returns:
	Format's number of palette entries.
****************************************************************************/
#define BPXL_NUM_PALETTE_ENTRIES(x) \
	(((x) & BPXL_P_PALETTE) ? 1 << ((x) & BPXL_P_COMP_SIZE_MASK) : 0)

/***************************************************************************
Summary:
	Returns a format's pixel component size.

Description:
	This macro will retrieve the number of bits in a specified pixel
	component for a specified format.

Input:
	x - Pixel format.
	n - Component number (3=A/Y0, 2=R/Y1/Y, 1=G/Cb, 0=B/Cr/P/L).

Returns:
	Format's pixel component size.
****************************************************************************/
#define BPXL_COMPONENT_SIZE(x,n) \
	(((x) >> (BPXL_P_COMP_LOC(x,n) * BPXL_P_COMP_SIZE_SHIFT)) & BPXL_P_COMP_SIZE_MASK)

/***************************************************************************
Summary:
	Returns a format's pixel component mask.

Description:
	This macro will retrieve the mask for a specified pixel component for
	a specified format shifted to the least significant bits.

Input:
	x - Pixel format.
	n - Component number (3=A/Y0, 2=R/Y1/Y, 1=G/Cb, 0=B/Cr/P/L).

Returns:
	Format's pixel component mask.
****************************************************************************/
#define BPXL_COMPONENT_MASK(x,n) \
	((1 << BPXL_COMPONENT_SIZE(x,n)) - 1)

/***************************************************************************
Summary:
	Returns a format's pixel component position.

Description:
	This macro will retrieve the position (in bits) for a specified pixel
	component for a specified format.

Input:
	x - Pixel format.
	n - Component number (3=A/Y0, 2=R/Y1/Y, 1=G/Cb, 0=B/Cr/P/L).

Returns:
	Format's pixel component position.
****************************************************************************/
#define BPXL_COMPONENT_POS(x,n) \
	((BPXL_COMPONENT_SIZE(x,n) == 0) ? 0 : \
	((BPXL_P_COMP_LOC(x,n) == 0) ? 0 : \
	((BPXL_P_COMP_LOC(x,n) == 1) ? BPXL_P_COMP_SIZE(x,0) : \
	((BPXL_P_COMP_LOC(x,n) == 2) ? BPXL_P_COMP_SIZE(x,0) + BPXL_P_COMP_SIZE(x,1) : \
	(BPXL_P_COMP_SIZE(x,0) + BPXL_P_COMP_SIZE(x,1) + BPXL_P_COMP_SIZE(x,2))))))

/***************************************************************************
Summary:
	Gets pixel component.

Description:
	This macro will get the pixel component of a pixel shifted into the
	low position using the specified format.

Input:
	x - Pixel format.
	p - Pixel.
	n - Component number (3=A/Y0, 2=R/Y1/Y, 1=G/Cb, 0=B/Cr/P/L).

Returns:
	Pixel component.
****************************************************************************/
#define BPXL_GET_COMPONENT(x,p,n) \
	(((p) >> BPXL_COMPONENT_POS(x,n)) & BPXL_COMPONENT_MASK(x,n))

/***************************************************************************
Summary:
	Makes RGB or YCbCr pixel from 8-bit components.

Description:
	This macro will shift 8-bit pixel components into position and combine
	them to create a pixel of the specified format.

Input:
	x  - Pixel format.
	c3 - Component 3 (A/Y0).
	c2 - Component 2 (R/Y1/Y).
	c1 - Component 1 (G/Cb).
	c0 - Component 0 (B/Cr/P/L).

Returns:
	Pixel in specified format.
****************************************************************************/
#define BPXL_MAKE_PIXEL(x,c3,c2,c1,c0) ( \
	(((c3) >> (8 - BPXL_COMPONENT_SIZE(x,3))) << BPXL_COMPONENT_POS(x,3)) | \
	(((c2) >> (8 - BPXL_COMPONENT_SIZE(x,2))) << BPXL_COMPONENT_POS(x,2)) | \
	(((c1) >> (8 - BPXL_COMPONENT_SIZE(x,1))) << BPXL_COMPONENT_POS(x,1)) | \
	(((c0) >> (8 - BPXL_COMPONENT_SIZE(x,0))) << BPXL_COMPONENT_POS(x,0)))

#define BPXL_MAKE_PIXEL_10BIT(x,c2,c1,c0) ( \
	((c2) << BPXL_COMPONENT_POS(x,2)) | \
	((c1) << BPXL_COMPONENT_POS(x,1)) | \
	((c0) << BPXL_COMPONENT_POS(x,0)))

/***************************************************************************
Summary:
	Converts pixel component from one format to another.

Description:
	This function will resize and reposition a pixel component from the
	source format to the destination format and put in the low position.
	This macro does not support color space conversion, eg. RGB to YCbCr.

Returns:
	BERR_SUCCESS - The source pixel componment was converted to the
	               destination format.
	BERR_INVALID_PARAMETER - An invalid parameter was supplied.

****************************************************************************/
BERR_Code BPXL_ConvertComponent(
	BPXL_Format eDstFormat,   /* [in] Destination format. */
	BPXL_Format eSrcFormat,   /* [in] Source format. */
	unsigned int uiSrcPixel,  /* [in] Source pixel. */
	unsigned int uiCompNum,   /* [in] Component number (3=A/Y0,
							     2=R/Y1/Y, 1=G/Cb, 0=B/Cr/P/L) */
	unsigned int *puiDstComp  /* [out] Pointer to converted pixel. */
);

/***************************************************************************
Summary:
	Converts pixel from one format to another.

Description:
	This function will resize and reposition pixel components from the source
	format to the destination format. This macro does not support color
	space conversion, eg. RGB to YCbCr.

Returns:
	BERR_SUCCESS - The source pixel was converted to the destination format.
	BERR_INVALID_PARAMETER - An invalid parameter was supplied.

****************************************************************************/
BERR_Code BPXL_ConvertPixel(
	BPXL_Format eDstFormat,    /* [in] Destination format */
	BPXL_Format eSrcFormat,    /* [in] Source format */
	unsigned int uiSrcPixel,   /* [in] Source pixel */
	unsigned int *puiDstPixel  /* [out] Pointer to returned destination
	                              pixel */
);

/***************************************************************************
Summary:
	Converts pixel from RGB format to YCbCr format.

Description:
	This function converts a pixel of any RGB format to a pixel of any
	YCbCr format.

Returns:
	BERR_SUCCESS - The source pixel was converted to the destination format.
	BERR_INVALID_PARAMETER - An invalid parameter was supplied.

****************************************************************************/
BERR_Code BPXL_ConvertPixel_RGBtoYCbCr(
	BPXL_Format eDstFormat,    /* [in] Destination format */
	BPXL_Format eSrcFormat,    /* [in] Source format */
	unsigned int uiSrcPixel,   /* [in] Source pixel */
	unsigned int *puiDstPixel  /* [out] Pointer to returned destination
	                              pixel */
);

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
/***************************************************************************
Summary:
	Converts pixel from RGB format to HdYCbCr format.

Description:
	This function converts a pixel of any RGB format to a pixel of any
	HdYCbCr format.

Returns:
	BERR_SUCCESS - The source pixel was converted to the destination format.
	BERR_INVALID_PARAMETER - An invalid parameter was supplied.

****************************************************************************/
BERR_Code BPXL_ConvertPixel_RGBtoHdYCbCr(
	BPXL_Format eDstFormat,    /* [in] Destination format */
	BPXL_Format eSrcFormat,    /* [in] Source format */
	unsigned int uiSrcPixel,   /* [in] Source pixel */
	unsigned int *puiDstPixel  /* [out] Pointer to returned destination
	                              pixel */
);
#endif
/***************************************************************************
Summary:
	Converts pixel from YCbCr format to RGB format.

Description:
	This function converts a pixel of any YCbCr format to a pixel of any
	RGB format. If the YCbCr format has no alpha, then the uiSrcAlpha
	parameter will be used for the alpha component.

Returns:
	BERR_SUCCESS - The source pixel was converted to the destination format.
	BERR_INVALID_PARAMETER - An invalid parameter was supplied.

****************************************************************************/
BERR_Code BPXL_ConvertPixel_YCbCrtoRGB(
	BPXL_Format eDstFormat,       /* [in] Destination format. */
	BPXL_Format eSrcFormat,       /* [in] Source format. */
	unsigned int uiSrcPixel,      /* [in] Source pixel. */
	unsigned int uiSrcAlign,      /* [in] Source alignment (even or odd). */
	unsigned int uiSrcAlpha,      /* [in] Constant alpha when source format
	                                 has no alpha. */
	unsigned int *puiDstPixel     /* [out] Pointer to returned destination
	                                 pixel. */
);

/***************************************************************************
Summary:
	Returns the number of bytes for a specific number of pixels.

Description:
	This function should be used whenever the user needs to know
	how many bytes are required for a row of pixels.

	This requires a special function since the value is not always
	the number of bits per pixel * the number of pixels. In the case
	of compressed formats, such as BPXL_eY08_Cb8_Y18_Cr8, there
	may be some rounding involved.

Returns:
	BERR_SUCCESS - The number of bytes was calculated.
	BERR_INVALID_PARAMETER - An invalid parameter was supplied.

****************************************************************************/
BERR_Code BPXL_GetBytesPerNPixels(
	BPXL_Format eFormat,          /* [in] Pixel format. */
	unsigned int uiNPixels,       /* [in] Number of pixels in row. */
	unsigned int *puiBytes        /* [out] The number of bytes required to
								     store the number of pixels of the
									 given format. */
);

/***************************************************************************
Summary:
	Gets the literal string of a pixel format.

Description:
	This gets the literal string of a pixel format.

Input:
	eFormat - Pixel format.

Returns:
	The literal string of the given pixel format
****************************************************************************/
const char* BPXL_ConvertFmtToStr(
	BPXL_Format eFormat
);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BPXL_H__ */

/* end of file */


