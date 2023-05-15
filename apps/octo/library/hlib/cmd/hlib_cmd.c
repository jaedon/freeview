/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <hlib.h>
#include <vkernel.h>

#include "_hlib_socket.h"
#include "_hlib_hash.h"

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#define	MAX_CMD_OUTPUT_LENGTH		4096

#define	ENTER_CRITICAL(a)				HxSEMT_Get(a->nSemId)
#define	LEAVE_CRITICAL(a)				HxSEMT_Release(a->nSemId)

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef struct
{
	HCHAR	szGroupName[HxCMD_MAX_WORD_LEN];

	HxHASH_t	*pstCmdTable;
} cmd_groupinfo_t;

typedef	struct
{
	HCHAR	szWord[HxCMD_MAX_WORD_LEN];
	HCHAR	szHelp[HxCMD_MAX_HELP_LEN];
	HCHAR	szUsage[HxCMD_MAX_USAGE_LEN];

	HINT32	(*pfnToDo)(void *arg);

} cmd_wordinfo_t;

#define	MAX_CMD_GETCHAR_BUFFER		40
typedef	struct
{
	HINT32	nClientHandle;
	HINT32	nSemId;
	HBOOL	bInit;

	HINT32	nRecvPos, nCurPos;
	HCHAR	acRecvChar[MAX_CMD_GETCHAR_BUFFER];

	HxHASH_t	*pstGroupTable;
} cmd_info_t;

/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___Private_FUNCTION_HEADER_______________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
static void	hlib_cmd_client_printf(cmd_info_t *cmdInfo, const HCHAR *format, ...)
{
	static HCHAR	s_szCmdPrintBuffer[MAX_CMD_OUTPUT_LENGTH];
	va_list		va;
	HCHAR		*p;

	HxSTD_MemSet(s_szCmdPrintBuffer, 0, MAX_CMD_OUTPUT_LENGTH);

	HLIB_STD_StrNCpySafe(s_szCmdPrintBuffer, HxCMD_TYPE_PRINTF, HxCMD_MAX_CMDTYPE_LEN);
	p = s_szCmdPrintBuffer + HxCMD_MAX_CMDTYPE_LEN;
	va_start(va, format);
	vsnprintf(p, MAX_CMD_OUTPUT_LENGTH - HxCMD_MAX_CMDTYPE_LEN, format, va);
	va_end(va);

	HLIB_SOCKET_Write(cmdInfo->nClientHandle, (const void*)s_szCmdPrintBuffer, HxCMD_MAX_CMDTYPE_LEN + HxSTD_StrLen(p) + 1, 0);
}

static void	hlib_cmd_client_print_usage(cmd_info_t *cmdInfo, cmd_wordinfo_t *pstWordInfo)
{
	hlib_cmd_client_printf(cmdInfo, "usage : %s\n", pstWordInfo->szUsage);
}

