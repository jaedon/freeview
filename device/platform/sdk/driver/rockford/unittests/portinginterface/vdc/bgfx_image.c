/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgfx_image.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/9/12 3:19p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/bgfx_image.c $
 * 
 * Hydra_Software_Devel/2   10/9/12 3:19p syang
 * SW7346-572: fix compile error for chips without grc
 * 
 * Hydra_Software_Devel/1   10/9/12 1:49p syang
 * SW7346-572: rm symbolic link and init a trimmed version
 * 
 * Hydra_Software_Devel/14   9/13/12 3:43p syang
 * SW7425-3896: remove hAlphaRSurface  from avc struct
 * 
 * Hydra_Software_Devel/13   1/10/11 6:38p darnstein
 * SW3548-1660: use new function BVDC_Display_GetInterruptName() instead
 * of BVDC_Test_Display_GetInterruptName().
 * 
 * Hydra_Software_Devel/12   12/10/10 5:34p vanessah
 * SW7422-43:  compile warning with GFX
 * 
 * Hydra_Software_Devel/11   11/24/10 12:55p pntruong
 * SW7552-17: Added initial 7552 support.
 *
 * Hydra_Software_Devel/10   10/11/10 1:10p jessem
 * SW7420-173: Added support for VFD as source feature.
 *
 * Hydra_Software_Devel/SW7420-173/1   5/25/10 2:54p jessem
 * SW7420-173: Added support for using VFD as  a  source.
 *
 * Hydra_Software_Devel/9   7/31/09 6:50p syang
 * PR 55812: add 7550 support
 *
 * Hydra_Software_Devel/8   4/27/07 3:13p nissen
 * PR 30062: Fixed call when opening VDC.
 *
 * Hydra_Software_Devel/7   12/19/06 12:01a pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   9/20/06 6:17p tdo
 * PR 23213, PR 23383: Adapt to new API
 *
 * Hydra_Software_Devel/6   6/5/06 4:05p syang
 * PR 21846:  checked in a wrong version last time.
 *
 * Hydra_Software_Devel/5   6/5/06 2:30p syang
 * PR 21846: fix compiling warning caused by gcc option "-O3"
 *
 * Hydra_Software_Devel/4   5/16/06 11:29a jessem
 * PR 17530: Removed direct calls to BTMR API. Using an extern to get
 * BTMR_Handle derived from AppFramework.
 *
 * Hydra_Software_Devel/3   5/11/06 4:36p hongtaoz
 * PR17530: added hTmr to open VDC;
 *
 * Hydra_Software_Devel/2   8/4/05 5:27p darnstein
 * PR16057: adapt to use with new appframeworks package: get rid of
 * CPUINT1_ calls. Also, use new global variable g_frmInfo instead of
 * g_h* variables.
 *
 * Hydra_Software_Devel/1   8/4/05 1:45p darnstein
 * PR16057: source files for GRC part of BMEtest.
 *
 ***************************************************************************/

#include <stdio.h>
#include "bgfx_blit.h"

/* This file is a trimmed version of
 *    /rockford/applications/bmetest/grc_test/bgfx_image.c
 *    /rockford/applications/bmetest/grc_test/bgfx_blit.c
 * You could copy from th original files if you need more BGFX functions
 */

uint32_t BGFX_ConvertPixel_RGBtoYCbCr( BPXL_Format eDstFormat, BPXL_Format eSrcFormat, uint32_t ulSrcPixel );
uint32_t BGFX_ConvertPixel_YCbCrtoRGB( BPXL_Format eDstFormat, BPXL_Format eSrcFormat, uint32_t ulSrcPixel, uint32_t ulSrcAlign, uint32_t ulSrcAlpha );

/***************************************************************************/
#define BYTEMASK(byte) (0x000000ff << (8*(byte)))

#define WrComp(pData,value,byte,x,y,pitch) \
	((uint32_t*)(pData))[(x)+(y)*(pitch)/4] = \
	((((uint32_t*)(pData))[(x)+(y)*(pitch)/4] & ~BYTEMASK(byte)) | \
	 (((value)<<(8*(byte)))&BYTEMASK(byte)))

#define RAMP_FLAT_LENGTH 0.1

typedef enum {
	eHORIZONTAL,
	eVERTICAL,
	eHORIZONTAL_UPDOWN,
	eVERTICAL_UPDOWN,
	eHORIZONTAL_SCALED,
	eVERTICAL_SCALED,
	eDIAG_TLBR,
	eDIAG_BLTR,
	eDIAG_SCALED_TLBR,
	eDIAG_SCALED_BLTR
} eRampType;

typedef enum {
	eY0 = 0,
	eCB = 1,
	eY1 = 2,
	eCR = 3
} eByte;

uint32_t GetRampVal( int32_t start, int32_t end, int32_t pos, int32_t sv, int32_t ev );
void Ramp( uint8_t *pData, uint32_t pitch, int32_t sx, int32_t sy, int32_t ex, int32_t ey, eByte byte, int32_t scale, int32_t sv, int32_t ev, eRampType rampType );
char *GetFormatString( BPXL_Format eFormat );

uint32_t GetRampVal
(
	int32_t start,
	int32_t end,
	int32_t pos,
	int32_t sv,
	int32_t ev
)
{
	int32_t rampStart, rampEnd;

	pos = (pos - start) % (end - start) + start;
	rampStart = (int32_t)((end-start) * RAMP_FLAT_LENGTH + start);
	rampEnd = (int32_t)((end-start) * (1-RAMP_FLAT_LENGTH) + start);

	if (pos <= rampStart)
		return sv;
	if (pos >= rampEnd)
		return ev;

	return sv + (int32_t)((int32_t)(ev - sv) *
		((float)(pos - rampStart) / (float)(rampEnd - rampStart)));
}

