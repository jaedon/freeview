/**************************************************************
*	@file	hom_dms.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#define	________________Header_Files_______________________________
#include 	<vkernel.h>
#include 	<hlib.h>

#include	"common.h"
#include 	"hom_int.h"
#include	"homapi.h"
#include 	"homma_int.h"
#include 	"homma_bus.h"
#include 	"homma_ipc_decode.h"
#include	"dms.h"

#define ________________Typedef____________________________________
typedef struct
{
	BUS_Handle			hMsgBusHandle;
	HOMMA_BUS_Target_e 	eSelf;

	HBOOL				bMonitorStorage;
	HBOOL				bPrevStartFailed;
	HBOOL				bRequestStart;
	HCHAR 				*pszFriendlyName;
} HOM_DMS_Context_t;

#define DMS_STANDBYWAIT_TIME			(10000)  /* (30000*2) */

#define ________________Static_Variables___________________________
static HOM_DMS_Context_t s_stHomDmsContext = { 0 };
#if defined(CONFIG_HOMMA_DMS)
static BUS_Handle s_stBusHnd = NULL;
#endif

#define ________________Internal_APIs_Prototype____________________
static HOM_DMS_Context_t * __hom_dms_getContext( void );

#define ________________Internal_Functions_________________________
static	void	__hom_dms_init( HOM_DMS_Context_t *pstHomDmsCtx)
{
	pstHomDmsCtx->eSelf = eBUS_TARGET_MOD_DMS;

	/* monitoring storage */
	pstHomDmsCtx->bMonitorStorage  = TRUE;
	pstHomDmsCtx->bPrevStartFailed = FALSE;
	pstHomDmsCtx->bRequestStart    = FALSE;
	pstHomDmsCtx->pszFriendlyName  = NULL;
}
static HOM_DMS_Context_t * __hom_dms_getContext( void )
{
	return &s_stHomDmsContext;
}

#if defined(CONFIG_HOMMA_DMS)
static HERROR	__hom_dms_request_start( const HCHAR * pszFriendlyName )
{
	HERROR	retErr = ERR_FAIL;
	DMS_Config_t stConfig;
	HOMMA_BUS_Data_t retData;
	CO_NET_Info_t *pNetInfo = NULL;
	CO_STORAGE_Info_t *pStoInfo = NULL;

	HxSTD_MemSet(&stConfig, 0x00, sizeof(DMS_Config_t));
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
		}
	}
	DMS_SetNetworkInfo( pNetInfo );

	/* Get storage list */
	HxSTD_MemSet( &retData, 0x00, sizeof(HOMMA_BUS_Data_t) );
	retErr = HOMMA_BUS_Request( eBUS_TARGET_SYS, eBUS_REQUEST_STORAGE_INFO, &retData, &retErr );
	if ( ERR_OK != retErr )
	{
		HxLOG_Error("[%s:%d] Error> storage information request is faild. \n", __func__, __LINE__);
		return ERR_FAIL;
	}

	if ( eBUS_DATATYPE_USER_DEF <= retData.eDataType )
	{
		pStoInfo = (CO_STORAGE_Info_t *)retData.pvData;
		if ( pStoInfo != NULL && 0 == pStoInfo->nDevCnt )
		{
			HxLOG_Error("[%s:%d] Error> avaiable stroage is not existed. \n", __func__, __LINE__);
		}
	}

	if (pStoInfo == NULL || pStoInfo->nDevCnt == 0)
	{
		HxLOG_Error( "[HOM_DMS] Error> Cant get storage infos!\n" );
	}
	else
	{
		DMS_SetStorageInfo( pStoInfo );
	}


	HxSTD_StrNCpy( stConfig.szFriendlyName, pszFriendlyName, sizeof(stConfig.szFriendlyName)-1 );
	retErr = DMS_Start( &stConfig );
	if ( ERR_OK != retErr )
	{
		HxLOG_Error( "[HOM_DMS] Error> Start Failed!!!!\n" );
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR	__hom_dms_send_notify(HINT32 cmd, HCHAR *pData, HUINT32	size)
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
	busMsg.nDataSize 	= sizeof(HOMMA_BUS_IpcSend_t);
	busMsg.bNeedFree 	= TRUE;

	HOMMA_BUS_SendMsg( eBUS_TARGET_SERVER, s_stBusHnd, &busMsg );

	return ERR_OK;
}
#endif

