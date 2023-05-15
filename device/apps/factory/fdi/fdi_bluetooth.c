/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :		 Factory Application
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */

/* humax header files */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <stddef.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>

#include <stdlib.h>

#include "fdi_common.h"
#include "fdi_bluetooth.h"

#include "di_err.h"
#include "drv_err.h"
#include "di_bt.h"

static BOOL		s_bIsBluetoothInit=FALSE;

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_BT_Open(void)
{		
	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_BT_Enable(void)
{	
	DI_BT_Enable(TRUE);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_BT_StartScan(void)
{	

	DI_BT_Scan(DI_BT_PROFILE_ALL);

	return FDI_NO_ERR;
}

FDI_ERR_CODE FDI_BT_GetBdAddr(unsigned char *pucBdAddr)
{
	DI_BT_DEV_INFO_t stBtDevInfo;
	
	DI_ERR_CODE	diErr = 0;
	
	if(pucBdAddr == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_BT_GetBdAddr] : Invalid Input Param\n"));
		return FDI_ERR;
	}
	
	diErr = DI_BT_GetHostInfo(&stBtDevInfo);
	if(diErr != 0)
	{
		return FDI_ERR;
	}

	memcpy(pucBdAddr, stBtDevInfo.ucBdAddr, 6);

	return FDI_NO_ERR;
}

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_BT_RegisterCallback(pfnBT_EVT_CALLBACK pCallback)
{
	
	if(pCallback == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_BT_RegisterCallback] : Invalid Input Param\n"));
		return FDI_ERR;
	}

	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_INSERTED, pCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_EXTRACTED, pCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_SCAN_SUCCESS, pCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_SCAN_FAIL, pCallback);
	DI_BT_RegisterCallback(DI_BT_NOTIFY_EVT_ENABLE, pCallback);

	return FDI_NO_ERR;
}


