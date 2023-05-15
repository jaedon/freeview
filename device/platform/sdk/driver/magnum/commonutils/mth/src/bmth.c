/*
 * ====================================================
 *
 * Portions of file Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * ====================================================
 */
/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmth.c $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 12/20/11 6:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/mth/bmth.c $
 * 
 * Hydra_Software_Devel/15   12/20/11 6:18p farshidf
 * SW3461-108: fix the log math function
 * 
 * Hydra_Software_Devel/14   4/20/11 11:38a farshidf
 * SWDTV-6190: add the BMTH_2560log10 used by frontend code
 * 
 * Hydra_Software_Devel/13   6/25/10 6:17p VISHK
 * SW7405-4552: Update bmth.h to include SunPro license
 * 
 * Hydra_Software_Devel/12   2/25/10 4:29p albertl
 * SW7550-273: Changed HILO 64 bit division to use David's bit shifting
 * implementation with 64-bit intermediates.
 * 
 * Hydra_Software_Devel/11   12/10/07 12:38p albertl
 * PR37989:  Removed functions using floats that are no longer needed.
 * 
 * Hydra_Software_Devel/10   6/12/07 1:52p albertl
 * PR31093:  Added new 64-bit hi lo math operations.
 * 
 * Hydra_Software_Devel/8   9/22/04 12:38p dlwin
 * PR 12728: Modified to support compiling with -O and with -pedantic
 * 
 * Hydra_Software_Devel/7   8/12/04 4:15p garylin
 * PR12273: use SDE math lib instead of Brcm math
 * 
 * Hydra_Software_Devel/6   12/30/03 6:29p dlwin
 * PR 9160: Removed warning from GNU C with -W
 * 
 * Hydra_Software_Devel/5   9/30/03 6:27p dlwin
 * Removed warnings when compiled for debug.
 * 
 * Hydra_Software_Devel/4   9/18/03 5:33p dlwin
 * Removed warnings
 * 
 * Hydra_Software_Devel/3   9/8/03 6:25p dlwin
 * Updated API function parameter comment for DocJet.
 * 
 * Hydra_Software_Devel/2   9/8/03 9:47a dlwin
 * Added code to implement these math functions.
 * 
 * Hydra_Software_Devel/1   9/3/03 3:31p dlwin
 * Initial version
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bmth.h"
#include "bmth_fix.h"


/*******************************************************************************
*
*	This file was implemented using the code from previous project.  To
*	minimize changes, the code has not been reformated.
*	
*
*******************************************************************************/

/* BDBG_MODULE(bmth); enable when adding debug prints */


void BMTH_HILO_64TO64_Neg(uint32_t xhi, uint32_t xlo, uint32_t *pouthi, uint32_t *poutlo)
{
	*poutlo = -(int32_t)xlo;
	*pouthi = -(int32_t)xhi - *poutlo;
}

void BMTH_HILO_32TO64_Mul(uint32_t x, uint32_t y, uint32_t *pouthi, uint32_t *poutlo)
{
	uint32_t xhi, xlo, yhi, ylo, ahi, alo, bhi, blo;

	xhi = (x >> 16) & 0xffff;
	xlo = x & 0xffff;
	yhi = (y >> 16) & 0xffff;
	ylo = y & 0xffff;

	*poutlo = xlo * ylo;

	BMTH_HILO_64TO64_Add(0, (xhi * ylo), 0, (yhi * xlo), &ahi, &alo);
	BMTH_HILO_64TO64_Add(0, ((*poutlo >> 16) & 0xffff), ahi, alo, &bhi, &blo);

	*poutlo = (*poutlo & 0xffff) | ((blo & 0xffff) << 16);
	*pouthi = ((blo >> 16) & 0xffff) | ((bhi &0xffff) << 16);
	*pouthi += xhi * yhi;
}

void BMTH_HILO_64TO64_Mul(uint32_t xhi, uint32_t xlo, uint32_t yhi, uint32_t ylo, uint32_t *pouthi, uint32_t *poutlo)
{
	BMTH_HILO_32TO64_Mul(xlo, ylo, pouthi, poutlo);
	*pouthi += (xhi * ylo) + (xlo * yhi);
}

