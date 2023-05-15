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
#include "fai_app.h"
#include "fai_sc.h"
#include "fdi_sc.h"
#include "fdi_panel.h"

/* model specific configuration header */
#include "factorysw_config.h"

/*******************************************************************************
* Debug level
*******************************************************************************/

/*******************************************************************************
* Definitions, typdef & const data
*******************************************************************************/
#define FAI_SC_BOOT_WAIT_DELAY	3000

#define FAI_SC_ATR_MAX_LENGTH 	33
#define FAI_SC_ATR_DIRECT_TS	0x3B
#define FAI_SC_ATR_INVERSE_TS	0x3F

typedef struct FAI_FAN_MSG
{
	unsigned long ulDeviceId;
    unsigned char aucTestedATR[FAI_SC_ATR_MAX_LENGTH];
	unsigned char aucTestedATRLen;

	FDI_SC_EVENT eEvent;
} FAI_SC_MSG_t;

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
unsigned long g_ulNumOfSc;
unsigned long g_ulScMsgId, g_ulScTaskId;

void INTRN_FAI_SC_Task(void *data);
static FDI_ERR_CODE INTRN_FAI_SC_NotifyCallback(unsigned long ulDeviceId, FDI_SC_EVENT eEvent);
static FAI_ERR_CODE INTRN_FAI_SC_AtrTest(unsigned long ulDeviceId, unsigned char* ucTestedAtr,unsigned char* ucTestedAtrLen);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_SC_Init(void)
{
	FDI_ERR_CODE eFdiErr;
	FVK_ERR_CODE eFvkErr;
	FDI_SC_STATUS eScStatus;
	int i;
	
	FDI_SC_GetCapability(&g_ulNumOfSc);
	for( i = 0; i < g_ulNumOfSc; i++ )
	{
		eFdiErr = FDI_SC_RegisterCallback(i, INTRN_FAI_SC_NotifyCallback);
		if(eFdiErr != FDI_NO_ERR)
		{
			return FAI_ERR;
		}
	}
	
	eFvkErr = FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_SC_MSG_t), (char*)"ScMsg", &g_ulScMsgId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_APP_Init] : FVK_MSG_Create Error \n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Create(INTRN_FAI_SC_Task, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "Sc Task", NULL, &g_ulScTaskId);
	if( eFvkErr!= FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_TASK_Create Error!!\n"));
		return FVK_ERR;
	}
	eFvkErr = FVK_TASK_Start(g_ulScTaskId);
	if( eFvkErr != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_TMD_Init] : FVK_TASK_Start Error!!\n"));
		return FVK_ERR;
	}

	for( i = 0; i < g_ulNumOfSc; i++ )
	{
		FDI_SC_GetCardStatus(i, &eScStatus);
		if( eScStatus == FDI_SC_PRESENT )
		{
			INTRN_FAI_SC_NotifyCallback(i, FDI_SC_INSERTED);
		}
		else
		{
			INTRN_FAI_SC_NotifyCallback(i, FDI_SC_EXTRACTED);
		}
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
#if (SMARTCARD_ATR_ENABLE==YES)
FAI_ERR_CODE FAI_SC_AtrCheck(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{

	FAI_TMD_RESULT_T tTMDResult;
	FAI_TMD_ERR eTmdErr = FAI_TMD_ERR_OK;
	unsigned char ucTmdErrCode[2];

	unsigned char aucTestedATR[FAI_SC_ATR_MAX_LENGTH];
	unsigned char aucTestedATRLen;
		
	FAI_ERR_CODE eFaiErr;
	FDI_SC_STATUS eStatus;
	unsigned char ucSlotId, ucDetection, ucProtocol, aucResult[5];
	
	FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

	if( ucArgc != 2 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SC_AtrCheck] : Argument Count Error=%d\n", ucArgc));

		eTmdErr = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					
		
		goto END;
	}

	FAI_TMD_GetParam(&ucSlotId, pucArgList, 0);
	FAI_TMD_GetParam(&ucDetection, pucArgList, 1);

	if (ucDetection == 0)
	{
		/* Insertion */
		eFaiErr = FDI_SC_GetCardStatus(ucSlotId, &eStatus);
		if (eFaiErr != FAI_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SC_AtrCheck] : Insertion : FDI_SC_GetCardStatus Error\n"));
			eTmdErr = FAI_TMD_ERR_FAIL;		
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
			ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					
			
			goto END;
		}

		if (eStatus != FDI_SC_PRESENT)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SC_AtrCheck] : Insertion : SC status Error\n"));
			eTmdErr = FAI_TMD_ERR_FAIL;			
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_SMARTCARD;
			ucTmdErrCode[1] = FAI_SMARTCARD_TMD_ERR_REMOVED;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					

			goto END;
		}

		ucProtocol = 0;
		eFaiErr = INTRN_FAI_SC_AtrTest(ucSlotId,aucTestedATR,&aucTestedATRLen);
		if( eFaiErr != FAI_NO_ERR )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SC_AtrCheck] : Insertion : INTRN_FAI_SC_AtrTest Error\n"));
			eTmdErr = FAI_TMD_ERR_FAIL;			
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_SMARTCARD;
			ucTmdErrCode[1] = FAI_SMARTCARD_TMD_ERR_ATR;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					
			
			goto END;
		}

		FAI_TMD_AddResultParam(&tTMDResult, aucTestedATR, aucTestedATRLen);
			
	}
	else if (ucDetection == 1)
	{
		/* Extraction */
		eFaiErr = FDI_SC_GetCardStatus(ucSlotId, &eStatus);
		if (eFaiErr != FAI_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SC_AtrCheck] : Extraction : FDI_SC_GetCardStatus Error\n"));
			eTmdErr = FAI_TMD_ERR_FAIL;	
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
			ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_FAIL;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					
			
			goto END;
		}

		if (eStatus != FDI_SC_NOT_PRESENT)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[FAI_SC_AtrCheck] : Extraction : SC status Error\n"));
			eTmdErr = FAI_TMD_ERR_FAIL;				
			ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_SMARTCARD;
			ucTmdErrCode[1] = FAI_SMARTCARD_TMD_ERR_INSERTED;
			FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));					
			
			goto END;
		}
		
	}