static HINT32	__hom_dms_sys_event_handle( HOMMA_BUS_Event_e eEvent, HOMMA_BUS_DataType_e eDataType, void *pvData )
{
#if defined(CONFIG_HOMMA_DMS)
	HERROR	hError = ERR_OK;
	HOM_DMS_Context_t *pHomDmsCtx = __hom_dms_getContext();
#endif
	HxLOG_Error("[ HOM_DMS ] ++system event:%d\n", eEvent);

#if defined(CONFIG_HOMMA_DMS)
	if (pHomDmsCtx->bMonitorStorage == TRUE)
	{
		if (pHomDmsCtx->bRequestStart == TRUE &&
			pHomDmsCtx->bPrevStartFailed == TRUE &&
			eEvent == eBUS_EVENT_SYS_FS_MOUNTED)
		{
			if(DMS_IsStart() == FALSE)
			{
				HxLOG_Debug("[%s:%d] Info> Retry DMS start() because of prev failure(%d)!\n", __func__, __LINE__, pHomDmsCtx->bPrevStartFailed);
				hError = __hom_dms_request_start(pHomDmsCtx->pszFriendlyName);
				if (hError == ERR_OK)
				{
					pHomDmsCtx->bPrevStartFailed = FALSE;
				}
				else
				{
					HxLOG_Error("[ HOM_DMS ] Error> failed to re-start DMS!\n");
				}
			}
			return ERR_OK; /* event 처리없이 return */
		}
		else if (eEvent == eBUS_EVENT_SYS_POWER_OPERATION) /* If there is no mount signal */
		{
			if ((DMS_IsStart() == FALSE) && (pHomDmsCtx->bRequestStart == TRUE))
			{
				HxLOG_Debug("[HOM_DMS] Retry DMS start() because of prev failure(%d)!\n", pHomDmsCtx->bPrevStartFailed);
				hError = __hom_dms_request_start(pHomDmsCtx->pszFriendlyName);
				if (hError == ERR_OK)
				{
					pHomDmsCtx->bPrevStartFailed = FALSE;
				}
				else
				{
					HxLOG_Error("[HOM_DMS] Error> failed to re-start DMS!\n");
				}
			}
			return ERR_OK; /* event 처리없이 return */
		}
		else if(eEvent == eBUS_EVENT_SYS_POWER_SHUTDOWN)
		{
			DxSchedule_t	stSchedule;
			HBOOL bDmsStop = TRUE;

			HxSTD_MemSet(&stSchedule, 0x0, sizeof(DxSchedule_t));

			if(DMS_IsStart() == TRUE)
			{
				if(PL_DMS_IsStreaming() == TRUE)
				{
					HxLOG_Warning("[ HOM_DMS ] DMS is still in streaming... Send wait for Shutdown to PAMA!!\n");
					PL_COMMON_WaitShutDown("DMS", DMS_STANDBYWAIT_TIME);
					bDmsStop = FALSE;
				}
				else if (SAPI_GetScheduleByType(DxRSVTYPE_ACTIVATION_TIME, 0, &stSchedule) == ERR_OK)
				{
					if(stSchedule.eStatus == DxRSVSTATUS_RUNNING)
					{
						HxLOG_Warning("[ HOM_DMS ] Server activation time is enabled... Send wait for Shutdown to PAMA!!\n");
						PL_COMMON_WaitShutDown("DMS", DMS_STANDBYWAIT_TIME);
						bDmsStop = FALSE;
					}
				}

				if(bDmsStop)
				{
					pHomDmsCtx->bPrevStartFailed = TRUE;
					DMS_Stop();
				}
			}
			else
			{
				if((pHomDmsCtx->bRequestStart == TRUE) &&
					(SAPI_GetScheduleByType(DxRSVTYPE_ACTIVATION_TIME, 0, &stSchedule) == ERR_OK))
				{
					if(stSchedule.eStatus == DxRSVSTATUS_RUNNING)
					{
						hError = __hom_dms_request_start(pHomDmsCtx->pszFriendlyName);
						if (hError == ERR_OK)
						{
							HxLOG_Warning("[ HOM_DMS ] DMS Started in active-standby mode because of [DxRSVTYPE_ACTIVATION_TIME]\n");

							/* 여기서 wait for shutdown을 날려주지 않으면 timming에 의해 DMS start 도중 S3 진입할 가능성 있음 */
							PL_COMMON_WaitShutDown("DMS", DMS_STANDBYWAIT_TIME);

							pHomDmsCtx->bPrevStartFailed = FALSE;
						}
						else
						{
							HxLOG_Error("[ HOM_DMS ] Error> failed to re-start DMS!\n");
						}
					}
					else
					{
						HxLOG_Warning("There is [DxRSVTYPE_ACTIVATION_TIME], but current status is (%d)\n", stSchedule.eStatus);
					}
				}
				else
				{
					HxLOG_Warning("pHomDmsCtx->bRequestStart (%d)\n", pHomDmsCtx->bRequestStart);
				}
			}
			return ERR_OK;
		}
	}

	return DMS_OnSystemEvent( eEvent, (void *)eDataType, pvData );
#else
	return ERR_OK;
#endif
}

