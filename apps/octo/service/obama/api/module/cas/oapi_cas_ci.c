/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_cas_ci.c
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
STATIC HULONG					s_ulOapiCasCiSemId;
STATIC APKS_CI_CIInfo_t				s_stOapiCasCiStatus;

#define	ENTER_CAS_CI_CACHE	VK_SEM_Get(s_ulOapiCasCiSemId)
#define	LEAVE_CAS_CI_CACHE	VK_SEM_Release(s_ulOapiCasCiSemId)

#define CHECK_INIT	{if(s_bCasCiInit != TRUE) return ERR_FAIL;}

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL					s_bCasCiInit;
#if defined(CONFIG_MW_CI)
STATIC OxCasCi_Notifier_t	s_pfCasCiApkNotifier;
STATIC HUINT32				s_ulSessionId;
STATIC OxCasCi_AppMmiNotifier_t	s_pfnCasCiApkAppMmiNitifier;
#endif

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___


#define _____NOTIFICATION_CALLBACKS_____

#if defined(CONFIG_MW_CI)

STATIC void oapi_cas_ci_onInited(HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{

	if(argc != 1)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}
	else if(HxOBJECT_BIN_SIZE(argv[0]) != sizeof(APKS_CI_CIInfo_t))
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid struct size [%d]\n\n", __FUNCTION__, HxOBJECT_BIN_SIZE(argv[0]));
		return;
	}
	else
	{
		s_stOapiCasCiStatus = *(APKS_CI_CIInfo_t *)HxOBJECT_BIN_DATA(argv[0]);
		s_bCasCiInit = TRUE;

		HxLOG_Debug("%s() ----> OAPI_CAS_CI status cache refreshed. Good to go ==============>\n");
	}

}

STATIC void oapi_cas_ci_onStatusChanged(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HUINT32					ulSessionId;

	if(s_bCasCiInit != TRUE)
		return;

	if(argc != 2)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}
	else if(HxOBJECT_BIN_SIZE(argv[1]) != sizeof(APKS_CI_CIInfo_t))
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

			ENTER_CAS_CI_CACHE;
			s_stOapiCasCiStatus = *(APKS_CI_CIInfo_t *)HxOBJECT_BIN_DATA(argv[1]);
			LEAVE_CAS_CI_CACHE;

			if(s_pfCasCiApkNotifier != NULL)
			{
				s_pfCasCiApkNotifier();
			}

		}
		else
		{
			HxLOG_Debug("%s() Discard old session [%d]. (Latest %d)\n", __FUNCTION__, ulSessionId, s_ulSessionId);
		}
	}

}



#if defined(CONFIG_MW_CI_PLUS)
STATIC void oapi_cas_ci_onAppMmiMessage(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	CI_AMMI_MSG_TYPE		eAmmiMsgType;
	HINT32				ulBinSize;
	void*				pvUserData;

	if(s_bCasCiInit != TRUE)
		return;

	if(argc != 2)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		return;
	}

	eAmmiMsgType = HxOBJECT_INT(argv[0]);

	ulBinSize = HxOBJECT_BIN_SIZE(argv[1]);
	pvUserData = HxOBJECT_BIN_DATA(argv[1]);


#if 0//def CONFIG_DEBUG
	{
		HxObject_t		*pstObject;
		CI_AMMI_CONTENT 	stContent;

		HxLOG_Debug("oapi... debug print AppMMI Event... \n");
		pstObject = HLIB_SERIALIZER_Unpack(pvUserData, ulBinSize);
		if (pstObject == NULL)
		{
			HxLOG_Error("cannot unpack the data \n");
			return ;
		}
		HxLOG_Debug("enMsgType: %d \n", eAmmiMsgType);
		switch(eAmmiMsgType)
		{
			case CI_AMMI_MSG_REQ_START:
				if (HxOBJECT_ARR_LEN(pstObject) != 3)
				{
					HxLOG_Error("invalid object arr len. \n");
					return ;
				}
				stContent.typeReqStart.usSessionId = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;
				stContent.typeReqStart.ucAppDomainIdLen = HxOBJECT_ARR_VAL(pstObject, 1)->u.bin.size;
				stContent.typeReqStart.pucAppDomainId = HxOBJECT_ARR_VAL(pstObject, 1)->u.bin.data;
				stContent.typeReqStart.ucInitialObjectLen = HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.size;
				stContent.typeReqStart.pucInitialObject = HxOBJECT_ARR_VAL(pstObject, 2)->u.bin.data;

				HxLOG_Debug("usSessionId: %d \n", stContent.typeReqStart.usSessionId);

				break;

			case CI_AMMI_MSG_FILE_ACK:
				if (HxOBJECT_ARR_LEN(pstObject) != 4)
				{
					HxLOG_Error("invalid object arr len. \n");
				}
				stContent.typeFileAck.usSessionId = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;
				stContent.typeFileAck.ucFileOK = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;
				stContent.typeFileAck.enReqType = HxOBJECT_ARR_VAL(pstObject, 2)->u.num;
				stContent.typeFileAck.ulBytesLen = HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.size;
				stContent.typeFileAck.pucBytes = HxOBJECT_ARR_VAL(pstObject, 3)->u.bin.data;

				HxLOG_Debug("usSessionId: %d \n", stContent.typeFileAck.usSessionId);

				break;

			case CI_AMMI_MSG_REQ_APP_ABORT:
				if (HxOBJECT_ARR_LEN(pstObject) != 2)
				{
					HxLOG_Error("invalid object arr len. \n");
				}
				stContent.typeReqAppAbort.usSessionId = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;
				stContent.typeReqAppAbort.enAbortReqCode = HxOBJECT_ARR_VAL(pstObject, 1)->u.num;

				HxLOG_Debug("usSessionId: %d \n", stContent.typeReqAppAbort.usSessionId);

				break;

			case CI_AMMI_MSG_APP_ABORT_ACK:
				if (HxOBJECT_ARR_LEN(pstObject) != 1)
				{
					HxLOG_Error("invalid object arr len. \n");
				}
				stContent.typeAppAbortAck.usSessionId = HxOBJECT_ARR_VAL(pstObject, 0)->u.num;

				HxLOG_Debug("usSessionId: %d \n", stContent.typeAppAbortAck.usSessionId);

				break;

			default:
				HxLOG_Error("ERROR : invalid message type (%d) \n", eAmmiMsgType);

		}
	}
