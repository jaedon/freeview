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
#include <string.h>

/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_tmd.h"
#include "fai_bluetooth.h"

#include "fdi_err.h"
#include "fdi_bluetooth.h"
#include "fdi_scart.h"

/* model specific configuration header */
#include "factorysw_config.h"

#if defined(FACTORY_USE_BT)
#include "di_bt.h"
#endif



/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
typedef enum
{
	FAI_BLUETOOTH_MSG_NONE = 0,
	FAI_BLUETOOTH_MSG_INSERTED,	
	FAI_BLUETOOTH_MSG_ENABLED,	
	FAI_BLUETOOTH_MSG_DISCOVERY_OK,
	FAI_BLUETOOTH_MSG_DISCOVERY_FAIL,
	FAI_BLUETOOTH_MSG_PARLING,
	FAI_BLUETOOTH_MSG_MAX
} FAI_BLUETOOTH_MSG_TYPE_e;

typedef enum
{
	FAI_BLUETOOTH_STATUS_DISABLE = 0,
	FAI_BLUETOOTH_STATUS_INSERTED,	
	FAI_BLUETOOTH_STATUS_READY,
	FAI_BLUETOOTH_STATUS_PENDING,	
	FAI_BLUETOOTH_STATUS_FAIL,		
	FAI_BLUETOOTH_STATUS_PASS
} FAI_BLUETOOTH_STATUS_e;

typedef struct FAI_BLUETOOTH_MSG
{
	FAI_BLUETOOTH_MSG_TYPE_e ucBluetoothMsgType;
} FAI_BLUETOOTH_MSG_t;

#if (BLUETOOTH_UI_ENABLE == YES)
#define FAI_BLUETOOTH_UI_TIMEOUT_SECONDS	20
#endif

/*******************************************************************************
* External variables and functions
*******************************************************************************/

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
unsigned long g_ulBluetoothMsgId, g_ulBluetoothTaskId;
static int	s_nRetryCount=0;
static FAI_BLUETOOTH_STATUS_e s_nBtStatus=FAI_BLUETOOTH_STATUS_DISABLE;
static HUINT32 g_uiDeviceCnt = 0;
static DI_BT_DEV_LIST_t g_sCbEvtDevInfo;
static HBOOL bBsaServerOpened = FALSE;
static unsigned long g_ulMSGId;


#if 1
static char	*s_pcBluetooth_DeviceName = "humax_bt_device"; /* Factory BT device */
#else
static char	*s_pcBluetooth_DeviceName = "JasonHan"; /* Just for test */
#endif
#define FAI_BLUETOOTH_DEFAULT_DEVICENAME 		"no-discovery"
#define FAI_BLUETOOTH_DEFAULT_DEVICENAME_UI		"ONTECH"

char	gBluetooth_DeviceName[BT_NB_NAME_LEN] = {0,}; /* Factory BT device */
unsigned int gBluetoothlen = 0;

