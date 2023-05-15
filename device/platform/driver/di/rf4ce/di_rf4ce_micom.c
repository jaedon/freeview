/*******************************************************************************
* File name : di_rf4ce_micom.c
* Author : D.K Lee
* description :
*
* Copyright (c)  by Humax Co., Ltd. Since 2007.
* All right reserved
*******************************************************************************/

/*******************************************************************************
* Header and definitions
*******************************************************************************/
/* global header files */

/* chip manufacture's  header files */
#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
#include "callbacks.h"
#include "uapi.h"
#endif

/* humax header files */
#include "htype.h"
#include "di_err.h"
#include "vkernel.h"
#include "drv_err.h"
#include "di_uart.h"
#include "di_rf4ce.h"
#include "di_rf4ce_priv.h"
#include "drv_micom.h"
#include "drv_hilcrest.h"


/*******************************************************************************
* Debug level
*******************************************************************************/
#ifdef DI_RF4CE_DEBUG
#define PrintInfo		DI_UART_Print
#define PrintDebug	DI_UART_Print
#define PrintError	DI_UART_Print
#else
#define PrintData		while (0) ((int (*)(char *, ...)) 0)
#define PrintDebug	while (0) ((int (*)(char *, ...)) 0)
#define PrintError	DI_UART_Print
#define PrintInfo		DI_UART_Print
#endif


/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/


/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/
extern RF4CE_INSTANCE_t g_tRF4CEInstance;


/*******************************************************************************
* Static variables and structures
*******************************************************************************/
#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
static unsigned char sucRf4cePairingTableIndex=0; 
#endif

/*******************************************************************************
* Static function prototypes
*******************************************************************************/
static void P_RF4CE_MICOM_PairingNotify(HUINT8 ucPairingResult);

#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
static void P_RF4CE_MICOM_ZrcUserDataNotify (unsigned char ucPairingIndex, unsigned char *pucData, unsigned char ucSize);
#endif


