/****************************************************************************
*
* File Name	: bus_mgrobj.c
*
* Description	: GWM의 Manager 관리 모듈..
*
*	* DO NOT MODIFY ! 	 DO NOT MODIFY ! 	 DO NOT MODIFY ! 	 DO NOT MODIFY ! *
*
* programed by Ahnsik, Choi.
*
* Revision History	:
*
* Date			Modification								Name
* -----------	-------------------------------------	------------------
* 2004/11/05	File Created.								AhnSik, Choi.
* 2008/01/10		revised									JK Baek
* 2008/02/25		alarming(for periodic process) added	JK Baek
*
****************************************************************************/



/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
#include <string.h>

#include <vkernel.h>

#include <octo_common.h>
#include <bus.h>
#include "_bus.h"

#if defined(CONFIG_USES_NFSROOT)
#define	OBAMA_BUS_EXECUTE_TIME_CHECK	100
#endif

#define OBAMA_BUS_WAITING_TIMEOUT	10	/* minutes */

/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */

/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */
STATIC	BUS_MgrObj_t		s_stMgrArray[BUS_MAX_MANAGER];
// DestroyAll 따위의 함수에 의해 DESTROY되는 Application이 또다시 DeatroyAll 따위의 함수를 호출하게 되면 Recursive 하게 호출되어 System HangUp의 가능성이 있음.
STATIC	HINT32			s_nFlagNowDestoryAll = 0;
STATIC	unsigned long		s_ulBusMgr_ReqMgrSem;

// DEBUG용
STATIC HBOOL				s_bBusMgrExecuting = FALSE;
STATIC BUS_MgrObj_t			s_stLastBusMgrObj;
STATIC HUINT32				s_ulLastMessage;
STATIC HUINT32				s_ulParam1;
STATIC HUINT32				s_ulParam2;
STATIC HUINT32				s_ulParam3;
STATIC HBOOL			 	s_bPrintMgrDestroyInfo = FALSE;



/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */

HERROR BUS_DBG_FindMsgBreakTracer (HUINT32 ulMsgValue, HBOOL *pbTraceAll);


STATIC	HINT32		bus_mgr_DirectCompare(LINK_OBJECT node1, LINK_OBJECT node2)
{ // 같은지 아닌지만 비교한다.
	return	(HINT32)node1 - (HINT32)node2;
}

STATIC BUS_Mgr_t	bus_mgr_AllocMgr(void)
{
	STATIC HINT32 	s_nCurrPos = 0;
	HINT32 			i;

	i = (s_nCurrPos +1) % BUS_MAX_MANAGER;
	while (i != s_nCurrPos)
	{
		if (s_stMgrArray[i].bAllocated == FALSE)
		{
			s_nCurrPos = i;
			s_stMgrArray[i].bAllocated = TRUE;

			return &s_stMgrArray[i];
		}

		i = (i+1) % BUS_MAX_MANAGER;
	}

	return NULL;
}

STATIC void			bus_mgr_FreeMgr(BUS_Mgr_t	app)
{
	HxLOG_Assert(app);

	app->link = NULL;
	app->bAllocated = FALSE;

	HxSTD_memset(app->szName, 0, MGR_NAME_LENGTH);		// RTTI 문자열을 클리어.
	app->nPriority = 0;

// Callback 은 App Destroy 후 타 App을 Create 할 때 Parent App로 callback을 줄 필요가 있으므로 여기서 NULL로 처리하지 않는다.
// 여기서 NULL로 안 해도 Alloc 해서 Create 할 때 새 Callback으로 덮어진다.
//	app->fnAppProc = NULL;
}

STATIC HINT32		bus_mgr_CompareMgrPriority(LINK_OBJECT node1, LINK_OBJECT node2)
{
	BUS_Mgr_t	app1, app2;

	HxLOG_Assert(node1);
	HxLOG_Assert(node2);

	app1 = (BUS_Mgr_t)node1;
	app2 = (BUS_Mgr_t)node2;
	return	app2->nPriority - app1->nPriority;
}

