/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hxrpc.c
	@brief

	Description:  									\n
	Module: RPC module for OCTO 20					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

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
 * (c) 2011-2013 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <_hlib_serializer.h>
#include <_hlib_hash.h>
#include <_hlib_cbuf.h>
#include <_hlib_queue.h>
#include <_hlib_stdlib.h>
#include <_hlib_log.h>
#include <_hlib_socket.h>
#include <_hlib_rwstream.h>
#include <_hlib_sem.h>
#include <_hlib_dbg.h>
#include <_hlib_rpc.h>
#include <_hlib_tree.h>
#include <_hlib_ansicode.h>
#include <_hlib_vector.h>
#include <_hlib_msg.h>
#include <vkernel.h>


#if	!defined(CONFIG_TARGET_ROOTFS_SQUASH)
#define	CONFIG_RPC_LOG
#endif

#define CONFIG_RPC_CHECK_DATA
#define CONFIG_RPC_CHECK_CRC

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	HxRPC_INC_REQID(prgid, reqid)			((prgid << 16) | (reqid & 0xFFFF))
#define	HxRPC_CHECK_REQID(nPid, reqid)			((((reqid & 0xFFFF0000)>>16) == nPid) ? TRUE : FALSE)
#define	HxRPC_SIGNATURE							"ilsb"

#define	ENTER_RPC_SEM(handle)		HxSEMT_Get(((RPC_t*)handle)->ulSemId)
#define	LEAVE_RPC_SEM(handle)		HxSEMT_Release(((RPC_t*)handle)->ulSemId)

#define	RPC_Malloc					HLIB_MEM_Malloc
#define	RPC_Calloc					HLIB_MEM_Calloc
#define	RPC_Realloc					HLIB_MEM_Realloc
#define	RPC_StrDup					HLIB_MEM_StrDup
#define	RPC_MemDup					HLIB_MEM_MemDup
#define	RPC_Free					HLIB_MEM_Free
#define	RPC_Malloc_CB				HLIB_MEM_Malloc_CB
#define	RPC_Free_CB					HLIB_MEM_Free_CB

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HBOOL		bIgnoreErrorCheck;
	HUINT32		ulSemId;

	HUINT16		usProgId;
	HINT32		nWritePort;
	HUINT32		ulRequestId;

	HINT32		nDocStream;
	HINT32		nSrzr;
	HINT32		nSrzrParam;

	struct
	{
		void		*pstObj;
		HxQueue_t	*pstJob;
	} stPool;

	HxTREE_t		*pWaitForResponse;
	HxQueue_t		*pReqFreed;

	HxCBUF_t			pstCBuf;
	HxHASH_t	*pstMethodTable;
	HxHASH_t	*pstCallbackTable;
	HxHASH_t	*pstProgIdTable;

	HxObject_t	*pstLastUnpackedObject;
	HxObject_t	*pstLastUnpackedParam;

	HxRPC_OnError_t	pfnOnError;
} RPC_t;

typedef struct
{
	HINT32			nClientHandle;
	HxRPC_Flags_e	eFlags;
} RPC_Client_t;

typedef struct
{
	HBOOL			bCallback;
	HBOOL			bReplaced;
	HCHAR			*pszProtoType;
	HxRPC_Func_t	pfnFunc;
	void			*pvUserData;
	HCHAR			*pszDescription;
	HxList_t		*pstClientHandles;
	HxList_t		*pstClients;	//RPC_Client_t

	HCHAR			*pszRepliProtoType;
} RPC_Method_t;

typedef struct
{
	HCHAR			*pszCallback;
	HxRPC_Noti_t	pfnNotifier;
	void			*pvUserData;
} RPC_Callback_t;

typedef struct
{
	HxRPC_Response_t	pfnResponse;
	void			*pvUserdata;

	HBOOL			bRequestDone;
	HERROR			hErrRequest;
	HxObject_t		requestData;
	HULONG			msgQ;
} RPC_Request_t;

typedef struct
{
	HxRPC_Func_t		  pfFunc;
	HINT32				  nClientHandle;
	HINT32				  nArgc;
	HxObject_t			**ppstArgv;
	HUINT32				  ulRequestId;
	void				 *pvUserData;
} RPC_ExecReqArg_t;

typedef struct
{
	HxRPC_Noti_t		  pfFunc;
	HINT32				  nArgc;
	HxObject_t			**ppstArgv;
	HUINT32				  ulRequestId;
	void				 *pvUserData;
} RPC_ExecNotiArg_t;

typedef struct
{
	HxRPC_Response_t	  pfFunc;
	HERROR				  hErr;
	HINT32				  nArgc;
	HxObject_t			**ppstArgv;
	HUINT32				  ulRequestId;
	void				 *pvUserData;
} RPC_ExecRespArg_t;

typedef struct
{
	HxRPC_CMD_e		 eCmd;
	union
	{
		RPC_ExecReqArg_t	 stRequest;
		RPC_ExecNotiArg_t	 stNotify;
		RPC_ExecRespArg_t	 stResponse;
	} unArg;

	HxObject_t		*pstUnpackedParam;
} RPC_ExecArg_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
#ifdef	CONFIG_RPC_LOG
static HINT32	s_nLogHandle = 0;
#endif
/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

