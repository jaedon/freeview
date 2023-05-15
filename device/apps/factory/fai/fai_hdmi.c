/*******************************************************************************
* File name :
* Author :
* description :
*
* Copyright (c) 2007 by Humax Co., Ltd.
* All right reserved
*******************************************************************************/

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
#include "fai_hdmi.h"
#include "fdi_hdmi.h"
#include "fdi_video.h"
#include "fdi_panel.h"

/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/


/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FAI_TMD_CATEGORY_HDCP	0x02
#define FAI_TMD_MODULE_HDCP	0x08
#define FAI_TMD_ACTION_HDCP	0x02

typedef enum
{
	FAI_HDMI_TEST_TMD,
	FAI_HDMI_TEST_UI,
	FAI_HDMI_TEST_NONE
} FAI_HDMI_TEST_TYPE;

typedef struct
{
	BOOL bHdcpResult;
} FAI_HDMI_MSG_t;

#define FAI_HDMI_HDCP_WAIT (10*1000)

#define FAI_HDMI_INIT_ADDR 		0x03
#define FAI_HDMI_DEST_ADDR		0x00
#define FAI_HDMI_CEC_CMD_GET_VERSION 	0x9F

/*******************************************************************************
* Global variables and structures
*******************************************************************************/


/*******************************************************************************
* External variables and functions
*******************************************************************************/


/*******************************************************************************
* Static variables and structures
*******************************************************************************/
unsigned long g_ulHdmiMsgId, g_ulHdmiTaskId, g_uiHdmiTimerId;
FAI_TESTMENU_H hHdcpMenu;
FAI_HDMI_TEST_TYPE g_eHdcpTestType = FAI_HDMI_TEST_NONE;
/*******************************************************************************
* Static function prototypes
*******************************************************************************/
void INTRN_FAI_HDMI_TestTask(void *data);
void INTRN_FAI_HDMI_HdcpConnectCallback(void *data);
void INTRN_FAI_HDMI_HdcpDisconnectCallback(void *data);
void INTRN_FAI_HDMI_HdcpPassCallback(void *data);
void INTRN_FAI_HDMI_HdcpFailCallback(void *data);
FVK_ERR_CODE INTRN_FAI_HDMI_HDCPTimerCallback(unsigned long ulTimerId, void *pvParams);

/*******************************************************************************
* function : FAI_HDMI_Init
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_HDMI_Init()
{
	FDI_ERR_CODE eFdiErr;

	if( FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_HDMI_MSG_t), (char*)"HdmiMsg", &g_ulHdmiMsgId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_Init] : FVK_MSG_Create \n"));
		return FAI_ERR;
	}
	if( FVK_TASK_Create(INTRN_FAI_HDMI_TestTask, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "Fan Task", NULL, &g_ulHdmiTaskId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_Init] : FVK_TASK_Create Error!!\n"));
		return FAI_ERR;
	}
	if( FVK_TASK_Start(g_ulHdmiTaskId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_Init] : FVK_TASK_Start Error!!\n"));
		return FAI_ERR;
	}
	if( FDI_HDMI_RegisterCallback(FDI_HDMI_EVENT_CONNECT, INTRN_FAI_HDMI_HdcpConnectCallback) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_Init] : FDI_HDMI_RegisterCallback Error!!\n"));
		return FAI_ERR;
	}
	if( FDI_HDMI_RegisterCallback(FDI_HDMI_EVENT_DISCONNECT, INTRN_FAI_HDMI_HdcpDisconnectCallback) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_Init] : FDI_HDMI_RegisterCallback Error!!\n"));
		return FAI_ERR;
	}
	if( FDI_HDMI_RegisterCallback(FDI_HDMI_EVENT_HDCP_SUCCESS, INTRN_FAI_HDMI_HdcpPassCallback) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_Init] : FDI_HDMI_RegisterCallback Error!!\n"));
		return FAI_ERR;
	}
	if( FDI_HDMI_RegisterCallback(FDI_HDMI_EVENT_HDCP_FAIL, INTRN_FAI_HDMI_HdcpFailCallback) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_Init] : FDI_HDMI_RegisterCallback Error!!\n"));
		return FAI_ERR;
	}

	return FAI_NO_ERR;
}

/*******************************************************************************
* function : FAI_HDMI_Cec
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (HDMI_CEC_ENALBE==YES)
FAI_ERR_CODE FAI_HDMI_Cec(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FDI_ERR_CODE eFdiErr;
	FDI_HDMI_CEC_MSG_t stCecMsg;
	unsigned char ucTmdErrCode[2];

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 0 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_Cec] : Argument Count Error=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_HDMI;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		goto END;
	}

	stCecMsg.ucInitAddr = FAI_HDMI_INIT_ADDR;
	stCecMsg.ucDestAddr = FAI_HDMI_DEST_ADDR;
	stCecMsg.ucMegLength = 1;
	stCecMsg.ucMessage[0] = FAI_HDMI_CEC_CMD_GET_VERSION; /* Get CEC Version */
	eFdiErr = FDI_HDMI_SendCecMsg(&stCecMsg);
	if(eFdiErr != FDI_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_Cec] : FDI_HDMI_EnableHdcp Error!!\n"));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_HDMI;
		ucTmdErrCode[1] = FAI_HDMI_TMD_ERR_CEC_FUNCTION;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		goto END;
	}