STATIC void	bus_mgr_UnlinkMgrObject(BUS_Mgr_t deleted_app, BUS_DestroyReason_t actionReason, HINT32 procReason)
{
	HxLOG_Assert(deleted_app);

	// 1. Application 리스트에서 삭제할 Application을 제거.
	bus_lnklist_RemoveItemWithCompare( (LINK_OBJECT*)&g_stMgrRoot, (LINK_OBJECT)deleted_app, bus_mgr_DirectCompare );

	// 삭제될 Application에 eMEVT_BUS_DESTROY 메세지 전달.  // 메세지를 전달하는 것은 링크트리스트에서 제거된 후에 호출할 것. 그렇지 않으면, GWM_DESTROY 메세지를 받아서 DestroyAllApplicationAfter 를 사용하는 경우, Recursive하게 호출되기 때문에 무한루프에 빠진다.  				- GWM_DESTROY 를 PostMessage 로 Queue에 넣어 처리하지 않는 경우
	bus_mgr_Call(deleted_app, eMEVT_BUS_DESTROY, (Handle_t) NULL, (HINT32)actionReason, actionReason, 0);


	// 6. Application 객체의 메모리를 해제.
	bus_mgr_FreeMgr(deleted_app);
}

/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */

void	bus_mgr_Init(void)
{
	HINT32		i;

	HLIB_DBG_SetupTrace("BUS", 5, "p:msg", "s:proc", "p:p1", "p:p2", "p:p3");
	HLIB_DBG_SetupTrace("BUSTIME", 6, "p:msg", "s:proc", "p:p1", "p:p2", "p:p3", "i:duration");
	for (i=0; i<BUS_MAX_MANAGER; i++)
	{
		s_stMgrArray[i].link			= NULL;
		s_stMgrArray[i].bAllocated		= FALSE;
		HxSTD_memset(s_stMgrArray[i].szName, 0, MGR_NAME_LENGTH);		// RTTI 문자열을 클리어.
		s_stMgrArray[i].nPriority		= 0;
		s_stMgrArray[i].fnMgrProc		= NULL;
		s_stMgrArray[i].fnParentMgrProc	= NULL;
	}

	if(VK_SEM_Create (&s_ulBusMgr_ReqMgrSem, "GwmReqAppSem", VK_SUSPENDTYPE_FIFO) != VK_OK)
	{
		return;
	}
}

BUS_Result_t		bus_mgr_Call(BUS_Mgr_t app, HINT32 message, Handle_t handle, HINT32 param1, HINT32 param2, HINT32 param3)
{
	BUS_Mgr_t		backupApp;
	BUS_Result_t	eRes;
	HBOOL			bTraceAll;
	HERROR			hErr;

	if (app == NULL)
	{
		HxLOG_Error("\n\n\t No Application in bus_mgr_Call() !! \n\n\a");
		return ERR_BUS_NO_OBJECT;
	}

	backupApp = g_stCurrentMgr;
	g_stCurrentMgr = app;

	if (app->fnMgrProc != NULL)
	{
#ifdef	GARA_CODE
		print_memdbg();
#endif

#if	OBAMA_BUS_EXECUTE_TIME_CHECK
		HUINT32		ulSystemTick;
#endif
//		HLIB_DBG_BeginTrace("BUS");
//		HLIB_DBG_Trace("BUS", message, BUS_MGR_GetMgrName(app), param1, param2, param3);

#if	OBAMA_BUS_EXECUTE_TIME_CHECK
		ulSystemTick = HLIB_STD_GetSystemTick();
#endif

		s_bBusMgrExecuting = TRUE;
		HxSTD_MemCopy (&s_stLastBusMgrObj, app, sizeof(BUS_MgrObj_t));
		s_ulLastMessage = message;
		s_ulParam1 = param1;
		s_ulParam2 = param2;
		s_ulParam3 = param3;

        if(message != eMEVT_BUS_SHUTDOWN_DONE)
        {
            HLIB_DBG_SetupTimeLimit(HLIB_DBG_Trace("BUS", eRes = app->fnMgrProc(message, handle, param1, param2, param3), message, BUS_MGR_GetMgrName(app), param1, param2, param3),
                BUS_MGR_GetMgrName(app),(OBAMA_BUS_WAITING_TIMEOUT*60),"msg:0x%x,p1:0x%x,p2:0x%x,p3:0x%x",message,param1, param2, param3);
        }
        else
        {
            HLIB_DBG_Trace("BUS", eRes = app->fnMgrProc(message, handle, param1, param2, param3), message, BUS_MGR_GetMgrName(app), param1, param2, param3);
        }
//		eRes = app->fnMgrProc(message, handle, param1, param2, param3);
#if	OBAMA_BUS_EXECUTE_TIME_CHECK
		if ((HLIB_STD_GetSystemTick() - ulSystemTick) > OBAMA_BUS_EXECUTE_TIME_CHECK)
		{
			HLIB_DBG_Trace("BUSTIME", {}, message, BUS_MGR_GetMgrName(app), param1, param2, param3, HLIB_STD_GetSystemTick() - ulSystemTick);
#if	0
			HLIB_DBG_BeginTrace("BUSTIME");
			HLIB_DBG_Trace("BUSTIME", message, BUS_MGR_GetMgrName(app), param1, param2, param3, HLIB_STD_GetSystemTick() - ulSystemTick);
			HLIB_DBG_EndTrace("BUSTIME");
#endif
		}
#endif
//		HLIB_DBG_EndTrace("BUS");

		s_bBusMgrExecuting = FALSE;
	}
	else
	{
		eRes = ERR_BUS_NO_OBJECT;
	}

	g_stCurrentMgr = backupApp;

	bTraceAll = FALSE;
	hErr = BUS_DBG_FindMsgBreakTracer (message, &bTraceAll);
	if (hErr == ERR_OK)
	{
		if (eRes == MESSAGE_BREAK)
			HLIB_CMD_Printf (" = MSG 0x%08X h[0x%08X], (%d, %d, %d) in APP [%s] : msg break\n", message, handle, param1, param2, param3, app->szName);
		else if (bTraceAll == TRUE)
			HLIB_CMD_Printf (" = MSG 0x%08X h[0x%08X], (%d, %d, %d) in APP [%s] : msg pass\n", message, handle, param1, param2, param3, app->szName);
	}

	return eRes;
}


