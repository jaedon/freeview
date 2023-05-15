/*******************************************************************
	File Description
*******************************************************************/
/*++
    @file        cxmgr_action.c
    @brief       Conax Manager Action

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
--*/
/*******************************************************************/
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
/*******************************************************************/
#define ________CXMGR_ACTION_Private_Include__________________________________
/*******************************************************************/
#include <conax.h>
#include <cxmgr_action.h>
#include <_svc_cas_sub_svc_api.h>

/*******************************************************************/
#define ________CXMGR_ACTION_Golbal_Value_____________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CXMGR_ACTION_Private_Define___________________________________
/*******************************************************************/
#define GET_STBHANDLE(a, b)					((a & 0xFF000000) | (b & 0x00FFFFFF))
#if defined(GET_ACTION_ID)
#undef GET_ACTION_ID
#define GET_ACTION_ID(x)					(svc_cas_SubSvcRmGetActionId(x))
#endif

typedef struct
{
	HBOOL				bConnect;	// CasMgr로부터 선택 받았느냐를 표시..
	Handle_t			hAction;
	Handle_t			hSvc;
	SvcCas_ActionType_e	eActType;
	HBOOL				bMainAction;
} CxActionTable_t;

typedef struct
{
	HUINT32				ulCasInstanceId;
	SvcCas_CbFuncList_t	stCbList;
	CxActionTable_t		stActionList[CAS_MAX_NUM_OF_ACTION];
} CxInstance_t;

/*******************************************************************/
#define ________CXMGR_ACTION_Private_Static_Value_____________________________
/*******************************************************************/
STATIC CxInstance_t	s_stCxActContext;
STATIC DxCopyrightControl_t	s_stCcInfo[CAS_MAX_NUM_OF_ACTION];

/*******************************************************************/
#define ________CXMGR_ACTION_Global_Value_____________________________________
/*******************************************************************/
/*******************************************************************/
#define ________CXMGR_ACTION_Private_Static_Prototype_________________________
/*******************************************************************/
/*******************************************************************/
#define ________CXMGR_ACTION_Private_Static_Prototype_Body____________________
/*******************************************************************/
STATIC CxInstance_t* local_cxact_GetContext(void)
{
	return &s_stCxActContext;
}

STATIC void local_cxact_InitInstance(void)
{
	int		i;

	for (i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		s_stCxActContext.stActionList[i].bConnect = FALSE;
		s_stCxActContext.stActionList[i].hAction = HANDLE_NULL;
		s_stCxActContext.stActionList[i].hSvc = HANDLE_NULL;
		s_stCxActContext.stActionList[i].eActType = eCAS_ActionType_None;
	}

	HxSTD_memset(&s_stCcInfo, 0, sizeof(DxCopyrightControl_t) * CAS_MAX_NUM_OF_ACTION);
}

// 동작 순서
// 1. startSession이 호출되면, session을 할당 받아서 hAction과 연결 시킨다. 이후 내부 operation에서는 hAction, hSvc가 아닌 hSession 값을 이용
// 2. updatePmt가 호출되면, 각 ES를 stream table에 저장하고 연결되는 ECM PID는 ecm table에 저장하고 각각 연결시킨다. (N:1 관계임)
// 3. setPid가 호출되면, 실제 사용되는 ES만 걸러낸다. (stream table update) 그리고, descrambler 할당을 받는다.
// 4.

#define Callback_functions__________________________________________________
STATIC void local_cxact_StartSession(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType)
{
	Handle_t		hStbHandle;
	HINT32			nActionId;
	CxInstance_t* 	pCxInst;

	ENTER_FUNCTION;

	pCxInst = local_cxact_GetContext();

	nActionId = GET_ACTION_ID(hAction);

	//HxLOG_Error("###########################################################\r\n");
	//HxLOG_Error("hAction : 0x%08x\r\n", hAction);
	//HxLOG_Error("nActionId : 0x%08x\r\n", nActionId);
	//HxLOG_Error("hSvc : 0x%08x\r\n", hSvc);
	//HxLOG_Error("eActType : %d\r\n", eActType);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		pCxInst->stActionList[nActionId].hAction = hAction;
		pCxInst->stActionList[nActionId].hSvc = hSvc;
		pCxInst->stActionList[nActionId].eActType = eActType;

		hStbHandle = GET_STBHANDLE(hAction, hSvc);
		//HxLOG_Error("hStbHandle : 0x%08x\r\n", hStbHandle);

		CXAPI_StartSvc(hStbHandle);
	}

	//HxLOG_Error("###########################################################\r\n");
	LEAVE_FUNCTION;
}

