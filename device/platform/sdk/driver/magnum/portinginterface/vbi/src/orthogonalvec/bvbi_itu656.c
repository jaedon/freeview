/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbi_itu656.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 6/24/09 4:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vbi/7420/bvbi_itu656.c $
 * 
 * Hydra_Software_Devel/3   6/24/09 4:38p darnstein
 * PR56289: BVBI compiles for 7340 chipset now.
 * 
 * Hydra_Software_Devel/2   12/4/08 6:08p darnstein
 * PR45819: 7420 software will now compile, but not link.
 * 
 * Hydra_Software_Devel/1   12/4/08 5:55p darnstein
 * PR45819: copy over from 7400 branch.
 * 
 * Hydra_Software_Devel/1   12/3/08 7:46p darnstein
 * PR45819: Source files for 7400 are no longer symbolic links.
 * 
 * Hydra_Software_Devel/1   11/13/06 3:03p darnstein
 * PR23247: Code to initialize ITU656 core to support output of teletext
 * through auxiliary serial port.
 * 
 ***************************************************************************/

#include "bstd.h"			/* standard types */
#include "bdbg.h"			/* Dbglib */
#include "bvbi.h"			/* VBI processing, this module. */
#include "bvbi_priv.h"		/* VBI internal data structures */

#if (BVBI_P_HAS_XSER_TT != 0) /** { **/

#include "bchp_itu656_0.h"	/* RDB info for ITU656 registers */

BDBG_MODULE(BVBI);


/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/

/***************************************************************************
* Implementation of "BVBI_" API functions
***************************************************************************/

/***************************************************************************
* Implementation of supporting VBI_ENC functions that are not in API
***************************************************************************/

BERR_Code BVBI_P_ITU656_Init( 
	BREG_Handle hReg, const BVBI_XSER_Settings* pXSERdefaultSettings )
{
	uint32_t ulReg;
	uint32_t iMode;

	BDBG_ENTER(BVBI_P_ITU656_Init);

	switch (pXSERdefaultSettings->ttSerialDataSync)
	{
    case BVBI_TTserialDataSync_EAV:
		iMode = BCHP_ITU656_0_ITU656_TTE_CTRL_TTE_MODE_EAV;
		break;
    case BVBI_TTserialDataSync_SAV:
		iMode = BCHP_ITU656_0_ITU656_TTE_CTRL_TTE_MODE_SAV;
		break;
    case BVBI_TTserialDataSync_RQ:
		iMode = BCHP_ITU656_0_ITU656_TTE_CTRL_TTE_MODE_RQ;
		break;
	default:
		iMode = BCHP_ITU656_0_ITU656_TTE_CTRL_TTE_MODE_DISABLED;
		break;
	}

	ulReg = (
	BCHP_FIELD_DATA (ITU656_0_ITU656_TTE_CTRL, TTE_MODE,    iMode) |
	BCHP_FIELD_DATA (ITU656_0_ITU656_TTE_CTRL, DELAY_COUNT, 
	      pXSERdefaultSettings->iTTserialDataSyncDelay) );
	BREG_Write32 (hReg, BCHP_ITU656_0_ITU656_TTE_CTRL, ulReg);

	BDBG_LEAVE(BVBI_P_ITU656_Init);

	return BERR_SUCCESS;
}

BERR_Code BVBI_P_ITU656_Enc_Program (
	BREG_Handle hReg,
	BVBI_XSER_Settings* pSettings,
	uint32_t ulActive_XSER_Standards)
{
	uint32_t ulReg;
	uint32_t iMode;

	BDBG_ENTER(BVBI_P_ITU656_Enc_Program);

	switch (pSettings->ttSerialDataSync)
	{
    case BVBI_TTserialDataSync_EAV:
		iMode = BCHP_ITU656_0_ITU656_TTE_CTRL_TTE_MODE_EAV;
		break;
    case BVBI_TTserialDataSync_SAV:
		iMode = BCHP_ITU656_0_ITU656_TTE_CTRL_TTE_MODE_SAV;
		break;
    case BVBI_TTserialDataSync_RQ:
		iMode = BCHP_ITU656_0_ITU656_TTE_CTRL_TTE_MODE_RQ;
		break;
	default:
		iMode = BCHP_ITU656_0_ITU656_TTE_CTRL_TTE_MODE_DISABLED;
		break;
	}

	if ((ulActive_XSER_Standards & BVBI_P_SELECT_TT) == 0)
		iMode = BCHP_ITU656_0_ITU656_TTE_CTRL_TTE_MODE_DISABLED;

	ulReg = (
	BCHP_FIELD_DATA (ITU656_0_ITU656_TTE_CTRL, TTE_MODE,    iMode) |
	BCHP_FIELD_DATA (ITU656_0_ITU656_TTE_CTRL, DELAY_COUNT, 
	      pSettings->iTTserialDataSyncDelay) );
	BREG_Write32 (hReg, BCHP_ITU656_0_ITU656_TTE_CTRL, ulReg);

	BDBG_LEAVE(BVBI_P_ITU656_Enc_Program);


	return BERR_SUCCESS;
}

#endif /** }  BVBI_P_HAS_XSER_TT **/
