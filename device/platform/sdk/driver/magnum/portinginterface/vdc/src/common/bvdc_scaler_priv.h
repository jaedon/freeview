/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bvdc_scaler_priv.h $
* $brcm_Revision: Hydra_Software_Devel/121 $
* $brcm_Date: 8/15/12 6:20p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_scaler_priv.h $
* 
* Hydra_Software_Devel/121   8/15/12 6:20p tdo
* SW7445-8: Create Appframework emulation environment for 7445 A0
*
* Hydra_Software_Devel/120   6/15/12 10:35a vanessah
* SW7435-237: reset vnet channel connections
*
* Hydra_Software_Devel/119   5/30/12 6:13p tdo
* SW7425-1210: SCL: Veritical Block Averaging theshold should be base on
* scaler's veritical tap.
*
* Hydra_Software_Devel/118   3/6/12 12:46a hongtaoz
* SW7435-9: added scl_5 support;
*
* Hydra_Software_Devel/117   2/7/12 11:07a vanessah
* SW7425-2296: removal of duplicate vert line store depth variable
*
* Hydra_Software_Devel/116   2/6/12 9:36p vanessah
* SW7425-2296: use scaler to do Orientation conversion
*
* Hydra_Software_Devel/115   9/7/11 12:53p tdo
* SW7425-1210: Undo previous check in, not working with old SCL yet.
*
* Hydra_Software_Devel/113   8/30/11 11:12a pntruong
* SW7425-1210: Updated block averaging theshold to be based on number of
* tap uses.
*
* Hydra_Software_Devel/112   8/15/11 4:10p pntruong
* SW7425-982: Implement de-ringing and de-jagging improvements for SCL.
*
* Hydra_Software_Devel/111   4/25/11 3:50p yuxiaz
* SW7425-348: Added software workaround for MADR alignment.
*
* Hydra_Software_Devel/110   12/9/10 11:22a yuxiaz
* SW7422-36: Bring up 3D for CAP-SCL case.
*
* Hydra_Software_Devel/109   11/10/10 6:57p hongtaoz
* SW7425-13: fixed SCL vertical scaling ratio and offset programming;
* reuse the legacy fixed point precision for bringup; TODO: improve
* fixed point math to support higher precision VSCL;
*
* Hydra_Software_Devel/108   10/12/10 3:56p hongtaoz
* SW7425-13, SW7425-30: added the 4th SCL support for 7425;
*
* Hydra_Software_Devel/107   9/15/10 7:52p vanessah
* SW7422-49:  5-zone nonlinear filtering support. backwards compatible
* with 3-zone compatibility
*
 * Hydra_Software_Devel/106   8/26/10 5:28p tdo
 * SW7422-57: Add simple vdc support