static void	hlib_cmd_client_receive_packets(void *pvUserData, HINT32 nSocketHandle, HUINT32 ulRequestId, HINT32 nSize, void *pvData)
{
	HCHAR		szCmdTemp[HxCMD_MAX_WORD_LEN];
	cmd_info_t	*cmdInfo = (cmd_info_t*)pvUserData;
	HCHAR		*pszData = (HCHAR *)pvData;

	if (nSize == 0 || pvData == NULL)
	{
		HxLOG_Warning("[%s:%s](%d) unexpected data is coming.\n", HLIB_STD_GetCurProcessName(), __FUNCTION__, __LINE__);

		return;
	}

	ENTER_CRITICAL(cmdInfo);

	if (HLIB_STD_StrStartWith(pszData, HxCMD_TYPE_PUTCHAR))
	{
		HxCMD_PutChar_t	*pstPutChar = (HxCMD_PutChar_t*)pvData;

		cmdInfo->acRecvChar[cmdInfo->nRecvPos] = pstPutChar->cChar;
		cmdInfo->nRecvPos = (cmdInfo->nRecvPos + 1) % MAX_CMD_GETCHAR_BUFFER;

		if (cmdInfo->nRecvPos == cmdInfo->nCurPos)
			HxLOG_Warning("GetChar Buffer is overwrapped!\n");
	} else
	if (HLIB_STD_StrStartWith(pszData, HxCMD_TYPE_REQUEST))
	{
		HxCMD_Request_t	*pstRequest = (HxCMD_Request_t*)pvData;
		cmd_groupinfo_t			*pstGroupInfo;
		cmd_wordinfo_t			*pstWordInfo;

		HLIB_STD_StrNCpySafe(szCmdTemp, pstRequest->szGroupName, HxCMD_MAX_WORD_LEN);
		HLIB_STD_StrLower(szCmdTemp);
		HxLOG_Debug("szCmdTemp : [%s][%s]\n", pstRequest->szGroupName, szCmdTemp);
		pstGroupInfo = (cmd_groupinfo_t*)HLIB_HASH_Lookup(cmdInfo->pstGroupTable, (void*)szCmdTemp);
		if (pstGroupInfo == NULL)
		{
			HxLOG_Error("[%s:%s](%d) can't find group [%s->%s]\n", HLIB_STD_GetCurProcessName(), __FUNCTION__, __LINE__, pstRequest->szGroupName, pstRequest->szWordName);

			goto err_rtn;
		}
		HLIB_STD_StrNCpySafe(szCmdTemp, pstRequest->szWordName, HxCMD_MAX_WORD_LEN);
		HLIB_STD_StrLower(szCmdTemp);
		pstWordInfo = (cmd_wordinfo_t*)HLIB_HASH_Lookup(pstGroupInfo->pstCmdTable, (void*)szCmdTemp);
		if (pstWordInfo == NULL)
		{
			HxLOG_Error("[%s:%s](%d) can't find word [%s->%s]\n", HLIB_STD_GetCurProcessName(), __FUNCTION__, __LINE__, pstRequest->szGroupName, pstRequest->szWordName);
			goto err_rtn;
		}

		if (pstWordInfo->pfnToDo)
		{
			HxLOG_Debug("call function : [%s][%s][%s]\n", pstWordInfo->szWord, pstWordInfo->szHelp, pstWordInfo->szUsage);
			LEAVE_CRITICAL(cmdInfo);
			if (pstWordInfo->pfnToDo(pstRequest->szArgument) != HxCMD_OK)
				hlib_cmd_client_print_usage(cmdInfo, pstWordInfo);

			return;
		}
		else

			HxLOG_Warning("[%s:%s](%d) can't find registered function [%s->%s]\n", HLIB_STD_GetCurProcessName(), __FUNCTION__, __LINE__, pstRequest->szGroupName, pstRequest->szWordName);
	} else
	{

		HxLOG_Warning("[%s:%s](%d) unexpected request handle.\n", HLIB_STD_GetCurProcessName(), __FUNCTION__, __LINE__);
	}

err_rtn:
	LEAVE_CRITICAL(cmdInfo);
}

static void	hlib_cmd_client_receive_status(void *pvUserData, HxSocket_Status_e eStatus, HINT32 nClient)
{
	(void)pvUserData;
	(void)eStatus;
	(void)nClient;
}

static void	hlib_cmd_client_listen(void *vp)
{
	cmd_info_t	*cmdInfo = (cmd_info_t*)vp;

	if (cmdInfo->nClientHandle)
		HLIB_SOCKET_Listen(cmdInfo->nClientHandle, TRUE, hlib_cmd_client_receive_packets, hlib_cmd_client_receive_status, (void*)cmdInfo);
}

static void	hlib_cmd_client_register_word(cmd_info_t *cmdInfo, const HCHAR *pszCmdGroup, const HCHAR *pszCmdWord, const HCHAR *pszCmdHelp)
{
	static HxCMD_Register_t	s_stCommRegister;

	HxSTD_MemSet(&s_stCommRegister, 0, sizeof(HxCMD_Register_t));

	HLIB_STD_StrNCpySafe(s_stCommRegister.szCmdType,   HxCMD_TYPE_REGISTER, HxCMD_MAX_CMDTYPE_LEN);
	HLIB_STD_StrNCpySafe(s_stCommRegister.szProcName,  HLIB_STD_GetCurProcessName(), HxCMD_MAX_WORD_LEN);
	HLIB_STD_StrNCpySafe(s_stCommRegister.szGroupName, pszCmdGroup, HxCMD_MAX_WORD_LEN);
	HLIB_STD_StrNCpySafe(s_stCommRegister.szWordName,  pszCmdWord,  HxCMD_MAX_WORD_LEN);
	HLIB_STD_StrNCpySafe(s_stCommRegister.szHelpName,  pszCmdHelp,  HxCMD_MAX_HELP_LEN);

	HLIB_SOCKET_Write(cmdInfo->nClientHandle, (const void*)&s_stCommRegister, sizeof(HxCMD_Register_t), 0);
}

static void	hlib_cmd_group_freetable(void *pvData)
{
	cmd_groupinfo_t	*info = (cmd_groupinfo_t*)pvData;

	if (info->pstCmdTable)
		HLIB_HASH_Destroy(info->pstCmdTable);

	HLIB_MEM_Free(pvData);
}