void Ramp(
	uint8_t *pData,
	uint32_t pitch,
	int32_t sx,
	int32_t sy,
	int32_t ex,
	int32_t ey,
	eByte byte,
	int32_t scale,
	int32_t sv,
	int32_t ev,
	eRampType rampType )
{
	int32_t i, j;

	for(j=sy; j<ey; ++j)
	{
		for(i=sx; i<ex; ++i)
		{
			switch(rampType)
			{
			case eHORIZONTAL:        WrComp(pData, GetRampVal(sx, ex, i, sv, ev), byte, i, j, pitch); break;
			case eVERTICAL:          WrComp(pData, GetRampVal(sy, ey, j, sv, ev), byte, i, j, pitch); break;
			case eHORIZONTAL_SCALED: WrComp(pData, GetRampVal(sx, ex, scale*(i-sx)+sx, sv, ev), byte, i, j, pitch); break;
			case eVERTICAL_SCALED:   WrComp(pData, GetRampVal(sy, ey, scale*(j-sy)+sy, sv, ev), byte, i, j, pitch); break;
			case eHORIZONTAL_UPDOWN: WrComp(pData, GetRampVal(sx, ex, ((i<((sx+ex)/2)) ? (2*i - sx) : (sx + 2*(ex - 1 - i))), sv, ev), byte, i, j, pitch); break;
			case eVERTICAL_UPDOWN:   WrComp(pData, GetRampVal(sy, ey, ((j<((sy+ey)/2)) ? (2*j - sy) : (sy + 2*(ey - 1 - j))), sv, ev), byte, i, j, pitch); break;
			case eDIAG_TLBR:         WrComp(pData, GetRampVal(sx+sy, ex+ey, i+j, sv, ev), byte, i, j, pitch); break;
			case eDIAG_BLTR:         WrComp(pData, GetRampVal(sx+sy, ex+ey, i+(ey-j+sy), sv, ev), byte, i, j, pitch); break;
			case eDIAG_SCALED_TLBR:  WrComp(pData, GetRampVal(sx+sy, ex+ey, scale*(i-sx)+sx+scale*(j-sy)+sy, sv, ev), byte, i, j, pitch); break;
			case eDIAG_SCALED_BLTR:  WrComp(pData, GetRampVal(sx+sy, ex+ey, scale*(i-sx)+sx+(scale*(ey-j)+sy), sv, ev), byte, i, j, pitch); break;
			}
		}
	}
}

