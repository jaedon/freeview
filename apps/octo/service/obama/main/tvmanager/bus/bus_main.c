/****************************************************************************
*
* File Name	: bus_main.c
*
* Description	: BUS main task and processing messages
*
*
* Revision History	:
*
* Date			Modification				Name
* -----------	-------------------------------------	------------------
* 2008/01/09		initial revision				JK Baek
* 2008/02/25		embeding alarm in GWM	JK Baek
*
****************************************************************************/

/* ======================================================================== */
/* Include. 																*/
/* ======================================================================== */
#include <string.h>

#include <octo_common.h>
//#include <mwc_util.h>

#include <bus.h>
#include <obama_rpcprotocol.h>

#include "_bus.h"

#include <otl.h>
/* ======================================================================== */
/* Global/Extern Variables.													*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Macros. 															*/
/* ======================================================================== */
#define BUS_MSG_QUE_NAME		"busMsgQ"
#define BUS_MAIN_TASK_NAME		"tBUS"

/* ======================================================================== */
/* Private Types.															*/
/* ======================================================================== */

/* ======================================================================== */
/* Global Variables. 														*/
/* ======================================================================== */
BUS_Mgr_t						g_stMgrRoot;
BUS_Mgr_t						g_stCurrentMgr;

/* ======================================================================== */
/* Private Constants. 														*/
/* ======================================================================== */

/* ======================================================================== */
/* Private Variables. 														*/
/* ======================================================================== */
STATIC HULONG					s_ulBusMainTaskId;
STATIC HULONG					s_ulBusMainMsgQId;
STATIC BUS_Callback_t			s_pfnAppIgnitor;
STATIC HUINT32					s_ulBusMainThreadId;


/* ======================================================================== */
/* Private Function prototypes. 											*/
/* ======================================================================== */

/* ======================================================================== */
/* Exported Functions. 														*/
/* ======================================================================== */

#if 0
STATIC BUS_Result_t		bus_main_ProcessOAPI(BUS_InternalMsg_t *msg)
{
	BUS_Result_t			eRes;
	HINT32				nAppCount, i;

	HxLOG_Info("++ %s() Start ++ (0x%x)\n", __FUNCTION__,msg->message);

	if ( MSG_OAPI_APP_REG == msg->message)
	{
		HxLOG_Print("[%s:%d] appCallback:0x%X AppName:%s msg:MSG_OAPI_APP_REG param1:0x%X param2:0x%X param3:0x%X",__FUNCTION__,__LINE__,
			msg->appCallback,msg->handle, msg->param1,msg->param2,msg->param3);

		// this is alloced string (Application Name)
		API_MEM_Free(msg->handle);

	}
	else
	{
		HxLOG_Print("[%s:%d] appCallback:0x%X handle:0x%X msg:0x%X param1:0x%X param2:0x%X param3:0x%X",__FUNCTION__,__LINE__,
			msg->appCallback,msg->handle,msg->message,msg->param1,msg->param2,msg->param3);
	}

END_OF_MSG_PROCESS:
	HxLOG_Info("-- %s() End ++\n",__FUNCTION__);

	return ERR_BUS_NO_ERROR;
}
#endif

