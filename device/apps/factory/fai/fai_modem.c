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

/* humax header files */
#include "fvk.h"
#include "taskdef.h"

#include "fai_common.h"
#include "fai_tmd.h"
#include "fai_testmenu.h"

/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define MODEM_TX_DATA	"HUMAX-MODEM-TEST"
#define MODEM_TX_DATA_LEN	20
#define MODEM_BUFFER_SIZE	40
#define MODEM_CALLING_TIMEOUT	30000	/*ms */
#define MODEM_RESPONSE_CONNECTED	0x31	/*1*/

#define MODEM_TEST_TYPE_UI	0x01
#define MODEM_TEST_TYPE_TMD	0x02

#define MODEM_MESSAGE_CHECK_VOLTAGE	1
#define MODEM_MESSAGE_SEND_DATA	2
#define MODEM_MESSAGE_CALL_21	3
#define MODEM_MESSAGE_CALL_23	4
#define MODEM_MESSAGE_CALL_TMD	5
#define MODEM_MESSAGE_END_CALL	6

#define MODEM_ACTION_ITEM_CHECK_VOLTAGE	0x01
#define MODEM_ACTION_ITEM_COMMUNICATION	0x03

typedef struct FAI_MODEM_MSG
{
	unsigned char ucMessage;
	unsigned char ucTestType;
	unsigned long ulDialNumber;
	FAI_TESTMENU_H hMenu;
} MODEM_MSG_T;

typedef enum
{
	FDI_MODEM_DISCONNECT = 0,
	FDI_MODEM_INIT_OK,
	FDI_MODEM_CALLING,
	FDI_MODEM_CONNECTED,
}MODEM_STATUS_e;


/*******************************************************************************
* External variables and functions
*******************************************************************************/


/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* Static variables and structures
*******************************************************************************/
static unsigned long s_ulModemMsgID;
static unsigned long s_ulModemTimerID;
static unsigned char s_aucModemTXData[MODEM_TX_DATA_LEN] = {'\0',};
static unsigned char s_ucCurrentMessage = 0;
static MODEM_STATUS_e s_ucModemCommState = FDI_MODEM_DISCONNECT;
static MODEM_MSG_T s_stModemMsg;
static unsigned char s_ucReceivedData[MODEM_BUFFER_SIZE]={'\0',};

static FAI_ERR_CODE INTRN_FAI_MODEM_VoltageCheck(unsigned char *pucVoltageStr, unsigned long *pulVoltLevel);
static void INTRN_FAI_MODEM_CheckDataReceive(unsigned int unLength);
static void INTRN_FAI_MODEM_Task(void *data);
static FVK_ERR_CODE INTRN_FAI_MODEM_Timer(unsigned long ulTimerID, void *pucArg);