BUS_Mgr_t		bus_mgr_GetNext(BUS_Mgr_t app)
{
	if (app != NULL)
		return	(BUS_Mgr_t)bus_lnklist_GetNext((LINK_OBJECT)app);//app->link.nextObject;
	else
		return	NULL;
}


/*
	BUS_MGR_RequestToCreate 함수의 Sync를 위한 기능
*/

void			bus_mgr_GetReqMgrSem (void)
{
	VK_SEM_Get (s_ulBusMgr_ReqMgrSem);
}

void			bus_mgr_ReleaseReqMgrSem (void)
{
	VK_SEM_Release (s_ulBusMgr_ReqMgrSem);
}

BUS_Result_t	bus_mgr_CreateByRequest (BUS_MgrReq_t *pstReq)
{
#if 0
	HINT32			nPriority;
	BUS_Mgr_t		this_app, new_app;
	BUS_Callback_t	this_callback;
	BUS_Result_t	eRes;
	HINT32			len = 0;
#endif
	BUS_Mgr_t		 new_app;
	BUS_Result_t	 eRes;

	if (pstReq->stMgrStatus.fnMgrProc == NULL)
	{
		return ERR_BUS_INITIAL_FAILED;
	}

	// 먼저, 이미 같은 이름의 Application이 동작하고 있는지 확인한다.
	new_app = BUS_MGR_Get (pstReq->stMgrStatus.fnMgrProc);
	if (new_app != NULL)
	{ // 이미 같은 이름의 Application이 동작 중이다.
		return ERR_BUS_INITIAL_FAILED;
	}

	// 새로운 application의 메모리를 할당.
	new_app = bus_mgr_AllocMgr();
	if (new_app == NULL)			{ return ERR_BUS_OUT_OF_MEMORY; }

	// 기본속성을 지정.
	new_app->link = NULL;
	HxSTD_memcpy (new_app->szName, pstReq->stMgrStatus.szName, MGR_NAME_LENGTH);
	new_app->nPriority			= pstReq->stMgrStatus.nPriority;
	new_app->fnMgrProc			= pstReq->stMgrStatus.fnMgrProc;
	new_app->fnParentMgrProc	= NULL;

	// Linked List(g_stMgrRoot)에 추가.
	eRes = bus_lnklist_AddItemWithCompareDescend( (LINK_OBJECT*)&g_stMgrRoot, (LINK_OBJECT)new_app, bus_mgr_CompareMgrPriority);

	if (eRes != ERR_BUS_NO_ERROR)
		return eRes;

	// 새로운 Application에 eMEVT_BUS_CREATE 메세지 전달.
	bus_mgr_Call (new_app, eMEVT_BUS_CREATE, pstReq->hHandle, pstReq->p1, pstReq->p2, pstReq->p3);

	// Create가 끝난 다음 모든 Application에 이 Application이 생성되었음을 알린다.
	// 단, Create 하자 마자 Destroy 된 App의 경우, Spawned 메시지를 보내지 않는다.
	new_app = BUS_MGR_Get (pstReq->stMgrStatus.fnMgrProc);
	if (new_app != NULL)
	{
		// p1 : application proc function,  p2 : parental proc function
		BUS_SendMessage (NULL, eMEVT_BUS_MGR_SPAWNED, pstReq->hHandle, (HINT32)pstReq->stMgrStatus.fnMgrProc, (HINT32)NULL, 0);
	}

	return	eRes;
}



