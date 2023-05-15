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
#include "fdi_clock.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FAI_CLOCK_DELAY_TIME		5

typedef enum
{
	FAI_CLOCK_TEST_TMD, 
	FAI_CLOCK_TEST_UI
} FAI_CLOCK_TEST_TYPE;

typedef struct FAI_CLOCK_MSG
{
	FAI_CLOCK_TEST_TYPE eTestType;
	FAI_TESTMENU_H hMenu;
} FAI_CLOCK_MSG_t;

/*******************************************************************************
* External variables and functions
*******************************************************************************/

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
unsigned long g_ulClkMsgId, g_ulClkTaskId;

void INTRN_FAI_CLOCK_Task(void *pData);
FAI_ERR_CODE INTRN_FAI_CLOCK_Check(void);

/*******************************************************************************
* function : 
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FAI_CLOCK_Init(void)
{
	FVK_ERR_CODE eFvkErr;
	
	eFvkErr = FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_CLOCK_MSG_t), (char*)"ClkMsg", &g_ulClkMsgId);
	if( eFvkErr	!= FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_APP_Init] : FVK_MSG_Create Error \n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Create(INTRN_FAI_CLOCK_Task, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "Sc Task", NULL, &g_ulClkTaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_TASK_Create Error!!\n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Start(g_ulClkTaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_TASK_Start Error!!\n"));
		return FVK_ERR;
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
FAI_ERR_CODE FAI_CLOCK_CheckRTC_UI(void)
{
	FVK_ERR_CODE eFvkErr;
	FAI_CLOCK_MSG_t tClkMsg;

	tClkMsg.eTestType = FAI_CLOCK_TEST_UI;
	FAI_TESTMENU_GetCurrentMenu(&tClkMsg.hMenu);
	
	eFvkErr = FVK_MSG_Send(g_ulClkMsgId, &tClkMsg, sizeof(FAI_CLOCK_MSG_t));
	if (eFvkErr != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_CLOCK_CheckRTC_UI] : Error FVK_MSG_Send() : %d\n", eFvkErr));
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
void INTRN_FAI_CLOCK_Task(void *pData)
{
	FAI_CLOCK_MSG_t tClkMsg;
	FVK_ERR_CODE eFvkErr;
	FAI_ERR_CODE eFaiErr;
	unsigned char aucTmdResult[2];
	
	while(1)
	{
		eFvkErr = FVK_MSG_Receive(g_ulClkMsgId, &tClkMsg, sizeof(FAI_CLOCK_MSG_t));
		if (eFvkErr != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_CLOCK_Task] : Error FVK_MSG_Receive() %d\n", eFvkErr));
			continue;
		}
		
		if( tClkMsg.eTestType == FAI_CLOCK_TEST_UI )
		{
			FAI_TESTMENU_ChangeMenuColor(tClkMsg.hMenu, RGB_COLOR_YELLOW);
			FAI_TESTMENU_ChangeMenu(tClkMsg.hMenu, TESTMENU_LINE_2, "RTC Check", RGB_COLOR_YELLOW);
			FAI_TESTMENU_ChangeRCUColor(tClkMsg.hMenu, RGB_COLOR_BLACK);
			
			eFaiErr = INTRN_FAI_CLOCK_Check();
			if( eFaiErr == FAI_NO_ERR )
			{
			
				FAI_TESTMENU_ChangeMenuColor(tClkMsg.hMenu, RGB_COLOR_BLACK);				
				FAI_TESTMENU_ChangeMenu(tClkMsg.hMenu, TESTMENU_LINE_2, "RTC Check", RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(tClkMsg.hMenu, TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);
			}
			else
			{
				FAI_TESTMENU_ChangeMenuColor(tClkMsg.hMenu, RGB_COLOR_RED);				
				FAI_TESTMENU_ChangeMenu(tClkMsg.hMenu, TESTMENU_LINE_2, "RTC Check", RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(tClkMsg.hMenu, TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);				
			}
		}
		else
		{
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
FAI_ERR_CODE INTRN_FAI_CLOCK_Check(void)
{
	unsigned long ulTime1, ulTime2;

	FDI_CLOCK_GetCurrentTime(&ulTime1);

	FVK_TASK_Sleep(FAI_CLOCK_DELAY_TIME*1000);

	FDI_CLOCK_GetCurrentTime(&ulTime2);

	if( ulTime1+FAI_CLOCK_DELAY_TIME == ulTime2 || ulTime1+FAI_CLOCK_DELAY_TIME+1 == ulTime2 )
	{
		return FAI_NO_ERR;
	}
	else
	{
		return FAI_ERR;
	}
}

