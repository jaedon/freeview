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

#include "htype.h"
#include "vkernel.h"
#include "di_uart.h"
#include "linden_trace.h"

#define DEBUG_MODULE	TRACE_MODULE_MON


#define USE_CMD_CRASH_TEST
#if defined(USE_CMD_CRASH_TEST)
void CMD_RegisterWord(HUINT32(*fnToDo)(void *arg), HINT8 *szWord, char *szHelp, char *szUsage);


// For Seminar
static HUINT32 P_CMD_Panic(void *szArgList)
{
	PrintEnter();
	VK_Panic();
	PrintExit();
	return 0;
}

static HUINT32 P_CMD_StackCrash(void *szArgList)
{
	HUINT8 ucTemp;
	HUINT32 *pulTemp;

	PrintEnter();

	pulTemp = (HUINT32*) &ucTemp;

	*pulTemp = 0;

	PrintExit();
	return 0;
}

static HUINT32 P_CMD_HeapCrash(void *szArgList)
{
	HUINT8 *pucTemp;

	PrintEnter();

	pucTemp = DD_MEM_Alloc(16);

	VK_memset(pucTemp, 0, 17);
	DD_MEM_Free(pucTemp);

	PrintExit();
	return 0;
}

static HUINT32 P_CMD_MakeMemoryLeak(void *szArgList)
{
	HUINT8 *pucTemp;

	PrintEnter();

	pucTemp = DD_MEM_Alloc(16);

	PrintExit();
	return 0;
}

static HUINT32 P_CMD_UnInitializedVar(void *szArgList)
{
	HUINT8 *p;
	HUINT32	var;

	PrintEnter();

	p = NULL;
	if (p != NULL)
	{
		var = 0;
	}

	DI_UART_Print("[P_CMD_UnInitializedVar] var [%d]\n", var);

	PrintExit();
	return 0;
}

static HUINT32 P_CMD_UnInitializedVar2(void *szArgList)
{
	HUINT8 *p;
	HUINT32	var;

	PrintEnter();

	p = NULL;
	if (p != NULL)
	{
		var = 0;
	}

	DI_UART_Print("[P_CMD_UnInitializedVar2] var [%d]\n", var);

	PrintExit();
	return 0;
}

static HUINT32 P_CMD_TestBoolFunc(void *szArgList)
{
	HBOOL			boolOk;
	HUINT32			is32Ok;
	HUINT8			is8Ok;

	PrintEnter();

	/* CMD_IS_BOOL_Func() is extern funcion in other .c fiel without function prototype */
	if (CMD_IS_BOOL_Func(0))
	{
		DI_UART_Print("[P_CMD_TestBoolFunc] should not be prited !!! : Logical Error\n");
	}
	is32Ok = CMD_IS_BOOL_Func(0);
	if (is32Ok)
	{
		DI_UART_Print("[P_CMD_TestBoolFunc] should not be prited !!! : Logical Error, isOk (%08X)\n", is32Ok);
	}
	boolOk = CMD_IS_BOOL_Func(0);
	if (boolOk)
	{
		DI_UART_Print("[P_CMD_TestBoolFunc] should not be prited !!! : Logical Error, boolOk (%08X)\n", boolOk);
	}
	is8Ok = CMD_IS_BOOL_Func(0);
	if (is8Ok)
	{
		DI_UART_Print("[P_CMD_TestBoolFunc] should not be prited !!! : Logical Error, boolOk (%08X)\n", boolOk);
	}

	PrintExit();
	return 0;
}

static HUINT32 CMD_TestFloatFunc(void *szArgList)
{
	PrintEnter();

	/* CMD_FLOAT_Func() is extern funcion in other .c fiel without function prototype */
	DI_UART_Print("[CMD_TestFloatFunc] CALL : CMD_FLOAT_Func(1.1)\n");
	CMD_FLOAT_Func(1.1);

	PrintExit();
	return 0;
}

#endif

void CMD_CrashTest_Init(void)
{

#if defined(USE_CMD_CRASH_TEST)
	CMD_RegisterWord(P_CMD_Panic, "crash_panic", "crash_panic", "crash_panic") ;
	CMD_RegisterWord(P_CMD_StackCrash, "crash_stack", "crash_stack", "crash_stack") ;
	CMD_RegisterWord(P_CMD_HeapCrash, "crash_heap", "crash_heap", "crash_heap") ;
	CMD_RegisterWord(P_CMD_MakeMemoryLeak, "crash_heapleak", "crash_heapleak", "crash_heapleak") ;
	CMD_RegisterWord(P_CMD_UnInitializedVar, "crash_uninit", "crash_uninit", "crash_uninit") ;
	CMD_RegisterWord(P_CMD_UnInitializedVar2, "crash_uninit2", "crash_uninit2", "crash_uninit2") ;
	CMD_RegisterWord(P_CMD_TestBoolFunc, "crash_boolfunc", "crash_boolfunc", "crash_boolfunc") ;
	CMD_RegisterWord(CMD_TestFloatFunc, "crash_floatfunc", "crash_floatfunc", "crash_floatfunc") ;
#endif

}
