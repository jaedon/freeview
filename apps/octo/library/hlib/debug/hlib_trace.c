/**************************************************************
*	@file		hxtrace.c
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
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
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/resource.h>

#include <hlib.h>
#include <_hlib_dbg.h>

#define	MAX_NAME_LEN			12
#define	MAX_ITEM_LEN			8
#define	MAX_TRACE_DEFAULT_NUM	40
#define	MAX_FORMAT_NUM			64
#define	MAX_LINE_LEN			128

#define	__P_P				" %12p"
#define	__S					" %24s"
#define	__4S				" %4s"
#define	__12S				" %12s"
#define	__I					" %12d"
#define	__4I				" %4d"
#define	__12I				" %12d"
#define	__12S				" %12s"

#ifdef	ENABLE_DEBUG
/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
typedef struct
{
	HINT32		fd;

	HINT32		nArgs;
	HINT32		nTraceNum;
	HUINT32		ulTick;

	HCHAR		aszTitle[MAX_ITEM_LEN][MAX_NAME_LEN];
	HINT32		anTitleType[MAX_ITEM_LEN];
	HCHAR		szFormat[MAX_FORMAT_NUM];

	HCHAR		*pszLast;
	HCHAR		*pszLine;
	HCHAR		szTempLine[MAX_LINE_LEN];
	HCHAR		*pszTraceData;
} tracemodule_t;
typedef struct
{
	HCHAR	szProcName[MAX_NAME_LEN];
	HUINT8	nullBuffer[MAX_LINE_LEN * 4];

	HxHASH_t	*pstTable;
} trace_t;

static trace_t	*s_pstTrace = NULL;

/**********************************************************************************************************/
#define ____P_PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
static void	hxdbg_trace_write(tracemodule_t *pstModuleInfo)
{
	lseek(pstModuleInfo->fd, 0, SEEK_SET);
	write(pstModuleInfo->fd, pstModuleInfo->pszTraceData, MAX_LINE_LEN * pstModuleInfo->nTraceNum);
//	fseek(pstModuleInfo->file, 0, SEEK_SET);
//	fwrite(pstModuleInfo->pszTraceData, MAX_LINE_LEN * pstModuleInfo->nTraceNum, 1, pstModuleInfo->file);
//	ftruncate(fileno(pstModuleInfo->file), ftell(pstModuleInfo->file));
}

/**********************************************************************************************************/
#define ____P_PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/
void	HLIB_DBG_InitTrace(void)
{
	if (s_pstTrace == NULL)
	{
		s_pstTrace = (trace_t*)HLIB_MEM_Calloc(sizeof(trace_t));

		s_pstTrace->pstTable = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, HLIB_MEM_Free_CB, HLIB_MEM_Free_CB);
		HLIB_STD_StrNCpySafe(s_pstTrace->szProcName, HLIB_STD_GetCurProcessName(), MAX_NAME_LEN);
	}
}

