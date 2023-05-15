/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hama_debug.c
	@brief

	Description:  									\n
	Module: HaMa debug function						\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

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
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include	<hlib.h>

#include	<directfb.h>
#include	<sawman.h>

#include	"hama_int.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static void		hama_debug_printWindowList(HxList_t *pstWindowList)
{
	HamaWindowInfo_t	*windowInfo;

	while (pstWindowList)
	{
		windowInfo = (HamaWindowInfo_t*)HLIB_LIST_Data(pstWindowList);

		HLIB_CMD_Printf("   - applId [0x%08x] hookProcessPid [%d] Pid [%d]\n", windowInfo->ulApplId, windowInfo->stHookPid, windowInfo->stProcessPid);
		HLIB_CMD_Printf("   - alreadyRemovePlane(%d) nCurrentOpacity(%02x) nLogicalOpacity(%02x)\n", windowInfo->bAlreadyRemovePlane, windowInfo->nCurrentOpacity, windowInfo->nLogicalOpacity);
		HLIB_CMD_Printf("   - style [0x%08x] focus [0x%08x] stack [0x%08x] panel [0x%08x]\n", windowInfo->eWindowStyle, windowInfo->nFocusPriority, windowInfo->nStackPriority, windowInfo->nPanelPriority);
		HLIB_CMD_Printf("   - SaWManWindowHandle [0x%08x] dfbWindowId [0x%08x]\n\n", windowInfo->sWindowHandle, windowInfo->ulDFBWindowId);

		pstWindowList = pstWindowList->next;
	}
}

static void		hama_debug_printProcessInfo(HamaProcessInfo_t *pstProcessInfo)
{
	HLIB_CMD_Printf("ProcessInfo\n");
	HLIB_CMD_Printf("  -- ProcessName   : [%s]\n", pstProcessInfo->szProcessName);
	HLIB_CMD_Printf("  --     PID       : [%d]\n", pstProcessInfo->stProcessPid);
	HLIB_CMD_Printf("  -- Socket Handle : [%d]\n", pstProcessInfo->nSocketHandle);
	HLIB_CMD_Printf("  -- state         : [%d]\n", pstProcessInfo->nSocketHandle);
	HLIB_CMD_Printf("  --    Running    : [%d sec]\n", (HLIB_STD_GetSystemTick() - pstProcessInfo->ulStartTime) / 1000);
	HLIB_CMD_Printf("  Window(Application) List \n");
	hama_debug_printWindowList(pstProcessInfo->pstWindowList);
	HLIB_CMD_Printf("\n");
}

static HBOOL	hama_debug_process_findListByPid(void *pvPid, void *pvProcInfo)
{
	pid_t	pid = (pid_t)pvPid;
	HamaProcessInfo_t	*pstProcInfo = (HamaProcessInfo_t*)pvProcInfo;

	if (pstProcInfo->stProcessPid == pid)
		return TRUE;

	return FALSE;
}

#if	0
static void	hama_debug_process_CheckOrderChange(HxList_t *pstAttachedProcessList)
{
	HINT32				i = 0;
	HxList_t			*procList = NULL;
	HamaProcessInfo_t	*pstProcInfo;
	static	pid_t		s_arrProcessPid[32] = {0, };

	if(s_arrProcessPid[0] == 0)
	{
		i = 0;
		HxSTD_MemSet(s_arrProcessPid, 0, sizeof(pid_t) * 32);
		procList = pstAttachedProcessList;
		while (procList)
		{
			pstProcInfo = (HamaProcessInfo_t*)HLIB_LIST_Data(procList);

			s_arrProcessPid[i++] = pstProcInfo->stProcessPid;
			if(i == 32) break;
			procList = procList->next;
		}
		return;
	}

	i = 0;
	procList = pstAttachedProcessList;
	while (procList)
	{
		pstProcInfo = (HamaProcessInfo_t*)HLIB_LIST_Data(procList);
		if(s_arrProcessPid[i++] != pstProcInfo->stProcessPid)
		{
			HxLOG_Critical("Order change detected!!\r\n");
			HxLOG_Critical("Order change detected!!\r\n");

			HxLOG_StringDump(pstProcInfo->szProcessName);
			HxLOG_HexIntDump(pstProcInfo->stProcessPid);
			HxLOG_DecIntDump(pstProcInfo->ulStartTime);
			HxLOG_HexIntDump(pstProcInfo->nSocketHandle);
			s_arrProcessPid[0] = 0;
		}
		if(i == 32) break;
		procList = procList->next;
	}

}
#endif

