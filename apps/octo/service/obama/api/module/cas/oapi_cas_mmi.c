/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_cas_mmi.c
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


typedef struct
{
	OxCasMmi_Notifier_t				pfnNotifier;
} oapiCasMmi_SessionNotifierInfo_t;

typedef struct
{
	DxCAS_MmiSessionType_e			eSessionType;
	OxCasMmiSessionStatus_e			eStatus;
	DxCAS_GroupId_e				eModule;
	HUINT32							nSlot;
} oapiCasMmi_ConnInfo_t;

typedef struct
{
	DxCAS_MmiSessionType_e		eSessionType;
	Handle_t						hUiHandle;
	HxList_t						*pstConnList;
	HxList_t						*pstUiEventList;
} oapiCasMmi_SessionInfo_t;

typedef struct
{
	OxCasMmi_Notifier_t				pfnNotifier;
	HxList_t							*pstSessionInfoList;
	HxList_t							*pstEventList;
} oapiCasMmi_SessionControl_t;

typedef struct
{
	DxCAS_MMI_SessionEventType_e		eventType;
	Handle_t							hSession;
	HUINT32							ulUiHandle;
	DxCAS_MmiSessionType_e			sessionType;
	HUINT32							accesscode;
	DxCAS_GroupId_e				eModule;
	HUINT32							nSlot;
} oapiCasMmi_SessionEventInfo_t;

typedef struct
{
	DxCAS_MmiSessionType_e				sessionType;
	DxCAS_MMI_SessionEventType_e			eventType;
	HCHAR								*pzEventData;
} oapiCasMmi_UiDisplayEventInfo_t;





/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC HULONG					s_ulOapiCasMmiSemId;
STATIC HBOOL						s_bCasMmiInit;
STATIC	oapiCasMmi_SessionControl_t		_stSessionControlHandle;
STATIC	oapiCasMmi_SessionNotifierInfo_t	_stSessionNotifierInfo[eDxCAS_SESSIONTYPE_MAX];
// async noti 를 위해서.
STATIC OxCasMmi_NotifierForAppkit_t	s_fnCasMmiApkNotifier;


#define	ENTER_OAPI_CAS_MMI_CACHE	VK_SEM_Get(s_ulOapiCasMmiSemId)
#define	LEAVE_OAPI_CAS_MMI_CACHE	VK_SEM_Release(s_ulOapiCasMmiSemId)

#define CHECK_INIT	{if(s_bCasMmiInit != TRUE) return;}




/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/

#define ___PRIVATE_FUNCTION___
STATIC void oapi_cas_mmi_CbFreeEventlist (void *pvData);
STATIC void oapi_cas_mmi_PostRequestSessionStart(Handle_t hSession, HUINT32 ulUiHandle, DxCAS_GroupId_e eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSessionType, const char *pcParamData);
STATIC void oapi_cas_mmi_PostRequestSessionStop(Handle_t hSession, HUINT32 ulUiHandle,DxCAS_GroupId_e eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSessionType);
STATIC void oapi_cas_mmi_PostSessionStatus(Handle_t hSession, DxCAS_GroupId_e eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSessionType,  OxCasMmiSessionStatus_e eSessionStatus);
STATIC void oapi_cas_mmi_PostUserInput(Handle_t hSession, DxCAS_GroupId_e eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSessionType, const HCHAR *pzInputData);

STATIC void oapi_cas_mmi_GetConnBySessionTypeAndHandle(HxList_t *pstSessionInfoList,
	oapiCasMmi_SessionInfo_t **ppstSessionInfo, oapiCasMmi_ConnInfo_t **ppstConn, DxCAS_MmiSessionType_e eSessionType, Handle_t hSession);
STATIC oapiCasMmi_SessionInfo_t	*oapi_cas_mmi_GetSessionBySessionTypeAndHandle(HxList_t *pstSessionInfoList, DxCAS_MmiSessionType_e eSession, Handle_t hSession);

STATIC oapiCasMmi_ConnInfo_t *oapi_cas_mmi_GetConn(HxList_t *pstConnList, DxCAS_GroupId_e	eModule, HUINT32 nSlot);
STATIC HERROR oapi_cas_mmi_AppendConnInfo(HxList_t **ppstConnList,
	DxCAS_GroupId_e	eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSessionType, oapiCasMmi_ConnInfo_t **ppstConnInfo);
STATIC HERROR oapi_cas_mmi_InitModule(void);
STATIC void oapi_cas_mmi_onSessionStatusChanged(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata);
STATIC void oapi_cas_mmi_onGetEventData(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata);

#if defined(CONFIG_DEBUG)
STATIC const HINT8 * oapi_cas_mmi_MakeStrEventType(DxCAS_MMI_SessionEventType_e eEvtType);
STATIC const HINT8 * oapi_cas_mmi_MakeStrSessionType(DxCAS_MmiSessionType_e eSessionType);
STATIC const HINT8 * oapi_cas_mmi_MakeCasType(DxCAS_GroupId_e eCasType);

#endif




STATIC void oapi_cas_mmi_CbFreeEventlist (void *pvData)
{
	DxCAS_MMI_Event_t		*pstEventData = (DxCAS_MMI_Event_t *)pvData;


	if (pstEventData != NULL)
	{
		if(pstEventData->pszEncodedUiEvent != NULL)
		{
			OxMEM_Free((void *)pstEventData->pszEncodedUiEvent);
			pstEventData->pszEncodedUiEvent = NULL;
		}
		OxMEM_Free((void *)pstEventData);
		pstEventData = NULL;
	}

	return;
}

INLINE oapiCasMmi_SessionControl_t	*oapi_cas_mmi_GetSessionControlHandle(void)
{
	return &_stSessionControlHandle;
}

