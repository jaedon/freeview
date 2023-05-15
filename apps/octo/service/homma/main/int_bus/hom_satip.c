/*************************************************************************************************************
	File            : hom_satip.c
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

#include 	"common.h"
#include 	"hom_int.h"
#include 	"homma_int.h"
#include 	"homma_bus.h"

#include 	"homapi.h"
#include 	"satip.h"

#define ________________Private_Static_Variables___________________
static BUS_Handle s_stBusHnd = NULL;

#define ________________Internal_Typedef_and_Structure_____________
typedef enum
{
	DLNA_SATIP_Unknown,
	DLNA_SATIP_START,
	DLNA_SATIP_STOP,
	DLNA_SATIP_STATUS,

	DLNA_SATIP_CmdEnd,
} DLNA_SATIP_Cmd_e;

typedef struct
{
	HBOOL				bPrevStartFailed;
	HBOOL				bRequestStart;
	HCHAR 				*pszFriendlyName;
} HOM_SATIP_Context_t;

#define ________________Static_Variables___________________________
static HOM_SATIP_Context_t s_stHomSatipContext = { 0 };

#define ________________Internal_Functions_________________________
static	void	__hom_satip_init( HOM_SATIP_Context_t *pstHomSatipCtx)
{
	pstHomSatipCtx->bPrevStartFailed = FALSE;
	pstHomSatipCtx->bRequestStart = FALSE;
	pstHomSatipCtx->pszFriendlyName = NULL;
}

static HINT32	__hom_satip_bus_event( HOMMA_BUS_Target_e eSender, HOMMA_BUS_DataType_e eDataType, void *pvData )
{
	return ERR_OK;
}
static HOM_SATIP_Context_t * __hom_satip_getContext( void )
{
	return &s_stHomSatipContext;
}

#if defined(CONFIG_HOMMA_SATIP)
static HERROR	__hom_satip_request_start( const HCHAR * pszFriendlyName )
{
	HERROR	retErr = ERR_FAIL;
	SATIP_Config_t stConfig;
	HOMMA_BUS_Data_t retData;
	CO_NET_Info_t *pNetInfo = NULL;
	CO_STORAGE_Info_t *pStoInfo = NULL;

	HxSTD_MemSet(&stConfig, 0x00, sizeof(SATIP_Config_t));
	HxSTD_MemSet(&retData, 0x00, sizeof(HOMMA_BUS_Data_t));

	HOMMA_BUS_Request( eBUS_TARGET_SYS, eBUS_REQUEST_NET_INFO, &retData, &retErr );
	if ( ERR_OK != retErr )
	{
		HxLOG_Error("[%s:%d] Error> network information request is faild. \n", __func__, __LINE__);
		return ERR_FAIL;
	}
	if ( eBUS_DATATYPE_USER_DEF <= retData.eDataType )
	{
		pNetInfo = (CO_NET_Info_t *)retData.pvData;
		if ( 0 == pNetInfo->nDevCnt )
		{
			HxLOG_Error("[%s:%d] Error> avaiable network interface is not existed. \n", __func__, __LINE__);
			return ERR_FAIL;
		}
	}
	SATIP_SetNetworkInfo( pNetInfo );

	HxSTD_StrNCpy( stConfig.szFriendlyName, pszFriendlyName, sizeof(stConfig.szFriendlyName)-1 );
	retErr = SATIP_Start( &stConfig );
	if ( ERR_OK != retErr )
	{
		HxLOG_Error( "[HOM_SATIP] Error> Start Failed!!!!\n" );
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR	__hom_satip_send_notify(HINT32 cmd, void *pData, HUINT32	size)
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

static HINT32	__hom_satip_sys_event_handle( HOMMA_BUS_Event_e eEvent, HOMMA_BUS_DataType_e eDataType, void *pvData )
{
#if defined(CONFIG_HOMMA_SATIP)
	HERROR	hError = ERR_OK;
	HOM_SATIP_Context_t *pHomSatipCtx = __hom_satip_getContext();
#endif
	HxLOG_Error("[ HOM_SATIP ] ++system event:%d\n", eEvent);

#if defined(CONFIG_HOMMA_SATIP)
	if (pHomSatipCtx->bPrevStartFailed == TRUE && pHomSatipCtx->bRequestStart == TRUE)
	{
		if(SATIP_IsStart() == FALSE)
		{
			HxLOG_Debug("[%s:%d] Info> Retry SATIP start() because of prev failure(%d)!\n", __func__, __LINE__, pHomSatipCtx->bPrevStartFailed);
			hError = __hom_satip_request_start(pHomSatipCtx->pszFriendlyName);
			if (hError == ERR_OK)
			{
				pHomSatipCtx->bPrevStartFailed = FALSE;
			}
			else
			{
				HxLOG_Error("[ HOM_SATIP ] Error> failed to re-start SATIP!\n");
			}
		}
		return ERR_OK; /* event 처리없이 return */
	}

