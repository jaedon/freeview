/**
	@file     _svc_cas_mgr_instance.c
	@brief    _svc_cas_mgr_instance.c (CAS Service)

	Description:
	Module:  \n
	Remarks :\n

	Copyright (c) 2012 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/
/********************************************************************
* Header Files
********************************************************************/
#define ___01_Header_Files___

#include <octo_common.h>
#include <util.h>

#include <svc_cas.h>
#include <_svc_cas_sub_api.h>

#include <_svc_cas_mgr_common.h>
#include <_svc_cas_mgr_svc_port.h>
#include <_svc_cas_mgr_pal_port.h>
#include <_svc_cas_mgr_main.h>
#include <_svc_cas_mgr_instance.h>

/********************************************************************
* For Debugging
********************************************************************/
#define ___02_Debugging___


/********************************************************************
* Local Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Local_Definitions___


/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___04_Extern_Vars_Func_Prototypes___

/********************************************************************
* Static Variables / Function Prototypes
********************************************************************/
#define ___05_Local_Vars_Func_Prototypes___

/**/
STATIC HUINT32				s_ulNumOfRegisteredInst;
STATIC SvcCas_DevInstance_t	*s_pstCasDevInst = NULL;


#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
typedef enum
{
	eSession_init = 0x00,
	eSession_started = 0x01,
	eSession_conneted = 0x02,
	eSession_setDesPid = 0x04,
	eSession_pmt_recieved = 0x08
} SESSION_STATUS;

STATIC HBOOL s_bTunerStateOf2nd = TRUE; // FALSE;
STATIC HUINT8 s_ucSessionStatusof1st = eSession_init;
STATIC SvcCas_SetPidInfo_t s_stCasSetPidInfo;
#endif

#if defined(CONFIG_DEBUG)
#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
STATIC const HINT8 *svc_cas_instance_MakeStrContextState(SvcCas_State_e eState);
STATIC const HINT8 *svc_cas_instance_MakeStrActionType(SvcCas_ActionType_e cmd);
#endif
STATIC HINT8 *svc_cas_instance_MakeStrCasInstType(SvcCas_InstanceType_e eType);
STATIC HINT8 *svc_cas_instance_MakeStrCamState(SvcCas_CamState_e eCAS_CamState);
#endif

/********************************************************************
* Static Functions
* Naming rule : "local"_"CasMgrInstance" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___

STATIC void svc_cas_instance_InitCasDevInst(void)
{
	HUINT32				i, j, k;
	SvcCas_DevInstance_t	*pstCasDevInst;

	VK_memset32(s_pstCasDevInst, 0, (sizeof(SvcCas_DevInstance_t) * CAS_MAX_NUM_OF_INSTANCE));

	for(i = 0; i < CAS_MAX_NUM_OF_INSTANCE; i++)
	{
		pstCasDevInst = &s_pstCasDevInst[i];

		pstCasDevInst->eLastNotify = eCAS_NOTI_Max;

		for(j = 0; j < CAS_MAX_NUM_OF_ACTION; j++)
		{
			pstCasDevInst->aulActionId[j] = HANDLE_NULL;
		}

		for(j = 0; j < CAS_MAX_NUM_OF_SLOT; j++)
		{
			for(k = 0; k < CAS_MAX_NUM_OF_CA_SYSID; k++)
			{
				pstCasDevInst->ausCaSysId[j][k] = 0xFFFF;
			}
		}
	}
}

STATIC SvcCas_DevInstance_t *svc_cas_instance_GetCasDevInst(HUINT32 ulInstId)
{
	if(s_pstCasDevInst == NULL)
	{
		HxLOG_Error("s_pstCasDevInst == NULL >>>\n");
		return NULL;
	}

	return &s_pstCasDevInst[ulInstId];
}

#if defined(CONFIG_MW_CI)
/* 요건 아직 CI에서만 사용하도록 하자.. */
STATIC SvcCas_DevInstance_t *svc_cas_instance_GetCasDevInstFromCasGIDNSlot(SvcCas_InstanceType_e eCASInstType, HUINT32 ulSlot)
{
	HUINT32				i;
	SvcCas_DevInstance_t	*pstCasDevInst = NULL;

	if(s_pstCasDevInst == NULL)
	{
		HxLOG_Error("s_pstCasDevInst == NULL >>>\n");
		return NULL;
	}

	if(s_ulNumOfRegisteredInst == 0)
	{
		HxLOG_Error("s_ulNumOfRegisteredInst is zero >>>\n");
		return NULL;
	}

	for(i = 0; i < s_ulNumOfRegisteredInst; i++)
	{
		pstCasDevInst = &s_pstCasDevInst[i];
		if(		(eCASInstType == pstCasDevInst->eCasInstType)
			&& 	(pstCasDevInst->ulSlot == ulSlot))
		{
			break;
		}
	}

	if(i >= s_ulNumOfRegisteredInst)
	{
		return NULL;
	}

	return pstCasDevInst;
}
#endif

#if defined(CONFIG_MW_CAS_NAGRA_DALTEST) && defined(CONFIG_MW_MM_PVR)
STATIC HERROR	svc_cas_instance_NotifyCallback_PreExtProcess_NagraMediaPVR(SvcCas_InstNotiType_e eNotiType, HUINT32 ulCasInstanceId, SvcCas_Context_t *pstContext)
{
	if ( (GET_ACTION_ID(pstContext->hAction) == eActionId_VIEW_FIRST + 1) || (GET_ACTION_ID(pstContext->hAction) == eActionId_REC_FIRST + 0) )
		return ERR_FAIL;

	return ERR_OK;
}

HERROR	svc_cas_instance_NotifyCallback_PostExtProcess_NagraMediaPVR(SvcCas_InstNotiType_e eNotiType, HUINT32 ulCasInstanceId, SvcCas_Context_t *pstContext)
{
	HERROR				nResult	= ERR_FAIL;
	SvcCas_DevInstance_t	*pstCasInst  = NULL;

	pstCasInst = svc_cas_instance_GetCasDevInst(ulCasInstanceId);
	if(pstCasInst == NULL)
	{
		HxLOG_Error("handle\n");
		return ERR_FAIL;
	}

	switch(eNotiType)
	{
	case eCAS_NOTI_Start:
		if(pstCasInst->stCbList.pfnCbStartSession != NULL)
		{
			s_ucSessionStatusof1st = eSession_started;
			if (s_bTunerStateOf2nd && pstContext->eActType == eCAS_ActionType_Live)
			{
				Handle_t act;
				// Veiw 0
				act = svc_cas_SvcPortRmMakeActionHandle(eActionId_VIEW_1, 0);
				pstCasInst->stCbList.pfnCbStartSession(act, pstContext->hSvc, pstContext->eActType);

#if (0)
				//Rec 0
				act = PAL_PIPE_GetActionHandle(eActionId_REC_0);
				pstCasInst->stCbList.pfnCbStartSession(act, pstContext->hSvc, pstContext->eActType);
#endif
			}
		}
		break;

	case eCAS_NOTI_Stop:
		if(pstCasInst->stCbList.pfnCbStopSession != NULL)
		{
			s_ucSessionStatusof1st = eSession_init;
			if (s_bTunerStateOf2nd && pstContext->eActType == eCAS_ActionType_Live)
			{
				Handle_t act;
				// View 0
				act = svc_cas_SvcPortRmMakeActionHandle(eActionId_VIEW_1, 0);
				pstCasInst->stCbList.pfnCbStopSession(act,pstContext->hNextSvc);

#if (0)
				// Rec 0
				act = PAL_PIPE_GetActionHandle(eActionId_REC_0);
				pstCasInst->stCbList.pfnCbStopSession(act,pstContext->hNextSvc);
#endif
			}
		}
		break;

	case eCAS_NOTI_SetDsc:
		if(pstCasInst->stCbList.pfnCbSetDSC != NULL)
		{
			s_ucSessionStatusof1st |= eSession_setDesPid;
			HxSTD_memset(&s_stCasSetPidInfo, 0, sizeof(SvcCas_SetPidInfo_t));
			HxSTD_memcpy(&s_stCasSetPidInfo, &pstContext->stCaDscInfo, sizeof(SvcCas_SetPidInfo_t));
			if (s_bTunerStateOf2nd && pstContext->eActType == eCAS_ActionType_Live)
			{
				Handle_t act;
				// View 0
				act = svc_cas_SvcPortRmMakeActionHandle(eActionId_VIEW_1, 0);
				pstCasInst->stCbList.pfnCbSetDSC(act, &pstContext->stCaDscInfo);

#if (0)
				// Rec 0
				act = PAL_PIPE_GetActionHandle(eActionId_REC_0);
				pstCasInst->stCbList.pfnCbSetDSC(act, &pstContext->stCaDscInfo);
#endif
			}
		}
		break;

	case eCAS_NOTI_UpdatePmt:
		if(pstCasInst->stCbList.pfnCbUpdatePmt != NULL)
		{
			s_ucSessionStatusof1st |= eSession_pmt_recieved;
			if (s_bTunerStateOf2nd && pstContext->eActType == eCAS_ActionType_Live)
			{
				Handle_t act;
				// View 0
				act = svc_cas_SvcPortRmMakeActionHandle(eActionId_VIEW_1, 0);
				pstCasInst->stCbList.pfnCbUpdatePmt(act, &pstContext->aucPmtRaw[0]);

#if (0)
				// Rec 0
				act = PAL_PIPE_GetActionHandle(eActionId_REC_0);
				pstCasInst->stCbList.pfnCbUpdatePmt(act, &pstContext->aucPmtRaw[0]);
#endif
			}
		}
		break;

	case eCAS_NOTI_ManageInst:
		if(pstCasInst->stCbList.pfnCbInstSelected != NULL)
		{
			if (pstContext->bConnect)
				s_ucSessionStatusof1st |= eSession_conneted;
			else
				s_ucSessionStatusof1st = eSession_init;
			if (s_bTunerStateOf2nd && pstContext->eActType == eCAS_ActionType_Live)
			{
				Handle_t act;
				// View 0
				act = svc_cas_SvcPortRmMakeActionHandle(eActionId_VIEW_1, 0);
				pstCasInst->stCbList.pfnCbInstSelected(act, pstContext->ulCasInstanceId, pstContext->bConnect);
#if (0)
				// Rec 0
				act = PAL_PIPE_GetActionHandle(eActionId_REC_0);
				pstCasInst->stCbList.pfnCbInstSelected(act, pstContext->ulCasInstanceId, pstContext->bConnect);
#endif
			}
		}
		break;

	case eCAS_NOTI_UpdateCat:
		if(pstCasInst->stCbList.pfnCbUpdateCat != NULL)
		{
			if (s_bTunerStateOf2nd && pstContext->eActType == eCAS_ActionType_Live)
			{
				Handle_t act;

				// View 0
				act = svc_cas_SvcPortRmMakeActionHandle(eActionId_VIEW_1, 0);
				pstCasInst->stCbList.pfnCbUpdateCat(act, pstContext->usUniqueId, &pstContext->aucCatRaw[0]);
#if (0)
				// Rec 0
				act = PAL_PIPE_GetActionHandle(eActionId_REC_0);
				pstCasInst->stCbList.pfnCbUpdateCat(act, pstContext->usUniqueId, &pstContext->aucCatRaw[0]);
#endif
			}
		}
		break;

	default:
		break;
	}

	return ERR_OK;
}

