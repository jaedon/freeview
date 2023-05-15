/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: memmgr.c $
* $brcm_Revision: Hydra_Software_Devel/15 $
* $brcm_Date: 10/17/12 5:01p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/linuxuser/memmgr.c $
* 
* Hydra_Software_Devel/15   10/17/12 5:01p vanessah
* SW7445-54: add user mode memc2 support
* 
* Hydra_Software_Devel/14   9/24/12 4:40p pntruong
* SW7552-361: Appframework does not set BMEM alignment correctly for
* several chips.
* 
* Hydra_Software_Devel/13   10/28/11 2:35p mward
* SW7435-9: Add 7435.
* 
* Hydra_Software_Devel/12   4/25/11 11:47a tdo
* SW7425-315: Add rockford/appframework support for upper memory
* range(>256MB) of MEMC0
*
* Hydra_Software_Devel/11   11/8/10 3:11p tdo
* SW7422-12: Bringup 7422 appframework
*
* Hydra_Software_Devel/10   2/13/09 4:18p tdo
* PR50391: Correct default alignment setting for 7335 and 7336
*
* Hydra_Software_Devel/9   2/9/09 4:15p tdo
* PR50391: Correct default alignment setting for 7420
*
* Hydra_Software_Devel/8   7/25/08 2:59p syang
* PR 43729: print msg for both cached and non-cached virtual base
*
* Hydra_Software_Devel/7   6/11/08 5:53p rpan
* PR43473: Use pre-defined AV memory base address if
* /proc/bcmdriver/meminfo doesn't exist.
*
* Hydra_Software_Devel/6   6/10/08 12:42p rpan
* PR43473: Automatically detect AV memory base address.
*
* Hydra_Software_Devel/5   5/31/07 12:09p maivu
* PR 31066: For 7400B0+ and 7405, MIPS CPU has a cache line of 64 bytes,
* so we need to align it properly to prevent data corruption.
*
* Hydra_Software_Devel/4   3/31/07 5:01p shyam
* PR 29285 : Added linuxclient support for simulations/emulation
*
* Hydra_Software_Devel/3   2/5/07 4:36p syang
* PR 23354: add mmap for mem ctrl 1
*
* Hydra_Software_Devel/2   7/7/06 1:53p maivu
* PR 20495: Modified to support 7411E0 and future revisions
*
* Hydra_Software_Devel/1   6/16/06 4:09p maivu
* PR 22212: Move to linuxuser directory
*
* Hydra_Software_Devel/8   2/14/06 2:37p maivu
* PR 19599: Use BCHP_7411_VER
*
* Hydra_Software_Devel/7   2/13/06 1:57p maivu
* PR 19599: Fixed 97398 builts
*
* Hydra_Software_Devel/6   2/10/06 5:03p maivu
* PR 19599: Fixed errors with prevous check-in for non-97398 builts
*
* Hydra_Software_Devel/5   2/10/06 3:31p maivu
* PR 19599: Add 7411 support for 97398
*
* Hydra_Software_Devel/4   8/31/05 5:25p jasonh
* PR 16943: FPGA reg handle only needed for 7038 Bx.
*
* Hydra_Software_Devel/3   7/29/05 2:58p jasonh
* PR 16397: Changed kmem to mem.
*
* Hydra_Software_Devel/2   7/27/05 3:54p jasonh
* PR 16397: Removed QAM. General clean-up. Moved chip-specific defines.
*
* Hydra_Software_Devel/13   11/3/04 6:42p hongtaoz
* PR13076: added cached memory support;
*
* Hydra_Software_Devel/12   10/7/04 5:26p pntruong
* PR12728: Fixed -pdantic build warnings and errors.
*
* Hydra_Software_Devel/11   9/20/04 2:00p darnstein
* PR 10943: #include memmgr.h in order to silence compiler warnings.
*
* Hydra_Software_Devel/9   9/20/04 1:35p darnstein
* PR 10943: Add function to allow changing the heap starting address
* from 0x02000000 to anything else.  Use this for dual PCI board.
*
* Hydra_Software_Devel/8   2/25/04 11:37a hongtaoz
* PR9858: Need to increase heap size from 32 -> 96 megabytes to support
* multiple HD windows/surfaces.
*
* Hydra_Software_Devel/7   2/4/04 7:57a marcusk
* PR9588: Added support for fpga, tested and working.
*
* Hydra_Software_Devel/6   10/21/03 5:23p dlwin
* Changed location of 3250 to 0xD0000000.
*
* Hydra_Software_Devel/5   10/21/03 9:57a hongtaoz
* closed /dev/kmem in memmgr_Done.().
*
* Hydra_Software_Devel/4   10/2/03 8:57p dlwin
* Added initialization code for Bcm3250 chip, Bcm3250 Interrupts, and
* Upg Interrupt.
*
* Hydra_Software_Devel/3   9/30/03 10:38a aram
* stub for memmgr_Done
*
* Hydra_Software_Devel/2   9/29/03 12:04p aram
* integrated with int1
*
* Hydra_Software_Devel/1   9/15/03 10:50a aram
* genesis of these files
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
#include <stdlib.h>
#include <string.h>