static int	hama_command_printProcessInfo(void *szArgs)
{
	HAMA_t		*pstHama;
	HxList_t	*list;
	HINT32		pid;
	HINT8 		*szArg;

	szArg = HLIB_CMD_GetNextArg((HCHAR **)&szArgs);
	if( szArg == NULL )
	{
		return HxCMD_ERR;
	}
	pid =  HLIB_STD_StrToINT32(szArg);

	pstHama = HAMA_GetInstance();

	if (pstHama)
	{
		HamaProcessInfo_t	*pstProcessInfo;

		if (pid < 0)
		{
			list = pstHama->pstAttachedProcessList;
			while(list)
			{
				pstProcessInfo = (HamaProcessInfo_t*)HLIB_LIST_Data(list);
				hama_debug_printProcessInfo(pstProcessInfo);

				list = list->next;
			}
		}
		else
		{
			list = HLIB_LIST_FindEx(pstHama->pstAttachedProcessList, (void*)pid, hama_debug_process_findListByPid);
			if (!list)
			{
				HAMA_ReleaseInstance(pstHama);
				return HxCMD_ERR;
			}

			pstProcessInfo = (HamaProcessInfo_t*)HLIB_LIST_Data(list);

			hama_debug_printProcessInfo(pstProcessInfo);
		}

		HLIB_CMD_Printf("Zombie Window List\n");
		hama_debug_printWindowList(pstHama->pstZombieWindowList);
		HLIB_CMD_Printf("End\n");
	}

	HAMA_ReleaseInstance(pstHama);

	return HxCMD_OK;
}

static HERROR	hama_command_doHamaApplicationDelete(HAMA_t *pstHama, pid_t eProcPid)
{
	HLIB_CMD_Printf("<--------------- Do not NEED Anymore.... [%s][%d]\n", __FUNCTION__, __LINE__);
	HLIB_CMD_Printf("<--------------- Do not NEED Anymore.... [%s][%d]\n", __FUNCTION__, __LINE__);
	HLIB_CMD_Printf("<--------------- Do not NEED Anymore.... [%s][%d]\n", __FUNCTION__, __LINE__);
	HLIB_CMD_Printf("<--------------- Do not NEED Anymore.... [%s][%d]\n", __FUNCTION__, __LINE__);
#if	0
	HBOOL		bSend;
	HamaProcessInfo_t	*procInfo;
	HamaWindowInfo_t	*windowInfo;

	bSend = FALSE;
	procInfo = HAMA_TOOL_GetProcessInfoByPid(applMgr, eProcPid);
	if (procInfo)
	{
		while (procInfo->pstWindowList)
		{
			windowInfo = (HamaWindowInfo_t*)HLIB_LIST_Data(procInfo->pstWindowList);
			hideWindowFromPlane(applMgr, windowInfo->sWindowHandle, 0);
			hama_window_removeWindowList(applMgr, procInfo, windowInfo);

			bSend = TRUE;
		}
	}

	if (bSend)
	{
		HamaContents_u 	contentInfo;

		contentInfo.nContents = 0;
		hama_signal_emitTarget(applMgr, getpid(), eHSIG_ShutDown, eHSIGTYPE_NUMBER, sizeof(HUINT32), contentInfo, procInfo->nSocketHandle);

		return ERR_OK;
	}
#endif
	return ERR_FAIL;
}

