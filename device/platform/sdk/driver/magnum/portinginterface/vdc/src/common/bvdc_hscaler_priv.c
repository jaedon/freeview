/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_hscaler_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/55 $
 * $brcm_Date: 9/5/12 2:05p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/common/bvdc_hscaler_priv.c $
 * 
 * Hydra_Software_Devel/55   9/5/12 2:05p vanessah
 * SW7425-3864: disable double-buffering in BVN
 * 
 * Hydra_Software_Devel/54   3/26/12 10:56a yuxiaz
 * SW7425-2753: Fixed BVN errors for 3d conversion.
 * 
 * Hydra_Software_Devel/53   12/1/11 4:32p yuxiaz
 * SW7425-968, SW7344-95: Merged into mainline.: added independent source
 * clipping of right window in 3D mode.
 * 
 * Hydra_Software_Devel/52   11/10/11 9:17p vanessah
 * SW7425-1115: add MCVP/MADR seamless transaction
 * 
 * Hydra_Software_Devel/51   9/20/11 11:01a yuxiaz
 * SW7425-1326: Bring up 3D support on 7425 B0
 * 
 * Hydra_Software_Devel/50   8/15/11 4:10p pntruong
 * SW7425-988: Implement de-ringing improvements for HSCL.
 *
 * Hydra_Software_Devel/49   12/8/10 5:44p tdo
 * SW7422-36: Correctly program MCVP 3D mode based on source and display
 * orientation
 *
 * Hydra_Software_Devel/48   11/15/10 5:35p tdo
 * SW7422-36: Properly program HSCL registers for 7422
 *
 * Hydra_Software_Devel/47   11/15/10 5:10p tdo
 * SW7422-36: Fix compiling error
 *
 * Hydra_Software_Devel/46   11/15/10 4:40p tdo
 * SW7422-36: Handle "left only" method for BVN internal data path
 *
 * Hydra_Software_Devel/45   11/5/10 3:08p vanessah
 * SW7422-52:  Two kinds of deinterlacers in the same chip
 *
 * Hydra_Software_Devel/44   8/26/10 5:27p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/43   6/18/10 4:20p rpan
 * SW7400-2763: New BVN resource management.
 *
 * Hydra_Software_Devel/42   5/18/10 1:22p pntruong
 * SW3548-2940: Corrected the macro for gfx/hscl testfeature1.
 *
 * Hydra_Software_Devel/41   4/28/10 8:08p tdo
 * SW7420-704: HDMI output has green tint in full screen mode.  Properly
 * program the coeff for HSCL
 *
 * Hydra_Software_Devel/40   4/19/10 10:16p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/39   4/7/10 11:31a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/38   4/6/10 3:28p tdo
 * SW7420-544: Change to 10-bit Scaler Coefficients for 7420 B0 and C0
 *
 * Hydra_Software_Devel/37   4/5/10 4:08p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/36   2/3/10 6:24p albertl
 * SW7125-180: Updated TESTFEATURE1 defines for 7125.
 *
 * Hydra_Software_Devel/35   1/29/10 6:08p syang
 * SW7550-230: write coeffs into RUL from shadow for HSCL_VER_4
 *
 * Hydra_Software_Devel/34   10/9/09 5:29p syang
 * SW7405-2516, SW3548-2439, SW7405-3046: rewrite HSCL setinfo_isr and
 * TryXferHrzSclToHscl_isr to do crop correctly; cleanup width alignment
 * for DCX_HSIZE_WORKAROUND
 *
 * Hydra_Software_Devel/33   9/3/09 10:31p pntruong
 * SW3548-2373: Rolled back PR56964.
 *
 * Hydra_Software_Devel/32   8/26/09 11:44a pntruong
 * SW7125-10: Fixed compiling errors on 7420.
 *
 * Hydra_Software_Devel/31   8/25/09 7:45p albertl
 * SW7125-10: Initial 7125 support.
 *
 * Hydra_Software_Devel/30   7/22/09 4:07p pntruong
 * PR56792: [config9] Repeated application of scaling (view modes) causes
 * loss of video/graphics on 1080i YPrPb and HDMI source.
 *
 * Hydra_Software_Devel/29   7/21/09 2:17p syang
 * PR 56964: take off the "connect drainer, en and then dis" step  in
 * vnet-drain code
 *
 * Hydra_Software_Devel/28   6/18/09 5:51p syang
 * PR 55877, PR 55871, PR 56127, PR 55812: update to support 7340 and 7550
 *
 * Hydra_Software_Devel/27   5/28/09 2:18p yuxiaz
 * PR55364: Disable dithering when any testfeature1 is enabled
 *
 * Hydra_Software_Devel/26   3/17/09 1:42a tdo
 * PR45785, PR45789: Fix compiling errors
 *
 * Hydra_Software_Devel/25   3/16/09 10:47p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/2   2/4/09 3:31p syang
 * PR 45785, PR 45789:  7420 HSCL uses the same coeff table as SCL_1/2,
 * but skip values in *_04_05 (0x00000000)
 *
 * Hydra_Software_Devel/7420_mcvp/1   1/23/09 11:30p syang
 * PR 45785, PR 45789: add PI support for new the new module  MCVP
 * (MCTF+MCDI)
 *
 * Hydra_Software_Devel/24   11/25/08 11:12a syang
 * PR 49562: add reset to output channel ping-pong buf of sub-modules and
 * loopbacks and free-channels
 *
 * Hydra_Software_Devel/23   11/20/08 9:31a syang
 * PR 48970: clean-up module disable and drian handling so that RUL dump
 * is easy to understand
 *
 * Hydra_Software_Devel/22   10/27/08 5:38p syang
 * PR 46477: back off SetEnable in in BuildRul_isr according to GetOps (it
 * causes window to disapper on 7400 D0)
 *
 * Hydra_Software_Devel/21   10/24/08 4:07p syang
 * PR 46477: in BuildRul_isr only disable in shadow regs  according to
 * GetOps (some SetEnable_isr does HW reset)
 *
 * Hydra_Software_Devel/20   10/24/08 11:05a syang
 * PR 46477: in BuildRul_isr reset enable bit according to GetOps
 *
 * Hydra_Software_Devel/19   10/23/08 9:40p tdo
 * PR46484:Initial VDC bringup for 7420
 *
 * Hydra_Software_Devel/18   10/22/08 1:59p hongtaoz
 * PR48143: only build necessary CAP/DNR/HSCL/SCL RULs when their
 * operational flags are enabled; this fixed SCL/HSCL enable error;
 *
 * Hydra_Software_Devel/17   10/20/08 1:48p pntruong
 * PR48032: Apply Kell factor for progressive to interlaced scaling case.
 *
 * Hydra_Software_Devel/16   10/13/08 10:14p tdo
 * PR46482: Use ENUM for number of horizontal taps read in
 * HW_CONFIGURURATION register  instead of hard coded number.
 *
 * Hydra_Software_Devel/15   9/24/08 6:20p pntruong
 * PR47113, PR45308: Allow flexibility in selecting builtins and user
 * defined filter coefficients.
 *
 * Hydra_Software_Devel/14   9/24/08 2:23p syang
 * PR 46487: Turn on enable_by_picture in HSCL for 3548 B0
 *
 * Hydra_Software_Devel/13   9/24/08 1:15p tdo
 * PR46482: Change FIR programming for SCL block for 3548 B0
 *
 * Hydra_Software_Devel/12   9/17/08 3:47p tdo
 * PR46482: B0:Change FIR programming for SCL block
 *
 * Hydra_Software_Devel/11   9/9/08 9:04p tdo
 * PR45802: Remove unnecessary macros and unused code
 *
 * Hydra_Software_Devel/10   8/8/08 4:09p yuxiaz
 * PR45484: More Dithering enable in VDC.
 *
 * Hydra_Software_Devel/9   8/8/08 3:29p yuxiaz
 * PR45484: Enable Dithering in VDC.
 *
 * Hydra_Software_Devel/8   8/4/08 2:31p yuxiaz
 * PR45367: Enable Dering in VDC.
 *
 * Hydra_Software_Devel/7   6/11/08 5:38p hongtaoz
 * PR43460: allows progressive source for HSCL;
 *
 * Hydra_Software_Devel/6   6/5/08 3:25p yuxiaz
 * PR39807: Fixed horizontal strech for HD source.
 *
 * Hydra_Software_Devel/5   6/4/08 4:32p yuxiaz
 * PR39807: VDC bring up on 3548: ENABLE_BY_PICTURE  is not working in
 * HSCL. Hard code to ALWAYS_ENABLE
 *
 * Hydra_Software_Devel/4   3/21/08 11:11a hongtaoz
 * PR39154, PR39202: added HSCL support;
 *
 * Hydra_Software_Devel/3   3/3/08 2:04p yuxiaz
 * PR39158, PR39155: Added dithering in HSCL. Move De-ring from MAD to
 * HSCL for 3548.
 *
 * Hydra_Software_Devel/2   2/28/08 10:11p pntruong
 * PR39154: Resolved clearcase merged.
 *
 * Hydra_Software_Devel/1   2/28/08 6:21p hongtaoz
 * PR39154: added HSCL block;
 *
 *
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bvdc.h"
#include "brdc.h"
#include "bvdc_hscaler_priv.h"
#include "bvdc_buffer_priv.h"
#include "bvdc_source_priv.h"
#include "bvdc_window_priv.h"
#include "bvdc_pep_priv.h"
#include "bvdc_vnet_priv.h"
#include "bvdc_mad_priv.h"

