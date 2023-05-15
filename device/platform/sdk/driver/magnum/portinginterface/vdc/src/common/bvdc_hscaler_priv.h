/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_hscaler_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/20 $
 * $brcm_Date: 3/26/12 10:56a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_hscaler_priv.h $
 * 
 * Hydra_Software_Devel/20   3/26/12 10:56a yuxiaz
 * SW7425-2753: Fixed BVN errors for 3d conversion.
 * 
 * Hydra_Software_Devel/19   12/1/11 4:32p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 * 
 * Hydra_Software_Devel/SW7425-968/1   11/14/11 5:51p yuxiaz
 * SW7425-968: Added independent source clipping of right window in HSCL.
 * 
 * Hydra_Software_Devel/18   8/15/11 4:10p pntruong
 * SW7425-988: Implement de-ringing improvements for HSCL.
 *
 * Hydra_Software_Devel/17   11/15/10 4:41p tdo
 * SW7422-36: Handle "left only" method for BVN internal data path
 *
 * Hydra_Software_Devel/16   11/5/10 3:08p vanessah
 * SW7422-52:  Two kinds of deinterlacers in the same chip
 *
 * Hydra_Software_Devel/15   6/18/10 4:21p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/14   4/7/10 11:31a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/13   4/5/10 4:09p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/12   10/9/09 5:29p syang
 * SW7405-2516, SW3548-2439, SW7405-3046: rewrite HSCL setinfo_isr and
 * TryXferHrzSclToHscl_isr to do crop correctly; cleanup width alignment
 * for DCX_HSIZE_WORKAROUND
 *
 * Hydra_Software_Devel/11   6/18/09 5:51p syang
 * PR 55877, PR 55871, PR 56127, PR 55812: update to support 7340 and 7550
 *
 * Hydra_Software_Devel/10   3/16/09 10:48p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:30p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/9   10/23/08 9:40p tdo
 * PR46484:Initial VDC bringup for 7420
 *
 * Hydra_Software_Devel/8   10/1/08 10:36a pntruong
 * PR46485: Need to make versions visible.
 *
 * Hydra_Software_Devel/7   9/24/08 6:20p pntruong
 * PR47113, PR45308: Allow flexibility in selecting builtins and user
 * defined filter coefficients.
 *
 * Hydra_Software_Devel/6   9/24/08 3:24p tdo
 * PR46482: Fix compiling error
 *
 * Hydra_Software_Devel/5   9/24/08 1:16p tdo
 * PR46482: Change FIR programming for SCL block for 3548 B0
 *
 * Hydra_Software_Devel/4   9/17/08 3:47p tdo
 * PR46482: B0:Change FIR programming for SCL block
 *
 * Hydra_Software_Devel/3   9/9/08 9:04p tdo
 * PR45802: Remove unnecessary macros and unused code
 *
 * Hydra_Software_Devel/2   3/3/08 2:04p yuxiaz
 * PR39158, PR39155: Added dithering in HSCL. Move De-ring from MAD to
 * HSCL for 3548.
 *
 * Hydra_Software_Devel/1   2/28/08 6:22p hongtaoz
 * PR39154: added HSCL block;
 *
 ***************************************************************************/
#ifndef BVDC_HSCALER_PRIV_H__
#define BVDC_HSCALER_PRIV_H__

#include "bvdc.h"
#include "bchp_common.h"
#include "bvdc_common_priv.h"
#include "bvdc_subrul_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_scaler_priv.h"

#if (BVDC_P_SUPPORT_HSCL) || (BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
#include "bchp_hscl_0.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Core revs to track increatmental changes! */
#define BVDC_P_SUPPORT_HSCL_VER_1             (1) /* 3548 Ax */
#define BVDC_P_SUPPORT_HSCL_VER_2             (2) /* 7420 */
#define BVDC_P_SUPPORT_HSCL_VER_3             (3) /* 3548 Bx */
#define BVDC_P_SUPPORT_HSCL_VER_4             (4) /* 7340, 7550 */
#define BVDC_P_SUPPORT_HSCL_VER_5             (5) /* 7422, 7425 */
#define BVDC_P_SUPPORT_HSCL_VER_6             (6) /* 7231B0, 7344B0, 7346B0, 7425B0, Added more derring knobs */


