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
#include "fvk.h"

#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_tmd.h"
#include "fai_fan.h"

#include "fdi_panel.h"
#include "fdi_fan.h"

/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FAI_FAN_RETRY_NUM	5
#define FAI_FAN_PASS_NUM	3

typedef enum
{
	FAI_FAN_TEST_TMD, 
	FAI_FAN_TEST_UI
} FAI_FAN_TEST_TYPE;

typedef struct FAI_FAN_MSG
{
	FAI_FAN_TEST_TYPE eTestType;
	FDI_FAN_CTRL_e eFanCtrl;
	unsigned char ucFanSpeedMin;
	unsigned char ucFanSpeedMax;
} FAI_FAN_MSG_t;

typedef void (*FAI_FAN_ProgressFunc_t)(unsigned char ucProgressRate);

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
FAI_TESTMENU_H hFanMenu;
unsigned long g_ulFanMsgId, g_ulFanTaskId;
BOOL g_bFanState;

FAI_ERR_CODE INTRN_FAI_FAN_Control(FDI_FAN_CTRL_e eSpeed, 
									unsigned char *ucResultSpeed,
									FAI_FAN_ProgressFunc_t pfnFunc);
void INTRN_FAI_FAN_TestTask(void *data);
void INTRN_FAI_FAN_ProgressFunc(unsigned long ulFanFeedback);

