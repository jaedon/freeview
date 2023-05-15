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
#include 	"dmp.h"

#define ________________Private_Static_Variables___________________
static BUS_Handle s_stBusHnd = NULL;

#define ________________Internal_Functions_________________________
HCHAR	*__encodebase64(const HCHAR *string)
{
	static HCHAR	binaryBuf[1024 * 4] = { 0 };

	if(string == NULL) return NULL;

	HLIB_MATH_EncodeBase64( binaryBuf, string, HxSTD_StrLen(string)+1 );

	return binaryBuf;
}

HCHAR	*__decodebase64(const HCHAR *string)
{
	static HCHAR	binaryBuf[1024 * 4] = { 0 };

	if(string == NULL) return NULL;

	HLIB_MATH_DecodeBase64( binaryBuf, string, HxSTD_StrLen(string));

	return binaryBuf;
}


static HERROR	__hom_dmp_send_notify(HINT32 cmd, HCHAR *pData, HUINT32	size)
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

static HERROR	__hom_dmp_bus_event( HOMMA_BUS_Event_e eEvt, HOMMA_BUS_DataType_e eDataType, void *pvData )
{
	switch(eEvt)
	{
		case eBUS_EVENT_MOD_START:
			break;
		case eBUS_EVENT_MOD_STOP:
			break;
		case eBUS_EVENT_SYS_NET_IF_ONLINE:
			break;
		case eBUS_EVENT_SYS_NET_IF_OFFLINE:
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

		case eBUS_EVENT_SYS_POWER_SHUTDOWN:
	  		// This handling is necessary because DMP can be run by ON action from Network Server settings screen.
	  		// DMP_Stop() check the running status of DMP, so it will be safe even when DMP already stopped.
	  		DMP_Stop();
	  		break;

		default:
			HxLOG_Print("[%s:%d] unknown evt(%d)\n", __FUNCTION__, __LINE__, eEvt);
	}

	return ERR_OK;
}

static void __hom_dmp_event(HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(p1)
	{
		case eDMP_ADD_DMS:
		case eDMP_REMOVE_DMS:
			{
				PL_DMP_DmsInfo_t	*pDms = (PL_DMP_DmsInfo_t *)p2;
				HxLOG_Print("#################### DMP %s DMS ##################### \n", ( p1 == eDMP_ADD_DMS ) ? "ADD" : "REMOVE");
				// !!) pDms must not be removed, and duplicated data will be removed in release handler - tjjang(2014/3/13)
				//__hom_dmp_send_notify((p1 == eDMP_ADD_DMS)?eHOMAPI_DMP_Event_DmsAdd:eHOMAPI_DMP_Event_DmsRemove, (HINT8 *)pDms, sizeof(PL_DMP_DmsInfo_t));
				__hom_dmp_send_notify((p1 == eDMP_ADD_DMS)?eHOMAPI_DMP_Event_DmsAdd:eHOMAPI_DMP_Event_DmsRemove, (HINT8 *)HLIB_STD_MemDup(pDms, sizeof(PL_DMP_DmsInfo_t)), sizeof(PL_DMP_DmsInfo_t));
			}
			break;

		case eDMP_ADD_ITEM:
			{
				HUINT32				nCount = 0, i = 0;
				CO_MediaInstence_t	*pList = NULL;
				HCHAR				*send_data = NULL;
				CO_MediaInstence_t	*pListItem = NULL;

				DMP_GetItemList(&nCount, &pList);
				send_data = HLIB_STD_MemCalloc(sizeof(CO_MediaInstence_t) * nCount);
				HxLOG_Print("#################### DMP ADD ITEM(%d) ##################### \n", nCount);

				for(i=0; i<nCount; i++)
				{
					pListItem = (CO_MediaInstence_t *)&pList[i];
					if (pListItem)
					{
						/* Substituted several blank object properties (not provided by DMS) */
						if (pListItem->Duration == (-1)) pListItem->Duration = 0;
						if (pListItem->Bitrate == (-1))  pListItem->Bitrate = 0;
					}

					HxSTD_MemCopy(send_data + (sizeof(CO_MediaInstence_t) * i), &pList[i], sizeof(CO_MediaInstence_t));
					if (0)
					{
						pListItem = (CO_MediaInstence_t *)&pList[i];
						HxLOG_Print("[%s:%d] [%d/%d] -- [title :%s]\n", __FUNCTION__, __LINE__, i, nCount, pListItem->Title);
					}
				}

				__hom_dmp_send_notify(eHOMAPI_DMP_Event_CdsListAdd, send_data, sizeof(CO_MediaInstence_t)*nCount);
			}
			break;

		case eDMP_SEARCH_ITEM:
			{
				HUINT32	nCount = 0;
				HOMxDMP_ContentResult_t contentResult = { 0 };
				HCHAR				*send_data = NULL;

				HINT32	nInfoSize = 0;
				HINT32	nItemSize = 0;
				PL_DMP_CDSResultInfo_t *pResult = (PL_DMP_CDSResultInfo_t *)p2;
				CO_MediaInstence_t	*pList = (CO_MediaInstence_t *)p3;

				nCount = pResult->nReturnCnt;
				HxLOG_Print("#################### DMP REQ_ID(%d), Searched ITEM(%d) ##################### \n", pResult->nReqId, nCount);
				contentResult.nReqId 		= pResult->nReqId;
				contentResult.nTotalMatch	= pResult->nTotalMatch;
				contentResult.nReturnCnt	= pResult->nReturnCnt;
				contentResult.bAllDMS		= pResult->bAllDMS;
				contentResult.nErrCode		= pResult->nError;

				HxSTD_snprintf( contentResult.szUDN, sizeof(contentResult.szUDN), "%s", pResult->szUDN);
				HxSTD_snprintf( contentResult.szCID, sizeof(contentResult.szCID), "%s", pResult->szCID);

				nInfoSize = sizeof(HOMxDMP_ContentResult_t);
				nItemSize = sizeof(CO_MediaInstence_t) * nCount;

				send_data = HLIB_STD_MemCalloc( nInfoSize + nItemSize );

				HxSTD_MemCopy( send_data, &contentResult, nInfoSize );

				if( pList && nCount )
					HxSTD_MemCopy( (send_data + nInfoSize), pList, nItemSize );

				if (contentResult.bAllDMS == TRUE)
				{
					/* All DMS */
					__hom_dmp_send_notify(eHOMAPI_DMP_Event_SearchAllResult, send_data, nInfoSize + nItemSize );
				}
				else
				{
					/* UDN based */
					__hom_dmp_send_notify(eHOMAPI_DMP_Event_SearchResult, send_data, nInfoSize + nItemSize );
				}
			}
			break;
	}
}

