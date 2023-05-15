/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_nomacrovision_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/34 $
 * $brcm_Date: 10/14/09 8:43a $
 *
 * Module Description:
 *		Private module for No Macrovision support. Do not include
 * bvdc_macrovision_priv.c, if this file is included in your project.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_nomacrovision_priv.c $
 * 
 * Hydra_Software_Devel/34   10/14/09 8:43a tdo
 * SW7405-3004: install 480P ARIB output via BVDC display settings.
 *
 * Hydra_Software_Devel/33   11/7/08 6:39p darnstein
 * PR46484: move new diagnostic commands into bvdc_dbg.h, where they
 * belong.
 *
 * Hydra_Software_Devel/32   3/10/08 9:15p darnstein
 * PR25971: first try at DCS for 7400
 *
 * Hydra_Software_Devel/31   10/31/07 11:15a pntruong
 * PR34239: Fixed build errors for non copy protection.
 *
 * Hydra_Software_Devel/30   10/30/07 7:23p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/29   12/14/06 4:15p pntruong
 * PR26039: Removed build warnings.
 *
 * Hydra_Software_Devel/28   9/25/06 4:23p hongtaoz
 * PR24330: removed dependency of non-macrovision code on macrovision
 * header file;
 *
 * Hydra_Software_Devel/27   7/7/06 2:52p pntruong
 * PR22081, PR20395, PR22062, PR22246:  Removed linux build warnings.
 *
 * Hydra_Software_Devel/26   6/30/06 5:45p hongtaoz
 * PR22081, PR20395, PR22062, PR22246: added SECAM Macrovision support;
 * fixed PAL_M dvi microcode; fixed PAL_M sub-carrier frequency setting;
 * PAL_NC should use the same microcode as normal PAL; fixed SECAM and
 * various PAL's csc matrices according to BT.470-6 spec; changed SRC
 * filter setting to beneifit higher freq response for HD output on
 * PRIM_VEC; fixed PAL sync tip setting for non-Macrovision build;
 *
 * Hydra_Software_Devel/25   5/19/06 1:48p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/24   3/24/06 5:40p hongtaoz
 * PR19204: updated neg sync tip value setting for PAL;
 *
 * Hydra_Software_Devel/23   9/21/05 6:30p albertl
 * PR16459:  Display format code restructured, moved to
 * bvdc_displayfmt_priv.c and bvdc_displayfmt.h, and changed to be  table
 * driven  to allow ease of adding new formats.
 *
 * Hydra_Software_Devel/22   8/25/05 4:35p hongtaoz
 * PR15749: fixed the typo for non-macrovision sync-tip level setting;
 *
 * Hydra_Software_Devel/21   3/17/05 6:40p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/20   12/3/04 7:40p hongtaoz
 * PR13164: added stub function BVDC_Display_SetMacrovisionTable; fixed
 * compile warnings;
 *
 * Hydra_Software_Devel/19   11/22/04 1:48p hongtaoz
 * PR13053: fixed negative sync value for SD YPrPb/RGB outputs;
 *
 * Hydra_Software_Devel/18   9/24/04 6:47p hongtaoz
 * PR12444: fixed blackout when changing macrovision types;
 *
 ***************************************************************************/
#include "bstd.h"          /* standard types */
#include "bvdc_display_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bchp_prim_vf.h"

BDBG_MODULE(BVDC_DISP);


/*************************************************************************
 *  {secret}
 *	Returns pointer to appropriate RamTable for display modes, which
 *  supports Macrovision (480i,PAL,480p). Should never get here otherwise!
 **************************************************************************/
const uint32_t* BVDC_P_GetRamTableMv_isr
(
	BVDC_P_VecPath               eVecPath,
	const BVDC_P_DisplayInfo    *pDispInfo,
	bool                         bArib480p
)
{
	BSTD_UNUSED (eVecPath);
	return BVDC_P_GetRamTable_isr(pDispInfo, bArib480p);
}

/*************************************************************************
 *  {secret}
 *	Returns pointer to appropriate ItTable for display modes.
 **************************************************************************/
const uint32_t* BVDC_P_GetItTableMv_isr
(
	const BVDC_P_DisplayInfo     *pDispInfo
)
{
	return BVDC_P_GetItTable_isr(pDispInfo);
}