*
* Hydra_Software_Devel/105   4/7/10 11:34a tdo
* SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
*
* Hydra_Software_Devel/104   4/5/10 4:13p tdo
* SW3548-2814: Improvements to VDC ulBlackMagic
*
* Hydra_Software_Devel/103   2/8/10 5:37p pntruong
* SW3548-2772: Refresh check of both luma/chroma coeffs set.
*
* Hydra_Software_Devel/102   10/29/09 9:21a rpan
* SW7342-48: Fixed SCL_HORIZ_FIR_INIT_STEP_FRAC bit mask.
*
* Hydra_Software_Devel/101   7/30/09 10:35a pntruong
* PR57216: Added support for scaler split screen demo modes.
*
* Hydra_Software_Devel/100   6/18/09 5:51p syang
* PR 55877, PR 55871, PR 56127, PR 55812: update to support 7340 and 7550
*
* Hydra_Software_Devel/99   6/11/09 5:52p pntruong
* PR55926: Initialized new vars.
*
* Hydra_Software_Devel/98   6/11/09 4:36p syang
* PR 55926: make sure changed vert coeff is built into RUL
*
* Hydra_Software_Devel/97   6/11/09 11:39a syang
* PR 55926:  avoid using point-sample if init-phase is not int
*
* Hydra_Software_Devel/96   6/2/09 3:03p syang
* PR 55646:  smoothly filtering  in field inversion cases
*
* Hydra_Software_Devel/95   5/21/09 3:01p tdo
* PR36666: Consider Kell Factor for SCL Coefficient selection
*
* Hydra_Software_Devel/94   11/4/08 7:10p pntruong
* PR48029: Added control for gain/horz/core weight factor for dejagging.
* Enforced dejagging on only if passed threshold.
*
* Hydra_Software_Devel/93   10/28/08 9:15p pntruong
* PR48028, PR48029: Allow controlling of scaler's dejagging and
* deringing.
*
* Hydra_Software_Devel/92   10/20/08 1:47p pntruong
* PR48032: Apply Kell factor for progressive to interlaced scaling case.
*
* Hydra_Software_Devel/91   10/9/08 2:51p syang
* PR 42693:  perform even numebr align for SCL-1/2 according to eId (for
* 3548 B0)
*
* Hydra_Software_Devel/90   10/1/08 10:36a pntruong
* PR46485: Need to make versions visible.
*
* Hydra_Software_Devel/89   9/24/08 6:18p pntruong
* PR47113, PR45308: Allow flexibility in selecting builtins and user
* defined filter coefficients.
*
* Hydra_Software_Devel/88   9/24/08 1:59p tdo
* PR46482: Fix warning
*
* Hydra_Software_Devel/87   9/24/08 1:16p tdo
* PR46482: Change FIR programming for SCL block for 3548 B0
*
* Hydra_Software_Devel/86   9/23/08 4:04p tdo
* PR46486: Program 444->422 downsampler and 422->444 upsampler for video
* scaler for 3548 B0
*
* Hydra_Software_Devel/85   9/19/08 3:56p tdo
* PR46481: Add dejagging to SCL implementation for 3548 B0
*
* Hydra_Software_Devel/84   9/17/08 3:48p tdo
* PR46482: B0:Change FIR programming for SCL block
*
* Hydra_Software_Devel/83   9/9/08 9:02p tdo
* PR45802: Remove unnecessary macros and unused code
*
* Hydra_Software_Devel/82   8/22/08 10:28a rpan
* PR45979: Remove VBI passthru support.
*
* Hydra_Software_Devel/81   8/8/08 3:30p yuxiaz
* PR45484: Enable Dithering in VDC.
*
* Hydra_Software_Devel/80   7/7/08 1:55p pntruong
* PR44617: Update 4-taps linebuffer threshold in SCL.
*
* Hydra_Software_Devel/79   6/26/08 7:18p pntruong
* PR44134: Allow user selection of builtin coeffs from 01->26 for
* increasing of softness -> sharpness.
*
* Hydra_Software_Devel/78   4/22/08 5:13p tdo
* PR41243: SCL Vertical Init Phase Adjustment For i2i 50->60 Rate
* Conversion
*
* Hydra_Software_Devel/77   3/19/08 7:43p hongtaoz
* PR39154, PR39202, PR39203: combined ANR and MAD in memory saving mode
* on 3548; implemented MAD specific change; updated SCL precision for
* 3548;
*
* Hydra_Software_Devel/76   3/18/08 7:07p pntruong
* PR34806: Added initial adc type for scart input.
*
* Hydra_Software_Devel/75   2/29/08 4:19p yuxiaz
* PR39158: Implement dithering in various BVN components for 3548.
*
* Hydra_Software_Devel/74   7/31/07 1:59p pntruong
* PR32487: Vertical scaler needs to be programmed to low-pass filter
* chroma for MAD source.  Added debug to print out name of selected
* coeffs set.
*
* Hydra_Software_Devel/73   7/26/07 12:03p pntruong
* PR33520: [SCL] Need to update the scale line buffer macro.
*
* Hydra_Software_Devel/72   5/4/07 5:18p pntruong
* PR28372: Need to trigger selection of new coeffs by added inputtype and
* index.
*
* Hydra_Software_Devel/71   5/1/07 4:43p syang
* PR 28541: 1) changed non-linear scl api; 2). carry sub-pxl accuracy
* through the pipe line by SrcHStep and SrcVStep
*
* Hydra_Software_Devel/70   4/6/07 2:39p pntruong
* PR28371: Lose resolution on high frequency for component 720p input.
*
* Hydra_Software_Devel/69   4/6/07 11:37a pntruong
* PR29194: Added the new coefficients.
*
* Hydra_Software_Devel/68   4/5/07 4:56p albertl
* PR29194:  Added BVDC_Window_SetCoefficientIndex and input and index
* matching support to scaler coefficient tables.
*
* Hydra_Software_Devel/67   4/2/07 11:30a syang
* PR 29243: clean up: take off the obselete old-vnet-code
*
* Hydra_Software_Devel/66   2/23/07 6:30p tdo
* PR 27970: Share LPB and FCH if source is the same to eliminate the out
* of resource error when number of LPB and FCH are limited
*
* Hydra_Software_Devel/65   2/16/07 3:38p syang
* PR 22563: make sure src cntrl percent change take effect
*
* Hydra_Software_Devel/64   1/17/07 7:52p hongtaoz
* PR23260: use #ifdef instead of #if ();
*
* Hydra_Software_Devel/63   1/17/07 7:38p hongtaoz
* PR23260: 3563 bringup simul windows;
*
* Hydra_Software_Devel/62   1/10/07 11:01a syang
* PR 22569:  add new vnet algorithm code to sub-modules such as scl, mad,
* dnr, mfd, vfd, cap, ...
*
* Hydra_Software_Devel/61   12/18/06 11:20p pntruong
* PR22577: Merged back to mainline.
*
* Hydra_Software_Devel/Refsw_Devel_3563/2   11/22/06 3:52p syang
* PR 26151: re-write shared resource manager. update letter box, mad, dnr
* shared-scl acquire/release code accordingly
*
* Hydra_Software_Devel/Refsw_Devel_3563/1   9/20/06 12:48p syang
* PR 20884: add SCL chroma coeff
*
* Hydra_Software_Devel/60   8/23/06 4:24p syang
* PR 22563: add new smooth non-linear scaling support for 3563; prepare
* to allow user to set non-linear scl central region percent; move un-
* known asp-ratio default handling to window's pixel-asp-ratio code
*
* Hydra_Software_Devel/59   8/18/06 4:25p albertl
* PR23117:  Added 7440 support.
*
* Hydra_Software_Devel/58   8/15/06 11:42a syang
* PR 23023: cleanup scale code: align width only when needed, choose
* vertical coeff basing on correct src height, avoid negative lLeft/lTop
* (that causes overflow)
*
* Hydra_Software_Devel/57   7/20/06 10:36a syang
* PR 22510: added 1 % down-scale support for 7401b0
*
* Hydra_Software_Devel/56   6/2/06 3:25p yuxiaz
* PR21892: 7401 B0 bring up: Use VDC_P_SUPPORT_BVB_IN_SIZE. Add
* BVB_IN_SIZE and PIC_OFFSET for scaler.
*
* Hydra_Software_Devel/55   5/26/06 3:42p pntruong
* PR20642: Refactored handling of hList.
*
* Hydra_Software_Devel/54   5/19/06 1:48p pntruong
* PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
* unnecessary global exported symbols.  Tighten const params.
*
* Hydra_Software_Devel/53   2/2/06 12:13p hongtaoz
* PR19082: bring up 7400;
*
* Hydra_Software_Devel/52   1/18/06 11:04a hongtaoz
* PR18233: added src/dst polarity as SCL optimizational condition;
*
* Hydra_Software_Devel/51   1/12/06 1:33p hongtaoz
* PR18233: added mosaic mode support;
*
* Hydra_Software_Devel/MosaicMode_Feature_PR18233/1   12/20/05 2:21p hongtaoz
* PR18233, PR18731: optimized rectangle computation and SCL settings; set
* bInitial flag when resetting MFD;
*
* Hydra_Software_Devel/50   10/3/05 4:35p pntruong
* PR17385: Added support for vertical/horizontal size matching in
* bvdc_scaler_priv.c for coeffs selection.  Fixed top panscan for block-
* averaging mode.
*
* Hydra_Software_Devel/49   6/30/05 2:53p hongtaoz
* PR16043, PR16015, PR15921, PR15888: added scaler reset RUL macro
* define;
*
* Hydra_Software_Devel/48   4/15/05 8:10p hongtaoz
* PR14691: reset video window's path when shutdown;
*
* Hydra_Software_Devel/47   4/14/05 10:25a syang
* PR 14692: re-organize picture's rectangles, fixed duplicated clips in
* diff modules, changed to do clip as soon as possible, and added
* capture clipping for C0
*
* Hydra_Software_Devel/46   3/17/05 6:40p pntruong
* PR14494: Add preliminary software support to 3560 A0.
*
* Hydra_Software_Devel/45   2/3/05 6:28p pntruong
* PR14024: Incorrect clipping occurs after an initial clip has been done.
*
* Hydra_Software_Devel/44   12/2/04 4:28p pntruong
* PR12854: Repeated display format changes causes loss of sync and
* flashing video.
*
* Hydra_Software_Devel/43   11/16/04 8:02p pntruong
* PR13076: Video jitter under heavy system load.  Added some
* optimizations, additional work needed.
*
***************************************************************************/
#ifndef BVDC_SCALER_PRIV_H__
#define BVDC_SCALER_PRIV_H__

#include "bvdc.h"
#include "bchp_common.h"
#include "bvdc_common_priv.h"
#include "bvdc_subrul_priv.h"
#include "bvdc_buffer_priv.h"
#include "bchp_scl_0.h"

