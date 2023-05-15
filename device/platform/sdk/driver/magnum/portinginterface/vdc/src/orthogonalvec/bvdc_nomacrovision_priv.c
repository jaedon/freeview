/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_nomacrovision_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 1/11/12 3:07p $
 *
 * Module Description:
 *		Private module for No Macrovision support. Do not include
 * bvdc_macrovision_priv.c, if this file is included in your project.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7420/bvdc_nomacrovision_priv.c $
 * 
 * Hydra_Software_Devel/7   1/11/12 3:07p darnstein
 * SW7125-1124: merge DCS Hybrid+ to main branch. DCS almost ready for
 * production.
 * 
 * Hydra_Software_Devel/SW7125-1124/1   12/1/11 5:37p darnstein
 * SW7125-1124: remove unneeded functions.
 * 
 * Hydra_Software_Devel/5   9/22/10 3:41p hongtaoz
 * SW7425-13: adding 7425 vdc support for nomv;
 * 
 * Hydra_Software_Devel/4   10/14/09 8:44a tdo
 * SW7405-3004: install 480P ARIB output via BVDC display settings.
 *
 * Hydra_Software_Devel/3   3/16/09 10:58p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/3   3/5/09 9:18p darnstein
 * PR51527: reverse previous check-in.
 *
 * Hydra_Software_Devel/7420_mcvp/2   3/4/09 5:52p tdo
 * PR51527: Fix compiling error
 *
 * Hydra_Software_Devel/7420_mcvp/1   3/3/09 7:26p darnstein
 * PR51527: Eliminate unused state variables eVbiPath and eVecPath. The
 * DCS files compile now, although they need a lot of work to acutally
 * function.
 *
 * Hydra_Software_Devel/2   12/17/08 4:40p rpan
 * PR45804: Return valid neg vsync value.
 *
 * Hydra_Software_Devel/1   11/4/08 9:56a tdo
 * PR45804: Implement and Bringup Orthorgonal VEC
 *
 ***************************************************************************/
#include "bstd.h"          /* standard types */
#include "bvdc_display_priv.h"
#include "bvdc_displayfmt_priv.h"

BDBG_MODULE(BVDC_DISP);


/*************************************************************************
 *  {secret}
 *	Returns pointer to appropriate RamTable for display modes, which
 *  supports Macrovision (480i,PAL,480p). Should never get here otherwise!
 **************************************************************************/
const uint32_t* BVDC_P_GetRamTableMv_isr
(
	const BVDC_P_DisplayInfo     *pDispInfo,
	bool                          bArib480p
)
{
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
 *	Returns SEC_VF_POS_SYNC_VALUES value
 **************************************************************************/
uint32_t BVDC_P_GetPosSyncValue
(
	BVDC_P_DisplayContext  *pDisplay,
	uint32_t              **ppulRul
)
{
	uint32_t    ulVfPosSync = 0;
	BSTD_UNUSED(ppulRul);
	BSTD_UNUSED(pDisplay);

	return(ulVfPosSync);
}

/*************************************************************************
 *  {secret}
 *	Returns SD format SEC_VF_NEG_SYNC_VALUES value
 **************************************************************************/
void BVDC_P_GetNegSyncValue
	( BVDC_P_DisplayInfo              *pDispInfo,
	  BVDC_P_Output                    eOutputColorSpace,
	  uint32_t*                        ulRegVal,
	  uint32_t*                        ulRegValEx)
{
	uint32_t ulValue;
	uint32_t ulValueEx;
	BSTD_UNUSED(pDispInfo);
	BSTD_UNUSED(eOutputColorSpace);
	/* 525/60 CVBS/Svideo-Y outputs use 714/286 picture/sync ratio;
	   525/60 YPbPr/RGB and 625/50 outputs use 700/300 picture/sync ratio */
	if((BVDC_P_Output_eYQI	 == eOutputColorSpace) ||
	   (BVDC_P_Output_eYQI_M == eOutputColorSpace) ||
	   (BVDC_P_Output_eYUV_N == eOutputColorSpace))
	{
		ulValue =
			BCHP_FIELD_DATA(VF_0_NEG_SYNC_VALUES, VALUE1,
				BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(
					BVDC_P_DAC_OUTPUT_NTSC_SYNC_LEVEL)) | /* 286 mv */
			BCHP_FIELD_DATA(VF_0_NEG_SYNC_VALUES, VALUE0, 16);
		ulValueEx = 
			BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(
				BVDC_P_DAC_OUTPUT_NTSC_SYNC_LEVEL);       /* 286 mv */
	}
	else
	{
		ulValue = /* 700mV/300mV pix/sync ratio */
			BCHP_FIELD_DATA(VF_0_NEG_SYNC_VALUES, VALUE1,
				BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(
					BVDC_P_DAC_OUTPUT_SYNC_LEVEL)) | /* 300 mv */
			BCHP_FIELD_DATA(VF_0_NEG_SYNC_VALUES, VALUE0, 16);
		ulValueEx = 
			BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(
				BVDC_P_DAC_OUTPUT_SYNC_LEVEL);       /* 300 mv */
	}

#if (BVDC_P_SUPPORT_VEC_VF_VER < 2)
	ulValue |= 
		BCHP_FIELD_DATA(VF_0_NEG_SYNC_VALUES, VALUE2, ulValueEx);
#else
	ulValueEx = 
		BCHP_FIELD_DATA(VF_0_NEG_SYNC_AMPLITUDE_EXTN, VALUE2, ulValueEx);
#endif
	*ulRegVal   = ulValue;
	*ulRegValEx = ulValueEx;
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

	BSTD_UNUSED(pDisplay);
	BSTD_UNUSED(pList);

	BDBG_LEAVE(BVDC_P_Vec_Build_SyncPCL_isr);
	return;
}


uint32_t BVDC_P_Display_Modify_SYNC_TRANS_0 (
	BVDC_P_DisplayInfo               *pNewInfo,
	uint32_t                         ulVfSTRegVal)
{
	BSTD_UNUSED (pNewInfo);

	return ulVfSTRegVal;
}

/* End of File */
