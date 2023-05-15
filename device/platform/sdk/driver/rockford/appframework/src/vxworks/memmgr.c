/***************************************************************************
*     Copyright (c) 2003-2006, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: memmgr.c $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 6/20/06 12:44p $
*
* Module Description:
*	Memory management for Vxworks
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/vxworks/memmgr.c $
* 
* Hydra_Software_Devel/4   6/20/06 12:44p maivu
* PR 22212: Cleanup
* 
* Hydra_Software_Devel/3   4/11/06 1:34p jessem
* PR18367: Reorganized functions in int1.c and intmgr.c to use VxWorks
* bsp functions. Modified memmgr.c to work under VxWorks.
* 
* Hydra_Software_Devel/1   3/10/06 12:47p maivu
* PR 18367: Initial Vxworks support
* 
***************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#include "bstd.h"
#include "bchp_common.h"

#include "platform.h"
#include "systeminit.h"
#include "memmgr.h"

/* device memory stuff */
static unsigned long  gp_memPBase = 20*1024*1024;

static void          *gp_FpgaRegVBase = 0;
static unsigned long  gFpgaRegSize    = 0;

#if (BCHP_7411_VER >= BCHP_VER_C0)
static void          *gp_reg7411VBase0 = 0;
static unsigned long  g7411RegSize0    = 0;
static void          *gp_reg7411VBase1 = 0;
static unsigned long  g7411RegSize1    = 0;
#endif

/* for non-pci mapping */
#define B_SYS_MAP(x)   ((x) | 0xa0000000)
#define B_SYS_MAP_CACHE(x) ((x) | 0x80000000ul)


int  memmgr_Init(BSystem_Info *pInfo)
{

#if (BCHP_CHIP==7038) && (defined(BCHP_REV_B0) || defined(BCHP_REV_B1) || defined(BCHP_REV_B2))
	/* FPGA register mapping */
	gFpgaRegSize    = REG_FPGA_SIZE;
	gp_FpgaRegVBase = B_SYS_MAP(REG_FPGA_PBASE);
#else
	gFpgaRegSize = 0;
	gp_FpgaRegVBase = NULL;
#endif	

#if (BCHP_7411_VER >= BCHP_VER_C0)
	g7411RegSize0    = 8192;
	gp_reg7411VBase0 = B_SYS_MAP(B_PHYS_7411A_BASE);

	g7411RegSize1    = 8192;
	gp_reg7411VBase1 = B_SYS_MAP(B_PHYS_7411B_BASE);
#endif

	pInfo->bmemAddress = B_SYS_MAP(gp_memPBase);
	pInfo->bmemCachedAddress = B_SYS_MAP_CACHE(gp_memPBase);
	pInfo->bmemAlignment = 0;

	/* For Vxworks, 20M for OS, 44M for heap */
	pInfo->bmemOffset = (size_t) gp_memPBase;
	pInfo->bmemSize = 44*1024*1024;

	pInfo->bregBaseAddr = B_SYS_MAP(REG_PBASE);
	pInfo->bregMaxRegOffset = REG_SIZE;

	pInfo->bregFpgaBaseAddr = gp_FpgaRegVBase;
	pInfo->bregFpgaMaxRegOffset = gFpgaRegSize;

#if (BCHP_7411_VER >= BCHP_VER_C0)
	pInfo->breg7411BaseAddr[0] = gp_reg7411VBase0;
	pInfo->breg7411MaxRegOffset[0] = g7411RegSize0;
	pInfo->breg7411BaseAddr[1] = gp_reg7411VBase1;
	pInfo->breg7411MaxRegOffset[1] = g7411RegSize1;
#endif

	fprintf( stderr, "Mem     Base:size %08lX : %08lX\n", gp_memPBase, pInfo->bmemSize );
	fprintf( stderr, "Reg     Base:size %08lX : %08lX\n", REG_PBASE, pInfo->bregMaxRegOffset );
		
	return 0;
}


int
memmgr_Done()
{
	return 0;
}

