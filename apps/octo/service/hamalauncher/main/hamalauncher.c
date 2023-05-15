/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hamalauncher.c
	@brief

	Description:  									\n
	Module: HaMa Launcher Service					\n

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
#include	<sys/types.h>
#include	<sys/wait.h>
#include	<unistd.h>

#include	<vkernel.h>
#include	<hlib.h>
#include	<hapi.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	HAMA_LAUNCHER_FORK				1
#define	HAMA_LAUNCHER_SIGCHLD			2
#define	HAMA_LAUNCHER_PROCESS_READY		3

#if defined(CONFIG_PROD_YS1000) || defined(CONFIG_PROD_YSR2000)
#define DEF_STR_SIZE					(1024*5)
#define DEF_TRIGGER_DIAG_STR				"DIAG_APP"
#define DEF_TRIGGER_OQC_STR				"OQC_APP"

#define NORMAL_WEB_APP					"source/main.js"
#define NUMA_WEB_APP					"source/main.js"   // if numa web is changed, this code have to be changed, tempory code
#define NUMA_WEB_APP_PATH				"/usr/browser/numa"

#define NUMA_FULL_NAME				"/usr/browser/webapps/bin/Luna"
#define OQC_FULL_NAME				"/usr/browser/webapps/bin/oqc"
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if defined(CONFIG_PROD_YS1000) || defined(CONFIG_PROD_YSR2000)
typedef enum
{
	eRunAbleWebApp_NORMAL,
	eRunAbleWebApp_NUMA,
	eRunAbleWebApp_OQC
} RunableWebApp_e;
#endif
typedef enum
{
	eProcess_None,
	eProcess_Ready,
	eProcess_Run,
	eProcess_Stop
} ProcessState_e;

typedef struct
{
	HCHAR	*pszName;
	HCHAR	*pszFullName;
	HCHAR	*pszCondition;
	HCHAR	**ppszArguments;
	HINT32	nArgCount;

	HBOOL	bRebootTerm;
	HBOOL	bManualStart;		// 사용하지 않는다.

	HCHAR	*pszSetDirName;		// Set Current dir from root to given path
	HBOOL	bSetExtArg; 		// Set Arguments from /proc/cmdline

	//	running time
	HINT32			pid;
	ProcessState_e	eState;
} ProcessInfo_t;

typedef struct
{
	HCHAR	*pszSource;
	HCHAR	*pszDest;
} BindMountInfo_t;

typedef struct
{
	//	hconfig.cfg
	HCHAR		*pszChRootPath;
	HxList_t	*pstBindMountList;
	HxList_t	*pstProcessList;
} HAMA_LAUNCHER_Info_t;

typedef struct
{
	HINT32	nMsg;

	HINT32	pid;
	HCHAR	*pszProcessName;
	HCHAR	*pszProcessFullPath;
	HCHAR	**ppszArguments;
	HINT32	nArgCount;
	HCHAR	*pszSetDirName;
	void	*pvUser;
	HINT32	st;
} HAMA_LAUNCHER_MSG_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
static HAMA_LAUNCHER_Info_t		s_stHamaLauncherInfo;
static HUINT32	s_ulHamaLauncherMsgQId = 0;


/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static void	hama_launcher_signal_handler(int signum, siginfo_t *siginfo, void *context)
{
	int		st;
	pid_t	pid;
	HAMA_LAUNCHER_MSG_t		stMsg;
//	HAMA_LAUNCHER_Info_t	*info = &s_stHamaLauncherInfo;

	HxLOG_Debug("sig_chld received!\n");
	while ((pid = waitpid(-1, &st, WNOHANG)) > 0)
	{
		HxSTD_MemSet(&stMsg, 0, sizeof(HAMA_LAUNCHER_MSG_t));
		stMsg.nMsg = HAMA_LAUNCHER_SIGCHLD;
		stMsg.pid = pid;
		stMsg.st = st;
		HLIB_MSG_Send(s_ulHamaLauncherMsgQId, &stMsg, sizeof(HAMA_LAUNCHER_MSG_t));
	}
}

