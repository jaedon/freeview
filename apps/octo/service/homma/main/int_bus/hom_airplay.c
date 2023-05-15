/*************************************************************************************************************
	File            : hom_airplay.c
	author          : parkjh4@humaxdigital.com
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
#include 	"homma_ipc_decode.h"

#include 	"airplay.h"
#define __MODULE__	"HOM:AIRPLAY"

#define ________________Private_Mecro_and_Defines__________________
#define	AIRPLAY_SERVICE_PORT (80)

#define ________________Private_Static_Variables___________________
static BUS_Handle s_stBusHnd = NULL;

#define ________________Internal_Functions_________________________

static HERROR	__hom_airplay_send_notify(HINT32 cmd, HCHAR *pData, HUINT32	size)
{
	HOMMA_BUS_Msg_t 	busMsg;
	HOMMA_BUS_IpcSend_t *pMsgData = (HOMMA_BUS_IpcSend_t *)HLIB_STD_MemCalloc(sizeof(HOMMA_BUS_IpcSend_t));

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

	HOMMA_BUS_SendMsg( eBUS_TARGET_SERVER, s_stBusHnd, &busMsg );

	return ERR_OK;
}

static HINT32	__hom_airplay_bus_event( HOMMA_BUS_Event_e eEvt, HOMMA_BUS_DataType_e eDataType, void *pvData )
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
			if( AIRPLAY_IsStart() && AIRPLAY_NeedRestart() )
			{
				AIRPLAY_Stop();
			}
			if ( !AIRPLAY_IsStart() )
			{
				PL_AIRPLAY_SERVICE_PARAM_t * tServiceParam = NULL;
				HxLOG_Debug("Get IF Online evt(%d)\n", eEvt);
				/* if ServiceParam is NULL, it gonna be set default. */
				AIRPLAY_SetServiceParameter(tServiceParam);
				if( AIRPLAY_Start() != ERR_OK )
				{
					HxLOG_Error("Error> Failed to start airplay. ip address is not available\n");
				}
				else
				{
					HxLOG_Debug("AIRPLAY started\n");
				}
			}
			else
			{
				HxLOG_Debug("AIRPLAY Started already... \n");
			}
			break;
		case eBUS_EVENT_SYS_NET_IF_OFFLINE:
			HxLOG_Debug("Get IF Offline evt(%d)\n", eEvt);
			/* if ServiceParam is NULL, it gonna be set default. */
			if ( AIRPLAY_IsStart() )
			{
				AIRPLAY_Stop();
				HxLOG_Debug("AIRPLAY Stopped\n");
			}
			break;
		case eBUS_EVENT_SYS_NET_IF_SWITCHING:
			HxLOG_Debug("Get IF Switching evt(%d)\n", eEvt);
			/* if ServiceParam is NULL, it gonna be set default. */
			if ( AIRPLAY_IsStart() )
			{
				AIRPLAY_Stop();
				HxLOG_Debug("AIRPLAY Stopped\n");
			}

			{
				PL_AIRPLAY_SERVICE_PARAM_t * tServiceParam = NULL;
				AIRPLAY_SetServiceParameter(tServiceParam);
				if( AIRPLAY_Start() != ERR_OK )
				{
					HxLOG_Debug("Failed to restart airplay. ip address is not available\n");
				}
				else
				{
					HxLOG_Debug("AIRPLAY Restarted\n");
				}
			}
			break;
		case eBUS_EVENT_SYS_POWER_OPERATION:
			HxLOG_Debug("Get Power Operation evt(%d)\n", eEvt);
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
			if( AIRPLAY_IsStart() && AIRPLAY_NeedRestart() )
			{
				AIRPLAY_Stop();
			}
			if ( !AIRPLAY_IsStart())
			{
				PL_AIRPLAY_SERVICE_PARAM_t * tServiceParam = NULL;
				HxLOG_Debug("Get IF Online evt(%d)\n", eEvt);
				/* if ServiceParam is NULL, it gonna be set default. */
				AIRPLAY_SetServiceParameter(tServiceParam);
				if( AIRPLAY_Start() != ERR_OK )
				{
					HxLOG_Debug("Failed to start airplay. ip address is not available\n");
				}
				else
				{
					HxLOG_Debug("AIRPLAY started\n");
				}
			}
			else
			{
				HxLOG_Debug("AIRPLAY Started already... \n");
			}
