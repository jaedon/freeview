/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xom_cas_ci.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

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
#include <hlib.h>
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <octo_common.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <svc_sys.h>
#include <svc_cas.h>
#include "../local_include/_xom_cas.h"

#include <mgr_cas.h>
#include <xmgr_cas.h>

#include <om_common.h>
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


STATIC HUINT32					s_ulSessionId;
STATIC APKS_CI_CIInfo_t				s_stXomCasCiStatus;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/



#define ___LOCAL_FUNCTIONS___

STATIC HERROR	xom_cas_ci_GetCIInfo_BASE(APKS_CI_CIInfo_t *pstInfo)
{
#if defined (CONFIG_MW_CI)
	HUINT32 			ulIdx;
	HCHAR				*szCamName;
#if defined (CONFIG_MW_CI_PLUS)
	HCHAR				szCiPlusVersion[APKS_CI_PLUS_VER_LENG];
#endif
#endif
	APKS_CI_CIInfo_t	stCIInfo;

	HxSTD_MemSet(&stCIInfo, 0, sizeof(APKS_CI_CIInfo_t));

#if defined (CONFIG_MW_CI)
	stCIInfo.bSupportCI = TRUE;

#if defined (CONFIG_MW_CI_PLUS)
	stCIInfo.bSupportCIPlus = TRUE;
	SVC_SYS_GetCiPlusVersionNumber((HUINT8 *)szCiPlusVersion);
	HLIB_STD_StrUtf8NCpy(stCIInfo.szCIPlusVer, szCiPlusVersion, APKS_CI_PLUS_VER_LENG);
	stCIInfo.szCIPlusVer[APKS_CI_PLUS_VER_LENG - 1] = '\0';
#else /* CONFIG_MW_CI_PLUS */
	stCIInfo.bSupportCIPlus = FALSE;
#endif /* CONFIG_MW_CI_PLUS */

	SVC_CAS_GetNumOfCISlot(&stCIInfo.ulSlotCount);

	if(stCIInfo.ulSlotCount > 0)
	{
		for(ulIdx = 0; ulIdx < stCIInfo.ulSlotCount; ulIdx ++)
		{
			stCIInfo.astCamInfoList[ulIdx].ulSlotNumber = ulIdx;

			SVC_CAS_GetCamState(ulIdx, &stCIInfo.astCamInfoList[ulIdx].eStatus);
			if(stCIInfo.astCamInfoList[ulIdx].eStatus == eDxCASCI_CAM_STATUS_Initialized)
			{
				HUINT8	*pszText = NULL;

				SVC_CAS_GetCamName(ulIdx, (HUINT8 **)&szCamName);
				HLIB_STD_StrUtf8NCpy(stCIInfo.astCamInfoList[ulIdx].szCamName, szCamName, APKS_CAM_NAME_LEN);

				//utf8
				MWC_UTIL_ConvSiStringToUtf8(NULL, SvcCas_UtilStrLen(szCamName), szCamName, &pszText);
				if(pszText != NULL)
				{
					HLIB_STD_StrUtf8NCpy(stCIInfo.astCamInfoList[ulIdx].szCamName, pszText, APKS_CAM_NAME_LEN);
					OxSI_Free(pszText);
				}

				stCIInfo.astCamInfoList[ulIdx].szCamName[APKS_CAM_NAME_LEN - 1] = '\0';

				HxLOG_Print("CamName (%s) \n", stCIInfo.astCamInfoList[ulIdx].szCamName);
			}
			else
			{
				HxSTD_MemSet(stCIInfo.astCamInfoList[ulIdx].szCamName, 0, APKS_CAM_NAME_LEN);
			}
		}
	}
#else /* CONFIG_MW_CI */
	stCIInfo.bSupportCI = FALSE;
	stCIInfo.ulSlotCount = 0;
#endif /* CONFIG_MW_CI */

	HxSTD_MemCopy(pstInfo, &stCIInfo, sizeof(APKS_CI_CIInfo_t));

	return ERR_OK;
}


#define _____OAPI_CALLBACK_FUNCTIONS_____
STATIC void xom_cas_ci_CheckStatusChange_BASE(void)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;
	{
		hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

		if (hErr == ERR_OK)
		{
			HLIB_RPC_Notify(nRpcHandle, RPC_OBAMA_CAS_CI_onStatusChanged, "ib", s_ulSessionId, &s_stXomCasCiStatus, sizeof(APKS_CI_CIInfo_t));
		}
	}

}

// CI Plus related.
STATIC void xom_cas_ci_ProcessAmmiReqMessage_BASE(HINT32 p1, HINT32 p2, HINT32 p3, const HCHAR *pszCallbackName)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;
	CI_AMMI_MSG_TYPE enMsgType = p1;
	HINT32 			ulSize = p2;
	void 				*pvData = (void*)p3;

	HxLOG_Trace();
	// p1에 session id 오지만, p3을 unpack 해도 존재하므로, p1은 일단 무시.

	HxLOG_Assert(ulSize && pvData);

