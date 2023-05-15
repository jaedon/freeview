/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hama_tool.c
	@brief

	Description:  									\n
	Module: HaMa Event Handler						\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
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
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<hlib.h>
#include	<vkernel.h>
#include	<directfb.h>
#include 	<sys/sysinfo.h>
#include	<sawman.h>

#include	"hama_int.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HINT32 	nPid;
	HUINT64	ullUsedTime;
	HCHAR 	szName[32];
	HUINT32 unCpuRate;
	HUINT32 unMemRate;
	HULONG 	ulVmRssMB;

}hama_tool_ProcessInfo_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
static HxList_t  	*s_pAppInfoList = NULL;
static HBOOL 		s_bActive = FALSE;

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static HBOOL	hama_tool_findListByPid(void *pvPid, void *pvProcInfo)
{
	HINT32	pid = (HINT32)pvPid;
	HamaProcessInfo_t	*pstProcInfo = (HamaProcessInfo_t*)pvProcInfo;

	if (pstProcInfo->stProcessPid == pid)
		return TRUE;

	return FALSE;
}

static HBOOL	hama_tool_findListBySocketHandle(void *pvSocketHandle, void *pvProcessInfo)
{
	HINT32	nSocketHandle = (HINT32)pvSocketHandle;
	HamaProcessInfo_t	*pstProcInfo = (HamaProcessInfo_t*)pvProcessInfo;

	if (nSocketHandle == pstProcInfo->nSocketHandle)
		return TRUE;

	return FALSE;
}

static	HBOOL	hama_tool_findListBySHandle(void *givenData, void *listData)
{
	HamaWindowInfo_t	*windowInfo = (HamaWindowInfo_t*)listData;
	SaWManWindowHandle	givenHandle = (SaWManWindowHandle)givenData;

	if (windowInfo->sWindowHandle == givenHandle)
		return TRUE;

	return FALSE;
}

static HBOOL	hama_tool_findListByWindowStyle(void *pvFilter, void *pvInfo)
{
	HWINDOWSTYLE_e		eStyle         = (HWINDOWSTYLE_e)pvFilter;
	HamaWindowInfo_t	*pstWindowInfo = (HamaWindowInfo_t*)pvInfo;

	if (pstWindowInfo->eWindowStyle & eStyle)
		return TRUE;

	return FALSE;
}

static HamaWindowInfo_t*	hama_tool_findNextFocusWindowInfo(HAMA_t *applMgr, HamaWindowInfo_t *pstWindowInfo, HUINT32 ulSymbol)
{
	HxList_t	*inputFilterOn;
	HxList_t	*list;

	HxLOG_Debug("findNextFocusWindowInfo currentWindowInfo : [%p]\n", pstWindowInfo);
	if (pstWindowInfo == NULL)
		list = applMgr->pstWindowListFocusOrder;
	else
	{
		list = HLIB_LIST_Find(applMgr->pstWindowListFocusOrder, (void*)pstWindowInfo);
		if (list)
			list = list->next;
	}

	if (list == NULL)
		return NULL;

	inputFilterOn = HLIB_LIST_FindEx(applMgr->pstWindowListFocusOrder, (void*)eHWS_WindowInputFilter, hama_tool_findListByWindowStyle);
	HxLOG_Debug("InputFilter is [%p]\n", inputFilterOn);

	while (list)
	{
		HamaWindowInfo_t	*pstTempWindowInfo;

		pstTempWindowInfo = (HamaWindowInfo_t*)HLIB_LIST_Data(list);
		if (HLIB_LIST_Find(pstTempWindowInfo->acceptableKeyList, (void*)ulSymbol) || pstTempWindowInfo->acceptableKeyList == NULL)
		{
			if (!inputFilterOn || (pstTempWindowInfo->eWindowStyle & (eHWS_WindowInputFilter | eHWS_WindowInputAlways)))
			{
				if (pstTempWindowInfo->eWindowStyle & eHWS_WindowInputEnable)
				{
					HxLOG_Debug("found window [%d]...\n", pstTempWindowInfo->ulApplId);
					break;
				}
			}
		}

		list = list->next;
	}

	return (HamaWindowInfo_t*)HLIB_LIST_Data(list);
}

static HUINT32	hama_tool_getRequestId(void)
{
	static HUINT32	_inputEventRequestId = 0;

	_inputEventRequestId++;
	_inputEventRequestId %= 0x1FFFF;

	return _inputEventRequestId + 1;
}

