/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bgio.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 6/16/11 5:58p $
 *
 * Module Description:
 *
 *
 * Revision History:
 * $brcm_Log: /magnum/portinginterface/gio/7038/bgio.c $
 * 
 * Hydra_Software_Devel/3   6/16/11 5:58p tdo
 * SWDTV-7592: add BDBG_OBJECT_ASSERT for BGIO.
 *
 * Hydra_Software_Devel/2   9/12/07 5:58p syang
 * PR 30391, PR 32351: BGIO only init for the pin created by BGIO to ovoid
 * override configures by other sw entity; BGIO read from HW reg (no more
 * sw buffering);
 *
 * Hydra_Software_Devel/1   2/24/04 7:17p syang
 * PR 9785: init version
 *
 ***************************************************************************/

#include "bgio.h"             /*  */
#include "bgio_priv.h"        /*  */
#include "bkni.h"

BDBG_MODULE(BGIO);
BDBG_OBJECT_ID(BGIO);

/***************************************************************************
 *
 */
BERR_Code BGIO_Open(
	BGIO_Handle *         phGpio,
	BCHP_Handle           hChip,
	BREG_Handle           hRegister )
{
	BERR_Code  eResult = BERR_SUCCESS;
	BGIO_P_Context *  pGpio = NULL;

	if ( (NULL == phGpio) ||
		 (NULL == hChip) ||
		 (NULL == hRegister) )
	{
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	pGpio = (BGIO_P_Context *)BKNI_Malloc( sizeof(BGIO_P_Context) );
	if ( NULL == pGpio )
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}
	BKNI_Memset((void*)pGpio, 0x0, sizeof(BGIO_P_Context));
	BDBG_OBJECT_SET(pGpio, BGIO);

	/* init pin list */
	BLST_D_INIT(&pGpio->PinHead);

	pGpio->hChip = hChip;
	pGpio->hRegister = hRegister;
	BGIO_P_MAIN_SET_BLACK_MAGIC(pGpio);

	*phGpio = pGpio;
	return eResult;
}

/***************************************************************************
 *
 */
BERR_Code BGIO_Close(
	BGIO_Handle           hGpio )
{
	BDBG_OBJECT_ASSERT(hGpio, BGIO);

	/* sub-modules have to be explicitly destroied first */
	if ( true != BLST_D_EMPTY(&hGpio->PinHead) )
	{
		return BERR_TRACE(BERR_LEAKED_RESOURCE);
	}

	BDBG_OBJECT_DESTROY(hGpio, BGIO);
	BKNI_Free((void*)hGpio);
	return BERR_SUCCESS;
}

/* End of File */