static ProcessInfo_t*	hama_launcher_getProcessInfoByPid(HINT32 pid)
{
	HxList_t		*list;
	ProcessInfo_t	*pInfo;

	list = s_stHamaLauncherInfo.pstProcessList;
	while (list)
	{
		pInfo = (ProcessInfo_t*)HLIB_LIST_Data(list);
		if (pInfo->pid == pid && pInfo->eState == eProcess_Run)
			return pInfo;
		list = HLIB_LIST_Next(list);
	}

	return NULL;
}

#if defined(CONFIG_PROD_YS1000) || defined(CONFIG_PROD_YSR2000)
static RunableWebApp_e	hama_launcher_isNormalRunableWebApp(void)
{
	HxFILE_t pxFile;
	RunableWebApp_e	 eWebApp = eRunAbleWebApp_NORMAL;

	pxFile = HLIB_FILE_Open("/proc/cmdline", "r");
	if (pxFile != NULL)
	{
		HUINT32 ulSize = DEF_STR_SIZE;
		HCHAR 	*szStr = NULL;

		szStr = (HCHAR*)HLIB_STD_MemAlloc(ulSize);
		if (szStr != NULL)
		{
			HUINT8 	*pucRet = NULL;
			pucRet = HLIB_FILE_GetString(pxFile, szStr, (HINT32)ulSize);
			if (pucRet != NULL)
			{
				HxSTR_t	*pxStr;
				pxStr = HLIB_STR_New((const HUINT8*)szStr);
				if (pxStr != NULL)
				{
					if (HLIB_STR_Find(pxStr, (const HUINT8*)DEF_TRIGGER_OQC_STR) != -1)
					{
						eWebApp = eRunAbleWebApp_OQC;
					}
					else if (HLIB_STR_Find(pxStr, (const HUINT8*)DEF_TRIGGER_DIAG_STR) != -1)
					{
						eWebApp = eRunAbleWebApp_NUMA;
					}

					HLIB_STR_Delete(pxStr);
				}
			}
			HLIB_STD_MemFree(szStr);
		}

		(void)HLIB_FILE_Close(pxFile);
	}

	return eWebApp;
}
#endif