#ifdef __cplusplus
extern "C" {
#endif

	/* Core revs to track increatmental changes! */
#define BVDC_P_SUPPORT_SCL_VER_1              (1) /* 7400 - Ax, Bx, Cx */
#define BVDC_P_SUPPORT_SCL_VER_2              (2) /* 7400 - Dx, Ex, others */
#define BVDC_P_SUPPORT_SCL_VER_3              (3) /* 3548 Ax */
#define BVDC_P_SUPPORT_SCL_VER_4              (4) /* 7420 */
#define BVDC_P_SUPPORT_SCL_VER_5              (5) /* 3548 B0 */
#define BVDC_P_SUPPORT_SCL_VER_6              (6) /* 7340, 7550: no HORIZ_*_04_05 from ver_4*/
#define BVDC_P_SUPPORT_SCL_VER_7              (7) /* 7422 */
#define BVDC_P_SUPPORT_SCL_VER_8              (8) /* 7231B0, 7344B0, 7346B0, 7425B0, Added more derring knobs */
#define BVDC_P_SUPPORT_SCL_VER_9              (9) /* 7445 */

	/***************************************************************************
	* Private register cracking macros
	***************************************************************************/
#define BVDC_P_SCL_GET_REG_IDX(reg) \
	((BCHP##_##reg - BCHP_SCL_0_REG_START) / sizeof(uint32_t))

	/* Get/Set reg data */
#define BVDC_P_SCL_GET_REG_DATA(reg) \
	(hScaler->aulRegs[BVDC_P_SCL_GET_REG_IDX(reg)])
#define BVDC_P_SCL_SET_REG_DATA(reg, data) \
	(BVDC_P_SCL_GET_REG_DATA(reg) = (uint32_t)(data))

	/* Get with index. */
#define BVDC_P_SCL_GET_REG_DATA_I(idx, reg) \
	(hScaler->aulRegs[BVDC_P_SCL_GET_REG_IDX(reg) + (idx)])

	/* Get field */
#define BVDC_P_SCL_GET_FIELD_NAME(reg, field) \
	(BVDC_P_GET_FIELD(BVDC_P_SCL_GET_REG_DATA(reg), reg, field))

	/* Compare field */
#define BVDC_P_SCL_COMPARE_FIELD_DATA(reg, field, data) \
	(BVDC_P_COMPARE_FIELD_DATA(BVDC_P_SCL_GET_REG_DATA(reg), reg, field, (data)))
#define BVDC_P_SCL_COMPARE_FIELD_NAME(reg, field, name) \
	(BVDC_P_COMPARE_FIELD_NAME(BVDC_P_SCL_GET_REG_DATA(reg), reg, field, name))

	/* This macro does a write into RUL (write, addr, data). 3 dwords. */
#define BVDC_P_SCL_WRITE_TO_RUL(reg, addr_ptr) \
	{ \
	*addr_ptr++ = BRDC_OP_IMM_TO_REG(); \
	*addr_ptr++ = BRDC_REGISTER(BCHP##_##reg + hScaler->ulRegOffset); \
	*addr_ptr++ = BVDC_P_SCL_GET_REG_DATA(reg); \
}

	/* This macro does a block write into RUL */
#define BVDC_P_SCL_BLOCK_WRITE_TO_RUL(from, to, pulCurrent) \
	do { \
	uint32_t ulBlockSize = \
	BVDC_P_REGS_ENTRIES(from, to);\
	*pulCurrent++ = BRDC_OP_IMMS_TO_REGS( ulBlockSize ); \
	*pulCurrent++ = BRDC_REGISTER(BCHP##_##from + hScaler->ulRegOffset); \
	BKNI_Memcpy((void*)pulCurrent, \
	(void*)&(hScaler->aulRegs[BVDC_P_SCL_GET_REG_IDX(from)]), \
	ulBlockSize * sizeof(uint32_t)); \
	pulCurrent += ulBlockSize; \
	} while(0)

	/* This macro does a scaler reset RUL. */
#define BVDC_P_SCL_RESET_RUL(hScaler) \
	{ \
	*pulCurrent++ = BRDC_OP_IMM_TO_REG(); \
	*pulCurrent++ = BRDC_REGISTER(hScaler->ulResetRegAddr); \
	*pulCurrent++ = hScaler->ulResetMask; \
	*pulCurrent++ = BRDC_OP_IMM_TO_REG(); \
	*pulCurrent++ = BRDC_REGISTER(hScaler->ulResetRegAddr); \
	*pulCurrent++ = 0; \
}

	/* number of registers in one block. */
#define BVDC_P_SCL_REGS_COUNT      \
	BVDC_P_REGS_ENTRIES(SCL_0_REG_START, SCL_0_REG_END)

	/* get register offset base on id. */
#ifdef BCHP_SCL_6_REG_START
#define BVDC_P_SCL_GET_REG_OFFSET(scaler_id) \
	((BVDC_P_ScalerId_eScl6==(scaler_id)) ? (BCHP_SCL_6_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl5==(scaler_id)) ? (BCHP_SCL_5_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl4==(scaler_id)) ? (BCHP_SCL_4_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl3==(scaler_id)) ? (BCHP_SCL_3_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl2==(scaler_id)) ? (BCHP_SCL_2_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl1==(scaler_id)) ? (BCHP_SCL_1_REG_START - BCHP_SCL_0_REG_START) \
	:(0))
#else
#ifdef BCHP_SCL_5_REG_START
#define BVDC_P_SCL_GET_REG_OFFSET(scaler_id) \
	((BVDC_P_ScalerId_eScl5==(scaler_id)) ? (BCHP_SCL_5_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl4==(scaler_id)) ? (BCHP_SCL_4_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl3==(scaler_id)) ? (BCHP_SCL_3_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl2==(scaler_id)) ? (BCHP_SCL_2_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl1==(scaler_id)) ? (BCHP_SCL_1_REG_START - BCHP_SCL_0_REG_START) \
	:(0))
#else
#ifdef BCHP_SCL_4_REG_START
#define BVDC_P_SCL_GET_REG_OFFSET(scaler_id) \
	((BVDC_P_ScalerId_eScl4==(scaler_id)) ? (BCHP_SCL_4_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl3==(scaler_id)) ? (BCHP_SCL_3_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl2==(scaler_id)) ? (BCHP_SCL_2_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl1==(scaler_id)) ? (BCHP_SCL_1_REG_START - BCHP_SCL_0_REG_START) \
	:(0))
#else
#ifdef BCHP_SCL_3_REG_START
#define BVDC_P_SCL_GET_REG_OFFSET(scaler_id) \
	((BVDC_P_ScalerId_eScl3==(scaler_id)) ? (BCHP_SCL_3_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl2==(scaler_id)) ? (BCHP_SCL_2_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl1==(scaler_id)) ? (BCHP_SCL_1_REG_START - BCHP_SCL_0_REG_START) \
	:(0))
#else
#ifdef BCHP_SCL_2_REG_START
#define BVDC_P_SCL_GET_REG_OFFSET(scaler_id) \
	((BVDC_P_ScalerId_eScl2==(scaler_id)) ? (BCHP_SCL_2_REG_START - BCHP_SCL_0_REG_START) \
	:(BVDC_P_ScalerId_eScl1==(scaler_id)) ? (BCHP_SCL_1_REG_START - BCHP_SCL_0_REG_START) \
	:(0))
#else
#ifdef BCHP_SCL_1_REG_START
#define BVDC_P_SCL_GET_REG_OFFSET(scaler_id) \
	((BVDC_P_ScalerId_eScl1==(scaler_id)) ? (BCHP_SCL_1_REG_START - BCHP_SCL_0_REG_START) \
	:(0))
#else
#define BVDC_P_SCL_GET_REG_OFFSET(scaler_id) (0)
#endif /* SCL_1 */
#endif /* SCL_2 */
#endif /* SCL_3 */
#endif /* SCL_4 */
#endif /* SCL_5 */
#endif /* SCL_6 */

	/* PR48032: Account for Kell Factor.  In the case of P -> I. */
#define BVDC_P_SCL_KELL_FACTOR                (70) /* 0.70 */
#define BVDC_P_APPLY_KELL_FACTOR(ulFirStep)   \
	(((ulFirStep) * 100) / (2 * BVDC_P_SCL_KELL_FACTOR))

	/* fixed point stuffs */
#if BVDC_P_SUPPORT_SCL_VER < BVDC_P_SUPPORT_SCL_VER_4
#define BVDC_P_SCL_H_NRM_I_BITS               (5)
#else
#define BVDC_P_SCL_H_NRM_I_BITS               (8)
#endif
#define BVDC_P_SCL_H_RATIO_I_BITS             (5)
#define BVDC_P_SCL_H_RATIO_F_BITS             (26)
#define BVDC_P_SCL_MAX_H_STEP                 (0x7fffffff)

#define BVDC_P_SCL_H_R_DLT_I_BITS             (1)
#define BVDC_P_SCL_H_R_DLT_F_BITS             (26)

#define BVDC_P_SCL_PHASE_ACCU_INIT_I_BITS     (5)
#define BVDC_P_SCL_PHASE_ACCU_INIT_F_BITS     (26)

#define BVDC_P_SCL_LUMA_INIT_PHASE_I_BITS     (11)
#define BVDC_P_SCL_LUMA_INIT_PHASE_F_BITS     (6)

#define BVDC_P_SCL_CHROMA_INIT_PHASE_I_BITS   (11)
#define BVDC_P_SCL_CHROMA_INIT_PHASE_F_BITS   (6)

#define BVDC_P_SCL_V_RATIO_I_BITS             (6)
#define BVDC_P_SCL_V_INIT_PHASE_I_BITS        (11)

#if BVDC_P_SUPPORT_SCL_VER > BVDC_P_SUPPORT_SCL_VER_2 /* 3548 */
#define BVDC_P_SCL_V_RATIO_F_BITS             (17)
#define BVDC_P_SCL_V_INIT_PHASE_F_BITS        (17)
#if BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_7 /* 7422 */
#define BVDC_P_SCL_V_RATIO_F_BITS_EXT         (26 - BVDC_P_SCL_V_RATIO_F_BITS)
#define BVDC_P_SCL_V_INIT_PHASE_F_BITS_EXT    (26 - BVDC_P_SCL_V_INIT_PHASE_F_BITS)
#endif
#else
#define BVDC_P_SCL_V_RATIO_F_BITS             (14)
#define BVDC_P_SCL_V_INIT_PHASE_F_BITS        (14)
#endif

#define BVDC_P_SCL_V_PAN_SCAN_I_BITS          (1)
#define BVDC_P_SCL_V_PAN_SCAN_F_BITS          (4)

#define BVDC_P_SCL_COEFFS_I_BITS              (1)
#define BVDC_P_SCL_COEFFS_F_BITS              (10)

#define BVDC_P_SCL_ZERO_F_BITS                (0)

#define BVDC_P_SCL_MAX_NRM_H_STEP  ((1 << (BVDC_P_SCL_H_NRM_I_BITS + BVDC_P_NRM_SRC_STEP_F_BITS))   - 1)
#define BVDC_P_SCL_MAX_NRM_H_R_DLT ((1 << (BVDC_P_SCL_H_R_DLT_I_BITS + BVDC_P_NRM_SRC_STEP_F_BITS))   - 1)

#define BVDC_P_SCL_H_RATIO_ONE                (1 << BVDC_P_SCL_H_RATIO_F_BITS)
#define BVDC_P_SCL_1ST_DECIMATION_THRESHOLD   (4 << BVDC_P_SCL_H_RATIO_F_BITS)
#define BVDC_P_SCL_2ND_DECIMATION_THRESHOLD   (8 << BVDC_P_SCL_H_RATIO_F_BITS)

#define BVDC_P_SCL_H_STEP_NRM_TO_SPEC(nrm_h_step)                            \
	((nrm_h_step) < BVDC_P_SCL_MAX_NRM_H_STEP)?                              \
	((nrm_h_step) << (BVDC_P_SCL_H_RATIO_F_BITS - BVDC_P_NRM_SRC_STEP_F_BITS)) : \
	(BVDC_P_SCL_MAX_H_STEP)

#define BVDC_P_SCL_V_RATIO_MAX   ((1 << (BVDC_P_SCL_V_RATIO_I_BITS+BVDC_P_SCL_V_RATIO_F_BITS)) - 1)
#define BVDC_P_SCL_V_RATIO_TRUNC(step)  ((step <= BVDC_P_SCL_V_RATIO_MAX)? step: BVDC_P_SCL_V_RATIO_MAX)
#define BVDC_P_SCL_V_RATIO_ONE BVDC_P_FLOAT_TO_FIXED(1.0, BVDC_P_SCL_V_RATIO_I_BITS, BVDC_P_SCL_V_RATIO_F_BITS)
#define BVDC_P_SCL_V_RATIO_KELL_RANGE BVDC_P_FLOAT_TO_FIXED(0.66667, BVDC_P_SCL_V_RATIO_I_BITS, BVDC_P_SCL_V_RATIO_F_BITS)

	/* Set horizontal scale down ratio */
#define BVDC_P_SCL_SET_HORZ_REGION02(region, end_pixel, step_inc) \
	{ \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_DEST_PIC_REGION_##region##_STEP_DELTA) &= ~( \
	BCHP_MASK(SCL_0_HORIZ_FIR_DEST_PIC_REGION_0_STEP_DELTA, SIZE)); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_DEST_PIC_REGION_##region##_STEP_DELTA) |=  ( \
	BCHP_FIELD_DATA(SCL_0_HORIZ_FIR_DEST_PIC_REGION_0_STEP_DELTA, SIZE, step_inc)); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DEST_PIC_REGION_##region##_END) &= ~( \
	BCHP_MASK(SCL_0_HORIZ_DEST_PIC_REGION_0_END, POSITION)); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DEST_PIC_REGION_##region##_END) |=  ( \
	BCHP_FIELD_DATA(SCL_0_HORIZ_DEST_PIC_REGION_0_END, POSITION, end_pixel)); \
}

#ifdef BCHP_SCL_0_HORIZ_DEST_PIC_REGION_N1_END
#define BVDC_P_SCL_SET_HORZ_REGION3N1(region, end_pixel)  \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DEST_PIC_REGION_##region##_END) &= ~( \
	BCHP_MASK(SCL_0_HORIZ_DEST_PIC_REGION_0_END, POSITION)); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DEST_PIC_REGION_##region##_END) |=  ( \
	BCHP_FIELD_DATA(SCL_0_HORIZ_DEST_PIC_REGION_0_END, POSITION, end_pixel));