//eBUS_EVENT_SYS_POWER_OPERATION = 30,

	if(eEvent == eBUS_EVENT_SYS_POWER_SHUTDOWN)
		if(SATIP_IsStart() == TRUE)
			pHomSatipCtx->bPrevStartFailed = TRUE;



	return SATIP_OnSystemEvent( eEvent, (void *)eDataType, pvData );
#else
	return ERR_OK;
#endif
}

static HINT32	__hom_satip_event( SATIP_EVENT_e eEvent, HINT32 p1, HINT32 p2, HINT32 p3 )
{
	switch(eEvent)
	{
		case eSATIP_EVT_STOP:
		case eSATIP_EVT_START:
		case eSATIP_EVT_READY:
			break;

		default:
			break;
	}

	return 0;
}
#endif /* CONFIG_HOMMA_SATIP */

static HERROR __hom_satip_decoder( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_IpcRecv_t *stJsonData )
{
	HERROR	hError = ERR_OK;
	HOM_SATIP_Context_t *pHomSatipCtx = __hom_satip_getContext();
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
		case eHOMAPI_SATIP_Command_Start:
			{
				HOMxSATIP_Args_Start_t *arg = (HOMxSATIP_Args_Start_t *) data->data;
				HxLOG_Debug("[ HOM_SATIP ] ==============================================\n");
				HxLOG_Debug("[ HOM_SATIP ] ===========> REQ SATIP START : %s \n", (HCHAR *)arg->szFriendlyName);
				HxLOG_Debug("[ HOM_SATIP ] ==============================================\n");
				pHomSatipCtx->bRequestStart = TRUE; /* SATIP start() failture 이후 Wait 상태에서 start_request 문제된다 */

				if (pHomSatipCtx->pszFriendlyName != NULL)
					{
					HOM_MEMFREE(pHomSatipCtx->pszFriendlyName);
					pHomSatipCtx->pszFriendlyName= NULL;
				}
				pHomSatipCtx->pszFriendlyName = HLIB_STD_StrDup((HCHAR *)arg->szFriendlyName);

#if defined(CONFIG_HOMMA_SATIP)
				hError = __hom_satip_request_start( (HCHAR *)arg->szFriendlyName);
				__hom_satip_send_notify(eHOMAPI_SATIP_Event_Started, NULL, 0);
#endif
				if (hError != ERR_OK)
				{
					HxLOG_Error("[ HOM_SATIP ] Error> failed to start SATIP!\n");
					pHomSatipCtx->bPrevStartFailed = TRUE;
				}
			}
			break;

		case eHOMAPI_SATIP_Command_Stop:
			{
				HxLOG_Debug("[ HOM_SATIP] ==============================================\n");
				HxLOG_Debug("[ HOM_SATIP] ===========> REQ SATIP STOP     ==============\n");
				HxLOG_Debug("[ HOM_SATIP] ==============================================\n");
				pHomSatipCtx->bRequestStart = FALSE;
				pHomSatipCtx->bPrevStartFailed = FALSE;
#if defined(CONFIG_HOMMA_SATIP)
				SATIP_Stop();
				__hom_satip_send_notify(eHOMAPI_SATIP_Event_Stopped, NULL, 0);
#endif
				if (pHomSatipCtx->pszFriendlyName != NULL)
				{
					HOM_MEMFREE(pHomSatipCtx->pszFriendlyName);
					pHomSatipCtx->pszFriendlyName= NULL;
				}
			}
			break;

		case eHOMAPI_SATIP_Command_Rename:
			{
				HOMxSATIP_Args_Rename_t *arg = (HOMxSATIP_Args_Rename_t *) data->data;
				HxLOG_Debug("[ HOM_SATIP] ==============================================\n");
				HxLOG_Debug("[ HOM_SATIP] ===========> REQ SATIP RENAME : %s \n", (HCHAR *)arg->szFriendlyName);
				HxLOG_Debug("[ HOM_SATIP] ==============================================\n");
#if defined(CONFIG_HOMMA_SATIP)
				SATIP_SetFriendlyName(arg->szFriendlyName);
#endif
				if (pHomSatipCtx->pszFriendlyName != NULL)
				{
					HOM_MEMFREE(pHomSatipCtx->pszFriendlyName);
					pHomSatipCtx->pszFriendlyName= NULL;
				}
				pHomSatipCtx->pszFriendlyName = HLIB_STD_StrDup((HCHAR *)arg->szFriendlyName);
			}

		default:
			break;
	}

	return ERR_OK;
}