static void		hama_launcher_MainLoop(void *arg)
{
	HAMA_LAUNCHER_Info_t	*info = (HAMA_LAUNCHER_Info_t*)arg;
	HAMA_LAUNCHER_MSG_t		stMsg;
	HAMA_LAUNCHER_MSG_t		stSendMsg;
	pid_t	pid;
	int		res = 100;
	HINT32 	i = 0;

	while (1)
	{
		if (HLIB_MSG_Receive(s_ulHamaLauncherMsgQId, &stMsg, sizeof(HAMA_LAUNCHER_MSG_t)) == ERR_OK)
		{
			HxSTD_MemSet(&stSendMsg, 0, sizeof(HAMA_LAUNCHER_MSG_t));
			switch (stMsg.nMsg)
			{
				case HAMA_LAUNCHER_PROCESS_READY:
					{
						HxList_t	*list;

						HxLOG_Debug("HAMA_LAUNCHER_PROCESS_READY : [%s]\n", stMsg.pszProcessName);
						list = info->pstProcessList;
						while (list)
						{
							ProcessInfo_t	*pInfo;

							pInfo = (ProcessInfo_t*)HLIB_LIST_Data(list);
							if (pInfo->eState == eProcess_None)
							{
								if (HxSTD_StrStr(stMsg.pszProcessName, pInfo->pszCondition))
								{
									pInfo->eState = eProcess_Ready;

									HxSTD_MemSet(&stSendMsg, 0, sizeof(HAMA_LAUNCHER_MSG_t));
									stSendMsg.nMsg = HAMA_LAUNCHER_FORK;
									stSendMsg.pszProcessName = HLIB_STD_StrDup(pInfo->pszName);
									stSendMsg.pszProcessFullPath = HLIB_STD_StrDup(pInfo->pszFullName);
									stSendMsg.pszSetDirName = HLIB_STD_StrDup(pInfo->pszSetDirName);
									stSendMsg.pvUser = (void*)pInfo;

									if( pInfo->nArgCount > 0 )
									{
										stSendMsg.ppszArguments = ( HCHAR ** )HLIB_MEM_Calloc(sizeof( HCHAR* ) * pInfo->nArgCount );
										stSendMsg.nArgCount = pInfo->nArgCount;

										for( i = 0 ; i < pInfo->nArgCount ; ++i )
										{
											if( pInfo->ppszArguments[i] )
											{
												stSendMsg.ppszArguments[i] = HLIB_STD_StrDup( pInfo->ppszArguments[i] );
											}
											else
											{
												stSendMsg.ppszArguments[i] = NULL;
											}
										}
									}

									if ( pInfo->bSetExtArg == TRUE )
									{
								#if defined(CONFIG_PROD_YS1000) || defined(CONFIG_PROD_YSR2000)
										RunableWebApp_e eWebApp = hama_launcher_isNormalRunableWebApp();

										stSendMsg.ppszArguments = ( HCHAR ** )HLIB_MEM_Calloc(sizeof( HCHAR* ) * 3 );
										stSendMsg.nArgCount = 3;

										stSendMsg.ppszArguments[0] = HLIB_STD_StrDup(stSendMsg.pszProcessFullPath);
										if(eWebApp == eRunAbleWebApp_NUMA)
										{
											stSendMsg.ppszArguments[1] = HLIB_STD_StrDup((HCHAR*)NUMA_WEB_APP );
											/* if the selected web app is NUMA, the current dir shall change to NUMA_WEB_APP_PATH from default path which is described in HCONFIG.CfG file */
											if (stSendMsg.pszSetDirName != NULL)
											{
												HLIB_STD_MemFree(stSendMsg.pszSetDirName);
											}
											stSendMsg.pszSetDirName = HLIB_STD_StrDup((HCHAR*)NUMA_WEB_APP_PATH);
										}
										else if(eWebApp == eRunAbleWebApp_OQC)
										{
											/* if the selected web app is OQC, Luna does not run*/
											if (HxSTD_StrCmp(stSendMsg.pszProcessFullPath, NUMA_FULL_NAME) == 0)
											{
												stSendMsg.pszProcessFullPath = HLIB_STD_StrDup(OQC_FULL_NAME);
											}
											else
											{
												stSendMsg.ppszArguments[1] = HLIB_STD_StrDup((HCHAR*)NORMAL_WEB_APP );
											}
										}
										else
										{
											stSendMsg.ppszArguments[1] = HLIB_STD_StrDup((HCHAR*)NORMAL_WEB_APP );
										}
										stSendMsg.ppszArguments[2] = NULL;
								#else
										/* someting to implement for the specific requirement */
										;
								#endif
									}


									HLIB_MSG_Send(s_ulHamaLauncherMsgQId, &stSendMsg, sizeof(HAMA_LAUNCHER_MSG_t));
								}
							}

							list = HLIB_LIST_Next(list);
						}
					}
					break;
				case HAMA_LAUNCHER_FORK:
					{
						ProcessInfo_t	*pInfo = (ProcessInfo_t*)stMsg.pvUser;

						if (pInfo && pInfo->eState == eProcess_Ready)
						{
							if (HLIB_STD_GetPID_ByProcessname(stMsg.pszProcessName) == -1)
							{
								if (HLIB_FILE_Exist(stMsg.pszProcessFullPath) == FALSE)
								{
									HxLOG_Error("can't find process [%s] in current hamalauncher\n", stMsg.pszProcessFullPath);
								}
								else
								{
									pid = vfork();
									if (!pid)
									{
										setsid();
										HxLOG_Warning("Spawn application pid[%d] : [%s]\n", HLIB_STD_GetPID(), stMsg.pszProcessName);

										/* change current dir from root to given dir for only this process */
										if ((stMsg.pszSetDirName != NULL) && (HxSTD_StrLen(stMsg.pszSetDirName) > 0))
										{
											HxLOG_Warning("Change current dir from root to %s-%s\n", stMsg.pszProcessName, stMsg.pszSetDirName);
											if (chdir(stMsg.pszSetDirName) < 0)
											{
												HxLOG_Critical("Can't change dir (%s-%s)", stMsg.pszProcessName, stMsg.pszSetDirName);
											}
										}

										res = execvp(stMsg.pszProcessFullPath, stMsg.ppszArguments);
										if (res < 0)
										{
											HxLOG_Error("can't find process [%s] - [%s]\n", stMsg.pszProcessFullPath, strerror(errno));
											exit(SIGABRT);
										}

										exit(0);
									}
									else
									{
										pInfo->eState = eProcess_Run;
										pInfo->pid    = pid;
									}
								}
							}
						}
						else
						{
							if(pInfo)
							{
								HxLOG_Error("process status is not valid : [%s][%d]\n", pInfo->pszName, pInfo->eState);
							}
						}
					}
					break;

				case HAMA_LAUNCHER_SIGCHLD:
					{
						ProcessInfo_t	*pInfo;

						pInfo = hama_launcher_getProcessInfoByPid(stMsg.pid);
						if (pInfo)
						{
							HxLOG_Debug("Process is stopped [%s]\n", pInfo->pszName);

#if defined(CONFIG_CRASH_AND_REEXECUTE)
							if (pInfo->bRebootTerm == FALSE)
							{
								//	restart process.
								pInfo->eState = eProcess_Ready;

								stSendMsg.nMsg = HAMA_LAUNCHER_FORK;
								stSendMsg.pszProcessName = HLIB_STD_StrDup(pInfo->pszName);
								stSendMsg.pszProcessFullPath = HLIB_STD_StrDup(pInfo->pszFullName);
								stSendMsg.pszSetDirName = HLIB_STD_StrDup(pInfo->pszSetDirName);
								stSendMsg.pvUser = (void*)pInfo;

								if( pInfo->nArgCount > 0 )
								{
									stSendMsg.ppszArguments = ( HCHAR ** )HLIB_MEM_Calloc(sizeof( HCHAR* ) * pInfo->nArgCount );
									stSendMsg.nArgCount = pInfo->nArgCount;

									for( i = 0 ; i < pInfo->nArgCount ; ++i )
									{
										if( pInfo->ppszArguments[i] )
										{
											stSendMsg.ppszArguments[i] = HLIB_STD_StrDup( pInfo->ppszArguments[i] );
										}
										else
										{
											stSendMsg.ppszArguments[i] = NULL;
										}
									}
								}

								HLIB_MSG_Send(s_ulHamaLauncherMsgQId, &stSendMsg, sizeof(HAMA_LAUNCHER_MSG_t));
							}
							else
							{
								//	reboot system
								system("reboot");
							}
#endif
						}
					}
					break;

				default:
					HxLOG_Warning("invalid message occurred...[%d]\n", stMsg.nMsg);
					break;
			}

			if (stMsg.pszProcessName)
			{
				HLIB_STD_MemFree(stMsg.pszProcessName);
			}
			if (stMsg.pszProcessFullPath)
			{
				HLIB_STD_MemFree(stMsg.pszProcessFullPath);
			}
			if (stMsg.pszSetDirName)
			{
				HLIB_STD_MemFree(stMsg.pszSetDirName);
			}

			if (stMsg.ppszArguments)
			{
				for(i = 0 ; i < stSendMsg.nArgCount ; ++i )
				{
					if(stMsg.ppszArguments[i])
					{
						HLIB_STD_MemFree(stMsg.ppszArguments[i]);
						stMsg.ppszArguments[i] = NULL;
					}
				}
				HLIB_STD_MemFree(stMsg.ppszArguments);
				stMsg.ppszArguments = NULL;
			}
		}
	}
}