void	HLIB_DBG_SetupTraceEx(HCHAR *module, HINT32 nTraceNum, HINT32 n, ...)
{
	HINT32			i, nTrace;
	HCHAR			*pszFormat;
	tracemodule_t	*pstModuleInfo;
	va_list			va;

	if (s_pstTrace == NULL)
		return;

	nTrace = (nTraceNum == -1 ? MAX_TRACE_DEFAULT_NUM : nTraceNum);
	if (nTrace <= 0)
	{
		HxLOG_Error("Error trace num!\n");
		HxLOG_Assert(0);
	}

	pstModuleInfo = (tracemodule_t*)HLIB_HASH_Lookup(s_pstTrace->pstTable, (void*)module);
	if (pstModuleInfo == NULL)
	{
		if (n > (MAX_ITEM_LEN - 1))
			n = MAX_ITEM_LEN - 1;

		pstModuleInfo = (tracemodule_t*)HLIB_MEM_Calloc(sizeof(tracemodule_t));
		pstModuleInfo->nArgs = n;
		pstModuleInfo->nTraceNum = nTrace;

		va_start(va, n);
		for (i = 0; i < n; i++)
		{
			pszFormat = va_arg(va, HCHAR*);
			if (HLIB_STD_StrStartWith(pszFormat, "p:"))
			{
				pstModuleInfo->anTitleType[i] = 0;
				HxSTD_StrCat(pstModuleInfo->szFormat, __P_P);
			}
			else if (HLIB_STD_StrStartWith(pszFormat, "i:"))
			{
				pstModuleInfo->anTitleType[i] = 1;
				HxSTD_StrCat(pstModuleInfo->szFormat, __I);
			}
			else if (HLIB_STD_StrStartWith(pszFormat, "s:"))
			{
				pstModuleInfo->anTitleType[i] = 2;
				HxSTD_StrCat(pstModuleInfo->szFormat, __S);
			}

			HLIB_STD_StrNCpySafe(pstModuleInfo->aszTitle[i], pszFormat + 2, MAX_NAME_LEN);
		}
		va_end(va);

		{
			HCHAR	szDir[128];
			HUINT32	ulFlag = O_WRONLY | O_CREAT;

			HxSTD_snprintf(szDir, 128, "/tmp/%s", s_pstTrace->szProcName);
			szDir[127] = 0;
			HLIB_DIR_Make(szDir);
			HLIB_DIR_Chmod(szDir, 666);

			HxSTD_snprintf(szDir, 128, "/tmp/%s/%s", s_pstTrace->szProcName, module);
			
			pstModuleInfo->fd = open(szDir, ulFlag, 0666);
		}

		pstModuleInfo->pszTraceData = HLIB_STD_MemCalloc(MAX_LINE_LEN * pstModuleInfo->nTraceNum);
		for (i = 0; i < pstModuleInfo->nTraceNum; i++)
			pstModuleInfo->pszTraceData[(i + 1) * MAX_LINE_LEN - 1] = '\n';
		pstModuleInfo->pszLast = pstModuleInfo->pszTraceData + MAX_LINE_LEN * (pstModuleInfo->nTraceNum - 1);

		HLIB_HASH_Insert(s_pstTrace->pstTable, (void*)HLIB_MEM_StrDup_CB(module), (void*)pstModuleInfo);
	}
}

#define	DBG_TICK_LEN	12

void*	HLIB_DBG_BeginTrace(HCHAR *module, ...)
{
	tracemodule_t	*pstModuleInfo;
	va_list	va;

	if (s_pstTrace == NULL)
		return NULL;

	pstModuleInfo = (tracemodule_t*)HLIB_HASH_Lookup(s_pstTrace->pstTable, (void*)module);
	if (pstModuleInfo == NULL)
	{
		HxLOG_Error("module is not registered!\n");
		return NULL;
	}

	HxSTD_MemMove(pstModuleInfo->pszTraceData + MAX_LINE_LEN, pstModuleInfo->pszTraceData + MAX_LINE_LEN * 2, MAX_LINE_LEN * (pstModuleInfo->nTraceNum - 2));
	pstModuleInfo->pszLine = pstModuleInfo->pszLast + DBG_TICK_LEN;
	HxSTD_MemSet(pstModuleInfo->pszLast, 0x20, DBG_TICK_LEN);
	pstModuleInfo->ulTick  = HLIB_STD_GetSystemTick();

	va_start(va, module);
	vsnprintf(pstModuleInfo->szTempLine, MAX_LINE_LEN - DBG_TICK_LEN - 1, pstModuleInfo->szFormat, va);
	HxSTD_MemCopy(pstModuleInfo->pszLine, pstModuleInfo->szTempLine, HxSTD_StrLen(pstModuleInfo->szTempLine));
	va_end(va);

	return (void*)pstModuleInfo;
}

void	HLIB_DBG_EndTrace(HCHAR *module, void *pvTrace)
{
	HUINT32		ulResTick;
	HCHAR		szTick[DBG_TICK_LEN - 1];
	tracemodule_t	*pstModuleInfo = (tracemodule_t*)pvTrace;

	if (s_pstTrace == NULL)
		return;

	pstModuleInfo = (tracemodule_t*)HLIB_HASH_Lookup(s_pstTrace->pstTable, (void*)module);
	if (pstModuleInfo == NULL)
	{
		HxLOG_Error("module is not registered!\n");
		return;
	}

	ulResTick = HLIB_STD_GetSystemTick() - pstModuleInfo->ulTick;
	HxSTD_snprintf(szTick, DBG_TICK_LEN - 1, "%11d", ulResTick);
	HxSTD_MemCopy(pstModuleInfo->pszLine - DBG_TICK_LEN, szTick, HxSTD_StrLen(szTick));

	hxdbg_trace_write(pstModuleInfo);

}
#endif