END:
	tTMDResult.eResult = eTmdErr;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function : FAI_HDMI_EnableHdcp
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (HDMI_HDCP_ENABLE==YES)
FAI_ERR_CODE FAI_HDMI_EnableHdcp(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	FDI_ERR_CODE eFdiErr;
	unsigned char ucOnOff;
	unsigned char ucTmdErrCode[2];
	BOOL bHdmiConnected;

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_EnableHdcp] : Argument Count Error=%d\n", ucArgc));
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_HDMI;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

		goto END;
	}

	FAI_TMD_GetParam(&ucOnOff, pucArgList, 0);

	g_eHdcpTestType = FAI_HDMI_TEST_TMD;

	FDI_HDMI_IsConnected(&bHdmiConnected);
	if(bHdmiConnected == FALSE)
	{
		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_HDMI;
		ucTmdErrCode[1] = FAI_HDMI_TMD_ERR_DISCONNECT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
		goto END;
	}

	if(ucOnOff)
	{
		eFdiErr = FDI_HDMI_EnableHdcp(TRUE);
		if(eFdiErr != FDI_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_EnableHdcp] : FDI_HDMI_EnableHdcp(TRUE) Error!!\n"));
			eTmdErr = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_HDMI;
			ucTmdErrCode[1] = FAI_HDMI_TMD_ERR_HDCP_FUNCTION;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			goto END;
		}
		FVK_TIMER_CreateStart(FAI_HDMI_HDCP_WAIT, INTRN_FAI_HDMI_HDCPTimerCallback, NULL, 0, FVK_TIMER_ONCE, &g_uiHdmiTimerId);
		return FAI_NO_ERR;
	}
	else
	{
		eFdiErr = FDI_HDMI_EnableHdcp(FALSE);
		if(eFdiErr != FDI_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_EnableHdcp] : FDI_HDMI_EnableHdcp(FALSE) Error!!\n"));
			eTmdErr = FAI_TMD_ERR_FAIL;
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_HDMI;
			ucTmdErrCode[1] = FAI_HDMI_TMD_ERR_HDCP_FUNCTION;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));
			goto END;
		}
	}

END:

	tTMDResult.ucCategory = FAI_CATEGORY_AV_OUTPUT;
	tTMDResult.ucModule = 0x08;
	tTMDResult.ucAction = 0x02;

	tTMDResult.eResult = eTmdErr;
	FAI_TMD_SetResult(&tTMDResult);
	return FAI_NO_ERR;
}
#endif