END:
	tTMDResult.eResult = eTmdErr;
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
void INTRN_FAI_SC_Task(void *data)
{
	FAI_SC_MSG_t tScMsg;
	FVK_ERR_CODE eVkRet;
	FAI_TESTMENU_H hMenu;
#if (SMARTCARD_UI_SLOT_SWAP==YES)
	const unsigned char ucUpperSlot = 0;
#else
	const unsigned char ucUpperSlot = 1;
#endif	
	/* 
	 *	처음 부팅시 카드를 인식하고 ATR 테스트를 하기 위해서
	 *	app가 뜰때까지 기다린다. 
	 */
	FVK_TASK_Sleep(FAI_SC_BOOT_WAIT_DELAY);
	
	while(1)
	{
		eVkRet = FVK_MSG_Receive(g_ulScMsgId, &tScMsg, sizeof(FAI_SC_MSG_t));
		if (eVkRet != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_SC_TestTask] : Error FVK_MSG_Receive() %d\n", eVkRet));
			continue;
		}
#if (SMARTCARD_UI_ENABLE==YES)
		FAI_APP_GetTestMenuHandle(SMARTCARD_UI_SHEET, SMARTCARD_UI_CELL, &hMenu);
		if( g_ulNumOfSc == 1 )
		{
			if( tScMsg.eEvent == FDI_SC_INSERTED )
			{
				FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_YELLOW);
				FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "SMART CARD", RGB_COLOR_YELLOW);
				FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "Detection", RGB_COLOR_YELLOW);
				FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "Test", RGB_COLOR_YELLOW);
				FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);

                          if(INTRN_FAI_SC_AtrTest(tScMsg.ulDeviceId,tScMsg.aucTestedATR,&tScMsg.aucTestedATRLen)!= FAI_NO_ERR )
				{
					FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_RED);
					FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_1, "CARD", RGB_COLOR_RED);
					FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "Detection", RGB_COLOR_RED);
					FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "Fail", RGB_COLOR_RED);
					FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);
					FDI_PANEL_Display(FAI_SC_ATRCHECK_FAIL_STR);
				}
				else
				{
					FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_BLACK);
					FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_1, "CARD", RGB_COLOR_BLACK);
					FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "Detection", RGB_COLOR_BLACK);
					FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "Pass", RGB_COLOR_BLACK);
					FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);
					FDI_PANEL_Display(FAI_SC_ATRCHECK_PASS_STR);
				}
			}
			else if( tScMsg.eEvent == FDI_SC_EXTRACTED )
			{
				FAI_TESTMENU_ChangeMenuColor(hMenu, RGB_COLOR_YELLOW);
				FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "CARD OUT", RGB_COLOR_YELLOW);
				FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);
				FDI_PANEL_Display(FAI_SC_ATRCHECK_OUT_STR);
			}
		}
		else	/* in case of 2 instance, dev_id 0 is lower, dev_id 1 is upper */
		{
			if( tScMsg.eEvent == FDI_SC_INSERTED )
			{
				if( tScMsg.ulDeviceId == ucUpperSlot )
				{
					FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_1, "CARD Upper", RGB_COLOR_YELLOW);
					FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "Test", RGB_COLOR_YELLOW);
				}
				else
				{
					FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "CARD Lower", RGB_COLOR_YELLOW);
					FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_4, "Test", RGB_COLOR_YELLOW);
				}
				FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);
				
				if( INTRN_FAI_SC_AtrTest(tScMsg.ulDeviceId,tScMsg.aucTestedATR,&tScMsg.aucTestedATRLen) != FAI_NO_ERR )
				{
					if( tScMsg.ulDeviceId == ucUpperSlot )
					{
						FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_1, "CARD Upper", RGB_COLOR_RED);
						FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "Fail", RGB_COLOR_RED);
					}
					else
					{
						FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "CARD Lower", RGB_COLOR_RED);
						FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_4, "Fail", RGB_COLOR_RED);
					}
				}
				else
				{
					if( tScMsg.ulDeviceId == ucUpperSlot )
					{
						FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_1, "CARD Upper", RGB_COLOR_BLACK);
						FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "Pass", RGB_COLOR_BLACK);
					}
					else
					{
						FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "CARD Lower", RGB_COLOR_BLACK);
						FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_4, "Pass", RGB_COLOR_BLACK);
					}
				}
			}
			else if( tScMsg.eEvent == FDI_SC_EXTRACTED )
			{
				if( tScMsg.ulDeviceId == ucUpperSlot )
				{
					FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_1, "CARD Upper", RGB_COLOR_YELLOW);
					FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_2, "Out", RGB_COLOR_YELLOW);
				}
				else
				{
					FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_3, "CARD Lower", RGB_COLOR_YELLOW);
					FAI_TESTMENU_ChangeMenu(hMenu, TESTMENU_LINE_4, "Out", RGB_COLOR_YELLOW);
				}
				FAI_TESTMENU_ChangeRCUColor(hMenu, RGB_COLOR_BLACK);
			}
		}