#endif /* defined(CONFIG_SUPPORT_S3MODE_FASTBOOT) */
			break;

		case eBUS_EVENT_SYS_POWER_SHUTDOWN: /* ePAMA_STATUS_Shutdown: Active Standby */
			HxLOG_Error("AIRPLAY : Got system event(%d) for active standby!\n", eEvt);
			if (1) /* Dont stop airplay for active-standby */
			{
				if ( AIRPLAY_IsStart())
				{
					 /* Also, shutdown airplay/airtunes when entering at active standby (airplay is not DMS/WOONs, renderer) */
					AIRPLAY_Stop();
				}
				else
				{
					HxLOG_Error("AIRPLAY Stopped already... \n");
				}
			}
			break;

//		case eBUS_EVENT_SYS_POWER_SHUTDOWNDONE:
		case eBUS_EVENT_SYS_POWER_REBOOT:
			HxLOG_Debug("AIRPLAY : Get Power Shutdown evt(%d)\n", eEvt);
			if ( AIRPLAY_IsStart())
			{
				AIRPLAY_Stop();
			}
			else
			{
				HxLOG_Error("AIRPLAY Stopped already... \n");
			}
			break;

		case eBUS_EVENT_SYS_MS_FORMAT_TRY:
		default:
			/* HxLOG_Print("[%s:%d] unknown evt(%d)\n", __FUNCTION__, __LINE__, eEvt); */
			break;
	}

	return ERR_OK;
}



