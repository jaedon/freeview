/**************************************************************
 *	@file		PowerManager.c
 *	Power Manager Application for OCTO 2.0
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2012/03/07
 *	@author			humax
 *	@brief			Power Manager Application
 **************************************************************/

/*
	마지막 POWER관련 관리, 즉 Shutdown관리 대상이 OCTO라는 가정하에
	해당 코드를 재 구성한다. (2012/07/08)
*/

#include	<stdio.h>
#include	<stdlib.h>
#include	<sys/time.h>
#include	<sys/resource.h>

#include	<vkernel.h>
#include	<hlib.h>
//#include	<oapi.h>
#include	<hapi.h>
//#include	<apk.h>
#include	<dlib.h>
#include	<directfb.h>
#include	<dapi.h>
#include 	"pama_int.h"

#if defined(CONFIG_HUMAX_CRASHREPORT)
#include "hcrlib.h"
#endif

#define	PWR_LOG(a)			HxLOG_Print("\033[30;44m[%s](%d) %s\033[0m\n", a)
#ifdef CONFIG_DEBUG
//#define PAMA_BUSTIME_PRINT
#endif
/**************************************************************
	internal structures and variables
 **************************************************************/

typedef struct
{
	HINT32			server;
	HUINT32			ulSemId;
	HINT32			nRpcHandle;
	HUINT32 		ulMainMsgQueue;
	HUINT32 		ulRequestShutdownTime;
	HUINT32 		ulWaitThreshold;
	HxList_t		*SyncWaitingList;
	HBOOL			bWaitShutDownFlag;
	HBOOL			bWaitRebootFlag;
	HBOOL			bMainAppReady;
	HBOOL			bSyncReady;
	PxSTATUS_e		eCurrentMode;
	PxSTATUS_e		eRequestMode;
	HSystemOperationMode_e	eFirstBootType;
} pama_t;


static pama_t*	pama_getInstance(void);
static void	pama_releaseInstance(pama_t *pama);

#define	MSG2STR(eMsg)		(\
								  ((eMsg) == ePAMA_MSG_REQUEST_OPERATION) 	? DxNAMEOF(ePAMA_MSG_REQUEST_OPERATION)		\
								: ((eMsg) == ePAMA_MSG_REQUEST_SHUTDOWN) 	? DxNAMEOF(ePAMA_MSG_REQUEST_SHUTDOWN)		\
								: ((eMsg) == ePAMA_MSG_REQUEST_REBOOT) 		? DxNAMEOF(ePAMA_MSG_REQUEST_REBOOT)		\
								: ((eMsg) == ePAMA_MSG_WAIT_SHUTDOWN) 		? DxNAMEOF(ePAMA_MSG_WAIT_SHUTDOWN)		\
								: ((eMsg) == ePAMA_MSG_WAIT_REBOOT) 		? DxNAMEOF(ePAMA_MSG_WAIT_REBOOT)		\
								: ((eMsg) == ePAMA_MSG_WAIT_SYNC) 		? DxNAMEOF(ePAMA_MSG_WAIT_SYNC)		\
								: ((eMsg) == ePAMA_MSG_SYNC_DONE) 		? DxNAMEOF(ePAMA_MSG_SYNC_DONE)		\
								: ((eMsg) == ePAMA_MSG_SET_POWERSAVING) 	? DxNAMEOF(ePAMA_MSG_SET_POWERSAVING)		\
								: ((eMsg) == ePAMA_MSG_SET_SLEEPMODE) 		? DxNAMEOF(ePAMA_MSG_SET_SLEEPMODE)		\
								: ((eMsg) == ePAMA_MSG_SET_BOOTMODE) 		? DxNAMEOF(ePAMA_MSG_SET_BOOTMODE)		\
								: ((eMsg) == ePAMA_MSG_SET_WIFIPOWER) 		? DxNAMEOF(ePAMA_MSG_SET_WIFIPOWER)		\
								: ((eMsg) == ePAMA_MSG_UNKNOWN) 			? DxNAMEOF(ePAMA_MSG_UNKNOWN)		\
								: NULL)
#define	STATUS2STR(eStatus)	(\
								  ((eStatus) == ePAMA_STATUS_Undefined) 	? DxNAMEOF(ePAMA_STATUS_Undefined)	\
								: ((eStatus) == ePAMA_STATUS_Operation) 	? DxNAMEOF(ePAMA_STATUS_Operation)	\
								: ((eStatus) == ePAMA_STATUS_Reboot) 		? DxNAMEOF(ePAMA_STATUS_Reboot)	\
								: ((eStatus) == ePAMA_STATUS_Shutdown) 		? DxNAMEOF(ePAMA_STATUS_Shutdown)	\
								: ((eStatus) == ePAMA_STATUS_Sync) 		? DxNAMEOF(ePAMA_STATUS_Sync)	\
								: ((eStatus) == ePAMA_STATUS_RebootDone) 		? DxNAMEOF(ePAMA_STATUS_RebootDone)	\
								: ((eStatus) == ePAMA_STATUS_ShutdownDone) 		? DxNAMEOF(ePAMA_STATUS_ShutdownDone)	\
								: ((eStatus) == ePAMA_STATUS_Unknown) 		? DxNAMEOF(ePAMA_STATUS_Unknown)	\
								: NULL)


#define	MSGEXT2MSGINT(eMsg)		(\
								  ((eMsg) == ePAMA_CMD_RequestOperation) 		? (ePAMA_MSG_REQUEST_OPERATION)		\
								: ((eMsg) == ePAMA_CMD_RequestShutDown) 		? (ePAMA_MSG_REQUEST_SHUTDOWN)		\
								: ((eMsg) == ePAMA_CMD_RequestReboot) 			? (ePAMA_MSG_REQUEST_REBOOT)		\
								: ((eMsg) == ePAMA_CMD_WaitShutDown) 			? (ePAMA_MSG_WAIT_SHUTDOWN)		\
								: ((eMsg) == ePAMA_CMD_WaitSync) 			? (ePAMA_MSG_WAIT_SYNC)		\
								: ((eMsg) == ePAMA_CMD_SyncDone)			? (ePAMA_MSG_SYNC_DONE) 	\
								: ((eMsg) == ePAMA_CMD_WaitReboot) 				? (ePAMA_MSG_WAIT_REBOOT)		\
								: ((eMsg) == ePAMA_CMD_SetPowerSavingOnStandby) ? (ePAMA_MSG_SET_POWERSAVING)		\
								: ((eMsg) == ePAMA_CMD_SetSleepModeOnStandby) 	? (ePAMA_MSG_SET_SLEEPMODE)		\
								: ((eMsg) == ePAMA_CMD_SetBootModeOnStandby) 	? (ePAMA_MSG_SET_BOOTMODE)		\
								: ((eMsg) == ePAMA_CMD_GetWifiPowerOnStandby)	? (ePAMA_MSG_SET_WIFIPOWER)		\
								: ePAMA_MSG_UNKNOWN)


typedef enum
{
	ePAMA_LASTMODE_OPERATION,
	ePAMA_LASTMODE_SHUTDOWN,
	ePAMA_LASTMODE_UNKNOWN
} PAMA_LASTMODE_e;