#include "bstd.h"
#include "bchp_common.h"

#include "platform.h"
#include "systeminit.h"
#include "memmgr.h"
#include "bcm_driver.h"
#include <sys/ioctl.h>

/* device memory stuff */
static unsigned long  gp_memPBase;
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

/* 3rd memory stuff */
#ifdef MEMC_2_MEM_PBASE
static unsigned long  gp_memPBase2 = MEMC_2_MEM_PBASE;
static void          *gp_memVBase2 = 0;
static void          *gp_memCBase2 = 0;  /* cached space */
static unsigned long  gMemSize2    = 0;
#endif
#ifdef MEMC_0_MEM_UBASE
static unsigned long  gp_memPBaseU = MEMC_0_MEM_UBASE;
static void          *gp_memVBaseU = 0;
static void          *gp_memCBaseU = 0;  /* cached space */
static unsigned long  gMemSizeU    = 0;
#endif

/* register stuff */
static unsigned long  gp_regPBase = 0;
static void          *gp_regVBase = 0;
static unsigned long  gRegSize    = 0;

static unsigned long  gp_FpgaRegPBase = 0;
static void          *gp_FpgaRegVBase = 0;
static unsigned long  gFpgaRegSize    = 0;

#ifdef BCHP_7411_VER
static unsigned long  gp_reg7411PBase0 = 0;
static void          *gp_reg7411VBase0 = 0;
static unsigned long  g7411RegSize0    = 0;
static unsigned long  gp_reg7411PBase1 = 0;
static void          *gp_reg7411VBase1 = 0;
static unsigned long  g7411RegSize1    = 0;
#endif

/* kmem fd */
static int	fd_access = -1;
static int	fd_cached = -1;

/* local defines */
#define MAX_REG_ADDR   (gp_regPBase+gRegSize-1)

static BERR_Code convert_alignment(unsigned alignment, unsigned *pAlignmentExponent)
{
    unsigned i;
    if ( alignment == 0 )
    {
        /* Default alignment */
        i = 0;
    }
    else
    {
        for ( i = 0; i < (sizeof(size_t)*8); i++ )
        {
            if ( alignment == 1UL<<i )
            {
                break;
            }
        }

        if ( i >= sizeof(size_t)*8 )
        {
            fprintf(stderr, "Invalid memory alignment\n");
            return BERR_INVALID_PARAMETER;
        }
    }
    *pAlignmentExponent = i;
    return BERR_SUCCESS;
}

static int get_linux_memory_size(void)
{
    char buf[512];
    char *ptr, *end;
    int result = 0;
    FILE *f;

    f = fopen("/proc/bcmdriver/meminfo", "r");
    if (!f) {
        fprintf( stderr, "Can't access /proc/bcmdriver/meminfo: Linux errno %d\n", errno);
        fprintf( stderr, "Load bcmdriver.ko before start app framework!\n");
        fprintf( stderr, "Use pre-defined Linux mem size 0x%x\n", MEM_PBASE);

		return (MEM_PBASE);
    }

    if (!fgets(buf, 512, f)) goto error;
    ptr = &buf[13];     /* "13" should point us to the "space" after "Kernel Memory " see umdrv.c */
    ptr += strspn(ptr, " ");
    end = strchr(ptr, ' ');
    if (!end) goto error;
    *end = 0;
    result = atoi(ptr);
error:
    if (f) fclose(f);
    return result;
}

