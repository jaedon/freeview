/*************************************************************************************************************
	File            : homapi.h
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

#include <hlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <vkernel.h>

#include <homapi.h>
#include <homma_int.h>
#include <homapi_dmp.h>
#include <homapi_dmr.h>
#include <homapi_dms.h>
#include <homapi_dial.h>
#include <hapi.h>

#define _________Private_Structure____________________________________________

typedef struct
{
	HBOOL		_isInitialized;
	pid_t       stProcPid;

	/* socket */
	HINT32      nSocketHandle;
	HINT32      pstSocketInfo;

	/* List */
	HxList_t    *pstNotifierList;

	/* semaphore */
	HUINT32     ulSemId;
	HUINT32     ulQuerySemId;
	HUINT32     ulConnectSemId;

	/* proc */
	HUINT32		ulSocketProcessId;
	HUINT32		ulPacketProcessId;
	HUINT32		ulMessageProcessId;

	/* message queue */
	HUINT32 	ulCommMsgQueueId;
	HUINT32  	ulProcessMsgQueueId;

	/* RPC Status */
	HBOOL		bConnectedToHomma;
 } homapi_ctx_t;

typedef struct
{
	HBOOL       bSuccess;
	HUINT8      *pucData;
	HINT32      nSize;
	HUINT32     ulRequestId;
} tempresult_t;

typedef struct
{
	HBOOL       bUsed;
	HUINT32     ulRequestId;
	void        *pvUserData;
} waitqueue_t;

typedef struct
{
	HUINT32     ulRequestId;
	HINT32      nSize;
	void        *pvData;
} rx_data_msg_t;

typedef struct
{
	void 		*param;
	HUINT32		size;
	HUINT32		cmd;
	HUINT32     access_code;
	HCHAR       notifierName[64];
} notify_data_msg_t;

typedef struct
{
	HUINT32     ulAccessCode;
	HCHAR       szNotifierName[64];

	HomApiNotifier pfnCallback;
} notifier_t;


#define _________Define____________________________________________
#define HOMMA_SIGNAL_FILE		"/tmp/.homma_signal.tmp"


#define _________HOMMA_API_Private_Value_________________________________________________

static homapi_ctx_t   *api_ctx = NULL;

/* wait queue */
static HxCBUF_t    	s_hCBuffer = NULL;
static HxHASH_t    	*s_pstWaitingList = NULL;
/* notifier list */
static HxHASH_t    	*s_pstNotifierTable = NULL;

static HOMAPI_CheckHommaStatusCallback s_fnStatusCallback = NULL;
static HBOOL s_bHommaIsReady = FALSE;

#define _________HOMMA_API_Private_Mecro_________________________________________________

#define ENTER_CRITICAL      HxSEMT_Get(api_ctx->ulSemId)
#define LEAVE_CRITICAL      HxSEMT_Release(api_ctx->ulSemId)

#define ENTER_QUERY_CRITICAL	HxSEMT_Get(api_ctx->ulQuerySemId)
#define LEAVE_QUERY_CRITICAL	HxSEMT_Release(api_ctx->ulQuerySemId)

#define ENTER_CONNECT_CRITICAL	HxSEMT_Get(api_ctx->ulConnectSemId)
#define LEAVE_CONNECT_CRITICAL	HxSEMT_Release(api_ctx->ulConnectSemId)

#define _QUERY_WAITING_TIME        		5000 /* 30000 (msec) */

#define _________HOMMA_API_Private_Function_______________________________________________
static void __homma_disconnect(void);
static void __homma_connect(void);
static homapi_ctx_t*  __get_instance(void);
static HUINT32  __new_request_id(void);

/*-----------------------------------------------------------------------------------------------------*/
/* Packet Receive &  Send */
/*-----------------------------------------------------------------------------------------------------*/

static void     __socket_changeStatus (void *userData, HxSocket_Status_e eStatus, HINT32 nClientHandle)
{
	if (eStatus == HxSOCKET_DISCONNECT_CLIENT)
	{
		s_bHommaIsReady = FALSE;

		__homma_disconnect();
		HLIB_SOCKET_Close(nClientHandle);

		if(s_fnStatusCallback)	s_fnStatusCallback(FALSE);
	}
}

static void     __socket_receivePacket (void *userData, HINT32 nSocketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	rx_data_msg_t	stMsg;

	stMsg.ulRequestId = requestId;
	stMsg.nSize       = size;
	if (size)
	{
		stMsg.pvData = HLIB_STD_MemDup(pvData, size);
	}
	else
	{
		stMsg.pvData = NULL;
	}

	if (VK_OK != VK_MSG_Send(api_ctx->ulCommMsgQueueId, (void*)&stMsg, sizeof(rx_data_msg_t)))
	{
		HxLOG_Error("Error> failed to sendmsg[qid:%d]\n", api_ctx->ulCommMsgQueueId);
		return;
	}
}