static HINT32	__hom_satip_bus_request_handler( HOMMA_BUS_Request_e eReq, HOMMA_BUS_Data_t *pstRetData )
{
	return 0;
}

static HINT32	__hom_satip_bus_release_handler( HOMMA_BUS_Target_e eSender, HOMMA_BUS_Msg_t *pstMsg )
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

static HINT32	__hom_satip_bus_message_handler( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_Msg_t *pstMsg )
{
	switch ( pstMsg->eMsgType )
	{
		case eBUS_MSGTYPE_EVENT :
#if defined(CONFIG_HOMMA_SATIP)
			if ( eBUS_TARGET_SYS == eSender )
			{
				__hom_satip_sys_event_handle( pstMsg->uEnum.eEvt, pstMsg->eDataType, pstMsg->pvData );
			}
#endif
			break;

		case eBUS_MSGTYPE_REQUEST :
			if ( eBUS_REQUEST_JSON_DECODE == pstMsg->uEnum.eReq && eBUS_DATATYPE_IPC_RECV == pstMsg->eDataType )
				__hom_satip_decoder( eSender, pstMsg->pvData );
			break;

		case eBUS_MSGTYPE_RESPONSE :
			break;

		default :
			break;
	}
	return 0;
}

#define ________________Public_Functions___________________________
void	HOM_SATIP_Run( void )
{
	HOM_SATIP_Context_t 		*pHomSatipCtx = __hom_satip_getContext();
	HOMMA_BUS_HndFuncList_t stFuncList;
	HOMMA_BUS_Msg_t 		stMsg;

	HxSTD_MemSet(&stFuncList, 0x00, sizeof(HOMMA_BUS_HndFuncList_t));
	HxSTD_MemSet(&stMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));

	__hom_satip_init(pHomSatipCtx);

	stFuncList.cbMsgHandleFunc = __hom_satip_bus_message_handler;
	stFuncList.cbReqHandleFunc = __hom_satip_bus_request_handler;
	stFuncList.cbRelHandleFunc = __hom_satip_bus_release_handler;

	s_stBusHnd = HOMMA_BUS_GetHandle( eBUS_TARGET_MOD_SATIP);
	HOMMA_BUS_Register( s_stBusHnd, &stFuncList );

#if defined(CONFIG_HOMMA_SATIP)
	SATIP_SetEvtListener(__hom_satip_event);
#endif

	stMsg.eMsgType = eBUS_MSGTYPE_EVENT;
	stMsg.uEnum.eEvt = 0;

	HOMMA_BUS_SendMsg( eBUS_TARGET_ALL, s_stBusHnd, &stMsg );
}