#endif

/* svc_cas_instance_NotifyCallback()의 각 eNotiType 실행 전 각 향별 / 모델별 뭔가 해줘야 하는 처리가 있다면 실행할 부분 */
STATIC HERROR	svc_cas_instance_NotifyCallback_PreExtProcess(SvcCas_InstNotiType_e eNotiType, HUINT32 ulCasInstanceId, SvcCas_Context_t *pstContext)
{
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST) && defined(CONFIG_MW_MM_PVR)
	return svc_cas_instance_NotifyCallback_PreExtProcess_NagraMediaPVR(eNotiType, ulCasInstanceId, pstContext);
#else
	return ERR_OK;
#endif // #if defined(CONFIG_MW_CAS_NAGRA_DALTEST) && defined(CONFIG_MW_MM_PVR)
}

/* svc_cas_instance_NotifyCallback()의 각 eNotiType 실행 후 각 향별 / 모델별 뭔가 해줘야 하는 처리가 있다면 실행할 부분 */
HERROR	svc_cas_instance_NotifyCallback_PostExtProcess(SvcCas_InstNotiType_e eNotiType, HUINT32 ulCasInstanceId, SvcCas_Context_t *pstContext)
{
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST) && defined(CONFIG_MW_MM_PVR)
	return svc_cas_instance_NotifyCallback_PostExtProcess_NagraMediaPVR(eNotiType, ulCasInstanceId, pstContext);
#endif // #if defined(CONFIG_MW_CAS_NAGRA_DALTEST) && defined(CONFIG_MW_MM_PVR)

	return ERR_OK;
}

#if defined(CAS_SUPPORT_NIT_MULTI)
HERROR svc_cas_instance_NotifyNitMultiSectionCallback(SvcCas_InstNotiType_e eNotiType, SvcCas_DevInstance_t *pstCasInst, SvcCas_Context_t *pstContext)
{
	HUINT8				ucLastSecNum;
	HUINT8				*pucNitMulSec = NULL;
	HUINT8				*pucTemp = NULL;
	HINT32				i;
	HUINT32				ulTotalLen = 0;
	HUINT32				ulSecLen = 0;

	if(pstCasInst->stCbList.pfnCbUpdateNit == NULL)
	{
		return ERR_FAIL;
	}

	ucLastSecNum = get8bit(pstContext->aucNitRaw[0] + 7);
	ucLastSecNum++;

	for(i = 0; i < ucLastSecNum; i++)
	{
		ulTotalLen += get12bit(pstContext->aucNitRaw[i] + 1) + 3;
	}

	pucNitMulSec = (HUINT8 *)OxCAS_Malloc(ulTotalLen );

	if(pucNitMulSec == NULL)
	{
		return ERR_FAIL;
	}

	pucTemp = pucNitMulSec;

	for(i = 0; i < ucLastSecNum; i++)
	{
		ulSecLen = get12bit(pstContext->aucNitRaw[i] + 1) + 3;

		HxSTD_memcpy(pucTemp, pstContext->aucNitRaw[i], ulSecLen);

		pucTemp += ulSecLen;
	}

	pstCasInst->eLastNotify = eCAS_NOTI_UpdateNit;
	pstCasInst->stCbList.pfnCbUpdateNit(pstContext->hAction, pstContext->usUniqueId, pucNitMulSec);

	OxCAS_Free(pucNitMulSec);

	return	ERR_OK;
}
#endif

STATIC HERROR svc_cas_instance_NotifyCallback(SvcCas_InstNotiType_e eNotiType, HUINT32 ulCasInstId, SvcCas_DevInstance_t *pstCasInst, SvcCas_Context_t *pstContext)
{
	HERROR	hErr;

	if (eNotiType == eCAS_NOTI_ManageInst)
	{
		if(pstContext->ulCasInstanceId != ulCasInstId)
		{
			HxLOG_Error("instance id\n");
			return ERR_FAIL;
		}
	}

	/* 기본 체크 외에 Notify 전 향별/모델별 체크 */
	hErr = svc_cas_instance_NotifyCallback_PreExtProcess(eNotiType, ulCasInstId, pstContext);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	HxLOG_Info("pstContext->hAction: 0x%x \n", pstContext->hAction);

	switch(eNotiType)
	{
	case eCAS_NOTI_Start:
		if(pstCasInst->stCbList.pfnCbStartSession != NULL)
		{
			pstCasInst->eLastNotify = eCAS_NOTI_Start;
			pstCasInst->stCbList.pfnCbStartSession(pstContext->hAction, pstContext->hSvc, pstContext->eActType);
		}
		break;

	case eCAS_NOTI_Stop:
		if(pstCasInst->stCbList.pfnCbStopSession != NULL)
		{
			pstCasInst->eLastNotify = eCAS_NOTI_Stop;
			pstCasInst->stCbList.pfnCbStopSession(pstContext->hAction, pstContext->hNextSvc);
		}
		break;

	case eCAS_NOTI_SetDsc:
		if(pstCasInst->stCbList.pfnCbSetDSC != NULL)
		{
			pstCasInst->eLastNotify = eCAS_NOTI_SetDsc;
			pstCasInst->stCbList.pfnCbSetDSC(pstContext->hAction, &pstContext->stCaDscInfo);
		}
		break;

	case eCAS_NOTI_UpdatePmt:
		if(pstCasInst->stCbList.pfnCbUpdatePmt != NULL)
		{
			pstCasInst->eLastNotify = eCAS_NOTI_UpdatePmt;
			pstCasInst->stCbList.pfnCbUpdatePmt(pstContext->hAction, &pstContext->aucPmtRaw[0]);
		}
		break;

	case eCAS_NOTI_ManageInst:
		if(pstCasInst->stCbList.pfnCbInstSelected != NULL)
		{
			pstCasInst->eLastNotify = eCAS_NOTI_ManageInst;
			pstCasInst->stCbList.pfnCbInstSelected(pstContext->hAction, pstContext->ulCasInstanceId, pstContext->bConnect);
		}
		break;

#if defined (CAS_SUPPORT_CAT)
	case eCAS_NOTI_UpdateCat:
		if(pstCasInst->stCbList.pfnCbUpdateCat != NULL)
		{
			pstCasInst->eLastNotify = eCAS_NOTI_UpdateCat;
			pstCasInst->stCbList.pfnCbUpdateCat(pstContext->hAction, pstContext->usUniqueId, &pstContext->aucCatRaw[0]);
		}
		break;
#endif

#if defined (CAS_SUPPORT_SDT)
	case eCAS_NOTI_UpdateSdt:
		if(pstCasInst->stCbList.pfnCbUpdateSdt != NULL)
		{
			pstCasInst->eLastNotify = eCAS_NOTI_UpdateSdt;
			pstCasInst->stCbList.pfnCbUpdateSdt(pstContext->hAction, pstContext->usUniqueId, pstContext->aucSdtRaw);
		}
		break;
#endif

#if defined(CAS_SUPPORT_NIT)
	case eCAS_NOTI_UpdateNit:
	#if defined(CAS_SUPPORT_NIT_MULTI)
		svc_cas_instance_NotifyNitMultiSectionCallback( eNotiType, ulCasInstId, pstContext);
	#else
		if(pstCasInst->stCbList.pfnCbUpdateNit != NULL)
		{
			pstCasInst->eLastNotify = eCAS_NOTI_UpdateNit;
			pstCasInst->stCbList.pfnCbUpdateNit(pstContext->hAction, pstContext->usUniqueId, pstContext->aucNitRaw[0]);
		}
		break;
	#endif
#endif

#if defined(CAS_SUPPORT_EIT)
	case eCAS_NOTI_UpdateEit:
		if(pstCasInst->stCbList.pfnCbUpdateEit != NULL)
		{
			pstCasInst->eLastNotify = eCAS_NOTI_UpdateEit;
			pstCasInst->stCbList.pfnCbUpdateEit(pstContext->hAction, pstContext->usUniqueId, pstContext->aucEitRaw[0], pstContext->aucEitRaw[1]);
		}
		break;
#endif

#if defined(CAS_SUPPORT_TDT)
	case eCAS_NOTI_UpdateTdt:
		if(pstCasInst->stCbList.pfnCbUpdateTdt != NULL)
		{
			pstCasInst->eLastNotify = eCAS_NOTI_UpdateTdt;
			pstCasInst->stCbList.pfnCbUpdateTdt(pstContext->hAction, pstContext->usUniqueId, pstContext->aucTdtRaw);
		}
		break;
#endif

#if defined(CAS_SUPPORT_BAT)
	case eCAS_NOTI_UpdateBat:
		if(pstCasInst->stCbList.pfnCbUpdateBat != NULL)
		{
			pstCasInst->eLastNotify = eCAS_NOTI_UpdateBat;
			pstCasInst->stCbList.pfnCbUpdateBat(pstContext->hAction, pstContext->usUniqueId, pstContext->aucBatRaw[0]);
		}
		break;
#endif

	default:
		HxLOG_Error("Unknown Event\n");
		return ERR_FAIL;
	}

	/* 기본 수행 외에 향별/모델별 후행 처리 */
	svc_cas_instance_NotifyCallback_PostExtProcess(eNotiType, ulCasInstId, pstContext);

	return ERR_OK;
}