STATIC BUS_Result_t		bus_main_ProcessMessage(BUS_InternalMsg_t *msg)
{
	BUS_Result_t		eRes;
	BUS_Mgr_t			traverse;
	BUS_Mgr_t			backupAppList[BUS_MAX_MANAGER];
	HINT32				nAppCount, i;

	HxLOG_Info("++ bus_main_ProcessMessage() Start ++ (0x%x)\n", msg->message);

	if (msg->message == eMEVT_BUS_REQUEST_CREATE)
	{
		BUS_MgrReq_t	*pstReq = (BUS_MgrReq_t *)msg->param1;

		if (pstReq != NULL)
		{
			bus_mgr_CreateByRequest (pstReq);
			OxAP_Free (pstReq);
		}

		if (msg->param2 == TRUE)
		{
			bus_mgr_ReleaseReqMgrSem();
		}

		goto END_OF_MSG_PROCESS;
	}

	// 특정한 Application을 Target으로 하는 메세지의 경우에는, 그 Target Application을 먼저 결정한다.

	/* If the target app is specified, find it and send the message */
	if (msg->appCallback != NULL)
	{
		eRes = bus_mgr_Call(BUS_MGR_Get(msg->appCallback), msg->message, msg->handle, msg->param1, msg->param2, msg->param3);
		goto END_OF_MSG_PROCESS;
	}

	// 그렇지 않으면 모든 Application을 순회하며 메세지 전달.
	// 일단 메세지가 전달 될 후보(?) app 들을 모두 백업해 둔다.
	traverse = g_stMgrRoot;
	nAppCount = 0;
	while (traverse != NULL)
	{
		backupAppList[nAppCount] = traverse;
		nAppCount++;
		traverse = bus_mgr_GetNext(traverse);
	}

	// 백업된 App 들에 대해서 순서대로 메세지를 전달.
	for (i = 0; i < nAppCount; i++)
	{
		// 그러나, backupAppList 에 백업된 app 이 다른 app의 메세지 처리 도중 이미 삭제되었을 가능성이 있으므로, 현재 살아있는 App 에 대해서만 실행한다.
		traverse = g_stMgrRoot;
		while (traverse != NULL)
		{
			if (traverse == backupAppList[i] )
			{ // 해당하는 Application 이라면,
				HxLOG_Info("BUS_InternalMsg_t:%08X >>> %s, \n", msg->message, BUS_MGR_GetMgrName(traverse) );

				eRes = bus_mgr_Call(traverse, msg->message, msg->handle, msg->param1, msg->param2, msg->param3);
				if (eRes == MESSAGE_BREAK)
				{ // 메세지 처리가 끝났으므로, 다음 App로의 전달을 끊는다. (for 루프를 종료)
					HxLOG_Info("\033[01;20m GWM BUS_InternalMsg_t Porcess() met MESSAGE_BREAK, the app ate msg(0x%x) is %s.\033[00m\n", msg->message, BUS_MGR_GetMgrName(traverse));
					goto END_OF_MSG_PROCESS;
				}
				break;	// backupAppList[i] 는 while 루프중에 하나만 존재하므로, 메세지 처리가 끝나면 while 루프만 종료. for 루프는 계속 된다.
			}
			traverse = bus_mgr_GetNext(traverse);
		}
	}

END_OF_MSG_PROCESS:
	HxLOG_Info("-- bus_main_ProcessMessage() End ++\n");

	return ERR_BUS_NO_ERROR;
}

