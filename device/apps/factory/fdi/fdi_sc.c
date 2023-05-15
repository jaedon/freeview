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
#include "fdi_common.h"
#include "fdi_sc.h"

#include "htype.h"
#include "di_smartcard.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FDI_SC_MAX	2

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
static pfnFDI_SC_EVT_CALLBACK s_afnScCallback[FDI_SC_MAX];

void INTRN_FDI_SC_EventCallBack(HUINT32 unUnitID, DI_SC_STATUS nStatus);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_SC_Open(unsigned long ulDeviceId)
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
FDI_ERR_CODE FDI_SC_GetCapability(unsigned long *pulNumofDevice)
{
	DI_ERR_CODE eDiErr;
#if defined(FACTORY_USE_SMARTCARD)

	eDiErr = DI_SC_GetCapability((HUINT32*)pulNumofDevice);
#else
#err
#endif
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SC_Init] DDI_SC_GetCapability Error : %d\n", eDiErr));
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
FDI_ERR_CODE FDI_SC_Reset(unsigned long ulDeviceId, unsigned char *pucAtr, unsigned char *pucLen)
{
	DI_ERR_CODE eDiErr;
	HUINT32 unAtrlen;
	
	FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SC_Reset] : DeviceID=%d\n", ulDeviceId));
	eDiErr = DI_SC_Reset((unsigned char)ulDeviceId, pucAtr, &unAtrlen);
	if(eDiErr != DI_ERR_OK )
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SC_Reset] : DI_SC_Reset\n"));
		return FDI_ERR;
	}

	*pucLen = unAtrlen;

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
FDI_ERR_CODE FDI_SC_SetParams(unsigned long ulDeviceId, FDI_SC_PROTOCOL eProtocol)
{
	DI_ERR_CODE eDiErr;

	if( eProtocol == FDI_T_14)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SC_SetParams] : eProtocol = %d\n",eProtocol));
		eDiErr = DI_SC_SetParams( ulDeviceId, 14,3000, 0,2, 1, 0,13, 5);
	}
	else
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SC_SetParams] : eProtocol = %d\n",eProtocol));
		eDiErr = DI_SC_SetParams(ulDeviceId, 0, 0, 0, 1, 1, 0, 13, 4);
	}

	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SC_SetParams] DI_SC_SetParams Error : %d\n", eDiErr));
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
FDI_ERR_CODE FDI_SC_GetCardStatus(unsigned long ulDeviceId, FDI_SC_STATUS *peStatus)
{
	DI_ERR_CODE eDiErr;
	DI_SC_STATUS eDiStatus;
		
	eDiErr = DI_SC_GetCardStatus(ulDeviceId, &eDiStatus);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SC_GetCardStatus] DI_SC_SetParams Error : %d\n", eDiErr));
		return FDI_ERR;
	}

	switch(eDiStatus)
	{
		case DI_SC_NOT_PRESENT:
			*peStatus = FDI_SC_NOT_PRESENT;
			break;
			
		case DI_SC_PRESENT:
			*peStatus = FDI_SC_PRESENT;
			break;

		default:
			FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SC_GetCardStatus] Unknown status : %d\n", eDiStatus));
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
FDI_ERR_CODE FDI_SC_RegisterCallback(unsigned long ulDeviceId, pfnFDI_SC_EVT_CALLBACK pfnCallback)
{
	DI_ERR_CODE eDiErr;

	if(pfnCallback == NULL)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SC_RegisterCallback] Invalid Parameter. pfnCallback==NULL\n"));
		return FDI_ERR;
	}

	s_afnScCallback[ulDeviceId] = pfnCallback;

	/* register callback function */
	eDiErr = DI_SC_RegisterCallback(ulDeviceId, INTRN_FDI_SC_EventCallBack);
	if(eDiErr != DI_ERR_OK)
	{
		FDI_PRINT(FDI_PRT_ERROR, ("[FDI_SC_RegisterCallback] DDI_SC_RegisterCallbackFunc Error : %d\n", eDiErr));
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
void INTRN_FDI_SC_EventCallBack(HUINT32 unUnitID, DI_SC_STATUS nStatus)
{
	if(s_afnScCallback[unUnitID] != NULL)
	{
		if( nStatus == DI_SC_PRESENT )
		{
			s_afnScCallback[unUnitID](unUnitID, FDI_SC_INSERTED);
		}
		else if( nStatus == DI_SC_NOT_PRESENT )
		{
			s_afnScCallback[unUnitID](unUnitID, FDI_SC_EXTRACTED);
		}
	}
}