BERR_Code BVDC_P_ChangeMvType
(
	BVDC_P_DisplayContext  *pDisplay,
	uint32_t              **ppulRul )
{
	BSTD_UNUSED(pDisplay);
	BSTD_UNUSED(ppulRul);
	return BERR_SUCCESS;
}

/*************************************************************************
 *  {secret}
 **************************************************************************/
uint32_t BVDC_P_GetItConfigMv_isr
(
	const BVDC_P_DisplayInfo     *pDispInfo
)
{
	return BVDC_P_GetItConfig_isr(pDispInfo);
}

/*************************************************************************
 *  {secret}
 *	Just a stub here to match with bvdc_macrovision_priv.c
 **************************************************************************/
uint32_t BVDC_P_GetAgc
(
	BVDC_P_DisplayContext  *pDisplay,
	BFMT_VideoFmt           eVideoFmt,
	uint32_t              **ppRul,
	BVDC_P_VecPath          eVecPath
)
{
	BSTD_UNUSED(pDisplay);
	BSTD_UNUSED(eVideoFmt);
	BSTD_UNUSED(ppRul);
	BSTD_UNUSED(eVecPath);
	return 0;
}

/*************************************************************************
 *  {secret}
 *	Returns SEC_VF_POS_SYNC_VALUES value
 **************************************************************************/
uint32_t BVDC_P_GetPosSyncValue
(
	BVDC_P_DisplayContext  *pDisplay,
	uint32_t              **ppulRul,
	BVDC_P_VecPath          eVecPath
)
{
	uint32_t    ulVfPosSync;
	BVDC_P_DisplayInfo *pDispInfo = &pDisplay->stCurInfo;
	BSTD_UNUSED(ppulRul);
	BSTD_UNUSED(eVecPath);

	if (VIDEO_FORMAT_IS_HD(pDispInfo->pFmtInfo->eVideoFmt))
	{
		ulVfPosSync =
			BCHP_FIELD_DATA(PRIM_VF_POS_SYNC_VALUES, VALUE1, 0x0eb) |
			BCHP_FIELD_DATA(PRIM_VF_POS_SYNC_VALUES, VALUE0, 0x0eb);
	}
	else if (VIDEO_FORMAT_IS_625_LINES(pDispInfo->pFmtInfo->eVideoFmt))
	{
		ulVfPosSync =
			BCHP_FIELD_DATA(PRIM_VF_POS_SYNC_VALUES, VALUE1, 0x230) |
			BCHP_FIELD_DATA(PRIM_VF_POS_SYNC_VALUES, VALUE0, 0x296);
	}
	else
	{
		ulVfPosSync =
			BCHP_FIELD_DATA(PRIM_VF_POS_SYNC_VALUES, VALUE1, 0x230) |
			BCHP_FIELD_DATA(PRIM_VF_POS_SYNC_VALUES, VALUE0, 0x2b1);
	}

	return(ulVfPosSync);
}

/*************************************************************************
 *  {secret}
 *	Returns SEC_CO_VF_POS_SYNC_VALUES value
 **************************************************************************/
uint32_t BVDC_P_GetCoPosSyncValue
(
	BVDC_P_DisplayContext  *pDisplay,
	uint32_t              **ppulRul,
	BVDC_P_VecPath          eVecPath
)
{
	return BVDC_P_GetPosSyncValue (pDisplay, ppulRul, eVecPath);
}

/*************************************************************************
 *  {secret}
 *	Returns SD format SEC_VF_NEG_SYNC_VALUES value
 **************************************************************************/
uint32_t BVDC_P_GetNegSyncValue
(
	BVDC_P_DisplayInfo     *pDispInfo,
	BVDC_P_Output           eOutputColorSpace
)
{
	uint32_t ulVfNegSync;
	BSTD_UNUSED(pDispInfo);

	/* 525/60 CVBS/Svideo-Y outputs use 714/286 picture/sync ratio;
	   525/60 YPbPr/RGB and 625/50 outputs use 700/300 picture/sync ratio */
	if((BVDC_P_Output_eYQI   == eOutputColorSpace) ||
	   (BVDC_P_Output_eYQI_M == eOutputColorSpace) ||
	   (BVDC_P_Output_eYUV_N == eOutputColorSpace))
	{
		ulVfNegSync =
			BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE2,
				BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_NTSC_SYNC_LEVEL)) | /* 286 mv */
			BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE1,
				BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_NTSC_SYNC_LEVEL)) | /* 286 mv */
			BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE0, 16);
	}
	else
	{
		ulVfNegSync = /* 700mV/300mV pix/sync ratio */
			BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE2,
				BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL)) | /* 300 mv */
			BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE1,
				BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL)) | /* 300 mv */
			BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE0, 16);
	}

	return(ulVfNegSync);
}