STATIC void				bus_main_Task(void *arg1)
{
	BUS_Result_t		eRes;
	BUS_InternalMsg_t 	msg;
	HERROR				err;
	unsigned long		timeout = 0;
//	HINT32				nDispatched;

	NOT_USED_PARAM(arg1);

	HxLOG_Info("\n\n++bus_main_Task Start ++\n\n\n");

	// for checking invalid message sends
	s_ulBusMainThreadId = HLIB_STD_GetTID();

	//	init obama rpc instance.
	OBAMA_RpcProtocol_ReadAndDispatch(1);


	/* launch system ignition app */
	if (NULL != s_pfnAppIgnitor)
	{
		BUS_MGR_Create(NULL, 1, (BUS_Callback_t)s_pfnAppIgnitor, 0, 0, 0, 0);
		bus_mgr_ReleaseReqMgrSem();
	}

	while (1)		/* infinite application task loop */
	{
		err = VK_MSG_ReceiveTimeout(s_ulBusMainMsgQId, &msg, sizeof(BUS_InternalMsg_t), timeout);

		//	MSG로 오건, Timeout으로 하건 일단 RpcDispatch는 해야 한다.
		if (OBAMA_RpcProtocol_ReadAndDispatch(1) > 0)
		{
			HxLOG_Info("Dispatched..\n");
		}

		if (err == VK_OK)
		{
			HxLOG_Info("msg=%08X, p=(%08X,%08X,%08X) to %s\n",
						msg.message, msg.param1, msg.param2, msg.param3, BUS_MGR_GetMgrName(BUS_MGR_Get(msg.appCallback)));

			eRes = bus_main_ProcessMessage(&msg);


			/*
			 *	fnFreeParam 이 유효 하면 param 은 alloc된 값으로 간주하고 free을 진행 한다.
			 */
			if( NULL != msg.fnFreeParam )
			{
				if ((void*)msg.param3 != NULL)
				{
					msg.fnFreeParam( (void*)msg.param3 );
				}
				else
				{
					HxLOG_Error("msg=%08X, p=(%08X,%08X,%08X) to %s\n",
						msg.message, msg.param1, msg.param2, msg.param3, BUS_MGR_GetMgrName(BUS_MGR_Get(msg.appCallback)));
				}
			}

			if (eRes != ERR_BUS_NO_ERROR)
			{
				HxLOG_Error("quit gwm task for bus_main_ProcessMessage()\n");
				break;
			}
		}
		{
			HINT32			nTimeOut;
			HUINT32			ulTimerId;
			BUS_Callback_t	pfAppCallback = NULL;
			BUS_Mgr_t		pstApp;

			nTimeOut = 0;
			while (nTimeOut == 0)
			{
				nTimeOut = bus_timer_CheckTimerOut(&pfAppCallback, &ulTimerId);
				if (nTimeOut == 0)
				{
					pstApp = BUS_MGR_Get(pfAppCallback);
					if (pstApp)
						bus_mgr_Call(pstApp, eMEVT_BUS_TIMER, 0, ulTimerId, 0, 0);
				}
				else
				{
					//HLIB_CMD_Printf("nTimeOut : [%d]\n", nTimeOut);
					nTimeOut = (nTimeOut > 1000) ? 1000 : nTimeOut;
					nTimeOut = (nTimeOut < 0) ? 1000 : nTimeOut;
				}
			}
			timeout= nTimeOut;

			//HLIB_CMD_Printf("new time out : [%d]\n", timeout);
		}
#if 0
		/*
			Timer 처리를 우선적으로 처리한다.
		*/
		err = ERR_OK;
		while (err == ERR_OK)
		{
			HUINT32			 ulTimerId;
			BUS_Callback_t	 pfAppCallback = NULL;
			BUS_Mgr_t		 pstApp;

			err = bus_timer_CheckTimerOut (&pfAppCallback, &ulTimerId);
			if (err == ERR_OK)
			{
				pstApp = BUS_MGR_Get (pfAppCallback);
				if (pstApp != NULL)
				{
					bus_mgr_Call (pstApp, eMEVT_BUS_TIMER, 0, ulTimerId, 0, 0);
				}
			}
		}

		err = VK_MSG_ReceiveTimeout (s_ulBusMainMsgQId, &msg, sizeof(BUS_InternalMsg_t), timeout);
		if(err == VK_OK)
		{ /* Priority #1 : Message processing */
			HxLOG_Info("[BUS_InternalMsg_t] msg=%08X, p=(%08X,%08X,%08X) to %s\n",
						msg.message, msg.param1, msg.param2, msg.param3, BUS_MGR_GetMgrName(BUS_MGR_Get(msg.appCallback)));

			eRes = bus_main_ProcessMessage(&msg);


			/*
			 *	fnFreeParam 이 유효 하면 param 은 alloc된 값으로 간주하고 free을 진행 한다.
			 */
			if( NULL != msg.fnFreeParam )
			{
				msg.fnFreeParam( (void*)msg.param3 );
			}

			if (eRes != ERR_BUS_NO_ERROR)
			{
				HxLOG_Error("quit gwm task for bus_main_ProcessMessage()\n");
				break;
			}

		}
		nDispatched = OBAMA_RpcProtocol_ReadAndDispatch(1);
		if(err == VK_TIMEOUT && nDispatched == 0)
			timeout = 10;
		else
			timeout = 0;
#endif
	}

	HxLOG_Info("\n\nGWM QUITS !\n\n\n");

	return;
}

