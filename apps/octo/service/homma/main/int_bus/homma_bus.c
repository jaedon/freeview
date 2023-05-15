/**************************************************************
*	@file	hom_bus.c
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

#define _____HEADER_INCLUDE______
#include <vkernel.h>
#include <hlib.h>

#include "hom_int.h"
#include "homma_bus.h"

#define _____PRIVATE_MACRO_______
#define	BUS_MSGQ_SIZE	128

#define _____PRIVATE_STRUCT______
typedef struct {
	HOMMA_BUS_Target_e eSrcType;
} BUS_Handle_t;

typedef	struct {
	BUS_Handle_t	*hHandId;
	BUS_MSG_HANDLE_Func		cbMsgHandleFunc;
	BUS_REQ_HANDLE_Func		cbReqHandleFunc;
	BUS_REL_HANDLE_Func		cbRelHandleFunc;
	HxList_t	*pNotifyList;
} BUS_ClientContext_t;

typedef	struct {
	HULONG 	ulMsgTaksId;
	HULONG	ulMsgQId;
	HULONG	ulEvtQId;

} BUS_TaskContext_t;

typedef	struct {
	HOMMA_BUS_Target_e eTarget;
	BUS_Handle_t	*pSrcHandle;
	HOMMA_BUS_Msg_t	stMsg;
} BUS_MsgData_t;

#define _____PRIVATE_VALUE_______

/* 이 부분은 Init을 통해 Memory 할당을 해야 할듯 */
static 	BUS_ClientContext_t s_stClientContext[eBUS_TARGET_MAX];
static 	BUS_TaskContext_t	s_stBusContext = { 0 };

#define _____PRIVATE_PROTOTYPE___

#define _____PRIVATE_INIT________
#define _____PRIVATE_DESTROY_____


#define _____PRIVATE_FUNCTIONS

STATIC BUS_TaskContext_t	*__bus_getTaskContext( void )
{
	return &s_stBusContext;
}

const HCHAR* __target(HOMMA_BUS_Target_e eTarget)
{
	switch (eTarget)
	{
		ENUM_TO_STR(eBUS_TARGET_NONE);
		ENUM_TO_STR(eBUS_TARGET_SYS);
		ENUM_TO_STR(eBUS_TARGET_SERVER);
		ENUM_TO_STR(eBUS_TARGET_MOD_MIN);
		ENUM_TO_STR(eBUS_TARGET_MOD_DMS);
		ENUM_TO_STR(eBUS_TARGET_MOD_DMP);
		ENUM_TO_STR(eBUS_TARGET_MOD_DMR);
		ENUM_TO_STR(eBUS_TARGET_MOD_WOONS);
		ENUM_TO_STR(eBUS_TARGET_MOD_DIAL);
		ENUM_TO_STR(eBUS_TARGET_MOD_AIRPLAY);
		ENUM_TO_STR(eBUS_TARGET_MOD_SATIP);
		ENUM_TO_STR(eBUS_TARGET_MOD_RC);

		ENUM_TO_STR(eBUS_TARGET_MOD_MAX);
		ENUM_TO_STR(eBUS_TARGET_MOD_ALL);
		ENUM_TO_STR(eBUS_TARGET_ALL);
		ENUM_TO_STR(eBUS_TARGET_MAX);

		default:
			break;
	}

	return ("Unknown Target");
}

STATIC BUS_ClientContext_t	*__bus_getClientContext( HOMMA_BUS_Target_e eTarget )
{
	if ( eBUS_TARGET_NONE > eTarget || eBUS_TARGET_MAX <= eTarget )
		return NULL;

	switch ( eTarget )
	{
		case eBUS_TARGET_NONE :
		case eBUS_TARGET_MOD_MIN :
		case eBUS_TARGET_MOD_MAX :
		case eBUS_TARGET_MOD_ALL :
		case eBUS_TARGET_ALL :
		case eBUS_TARGET_MAX :
			return NULL;
		default :
			break;
	}

	return  &s_stClientContext[eTarget];
}