// Application이 호출하여 사용할 기능.


#define		_____GWM_APP_INFO______

BUS_Mgr_t		BUS_MGR_Get(BUS_Callback_t fnAppProc)
{
	BUS_Mgr_t		appList;

	if(fnAppProc != NULL)
	{
		appList = (BUS_Mgr_t)g_stMgrRoot;
		while(appList != NULL)
		{
			if (appList->fnMgrProc == fnAppProc)		// 이미 같은 이름의 Application이 실행중.
			{
				return	appList;
			}
			appList = (BUS_Mgr_t)bus_lnklist_GetNext((LINK_OBJECT)appList);//(BUS_Mgr_t)appList->link.nextObject;
		}
	}

	return NULL;
}

BUS_Mgr_t		BUS_MGR_GetThis(void)
{
	return g_stCurrentMgr;
}

BUS_Callback_t	BUS_MGR_GetParentCallback(void)
{
	return (g_stCurrentMgr != NULL) ? g_stCurrentMgr->fnParentMgrProc : NULL;
}

HINT8 *BUS_MGR_GetMgrName(BUS_Mgr_t app)
{
	if (app != NULL)
	{
		return	app->szName;
	}
	else
	{
		return	"not exists app~!";
	}
}

void BUS_MGR_SetMgrName(BUS_Mgr_t app, HINT8 *pszProcName)
{
	if (app == NULL)
	{
		return;
	}

	if ((pszProcName != NULL) && (pszProcName[0] != '\0'))
	{
		if(app->szName[0] == '\0')
		{ /* 비어 있으면 덮어 쓰고 ... */
			strncpy(app->szName, pszProcName, MGR_NAME_LENGTH - 1);		// RTTI 문자열을 복사.
			app->szName[MGR_NAME_LENGTH - 1] = '\0';
		}
		else
		{ /* 이미 있으면 못 쓴다. */
		}
	}
	else
	{
		app->szName[0] = '\0';
	}

}


BUS_Callback_t	BUS_MGR_GetMgrCallback(BUS_Mgr_t app)
{
	if (app != NULL)
	{
		return app->fnMgrProc;
	}
	else
	{
		HxLOG_Critical("\n\n\n\n");
		return NULL;
	}
}


#define		_____GWM_APP_CREATE_DELETE_____