/*******************************************************************************
* function : FAI_HDMI_EnableHDCP_UI
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
#if (HDMI_HDCP_UI_ENABLE==YES)
FAI_ERR_CODE FAI_HDMI_EnableHDCP_UI(void)
{
	g_eHdcpTestType = FAI_HDMI_TEST_UI;
	FAI_TESTMENU_GetCurrentMenu(&hHdcpMenu);

	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "HDCP Testing", RGB_COLOR_YELLOW);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);

	/* disable HDCP : for retest */
	FDI_HDMI_EnableHdcp(FALSE);

	if( FDI_HDMI_EnableHdcp(TRUE) != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_HDCPOnOff_UI] : FDI_HDMI_EnableHdcp Error!!\n"));
		goto TEST_FAIL;
	}
	return FAI_NO_ERR;

TEST_FAIL:
	FAI_TESTMENU_ChangeCurrentMenuColor(RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentMenu(TESTMENU_LINE_2, "HDCP Fail", RGB_COLOR_RED);
	FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
	FDI_PANEL_Display(FAI_HDCP_FAIL_STR);

	return FAI_ERR;
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
FAI_ERR_CODE FAI_HDMI_CecCtrl_UI(void)
{
	return FAI_NO_ERR;
}

/*******************************************************************************
* function : INTRN_FAI_HDMI_TestTask
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void INTRN_FAI_HDMI_TestTask(void *data)
{
	FAI_TMD_RESULT_T tTMDResult;
	FAI_HDMI_MSG_t tHdmiMsg;
	FVK_ERR_CODE eVkRet;
	unsigned char ucTmdErrCode[2];
	while(1)
	{
		eVkRet = FVK_MSG_Receive(g_ulHdmiMsgId, &tHdmiMsg, sizeof(FAI_HDMI_MSG_t));
		if (eVkRet != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_HDMI_TestTask] : Error FVK_MSG_Receive() %d\n", eVkRet));
			continue;
		}

		if( g_eHdcpTestType == FAI_HDMI_TEST_TMD )
		{
			if( tHdmiMsg.bHdcpResult == TRUE ) 				/* HDCP authentication success */
			{
				FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

				tTMDResult.ucCategory = FAI_TMD_CATEGORY_HDCP;
				tTMDResult.ucModule = FAI_TMD_MODULE_HDCP;
				tTMDResult.ucAction = FAI_TMD_ACTION_HDCP;
				tTMDResult.eResult = FAI_TMD_ERR_OK;

				FAI_TMD_SetResult(&tTMDResult);
				FVK_TIMER_Destory(g_uiHdmiTimerId);

				FDI_PANEL_Display(FAI_HDCP_PASS_STR);
			}
			else											/* HDCP authentication fail */
			{
				FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

				tTMDResult.ucCategory = FAI_TMD_CATEGORY_HDCP;
				tTMDResult.ucModule = FAI_TMD_MODULE_HDCP;
				tTMDResult.ucAction = FAI_TMD_ACTION_HDCP;
				tTMDResult.eResult = FAI_TMD_ERR_FAIL;

				ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_HDMI;
				ucTmdErrCode[1] = FAI_HDMI_TMD_ERR_HDCP_ENABLE;
				FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

				FAI_TMD_SetResult(&tTMDResult);

				FDI_PANEL_Display(FAI_HDCP_FAIL_STR);
			}
			FVK_TIMER_Destory(g_uiHdmiTimerId);
		}
		else
		{
			if( tHdmiMsg.bHdcpResult == TRUE ) 				/* HDCP authentication success */
			{
				FAI_TESTMENU_ChangeMenuColor(hHdcpMenu, RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeMenu(hHdcpMenu, TESTMENU_LINE_2, "HDCP Pass", RGB_COLOR_BLACK);
				FAI_TESTMENU_ChangeRCUColor(hHdcpMenu, RGB_COLOR_BLACK);
				FDI_PANEL_Display(FAI_HDCP_PASS_STR);
			}
			else
			{
				FAI_TESTMENU_ChangeMenuColor(hHdcpMenu, RGB_COLOR_RED);
				FAI_TESTMENU_ChangeMenu(hHdcpMenu, TESTMENU_LINE_2, "HDCP Fail", RGB_COLOR_RED);
				FAI_TESTMENU_ChangeRCUColor(hHdcpMenu, RGB_COLOR_BLACK);
				FDI_PANEL_Display(FAI_HDCP_FAIL_STR);
			}
		}
	}
}