#if (BVDC_P_SUPPORT_DMISC)
#include "bchp_dmisc.h"
#endif

#if (BVDC_P_SUPPORT_HSCL) || (BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED)
#include "bchp_hscl_0.h"

#ifdef BCHP_HSCL_1_REG_START
#include "bchp_hscl_1.h"
#endif

BDBG_MODULE(BVDC_HSCL);
BDBG_OBJECT_ID(BVDC_HSL);

/* TODO: Temporary setting */
/* Dither settings for HSCL */
#define BVDC_P_DITHER_HSCL_LFSR_VALUE                    (0x420F8)
#define BVDC_P_DITHER_HSCL_HORZ_LFSR_CTRL_T0             (0x6)
#define BVDC_P_DITHER_HSCL_HORZ_LFSR_CTRL_T1             (0x4)
#define BVDC_P_DITHER_HSCL_HORZ_LFSR_CTRL_T2             (0x5)

/***************************************************************************
 * {private}
 *
 */
BERR_Code BVDC_P_Hscaler_Create
	( BVDC_P_Hscaler_Handle            *phHscaler,
	  BVDC_P_HscalerId                  eHscalerId,
	  BVDC_P_Resource_Handle            hResource,
	  BREG_Handle                       hReg
)
{
	BVDC_P_HscalerContext *pHscaler;

	BDBG_ENTER(BVDC_P_Hscaler_Create);

	BDBG_ASSERT(phHscaler);

	/* Use: to see messages */
	/* BDBG_SetModuleLevel("BVDC_HSCL", BDBG_eMsg); */

	/* The handle will be NULL if create fails. */
	*phHscaler = NULL;

	/* Alloc the context. */
	pHscaler = (BVDC_P_HscalerContext*)
		(BKNI_Malloc(sizeof(BVDC_P_HscalerContext)));
	if(!pHscaler)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pHscaler, 0x0, sizeof(BVDC_P_HscalerContext));
	BDBG_OBJECT_SET(pHscaler, BVDC_HSL);

	pHscaler->eId          = eHscalerId;
	pHscaler->hReg         = hReg;
	pHscaler->ulRegOffset  = 0;

#ifdef BCHP_HSCL_1_REG_START
	pHscaler->ulRegOffset = (eHscalerId - BVDC_P_HscalerId_eHscl0) *
		(BCHP_HSCL_1_REVISION_ID - BCHP_HSCL_0_REVISION_ID);
#endif

	/* Init to the default filter coeffficient tables. */
	BVDC_P_GetFirCoeffs_isr(&pHscaler->pHorzFirCoeffTbl, NULL);
	BVDC_P_GetChromaFirCoeffs_isr(&pHscaler->pChromaHorzFirCoeffTbl, NULL);

#if BVDC_P_SUPPORT_DMISC
	/* Hscaler reset address */
	pHscaler->ulResetRegAddr = BCHP_DMISC_SOFT_RESET;
#endif

	/* TODO: remove when name fixed in RDB */
#ifdef BCHP_DMISC_SOFT_RESET_HSCL_MASK
	pHscaler->ulResetMask    = BCHP_DMISC_SOFT_RESET_HSCL_MASK << (pHscaler->eId);
#endif

#ifdef BCHP_DMISC_SOFT_RESET_HSCL_0_MASK
	pHscaler->ulResetMask    = BCHP_DMISC_SOFT_RESET_HSCL_0_MASK << (pHscaler->eId);