void BMTH_HILO_64TO64_Add(uint32_t xhi, uint32_t xlo, uint32_t yhi, uint32_t ylo, uint32_t *pouthi, uint32_t *poutlo)
{
	uint32_t tempxlo = xlo;
	uint32_t tempxhi = xhi;
	uint32_t tempylo = ylo;
	uint32_t tempyhi = yhi;

	*poutlo = tempxlo + tempylo;
	*pouthi = tempxhi + tempyhi + (*poutlo < tempylo);
}

#if 0
void BMTH_HILO_64TO64_Div32(uint32_t xhi, uint32_t xlo, uint32_t y, uint32_t *pouthi, uint32_t *poutlo)
{
	uint32_t newxhi = xhi;
	uint32_t newxlo = xlo;
	uint32_t newq1 = 0;
	uint32_t r = 0;
	uint32_t curshift = 32; /* shift value of current 32 bits being divided */
	uint32_t rbits = 0;     /* remainder bits */
	uint32_t xlowbits = 0;  /* bits to concatenate from xlo with r to form new xhi*/
	*pouthi = 0;
	*poutlo = 0;
	uint32_t loop = 0;

	while (1)
	{
		newq1 = (newxhi / y);
		r = newxhi % y;

		BMTH_HILO_64TO64_Add(*pouthi, *poutlo,
			                 (curshift > 0)?  (newq1 >> (32 - curshift)) : 0,
							 (curshift < 32)? (newq1 << curshift) : 0,
							 pouthi, poutlo);

		if (curshift == 0)
		{
			break;
		}

		rbits = BMTH_FIX_LOG2(r) + 1;
		xlowbits = 32 - rbits;

		if (xlowbits > curshift)
		{
			xlowbits = curshift;
		}

		newxhi = (r << xlowbits) | (newxlo >> (32 - xlowbits));
		newxlo = newxlo << xlowbits;
		curshift -= xlowbits;
	}
	printf("loop %d\n", loop);
}
#else
#define BMTH_PRINTF(x)

void BMTH_HILO_64TO64_Div32(uint32_t xhi, uint32_t xlo, uint32_t y, uint32_t *pouthi, uint32_t *poutlo)
{
    /* this algo uses 64 bit add/subtract/bitshift, but not 64 bit multiply/divide */
    uint64_t xx = (uint64_t)xhi << 32 | xlo;
    uint64_t yy = y;
    uint64_t result = 0;

    if (y == 0) {
        *pouthi = 0;
        *poutlo = 0;
        return;
    }

    while (1) {
        uint64_t prev_yy = 0;
        uint64_t temp_yy = yy;
        uint64_t shift = 0;

        BMTH_PRINTF(("%llx / %llx = \n", xx, yy));
        while (xx >= temp_yy) {
            if (shift == 0)
                shift = 1;
            else
                shift <<= 1;
            prev_yy = temp_yy; /* if we fail the next test, prev_yy is it */
            temp_yy <<= 1;
        }
        if (!shift) break;

        BMTH_PRINTF(("%llx -= %llx, %llx += %llx\n", xx, prev_yy, result, shift));
        xx -= prev_yy;
        result += shift;
    }
    *pouthi = (uint32_t)(result >> 32);
    *poutlo = (uint32_t)(result & 0xFFFFFFFF);
    BMTH_PRINTF(("result = %08x%08x\n", *pouthi, *poutlo));
}
#endif

uint32_t BMTH_2560log10(uint32_t x)
{
  int32_t  x1;
  int32_t  x2;
  int32_t  x3;
  int32_t  x4;

  if (x == 0)
    return 0;

  x1 = 31;
  while (!((x >> x1) & 1))
    x1 = x1-1;
  x1 = x1+1;

  if (x1 > 20)
    x2 = (int32_t)(x >> (x1 - 8));
  else
    x2 = (int32_t)((x << 8) >> x1);
   
  x3 = -24381739 + x2*(62348 + (x2 << 7));
  x4 = 5907991 + (x2 << 16);

  return (unsigned)((770*(x3/(x4 >> 8) + (x1 << 8))) >> 8);
}