#endif

#define BVDC_P_SCL_SET_HORZ_RGN1(end_pixel_1) \
	{ \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DEST_PIC_REGION_1_END) &= ~( \
	BCHP_MASK(SCL_0_HORIZ_DEST_PIC_REGION_1_END, POSITION)); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DEST_PIC_REGION_1_END) |=  ( \
	BCHP_FIELD_DATA(SCL_0_HORIZ_DEST_PIC_REGION_1_END, POSITION, end_pixel_1)); \
}

#if BVDC_P_SUPPORT_SCL_VER < BVDC_P_SUPPORT_SCL_VER_4
#define BVDC_P_SCL_SET_HORZ_RGN1_STEP(step_int_size, step_frac_size)\
	{ \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_INIT_STEP) &= ~( \
	BCHP_MASK(SCL_0_HORIZ_FIR_INIT_STEP, SIZE)); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_INIT_STEP) |=  ( \
	BCHP_FIELD_DATA(SCL_0_HORIZ_FIR_INIT_STEP, SIZE, \
	step_int_size & BCHP_SCL_0_HORIZ_FIR_INIT_STEP_SIZE_MASK)); \
}
#define BVDC_P_SCL_SET_HORZ_RATIO(step) \
	{ \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_INIT_STEP) &= ~( \
	BCHP_MASK(SCL_0_HORIZ_FIR_INIT_STEP, SIZE)); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_INIT_STEP) |=  ( \
	BCHP_FIELD_DATA(SCL_0_HORIZ_FIR_INIT_STEP, SIZE, step)); \
}
#else
#define BVDC_P_SCL_SET_HORZ_RGN1_STEP(step_int_size, step_frac_size)\
	{ \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_INIT_STEP_FRAC) &= ~( \
	BCHP_MASK(SCL_0_HORIZ_FIR_INIT_STEP_FRAC, FRAC_SIZE)); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_INIT_STEP_FRAC) |=  ( \
	BCHP_FIELD_DATA(SCL_0_HORIZ_FIR_INIT_STEP_FRAC, FRAC_SIZE, \
	((step_frac_size & ((1 << BVDC_P_SCL_H_RATIO_F_BITS)-1))))); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_INIT_STEP_INT) &= ~( \
	BCHP_MASK(SCL_0_HORIZ_FIR_INIT_STEP_INT, INT_SIZE)); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_INIT_STEP_INT) |=  ( \
	BCHP_FIELD_DATA(SCL_0_HORIZ_FIR_INIT_STEP_INT, INT_SIZE, \
	step_int_size & BCHP_SCL_0_HORIZ_FIR_INIT_STEP_INT_INT_SIZE_MASK)); \
}
#define BVDC_P_SCL_SET_HORZ_RATIO(step) \
	{ \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_INIT_STEP_FRAC) &= ~( \
	BCHP_MASK(SCL_0_HORIZ_FIR_INIT_STEP_FRAC, FRAC_SIZE)); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_INIT_STEP_FRAC) |=  ( \
	BCHP_FIELD_DATA(SCL_0_HORIZ_FIR_INIT_STEP_FRAC, FRAC_SIZE, \
	((step & ((1 << BVDC_P_SCL_H_RATIO_F_BITS)-1))))); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_INIT_STEP_INT) &= ~( \
	BCHP_MASK(SCL_0_HORIZ_FIR_INIT_STEP_INT, INT_SIZE)); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_FIR_INIT_STEP_INT) |=  ( \
	BCHP_FIELD_DATA(SCL_0_HORIZ_FIR_INIT_STEP_INT, INT_SIZE, \
	step >> BVDC_P_SCL_H_RATIO_F_BITS)); \
}
#endif

