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
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


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

/**************************************************************************************************/
#define _________Nova_Private_Include_________________________________________________
/**************************************************************************************************/
#include <vkernel.h>
#include <hlib.h>

#include <nova_api.h>


/**************************************************************************************************/
#define _________Nova_Private_Macro_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Struct_________________________________________________
/**************************************************************************************************/
typedef struct{
	HINT32			        nClientSocketInfo;
	HINT32			        nRpcHandle;
	HUINT32                 ulSemId;
	NOVAAPI_EventCallback   fnCallback;
}novaapi_context;

/**************************************************************************************************/
#define _________Nova_Private_Value_________________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define _________Nova_Private_Prototype_________________________________________________
/**************************************************************************************************/
STATIC novaapi_context *novaapi_getinstance(void);
STATIC void             novaapi_releaseinstance(novaapi_context *context);
/**************************************************************************************************/
#define _________Nova_Private_functions________________________________________________
/**************************************************************************************************/

STATIC HERROR novapi_OnEvent(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	novaapi_context		*context        = (novaapi_context*)pvUserData;
	HERROR			    hErr            = ERR_FAIL;
	HUINT32             *pResultData    = NULL;
	NOVAAPI_Event_e     eEvt;

	//확장???
	if (1 != nArgc)
	{
		HxLOG_Error ("Argument not valid: nova.onEvent ()\n");
		return ERR_FAIL;
	}

	eEvt = HxOBJECT_INT(apArgv[0]);

	switch(eEvt)
	{
	case eNOVAAPI_SATIP_Event_Started:
	case eNOVAAPI_SATIP_Event_Stopped:
	case eNOVAAPI_SATIP_Event_Status:
		{
			//	if(data != NULL && size != 0)
			//		pResultData = HLIB_STD_MemDup(data, size);
		}
		break;
	}

	if (context->fnCallback)
		context->fnCallback(eEvt, (HUINT32)pResultData, (HUINT32)1);

	if ( pResultData )
		HLIB_STD_MemFree( pResultData );

	return hErr;
}


STATIC void novaapi_RegisterRpcCallback(novaapi_context *context)
{
	HLIB_RPC_RegisterNotifyCallback(context->nRpcHandle, RPC_NOVA_EVENT, "i",context, novapi_OnEvent);
}


STATIC void novaapi_Disconnect(void)
{
	novaapi_context *context = novaapi_getinstance();

	if(context==NULL) return;

	context->nClientSocketInfo = 0;
	HLIB_RPC_Close(context->nRpcHandle);
	novaapi_releaseinstance(context);

}

STATIC void novaapi_Connect(novaapi_context *context)
{
	context->nClientSocketInfo		= HLIB_EXTRA_OpenClient("nova");//HLIB_SOCKET_OpenClient(HxSOCKET_UDS, SAMA_SIGNAL_UDS, 0);
	context->nRpcHandle		        = HLIB_RPC_Open(context->nClientSocketInfo, HLIB_STD_GetPID());
	novaapi_RegisterRpcCallback(context);
}




STATIC void novaapi_onPacketReceived (void *pvUserData, HINT32 nSocket, HUINT32 ulRequestId, HINT32 nSize, void *pvData)
{
	novaapi_context		*pstInst = (novaapi_context*)pvUserData;
	HxSEMT_Get(pstInst->ulSemId);
	HLIB_RPC_ProcessRPC(pstInst->nRpcHandle, (HUINT32)nSize, pvData, ulRequestId, nSocket);
	HxSEMT_Release(pstInst->ulSemId);

	printf("-\n");
}

STATIC void novaapi_onStatusChanged (void *pvUserData, HxSocket_Status_e eStatus, HINT32 nSocket)
{
	novaapi_context		*pstInst = (novaapi_context*)pvUserData;

	HxLOG_Debug("(+) eStatus(%d)\n", eStatus);

	switch (eStatus)
	{
	case HxSOCKET_DISCONNECT_CLIENT:
		HxLOG_Critical("HxSOCKET_DISCONNECT_CLIENT!!!\n");
		HxSEMT_Get(pstInst->ulSemId);
		novaapi_Disconnect();
		HxSEMT_Release(pstInst->ulSemId);
		break;

	default:
		break;
	}

	HxLOG_Debug("(-)\n");
}



STATIC void novaapi_socketListener (void *arg)
{
	novaapi_context 	*pcontext = (novaapi_context *)arg;

	while(1)
	{
		if(pcontext->nClientSocketInfo)
		{
			HLIB_SOCKET_ListenTimeout(pcontext->nClientSocketInfo,10, novaapi_onPacketReceived, novaapi_onStatusChanged, (void *)pcontext);
		}
		else
		{
			HLIB_STD_TaskSleep(100);
			HxSEMT_Get(pcontext->ulSemId);
			novaapi_Connect(pcontext);
			HxSEMT_Release(pcontext->ulSemId);
		}
	}
}