static HERROR	__hom_dmp_cmd_process( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_IpcRecv_t *stJsonData )
{
	HINT32	nOper =	stJsonData->nOpCode;
	comm_msg_data_t *data = stJsonData->pvJson;

	/* Get */
	if ( eBUS_CMD_GET == nOper )
	{
		HCHAR   			*endcode_str = NULL;
		HINT32				nStrSize = 0;
		HOMMA_BUS_Msg_t 	busMsg;
		HOMMA_BUS_IpcSend_t *pMsgData =  NULL;

		HxLOG_Debug("######## DMP GET(+) ######## data->cmd(%d)\n", data->cmd);
		switch ( data->cmd)
		{
			case eHOMAPI_DMP_Command_DmsList:
				{
					HINT32	nCount = 0;
					HINT32	nCdsSize = 0;
					HERROR	hError = ERR_FAIL;
					PL_DMP_DmsInfo_t *pDmsList = NULL;

					hError = DMP_GetDmsList(&nCount, &pDmsList);
					if(hError != ERR_OK)
					{
						HxLOG_Error("Error> Fail to get dms list.\n");
					}

					nCdsSize  = sizeof(PL_DMP_DmsInfo_t) * nCount;
					nStrSize =  sizeof(HINT32)+ nCdsSize;
					endcode_str = HLIB_STD_MemCalloc(nStrSize);

					HxSTD_MemCopy( endcode_str, &nCount, sizeof(HINT32) );
					if ( nCount )
					{
						HxSTD_MemCopy( endcode_str+sizeof(HINT32), pDmsList, nCdsSize );
					}

					HxLOG_Debug("getDmsList() : cnt(%d), dataSize(%d)######## \n",nCount, nStrSize);
				}
				break;

			case eHOMAPI_DMP_Command_DmsDiskInfo:
			case eHOMAPI_DMP_Command_DmsRecDst:
				HxLOG_Error("Error> Unsupported command(%d)!\n", data->cmd);
				break;

			default:
				break;
		}

		pMsgData = (HOMMA_BUS_IpcSend_t *)HLIB_STD_MemCalloc( sizeof(HOMMA_BUS_IpcSend_t) );
		HxSTD_MemSet(&busMsg, 0x00,sizeof(HOMMA_BUS_Msg_t));

		pMsgData->nPairId 			= stJsonData->nPairId ;
		pMsgData->pszJson 			= endcode_str;
		pMsgData->bMalloc 			= TRUE;
		pMsgData->nClientHandle	= stJsonData->nClientHandle;
		pMsgData->cmd					= data->cmd;
		pMsgData->nOpCode			= 0;
		pMsgData->data 				= (void *)endcode_str;
		pMsgData->size				= nStrSize;

		busMsg.eMsgType 	= eBUS_MSGTYPE_RESPONSE;
		busMsg.uEnum.eRes 	= eBUS_REQUEST_SERVER_SENDING;
		busMsg.eDataType 	= eBUS_DATATYPE_IPC_SEDN;
		busMsg.pvData 		= pMsgData;
		busMsg.nDataSize 	= sizeof(HOMMA_BUS_IpcSend_t);
		busMsg.bNeedFree 	= TRUE;

		HOMMA_BUS_SendMsg( eSender, s_stBusHnd, &busMsg );
		HxLOG_Debug("[%s:%d] ######## DMP GET(-) ######## \n", __func__, __LINE__);
		return ERR_OK;
	}

	switch ( data->cmd)
	{
		case eHOMAPI_DMP_Command_Start:
			HxLOG_Print("[%s:%d] ### DMP START ### \n", __FUNCTION__, __LINE__);
			DMP_Start();
			break;

		case eHOMAPI_DMP_Command_Stop:
			HxLOG_Print("[%s:%d] ### DMP STOP ### \n", __FUNCTION__, __LINE__);
			DMP_Stop();
			break;

		case eHOMAPI_DMP_Command_Browse:
			{
				HOMxDMP_Args_Browse_t *param = (HOMxDMP_Args_Browse_t *)data->data;
				HxLOG_Print("[%s:%d] ### Browse (%s/%s/%d/%d) ### \n", __FUNCTION__, __LINE__, \
					   	param->szUDN, __decodebase64(param->szCID), param->nStartIdx, param->nRequestNum);

				DMP_Browse(param->szUDN, __decodebase64(param->szCID), 0, param->nStartIdx, param->nRequestNum);
			}
			break;

		case eHOMAPI_DMP_Command_Search:
			{
				HOMxDMP_Args_Search_t *param = (HOMxDMP_Args_Search_t *)data->data;
				HxLOG_Print("[%s:%d] ### DMP Search ###, keyword(%s) \n", __FUNCTION__, __LINE__, param->szKeyword);
				DMP_Search(param->szUDN, __decodebase64(param->szCID), param->szKeyword, 0, param->nStartIdx, param->nRequestNum, param->nRequestId);
			}
			break;

		case eHOMAPI_DMP_Command_SearchAll:
			{
				HOMxDMP_Args_SearchAll_t *param = (HOMxDMP_Args_SearchAll_t *)data->data;
				HxLOG_Print("[%s:%d] ### DMP SearchAll ###, keyword(%s) \n", __FUNCTION__, __LINE__, param->szKeyword);
				DMP_SearchAll(__decodebase64(param->szCID), param->szKeyword, 0, param->nRequestId);
			}
			break;

		case eHOMAPI_DMP_Command_DmsRefresh:
			{
				HOMxDMP_Args_DmsRefresh_t *param = (HOMxDMP_Args_DmsRefresh_t *)data->data;
				HxLOG_Print("[%s:%d] ### Refresh (%s) ### \n", __FUNCTION__, __LINE__, param->szUDN);
				DMP_RefreshDms(param->szUDN);
			}
			break;

		default :
			HxLOG_Error("Error> Unsupported command(%d)!\n", data->cmd);
			break;
	}

	return ERR_OK;
}