#define BVDC_P_SCL_SET_HORZ_STEP_MISC(end_pixel_1, step_init) \
	{ \
	BVDC_P_SCL_SET_HORZ_RATIO(step_init); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DEST_PIC_REGION_1_END) &= ~( \
	BCHP_MASK(SCL_0_HORIZ_DEST_PIC_REGION_1_END, POSITION)); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_HORIZ_DEST_PIC_REGION_1_END) |=  ( \
	BCHP_FIELD_DATA(SCL_0_HORIZ_DEST_PIC_REGION_1_END, POSITION, end_pixel_1)); \
}

/* Set vertical scale down ratio */
#if (BVDC_P_SUPPORT_SCL_VER <= BVDC_P_SUPPORT_SCL_VER_6)
#define BVDC_P_SCL_SET_VERT_RATIO(ratio) \
{ \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_FIR_SRC_PIC_STEP) &= ~( \
	BCHP_MASK(SCL_0_VERT_FIR_SRC_PIC_STEP, SIZE)); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_FIR_SRC_PIC_STEP) |=  ( \
	BCHP_FIELD_DATA(SCL_0_VERT_FIR_SRC_PIC_STEP, SIZE, ratio)); \
}
#else /* TODO: improve fixed point math to support higher precision SCL */
#define BVDC_P_SCL_SET_VERT_RATIO(ratio) \
	{ \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_FIR_INIT_PIC_STEP) &= ~( \
	BCHP_MASK(SCL_0_VERT_FIR_INIT_PIC_STEP, SIZE)); \
	BVDC_P_SCL_GET_REG_DATA(SCL_0_VERT_FIR_INIT_PIC_STEP) |=  ( \
	BCHP_FIELD_DATA(SCL_0_VERT_FIR_INIT_PIC_STEP, SIZE, (ratio << BVDC_P_SCL_V_RATIO_F_BITS_EXT))); \
}
#endif

	/* Initial phase values. */
#define BVDC_P_H_INIT_PHASE_0_POINT_5   BVDC_P_FLOAT_TO_FIXED(0.5, \
	BVDC_P_SCL_PHASE_ACCU_INIT_I_BITS, BVDC_P_SCL_PHASE_ACCU_INIT_F_BITS)
#define BVDC_P_H_INIT_PHASE_RATIO_ADJ(hsr) \
	(hsr >> (BVDC_P_SCL_H_RATIO_F_BITS - BVDC_P_SCL_PHASE_ACCU_INIT_F_BITS))
#define BVDC_P_V_INIT_PHASE_0_POINT_5   BVDC_P_FLOAT_TO_FIXED(0.5, \
	BVDC_P_SCL_V_INIT_PHASE_I_BITS, BVDC_P_SCL_V_INIT_PHASE_F_BITS)
#define BVDC_P_V_INIT_PHASE_1_POINT_0   BVDC_P_FLOAT_TO_FIXED(1.0, \
	BVDC_P_SCL_V_INIT_PHASE_I_BITS, BVDC_P_SCL_V_INIT_PHASE_F_BITS)
#define BVDC_P_V_INIT_PHASE_2_POINT_0   BVDC_P_FLOAT_TO_FIXED(2.0, \
	BVDC_P_SCL_V_INIT_PHASE_I_BITS, BVDC_P_SCL_V_INIT_PHASE_F_BITS)
#define BVDC_P_V_INIT_PHASE_0_POINT_25  BVDC_P_FLOAT_TO_FIXED(0.25, \
	BVDC_P_SCL_V_INIT_PHASE_I_BITS, BVDC_P_SCL_V_INIT_PHASE_F_BITS)

#define BVDC_P_V_INIT_PHASE_RATIO_ADJ(vsr) \
	(vsr >> (BVDC_P_SCL_V_RATIO_F_BITS - BVDC_P_SCL_V_INIT_PHASE_F_BITS))

/* Handy macros to build the table. */
#define BVDC_P_CT_UNUSED            (0)
#define BVDC_P_CT_P2I               (1)
#define BVDC_P_CT_2_TAP             (2)
#define BVDC_P_CT_4_TAP             (4)
#define BVDC_P_CT_6_TAP             (6)
#define BVDC_P_CT_8_TAP             (8)
#define BVDC_P_CT_12_TAP            (12)
#define BVDC_P_CT_16_TAP            (16)

/* horizontal scaling coeff table macroes */
#define BVDC_P_CT_H_RATIO_GENERIC(index, input, raster, operation, ratio, tap, table) \
{                                                                             \
	(index),                                                                  \
	BVDC_P_CtInput_e##input,                                                  \
	BVDC_P_CtRaster_e##raster,                                                \
	(operation),                                                              \
	BVDC_P_FLOAT_TO_FIXED(                                                    \
		(ratio), BVDC_P_SCL_H_RATIO_I_BITS, BVDC_P_SCL_H_RATIO_F_BITS),       \
	(tap),                                                                    \
	BVDC_P_CT_UNUSED,                                                         \
	BVDC_P_CT_UNUSED,                                                         \
	BVDC_P_CT_UNUSED,                                                         \
	(table),                                                                  \
	#table                                                                    \
}

