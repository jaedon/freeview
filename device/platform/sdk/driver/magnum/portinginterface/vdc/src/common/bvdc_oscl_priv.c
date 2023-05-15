/***************************************************************************
 *     Copyright (c) 2009-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_oscl_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 1/24/12 5:15p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_oscl_priv.c $
 * 
 * Hydra_Software_Devel/10   1/24/12 5:15p pntruong
 * SW7468-400: Corrected the initial phase programming.
 *
 * Hydra_Software_Devel/9   4/19/10 10:17p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic. Move
 * BDBG_OBJECT_ID_DECLARE private header files instead of .c.
 *
 * Hydra_Software_Devel/8   4/7/10 11:33a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/7   4/5/10 4:12p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/6   12/21/09 2:45p rpan
 * SW7468-30: 1) When OSCL is enabled for 1080p display, top and bottom
 * RDC slots are used for building RULs.
 * 2) Addressed the video format stored at hCompistor being overwritten
 * issue.
 *
 * Hydra_Software_Devel/5   12/15/09 1:57p rpan
 * SW7468-30: Brought up 1080p output on 7468.
 *
 * Hydra_Software_Devel/4   12/14/09 4:28p rpan
 * SW7468-30: Enable BUFF_BYPASS for non-1080p mode.
 *
 * Hydra_Software_Devel/3   11/30/09 4:37p rpan
 * SW7468-30: Fixed a compile error for chips without OSCL.
 *
 * Hydra_Software_Devel/2   11/30/09 4:08p pntruong
 * SW7468-30: Corrected standard header.
 *
 ***************************************************************************/
#include "bvdc_oscl_priv.h"
#include "bmth_fix.h"
#include "bvdc_common_priv.h"

#if (BVDC_P_SUPPORT_OSCL)
#include "bchp_oscl_0.h"
#endif


BDBG_MODULE(BVDC_OSCL);

#if (BVDC_P_SUPPORT_OSCL)
void BVDC_P_OSCL_BuildRul_isr
	( BVDC_Compositor_Handle           hCompositor,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId )
{
	BVDC_P_Compositor_Info	 *pCurInfo;

	BDBG_OBJECT_ASSERT(hCompositor, BVDC_CMP);
	pCurInfo = &hCompositor->stCurInfo;

	/* Note: Currently we update each field of OSCL every vSync. This can be
	 * optimized so that common fields will be programmed only once, and only
	 * INIT_PHASE gets updated every vSync. */
	if (pCurInfo->bEnableOScl)
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_OSCL_0_SRC_HSIZE);
		*pList->pulCurrent++ = BCHP_FIELD_DATA(OSCL_0_SRC_HSIZE, HSIZE, pCurInfo->pFmtInfo->ulWidth);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_OSCL_0_DST_VSIZE);
		*pList->pulCurrent++ = BCHP_FIELD_DATA(OSCL_0_DST_VSIZE, VSIZE, pCurInfo->pFmtInfo->ulHeight);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_OSCL_0_VERT_FIR_INIT_PHASE);
		*pList->pulCurrent++ = (BAVC_Polarity_eTopField == eFieldId) ? 0 : 0xffff8000;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_OSCL_0_VERT_FIR_SRC_STEP);
		*pList->pulCurrent++ = 0x00080000;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_OSCL_0_VERT_FIR_COEFF1_PHASE_00_01);
		*pList->pulCurrent++ = 0x08001000;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_OSCL_0_VERT_FIR_COEFF1_PHASE_02_03);
		*pList->pulCurrent++ = 0x08000800;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_OSCL_0_VERT_FIR_COEFF1_PHASE_04_05);
		*pList->pulCurrent++ = 0x08000800;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_OSCL_0_VERT_FIR_COEFF1_PHASE_06_07);
		*pList->pulCurrent++ = 0x08000800;

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_OSCL_0_CTRL);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(OSCL_0_CTRL, MODE, FILTER) |
			BCHP_FIELD_ENUM(OSCL_0_CTRL, ENABLE_CTRL, AUTO_DISABLE) |
			BCHP_FIELD_ENUM(OSCL_0_CTRL, BUFF_BYPASS, NORMAL)  |
			BCHP_FIELD_ENUM(OSCL_0_CTRL, SCALE_BYPASS, NORMAL) |
			BCHP_FIELD_ENUM(OSCL_0_CTRL, BUFF_ENABLE, ENABLE)  |
			BCHP_FIELD_ENUM(OSCL_0_CTRL, SCALE_ENABLE, ENABLE);
	}
	else
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_OSCL_0_CTRL);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(OSCL_0_CTRL, SCALE_BYPASS, BYPASS) |
			BCHP_FIELD_ENUM(OSCL_0_CTRL, BUFF_BYPASS, BYPASS);

	}
}
#endif
/* End of File */
