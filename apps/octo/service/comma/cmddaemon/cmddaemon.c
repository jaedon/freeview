/**************************************************************
 *	@file		main.c
 *	Humax Command Gateway Daemon for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/09/12
 *	@author			humax
 *	@brief			Humax Command Gateway Daemon for OCTO 2.0
 **************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/resource.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <termio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/stat.h>

#include <hlib.h>
#include <vkernel.h>

#include <cmd_int.h>

/**************************************************************
	defines
 **************************************************************/
#define	LOG_VERSION					"1.00"

#define	CMDMGR_OCCUPIED				"occupied"
#define	MAX_CMDLOG_LEN				256
#define	DEFAULT_LOG_PATH			CONFIG_PRODUCT_USERDATA_DIR "/log"

#define	ENTER_CRITICAL(a)			HxSEMT_Get(a->ulSemId)
#define	LEAVE_CRITICAL(a)			HxSEMT_Release(a->ulSemId)

#define	PROMPT_COLOR_PROC		C_CODE_F_YELLOW
#define	PROMPT_COLOR_GROUP		C_CODE_F_GREEN

// Each file is under 70K...
#define	MAX_LOGCAPTURE_THRESHOLD		512			//	Log Item Count
#define	MAX_LOGCAPTURE_FILENO			64			//	Log File Index
#define	DEFAULT_SYNC_DURATION			7200		//	2 HOURS

/**************************************************************
	MACRO
 **************************************************************/

/**************************************************************
	internal structures and variables
 **************************************************************/
typedef	struct
{
	HCHAR		szGroupKey[HxCMD_MAX_WORD_LEN];
	HCHAR		szGroupName[HxCMD_MAX_WORD_LEN];

	HxHASH_t	*pstWordTable;
} cmdmgr_groupinfo_t;

typedef struct
{
	HCHAR		szFileName[HxCMD_MAX_FILE_LEN];
	HxFILE_t		fLog;
} cmdmgr_logfile_t;

typedef struct
{
	HCHAR		szProcName[HxCMD_MAX_WORD_LEN];
	HINT32		nClientHandle;

	cmdmgr_groupinfo_t	*pstCurrentGroup;
	HxHASH_t		*pstGroupTable;
} cmdmgr_procinfo_t;

typedef	struct
{
	HINT32		sockfd, clisockfd;

	HINT32		nLogNo;
	HCHAR		szCmdLogPath[MAX_CMDLOG_LEN];
	HINT32		nCmdServerHandle;
	HUINT32		ulSemId;
	HINT32		nCommandIndex;
	HCHAR		*pszCommand;
	HCHAR		*pszTempBuffer;
	HCHAR		*pszCmdHistory;
	HBOOL		bHistroy;

	HxList_t		*pstRouteGetCharSocket;
	HxHASH_t	*pstLogFileTable;

	HINT32			nLocalLogCount;
	HUINT32			ulLocalLogStartTime;
	HxList_t		*pstBufferedLogList;

	cmdmgr_procinfo_t	*pstCurrentProc;
	HxHASH_t		*pstProcTable;
} cmd_manager_t;

//	For Log Capture
typedef struct
{
	HUINT32		ulSystemRunning;
	HUINT32		ulLocalTime;
	HCHAR		szLog[HxCMD_MAX_LOG_LEN];
} cmdmgr_loginfo_t;

/**************************************************************
	prototype
**************************************************************/
static int	cmdmgr_serverinetdirect_transmit(int fd, void *wrbuf, int len);
static int	cmdmgr_serverinetdirect_receive(int fd, void *rdbuf, int len);
static void	cmdmgr_serverinetdirect_negotiate(int fd);
static void	cmdmgr_server_logsync(cmd_manager_t *cmdInfo);

/**************************************************************
	private functions
 **************************************************************/
static void	cmdmgr_freeGroupInfo(void *pv)
{
	cmdmgr_groupinfo_t	*pstGroupInfo = (cmdmgr_groupinfo_t*)pv;

	if (pstGroupInfo->pstWordTable)
		HLIB_HASH_Destroy(pstGroupInfo->pstWordTable);

	HLIB_STD_MemFree(pv);
}

static void	cmdmgr_freeProcInfo(void *pv)
{
	cmdmgr_procinfo_t	*pstProcInfo = (cmdmgr_procinfo_t*)pv;

	if (pstProcInfo->pstGroupTable)
		HLIB_HASH_Destroy(pstProcInfo->pstGroupTable);

	HLIB_STD_MemFree(pv);
}

static void cmdmgr_server_removeClient(cmd_manager_t *cmdInfo, HINT32 nClientHandle)
{
	HxHASH_TableIter_t	iter;
	HCHAR				*pszProcName;
	cmdmgr_procinfo_t	*pstProcInfo;

	HLIB_HASH_IterInit(&iter, cmdInfo->pstProcTable);
	while (HLIB_HASH_IterNext(&iter, (void**)&pszProcName, (void**)&pstProcInfo))
	{
		if (pstProcInfo->nClientHandle == nClientHandle)
			break;
	}

	if (pstProcInfo == NULL)
		return;
	if (pstProcInfo->nClientHandle == nClientHandle)
	{
		HxLOG_Debug("--- process name : [%s]\n", pszProcName);
		HLIB_HASH_Remove(cmdInfo->pstProcTable, (void*)pszProcName);
	}
}