#endif

	/* init the SubRul sub-module */
	BVDC_P_SubRul_Init(&(pHscaler->SubRul), BVDC_P_Hscaler_MuxAddr(pHscaler),
		BVDC_P_Hscaler_PostMuxValue(pHscaler), BVDC_P_DrainMode_eBack, 0, hResource);

	/* All done. now return the new fresh context to user. */
	*phHscaler = (BVDC_P_Hscaler_Handle)pHscaler;

	BDBG_LEAVE(BVDC_P_Hscaler_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Hscaler_Destroy
	( BVDC_P_Hscaler_Handle             hHscaler )
{
	BDBG_ENTER(BVDC_P_Hscaler_Destroy);
	BDBG_OBJECT_ASSERT(hHscaler, BVDC_HSL);

	BDBG_OBJECT_DESTROY(hHscaler, BVDC_HSL);
	/* Release context in system memory */
	BKNI_Free((void*)hHscaler);

	BDBG_LEAVE(BVDC_P_Hscaler_Destroy);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Hscaler_Init
	( BVDC_P_Hscaler_Handle            hHscaler )
{
	uint32_t  ulHorzLfsrCtrlT0, ulHorzLfsrCtrlT1, ulHorzLfsrCtrlT2, ulHorzLfsrValue;
	uint32_t  ulReg;
	uint32_t  ulTaps;

	BDBG_ENTER(BVDC_P_Hscaler_Init);
	BDBG_OBJECT_ASSERT(hHscaler, BVDC_HSL);

	hHscaler->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;

	/* Clear out shadow registers. */
	BKNI_Memset((void*)hHscaler->aulRegs, 0x0, sizeof(hHscaler->aulRegs));

	hHscaler->ulPrevSrcWidth = 0xffffffff;
	/* one value is enough to cause re-setinfo
	hHscaler->lPrevHsclCutLeft = 0xffffffff;
	hHscaler->ulPrevHsclCutWidth = 0xffffffff;
	hHscaler->ulPrevOutWidth = 0xffffffff;
	hHscaler->ulPrevSrcHeight = 0xffffffff;*/

#if (BVDC_P_SUPPORT_HSCL_VER >= BVDC_P_SUPPORT_HSCL_VER_2)
	ulReg = BREG_Read32(hHscaler->hReg, BCHP_HSCL_0_HW_CONFIGURATION + hHscaler->ulRegOffset);

	hHscaler->bDithering = BCHP_GET_FIELD_DATA(ulReg, HSCL_0_HW_CONFIGURATION, DITHERING) ? true : false;
	hHscaler->bDeringing = BCHP_GET_FIELD_DATA(ulReg, HSCL_0_HW_CONFIGURATION, DERINGING) ? true : false;
	ulTaps = BCHP_GET_FIELD_DATA(ulReg, HSCL_0_HW_CONFIGURATION, HORIZ_TAPS);
	switch(ulTaps)
	{
		case BCHP_HSCL_0_HW_CONFIGURATION_HORIZ_TAPS_HORIZ_8_TAPS:
			hHscaler->ulHorzTaps = BVDC_P_CT_8_TAP;
			break;
		case BCHP_HSCL_0_HW_CONFIGURATION_HORIZ_TAPS_HORIZ_12_TAPS:
			hHscaler->ulHorzTaps = BVDC_P_CT_12_TAP;
			break;
		case BCHP_HSCL_0_HW_CONFIGURATION_HORIZ_TAPS_HORIZ_16_TAPS:
			hHscaler->ulHorzTaps = BVDC_P_CT_16_TAP;
			break;
		default: break;
	}

#else
	hHscaler->ulHorzTaps = BVDC_P_CT_8_TAP;
	hHscaler->bDithering = true;
	hHscaler->bDeringing = true;

	BSTD_UNUSED(ulReg);
	BSTD_UNUSED(ulTaps);
#endif

	/* Dither */
	ulHorzLfsrCtrlT0 = BVDC_P_DITHER_HSCL_HORZ_LFSR_CTRL_T0;
	ulHorzLfsrCtrlT1 = BVDC_P_DITHER_HSCL_HORZ_LFSR_CTRL_T1;
	ulHorzLfsrCtrlT2 = BVDC_P_DITHER_HSCL_HORZ_LFSR_CTRL_T2;
	ulHorzLfsrValue  = BVDC_P_DITHER_HSCL_LFSR_VALUE;
	BVDC_P_Dither_Init(&hHscaler->stDither, ulHorzLfsrCtrlT0,
		ulHorzLfsrCtrlT1, ulHorzLfsrCtrlT2, ulHorzLfsrValue);

#ifdef BCHP_HSCL_0_DERINGING
	if(hHscaler->bDeringing)
	{
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_DERINGING) &=  ~(
#if (BVDC_P_SUPPORT_HSCL_VER >= BVDC_P_SUPPORT_HSCL_VER_6)
			BCHP_MASK(HSCL_0_DERINGING, HORIZ_CHROMA_PASS_FIRST_RING ) |
			BCHP_MASK(HSCL_0_DERINGING, HORIZ_LUMA_PASS_FIRST_RING ) |
#endif
			BCHP_MASK(HSCL_0_DERINGING, HORIZ_CHROMA_DERINGING ) |
			BCHP_MASK(HSCL_0_DERINGING, HORIZ_LUMA_DERINGING ));

		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_DERINGING) |=  (
#if (BVDC_P_SUPPORT_HSCL_VER >= BVDC_P_SUPPORT_HSCL_VER_6)
			BCHP_FIELD_ENUM(HSCL_0_DERINGING, HORIZ_CHROMA_PASS_FIRST_RING, ENABLE ) |
			BCHP_FIELD_ENUM(HSCL_0_DERINGING, HORIZ_LUMA_PASS_FIRST_RING, ENABLE ) |
#endif
			BCHP_FIELD_ENUM(HSCL_0_DERINGING, HORIZ_CHROMA_DERINGING, ON ) |
			BCHP_FIELD_ENUM(HSCL_0_DERINGING, HORIZ_LUMA_DERINGING,   ON ));
	}
#endif

#ifdef BCHP_HSCL_0_HORIZ_DITHER_CTRL
	if(hHscaler->bDithering)
	{
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_DITHER_LFSR_INIT) &= ~(
			BCHP_MASK(HSCL_0_HORIZ_DITHER_LFSR_INIT, SEQ) |
			BCHP_MASK(HSCL_0_HORIZ_DITHER_LFSR_INIT, VALUE));
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_DITHER_LFSR_INIT) |=  (
			BCHP_FIELD_DATA(HSCL_0_HORIZ_DITHER_LFSR_INIT, SEQ,   hHscaler->stDither.ulLfsrSeq  ) |
			BCHP_FIELD_DATA(HSCL_0_HORIZ_DITHER_LFSR_INIT, VALUE, hHscaler->stDither.ulLfsrValue));

		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_DITHER_CTRL) &=  ~(
			BCHP_MASK(HSCL_0_HORIZ_DITHER_CTRL, MODE      ) |
			BCHP_MASK(HSCL_0_HORIZ_DITHER_CTRL, OFFSET_CH2) |
			BCHP_MASK(HSCL_0_HORIZ_DITHER_CTRL, SCALE_CH2 ) |
			BCHP_MASK(HSCL_0_HORIZ_DITHER_CTRL, OFFSET_CH1) |
			BCHP_MASK(HSCL_0_HORIZ_DITHER_CTRL, SCALE_CH1 ) |
			BCHP_MASK(HSCL_0_HORIZ_DITHER_CTRL, OFFSET_CH0) |
			BCHP_MASK(HSCL_0_HORIZ_DITHER_CTRL, SCALE_CH0 ));
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_DITHER_CTRL) |=  (
			BCHP_FIELD_DATA(HSCL_0_HORIZ_DITHER_CTRL, MODE,       hHscaler->stDither.ulMode     ) |
			BCHP_FIELD_DATA(HSCL_0_HORIZ_DITHER_CTRL, OFFSET_CH2, hHscaler->stDither.ulCh2Offset) |
			BCHP_FIELD_DATA(HSCL_0_HORIZ_DITHER_CTRL, SCALE_CH2,  hHscaler->stDither.ulCh2Scale ) |
			BCHP_FIELD_DATA(HSCL_0_HORIZ_DITHER_CTRL, OFFSET_CH1, hHscaler->stDither.ulCh1Offset) |
			BCHP_FIELD_DATA(HSCL_0_HORIZ_DITHER_CTRL, SCALE_CH1,  hHscaler->stDither.ulCh1Scale ) |
			BCHP_FIELD_DATA(HSCL_0_HORIZ_DITHER_CTRL, OFFSET_CH0, hHscaler->stDither.ulCh0Offset) |
			BCHP_FIELD_DATA(HSCL_0_HORIZ_DITHER_CTRL, SCALE_CH0,  hHscaler->stDither.ulCh0Scale ));

		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_DITHER_LFSR_CTRL) &= ~(
			BCHP_MASK(HSCL_0_HORIZ_DITHER_LFSR_CTRL, T0) |
			BCHP_MASK(HSCL_0_HORIZ_DITHER_LFSR_CTRL, T1) |
			BCHP_MASK(HSCL_0_HORIZ_DITHER_LFSR_CTRL, T2));
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_DITHER_LFSR_CTRL) |=  (
			BCHP_FIELD_DATA(HSCL_0_HORIZ_DITHER_LFSR_CTRL, T0, hHscaler->stDither.ulLfsrCtrlT0) |
			BCHP_FIELD_DATA(HSCL_0_HORIZ_DITHER_LFSR_CTRL, T1, hHscaler->stDither.ulLfsrCtrlT1) |
			BCHP_FIELD_DATA(HSCL_0_HORIZ_DITHER_LFSR_CTRL, T2, hHscaler->stDither.ulLfsrCtrlT2));
	}
#endif

#if (BVDC_P_SUPPORT_BVN_444 != 0)
/* only TV chips use 444, settop box chips always use 422;
 * and future TV chips use 444 for HSCL1 too */
#if (BVDC_P_SUPPORT_SCL_VER == BVDC_P_SUPPORT_SCL_VER_5)
	if (BVDC_P_HscalerId_eHscl0 < hHscaler->eId)
		hHscaler->ulSrcHrzAlign  = 2;  /* even for 4:2:2 */
	else
		hHscaler->ulSrcHrzAlign  = 1;
#else
	hHscaler->ulSrcHrzAlign  = 1;
#endif
#else
	hHscaler->ulSrcHrzAlign  = 2;
#endif

	/* Initialize state. */
	hHscaler->bInitial = true;

	BDBG_LEAVE(BVDC_P_Hscaler_Init);
	return;
}