static HERROR __socket_sendPacket (homapi_ctx_t *_ctx, _hmpacket_t *pszDataString, HUINT32 ulRequestId)
{
	HUINT32 size = sizeof(_hmpacket_t);
	HINT8	*buf = NULL;

	if (pszDataString->dlen) size+=pszDataString->dlen;
	//HxLOG_Print("write[sock:%d, size:%d, dlen:%d, reqid:%d]\n", _ctx->nSocketHandle, size, pszDataString->dlen, ulRequestId);

	buf = HLIB_STD_MemCalloc(size);
	memcpy(buf, pszDataString, sizeof(_hmpacket_t));

	if (pszDataString->dlen)
		memcpy( buf + sizeof(_hmpacket_t), pszDataString->data, pszDataString->dlen);

	if (HLIB_SOCKET_Write(_ctx->nSocketHandle, buf, size, ulRequestId) != ERR_OK)
	{
		HxLOG_Error("Error> failed to write[sock:%d, size:%d, reqid:%d]\n", _ctx->nSocketHandle, size, ulRequestId);
		HLIB_STD_MemFree(buf);
		return ERR_FAIL;
	}

	HLIB_STD_MemFree(buf);
	return ERR_OK;
}

static void    	 __socket_listener (void *arg)
{
	homapi_ctx_t *_ctx = (homapi_ctx_t*)arg;

	while(1)
	{
		if(_ctx->nSocketHandle)
		{
			HLIB_SOCKET_Listen(_ctx->nSocketHandle, TRUE, __socket_receivePacket, __socket_changeStatus, NULL);
		}
		else
		{
			HLIB_STD_TaskSleep(100);
		}
	}
}

static void 	__notifier_process(HUINT8 *pucData, HINT32 nSize, HUINT32 ulRequestId)
{
	notify_data_msg_t	*msg = NULL;
	_hmpacket_t	*packet = (_hmpacket_t *)pucData;
	HCHAR			*param = NULL;

	if (packet->type != HOMxCommType_EVENT)
	{
		HxLOG_Error("Error> It's NOT notify packet. \n");
		return;
	}

	msg = (notify_data_msg_t*)HLIB_STD_MemCalloc(sizeof(notify_data_msg_t));
	msg->access_code = packet->access;
	HLIB_STD_StrNCpySafe(msg->notifierName, packet->notifier, 64);

	param = HLIB_STD_MemCalloc(packet->dlen);
	HxSTD_MemCopy(param, pucData + sizeof(_hmpacket_t), packet->dlen);

	msg->param = param;
	msg->size = packet->dlen;
	msg->cmd = packet->cmd;

	VK_MSG_Send(api_ctx->ulProcessMsgQueueId, (void*)&msg, sizeof(notify_data_msg_t));
}

/* packet process */
static void		 __comm_processer(void *param)
{
	rx_data_msg_t	msg;
	waitqueue_t  *pstWaitingInfo = NULL;

	while (1)
	{
		if (VK_MSG_Receive(api_ctx->ulCommMsgQueueId, (void*)&msg, sizeof(rx_data_msg_t)) == VK_OK)
		{
			ENTER_CRITICAL;

			/* request(get) response */
			pstWaitingInfo = HLIB_HASH_Lookup(s_pstWaitingList, (void*)msg.ulRequestId);
			if (pstWaitingInfo)
			{
				//HxLOG_Warning("[%s:%d] Found Query in Waiting list .... !!! \n", __FUNCTION__, __LINE__);

				if (pstWaitingInfo->bUsed == FALSE)
				{
					HxLOG_Warning("cb is already removed!!!\n");
				}
				else
				{
					tempresult_t   *pstResult = pstWaitingInfo->pvUserData;

					pstResult->bSuccess    = TRUE;
					pstResult->pucData     = HLIB_STD_MemDup(msg.pvData, msg.nSize);
					pstResult->nSize       = msg.nSize;
					pstResult->ulRequestId = 0xFFFFFFFF;

					HLIB_HASH_Remove(s_pstWaitingList, (void*)msg.ulRequestId);
					pstWaitingInfo->bUsed = FALSE;
				}
			}
			/* notify */
			else
			{
				//HxLOG_Warning("[%s:%d] Notifier Received Packet ! \n", __FUNCTION__, __LINE__);
				__notifier_process(msg.pvData, msg.nSize, msg.ulRequestId);
			}

			if (msg.pvData)
			{
				HLIB_STD_MemFree(msg.pvData);
				msg.pvData = NULL;
			}

			LEAVE_CRITICAL;
		}
	}
}