/***************************************************************************
 * Private register cracking macros
 ***************************************************************************/
#if (BVDC_P_SUPPORT_HSCL) || (BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)

#define BVDC_P_HSCL_GET_REG_IDX(reg) \
	((BCHP##_##reg - BCHP_HSCL_0_REG_START) / sizeof(uint32_t))

/* Get/Set reg data */
#define BVDC_P_HSCL_GET_REG_DATA(reg) \
	(hHscaler->aulRegs[BVDC_P_HSCL_GET_REG_IDX(reg)])
#define BVDC_P_HSCL_SET_REG_DATA(reg, data) \
	(BVDC_P_HSCL_GET_REG_DATA(reg) = (uint32_t)(data))

/* Get with index. */
#define BVDC_P_HSCL_GET_REG_DATA_I(idx, reg) \
	(hHscaler->aulRegs[BVDC_P_HSCL_GET_REG_IDX(reg) + (idx)])

/* Get field */
#define BVDC_P_HSCL_GET_FIELD_NAME(reg, field) \
	(BVDC_P_GET_FIELD(BVDC_P_HSCL_GET_REG_DATA(reg), reg, field))

/* Compare field */
#define BVDC_P_HSCL_COMPARE_FIELD_DATA(reg, field, data) \
	(BVDC_P_COMPARE_FIELD_DATA(BVDC_P_HSCL_GET_REG_DATA(reg), reg, field, (data)))
#define BVDC_P_HSCL_COMPARE_FIELD_NAME(reg, field, name) \
	(BVDC_P_COMPARE_FIELD_NAME(BVDC_P_HSCL_GET_REG_DATA(reg), reg, field, name))

/* This macro does a write into RUL (write, addr, data). 3 dwords. */
#define BVDC_P_HSCL_WRITE_TO_RUL(reg, addr_ptr) \
{ \
	*addr_ptr++ = BRDC_OP_IMM_TO_REG(); \
	*addr_ptr++ = BRDC_REGISTER(BCHP##_##reg + hHscaler->ulRegOffset); \
	*addr_ptr++ = BVDC_P_HSCL_GET_REG_DATA(reg); \
}

/* This macro does a block write into RUL */
#define BVDC_P_HSCL_BLOCK_WRITE_TO_RUL(from, to, pulCurrent) \
do { \
	uint32_t ulBlockSize = \
		BVDC_P_REGS_ENTRIES(from, to);\
	*pulCurrent++ = BRDC_OP_IMMS_TO_REGS( ulBlockSize ); \
	*pulCurrent++ = BRDC_REGISTER(BCHP##_##from + hHscaler->ulRegOffset); \
	BKNI_Memcpy((void*)pulCurrent, \
		(void*)&(hHscaler->aulRegs[BVDC_P_HSCL_GET_REG_IDX(from)]), \
		ulBlockSize * sizeof(uint32_t)); \
	pulCurrent += ulBlockSize; \
} while(0)

/* This macro does a scaler reset RUL. */
#define BVDC_P_HSCL_RESET_RUL(hHscaler) \
{ \
	*pulCurrent++ = BRDC_OP_IMM_TO_REG(); \
	*pulCurrent++ = BRDC_REGISTER(hHscaler->ulResetRegAddr); \
	*pulCurrent++ = hHscaler->ulResetMask; \
	*pulCurrent++ = BRDC_OP_IMM_TO_REG(); \
	*pulCurrent++ = BRDC_REGISTER(hHscaler->ulResetRegAddr); \
	*pulCurrent++ = 0; \
}

/* number of registers in one block. */
#define BVDC_P_HSCL_REGS_COUNT      \
	BVDC_P_REGS_ENTRIES(HSCL_0_REG_START, HSCL_0_REG_END)

/* set scaling regions */
#define BVDC_P_HSCL_SET_HORZ_REGION02(region, end_pixel, step_inc) \
{ \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_DEST_PIC_REGION_##region##_STEP_DELTA) &= ~( \
		BCHP_MASK(HSCL_0_HORIZ_FIR_DEST_PIC_REGION_0_STEP_DELTA, SIZE)); \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_DEST_PIC_REGION_##region##_STEP_DELTA) |=  ( \
		BCHP_FIELD_DATA(HSCL_0_HORIZ_FIR_DEST_PIC_REGION_0_STEP_DELTA, SIZE, step_inc)); \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_DEST_PIC_REGION_##region##_END) &= ~( \
		BCHP_MASK(HSCL_0_HORIZ_DEST_PIC_REGION_0_END, POSITION)); \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_DEST_PIC_REGION_##region##_END) |=  ( \
		BCHP_FIELD_DATA(HSCL_0_HORIZ_DEST_PIC_REGION_0_END, POSITION, end_pixel)); \
}

