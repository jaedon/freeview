/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/**
	@file     vamgr_action.c
	@brief    vamgr_action.c (Viaccess MAIN)

	Description: 주로 CasMgr에서 전달되는 action에 대응한 작업  \n
	Module: MW/CAS/ CONAX \n
	Remarks :\n

	Copyright (c) 2009 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include "vamgr_action.h"
#include "vamgr_main.h"

#include "va_pi_api.h"
#include <va_setup.h>

#include <_svc_cas_mgr_main.h>
#include <_svc_cas_sub_api.h>
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
//#define DEBUG_PMT_RAW_DATA
//#define DEBUG_CAT_RAW_DATA

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define FUNC_ENTER 		HxLOG_Debug("(+)Enter!!\n")
#define FUNC_LEAVE(x)	HxLOG_Debug("(-)Leave!!(0x%X)\n", x)


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

// 동작 순서
// 1. startSession이 호출되면, session을 할당 받아서 hAction과 연결 시킨다. 이후 내부 operation에서는 hAction, hSvc가 아닌 hSession 값을 이용
// 2. updatePmt가 호출되면, 각 ES를 stream table에 저장하고 연결되는 ECM PID는 ecm table에 저장하고 각각 연결시킨다. (N:1 관계임)
// 3. setPid가 호출되면, 실제 사용되는 ES만 걸러낸다. (stream table update) 그리고, descrambler 할당을 받는다.
// 4.

#define Callback_functions__________________________________________________

STATIC void local_vaact_StartSession(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType)
{

	VaInstance_t		*pInst;
	SvcCas_Context_t 	*pContext = NULL;
	HINT32			nActionId;
	Handle_t 			hVaAction;

	HxLOG_Print("[Enter(%x,%x,%d)]\n", hAction, hSvc, eActType);

	pInst = VAMGR_GetInstance();
	pContext = (SvcCas_Context_t *)svc_cas_MgrSubGetActionContextByActionId(hAction);

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
	if (pContext == NULL ||
		(hAction == HANDLE_NULL && eActType != eCAS_ActionType_Playback && eActType != eCAS_ActionType_Tsr_Play))
#else
	if (hAction == HANDLE_NULL || pContext == NULL)
#endif
	{
		HxLOG_Error("Handle is invalid\r\n");
		return;
	}

	nActionId = GET_ACTION_ID(hAction);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		pInst->stActionList[nActionId].hAction 	= hAction;
		pInst->stActionList[nActionId].hSvc 	= hSvc;
		pInst->stActionList[nActionId].eActType = eActType;

		hVaAction = GET_STBHANDLE(hAction, hSvc);

		HxLOG_Info("[%x,%x,%d] VA_PI_Adapt_StartProgram(%x) calling...\n", hAction, hSvc, eActType, hVaAction);
		VA_PI_Adapt_StartProgram(hVaAction, pContext->usUniqueId);
	}
	else
	{
		HxLOG_Error("nActionId is invalid (%d)\r\n", nActionId);
	}

	HxLOG_Print("[Exit]\n");
}

STATIC void local_vaact_StopSession(Handle_t hAction, Handle_t hNextSvc)
{
	VaInstance_t		*pInst;
	HINT32			nActionId;
	Handle_t 			hVaAction;
	HBOOL			bNextSvcExist;

	HxLOG_Print("[Enter(%x,%x)]\n", hAction, hNextSvc);

	pInst = VAMGR_GetInstance();
	nActionId = GET_ACTION_ID(hAction);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		hVaAction = GET_STBHANDLE(hAction, pInst->stActionList[nActionId].hSvc);

		if (hNextSvc == HANDLE_NULL)
			bNextSvcExist = FALSE;
		else
			bNextSvcExist = TRUE;

		HxLOG_Info("[%x,%x] VA_PI_Adapt_StopProgram(%x) calling...\n", hAction, hNextSvc, hVaAction);
		VA_PI_Adapt_StopProgram(hVaAction);

		pInst->stActionList[nActionId].bConnect	= FALSE;
		pInst->stActionList[nActionId].bPmt		= FALSE;
		pInst->stActionList[nActionId].hAction	= HANDLE_NULL;
		pInst->stActionList[nActionId].hSvc		= HANDLE_NULL;
	}
	else
	{
		HxLOG_Error("nActionId is invalid (%d)\r\n", nActionId);
	}

	HxLOG_Print("[Exit]\n");
}

STATIC void local_vaact_SetPid(Handle_t hAction, SvcCas_SetPidInfo_t *pstCaDscInfo)
{
	// stream table 정리
	VaInstance_t		*pInst;
	HINT32			nActionId;
	Handle_t 			hVaAction;

	HxLOG_Print("[Enter]\n");

	if (pstCaDscInfo == NULL)
	{
		HxLOG_Error("pstCaDscInfo is NULL\n");
		return;
	}

	pInst = VAMGR_GetInstance();
	nActionId = GET_ACTION_ID(hAction);

#ifdef CONFIG_DEBUG
{
	int i;
	for (i = 0; i < pstCaDscInfo->ulPidNum; i++)
	{
		HxLOG_Info("[local_vaact_SetPid:%d = %x(%x)]\n", i, pstCaDscInfo->stPidInfo[i].usPid, pstCaDscInfo->stPidInfo[i].esType);
	}
}
#endif

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		hVaAction = GET_STBHANDLE(hAction, pInst->stActionList[nActionId].hSvc);
		HxLOG_Info("[%x] VA_PI_Adapt_SetPid(%x) calling...\n", hAction, hVaAction);
		VA_PI_Adapt_SetPid(hVaAction, pstCaDscInfo);
	}
	else
	{
		HxLOG_Error("nActionId is invalid (%d)\n", nActionId);
	}

	HxLOG_Print("[Exit]\n");
}

STATIC void local_vaact_UpdatePmt(Handle_t hAction, HUINT8 *pucPmt)
{
	VaInstance_t		*pInst;
	HINT32			nActionId;
	Handle_t 			hVaAction;
	SvcCas_PmtInfo_t 	*pstPmtInfo;
	HUINT32			ulSecStatus;
	HUINT32			ulErr;
	int			nLength;

	HxLOG_Print("[Enter]\n");

	if (pucPmt == NULL)
	{
		HxLOG_Error("pucPmt is NULL\n");
		return;
	}

	nLength = Get12bit(pucPmt + 1) + 3;

	if (nLength < 16 || nLength > VA_SI_SECTION_SIZE)
	{
		HxLOG_Error("wrong length (%d)\n", nLength);
		return;
	}

	if (nLength == 16)
	{
		HxLOG_Info("no descriptor\n");
	}

	if (pucPmt[0] != 0x02 || !(pucPmt[5] & 0x1))
	{
		HxLOG_Error("wrong PMT (%x, %x)\n", pucPmt[0], pucPmt[5]);
		return;
	}

#ifdef DEBUG_PMT_RAW_DATA
	{
		int i;

		HxLOG_Print("[PMT_START][%d] : ", nLength);
		for (i = 0; i < nLength; i++)
		{
			HxLOG_Print("%02X ", pucPmt[i]);
		}
		HxLOG_Print("[PMT_END]\n");
	}
#endif

	pInst = VAMGR_GetInstance();
	nActionId = GET_ACTION_ID(hAction);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
	 	ulErr = svc_cas_MgrSubGetPmtSectionState(hAction, &ulSecStatus);
		if (ulErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_MgrSubGetPmtSectionState return error(%x), Action(0x%x)\n", ulErr, hAction);
			return;
		}

		if (ulSecStatus != TRUE)
		{
			HxLOG_Error("PMT Section status (%d)\n", ulSecStatus);
			return;
		}

		pInst->stActionList[nActionId].bPmt = TRUE;

		hVaAction = GET_STBHANDLE(hAction, pInst->stActionList[nActionId].hSvc);

#if 0 // org
		// 아직 select되지 않았다면 select되었을 때 pmt를 전달할 것이다.
		if (pInst->stActionList[nActionId].bConnect)
		{
			if ((pstPmtInfo = svc_cas_MgrSubGetPmtInfo(hAction)) == NULL)
			{
				HxLOG_Error("error in svc_cas_MgrSubGetPmtInfo\n");
				return;
			}

			HxLOG_Info("[%x] VA_PI_Adapt_PmtChange(%x) calling... (ServiceId = %x, VerNum = %x)\n", hAction, hVaAction, pstPmtInfo->usServiceId, pstPmtInfo->ucVerNum);
			VA_PI_Adapt_PmtChange(hVaAction, pstPmtInfo);
		}
#else // hmkim : modified.
		// Viaccess Driver Test "DSCR_STREAM_3" 의 경우 채널 전환하여 테스트할 서비스가 앞부분은 scramble 이고 뒷부분은 clear 이다.
		// 만약 채널 전환하는 시점에서 마침 서비스가 clear 구간에 play 되고 있으면 위 코드에선 VA_PI_Adapt_PmtChange() 가 호출되지 않으며 이로 인해 결국 테스트가 fail 된다.
		// 원래의 코드에서 bConnect 가 false 인 경우에 왜 VA_PI_Adapt_PmtChange() 를 호출하지 않고 VA_UI_Adapt_SendLastEventMsgToAp() 를 호출하는지 이해가 되지 않음.
		// PMT update 를 받으면 무조건 VA_PI_Adapt_PmtChange() 를 호출하도록 수정함. 또한 local_vaact_ManageInstance() 도 함께 수정함.

		if ((pstPmtInfo = svc_cas_MgrSubGetPmtInfo(hAction)) == NULL)

		{
			HxLOG_Error("error in svc_cas_MgrSubGetPmtInfo\n");
			return;
		}

		HxLOG_Info("[%x] VA_PI_Adapt_PmtChange(%x) calling... (ServiceId = %x, VerNum = %x)\n", hAction, hVaAction, pstPmtInfo->usServiceId, pstPmtInfo->ucVerNum);
//		VA_PI_Adapt_PmtChange(hVaAction, pstPmtInfo);  //__VIACCESS50__
		VA_PI_Adapt_PmtChange(hVaAction, pstPmtInfo, pucPmt);
#endif
	}
	else
	{
		HxLOG_Error("nActionId is invalid (%d)\r\n", nActionId);
	}

	HxLOG_Print("[Exit]\n");
}

STATIC void local_vaact_UpdateCat(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pucCat)
{
	// cat 다듬에서 s/c에 전달
	VaInstance_t		*pInst;
	HINT32			nActionId;
	Handle_t 			hVaAction;
	int 		nLength;

	HxLOG_Print("[Enter]\n");

	if (pucCat == NULL)
	{
		HxLOG_Error("pucCat is NULL\n");
		return;
	}

	nLength = Get12bit(pucCat + 1) + 3;

	if (nLength < 12 || nLength > VA_SI_SECTION_SIZE)
	{
		HxLOG_Error("wrong length (%d)\n", nLength);
		return;
	}

	if (nLength == 12)
	{
		HxLOG_Info("no descriptor\n");
	}

	if (pucCat[0] != 0x01 || !(pucCat[5] & 0x1))
	{
		HxLOG_Error("wrong CAT (%x, %x)\n", pucCat[0], pucCat[5]);
		return;
	}

#ifdef DEBUG_CAT_RAW_DATA
	{
		int i;

		HxLOG_Print("[CAT_START][%d] : ", nLength);
		for (i = 0; i < nLength; i++)
		{
			HxLOG_Print("%02X ", pucCat[i]);
		}
		HxLOG_Print("[CAT_END]\n");
	}
#endif

	pInst = VAMGR_GetInstance();
	nActionId = GET_ACTION_ID(hAction);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		hVaAction = GET_STBHANDLE(hAction, pInst->stActionList[nActionId].hSvc);

		HxLOG_Info("[%x] VA_PI_Adapt_CatChange(%x) calling...\n", hAction, hVaAction);
		VA_PI_Adapt_CatChange(hVaAction, usUniqueId, pucCat);
	}
	else
	{
		HxLOG_Error("nActionId is invalid (%d)\r\n", nActionId);
	}

	HxLOG_Print("[Exit]\n");
}

// below callback functions are not used
//void local_vaact_UpdateSdt (Handle_t hAction, HUINT16 usUniqueId)
//void local_vaact_UpdateNit (Handle_t hAction, HUINT16 usUniqueId)
//void local_vaact_UpdateEit (Handle_t hAction, HUINT16 usUniqueId)

STATIC void local_vaact_ManageInstance(Handle_t hAction, HUINT32 ulCasInstanceId, HBOOL bConnect)
{
	VaInstance_t		*pInst;
	HINT32			nActionId;
	Handle_t 			hVaAction;
	SvcCas_PmtInfo_t 	*pstPmtInfo;

	HxLOG_Print("[Enter]\n");

	pInst = VAMGR_GetInstance();
	nActionId = GET_ACTION_ID(hAction);

	if (nActionId < CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Info("hAction = %x, CAS bConnect = %d, VA bConnect = %d, bPmt = %d\n", hAction, bConnect, pInst->stActionList[nActionId].bConnect, pInst->stActionList[nActionId].bPmt);

		if (pInst->stActionList[nActionId].bConnect != bConnect)
		{
			pInst->stActionList[nActionId].bConnect = bConnect;
		}

		hVaAction = GET_STBHANDLE(hAction, pInst->stActionList[nActionId].hSvc);

		VA_PI_Adapt_Select(hVaAction, pInst->stActionList[nActionId].bConnect);

		// select되기전에는 pmt를 전달 못했을 것이므로, 여기서 전달한다.
		if (pInst->stActionList[nActionId].bConnect == TRUE && pInst->stActionList[nActionId].bPmt == TRUE)
		{
			if ((pstPmtInfo = svc_cas_MgrSubGetPmtInfo(hAction)) == NULL)
			{
				HxLOG_Error("error in svc_cas_MgrSubGetPmtInfo\n");
				return;
			}

//			VA_PI_Adapt_PmtChange(hVaAction, pstPmtInfo);
		}
	}
	HxLOG_Print("[Exit]\n");
}

// 마지막으로 보냈던 메시지를 얻어간다.
STATIC void local_vaact_CheckLastMsg(Handle_t hAction, SvcCas_MsgType_e *peMsgType, DxCAS_GroupId_e *peCasId, void **ppMessage, SvcCas_ShowType_e *peShowType)
{
	HxLOG_Print("[Enter]\n");

	HxLOG_Print("[Exit]\n");
}

HERROR VAACT_UpdateCasInfo(HUINT16 usScSlot, HUINT16 *pusCaSysId, HUINT32 ulNumCaSysId, HUINT32 ulMaxNumSvc, HBOOL bAvailable)
{
	VaInstance_t		*pInst;

	HxLOG_Print("[Enter]\n");

	pInst = VAMGR_GetInstance();

	if (svc_cas_MgrSubUpdateCasInfo(pInst->hCasInst,
								eDxCAS_GROUPID_VA,
								usScSlot,
								pusCaSysId,
								ulNumCaSysId,
								ulMaxNumSvc,
								bAvailable) != ERR_OK)

	{
		HxLOG_Error("error in svc_cas_MgrSubUpdateCasInfo\n");
		return ERR_FAIL;
	}

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}

#if defined(CONFIG_MW_CAS_VIACCESS_PVR)
DxCopyrightControl_Viaccess_t	s_stVaDrmInfo;
HERROR VAACT_UpdateDrmInfo(Handle_t hAction, HBOOL bRecordForbidden, HBOOL bTsrForbidden)
{
	VaInstance_t		*pInst;
	DxCopyrightControl_Viaccess_t	drm_info;

	HxLOG_Print("[Enter]\n");

	pInst = VAMGR_GetInstance();

	if (0) //bRecordForbidden == FALSE && bTsrForbidden == FALSE)
	{
		if (CASMGR_UpdateDrmInfo(hAction, pInst->hCasInst, NULL) != ERR_OK)
		{
			HxLOG_Print("error in CASMGR_UpdateDrmInfo\n");
			return ERR_FAIL;
		}
	}
	else
	{
		s_stVaDrmInfo.bRecordForbidden = bRecordForbidden;
		s_stVaDrmInfo.bTsrForbidden = bTsrForbidden;

		if (CASMGR_UpdateDrmInfo(hAction, pInst->hCasInst, (void *)&s_stVaDrmInfo) != ERR_OK)
		{
			HxLOG_Print("error in CASMGR_UpdateDrmInfo\n");
			return ERR_FAIL;
		}
	}

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}
#endif

HERROR VAACT_Init(void)
{
	VaInstance_t	*pInst;
	HUINT16			usCaSysId = 0x500;
	HUINT32			ulSlot = 0;

	pInst = VAMGR_GetInstance();
	pInst->stCbList.pfnCbStartSession 	= local_vaact_StartSession;
	pInst->stCbList.pfnCbStopSession 	= local_vaact_StopSession;
//	pInst->stCbList.pfnCbPauseSession 	= NULL;
//	pInst->stCbList.pfnCbResumeSession 	= NULL;
	pInst->stCbList.pfnCbSetDSC 		= local_vaact_SetPid;
	pInst->stCbList.pfnCbUpdatePmt 		= local_vaact_UpdatePmt;
	pInst->stCbList.pfnCbUpdateCat		= local_vaact_UpdateCat;
	pInst->stCbList.pfnCbUpdateSdt 		= NULL;
	pInst->stCbList.pfnCbUpdateNit 		= NULL;
	pInst->stCbList.pfnCbUpdateEit		= NULL;
	pInst->stCbList.pfnCbInstSelected	= local_vaact_ManageInstance;
	pInst->stCbList.pfnCbCheckLastMsg 	= local_vaact_CheckLastMsg;

//  TODO: 여러개의 sub callback 연결시를 고민 해봐야 한다.
//			우선은 slot 0 에 개수 하나로 setting 한다.
	if (svc_cas_MgrSubRegisterCb(eCasInst_Embedded,
									eDxCAS_GROUPID_VA,
									ulSlot,
									&pInst->stCbList,
									1,
									&pInst->hCasInst) != ERR_OK)

	{
		HxLOG_Error("\r\n");
		return ERR_FAIL;
	}

#ifdef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST
	VAACT_UpdateCasInfo((HUINT16)0, &usCaSysId, (HUINT32)1, (HUINT32)VA_MAIN_ACS_NUM, TRUE);
#else
	VAACT_UpdateCasInfo((HUINT16)0, &usCaSysId, (HUINT32)1, (HUINT32)VA_MAIN_ACS_NUM, FALSE);
#endif

	HxLOG_Print("[Exit]\n");
	return ERR_OK;
}


/*********************** End of File ******************************/