#endif
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
static FDI_ERR_CODE INTRN_FAI_SC_NotifyCallback(unsigned long ulDeviceId, FDI_SC_EVENT eEvent)
{
	FVK_ERR_CODE eVkRet;
	FAI_SC_MSG_t tScMsg;
	FAI_PP_SHEET eCurrentSheet;

	tScMsg.ulDeviceId = ulDeviceId;
	tScMsg.eEvent = eEvent;

	eVkRet = FVK_MSG_Send(g_ulScMsgId, &tScMsg, sizeof(FAI_SC_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SC_NotifyCallback] : Error FVK_MSG_Send() : %d\n", eVkRet));
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
static FAI_ERR_CODE INTRN_FAI_SC_AtrTest(unsigned long ulDeviceId, unsigned char* ucTestedAtr,unsigned char* ucTestedAtrLen)
{
	FDI_ERR_CODE eFdiErr;
	unsigned char aucAtr[FAI_SC_ATR_MAX_LENGTH];
	unsigned char ucAtrLength;
	FDI_SC_PROTOCOL eSupportProtocol, eProtocol;
       
	/* get support protocol */
	eSupportProtocol = 0;
#if (SMARTCARD_UI_PROTOCOL&SMARTCARD_PROTOCOL_T0)
	eSupportProtocol |= FDI_T_0;
#endif

#if (SMARTCARD_UI_PROTOCOL&SMARTCARD_PROTOCOL_T1)
	eSupportProtocol |= FDI_T_1;
#endif

#if (SMARTCARD_UI_PROTOCOL&SMARTCARD_PROTOCOL_T14)
	eSupportProtocol |= FDI_T_14;
#endif

	while(eSupportProtocol)
	{
		eProtocol = eSupportProtocol&FDI_T_0;
		if (eProtocol == 0)
		{
			eProtocol = eSupportProtocol&FDI_T_1;
			if (eProtocol == 0)
			{
				eProtocol = eSupportProtocol&FDI_T_14;
				if (eProtocol == 0)
				{
					break;
				}
			}
		}
		eSupportProtocol &= ~eProtocol;

		eFdiErr = FDI_SC_SetParams(ulDeviceId, eProtocol);
		if(eFdiErr != FDI_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SC_AtrTest] : FDI_SC_SetParams Error\n"));
			continue;
		}
		
		eFdiErr = FDI_SC_Reset(ulDeviceId, aucAtr, &ucAtrLength);
		if(eFdiErr != FDI_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SC_AtrTest] : FDI_SC_Reset Error\n"));
			continue;
		}

		
		
		/* verify.. */
		if( ucAtrLength <= 0 || ucAtrLength > FAI_SC_ATR_MAX_LENGTH )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SC_AtrTest] : ATR Length Error!!\n"));
			continue;
		}
		
		if( aucAtr[0] != FAI_SC_ATR_DIRECT_TS && aucAtr[0] != FAI_SC_ATR_INVERSE_TS )
		{
			FAI_PRINT(FAI_PRT_ERROR, ("[INTRN_FAI_SC_AtrTest] : ATR Parsing Error!!\n"));
			continue;
		}
		
		*ucTestedAtrLen=ucAtrLength;
		FVK_MEM_Copy(ucTestedAtr, aucAtr, ucAtrLength);
		return FAI_NO_ERR;
	}
TEST_FAIL:
	return FAI_ERR;
}