/* Set horizontal scale down ratio */
#define BVDC_P_HSCL_HORZ_SCALE_REGION          (1)
#if (BVDC_P_SUPPORT_HSCL_VER < BVDC_P_SUPPORT_HSCL_VER_2)
#define BVDC_P_HSCL_SET_HORZ_RATIO(ratio) \
{ \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_INIT_STEP) &= ~( \
		BCHP_MASK(HSCL_0_HORIZ_FIR_INIT_STEP, SIZE)); \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_INIT_STEP) |=  ( \
		BCHP_FIELD_DATA(HSCL_0_HORIZ_FIR_INIT_STEP, SIZE, ratio)); \
}
#else
#define BVDC_P_HSCL_SET_HORZ_RATIO(ratio) \
{ \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_INIT_STEP_FRAC) &= ~( \
		BCHP_MASK(HSCL_0_HORIZ_FIR_INIT_STEP_FRAC, FRAC_SIZE)); \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_INIT_STEP_FRAC) |=  ( \
		BCHP_FIELD_DATA(HSCL_0_HORIZ_FIR_INIT_STEP_FRAC, FRAC_SIZE, \
		                ratio & ~(1 << BVDC_P_SCL_H_RATIO_F_BITS))); \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_INIT_STEP_INT) &= ~( \
		BCHP_MASK(HSCL_0_HORIZ_FIR_INIT_STEP_INT, INT_SIZE)); \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_INIT_STEP_INT) |=  ( \
		BCHP_FIELD_DATA(HSCL_0_HORIZ_FIR_INIT_STEP_INT, INT_SIZE, \
		                ratio >> BVDC_P_SCL_H_RATIO_F_BITS)); \
}
#endif

#define BVDC_P_HSCL_SET_HORZ_REGION02(region, end_pixel, step_inc) \
{ \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_DEST_PIC_REGION_##region##_STEP_DELTA) &= ~( \
		BCHP_MASK(HSCL_0_HORIZ_FIR_DEST_PIC_REGION_0_STEP_DELTA, SIZE)); \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_DEST_PIC_REGION_##region##_STEP_DELTA) |=  ( \
		BCHP_FIELD_DATA(HSCL_0_HORIZ_FIR_DEST_PIC_REGION_0_STEP_DELTA, SIZE, step_inc)); \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_DEST_PIC_REGION_##region##_END) &= ~( \
		BCHP_MASK(HSCL_0_HORIZ_DEST_PIC_REGION_0_END, POSITION)); \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_DEST_PIC_REGION_##region##_END) |=  ( \
		BCHP_FIELD_DATA(HSCL_0_HORIZ_DEST_PIC_REGION_0_END, POSITION, end_pixel)); \
}

#define BVDC_P_HSCL_SET_HORZ_STEP_MISC(end_pixel_1, step_init) \
{ \
	BVDC_P_HSCL_SET_HORZ_RATIO(step_init); \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_DEST_PIC_REGION_1_END) &= ~( \
		BCHP_MASK(HSCL_0_HORIZ_DEST_PIC_REGION_1_END, POSITION)); \
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_DEST_PIC_REGION_1_END) |=  ( \
		BCHP_FIELD_DATA(HSCL_0_HORIZ_DEST_PIC_REGION_1_END, POSITION, end_pixel_1)); \
}

/* fixed point stuffs */
#define BVDC_P_HSCL_H_PAN_SCAN_I_BITS          (1)
#define BVDC_P_HSCL_H_PAN_SCAN_F_BITS          (6)