static HINT32	__hom_dmp_bus_request_handler( HOMMA_BUS_Request_e eReq, HOMMA_BUS_Data_t *pstRetData )
{
	return 0;
}

static HINT32	__hom_dmp_bus_release_handler( HOMMA_BUS_Target_e eSender, HOMMA_BUS_Msg_t *pstMsg )
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

static HINT32	__hom_dmp_bus_message_handler( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_Msg_t *pstMsg )
{
	switch ( pstMsg->eMsgType )
	{
		case eBUS_MSGTYPE_EVENT :
			if (__hom_dmp_bus_event( pstMsg->uEnum.eEvt, pstMsg->eDataType, pstMsg->pvData ) != ERR_OK)
			{
				HxLOG_Error("Error> Cant process bus event(%d)!\n", pstMsg->uEnum.eEvt);
			}
			break;

		case eBUS_MSGTYPE_REQUEST :
			if ( eBUS_REQUEST_JSON_DECODE == pstMsg->uEnum.eReq && eBUS_DATATYPE_IPC_RECV == pstMsg->eDataType )
			{
				if (__hom_dmp_cmd_process( eSender, pstMsg->pvData ) != ERR_OK)
				{
					HxLOG_Error("Error> Cant process bus command!\n");
				}
			}
			break;

		case eBUS_MSGTYPE_RESPONSE :
			break;

		default :
			break;
	}
	return 0;
}

#define ________________Public_Functions___________________________
void	HOM_DMP_Run( void )
{
	HOMMA_BUS_HndFuncList_t stFuncList;
	HOMMA_BUS_Msg_t 		stMsg;

	HxSTD_MemSet(&stMsg, 0x00,sizeof(HOMMA_BUS_Msg_t));
	HxSTD_MemSet(&stFuncList, 0x00,sizeof(HOMMA_BUS_HndFuncList_t));
	s_stBusHnd = HOMMA_BUS_GetHandle( eBUS_TARGET_MOD_DMP );

	stFuncList.cbMsgHandleFunc = __hom_dmp_bus_message_handler;
	stFuncList.cbReqHandleFunc = __hom_dmp_bus_request_handler;
	stFuncList.cbRelHandleFunc = __hom_dmp_bus_release_handler;

	HOMMA_BUS_Register( s_stBusHnd, &stFuncList );
	DMP_RegisterEventCallback(__hom_dmp_event);

	stMsg.eMsgType = eBUS_MSGTYPE_EVENT;
	stMsg.uEnum.eEvt = 0;

	HOMMA_BUS_SendMsg( eBUS_TARGET_ALL, s_stBusHnd, &stMsg );
}
