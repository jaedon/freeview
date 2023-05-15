/***************************************************************************
 *     Copyright (c) 2007, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdgp_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/21/07 1:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/dgp/7405/bdgp_priv.c $
 * 
 * Hydra_Software_Devel/1   6/21/07 1:55p nissen
 * PR 29766: Initial version of DGP module.
 *
 ***************************************************************************/

#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#include "bchp_dgp_0.h"
#include "bchp_int_id_bvnb_intr2.h"
#include "bdgp.h"
#include "bdgp_priv.h"

/***************************************************************************/
void BDGP_P_Trigger_isr( 
	void *pvParam1, 
	int iParam2 )
{
 	BDGP_Handle hDgp = (BDGP_Handle) pvParam1;
	BSTD_UNUSED( iParam2 );

	/* call installed callback function */
	if( hDgp->pfCallback )
		(*hDgp->pfCallback)( hDgp->pvCallbackParm1, hDgp->iCallbackParm2 );
}

/* End of File */