/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_MODEM_Init(void)
{
	unsigned long ulModemTaskID;

	sprintf(s_aucModemTXData, "%s", MODEM_TX_DATA);
	
	if( FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(MODEM_MSG_T), (char*)"MODEM_MSG", &s_ulModemMsgID) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MODEM_Task] : FVK_MSG_Create Error!!\n"));
		return FAI_ERR;
	}
	
	if( FVK_TASK_Create(INTRN_FAI_MODEM_Task, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "ModemTask", NULL, &ulModemTaskID) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MODEM_Task] : FVK_TASK_Create Error!!\n"));
		return FAI_ERR;
	}

	FDI_MODEM_RegisterCallback(INTRN_FAI_MODEM_CheckDataReceive);
	
	if( FVK_TASK_Start(ulModemTaskID) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MODEM_Task] : FVK_TASK_Start Error!!\n"));
		return FAI_ERR;
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (MODEM_LINE_VOLT_ENABLE==YES)
FAI_ERR_CODE FAI_MODEM_VoltageCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
    FVK_ERR_CODE eFvkError = FVK_NO_ERR;
    FAI_ERR_CODE eFaiError = FAI_NO_ERR;
    MODEM_MSG_T tModemMsg;
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[%s] : Argument Count Error=%d\n",  __func__, ucArgc));
		
		tTMDResult.ucCategory = FAI_CATEGORY_PERIPHERAL;
		tTMDResult.ucModule = 2;
		tTMDResult.ucAction = 1;

		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		FAI_TMD_SetResult(&tTMDResult);
		return FAI_ERR;
	}

    /* Send msg */
    FAI_TESTMENU_GetCurrentMenu(&tModemMsg.hMenu);
    tModemMsg.ucMessage = MODEM_MESSAGE_CHECK_VOLTAGE;
    tModemMsg.ucTestType = MODEM_TEST_TYPE_TMD;
    eFvkError = FVK_MSG_Send(s_ulModemMsgID, &tModemMsg, sizeof(MODEM_MSG_T));
    if (eFvkError != FVK_NO_ERR)
    {
        FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MODEM_VoltageCheck] : Error FVK_MSG_Send() : %d\n", eFvkError));
		
		tTMDResult.ucCategory = FAI_CATEGORY_PERIPHERAL;
		tTMDResult.ucModule = 2;
		tTMDResult.ucAction = 1;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	

		FAI_TMD_SetResult(&tTMDResult);

        return FAI_ERR;
    }

    return eFaiError;
}
#endif

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (MODEM_LINE_VOLT_UI_ENABLE==YES)
FAI_ERR_CODE FAI_MODEM_VoltageCheck_UI(void)
{
	FVK_ERR_CODE	eFvkError = FVK_NO_ERR;
	FAI_ERR_CODE	eFaiError = FAI_NO_ERR;
	MODEM_MSG_T		tModemMsg;
	
	/* Display modem voltage test start */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Modem Voltage", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	/* Send msg */
	FAI_TESTMENU_GetCurrentMenu(&tModemMsg.hMenu);
	tModemMsg.ucMessage = MODEM_MESSAGE_CHECK_VOLTAGE;
	tModemMsg.ucTestType = MODEM_TEST_TYPE_UI;
	eFvkError = FVK_MSG_Send(s_ulModemMsgID, &tModemMsg, sizeof(MODEM_MSG_T));
	if (eFvkError != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MODEM_StartCallTest] : Error FVK_MSG_Send() : %d\n", eFvkError));
		eFaiError = FAI_ERR;
	}
	
	return eFaiError;
}
#endif

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (MODEM_CALL_ENABLE==YES)
FAI_ERR_CODE FAI_MODEM_Communication(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
    FAI_ERR_CODE	eFaiError = FAI_NO_ERR;
    FVK_ERR_CODE	eFvkError = FVK_NO_ERR;
    unsigned char	ucDialNumber = 0;
    FAI_TMD_RESULT_T tTMDResult;
    FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
    FDI_ERR_CODE eFdiErr;
	unsigned char ucTmdErrCode[2];

    FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
    /* 1. parse parameter */
    if( ucArgc != 1 )
    {
        eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					
        goto END;
    }
    FAI_TMD_GetParam(&ucDialNumber, pucArgList, 0);

    /* 2. Send msg */
    FAI_TESTMENU_GetCurrentMenu(&s_stModemMsg.hMenu);
    s_stModemMsg.ucMessage = ucDialNumber;
    eFvkError = FVK_MSG_Send(s_ulModemMsgID, &s_stModemMsg, sizeof(MODEM_MSG_T));
    if (eFvkError != FVK_NO_ERR)
    {
        FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MODEM_StartCallTest] : Error FVK_MSG_Send() : %d\n", eFvkError));
        eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_MESSAGE_SEND_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));			
        goto END;
    }

    /* 3. start connection timer */
    if(s_ulModemTimerID != 0)
    {
        FDI_MODEM_Disconnect();
        FVK_TIMER_Destory(s_ulModemTimerID);
        s_ulModemTimerID = 0;
        FVK_MEM_Set(&s_stModemMsg, 0x00, sizeof(s_stModemMsg));
    }
    FVK_TIMER_CreateStart(MODEM_CALLING_TIMEOUT, INTRN_FAI_MODEM_Timer, &s_stModemMsg, sizeof(s_stModemMsg), FVK_TIMER_ONCE, &s_ulModemTimerID);

    return FAI_NO_ERR;
    