static cmd_info_t*	hlib_cmd_getInstance(void)
{
	static cmd_info_t	*s_pstCmdInit = NULL;

	if (s_pstCmdInit == NULL)
	{
		HUINT32		ulThreadId;

		s_pstCmdInit = (cmd_info_t*)HLIB_MEM_Calloc(sizeof(cmd_info_t));
		HxSEMT_Create(&s_pstCmdInit->nSemId, (const HCHAR*)"cmdtool", 0);
		s_pstCmdInit->nClientHandle = HLIB_EXTRA_OpenClient("cmddaemon");//HLIB_SOCKET_OpenClient(HxSOCKET_UDS, (void*)HxCMD_SIGNAL_CMD, 2);		//	comma가 존재하면 connection.

		if (s_pstCmdInit->nClientHandle)
		{

			HxLOG_Warning("** (%s) is connected to cmddaemon\n", HLIB_STD_GetCurProcessName());

			//	Create Thread for socket listen.
			VK_TASK_Create(hlib_cmd_client_listen, VK_TASK_PRIORITY_MW_REF, SIZE_64K, "cmdtool", (void*)s_pstCmdInit, (unsigned long*)&ulThreadId, 0);
			VK_TASK_Start(ulThreadId);
		}
	}

	return s_pstCmdInit;
}

static HERROR	hlib_cmd_registerWord(cmd_info_t *cmdInfo, const HCHAR *pszCmdGroup, HINT32 (*fnToDo)(void *arg), const HCHAR *pszCmdWord, const HCHAR *pszCmdHelp, const HCHAR *pszCmdUsage)
{
	HCHAR		szCmdTemp[HxCMD_MAX_WORD_LEN];
	cmd_groupinfo_t	*cmdGroupInfo;
	cmd_wordinfo_t	*cmdWordInfo;

	if (cmdInfo->pstGroupTable == NULL)
	{
		//cmdInfo->pstGroupTable = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, HLIB_MEM_Free_CB, (_HxDestroyNotify)hlib_cmd_group_freetable);
		cmdInfo->pstGroupTable = HLIB_HASH_New(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual);
	}

	HxLOG_Debug("pszCmdGroup[%s] pszCmdWord[%s] pszCmdHelp[%s] pszCmdUsage[%s]\n", pszCmdGroup, pszCmdWord, pszCmdHelp, pszCmdUsage);
	HLIB_STD_StrNCpySafe(szCmdTemp, pszCmdGroup, HxCMD_MAX_WORD_LEN);
	HLIB_STD_StrLower(szCmdTemp);
	cmdGroupInfo = (cmd_groupinfo_t*)HLIB_HASH_Lookup(cmdInfo->pstGroupTable, szCmdTemp);
	if (cmdGroupInfo == NULL)
	{
		cmdGroupInfo = (cmd_groupinfo_t*)HLIB_MEM_Calloc(sizeof(cmd_groupinfo_t));

		HLIB_STD_StrNCpySafe(cmdGroupInfo->szGroupName, pszCmdGroup, HxCMD_MAX_WORD_LEN);
		HxLOG_Debug("register group [%s:%s]\n", pszCmdGroup, szCmdTemp);
		HLIB_HASH_Insert(cmdInfo->pstGroupTable, (void*)HLIB_MEM_StrDup_CB(szCmdTemp), (void*)cmdGroupInfo);
	}

	if (cmdGroupInfo->pstCmdTable == NULL)
		cmdGroupInfo->pstCmdTable = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, HLIB_MEM_Free_CB, (_HxDestroyNotify)HLIB_MEM_Free_CB);

	HLIB_STD_StrNCpySafe(szCmdTemp, pszCmdWord, HxCMD_MAX_WORD_LEN);
	HLIB_STD_StrLower(szCmdTemp);
	cmdWordInfo = (cmd_wordinfo_t*)HLIB_HASH_Lookup(cmdGroupInfo->pstCmdTable, (void*)szCmdTemp);
	if (cmdWordInfo == NULL)
	{
		cmdWordInfo = (cmd_wordinfo_t*)HLIB_MEM_Calloc(sizeof(cmd_wordinfo_t));
		HLIB_STD_StrNCpySafe(cmdWordInfo->szWord,  pszCmdWord,  HxCMD_MAX_WORD_LEN);
		HLIB_STD_StrNCpySafe(cmdWordInfo->szHelp,  pszCmdHelp,  HxCMD_MAX_HELP_LEN);
		HLIB_STD_StrNCpySafe(cmdWordInfo->szUsage, pszCmdUsage, HxCMD_MAX_USAGE_LEN);
		cmdWordInfo->pfnToDo = fnToDo;

		HLIB_HASH_Insert(cmdGroupInfo->pstCmdTable, (void*)HLIB_MEM_StrDup_CB(szCmdTemp), (void*)cmdWordInfo);

		hlib_cmd_client_register_word(cmdInfo, pszCmdGroup, pszCmdWord, pszCmdHelp);
	} else
	{
		HxLOG_Warning("already registered command [%s](%s)\n", pszCmdGroup, pszCmdWord);
	}

	return ERR_OK;
}

static HINT32	s_nStartSec = 30;
static HINT32	s_nDurationSec = 5;
static HBOOL	s_bAllThread = TRUE;
static HCHAR	s_szThreadName[64];
static HUINT32	s_ulMinSize = 0;
static HUINT32	s_ulMaxSize = 0x7FFFFFFF;