static int	hama_command_remove_delete_application(void *szArgs)
{
	HAMA_t		*pstHama;
	HUINT32 	pid;
	HINT32		nMonitor;
	HINT8		*szArg_pid, *szArg_monitor;

	szArg_pid = HLIB_CMD_GetNextArg((HCHAR **)&szArgs);
	if( szArg_pid == NULL )
		return HxCMD_ERR;

	pid =  HLIB_STD_StrToUINT32(szArg_pid);

	szArg_monitor = HLIB_CMD_GetNextArg((HCHAR **)&szArgs);
	if( szArg_monitor == NULL )
		nMonitor = 1;
	else
		nMonitor = HLIB_STD_StrToINT32(szArg_monitor);

	HLIB_CMD_Printf("\n\n[%s,%d]  pid : %d, nMonitor : %d\n\n",__FUNCTION__,__LINE__, pid, nMonitor);

	pstHama = HAMA_GetInstance();
#if 0
	HAMA_SCENARIO_SetMonitor(HLIB_STD_GetProcessName(pid), (HBOOL)nMonitor);
#endif
	hama_command_doHamaApplicationDelete(pstHama, pid);
	HAMA_ReleaseInstance(pstHama);

	return HxCMD_OK;
}

static void	hama_command_print_VMRSS(HCHAR *pszProcName, pid_t pid)
{
#ifdef	OS_LINUX
	HCHAR	procName[256];
	HCHAR	line[256];
	FILE	*fp;

	HxSTD_snprintf(procName, 256, "/proc/%d/status", pid);

	if (fp = fopen(procName, "r"), NULL == fp)
	{
		HLIB_CMD_Printf("error to open file : [%s]\n", procName);
		return;
	}

	while (fgets(line, 255, fp))
	{
		if (HLIB_STD_StrCaseStartWith(line, "Name") || HLIB_STD_StrCaseStartWith(line, "VmRSS") || HLIB_STD_StrCaseStartWith(line, "VmSize"))
		{
			HLIB_CMD_Printf("%s", line);
		}
	}
	fclose(fp);
#endif
}

static HINT32	hama_command_getProcessId(HCHAR *pszProcName)
{
#ifdef	OS_LINUX
	DIR	*dir_p;
	struct	dirent	*dir_entry;
	HCHAR	dirname[40];
	HCHAR	targetname[252];
	HINT32	targetresult;
	HCHAR	exelink[252];
	HINT32	errorcount;
	HINT32	result;

	errorcount=0;
	result=0;
	dir_p = opendir("/proc/");

	while(NULL != (dir_entry = readdir(dir_p)))
	{
		if (strspn(dir_entry->d_name, "0123456789") == strlen(dir_entry->d_name))
		{
			strncpy(dirname, "/proc/", sizeof(dirname)-1);
			strncat(dirname, dir_entry->d_name, sizeof(dirname)-1);
			strncat(dirname, "/", sizeof(dirname)-1);
			exelink[0] = 0;
			strncat(exelink, dirname, sizeof(exelink)-1);
			strncat(exelink, "exe", sizeof(exelink)-1);
			targetresult = readlink(exelink, targetname, sizeof(targetname)-1);
			if (targetresult > 0)
			{
				targetname[targetresult] = 0;
				if (strstr(targetname, pszProcName) != NULL)
				{
					result = atoi(dir_entry->d_name);
					closedir(dir_p);
					return result;
				}
			}
		}
	}

	closedir(dir_p);

	return result;
#endif
}

static int	hama_command_print_memory_usage(void *szArgs)
{
	HAMA_t	*pstHama;
	HINT8	*pszArgProcName;

	HxList_t	*list;
	HamaProcessInfo_t	*procInfo;

	pstHama = HAMA_GetInstance();

	pszArgProcName = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if (pszArgProcName == NULL)
	{
		list = pstHama->pstAttachedProcessList;
		while (list)
		{
			procInfo = (HamaProcessInfo_t*)HLIB_LIST_Data(list);
			list = list->next;
			hama_command_print_VMRSS(procInfo->szProcessName, procInfo->stProcessPid);
		}
	}

	HAMA_ReleaseInstance(pstHama);

	return HxCMD_OK;
}