#if defined(CONFIG_DEBUG)
static HERROR bus_cmd_GetArgValue (HCHAR *szString, HUINT32 *pulValue)
{
	HUINT32			 ulValue;
	HUINT32			 ulIndex, ulLength;
	HBOOL			 bValid;

	if (szString == NULL || pulValue == NULL)				{ return HxCMD_ERR; }

	ulLength = strlen (szString);

	// 1. Check whether the string is the decimal value.
	bValid = (ulLength > 0) ? TRUE : FALSE;
	ulValue = 0;
	for (ulIndex = 0; ulIndex < ulLength; ulIndex++)
	{
		if (szString[ulIndex] >= '0' && szString[ulIndex] <= '9')
		{
			ulValue *= 10;
			ulValue += (szString[ulIndex] - '0');
		}
		else
		{
			bValid = FALSE;
			break;
		}
	}

	if (bValid == TRUE)
	{
		*pulValue = ulValue;
		return HxCMD_OK;
	}

	// 2. Check whether the string is the hexadecimal value.
	bValid = FALSE;
	if ((ulLength > 2) && (szString[0] == '0') && (szString[1] == 'X'))
	{
		bValid = TRUE;
		ulValue = 0;
		for (ulIndex = 2; ulIndex < ulLength; ulIndex++)
		{
			if (szString[ulIndex] >= '0' && szString[ulIndex] <= '9')
			{
				ulValue *= 16;
				ulValue += (szString[ulIndex] - '0');
			}
			else if (szString[ulIndex] >= 'A' && szString[ulIndex] <= 'F')
			{
				ulValue *= 16;
				ulValue += (szString[ulIndex] - 'A' + 10);
			}
			else
			{
				bValid = FALSE;
				break;
			}
		}
	}

	if (bValid == TRUE)
	{
		*pulValue = ulValue;
		return HxCMD_OK;
	}

	return HxCMD_ERR;


}

static int	bus_cmd_SetTraceMsg (void *szArgList)
{
	HCHAR	*aucArg=NULL;
	HCHAR	aucStr[20], aucArg2[20];
	HUINT32	ulValue;
	HBOOL	bTraceAll;
	HERROR	hErr;

    /********************
     * get 1st argument *
     ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%s", aucStr) == 0 ) return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

    /********************
     * get 2nd argument *
     ********************/
	aucArg2[0] = '\0';
	aucArg = HLIB_CMD_GetNextArg( (HCHAR**)&szArgList );
	if( aucArg != NULL )
	{
		if( sscanf(aucArg, "%s", aucArg2) == 0 )
		{
			aucArg2[0] = '\0';
		}
	}

	HLIB_STD_StrUpper(aucArg2);

	hErr = bus_cmd_GetArgValue (aucStr, &ulValue);
	if (hErr != HxCMD_OK) 			{ return HxCMD_ERR; }

	if (strcmp (aucArg2, "ALL") == 0)
	{
		bTraceAll = TRUE;
	}
	else
	{
		bTraceAll = FALSE;
	}

	hErr = BUS_DBG_RegisterMsgBreakTracer (ulValue, bTraceAll);
	if (hErr != ERR_OK)
	{
		HLIB_CMD_Printf("[ERROR] BUS_DBG_RegisterMsgBreakTracer %08x\n", hErr);
	}

	return HxCMD_OK;
}

static int	bus_cmd_ResetTraceMsg (void *szArgList)
{
	HCHAR	 *aucArg=NULL;
	HCHAR	 aucStr[20];
	HUINT32			ulValue;
	HERROR		hErr;

    /********************
     * get 1st argument *
     ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%s", aucStr) == 0 ) return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

#if 0
	/********************
     * get 2nd argument *
     ********************/
	aucArg = CMD_NextArg( (HCHAR**)&szArgList );
	if( aucArg != NULL ) {
		if( sscanf(aucArg, "%s", aucArg2) == 0 ) return HxCMD_ERR;
	}

	HLIB_STD_StrUpper(aucArg2);
#endif

	hErr = bus_cmd_GetArgValue (aucStr, &ulValue);
	if (hErr != HxCMD_OK) 			{ return HxCMD_ERR; }

	hErr = BUS_DBG_UnregisterMsgBreakTracer (ulValue);
	if (hErr != ERR_OK)
	{
		HLIB_CMD_Printf ("[ERROR] BUS_DBG_UnregisterMsgBreakTracer %08x\n", hErr);
	}

	return HxCMD_OK;
}

static int	bus_cmd_PrintTraceMsgList (void *szArgList)
{
    /********************
     * get 1st argument *
     ********************/
#if 0
	aucArg = CMD_NextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%s", aucStr) == 0 ) return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);
#endif

	BUS_DBG_DumpMsgBreakTracer();

	return HxCMD_OK;
}

static int	bus_cmd_PrintProc (void *szArgList)
{
	BUS_DBG_DumpMgrList (szArgList);
	return HxCMD_OK;
}

static int	bus_cmd_PrintStuckProc (void *szArgList)
{
	BUS_DBG_ShowLastMgr (szArgList);
	return HxCMD_OK;
}

