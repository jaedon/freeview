/*************************************************************************************************************
	File            : hom_dmp.c
	author          : IPM tipmsw1@humaxdigital.com
	comment         :
	date            :
	attention       :

	(c) 2011-2012 Humax Co., Ltd.
	This program is produced by Humax Co., Ltd. (Humax) and
	the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
	non-assignable, non-transferable and non-exclusive license to use this Software.
	You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
	you agree to the responsibility to take all reasonable efforts to protect the any information
	you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
	reverse engineer or extract the source code of this Software unless you have Humax's written permission
	to do so.
	If you have no authorized license, discontinue using this Software immediately.

	THE SOFTWARE IS PROVIDED ¡°AS IS¡± AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
	EITHER EXPRESS, IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
	IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR
	INDIRECT, SPECIAL, CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR
	INABILITY TO USE THE SOFTWARE.

	This License is effective until terminated. You may terminate this License at any time
	by destroying all copies of the Software including all documentation. This License will terminate
	immediately without notice from Humax to you if you fail to comply with any provision of this License.
	Upon termination, you must destroy all copies of the Software and all documentation.

	The laws of the Republic of Korea will apply to any disputes arising out of or relating to
	this Copyright Notice. All claims arising out of or relating to this Copyright Notice will be litigated
	in the Seoul Central District Court, in the Republic of Korea.
*************************************************************************************************************/

#define	________________Header_Files_______________________________
#include 	<vkernel.h>
#include 	<hlib.h>

#include	"common.h"
#include 	"homma_int.h"
#include 	"hom_int.h"
#include 	"homma_bus.h"
#include 	"homma_ipc_decode.h"
#include	"woons.h"

#define ________________Defines____________________________________
#define	__MODULE__				"HOM:WOONSERVER"
#define	DLNA_ACTION_TAG			"action"
#define	DLNA_TYPE_TAG			"type"
#define DLNA_STARTARG_TAG		"start_arg"

#define ________________Typedef____________________________________
typedef struct
{
	BUS_Handle			hMsgBusHandle;
	HOMMA_BUS_Target_e 	eSelf;

	HINT32   			clientSocket;
	HINT32   			reqId;

	/* monitoring storage */
	HBOOL				bMonitorDMS;
	HBOOL				bPrevStartFailed;
	HBOOL				bRequestStart;
	HCHAR 				*pszFriendlyName;
} HOM_WoonServer_Context_t;

#define ________________Static_Variables___________________________
static HOM_WoonServer_Context_t s_stHomWoonserverContext;

#define ________________Internal_Functions_________________________
static	void	__hom_woonserver_init( HOM_WoonServer_Context_t *pstHomWoonsCtx)
{
	pstHomWoonsCtx->eSelf = eBUS_TARGET_MOD_WOONS;

	/* monitoring storage */
	pstHomWoonsCtx->bMonitorDMS      = TRUE;
	pstHomWoonsCtx->bPrevStartFailed = FALSE;
	pstHomWoonsCtx->bRequestStart    = FALSE;
	pstHomWoonsCtx->pszFriendlyName  = NULL;
}

static HOM_WoonServer_Context_t * __ctx( void )
{
	return &s_stHomWoonserverContext;
}