/*
	verify signature
*/
static HBOOL	hxrpc_VerifySignature(const HCHAR *pszSignature)
{
	while (*pszSignature != '\0')
	{
		if (strchr(HxRPC_SIGNATURE, *pszSignature++) == NULL)
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*
	make prototype of method.
	for searching method in hash table.
*/
static HCHAR*	hxrpc_MakeProtoType(RPC_t *pstRpc, const HCHAR *pszMethodName, const HCHAR *pszSignature)
{
	HCHAR	*pszResult;

	pszResult = HLIB_CBUF_GetBuffer(pstRpc->pstCBuf, 1024);
	HxSTD_PrintToStrN(pszResult, 1024, "%s(%s)", pszMethodName, HxSTD_StrEmpty(pszSignature) ? "" : pszSignature);

	return pszResult;
}

/*
	make signature with object
*/
static const HCHAR*	hxrpc_MakeSignature(RPC_t *pstRpc, const HxObject_t *pstObject)
{
	HCHAR		*pszBuf;
	HUINT32		i, ulSize;

	if (pstObject == NULL || pstObject->type != HxOBJECT_TYPE_ARR)
		return NULL;


	ulSize = HxOBJECT_ARR_LEN(pstObject);
	pszBuf = HLIB_CBUF_GetBuffer(pstRpc->pstCBuf, ulSize + 1);

	for (i = 0 ; i < ulSize ; i++)
	{
		pszBuf[i] = (HCHAR)HxOBJECT_ARR_VAL(pstObject, i)->type;
	}
	pszBuf[i] = '\0';

	return pszBuf;
}

/*
	free method data type
*/
static void		hxrpc_FreeMethod (RPC_Method_t *pstMethod)
{
	if (pstMethod)
	{
		if (pstMethod->pszProtoType)
			RPC_Free(pstMethod->pszProtoType);
		if (pstMethod->pszDescription)
			RPC_Free(pstMethod->pszDescription);
		RPC_Free(pstMethod);
	}
}

static void		hxrpc_FreeCallback(RPC_Callback_t *pstCallback)
{
	if (pstCallback)
	{
		if (pstCallback->pszCallback)
			RPC_Free(pstCallback->pszCallback);
		RPC_Free(pstCallback);
	}
}

//
//	Make RPC data for method or callback
static HERROR	hxrpc_MakeSerializerData(RPC_t *pstRpc, HINT32 nProgId, HINT32 nSrzr, HINT32 nSrzrParam, HxRPC_CMD_e eCmd, const HCHAR *pszName, HERROR hErr, const HCHAR *pszSignature, va_list ap)
{
	const HCHAR	*pszText;
	HINT64		nNumber;
	HUINT32		ulSize;
	void 		*pvData;

	HLIB_SERIALIZER_Clear(nSrzr);
	HxSERIALIZER_Array(nSrzr);
	HxSERIALIZER_Integer(nSrzr, eCmd);
		HxSERIALIZER_Integer(nSrzr, nProgId);
		HxSERIALIZER_Integer(nSrzr, (HINT32)hErr);
		if(pszName != NULL)
		{
			HxSERIALIZER_String (nSrzr, pszName);
		}
		if (pszSignature)
		{
			HLIB_SERIALIZER_Clear(nSrzrParam);
			HxSERIALIZER_Array(nSrzrParam);

			while (*pszSignature != '\0')
			{
				switch (*pszSignature++)
				{
					case 's':
						pszText = va_arg(ap, const HCHAR *);
						if (pszText == NULL)
							HxSERIALIZER_String(nSrzrParam, "");
						else
							HxSERIALIZER_String(nSrzrParam, pszText);
						break;
					case 'i':
						nNumber = va_arg(ap, HINT32);
						HxSERIALIZER_Integer(nSrzrParam, nNumber);
						break;
					case 'l':
						nNumber = va_arg(ap, HINT64);
						HxSERIALIZER_LongInteger(nSrzrParam, nNumber);
						break;
					case 'b':
						pvData = va_arg(ap, void *);
						ulSize = va_arg(ap, HUINT32);
						HxSERIALIZER_Binary(nSrzrParam, pvData, ulSize);
						break;
					default:
						break;
				}
			}

			HxSERIALIZER_ArrayEnd(nSrzrParam);
			pvData = HLIB_SERIALIZER_Pack(nSrzrParam, &ulSize);
			if (pvData && ulSize)
				HxSERIALIZER_Binary(nSrzr, pvData, ulSize);
		}
	HxSERIALIZER_ArrayEnd(nSrzr);

	return ERR_OK;
}

/*
	do callback with eHLIB_RPC_CMD_REQUEST type
*/
static HERROR	hxrpc_GetArgsRequest (RPC_t *pstRpc, HxObject_t *pstObject, HUINT32 ulRequestId, HINT32 nClientHandle, RPC_ExecArg_t *pstExecArg)
{
	const HCHAR		*pszMethod;
	const HCHAR		*pszSignature;
	const HCHAR		*pszProtoType;
	RPC_Method_t	*pstMethod;
	HxObject_t		*pstParam;
	HINT32	nArgc;
	HxObject_t	**ppArgv;

	//	get method name in object
	pszMethod = HxOBJECT_ARR_VAL(pstObject, 3)->u.str;
	if (pszMethod == NULL)
	{
		HxLOG_Error("Method is not valid !\n");
		return ERR_FAIL;
	}

	HxLOG_Debug("pszMethod : [%s]\n", pszMethod);

	//	get parameters in object
	pstParam = NULL;
	if (HxOBJECT_ARR_LEN(pstObject) == 5)
	{
		pstParam = HxOBJECT_ARR_VAL(pstObject, 4);
		pstParam = HLIB_SERIALIZER_UnpackWithCache(pstRpc->stPool.pstObj, HxOBJECT_BIN_DATA(pstParam), HxOBJECT_BIN_SIZE(pstParam), FALSE);
		if (pstParam == NULL)
		{
			HxLOG_Error("Invalid Parameters\n");
			return ERR_FAIL;
		}
	}

	//	make prototype for find function in table.
	pszSignature = hxrpc_MakeSignature(pstRpc, pstParam);
	pszProtoType = hxrpc_MakeProtoType(pstRpc, pszMethod, pszSignature);

	pstMethod = (RPC_Method_t*)HLIB_HASH_Lookup(pstRpc->pstMethodTable, pszProtoType);
	if (pstMethod == NULL)
	{
		HxLOG_Error("[%s:%d method [%s] [%s] is not registered!\n", __FUNCTION__, __LINE__, pszMethod, pszProtoType);
		return ERR_FAIL;
	}
	HxLOG_Debug("pstMethod [%p] - ProtoType [%s]\n", pstMethod, pstMethod->pszProtoType);

	//	make parameter to object type.
	if (pstParam)
	{
		nArgc = HxOBJECT_ARR_LEN(pstParam);
		ppArgv = pstParam->u.array.values;
	} else
	{
		nArgc = 0;
		ppArgv = NULL;
	}

	//	call registered function.
	HxLOG_Debug("Call Function [%s]\n", pszProtoType);

	pstExecArg->eCmd = eHLIB_RPC_CMD_REQUEST;
	pstExecArg->unArg.stRequest.pfFunc			= pstMethod->pfnFunc;
	pstExecArg->unArg.stRequest.nClientHandle	= nClientHandle;
	pstExecArg->unArg.stRequest.nArgc			= nArgc;
	pstExecArg->unArg.stRequest.ppstArgv		= ppArgv;
	pstExecArg->unArg.stRequest.ulRequestId		= ulRequestId;
	pstExecArg->unArg.stRequest.pvUserData		= pstMethod->pvUserData;
	pstExecArg->pstUnpackedParam = pstParam;

	return ERR_OK;
}

static HERROR hxrpc_GetArgsNotifier (RPC_t *pstRpc, HxObject_t *pstObject, HUINT32 ulRequestId, RPC_ExecArg_t *pstExecArg)
{
	const HCHAR		*pszCallback;
	RPC_Callback_t	*pstCallback;
	HxObject_t		*pstParam;
	HINT32	nArgc;
	HxObject_t	**ppArgv;

	//	get callback name in object
	pszCallback = HxOBJECT_ARR_VAL(pstObject, 3)->u.str;
	if (pszCallback == NULL)
	{
		HxLOG_Error("Method is not valid !\n");
		return ERR_FAIL;
	}

	//	get parameters in object
	pstParam = NULL;
	if (HxOBJECT_ARR_LEN(pstObject) == 5)
	{
		pstParam = HxOBJECT_ARR_VAL(pstObject, 4);
		pstParam = HLIB_SERIALIZER_UnpackWithCache(pstRpc->stPool.pstObj, HxOBJECT_BIN_DATA(pstParam), HxOBJECT_BIN_SIZE(pstParam), FALSE);
		if (pstParam == NULL)
		{
			HxLOG_Error("Invalid Parameters\n");
			return ERR_FAIL;
		}
	}

	//	make prototype for find function in table.
	pstCallback = (RPC_Callback_t*)HLIB_HASH_Lookup(pstRpc->pstCallbackTable, (const void*)pszCallback);
	if (pstCallback == NULL)
	{
		HxLOG_Error("[%s:%d method [%s] is not registered!\n", __FUNCTION__, __LINE__, pszCallback);
		return ERR_FAIL;
	}
	HxLOG_Debug("pstMethod [%p] - ProtoType [%s]\n", pstCallback, pstCallback->pszCallback);

	//	make parameter to object type.
	if (pstParam)
	{
		nArgc = HxOBJECT_ARR_LEN(pstParam);
		ppArgv = pstParam->u.array.values;
	} else
	{
		nArgc = 0;
		ppArgv = NULL;
	}

	//	call registered function.
	pstExecArg->eCmd = eHLIB_RPC_CMD_CALLBACK;
	pstExecArg->unArg.stNotify.pfFunc			= pstCallback->pfnNotifier;
	pstExecArg->unArg.stNotify.nArgc			= nArgc;
	pstExecArg->unArg.stNotify.ppstArgv			= ppArgv;
	pstExecArg->unArg.stNotify.ulRequestId		= ulRequestId;
	pstExecArg->unArg.stNotify.pvUserData		= pstCallback->pvUserData;
	pstExecArg->pstUnpackedParam = pstParam;

	return ERR_OK;
}

static RPC_Request_t *	hxrpc_PopRequest (RPC_t *pRPC, HUINT32 ulRequestId)
{
	RPC_Request_t	*pReq;

	pReq = (RPC_Request_t *)HLIB_TREE_Lookup(pRPC->pWaitForResponse, (void *)ulRequestId);
	if (pReq)
	{
		HLIB_TREE_Steal(pRPC->pWaitForResponse, (void *)ulRequestId);
	}
	return pReq;
}

static RPC_Request_t *	hxrpc_GetRequest (RPC_t *pRPC, HUINT32 ulRequestId)
{
	RPC_Request_t	*pReq;

	pReq = (RPC_Request_t *)HLIB_TREE_Lookup(pRPC->pWaitForResponse, (void *)ulRequestId);

	return pReq;
}

static HERROR hxrpc_GetArgsResponse (RPC_t *pstRpc, HxObject_t *pstObject, HUINT32 ulRequestId, HERROR hErr, RPC_ExecArg_t *pstExecArg, HBOOL bSync)
{
	HxObject_t		*pstParam;
	HINT32	nArgc;
	HxObject_t	**ppArgv;
	RPC_Request_t	*pReq;

	HxLOG_Trace();

	HxLOG_Debug("pop request %p %d \n", pstRpc, ulRequestId);
	pReq = hxrpc_GetRequest(pstRpc, ulRequestId);
	if (pReq == NULL)
	{
		HxLOG_Error("Cannot find request(%d)\n", ulRequestId);
		return ERR_FAIL;
	}

	//	get parameters in object
	pstParam = NULL;
	if (HxOBJECT_ARR_LEN(pstObject) == 5)
	{
		pstParam = HxOBJECT_ARR_VAL(pstObject, 4);
		pstParam = HLIB_SERIALIZER_UnpackWithCache(pstRpc->stPool.pstObj, HxOBJECT_BIN_DATA(pstParam), HxOBJECT_BIN_SIZE(pstParam), FALSE);
		if (pstParam == NULL)
		{
			HxLOG_Error("Invalid Parameters\n");
			return ERR_FAIL;
		}
	}

	//	make parameter to object type.
	if (pstParam)
	{
		nArgc = HxOBJECT_ARR_LEN(pstParam);
		ppArgv = pstParam->u.array.values;
	} else
	{
		nArgc = 0;
		ppArgv = NULL;
	}

	//	call registered function.
	pstExecArg->eCmd = (bSync == TRUE) ? eHLIB_RPC_CMD_RESPONSE_SYNC : eHLIB_RPC_CMD_RESPONSE;
	pstExecArg->unArg.stResponse.pfFunc			= pReq->pfnResponse;
	pstExecArg->unArg.stResponse.hErr			= hErr;
	pstExecArg->unArg.stResponse.nArgc			= nArgc;
	pstExecArg->unArg.stResponse.ppstArgv		= ppArgv;
	pstExecArg->unArg.stResponse.ulRequestId	= ulRequestId;
	pstExecArg->unArg.stResponse.pvUserData		= pReq->pvUserdata;
	pstExecArg->pstUnpackedParam = pstParam;

	return ERR_OK;
}

static HBOOL	hxrpc_compareHandle(void *userData, void *data)
{
	RPC_Client_t *userItem = (RPC_Client_t *)userData;
	RPC_Client_t *listItem = (RPC_Client_t *)data;

	if (userItem->nClientHandle == listItem->nClientHandle)
		return TRUE;

	return FALSE;
}


//
//	rpcsys:RegisterCallback (HCHAR *callbackName, HCHAR *signature)
static HERROR	hxrpc_cbRegisterCallback(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	RPC_t	*pstRpc = (RPC_t*)pvUserData;

	const HCHAR	*pszCallbackName;
	const HCHAR	*pszSignature;

	RPC_Method_t	*pstMethod;
	HCHAR			*pszProtoType;
	RPC_Client_t	*pstClient = NULL;

	if (nArgc != 2)
	{
		HxLOG_Error("rpcsys:RegisterCallback : is not enough parameter! (callback_name, callback_signature)\n");
		return ERR_FAIL;
	}

	pszCallbackName = HxOBJECT_STR(apArgv[0]);
	pszSignature    = HxOBJECT_STR(apArgv[1]);

	ENTER_RPC_SEM(pstRpc);

	if (pszSignature && !hxrpc_VerifySignature(pszSignature))
	{
		HxLOG_Error("invalid signature : %s\n", pszSignature);

		goto err_rtn;
	}

	if (pstRpc->pstMethodTable == NULL)
	{
		HxLOG_Error("Callback is not registered! [%s]\n", pszCallbackName);

		goto err_rtn;
	}

	pstClient =  (RPC_Client_t*)RPC_Calloc(sizeof(RPC_Client_t));
	if (pstClient == NULL)
	{
		HxLOG_Error("memory alloc failed. [%s]\n", pszCallbackName);

		goto err_rtn;
	}

	pszProtoType = hxrpc_MakeProtoType(pstRpc, pszCallbackName, pszSignature);
	pstMethod = HLIB_HASH_Lookup(pstRpc->pstMethodTable, pszProtoType);
	if (pstMethod == NULL)
	{
		if (pstRpc->bIgnoreErrorCheck)
		{
			HxLOG_Debug("Callback is not registered! [%s]\n", pszCallbackName);

			pstMethod = (RPC_Method_t*)RPC_Calloc(sizeof(RPC_Method_t));
			pstMethod->pszProtoType   = RPC_StrDup(pszProtoType);
			pstMethod->bCallback      = TRUE;
		#ifdef	CONFIG_DEBUG
			pstMethod->pszDescription = NULL;
		#endif
			HLIB_HASH_Insert(pstRpc->pstMethodTable, pstMethod->pszProtoType, pstMethod);
		} else
		{
			HxLOG_Error("Callback is not registered! [%s]\n", pszCallbackName);
			goto err_rtn;
		}
	}

	pstClient->nClientHandle = nClientHandle;
	pstClient->eFlags = eHLIB_RPC_FLAG_NONE;
	if (HLIB_LIST_FindEx(pstMethod->pstClients, (const void*)pstClient,hxrpc_compareHandle))
	{
		HxLOG_Error("Callback is already registerer! [%s]\n", pszCallbackName);

		goto err_rtn;
	}

	HxLOG_Debug("callback is registered [%s] - [%p]\n", pszProtoType, pstClient->nClientHandle);
	pstMethod->pstClients = HLIB_LIST_Append(pstMethod->pstClients, (void*)pstClient);

	LEAVE_RPC_SEM(pstRpc);

	return ERR_OK;

err_rtn:
	LEAVE_RPC_SEM(pstRpc);

	if (pstClient != NULL)
	{
		RPC_Free(pstClient);
	}
	return ERR_FAIL;
}

static HERROR	hxrpc_RemoveClientHandle(HINT32 nRpcHandle, HINT32 nClientHandle)
{
	RPC_t	*pstRpc = (RPC_t*)nRpcHandle;
	RPC_Method_t	*pstMethod;
	RPC_Client_t	stClient;
	HxHASH_TableIter_t	iter;

	stClient.nClientHandle = nClientHandle;
	HLIB_HASH_IterInit(&iter, pstRpc->pstMethodTable);
	while (HLIB_HASH_IterNext(&iter, NULL, (void**)&pstMethod))
		pstMethod->pstClients = HLIB_LIST_RemoveIf(pstMethod->pstClients, (void*)&stClient, hxrpc_compareHandle, RPC_Free_CB);

	return ERR_OK;
}

static HERROR	hxrpc_GetProgIdFromClientHandle(HINT32 nRpcHandle, HINT32 nClientHandle, HINT32 *pnProgId)
{
	RPC_t	*pstRpc = (RPC_t*)nRpcHandle;
	HINT32	nProgId;

	nProgId = (HINT32)HLIB_HASH_Lookup(pstRpc->pstProgIdTable, (void*)nClientHandle);
	if (pnProgId)
		*pnProgId = nProgId;

	if (nProgId)
		return ERR_OK;

	return ERR_FAIL;
}

static HERROR	hxrpc_SetOnError (HINT32 nRpcHandle, HxRPC_OnError_t pfnOnError)
{
	RPC_t	*pstRpc = (RPC_t*)nRpcHandle;

	pstRpc->pfnOnError = pfnOnError;

	return ERR_OK;
}

static HERROR	hxrpc_SetClientFlag(HINT32 nClient, HxRPC_Flags_e eFlag)
{
	RPC_Client_t	*pstClient = (RPC_Client_t*)nClient;

	pstClient->eFlags |= eFlag;

	return ERR_OK;
}

static HINT32	hxrpc_Open(HINT32 nRoute, HINT32 nProgId, HBOOL bIgnoreError)
{
	RPC_t	*pstRpc;

	pstRpc = RPC_Calloc(sizeof(RPC_t));
	if (pstRpc == NULL)
		return 0;

	HxSEMT_Create(&pstRpc->ulSemId, "rpcsem", 0);

	pstRpc->bIgnoreErrorCheck = bIgnoreError;

	pstRpc->usProgId   = nProgId;
	pstRpc->nWritePort = nRoute;
	pstRpc->pstCBuf    = HLIB_CBUF_New(8192);

	pstRpc->nDocStream = HLIB_RWSTREAM_OpenEx(NULL, 1024, RPC_Malloc_CB, RPC_Free_CB);
	pstRpc->nSrzr = HLIB_SERIALIZER_OpenEx(RPC_Malloc_CB, RPC_Free_CB);
	pstRpc->nSrzrParam = HLIB_SERIALIZER_OpenEx(RPC_Malloc_CB, RPC_Free_CB);

	pstRpc->stPool.pstObj = HLIB_SERIALIZER_OpenUnpackCache();
	pstRpc->stPool.pstJob = HLIB_QUEUE_New((HxQueue_RemoveCallback_t)RPC_Free_CB);
	pstRpc->pWaitForResponse = HLIB_TREE_NewFull(NULL, NULL, NULL, (HxDestroyNotify)RPC_Free_CB);
	HLIB_TREE_NodeCache(pstRpc->pWaitForResponse, TRUE);
	pstRpc->pReqFreed = HLIB_QUEUE_New((HxQueue_RemoveCallback_t)RPC_Free_CB);

	return (HINT32)pstRpc;
}

static RPC_Request_t *hxrpc_AllocRequest (RPC_t *pRPC)
{
	RPC_Request_t *pReq;

	pReq = (RPC_Request_t *)HLIB_QUEUE_Pop(pRPC->pReqFreed);
	if (pReq == NULL)
	{
		return (RPC_Request_t *)RPC_Calloc(sizeof(RPC_Request_t));
	}
	memset(pReq, 0, sizeof(RPC_Request_t));
	return pReq;
}

#ifdef	CONFIG_RPC_LOG
static void	hxrpc_dbg_writeLog(RPC_t *rpcInfo, void *pvData, HINT32 nSize, HINT32 nRequestId)
{
	HUINT32			ulSystemTick;

	if (s_nLogHandle == 0)
	{
		HCHAR	*pszBuf = HLIB_CBUF_GetBuffer(rpcInfo->pstCBuf, 1024);

		HxSTD_snprintf(pszBuf, 1024, "/rpclog.%s", HLIB_STD_GetCurProcessName());
		s_nLogHandle = open(pszBuf, O_WRONLY | O_CREAT, 0666);
		HxSTD_snprintf(pszBuf, 1024, "%s", RPC_LOG_TEXT);
		if (s_nLogHandle > -1)
		{
			write(s_nLogHandle, pszBuf, HxSTD_StrLen(pszBuf) + 1);
		}
	}

	if(s_nLogHandle > -1)
	{
		ulSystemTick = HLIB_STD_GetSystemTick();
		write(s_nLogHandle, &ulSystemTick, 4);
		write(s_nLogHandle, &nSize, 4);
		write(s_nLogHandle, &nRequestId, 4);
		write(s_nLogHandle, pvData, nSize);
	}
}
#endif

static HINT32	hxrpc_Method(HINT32 nRpcHandle, HxRPC_Response_t pfnCallback, void *pvUserdata, const HCHAR *pszMethodName, const HCHAR *pszSignature, va_list ap, HBOOL bSync)
{
	HINT32		nReqId;
	RPC_t		*pstRpc = (RPC_t*)nRpcHandle;
	HUINT32		ulSize;
	void 		*pvData;
	const HCHAR *szText;


	HxLOG_Debug("Method : [%s] Signature [%s]\n", pszMethodName, pszSignature);

	if (pszSignature && !hxrpc_VerifySignature(pszSignature))
	{
		HxLOG_Error("Invalid Signature (%s)\n", pszSignature);

		return -1;
	}

	if (bSync)
		hxrpc_MakeSerializerData(pstRpc, pstRpc->usProgId, pstRpc->nSrzr, pstRpc->nSrzrParam, eHLIB_RPC_CMD_REQUEST_SYNC, pszMethodName, ERR_OK, pszSignature, ap);
	else
		hxrpc_MakeSerializerData(pstRpc, pstRpc->usProgId, pstRpc->nSrzr, pstRpc->nSrzrParam, eHLIB_RPC_CMD_REQUEST, pszMethodName, ERR_OK, pszSignature, ap);

	if ((szText = HLIB_SERIALIZER_Error(pstRpc->nSrzr)) != NULL)
	{
		HxLOG_Error("Serialize failed : %s\n", szText);

		return -1;
	}

	pvData = HLIB_SERIALIZER_Pack(pstRpc->nSrzr, &ulSize);

	pstRpc->ulRequestId = (pstRpc->ulRequestId + 1) & 0xFFFF;
	nReqId = HxRPC_INC_REQID(pstRpc->usProgId, pstRpc->ulRequestId);

#ifdef	CONFIG_RPC_LOG
	hxrpc_dbg_writeLog(pstRpc, pvData, ulSize, nReqId);
#endif

	HLIB_SOCKET_Write(pstRpc->nWritePort, pvData, ulSize, nReqId);

	if (pfnCallback)
	{
		RPC_Request_t	*pRequest = hxrpc_AllocRequest(pstRpc);

		if (pRequest)
		{
			pRequest->pfnResponse = pfnCallback;
			pRequest->pvUserdata  = pvUserdata;
			if (bSync == TRUE)
			{
				if (HLIB_MSG_Create(2L, sizeof(HINT32), "RpcSyncMsg", &pRequest->msgQ, 0))
				{
					pRequest->msgQ = 0L;
				}
			}
			HxLOG_Debug("insert request %p %d \n", pstRpc, nReqId);
			HLIB_TREE_Insert(pstRpc->pWaitForResponse, (void *)nReqId, pRequest);
		}
	}

	return nReqId;
}

void*	hxrpc_MakeSerializeArgs(HINT32 nRpcHandle, const HCHAR *pszSignature, va_list ap, HINT32 *pnSize)
{
	RPC_t	*pstRpc = (RPC_t*)nRpcHandle;
	const HCHAR	*pszText;
	HINT64		nNumber;
	HUINT32		ulSize;
	void 		*pvData;

	if (pszSignature == NULL)
	{
		*pnSize = 0;
		return NULL;
	}

	HLIB_SERIALIZER_Clear(pstRpc->nSrzrParam);
	HxSERIALIZER_Array(pstRpc->nSrzrParam);

	while (*pszSignature != '\0')
	{
		switch (*pszSignature++)
		{
			case 's':
				pszText = va_arg(ap, const HCHAR *);
				if (pszText == NULL)
					HxSERIALIZER_String(pstRpc->nSrzrParam, "");
				else
					HxSERIALIZER_String(pstRpc->nSrzrParam, pszText);
				break;
			case 'i':
				nNumber = va_arg(ap, HINT32);
				HxSERIALIZER_Integer(pstRpc->nSrzrParam, nNumber);
				break;
			case 'l':
				nNumber = va_arg(ap, HINT64);
				HxSERIALIZER_Integer(pstRpc->nSrzrParam, nNumber);
				break;
			case 'b':
				pvData = va_arg(ap, void *);
				ulSize = va_arg(ap, HUINT32);
				HxSERIALIZER_Binary(pstRpc->nSrzrParam, pvData, ulSize);
				break;
			default:
				break;
		}
	}

	HxSERIALIZER_ArrayEnd(pstRpc->nSrzrParam);
	pvData = HLIB_SERIALIZER_Pack(pstRpc->nSrzrParam, &ulSize);

	*pnSize = ulSize;

	return pvData;
}

static HERROR	hxrpc_RegisterMethod(HINT32 nRpcHandle, const HCHAR *pszMethodName, const HCHAR *pszSignature, HxRPC_Func_t pfnFunction, void *pvUserData, const HCHAR *pszDescription)
{
	RPC_t	*pstRpc = (RPC_t*)nRpcHandle;
	RPC_Method_t	*pstMethod;
	HCHAR	*pszProtoType;

	if (pszSignature && !hxrpc_VerifySignature(pszSignature))
	{
		HxLOG_Error("invalid signature : %s\n", pszSignature);

		return ERR_FAIL;
	}

	if (pstRpc->pstMethodTable == NULL)
		pstRpc->pstMethodTable = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, NULL, (_HxDestroyNotify)hxrpc_FreeMethod);

	pszProtoType = hxrpc_MakeProtoType(pstRpc, pszMethodName, pszSignature);

	pstMethod = (RPC_Method_t*)RPC_Calloc(sizeof(RPC_Method_t));
	pstMethod->pszProtoType	= RPC_StrDup(pszProtoType);
	pstMethod->pvUserData	= pvUserData;
	pstMethod->pfnFunc		= pfnFunction;
#ifdef	CONFIG_DEBUG
	pstMethod->pszDescription = RPC_StrDup(pszDescription);
#endif

	if (HLIB_HASH_Lookup(pstRpc->pstMethodTable, pstMethod->pszProtoType))
	{
		HxLOG_Error("Method is already registered! [%s]\n", pszMethodName);
		hxrpc_FreeMethod(pstMethod);

		return ERR_FAIL;
	}

	HLIB_HASH_Insert(pstRpc->pstMethodTable, pstMethod->pszProtoType, pstMethod);

	return ERR_OK;
}

static HERROR	hxrpc_Callback(HINT32 nRpcHandle, HINT32 nClientHandle, HINT32 nRequestId, const HCHAR *pszCallbackName, const HCHAR *pszSignature, HBOOL bPrototypeWitoutSignature, va_list ap)
{
	RPC_t		*pstRpc = (RPC_t*)nRpcHandle;
	HUINT32		ulSize;
	HCHAR		*pszProtoType;
	RPC_Method_t	*pstCallback;
	const HCHAR	*pszText;
	void 		*pvData;
	HxList_t	*pstClientList;
	RPC_Client_t	*pstClient = NULL;
	HERROR			hErr;
#ifdef CONFIG_DEBUG
	HBOOL		bWritten = FALSE;
#endif

	if (pszSignature && !hxrpc_VerifySignature(pszSignature))
	{
		HxLOG_Error("Invalid Signature (%s)\n", pszSignature);

		return ERR_FAIL;
	}

	if(bPrototypeWitoutSignature == TRUE)
	{
		pszProtoType = hxrpc_MakeProtoType(pstRpc, pszCallbackName, NULL);
	}
	else
	{
		pszProtoType = hxrpc_MakeProtoType(pstRpc, pszCallbackName, pszSignature);
	}
	pstCallback = (RPC_Method_t*)HLIB_HASH_Lookup(pstRpc->pstMethodTable, (void*)pszProtoType);
	if (pstCallback == NULL)
	{
		if (pstRpc->bIgnoreErrorCheck)
		{
			pstCallback = (RPC_Method_t*)RPC_Calloc(sizeof(RPC_Method_t));
			pstCallback->pszProtoType = RPC_StrDup(pszProtoType);
			pstCallback->bCallback    = TRUE;
#ifdef	CONFIG_DEBUG
			pstCallback->pszDescription = NULL;
#endif
			HLIB_HASH_Insert(pstRpc->pstMethodTable, pstCallback->pszProtoType, pstCallback);
		} else
		{
			HxLOG_Error("Unregistered callback (%s) - [%s]\n", pszCallbackName, pszProtoType);

			return ERR_FAIL;
		}
	}

	hxrpc_MakeSerializerData(pstRpc, pstRpc->usProgId, pstRpc->nSrzr, pstRpc->nSrzrParam, eHLIB_RPC_CMD_CALLBACK, pszCallbackName, ERR_OK, pszSignature, ap);

	if ((pszText = HLIB_SERIALIZER_Error(pstRpc->nSrzr)) != NULL)
	{
		HxLOG_Error("Serialize failed : %s\n", pszText);

		return ERR_FAIL;
	}

	pvData = HLIB_SERIALIZER_Pack(pstRpc->nSrzr, &ulSize);

#ifdef	CONFIG_RPC_LOG
	hxrpc_dbg_writeLog(pstRpc, pvData, ulSize, nRequestId);
#endif

	HxLOG_Debug("Write Callback --- [%p]\n", pstCallback->pstClients);
	pstClientList = pstCallback->pstClients;

	for ( ; pstClientList != NULL; pstClientList = HLIB_LIST_Next(pstClientList))
	{
		pstClient = (RPC_Client_t *)HLIB_LIST_Data(pstClientList);
		if (pstClient == NULL)
		{
			continue;
		}

		if (pstClient->eFlags & eHLIB_RPC_FLAG_BLOCKED)
		{
			HxLOG_Error("Client blocked (0x%x) callback[%s] \n",(HINT32)pstClient,pszCallbackName);
			continue;
		}

		//	nClient == 0 or nClientHandle == clientListHandle
		if (!nClientHandle || (nClientHandle == pstClient->nClientHandle))
		{
#ifdef CONFIG_DEBUG
			bWritten = TRUE;
#endif
			HxLOG_Debug("Write handle : [%d]\n", pstClient->nClientHandle);
			hErr = HLIB_SOCKET_Write(pstClient->nClientHandle, pvData, ulSize, nRequestId);
			if (hErr != ERR_OK)
			{
				if (pstRpc->pfnOnError != NULL)
				{
					pstRpc->pfnOnError((HINT32)pstClient, HLIB_SOCKET_GetError());
				}
			}
		}
	}

#ifdef 	CONFIG_DEBUG
	if (bWritten == FALSE)
		HxLOG_Warning("no client exists.\n");
#endif

	return ERR_OK;
}




static HERROR	hxrpc_Response(HINT32 nRpcHandle, HINT32 nClientHandle, HINT32 nRequestId, HERROR hErr, const HCHAR *pszSignature, va_list ap, HBOOL bSync)
{

	RPC_t		*pstRpc = (RPC_t*)nRpcHandle;
	HUINT32 	ulSize;
	const HCHAR *pszText;
	void		*pvData;


	if (pszSignature && !hxrpc_VerifySignature(pszSignature))
	{
		HxLOG_Error("Invalid Signature (%s)\n", pszSignature);

		return ERR_FAIL;
	}

	if(nClientHandle == 0)
	{
		HxLOG_Error("nClientHandle == 0\n");

		return ERR_FAIL;
	}

	if (bSync == TRUE)
		hxrpc_MakeSerializerData(pstRpc, pstRpc->usProgId, pstRpc->nSrzr, pstRpc->nSrzrParam, eHLIB_RPC_CMD_RESPONSE_SYNC, "eHLIB_RPC_CMD_RESPONSE_SYNC", hErr, pszSignature, ap);
	else
		hxrpc_MakeSerializerData(pstRpc, pstRpc->usProgId, pstRpc->nSrzr, pstRpc->nSrzrParam, eHLIB_RPC_CMD_RESPONSE, "eHLIB_RPC_CMD_RESPONSE", hErr, pszSignature, ap);

	if ((pszText = HLIB_SERIALIZER_Error(pstRpc->nSrzr)) != NULL)
	{
		HxLOG_Error("Serialize failed : %s\n", pszText);

		return ERR_FAIL;
	}

	pvData = HLIB_SERIALIZER_Pack(pstRpc->nSrzr, &ulSize);
#ifdef	CONFIG_RPC_LOG
	hxrpc_dbg_writeLog(pstRpc, pvData, ulSize, nRequestId);
#endif

	{
		if (nClientHandle != 0)
		{

			HxLOG_Debug("Write nClientHandle : [%d]\n", nClientHandle);
			HLIB_SOCKET_Write(nClientHandle, pvData, ulSize, nRequestId);
		}
	}

	return ERR_OK;


}


static HERROR	hxrpc_RegisterCallback(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature, void *pvData, HxRPC_Noti_t cbRpcCallback)
{
	RPC_t	*pstRpc = (RPC_t*)nRpcHandle;
	RPC_Callback_t	*pstCallback;

	//	Signature Check
	if (pszSignature && !hxrpc_VerifySignature(pszSignature))
	{
		HxLOG_Error("Invalid Signature (%s)\n", pszSignature);

		return ERR_FAIL;
	}

	if (pstRpc->pstCallbackTable == NULL)
		pstRpc->pstCallbackTable = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, NULL, (_HxDestroyNotify)hxrpc_FreeCallback);

	pstCallback = (RPC_Callback_t*)HLIB_HASH_Lookup(pstRpc->pstCallbackTable, pszCallbackName);
	if (pstCallback && pstCallback->pfnNotifier == cbRpcCallback)
	{
		HxLOG_Error("Callback is already registered! [%s]\n", pszCallbackName);

		return ERR_FAIL;
	}

	pstCallback = (RPC_Callback_t*)RPC_Calloc(sizeof(RPC_Callback_t));
	pstCallback->pszCallback  = RPC_StrDup(pszCallbackName);
	pstCallback->pvUserData   = pvData;
	pstCallback->pfnNotifier  = cbRpcCallback;

	HLIB_HASH_Insert(pstRpc->pstCallbackTable, pstCallback->pszCallback, pstCallback);

	return ERR_OK;
}