/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Hscaler_BuildRul_SetEnable_isr
	( BVDC_P_Hscaler_Handle            hHscaler,
	  BVDC_P_ListInfo                 *pList )
{
	BDBG_OBJECT_ASSERT(hHscaler, BVDC_HSL);

	/* Add scaler registers to RUL using block write */
	if(hHscaler->ulUpdateAll)
	{
		hHscaler->ulUpdateAll--;
		/* optimize scaler mute RUL */
		BVDC_P_HSCL_BLOCK_WRITE_TO_RUL(HSCL_0_HORIZ_CONTROL, HSCL_0_DEST_PIC_SIZE, pList->pulCurrent);
#if (BVDC_P_SUPPORT_HSCL_VER >= BVDC_P_SUPPORT_HSCL_VER_5)
		BVDC_P_HSCL_BLOCK_WRITE_TO_RUL(HSCL_0_SRC_PIC_HORIZ_PAN_SCAN, HSCL_0_HORIZ_DEST_PIC_REGION_3_END, pList->pulCurrent);
		BVDC_P_HSCL_WRITE_TO_RUL(HSCL_0_VIDEO_3D_MODE, pList->pulCurrent);
#else
		BVDC_P_HSCL_BLOCK_WRITE_TO_RUL(HSCL_0_SRC_PIC_HORIZ_PAN_SCAN, HSCL_0_HORIZ_DEST_PIC_REGION_2_END, pList->pulCurrent);
#endif

#ifdef BCHP_HSCL_0_DERINGING
		if(hHscaler->bDeringing)
		{
			BVDC_P_HSCL_WRITE_TO_RUL(HSCL_0_DERINGING, pList->pulCurrent);
		}
#endif

#if (BVDC_P_SUPPORT_HSCL_VER == BVDC_P_SUPPORT_HSCL_VER_1)
		BVDC_P_HSCL_BLOCK_WRITE_TO_RUL(HSCL_0_HORIZ_FIR_COEFF_PHASE0_00_01, HSCL_0_HORIZ_FIR_CHROMA_COEFF_PHASE7_06_07, pList->pulCurrent);
#elif (BVDC_P_SUPPORT_HSCL_VER == BVDC_P_SUPPORT_HSCL_VER_2) || (BVDC_P_SUPPORT_HSCL_VER == BVDC_P_SUPPORT_HSCL_VER_4)
		BVDC_P_HSCL_BLOCK_WRITE_TO_RUL(HSCL_0_HORIZ_FIR_LUMA_COEFF_PHASE0_00_01, HSCL_0_HORIZ_FIR_CHROMA_COEFF_PHASE7_02_03, pList->pulCurrent);
#elif (BVDC_P_SUPPORT_HSCL_VER == BVDC_P_SUPPORT_HSCL_VER_3) || (BVDC_P_SUPPORT_HSCL_VER >= BVDC_P_SUPPORT_HSCL_VER_5)
		BVDC_P_HSCL_BLOCK_WRITE_TO_RUL(HSCL_0_HORIZ_FIR_LUMA_COEFF_PHASE0_00_01, HSCL_0_HORIZ_FIR_CHROMA_COEFF_PHASE7_04_05, pList->pulCurrent);
#endif
		BVDC_P_HSCL_WRITE_TO_RUL(HSCL_0_TOP_CONTROL, pList->pulCurrent);
	}

#ifdef BCHP_HSCL_0_HORIZ_DITHER_CTRL
	if(hHscaler->bDithering)
	{
		BVDC_P_HSCL_BLOCK_WRITE_TO_RUL(HSCL_0_HORIZ_DITHER_LFSR_INIT, HSCL_0_HORIZ_DITHER_CTRL, pList->pulCurrent);
	}
#endif

	BVDC_P_HSCL_WRITE_TO_RUL(HSCL_0_ENABLE, pList->pulCurrent);
}


#if (BVDC_P_SUPPORT_HSCL)

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hscaler_AcquireConnect
 *
 * It is called by BVDC_Window_Validate after changing from diable mad to
 * enable mad, .
 */
BERR_Code BVDC_P_Hscaler_AcquireConnect
	( BVDC_P_Hscaler_Handle            hHscaler,
	  BVDC_P_WindowId                   eWinId )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Hscaler_AcquireConnect);

	hHscaler->eWinId = eWinId;

	/* init hscaler */
	BVDC_P_Hscaler_Init(hHscaler);

	BDBG_LEAVE(BVDC_P_Hscaler_AcquireConnect);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hscaler_ReleaseConnect_isr
 *
 * It is called after window decided that mad is no-longer used by HW in its
 * vnet mode (i.e. it is really shut down and teared off from vnet).
 */
BERR_Code BVDC_P_Hscaler_ReleaseConnect_isr
	( BVDC_P_Hscaler_Handle        *phHscaler )
{
	BERR_Code  eResult = BERR_SUCCESS;

	BDBG_ENTER(BVDC_P_Hscaler_ReleaseConnect_isr);

	/* handle validation */
	if (NULL != *phHscaler)
	{
		BDBG_OBJECT_ASSERT(*phHscaler, BVDC_HSL);
		BDBG_ASSERT(0 == (*phHscaler)->SubRul.ulWinsActFlags);

		BVDC_P_Resource_ReleaseHandle_isr(
			BVDC_P_SubRul_GetResourceHandle_isr(&(*phHscaler)->SubRul),
			BVDC_P_ResourceType_eHscl, (void *)(*phHscaler));

		(*phHscaler)->eWinId = BVDC_P_WindowId_eUnknown;

		/* this makes win to stop calling Hscl code */
		*phHscaler = NULL;
	}

	BDBG_LEAVE(BVDC_P_Hscaler_ReleaseConnect_isr);
	return BERR_TRACE(eResult);
}

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hscaler_BuildRul_DrainVnet_isr
 *
 * called by BVDC_P_Hscaler_BuildRul_isr after resetting to drain the module and
 * its pre-patch FreeCh or LpBack
 */
static void BVDC_P_Hscaler_BuildRul_DrainVnet_isr
	( BVDC_P_Hscaler_Handle          hHscaler,
	  BVDC_P_ListInfo               *pList )
{
	uint32_t  ulChnResetReg = 0, ulChnResetMask = 0;

	BDBG_OBJECT_ASSERT(hHscaler, BVDC_HSL);
	/* reset sub and connect the module to a drain */
	BVDC_P_SubRul_StartDrain_isr(&(hHscaler->SubRul), pList,
	    hHscaler->ulResetRegAddr, hHscaler->ulResetMask);

	/* drian the  module */
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HSCL_0_ENABLE, hHscaler->ulRegOffset,
		BCHP_FIELD_ENUM(HSCL_0_ENABLE, SCALER_ENABLE, ON));
	BVDC_P_SUBRUL_ONE_REG(pList, BCHP_HSCL_0_ENABLE, hHscaler->ulRegOffset,
		BCHP_FIELD_ENUM(HSCL_0_ENABLE, SCALER_ENABLE, OFF));

	/* disconnect the module from the drain; reset it again; drain the
	 * free-channel or loop-back used to to patch the vnet; and also
	 * reset the channel ping-pong buf of the module and the patcher. */
#ifdef BCHP_MMISC_VNET_B_CHANNEL_RESET
	ulChnResetReg  = BCHP_MMISC_VNET_B_CHANNEL_RESET;
	ulChnResetMask = BCHP_MMISC_VNET_B_CHANNEL_RESET_HSCL_0_RESET_MASK <<
		(hHscaler->eId - BVDC_P_HscalerId_eHscl0);
#endif
	BVDC_P_SubRul_FinishDrain_isr(&(hHscaler->SubRul), pList,
		hHscaler->ulResetRegAddr, hHscaler->ulResetMask,
		ulChnResetReg, ulChnResetMask);
}