static	HBOOL	hama_tool_findListByAppId(void *pvAppId, void *pvWindowInfo)
{
	HUINT32				applId = (HUINT32)pvAppId;
	HamaWindowInfo_t	*windowInfo = (HamaWindowInfo_t*)pvWindowInfo;

	if (windowInfo->ulApplId == applId)
		return TRUE;

	return FALSE;
}

static HBOOL	hama_tool_findListByWindowPid(void *pvPid, void *pvWindowInfo)
{
	pid_t	pid = (pid_t)pvPid;
	HamaWindowInfo_t	*windowInfo = (HamaWindowInfo_t*)pvWindowInfo;

	if (pid == windowInfo->stProcessPid)
		return TRUE;

	return FALSE;
}

static HxList_t *hama_tool_GetProcessStatus(void)
{
	HxDIR_t 	*pDir;
	HxList_t 	*pProcStatusList = NULL;

	pDir = HLIB_DIR_Open("/proc");
	if(pDir)
	{
		HCHAR *psName,*psPath;
		do
		{
			psName = HLIB_DIR_GetName(pDir);
			psPath = HLIB_DIR_GetPath(pDir);

			if(psName[0] != '.')
			{
				// proc/[pid]/stat
				if(HLIB_DIR_IsDir(pDir)== TRUE && HLIB_STD_IsInteger(psName) == TRUE)
				{
					HCHAR szFullPath[128] ={0,};

					HxSTD_snprintf(szFullPath,sizeof(szFullPath),"%s/%s/status",psPath,psName);
					pProcStatusList = HLIB_LIST_Append(pProcStatusList, (void*)HAMA_StrDup(szFullPath));
				}
			}
		}while(HLIB_DIR_Next(pDir) == ERR_OK);
		HLIB_DIR_Close(pDir);
	}

	return pProcStatusList;
}

static HBOOL hama_tool_pidcompare(void *pUser,void *pItem)
{
	HINT32 nPid 				= (HINT32)pUser;
	hama_tool_ProcessInfo_t	*pInfo 	= (hama_tool_ProcessInfo_t*)pItem;

	if(pInfo->nPid == nPid)
		return TRUE;
	return FALSE;
}

static HBOOL hama_tool_namecompare(void *pUser,void *pItem)
{
	HCHAR *pszName 				= (HCHAR*)pUser;
	hama_tool_ProcessInfo_t	*pInfo 	= (hama_tool_ProcessInfo_t*)pItem;

	if(HxSTD_StrNCmp(pInfo->szName ,pszName,sizeof(pInfo->szName))==0)
		return TRUE;
	return FALSE;
}

static HINT32 hama_tool_rss_compare(void *pUser,void *pItem)
{
	hama_tool_ProcessInfo_t	*pUserItem,*pCurItem;

	pUserItem = (hama_tool_ProcessInfo_t*)pUser;
	pCurItem = (hama_tool_ProcessInfo_t*)pItem;

	return pUserItem->ulVmRssMB - pCurItem->ulVmRssMB;
}

static void hama_tool_GetProcessStat(HUINT64 ullDiffJiffies)
{
	HxList_t			*pList;
	hama_tool_ProcessInfo_t	*pInfo;
	HxFILE_t			pFile;

	pList = s_pAppInfoList;
	while(pList)
	{
		HCHAR 		szPath[128]={0,};

		pInfo = (hama_tool_ProcessInfo_t*)HLIB_LIST_Data(pList);
		HxSTD_snprintf(szPath,sizeof(szPath),"/proc/%d/stat",pInfo->nPid);
		pFile = HLIB_FILE_Open(szPath,"r");
		if(pFile)
		{
			HINT32 nDummy=0,nPid=0;
			HCHAR  dDummy=0,szProcessName[24]={0,};
			HUINT64	ullDummy=0,ullUserTime=0,ullSystime=0,ullCurUsedTime;

			// get processname + usermode used time + kernelmode used time
			(void)fscanf(pFile,
				"%d %s %c %d" 	/*pid,command,state,ppid*/
				"%d %d %d %d %lld %lld %lld %lld %lld"
				"%lld %lld"		/*usertime,systemtime */
				/*
				skip others....
				*/,
				&nPid,szProcessName,&dDummy,&nDummy,
				&nDummy,&nDummy,&nDummy,&nDummy,&ullDummy,&ullDummy,&ullDummy,&ullDummy,&ullDummy,
				&ullUserTime,&ullSystime);

			ullCurUsedTime = ullUserTime + ullSystime;
			pInfo->unCpuRate = (HUINT32)(100 * (ullCurUsedTime- pInfo->ullUsedTime) / (ullDiffJiffies));
			pInfo->ullUsedTime = ullCurUsedTime;

			HLIB_FILE_Close(pFile);
		}// end of file open
		pList = HLIB_LIST_Next(pList);
	}
}

