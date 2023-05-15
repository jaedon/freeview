/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmth_fix.c $
 * $brcm_Revision: Hydra_Software_Devel/12 $
 * $brcm_Date: 3/17/11 4:31p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/mth/bmth_fix.c $
 * 
 * Hydra_Software_Devel/12   3/17/11 4:31p albertl
 * SW7405-5156: Added back int32_t cast which allows for proper sign
 * propogation when casting to int64_t.
 * 
 * Hydra_Software_Devel/11   2/14/11 4:54p albertl
 * SW35230-2975: Changed  multiplication to really use 64-bit  *
 * intermediate.
 * 
 * Hydra_Software_Devel/9   8/26/10 7:46p albertl
 * SW7405-4556, SW7405-4515: Changed multiplication to use 64-bit
 * intermediate.  Added fix to float macro.
 * 
 * Hydra_Software_Devel/8   2/17/09 6:48p albertl
 * PR51613: Updated to correct naming conventions.
 * 
 * Hydra_Software_Devel/7   11/14/08 6:12p albertl
 * PR47814:  Added debug code to check for fixed multiplication overflows.
 * 
 * Hydra_Software_Devel/6   8/4/08 1:57p tdo
 * PR45389: Make inline user definable because it is not compile for some
 * std.
 * 
 * Hydra_Software_Devel/6   8/4/08 1:46p tdo
 * PR45389: Make inline user definable because it is not compile for some
 * std.
 * 
 * Hydra_Software_Devel/6   8/4/08 1:43p tdo
 * PR45389: Make inline user definable because it is not compile for some
 * std.
 *
 * Hydra_Software_Devel/5   2/8/08 4:46p tdo
 * PR37722: Use inline function to optimize VDC math related performance
 *
 * Hydra_Software_Devel/4   9/20/07 1:25a albertl
 * PR35135:  Cleaned up color space matrices and changed them to use same
 * macro system.  Added color space conversion functionality to graphics
 * windows.
 *
 * Hydra_Software_Devel/3   8/9/07 12:27p tdo
 * PR33574: Convert more macro defines into functions to avoid compiling
 * errors in WinCE build
 *
 * Hydra_Software_Devel/2   8/3/07 5:18p tdo
 * PR33574: Make BMTH_FIX_SIGNED_MUL into function call to eliminate
 * compiling errors in WinCE platform build
 *
 * Hydra_Software_Devel/1   2/22/06 6:11p albertl
 * PR18913:  Initial revision of BMTH_FIX fixed point math library.
 *
 ***************************************************************************/
#include "bmth_fix.h"
#include "bstd.h"

/* sin magnitude lookup table in signed 2.10 notation */
static const int32_t s_lFixSinMagTable[] =
{
	0x0000, 0x000d, 0x0019, 0x0026, 0x0033, 0x003f, 0x004c, 0x0059,
	0x0065, 0x0072, 0x007e, 0x008b, 0x0097, 0x00a4, 0x00b0, 0x00bd,
	0x00c9, 0x00d6, 0x00e2, 0x00ee, 0x00fb, 0x0107, 0x0113, 0x011f,
	0x012b, 0x0138, 0x0144, 0x0150, 0x015b, 0x0167, 0x0173, 0x017f,
	0x018b, 0x0196, 0x01a2, 0x01ad, 0x01b9, 0x01c4, 0x01d0, 0x01db,
	0x01e6, 0x01f1, 0x01fc, 0x0207, 0x0212, 0x021d, 0x0228, 0x0232,
	0x023d, 0x0247, 0x0252, 0x025c, 0x0266, 0x0270, 0x027a, 0x0284,
	0x028e, 0x0297, 0x02a1, 0x02ab, 0x02b4, 0x02bd, 0x02c6, 0x02cf,
	0x02d8, 0x02e1, 0x02ea, 0x02f3, 0x02fb, 0x0303, 0x030c, 0x0314,
	0x031c, 0x0324, 0x032c, 0x0333, 0x033b, 0x0342, 0x0349, 0x0351,
	0x0358, 0x035f, 0x0365, 0x036c, 0x0372, 0x0379, 0x037f, 0x0385,
	0x038b, 0x0391, 0x0396, 0x039c, 0x03a1, 0x03a7, 0x03ac, 0x03b1,
	0x03b6, 0x03ba, 0x03bf, 0x03c3, 0x03c7, 0x03cb, 0x03cf, 0x03d3,
	0x03d7, 0x03da, 0x03de, 0x03e1, 0x03e4, 0x03e7, 0x03e9, 0x03ec,
	0x03ee, 0x03f1, 0x03f3, 0x03f5, 0x03f6, 0x03f8, 0x03fa, 0x03fb,
	0x03fc, 0x03fd, 0x03fe, 0x03ff, 0x03ff, 0x0400, 0x0400, 0x0400
};