/***************************************************************************
* Copies an ARGB8888 color ramp image into a surface of any size.
***************************************************************************/
void BGFX_MakeImageARGB8888(
	BSUR_Surface_Handle hSurface )
{
	BPXL_Format eFormat;
	uint8_t *pImage;
	uint32_t ww, hh, pp;

	BSUR_Surface_GetAddress( hSurface, (void *) (&pImage), &pp );
	BSUR_Surface_GetDimensions( hSurface, &ww, &hh );
	BSUR_Surface_GetFormat( hSurface, &eFormat );

	if( BPXL_BITS_PER_PIXEL(eFormat) == 16 )
		ww /= 2;

	/* Y0 ramps */
	Ramp( pImage, pp, 0,      0,      ww/3,   hh/3,   eY0, 2, 0,   255, eDIAG_SCALED_TLBR);
	Ramp( pImage, pp, 0,      hh/3,   ww/3,   2*hh/3, eY0, 2, 0,   255, eDIAG_SCALED_TLBR);
	Ramp( pImage, pp, 0,      2*hh/3, ww/3,   hh,     eY0, 2, 255,   0, eDIAG_SCALED_TLBR);
	Ramp( pImage, pp, ww/3,   0,      2*ww/3, hh/3,   eY0, 2, 0,   255, eDIAG_SCALED_TLBR);
	Ramp( pImage, pp, ww/3,   hh/3,   2*ww/3, 2*hh/3, eY0, 2, 255,   0, eDIAG_SCALED_TLBR);
	Ramp( pImage, pp, ww/3,   2*hh/3, 2*ww/3, hh,     eY0, 2, 0,   255, eDIAG_SCALED_TLBR);
	Ramp( pImage, pp, 2*ww/3, 0,      ww,     hh/3,   eY0, 2, 255,   0, eDIAG_SCALED_TLBR);
	Ramp( pImage, pp, 2*ww/3, hh/3,   ww,     2*hh/3, eY0, 2, 0,   255, eDIAG_SCALED_TLBR);
	Ramp( pImage, pp, 2*ww/3, 2*hh/3, ww,     hh,     eY0, 2, 255,   0, eDIAG_SCALED_TLBR);

	/* Y1 ramps */
	Ramp( pImage, pp, 0,      0,      ww/3,   hh/3,   eY1, 1, 0,   255, eDIAG_BLTR);
	Ramp( pImage, pp, 0,      hh/3,   ww/3,   2*hh/3, eY1, 1, 0,   255, eDIAG_BLTR);
	Ramp( pImage, pp, 0,      2*hh/3, ww/3,   hh,     eY1, 1, 255,   0, eDIAG_BLTR);
	Ramp( pImage, pp, ww/3,   0,      2*ww/3, hh/3,   eY1, 1, 255,   0, eDIAG_BLTR);
	Ramp( pImage, pp, ww/3,   hh/3,   2*ww/3, 2*hh/3, eY1, 1, 0,   255, eDIAG_BLTR);
	Ramp( pImage, pp, ww/3,   2*hh/3, 2*ww/3, hh,     eY1, 1, 255,   0, eDIAG_BLTR);
	Ramp( pImage, pp, 2*ww/3, 0,      ww,     hh/3,   eY1, 1, 0,   255, eDIAG_BLTR);
	Ramp( pImage, pp, 2*ww/3, hh/3,   ww,     2*hh/3, eY1, 1, 0,   255, eDIAG_BLTR);
	Ramp( pImage, pp, 2*ww/3, 2*hh/3, ww,     hh,     eY1, 1, 255,   0, eDIAG_BLTR);

	/* CR ramps */
	Ramp( pImage, pp, 0,      0,      ww/4,   hh/4,   eCR, 1, 0,   255, eHORIZONTAL);
	Ramp( pImage, pp, 0,      hh/4,   ww/4,   2*hh/4, eCR, 1, 0,   255, eVERTICAL);
	Ramp( pImage, pp, 0,      2*hh/4, ww/4,   3*hh/4, eCR, 1, 0,   255, eHORIZONTAL);
	Ramp( pImage, pp, 0,      3*hh/4, ww/4,   hh,     eCR, 1, 0,   255, eVERTICAL);
	Ramp( pImage, pp, ww/4,   0,      2*ww/4, hh/4,   eCR, 1, 0,   255, eHORIZONTAL);
	Ramp( pImage, pp, ww/4,   hh/4,   2*ww/4, 2*hh/4, eCR, 1, 0,   255, eVERTICAL);
	Ramp( pImage, pp, ww/4,   2*hh/4, 2*ww/4, 3*hh/4, eCR, 1, 0,   255, eHORIZONTAL);
	Ramp( pImage, pp, ww/4,   3*hh/4, 2*ww/4, hh,     eCR, 1, 0,   255, eVERTICAL);
	Ramp( pImage, pp, 2*ww/4, 0,      3*ww/4, hh/4,   eCR, 1, 0,   255, eHORIZONTAL);
	Ramp( pImage, pp, 2*ww/4, hh/4,   3*ww/4, 2*hh/4, eCR, 1, 0,   255, eVERTICAL);
	Ramp( pImage, pp, 2*ww/4, 2*hh/4, 3*ww/4, 3*hh/4, eCR, 1, 0,   255, eHORIZONTAL);
	Ramp( pImage, pp, 2*ww/4, 3*hh/4, 3*ww/4, hh,     eCR, 1, 0,   255, eVERTICAL);
	Ramp( pImage, pp, 3*ww/4, 0,      ww,     hh/4,   eCR, 1, 0,   255, eHORIZONTAL);
	Ramp( pImage, pp, 3*ww/4, hh/4,   ww,     2*hh/4, eCR, 1, 0,   255, eVERTICAL);
	Ramp( pImage, pp, 3*ww/4, 2*hh/4, ww,     3*hh/4, eCR, 1, 0,   255, eHORIZONTAL);
	Ramp( pImage, pp, 3*ww/4, 3*hh/4, ww,     hh,     eCR, 1, 0,   255, eVERTICAL);

	/* CB ramps */
	Ramp( pImage, pp, 0,      0,      ww/4,   hh/4,   eCB, 2, 0,   255, eHORIZONTAL_SCALED);
	Ramp( pImage, pp, 0,      hh/4,   ww/4,   2*hh/4, eCB, 1, 255,   0, eHORIZONTAL_UPDOWN);
	Ramp( pImage, pp, 0,      2*hh/4, ww/4,   3*hh/4, eCB, 1, 0,   255, eVERTICAL_UPDOWN);
	Ramp( pImage, pp, 0,      3*hh/4, ww/4,   hh,     eCB, 2, 255,   0, eVERTICAL_SCALED);
	Ramp( pImage, pp, ww/4,   0,      2*ww/4, hh/4,   eCB, 1, 255,   0, eHORIZONTAL_UPDOWN);
	Ramp( pImage, pp, ww/4,   hh/4,   2*ww/4, 2*hh/4, eCB, 2, 255,   0, eHORIZONTAL_SCALED);
	Ramp( pImage, pp, ww/4,   2*hh/4, 2*ww/4, 3*hh/4, eCB, 2, 0,   255, eVERTICAL_SCALED);
	Ramp( pImage, pp, ww/4,   3*hh/4, 2*ww/4, hh,     eCB, 1, 0,   255, eVERTICAL_UPDOWN);
	Ramp( pImage, pp, 2*ww/4, 0,      3*ww/4, hh/4,   eCB, 1, 0,   255, eHORIZONTAL_UPDOWN);
	Ramp( pImage, pp, 2*ww/4, hh/4,   3*ww/4, 2*hh/4, eCB, 2, 0,   255, eHORIZONTAL_SCALED);
	Ramp( pImage, pp, 2*ww/4, 2*hh/4, 3*ww/4, 3*hh/4, eCB, 2, 255,   0, eVERTICAL_SCALED);
	Ramp( pImage, pp, 2*ww/4, 3*hh/4, 3*ww/4, hh,     eCB, 1, 255,   0, eVERTICAL_UPDOWN);
	Ramp( pImage, pp, 3*ww/4, 0,      ww,     hh/4,   eCB, 2, 255,   0, eHORIZONTAL_SCALED);
	Ramp( pImage, pp, 3*ww/4, hh/4,   ww,     2*hh/4, eCB, 1, 0,   255, eHORIZONTAL_UPDOWN);
	Ramp( pImage, pp, 3*ww/4, 2*hh/4, ww,     3*hh/4, eCB, 1, 255,   0, eVERTICAL_UPDOWN);
	Ramp( pImage, pp, 3*ww/4, 3*hh/4, ww,     hh,     eCB, 2, 0,   255, eVERTICAL_SCALED);
}

