/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmth.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 4/20/11 1:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/mth/bmth.h $
 * 
 * Hydra_Software_Devel/5   4/20/11 1:28p farshidf
 * SWDTV-6190: add the 35233 files
 * 
 * Hydra_Software_Devel/5   4/20/11 11:42a farshidf
 * SWDTV-6190: add BMTH_2560log10
 * 
 * Hydra_Software_Devel/4   12/10/07 12:38p albertl
 * PR37989:  Removed functions using floats that are no longer needed.
 * 
 * Hydra_Software_Devel/3   6/11/07 6:51p albertl
 * PR28893:  Added new hi lo 64-bit operations.
 * 
 * Hydra_Software_Devel/2   9/8/03 6:25p dlwin
 * Updated API function parameter comment for DocJet.
 * 
 * Hydra_Software_Devel/1   9/3/03 3:31p dlwin
 * Initial version
 * 
 ***************************************************************************/

/*= Module Overview *********************************************************
<verbatim>

Overview
This common utility module provides the required math functions.


Design
none


Usage
none


Sample Code
none


</verbatim>
***************************************************************************/


#ifndef BMTH_H__
#define BMTH_H__


#ifdef __cplusplus
extern "C" {
#endif


void BMTH_HILO_64TO64_Neg(uint32_t xhi, uint32_t xlo, uint32_t *pouthi, uint32_t *poutlo);
void BMTH_HILO_32TO64_Mul(uint32_t x, uint32_t y, uint32_t *pouthi, uint32_t *poutlo);
void BMTH_HILO_64TO64_Mul(uint32_t xhi, uint32_t xlo, uint32_t yhi, uint32_t ylo, uint32_t *pouthi, uint32_t *poutlo);
void BMTH_HILO_64TO64_Add(uint32_t xhi, uint32_t xlo, uint32_t yhi, uint32_t ylo, uint32_t *pouthi, uint32_t *poutlo);
void BMTH_HILO_64TO64_Div32(uint32_t xhi, uint32_t xlo, uint32_t y, uint32_t *pouthi, uint32_t *poutlo);
uint32_t BMTH_2560log10(uint32_t x);

#ifdef __cplusplus
}
#endif
 
#endif