#if defined(CONFIG_DEBUG)
STATIC const HINT8 * oapi_cas_mmi_MakeCasType(DxCAS_GroupId_e eCasType)
{
	switch (eCasType)
	{
		ENUM_TO_STR(eDxCAS_GROUPID_NONE);
		ENUM_TO_STR(eDxCAS_GROUPID_CAM);
		ENUM_TO_STR(eDxCAS_GROUPID_NA);
		ENUM_TO_STR(eDxCAS_GROUPID_IR);
		ENUM_TO_STR(eDxCAS_GROUPID_CX);
		ENUM_TO_STR(eDxCAS_GROUPID_VA);
		ENUM_TO_STR(eDxCAS_GROUPID_ND);
		ENUM_TO_STR(eDxCAS_GROUPID_MG);
		ENUM_TO_STR(eDxCAS_GROUPID_SATSA);
		ENUM_TO_STR(eDxCAS_GROUPID_VX);
		default:
			break;
	}
	return ("Unknown");
}

STATIC const HINT8 * oapi_cas_mmi_MakeStrEventType(DxCAS_MMI_SessionEventType_e eEvtType)
{
	switch (eEvtType)
	{
		ENUM_TO_STR(eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTART);
		ENUM_TO_STR(eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTOP);
		ENUM_TO_STR(eDxCASMMI_EVENTTYPE_SESSIONSTARTED);
		ENUM_TO_STR(eDxCASMMI_EVENTTYPE_SESSIONSTOPPED);
		ENUM_TO_STR(eDxCASMMI_EVENTTYPE_DISPLAYUI);
		ENUM_TO_STR(eDxCASMMI_EVENTTYPE_NONE);
		default:
			break;
	}
	return ("Unknown");
}

STATIC const HINT8 * oapi_cas_mmi_MakeStrSessionType(DxCAS_MmiSessionType_e eSessionType)
{
	switch (eSessionType)
	{
		ENUM_TO_STR(eDxCAS_SESSIONTYPE_NONE);
		ENUM_TO_STR(eDxCAS_SESSIONTYPE_BANNER);
		ENUM_TO_STR(eDxCAS_SESSIONTYPE_MENU);
		ENUM_TO_STR(eDxCAS_SESSIONTYPE_EXTENSION);
		ENUM_TO_STR(eDxCAS_SESSIONTYPE_MAX);
		default:
			break;
	}
	return ("Unknown");
}

#endif /* defined(CONFIG_DEBUG) */


STATIC HERROR oapi_cas_mmi_InitModule(void)
{
	HERROR						hError = ERR_OK;
	oapiCasMmi_SessionControl_t 	*pstControlHandle = NULL;

	HxLOG_Trace();

	pstControlHandle = oapi_cas_mmi_GetSessionControlHandle();
	if(pstControlHandle != NULL)
	{
		HxSTD_MemSet(pstControlHandle, 0x00, sizeof(oapiCasMmi_SessionControl_t));
	}
	return hError;
}


STATIC oapiCasMmi_SessionInfo_t	*oapi_cas_mmi_GetSessionBySessionTypeAndHandle(HxList_t *pstSessionInfoList, DxCAS_MmiSessionType_e eSession, Handle_t hSession)
{
	HxList_t				*pstSessionItem = NULL;
	oapiCasMmi_SessionInfo_t	*pstSessionInfo = NULL;

	pstSessionItem = pstSessionInfoList;

	while(pstSessionItem != NULL)
	{
		pstSessionInfo = (oapiCasMmi_SessionInfo_t *)HLIB_LIST_Data(pstSessionItem);
		if(pstSessionInfo != NULL)
		{
			if(pstSessionInfo->eSessionType == eSession && (Handle_t)pstSessionInfo == hSession)
			{
				break;
			}
		}

		pstSessionItem = pstSessionItem->next;
		pstSessionInfo = NULL;
	}

	return pstSessionInfo;
}

STATIC oapiCasMmi_ConnInfo_t *oapi_cas_mmi_GetConn(HxList_t *pstConnList, DxCAS_GroupId_e	eModule, HUINT32 nSlot)
{
	HxList_t				*pstConnItem = NULL;
	oapiCasMmi_ConnInfo_t		*pstConnInfo = NULL;

	pstConnItem = pstConnList;
	while(pstConnItem != NULL)
	{
		pstConnInfo = (oapiCasMmi_ConnInfo_t *)HLIB_LIST_Data(pstConnItem);
		if(pstConnInfo != NULL)
		{
			if((pstConnInfo->eModule == eModule) &&
				(pstConnInfo->nSlot == nSlot))
			{
				break;
			}
		}
		pstConnItem = pstConnItem->next;
		pstConnInfo = NULL;
	}
	return pstConnInfo;
}






STATIC void oapi_cas_mmi_PostUserInput(Handle_t hSession, DxCAS_GroupId_e eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSessionType, const HCHAR *pzInputData)
{

	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s:%04d] CAS Type [%s], Session Type [%s]\n", __FUNCTION__, __LINE__,
			oapi_cas_mmi_MakeCasType(eModule),
			oapi_cas_mmi_MakeStrSessionType(eSessionType));
#endif /* defined(CONFIG_DEBUG) */

	if(pzInputData == NULL)
	{
		HxLOG_Error("\n");
		return;
	}

	HxLOG_Info("pzInputData: %s \n", pzInputData);
	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_CAS_MMI_UiResponse, "iiibi", eModule, nSlot, eSessionType, pzInputData, HxSTD_StrLen(pzInputData), hSession);




	return;
}



STATIC void oapi_cas_mmi_PostRequestSessionStop(Handle_t hSession, HUINT32 ulUiHandle, DxCAS_GroupId_e eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSessionType)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s:%04d] CAS Type [%s], Session Type [%s]\n", __FUNCTION__, __LINE__,
			oapi_cas_mmi_MakeCasType(eModule),
			oapi_cas_mmi_MakeStrSessionType(eSessionType));
#endif /* defined(CONFIG_DEBUG) */
	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_CAS_MMI_CloseSession, "iiiii", hSession, ulUiHandle, eModule, nSlot, eSessionType);

	return;
}

