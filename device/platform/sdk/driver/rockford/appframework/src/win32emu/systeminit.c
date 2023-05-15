/***************************************************************************
*     Copyright (c) 2003, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: systeminit.c $
* $brcm_Revision: Hydra_Software_Devel/6 $
* $brcm_Date: 10/5/03 6:14p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /TestTools/midas/magnum/services/basemodules/kni/win32/systeminit.c $
* 
* Hydra_Software_Devel/6   10/5/03 6:14p frusso
* merged ikos and i2c modules
* 
* Hydra_Software_Devel/5   10/1/03 11:34a frusso
* added include for memc rdb
* 
* Hydra_Software_Devel/4   9/30/03 10:31a frusso
* Added support for emulation
* 
* Hydra_Software_Devel/3   9/23/03 1:04p aram
* started working on 
* 
* Hydra_Software_Devel/2   9/18/03 9:14a frusso
* Added BSystem_Init/BSystem_Uninit stubs
* 
* Hydra_Software_Devel/1   9/17/03 1:51p aram
* genesis of this file
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "bchp_memc.h"
#include "bchp_memc_ddr.h"
#include "bstd.h"
#include "bmem.h"
#include "bkni.h"
#include "systeminit.h"
#include "bkni_emu.h"
#include "bkni_emu_ikos.h"
#include "bkni_emu_i2c.h"

#pragma warning( disable: 4509 )  /* nonstandard extension for exceptio handling */

#define IKOS_HOST "ikosh-irva-103"
#define IKOS_PORT "9999"

#define BEMU_IKOS	1
#define BEMU_I2C	2

static uint32_t g_emuType = 0;

static bool GetEmulationUri( int argc, char **argv, char* pszEmulationUri )
{
	int i;
	char* p;
	for ( i=1; i<argc; i++ )
	{
		if ( (p=strstr(argv[i], "--emulation-ikos") ) != NULL )
		{
			strcpy( pszEmulationUri, p+12 );
			g_emuType = BEMU_IKOS;
			return true;
		}
		else if ( (p=strstr(argv[i], "--emulation-i2c") ) != NULL )
		{
			strcpy( pszEmulationUri, p+12 );
			g_emuType = BEMU_I2C;
			return true;
		}
	}

	return false;
}

int BKNI_EMU_ReadMem( uint32_t address, void *data, size_t size )
{
	switch ( g_emuType )
	{
	case BEMU_IKOS:
		BKNI_EMU_IkosReadMem( address, data, size );
		break;

	case BEMU_I2C:
		BKNI_EMU_I2cReadMem( address, data, size );
		break;

	default:
		printf( "ReadMem: 0x%08X, %ld\n", address, size );
	}

	return 0;
}

int BKNI_EMU_WriteMem( uint32_t address, void *data, size_t size )
{
	size_t i;
	switch ( g_emuType )
	{
	case BEMU_IKOS:
		BKNI_EMU_IkosWriteMem( address, data, size );
		break;

	case BEMU_I2C:
		BKNI_EMU_I2cWriteMem( address, data, size );
		break;

	default:
		printf( "WriteMem: 0x%08X,  ", address );
		for ( i=0; i<size; i++ )
			printf( "0x%02X", ((char*)data)[i] );
		printf( "\n" );
		break;
	}

	return 0;
}

int BKNI_EMU_ReadReg( uint32_t address, uint32_t *data )
{
	switch ( g_emuType )
	{
	case BEMU_IKOS:
		BKNI_EMU_IkosReadReg( address, data );
		break;

	case BEMU_I2C:
		BKNI_EMU_I2cReadReg( address, data );
		break;

	default:
		printf( "ReadReg: 0x%08X\n", address );
		break;
	}

	return 0;
}

int BKNI_EMU_WriteReg( uint32_t address, uint32_t data )
{
	switch ( g_emuType )
	{
	case BEMU_IKOS:
		BKNI_EMU_IkosWriteReg( address, data );
		break;

	case BEMU_I2C:
		BKNI_EMU_I2cWriteReg( address, data );
		break;

	default:
		printf( "WriteReg: 0x%08X, 0x%08X\n", address, data );
		break;
	}

	return 0;
}