/***************************************************************************
* Copies any RGB or YCbCr surface's image into a surface of any format.
***************************************************************************/
void BGFX_CopyImage(
	BSUR_Surface_Handle hDstSurface,
	BSUR_Surface_Handle hSrcSurface )
{
	BPXL_Format eDstFormat;
	BPXL_Format eSrcFormat;
	uint8_t *pDstImage;
	uint8_t *pSrcImage;
	uint32_t ulDstPitch;
	uint32_t ulSrcPitch;
	uint32_t ulWidth;
	uint32_t ulHeight;
	uint32_t ulDstBpp;
	uint32_t ulSrcBpp;
	uint32_t ulDstPixel = 0;
	uint32_t ulSrcPixel = 0;
	uint32_t ii, jj;

	BSUR_Surface_GetFormat( hDstSurface, &eDstFormat );
	BSUR_Surface_GetFormat( hSrcSurface, &eSrcFormat );
	BSUR_Surface_GetAddress( hDstSurface, (void *) (&pDstImage), &ulDstPitch );
	BSUR_Surface_GetAddress( hSrcSurface, (void *) (&pSrcImage), &ulSrcPitch );
	BSUR_Surface_GetDimensions( hDstSurface, &ulWidth, &ulHeight );

	ulDstBpp = BPXL_BITS_PER_PIXEL(eDstFormat);
	ulSrcBpp = BPXL_BITS_PER_PIXEL(eSrcFormat);

	for( jj = 0; jj < ulHeight; ++jj )
	{
		uint8_t *pucDstScan = pDstImage + ulDstPitch * jj;
		uint8_t *pucSrcScan = pSrcImage + ulSrcPitch * jj;

		for( ii = 0; ii < ulWidth; ++ii )
		{
			/* Load src pixel */
			if( BPXL_IS_YCbCr_FORMAT(eSrcFormat) )
			{
				switch( ulSrcBpp )
				{
				case 32: ulSrcPixel = (uint32_t) ((uint32_t *) pucSrcScan)[ii]; break;
				case 16: ulSrcPixel = (uint32_t) ((uint32_t *) pucSrcScan)[ii / 2]; break;
				}
			}
			else
			{
				switch( ulSrcBpp )
				{
				case 32: ulSrcPixel = (uint32_t) ((uint32_t *) pucSrcScan)[ii]; break;
				case 16: ulSrcPixel = (uint32_t) ((uint16_t *) pucSrcScan)[ii]; break;
				case 8:  ulSrcPixel = (uint32_t) ((uint8_t *)  pucSrcScan)[ii]; break;
				}
			}

			/* Convert pixel */
			if( BPXL_IS_YCbCr_FORMAT(eSrcFormat) )
			{
				if( BPXL_IS_YCbCr_FORMAT(eDstFormat) )
				{
					BPXL_ConvertPixel(eDstFormat, eSrcFormat, ulSrcPixel, &ulDstPixel );
				}
				else
				{
					ulDstPixel = BGFX_ConvertPixel_YCbCrtoRGB( eDstFormat, eSrcFormat, ulSrcPixel, ii, 0xFF );
				}
			}
			else
			{
				if( BPXL_IS_YCbCr_FORMAT(eDstFormat) )
				{
					ulDstPixel = BGFX_ConvertPixel_RGBtoYCbCr( eDstFormat, eSrcFormat, ulSrcPixel );
				}
				else if( BPXL_IS_PALETTE_FORMAT(eDstFormat) )
				{
					BPXL_ConvertPixel( BPXL_eA8_R8_G8_B8, eSrcFormat, ulSrcPixel, (unsigned int *)&ulDstPixel );
				}
				else
				{
					BPXL_ConvertPixel( eDstFormat, eSrcFormat, ulSrcPixel, (unsigned int *)&ulDstPixel );
				}
			}

			/* Store dst pixel */
			if( BPXL_IS_YCbCr444_10BIT_FORMAT(eDstFormat) )
			{
				((uint32_t *) pucDstScan)[ii] = ulDstPixel;
			}
			else if( BPXL_IS_YCbCr_FORMAT(eDstFormat) )
			{
				switch( ulDstBpp )
				{
				case 32: ((uint32_t *) pucDstScan)[ii] = ulDstPixel; break;
				case 16:
					if( ii & 1 )
					{
						uint32_t ulComp;
						BPXL_ConvertComponent( eDstFormat, eDstFormat, ulDstPixel, 2, (unsigned int *)&ulComp );
						pucDstScan[ii * 2 + BPXL_COMPONENT_POS(eDstFormat, 2) / 8 - 2] = (uint8_t) ulComp;
					}
					else
					{
						((uint32_t *) pucDstScan)[ii / 2] = ulDstPixel;
					}
					break;
				}
			}
			else if( BPXL_IS_PALETTE_FORMAT(eDstFormat) )
			{
				switch( ulDstBpp )
				{
				case 16: ((uint16_t *) pucDstScan)[ii] = (uint16_t)
					(((((ulDstPixel >> (16 + 5)) & 0x7) << 5) |
					(((ulDstPixel >> (8 + 5)) & 0x7) << 2) |
					((ulDstPixel >> 6) & 0x3)) | ((ulDstPixel >> 16) & 0xFF00));
					break;
				case 8: pucDstScan[ii] = (uint8_t)
					((((ulDstPixel >> (16 + 5)) & 0x7) << 5) |
					(((ulDstPixel >> (8 + 5)) & 0x7) << 2) |
					((ulDstPixel >> 6) & 0x3));
					break;
				case 4: ulDstPixel =
					((((ulDstPixel >> (16 + 7)) & 0x1) << 3) |
					(((ulDstPixel >> (8 + 6)) & 0x3) << 1) |
					((ulDstPixel >> 7) & 0x1));
					break;
				case 2: ulDstPixel =
					((((ulDstPixel >> (16 + 7)) & 0x1) << 1) |
					((ulDstPixel >> (8 + 7)) & 0x1));
					break;
				}

				if( ulDstBpp < 8 )
				{
					uint32_t ulPpb   = 8 / ulDstBpp;
					uint32_t ulMask  = BPXL_PIXEL_MASK(eDstFormat);
					uint32_t ulShift = ((ulPpb - 1) - (ii & (ulPpb - 1))) * ulDstBpp;

					pucDstScan[ii/ulPpb] &= ~(ulMask << ulShift);
					pucDstScan[ii/ulPpb] |= (ulDstPixel & ulMask) << ulShift;
				}
			}
			else
			{
				switch( ulDstBpp )
				{
				case 32: ((uint32_t *) pucDstScan)[ii] = (uint32_t) ulDstPixel; break;
				case 16: ((uint16_t *) pucDstScan)[ii] = (uint16_t) ulDstPixel; break;
				case 8:  ((uint8_t  *) pucDstScan)[ii] = (uint8_t)  ulDstPixel; break;
				case 4:
				case 2:
				case 1:
					{
						uint32_t ulPpb   = 8 / ulDstBpp;
						uint32_t ulMask  = BPXL_PIXEL_MASK(eDstFormat);
						uint32_t ulShift = ((ulPpb - 1) - (ii & (ulPpb - 1))) * ulDstBpp;

						pucDstScan[ii/ulPpb] &= ~(ulMask << ulShift);
						pucDstScan[ii/ulPpb] |= (ulDstPixel & ulMask) << ulShift;
					}
					break;
				}
			}
		}
	}
}