static HERROR	hama_launcher_loadConfig(HAMA_LAUNCHER_Info_t *pstInfo, const HCHAR *pszExeFullName, const HCHAR *pszConfigName)
{
	HCHAR		szExeFullPath[128], *pTemp;
	HCHAR		szConfigFullName[128];
	HINT32		i, nSubLen;
	HxJSON_t	jRoot, jSub, jItem;

	//	get path from argv[0]
	HLIB_PATH_DirName(pszExeFullName, szExeFullPath, 128);
	HxSTD_snprintf(szConfigFullName, 128, "%s/%s", szExeFullPath, pszConfigName);
	szConfigFullName[127] = 0;

	jRoot = HLIB_JSON_DecodeFile(szConfigFullName);
	if (jRoot == NULL)
	{
		HxLOG_Critical("HamaLauncher is stopped : can't find %s\n", szConfigFullName);
		goto err_rtn;
	}

	//	set chroot condition
	pstInfo->pszChRootPath = (pTemp = (HCHAR*)HLIB_JSON_Object_GetString(jRoot, "chroot")) ? HLIB_STD_StrDup(pTemp) : "";

	//	set bind mount condition
	jSub = HxJSON_Object_Get(jRoot, "bindmount");
	if (jSub)
	{
		nSubLen = HLIB_JSON_GetSize(jSub);
		for (i = 0; i < nSubLen; i++)
		{
			BindMountInfo_t		*pstBindMountInfo;

			jItem = HxJSON_Array_Get(jSub, i);

			pstBindMountInfo = (BindMountInfo_t*)HLIB_STD_MemCalloc(sizeof(BindMountInfo_t));
			pstBindMountInfo->pszDest   = (pTemp = (HCHAR*)HLIB_JSON_Object_GetString(jItem, "dst")) ? HLIB_STD_StrDup(pTemp) : "";
			pstBindMountInfo->pszSource = (pTemp = (HCHAR*)HLIB_JSON_Object_GetString(jItem, "src")) ? HLIB_STD_StrDup(pTemp) : "";
			pstInfo->pstBindMountList = HLIB_LIST_Append(pstInfo->pstBindMountList, (void*)pstBindMountInfo);
		}
	}

	//	set process configuration
	jSub = HxJSON_Object_Get(jRoot, "pconfig");
	if (jSub)
	{
		nSubLen = HLIB_JSON_GetSize(jSub);
		for (i = 0; i < nSubLen; i++)
		{
			ProcessInfo_t	*pstProcInfo;

			jItem = HxJSON_Array_Get(jSub, i);

			pstProcInfo = (ProcessInfo_t*)HLIB_STD_MemCalloc(sizeof(ProcessInfo_t));

			pstProcInfo->pszSetDirName = (pTemp = (HCHAR*)HLIB_JSON_Object_GetString(jItem, "setdir")) ? HLIB_STD_StrDup(pTemp) : "";
			pstProcInfo->bSetExtArg = HLIB_JSON_Object_GetInteger(jItem, "extarg");

			pstProcInfo->bRebootTerm = HLIB_JSON_Object_GetInteger(jItem, "term");
			pstProcInfo->bManualStart = HLIB_JSON_Object_GetInteger(jItem, "manual");
			pstProcInfo->pszCondition = (pTemp = (HCHAR*)HLIB_JSON_Object_GetString(jItem, "condition")) ? HLIB_STD_StrDup(pTemp) : "hama";
			pstProcInfo->pszName = (pTemp = (HCHAR*)HLIB_JSON_Object_GetString(jItem, "name")) ? HLIB_STD_StrDup(pTemp) : "";
			pstProcInfo->pszFullName = (pTemp = (HCHAR*)HLIB_JSON_Object_GetString(jItem, "exec")) ? HLIB_STD_StrDup(pTemp) : "";

			// 추가적으로 실행 인자가 있는경우의 파싱 "arguments": [{"arg":"argument1"}, {"arg":"argument2"},  ... ]
			{
				HxJSON_t arr, val;

				arr = HLIB_JSON_Object_GetArray(jItem, "arguments");
				if (arr)
				{
					HINT32 nArr = HLIB_JSON_GetSize(arr);
					HINT32 idx = 0;

					// 첫 인자와 마지막 인자는 규칙상 추가 해야 함. 첫 인자는 실행파일 명, 마지막 인자는 항상 NULL
					pstProcInfo->nArgCount = nArr + 2;
					pstProcInfo->ppszArguments = ( HCHAR ** )HLIB_MEM_Calloc(sizeof( HCHAR* ) * pstProcInfo->nArgCount );

					// 첫 인자는 실행파일 자신
					pstProcInfo->ppszArguments[idx++] = HLIB_STD_StrDup(pTemp);
					// 각 argumet 파싱
					HxJSON_ARRAY_EACH(arr, val, {
						pstProcInfo->ppszArguments[idx++] = HLIB_STD_StrDup(HLIB_JSON_Object_GetString(val, "arg"));
					});
					// 마지막은 항상 NULL로 설정해야 함!!
					pstProcInfo->ppszArguments[idx] = NULL;
				}
			}

			pstInfo->pstProcessList = HLIB_LIST_Append(pstInfo->pstProcessList, (void*)pstProcInfo);
		}
	} else
	{
		HxLOG_Critical("No process config is registered!\n");
		goto err_rtn;
	}

	return ERR_OK;

err_rtn:
	return ERR_FAIL;
}

