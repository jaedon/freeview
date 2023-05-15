/***************************************************************************
*     Copyright (c) 2003-2007, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: framework_board.c $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 4/2/07 1:57p $
*
* Module Description: Framework initialization header file
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/board/sims/framework_board.c $
* 
* Hydra_Software_Devel/1   4/2/07 1:57p shyam
* PR 29285 : Added linuxclient support for simulations/emulation
* 
***************************************************************************/

#include "framework_board.h"

int BFrmWorkBoard_Init( const BSystem_Info *pSysInfo, BFramework_Info *pFrameworkInfo )
{
	BSTD_UNUSED(pSysInfo) ;
	BSTD_UNUSED(pFrameworkInfo) ;
	return 0;
}
	
void BFrmWorkBoard_Uninit( BFramework_Info *pFrameworkInfo )
{
	BSTD_UNUSED(pFrameworkInfo) ;
}

/* End of file */
