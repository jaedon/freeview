/***************************************************************************
*     Copyright (c) 2003-2007, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: framework_board.h $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 4/2/07 1:57p $
*
* Module Description: Framework initialization header file
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/board/sims/framework_board.h $
* 
* Hydra_Software_Devel/1   4/2/07 1:57p shyam
* PR 29285 : Added linuxclient support for simulations/emulation
* 
* Hydra_Software_Devel/4   2/5/07 4:18p hongtaoz
* PR27621: bring up PCMCIA streamer card for mvd_vdc_test;
* 
* Hydra_Software_Devel/3   1/16/07 11:27a hongtaoz
* PR23260, PR23280: 3563 bringup added LCD panel ctrl;
* 
* Hydra_Software_Devel/2   1/13/07 5:10p yuxiaz
* PR22572: Added second ddr memory controller in appframework.
* 
* Hydra_Software_Devel/1   8/7/06 2:12p pntruong
* PR22572: Need initial 93563 appframework bringup.
* 
***************************************************************************/
#ifndef FRMWORKBOARD_H__
#define FRMWORKBOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bstd.h"
#include "bmem.h"
#include "systeminit.h"
#include "framework.h"

typedef struct BFrmWorkBoard_Info
{
	int reserved ;
} BFrmWorkBoard_Info;

int BFrmWorkBoard_Init( const BSystem_Info *pSysInfo, BFramework_Info *pFrameworkInfo ) ;
void BFrmWorkBoard_Uninit( BFramework_Info *pFrameworkInfo ) ;

#ifdef __cplusplus
}
#endif

#endif

/* End of file */
