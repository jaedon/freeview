/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_drmagent.c
	@brief

	Description:  									\n
	Module: OAPI / API					\n

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
#include <vkernel.h>
#include <hlib.h>
#include <oapi.h>
#include <obama_int.h>

#include "../../oapi_mem.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/




/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC HULONG					s_ulDrmAgentSemId;
STATIC HBOOL						s_bDrmAgentInit;
STATIC OxDrmAgent_MessageResultNotifier_t	s_fnDrmAgentMessageResultApkNotifier;
STATIC OxDrmAgent_StatusChangeNotifier_t	s_fnDrmAgentStatusChangeApkNotifier;





#define	ENTER_OAPI_DRMAGENT_CACHE	VK_SEM_Get(s_ulDrmAgentSemId)
#define	LEAVE_OAPI_DRMAGENT_CACHE	VK_SEM_Release(s_ulDrmAgentSemId)

#define CHECK_INIT	{if(s_bDrmAgentInit != TRUE) return ERR_FAIL;}




/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

#define ___PRIVATE_FUNCTION___
STATIC void oapi_drmagent_onDrmMessage(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata);
STATIC void oapi_drmagent_onDrmSystemStatusChange(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata);






#define ___EVENT_LISTENER_FUNCTIONS_______________________________________________________________________
STATIC void oapi_drmagent_onDrmMessage(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HINT32						nMsgId = 0;
	HCHAR						*pzResultMsg = NULL;
	HUINT32						ulResultCode = 0;


	HxLOG_Trace();

	if (argc != 3)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", argc);
		return;
	}

	pzResultMsg = (HCHAR *)HxOBJECT_STR(argv[0]);
	nMsgId = HxOBJECT_INT(argv[1]);
	ulResultCode = HxOBJECT_INT(argv[2]);

	if(pzResultMsg == NULL)
	{
		HxLOG_Error("pzResultMsg == NULL \n");
		return;
	}

	HxLOG_Debug("pzResultMsg: %s, nMsgId: %d, ulResultCode: %d \n", pzResultMsg, nMsgId, ulResultCode);

	if(s_fnDrmAgentMessageResultApkNotifier != NULL)
	{
		s_fnDrmAgentMessageResultApkNotifier(pzResultMsg, nMsgId, ulResultCode);
	}

	return;
}


STATIC void oapi_drmagent_onDrmSystemStatusChange(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HCHAR						*pzDrmSystemId = NULL;


	HxLOG_Trace();

	if (argc != 1)
	{
		HxLOG_Error ("Argument wrong: nArgc(%d)\n", argc);
		return;
	}

	pzDrmSystemId = (HCHAR *)HxOBJECT_STR(argv[0]);

	if(pzDrmSystemId == NULL)
	{
		HxLOG_Error("pzDrmSystemId == NULL \n");
		return;
	}

	HxLOG_Debug("pzDrmSystemId: %s \n", pzDrmSystemId);

	if(s_fnDrmAgentStatusChangeApkNotifier != NULL)
	{
		s_fnDrmAgentStatusChangeApkNotifier(pzDrmSystemId);
	}

	return;

}



#define ___GLOBAL_FUNCTIONS___

void	OAPI_DRMAGENT_Init(OxDrmAgent_MessageResultNotifier_t fnDrmAgentMessageResultNotifier, OxDrmAgent_StatusChangeNotifier_t fnDrmAgentStatusChangeNotifier)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	HxLOG_Trace();

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	s_bDrmAgentInit = TRUE;

	if ( VK_SEM_Create(&s_ulDrmAgentSemId, "OapiCasMmiSema", VK_SUSPENDTYPE_PRIORITY) != VK_OK )
	{
		HxLOG_Error("VK_SEM_Create error! \n");
	}

	HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_DRMAGENT_onDrmMessage, "sii", NULL, (HxRPC_Noti_t)oapi_drmagent_onDrmMessage);
	HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_DRMAGENT_onDrmSystemStatusChanged, "s", NULL, (HxRPC_Noti_t)oapi_drmagent_onDrmSystemStatusChange);

	s_fnDrmAgentMessageResultApkNotifier = fnDrmAgentMessageResultNotifier;
	s_fnDrmAgentStatusChangeApkNotifier = fnDrmAgentStatusChangeNotifier;


}

void	OAPI_DRMAGENT_SendDrmMessage(const HCHAR *pzMsgType, const HCHAR *pzMsg, const HCHAR *pzDrmSystemId, HUINT16 usMsgId)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	HxLOG_Trace();

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if(pzMsgType == NULL || pzMsg == NULL || pzDrmSystemId == NULL)
	{
		HxLOG_Error("pzMsgType == NULL || pzMsg == NULL || pzDrmSystemId == NULL \n");
		return;
	}

	HxLOG_Debug("pzMsgType: %s\n", pzMsgType);
	HxLOG_Debug("pzMsg: %s\n", pzMsg);
	HxLOG_Debug("pzDrmSystemId: %s \n", pzDrmSystemId);
	HxLOG_Debug("pzDrmSystemId: %d \n", usMsgId);

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_DRMAGENT_SendDrmMessage, "sssi", pzMsgType, pzMsg, pzDrmSystemId, usMsgId);



}

void	OAPI_DRMAGENT_SendDrmSystemStatusRequest(const HCHAR *pzDrmSystemId)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	HxLOG_Trace();

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if(pzDrmSystemId == NULL)
	{
		HxLOG_Error("pzDrmSystemId == NULL \n");
		return;
	}

	HxLOG_Info("pzDrmSystemId: %s \n", pzDrmSystemId);

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_DRMAGENT_SendDrmSystemStautsRequest, "s", pzDrmSystemId);


}