#define BVDC_P_HSCL_H_RATIO_I_BITS             (4)
#define BVDC_P_HSCL_H_RATIO_F_BITS             (17)

#define BVDC_P_HSCL_LUMA_INIT_PHASE_I_BITS     (11)
#define BVDC_P_HSCL_LUMA_INIT_PHASE_F_BITS     (6)

#define BVDC_P_HSCL_CHROMA_INIT_PHASE_I_BITS   (11)
#define BVDC_P_HSCL_CHROMA_INIT_PHASE_F_BITS   (6)

#define BVDC_P_HSCL_COEFFS_I_BITS              (1)
#define BVDC_P_HSCL_COEFFS_F_BITS              (10)

#define BVDC_P_HSCL_LARGEST_F_BITS             (17)
#define BVDC_P_HSCL_ZERO_F_BITS                (0)

/* to normalize everything into S14.17 fixed format */
#define BVDC_P_HSCL_NORMALIZE(value, f_bit) ((value) << (BVDC_P_HSCL_LARGEST_F_BITS - (f_bit)))

/* to innormalize everything from S14.17 fixed format */
#define BVDC_P_HSCL_NORM_2_SPEC(value, f_bit) ((value) >> (BVDC_P_HSCL_LARGEST_F_BITS - (f_bit)))

/* Miscellaneous constants */
#define BVDC_P_HSCL_HORZ_REGIONS_COUNT         (1)

#define BVDC_P_HSCL_HORZ_FIR_TAP_COUNT         (8)
#define BVDC_P_HSCL_HORZ_FIR_PHASE_COUNT       (8)

#define BVDC_P_HSCL_FIR_TAP_COUNT_MAX          (8)
#define BVDC_P_HSCL_FIR_PHASE_COUNT_MAX        (8)

#define BVDC_P_HSCL_4TAP_HORZ_THRESHOLD_0      (1280)
#define BVDC_P_HSCL_4TAP_HORZ_THRESHOLD_1      (1024)

#define BVDC_P_HSCL_HORZ_HWF_FACTOR            (2)
#define BVDC_P_HSCL_SRC_HORZ_THRESHOLD         (960)

/* Make Horizontal ratio */
#define BVDC_P_HSCL_MAKE_H_RATIO(src, dst) \
	(BVDC_P_HSCL_NORM_2_SPEC((src), BVDC_P_HSCL_H_RATIO_F_BITS) / (dst))

#define BVDC_P_HSCL_HORZ_1_FIXED BVDC_P_FLOAT_TO_FIXED(1.000, \
	BVDC_P_HSCL_LUMA_INIT_PHASE_I_BITS, BVDC_P_HSCL_LUMA_INIT_PHASE_F_BITS)

#define BVDC_P_HSCL_FIR_COEFFS_MAX \
	(BVDC_P_HSCL_FIR_TAP_COUNT_MAX * BVDC_P_HSCL_FIR_PHASE_COUNT_MAX)

#define BVDC_P_HSCL_LAST UINT32_C(-1)
#else
#define BVDC_P_HSCL_REGS_COUNT     1
#ifndef BCHP_VNET_B_CAP_0_SRC_SOURCE_HSCL_0
#define BCHP_VNET_B_CAP_0_SRC_SOURCE_HSCL_0 0
#endif
#endif

/* VNET */
#if (BVDC_P_SUPPORT_HSCL)
#define BVDC_P_Hscaler_MuxAddr(hScl)       (BCHP_VNET_F_HSCL_0_SRC + (hScl)->eId * sizeof(uint32_t))
#define BVDC_P_Hscaler_PostMuxValue(hScl)  (BCHP_VNET_B_CAP_0_SRC_SOURCE_HSCL_0 + (hScl)->eId)
#else
#define BVDC_P_Hscaler_MuxAddr(hScl)       (0)
#define BVDC_P_Hscaler_PostMuxValue(hScl)  (0)
#endif
#define BVDC_P_Hscaler_SetVnet_isr(hScl, ulSrcMuxValue, eVnetPatchMode) \
   BVDC_P_SubRul_SetVnet_isr(&((hScl)->SubRul), ulSrcMuxValue, eVnetPatchMode)