static HINT32 hlib_cmd_printMem(void *arg)
{
	HCHAR	*pszFilterName = NULL;
	HCHAR	*pszOptName;
	HCHAR	*pszFileName = NULL;
	FILE	*fOut;

	pszOptName = HLIB_CMD_GetNextArg((HCHAR**)&arg);
	if (pszOptName == NULL)
		return HxCMD_ERR;

	if (strstr(pszOptName, "-f"))
		pszFilterName = (HCHAR*)(pszOptName + 2);
	else
		pszFileName = pszOptName;

	if (pszFileName == NULL)
	{
		pszFileName = HLIB_CMD_GetNextArg((HCHAR**)&arg);
		if (pszFileName == NULL)
			return HxCMD_ERR;
	}

	HLIB_CMD_Printf("\n - [%s] mem conf - start (%d) duration (%d)\n", HLIB_STD_GetCurProcessName(), s_nStartSec, s_nDurationSec);
	HLIB_CMD_Printf("\n - [%s] mem conf - thread (%s)\n", HLIB_STD_GetCurProcessName(), s_szThreadName);
	HLIB_CMD_Printf("\n - [%s] mem conf - minSize (%u) maxSize (%u)\n", HLIB_STD_GetCurProcessName(), s_ulMinSize, s_ulMaxSize);

	if (MEMDBG_GetUsedMemory() == 0)
	{
		HLIB_CMD_Printf("No memory configuration detected : [%s]\n", HLIB_STD_GetCurProcessName());
		HLIB_CMD_Printf("  - Please link memory debugger object on your process.\n");
		HLIB_CMD_Printf("  - STEP 1 : Find link option in make (apps/make/appl_make/%s/HumaxLinkLibrary.mak or apps/make/octosvc_make/%s/svc_make/HumaxLinkLibrary.mak)\n", HLIB_STD_GetCurProcessName(), HLIB_STD_GetCurProcessName());
		HLIB_CMD_Printf("  - STEP 2 : comments APP-LINK-y += -lmemory-null, and uncomments APP-LINK-y += -lmemory-debug\n");
		HLIB_CMD_Printf("  - STEP 3 : compile it. - just link\n");
		HLIB_CMD_Printf("\n  - In case of Corsair, please add LIBS += memory-debug in corsair/base-2.0/humax-impl/components/corsair/component.mk\n");

		return HxCMD_OK;
	}

	if (TRUE == s_bAllThread)
	{
		strncpy(s_szThreadName, "all thread", 63);
		s_szThreadName[63] = '\0';
	}

	fOut = fopen(pszFileName, "w");
	if (fOut == NULL)
	{
		HLIB_CMD_Printf("file open failed. try /tmp/%s instead \n", pszFileName);
		return HxCMD_ERR;
	}

	MEMDBG_Summary(-1, s_nStartSec, s_nDurationSec, s_szThreadName, s_ulMinSize, s_ulMaxSize, pszFilterName, fOut);
	fclose(fOut);

	HLIB_CMD_Printf("  - %s is logged\n", pszFileName);

	return HxCMD_OK;

}

static HINT32 hlib_cmd_printBacktraceMem (void *arg)
{
#if defined(CONFIG_DEBUG)
	void	*pvPtrAddr;
	HCHAR	*pszOptName;

	pszOptName = HLIB_CMD_GetNextArg((HCHAR**)&arg);
	if (pszOptName == NULL)
		return HxCMD_ERR;

	pvPtrAddr = (void *)HLIB_STD_StrToINT32(pszOptName);
	if (NULL == pvPtrAddr)
		return HxCMD_ERR;

	MEMDBG_GetBackTrace(-1, pvPtrAddr, HLIB_CMD_Printf);
#endif
	return HxCMD_OK;
}

static HINT32 hlib_cmd_printTaskMemoryUsage (void *arg)
{
	HCHAR	*pszTaskName = NULL;
	HCHAR	*pszOptName;
	HCHAR	*pszFileName = NULL;
	FILE	*fOut;

	pszOptName = HLIB_CMD_GetNextArg((HCHAR**)&arg);
	if (pszOptName == NULL)
		return HxCMD_ERR;

	if (strstr(pszOptName, "-t"))
		pszTaskName = (HCHAR*)(pszOptName + 2);
	else
		pszFileName = pszOptName;

	if (pszFileName == NULL)
	{
		pszFileName = HLIB_CMD_GetNextArg((HCHAR**)&arg);
		if (pszFileName == NULL)
			return HxCMD_ERR;
	}

	if (MEMDBG_GetUsedMemory() == 0)
	{
		HLIB_CMD_Printf("No memory configuration detected : [%s]\n", HLIB_STD_GetCurProcessName());
		HLIB_CMD_Printf("  - Please link memory debugger object on your process.\n");
		HLIB_CMD_Printf("  - STEP 1 : Find link option in make (apps/make/appl_make/%s/HumaxLinkLibrary.mak or apps/make/octosvc_make/%s/svc_make/HumaxLinkLibrary.mak)\n", HLIB_STD_GetCurProcessName(), HLIB_STD_GetCurProcessName());
		HLIB_CMD_Printf("  - STEP 2 : comments APP-LINK-y += -lmemory-null, and uncomments APP-LINK-y += -lmemory-debug\n");
		HLIB_CMD_Printf("  - STEP 3 : compile it. - just link\n");
		HLIB_CMD_Printf("\n  - In case of Corsair, please add LIBS += memory-debug in corsair/base-2.0/humax-impl/components/corsair/component.mk\n");

		return HxCMD_OK;
	}

	fOut = fopen(pszFileName, "w");
	if(fOut != NULL)
	{
		MEMDBG_LogTaskMemUsage(pszTaskName, fOut);
		fclose(fOut);
	}

	HLIB_CMD_Printf("  - %s is logged\n", pszFileName);

	return HxCMD_OK;
}

