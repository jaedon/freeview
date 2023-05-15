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
#include "fai_app.h"
#include "fai_channel.h"
#include "fai_tmd.h"
#include "fdi_err.h"
#include "fdi_panel.h"
#include "fdi_irb.h"

/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
typedef struct
{
	FAI_TESTMENU_H hMenu;
} IRB_MSG_T;


/*******************************************************************************
* External variables and functions
*******************************************************************************/


/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* Static variables and structures
*******************************************************************************/
static unsigned long s_ulIRBMsgQ = 0;
static unsigned int	s_unIrbTestKey = RCU_KEY_3;

static void INTRN_FAI_IRB_Task(void *pvArgc);


/*******************************************************************************
* function : 
* description : 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_IRB_Init(void)
{
	FVK_ERR_CODE	eFvkError = FVK_NO_ERR;
	unsigned long ulTaskID = 0;
	IRB_MSG_T		tIrbMsg;
	
	eFvkError = FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(tIrbMsg), "IRB_MSG", &s_ulIRBMsgQ);
	if(eFvkError != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_IRB_Init] : FVK_MSG_Create \n"));
		return FVK_ERR;
	}
	FAI_PRINT(0, ("[task] s_ulIRBMsgQ: 0x%x \n",s_ulIRBMsgQ));
	
	eFvkError = FVK_TASK_Create(INTRN_FAI_IRB_Task, FVK_TASK_DEFAULT_PRIORITY, 512, "IRB_TASK", NULL, &ulTaskID);
	if(eFvkError != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_IRB_Init] : FVK_TASK_Create \n"));
		return FVK_ERR;
	}
	eFvkError = FVK_TASK_Start(ulTaskID);
	if(eFvkError != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_IRB_Init] : FVK_TASK_Create \n"));
		return FVK_ERR;
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
FAI_ERR_CODE FAI_IRB_KeyLoopBack(unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_FUNC_ENTER;

	
	if(ucArgc != 1)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_AV_Play] : Argument Count Error=%d\n", ucArgc));
	}

	/* get params */
	FAI_TMD_GetParam((unsigned char*)&s_unIrbTestKey, pucArgList, 0);

	FAI_FUNC_EXIT;
	return FAI_ERR;	
}

/*******************************************************************************
* function : FAI_IRB_KeyLoopBack_UI
* description : UI test mode에서는 key 값을 지정하여 send한다. 
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (IRB_BLASTER_UI_ENABLE==YES)
FAI_ERR_CODE FAI_IRB_KeyLoopBack_UI(void)
{
	FAI_ERR_CODE	eFaiError = FAI_NO_ERR;
	FVK_ERR_CODE	eFvkError = FVK_NO_ERR;
	unsigned char	ucMessage = 0xFF;
	IRB_MSG_T		tIrbMsg;
	
	FAI_FUNC_ENTER;

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "IR Blaster", RGB_COLOR_YELLOW);
	FDI_IRB_SendData(0x5);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	FAI_TESTMENU_GetCurrentMenu(&tIrbMsg.hMenu);
	eFvkError = FVK_MSG_Send(s_ulIRBMsgQ, &tIrbMsg, sizeof(tIrbMsg));
	if (eFvkError != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_MODEM_StartCallTest] : Error FVK_MSG_Send() : %d\n", eFvkError));
		eFaiError = FAI_ERR;
	}

	FAI_FUNC_EXIT;
	return FVK_NO_ERR;	
}
#endif

static void INTRN_FAI_IRB_Task(void *pvArgc)
{
	IRB_MSG_T		tIrbMsg;
	FVK_ERR_CODE	eFvkError = FVK_NO_ERR;
	unsigned long	ulCurrentKey =0;
	unsigned int	unRetry = 50;
	
	while(1)
	{
		eFvkError = FVK_MSG_Receive(s_ulIRBMsgQ, &tIrbMsg, sizeof(tIrbMsg));
		if(eFvkError != FVK_NO_ERR)
		{
			continue;
		}
		
		unRetry = 50;
		while(unRetry > 0)
		{
			ulCurrentKey = FAI_APP_GetCurrentRcuKey();	
			if(ulCurrentKey != IRB_BLASTER_UI_INPUT)
			{
				if(ulCurrentKey == s_unIrbTestKey)
				{
					FDI_PANEL_Display(FAI_IRB_TEST_PASS_STR);
					FAI_TESTMENU_ChangeMenuColor(tIrbMsg.hMenu, RGB_COLOR_BLACK);
					FAI_TESTMENU_ChangeMenu(tIrbMsg.hMenu, TESTMENU_LINE_2, "IR Blaster", RGB_COLOR_BLACK);
					FAI_TESTMENU_ChangeMenu(tIrbMsg.hMenu, TESTMENU_LINE_3, "PASS", RGB_COLOR_BLACK);
				}
				else
				{
					FAI_TESTMENU_ChangeMenuColor(tIrbMsg.hMenu, RGB_COLOR_RED);
					FAI_TESTMENU_ChangeMenu(tIrbMsg.hMenu, TESTMENU_LINE_2, "IR Blaster", RGB_COLOR_RED);
					FAI_TESTMENU_ChangeMenu(tIrbMsg.hMenu, TESTMENU_LINE_3, "FAIL", RGB_COLOR_RED);
					FDI_PANEL_Display(FAI_IRB_TEST_FAIL_STR);
				}
				break;
			}
			else
			{
				FAI_PRINT(1, (" value: 0x%x 0x%x  \n",ulCurrentKey, s_unIrbTestKey));
				FVK_TASK_Sleep(100);
				unRetry--;
			}
		}

		if(ulCurrentKey != s_unIrbTestKey)
		{
			FAI_TESTMENU_ChangeMenuColor(tIrbMsg.hMenu, RGB_COLOR_RED);
			FAI_TESTMENU_ChangeMenu(tIrbMsg.hMenu, TESTMENU_LINE_2, "IR Blaster", RGB_COLOR_RED);
			FAI_TESTMENU_ChangeMenu(tIrbMsg.hMenu, TESTMENU_LINE_3, "FAIL", RGB_COLOR_RED);
			FDI_PANEL_Display(FAI_IRB_TEST_FAIL_STR);
		}
	}
}