STATIC void local_cxact_StopSession(Handle_t hAction, Handle_t hNextSvc)
{
	Handle_t		hStbHandle;
	HINT32			nActionId;
	CxInstance_t* 	pCxInst;
	HBOOL			bNextSvcExist;

	ENTER_FUNCTION;

	pCxInst = local_cxact_GetContext();

	nActionId = GET_ACTION_ID(hAction);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		hStbHandle = GET_STBHANDLE(hAction, pCxInst->stActionList[nActionId].hSvc);

		if (hNextSvc == HANDLE_NULL)
		{
			bNextSvcExist = FALSE;
		}
		else
		{
			bNextSvcExist = TRUE;
		}

		CXAPI_StopSvc(hStbHandle, bNextSvcExist);

		pCxInst->stActionList[nActionId].bConnect = FALSE;
		pCxInst->stActionList[nActionId].hAction = HANDLE_NULL;
		pCxInst->stActionList[nActionId].hSvc = HANDLE_NULL;
	}
	LEAVE_FUNCTION;
}

STATIC void local_cxact_SetPid(Handle_t hAction, SvcCas_SetPidInfo_t* pstCaDscInfo)
{
	// stream table 정리
	int				i;
	Handle_t		hStbHandle;
	HINT32			nActionId;
	CxEsPidInfo_t* 	pstInfo;
	CxInstance_t* 	pCxInst;

	ENTER_FUNCTION;

	pCxInst = local_cxact_GetContext();
	nActionId = GET_ACTION_ID(hAction);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		hStbHandle = GET_STBHANDLE(hAction, pCxInst->stActionList[nActionId].hSvc);

		switch(pCxInst->stActionList[nActionId].eActType)
		{
			case eCAS_ActionType_Playback:
			case eCAS_ActionType_Tsr_Play:
				break;

			default:
				pstInfo = (CxEsPidInfo_t *)OxCAS_Malloc(sizeof(CxEsPidInfo_t));
				if (pstInfo != NULL)
				{
					for (i = 0; i < pstCaDscInfo->ulPidNum && i < CX_MAX_NUM_OF_ESPID ; i++)
					{
						pstInfo->eType[i] = pstCaDscInfo->stPidInfo[i].esType;
						pstInfo->usPid[i] = pstCaDscInfo->stPidInfo[i].usPid;
					}

					pstInfo->ulPidNum = i;

					CXAPI_SetPid(hStbHandle, pstInfo);
				}
				break;
		}
	}

	LEAVE_FUNCTION;
}

STATIC void local_cxact_UpdatePmt(Handle_t hAction, HUINT8* pucPmt)
{
	// pmt 처리해주세요
	Handle_t			hStbHandle;
	HINT32				nActionId;
	SvcCas_PmtInfo_t* 	pstPmtInfo;
	HUINT32				ulSecStatus;
	HUINT32				ulErr;
	CxInstance_t* 		pCxInst;

	ENTER_FUNCTION;

	pCxInst = local_cxact_GetContext();

	nActionId = GET_ACTION_ID(hAction);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		hStbHandle = GET_STBHANDLE(hAction, pCxInst->stActionList[nActionId].hSvc);

		ulErr = svc_cas_MgrSubGetPmtSectionState(hAction, &ulSecStatus);
		if (ulErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_MgrSubGetPmtSectionState return error(%x), Action(0x%x)\n", ulErr, hAction);
			LEAVE_FUNCTION;
			return;
		}

		if (ulSecStatus != TRUE)
		{
			HxLOG_Error("PMT Section status (%d)\n", ulSecStatus);
			LEAVE_FUNCTION;
			return;
		}

		if ((pstPmtInfo = svc_cas_MgrSubGetPmtInfo(hAction)) == NULL)
		{
			HxLOG_Error("[local_caPsi_PmtReceived] error in svc_cas_MgrSubGetPmtInfo\n");
			LEAVE_FUNCTION;
			return;
		}

		switch(pCxInst->stActionList[nActionId].eActType)
		{
			case eCAS_ActionType_Playback:
			case eCAS_ActionType_Tsr_Play:
				break;

			default:
				CXAPI_UpdatePmt(hStbHandle, pstPmtInfo);
				break;
		}
	}
	LEAVE_FUNCTION;
}