static int	bus_cmd_PrintTimerList (void *szArgList)
{
	BUS_DBG_ShowTimer (szArgList);
	return HxCMD_OK;
}

static int	bus_cmd_SetDestroyLog (void *szArgList)
{
	HCHAR	*aucArg=NULL;
	HCHAR	aucStr[20];
	HUINT32	ulValue;
	HERROR	hErr;

    /********************
     * get 1st argument *
     ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL ) return HxCMD_ERR;
	if( sscanf(aucArg, "%s", aucStr) == 0 ) return HxCMD_ERR;

	HLIB_STD_StrUpper((HCHAR*)aucStr);


	hErr = bus_cmd_GetArgValue (aucStr, &ulValue);
	if (hErr != HxCMD_OK) 			{ return HxCMD_ERR; }

	hErr = BUS_DBG_SetDestroyLog ((HBOOL)ulValue);
	if (hErr != ERR_OK)
	{
		HLIB_CMD_Printf("[ERROR] BUS_DBG_SetDestroyLog %08x\n", hErr);
	}

	return HxCMD_OK;
}

/**********************************************
	Command 수행 함수를 등록하는 통합 등록 함수.
***********************************************/
#define	hCmdHandle			"bus"

void bus_main_RegisterCommand(void)
{
	HLIB_CMD_RegisterWord(hCmdHandle,
				bus_cmd_SetTraceMsg,
				"settracemsg",						/* keyword */
				"Set the message value to trace the proc traverse",			/*	 help  */
				"settracemsg [MSG_VALUE] (ALL)");	/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,
				bus_cmd_ResetTraceMsg,
				"resettracemsg",					/* keyword */
				"Reset the trace message by trace list index",			/*	 help  */
				"resettracemsg [trace message index]");	/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,
				bus_cmd_PrintTraceMsgList,
				"printtracelist",						/* keyword */
				"Print the list of trace message",			/*	 help  */
				"printtracelist");					/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,
				bus_cmd_PrintProc,
				"printproc",
				"Print current active proc on the manager bus",
				"printproc");

	HLIB_CMD_RegisterWord(hCmdHandle,
				bus_cmd_PrintStuckProc,
				"printstuckproc",
				"Print stuck proc",
				"printstuckproc");

	HLIB_CMD_RegisterWord(hCmdHandle,
				bus_cmd_PrintTimerList,
				"printtimerlist",
				"Print timer list",
				"printtimerlist");

	HLIB_CMD_RegisterWord(hCmdHandle,
				bus_cmd_SetDestroyLog,
				"log_destroy",
				"log_destroy [0/1]",
				"log_destroy [0/1]");
}
#endif

void		BUS_Init(void)
{
	HINT32		err;

	/* init application object */
	bus_mgr_Init();

	/* init application link */
	g_stMgrRoot = g_stCurrentMgr = NULL;

	/* create & init timer task */
	bus_timer_Init();

	/* create & init message que */
	err = VK_MSG_Create(GWM_MSGQ_SIZE, sizeof(BUS_InternalMsg_t), BUS_MSG_QUE_NAME, &s_ulBusMainMsgQId, VK_SUSPENDTYPE_FIFO);
	if (err != VK_OK)
	{
		HxLOG_Error("GWM msgQ creation fail err = 0x%x\n", err);
		return;
	}

	/* create and start GWM task */
	err = VK_TASK_Create(bus_main_Task, GWM_TASK_PRIORITY, GWM_TASK_STACK_SIZE,
						BUS_MAIN_TASK_NAME, NULL, &s_ulBusMainTaskId, 0);
	if (err != VK_OK)
	{
		HxLOG_Error("GWM task creation fail err = 0x%x\n", err);
		return;
	}

#if defined(CONFIG_DEBUG)
	bus_main_RegisterCommand();
#endif
}

void 		BUS_Start(BUS_Callback_t pfnAppIgnitor)
{
//	HINT32		err;

	CHECK_BOOT_TIME("BUS_Start - Start");

	if(pfnAppIgnitor == NULL)
	{
		HxLOG_Print("\t\tGWM start with no start procedure.\n\n");
	}

	s_pfnAppIgnitor = pfnAppIgnitor;

	if (pfnAppIgnitor != NULL)
	{
		bus_mgr_GetReqMgrSem();
	}

	VK_TASK_Start(s_ulBusMainTaskId);

	if (pfnAppIgnitor != NULL)
	{
		bus_mgr_GetReqMgrSem();
		bus_mgr_ReleaseReqMgrSem();
	}

	CHECK_BOOT_TIME("BUS_Start - End");

}

