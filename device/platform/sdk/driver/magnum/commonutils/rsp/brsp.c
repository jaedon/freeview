/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brsp.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 9/8/03 6:29p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/rsp/brsp.c $
 * 
 * Hydra_Software_Devel/2   9/8/03 6:29p dlwin
 * Updated API function parameter comment for DocJet.
 * 
 * Hydra_Software_Devel/1   9/3/03 3:34p dlwin
 * Initial version
 * 
 ***************************************************************************/
#include "bstd.h"
#include "brsp.h"
#include "brsp_mbyte.h"
#include "brsp_priv.h"




BDBG_MODULE(brsp);

BERR_Code BRSP_ParseRegScript(
	BREG_Handle hReg,					/* [in] Handle to Register */
	const BRSP_ScriptTbl *pScriptTbl,	/* [in] Pointer to Register Script Tbl */
	void *phDev							/* [in] Handle of calling device */
	)
{
	BRSP_P_MByte_Handle hRsp;

	hRsp.hReg = hReg;
	hRsp.nbrShadowReg = 0;
	hRsp.coreOffset = 0;
	hRsp.magicId = BRSP_MAGIC_ID;
	hRsp.pShadowRegs = NULL;
	return( BRSP_MByte_ParseRegScript( &hRsp, pScriptTbl, phDev ) );
}





                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          













































































































































































































































































































