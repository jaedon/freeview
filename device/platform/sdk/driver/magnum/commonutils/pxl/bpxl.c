/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpxl.c $
 * $brcm_Revision: Hydra_Software_Devel/19 $
 * $brcm_Date: 1/17/11 7:25p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/pxl/bpxl.c $
 * 
 * Hydra_Software_Devel/19   1/17/11 7:25p nissen
 * SW7405-3671: Added luma formats.
 * 
 * Hydra_Software_Devel/18   8/17/10 2:21p nissen
 * SW35230-1029: Fixed error when calling BPXL_GetBytesPerNPixels with a
 * invalid PXL format.
 * 
 * Hydra_Software_Devel/17   12/17/09 2:15p nissen
 * SW7405-3590: Fixed converting YCbCr 422 formats to/from other YCbCr
 * formats.
 * 
 * Hydra_Software_Devel/16   12/14/09 11:10a nissen
 * SW7405-3590: Fixed converting RGB to 10-bit YCbCr.
 * 
 * Hydra_Software_Devel/15   4/4/08 10:07a nissen
 * PR 40122: Added more support for 10-bit 422 40-bit packed formats.
 * 
 * Hydra_Software_Devel/14   3/28/08 10:18a nissen
 * PR 40122: Added 10-bit 422 40-bit packed formats.
 * 
 * Hydra_Software_Devel/13   9/17/07 4:39p syang
 * PR 34592: eY8_Cb8_Cr8_A8 and eA8_Cr8_Cb8_Y8 was accidently removed by
 * merging from 7401
 * 
 * Hydra_Software_Devel/12   9/14/07 10:57a syang
 * PR 33575: merge from 7401
 * 
 * Hydra_Software_Devel/Refsw_Dedicated_vdc_7401/1   9/7/07 5:12p jessem
 * PR 34592: Added BPXL_P_Format_Info  table and BPXL_ConvertFmtToStr to
 * help convert pixel format to a string literal.
 * 
 * Hydra_Software_Devel/8   8/25/06 10:39a nissen
 * PR 20763: Added support for 10-bit versions of YCbCr 444 and 422.
 *
 * Hydra_Software_Devel/7   2/24/05 1:46p nissen
 * PR 14207: Added bstd.h include.
 *
 * Hydra_Software_Devel/6   10/9/03 1:32p nissen
 * Removed use of float constants.
 *
 * Hydra_Software_Devel/5   9/19/03 3:42p nissen
 * Changed use of float type to int when doing color space conversion (PR
 * 8043).
 *
 * Hydra_Software_Devel/4   8/26/03 4:18p nissen
 * Renamed YCrCb to YCbCr.
 *
 * Hydra_Software_Devel/3   8/6/03 2:21p nissen
 * Fixed bug in YCrCb to RGB color conversion. Fixed bug when getting
 * bytes per n pixels for sub-byte formats.
 *
 * Hydra_Software_Devel/2   6/9/03 3:05p nissen
 * Added pixel and component conversion functions.
 *
 * Hydra_Software_Devel/1   5/28/03 3:39p nissen
 * Pixel Format module.
 *
 ***************************************************************************/

#include "bstd.h"
#include "bstd_defs.h"
#include "berr.h"
#include "bdbg.h"
#include "bpxl.h"


typedef struct
{
	BPXL_Format ePxlFmt;
	const char *pcFmtName;
} BPXL_P_Format_Info;


