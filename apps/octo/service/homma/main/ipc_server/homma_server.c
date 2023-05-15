/**************************************************************
*	@file	hom_server.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / WOON
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
#include <vkernel.h>
#include <hlib.h>

#include "homma_int.h"
#include "homma_server.h"
#include "homma_ipc_decode.h"

#include "homma_bus.h"
#include "define.h"

#define ________________Defines____________________________________

#define ________________Typedef____________________________________
typedef struct {
	HCHAR 				szName[64];
	HINT32 				nAccessCode;
	HOMMA_BUS_Target_e 	eTarget;
} homma_notifier_t;

typedef struct {

	HINT32				sock;
	HCHAR				*uds;
	HULONG				tid;

	BUS_Handle			bus;

	HxList_t			*sessions;
	HxList_t    		*notifiers;
	HINT32 				semaphore;
} homma_server_t;

typedef enum
{
	eHOMMA_Notifier_Start = 0,
	eHOMMA_Notifier_Stop,
	eHOMMA_Notifier_Status,
} Notifier_Type_e;

TAG2ENUM_t      TAG_HOMMA_Notifier_Type_Table =
{
	eTAG_Pair,
	{
		{   "start",        	eHOMMA_Notifier_Start},
		{   "stop",         	eHOMMA_Notifier_Stop},
		{   "status",       	eHOMMA_Notifier_Status},
		{   NULL,               -1                      }
	}
};

#define ________________Static_Variables___________________________
static homma_server_t	s_stServer;

#define ________________Internal_Functions_________________________
static homma_server_t	*__server( void )
{
	return &s_stServer;
}

/*-----------------------------------------------------------------------------
 *  Session
 *-----------------------------------------------------------------------------*/
HERROR __server_session_add(HINT32 sock)
{
	homma_server_t 	*server = __server();
	homma_session_t *session = HLIB_STD_MemCalloc(sizeof(homma_session_t));

	if ( !session )
	{
		HxLOG_Error("session is not allocated\n");
		return ERR_FAIL;
	}

	memset(session, 0, sizeof(homma_session_t));

	session->sock = sock;
	HxSEMT_Create(&session->semaphore, "homma_server_session_sem", 0);


	HxSEMT_Get(server->semaphore);
	server->sessions = HLIB_LIST_Append(server->sessions, session);
	HxSEMT_Release(server->semaphore);

	return ERR_OK;
}

homma_session_t *__server_session_find(HINT32 sock)
{
	homma_server_t 	*server = __server();
    HxList_t 		*pos = NULL;
	homma_session_t *session = NULL;

	HxSEMT_Get(server->semaphore);
    pos = HLIB_LIST_First(server->sessions);

    while ( pos )
    {

        session = HLIB_LIST_Data(pos);

		if(!session || session->sock == sock)
		{
			break;
		}

        pos = HLIB_LIST_Next(pos);
    }

	HxSEMT_Release(server->semaphore);
	return session;
}

HERROR	__server_session_del(HINT32 sock)
{
	homma_server_t 	*server = __server();
	homma_session_t *session = __server_session_find(sock);

	if ( session )
	{
		HxSEMT_Destroy(session->semaphore);

		HxSEMT_Get(server->semaphore);
		server->sessions = HLIB_LIST_Remove(server->sessions, session);
		HxSEMT_Release(server->semaphore);

		HOM_MEMFREE(session);
		return ERR_OK;
	}

	return ERR_FAIL;
}

static void __server_packet_changed (void *userData, HxSocket_Status_e eStatus, HINT32 clientHandle)
{
	homma_server_t *server = (homma_server_t *)userData;
    HERROR hErr = ERR_FAIL;

    if ( !server ) return;

	switch ( eStatus)
	{
		case HxSOCKET_ADD_CLIENT:
            hErr = __server_session_add(clientHandle);
            if (hErr != ERR_OK)
            {
                HxLOG_Error("__server_session_add failed(hErr=%d)\n", hErr);
            }
			break;

		case HxSOCKET_REMOVE_CLIENT:
		case HxSOCKET_DISCONNECT_CLIENT:
			__server_session_del(clientHandle);
			break;

		default:
			break;
	}
}