static HBOOL	hxrpc_FindCallbackProtoType(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature)
{
	RPC_t	*pstRpc = (RPC_t*)nRpcHandle;
	HCHAR	*pszProtoType;

	if (pszSignature && !hxrpc_VerifySignature(pszSignature))
	{
		HxLOG_Error("invalid signature : %s\n", pszSignature);

		return FALSE;
	}

	if (pstRpc->pstMethodTable == NULL)
	{
		HxLOG_Error("Callback is not registered!\n");

		return FALSE;
	}

	pszProtoType = hxrpc_MakeProtoType(pstRpc, pszCallbackName, pszSignature);
	if (HLIB_HASH_Lookup(pstRpc->pstMethodTable, pszProtoType))
		return TRUE;

	return FALSE;
}

static HERROR	hxrpc_SetFakeCallbackProtoType(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszOldSignature, const HCHAR *pszNewSignature)
{
	RPC_t	*pstRpc = (RPC_t*)nRpcHandle;
	HCHAR	*pszProtoType;
	RPC_Method_t	*pstCallbackProto;

	if (pszOldSignature && !hxrpc_VerifySignature(pszOldSignature))
	{
		HxLOG_Error("invalid signature : %s\n", pszOldSignature);

		return ERR_FAIL;
	}

	if (pszNewSignature && !hxrpc_VerifySignature(pszNewSignature))
	{
		HxLOG_Error("invalid signature : %s\n", pszNewSignature);

		return ERR_FAIL;
	}

	if (pstRpc->pstMethodTable == NULL)
	{
		HxLOG_Error("Callback is not registered!\n");

		return ERR_FAIL;
	}

	pszProtoType = hxrpc_MakeProtoType(pstRpc, pszCallbackName, pszOldSignature);
	pstCallbackProto = (RPC_Method_t*)HLIB_HASH_Lookup(pstRpc->pstMethodTable, pszProtoType);
	if (pstCallbackProto == NULL)
	{
		HxLOG_Error("Can't find existed callback named [%s]\n", pszProtoType);
		return ERR_FAIL;
	}
	if (pstCallbackProto && !pstCallbackProto->bReplaced)
	{
		pstCallbackProto->bReplaced = TRUE;
		if (pstCallbackProto->pszRepliProtoType == NULL)
			pstCallbackProto->pszRepliProtoType = RPC_StrDup(hxrpc_MakeProtoType(pstRpc, pszCallbackName, pszNewSignature));

		return ERR_OK;
	} else
	{
		HxLOG_Debug("The prototype is already replaced!! [%s]\n", pszCallbackName);
	}

	return ERR_FAIL;
}