int  memmgr_Init(int argc, char *argv[], BSystem_Info *pInfo)
{
    int fdBcmdriver;
    unsigned dcacheLineSize=0;

	BSTD_UNUSED(argc);
	BSTD_UNUSED(argv);

	if (fd_access < 0)
	{
		/* detect size of memory allocated to Linux */
		gp_memPBase = (unsigned long) get_linux_memory_size();

		fprintf( stderr, "AV memory base address is 0x%x\n", (int) gp_memPBase);

		fprintf( stderr, "Mapping memory and registers...\n" );

		/* open the access device and mmap the whole address space. */
		fd_access = open("/dev/mem",O_RDWR+O_SYNC);/* O_SYNC is for non cached access. */
		if (fd_access < 0)
		{
			fprintf( stderr,"bcmKNIMmuMap(): error opening kmem device!!!\n");
			return 0;
		}

		/* cached access */
		fd_cached = open("/dev/mem",O_RDWR);
		if (fd_access < 0)
		{
			fprintf( stderr,"bcmKNIMmuMap(): error opening kmem device!!!\n");
			return 0;
		}

		/* calculate how much memory left for hardware use */
		gMemSize    = DRAM_SIZE - gp_memPBase;

		/* uncached mapping for main mem */
		gp_memVBase = mmap(0, gMemSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd_access, gp_memPBase);
		if(gp_memVBase == MAP_FAILED)
		{
			fprintf( stderr,"bcmKNIMmuMap(): non-cached space mmap failed\n");
			return -1;
		}

		/* cached mapping for main mem */
		gp_memCBase = mmap(0, gMemSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd_cached, gp_memPBase);
		if(gp_memCBase == MAP_FAILED)
		{
			fprintf( stderr,"bcmKNIMmuMap(): cached space mmap failed\n");
			return -1;
		}

#ifdef MEMC_0_MEM_UBASE
		gMemSizeU = MEMC_0_MEM_USIZE;

		/* uncached mapping for upper memc 0 */
		gp_memVBaseU = mmap(0, gMemSizeU, PROT_READ|PROT_WRITE, MAP_SHARED, fd_access, gp_memPBaseU);
		if(gp_memVBaseU == MAP_FAILED)
		{
			fprintf( stderr,"bcmKNIMmuMap(): E non-cached space mmap failed\n");
			return -1;
		}

		/* cached mapping for upper memc 0 */
		gp_memCBaseU = mmap(0, gMemSizeU, PROT_READ|PROT_WRITE, MAP_SHARED, fd_cached, gp_memPBaseU);
		if(gp_memCBaseU == MAP_FAILED)
		{
			fprintf( stderr,"bcmKNIMmuMap(): E cached space mmap failed\n");
			return -1;
		}
#endif

#ifdef MEMC_1_MEM_PBASE
		gMemSize1 = MEMC_1_DRAM_SIZE;

		/* uncached mapping for 2nd mem */
		gp_memVBase1 = mmap(0, gMemSize1, PROT_READ|PROT_WRITE, MAP_SHARED, fd_access, gp_memPBase1);
		if(gp_memVBase1 == MAP_FAILED)
		{
			fprintf( stderr,"bcmKNIMmuMap(): non-cached space mmap failed\n");
			return -1;
		}

		/* cached mapping for 2nd mem */
		gp_memCBase1 = mmap(0, gMemSize1, PROT_READ|PROT_WRITE, MAP_SHARED, fd_cached, gp_memPBase1);
		if(gp_memCBase1 == MAP_FAILED)
		{
			fprintf( stderr,"bcmKNIMmuMap(): cached space mmap failed\n");
			return -1;
		}
#endif

#ifdef MEMC_2_MEM_PBASE
		gMemSize2 = MEMC_2_DRAM_SIZE;

		/* uncached mapping for 3rd mem */
		gp_memVBase2 = mmap(0, gMemSize2, PROT_READ|PROT_WRITE, MAP_SHARED, fd_access, gp_memPBase2);
		if(gp_memVBase2 == MAP_FAILED)
		{
			fprintf( stderr,"bcmKNIMmuMap(): non-cached space mmap failed\n");
			return -1;
		}

		/* cached mapping for 2nd mem */
		gp_memCBase2 = mmap(0, gMemSize2, PROT_READ|PROT_WRITE, MAP_SHARED, fd_cached, gp_memPBase2);
		if(gp_memCBase2 == MAP_FAILED)
		{
			fprintf( stderr,"bcmKNIMmuMap(): cached space mmap failed\n");
			return -1;
		}
#endif
		/* chipset register mapping */
		gp_regPBase = REG_PBASE;
		gRegSize    = REG_SIZE;
		gp_regVBase = mmap(0, gRegSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd_access, gp_regPBase);
		if(gp_regVBase == MAP_FAILED)
		{
			fprintf( stderr,"bcmKNIMmuMap(): regspace mmap failed\n");
			return -1;
		}

#if (BCHP_CHIP==7038) && (defined(BCHP_REV_B0) || defined(BCHP_REV_B1) || defined(BCHP_REV_B2))
		/* FPGA register mapping */
		gp_FpgaRegPBase = REG_FPGA_PBASE;
		gFpgaRegSize    = REG_FPGA_SIZE;
		gp_FpgaRegVBase = mmap(0, gFpgaRegSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd_access, gp_FpgaRegPBase);
		if(gp_FpgaRegVBase == MAP_FAILED)
		{
			fprintf( stderr,"bcmKNIMmuMap(): Fpgaregspace mmap failed\n");
			return -1;
		}
#else
		gp_FpgaRegVBase = NULL;
		gFpgaRegSize = 0;
#endif

#ifdef BCHP_7411_VER
		gp_reg7411PBase0 = B_PHYS_7411A_BASE;
		g7411RegSize0    = 8192;
		gp_reg7411VBase0 = mmap(0, g7411RegSize0,
		                           PROT_READ|PROT_WRITE, MAP_SHARED, fd_access, gp_reg7411PBase0);
		if(gp_reg7411VBase0 == MAP_FAILED)
		{
			fprintf( stderr,"bcmKNIMmuMap(): 7411regspace0 mmap failed\n");
			return -1;
		}

		gp_reg7411PBase1 = B_PHYS_7411B_BASE;
		g7411RegSize1    = 8192;
		gp_reg7411VBase1 = mmap(0, g7411RegSize1,
		                           PROT_READ|PROT_WRITE, MAP_SHARED, fd_access, gp_reg7411PBase1);
		if(gp_reg7411VBase1 == MAP_FAILED)
		{
			fprintf( stderr,"bcmKNIMmuMap(): 7411regspace1 mmap failed\n");
			return -1;
		}
#endif

		fprintf( stderr, "gp_memPBase:     %08lX, gp_memVBase:  %p, gp_memCBase:  %p, gMemSize:  %08lX\n",  gp_memPBase, gp_memVBase, gp_memCBase, gMemSize );
#ifdef MEMC_0_MEM_UBASE
		fprintf( stderr, "gp_memPBaseU:    %08lX, gp_memVBaseU: %p, gp_memCBaseU: %p, gMemSizeU: %08lX\n",  gp_memPBaseU, gp_memVBaseU, gp_memCBaseU, gMemSizeU );
#endif
#ifdef MEMC_1_MEM_PBASE
		fprintf( stderr, "gp_memPBase1:    %08lX, gp_memVBase1: %p, gp_memCBase1: %p, gMemSize1: %08lX\n",  gp_memPBase1, gp_memVBase1, gp_memCBase1, gMemSize1 );
#endif
#ifdef MEMC_2_MEM_PBASE
		fprintf( stderr, "gp_memPBase2:    %08lX, gp_memVBase2: %p, gp_memCBase2: %p, gMemSize2: %08lX\n",	gp_memPBase2, gp_memVBase2, gp_memCBase2, gMemSize2 );
#endif
		fprintf( stderr, "gp_regPBase:     %08lX, gp_regVBase:  %p, gRegSize:  %08lX\n",  gp_regPBase, gp_regVBase, gRegSize );
		fprintf( stderr, "gp_FpgaRegPBase: %08lX, gp_FpgaRegVBase: %p, gFpgaRegSize: %08lX\n",  gp_FpgaRegPBase, gp_FpgaRegVBase, gFpgaRegSize );
	}

	pInfo->bmemAddress = gp_memVBase;
	pInfo->bmemCachedAddress = gp_memCBase;
	/* Follows nexus convention for determine cache line size */
    pInfo->bmemAlignment = 9;   /* Default */

    fdBcmdriver = open("/dev/brcm0", O_RDWR);
    if ( fdBcmdriver >= 0 )
    {
        int rc = ioctl(fdBcmdriver, BRCM_IOCTL_GET_DCACHE_LINE_SIZE, &dcacheLineSize);
        if ( 0 == rc )
        {
            convert_alignment(dcacheLineSize, &pInfo->bmemAlignment);
        }
        close(fdBcmdriver);
    }

	pInfo->bmemOffset = (size_t) gp_memPBase;
	pInfo->bmemSize = gMemSize;

#ifdef MEMC_0_MEM_UBASE
	pInfo->bmemAddressU = gp_memVBaseU;
	pInfo->bmemCachedAddressU = gp_memCBaseU;
	pInfo->bmemAlignmentU = 0;
	pInfo->bmemOffsetU = (size_t) gp_memPBaseU;
	pInfo->bmemSizeU = gMemSizeU;
#endif

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
#ifdef MEMC_0_MEM_UBASE
	fprintf( stderr, "Mem_E:  Base:size %08lX : %08lX\n", gp_memPBaseU, gMemSizeU );
#endif
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
	if( fd_access >= 0)
	{
		close( fd_access );
		fd_access = -1;
	}
	if( fd_cached >= 0)
	{
		close( fd_cached );
		fd_cached = -1;
	}
	return 0;
}