static HINT32	hlib_cmd_memConf(void *arg)
{
	HCHAR	*pszOptName;
	HCHAR	*pszOptInt;

	pszOptName = HLIB_CMD_GetNextArg((HCHAR**)&arg);
	if (pszOptName == NULL)
		goto err_rtn;

	if (TRUE == s_bAllThread)
	{
		strncpy(s_szThreadName, "all thread", 63);
		s_szThreadName[63] = '\0';
	}

	if (HxSTD_StrCmp(pszOptName, "start") == 0)
	{
		pszOptInt = HLIB_CMD_GetNextArg((HCHAR**)&arg);
		if(pszOptInt == NULL)
			return HxCMD_ERR;
		s_nStartSec = HLIB_STD_StrToINT32(pszOptInt);
	} else
	if (HxSTD_StrCmp(pszOptName, "duration") == 0)
	{
		pszOptInt = HLIB_CMD_GetNextArg((HCHAR**)&arg);
		if(pszOptInt == NULL)
			return HxCMD_ERR;
		s_nDurationSec = HLIB_STD_StrToINT32(pszOptInt);
	} else
	if (HxSTD_StrCmp(pszOptName, "thread") == 0)
	{
		pszOptName = HLIB_CMD_GetNextArg((HCHAR**)&arg);
		if(pszOptName == NULL)
			return HxCMD_ERR;
		if(strcmp(pszOptName, "all") == 0)
		{
			s_bAllThread = TRUE;
			strncpy(s_szThreadName, "all thread", 63);
			s_szThreadName[63] = '\0';
		}
		else if(strcmp(pszOptName, "no_vk") == 0)
		{
			s_bAllThread = FALSE;
			strncpy(s_szThreadName, "no VK thread", 63);
			s_szThreadName[63] = '\0';
		}
		else
		{
			s_bAllThread = FALSE;
			strncpy(s_szThreadName, pszOptName, 63);
			s_szThreadName[63] = '\0';
		}
	} else
	if (HxSTD_StrCmp(pszOptName, "minsize") == 0)
	{
		pszOptInt = HLIB_CMD_GetNextArg((HCHAR**)&arg);
		if(pszOptInt == NULL)
			return HxCMD_ERR;
		s_ulMinSize = HLIB_STD_StrToINT32(pszOptInt);
	} else
	if (HxSTD_StrCmp(pszOptName, "maxsize") == 0)
	{
		pszOptInt = HLIB_CMD_GetNextArg((HCHAR**)&arg);
		if(pszOptInt == NULL)
			return HxCMD_ERR;
		s_ulMaxSize = HLIB_STD_StrToINT32(pszOptInt);
	} else
	goto err_rtn;

	HLIB_CMD_Printf("\n - [%s] mem conf - start (%d) duration (%d)\n", HLIB_STD_GetCurProcessName(), s_nStartSec, s_nDurationSec);
	HLIB_CMD_Printf("\n - [%s] mem conf - thread (%s)\n", HLIB_STD_GetCurProcessName(), s_szThreadName);
	HLIB_CMD_Printf("\n - [%s] mem conf - minSize (%u) maxSize (%u)\n", HLIB_STD_GetCurProcessName(), s_ulMinSize, s_ulMaxSize);

	return HxCMD_OK;

err_rtn:
	HLIB_CMD_Printf("\n - [%s] mem conf - start (%d) duration (%d)\n", HLIB_STD_GetCurProcessName(), s_nStartSec, s_nDurationSec);
	HLIB_CMD_Printf("\n - [%s] mem conf - thread (%s)\n", HLIB_STD_GetCurProcessName(), s_szThreadName);
	HLIB_CMD_Printf("\n - [%s] mem conf - minSize (%u) maxSize (%u)\n", HLIB_STD_GetCurProcessName(), s_ulMinSize, s_ulMaxSize);
	HLIB_CMD_Printf(" - memconf command : \n");
	HLIB_CMD_Printf("   - ex : memconf start 5  - print memory pointer after 5 sec from system boot\n");
	HLIB_CMD_Printf("   - ex : memconf duration 30 - print memory pointer which is allocated over 30 seconds\n");

	return HxCMD_ERR;
}