/*******************************************************************************
* Functions
*******************************************************************************/
static void INTRN_FAI_BLUETOOTH_Task(void *pData);
static void INTRN_FAI_BLUETOOTH_Callback(void *data);
static HBOOL INTRN_FAI_BLUETOOTH_Compare_DevName(void);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
*******************************************************************************/
static void INTRN_FAI_BLUETOOTH_Task(void *pData)
{
	FAI_BLUETOOTH_MSG_t tBluetoothMsg;
	FVK_ERR_CODE eVkRet;
	HBOOL bFound = FALSE;
	FDI_ERR_CODE fdiErr = 0;

	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];

	while(1)
	{
		eVkRet = FVK_MSG_Receive(g_ulBluetoothMsgId, &tBluetoothMsg, sizeof(tBluetoothMsg));
		if (eVkRet != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_IP_Task] : Error FVK_MSG_Receive() %d\n", eVkRet));
			continue;
		}
		else
		{
			FAI_PRINT(FAI_PRT_WARNING, ("INTRN_FAI_BLUETOOTH_Task MSG = %d \n", tBluetoothMsg.ucBluetoothMsgType));
			switch(tBluetoothMsg.ucBluetoothMsgType)
			{
				case FAI_BLUETOOTH_MSG_INSERTED:
					FAI_PRINT(FAI_PRT_ERROR, ("### Bluetooth device is ready !!! ###\n"));
					s_nBtStatus = FAI_BLUETOOTH_STATUS_INSERTED;
					break;

				case FAI_BLUETOOTH_MSG_ENABLED:
					FAI_PRINT(FAI_PRT_ERROR, ("### Bluetooth device is BSA server ENABLED !!! ###\n"));
					s_nBtStatus = FAI_BLUETOOTH_STATUS_READY;
					break;
					
				case FAI_BLUETOOTH_MSG_DISCOVERY_OK:
					bFound = INTRN_FAI_BLUETOOTH_Compare_DevName();
					if(bFound)
					{
						s_nBtStatus = FAI_BLUETOOTH_STATUS_PASS;	
						tTMDResult.usMsgId	= g_ulBluetoothMsgId;
						tTMDResult.eResult = FAI_TMD_ERR_OK;
						//ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_WIFI;
						//ucTmdErrCode[1] = FAI_WIFI_TMD_ERR_CONNECT;
						//FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
						FAI_TMD_SetResult(&tTMDResult);
					}
					else
					{
						s_nBtStatus = FAI_BLUETOOTH_STATUS_FAIL;

						tTMDResult.usMsgId	= g_ulBluetoothMsgId;
						tTMDResult.eResult = FAI_TMD_ERR_FAIL;
						ucTmdErrCode[0] = 0x00;
						ucTmdErrCode[1] = 0x01;
						FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
						FAI_TMD_SetResult(&tTMDResult);	
					}
					break;
			
				case FAI_BLUETOOTH_MSG_DISCOVERY_FAIL:
					if(s_nRetryCount < 5)
					{
						s_nRetryCount++;
						FDI_BT_StartScan();
						FAI_PRINT(FAI_PRT_ERROR, ("### Restart Discovery (Count %d) ###\n", s_nRetryCount));
					}
					else
					{
						s_nBtStatus = FAI_BLUETOOTH_STATUS_FAIL;
						FAI_PRINT(FAI_PRT_ERROR, ("### Discovery failed (Count %d) ###\n", s_nRetryCount));
					}
					break;
			
				default:
					break;
			}
		}
 	}
}


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
*******************************************************************************/
void INTRN_FAI_BLUETOOTH_Callback(void *data)
{
	FVK_ERR_CODE eVkRet;
	DI_BT_CALLBACK_DATA_t *stCbData;
	DI_BT_DEV_LIST_t *stDevInfo;
	FAI_BLUETOOTH_MSG_t tBluetoothMsg;
	
	stCbData = (DI_BT_CALLBACK_DATA_t *)data;
	stDevInfo = (DI_BT_DEV_LIST_t *)stCbData->pvEventData;
	
	FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_BLUETOOTH_Callback] : GET GET GET : %d\n", stCbData->eEvent));
	switch(stCbData->eEvent)
	{
		case DI_BT_NOTIFY_EVT_INSERTED:
			tBluetoothMsg.ucBluetoothMsgType = FAI_BLUETOOTH_MSG_INSERTED;
			break;
			
		case DI_BT_NOTIFY_EVT_ENABLE:
			tBluetoothMsg.ucBluetoothMsgType = FAI_BLUETOOTH_MSG_ENABLED;
			break;			
			
		case DI_BT_NOTIFY_EVT_SCAN_SUCCESS:
			tBluetoothMsg.ucBluetoothMsgType = FAI_BLUETOOTH_MSG_DISCOVERY_OK;
			g_uiDeviceCnt = stCbData->ulNumOfData;
			memcpy(&g_sCbEvtDevInfo, stDevInfo, sizeof(g_sCbEvtDevInfo));
			break;

		case DI_BT_NOTIFY_EVT_SCAN_FAIL:
			tBluetoothMsg.ucBluetoothMsgType = FAI_BLUETOOTH_MSG_DISCOVERY_FAIL;
			break;

		default:
			break;
	}