/*************************************************************************
 *  {secret}
 *  This function is for SD RGB output only.
 *	Returns SEC_VF_FORMAT_ADDER value
 **************************************************************************/
uint32_t BVDC_P_GetFmtAdderValue
(
	BVDC_P_DisplayInfo     *pDispInfo
)
{
	const uint32_t *pTable;
	BVDC_P_GetVfTable_isr(pDispInfo, BVDC_P_Output_eRGB, &pTable, NULL);
	return pTable[0];
}

/*************************************************************************
 *  {secret}
 *  This function is to validate the macrovision settings.
 **************************************************************************/
BERR_Code BVDC_P_ValidateMacrovision
(
	BVDC_P_DisplayContext   *pDisplay
)
{
	BSTD_UNUSED(pDisplay);
	return BERR_SUCCESS;
}


/*************************************************************************
 *  {secret}
 *  BVDC_P_Build_SyncPCL_isr
 *  Build the IT PCL_0 setting for h/v sync outputs;
 **************************************************************************/
void BVDC_P_Vec_Build_SyncPCL_isr
	( BVDC_P_DisplayContext           *pDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	BDBG_ENTER(BVDC_P_Vec_Build_SyncPCL_isr);

	/* PR13177: For DTV application, RGB + Hsync are Dac outputs, Vsync is
	Ext digital output. */
	if(BVDC_P_Display_FindDac(pDisplay, BVDC_DacOutput_eHsync))
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_IT_PCL_0 + pDisplay->lItOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, VBI_DATA_ACTIVE_ENABLE,         ON) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, VBI_DATA_ACTIVE_MUX_SELECT ,    3) |
#if BVDC_P_SUPPORT_EXT_SYNC_PCL_0
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, EXT_VSYNC_ENABLE,               ON) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, EXT_VSYNC_MUX_SELECT,           0) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, EXT_HSYNC_ENABLE,               ON) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, EXT_HSYNC_MUX_SELECT,           0) |
#endif
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_4 ,            ZERO) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_3 ,            ZERO) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_2 ,            ZERO) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_1 ,            ZERO) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_0 ,            ONE) | /* take Hsync only */
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_E_SELECT,           0) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_D_SELECT,           0) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_4_SELECT,           0) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_3_SELECT,           0);
	}
	else /* normal setting */
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_IT_PCL_0 + pDisplay->lItOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, VBI_DATA_ACTIVE_ENABLE,         ON) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, VBI_DATA_ACTIVE_MUX_SELECT ,    3) |
#if BVDC_P_SUPPORT_EXT_SYNC_PCL_0
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, EXT_VSYNC_ENABLE,               ON) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, EXT_VSYNC_MUX_SELECT,           0) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, EXT_HSYNC_ENABLE,               ON) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, EXT_HSYNC_MUX_SELECT,           0) |
#endif
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_4 ,            ZERO) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_3 ,            ZERO) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_2 ,            LINE_A) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_1 ,            LINE_B) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_0 ,            LINE_C) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_E_SELECT,           0) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_D_SELECT,           3) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_4_SELECT,           0) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_3_SELECT,           2);
	}

	BDBG_LEAVE(BVDC_P_Vec_Build_SyncPCL_isr);
	return;
}

BVDC_P_Prot_Alt BVDC_P_Get_Prot_Alt (void)
{
	return BVDC_P_PROT_Alt_None;
}


uint32_t BVDC_P_Display_Modify_SYNC_TRANS_0 (
	BVDC_P_DisplayInfo               *pNewInfo,
	uint32_t                         ulVfSTRegVal)
{
	BSTD_UNUSED (pNewInfo);

	return ulVfSTRegVal;
}
#ifdef BVDC_P_DISPLAY_DUMP
void BVDC_Display_MV_DumpAll_aulVfTable (void)
{
}
void BVDC_Display_MV_DumpAll_aulItTable (void)
{
}
void BVDC_Display_MV_DumpAll_ulItConfig (void)
{
}
void BVDC_Display_MV_DumpTables (void)
{
}
#endif

/* End of File */