STATIC void local_cxact_UpdateCat(Handle_t hAction, HUINT16 usUniqueId, HUINT8* pucCat)
{
	// cat 다듬에서 s/c에 전달
	Handle_t		hStbHandle;
	HINT32			nActionId;
	CxInstance_t* 	pCxInst;

	ENTER_FUNCTION;

	pCxInst = local_cxact_GetContext();

	nActionId = GET_ACTION_ID(hAction);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		hStbHandle = GET_STBHANDLE(hAction, pCxInst->stActionList[nActionId].hSvc);

		switch(pCxInst->stActionList[nActionId].eActType)
		{
			case eCAS_ActionType_Playback:
			case eCAS_ActionType_Tsr_Play:
				break;

			default:
				CXAPI_UpdateCat(hStbHandle, usUniqueId, pucCat);
				break;
		}
	}
	LEAVE_FUNCTION;
}

// below callback functions are not used
//void local_cxact_UpdateSdt (Handle_t hAction, HUINT16 usUniqueId)
//void local_cxact_UpdateNit (Handle_t hAction, HUINT16 usUniqueId)
//void local_cxact_UpdateEit (Handle_t hAction, HUINT16 usUniqueId)

STATIC void local_cxact_Select(Handle_t hAction, HUINT32 ulCasInstanceId, HBOOL bConnect)
{
	HINT32			nActionId;
	CxInstance_t* 	pCxInst;
	Handle_t		hStbHandle;

	ENTER_FUNCTION;

	pCxInst = local_cxact_GetContext();

	nActionId = GET_ACTION_ID(hAction);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		if (pCxInst->stActionList[nActionId].bConnect != bConnect)
		{
			pCxInst->stActionList[nActionId].bConnect = bConnect;
		}

		hStbHandle = GET_STBHANDLE(hAction, pCxInst->stActionList[nActionId].hSvc);

		switch(pCxInst->stActionList[nActionId].eActType)
		{
			case eCAS_ActionType_Playback:
			case eCAS_ActionType_Tsr_Play:
				break;

			default:
				CXAPI_Select(hStbHandle, pCxInst->stActionList[nActionId].bConnect);
				break;
		}
	}

	LEAVE_FUNCTION;
}

// 마지막으로 보냈던 메시지를 얻어간다.
STATIC void local_cxact_CheckLastMsg(Handle_t hAction, SvcCas_MsgType_e* peMsgType, DxCAS_GroupId_e* peCasId, void** ppMessage, SvcCas_ShowType_e* peShowType)
{
	ENTER_FUNCTION;

	LEAVE_FUNCTION;
}
/*******************************************************************/
#define ________CXMGR_ACTION_Public_Functions_Body____________________________
/*******************************************************************/
SvcCas_ActionType_e CXACT_GetActionTypeFromActionHandle(Handle_t hAction)
{
	HINT32	nActionId;

	nActionId = svc_cas_SubSvcRmGetActionId(hAction);

	if (nActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		return eCAS_ActionType_None;
	}

	return s_stCxActContext.stActionList[nActionId].eActType;
}