STATIC void oapi_cas_mmi_PostSessionStatus(Handle_t hSession, DxCAS_GroupId_e eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSessionType,  OxCasMmiSessionStatus_e eSessionStatus)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s:%04d] CAS Type [%s], Session Type [%s]\n", __FUNCTION__, __LINE__,
			oapi_cas_mmi_MakeCasType(eModule),
			oapi_cas_mmi_MakeStrSessionType(eSessionType));
#endif /* defined(CONFIG_DEBUG) */

	HLIB_RPC_Call (nRpcHandle, RPC_OAPI_CAS_MMI_ChangeSessionState, "iiiii", eModule, nSlot, eSessionType, eSessionStatus, hSession);



	return ;

}


#define MAX_OPEN_SESSION_EXTEDED_PARAM_SIZE	256
STATIC void oapi_cas_mmi_PostRequestSessionStart(Handle_t hSession, HUINT32 ulUiHandle, DxCAS_GroupId_e eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSessionType, const char *pcParamData)
{
	HCHAR						*pzMenuGroup = NULL;
	HxJSON_t					pJsonRoot = NULL;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();

	// web ap 에서 json type의 param 을 {"param:param1"} 형태로 내려주는 경우가 존재.
	if(pcParamData != NULL && HxSTD_StrCmp(pcParamData, "undefined") && HxSTD_StrCmp(pcParamData, "null"))
	{
		HxLOG_Info("pcParamData: %s \n", pcParamData);

		if (HxSTD_StrCmp(pcParamData, "null") == 0)
		{
			HxLOG_Info("pcParamData is 'NULL'... Do Nothing...\n");
		}
		else
		{
			pJsonRoot = HLIB_JSON_Decode(pcParamData);
			if(pJsonRoot != NULL)
			{
				pzMenuGroup = (HCHAR *)HLIB_JSON_Object_GetString(pJsonRoot, (const HCHAR *)"menugroup");
				HxLOG_Info("pzMenuGroup: %s \n", pzMenuGroup);
			}
		}

	}


#if defined(CONFIG_DEBUG)
		HxLOG_Print("[%s:%04d] CAS Type [%s], Session Type [%s] Slot [%d]...\n", __FUNCTION__, __LINE__,
				oapi_cas_mmi_MakeCasType(eModule),
				oapi_cas_mmi_MakeStrSessionType(eSessionType),
				nSlot);
#endif /* defined(CONFIG_DEBUG) */


	if(pzMenuGroup != NULL)
	{
		HLIB_RPC_Call (nRpcHandle, RPC_OAPI_CAS_MMI_OpenSession, "iiiiib", hSession, ulUiHandle, eModule, nSlot, eSessionType, pzMenuGroup, strlen(pzMenuGroup));
	}
	else
	{
		HLIB_RPC_Call (nRpcHandle, RPC_OAPI_CAS_MMI_OpenSession, "iiiiib", hSession, ulUiHandle, eModule, nSlot, eSessionType, "NULL", 4);
	}

	if(pJsonRoot != NULL)
	{
		HLIB_JSON_Delete(pJsonRoot);
		pJsonRoot = NULL;
	}



	return;
}


STATIC HERROR oapi_cas_mmi_AppendConnInfo(HxList_t **ppstConnList,
	DxCAS_GroupId_e	eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSessionType,oapiCasMmi_ConnInfo_t **ppstConnInfo)
{
	HERROR					hError = ERR_OK;
	oapiCasMmi_ConnInfo_t		*pstConnInfo = NULL;

	pstConnInfo = (oapiCasMmi_ConnInfo_t *)OxMEM_Malloc(sizeof(oapiCasMmi_ConnInfo_t));
	if(pstConnInfo == NULL)
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	HxSTD_MemSet(pstConnInfo, 0x00, sizeof(oapiCasMmi_ConnInfo_t));

	pstConnInfo->eStatus = eOxCASMMI_SESSIONSTATUS_STOPPED;
	pstConnInfo->eModule = eModule;
	pstConnInfo->nSlot = nSlot;
	pstConnInfo->eSessionType = eSessionType;

	(*ppstConnList) = HLIB_LIST_Append((*ppstConnList), pstConnInfo);
	(*ppstConnInfo) = pstConnInfo;

END_FUNC:
	return hError;
}

STATIC void oapi_cas_mmi_GetConnBySessionTypeAndHandle(HxList_t *pstSessionInfoList,
	oapiCasMmi_SessionInfo_t **ppstSessionInfo, oapiCasMmi_ConnInfo_t **ppstConn, DxCAS_MmiSessionType_e eSessionType, Handle_t hSession)
{
	HBOOL					bContinue = TRUE;
	HxList_t				*pstConnItem = NULL;
	HxList_t				*pstSessionItem = NULL;
	oapiCasMmi_ConnInfo_t		*pstConn = NULL;
	oapiCasMmi_SessionInfo_t	*pstSessionInfo = NULL;

	(*ppstSessionInfo) = NULL;
	(*ppstConn) = NULL;

	pstSessionItem = pstSessionInfoList;
	while(pstSessionItem != NULL)
	{
		pstSessionInfo = (oapiCasMmi_SessionInfo_t *)HLIB_LIST_Data(pstSessionItem);
		if(pstSessionInfo != NULL && (Handle_t)pstSessionInfo == hSession)
		{
			pstConnItem = pstSessionInfo->pstConnList;
			while(pstConnItem != NULL)
			{
				pstConn = (oapiCasMmi_ConnInfo_t *)HLIB_LIST_Data(pstConnItem);
				if(pstConn != NULL)
				{
					if(pstConn->eSessionType == eSessionType)
					{
						(*ppstSessionInfo) = pstSessionInfo;
						(*ppstConn) = pstConn;
						bContinue = FALSE;
						break;
					}
				}
				pstConnItem = pstConnItem->next;
				pstConn = NULL;
			}

			if(bContinue == FALSE)
			{
				break;
			}
		}

		pstSessionItem = pstSessionItem->next;
		pstSessionInfo = NULL;
	}
	return;
}







#define ___EVENT_LISTENER_FUNCTIONS_______________________________________________________________________