typedef enum
{
	ePAMA_MSG_REQUEST_OPERATION,
	ePAMA_MSG_REQUEST_SHUTDOWN,
	ePAMA_MSG_REQUEST_REBOOT,
	ePAMA_MSG_WAIT_SHUTDOWN,
	ePAMA_MSG_WAIT_REBOOT,
	ePAMA_MSG_WAIT_SYNC,
	ePAMA_MSG_SYNC_DONE,
	ePAMA_MSG_SET_POWERSAVING,
	ePAMA_MSG_SET_SLEEPMODE,
	ePAMA_MSG_SET_BOOTMODE,
	ePAMA_MSG_SET_WIFIPOWER,

	ePAMA_MSG_UNKNOWN
} PAMA_MSG_e;

typedef struct
{
	PAMA_MSG_e	eMsg;
	HUINT32		ulParam1;
	HCHAR		*pszProcessName;
} PAMA_MSG_t;

#if defined(CONFIG_DEBUG)
#include	"pama_debug.c"
#endif


#define	PAMA_MSGQ_SIZE						64
#define	PAMA_WAITING_DURATION				6000
#define 	PAMA_WAITING_RCU_DURATION			1500
#define 	PAMA_WAITING_SYNC_DURATION		2000


/**************************************************************
	global functions
 **************************************************************/

/**************************************************************
	private functions
 **************************************************************/
static void		pama_logcapture_write(HCHAR *pszString)
{
	HLIB_CMD_CaptureLog("%s", pszString);
}

static HBOOL		pama_syncwatinglist_compare(void *user_data,void *list_data)
{
	return (HLIB_STD_StrCaseCmp((HCHAR *)user_data, (HCHAR *)list_data) == 0);
}

#define	________PAMA_HAPI_SIGNAL_HANDLER_______________

static HERROR	pama_system_onBootupStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	HSystemBootupStatus_t	*pstBootupStatus;
	pama_t		*pama = pama_getInstance();

	HxLOG_Assert(nArgc == 1);
	pstBootupStatus = HxOBJECT_BIN_DATA(apArgv[0]);


	if(pama->eFirstBootType == eHSystemOperationMode_UNKNOWN)
	{
		PAMA_MSG_t		msg;
		pama->eFirstBootType = pstBootupStatus ? pstBootupStatus->eSystemCurrentMode:eHSystemOperationMode_UNKNOWN;

		if(pama->eFirstBootType == eHSystemOperationMode_NORMAL)
		{
			msg.eMsg = ePAMA_MSG_REQUEST_OPERATION;
			msg.pszProcessName = HLIB_STD_StrDup(HLIB_STD_GetCurProcessName());
			HLIB_MSG_Send(pama->ulMainMsgQueue, &msg, sizeof(PAMA_MSG_t));
		}
		else if(pama->eFirstBootType == eHSystemOperationMode_SLEEP)
		{
			 if(pama->bMainAppReady == TRUE)
			{
				msg.eMsg = ePAMA_MSG_REQUEST_SHUTDOWN;
				msg.pszProcessName = HLIB_STD_StrDup(HLIB_STD_GetCurProcessName());
				HLIB_MSG_Send(pama->ulMainMsgQueue, &msg, sizeof(PAMA_MSG_t));
			}
		}
	}

	pama_releaseInstance(pama);

	return ERR_OK;
}


static HERROR	pama_system_onMainApplicationReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUser)
{
	pama_t		*pama = pama_getInstance();
	PAMA_MSG_t		msg;

	pama->bMainAppReady = TRUE;

	HAPI_SetRegistryInt(PxDB_MAINAPP_READY, pama->bMainAppReady);

	if(pama->eFirstBootType == eHSystemOperationMode_SLEEP)
	{
		msg.eMsg = ePAMA_MSG_REQUEST_SHUTDOWN;
		msg.pszProcessName = HLIB_STD_StrDup(HLIB_STD_GetCurProcessName());
		HLIB_MSG_Send(pama->ulMainMsgQueue, &msg, sizeof(PAMA_MSG_t));
	}
	pama_releaseInstance(pama);

	return ERR_OK;
}

#define	________PAMA_SVR_SIGNAL_HANDLER_______________


#define		_________PAMA_MAIN_TASK_BEGIN________
static HERROR	pama_set_cmd(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	pama_t	*pama = (pama_t*)pvUserData;
	PAMA_MSG_t		msg;
	HxLOG_Trace();
	HxLOG_Assert(pama);

	HxSTD_MemSet(&msg, 0, sizeof(PAMA_MSG_t));
	msg.eMsg = MSGEXT2MSGINT(HxOBJECT_INT(apArgv[0]));
	msg.ulParam1 =	 HxOBJECT_INT(apArgv[1]);
 	msg.pszProcessName = HLIB_STD_StrDup(HxOBJECT_STR(apArgv[2]));
 	HxLOG_Error("PROC[%s] msg[%s]\n", msg.pszProcessName, MSG2STR(msg.eMsg));
	HLIB_MSG_Send(pama->ulMainMsgQueue, (void*)&msg, sizeof(PAMA_MSG_t));
	return ERR_OK;

}

static void		pama_server_on_status_change(void *userData, HxSocket_Status_e eStatus, HINT32 clientHandle)
{
	pama_t	*pama = (pama_t*)userData;

	if (eStatus == HxSOCKET_REMOVE_CLIENT)
		HLIB_RPC_RemoveClientHandle(pama->nRpcHandle, clientHandle);
}
static	void	pama_server_PacketReceived(void *userData, HINT32 socketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	pama_t	*pama = (pama_t*)userData;
	HLIB_RPC_ProcessRPC(pama->nRpcHandle, size, pvData, requestId, socketHandle);
}

static void	pama_server_listener (void *arg)
{
	pama_t	*pama = (pama_t*)arg;

	HLIB_SOCKET_Listen(pama->server, TRUE, pama_server_PacketReceived, pama_server_on_status_change, arg);
}

static HBOOL	pama_is_supported_powerkey(HINT32 key_symbol)
{
	HBOOL isKey = FALSE;
	switch(key_symbol)
	{
		case DIKS_CHANNEL_UP:		case DIKS_CHANNEL_DOWN:
		case DIKS_0:		case DIKS_1:		case DIKS_2:		case DIKS_3:
		case DIKS_4:		case DIKS_5:		case DIKS_6:		case DIKS_7:
		case DIKS_8:		case DIKS_9:		case DIKS_POWER:
			isKey = TRUE;
		default:
			break;
	}
	return isKey;
}


static  void pama_do_reboot(void)
{
	HxLOG_Trace();
	HxLOG_Warning("PAMA BroadcastSignal ePAMA_STATUS_RebootDone\n");
	HAPI_BroadcastSignal("signal:onSystemStateChanged", 1, "i", ePAMA_STATUS_RebootDone);
	// Add Reboot DI Function
#if !defined(WIN32)
	SEPAPI_Reboot(1);
#endif
}

static  void pama_do_shutdown(pama_t *pama)
{
	HxLOG_Trace();
	HxLOG_Warning("PAMA BroadcastSignal ePAMA_STATUS_ShutdownDone\n");
	HAPI_BroadcastSignal("signal:onSystemStateChanged", 1, "i", ePAMA_STATUS_ShutdownDone);
	pama->eFirstBootType = eHSystemOperationMode_UNKNOWN;
	HLIB_STD_TaskSleep(1000);
	// Add Shutdown DI Function
}