#if defined(CONFIG_HOMMA_WOON)
static HERROR	__hom_woonserver_start( const HCHAR *pszFriendlyName )
{
	HERROR				retErr = ERR_FAIL;
	HOMMA_BUS_Data_t 	retData;
	CO_NET_Info_t 		*pNetInfo = NULL;

	// This is no more need, dms will automatically start up woons when he is started
	return ERR_OK;
	/*
	HxSTD_MemSet(&retData, 0x00, sizeof(HOMMA_BUS_Data_t));

	HOMMA_BUS_Request( eBUS_TARGET_SYS, eBUS_REQUEST_NET_INFO, &retData, &retErr );
	if ( ERR_OK != retErr ) 	return ERR_FAIL;

	if ( eBUS_DATATYPE_USER_DEF <= retData.eDataType )
	{
		pNetInfo = (CO_NET_Info_t *)retData.pvData;
		if ( 0 == pNetInfo->nDevCnt )
		{
			HxLOG_Error("[%s:%d] available network interface is NOT existed. \n", __MODULE__, __LINE__);
			return ERR_FAIL;
		}
		else
			HxLOG_Info("[%s:%d] Default Network Interface - %d \n", __MODULE__, __LINE__, pNetInfo->nDevCnt);
	}

	if (!WOONS_IsStarted())
	{
		retErr = WOONS_Start(pszFriendlyName, pNetInfo->pstIfaces->szMacAddr);
		if ( ERR_OK != retErr )
		{
			HxLOG_Error( "[%s:%d] Error> WoonServer Start Failed!!!!\n" );
			return ERR_FAIL;
		}
	}

	return ERR_OK;
	*/
}
#endif

static HINT32	__hom_woonserver_sys_event_handle( HOMMA_BUS_Target_e eSender,  HOMMA_BUS_Event_e eEvent, HOMMA_BUS_DataType_e eDataType, void *pvData )
{
#if defined(CONFIG_HOMMA_WOON)
	HERROR	hError = ERR_OK;
	HOM_WoonServer_Context_t *pCtx = __ctx();
#endif
	const CO_STORAGE_Mount_t *pMountInfo = NULL;

	switch ( eEvent )
	{
		case eBUS_EVENT_MOD_START:
			{
#if defined(CONFIG_HOMMA_WOON)
				if ((pCtx->bMonitorDMS == TRUE) && (eSender == eBUS_TARGET_MOD_DMS))
				{
					if ((pCtx->bRequestStart == TRUE) &&
						(pCtx->bPrevStartFailed == TRUE) && (WOONS_IsStarted() == FALSE))
					{
						HxLOG_Debug("[%s:%d] Info> Retry WOON server start() because of prev failure(%d)!\n", __func__, __LINE__, pCtx->bPrevStartFailed);
						HxLOG_Error("[ HOM_WOONSERVER] ===========> REQ WOON START : %s (prev:%d) ==============\n", (HCHAR *)pCtx->pszFriendlyName, pCtx->bPrevStartFailed);
						hError = __hom_woonserver_start(pCtx->pszFriendlyName);
						if (hError == ERR_OK)
						{
							pCtx->bPrevStartFailed = FALSE;
						}
						else
						{
							HxLOG_Error("[ HOM_WOONSERVER ] Error> failed to re-start DMS!\n");
						}
					}
				}
#endif
			}
			break;

		case eBUS_EVENT_SYS_FS_MOUNTED :
			pMountInfo = (CO_STORAGE_Mount_t *)pvData;
			if ( pMountInfo )
				HxLOG_Print( "[HOM_WOONSERVER] MOUNT PATH : %s\n", pMountInfo->szPath );
			break;

		case eBUS_EVENT_SYS_FS_UNMOUNTED :
			pMountInfo = (CO_STORAGE_Mount_t *)pvData;
			if ( pMountInfo )
				HxLOG_Print( "[HOM_WOONSERVER] UNMOUNT PATH : %s\n", pMountInfo->szPath );
			break;

		case eBUS_EVENT_SYS_NET_IF_ONLINE :
		case eBUS_EVENT_SYS_NET_IF_OFFLINE :
		case eBUS_EVENT_SYS_NET_IF_SWITCHING :
			{
				const CO_NET_Iface_t *pIfaceInfo = (CO_NET_Iface_t *)pvData;
				HxLOG_Print("[%s:%d] network ifce(%s) status is chagned.. \n", __MODULE__, __LINE__, pIfaceInfo->szName);
			}
			break;

		case eBUS_EVENT_SYS_POWER_REBOOT:
		case eBUS_EVENT_SYS_POWER_SHUTDOWN:
		//case eBUS_EVENT_SYS_POWER_SHUTDOWNDONE:
		case eBUS_EVENT_SYS_MS_FORMAT_TRY:
			{
				HxLOG_Print( "[WOONS_DMS] REBOOT or SHUTDOWN or FORMAT \n");
#if defined(CONFIG_HOMMA_WOON)
				pCtx->bPrevStartFailed = TRUE;
				pCtx->bRequestStart = TRUE;
				//WOONS_Stop(); // DMS will stop it
#endif
			}
			break;


	default :
			break;
	}

	return ERR_OK;
}