SvcCas_ActionType_e CXACT_GetActionTypeFromStbHandle(Handle_t hStbHandle)
{
	HINT32	nActionId;

	nActionId = GET_ACTION_ID_FROM_STBHANDLE(hStbHandle);

	if (nActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		return eCAS_ActionType_None;
	}

	return s_stCxActContext.stActionList[nActionId].eActType;
}

Handle_t CXACT_GetSvcHandleFromStbHandle(Handle_t hStbHandle)
{
	HINT32	nActionId;

	nActionId = GET_ACTION_ID_FROM_STBHANDLE(hStbHandle);

	if (nActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		return HANDLE_NULL;
	}

	return s_stCxActContext.stActionList[nActionId].hSvc;
}

HBOOL CXACT_IsLiveActionTypeFromStbHandle(Handle_t hStbHandle)
{
	SvcCas_ActionType_e	eActionType;

	eActionType = CXACT_GetActionTypeFromStbHandle(hStbHandle);

	if (eActionType != eCAS_ActionType_Live)
	{
		return FALSE;
	}

	return TRUE;
}

HBOOL CXACT_IsTsrActionTypeFromStbHandle(Handle_t hStbHandle)
{
	SvcCas_ActionType_e	eActionType;

	eActionType = CXACT_GetActionTypeFromStbHandle(hStbHandle);

	if (eActionType != eCAS_ActionType_Tsr_Rec)
	{
		return FALSE;
	}

	return TRUE;
}

HBOOL CXACT_IsRecActionTypeFromStbHandle(Handle_t hStbHandle)
{
	SvcCas_ActionType_e	eActionType;

	eActionType = CXACT_GetActionTypeFromStbHandle(hStbHandle);

	if (eActionType != eCAS_ActionType_Recording)
	{
		return FALSE;
	}

	return TRUE;
}

#if defined(CONFIG_MW_MM_DELAYED_REC)
HERROR CXACT_UpdateTsrActionTypeToRecord(Handle_t hAction)
{
	// tsr -> delayed rec으로 변경된 경우엔 action type을 recording으로 변경해 주자
	HINT32	nActionId;
	Handle_t hStbHandle;

	nActionId = GET_ACTION_ID(hAction);

	//VK_Print("ooo - CXMGR_UpdateTsrActionTypeToRecord : hAction(0x%X) nActionId(%d) \n", hAction, nActionId);

	if (nActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		return ERR_FAIL;
	}

	if (s_stCxActContext.stActionList[nActionId].eActType != eCAS_ActionType_Tsr_Rec)
	{
		return ERR_FAIL;
	}

	s_stCxActContext.stActionList[nActionId].eActType = eCAS_ActionType_Recording;

	hStbHandle = GET_STBHANDLE(s_stCxActContext.stActionList[nActionId].hAction, s_stCxActContext.stActionList[nActionId].hSvc);

	CXAPI_MoveSvc(hStbHandle);

	return ERR_OK;
}
#endif