static void pama_print_bustime(void)
{
#ifdef PAMA_BUSTIME_PRINT
	system("cat /tmp/obama/BUSTIME");
#endif
}

static void		pama_system_main(void *arg)
{
	HUINT32		ulMsgQueueId = (HUINT32)arg;
	pama_t	*pama;
	PAMA_MSG_t	msg;

	//pama_logcapture_write("Process Launched");
	while (1)
	{
		if (HLIB_MSG_ReceiveTimeout(ulMsgQueueId, &msg, sizeof(PAMA_MSG_t), 400) == ERR_OK)
		{
			pama = pama_getInstance();
			HxLOG_Warning("PAMA_MAIN[%d] - Msg Received : [%s] time[%d] From [%s]\n", __LINE__, MSG2STR(msg.eMsg), msg.ulParam1, msg.pszProcessName);
			HxLOG_Warning("====> Main App is %s Ready\n", pama->bMainAppReady?"":"Not");
			HxLOG_Warning("\t\tCUR STATUS[%s] REQ STATUS[%s] \n\t\tShutDown Request [%s] Reboot Request[%s] \t\tTime[%d] Threshold[%d]\n"
				, STATUS2STR(pama->eCurrentMode), STATUS2STR(pama->eRequestMode), pama->bWaitShutDownFlag?"YES":"NO", pama->bWaitRebootFlag?"YES":"NO"
				, HLIB_STD_GetSystemTick(), pama->ulWaitThreshold);

			switch (msg.eMsg)
			{
				case ePAMA_MSG_REQUEST_OPERATION :
					if(pama->eRequestMode != ePAMA_STATUS_Reboot
						&& pama->eRequestMode != ePAMA_STATUS_Operation)
					{
						pama->eRequestMode = ePAMA_STATUS_Operation;
						HAPI_SetRegistryInt(PxDB_LAST_STANDBY, FALSE);
						HxLOG_Warning("PAMA BroadcastSignal ePAMA_STATUS_Operation\n");
						HAPI_BroadcastSignal("signal:onSystemStateChanged", 1, "i", ePAMA_STATUS_Operation);
					}
					break;

				case ePAMA_MSG_REQUEST_SHUTDOWN :
					if(pama->eRequestMode != ePAMA_STATUS_Reboot)
					{
						if(pama->eCurrentMode == ePAMA_STATUS_ShutdownDone) pama->eCurrentMode	= ePAMA_STATUS_Operation;	// For S3 // shutdown done 이 된 상태이므로 shutdown으로 진행이 되지 않는다.
						pama->eRequestMode = ePAMA_STATUS_Shutdown;
						pama->ulWaitThreshold = HLIB_STD_GetSystemTick() + PAMA_WAITING_DURATION;
						pama->ulRequestShutdownTime = HLIB_STD_GetSystemTick();
						HAPI_SetRegistryInt(PxDB_LAST_STANDBY, TRUE);
						HxLOG_Warning("PAMA BroadcastSignal ePAMA_STATUS_Shutdown\n");
						HAPI_BroadcastSignal("signal:onSystemStateChanged", 1, "i", ePAMA_STATUS_Shutdown);
					}
					break;

				case ePAMA_MSG_REQUEST_REBOOT :
					pama->eRequestMode = ePAMA_STATUS_Reboot;
					pama->ulWaitThreshold = HLIB_STD_GetSystemTick() + PAMA_WAITING_DURATION;
					HxLOG_Warning("PAMA BroadcastSignal ePAMA_STATUS_Reboot\n");
					HAPI_BroadcastSignal("signal:onSystemStateChanged", 1, "i", ePAMA_STATUS_Reboot);
					break;

				case ePAMA_MSG_WAIT_SHUTDOWN :
					if(pama->eRequestMode == ePAMA_STATUS_Shutdown)
					{
						pama->bWaitShutDownFlag = TRUE;
						if(pama->ulWaitThreshold < (HLIB_STD_GetSystemTick() + msg.ulParam1))
						{
							pama->ulWaitThreshold = HLIB_STD_GetSystemTick() + msg.ulParam1;
						}
						HxLOG_Warning("ePAMA_MSG_WAIT_SHUTDOWN [%d] [%d] [%d]\n", pama->ulWaitThreshold, msg.ulParam1, HLIB_STD_GetSystemTick());
					}
					break;

				case ePAMA_MSG_WAIT_REBOOT :
					if(pama->eRequestMode == ePAMA_STATUS_Reboot)
					{
						pama->bWaitRebootFlag = TRUE;
						if(pama->ulWaitThreshold < (HLIB_STD_GetSystemTick() + msg.ulParam1))
						{
							pama->ulWaitThreshold = HLIB_STD_GetSystemTick() + msg.ulParam1;
						}
						HxLOG_Warning("ePAMA_MSG_WAIT_REBOOT [%d] [%d] [%d]\n", pama->ulWaitThreshold, msg.ulParam1, HLIB_STD_GetSystemTick());
					}
					break;
				case ePAMA_MSG_WAIT_SYNC :
					if(pama->eRequestMode == ePAMA_STATUS_Reboot || pama->eRequestMode == ePAMA_STATUS_Shutdown  )
					{
						pama->bSyncReady = FALSE;
						pama->SyncWaitingList = HLIB_LIST_Append(pama->SyncWaitingList, HLIB_STD_StrDup(msg.pszProcessName));
						if(pama->ulWaitThreshold < (HLIB_STD_GetSystemTick() + msg.ulParam1))
						{
							pama->ulWaitThreshold = HLIB_STD_GetSystemTick() + msg.ulParam1;
						}
						HxLOG_Warning("ePAMA_MSG_WAIT_SYNC Thred[%d] Wait[%d] Cur[%d]\n", pama->ulWaitThreshold, msg.ulParam1, HLIB_STD_GetSystemTick());
					}
					break;
				case ePAMA_MSG_SYNC_DONE:
					if(pama->eRequestMode == ePAMA_STATUS_Reboot || pama->eRequestMode == ePAMA_STATUS_Shutdown  )
					{
						if (pama->SyncWaitingList == NULL)
						{
							break;
						}

						pama->SyncWaitingList = HLIB_LIST_RemoveIf(pama->SyncWaitingList, msg.pszProcessName, pama_syncwatinglist_compare, free);
						if (pama->SyncWaitingList == NULL)
						{
							pama->bSyncReady = TRUE;
							pama->ulWaitThreshold = HLIB_STD_GetSystemTick();
						}
						HxLOG_Warning("ePAMA_MSG_SYNC_DONE Thred[%d] Wait[%d] Cur[%d]\n", pama->ulWaitThreshold, msg.ulParam1, HLIB_STD_GetSystemTick());
					}
					break;
				case ePAMA_MSG_SET_POWERSAVING :
					HAPI_SetRegistryInt(PxDB_POWERSAVING_ON_STANDBY, msg.ulParam1);
					break;

				case ePAMA_MSG_SET_SLEEPMODE :
					HAPI_SetRegistryInt(PxDB_SLEEPMODE_ON_STANDBY, msg.ulParam1);
					break;

				case ePAMA_MSG_SET_BOOTMODE :
					HAPI_SetRegistryInt(PxDB_BOOTMODE_ON_STANDBY, msg.ulParam1);
					break;

				case ePAMA_MSG_SET_WIFIPOWER :
					HAPI_SetRegistryInt(PxDB_WIFIPOWER_ON_STANDBY, msg.ulParam1);
					break;

				default:
					break;
			}

			if (msg.pszProcessName)
				HLIB_STD_MemFree(msg.pszProcessName);

			pama_releaseInstance(pama);
		}
		else
		{
//			HInputEvent_t	hKeyEvent;

			pama = pama_getInstance();
			//	Status가 다르다는 말은 Power Sequence가 진행 중인것을 의미 한다.
			if (pama->eCurrentMode != pama->eRequestMode)
			{
				//HxLOG_Print("PAMA_MAIN[%d] : Check Status current(%s) Request(%s)\n", __LINE__, STATUS2STR(pama->eCurrentMode), STATUS2STR(pama->eRequestMode));
				if (pama->eRequestMode == ePAMA_STATUS_Operation)
				{
					HxLOG_Error("PAMA_MAIN : ePAMA_STATUS_Operation!!!\n");
					pama->eCurrentMode = ePAMA_STATUS_Operation;
				}
				else if(pama->eCurrentMode != ePAMA_STATUS_ShutdownDone && pama->eCurrentMode != ePAMA_STATUS_RebootDone)	// request mode is Shutdown or Reboot
				{
					if (HLIB_STD_GetSystemTick() > pama->ulWaitThreshold)
					{
						if ((pama->bWaitShutDownFlag == TRUE || pama->bMainAppReady == FALSE)
							&& pama->eRequestMode == ePAMA_STATUS_Shutdown)
						{
							pama->bWaitShutDownFlag = FALSE;
							HxLOG_Error("PAMA_MAIN : Wait Flag is SET --> resend ePAMA_STATUS_Shutdown Thres[%d] Cur[%d]!!!\n", pama->ulWaitThreshold, HLIB_STD_GetSystemTick());
							pama->ulWaitThreshold = HLIB_STD_GetSystemTick() + PAMA_WAITING_DURATION;
							HAPI_SetRegistryInt(PxDB_LAST_STANDBY, TRUE);
							HxLOG_Warning("PAMA BroadcastSignal ePAMA_STATUS_Shutdown\n");
							HAPI_BroadcastSignal("signal:onSystemStateChanged", 1, "i", ePAMA_STATUS_Shutdown);
							pama_print_bustime();
						}
						else if (pama->bWaitRebootFlag  == TRUE && pama->eRequestMode == ePAMA_STATUS_Reboot)
						{
							pama->bWaitRebootFlag = FALSE;
							HxLOG_Error("PAMA_MAIN : Wait Flag is SET --> resend ePAMA_STATUS_Reboot!!!\n");
							pama->ulWaitThreshold = HLIB_STD_GetSystemTick() + PAMA_WAITING_DURATION;
							HxLOG_Warning("PAMA BroadcastSignal ePAMA_STATUS_Reboot\n");
							HAPI_BroadcastSignal("signal:onSystemStateChanged", 1, "i", ePAMA_STATUS_Reboot);
							pama_print_bustime();
						}
						else if (pama->bSyncReady == FALSE && (pama->eRequestMode == ePAMA_STATUS_Reboot || pama->eRequestMode == ePAMA_STATUS_Shutdown ))
						{
							pama->eCurrentMode = ePAMA_STATUS_Sync;
							pama->bSyncReady = TRUE;
							HxLOG_Error("PAMA_MAIN : PAMA Go REBOOT sync  CALL : Wait for Umount Devices Thres[%d] Cur[%d]!!!\n", pama->ulWaitThreshold, HLIB_STD_GetSystemTick());
							pama->ulWaitThreshold = HLIB_STD_GetSystemTick() + PAMA_WAITING_SYNC_DURATION;
							HxLOG_Warning("PAMA BroadcastSignal ePAMA_STATUS_Sync\n");
							HAPI_BroadcastSignal("signal:onSystemStateChanged", 1, "i", ePAMA_STATUS_Sync);
							pama_print_bustime();
						}
						else
						{
							//	기다릴 만큼 기다렸다면...
							pama->SyncWaitingList = HLIB_LIST_RemoveAllFunc(pama->SyncWaitingList, HLIB_STD_MemFree_CB);
							pama->bSyncReady = FALSE;
							if (pama->eRequestMode == ePAMA_STATUS_Shutdown)
							{
								pama_print_bustime();
								pama->eCurrentMode = ePAMA_STATUS_ShutdownDone;
								HxLOG_Error("PAMA_MAIN : PAMA Go SHUTDOWN DONE  CALL@@@ Thres[%d] Cur[%d]!!!\n", pama->ulWaitThreshold, HLIB_STD_GetSystemTick());
								//pama_logcapture_write("Shutdown process is ready. Call SHUTDOWN");
								pama_do_shutdown(pama);
							}
							else if (pama->eRequestMode == ePAMA_STATUS_Reboot)
							{
								pama_print_bustime();
								pama->eCurrentMode = ePAMA_STATUS_RebootDone;
								//pama_logcapture_write("Reboot process is ready. Call Reboot");
								HxLOG_Error("PAMA_MAIN : PAMA Go REBOOT DONE\n");
								pama_do_reboot();
							}
						}
					}
					else
					{
#ifdef CONFIG_DEBUG
						static HUINT32 debugCount = 0;
						if(debugCount % 100 == 0)
						HxLOG_Print("PAMA_MAIN : PAMA NOTHING TO DO. Remained Action Time\n");

						debugCount++;
#endif
					}
				}
			}

			pama_releaseInstance(pama);
#if 0
			if (HAPI_GetInputEvent(eAWINDOW_PAMA, &hKeyEvent, 0) == TRUE)
			{
				HInputEvent_t	trashBuffer;
				if (pama->bMainAppReady == FALSE)
				{
					HxLOG_Error("PAMA_MAIN[%d] : Return consume key because Not Ready Main Application..\n",__LINE__);
					HAPI_ReturnKeyEvent(eAWINDOW_PAMA, eHINPUTRET_CONSUMED, &hKeyEvent);
				}
				else
				if ( pama_is_supported_powerkey(hKeyEvent.key_symbol) )
				{
					HxLOG_Error("Pama Get Ke Event Press\n", (hKeyEvent.type==DIET_KEYPRESS)?"Press":"Release");

					if (hKeyEvent.type == DIET_KEYPRESS)
					{
						if (pama->eRequestMode == ePAMA_STATUS_Shutdown && (pama->eCurrentMode == ePAMA_STATUS_Shutdown || pama->eCurrentMode == ePAMA_STATUS_Operation || pama->eCurrentMode == ePAMA_STATUS_Undefined))	//	Operation --> Shutdown진입 중일 경우의 키처리.
						{
							pama->eRequestMode = ePAMA_STATUS_Operation;
							pama->eCurrentMode   = ePAMA_STATUS_Operation;
							HxLOG_Error("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Pama Get Key POWER\n");
							HxLOG_Error("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Pama Get Key POWER\n");
							HxLOG_Error("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Pama Get Key POWER\n");
							//	Operation에서 Shutdown 진입 중간에 다시 operation요청 받음.
							pama->eCurrentMode = ePAMA_STATUS_Operation;
							HAPI_SetRegistryInt(PxDB_LAST_STANDBY, FALSE);
							HAPI_BroadcastSignal("signal:onSystemStateChanged", 1, "i", ePAMA_STATUS_Operation);
							HAPI_ReturnKeyEvent(eAWINDOW_PAMA, eHINPUTRET_CONSUMED, &hKeyEvent);
							//	PAMA에 들어온 키 버퍼를 비운다.
							while (HAPI_GetInputEvent(eAWINDOW_PAMA, &trashBuffer, 0))
								HAPI_ReturnKeyEvent(eAWINDOW_PAMA, eHINPUTRET_CONSUMED, &trashBuffer);
						}
					}
				}
				HAPI_ReturnKeyEvent(eAWINDOW_PAMA, eHINPUTRET_PASS, &hKeyEvent);
			}
#endif
		}
	}
}