static void	cmdmgr_server_registerCmdInfo(cmd_manager_t *cmdInfo, HINT32 nClientHandle, HCHAR *pszProcName, HCHAR *pszGroupName, HCHAR *pszWordName, HCHAR *pszHelpName)
{
	static	HCHAR		s_szProcName[HxCMD_MAX_WORD_LEN], s_szGroupName[HxCMD_MAX_WORD_LEN], s_szWordName[HxCMD_MAX_WORD_LEN];
	cmdmgr_procinfo_t	*pstProcInfo;
	cmdmgr_groupinfo_t	*pstGroupInfo;

	HLIB_STD_StrNCpySafe(s_szProcName,  pszProcName,  HxCMD_MAX_WORD_LEN);
	HLIB_STD_StrNCpySafe(s_szGroupName, pszGroupName, HxCMD_MAX_WORD_LEN);
	HLIB_STD_StrNCpySafe(s_szWordName,  pszWordName,  HxCMD_MAX_WORD_LEN);
	HLIB_STD_StrLower(s_szProcName);
	HLIB_STD_StrLower(s_szGroupName);
	HLIB_STD_StrLower(s_szWordName);

	pstProcInfo = (cmdmgr_procinfo_t*)HLIB_HASH_Lookup(cmdInfo->pstProcTable, (void*)s_szProcName);
	if (pstProcInfo == NULL)
	{
		pstProcInfo = (cmdmgr_procinfo_t*)HLIB_STD_MemCalloc(sizeof(cmdmgr_procinfo_t));
		HxSTD_snprintf(pstProcInfo->szProcName, HxCMD_MAX_WORD_LEN, PROMPT_COLOR_PROC "%s" C_CODE_RESET, pszProcName);
		pstProcInfo->nClientHandle = nClientHandle;
		pstProcInfo->pstGroupTable = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, HLIB_STD_MemFree_CB, cmdmgr_freeGroupInfo);

		HLIB_HASH_Insert(cmdInfo->pstProcTable, (void*)HLIB_STD_StrDup(s_szProcName), (void*)pstProcInfo);
	}

	pstGroupInfo = (cmdmgr_groupinfo_t*)HLIB_HASH_Lookup(pstProcInfo->pstGroupTable, (void*)s_szGroupName);
	if (pstGroupInfo == NULL)
	{
		pstGroupInfo = (cmdmgr_groupinfo_t*)HLIB_STD_MemCalloc(sizeof(cmdmgr_groupinfo_t));
		HLIB_STD_StrNCpySafe(pstGroupInfo->szGroupKey, pszGroupName, HxCMD_MAX_WORD_LEN);
		HxSTD_snprintf(pstGroupInfo->szGroupName, HxCMD_MAX_WORD_LEN, PROMPT_COLOR_GROUP "%s" C_CODE_RESET, pszGroupName);
		pstGroupInfo->pstWordTable = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, HLIB_STD_MemFree_CB, HLIB_STD_MemFree_CB);

		HLIB_HASH_Insert(pstProcInfo->pstGroupTable, (void*)HLIB_STD_StrDup(s_szGroupName), (void*)pstGroupInfo);
	}

	if (HLIB_HASH_Lookup(pstGroupInfo->pstWordTable, (void*)s_szWordName) == NULL)
	{
		HLIB_HASH_Insert(pstGroupInfo->pstWordTable, (void*)HLIB_STD_StrDup(s_szWordName), (void*)HLIB_STD_StrDup(pszHelpName));
		HxLOG_Debug("[%s]word is registered with group(%s), word(%s) and process(%s)\n", HLIB_STD_GetCurProcessName(), pszGroupName, pszWordName, pszProcName);

	} else
	{
		HxLOG_Warning("[%s]already registered group(%s) word(%s) and process(%s)\n", HLIB_STD_GetCurProcessName(), pszGroupName, pszWordName, pszProcName);
	}
}

static void	cmdmgr_server_write(cmd_manager_t *cmdInfo, HINT32 nClient, void *pvData, HINT32 size, HINT32 reqId)
{
	HLIB_SOCKET_Write(nClient, (const void*)pvData, size, reqId);
}

static void	cmdmgr_server_printf(cmd_manager_t *cmdInfo, const HCHAR *pszData, ...)
{
	HCHAR		*p;
	HINT32		len;
	va_list		va;

	len = HxSTD_StrLen(C_CODE_RESET);
	HxSTD_StrNCpy(cmdInfo->pszTempBuffer, C_CODE_RESET, CMDMGR_MAX_INPUT_BUFFER);
	p = cmdInfo->pszTempBuffer + len;
	va_start(va, pszData);
	vsnprintf(p, CMDMGR_MAX_INPUT_BUFFER - len, pszData, va);
	va_end(va);

	cmdmgr_serverinetdirect_transmit(cmdInfo->clisockfd, cmdInfo->pszTempBuffer, HxSTD_StrLen(cmdInfo->pszTempBuffer));
}

static void	cmdmgr_server_runshell(cmd_manager_t *cmdInfo, HCHAR *pszCmd, HCHAR *pszArg)
{
	static HCHAR	*s_pszRunShellCmd = NULL;

	if (s_pszRunShellCmd == NULL)
		s_pszRunShellCmd = (HCHAR*)HLIB_STD_MemAlloc(1024);

	if (pszCmd == NULL)
		return;

	HxSTD_MemSet(s_pszRunShellCmd, 0, 1024);
	if (pszArg)
		HxSTD_snprintf(s_pszRunShellCmd, 1024, "%s %s", pszCmd, pszArg);
	else
		HxSTD_snprintf(s_pszRunShellCmd, 1024, "%s", pszCmd);

	VK_SYSTEM_Command(s_pszRunShellCmd);
}

