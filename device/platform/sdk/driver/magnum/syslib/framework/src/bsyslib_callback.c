/***************************************************************************
*     Copyright (c) 2004-2008, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsyslib_callback.c $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 4/3/08 2:33p $
*
* Revision History:
*
* $brcm_Log: /magnum/syslib/framework/noarch/bsyslib_callback.c $
* 
* Hydra_Software_Devel/2   4/3/08 2:33p bandrews
* PR40090: synclib needs to have different contexts for syslib callbacks
* and synclib callbacks
***************************************************************************/

#include "bstd.h"
#include "bsyslib_callback.h"

BDBG_MODULE(syslib_callback);

void BSYSlib_Callback_Init(
	BSYSlib_Callback * psCallback
)
{
	BDBG_ENTER(BSYSlib_Callback_Init);

	BDBG_ASSERT(psCallback);

	psCallback->pfDo = NULL;
	psCallback->pvParm1 = NULL;
	psCallback->iParm2 = 0;

	BDBG_LEAVE(BSYSlib_Callback_Init);
}

BERR_Code BSYSlib_Callback_Invoke(
	BSYSlib_Callback * psCallback
)
{
	BERR_Code rc = BERR_SUCCESS;
	
	BDBG_ENTER(BSYSlib_Callback_Invoke);

	BDBG_ASSERT(psCallback);

	if (psCallback->pfDo)
	{
		rc = psCallback->pfDo(psCallback->pvParm1, psCallback->iParm2);
	}

	BDBG_LEAVE(BSYSlib_Callback_Invoke);
	return rc;
}