/*************************************************************************
 * BMTH_FIX_SIGNED_CONVERT
 *
 *************************************************************************/
BMTH_INLINE uint32_t BMTH_FIX_SIGNED_CONVERT(uint32_t x, uint32_t inint, uint32_t infract, uint32_t outint, uint32_t outfract)
{
	uint32_t lFixOut;
	uint32_t ulFixInt;
	uint32_t ulSignBit;

	ulFixInt = (infract > outfract) ? (x >> (infract - outfract)) : (x << (outfract - infract));
	if(inint > outint)
	{
		ulSignBit = 0;
	}
	else
	{
		if(x & BMTH_P_FIX_SIGN_BIT(inint, infract))
		{
			ulSignBit = ~BMTH_P_FIX_SIGNED_MASK(inint, outfract);
		}
		else
		{
			ulSignBit = 0;
		}
	}
	lFixOut = (ulFixInt | ulSignBit) & BMTH_P_FIX_SIGNED_MASK(outint, outfract);

	return lFixOut;
}

/*************************************************************************
 * BMTH_FIX_SIGNED_MUL
 *
 *************************************************************************/
BMTH_INLINE uint32_t BMTH_FIX_SIGNED_MUL(uint32_t x, uint32_t y, uint32_t xint, uint32_t xfract, uint32_t yint, uint32_t yfract, uint32_t outint, uint32_t outfract)
{
	int64_t lFixOut; /* 64-bit to hold intermediate multiplied value of two 32-bit values */

#if 0
	/* used to debug overflows */
	int32_t lhi, llo;
	int32_t lExtendedX = (int32_t)BMTH_FIX_SIGNED_CONVERT(x, xint, xfract, BMTH_P_FIX_SIGNED_MAX_BITS - xfract, xfract);
	int32_t lExtendedY = (int32_t)BMTH_FIX_SIGNED_CONVERT(y, yint, yfract, BMTH_P_FIX_SIGNED_MAX_BITS - yfract, yfract);
	int32_t lSignMask = BMTH_P_FIX_SIGN_BIT(BMTH_P_FIX_SIGNED_MAX_BITS, 0);

	BMTH_HILO_32TO64_Mul (lExtendedX, lExtendedY, &lhi, &llo);

	if (lhi)
	{
		if ((!(lExtendedX & lSignMask) && !(lExtendedY & lSignMask)) ||
			(!(llo & lSignMask) && (((lExtendedX & lSignMask) && !(lExtendedY & lSignMask)) ||
			                         (!(lExtendedX & lSignMask) && (lExtendedY & lSignMask)))) ||
			((llo & lSignMask) && (lExtendedX & lSignMask) && (lExtendedY & lSignMask)))
		{
			printf("BMTH_FIX_SIGNED_MUL OVERFLOW! x: 0x%x, y: 0x%x, lhi: 0x%x, llo: 0x%x\n", lExtendedX, lExtendedY, lhi, llo);
		}
	}

	if(outfract > (xfract + yfract))
	{
		lFixOut = llo  << (outfract - (xfract + yfract));
	}
	else
	{
		lFixOut = llo >> ((xfract + yfract) - outfract);
	}

#else
	if(outfract > (xfract + yfract))
	{
		lFixOut = ((int64_t)(int32_t)BMTH_FIX_SIGNED_CONVERT(x, xint, xfract, BMTH_P_FIX_SIGNED_MAX_BITS - xfract, xfract) *
				            (int32_t)BMTH_FIX_SIGNED_CONVERT(y, yint, yfract, BMTH_P_FIX_SIGNED_MAX_BITS - yfract, yfract))
				  << (outfract - (xfract + yfract));
	}
	else
	{
		lFixOut = ((int64_t)(int32_t)BMTH_FIX_SIGNED_CONVERT(x, xint, xfract, BMTH_P_FIX_SIGNED_MAX_BITS - xfract, xfract) *
				            (int32_t)BMTH_FIX_SIGNED_CONVERT(y, yint, yfract, BMTH_P_FIX_SIGNED_MAX_BITS - yfract, yfract))
				  >> ((xfract + yfract) - outfract);
	}

#endif

	return (uint32_t)(lFixOut & (int32_t)BMTH_P_FIX_SIGNED_MASK(outint, outfract));
}