static void	cmdmgr_server_request(cmd_manager_t *cmdInfo, HCHAR *pszData)
{
	static HxCMD_Request_t		s_stRequestCmd;

	cmdmgr_procinfo_t		*procInfo = NULL;
	cmdmgr_groupinfo_t		*groupInfo = NULL;
	void					*wordInfo = NULL;
	HxCMD_Request_t	*pstRequest = &s_stRequestCmd;

	HCHAR					*pszArg;

	pszArg = HLIB_CMD_GetNextArg((HCHAR**)&pszData);

	if (pszArg)
	{
		HLIB_STD_StrLower(pszArg);
		procInfo = HLIB_HASH_Lookup(cmdInfo->pstProcTable, (void*)pszArg);
	}
	if (procInfo == NULL)
		procInfo = cmdInfo->pstCurrentProc;
	else
		pszArg = HLIB_CMD_GetNextArg((HCHAR**)&pszData);
	if (procInfo == NULL)
	{
//		cmdmgr_server_printf(cmdInfo, "(error) root process is not defined!\r\n");
		cmdmgr_server_runshell(cmdInfo, pszArg, pszData);
		return;
	}
	if (procInfo != cmdInfo->pstCurrentProc)
	{
		if (cmdInfo->pstCurrentProc)
			cmdInfo->pstCurrentProc->pstCurrentGroup = NULL;
		cmdInfo->pstCurrentProc = procInfo;
		cmdmgr_server_printf(cmdInfo, "chproc is ok\n");
	}

	if (pszArg)
	{
		HLIB_STD_StrLower(pszArg);
		groupInfo = HLIB_HASH_Lookup(procInfo->pstGroupTable, (void*)pszArg);
	}
	if (groupInfo == NULL)
		groupInfo = procInfo->pstCurrentGroup;
	else
		pszArg = HLIB_CMD_GetNextArg((HCHAR**)&pszData);
	if (groupInfo == NULL)
	{
//		cmdmgr_server_printf(cmdInfo, "(error) can't find group in process! (%s:%s)\n", procInfo->szProcName, pszArg);
		cmdmgr_server_runshell(cmdInfo, pszArg, pszData);
		return;
	}
	if (groupInfo != procInfo->pstCurrentGroup)
	{
		procInfo->pstCurrentGroup = groupInfo;
		cmdmgr_server_printf(cmdInfo, "chgroup is ok\n");
	}

	if (pszArg)
	{
		HLIB_STD_StrLower(pszArg);
		wordInfo = HLIB_HASH_Lookup(groupInfo->pstWordTable, (void*)pszArg);
	}
	if ((wordInfo == NULL) && (pszArg))
	{
//		cmdmgr_server_printf(cmdInfo, "(error) can't find word in (%s:%s:%s)\n", procInfo->szProcName, groupInfo->szGroupName, pszArg);
		cmdmgr_server_runshell(cmdInfo, pszArg, pszData);
		return;
	}

	if (pszArg)
	{
		HxLOG_Debug("cmdRequest : [%s:%s:%s](%s)\n", procInfo->szProcName, groupInfo->szGroupName, pszArg, pszData);

		HxSTD_MemSet(pstRequest, 0, sizeof(HxCMD_Request_t));
		HLIB_STD_StrNCpySafe(pstRequest->szCmdType, HxCMD_TYPE_REQUEST, HxCMD_MAX_CMDTYPE_LEN);
		HLIB_STD_StrNCpySafe(pstRequest->szGroupName, groupInfo->szGroupKey, HxCMD_MAX_WORD_LEN);
		HLIB_STD_StrNCpySafe(pstRequest->szWordName,  pszArg, HxCMD_MAX_WORD_LEN);
		HLIB_STD_StrNCpySafe(pstRequest->szArgument,  pszData, HxCMD_MAX_ARGU_LEN);

		cmdmgr_server_write(cmdInfo, procInfo->nClientHandle, (void*)pstRequest, sizeof(HxCMD_Request_t), 0);
	}
}

static void	cmdmgr_server_logcapture(cmd_manager_t *cmdInfo, HCHAR *pszProcName, HUINT32 ulSystemRunning, HUINT32 ulLocalTime, HCHAR *pszLog)
{
	cmdmgr_loginfo_t	*pstLogInfo = (cmdmgr_loginfo_t*)HLIB_STD_MemCalloc(sizeof(cmdmgr_loginfo_t));

	if (cmdInfo->ulLocalLogStartTime == 0)
		cmdInfo->ulLocalLogStartTime = ulLocalTime;

	pstLogInfo->ulLocalTime = ulLocalTime;
	pstLogInfo->ulSystemRunning = ulSystemRunning;
	HLIB_STD_StrNCpySafe(pstLogInfo->szLog, pszLog, HxCMD_MAX_LOG_LEN);
	HLIB_STD_StrReplaceChar(pstLogInfo->szLog, pstLogInfo->szLog, '\n', '_');

	cmdInfo->pstBufferedLogList = HLIB_LIST_Append(cmdInfo->pstBufferedLogList, (void*)pstLogInfo);
	cmdInfo->nLocalLogCount++;

	if (cmdInfo->nLocalLogCount > MAX_LOGCAPTURE_THRESHOLD)
		cmdmgr_server_logsync(cmdInfo);
}

static HINT32	cmdmgr_log_findN(cmd_manager_t *cmdInfo, HCHAR *pszFilePath)
{
	HBOOL		bSlots[MAX_LOGCAPTURE_FILENO + 1];
	HINT32		i, no;
	HxDIR_t		*dir = HLIB_DIR_Open(pszFilePath);

	HxSTD_MemSet(bSlots, FALSE, sizeof(HBOOL) * (MAX_LOGCAPTURE_FILENO + 1));
	if (dir == NULL)
		return 0;

	//	datetime.N
	no = 0;
	do
	{
		HCHAR	*pszName = HLIB_DIR_GetName(dir);
		HCHAR	*pszNo;

		pszNo = (HCHAR*)HxSTD_StrRChar(pszName, '.');
		if (pszNo)
		{
			pszNo++;
			no = HLIB_STD_StrToINT32(pszNo);
			bSlots[no % MAX_LOGCAPTURE_FILENO] = TRUE;
		}
	} while (HLIB_DIR_Next(dir) == ERR_OK);
	HLIB_DIR_Close(dir);

	for (i = 0; i < MAX_LOGCAPTURE_FILENO + 1; i++)
	{
		if (bSlots[i] == FALSE)
			return (i % MAX_LOGCAPTURE_FILENO);
	}

	return 0;
}

