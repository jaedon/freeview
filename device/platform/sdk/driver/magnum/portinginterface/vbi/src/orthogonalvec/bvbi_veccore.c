/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_veccore.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 4/21/11 7:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7420/bvbi_veccore.c $
 * 
 * Hydra_Software_Devel/9   4/21/11 7:21p darnstein
 * SW7125-916: Wrong interrupt control register was being programmed.
 * 
 * Hydra_Software_Devel/8   10/1/10 2:45p darnstein
 * SW7422-46: Adapt to 7422 and 7425 chipsets.
 * 
 * Hydra_Software_Devel/7   1/8/10 3:31p darnstein
 * SW7550-128: If a VBIU    encoder object already owns a VBI core of a
 * certain type, then do not allocate a second core of the same type.
 * 
 * Hydra_Software_Devel/6   1/7/10 5:14p darnstein
 * SW7420-525: fix a programming error (mine) involving the release of a
 * resource.
 * 
 * Hydra_Software_Devel/5   1/4/10 2:22p darnstein
 * SW7420-525: fix sloppy programming (mine) involving a loop variable
 * being used outside of its loop.
 * 
 * Hydra_Software_Devel/4   1/26/09 5:40p darnstein
 * PR45819: fix mechanical error in "reserve core" routine.
 * 
 * Hydra_Software_Devel/3   12/11/08 4:22p darnstein
 * PR45819: program VBI_ENC and VEC_CFG cores.
 * 
 * Hydra_Software_Devel/2   12/8/08 7:36p darnstein
 * PR49987: fix bug in BVBI_Encode_Create(). Affects 656 output.
 * 
 * Hydra_Software_Devel/1   12/3/08 8:02p darnstein
 * PR45819:
 * 
 ***************************************************************************/

#include "bstd.h"           /* standard types */
#include "bdbg.h"           /* Dbglib */
#include "bkni.h"			/* For critical sections */
#include "bvbi.h"           /* VBI processing, this module. */
#include "bvbi_priv.h"      /* VBI internal data structures */

BDBG_MODULE(BVBI);

/***************************************************************************
 *
 */
BERR_Code BVBI_P_Encode_ReserveCore (
	BAVC_VbiPath eDest, uint32_t ulSelect_Standard,
	uint8_t vecHwCoreMask[BVBI_P_EncCoreType_eLAST],
	uint8_t vecHwCoreMask_656[BVBI_P_EncCoreType_eLAST],
	uint8_t hwCoreIndex[BVBI_P_EncCoreType_eLAST])
{
	bool bIs656;
	uint8_t* hwCoreMask;
	uint32_t ulCoreIndex;
	uint32_t upperLimit = 0;
	uint32_t ulSelectedCoreIndex = 0xFF;
	BVBI_P_EncCoreType eType = BVBI_P_EncCoreType_eLAST;

	bIs656 = BVBI_P_is656 (eDest);

	switch (ulSelect_Standard)
	{
	case BVBI_P_SELECT_AMOL:
		upperLimit = bIs656 ? BVBI_P_NUM_AMOLE_656 : BVBI_P_NUM_AMOLE;
		eType = BVBI_P_EncCoreType_eAMOLE;
		break;
	case BVBI_P_SELECT_CC:
	case BVBI_P_SELECT_MCC:
		upperLimit = bIs656 ? BVBI_P_NUM_CCE_656 : BVBI_P_NUM_CCE;
		eType = BVBI_P_EncCoreType_eCCE;
		break;
	case BVBI_P_SELECT_CGMSA:
	case BVBI_P_SELECT_CGMSB:
		upperLimit = bIs656 ? BVBI_P_NUM_CGMSAE_656 : BVBI_P_NUM_CGMSAE;
		eType = BVBI_P_EncCoreType_eCGMSAE;
		break;
	case BVBI_P_SELECT_GS:
		upperLimit = bIs656 ? BVBI_P_NUM_GSE_656 : BVBI_P_NUM_GSE;
		eType = BVBI_P_EncCoreType_eGSE;
		break;
	case BVBI_P_SELECT_SCTE:
		upperLimit = bIs656 ? BVBI_P_NUM_SCTEE_656 : BVBI_P_NUM_CCE;
		eType = BVBI_P_EncCoreType_eSCTE;
		break;
	case BVBI_P_SELECT_TT:
		upperLimit = bIs656 ? BVBI_P_NUM_TTE_656 : BVBI_P_NUM_TTE;
		eType = BVBI_P_EncCoreType_eTTE;
		break;
	case BVBI_P_SELECT_VPS:
	case BVBI_P_SELECT_WSS:
		upperLimit = bIs656 ? BVBI_P_NUM_WSE_656 : BVBI_P_NUM_WSE;
		eType = BVBI_P_EncCoreType_eWSE;
		break;
	case BVBI_P_SELECT_VBIENC:
		upperLimit = 1;
		eType = BVBI_P_EncCoreType_eVBIENC;
		break;
	case BVBI_P_SELECT_ANCI:
		upperLimit = bIs656 ? BVBI_P_NUM_ANCI656_656 : 0 ;
		eType = BVBI_P_EncCoreType_eANCI;
		break;
	default:
		BDBG_ASSERT(0);
		break;
	}

	/* 
	 * Only one copy of each core type is required. If a core of the required
	 * type has already been allocated, then no further work is necessary.
	 */
	if (hwCoreIndex[eType] == 0xFF)
	{
		/* Special handling for VBI_ENC core. There is only one per VEC path. */
		if (eType == BVBI_P_EncCoreType_eVBIENC)
		{
			hwCoreIndex[eType] = eDest - BAVC_VbiPath_eVec0;
			return BERR_SUCCESS;
		}

		/* Special handling for ANCI656 core. There is only one per VEC path. */
		if (eType == BVBI_P_EncCoreType_eANCI)
		{
			if (bIs656)
			{
				hwCoreIndex[eType] = eDest - BAVC_VbiPath_eBypass0;
				return BERR_SUCCESS;
			}
			else
			{
				return BVBI_ERR_HW_UNSUPPORTED;
			}
		}

		/* Search for an appropriate core not already in use */
		if (bIs656)
		{
			hwCoreMask = vecHwCoreMask_656;
		}
		else
		{
			hwCoreMask = vecHwCoreMask;
		}
		for (ulCoreIndex = 0 ; ulCoreIndex < upperLimit ; ++ulCoreIndex)
		{
			if (((hwCoreMask[eType]) & (1 << ulCoreIndex)) == 0x0)
			{
				ulSelectedCoreIndex = ulCoreIndex;
				hwCoreMask[eType] |= (1 << ulSelectedCoreIndex);
				hwCoreIndex[eType] = ulSelectedCoreIndex;
				break;
			}
		}

		/* No available hardware core */
		if (ulSelectedCoreIndex == 0xFF)
		{
			return BVBI_ERR_HW_UNSUPPORTED;
		}
	}

	/* Success! */
	return BERR_SUCCESS;
}

