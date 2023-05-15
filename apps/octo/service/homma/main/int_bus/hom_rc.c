/*************************************************************************************************************
	File            : hom_rc.c
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

	THE SOFTWARE IS PROVIDED ¢®¡ÆAS IS¢®¡¾ AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES,
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

#include 	"common.h"
#include 	"hom_int.h"
#include 	"homma_int.h"
#include 	"homma_bus.h"

#include 	"homapi.h"
#include 	"rc.h"

#define ________________Private_Static_Variables___________________
static BUS_Handle s_stBusHnd = NULL;

#define ________________Internal_Typedef_and_Structure_____________
typedef enum
{
	DLNA_RC_Unknown,
	DLNA_RC_START,
	DLNA_RC_STOP,
	DLNA_RC_STATUS,

	DLNA_RC_APP_REGIST,
	DLNA_RC_APP_UNREGIST,

	DLNA_RC_ReqAppLaunch,
	DLNA_RC_ReqAppStop,
	DLNA_RC_ReqAppStatus,

	DLNA_RC_CmdEnd,
} DLNA_RC_Cmd_e;

#if defined(CONFIG_HOMMA_RC)
static HERROR	__hom_rc_send_notify(HINT32 cmd, void *pData, HUINT32	size);
#endif

#define ________________Internal_Functions_________________________
static HINT32	__hom_rc_sys_event_handle( HOMMA_BUS_Event_e eEvt, HOMMA_BUS_DataType_e eDataType, void *pvData )
{
	switch(eEvt)
	{
		case eBUS_EVENT_MOD_START:
			break;
		case eBUS_EVENT_MOD_STOP:
			break;
		case eBUS_EVENT_SYS_FILE_COPY_DONE:
		case eBUS_EVENT_SYS_FILE_DELETE_DONE:
		case eBUS_EVENT_SYS_FILE_CREATE_DONE:
		case eBUS_EVENT_SYS_REC_ADDED:
		case eBUS_EVENT_SYS_REC_DELETED: /* record file will be process by FILE_DELETE_DONE */
		case eBUS_EVENT_SYS_REC_UPDATED:
		case eBUS_EVENT_SYS_PW_STANDBY:
		case eBUS_EVENT_SYS_FS_MOUNTED:
			break;

		case eBUS_EVENT_SYS_NET_IF_ONLINE:
#if 0 //defined(CONFIG_HOMMA_RC)
			{
				HxLOG_Debug("[RC] Got network online event(%d)!\n", eEvt);
				if (RC_IsStart() == TRUE)
				{
					RC_SetApplicationURL();
				}
#if 0
				else
				{
					/* not confirmed and network change for dial already done at mxlib(dial upnp stack) */
					RC_Init(NULL);
					__hom_dial_send_notify(eHOMAPI_DIAL_Event_Started, NULL, 0);
					HxLOG_Debug("DIAL started\n");
				}
#endif
			}
#endif
			break;

		case eBUS_EVENT_SYS_NET_IF_OFFLINE:
#if defined(CONFIG_HOMMA_RC)
			{
				HxLOG_Debug("[RC] Got network offline event(%d)!\n", eEvt);
#if 0
				/* not confirmed and network change for dial already done at mxlib(dial upnp stack) */
				if (DIAL_IsStart() == TRUE)
				{
					DIAL_Deinit();
					__hom_dial_send_notify(eHOMAPI_DIAL_Event_Stopped, NULL, 0);
					HxLOG_Debug("DIAL Stopped\n");
				}
#endif

			}
#endif
			break;

		case eBUS_EVENT_SYS_NET_IF_SWITCHING:
#if 0 //defined(CONFIG_HOMMA_RC)
			{
				HxLOG_Debug("[RC] Got network switching event(%d)!\n", eEvt);
				if (RC_IsStart() == TRUE)
				{
					RC_SetApplicationURL();
#if 0
					/* not confirmed and network change for dial already done at mxlib(dial upnp stack) */
					/* Dont send DIAL started/stoppped event */
					DIAL_Deinit();
					HxLOG_Debug("DIAL Stopped\n");

					RC_Init(NULL);
					HxLOG_Debug("DIAL Restarted\n");
#endif
				}
			}
#endif
			break;

		case eBUS_EVENT_SYS_POWER_OPERATION:
#if defined(CONFIG_HOMMA_RC)
			{
				HxLOG_Debug("[RC] Get Power Operation evt(%d)\n", eEvt);
				if (RC_IsStart() != TRUE)
				{
					RC_Init(NULL);
					__hom_rc_send_notify(eHOMAPI_RC_Event_Started, NULL, 0);
				}
				else
				{
					HxLOG_Debug("RC already started... \n");
				}

				RC_SetPowerStatus(TRUE);
			}
#endif
			break;

		case eBUS_EVENT_SYS_POWER_SHUTDOWN: /* ePAMA_STATUS_Shutdown: Active Standby */
#if 0 //defined(CONFIG_HOMMA_RC)
			{
				HxLOG_Error("RC : Got system event(%d) for active standby!\n", eEvt);
				if (RC_IsStart() == TRUE)
				{
					/* Fixed redmine #85676, #85677;
					   Also, shutdown DIAL when entering at active standby (DIAL is not DMS/WOONs, renderer) */
					RC_Deinit();
					__hom_rc_send_notify(eHOMAPI_RC_Event_Stopped, NULL, 0);
				}
				else
				{
					HxLOG_Debug("RC already stopped... \n");
				}
				RC_SetPowerStatus(FALSE);
			}
#else
			RC_SetPowerStatus(FALSE);
#endif
			break;

		case eBUS_EVENT_SYS_POWER_SHUTDOWNDONE: /* Fixed redmine #85676, #85677 */
		case eBUS_EVENT_SYS_POWER_REBOOT:
#if 0//defined(CONFIG_HOMMA_RC)
			{
				HxLOG_Debug("[RC] Get Power Shutdown evt(%d)\n", eEvt);
				if (RC_IsStart() == TRUE)
				{
					RC_Deinit();
					__hom_rc_send_notify(eHOMAPI_RC_Event_Stopped, NULL, 0);
				}
				else
				{
					HxLOG_Debug("RC already stopped... \n");
				}
			}
#endif
			break;

		case eBUS_EVENT_SYS_MS_FORMAT_TRY:
		default:
			/* HxLOG_Print("[%s:%d] unknown evt(%d)\n", __FUNCTION__, __LINE__, eEvt); */
			break;
	}

	return ERR_OK;
}

#if defined(CONFIG_HOMMA_RC)
static HERROR	__hom_rc_send_notify(HINT32 cmd, void *pData, HUINT32 size)
{
	HOMMA_BUS_Msg_t busMsg;
	HOMMA_BUS_IpcSend_t *pMsgData = (HOMMA_BUS_IpcSend_t *)HLIB_STD_MemAlloc( sizeof(HOMMA_BUS_IpcSend_t) );

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
	busMsg.nDataSize 	= sizeof(HOMMA_BUS_Data_t);
	busMsg.bNeedFree 	= TRUE;

	HOMMA_BUS_SendMsg( eBUS_TARGET_SERVER, s_stBusHnd, &busMsg );

	return ERR_OK;
}