static void	cmdmgr_server_logsync(cmd_manager_t *cmdInfo)
{
	HUINT32				ulSystemSec;
	HxList_t			*list;
	HxFILE_t			fLog;
	HxDATETIME_t		dateTime;
	cmdmgr_loginfo_t	*logInfo;

	static HCHAR	s_szLogFileName[256];

	if (cmdInfo->nLogNo == -1)
	{
		//	Log Sync First --> check no, compatibility and etc.
		HxSTD_snprintf(s_szLogFileName, 256, "%s/%s", cmdInfo->szCmdLogPath, LOG_VERSION);
		if (HLIB_FILE_Exist(s_szLogFileName) == FALSE)
		{
			HLIB_DIR_Delete(cmdInfo->szCmdLogPath);
			HLIB_DIR_Make(cmdInfo->szCmdLogPath);
			HLIB_FILE_Touch(s_szLogFileName);
		}

		cmdInfo->nLogNo = cmdmgr_log_findN(cmdInfo, cmdInfo->szCmdLogPath);
	}

	HLIB_DATETIME_ConvertUnixTimeToDateTime(cmdInfo->ulLocalLogStartTime, &dateTime);
	HxSTD_snprintf(s_szLogFileName, 256, "%s/syslog.%d", cmdInfo->szCmdLogPath, (cmdInfo->nLogNo + 1) % MAX_LOGCAPTURE_FILENO);
	HLIB_FILE_Delete(s_szLogFileName);
	HxSTD_snprintf(s_szLogFileName, 256, "%s/syslog.%d", cmdInfo->szCmdLogPath, (cmdInfo->nLogNo % MAX_LOGCAPTURE_FILENO));
	fLog = HLIB_FILE_Open(s_szLogFileName, "w");
	if (fLog)
	{
		HLIB_FILE_Printf(fLog, "Log Started at %04d.%02d.%02d - %02d:%02d:%02d.%03d\n", dateTime.stDate.usYear, dateTime.stDate.ucMonth, dateTime.stDate.ucDay,
			dateTime.stTime.ucHour, dateTime.stTime.ucMinute, dateTime.stTime.ucSecond, dateTime.stTime.usMillisecond);

		list = HLIB_LIST_First(cmdInfo->pstBufferedLogList);
		while (list)
		{
			logInfo = (cmdmgr_loginfo_t*)HLIB_LIST_Data(list);
			ulSystemSec = logInfo->ulSystemRunning / 1000;
			HLIB_DATETIME_ConvertUnixTimeToDateTime(logInfo->ulLocalTime, &dateTime);
			HLIB_FILE_Printf(fLog, "[%04dm:%02ds:%03dms] [%02d.%02d][%02d:%02d:%02d][%s]\n",
				ulSystemSec / 60, ulSystemSec % 60, logInfo->ulSystemRunning % 1000,
				dateTime.stDate.ucMonth, dateTime.stDate.ucDay, dateTime.stTime.ucHour, dateTime.stTime.ucMinute, dateTime.stTime.ucSecond,
				logInfo->szLog);
			list = HLIB_LIST_Next(list);
		}

		HLIB_FILE_Close(fLog);
	}

	cmdInfo->pstBufferedLogList = HLIB_LIST_RemoveAllFunc(cmdInfo->pstBufferedLogList, HLIB_STD_MemFree_CB);

	//	reset logcount and logstarttime;
	cmdInfo->nLogNo = (cmdInfo->nLogNo + 1) % MAX_LOGCAPTURE_FILENO;
	cmdInfo->nLocalLogCount = 0;
	cmdInfo->ulLocalLogStartTime = 0;

	HxLOG_Debug("sync is done\n");
}

static void	cmdmgr_server_logBackup(cmd_manager_t *cmdInfo, HCHAR *pszFilePath)
{
	static HCHAR	s_szSourceFile[512], s_szDestFile[512];

	HxDIR_t			*dir;

	HxLOG_Debug("pszFilePath : [%s]\n", pszFilePath);
	cmdmgr_server_logsync(cmdInfo);

	if (HLIB_DIR_IsDirPath(cmdInfo->szCmdLogPath) == FALSE)
		HLIB_DIR_Make(cmdInfo->szCmdLogPath);

	dir = HLIB_DIR_Open(cmdInfo->szCmdLogPath);
	if (dir)
	{
		do
		{
			HCHAR	*pszName = HLIB_DIR_GetName(dir);

			HxLOG_Debug("source file : [%s](%s)\n", cmdInfo->szCmdLogPath, pszName);
			if (HLIB_STD_StrEndWith(pszName, ".log"))
			{
				HxSTD_snprintf(s_szSourceFile, 512, "%s/%s", cmdInfo->szCmdLogPath, pszName);
				HxSTD_snprintf(s_szDestFile,   512, "%s/%s", pszFilePath, pszName);
				s_szSourceFile[511] = 0;
				s_szDestFile  [511] = 0;
				HxLOG_Warning("LOGBACK : [%s --> %s]\n", s_szSourceFile, s_szDestFile);
				HLIB_FILE_Copy(s_szSourceFile, s_szDestFile);
			}
		} while (HLIB_DIR_Next(dir) == ERR_OK);
		HLIB_DIR_Close(dir);
	} else
		HxLOG_Warning("cmdLogPath is not valid [%s]\n", cmdInfo->szCmdLogPath);
}

static void cmdmgr_server_logFile(cmd_manager_t *cmdInfo, HxCMD_LogFile_t	*pstLogFile)
{
	if(cmdInfo && cmdInfo->pstLogFileTable
		&& pstLogFile && pstLogFile->szFileName && pstLogFile->szLogData)
	{
		cmdmgr_logfile_t *pstFileInfo = HLIB_HASH_Lookup(cmdInfo->pstLogFileTable, (void*)pstLogFile->szFileName);

		if(pstFileInfo == NULL)
		{
			if(HxSTD_StrEmpty(pstFileInfo->szFileName) == FALSE)
				HLIB_FILE_Delete(pstFileInfo->szFileName);
			pstFileInfo  = (cmdmgr_logfile_t*)HLIB_STD_MemCalloc(sizeof(cmdmgr_logfile_t));
			if(pstFileInfo != NULL)
			{
				HxSTD_snprintf(pstFileInfo->szFileName, HxCMD_MAX_FILE_LEN-1, "%s", pstLogFile->szFileName);
				pstFileInfo->fLog = HLIB_FILE_Open(pstLogFile->szFileName, "w");
				HLIB_HASH_Insert(cmdInfo->pstLogFileTable, (void*)HLIB_STD_StrDup(pstLogFile->szFileName), (void*)pstFileInfo);
			}
			else
			{
				HxLOG_Debug("pstFileInfo is not alloc !!!\n");
			}
		}

		if(pstFileInfo && pstFileInfo->fLog)
		{
			HUINT32 			ulSystemSec;

			ulSystemSec = pstLogFile->ulSystemTime / 1000;
			HLIB_FILE_Printf(pstFileInfo->fLog, "[%04dm:%02ds:%03dms] [%s]\n",
				ulSystemSec / 60, ulSystemSec % 60, pstLogFile->ulSystemTime % 1000,
				pstLogFile->szLogData);
			HLIB_FILE_Flush(pstFileInfo->fLog);
		}
	}
}