static void __hom_airplay_event(HINT32 p1, HINT32 p2, HINT64 p3)
{
	switch(p1)
	{
		case eAIRPLAY_OP_PLAY:
			{
				HOMxAIRPLAY_PARAMETER_WITH_URL_t *pParam = HLIB_STD_MemCalloc(sizeof(HOMxAIRPLAY_PARAMETER_WITH_URL_t));
				HINT32 szUrl = HOM_STRLEN((void*)p2);

				if(pParam == NULL)				{ return; }

				HxLOG_Print("#################### AIRPLAY PLAY ##################### \n");
				HxLOG_Print("[ %s:%d ] Play request\n", __MODULE__, __LINE__);
				HxLOG_Print("[ %s:%d ] URL : %s\n", __MODULE__, __LINE__, (HCHAR*)p2);
				HxLOG_Print("[ %s:%d ] Start from : %lld%\n", __MODULE__, __LINE__, p3);
				if(szUrl > AIRPLAY_URL_MAX - 1)
				{
					HxLOG_Error("URL is too long (%d), increase AIRPLAY_URL_MAX definition\n", szUrl);
						HLIB_STD_MemFree(pParam);
					return;
				}

				HOM_MEMCOPY(&pParam->contentURL, (void*)p2, szUrl);
				pParam->decimalParam = p3;
				__hom_airplay_send_notify(eHOMAPI_AIRPLAY_OP_PLAY, (HCHAR *)pParam, sizeof(HOMxAIRPLAY_PARAMETER_WITH_URL_t));
			}
			break;
		case eAIRPLAY_OP_SETPOSITION:
			{
				HOMxAIRPLAY_PARAMETER_t *pParam = HLIB_STD_MemCalloc(sizeof(HOMxAIRPLAY_PARAMETER_t));
				if(pParam == NULL)				{ return; }

				HxLOG_Print("#################### AIRPLAY SETPOSITION ##################### \n");
				HxLOG_Print("[ %s:%d ] SetPosition request\n", __MODULE__, __LINE__);
				HxLOG_Print("[ %s:%d ] Jump to : %lldms\n", __MODULE__, __LINE__, p3);

				pParam->decimalParam = p3;

				__hom_airplay_send_notify(eHOMAPI_AIRPLAY_OP_SETPOSITION, (HCHAR *)pParam, sizeof(HOMxAIRPLAY_PARAMETER_t));
			}
			break;
		case eAIRPLAY_OP_SETRATE:
			{
				HOMxAIRPLAY_PARAMETER_t *pParam = HLIB_STD_MemCalloc(sizeof(HOMxAIRPLAY_PARAMETER_t));
				if(pParam == NULL)				{ return; }

				HxLOG_Print("#################### AIRPLAY SETRATE ##################### \n");
				HxLOG_Print("[ %s:%d ] SetRate request\n", __MODULE__, __LINE__);
				HxLOG_Print("[ %s:%d ] Multiplier : x%lld\n", __MODULE__, __LINE__, p3);

				pParam->decimalParam = p3;

				__hom_airplay_send_notify(eHOMAPI_AIRPLAY_OP_SETRATE, (HCHAR *)pParam, sizeof(HOMxAIRPLAY_PARAMETER_t));
			}
			break;
		case eAIRPLAY_OP_STOP:
			{
				HxLOG_Print("#################### AIRPLAY STOP ##################### \n");
				HxLOG_Print("[ %s:%d ] Stop request\n", __MODULE__, __LINE__);
				__hom_airplay_send_notify(eHOMAPI_AIRPLAY_OP_STOP, NULL, 0);
			}
			break;
		case eAIRPLAY_OP_SHOWPHOTO:
			{
				HOMxAIRTUNES_ALBUMART_t * pParam = HLIB_STD_MemCalloc(sizeof(HOMxAIRTUNES_ALBUMART_t));
				if(pParam == NULL)				{ return; }

				HOM_MEMCOPY(pParam->szAlbumArtPath , (void*)p2, sizeof(pParam->szAlbumArtPath));

				HxLOG_Print("#################### AIRTUNES SET ALBUMART ##################### \n");
				HxLOG_Print("[ %s:%d ] Show Photo request\n", __MODULE__, __LINE__);

				__hom_airplay_send_notify(eHOMAPI_AIRPLAY_OP_SHOWPHOTO, (HCHAR *)pParam, sizeof(HOMxAIRTUNES_ALBUMART_t));
			}
			break;
		case eAIRPLAY_OP_GET_PLAYBACK_INFO:
			{
				HxLOG_Print("#################### AIRPLAY PLAYBACK INFO ##################### \n");
				HxLOG_Print("[ %s:%d ] Playback info request\n", __MODULE__, __LINE__);
				__hom_airplay_send_notify(eHOMAPI_AIRPLAY_OP_GET_PLAYBACK_INFO, NULL, 0);
			}
			break;
		case eAIRTUNES_OP_PLAY:
			{
				HOMxAIRTUNES_URL_t *pParam = HLIB_STD_MemCalloc(sizeof(HOMxAIRTUNES_URL_t));
				HINT32 szUrl = HOM_STRLEN((void*)p2);

				if(pParam == NULL)				{ return; }

				HxLOG_Print("#################### AIRTUNES PLAY ##################### \n");
				HxLOG_Print("[ %s:%d ] Play request\n", __MODULE__, __LINE__);
				HxLOG_Print("[ %s:%d ] URL : %s\n", __MODULE__, __LINE__, (HCHAR*)p2);
				if(szUrl > AIRPLAY_URL_MAX - 1)
				{
					HxLOG_Error("URL is too long (%d), increase AIRPLAY_URL_MAX definition\n", szUrl);
						HLIB_STD_MemFree(pParam);
					return;
				}

				HOM_MEMCOPY(&pParam->contentURL, (void*)p2, szUrl);
				__hom_airplay_send_notify(eHOMAPI_AIRTUNES_OP_PLAY, (HCHAR *)pParam, sizeof(HOMxAIRTUNES_URL_t));
			}
			break;
		case eAIRTUNES_OP_FLUSH:
			{
				HxLOG_Print("#################### AIRTUNES FLUSH ##################### \n");
				HxLOG_Print("[ %s:%d ] Flush request\n", __MODULE__, __LINE__);
				__hom_airplay_send_notify(eHOMAPI_AIRTUNES_OP_FLUSH, NULL, 0);
			}
			break;
		case eAIRTUNES_OP_STOP:
			{
				HxLOG_Print("#################### AIRTUNES STOP ##################### \n");
				HxLOG_Print("[ %s:%d ] Stop request\n", __MODULE__, __LINE__);
				__hom_airplay_send_notify(eHOMAPI_AIRTUNES_OP_STOP, NULL, 0);
			}
			break;
		case eAIRTUNES_OP_SET_METADATA:
			{
				HOMxAIRTUNES_METADATA_t *pParam = HLIB_STD_MemCalloc(sizeof(HOMxAIRTUNES_METADATA_t));
				if(pParam == NULL)				{ return; }

				HOM_MEMCOPY(pParam, (void*)p2, sizeof(HOMxAIRTUNES_METADATA_t));

				HxLOG_Print("#################### AIRTUNES SET METADATA ##################### \n");
				HxLOG_Print("[ %s:%d ] Set Metadata request\n", __MODULE__, __LINE__);
				__hom_airplay_send_notify(eHOMAPI_AIRTUNES_OP_SET_METADATA, (HCHAR *)pParam, sizeof(HOMxAIRTUNES_METADATA_t));
			}
			break;
		case eAIRTUNES_OP_SET_ALBUMART:
			{
				HOMxAIRTUNES_ALBUMART_t * pParam = HLIB_STD_MemCalloc(sizeof(HOMxAIRTUNES_ALBUMART_t));
				if(pParam == NULL)				{ return; }

				HOM_MEMCOPY(pParam->szAlbumArtPath , (void*)p2, sizeof(pParam->szAlbumArtPath));

				HxLOG_Print("#################### AIRTUNES SET ALBUMART ##################### \n");
				HxLOG_Print("[ %s:%d ] Set Albumart request\n", __MODULE__, __LINE__);
				__hom_airplay_send_notify(eHOMAPI_AIRTUNES_OP_SET_ALBUMART, (HCHAR *)pParam, sizeof(HOMxAIRTUNES_ALBUMART_t));
			}
			break;
		default:
			break;
	}
}