static HINT32	hlib_cmd_crashMem(void *arg)
{
	FILE	*fOut;

	HLIB_CMD_Printf("\n - start memory check.... - used memory : [%d]\n", MEMDBG_GetUsedMemory());

	fOut = fopen("/corrupted.log", "w");
	if(fOut == NULL)
	{
		return HxCMD_ERR;
	}

	MEMDBG_CheckMem(-1, fOut);
	fclose(fOut);

	HLIB_CMD_Printf(" - end memory check. - logged in /corrupted.log\n");

	return HxCMD_OK;
}

static HINT32	hlib_cmd_currSec(void *arg)
{
	HLIB_CMD_Printf("\n - current seconds from the boot = %d\n", HLIB_STD_GetSystemTick() / 1000);

	return HxCMD_OK;
}

static HINT32 hlib_cmd_hlibMemDbgPrint(void *arg)
{
#if defined(CONFIG_DEBUG)
	HCHAR	*pszOptName, *pszOptNext;
	HUINT32	num = 0;

	pszOptName = HLIB_CMD_GetNextArg((HCHAR**)&arg);
	if (pszOptName == NULL)
	{
		HLIB_CMD_Printf(" - hlibmem command : \n");
		HLIB_CMD_Printf("	- ex : hlibmem hlibfile hash\n");
		HLIB_CMD_Printf("	- ex : hlibmem orgfile tvtvepg\n");
		HLIB_CMD_Printf("	- ex : hlibmem address 7B836A\n");
		HLIB_CMD_Printf("	- ex : hlibmem all\n");
		return HxCMD_ERR;
	}

	pszOptNext = HLIB_CMD_GetNextArg((HCHAR**)&arg);

	if(HxSTD_StrCmp(pszOptName, "address") == 0)
	{
		num = HLIB_STD_StrToINT32(pszOptNext);
	}

	MEMDBG_HLIB_CONF(s_nStartSec, s_nDurationSec, HLIB_CMD_Printf);


	if(MEMDBG_HLIB_Print(pszOptName, pszOptNext, (void*)num) == ERR_FAIL)
	{
		HLIB_CMD_Printf(" - hlibmem command : \n");
		HLIB_CMD_Printf("	- ex : hlibmem hlibfile hash\n");
		HLIB_CMD_Printf("	- ex : hlibmem orgfile tvtvepg\n");
		HLIB_CMD_Printf("	- ex : hlibmem address 7B836A\n");
		HLIB_CMD_Printf("	- ex : hlibmem all\n");
		return HxCMD_ERR;
	}
	else
#endif
	{
		return HxCMD_OK;
	}

}


#define	CHECK_CMD_CONNECTED(a)			do { if (a->nClientHandle == 0) { HxLOG_Warning("client is not connected (%s)\n", HLIB_STD_GetCurProcessName()); LEAVE_CRITICAL(a); return ERR_FAIL;} } while (0)
/* *************************************************************************************************** */
#define ___Public_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
HERROR	HLIB_CMD_RegisterWord(const HCHAR *pszCmdGroup, HINT32 (*fnToDo)(void *arg), const HCHAR *pszCmdWord, const HCHAR *pszCmdHelp, const HCHAR *pszCmdUsage)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	CMD_RegisterGroup(pszCmdGroup, pszCmdGroup, "");
	CMD_RegisterWord(fnToDo, pszCmdWord, pszCmdHelp, pszCmdUsage);
	CMD_ReleaseRegisterGroup();
#else
	cmd_info_t	*cmdInfo = hlib_cmd_getInstance();

	ENTER_CRITICAL(cmdInfo);
	CHECK_CMD_CONNECTED(cmdInfo);

	if (cmdInfo->bInit == FALSE)
	{
		hlib_cmd_registerWord(cmdInfo, "mem", hlib_cmd_printMem, "printmem", "print memory usage to outputfile", "printmem -ffiltername outputfilename");
		hlib_cmd_registerWord(cmdInfo, "mem", hlib_cmd_memConf, "memconf",  "configuration of printmem", "memconf start 5 | duration 10");
		hlib_cmd_registerWord(cmdInfo, "mem", hlib_cmd_crashMem, "memcheck", "check memory corruption - if corrupted, log in corrupted.log", "memcheck");
		hlib_cmd_registerWord(cmdInfo, "mem", hlib_cmd_currSec, "currsec", "print the current second from the boot", "currsec");
		hlib_cmd_registerWord(cmdInfo, "mem", hlib_cmd_printBacktraceMem, "membt", "find the memory allocation backtrace", "membt 0x37ff32");
		hlib_cmd_registerWord(cmdInfo, "mem", hlib_cmd_printTaskMemoryUsage, "taskmem", "print memory usage per task to outputfile", "taskmem -ttaskname outputfilename");

		hlib_cmd_registerWord(cmdInfo, "mem", hlib_cmd_hlibMemDbgPrint, "hlibmem", "print hlib structure's meminfo. only queue, tree, socket, stack, tree, vector, hash.", "hlibmem");

		cmdInfo->bInit = TRUE;
	}

	hlib_cmd_registerWord(cmdInfo, pszCmdGroup, fnToDo, pszCmdWord, pszCmdHelp, pszCmdUsage);

	LEAVE_CRITICAL(cmdInfo);
