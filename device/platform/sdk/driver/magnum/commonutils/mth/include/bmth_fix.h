/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmth_fix.h $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 8/26/10 7:46p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/mth/bmth_fix.h $
 * 
 * Hydra_Software_Devel/9   8/26/10 7:46p albertl
 * SW7405-4556, SW7405-4515: Changed multiplication to use 64-bit
 * intermediate.  Added fix to float macro.
 * 
 * Hydra_Software_Devel/8   8/4/08 1:44p tdo
 * PR45389: Make inline user definable because it is not compile for some
 * std.
 *
 * Hydra_Software_Devel/7   4/2/08 5:17p albertl
 * PR35135:  Changed BMTH_FIX_SIGNED_FTOFIX to round float values to
 * nearest fixed point value.
 *
 * Hydra_Software_Devel/6   9/20/07 1:26a albertl
 * PR35135:  Cleaned up color space matrices and changed them to use same
 * macro system.  Added color space conversion functionality to graphics
 * windows.
 *
 * Hydra_Software_Devel/5   8/9/07 12:27p tdo
 * PR33574: Convert more macro defines into functions to avoid compiling
 * errors in WinCE build
 *
 * Hydra_Software_Devel/4   8/3/07 5:19p tdo
 * PR33574: Make BMTH_FIX_SIGNED_MUL into function call to eliminate
 * compiling errors in WinCE platform build
 *
 * Hydra_Software_Devel/3   6/12/07 7:17p albertl
 * PR31093:  Reverted to older macros that handled signed bits properly.
 *
 * Hydra_Software_Devel/2   6/11/07 6:56p albertl
 * PR31093:  Fixed sign errors in macros.
 *
 * Hydra_Software_Devel/1   2/22/06 6:10p albertl
 * PR18913:  Initial revision of BMTH_FIX fixed point math library.
 *
 ***************************************************************************/
#ifndef BMTH_FIX_H__
#define BMTH_FIX_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BMTH_INLINE
#define BMTH_INLINE
#endif

#include "bstd.h"
#include "bmth_fix_priv.h"

/***************************************************************************
Summary:
	convert one fixed point value to another
***************************************************************************/
BMTH_INLINE uint32_t BMTH_FIX_SIGNED_CONVERT(uint32_t x, uint32_t inint, uint32_t infract, uint32_t outint, uint32_t outfract);

/***************************************************************************
Summary:
	convert fixed point to integer
***************************************************************************/
#define BMTH_FIX_SIGNED_FIXTOI(x, intbits, fractbits) \
	(((x) & BMTH_P_FIX_SIGN_BIT(intbits, fractbits)) ? \
		((x) >> (fractbits) | ~BMTH_P_FIX_SIGNED_MASK(intbits, fractbits)) : \
		((x) >> (fractbits)))

/***************************************************************************
Summary:
	convert integer to fixed point
***************************************************************************/
#define BMTH_FIX_SIGNED_ITOFIX(x, intbits, fractbits) \
	(((x) << fractbits) & BMTH_P_FIX_SIGNED_MASK(intbits, fractbits))

/***************************************************************************
Summary:
	convert float to fixed point
***************************************************************************/
#define BMTH_FIX_SIGNED_FTOFIX(x, intbits, fractbits) \
	((int32_t)(((x) * (1 << fractbits)) + ((x > 0) ? 0.5f : -0.5f)) & BMTH_P_FIX_SIGNED_MASK(intbits, fractbits))

/***************************************************************************
Summary:
	convert fixed to float point
***************************************************************************/
#define BMTH_FIX_SIGNED_FIXTOF(x, intbits, fractbits) \
	((int32_t)((BMTH_P_FIX_SIGN_BIT(intbits, fractbits) & x) ? \
		-((~BMTH_P_FIX_SIGN_BIT(intbits, fractbits) & ~x) + 1) : x ) / (float)(1 << fractbits))