static HINT32	__hom_dms_cmd_process( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_IpcRecv_t *pstData )
{
	HERROR	hError = ERR_OK;
	HOM_DMS_Context_t *pHomDmsCtx = __hom_dms_getContext();
	HINT32	nOper 	=	pstData->nOpCode;

	comm_msg_data_t *data = pstData->pvJson;

	if ( eBUS_CMD_GET == nOper )
	{
		HINT32				flag = 0;
		HCHAR   			*endcode_str = NULL;
		HINT32				nStrSize = 0;
		HOMMA_BUS_Msg_t busMsg = { 0 };
		HOMMA_BUS_IpcSend_t *pMsgData = NULL;

		HxLOG_Debug("######## DMS GET(+) ######## data->cmd(%d)\n", data->cmd);

		switch( data->cmd)
		{
		case eHOMAPI_DMS_Command_GetLiveStreamInfo:
			{
#if defined(CONFIG_HOMMA_DMS)
				HOMxDMS_Args_LiveStreamInfo_t *param = (HOMxDMS_Args_LiveStreamInfo_t *)data->data;

				HxLOG_Debug("[ HOM_DMS ] ==============================================\n");
				HxLOG_Debug("[ HOM_DMS ] ===========> REQ DMS Get Live Stream Info : %d \n", (HCHAR *)param->nSvcUid);
				HxLOG_Debug("[ HOM_DMS ] ==============================================\n");

				if ((DMS_IsStart() == TRUE))
				{
					HINT32	nCount = 0;
					HINT32	nCdsSize = 0;
					HERROR	hError = ERR_FAIL;
					PL_DMS_LiveStreamInfo_t *pResultData = NULL;

					hError = DMS_GetLiveStreamInfo(param->nSvcUid, &nCount, &pResultData);

					if(pResultData != NULL)
					{
						if(hError == ERR_OK && nCount > 0)
						{
							nCdsSize  = sizeof(PL_DMS_LiveStreamInfo_t) * nCount;
							nStrSize =  sizeof(HINT32) + nCdsSize;
							endcode_str = HLIB_STD_MemCalloc(nStrSize);

							HxSTD_MemCopy( endcode_str, &nCount, sizeof(HINT32) );
							HxSTD_MemCopy( endcode_str+sizeof(HINT32), pResultData, nCdsSize );
						}

						HOM_MEMFREE(pResultData);
					}

					HxLOG_Debug("Get Live Stream Info() : cnt(%d), dataSize(%d)######## \n",nCount, nStrSize);
				}

				flag = 1;
#endif
			}
			break;

		default:
			break;
		}

		if ( data->size == 0 || flag == 1)
		{
			pMsgData = (HOMMA_BUS_IpcSend_t *)HLIB_STD_MemCalloc( sizeof(HOMMA_BUS_IpcSend_t) );
			HxSTD_MemSet(&busMsg, 0x00,sizeof(HOMMA_BUS_Msg_t));

			pMsgData->nPairId		= pstData->nPairId ;
			pMsgData->pszJson 		= endcode_str;
			pMsgData->bMalloc		= TRUE;
			pMsgData->nClientHandle = pstData->nClientHandle;
			pMsgData->cmd			= data->cmd;
			pMsgData->nOpCode		= 0;
			pMsgData->data 			= (void *)endcode_str;
			pMsgData->size			= nStrSize;

			busMsg.eMsgType 	= eBUS_MSGTYPE_RESPONSE;
			busMsg.uEnum.eRes 	= eBUS_REQUEST_SERVER_SENDING;
			busMsg.eDataType 	= eBUS_DATATYPE_IPC_SEDN;
			busMsg.pvData 		= pMsgData;
			busMsg.nDataSize 	= sizeof(HOMMA_BUS_IpcSend_t);
			busMsg.bNeedFree 	= TRUE;

			HOMMA_BUS_SendMsg( eSender, pHomDmsCtx->hMsgBusHandle, &busMsg );
			HxLOG_Debug("[%s:%d] ######## DMS GET(-) ######## \n", __func__, __LINE__);

			return ERR_OK;
		}
	}


	switch ( data->cmd )
	{
		case eHOMAPI_DMS_Command_Start:
			{
				HOMxDMS_Args_Start_t *arg = (HOMxDMS_Args_Start_t *)data->data;
				if ((pHomDmsCtx->bPrevStartFailed == TRUE) && (pHomDmsCtx->bRequestStart == TRUE))
				{
					HxLOG_Error("### [%s:%d] Error> Overlapped start() request(prev:%d)! ###\n", __func__, __LINE__, pHomDmsCtx->bPrevStartFailed);
					;
				}
				pHomDmsCtx->bRequestStart = TRUE; /* DMS start() failture 이후 Wait 상태에서 start_request 문제된다 */

#if defined(CONFIG_HOMMA_DMS)
				if(DMS_IsStart() == TRUE)
					HxLOG_Error("### [%s:%d] Error> Already DMS started! ###\n", __func__, __LINE__);

				if (pHomDmsCtx->pszFriendlyName != NULL)
				{
					HOM_MEMFREE(pHomDmsCtx->pszFriendlyName);
					pHomDmsCtx->pszFriendlyName= NULL;
				}
				pHomDmsCtx->pszFriendlyName = HLIB_STD_StrDup((HCHAR *)arg->szFriendlyName);

				hError = __hom_dms_request_start( (HCHAR *)arg->szFriendlyName);
#endif
				HxLOG_Info("[ HOM_DMS ] ==============================================\n");
				HxLOG_Info("[ HOM_DMS ] ===========> REQ DMS START : %s \n", (HCHAR *)arg->szFriendlyName);
				HxLOG_Info("[ HOM_DMS ] ==============================================\n");

				if (hError != ERR_OK)
				{
					HxLOG_Error("[ HOM_DMS ] Error> failed to start DMS!\n");
					pHomDmsCtx->bPrevStartFailed = TRUE;
				}
			}
			break;

		case eHOMAPI_DMS_Command_Stop:
			{
				HxLOG_Info("[ HOM_DMS ] ==============================================\n");
				HxLOG_Info("[ HOM_DMS ] ===========> REQ DMS STOP \n");
				HxLOG_Info("[ HOM_DMS ] ==============================================\n");

				pHomDmsCtx->bPrevStartFailed = FALSE;
				pHomDmsCtx->bRequestStart = FALSE;
#if defined(CONFIG_HOMMA_DMS)
				if(DMS_IsStart() == FALSE)
					HxLOG_Error("### [%s:%d] Error> Already DMS stopped! ###\n", __func__, __LINE__);

				DMS_Stop();
#endif
				if (pHomDmsCtx->pszFriendlyName != NULL)
				{
					HOM_MEMFREE(pHomDmsCtx->pszFriendlyName);
					pHomDmsCtx->pszFriendlyName= NULL;
				}
			}
			break;

		case eHOMAPI_DMS_Command_Rename:
			{
#if defined(CONFIG_HOMMA_DMS)
				HOMMA_BUS_Data_t 	retData;
				HOMxDMS_Args_Rename_t *param = (HOMxDMS_Args_Rename_t *)data->data;

				HxSTD_MemSet(&retData, 0x00, sizeof(HOMMA_BUS_Data_t));

				HxLOG_Info("[ HOM_DMS ] ==============================================\n");
				HxLOG_Info("[ HOM_DMS ] ===========> REQ DMS RENAME : %s \n", (HCHAR *)param->szFriendlyName);
				HxLOG_Info("[ HOM_DMS ] ==============================================\n");

				if ((DMS_IsStart() == TRUE) && !HxSTD_StrEmpty(param->szFriendlyName))
				{
					DMS_SetName(param->szFriendlyName);
					if (1)
					{
						if (pHomDmsCtx->pszFriendlyName != NULL)
						{
							HOM_MEMFREE(pHomDmsCtx->pszFriendlyName);
							pHomDmsCtx->pszFriendlyName= NULL;
						}
						pHomDmsCtx->pszFriendlyName = HLIB_STD_StrDup((HCHAR *)param->szFriendlyName);
					}
				}
#endif
			}
			break;

		case eHOMAPI_DMS_Command_Info:
			HxLOG_Debug("[ HOM_DMS ] ===========> REQ DMS Info ==============\n");
			break;

		default :
			break;
	}

	return ERR_OK;
}