static HINT32	__hom_woonserver_cmd_process( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_IpcRecv_t *stJsonData )
{
#if defined(CONFIG_HOMMA_WOON)
	HERROR	hError = ERR_OK;
#endif
	HOM_WoonServer_Context_t *pCtx = __ctx();
	HINT32	nOper =	stJsonData->nOpCode;
	comm_msg_data_t *data = stJsonData->pvJson;

	pCtx->clientSocket = stJsonData->nClientHandle;
	pCtx->reqId = stJsonData->nPairId;

	if ( eBUS_CMD_GET == nOper )
	{
		if ( data->size == 0)
		{
			HOMMA_BUS_Msg_t busMsg;
			HOMMA_BUS_IpcSend_t *pSendData = (HOMMA_BUS_IpcSend_t *)HLIB_STD_MemCalloc( sizeof(HOMMA_BUS_IpcSend_t) );
			HxSTD_MemSet(&busMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));

			busMsg.eMsgType = 	eBUS_MSGTYPE_RESPONSE;
			busMsg.uEnum.eRes = eBUS_REQUEST_SERVER_SENDING;

			pSendData->nPairId 			= stJsonData->nPairId ;
			pSendData->pszJson 			= NULL;
			pSendData->bMalloc 			= TRUE;
			pSendData->nClientHandle 	= stJsonData->nClientHandle;

			busMsg.eMsgType 	= eBUS_MSGTYPE_RESPONSE;
			busMsg.uEnum.eRes 	= eBUS_REQUEST_SERVER_SENDING;
			busMsg.pvData 		= (void *)pSendData;
			busMsg.eDataType 	= eBUS_DATATYPE_IPC_SEDN;
			busMsg.nDataSize 	= sizeof(HOMMA_BUS_IpcSend_t);
			busMsg.bNeedFree 	= TRUE;

			HOMMA_BUS_SendMsg( eBUS_TARGET_SERVER, pCtx->hMsgBusHandle, &busMsg );
			return ERR_OK;
		}
	}

	switch(data->cmd)
	{
		case eHOMAPI_WoonServer_Command_Start:
			{
				HOMxWoonServer_Args_Start_t *param = (HOMxWoonServer_Args_Start_t*)data->data;

				HxLOG_Debug("[ HOM_WOONS] ==============================================\n");
				HxLOG_Debug("[ HOM_WOONS] ===========> REQ WOON START : %s \n", (HCHAR *)param->szFriendlyName);
				HxLOG_Debug("[ HOM_WOONS] ==============================================\n");

#if defined(CONFIG_HOMMA_WOON)
				if ((pCtx->bPrevStartFailed == TRUE) && (pCtx->bRequestStart == TRUE))
				{
					HxLOG_Error("### [%s:%d] Error> Overlapped start() request(prev:%d)! ###\n", __func__, __LINE__, pCtx->bPrevStartFailed);
					;
				}
				if(WOONS_IsStarted() == TRUE)
				{
					HxLOG_Error("### [%s:%d] Error> Already WOON server started! ###\n", __func__, __LINE__);
				}

				pCtx->bRequestStart = TRUE;

				if (pCtx->pszFriendlyName != NULL)
				{
					HOM_MEMFREE(pCtx->pszFriendlyName);
					pCtx->pszFriendlyName = NULL;
				}
				pCtx->pszFriendlyName = HLIB_STD_StrDup((HCHAR *)param->szFriendlyName);

				hError = __hom_woonserver_start((HCHAR *)param->szFriendlyName);
				if (hError != ERR_OK)
				{
					HxLOG_Error("[ HOM_WOONS ] Error> failed to start WOON server!\n");
					pCtx->bPrevStartFailed = TRUE;
				}
#endif
			}
			break;

		case eHOMAPI_WoonServer_Command_Stop:
			{
				HxLOG_Debug("[ HOM_WOONS] ==============================================\n");
				HxLOG_Debug("[ HOM_WOONS] ===========> REQ WOON Server STOP ============\n");
				HxLOG_Debug("[ HOM_WOONS] ==============================================\n");
#if defined(CONFIG_HOMMA_WOON)
				pCtx->bPrevStartFailed = FALSE;
				pCtx->bRequestStart = FALSE;

				if(WOONS_IsStarted() == FALSE)
					HxLOG_Debug("### [%s:%d] Error> Already WOON server stopped! ###\n", __func__, __LINE__);

				//WOONS_Stop(); // DMS will stop it

				if (pCtx->pszFriendlyName != NULL)
				{
					HOM_MEMFREE(pCtx->pszFriendlyName);
					pCtx->pszFriendlyName= NULL;
				}
#endif
			}
			break;
		case eHOMAPI_WoonServer_Command_Rename:
			{
#if defined(CONFIG_HOMMA_WOON)
				HOMxWoonServer_Args_Rename_t *param = (HOMxWoonServer_Args_Rename_t*)data->data;
				HxLOG_Debug("[%s:%d] ######### WoonServer Rename - %s ########### \n", __MODULE__, __LINE__, param->szFriendlyName);

				WOONS_Rename((HCHAR *)param->szFriendlyName);
#endif
			}
			break;

		case eHOMAPI_WoonServer_Command_TestConnection:
			{
				HxLOG_Debug("[%s:%d] ######### WoonServer Test Connection ########### \n", __MODULE__, __LINE__);
#if defined(CONFIG_HOMMA_WOON)
				WOONS_TestConnection();
#endif
			}

	}

	return ERR_OK;
}