static void		__hom_rc_event(RC_Event_e event, void *pData1, void *pData2) //Data : [message |url | server name |client name]
{
	switch(event)
	{
		case RC_CLIENT_PAIRED:
			{
				RC_EventInfo_t *pstRcInfo = HLIB_STD_MemCalloc(sizeof(RC_EventInfo_t));

				if(NULL == pData2)
				{
					pstRcInfo->szName[0] = '\0';
				}
				else
				{
					HxSTD_snprintf(pstRcInfo->szName, sizeof(pstRcInfo->szName), "%s", (HCHAR *)pData2);
				}

				if(NULL == pData1)
				{
					pstRcInfo->szData[0] = '\0';
				}
				else
				{
					HxSTD_snprintf(pstRcInfo->szData, sizeof(pstRcInfo->szData), "%s", (HCHAR *)pData1);
				}

				HxLOG_Error("[%s:%d] ### RC_CLIENT_PAIRED: %s / %s \n", __FUNCTION__, __LINE__, pstRcInfo->szData, pstRcInfo->szName);
				__hom_rc_send_notify(eHOMAPI_RC_Event_Paired, (void *)pstRcInfo, sizeof(RC_EventInfo_t));
			}
			break;

		case RC_CLIENT_UNPAIRED:
			{
				RC_EventInfo_t *pstRcInfo = HLIB_STD_MemCalloc(sizeof(RC_EventInfo_t));

				if(NULL == pData1)
				{
					pstRcInfo->szData[0] = '\0';
				}
				else
				{
					HxSTD_snprintf(pstRcInfo->szData, sizeof(pstRcInfo->szData), "%s", (HCHAR *)pData1);
				}

				HxLOG_Error("[%s:%d] ### RC_CLIENT_UNPAIRED: %s \n", __FUNCTION__, __LINE__, pstRcInfo->szData);
				__hom_rc_send_notify(eHOMAPI_RC_Event_Unpaired, (void *)pstRcInfo, sizeof(RC_EventInfo_t));
			}
			break;

		case RC_CLIENT_MESSAGE:
			{
				RC_EventInfo_t *pstRcInfo = HLIB_STD_MemCalloc(sizeof(RC_EventInfo_t));

				if(NULL == pData1)
				{
					pstRcInfo->szData[0] = '\0';
				}
				else
				{
					HxSTD_snprintf(pstRcInfo->szData, sizeof(pstRcInfo->szData), "%s", (HCHAR *)pData1);
				}

				HxLOG_Error("[%s:%d] ### RC_CLIENT_MESSAGE: %s \n", __FUNCTION__, __LINE__, pstRcInfo->szData);
				__hom_rc_send_notify(eHOMAPI_RC_Event_Message, (void *)pstRcInfo, sizeof(RC_EventInfo_t));
			}
			break;

		case RC_LAUNCH_APP:
			{
				RC_EventInfo_t *pstRcInfo = HLIB_STD_MemCalloc(sizeof(RC_EventInfo_t));

				if(NULL != pData1)
				{
					HxSTD_snprintf(pstRcInfo->szData, sizeof(pstRcInfo->szData), "%s", (HCHAR *)pData1);

					HxLOG_Error("[%s:%d] ### RC_LAUNCH_APP: %s \n", __FUNCTION__, __LINE__, pstRcInfo->szData);
					__hom_rc_send_notify(eHOMAPI_RC_Event_LaunchApp, (void *)pstRcInfo, sizeof(RC_EventInfo_t));
				}
			}
			break;

		case RC_SET_FRIENDLY_NAME:
			{
				RC_EventInfo_t *pstRcInfo = HLIB_STD_MemCalloc(sizeof(RC_EventInfo_t));

				if(NULL == pData1)
				{
					pstRcInfo->szName[0] = '\0';
				}
				else
				{
					HxSTD_snprintf(pstRcInfo->szName, sizeof(pstRcInfo->szName), "%s", (HCHAR *)pData1);
				}

				HxLOG_Error("[%s:%d] ### RC_SET_FRIENDLY_NAME: %s \n", __FUNCTION__, __LINE__, pstRcInfo->szName);
				__hom_rc_send_notify(eHOMAPI_RC_Event_SetFriendlyName, (void *)pstRcInfo, sizeof(RC_EventInfo_t));
			}
			break;

		default:
			break;
	}
}
#endif