#define BVDC_P_CT_H_SIZE_INPUT(index, input, raster, srcsize, dstsize, delta, tap, table) \
	{                                                                             \
	(index),                                                                  \
	BVDC_P_CtInput_e##input,                                                  \
	BVDC_P_CtRaster_e##raster,                                                \
	BVDC_P_CtLutOp_eEqual,                                                    \
	BVDC_P_CT_UNUSED,                                                         \
	(tap),                                                                    \
	(srcsize),                                                                \
	(dstsize),                                                                \
	(delta),                                                                  \
	(table),                                                                  \
	#table                                                                    \
}

#define BVDC_P_CT_H_SIZE(srcsize, dstsize, delta, tap, table) \
	BVDC_P_CT_H_SIZE_INPUT(0, Any, Any, srcsize, dstsize, delta, tap, table)

#define BVDC_P_CT_H_RATIO_EQ(ratio, tap, table) \
	BVDC_P_CT_H_RATIO_GENERIC(0, Any, Any, BVDC_P_CtLutOp_eEqual, ratio, tap, table)

#define BVDC_P_CT_H_RATIO_GT(ratio, tap, table) \
	BVDC_P_CT_H_RATIO_GENERIC(0, Any, Any, BVDC_P_CtLutOp_eGreaterThan, ratio, tap, table)

#define BVDC_P_CT_H_RATIO_LT(ratio, tap, table) \
	BVDC_P_CT_H_RATIO_GENERIC(0, Any, Any, BVDC_P_CtLutOp_eLessThan, ratio, tap, table)

#define BVDC_P_CT_H_RATIO_GT_EQ(ratio, tap, table) \
	BVDC_P_CT_H_RATIO_GENERIC(0, Any, Any, BVDC_P_CtLutOp_eGreaterThanEqual, ratio, tap, table)

#define BVDC_P_CT_H_RATIO_LT_EQ(ratio, tap, table) \
	BVDC_P_CT_H_RATIO_GENERIC(0, Any, Any, BVDC_P_CtLutOp_eLessThanEqual, ratio, tap, table)


#define BVDC_P_CT_H_RATIO_EQ_INPUT(index, input, raster, ratio, tap, table) \
	BVDC_P_CT_H_RATIO_GENERIC(index, input, raster, BVDC_P_CtLutOp_eEqual, ratio, tap, table)

#define BVDC_P_CT_H_RATIO_GT_INPUT(index, input, raster, ratio, tap, table) \
	BVDC_P_CT_H_RATIO_GENERIC(index, input, raster, BVDC_P_CtLutOp_eGreaterThan, ratio, tap, table)

#define BVDC_P_CT_H_RATIO_LT_INPUT(index, input, raster, ratio, tap, table) \
	BVDC_P_CT_H_RATIO_GENERIC(index, input, raster, BVDC_P_CtLutOp_eLessThan, ratio, tap, table)

#define BVDC_P_CT_H_RATIO_GT_EQ_INPUT(index, input, raster, ratio, tap, table) \
	BVDC_P_CT_H_RATIO_GENERIC(index, input, raster, BVDC_P_CtLutOp_eGreaterThanEqual, ratio, tap, table)

#define BVDC_P_CT_H_RATIO_LT_EQ_INPUT(index, input, raster, ratio, tap, table) \
	BVDC_P_CT_H_RATIO_GENERIC(index, input, raster, BVDC_P_CtLutOp_eLessThanEqual, ratio, tap, table)


	/* vertical scaling coeff table macroes */
#define BVDC_P_CT_V_RATIO_GENERIC(index, input, raster, operation, ratio, tap, table) \
{                                                                             \
	(index),                                                                  \
	BVDC_P_CtInput_e##input,                                                  \
	BVDC_P_CtRaster_e##raster,                                                \
	(operation),                                                              \
	BVDC_P_FLOAT_TO_FIXED((ratio), BVDC_P_SCL_V_RATIO_I_BITS, BVDC_P_SCL_V_RATIO_F_BITS), \
	(tap),                                                                    \
	BVDC_P_CT_UNUSED,                                                         \
	BVDC_P_CT_UNUSED,                                                         \
	BVDC_P_CT_UNUSED,                                                         \
	(table),                                                                  \
	#table                                                                    \
}

#define BVDC_P_CT_V_SIZE_INPUT(index, input, raster, srcsize, dstsize, delta, tap, table) \
{                                                                             \
	(index),                                                                  \
	BVDC_P_CtInput_e##input,                                                  \
	BVDC_P_CtRaster_e##raster,                                                \
	BVDC_P_CtLutOp_eEqual,                                                    \
	BVDC_P_CT_UNUSED,                                                         \
	(tap),                                                                    \
	(srcsize),                                                                \
	(dstsize),                                                                \
	(delta),                                                                  \
	(table),                                                                  \
	#table                                                                    \
}

#define BVDC_P_CT_V_SIZE(srcsize, dstsize, delta, tap, table) \
	BVDC_P_CT_V_SIZE_INPUT(0, Any, Any, srcsize, dstsize, delta, tap, table)

#define BVDC_P_CT_V_RATIO_EQ(ratio, tap, table) \
	BVDC_P_CT_V_RATIO_GENERIC(0, Any, Any, BVDC_P_CtLutOp_eEqual, ratio, tap, table)

#define BVDC_P_CT_V_RATIO_GT(ratio, tap, table) \
	BVDC_P_CT_V_RATIO_GENERIC(0, Any, Any, BVDC_P_CtLutOp_eGreaterThan, ratio, tap,  table)

#define BVDC_P_CT_V_RATIO_LT(ratio, tap, table) \
	BVDC_P_CT_V_RATIO_GENERIC(0, Any, Any, BVDC_P_CtLutOp_eLessThan, ratio, tap, table)

#define BVDC_P_CT_V_RATIO_GT_EQ(ratio, tap, table) \
	BVDC_P_CT_V_RATIO_GENERIC(0, Any, Any, BVDC_P_CtLutOp_eGreaterThanEqual, ratio, tap, table)

#define BVDC_P_CT_V_RATIO_LT_EQ(ratio, tap, table) \
	BVDC_P_CT_V_RATIO_GENERIC(0, Any, Any, BVDC_P_CtLutOp_eLessThanEqual, ratio, tap, table)


#define BVDC_P_CT_V_RATIO_EQ_INPUT(index, input, raster, ratio, tap, table) \
	BVDC_P_CT_V_RATIO_GENERIC(index, input, raster, BVDC_P_CtLutOp_eEqual, ratio, tap, table)

#define BVDC_P_CT_V_RATIO_GT_INPUT(index, input, raster, ratio, tap, table) \
	BVDC_P_CT_V_RATIO_GENERIC(index, input, raster, BVDC_P_CtLutOp_eGreaterThan, ratio, tap, table)

#define BVDC_P_CT_V_RATIO_LT_INPUT(index, input, raster, ratio, tap, table) \
	BVDC_P_CT_V_RATIO_GENERIC(index, input, raster, BVDC_P_CtLutOp_eLessThan, ratio, tap, table)

#define BVDC_P_CT_V_RATIO_GT_EQ_INPUT(index, input, raster, ratio, tap, table) \
	BVDC_P_CT_V_RATIO_GENERIC(index, input, raster, BVDC_P_CtLutOp_eGreaterThanEqual, ratio, tap, table)

#define BVDC_P_CT_V_RATIO_LT_EQ_INPUT(index, input, raster, ratio, tap, table) \
	BVDC_P_CT_V_RATIO_GENERIC(index, input, raster, BVDC_P_CtLutOp_eLessThanEqual, ratio, tap, table)

	/* User selectable */
