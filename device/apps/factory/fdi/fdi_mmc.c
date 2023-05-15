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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termio.h>
#include <termios.h>
#include <poll.h>

/* chip manufacture's  header files */

/* humax header files */
#include "fdi_common.h"
#include "fdi_mmc.h"

#include "htype.h"
#include "di_mmc.h"
#include "di_err.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
static FDI_MMC_INFO_t s_tFdiMMCInfo;


/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
pfnFDI_MMC_NotifyFunc g_pfnMMCNofifyFunc;
pfnFDI_MMC_SvcCallbackFunc g_pfnMMCSvcCBFunc;

DI_ERR_CODE INTRN_FDI_MMC_NotifyFunc(FDI_MMC_EVENT_e etMMCEventType,void *pucMMCInfo);
DI_ERR_CODE INTRN_FDI_MMC_SvcCBFunc(DI_MMC_SVC_EVT_E etMMCSvcEventType);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MMC_Open(void)
{
	DI_ERR_CODE err = DI_ERR_OK;

	err = DI_MMC_PowerControl(DI_MMC_PORT_1,DI_MMC_PWR_ON);
	if(err != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MMC_Open] : DI_MMC_PowerControl Error!!\n"));
		return FDI_ERR;
	}
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
FDI_ERR_CODE FDI_MMC_UnMount(FDI_MMC_PORT_E port)