#if 0	
	if(stCbData->eEvent == DI_BT_NOTIFY_EVT_SCAN_SUCCESS)
	{
		int index;
		for (index = 0; index < g_uiDeviceCnt; index++)
		{
			printf("[%d] name %s \n", index, g_sCbEvtDevInfo.stBtDevList[index].ucName);
			printf("[%d] BdAddr %x:%x:%x:%x:%x:%x \n", 
						index,
						g_sCbEvtDevInfo.stBtDevList[index].ucBdAddr[0],
						g_sCbEvtDevInfo.stBtDevList[index].ucBdAddr[1],
						g_sCbEvtDevInfo.stBtDevList[index].ucBdAddr[2],
						g_sCbEvtDevInfo.stBtDevList[index].ucBdAddr[3],
						g_sCbEvtDevInfo.stBtDevList[index].ucBdAddr[4],
						g_sCbEvtDevInfo.stBtDevList[index].ucBdAddr[5]);
			printf("[%d] BdAddr %x:%x:%x \n", 
						index,
						g_sCbEvtDevInfo.stBtDevList[index].ucClass[0],
						g_sCbEvtDevInfo.stBtDevList[index].ucClass[1],
						g_sCbEvtDevInfo.stBtDevList[index].ucClass[2]);
		}
	}
#endif	
	eVkRet = FVK_MSG_Send(g_ulBluetoothMsgId, &tBluetoothMsg, sizeof(tBluetoothMsg));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_BLUETOOTH_Callback] : Error FVK_MSG_Send() : %d\n", eVkRet));
	}
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
*******************************************************************************/
static HBOOL INTRN_FAI_BLUETOOTH_Compare_DevName(void)
{
	FAI_BLUETOOTH_MSG_t tBluetoothMsg;
	FVK_ERR_CODE eVkRet;
	int index, ret = 0;
	HBOOL bResult = FALSE;
	
	for(index = 0; index < g_uiDeviceCnt; index++)
	{
		ret = strcmp(gBluetooth_DeviceName, g_sCbEvtDevInfo.stBtDevList[index].ucName);
		if(ret)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_BLUETOOTH_Compare_DevName] : Can not found device in list!!! (%d)\n", ret));
		}
		else
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_BLUETOOTH_Compare_DevName] : Found device in list !!! (%d)\n", ret));
			bResult = TRUE;
			break;
		}
	}

	return bResult;
}

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
*******************************************************************************/
FAI_ERR_CODE FAI_BLUETOOTH_Init(void)
{
	FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_Init] : ++\n"));

	if( FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_BLUETOOTH_MSG_t), (char*)"BluetoothMsg", &g_ulBluetoothMsgId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_Init] : FVK_MSG_Create \n"));
		return FVK_ERR;
	}
	if( FVK_TASK_Create(INTRN_FAI_BLUETOOTH_Task, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "Bluetooth Task", NULL, &g_ulBluetoothTaskId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_Init] : FVK_TASK_Create Error!!\n"));
		return FVK_ERR;
	}
	if( FVK_TASK_Start(g_ulBluetoothTaskId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_Init] : FVK_TASK_Start Error!!\n"));
		return FVK_ERR;
	}

	(void)FDI_BT_RegisterCallback(INTRN_FAI_BLUETOOTH_Callback);
	
	s_nBtStatus = FAI_BLUETOOTH_STATUS_DISABLE;

	gBluetoothlen = 0;
	memset(gBluetooth_DeviceName,0,BT_NB_NAME_LEN);
	FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_Init] : --\n"));

	return FAI_NO_ERR;
}

