/***************************************************************************
*     Copyright (c) 2003-2007, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: intmgr.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 3/31/07 11:03a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/linuxclient/intmgr.h $
* 
* Hydra_Software_Devel/1   3/31/07 11:03a shyam
* PR 29285 : Added linuxclient support for simulations/emulation
* 
* Hydra_Software_Devel/2   7/7/06 1:54p maivu
* PR 20495: Modified to support 7411E0 and future revisions
* 
* Hydra_Software_Devel/1   6/16/06 4:10p maivu
* PR 22212: Move to linuxuser directory
* 
* Hydra_Software_Devel/4   3/10/06 10:52a maivu
* PR 19599: Fixed previous check-in built error with non-97398 platforms
* 
* Hydra_Software_Devel/3   3/9/06 5:00p maivu
* PR 19599: Move OS dependent code to prepare for Vxworks support
* 
* Hydra_Software_Devel/2   7/27/05 3:52p jasonh
* PR 16397: General clean-up and added new function to register all L1
* Isrs
* 
* Hydra_Software_Devel/1   9/11/03 5:57p aram
* genesis of these files
***************************************************************************/
 
#ifndef INTMGR_H
#define INTMGR_H

#include "bint_plat.h"

int intmgr_Init( void ) ;
void intmgr_Uninit(void) ;

int intmgr_DebugLevelSet(int level) ;

void intmgr_EnableIsrs( BINT_Handle intHandle ) ;

#endif