/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Hscaler_BuildRul_isr
	( const BVDC_P_Hscaler_Handle      hHscaler,
	  BVDC_P_ListInfo                 *pList,
	  BVDC_P_State                     eVnetState,
	  BVDC_P_PicComRulInfo            *pPicComRulInfo )
{
	uint32_t  ulRulOpsFlags;

	BDBG_ENTER(BVDC_P_Hscaler_BuildRul_isr);
	BDBG_OBJECT_ASSERT(hHscaler, BVDC_HSL);

	/* currently this is only for vnet building / tearing-off*/

	ulRulOpsFlags = BVDC_P_SubRul_GetOps_isr(
		&(hHscaler->SubRul), pPicComRulInfo->eWin, eVnetState, pList->bLastExecuted);

	if ((0 == ulRulOpsFlags) ||
		(ulRulOpsFlags & BVDC_P_RulOp_eReleaseHandle))
		return;
	else if (ulRulOpsFlags & BVDC_P_RulOp_eDisable)
	{
		BVDC_P_SubRul_DropOffVnet_isr(&(hHscaler->SubRul), pList);
		BVDC_P_Hscaler_SetEnable_isr(hHscaler, false);
		BVDC_P_HSCL_WRITE_TO_RUL(HSCL_0_ENABLE, pList->pulCurrent);
	}

	/* If rul failed to execute last time we'd re reprogrammed possible
	 * missing registers. */
	if((!pList->bLastExecuted)|| (hHscaler->bInitial))
	{
		hHscaler->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;
	}

	/* reset */
	if(hHscaler->bInitial)
	{
		/* BVDC_P_BUILD_RESET(pList->pulCurrent,
			hHscaler->ulResetRegAddr, hHscaler->ulResetMask); */
		hHscaler->bInitial = false;
	}

	if (ulRulOpsFlags & BVDC_P_RulOp_eEnable)
	{
		BVDC_P_Hscaler_BuildRul_SetEnable_isr(hHscaler, pList);

		/* join in vnet after enable. note: its src mux is initialed as disabled */
		if (ulRulOpsFlags & BVDC_P_RulOp_eVnetInit)
		{
			BVDC_P_SubRul_JoinInVnet_isr(&(hHscaler->SubRul), pList);
		}
	}

	if (ulRulOpsFlags & BVDC_P_RulOp_eDrainVnet)
	{
		BVDC_P_Hscaler_BuildRul_DrainVnet_isr(hHscaler, pList);
	}

	BDBG_LEAVE(BVDC_P_Hscaler_BuildRul_isr);
	return;
}

#endif


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Hscaler_SetFirCoeff
	( BVDC_P_Hscaler_Handle            hHscaler,
	  const uint32_t                  *pulHorzFirCoeff )
{
	int i;

	BDBG_OBJECT_ASSERT(hHscaler, BVDC_HSL);
	/* write horiz entries into registers */
	for(i = 0; (pulHorzFirCoeff) && (*pulHorzFirCoeff != BVDC_P_HSCL_LAST); i++)
	{
#if (BVDC_P_SUPPORT_HSCL_VER == BVDC_P_SUPPORT_HSCL_VER_1)
		hHscaler->aulRegs[BVDC_P_HSCL_GET_REG_IDX(HSCL_0_HORIZ_FIR_COEFF_PHASE0_00_01) + i] =
			*pulHorzFirCoeff;
#else
		hHscaler->aulRegs[BVDC_P_HSCL_GET_REG_IDX(HSCL_0_HORIZ_FIR_LUMA_COEFF_PHASE0_00_01) + i] =
			*pulHorzFirCoeff;
#endif
		pulHorzFirCoeff++;
	}
}


/***************************************************************************
 * {private}
 *
 */
static void BVDC_P_Hscaler_SetChromaFirCoeff
	( BVDC_P_Hscaler_Handle            hHscaler,
	  const uint32_t                  *pulHorzFirCoeff )
{
	int i;

	BDBG_OBJECT_ASSERT(hHscaler, BVDC_HSL);
	/* write 32 hor entries into registers */
	for(i = 0; (pulHorzFirCoeff) && (*pulHorzFirCoeff != BVDC_P_HSCL_LAST); i++)
	{
		hHscaler->aulRegs[BVDC_P_HSCL_GET_REG_IDX(HSCL_0_HORIZ_FIR_CHROMA_COEFF_PHASE0_00_01) + i] =
			*pulHorzFirCoeff;
		pulHorzFirCoeff++;
	}
}


/***************************************************************************
 * {private}
 *
 * The following notes illustrate the capabilities of the hscaler.  It
 * mainly shows different modes scaler operates to achieve the desire
 * scaling ratio with the best quality.
 *
 * Sx = in/out.  For example src=1920 dst=720 then Sx = 1920/720 or 2.66.
 * Hence this is what Sx means:
 *   Sx >  1 Scale down
 *   Sx <  1 Scale up
 *   Sx == 1 Unity.
 *
 * Actuall Sx tells how much we're scaling down.  For example
 * Sx = 4 means we're scaling down 4 times.
 *
 * There is no hard-wired filter in HSCL, so Sx is the value that goes into
 * horizontal FIR ratio register.
 *
 * Sx must be [32.0, 0.125].
 *
 *
 * [[ Conclusion ]]
 *  With the above information the theoretical scaling capacities are:
 *
 *  Sx = 32:1 to 1:32
 */
