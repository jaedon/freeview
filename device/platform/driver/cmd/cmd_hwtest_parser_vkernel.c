/*********************************************************************
*	author :
*
*	file descriptions....
*		:
*
*	Copyright (c) 2004 by Humax Co., Ltd.
*	All rights reserved
********************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <ctype.h>
#include <sys/syscall.h>

#include "htype.h"
#include "taskdef.h"
#include "vkernel.h"
#include "di_uart.h"
#include "cmdtool.h"

#define CMD_OK					0
#define CMD_ERR					1

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgs)

#define AssertDI(arg1, arg2) if(arg1 != DI_ERR_OK) {CMD_Printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

static unsigned long sSockServTaskId;
static unsigned long sSockCliTaskId[5];
//static HUINT32 hSocket;

extern void VK_TASK_PrintTaskList(void);

int CMD_HwTest_Parser_VK(void *szArgs)
{
	GET_ARGS;
//	DI_ERR_CODE diErr = DI_ERR_OK;

	// Slow Blanking : 12V(4:3), 6V(16:9), 0V(Int. TV)
	if CMD_IS("mem")
	{
		if PARAM_IS("all")
		{
		}
#if defined(AP_MEMORY_DEBUG)
		AP_MEM_PrintList();
#endif
#if defined(API_MEMORY_DEBUG)
		API_MEM_PrintList();
#endif
#if defined(MD_MEMORY_DEBUG)
		MD_MEM_PrintList();
#endif
#if defined(MW_MEMORY_DEBUG)
		MW_MEM_PrintList();
#endif
#if defined(PL_MEMORY_DEBUG)
		PL_MEM_PrintList();
#endif
#if defined(SI_MEMORY_DEBUG)
		SI_MEM_PrintList();
#endif
#if defined(CA_MEMORY_DEBUG)
		CA_MEM_PrintList();
#endif
#if defined(EX_MEMORY_DEBUG)
		EX_MEM_PrintList();
#endif
#if defined(DB_MEMORY_DEBUG)
		DB_MEM_PrintList();
#endif
#if defined(DD_MEMORY_DEBUG)
		DD_MEM_PrintList();
#endif
#if defined(FS_MEMORY_DEBUG)
		FS_MEM_PrintList();
#endif
#if defined(ZZ_MEMORY_DEBUG)
		ZZ_MEM_PrintList();
#endif
#if defined(DSMCC_MEMORY_DEBUG)
		DSMCC_MEM_PrintList();
#endif
#if defined(EPG_MEMORY_DEBUG)
		EPG_MEM_PrintList();
#endif
		iResult = CMD_OK;
	}
	else if CMD_IS("sem")
	{
#if defined(VK_SEM_DEBUG)
		VK_SEM_PrintSemList();
#endif
		iResult = CMD_OK;
	}
	else if CMD_IS("task")
	{
#if defined(CONFIG_DEBUG)
		VK_TASK_PrintTaskList();
#endif
		iResult=CMD_OK;
	}
#if defined(VK_MSG_DEBUG)
	else if CMD_IS("msg")
	{
		VK_MSG_PrintMsgQList();
		iResult=CMD_OK;
	}
#if 0
	else if CMD_IS("msgclear")
	{
		HUINT32 	ulMsgQId;
		HINT32		res;

		aucArg = CMD_NextArg((char **)&szArgList);
		if( aucArg == NULL ) return CMD_ERR;
		if( VK_sscanf(aucArg, "%s", &aucArg1) == 0 ) return CMD_ERR;

		if(VK_strncmp(aucArg1,"0x",2) == 0)   { res = VK_sscanf(aucArg1,"0x%x", &ulMsgQId); }
		else							   { res = VK_sscanf(aucArg1, "%d", &ulMsgQId); }
		if(res != 1)					   return CMD_ERR;

		VK_MSG_Clear(ulMsgQId);
	}
#endif
#endif
#if defined(VK_TIMER_DEBUG)
	else if CMD_IS("timer")
	{
		VK_TIMER_PrintTimerList();
		iResult=CMD_OK;
	}
#endif
#if defined(CONFIG_DEBUG)
	else if CMD_IS("intr")
	{
		VK_INTR_PrintIntrList();
		iResult = CMD_OK;
	}
#endif
	else if CMD_IS("sock")
	{
		VK_SOCKET_PrintSockInfo();
		iResult = CMD_OK;
	}
	else if CMD_IS("rpc")
	{
		VK_RPC_PrintRpcInfo();
		iResult = CMD_OK;
	}

	(void)(szParam1);
	(void)(szParam2);

	return iResult;
}