/****************************************************************************/
/* Making an entry for pixel format                                         */
/****************************************************************************/
#define BPXL_P_MAKE_PXL(PxlFmt)                                             \
{                                                                           \
	(PxlFmt),                                                               \
	(#PxlFmt)                                                               \
}

static const BPXL_P_Format_Info s_aPxlFmtInfo[] =
{
	/* YCbCr 444 */
	BPXL_P_MAKE_PXL(BPXL_eA8_Y8_Cb8_Cr8),
	BPXL_P_MAKE_PXL(BPXL_eCr8_Cb8_Y8_A8),
	BPXL_P_MAKE_PXL(BPXL_eY8_Cb8_Cr8_A8),
	BPXL_P_MAKE_PXL(BPXL_eA8_Cr8_Cb8_Y8),

	/* YCbCr 422 */
	BPXL_P_MAKE_PXL(BPXL_eCr8_Y18_Cb8_Y08),
	BPXL_P_MAKE_PXL(BPXL_eY18_Cr8_Y08_Cb8),
	BPXL_P_MAKE_PXL(BPXL_eY08_Cb8_Y18_Cr8),
	BPXL_P_MAKE_PXL(BPXL_eCb8_Y08_Cr8_Y18),
	BPXL_P_MAKE_PXL(BPXL_eCb8_Y18_Cr8_Y08),
	BPXL_P_MAKE_PXL(BPXL_eY18_Cb8_Y08_Cr8),
	BPXL_P_MAKE_PXL(BPXL_eY08_Cr8_Y18_Cb8),
	BPXL_P_MAKE_PXL(BPXL_eCr8_Y08_Cb8_Y18),

	/* YCbCr 420 */
	BPXL_P_MAKE_PXL(BPXL_eY8),
	BPXL_P_MAKE_PXL(BPXL_eCb8_Cr8),
	BPXL_P_MAKE_PXL(BPXL_eCr8_Cb8),

	/* YCbCr 444 10-bit */
	BPXL_P_MAKE_PXL(BPXL_eX2_Cr10_Y10_Cb10),

	/* YCbCr 422 10-bit */
	BPXL_P_MAKE_PXL(BPXL_eX2_Y010_Cb10_Y110_X2_Cr10_Y010_Cb10_X2_Y110_Cr10_Y010_X2_Cb10_Y110_Cr10),

	/* YCbCr 422 10-bit (packed 40-bit pixel)*/
	BPXL_P_MAKE_PXL(BPXL_eCr10_Y110_Cb10_Y010),
	BPXL_P_MAKE_PXL(BPXL_eY110_Cr10_Y010_Cb10),
	BPXL_P_MAKE_PXL(BPXL_eY010_Cb10_Y110_Cr10),
	BPXL_P_MAKE_PXL(BPXL_eCb10_Y010_Cr10_Y110),
	BPXL_P_MAKE_PXL(BPXL_eCb10_Y110_Cr10_Y010),
	BPXL_P_MAKE_PXL(BPXL_eY110_Cb10_Y010_Cr10),
	BPXL_P_MAKE_PXL(BPXL_eY010_Cr10_Y110_Cb10),
	BPXL_P_MAKE_PXL(BPXL_eCr10_Y010_Cb10_Y110),

	/* RGB */
	BPXL_P_MAKE_PXL(BPXL_eA8_R8_G8_B8),
	BPXL_P_MAKE_PXL(BPXL_eA8_B8_G8_R8),
	BPXL_P_MAKE_PXL(BPXL_eR8_G8_B8_A8),
	BPXL_P_MAKE_PXL(BPXL_eB8_G8_R8_A8),

	BPXL_P_MAKE_PXL(BPXL_eX8_R8_G8_B8),
	BPXL_P_MAKE_PXL(BPXL_eX8_B8_G8_R8),
	BPXL_P_MAKE_PXL(BPXL_eR8_G8_B8_X8),
	BPXL_P_MAKE_PXL(BPXL_eB8_G8_R8_X8),

	BPXL_P_MAKE_PXL(BPXL_eR8_G8_B8),
	BPXL_P_MAKE_PXL(BPXL_eB8_G8_R8),

	BPXL_P_MAKE_PXL(BPXL_eR5_G6_B5),
	BPXL_P_MAKE_PXL(BPXL_eB5_G6_R5),

	BPXL_P_MAKE_PXL(BPXL_eA1_R5_G5_B5),
	BPXL_P_MAKE_PXL(BPXL_eA1_B5_G5_R5),
	BPXL_P_MAKE_PXL(BPXL_eR5_G5_B5_A1),
	BPXL_P_MAKE_PXL(BPXL_eB5_G5_R5_A1),

	BPXL_P_MAKE_PXL(BPXL_eX1_R5_G5_B5),
	BPXL_P_MAKE_PXL(BPXL_eX1_B5_G5_R5),
	BPXL_P_MAKE_PXL(BPXL_eR5_G5_B5_X1),
	BPXL_P_MAKE_PXL(BPXL_eB5_G5_R5_X1),

	BPXL_P_MAKE_PXL(BPXL_eW1_R5_G5_B5),
	BPXL_P_MAKE_PXL(BPXL_eW1_B5_G5_R5),
	BPXL_P_MAKE_PXL(BPXL_eR5_G5_B5_W1),
	BPXL_P_MAKE_PXL(BPXL_eB5_G5_R5_W1),

	BPXL_P_MAKE_PXL(BPXL_eA4_R4_G4_B4),
	BPXL_P_MAKE_PXL(BPXL_eA4_B4_G4_R4),
	BPXL_P_MAKE_PXL(BPXL_eR4_G4_B4_A4),
	BPXL_P_MAKE_PXL(BPXL_eB4_G4_R4_A4),

	BPXL_P_MAKE_PXL(BPXL_eX4_R4_G4_B4),
	BPXL_P_MAKE_PXL(BPXL_eX4_B4_G4_R4),
	BPXL_P_MAKE_PXL(BPXL_eR4_G4_B4_X4),
	BPXL_P_MAKE_PXL(BPXL_eB4_G4_R4_X4),

	/* Palette */
	BPXL_P_MAKE_PXL(BPXL_eP1),
	BPXL_P_MAKE_PXL(BPXL_eP2),
	BPXL_P_MAKE_PXL(BPXL_eP4),
	BPXL_P_MAKE_PXL(BPXL_eP8),
	BPXL_P_MAKE_PXL(BPXL_eA8_P8),
	BPXL_P_MAKE_PXL(BPXL_eY8_P8),

	/* Alpha */
	BPXL_P_MAKE_PXL(BPXL_eA1),
	BPXL_P_MAKE_PXL(BPXL_eA2),
	BPXL_P_MAKE_PXL(BPXL_eA4),
	BPXL_P_MAKE_PXL(BPXL_eA8),
	BPXL_P_MAKE_PXL(BPXL_eW1),

	/* 3D (Special) */
	BPXL_P_MAKE_PXL(BPXL_eL8),
	BPXL_P_MAKE_PXL(BPXL_eL8_A8),
	BPXL_P_MAKE_PXL(BPXL_eZ16),

	BPXL_P_MAKE_PXL(BPXL_eL4_A4),
	BPXL_P_MAKE_PXL(BPXL_eL15_L05_A6),

	BPXL_P_MAKE_PXL(BPXL_INVALID)
};

/***************************************************************************/
BERR_Code BPXL_ConvertComponent(
	BPXL_Format eDstFormat,
	BPXL_Format eSrcFormat,
	unsigned int uiSrcPixel,
	unsigned int uiCompNum,
	unsigned int *puiDstComp )
{
	unsigned int uiDstComp = 0;
	unsigned int uiSrcSize;
	unsigned int uiDstSize;

	/* Check parameter */
	if( puiDstComp == NULL )
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Convert pixel components */
	uiSrcSize = BPXL_COMPONENT_SIZE(eSrcFormat, uiCompNum);
	uiDstSize = BPXL_COMPONENT_SIZE(eDstFormat, uiCompNum);

	/* Check if both src and dst have specified component */
	if( uiSrcSize && uiDstSize )
	{
		unsigned int uiSrcPos = BPXL_COMPONENT_POS(eSrcFormat, uiCompNum);
		unsigned int uiSrcMask = BPXL_COMPONENT_MASK(eSrcFormat, uiCompNum);
		unsigned int uiDstMask = BPXL_COMPONENT_MASK(eDstFormat, uiCompNum);

		if( uiSrcSize >= uiDstSize )
		{
			/* Convert component down in size */
			uiDstComp = (uiSrcPixel >> (uiSrcPos + uiSrcSize - uiDstSize)) & uiDstMask;
		}
		else
		{
			/* Convert component up using high to low order replication */
			signed int siBitCount = uiDstSize;

			while( siBitCount > 0 )
			{
				siBitCount -= (signed int) uiSrcSize;
				uiDstComp |= (siBitCount > 0) ?
					((uiSrcPixel >> uiSrcPos) & uiSrcMask) << siBitCount :
					((uiSrcPixel >> uiSrcPos) & uiSrcMask) >> siBitCount;
			}
		}
	}

	*puiDstComp = uiDstComp;

	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BPXL_ConvertPixel(
	BPXL_Format eDstFormat,
	BPXL_Format eSrcFormat,
	unsigned int uiSrcPixel,
	unsigned int *puiDstPixel )
{
	unsigned int uiDstPixel = 0;
	unsigned int uiDstComp;
	unsigned int ii;

	/* Check parameter */
	if( puiDstPixel == NULL )
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Convert pixel components */
	if( BPXL_IS_YCbCr422_FORMAT(eSrcFormat) && (!BPXL_IS_YCbCr422_FORMAT(eDstFormat)) )
	{
		for( ii = 0; ii < 3; ++ ii )
		{
			BPXL_ConvertComponent( eDstFormat, eSrcFormat, uiSrcPixel, ii, &uiDstComp );
			uiDstPixel |= uiDstComp << BPXL_COMPONENT_POS(eDstFormat, ii);
		}
	}
	else if( (!BPXL_IS_YCbCr422_FORMAT(eSrcFormat)) && BPXL_IS_YCbCr422_FORMAT(eDstFormat) )
	{
		BPXL_ConvertComponent( eDstFormat, eSrcFormat, uiSrcPixel, 0, &uiDstComp );
		uiDstPixel |= uiDstComp << BPXL_COMPONENT_POS(eDstFormat, 0);
		BPXL_ConvertComponent( eDstFormat, eSrcFormat, uiSrcPixel, 1, &uiDstComp );
		uiDstPixel |= uiDstComp << BPXL_COMPONENT_POS(eDstFormat, 1);
		BPXL_ConvertComponent( eDstFormat, eSrcFormat, uiSrcPixel, 2, &uiDstComp );
		uiDstPixel |= uiDstComp << BPXL_COMPONENT_POS(eDstFormat, 2);
		uiDstPixel |= uiDstComp << BPXL_COMPONENT_POS(eDstFormat, 3);
	}
	else
	{
		for( ii = 0; ii < 4; ++ ii )
		{
			BPXL_ConvertComponent( eDstFormat, eSrcFormat, uiSrcPixel, ii, &uiDstComp );
			uiDstPixel |= uiDstComp << BPXL_COMPONENT_POS(eDstFormat, ii);
		}
	}

	*puiDstPixel = uiDstPixel;

	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BPXL_ConvertPixel_RGBtoYCbCr(
	BPXL_Format eDstFormat,
	BPXL_Format eSrcFormat,
	unsigned int uiSrcPixel,
	unsigned int *puiDstPixel )
{
	unsigned int ulA, ulR, ulG, ulB;
	unsigned int ulY, ulCr, ulCb;

	/* Check parameter */
	if( puiDstPixel == NULL )
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Get ARGB components */
	BPXL_ConvertComponent( BPXL_eA8_R8_G8_B8, eSrcFormat, uiSrcPixel, 0, &ulB );
	BPXL_ConvertComponent( BPXL_eA8_R8_G8_B8, eSrcFormat, uiSrcPixel, 1, &ulG );
	BPXL_ConvertComponent( BPXL_eA8_R8_G8_B8, eSrcFormat, uiSrcPixel, 2, &ulR );
	BPXL_ConvertComponent( BPXL_eA8_R8_G8_B8, eSrcFormat, uiSrcPixel, 3, &ulA );

	/* Convert RGB components to YCbCr */
	/* Y  = R *  0.257 + G *  0.504 + B *  0.098 + 16  */
	/* Cb = R * -0.148 + G * -0.291 + B *  0.439 + 128 */
	/* Cr = R *  0.439 + G * -0.368 + B * -0.071 + 128 */
	ulY  = ulR *  0x41CA + ulG *  0x8106 + ulB *  0x1916 + 0x100000;
	ulCb = ulR * -0x25E3 + ulG * -0x4A7F + ulB *  0x7062 + 0x800000;
	ulCr = ulR *  0x7062 + ulG * -0x5E35 + ulB * -0x122D + 0x800000;

	/* Make destination pixel */
	if( eDstFormat == BPXL_eX2_Cr10_Y10_Cb10 )
	{
		ulY  = ulY >> 14;
		ulCb = ulCb >> 14;
		ulCr = ulCr >> 14;
		*puiDstPixel = BPXL_MAKE_PIXEL_10BIT(eDstFormat, ulY, ulCb, ulCr);
	}
	else
	{
		ulY  = ulY >> 16;
		ulCb = ulCb >> 16;
		ulCr = ulCr >> 16;
		*puiDstPixel = BPXL_MAKE_PIXEL(eDstFormat, BPXL_HAS_ALPHA(eDstFormat) ? ulA : ulY, ulY, ulCb, ulCr);
	}

	return BERR_SUCCESS;
}

/***************************************************************************/
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD)
BERR_Code BPXL_ConvertPixel_RGBtoHdYCbCr(
	BPXL_Format eDstFormat,
	BPXL_Format eSrcFormat,
	unsigned int uiSrcPixel,
	unsigned int *puiDstPixel )
{
	unsigned int ulA, ulR, ulG, ulB;
	unsigned int ulY, ulCr, ulCb;

	/* Check parameter */
	if( puiDstPixel == NULL )
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Get ARGB components */
	BPXL_ConvertComponent( BPXL_eA8_R8_G8_B8, eSrcFormat, uiSrcPixel, 0, &ulB );
	BPXL_ConvertComponent( BPXL_eA8_R8_G8_B8, eSrcFormat, uiSrcPixel, 1, &ulG );
	BPXL_ConvertComponent( BPXL_eA8_R8_G8_B8, eSrcFormat, uiSrcPixel, 2, &ulR );
	BPXL_ConvertComponent( BPXL_eA8_R8_G8_B8, eSrcFormat, uiSrcPixel, 3, &ulA );

	/* Convert RGB components to YCbCr */
	/* Y  = R *  0.183 + G *  0.614 + B *  0.062 + 16  */
	/* Cb = R * -0.101 + G *-0.338 + B *  0.439 + 128 */
	/* Cr = R *  0.439 + G * -0.399 + B * -0.04 + 128 */  
	ulY  = ulR *  0x2ED9 + ulG *  0x9D2F + ulB *  0xFDF + 0x100000;
	ulCb = ulR * -0x19DB + ulG * -0x5687 + ulB *  0x7062 + 0x800000;
	ulCr = ulR *  0x7062 + ulG * -0x6624 + ulB * -0xA3D + 0x800000;

	/* Make destination pixel */
	if( eDstFormat == BPXL_eX2_Cr10_Y10_Cb10 )
	{
		ulY  = ulY >> 14;
		ulCb = ulCb >> 14;
		ulCr = ulCr >> 14;
		*puiDstPixel = BPXL_MAKE_PIXEL_10BIT(eDstFormat, ulY, ulCb, ulCr);
	}
	else
	{
		ulY  = ulY >> 16;
		ulCb = ulCb >> 16;
		ulCr = ulCr >> 16;
		*puiDstPixel = BPXL_MAKE_PIXEL(eDstFormat, BPXL_HAS_ALPHA(eDstFormat) ? ulA : ulY, ulY, ulCb, ulCr);
	}

	return BERR_SUCCESS;
}

#endif
/***************************************************************************/
BERR_Code BPXL_ConvertPixel_YCbCrtoRGB(
	BPXL_Format eDstFormat,
	BPXL_Format eSrcFormat,
	unsigned int uiSrcPixel,
	unsigned int uiSrcAlign,
	unsigned int uiSrcAlpha,
	unsigned int *puiDstPixel )
{
	signed int lA, lR, lG, lB;
	signed int lY, lY0, lY1, lCr, lCb;

	/* Check parameter */
	if( puiDstPixel == NULL )
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Get YCbCr components */
	lCr = BPXL_GET_COMPONENT(eSrcFormat, uiSrcPixel, 0) - 128;
	lCb = BPXL_GET_COMPONENT(eSrcFormat, uiSrcPixel, 1) - 128;
	lY1 = BPXL_GET_COMPONENT(eSrcFormat, uiSrcPixel, 2) - 16;
	lY0 = BPXL_GET_COMPONENT(eSrcFormat, uiSrcPixel, 3) - 16;
	lY  = (BPXL_HAS_ALPHA(eSrcFormat) || (uiSrcAlign & 1)) ? lY1 : lY0;

	/* Convert to ARGB components */
	/* R = (Y - 16) * 1.164 + (Cr - 128) * 1.596 */
	/* G = (Y - 16) * 1.164 - (Cr - 128) * 0.813 - (Cb - 128) * 0.391 */
	/* B = (Y - 16) * 1.164 + (Cb - 128) * 2.018 */
	lA = (signed int) (BPXL_HAS_ALPHA(eSrcFormat) ? BPXL_GET_COMPONENT(eSrcFormat, uiSrcPixel, 3) : uiSrcAlpha);
	lR = (signed int) ((lY * 0x129FB + lCr * 0x19893) >> 16);
	lG = (signed int) ((lY * 0x129FB - lCr * 0x0D020 - lCb * 0x06418) >> 16);
	lB = (signed int) ((lY * 0x129FB + lCb * 0x2049B) >> 16);

	/* Clamp RGB components */
	lR = (lR > 0xFF) ? 0xFF : ((lR < 0) ? 0 : lR);
	lG = (lG > 0xFF) ? 0xFF : ((lG < 0) ? 0 : lG);
	lB = (lB > 0xFF) ? 0xFF : ((lB < 0) ? 0 : lB);

	/* Make destination pixel */
	*puiDstPixel = BPXL_MAKE_PIXEL(eDstFormat, lA, lR, lG, lB);

	return BERR_SUCCESS;
}

/***************************************************************************/
BERR_Code BPXL_GetBytesPerNPixels(
	BPXL_Format eFormat,
	unsigned int uiNPixels,
	unsigned int *puiBytes )
{
	unsigned int uiBytes = 0;

	/* Check parameter */
	if( puiBytes == NULL )
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* check if format is YCbCr 422 10-bit */
	if( BPXL_IS_YCbCr422_10BIT_FORMAT(eFormat) )
	{
		/* align size to 6 pixels (per 4 dwords) */
		uiBytes = ((uiNPixels + 5) / 6) * 16;
	}
	/* check if format is YCbCr 422 10-bit packed format (eg. BPXL_eCr10_Y110_Cb10_Y010)*/
	else if( BPXL_IS_YCbCr422_10BIT_PACKED_FORMAT(eFormat) )
	{
		/* align size to 8 pixels (per 5 dwords) */
		uiBytes = ((uiNPixels + 7) / 8) * 20;
	}
	else if( eFormat != BPXL_INVALID )
	{
		unsigned int uiBitsPerPixel = BPXL_BITS_PER_PIXEL(eFormat);

		/* calculate bytes for sub-byte formats */
		if( uiBitsPerPixel < 8 )
		{
			uiNPixels += 8 / uiBitsPerPixel - 1;
			uiBytes = uiNPixels / (8 / uiBitsPerPixel);
		}
		/* calculate bytes for formats that are a byte or larger */
		else
		{
			/* align size of YCbCr 422 and YCbCr 420 chroma formats to 2 pixels */
			if( BPXL_IS_YCbCr422_FORMAT(eFormat) || BPXL_IS_YCbCr420_CHROMA_FORMAT(eFormat) )
				uiNPixels = (uiNPixels + 1) & (~1);

			uiBytes = (uiNPixels * uiBitsPerPixel) / 8;
		}
	}

	*puiBytes = uiBytes;
	return BERR_SUCCESS;
}

/***************************************************************************/
const char* BPXL_ConvertFmtToStr(
		BPXL_Format eFormat )
{
	uint32_t i = 0;

	while (s_aPxlFmtInfo[i].ePxlFmt != BPXL_INVALID)
	{
		if (s_aPxlFmtInfo[i].ePxlFmt == eFormat)
		{
			return s_aPxlFmtInfo[i].pcFmtName;
		}
		i++;
	}

	return "BPXL_INVALID";
}
/* End of File */