static HERROR	hxrpc_RegisterCallbackProtoType(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature, const HCHAR *pszDescription)
{
	RPC_t	*pstRpc = (RPC_t*)nRpcHandle;
	RPC_Method_t	*pstMethod;
	HCHAR	*pszProtoType;

	if (pszSignature && !hxrpc_VerifySignature(pszSignature))
	{
		HxLOG_Error("invalid signature : %s\n", pszSignature);

		return ERR_FAIL;
	}

	if (pstRpc->pstMethodTable == NULL)
		pstRpc->pstMethodTable = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, NULL, (_HxDestroyNotify)hxrpc_FreeMethod);

	pszProtoType = hxrpc_MakeProtoType(pstRpc, pszCallbackName, pszSignature);

	pstMethod = (RPC_Method_t*)RPC_Calloc(sizeof(RPC_Method_t));
	pstMethod->pszProtoType	  = RPC_StrDup(pszProtoType);
	pstMethod->bCallback      = TRUE;
#ifdef	CONFIG_DEBUG
	pstMethod->pszDescription = RPC_StrDup(pszDescription);
#endif

	if (HLIB_HASH_Lookup(pstRpc->pstMethodTable, pstMethod->pszProtoType))
	{
		HxLOG_Error("Method is already registered! [%s]\n", pszCallbackName);
		hxrpc_FreeMethod(pstMethod);

		return ERR_FAIL;
	}

	HLIB_HASH_Insert(pstRpc->pstMethodTable, pstMethod->pszProtoType, pstMethod);

	return ERR_OK;
}