static int hama_printWindowOrderStatus(void *szArgs)
{
	HAMA_t	*pstHama;
	HxList_t *list;
	HxList_t *windowList;
	HxList_t *processList;
	HamaWindowInfo_t	*windowInfo;

	HamaProcessInfo_t	*processInfo;

	HamaKeyProcessInfo_t *pstKeyProcInfo;

	pstHama = HAMA_GetInstance();

	HLIB_CMD_Printf("\n\n");


	processList = pstHama->pstAttachedProcessList;

	HLIB_CMD_Printf("\nPROCESS INFO ____________________\n\n");
	while(processList)
	{
		processInfo = (HamaProcessInfo_t*)HLIB_LIST_Data(processList);

		HLIB_CMD_Printf("[[[processName : %s ,     pid : 0x%X]]]\n",
			processInfo->szProcessName, processInfo->stProcessPid);

		windowList = processInfo->pstWindowList;
		#if 0
		while(windowList)
		{

			windowInfo = (HamaWindowInfo_t*)HLIB_LIST_Data(windowList);
			HLIB_CMD_Printf("\t-- window id : 0x%X, applID: 0x%X,  windowStyle : 0x%x, stackPriority : 0x%x, focusPriority : 0x%X, currentOpacity : 0x%X\n",
				windowInfo->ulDFBWindowId, windowInfo->ulApplId, windowInfo->eWindowStyle,
				windowInfo->nStackPriority, windowInfo->nFocusPriority, windowInfo->nCurrentOpacity);

			windowList = windowList->next;
		}
		#endif
		hama_debug_printWindowList(windowList);
		HLIB_CMD_Printf("\n");

		processList = processList->next;
	}

	HLIB_CMD_Printf("\n[   Window List Focus Order     ] \n");

	list = pstHama->pstWindowListFocusOrder;
	while(list)
	{
		windowInfo = (HamaWindowInfo_t*)HLIB_LIST_Data(list);
		if (windowInfo)
		{
			processInfo = HAMA_TOOL_GetProcessInfoByPid(pstHama, windowInfo->stProcessPid);

			HLIB_CMD_Printf("windowID : 0x%X ,  applID: 0x%X,   stackPriority : 0x%X,  FocusPriority : 0x%X\t\tprocessName : %s\n",
				windowInfo->ulDFBWindowId, windowInfo->ulApplId, windowInfo->nStackPriority,
				windowInfo->nFocusPriority, processInfo->szProcessName);
		}
		list = list->next;
	}

	pstKeyProcInfo = &(pstHama->stKeyProcessInfo);

	if (pstKeyProcInfo->pstCurrentProcessInfo)
	{
		HLIB_CMD_Printf("\ncurrent focused process name : %s\n\n",
		pstKeyProcInfo->pstCurrentProcessInfo->szProcessName);
	}

	if (pstKeyProcInfo->pstCurrentWindowInfo)
	{
		HLIB_CMD_Printf("\ncurrent Received key window ID : 0x%X, applID: 0x%X, currentOpacity: 0x%X\n\n",
		pstKeyProcInfo->pstCurrentWindowInfo->ulDFBWindowId,
			pstKeyProcInfo->pstCurrentWindowInfo->ulApplId,
			pstKeyProcInfo->pstCurrentWindowInfo->nCurrentOpacity);
	}

	HLIB_CMD_Printf("\n\n\n");

	HAMA_ReleaseInstance(pstHama);

	return 0;
}

static int hama_printRPCFunction(void *szArgs)
{
	HAMA_t	*pstHama = HAMA_GetInstance();

	HLIB_RPC_PrintHelp(pstHama->nRPCHandle, NULL);

	HAMA_ReleaseInstance(pstHama);

	return 0;
}

#ifdef	CONFIG_DEBUG
extern HERROR	HAMA_REGISTRY_PrintCMD(HAMA_t *pstHama);
#endif
static int	hama_printRegistry(void *szArgs)
{
	HAMA_t	*pstHama = HAMA_GetInstance();

#ifdef	CONFIG_DEBUG
	HAMA_REGISTRY_PrintCMD(pstHama);
#endif

	HAMA_ReleaseInstance(pstHama);

	return 0;
}

