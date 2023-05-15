/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/


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

/**************************************************************************************************/
#define _________Nova_Private_Include_________________________________________________
/**************************************************************************************************/
#include <nova_int.h>

/**************************************************************************************************/
#define _________Nova_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Struct_________________________________________________
/**************************************************************************************************/
typedef struct
{
	HINT32			server;
	HINT32			nRpcHandle;
	HUINT32 		ulMainMsgQueue;
	HUINT32 		ulRequestShutdownTime;
	HUINT32 		ulWaitThreshold;
} nova_t;

/**************************************************************************************************/
#define _________Nova_Private_Value_________________________________________________
/**************************************************************************************************/
extern NOVA_Server_t g_ServerContext;

/**************************************************************************************************/
#define _________Nova_Private_Prototype_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_functions________________________________________________
/**************************************************************************************************/
static void		nova_server_on_status_change(void *userData, HxSocket_Status_e eStatus, HINT32 clientHandle)
{
	nova_t	*nova = (nova_t*)userData;

	if (eStatus == HxSOCKET_REMOVE_CLIENT)
		HLIB_RPC_RemoveClientHandle(nova->nRpcHandle, clientHandle);
}
static	void	nova_server_PacketReceived(void *userData, HINT32 socketHandle, HUINT32 requestId, HINT32 size, void *pvData)
{
	nova_t	*nova = (nova_t*)userData;
	HLIB_RPC_ProcessRPC(nova->nRpcHandle, size, pvData, requestId, socketHandle);
}
static void	nova_server_listener (void *arg)
{
	nova_t	*nova = (nova_t*)arg;
	HLIB_SOCKET_Listen(nova->server, TRUE, nova_server_PacketReceived, nova_server_on_status_change, arg);
}

static nova_t*	nova_getInstance(void)
{
	static nova_t	*s_pstNova = NULL;

	if (s_pstNova == NULL)
	{
		unsigned long	tid;

		s_pstNova = (nova_t*)AP_Malloc(sizeof(nova_t));
		s_pstNova->server = HLIB_EXTRA_OpenServer("nova");//HLIB_SOCKET_OpenServer(HxSOCKET_UDS, PAMA_SIGNAL_UDP);
		s_pstNova->nRpcHandle = HLIB_RPC_Open(s_pstNova->server, HLIB_STD_GetPID());

		VK_TASK_Create(nova_server_listener, VK_TASK_PRIORITY_MW_REF, SIZE_32K, "novasrv", (void*)s_pstNova, &tid, 0);
		VK_TASK_Start(tid);

		return s_pstNova;
	}

	return s_pstNova;
}


static HERROR	nova_OnStart(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	NOVA_MSG_t  stMsg;
	nova_t	    *nova           = (nova_t*)pvUserData;
	HCHAR       *friendlyName   = NULL;

	HxLOG_Trace();
	HxLOG_Assert(nova);
	HxSTD_MemSet(&stMsg,0x0,sizeof(NOVA_MSG_t));

	friendlyName = HxOBJECT_STR(apArgv[0]);
	stMsg.message = MGS_NOVA_RPC_START;
	if(friendlyName)
		stMsg.param1 = (HINT32)AP_StrDup(friendlyName);

	NOVA_MSG_Send(&stMsg);

	return ERR_OK;
}


static HERROR	nova_OnStop(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	NOVA_MSG_t  stMsg;
	nova_t	    *nova = (nova_t*)pvUserData;

	HxLOG_Trace();
	HxLOG_Assert(nova);
	HxSTD_MemSet(&stMsg,0x0,sizeof(NOVA_MSG_t));

	stMsg.message = MGS_NOVA_RPC_STOP;

	NOVA_MSG_Send(&stMsg);

	return ERR_OK;
}

