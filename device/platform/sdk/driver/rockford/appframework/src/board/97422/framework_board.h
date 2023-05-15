/***************************************************************************
*     Copyright (c) 2003-2011, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: framework_board.h $
*
* Module Description: Framework initialization header file
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/board/97422/framework_board.h $
* 
* Hydra_Software_Devel/2   4/25/11 11:47a tdo
* SW7425-315: Add rockford/appframework support for upper memory
* range(>256MB) of MEMC0
*
* Hydra_Software_Devel/1   6/22/10 4:00p vanessah
* SW7422-12:  add source control framework_board.c
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

#include "btst_kni.h"

#if (FRAMEWORK_DO_I2C == 1)
#include "bi2c.h"
#endif

#if (FRAMEWORK_DO_FPGA == 1)
#include "bfpga.h"
#endif

typedef struct BFrmWorkBoard_Info
{
#ifdef EMULATION
	BTST_TaskHandle 	hIntMonitorTask;
#else

#if (FRAMEWORK_DO_FPGA == 1)
	BREG_Handle	        hFpgaReg;
	BFPGA_Handle	    hFpga;
#endif

#if (FRAMEWORK_DO_I2C == 1)
	BI2C_Handle         hI2c;
	unsigned int        uiNumI2c;
	BI2C_ChannelHandle  ahI2cChannels[BI2C_MAX_I2C_CHANNELS];
	BREG_I2C_Handle     ahI2cRegHandles[BI2C_MAX_I2C_CHANNELS];
#endif

#endif /* EMULATION */

	BMEM_Heap_Handle    hMemcU;         /* upper heap 0, old  BMEM_Handle */
	BMEM_Heap_Handle    hMemc1;         /* extra heap 1, old  BMEM_Handle */
} BFrmWorkBoard_Info;

int BFrmWorkBoard_Init( const BSystem_Info *pSysInfo, BFramework_Info *pFrameworkInfo );
void BFrmWorkBoard_Uninit( BFramework_Info *pFrameworkInfo );

#ifdef __cplusplus
}
#endif

#endif

