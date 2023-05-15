/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_fp.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 4/26/11 1:00p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_fp.h $
 * 
 * Hydra_Software_Devel/4   4/26/11 1:00p btosi
 * SW7405-4736: tweaked the base 10 conversion code
 * 
 * Hydra_Software_Devel/3   4/25/11 1:12p btosi
 * SW7405-4736: for BXDM_PPDBG, print fractional part of average STC delta
 * 
 * Hydra_Software_Devel/2   4/19/11 12:00p btosi
 * SW7405-4736: print parsed fields instead of raw data
 * 
 * Hydra_Software_Devel/1   2/16/10 10:51a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#ifndef bxdm_pp_FP_H__
#define bxdm_pp_FP_H__

#ifdef __cplusplus
extern "C" {
#endif

#if 0
}
#endif

#define BXDM_PictureProvider_P_FixedPoint_FractionalOverflow 0x10000L
#define BXDM_PictureProvider_P_FixedPoint_FractionalShift 16
#define BXDM_PictureProvider_P_FixedPoint_FractionalMask 0xFFFF

/* 
 * These constants are used to extract the digits to the right of the decimal point.
 */
#define BXDM_PictureProvider_P_FixedPoint_FractionalMsbMask 0x8000
#define BXDM_PictureProvider_P_FixedPoint_FractionalBitsOfPrecision 16

/* Fixed Point Artitmetic */
typedef struct BXDM_PPFP_P_DataType
{
      uint32_t uiWhole;
      uint32_t uiFractional;
} BXDM_PPFP_P_DataType;

void BXDM_PPFP_P_FixPtAdd(
   const BXDM_PPFP_P_DataType *pstOperandA,
   const BXDM_PPFP_P_DataType *pstOperandB,
   BXDM_PPFP_P_DataType *pstResult
   );

void BXDM_PPFP_P_FixPtSub(
   const BXDM_PPFP_P_DataType *pstOperandA, 
   const BXDM_PPFP_P_DataType *pstOperandB,
   BXDM_PPFP_P_DataType *pstResult
   );

void BXDM_PPFP_P_FixPtDiv(
   const BXDM_PPFP_P_DataType *pstOperand,
   const uint32_t uiDivisor,
   BXDM_PPFP_P_DataType *pstResult
   );

void BXDM_PPFP_P_FixPtMult(
   const BXDM_PPFP_P_DataType* pstOperand,
   const uint32_t uiMultiplier,
   BXDM_PPFP_P_DataType* pstResult
   );

void BXDM_PPFP_P_FixPtFractionalMul(
   const BXDM_PPFP_P_DataType* pstOperand,
   const uint16_t uiNumerator,
   const uint16_t uiDenominator,
   BXDM_PPFP_P_DataType* pstResult
   );

void BXDM_PPFP_P_FixPtBinaryFractionToBase10(
   BXDM_PPFP_P_DataType* pstOperand,
   uint32_t uiDstPrecision,
   uint32_t * puiBase10Fraction
   );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef bxdm_pp_FP_H__ */