inline HINT32	__bus_broadcastAll( HOMMA_BUS_MsgType_e eMsgType, BUS_Handle_t *pstHandle, const HOMMA_BUS_Msg_t *pstMsg )
{
	BUS_ClientContext_t *pClientCtx = NULL;
	HOMMA_BUS_Target_e exclMod = eBUS_TARGET_NONE;
	HINT32 i = 0;

	if( pstHandle )
	{
		exclMod = pstHandle->eSrcType;
	}

	if (!pstMsg )
	{
		return 1;
	}

	for ( i = 0;  i < eBUS_TARGET_MAX; i++ )
	{
		pClientCtx = NULL;
		if ( exclMod == i )
		{
			continue;
		}

		pClientCtx = __bus_getClientContext( (HOMMA_BUS_Target_e)i );
		if ( !pClientCtx )
		{
			continue;
		}

		if ( pClientCtx->cbMsgHandleFunc )
		{
			pClientCtx->cbMsgHandleFunc( exclMod, pstMsg );
		}
	}

	return 0;
}

static const char * s_PrintMsgType(HOMMA_BUS_MsgType_e e) {
  switch (e) {
	ENUM_TO_STR(eBUS_MSGTYPE_EVENT);
	ENUM_TO_STR(eBUS_MSGTYPE_REQUEST);
	ENUM_TO_STR(eBUS_MSGTYPE_RESPONSE);
	ENUM_TO_STR(eBUS_MSGTYPE_COMMAND);
	ENUM_TO_STR(eBUS_MSGTYPE_JSON);
	ENUM_TO_STR(eBUS_MSGTYPE_UNKNOWN);
	ENUM_TO_STR(eBUS_MSGTYPE_MAX);
  default:
	break;
  }
  return NULL;
}

inline HINT32	__bus_broadcastMod( HOMMA_BUS_MsgType_e eMsgType, BUS_Handle_t *pstHandle, const HOMMA_BUS_Msg_t *pstMsg )
{
	BUS_ClientContext_t *pClientCtx = NULL;
	HOMMA_BUS_Target_e exclMod = eBUS_TARGET_NONE;
	HINT32 i = 0;

	if( pstHandle )
	{
		exclMod = pstHandle->eSrcType;
	}

	for ( i = eBUS_TARGET_MOD_MIN+1 ;  i < eBUS_TARGET_MOD_MAX; i++ )
	{
		pClientCtx = NULL;
		if ( exclMod == i )
		{
			continue;
		}

		pClientCtx = __bus_getClientContext( (HOMMA_BUS_Target_e)i );
		if ( !pClientCtx )
		{
			continue;
		}

		if ( pClientCtx->cbMsgHandleFunc )
		{
			pClientCtx->cbMsgHandleFunc( exclMod, pstMsg );
		}
	}

	return 0;
}

