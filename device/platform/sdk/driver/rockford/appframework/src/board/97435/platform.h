/***************************************************************************
*     Copyright (c) 2003-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: platform.h $
* $brcm_Revision: Hydra_Software_Devel/2 $
* $brcm_Date: 3/6/12 10:24a $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/appframework/src/board/97435/platform.h $
* 
* Hydra_Software_Devel/2   3/6/12 10:24a vanessah
* SW7435-9: increase upper memory size
* 
* Hydra_Software_Devel/1   10/26/11 2:08p mward
* SW7435-9: Add 97435 appframework board.  Initial platform.h adapted
* from 7425 version.
* 
* Hydra_Software_Devel/4   4/25/11 11:47a tdo
* SW7425-315: Add rockford/appframework support for upper memory
* range(>256MB) of MEMC0
*
***************************************************************************/

#ifndef PLATFORM_H
#define PLATFORM_H

#include "bchp_common.h"
#include "bchp_hif_cpu_intr1.h"
#include "bchp_7435.h"
#include "bint_7435.h"


/**************************************************************************
 * NOTE: Other platform specific code is found in framework.c in the
 * initialization of the FPGA
 **************************************************************************/


/**************************************************************************
 * Chip specific functions
 **************************************************************************/
#define BCHP_OPEN                  BCHP_Open7435
#define BINT_GET_SETTINGS   BINT_7435_GetSettings


/**************************************************************************
 * MIPS Physical Memory Map
 **************************************************************************/

#ifdef EMULATION
/* 64-bit DDR space */
#define MEM_PBASE            0x00000000	 /* 256 Megs for O/S */
#define DRAM_SIZE            (256*1024*1024)
#else
/* 64-bit DDR space */
#define MEM_PBASE            0x04000000   /*  0 -  64 Meg for O/S */
                                           /* 64 - 256 Meg for BMEM */
#define DRAM_SIZE            (256*1024*1024)
#endif

#define MEMC_0_MEM_UBASE     0x20000000
#define MEMC_0_MEM_USIZE     0x0c000000

/* 16-bit DDR space */
#define MEMC_1_MEM_PBASE     0x90000000
#define MEMC_1_DRAM_SIZE     (256*1024*1024)

/**************************************************************************
 * Register Mappings
 **************************************************************************/
#define REG_PBASE         BCHP_PHYSICAL_OFFSET
#define REG_SIZE          BCHP_REGISTER_END


/**************************************************************************
 * KSEG Mapping Definitions and Macro's
 **************************************************************************/
#define BCM_K1BASE                0xa0000000
#define BCM_PHYS_TO_K1(x)   ((x) | BCM_K1BASE)


/**************************************************************************
 * BCM7038 Interrupt Level 1 Base Address
 **************************************************************************/
#define CPUINT1C_ADR_BASE  \
	BCM_PHYS_TO_K1(BCHP_PHYSICAL_OFFSET+BCHP_HIF_CPU_INTR1_INTR_W0_STATUS)


#endif /* PLATFORM_H */

/* End of file */