/*******************************************************************************
* function : DRV_RF4CE_MICOM_Init
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DRV_Error DRV_RF4CE_MICOM_Init(void)
{
	DRV_MICOM_RegisterRf4cePairingResultCallback(P_RF4CE_MICOM_PairingNotify);

#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
	DRV_MICOM_RegisterRf4ceUserDataIncomingCallback(P_RF4CE_MICOM_ZrcUserDataNotify);
#endif	

	return DRV_OK;
}

/*******************************************************************************
* function : DI_RF4CE_SetPairing
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_RF4CE_SetPairing(HBOOL bPairing,HUINT32 ulTimeout)
{
	DRV_Error nDrvRet = DRV_OK;
	HINT32 nVkRet;

	nVkRet = VK_SEM_Get(g_tRF4CEInstance.ulSemID);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		return DI_ERR_ERROR;
	}
	
	if(bPairing == TRUE)
	{
		nDrvRet=DRV_MICOM_SetPairing((unsigned char)ulTimeout);
	}
	else
	{
		nDrvRet=DRV_MICOM_SetPairing((unsigned char)0);
	}
	
	if(nDrvRet)
	{
		PrintError("[%s] ERR DRV_MICOM_GetPairedDevMac %d!!\n", __func__, nDrvRet);
		VK_SEM_Release(g_tRF4CEInstance.ulSemID);
		return DI_ERR_OK;
	}

	VK_SEM_Release(g_tRF4CEInstance.ulSemID);

	return DI_ERR_OK;
}


/*******************************************************************************
* function : DI_RF4CE_GetPairedDevCount
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_RF4CE_GetPairedDevCount(HUINT32 *pulCnt)
{
	DRV_Error nDrvRet = DRV_OK;
	HINT32 nVkRet;

	nVkRet = VK_SEM_Get(g_tRF4CEInstance.ulSemID);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		return DI_ERR_ERROR;
	}

	nDrvRet=DRV_MICOM_GetPairedDevs((unsigned char *)pulCnt);
	if(nDrvRet)
	{
		PrintError("[%s] ERR DRV_MICOM_GetPairedDevMac %d!!\n", __func__, nDrvRet);
		VK_SEM_Release(g_tRF4CEInstance.ulSemID);
		return DI_ERR_OK;
	}

	VK_SEM_Release(g_tRF4CEInstance.ulSemID);

	return DI_ERR_OK;
}


/*******************************************************************************
* function : DI_RF4CE_GetIEEEAddr
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
DI_ERR_CODE DI_RF4CE_GetIEEEAddr(HUINT32 ulIndex, HUINT8 *pucValue)
{
	DRV_Error nDrvRet = DRV_OK;
	HINT32 nVkRet;

	nVkRet = VK_SEM_Get(g_tRF4CEInstance.ulSemID);
	if(nVkRet != VK_OK)
	{
		PrintError("[%s] VK_SEM_Get failed!\n",__func__);
		return DI_ERR_ERROR;
	}
	
	nDrvRet=DRV_MICOM_GetPairedDevIEEEAddr(ulIndex,pucValue);
	if(nDrvRet)
	{
		PrintError("[%s] ERR DRV_MICOM_GetPairedDevMac %d!!\n", __func__, nDrvRet);
		VK_SEM_Release(g_tRF4CEInstance.ulSemID);
		return DI_ERR_OK;
	}

	VK_SEM_Release(g_tRF4CEInstance.ulSemID);
	
	return DI_ERR_OK;
}

#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
/*******************************************************************************
* function : DI_RF4CE_GetIEEEAddr
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
unsigned char DRV_RF4CE_GetCurrentPairingTableIndex(void)
{
	return sucRf4cePairingTableIndex;
}
#endif

/*******************************************************************************
* function : DRV_RF4CE_MICOM_Task
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
void DRV_RF4CE_MICOM_Task(void *pvParam)
{
	int nVkRet;
	RF4CE_MSG_t tRf4ceMsg;
	DI_RF4CE_CALLBACK_DATA_t tCallbackData;
	
#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
	UAPIDataPkt tUapiDataPacket;

#if defined(RF4CE_DEBUG_USER_DATA)	
	int i;
	unsigned char	*pucData;
#endif	
#endif
	
	pvParam = pvParam;

	while (1)
	{
		nVkRet = VK_MSG_Receive(g_tRF4CEInstance.ulMsgQID, &tRf4ceMsg, sizeof(RF4CE_MSG_t));
		if (nVkRet != VK_OK)
		{
			continue;
		}

		switch (tRf4ceMsg.eMsg)
		{
			case RF4CE_EVT_PARING_RESULT:
				if(tRf4ceMsg.aucParam[0] == 1) /* Fail */
				{
					if(g_tRF4CEInstance.tCallbackInfo[DI_RF4CE_NOTIFY_EVT_PAIRING_FAILED].pfnCallback)
					{
						tCallbackData.eEvent = DI_RF4CE_NOTIFY_EVT_PAIRING_FAILED;
						(*g_tRF4CEInstance.tCallbackInfo[DI_RF4CE_NOTIFY_EVT_PAIRING_FAILED].pfnCallback)(&tCallbackData);
					}
				}
				else /* Sucess */
				{
					if(g_tRF4CEInstance.tCallbackInfo[DI_RF4CE_NOTIFY_EVT_PAIRING_SUCCESS].pfnCallback)
					{
						tCallbackData.eEvent = DI_RF4CE_NOTIFY_EVT_PAIRING_SUCCESS;
						(*g_tRF4CEInstance.tCallbackInfo[DI_RF4CE_NOTIFY_EVT_PAIRING_SUCCESS].pfnCallback)(&tCallbackData);
					}
				}
				break;

#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
			case RF4CE_EVT_ZRC_USER_DATA_INCOMING:
#if defined(RF4CE_DEBUG_USER_DATA)			
				DI_UART_Print("[P_RF4CE_Task] : User Data Incomming(Size(%d) @ ParingIndex(%d)\n", (tRf4ceMsg.ulMsgSize-1 -sizeof(UAPIHdr_t)), tRf4ceMsg.aucParam[0]);
				for (i=0; i<tRf4ceMsg.ulMsgSize-1; i++)
				{
					DI_UART_Print("%x ", tRf4ceMsg.aucParam[1+i]);
				}
				DI_UART_Print("\n");