STATIC void oapi_cas_mmi_onGetEventData(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	oapiCasMmi_SessionInfo_t 		*pstSessionInfo = NULL;
	DxCAS_MMI_Event_t				*pstEvent = NULL;
	HERROR						hError = ERR_OK;
	oapiCasMmi_SessionControl_t 	*pstControlHandle = NULL;
	HCHAR				*decodeBuffer = NULL;
	HCHAR				*endcodedBuffer = NULL;
	HUINT32 				ulStreamSize = 0;
	DxCas_UIAttribute_e			ulAttr = 0;

	HxLOG_Trace();

	if(argc != 7)
	{
		HxLOG_Error("==========> ERROR !!!! %s() has invalid argc[%d]\n\n", __FUNCTION__, argc);
		goto END_FUNC;
	}

	else
	{
		pstEvent = (DxCAS_MMI_Event_t *)OxMEM_Calloc(sizeof(DxCAS_MMI_Event_t));
		if(pstEvent == NULL)
		{
			HxLOG_Error("\n");
			hError = ERR_FAIL;
			goto END_FUNC;
		}
		pstEvent->hSession = (Handle_t)HxOBJECT_INT(argv[0]);
		HxLOG_Debug("hSession: 0x%x \n", pstEvent->hSession);
		pstEvent->eventType = HxOBJECT_INT(argv[1]);
		pstEvent->eModule = HxOBJECT_INT(argv[2]);
		pstEvent->nSlot = HxOBJECT_INT(argv[3]);
		pstEvent->sessionType = HxOBJECT_INT(argv[4]);
		ulAttr = HxOBJECT_INT(argv[5]);
		endcodedBuffer = HxOBJECT_BIN_DATA(argv[6]);
		ulStreamSize = HxOBJECT_BIN_SIZE(argv[6]);
		if(endcodedBuffer != NULL)
		{
			decodeBuffer = OxMEM_Calloc(ulStreamSize+1);
			if(decodeBuffer != NULL)
			{
				HLIB_MATH_DecodeBase64(decodeBuffer, endcodedBuffer, ulStreamSize);
				pstEvent->pszEncodedUiEvent = decodeBuffer;
			}
		}



		HxLOG_Info("pstEvent->pszEncodedUiEvent: %s \n", pstEvent->pszEncodedUiEvent);


		pstControlHandle = oapi_cas_mmi_GetSessionControlHandle();
		HxLOG_Trace();
		if(pstControlHandle == NULL)
		{
			HxLOG_Error("\n");
			hError = ERR_FAIL;
			OxMEM_Free(pstEvent);
			goto END_FUNC;
		}
		HxLOG_Trace();
		pstSessionInfo = oapi_cas_mmi_GetSessionBySessionTypeAndHandle(pstControlHandle->pstSessionInfoList, pstEvent->sessionType, pstEvent->hSession);

		if(pstSessionInfo != NULL)
		{
			ENTER_OAPI_CAS_MMI_CACHE;
			pstSessionInfo->pstUiEventList = HLIB_LIST_Append(pstSessionInfo->pstUiEventList, (void *)pstEvent);
			LEAVE_OAPI_CAS_MMI_CACHE;
			HxLOG_Trace();
			if(_stSessionNotifierInfo[pstEvent->sessionType].pfnNotifier!= NULL)
			{
				HxLOG_Trace();
				//LEAVE_OAPI_CAS_MMI_CACHE;
				if(s_fnCasMmiApkNotifier != NULL)
				{
					s_fnCasMmiApkNotifier((_stSessionNotifierInfo[pstEvent->sessionType].pfnNotifier), eDxCASMMI_EVENTTYPE_DISPLAYUI, (HUINT32)pstSessionInfo->eSessionType, (HUINT32)pstSessionInfo);
#if defined(CONFIG_CAS_CONAX_FINGERPRINT)
					if (ulAttr & eDxCasUiAttr_FingerPrint)
					{
						HAPI_BroadcastSignal("signal:onFingerprintState", 0, "s", pstEvent->pszEncodedUiEvent);
					}
#endif
				}
			}
				//ENTER_OAPI_CAS_MMI_CACHE;
		}
		else
		{
			OxMEM_Free(pstEvent);
		}

		HxLOG_Trace();


	}

END_FUNC:

//	do not free below --> it will be freed in GetUiEvent
//	if (pstEvent)
//		OxMEM_Free(pstEvent);


	return ;

}


STATIC void oapi_cas_mmi_onSessionStatusChanged(HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HERROR						hError = ERR_OK;
	HERROR 						eResult = ERR_OK;
	oapiCasMmi_SessionControl_t		*pstControlHandle = NULL;
	oapiCasMmi_SessionInfo_t 	*pstSession = NULL;
	oapiCasMmi_SessionEventInfo_t	stSessionEventInfo;
	oapiCasMmi_ConnInfo_t			*pstConn = NULL;

	HxLOG_Trace();


	HxSTD_MemSet(&stSessionEventInfo, 0x00, sizeof(oapiCasMmi_SessionEventInfo_t));
	stSessionEventInfo.hSession = HxOBJECT_INT(argv[0]);
	stSessionEventInfo.ulUiHandle = HxOBJECT_INT(argv[1]);
	stSessionEventInfo.eModule = HxOBJECT_INT(argv[2]);
	stSessionEventInfo.nSlot = HxOBJECT_INT(argv[3]);
	stSessionEventInfo.eventType = HxOBJECT_INT(argv[4]);
	stSessionEventInfo.sessionType = HxOBJECT_INT(argv[5]);
	if((stSessionEventInfo.eventType == eDxCASMMI_EVENTTYPE_NONE) || (stSessionEventInfo.sessionType == eDxCAS_SESSIONTYPE_NONE))
	{
		HxLOG_Error("\n");
		eResult = ERR_FAIL;
		goto END_FUNC;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s:%04d] hSession[0x%x] Cas Type [%s] Event Type [%s], Session Type [%s]\n", __FUNCTION__, __LINE__,
			stSessionEventInfo.hSession,
			oapi_cas_mmi_MakeCasType(stSessionEventInfo.eModule),
			oapi_cas_mmi_MakeStrEventType(stSessionEventInfo.eventType),
			oapi_cas_mmi_MakeStrSessionType(stSessionEventInfo.sessionType));