#define BVDC_P_CT_USER_SELECTABLE(index, tap, table) \
	{(index), BVDC_P_CtInput_eAny, BVDC_P_CtRaster_eAny, BVDC_P_CtLutOp_eUserSelectable, \
	BVDC_P_CT_UNUSED, (tap), BVDC_P_CT_UNUSED, \
	BVDC_P_CT_UNUSED, BVDC_P_CT_UNUSED, (table), #table}

#define BVDC_P_CT_ALWAYS(table) \
	{0, BVDC_P_CtInput_eAny, BVDC_P_CtRaster_eAny, BVDC_P_CtLutOp_eAlways,  \
	BVDC_P_CT_UNUSED, BVDC_P_CT_UNUSED, BVDC_P_CT_UNUSED, \
	BVDC_P_CT_UNUSED, BVDC_P_CT_UNUSED, (table), #table}

#define BVDC_P_CT_END \
	{0, BVDC_P_CtInput_eAny, BVDC_P_CtRaster_eAny, BVDC_P_CtLutOp_eLast,    \
	BVDC_P_CT_UNUSED, BVDC_P_CT_UNUSED, BVDC_P_CT_UNUSED, \
	BVDC_P_CT_UNUSED, BVDC_P_CT_UNUSED, NULL, "NULL" }

	/* Miscellaneous constants */
#define BVDC_P_SCL_HORZ_REGIONS_COUNT         (9)
#define BVDC_P_SCL_VERT_IPC_COUNT             (20)

#define BVDC_P_SCL_VERT_FIR_TAP_COUNT         (4)
#define BVDC_P_SCL_VERT_FIR_PHASE_COUNT       (8)

#define BVDC_P_SCL_HORZ_FIR_TAP_COUNT         (8)
#define BVDC_P_SCL_HORZ_FIR_PHASE_COUNT       (8)

#define BVDC_P_SCL_FIR_TAP_COUNT_MAX          (8)
#define BVDC_P_SCL_FIR_PHASE_COUNT_MAX        (8)

#define BVDC_P_SCL_MAX_BLK_AVG                (31)

#if BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_4
#define BVDC_P_SCL_HD_SCL_LINE_BUFFER         (1920)
#else
#define BVDC_P_SCL_HD_SCL_LINE_BUFFER         (1280)
#endif
#define BVDC_P_SCL_SD_SCL_LINE_BUFFER         (720)

#define BVDC_P_SCL_HORZ_HWF_FACTOR            (2)

#define BVDC_P_SCL_HORZ_1_FIXED BVDC_P_FLOAT_TO_FIXED(1.000, \
	BVDC_P_SCL_LUMA_INIT_PHASE_I_BITS, BVDC_P_SCL_LUMA_INIT_PHASE_F_BITS)

#define BVDC_P_SCL_VERT_1_FIXED BVDC_P_FLOAT_TO_FIXED(1.000, \
	BVDC_P_SCL_V_INIT_PHASE_I_BITS, BVDC_P_SCL_V_INIT_PHASE_F_BITS)

#define BVDC_P_SCL_DEJAGGING_ON_THESHOLD BVDC_P_FLOAT_TO_FIXED(0.666, \
	BVDC_P_SCL_V_RATIO_I_BITS, BVDC_P_SCL_V_RATIO_F_BITS)

#define BVDC_P_SCL_8TAP_BLK_AVG_VERT_THRESHOLD BVDC_P_FLOAT_TO_FIXED(8, \
	BVDC_P_SCL_V_RATIO_I_BITS, BVDC_P_SCL_V_RATIO_F_BITS)

#define BVDC_P_SCL_6TAP_BLK_AVG_VERT_THRESHOLD BVDC_P_FLOAT_TO_FIXED(6, \
	BVDC_P_SCL_V_RATIO_I_BITS, BVDC_P_SCL_V_RATIO_F_BITS)

#define BVDC_P_SCL_4TAP_BLK_AVG_VERT_THRESHOLD BVDC_P_FLOAT_TO_FIXED(4, \
	BVDC_P_SCL_V_RATIO_I_BITS, BVDC_P_SCL_V_RATIO_F_BITS)

#define BVDC_P_SCL_2TAP_BLK_AVG_VERT_THRESHOLD BVDC_P_FLOAT_TO_FIXED(2, \
	BVDC_P_SCL_V_RATIO_I_BITS, BVDC_P_SCL_V_RATIO_F_BITS)

#define BVDC_P_SCL_FIR_COEFFS_MAX \
	(BVDC_P_SCL_FIR_TAP_COUNT_MAX * BVDC_P_SCL_FIR_PHASE_COUNT_MAX)

#define BVDC_P_SCL_LAST UINT32_C(-1)

#define BVDC_P_Scaler_MuxAddr(hScl)       (BCHP_VNET_F_SCL_0_SRC + (hScl)->eId * sizeof(uint32_t))
#define BVDC_P_Scaler_PostMuxValue(hScl)  \
(((hScl)->eId == BVDC_P_ScalerId_eScl4) ? BCHP_VNET_B_CAP_0_SRC_SOURCE_Scaler_4 : \
(BCHP_VNET_B_CAP_0_SRC_SOURCE_Scaler_0 + (hScl)->eId))
#define BVDC_P_Scaler_SetVnet_isr(hScl, ulSrcMuxValue, eVnetPatchMode) \
	BVDC_P_SubRul_SetVnet_isr(&((hScl)->SubRul), ulSrcMuxValue, eVnetPatchMode)
#define BVDC_P_Scaler_UnsetVnet_isr(hScl) \
	BVDC_P_SubRul_UnsetVnet_isr(&((hScl)->SubRul))

/***************************************************************************
* Scaler private data structures
***************************************************************************/
typedef enum
{
	BVDC_P_CtLutOp_eEqual = 0,
	BVDC_P_CtLutOp_eGreaterThan,
	BVDC_P_CtLutOp_eGreaterThanEqual,
	BVDC_P_CtLutOp_eLessThan,
	BVDC_P_CtLutOp_eLessThanEqual,
	BVDC_P_CtLutOp_eUserSelectable,
	BVDC_P_CtLutOp_eAlways,
	BVDC_P_CtLutOp_eLast
} BVDC_P_CtLut;

typedef enum
{
	BVDC_P_CtInput_eIfd = 0,
	BVDC_P_CtInput_eCvbs,
	BVDC_P_CtInput_eSvideo,
	BVDC_P_CtInput_eYPrPb,
	BVDC_P_CtInput_ePcIn,
	BVDC_P_CtInput_eScart,
	BVDC_P_CtInput_eHdDvi,
	BVDC_P_CtInput_eMpeg,
	BVDC_P_CtInput_eItu656,
	BVDC_P_CtInput_eAny,
	BVDC_P_CtInput_eUnknown
} BVDC_P_CtInput;

typedef enum
{
	BVDC_P_CtRaster_ePro = 0,
	BVDC_P_CtRaster_eInt,
	BVDC_P_CtRaster_eAny,
	BVDC_P_CtRaster_eUnknown
} BVDC_P_CtRaster;

/* Vert/Horz FIR coefficients. */
typedef struct
{
	uint32_t               ulCtIndex;
	BVDC_P_CtInput         eCtInputType;
	BVDC_P_CtRaster        eCtRaster;
	BVDC_P_CtLut           eCtLutOp;
	uint32_t               ulDownRatio; /* ratio */
	uint32_t               ulTapMode;   /* 2-tap, 4-tap, 8-tap */
	uint32_t               ulSrcSize;
	uint32_t               ulDstSize;
	uint32_t               ulDelta;
	const uint32_t        *pulCoeffs;
	const char            *pchName;
} BVDC_P_FirCoeffTbl;