static HERROR __hom_rc_cmd_process( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_IpcRecv_t *stJsonData )
{
	HINT32	nOper =	stJsonData->nOpCode;
	comm_msg_data_t *data = stJsonData->pvJson;

	if ( eBUS_CMD_GET == nOper )
	{
		HCHAR	*endcode_str = NULL;

		if ( data->size == 0)
		{
			HOMMA_BUS_Msg_t 	busMsg;
			HOMMA_BUS_IpcSend_t *pMsgData = (HOMMA_BUS_IpcSend_t *)HLIB_STD_MemCalloc( sizeof(HOMMA_BUS_IpcSend_t));
			HxSTD_MemSet(&busMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));

			pMsgData->nPairId = stJsonData->nPairId ;
			pMsgData->pszJson = endcode_str ;
			pMsgData->bMalloc = TRUE;
			pMsgData->nClientHandle = stJsonData->nClientHandle;

			busMsg.eMsgType 	= eBUS_MSGTYPE_RESPONSE;
			busMsg.uEnum.eRes 	= eBUS_REQUEST_SERVER_SENDING;
			busMsg.eDataType 	= eBUS_DATATYPE_IPC_SEDN;
			busMsg.pvData 		= pMsgData;
			busMsg.nDataSize 	= sizeof(HOMMA_BUS_IpcSend_t);
			busMsg.bNeedFree 	= TRUE;

			HOMMA_BUS_SendMsg( eSender, s_stBusHnd, &busMsg );
			return ERR_OK;
		}
	}

	/* Set */
	switch(data->cmd)
	{
		case eHOMAPI_RC_Command_Start:
			{
				HOMxRC_Args_Start_t *arg = (HOMxRC_Args_Start_t *) data->data;
				HxLOG_Print("[%s:%d] ###  RC START (%s) ###\n", __FUNCTION__, __LINE__, arg->szFriendlyName);

#if defined(CONFIG_HOMMA_RC)
				RC_Init(arg->szFriendlyName);
				__hom_rc_send_notify(eHOMAPI_RC_Event_Started, NULL, 0);
#endif
			}
			break;

		case eHOMAPI_RC_Command_Stop:
			{
				HxLOG_Print("[%s:%d] ###  RC STOP ###\n", __FUNCTION__, __LINE__);
#if defined(CONFIG_HOMMA_RC)
				RC_Deinit();
				__hom_rc_send_notify(eHOMAPI_RC_Event_Stopped, NULL, 0);
#endif
			}
			break;

		case eHOMAPI_RC_Command_Rename:
			{
				HOMxRC_Args_Rename_t *arg = (HOMxRC_Args_Rename_t *) data->data;
				HxLOG_Print("[%s:%d] ###  RC RENAME (%s) ###\n", __FUNCTION__, __LINE__, arg->szFriendlyName);

#if defined(CONFIG_HOMMA_RC)
				RC_SetFriendlyName(arg->szFriendlyName);
#endif
			}
			break;

		case eHOMAPI_RC_Command_Unpair:
			{
				HxLOG_Print("[%s:%d] ###  RC Unpair Client ###\n", __FUNCTION__, __LINE__);
#if defined(CONFIG_HOMMA_RC)
				RC_UnpairClient();
				__hom_rc_send_notify(eHOMAPI_RC_Event_Unpaired, NULL, 0);
#endif
			}
			break;

		default:
			break;
	}

	return ERR_OK;
}

static HINT32	__hom_rc_bus_request_handler( HOMMA_BUS_Request_e eReq, HOMMA_BUS_Data_t *pstRetData )
{
	return 0;
}

static HINT32	__hom_rc_bus_release_handler( HOMMA_BUS_Target_e eSender, HOMMA_BUS_Msg_t *pstMsg )
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

	if ( pstMsg->bNeedFree && pstMsg->pvData )
	{
		HOM_MEMFREE( pstMsg->pvData );
		pstMsg->pvData = NULL;
	}

	return 0;
}

static HINT32	__hom_rc_bus_message_handler( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_Msg_t *pstMsg )
{
	switch ( pstMsg->eMsgType )
	{
		case eBUS_MSGTYPE_EVENT :
			__hom_rc_sys_event_handle( pstMsg->uEnum.eEvt, pstMsg->eDataType, pstMsg->pvData );
			break;

		case eBUS_MSGTYPE_REQUEST :
			if ( eBUS_REQUEST_JSON_DECODE == pstMsg->uEnum.eReq && eBUS_DATATYPE_IPC_RECV == pstMsg->eDataType )
				__hom_rc_cmd_process( eSender, pstMsg->pvData );
			break;

		case eBUS_MSGTYPE_RESPONSE :
			break;

		default :
			break;
	}
	return 0;
}

#define ________________Public_Functions___________________________
void	HOM_RC_Run( void )
{
	HOMMA_BUS_HndFuncList_t stFuncList;
	HOMMA_BUS_Msg_t 		stMsg;

	HxSTD_MemSet(&stFuncList, 0x00, sizeof(HOMMA_BUS_HndFuncList_t));
	HxSTD_MemSet(&stMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));

	stFuncList.cbMsgHandleFunc = __hom_rc_bus_message_handler;
	stFuncList.cbReqHandleFunc = __hom_rc_bus_request_handler;
	stFuncList.cbRelHandleFunc = __hom_rc_bus_release_handler;

	s_stBusHnd = HOMMA_BUS_GetHandle( eBUS_TARGET_MOD_RC);
	HOMMA_BUS_Register( s_stBusHnd, &stFuncList );

#if defined(CONFIG_HOMMA_RC)
	RC_RegisterCallback(__hom_rc_event);
#endif

	stMsg.eMsgType = eBUS_MSGTYPE_EVENT;
	stMsg.uEnum.eEvt = 0;

	HOMMA_BUS_SendMsg( eBUS_TARGET_ALL, s_stBusHnd, &stMsg );
}