void BVDC_P_Hscaler_SetInfo_isr
	( BVDC_P_Hscaler_Handle            hHscaler,
	  BVDC_Window_Handle               hWindow,
	  const BVDC_P_PictureNodePtr      pPicture )
{
	uint32_t ulSrcHSize;               /* really scaled src width in pixel unit */
	uint32_t ulDstHSize;               /* Dst width in pixel unit */
	uint32_t ulDstVSize;               /* Dst height, in row unit */
	uint32_t ulAlgnSrcHSize;           /* src width into the 1st one of half band or FIR, pixel unit */
	uint32_t ulBvbInHSize;             /* input bvb width in pixel unit */
	uint32_t ulPicOffsetLeft = 0;      /* horizontal Pan/Scan part cut by PIC_OFFSET, pixel unit */
#if (BVDC_P_SUPPORT_HSCL_VER >= BVDC_P_SUPPORT_HSCL_VER_6)
	uint32_t ulPicOffsetLeft_R = 0;    /* horizontal Pan/Scan part cut by PIC_OFFSET_R, pixel unit */
#endif
	uint32_t ulPanScanLeft;            /* horizontal Pan/Scan vector in S11.6 format */
	const BVDC_P_FirCoeffTbl *pHorzFirCoeff;
	uint32_t ulNrmHrzStep;              /* Total horizontal src step per dest pixel, U11.21 */
	uint32_t ulFirHrzStep = 0;          /* FIR hrz src step per dest pixel, HW reg fmt, for coeff select */
	int32_t  lHrzPhsAccInit = 0;
	uint32_t ulMaxX;
	BVDC_P_Rect  *pHsclIn, *pHsclOut;

	BDBG_ENTER(BVDC_P_Hscaler_SetInfo_isr);
	BDBG_OBJECT_ASSERT(hHscaler, BVDC_HSL);
	BDBG_OBJECT_ASSERT(hWindow, BVDC_WIN);
	BDBG_OBJECT_ASSERT(hWindow->stCurInfo.hSource, BVDC_SRC);
	pHsclIn = pPicture->pHsclIn;
	pHsclOut = pPicture->pHsclOut;
	

	ulDstVSize = pHsclOut->ulHeight >> (BAVC_Polarity_eFrame!=pPicture->eSrcPolarity);
	/* any following info changed -> re-calculate SCL settings */
	if((hHscaler->ulPrevSrcWidth != pHsclIn->ulWidth) ||
	   (hHscaler->lPrevHsclCutLeft != pPicture->lHsclCutLeft) ||
	   (hHscaler->lPrevHsclCutLeft_R != pPicture->lHsclCutLeft_R) ||
	   (hHscaler->ulPrevHsclCutWidth != pPicture->ulHsclCutWidth) ||
	   (hHscaler->ulPrevOutWidth != pHsclOut->ulWidth) ||
	   (hHscaler->ulPrevSrcHeight != ulDstVSize) ||  /* no vrt scl */
#if (BVDC_P_SUPPORT_3D_VIDEO)
		(pPicture->eSrcOrientation != hHscaler->ePrevSrcOrientation)    ||
		(pPicture->eDispOrientation  != hHscaler->ePrevDispOrientation)   ||
#endif
	   (hWindow->stCurInfo.stCtIndex.ulHsclHorzLuma != hHscaler->ulPrevCtIndexLuma) ||
	   (hWindow->stCurInfo.stCtIndex.ulHsclHorzChroma != hHscaler->ulPrevCtIndexChroma) ||
	   (hWindow->stCurInfo.hSource->stCurInfo.eCtInputType != hHscaler->ePrevCtInputType) ||
	   !BVDC_P_HSCL_COMPARE_FIELD_DATA(HSCL_0_ENABLE, SCALER_ENABLE, 1))
	{
		/* for next "dirty" check */
		hHscaler->ulPrevSrcWidth = pHsclIn->ulWidth;
		hHscaler->lPrevHsclCutLeft = pPicture->lHsclCutLeft;
		hHscaler->lPrevHsclCutLeft_R = pPicture->lHsclCutLeft_R;
		hHscaler->ulPrevHsclCutWidth = pPicture->ulHsclCutWidth;
		hHscaler->ulPrevOutWidth = pHsclOut->ulWidth;
		hHscaler->ulPrevSrcHeight = pHsclIn->ulHeight >> (BAVC_Polarity_eFrame!=pPicture->eSrcPolarity);
		hHscaler->ulPrevCtIndexLuma = hWindow->stCurInfo.stCtIndex.ulHsclHorzLuma;
		hHscaler->ulPrevCtIndexChroma = hWindow->stCurInfo.stCtIndex.ulHsclHorzChroma;
		hHscaler->ePrevCtInputType = hWindow->stCurInfo.hSource->stCurInfo.eCtInputType;

		hHscaler->ePrevSrcOrientation  = pPicture->eSrcOrientation;
		hHscaler->ePrevDispOrientation = pPicture->eDispOrientation;

		hHscaler->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;

		/* -----------------------------------------------------------------------
		 * 1). Init some regitsers first, they might be modified later basing on
		 * specific conditions
		 */

		/* scaler panscan will be combined with init phase */
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_SRC_PIC_HORIZ_PAN_SCAN) &= ~(
			BCHP_MASK(HSCL_0_SRC_PIC_HORIZ_PAN_SCAN, OFFSET));

		/* Always re-enable after set info. */
		/* Horizontal scaler settings (and top control)!  Choose scaling order,
		 * and how much to decimate data. */
#if (BVDC_P_SUPPORT_HSCL_VER >= BVDC_P_SUPPORT_HSCL_VER_3)
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_TOP_CONTROL) =  (
			BCHP_FIELD_ENUM(HSCL_0_TOP_CONTROL, UPDATE_SEL,   ALWAYS_UPDATE) |
			BCHP_FIELD_ENUM(HSCL_0_TOP_CONTROL, ENABLE_CTRL,  ENABLE_BY_PICTURE));
#else
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_TOP_CONTROL) =  (
			BCHP_FIELD_ENUM(HSCL_0_TOP_CONTROL, UPDATE_SEL,   UPDATE_BY_PICTURE) |
			BCHP_FIELD_ENUM(HSCL_0_TOP_CONTROL, ENABLE_CTRL,  ALWAYS_ENABLE));
#endif

		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_ENABLE) =
			BCHP_FIELD_ENUM(HSCL_0_ENABLE, SCALER_ENABLE, ON);

		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_CONTROL) &= ~(
			BCHP_MASK(HSCL_0_HORIZ_CONTROL, FIR_ENABLE          ) |
			BCHP_MASK(HSCL_0_HORIZ_CONTROL, MASK_HSCL_LONG_LINE ) |
			BCHP_MASK(HSCL_0_HORIZ_CONTROL, MASK_HSCL_SHORT_LINE) |
			BCHP_MASK(HSCL_0_HORIZ_CONTROL, STALL_DRAIN_ENABLE  ));

		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_CONTROL) |=  (
			(((1 << BVDC_P_NRM_SRC_STEP_F_BITS) == pPicture->ulHsclNrmHrzSrcStep)?
			 BCHP_FIELD_ENUM(HSCL_0_HORIZ_CONTROL, FIR_ENABLE,          OFF) :
			 BCHP_FIELD_ENUM(HSCL_0_HORIZ_CONTROL, FIR_ENABLE,          ON ))|
			BCHP_FIELD_ENUM(HSCL_0_HORIZ_CONTROL, MASK_HSCL_LONG_LINE,  ON ) |
			BCHP_FIELD_ENUM(HSCL_0_HORIZ_CONTROL, MASK_HSCL_SHORT_LINE, ON ) |
			BCHP_FIELD_ENUM(HSCL_0_HORIZ_CONTROL, STALL_DRAIN_ENABLE,   OFF));

#if (BVDC_P_SUPPORT_HSCL_VER >= BVDC_P_SUPPORT_HSCL_VER_5)
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_VIDEO_3D_MODE) =
			BCHP_FIELD_DATA(HSCL_0_VIDEO_3D_MODE, BVB_VIDEO,
			BVDC_P_VNET_USED_HSCL_AT_WRITER(pPicture->stVnetMode) ?
			pPicture->eSrcOrientation : pPicture->eDispOrientation);
#endif

		/* -----------------------------------------------------------------------
		 * 2). Need to calculate the horizontal scaling factors before src width
		 * alignment and init phase can be decided
		 */

		/* output size */
		ulDstHSize = pHsclOut->ulWidth;

		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_DEST_PIC_SIZE) &= ~(
			BCHP_MASK(HSCL_0_DEST_PIC_SIZE, HSIZE) |
			BCHP_MASK(HSCL_0_DEST_PIC_SIZE, VSIZE));
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_DEST_PIC_SIZE) |=  (
			BCHP_FIELD_DATA(HSCL_0_DEST_PIC_SIZE, HSIZE, ulDstHSize) |
			BCHP_FIELD_DATA(HSCL_0_DEST_PIC_SIZE, VSIZE, ulDstVSize));

		/* the src size really scaled and output */
		ulSrcHSize = pPicture->ulHsclCutWidth;

		/* pan scan:  left format S11.6, top format S11.14 */
		ulPanScanLeft = pPicture->lHsclCutLeft;

		/* separate the amount cut by HSCL_0_PIC_OFFSET and FIR_LUMA_SRC_PIC_OFFSET */
		ulPicOffsetLeft = (ulPanScanLeft >> 6) & ~(hHscaler->ulSrcHrzAlign - 1);
		ulPanScanLeft -= (ulPicOffsetLeft << 6);

#if (BVDC_P_SUPPORT_HSCL_VER >= BVDC_P_SUPPORT_HSCL_VER_6)
		ulPicOffsetLeft_R = (pPicture->lHsclCutLeft_R >> 6) & ~(hHscaler->ulSrcHrzAlign - 1);
