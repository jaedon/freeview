/***************************************************************************
 *     Copyright (c) 2004-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_udivdi3.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 1/16/08 5:24p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/linux/driver/build/97038/bsettop_udivdi3.c $
 * 
 * 2   1/16/08 5:24p vsilyaev
 * PR 38682: Remove unneeded #include's
 * 
 * 1   9/23/05 6:41p mphillip
 * PR10860: Implement a non-GPL (i.e. non-libgcc) udivdi3 to handle the
 * 97038 Bx encoder platform, which requires frequent uint64 divisions...
 * 
 *******************************************************/
#include "bstd.h"

/* This is a custom re-implementation of udivdi3 (a function call emitted by gcc to handle 
 * long long integer division.  The basic algorithm handles certain easy cases first, then 
 * goes into binary long division
 */

uint64_t __udivdi3 (uint64_t n, uint64_t d) /* [n]umerator, [d]enominator */
{
	uint64_t r;
	if (n == 0) {
	/* If n == 0, special case to 0 (0/x == 0) */
		r = 0;
	} else if (d > n) {
	/* If d > n, special case to 0 (n would be the remainder) */
		r = 0;
	} else if (n < (d*2)) {
	/* If n < d*2, special case to 1 */
		r = 1;
	} else if ((d <= (uint64_t)0x00000000FFFFFFFF) && (n <= (uint64_t)0x00000000FFFFFFFF)) {
	/* If they are 32bit quantities, go ahead and perform a basic 32bit div instead */
		uint32_t n32 = (uint32_t)n;
		uint32_t d32 = (uint32_t)d;
		uint32_t r32 = n32/d32;

		r = r32;
	} else { /* Otherwise, complicated division time */

		/* This segment of code is based on the routine by Ian Kaplan at 
		 * http://www.bearcave.com/software/divide.htm, which was licensed at 
		 * the time as:
		 *
		 * "Use of this program, for any purpose, is granted the author, Ian 
		 * Kaplan, as long as this copyright notice is included in the source code 
		 * or any source code derived from this program.  The user assumes all
		 * responsibility for using this code.
		 *
		 * Ian Kaplan, October 1996
		 */
		uint64_t t = 0, tmp = 0;
		uint64_t remainder = 0;
		uint32_t bits = 64;
		uint32_t ix;

		r = 0;
		/* Figure out about what the order of the remainder would be, to shortcut 
		 * a great deal of the math */
		while (remainder < d) {
			remainder = (remainder << 1) | (n>>63);
			tmp = n;
			n = n << 1;
			bits--;
		}
		/* Undo the last step, since we just went one too far */
		n = tmp;
		remainder = remainder >> 1;
		bits++;
		/* And now, buckle down and do the rest of the work */
		for (ix=0; ix < bits; ix++) {
			remainder = (remainder << 1) | (n>>63);
			tmp = remainder - d;
			t = !(tmp>>63);
			n = n << 1;
			r = (r << 1) | t;
			if (t) {
				remainder = tmp;
			}
		}
		/* End of code based on Ian Kaplan's work. */
	}
	return r;
}