HERROR svc_cas_instance_RegisterCb(	SvcCas_InstanceType_e	eCasInstType,
												DxCAS_GroupId_e			eCasId,
												HUINT32				ulSlot,
												SvcCas_CbFuncList_t	*pCbList,
												HUINT32				ulNumOfSlot,
												HUINT32				*pulCasInstanceId)
{
	HINT32 i = 0, nSlotCount = 0;
	SvcCas_DevInstance_t *pstCasInst = NULL;

#if defined(CONFIG_DEBUG)
	HxLOG_Info("instance type (%s), CaSystemId(%d) \n", svc_cas_instance_MakeStrCasInstType(eCasInstType), eCasId);
#endif

	if(pulCasInstanceId == NULL || pCbList == NULL)
	{
		HxLOG_Error("Null pointer\n");
		return ERR_FAIL;
	}

	*pulCasInstanceId = HANDLE_NULL;

	HxLOG_Info("\n\n====================>[svc_cas_instance_RegisterCb1] \n");
	for(i=0; i<CAS_MAX_NUM_OF_INSTANCE; i++)
	{
		HxLOG_Info("\n\n====================>[svc_cas_instance_RegisterCb2] \n");

		// Instance Table에서 Index로 Instance를 Get.
		pstCasInst = svc_cas_instance_GetCasDevInst(i);
		if(pstCasInst == NULL)
		{
			continue;
		}

		HxLOG_Info("\n\n====================>[svc_cas_instance_RegisterCb3] \n");
		// is this cas instance allocated by another sub cas?
		if(pstCasInst->bAllocated == TRUE)
		{
			continue;
		}

		HxLOG_Info("\n\n====================>[svc_cas_instance_RegisterCb4 \n");
		if(eCasInstType == eCasInst_Embedded || eCasInstType == eCasInst_CI)
		{
			pstCasInst->eCasInstType = eCasInstType;

			/*
				2009.06.29
				sub cas 등록 시점에서는 ca system id를 모른다.
				detail 한 ca system id는 smart card inserted, cam inserted .
			*/
			//Ca System Id Copy
			for(nSlotCount = 0; nSlotCount < CAS_MAX_NUM_OF_SLOT; nSlotCount++)
			{
				pstCasInst->ulNumCaSysId[nSlotCount] = 0;
			}

			//Callback 등록.
			pstCasInst->stCbList	= *pCbList;
			pstCasInst->eCasId	= eCasId;
			pstCasInst->ulSlot		= ulSlot;

			*pulCasInstanceId = i;				// Instance의 Table In로 전달해도 된다.
			HxLOG_Info("(%d)-th instance allocated.\n", i);

			// cas instance table에 등록 됨.
			pstCasInst->bAllocated = TRUE;
			pstCasInst->ulNumOfSlot = ulNumOfSlot;
			s_ulNumOfRegisteredInst++;

			break;
		}
		else
		{
			// ERROR.
		}
	}

	if(*pulCasInstanceId == HANDLE_NULL)
	{
		HxLOG_Error("not allocated.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}


#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
STATIC HUINT32 svc_cas_instance_GetNumberOfExistCam(void)
{
	HUINT32 						i;
	HUINT32						unNumberOfExistCam = 0;
	SvcCas_DevCiState_e				eCiState;
	SvcCas_DevInstance_t			*pstDevInst;

	for(i = 0; i < s_ulNumOfRegisteredInst; i++)
	{
		pstDevInst = &s_pstCasDevInst[i];

		HxLOG_Info("index = %d, slot = %d, eCasInstType = %d, eCamState = %d\n", i,
			pstDevInst->ulSlot, pstDevInst->eCasInstType, pstDevInst->eCamState);

		/* Cas instance type 이 CI 이고 Cam이 초기화 되었는데
			참고하는 녀석이 없으면 path가 연결이 안된거다... 연결 시켜 주자~~~!!
		*/
		if( 	(pstDevInst->eCasInstType == eCasInst_CI)
			&&	(pstDevInst->eCamState == eCAS_CAM_STATE_Initialized))
		{
			eCiState = svc_cas_PalPortCiCheckModule(pstDevInst->ulSlot);
			HxLOG_Info("GetNumberOfExistCam - ulSlot = %d, eCiState = %d\n", pstDevInst->ulSlot, eCiState);
			if(eCiState == eCAS_DEV_CI_INSERTED)
			{
				unNumberOfExistCam++;
			}
		}
	}

	return unNumberOfExistCam;
}

STATIC SvcCas_DevInstance_t *svc_cas_instance_FindNotConnectPathCi(void)
{
	HUINT32						i;
	SvcCas_DevCiState_e				eCiState;
	SvcCas_DevInstance_t			*pstDevInst;

	for(i = 0; i < s_ulNumOfRegisteredInst; i++)
	{
		pstDevInst = &s_pstCasDevInst[i];

		#if defined(CONFIG_DEBUG)
		HxLOG_Info("FindNotConnectPathCi - index = %d, ulSlot = %d, eCasInstType = %s, eCamState = %s, ulCurRefCnt = %d\n", i,
			pstDevInst->ulSlot,
			svc_cas_instance_MakeStrCasInstType(pstDevInst->eCasInstType),
			svc_cas_instance_MakeStrCamState(pstDevInst->eCamState),
			pstDevInst->ulCurRefCnt);
		#endif

		/* Cas instance type 이 CI 이고 Cam이 초기화 되었는데
			참고하는 녀석이 없으면 path가 연결이 안된거다... 연결 시켜 주자~~~!!
		*/
 		if(		(pstDevInst->eCasInstType == eCasInst_CI)
			&&	(pstDevInst->eCamState == eCAS_CAM_STATE_Initialized)
			&&	(pstDevInst->ulCurRefCnt == 0))
		{
			eCiState = svc_cas_PalPortCiCheckModule(pstDevInst->ulSlot);
			HxLOG_Info("FindNotConnectPathCi - ulSlot = %d, eCiState = %d\n", pstDevInst->ulSlot, eCiState);
			if(eCiState == eCAS_DEV_CI_INSERTED)
			{
				return pstDevInst;
			}
		}
	}

	return NULL;
}

STATIC SvcCas_DevInstance_t *svc_cas_instance_FindConnectPathCi(void)
{
	HUINT32						i;
	SvcCas_DevCiState_e				eCiState;
	SvcCas_DevInstance_t			*pstDevInst;

	for(i = 0; i < s_ulNumOfRegisteredInst; i++)
	{
		pstDevInst = &s_pstCasDevInst[i];

		#if defined(CONFIG_DEBUG)
		HxLOG_Info("FindConnectPathCi - index = %d, ulSlot = %d, eCasInstType = %s, eCamState = %s, ulCurRefCnt = %d\n", i,
			pstDevInst->ulSlot,
			svc_cas_instance_MakeStrCasInstType(pstDevInst->eCasInstType),
			svc_cas_instance_MakeStrCamState(pstDevInst->eCamState),
			pstDevInst->ulCurRefCnt);
		#endif

		/* Cas instance type 이 CI 이고 Cam이 초기화 되었고	참고하는 녀석이 있으면 연결 된 거다..
		*/
 		if(		(pstDevInst->eCasInstType == eCasInst_CI)
			&&	(pstDevInst->eCamState == eCAS_CAM_STATE_Initialized)
			&&	(pstDevInst->ulCurRefCnt != 0))
		{
			eCiState = svc_cas_PalPortCiCheckModule(pstDevInst->ulSlot);
			HxLOG_Info("FindConnectPathCi - ulSlot = %d, eCiState = %d\n", pstDevInst->ulSlot, eCiState);
			if(eCiState == eCAS_DEV_CI_INSERTED)
			{
				return pstDevInst;
			}
		}
	}

	return NULL;
}

#define CAPMT_APDU_TAG							0x9F8032
#define CaPmtHeaderSize								9 //(ca_pmt_tag 3, ca_pmt_list_management 1, program_number 2, ver+cur 1, program_info_length 2 - without length)
#define CapmtEsHeaderSize							5

#define CaPmtListManagement_Only					3

STATIC HUINT8 svc_cas_instance_GetLengthField(HUINT8 *pucLenField, unsigned short usLen)
{
	if (usLen <= 127)
	{
		*pucLenField = (HUINT8)usLen;
		return 1;
	}
	else if (usLen <= 255)
	{
		*pucLenField++ = 0x81;
		*pucLenField = (HUINT8)usLen;
		return 2;
	}
	else // if (usLen <= 65535) : ca_pmt 를 만들 때 이 이상의 처리는 사실 불필요하므로 고려하지 않는다.
	{
		*pucLenField++ = 0x82;
		*pucLenField++ = (HUINT8)((usLen >> 8) & 0x00ff);
		*pucLenField = (HUINT8)(usLen & 0x00ff);
		return 3;
	}
}

STATIC void svc_cas_instance_MakeClearCaPmt(SvcCas_PmtInfo_t *pstPmtInfo, HUINT8 *pucRetClearCaPmt, HUINT16 *pusRetRawLen)
{
	HUINT8				aucLengthField[3];
	HUINT8				ucLengthFieldSize;
	HUINT16				usIdx = 0, i = 0;
	HUINT16 				usTotalLen = 0;
	SvcCas_PmtElement_t		*pPmtElement;

	//calculate ca pmt total length
	usTotalLen = (CaPmtHeaderSize + (pstPmtInfo->ulNumOfElement * CapmtEsHeaderSize));
	ucLengthFieldSize = svc_cas_instance_GetLengthField(aucLengthField, usTotalLen-3); // -ca_pmt_tag 3
	usTotalLen += ucLengthFieldSize;

	/* ca_pmt_tag (24 bits) */
	pucRetClearCaPmt[usIdx++] = (HUINT8)((CAPMT_APDU_TAG >> 16) & 0xFF);
	pucRetClearCaPmt[usIdx++] = (HUINT8)((CAPMT_APDU_TAG >> 8) & 0xFF);
	pucRetClearCaPmt[usIdx++] = (HUINT8)(CAPMT_APDU_TAG & 0xFF);

	/* length field */
	HxSTD_memcpy(&pucRetClearCaPmt[usIdx], aucLengthField, ucLengthFieldSize);
	usIdx += ucLengthFieldSize;

	/* ca_pmt_list_managrment (8 bits) */
	pucRetClearCaPmt[usIdx++] = (HUINT8)CaPmtListManagement_Only;

	/* program number 16 bits*/
	pucRetClearCaPmt[usIdx++]  = (HUINT8)((pstPmtInfo->usServiceId  >> 8) & 0xFF);
	pucRetClearCaPmt[usIdx++]  = (HUINT8)(pstPmtInfo->usServiceId & 0xFF);

	/* reserved (2 bits) + version_number (5 bits) + current_next_indicator (1 bit) */
	pucRetClearCaPmt[usIdx] = (HUINT8)((pstPmtInfo->ucVerNum << 1) & 0x3e);
	pucRetClearCaPmt[usIdx++] |= (HUINT8)(pstPmtInfo->bCurNextInd & 0x01);

	/* reserved (4 bits) + program_info_length (12 bits) */
	pucRetClearCaPmt[usIdx++] = 0;
	pucRetClearCaPmt[usIdx++] = 0;

	for(i = 0; i < pstPmtInfo->ulNumOfElement && i < CAS_MAX_NUM_OF_ES; i++)
	{
		pPmtElement = &pstPmtInfo->stElementInfo[i];

		/* stream_type (8 bits) */
		pucRetClearCaPmt[usIdx++] = pPmtElement->ucStreamType;

		/* reserved (3 bits) + elementary_PID (13 bits) */
		pucRetClearCaPmt[usIdx++] = (HUINT8)((pPmtElement->usPid >> 8) & 0x1f);
		pucRetClearCaPmt[usIdx++] = (HUINT8)(pPmtElement->usPid & 0xff);

		/* reserved (4 bits) + ES_info_length (12 bits) */
		pucRetClearCaPmt[usIdx++] = 0;
		pucRetClearCaPmt[usIdx++] = 0;
	}

	*pusRetRawLen = usTotalLen;

	return;
}

STATIC void svc_cas_instance_SendClearCaPmt(SvcCas_Context_t *pstCasActionContext, HUINT32 ulSlot)
{
	STATIC HUINT8	s_aucPmtRaw[CAS_MAX_SECTION_LENGTH];	// 동일한 CA PMT를 중복해서 보내는 것 방지하기 위하여 추가.
															// DTVLab 인증시 동일한 CA PMT를 add로 날리면 인증 CAM에서 error 발생하므로,
															// 동작상에 문제가 없더라도 이 부분을 예외 처리 하도록 함.
	HUINT8			aucPmtRaw[CAS_MAX_SECTION_LENGTH];
	HUINT16			usRawLen;
	HERROR			hErr;

	svc_cas_instance_MakeClearCaPmt(&pstCasActionContext->stPmtInfo, aucPmtRaw, &usRawLen);

	if(HxSTD_memcmp(s_aucPmtRaw, aucPmtRaw, usRawLen) == 0)
		return;

	HxSTD_memcpy(s_aucPmtRaw, aucPmtRaw, usRawLen);

	HxLOG_Info("send Clear Ca Pmt\n");
	hErr = svc_cas_SendCaPmt(ulSlot, aucPmtRaw, usRawLen);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("xsvc_cas_SendCaPmt() failed \n");
	}

	return;
}

STATIC HUINT32 svc_cas_instance_CountNotConnectPathCi(void)
{
	HUINT32					i;
	HUINT32					unRetCount = 0;
	SvcCas_DevCiState_e			eCiState;
	SvcCas_DevInstance_t		*pstDevInst;

	for(i = 0; i < s_ulNumOfRegisteredInst; i++)
	{
		pstDevInst = &s_pstCasDevInst[i];

		/* Cas instance type 이 CI 이고 Cam이 초기화 되었는데
			참고하는 녀석이 없으면 path가 연결이 안된거다... 연결 시켜 주자~~~!!
		*/
 		if(		(pstDevInst->eCasInstType == eCasInst_CI)
			&&	(pstDevInst->eCamState == eCAS_CAM_STATE_Initialized)
			&&	(pstDevInst->ulCurRefCnt == 0))
		{
			eCiState = svc_cas_PalPortCiCheckModule(pstDevInst->ulSlot);
			HxLOG_Info("ulSlot = %d, eCiState = %d\n", pstDevInst->ulSlot, eCiState);
			if(eCiState == eCAS_DEV_CI_INSERTED)
			{
				unRetCount++;
			}
		}
	}

	return unRetCount;
}

STATIC void svc_cas_instance_DisconnectAllCiPath(void)
{
	HINT32						nCI_Id;
	HUINT32						i;
	HERROR						hErr;
	SvcCas_Context_t				*pstCasActionContext;

	for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		pstCasActionContext = svc_cas_MgrGetActionContextByActionId(i);
		if(pstCasActionContext == NULL || pstCasActionContext->state == eCAS_State_Idle)
		{
			HxLOG_Info("Index(%d) continue\n", i);
			continue;
		}

		hErr = svc_cas_PalPortCiGetPath(pstCasActionContext->ulDemuxId, &nCI_Id);
		if(hErr != ERR_OK || nCI_Id == -1)
		{
			HxLOG_Info("Index(%d) nCI_Id = %d continue\n", i, nCI_Id);
			continue;
		}

		HxLOG_Info("svc_cas_PalPortCiPlugInCi() actionId(0x%x), Index(%d) -1\n", pstCasActionContext->ulActionId, i);
		hErr = svc_cas_PalPortCiPlugInCi(pstCasActionContext->ulActionId, -1);
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("ci set path error\n");
		}

	}

}