/***************************************************************************
* Dumps the image of a surfce of any format.
***************************************************************************/
void BGFX_DumpImage(
	BSUR_Surface_Handle hSurface,
	BSUR_Surface_Handle hSrcSurface,
	uint8_t *pucOp )
{
	FILE *outfp1;
	BPXL_Format eFormat;
	BPXL_Format eSrcFormat = 0;
	uint8_t *pImage;
	uint32_t ii, jj, ww, hh, pp;
	char name[256];
	static uint32_t nn = 0;

	BSUR_Surface_GetAddress( hSurface, (void *) (&pImage), &pp );
	BSUR_Surface_GetDimensions( hSurface, &ww, &hh );
	BSUR_Surface_GetFormat( hSurface, &eFormat );

	if( hSrcSurface )
	{
		BSUR_Surface_GetFormat( hSrcSurface, &eSrcFormat );
		sprintf( name, "test_%03d_%s_%s_%s.pic", nn++,
			GetFormatString(eSrcFormat), GetFormatString(eFormat), pucOp );
	}
	else
	{
		sprintf( name, "test_%03d_%s_%s.pic", nn++, GetFormatString(eFormat), pucOp );
	}

	outfp1 = fopen(name, "w");

	if (outfp1)
	{
		fprintf(outfp1, "width  %d\n", ww );
		fprintf(outfp1, "height %d\n", hh );
		fprintf(outfp1, "format %x %s\n", eFormat, GetFormatString(eFormat) );

		if( BPXL_IS_PALETTE_FORMAT(eFormat) )
		{
			BSUR_Palette_Handle hPalette = 0;
			BSUR_Surface_GetPalette( hSurface, &hPalette );
			if( hPalette )
			{
				void *pvPalette = NULL;
				uint32_t *pulPalette = NULL;
				BSUR_Palette_GetAddress( hPalette, &pvPalette );
				if( NULL != (pulPalette = (uint32_t *) pvPalette) )
				{
					for( ii = 0; ii < (uint32_t) BPXL_NUM_PALETTE_ENTRIES(eFormat); ++ii )
					{
						fprintf( outfp1, "%08x\n", pulPalette[ii] );
					}
				}
			}
		}

		for( jj = 0; jj < hh; ++jj )
		{
			uint32_t *pulScan = (uint32_t *) (pImage + jj * pp);
			uint16_t *pusScan = (uint16_t *) (pImage + jj * pp);
			uint8_t  *pucScan = (uint8_t *)  (pImage + jj * pp);
			uint32_t ulPixel = 0;

			for( ii = 0; ii < ww; ++ii )
			{
				if( BPXL_IS_YCbCr_FORMAT(eFormat) )
				{
					switch( BPXL_BITS_PER_PIXEL(eFormat) )
					{
					case 32:
						ulPixel = (uint32_t) pulScan[ii];
						fprintf( outfp1, "%08x\n", ulPixel );
						break;
					case 16:
						if( (ii & 1) == 0 )
						{
							ulPixel = (uint32_t) pulScan[ii / 2];
							fprintf( outfp1, "%08x\n", ulPixel );
						}
						break;
					}
				}
				else
				{
					switch( BPXL_BITS_PER_PIXEL(eFormat) )
					{
					case 32:
						ulPixel = (uint32_t) pulScan[ii];
						fprintf( outfp1, "%08x\n", ulPixel );
						break;
					case 24:
						ulPixel = (uint32_t) pucScan[ii * 3];
						ulPixel |= ((uint32_t) pucScan[ii * 3 + 1]) << 8;
						ulPixel |= ((uint32_t) pucScan[ii * 3 + 2]) << 16;
						fprintf( outfp1, "%06x\n", ulPixel );
						break;
					case 16:
						ulPixel = (uint32_t) pusScan[ii];
						fprintf( outfp1, "%04x\n", ulPixel );
						break;
					case 8:
						ulPixel = (uint32_t) pucScan[ii];
						fprintf( outfp1, "%02x\n", ulPixel );
						break;
					case 4:
					case 2:
					case 1:
						{
							uint32_t ulPpb   = 8 / BPXL_BITS_PER_PIXEL(eFormat);
							uint32_t ulMask  = BPXL_PIXEL_MASK(eFormat);
							uint32_t ulShift = ((ulPpb - 1) - (ii & (ulPpb - 1))) * BPXL_BITS_PER_PIXEL(eFormat);
							ulPixel = (pucScan[ii/ulPpb] >> ulShift) & ulMask;
							fprintf( outfp1, "%02x\n", ulPixel );
						}
						break;
					}
				}
			}
		}

		fclose( outfp1 );
	}
}