#if defined(CONFIG_DEBUG)
#define ENUM_TO_STR(enumVal) 			case enumVal: return #enumVal;

static char * getKeystring(DFBInputDeviceKeySymbol key_symbol)
{
	switch(key_symbol)
	{
	ENUM_TO_STR(DIKS_NULL);
	ENUM_TO_STR(DIKS_BACKSPACE);
	ENUM_TO_STR(DIKS_TAB);
	ENUM_TO_STR(DIKS_RETURN);
	ENUM_TO_STR(DIKS_CANCEL);
	ENUM_TO_STR(DIKS_ESCAPE);
	ENUM_TO_STR(DIKS_SPACE);
	ENUM_TO_STR(DIKS_EXCLAMATION_MARK);
	ENUM_TO_STR(DIKS_QUOTATION);
	ENUM_TO_STR(DIKS_NUMBER_SIGN);
	ENUM_TO_STR(DIKS_DOLLAR_SIGN);
	ENUM_TO_STR(DIKS_PERCENT_SIGN);
	ENUM_TO_STR(DIKS_AMPERSAND);
	ENUM_TO_STR(DIKS_APOSTROPHE);
	ENUM_TO_STR(DIKS_PARENTHESIS_LEFT);
	ENUM_TO_STR(DIKS_PARENTHESIS_RIGHT);
	ENUM_TO_STR(DIKS_PLUS_SIGN);
	ENUM_TO_STR(DIKS_COMMA);
	ENUM_TO_STR(DIKS_MINUS_SIGN);
	ENUM_TO_STR(DIKS_PERIOD);
	ENUM_TO_STR(DIKS_SLASH);
	ENUM_TO_STR(DIKS_0);
	ENUM_TO_STR(DIKS_1);
	ENUM_TO_STR(DIKS_2);
	ENUM_TO_STR(DIKS_3);
	ENUM_TO_STR(DIKS_4);
	ENUM_TO_STR(DIKS_5);
	ENUM_TO_STR(DIKS_6);
	ENUM_TO_STR(DIKS_7);
	ENUM_TO_STR(DIKS_8);
	ENUM_TO_STR(DIKS_9);
	ENUM_TO_STR(DIKS_COLON);
	ENUM_TO_STR(DIKS_SEMICOLON);
	ENUM_TO_STR(DIKS_LESS_THAN_SIGN);
	ENUM_TO_STR(DIKS_EQUALS_SIGN);
	ENUM_TO_STR(DIKS_GREATER_THAN_SIGN);
	ENUM_TO_STR(DIKS_QUESTION_MARK);
	ENUM_TO_STR(DIKS_AT);
	ENUM_TO_STR(DIKS_CAPITAL_A);
	ENUM_TO_STR(DIKS_CAPITAL_B);
	ENUM_TO_STR(DIKS_CAPITAL_C);
	ENUM_TO_STR(DIKS_CAPITAL_D);
	ENUM_TO_STR(DIKS_CAPITAL_E);
	ENUM_TO_STR(DIKS_CAPITAL_F);
	ENUM_TO_STR(DIKS_CAPITAL_G);
	ENUM_TO_STR(DIKS_CAPITAL_H);
	ENUM_TO_STR(DIKS_CAPITAL_I);
	ENUM_TO_STR(DIKS_CAPITAL_J);
	ENUM_TO_STR(DIKS_CAPITAL_K);
	ENUM_TO_STR(DIKS_CAPITAL_L);
	ENUM_TO_STR(DIKS_CAPITAL_M);
	ENUM_TO_STR(DIKS_CAPITAL_N);
	ENUM_TO_STR(DIKS_CAPITAL_O);
	ENUM_TO_STR(DIKS_CAPITAL_P);
	ENUM_TO_STR(DIKS_CAPITAL_Q);
	ENUM_TO_STR(DIKS_CAPITAL_R);
	ENUM_TO_STR(DIKS_CAPITAL_S);
	ENUM_TO_STR(DIKS_CAPITAL_T);
	ENUM_TO_STR(DIKS_CAPITAL_U);
	ENUM_TO_STR(DIKS_CAPITAL_V);
	ENUM_TO_STR(DIKS_CAPITAL_W);
	ENUM_TO_STR(DIKS_CAPITAL_X);
	ENUM_TO_STR(DIKS_CAPITAL_Y);
	ENUM_TO_STR(DIKS_CAPITAL_Z);
	ENUM_TO_STR(DIKS_SQUARE_BRACKET_LEFT);
	ENUM_TO_STR(DIKS_BACKSLASH);
	ENUM_TO_STR(DIKS_SQUARE_BRACKET_RIGHT);
	ENUM_TO_STR(DIKS_CIRCUMFLEX_ACCENT);
	ENUM_TO_STR(DIKS_UNDERSCORE);
	ENUM_TO_STR(DIKS_GRAVE_ACCENT);
	ENUM_TO_STR(DIKS_SMALL_A);
	ENUM_TO_STR(DIKS_SMALL_B);
	ENUM_TO_STR(DIKS_SMALL_C);
	ENUM_TO_STR(DIKS_SMALL_D);
	ENUM_TO_STR(DIKS_SMALL_E);
	ENUM_TO_STR(DIKS_SMALL_F);
	ENUM_TO_STR(DIKS_SMALL_G);
	ENUM_TO_STR(DIKS_SMALL_H);
	ENUM_TO_STR(DIKS_SMALL_I);
	ENUM_TO_STR(DIKS_SMALL_J);
	ENUM_TO_STR(DIKS_SMALL_K);
	ENUM_TO_STR(DIKS_SMALL_L);
	ENUM_TO_STR(DIKS_SMALL_M);
	ENUM_TO_STR(DIKS_SMALL_N);
	ENUM_TO_STR(DIKS_SMALL_O);
	ENUM_TO_STR(DIKS_SMALL_P);
	ENUM_TO_STR(DIKS_SMALL_Q);
	ENUM_TO_STR(DIKS_SMALL_R);
	ENUM_TO_STR(DIKS_SMALL_S);
	ENUM_TO_STR(DIKS_SMALL_T);
	ENUM_TO_STR(DIKS_SMALL_U);
	ENUM_TO_STR(DIKS_SMALL_V);
	ENUM_TO_STR(DIKS_SMALL_W);
	ENUM_TO_STR(DIKS_SMALL_X);
	ENUM_TO_STR(DIKS_SMALL_Y);
	ENUM_TO_STR(DIKS_SMALL_Z);

	ENUM_TO_STR(DIKS_CURLY_BRACKET_LEFT);
	ENUM_TO_STR(DIKS_VERTICAL_BAR);
	ENUM_TO_STR(DIKS_CURLY_BRACKET_RIGHT);
	ENUM_TO_STR(DIKS_TILDE);
	ENUM_TO_STR(DIKS_DELETE);

	ENUM_TO_STR(DIKS_CURSOR_LEFT);
	ENUM_TO_STR(DIKS_CURSOR_RIGHT);
	ENUM_TO_STR(DIKS_CURSOR_UP);
	ENUM_TO_STR(DIKS_CURSOR_DOWN);
	ENUM_TO_STR(DIKS_INSERT);
	ENUM_TO_STR(DIKS_HOME);
	ENUM_TO_STR(DIKS_END);
	ENUM_TO_STR(DIKS_PAGE_UP);
	ENUM_TO_STR(DIKS_PAGE_DOWN);
	ENUM_TO_STR(DIKS_PRINT);
	ENUM_TO_STR(DIKS_PAUSE);
	ENUM_TO_STR(DIKS_OK);
	ENUM_TO_STR(DIKS_SELECT);
	ENUM_TO_STR(DIKS_GOTO);
	ENUM_TO_STR(DIKS_CLEAR);
	ENUM_TO_STR(DIKS_POWER);
	ENUM_TO_STR(DIKS_POWER2);
	ENUM_TO_STR(DIKS_OPTION);
	ENUM_TO_STR(DIKS_MENU);
	ENUM_TO_STR(DIKS_HELP);
	ENUM_TO_STR(DIKS_INFO);
	ENUM_TO_STR(DIKS_TIME);
	ENUM_TO_STR(DIKS_VENDOR);

	ENUM_TO_STR(DIKS_ARCHIVE);
	ENUM_TO_STR(DIKS_PROGRAM);
	ENUM_TO_STR(DIKS_CHANNEL);
	ENUM_TO_STR(DIKS_FAVORITES);
	ENUM_TO_STR(DIKS_EPG);
	ENUM_TO_STR(DIKS_PVR);
	ENUM_TO_STR(DIKS_MHP);
	ENUM_TO_STR(DIKS_LANGUAGE);
	ENUM_TO_STR(DIKS_TITLE);
	ENUM_TO_STR(DIKS_SUBTITLE);
	ENUM_TO_STR(DIKS_ANGLE);
	ENUM_TO_STR(DIKS_ZOOM);
	ENUM_TO_STR(DIKS_MODE);
	ENUM_TO_STR(DIKS_KEYBOARD);
	ENUM_TO_STR(DIKS_PC);
	ENUM_TO_STR(DIKS_SCREEN);
	ENUM_TO_STR(DIKS_TV);
	ENUM_TO_STR(DIKS_TV2);
	ENUM_TO_STR(DIKS_VCR);
	ENUM_TO_STR(DIKS_VCR2);
	ENUM_TO_STR(DIKS_SAT);
	ENUM_TO_STR(DIKS_SAT2);
	ENUM_TO_STR(DIKS_CD);
	ENUM_TO_STR(DIKS_TAPE);
	ENUM_TO_STR(DIKS_RADIO);
	ENUM_TO_STR(DIKS_TUNER);
	ENUM_TO_STR(DIKS_PLAYER);
	ENUM_TO_STR(DIKS_TEXT);
	ENUM_TO_STR(DIKS_DVD);
	ENUM_TO_STR(DIKS_AUX);
	ENUM_TO_STR(DIKS_MP3);
	ENUM_TO_STR(DIKS_PHONE);
	ENUM_TO_STR(DIKS_AUDIO);
	ENUM_TO_STR(DIKS_VIDEO);

	ENUM_TO_STR(DIKS_INTERNET);
	ENUM_TO_STR(DIKS_MAIL);
	ENUM_TO_STR(DIKS_NEWS);
	ENUM_TO_STR(DIKS_DIRECTORY);
	ENUM_TO_STR(DIKS_LIST);
	ENUM_TO_STR(DIKS_CALCULATOR);
	ENUM_TO_STR(DIKS_MEMO);
	ENUM_TO_STR(DIKS_CALENDAR);
	ENUM_TO_STR(DIKS_EDITOR);
	ENUM_TO_STR(DIKS_RED);
	ENUM_TO_STR(DIKS_GREEN);
	ENUM_TO_STR(DIKS_YELLOW);
	ENUM_TO_STR(DIKS_BLUE);

	ENUM_TO_STR(DIKS_CHANNEL_UP);
	ENUM_TO_STR(DIKS_CHANNEL_DOWN);
	ENUM_TO_STR(DIKS_BACK);
	ENUM_TO_STR(DIKS_FORWARD);
	ENUM_TO_STR(DIKS_FIRST);
	ENUM_TO_STR(DIKS_LAST);
	ENUM_TO_STR(DIKS_VOLUME_UP);
	ENUM_TO_STR(DIKS_VOLUME_DOWN);
	ENUM_TO_STR(DIKS_MUTE);
	ENUM_TO_STR(DIKS_AB);
	ENUM_TO_STR(DIKS_PLAYPAUSE);
	ENUM_TO_STR(DIKS_PLAY);
	ENUM_TO_STR(DIKS_STOP);
	ENUM_TO_STR(DIKS_RESTART);
	ENUM_TO_STR(DIKS_SLOW);
	ENUM_TO_STR(DIKS_FAST);
	ENUM_TO_STR(DIKS_RECORD);
	ENUM_TO_STR(DIKS_EJECT);
	ENUM_TO_STR(DIKS_SHUFFLE);
	ENUM_TO_STR(DIKS_REWIND);
	ENUM_TO_STR(DIKS_FASTFORWARD);
	ENUM_TO_STR(DIKS_PREVIOUS);
	ENUM_TO_STR(DIKS_NEXT);
	ENUM_TO_STR(DIKS_BEGIN);
	ENUM_TO_STR(DIKS_DIGITS);
	ENUM_TO_STR(DIKS_TEEN);
	ENUM_TO_STR(DIKS_TWEN);
	ENUM_TO_STR(DIKS_BREAK);
	ENUM_TO_STR(DIKS_EXIT);
	ENUM_TO_STR(DIKS_SETUP);
	ENUM_TO_STR(DIKS_CURSOR_LEFT_UP);
	ENUM_TO_STR(DIKS_CURSOR_LEFT_DOWN);
	ENUM_TO_STR(DIKS_CURSOR_UP_RIGHT);
	ENUM_TO_STR(DIKS_CURSOR_DOWN_RIGHT);
	ENUM_TO_STR(DIKS_PIP);
	ENUM_TO_STR(DIKS_SWAP);
	ENUM_TO_STR(DIKS_MOVE);
	ENUM_TO_STR(DIKS_FREEZE);
	ENUM_TO_STR(DIKS_F1);
	ENUM_TO_STR(DIKS_F2);
	ENUM_TO_STR(DIKS_F3);
	ENUM_TO_STR(DIKS_F4);
	ENUM_TO_STR(DIKS_F5);
	ENUM_TO_STR(DIKS_F6);
	ENUM_TO_STR(DIKS_F7);
	ENUM_TO_STR(DIKS_F8);
	ENUM_TO_STR(DIKS_F9);
	ENUM_TO_STR(DIKS_F10);
	ENUM_TO_STR(DIKS_F11);
	ENUM_TO_STR(DIKS_F12);
	ENUM_TO_STR(DIKS_SHIFT);
	ENUM_TO_STR(DIKS_CONTROL);
	ENUM_TO_STR(DIKS_ALT);
	ENUM_TO_STR(DIKS_ALTGR);
	ENUM_TO_STR(DIKS_META);
	ENUM_TO_STR(DIKS_SUPER);
	ENUM_TO_STR(DIKS_HYPER);
	ENUM_TO_STR(DIKS_CAPS_LOCK);
	ENUM_TO_STR(DIKS_NUM_LOCK);
	ENUM_TO_STR(DIKS_SCROLL_LOCK);
	ENUM_TO_STR(DIKS_DEAD_ABOVEDOT);
	ENUM_TO_STR(DIKS_DEAD_ABOVERING);
	ENUM_TO_STR(DIKS_DEAD_ACUTE);
	ENUM_TO_STR(DIKS_DEAD_BREVE);
	ENUM_TO_STR(DIKS_DEAD_CARON);
	ENUM_TO_STR(DIKS_DEAD_CEDILLA);
	ENUM_TO_STR(DIKS_DEAD_CIRCUMFLEX);
	ENUM_TO_STR(DIKS_DEAD_DIAERESIS);
	ENUM_TO_STR(DIKS_DEAD_DOUBLEACUTE);
	ENUM_TO_STR(DIKS_DEAD_GRAVE);
	ENUM_TO_STR(DIKS_DEAD_IOTA);
	ENUM_TO_STR(DIKS_DEAD_MACRON);
	ENUM_TO_STR(DIKS_DEAD_OGONEK);
	ENUM_TO_STR(DIKS_DEAD_SEMIVOICED_SOUND);
	ENUM_TO_STR(DIKS_DEAD_TILDE);
	ENUM_TO_STR(DIKS_DEAD_VOICED_SOUND);

	default:
		return "unknown";
	}
}
#endif