#endif

/********************************************************************
* Module Internal Functions
* Naming rule : "CasMgrInstance" _ "Function Name"
********************************************************************/
#define ___07_Module_Internal_APIs___

HERROR svc_cas_InstanceInitModule(void)
{
	if(CAS_MAX_NUM_OF_INSTANCE > 0)
	{
		s_pstCasDevInst = (SvcCas_DevInstance_t *)OxCAS_Malloc(sizeof(SvcCas_DevInstance_t) * CAS_MAX_NUM_OF_INSTANCE);

		if(s_pstCasDevInst == (SvcCas_DevInstance_t *)NULL)
		{
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		svc_cas_instance_InitCasDevInst();
	}
	else
	{
		HxLOG_Critical("Not exist CAS Instance\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

SvcCas_DevInstance_t *svc_cas_InstanceGetCasDevInst(HUINT32 ulCasInstId)
{
	if(ulCasInstId >= CAS_MAX_NUM_OF_INSTANCE)
	{
		return NULL;
	}

	return svc_cas_instance_GetCasDevInst(ulCasInstId);
}

SvcCas_InstanceType_e svc_cas_InstanceGetCasDevInstType(HUINT32 ulCasInstId)
{
	SvcCas_DevInstance_t	*pstDevInst;

	pstDevInst = svc_cas_InstanceGetCasDevInst(ulCasInstId);
	if(pstDevInst == NULL)
	{
		return eCasInst_None;
	}

	return pstDevInst->eCasInstType;
}

HBOOL svc_cas_InstanceCompareCasId(DxCAS_GroupId_e eSourceId, DxCAS_GroupId_e eTargetId)
{
	if ( eSourceId == eTargetId )
		return TRUE;

	return FALSE;
}

HBOOL svc_cas_InstanceCheckSmartCardValidityByCasId(DxCAS_GroupId_e eCasId)
{
	HUINT32 i = 0, nSlotCount = 0;
	SvcCas_DevInstance_t *pCaInst = NULL;

	for(i=0; i<CAS_MAX_NUM_OF_INSTANCE; i++)
	{
		pCaInst = svc_cas_InstanceGetCasDevInst(i);
		if(pCaInst == NULL)
		{
			continue;
		}

		if(pCaInst->eCasInstType == eCasInst_Embedded)
		{
			if ( svc_cas_InstanceCompareCasId(pCaInst->eCasId, eCasId) == TRUE &&
				(pCaInst->bAllocated == TRUE) )
			{
				for(nSlotCount = 0; nSlotCount < CAS_MAX_NUM_OF_SLOT; nSlotCount++)
				{
					if(pCaInst->bHaveDevice[nSlotCount] == TRUE)
					{
						HxLOG_Info("%d-th instance have smart card \n", i);
						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}


HUINT32 svc_cas_InstanceGetNumOfCasDevInst(void)
{
	return s_ulNumOfRegisteredInst;
}

HERROR svc_cas_InstanceGetCamState(HUINT16 usSlotId, SvcCas_CamState_e *eCamState)
{
	HUINT32				i;
	SvcCas_DevInstance_t	*pstDevInst;

	for(i = 0; i < CAS_MAX_NUM_OF_INSTANCE; i++)
	{
		pstDevInst = svc_cas_instance_GetCasDevInst(i);

		if(pstDevInst != NULL)
		{
			if( (pstDevInst->eCasInstType == eCasInst_CI) && (pstDevInst->ulSlot == usSlotId))
			{
				*eCamState = pstDevInst->eCamState;
				HxLOG_Info("%s, CAM State[%d]\n", __FUNCTION__, pstDevInst->eCamState);
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}

HERROR svc_cas_InstanceGetCamName(HUINT16 usSlotId, HUINT8 **ppszCamInfo)
{
	HUINT32 			i;
	SvcCas_DevInstance_t	*pstDevInst;

	for(i = 0; i < CAS_MAX_NUM_OF_INSTANCE; i++)
	{
		pstDevInst = svc_cas_instance_GetCasDevInst(i);
		if(pstDevInst != NULL)
		{
			if( (pstDevInst->eCasInstType == eCasInst_CI) && (pstDevInst->ulSlot == usSlotId))
			{
				HxLOG_Info("svc_cas_InstanceGetCamName() --> CamName (%s) \n", pstDevInst->stCamAppInfo.szText);
				*ppszCamInfo = pstDevInst->stCamAppInfo.szText;
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}

HERROR svc_cas_InstanceGetOpName(HUINT16 usSlotId, HUINT8 **ppszOpName)
{
	return svc_cas_CiGetOpName(usSlotId, ppszOpName);
}

HERROR svc_cas_InstanceRegisterCb(SvcCas_InstanceType_e	eCasInstType,
										DxCAS_GroupId_e			eCasId,
										HUINT32				ulSlot,
										SvcCas_CbFuncList_t	*pCbList,
										HUINT32				ulNumOfSlot,
										HUINT32				*pulCasInstanceId)
{
	return svc_cas_instance_RegisterCb(eCasInstType, eCasId, ulSlot, pCbList, ulNumOfSlot, pulCasInstanceId);
}

HERROR svc_cas_InstanceStartSession(SvcCas_Context_t *pstContext)
{
	HUINT32				i;
	HERROR				hErr;
	SvcCas_DevInstance_t	*pCasInst = NULL;

	if(pstContext == NULL)
	{
		HxLOG_Error("Context Null.\n");
		return ERR_FAIL;
	}

	if(pstContext->bStarted == TRUE)
	{
		HxLOG_Error("this session already started.\n");
		return ERR_OK;
	}

	for(i = 0; i < CAS_MAX_NUM_OF_INSTANCE; i++)
	{
		pCasInst = svc_cas_instance_GetCasDevInst(i);
		if (pCasInst == NULL)
		{
			HxLOG_Error("pCasInst == NULL >>\n");
			continue;
		}

		if(pCasInst->bAllocated == TRUE)
		{
			hErr = svc_cas_instance_NotifyCallback(eCAS_NOTI_Start, i, pCasInst, pstContext);
			if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
		}
	}

	pstContext->bStarted  = TRUE;

	return ERR_OK;
}

HERROR svc_cas_InstanceStopSession(SvcCas_Context_t *pstContext)
{
	HERROR				hErr;
	SvcCas_DevInstance_t 	*pstCasInst = NULL;
 	int i;

	if(pstContext == NULL)
	{
		HxLOG_Error("Context Null.\n");
		return ERR_FAIL;
	}

	for(i = 0; i < CAS_MAX_NUM_OF_INSTANCE; i++)
	{
		pstCasInst = svc_cas_instance_GetCasDevInst(i);
		if (pstCasInst == NULL)
		{
			continue;
		}

		if(pstCasInst->bAllocated == TRUE)
		{
			hErr = svc_cas_instance_NotifyCallback(eCAS_NOTI_Stop, i, pstCasInst, pstContext);
			if(hErr != ERR_OK)		HxLOG_Critical("Critical Eror!!!\n");
		}
	}

	return ERR_OK;
}

HERROR svc_cas_InstanceSetDeScramble(SvcCas_Context_t *pstContext)
{
	HERROR				hErr;
	SvcCas_DevInstance_t	*pstCaInst = NULL;
 	int i;

	if(pstContext == NULL)
	{
		HxLOG_Error("Context Null.\n");
		return ERR_FAIL;
	}

	/* 왜? 이렇게 했을까? 아시는 분은 답변좀 달아 주세요~ */
#if defined(CONFIG_SUPPORT_FCC) || defined(CONFIG_MW_MM_DELAYED_REC)
	/*
		FCC 적용된 모델의 경우 아래와 같이 First View가 현재 시청 중인 Main View와 같다고 보장을 할 수가 없다.
		이 경우 mgr에서 내려주는 PID와 CAS Context에서 적용되는 PID가 달라질 가능성이 높다. 물론 SUB CAS에서 SET PID 에 대한 데이터를 사용하지 않을 수도 있지만...
		그런데 아래 코드가 왜 들어 갔을까? PMT 기준으로 모든 ES PID에 대해 descrambling 후 recording를 시도하고 있도록 구현되어 있는데...

		Delayed Rec가 적용된 모델도 Live 기준으로 descramble 하게 되면 Select된 Audio만 descramble 되어 record가 된다.
	*/
#else
	if(pstContext->eActType == eCAS_ActionType_Tsr_Rec)
	{
		SvcCas_Context_t *pContext_Live;

		pContext_Live = svc_cas_MgrGetActionContextByActionId(eActionId_VIEW_FIRST);

		HxSTD_memset(&pstContext->stCaDscInfo, 0x00, sizeof(SvcCas_SetPidInfo_t));
		HxSTD_memcpy(&pstContext->stCaDscInfo, &pContext_Live->stCaDscInfo, sizeof(SvcCas_SetPidInfo_t));
	}
#endif
	//svc_cas_DbgPrintPidInfo(&pContext->stCaDscInfo);

	for(i = 0; i<CAS_MAX_NUM_OF_INSTANCE; i++)
	{
		pstCaInst = svc_cas_instance_GetCasDevInst(i);
		if (pstCaInst == NULL)
		{
			continue;
		}
		if(pstCaInst->bAllocated == TRUE)
		{
			hErr = svc_cas_instance_NotifyCallback(eCAS_NOTI_SetDsc, i, pstCaInst, pstContext);
			if(hErr != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
		}
	}

	pstContext->bDscStarted = TRUE;

	return ERR_OK;
}

HERROR svc_cas_InstanceManageCasInstance(SvcCas_Context_t *pstContext, HBOOL bConnect)
{
	HUINT32				nResult = 0;
	SvcCas_DevInstance_t	*pstCasInst = NULL;

	if(pstContext == NULL)
	{
		HxLOG_Error("Context Null.\n");
		return ERR_FAIL;
	}

	if(pstContext->ulCasInstanceId >= CAS_MAX_NUM_OF_INSTANCE)
	{
		HxLOG_Error("this session have not any instance for descrambling, pstContext->ulCasInstanceId = 0x%x\n", pstContext->ulCasInstanceId);
		return ERR_FAIL;
	}

	pstCasInst = svc_cas_instance_GetCasDevInst(pstContext->ulCasInstanceId);
	if(pstCasInst == NULL)
	{
		HxLOG_Error("svc_cas_instance_GetCasDevInst() error\n");
		return ERR_FAIL;
	}

/*	Action context의 instance idx mapping은 여기서.
*/
	pstContext->bConnect = bConnect;

	nResult = svc_cas_instance_NotifyCallback(eCAS_NOTI_ManageInst, pstContext->ulCasInstanceId, pstCasInst, pstContext);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("Notify Cb.\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR svc_cas_InstanceUpdatePmt(SvcCas_Context_t *pstContext)
{
	HUINT32				i;
	HERROR				hErr;
	SvcCas_DevInstance_t	*pstCasInst = NULL;

	if(pstContext == NULL)
	{
		HxLOG_Error("Context Null.\n");
		return ERR_FAIL;
	}

	if(pstContext->bPmtUpdated != TRUE)
	{
		HxLOG_Error("this session doesn't have any pmt \n");
		return ERR_FAIL;
	}

	for(i = 0; i<CAS_MAX_NUM_OF_INSTANCE; i++)
	{
		pstCasInst = svc_cas_instance_GetCasDevInst(i);
		if (pstCasInst == NULL)
		{
			HxLOG_Info("pstCasInst == NULL >>\n");
			continue;
		}

		if(pstCasInst->bAllocated == TRUE)
		{
			hErr = svc_cas_instance_NotifyCallback(eCAS_NOTI_UpdatePmt, i, pstCasInst, pstContext);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
		}

		// 28nm의 HW 제약사항으로 CA service의 AV가 보인 후(실제로는 descrambler가 할당된 후) Recording 동작이 되어야 한다. (descramble to re-encryption 모델)
		// DSC resource를 공유하는 구조가 아니기에 CAS OK/FAIL 이벤트를 이용해서 작업을 해야 하는 상황이며,
		// 작업방향에 따라서 XMGR REC module에서 FTA 경우라도 CAS_OK 이벤트를 받아야 REC가 되도록 할 수도 있으니 여기서 미리 보내자.
		// 각 SUB CAS에서 조건에 따라 처리하고 싶다면 아래 define은 무시하고 SUB CAS로 옮기도록 작업이 필요함
#if defined(CONFIG_IRDETO_HMX_PVR) || defined(CONFIG_MW_CAS_VIACCESS) || defined(CONFIG_MW_CAS_CONAX) || defined(CONFIG_MW_CAS_NAGRA) || defined(CONFIG_MW_CAS_VERIMATRIX)
		if(pstContext->eChType == eCasChannel_FTA)
		{
			hErr = svc_cas_MgrSubUpdateCasMessage(pstContext->hAction, eCAS_MSG_TYPE_Ok, pstCasInst->eCasId, NULL, eCasShowType_Both);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
		}
#endif
	}

	return ERR_OK;
}

#if defined (CAS_SUPPORT_CAT)
HERROR svc_cas_InstanceUpdateCat(SvcCas_Context_t *pstContext)
{
	HUINT32				i;
	HERROR				hErr;
	SvcCas_DevInstance_t	*pstCasInst = NULL;

	if(pstContext == NULL)
	{
		HxLOG_Error("Context Null.\n");
		return ERR_FAIL;
	}

	if(pstContext->bCatUpdated != TRUE)
	{
		HxLOG_Error("this session doesn't have any pmt \n");
		return ERR_FAIL;
	}

	for(i = 0; i<CAS_MAX_NUM_OF_INSTANCE; i++)
	{
		pstCasInst = svc_cas_instance_GetCasDevInst(i);
		if (pstCasInst == NULL)
		{
			HxLOG_Info("pstCasInst == NULL >>\n");
			continue;
		}

		if(pstCasInst->bAllocated == TRUE)
		{
			hErr = svc_cas_instance_NotifyCallback(eCAS_NOTI_UpdateCat, i, pstCasInst, pstContext);
			if(hErr != ERR_OK)	HxLOG_Critical("Critical Eror!!!\n");
		}
	}

	return ERR_OK;
}


#endif

#if defined (CAS_SUPPORT_SDT)
HERROR svc_cas_InstanceUpdateSdt(SvcCas_Context_t *pContext)
{
	HUINT32				i;
	HERROR				hErr;
	SvcCas_DevInstance_t	*pCaInst = NULL;

	if (pContext == NULL)
	{
		HxLOG_Error("Context Null.\n");
		return ERR_FAIL;
	}

	if(pContext->bSdtUpdated != TRUE)
	{
		HxLOG_Info("this session does not have sdt descriptor, yet\n");
		return ERR_FAIL;
	}

	for(i = 0; i<CAS_MAX_NUM_OF_INSTANCE; i++)
	{
		pCaInst = svc_cas_instance_GetCasDevInst(i);
		if (pCaInst == NULL)
		{
			continue;
		}
		if(pCaInst->bAllocated == TRUE)
		{
			hErr = svc_cas_instance_NotifyCallback(eCAS_NOTI_UpdateSdt, i, pCaInst, pContext);
			if(hErr !=ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");
		}
	}
	return ERR_OK;
}
#endif //#if defined (CAS_SUPPORT_SDT)

HERROR svc_cas_InstanceUpdateCasInfo(SvcCas_CheckDeviceMsg_t *pstMsg)
{
	SvcCas_DevInstance_t *pstCaInst = NULL;

	if(pstMsg == NULL)
	{
		HxLOG_Error("instance info is null.\n");
		return ERR_FAIL;
	}

	HxLOG_Info("instance id (%d) \n", pstMsg->ulCaInstanceId);

	pstCaInst = svc_cas_instance_GetCasDevInst(pstMsg->ulCaInstanceId);
	if(pstCaInst == NULL)
	{
		HxLOG_Error("instance id is wrong.\n");
		return ERR_FAIL;
	}

	if (pstCaInst->bAllocated != TRUE)
	{
		/* cas mgr에 등록하지 않은 sub cas의 update 요청이다. 에러지..
		*/
		HxLOG_Error("notify by not registerd sub-cas.\n");
		return ERR_FAIL;
	}

	if(pstMsg->utData.stCasInfo.ulNumCaSysId > CAS_MAX_NUM_OF_CA_SYSID)
	{
		pstCaInst->ulNumCaSysId[pstMsg->usSlotId] = CAS_MAX_NUM_OF_CA_SYSID;
	}
	else
	{
		pstCaInst->ulNumCaSysId[pstMsg->usSlotId] = pstMsg->utData.stCasInfo.ulNumCaSysId;
	}

	if(pstCaInst->ulNumCaSysId[pstMsg->usSlotId] == 0)
	{
		pstCaInst->bHaveDevice[pstMsg->usSlotId] = FALSE;
	}
	else
	{
		pstCaInst->bHaveDevice[pstMsg->usSlotId] = TRUE;
	}
	pstCaInst->ulNumOfMaxSvc = pstMsg->utData.stCasInfo.ulMaxNumSvc;
	VK_memcpy32(&pstCaInst->ausCaSysId[pstMsg->usSlotId], pstMsg->utData.stCasInfo.ausCaSysId, pstCaInst->ulNumCaSysId[pstMsg->usSlotId] * sizeof(HUINT16));

	return ERR_OK;
}

#if defined(CONFIG_MW_CI)
HERROR svc_cas_InstanceUpdateCamState(SvcCas_CheckDeviceMsg_t *pstMsg)
{
	HUINT32				i;
	HUINT32				ulCurRefCnt = 0;
	HUINT32 				aulActionId[CAS_MAX_NUM_OF_ACTION];
	SvcCas_DevInstance_t	*pstCaInst = NULL;

	if(pstMsg == NULL)
	{
		HxLOG_Error("instance info is null.\n");
		return ERR_FAIL;
	}

	pstCaInst = svc_cas_instance_GetCasDevInstFromCasGIDNSlot(eCasInst_CI, pstMsg->usSlotId);
	if(pstCaInst == NULL)
	{
		HxLOG_Error("instance id is wrong.\n");
		return ERR_FAIL;
	}

	if (pstCaInst->bAllocated != TRUE)
	{
		/* cas mgr에 등록하지 않은 sub cas의 update 요청이다. 에러지..
		*/
		HxLOG_Error("notify by not registerd sub-cas.\n");
		return ERR_FAIL;
	}

	pstCaInst->eCamState = pstMsg->utData.eCamState;
	HxLOG_Info("pstCaInst->ulSlot (%d) \n", pstCaInst->ulSlot);
	HxLOG_Info("pstCaInst->eCamState (%d) \n", pstCaInst->eCamState);

	// CAM이 제거된 상태면 연결을 해지한 후 descramble가 되지 않는 서비스를 search해서 다시 연결한다.
	if(pstCaInst->eCamState == eCAS_CAM_STATE_Removed)
	{
		VK_memset32(&pstCaInst->stCamAppInfo, 0, sizeof(SvcCas_CiAppInfo_t));

		/* disconnect하면서 pstCasInst->aulActionId[] 을 다시 정렬 하기 때문에
			미리 멈출 action Id를 저장해 놓고 disconnect 하도록 한다.
		*/
		ulCurRefCnt = pstCaInst->ulCurRefCnt;
		for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
		{
			aulActionId[i] = HANDLE_NULL;
		}

		for(i = 0; i < ulCurRefCnt; i++)
		{
			aulActionId[i] = pstCaInst->aulActionId[i];
		}

		/* 해당 CAM에 관련된 action stop
		*/
		for(i = 0 ; i < ulCurRefCnt; i++)
		{
			svc_cas_MgrDisconnectService(aulActionId[i], FALSE);
		}

		/* descramble가 되지 않는 서비스를 search해서 다시 연결한다.
		*/
// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
		svc_cas_MgrSearchNotDesCasContextNConnect(HANDLE_NULL, eDxCAS_GROUPID_CAM);
#else
		svc_cas_MgrSearchNotDesCasContextNConnect(pstMsg->ulCaInstanceId);
#endif
	}

	return ERR_OK;
}

HERROR svc_cas_InstanceUpdateCamAppInfo(SvcCas_CheckDeviceMsg_t *pstMsg)
{
	SvcCas_DevInstance_t *pstCaInst = NULL;

	if(pstMsg == NULL)
	{
		HxLOG_Error("instance info is null.\n");
		return ERR_FAIL;
	}

	HxLOG_Info("slot id (%d) \n", pstMsg->usSlotId);

	pstCaInst = svc_cas_instance_GetCasDevInstFromCasGIDNSlot(eCasInst_CI, pstMsg->usSlotId);
	if(pstCaInst == NULL)
	{
		HxLOG_Error("instance id is wrong.\n");
		return ERR_FAIL;
	}

	if (pstCaInst->bAllocated != TRUE)
	{
		/* cas mgr에 등록하지 않은 sub cas의 update 요청이다. 에러지..
		*/
		HxLOG_Error("notify by not registerd sub-cas.\n");
		return ERR_FAIL;
	}

	pstCaInst->stCamAppInfo = pstMsg->utData.stCasCiAppInfo;
	HxLOG_Info("%s, CI APP Type:[%d], manu[%x], manuCode[%x], CamName[%s]\n",
		__FUNCTION__, pstCaInst->stCamAppInfo.eCiAppType, pstCaInst->stCamAppInfo.usManufacturer, pstCaInst->stCamAppInfo.usManufacturerCode, pstCaInst->stCamAppInfo.szText);
	return ERR_OK;
}
#endif

HERROR svc_cas_InstanceConnectCasInstance(SvcCas_Context_t *pstContext, HUINT32 ulCasInstId)
{
	SvcCas_DevInstance_t	*pstCasInst;
	HINT32	i;

	if(ulCasInstId >= CAS_MAX_NUM_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	pstCasInst = svc_cas_instance_GetCasDevInst(ulCasInstId);
	if(pstCasInst == NULL)
	{
		return ERR_FAIL;
	}

	/* CI일 경우는 최대한 빨리 화면을 show 해주기 위해서 우선 eSEVT_CAS_OK 를 올리고 시작한다.
	*/
	if(pstCasInst->eCasInstType == eCasInst_CI)
	{
		svc_cas_SvcPortUtilPostMsgToAp(eSEVT_CAS_OK, pstContext->hAction, eDxCAS_GROUPID_NONE, 0, 0);
	}

	if(pstCasInst->ulCurRefCnt >= CAS_MAX_NUM_OF_ACTION)
	{
		// 해당 경우가 있다면 logic에 문제가 있는 것이다.
		HxLOG_Critical("ulCurRefCnt (%d), CAS_MAX_NUM_OF_ACTION (%d)\n", pstCasInst->ulCurRefCnt, CAS_MAX_NUM_OF_ACTION);
		return ERR_FAIL;
	}

	// 동일 서비스인데 connect 요청이 있는지 확인해 본다.(PMT만 update 되었을 경우...)
	for(i = 0 ; i < CAS_MAX_NUM_OF_ACTION ; i++)
	{
		if(pstContext->ulActionId != HANDLE_NULL
			&& pstCasInst->aulActionId[i] == pstContext->ulActionId
			&& pstContext->ulCasInstanceId == ulCasInstId
			&& pstContext->abTryCasInst[ulCasInstId] == TRUE)
		{
			break;
		}
	}

	if(i == CAS_MAX_NUM_OF_ACTION)
	{
		pstCasInst->aulActionId[pstCasInst->ulCurRefCnt] = pstContext->ulActionId;
		pstCasInst->ulCurRefCnt++;
		pstContext->ulCasInstanceId = ulCasInstId;
		pstContext->abTryCasInst[ulCasInstId] = TRUE;
	}

	return ERR_OK;
}

HERROR svc_cas_InstanceDisconnectCasInstance(SvcCas_Context_t *pstContext)
{
	HBOOL				bFind = FALSE;
	HUINT32				i, j;
	SvcCas_DevInstance_t	*pstCasInst;

	if(pstContext->ulCasInstanceId >= CAS_MAX_NUM_OF_INSTANCE)
	{
		HxLOG_Error("pstContext->ulCasInstanceId = [0X%x]\n", pstContext->ulCasInstanceId);
		return ERR_FAIL;
	}

	pstCasInst = svc_cas_instance_GetCasDevInst(pstContext->ulCasInstanceId);
	if(pstCasInst == NULL)
	{
		HxLOG_Error("pstCasInst is NULL\n");
		return ERR_FAIL;
	}

	for(i = 0; i < pstCasInst->ulCurRefCnt; i++)
	{
		if(pstCasInst->aulActionId[i] == pstContext->ulActionId)
		{
			bFind = TRUE;
			break;
		}
	}

	if(bFind == FALSE)
	{
		HxLOG_Error("can't find action id~~~~\n");
		return ERR_FAIL;
	}

	if(i ==  pstCasInst->ulCurRefCnt-1)
	{
		/* 가장 마지막 action id 이면 그냥 삭제 하면 된다. */
		pstCasInst->aulActionId[i] = HANDLE_NULL;
	}
	else
	{
		/* 마지막 action id가 아니면 아래로 shift해야 한다.
		*/
		for(j = i; j < pstCasInst->ulCurRefCnt-1; j++)
		{
			pstCasInst->aulActionId[j] = pstCasInst->aulActionId[j+1];
		}

		pstCasInst->aulActionId[j] = HANDLE_NULL;
	}

	pstCasInst->ulCurRefCnt--;

	pstContext->ulCasInstanceId = HANDLE_NULL;
	VK_memset32(pstContext->abTryCasInst, 0, sizeof(HBOOL) * CAS_MAX_NUM_OF_INSTANCE);

	return ERR_OK;
}

HBOOL svc_cas_InstanceIsAdaptedSCInserted(HUINT32 ulCaInstanceId, HUINT32 ulSlotId)
{
	SvcCas_DevInstance_t *pstCasInst = NULL;

	if(CAS_MAX_NUM_OF_SLOT <= ulSlotId)
	{
		HxLOG_Error("ulSlotId exeeds its Maximum.\n");
		return FALSE;
	}

	pstCasInst = svc_cas_instance_GetCasDevInst(ulCaInstanceId);
	if(pstCasInst == NULL)
	{
		HxLOG_Error("instance info is null.\n");
		return FALSE;
	}

	if(pstCasInst->bAllocated == FALSE)
	{
		HxLOG_Error("instance info was not allocated.\n");
		return FALSE;
	}

	if(pstCasInst->eCasInstType != eCasInst_Embedded)
	{
		HxLOG_Error("It's not embedded\n");
		return FALSE;
	}

	if(pstCasInst->bHaveDevice[ulSlotId] == TRUE)
	{
		HxLOG_Error("instance info was valid and owned slot.\n");
		return TRUE;
	}
	else
	{
		HxLOG_Error("instance info was not valid.\n");
		return FALSE;
	}
}

#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
void svc_cas_InstanceFindNotConnectPathCiNConnect(HUINT32 ulSearchedCasInstId)
{
	HUINT32				unNumberOfNotConnectPathCi;
	HUINT32				ulTunerId;
	HUINT32				unNumberOfExistCam;
	HERROR				hErr;
	SvcCas_Context_t		*pstCasActionContext;
	SvcCas_DevInstance_t	*pstDevInst;
	SvcCas_DevInstance_t	*pstSearchedDevInst = NULL;
	SvcCas_DevCiPathway_e	eCarmCiPathWay;

	HxLOG_Info("svc_cas_InstanceFindNotConnectPathCiNConnect() + \n");

	/* live가 살아 있을때만 연결 시켜 주도록 한다. */
	pstCasActionContext = svc_cas_MgrGetActionContextByActionId(eActionId_VIEW_FIRST);
	if(	!(	(pstCasActionContext->eActType == eCAS_ActionType_Live)
		&&	(pstCasActionContext->state > eCAS_State_Idle)))
	{

		#if defined(CONFIG_DEBUG)
		HxLOG_Info("It's not live or Idle: eActType = %s,  state = %s\n",
					svc_cas_instance_MakeStrActionType(pstCasActionContext->eActType),
					svc_cas_instance_MakeStrContextState(pstCasActionContext->state));
		#endif

		return;
	}

	unNumberOfExistCam = svc_cas_instance_GetNumberOfExistCam();
	HxLOG_Info("unNumberOfExistCam = %d\n", unNumberOfExistCam);

	// CAM이 없으면 당근 할일 없으니 return
	if(unNumberOfExistCam == 0)
	{
		return;
	}

	if(ulSearchedCasInstId < s_ulNumOfRegisteredInst)
	{
		pstSearchedDevInst = svc_cas_instance_GetCasDevInst(ulSearchedCasInstId);
	}

	hErr = svc_cas_SvcPortRmGetResourceId(pstCasActionContext->ulActionId, eCAS_RM_ITEM_TUNER_GROUP, &ulTunerId);
	if(hErr != ERR_OK)
	{
		HxLOG_Error(" svc_cas_SvcPortRmGetResourceId() failed\n");
		return;
	}

	/*	path가 연결된 CI가 없으면?
	*/
	pstDevInst = svc_cas_instance_FindConnectPathCi();
	if(pstDevInst == NULL)
	{
		HxLOG_Info("pstDevInst is NULL\n");

		pstDevInst = svc_cas_instance_FindNotConnectPathCi();
		if(pstDevInst == NULL)
		{
			HxLOG_Error(" svc_cas_instance_FindNotConnectPathCi() pstDevInst is NULL\n");
			return;
		}

		/* path가 연결되지 않은 CI가 몇개 인가?
		*/
		unNumberOfNotConnectPathCi = svc_cas_instance_CountNotConnectPathCi();
		HxLOG_Info("unNumberOfNotConnectPathCi = %d\n", unNumberOfNotConnectPathCi);
		if(unNumberOfNotConnectPathCi == 0)
		{
			HxLOG_Info("Number of not connect path ci is zero\n");
			return;
		}
		else if(unNumberOfNotConnectPathCi == 1)
		{
			eCarmCiPathWay = eCAS_CI_PATH_NORMAL_PASSTHROUGH;
		}
		else
		{
			/*  FTA를  CI에 두 개 넣어야 할때는 custom pass through 시키도록 한다.
			*/
			eCarmCiPathWay = eCAS_CI_PATH_CUSTOM_PASSTHROUGH;
		}

		hErr = svc_cas_PalPortCiSetExtendedTsPath(ulTunerId, pstDevInst->ulSlot, pstCasActionContext->ulDemuxId, eCarmCiPathWay);
		if(hErr != ERR_OK)
		{
			HxLOG_Error(" svc_cas_SvcPortRmGetResourceId() failed\n");
			return;
		}

		/* send clear ca pmt */
		if(unNumberOfNotConnectPathCi == 1)
		{
			svc_cas_instance_SendClearCaPmt(pstCasActionContext, pstDevInst->ulSlot);
		}
		else
		{
			svc_cas_instance_SendClearCaPmt(pstCasActionContext, 0);
			svc_cas_instance_SendClearCaPmt(pstCasActionContext, 1);
		}

		HxLOG_Info("svc_cas_InstanceFindNotConnectPathCiNConnect() - \n");
		return;
	}
	/*	path가 연결된 CI가 있는데 선택된 SUB CAS가 없거나 선택 되었어도 Embedded CAS 인 경우
	*/
	else if(		(ulSearchedCasInstId == HANDLE_NULL)
			||	(pstSearchedDevInst != NULL && pstSearchedDevInst->eCasInstType == eCasInst_Embedded))
	{
		HUINT32			ulConnectedTunerId;
		SvcCas_Context_t	*pstConnectedContext;

		HxLOG_Info("ulSearchedCasInstId = %d, pstSearchedDevInst = 0x%x\n", ulSearchedCasInstId, pstSearchedDevInst);

		/*	path가 연결된 CI가 있는데 선택된 SUB CAS가 없다.
			그런데 CAM이 하나면? 할 것이 없다.. 이미 연결되어 있으므로.
		*/
		if(unNumberOfExistCam == 1)
		{
			HxLOG_Info("unNumberOfExistCam == 1\n");
			return;
		}

		if(svc_cas_instance_FindNotConnectPathCi() == NULL)
		{
			// 이 경우라면 삽입되어 있는 모든 CAM에 PATH가 연결되어 있는 상태이다. (삽입된 CAM으로 서비스가 다 되고 있다.)
			HxLOG_Print("pstDevInst is NULL - FindNotConnectPathCi\n");
			return;
		}

		if(svc_cas_SvcPortRmGetResourceId(pstDevInst->aulActionId[0], eCAS_RM_ITEM_TUNER_GROUP, &ulConnectedTunerId) != ERR_OK)
		{
			HxLOG_Error("svc_cas_SvcPortRmGetResourceId() error\n");
			return;
		}

		if(ulTunerId == ulConnectedTunerId)
		{
			/*	CAM이 두 갠데 이미 연결된 path가 있다?
				그렇다면 연결된 path를 기준으로 out put없는 diasy chain으로 연결 한다.
			*/
			hErr = svc_cas_PalPortCiSetExtendedTsPath(ulTunerId, pstDevInst->ulSlot, pstCasActionContext->ulDemuxId, eCAS_CI_PATH_CUSTOM_PASSTHROUGH);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("svc_cas_PalPortCiSetExtendedTsPath() failed\n");
				return;
			}

			svc_cas_instance_SendClearCaPmt(pstCasActionContext, (pstDevInst->ulSlot == 0) ? 1 : 0 );
		}
		else
		{
			/* Live와 다른 Tuner에 CAM이 연결되어 있다. daisy chain 상태에서 다른 TP의 채널전환시 해당 케이스로 온다. */
			pstConnectedContext = svc_cas_MgrGetActionContextByActionId(pstDevInst->aulActionId[0]);
			if(pstConnectedContext == NULL)// 해당 케이스가 발생해서는 안된다.
			{
				HxLOG_Error("svc_cas_MgrGetActionContextByActionId() failed\n");
				return;
			}

			if(svc_cas_PalPortCiSetExtendedTsPath(ulConnectedTunerId, pstDevInst->ulSlot, pstConnectedContext->ulDemuxId, eCAS_CI_PATH_NORMAL_PASSTHROUGH) != ERR_OK)
			{
				HxLOG_Error("svc_cas_PalPortCiSetExtendedTsPath() failed\n");
				return;
			}
		}

		HxLOG_Info("svc_cas_InstanceFindNotConnectPathCiNConnect() - \n");
		return;
	}

	/*	이곳 까지 오면 path가 연결된 CI가 있는데 선택된 SUB CAS가 CI 인 경우 이기 때문에
		아무것도 하지 않고 그냥 return 한다. 왜? -> ci mgr에 path 설정을 할것이기 때문에~
	*/
	HxLOG_Info("do not anything\n");
	HxLOG_Info("svc_cas_InstanceFindNotConnectPathCiNConnect() - \n");
	return;
}

#endif

/********************************************************************
* For Debugging
********************************************************************/

#define ___08_Debugging_APIs___

#if defined(CONFIG_DEBUG)
#if 0
STATIC HINT8 *svc_cas_instance_MakeStrCaSysId(SvcCas_CaSysIdSpecifier_e cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(eCA_SYS_ID_STANDARD);
		ENUM_TO_STR(eCA_SYS_ID_CANAL_PLUS);
		ENUM_TO_STR(eCA_SYS_ID_CCETT);
		ENUM_TO_STR(eCA_SYS_ID_DEUTSCHE_TEL);
		ENUM_TO_STR(eCA_SYS_ID_EURODEC);
		ENUM_TO_STR(eCA_SYS_ID_FRANCE_TEL);
		ENUM_TO_STR(eCA_SYS_ID_IRDETO);
		ENUM_TO_STR(eCA_SYS_ID_JERROID_GI);
		ENUM_TO_STR(eCA_SYS_ID_MATRA_COM);
		ENUM_TO_STR(eCA_SYS_ID_NEWS_DATACOM);
		ENUM_TO_STR(eCA_SYS_ID_NOKIA);
		ENUM_TO_STR(eCA_SYS_ID_NORWEGIAN_TEL);
		ENUM_TO_STR(eCA_SYS_ID_NTL);
		ENUM_TO_STR(eCA_SYS_ID_PHILIPS);
		ENUM_TO_STR(eCA_SYS_ID_SCI_ATLANTA);
		ENUM_TO_STR(eCA_SYS_ID_SONY);
		ENUM_TO_STR(eCA_SYS_ID_TANDBERG);
		ENUM_TO_STR(eCA_SYS_ID_THOMSON);
		ENUM_TO_STR(eCA_SYS_ID_TV_COM);
		ENUM_TO_STR(eCA_SYS_ID_HPT_CPT);
		ENUM_TO_STR(eCA_SYS_ID_HRT_CRT);
		ENUM_TO_STR(eCA_SYS_ID_IBM);
		ENUM_TO_STR(eCA_SYS_ID_NERA);
		ENUM_TO_STR(eCA_SYS_ID_BETA_TECH);
		ENUM_TO_STR(eCA_SYS_ID_KUDELSKI_SA);
		ENUM_TO_STR(eCA_SYS_ID_TITAN_INFO);
		ENUM_TO_STR(eCA_SYS_ID_TELEFONICA);
		ENUM_TO_STR(eCA_SYS_ID_STENTOR);
		ENUM_TO_STR(eCA_SYS_ID_TADIRAN_SCOPUS);
		ENUM_TO_STR(eCA_SYS_ID_BARCO_AS);
		ENUM_TO_STR(eCA_SYS_ID_NUM);
	}
	return ("Unknown");
}
#endif

#if defined(CONFIG_MW_CI) && defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)

STATIC const HINT8 * svc_cas_instance_MakeStrContextState(SvcCas_State_e eState)
{
	switch (eState)
	{
		ENUM_TO_STR(eCAS_State_Idle);
		ENUM_TO_STR(eCAS_State_Started);
		ENUM_TO_STR(eCAS_State_SetPid);
		ENUM_TO_STR(eCAS_State_subCasNotSelected);
		ENUM_TO_STR(eCAS_State_subCasSelected);
		ENUM_TO_STR(eCAS_State_CasOK);
		ENUM_TO_STR(eCAS_State_CasFail);
		ENUM_TO_STR(eCAS_State_Max);
	}

	return ("Unknown");
}

STATIC const HINT8 *svc_cas_instance_MakeStrActionType(SvcCas_ActionType_e cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(eCAS_ActionType_None);
		ENUM_TO_STR(eCAS_ActionType_Live);
		ENUM_TO_STR(eCAS_ActionType_Playback);
		ENUM_TO_STR(eCAS_ActionType_Recording);
		ENUM_TO_STR(eCAS_ActionType_Tsr_Play);
		ENUM_TO_STR(eCAS_ActionType_Tsr_Rec);
		ENUM_TO_STR(eCAS_ActionType_Descramble_a_File);
		ENUM_TO_STR(eCAS_ActionType_Data);
		ENUM_TO_STR(eCAS_ActionType_EngineeringTS);
		ENUM_TO_STR(eCAS_ActionType_RfVod);
		ENUM_TO_STR(eCAS_ActionType_Max);
	}
	return ("Unknown");
}
#endif

STATIC HINT8 *svc_cas_instance_MakeStrCamState(SvcCas_CamState_e eCAS_CamState)
{
	switch (eCAS_CamState)
	{
		ENUM_TO_STR(eCAS_CAM_STATE_Removed);
		ENUM_TO_STR(eCAS_CAM_STATE_Inserted);
		ENUM_TO_STR(eCAS_CAM_STATE_Initializing);
		ENUM_TO_STR(eCAS_CAM_STATE_Initialized);
	}
	return ("Unknown");
}

STATIC HINT8 *svc_cas_instance_MakeStrCasInstType(SvcCas_InstanceType_e eType)
{
	switch (eType)
	{
		ENUM_TO_STR(eCasInst_None);
		ENUM_TO_STR(eCasInst_Embedded);
		ENUM_TO_STR(eCasInst_CI);
		ENUM_TO_STR(eCasInst_Max);
	}
	return ("Unknown");
}

static HINT8 *svc_cas_instance_MakeStrCasGroupId(DxCAS_GroupId_e eCasGroupId)
{
	switch (eCasGroupId)
	{
		ENUM_TO_STR(eDxCAS_GROUPID_NONE);
		ENUM_TO_STR(eDxCAS_GROUPID_MG);
		ENUM_TO_STR(eDxCAS_GROUPID_VA);
		ENUM_TO_STR(eDxCAS_GROUPID_IR);

		ENUM_TO_STR(eDxCAS_GROUPID_CX);
		ENUM_TO_STR(eDxCAS_GROUPID_CR);
		ENUM_TO_STR(eDxCAS_GROUPID_NA);
		ENUM_TO_STR(eDxCAS_GROUPID_ND);
		ENUM_TO_STR(eDxCAS_GROUPID_SATSA);
		ENUM_TO_STR(eDxCAS_GROUPID_COMMON);
		ENUM_TO_STR(eDxCAS_GROUPID_CAM);
		default: return "unknown";
	}
	return ("Unknown");
}

STATIC HINT8 *svc_cas_instance_MakeStrCbNotiType(SvcCas_InstNotiType_e eType)
{
	switch (eType)
	{
		ENUM_TO_STR(eCAS_NOTI_Start);
		ENUM_TO_STR(eCAS_NOTI_Stop);
		ENUM_TO_STR(eCAS_NOTI_SetDsc);			// set descrambler, pid setting.
		ENUM_TO_STR(eCAS_NOTI_ManageInst);
		ENUM_TO_STR(eCAS_NOTI_UpdatePmt);
		ENUM_TO_STR(eCAS_NOTI_UpdateCat);
		ENUM_TO_STR(eCAS_NOTI_UpdateSdt);
		ENUM_TO_STR(eCAS_NOTI_UpdateNit);
		ENUM_TO_STR(eCAS_NOTI_UpdateEit);
		ENUM_TO_STR(eCAS_NOTI_UpdateTdt);
		ENUM_TO_STR(eCAS_NOTI_UpdateBat);
		ENUM_TO_STR(eCAS_NOTI_Max);
		default:	break;
	}
	return ("Unknown");
}

STATIC HINT8 *svc_cas_instance_MakeStrCiAppType(SvcCas_CiAppType_e eCiAppType)
{
	switch (eCiAppType)
	{
		ENUM_TO_STR(eCiAppType_Ca);
		ENUM_TO_STR(eCiAppType_Epg);
		ENUM_TO_STR(eCiAppType_SwUpgrade);
		ENUM_TO_STR(eCiAppType_NetInterface);
		ENUM_TO_STR(eCiAppType_AccessAids);
		ENUM_TO_STR(eCiAppType_Unclassified);
	}
	return ("Unknown");
}

HERROR svc_cas_InstancePrintCaInstTableById(HUINT32 ulCaInstanceId)
{
	HUINT32				i, j = 0;
	SvcCas_DevInstance_t	*pInst = NULL;
#if defined(CONFIG_DEBUG)

	if(ulCaInstanceId >= CAS_MAX_NUM_OF_INSTANCE)
	{
		return ERR_FAIL;
	}

	pInst = svc_cas_instance_GetCasDevInst(ulCaInstanceId);
	if(pInst == NULL)
	{
		return ERR_FAIL;
	}

	HLIB_CMD_Printf("\n================= print instance table [Id:%d] ================= \n", ulCaInstanceId);

	HLIB_CMD_Printf("Instance Type : %s \n", svc_cas_instance_MakeStrCasInstType(pInst->eCasInstType));
	HLIB_CMD_Printf("eCasId : %s \n", svc_cas_instance_MakeStrCasGroupId(pInst->eCasId));
	HLIB_CMD_Printf("ulSlot : %d \n", pInst->ulSlot);
	HLIB_CMD_Printf("is Allocated : %d \n", pInst->bAllocated);
	HLIB_CMD_Printf("is bUsed : %d \n", pInst->bUsed);
	HLIB_CMD_Printf("eLastNotify : %s \n", svc_cas_instance_MakeStrCbNotiType(pInst->eLastNotify));

	for(i = 0; i < CAS_MAX_NUM_OF_SLOT; i++)
	{

		HLIB_CMD_Printf("    ------------\n");
		HLIB_CMD_Printf("    Have a device [%d] : %d \n", i, pInst->bHaveDevice[i]);
		HLIB_CMD_Printf("    ulNumCaSysId[%d] : %d \n", i, pInst->ulNumCaSysId[i]);
		for(j = 0; j < pInst->ulNumCaSysId[i]; j++)
		{
			HLIB_CMD_Printf("    [%d]-th CaSysId(0x%x) \n", i, pInst->ausCaSysId[i][j]);
		}
		HLIB_CMD_Printf("    ------------\n");
	}

	for(i = 0; i < pInst->ulCurRefCnt; i++)
	{
		HLIB_CMD_Printf("    ------------\n");
		HLIB_CMD_Printf("    pInst->aulActionId[%d] = %d\n", i, pInst->aulActionId[i]);
		HLIB_CMD_Printf("    ------------\n");
	}

	HLIB_CMD_Printf("ulNumOfMaxSvc : %d \n", pInst->ulNumOfMaxSvc);
	HLIB_CMD_Printf("ulCurRefCnt : %d \n", pInst->ulCurRefCnt);

	if(pInst->eCasInstType == eCasInst_CI)
	{
		HLIB_CMD_Printf("eCamState : %s \n", svc_cas_instance_MakeStrCamState(pInst->eCamState));
		HLIB_CMD_Printf("SvcCas_CiAppType_e : %s \n", svc_cas_instance_MakeStrCiAppType(pInst->stCamAppInfo.eCiAppType));
		HLIB_CMD_Printf("usManufacturer = %d\n", pInst->stCamAppInfo.usManufacturer);
		HLIB_CMD_Printf("usManufacturerCode = %d\n", pInst->stCamAppInfo.usManufacturerCode);
		HLIB_CMD_Printf("szText = %s\n", pInst->stCamAppInfo.szText);
		HLIB_CMD_Printf("ucIsInitialized = %d\n", pInst->stCamAppInfo.ucIsInitialized);
	}

	HLIB_CMD_Printf ("\n========================================================= \n");
#endif
	return ERR_OK;
}

HERROR svc_cas_InstancePrintCaInstTable(void)
{
	HUINT32 			idx = 0;

	HLIB_CMD_Printf ("\n========================= print instance table for all ============================= \n");

	for(idx = 0; idx < CAS_MAX_NUM_OF_INSTANCE; idx++)
	{
		svc_cas_InstancePrintCaInstTableById(idx);
	}

	HLIB_CMD_Printf ("\n===================== end of print instance table for all ========================== \n");

	return ERR_OK;
}

#endif

/*********************** End of File ******************************/