static void hama_tool_CutProcessList(void )
{
	HxList_t			*pList,*pNewList = NULL;
	hama_tool_ProcessInfo_t	*pInfo;

	if(HLIB_LIST_Length(s_pAppInfoList) > HAMA_MAX_MONITOR_PROCESS_NUM)
	{
		HINT32 		i = 0;

		pList = s_pAppInfoList;
		while(pList)
		{
			if(++i > HAMA_MAX_MONITOR_PROCESS_NUM)
			{
				break;
			}
			pInfo = (hama_tool_ProcessInfo_t*)HLIB_LIST_Data(pList);
			pNewList = HLIB_LIST_Append(pNewList,HAMA_MemDup(pInfo,sizeof(hama_tool_ProcessInfo_t)));
			pList = HLIB_LIST_Next(pList);
		}

		s_pAppInfoList = HLIB_LIST_RemoveAllFunc(s_pAppInfoList,HAMA_Free_CB);
		s_pAppInfoList = pNewList;
	}
}

static void hama_tool_GetProcessMemInfo(HxList_t *pPathList,HULONG ulTotalRamMB)
{
	HxFILE_t 			pFile;
	HxList_t			*pList,*pFindList;
	hama_tool_ProcessInfo_t	*pInfo;
	HCHAR 				*pPath;

	pList = pPathList;

	while(pList)
	{
		HULONG ulVmRss=0;
		HINT32  nPid=0;
		HCHAR	szline[256],szProcessName[24]={0,};

		pPath = (HCHAR*)HLIB_LIST_Data(pList);
		pFile = HLIB_FILE_Open(pPath,"r");
		if(pFile == NULL)
		{
			HxLOG_Error("Open Fail...[%s]\n",pPath);
			pList = HLIB_LIST_Next(pList);
			continue;
		}

		while (HLIB_FILE_GetString(pFile,szline, 255))
		{
			if (HLIB_STD_StrCaseStartWith(szline, "Name:"))
			{
				sscanf(szline,"Name: %s",szProcessName);
			}
			else if (HLIB_STD_StrCaseStartWith(szline, "Pid:"))
			{
				sscanf(szline,"Pid: %d",&nPid);
			}
			else if (HLIB_STD_StrCaseStartWith(szline, "VmRSS:"))
			{
				sscanf(szline,"VmRSS: %ld",&ulVmRss);
				break;
			}
		}
		HLIB_FILE_Close(pFile);

		pFindList = HLIB_LIST_FindEx(s_pAppInfoList,(void*)nPid,hama_tool_pidcompare);
		if(pFindList)
		{
			pInfo = (hama_tool_ProcessInfo_t*)HLIB_LIST_Data(pFindList);
			s_pAppInfoList = HLIB_LIST_Remove(s_pAppInfoList,(void*)pInfo);
		}
		else
		{
			// find dead process..
			pFindList = HLIB_LIST_FindEx(s_pAppInfoList,(void*)szProcessName,hama_tool_namecompare);
			if(pFindList)
			{
				pInfo = (hama_tool_ProcessInfo_t*)HLIB_LIST_Data(pFindList);
				s_pAppInfoList = HLIB_LIST_Remove(s_pAppInfoList,(void*)pInfo);
				HAMA_Free(pInfo);
			}

			pInfo = (hama_tool_ProcessInfo_t*)HAMA_Calloc(sizeof(hama_tool_ProcessInfo_t));
		}
		pInfo->nPid = nPid;
		HxSTD_StrNCpy(pInfo->szName,szProcessName,sizeof(pInfo->szName)-1);

		pInfo->ulVmRssMB = ulVmRss / 1024;
		pInfo->unMemRate = (HUINT32)(100.0 * ((pInfo->ulVmRssMB / ((HFLOAT32)ulTotalRamMB))));

		s_pAppInfoList = HLIB_LIST_AppendSortEx(s_pAppInfoList,(void*)pInfo,hama_tool_rss_compare);
		pList = HLIB_LIST_Next(pList);
	}

	hama_tool_CutProcessList();

}
static void hama_tool_monitorNotify(HUINT32 unCpuRate,HUINT32 unMemRate,HUINT32 unMemFreeSizeMB)
{
	HSystemMointorInfo_t	*pInfo;
	hama_tool_ProcessInfo_t	*pItem;
	HSystemProcessInfo_t	*pProcess;
	HxList_t 				*pList;
	HAMA_t					*pstHama;
	HINT32 					i;

	if(s_pAppInfoList == NULL)
	{
		HxLOG_Error("s_pAppInfoList is null..");
		return;
	}
	pInfo = (HSystemMointorInfo_t*)HAMA_Calloc(sizeof(HSystemMointorInfo_t));

	pInfo->unMemFreeSizeMB  = unMemFreeSizeMB;
	pInfo->unMemRate		= unMemRate;
	pInfo->unCpuRate		= unCpuRate;

	for(i =0; i < HAMA_MAX_MONITOR_PROCESS_NUM; i++)
	{
		pList = HLIB_LIST_GetListItem(s_pAppInfoList,i);
		if(pList)
		{
			pItem = (hama_tool_ProcessInfo_t*)HLIB_LIST_Data(pList);
			pProcess = &pInfo->stProcess[i];

			HxSTD_StrNCpy(pProcess->szProcessName,pItem->szName,sizeof(pProcess->szProcessName)-1);
			pProcess->unCpuRate = pItem->unCpuRate;
			pProcess->unMemRate = pItem->unMemRate;
			pProcess->unUsedMemSizeMB = (HUINT32)pItem->ulVmRssMB;
		}
	}
	pstHama = HAMA_GetInstance();
	HLIB_RPC_Notify(pstHama->nRPCHandle,"hama:onSendProcessInfo", "b", pInfo,sizeof(HSystemMointorInfo_t));
	HAMA_Free(pInfo);
	HAMA_ReleaseInstance(pstHama);

#ifdef	CONFIG_DEBUG

	HxLOG_Info("Cpu usage : %d%%, Mem usage : %d%% [Free:%ld MB] \n",unCpuRate,unMemRate,unMemFreeSizeMB);
	pList = s_pAppInfoList;
	HxLOG_Info("\tNAME\t\tMEM\tCPU\n");
	while(pList)
	{
		hama_tool_ProcessInfo_t *pProcess;
		pProcess = (hama_tool_ProcessInfo_t*)HLIB_LIST_Data(pList);
		HxLOG_Info("%10s\t%5ld MB[%3d%%]\t%3d%%\n",
			pProcess->szName,pProcess->ulVmRssMB,pProcess->unMemRate,pProcess->unCpuRate);
		pList = HLIB_LIST_Next(pList);
	}

#endif

}
static void	hama_tool_monitorTask (void *arg)
{
	HINT32			i;
	HUINT32			unCpuUsed,unMemUsed;
	HxFILE_t		pFile = NULL;
	HxList_t		*pList;
	HCHAR  			szCpuId[4] = {0};
	struct 	sysinfo stInfo;
	HULONG			ulTotalRamMB,ulFreeRamMB;
	HUINT64			ullSavePass[4] = {0,},ullSavedSum=0;
	HUINT64 		ullPresent[10] = {0,};
	HUINT64 		ullSum= 0,ullDiff[10]={0,};

	while(1)
	{
		if(s_bActive == FALSE)
		{
			HxLOG_Info("s_bActive is FALSE.");
			goto GO_SLEEP;
		}
		// get cpu info
		pFile = HLIB_FILE_Open("/proc/stat","r");
		if(pFile)
		{
			(void)fscanf(pFile, "%s %lld %lld %lld %lld %lld %lld %lld %lld %lld %lld",
				szCpuId,&ullPresent[0],&ullPresent[1],&ullPresent[2],&ullPresent[3],&ullPresent[4],
				&ullPresent[5],&ullPresent[6],&ullPresent[7],&ullPresent[8],&ullPresent[9]);
			HLIB_FILE_Close(pFile);

			// get mem info
			sysinfo(&stInfo);
			ulTotalRamMB = ((stInfo.totalram)/ (1024 *1024));
			ulFreeRamMB = ((stInfo.freeram)/ (1024 *1024));

			//  /proc/[pid]/status files
			pList = hama_tool_GetProcessStatus();
			if(pList)
			{
				// get meminfo
				hama_tool_GetProcessMemInfo(pList,ulTotalRamMB);
				HLIB_LIST_RemoveAllFunc(pList,HAMA_Free_CB);
			}
			else
			{
				HxLOG_Info("process list empty.");
				goto GO_SLEEP;
			}

			for(i = 0,ullSum = 0; i < 10; ++i)
				ullSum += ullPresent[i];

			hama_tool_GetProcessStat(ullSum - ullSavedSum);
			ullSavedSum = ullSum;

			for(i= 0, ullSum = 0;i < 4; ++i) // get sum = user + nice + system + idle
			{
				ullDiff[i] = ullPresent[i] - ullSavePass[i];
				ullSum +=  ullDiff[i];
			}
			HxSTD_MemCopy(ullSavePass,ullPresent,sizeof(ullSavePass));

			unCpuUsed = (HUINT32)(100.0*(1.0- (ullDiff[3] / (HFLOAT64)ullSum)));
			unMemUsed = (HUINT32)(100.0*(((ulTotalRamMB - ulFreeRamMB) / ((HFLOAT32)ulTotalRamMB))));

			hama_tool_monitorNotify(unCpuUsed,unMemUsed,ulFreeRamMB);
		}
GO_SLEEP:
		VK_TASK_Sleep(2000);
	}
}