/*
	RPC Initializing.
*/
HINT32	HLIB_RPC_Open(HINT32 nRoute, HINT32 nProgId)
{
	HINT32	nRpcHandle;

	//	Initialize RPC Module
	nRpcHandle = hxrpc_Open(nRoute, nProgId, FALSE);

	//	Register System RPC
	HLIB_RPC_RegisterCall(nRpcHandle, "rpcsys:RegisterCallback", "ss", hxrpc_cbRegisterCallback, (void*)nRpcHandle, "RPC system call : Register Callback");

	return nRpcHandle;
}

/*
	RPC Initializing.
	with Ignore error check
*/
HINT32	HLIB_RPC_OpenEx(HINT32 nRoute, HINT32 nProgId, HBOOL bIgnoreError)
{
	HINT32	nRpcHandle;

	//	Initialize RPC Module
	nRpcHandle = hxrpc_Open(nRoute, nProgId, bIgnoreError);

	//	Register System RPC
	HLIB_RPC_RegisterCall(nRpcHandle, "rpcsys:RegisterCallback", "ss", hxrpc_cbRegisterCallback, (void*)nRpcHandle, "RPC system call : Register Callback");

	return nRpcHandle;
}

/*
	RPC Destroy
*/
HINT32	HLIB_RPC_Close(HINT32 nRpcHandle)
{
	RPC_t	*pstRpc = (RPC_t*)nRpcHandle;

	HxLog_NullCheckReturnValue(pstRpc, ERR_FAIL);

	if (pstRpc->pstCBuf)
		HLIB_CBUF_Delete(pstRpc->pstCBuf);
	if (pstRpc->nDocStream)
		HLIB_RWSTREAM_Close(pstRpc->nDocStream);
	if (pstRpc->nSrzr)
		HLIB_SERIALIZER_Close(pstRpc->nSrzr);
	if (pstRpc->nSrzrParam)
		HLIB_SERIALIZER_Close(pstRpc->nSrzrParam);
	if (pstRpc->stPool.pstObj)
		HLIB_SERIALIZER_CloseUnpackCache(pstRpc->stPool.pstObj);
	if (pstRpc->stPool.pstJob)
		HLIB_QUEUE_Delete(pstRpc->stPool.pstJob);
	if (pstRpc->pstLastUnpackedObject)
		HLIB_SERIALIZER_FreeObject(pstRpc->pstLastUnpackedObject);
	if (pstRpc->pstLastUnpackedParam)
		HLIB_SERIALIZER_FreeObject(pstRpc->pstLastUnpackedParam);
	HxSEMT_Destroy(pstRpc->ulSemId);

	RPC_Free(pstRpc);

	return 0;
}

/*
	Client Side
	Call method in client.
	Return value is request id.
*/
HINT32	HLIB_RPC_Call(HINT32 nRpcHandle, const HCHAR *pszMethodName, const HCHAR *pszSignature, ...)
{
	HINT32		res;
	va_list		ap;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, -1);

	ENTER_RPC_SEM(nRpcHandle);

	va_start(ap, pszSignature);
	res = hxrpc_Method(nRpcHandle, NULL, NULL, pszMethodName, pszSignature, ap, FALSE);
	va_end(ap);

	LEAVE_RPC_SEM(nRpcHandle);

	return res;
}

HINT32	HLIB_RPC_Request(HINT32 nRpcHandle, HxRPC_Response_t pfnCallback, void *pvUserdata, const HCHAR *pszMethodName, const HCHAR *pszSignature, ...)
{
	HINT32		res;
	va_list		ap;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, -1);

	ENTER_RPC_SEM(nRpcHandle);

	va_start(ap, pszSignature);
	res = hxrpc_Method(nRpcHandle, pfnCallback, pvUserdata, pszMethodName, pszSignature, ap, FALSE);
	va_end(ap);

	LEAVE_RPC_SEM(nRpcHandle);

	return res;

}

/*
	Wait RPC_Method/RPC_Request
*/
HERROR  HLIB_RPC_Join (HINT32 nRpcHandle, HINT32 nRequestId, HUINT32 ulTimeout)
{
	RPC_t		*pstRpc = (RPC_t*)nRpcHandle;
	HUINT32	ulTick;

	HxLOG_Trace();
	HxLog_NullCheckReturnValue(pstRpc, ERR_FAIL);
	HxLog_ExprCheckReturnValue(nRequestId < 0, ERR_FAIL);

	HxLOG_Debug("nRequestId: %d, ulTimeout: %d \n", nRequestId, ulTimeout);

	ulTick = HLIB_STD_GetSystemTick();
	for (;;)
	{
		HLIB_STD_TaskSleep(10);
		if (pstRpc)
		{
			ENTER_RPC_SEM(nRpcHandle);
			if (HLIB_TREE_Lookup(pstRpc->pWaitForResponse, (void *)nRequestId) == NULL)
			{
				HxLOG_Debug(HxANSI_COLOR_GREEN("JOIN %X")"\n", nRequestId);
				LEAVE_RPC_SEM(nRpcHandle);
				return ERR_OK;
			}
			LEAVE_RPC_SEM(nRpcHandle);
		}

		if (HLIB_STD_GetSystemTick() - ulTick > ulTimeout)
			break;

	}

	{
		RPC_Request_t *pstReq = NULL;

		ENTER_RPC_SEM(pstRpc);
		pstReq = hxrpc_PopRequest(pstRpc, nRequestId);
		if (pstReq)
		{
			HLIB_QUEUE_Push(pstRpc->pReqFreed, pstReq);
		}
		LEAVE_RPC_SEM(pstRpc);
	}

	HxLOG_Critical(HxANSI_COLOR_RED("JOIN TIMED OUT")"\n");
	return ERR_FAIL;
}


HINT32	HLIB_RPC_SyncRequest(HINT32 nRpcHandle, HxRPC_Response_t pfnCallback, void *pvUserdata, const HCHAR *pszMethodName, const HCHAR *pszSignature, ...)
{
	HINT32		res;
	va_list		ap;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, -1);

	ENTER_RPC_SEM(nRpcHandle);

	va_start(ap, pszSignature);
	res = hxrpc_Method(nRpcHandle, pfnCallback, pvUserdata, pszMethodName, pszSignature, ap, TRUE);
	va_end(ap);

	LEAVE_RPC_SEM(nRpcHandle);

	return res;

}