/*-----------------------------------------------------------------------------------------------------*/
/* Debug Command Tool */
/*-----------------------------------------------------------------------------------------------------*/

#ifdef  CONFIG_DEBUG
static HINT32   cmd_dmp_test(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if ( aucArg == NULL )    return HxCMD_ERR;
	if ( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )    return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if (VK_strcmp(aucStr, "START") == 0)
	{
		HLIB_CMD_Printf("\n*** homapi dmp start ***\n");
		HOMAPI_DMP_Start();
	}
	else if (VK_strcmp(aucStr, "STOP") == 0)
	{
		HLIB_CMD_Printf("\n*** homapi dmp stop ***\n");
		HOMAPI_DMP_Stop();
	}
	else
	{

	}

	return HxCMD_OK;
}

static HINT32   cmd_dmr_test(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if ( aucArg == NULL )    return HxCMD_ERR;
	if ( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )    return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if (VK_strcmp(aucStr, "START") == 0)
	{
		HLIB_CMD_Printf("\n*** homapi dmr start ***\n");
		HOMAPI_DMR_Start(NULL);
	}
	else if (VK_strcmp(aucStr, "STOP") == 0)
	{
		HLIB_CMD_Printf("\n*** homapi dmr stop ***\n");
		HOMAPI_DMR_Stop();
	}
	else
	{

	}


	return HxCMD_OK;
}

static HINT32   cmd_dms_test(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if ( aucArg == NULL )    return HxCMD_ERR;
	if ( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )    return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if (VK_strcmp(aucStr, "START") == 0)
	{
		HLIB_CMD_Printf("\n*** homapi dms start ***\n");
		HOMAPI_DMS_Start("HMS1000S_DMS", 1);
	}
	else if (VK_strcmp(aucStr, "STOP") == 0)
	{
		HLIB_CMD_Printf("\n*** homapi dms start ***\n");
		HOMAPI_DMS_Stop();
	}
	else
	{

	}

	return HxCMD_OK;
}

static HINT32   cmd_dial_test(void *szArgList)
{
	HCHAR    *aucArg=NULL, aucStr[20];

	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if ( aucArg == NULL )    return HxCMD_ERR;
	if ( sscanf(aucArg, "%s", (char *)&aucStr) == 0 )    return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if (VK_strcmp(aucStr, "START") == 0)
	{
		HLIB_CMD_Printf("\n*** homapi dial server start ***\n");
		HOMAPI_DIAL_Start("HMS1000S_DIAL");
	}
	else if (VK_strcmp(aucStr, "STOP") == 0)
	{
		HLIB_CMD_Printf("\n*** homapi dial server stop ***\n");
		HOMAPI_DIAL_Stop();
	}
	else if (VK_strcmp(aucStr, "REGAPP") == 0)
	{
		HLIB_CMD_Printf("\n*** homapi dial applicatino regist ***\n");
		HOMAPI_DIAL_AppRegist("YouTube");
	}
	else
	{

	}

	return HxCMD_OK;
}

#endif

/*-----------------------------------------------------------------------------------------------------*/
/* Event Notifier */
/*-----------------------------------------------------------------------------------------------------*/

static HBOOL   __notifier_findNotifier(void *pvNotifierName, void *pvNotifierInfo)
{
	HCHAR   	*pszNotifierName = (HCHAR*)pvNotifierName;
	notifier_t	*pstNotifierInfo = (notifier_t*)pvNotifierInfo;

	if (HxSTD_StrCmp(pszNotifierName, pstNotifierInfo->szNotifierName) == 0)
		return TRUE;

	return FALSE;
}