void 	HAMA_TOOL_ProcessMonitor(HBOOL bActive)
{
	static HULONG s_ulTaskId = 0;

	s_bActive = bActive;
	if(s_ulTaskId == 0 )
	{
		(void)VK_TASK_Create(hama_tool_monitorTask, VK_TASK_PRIORITY_MW_REF, SIZE_256K, "hama_tool_monitor", NULL, &s_ulTaskId, 0);
		(void)VK_TASK_Start(s_ulTaskId);
	}
}

HERROR	HAMA_TOOL_RemoveWindowInfoInProcess(HAMA_t *pstHama, HamaWindowInfo_t *pstWindowInfo)
{
	HxList_t	*list;
	HamaProcessInfo_t	*processInfo;

	list = pstHama->pstAttachedProcessList;
	while (list)
	{
		processInfo = (HamaProcessInfo_t*)HLIB_LIST_Data(list);
		if (HLIB_LIST_Find(processInfo->pstWindowList, pstWindowInfo))
			processInfo->pstWindowList = HLIB_LIST_Remove(processInfo->pstWindowList, pstWindowInfo);
		list = list->next;
	}

	return ERR_OK;
}

HamaProcessInfo_t*	HAMA_TOOL_GetProcessInfoByPid(HAMA_t *pstHama, HINT32 pid)
{
	HxList_t	*list;

	list = HLIB_LIST_FindEx(pstHama->pstAttachedProcessList, (void*)pid, hama_tool_findListByPid);
	if (list)
		return (HamaProcessInfo_t*)HLIB_LIST_Data(list);

	return NULL;
}