BUS_Result_t		BUS_MGR_Create(HINT8 *szAppName, HUINT32 ulProperty, BUS_Callback_t fnAppProc,
									HINT32 handle, HINT32 param1, HINT32 param2, HINT32 param3)
{
	HINT32			nPriority;
	BUS_Mgr_t		this_app, new_app;
	BUS_Callback_t	this_callback;
	BUS_Result_t	eRes;
	HINT32			len = 0;

	if (fnAppProc == NULL)
	{
		return ERR_FAIL;
	}

	this_callback = NULL;

	this_app = BUS_MGR_GetThis();
	this_callback = (this_app != NULL) ? BUS_MGR_GetMgrCallback (this_app) : NULL;

	len = (szAppName != NULL) ? strlen(szAppName) : 0;
	if (len == 0)
	{
		//HxLOG_Print ("[BUS_MGR_Create] application name is NULL or No string.\n");
	}

	// 먼저, 이미 같은 이름의 Application이 동작하고 있는지 확인한다.
	new_app = BUS_MGR_Get(fnAppProc);
	if (new_app != NULL)
	{ // 이미 같은 이름의 Application이 동작 중이다.
		BUS_SendMessage (NULL, eMEVT_BUS_MGR_ALREADY_EXIST, handle, (HINT32)fnAppProc, (HINT32)this_callback, 0);
		return ERR_BUS_INITIAL_FAILED;
	}

	// 새로운 application의 메모리를 할당.
	new_app = bus_mgr_AllocMgr();
	if (new_app == NULL)			{ return ERR_BUS_OUT_OF_MEMORY; }

	// 기본속성을 지정.
	new_app->link = NULL;

	if (len > 0)
	{
		strncpy (new_app->szName, szAppName, MGR_NAME_LENGTH - 1);		// RTTI 문자열을 복사.
		new_app->szName[MGR_NAME_LENGTH - 1] = '\0';
	}
	else
	{
		new_app->szName[0] = '\0';
	}

	nPriority	= (HINT32)(ulProperty & BUS_MASK_PRIORITY);

	new_app->nPriority	= nPriority;
	new_app->fnMgrProc	= fnAppProc;
	new_app->fnParentMgrProc = (this_app != NULL) ? this_app->fnMgrProc : NULL;

	// Linked List(g_stMgrRoot)에 추가.
	eRes = bus_lnklist_AddItemWithCompareDescend( (LINK_OBJECT*)&g_stMgrRoot, (LINK_OBJECT)new_app, bus_mgr_CompareMgrPriority);

	if (eRes != ERR_BUS_NO_ERROR)
		return eRes;

	// 새로운 Application에 eMEVT_BUS_CREATE 메세지 전달.
	bus_mgr_Call (new_app, eMEVT_BUS_CREATE, handle, param1, param2, param3);

	// Create가 끝난 다음 모든 Application에 이 Application이 생성되었음을 알린다.
	// 단, Create 하자 마자 Destroy 된 App의 경우, Spawned 메시지를 보내지 않는다.
	new_app = BUS_MGR_Get (fnAppProc);
	if (new_app != NULL)
	{
		// p1 : application proc function,  p2 : parental proc function
		BUS_SendMessage (NULL, eMEVT_BUS_MGR_SPAWNED, handle, (HINT32)fnAppProc, (HINT32)this_callback, 0);
	}
	else
	{
		eRes = ERR_BUS_INITIAL_FAILED;
	}

	return	eRes;
}