static void	cmdmgr_server_receive_packet(void *pvUserData, HINT32 nSocketHandle, HUINT32 ulRequestId, HINT32 nSize, void *pvData)
{
	cmd_manager_t	*cmdInfo = (cmd_manager_t*)pvUserData;
	HCHAR			*pszData = (HCHAR*)pvData;

	if (nSize == 0 || pvData == NULL)
	{

		HxLOG_Warning("[%s]unexpected data is coming.\n", HLIB_STD_GetCurProcessName());

		return;
	}

	ENTER_CRITICAL(cmdInfo);

	if (HLIB_STD_StrStartWith(pszData, HxCMD_TYPE_PUTCHAR))			//	Client request echo char data.
	{
		cmdInfo->pstRouteGetCharSocket = HLIB_LIST_Append(cmdInfo->pstRouteGetCharSocket, (void*)nSocketHandle);
	} else
	if (HLIB_STD_StrStartWith(pszData, HxCMD_TYPE_PRINTF))
	{
		HCHAR	*pszString = pszData + HxCMD_MAX_CMDTYPE_LEN;

		cmdmgr_server_printf(cmdInfo, pszString);
	} else
	if (HLIB_STD_StrStartWith(pszData, HxCMD_TYPE_LOGCAPTURE))
	{
		HxCMD_LogCapture_t		*pstLogCapture = (HxCMD_LogCapture_t*)pvData;

		cmdmgr_server_logcapture(cmdInfo, pstLogCapture->szProcName, pstLogCapture->ulSystemRunning, pstLogCapture->ulLocalTime, pstLogCapture->szLogCapture);
	} else
	if (HLIB_STD_StrStartWith(pszData, HxCMD_TYPE_REGISTER))
	{
		HxCMD_Register_t		*pstRegister = (HxCMD_Register_t*)pvData;

		cmdmgr_server_registerCmdInfo(cmdInfo, nSocketHandle, pstRegister->szProcName, pstRegister->szGroupName, pstRegister->szWordName, pstRegister->szHelpName);
	} else
	if (HLIB_STD_StrStartWith(pszData, HxCMD_TYPE_LOGSYNC))
	{
		cmdmgr_server_logsync(cmdInfo);
	} else
	if (HLIB_STD_StrStartWith(pszData, HxCMD_TYPE_LOGBACKUP))
	{
		HCHAR	*pszFilePath = pszData + HxCMD_MAX_CMDTYPE_LEN;

		cmdmgr_server_logBackup(cmdInfo, pszFilePath);
	} else
	if (HLIB_STD_StrStartWith(pszData, HxCMD_TYPE_LOGFILE))
	{
		HxCMD_LogFile_t		*pstLogFile = (HxCMD_LogFile_t*)pvData;

		cmdmgr_server_logFile(cmdInfo, pstLogFile);
	}
	else
	{

		HxLOG_Warning("[%s] unexpected request handle.\n", HLIB_STD_GetCurProcessName());
		HLIB_LOG_Dump(pvData, nSize, 0, 0);
	}

	LEAVE_CRITICAL(cmdInfo);
}

static void	cmdmgr_server_receive_status(void *pvUserData, HxSocket_Status_e eStatus, HINT32 nClient)
{
	cmd_manager_t	*cmdInfo = (cmd_manager_t*)pvUserData;

	if (eStatus == HxSOCKET_REMOVE_CLIENT || eStatus == HxSOCKET_DISCONNECT_CLIENT)
	{
		cmdmgr_server_removeClient(cmdInfo, nClient);
	}
}

static void	cmdmgr_server_listen(void *vp)
{
	cmd_manager_t	*cmdInfo = (cmd_manager_t*)vp;

	if (cmdInfo->nCmdServerHandle)
		HLIB_SOCKET_Listen(cmdInfo->nCmdServerHandle, TRUE, cmdmgr_server_receive_packet, cmdmgr_server_receive_status, (void*)cmdInfo);
}

static void	cmdmgr_serverctl_showPrompt(cmd_manager_t *cmdInfo)
{
	HCHAR	szProcName[64];
	HCHAR	szGroupName[64];

	HLIB_STD_StrNCpySafe(szProcName,  "root", 32);
	HLIB_STD_StrNCpySafe(szGroupName, "", 32);

	if (cmdInfo->pstCurrentProc)
	{
		HLIB_STD_StrNCpySafe(szProcName, cmdInfo->pstCurrentProc->szProcName, 32);
		if (cmdInfo->pstCurrentProc->pstCurrentGroup)
			HLIB_STD_StrNCpySafe(szGroupName, cmdInfo->pstCurrentProc->pstCurrentGroup->szGroupName, 32);
	}

	cmdmgr_server_printf(cmdInfo, "%s:%s> " , szProcName, szGroupName);
}

static void	cmdmgr_serverctl_printColumn(cmd_manager_t *cmdInfo, HINT32 nLen, HCHAR *pszData)
{
	HCHAR	szData[HxCMD_MAX_WORD_LEN];
	HCHAR	szExpression[16] = {0, };
	HxSTD_snprintf(szExpression, sizeof(szExpression), "%%-%ds", nLen);

	HxSTD_MemSet(szData, 0x20, HxCMD_MAX_WORD_LEN);
	HxSTD_snprintf(szData, sizeof(szData), szExpression, pszData);

	cmdmgr_server_printf(cmdInfo, szData);
}

static void	cmdmgr_serverctl_printProcList(cmd_manager_t *cmdInfo)
{
	HINT32	i;
	HxHASH_TableIter_t	iter;
	cmdmgr_procinfo_t	*procInfo;

	i = 0;
	cmdmgr_server_printf(cmdInfo, "** registered process list");
	HLIB_HASH_IterInit(&iter, cmdInfo->pstProcTable);

	while (HLIB_HASH_IterNext(&iter, NULL, (void**)&procInfo))
	{
		if ((i & 0x03) == 0)
			cmdmgr_server_printf(cmdInfo, "\n    ");
		cmdmgr_serverctl_printColumn(cmdInfo, 48, procInfo->szProcName);
		i++;
	}
	cmdmgr_server_printf(cmdInfo, "\n");
}

static void	cmdmgr_serverctl_printGroupList(cmd_manager_t *cmdInfo, cmdmgr_procinfo_t *procInfo)
{
	HINT32	i;
	HxHASH_TableIter_t	iter;
	cmdmgr_groupinfo_t	*groupInfo;

	i = 0;
	cmdmgr_server_printf(cmdInfo, "** registered group list in %s", procInfo->szProcName);
	HLIB_HASH_IterInit(&iter, procInfo->pstGroupTable);

	while (HLIB_HASH_IterNext(&iter, NULL, (void**)&groupInfo))
	{
		if ((i & 0x03) == 0)
			cmdmgr_server_printf(cmdInfo, "\n    ");
		cmdmgr_serverctl_printColumn(cmdInfo, 48, groupInfo->szGroupName);
		i++;
	}
	cmdmgr_server_printf(cmdInfo, "\n");
}