#endif /* defined(CONFIG_DEBUG) */

	pstControlHandle = oapi_cas_mmi_GetSessionControlHandle();
	if(pstControlHandle == NULL)
	{
		HxLOG_Error("\n");
		eResult = ERR_FAIL;
		goto END_FUNC;
	}

	switch(stSessionEventInfo.eventType)
	{
		case eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTART:
			{

				pstSession = oapi_cas_mmi_GetSessionBySessionTypeAndHandle(pstControlHandle->pstSessionInfoList, stSessionEventInfo.sessionType, stSessionEventInfo.hSession);

				if(pstSession == NULL)
				{
					   pstSession = OxMEM_Calloc(sizeof(oapiCasMmi_SessionInfo_t));
					   if(pstSession != NULL)
					   {
						   pstSession->eSessionType = stSessionEventInfo.sessionType;
						   pstSession->hUiHandle = stSessionEventInfo.ulUiHandle;
						   pstControlHandle->pstSessionInfoList = HLIB_LIST_Append(pstControlHandle->pstSessionInfoList, (void *)pstSession);
					   }
					   else
					   {
						   HxLOG_Error("alloc fail \n");
						   goto END_FUNC;

					   }
				}


				if(pstSession == NULL)
				{
					HxLOG_Error("NO SESSION INFO %d\n", stSessionEventInfo.sessionType);
					goto END_FUNC;
				}
				else
				{
					pstConn = oapi_cas_mmi_GetConn(pstSession->pstConnList, stSessionEventInfo.eModule, stSessionEventInfo.nSlot);
					if(pstConn == NULL)
					{
						hError = oapi_cas_mmi_AppendConnInfo(&(pstSession->pstConnList), stSessionEventInfo.eModule, stSessionEventInfo.nSlot, stSessionEventInfo.sessionType, &pstConn);
						if((hError != ERR_OK) || (pstConn == NULL))
						{
							hError = ERR_FAIL;
							goto END_FUNC;
						}
					}

					// TODO: 이전 status check 가 필요할 수 있다.
					pstConn->eStatus = eOxCASMMI_SESSIONSTATUS_STARTREADY;

					oapi_cas_mmi_PostRequestSessionStart((Handle_t)pstSession, stSessionEventInfo.ulUiHandle, pstConn->eModule, pstConn->nSlot, stSessionEventInfo.sessionType, NULL);

					if(_stSessionNotifierInfo[pstSession->eSessionType].pfnNotifier!= NULL)
					{
						HxLOG_Trace();
						if(s_fnCasMmiApkNotifier != NULL)
						{
							s_fnCasMmiApkNotifier((_stSessionNotifierInfo[pstSession->eSessionType].pfnNotifier), stSessionEventInfo.eventType, (HUINT32)pstSession->eSessionType, (HUINT32)pstSession);
						}
					}


				}


			}
			break;

		case eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTOP:
			pstSession = oapi_cas_mmi_GetSessionBySessionTypeAndHandle(pstControlHandle->pstSessionInfoList, stSessionEventInfo.sessionType, stSessionEventInfo.hSession);
			if(pstSession == NULL)
			{
				HxLOG_Error("\n");
				eResult = ERR_FAIL;
				goto END_FUNC;
			}
			else
			{
				pstConn = oapi_cas_mmi_GetConn(pstSession->pstConnList, stSessionEventInfo.eModule, stSessionEventInfo.nSlot);
				if(pstConn == NULL)
				{
					HxLOG_Error("NO CONN INFO 0x%x %d\n", stSessionEventInfo.eModule, stSessionEventInfo.nSlot);
					eResult = ERR_FAIL;
					goto END_FUNC;
				}


				// TODO: 여기서 opl로 알려주고, opl에서 session handle 정리 후, close session을 내려주는 것이 맞다.

				if(_stSessionNotifierInfo[pstSession->eSessionType].pfnNotifier!= NULL)
				{
					HxLOG_Trace();
					if(s_fnCasMmiApkNotifier != NULL)
					{
						s_fnCasMmiApkNotifier((_stSessionNotifierInfo[pstSession->eSessionType].pfnNotifier), stSessionEventInfo.eventType, (HUINT32)pstSession->eSessionType, (HUINT32)pstSession);
					}
				}



			}
			break;

		case eDxCASMMI_EVENTTYPE_SESSIONSTARTED:
			pstSession = oapi_cas_mmi_GetSessionBySessionTypeAndHandle(pstControlHandle->pstSessionInfoList, stSessionEventInfo.sessionType, stSessionEventInfo.hSession);
			if(pstSession == NULL)
			{
				HxLOG_Error("no session info \n");
				eResult = ERR_FAIL;
				goto END_FUNC;
			}
			else
			{
				// webapp 에서부터 session이 열린 경우에는 UI Handle이 동록되지 않을 경우도 있음.
				pstSession->hUiHandle = stSessionEventInfo.ulUiHandle;

				pstConn = oapi_cas_mmi_GetConn(pstSession->pstConnList, stSessionEventInfo.eModule, stSessionEventInfo.nSlot);
				if(pstConn == NULL)
				{
					HxLOG_Error("NO CONN INFO 0x%x %d\n", stSessionEventInfo.eModule, stSessionEventInfo.nSlot);
					eResult = ERR_FAIL;
					goto END_FUNC;
				}

				HxLOG_Debug("pstConn: %p, pstConn->eStatus: %d \n", pstConn, pstConn->eStatus);
				// TODO: check eStatus.
				switch(pstConn->eStatus)
				{
					case eOxCASMMI_SESSIONSTATUS_STARTREADY:
						pstConn->eStatus = eOxCASMMI_SESSIONSTATUS_STARTED;
						oapi_cas_mmi_PostSessionStatus((Handle_t)pstSession, pstConn->eModule, pstConn->nSlot, stSessionEventInfo.sessionType, eOxCASMMI_SESSIONSTATUS_STARTED);

						if(_stSessionNotifierInfo[pstSession->eSessionType].pfnNotifier!= NULL)
						{
							HxLOG_Trace();
							//LEAVE_OAPI_CAS_MMI_CACHE;
							if(s_fnCasMmiApkNotifier != NULL)
							{
								s_fnCasMmiApkNotifier((_stSessionNotifierInfo[pstSession->eSessionType].pfnNotifier), stSessionEventInfo.eventType, (HUINT32)pstSession->eSessionType, (HUINT32)pstSession);
							}
						}


						break;

					default:
						break;
				}
			}
			break;
		default:
			HxLOG_Error("\n");
			eResult = ERR_FAIL;
			goto END_FUNC;
			break;
	}

