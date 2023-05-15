/*************************************************************************************************************
	File            : hom_dial.c
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
#include 	"dial.h"

#define ________________Private_Static_Variables___________________
static BUS_Handle s_stBusHnd = NULL;

#define ________________Internal_Typedef_and_Structure_____________
typedef enum
{
	DLNA_DIAL_Unknown,
	DLNA_DIAL_START,
	DLNA_DIAL_STOP,
	DLNA_DIAL_STATUS,

	DLNA_DIAL_APP_REGIST,
	DLNA_DIAL_APP_UNREGIST,

	DLNA_DIAL_ReqAppLaunch,
	DLNA_DIAL_ReqAppStop,
	DLNA_DIAL_ReqAppStatus,

	DLNA_DIAL_CmdEnd,
} DLNA_DIAL_Cmd_e;

#if defined(CONFIG_HOMMA_DIAL)
static HERROR	__hom_dial_send_notify(HINT32 cmd, void *pData, HUINT32	size);
#endif

#define ________________Internal_Functions_________________________
static HINT32	__hom_dial_bus_event( HOMMA_BUS_Event_e eEvt, HOMMA_BUS_DataType_e eDataType, void *pvData )
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
#if defined(CONFIG_HOMMA_DIAL)
			{
				HxLOG_Debug("[DIAL] Got network online event(%d)!\n", eEvt);
				if (DIAL_IsStart() == TRUE)
				{
					DIAL_SetApplicationURL();
				}
			}
#endif
			break;

		case eBUS_EVENT_SYS_NET_IF_OFFLINE:
#if defined(CONFIG_HOMMA_DIAL)
			{
				HxLOG_Debug("[DIAL] Got network offline event(%d)!\n", eEvt);
			}
#endif
			break;

		case eBUS_EVENT_SYS_NET_IF_SWITCHING:
#if defined(CONFIG_HOMMA_DIAL)
			{
				HxLOG_Debug("[DIAL] Got network switching event(%d)!\n", eEvt);
				if (DIAL_IsStart() == TRUE)
				{
					DIAL_SetApplicationURL();
				}
			}
#endif
			break;

		case eBUS_EVENT_SYS_POWER_OPERATION:
#if defined(CONFIG_HOMMA_DIAL)
			{
				HxLOG_Debug("[DIAL] Get Power Operation evt(%d)\n", eEvt);
				if (DIAL_IsStart() != TRUE)
				{
					DIAL_Init(NULL);
					__hom_dial_send_notify(eHOMAPI_DIAL_Event_Started, NULL, 0);
				}
				else
				{
					HxLOG_Debug("DIAL already started... \n");
				}
			}
#endif
			break;

		case eBUS_EVENT_SYS_POWER_SHUTDOWN: /* ePAMA_STATUS_Shutdown: Active Standby */
#if defined(CONFIG_HOMMA_DIAL)
			if (1) /* Dont stop DIAL for active-standby */
			{
				HxLOG_Error("DIAL : Got system event(%d) for active standby!\n", eEvt);
				if (DIAL_IsStart() == TRUE)
				{
					/* Fixed redmine #85676, #85677;
					   Also, shutdown DIAL when entering at active standby (DIAL is not DMS/WOONs, renderer) */
					DIAL_Deinit();
					__hom_dial_send_notify(eHOMAPI_DIAL_Event_Stopped, NULL, 0);
				}
				else
				{
					HxLOG_Debug("DIAL already stopped... \n");
				}
			}
#endif
			break;

//		case eBUS_EVENT_SYS_POWER_SHUTDOWNDONE: /* Fixed redmine #85676, #85677 */
		case eBUS_EVENT_SYS_POWER_REBOOT:
#if defined(CONFIG_HOMMA_DIAL)
			{
				HxLOG_Debug("[DIAL] Get Power Shutdown evt(%d)\n", eEvt);
				if (DIAL_IsStart() == TRUE)
				{
					DIAL_Deinit();
					__hom_dial_send_notify(eHOMAPI_DIAL_Event_Stopped, NULL, 0);
				}
				else
				{
					HxLOG_Debug("DIAL already stopped... \n");
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

#if defined(CONFIG_HOMMA_DIAL)
static HERROR	__hom_dial_send_notify(HINT32 cmd, void *pData, HUINT32	size)
{
	HOMMA_BUS_Msg_t busMsg;
	HOMMA_BUS_IpcSend_t *pMsgData = (HOMMA_BUS_IpcSend_t *)HLIB_STD_MemAlloc( sizeof(HOMMA_BUS_IpcSend_t) );

	if (pMsgData == NULL)
	{
		HxLOG_Error("Error> Out of memory!\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pMsgData, 0x00, sizeof(HOMMA_BUS_IpcSend_t));
	pMsgData->pszJson = pData;
	pMsgData->bMalloc = TRUE;
	pMsgData->nOpCode = 1;
	pMsgData->cmd = cmd;
	pMsgData->size = size;

	HxSTD_MemSet(&busMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));
	busMsg.eMsgType 	= eBUS_MSGTYPE_RESPONSE;
	busMsg.uEnum.eRes 	= eBUS_REQUEST_SERVER_SENDING;
	busMsg.eDataType 	= eBUS_DATATYPE_IPC_SEDN;
	busMsg.pvData 		= pMsgData;
	busMsg.nDataSize 	= sizeof(HOMMA_BUS_Data_t);
	busMsg.bNeedFree 	= TRUE;

	HOMMA_BUS_SendMsg( eBUS_TARGET_SERVER, s_stBusHnd, &busMsg );

	return ERR_OK;
}

static void		__hom_dial_event(DIAL_Event_e event, const HCHAR * app_name, void * data)
{
	switch(event)
	{
		case DIAL_APP_LAUNCH:
			{
				DIAL_EventInfo_t *app = HLIB_STD_MemCalloc(sizeof(DIAL_EventInfo_t));
				HxLOG_Print("[%s:%d] ### DIAL %s APP LAUNCH ### \n", __FUNCTION__, __LINE__, app_name);
				HxLOG_Print("[%s:%d] ### URL - %s \n", __FUNCTION__, __LINE__, (HCHAR *)data);
				HxSTD_snprintf(app->szApplicationName, sizeof(app->szApplicationName), "%s", app_name);
				HxSTD_snprintf(app->szUrl, sizeof(app->szUrl), "%s", (HCHAR *)data);
				__hom_dial_send_notify(eHOMAPI_DIAL_Event_AppLaunch, (void *)app, sizeof(DIAL_EventInfo_t));
			}
			break;

		case DIAL_APP_STATUS:
			{

			}
			break;

		case DIAL_APP_STOP:
			{
				DIAL_EventInfo_t *app = HLIB_STD_MemCalloc(sizeof(DIAL_EventInfo_t));
				HxLOG_Print("[%s:%d] ### DIAL %s APP STOP ### \n", __FUNCTION__, __LINE__, app_name);
				HxSTD_snprintf(app->szApplicationName, sizeof(app->szApplicationName), "%s", app_name);
				__hom_dial_send_notify(eHOMAPI_DIAL_Event_AppStop, (void *)app, sizeof(DIAL_EventInfo_t));
			}
			break;

		default:
			break;
	}
}
#endif
static HERROR s_send_response(HOMMA_BUS_Target_e eSender,
							  const HOMMA_BUS_IpcRecv_t *stJsonData,
							  HINT32 cmd,
							  HCHAR * jsonData,
							  HBOOL alloced,
							  void * data,
							  HINT32 size) {

	HOMMA_BUS_Msg_t busMsg;
	HOMMA_BUS_IpcSend_t * pMsgData = (HOMMA_BUS_IpcSend_t *)HLIB_STD_MemCalloc( sizeof(HOMMA_BUS_IpcSend_t));

	if (pMsgData == NULL) {
		HxLOG_Error("Error> out of memory!\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pMsgData, 0x0, sizeof(HOMMA_BUS_IpcSend_t));

	pMsgData->nPairId = stJsonData->nPairId;
	pMsgData->pszJson = jsonData;
	pMsgData->bMalloc = alloced;
	pMsgData->nClientHandle = stJsonData->nClientHandle;
	pMsgData->cmd = cmd;
	pMsgData->nOpCode = eBUS_CMD_NONE;
	pMsgData->data = data;
	pMsgData->size = size;

	HxSTD_MemSet(&busMsg, 0x0, sizeof(HOMMA_BUS_Msg_t));
	busMsg.eMsgType 	= eBUS_MSGTYPE_RESPONSE;
	busMsg.uEnum.eRes 	= eBUS_REQUEST_SERVER_SENDING;
	busMsg.eDataType 	= eBUS_DATATYPE_IPC_SEDN;
	busMsg.pvData 		= pMsgData;
	busMsg.nDataSize 	= sizeof(HOMMA_BUS_IpcSend_t);
	busMsg.bNeedFree 	= TRUE;

	HOMMA_BUS_SendMsg( eSender, s_stBusHnd, &busMsg );
	return ERR_OK;
}

static HERROR __hom_dial_decoder( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_IpcRecv_t *stJsonData )
{
	HINT32	nOper =	stJsonData->nOpCode;
	comm_msg_data_t *data = stJsonData->pvJson;

	if ( eBUS_CMD_GET == nOper )
	{
		HCHAR	*encode_str = NULL;

		switch (data->cmd) {
		case eHOMAPI_DIAL_Command_IsRunning:
			{
#if defined(CONFIG_HOMMA_DIAL)
				HBOOL running = DIAL_IsStart();
#else
				HBOOL running = FALSE;
#endif
				HINT32 size = 0;
				const HCHAR * runstatus = running ? "YES" : "NO";
				encode_str = HLIB_STD_StrDup(runstatus);
				size = HxSTD_StrLen(encode_str) + 1;
				return s_send_response(eSender, stJsonData,
									   data->cmd,
									   encode_str, TRUE,
									   (void*)encode_str, size);
			}
		case eHOMAPI_DIAL_Command_GetFriendlyName:
			{
				HINT32 size = 0;
				HCHAR friendlyName[1024] = {0,};
#if defined(CONFIG_HOMMA_DIAL)
				DIAL_GetFriendlyName(friendlyName, sizeof(friendlyName));
#endif
				encode_str = HLIB_STD_StrDup(friendlyName);
				size = HxSTD_StrLen(encode_str) + 1;
				return s_send_response(eSender, stJsonData,
									   data->cmd,
									   encode_str, TRUE,
									   (void*)encode_str, size);
			}
		case eHOMAPI_DIAL_Command_GetAdditionalDataUrl:
			{
				HINT32 size = 0;
				HCHAR additionalDataUrl[1024] = {0,};
#if defined(CONFIG_HOMMA_DIAL)
				HCHAR * appName = (HCHAR*)data->data;
				DIAL_GetAdditionalDataUrl(additionalDataUrl, sizeof(additionalDataUrl), appName);
#endif
				encode_str = HLIB_STD_StrDup(additionalDataUrl);
				size = HxSTD_StrLen(encode_str) + 1;
				return s_send_response(eSender, stJsonData,
									   data->cmd,
									   encode_str, TRUE,
									   (void*)encode_str, size);
			}
		default:
			if ( data->size == 0) {
				return s_send_response(eSender, stJsonData, data->cmd, encode_str, TRUE, NULL, 0);
			}
			break;
		}
	}

	/* Set */
	switch(data->cmd)
	{
		case eHOMAPI_DIAL_Command_Start:
			{
				HOMxDIAL_Args_Start_t *arg = (HOMxDIAL_Args_Start_t *) data->data;
				HxLOG_Print("[%s:%d] ###  DIAL START (%s) ###\n", __FUNCTION__, __LINE__, arg->szFriendlyName);

#if defined(CONFIG_HOMMA_DIAL)
				DIAL_Init(NULL);
				__hom_dial_send_notify(eHOMAPI_DIAL_Event_Started, NULL, 0);
#endif
			}
			break;

		case eHOMAPI_DIAL_Command_Stop:
			{
				HxLOG_Print("[%s:%d] ###  DIAL STOP ###\n", __FUNCTION__, __LINE__);
#if defined(CONFIG_HOMMA_DIAL)
				DIAL_Deinit();
				__hom_dial_send_notify(eHOMAPI_DIAL_Event_Stopped, NULL, 0);
#endif
			}
			break;

		case eHOMAPI_DIAL_Command_Rename:
			{
				HOMxDIAL_Args_Rename_t *arg = (HOMxDIAL_Args_Rename_t *) data->data;
				HxLOG_Print("[%s:%d] ###  DIAL RENAME (%s) ###\n", __FUNCTION__, __LINE__, arg->szFriendlyName);

#if defined(CONFIG_HOMMA_DIAL)
				DIAL_SetFriendlyName(arg->szFriendlyName);
#endif
			}
			break;


		case eHOMAPI_DIAL_Command_AppStatusUpdate:
			{
				HOMxDIAL_Args_AppStatus_t *app = (HOMxDIAL_Args_AppStatus_t *)data->data;
#if defined(CONFIG_HOMMA_DIAL)
				DIAL_ApplicationStatusUpdate(app->szApplicationName, app->nStatus);
#endif
				HxLOG_Print("[%s:%d] ###  DIAL(%s) STATUS(%d) ###\n", __FUNCTION__, __LINE__, app->szApplicationName, app->nStatus);
			}
			break;

		case eHOMAPI_DIAL_Command_AppRegist:
			{
				HOMxDIAL_Args_AppRegist_t *app = (HOMxDIAL_Args_AppRegist_t *)data->data;
#if defined(CONFIG_HOMMA_DIAL)
				DIAL_RegisterApplication(app->szApplicationName);
#endif
				HxLOG_Print("[%s:%d] ###  DIAL REGIST APP (%s) ###\n", __FUNCTION__, __LINE__, app->szApplicationName);
			}
			break;

		case eHOMAPI_DIAL_Command_AppUnregist:
			{
				HOMxDIAL_Args_AppUnregist_t *app = (HOMxDIAL_Args_AppUnregist_t *)data->data;
#if defined(CONFIG_HOMMA_DIAL)
				DIAL_UnregisterApplication(app->szApplicationName);
#endif
				HxLOG_Print("[%s:%d] ###  DIAL UNREGIST APP (%s) ###\n", __FUNCTION__, __LINE__, app->szApplicationName);
			}
			break;

		case eHOMAPI_DIAL_Command_SetCookie:
			{
#if defined(CONFIG_HOMMA_DIAL)
				HOMxDIAL_Args_Cookie_t * cookie = (HOMxDIAL_Args_Cookie_t *)data->data;

				// set cookie
				HxLOG_Print("[%s:%d] ###  DIAL SET COOKIE (%s) ###\n", __FUNCTION__, __LINE__, cookie);
				DIAL_SetCookie(cookie->cookie);
#endif
			}
			break;


		default:
			HxLOG_Error("[%s:%d] ### Unknown command (%s) ###\n", __FUNCTION__, __LINE__, data->cmd);
			break;
	}

	return ERR_OK;
}

static HINT32	__hom_dial_bus_request_handler( HOMMA_BUS_Request_e eReq, HOMMA_BUS_Data_t *pstRetData )
{
	return 0;
}

static HINT32	__hom_dial_bus_release_handler( HOMMA_BUS_Target_e eSender, HOMMA_BUS_Msg_t *pstMsg )
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

static HINT32	__hom_dial_bus_message_handler( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_Msg_t *pstMsg )
{
	switch ( pstMsg->eMsgType )
	{
		case eBUS_MSGTYPE_EVENT :
			__hom_dial_bus_event( pstMsg->uEnum.eEvt, pstMsg->eDataType, pstMsg->pvData );
			break;

		case eBUS_MSGTYPE_REQUEST :
			if ( eBUS_REQUEST_JSON_DECODE == pstMsg->uEnum.eReq && eBUS_DATATYPE_IPC_RECV == pstMsg->eDataType )
				__hom_dial_decoder( eSender, pstMsg->pvData );
			break;

		case eBUS_MSGTYPE_RESPONSE :
			break;

		default :
			break;
	}
	return 0;
}

#define ________________Public_Functions___________________________
void	HOM_DIAL_Run( void )
{
	HOMMA_BUS_HndFuncList_t stFuncList;
	HOMMA_BUS_Msg_t 		stMsg;

	HxSTD_MemSet(&stFuncList, 0x00, sizeof(HOMMA_BUS_HndFuncList_t));
	HxSTD_MemSet(&stMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));

	stFuncList.cbMsgHandleFunc = __hom_dial_bus_message_handler;
	stFuncList.cbReqHandleFunc = __hom_dial_bus_request_handler;
	stFuncList.cbRelHandleFunc = __hom_dial_bus_release_handler;

	s_stBusHnd = HOMMA_BUS_GetHandle( eBUS_TARGET_MOD_DIAL);
	HOMMA_BUS_Register( s_stBusHnd, &stFuncList );

#if defined(CONFIG_HOMMA_DIAL)
	DIAL_RegisterCallback(__hom_dial_event);
#endif

	stMsg.eMsgType = eBUS_MSGTYPE_EVENT;
	stMsg.uEnum.eEvt = 0;

	HOMMA_BUS_SendMsg( eBUS_TARGET_ALL, s_stBusHnd, &stMsg );
}