Handle_t CXACT_GetActionHandle(Handle_t hStbHandle)
{
	HINT32	nActionId;
	int	i;

	if (hStbHandle == HANDLE_NULL)
	{
		// playback 관련된 Action이 있는지 먼저 찾아보고..
		for (i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
		{
			switch(s_stCxActContext.stActionList[i].eActType)
			{
				case eCAS_ActionType_Playback:
				case eCAS_ActionType_Tsr_Play:
					return s_stCxActContext.stActionList[i].hAction;
					break;

				default:
					break;
			}
		}

		// FCC에서 main action도 찾아보고...
#if defined(CONFIG_SUPPORT_FCC)
		// 부팅 직후 채널전환이 없을 경우에는 여기에서 못 찾아요...
		for (i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
		{
			if (s_stCxActContext.stActionList[i].eActType == eCAS_ActionType_Live)
			{
				if(s_stCxActContext.stActionList[i].bMainAction)
				{
					return s_stCxActContext.stActionList[i].hAction;
				}
			}
		}
#endif
		// 부팅직후인 듯 하니 첫번째 live가 main action이네..
		for (i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
		{
			if (s_stCxActContext.stActionList[i].eActType == eCAS_ActionType_Live)
			{
				return s_stCxActContext.stActionList[i].hAction;
			}
		}
	}

	nActionId = GET_ACTION_ID_FROM_STBHANDLE(hStbHandle);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		return s_stCxActContext.stActionList[nActionId].hAction;
	}

	return HANDLE_NULL;
}

Handle_t CXACT_GetStbHandleFromActionHandle(Handle_t hAction)
{
	int	i;

	for (i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		if (s_stCxActContext.stActionList[i].hAction == hAction)
		{
			return GET_STBHANDLE(hAction, s_stCxActContext.stActionList[i].hSvc);
		}
	}

	return HANDLE_NULL;
}

HERROR CXACT_UpdateCasInfo(HUINT16 usScSlot, HUINT16* pusCaSysId, HUINT32 ulNumCaSysId, HUINT32 ulMaxNumSvc, HBOOL bAvailable)
{
	CxInstance_t* 	pCxInst;

	ENTER_FUNCTION;

	pCxInst = local_cxact_GetContext();
	if (svc_cas_MgrSubUpdateCasInfo(pCxInst->ulCasInstanceId,	//
								eDxCAS_GROUPID_CX, 		//
								usScSlot, 				//
								pusCaSysId, 			//
								ulNumCaSysId, 			//
								ulMaxNumSvc, 			//
								bAvailable) != ERR_OK)	//
	{
		HxLOG_Error("error in svc_cas_MgrSubUpdateCasInfo\n");
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

HERROR CXACT_UpdateDrmInfo(Handle_t hAction, DxCopyrightControl_ConaxDrmInfo_t *pstDrmInfo)
{
	CxInstance_t *pstConaxInstance;
	HUINT32 ulActionId;

	if (hAction == HANDLE_NULL || pstDrmInfo == NULL)
	{
		HxLOG_Error("invalid parameter\n");
		return ERR_FAIL;
	}

	ulActionId = svc_cas_SubSvcRmGetActionId(hAction);
	if(ulActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Error("invalid ActionId (%d, %d)\n", ulActionId, CAS_MAX_NUM_OF_ACTION);
		return ERR_FAIL;
	}

#if 0
	if(HxSTD_memcmp(&s_stCcInfo[ulActionId].stCasCcInfo.info.stConaxDrmInfo, pstDrmInfo, sizeof(DxCopyrightControl_ConaxDrmInfo_t)) == 0)
	{
		HxLOG_Print("same drm info, ignore\n");
		return ERR_OK;
	}
#endif

	pstConaxInstance = local_cxact_GetContext();

	s_stCcInfo[ulActionId].bCasCc = TRUE;
	s_stCcInfo[ulActionId].stCasCcInfo.eCasCcType = eDxCopyrightControl_CasCcType_Conax;
	HxSTD_memcpy(&s_stCcInfo[ulActionId].stCasCcInfo.info.stConaxDrmInfo, pstDrmInfo, sizeof(DxCopyrightControl_ConaxDrmInfo_t));

	if(svc_cas_MgrSubUpdateDrmInfo(hAction, pstConaxInstance->ulCasInstanceId, (void*)&s_stCcInfo[ulActionId], eDxCopyrightControl_CasCcType_Conax, 0) != ERR_OK)
	{
		HxLOG_Error("error in svc_cas_MgrSubUpdateDrmInfo\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR CXACT_GetDrmInfo(Handle_t hAction, DxCopyrightControl_ConaxDrmInfo_t *pstDrmInfo)
{
	HUINT32 ulActionId;

	if (hAction == HANDLE_NULL || pstDrmInfo == NULL)
	{
		HxLOG_Error("invalid parameter\n");
		return ERR_FAIL;
	}

	ulActionId = svc_cas_SubSvcRmGetActionId(hAction);
	if(ulActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Error("invalid ActionId (%d, %d)\n", ulActionId, CAS_MAX_NUM_OF_ACTION);
		return ERR_FAIL;
	}

	HxSTD_memcpy(pstDrmInfo, &s_stCcInfo[ulActionId].stCasCcInfo.info.stConaxDrmInfo, sizeof(DxCopyrightControl_ConaxDrmInfo_t));

	return ERR_OK;
}

#if defined(CONFIG_SUPPORT_FCC)
HERROR CXACT_SetMainAction(Handle_t hMainAction, Handle_t hPrevAction)
{
	int i;
	HUINT32 ulActionId;
	Handle_t hMainStbHandle, hPrevStbHandle = HANDLE_NULL;
	CxActionTable_t *pstPrevAction = NULL;

	for(i = 0 ; i < CAS_MAX_NUM_OF_ACTION ; i++)
	{
		if(s_stCxActContext.stActionList[i].bMainAction == TRUE)
		{
#if defined(CONFIG_DEBUG)
			// MainView가 두개일 수가 없다. 로직상 오류이다...
			if(pstPrevAction != NULL)
			{
				HxLOG_Warning("check it...!!!\n");
			}
#endif
			pstPrevAction = &s_stCxActContext.stActionList[i];
		}

		s_stCxActContext.stActionList[i].bMainAction = FALSE;
	}
#if 0
	ulActionId = GET_ACTION_ID(hPrevAction);
	if(ulActionId < CAS_MAX_NUM_OF_ACTION)
	{
		hPrevStbHandle = GET_STBHANDLE(s_stCxActContext.stActionList[ulActionId].hAction, s_stCxActContext.stActionList[ulActionId].hSvc);
	}
	else
	{
		HxLOG_Error("Check ulActionId...\n");
		return ERR_FAIL;
	}
#else
	if(pstPrevAction != NULL)
	{
		hPrevStbHandle = GET_STBHANDLE(pstPrevAction->hAction, pstPrevAction->hSvc);
	}
#endif

	ulActionId = GET_ACTION_ID(hMainAction);
	if(ulActionId >= CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Error("Check ulActionId...\n");
		return ERR_FAIL;
	}

	s_stCxActContext.stActionList[ulActionId].bMainAction = TRUE;
	hMainStbHandle = GET_STBHANDLE(s_stCxActContext.stActionList[ulActionId].hAction, s_stCxActContext.stActionList[ulActionId].hSvc);

	CXAPI_SetMainHandle(hMainStbHandle, hPrevStbHandle);

	return ERR_OK;
}
#endif

HERROR CXACT_ReInit(void)
{
	int i;

	for(i = 0 ; i < CAS_MAX_NUM_OF_ACTION ; i++)
	{
		s_stCxActContext.stActionList[i].bMainAction = FALSE;
	}

	return ERR_OK;
}

HERROR CXACT_Init(void)
{
	CxInstance_t* 	pCxInst;

	ENTER_FUNCTION;
	local_cxact_InitInstance();

	pCxInst = local_cxact_GetContext();
	pCxInst->stCbList.pfnCbStartSession = local_cxact_StartSession;
	pCxInst->stCbList.pfnCbStopSession = local_cxact_StopSession;
	pCxInst->stCbList.pfnCbSetDSC = local_cxact_SetPid;
	pCxInst->stCbList.pfnCbUpdatePmt = local_cxact_UpdatePmt;
	pCxInst->stCbList.pfnCbUpdateCat = local_cxact_UpdateCat;
	pCxInst->stCbList.pfnCbUpdateSdt = NULL;
	pCxInst->stCbList.pfnCbUpdateNit = NULL;
	pCxInst->stCbList.pfnCbUpdateEit = NULL;
	pCxInst->stCbList.pfnCbInstSelected = local_cxact_Select;
	pCxInst->stCbList.pfnCbCheckLastMsg = local_cxact_CheckLastMsg;

	if (svc_cas_MgrSubRegisterCb(eCasInst_Embedded, 	//
									eDxCAS_GROUPID_CX, 	//
									0, 					//
									&pCxInst->stCbList, //
									1, 					//
									&pCxInst->ulCasInstanceId) != ERR_OK)	//
	{
		LEAVE_FUNCTION;
		return ERR_FAIL;
	}

	LEAVE_FUNCTION;
	return ERR_OK;
}