static void	hama_launcher_printConfig(HAMA_LAUNCHER_Info_t *pstInfo)
{
	HxList_t	*list;

	HxLOG_Debug("HAMA-LAUNCHER ----\n");
	if (HxSTD_StrEmpty(pstInfo->pszChRootPath) == FALSE)
	{
		HxLOG_Debug("\tChRoot Path : %s\n", pstInfo->pszChRootPath);
	}

	if (pstInfo->pstBindMountList)
	{
		HxLOG_Debug("\tBind-Mount List : \n");
		list = pstInfo->pstBindMountList;
		while (list)
		{
			BindMountInfo_t		*info;

			info = (BindMountInfo_t*)HLIB_LIST_Data(list);
			HxLOG_Debug("\t\t[%30s] -- [%30s]\n", info->pszDest, info->pszSource);
			list = list->next;
		}
	}

	if (pstInfo->pstProcessList)
	{
		HxLOG_Debug("\tProcess List : \n");
		list = pstInfo->pstProcessList;
		while (list)
		{
			ProcessInfo_t	*info;

			info = (ProcessInfo_t*)HLIB_LIST_Data(list);
			HxLOG_Debug("\t\t[%s] (%s) term [%d] setchdir [%s] condition [%s]\n", info->pszName, info->pszFullName, info->bRebootTerm, info->pszSetDirName, info->pszCondition);
			list = list->next;
		}
	}
}