STATIC novaapi_context *novaapi_getinstance(void)
{
	static novaapi_context *s_pcontext = NULL;
	HULONG                  ulTaskId=0;

	if(s_pcontext == NULL)
	{
		s_pcontext = (novaapi_context*)HLIB_MEM_Calloc(sizeof(novaapi_context));
		(void)HxSEMT_Create(&(s_pcontext->ulSemId), "novaapi_sem", HxSEMT_FIFO);
		HxSEMT_Get(s_pcontext->ulSemId);
		novaapi_Connect(s_pcontext);
		(void)VK_TASK_Create(novaapi_socketListener, 64, SIZE_512K, "novaapi_listener", (void *)s_pcontext, &ulTaskId, 0);
		(void)VK_TASK_Start(ulTaskId);
		HxLOG_Warning("** (%s) is connected to Nova\n", HLIB_STD_GetCurProcessName());
	}
	else
	{
		HxSEMT_Get(s_pcontext->ulSemId);
	}
	return s_pcontext;
}

STATIC void novaapi_releaseinstance(novaapi_context *context)
{
	HxSEMT_Release(context->ulSemId);
}
/**************************************************************************************************/
#define _________Nova_Public_Functions__________________________________________________________
/**************************************************************************************************/
void	NOVAAPI_SATIP_Init( void )
{
}

void	NOVAAPI_SATIP_DeInit( void )
{

}

void	NOVAAPI_SATIP_RegisterNotifier( NOVAAPI_EventCallback pfnObjectHandler )
{
	novaapi_context 	*nova  = novaapi_getinstance();

	HxLOG_Trace();
	if(nova == NULL) return;

	nova->fnCallback= pfnObjectHandler;
	novaapi_releaseinstance(nova);
}

void	NOVAAPI_SATIP_UnRegisterNotifier( void )
{
	novaapi_context 	*nova  = novaapi_getinstance();

	HxLOG_Trace();
	if(nova == NULL)    return;

	nova->fnCallback= NULL;
	novaapi_releaseinstance(nova);
}

HERROR	NOVAAPI_SATIP_Start( HCHAR *pFriendlyName)
{
#if defined(CONFIG_SUPPORT_NOVA_APP)
	novaapi_context			*nova = NULL;
	HINT32 ret = 0;

	HxLOG_Trace();
	nova = novaapi_getinstance();
	if(nova==NULL)
		return ERR_FAIL;

	ret = HLIB_RPC_Call(nova->nRpcHandle, RPC_NOVAAPI_START, "s", pFriendlyName);
	if(ret == -1)
	{
		novaapi_releaseinstance(nova);
		return ERR_FAIL;
	}
	novaapi_releaseinstance(nova);
	return ERR_OK;
#else
	HxLOG_Error("### (NOVAAPI_SATIP_Start) Not Support SATIP Server###\n");
	return ERR_FAIL;
#endif
}

HERROR	NOVAAPI_SATIP_Stop( void )
{
#if defined(CONFIG_SUPPORT_NOVA_APP)
	novaapi_context			*nova = NULL;
	HINT32 ret = 0;

	HxLOG_Trace();
	nova = novaapi_getinstance();
	if(nova==NULL)
		return ERR_FAIL;

	ret = HLIB_RPC_Call(nova->nRpcHandle, RPC_NOVAAPI_STOP,NULL);
	if(ret == -1)
	{
		novaapi_releaseinstance(nova);
		return ERR_FAIL;
	}
	novaapi_releaseinstance(nova);
	return ERR_OK;
#else
	HxLOG_Error("### (NOVAAPI_SATIP_Stop) Not Support SATIP Server###\n");
	return ERR_FAIL;
#endif
}

HERROR	NOVAAPI_SATIP_SetFriendlyName(const  HCHAR *pFriendlyName)
{
#if defined(CONFIG_SUPPORT_NOVA_APP)
	novaapi_context			*nova = NULL;
	HINT32 ret = 0;

	HxLOG_Trace();
	nova = novaapi_getinstance();
	if(nova==NULL)
		return ERR_FAIL;

	ret = HLIB_RPC_Call(nova->nRpcHandle, RPC_NOVAAPI_SET_FRIENDLYNAME,"s",pFriendlyName);
	if(ret == -1)
	{
		novaapi_releaseinstance(nova);
		return ERR_FAIL;
	}
	novaapi_releaseinstance(nova);
	return ERR_OK;
#else
	HxLOG_Error("### (NOVAAPI_SATIP_SetFriendlyName) Not Support SATIP Server###\n");
	return ERR_FAIL;
#endif
}





/**************************************************************************************************/
#define _________Nova__End_Of_File____________________________________________________
/**************************************************************************************************/