FAI_ERR_CODE FAI_BLUETOOTH_TEST_TMD(unsigned short usMsgId, unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	FDI_ERR_CODE eFdiErr;
	int i=0;
	unsigned char ucTmdErrCode[2];

	g_ulMSGId=usMsgId;

	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_BLUETOOTH_TEST_TMD] : ++\n"));

	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		tTMDResult.ucCategory = 0x10;
		tTMDResult.ucModule 	= 0x0E;
		tTMDResult.ucAction 	= 0x001;
		tTMDResult.usMsgId	= usMsgId;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_ETHERNET;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

	if(s_nBtStatus < FAI_BLUETOOTH_STATUS_INSERTED)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_TEST_TMD] : Error: s_nBtStatus < FAI_BLUETOOTH_STATUS_INSERTED \n"));
		goto TEST_FAIL;
	}
	
	if(bBsaServerOpened != TRUE)
	{
		FDI_BT_Enable();
		for(i=0; i<5; i++)
		{
			if(s_nBtStatus == FAI_BLUETOOTH_STATUS_READY)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_Test_UI] : FDI_BT_Enable() OK !!! \n"));
				bBsaServerOpened = TRUE;
				break;
			}	
			VK_TASK_Sleep(1000);
		}
		
		if(i == 5)
		{
			s_nBtStatus = FAI_BLUETOOTH_STATUS_INSERTED;

			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_Test_UI] : Error: FDI_BT_Enable() \n"));

			goto TEST_FAIL;	
		}
	}

	s_nRetryCount = 0;	

	FAI_TMD_StartTest(0);

	if((ucArgc == 0)||(pucArgList == NULL))
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_TEST_TMD] : Error: Don't get bluetoothname!!! \n"));
		goto TEST_FAIL;
	}

	gBluetoothlen = 0;
	memset(gBluetooth_DeviceName,0,BT_NB_NAME_LEN);

	FAI_TMD_GetParam(gBluetooth_DeviceName, pucArgList,0);
	gBluetoothlen = strlen(gBluetooth_DeviceName);

	FAI_PRINT(0, ("[FAI_BLUETOOTH_TEST_TMD] : Device Name(%s)(%d) ==\r\n",gBluetooth_DeviceName,gBluetoothlen));

	eFdiErr = FDI_BT_StartScan();
	if(eFdiErr != FDI_NO_ERR)
	{
		s_nBtStatus = FAI_BLUETOOTH_STATUS_INSERTED;
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_TEST_TMD] : Error: FDI_BT_StartScan() \n"));
		goto TEST_FAIL;
	}
	else
	{
		s_nBtStatus = FAI_BLUETOOTH_STATUS_PENDING;
	}

	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_BLUETOOTH_TEST_TMD] : --\n"));
	
	return FAI_NO_ERR;
TEST_FAIL:
	tTMDResult.ucCategory = 0x10;
	tTMDResult.ucModule 	= 0x0E;
	tTMDResult.ucAction 	= 0x001;
	tTMDResult.usMsgId	= usMsgId;
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;
	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_ETHERNET;
	ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_ERR;
}

FAI_ERR_CODE FAI_BLUETOOTH_GetResult_TMD(unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char aucResult[50];
	
	FAI_TMD_StartTest(0);

	if(s_nBtStatus == FAI_BLUETOOTH_STATUS_PASS)
	{
		/* result for tmd */
		aucResult[0] = 0x01;
		aucResult[1] = 0x01;

		aucResult[2] = gBluetoothlen;
		sprintf(&aucResult[3], "%s", gBluetooth_DeviceName);
		
		FAI_TMD_SetTestResult(0x10, 0x0F, 0x02, 0, FAI_TMD_TEST_NO_ERROR, 2, aucResult, 2+(aucResult[2]+1));
	}
	else if((s_nBtStatus == FAI_BLUETOOTH_STATUS_FAIL)||(s_nBtStatus == FAI_BLUETOOTH_STATUS_DISABLE))
	{
		/* result for tmd */
		aucResult[0] = 0x01;
		aucResult[1] = 0x00;
		
		/* result for tmd */
		aucResult[2] = strlen(FAI_BLUETOOTH_DEFAULT_DEVICENAME);
		sprintf(&aucResult[3], "%s", FAI_BLUETOOTH_DEFAULT_DEVICENAME);

		aucResult[2+(aucResult[2]+1)] = 2;	/* length of param */
		aucResult[2+(aucResult[2]+1)+1] = (FAI_TMD_DEFAULT_ERROR_CODE>>8) & 0xFF;	/* result - error code high */
		aucResult[2+(aucResult[2]+1)+2] = (FAI_TMD_DEFAULT_ERROR_CODE) & 0xFF;		/* result - error code low */
			
		FAI_TMD_SetTestResult(0x10, 0x0F, 0x02, 0, FAI_TMD_TEST_ERROR, 3, aucResult, 2+(aucResult[2]+1)+FAI_TMD_ERROR_PARM_LENGTH);
	}	
	else if(s_nBtStatus == FAI_BLUETOOTH_STATUS_PENDING)
	{
		FAI_TMD_SetTestResult(0x10, 0x0F, 0x02, 0, FAI_TMD_TEST_PENDING, 0, NULL, 0);
	}

	return FAI_NO_ERR;
}