HERROR	HLIB_RPC_SyncGet(HINT32 nRpcHandle, HINT32 nRequestId, HUINT32 ulTimeout, void *pRetData, HINT32 nSize, HxObjectTag_e type)
{
	HERROR		hRet = ERR_FAIL;
	RPC_Request_t *pstReq = NULL;
	RPC_t	*pstRpc = (RPC_t*)nRpcHandle;
	HINT32	msgRet;
	HINT32	msg;
	HULONG	msgQ = 0;

	HxLog_NullCheckReturnValue(pstRpc, ERR_FAIL);
	HxLog_ExprCheckReturnValue(nRequestId < 0, ERR_FAIL);

	ENTER_RPC_SEM(pstRpc);
	pstReq = (RPC_Request_t *) HLIB_TREE_Lookup(pstRpc->pWaitForResponse, (void *)nRequestId);
	if (pstReq != NULL)
	{
		msgQ = pstReq->msgQ;
	}
	LEAVE_RPC_SEM(pstRpc);

	/* Is enough 5 sec */
	ulTimeout = 5*1000;

	while (1)
	{
		msgRet = HLIB_MSG_ReceiveTimeout(msgQ, (void *) &msg, sizeof(HINT32), ulTimeout);
		if (msgRet != VK_OK)
		{
			HxLOG_Critical("RPC Loss ? \n");
			break;
		}

		ENTER_RPC_SEM(pstRpc);
		pstReq = (RPC_Request_t *) HLIB_TREE_Lookup(pstRpc->pWaitForResponse, (void *)nRequestId);
		if (pstReq == NULL)
		{
			HxLOG_Critical("Check it. ? \n");
			LEAVE_RPC_SEM(pstRpc);
			break;
		}
		if (pstReq->bRequestDone == FALSE)
		{
			HxLOG_Critical("Check it. ? \n");
			LEAVE_RPC_SEM(pstRpc);
			break;
		}

		hRet = pstReq->hErrRequest;
		if (hRet != ERR_OK)
		{
			LEAVE_RPC_SEM(pstRpc);
			break;
		}
		if (type != pstReq->requestData.type)
		{
			HxLOG_Critical("Check input and output \n");
			hRet = ERR_FAIL;
			LEAVE_RPC_SEM(pstRpc);
			break;
		}

		switch (type)
		{
			case HxOBJECT_TYPE_BIN:
				{
					if (nSize >= pstReq->requestData.u.bin.size)
					{
						if ((pstReq->requestData.u.bin.data) && (pRetData))
						{
							memcpy(pRetData, pstReq->requestData.u.bin.data, pstReq->requestData.u.bin.size);
							if (nSize > pstReq->requestData.u.bin.size)
							{
#if !defined(CONFIG_PROD_OS_EMULATOR)
								memset(pRetData + pstReq->requestData.u.bin.size, 0, (nSize - pstReq->requestData.u.bin.size));
#endif
							}
						}
					}
					else
					{
						HxLOG_Critical(HxANSI_COLOR_RED("Size error")"\n");
						hRet = ERR_FAIL;
					}

					if (pstReq->requestData.u.bin.data)
					{
						HLIB_MEM_Free(pstReq->requestData.u.bin.data);
						pstReq->requestData.u.bin.data = NULL;
						pstReq->requestData.u.bin.size = 0;
					}
				}
				break;
			case HxOBJECT_TYPE_INT:
				{
					HINT32 *pData = (HINT32 *) pRetData;

					*pData = pstReq->requestData.u.num;
				}
				break;
			case HxOBJECT_TYPE_INT64:
				{
					HINT64 *pData = (HINT64 *) pRetData;

					*pData = pstReq->requestData.u.num64;
				}
				break;
			default:
				break;
		}

		LEAVE_RPC_SEM(pstRpc);
		break;
	}

	ENTER_RPC_SEM(pstRpc);
	pstReq = hxrpc_PopRequest(pstRpc, nRequestId);
	if (pstReq)
	{
		HLIB_MSG_Destroy(pstReq->msgQ);
		HLIB_QUEUE_Push(pstRpc->pReqFreed, pstReq);
	}
	LEAVE_RPC_SEM(pstRpc);
	return hRet;
}



/*
	Process Method and Callback.
*/
HERROR	HLIB_RPC_ProcessRPC(HINT32 nRpcHandle, HUINT32 ulSize, void *pvData, HUINT32 ulRequestId, HINT32 nClientHandle)
{
	RPC_t					*pstRpc = (RPC_t*)nRpcHandle;
	const HCHAR				*pszSignature;
	HxObject_t				*pstObject;
	HxRPC_CMD_e				 eCmd;
	HINT32					 nProgId;
	RPC_ExecArg_t			 stExecArg;
	HERROR					 err;
	HERROR					 hReturnErr;

	HxLog_NullCheckReturnValue(pstRpc, ERR_FAIL);
	HxLog_ExprCheckReturnValue(ulSize == 0, ERR_FAIL);
	HxLog_NullCheckReturnValue(pvData, ERR_FAIL);

	HxSTD_MemSet(&stExecArg, 0, sizeof(RPC_ExecArg_t));
	stExecArg.eCmd = eHLIB_RPC_CMD_NONE;

	ENTER_RPC_SEM(nRpcHandle);

	if (NULL != pstRpc->pstLastUnpackedObject)
	{
		HLIB_SERIALIZER_FreeObjectWithCache(pstRpc->stPool.pstObj, pstRpc->pstLastUnpackedObject);
		pstRpc->pstLastUnpackedObject = NULL;
	}

	if (NULL != pstRpc->pstLastUnpackedParam)
	{
		HLIB_SERIALIZER_FreeObjectWithCache(pstRpc->stPool.pstObj, pstRpc->pstLastUnpackedParam);
		pstRpc->pstLastUnpackedParam = NULL;
	}

	pstObject = HLIB_SERIALIZER_UnpackWithCache(pstRpc->stPool.pstObj, pvData, ulSize, TRUE);
	if (pstObject == NULL)
	{
		LEAVE_RPC_SEM(nRpcHandle);
		return ERR_FAIL;
	}
	pstRpc->pstLastUnpackedObject = pstObject;

	pszSignature = hxrpc_MakeSignature(pstRpc, pstObject);

	if (pszSignature == NULL || !HLIB_STD_StrStartWith(pszSignature, "iiis"))
	{
		HxLOG_Error("Invalid signature [%s]\n", pszSignature);

		LEAVE_RPC_SEM(nRpcHandle);
		return ERR_FAIL;
	}

	eCmd = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;
	nProgId = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
	hReturnErr = (HERROR)(HxOBJECT_ARR_VAL(pstObject, 2)->u.num); // for response.

	//	make or set table for matching programid and client handle
	if (pstRpc->pstProgIdTable == NULL)
		pstRpc->pstProgIdTable = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, NULL);
	if (HLIB_HASH_Lookup(pstRpc->pstProgIdTable, (void*)nClientHandle) == NULL)
		HLIB_HASH_Insert(pstRpc->pstProgIdTable, (void*)nClientHandle, (void*)nProgId);

#ifdef	CONFIG_RPC_LOG
	hxrpc_dbg_writeLog(pstRpc, pvData, ulSize, ulRequestId);
#endif

	switch (eCmd)
	{
	case eHLIB_RPC_CMD_REQUEST:
		err = hxrpc_GetArgsRequest(pstRpc, pstObject, ulRequestId, nClientHandle, &stExecArg);
		break;

	case eHLIB_RPC_CMD_CALLBACK:
		err = hxrpc_GetArgsNotifier(pstRpc, pstObject, ulRequestId, &stExecArg);
		break;

	case eHLIB_RPC_CMD_RESPONSE:
		err = hxrpc_GetArgsResponse(pstRpc, pstObject, ulRequestId, hReturnErr, &stExecArg, FALSE);
		break;

	case eHLIB_RPC_CMD_REQUEST_SYNC:
		err = hxrpc_GetArgsRequest(pstRpc, pstObject, ulRequestId, nClientHandle, &stExecArg);
		break;

	case eHLIB_RPC_CMD_RESPONSE_SYNC:
		err = hxrpc_GetArgsResponse(pstRpc, pstObject, ulRequestId, hReturnErr, &stExecArg, TRUE);
		break;

	default:
		err = ERR_OK;
		break;
	}

	pstRpc->pstLastUnpackedObject = pstObject;
	pstRpc->pstLastUnpackedParam = stExecArg.pstUnpackedParam;

	LEAVE_RPC_SEM(nRpcHandle);

	if(err != ERR_OK)
	{
		HxLOG_Error("hlib rpc cmd result error = [%d]\n", err);
		return err;
	}

	switch (stExecArg.eCmd)
	{
	case eHLIB_RPC_CMD_REQUEST:
		{
			RPC_ExecReqArg_t	*pstReq = &(stExecArg.unArg.stRequest);

			err = pstReq->pfFunc(pstReq->nClientHandle, pstReq->nArgc, (void *)pstReq->ppstArgv, pstReq->ulRequestId, pstReq->pvUserData);
		}
		break;

	case eHLIB_RPC_CMD_CALLBACK:
		{
			RPC_ExecNotiArg_t	*pstNoti = &(stExecArg.unArg.stNotify);

			err = pstNoti->pfFunc(pstNoti->nArgc, (void *)pstNoti->ppstArgv, pstNoti->ulRequestId, pstNoti->pvUserData);
		}
		break;

	case eHLIB_RPC_CMD_RESPONSE:
		{
			RPC_ExecRespArg_t	*pstResp = &(stExecArg.unArg.stResponse);
			RPC_Request_t		*pstReq;

			err = pstResp->pfFunc(pstResp->hErr, pstResp->nArgc, (void *)pstResp->ppstArgv, pstResp->ulRequestId, pstResp->pvUserData);

			ENTER_RPC_SEM(nRpcHandle);
			pstReq = hxrpc_PopRequest(pstRpc, ulRequestId);
			if (pstReq)
			{
				HLIB_QUEUE_Push(pstRpc->pReqFreed, pstReq);
			}
			LEAVE_RPC_SEM(nRpcHandle);
		}
		break;

	case eHLIB_RPC_CMD_REQUEST_SYNC:
		{
			RPC_ExecReqArg_t	*pstReq = &(stExecArg.unArg.stRequest);

			err = pstReq->pfFunc(pstReq->nClientHandle, pstReq->nArgc, (void *)pstReq->ppstArgv, pstReq->ulRequestId, pstReq->pvUserData);
		}
		break;

	case eHLIB_RPC_CMD_RESPONSE_SYNC:
		{
			RPC_ExecRespArg_t	*pstResp = &(stExecArg.unArg.stResponse);
			RPC_Request_t		*pstReq = NULL;

			ENTER_RPC_SEM(nRpcHandle);
			pstReq = hxrpc_GetRequest(pstRpc, ulRequestId);
			if (pstReq)
			{
				pstReq->bRequestDone = TRUE;
				pstReq->hErrRequest = pstResp->hErr;

				if ((pstReq->hErrRequest == ERR_OK) && (pstResp->nArgc > 0))
				{
					pstReq->requestData.type = pstResp->ppstArgv[0]->type;
					switch (pstResp->ppstArgv[0]->type)
					{
						case HxOBJECT_TYPE_BIN:
							pstReq->requestData.u.bin.size = pstResp->ppstArgv[0]->u.bin.size;
							pstReq->requestData.u.bin.data = (void *) HLIB_MEM_Malloc(pstReq->requestData.u.bin.size);
							if (pstReq->requestData.u.bin.data)
							{
								memcpy(pstReq->requestData.u.bin.data, pstResp->ppstArgv[0]->u.bin.data, pstReq->requestData.u.bin.size);
							}
							break;
						case HxOBJECT_TYPE_INT:
							pstReq->requestData.u.num = pstResp->ppstArgv[0]->u.num;
							break;
						case HxOBJECT_TYPE_INT64:
							pstReq->requestData.u.num64 = pstResp->ppstArgv[0]->u.num64;
							break;
						default:
							break;
					}
				}

				{
					HINT32	msg = 0;

					(void) HLIB_MSG_Send(pstReq->msgQ, (const void *) &msg, sizeof(HINT32));
				}
			}
			LEAVE_RPC_SEM(nRpcHandle);
		}
		break;


	default:
		err = ERR_OK;
		break;
	}

	return err;
}