#endif
	return ERR_OK;
}

HERROR	DBG_HLIB_CMD_Printf(const HCHAR *format, ...)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	static HCHAR	s_szCmdPrintBuffer[MAX_CMD_OUTPUT_LENGTH];

	// TODO:: 임시
	va_list vl;

	va_start(vl, format);
	vsnprintf(s_szCmdPrintBuffer, MAX_CMD_OUTPUT_LENGTH, format, vl);
	va_end(vl);

	DI_UART_Print(s_szCmdPrintBuffer);
#else
	cmd_info_t	*cmdInfo = hlib_cmd_getInstance();
	static HCHAR	s_szCmdPrintBuffer[MAX_CMD_OUTPUT_LENGTH];
	va_list		va;
	HCHAR		*p;

	ENTER_CRITICAL(cmdInfo);
	CHECK_CMD_CONNECTED(cmdInfo);

	HxSTD_MemSet(s_szCmdPrintBuffer, 0, MAX_CMD_OUTPUT_LENGTH);

	HLIB_STD_StrNCpySafe(s_szCmdPrintBuffer, HxCMD_TYPE_PRINTF, HxCMD_MAX_CMDTYPE_LEN);
	p = s_szCmdPrintBuffer + HxCMD_MAX_CMDTYPE_LEN;
	va_start(va, format);
	vsnprintf(p, MAX_CMD_OUTPUT_LENGTH - HxCMD_MAX_CMDTYPE_LEN, format, va);
	va_end(va);

	HLIB_SOCKET_Write(cmdInfo->nClientHandle, (const void*)s_szCmdPrintBuffer, HxCMD_MAX_CMDTYPE_LEN + HxSTD_StrLen(p) + 1, 0);

	LEAVE_CRITICAL(cmdInfo);
#endif
	return ERR_OK;
}

HCHAR*	HLIB_CMD_GetNextArg(HCHAR **pszLine)
{
	HCHAR	*pszReturnArg = NULL;
	HCHAR	*pszFrom;

	pszFrom = *pszLine;
	while (*pszFrom == ' ' || *pszFrom == '\t')
		pszFrom++;

	if (*pszFrom != 0)
	{
		pszReturnArg = pszFrom;
		while (*pszFrom != 0 && *pszFrom != ' ' && *pszFrom != '\t')
			pszFrom++;
		if (*pszFrom != 0)
		{
			*pszFrom = 0;
			pszFrom++;
		}
	}
	*pszLine = pszFrom;

	return pszReturnArg;
}

HERROR	DBG_HLIB_CMD_CaptureLog(const HCHAR *format, ...)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO::임시
#else
	static HxCMD_LogCapture_t	*s_pstCmdLogCapture = NULL;
	cmd_info_t		*cmdInfo = hlib_cmd_getInstance();
	va_list			va;

	ENTER_CRITICAL(cmdInfo);
	CHECK_CMD_CONNECTED(cmdInfo);

	if (s_pstCmdLogCapture == NULL)
		s_pstCmdLogCapture = (HxCMD_LogCapture_t*)HLIB_MEM_Malloc(sizeof(HxCMD_LogCapture_t));

	HxSTD_MemSet(s_pstCmdLogCapture, 0, sizeof(HxCMD_LogCapture_t));

	s_pstCmdLogCapture->ulSystemRunning = HLIB_STD_GetSystemTick();
	(void)VK_CLOCK_GetTime((unsigned long*)&s_pstCmdLogCapture->ulLocalTime);

	HLIB_STD_StrNCpySafe(s_pstCmdLogCapture->szCmdType,    HxCMD_TYPE_LOGCAPTURE, HxCMD_MAX_CMDTYPE_LEN);
	HLIB_STD_StrNCpySafe(s_pstCmdLogCapture->szProcName,   HLIB_STD_GetCurProcessName(), HxCMD_MAX_WORD_LEN);

	va_start(va, format);
	vsnprintf(s_pstCmdLogCapture->szLogCapture, HxCMD_MAX_LOG_LEN - 1, format, va);
	va_end(va);

	HLIB_SOCKET_Write(cmdInfo->nClientHandle, (const void*)s_pstCmdLogCapture, sizeof(HxCMD_LogCapture_t), 0);

	LEAVE_CRITICAL(cmdInfo);
#endif
	return ERR_OK;
}