static void 	__notifier_add(HUINT32 ulAccessCode, const HCHAR *pszNotifierName, HomApiNotifier pfnCallback)
{
	HxList_t    *pstNotifierList;
	notifier_t 	*pstNotifierInfo;

	pstNotifierInfo = (notifier_t*)HLIB_STD_MemCalloc(sizeof(notifier_t));
	pstNotifierInfo->ulAccessCode = ulAccessCode;
	pstNotifierInfo->pfnCallback  = pfnCallback;
	HLIB_STD_StrNCpySafe(pstNotifierInfo->szNotifierName, pszNotifierName, 64);

	pstNotifierList = (HxList_t*)HLIB_HASH_Lookup(s_pstNotifierTable, (void*)ulAccessCode);
	if (pstNotifierList == NULL)
	{
		pstNotifierList = HLIB_LIST_Append(pstNotifierList, (void*)pstNotifierInfo);
		HLIB_HASH_Insert(s_pstNotifierTable, (void*)ulAccessCode, (void*)pstNotifierList);
		HxLOG_Debug("New Notifier < %s : %x > \n", pszNotifierName, ulAccessCode);

		/*  Test Code  */
		if (api_ctx != NULL)
			api_ctx->pstNotifierList = HLIB_LIST_Append(api_ctx->pstNotifierList, HLIB_STD_MemDup((void*)pstNotifierInfo, sizeof(notifier_t)));

		return;
	}

	if (HLIB_LIST_FindEx(pstNotifierList, (void*)pszNotifierName, __notifier_findNotifier))
	{
		HxLOG_Warning("notifier [%s] is already registered!\n", pszNotifierName);
		HLIB_STD_MemFree(pstNotifierInfo); pstNotifierInfo = NULL;
		return;
	}

	HLIB_LIST_Append(pstNotifierList, (void*)pstNotifierInfo);
}

static void 	__notifier_remove(HUINT32 ulAccessCode, const HCHAR *pszNotifierName)
{
	HxList_t    *pstNotifierList, *list;
	notifier_t *pstNotifierInfo;

	pstNotifierList = (HxList_t*)HLIB_HASH_Lookup(s_pstNotifierTable, (void*)ulAccessCode);
	if (pstNotifierList == NULL)
	{
		HxLOG_Error("notifier[%s] was not registered!\n", pszNotifierName);
		return;
	}

	list = HLIB_LIST_FindEx(pstNotifierList, (void*)pszNotifierName, __notifier_findNotifier);
	if (list == NULL)
	{
		HxLOG_Error("notifier[%s] was not registered!\n", pszNotifierName);
		return;
	}

	pstNotifierInfo = (notifier_t*)HLIB_LIST_Data(list);
	pstNotifierList = HLIB_LIST_Remove(pstNotifierList, (void*)pstNotifierInfo);
	HLIB_STD_MemFree(pstNotifierInfo);

	if (pstNotifierList == NULL)
		HLIB_HASH_Remove (s_pstNotifierTable, (void*)ulAccessCode);
	 else
		HLIB_HASH_Replace(s_pstNotifierTable, (void*)ulAccessCode, (void*)pstNotifierList);
}

static void		 __message_processer(void *param)
{
	HxList_t    *list;

	notify_data_msg_t	*msg = (notify_data_msg_t*)param;

	HxLOG_Assert(msg);

	while (1)
	{
		if (VK_MSG_Receive(api_ctx->ulProcessMsgQueueId, (void*)&msg, sizeof(notify_data_msg_t)) == VK_OK)
		{
			//ENTER_CRITICAL;

			list = HLIB_HASH_Lookup(s_pstNotifierTable, (void*)msg->access_code);
			if (list)
			{
				list = HLIB_LIST_FindEx(list, (void*)msg->notifierName, __notifier_findNotifier);
				if (list)
				{
					notifier_t * notifierInfo = (notifier_t*)HLIB_LIST_Data(list);

					if (notifierInfo->pfnCallback)
					{
						notifierInfo->pfnCallback(msg->access_code, msg->cmd, msg->param, msg->size);
					}
				}
			}

			//LEAVE_CRITICAL;

			if (msg->param != NULL)
				HLIB_STD_MemFree(msg->param);
		}
	}
}

/*-----------------------------------------------------------------------------------------------------*/
/* HOM API Init */
/*-----------------------------------------------------------------------------------------------------*/

static HERROR __onReadyProcess(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HCHAR szProcessName[128] = {0, };

	HxSTD_snprintf(szProcessName, sizeof(szProcessName), "%s", HxOBJECT_STR(apArgv[0]));

	if (HxSTD_StrNCmp(szProcessName, "homma", HxSTD_StrLen("homma")) == 0)
	{
		HxLOG_Debug("HOMAPI HAPI onReadyProcess : [%s]\n", HxOBJECT_STR(apArgv[0]));

		(void)__get_instance();
		__homma_connect();

		if(s_bHommaIsReady == FALSE)
		{
			s_bHommaIsReady = TRUE;
			
			if(s_fnStatusCallback)	s_fnStatusCallback(TRUE);
		}
	}

	return ERR_OK;
}

static void __homma_disconnect(void)
{
	homapi_ctx_t	*ctx = api_ctx;

	ENTER_CONNECT_CRITICAL;

	ctx->nSocketHandle = 0;
	ctx->bConnectedToHomma = FALSE;

	LEAVE_CONNECT_CRITICAL;

	HxLOG_Error(">>>>>>>>>>>>>>>>>>>>>>>>>> HOMMA DISCONNECT <<<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n");
}

