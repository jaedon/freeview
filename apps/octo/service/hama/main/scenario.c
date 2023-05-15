/**************************************************************
 *	@file		scenario.c
 *	Application Scenario Controller for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/01
 *	@author			humax
 *	@brief			Application Scenarion Controller for OCTO 2.0 Project
 **************************************************************/
#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/types.h>
#include	<sys/wait.h>
#include	<unistd.h>

#include	<htype.h>
#include	<vkernel.h>
#include	<hlib.h>

#include	"hama_int.h"

//#define	DEBUG_RUN_MODULE
//#define	DEBUG_RESURRECTION
/**************************************************************
	internal structures and variables
 **************************************************************/
typedef	struct
{
	HWINID_e		eApplId;
	HUINT32			focusOrder;
	HUINT32			stackOrder;
	HUINT32			panelOrder;
} SCENARIO_t;

static	SCENARIO_t	_astScenarioList[] =
{
	{	eAWINDOW_HAMA,					0x2000,		0x0000,		0x0000},
	{	eAWINDOW_DAMA,					0x2000,		0x0000,		0x0000},
	{	eAWINDOW_TR069,					0x2000,		0x0000,		0x0000},

	{	eAWINDOW_PAMA,					0x1000,		0x0000,		0x0000},
	{	eAWINDOW_NETFLIXMANAGER,		0x0991,		0x0000,		0x0000},

	{	eAWINDOW_TESTAPP,				0x0990,		0x0000,		0x0000},

	{	eAWINDOW_GWMPOPUP,				0x0900, 	0x0100, 	0x0200},
	{	eAWINDOW_KEYBOARD,				0x0901,		0x0101,		0x0300},
	{	eAWINDOW_SUBTITLEPLAYER,		0x0500,		0x0500,		0x0000},
	{	eAWINDOW_MHEGPLAYER,			0x0800,		0x0400,		0x0000},
	{	eAWINDOW_MHEGPLAYER_IMG_STILL,	0x0100,		0x0401,		0x0000},
	//{	eAWINDOW_MHEGIB,				0x0800,		0x0400,		0x0000},
	{	eAWINDOW_MHEGIB,				0x0900,		0x0400,		0x0000},
	{	eAWINDOW_NETFLIX,				0x0801,		0x0401,		0x0301},

	{	eAWINDOW_OIPF,					0x0600,		0x0301,		0x0100},
	{	eAWINDOW_HBBTV,					0x0601,		0x0302,		0x0100},
	{	eAWINDOW_BSAUTOMSG, 			0x0602, 	0x0320,		0x0100}, // japan (OSD plane) - 5 plane top
	{	eAWINDOW_OTVB,					0x0603,		0x0310,		0x0100},
	{	eAWINDOW_OTVS,					0x0604,		0x0310,		0x0100},
	{	eAWINDOW_WIND3,					0x0610,		0x0300,		0x0100},
	{	eAWINDOW_JAPAN_NATIVE_UI,		0x0804,		0x0501,		0x0000}, // UI
	{	eAWINDOW_ARIBCC_SUP,			0x0803,		0x0400,		0x0000}, // japan
	{	eAWINDOW_ARIBCC_CAP,			0x0802,		0x0401,		0x0000}, // japan
	{	eAWINDOW_BML,					0x0801,		0x0500,		0x0000}, // japan
	{	eAWINDOW_BMLSTILL,				0x0800,		0x0501,		0x0000}, // japan - 5 plane bottom
	{	eAWINDOW_RFVOD,					0x0850/* BML과 keyboard 사이 */,		0x0350,		0x0000}, // japan
	{	eAWINDOW_ANDROID,				0x0902/* 최 상위 */,		0x0600,		0x0000}, // japan
};

#define	PRIORITY_NORMAL			0xFFFF

#define	PRIORITY_FOCUS_NORMAL	0x09FF		//	Key Focus TOP
#define	PRIORITY_STACK_NORMAL	0x0010		//	Drawing Focus TOP
#define	PRIORITY_PANEL_NORMAL	0x0000		//	PANEL focus bottom

#define	MAX_SCENARIO_NUM		(sizeof(_astScenarioList) / sizeof(SCENARIO_t))

static	HUINT32			s_ulScenarioSemId = 0;

/**************************************************************
	private functions
 **************************************************************/
static	SCENARIO_t*	scenario_findInfo(HWINID_e applId)
{
	HINT32		i;

	for (i = 0; i < MAX_SCENARIO_NUM; i++)
	{
		if (_astScenarioList[i].eApplId == applId)
			return &_astScenarioList[i];
	}

	return NULL;
}