static void cmdmgr_serverctl_printWordList(cmd_manager_t *cmdInfo, cmdmgr_procinfo_t *procInfo, cmdmgr_groupinfo_t *groupInfo)
{
	HxHASH_TableIter_t	iter;
	HCHAR				*pszUsage, *pszWord;

	cmdmgr_server_printf(cmdInfo, "** registered word list in %s\n", groupInfo->szGroupName);
	HLIB_HASH_IterInit(&iter, groupInfo->pstWordTable);
	while (HLIB_HASH_IterNext(&iter, (void**)&pszWord, (void**)&pszUsage))
	{
		cmdmgr_server_printf(cmdInfo, "    ");
		cmdmgr_serverctl_printColumn(cmdInfo, 16, pszWord);
		cmdmgr_server_printf(cmdInfo, "  :  %s\n", pszUsage);
	}
	cmdmgr_server_printf(cmdInfo, "\n");
}

static void cmdmgr_serverctl_printHelp(cmd_manager_t *cmdInfo)
{
	HCHAR	szProcName[64];
	HCHAR	szGroupName[64];

	HLIB_STD_StrNCpySafe(szProcName,  "root", 32);
	HLIB_STD_StrNCpySafe(szGroupName, "", 32);

	cmdmgr_server_printf(cmdInfo, "* help [%s:%s]\n", szProcName, szGroupName);
	cmdmgr_serverctl_printProcList(cmdInfo);
	if (cmdInfo->pstCurrentProc)
	{
		cmdmgr_procinfo_t	*procInfo = cmdInfo->pstCurrentProc;

		cmdmgr_serverctl_printGroupList(cmdInfo, procInfo);

		if (procInfo->pstCurrentGroup)
		{
			cmdmgr_groupinfo_t	*groupInfo = procInfo->pstCurrentGroup;

			cmdmgr_serverctl_printWordList(cmdInfo, procInfo, groupInfo);
		}
	}
	cmdmgr_server_printf(cmdInfo, "** helpall : display all command set\n");
}

static void cmdmgr_serverctl_printHelpAll(cmd_manager_t *cmdInfo)
{
	HCHAR	szProcName[64];
	HCHAR	szGroupName[64];

	HxHASH_TableIter_t	iter, iterGroup, iterCmd;
	cmdmgr_procinfo_t	*procInfo;
	cmdmgr_groupinfo_t	*groupInfo;

	HCHAR	*pszUsage, *pszWord;

	HLIB_STD_StrNCpySafe(szProcName, "root", 32);
	HLIB_STD_StrNCpySafe(szGroupName, "", 32);

	cmdmgr_server_printf(cmdInfo, "* help all\n");

	HLIB_HASH_IterInit(&iter, cmdInfo->pstProcTable);
	while (HLIB_HASH_IterNext(&iter, NULL, (void**)&procInfo))
	{
		cmdmgr_server_printf(cmdInfo, "[%s]\n", procInfo->szProcName);

		HLIB_HASH_IterInit(&iterGroup, procInfo->pstGroupTable);
		while (HLIB_HASH_IterNext(&iterGroup, NULL, (void**)&groupInfo))
		{
			cmdmgr_server_printf(cmdInfo, "  [%s]\n", groupInfo->szGroupName);
			HLIB_HASH_IterInit(&iterCmd, groupInfo->pstWordTable);
			while (HLIB_HASH_IterNext(&iterCmd, (void**)&pszWord, (void**)&pszUsage))
			{
				cmdmgr_server_printf(cmdInfo, "     ");
				cmdmgr_serverctl_printColumn(cmdInfo, 16, pszWord);
				cmdmgr_server_printf(cmdInfo, "  :  %s\n", pszUsage);
			}
		}
	}
}

static void	cmdmgr_serverctl_processKey(cmd_manager_t *cmdInfo, HINT32 nSocket, HUINT8 data)
{
	//	if getchar routing table is existed,
	if (cmdInfo->pstRouteGetCharSocket)
	{
		HxList_t	*list;
		HxCMD_PutChar_t		stPutChar;

		HLIB_STD_StrNCpySafe(stPutChar.szCmdType, HxCMD_TYPE_PUTCHAR, HxCMD_MAX_CMDTYPE_LEN);
		stPutChar.cChar = data;

		list = cmdInfo->pstRouteGetCharSocket;
		while (list)
		{
			cmdmgr_server_write(cmdInfo, (HINT32)HLIB_LIST_Data(list), (void*)&stPutChar, sizeof(HxCMD_PutChar_t), 0);
			list = list->next;
		}
		return;
	}

	if (data == 0x20 && cmdInfo->nCommandIndex == 0)
		return;

	if(data == 0x81)
	{
		if(cmdInfo->bHistroy)
		{
			HxSTD_StrNCpy(cmdInfo->pszCommand, cmdInfo->pszCmdHistory, CMDMGR_MAX_INPUT_BUFFER);
			cmdInfo->nCommandIndex = HxSTD_StrLen(cmdInfo->pszCommand);
			cmdmgr_server_printf(cmdInfo, cmdInfo->pszCommand);
			cmdInfo->bHistroy = FALSE;
		}
	}

	if (isprint(data))
		cmdInfo->pszCommand[cmdInfo->nCommandIndex++] = data;
	if ((data == 0x08 || data == 0x7F)&& cmdInfo->nCommandIndex)
		cmdInfo->nCommandIndex--;

	if (data == 0x0a)
	{
		cmdInfo->pszCommand[cmdInfo->nCommandIndex]  = 0;

		if(cmdInfo->nCommandIndex != 0)
		{
			if(HxSTD_StrCmp(cmdInfo->pszCommand, cmdInfo->pszCmdHistory) != 0)
			{
				HxSTD_StrNCpy(cmdInfo->pszCmdHistory, cmdInfo->pszCommand, CMDMGR_MAX_INPUT_BUFFER);
			}
			cmdInfo->bHistroy = TRUE;
		}

		if (HLIB_STD_StrCaseStartWith(cmdInfo->pszCommand, "exit"))
		{
			close(cmdInfo->clisockfd);
			cmdInfo->clisockfd = -1;
			HxLOG_Print("[%s:%d] force close remote access\n", __FUNCTION__, __LINE__);
		} else
		if (HLIB_STD_StrCaseStartWith(cmdInfo->pszCommand, "helpall"))
		{
			cmdmgr_serverctl_printHelpAll(cmdInfo);
		} else
		if (HLIB_STD_StrCaseStartWith(cmdInfo->pszCommand, "help"))
		{
			cmdmgr_serverctl_printHelp(cmdInfo);
		} else
		{
			cmdmgr_server_request(cmdInfo, cmdInfo->pszCommand);
		}

		cmdInfo->nCommandIndex = 0;
	}
}

