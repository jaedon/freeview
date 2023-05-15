/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hxrpc.h
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

#ifndef __HLIB_RPC_H__
#define __HLIB_RPC_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <_hlib_serializer.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	HxRPC_GetProgramIdByRequestId(nRequestId)		((nRequestId >> 16) & 0x0000FFFF)

#define	RPC_LOG_TEXT		"please use rpctool $(process).rpclog\nYou must compile octo-tools first.\n"

#define RPC_DEFAULT_TIMEOUT		(30 * 1000)				/* 30 sec */

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	eHLIB_RPC_CMD_REQUEST,
	eHLIB_RPC_CMD_CALLBACK,
	eHLIB_RPC_CMD_RESPONSE,
	eHLIB_RPC_CMD_REQUEST_SYNC,
	eHLIB_RPC_CMD_RESPONSE_SYNC,
	eHLIB_RPC_CMD_NONE
} HxRPC_CMD_e;

typedef enum
{
	eHLIB_RPC_FLAG_NONE = 0,
	eHLIB_RPC_FLAG_BLOCKED = 0x0001,
} HxRPC_Flags_e;

typedef HERROR	(* HxRPC_Func_t) (HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata);
typedef HERROR	(* HxRPC_Noti_t) (HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
typedef HERROR	(* HxRPC_Response_t) (HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData);
typedef HERROR	(* HxRPC_OnError_t) (HINT32 nClientHandle, HUINT32 errorno);


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
/*
	RPC Initializing.
*/
HINT32	HLIB_RPC_Open(HINT32 nRoute, HINT32 nProgId);

/*
	RPC Initializing.
	with Ignore error check
*/
HINT32	HLIB_RPC_OpenEx(HINT32 nRoute, HINT32 nProgId, HBOOL bIgnoreError);

/*
	RPC Destroy
*/
HINT32	HLIB_RPC_Close(HINT32 nRpcHandle);

/*
	Client Side
	Call method in client.
	Return value is request id.
*/
HINT32	HLIB_RPC_Call(HINT32 nRpcHandle, const HCHAR *pszMethodName, const HCHAR *pszSignature, ...);
HINT32	HLIB_RPC_Request(HINT32 nRpcHandle, HxRPC_Response_t pfnCallback, void *pvUserdata, const HCHAR *pszMethodName, const HCHAR *pszSignature, ...);

/*
	Wait RPC_Method/RPC_Request
*/
HERROR  HLIB_RPC_Join (HINT32 nRpcHandle, HINT32 nRequestId, HUINT32 ulTimeout);


/*
	Client Side
	Call method in client.
	Return value is request id.
*/
HINT32	HLIB_RPC_SyncRequest(HINT32 nRpcHandle, HxRPC_Response_t pfnCallback, void *pvUserdata, const HCHAR *pszMethodName, const HCHAR *pszSignature, ...);

/*
	Wait RPC_Method and Get Result/HLIB_RPC_SyncRequest
*/

HERROR	HLIB_RPC_SyncGet(HINT32 nRpcHandle, HINT32 nRequestId, HUINT32 ulTimeout, void *pRetData, HINT32 nSize, HxObjectTag_e type);


/*
	Server Side
	Process Method and Callback in Server side.
*/
HERROR	HLIB_RPC_ProcessRPC(HINT32 nRpcHandle, HUINT32 ulSize, void *pvData, HUINT32 ulRequestId, HINT32 nClientHandle);

/*
	Server Side
	register method in server module
*/
HERROR	HLIB_RPC_RegisterCall(HINT32 nRpcHandle, const HCHAR *pszMethodName, const HCHAR *pszSignature, HxRPC_Func_t pfnFunction, void *pvUserData, const HCHAR *pszDescription);
HERROR	HLIB_RPC_RegisterRequest(HINT32 nRpcHandle, const HCHAR *pszMethodName, const HCHAR *pszSignature, HxRPC_Func_t pfnFunction, void *pvUserData, const HCHAR *pszDescription);

//
//	Server Side
//	send callback with parameters
HERROR	HLIB_RPC_Notify(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature, ...);
HERROR	HLIB_RPC_NotifyWithRequestId(HINT32 nRpcHandle, const HCHAR *pszCallbackName, HINT32 nRequestId, const HCHAR *pszSignature, ...);
HERROR	HLIB_RPC_NotifyWithoutSignatureCheck(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature, ...);

//
//	Server Side
//	send response regarding HLIB_RPC_Request
void		HLIB_RPC_Response (HINT32 nRpcHandle, HINT32 nClientHandle, HINT32 nRequestId, HERROR hErr, const HCHAR *pszSignature, ...);


//
//	Server Side
//	send callback to specific client with parameters
HERROR	HLIB_RPC_CallbackSpecific(HINT32 nRpcHandle, HINT32 nClientHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature, ...);

//	ClientSide
//	Register Callback
HERROR	HLIB_RPC_RegisterNotifyCallback(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature, void *pvData, HxRPC_Noti_t cbRpcCallback);

//	Server Side
//	Register Callback ProtoType
HERROR	HLIB_RPC_CreateNotification(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature, const HCHAR *pszDescription);

//	Server Side
//	find callback prototypes
HBOOL	HLIB_RPC_FindCallbackPrototype(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszSignature);

//	Server Side
//	set fake callback prototypes
HERROR	HLIB_RPC_SetFakeCallbackPrototype(HINT32 nRpcHandle, const HCHAR *pszCallbackName, const HCHAR *pszOldSignature, const HCHAR *pszNewSignature);

HERROR	HLIB_RPC_RemoveClientHandle(HINT32 nRpcHandle, HINT32 nClientHandle);

//	Make Serialized Data with arguments.
//	return internal memory without allocation.
void*	HLIB_RPC_MakeSerialize(HINT32 nRpcHandle, const HCHAR *pszSignature, va_list ap, HINT32 *pnSize);

HERROR	HLIB_RPC_GetProgramIdFromClientHandle(HINT32 nRpcHandle, HINT32 nClientHandle, HINT32 *pnProgramId);

HBOOL	HLIB_RPC_IsMyRequest(HINT32 nRequestId);
HUINT16	HLIB_RPC_GetProgramIdFromRequestId(HINT32 nRequestId);

HERROR	HLIB_RPC_PrintHelp(HINT32 nRpcHandle, FILE *fp);

HERROR	HLIB_RPC_SetOnError (HINT32 nRpcHandle, HxRPC_OnError_t pfnOnError);

HERROR	HLIB_RPC_SetClientFlag (HINT32 nClient, HxRPC_Flags_e eFlag);

#endif
/* end of file */