static HINT32	__hom_woonserver_bus_request_handler( HOMMA_BUS_Request_e eReq, HOMMA_BUS_Data_t *pstRetData )
{
	return 0;
}

static HINT32	__hom_woonserver_bus_release_handler( HOMMA_BUS_Target_e eSender, HOMMA_BUS_Msg_t *pstMsg )
{

	switch ( pstMsg->eDataType )
	{
		case eBUS_DATATYPE_IPC_SEDN :
			{
				HOMMA_BUS_IpcSend_t *pIpcSend = pstMsg->pvData;
				if ( pIpcSend && pIpcSend->bMalloc && pIpcSend->pszJson )
				{
					HOM_MEMFREE( pIpcSend->pszJson );
					pIpcSend->pszJson = NULL;
				}
			}
			break;
		default :
			break;
	}

	/* 이 부분은 BUS에서 공통으로 처리하도록 할까?? */
	if ( pstMsg->bNeedFree && pstMsg->pvData )
	{
		HOM_MEMFREE( pstMsg->pvData );
		pstMsg->pvData = NULL;
	}

	return 0;
}

static HINT32	__hom_woonserver_bus_message_handler( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_Msg_t *pstMsg )
{
	switch ( pstMsg->eMsgType )
	{
		case eBUS_MSGTYPE_EVENT :
		//	if ( eBUS_TARGET_SYS == eSender )
				__hom_woonserver_sys_event_handle( eSender,  pstMsg->uEnum.eEvt, pstMsg->eDataType, pstMsg->pvData );
			break;

		case eBUS_MSGTYPE_REQUEST :
			if ( eBUS_REQUEST_JSON_DECODE == pstMsg->uEnum.eReq && eBUS_DATATYPE_IPC_RECV == pstMsg->eDataType )
				__hom_woonserver_cmd_process( eSender, pstMsg->pvData );
			break;

		case eBUS_MSGTYPE_RESPONSE :
			break;

		default :
			break;
	}
	return 0;
}