END_FUNC:




	return ;
}


#define ___GLOBAL_FUNCTIONS___



void	OAPI_CAS_MMI_Init(OxCasMmi_NotifierForAppkit_t pfnAppkitNotifier)
{
#if defined(CONFIG_MW_CAS) || defined(CONFIG_MW_CI)
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HxLOG_Trace();

	s_bCasMmiInit = TRUE;

	if ( VK_SEM_Create(&s_ulOapiCasMmiSemId, "OapiCasMmiSema", VK_SUSPENDTYPE_PRIORITY) != VK_OK )
	{
		HxLOG_Error("VK_SEM_Create error! \n");
	}
	HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_MMI_onSessionStatusChanged, "iiiiii", NULL, (HxRPC_Noti_t)oapi_cas_mmi_onSessionStatusChanged);
	HLIB_RPC_RegisterNotifyCallback (nRpcHandle, RPC_OBAMA_CAS_MMI_onGetEventData, "iiiiiib", NULL, (HxRPC_Noti_t)oapi_cas_mmi_onGetEventData);



	oapi_cas_mmi_InitModule();

	s_fnCasMmiApkNotifier = pfnAppkitNotifier;


#endif
}



void	OAPI_CAS_MMI_RemoveEvtInfo(DxCAS_MMI_Event_t *pstEvent)
{

	HxLOG_Trace();

	CHECK_INIT;

	ENTER_OAPI_CAS_MMI_CACHE;

	if(pstEvent != NULL)
	{
		if(pstEvent->pszEncodedUiEvent != NULL)
		{
			OxMEM_Free((void *)pstEvent->pszEncodedUiEvent);
		}
		OxMEM_Free((void *)pstEvent);
	}


	LEAVE_OAPI_CAS_MMI_CACHE;

}

void	OAPI_CAS_MMI_GetEvtInfo(DxCAS_MMI_Event_t **ppstEvent)
{
	HERROR						hError = ERR_OK;
	HxList_t					*pstEvtItem = NULL;
	oapiCasMmi_SessionControl_t 	*pstControlHandle = NULL;
	DxCAS_MMI_Event_t			*pstEvtData = NULL;

	HxLOG_Trace();

	CHECK_INIT;

	ENTER_OAPI_CAS_MMI_CACHE;

	pstControlHandle = oapi_cas_mmi_GetSessionControlHandle();
	if((pstControlHandle == NULL) ||
		(ppstEvent == NULL))
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	(*ppstEvent) = NULL;
	pstEvtItem = HLIB_LIST_Last(pstControlHandle->pstEventList);
	if(pstEvtItem == NULL)
	{
		HxLOG_Error("NO EVENT\n");
		goto END_FUNC;
	}

	pstEvtData = (DxCAS_MMI_Event_t *)HLIB_LIST_Data(pstEvtItem);
	pstControlHandle->pstEventList = HLIB_LIST_Remove(pstControlHandle->pstEventList, pstEvtData);  //pstEvdData free는 OAPI_CAS_MMI_RemoveEvtInfo 에서.
	(*ppstEvent) = pstEvtData;

	HxLOG_Print("[%s:%04d] GetEvtInfo\n", __FUNCTION__, __LINE__);
	HxLOG_Print("\t eventType %d\n", pstEvtData->eventType);
	HxLOG_Print("\t sessionType %d\n", pstEvtData->sessionType);
	HxLOG_Print("\t eModule 0x%x\n", pstEvtData->eModule);
	HxLOG_Print("\t nSlot %d\n", pstEvtData->nSlot);

	pstControlHandle->pstEventList = HLIB_LIST_RemoveAllFunc(pstControlHandle->pstEventList, oapi_cas_mmi_CbFreeEventlist);


END_FUNC:

	LEAVE_OAPI_CAS_MMI_CACHE;

}

void	OAPI_CAS_MMI_RemoveUiEvent(DxCAS_MmiSessionType_e eSessionType, DxCAS_MMI_Event_t *pstEvent)
{

	HxLOG_Print("[%s:%04d] RemoveUiEvent eSessionType %d\n", __FUNCTION__, __LINE__, eSessionType);

	HxLOG_Trace();

	CHECK_INIT;

	ENTER_OAPI_CAS_MMI_CACHE;

	if(pstEvent != NULL)
	{
		if(pstEvent->pszEncodedUiEvent != NULL)
		{
			OxMEM_Free((void *)pstEvent->pszEncodedUiEvent);
			pstEvent->pszEncodedUiEvent = NULL;
		}
		OxMEM_Free((void *)pstEvent);
		pstEvent = NULL;
	}


	LEAVE_OAPI_CAS_MMI_CACHE;

}