static	SCENARIO_t* scenario_findLowerFocus(HINT32 focus)
{
	HINT32		i, last, index;

	index = -1;
	last = 0;
	for (i = 0; i < MAX_SCENARIO_NUM; i++)
	{
		if (_astScenarioList[i].focusOrder < focus)
		{
			if (last < _astScenarioList[i].focusOrder)
			{
				last = _astScenarioList[i].focusOrder;
				index = i;
			}
		}
	}

	if (index != -1)
		return &_astScenarioList[index];

	return NULL;
}

static	SCENARIO_t* scenario_findLowerStack(HINT32 stack)
{
	HINT32		i, last, index;

	index = -1;
	last = 0;
	for (i = 0; i < MAX_SCENARIO_NUM; i++)
	{
		if (_astScenarioList[i].stackOrder < stack)
		{
			if (last < _astScenarioList[i].stackOrder)
			{
				last = _astScenarioList[i].stackOrder;
				index = i;
			}
		}
	}

	if (index != -1)
		return &_astScenarioList[index];

	return NULL;
}

/**************************************************************
	public functions
 **************************************************************/
/************
************/
#define	___PROCESS_SCENARIO______
typedef	enum
{
	eHSPROC_STATE_UNKNOWN,
	eHSPROC_STATE_STOPPED,
	eHSPROC_STATE_STOPPEDSIGNAL,
	eHSPROC_STATE_RUNNING,
	eHSPROC_STATE_READYTORUN,

	eHSPROC_STATE_MAX
} HamaSProcState_e;

typedef enum
{
	eHSPROC_TYPE_AUTO,
	eHSPROC_TYPE_MANUAL,
} HamaSProcType_e;

typedef	struct
{
	pid_t				stPid;
	HamaSProcState_e	eState;
	HamaSProcType_e		eType;

	HCHAR	*pszProcessName;
	HBOOL	bMonitor;
	HCHAR	*pszConditionName;
	HBOOL	bNeedReboot;
	HCHAR	*pszModuleName;
} HamaSProcess_t;

typedef struct
{
	HINT32	nProgId;
	HCHAR	szProcessName[256];
} ReadyApps_t;

static HxList_t		*s_pstProcessScenarioList = NULL;
static HxList_t		*s_listReadyApps = NULL;

static void	scenario_process_spawn(HAMA_t *pstHama, HamaSProcess_t *pstProcInfo)
{
#if	1
	HxLOG_Debug("process spawn : [%s]\n", pstProcInfo->pszProcessName);
	HLIB_RPC_Notify(pstHama->nRPCHandle, "hama:onRequestFork", "s", pstProcInfo->pszProcessName);
#else
	pid_t	pid;

//1 필요 없을 듯하여 막음. Test 후 필요시 추가 작업
#if 0//defined(CONFIG_DROP_ROOT_PRIVILEGES)
	SECAPI_Vfork(pstProcInfo->pszProcessName, &pid);
#else
	pid = vfork();
	if (!pid)
	{
		setsid();
		HxLOG_Print("[%s](%d) Spawn application : [%s]\n", __FUNCTION__, __LINE__, pstProcInfo->pszProcessName);
		if (execvp(pstProcInfo->pszProcessName, NULL) < 0)
			exit(SIGABRT);

		exit(0);
	}
#endif
	pstProcInfo->stPid = pid;
	pstProcInfo->eState = eHSPROC_STATE_RUNNING;
#endif
}

static HBOOL	local_scenario_check_condition(const HCHAR *condition)
{
	HxList_t	*list = s_listReadyApps;
	ReadyApps_t	*pstReadyApp;

	HxLOG_Debug("condition : [%s]\n", condition);
	while (list)
	{
		pstReadyApp = (ReadyApps_t*)HLIB_LIST_Data(list);
		if (HxSTD_StrCmp(pstReadyApp->szProcessName, condition) == 0)
			return TRUE;

		list = list->next;
	}

#if	defined(CONFIG_DEBUG) && !defined(DEBUG_RESURRECTION)
	HxLOG_Debug("Fail to find condition... [%s]\n", condition);
	list = s_listReadyApps;
	while (list)
	{
		pstReadyApp = (ReadyApps_t*)HLIB_LIST_Data(list);
		HxLOG_Debug("Condition List [%s]\n", pstReadyApp->szProcessName);
		list = list->next;
	}
#endif

	return FALSE;
}