HINT32	__bus_sendMsg( HOMMA_BUS_Target_e eTarget, BUS_Handle_t *pstSrcHandle, const HOMMA_BUS_Msg_t *pstMsg )
{
	BUS_ClientContext_t	*pClientCtx = __bus_getClientContext( eTarget );
	HOMMA_BUS_MsgType_e  eMsgType = eBUS_MSGTYPE_UNKNOWN;
	BUS_ClientContext_t	*pSenderCtx = NULL;
	HOMMA_BUS_Target_e eSender = eBUS_TARGET_NONE;

	if( !pstMsg )
	{
		HxLOG_Print("[%s:%d] Error> Message is NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	if ( !pstSrcHandle )
	{
		HxLOG_Print("[%s:%d] Error> Source Handle is NULL(tgt:%d, msg:%d)\n", __FUNCTION__, __LINE__, eTarget, pstMsg->eMsgType);
		return ERR_FAIL;
	}

	eMsgType = pstMsg->eMsgType;
	eSender = ((BUS_Handle_t *)pstSrcHandle)->eSrcType;

	//HxLOG_Warning("[homma_bus.c]__bus_sendMsg/eSender:%s ==> eTarget:%s/msg type:%s\n", __target(eSender), __target(eTarget), s_PrintMsgType(eMsgType));

	/* 다른 모듈의 콜백 함수에서 Task를 잡고 있으면, 문제 발생함. */
	if ( eBUS_TARGET_MOD_ALL == eTarget )
	{
		__bus_broadcastMod( eMsgType, pstSrcHandle, pstMsg );
	}
	else if ( eBUS_TARGET_ALL == eTarget )
	{
		__bus_broadcastAll( eMsgType, pstSrcHandle, pstMsg );
	}
	else
	{
		if( !pClientCtx )
		{
			HxLOG_Print("[%s:%d] Error> target Context(%s) is NULL \n", __FUNCTION__, __LINE__, __target(eTarget));
			return ERR_FAIL;
		}

		if ( pClientCtx->cbMsgHandleFunc )
		{
			pClientCtx->cbMsgHandleFunc( eSender, pstMsg );
		}
		else
			HxLOG_Print("[%s:%d] Error> target(%s) is not exised message handler \n", __FUNCTION__, __LINE__, __target(eTarget));
	}

	if ( (eBUS_MSGTYPE_EVENT == eMsgType) &&
			(eBUS_TARGET_NONE < pstSrcHandle->eSrcType) &&
		   	(eBUS_TARGET_MAX > pstSrcHandle->eSrcType) )
	{
		BUS_ClientContext_t	*pHndCtx = __bus_getClientContext( pstSrcHandle->eSrcType );
		BUS_EVT_HANDLE_Func	fnNotify = NULL;

		HOMMA_BUS_MsgEvt_t stEvt = { 0 };
		HxList_t	*pList = NULL;

		stEvt.eDataType = pstMsg->eDataType;
		stEvt.nDataSize = pstMsg->nDataSize;
		stEvt.pvData 	= pstMsg->pvData;

		if(pHndCtx != NULL)
		{
			pList = pHndCtx->pNotifyList;

			while( pList )
			{
				fnNotify = (BUS_EVT_HANDLE_Func)HLIB_LIST_Data( pList );
				if( fnNotify )
				{
					fnNotify( pstMsg->uEnum.eEvt, &stEvt, eSender );
				}

				pList = HLIB_LIST_Next( pList );
			}
		}
	}

	pSenderCtx = __bus_getClientContext( eSender );
	if ( !pSenderCtx )
	{
		HxLOG_Print("[%s:%d] Error> Sender Context is NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	switch (  eMsgType )
	{
		/* EVENT를 제외한 REQUEST나 RESPONE은 Connect Func에게 보내지 않는다 */
		case eBUS_MSGTYPE_EVENT :
		case eBUS_MSGTYPE_REQUEST :
		case eBUS_MSGTYPE_RESPONSE:
		case eBUS_MSGTYPE_COMMAND :
			if ( eTarget != eSender && pstMsg->pvData )
			{
				if( pSenderCtx->cbRelHandleFunc )
				{
					HxLOG_Print ( "[%s:%s:%d] \n", __FILE__, __FUNCTION__, __LINE__ );
					pSenderCtx->cbRelHandleFunc( eSender,  (HOMMA_BUS_Msg_t*) pstMsg );
				}
#if 0
				else
					HxLOG_Print("[%s:%d] Target(%s) cbRelHandleFunc is null \n", __FUNCTION__, __LINE__, __target(eTarget));
#endif
			}
			break;
		default :
			HxLOG_Print("[%s:%d] Error> Message Type is invalid. \n", __FUNCTION__, __LINE__);
			return ERR_OK;
	}

	return ERR_OK;
}

static void __bus_internal_msg_task (void *arg)
{
	BUS_TaskContext_t	*pstBusCtx = __bus_getTaskContext();

	BUS_MsgData_t	stMsgData;
	VK_ERR_CODE eVkErr;
	HERROR retErr = ERR_FAIL;
	HULONG msgQId = pstBusCtx->ulMsgQId;

	if ( !msgQId )
	{
		HxLOG_Critical("Critical Eror!!! : msgQId == 0\n");
		return ;
	}

	while ( TRUE )
	{
		eVkErr = VK_MSG_Receive( pstBusCtx->ulMsgQId, &stMsgData, sizeof(BUS_MsgData_t));
		if ( VK_OK != eVkErr )
		{
			VK_TASK_Sleep(10);
			continue;
		}

		retErr = __bus_sendMsg( stMsgData.eTarget, stMsgData.pSrcHandle, &stMsgData.stMsg) ;
		if ( ERR_OK != retErr )
		{
			HxLOG_Critical("Critical Eror!!!\n");
		}
	}
}

#define _____PUBLIC_FUNCTIONS____

void	HOMMA_BUS_Init( void )
{
	BUS_TaskContext_t	*pstBusCtx = __bus_getTaskContext();

	HxSTD_MemSet( pstBusCtx, 0x00, sizeof(BUS_TaskContext_t));

	HxLOG_Print("ROMA BUS Init!!!\n");

	if ( VK_OK != VK_MSG_Create( BUS_MSGQ_SIZE, sizeof(BUS_MsgData_t), "HOMMA_BUS_MSGQ", &pstBusCtx->ulMsgQId, VK_SUSPENDTYPE_FIFO ))
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return;
	}

	if ( VK_OK != VK_TASK_Create( __bus_internal_msg_task, VK_TASK_PRIORITY_MW_REF, SIZE_256K,
			"__bus_internal_msg_task", pstBusCtx, &pstBusCtx->ulMsgTaksId, 0 ))
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return;
	}

	if ( VK_OK != VK_TASK_Start( pstBusCtx->ulMsgTaksId ))
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return;
	}

}

void	HOMMA_BUS_DeInit( void )
{
	BUS_TaskContext_t	*pstBusCtx = __bus_getTaskContext();
	HxLOG_Print("ROMA Process Init!!!\n");

	if ( pstBusCtx->ulMsgQId )
	{
		VK_MSG_Destroy( pstBusCtx->ulMsgQId );
	}

	//VK_TASK_Destroy( pstBusCtx->ulMsgTaksId );
}

BUS_Handle	HOMMA_BUS_GetHandle( HOMMA_BUS_Target_e eTarget )
{
	BUS_ClientContext_t *pClientCtx = __bus_getClientContext( eTarget );

	if ( !pClientCtx )
	{
		//HxLOG_Print("[%s:%d] Client(%s) Context is not created .. ! \n", __FUNCTION__, __LINE__, __target(eTarget));
		return NULL;
	}

	if ( !pClientCtx->hHandId )
	{
		pClientCtx->hHandId = (BUS_Handle_t *)HLIB_STD_MemAlloc(sizeof(BUS_Handle_t));
		pClientCtx->hHandId->eSrcType = eTarget;
	}

	return pClientCtx->hHandId;
}

HINT32	HOMMA_BUS_Register( BUS_Handle hHandle, const HOMMA_BUS_HndFuncList_t *pstHndFuncList )
{
	BUS_ClientContext_t *pClientCtx = NULL;
	HINT32 errRet = 0;
	BUS_Handle_t *pHandle = (BUS_Handle_t *)hHandle;

	if ( !pHandle )
	{
		HxLOG_Print("[%s:%d] handle is NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	pClientCtx = __bus_getClientContext( pHandle->eSrcType );
	if ( !pClientCtx )
	{
		HxLOG_Print("[%s:%d] client context is NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	if ( !pClientCtx->hHandId )
	{
		HxLOG_Print("[%s:%d] client handle is NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	pClientCtx->cbMsgHandleFunc = pstHndFuncList->cbMsgHandleFunc;
	pClientCtx->cbReqHandleFunc = pstHndFuncList->cbReqHandleFunc;
	pClientCtx->cbRelHandleFunc = pstHndFuncList->cbRelHandleFunc; // 2014/2/28 mhakng: 이것이 없으면 메모리  release handler함수가 호출되지 않는다.

	return errRet;
}

HINT32	HOMMA_BUS_UnRegister( BUS_Handle hHandle )
{
	BUS_ClientContext_t *pClientCtx = (BUS_ClientContext_t *)hHandle;
	if ( !pClientCtx )
	{
		return ERR_FAIL;
	}
	pClientCtx->cbMsgHandleFunc = NULL;
	pClientCtx->cbReqHandleFunc = NULL;
	pClientCtx->cbRelHandleFunc = NULL;

	return ERR_OK;
}

HINT32	HOMMA_BUS_Connect( HOMMA_BUS_Target_e eTarget, BUS_EVT_HANDLE_Func cbNotify )
{
	BUS_ClientContext_t *pClientCtx = __bus_getClientContext( eTarget );
	if ( !pClientCtx )
	{
		return ERR_FAIL;
	}

	pClientCtx->pNotifyList =  HLIB_LIST_Append( pClientCtx->pNotifyList, (void *)cbNotify );
	return ERR_OK;
}

HINT32	HOMMA_BUS_Disconnect( HOMMA_BUS_Target_e eTarget, BUS_EVT_HANDLE_Func cbNotify )
{
	BUS_ClientContext_t *pClientCtx = __bus_getClientContext( eTarget );
	if ( !pClientCtx )
	{
		return ERR_FAIL;
	}
	pClientCtx->pNotifyList =  HLIB_LIST_Remove( pClientCtx->pNotifyList, (void *)cbNotify );

	return ERR_OK;
}

/* 보내고 끝 */
/* sync로 동작 */

HINT32	HOMMA_BUS_SendMsg( HOMMA_BUS_Target_e eTarget, BUS_Handle hSrcHandle, const HOMMA_BUS_Msg_t *pstMsg )
{
	BUS_MsgData_t	stMsgData = { 0 };
	BUS_TaskContext_t	*pTaskCtx  = __bus_getTaskContext();
	HERROR	retErr = ERR_FAIL;

	if( !pstMsg )
	{
		HxLOG_Print("[%s:%d] Message is NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	stMsgData.eTarget = eTarget;
	stMsgData.pSrcHandle = (BUS_Handle_t *)hSrcHandle;
	HxSTD_MemCopy( &stMsgData.stMsg, pstMsg, sizeof(HOMMA_BUS_Msg_t) );

	retErr =  VK_MSG_SendTimeout( pTaskCtx->ulMsgQId, (void *)&stMsgData, sizeof(BUS_MsgData_t), 0 );
	if ( VK_OK != retErr )
	{
		HxLOG_Critical("Critical Eror!!!\n");
	}

	return retErr;
}

/* request를 하면 respone이 와야 함. */
/* sync로 동작 */
HINT32	HOMMA_BUS_Request( HOMMA_BUS_Target_e eTarget, HOMMA_BUS_Response_e eReq, HOMMA_BUS_Data_t *pstRetData, HINT32 *pnError )
{
	BUS_ClientContext_t *pClientCtx = __bus_getClientContext( eTarget );
	HINT32	nRet = 0;

	if ( pClientCtx && pClientCtx->cbReqHandleFunc )
	{
		nRet = pClientCtx->cbReqHandleFunc( eReq, pstRetData );
	}
	if ( pnError )
	{
		*pnError = nRet;
	}

	return ERR_OK;
}

#define _____END_OF_FILE_________