void	OAPI_CAS_MMI_GetUiEvent(DxCAS_MmiSessionType_e eSession, Handle_t hSession, DxCAS_MMI_Event_t **ppstEvent)
{
	HERROR						hError = ERR_OK;
	HxList_t					*pstEvtItem = NULL;
	oapiCasMmi_SessionControl_t 	*pstControlHandle = NULL;
	oapiCasMmi_SessionInfo_t		*pstSession = NULL;
	DxCAS_MMI_Event_t			*pstEvtData = NULL;


	HxLOG_Trace();

	CHECK_INIT;

	ENTER_OAPI_CAS_MMI_CACHE;

#if defined(CONFIG_DEBUG)
	HxLOG_Print("[%s:%04d] hSession: 0x%x, Session Type [%s]\n", __FUNCTION__, __LINE__,
			hSession, oapi_cas_mmi_MakeStrSessionType(eSession));
#endif /* defined(CONFIG_DEBUG) */

	pstControlHandle = oapi_cas_mmi_GetSessionControlHandle();
	if((pstControlHandle == NULL) ||
		(ppstEvent == NULL))
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	(*ppstEvent) = NULL;


	pstSession = oapi_cas_mmi_GetSessionBySessionTypeAndHandle(pstControlHandle->pstSessionInfoList, eSession, hSession);
	if(pstSession == NULL)
	{
		HxLOG_Error("NO SESSION INFO %d\n", eSession);
		goto END_FUNC;
	}
	else
	{
#if defined (CONFIG_MW_CAS_NAGRA_DALTEST) || defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
		pstEvtItem = HLIB_LIST_First(pstSession->pstUiEventList);

		if(pstEvtItem == NULL)
		{
			HxLOG_Error("NO EVENT\n");
			goto END_FUNC;
		}

		pstEvtData = (DxCAS_MMI_Event_t *)HLIB_LIST_Data(pstEvtItem);
		pstSession->pstUiEventList = HLIB_LIST_Remove(pstSession->pstUiEventList, pstEvtData); //pstEvdData free는 OAPI_CAS_MMI_RemoveEvtInfo 에서.
		(*ppstEvent) = pstEvtData;
		HxLOG_Info("pstEvtData->pszEncodedUiEvent: %s \n", pstEvtData->pszEncodedUiEvent);
#else
		pstEvtItem = HLIB_LIST_Last(pstSession->pstUiEventList);

		if(pstEvtItem == NULL)
		{
			HxLOG_Error("NO EVENT\n");
			goto END_FUNC;
		}

		pstEvtData = (DxCAS_MMI_Event_t *)HLIB_LIST_Data(pstEvtItem);
		pstSession->pstUiEventList = HLIB_LIST_Remove(pstSession->pstUiEventList, pstEvtData); //pstEvdData free는 OAPI_CAS_MMI_RemoveEvtInfo 에서.
		(*ppstEvent) = pstEvtData;
		HxLOG_Info("pstEvtData->pszEncodedUiEvent: %s \n", pstEvtData->pszEncodedUiEvent);

		pstSession->pstUiEventList = HLIB_LIST_RemoveAllFunc(pstSession->pstUiEventList, oapi_cas_mmi_CbFreeEventlist);
#endif
	}

END_FUNC:

	LEAVE_OAPI_CAS_MMI_CACHE;

}


void	OAPI_CAS_MMI_SendUiResponse(DxCAS_GroupId_e	eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSession, Handle_t hSession, const HCHAR *pzInputData)
{

	HERROR						hError = ERR_OK;
	oapiCasMmi_SessionControl_t 	*pstControlHandle = NULL;
	oapiCasMmi_SessionInfo_t		*pstSession = NULL;
	oapiCasMmi_ConnInfo_t			*pstConn = NULL;

	HxLOG_Trace();

	CHECK_INIT;

	ENTER_OAPI_CAS_MMI_CACHE;

	HxLOG_Print("[%s:%04d] SendUiResponse hSession: 0x%x, eSession %d eModule 0x%x nSlot %d\n", __FUNCTION__, __LINE__,
		hSession, eSession, eModule, nSlot);

	pstControlHandle = oapi_cas_mmi_GetSessionControlHandle();
	if((pstControlHandle == NULL) ||
		(pzInputData == NULL))
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pstSession = oapi_cas_mmi_GetSessionBySessionTypeAndHandle(pstControlHandle->pstSessionInfoList, eSession, hSession);
	if(pstSession == NULL)
	{
		HxLOG_Error("NO SESSION INFO %d\n", eSession);
		goto END_FUNC;
	}
	else
	{
		pstConn = oapi_cas_mmi_GetConn(pstSession->pstConnList, eModule, nSlot);
		if(pstConn == NULL)
		{
			HxLOG_Error("NO CONN INFO 0x%x %d\n", eModule, nSlot);
			goto END_FUNC;
		}

		switch(pstConn->eStatus)
		{
			case eOxCASMMI_SESSIONSTATUS_STARTED:
				oapi_cas_mmi_PostUserInput((Handle_t)pstSession, pstConn->eModule, pstConn->nSlot, eSession, pzInputData);
				break;
			default:
				hError = ERR_FAIL;
				goto END_FUNC;
				break;
		}
	}

END_FUNC:

	LEAVE_OAPI_CAS_MMI_CACHE;

}
STATIC HERROR oapi_cas_mmi_RemoveSessionInfo(HxList_t **ppstSessionList, oapiCasMmi_SessionInfo_t *pstSessionInfo)
{
	HERROR							hError = ERR_OK;

	HLIB_LIST_RemoveAll(pstSessionInfo->pstConnList);
	HLIB_LIST_RemoveAllFunc(pstSessionInfo->pstUiEventList, oapi_cas_mmi_CbFreeEventlist);

	*ppstSessionList = HLIB_LIST_Remove(*ppstSessionList, (void *)pstSessionInfo);


	return hError;
}

