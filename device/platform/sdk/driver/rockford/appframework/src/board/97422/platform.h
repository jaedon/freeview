/***************************************************************************
*     Copyright (c) 2003-2010, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
***************************************************************************/

#ifndef PLATFORM_H
#define PLATFORM_H

#include "bchp_common.h"
#include "bchp_hif_cpu_intr1.h"
#include "bchp_7422.h"
#include "bint_7422.h"


/**************************************************************************
 * NOTE: Other platform specific code is found in framework.c in the
 * initialization of the FPGA
 **************************************************************************/


/**************************************************************************
 * Chip specific functions
 **************************************************************************/
#define BCHP_OPEN                  BCHP_Open7422
#define BINT_GET_SETTINGS   BINT_7422_GetSettings


/**************************************************************************
 * MIPS Physical Memory Map
 **************************************************************************/

#ifdef EMULATION
/* 64-bit DDR space */
#define MEM_PBASE            0x00000000	 /* 128 Megs for O/S */
#define DRAM_SIZE            (128*1024*1024)
#else
/* 64-bit DDR space */
#define MEM_PBASE            0x04000000   /*  0 -  62 Meg for O/S */
                                          /* 62 -  64 Meg for XVD firmware */
                                          /* 64 - 256 Meg for MEM */
#define DRAM_SIZE            (256*1024*1024)
#endif

/* 16-bit DDR space */
#define MEMC_1_MEM_PBASE     0x90000000ul   /*  0 -  64 MB for AVD_0 private frame stores */
#define MEMC_1_DRAM_SIZE     (256*1024*1024)

/**************************************************************************
 * Register Mappings
 **************************************************************************/
#define REG_PBASE       BCHP_PHYSICAL_OFFSET
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