static const HCHAR * __server_find_module_name( HOMMA_BUS_Target_e eModule )
{
	switch ( eModule )
	{
		case eBUS_TARGET_MOD_DMP :
			return "dmp";
		case eBUS_TARGET_MOD_DMS :
			return "dms";
		case eBUS_TARGET_MOD_DMR :
			return "dmr";
		case eBUS_TARGET_MOD_WOONS :
			return "woonserver";
		case eBUS_TARGET_MOD_DIAL:
			return "dial";
		case eBUS_TARGET_MOD_AIRPLAY:
			return "airplay";
		case eBUS_TARGET_MOD_SATIP:
			return "satip";
		case eBUS_TARGET_MOD_RC:
			return "rc";

		default:
			return NULL;
	}

	return NULL;
}

static HUINT32 __server_get_accesscode( HOMMA_BUS_Target_e eTarget )
{
#if 0
	return (HINT32)HOMMA_BUS_GetHandle( eTarget );
#else
	/*  homma reconnection 시 access code 변동으로 인해 고정  */
#define	DEFAULT_ACCESS_CODE 0x0000AA00
	switch ( eTarget)
	{
		case eBUS_TARGET_MOD_DMP :
			return DEFAULT_ACCESS_CODE+1;
		case eBUS_TARGET_MOD_DMS :
			return DEFAULT_ACCESS_CODE+2;
		case eBUS_TARGET_MOD_DMR :
			return DEFAULT_ACCESS_CODE+3;
		case eBUS_TARGET_MOD_WOONS :
			return DEFAULT_ACCESS_CODE+4;
		case eBUS_TARGET_MOD_DIAL:
			return DEFAULT_ACCESS_CODE+5;
		case eBUS_TARGET_MOD_AIRPLAY:
			return DEFAULT_ACCESS_CODE+6;
		case eBUS_TARGET_MOD_SATIP:
			return DEFAULT_ACCESS_CODE+7;
		case eBUS_TARGET_MOD_RC:
			return DEFAULT_ACCESS_CODE+8;
		default:
			return 0;
	}

	return 0;
#endif
}

static HOMMA_BUS_Target_e __target_by_access(HUINT32 access)
{
 	HUINT32	i =0;

	for(i=0; i<eBUS_TARGET_MAX; i++)
	{
		if(access == __server_get_accesscode(i))
			return i;
	}

	return 0;
}

/*-----------------------------------------------------------------------------
 *  Notifier
 *-----------------------------------------------------------------------------*/
static void __server_packet_send( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_IpcSend_t *pstSendData )
{
	const HCHAR *module = NULL;
	HINT32 requestId = 0;
	HUINT32 access_code =  __server_get_accesscode( eSender );
	HINT32 clientSocket = 0;

	_hmpacket_t	packet;
	HUINT32	packet_size = 0;
	HINT8	*data = NULL;

	HxSTD_MemSet(&packet, 0, sizeof(_hmpacket_t));

	module = __server_find_module_name( eSender );
	if ( !module )
	{
		HxLOG_Error("Not found module .... ! \n");
		return ;
	}

	requestId = pstSendData->nPairId;
	clientSocket = pstSendData->nClientHandle;
	if ( !clientSocket )
	{
		HxLOG_Error("Client socket is not existed.. ! \n");
		return ;
	}

	packet.cmd = pstSendData->cmd;
	packet.access = access_code;
	packet.type = HOMxCommType_MESSAGE;
	packet.dlen = pstSendData->size;
	packet_size = pstSendData->size + sizeof(_hmpacket_t);
	if(packet_size <= 0)
	{
		HxLOG_Error("Invalid packet_size ! \n");
		return;
	}

	data = HLIB_STD_MemCalloc(packet_size);
	if(data == NULL)
	{
		HxLOG_Error("Fail memory alloc ! \n");
		return ;
	}

	if(sizeof(_hmpacket_t) > 0)
	{
		HxSTD_MemCopy(data, &packet, sizeof(_hmpacket_t));
	}

	if(pstSendData->size > 0)
	{
		HxSTD_MemCopy(data+sizeof(_hmpacket_t), pstSendData->data, pstSendData->size);
	}

	HLIB_SOCKET_Write(clientSocket, data, packet_size, requestId);
	HOM_MEMFREE(data);
}