#endif



	if(s_pfnCasCiApkAppMmiNitifier != NULL)
	{
		s_pfnCasCiApkAppMmiNitifier(eAmmiMsgType, pvUserData, ulBinSize);
	}
	return;
}
#endif

#endif

#define ___GLOBAL_FUNCTIONS___

HERROR OAPI_CAS_CI_Init(OxCasCi_Notifier_t fnApkNotifier, OxCasCi_AppMmiNotifier_t fnAppMmiApkNotifier)
{
#if defined(CONFIG_MW_CI)
	HINT32		nReqId;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Debug("%s()\n", __FUNCTION__);

	if (s_bCasCiInit != TRUE)
	{

		if ( VK_SEM_Create(&s_ulOapiCasCiSemId, "OapiCasCiSema", VK_SUSPENDTYPE_PRIORITY) != VK_OK )
		{
			HxLOG_Error("VK_SEM_Create error! \n");
		}

		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_CI_onStatusChanged, "ib", NULL, (HxRPC_Noti_t)oapi_cas_ci_onStatusChanged);

		nReqId = HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_cas_ci_onInited, NULL, RPC_OAPI_CAS_CI_Init, NULL);

		// CI Plus related
#if defined(CONFIG_MW_CI_PLUS)
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_CI_onCiAmmiMsgReqStart, "ib", NULL, (HxRPC_Noti_t)oapi_cas_ci_onAppMmiMessage);
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_CI_onCiAmmiMsgFileAck, "ib", NULL, (HxRPC_Noti_t)oapi_cas_ci_onAppMmiMessage);
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_CI_onCiAmmiMsgReqAppAbort, "ib", NULL, (HxRPC_Noti_t)oapi_cas_ci_onAppMmiMessage);
		HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_CI_onCiAmmiMsgAppAbortAck, "ib", NULL, (HxRPC_Noti_t)oapi_cas_ci_onAppMmiMessage);

#endif

		s_pfCasCiApkNotifier = fnApkNotifier;
		s_pfnCasCiApkAppMmiNitifier = fnAppMmiApkNotifier;


	}
#endif

	return ERR_OK;
}


HERROR OAPI_CAS_CI_GetStatus(APKS_CI_CIInfo_t *pstStatus)
{
	CHECK_INIT;

	ENTER_CAS_CI_CACHE;
	*pstStatus = s_stOapiCasCiStatus;
	LEAVE_CAS_CI_CACHE;

	return ERR_OK;

}

// CI Plust related.

HERROR OAPI_CAS_CI_RequestFileorData (HUINT16 usSessionId, CI_AMMI_REQUEST_TYPE enReqType, HUINT8 *pucReqName, HULONG ulReqNameLen)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	CHECK_INIT;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Debug("%s()\n", __FUNCTION__);

	if(hErr == ERR_OK)
	{
		(void)HLIB_RPC_Call (nRpcHandle, RPC_OAPI_CAS_CI_RequestFileorData, "iib", usSessionId, enReqType, pucReqName, ulReqNameLen);
	}

	return ERR_OK;



}
HERROR OAPI_CAS_CI_RequestAppAbort (HUINT16 usSessionId, CI_AMMI_ABORT_REQUEST_CODE enAbortReqCode)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	CHECK_INIT;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Debug("%s()\n", __FUNCTION__);

	if(hErr == ERR_OK)
	{
		(void)HLIB_RPC_Call (nRpcHandle, RPC_OAPI_CAS_CI_RequestAppAbort, "ii", usSessionId, enAbortReqCode);
	}

	return ERR_OK;



}