typedef struct BVDC_P_ScalerContext
{
	BDBG_OBJECT(BVDC_SCL)

	/* flag initial state, requires reset; */
	bool                           bInitial;
	uint32_t                       ulResetRegAddr;
	uint32_t                       ulResetMask;
	uint32_t                       ulVnetResetAddr;
	uint32_t                       ulVnetResetMask;

	/* Scaler internal line buffers! */
	uint32_t                       ulVertLineDepth;
	uint32_t                       ulVertBlkAvgThreshold;

	/* Fir coeff tables (root) */
	const BVDC_P_FirCoeffTbl      *pHorzFirCoeffTbl;
	const BVDC_P_FirCoeffTbl      *pVertFirCoeffTbl;
	const BVDC_P_FirCoeffTbl      *pChromaHorzFirCoeffTbl;
	const BVDC_P_FirCoeffTbl      *pChromaVertFirCoeffTbl;

	/* vert coeff */
	const BVDC_P_FirCoeffTbl      *pPrevVertFirCoeff;  /* coeff set into shadow reg */
	const BVDC_P_FirCoeffTbl      *pVertFirCoeff;  /* regular use */
	const BVDC_P_FirCoeffTbl      *pPrevChromaVertFirCoeff;  /* regular use */
	const BVDC_P_FirCoeffTbl      *pChromaVertFirCoeff;  /* regular use */
	/* forced smooth coeff for frac init phase  */
	const BVDC_P_FirCoeffTbl      *pFracInitPhaseVertFirCoeff;
	const BVDC_P_FirCoeffTbl      *pChromaFracInitPhaseVertFirCoeff;

	/* Pre-computed vertical initial phase for all fields from->to. */
	uint32_t                       aaulInitPhase[BVDC_P_MAX_POLARITY][BVDC_P_MAX_POLARITY];
	uint32_t                       aulBlkAvgSize[BVDC_P_MAX_POLARITY][BVDC_P_MAX_POLARITY];
	uint32_t                       ulUpdateAll;
	uint32_t                       ulUpdateCoeff;
	int32_t                        lPrevHPanScan;
	int32_t                        lPrevVPanScan;

	/* store previous scaler info */
	BVDC_P_Rect                    stPrevSclCut;
	BVDC_P_Rect                    stPrevSclOut;
	BVDC_P_Rect                    stPrevSclIn;
	uint32_t                       ulPrevNonlinearSrcWidth;
	uint32_t                       ulPrevNonlinearSclOutWidth;
	BVDC_CoefficientIndex          stCtIndex;
	BVDC_Scaler_Settings           stSclSettings;
	BAVC_Polarity                  ePrevSrcPolarity;
	BAVC_Polarity                  ePrevDstPolarity;
	BFMT_Orientation               ePrevSrcOrientation;
	BFMT_Orientation               ePrevDispOrientation;
	BVDC_SplitScreenMode           ePrevDeJagging;
	BVDC_SplitScreenMode           ePrevDeRinging;
	BVDC_P_CtInput                 ePrevCtInputType;
	bool                           bHandleFldInv;

	/* Back pointer to window handle for  PEP to build rul */
	BVDC_Window_Handle             hWindow;
	BREG_Handle                    hReg;

	/* private fields. */
	BVDC_P_ScalerId                eId;
	uint32_t                       ulRegOffset; /* SCL_0, SCL_1, and etc. */
	uint32_t                       aulRegs[BVDC_P_SCL_REGS_COUNT];

	/* sub-struct to manage vnet and rul build opreations */
	BVDC_P_SubRulContext           SubRul;

	/* Dither */
	BVDC_P_DitherSetting           stHorzDither;
	BVDC_P_DitherSetting           stVertDither;

	/* 422 to 444 up sampler setting */
	BVDC_422To444UpSampler         stUpSampler;
	/* 444 to 422 down sampler setting */
	BVDC_444To422DnSampler         stDnSampler;

	uint32_t                       ulHorzTaps;
	uint32_t                       ulVertTaps;

	bool                           bDeJagging;
	bool                           bDeRinging;

	/* src width alignment: 1 for 444 format internally, 2 for 422 format internally */
	uint32_t                       ulSrcHrzAlign;

} BVDC_P_ScalerContext;


/***************************************************************************
* Scaler private functions
***************************************************************************/
BERR_Code BVDC_P_Scaler_Create
	( BVDC_P_Scaler_Handle            *phScaler,
	  BVDC_P_ScalerId                  eScalerId,
	  BVDC_P_Resource_Handle           hResource,
	  BREG_Handle                      hReg );

void BVDC_P_Scaler_Destroy
	( BVDC_P_Scaler_Handle             hScaler );

void BVDC_P_Scaler_Init
	( BVDC_P_Scaler_Handle             hScaler,
	  BVDC_Window_Handle               hWindow );

void BVDC_P_Scaler_BuildRul_isr
	( const BVDC_P_Scaler_Handle       hScaler,
	  BVDC_P_ListInfo                 *pList,
	  BVDC_P_State                     eVnetState,
	  BVDC_P_PicComRulInfo            *pPicComRulInfo );

const BVDC_P_FirCoeffTbl* BVDC_P_SelectFirCoeff_isr
	( const BVDC_P_FirCoeffTbl         *pFirstCoeffEntry,
	  uint32_t                          ulCtIndex,
	  BVDC_P_CtInput                    eCtInputType,
	  BAVC_Polarity                     ePolarity,
	  uint32_t                          ulDownRatio,
	  uint32_t                          ulTapMode,
	  uint32_t                          ulSrcSize,
	  uint32_t                          ulDstSize );

void BVDC_P_Scaler_SetInfo_isr
	( BVDC_P_Scaler_Handle             hScaler,
	  const BVDC_P_PictureNodePtr      pPicture );

void BVDC_P_Scaler_SetEnable_isr
	( BVDC_P_Scaler_Handle             hScaler,
	  bool                             bEnable );

void BVDC_P_GetFirCoeffs_isr
	( const BVDC_P_FirCoeffTbl       **ppHorzFirCoeffTbl,
	const BVDC_P_FirCoeffTbl       **ppVertFirCoeffTbl );

void BVDC_P_GetChromaFirCoeffs_isr
	( const BVDC_P_FirCoeffTbl       **ppHorzFirCoeffTbl,
	  const BVDC_P_FirCoeffTbl       **ppVertFirCoeffTbl );

void BVDC_P_Scaler_3ZoneNonLinear
	( BVDC_P_Scaler_Handle             hScaler,
	  const BVDC_P_PictureNodePtr      pPicture,
	  uint32_t                        *pFirHrzStep,
	  uint32_t                        *pHrzStep,
	  uint32_t                        *pFirHrzStepInit );

void BVDC_P_Scaler_5ZoneNonLinear
	( BVDC_P_Scaler_Handle             hScaler,
	  const BVDC_P_PictureNodePtr      pPicture,
	  uint32_t                        *pFirHrzStep,
	  uint32_t                        *pHrzStep,
	  uint32_t                        *pFirHrzStepInit);

bool BVDC_P_Scaler_Validate_VertDepth_isr
	(BVDC_Window_Handle                 hWindow,
	 const BVDC_P_Scaler_Handle         hScaler);
#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_SCALER_PRIV_H__ */
/* End of file. */