static void __server_packet_notify( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_IpcSend_t *pstSendData )
{
	homma_server_t 		*server = __server();

	HxList_t			*pSessionList = NULL;
	HxList_t			*pNotifierList = NULL;

	HINT32 				i=0, cnt = 0;

	homma_notifier_t 	*pNoti = NULL;
	homma_session_t 	*session;

	HxSEMT_Get(server->semaphore);
	cnt = HLIB_LIST_Length(HLIB_LIST_First(server->sessions));


	for(i=0; i<cnt; i++)
	{
		pSessionList = HLIB_LIST_GetListItem(HLIB_LIST_First(server->sessions), i);
		session = HLIB_LIST_Data(pSessionList);

		if(!session)
		{
			continue;
		}

		HxSEMT_Get(session->semaphore);
		pNotifierList = HLIB_LIST_First(session->notifiers);



		if(!pNotifierList)
		{
			HxSEMT_Release(session->semaphore);
			continue;
		}

		while(pNotifierList)
		{
			pNoti = (homma_notifier_t *)HLIB_LIST_Data(pNotifierList);

			if(!pNoti || pNoti->eTarget != eSender)
			{
				pNotifierList = pNotifierList->next;
				continue;
			}
			else
			{
				_hmpacket_t packet;
				HINT8		*data = NULL;
				HUINT32		packet_size = 0;

				HxSTD_MemSet(&packet, 0x00, sizeof(_hmpacket_t));

				packet.service = (HINT8) eSender;
				packet.type = HOMxCommType_EVENT;
				packet.access = pNoti->nAccessCode;
				packet.operation = HOMxNotify_Event_Status;

				packet.dlen = pstSendData->size;
				packet_size = sizeof(_hmpacket_t) + packet.dlen;
				packet.cmd = pstSendData->cmd;

				HxSTD_snprintf(packet.notifier, sizeof(packet.notifier), "%s", pNoti->szName);
				data = HLIB_STD_MemCalloc(packet_size);
				HxSTD_MemCopy(data, &packet, sizeof(_hmpacket_t));
				HxSTD_MemCopy(data + sizeof(_hmpacket_t), pstSendData->pszJson, packet.dlen);

				if(HLIB_SOCKET_Write(session->sock, data, packet_size, 0) != ERR_OK)
					HxLOG_Critical("Write Fail !!! \n");

				HOM_MEMFREE(data);
			}

			pNotifierList = pNotifierList->next;
		}

		HxSEMT_Release(session->semaphore);

	}
	HxSEMT_Release(server->semaphore);
}

