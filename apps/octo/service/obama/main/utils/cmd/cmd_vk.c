/**
	@file     cmd_pal_key.c
	@brief    PAL/KEY command tool

	Description: 		\n
	Module: PAL/KEY			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/

/********************************************************************
 Header Files
 ********************************************************************/
#include <hlib.h>
#include <stb_params.h>
#include <octo_common.h>
#include <pal_transcoder.h>


#include <pal_transcoder.h>
#include <pal_ringbuf.h>




/********************************************************************
 Macro Definition
 ********************************************************************/



/********************************************************************
 External Variable Declaration
 ********************************************************************/

/********************************************************************
 Static Function
 ********************************************************************/

/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/


/************************
	Command 수행 함수
*************************/
#if defined(CONFIG_DEBUG)


STATIC int local_cmd_vk(void *szArgList)
{

	HCHAR	*pszArg = NULL;

	// get 1st argument
	pszArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if(pszArg == NULL)
	{
		pszArg = "all";
	}

	HLIB_CMD_Printf("[%s:%d] start cmd ... (vk %s)\r\n\r\n", __FUNCTION__, __LINE__, pszArg);
	if(HLIB_STD_StrCaseCmp(pszArg, "all") == 0)
	{
	}
	else if ( HLIB_STD_StrCaseCmp(pszArg, "sem")== 0 )
	{
		#if defined(VK_SEM_DEBUG)
			VK_SEM_PrintSemList();
		#else
			HLIB_CMD_Printf("[%s:%d] do not include VK_SEM_DEBUG", __FUNCTION__, __LINE__ );
		#endif
	}
	else if ( HLIB_STD_StrCaseCmp(pszArg, "task") == 0)
	{
		#if defined(VK_TASK_DEBUG)
			VK_TASK_PrintTaskList();
		#else
			HLIB_CMD_Printf("[%s:%d] do not include VK_TASK_DEBUG", __FUNCTION__, __LINE__ );
		#endif
	}
	else if ( HLIB_STD_StrCaseCmp(pszArg, "msg")== 0 )
	{
		#if defined(VK_MSG_DEBUG)
			VK_MSG_PrintMsgQList();
		#else
			HLIB_CMD_Printf("[%s:%d] do not include VK_MSG_DEBUG", __FUNCTION__, __LINE__ );
		#endif
	}
	else if ( HLIB_STD_StrCaseCmp(pszArg, "timer") == 0)
	{
		#if defined(VK_TIMER_DEBUG)
			VK_TIMER_PrintTimerList();
		#else
			HLIB_CMD_Printf("[%s:%d] do not include VK_TIMER_DEBUG", __FUNCTION__, __LINE__ );
		#endif
	}
	else
	{
		HLIB_CMD_Printf("[%s:%d] %s => Invalid Argument!!!", __FUNCTION__, __LINE__, pszArg);
		return -1;
	}

	HLIB_CMD_Printf("\r\n[%s:%d] start end ... (vk %s)\r\n", __FUNCTION__, __LINE__, pszArg);
	return 0;
}


#endif




/**********************************************
	Command 수행 함수를 등록하는 통합 등록 함수.
***********************************************/
void CMD_Register_VK(void)
{
#if defined(CONFIG_DEBUG)
#define	hCmdHandle		"VK"
	HLIB_CMD_RegisterWord(hCmdHandle,
					local_cmd_vk,
	/* keyword */	(HCHAR *)"vkcmd",
	/* help */		(HCHAR *)"vkcmd [mem/sem/task/timer]",
	/* usage */ 	(HCHAR *)"vkcmd [command] [arg1] (ex : vkcmd sem)");

#endif
}