#define BVDC_P_Hscaler_UnsetVnet_isr(hScl) \
   BVDC_P_SubRul_UnsetVnet_isr(&((hScl)->SubRul))

/***************************************************************************
 * Hscaler private data structures
 ***************************************************************************/
typedef struct BVDC_P_HscalerContext
{
	BDBG_OBJECT(BVDC_HSL)

	/* flag initial state, requires reset; */
	bool                           bInitial;
	uint32_t                       ulResetRegAddr;
	uint32_t                       ulResetMask;

	/* Fir coeff tables */
	const BVDC_P_FirCoeffTbl      *pHorzFirCoeffTbl;
	const BVDC_P_FirCoeffTbl      *pChromaHorzFirCoeffTbl;

	/* update flag. */
	uint32_t                       ulUpdateAll;

	BVDC_P_WindowId                eWinId;

	uint32_t                       ulPrevSrcWidth;
	int32_t                        lPrevHsclCutLeft;
	int32_t                        lPrevHsclCutLeft_R;
	uint32_t                       ulPrevHsclCutWidth;
	uint32_t                       ulPrevOutWidth;
	uint32_t                       ulPrevSrcHeight;
	uint32_t                       ulPrevCtIndexLuma;
	uint32_t                       ulPrevCtIndexChroma;
	BVDC_P_CtInput                 ePrevCtInputType;

	/* private fields. */
	BVDC_P_HscalerId               eId;
	uint32_t                       ulRegOffset; /* HSCL_0, HSCL_1, and etc. */
	uint32_t                       aulRegs[BVDC_P_HSCL_REGS_COUNT];
	BREG_Handle                    hReg;

	/* sub-struct to manage vnet and rul build opreations */
	BVDC_P_SubRulContext           SubRul;

	/* Dither */
	BVDC_P_DitherSetting           stDither;

	uint32_t                       ulHorzTaps;

	bool                           bDithering;
	bool                           bDeringing;

	BFMT_Orientation               ePrevSrcOrientation;
	BFMT_Orientation               ePrevDispOrientation;

	/* src width alignment: 1 for 444 format internally, 2 for 422 format internally */
	uint32_t                       ulSrcHrzAlign;
} BVDC_P_HscalerContext;


/***************************************************************************
 * Hscaler private functions
 ***************************************************************************/
BERR_Code BVDC_P_Hscaler_Create
	( BVDC_P_Hscaler_Handle           *phHscaler,
	  BVDC_P_HscalerId                 eHscalerId,
	  BVDC_P_Resource_Handle           hResource,
	  BREG_Handle                      hReg );

void BVDC_P_Hscaler_Destroy
	( BVDC_P_Hscaler_Handle            hHscaler );

void BVDC_P_Hscaler_Init
	( BVDC_P_Hscaler_Handle            hHscaler );

BERR_Code BVDC_P_Hscaler_AcquireConnect
	( BVDC_P_Hscaler_Handle            hHscaler,
	  BVDC_P_WindowId                  eWinId );

BERR_Code BVDC_P_Hscaler_ReleaseConnect_isr
	( BVDC_P_Hscaler_Handle           *phHscaler );

void BVDC_P_Hscaler_BuildRul_SetEnable_isr
	( BVDC_P_Hscaler_Handle            hHscaler,
	  BVDC_P_ListInfo                 *pList );

void BVDC_P_Hscaler_BuildRul_isr
	( const BVDC_P_Hscaler_Handle      hHscaler,
	  BVDC_P_ListInfo                 *pList,
	  BVDC_P_State                     eVnetState,
	  BVDC_P_PicComRulInfo            *pPicComRulInfo );

void BVDC_P_Hscaler_SetInfo_isr
	( BVDC_P_Hscaler_Handle            hHscaler,
	  BVDC_Window_Handle               hWindow,
	  const BVDC_P_PictureNodePtr      pPicture );

void BVDC_P_Hscaler_SetEnable_isr
	( BVDC_P_Hscaler_Handle            hHscaler,
	  bool                             bEnable );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_HSCALER_PRIV_H__ */
/* End of file. */