END:
    tTMDResult.eResult = eTmdErr;
	tTMDResult.ucCategory = CATEGORY_PERIPHERAL;
	tTMDResult.ucModule = 2;
	tTMDResult.ucAction = 2;
    FVK_MEM_Copy(&tTMDResult.aucParams, &s_ucReceivedData, sizeof(s_ucReceivedData));        
    FAI_TMD_SetResult(&tTMDResult);
    return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (MODEM_CALL_21_UI_ENABLE==YES) || (MODEM_CALL_23_UI_ENABLE==YES)
FAI_ERR_CODE FAI_MODEM_Communication_UI(void)
{
	FAI_ERR_CODE	eFaiError = FAI_NO_ERR;
	FVK_ERR_CODE	eFvkError = FVK_NO_ERR;
	unsigned long ulRcuKey;
	
	FAI_FUNC_ENTER;
	
	/* Display modem test start */
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	ulRcuKey = FAI_APP_GetCurrentRcuKey();
	if( ulRcuKey == MODEM_CALL_21_UI_INPUT )
	{
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "MODEM 21", RGB_COLOR_YELLOW);
		s_stModemMsg.ucMessage = MODEM_MESSAGE_CALL_21;
	}
	else if( ulRcuKey == MODEM_CALL_23_UI_INPUT )
	{
		FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "MODEM 23", RGB_COLOR_YELLOW);
		s_stModemMsg.ucMessage = MODEM_MESSAGE_CALL_23;
	}

	/* Send msg */
	FAI_TESTMENU_GetCurrentMenu(&s_stModemMsg.hMenu);
	eFvkError = FVK_MSG_Send(s_ulModemMsgID, &s_stModemMsg, sizeof(MODEM_MSG_T));
	if (eFvkError != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MODEM_Communication_UI] : Error FVK_MSG_Send() : %d\n", eFvkError));
		eFaiError = FAI_ERR;
	}

	/* start connection timer */
	if(s_ulModemTimerID != 0)
	{
		FDI_MODEM_Disconnect();
		FVK_TIMER_Destory(s_ulModemTimerID);
		s_ulModemTimerID = 0;
		FVK_MEM_Set(&s_stModemMsg, 0x00, sizeof(s_stModemMsg));
	}
	FVK_TIMER_CreateStart(MODEM_CALLING_TIMEOUT, INTRN_FAI_MODEM_Timer, &s_stModemMsg, sizeof(s_stModemMsg), FVK_TIMER_ONCE, &s_ulModemTimerID);

	FAI_FUNC_EXIT;
	return eFaiError;
}
#endif

/*******************************************************************************
* function : INTRN_FAI_MODEM_VoltageCheck
* description : CHECK VOLTAGE 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FAI_ERR_CODE INTRN_FAI_MODEM_VoltageCheck(unsigned char *pucVoltageStr, unsigned long *pulVoltLevel)
{
	unsigned int i = 0, volt = 0, unResponseLen = 0;
	unsigned char aucResponseBuf[20] = {'\0',};
	
	/* Send "AT-TRV" Command */
	if( FDI_MODEM_TxCommand("AT-TRV") != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MODEM_CheckVoltage] : FDI_MODEM_TxCommand\n"));
		return FAI_ERR;
	}
	
	/* Receive ack */
	if( FDI_MODEM_RxResponse(aucResponseBuf, &unResponseLen, 500) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MODEM_CheckVoltage] : FDI_MODEM_RxResponse Error\n"));		
		return FAI_ERR;
	}
	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_MODEM_CheckVoltage] : string: %s length :%d\n", aucResponseBuf, unResponseLen));
		
	/* Verify voltage */
	for( i = 0; i < unResponseLen; i++ )
	{
		if( (aucResponseBuf[i] >= '0') && (aucResponseBuf[i] <= '9') )
		{
			volt = volt*10 + (aucResponseBuf[i]-'0');
		}
		else if( aucResponseBuf[i] == '.' )
		{
			break;
		}
	}

	FVK_MEM_Set(aucResponseBuf, '\0', sizeof(aucResponseBuf));
	sprintf(pucVoltageStr, "%dV", volt);
	*pulVoltLevel = volt;

	if(volt < 20)
	{
		return FAI_ERR;
	}
	
	return FAI_NO_ERR;
}