//	argV[0] : char* Ready Process Name.
//	argV[1] : int pid
static HERROR	hama_launcher_onReadyProcess(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_LAUNCHER_MSG_t		stMsg;

	HxLOG_Debug("onReadyProcess : [%s]\n", HxOBJECT_STR(apArgv[0]));

	if (nArgc != 2)
	{
		HxLOG_Error("Invalid argument num \n");
#if defined(CONFIG_DEBUG)
		HxLOG_Assert(nArgc == 2);
#endif
		return ERR_OK;
	}

	if (HxSTD_StrEmpty(HxOBJECT_STR(apArgv[0])) == TRUE)
	{
		HxLOG_Error("Process name is null!\n");
		return ERR_OK;
	}

	HxSTD_MemSet(&stMsg, 0, sizeof(HAMA_LAUNCHER_MSG_t));
	stMsg.pszProcessName = HLIB_STD_StrDup(HxOBJECT_STR(apArgv[0]));
	stMsg.nMsg = HAMA_LAUNCHER_PROCESS_READY;
	HLIB_MSG_Send(s_ulHamaLauncherMsgQId, &stMsg, sizeof(HAMA_LAUNCHER_MSG_t));

	return ERR_OK;
}

#include	<sys/mount.h>
static void	hama_launcher_bindMount(HxList_t *list)
{
	BindMountInfo_t		*pstInfo;

	while (list)
	{
		pstInfo = (BindMountInfo_t*)HLIB_LIST_Data(list);

		if (mount(pstInfo->pszSource, pstInfo->pszDest, "none", MS_BIND, NULL) != 0)
		{
			HxLOG_Error("fail to bind mount : [%s --> %s]\n", pstInfo->pszSource, pstInfo->pszDest);
		}
		list = HLIB_LIST_Next(list);
	}
}