static HERROR	__server_notifier_decode(homma_session_t *session, _hmpacket_t *data)
{
	HOMMA_BUS_Target_e 	eTarget;
	homma_notifier_t	*p = NULL;
	HxList_t			*pList = NULL;

	HUINT32	AccessCode = data->access;
	HCHAR 	*pszNotifierName = NULL;

	if(data->data == NULL)
	{
		HxLOG_Error("notifier name is null . \n");
	   	return ERR_FAIL;
	}

	HxSEMT_Get(session->semaphore);
	pList = HLIB_LIST_First(session->notifiers);

	pszNotifierName = data->data;

	eTarget = __target_by_access(AccessCode);
	if(eTarget == 0)
	{
		HxLOG_Error("AccessCode(%x) Error !!! \n", data->access);
		HxSEMT_Release(session->semaphore);
		return ERR_FAIL;
	}

	while(pList)
	{
		homma_notifier_t *pNotify = HLIB_LIST_Data(pList);

		if((pNotify->nAccessCode == AccessCode) && HxSTD_StrCmp(pNotify->szName, pszNotifierName))
		{
			HxLOG_Error("already registed notifier \n");
			break;
		}
		pList = pList->next;
	}

	switch(data->operation)
	{
		case eHOMMA_Notifier_Start:
			{
				if(pList == NULL)
				{
					p = HLIB_STD_MemCalloc(sizeof(homma_notifier_t));
					if(p == NULL)
					{
						HxLOG_Critical("Fail Memory alloc . \n");
						HxSEMT_Release(session->semaphore);
						return ERR_FAIL;
					}

					HxSTD_snprintf(p->szName, sizeof(p->szName), "%s", pszNotifierName);
					// e.g. szName list
					// ----------------
					// dlna_appkit_1
					// dlna_appkit_2
					// dlna_appkit_3
					// dlna_appkit_4
					// dlna_appkit_5
					// dlna_appkit_6
					// dlna_appkit_7
					p->nAccessCode = AccessCode;
					p->eTarget = eTarget;

					session->notifiers = HLIB_LIST_Append(session->notifiers, (void *)p);
				}
			}
			break;

		case eHOMMA_Notifier_Stop:
			{
				if(pList != NULL)
				{
					p =  HLIB_LIST_Data(pList);
					session->notifiers = HLIB_LIST_Remove(session->notifiers, (void *)p);
					HOM_MEMFREE(p);
				}
			}
			break;

		default:
			break;
	}

	HxSEMT_Release(session->semaphore);
	return ERR_OK;
}

static void __server_packet_received (void *userData, HINT32 clientSocket, HUINT32 requestId, HINT32 size, void *pvData)
{
	_hmpacket_t		*data = pvData;
	homma_session_t *session = 	__server_session_find(clientSocket);
	HCHAR 			*add_data = NULL;

	if(data->dlen > 0)
	{
		add_data = HLIB_STD_MemCalloc(data->dlen+1);
		HxSTD_MemCopy(add_data, pvData + sizeof(_hmpacket_t), data->dlen);
		add_data[data->dlen] = '\0';
		data->data = add_data;
	}

	if(data->type == HOMxCommType_MESSAGE)
	{
		if(session)
        {
            session_send_to_msg(session, requestId, HLIB_STD_MemDup(data, sizeof(_hmpacket_t)), sizeof(_hmpacket_t));
        }
		else
        {
			HxLOG_Error("session search is failed(%x). \n", clientSocket);
        }
	}

	if(data->type == HOMxCommType_EVENT)
    {
		__server_notifier_decode(session, data);

        if (data->data != NULL)  // 2014/2/27 mhkang: Memory Leak
        {
            HLIB_STD_MemFree(data->data);
            data->data = NULL;
        }
    }
}

/*-----------------------------------------------------------------------------
 *  BUS Event
 *-----------------------------------------------------------------------------*/
static HINT32	__server_event_capture_from_dms( HINT32 event, const HOMMA_BUS_MsgEvt_t *pstMsgEvt, HOMMA_BUS_Target_e eSender )
{
	return 0;
}

