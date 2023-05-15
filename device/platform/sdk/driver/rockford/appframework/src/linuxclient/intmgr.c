/***************************************************************************
*     Copyright (c) 2003-2007, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: intmgr.c $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 3/31/07 11:03a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/linuxclient/intmgr.c $
* 
* Hydra_Software_Devel/1   3/31/07 11:03a shyam
* PR 29285 : Added linuxclient support for simulations/emulation
* 
* Hydra_Software_Devel/1   9/11/03 5:57p aram
* genesis of these files
***************************************************************************/
 
#include "bstd.h"
#include "bint_plat.h"
#include "intmgr.h"

int intmgr_Init( void ) { return 0;}  
void intmgr_Uninit(void) {}

int intmgr_DebugLevelSet(int level) { BSTD_UNUSED(level) ; return 0 ;} 

void intmgr_EnableIsrs( BINT_Handle intHandle ) { BSTD_UNUSED(intHandle) ;}