HERROR	HAMA_SCENARIO_Init(HCHAR *pszFileName)
{
	HINT32		i, len;
	HxJSON_t	jAppList, jExecApp, jRoot = NULL;

	if (VK_SEM_Create((unsigned long*)&s_ulScenarioSemId, "scensem", VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		HxLOG_Error("VK_SEM_Create() failed!!\n");
	}

	if (pszFileName)
		jRoot = HLIB_JSON_DecodeFile(pszFileName);

	if (jRoot == NULL)
	{
		HxLOG_Error("can't find scenario file (/etc/octosystem.ini)\n");
		return ERR_FAIL;
	}

	jAppList = HxJSON_Object_Get(jRoot, "hama");
	len = HLIB_JSON_GetSize(jAppList);
	for (i = 0; i < len; i++)
	{
		HamaSProcess_t	*procInfo;

		procInfo = (HamaSProcess_t*)HLIB_STD_MemCalloc(sizeof(HamaSProcess_t));

		jExecApp = HxJSON_Array_Get(jAppList, i);
		procInfo->pszProcessName   = (HCHAR*)HLIB_JSON_Object_GetString(jExecApp, "exec");
		procInfo->pszConditionName = (HCHAR*)HLIB_JSON_Object_GetString(jExecApp, "condition");
		procInfo->pszModuleName    = (HCHAR*)HLIB_JSON_Object_GetString(jExecApp, "module");
		procInfo->bMonitor         = HLIB_JSON_Object_GetInteger(jExecApp, "monitor");
		procInfo->bNeedReboot      = HLIB_JSON_Object_GetInteger(jExecApp, "termreboot");
		procInfo->eState           = eHSPROC_STATE_UNKNOWN;

		if (HxSTD_StrCmp(HLIB_JSON_Object_GetString(jExecApp, "type"), "auto") == 0)
			procInfo->eType		= eHSPROC_TYPE_AUTO;
		else
			procInfo->eType		= eHSPROC_TYPE_MANUAL;

		procInfo->pszProcessName = (procInfo->pszProcessName ? HLIB_STD_StrDup(procInfo->pszProcessName) : "");
		procInfo->pszConditionName = (procInfo->pszConditionName ? HLIB_STD_StrDup(procInfo->pszConditionName) : "");
		procInfo->pszModuleName    = (procInfo->pszModuleName ? HLIB_STD_StrDup(procInfo->pszModuleName) : "");

		s_pstProcessScenarioList = HLIB_LIST_Append(s_pstProcessScenarioList, (void*)procInfo);
	}

	HLIB_JSON_Delete(jRoot);

	return ERR_OK;
}

HINT32	HAMA_SCENARIO_GetPriority(HWINID_e applId, HSTACKTYPE_e eStackType)
{
	SCENARIO_t	*scenario;

	scenario = scenario_findInfo(applId);
	if (scenario == NULL)
	{
		switch (eStackType)
		{
			case eHSTACK_FOCUS:
				return PRIORITY_FOCUS_NORMAL;
			case eHSTACK_WINDOW:
				return PRIORITY_STACK_NORMAL;
			default:
				return PRIORITY_PANEL_NORMAL;
		}
	}

	switch (eStackType)
	{
		case eHSTACK_FOCUS:
			return scenario->focusOrder;
		case eHSTACK_WINDOW:
			return scenario->stackOrder;
		case eHSTACK_PANEL:
			return scenario->panelOrder;

		default:
			break;
	}

	return PRIORITY_NORMAL;
}

HBOOL	HAMA_SCENARIO_IsManagerControl(HWINID_e applId)
{
	SCENARIO_t	*scenario;

	scenario = scenario_findInfo(applId);
	if (scenario == NULL)
		return FALSE;

	return TRUE;
}

HBOOL	HAMA_SCENARIO_IsExternalAppId(HWINID_e applId)
{
	switch(applId)
	{
		case eAWINDOW_NETFLIX : 
			return TRUE;
		default :
			break;
	}
	return FALSE;
}

HUINT32	HAMA_SCENARIO_GetLowerFocusAppId(HWINID_e applId)
{
	SCENARIO_t	*scenario;
	HINT32		curFocus;

	scenario = scenario_findInfo(applId);
	if (scenario == NULL)
		return 0;

	curFocus = scenario->focusOrder;
	scenario = scenario_findLowerFocus(curFocus);
	if (scenario)
		return scenario->eApplId;

	return 0;
}

HUINT32	HAMA_SCENARIO_GetLowerStackAppId(HWINID_e applId)
{
	SCENARIO_t	*scenario;
	HINT32		curStack;

	scenario = scenario_findInfo(applId);
	if (scenario == NULL)
		return 0;

	curStack = scenario->stackOrder;
	scenario = scenario_findLowerStack(curStack);
	if (scenario)
		return scenario->eApplId;

	return 0;
}

#if 0
HINT32	HAMA_SCENARIO_GetReadyAppList(HINT32 *pnProgIdList, HINT32 nMaxNum)
{
	HxList_t	*list = s_listReadyApps;

	HINT32		nCnt;
	ReadyApps_t	*pstReadyApp;

	VK_SEM_Get(s_ulScenarioSemId);

	nCnt = 0;
	while (list)
	{
		pstReadyApp = (ReadyApps_t*)HLIB_LIST_Data(list);
		pnProgIdList[nCnt++] = pstReadyApp->nProgId;
		if (nCnt >= nMaxNum)
			break;

		list = list->next;
	}

	VK_SEM_Release(s_ulScenarioSemId);

	return nCnt;
}

HERROR	HAMA_SCENARIO_AddReadyApp(HINT32 nProgId, HCHAR *readyAppName)
{
	ReadyApps_t		*pstReadyApps;

	VK_SEM_Get(s_ulScenarioSemId);

	pstReadyApps = (ReadyApps_t*)HAMA_Calloc(sizeof(ReadyApps_t));
	pstReadyApps->nProgId = nProgId;
	HLIB_STD_StrNCpySafe(pstReadyApps->szProcessName, readyAppName, 256);
	s_listReadyApps = HLIB_LIST_Append(s_listReadyApps, (void*)pstReadyApps);
	VK_SEM_Release(s_ulScenarioSemId);

	return ERR_OK;
}

HERROR	HAMA_SCENARIO_SetMonitor(const HCHAR *pszProcName, HBOOL bMonitor)
{
	HxList_t	*list;
	HamaSProcess_t	*pstProcInfo;

	VK_SEM_Get(s_ulScenarioSemId);
	list = s_pstProcessScenarioList;
	while (list)
	{
		pstProcInfo = (HamaSProcess_t*)HLIB_LIST_Data(list);
		if (HxSTD_StrCmp(pstProcInfo->pszProcessName, pszProcName) == 0)
		{
			HxLOG_Warning("--> [%s] is changed --> [%d]\n", pszProcName, bMonitor);
			pstProcInfo->bMonitor = bMonitor;
			VK_SEM_Release(s_ulScenarioSemId);

			return ERR_OK;
		}
		list = list->next;
	}
	VK_SEM_Release(s_ulScenarioSemId);

	HxLOG_Warning("Can't find process name(%s) in scenario\n", pszProcName);

	return ERR_OK;
}

HERROR	HAMA_SCENARIO_ProcessStartedSignal(const HCHAR *pszProcessName, HINT32 pid)
{
	HxList_t		*list = s_pstProcessScenarioList;
	HamaSProcess_t	*procInfo;

	VK_SEM_Get(s_ulScenarioSemId);
	while (list)
	{
		procInfo = (HamaSProcess_t*)HLIB_LIST_Data(list);
		if (HxSTD_StrCmp(procInfo->pszProcessName, pszProcessName) == 0)
		{
			procInfo->stPid = pid;
			procInfo->eState = eHSPROC_STATE_RUNNING;

			HxLOG_Warning("Process is running state [%s]\n", pszProcessName);

			break;
		}

		list = list->next;
	}

	VK_SEM_Release(s_ulScenarioSemId);

	return ERR_OK;
}

HERROR	HAMA_SCENARIO_ProcessStoppedSignal(pid_t pid, int st)
{
	HxList_t		*list = s_pstProcessScenarioList;
	HamaSProcess_t	*procInfo;

	VK_SEM_Get(s_ulScenarioSemId);
	while (list)
	{
		procInfo = (HamaSProcess_t*)HLIB_LIST_Data(list);
		if (procInfo->stPid == pid)
		{
            if ( WIFEXITED(st) )
            {
				//	TODO: Check Status --> if it is working perfectly, we can remove monitor mode check.
				//	TODO: normal exit --> monitoring flag is disabled.
            }
            else if ( WIFSIGNALED(st) )
            {
				//	TODO: Check Status --> if it is working perfectly, we can remove monitor mode check.
				//	TODO: signaled --> monitoring flag is enabled.
            }
            else if ( WIFSTOPPED(st) )
            {
				//	TODO: Check Status --> if it is working perfectly, we can remove monitor mode check.
				//	TODO: monitoring flag is enabled.
            }
#if	defined(CONFIG_DEBUG) && !defined(DEBUG_RESURRECTION)
			procInfo->eState = eHSPROC_STATE_STOPPED;
#else
			if (procInfo->bMonitor)
				procInfo->eState = eHSPROC_STATE_STOPPEDSIGNAL;
			else
				procInfo->eState = eHSPROC_STATE_STOPPED;

			if (procInfo->bNeedReboot)
				system("reboot");
#endif
		}
		list = list->next;
	}
	VK_SEM_Release(s_ulScenarioSemId);

	return ERR_OK;
}

HERROR	HAMA_SCENARIO_RegisterProcessByLauncher(HCHAR *pszArg)
{
	//	LXC별 HamaLauncher가 관리하는 Process List를 등록하여 관리할 예정이 었으나,
	//	그럴 필요 없이, HAMA_SCENARIO_RunModule을 추가 호출함으로서 해결하는 방식으로 수정.
	//	HaMaLauncher에서 이로 인해 Spawn요청이 오더라도 해당 Process작업 군에 해당 Process가 도는지 확인하고 Spawn처리하게
	//	변경됨.
	(void)(pszArg);
	return ERR_OK;
}

HERROR	HAMA_SCENARIO_RunModule(HAMA_t *pstHama, const HCHAR *pszModules, void *userData)
{
	HBOOL		bLaunch;
	HxList_t	*list;
	HamaSProcess_t	*procInfo;

	VK_SEM_Get(s_ulScenarioSemId);

	HxLOG_Debug("--> pszModules : [%s]\n", pszModules);
	list = s_pstProcessScenarioList;
	while (list)
	{
		procInfo = (HamaSProcess_t*)HLIB_LIST_Data(list);

		bLaunch = TRUE;

#ifdef	DEBUG_RUN_MODULE
		HxLOG_Warning("[%s] Process [%s]\n", __FUNCTION__, procInfo->pszProcessName);
		HxLOG_Warning("[%s] eState  [%d]\n", __FUNCTION__, procInfo->eState);
		HxLOG_Warning("[%s] Modules [%s : %s]\n", __FUNCTION__, procInfo->pszModuleName, pszModules);
		HxLOG_Warning("[%s] condition [%s]\n", __FUNCTION__, procInfo->pszConditionName);
#endif

		if (procInfo->eState == eHSPROC_STATE_STOPPED)
		{
			list = list->next;
			continue;
		}

		if (procInfo->eState == eHSPROC_STATE_STOPPEDSIGNAL)
		{
			HxLOG_Debug("re-start process named [%s]\n", procInfo->pszProcessName);
			procInfo->eState = eHSPROC_STATE_READYTORUN;
		}

		if (procInfo->eState != eHSPROC_STATE_READYTORUN)
		{
			if (!HxSTD_StrEmpty(pszModules))
			{
				if (HxSTD_StrEmpty(procInfo->pszModuleName))
					bLaunch = FALSE;
				else
				if (HLIB_STD_StrCaseStr(pszModules, procInfo->pszModuleName) == NULL)
					bLaunch = FALSE;
			} else
			{
				if (!HxSTD_StrEmpty(procInfo->pszModuleName))
					bLaunch = FALSE;
			}

			if (procInfo->eState == eHSPROC_STATE_RUNNING)
				bLaunch = FALSE;

			if (bLaunch == TRUE)
				procInfo->eState = eHSPROC_STATE_READYTORUN;
		}

		if (!HxSTD_StrEmpty(procInfo->pszConditionName))
		{
			if (local_scenario_check_condition(procInfo->pszConditionName) == FALSE)
				bLaunch = FALSE;
		}

#ifdef	DEBUG_RUN_MODULE
		HxLOG_Warning("Application is launched!\n");
#endif
		if (bLaunch == TRUE)
		{
			HxLOG_Info("application [%s] is running (check pid...)\n", procInfo->pszProcessName);
			if (HLIB_STD_GetPID_ByProcessname(procInfo->pszProcessName) == -1)
			{
				HLIB_CMD_CaptureLog("%s Running Start",procInfo->pszProcessName);

				scenario_process_spawn(pstHama, procInfo);

				HLIB_CMD_CaptureLog("%s Running Done",procInfo->pszProcessName);
			} else
			{
				HLIB_CMD_CaptureLog("%s is already runn..", procInfo->pszProcessName);
			}
		}

		list = list->next;
	}

	VK_SEM_Release(s_ulScenarioSemId);

	return ERR_OK;
}
#endif

