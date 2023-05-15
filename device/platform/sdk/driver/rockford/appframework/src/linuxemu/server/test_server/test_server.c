/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: test_server.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/28/09 2:47p $
 *
 * Module Description:
 *   The source code for the stb server.
 *
 * Revision History:
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
#include "bchp_common.h"
#include "bchp_sun_top_ctrl.h"

#include "berr.h"
#include "bstd_defs.h"
#include "bdbg.h"
#include "bkni.h"

#include "server.h"
#include "test_server.h"

#ifdef BCU_TBX_MODE
#include <testbench.h>
#include <signal.h>
#include <dvt_cmd_api.h>
#include <util.h>
#include <timer.h>
#include "events.h"
#include <simthread.h>
#include "io.h"
#include "io_port.h"

#define USE_RBUS
#define EMU_CMD_API_VERBOSE
#define DDR_64_SIZE_MB 32
#define DDR_32_SIZE_MB 16
#define DDR_SRAM_EARLY_DATA_MODE
#define POLL_STATUS_PERIOD 1000000



#define pout(s) do { fprintf( stderr, \
	"%s line %d errno %d:" s "\n", __FILE__, __LINE__, errno ); \
} while( 0 )

/*============= Modules List ===============================
	add/remove the modules according to emulator's capacity;
	if the module is not compiled in the emulator, GISB access
	timeout might terminate the server.
 */
#define BEMU_IS_VALID_ADDR( addr ) ( \
	/* CLKGEN */\
	(( addr >= 0x10462000 ) && ( addr < 0x10462fff )) ||\
	/* GFX */\
	(( addr >= 0x10480000 ) && ( addr < 0x10485000 )) ||\
	/* AVD 0 */\
	(( addr >= 0x10000000 ) && ( addr < 0x10100fff )) ||\
	/* AVD 1 */\
	(( addr >= 0x10600000 ) && ( addr < 0x10700fff )) ||\
	/* XPT */\
	(( addr >= 0x10740000 ) && ( addr < 0x1077ffff )) ||\
	/* BSP */\
	(( addr >= 0x10320000 ) && ( addr < 0x1032ffff )) ||\
	/* SECTOP */\
	(( addr >= 0x10360000 ) && ( addr < 0x10360fff )) ||\
	/* SECTOP SECURE */\
	(( addr >= 0x10364000 ) && ( addr < 0x10366fff )) ||\
	/* AUD ALL */\
	(( addr >= 0x10380000 ) && ( addr < 0x1039ffff )) ||\
	/* AUD SECURE */\
	(( addr >= 0x103a0000 ) && ( addr < 0x103a0fff )) ||\
	/* MEMC_0 ALL */\
	(( addr >= 0x103b0000 ) && ( addr < 0x103bb000 )) ||\
	/* MEMC_1 ALL */\
	(( addr >= 0x103c0000 ) && ( addr < 0x103cb000 )) ||\
	/* BVN_TOP */\
	(( addr >= 0x10780000 ) && ( addr < 0x107affff )) ||\
	/* SUNDRY */\
	(( addr >= 0x10400000 ) && ( addr < 0x1040cfff )) \
)

#if 0
/* MiniTitanPlus1 */\
(( addr >= 0x10300000 ) && ( addr < 0x10360bff )) ||\
/* MiniTitanPlus2 */\
(( addr >= 0x10380000 ) && ( addr < 0x103e0bff )) ||\
/* Xpt */\
(( addr >= 0x10200000 ) && ( addr < 0x1027ffff )) ||

#endif

#else

#define BEMU_IS_VALID_ADDR( addr ) 1

#endif

static BEMU_Settings s_stSettings = 
{
	sizeof(BEMU_Settings)
};

BSystem_Info g_stSysInfo;
bool         g_bFail;

BDBG_MODULE(STB_SERVER);