#if defined(CONFIG_HOMMA_WOON)
static HERROR	__hom_woonserver_send_notify(HINT32 cmd, HCHAR *pData, HUINT32	size)
{
	HOMMA_BUS_Msg_t 			busMsg;
	HOM_WoonServer_Context_t 	*pCtx = __ctx();
	HOMMA_BUS_IpcSend_t 		*pMsgData = (HOMMA_BUS_IpcSend_t *)HLIB_STD_MemAlloc( sizeof(HOMMA_BUS_IpcSend_t) );

	HxSTD_MemSet(&busMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));

	pMsgData->pszJson = pData;
	pMsgData->bMalloc = TRUE;
	pMsgData->nOpCode = 1;
	pMsgData->cmd = cmd;
	pMsgData->size = size;

	busMsg.eMsgType 	= eBUS_MSGTYPE_RESPONSE;
	busMsg.uEnum.eRes 	= eBUS_REQUEST_SERVER_SENDING;
	busMsg.eDataType 	= eBUS_DATATYPE_IPC_SEDN;
	busMsg.pvData 		= pMsgData;
	busMsg.nDataSize 	= sizeof(HOMMA_BUS_IpcSend_t);
	busMsg.bNeedFree 	= TRUE;

	HOMMA_BUS_SendMsg( eBUS_TARGET_SERVER, pCtx->hMsgBusHandle, &busMsg );

	return ERR_OK;
}