/*************************************************************************
 * BMTH_FIX_LOG2
 *
 *************************************************************************/
BMTH_INLINE uint32_t BMTH_FIX_LOG2(uint32_t x)
{
	const uint32_t bitarray[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
	const uint32_t shiftarray[] = {1, 2, 4, 8, 16};
	int32_t i = 0;
	uint32_t log = 0;
	for (i = 4; i >= 0; i--)
	{
		if (x & bitarray[i])
		{
			x >>= shiftarray[i];
			log |= shiftarray[i];
		}
	}
	return log;
}

/*************************************************************************
 * BMTH_P_FIX_SIGNED_MININTBITS
 *
 *************************************************************************/
BMTH_INLINE uint32_t BMTH_P_FIX_SIGNED_MININTBITS(uint32_t x, uint32_t intbits, uint32_t fractbits)
{
	uint32_t ulMinIntBits;
	int32_t ulFixInput = BMTH_FIX_SIGNED_CONVERT(x, intbits, fractbits, BMTH_P_FIX_SIGNED_MAX_BITS, 0);

	if(x & BMTH_P_FIX_SIGN_BIT(intbits, fractbits))
	{
		ulMinIntBits = BMTH_FIX_LOG2((uint32_t)((int32_t)ulFixInput * (-1)));
	}
	else
	{
		ulMinIntBits = BMTH_FIX_LOG2(ulFixInput) + 1;
	}
	return ulMinIntBits;
}


/*************************************************************************
 * BMTH_FIX_SIGNED_MOD
 *
 *************************************************************************/
uint32_t BMTH_FIX_SIGNED_MOD(uint32_t x, uint32_t y, uint32_t xint, uint32_t xfract, uint32_t yint, uint32_t yfract, uint32_t outint, uint32_t outfract)
{
	int32_t  lFixX;
	int32_t  lFixY;
	int32_t  lFixMod;
	uint32_t ulTempIntBits = 0;
	uint32_t ulTempFractBits = 0;

#if 1
	uint32_t ulTempXIntBits = 0;
	uint32_t ulTempYIntBits = 0;

	ulTempXIntBits = BMTH_P_FIX_SIGNED_MININTBITS(x, xint, xfract);
	ulTempYIntBits = BMTH_P_FIX_SIGNED_MININTBITS(y, yint, yfract);
	ulTempIntBits = (ulTempXIntBits > ulTempYIntBits) ? ulTempXIntBits : ulTempYIntBits;

	if (ulTempIntBits < outint)
	{
		ulTempIntBits = outint;
	}
#else
	ulTempIntBits = outint;
#endif

	ulTempFractBits = BMTH_P_FIX_SIGNED_MAX_BITS - ulTempIntBits;

	/* convert to common format */
	lFixX = BMTH_FIX_SIGNED_CONVERT(x, xint, xfract,
		                            ulTempIntBits, ulTempFractBits);
	lFixY = BMTH_FIX_SIGNED_CONVERT(y, yint, yfract,
		                            ulTempIntBits, ulTempFractBits);
	lFixMod = lFixX % lFixY;

	return BMTH_FIX_SIGNED_CONVERT(lFixMod, ulTempIntBits, ulTempFractBits, outint, outfract);
}


/***************************************************************************
 * {private}
 *
 */
uint32_t BMTH_P_FIX_SIGNED_SIN_CALC(uint32_t x, uint32_t xint, uint32_t xfract, uint32_t sinint, uint32_t sinfract)
{
	uint32_t ulRadIntBits      = xint;
	uint32_t ulRadFractBits    = xfract;
	uint32_t ulSinMagIntBits   = BMTH_P_FIX_SIGNED_SINMAG_INT_BITS;
	uint32_t ulSinMagFractBits = BMTH_P_FIX_SIGNED_SINMAG_FRACT_BITS;
	uint32_t ulInterpIntBits   = BMTH_P_FIX_SIGNED_RAD_INTERP_INT_BITS;
	uint32_t ulInterpFractBits = BMTH_P_FIX_SIGNED_RAD_INTERP_FRACT_BITS;

	int32_t ulFixX = x;
	int32_t ulFixXModPi;
	int32_t ulFixFirstQuadX;
	int32_t ulFixPi = BMTH_FIX_SIGNED_GET_PI(ulRadIntBits, ulRadFractBits);
	int32_t ulFixHalfPi = ulFixPi / 2;

	uint32_t ulRadShiftBits = 0;
	uint32_t ulSinMagTableSizeBits = 0;
	uint32_t ulTotalBits = 0;
	uint32_t ulSinMagIdx = 0;
	int32_t ulFixSinMag = 0;
	int32_t ulFixSinMagNext = 0;

	int32_t ulFixInterpNum = 0;
	int32_t ulFixInterpDem = 0;
	uint32_t ulFixInterpCoeff = 0;


	/* convert to positive angle */
	ulFixX = (ulFixX & BMTH_P_FIX_SIGN_BIT(ulRadIntBits, ulRadFractBits)) ?
			 (ulFixX + (2 * ulFixPi)) : ulFixX;
	ulFixX &= BMTH_P_FIX_SIGNED_MASK(ulRadIntBits, ulRadFractBits);

	ulFixXModPi = BMTH_FIX_SIGNED_MOD(ulFixX, ulFixPi,
	                                    ulRadIntBits, ulRadFractBits,
	                                    ulRadIntBits, ulRadFractBits,
	                                    ulRadIntBits, ulRadFractBits);

	/* convert to first quadrant equivalent with same sin value */
	ulFixFirstQuadX = (ulFixXModPi <= ulFixHalfPi) ?
			ulFixXModPi : (ulFixPi - ulFixXModPi);

	/* get sin magnitude table index */
	ulSinMagTableSizeBits = BMTH_P_FIX_SIGNED_MININTBITS((BMTH_P_FIX_SINMAG_TABLE_SIZE - 1), BMTH_P_FIX_SIGNED_MAX_BITS, 0);
	ulTotalBits = ulRadIntBits + ulRadFractBits + ulSinMagTableSizeBits;

	if (ulTotalBits > BMTH_P_FIX_SIGNED_MAX_BITS)
	{
		ulRadShiftBits = ulTotalBits - BMTH_P_FIX_SIGNED_MAX_BITS;
	}

	ulSinMagIdx = ((ulFixFirstQuadX >> ulRadShiftBits) * (BMTH_P_FIX_SINMAG_TABLE_SIZE - 1)) /
		           (ulFixHalfPi >> ulRadShiftBits);
	ulFixSinMag = s_lFixSinMagTable[ulSinMagIdx];
	ulFixSinMagNext = (ulSinMagIdx < (BMTH_P_FIX_SINMAG_TABLE_SIZE - 1)) ?
		(s_lFixSinMagTable[ulSinMagIdx + 1]) : (s_lFixSinMagTable[ulSinMagIdx]);


	ulFixInterpNum = ulFixFirstQuadX - (((ulSinMagIdx * (ulFixHalfPi >> ulRadShiftBits)) /
	                                     (BMTH_P_FIX_SINMAG_TABLE_SIZE - 1)) << ulRadShiftBits);
	ulFixInterpDem = ulFixHalfPi / BMTH_P_FIX_SINMAG_TABLE_SIZE;


	ulFixInterpNum = BMTH_FIX_SIGNED_CONVERT(ulFixInterpNum,
											 ulRadIntBits, ulRadFractBits,
											 ulInterpIntBits, ulInterpFractBits);

	ulFixInterpDem = BMTH_FIX_SIGNED_CONVERT(ulFixInterpDem,
											 ulRadIntBits, ulRadFractBits,
											 ulInterpIntBits, ulInterpFractBits);

	ulFixInterpCoeff = BMTH_FIX_SIGNED_DIV(ulFixInterpNum, ulFixInterpDem,
								           ulInterpIntBits, ulInterpFractBits,
								           ulInterpIntBits, ulInterpFractBits,
								           ulInterpIntBits, ulInterpFractBits);

	ulFixSinMag = BMTH_FIX_SIGNED_CONVERT(ulFixSinMag,
										  ulSinMagIntBits, ulSinMagFractBits,
										  ulInterpIntBits, ulInterpFractBits);

	ulFixSinMagNext = BMTH_FIX_SIGNED_CONVERT(ulFixSinMagNext,
											  ulSinMagIntBits, ulSinMagFractBits,
											  ulInterpIntBits, ulInterpFractBits);

	return (BMTH_FIX_SIGNED_CONVERT(ulFixSinMag,
									ulInterpIntBits, ulInterpFractBits,
									sinint, sinfract) +
		    BMTH_FIX_SIGNED_MUL((ulFixSinMagNext - ulFixSinMag), ulFixInterpCoeff,
							    ulInterpIntBits, ulInterpFractBits,
								ulInterpIntBits, ulInterpFractBits,
	                            sinint, sinfract)) *
		   ((ulFixX > ulFixPi) ? -1 : 1);  /* add the sign back */
}


/*************************************************************************
 * BMTH_FIX_SIGNED_COS
 *
 *************************************************************************/
uint32_t BMTH_FIX_SIGNED_COS(uint32_t x, uint32_t xint, uint32_t xfract, uint32_t sinint, uint32_t sinfract)
{
	uint32_t ulFixX;
	uint32_t ulRadIntBits      = BMTH_P_FIX_SIGNED_RAD_INT_BITS;
	uint32_t ulRadFractBits    = BMTH_P_FIX_SIGNED_RAD_FRACT_BITS;

	ulFixX = BMTH_FIX_SIGNED_RADTO2PI(x, xint, xfract, ulRadIntBits, ulRadFractBits);
	ulFixX += BMTH_FIX_SIGNED_GET_HALF_PI(ulRadIntBits, ulRadFractBits);

	return BMTH_P_FIX_SIGNED_SIN_CALC(ulFixX, ulRadIntBits, ulRadFractBits, sinint, sinfract);
}


/*************************************************************************
 * BMTH_FIX_SIGNED_SIN
 *
 *************************************************************************/
uint32_t BMTH_FIX_SIGNED_SIN(uint32_t x, uint32_t xint, uint32_t xfract, uint32_t sinint, uint32_t sinfract)
{
	uint32_t ulFixX;
	uint32_t ulRadIntBits      = BMTH_P_FIX_SIGNED_RAD_INT_BITS;
	uint32_t ulRadFractBits    = BMTH_P_FIX_SIGNED_RAD_FRACT_BITS;

	ulFixX = BMTH_FIX_SIGNED_RADTO2PI(x, xint, xfract, ulRadIntBits, ulRadFractBits);

	return BMTH_P_FIX_SIGNED_SIN_CALC(ulFixX, ulRadIntBits, ulRadFractBits, sinint, sinfract);
}


/* end of file */