{
	DI_ERR_CODE err = DI_ERR_OK;

	FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MMC_UnMount] : %d \n", port));
	err = DI_MMC_Umount(port);
	if(err != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MMC_UnMount] : DI_MMC_Umount Error!!\n"));
		return FDI_ERR;
	}
	
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
FDI_ERR_CODE FDI_MMC_Mount(FDI_MMC_PORT_E port, unsigned char *mount_dir)
{

	DI_ERR_CODE err = DI_ERR_OK;

	err = DI_MMC_Mount(port, mount_dir);	
	if(err != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MMC_Mount] : DI_MMC_Mount Error!!\n"));
		return FDI_ERR;
	}
	
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
FDI_ERR_CODE FDI_MMC_Format(FDI_MMC_PORT_E port)
{
	DI_ERR_CODE err = DI_ERR_OK;

	err = DI_MMC_Format(port);	
	if(err != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MMC_Format] : DI_MMC_Format Error!!\n"));
		return FDI_ERR;
	}
	
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
FDI_ERR_CODE FDI_MMC_GetWriteLockState(BOOL *pbState)
{
	DI_ERR_CODE err = DI_ERR_OK;
	DI_MMC_INFO_T tMmcInfo;

	err = DI_MMC_GetMMCInfo(1, &tMmcInfo);	
	if(err != DI_ERR_OK)
	{
		*pbState = 0;
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MMC_GetWriteLockState] : DI_MMC_GetMMCInfo Error!!\n"));
		return FDI_ERR;
	}
	
	*pbState = tMmcInfo.bCardLock;
	
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
FDI_ERR_CODE FDI_MMC_GetDataLineWidth(unsigned char *pucWidth)
{
	
	DI_MMC_INFO_T tMmcInfo;
	DI_ERR_CODE err = DI_ERR_OK;
	
	err = DI_MMC_GetMMCInfo(1, &tMmcInfo);	
	if(err != DI_ERR_OK)
	{
		*pucWidth =0;
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MMC_GetDataLineWidth] : DI_MMC_GetMMCInfo Error!!\n"));
		return FDI_ERR;
	}

	if(!tMmcInfo.nMounted)
	{
		*pucWidth =0;
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MMC_Test4bitDataLine] : SD Card Not Mounted!!\n"));
		return FDI_ERR;
	}

	*pucWidth = tMmcInfo.ucBusWidths;

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
FDI_ERR_CODE FDI_MMC_GetSerialNumber(unsigned char *pucSN)
{
	
	DI_MMC_INFO_T tMmcInfo;
	DI_ERR_CODE err = DI_ERR_OK;
	
	err = DI_MMC_GetMMCInfo(1, &tMmcInfo);
	if(err != DI_ERR_OK)
	{
		pucSN[0] = 0;
		pucSN[1] = 0;
		pucSN[2] = 0;
		pucSN[3] = 0;
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MMC_GetSerialNumber] : DI_MMC_GetMMCInfo Error!!\n"));
		return FDI_ERR;
	}
	
	if(!tMmcInfo.nMounted)
	{
		pucSN[0] = 0;
		pucSN[1] = 0;
		pucSN[2] = 0;
		pucSN[3] = 0;
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MMC_GetSerialNumber] : SD Card Not Mounted!!\n"));
		return FDI_ERR;
	}
	pucSN[0] = tMmcInfo.ucPSN[0];
	pucSN[1] = tMmcInfo.ucPSN[1];
	pucSN[2] = tMmcInfo.ucPSN[2];
	pucSN[3] = tMmcInfo.ucPSN[3];

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
FDI_ERR_CODE FDI_MMC_RegisterCallback(pfnFDI_MMC_NotifyFunc pfnCallback)
{
	DI_ERR_CODE err = DI_ERR_OK;

	g_pfnMMCNofifyFunc = pfnCallback;

	err = DI_MMC_RegisterCallbackFunc((pfnDI_MMC_CallbackFunc)INTRN_FDI_MMC_NotifyFunc);
	if(err != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MMC_RegisterCallback] : DI_MMC_RegisterCallbackFunc Error!!\n"));
		return FDI_ERR;
	}

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
DI_ERR_CODE INTRN_FDI_MMC_NotifyFunc(FDI_MMC_EVENT_e etMMCEventType, void *pvMMCInfo)
{
	int				i, index =0;
	DI_MMC_HP_EVT_T		*ptDiMMCInfo;

	ptDiMMCInfo = (DI_MMC_HP_EVT_T *)pvMMCInfo;

	if( ptDiMMCInfo != NULL )
	{
		FDI_PRINT(FDI_PRT_DBGINFO, ("++[INTRN_FDI_MMC_NotifyFunc] event %d, port %d, dev name %s\n",
			etMMCEventType, ptDiMMCInfo->portnum, ptDiMMCInfo->dev_name));

		if( etMMCEventType == FDI_MMC_EVENT_INSERT )
		{
			memcpy(&s_tFdiMMCInfo,ptDiMMCInfo,sizeof(DI_MMC_HP_EVT_T));
		}
		else if( etMMCEventType == FDI_MMC_EVENT_EXTRACT)
		{
			memcpy(&s_tFdiMMCInfo,ptDiMMCInfo,sizeof(DI_MMC_HP_EVT_T));
		}

		if( (g_pfnMMCNofifyFunc != NULL))
		{
			FDI_PRINT(FDI_PRT_DBGINFO, ("--[INTRN_FDI_MMC_NotifyFunc] callback event %d, port %d, \n",
				etMMCEventType , ptDiMMCInfo->portnum));

			g_pfnMMCNofifyFunc(etMMCEventType, &s_tFdiMMCInfo);
		}
	}

	return DI_ERR_OK;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MMC_RegisterSvcCallback(pfnFDI_MMC_SvcCallbackFunc pfnCallback)
{
	DI_ERR_CODE err = DI_ERR_OK;

	g_pfnMMCSvcCBFunc = pfnCallback;

	err = DI_MMC_RegisterSvcCallbackFunc(INTRN_FDI_MMC_SvcCBFunc);
	if(err != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_MMC_RegisterSvcCallback] : DI_MMC_RegisterSvcCallbackFunc Error!!\n"));
		return FDI_ERR;
	}

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
DI_ERR_CODE INTRN_FDI_MMC_SvcCBFunc(DI_MMC_SVC_EVT_E etMMCSvcEventType)
{
	int				i, index =0;
	FDI_MMC_SVC_EVT_e	etMMCSvcMsg;
	
	if( etMMCSvcEventType < DI_MMC_SVC_EVT_MAX )
	{
		FDI_PRINT(FDI_PRT_DBGINFO, ("++[INTRN_FDI_MMC_SvcCBFunc] event %d\n", etMMCSvcEventType));

		if( etMMCSvcEventType == DI_MMC_SVC_EVT_FORMAT_DONE )
		{
			etMMCSvcMsg = FDI_MMC_SVC_EVT_FORMAT_DONE;
		}
		else if( etMMCSvcEventType == DI_MMC_SVC_EVT_FORMAT_FAIL)
		{
			etMMCSvcMsg = FDI_MMC_SVC_EVT_FORMAT_FAIL;
		}
		else if( etMMCSvcEventType == DI_MMC_SVC_EVT_MOUNT_DONE)
		{
			etMMCSvcMsg = FDI_MMC_SVC_EVT_MOUNT_DONE;
		}
		else if( etMMCSvcEventType == DI_MMC_SVC_EVT_MOUNT_FAIL)
		{
			etMMCSvcMsg = FDI_MMC_SVC_EVT_MOUNT_FAIL;
		}
		else if( etMMCSvcEventType == DI_MMC_SVC_EVT_UMOUNT_DONE)
		{
			etMMCSvcMsg = FDI_MMC_SVC_EVT_UMOUNT_DONE;
		}
		else if( etMMCSvcEventType == DI_MMC_SVC_EVT_UMOUNT_FAIL)
		{
			etMMCSvcMsg = FDI_MMC_SVC_EVT_UMOUNT_FAIL;
		}

		if(g_pfnMMCSvcCBFunc != NULL)
		{
			FDI_PRINT(FDI_PRT_DBGINFO, ("--[INTRN_FDI_MMC_NotifyFunc] callback event %d\n", etMMCSvcMsg));
			g_pfnMMCSvcCBFunc(etMMCSvcMsg);
		}
	}

	return DI_ERR_OK;
}


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_MMC_Close(void)
{

	FDI_PRINT(FDI_PRT_DBGINFO, ("[FDI_MMC_Close] : close success!!\n"));

	return FDI_NO_ERR;
}