static int	cmdmgr_serverinetdirect_transmit(int fd, void *wrbuf, int len)
{
	int	ret = 0;
	HCHAR	*p, *crlf = "\r\n";

	p = (HCHAR*)wrbuf;
	while (*p && len)
	{
		if (*p == '\n')
		{
			ret=send(fd, crlf, 2, 0);
		} else
			ret=send(fd, p, 1, 0);

		if(ret < 0)
		{
			HxLOG_Debug("[%s:%d] send error\n",__FUNCTION__, __LINE__);
		}
		p++;
		len--;
	}
//	ret = send(fd, wrbuf, len, 0);
	return ret;
}

static int	cmdmgr_serverinetdirect_receive(int fd, void *rdbuf, int len)
{
	int	ret = 0;
	ret = recv(fd, rdbuf, len, 0);
	return ret;
}

static void	cmdmgr_serverinetdirect_negotiate(int fd)
{
	unsigned char	buf[12];

	buf[0] = 255;
	buf[1] = 251;
	buf[2] = 1;

	buf[3] = 255;
	buf[4] = 251;
	buf[5] = 3;

	buf[6] = 255;
	buf[7] = 253;
	buf[8] = 3;

	cmdmgr_serverinetdirect_transmit(fd, buf, 9);
}

static void	cmdmgr_serverinetdirect_listen(void *vp)
{
	int	ret = 0;
	socklen_t	clilen;
	struct sockaddr_in	cli_addr;
	HCHAR	rdbuffer[8], wrbuffer[8];
	cmd_manager_t	*cmdInfo = (cmd_manager_t*)vp;

	while (1)
	{
		VK_TASK_Sleep(10);
		if (cmdInfo->clisockfd < 0)
		{
			clilen = sizeof(cli_addr);
			cmdInfo->clisockfd = accept(cmdInfo->sockfd, (struct sockaddr *)&cli_addr, &clilen);
			if (cmdInfo->clisockfd >= 0)
			{
				HxLOG_Debug("connected...\n");
				cmdmgr_serverinetdirect_negotiate(cmdInfo->clisockfd);
				cmdmgr_serverctl_showPrompt(cmdInfo);
			}
		}

		if (cmdInfo->clisockfd >= 0)
		{
			bzero(rdbuffer, 8);
			bzero(wrbuffer, 8);

			ret = cmdmgr_serverinetdirect_receive(cmdInfo->clisockfd, rdbuffer, 1);
			if (ret < 0)
				continue;
			else if (ret == 0)
			{
				close(cmdInfo->clisockfd);
				cmdInfo->clisockfd = -1;
			} else
			{
				if (rdbuffer[0] == -1)
				{
					(void)cmdmgr_serverinetdirect_receive(cmdInfo->clisockfd, rdbuffer, 2);
					continue;
				} else
				if(rdbuffer[0] == 0x1b)
				{
					(void)cmdmgr_serverinetdirect_receive(cmdInfo->clisockfd, rdbuffer, 2);
					rdbuffer[0] = 0x40 + rdbuffer[1];
				} else
				if(rdbuffer[0] == 0x08 && cmdInfo->nCommandIndex == 0)
				{
					continue;
				} else
				{
					wrbuffer[0] = rdbuffer[0];
					if (wrbuffer[0] == 0x07 || wrbuffer[0] == 0x7F)
					{
						wrbuffer[0] = 0x08;
						wrbuffer[1] = 0x20;
						wrbuffer[2] = 0x08;
						cmdmgr_serverinetdirect_transmit(cmdInfo->clisockfd, wrbuffer, 3);
					} else
						cmdmgr_serverinetdirect_transmit(cmdInfo->clisockfd, wrbuffer, 1);
				}

				if (rdbuffer[0] == 0x0d)
				{
					(void)cmdmgr_serverinetdirect_receive(cmdInfo->clisockfd, rdbuffer, 1);
					if (rdbuffer[0] == 0x00)
						wrbuffer[0] = rdbuffer[0] = 0x0a;
					else
						wrbuffer[0] = rdbuffer[0];
					cmdmgr_serverinetdirect_transmit(cmdInfo->clisockfd, wrbuffer, 1);
				}
			}

			if (cmdInfo->clisockfd >= 0)
			{
				cmdmgr_serverctl_processKey(cmdInfo, cmdInfo->clisockfd, rdbuffer[0]);
				if ((rdbuffer[0] == 0x0a || rdbuffer[0] == 0x0d) && (cmdInfo->pstRouteGetCharSocket == NULL))
					cmdmgr_serverctl_showPrompt(cmdInfo);
			}
		}
	}
}