/*******************************************************************************
* function : INTRN_FAI_MODEM_CheckDataReceive
* description : communication test 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void INTRN_FAI_MODEM_CheckDataReceive(unsigned int unLength)
{
	FVK_ERR_CODE	eFvkError = FVK_NO_ERR;
	MODEM_MSG_T		tModemMsg;
	unsigned char	ucData = 0;
	static unsigned char	ucPreData = 0;
	
	if(s_ucModemCommState == FDI_MODEM_CALLING)
	{
		FDI_MODEM_RxData(&ucData, 1);
		if(ucData == MODEM_RESPONSE_CONNECTED)
		{
			ucPreData = ucData;
		}
		else if(ucData == 0xd)
		{
			if(ucPreData == MODEM_RESPONSE_CONNECTED)
			{
				/* Send msg */
				tModemMsg.hMenu = s_stModemMsg.hMenu;
				tModemMsg.ucMessage = MODEM_MESSAGE_SEND_DATA;
				eFvkError = FVK_MSG_Send(s_ulModemMsgID, &tModemMsg, sizeof(MODEM_MSG_T));
				if (eFvkError != FVK_NO_ERR)
				{
					FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MODEM_StartCallTest] : Error FVK_MSG_Send() : %d\n", eFvkError));
				}	
			}
			ucPreData = 0x0;
		}
	}
}