/***************************************************************************
* Makes a ramp palette for any palette format.
***************************************************************************/
void BGFX_MakePalette(
	uint32_t *pulPalette,
	uint32_t ulBpp )
{
	uint32_t rr, gg, bb;

	if( ulBpp >= 8 )
	{
		for( rr = 0; rr < 8; ++rr )
			for( gg = 0; gg < 8; ++gg )
				for( bb = 0; bb < 4; ++bb )
					pulPalette[(rr << 5) | (gg << 2) | bb] = 0xFF000000 |
						((((rr << 3) | rr) << 2) << 16) |
						((((gg << 3) | gg) << 2) << 8) |
						((((bb << 4) | (bb << 2) | bb) << 2));
	}
	else if( ulBpp == 4 )
	{
		for( rr = 0; rr < 2; ++rr )
			for( gg = 0; gg < 4; ++gg )
				for( bb = 0; bb < 2; ++bb )
					pulPalette[(rr << 3) | (gg << 1) | bb] = 0xFF000000 |
						((((rr << 3) | (rr << 2) | (rr << 1) | rr) << 4) << 16) |
						((((gg << 6) | (gg << 4) | (gg << 2) | gg) << 2) << 8) |
						((((bb << 3) | (bb << 2) | (bb << 1) | bb) << 4));
	}
	else
	{
		for( rr = 0; rr < 2; ++rr )
			for( gg = 0; gg < 2; ++gg )
					pulPalette[(rr << 1) | gg] = 0xFF000000 |
						((((rr << 3) | (rr << 2) | (rr << 1) | rr) << 4) << 16) |
						((((gg << 3) | (gg << 2) | (gg << 1) | gg) << 4) << 8);
	}
}

/***************************************************************************/
uint32_t BGFX_ConvertPixel_RGBtoYCbCr(
	BPXL_Format eDstFormat,
	BPXL_Format eSrcFormat,
	uint32_t ulSrcPixel )
{
	uint32_t ulA, ulR, ulG, ulB;
	uint32_t ulY, ulCr, ulCb;
	uint32_t ulDstPixel;

	/* Get ARGB components */
	BPXL_ConvertComponent( BPXL_eA8_R8_G8_B8, eSrcFormat, ulSrcPixel, 0, (unsigned int*)&ulB );
	BPXL_ConvertComponent( BPXL_eA8_R8_G8_B8, eSrcFormat, ulSrcPixel, 1, (unsigned int*)&ulG );
	BPXL_ConvertComponent( BPXL_eA8_R8_G8_B8, eSrcFormat, ulSrcPixel, 2, (unsigned int*)&ulR );
	BPXL_ConvertComponent( BPXL_eA8_R8_G8_B8, eSrcFormat, ulSrcPixel, 3, (unsigned int*)&ulA );

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
		ulDstPixel = BPXL_MAKE_PIXEL_10BIT(eDstFormat, ulY, ulCb, ulCr);
	}
	else
	{
		ulY  = ulY >> 16;
		ulCb = ulCb >> 16;
		ulCr = ulCr >> 16;
		ulDstPixel = BPXL_MAKE_PIXEL(eDstFormat, BPXL_HAS_ALPHA(eDstFormat) ? ulA : ulY, ulY, ulCb, ulCr);
	}

	/* Make destination pixel */
	return ulDstPixel;
}

/***************************************************************************/
uint32_t BGFX_ConvertPixel_YCbCrtoRGB(
	BPXL_Format eDstFormat,
	BPXL_Format eSrcFormat,
	uint32_t ulSrcPixel,
	uint32_t ulSrcAlign,
	uint32_t ulSrcAlpha )
{
	int32_t lA, lR, lG, lB;
	int32_t lY, lY0, lY1, lCr, lCb;

	/* Get YCbCr components */
	lCr = BPXL_GET_COMPONENT(eSrcFormat, ulSrcPixel, 0) - 128;
	lCb = BPXL_GET_COMPONENT(eSrcFormat, ulSrcPixel, 1) - 128;
	lY1 = BPXL_GET_COMPONENT(eSrcFormat, ulSrcPixel, 2) - 16;
	lY0 = BPXL_GET_COMPONENT(eSrcFormat, ulSrcPixel, 3) - 16;
	lY  = (BPXL_HAS_ALPHA(eSrcFormat) || (ulSrcAlign & 1)) ? lY1 : lY0;

	/* Convert to ARGB components */
	/* R = (Y - 16) * 1.164 + (Cr - 128) * 1.596 */
	/* G = (Y - 16) * 1.164 - (Cr - 128) * 0.813 - (Cb - 128) * 0.391 */
	/* B = (Y - 16) * 1.164 + (Cb - 128) * 2.018 */
	lA = (signed int) (BPXL_HAS_ALPHA(eSrcFormat) ? BPXL_GET_COMPONENT(eSrcFormat, ulSrcPixel, 3) : ulSrcAlpha);
	lR = (signed int) ((lY * 0x129FB + lCr * 0x19893) >> 16);
	lG = (signed int) ((lY * 0x129FB - lCr * 0x0D020 - lCb * 0x06418) >> 16);
	lB = (signed int) ((lY * 0x129FB + lCb * 0x2049B) >> 16);

	/* Clamp RGB components */
	lR = (lR > 0xFF) ? 0xFF : ((lR < 0) ? 0 : lR);
	lG = (lG > 0xFF) ? 0xFF : ((lG < 0) ? 0 : lG);
	lB = (lB > 0xFF) ? 0xFF : ((lB < 0) ? 0 : lB);

	/* Make destination pixel */
	return BPXL_MAKE_PIXEL(eDstFormat, lA, lR, lG, lB);
}