HamaProcessInfo_t*	HAMA_TOOL_GetProcessInfoByClient(HxList_t *pstProcessList, HINT32 nClientHandle)
{
	HxList_t	*list;

	list = HLIB_LIST_FindEx(pstProcessList, (void*)nClientHandle, hama_tool_findListBySocketHandle);

	return (HamaProcessInfo_t*)HLIB_LIST_Data(list);
}

HamaWindowInfo_t*	HAMA_TOOL_GetWindowInfoBySHandle(HxList_t *pstWindowList, HINT32 sHandle)
{
	HxList_t	*list;

	list = HLIB_LIST_FindEx(pstWindowList, (void*)sHandle, hama_tool_findListBySHandle);
	if (list)
		return (HamaWindowInfo_t*)HLIB_LIST_Data(list);

	return NULL;
}

HamaWindowInfo_t*	HAMA_TOOL_GetWindowInfoByAppId(HxList_t *pstList, HWINID_e eWindowId)
{
	HxList_t	*list;

	list = HLIB_LIST_FindEx(pstList, (void*)eWindowId, hama_tool_findListByAppId);
	if (list)
		return (HamaWindowInfo_t*)HLIB_LIST_Data(list);

	return NULL;
}

HamaWindowInfo_t*	HAMA_TOOL_GetWindowInfoByPid(HxList_t *pstList, HINT32 nProgId)
{
	HxList_t	*list;

	list = HLIB_LIST_FindEx(pstList, (void*)nProgId, hama_tool_findListByWindowPid);
	if (list)
		return (HamaWindowInfo_t*)HLIB_LIST_Data(list);

	return NULL;

}

HINT32		HAMA_TOOL_CompWindowFocusOrder(void *a, void *b)
{
	HamaWindowInfo_t	*src = (HamaWindowInfo_t*)a;
	HamaWindowInfo_t	*inList = (HamaWindowInfo_t*)b;

	return src->nFocusPriority - inList->nFocusPriority;
}

