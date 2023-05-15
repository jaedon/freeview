/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_ccb_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 10/17/08 2:33p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_ccb_priv.c $
 * 
 * Hydra_Software_Devel/8   10/17/08 2:33p tdo
 * PR47744: Remove warnings and optimize data copying
 *
 * Hydra_Software_Devel/7   10/17/08 2:12p tdo
 * PR47744: Provide smooth updating of CCB table
 *
 * Hydra_Software_Devel/6   2/19/08 9:57a rpan
 * PR39421: Added DVI color converter block support to bcm3548.
 *
 * Hydra_Software_Devel/5   6/26/07 12:08p tdo
 * PR32520: Use AUTO mode so that CCB table can be updated while active
 *
 * Hydra_Software_Devel/4   3/26/07 11:03p tdo
 * PR 28402: Provide API to load customized CCB table for whitebalance
 *
 * Hydra_Software_Devel/3   2/6/07 3:21p tdo
 * PR 27576: Move CAB table and DVI_CCB table program out of critical
 * section
 *
 * Hydra_Software_Devel/2   1/16/07 8:01p hongtaoz
 * PR24457: bringup 3563 gamma correction;
 *
 * Hydra_Software_Devel/1   12/18/06 11:41p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/2   11/1/06 2:29p tdo
 * PR 24457:  Add color temp to white balance support for DVO output
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   10/23/06 2:25p tdo
 * PR 24457: Add support for white balance
 *
 ***************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "bvdc_display_priv.h"

BDBG_MODULE(BVDC_DISP);

void BVDC_P_Vec_BuildCCBRul_isr
	( BVDC_P_DisplayContext           *pDisplay,
	  BVDC_P_ListInfo                 *pList)
{
#if BVDC_P_VEC_SUPPORT_DVI_COLOR_CNVT
	/* TODO: This build RUL should be off the current context */
	BVDC_P_DisplayInfo  *pNewInfo = &pDisplay->stNewInfo;

	BDBG_ENTER(BVDC_P_Vec_BuildCCBRul_isr);
	BDBG_MSG(("disp[%d] - Build BVDC_P_Vec_BuildCCBRul_isr", pDisplay->eId));

	if(pNewInfo->bCCEnable)
	{
		if(!pNewInfo->bUserCCTable)
		{
			const BVDC_P_FormatCCBTbl *pCCBTbl;
			pCCBTbl = BVDC_P_Display_GetCCBTable(pNewInfo->ulGammaTableId, pNewInfo->ulColorTempId);
			if(pCCBTbl == NULL)
			{
				return;
			}

			/* Copy pCCBTbl to RUL */
			BDBG_MSG(("disp[%d] - Load Internal table", pDisplay->eId));
			*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_CCB_TABLE_SIZE);
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_CCB_CCB_ELEMENT_i_ARRAY_BASE);
			BKNI_Memcpy((void*)pList->pulCurrent, (void*)&pCCBTbl->pulCCBTbl[0], BVDC_P_CCB_TABLE_SIZE * sizeof(uint32_t));
			pList->pulCurrent += BVDC_P_CCB_TABLE_SIZE;
		}
		else
		{
			/* Copy user table to RUL */
			BDBG_MSG(("disp[%d] - Load External table", pDisplay->eId));
			*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_CCB_TABLE_SIZE);
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_CCB_CCB_ELEMENT_i_ARRAY_BASE);
			BKNI_Memcpy((void*)pList->pulCurrent, (void*)&pNewInfo->aulUserCCTable[0], BVDC_P_CCB_TABLE_SIZE * sizeof(uint32_t));
			pList->pulCurrent += BVDC_P_CCB_TABLE_SIZE;
		}
	}

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_DVI_CCB_CCB_CTRL);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(DVI_CCB_CCB_CTRL, BYPASS,                    AUTO) |
		BCHP_FIELD_ENUM(DVI_CCB_CCB_CTRL, CH2_WR_ENABLE,               ON) |
		BCHP_FIELD_ENUM(DVI_CCB_CCB_CTRL, CH1_WR_ENABLE,               ON) |
		BCHP_FIELD_ENUM(DVI_CCB_CCB_CTRL, CH0_WR_ENABLE,               ON) |
		BCHP_FIELD_DATA(DVI_CCB_CCB_CTRL, CH2_ENABLE, pNewInfo->bCCEnable) |
		BCHP_FIELD_DATA(DVI_CCB_CCB_CTRL, CH1_ENABLE, pNewInfo->bCCEnable) |
		BCHP_FIELD_DATA(DVI_CCB_CCB_CTRL, CH0_ENABLE, pNewInfo->bCCEnable);

	BDBG_LEAVE(BVDC_P_Vec_BuildCCBRul_isr);
#else
	BSTD_UNUSED(pDisplay);
	BSTD_UNUSED(pList);
#endif

	return;
}

/* End of File */