/*******************************************************************************
* function : INTRN_FAI_MODEM_Task
* description : communication test 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static void INTRN_FAI_MODEM_Task(void *data)
{
    FAI_ERR_CODE	eFaiError = FAI_NO_ERR;
    FDI_ERR_CODE	eFdiError = FDI_NO_ERR;
    MODEM_MSG_T tModemMsg;
    FVK_ERR_CODE eVkRet;
    unsigned char aucData[MODEM_BUFFER_SIZE]={'\0',};
    unsigned char aucVoltageString[10]={'\0',};
    unsigned char ucIndex = 0;
    unsigned long ulVoltage = 0;
    FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];

    s_ucModemCommState = FDI_MODEM_INIT_OK;

    FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
    
    while(1)
    {	
            eVkRet = FVK_MSG_Receive(s_ulModemMsgID, &tModemMsg, sizeof(MODEM_MSG_T));
        if (eVkRet != FVK_NO_ERR)
        {
            FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_MODEM_Task] : Error FVK_MSG_Receive() %d\n", eVkRet));
            continue;
        }

        if(tModemMsg.ucMessage == MODEM_MESSAGE_CHECK_VOLTAGE)
        {
            eFaiError = INTRN_FAI_MODEM_VoltageCheck(aucVoltageString, &ulVoltage);
            if(tModemMsg.ucTestType == MODEM_TEST_TYPE_UI)
            {
                if(eFaiError == FAI_NO_ERR)
                {
                    FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_BLACK);
                    FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Modem Voltage", RGB_COLOR_BLACK);
                    FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);
                    FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, aucVoltageString, RGB_COLOR_BLACK);
                    FDI_PANEL_Display(FAI_MODEM_VOLTAGE_PASS);
                }
                else
                {
                    FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
                    FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "Modem Voltage", RGB_COLOR_RED);
                    FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);	
                    FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, aucVoltageString, RGB_COLOR_RED);	
                    FDI_PANEL_Display(FAI_MODEM_VOLTAGE_FAIL);
                }
            }
            else if(tModemMsg.ucTestType == MODEM_TEST_TYPE_TMD)
            {
                if(eFaiError == FAI_NO_ERR)
                {
                    tTMDResult.eResult = FAI_TMD_ERR_OK;
                }
                else
                {
                    tTMDResult.eResult = FAI_TMD_ERR_FAIL;
					ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_MODEM;
					ucTmdErrCode[1] = FAI_MODEM_TMD_ERR_VOLTAGE_FAIL;
					FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));			
                }
                tTMDResult.ucAction = MODEM_ACTION_ITEM_CHECK_VOLTAGE;
                FAI_TMD_SetResult(&tTMDResult);
            }
            continue;
        }
        else if((tModemMsg.ucMessage >= MODEM_MESSAGE_CALL_21) && (tModemMsg.ucMessage <= MODEM_MESSAGE_CALL_TMD))
        {
            s_ucCurrentMessage = tModemMsg.ucMessage;

            /* Dialup number */
            sprintf(aucData, "%d", tModemMsg.ulDialNumber);
            FAI_PRINT(FAI_PRT_DBGINFO, ("[INTRN_FAI_MODEM_Task] : MODEM_MSG_TEST_PORT %d \n", tModemMsg.ucMessage));
            FVK_TASK_Sleep(1000);

            /* 1. dialing to port number */
            if( FDI_MODEM_Connect(aucData) != FDI_NO_ERR )
            {
                FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MODEM_Task] : FDI_MODEM_Connect Error!!\n"));
                tTMDResult.ucAction = MODEM_ACTION_ITEM_COMMUNICATION;
                tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_MODEM;
				ucTmdErrCode[1] = FAI_MODEM_TMD_ERR_CALL_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					
                goto error;
            }
            s_ucModemCommState = FDI_MODEM_CALLING;
            continue;
        }
        else if(tModemMsg.ucMessage == MODEM_MESSAGE_SEND_DATA)
        {
            s_ucModemCommState = FDI_MODEM_CONNECTED;
            FDI_MODEM_FlushRxBuffer();

            /* 2. Send specific data */
            if( FDI_MODEM_TxData(s_aucModemTXData, strlen(s_aucModemTXData)) != FDI_NO_ERR )
            {
                FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MODEM_Task] : FDI_MODEM_TxData Error!!\n"));
                tTMDResult.ucAction = MODEM_ACTION_ITEM_COMMUNICATION;
                tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_MODEM;
				ucTmdErrCode[1] = FAI_MODEM_TMD_ERR_COMMUNICATION_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					
				
                goto error;
            }

            /* 3. wait to receive data during 1s */
            FVK_MEM_Set(s_ucReceivedData, 0x00, sizeof(s_ucReceivedData));
            FVK_TASK_Sleep(1000);
            if( FDI_MODEM_RxData(s_ucReceivedData, strlen(s_aucModemTXData)) != FDI_NO_ERR )
            {
                FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MODEM_Task] : FDI_MODEM_TxData Error!!\n"));
                tTMDResult.eResult = FAI_TMD_ERR_FAIL;
				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_MODEM;
				ucTmdErrCode[1] = FAI_MODEM_TMD_ERR_COMMUNICATION_FAIL;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					
				
                goto error;
            }
        }
        else if(tModemMsg.ucMessage == MODEM_MESSAGE_END_CALL)
        {
            FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MODEM_Task] :time over for connection!! \n"));
            tTMDResult.ucAction = MODEM_ACTION_ITEM_COMMUNICATION;
            tTMDResult.eResult = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_MODEM;
			ucTmdErrCode[1] = FAI_MODEM_TMD_ERR_CALL_FAIL;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					
			
            goto error;
        }
#if 0 /* do not need to use */
        /* 4. verify received data */
        while(s_aucModemTXData[ucIndex] != '\0')
        {
            if(aucData[ucIndex] != s_aucModemTXData[ucIndex])
            {
                FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MODEM_Task] : Verification Error!!\n"));
                tTMDResult.ucAction = MODEM_ACTION_ITEM_COMMUNICATION;
                goto error;
            }
            ucIndex++;
        }