#if 0//def CONFIG_DEBUG
	{
		HxObject_t		*pstObject;
		CI_AMMI_CONTENT		stContent;

		HxLOG_Debug("xom...debug print AppMMI Event... \n");
		pstObject = HLIB_SERIALIZER_Unpack(pvData, ulSize);
		if (pstObject == NULL)
		{
			HxLOG_Error("cannot unpack the data \n");
			return ;
		}
		HxLOG_Debug("enMsgType: %d \n", enMsgType);
		switch(enMsgType)
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
				HxLOG_Error("ERROR : invalid message type (%d) \n", enMsgType);

		}
	}
#endif


	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr == ERR_OK)
	{
		(void)HLIB_RPC_Notify(nRpcHandle, pszCallbackName,
							"ib",
							enMsgType,
							pvData,
							ulSize);
	}

	return ;

}



#define _____OAPI_COMMAND_FUNCTIONS_____
STATIC HERROR xom_cas_ci_InitRpc_BASE(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;


	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

	HxLOG_Debug("\t%s(reqID:0x%x)\n", __FUNCTION__, nRequestId);
	HLIB_RPC_Response(nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", &s_stXomCasCiStatus, sizeof(APKS_CI_CIInfo_t));

	return ERR_OK;
}

// CI Plust related.
STATIC HERROR xom_cas_ci_RequestFileorData_BASE(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT16 					usSessionId = 0;
	CI_AMMI_REQUEST_TYPE 		enReqType = 0;
	HUINT8 					*pucReqName = NULL;
	HULONG 					ulReqNameLen = 0;
	HUINT8					*pszReqName = NULL;

	HxLOG_Debug("\t%s(reqID:0x%x)\n", __FUNCTION__, nRequestId);

	if(argc != 3)
	{
		HxLOG_Error("invalide argc: %d \n", argc);
		return ERR_FAIL;
	}

	usSessionId = HxOBJECT_INT(argv[0]);
	enReqType = HxOBJECT_INT(argv[1]);
	pucReqName = HxOBJECT_BIN_DATA(argv[2]);
	ulReqNameLen = (HULONG)HxOBJECT_BIN_SIZE(argv[2]);

	if(pucReqName == NULL)
	{
		HxLOG_Error("pucReqName is NULL \n");
		return ERR_FAIL;
	}

	// due to strlen, create null-terminated string.
	pszReqName = HLIB_STD_MemCalloc(ulReqNameLen+1);
	if (pszReqName)
	{
		HLIB_STD_MemCpySafe(pszReqName, ulReqNameLen, pucReqName, ulReqNameLen);

		HxLOG_Debug("usSessionId: 0x%x, enReqType: 0x%x, *pucReqName: %s, ulReqNameLen: %d \n", usSessionId, enReqType, pszReqName, ulReqNameLen);

		(void)BUS_PostData(NULL, eOEVT_CAS_CI_REQUESTFILEORDATA, HANDLE_NULL, usSessionId, enReqType, pszReqName, ulReqNameLen+1);

		HLIB_STD_MemFree(pszReqName);
	}
	return ERR_OK;
}

STATIC HERROR xom_cas_ci_RequestAppAbort_BASE(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *pvUserData)
{
	HUINT16							usSessionId = 0;
	CI_AMMI_ABORT_REQUEST_CODE 		enAbortReqCode = 0;


	HxLOG_Debug("\t%s(reqID:0x%x)\n", __FUNCTION__, nRequestId);

	if(argc != 2)
	{
		HxLOG_Error("invalide argc: %d \n", argc);
		return ERR_FAIL;
	}

	usSessionId = HxOBJECT_INT(argv[0]);
	enAbortReqCode = HxOBJECT_INT(argv[1]);

	(void)BUS_PostMessage(NULL, eOEVT_CAS_CI_REQUESTAPPABORT, HANDLE_NULL, usSessionId, enAbortReqCode, 0);

	HxLOG_Debug("usSessionId: 0x%x, enAbortReqCode: 0x%x \n", usSessionId, enAbortReqCode);

	return ERR_OK;
}



#define _____PROCEDURE_____
STATIC BUS_Result_t xmproc_cas_ci_BASE(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t				result = MESSAGE_PASS;


	switch(message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Info("%s created\n", (HINT8*) __FUNCTION__);
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			break;

		case eMEVT_SYSCTL_CI_CAM_STATUS:
		case eMEVT_SYSCTL_CI_CAM_STATUS_EXTRACTED:
		case eMEVT_SYSCTL_CI_CAM_STATUS_INSERTED:
		case eMEVT_SYSCTL_CI_CAM_STATUS_INITIALIZED:
			result = MESSAGE_BREAK;
			HxLOG_Info("receive eMEVT_SYSCTL_CI_CAM_STATUS\n");
			xom_cas_ci_GetCIInfo_BASE(&s_stXomCasCiStatus);
			xom_cas_ci_CheckStatusChange_BASE();
			break;

		case eMEVT_XMGR_CAS_CI_AMMI_REQ_START: // using serializer.
			HxLOG_Debug("eMEVT_XMGR_CAS_CI_AMMI_REQ_START \n");
			xom_cas_ci_ProcessAmmiReqMessage_BASE(p1,p2,p3, RPC_OBAMA_CAS_CI_onCiAmmiMsgReqStart);
			break;

		case eMEVT_XMGR_CAS_CI_AMMI_FILE_ACK: // using serializer.
			HxLOG_Debug("eMEVT_XMGR_CAS_CI_AMMI_FILE_ACK \n");
			xom_cas_ci_ProcessAmmiReqMessage_BASE(p1,p2,p3, RPC_OBAMA_CAS_CI_onCiAmmiMsgFileAck);
			break;

		case eMEVT_XMGR_CAS_CI_AMMI_REQ_APP_ABORT: // using serializer.
			HxLOG_Debug("eMEVT_XMGR_CAS_CI_AMMI_REQ_APP_ABORT \n");
			xom_cas_ci_ProcessAmmiReqMessage_BASE(p1,p2,p3, RPC_OBAMA_CAS_CI_onCiAmmiMsgReqAppAbort);
			break;

		case eMEVT_XMGR_CAS_CI_AMMI_APP_ABORT_ACK: // using serializer.
			HxLOG_Debug("eMEVT_XMGR_CAS_CI_AMMI_APP_ABORT_ACK \n");
			xom_cas_ci_ProcessAmmiReqMessage_BASE(p1,p2,p3, RPC_OBAMA_CAS_CI_onCiAmmiMsgAppAbortAck);
			break;

		default:
			result = MESSAGE_PASS;
			break;


	}

	return result;
}

#define ___GLOBAL_FUNCTIONS___
HERROR xom_cas_ci_Init_BASE(void)
{
	HERROR			err;
	HINT32			nRpcHandle = 0;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	// Initialize OAPI cache data
	xom_cas_ci_GetCIInfo_BASE(&s_stXomCasCiStatus);

	// Create event handler proc
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, xmproc_cas_ci_BASE, 0, 0, 0, 0);

	err = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(err == ERR_OK)
	{

		//	-------   RPC Methods   -------	//
		err = HLIB_RPC_RegisterRequest (nRpcHandle, RPC_OAPI_CAS_CI_Init, NULL, (HxRPC_Func_t)xom_cas_ci_InitRpc_BASE, NULL,
								"Activate the cas.ci module: (void)");
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterRequest (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_CAS_CI_Init, err);
		}

			// CI Plus related.
		err = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_CAS_CI_RequestFileorData, "iib", (HxRPC_Func_t)xom_cas_ci_RequestFileorData_BASE, NULL,
								"Request File Or Data (CI Plus related.) : (HUINT16 usSessionId, CI_AMMI_REQUEST_TYPE enReqType, HUINT8 *pucReqName, HULONG ulReqNameLen)");
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_CAS_CI_RequestFileorData, err);
		}

		err = HLIB_RPC_RegisterCall (nRpcHandle, RPC_OAPI_CAS_CI_RequestAppAbort, "ii", (HxRPC_Func_t)xom_cas_ci_RequestAppAbort_BASE, NULL,
								"Request File Or Data (CI Plus related.) : (HUINT16 usSessionId, CI_AMMI_ABORT_REQUEST_CODE enAbortReqCode)");
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterMethod (nRpcHandle, %s) returned error(%d)\n", RPC_OAPI_CAS_CI_RequestAppAbort, err);
		}


		//	-------   Notifier	Callbakcs------- //
		err = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_CI_onStatusChanged, "ib",
								"CI status changed\n"
								"\t\n"
								);
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_CI_onStatusChanged, err);
		}

			// CI Plus related.
		err = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_CI_onCiAmmiMsgReqStart, "ib",
								"RPC_OBAMA_CAS_CI_onCiAmmiMsgReqStart\n"
								"\t\n"
								);
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_CI_onCiAmmiMsgReqStart, err);
		}


		err = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_CI_onCiAmmiMsgFileAck, "ib",
							"RPC_OBAMA_CAS_CI_onCiAmmiMsgFileAck\n"
							"\t\n"
							);
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_CI_onCiAmmiMsgFileAck, err);
		}


		err = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_CI_onCiAmmiMsgReqAppAbort, "ib",
							"RPC_OBAMA_CAS_CI_onCiAmmiMsgReqAppAbort\n"
							"\t\n"
							);
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_CI_onCiAmmiMsgReqAppAbort, err);
		}


		err = HLIB_RPC_CreateNotification (nRpcHandle, RPC_OBAMA_CAS_CI_onCiAmmiMsgAppAbortAck, "ib",
							"RPC_OBAMA_CAS_CI_onCiAmmiMsgAppAbortAck\n"
							"\t\n"
							);
		if(err != ERR_OK)
		{
			HxLOG_Error("\tHLIB_RPC_RegisterCallbackProtoType (nRpcHandle, %s) returned error(%d)\n", RPC_OBAMA_CAS_CI_onCiAmmiMsgAppAbortAck, err);
		}



	}

	return ERR_OK;

}