BERR_Code BEMU_Open(BEMU_Handle *phEmu, const struct BEMU_Settings *pstSettings)
{
	BERR_Code err = BERR_SUCCESS;
	BEMU_P_Context *pEmu;

	/* TODO: pass through settings to emulator */
	BSTD_UNUSED(pstSettings);
	
	BDBG_MSG(("Initialize ikos_server."));

	pEmu = (BEMU_P_Context *)BKNI_Malloc(sizeof(BEMU_P_Context));
	if (pEmu == NULL)
	{
		BDBG_ERR(("Out of memory"));
		return BERR_OUT_OF_SYSTEM_MEMORY;
	}

	pEmu->ulBlackMagic = sizeof(BEMU_P_Context);

	BDBG_MSG(("Initialize regscope_server."));
	memmgr_Init(0, NULL, &g_stSysInfo);

	*phEmu = (BEMU_Handle)pEmu;

	return err;
}

BERR_Code BEMU_Close(BEMU_Handle hEmu)
{
	BKNI_Free(hEmu);

	memmgr_Done();

	return BERR_SUCCESS;	
}

void BEMU_GetSettings(struct BEMU_Settings *pSettings)
{
	*pSettings = s_stSettings;
}

BERR_Code BEMU_RegisterRead(BEMU_Handle hEmu, uint32_t ulAddr, uint32_t *pulData)
{
	BERR_Code err = BERR_SUCCESS;	
	BEMU_P_Context *pEmu;

	BEMU_P_GET_CONTEXT(hEmu, pEmu);
	
	if (BEMU_IS_VALID_ADDR(ulAddr))
	{
		/* regscope client somehow tries to guess chip ID twice.
		   Here is workaround. */
		static int iChipIdRead = 2;
		
		BDBG_MSG(("STB_REG_READ address: 0x%08X", ulAddr));		

		if(iChipIdRead)
		{
			if(ulAddr - BCHP_PHYSICAL_OFFSET != BCHP_SUN_TOP_CTRL_PROD_REVISION)
			{
				return 0;
			} else
			{
				iChipIdRead--;
			}
		}
		
		*pulData = *((volatile unsigned long *)((uint32_t)g_stSysInfo.bregBaseAddr + ulAddr - BCHP_PHYSICAL_OFFSET));
	}
	else
	{
		BDBG_MSG(("STB_REG_READ addr 0x%08x out of range!\n", ulAddr));
		err = BERR_INVALID_PARAMETER;
	}	
	return err;
}

BERR_Code BEMU_RegisterWrite(BEMU_Handle hEmu, uint32_t ulAddr, uint32_t ulData)
{
	BERR_Code err = BERR_SUCCESS;	
	BEMU_P_Context *pEmu;

	BEMU_P_GET_CONTEXT(hEmu, pEmu);
	
	if (BEMU_IS_VALID_ADDR( ulAddr ))
	{
		BDBG_MSG(("STB_REG_WRITE address: 0x%08X data 0x%08X", ulAddr, ulData));
		*((volatile unsigned long *)((unsigned int)g_stSysInfo.bregBaseAddr + ulAddr - BCHP_PHYSICAL_OFFSET)) = ulData;
	} 
	else
	{
		BDBG_MSG(("STB_REG_WRITE addr 0x%08x out of range!", ulAddr ));
		err = BERR_INVALID_PARAMETER;
	}
	
	return err;
}

BERR_Code BEMU_MemoryRead(BEMU_Handle hEmu, uint32_t ulAddr, uint32_t ulSize, uint8_t *pucData)
{
	BERR_Code err = BERR_SUCCESS;	
	BEMU_P_Context *pEmu;
	uint32_t i;
	uint8_t *pAddr;

	BDBG_ASSERT(pucData);
	BEMU_P_GET_CONTEXT(hEmu, pEmu);
	
	/* NOTE: we are going to take the absolute physical address from now upon. */
	pAddr = (uint8_t*)((uint32_t)g_stSysInfo.bmemAddress + ulAddr - g_stSysInfo.bmemOffset);

	/* don't probe the memory region beyond the mapped ranged! */
	if(ulAddr < g_stSysInfo.bmemOffset)
	{
		BDBG_MSG(("!!! regscope_server MemoryRead address 0x%x is less than base offset 0x%x !!!",
			ulAddr, g_stSysInfo.bmemOffset));
		g_bFail = true;
		return err;
	}
	else if(ulAddr + ulSize > g_stSysInfo.bmemOffset + g_stSysInfo.bmemSize)
	{
		BDBG_MSG(("!!! regscope_server MemoryRead addr 0x%x is greater than mapped range 0x%x !!!",
			ulAddr + ulSize, g_stSysInfo.bmemOffset + g_stSysInfo.bmemSize));
		g_bFail = true;
		return err;
	}

	for(i = 0; i < ulSize; i += sizeof(uint32_t))
	{
		/* note: linux might run at little or big endian */
#if (BSTD_CPU_ENDIAN==BSTD_ENDIAN_LITTLE)
		*(pucData + i)     = *(pAddr + i + 3);
		*(pucData + i + 1) = *(pAddr + i + 2);
		*(pucData + i + 2) = *(pAddr + i + 1);
		*(pucData + i + 3) = *(pAddr + i);
#elif (BSTD_CPU_ENDIAN==BSTD_ENDIAN_BIG)
		*(pucData + i)     = *(pAddr + i);
		*(pucData + i + 1) = *(pAddr + i + 1);
		*(pucData + i + 2) = *(pAddr + i + 2);
		*(pucData + i + 3) = *(pAddr + i + 3);
#else
	#error "Please specify the CPU endianess!"
#endif
	}
	return err;
}