void		BUS_PostRPCReady(void)
{
	BUS_InternalMsg_t	msg;
	HINT32				err;

	// GWM task 내부에서 동작하는 application으로 부터 GWM 으로 메세지를
	// 전송하는 함수. 스스로 dead lock 걸리지 않으려면 timeout으로 동작해야 한다.
	msg.appCallback = NULL;
	msg.message = eMEVT_BUS_RPC_READY;
	msg.handle = 0;
	msg.param1 = 0;
	msg.param2 = 0;
	msg.param3 = 0;

 	/*	N/A */
	msg.fnFreeParam = NULL;

	err = VK_MSG_SendTimeout(s_ulBusMainMsgQId, &msg, sizeof(BUS_InternalMsg_t), 0);
	if (err != VK_OK)
	{
		HxLOG_Error("VK_MSG_SendTimeout failed. s_ulBusMainMsgQId(%d) \n",s_ulBusMainMsgQId);
	}
}

BUS_Result_t BUS_SendMessage(BUS_Callback_t appCallback, HINT32 message,
						Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3)
{
	BUS_Mgr_t			traverse, nextApp;
	BUS_Result_t		eRes = ERR_BUS_NO_ERROR;

#ifdef CONFIG_DEBUG
{
	HUINT32				ulCurrentTid = HLIB_STD_GetTID();
	if(s_ulBusMainThreadId != ulCurrentTid)
	{
		HxLOG_Error("\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		HxLOG_Error("ERROR!! WRONG USAGE OF BUS_SendMessage, BusMainTid = %d, CurrentTid = %d\n", s_ulBusMainThreadId, ulCurrentTid);
		HxLOG_Error("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n");
		HxLOG_Assert(0);
	}
}
#endif

	if (appCallback == NULL)
	{	/* broadcast the message */
		HxLOG_Info("\t\tStart of Sended Message Process(%X)\n", message);

		traverse = g_stMgrRoot;
		while (traverse != NULL)
		{
			nextApp = bus_mgr_GetNext(traverse);			// 중요. !!;
					/* 다음에 처리될 Application을 백업해 두어야만 한다. */
					/* 그렇지 않으면, GWM_DestroyApplication에 의해서 Linked List가 변경된 경우에, */
					/* MESSAGE_NOTUSED 로 처리되는 메세지를 놓쳐 버릴 수가 있다. */
					/* -- 다시 잘 생각해 보자, 이걸로 확실히 메세지를 빼먹지 않고 모든 Appl 에 전달할 수 있을까.? 특히나 메세지 처리 도중에 Destroy 가 되는 app 가 있다면 ?? */
			HxLOG_Info("\t\tMSG:%08X >>> %s, \n", message, BUS_MGR_GetMgrName(nextApp));
			eRes = bus_mgr_Call(traverse, message, hAct, param1, param2, param3);
			if (eRes == MESSAGE_BREAK)
				break;
			traverse = nextApp;
		}
		HxLOG_Info("\t\tEnd of Sended Message Process\n");
		return eRes;
	}
	else
	{	/* transfer the message directly */
		return bus_mgr_Call(BUS_MGR_Get(appCallback), message, hAct, param1, param2, param3);
	}
}


BUS_Result_t BUS_PostMessage(BUS_Callback_t appCallback, HINT32 message,
						Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3)
{
	BUS_InternalMsg_t	msg;
	HINT32				err;

	// GWM task 내부에서 동작하는 application으로 부터 GWM 으로 메세지를
	// 전송하는 함수. 스스로 dead lock 걸리지 않으려면 timeout으로 동작해야 한다.
	msg.appCallback = appCallback;
	msg.message = message;
	msg.handle = hAct;
	msg.param1 = param1;
	msg.param2 = param2;
	msg.param3 = param3;

 	/*	N/A */
	msg.fnFreeParam = NULL;

	err = VK_MSG_SendTimeout(s_ulBusMainMsgQId, &msg, sizeof(BUS_InternalMsg_t), 0);
//	HxLOG_Assert(err == VK_OK);

	return	err;
}

BUS_Result_t BUS_SendData(BUS_Callback_t appCallback, HINT32 message,
						Handle_t hAct, HINT32 param1, HINT32 param2, void *pData, HUINT32 ulSize)
{
	return BUS_SendMessage(appCallback, message, hAct, param1, param2, (HINT32)pData);
}

// Bus안에서 pData가 이미 복사 되어 있으므로, PostData를 부르는 측에서 free를 바로 해준다.
BUS_Result_t BUS_PostData(BUS_Callback_t appCallback, HINT32 message,
						Handle_t hAct, HINT32 param1, HINT32 param2, void *pData, HUINT32 ulSize)
{
	void *pBuf = NULL;
	BUS_InternalMsg_t	msg;
	HINT32				err;

	if(ulSize > 0)
	{
		pBuf = (void *)HLIB_STD_MemCalloc(ulSize);
		if(pBuf == NULL)
		{
			HxLOG_Error("Alloc fail! \n");
			return ERR_BUS_OUT_OF_MEMORY;
		}

		HLIB_STD_MemCpySafe(pBuf, ulSize, pData, ulSize);

	}

	msg.appCallback = appCallback;
	msg.message = message;
	msg.handle = hAct;
	msg.param1 = param1;
	msg.param2 = param2;
	msg.param3 = (HINT32)pBuf;

	/*	N/A */
	msg.fnFreeParam = HLIB_STD_MemFree_CB;

	err = VK_MSG_SendTimeout(s_ulBusMainMsgQId, &msg, sizeof(BUS_InternalMsg_t), 0);
//	HxLOG_Assert(err == VK_OK);

	return	err;
}


BUS_Result_t		BUS_ProcessMessageDefault(HINT32 message, Handle_t hAct, HINT32 param1, HINT32 param2, HINT32 param3)
{
	// 현재는 사용되지 않는 Parameter 들.. 다른 메세지에서 사용할 수도 있을 것이다.
	// Warning 메세지를 없애기 위해서,
	param2 = param2;
	param3 = param3;

	switch (message)
	{
		/* Standby로 진입한다는 통보. 각 app에서 가로채서 처리하지 않으면 이 함수가 불리고
		결국은 아래와 같이 자살하는 동작을 수행한다. */
		case eMEVT_BUS_READY_SHUTDOWN:
			HxLOG_Print("\t************** Destroy [%s] application by eMEVT_BUS_GO_SHUTDOWN\n", BUS_MGR_GetMgrName(g_stCurrentMgr));
			BUS_MGR_Destroy(0);
			return MESSAGE_PASS;

		default:
			break;
	}
	return MESSAGE_PASS;
}

typedef	struct tagMessageName
{
	HINT32	message;
	HINT8	szMsgName[64];
} GwmMsgName;

GwmMsgName g_szMsgName[(eMEVT_BUS_END - eMEVT_BUS_START)+1] =
	{
		{eMEVT_BUS_CREATE, 		VALUE_TO_STR(eMEVT_BUS_CREATE)},
		{eMEVT_BUS_DESTROY, 		VALUE_TO_STR(eMEVT_BUS_DESTROY)},
		{eMEVT_BUS_MGR_SPAWNED,	VALUE_TO_STR(eMEVT_BUS_MGR_SPAWNED)},
		{eMEVT_BUS_MGR_KILLED,	VALUE_TO_STR(eMEVT_BUS_MGR_KILLED)},
		{eMEVT_BUS_TIMER,			VALUE_TO_STR(eMEVT_BUS_TIMER)},
		{eMEVT_BUS_GO_SHUTDOWN,	VALUE_TO_STR(eMEVT_BUS_READY_SHUTDOWN)},
		{eMEVT_BUS_GO_SHUTDOWN,	VALUE_TO_STR(eMEVT_BUS_GO_SHUTDOWN)},
		{eMEVT_BUS_GO_OPERATION,	VALUE_TO_STR(eMEVT_BUS_GO_OPERATION)},
	};

HINT8*	BUS_DBG_GetMsgName(HINT32 message)
{
	HINT32	nInternalMsg = message - eMEVT_BUS_START;

	if (nInternalMsg > (eMEVT_BUS_END - eMEVT_BUS_START))
		return NULL;

	return g_szMsgName[nInternalMsg].szMsgName;
}


/* End of File. ---------------------------------------------------------- */