static HINT32	__hom_airplay_cmd_process( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_IpcRecv_t *stJsonData )
{
	HINT32	nOper =	stJsonData->nOpCode;
	comm_msg_data_t *data = stJsonData->pvJson;

	/* Get */
	if ( eBUS_CMD_GET == nOper )
	{
		HCHAR   			*endcode_str = NULL;
		HOMMA_BUS_Msg_t 	busMsg;
		HOMMA_BUS_IpcSend_t *pMsgData =  NULL;

		if ( data->size != 0)
		{
			switch ( data->cmd)
			{
			default:
				break;
			}

			return ERR_FAIL;
		}

		pMsgData = (HOMMA_BUS_IpcSend_t *)HLIB_STD_MemCalloc( sizeof(HOMMA_BUS_IpcSend_t) );
		HxSTD_MemSet(&busMsg, 0x00,sizeof(HOMMA_BUS_Msg_t));

		pMsgData->nPairId 		= stJsonData->nPairId ;
		pMsgData->pszJson 		= endcode_str ;
		pMsgData->bMalloc 		= TRUE;
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

	switch (data->cmd)
	{
	case eHOMAPI_AIRPLAY_Command_Start:
	{
		if( AIRPLAY_IsStart() && AIRPLAY_NeedRestart() )
		{
			AIRPLAY_Stop();
		}

		if ( !AIRPLAY_IsStart() )
		{
			PL_AIRPLAY_SERVICE_PARAM_t * tServiceParam = NULL;
			HxLOG_Print("[%s:%d] ### AIRPLAY START ### \n", __FUNCTION__, __LINE__);
			/* if ServiceParam is NULL, it gonna be set default. */
			AIRPLAY_SetServiceParameter(tServiceParam);
			if(AIRPLAY_Start() != ERR_OK)
			{
				HxLOG_Error("Error> Failed to start airplay. ip address is not available\n");
			}
			else
			{
				HxLOG_Debug("AIRPLAY started\n");
			}
		}
		else
		{
			HxLOG_Debug("AIRPLAY Started already... \n");
		}
	}
	break;

	case eHOMAPI_AIRPLAY_Command_Stop:
	{
		HxLOG_Print("[%s:%d] ### AIRPLAY STOP ### \n", __FUNCTION__, __LINE__);
		AIRPLAY_Stop();
	}
	break;

	case eHOMAPI_AIRPLAY_Command_SetPlaybackInfo:
	{
		HOMxAIRPLAY_PLAYBACK_INFO_t* homx_pbinfo = (HOMxAIRPLAY_PLAYBACK_INFO_t*)data->data;
		PL_AIRPLAY_PLAYBACK_INFO_t pbinfo;

		HxLOG_Print("[%s:%d] ### AIRPLAY SET PLAYBACK INFO ###\n", __FUNCTION__, __LINE__);

		switch(homx_pbinfo->ePlaybackStatus)
		{
		case eHOMAPI_AIRPLAY_PLAYBACK_PAUSE:
			HxLOG_Print("[%s:%d] Status : Paused\n", __FUNCTION__, __LINE__);
			pbinfo.ePlaybackStatus = PL_AIRPLAY_PLAYBACK_PAUSE;
			break;
		case eHOMAPI_AIRPLAY_PLAYBACK_PLAYING:
			HxLOG_Print("[%s:%d] Status : Playing\n", __FUNCTION__, __LINE__);
			pbinfo.ePlaybackStatus = PL_AIRPLAY_PLAYBACK_PLAYING;
			break;
		case eHOMAPI_AIRPLAY_PLAYBACK_REACHED_EOF:
			HxLOG_Print("[%s:%d] Status : Eof\n",__FUNCTION__, __LINE__);
			pbinfo.ePlaybackStatus = PL_AIRPLAY_PLAYBACK_REACHED_EOF;
			break;
		case eHOMAPI_AIRPLAY_PLAYBACK_STOPPED:
			HxLOG_Print("[%s:%d] Status : Stopped\n",__FUNCTION__, __LINE__);
			pbinfo.ePlaybackStatus = PL_AIRPLAY_PLAYBACK_STOPPED;
			break;
		case eHOMAPI_AIRPLAY_PLAYBACK_LOADING:
			HxLOG_Print("[%s:%d] Status : Loading\n",__FUNCTION__, __LINE__);
			pbinfo.ePlaybackStatus = PL_AIRPLAY_PLAYBACK_LOADING;
			break;
		case eHOMAPI_AIRPLAY_PLAYBACK_ERROR:
			HxLOG_Print("[%s:%d] Status : Error\n", __FUNCTION__, __LINE__);
			pbinfo.ePlaybackStatus = PL_AIRPLAY_PLAYBACK_ERROR;
			break;
		case eHOMAPI_AIRPLAY_PLAYBACK_NONE:
		default:
			pbinfo.ePlaybackStatus = PL_AIRPLAY_PLAYBACK_NONE;
			break;
		}
		pbinfo.ullBufferedRange = homx_pbinfo->ullBufferedRange;
		pbinfo.ullDuration = homx_pbinfo->ullDuration;
		pbinfo.ullPosition = homx_pbinfo->ullPosition;
		pbinfo.ullSeekableRange = homx_pbinfo->ullSeekableRange;
		pbinfo.ullStartPosition = homx_pbinfo->ullStartPosition;
		pbinfo.unRate = homx_pbinfo->unRate;
		//HxLOG_Print("[%s:%d] Duration : %lld\n", __FUNCTION__, __LINE__, homx_pbinfo->ullDuration);
		//HxLOG_Print("[%s:%d] Position : %lld\n", __FUNCTION__, __LINE__, homx_pbinfo->ullPosition);
		//HxLOG_Print("[%s:%d] Rate : %d\n",  __FUNCTION__, __LINE__, homx_pbinfo->unRate);
		AIRPLAY_SetPlaybackInfo(pbinfo);
	}
	break;

	default :
		break;
	}

	return ERR_OK;
}

static HINT32	__hom_airplay_bus_request_handler( HOMMA_BUS_Request_e eReq, HOMMA_BUS_Data_t *pstRetData )
{
	return 0;
}

static HINT32	__hom_airplay_bus_release_handler( HOMMA_BUS_Target_e eSender, HOMMA_BUS_Msg_t *pstMsg )
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

static HINT32	__hom_airplay_bus_message_handler( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_Msg_t *pstMsg )
{
	switch ( pstMsg->eMsgType )
	{
		case eBUS_MSGTYPE_EVENT :
			__hom_airplay_bus_event( pstMsg->uEnum.eEvt, pstMsg->eDataType, pstMsg->pvData );
			break;

		case eBUS_MSGTYPE_REQUEST :
			if ( eBUS_REQUEST_JSON_DECODE == pstMsg->uEnum.eReq && eBUS_DATATYPE_IPC_RECV == pstMsg->eDataType )
				__hom_airplay_cmd_process( eSender, pstMsg->pvData );
			break;

		case eBUS_MSGTYPE_RESPONSE :
			break;

		default :
			break;
	}
	return 0;
}

#define ________________Public_Functions___________________________
void	HOM_AIRPLAY_Run( void )
{
	HOMMA_BUS_HndFuncList_t stFuncList;
	HOMMA_BUS_Msg_t 		stMsg;

	HxSTD_MemSet(&stMsg, 0x00,sizeof(HOMMA_BUS_Msg_t));
	HxSTD_MemSet(&stFuncList, 0x00,sizeof(HOMMA_BUS_HndFuncList_t));
	s_stBusHnd = HOMMA_BUS_GetHandle( eBUS_TARGET_MOD_AIRPLAY );

	stFuncList.cbMsgHandleFunc = __hom_airplay_bus_message_handler;
	stFuncList.cbReqHandleFunc = __hom_airplay_bus_request_handler;
	stFuncList.cbRelHandleFunc = __hom_airplay_bus_release_handler;

	HOMMA_BUS_Register( s_stBusHnd, &stFuncList );
	AIRPLAY_RegisterEventCallback(__hom_airplay_event);

	stMsg.eMsgType = eBUS_MSGTYPE_EVENT;
	stMsg.uEnum.eEvt = 0;

	HOMMA_BUS_SendMsg( eBUS_TARGET_ALL, s_stBusHnd, &stMsg );
}