#if (BLUETOOTH_UI_ENABLE == YES)
FAI_ERR_CODE FAI_BLUETOOTH_Test_UI(void)
{
	FDI_ERR_CODE eFdiErr;
	int timeout = (FAI_BLUETOOTH_UI_TIMEOUT_SECONDS * 1000) / 100;
	int i=0;
	
	/* Display Bluetooth test start */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "BLUETOOTH", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Test", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_BLUETOOTH_Test_UI] : ++\n"));

	if (s_nBtStatus < FAI_BLUETOOTH_STATUS_INSERTED)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_Test_UI] : Error: s_nBtStatus < FAI_BLUETOOTH_STATUS_INSERTED \n"));
		goto TEST_FAIL;
	}
	
	if(bBsaServerOpened != TRUE)
	{
		FDI_BT_Enable();
		for(i=0; i<5; i++)
		{
			if(s_nBtStatus == FAI_BLUETOOTH_STATUS_READY)
			{
				FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_Test_UI] : FDI_BT_Enable() OK !!! \n"));
				bBsaServerOpened = TRUE;
				break;
			}	
			VK_TASK_Sleep(1000);
		}
		
		if(i == 5)
		{
			s_nBtStatus = FAI_BLUETOOTH_STATUS_INSERTED;

			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_Test_UI] : Error: FDI_BT_Enable() \n"));

			goto TEST_FAIL;	
		}
	}

	s_nRetryCount = 0;	

	gBluetoothlen = 0;
	VK_MEM_Memset(gBluetooth_DeviceName, 0, BT_NB_NAME_LEN);
	VK_MEM_Memcpy(gBluetooth_DeviceName, FAI_BLUETOOTH_DEFAULT_DEVICENAME_UI, strlen(FAI_BLUETOOTH_DEFAULT_DEVICENAME_UI));
	gBluetoothlen = strlen(gBluetooth_DeviceName);

	FAI_PRINT(0, ("[FAI_BLUETOOTH_Test_UI] : Device Name(%s)(%d) ==\r\n", gBluetooth_DeviceName, gBluetoothlen));

	eFdiErr = FDI_BT_StartScan();
	if (eFdiErr != FDI_NO_ERR)
	{
		s_nBtStatus = FAI_BLUETOOTH_STATUS_INSERTED;

		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_Test_UI] : Error: FDI_BT_StartScan() \n"));

		goto TEST_FAIL;
	}
	else
	{
		s_nBtStatus = FAI_BLUETOOTH_STATUS_PENDING;
	}

	while (s_nBtStatus == FAI_BLUETOOTH_STATUS_PENDING)
	{
		VK_TASK_Sleep(100);	/* 100 ms */

		timeout--;

		if (timeout == 0)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_BLUETOOTH_Test_UI] : Timeout!!! \n"));

			goto TEST_FAIL;
		}
	}

	if (s_nBtStatus == FAI_BLUETOOTH_STATUS_PASS)
	{
		FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "BLUETOOTH", RGB_COLOR_BLACK);
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK); 
//		FDI_PANEL_Display(FAI_BT_TEST_PASS_STR);
		FDI_PANEL_Display("BT PASS");
	}
	else
	{
		goto TEST_FAIL;
	}

	return FAI_NO_ERR;

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "BLUETOOTH", RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
//	FDI_PANEL_Display(FAI_BT_TEST_FAIL_STR);
	FDI_PANEL_Display("BT FAIL");

	return FAI_NO_ERR;
}
#endif

