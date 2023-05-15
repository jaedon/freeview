/*******************************************************************/
/************************* File Description ************************/
/*******************************************************************/
// File Name: di_factory.c
// Original Author: 
// File Description:
// Remarks:

/*******************************************************************/
/* Copyright (c) 2008 HUMAX Co., Ltd.                     */
/* All rights reserved.                                             */
/*******************************************************************/


/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
/* include common utils first! */
#include "linden_trace.h"
#include "bstd.h"
#include "nexus_security.h"

/* include drv headers */
#include "vkernel.h"

/* include di headers */
#include "di_err.h"
#include "drv_err.h"
#include "drv_flash.h"
#if !defined(CONFIG_DI20)
#include "di_factory.h"
#endif

#define DEBUG_MODULE			TRACE_MODULE_DRV_MISC


#define BEP_OFFSET		0x00
#define BEP_SIZE		2

#define FACTORT_OFFSET	0xE0000
#define FACTORT_SIZE	(512*1024)

/* Normal Boot Entry is 0xBFC00000 */
#define NORMAL_BOOT_ENTRY		(0xBFC0)


/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/
#if defined(CONFIG_NOR_FLASH)
static int s_fd = 0;
#endif

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/

/********************************************************************
 Function   : DRV_FACTORY_Init
 Description :
 Input      : 	none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_FACTORY_Init (void)
{
	
	PrintEnter();

#if defined(CONFIG_NOR_FLASH)
	DRV_FLASH_GetCFE_Fd(&s_fd);
#endif

	PrintExit();

	return DRV_OK;
}


/********************************************************************
 Function   : DI_FACTORY_DefaultSet
 Description :
 Input      : 	none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DI_ERR_CODE DI_FACTORY_DefaultSet (void)
{
#if defined(CONFIG_NOR_FLASH)
	int				i, nErr, retry_count = 5;
	unsigned short 	databuf, tempdata;
	unsigned long  	dest_addr;
	unsigned long	errflag = 0;

	
	PrintEnter();

	/* set BEP first */
	databuf   = NORMAL_BOOT_ENTRY;
	do
	{
		PrintDebug("block start address = 0x%x\n", BEP_OFFSET);
		PrintDebug("BEP restore: data = 0x%x\n", databuf);

		nErr = DRV_FLASH_Write(s_fd, BEP_OFFSET, (unsigned char *)&databuf, 2);
		if(nErr != 0)
			PrintError("Error in DRV_FLASH_Write\r\n");

		DRV_FLASH_Read(s_fd, BEP_OFFSET, (unsigned char *)&tempdata, 2);

		PrintDebug("veritfy BEP : 0x%x-0x%x\n", tempdata, databuf);
		if(tempdata == databuf)
			break;
	}while(retry_count--);

	if(retry_count <= 0)
	{
		PrintError("Failed restoring BEP!\n");
		return DI_ERR_ERROR;
	}

	PrintDebug("BEP Write OK!!\n");

	/* erase Factory Loader */
	dest_addr = FACTORT_OFFSET;
	for(i=0; i<4; i++)
	{
		nErr = DRV_FLASH_EraseBlock(s_fd, dest_addr);
		if(nErr != 0)
		{
			errflag = (errflag << 1) & 0x01;
			PrintError("Erase Error in (0x%X)--addr(0x%X)\n", i, dest_addr);
		}
		dest_addr += 0x20000;
	}

	if(errflag)
	{
		return DI_ERR_ERROR;
	}

	PrintDebug("Erase Factory Loader OK!!\n");

	PrintExit();
#endif

	return DI_ERR_OK;
}

#if !defined(CONFIG_DI20)
/********************************************************************
 Function   : DI_FACTORY_RegisterDownloadNotify
 Description :
 Input      : 	pfnDI_FACTORY_DOWNLOAD_Notify
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DI_ERR_CODE DI_FACTORY_RegisterDownloadNotify (pfnDI_FACTORY_DOWNLOAD_Notify fnFacNotify)
{
	PrintEnter();
	HAPPY(fnFacNotify);

	PrintExit();

	return DI_ERR_OK;
}
#endif