#endif
				/* RemoteID (ParingTable Index) */
				sucRf4cePairingTableIndex =	tRf4ceMsg.aucParam[0];
				tUapiDataPacket.remoteID = sucRf4cePairingTableIndex;

				/* deviceID (RF4CE as 0 */
				tUapiDataPacket.deviceID = UAPI_RF4CE_NIF;
		
				/* UAPI Header */
				VK_MEM_Memcpy(&tUapiDataPacket.uapiHdr, &tRf4ceMsg.aucParam[1], sizeof(UAPIHdr_t));
				
#if defined(RF4CE_DEBUG_USER_DATA)				
				DI_UART_Print("==================[P_RF4CE_Task] UAPI Header================\n");
				pucData = (unsigned char *)&tUapiDataPacket.uapiHdr;
				for (i=0; i<sizeof(UAPIHdr_t); i++)
				{
					DI_UART_Print("0x%x ", pucData[i]);
				}
				DI_UART_Print("\n");
#endif

				/* Pure Data Length */					
				tUapiDataPacket.dataLength = tRf4ceMsg.ulMsgSize-1 -sizeof(UAPIHdr_t);

				/* Pointer to Pure Data */
				tUapiDataPacket.data = &tRf4ceMsg.aucParam[1+sizeof(UAPIHdr_t)];
#if defined(RF4CE_DEBUG_USER_DATA)		
#if 0
				DI_UART_Print("==================[P_RF4CE_Task] UEI Data Length (%d) and Pointer(0x%x)================\n", tUapiDataPacket.dataLength, tUapiDataPacket.data);

				DI_UART_Print("==================[P_RF4CE_Task] Total UEI Data ================\n");				
				pucData = (unsigned char *)&tUapiDataPacket;

				for (i=0; i<sizeof(UAPIDataPkt) + tUapiDataPacket.dataLength; i++)
				{
					DI_UART_Print("0x%02x ", pucData[i]);
				}
				DI_UART_Print("\n");
#endif
#endif
				
				/* Call UAPI API to feed raw data to Lib */
				UAPIDeMultiplexer(&tUapiDataPacket);
				break;
#endif
				
			default:
				break;
		}
	}
}


/*******************************************************************************
* function : P_RF4CE_MICOM_PairingNotify
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_RF4CE_MICOM_PairingNotify(HUINT8 ucPairingResult)
{
	RF4CE_MSG_t	tRf4ceMsg;
	HINT32 nVkRet = VK_OK;

	tRf4ceMsg.eMsg = RF4CE_EVT_PARING_RESULT;
	tRf4ceMsg.ulMsgSize =1;
	tRf4ceMsg.aucParam[0] = ucPairingResult;

	nVkRet = VK_MSG_Send(g_tRF4CEInstance.ulMsgQID, &tRf4ceMsg, sizeof(RF4CE_MSG_t));
}

#if defined(CONFIG_RF4CE_MOTION_TOUCH_UEI_HILCREST)
/*******************************************************************************
* function : P_RF4CE_MICOM_ZrcUserDataNotify
* description :
* input :
* output :
* return : 
* 기타 참고자료 및 파일
*******************************************************************************/
static void P_RF4CE_MICOM_ZrcUserDataNotify (unsigned char ucPairingIndex, unsigned char *pucData, unsigned char ucSize)
{
	RF4CE_MSG_t	Rf4ceMsg;
	HINT32 nVkRet = VK_OK;

	PrintDebug("[P_RF4CE_MICOM_ZrcUserDataNotify] User Data Size(%d) @ ParingIndex(%d)\n", ucSize, ucPairingIndex);

	if (ucSize >= RF4CE_MAX_MSG_SIZE)
	{
		PrintError("[P_RF4CE_MICOM_ZrcUserDataNotify] Invalid User Data Size(%d) @ ParingIndex(%d)\n", ucSize, ucPairingIndex);
		return;
	}

	Rf4ceMsg.eMsg = RF4CE_EVT_ZRC_USER_DATA_INCOMING;
	Rf4ceMsg.ulMsgSize =1+ucSize; /* including PairingIndex */
	Rf4ceMsg.aucParam[0] = ucPairingIndex;
	VK_MEM_Memcpy(&Rf4ceMsg.aucParam[1], pucData, ucSize);
	nVkRet = VK_MSG_Send(g_tRF4CEInstance.ulMsgQID, &Rf4ceMsg, sizeof(RF4CE_MSG_t));
}
#endif


/* end of file */