static void	pama_system_keyevent(void *arg)
{
	pama_t			*pama;
	HInputEvent_t	hKeyEvent;
	HInputEvent_t	trashBuffer;

	while (1)
	{
		if (HAPI_GetInputEvent(eAWINDOW_PAMA, &hKeyEvent, -1) == TRUE)
		{
			pama = pama_getInstance();

			if (pama->bMainAppReady == FALSE)
			{
				HxLOG_Error("PAMA_MAIN[%d] : Return consume key because Not Ready Main Application..\n",__LINE__);
#if defined(CONFIG_PRISM_ZAPPING)
				HAPI_ReturnKeyEvent(eAWINDOW_PAMA, eHINPUTRET_PASS, &hKeyEvent);
#else
				HAPI_ReturnKeyEvent(eAWINDOW_PAMA, eHINPUTRET_CONSUMED, &hKeyEvent);
#endif
			}
			else if ( pama_is_supported_powerkey(hKeyEvent.key_symbol) )
			{
#ifdef CONFIG_DEBUG
				HxLOG_Print(HxANSI_COLOR_YELLOW("Pama Get Key Event, %s [%s]\n"), getKeystring(hKeyEvent.key_symbol), (hKeyEvent.type==DIET_KEYPRESS)?"Press":"Release");
#endif

				if (hKeyEvent.type == DIET_KEYPRESS)
				{
					HxLOG_Print(HxANSI_COLOR_YELLOW("pama->eRequestMode           = %d\n"), pama->eRequestMode);
					HxLOG_Print(HxANSI_COLOR_YELLOW("pama->eCurrentMode           = %d\n"), pama->eCurrentMode);
					HxLOG_Print(HxANSI_COLOR_YELLOW("pama->ulRequestShutdownTime  = %d (diff:%d)\n"), pama->ulRequestShutdownTime, HLIB_STD_GetSystemTick() - (pama->ulRequestShutdownTime + PAMA_WAITING_RCU_DURATION));
					if (pama->eRequestMode == ePAMA_STATUS_Shutdown
						&& (pama->eCurrentMode == ePAMA_STATUS_Shutdown
							|| pama->eCurrentMode == ePAMA_STATUS_Operation
							|| pama->eCurrentMode == ePAMA_STATUS_Undefined))	//	Operation --> Shutdown진입 중일 경우의 키처리.
					{
						if(pama->ulRequestShutdownTime == 0
							|| (pama->ulRequestShutdownTime + PAMA_WAITING_RCU_DURATION) < HLIB_STD_GetSystemTick()
							)
						{
							pama->eRequestMode = ePAMA_STATUS_Operation;
							pama->eCurrentMode   = ePAMA_STATUS_Operation;
							HxLOG_Error("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Pama Get Key POWER\n");
							HxLOG_Error("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Pama Get Key POWER\n");
							HxLOG_Error("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Pama Get Key POWER\n");
							//	Operation에서 Shutdown 진입 중간에 다시 operation요청 받음.
							pama->eCurrentMode = ePAMA_STATUS_Operation;
							HAPI_SetRegistryInt(PxDB_LAST_STANDBY, FALSE);
							HxLOG_Warning("PAMA BroadcastSignal ePAMA_STATUS_Operation\n");
							HAPI_BroadcastSignal("signal:onSystemStateChanged", 1, "i", ePAMA_STATUS_Operation);
							HAPI_ReturnKeyEvent(eAWINDOW_PAMA, eHINPUTRET_CONSUMED, &hKeyEvent);
							//	PAMA에 들어온 키 버퍼를 비운다.
							while (HAPI_GetInputEvent(eAWINDOW_PAMA, &trashBuffer, 0))
								HAPI_ReturnKeyEvent(eAWINDOW_PAMA, eHINPUTRET_CONSUMED, &trashBuffer);
						}
					}
				}
			}

			HAPI_ReturnKeyEvent(eAWINDOW_PAMA, eHINPUTRET_PASS, &hKeyEvent);

			pama_releaseInstance(pama);
		}
	}
}

