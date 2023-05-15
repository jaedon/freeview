/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 LED & 7Seg Test
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
#include "fai_testparams.h"
#include "fai_tmd.h"
#include "fai_fkey.h"
#include "fdi_key.h"
#include "fdi_panel.h"

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
	FAI_FKEY_TEST_TYPE eType;
	int	 nKeyValue;
} FAI_KEY_MSG_t;

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
unsigned long g_ulKeyMsgId, g_ulKeyTaskId, g_uiKeyTimerId;

unsigned long g_ulCurrentTestFKey, g_ulLastPressedFKey, g_ulFrontKey, g_ulFrontKeyNum = 0;
unsigned short g_usMsgId[MAX_TMD_TEST_MULTI_INDEX];
FAI_ERR_CODE INTRN_FAI_FKEY_Toggle();
FAI_ERR_CODE INTRN_FAI_FKEY_SetResult(FDI_FKEY_TYPE eFKeyType);
FAI_ERR_CODE INTRN_FAI_FKEY_Reset();

/*******************************************************************************
* function : INTRN_FAI_FKEY_TestTask
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void INTRN_FAI_FKEY_TestTask(void *data)
{
	FAI_TMD_RESULT_T tTMDResult;
	unsigned short usErrCode = FAI_TMD_DEFAULT_ERROR_CODE;
	FAI_KEY_MSG_t tKeyMsg;
	FVK_ERR_CODE eVkRet;
	FAI_FKEY_TEST_TYPE eCmdType;
	unsigned char aucAgcResult[4];
	
	int nIndex;
	while(1)
	{
		eVkRet = FVK_MSG_Receive(g_ulKeyMsgId, &tKeyMsg, sizeof(FAI_KEY_MSG_t));
		if (eVkRet != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_FKEY_TestTask] : Error FVK_MSG_Receive() %d\n", eVkRet));
			continue;
		}

		eCmdType = tKeyMsg.eType;
		if(eCmdType == FAI_FKEY_TEST_TMD)
		{

		}
		else if(eCmdType == FAI_FKEY_TEST_RESULT)
		{
			if(g_ulCurrentTestFKey & tKeyMsg.nKeyValue)
			{
				g_ulCurrentTestFKey &= (~tKeyMsg.nKeyValue);
				
				for(nIndex = 0 ; nIndex < 32  ; nIndex++)	
				{
					if((tKeyMsg.nKeyValue >> nIndex) & 0x1)
					{
						break;
					}
				}

				FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

				aucAgcResult[0] = (tKeyMsg.nKeyValue&0xFF000000)>>24;
				aucAgcResult[1] = (tKeyMsg.nKeyValue&0x00FF0000)>>16;
				aucAgcResult[2] = (tKeyMsg.nKeyValue&0x0000FF00)>>8;
				aucAgcResult[3] = (tKeyMsg.nKeyValue&0x000000FF);
					
				tTMDResult.ucCategory = 0x04;
				tTMDResult.ucModule 	= 0x01;
				tTMDResult.ucAction 	= 0x01;
				tTMDResult.usMsgId	= g_usMsgId[nIndex];
				tTMDResult.eResult 	= FAI_TMD_ERR_OK;
				FAI_TMD_AddResultParam(&tTMDResult, aucAgcResult, 4);
				
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
FAI_ERR_CODE FAI_FKEY_Init()
{
	if( FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_KEY_MSG_t), (char*)"KeyMsg", &g_ulKeyMsgId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_Init] : FVK_MSG_Create \n"));
		return FAI_ERR;
	}
	if( FVK_TASK_Create(INTRN_FAI_FKEY_TestTask, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "Key Task", NULL, &g_ulKeyTaskId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_Init] : FVK_TASK_Create Error!!\n"));
		return FAI_ERR;
	}
	if( FVK_TASK_Start(g_ulKeyTaskId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_HDMI_Init] : FVK_TASK_Start Error!!\n"));
		return FAI_ERR;
	}

	INTRN_FAI_FKEY_Reset();
	return FAI_NO_ERR;
}

#if (FRONT_KEY_ENABLE==YES)
/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FKEY_SetTestKey(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char aucKey[4],ucKeyLen;
	int nKey,nIndex;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	
	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FKEY_SetTestKey] : Argument Count Error=%d\n", ucArgc));
		FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
		
		tTMDResult.ucCategory = FAI_CATEGORY_FRONT_PANEL;
		tTMDResult.ucModule = 1;
		tTMDResult.ucAction = 1;
		tTMDResult.usMsgId	= usMsgId;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		
		FAI_TMD_SetResult(&tTMDResult);
		
		return FAI_ERR;
	}

	ucKeyLen = FAI_TMD_GetParam(aucKey, pucArgList, 0);
	FAI_TMD_ArgToValue((unsigned long*)&nKey, aucKey, ucKeyLen);

	if( nKey >= FRONT_KEY_MAX )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FKEY_SetTestKey] : Undefined Key=%d\n", nKey));
		FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
		
		tTMDResult.ucCategory = FAI_CATEGORY_FRONT_PANEL;
		tTMDResult.ucModule = 1;
		tTMDResult.ucAction = 1;
		tTMDResult.usMsgId	= usMsgId;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_KEY;
		ucTmdErrCode[1] = FAI_KEY_TMD_ERR_INVALID_KEY_NUMBER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		
		FAI_TMD_SetResult(&tTMDResult);

		
		return FAI_ERR;
	}

	//g_ulCurrentTestFKey = (1<<(aucKey-1));	
	g_ulCurrentTestFKey = g_ulCurrentTestFKey|nKey;
	
	for(nIndex = 0 ; nIndex < 32  ; nIndex++)	
	{
		if((nKey >> nIndex) & 0x1)
		{
			break;
		}
	}
	g_usMsgId[nIndex] = usMsgId;

	return FAI_NO_ERR;
}
#endif
void FAI_FKEY_CALLBACK(unsigned int unKeyCode)
{
#if (FRONT_KEY_ENABLE==YES)
	FAI_KEY_MSG_t tKeyMsg;
	FVK_ERR_CODE eVkRet;

	FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FKEY_CALLBACK] : ++\n"));
	
	tKeyMsg.eType		=	FAI_FKEY_TEST_RESULT;
	tKeyMsg.nKeyValue 	=	unKeyCode;
	eVkRet = FVK_MSG_Send(g_ulKeyMsgId, &tKeyMsg, sizeof(FAI_KEY_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_FKEY_CALLBACK] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return;
	}
#endif	
}


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FAI_ERR_CODE FAI_FKEY_Standby()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_STANDBY_STR);

	g_ulLastPressedFKey = FRONT_KEY_POWER;
	g_ulFrontKey &= (~FRONT_KEY_POWER);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_Guide()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_GUIDE_STR);

	g_ulLastPressedFKey = FRONT_KEY_GUIDE;
	g_ulFrontKey &= (~FRONT_KEY_GUIDE);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_Ok()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_OK_STR);

	g_ulLastPressedFKey = FRONT_KEY_OK;
	g_ulFrontKey &= (~FRONT_KEY_OK);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_ChUp()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_CHUP_STR);

	g_ulLastPressedFKey = FRONT_KEY_CHUP;
	g_ulFrontKey &= (~FRONT_KEY_CHUP);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_ChDown()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_CHDOWN_STR);

	g_ulLastPressedFKey = FRONT_KEY_CHDOWN;
	g_ulFrontKey &= (~FRONT_KEY_CHDOWN);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_VolUp()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_VOLUP_STR);

	g_ulLastPressedFKey = FRONT_KEY_VOLUP;
	g_ulFrontKey &= (~FRONT_KEY_VOLUP);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_VolDown()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_VOLDOWN_STR);

	g_ulLastPressedFKey = FRONT_KEY_VOLDOWN;
	g_ulFrontKey &= (~FRONT_KEY_VOLDOWN);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_TvRadio()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_TVRADIO_STR);

	g_ulLastPressedFKey = FRONT_KEY_TVRADIO;
	g_ulFrontKey &= (~FRONT_KEY_TVRADIO);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_Play()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_PLAY_STR);

	g_ulLastPressedFKey = FRONT_KEY_PLAY;
	g_ulFrontKey &= (~FRONT_KEY_PLAY);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_Info()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_INFO_STR);

	g_ulLastPressedFKey = FRONT_KEY_INFO;
	g_ulFrontKey &= (~FRONT_KEY_INFO);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_WPS()
{
#if defined(CONFIG_DI20)
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_WPS_STR);

	g_ulLastPressedFKey = FRONT_KEY_WPS;
	g_ulFrontKey &= (~FRONT_KEY_WPS);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
	}
#endif
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
FAI_ERR_CODE FAI_FKEY_Network()
{
#if defined(CONFIG_DI20)
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_NETWORK_STR);

	g_ulLastPressedFKey = FRONT_KEY_NETWORK;
	g_ulFrontKey &= (~FRONT_KEY_NETWORK);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
	}
#endif
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
FAI_ERR_CODE FAI_FKEY_Up()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_UP_STR);

	g_ulLastPressedFKey = FRONT_KEY_UP;
	g_ulFrontKey &= (~FRONT_KEY_UP);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_Down()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_DOWN_STR);

	g_ulLastPressedFKey = FRONT_KEY_DOWN;
	g_ulFrontKey &= (~FRONT_KEY_DOWN);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_Left()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_LEFT_STR);

	g_ulLastPressedFKey = FRONT_KEY_LEFT;
	g_ulFrontKey &= (~FRONT_KEY_LEFT);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_Right()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_RIGHT_STR);

	g_ulLastPressedFKey = FRONT_KEY_RIGHT;
	g_ulFrontKey &= (~FRONT_KEY_RIGHT);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_Menu()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_MENU_STR);

	g_ulLastPressedFKey = FRONT_KEY_MENU;
	g_ulFrontKey &= (~FRONT_KEY_MENU);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
FAI_ERR_CODE FAI_FKEY_Back()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_BACK_STR);

	g_ulLastPressedFKey = FRONT_KEY_BACK;
	g_ulFrontKey &= (~FRONT_KEY_BACK);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_PASS_STR);
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
#if defined(CONFIG_DI20)
FAI_ERR_CODE FAI_FKEY_SwReset()
{
	INTRN_FAI_FKEY_Toggle();

	FDI_PANEL_Display(FAI_FKEY_SWRESET_STR);

	g_ulLastPressedFKey = FRONT_KEY_SW_REBOOT;
	g_ulFrontKey &= (~FRONT_KEY_SW_REBOOT);
	if( g_ulFrontKey == 0 )
	{
		FDI_PANEL_Display(FAI_FKEY_SWRESET_STR);
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
FAI_ERR_CODE INTRN_FAI_FKEY_Reset()
{
	g_ulFrontKeyNum = 0;

#if	(FKEY_STANDBY_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_POWER;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_GUIDE_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_GUIDE;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_OK_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_OK;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_CHUP_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_CHUP;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_CHDOWN_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_CHDOWN;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_VOLUP_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_VOLUP;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_VOLDOWN_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_VOLDOWN;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_TVRADIO_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_TVRADIO;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_PLAY_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_PLAY;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_INFO_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_INFO;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_WPS_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_WPS;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_NETWORK_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_NETWORK;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_UP_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_UP;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_DOWN_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_DOWN;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_LEFT_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_LEFT;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_RIGHT_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_RIGHT;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_MENU_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_MENU;
	g_ulFrontKeyNum++;
#endif
#if	(FKEY_BACK_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_BACK;
	g_ulFrontKeyNum++;
#endif
#if (FKEY_SWREBOOT_UI_ENABLE==YES)
	g_ulFrontKey |= FRONT_KEY_SW_REBOOT;
	g_ulFrontKeyNum++;
#endif

	FAI_PRINT(FAI_PRT_DBGINFO, ("[FAI_FKEY_Init] : Fkeys=%08x, KeyNum=%d\n", g_ulFrontKey, g_ulFrontKeyNum));

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
FAI_ERR_CODE INTRN_FAI_FKEY_Toggle()
{
	if( GET_STEP() == 0 )
	{
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLACK);
		SET_STEP(1);
	}
	else if( GET_STEP() == 1 )
	{
		FAI_TESTMENU_ChangeCurrentRCUColor(RGB_COLOR_BLUE);
		SET_STEP(0);
	}

	return FAI_NO_ERR;
}