HERROR	HLIB_CMD_SyncLog(void)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO::임시
#else
	static HCHAR	s_szLogSyncPath[HxCMD_MAX_CMDTYPE_LEN * 2];
	cmd_info_t		*cmdInfo = hlib_cmd_getInstance();

	ENTER_CRITICAL(cmdInfo);
	CHECK_CMD_CONNECTED(cmdInfo);

	HxSTD_MemSet(s_szLogSyncPath, 0, HxCMD_MAX_CMDTYPE_LEN * 2);

	HxSTD_snprintf(s_szLogSyncPath, HxCMD_MAX_CMDTYPE_LEN * 2 - 1, "%s;", HxCMD_TYPE_LOGSYNC);

	HLIB_SOCKET_Write(cmdInfo->nClientHandle, (const void*)s_szLogSyncPath, HxSTD_StrLen(s_szLogSyncPath) + 1, 0);

	LEAVE_CRITICAL(cmdInfo);
#endif
	return ERR_OK;
}

HERROR	HLIB_CMD_BackupLog(const HCHAR *pszPath)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO::임시
#else
	static HCHAR	s_szLogBackupPath[MAX_CMD_OUTPUT_LENGTH];
	cmd_info_t		*cmdInfo = hlib_cmd_getInstance();

	ENTER_CRITICAL(cmdInfo);
	CHECK_CMD_CONNECTED(cmdInfo);

	HxSTD_MemSet(s_szLogBackupPath, 0, MAX_CMD_OUTPUT_LENGTH);

	HxSTD_snprintf(s_szLogBackupPath, MAX_CMD_OUTPUT_LENGTH - 1, "%s;%s", HxCMD_TYPE_LOGBACKUP, pszPath);

	HLIB_SOCKET_Write(cmdInfo->nClientHandle, (const void*)s_szLogBackupPath, HxSTD_StrLen(s_szLogBackupPath) + 1, 0);

	LEAVE_CRITICAL(cmdInfo);
#endif
	return ERR_OK;
}

HERROR	HLIB_CMD_FileLog(const HCHAR * file, const HCHAR *format, ...)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	//TODO::임시
#else
	static HxCMD_LogFile_t		*s_pstCmdLogFile = NULL;
	cmd_info_t		*cmdInfo = hlib_cmd_getInstance();
	va_list			va;

	ENTER_CRITICAL(cmdInfo);
	CHECK_CMD_CONNECTED(cmdInfo);

	s_pstCmdLogFile = (HxCMD_LogFile_t*)HLIB_MEM_Calloc(sizeof(HxCMD_LogFile_t));


	s_pstCmdLogFile->ulSystemTime = HLIB_STD_GetSystemTick();

	HLIB_STD_StrNCpySafe(s_pstCmdLogFile->szCmdType,    HxCMD_TYPE_LOGFILE, HxCMD_MAX_CMDTYPE_LEN);
	HLIB_STD_StrNCpySafe(s_pstCmdLogFile->szFileName,   file, HxCMD_MAX_FILE_LEN);

	va_start(va, format);
	vsnprintf(s_pstCmdLogFile->szLogData, HxCMD_MAX_LOG_LEN - 1, format, va);
	va_end(va);

	HLIB_SOCKET_Write(cmdInfo->nClientHandle, (const void*)s_pstCmdLogFile, sizeof(HxCMD_LogCapture_t), 0);

	LEAVE_CRITICAL(cmdInfo);
#endif
	return ERR_OK;
}


HERROR	HLIB_CMD_InitGetChar(void)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
		//TODO::임시
#else
	cmd_info_t		*cmdInfo = hlib_cmd_getInstance();

	ENTER_CRITICAL(cmdInfo);

	CHECK_CMD_CONNECTED(cmdInfo);
	HLIB_SOCKET_Write(cmdInfo->nClientHandle, (const void*)HxCMD_TYPE_PUTCHAR, HxSTD_StrLen(HxCMD_TYPE_PUTCHAR) + 1, 0);

	LEAVE_CRITICAL(cmdInfo);
#endif
	return ERR_OK;
}

HERROR	HLIB_CMD_RecvGetChar(HCHAR *pChar)
{
#if defined(CONFIG_PROD_OS_EMULATOR)
	*pChar = getchar();
#else
	cmd_info_t		*cmdInfo = hlib_cmd_getInstance();

	ENTER_CRITICAL(cmdInfo);

	if (cmdInfo->nRecvPos != cmdInfo->nCurPos)
	{
		if (pChar)
			*pChar = cmdInfo->acRecvChar[cmdInfo->nCurPos];

		cmdInfo->nCurPos = (cmdInfo->nCurPos + 1) % MAX_CMD_GETCHAR_BUFFER;
		LEAVE_CRITICAL(cmdInfo);

		return ERR_OK;
	}

	LEAVE_CRITICAL(cmdInfo);
#endif
	return ERR_FAIL;
}