/*
	Server Side
	register method in server module
*/
HERROR	HLIB_RPC_RegisterCall(HINT32 nRpcHandle, const HCHAR *pszMethodName, const HCHAR *pszSignature, HxRPC_Func_t pfnFunction, void *pvUserData, const HCHAR *pszDescription)
{
	HERROR	res;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, ERR_FAIL);
	HxLog_NullCheckReturnValue(pszMethodName, ERR_FAIL);
	HxLog_NullCheckReturnValue(pfnFunction, ERR_FAIL);

	ENTER_RPC_SEM(nRpcHandle);
	res = hxrpc_RegisterMethod(nRpcHandle, pszMethodName, pszSignature, pfnFunction, pvUserData, pszDescription);
	LEAVE_RPC_SEM(nRpcHandle);

	return res;
}

/*
	Server Side
	register method in server module
*/
HERROR	HLIB_RPC_RegisterRequest(HINT32 nRpcHandle, const HCHAR *pszMethodName, const HCHAR *pszSignature, HxRPC_Func_t pfnFunction, void *pvUserData, const HCHAR *pszDescription)
{
	HERROR	res;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, ERR_FAIL);
	HxLog_NullCheckReturnValue(pszMethodName, ERR_FAIL);
	HxLog_NullCheckReturnValue(pfnFunction, ERR_FAIL);

	ENTER_RPC_SEM(nRpcHandle);
	res = hxrpc_RegisterMethod(nRpcHandle, pszMethodName, pszSignature, pfnFunction, pvUserData, pszDescription);
	LEAVE_RPC_SEM(nRpcHandle);

	return res;
}



//
//	Server Side
//	send callback with parameters
HERROR	HLIB_RPC_Notify(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature, ...)
{
	HERROR	res;
	va_list	ap;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, ERR_FAIL);
	HxLog_NullCheckReturnValue(pszCallbackName, ERR_FAIL);
	ENTER_RPC_SEM(nRpcHandle);

	va_start(ap, pszSignature);
	res = hxrpc_Callback(nRpcHandle, 0, 0, pszCallbackName, pszSignature, FALSE, ap);
	va_end(ap);

	LEAVE_RPC_SEM(nRpcHandle);

	return res;
}

//
//	Server Side
//	send callback with parameters
HERROR	HLIB_RPC_NotifyWithRequestId(HINT32 nRpcHandle, const HCHAR *pszCallbackName, HINT32 nRequestId, const HCHAR *pszSignature, ...)
{
	HERROR	res;
	va_list	ap;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, ERR_FAIL);
	HxLog_NullCheckReturnValue(pszCallbackName, ERR_FAIL);
	ENTER_RPC_SEM(nRpcHandle);

	va_start(ap, pszSignature);
	res = hxrpc_Callback(nRpcHandle, 0, nRequestId, pszCallbackName, pszSignature, FALSE, ap);
	va_end(ap);

	LEAVE_RPC_SEM(nRpcHandle);

	return res;
}


//
//	Server Side
//	send callback with parameters
//HLIB_RPC_CreateNotification(server side), HLIB_RPC_RegisterNotifyCallback (client side) ½Ã
//signature °ªÀ» NULL ·Î ÁáÀ» ¶§ »ç¿ë.
HERROR	HLIB_RPC_NotifyWithoutSignatureCheck(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature, ...)
{
	HERROR	res;
	va_list	ap;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, ERR_FAIL);
	HxLog_NullCheckReturnValue(pszCallbackName, ERR_FAIL);
	ENTER_RPC_SEM(nRpcHandle);

	va_start(ap, pszSignature);
	res = hxrpc_Callback(nRpcHandle, 0, 0, pszCallbackName, pszSignature, TRUE, ap);
	va_end(ap);

	LEAVE_RPC_SEM(nRpcHandle);

	return res;
}


//
//	Server Side
//	send response regarding HLIB_RPC_Request
void		HLIB_RPC_Response (HINT32 nRpcHandle, HINT32 nClientHandle, HINT32 nRequestId, HERROR hErr, const HCHAR *pszSignature, ...)
{

	HERROR	res;
	va_list ap;

	HxLog_NullCheckReturnOnly((RPC_t*) nRpcHandle);
	HxLog_ExprCheckReturnOnly(nClientHandle == 0);
	ENTER_RPC_SEM(nRpcHandle);

	va_start(ap, pszSignature);
	res = hxrpc_Response(nRpcHandle, nClientHandle, nRequestId, hErr, pszSignature, ap, FALSE);
	va_end(ap);

	LEAVE_RPC_SEM(nRpcHandle);

	return;

}


//
//	Server Side
//	send response regarding HLIB_RPC_Request
void		HLIB_RPC_ResponseSync (HINT32 nRpcHandle, HINT32 nClientHandle, HINT32 nRequestId, HERROR hErr, const HCHAR *pszSignature, ...)
{
	va_list ap;

	HxLog_NullCheckReturnOnly((RPC_t*) nRpcHandle);
	HxLog_ExprCheckReturnOnly(nClientHandle == 0);
	ENTER_RPC_SEM(nRpcHandle);

	va_start(ap, pszSignature);
	(void) hxrpc_Response(nRpcHandle, nClientHandle, nRequestId, hErr, pszSignature, ap, TRUE);
	va_end(ap);

	LEAVE_RPC_SEM(nRpcHandle);

	return;

}


//	Server Side
//	send callback to specific client with parameter
HERROR	HLIB_RPC_CallbackSpecific(HINT32 nRpcHandle, HINT32 nClientHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature, ...)
{
	HERROR	res;
	va_list	ap;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, ERR_FAIL);
	HxLog_NullCheckReturnValue(pszCallbackName, ERR_FAIL);

	ENTER_RPC_SEM(nRpcHandle);

	va_start(ap, pszSignature);
	res = hxrpc_Callback(nRpcHandle, nClientHandle, 0, pszCallbackName, pszSignature, FALSE, ap);
	va_end(ap);

	LEAVE_RPC_SEM(nRpcHandle);

	return res;
}

//	ClientSide
//	Register Callback
HERROR	HLIB_RPC_RegisterNotifyCallback(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature, void *pvData, HxRPC_Noti_t cbRpcCallback)
{
	HERROR	res = ERR_FAIL;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, ERR_FAIL);
	HxLog_NullCheckReturnValue(pszCallbackName, ERR_FAIL);
	HxLog_NullCheckReturnValue(cbRpcCallback, ERR_FAIL);

	//	Register Callback in Server
	if (HLIB_RPC_Call(nRpcHandle, "rpcsys:RegisterCallback", "ss", pszCallbackName, pszSignature) >= 0)
	{
		ENTER_RPC_SEM(nRpcHandle);
		res = hxrpc_RegisterCallback(nRpcHandle, pszCallbackName, pszSignature, pvData, cbRpcCallback);
		LEAVE_RPC_SEM(nRpcHandle);
	}

	return res;
}

//	Server Side
//	Register Callback ProtoType
HERROR	HLIB_RPC_CreateNotification(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature, const HCHAR *pszDescription)
{
	HERROR	res;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, ERR_FAIL);
	HxLog_NullCheckReturnValue(pszCallbackName, ERR_FAIL);
	ENTER_RPC_SEM(nRpcHandle);

	res = hxrpc_RegisterCallbackProtoType(nRpcHandle, pszCallbackName, pszSignature, pszDescription);

	LEAVE_RPC_SEM(nRpcHandle);

	return res;
}

//	Server side
//	remove client handle list
HERROR	HLIB_RPC_RemoveClientHandle(HINT32 nRpcHandle, HINT32 nClientHandle)
{
	HERROR	res;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, ERR_FAIL);

	ENTER_RPC_SEM(nRpcHandle);

	res = hxrpc_RemoveClientHandle(nRpcHandle, nClientHandle);

	LEAVE_RPC_SEM(nRpcHandle);

	return res;
}

