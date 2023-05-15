/***************************************************************************
*     Copyright (c) 2003-2009, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: memmgr.c $
* $brcm_Revision: Hydra_Software_Devel/3 $
* $brcm_Date: 4/28/09 3:17p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/linuxclient/memmgr.c $
* 
* Hydra_Software_Devel/3   4/28/09 3:17p jessem
* PR 54380: Changed all references to IKOS to EMULATION, EMU or BEMU.
* 
* Hydra_Software_Devel/2   10/16/07 2:54p jessem
* PR 36129: Changed calls to BCHIPIF_xxx to IKOS_Client_xxx, Removed DRAM
* and REG size defines and used corresponding defines in platform.h
* instead.
* 
* Hydra_Software_Devel/1   3/31/07 11:03a shyam
* PR 29285 : Added linuxclient support for simulations/emulation
* 
***************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>

#include "systeminit.h"
#include "memmgr.h"

#ifdef EMULATION
#include "pagelib.h"
#include "client.h"
#endif
#include "platform.h"

/* device memory stuff */
static unsigned long  gp_memPBase = MEM_PBASE;
static void          *gp_memVBase = 0;
static void          *gp_memCBase = 0;  /* cached space */
static unsigned long  gMemSize    = 0;

/* 2nd memory stuff */
#ifdef MEMC_1_MEM_PBASE	
static unsigned long  gp_memPBase1 = MEMC_1_MEM_PBASE;
static void          *gp_memVBase1 = 0;
static void          *gp_memCBase1 = 0;  /* cached space */
static unsigned long  gMemSize1    = 0;
#endif

/* register stuff */
static unsigned long  gp_regPBase = 0;
static void          *gp_regVBase = 0;
static unsigned long  gRegSize    = REG_SIZE;

static unsigned long  gp_FpgaRegPBase = 0;
static void          *gp_FpgaRegVBase = 0;
static unsigned long  gFpgaRegSize    = 0;

static int initdone  = 0 ;

int memmgr_Init(int argc, char *argv[], BSystem_Info *pInfo)
{
	BSTD_UNUSED(argc);
	BSTD_UNUSED(argv);	
	
	if (!initdone)
	{
		fprintf( stderr, "Mapping memory and registers...\n" );

		/* calculate how much memory left for hardware use */
		gMemSize    = DRAM_SIZE - gp_memPBase;
		
		/* Mem access init */
		if( BEMU_Client_PageInit(DRAM_SIZE, &gp_memVBase, gp_memPBase) )
			return -1 ;

		/* No FPGA either */

		fprintf( stderr, "gp_memPBase:     %08lX, gp_memVBase:  %p, gMemSize:  %08lX\n",  gp_memPBase, gp_memVBase, gMemSize );
#ifdef MEMC_1_MEM_PBASE
		fprintf( stderr, "gp_memPBase1:    %08lX, gp_memCBase1: %p, gMemSize1: %08lX\n",  gp_memPBase1, gp_memCBase1, gMemSize1 );
#endif		
		fprintf( stderr, "gp_regPBase:     %08lX, gp_regVBase:  %p, gRegSize:  %08lX\n",  gp_regPBase, gp_regVBase, gRegSize );
		fprintf( stderr, "gp_FpgaRegPBase: %08lX, gp_FpgaRegVBase: %p, gFpgaRegSize: %08lX\n",  gp_FpgaRegPBase, gp_FpgaRegVBase, gFpgaRegSize );
		initdone = 1 ;
	}

	pInfo->bmemAddress = gp_memVBase;
	pInfo->bmemCachedAddress = gp_memCBase;
	pInfo->bmemAlignment = 0;
	pInfo->bmemOffset = (size_t) gp_memPBase;
	pInfo->bmemSize = gMemSize;

#ifdef MEMC_1_MEM_PBASE	
	pInfo->bmemAddress1 = gp_memVBase1;
	pInfo->bmemCachedAddress1 = gp_memCBase1;
	pInfo->bmemAlignment1 = 0;
	pInfo->bmemOffset1 = (size_t) gp_memPBase1;
	pInfo->bmemSize1 = gMemSize1;
#endif
	
	pInfo->bregBaseAddr = gp_regVBase;
	pInfo->bregMaxRegOffset = gRegSize;

	pInfo->bregFpgaBaseAddr = gp_FpgaRegVBase;
	pInfo->bregFpgaMaxRegOffset = gFpgaRegSize;

#ifdef BCHP_7411_VER
	pInfo->breg7411BaseAddr[0] = gp_reg7411VBase0;
	pInfo->breg7411MaxRegOffset[0] = g7411RegSize0;
	pInfo->breg7411BaseAddr[1] = gp_reg7411VBase1;
	pInfo->breg7411MaxRegOffset[1] = g7411RegSize1;
#endif

	fprintf( stderr, "Mem     Base:size %08lX : %08lX\n", gp_memPBase, gMemSize );
#ifdef MEMC_1_MEM_PBASE
	fprintf( stderr, "Mem_1   Base:size %08lX : %08lX\n", gp_memPBase1, gMemSize1 );	
#endif	
	fprintf( stderr, "Reg     Base:size %08lX : %08lX\n", gp_regPBase, gRegSize );
	fprintf( stderr, "FpgaReg Base:size %08lX : %08lX\n", gp_FpgaRegPBase, gFpgaRegSize );

	return 0;
}


int
memmgr_Done()
{
	if(initdone)
	{
		BEMU_Client_PageUnInit();
	}
	
	return 0;
}