static void	pama_releaseInstance(pama_t *pama)
{
	HxSEMT_Release(pama->ulSemId);
}

static pama_t*	pama_getInstance(void)
{
	static pama_t	*s_pstPama = NULL;

	if (s_pstPama == NULL)
	{
		unsigned long	tid;

		s_pstPama = (pama_t*)HLIB_STD_MemCalloc(sizeof(pama_t));
		HxSEMT_Create(&s_pstPama->ulSemId, "pamasem", HxSEMT_FIFO);

		HxSEMT_Get(s_pstPama->ulSemId);
		s_pstPama->bMainAppReady	= FALSE;
		s_pstPama->bSyncReady		= FALSE;		
		s_pstPama->eCurrentMode       = ePAMA_STATUS_Undefined;
		s_pstPama->eRequestMode     = ePAMA_STATUS_Undefined;
		s_pstPama->eFirstBootType	= eHSystemOperationMode_UNKNOWN;
		s_pstPama->SyncWaitingList		= NULL;
		s_pstPama->server = HLIB_EXTRA_OpenServer("pama");//HLIB_SOCKET_OpenServer(HxSOCKET_UDS, PAMA_SIGNAL_UDP);
		s_pstPama->nRpcHandle = HLIB_RPC_Open(s_pstPama->server, HLIB_STD_GetPID());
		HLIB_MSG_Create(PAMA_MSGQ_SIZE, sizeof(PAMA_MSG_t), "pamaQ", (HULONG*)&s_pstPama->ulMainMsgQueue, eHLIB_SUSPENDTYPE_FIFO);

		HAPI_AddVirtualWindow(eAWINDOW_PAMA);

		HAPI_ConnectSignal("signal:onBootupStateChanged",   NULL, pama_system_onBootupStateChanged);
		HAPI_ConnectSignal("signal:onMainApplicationReady", NULL, pama_system_onMainApplicationReady);

		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_POWER, TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_CHANNEL_UP, TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_CHANNEL_DOWN, TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_0, TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_1, TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_2, TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_3, TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_4, TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_5, TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_6, TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_7, TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_8, TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_9, TRUE);

		HAPI_SetWindowStyle(eAWINDOW_PAMA, eHWS_WindowInputAlways, FALSE);

		//	for papi event
		VK_TASK_Create(pama_server_listener, VK_TASK_PRIORITY_MW_REF, SIZE_32K, "pamasvr", (void*)s_pstPama, &tid, 0);
		VK_TASK_Start(tid);

		//	for main task
		VK_TASK_Create(pama_system_main, VK_TASK_PRIORITY_MW_REF, SIZE_32K, "pama", (void*)s_pstPama->ulMainMsgQueue, &tid, 0);
		VK_TASK_Start(tid);

		//	for key task
		VK_TASK_Create(pama_system_keyevent, VK_TASK_PRIORITY_MW_REF, SIZE_32K, "pamakey", (void*)s_pstPama, &tid, 0);
		VK_TASK_Start(tid);

		HLIB_RPC_RegisterCall(s_pstPama->nRpcHandle, "pama.setCommand", "iis", pama_set_cmd,
			s_pstPama, "set Command: (PxSTATUS_e cmd, HINT32 time, HCHAR * procName)");

		HAPI_SetRegistryInt(PxDB_MAINAPP_READY, s_pstPama->bMainAppReady);
	}
	else
	{
		HxSEMT_Get(s_pstPama->ulSemId);
	}

	return s_pstPama;
}