static HINT32	__server_bus_release_handler( HOMMA_BUS_Target_e eSender, HOMMA_BUS_Msg_t *pstMsg )
{
    HOMxCommData_t* pData = NULL;

	switch ( pstMsg->eDataType )
	{
		case eBUS_DATATYPE_IPC_RECV :
			{
				HOMMA_BUS_IpcRecv_t *pIpcRecv = pstMsg->pvData;
				if ( pIpcRecv && pIpcRecv->bMalloc && pIpcRecv->pvJson )
				{
                    pData = (HOMxCommData_t*)pIpcRecv->pvJson;
                    if (pData->data != NULL)
                    {
                        HLIB_STD_MemFree(pData->data);
                        pData->data = NULL;
                    }

                    HOM_MEMFREE( pIpcRecv->pvJson );
					pIpcRecv->pvJson = NULL;

				}
			}
			break;
		default :
			HxLOG_Warning("Check memory leak! (eDataType:%d)\n", pstMsg->eDataType);
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

#if 0
static HINT32	__server_bus_evnet_handler( HINT32 event, HINT32 p1, HINT32 p2, HINT32 p3 )
{
	return 0;
}
#endif

static HINT32	__server_bus_request_handler( HOMMA_BUS_Request_e eReq, HOMMA_BUS_Data_t *pstRetData )
{
	return 0;
}

static HINT32	__server_bus_message_handler( HOMMA_BUS_Target_e eSender, const HOMMA_BUS_Msg_t *pstMsg )
{
	HOMMA_BUS_IpcSend_t *pSendData = NULL;

	switch ( pstMsg->eMsgType )
	{
		case eBUS_MSGTYPE_EVENT :
			break;

		case eBUS_MSGTYPE_REQUEST :
			if ( (eBUS_REQUEST_SERVER_SENDING == pstMsg->uEnum.eReq) && (eBUS_DATATYPE_IPC_SEDN == pstMsg->eDataType) )
			{
				pSendData = (HOMMA_BUS_IpcSend_t *)pstMsg->pvData;
				__server_packet_send( eSender, pSendData );
			}
			break;

		case eBUS_MSGTYPE_RESPONSE :
			if ( eBUS_REQUEST_SERVER_SENDING == pstMsg->uEnum.eRes )
			{
				pSendData = (HOMMA_BUS_IpcSend_t *)pstMsg->pvData;

				if(pSendData->nOpCode)
					__server_packet_notify(eSender, pSendData);
				else
					__server_packet_send(eSender, pSendData);
			}
			break;

		default :
			break;
	}
	return 0;
}

static void		__server_task(void *arg)
{
	homma_server_t 	*server = (homma_server_t *)arg;
	HLIB_SOCKET_Listen( server->sock, TRUE, __server_packet_received, __server_packet_changed, server);
}

#define ________________Public_Interfaces__________________________
void	HOMMA_SERVER_Run()
{
	HOMMA_BUS_HndFuncList_t stFuncList = { 0 };
	homma_server_t 	*server = __server();
	HUINT32	ulTaskId = 0;

	server = HxSTD_MemSet(server, 0x00, sizeof(homma_server_t));

	stFuncList.cbMsgHandleFunc = __server_bus_message_handler;
	stFuncList.cbReqHandleFunc = __server_bus_request_handler;
	stFuncList.cbRelHandleFunc = __server_bus_release_handler;

	HxLOG_Warning(HxANSI_COLOR_CYAN("HOMMA Server START !!!!\n"));

	server->sock	= HLIB_EXTRA_OpenServer("homma");
	server->bus 	= HOMMA_BUS_GetHandle(eBUS_TARGET_SERVER);

	HOMMA_BUS_Register( server->bus, &stFuncList );
	HOMMA_BUS_Connect( eBUS_TARGET_MOD_DMS, __server_event_capture_from_dms );

	HxSEMT_Create(&server->semaphore, "homma_server_sem", 0);

	VK_TASK_Create( __server_task, VK_TASK_PRIORITY_MW_REF, SIZE_16K, "hommaServerTask", (void*)server, (unsigned long*)&ulTaskId, 0);
	VK_TASK_Start(ulTaskId);
}

HBOOL	HOMMA_SERVER_Response( HINT32  nSock, const void *data, HUINT32 nSize, HUINT32 nReqId )
{
	HxLOG_Print("ROMA Server Respone!!!\n");
	return TRUE;
}

HUINT32 	HOMMA_SERVER_Broadcast( const void *data, HUINT32 size, HUINT32 reqId )
{
	return 0;
}
#define _____END_OF_FILE_________