//	Server side
//	find callback prototype with given.
HBOOL	HLIB_RPC_FindCallbackPrototype(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature)
{
	HBOOL	ret;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, ERR_FAIL);
	HxLog_NullCheckReturnValue(pszCallbackName, ERR_FAIL);
	ENTER_RPC_SEM(nRpcHandle);

	ret = hxrpc_FindCallbackProtoType(nRpcHandle, pszCallbackName, pszSignature);

	LEAVE_RPC_SEM(nRpcHandle);

	return ret;
}

HERROR	HLIB_RPC_SetFakeCallbackPrototype(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszOldSignature, const HCHAR *pszNewSignature)
{
	HERROR	ret;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, ERR_FAIL);
	HxLog_NullCheckReturnValue(pszCallbackName, ERR_FAIL);
	ENTER_RPC_SEM(nRpcHandle);

	ret = hxrpc_SetFakeCallbackProtoType(nRpcHandle, pszCallbackName, pszOldSignature, pszNewSignature);

	LEAVE_RPC_SEM(nRpcHandle);

	return ret;
}

//	Make Serialized Data with arguments.
//	return internal memory without allocation.
void*	HLIB_RPC_MakeSerialize(HINT32 nRpcHandle, const HCHAR *pszSignature, va_list ap, HINT32 *pnSize)
{
	void	*pvData;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, NULL);

	ENTER_RPC_SEM(nRpcHandle);

	pvData = hxrpc_MakeSerializeArgs(nRpcHandle, pszSignature, ap, pnSize);

	LEAVE_RPC_SEM(nRpcHandle);

	return pvData;
}

//	Server Side
//	Get ProgramId from ClientHandle
HERROR	HLIB_RPC_GetProgramIdFromClientHandle(HINT32 nRpcHandle, HINT32 nClientHandle, HINT32 *pnProgramId)
{
	HERROR	res;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, ERR_FAIL);

	ENTER_RPC_SEM(nRpcHandle);

	res = hxrpc_GetProgIdFromClientHandle(nRpcHandle, nClientHandle, pnProgramId);

	LEAVE_RPC_SEM(nRpcHandle);

	return res;
}

static const HCHAR *	hxrpc_MakeHelp(RPC_t *pstRpc, const RPC_Method_t *pstMethod, HBOOL bHighlight)
{
	HCHAR	*szBuf;
	HCHAR	*szSave;
	HCHAR	*szFunction;
	HCHAR	*szSignature;
	HUINT32	ulLen;
	HINT32	nStream;
	HCHAR	*pszProto;

	nStream = pstRpc->nDocStream;
	HLIB_RWSTREAM_Rewind(nStream);

	if (pstMethod->pszRepliProtoType)
		pszProto = pstMethod->pszRepliProtoType;
	else
		pszProto = pstMethod->pszProtoType;

	ulLen = strlen(pszProto) + 1;
	szBuf = HLIB_CBUF_GetBuffer(pstRpc->pstCBuf, ulLen);
	memcpy(szBuf, pszProto, ulLen);

#define _RESET					"[00;39;49m"
#define _KEYWORD				"[01;32m"
#define _FUNCTION				"[01;34m"
#define _TYPE					"[01;35m"

	if (pstMethod->bReplaced)
		HLIB_RWSTREAM_Print(nStream, "(R)");
	else
		HLIB_RWSTREAM_Print(nStream, "   ");

	if (pstMethod->bCallback)
	{
		if (bHighlight)
			HLIB_RWSTREAM_Print(nStream, _KEYWORD"callback "_RESET);
		else
			HLIB_RWSTREAM_Print(nStream, "callback ");
	} else
	{
		if (bHighlight)
			HLIB_RWSTREAM_Print(nStream, _KEYWORD"    void "_RESET);
		else
			HLIB_RWSTREAM_Print(nStream, "    void ");
	}

	szFunction  = HxSTD_StrTok(szBuf, "()", &szSave);
	if (bHighlight)
		HLIB_RWSTREAM_Print(nStream, _FUNCTION"%s"_RESET, szFunction);
	else
		HLIB_RWSTREAM_Print(nStream, "%s(%s)", szFunction);

	HLIB_RWSTREAM_Print(nStream, " (");
	szSignature = HxSTD_StrTok(NULL , "()", &szSave);
	if (szSignature == NULL)
	{
		if (bHighlight)
			HLIB_RWSTREAM_Print(nStream, _KEYWORD"void"_RESET);
		else
			HLIB_RWSTREAM_Print(nStream, "void");
	}
	else if (strcmp(szSignature, "function") == 0)
	{
		if (bHighlight)
			HLIB_RWSTREAM_Print(nStream, _KEYWORD"function"_RESET" callback");
		else
			HLIB_RWSTREAM_Print(nStream, "function callback");
	}
	else
	{
		HINT32	c = 0;
		while (*szSignature != '\0')
		{
			if (c > 0)
				HLIB_RWSTREAM_Print(nStream, ", ");

			if (bHighlight)
			{
				switch(*szSignature++)
				{
				case 's': HLIB_RWSTREAM_Print(nStream, _KEYWORD"const"_TYPE" HCHAR"_RESET" *string"); break;
				case 'i': HLIB_RWSTREAM_Print(nStream, _TYPE"HINT32"_RESET" integer"); break;
				case 'l': HLIB_RWSTREAM_Print(nStream, _TYPE"HINT64"_RESET" integer"); break;
				case 'b': HLIB_RWSTREAM_Print(nStream, _TYPE"void"_RESET" *pvData, "_TYPE"int"_RESET" size"); break;
				default:  HLIB_RWSTREAM_Print(nStream, _KEYWORD"void"_RESET" * unknown"); break;
				}
			}
			else
			{
				switch(*szSignature++)
				{
				case 's': HLIB_RWSTREAM_Print(nStream, "const HCHAR *string"); break;
				case 'i': HLIB_RWSTREAM_Print(nStream, "HINT32 integer"); break;
				case 'l': HLIB_RWSTREAM_Print(nStream, "HINT64 integer"); break;
				case 'b': HLIB_RWSTREAM_Print(nStream, "void *pvData, HINT32 size"); break;
				default:  HLIB_RWSTREAM_Print(nStream, "void * unknown"); break;
				}
			}
			c++;
		}
	}
	HLIB_RWSTREAM_Print(nStream, ")");
	return HLIB_RWSTREAM_GetBuf(nStream);
}

#if defined(CONFIG_PROD_OS_EMULATOR)
#define _DEF_FPRINTF(x,	y, ...)	DI_UART_Print(y, __VA_ARGS__);
#else
#define _DEF_FPRINTF	fprintf
#endif

HERROR	HLIB_RPC_PrintHelp(HINT32 nRpcHandle, FILE *fp)
{
	RPC_t				*pstRpc = (RPC_t*)nRpcHandle;
	HxHASH_TableIter_t	stIter;
	RPC_Method_t		*pstMethod;
	const HCHAR			*pszProtoType;
	HBOOL				bHighlight = FALSE;

	HxSEMT_Get(pstRpc->ulSemId);

	if (fp == NULL)
	{
		fp = stdout;
		bHighlight = TRUE;
	}

	_DEF_FPRINTF(fp, "\n    ==== HELP (%s) =====\n\n", HLIB_STD_GetCurProcessName());

	HLIB_HASH_IterInit(&stIter, pstRpc->pstMethodTable);
	while (HLIB_HASH_IterNext(&stIter, NULL, (void**)&pstMethod))
	{
		if (pstMethod->bCallback)
			continue;
		pszProtoType = hxrpc_MakeHelp(pstRpc, pstMethod, bHighlight);
		_DEF_FPRINTF(fp, "%s\n", pszProtoType);
		if (pstMethod->pszDescription)
			_DEF_FPRINTF(fp, "\t%s\n", pstMethod->pszDescription);
	}

	HLIB_HASH_IterInit(&stIter, pstRpc->pstMethodTable);
	while (HLIB_HASH_IterNext(&stIter, NULL, (void**)&pstMethod))
	{
		if (!pstMethod->bCallback || pstMethod->bReplaced)
			continue;
		pszProtoType = hxrpc_MakeHelp(pstRpc, pstMethod, bHighlight);
		_DEF_FPRINTF(fp, "%s\n", pszProtoType);
		if (pstMethod->pszDescription)
			_DEF_FPRINTF(fp, "\t%s\n", pstMethod->pszDescription);
	}

	HLIB_HASH_IterInit(&stIter, pstRpc->pstMethodTable);
	while (HLIB_HASH_IterNext(&stIter, NULL, (void**)&pstMethod))
	{
		if (!pstMethod->bReplaced)
			continue;
		pszProtoType = hxrpc_MakeHelp(pstRpc, pstMethod, bHighlight);
		_DEF_FPRINTF(fp, "%s\n", pszProtoType);
		if (pstMethod->pszDescription)
			_DEF_FPRINTF(fp, "\t%s\n", pstMethod->pszDescription);
	}

	HxSEMT_Release(pstRpc->ulSemId);

	return ERR_OK;
}

//	Client Side
HBOOL	HLIB_RPC_IsMyRequest(HINT32 nRequestId)
{
	HBOOL	bIsMyReqeust = FALSE;
	HUINT32	nPid = HLIB_STD_GetPID();

	HxLOG_Trace();

	bIsMyReqeust = HxRPC_CHECK_REQID(nPid, nRequestId);

	HxLOG_Debug("bIsMyReqeust %d \n", bIsMyReqeust);

	return bIsMyReqeust;
}

//	Client Side
//	Get ProgramId from RequestId
HUINT16	HLIB_RPC_GetProgramIdFromRequestId(HINT32 nRequestId)
{
	return ((nRequestId & 0xFFFF0000) >> 16);
}

HERROR	HLIB_RPC_SetOnError (HINT32 nRpcHandle, HxRPC_OnError_t pfnOnError)
{
	HERROR	res;

	HxLog_NullCheckReturnValue((RPC_t*)nRpcHandle, ERR_FAIL);

	ENTER_RPC_SEM(nRpcHandle);

	res = hxrpc_SetOnError(nRpcHandle, pfnOnError);

	LEAVE_RPC_SEM(nRpcHandle);

	return res;
}
HERROR	HLIB_RPC_SetClientFlag (HINT32 nClient, HxRPC_Flags_e eFlag)
{
	HERROR	res;

	HxLog_ExprCheckReturnValue(nClient == 0, ERR_FAIL);

	res = hxrpc_SetClientFlag(nClient, eFlag);

	return res;
}