BERR_Code BEMU_MemoryWrite(BEMU_Handle hEmu, uint32_t ulAddr, uint32_t ulSize, uint8_t *pucData)
{
	BERR_Code err = BERR_SUCCESS;
	BEMU_P_Context *pEmu;
	uint32_t i;
	uint8_t *pAddr;

	BEMU_P_GET_CONTEXT(hEmu, pEmu);

	/* for regscope_server, command 'mw addr data size' is writing the same 32-bit data to size of continuous bytes
	   pointed by ulAddr */
	/* NOTE: we are going to take the absolute physical address from now upon. */
	pAddr = (uint8_t*)((uint32_t)g_stSysInfo.bmemAddress + ulAddr - g_stSysInfo.bmemOffset);

	/* don't probe the memory region beyond the mapped ranged! */
	if(ulAddr < g_stSysInfo.bmemOffset)
	{
		BDBG_MSG(("!!! regscope_server MemoryWrite address 0x%x is less than base offset 0x%x !!!",
			ulAddr, g_stSysInfo.bmemOffset));
		g_bFail = true;
		return err;
	}
	else if(ulAddr + ulSize > g_stSysInfo.bmemOffset + g_stSysInfo.bmemSize)
	{
		BDBG_MSG(("!!! regscope_server MemoryWrite addr 0x%x is greater than mapped range 0x%x !!!",
			ulAddr + ulSize, g_stSysInfo.bmemOffset + g_stSysInfo.bmemSize));
		g_bFail = true;
		return err;
	}

	for(i = 0; i < ulSize; i += sizeof(uint32_t))
	{
		/* note: linux is running at little endian, network is big endian. */
#if (BSTD_CPU_ENDIAN==BSTD_ENDIAN_LITTLE)
		*(pAddr + i + 3) = *(pucData);
		*(pAddr + i + 2) = *(pucData + 1);
		*(pAddr + i + 1) = *(pucData + 2);
		*(pAddr + i)     = *(pucData + 3);
#elif (BSTD_CPU_ENDIAN==BSTD_ENDIAN_BIG)
		*(pAddr + i)     = *(pucData);
		*(pAddr + i + 1) = *(pucData + 1);
		*(pAddr + i + 2) = *(pucData + 2);
		*(pAddr + i + 3) = *(pucData + 3);
#else
	#error "Please specify the CPU endianess!"
#endif
	}
	return err;
}

BERR_Code BEMU_Command(BEMU_Handle hEmu, uint32_t ulCommand, int iSize, int32_t * pilData)
{
	BERR_Code err = BERR_SUCCESS;
	BEMU_P_Context *pEmu;
	int i;

	BEMU_P_GET_CONTEXT(hEmu, pEmu);

	BDBG_WRN(("Dummy BEMU_Command "));

	printf("ulCommand = %d\n", ulCommand);
	printf("iSize = %d\n", iSize);

	for (i=0; i<iSize; i++)
	{
		printf("ilData[%d] = 0x%x\n", i, pilData[i]);
	}
	return err;
}


/* end of file */