/***************************************************************************/
char *GetFormatString( BPXL_Format eFormat )
{
	switch( eFormat )
	{
	case BPXL_eCr8_Y18_Cb8_Y08: return "CrY1CbY0";
	case BPXL_eY18_Cr8_Y08_Cb8: return "Y1CrY0Cb";
	case BPXL_eY08_Cb8_Y18_Cr8: return "Y0CbY1Cr";
	case BPXL_eCb8_Y08_Cr8_Y18: return "CbY0CrY1";
	case BPXL_eA8_Y8_Cb8_Cr8:   return "AYCbCr";
	case BPXL_eA8_R8_G8_B8:     return "ARGB8888";
	case BPXL_eA8_B8_G8_R8:     return "ABGR8888";
	case BPXL_eR8_G8_B8_A8:     return "RGBA8888";
	case BPXL_eB8_G8_R8_A8:     return "BGRA8888";
	case BPXL_eX8_R8_G8_B8:     return "XRGB8888";
	case BPXL_eX8_B8_G8_R8:     return "XBGR8888";
	case BPXL_eR8_G8_B8_X8:     return "RGBX8888";
	case BPXL_eB8_G8_R8_X8:     return "BGRX8888";
	case BPXL_eR8_G8_B8:        return "RGB888";
	case BPXL_eB8_G8_R8:        return "BGR888";
	case BPXL_eR5_G6_B5:        return "RGB565";
	case BPXL_eB5_G6_R5:        return "BGR565";
	case BPXL_eA1_R5_G5_B5:     return "ARGB1555";
	case BPXL_eA1_B5_G5_R5:     return "ABGR1555";
	case BPXL_eR5_G5_B5_A1:     return "RGBA5551";
	case BPXL_eB5_G5_R5_A1:     return "BGRA5551";
	case BPXL_eX1_R5_G5_B5:     return "XRGB1555";
	case BPXL_eX1_B5_G5_R5:     return "XBGR1555";
	case BPXL_eR5_G5_B5_X1:     return "RGBX5551";
	case BPXL_eB5_G5_R5_X1:     return "BGRX5551";
	case BPXL_eW1_R5_G5_B5:     return "WRGB1555";
	case BPXL_eW1_B5_G5_R5:     return "WBGR1555";
	case BPXL_eR5_G5_B5_W1:     return "RGBW5551";
	case BPXL_eB5_G5_R5_W1:     return "BGRW5551";
	case BPXL_eA4_R4_G4_B4:     return "ARGB4444";
	case BPXL_eA4_B4_G4_R4:     return "ABGR4444";
	case BPXL_eR4_G4_B4_A4:     return "RGBA4444";
	case BPXL_eB4_G4_R4_A4:     return "BGRA4444";
	case BPXL_eX4_R4_G4_B4:     return "XRGB4444";
	case BPXL_eX4_B4_G4_R4:     return "XBGR4444";
	case BPXL_eR4_G4_B4_X4:     return "RGBX4444";
	case BPXL_eB4_G4_R4_X4:     return "BGRX4444";
	case BPXL_eP1:              return "P1";
	case BPXL_eP2:              return "P2";
	case BPXL_eP4:              return "P4";
	case BPXL_eP8:              return "P8";
	case BPXL_eA8_P8:           return "AP88";
	case BPXL_eL8:              return "L8";
	case BPXL_eL8_A8:           return "LA88";
	case BPXL_eA1:              return "A1";
	case BPXL_eA2:              return "A2";
	case BPXL_eA4:              return "A4";
	case BPXL_eA8:              return "A8";
	case BPXL_eW1:              return "W1";
	case BPXL_eZ16:             return "Z16";
	case BPXL_eCr8_Cb8_Y8_A8:   return "CrCbYA";
	case BPXL_eY8_P8:           return "Y8P8";
	case BPXL_eY8:              return "Y8";
	default:                    return "unknown";
	}
	return NULL;
}

#if (BTST_P_SUPPORT_GRC)