static void* g_pHeap = 0;

void memc_init( void )
{
	uint32_t data;

	/* set the GISB timeout value to something reasonable.
	   assume the  server already did it after the successful connect.
	BKNI_EMU_WriteReg( BCHP_SUN_TOP_CTRL_SW_RESET, 0x00000000 ); //Unresets everything
	BKNI_EMU_WriteReg( BCHP_SUN_GISB_ARB_TIMER, 0x00000100 );*/

	/* memory programming */

	BKNI_EMU_ReadReg( BCHP_MEMC_CORE_REV_ID, &data );
	printf( "MEMC_CORE_REV_ID = %08X\n", data );

	BKNI_EMU_ReadReg( BCHP_MEMC_DDR_IOBUF_REV_ID, &data );
	printf( "MEMC_DDR_IOBUF_REV_ID = %08X\n", data );

	BKNI_EMU_ReadReg( BCHP_MEMC_DDR_DRAM_MODE, &data );
	printf( "MEMC_DDR_DRAM_MODE = %08X\n", data );

	BKNI_EMU_WriteReg( BCHP_MEMC_DDR_DRAM_MODE, 0x00000003 );

	BKNI_EMU_ReadReg( BCHP_MEMC_DDR_DRAM_MODE, &data );
	printf( "MEMC_DDR_DRAM_MODE = %08X\n", data );

	BKNI_EMU_ReadReg( BCHP_MEMC_DDR_DRAM_TIMING, &data );
	printf( "MEMC_DDR_DRAM_MODE = %08X\n", data );

	BKNI_EMU_WriteReg( BCHP_MEMC_DDR_CNTRLR_CONFIG, 0x00000002 );
	BKNI_EMU_WriteReg( BCHP_MEMC_DDR_CNTRLR_START_SEQ, 0x00000001 );
}

int BSystem_Init(int argc, char *argv[], BSystem_Info *pSystemInfo)
{
	BKNI_EMU_Settings settings;
	size_t sizeMem, nMaxRegOffset;
	int iError = 0;

	char szEmulationUri[256];
	GetEmulationUri( argc, argv, szEmulationUri );

	switch ( g_emuType )
	{
	case BEMU_IKOS:
		iError = BKNI_EMU_IkosInit( szEmulationUri );
		break;

	case BEMU_I2C:
		//iError = BKNI_EMU_I2CInit( szEmulationUri );
		break;
	}
	
	if ( iError )
	{
		printf( "IKOS failed to initialize (Error %d)\n", iError );
		return -1;
	}

	nMaxRegOffset = (5 * 1024 * 1024);
	sizeMem = (32 * 1024 * 1024);
	g_pHeap = VirtualAlloc( NULL, sizeMem, MEM_COMMIT, PAGE_NOACCESS );

	BKNI_Memset( &settings, 0, sizeof(BKNI_EMU_Settings) );

	settings.bmemAddress = g_pHeap;
	settings.bmemSize = sizeMem;
	settings.bemuReadHandler = BKNI_EMU_ReadMem;
	settings.bemuWriteHandler = BKNI_EMU_WriteMem;
	BKNI_EMU_Init( settings );

	pSystemInfo->bmemAddress = (void*)((uintptr_t)g_pHeap + nMaxRegOffset);
	pSystemInfo->bmemAlignment = 4;
	pSystemInfo->bmemOffset = 0;
	pSystemInfo->bmemSize = sizeMem - nMaxRegOffset;

	pSystemInfo->bregBaseAddr = g_pHeap;
	pSystemInfo->bregMaxRegOffset = nMaxRegOffset;

	__try
	{
		memc_init();
	}
	__except( BKNI_EMU_MemoryHandler( GetExceptionInformation() ) )
	{
		printf( "Midas Exception!\n" );
	}
	return 0;
}

int BSystem_Uninit(void)
{
	switch ( g_emuType )
	{
	case BEMU_IKOS:
		BKNI_EMU_IkosUninit();
		break;

	case BEMU_I2C:
		BKNI_EMU_I2cUninit();
		break;
	}

	if ( g_pHeap )
        VirtualFree( (LPVOID)g_pHeap, 0, MEM_RELEASE );

	return 0;
}