static DxDBSetupItem_t *s_pstItemLastStandby = NULL;
static DxDBSetupItem_t *s_pstItemPowerSaving = NULL;



STATIC void		pama_db_Getter(const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *pvUser)
{
	DxDBSetupItem_t	*pstItemInfo;
	DAxIterator_t	iter;

	HxLOG_Print("[%s] size[%d] data[%lld]\n", __FUNCTION__, size, uid);

	if (pvData == NULL || size == 0)
	{
		HxLOG_Error("Invalid parameter\n");
		return;
	}

	DAPI_InitIterator(&iter, pvData, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		pstItemInfo = (DxDBSetupItem_t*)iter.data;
		if (iter.uid != (HUID)pstItemInfo->uid)
		{
			HxLOG_Critical("Getter Fail~~~~~~~!\n");
			HxLOG_Assert(0);
		}
		if(iter.uid == eDxSETUP_USER_LAST_STANDBY)
			s_pstItemLastStandby = HLIB_STD_MemDup(iter.data, sizeof(DxDBSetupItem_t) + pstItemInfo->nSize);
		else if(iter.uid == eDxSETUP_SYSTEM_PWR_SAVING_ON_STANDBY)
			s_pstItemPowerSaving = HLIB_STD_MemDup(iter.data, sizeof(DxDBSetupItem_t) + pstItemInfo->nSize);
	}
}


