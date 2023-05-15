/**************************************************************
*	@file		ondk.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
*	@author			humax
*	@brief			Minimized Graphic Library for DirectFB
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/
/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include 	<ondk.h>

#include 	"../gwm/ondk_gwm_internal.h"


/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/

#define ONDK_DEBUG_COMMANDNAME 		"ONDK"
#define COMMAND_NAME					ONDK_DEBUG_COMMANDNAME


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

STATIC HINT32 ondk_cmd_Info(void *szArgList)
{

	HLIB_CMD_Printf("Hello World..!!\n" );



	return HxCMD_OK;
}

STATIC HINT32 local_cmd_ondk_VK(void *szArgList)
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
			HLIB_CMD_Printf("[%s:%d] do not include VK_TASK_DEBUG", __FUNCTION__, __LINE__ );
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

STATIC HINT32 local_cmd_ondk_DumpString(void *szArgList)
{
	(void)szArgList;

	HLIB_CMD_Printf("============================================================================\n");
	DBG_ONDK_STRINGS_Dump(0);
	HLIB_CMD_Printf("============================================================================\n");


	return 0;
}

STATIC HINT32 local_cmd_ondk_DumpImage(void *szArgList)
{
	(void)szArgList;

	HLIB_CMD_Printf("============================================================================\n");
	DBG_ONDK_IMG_Dump(0);
	HLIB_CMD_Printf("============================================================================\n");


	return 0;
}


STATIC HINT32 local_cmd_ondk_DumpFont(void *szArgList)
{
	(void)szArgList;

	HLIB_CMD_Printf("============================================================================\n");
	DBG_ONDK_FONT_Dump(0);
	HLIB_CMD_Printf("============================================================================\n");


	return 0;
}

STATIC HINT32 local_cmd_ondk_DrawUpdateRect(void *szArgList)
{
	(void)szArgList;

	HLIB_CMD_Printf("============================================================================\n");
	DBG_ONDK_GWM_GFX_UpdateScreenShowLine();
	HLIB_CMD_Printf("============================================================================\n");

	return 0;
}

STATIC HINT32 local_cmd_ondk_test(void *szArgList)
{
	HCHAR	*pszArg = NULL;

	ONDK_TEST_Init();

	HLIB_CMD_Printf("============================================================================\n");

	pszArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	while(1)
	{
		ONDK_TEST_Start(pszArg);
		pszArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(NULL == pszArg)
		{
			break;
		}
	}

	HLIB_CMD_Printf("============================================================================\n");

	return 0;
}



/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/


ONDK_Result_t  ONDK_DEBUG_Init(HINT32 argc, HCHAR **argv)
{
	HLIB_CMD_RegisterWord(COMMAND_NAME,
						ondk_cmd_Info,
						"ondkinfo",
						"ondkinfo",							/*   help  */
						"ondkinfo");						/* keyword */
#if 0
	HLIB_CMD_RegisterWord(COMMAND_NAME,
						ondk_cmd_VitualRcu,
						"hello",
						"help..",							/*   help  */
						"keyword..");						/* keyword */
#endif

	HLIB_CMD_RegisterWord(COMMAND_NAME,
						local_cmd_ondk_test,
						"ondktest",
						"ondktest osd/font/msghook/string/image",
						"ondktest osd/font/msghook/string/image");

	HLIB_CMD_RegisterWord(COMMAND_NAME,
						DBG_ONDK_GWM_TIMER_DumpList,
						"timerlist",
						"timerlist",
						"timerlist");

	HLIB_CMD_RegisterWord(COMMAND_NAME,
						DBG_ONDK_GWM_APP_DumpList,
						"gwmlist",
						"gwmlist",
						"gwmlist");

	HLIB_CMD_RegisterWord(COMMAND_NAME,
						local_cmd_ondk_DumpString,
						"stringlist",
						"stringlist",
						"stringlist");

	HLIB_CMD_RegisterWord(COMMAND_NAME,
						local_cmd_ondk_DumpImage,
						"imagelist",
						"imagelist",
						"imagelist");


	HLIB_CMD_RegisterWord(COMMAND_NAME,
						local_cmd_ondk_DumpFont,
						"fontlist",
						"fontlist",
						"fontlist");

	HLIB_CMD_RegisterWord(COMMAND_NAME,
						local_cmd_ondk_DrawUpdateRect,
						"updaterect",
						"updaterect",
						"updaterect");

	HLIB_CMD_RegisterWord(COMMAND_NAME,
					local_cmd_ondk_VK,
	/* keyword */	(HCHAR *)"vkcmd",
	/* help */		(HCHAR *)"vkcmd [mem/sem/task/timer]",
	/* usage */ 	(HCHAR *)"vkcmd [command] [arg1] (ex : vkcmd sem)");

	return ONDK_RET_OK;
}