/***************************************************************************
Description:
	RGB to YCbCr color matrix table.
****************************************************************************/
/* Y  = R *  0.257 + G *  0.504 + B *  0.098 + 16  */
/* Cb = R * -0.148 + G * -0.291 + B *  0.439 + 128 */
/* Cr = R *  0.439 + G * -0.368 + B * -0.071 + 128 */
static int32_t s_ai32_Matrix_RGBtoYCbCr[20] = 
{ 
	(int32_t) ( 0.257f * (1 << 10)),   /*  R factor for Y */
	(int32_t) ( 0.504f * (1 << 10)),   /*  G factor for Y */
	(int32_t) ( 0.098f * (1 << 10)),   /*  B factor for Y */
	(int32_t) 0,                       /*  A factor for Y */
	(int32_t) (16 * (1 << 10)),        /* Increment for Y */
	(int32_t) (-0.148f * (1 << 10)),   /*  R factor for Cb */
	(int32_t) (-0.291f * (1 << 10)),   /*  G factor for Cb */
	(int32_t) ( 0.439f * (1 << 10)),   /*  B factor for Cb */
	(int32_t) 0,                       /*  A factor for Cb */
	(int32_t) (128 * (1 << 10)),       /* Increment for Cb */
	(int32_t) ( 0.439f * (1 << 10)),   /*  R factor for Cr */
	(int32_t) (-0.368f * (1 << 10)),   /*  G factor for Cr */
	(int32_t) (-0.071f * (1 << 10)),   /*  B factor for Cr */
	(int32_t) 0,                       /*  A factor for Cr */
	(int32_t) (128 * (1 << 10)),       /* Increment for Cr */
	(int32_t) 0,                       /*  R factor for A */
	(int32_t) 0,                       /*  G factor for A */
	(int32_t) 0,                       /*  B factor for A */
	(int32_t) (1 << 10),               /*  A factor for A */
	(int32_t) 0,                       /* Increment for A */
};

/***************************************************************************
Description:
	YCbCr to RGB color matrix table.
****************************************************************************/
/* R = Y * 1.164 + Cr * 1.596 - 223 */
/* G = Y * 1.164 - Cr * 0.813 - Cb * 0.391 + 135 */
/* B = Y * 1.164 + Cb * 2.018 - 277 */
static int32_t s_ai32_Matrix_YCbCrtoRGB[20] = 
{ 
	(int32_t) ( 1.164f * (1 << 10)),   /*  Y factor for R */
	(int32_t) 0,                       /* Cb factor for R */
	(int32_t) ( 1.596f * (1 << 10)),   /* Cr factor for R */
	(int32_t) 0,                       /*  A factor for R */
	(int32_t) (-223 * (1 << 10)),      /* Increment for R */
	(int32_t) ( 1.164f * (1 << 10)),   /*  Y factor for G */
	(int32_t) (-0.391f * (1 << 10)),   /* Cb factor for G */
	(int32_t) (-0.813f * (1 << 10)),   /* Cr factor for G */
	(int32_t) 0,                       /*  A factor for G */
	(int32_t) (134 * (1 << 10)),       /* Increment for G */
	(int32_t) ( 1.164f * (1 << 10)),   /*  Y factor for B */
	(int32_t) ( 2.018f * (1 << 10)),   /* Cb factor for B */
	(int32_t) 0,                       /* Cr factor for B */
	(int32_t) 0,                       /*  A factor for B */
	(int32_t) (-277 * (1 << 10)),      /* Increment for B */
	(int32_t) 0,                       /*  Y factor for A */
	(int32_t) 0,                       /* Cb factor for A */
	(int32_t) 0,                       /* Cr factor for A */
	(int32_t) (1 << 10),               /*  A factor for A */
	(int32_t) 0,                       /* Increment for A */
};

/***************************************************************************
Description:
	Color converts pixels from one color space into another, supporting 
	RGB to YCbCr and YCbCr to RGB. It uses a constant alpha when the 
	source pixel does not have an alpha component.
****************************************************************************/
void BGFX_Blit_ColorSpaceConvert(
	BGRC_Handle hGrc,
	BSUR_Surface_Handle hSrcSurface,
	BSUR_Surface_Handle hDstSurface,
	BGFX_Rect *pSrcRect,
	BGFX_Rect *pDstRect,
	uint32_t ulPixel )
{
	BPXL_Format eSrcFormat;
	BPXL_Format eDstFormat;

	BSUR_Surface_GetFormat( hSrcSurface, &eSrcFormat );
	BSUR_Surface_GetFormat( hDstSurface, &eDstFormat );

	/* Set source surface and rectangle */
	BGRC_Source_SetSurface( hGrc, hSrcSurface );
	BGRC_Source_SetRectangle( hGrc, pSrcRect->ulX, pSrcRect->ulY, pSrcRect->ulWidth, pSrcRect->ulHeight );

	/* Set source constant color */
	BGRC_Source_SetColor( hGrc, ulPixel );

	/* Set source color matrix for color space conversion */
	BGRC_Source_ToggleColorMatrix( hGrc, true );
	BGRC_Source_SetColorMatrix5x4( hGrc, BPXL_IS_RGB_FORMAT(eSrcFormat) ?
		s_ai32_Matrix_RGBtoYCbCr : s_ai32_Matrix_YCbCrtoRGB, 10 );

	/* Set destination surface to NULL (default) */
	BGRC_Destination_SetSurface( hGrc, NULL );

	/* Set output surface and rectangle */
	BGRC_Output_SetSurface( hGrc, hDstSurface );
	BGRC_Output_SetRectangle( hGrc, pDstRect->ulX, pDstRect->ulY, pDstRect->ulWidth, pDstRect->ulHeight );

	/* Set output selection to select source (default) */
	BGRC_Output_SetColorKeySelection( hGrc, 
		BGRC_Output_ColorKeySelection_eTakeSource, 
		BGRC_Output_ColorKeySelection_eTakeSource, 
		BGRC_Output_ColorKeySelection_eTakeDestination, 
		BGRC_Output_ColorKeySelection_eTakeDestination );

	/* Initiate blit */
#ifdef IKOS_EMULATION
	IKOS_IssueStateAndWait( hGrc );
#else
	BGRC_IssueStateAndWait( hGrc );
#endif

#ifdef DUMP_IMAGE
	BGFX_DumpImage( hDstSurface, hSrcSurface, 
		(uint8_t *) (BPXL_IS_RGB_FORMAT(eSrcFormat) ? "RGBtoYCbCr" : "YCbCrtoRGB") );
#endif
}

#endif

/* End of file */