static void __homma_connect(void)
{
	HERROR      herr = ERR_OK;
	homapi_ctx_t	*ctx = api_ctx;

	if (ctx == NULL)
	{
		HxLOG_Error("** homapi init error. ** \n");
		return;
	}

	ENTER_CONNECT_CRITICAL;

	if(ctx->bConnectedToHomma == TRUE)
	{
		HxLOG_Error("** homapi already connected to homma ** \n");
		LEAVE_CONNECT_CRITICAL;
		return;
	}

	HxLOG_Debug(">>>>>>>>>>>>>>>>>>>>>>>>>> HOMMA CONNECT <<<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n");

	ctx->pstSocketInfo = HLIB_EXTRA_OpenClient("homma");//HLIB_SOCKET_OpenClient(HxSOCKET_UDS, HOMMA_SIGNAL_UDS, 0);
	ctx->nSocketHandle = ctx->pstSocketInfo;

	HxLOG_Warning("** (%s) is connected to HOMMA.\n", HLIB_STD_GetCurProcessName());


	/*  TODO - regist notifier  */
	{
		_hmpacket_t	packet;
		HCHAR		szNotifierName[32] = {0, };
		HxList_t    *list = HLIB_LIST_First(ctx->pstNotifierList);
		notifier_t	*notifier = NULL;

		HxLOG_Debug("** homapi notifier(%d) regiet ** \n", HLIB_LIST_Length(list));

		while(list)
		{
			notifier = HLIB_LIST_Data(list);
			if (notifier)
			{
				HxSTD_snprintf(szNotifierName, 32, "%s", notifier->szNotifierName);

				//HxLOG_Warning("** homapi notifier regiet ** \n");
				HxSTD_MemSet(&packet, 0, sizeof(_hmpacket_t));
				packet.type = HOMxCommType_EVENT;
				packet.access = notifier->ulAccessCode;
				packet.operation = HOMxNotify_Event_Start;
				packet.dlen = HxSTD_StrLen(szNotifierName);
				packet.data = HLIB_STD_StrDup(szNotifierName);
				herr = __socket_sendPacket(ctx, &packet, __new_request_id());
				if (herr != ERR_OK)
				{
					HxLOG_Error("Write error! [sock:%d]\n", ctx->nSocketHandle);
				}

				HLIB_STD_MemFree(packet.data);
			}
			list = list->next;
		}
	}

	ctx->bConnectedToHomma = TRUE;

	LEAVE_CONNECT_CRITICAL;
}


static homapi_ctx_t*  __get_instance(void)
{
	if (api_ctx == NULL)
	{
		api_ctx = (homapi_ctx_t*)HLIB_STD_MemCalloc(sizeof(homapi_ctx_t));

		api_ctx->stProcPid     = getpid();

		HxSEMT_Create(&api_ctx->ulSemId, "homapi_sem", 0);
		HxSEMT_Create(&api_ctx->ulQuerySemId, "homapi_qsem", 0);
		HxSEMT_Create(&api_ctx->ulConnectSemId, "homapi_csem", 0);

		s_hCBuffer       = HLIB_CBUF_New((sizeof(waitqueue_t) * 512));

		/* Waiting List for HOMMA Response */
		s_pstWaitingList = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, NULL);

		/* Event Listener List */
		s_pstNotifierTable = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, NULL);

//		ENTER_CRITICAL;

		if ( VK_OK != VK_MSG_Create(64, sizeof(rx_data_msg_t), "rx_data_msgQ", (unsigned long*)&api_ctx->ulCommMsgQueueId, VK_SUSPENDTYPE_FIFO))
		{
			HxLOG_Critical("Critical Eror!!!\n");
			return NULL;
		}

		if ( VK_OK != VK_MSG_Create(64, sizeof(notify_data_msg_t), "rx_notify_msgQ", (unsigned long*)&api_ctx->ulProcessMsgQueueId, VK_SUSPENDTYPE_FIFO))
		{
			HxLOG_Critical("Critical Eror!!!\n");
			return NULL;
		}

		if ( VK_OK != VK_TASK_Create(__comm_processer, 70, SIZE_64K, "homapi_packet", (void*)api_ctx, (unsigned long*)&api_ctx->ulPacketProcessId, 0))
		{
			HxLOG_Critical("Critical Eror!!!\n");
			return NULL;
		}

		if ( VK_OK != VK_TASK_Start(api_ctx->ulPacketProcessId))
		{
			HxLOG_Critical("Critical Eror!!!\n");
			return NULL;
		}

		if ( VK_OK != VK_TASK_Create(__message_processer, 70, SIZE_64K, "homapi_processer", (void*)api_ctx, (unsigned long*)&api_ctx->ulMessageProcessId, 0))
		{
			HxLOG_Critical("Critical Eror!!!\n");
			return NULL;
		}

		if ( VK_OK != VK_TASK_Start(api_ctx->ulMessageProcessId))
		{
			HxLOG_Critical("Critical Eror!!!\n");
			return NULL;
		}

		if(s_bHommaIsReady == TRUE)
		{
			__homma_connect();
		}
		else
		{
			// It will try to connect to homma after receiving homma process ready signal
			HxLOG_Warning("** homma RPC server is not yet ready **\n");
		}

		if( VK_OK != VK_TASK_Create(__socket_listener, 70, SIZE_64K, "homapi_listener", (void *)api_ctx, (unsigned long*)&api_ctx->ulSocketProcessId, 0))
		{
			HxLOG_Critical("Critical Error!!!\n");
			return NULL;
		}

		if( VK_OK != VK_TASK_Start(api_ctx->ulSocketProcessId))
		{
			HxLOG_Critical("Critical Error!!!\n");
			return NULL;
		}