static void __hom_woonserver_callback_process( WOONS_Event_e eEvent, HINT32 p1, HINT32 p2, HINT32 p3 )
{
	HOM_WoonServer_Context_t 	*pCtx = __ctx();
	HOMMA_BUS_Msg_t 			stMsg;

	HxSTD_MemSet(&stMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));
	stMsg.eMsgType = eBUS_MSGTYPE_EVENT;

	switch ( eEvent )
	{
		case WOONSERVER_STARTED:
			{
				HOMxWoonServer_Event_Started_t  *cb_data  = NULL;
				cb_data = (HOMxWoonServer_Event_Started_t *)HLIB_STD_MemCalloc(sizeof(HOMxWoonServer_Event_Started_t));

				cb_data->isStarted = 1;

				stMsg.uEnum.eEvt = eBUS_EVENT_MOD_START;
				HOMMA_BUS_SendMsg( eBUS_TARGET_ALL, pCtx->hMsgBusHandle, &stMsg );

				HxLOG_Info("[%s:%d] #### [EVT] WoonnServer Start ##### \n", __MODULE__, __LINE__);

				__hom_woonserver_send_notify(eHOMAPI_WoonServer_Event_Start, (HCHAR *)cb_data, sizeof(HOMxWoonServer_Event_Started_t));
			}
			break;

		case WOONSERVER_STOPPED:
			{
				HOMxWoonServer_Event_Stopped_t *cb_data = NULL;
				cb_data = (HOMxWoonServer_Event_Stopped_t *)HLIB_STD_MemCalloc(sizeof(HOMxWoonServer_Event_Stopped_t));
				cb_data->isStopped = 1;

				stMsg.uEnum.eEvt = eBUS_EVENT_MOD_STOP;
				HOMMA_BUS_SendMsg( eBUS_TARGET_ALL, pCtx->hMsgBusHandle, &stMsg );

				HxLOG_Info("[%s:%d] #### [EVT] WoonnServer Stop ##### \n", __MODULE__, __LINE__);

				__hom_woonserver_send_notify(eHOMAPI_WoonServer_Event_Stop, (HCHAR *)cb_data, sizeof(HOMxWoonServer_Event_Started_t));
			}
			break;

		case WOONSERVER_RENAMED:
			{
				HOMxWoonServer_Event_Renamed_t *cb_data = NULL;
				cb_data = (HOMxWoonServer_Event_Renamed_t *)HLIB_STD_MemCalloc(sizeof(HOMxWoonServer_Event_Renamed_t));
				cb_data->isRenamed = 1;

				HxLOG_Info("[%s:%d] #### [EVT] WoonnServer Rename ##### \n", __MODULE__, __LINE__);

				__hom_woonserver_send_notify(eHOMAPI_WoonServer_Event_Renamed, (HCHAR *)cb_data, sizeof(HOMxWoonServer_Event_Renamed_t));
			}
			break;

		case WOONSERVER_TESTCONNECTION_RESULT:
			{
				HOMxWoonServer_Event_ServerInfo_t *cb_data = NULL;
				WOON_SERVER_INFO_t	Info;
				HxSTD_MemSet(&Info, 0x00, sizeof(WOON_SERVER_INFO_t));

				cb_data = (HOMxWoonServer_Event_ServerInfo_t*)HLIB_STD_MemCalloc(sizeof(HOMxWoonServer_Event_ServerInfo_t));

				WOON_GetServerInfo(&Info);

				cb_data->nState = Info.nState;
				cb_data->nPubPort = Info.nPubPort;
				cb_data->nExtPort = Info.nExtPort;

				HxSTD_snprintf(cb_data->szLocalIp, sizeof(cb_data->szLocalIp), "%s", Info.szLocalIp);
				HxSTD_snprintf(cb_data->szExtIp, sizeof(cb_data->szExtIp), "%s", Info.szExtIp);
				HxSTD_snprintf(cb_data->szPubIp, sizeof(cb_data->szPubIp), "%s", Info.szPubIp);

				__hom_woonserver_send_notify(eHOMAPI_WoonServer_Event_TestConnection, (HCHAR *)cb_data, sizeof(HOMxWoonServer_Event_ServerInfo_t));

				HxLOG_Info("[%s:%d] #### [EVT] WoonnServer Test Connection Result ##### \n", __MODULE__, __LINE__);
			}
			break;

		case WOONSERVER_NOTIFY_PINCODE:
			{
				HOMxWoonServer_Event_PinCode_t *cb_data = NULL;
				cb_data = (HOMxWoonServer_Event_PinCode_t *)HLIB_STD_MemCalloc(sizeof(HOMxWoonServer_Event_PinCode_t));

				cb_data->bOpen = (HINT32)p1; /* refer __woons_evt_cb() of woons.c #97086 */

				HxSTD_snprintf(cb_data->szPinCode, sizeof(cb_data->szPinCode), "%s", (HCHAR *)p2);

				__hom_woonserver_send_notify(eHOMAPI_WoonServer_Event_PinCode, (HCHAR *)cb_data, sizeof(HOMxWoonServer_Event_PinCode_t));

				HxLOG_Info("[%s:%d] #### [EVT] WoonnServer Notify Pincode(%s) ##### \n", __MODULE__, __LINE__, (HCHAR *)p2);

			}
			break;

		default :
			break;
	}
}
#endif

#define ________________Public_Interfaces__________________________
void	HOM_WOONSERVER_Run( void )
{
	HOM_WoonServer_Context_t *pCtx = __ctx();
	HOMMA_BUS_HndFuncList_t stFuncList;

	HxSTD_MemSet(pCtx, 0x00, sizeof(HOM_WoonServer_Context_t));
	__hom_woonserver_init( pCtx );

	HxSTD_MemSet(&stFuncList, 0x00, sizeof(HOMMA_BUS_HndFuncList_t));
	stFuncList.cbMsgHandleFunc = __hom_woonserver_bus_message_handler;
	stFuncList.cbReqHandleFunc = __hom_woonserver_bus_request_handler;
	stFuncList.cbRelHandleFunc = __hom_woonserver_bus_release_handler;

	pCtx->hMsgBusHandle = HOMMA_BUS_GetHandle( eBUS_TARGET_MOD_WOONS);
	HOMMA_BUS_Register( pCtx->hMsgBusHandle, &stFuncList );

#if defined(CONFIG_HOMMA_WOON)
	WOONS_RegisterEventCallback( __hom_woonserver_callback_process );
#endif

	HxLOG_Warning("HOM_WOONSERVER Run!!!\n");
}
/*  end of file  */