#endif
        FAI_TESTMENU_ChangeMenuColor(tModemMsg.hMenu, RGB_COLOR_BLACK);
        if(s_ucCurrentMessage == MODEM_MESSAGE_CALL_21)
        {
            FAI_TESTMENU_ChangeMenu(tModemMsg.hMenu, TESTMENU_LINE_2, "MODEM 21", RGB_COLOR_BLACK);
            FDI_PANEL_Display(FAI_MODEM_COMM_21_PASS);
        }
        else
        {
            FAI_TESTMENU_ChangeMenu(tModemMsg.hMenu, TESTMENU_LINE_2, "MODEM 23", RGB_COLOR_BLACK);
            FDI_PANEL_Display(FAI_MODEM_COMM_23_PASS);
        }
        FAI_TESTMENU_ChangeMenu(tModemMsg.hMenu, TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);	

        s_ucModemCommState = FDI_MODEM_INIT_OK;
        FDI_MODEM_Disconnect();
        FVK_TIMER_Destory(s_ulModemTimerID);
        s_ulModemTimerID = 0;
        FVK_MEM_Set(&s_stModemMsg, 0x00, sizeof(s_stModemMsg));
        tTMDResult.ucCategory = CATEGORY_PERIPHERAL;
        tTMDResult.ucModule = MODULE_PERIPHERAL_MODEM;
        tTMDResult.eResult = FAI_TMD_ERR_OK;
        FVK_MEM_Copy(&tTMDResult.aucParams, &s_ucReceivedData, sizeof(s_ucReceivedData));
        FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
        FAI_TMD_SetResult(&tTMDResult);
        continue;

        error:
            FAI_TESTMENU_ChangeMenuColor(tModemMsg.hMenu, RGB_COLOR_RED);
            if(s_ucCurrentMessage == MODEM_MESSAGE_CALL_21)
            {
                FAI_TESTMENU_ChangeMenu(tModemMsg.hMenu, TESTMENU_LINE_2, "MODEM 21", RGB_COLOR_RED);
                FDI_PANEL_Display(FAI_MODEM_COMM_21_FAIL);
            }
            else
            {
                FAI_TESTMENU_ChangeMenu(tModemMsg.hMenu, TESTMENU_LINE_2, "MODEM 23", RGB_COLOR_RED);
                FDI_PANEL_Display(FAI_MODEM_COMM_23_FAIL);
            }
            FAI_TESTMENU_ChangeMenu(tModemMsg.hMenu, TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);	

            s_ucModemCommState = FDI_MODEM_INIT_OK;
            FDI_MODEM_Disconnect();
            FVK_TIMER_Destory(s_ulModemTimerID);
            s_ulModemTimerID = 0;
            FVK_MEM_Set(&s_stModemMsg, 0x00, sizeof(s_stModemMsg));
            tTMDResult.ucCategory = CATEGORY_PERIPHERAL;
            tTMDResult.ucModule = MODULE_PERIPHERAL_MODEM;
            tTMDResult.eResult = FAI_TMD_ERR_FAIL;
            FAI_TMD_SetResult(&tTMDResult);
    }
}

/*******************************************************************************
* function : INTRN_FAI_MODEM_Timer
* description : check valid time for connection 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
static FVK_ERR_CODE INTRN_FAI_MODEM_Timer(unsigned long ulTimerID, void *pucArg)
{
	FVK_ERR_CODE	eFvkError = FVK_NO_ERR;
	MODEM_MSG_T		*ptModemMsg = (MODEM_MSG_T *)pucArg;

	if(s_ucModemCommState == FDI_MODEM_CALLING)
	{
		ptModemMsg->ucMessage = MODEM_MESSAGE_END_CALL;
		eFvkError = FVK_MSG_Send(s_ulModemMsgID, ptModemMsg, sizeof(MODEM_MSG_T));
		if (eFvkError != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_MODEM_Timer] : Error FVK_MSG_Send() : %d\n", eFvkError));
		}	
	}

	return FVK_OK;
}