void	OAPI_CAS_MMI_CloseSession(DxCAS_GroupId_e	eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSession, Handle_t hSession)
{

	HERROR						hError = ERR_OK;
	oapiCasMmi_SessionControl_t 	*pstControlHandle = NULL;
	oapiCasMmi_SessionInfo_t		*pstSession = NULL;
	oapiCasMmi_ConnInfo_t			*pstConn = NULL;

	HxLOG_Print("[%s:%04d] CloseSession hSession: 0x%x,eSession %d eModule 0x%x nSlot %d\n", __FUNCTION__, __LINE__,
		hSession, eSession, eModule, nSlot);

	HxLOG_Trace();

	CHECK_INIT;

	ENTER_OAPI_CAS_MMI_CACHE;

	pstControlHandle = oapi_cas_mmi_GetSessionControlHandle();
	if(pstControlHandle == NULL)
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}

	pstSession = oapi_cas_mmi_GetSessionBySessionTypeAndHandle(pstControlHandle->pstSessionInfoList, eSession, hSession);
	if(pstSession == NULL)
	{
		HxLOG_Error("NO SESSION INFO %d\n", eSession);
		goto END_FUNC;
	}
	else
	{
		pstConn = oapi_cas_mmi_GetConn(pstSession->pstConnList, eModule, nSlot);
		if(pstConn == NULL)
		{
			HxLOG_Error("NO CONN INFO 0x%x %d\n", eModule, nSlot);
			goto END_FUNC;
		}

		// TODO: 여기서 session, conn info, event data free.
		(void)oapi_cas_mmi_RemoveSessionInfo(&(pstControlHandle->pstSessionInfoList), pstSession);
		pstConn->eStatus = eOxCASMMI_SESSIONSTATUS_STOPPED;
		oapi_cas_mmi_PostRequestSessionStop((Handle_t)pstSession, pstSession->hUiHandle, pstConn->eModule, pstConn->nSlot, eSession);
	}

END_FUNC:

	LEAVE_OAPI_CAS_MMI_CACHE;

}

Handle_t	OAPI_CAS_MMI_OpenSession(DxCAS_GroupId_e	eModule, HUINT32 nSlot, DxCAS_MmiSessionType_e eSession, const char *pcParamData)
{

	HERROR						hError = ERR_OK;
	oapiCasMmi_ConnInfo_t			*pstConn = NULL;
	oapiCasMmi_SessionControl_t 	*pstControlHandle = NULL;
	oapiCasMmi_SessionInfo_t		*pstSession = NULL;

	HxLOG_Print("[%s:%04d] OpenSession eSession %d eModule 0x%x nSlot %d\n", __FUNCTION__, __LINE__,
		eSession, eModule, nSlot);

	HxLOG_Trace();

	//CHECK_INIT;
	if(s_bCasMmiInit != TRUE)
	{
		return 0;
	}

	ENTER_OAPI_CAS_MMI_CACHE;

	pstControlHandle = oapi_cas_mmi_GetSessionControlHandle();
	if(pstControlHandle == NULL)
	{
		HxLOG_Error("\n");
		hError = ERR_FAIL;
		goto END_FUNC;
	}


	// OAPI_CAS_MMI_CloseSession 에서 free하게 됨.
	pstSession = OxMEM_Calloc(sizeof(oapiCasMmi_SessionInfo_t));
	if(pstSession != NULL)
	{
		pstSession->eSessionType = eSession;
		pstControlHandle->pstSessionInfoList = HLIB_LIST_Append(pstControlHandle->pstSessionInfoList, (void *)pstSession);
	}
	else
	{
		HxLOG_Error("alloc fail \n");
		goto END_FUNC;

	}

	pstConn = oapi_cas_mmi_GetConn(pstSession->pstConnList, eModule, nSlot);
	if(pstConn == NULL)
	{
		hError = oapi_cas_mmi_AppendConnInfo(&(pstSession->pstConnList), eModule, nSlot, eSession, &pstConn);
		if((hError != ERR_OK) || (pstConn == NULL))
		{
			hError = ERR_FAIL;
			goto END_FUNC;
		}
	}

	// TODO: 이전 status check 가 필요할 수 있다.
	pstConn->eStatus = eOxCASMMI_SESSIONSTATUS_STARTREADY;
	HxLOG_Debug("pstSession: %p, pstConn: %p \n", pstSession, pstConn);
	oapi_cas_mmi_PostRequestSessionStart((Handle_t)pstSession, 0, pstConn->eModule, pstConn->nSlot, eSession, pcParamData);



END_FUNC:

	LEAVE_OAPI_CAS_MMI_CACHE;
	if(hError == ERR_FAIL && pstSession != NULL)
	{
		OxMEM_Free(pstSession);
		pstSession = NULL;
	}

	return (Handle_t)pstSession;
}


void	OAPI_CAS_MMI_UnRegisterSessionNotifier(DxCAS_MmiSessionType_e eSession)
{

	HERROR						hError = ERR_OK;
	HxLOG_Trace();

	CHECK_INIT;

	ENTER_OAPI_CAS_MMI_CACHE;
	if(eSession >= eDxCAS_SESSIONTYPE_MAX)
	{
		HxLOG_Error("eSession(%d) is not appropriate \n", eSession);
		hError = ERR_FAIL;
		goto END_FUNC;
	}
	_stSessionNotifierInfo[eSession].pfnNotifier = NULL;


END_FUNC:

	LEAVE_OAPI_CAS_MMI_CACHE;

}


void	OAPI_CAS_MMI_RegisterSessionNotifier(DxCAS_MmiSessionType_e eSession, OxCasMmi_Notifier_t pfnObjectHandler)
{
	HERROR						hError = ERR_OK;

	HxLOG_Trace();

	CHECK_INIT;

	ENTER_OAPI_CAS_MMI_CACHE;

	if(eSession >= eDxCAS_SESSIONTYPE_MAX)
	{
		HxLOG_Error("eSession(%d) is not appropriate \n", eSession);
		hError = ERR_FAIL;
		goto END_FUNC;
	}
	_stSessionNotifierInfo[eSession].pfnNotifier = pfnObjectHandler;
END_FUNC:

	LEAVE_OAPI_CAS_MMI_CACHE;

}



void	OAPI_CAS_MMI_UnRegisterEventNotifier(HUINT32 objId)
{
	// not used

	oapiCasMmi_SessionControl_t 	*pstControlHandle = NULL;

	HxLOG_Trace();

	CHECK_INIT;

	ENTER_OAPI_CAS_MMI_CACHE;

	pstControlHandle = oapi_cas_mmi_GetSessionControlHandle();
	if(pstControlHandle != NULL)
	{
		pstControlHandle->pfnNotifier = NULL;
	}


	LEAVE_OAPI_CAS_MMI_CACHE;

}