/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FAN_Init(void)
{
	g_bFanState = FALSE;
	
	if( FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_FAN_MSG_t), (char*)"FanMsg", &g_ulFanMsgId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_APP_Init] : FVK_MSG_Create \n"));
		return FVK_ERR;
	}
	if( FVK_TASK_Create(INTRN_FAI_FAN_TestTask, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "Fan Task", NULL, &g_ulFanTaskId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_TASK_Create Error!!\n"));
		return FVK_ERR;
	}
	if( FVK_TASK_Start(g_ulFanTaskId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_TASK_Start Error!!\n"));
		return FVK_ERR;
	}	

	FDI_FAN_SpeedCtrl(FDI_FAN_CTRL_STOP);
	
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
#if (FAN_SPEED_CONTROL_ENABLE==YES)
FAI_ERR_CODE FAI_FAN_SpeedCtrl(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{

//--
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FDI_ERR_CODE eFdiErr;
	unsigned char ucTmdErrCode[2];

	
	FAI_FAN_MSG_t tFanMsg;
	FVK_ERR_CODE eVkRet;
	unsigned char ucFanSpeed, ucFanSpeedMin, ucFanSpeedMax;
	unsigned char aucTmdResult[7];
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FAN_SpeedCtrl] : Argument Count Error=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					
		goto END;
	}
	
	FAI_TMD_GetParam(&ucFanSpeed, pucArgList, 0);

	tFanMsg.eTestType = FAI_FAN_TEST_TMD;
	tFanMsg.eFanCtrl = ucFanSpeed;
	tFanMsg.ucFanSpeedMin = 0;
	tFanMsg.ucFanSpeedMax = 0;
	eVkRet = FVK_MSG_Send(g_ulFanMsgId, &tFanMsg, sizeof(FAI_FAN_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MODEM_StartCallTest] : Error FVK_MSG_Send() : %d\n", eVkRet));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_FAN;
		ucTmdErrCode[1] = FAI_FAN_TMD_ERR_GET_SPEED_FAIL;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					
		goto END;
	}

	return FAI_NO_ERR;

END:
	tTMDResult.ucCategory	= CATEGORY_PERIPHERAL;
	tTMDResult.ucModule = MODULE_PERIPHERAL_FAN;
	tTMDResult.ucAction = 0x01;

	tTMDResult.eResult= eTmdErr;
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
#if (FAN_UI_ENABLE==YES)
FAI_ERR_CODE FAI_FAN_OnOff_UI(void)
{
	FAI_FAN_MSG_t tFanMsg;
	FVK_ERR_CODE eVkRet;


	/* Start Fan Test */
	tFanMsg.eTestType = FAI_FAN_TEST_UI;
	tFanMsg.eFanCtrl = FDI_FAN_CTRL_STOP;

	FAI_TESTMENU_GetCurrentMenu(&hFanMenu);
	eVkRet = FVK_MSG_Send(g_ulFanMsgId, &tFanMsg, sizeof(FAI_FAN_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MODEM_StartCallTest] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return FAI_ERR;
	}
	
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
void INTRN_FAI_FAN_TestTask(void *data)
{

	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;


	
	FAI_FAN_MSG_t tFanMsg;
	FVK_ERR_CODE eVkRet;
	int i;
	unsigned char ucSpeedStr[10];
	unsigned char aucTmdResult[7], ucFeedback;
	unsigned char ucSpeedMinMax[10];	
	
	while(1)
	{
		eVkRet = FVK_MSG_Receive(g_ulFanMsgId, &tFanMsg, sizeof(FAI_FAN_MSG_t));
		if (eVkRet != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_FAN_TestTask] : Error FVK_MSG_Receive() %d\n", eVkRet));
			continue;
		}
		
		memset(ucSpeedMinMax, 0x00, 10);
		sprintf(ucSpeedMinMax, "%d~%d", FAI_UI_SPEED_MIN, FAI_UI_SPEED_MAX);
		
		if( tFanMsg.eTestType == FAI_FAN_TEST_UI )
		{
			/* display check message */
			FDI_PANEL_Display("FAN Test");

			/* 1nd : Check Low Speed */
			tFanMsg.eFanCtrl = FDI_FAN_CTRL_LOW;

			FAI_TESTMENU_ChangeMenu(hFanMenu, TESTMENU_LINE_1, "FAN On", RGB_COLOR_YELLOW);
			FAI_TESTMENU_ChangeMenu(hFanMenu, TESTMENU_LINE_2, ucSpeedMinMax, RGB_COLOR_YELLOW);
			FAI_TESTMENU_ChangeMenu(hFanMenu, TESTMENU_LINE_3, "FAN Off", RGB_COLOR_YELLOW);
			FAI_TESTMENU_ChangeMenu(hFanMenu, TESTMENU_LINE_4, "0~5hz", RGB_COLOR_YELLOW);
			FAI_TESTMENU_ChangeRCUColor(hFanMenu, RGB_COLOR_BLACK);

			INTRN_FAI_FAN_Control(tFanMsg.eFanCtrl, &ucFeedback, INTRN_FAI_FAN_ProgressFunc);
			if( FAI_UI_SPEED_MIN <= ucFeedback && FAI_UI_SPEED_MAX >= ucFeedback )
			{
				sprintf(ucSpeedStr, "Pass (%d)", ucFeedback);
			
				//FAI_TESTMENU_ChangeMenuColor(hFanMenu, RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(hFanMenu, TESTMENU_LINE_1, "FAN ON", RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(hFanMenu, TESTMENU_LINE_2, ucSpeedStr, RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeRCUColor(hFanMenu, RGB_COLOR_BLACK);
				FDI_PANEL_Display(FAI_FAN_ON_PASS_STR);
			}
			else
			{
				sprintf(ucSpeedStr, "Fail (%d)", ucFeedback);

				//FAI_TESTMENU_ChangeMenuColor(hFanMenu, RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(hFanMenu, TESTMENU_LINE_1, "FAN ON", RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(hFanMenu, TESTMENU_LINE_2, ucSpeedStr, RGB_COLOR_RED);
				FAI_TESTMENU_ChangeRCUColor(hFanMenu, RGB_COLOR_BLACK);
				FDI_PANEL_Display(FAI_FAN_ON_FAIL_STR);
			}

			/* 2st : Check STOP condition */
			tFanMsg.eFanCtrl = FDI_FAN_CTRL_STOP;

			//FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
//			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_3, "FAN Off", RGB_COLOR_YELLOW);
//			FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_4, "0~5hz", RGB_COLOR_YELLOW);
				
			INTRN_FAI_FAN_Control(tFanMsg.eFanCtrl, &ucFeedback, INTRN_FAI_FAN_ProgressFunc);
			if( ucFeedback == 0 )
			{
				sprintf(ucSpeedStr, "Pass (%d)", ucFeedback);

				//FAI_TESTMENU_ChangeMenuColor(hFanMenu, RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(hFanMenu, TESTMENU_LINE_3, "FAN OFF", RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(hFanMenu, TESTMENU_LINE_4, ucSpeedStr, RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeRCUColor(hFanMenu, RGB_COLOR_GREEN);					
				FDI_PANEL_Display(FAI_FAN_OFF_PASS_STR);
			}
			else
			{
				//FAI_TESTMENU_ChangeMenuColor(hFanMenu, RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(hFanMenu, TESTMENU_LINE_3, "FAN OFF", RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(hFanMenu, TESTMENU_LINE_4, "Fail", RGB_COLOR_RED);
				FAI_TESTMENU_ChangeRCUColor(hFanMenu, RGB_COLOR_GREEN);	
				FDI_PANEL_Display(FAI_FAN_OFF_FAIL_STR);
			}

			/* fan stop */
//			FDI_FAN_SpeedCtrl(FDI_FAN_CTRL_STOP);
			
		}
		else if( tFanMsg.eTestType == FAI_FAN_TEST_TMD )
		{
			{			
				INTRN_FAI_FAN_Control(tFanMsg.eFanCtrl, &ucFeedback, NULL);
				FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
					
				tTMDResult.ucCategory	= CATEGORY_PERIPHERAL;
				tTMDResult.ucModule	= MODULE_PERIPHERAL_FAN;
				tTMDResult.ucAction	= 0x01;
				tTMDResult.eResult	= FAI_TMD_ERR_OK;
				FAI_TMD_AddResultParam(&tTMDResult, &ucFeedback, sizeof(ucFeedback));
				
				FAI_TMD_SetResult(&tTMDResult);	
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
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE INTRN_FAI_FAN_Control(FDI_FAN_CTRL_e eSpeed, 
									unsigned char *ucResultSpeed,
									FAI_FAN_ProgressFunc_t pfnFunc)
{
	int i;
	unsigned long ulFanFeedback = 0, ulTotalFeedback = 0, ulAvgFeedback = 0;

	FDI_FAN_SpeedCtrl(eSpeed);

	FVK_TASK_Sleep(3000);

	if( eSpeed == FDI_FAN_CTRL_STOP )
	{
		FDI_FAN_GetSpeed(&ulFanFeedback);
		*ucResultSpeed = (unsigned char)ulFanFeedback;
	}
	else
	{
		for( i = 0; i < FAI_FAN_RETRY_NUM; i++ )
		{
			FDI_FAN_GetSpeed(&ulFanFeedback);
			if( pfnFunc !=NULL)
			{
				pfnFunc(ulFanFeedback);
			}

			if( i >= (FAI_FAN_RETRY_NUM-FAI_FAN_PASS_NUM) )	ulTotalFeedback += ulFanFeedback;
			
			FVK_TASK_Sleep(2000);
		}
		ulAvgFeedback = ulTotalFeedback/FAI_FAN_PASS_NUM;
		*ucResultSpeed = (unsigned char)ulAvgFeedback;
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
void INTRN_FAI_FAN_ProgressFunc(unsigned long ulFanFeedback)
{
	unsigned char ucSpeedStr[10];

	sprintf(ucSpeedStr, "(%d)", ulFanFeedback);
	FAI_TESTMENU_ChangeMenu(hFanMenu, TESTMENU_LINE_4, ucSpeedStr, RGB_COLOR_YELLOW);
}

