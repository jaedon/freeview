/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 RCU Test
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/*******************************************************************************
* header and definitions
*******************************************************************************/
/* global header files */
#if !defined(CONFIG_OS_RTOS)
#include <memory.h>
#endif

/* chip manufacture's  header files */

/* humax header files */
#include "fvk.h"
#include "fai_common.h"
#include "fai_testmenu.h"
#include "fai_testparams.h"
#include "fai_tmd.h"
#include "fai_rcu.h"
#include "fdi_key.h"

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
	int	 nKeyValue;
} FAI_RCU_MSG_t;

/*******************************************************************************
* External variables and functions
*******************************************************************************/
/* external variables  and functions */

/*******************************************************************************
* Global variables and structures
*******************************************************************************/
/* global variables and structures */
unsigned long g_ulRCUMsgId, g_ulRcuTaskId;

unsigned short g_usRcuMsgId[MAX_TMD_TEST_MULTI_INDEX],g_usRcuKeyId[MAX_TMD_TEST_MULTI_INDEX];
/*******************************************************************************
* function : INTRN_FAI_RCU_TestTask
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
void INTRN_FAI_RCU_TestTask(void *data)
{
	FAI_TMD_RESULT_T tTMDResult;
	unsigned short usErrCode = FAI_TMD_DEFAULT_ERROR_CODE;
	FAI_RCU_MSG_t tKeyMsg;
	FVK_ERR_CODE eVkRet;
	unsigned char aucAgcResult[4];
	
	int nIndex;
	while(1)
	{
		eVkRet = FVK_MSG_Receive(g_ulRCUMsgId, &tKeyMsg, sizeof(FAI_RCU_MSG_t));
		if (eVkRet != FVK_NO_ERR)
		{
			FAI_PRINT(FAI_PRT_WARNING, ("[INTRN_FAI_RCU_TestTask] : Error FVK_MSG_Receive() %d\n", eVkRet));
			continue;
		}

		for(nIndex = 0 ; nIndex < 32  ; nIndex++)	
		{
			if(g_usRcuKeyId[nIndex]==tKeyMsg.nKeyValue)
			{
				g_usRcuKeyId[nIndex] = 0;
				break;
			}
		}

		if(nIndex < 32)
		{
			FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));

			aucAgcResult[0] = (tKeyMsg.nKeyValue&0xFF000000)>>24;
			aucAgcResult[1] = (tKeyMsg.nKeyValue&0x00FF0000)>>16;
			aucAgcResult[2] = (tKeyMsg.nKeyValue&0x0000FF00)>>8;
			aucAgcResult[3] = (tKeyMsg.nKeyValue&0x000000FF);
				
			tTMDResult.ucCategory = 0x04;
			tTMDResult.ucModule 	= 0x04;
			tTMDResult.ucAction 	= 0x02;
			tTMDResult.usMsgId	= g_usRcuMsgId[nIndex];

			tTMDResult.eResult 	= FAI_TMD_ERR_OK;
			FAI_TMD_AddResultParam(&tTMDResult, aucAgcResult, 4);
			
			FAI_TMD_SetResult(&tTMDResult);
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
FAI_ERR_CODE FAI_RCU_Init()
{
	if( FVK_MSG_Create(FVK_TASK_DEFAULT_QUEUE_SIZE, sizeof(FAI_RCU_MSG_t), (char*)"KeyMsg", &g_ulRCUMsgId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_RCU_Init] : FVK_MSG_Create \n"));
		return FAI_ERR;
	}
	if( FVK_TASK_Create(INTRN_FAI_RCU_TestTask, FVK_TASK_DEFAULT_PRIORITY, FVK_TASK_DEFAULT_STACK_SIZE, "Key Task", NULL, &g_ulRcuTaskId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_RCU_Init] : FVK_TASK_Create Error!!\n"));
		return FAI_ERR;
	}
	if( FVK_TASK_Start(g_ulRcuTaskId) != FVK_NO_ERR )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_RCU_Init] : FVK_TASK_Start Error!!\n"));
		return FAI_ERR;
	}
	memset(g_usRcuKeyId,0,MAX_TMD_TEST_MULTI_INDEX);
	
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
#if (FRONT_RCU_ENABLE==YES)
FAI_ERR_CODE FAI_RCU_SetTestKey(unsigned short usMsgId,unsigned char ucArgc, unsigned char *pucArgList)
{
	unsigned char ucKey[4],ucKeyLen;
	unsigned long nKey;
	int nIndex;
	unsigned char ucTmdErrCode[2];
	FAI_TMD_RESULT_T tTMDResult;
	
	if( ucArgc != 1 )
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_RCU_SetTestKey] : Argument Count Error=%d\n", ucArgc));
		FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
		
		tTMDResult.ucCategory = FAI_CATEGORY_FRONT_PANEL;
		tTMDResult.ucModule = 4;
		tTMDResult.ucAction = 2;
		tTMDResult.usMsgId	= usMsgId;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_COMMON;
		ucTmdErrCode[1] = FAI_COMMON_TMD_ERR_INVALID_ARGUMENT;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		
		FAI_TMD_SetResult(&tTMDResult);
		
		return FAI_ERR;
	}

	ucKeyLen = FAI_TMD_GetParam(ucKey, pucArgList, 0);
	FAI_TMD_ArgToValue(&nKey, ucKey, ucKeyLen);
	
	for(nIndex = 0 ; nIndex < 32  ; nIndex++)	
	{
		if(g_usRcuKeyId[nIndex] == 0)
		{
			g_usRcuKeyId[nIndex] = nKey;
			break;
		}
	}
	if(nIndex == 32)
	{
		FVK_MEM_Set(&tTMDResult, 0x00, sizeof(tTMDResult));
		
		tTMDResult.ucCategory = FAI_CATEGORY_FRONT_PANEL;
		tTMDResult.ucModule = 4;
		tTMDResult.ucAction = 2;
		tTMDResult.usMsgId	= usMsgId;
		tTMDResult.eResult = FAI_TMD_ERR_FAIL;
		ucTmdErrCode[0] = FAI_MODULE_TMD_ERR_KEY;
		ucTmdErrCode[1] = FAI_RCU_TMD_ERR_INVALID_KEY_NUMBER;
		FAI_TMD_AddResultParam(&tTMDResult, &ucTmdErrCode[0], sizeof(ucTmdErrCode));	
		
		FAI_TMD_SetResult(&tTMDResult);
	}

	g_usRcuMsgId[nIndex] = usMsgId;

	return FAI_NO_ERR;
}
#endif
void FAI_RCU_CALLBACK(unsigned int unKeyCode)
{
#if (FRONT_RCU_ENABLE==YES)
	FAI_RCU_MSG_t tKeyMsg;
	FVK_ERR_CODE eVkRet;

	FAI_PRINT(FAI_PRT_ERROR, ("[FAI_RCU_CALLBACK] : ++(keycode=0x%x)\n",unKeyCode));
	
	tKeyMsg.nKeyValue 	=	unKeyCode;
	eVkRet = FVK_MSG_Send(g_ulRCUMsgId, &tKeyMsg, sizeof(FAI_RCU_MSG_t));
	if (eVkRet != FVK_NO_ERR)
	{
		FAI_PRINT(FAI_PRT_ERROR, ("[FAI_RCU_CALLBACK] : Error FVK_MSG_Send() : %d\n", eVkRet));
		return;
	}
#endif	
}