static int	hama_ProcessMonitor(void *szArgs)
{
	HINT8	*szArg;

	szArg = HLIB_CMD_GetNextArg((HCHAR **)&szArgs);
	if( szArg == NULL )
	{
		return HxCMD_ERR;
	}

	if(HxSTD_StrNCmp(szArg,"start",HxSTD_StrLen(szArg))==0)
	{
		HLIB_CMD_Printf("Start monitor\n");
		HAMA_TOOL_ProcessMonitor(TRUE);
	}
	else
	{
		HLIB_CMD_Printf("Stop monitor\n");
		HAMA_TOOL_ProcessMonitor(FALSE);
	}
	return HxCMD_OK;
}


static int	hama_changeOrder(void *szArgs)
{
	HAMA_t	*pstHama;
	HINT8	*szArg;
	HINT32	nOrderType, nTargetId, nBaseId, nRelation;

	szArg = HLIB_CMD_GetNextArg((HCHAR **)&szArgs);
	if( szArg == NULL )
		return HxCMD_ERR;
	nOrderType = HLIB_STD_StrToINT32(szArg);

	szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if (szArg == NULL)
		return HxCMD_ERR;
	nTargetId = HLIB_STD_StrToINT32(szArg);

	szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if (szArg == NULL)
		return HxCMD_ERR;
	nBaseId = HLIB_STD_StrToINT32(szArg);

	szArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgs);
	if (szArg == NULL)
		return HxCMD_ERR;
	nRelation = HLIB_STD_StrToINT32(szArg);

	pstHama = HAMA_GetInstance();

	switch (nOrderType)
	{
		case eHSTACK_FOCUS:
			HAMA_TOOL_ChangeFocusOrder(pstHama, nTargetId, nBaseId, nRelation);
			break;
		case eHSTACK_WINDOW:
			HAMA_TOOL_ChangeStackOrder(pstHama, nTargetId, nBaseId, nRelation);
			break;
		default :
			break;
	}

	HAMA_ReleaseInstance(pstHama);

	return HxCMD_OK;
}

void	HAMA_DEBUG_InitCommandTools(void)
{
#define	cmdHandle		"hamacmd"
	HLIB_CMD_RegisterWord(cmdHandle, hama_changeOrder,
					 (HINT8 *)"changeorder",
					 (HINT8 *)"change stack or focus order : changeorder (0:1)(focus:window) targetid baseid (0:1)(up/down)",
					 (HINT8 *)"changeorder 0 1 3 0");

	HLIB_CMD_RegisterWord(cmdHandle, hama_command_printProcessInfo,
					 (HINT8 *)"print",
					 (HINT8 *)"hama process info print",
					 (HINT8 *)"pid (all is pid < 0)");

	HLIB_CMD_RegisterWord(cmdHandle, hama_command_remove_delete_application,
					 (HINT8 *)"remove",
					 (HINT8 *)"remove application",
					 (HINT8 *)"pid , monitor(1:0)");

	HLIB_CMD_RegisterWord(cmdHandle, hama_command_print_memory_usage,
					 (HINT8 *)"printmem",
					 (HINT8 *)"VmRSS : Physically used, VmSize : Virtually used",
					 (HINT8 *)"printmem filename (if filename is existed, print hama usage only)");

	HLIB_CMD_RegisterWord(cmdHandle, hama_printWindowOrderStatus,
					 (HINT8 *)"printorder",
					 (HINT8 *)"window priority status",
					 (HINT8 *)"no param");

	HLIB_CMD_RegisterWord(cmdHandle, hama_printRPCFunction,
					 (HINT8 *)"printrpc",
					 (HINT8 *)"print rpc",
					 (HINT8 *)"print rpc");

	HLIB_CMD_RegisterWord(cmdHandle, hama_printRegistry,
					 (HINT8 *)"printreg",
					 (HINT8 *)"printreg",
					 (HINT8 *)"printreg");

	HLIB_CMD_RegisterWord(cmdHandle, hama_ProcessMonitor,
					 (HINT8 *)"monitor",
					 (HINT8 *)"start or stop system monitor",
					 (HINT8 *)"[start/stop]");

}