/*
	GWM 안에서 불리면 절대 안 되는 함수
*/
BUS_Result_t		BUS_MGR_RequestToCreate (HINT8 *szAppName, HUINT32 ulProperty, BUS_Callback_t fnAppProc,
									HINT32 handle, HINT32 param1, HINT32 param2, HINT32 param3, HBOOL bSync)
{
	HUINT32			 ulNameLen;
	BUS_MgrReq_t	*pstAppReq = NULL;
	BUS_Result_t	 eRet;

	if (fnAppProc == NULL)
	{
		return ERR_BUS_NO_OBJECT;
	}

	pstAppReq = OxAP_Malloc (sizeof(BUS_MgrReq_t));
	if (pstAppReq == NULL)
	{
		return ERR_BUS_NO_OBJECT;
	}

	HxSTD_memset (pstAppReq, 0, sizeof(BUS_MgrReq_t));

	ulNameLen = (szAppName != NULL) ? VK_strlen (szAppName) : 0;
	if (ulNameLen > 0)
	{
		HLIB_STD_StrNCpySafe(pstAppReq->stMgrStatus.szName, szAppName, MGR_NAME_LENGTH);
	}

	pstAppReq->stMgrStatus.nPriority		= (ulProperty & BUS_MASK_PRIORITY);
	pstAppReq->stMgrStatus.fnMgrProc		= fnAppProc;
	pstAppReq->stMgrStatus.fnParentMgrProc	= NULL;
	pstAppReq->hHandle	= handle;
	pstAppReq->p1		= param1;
	pstAppReq->p2		= param2;
	pstAppReq->p3		= param3;

	if (bSync == TRUE)
	{
		bus_mgr_GetReqMgrSem();
	}

	eRet = BUS_PostMessage (NULL, eMEVT_BUS_REQUEST_CREATE, HANDLE_NULL, (HINT32)pstAppReq, bSync, 0);
	if (eRet != ERR_OK)
	{
		OxAP_Free (pstAppReq);
		if (bSync == TRUE)
		{
			bus_mgr_ReleaseReqMgrSem();
		}
		return ERR_BUS_INITIAL_FAILED;
	}

	if (bSync == TRUE)
	{
		bus_mgr_GetReqMgrSem();
		bus_mgr_ReleaseReqMgrSem();
	}

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t			bus_mgr_Destroy(BUS_Callback_t fnAppProc, BUS_DestroyReason_t actionReason, HINT32 procReason, const char *szFunction, HUINT32 ulLine)
{
	BUS_Mgr_t		delete_app, AppBackupPtr;
	BUS_Callback_t	delete_app_callback;

	//HxLOG_Info("Application Destroy됩니다.%s from %s \n",application_name, BUS_MGR_GetMgrName(g_stCurrentMgr) );
	//HxLOG_Assert(GWM_IsGwmTask() == TRUE);

	// 삭제되는 대상이 어떤 Application인지 먼저 확인.
	if (fnAppProc == NULL)
	{
		delete_app = g_stCurrentMgr;
	}
	else
	{
		delete_app = BUS_MGR_Get(fnAppProc);
	}

	if (delete_app == NULL)			// 삭제하려는 Application이 실행중이 아니다.
	{
		return ERR_BUS_NO_OBJECT;
	}

#if defined(CONFIG_DEBUG)
	if (TRUE == s_bPrintMgrDestroyInfo)
	{
		HLIB_CMD_Printf("\n\n\n");
		HLIB_CMD_Printf(">>>>> Proc(0x%08x:%s) was destoryed in (%s:%d)\n", (HUINT32)delete_app, delete_app->szName, szFunction, ulLine);
		HLIB_CMD_Printf("\n\n\n");
	}
#endif
	HxLOG_Warning("Destroying... procName[%s]\n", delete_app->szName);

	delete_app_callback = BUS_MGR_GetMgrCallback (delete_app);

	AppBackupPtr = g_stCurrentMgr;
	g_stCurrentMgr = delete_app;

	// 1. 설정된 TIMER가 있다면 모두 해제한다.
	bus_timer_KillAll(delete_app);

	/* eMEVT_BUS_DESTROY msg는 unlink하면서 불리게 된다. */

				// 고려해야 할 문제점이 하나 있다. BUS_MGR_Destroy(0)를 호출하고 나서, 메세지 처리가 끝나기 전에 다른 무엇을 계속하게 되는 경우,
				// 이미 해당 Application 은 GWM_DESTROY 를 처리한 다음의 루틴이 되므로 DESTROY 에서 포인터 등의 변수가 초기화 되어 System Crash 의 원인이 되기도 한다.
				// 따라서, OC-1000 에서와 같이 DESTROY 방식은 메세지 QUEUE 를 이용하여 처리하도록 해야만 할 것이다.
				// --> 이러한 문제는 Application 측에서 관리를 잘 못한 문제가 된다. 이와 같은 경우가 GWM측의 문제가 되는 경우는 Application Object 자체가 memfree 되는 경우에만 한하게 된다.
	bus_mgr_UnlinkMgrObject(delete_app, actionReason, procReason);

	g_stCurrentMgr = AppBackupPtr;


	// Delete 가 끝난 다음 모든 Application에 이 Application이 삭제되었음을 알린다.
	// p1 : application proc function
	if (delete_app_callback != NULL)
	{
		BUS_SendMessage (NULL, eMEVT_BUS_MGR_KILLED, HANDLE_NULL, (HINT32)delete_app_callback, actionReason, procReason);
	}

	return	ERR_BUS_NO_ERROR;
}

BUS_Result_t			__BUS_MGR_DestroyWithReason(BUS_Callback_t fnAppProc, BUS_DestroyReason_t eDestroyReason, HINT32 procReason, const char *szFunction, HUINT32 ulLine)
{
	HxLOG_Print("BUS_MGR_DestroyWithReason fnAppProc[%x] reason[%d] [%d]\n", fnAppProc, eDestroyReason, procReason);
	return bus_mgr_Destroy(fnAppProc, (HINT32)eDestroyReason, procReason, szFunction, ulLine);
}

BUS_Result_t			__BUS_MGR_Destroy(BUS_Callback_t fnAppProc, const char *szFunction, HUINT32 ulLine)
{
	return bus_mgr_Destroy(fnAppProc, eDESTROY_BUS_MYSELF, 0, szFunction, ulLine);
}


BUS_Result_t			BUS_MGR_DestroyAll(void)
{
	BUS_Mgr_t		traverse;
	BUS_Mgr_t		reverseAppList[BUS_MAX_MANAGER] = {0, };
	HINT32			nAppCount;

	if (s_nFlagNowDestoryAll)
		return ERR_BUS_NO_OBJECT;
	s_nFlagNowDestoryAll = 1;

//// tempApp 로 백업해 둔 포인터 변수는 BUS_MGR_Destroy() 함수자체가 Recursive 하게 호출됨에 따라 그 내부에서 다른 Application들을 강제로 Destroy 하는 등의 이유로
//// 변질될 가능성이 있기 때문에, 이러한 경우 Destroy 후의 		traverse = tempApp;  여기에서 유효성이 보장되지 않는다.
//// 따라서 Destroy 될 Application 들을 미리 변수에 저장한 후에 처리하도록 수정하였다.
/*	traverse = g_stMgrRoot;
	while(traverse != NULL)
	{
		tempApp = bus_mgr_GetNext(traverse);		//-> 여기에서 Backup 해 둔 Pointer 가
		BUS_MGR_Destroy(traverse->fnAppProc);		//-> 여기에서 Recursive 하게 호출되면서 변질될 수 있다. (GWM_DESTROY 메세지를 받아 Linked list 상의 후반에 붙은 Application을 지우는 경우.)
		traverse = tempApp;								//-> 그러면 여기에서는 이미 무효하게 되므로 버그 발생 !!
	}
*/
	// Application을 역순으로 저장.
	traverse = g_stMgrRoot;
	nAppCount = 0;
	while (traverse != NULL)
	{
		reverseAppList[nAppCount] = traverse;
		nAppCount++;
		traverse = bus_mgr_GetNext(traverse);
	}

	while (nAppCount > 0)
	{
		nAppCount--;
		BUS_MGR_Destroy(reverseAppList[nAppCount]->fnMgrProc);
	}
//// 2005.10.24.  By AhnSik.

	s_nFlagNowDestoryAll = 0;
	return MESSAGE_BREAK;		//BUS_MGR_Destroy(g_stCurrentMgr->fnAppProc);
}


#define		_____GWM_APP_DEBUGGING______

STATIC void	local_gwmapp_DumpManager(BUS_Mgr_t mgrObj)
{
	HLIB_CMD_Printf("Manager=[" HxANSI_COLOR_CYAN("%s") "]%X:\n", mgrObj->szName,(HINT32)mgrObj);
	if (mgrObj==NULL)
		return;

	HLIB_CMD_Printf("\tpriority=%d, fnAppProc=%08X, \n", mgrObj->nPriority, (HINT32)mgrObj->fnMgrProc);
}

HINT32	BUS_DBG_DumpMgrList(void *arg)
{
	BUS_Mgr_t		base_mgr;

	HLIB_CMD_Printf ("\n------ BUS_DBG_DumpMgrList  ------\n");

	base_mgr = (BUS_Mgr_t)g_stMgrRoot;
	while(base_mgr != NULL)
	{	local_gwmapp_DumpManager(base_mgr);
		base_mgr = (BUS_Mgr_t)bus_lnklist_GetNext((LINK_OBJECT)base_mgr);
	}

	(void)arg;



	return 0;
}

HINT32	BUS_DBG_ShowLastMgr (void *arg)
{
	HLIB_CMD_Printf (">>> Last Executed MGR:\n");
	HLIB_CMD_Printf ("   Name [%s]\n", s_stLastBusMgrObj.szName);
	HLIB_CMD_Printf ("   Function [0x%08x]\n", s_stLastBusMgrObj.fnMgrProc);
	HLIB_CMD_Printf ("	 message (0x%08x) param1(0x%08x) param2(0x%08x) param3(0x%08x)\n", s_ulLastMessage, s_ulParam1, s_ulParam2, s_ulParam3);
	HLIB_CMD_Printf ("   Executing... (%d)\n", s_bBusMgrExecuting);

	return 0;
}



#define xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#define MAX_TRACE_MSGBREAK			16

typedef struct
{
	HUINT32		 ulMsgId;
	HBOOL		 bTraceAll;
} BUS_MsgTracer_t;

STATIC HUINT32			 s_ulNumMsgBreakTracer = 0;
STATIC BUS_MsgTracer_t	 s_astMsgTracerArray[MAX_TRACE_MSGBREAK];

HERROR BUS_DBG_RegisterMsgBreakTracer (HUINT32 ulMsgId, HBOOL bTraceAll)
{
	HUINT32		 ulIndex;

	if (s_ulNumMsgBreakTracer >= MAX_TRACE_MSGBREAK)
	{
		return ERR_FAIL;
	}

	for (ulIndex = 0; ulIndex < s_ulNumMsgBreakTracer; ulIndex++)
	{
		if (s_astMsgTracerArray[ulIndex].ulMsgId == ulMsgId)
		{
			s_astMsgTracerArray[ulIndex].bTraceAll = bTraceAll;
			return ERR_OK;
		}
	}

	s_astMsgTracerArray[s_ulNumMsgBreakTracer].ulMsgId = ulMsgId;
	s_astMsgTracerArray[s_ulNumMsgBreakTracer].bTraceAll = bTraceAll;
	s_ulNumMsgBreakTracer ++;

	return ERR_OK;
}

HERROR BUS_DBG_UnregisterMsgBreakTracer (HUINT32 ulDelIndex)
{
	HUINT32		 ulIndex;

	if (ulDelIndex >= s_ulNumMsgBreakTracer)
	{
		return ERR_FAIL;
	}

	for (ulIndex = ulDelIndex; ulIndex < s_ulNumMsgBreakTracer - 1; ulIndex++)
	{
		s_astMsgTracerArray[ulIndex] = s_astMsgTracerArray[ulIndex + 1];
	}

	s_ulNumMsgBreakTracer --;

	return ERR_OK;
}

HERROR BUS_DBG_FindMsgBreakTracer (HUINT32 ulMsgValue, HBOOL *pbTraceAll)
{
	HUINT32		 ulIndex;

	for (ulIndex = 0; ulIndex < s_ulNumMsgBreakTracer; ulIndex++)
	{
		if (s_astMsgTracerArray[ulIndex].ulMsgId == ulMsgValue)
		{
			if (pbTraceAll != NULL)
			{
				*pbTraceAll = s_astMsgTracerArray[ulIndex].bTraceAll;
			}

			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR BUS_DBG_DumpMsgBreakTracer (void)
{
	HUINT32		 ulIndex;

	HLIB_CMD_Printf ("\n\n==========================================================\n");
	HLIB_CMD_Printf (" Msg Break Tracer :\n");
	if (s_ulNumMsgBreakTracer == 0)
	{
		HLIB_CMD_Printf ("     Noting Registered.\n");
	}
	else
	{
		for (ulIndex = 0; ulIndex < s_ulNumMsgBreakTracer; ulIndex++)
		{
			HLIB_CMD_Printf (" 	Index[%02d] : Msg 0x%08x (%s)\n", ulIndex, s_astMsgTracerArray[ulIndex].ulMsgId,
										(s_astMsgTracerArray[ulIndex].bTraceAll != FALSE) ? "Trace All" : "Trace Break");
		}
	}
	HLIB_CMD_Printf ("==========================================================\n");

	return ERR_OK;
}

HERROR BUS_DBG_SetDestroyLog (HBOOL bDisplay)
{
	s_bPrintMgrDestroyInfo = bDisplay;
	return ERR_OK;
}


/* end of file */