/***************************************************************************
 *
 */
void BVBI_P_Encode_ReleaseCore (
	BAVC_VbiPath eDest, uint32_t ulSelect_Standard,
	uint32_t ulActive_Standards, 
	uint8_t vecHwCoreMask[BVBI_P_EncCoreType_eLAST],
	uint8_t vecHwCoreMask_656[BVBI_P_EncCoreType_eLAST],
	uint8_t hwCoreIndex[BVBI_P_EncCoreType_eLAST])
{

	bool bIs656;
	uint8_t* hwCoreMask;
	uint32_t ulCoreIndex = 0xFF;
	BVBI_P_EncCoreType eType = BVBI_P_EncCoreType_eLAST;

	bIs656 = BVBI_P_is656 (eDest);

	switch (ulSelect_Standard)
	{
	case BVBI_P_SELECT_AMOL:
		eType = BVBI_P_EncCoreType_eAMOLE;
		break;
	case BVBI_P_SELECT_CC:
		if ((ulActive_Standards & BVBI_P_SELECT_MCC) == 0)
			eType = BVBI_P_EncCoreType_eCCE;
		break;
	case BVBI_P_SELECT_MCC:
		if ((ulActive_Standards & BVBI_P_SELECT_CC) == 0)
			eType = BVBI_P_EncCoreType_eCCE;
		break;
	case BVBI_P_SELECT_CGMSA:
		if ((ulActive_Standards & BVBI_P_SELECT_CGMSB) == 0)
			eType = BVBI_P_EncCoreType_eCGMSAE;
		break;
	case BVBI_P_SELECT_CGMSB:
		if ((ulActive_Standards & BVBI_P_SELECT_CGMSA) == 0)
			eType = BVBI_P_EncCoreType_eCGMSAE;
		break;
	case BVBI_P_SELECT_GS:
		eType = BVBI_P_EncCoreType_eGSE;
		break;
	case BVBI_P_SELECT_SCTE:
		eType = BVBI_P_EncCoreType_eSCTE;
		break;
	case BVBI_P_SELECT_TT:
		eType = BVBI_P_EncCoreType_eTTE;
		break;
	case BVBI_P_SELECT_VPS:
		if ((ulActive_Standards & BVBI_P_SELECT_WSS) == 0)
			eType = BVBI_P_EncCoreType_eWSE;
		break;
	case BVBI_P_SELECT_WSS:
		if ((ulActive_Standards & BVBI_P_SELECT_VPS) == 0)
			eType = BVBI_P_EncCoreType_eWSE;
		break;
	case BVBI_P_SELECT_VBIENC:
		eType = BVBI_P_EncCoreType_eVBIENC;
		break;
	case BVBI_P_SELECT_ANCI:
		eType = BVBI_P_EncCoreType_eANCI;
		break;
	default:
		BDBG_ASSERT(0);
		break;
	}

	/* Release in main VBI handle */
	if (bIs656)
	{
		hwCoreMask = vecHwCoreMask_656;
	}
	else
	{
		hwCoreMask = vecHwCoreMask;
	}
	if (eType != BVBI_P_EncCoreType_eLAST)
	{
		ulCoreIndex = hwCoreIndex[eType];
		if (ulCoreIndex != 0xFF)
		{
			hwCoreMask[eType] &= ~(1 << ulCoreIndex);
			hwCoreIndex[eType] = 0xFF;
		}
	}
}


/***************************************************************************
 *
 */
void BVBI_P_Encode_ConnectCores (
	BREG_Handle hReg, BAVC_VbiPath eDest, 
	uint8_t hwCoreIndex[BVBI_P_EncCoreType_eLAST])
{
	BVBI_P_VE_Crossbar_Program (hReg, eDest, hwCoreIndex);
}
