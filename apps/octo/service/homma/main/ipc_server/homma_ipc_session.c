/**************************************************************
*	@file	hom_server.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			DLNA / WOON
**************************************************************/

/*
* ¨Ï 2011-2012 Humax Co., Ltd.
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
	HCHAR		*data;
	HINT32		len;
	HUINT32		req_id;
} session_msg_t;

#define ________________Static_Variables___________________________
typedef enum {
    SESSION_MSGQ_SIZE       = 32,
    SESSION_MSGQ_TIMEOUT    = 500
} eGlobalNumaServer;

#define ________________Internal_Functions_________________________
static HOMMA_BUS_Target_e __server_find_module( HOMxService_e svc)
{
	switch(svc)
	{
		case eHOMAPI_Service_DMP:	return eBUS_TARGET_MOD_DMP;
		case eHOMAPI_Service_DMR:	return eBUS_TARGET_MOD_DMR;
		case eHOMAPI_Service_DMS:	return eBUS_TARGET_MOD_DMS;
		case eHOMAPI_Service_WoonServer:	return eBUS_TARGET_MOD_WOONS;
		case eHOMAPI_Service_Dial:			return eBUS_TARGET_MOD_DIAL;
		case eHOMAPI_Service_AIRPLAY:		return eBUS_TARGET_MOD_AIRPLAY;
		case eHOMAPI_Service_SatIpServer:	return eBUS_TARGET_MOD_SATIP;
		case eHOMAPI_Service_RC:			return eBUS_TARGET_MOD_RC;
		default: return eBUS_TARGET_NONE;
	}

	return eBUS_TARGET_NONE;
}


static void __session_message_process(homma_session_t *session, HUINT32 requestId, void *pvData, HINT32 size)
{
	HOMMA_BUS_Msg_t busMsg = { 0 };
	HOMMA_BUS_IpcRecv_t *pRecvData = NULL;
	HOMMA_BUS_Target_e eTarget = eBUS_TARGET_NONE;
	BUS_Handle bus 	= HOMMA_BUS_GetHandle(eBUS_TARGET_SERVER);
	HOMxCommData_t *data = NULL;
	_hmpacket_t recv;

	HxSTD_MemCopy(&recv, pvData, sizeof(_hmpacket_t));

	pRecvData = (HOMMA_BUS_IpcRecv_t *)HLIB_STD_MemAlloc( sizeof(HOMMA_BUS_IpcRecv_t) );
	HxSTD_MemSet(pRecvData, 0x0, sizeof(HOMMA_BUS_IpcRecv_t));
	eTarget = __server_find_module(recv.service);

	switch(recv.operation )
	{
		case HOMxDataOperation_GET:
			{
				pRecvData->nOpCode = eBUS_CMD_GET;
				session->queries = requestId;
				break;
			}
		case HOMxDataOperation_SET:
			{
				pRecvData->nOpCode = eBUS_CMD_SET;
				break;
			}
		case HOMxDataOperation_STATUS:
			{
				pRecvData->nOpCode = eBUS_CMD_STATUS;
				break;
			}
	}

	pRecvData->nClientHandle = session->sock;
	pRecvData->nPairId = requestId ;

	data = HLIB_STD_MemCalloc(sizeof(HOMxCommData_t));
    HxSTD_MemSet(data, 0 , sizeof(HOMxCommData_t));
	data->cmd = recv.cmd;

	if(recv.dlen > 0)
	{
		data->data = HLIB_STD_MemDup( recv.data, recv.dlen);
		data->size = recv.dlen;
	}
	pRecvData->pvJson = data ;

	pRecvData->bMalloc = TRUE;

	memset(&busMsg, 0, sizeof(HOMMA_BUS_Msg_t));
	busMsg.eMsgType 	= eBUS_MSGTYPE_REQUEST;
	busMsg.uEnum.eReq 	= eBUS_REQUEST_JSON_DECODE;
	busMsg.eDataType 	= eBUS_DATATYPE_IPC_RECV;
	busMsg.nDataSize 	= sizeof(HOMMA_BUS_IpcRecv_t);
	busMsg.bNeedFree 	= TRUE;
	busMsg.pvData 		= (void *)pRecvData;

	HOMMA_BUS_SendMsg( eTarget, bus, &busMsg );

    // 2014/2.26 mhkang: Memory Leak
    if (recv.data != NULL)
    {
        HLIB_STD_MemFree(recv.data);
        recv.data = NULL;
    }

    if (pvData != NULL)
    {
        HLIB_STD_MemFree(pvData);
        pvData = NULL;
    }
}

#define ________________Public_Interfaces__________________________

HERROR session_send_to_msg(homma_session_t *session, HUINT32 requestId, const HCHAR *data, HINT32 len)
{
	__session_message_process(session, requestId, (void*)data, len);
	
	return ERR_OK;
}

#define _____END_OF_FILE_________