void pama_db_move(void)
{
	HUINT32 nValue = 0;
	if(HAPI_GetRegistryInt(PxDB_LAST_STANDBY, &nValue) == ERR_FAIL)
	{
		HUINT32 id = DAPI_Get(DxDBSETUP_SYSTEM,(HUID)eDxSETUP_USER_LAST_STANDBY,pama_db_Getter,NULL);
		HxLOG_Print("[%s] There is not ActiveStandbyDb in HAMA\n", __FUNCTION__);

		DAPI_Wait(10000, id);
		if(s_pstItemLastStandby)
		{
			HxLOG_Print("[%s] Get DB from DAMA\n", __FUNCTION__);
			HAPI_SetRegistryInt(PxDB_LAST_STANDBY,s_pstItemLastStandby->nValue);
		}
		else
		{
			HxLOG_Error("[%s] Can Not Get DB from DAMA\n", __FUNCTION__);
			HAPI_SetRegistryInt(PxDB_LAST_STANDBY, 0);
		}
	}
	else
	{
		HxLOG_Print("[%s] There is ActiveStandbyDb in HAMA\n", __FUNCTION__);
	}

	if(HAPI_GetRegistryInt(PxDB_POWERSAVING_ON_STANDBY, &nValue) == ERR_FAIL)
	{
		HUINT32 id = DAPI_Get(DxDBSETUP_SYSTEM,(HUID)eDxSETUP_SYSTEM_PWR_SAVING_ON_STANDBY,pama_db_Getter,NULL);
		HxLOG_Print("[%s] There is not ActiveStandbyDb in HAMA\n", __FUNCTION__);

		DAPI_Wait(10000, id);
		if(s_pstItemPowerSaving)
		{
			HxLOG_Print("[%s] Get DB from DAMA\n", __FUNCTION__);
			HAPI_SetRegistryInt(PxDB_POWERSAVING_ON_STANDBY,s_pstItemPowerSaving->nValue);
		}
		else
		{
			HxLOG_Error("[%s] Can Not Get DB from DAMA\n", __FUNCTION__);
#if defined(CONFIG_PROD_DIGIMAXT2)
			HAPI_SetRegistryInt(PxDB_POWERSAVING_ON_STANDBY, 0);  // default setting off:1 for Digimax-T2 (S3)
#else
			HAPI_SetRegistryInt(PxDB_POWERSAVING_ON_STANDBY, 1);  // default setting off:0, on:1
#endif
		}
	}
	else
	{
		HxLOG_Print("[%s] There is ActiveStandbyDb in HAMA\n", __FUNCTION__);
	}
}



/**************************************************************
	public functions
 **************************************************************/

int		PAMA_Main(int argc, char **argv)
{
	VK_Init();
#if defined(CONFIG_PROD_OS_EMULATOR)
	EMU_UART_Init();
#endif

	//	Drop Root Previleges --> if CONFIG_DROP_ROOT_PRIVILEGES is ON,
	HLIB_EXTRA_DropRootPrivileges("pama");

#ifdef	CONFIG_DEBUG
	HLIB_DBG_Init();

	Pama_command_Init();
#endif
	signal(SIGPIPE, SIG_IGN);

#if defined(CONFIG_HUMAX_CRASHREPORT)
	HCR_Init(NULL);
#endif

	pama_db_move();

	pama_releaseInstance(pama_getInstance());

	return 0;
}