static HERROR	nova_OnSetFriendlyName(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	NOVA_MSG_t  stMsg;
	nova_t	    *nova           = (nova_t*)pvUserData;
	HCHAR       *friendlyName   = NULL;

	HxLOG_Trace();
	HxLOG_Assert(nova);
	HxSTD_MemSet(&stMsg,0x0,sizeof(NOVA_MSG_t));

	friendlyName = HxOBJECT_STR(apArgv[0]);
	stMsg.message = MGS_NOVA_RPC_SET_FRIENDLYNAME;
	if(friendlyName)
		stMsg.param1 = (HINT32)AP_StrDup(friendlyName);

	NOVA_MSG_Send(&stMsg);

	return ERR_OK;

}
static void nova_registerRpcMethods(nova_t *nova)
{
	HLIB_RPC_RegisterCall(nova->nRpcHandle, RPC_NOVAAPI_START, "s", nova_OnStart,	nova, "Nova SAT>IP Start (char *FriendlyName)");
	HLIB_RPC_RegisterCall(nova->nRpcHandle, RPC_NOVAAPI_STOP, NULL, nova_OnStop,	nova, "Nova SAT>IP Stop");
	HLIB_RPC_RegisterCall(nova->nRpcHandle, RPC_NOVAAPI_SET_FRIENDLYNAME, "s", nova_OnSetFriendlyName,	nova, "Nova SAT>IP SetFriendlyName (char *FriendlyName)");
}


static void nova_registerRpcCallbacks(nova_t *nova)
{
	(void)HLIB_RPC_CreateNotification (nova->nRpcHandle, RPC_NOVA_EVENT, "i",
		"Notifying the Nova status:\n"
		"\t - (void *data,int size)");

}


/**************************************************************************************************/
#define _________Nova_Public_Functions__________________________________________________________
/**************************************************************************************************/

void NOVA_RPC_Init(void)
{
	nova_t	*nova =  nova_getInstance();

	HxLOG_Trace();
	if(nova == NULL) return;
	nova_registerRpcMethods(nova);
	nova_registerRpcCallbacks(nova);
	HxLOG_Debug("************** NOVA_RPC_Init Done ******************\n");
}

HERROR NOVA_RPC_NotifyEvent(NOVAAPI_Event_e eEvt)
{
	HERROR  hErr;
	nova_t	*nova =  nova_getInstance();

	HxLOG_Trace();
	if(nova == NULL) return ERR_FAIL;

	hErr = HLIB_RPC_Notify(nova->nRpcHandle, RPC_NOVA_EVENT,"i",eEvt);

	return hErr;
}


void NOVA_RPC_Msg(NOVA_MSG_t *pstMsgData)
{
	HxLOG_Trace();

	switch(pstMsgData->message)
	{
	case MGS_NOVA_RPC_START:
		printf(HxANSI_COLOR_PURPLE("[MGS_NOVA_RPC_START]> %s\n"),(HCHAR*)pstMsgData->param1);

		g_ServerContext.isServerActive = true;
		sprintf(g_ServerContext.modelName, (char*)pstMsgData->param1);

		if(NOVA_ServerStart((HCHAR*)pstMsgData->param1) == false)
		{
			HxLOG_Error(HxANSI_COLOR_RED("NOVA_ServerInit failed\n"));
		}
		AP_Free((HCHAR*)pstMsgData->param1);
		break;
	case MGS_NOVA_RPC_STOP:
		printf(HxANSI_COLOR_PURPLE("[MGS_NOVA_RPC_STOP]> \n"));

		g_ServerContext.isServerActive = false;

		NOVA_ServerShutdown();
		break;
	case MGS_NOVA_RPC_SET_FRIENDLYNAME:
		printf(HxANSI_COLOR_PURPLE("[MGS_NOVA_RPC_SET_FRIENDLYNAME]> %s\n"),(HCHAR*)pstMsgData->param1);

		sprintf(g_ServerContext.modelName, (char*)pstMsgData->param1);

		if (NOVA_ServerSetFriendlyName((HCHAR*)pstMsgData->param1) == false)
		{
			HxLOG_Error(HxANSI_COLOR_RED("NOVA_ServerSetFriendlyName failed\n"));
		}
		AP_Free((HCHAR*)pstMsgData->param1);
		break;
	default:
		printf("[NotImplement]msg:0x%x,p1:%x,p2:%x,p3:%x,p4:%x",
			pstMsgData->message,pstMsgData->param1,pstMsgData->param2,pstMsgData->param3,pstMsgData->param4);
		break;
	}
}


void NOVA_PRC_Print(void)
{
	nova_t	*nova =  nova_getInstance();

	HxLOG_Trace();
	if(nova == NULL) return;
	HLIB_RPC_PrintHelp(nova->nRpcHandle, NULL);
}



/**************************************************************************************************/
#define _________Nova__End_Of_File____________________________________________________
/**************************************************************************************************/