HERROR		HAMA_TOOL_ConvertDFBKeyToHInputKey(DFBInputEvent *pstDFBKey, HInputEvent_t *pstInputKey)
{
	HAMA_TOOL_CONVDFB2HINPUT(pstDFBKey, pstInputKey);

	return ERR_OK;
}

HERROR		HAMA_TOOL_AddKeyEventToList(HAMA_t *pstHama, HInputEvent_t *pstInputEvent)
{
	static HBOOL		s_bLastPress = FALSE;
	HInputEvent_t		*pstWindowEvent;

	if (pstInputEvent->type == DIET_KEYPRESS)
	{
		if ( HLIB_LIST_Length (pstHama->pstKeyEventList) > HAMA_MAX_KEY_BUFFER_NUM )
		{
			HxLOG_Error("\nHKey Event is Full. Skip Event!\n\n");
			return ERR_FAIL;
		}
		else
			s_bLastPress = TRUE;
	}
	else
	{
		if (!s_bLastPress)
			return ERR_FAIL;
		else
			s_bLastPress = FALSE;
	}

	pstWindowEvent = HAMA_MemDup(pstInputEvent, sizeof(HInputEvent_t));
	pstHama->pstKeyEventList  = HLIB_LIST_Append(pstHama->pstKeyEventList, (void*)pstWindowEvent);

	return ERR_OK;
}