static cmd_manager_t*	cmdmgr_getInstance(void)
{
	static cmd_manager_t	*s_pstCmdInfo = NULL;

	if (s_pstCmdInfo == NULL)
	{
		HUINT32	ulThreadId;
		struct timeval	timeout;
		struct sockaddr_in	serv_addr;

		VK_Init();

		HLIB_DBG_Init();

		s_pstCmdInfo = (cmd_manager_t*)HLIB_STD_MemCalloc(sizeof(cmd_manager_t));
		HxSEMT_Create(&s_pstCmdInfo->ulSemId, "cmdmgrsem", 0);
		s_pstCmdInfo->nCommandIndex = 0;
		s_pstCmdInfo->sockfd = -1;
		s_pstCmdInfo->clisockfd = -1;
		s_pstCmdInfo->nLogNo = -1;
		s_pstCmdInfo->pszCommand = (HCHAR*)HLIB_STD_MemCalloc(CMDMGR_MAX_INPUT_BUFFER);
		s_pstCmdInfo->pszTempBuffer = (HCHAR*)HLIB_STD_MemCalloc(CMDMGR_MAX_INPUT_BUFFER);
		s_pstCmdInfo->pstProcTable = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, HLIB_STD_MemFree_CB, cmdmgr_freeProcInfo);
		s_pstCmdInfo->pstLogFileTable = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, HLIB_STD_MemFree_CB, HLIB_STD_MemFree_CB);
		s_pstCmdInfo->pszCmdHistory = (HCHAR*)HLIB_STD_MemCalloc(CMDMGR_MAX_INPUT_BUFFER);
		s_pstCmdInfo->bHistroy = FALSE;

		s_pstCmdInfo->nCmdServerHandle = HLIB_EXTRA_OpenServer("cmddaemon");// HLIB_SOCKET_OpenServer(HxSOCKET_UDS, (void*)HxCMD_SIGNAL_CMD);

		s_pstCmdInfo->sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (s_pstCmdInfo->sockfd < 0)
		{
			HxLOG_Error("socket open failed!\n");
			HLIB_STD_MemFree(s_pstCmdInfo);
			HxLOG_Assert(0);
			return NULL;
		}
		bzero((char*)&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(HLIB_EXTRA_GetPortNo("cmdconsole"));

		if (bind(s_pstCmdInfo->sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
			HxLOG_Error("bind failed!\n");

		listen(s_pstCmdInfo->sockfd, 5);
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;

		if (setsockopt(s_pstCmdInfo->sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0)
		{
			HxLOG_Error("set sockopt failed!\n");
			HLIB_STD_MemFree(s_pstCmdInfo);
			HxLOG_Assert(0);
			return NULL;
		}

		VK_TASK_Create(cmdmgr_serverinetdirect_listen, VK_TASK_PRIORITY_MW_REF, SIZE_64K, "cmdtoolinet", (void*)s_pstCmdInfo, (unsigned long *)&ulThreadId, 0);
		VK_TASK_Start(ulThreadId);

		//	Create Thread for socket listen.
		VK_TASK_Create(cmdmgr_server_listen, VK_TASK_PRIORITY_MW_REF, SIZE_64K, "cmdtool", (void*)s_pstCmdInfo, (unsigned long*)&ulThreadId, 0);
		VK_TASK_Start(ulThreadId);

	}

	return s_pstCmdInfo;
}

static void	cmdmgr_sighandler(int signum, siginfo_t *siginfo, void *context)
{
	cmd_manager_t	*cmdInfo = cmdmgr_getInstance();

	close(cmdInfo->sockfd);
	cmdmgr_server_logsync(cmdInfo);
	exit(0);
}

static void	cmdmgr_setupSignals(void)
{
	struct sigaction	action;

	memset(&action, 0, sizeof(action));

	action.sa_sigaction = cmdmgr_sighandler;
	action.sa_flags     = SA_SIGINFO;
	sigemptyset(&action.sa_mask);

	sigaction(SIGHUP,  &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGINT,  &action, NULL);
	sigaction(SIGKILL, &action, NULL);
	sigaction(SIGQUIT, &action, NULL);
	sigaction(SIGABRT, &action, NULL);

	sigaction(SIGSEGV, &action, NULL);
}

/**************************************************************
	public functions
 **************************************************************/
//static void	test_func(cmd_manager_t *cmdInfo);

#define 	_________________PUBLIC_API____________________________________
static void	cmdmgr_sync_task(void *arg)
{
	HUINT32	ulSyncDuration;
	cmd_manager_t	*cmdmgr = cmdmgr_getInstance();

	ulSyncDuration = 0;
	do
	{
		if ((ulSyncDuration + DEFAULT_SYNC_DURATION) < (HLIB_STD_GetSystemTick() / 1000))
		{
			HxLOG_Warning("---> CMD Sync start!\n");
			ENTER_CRITICAL(cmdmgr);
			cmdmgr_server_logsync(cmdmgr);
			LEAVE_CRITICAL(cmdmgr);
			ulSyncDuration = (HLIB_STD_GetSystemTick() / 1000);
		}
		HLIB_STD_TaskSleep(1000);
	} while (1);
}

int		cmddaemon_main(int argc, char **argv)
{
	unsigned long	ulTaskId;
	cmd_manager_t	*cmdmgr = cmdmgr_getInstance();

	cmdmgr_setupSignals();

	//	Drop Root Previleges --> if CONFIG_DROP_ROOT_PRIVILEGES is ON,
	HLIB_EXTRA_DropRootPrivileges("settop");

	if (argc > 1)
		HLIB_STD_StrNCpySafe(cmdmgr->szCmdLogPath, (HCHAR*)argv[1], MAX_CMDLOG_LEN);
	else
		HLIB_STD_StrNCpySafe(cmdmgr->szCmdLogPath, DEFAULT_LOG_PATH, MAX_CMDLOG_LEN);

	VK_TASK_Create(cmdmgr_sync_task, 40, SIZE_16K, "cmdmgr_logsync", (void*)NULL, (unsigned long *)&ulTaskId, 0);
	VK_TASK_Start(ulTaskId);

	return 0;
}

int		main(int argc, char **argv)
{
	cmddaemon_main(argc, argv);

	while (TRUE)
		HLIB_STD_TaskSleep(1000000);

	return 0;
}

#if	0
static void	test_func(cmd_manager_t *cmdInfo)
{
	HxCMD_Register_t		*pstRegister = (HxCMD_Register_t*)HLIB_STD_MemCalloc(sizeof(HxCMD_Register_t));

	cmdmgr_server_registerCmdInfo(cmdInfo, 0, "test1proc", "test1Group", "test1func", "testusage");
	cmdmgr_server_registerCmdInfo(cmdInfo, 0, "test1proc", "test1Group", "test2func", "testusage");
	cmdmgr_server_registerCmdInfo(cmdInfo, 0, "test1proc", "test1Group", "test3func", "testusage");
	cmdmgr_server_registerCmdInfo(cmdInfo, 0, "test2proc", "test1Group", "test1func", "testusage");
	cmdmgr_server_registerCmdInfo(cmdInfo, 0, "test3proc", "test1Group", "test1func", "testusage");

}
#endif