static HINT32	__hom_dms_bus_request_handler( HOMMA_BUS_Request_e eReq, HOMMA_BUS_Data_t *pstRetData )
{
	return 0;
}

static HINT32	__hom_dms_bus_release_handler( HOMMA_BUS_Target_e eSender, HOMMA_BUS_Msg_t *pstMsg )
{

	switch ( pstMsg->eDataType )
	{
		case eBUS_DATATYPE_IPC_SEDN :
			{
				HOMMA_BUS_IpcSend_t *pIpcSend = pstMsg->pvData;
				if ( pIpcSend && pIpcSend->bMalloc && pIpcSend->pszJson )
				{
					HLIB_STD_MemFree( pIpcSend->pszJson );
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
		HLIB_STD_MemFree( pstMsg->pvData );
		pstMsg->pvData = NULL;
	}

	return 0;
}

static HINT32	__hom_dms_bus_message_handler( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_Msg_t *pstMsg )
{
	switch ( pstMsg->eMsgType )
	{
		case eBUS_MSGTYPE_EVENT :
			if ( eBUS_TARGET_SYS == eSender )
			{
				__hom_dms_sys_event_handle( pstMsg->uEnum.eEvt, pstMsg->eDataType, pstMsg->pvData );
			}
			break;

		case eBUS_MSGTYPE_REQUEST :
			if ( eBUS_REQUEST_JSON_DECODE == pstMsg->uEnum.eReq && eBUS_DATATYPE_IPC_RECV == pstMsg->eDataType )
			{
				__hom_dms_cmd_process( eSender, pstMsg->pvData );
			}
			break;

		case eBUS_MSGTYPE_RESPONSE :
			break;

		default :
			break;
	}
	return 0;
}


#if defined(CONFIG_HOMMA_DMS)
static HINT32	__hom_dms_callback_process( DMS_EVENT_e eEvent, HINT32 p1, HINT32 p2, HINT32 p3 )
{
	HOM_DMS_Context_t *pHomDmsCtx = __hom_dms_getContext();
	HOMMA_BUS_Msg_t stMsg = { 0 };

	HxSTD_MemSet(&stMsg, 0x00, sizeof(HOMMA_BUS_Msg_t));
	stMsg.eMsgType = eBUS_MSGTYPE_EVENT;

	switch ( eEvent )
	{
		case eDMS_EVT_START :
			stMsg.uEnum.eEvt = eBUS_EVENT_MOD_START;
			HOMMA_BUS_SendMsg( eBUS_TARGET_ALL, pHomDmsCtx->hMsgBusHandle, &stMsg );
			break;

		case eDMS_EVT_STOP :
			stMsg.uEnum.eEvt = eBUS_EVENT_MOD_STOP;
			HOMMA_BUS_SendMsg( eBUS_TARGET_ALL, pHomDmsCtx->hMsgBusHandle, &stMsg );
			break;

		default :
			break;
	}
	return 0;
}
#endif

#define ________________Public_Interfaces__________________________
void	HOM_DMS_Run( void )
{
	HOM_DMS_Context_t 		*pHomDmsCtx = __hom_dms_getContext();
	HOMMA_BUS_HndFuncList_t stFuncList;

	HxSTD_MemSet(&stFuncList, 0x00, sizeof(HOMMA_BUS_HndFuncList_t));
	__hom_dms_init( pHomDmsCtx );

	stFuncList.cbMsgHandleFunc = __hom_dms_bus_message_handler;
	stFuncList.cbReqHandleFunc = __hom_dms_bus_request_handler;
	stFuncList.cbRelHandleFunc = __hom_dms_bus_release_handler;

	pHomDmsCtx->hMsgBusHandle = HOMMA_BUS_GetHandle( eBUS_TARGET_MOD_DMS );
	HOMMA_BUS_Register( pHomDmsCtx->hMsgBusHandle, &stFuncList );

	s_stBusHnd = pHomDmsCtx->hMsgBusHandle;

#if defined(CONFIG_HOMMA_DMS)
	DMS_Initialize();
	DMS_SetEvtListener( __hom_dms_callback_process );
#endif

#if defined(CONFIG_HOMMA_WOONS)
	WOONS_Init();
#endif

	HxLOG_Print("HOM_DMS Run!!!\n");
}
#define _____END_OF_FILE_________