static void	hama_launcher_chRoot(HCHAR *pszChRootPath)
{
	HCHAR		szExeFullPath[128];

	HLIB_PATH_DirName(pszChRootPath, szExeFullPath, 128);
	HLIB_EXTRA_Chroot(szExeFullPath);
#ifdef	CONFIG_DEBUG
	HxLOG_Error("####[%s]####\n", pszChRootPath);
	system("ls -al /home");
	HxLOG_Error("##################\n");
#endif
}

int	hamalauncher_main(int argc, char *argv[])
{
#define	HCONFIG_NAME		"/hconfig.cfg"
	unsigned long	tid;
	static struct	sigaction	action;

	VK_Init();

#ifdef	CONFIG_DEBUG
	HLIB_DBG_Init();
#endif

	//
	action.sa_sigaction = hama_launcher_signal_handler;
	action.sa_flags = SA_SIGINFO;
	sigfillset(&(action.sa_mask));
	sigaction(SIGCHLD, &action, NULL);

	if (hama_launcher_loadConfig(&s_stHamaLauncherInfo, argv[0], HCONFIG_NAME) != ERR_OK)
	{
		HxLOG_Critical("----> ERROR!!!!!!!!!\nCritical!!!!\n************************\n************************\n");
		return 0;
	}

	hama_launcher_printConfig(&s_stHamaLauncherInfo);

	if (s_stHamaLauncherInfo.pstBindMountList)
		hama_launcher_bindMount(s_stHamaLauncherInfo.pstBindMountList);

	if (HxSTD_StrEmpty(s_stHamaLauncherInfo.pszChRootPath) == FALSE)
		hama_launcher_chRoot(s_stHamaLauncherInfo.pszChRootPath);

	HLIB_MSG_Create(128, sizeof(HAMA_LAUNCHER_MSG_t), "hamalaucnherQ", (HULONG*)&s_ulHamaLauncherMsgQId, eHLIB_SUSPENDTYPE_FIFO);
	VK_TASK_Create(hama_launcher_MainLoop, 70, SIZE_64K, "hamalauncherM", (unsigned long*)&s_stHamaLauncherInfo, &tid, 0);
	VK_TASK_Start(tid);

	//	HAMA를 비롯한 Process들의 Ready Signal을 받는다.
	HAPI_ConnectSystemSignal("hama:onReadyProcess", "si", NULL, hama_launcher_onReadyProcess);

	return 0;
}

int main(int argc, char *argv[])
{
	hamalauncher_main(argc, argv);

	while (1) VK_TASK_Sleep(3600000);

	return 0;
}