/***************************************************************************
Summary:
	fixed point operation multiply
***************************************************************************/
BMTH_INLINE uint32_t BMTH_FIX_SIGNED_MUL(uint32_t x, uint32_t y, uint32_t xint, uint32_t xfract, uint32_t yint, uint32_t yfract, uint32_t outint, uint32_t outfract);

/***************************************************************************
Summary:
	fixed point operation divide
***************************************************************************/
#define BMTH_FIX_SIGNED_DIV(x, y, xint, xfract, yint, yfract, outint, outfract) \
( \
	(uint32_t)(((outfract > ((xfract) - (yfract))) ? \
					((int32_t)BMTH_FIX_SIGNED_CONVERT(x, xint, xfract, BMTH_P_FIX_SIGNED_MAX_BITS - (xfract), xfract) << (outfract - (xfract - yfract))) : \
					((int32_t)BMTH_FIX_SIGNED_CONVERT(x, xint, xfract, BMTH_P_FIX_SIGNED_MAX_BITS - (xfract), xfract) >> ((xfract - yfract) - outfract))) / \
			   (int32_t)BMTH_FIX_SIGNED_CONVERT(y, yint, yfract, BMTH_P_FIX_SIGNED_MAX_BITS - yfract, yfract)) \
)


/***************************************************************************
Summary:
	fixed point operation modulus
***************************************************************************/
uint32_t BMTH_FIX_SIGNED_MOD(uint32_t x, uint32_t y, uint32_t xint, uint32_t xfract, uint32_t yint, uint32_t yfract, uint32_t outint, uint32_t outfract);


/***************************************************************************
Summary:
	fixed point log 2
***************************************************************************/
BMTH_INLINE uint32_t BMTH_FIX_LOG2(uint32_t x);


/***************************************************************************
Summary:
	gets pi in fixed point format
****************************************************************************/
#define BMTH_FIX_SIGNED_GET_PI(intbits, fractbits) \
	 ((BMTH_P_FIX_UNSIGNED_PI >> (BMTH_P_FIX_UNSIGNED_RAD_FRACT_BITS - fractbits)) & \
	  BMTH_P_FIX_SIGNED_MASK(intbits, fractbits))


/***************************************************************************
Summary:
	gets pi/2 in fixed point format
****************************************************************************/
#define BMTH_FIX_SIGNED_GET_HALF_PI(intbits, fractbits) \
	(BMTH_FIX_SIGNED_GET_PI(intbits, fractbits) / 2)


/***************************************************************************
Summary:
	converts a radian to a value between -2 pi and 2 pi
****************************************************************************/
#define BMTH_FIX_SIGNED_RADTO2PI(x, intbits, fractbits, outint, outfract) \
	(BMTH_FIX_SIGNED_MOD(x, (2 * BMTH_FIX_SIGNED_GET_PI(outint, outfract)), \
	                     intbits, fractbits, \
                         BMTH_P_FIX_SIGNED_RAD_INT_BITS, BMTH_P_FIX_SIGNED_RAD_FRACT_BITS, \
                         outint, outfract))


/***************************************************************************
Summary:
	sin, with linear interpolation
****************************************************************************/
uint32_t BMTH_FIX_SIGNED_SIN(uint32_t x, uint32_t xint, uint32_t xfract, uint32_t sinint, uint32_t sinfract);


/***************************************************************************
Summary:
	cos, with linear interpolation
****************************************************************************/
uint32_t BMTH_FIX_SIGNED_COS(uint32_t x, uint32_t xint, uint32_t xfract, uint32_t sinint, uint32_t sinfract);


/***************************************************************************
Summary:
	finds the minimum number of bits needed to represent integer value
****************************************************************************/
BMTH_INLINE uint32_t BMTH_P_FIX_SIGNED_MININTBITS(uint32_t x, uint32_t intbits, uint32_t fractbits);

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef BMTH_FIX_H__ */
/* End of File */