HERROR	HAMA_TOOL_SendInputEvent(HAMA_t *pstHama, HInputEvent_t *pstReturnEvent)
{
	HInputEvent_t			*pstWindowEventInfo = NULL;
	HamaKeyProcessInfo_t	*pstKeyProcInfo = NULL;
	HamaWindowInfo_t		*pstFocusWindowInfo = NULL;

	pstKeyProcInfo = &(pstHama->stKeyProcessInfo);
	if (pstKeyProcInfo == NULL)
		return ERR_FAIL;

	if (pstHama->pstKeyEventList == NULL)
		return ERR_FAIL;
#if 0//defined(CONFIG_PROD_OS_EMULATOR)
	{
		HInputEvent_t	stInputEvent;
		pstWindowEventInfo = (HInputEvent_t*)HLIB_LIST_Data(pstHama->pstKeyEventList);
		//TODO:: 임시
		HxSTD_MemCopy(&stInputEvent, pstWindowEventInfo, sizeof(HInputEvent_t));

		HLIB_RPC_CallbackSpecific(pstHama->nRPCHandle, 0, "hama:onKeyPressed", "ib",
			eAWINDOW_GWMPOPUP, &stInputEvent, sizeof(HInputEvent_t));

		pstHama->pstKeyEventList = HLIB_LIST_Remove(pstHama->pstKeyEventList, (void*)pstWindowEventInfo);
	}

	return ERR_OK;
#else
	if (pstHama->pstWindowListFocusOrder == NULL)
	{
		HxLOG_Warning("No application is registered yet..\n");
		return ERR_FAIL;
	}
#endif
	if (pstReturnEvent == NULL && pstKeyProcInfo->bWaitingReturn)
		return ERR_FAIL;

	pstWindowEventInfo = (HInputEvent_t*)HLIB_LIST_Data(pstHama->pstKeyEventList);

	if (pstReturnEvent)
	{
		//	Return Key event is coming...
		HxLOG_Debug("process return event : request : [%d : %d] flag [%d] - [%d]\n", pstKeyProcInfo->ulWaitingRequestId, pstReturnEvent->request_id, pstKeyProcInfo->bWaitingReturn, pstReturnEvent->eInputReturnType);
		if ((pstKeyProcInfo->ulWaitingRequestId == pstReturnEvent->request_id) && (pstKeyProcInfo->bWaitingReturn))
		{
			pstKeyProcInfo->bWaitingReturn = FALSE;
			if (pstReturnEvent->eInputReturnType == eHINPUTRET_CONSUMED)
			{
				pstKeyProcInfo->pstCurrentWindowInfo = NULL;
				if (pstWindowEventInfo)
				{
					pstHama->pstKeyEventList = HLIB_LIST_Remove(pstHama->pstKeyEventList, (void*)pstWindowEventInfo);
					HAMA_Free(pstWindowEventInfo);
				}
				pstWindowEventInfo = (HInputEvent_t*)HLIB_LIST_Data(pstHama->pstKeyEventList);
			}
		} else
		{
			HxLOG_Warning("Abnormal Return Key Event is coming... ---> Clear All Key Event Buffer!\n");
			HxSTD_MemSet(pstKeyProcInfo, 0, sizeof(HamaKeyProcessInfo_t));
			pstHama->pstKeyEventList = HLIB_LIST_RemoveAllFunc(pstHama->pstKeyEventList, HAMA_Free_CB);
			return ERR_OK;
		}

		if (pstWindowEventInfo == NULL)
		{
			HxLOG_Debug("end of event list\n");
			return ERR_OK;
		}
	}

	if (pstKeyProcInfo->bWaitingReturn == TRUE)
	{
		HxLOG_Warning("Waiting for other key return message.\n");
		return ERR_OK;
	}

	if (pstWindowEventInfo)
	{
		pstFocusWindowInfo = hama_tool_findNextFocusWindowInfo(pstHama, pstKeyProcInfo->pstCurrentWindowInfo, pstWindowEventInfo->key_symbol);
		while ((pstFocusWindowInfo == NULL) && (pstWindowEventInfo))
		{
			HxLOG_Debug("doesn't find windowInfo which is need key symbol [%d]\n", pstWindowEventInfo->key_symbol);
			HxLOG_Debug("Check Next Window Event...\n");

			pstHama->pstKeyEventList = HLIB_LIST_Remove(pstHama->pstKeyEventList, (void*)pstWindowEventInfo);
			HAMA_Free(pstWindowEventInfo);
			pstWindowEventInfo = (HInputEvent_t*)HLIB_LIST_Data(pstHama->pstKeyEventList);
			if (pstWindowEventInfo == NULL)
			{
				HxSTD_MemSet(pstKeyProcInfo, 0, sizeof(HamaKeyProcessInfo_t));
				HxLOG_Debug("No more Window Event...\n");
				return ERR_FAIL;
			}

			pstFocusWindowInfo = hama_tool_findNextFocusWindowInfo(pstHama, NULL, pstWindowEventInfo->key_symbol);
		}

		if (pstFocusWindowInfo && pstWindowEventInfo)
		{
			HInputEvent_t	stInputEvent;

			pstKeyProcInfo->pstCurrentWindowInfo  = pstFocusWindowInfo;
			if (pstFocusWindowInfo->stHookPid)
				pstKeyProcInfo->pstCurrentProcessInfo = HAMA_TOOL_GetProcessInfoByPid(pstHama, pstFocusWindowInfo->stHookPid);
			else
				pstKeyProcInfo->pstCurrentProcessInfo = HAMA_TOOL_GetProcessInfoByPid(pstHama, pstFocusWindowInfo->stProcessPid);

			if (pstKeyProcInfo->pstCurrentProcessInfo == NULL)
			{
				HxLOG_Error("can't find process pid [%d]\n", pstFocusWindowInfo->stProcessPid);
				return ERR_FAIL;
			}

			HxLOG_Debug("find process and window for inputEvent : [%s:0x%x][%d:0x%x]\n",
													pstKeyProcInfo->pstCurrentProcessInfo->szProcessName, pstKeyProcInfo->pstCurrentProcessInfo->stProcessPid,
																					pstKeyProcInfo->pstCurrentWindowInfo->ulApplId, pstKeyProcInfo->pstCurrentWindowInfo->stProcessPid);

			pstKeyProcInfo->bWaitingReturn		= TRUE;
			pstKeyProcInfo->ulLastEventSendTime = HLIB_STD_GetSystemTick();
			pstKeyProcInfo->ulWaitingRequestId	= hama_tool_getRequestId();

			HxSTD_MemCopy(&stInputEvent, pstWindowEventInfo, sizeof(HInputEvent_t));
			stInputEvent.request_id = pstKeyProcInfo->ulWaitingRequestId;
			stInputEvent.window_id  = pstFocusWindowInfo->ulApplId;
			stInputEvent.eInputReturnType = eHINPUTRET_NONE;

			//	Write Log Capture
			if(stInputEvent.type == DIET_KEYPRESS)
			{
				HLIB_CMD_CaptureLog("Key Event,EvtType=0x%X,KeySymbol=0x%X,TargetWindowId=0x%X",
						stInputEvent.type, stInputEvent.key_symbol, stInputEvent.window_id);
			}

			//	HaMa Key Debug
			HLIB_DBG_Trace("HaMaKey", {}, stInputEvent.key_symbol, stInputEvent.type, stInputEvent.request_id, "HaMa", pstKeyProcInfo->pstCurrentProcessInfo->szProcessName);
//			HLIB_DBG_BeginTrace("HaMaKey");
//			HLIB_DBG_Trace("HaMaKey", stInputEvent.key_symbol, stInputEvent.type, stInputEvent.request_id, "HaMa", pstKeyProcInfo->pstCurrentProcessInfo->szProcessName);
//			HLIB_DBG_EndTrace("HaMaKey");

			//	Send Callback to Specific Client
			HLIB_RPC_CallbackSpecific(pstHama->nRPCHandle, pstKeyProcInfo->pstCurrentProcessInfo->nSocketHandle, "hama:onKeyPressed", "ib",
				stInputEvent.window_id, &stInputEvent, sizeof(HInputEvent_t));

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR	HAMA_TOOL_ChangeFocusOrder(HAMA_t *pstHama, HUINT32 targetId, HUINT32 baseId, HRELATION_e eRelation)
{
	HamaWindowInfo_t	*targetInfo, *baseInfo;
	HRELATION_e	relation;

	targetInfo = HAMA_TOOL_GetWindowInfoByAppId(pstHama->pstWindowListFocusOrder, targetId);
	baseInfo   = HAMA_TOOL_GetWindowInfoByAppId(pstHama->pstWindowListFocusOrder, baseId);
	relation = eRelation;

	if (targetInfo && baseInfo)
	{
		pstHama->pstWindowListFocusOrder = HLIB_LIST_Remove(pstHama->pstWindowListFocusOrder, (void*)targetInfo);
		{
			HINT32		position;
			HxList_t	*cur;

			position = 0;
			cur = pstHama->pstWindowListFocusOrder;
			while (cur)
			{
				if (HLIB_LIST_Data(cur) == baseInfo)
					break;
				position++;
				cur = cur->next;
			}

			if (relation == eHRELATION_UP)
				position++;

			pstHama->pstWindowListFocusOrder = HLIB_LIST_Insert(pstHama->pstWindowListFocusOrder, (void*)targetInfo, position);
		}
	} else
		return ERR_FAIL;

	return ERR_OK;
}

HERROR	HAMA_TOOL_ChangeStackOrder(HAMA_t *pstHama, HUINT32 ulTargetId, HUINT32 ulBaseId, HRELATION_e eRelation)
{
	HamaWindowInfo_t	*targetInfo, *baseInfo;
	HRELATION_e	relation;

	targetInfo = HAMA_TOOL_GetWindowInfoByAppId(pstHama->pstWindowListFocusOrder, ulTargetId);
	baseInfo   = HAMA_TOOL_GetWindowInfoByAppId(pstHama->pstWindowListFocusOrder, ulBaseId);
	relation = eRelation;

	if (baseInfo && targetInfo)
	{
		if (targetInfo->nCurrentOpacity > 0)
		{
			targetInfo->bAlreadyRemovePlane = FALSE;
			pstHama->sMgr->Lock(pstHama->sMgr);
			pstHama->sMgr->InsertWindow(pstHama->sMgr, targetInfo->sWindowHandle, baseInfo->sWindowHandle, (relation == eHRELATION_UP) ? SWMWR_TOP : SWMWR_BOTTOM);
			pstHama->sMgr->QueueUpdate(pstHama->sMgr, DWSC_MIDDLE, NULL);
			pstHama->sMgr->ProcessUpdates(pstHama->sMgr, DSFLIP_BLIT);
			pstHama->sMgr->Unlock(pstHama->sMgr);
		} else
		{
			HxLOG_Error("Target is in HIDE mode...\n");
			return ERR_FAIL;
		}
	} else
		return ERR_FAIL;

	return ERR_OK;
}

void*	HAMA_TOOL_GetRPCBinary(HAMA_t *pstHama, HINT32 *pnSize, const HCHAR *pszSignature, ...)
{
	va_list	ap;
	void	*pvData, *pvResult;
	HINT32	nSize;

	va_start(ap, pszSignature);
	pvData = HLIB_RPC_MakeSerialize(pstHama->nRPCHandle, pszSignature, ap, &nSize);
	va_end(ap);

	if (nSize > 0)
	{
		pvResult = (void*)HLIB_CBUF_GetBuffer(pstHama->nCBuf, nSize);
		HxSTD_MemCopy(pvResult, pvData, nSize);

		*pnSize = nSize;
		return pvResult;
	}

	*pnSize = 0;

	return NULL;
}

//	Send signal for hama wakeup
void	HAMA_TOOL_SendWakeup(HAMA_t *pstHama)
{
	HAMA_EVT_t	stEvt;

	HLIB_MSG_Send(pstHama->ulHamaEvtQId, &stEvt, sizeof(HAMA_EVT_t));
}