#endif

		/* the src size that get into the first scaler sub-modules (e.g. HW half-band
		 * filter if it is scaled down a lot): it includes the FIR_LUMA_SRC_PIC_OFFSET,
		 * but not the HSCL_0_PIC_OFFSET, it has to be rounded-up for alignment */
		ulMaxX = ulPanScanLeft + (ulSrcHSize << 6);
		ulAlgnSrcHSize = ((ulMaxX + ((1<< 6) - 1)) >>  6);
		ulAlgnSrcHSize = BVDC_P_ALIGN_UP(ulAlgnSrcHSize, hHscaler->ulSrcHrzAlign);

		/* Init the input/output horizontal size of FIRs */
		/*ulFirSrcHSize = ulSrcHSize;*/

		/* HSCL only do linear scaling */
		/*if(0 == pPicture->ulNonlinearSclOutWidth)*/
		{
			/* Horizantal step HW reg uses U5.17 in older arch, U5.26 after smooth non-linear is
			 * suported. Since CPU uses 32 bits int, calc step with 26 bits frac needs special
			 * handling (see the delta calcu in the case of nonlinear scaling). It is the step
			 * delta and internal step accum reg, not the initial step value, that really need 26
			 * frac bits, therefore we use 21 bits for trade off */
			ulNrmHrzStep = pPicture->ulHsclNrmHrzSrcStep;    /* U11.21 */
			ulFirHrzStep = /*ulHrzStep =*/ BVDC_P_SCL_H_STEP_NRM_TO_SPEC(ulNrmHrzStep); /* U4.17, U5.17, U5.26 */

#if (BVDC_P_SUPPORT_SCL_VER >= BVDC_P_SUPPORT_SCL_VER_4)
			if (((1<<BVDC_P_NRM_SRC_STEP_F_BITS) == ulNrmHrzStep) &&
				(0 == ulPanScanLeft) && (ulSrcHSize == ulDstHSize))
			{
				/* unity scale and no phase shift, turn off FIR for accuracy */
				BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_CONTROL) &= ~(
					BCHP_MASK(HSCL_0_HORIZ_CONTROL, FIR_ENABLE));
				BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_CONTROL) |=  (
					BCHP_FIELD_ENUM(HSCL_0_HORIZ_CONTROL, FIR_ENABLE,  OFF));
			}
#endif
			/*ulFirHrzStepInit = ulFirHrzStep;*/

			/* set step size and region_0 end */
			BVDC_P_HSCL_SET_HORZ_RATIO(ulFirHrzStep);
			BVDC_P_HSCL_SET_HORZ_REGION02(0, ulDstHSize, 0)
		}

		/* -----------------------------------------------------------------------
		 * 3). Now we can set src size, offset and bvb size
		 */
		ulBvbInHSize = pHsclIn->ulWidth;

		/* in older chips, align ulBvbInHSize up if ulAlgnSrcHSize has been aligned
		 * up due to half-band.
		 * note: align ulBvbInHSize up might cause "short line" error, that is OK
		 * and scl input module would patch. If we don't align up, SCL might hang */
		if (1 != hHscaler->ulSrcHrzAlign)
			ulBvbInHSize  = BVDC_P_MAX(ulBvbInHSize, ulAlgnSrcHSize + ulPicOffsetLeft);
		else
			ulAlgnSrcHSize = BVDC_P_MIN(ulAlgnSrcHSize, ulBvbInHSize - ulPicOffsetLeft);

		/* without this we might see flash when we move up with dst cut if MAD is disabled? */
		/*ulBvbInVSize  = BVDC_P_MAX(ulBvbInVSize, ulAlgnSrcVSize + ulPicOffsetTop);*/

		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_PIC_OFFSET) &= ~(
			BCHP_MASK(HSCL_0_PIC_OFFSET, HSIZE) |
			BCHP_MASK(HSCL_0_PIC_OFFSET, VSIZE));
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_PIC_OFFSET) |=  (
			BCHP_FIELD_DATA(HSCL_0_PIC_OFFSET, HSIZE, ulPicOffsetLeft) |
			BCHP_FIELD_DATA(HSCL_0_PIC_OFFSET, VSIZE, 0));

#if (BVDC_P_SUPPORT_HSCL_VER >= BVDC_P_SUPPORT_HSCL_VER_6)
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_PIC_OFFSET_R) &= ~(
			BCHP_MASK(HSCL_0_PIC_OFFSET_R, ENABLE) |
			BCHP_MASK(HSCL_0_PIC_OFFSET_R, HSIZE));
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_PIC_OFFSET_R) |=  (
			BCHP_FIELD_DATA(HSCL_0_PIC_OFFSET_R, ENABLE, (ulPicOffsetLeft != ulPicOffsetLeft_R)) |
			BCHP_FIELD_DATA(HSCL_0_PIC_OFFSET_R, HSIZE, ulPicOffsetLeft_R));
#endif

		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_BVB_IN_SIZE) &= ~(
			BCHP_MASK(HSCL_0_BVB_IN_SIZE, HSIZE) |
			BCHP_MASK(HSCL_0_BVB_IN_SIZE, VSIZE));
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_BVB_IN_SIZE) |=  (
			BCHP_FIELD_DATA(HSCL_0_BVB_IN_SIZE, HSIZE, ulBvbInHSize) |
			BCHP_FIELD_DATA(HSCL_0_BVB_IN_SIZE, VSIZE, ulDstVSize));

		/* SRC_PIC_SIZE should include FIR_LUMA_SRC_PIC_OFFSET and align */
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_SRC_PIC_SIZE) &= ~(
			BCHP_MASK(HSCL_0_SRC_PIC_SIZE, HSIZE) |
			BCHP_MASK(HSCL_0_SRC_PIC_SIZE, VSIZE));
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_SRC_PIC_SIZE) |=  (
			BCHP_FIELD_DATA(HSCL_0_SRC_PIC_SIZE, HSIZE, ulAlgnSrcHSize) |
			BCHP_FIELD_DATA(HSCL_0_SRC_PIC_SIZE, VSIZE, ulDstVSize));

		/* -----------------------------------------------------------------------
		 * 4). set coeffs for both horizontal
		 */
		pHorzFirCoeff = BVDC_P_SelectFirCoeff_isr(hHscaler->pHorzFirCoeffTbl,
			hWindow->stCurInfo.stCtIndex.ulHsclHorzLuma,
			hWindow->stCurInfo.hSource->stCurInfo.eCtInputType,
			pPicture->PicComRulInfo.eSrcOrigPolarity, ulFirHrzStep,
			hHscaler->ulHorzTaps, ulSrcHSize, ulDstHSize);
		BDBG_MSG(("Luma H Coeffs  : %s", pHorzFirCoeff->pchName));
		BVDC_P_Hscaler_SetFirCoeff(hHscaler, pHorzFirCoeff->pulCoeffs);

		pHorzFirCoeff = BVDC_P_SelectFirCoeff_isr(hHscaler->pChromaHorzFirCoeffTbl,
			hWindow->stCurInfo.stCtIndex.ulHsclHorzChroma,
			hWindow->stCurInfo.hSource->stCurInfo.eCtInputType,
			pPicture->PicComRulInfo.eSrcOrigPolarity, ulFirHrzStep,
			hHscaler->ulHorzTaps, ulSrcHSize, ulDstHSize);
		BDBG_MSG(("Chroma H Coeffs: %s", pHorzFirCoeff->pchName));
		BVDC_P_Hscaler_SetChromaFirCoeff(hHscaler, pHorzFirCoeff->pulCoeffs);

		/* -----------------------------------------------------------------------
		 * 5). set init phase for horizontal
		 */
		/* Compute the phase accumulate intial value in S11.6 or S5.26 */
		lHrzPhsAccInit = BVDC_P_FIXED_A_MINUS_FIXED_B(
			BVDC_P_H_INIT_PHASE_RATIO_ADJ(ulFirHrzStep) / 2,
			BVDC_P_H_INIT_PHASE_0_POINT_5);

		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET) &= ~(
			BCHP_MASK(HSCL_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET, VALUE));
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_CHROMA_SRC_PIC_OFFSET) &= ~(
			BCHP_MASK(HSCL_0_HORIZ_FIR_CHROMA_SRC_PIC_OFFSET, VALUE));
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET) |=  (
			BCHP_FIELD_DATA(HSCL_0_HORIZ_FIR_LUMA_SRC_PIC_OFFSET, VALUE,
			ulPanScanLeft));
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_CHROMA_SRC_PIC_OFFSET) |=  (
			BCHP_FIELD_DATA(HSCL_0_HORIZ_FIR_CHROMA_SRC_PIC_OFFSET, VALUE,
			ulPanScanLeft));

		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_INIT_PHASE_ACC) &= ~(
			BCHP_MASK(HSCL_0_HORIZ_FIR_INIT_PHASE_ACC, SIZE));
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_INIT_PHASE_ACC) |=  (
			BCHP_FIELD_DATA(HSCL_0_HORIZ_FIR_INIT_PHASE_ACC, SIZE,
			lHrzPhsAccInit));