/*******************************************************************************
* function : INTRN_FAI_HDMI_HdcpPassCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void INTRN_FAI_HDMI_HdcpPassCallback(void *data)
{
	FAI_HDMI_MSG_t tHdmiMsg;
	FVK_ERR_CODE eVkRet;

	FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_HdcpPassCallback] : ++\n"));

	tHdmiMsg.bHdcpResult = TRUE;
	eVkRet = FVK_MSG_Send(g_ulHdmiMsgId, &tHdmiMsg, sizeof(FAI_HDMI_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_HdcpPassCallback] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return;
	}
}

/*******************************************************************************
* function : INTRN_FAI_HDMI_HdcpFailCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void INTRN_FAI_HDMI_HdcpFailCallback(void *data)
{
	FAI_HDMI_MSG_t tHdmiMsg;
	FVK_ERR_CODE eVkRet;

	FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDMI_HdcpFailCallback] : ++\n"));

	tHdmiMsg.bHdcpResult = FALSE;
	eVkRet = FVK_MSG_Send(g_ulHdmiMsgId, &tHdmiMsg, sizeof(FAI_HDMI_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_HdcpPassCallback] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return;
	}
}

/*******************************************************************************
* function : INTRN_FAI_HDMI_HdcpConnectCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void INTRN_FAI_HDMI_HdcpConnectCallback(void *data)
{
	FDI_ERR_CODE eFdiErr;

	eFdiErr = FDI_VIDEO_ResetDisplayStandard();
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDMI_HdcpConnectCallback] : FDI_VIDEO_ResetDisplayStandard \n"));
	}

	eFdiErr = FDI_HDMI_EnableOutput(TRUE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDMI_HdcpConnectCallback] : FDI_HDMI_EnableOutput \n"));
	}
}

/*******************************************************************************
* function : INTRN_FAI_HDMI_HdcpDisconnectCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void INTRN_FAI_HDMI_HdcpDisconnectCallback(void *data)
{
	FDI_ERR_CODE eFdiErr;

	eFdiErr = FDI_HDMI_EnableOutput(FALSE);
	if( eFdiErr != FDI_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_HDMI_HdcpConnectCallback] : FDI_HDMI_EnableOutput \n"));
	}
}

/*******************************************************************************
* function : INTRN_FAI_HDMI_HDCPTimerCallback
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FVK_ERR_CODE INTRN_FAI_HDMI_HDCPTimerCallback(unsigned long ulTimerId, void *pvParams)
{
	FAI_TMD_RESULT_T tTMDResult;
	unsigned char ucTmdErrCode[2];

	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	tTMDResult.ucCategory = FAI_TMD_CATEGORY_HDCP;
	tTMDResult.ucModule = FAI_TMD_MODULE_HDCP;
	tTMDResult.ucAction = FAI_TMD_ACTION_HDCP;
	tTMDResult.eResult = FAI_TMD_ERR_FAIL;

	ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_HDMI;
	ucTmdErrCode[1] = FAI_HDMI_TMD_ERR_HDCP_ENABLE;
	FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));

	FAI_TMD_SetResult(&tTMDResult);

	FDI_PANEL_Display(FAI_HDCP_FAIL_STR);
}