//		LEAVE_CRITICAL;

#ifdef  CONFIG_DEBUG
        HLIB_CMD_RegisterWord("homapi", cmd_dmp_test, "dmp", "start | stop ", "dmp");
        HLIB_CMD_RegisterWord("homapi", cmd_dmr_test, "dmr", "start | stop ", "dmr");
        HLIB_CMD_RegisterWord("homapi", cmd_dms_test, "dms", "start | stop ", "dms");
        HLIB_CMD_RegisterWord("homapi", cmd_dial_test, "dial", "start | stop | regapp", "dial server");
#endif
	}

	return api_ctx;

}

static HUINT32  __new_request_id(void)
{
	static HUINT32  s_ulRequestId = 0;

	s_ulRequestId = (s_ulRequestId + 1) & 0x0FFFFFFF;
	if (s_ulRequestId == 0xFFFFFFFF)
		s_ulRequestId = 0;

	return s_ulRequestId;
}

HERROR  _SendDataWithCallback(_hmpacket_t *pucData, void *pvUser )
{
	HERROR      herr = ERR_OK;
	HUINT32     ulReqId;
	HINT32  i = 512;
	waitqueue_t  *pstWaitQueue;

	ENTER_CRITICAL;

	pstWaitQueue = (waitqueue_t*)HLIB_CBUF_GetBuffer(s_hCBuffer, sizeof(waitqueue_t));

	while ((pstWaitQueue->bUsed == TRUE) && i--)
	{
		HxLOG_Critical("One of message is still waiting result from HOMMA\n");
		pstWaitQueue = (waitqueue_t*)HLIB_CBUF_GetBuffer(s_hCBuffer, sizeof(waitqueue_t));
	}

	if (pstWaitQueue->bUsed == TRUE)
	{
		HxLOG_Critical("HOM API Protocol is aborted due to circular full is full\n");
		HxLOG_Assert(0);
		LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	ulReqId = __new_request_id();

	pstWaitQueue->bUsed = TRUE;
	pstWaitQueue->pvUserData = pvUser;
	pstWaitQueue->ulRequestId = ulReqId;

	HxLOG_Debug("Insert WaitingList .... \n");
	HLIB_HASH_Insert(s_pstWaitingList, (void*)ulReqId, (void*)pstWaitQueue);

	herr = 	__socket_sendPacket(api_ctx, pucData, ulReqId);
	if (herr != ERR_OK)
	{
		HxLOG_Error("Write error \n");
		HLIB_HASH_Remove(s_pstWaitingList, (void*)ulReqId);
		pstWaitQueue->bUsed = FALSE;
		LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	LEAVE_CRITICAL;

	return ERR_OK;
}

HERROR  _HOM_API_Wait(HUINT32 ulWaitTick, tempresult_t *stResult)
{
	HUINT32     starttick;
	HUINT32     duration;
	HUINT32     ulSize = 0;

	duration  = 0;
	starttick = HLIB_STD_GetSystemTick();

	ENTER_CRITICAL;
	while (duration < ulWaitTick)
	{
		ulSize = HLIB_HASH_Size(s_pstWaitingList);

		//HxLOG_Debug("Waiting QUEUE size : [%d]\n", ulSize);

		if (ulSize == 0) break;

		LEAVE_CRITICAL;
		HLIB_STD_TaskSleep(10);
		duration = HLIB_STD_GetSystemTick() - starttick;
		ENTER_CRITICAL;
	}

//	if (duration < ulWaitTick || stResult->bSuccess == FALSE) {
	if (stResult->bSuccess == FALSE)
	{
		HxLOG_Warning("takes too long time to flush HOM API buffer!! (%d msec, size:%d)\n", duration, ulSize);
		HLIB_HASH_Remove(s_pstWaitingList, (void*)stResult->ulRequestId);
		LEAVE_CRITICAL;
		return ERR_FAIL;
	}

	LEAVE_CRITICAL;
	return ERR_OK;
}

HERROR 	_HOM_API_Query(_hmpacket_t *command, void *pvUserData)
{
	HERROR  herr = ERR_FAIL;
	tempresult_t   stResult;

	char **cbData = pvUserData;

	ENTER_QUERY_CRITICAL;

	HxSTD_MemSet(&stResult, 0x00, sizeof(tempresult_t));

	stResult.bSuccess = FALSE;
	herr = _SendDataWithCallback(command, (void*)&stResult);
	if (herr == ERR_OK)
	{
		herr = _HOM_API_Wait(_QUERY_WAITING_TIME, (void*)&stResult);
		if (herr == ERR_OK)
		{
			if (stResult.pucData)
			{
				*cbData = HLIB_STD_MemDup(stResult.pucData, stResult.nSize);
				HLIB_STD_MemFree(stResult.pucData);
			}
		}
	}

	LEAVE_QUERY_CRITICAL;

	return herr = ERR_OK;
}


#define _________HOMAPI_Public_Functions_____________________________________________

void    HOMAPI_Call (HOMxService_e target, void *data)
{
	HERROR      herr = ERR_OK;
	_hmpacket_t		packet;
	HOMxCommData_t	*pData = data;
	homapi_ctx_t	*_ctx = __get_instance();

	//HxLOG_Debug("++Target(%d) Command(%d) Size(%d) sock(%d) conn(%d)\n", target, pData->cmd, pData->size, _ctx->nSocketHandle, _ctx->bConnectedToHomma);

	if (_ctx->bConnectedToHomma == FALSE)
	{
		HxLOG_Error("Error> disconnect status !! \n");
		return;
	}

	HxSTD_MemSet(&packet, 0, sizeof(_hmpacket_t));

	packet.service = target;
	packet.type = HOMxCommType_MESSAGE;
	packet.operation = HOMxDataOperation_SET;
	packet.cmd = pData->cmd; /* HOMxDMP_Command_e */

	if (pData->size)
	{
		packet.data = HLIB_STD_MemDup(pData->data, pData->size);
		packet.dlen = pData->size;
	}
	else
	{
		packet.data = NULL; packet.dlen = 0;
	}

	herr = __socket_sendPacket(_ctx, &packet, __new_request_id());
	if (herr != ERR_OK)
	{
		HxLOG_Error("Write error! [sock:%d]\n", _ctx->nSocketHandle);
		return;
	}

	if (packet.data)
	{
		HLIB_STD_MemFree(packet.data); packet.data = NULL;
	}

	HxLOG_Debug("--Target(%d) Command(%d) \n", target, pData->cmd);
}

HUINT32 HOMAPI_Query (HOMxService_e target, void *data, void *userdata, HBOOL (* process) (HUINT32 cmd, void *, HUINT32 size, void *))
{
	HCHAR       *recv = NULL;
	HERROR      res;
	HUINT32 	access_code = 0;
	HOMxCommData_t	*pData = data;
	_hmpacket_t		packet, *recv_data;
	homapi_ctx_t	*_ctx = __get_instance();

	if (_ctx->bConnectedToHomma == FALSE)
	{
		HxLOG_Error("Error> disconnect status !! \n");
		return 0;
	}

	HxSTD_MemSet(&packet, 0, sizeof(_hmpacket_t));

	//packet.magic = 0xFEFB;
	packet.type = HOMxCommType_MESSAGE;
	packet.service = target;
	packet.operation = HOMxDataOperation_GET;

	if (pData != NULL)
	{
//		HxLOG_Warning("++Target(%d) Command(%d) sock(%d) conn(%d)\n", target, pData->cmd, _ctx->nSocketHandle, _ctx->bConnectedToHomma);

		if (pData->size)
		{
			packet.data = HLIB_STD_MemDup(pData->data, pData->size);
			packet.dlen = pData->size;
		}
		packet.cmd = pData->cmd;
	}
	else
	{
		packet.dlen = 0;
		packet.data = NULL;
		/* packet.cmd */
	}

	(void) __get_instance();

	res = _HOM_API_Query(&packet, (void*)&recv);
	if (res != ERR_OK)
	{
		HxLOG_Critical("Error> HOMAPI_Query is FAILED!!! \n");
		data = NULL;
		return 0;
	}

	if (packet.data)
	{
		HLIB_STD_MemFree(packet.data);
		packet.data = NULL;
	}

	if (recv != NULL)
	{
		recv_data = (_hmpacket_t *)recv;

		if (recv_data->access == 0)
		{
			HxLOG_Error("Error> It's invalid recv data. \n");
			HLIB_STD_MemFree(recv);
			return 0;
		}

		access_code = recv_data->access;

		if (process)
		{
			if (recv_data->dlen > 0) {
				(void) process(recv_data->cmd, recv+sizeof(_hmpacket_t), recv_data->dlen, userdata);
			} else {
				(void) process(recv_data->cmd, NULL, 0, userdata);
			}
		}

		HLIB_STD_MemFree(recv);
	}

	return access_code;
}


HERROR	HOMAPI_ConnectNotifier(HUINT32 ulAccessCode, HomApiNotifier pfnCallback)
{
	HERROR      herr = ERR_OK;
	_hmpacket_t	packet;
	static  HUINT32 _appkit_notifier_id = 0;
	homapi_ctx_t	*_ctx = __get_instance();
	HCHAR			szNotifierName[32] = {0, };

	HxSTD_snprintf(szNotifierName, 32, "dlna_appkit_%d", ++_appkit_notifier_id);

	HxSTD_MemSet(&packet, 0, sizeof(_hmpacket_t));

	packet.type = HOMxCommType_EVENT;
	packet.access = ulAccessCode;
	packet.operation = HOMxNotify_Event_Start;
	packet.dlen = HxSTD_StrLen(szNotifierName);
	packet.data = HLIB_STD_StrDup(szNotifierName);

	herr = __socket_sendPacket(_ctx, &packet, __new_request_id());
	if (herr != ERR_OK)
	{
		HxLOG_Error("Write error! [sock:%d]\n", _ctx->nSocketHandle);
		HLIB_STD_MemFree(packet.data);
		return ERR_FAIL;
	}

	__notifier_add(ulAccessCode, szNotifierName, pfnCallback);

	HLIB_STD_MemFree(packet.data);

	return ERR_OK;
}


HERROR	HOMAPI_DisconnectNotifier(HUINT32 ulAccessCode, HUINT32 notifier_id)
{
	HERROR herr = ERR_OK;
	_hmpacket_t			packet;
	HCHAR       		szNotifierName[32] = {0, };
	homapi_ctx_t      *_ctx = __get_instance();

	HxSTD_snprintf(szNotifierName, 32, "dlna_appkit_%d", notifier_id);

	HxSTD_MemSet(&packet, 0, sizeof(_hmpacket_t));

	packet.type = HOMxCommType_EVENT;
	packet.access = ulAccessCode;
	packet.operation = HOMxNotify_Event_Stop;
	packet.dlen = HxSTD_StrLen(szNotifierName);
	packet.data = HLIB_STD_StrDup(szNotifierName);

	herr = __socket_sendPacket(_ctx, &packet, __new_request_id());
	if (herr != ERR_OK)
	{
		HxLOG_Error("Write error! [sock:%d]\n", _ctx->nSocketHandle);
		HLIB_STD_MemFree(packet.data);
		return ERR_FAIL;
	}

	__notifier_remove(ulAccessCode, szNotifierName);

	HLIB_STD_MemFree(packet.data);

	return ERR_OK;
}

HERROR HOMAPI_RegisterCallbackAndCheckHommaStatus(HOMAPI_CheckHommaStatusCallback fnCallback)
{
	HxLOG_Trace();

	if(s_fnStatusCallback == NULL)
	{
		s_fnStatusCallback = fnCallback;
	}
	else
	{
		HxLOG_Warning("callback is already registered!\n");
	}

	if(HLIB_FILE_Exist(HOMMA_SIGNAL_FILE))
	{
		// send callback to appkit layer
		if(s_bHommaIsReady == FALSE)
		{
			s_bHommaIsReady = TRUE;
			
			if(s_fnStatusCallback)	s_fnStatusCallback(TRUE);
		}
	}
	else
	{
		s_bHommaIsReady = FALSE;

		// send callback to appkit layer
		if(s_fnStatusCallback)	s_fnStatusCallback(FALSE);
	}

	HAPI_ConnectSystemSignal("hama:onReadyProcess", "si", NULL, __onReadyProcess);

	return ERR_OK;
}

HERROR HOMAPI_UnRegisterCheckHommaStatus(void)
{
	HxLOG_Trace();

	s_fnStatusCallback = NULL;

	return ERR_OK;
}