#if (BVDC_P_SUPPORT_HSCL_VER >= BVDC_P_SUPPORT_HSCL_VER_5)
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_INIT_PHASE_ACC_R) &= ~(
			BCHP_MASK(HSCL_0_HORIZ_FIR_INIT_PHASE_ACC_R, SIZE));
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_FIR_INIT_PHASE_ACC_R) |=  (
			BCHP_FIELD_DATA(HSCL_0_HORIZ_FIR_INIT_PHASE_ACC_R, SIZE,
			lHrzPhsAccInit));
#endif
	}

#ifdef BCHP_HSCL_0_HORIZ_DITHER_CTRL
	/* Dither */
	if(hHscaler->bDithering)
	{
		if((BVDC_P_VNET_USED_ANR(hWindow->stVnetMode) &&
		    hWindow->stCurResource.hAnr && hWindow->stCurResource.hAnr->stTestFeature1.bEnable) ||
		   (BVDC_P_VNET_USED_MAD(hWindow->stVnetMode) &&
		    hWindow->stCurResource.hMad32 && hWindow->stCurResource.hMad32->stTestFeature1.bEnable) ||
		   (BVDC_P_VNET_USED_SCALER_AT_WRITER(hWindow->stVnetMode) &&
		    hWindow->stCurInfo.stCapTestFeature1.bEnable))
		{
			/* Turn off dither if TestFeature1 is enabled for MAD or ANR
			 * or CAP at reader */
			/* Dither off when:
			 *  1) TestFeature1 is enabled on ANR since HSCL->ANR always
			 *  2) TestFeature1 is enabled on MAD since HSCL->MAD always
			 *  3) TestFeature1 is enabled on CAP for HSCL->ANR/MAD->CAP
			 */
			hHscaler->stDither.ulMode = 0;
		}
		else
		{
			hHscaler->stDither.ulMode = 2;
		}

		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_DITHER_CTRL) &=  ~(
			BCHP_MASK(HSCL_0_HORIZ_DITHER_CTRL, MODE));
		BVDC_P_HSCL_GET_REG_DATA(HSCL_0_HORIZ_DITHER_CTRL) |=  (
			BCHP_FIELD_DATA(HSCL_0_HORIZ_DITHER_CTRL, MODE, hHscaler->stDither.ulMode));
	}
#endif

	/* Printing out ratio in float format would be nice, but PI
	 * code does permit float. */
	if(BVDC_P_RUL_UPDATE_THRESHOLD == hHscaler->ulUpdateAll)
	{
		BDBG_MSG(("-------------------------"));
		BDBG_MSG(("Hscaler[%d]         : %dx%d to %dx%d", hHscaler->eId,
			pHsclIn->ulWidth,  pHsclIn->ulHeight,
			pHsclOut->ulWidth, pHsclOut->ulHeight));
		BDBG_MSG(("Hscaler[%d]'clip    : left %d, width %d, left_R %d", 
				hHscaler->eId, pPicture->lHsclCutLeft, pPicture->ulHsclCutWidth,
				pPicture->lHsclCutLeft_R));
		BDBG_MSG(("ulFirHrzStep      : %-8lx", ulFirHrzStep));
		BDBG_MSG(("PIC_OFFSET_LEFT   : %-8lx", ulPicOffsetLeft));
		BDBG_MSG(("H_InitPhase       : %-8lx", lHrzPhsAccInit));
	}

	BDBG_LEAVE(BVDC_P_Hscaler_SetInfo_isr);
	return;
}

/***************************************************************************
 * {private}
 *
 */
void BVDC_P_Hscaler_SetEnable_isr
	( BVDC_P_Hscaler_Handle            hHscaler,
	  bool                             bEnable )
{
	BDBG_OBJECT_ASSERT(hHscaler, BVDC_HSL);

	if(!BVDC_P_HSCL_COMPARE_FIELD_DATA(HSCL_0_ENABLE, SCALER_ENABLE, bEnable))
	{
		hHscaler->ulUpdateAll = BVDC_P_RUL_UPDATE_THRESHOLD;
	}

	/* Turn on/off the scaler. */
	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_ENABLE) &= ~(
		BCHP_MASK(HSCL_0_ENABLE, SCALER_ENABLE));

	BVDC_P_HSCL_GET_REG_DATA(HSCL_0_ENABLE) |=  (bEnable
		? BCHP_FIELD_ENUM(HSCL_0_ENABLE, SCALER_ENABLE, ON)
		: BCHP_FIELD_ENUM(HSCL_0_ENABLE, SCALER_ENABLE, OFF));

	return;
}
#else
BERR_Code BVDC_P_Hscaler_Create
	( BVDC_P_Hscaler_Handle           *phHscaler,
	  BVDC_P_HscalerId                 eHscalerId,
	  BVDC_P_Resource_Handle           hResource,
	  BREG_Handle                      hReg )
{
	BSTD_UNUSED(phHscaler);
	BSTD_UNUSED(eHscalerId);
	BSTD_UNUSED(hResource);
	BSTD_UNUSED(hReg);
	return BERR_SUCCESS;
}

void BVDC_P_Hscaler_Destroy
	( BVDC_P_Hscaler_Handle            hHscaler )
{
	BSTD_UNUSED(hHscaler);
	return;
}

void BVDC_P_Hscaler_Init
	( BVDC_P_Hscaler_Handle            hHscaler )
{
	BSTD_UNUSED(hHscaler);
	return;
}

void BVDC_P_Hscaler_BuildRul_SetEnable_isr
	( BVDC_P_Hscaler_Handle            hHscaler,
	  BVDC_P_ListInfo                 *pList )
{
	BSTD_UNUSED(hHscaler);
	BSTD_UNUSED(pList);
}

void BVDC_P_Hscaler_SetInfo_isr
	( BVDC_P_Hscaler_Handle            hHscaler,
	  BVDC_Window_Handle               hWindow,
	  const BVDC_P_PictureNodePtr      pPicture )
{
	BSTD_UNUSED(hHscaler);
	BSTD_UNUSED(hWindow);
	BSTD_UNUSED(pPicture);
	return;
}

void BVDC_P_Hscaler_SetEnable_isr
	( BVDC_P_Hscaler_Handle            hHscaler,
	  bool                             bEnable )
{
	BSTD_UNUSED(hHscaler);
	BSTD_UNUSED(bEnable);
	return;
}
#endif  /* #if (BVDC_P_SUPPORT_HSCL) || (BVDC_P_SUPPORT_HSCL_MAD_HARD_WIRED) */

#if !(BVDC_P_SUPPORT_HSCL)

BERR_Code BVDC_P_Hscaler_AcquireConnect
	( BVDC_P_Hscaler_Handle            hHscaler,
	  BVDC_P_WindowId                  eWinId)
{
	BSTD_UNUSED(hHscaler);
	BSTD_UNUSED(eWinId);
	return BERR_SUCCESS;
}

BERR_Code BVDC_P_Hscaler_ReleaseConnect_isr
	( BVDC_P_Hscaler_Handle           *phHscaler )
{
	BSTD_UNUSED(phHscaler);
	return BERR_SUCCESS;
}

void BVDC_P_Hscaler_BuildRul_isr
	( const BVDC_P_Hscaler_Handle      hHscaler,
	  BVDC_P_ListInfo                 *pList,
	  BVDC_P_State                     eVnetState,
	  BVDC_P_PicComRulInfo            *pPicComRulInfo )
{
	BSTD_UNUSED(hHscaler);
	BSTD_UNUSED(pList);
	BSTD_UNUSED(eVnetState);
	BSTD_UNUSED(pPicComRulInfo);
	return;
}

#endif /* #if !(BVDC_P_SUPPORT_HSCL) */

/* End of file. */
