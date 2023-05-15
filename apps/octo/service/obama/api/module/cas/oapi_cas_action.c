/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_cas_action.c
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
STATIC HULONG					s_ulOapiCasActionSemId;
STATIC APKS_CAS_ActionInfo_t				s_stOapiCasActionStatus;

#define	ENTER_CAS_ACTION_CACHE	VK_SEM_Get(s_ulOapiCasActionSemId)
#define	LEAVE_CAS_ACTION_CACHE	VK_SEM_Release(s_ulOapiCasActionSemId)

#define CHECK_INIT	{if(s_bCasActionInit != TRUE) return ERR_FAIL;}

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL					s_bCasActionInit;
STATIC OxOutputAudio_Notifier_t	s_pfCasActionApkNotifier;
STATIC HUINT32				s_ulSessionId;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___


#define _____NOTIFICATION_CALLBACKS_____
STATIC void oapi_cas_action_onInited(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{

	if(argc != 1)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}
	else if(HxOBJECT_BIN_SIZE(argv[0]) != sizeof(APKS_CAS_ActionInfo_t))
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid struct size [%d]\n\n", __FUNCTION__, HxOBJECT_BIN_SIZE(argv[0]));
		return;
	}
	else
	{
		s_stOapiCasActionStatus = *(APKS_CAS_ActionInfo_t *)HxOBJECT_BIN_DATA(argv[0]);
		s_bCasActionInit = TRUE;

		HxLOG_Debug("%s() ----> OAPI_CAS_CI status cache refreshed. Good to go ==============>\n");
	}

}
STATIC void oapi_cas_action_onStatusChanged(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HUINT32					ulSessionId;

	if(s_bCasActionInit != TRUE)
		return;

	if(argc != 2)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}
	else if(HxOBJECT_BIN_SIZE(argv[1]) != sizeof(APKS_CAS_ActionInfo_t))
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid struct size [%d]\n\n", __FUNCTION__, HxOBJECT_BIN_SIZE(argv[0]));
		return;
	}
	else
	{
		ulSessionId = HxOBJECT_INT(argv[0]);

		if(ulSessionId == s_ulSessionId)
		{
			HxLOG_Debug("%s()\n", __FUNCTION__);

			ENTER_CAS_ACTION_CACHE;
			s_stOapiCasActionStatus = *(APKS_CAS_ActionInfo_t *)HxOBJECT_BIN_DATA(argv[1]);
			LEAVE_CAS_ACTION_CACHE;

			if(s_pfCasActionApkNotifier != NULL)
			{
				s_pfCasActionApkNotifier();
			}

		}
		else
		{
			HxLOG_Debug("%s() Discard old session [%d]. (Latest %d)\n", __FUNCTION__, ulSessionId, s_ulSessionId);
		}
	}

}

#define ___GLOBAL_FUNCTIONS___

HERROR OAPI_CAS_ACTION_Init(OxCasAction_Notifier_t fnApkNotifier)
{
#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI)
	HINT32		nReqId;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Debug("%s()\n", __FUNCTION__);

	if (s_bCasActionInit != TRUE)
	{

		if ( VK_SEM_Create(&s_ulOapiCasActionSemId, "OapiCasActionSema", VK_SUSPENDTYPE_PRIORITY) != VK_OK )
		{
			HxLOG_Error("VK_SEM_Create error! \n");
		}
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_ACTION_onStatusChanged, "ib", NULL, (HxRPC_Noti_t)oapi_cas_action_onStatusChanged);
		nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_cas_action_onInited, NULL, RPC_OAPI_CAS_ACTION_Init, NULL);

		s_pfCasActionApkNotifier = fnApkNotifier;


	}
#endif

	return ERR_OK;
}


HERROR OAPI_CAS_ACTION_GetStatus(APKS_CAS_ActionInfo_t *pstStatus)
{
	CHECK_INIT;

	ENTER_CAS_ACTION_CACHE;
	*pstStatus = s_stOapiCasActionStatus;
	LEAVE_CAS_ACTION_CACHE;

	return ERR_OK;

}

