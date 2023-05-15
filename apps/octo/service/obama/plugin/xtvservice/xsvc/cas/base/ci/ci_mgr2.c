/**
	@file     ci_mgr2.c
	@brief    ci_mgr2.c (CAS Service)

	Description: CAS Manager와 Interface 및 CI Stack 관리 등을 위한 CI Manager.					\n
	Module: 									\n
	Remarks : 										\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/********************************************************************
* Header Files
********************************************************************/
#define ___01_Header_Files___
#include <rlib.h>

#include <octo_common.h>
#include <util.h>
#include <linkedlist.h>
#include <db_svc.h>

//#include <pal_pipe.h>
#include <svc_cas.h>
#include <_svc_cas_sub_svc_api.h>
#include <_svc_cas_sub_dev_api.h>

#include <_svc_cas_mgr_main.h>
#include <_svc_cas_sub_svc_api.h>

#include <xsvc_cas.h>
#include <pal_ci.h>

#include <ci.h>
#include <ci_res.h>
#include <ci_ctrl.h>
#include <ci_adapt.h>
#include <ci_mgr_int.h>

#if defined(CONFIG_MW_CI_PLUS)
#include <ci_plus.h>
#endif

#include <isosvc.h>

#include "../../local_include/_xsvc_cas.h"

/********************************************************************
* For Debugging
********************************************************************/
#define ___02_Debugging___
#define _CI_DEBUG_ 0


/********************************************************************
* Local Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Local_Definitions___

//#define CONFIG_SUPPORT_RETRY_CAPMT
#define CONFIG_MW_CI_CHECK_AV

// 짧은 주기로 채널전환을 할 시 AV가 깨지는 현상 발생. 짧게 체크하여 AV state를 update 하자.
#define CIMGR_CHECK_AV_PERIOD		(1 * 1000)
#define CIMGR_CHECK_AV_COUNT		10

#define	ENTER_CRITICAL_MMI	VK_SEM_Get(s_ulMmiDataSema)
#define	LEAVE_CRITICAL_MMI	VK_SEM_Release(s_ulMmiDataSema)

#define CAS_RESOURCE_NONE				0xFFFFFFFF
#define CAS_FILTER_ID_NULL				0xFFFFFFFF

// State.
#define CAS_USED 						1
#define CAS_NOT_USED 					0
#define FreeCiMode						0x00000400 //SVC_UserFlag1_t에 있는 eSvcUserFlag_11_FreeCiMode 이다.

// ci plus private data value
#define CI_PLUS_PRIVATE_DATA_VALUE	0x40

#if defined(CONFIG_MW_CI_PLUS)
typedef struct
{
	HBOOL				bUsed;					// Sdt slot flag
	HUINT32				ulActionId;				// Action Id
	HUINT32				ulDemuxId;				// current demux id /* HUINT32 */
	HUINT16				usTsId;					// current network ts id
	HUINT16				usUniqueId;				// octo's unique id

	//Section Info
	POINTER_LIST_T		*pstList;				// descriptor list
	HUINT32 				ulSvcNum;				// number of service in sdt act.

	//FilterParam
	HUINT32 				ulFilterId;				// sdt filter id.
	HUINT8				ucTableId;				// sdt table id.
	HUINT16				usExtId;				// ts id.
	HUINT16				usPid;					// sdt pid.
	HUINT32				eFilterMode;			// PalSef_FilterMode_e (once, monitor, continue)
	SvcCas_SiTableState_e	ulTblState;				// Filter state.
	HBOOL				bUseVerNum;				// filtering condition.
	HBOOL				bCrc;					// filtering condition.
	HBOOL				bUseExtId;				// filtering condition.
	HBOOL				bTableFilter;			// filtering condition.
}CAS_SdtStatus_t;
#endif

typedef struct
{
	POINTER_LIST_T		*s_pstList;
}CI_MmiData_t;

typedef struct
{
	HBOOL				bTuned;
	HUINT8				ucDesLen;
	HUINT8				*pucDes;
} CI_OperatorTune_t;

typedef struct
{
	HUINT16				usTsId;
	HUINT16				usOnId;
	DxDeliveryType_e	eDelivery;
	SvcCas_TuningDesInfo_t	stTuningDes;
	POINTER_LIST_T		*pSvcList;		// SvcCas_OperatorSvc_t
}CI_OperatorNitTsLoop_t;

typedef struct
{
	HUINT8			*pucNitRaw;

	// parsing
	HUINT16			usNetId;
	HUINT8			szNetName[256];

	POINTER_LIST_T	*pTsLoopList;	// CI_OperatorNitTsLoop_t
} CI_OperatorNit_t;

typedef struct
{
	CI_SLOT_ID			usSlotId;

	HUINT8				szProfName[256];
	HUINT8				aucTextCoding[7];
	POINTER_LIST_T		*pTuneList;		// CICAM에서 받은 TP list
	POINTER_LIST_T		*pNitList;		// CICAM에서 받은 NIT 및 NIT parsing 결과

	HUINT8				ucCurTpIdx;		//  TV Service로 요쳥한 TP 인덱스
	SvcCas_OperatorTune_t	stOpTune;		// TV Service로 올린 정보
} CI_OperatorProfile_t;

typedef struct {
	HUINT16		usSvcId;
	HUINT8		ucSvcType;
	HUINT8		ucVisible;
	HUINT8		ucSelectable;
	HUINT16		usLcn;
	HUINT8		ucSvcProviderNameLen;
	HUINT8		aucSvcProviderName[256];
	HUINT8		ucSvcNameLen;
	HUINT8		aucSvcName[256];
}CI_CiplusSvcDes_t;

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___04_Extern_Vars_Func_Prototypes___

//2009.02.06 BSPARK. Mmi Open 시 Retry 하지 않도록 수정.
extern HBOOL g_bMmiOpened;

#if defined(SUPPORT_SMART_CI_MMI_PINCODE)
extern int CiMmi_CheckWaitingUserInput(CI_SLOT_ID usSlotId, unsigned long* IsWaiting);
#endif

#define CI_MGR_MAX_DRM_INFO	20		// 배열 크기가 부족할 경우 확장 가능.

/********************************************************************
* Static Variables / Function Prototypes
********************************************************************/
#define ___05_Local_Vars_Func_Prototypes___

// ca system id 가 존재하는지만 확인.
HUINT8 g_aucIsCaSysId[CARM_MAX_CISLOT_NUMBER];	/* BS : trans.c에서 사용. */

static HULONG 			s_ulCiMgrTaskId;
static HULONG 			s_ulCiMgrMsgQId;

static CI_Context_t			s_astCiContext[CI_MAX_NUM_OF_ACT];
static CI_CamInst_t		s_astCiCam[CARM_MAX_CISLOT_NUMBER];

// TODO: base에서 작업 재시작시 전역변수 정리할 것.!

static HBOOL				s_abUriSent[CARM_MAX_CISLOT_NUMBER];
static HBOOL				s_abCheckAvMoreCount[CARM_MAX_CISLOT_NUMBER];

static HUINT32			s_ulMmiVersion;
static CI_MmiData_t		s_stMmiData[CARM_MAX_CISLOT_NUMBER];

static HUINT32			s_ulMmiDataSema;		/* mmi data getting 시 사용 */

static SvcCas_TuneInfo_t 		s_stCasTuneInfo;		// CAM Tune 동작시 사용

static HUINT8 					s_ucCiDrmInfoVer;
static DxCopyrightControl_t 				s_astCiDrmInfo[CI_MGR_MAX_DRM_INFO];
static eDxCopyrightControl_CiPlusDrmWriteInfo_e		s_eCiDrmWriteState[CI_MGR_MAX_DRM_INFO];

#if defined(CONFIG_MW_CI_PLUS)
//CI Plus 용 SDT Status
static CAS_SdtStatus_t		s_stSdtStatus[CAS_MAX_NUM_OF_TS];

static DxCopyrightControl_CiPlusUriInfo_t		s_stUriInfo[CARM_MAX_CISLOT_NUMBER];

static HUINT32			s_ulCcReportVersion;
static SvcCas_CcReport_t		s_stCcReportStatus[CARM_MAX_CISLOT_NUMBER];

static HUINT32 			s_ulCiPlusUpgradeVersion;
static SvcCas_CiPlusUpgrade_t	s_stCiPlusUpgradeStatus[CARM_MAX_CISLOT_NUMBER];

static HUINT8			s_aucCiPrvSvcCamId[8] = {0,};
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
static CI_OperatorProfile_t	s_stOperatorProfile[CARM_MAX_CISLOT_NUMBER];
#endif
#endif

static HERROR local_CiMgr_RegisterCb(void);
static HERROR local_CiMgr_StopDescramble(CI_Context_t *pstCiContext, CI_SLOT_ID usSlotId);
#if defined(CONFIG_SUPPORT_RETRY_CAPMT)
static HERROR local_CiMgr_StartRetrySvc(CI_Context_t *pCiContext);
#endif

#if defined(CONFIG_MW_CI_CHECK_AV)
static HERROR local_CiMgr_StartCheckAv(CI_Context_t *pCiContext);
static HERROR local_CiMgr_StopCheckAv(CI_Context_t *pstCiContext);
#endif

#if defined(CONFIG_MW_CI_PLUS)
static HERROR local_CiMgr_InitSdtStatus(HUINT32 ulIdx);
static HERROR local_CiMgr_ProcDscSetForCiPlus(	CI_CamInst_t *pCam,
													HUINT32 ulDemuxId,
													HBOOL bIsUpdate,
													HBOOL bIsRetry,
													HBOOL bCalledByStartDescramble /* 이건 디버깅 용도 */);

static HERROR local_CiMgr_GetCiDescInfo(CI_Context_t *pCiContext, CAS_CiDscInfo_t **pDscInfo);
static HERROR local_CiMgr_ProcCheckShunCondition(CI_SLOT_ID usSlotId, CAS_CiDscInfo_t *pstCiDsc, HBOOL *bIsNeedShun);
static HERROR local_CiMgr_MakeCiDrmInfo(eDxCopyrightControl_CiPlusDrmType_e eCiPlusDrmType, void *pData, DxCopyrightControl_t **ppDrmInfo, eDxCopyrightControl_CiPlusDrmWriteInfo_e eCiPlusDrmWriteInfo);
static HBOOL local_CiMgr_CheckClearCaPmtCondition(CI_Context_t *pCiContext, HBOOL bNeedCam);
static void local_CiMgr_SendClearCaPmt(CI_SLOT_ID usSlotId, CI_Context_t *pstContext);
#endif

#if defined(CONFIG_DEBUG)
static const HINT8 * local_CiMgr_Cmd2Str(CI_RCV_EVENT cmd);
static const HINT8 *local_CiMgr_MakeStrActionType(SvcCas_ActionType_e cmd);
static const HINT8 *local_CiMgr_MakeStrSiTblState(SvcCas_SiTableState_e eState);
static void			local_CiMgr_PrintOperatorNit(CI_OperatorProfile_t *pstContents);
#endif

/********************************************************************
* Static Functions
* Naming rule : "local"_"CiMgr" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___

static HINT32 local_CiMgr_SendEvent(CI_RCV_EVENT eEvt, void *pvEvtParam, unsigned long ulSize)
{
	HUINT8 aucMsg[MW_CAS_CIMGR_MSG_SIZE];

	if(pvEvtParam == NULL)
	{
		return VK_ERROR;
	}

	if((sizeof(CI_RCV_EVENT) + ulSize) > MW_CAS_CIMGR_MSG_SIZE)
	{
		HxLOG_Error("\n");
		return VK_ERROR;
	}

	HxSTD_memset(aucMsg, 0, MW_CAS_CIMGR_MSG_SIZE);
	HxSTD_memcpy(aucMsg, &eEvt, sizeof(CI_RCV_EVENT));
	HxSTD_memcpy(&aucMsg[sizeof(CI_RCV_EVENT)], pvEvtParam, ulSize);

	return VK_MSG_SendTimeout(s_ulCiMgrMsgQId, (void *)aucMsg, MW_CAS_CIMGR_MSG_SIZE, 0);

}

static void local_CiMgr_InitContext(HUINT32 ulContextId)
{
	HUINT32 i = 0;

	VK_memset32(&s_astCiContext[ulContextId], 0x00, sizeof(CI_Context_t));

	s_astCiContext[ulContextId].hAction = HANDLE_NULL;
	s_astCiContext[ulContextId].hSvc = HANDLE_NULL;
	s_astCiContext[ulContextId].eActType = eCAS_ActionType_None;
	s_astCiContext[ulContextId].eState = eCiMgrState_Stopped;
	s_astCiContext[ulContextId].bPathConnected = FALSE;
	s_astCiContext[ulContextId].bSendCaPmt = FALSE;

	s_astCiContext[ulContextId].ulRetryTimerId = INVALID_TIMER_ID;
	s_astCiContext[ulContextId].ulCheckAvTimerId = INVALID_TIMER_ID;

#if defined(_LIMIT_CI_RETRY_COUNT_)
	s_astCiContext[ulContextId].ucRetryCnt = 0;
#endif

	s_astCiContext[ulContextId].pCam = NULL;

	s_astCiContext[ulContextId].ulTunerId = CAS_RESOURCE_NONE;
	s_astCiContext[ulContextId].ulDemuxId = CAS_RESOURCE_NONE;

#if defined(CONFIG_MW_CI_PLUS)
	s_astCiContext[ulContextId].usUniqueId = 0;
	s_astCiContext[ulContextId].usSvcId = 0;

	s_astCiContext[ulContextId].eSdtTblState = eCASSI_Table_Skipped;
	s_astCiContext[ulContextId].bSvcShunned = FALSE;
	s_astCiContext[ulContextId].bRcvUri = FALSE;
#if defined (CONFIG_MW_CI_PLUS_V_1_3)
	s_astCiContext[ulContextId].bRecordProcessDone = FALSE;
#endif
#endif

	for(i=0; i<CAS_MAX_NUM_OF_ES; i++)
	{
		s_astCiContext[ulContextId].stProgInfo.aEsInfo[i].usEsPid = PID_NULL;
	}

	return;
}

static void local_CiMgr_InitContexts(void)
{
	HUINT32 i;

	for (i = 0; i < CI_MAX_NUM_OF_ACT; i++)
	{
		local_CiMgr_InitContext(i);
	}

	return;
}

static void local_CiMgr_InitCamInst(HUINT16 usSlotId)
{
	HUINT32 i;

	if(usSlotId >= CARM_MAX_CISLOT_NUMBER)
	{
		return;
	}

	HxSTD_memset(&s_astCiCam[usSlotId], 0, sizeof(CI_CamInst_t));
	s_astCiCam[usSlotId].usSlotId = CARM_INVALID_CI_SLOT_ID;
	s_astCiCam[usSlotId].hCasDevice = HANDLE_NULL;

	for (i = 0; i < CAS_MAX_NUM_OF_ES; i++)
	{
		s_astCiCam[usSlotId].PidInfoForCaPmt[i].usPid = PID_NULL;
		s_astCiCam[usSlotId].PidInfoForCaPmt[i].esType = eEsType_None;
	}

	for (i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		s_astCiCam[usSlotId].hActList[i] = HANDLE_NULL;
	}

	#if defined(CONFIG_MW_CI_PLUS)
	for (i = 0; i < CI_MAX_NUM_OF_DMX; i++)
	{
		HUINT32 j = 0;
		for (j = 0; j < CAS_MAX_NUM_OF_DSC; j++)
		{
			s_astCiCam[usSlotId].stDscInfo[i][j].ulDscId = CAS_RESOURCE_NONE;
			s_astCiCam[usSlotId].stDscInfo[i][j].usPid = PID_NULL;
		}
	}

	s_astCiCam[usSlotId].hLastSvc = HANDLE_NULL;
	s_astCiCam[usSlotId].bClearCaPmt = TRUE;
	#endif
}

static void local_CiMgr_InitCamInsts(void)
{
	HUINT32 i;

	for (i = 0; i < CARM_MAX_CISLOT_NUMBER; i++)
	{
		local_CiMgr_InitCamInst(i);
	}

	return;
}

static HERROR local_CiMgr_InitModule(void)
{
	HERROR		hErr;
	HUINT32 		nResult = 0;

/*	초기화 순서 변경하지 말것!!!
*/

/* CI mgr 에서 운용하는 글로벌, 로컬 변수 초기화 */
	VK_memset32(g_aucIsCaSysId, 0, sizeof(HUINT8) * CARM_MAX_CISLOT_NUMBER);
	local_CiMgr_InitCamInsts();
	local_CiMgr_InitContexts();

/* CI stack 에서 운용하는 변수 초기화 */
	hErr = xsvc_cas_CiInit();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("xsvc_cas_CiInit() failed\n");
		return ERR_FAIL;
	}

/* 콜백 함수 등록  */
	hErr = local_CiMgr_RegisterCb();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("local_CiMgr_RegisterCb() failed\n");
		return ERR_FAIL;
	}

/* register control list */
	hErr = CI_Ctrl_RegisterCtrlList();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("CA_CI_RegisterCtrlList() failed\n");
		return ERR_FAIL;
	}

/* Host Service Shunning 을 위한 초기화 */
#if defined(CONFIG_MW_CI_PLUS)
	{
		HUINT32 	i;
		for(i = 0; i<CAS_MAX_NUM_OF_TS; i++)
		{
			local_CiMgr_InitSdtStatus(i);
		}
	}
#endif

	nResult = VK_SEM_Create((HULONG *)&s_ulMmiDataSema,"mmidata", VK_SUSPENDTYPE_PRIORITY);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("Creation Error: s_ulCasMmiDataSema sema \n");
		return ERR_FAIL;
	}

	HxSTD_memset(&s_stOperatorProfile, 0, sizeof(CI_OperatorProfile_t) * CARM_MAX_CISLOT_NUMBER);
	HxSTD_memset(&s_abUriSent, 0, sizeof(HBOOL) * CARM_MAX_CISLOT_NUMBER);

	return ERR_OK;
}

static HERROR local_CiMgr_GetResourceId(CI_Context_t *pstContext)
{
	HUINT32		ulActionId;
	HERROR		hErr;
	HUINT32		ulTunerId = RxRSCID_NULL;
	Handle_t		hAction = HANDLE_NULL;

	if (pstContext == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	hAction = pstContext->hAction;
	ulActionId = svc_cas_SubSvcRmGetActionId(hAction);
	if(ulActionId >= NUM_MAX_ACTION)
	{
		return ERR_FAIL;
	}

	hErr = svc_cas_SubSvcRmGetResourceId(ulActionId, eCAS_RM_ITEM_DEMUX, &pstContext->ulDemuxId);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("Critical Error!!!\n");
	}

	switch(pstContext->eActType)
	{
	case eCAS_ActionType_Live:
	case eCAS_ActionType_Recording:
	case eCAS_ActionType_Tsr_Rec:
		hErr = svc_cas_SubSvcRmGetResourceId(ulActionId, eCAS_RM_ITEM_TUNER_GROUP, &ulTunerId);
		if(hErr != ERR_OK)
		{
			HxLOG_Critical("Critical Eror!!!eActType = (%d)\n", pstContext->eActType);
		}
		pstContext->ulTunerId = ulTunerId;
		break;

	default:
		break;
	}

	return ERR_OK;
}

static void local_CiMgr_DefineChannelType(SvcCas_PmtInfo_t *pstPmtInfo, SvcCas_ChannelType_e *eChType)
{
	SvcCas_ChannelType_e type;
	HUINT32 i = 0;

	if(pstPmtInfo == NULL || eChType == NULL)
	{
		return;
	}

	type = eCasChannel_FTA;

	if(pstPmtInfo->bProgramCaFlag == TRUE)
	{
		type = eCasChannel_Scramble;
	}

	for(i=0; i<pstPmtInfo->ulNumOfElement; i++)
	{
		if(pstPmtInfo->stElementInfo[i].bElementCaFlag == TRUE)
		{
			type = eCasChannel_Scramble;
		}
	}

	*eChType = type;

	return;
}

static Handle_t local_CiMgr_GetActionBySlotId(HUINT16 usSlotId)
{
	HUINT32			i;
	CI_Context_t		*pstCiCxt;

	for(i = 0; i < CI_MAX_NUM_OF_ACT; i++)
	{
		pstCiCxt = &s_astCiContext[i];

		if(pstCiCxt->pCam != NULL)
		{
			if(pstCiCxt->pCam->usSlotId == usSlotId)
			{
				return pstCiCxt->hAction;
			}
		}
	}

	return HANDLE_NULL;
}

static CI_MmiData_t *local_CiMgr_GetListBySlotId(HUINT16 usSlotId)
{
	if(usSlotId >= CARM_MAX_CISLOT_NUMBER)
	{
		return NULL;
	}

	return &s_stMmiData[usSlotId];
}

STATIC HERROR local_CiMgr_AddMmiDataList(HUINT16 usSlotId, SvcCas_MmiData_t *pstMmiData)
{
	SvcCas_MmiData_t *pMmiData = NULL;
	CI_MmiData_t *pDataSet = NULL;

	if(pstMmiData == NULL)
	{
		HxLOG_Error("null\n");
		return ERR_FAIL;
	}

	pMmiData = (SvcCas_MmiData_t *) OxCAS_Malloc(sizeof(SvcCas_MmiData_t));
	if ( pMmiData == NULL )
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_memcpy(pMmiData, pstMmiData, sizeof(SvcCas_MmiData_t));

	// Get Root
	pDataSet = local_CiMgr_GetListBySlotId(usSlotId);

	// Append List.
	pDataSet->s_pstList = UTIL_LINKEDLIST_AppendListItemToPointerList(pDataSet->s_pstList, pMmiData);
	if(pDataSet->s_pstList == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxLOG_Info("Version(%d)\n", pMmiData->ulVersion);

	return ERR_OK;
}

static HERROR local_CiMgr_UpdateMmiData(HUINT16 usSlotId, HUINT32 ulVersion, Handle_t hMmi, MMI_REQ nMmiReq, void *pvParam, HULONG ulParamSize)
{
	HUINT32 nResult = 0;
	SvcCas_MmiData_t stMmiData;

	HxLOG_Info("slot(%d), ver(%d), hMmi(0x%x), param(%d) \n",
		usSlotId, ulVersion, (HUINT32)hMmi, (HUINT32)ulParamSize);

	// Mapping Data
	switch(nMmiReq)
	{
	case MMI_OPEN:
		stMmiData.eDataType = eCasMmiType_Open;
		stMmiData.eCasInstType = eCasInst_CI;
		stMmiData.hMmi = hMmi;
		stMmiData.usSlotId = usSlotId;
		stMmiData.ulVersion = ulVersion;
		stMmiData.pvMmiData = NULL;
		stMmiData.ulMmiDataSize = ulParamSize;
		break;

	case MMI_CLOSE:
		stMmiData.eDataType = eCasMmiType_Close;
		stMmiData.eCasInstType = eCasInst_CI;
		stMmiData.hMmi = hMmi;
		stMmiData.usSlotId = usSlotId;
		stMmiData.ulVersion = ulVersion;
		stMmiData.pvMmiData = NULL;
		stMmiData.ulMmiDataSize = ulParamSize;
		break;

	case MMI_ENQ:
		{
			MMI_ENQ_PARAM *pEnq = NULL;

			pEnq = (MMI_ENQ_PARAM *)pvParam;
			if(pEnq == NULL)
			{
				break;
			}

			#ifdef CONFIG_DEBUG
			HxLOG_Info("[MMI_ENQ] \n\t [%s] --> blind = %d, answer length = %d \n", pEnq->pszPrompt, pEnq->ucBlindOnOff, pEnq->ucAnswerLen);
			#endif

			stMmiData.eDataType = eCasMmiType_Enq;
			stMmiData.eCasInstType = eCasInst_CI;
			stMmiData.hMmi = hMmi;
			stMmiData.usSlotId = usSlotId;
			stMmiData.ulVersion = ulVersion;
			stMmiData.pvMmiData = (MMI_ENQ_PARAM *)pvParam;
			stMmiData.ulMmiDataSize = ulParamSize;
		}
		break;

	case MMI_MENU:
		stMmiData.eDataType = eCasMmiType_Menu;
		stMmiData.eCasInstType = eCasInst_CI;
		stMmiData.hMmi = hMmi;
		stMmiData.usSlotId = usSlotId;
		stMmiData.ulVersion = ulVersion;
		stMmiData.pvMmiData = (MMI_MENU_LIST_PARAM *)pvParam;

		#ifdef CONFIG_DEBUG
		{
			MMI_MENU_LIST_PARAM *pTemp = (MMI_MENU_LIST_PARAM *)pvParam;
			int i;
			HxLOG_Print("[MMI_MENU] \n");
			HxLOG_Print("\t [%s] \n", pTemp->pszTitle);
			HxLOG_Print("\t [%s] \n", pTemp->pszSubTitle);
			for (i = 0; i < pTemp->ulNumOfItem; i++)
				HxLOG_Print("\t\t [%02d:%s] \n", i+1, pTemp->ppszItem[i]);
			HxLOG_Print("\t [%s] \n", pTemp->pszBottom);
		}
		#endif

		stMmiData.ulMmiDataSize = ulParamSize;
		break;

	case MMI_LIST:
		stMmiData.eDataType = eCasMmiType_List;
		stMmiData.eCasInstType = eCasInst_CI;
		stMmiData.hMmi = hMmi;
		stMmiData.usSlotId = usSlotId;
		stMmiData.ulVersion = ulVersion;
		stMmiData.pvMmiData = (MMI_MENU_LIST_PARAM *)pvParam;

		#ifdef CONFIG_DEBUG
		{
			MMI_MENU_LIST_PARAM *pTemp = (MMI_MENU_LIST_PARAM *)pvParam;
			int i;
			HxLOG_Print("[MMI_LIST] \n");
			HxLOG_Print("\t [%s] \n", pTemp->pszTitle);
			HxLOG_Print("\t [%s] \n", pTemp->pszSubTitle);
			for (i = 0; i < pTemp->ulNumOfItem; i++)
				HxLOG_Print("\t\t [%02d:%s] \n", i+1, pTemp->ppszItem[i]);
			HxLOG_Print("\t [%s] \n", pTemp->pszBottom);
		}
		#endif

		stMmiData.ulMmiDataSize = ulParamSize;
		break;

	case MMI_TEXT:
		stMmiData.eDataType = eCasMmiType_Text;
		stMmiData.eCasInstType = eCasInst_CI;
		stMmiData.hMmi = hMmi;
		stMmiData.usSlotId = usSlotId;
		stMmiData.ulVersion = ulVersion;
		stMmiData.pvMmiData = (HUINT8 *)pvParam;

		#ifdef CONFIG_DEBUG
		HxLOG_Print("[MMI_TEXT] \n");
		HxLOG_Print("\t [%s] \n", (HUINT8 *)pvParam);
		#endif
		stMmiData.ulMmiDataSize = ulParamSize;
		break;

	default :
		return ERR_FAIL;
	}

	nResult = local_CiMgr_AddMmiDataList(usSlotId, &stMmiData);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("failure in add mmi data in list\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HBOOL local_CiMgr_ListFilter(void *pvItem, void *pvFilter)
{
	SvcCas_MmiData_t *pData = (SvcCas_MmiData_t *)pvItem;
	HUINT32 ulVersion = *(HUINT32*)pvFilter;

	if(pData->ulVersion == ulVersion)
	{
		return TRUE;
	}

	return FALSE;
}

static void local_CiMgr_DelMmiDataList(void *pstMmiData)
{
	SvcCas_MmiData_t *pstParam = NULL;

	if(pstMmiData == NULL)
	{
		HxLOG_Error("null\n");
		return;
	}

	pstParam = (SvcCas_MmiData_t *)pstMmiData;

	if(pstParam->pvMmiData != NULL)
	{
		HxLOG_Info("version(%d)' data was deleted\n", pstParam->ulVersion);
		OxCAS_Free(pstParam->pvMmiData);
		pstParam->pvMmiData = NULL;
	}

	HxLOG_Info("version(%d) was deleted\n", pstParam->ulVersion);

	OxCAS_Free(pstParam);
	pstParam = NULL;

	return;
}


////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_INSTANCE_MANAGEMENT___
////////////////////////////////////////////////////////////
static CI_Context_t * local_CiMgr_GetContextByAction(Handle_t hAction)
{
	HUINT32 ulActionId = 0;

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("handle is null. \n");
		return (CI_Context_t *)NULL;
	}

	ulActionId = svc_cas_SubSvcRmGetActionId(hAction);
	if(ulActionId >= CI_MAX_NUM_OF_ACT)
	{
		HxLOG_Error("action id is wrong. \n");
		return (CI_Context_t *)NULL;
	}

	return &s_astCiContext[ulActionId];

}

static CI_Context_t * local_CiMgr_GetContextByActionId(HUINT32 ulActionId)
{
	if(ulActionId >= CI_MAX_NUM_OF_ACT)
	{
		HxLOG_Error("action id is wrong. \n");
		return (CI_Context_t *)NULL;
	}

	return &s_astCiContext[ulActionId];
}

static HERROR local_CiMgr_AddActionToCamInstance(CI_CamInst_t *pstCam, Handle_t hAction)
{
	HBOOL		bFound = FALSE;
	HUINT32		i = 0;


	if (pstCam == NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		if (pstCam->hActList[i] == hAction)
		{
			return ERR_OK;
		}
	}

	for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		if (pstCam->hActList[i] == HANDLE_NULL)
		{
			bFound = TRUE;
			pstCam->hActList[i] = hAction;
			pstCam->ulNumOfAct++;
			break;
		}
	}

	if (pstCam->ulNumOfAct >= CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	if(bFound != TRUE)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static CI_Context_t *local_CiMgr_GetCiContextBySlotId(HUINT16 usSlotId)
{
	HUINT32 i = 0;
	CI_Context_t	*pstCiContext;

	for(i=0; i<CAS_MAX_NUM_OF_ACTION; i++)
	{
		pstCiContext = &s_astCiContext[i];

		if(pstCiContext->pCam == NULL)
		{
			continue;
		}

		if(pstCiContext->pCam->usSlotId == usSlotId)
		{
			return pstCiContext;
		}
	}

	return NULL;
}

static HERROR local_CiMgr_DeleteActionToCamInstance(CI_CamInst_t *pCam, Handle_t hAction)
{
	HUINT32 i = 0;
	HBOOL bFound = FALSE;

	if (pCam == NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	if (pCam->ulNumOfAct == 0)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		if (pCam->hActList[i] == hAction)
		{
			bFound = TRUE;
			pCam->hActList[i] = HANDLE_NULL;
			pCam->ulNumOfAct--;
		}
	}

	if(bFound != TRUE)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HBOOL local_CiMgr_CheckActionHandleInCamInstance(CI_CamInst_t *pCam, CI_Context_t *pCiContext)
{
	HUINT32 i = 0;

	if (pCam == NULL || pCiContext == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		if (pCam->hActList[i] != HANDLE_NULL)
		{
			HxLOG_Print("(0x%x) (0x%x) \n", pCam->hActList[i], pCiContext->hAction);

			if (pCam->hActList[i] == pCiContext->hAction)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

static HERROR local_CiMgr_CheckCaPmtSending(CI_CamInst_t *pstCam, CI_Context_t *pCiContext, HBOOL *bIsNeedSend)
{
	HUINT32 i = 0;
	CI_Context_t *pTemp = (CI_Context_t *)NULL;
	HBOOL bIsSameSvc = FALSE;

	if (pstCam == NULL || pCiContext == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	*bIsNeedSend = FALSE;

	HxLOG_Info("hAction (0x%08x)\n", pCiContext->hAction);

	#ifdef DEBUG_ACT_LIST
	local_CiMgr_PrintActionList(pstCam);
	#endif

	if (pstCam->ulNumOfAct == 0)
	{
		HxLOG_Info(" ulNumOfAct is zero \n");
		*bIsNeedSend = TRUE;
		pCiContext->bSendCaPmt = TRUE;
		pstCam->ucCurPmtVer = pCiContext->stProgInfo.ucPrePmtVer;
		pstCam->ulCurPmtCrc = pCiContext->stProgInfo.ulPrePmtCrc;

	}
	else
	{
		for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
		{
			if (pstCam->hActList[i] != HANDLE_NULL)
			{
				pTemp = local_CiMgr_GetContextByAction(pstCam->hActList[i]);
				if(pTemp == NULL)
				{
					HxLOG_Critical("\n");
					continue;
				}

				if (pTemp == pCiContext)
				{
					continue;
				}

				if (pTemp->hSvc == pCiContext->hSvc)
				{
					HxLOG_Info("Temp->hAction (0x%x), Temp->hSvc (0x%x), pCiContext->hAction (0x%x), pCiContext->hSvc (0x%x), pTemp->bSendCaPmt (%d)\n",
						pTemp->hAction, pTemp->hSvc, pCiContext->hAction, pCiContext->hSvc, pTemp->bSendCaPmt);

					if (pTemp->bSendCaPmt == TRUE)
					{
						bIsSameSvc = TRUE;
						break;
					}
				}
			}
		}

		HxLOG_Info("bIsSameSvc (%d) \n", bIsSameSvc);

		if (bIsSameSvc == TRUE)
		{
			pCiContext->bSendCaPmt = TRUE;

#if 1
			/* same service 지만, pmt version이 변경 된 경우에는 ca pmt를 보낸다. */
			if (*bIsNeedSend == FALSE)
			{
				HxLOG_Info("cam version (%x) context version(%x) cam crc (%x) context crc (%x)\n",
					pstCam->ucCurPmtVer, pCiContext->stProgInfo.ucPrePmtVer, pstCam->ulCurPmtCrc, pCiContext->stProgInfo.ulPrePmtCrc);

				if((pstCam->ucCurPmtVer != pCiContext->stProgInfo.ucPrePmtVer)
					&& (pstCam->ulCurPmtCrc != pCiContext->stProgInfo.ulPrePmtCrc))
				{
					pCiContext->bSendCaPmt = TRUE;
					*bIsNeedSend = TRUE;
					pstCam->ucCurPmtVer = pCiContext->stProgInfo.ucPrePmtVer;
					pstCam->ulCurPmtCrc = pCiContext->stProgInfo.ulPrePmtCrc;
				}
			}
#endif
		}
		else
		{
			if (pCiContext->eActType == eCAS_ActionType_Live
				|| pCiContext->eActType == eCAS_ActionType_Playback
				|| pCiContext->eActType == eCAS_ActionType_Tsr_Play)
			{
				pCiContext->bSendCaPmt = TRUE;
				*bIsNeedSend = TRUE;
				pstCam->ucCurPmtVer = pCiContext->stProgInfo.ucPrePmtVer;
				pstCam->ulCurPmtCrc = pCiContext->stProgInfo.ulPrePmtCrc;

			}
			else
			{
				pCiContext->bSendCaPmt = FALSE;
			}
		}
	}
	HxLOG_Info(" bIsNeedSend (%d)\n", *bIsNeedSend);

	#ifdef DEBUG_ACT_LIST
	local_CiMgr_PrintActionList(pstCam);
	#endif

	return ERR_OK;
}

static HERROR local_CiMgr_PlugInCi(HUINT32 ulActionId, HINT32 nSlotId)
{
	HINT32 nCiSlotId;

#if 0 	// TODO: scramble 채널에서 CAM 삽입 할 경우 Record Start가 발생하지 않아서 아래 조건문 넣은것으로 보임 (PH1 branch repo rev.102762 swkim 수정사항)
	if(s_astCiContext[ulActionId].eActType ==  eCAS_ActionType_Playback || s_astCiContext[ulActionId].eActType ==  eCAS_ActionType_Tsr_Play)
	{
		return ERR_OK;
	}
#endif	// 위 조건 때문에 Scrambled Rec contents playback 시 descramble 루틴을 타지 못하여 일단 수정 함. (인증 작업시 위에 내용 고려해서 근본적으로 고치는 작업 필요)
	nCiSlotId = nSlotId;

	return svc_cas_DevCiPlugInCi(ulActionId, nCiSlotId);
}

static HUINT32 local_CiMgr_GetNumberOfSharedSvc(CI_Context_t *pContext)
{
	HUINT32			i;
	HUINT32			unNumberOfSharedSvc = 0;
	CI_Context_t		*pstCiContext;

	for(i = 0; i < CI_MAX_NUM_OF_ACT; i++)
	{
		pstCiContext = &s_astCiContext[i];
		if(pstCiContext == pContext)
		{
			continue;
		}

		if(pstCiContext->pCam == pContext->pCam)
		{
			unNumberOfSharedSvc++;
		}
	}

	return unNumberOfSharedSvc;
}

#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
static HUINT32 local_CiMgr_GetNumberOfExistCam(void)
{
	HUINT32	i;
	HUINT32	unNumberOfExistCam = 0;

	for(i = 0 ; i < CARM_MAX_CISLOT_NUMBER; i++)
	{
		if(s_astCiCam[i].eCamState == CI_CAM_INITIALIZED)
		{
			unNumberOfExistCam++;
		}
	}

	return unNumberOfExistCam;
}

static CI_Context_t *local_CiMgr_GetOtherSvcContext(CI_Context_t *pContext, SvcCas_ActionType_e eActType)
{
	HUINT32 i;

	for(i = 0; i < CI_MAX_NUM_OF_ACT; i++)
	{
		if(&s_astCiContext[i] == pContext)
			continue;

		if(s_astCiContext[i].pCam == NULL || s_astCiContext[i].pCam == pContext->pCam)
			continue;

		// eCAS_ActionType_Max : 해당되는거 아무거나 찾는다. 있는지만 확인하는 용도.
		if(s_astCiContext[i].eState > eCiMgrState_Stopped && (eActType == eCAS_ActionType_Max || s_astCiContext[i].eActType == eActType))
			return &s_astCiContext[i];
	}

	return NULL;
}

static CI_Context_t *local_CiMgr_GetSameSvcContext(CI_Context_t *pContext, SvcCas_ActionType_e eActType)
{
	HUINT32 i;

	for(i = 0; i < CI_MAX_NUM_OF_ACT; i++)
	{
		if(&s_astCiContext[i] == pContext)
			continue;

		if(s_astCiContext[i].pCam == NULL || s_astCiContext[i].pCam != pContext->pCam)
			continue;

		if(s_astCiContext[i].eState > eCiMgrState_Stopped && (eActType == eCAS_ActionType_Max || s_astCiContext[i].eActType == eActType))
			return &s_astCiContext[i];
	}

	return NULL;
}

static HUINT32 local_CiMgr_GetNumberOfConnectedSvc(CI_Context_t *pOrgContext)
{
	HUINT32			i;
	HUINT32			unNumberOfConnectedSvc = 0;
	CI_Context_t		*pstCiContext;

	for(i = 0; i < CI_MAX_NUM_OF_ACT; i++)
	{
		pstCiContext = &s_astCiContext[i];

		if(pstCiContext == pOrgContext)
		{
			continue;
		}

		if(pstCiContext->pCam != NULL)
		{
			unNumberOfConnectedSvc++;
		}
	}

	return unNumberOfConnectedSvc;
}

static HBOOL local_CiMgr_IsDaisyChainPath(void)
{
	HUINT32 				i;
	HUINT32 				unNumberOfCi;
	HERROR				hErr;
	SvcCas_DevCiPathway_e	eCiPathWay;

	unNumberOfCi = svc_cas_DevCiGetNumberOfModule();

	for(i = 0; i < unNumberOfCi; i++)
	{
		hErr = svc_cas_DevCiGetCurrentPathWay(i, &eCiPathWay);
		if(hErr == ERR_OK && eCiPathWay == eCAS_CI_PATH_DAISYCHAIN_PASSTHROUGH)
		{
			return TRUE;
		}
	}
	return FALSE;
}

static HBOOL local_CiMgr_IsCustomPath(void)
{
	HUINT32				i;
	HUINT32				unNumberOfCi;
	HERROR				hErr;
	SvcCas_DevCiPathway_e	eCiPathWay;

	unNumberOfCi = svc_cas_DevCiGetNumberOfModule();

	for(i = 0; i < unNumberOfCi; i++)
	{
		hErr = svc_cas_DevCiGetCurrentPathWay(i, &eCiPathWay);
		if(hErr == ERR_OK && eCiPathWay == eCAS_CI_PATH_CUSTOM_PASSTHROUGH)
		{
			return TRUE;
		}
	}
	return FALSE;
}

static HBOOL local_CiMgr_IsNormalPath(void)
{
	HUINT32				i;
	HUINT32				unNumberOfCi;
	HERROR				hErr;
	SvcCas_DevCiPathway_e	eCiPathWay;

	unNumberOfCi = svc_cas_DevCiGetNumberOfModule();

	for(i = 0; i < unNumberOfCi; i++)
	{
		hErr = svc_cas_DevCiGetCurrentPathWay(i, &eCiPathWay);
		if(hErr == ERR_OK && eCiPathWay == eCAS_CI_PATH_NORMAL_PASSTHROUGH)
		{
			return TRUE;
		}
	}
	return FALSE;
}

static CI_Context_t *local_CiMgr_GetSameTpContext(CI_Context_t *pOrgContext)
{
	HUINT32			i;
	CI_Context_t		*pstCiContext;

	for(i = 0; i < CI_MAX_NUM_OF_ACT; i++)
	{
		pstCiContext = &s_astCiContext[i];
		if(		(pstCiContext == pOrgContext)
			||	(pstCiContext->hAction == HANDLE_NULL)
			||	(pstCiContext->pCam == NULL))
		{
			continue;
		}

		if(pstCiContext->ulTunerId == pOrgContext->ulTunerId)
		{
			return pstCiContext;
		}
	}

	return NULL;
}

static CI_Context_t *local_CiMgr_GetPossibleSharedSvc(CI_Context_t *pOrgContext)
{

	HUINT32 			i;
	CI_Context_t		*pstCiContext = NULL;

	for(i = 0; i < CI_MAX_NUM_OF_ACT; i++)
	{
		pstCiContext = &s_astCiContext[i];

		if(pstCiContext == pOrgContext)
		{
			continue;
		}

		if(pstCiContext->pCam == pOrgContext->pCam)
		{
			return pstCiContext;
		}
	}

	return NULL;
}

static CI_Context_t *local_CiMgr_GetNoSharedCiSvc(CI_Context_t *pOrgContext)
{

	HUINT32 			i;
	CI_Context_t		*pstCiContext = NULL;

	for(i = 0; i < CI_MAX_NUM_OF_ACT; i++)
	{
		pstCiContext = &s_astCiContext[i];

		if(pstCiContext == pOrgContext)
		{
			continue;
		}

		if(pstCiContext->pCam == NULL)
		{
			continue;
		}

		if(pstCiContext->pCam != pOrgContext->pCam)
		{
			return pstCiContext;
		}
	}

	return NULL;
}

#if 1
static HERROR local_CiMgr_StartPathExtControl(CI_Context_t *pOrgContext)
{
	HUINT32	unNumOfExistCam;
	HUINT16	usSlotId;
	CI_Context_t		*pstContext;
	SvcCas_DevCiPathway_e	eCarmCiPathWay;

	/*
		CAM이 없거나 3개 이상이다.
			현재 구현된 내용으로는 나와서는 안된다.

		CAM이 한개다. : NORMAL PATH

		CAM이 두개다.
			다른 CAM에 연결된 서비스가 있다.
				1. 새로운 서비스와 다른 TP의 서비스이다 : NORMAL PATH
				2. 새로운 서비스와 같은 TP의 서비스이다 : DAISYCHAIN PATH

			다른 CAM에 연결된 서비스가 없다.
				1. 새로운 서비스가 Live, TSR, Recording이다 : CUSTOM PATH
				2. 새로운 서비스가 그 외다 : NORMAL PATH
					TSR playback, Playback...
	*/

	unNumOfExistCam = local_CiMgr_GetNumberOfExistCam();
	if(unNumOfExistCam == 0 || unNumOfExistCam > 2)
	{
		HxLOG_Error("Invalid unNumOfExistCam (%d)\n", unNumOfExistCam);
		return ERR_FAIL;
	}

	eCarmCiPathWay = eCAS_CI_PATH_NORMAL_PASSTHROUGH;
	usSlotId = pOrgContext->pCam->usSlotId;

	if(unNumOfExistCam == 1)	//CAM이 한개다.
	{
		eCarmCiPathWay = eCAS_CI_PATH_NORMAL_PASSTHROUGH;
	}

	if(unNumOfExistCam == 2)	//CAM이 두개다.
	{
		pstContext = local_CiMgr_GetOtherSvcContext(pOrgContext, eCAS_ActionType_Max);
		if(pstContext != NULL)	// 다른 CAM에 서비스가 있다.
		{
			if(pstContext->ulTunerId != pOrgContext->ulTunerId)
			{
				eCarmCiPathWay = eCAS_CI_PATH_NORMAL_PASSTHROUGH;
			}
			else
			{
#if 0
				if(local_CiMgr_IsDaisyChainPath() == TRUE)
				{
					HxLOG_Print("Already daisy chain path...\n");
					return ERR_OK;
				}

				s_usDaisyChainSlotId = pOrgContext->pCam->usSlotId == 0 ? 1 : 0;
				HxLOG_Print("DaisyChain will be set with slot #%d\n", s_usDaisyChainSlotId);

				usSlotId = s_usDaisyChainSlotId;
				eCarmCiPathWay = eCAS_CI_PATH_DAISYCHAIN_PASSTHROUGH;
#else
				usSlotId = pstContext->pCam->usSlotId;
				eCarmCiPathWay = eCAS_CI_PATH_DAISYCHAIN_PASSTHROUGH;
#endif
			}
		}
		else	// 다른 CAM에 서비스가 없다.
		{
			switch(pOrgContext->eActType)
			{
				case eCAS_ActionType_Live:
				case eCAS_ActionType_Tsr_Rec:
				case eCAS_ActionType_Recording:
					eCarmCiPathWay = eCAS_CI_PATH_CUSTOM_PASSTHROUGH;
					break;

				case eCAS_ActionType_Tsr_Play:	// 명확히 표기하기 위해...
				case eCAS_ActionType_Playback:	// 명확히 표기하기 위해...
				default:
					eCarmCiPathWay = eCAS_CI_PATH_NORMAL_PASSTHROUGH;
					break;
			}
		}
	}

	if(svc_cas_DevCiSetExtendedTsPath(pOrgContext->ulTunerId, usSlotId, pOrgContext->ulDemuxId, eCarmCiPathWay) != ERR_OK)
	{
		HxLOG_Error("svc_cas_DevCiSetExtendedTsPath() error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
#else
static HERROR local_CiMgr_StartPathExtControl(CI_Context_t *pOrgContext)
{
	HUINT32		unTunerId = -1;
	HUINT32		unNumberOfConnectedSvc = local_CiMgr_GetNumberOfConnectedSvc(pOrgContext);
	HUINT32		unNumOfExistCam = local_CiMgr_GetNumberOfExistCam();
	HUINT32		unNumfOfShareSvc = local_CiMgr_GetNumberOfSharedSvc(pOrgContext);
	HERROR		hErr;
	CI_Context_t		*pContext;
	SvcCas_DevCiPathway_e 	eCarmCiPathWay = eCAS_CI_PATH_NORMAL_PASSTHROUGH;
	HUINT16		usSlotId = pOrgContext->pCam->usSlotId;
#if 1
	/* 기존에 연결된 서비스가 있다.
		CAM이 한개다.
			공유 할수 있으면 이미 연결되어 있으므로 path 설정하지 않고 return한다.
			--> 특정 Action에 대해서만 간단히 체크해 보고 PATH 설정은 무조건 내린다. 단, PAL에서 해당 Slot에 대한 내용은 관리하고 있다.
		CAM이 두개다.
			새로운 서비스가 Live, TSR Rec, Recording이 아니면 --> TSR play, Playback, ...
				CARM_CI_NORMAL_PASSTHROUGH로 path 구성 다른 CAM은 신경 쓰지 않는다.
			새로운 서비스가 Live, TSR Rec, Recording이면
				다른 CAM이 Playback, recording 이면
					현 서비스와 공유 한 CAM이 없으면 NORMAL_PASSTHROUGH 를 구성하도록 한다.
					공유 한 CAM이 있으면 다시 path 구성할 필요 없음.
				공유할 CAM이 없으면
					1. 현 path가 CUSTOM_PASSTHROUGH이다.
						TP를 공유 할수 없으면 NORMAL_PASSTHROUGH 를 구성하도록 한다.
						TP를 공유 할수 있으면 DAISYCHAIN_PASSTHROUGH로 변경한다.
					2. 현 path가 DAISYCHAIN_PASSTHROUGH이다.
						path 구성할 필요 없음. 이전 path 그냥 이용하면됨.
					3. 현 path가 CARM_CI_NORMAL_PASSTHROUGH이다.
						TP를 공유 할수 없으면 NORMAL_PASSTHROUGH 를 구성하도록 한다.
						TP를 공유 할수 있으면 DAISYCHAIN_PASSTHROUGH로 변경한다.
					4. 그 외의 path는 NORMAL_PASSTHROUGH 를 구성하도록 한다.

					1, 3번은 내용이 같다. 합 할 수 있는가?
				공유할 CAM이 있으면
					path 구성할 필요 없음. 이전 path 그냥 이용하면됨.
	*/

	if(unNumOfExistCam == 1)	//CAM이 한개다.
	{
		/*  공유 할수 있는 Live와 TSR외에는 에러다.. 디버깅 필요.
			왜? 현재 CAM이 이미 사용중인데 다른 녀석이 사용하려 한다..  그런경우는 LIVE, TSR밖에 없다.
				live streaming은 담에 생각하자.
			--> Scrambled 영역이 포함된 TSR Play의 경우 CAM을 필요로 한다. 아래 내용을 변경함.	*/
		if((pOrgContext->eActType == eCAS_ActionType_Live) || (pOrgContext->eActType == eCAS_ActionType_Tsr_Rec))
		{
			if(unNumfOfShareSvc > 1)
			{
				HxLOG_Critical("unNumfOfShareSvc > 1\n");
				return ERR_FAIL;
			}
		}

		/*	공유 할수 있으면 이미 연결되어 있으므로 path 설정하지 않고 return한다.
			--> PATH 설정은 무조건 내리되 PAL에서 동일한 PATH에 대해 재구성하지 않도록 작업 추가함.
				즉, 1개일 경우에는 내려 달라고 하는대로 내려버림.	*/
		eCarmCiPathWay = eCAS_CI_PATH_NORMAL_PASSTHROUGH;
		goto ONLY_CONNECT;
	}

	if(unNumOfExistCam == 2)	//CAM이 두개다.
	{
		switch(pOrgContext->eActType)
		{
			case eCAS_ActionType_Live:
			case eCAS_ActionType_Tsr_Rec:
			case eCAS_ActionType_Recording:
				/*	다른 CAM이 Playback, recording 이고 현 서비스와 공유 한 CAM이 없으면
					NORMAL_PASSTHROUGH 를 구성하도록 한다.
					--> 다른 CAM의 현재 Action이 TSR play, Playback, Recording일 경우 NORMAL PATH 구성한다.*/
				if(local_CiMgr_GetOtherSvcContext(pOrgContext, eCAS_ActionType_Tsr_Play) != NULL
					|| local_CiMgr_GetOtherSvcContext(pOrgContext, eCAS_ActionType_Playback) != NULL)
				{
					eCarmCiPathWay = eCAS_CI_PATH_NORMAL_PASSTHROUGH;
					break;
				}

				if((pOrgContext->eActType == eCAS_ActionType_Live)
					|| (pOrgContext->eActType == eCAS_ActionType_Tsr_Rec))
				{
					HINT32 i;

					if(unNumfOfShareSvc > 1)
					{
						HxLOG_Critical("unNumfOfShareSvc > 1\n");
						return ERR_FAIL;
					}

#if 0
					if(local_CiMgr_GetOtherSvcContext(pOrgContext, eCAS_ActionType_Recording) == NULL
#if defined(CONFIG_MW_MM_DELAYED_REC)
						&& local_CiMgr_GetOtherSvcContext(pOrgContext, eCAS_ActionType_Tsr_Rec) == NULL
#endif
						)
#else
					// 다른 CAM에 CONNECT된 서비스가 있는지 찾아보자. 없으면 CUSTOM PATH이다.
					for(i = 0; i < CI_MAX_NUM_OF_ACT; i++)
					{
						if(&s_astCiContext[i] == pOrgContext)
							continue;

						if(s_astCiContext[i].pCam == NULL || s_astCiContext[i].eState == eCiMgrState_Stopped)
							continue;

						if(s_astCiContext[i].pCam != pOrgContext->pCam)
							break;
					}

					if(i == CI_MAX_NUM_OF_ACT)
#endif
					{
						eCarmCiPathWay = eCAS_CI_PATH_CUSTOM_PASSTHROUGH;
						break;
					}
				}

				// 공유할 CAM이 없으면
				if(local_CiMgr_GetPossibleSharedSvc(pOrgContext) == NULL)
				{
					pContext = local_CiMgr_GetSameTpContext(pOrgContext);
					if(local_CiMgr_IsCustomPath() == TRUE)	//현 path가 CUSTOM_PASSTHROUGH이다.
					{
						if(pContext == NULL)	// TP를 공유 할수 없으면 NORMAL_PASSTHROUGH 를 구성하도록 한다.
						{
							eCarmCiPathWay = eCAS_CI_PATH_NORMAL_PASSTHROUGH;
						}
						else	//TP를 공유 할수 있으면 DAISYCHAIN_PASSTHROUGH로  변경한다.
						{
							usSlotId = pOrgContext->pCam->usSlotId == 0 ? 1 : 0;
							eCarmCiPathWay = eCAS_CI_PATH_DAISYCHAIN_PASSTHROUGH;
						}
					}
					else if(local_CiMgr_IsNormalPath() == TRUE)	//현 path가 NORMAL_PASSTHROUGH 이다.
					{
						if(pContext == NULL)	// TP를 공유 할수 없으면 NORMAL_PASSTHROUGH 를 구성하도록 한다.
						{
							eCarmCiPathWay = eCAS_CI_PATH_NORMAL_PASSTHROUGH;
						}
						else	//TP를 공유 할수 있으면 DAISYCHAIN_PASSTHROUGH로  변경한다.
						{
							usSlotId = pOrgContext->pCam->usSlotId == 0 ? 1 : 0;
							eCarmCiPathWay = eCAS_CI_PATH_DAISYCHAIN_PASSTHROUGH;
						}
					}
					else	//현 path가 DAISYCHAIN_PASSTHROUGH이다.
					{
						if(pContext == NULL)	// TP를 공유 할수 없으면 NORMAL_PASSTHROUGH 를 구성하도록 한다.
						{
							eCarmCiPathWay = eCAS_CI_PATH_NORMAL_PASSTHROUGH;
						}
						else	// 이미 daisy chain이기에 할 필요 없다.
						{
							usSlotId = pOrgContext->pCam->usSlotId == 0 ? 1 : 0;
							eCarmCiPathWay = eCAS_CI_PATH_DAISYCHAIN_PASSTHROUGH;
						}
					}
				}
				else	// 해당 CAM이 서비스가 되고 있는 상황이다. 기존 정보를 그대로 이용하자.
				{
					return ERR_OK;
				}
				break;

			default:
				//CARM_CI_NORMAL_PASSTHROUGH로 path 구성. 다른 CAM은 신경 쓰지 않는다.
				eCarmCiPathWay = eCAS_CI_PATH_NORMAL_PASSTHROUGH;
				break;
		}
	}

ONLY_CONNECT:
	if( (pOrgContext->eActType == eCAS_ActionType_Live)
		|| (pOrgContext->eActType == eCAS_ActionType_Tsr_Rec)
		|| (pOrgContext->eActType == eCAS_ActionType_Recording) )
	{
		if(svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(pOrgContext->hAction), eCAS_RM_ITEM_TUNER_GROUP, &unTunerId) != ERR_OK)
		{
			HxLOG_Critical("svc_cas_SubSvcRmGetResourceId() error!!!\n");
		}
	}

	svc_cas_DevCiSetExtendedTsPath(unTunerId, usSlotId, pOrgContext->ulDemuxId, eCarmCiPathWay);
	return ERR_OK;
#else
	/* 기존에 연결된 서비스가 없는데
			CAM이 두개다.
				Live나 TSR은 CAM이 두개면 다른 CAM에 TS를 넣도록 하자. 그외에는 다른 CAM에 TS를 넣지 않도록 한다.
			CAM이 한개다.
				그럼 걍 CARM_CI_NORMAL_PASSTHROUGH로 연결
	*/
	if(unNumberOfConnectedSvc == 0)
	{
		if(unNumOfExistCam == 2)
		{
			if(pOrgContext->eActType == eCAS_ActionType_Live || pOrgContext->eActType == eCAS_ActionType_Tsr_Rec)
			{
				eCarmCiPathWay = eCAS_CI_PATH_CUSTOM_PASSTHROUGH;
			}
		}
	}
	/* 기존에 연결된 서비스가 있다.
		CAM이 한개다.
			공유 할수 있으면 이미 연결되어 있으므로 path 설정하지 않고 return한다.
		CAM이 두개다.
			새로운 서비스가 Live나 TSR이 아니면
				CARM_CI_NORMAL_PASSTHROUGH로 path 구성 다른 CAM은 신경 쓰지 않는다.
			새로운 서비스가 Live나 TSR이면
				다른 CAM이 Playback, recording 이면
					현 서비스와 공유 한 CAM이 없으면 NORMAL_PASSTHROUGH 를 구성하도록 한다.
						공유 한 CAM이 있으면 다시 path 구성할 필요 없음.
				공유할 CAM이 없으면
					1. 현 path가 CUSTOM_PASSTHROUGH이다.
						TP를 공유 할수 없으면 NORMAL_PASSTHROUGH 를 구성하도록 한다.
						TP를 공유 할수 있으면 DAISYCHAIN_PASSTHROUGH로 변경한다.
					2. 현 path가 DAISYCHAIN_PASSTHROUGH이다.
						path 구성할 필요 없음. 이전 path 그냥 이용하면됨.
					3. 현 path가 CARM_CI_NORMAL_PASSTHROUGH이다.
						TP를 공유 할수 없으면 NORMAL_PASSTHROUGH 를 구성하도록 한다.
						TP를 공유 할수 있으면 DAISYCHAIN_PASSTHROUGH로 변경한다.
					4. 그 외의 path는 NORMAL_PASSTHROUGH 를 구성하도록 한다.

					1, 3번은 내용이 같다. 합 할 수 있는가?
				공유할 CAM이 있으면
					path 구성할 필요 없음. 이전 path 그냥 이용하면됨.
	*/
	else
	{
		//CAM이 한개다.
		if(unNumOfExistCam == 1)
		{
			/*  공유 할수 있는 Live와 TSR외에는 에러다.. 디버깅 필요.
				왜? 현재 CAM이 이미 사용중인데 다른 녀석이 사용하려 한다..  그런경우는 LIVE, TSR밖에 없다.
					live streaming은 담에 생각하자.
			*/
			if(		(pOrgContext->eActType != eCAS_ActionType_Live)
				&&	(pOrgContext->eActType != eCAS_ActionType_Tsr_Rec))
			{
				HxLOG_Critical("Critical Error!!!\n");
				return ERR_FAIL;
			}

			// live 혹은 TSR인데 이미 공유된 것이 있다? 그러면 에러다 디버깅 필요. */
			if(unNumfOfShareSvc > 1)
			{
				HxLOG_Critical("Critical Error!!!\n");
				return ERR_FAIL;
			}

			//공유 할수 있으면 이미 연결되어 있으므로 path 설정하지 않고 return한다.
			return ERR_OK;
		}
		//CAM이 두개다.
		else
		{
			//새로운 서비스가 Live나 TSR이면
			if(		(pOrgContext->eActType == eCAS_ActionType_Live)
				||	(pOrgContext->eActType == eCAS_ActionType_Tsr_Rec))
			{
				/*	다른 CAM이 Playback, recording 이고
						현 서비스와 공유 한 CAM이 없으면 CARM_CI_NORMAL_PASSTHROUGH 를 구성하도록 한다.
								   공유 한 CAM이 있으면 다시 path 구성할 필요 없음.
				*/
				pContext = local_CiMgr_GetNoSharedCiSvc(pOrgContext);
				if(		(pContext != NULL)
					&& 	(		(pContext->eActType == eCAS_ActionType_Playback)
							||	(pContext->eActType == eCAS_ActionType_Recording)))
				{
					if(unNumfOfShareSvc == 0)
					{
						goto ONLY_CONNECT;
					}

					return ERR_OK;
				}

				// 공유할 CAM이 없으면
				pContext = local_CiMgr_GetPossibleSharedSvc(pOrgContext);
				if(pContext == NULL)
				{
					//현 path가 CUSTOM_PASSTHROUGH이다.
					if(local_CiMgr_IsCustomPath() == TRUE)
					{
						// TP를 공유 할수 없으면 NORMAL_PASSTHROUGH 를 구성하도록 한다.
						pContext = local_CiMgr_GetSameTpContext(pOrgContext);
						if(pContext == NULL)
						{
							eCarmCiPathWay = eCAS_CI_PATH_NORMAL_PASSTHROUGH;
						}
						//TP를 공유 할수 있으면 DAISYCHAIN_PASSTHROUGH로  변경한다.
						else
						{
							eCarmCiPathWay = eCAS_CI_PATH_DAISYCHAIN_PASSTHROUGH;
						}
					}
					//현 path가 DAISYCHAIN_PASSTHROUGH이다.
					else if(local_CiMgr_IsDaisyChainPath() == TRUE)
					{
						// path 구성할 필요 없음. 이전 path 그냥 이용하면됨.
						return ERR_OK;
					}
					//현 path가 NORMAL_PASSTHROUGH 이다.
					else if(local_CiMgr_IsNormalPath() == TRUE)
					{
						// TP를 공유 할수 없으면 NORMAL_PASSTHROUGH 를 구성하도록 한다.
						pContext = local_CiMgr_GetSameTpContext(pOrgContext);
						if(pContext == NULL)
						{
							eCarmCiPathWay = eCAS_CI_PATH_NORMAL_PASSTHROUGH;
						}
						//TP를 공유 할수 있으면 DAISYCHAIN_PASSTHROUGH로  변경한다.
						else
						{
							eCarmCiPathWay = eCAS_CI_PATH_DAISYCHAIN_PASSTHROUGH;
						}
					}
					else
					{
						//그 외의 path는 NORMAL_PASSTHROUGH 를 구성하도록 한다.
						goto ONLY_CONNECT;
					}
				}
				else
				{
					//공유할 CAM이 있으면 현 상황 대로 그냥 연결 하면됨. path 구성할 필요 없음.
					return ERR_OK;
				}
			}
			//새로운 서비스가 playback이면
			else
			{
				//CARM_CI_NORMAL_PASSTHROUGH로 path 구성. 다른 CAM은 신경 쓰지 않는다.
				goto ONLY_CONNECT;
			}
		}
	}

ONLY_CONNECT:

	if( (pOrgContext->eActType == eCAS_ActionType_Live)
		|| (pOrgContext->eActType == eCAS_ActionType_Tsr_Rec)
		|| (pOrgContext->eActType == eCAS_ActionType_Recording) )
	{
		hErr = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(pOrgContext->hAction), eCAS_RM_ITEM_TUNER_GROUP, &unTunerId);
		if(hErr == ERR_OK)
		{
			svc_cas_DevCiSetExtendedTsPath(unTunerId, pOrgContext->pCam->usSlotId, pOrgContext->ulDemuxId, eCarmCiPathWay);
		}
		else
		{
			HxLOG_Critical("Critical Error!!!\n");
		}
	}
	else
	{
		svc_cas_DevCiSetExtendedTsPath(-1, pOrgContext->pCam->usSlotId, pOrgContext->ulDemuxId, eCarmCiPathWay);
	}

	return ERR_OK;
#endif
}
#endif

static HERROR local_CiMgr_StopPathExtControl (CI_Context_t *pContext)
{
	HUINT32		unNumOfExistCam;
	HUINT32		unNumOfSharedSvc;
	CI_Context_t	*pstOtherSvcContext = NULL;

	unNumOfExistCam = local_CiMgr_GetNumberOfExistCam();

	HxLOG_Info("unNumOfExistCam = %d,  pContext->hAction = 0x%x\n", unNumOfExistCam, pContext->hAction);

	/* CAM이 하나 혹은 없을 경우는 그냥 stop하면 된다. -> 다음번 service에서 연결을 관장한다.
		2개 있으면 공유하고 있는 서비스에 따라서 틀려진다. -> path를 여기서 다시 해야하는 경우가 발생한다.
	*/
	if(unNumOfExistCam == 2)
	{
		/* 공유하고 있는 서비스가 있는가?
			있으면 그냥 stop
			없으면 - daisy chain이 아니면 그냥 stop -> normal path일 경우에도 CUSTOM_PASSTHROUGH로 변경.
				   - daisy chain이면 남아 있는 Live서비스를 기준으로 CARM_CI_PATH_CUSTOM_PASSTHROUGH로 path 설정을 하도록한다.
		*/
		unNumOfSharedSvc = local_CiMgr_GetNumberOfSharedSvc(pContext);

		HxLOG_Info("unNumOfSharedSvc = %d \n", unNumOfSharedSvc);

		if(unNumOfSharedSvc == 0)
		{
			pstOtherSvcContext = local_CiMgr_GetOtherSvcContext(pContext, eCAS_ActionType_Max);
			if(pstOtherSvcContext != NULL)
			{
				svc_cas_DevCiSetExtendedTsPath(pstOtherSvcContext->ulTunerId, pstOtherSvcContext->pCam->usSlotId, pstOtherSvcContext->ulDemuxId, eCAS_CI_PATH_CUSTOM_PASSTHROUGH);
			}
		}
	}

	pContext->bPathConnected = FALSE;

	local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pContext->hAction), -1);

	return ERR_OK;
}

static HERROR local_CiMgr_ChangePathExtControl(CI_Context_t *pContext, HUINT16 usSlotId, HBOOL bConnect)
{
	HERROR		hErr;

	if(bConnect == TRUE)
	{
		hErr = local_CiMgr_StartPathExtControl(pContext);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("local_CiMgr_StartPathExtControl() error \n");
		}
	}
	else
	{
		hErr = local_CiMgr_StopPathExtControl(pContext);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("local_CiMgr_StopPathExtControl() error \n");
		}
	}

	return ERR_OK;
}

#endif

#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
static HERROR local_CiMgr_SetPath(CI_Context_t *pCiContext)
{
	CI_Context_t	*pstContext;

	switch(pCiContext->eActType)
	{
		case eCAS_ActionType_Live:
			pstContext = local_CiMgr_GetSameSvcContext(pCiContext, eCAS_ActionType_Tsr_Rec);
			if(pstContext == NULL)
			{
				HxLOG_Print("eCAS_ActionType_Live - pstContext NULL!\n");
				return ERR_OK;
			}
			break;

		default:
			return ERR_OK;
	}

	local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pstContext->hAction), pCiContext->pCam->usSlotId);

	return ERR_OK;
}

static HERROR local_CiMgr_ReleasePath(CI_Context_t *pCiContext)
{
	CI_Context_t	*pstContext;

	switch(pCiContext->eActType)
	{
		case eCAS_ActionType_Tsr_Play:
			pstContext = local_CiMgr_GetSameSvcContext(pCiContext, eCAS_ActionType_Tsr_Rec);
			if(pstContext == NULL)
			{
				HxLOG_Error("eCAS_ActionType_Tsr_Play - pstContext NULL!\n");
				return ERR_FAIL;
			}
			break;

		default:
			return ERR_OK;
	}

	local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pstContext->hAction), -1);

	return ERR_OK;
}
#else
static HERROR local_CiMgr_ChangePath(CI_Context_t *pContext, HUINT16 usSlotId, HBOOL bConnect)
{
	HINT32	nSlotId		= usSlotId;
	HUINT32 ulSrcId		= RxRSCID_NULL;
	HUINT32 ulDemuxId	= RxRSCID_NULL;
	HERROR	hErr;

	if (pContext == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	ulDemuxId = pContext->ulDemuxId;
	ulSrcId = pContext->ulTunerId;

	if(bConnect == FALSE)
	{
		nSlotId = -1;
	}

	switch(pContext->eActType)
	{
	case eCAS_ActionType_Recording:
	case eCAS_ActionType_Playback:
	case eCAS_ActionType_Descramble_a_File:
		ulSrcId = 0;
		break;

	case eCAS_ActionType_Live:
	case eCAS_ActionType_Tsr_Rec:
	case eCAS_ActionType_Tsr_Play:
		break;

	case eCAS_ActionType_None:
	case eCAS_ActionType_Data:
	case eCAS_ActionType_EngineeringTS:
	case eCAS_ActionType_RfVod:
	case eCAS_ActionType_Max:
		HxLOG_Critical("Critical Error!!!\n");
		return ERR_FAIL;
	}

	HxLOG_Error("Live resource id, tuner(%d) & ci(%d) & demux(%d) bConnect(%d) \n",	ulSrcId, nSlotId, ulDemuxId, bConnect);

	hErr = local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pContext->hAction), nSlotId);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("Critical Error!!!\n");
	}

	pContext->bPathConnected = bConnect;

	return ERR_OK;
}

static HERROR local_CiMgr_ClearPath(CI_CamInst_t *pCam)
{
	HUINT32 i = 0;
	CI_Context_t *pTemp = NULL;

	if (pCam == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		if (pCam->hActList[i] != HANDLE_NULL)
		{
			HxLOG_Print("hAction (0x%08x)\n", pCam->hActList[i]);

			pTemp = local_CiMgr_GetContextByAction(pCam->hActList[i]);
			if(pTemp == NULL)
			{
				HxLOG_Critical("\n");
				continue;
			}

			HxLOG_Print("hAction (0x%08x) is going to clean. \n", pCam->hActList[i]);

			local_CiMgr_ChangePath(pTemp, pCam->usSlotId, FALSE);

			pTemp->bSendCaPmt = FALSE;
		}
	}

	return ERR_OK;
}

static HERROR local_CiMgr_CheckPathChanging(CI_CamInst_t *pCam, CI_Context_t *pCiContext, HBOOL *bIsNeedPathChange)
{
	HUINT32 i = 0;
	CI_Context_t *pTemp = (CI_Context_t *)NULL;
	HBOOL bIsSameSvc = FALSE;

	if (pCam == NULL || pCiContext == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	*bIsNeedPathChange = FALSE;

	HxLOG_Info("hAction (0x%08x)\n", pCiContext->hAction);

	#ifdef DEBUG_ACT_LIST
	local_CiMgr_PrintActionList(pCam);
	#endif

	if (pCam->ulNumOfAct == 0)
	{
		HxLOG_Info(" ulNumOfAct is zero \n");
		*bIsNeedPathChange = TRUE;
	}
	else if (pCiContext->eActType == eCAS_ActionType_Descramble_a_File)
	{
		*bIsNeedPathChange = TRUE;
	}
	else
	{
		#if defined(CONFIG_DEBUG)
		HxLOG_Info("pCiContext->hAction (0x%x), pCiContext->hSvc (0x%x), pCiContext->eActType = %s\n",
							pCiContext->hAction, pCiContext->hSvc, local_CiMgr_MakeStrActionType(pCiContext->eActType));
		#endif

		for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
		{
			if (pCam->hActList[i] != HANDLE_NULL)
			{
				pTemp = local_CiMgr_GetContextByAction(pCam->hActList[i]);
				if(pTemp == NULL)
				{
					HxLOG_Critical("\n\n");
					continue;
				}

				if (pTemp == pCiContext)
				{
					continue;
				}

				if (pTemp->hSvc == pCiContext->hSvc)
				{
					#if defined(CONFIG_DEBUG)
					HxLOG_Info("Temp->hAction (0x%x), Temp->hSvc (0x%x), pTemp->bSendCaPmt (%d), pTemp->eActType = %s\n",
															pTemp->hAction, pTemp->hSvc, pTemp->bSendCaPmt, local_CiMgr_MakeStrActionType(pTemp->eActType));
					#endif

					if (pTemp->bPathConnected == TRUE)
					{
						bIsSameSvc = TRUE;
						break;
					}
				}
			}
		}

		HxLOG_Info("bIsSameSvc (%d) \n", bIsSameSvc);

		if (bIsSameSvc == TRUE)
		{
			*bIsNeedPathChange = TRUE;
		}
		else
		{
			if (pCiContext->eActType == eCAS_ActionType_Live
				|| pCiContext->eActType == eCAS_ActionType_Playback
				|| pCiContext->eActType == eCAS_ActionType_Tsr_Play)
			{
				HxLOG_Info(" pCiContext->eActType = %d \n", pCiContext->eActType);
				/* 캠과 연결된 다른 path release해야 함. */
				local_CiMgr_ClearPath(pCam);
				*bIsNeedPathChange = TRUE;
			}
		}
	}
	HxLOG_Info(" bIsNeedPathChange (%d)\n", *bIsNeedPathChange);

	#ifdef DEBUG_ACT_LIST
	local_CiMgr_PrintActionList(pCam);
	#endif

	return ERR_OK;
}

static HERROR local_CiMgr_SearchOtherActionForConnecting(CI_CamInst_t *pCam, CI_Context_t *pCurrent)
{
	HUINT32 i = 0;
	HERROR		hErr = ERR_OK;

	if (pCam == NULL || pCurrent == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	if ( (pCurrent->eActType == eCAS_ActionType_Live || pCurrent->eActType == eCAS_ActionType_Playback || pCurrent->eActType == eCAS_ActionType_Tsr_Play)
		!= TRUE)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	for(i = 0; i < CI_MAX_NUM_OF_ACT; i++)
	{
		if (s_astCiContext[i].hAction != HANDLE_NULL && s_astCiContext[i].hAction != pCurrent->hAction)
		{
			if (s_astCiContext[i].pCam != NULL)
			{
				if (s_astCiContext[i].eActType == eCAS_ActionType_Descramble_a_File)
				{
					HxLOG_Print("hAction (0x%08x) \n", s_astCiContext[i].hAction);
					local_CiMgr_ChangePath(&s_astCiContext[i], s_astCiContext[i].pCam->usSlotId, TRUE);

					local_CiMgr_AddActionToCamInstance(pCurrent->pCam, s_astCiContext[i].hAction);
					break;
				}

				if (s_astCiContext[i].hSvc == pCurrent->hSvc)
				{
					if (s_astCiContext[i].bPathConnected == FALSE)
					{
						HxLOG_Print("hAction (0x%08x) \n", s_astCiContext[i].hAction);
						local_CiMgr_ChangePath(&s_astCiContext[i], s_astCiContext[i].pCam->usSlotId, TRUE);

						local_CiMgr_AddActionToCamInstance(pCurrent->pCam, s_astCiContext[i].hAction);
					}
				}
				#if defined (CONFIG_MW_CI_PLUS)
				else
				{
					if (s_astCiContext[i].hAction != HANDLE_NULL
						&& s_astCiContext[i].ulDemuxId != CAS_RESOURCE_NONE
						&& s_astCiContext[i].pCam != NULL)

					{
						HUINT32 ulDemuxId = 0xffffffff;
						CI_CamInst_t *pCam = NULL;

						ulDemuxId = s_astCiContext[i].ulDemuxId;
						pCam = s_astCiContext[i].pCam;

						if (pCam->ulNumOfDsc[ulDemuxId] != 0)
						{
							HUINT32 j = 0;

							for (j=0; j<pCam->ulNumOfDsc[ulDemuxId]; j++)
							{
								/* descrambler close */
								hErr = svc_cas_DevDscClose(pCam->stDscInfo[ulDemuxId][j].ulDscId);
								if (hErr != ERR_OK)
									HxLOG_Error("svc_cas_DevDscClose() ERROR!!\n");

								pCam->stDscInfo[ulDemuxId][j].ulDscId = CAS_RESOURCE_NONE;
								pCam->stDscInfo[ulDemuxId][j].usPid = PID_NULL;
							}
							pCam->ulNumOfDsc[ulDemuxId] = 0;
						}
					}
				}
				#endif
			}
		}
	}

	return ERR_OK;
}
#endif

#define _____CasMgr_CB________________________________________________________________________________________________________

static void local_CiMgr_ReqStartSession(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType, HUINT32 unSlotId)
{
	CI_ADD_SVC_PARAM param;
	HERROR nResult = ERR_FAIL;

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("invalid handle\n");
		return;
	}

	param.hAction		= hAction;
	param.unSlotId	= unSlotId;
	param.hSvc		= hSvc; // pvr의 경우 hSvc가 HANDLE_NULL 임.
	param.eActType	= eActType;

	#if defined(CONFIG_DEBUG)
	HxLOG_Info("hAction(0x%08x), hSvc(0x%08x), eActType(%s) unSlotId(%d)\n",
		(HUINT32)hAction, (HUINT32)hSvc, local_CiMgr_MakeStrActionType(eActType), unSlotId);
	#endif

	nResult = local_CiMgr_SendEvent(CI_START, (void *)&param, (unsigned long)sizeof(CI_ADD_SVC_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

	return;
}

static void local_CiMgr_ReqStopSession(Handle_t hAction, Handle_t hNextSvc, HUINT32 unSlotId) // TODO : Stop/Delete를 통합...Manage Instance의 Sequence 를 적용.
{
	CI_DELETE_SVC_PARAM stopSvc;
	HERROR nResult = ERR_FAIL;

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("invalid handle\n");
		return;
	}

	HxLOG_Info("hAction(0x%08x), hNextSvc(0x%08x) unSlotId(%d)\n", (HUINT32)hAction, (HUINT32)hNextSvc, unSlotId);

	stopSvc.hAction	= hAction;
	stopSvc.unSlotId	= unSlotId;
	stopSvc.hNextSvc	= hNextSvc;

	nResult = local_CiMgr_SendEvent(CI_STOP, (void *)&stopSvc, (unsigned long)sizeof(CI_DELETE_SVC_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

	return;
}

static void local_CiMgr_ReqUpdatePmt(Handle_t hAction, HUINT8 *pucPmt, HUINT32 unSlotId)
{
	CI_PMT_CHANGED_PARAM pmtChanged;
	HERROR nResult = ERR_FAIL;

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("invalid handle\n");
		return;
	}

	if (pucPmt == NULL)
	{
		HxLOG_Error("pucPmt is null\n");
		//return;
	}

	pmtChanged.hAction	= hAction;
	pmtChanged.unSlotId	= unSlotId;
	/*
		pmt raw를 사용하는 경우, pmt pointer 전달 받으면 Length 확인해서 mem alloc 후 copy 하도록 한다.

		현재는 사용하지 않음.
	*/
	/*
		ulLen = ( get12bit(pucPmt+1) + 3 );

		pmtChanged.pucTbl = (HUINT8 *)OxCAS_Malloc(ulLen);
		if (pmtChanged.pucTbl == NULL)
		{
			return;
		}
		HxSTD_memcpy(pmtChanged.pucTbl, pucPmt, sizeof(HUINT8)*ulLen);
	*/

	pmtChanged.pucTbl = pucPmt;

	HxLOG_Info("hAction (0x%08x)\n", hAction);

	nResult = local_CiMgr_SendEvent(CI_PMT_CHANGED, (void *)&pmtChanged, (unsigned long)sizeof(CI_PMT_CHANGED_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

	return;
}

static HERROR local_CiMgr_UpdatePmtLocation(Handle_t hAction, HUINT32 unSlotId)
{
	CI_PMT_CHANGED_LOCATION_PARAM		pmtChangedLoc;

	HERROR nResult = ERR_FAIL;

	HxLOG_Info("+++ \n");

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("invalid handle\n");
		return ERR_FAIL;
	}

	pmtChangedLoc.hAction	= hAction;
	pmtChangedLoc.unSlotId = unSlotId;

	pmtChangedLoc.pstPmtInfo = &s_stCasTuneInfo.utInfo.stBroadcastReq.stPmtInfo;

#if defined(_CI_DEBUG_)

	HxLOG_Info("pmtChangedLoc.pstPmtInfo.\n");

	HxLOG_Info("\t usServiceId = 0x%x\n", pmtChangedLoc.pstPmtInfo->usServiceId);
	HxLOG_Info("\t usPcrPid = 0x%x\n", pmtChangedLoc.pstPmtInfo->usPcrPid);
	HxLOG_Info("\t usEleNum = %d\n", pmtChangedLoc.pstPmtInfo->usEleNum);
	HxLOG_Info("\t stEleInfo[0].ucstream_type = 0x%x, usElePid = 0x%x\n", pmtChangedLoc.pstPmtInfo->stEleInfo[0].ucstream_type, pmtChangedLoc.pstPmtInfo->stEleInfo[0].usElePid);
	HxLOG_Info("\t stEleInfo[1].ucstream_type = 0x%x, usElePid = 0x%x\n", pmtChangedLoc.pstPmtInfo->stEleInfo[1].ucstream_type, pmtChangedLoc.pstPmtInfo->stEleInfo[1].usElePid);

	HxLOG_Info("\t stEleInfo[0].stElementCaDescriptor[0].usCaSysId = 0x%x\n", pmtChangedLoc.pstPmtInfo->stEleInfo[0].stElementCaDescriptor[0].usCaSysId);
	HxLOG_Info("\t stEleInfo[0].stElementCaDescriptor[0].usCaPid = 0x%x\n", pmtChangedLoc.pstPmtInfo->stEleInfo[0].stElementCaDescriptor[0].usCaPid);

#ifdef CONFIG_DEBUG
{
	HUINT32 iidx =0;
	HxLOG_Info("received pucRaw data[%d]: \n", pmtChangedLoc.pstPmtInfo->stEleInfo[0].stElementCaDescriptor[0].ucRawLen);
	for ( iidx = 0; iidx < pmtChangedLoc.pstPmtInfo->stEleInfo[0].stElementCaDescriptor[0].ucRawLen; iidx++)
	{
		HxLOG_Print("%02X ", pmtChangedLoc.pstPmtInfo->stEleInfo[0].stElementCaDescriptor[0].pucRaw[iidx]);

		if (iidx % 16 == (16-1))	  {
			HxLOG_Print("\n");
		}
	}
	HxLOG_Print("\n");
}
#endif

	HxLOG_Info("\t stEleInfo[1].stElementCaDescriptor[0].usCaSysId = 0x%x\n", pmtChangedLoc.pstPmtInfo->stEleInfo[1].stElementCaDescriptor[0].usCaSysId);
	HxLOG_Info("\t stEleInfo[1].stElementCaDescriptor[0].usCaPid = 0x%x\n", pmtChangedLoc.pstPmtInfo->stEleInfo[1].stElementCaDescriptor[0].usCaPid);
#ifdef CONFIG_DEBUG
	{
		HUINT32 iidx =0;
		HxLOG_Info("received pucRaw data[%d]: \n", pmtChangedLoc.pstPmtInfo->stEleInfo[1].stElementCaDescriptor[0].ucRawLen);
		for ( iidx = 0; iidx < pmtChangedLoc.pstPmtInfo->stEleInfo[1].stElementCaDescriptor[0].ucRawLen; iidx++)
		{
			HxLOG_Print("%02X ", pmtChangedLoc.pstPmtInfo->stEleInfo[1].stElementCaDescriptor[0].pucRaw[iidx]);

			if (iidx % 16 == (16-1))	  {
				HxLOG_Print("\n");
			}
		}
		HxLOG_Print("\n");
	}
#endif
#endif

	nResult = local_CiMgr_SendEvent(CI_PMT_CHANGED_LOCATION, (void *)&pmtChangedLoc, (unsigned long)sizeof(CI_PMT_CHANGED_LOCATION_PARAM));

	if(ERR_OK!=nResult)
		HxLOG_Critical("Critical Error!!!\n");

	return ERR_OK;
}

static void local_CiMgr_ReqManageInstance(	Handle_t	hAction,				/* Action id */
													HUINT32	ulCaInstanceId,		/* Instatnce id를 전달하는 의도는 CI Slot 단위로 Instance Id가 다르기 */
													HBOOL	bConnect,
													HUINT32	unSlotId)			/* TRUE : Descramble 시도 요청, FALSE : Descramble 중지 요청 */
{
	CI_MANAGE_CA_INST_PARAM param;
	HERROR nResult = ERR_FAIL;

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("invalid handle\n");
		return;
	}

	HxLOG_Info("hAction(0x%08x), CaInstanceId(0x%x), connect(%d)\n", (HUINT32)hAction, (HUINT32)ulCaInstanceId, bConnect);

	param.hAction		= hAction;
	param.unSlotId	= unSlotId;
	param.hCasDevice	= ulCaInstanceId;
	param.bConnect	= bConnect;

	nResult = local_CiMgr_SendEvent(CI_MANAGE_CA_INST, (void *)&param, (unsigned long)sizeof(CI_MANAGE_CA_INST_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

	return;
}

#if defined(CONFIG_MW_CI_PLUS)

// SDT Raw를 직접 받아라.
static void local_CiMgr_ReqUpdateSdt(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pSdtRaw, HUINT32 unSlotId)
{
	CI_SDT_CHANGED_PARAM param;
	HERROR nResult = ERR_FAIL;
	HUINT32 ulLen = 0;

	if(hAction == HANDLE_NULL || pSdtRaw == NULL)
	{
		HxLOG_Error("invalid handle hAction(0x%x), pSdtRaw(0x%x)\n", hAction, pSdtRaw );
		return;
	}

	if(usUniqueId == 0)
	{
		HxLOG_Error("invalid UniqueId\n");
		return;
	}

	HxLOG_Info("ActionHandle(0x%08x), UniqueId(%x)\n", hAction, usUniqueId);

	param.hAction		= hAction;
	param.unSlotId	= unSlotId;
	param.usUniqueId	= usUniqueId;

	ulLen = get12bit(pSdtRaw+1) + 3;
	param.pucTbl = (HUINT8 *)OxCAS_Malloc(ulLen);
	if (param.pucTbl == NULL )
	{
		HxLOG_Error("Action:0x%x, can not allocate memory. \n", hAction);
		return;
	}

	HxSTD_memcpy(param.pucTbl, pSdtRaw, ulLen);

	nResult = local_CiMgr_SendEvent(CI_SDT_CHANGED, (void *)&param, (unsigned long)sizeof(CI_SDT_CHANGED_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

	return;
}
#endif

static void local_CiMgr_StartSessionSlot0(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType)
{
	local_CiMgr_ReqStartSession(hAction, hSvc, eActType, 0);
}

static void local_CiMgr_StartSessionSlot1(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType)
{
	local_CiMgr_ReqStartSession(hAction, hSvc, eActType, 1);
}

static void local_CiMgr_StopSessionSlot0(Handle_t hAction, Handle_t hNextSvc)
{
	local_CiMgr_ReqStopSession(hAction, hNextSvc, 0);
}

static void local_CiMgr_StopSessionSlot1(Handle_t hAction, Handle_t hNextSvc)
{
	local_CiMgr_ReqStopSession(hAction, hNextSvc, 1);
}

static void local_CiMgr_UpdatePmtSlot0(Handle_t hAction, HUINT8 *pucPmt)
{
	local_CiMgr_ReqUpdatePmt(hAction, pucPmt, 0);
}

static void local_CiMgr_UpdatePmtSlot1(Handle_t hAction, HUINT8 *pucPmt)
{
	local_CiMgr_ReqUpdatePmt(hAction, pucPmt, 1);
}

static void local_CiMgr_ManageInstanceSlot0(Handle_t hAction, HUINT32 ulCaInstanceId, HBOOL bConnect)
{
	local_CiMgr_ReqManageInstance(hAction, ulCaInstanceId, bConnect, 0);
}

static void local_CiMgr_ManageInstanceSlot1(Handle_t hAction, HUINT32 ulCaInstanceId, HBOOL bConnect)
{
	local_CiMgr_ReqManageInstance(hAction, ulCaInstanceId, bConnect, 1);
}

#if defined(CONFIG_MW_CI_PLUS)
static void local_CiMgr_UpdateSdtSlot0(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pSdtRaw)
{
	local_CiMgr_ReqUpdateSdt(hAction, usUniqueId, pSdtRaw, 0);
}

static void local_CiMgr_UpdateSdtSlot1(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pSdtRaw)
{
	local_CiMgr_ReqUpdateSdt(hAction, usUniqueId, pSdtRaw, 1);
}
#endif

static HERROR local_CiMgr_GetCasMgrCbFuncList(HUINT32 unSlotId, SvcCas_CbFuncList_t *pstCbList)
{
	VK_memset32(pstCbList, 0x00, sizeof(SvcCas_CbFuncList_t));

	switch(unSlotId)
	{
	case 0:
		/* register slot 0 callback */
		pstCbList->pfnCbStartSession	= local_CiMgr_StartSessionSlot0;
		pstCbList->pfnCbStopSession	= local_CiMgr_StopSessionSlot0;
		pstCbList->pfnCbUpdatePmt	= local_CiMgr_UpdatePmtSlot0;
		pstCbList->pfnCbInstSelected	= local_CiMgr_ManageInstanceSlot0;
		#if defined(CONFIG_MW_CI_PLUS)
		pstCbList->pfnCbUpdateSdt	= local_CiMgr_UpdateSdtSlot0;
		#endif
		break;
	case 1:
		/* register slot 1 callback */
		pstCbList->pfnCbStartSession	= local_CiMgr_StartSessionSlot1;
		pstCbList->pfnCbStopSession 	= local_CiMgr_StopSessionSlot1;
		pstCbList->pfnCbUpdatePmt	= local_CiMgr_UpdatePmtSlot1;
		pstCbList->pfnCbInstSelected	= local_CiMgr_ManageInstanceSlot1;
		#if defined(CONFIG_MW_CI_PLUS)
		pstCbList->pfnCbUpdateSdt	= local_CiMgr_UpdateSdtSlot1;
		#endif
		break;

	default:	return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_CiMgr_RegisterCasMgrCb(void)
{
	HUINT32				i;
	HERROR				hErr;
	Handle_t				hCasDevice = HANDLE_NULL;
	SvcCas_CbFuncList_t	cbList;

	for(i = 0; i < CAS_MAX_NUM_OF_CAM; i++)
	{
/* 요거 안해되 될것 같다.. 추후에 CB함수 걍 바꾸도록 하자~ ㅋ*/
		hErr = local_CiMgr_GetCasMgrCbFuncList(i, &cbList);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("local_CiMgr_GetCasMgrCbFuncList(%d) failed\n", i);
			return ERR_FAIL;
		}

		hErr = svc_cas_MgrSubRegisterCb(eCasInst_CI, eDxCAS_GROUPID_CAM, i, &cbList, 1, &hCasDevice);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_InstanceResisterCb() slotid(%d)failed\n", i);
			return ERR_FAIL;
		}

		s_astCiCam[i].hCasDevice = hCasDevice; // Instance Table Index

		HxLOG_Info("Slot Id (%d), Cam Instance (%d) + Cas Instance (%d)\n", i, i, hCasDevice);
	}

	return ERR_OK;
}

#define _____CI_CB________________________________________________________________________________________________________

static HINT32 local_CiMgr_ReqUpdateCamState(CI_SLOT_ID usSlotId, CI_CAM_STATE eCamState)
{
	CI_UPDATE_CAM_STATE_PARAM updateCamSt;
	HERROR nResult = ERR_FAIL;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	updateCamSt.usSlotId = usSlotId;
	updateCamSt.eCamState = eCamState;

	nResult = local_CiMgr_SendEvent(CI_UPDATE_CAM_STATE, (void *)&updateCamSt, (unsigned long)sizeof(CI_UPDATE_CAM_STATE_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

	return ERR_OK;
}

static HINT32 local_CiMgr_ReqUpdateCaInst(CI_SLOT_ID usSlotId, unsigned short usCaInfoCnt, unsigned short *ausCaSysId)
{
	CI_UPDATE_CA_INST_PARAM updateCaInst;
	HERROR nResult = ERR_FAIL;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	updateCaInst.usSlotId = usSlotId;

	if (usCaInfoCnt == 0) // CA instance 가 release 된 상황 (즉, CAS 세션 종료로 CiCas_DeleteCaInst() 수행된 것)
	{
		updateCaInst.ausCaSysId = NULL;
		updateCaInst.usCaInfoCnt = 0;
	}
	else
	{
		if (ausCaSysId == NULL) // CA instance 가 release 된 상황은 위에서 거르기 때문에 여기서 이런 에러가 발생하면 곤란...
		{
			HxLOG_Error("\n");
			return ERR_FAIL;
		}

		updateCaInst.ausCaSysId = (HUINT16 *)OxCAS_Malloc(sizeof(HUINT16) * usCaInfoCnt); // local_cimgr_ProcUpdateCaInst()에서 free.
		if (updateCaInst.ausCaSysId == NULL)
		{
			HxLOG_Error("mem alloc failure\n");
			return ERR_FAIL;
		}
		HxSTD_memcpy(updateCaInst.ausCaSysId, ausCaSysId, sizeof(HUINT16) * usCaInfoCnt);

		updateCaInst.usCaInfoCnt = usCaInfoCnt;
	}

	nResult = local_CiMgr_SendEvent(CI_UPDATE_CA_INST, (void *)&updateCaInst, (unsigned long)sizeof(CI_UPDATE_CA_INST_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;
}

#if defined(CI_PLUS_PVR_ENABLE)
static HINT32 local_CiMgr_ReqUpdateCaPvrInst(CI_SLOT_ID usSlotId, unsigned short usCaPvrInfoCnt, unsigned short *ausCaSysId, CI_CA_PVR_REC_MODE *aucCaRecMode)
{
	CI_UPDATE_CA_PVR_INST_PARAM updateCaPvrInst;
	HERROR nResult = ERR_FAIL;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	updateCaPvrInst.usSlotId = usSlotId;

	if (usCaPvrInfoCnt == 0) // CA PVR instance 가 release 된 상황 (즉, CA PVR 세션 종료로 CiCaPvr_DeleteCaPvrInst() 수행된 것)
	{
		updateCaPvrInst.ausCaPvrCaSysId = NULL;
		updateCaPvrInst.aucCaRecMode = NULL;
		updateCaPvrInst.usCaPvrInfoCnt = 0;
	}
	else
	{
		if (ausCaSysId == NULL || aucCaRecMode == NULL) // CA PVR instance 가 release 된 상황은 위에서 거르기 때문에 여기서 이런 에러가 발생하면 곤란...
		{
			HxLOG_Error("\n");
			return ERR_FAIL;
		}

		updateCaPvrInst.ausCaPvrCaSysId = (HUINT16 *)OxCAS_Malloc(sizeof(HUINT16) * usCaPvrInfoCnt); // local_cimgr_ProcUpdateCaPvrInst()에서 free.
		if (updateCaPvrInst.ausCaPvrCaSysId == NULL)
		{
			HxLOG_Error("mem alloc failure for ausCaPvrCaSysId\n");
			return ERR_FAIL;
		}
		HxSTD_memcpy(updateCaPvrInst.ausCaPvrCaSysId, ausCaSysId, sizeof(HUINT16) * usCaPvrInfoCnt);

		updateCaPvrInst.aucCaRecMode = (CI_CA_PVR_REC_MODE *)OxCAS_Malloc(sizeof(CI_CA_PVR_REC_MODE) * usCaPvrInfoCnt); // local_cimgr_ProcUpdateCaPvrInst()에서 free.
		if (updateCaPvrInst.aucCaRecMode == NULL)
		{
			HxLOG_Error("mem alloc failure for aucCaRecMode\n");
			return ERR_FAIL;
		}
		HxSTD_memcpy(updateCaPvrInst.aucCaRecMode, aucCaRecMode, sizeof(CI_CA_PVR_REC_MODE) * usCaPvrInfoCnt);

		updateCaPvrInst.usCaPvrInfoCnt = usCaPvrInfoCnt;
	}

	nResult = local_CiMgr_SendEvent(CI_UPDATE_CA_PVR_INST, (void *)&updateCaPvrInst, (unsigned long)sizeof(CI_UPDATE_CA_PVR_INST_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

	return ERR_OK;
}
#endif

#if 1 // #if defined(CONFIG_MW_CI_PLUS_PVR) : 당장은 CA PVR 지원을 위해 추가한 것이지만 Multiple CA application selection procedure 구현 같은 다른 용도로 확장해서 쓸 여지가 있으므로 일반적으로 정의해 둠.
static HINT32 local_CiMgr_ReqUpdateCasState(CI_SLOT_ID usSlotId, CI_CAS_STATE eCasState)
{
	CI_UPDATE_CAS_STATE_PARAM updateCasSt;
	HERROR nResult = ERR_FAIL;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	updateCasSt.usSlotId = usSlotId;
	updateCasSt.eCasState = eCasState;

	nResult = local_CiMgr_SendEvent(CI_UPDATE_CAS_STATE, (void *)&updateCasSt, (unsigned long)sizeof(CI_UPDATE_CAS_STATE_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

	return ERR_OK;
}
#endif

static HINT32 local_CiMgr_UpdateMmiRequest(Handle_t hMmi, MMI_REQ nMmiReq, void *pvParam, HULONG ulParamSize)
{
	CI_SLOT_ID		usSlotId;
	HUINT16			usSsNo;
	int				errRet;
	HERROR			nResult = ERR_FAIL;

	HxLOG_Info(" \n");

	/* CAM Slot ID Check */
	errRet = CI_MmiGetSlotId(hMmi, &usSlotId, &usSsNo);
	if (errRet != ERR_OK ) {
		HxLOG_Error(" : hMmi = 0x%x has wrong slot id !!\n", hMmi);
		return ERR_FAIL;
	}

	if(usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error(" : hMmi = 0x%x has wrong slot id (%d) !!\n", hMmi, usSlotId);
		return ERR_FAIL;
	}

	ENTER_CRITICAL_MMI;

	// increase mmi data verison
	++s_ulMmiVersion;

	// Mapping Data
	nResult = local_CiMgr_UpdateMmiData((HUINT32)usSlotId, s_ulMmiVersion, hMmi, nMmiReq, pvParam, ulParamSize);
	if(nResult != ERR_OK)			HxLOG_Critical("Critical Error!!!\n");

	// Send To Msg
	switch(nMmiReq)
	{
	case MMI_OPEN:
		svc_cas_MgrSubUpdateMmi(eCasMmiType_Open, usSlotId, s_ulMmiVersion);
		break;
	case MMI_CLOSE:
		svc_cas_MgrSubUpdateMmi(eCasMmiType_Close, usSlotId, s_ulMmiVersion);
		break;
	case MMI_ENQ:
		svc_cas_MgrSubUpdateMmi(eCasMmiType_Enq, usSlotId, s_ulMmiVersion);
		break;
	case MMI_MENU:
		svc_cas_MgrSubUpdateMmi(eCasMmiType_Menu, usSlotId, s_ulMmiVersion);
		break;
	case MMI_LIST:
		svc_cas_MgrSubUpdateMmi(eCasMmiType_List, usSlotId, s_ulMmiVersion);
		break;
	case MMI_TEXT:
		svc_cas_MgrSubUpdateMmi(eCasMmiType_Text, usSlotId, s_ulMmiVersion);
		break;
	default :
		HxLOG_Critical("\n");
		break;
	}

	LEAVE_CRITICAL_MMI;

#if defined(CONFIG_MW_CI_CHECK_AV)
{
	// MR 채널로 인하여 instance 관리가 꼬인다. 협의된 내용이 MMI가 출력되어 무조건 instance 관리를 하지 않도록 한다.
	// -> MMI가 닫히면 AV를 check 하도록 한다.
	HINT32 i;

	switch(nMmiReq)
	{
		case MMI_TEXT:
		case MMI_ENQ:
		case MMI_MENU:
		case MMI_LIST:
			for(i = 0 ; i < CI_MAX_NUM_OF_ACT ; i++)
			{
				if(s_astCiContext[i].pCam == NULL || s_astCiContext[i].pCam->usSlotId != usSlotId)
					continue;

				local_CiMgr_StopCheckAv(&s_astCiContext[i]);
			}
			break;

		case MMI_CLOSE:
			for(i = 0 ; i < CI_MAX_NUM_OF_ACT ; i++)
			{
				if(s_astCiContext[i].pCam == NULL || s_astCiContext[i].pCam->usSlotId != usSlotId)
					continue;

				local_CiMgr_StartCheckAv(&s_astCiContext[i]);
			}
			break;

		default:
			break;
	}
}
#endif
	return ERR_OK;
}

static HERROR local_CiMgr_RegisterCiCb(void)
{
	CI_NOTIFY_FUNC_LIST notifyList;

/* 콜백 함수 등록 (CI stack -> CI mgr) */

	HxSTD_memset(&notifyList, 0, sizeof(CI_NOTIFY_FUNC_LIST));
	notifyList.pfnNotifyCamState	= (CI_NOTIFY_CAM_STATE_FUNC)local_CiMgr_ReqUpdateCamState; // 이건 CI mgr 로 notify 하기 위한 용도.
	notifyList.pfnNotifyCaInfo	= (CI_NOTIFY_CA_INFO_FUNC)local_CiMgr_ReqUpdateCaInst;
#if defined(CI_PLUS_PVR_ENABLE)
	notifyList.pfnNotifyCaPvrInfo	= (CI_NOTIFY_CA_PVR_INFO_FUNC)local_CiMgr_ReqUpdateCaPvrInst;
#endif

#if 1 // #if defined(CONFIG_MW_CI_PLUS_PVR) : 당장은 CA PVR 지원을 위해 추가한 것이지만 Multiple CA application selection procedure 구현 같은 다른 용도로 확장해서 쓸 여지가 있으므로 일반적으로 정의해 둠.
	notifyList.pfnNotifyCasState	= (CI_NOTIFY_CAS_STATE_FUNC)local_CiMgr_ReqUpdateCasState;
#endif

	CI_AddNotifyFunc(notifyList);

/* 콜백 함수 등록 (CI stack -> CAS mgr) */

	HxSTD_memset(&notifyList, 0, sizeof(CI_NOTIFY_FUNC_LIST));
	notifyList.pfnNotifyCamState	= (CI_NOTIFY_CAM_STATE_FUNC)svc_cas_MgrSubUpdateCamState;	// 이건 Cas Mgr 로 notify 하기 위한 용도.
	notifyList.pfnNotifyAppInfo	= (CI_NOTIFY_APP_INFO_FUNC)svc_cas_MgrSubUpdateCamAppinfo;
	notifyList.pfnNotifyMmiReq	= (CI_NOTIFY_MMI_REQ_FUNC)local_CiMgr_UpdateMmiRequest;	// CAS Mgr로 notify하긴 하지만 local함수를 사용한다. ㅡㅡ;;

	CI_AddNotifyFunc(notifyList);

	return ERR_OK;
}

#define _____CI_PLUS_CB________________________________________________________________________________________________________

#if defined(CONFIG_MW_CI_PLUS)

// 이 콜백 함수에서 직접 디스크램블러에 SetKey 를 하게 되면 racing condition 및 캠에 confirm 을 주는 것이 늦어져 문제가 발생할 수 있으므로 CI mgr 타스크에서 처리하도록 메시지 전송해야 한다!
static HINT32 local_CiMgr_ReqSetKey(CI_SLOT_ID usSlotId, CIPLUS_KeyInfo_t *pstKeyInfo)
{
	CI_PLUS_KEY_CHANGE_PARAM param;
	HERROR nResult = ERR_FAIL;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	if (pstKeyInfo == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	param.usSlotId = usSlotId;

	switch (pstKeyInfo->enKeyCipher)
	{
		case CI_CC_KEY_CIPHER_DES_56_ECB :
			param.eDscType = eCAS_DSC_TYPE_DES_CLEAR;
			HxSTD_memcpy(param.ucCck, pstKeyInfo->pucCck, 8);
			break;

		case CI_CC_KEY_CIPHER_AES_128_CBC :
			param.eDscType = eCAS_DSC_TYPE_AES_CLEAR;
			HxSTD_memcpy(param.ucCck, pstKeyInfo->pucCck, 16);
			HxSTD_memcpy(param.ucCiv, pstKeyInfo->pucCiv, 16);
			break;

		default :
			HxLOG_Error("Invalid enKeyCipher (%d)\n", pstKeyInfo->enKeyCipher);
			return ERR_FAIL;
	}

	switch (pstKeyInfo->enKeyMode)
	{
	case CI_CC_KEY_MODE_EVEN :
		param.eParity = eCAS_DSC_PARITY_EVEN;
		break;

	case CI_CC_KEY_MODE_ODD :
		param.eParity = eCAS_DSC_PARITY_ODD;
		break;

	default :
		HxLOG_Error("Invalid enKeyMode (%d)\n", pstKeyInfo->enKeyMode);
		return ERR_FAIL;
	}

	HxLOG_Info("local_CiMgr_ReqSetKey CI_PLUS_KEY_CHANGED send \n");

	nResult = local_CiMgr_SendEvent(CI_PLUS_KEY_CHANGED, (void *)&param, (unsigned long)sizeof(CI_PLUS_KEY_CHANGE_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

	return ERR_OK;
}

// 이 콜백 함수에서 직접 URI control 을 하게 되면 캠에 confirm 을 주는 것이 늦어져 문제가 발생할 수 있으므로 CI mgr 타스크에서 처리하도록 메시지 전송해야 한다!
static HINT32 local_CiMgr_ReqUriControl(CI_SLOT_ID usSlotId, CIPLUS_UriInfo_t *pstUriInfo)
{
	CI_PLUS_URI_CONTROL_PARAM param;
	HUINT32 nResult = 0;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	if (pstUriInfo == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	param.usSlotId = usSlotId;
	HxSTD_memcpy(&param.stUriInfo, pstUriInfo, sizeof(CIPLUS_UriInfo_t));

	nResult = local_CiMgr_SendEvent(CI_PLUS_URI_CONTROL, (void *)&param, (unsigned long)sizeof(CI_PLUS_URI_CONTROL_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

	return ERR_OK;
}

// 이 콜백 함수에서 직접 SRM control 을 하게 되면 캠에 confirm 을 주는 것이 늦어져 문제가 발생할 수 있으므로 CI mgr 타스크에서 처리하도록 메시지 전송해야 한다!
static HINT32 local_CiMgr_ReqSrmControl(CI_SLOT_ID usSlotId, unsigned char *pucSrmData, unsigned short usSrmDataLen)
{
	CI_PLUS_SRM_CONTROL_PARAM param;
	HERROR nResult = ERR_FAIL;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	if (pucSrmData == NULL || usSrmDataLen == 0)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	param.usSlotId = usSlotId;
	param.pucSrmData = (unsigned char *)OxCAS_Malloc(usSrmDataLen); // local_cimgr_ProcSrmControl()에서 free.
	HxSTD_memcpy(param.pucSrmData, pucSrmData, usSrmDataLen);

	nResult = local_CiMgr_SendEvent(CI_PLUS_SRM_CONTROL, (void *)&param, (unsigned long)sizeof(CI_PLUS_SRM_CONTROL_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

	return ERR_OK;
}

static HINT32 local_CiMgr_ReqHostTune(CI_SLOT_ID usSlotId, SvcCas_CiPlusTuneInfo_t *pstTuneInfo)
{
	CI_PLUS_CAM_TUNE_PARAM param;
	HERROR nResult = ERR_FAIL;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	if (pstTuneInfo == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	param.usSlotId = usSlotId;
	HxSTD_memcpy(&param.stTuneInfo, pstTuneInfo, sizeof(SvcCas_CiPlusTuneInfo_t));

	nResult = local_CiMgr_SendEvent(CI_PLUS_CAM_TUNE, (void *)&param, (unsigned long)sizeof(CI_PLUS_CAM_TUNE_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

	return ERR_OK;
}

static HINT32 local_CiMgr_ReqPidReplace(CI_SLOT_ID usSlotId, CIPLUS_PidInfo_t *pstPidInfo) // <TODO_CI_PLUS> : 동작 검증되지 않았음. 추후 확인 필요 !!
{
	CI_Context_t *pCiContext = NULL;
	HUINT32 i = 0;
#if 0
	HBOOL bFound = FALSE;
	HERROR nResult = ERR_FAIL;
#endif

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	if (pstPidInfo == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	HxLOG_Info("mode (%d), OrgPid (%x), TarPid (%x)\n", pstPidInfo->eReplaceMode, pstPidInfo->usOriginalPid, pstPidInfo->usTargetPId);

	for(i=0; i<CI_MAX_NUM_OF_ACT; i++)
	{
		if(s_astCiContext[i].pCam != NULL)
		{
			if(s_astCiContext[i].pCam->usSlotId == usSlotId)
			{
				pCiContext = &s_astCiContext[i];
				break;
			}
		}
	}

	if(pCiContext == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	for(i=0; i<pCiContext->stProgInfo.ucNumEs; i++)
	{
		if(eCiPlus_Replace_Start == pstPidInfo->eReplaceMode)
		{
			if(pstPidInfo->usOriginalPid == pCiContext->stProgInfo.aEsInfo[i].usEsPid)
			{
				HxLOG_Info("Original Pid(%x) Found\n", pstPidInfo->usOriginalPid);
//				bFound = TRUE;
				pCiContext->stProgInfo.aEsInfo[i].usEsPid = pstPidInfo->usTargetPId;
			}
		}
		else if(eCiPlus_Replace_Stop == pstPidInfo->eReplaceMode || eCiPlus_Replace_Cancel == pstPidInfo->eReplaceMode)
		{
			if(pstPidInfo->usTargetPId == pCiContext->stProgInfo.aEsInfo[i].usEsPid)
			{
				HxLOG_Info("usTargetPId(%x) Found\n", pstPidInfo->usTargetPId);
//				bFound = TRUE;
				pCiContext->stProgInfo.aEsInfo[i].usEsPid = pstPidInfo->usOriginalPid;
			}
		}
		else
		{
			HxLOG_Error("Unknown Pid Replacement mode (%d)\n", pstPidInfo->eReplaceMode);
		}
	}

#if 0
	if(bFound == TRUE)
	{
		nResult = local_cimgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 1, 0);
		if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");
	}
#endif
	return ERR_OK;
}

static int local_CiMgr_ReqUpdateCcResource(CI_SLOT_ID usSlotId, CIPLUS_CcReportInfo_t *pstCcReportInfo)
{
	HERROR nResult = ERR_FAIL;
	CI_UPDATE_CA_INST_PARAM	updateCcInst;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d) \n", usSlotId);
		return ERR_FAIL;
	}

	if (pstCcReportInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}
#if 0
	s_astCiCam[usSlotId].eCcReport = pstCcReportInfo->eCcReport;
#else
	switch(pstCcReportInfo->eCcReport)
	{
		case eCiPlus_CcReport_CanNotLoadHostLicenseConstants:
		case eCiPlus_CcReport_HostLicenseConstantsNotSaved:
		case eCiPlus_CcReport_UriVersionNegotiationDone:
		case eCiPlus_CcReport_AuthenticationFailed:
		case eCiPlus_CcReport_OperationFailed:
		case eCiPlus_CcReport_NoResponseFromCam:
			s_astCiCam[usSlotId].bAuthFinish = TRUE;
			break;

		default:
			break;
	}
#endif

	if (pstCcReportInfo->eCcReport != eCiPlus_CcReport_AuthenticationDoneAndStartSacEstablishment
		&& pstCcReportInfo->eCcReport != eCiPlus_CcReport_UriVersionNegotiationDone)
	{
		// 이들은 UI 처리를 위한 용도로 여기서는 처리할 것이 없다.
		return ERR_OK;
	}

	HxLOG_Info("eCcReport (%d)	\n", pstCcReportInfo->eCcReport);

	updateCcInst.usSlotId = usSlotId;

	nResult = local_CiMgr_SendEvent(CI_UPDATE_CC, (void *)&updateCcInst, (unsigned long)sizeof(CI_UPDATE_CA_INST_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

	return ERR_OK;
}

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
static int local_CiMgr_PostAvBlankEvent(Handle_t hAction, CIPLUS_CcPinStatusInfo_t *pstCcPinStatusInfo)
{
	HERROR 			nResult = ERR_FAIL;
	HxLOG_Print("ePinStatus (%d)	\n", pstCcPinStatusInfo->ePinStatus);

	switch (pstCcPinStatusInfo->ePinStatus)
	{
		case CI_PIN_STATUS_PIN_CORRECT :
		case CI_PIN_STATUS_VIDEO_BLAKING_NOT_REQUIRED :
			{
				/* TV manager(ap_recorder) 쪽으로 녹화 정보 전달 */
				nResult = svc_cas_MgrSubUpdateCasEtcEvent(hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM, eSEVT_CAS_DRM_AV_BLANK, eDxCAS_GROUPID_CAM, FALSE, pstCcPinStatusInfo->unMetaIndex);
				if(nResult != ERR_OK)
				{
					HxLOG_Error("CasMgrSub_UpdateCasEtcEvent() failed.\n");
					return nResult;
				}

			}
			break;
		case CI_PIN_STATUS_BAD_PIN :
		case CI_PIN_STATUS_CICAM_BUSY :
		case CI_PIN_STATUS_PIN_UNCONFIRMED :
		case CI_PIN_STATUS_CONTENT_CA_SCRAMBLED :
			// TODO: AV Blanking
			break;
		default:
			break;
	}
	return ERR_OK;
}

// TODO: 아래 함수에서 상태 업데이트를 하는데, WatchTV 및 Playback 시나리오에서 AV blanking 등 컨트롤 하는 부분 추가 구현 필요
// TODO: CAS Manager ver2에 맞도록 코드 수정 필요. CI+ UI 작업할때 수행하도록 (kkkim)
static int local_CiMgr_ReqUpdatePinStatus(CI_SLOT_ID usSlotId, CIPLUS_CcPinStatusInfo_t *pstCcPinStatusInfo)
{
	Handle_t 		hAction = HANDLE_NULL;
	HERROR 			nResult = ERR_FAIL;

	if (pstCcPinStatusInfo == NULL)
	{
		HxLOG_Assert(0);
		return ERR_FAIL;
	}

	if (usSlotId >= PAL_CI_GetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d) \n", usSlotId);
		return ERR_FAIL;
	}

	// hAction이 HANDLE_NULL인 경우 live를 위한 event를 올리는 것이지만, 일단 live를 포함하여 현재 CAM을 타는 모든 action을 찾아서 전송
	// side가 있다면 이 부분 수정 필요.
	if (pstCcPinStatusInfo->hAction == HANDLE_NULL)
	{
		CI_CamInst_t		*pCam = (CI_CamInst_t *)NULL;
		Handle_t			hAction = HANDLE_NULL;
		HBOOL				bFound = 0;
		HUINT32				i, j;

		pCam = &s_astCiCam[usSlotId];

		for (i=0; i<CI_MAX_NUM_OF_ACT; i++)
		{
			bFound = FALSE;
			if (s_astCiContext[i].pCam == pCam)
			{
				hAction = s_astCiContext[i].hAction;
				if(hAction == HANDLE_NULL)
				{
					continue;
				}

				for (j=0; j<CAS_MAX_NUM_OF_ACTION; j++)
				{
					if (pCam->hActList[j] == hAction)
					{
						HxLOG_Print("action handle (0x%x) hAction (0x%x)\n", pCam->hActList[j], hAction);
						bFound = TRUE;
						break;
					}
				}

				if (bFound == TRUE)
				{
					nResult= local_CiMgr_PostAvBlankEvent(hAction, pstCcPinStatusInfo);
					if(nResult != ERR_OK)
					{
						HxLOG_Error("local_CiMgr_PostAvBlankEvent() failed.\n");
						return ERR_FAIL;
					}
				}
				else
				{
					HxLOG_Error("ERROR: check...\n");
				}
			}
		}
	}
	else
	{
		// playback 에 대하여 A/V 동작에 관여 함
		hAction = pstCcPinStatusInfo->hAction;
		nResult= local_CiMgr_PostAvBlankEvent(hAction, pstCcPinStatusInfo);
		if(nResult != ERR_OK)
		{
			HxLOG_Error("local_CiMgr_PostAvBlankEvent() failed.\n");
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

// TODO: 각 동작마다 시나리오에 맞도록 EVT 만든후 전송!(kkkim)
static int local_CiMgr_ReqUpdateCiPlusConfirmOperation(CI_SLOT_ID usSlotId, CIPLUS_CcConfirmDataInfo_t *pstCcConfirmDataInfo)
{
	HERROR hErr;

	switch (pstCcConfirmDataInfo->eCcProtocolType)
	{
		case eCiCcProtocolType_None:
			HxLOG_Info("Ignore eCiCcProtocolType_None. If this Log is shown, check why it comes to here.\n");
			break;
		case eCiCcProtocolType_PlaybackLicenseExchange:
			{
				// TODO: EVT가 Playback 쪽으로 전송 되어야 함. 세부 내용은 아래 주석 참고. / Metafile 처리 부분 구현 완료 후 추가할것! (kkkim)
				// Playback 쪽에서는 Program number 비교해보고, 같은 program number를 재생 하고 있는 action에서 처리
				// Playback License Exchange 동작을 요청한 부분의 timestamp에 담겨 있는 내용을
				// 이 EVT를 통하여 전송 받은 URI 및 License Data로 교체 하여 metafile에 저장 하도록 한다.

				/* playback 시 pCiContext->usSvcId = 0 이 되어 문제 발생하여 program num을 비교하는 부분을 막음.
					실제 update 하는 부분에서 program num 을 비교하여 동일한지 확인하고 저장하도록 되어 있음 */
				// CI_Context_t		*pCiContext = NULL;

				// 아래 Action으로 Ci Context의 Action과 비교하는 과정에서 Playback에 대한 Action은 CI context에서 가지고있지 않을 수 있다.
				// 이러한 경우 아래 코드에서 문제가 나올 수 있으므로 확인해보고,
				// 문제가 있을 경우에는 program number를 위로 올려서 playback쪽에서 비교하여 처리하거나 program number 비교 자체를 안하는 방향으로... (kkkim)
				/*
				pCiContext = local_CiMgr_GetContextByAction(pstCcConfirmDataInfo->hAction);
				if (pCiContext == NULL)
				{
					HxLOG_Error("Sequence Error.\n");	// 반드시 잡아야 함
					return ERR_FAIL;
				}

				if (pCiContext->usSvcId != pstCcConfirmDataInfo->data.stPbLicenseInfo.usProgramNumber)
				{
					HxLOG_Error("Program number is different!! Sequence Error!! \n");
					return ERR_FAIL;
				}
				*/

				if (pstCcConfirmDataInfo->data.stPbLicenseInfo.eLicenseStatus == CI_CONTENT_LICENSE_STATUS_OK)
				{
					DxCopyrightControl_CiPlusUriInfo_t		stCiPlusUriInfo;
					DxCopyrightControl_CiPlusLicenseInfo_t	stCiPlusLicenseInfo;
					DxCopyrightControl_t			*pstDrmInfo = NULL;

					stCiPlusUriInfo.ucMode = CI_CC_URI_MODE_FROMCAM;
					stCiPlusUriInfo.ucAps = pstCcConfirmDataInfo->data.stPbLicenseInfo.ucAps;
					stCiPlusUriInfo.ucEmi = pstCcConfirmDataInfo->data.stPbLicenseInfo.ucEmi;
					stCiPlusUriInfo.ucIct = pstCcConfirmDataInfo->data.stPbLicenseInfo.ucIct;
					stCiPlusUriInfo.ucRct = pstCcConfirmDataInfo->data.stPbLicenseInfo.ucRct;
					stCiPlusUriInfo.ucDot = pstCcConfirmDataInfo->data.stPbLicenseInfo.ucDot;
					stCiPlusUriInfo.ulMinutesForRL = pstCcConfirmDataInfo->data.stPbLicenseInfo.ulMinutesForRL;

					/* URI 정보를 DRM data로 만든 후 DRM Signalled event로 TV manager로 전송 (ap_player.c 파일 EVT_CAS_DRM_SIGNALLED에서 처리 될 것임) */
					hErr = local_CiMgr_MakeCiDrmInfo(eDxCopyrightControl_CiPlusDrmType_Uri, (void *)&stCiPlusUriInfo, &pstDrmInfo, eDxCopyrightControl_CiPlusDrmWriteInfo_Writing);
					if(hErr != ERR_OK)
					{
						HxLOG_Error("local_CiMgr_MakeCiDrmInfo() failed.\n");
						return ERR_FAIL;
					}

					// 여기서 unMetaIndex 는 license data에 대한 timestamp의 index 이며 보통은 unMetaIndex -1 이 URI 라고 판단됨.
					// 하지만 ap_player에서 이 index에서 부터 1씩 빼면서 license data의 ulTime 값과 동일한 timestamp를 찾고 URI type인지 확인해서
					// license 와 함께 전송된 uri 라고 판단하고 필요한 사항 update 하도록 함.
					hErr = svc_cas_MgrSubUpdateCasEtcEventData(pstCcConfirmDataInfo->hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM, eSEVT_CAS_DRM_SIGNALLED,
								0, pstCcConfirmDataInfo->data.stPbLicenseInfo.unMetaIndex, (void*)pstDrmInfo, sizeof(DxCopyrightControl_t));
					if(hErr != ERR_OK)
					{
						HxLOG_Error("svc_cas_MgrSubUpdateCasEtcEvent() failed.\n");
						return ERR_FAIL;
					}


					/* URI 정보를 DRM data로 만든 후 DRM Signalled event로 TV manager로 전송 (ap_player.c 파일 EVT_CAS_DRM_SIGNALLED에서 처리 될 것임) */
					stCiPlusLicenseInfo.usLicenseLength = pstCcConfirmDataInfo->data.stPbLicenseInfo.unLicenseDataLen;
					HxSTD_memcpy(stCiPlusLicenseInfo.aucLicenseData, pstCcConfirmDataInfo->data.stPbLicenseInfo.pucLicenseData, pstCcConfirmDataInfo->data.stPbLicenseInfo.unLicenseDataLen);
					stCiPlusLicenseInfo.usProgramNum = pstCcConfirmDataInfo->data.stPbLicenseInfo.usProgramNumber;
					//stCiPlusLicenseInfo.aucBindedCamId : TODO?

					hErr = local_CiMgr_MakeCiDrmInfo(eDxCopyrightControl_CiPlusDrmType_License, (void *)&stCiPlusLicenseInfo, &pstDrmInfo, eDxCopyrightControl_CiPlusDrmWriteInfo_Writing);
					if(hErr != ERR_OK)
					{
						HxLOG_Error("local_CiMgr_MakeCiDrmInfo() failed.\n");
						return ERR_FAIL;
					}

					hErr = svc_cas_MgrSubUpdateCasEtcEventData(pstCcConfirmDataInfo->hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM, eSEVT_CAS_DRM_SIGNALLED,
						0, pstCcConfirmDataInfo->data.stPbLicenseInfo.unMetaIndex, (void*)pstDrmInfo, sizeof(DxCopyrightControl_t));	// p1, p2, p3: p2는 Playback시에만 사용.
					if(hErr != ERR_OK)
					{
						HxLOG_Error("svc_cas_MgrSubUpdateCasEtcEvent() failed.\n");
						return ERR_FAIL;
					}
				}
				else
				{
					// License Status가 OK 가 아니면, 더이상 해당 license 가 관리하는 영역에 대해서는 playback을 해 줄 수 없는 상황이다.
					// 따라서 현재 playback 하고 있는 구간에 대하여 playback 불가능 하다는 UI를 화면에 보이고, playback을 종료하거나,
					// 해당 content 뒷부분에 보여줄 수 있는 화면이 있다면 그 화면을 출력하도록 한다. (UI 시나리오 문서 참고)
					// 또한 License status를 FAIL 상태로 올려줘서 meta file로 저장을 하고 다음번 playback 할 때 해당 구간을 license exchange 없이 바로 시청 불가 처리하도록 함.

					hErr = svc_cas_MgrSubUpdateCasEtcEventData(pstCcConfirmDataInfo->hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM, eSEVT_CAS_DRM_AV_BLANK,
						eDxCAS_GROUPID_CAM, TRUE, NULL, 0);
					if(hErr != ERR_OK)
					{
						HxLOG_Error("CasMgrSub_UpdateCasEtcEvent() failed.\n");
						return ERR_FAIL;
					}
				}
			}
			break;
		case eCiCcProtocolType_LicenseCheckExchange:
			{
				// TODO: UI 시나리오에 따라 필요한 경우 추가 (kkkim)
				// UI 시나리오에서 녹화된 content가 몇번 더 재생 가능한지 파악하고 뿌려주기 위해 사용
				// 현재 시나리오상 아직은 구현 할 필요 없고, 만약 이런 시나리오가 추가 된다면 그때 구현하도록 한다.
			}
			break;
		case eCiCcProtocolType_RecordStart:
			{
				DxCopyrightControl_CiPlusCcRecCtrl_t		stCiPlusRecCtrl;
				DxCopyrightControl_t			*pstDrmInfo;

				if (pstCcConfirmDataInfo->data.stRecordStartInfo.eRecordStartStatus == CI_CC_STATUS_OK)
				{
					stCiPlusRecCtrl.eCiPlusCcRecInfo = eDxCopyrightControl_CiPlusCcRecInfo_RecordStart;
				}
				else
				{
					stCiPlusRecCtrl.eCiPlusCcRecInfo = eDxCopyrightControl_CiPlusCcRecInfo_RecordError;
				}

				hErr = local_CiMgr_MakeCiDrmInfo(eDxCopyrightControl_CiPlusDrmType_RecCtrl, (void *)&stCiPlusRecCtrl, &pstDrmInfo, eDxCopyrightControl_CiPlusDrmWriteInfo_None);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("local_CiMgr_MakeCiDrmInfo() failed.\n");
					return ERR_FAIL;
				}

				/* TV manager (ap_recorder 쪽으)로 녹화 정보를 올려 줌. */
				// TODO: Event를 받게 될 대상 Action이 CI manager에서 들고 있는 action 인지 확인하고 올리는 과정 필요 (kkkim)

				hErr = svc_cas_MgrSubUpdateCasEtcEventData(pstCcConfirmDataInfo->hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM, eSEVT_CAS_DRM_SIGNALLED, 0, 0, (void*)pstDrmInfo, sizeof(DxCopyrightControl_t));
				if(hErr != ERR_OK)
				{
					HxLOG_Error("svc_cas_MgrSubUpdateCasEtcEvent() failed.\n");
					return ERR_FAIL;
				}
			}

			break;
		case eCiCcProtocolType_RecordStop:
			{
				// Rocord stop message를 받았을 때 Host쪽에서 동작해야할 사항이 있다면 이곳에서 부터 추가
				// 그런데, Record Stop은 Host 시나리오상 먼저 triggering 되어서 stop 된 후 CAM쪽에 전송 하는 것이기 때문에
				// stop 에 대한 응답을 CAM으로 부터 받아도 그에 따른 동작을 더 진행 할 필요가 없음.
			}

			break;
		case eCiCcProtocolType_ChangeOperatingMode:
			{
				// change operation mode message를 받았을 때 Host쪽에서 동작해야할 사항이 있다면 이곳에서 부터 추가.
				// 그런데, Host 시나리오상 TSR <-> watch and buffer 로 바뀌었을 때 Host쪽에서 triggering 되어서 이를 CAM쪽에 전송 하는 것이고
				// Error와 관련된 응답을 CAM으로 받는다 하여도 Host 시나리오에 영향을 미치기에는 애매한 상황임.
				// 따라서 일단 Change Operating Mode에 대한 응답을 CAM으로 부터 받았을 때, status를 보고 Host 시나리오 조정하는 부분은 일단 보류 함.
			}
			break;
		default:
			break;
	}

	return ERR_OK;
}


static int local_CiMgr_ReqUpdateCcLicense(CI_SLOT_ID usSlotId, unsigned int unSessionId, CI_CONTENT_LICENSE_STATUS enLicenseStatus, CI_CONTENT_LICENSE_INFO *pstLicenseInfo)
{
	CI_CamInst_t		*pCam = (CI_CamInst_t *)NULL;
	CI_Context_t		*pCiContext = NULL;
	Handle_t			hAction = HANDLE_NULL;
	HBOOL				bFound = 0;
	HUINT32				i, j;
	HERROR				hErr = ERR_FAIL;

	if (pstLicenseInfo == NULL)
	{
		HxLOG_Error("invalid pstLicenseInfo.\n");
		return ERR_FAIL;
	}

	pCiContext = local_CiMgr_GetContextByAction(pstLicenseInfo->hAction);
	if (pCiContext == NULL)
	{
		HxLOG_Error("Sequence Error.\n");	// 반드시 잡아야 함
		return ERR_FAIL;
	}

	if (pCiContext->usSvcId != pstLicenseInfo->usProgramNumber)
	{
		HxLOG_Error("Program number is different!! Sequence Error!! \n");
		return ERR_FAIL;
	}

	if (enLicenseStatus == CI_CONTENT_LICENSE_STATUS_OK)
	{
		DxCopyrightControl_CiPlusUriInfo_t		stCiPlusUriInfo;
		DxCopyrightControl_CiPlusLicenseInfo_t	stCiPlusLicenseInfo;
		DxCopyrightControl_t			*pstDrmInfo;

		stCiPlusUriInfo.ucMode = CI_CC_URI_MODE_FROMCAM;
		stCiPlusUriInfo.ucAps = pstLicenseInfo->ucAps;
		stCiPlusUriInfo.ucEmi = pstLicenseInfo->ucEmi;
		stCiPlusUriInfo.ucIct = pstLicenseInfo->ucIct;
		stCiPlusUriInfo.ucRct = pstLicenseInfo->ucRct;
		stCiPlusUriInfo.ucDot = pstLicenseInfo->ucDot;
		stCiPlusUriInfo.ulMinutesForRL = pstLicenseInfo->ulMinutesForRL;

#if defined(CONFIG_DEBUG) // 중요 로그이므로 항시 찍어 주자.
		HxLOG_Print(" APS (%x), EMI (%x), ICT (%x), RCT (%x), DOT (%x), RL (%ld min)\n",
			stCiPlusUriInfo.ucAps, stCiPlusUriInfo.ucEmi, stCiPlusUriInfo.ucIct, stCiPlusUriInfo.ucRct, stCiPlusUriInfo.ucDot, stCiPlusUriInfo.ulMinutesForRL);
#endif
		// the step 18 of 5.1.4. Operating Mode Transitions From Unattended Mode
		// standby 에서 예약 녹화 중에 license update가 발생한 뒤 power on 시 analog output에 DOT 적용이 되지 않아 적용함. 문제가 있다면 standby 시에만 적용하도록 함
		s_stUriInfo[usSlotId] = stCiPlusUriInfo;


		/* URI 정보를 DRM data로 만든 후 DRM Signalled event로 TV manager로 전송 */
		hErr = local_CiMgr_MakeCiDrmInfo(eDxCopyrightControl_CiPlusDrmType_Uri, (void *)&stCiPlusUriInfo, &pstDrmInfo, eDxCopyrightControl_CiPlusDrmWriteInfo_Writing);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("local_CiMgr_MakeCiDrmInfo() failed.\n");
			return ERR_FAIL;
		}

		pCam = &s_astCiCam[usSlotId];

		for (i=0; i<CI_MAX_NUM_OF_ACT; i++)
		{
			bFound = FALSE;
			if (s_astCiContext[i].pCam == pCam)
			{
				hAction = s_astCiContext[i].hAction;
				if(hAction == HANDLE_NULL)
				{
					continue;
				}

				for (j=0; j<CAS_MAX_NUM_OF_ACTION; j++)
				{
					if (pCam->hActList[j] == hAction)
					{
						HxLOG_Print("action handle (0x%x) hAction (0x%x)\n", pCam->hActList[j], hAction);
						bFound = TRUE;
						break;
					}
				}

				if (bFound == TRUE)
				{
		/* TV manager(ap_recorder) 쪽으로 녹화 정보 전달 */
					hErr = svc_cas_MgrSubUpdateCasEtcEventData(hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM, eSEVT_CAS_DRM_SIGNALLED, 0, 0, (void*)pstDrmInfo, sizeof(DxCopyrightControl_t));
					if(hErr != ERR_OK)
					{
						HxLOG_Error("CasMgrSub_UpdateCasEtcEvent() failed.\n");
						return ERR_FAIL;
					}

					// s_astCiContext[i].bRcvUri = TRUE;
				}
				else
				{
					HxLOG_Error("ERROR: check...\n");
				}
			}
		}

		// s_abUriSent[pCam->usSlotId] = TRUE;

		/* URI 정보를 DRM data로 만든 후 DRM Signalled event로 TV manager로 전송 */
		stCiPlusLicenseInfo.usProgramNum = pstLicenseInfo->usProgramNumber;
		stCiPlusLicenseInfo.usLicenseLength = pstLicenseInfo->unLicenseDataLen;
		HxSTD_memcpy(stCiPlusLicenseInfo.aucLicenseData, pstLicenseInfo->pucLicenseData, pstLicenseInfo->unLicenseDataLen);
		HxSTD_memcpy(stCiPlusLicenseInfo.aucBindedCamId, pstLicenseInfo->ucBindedCamId, 8);

		hErr = local_CiMgr_MakeCiDrmInfo(eDxCopyrightControl_CiPlusDrmType_License, (void *)&stCiPlusLicenseInfo, &pstDrmInfo, eDxCopyrightControl_CiPlusDrmWriteInfo_Writing);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("local_CiMgr_MakeCiDrmInfo() failed.\n");
			return ERR_FAIL;
		}

		/* TV manager(ap_recorder) 쪽으로 녹화 정보 전달 */

		hErr = svc_cas_MgrSubUpdateCasEtcEventData(pstLicenseInfo->hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM, eSEVT_CAS_DRM_SIGNALLED, 0, 0, (void*)pstDrmInfo, sizeof(DxCopyrightControl_t));
		if(hErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_MgrSubUpdateCasEtcEvent() failed.\n");
			return ERR_FAIL;
		}
	}
	else
	{
		// License Status가 OK 가 아니면, 녹화되는 service는 scramble 되어 전달됨.
		// 따라서 OK가 아닌 경우에 원래 녹화 시나리오에서 정의된 바와 같이 scrambled 녹화를 진행할 것이고, 그렇다면 license 관리가 필요 없다.
		// 혹시 license status가 no entitlement 및 error등 으로 왔을때, recording 상태에 영향을 줘야 할 내용이 있다면 여기에 추가 할 것.
	}

	return ERR_OK;
}

static int local_CiMgr_ReqUpdateCcPinEvent(CI_SLOT_ID usSlotId, CI_PIN_EVENT_INFO *pstCcPinEvent, Handle_t hRecAction)
{
	CI_Context_t				*pCiContext = NULL;
	DxCopyrightControl_t					*pstDrmInfo;
	DxCopyrightControl_CiPlusParentalCtrlInfo_t	stCiPlusPcInfo;
	CI_CamInst_t				*pCam = (CI_CamInst_t *)NULL;
	Handle_t					hAction = HANDLE_NULL;
	HBOOL						bFound = 0;
	HUINT32						i, j;
	HERROR						hErr = ERR_FAIL;

	if (pstCcPinEvent == NULL)
	{
		HxLOG_Error("invalid pstCcPinEvent.\n");
		return ERR_FAIL;
	}

	pCiContext = local_CiMgr_GetContextByAction(hRecAction);
	if (pCiContext == NULL)
	{
		HxLOG_Error("Sequence Error.\n");	// 반드시 잡아야 함
		return ERR_FAIL;
	}

	if (pCiContext->usSvcId != pstCcPinEvent->usProgramNum)
	{
		HxLOG_Error("Program number is different!! Sequence Error!! \n");
		return ERR_FAIL;
	}

	stCiPlusPcInfo.utTimeStamp 	= pstCcPinEvent->utEventTime;
	stCiPlusPcInfo.usProgramNum = pstCcPinEvent->usProgramNum;
	stCiPlusPcInfo.ucAgeLimit	= pstCcPinEvent->ucRating;
	HxSTD_memcpy(stCiPlusPcInfo.aucPrivateData, pstCcPinEvent->aucPrivateData, 15);
	HxSTD_memcpy(stCiPlusPcInfo.aucBindedCamId, pstCcPinEvent->aucBindedCamId, 8);

	/* URI 정보를 DRM data로 만든 후 DRM Signalled event로 TV manager로 전송 */
	hErr = local_CiMgr_MakeCiDrmInfo(eDxCopyrightControl_CiPlusDrmType_ParentalCtrl, (void *)&stCiPlusPcInfo, &pstDrmInfo, eDxCopyrightControl_CiPlusDrmWriteInfo_Writing);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("local_CiMgr_MakeCiDrmInfo() failed.\n");
		return hErr;
	}

	pCam = &s_astCiCam[usSlotId];

	for (i=0; i<CI_MAX_NUM_OF_ACT; i++)
	{
		bFound = FALSE;
		if (s_astCiContext[i].pCam == pCam)
		{
			hAction = s_astCiContext[i].hAction;
			if(hAction == HANDLE_NULL)
			{
				continue;
			}

			for (j=0; j<CAS_MAX_NUM_OF_ACTION; j++)
			{
				if (pCam->hActList[j] == hAction)
				{
					HxLOG_Print("action handle (0x%x) hAction (0x%x)\n", pCam->hActList[j], hAction);
					bFound = TRUE;
					break;
				}
			}

			if (bFound == TRUE)
			{
				hErr = svc_cas_MgrSubUpdateCasEtcEventData(hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM, eSEVT_CAS_DRM_SIGNALLED, 0, 0, (void*)pstDrmInfo, sizeof(DxCopyrightControl_t));
				if(hErr != ERR_OK)
				{
					HxLOG_Error("CasMgrSub_UpdateCasEtcEvent() failed.\n");
					return hErr;
				}
			}
			else
			{
				HxLOG_Error("ERROR: check...\n");
			}
		}
	}

	return ERR_OK;
}


#endif	// #if defined(CONFIG_MW_CI_PLUS_CC_V_2)

static int local_CiMgr_ReqCcReport(CI_SLOT_ID usSlotId, CIPLUS_CcReportInfo_t *pstCcReportInfo)
{
	SvcCas_CcReport_t *pData = NULL;

	if(usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	if(pstCcReportInfo == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	if(pstCcReportInfo->eCcReport == eCiPlus_CcReport_AuthenticationDoneAndStartSacEstablishment
		|| pstCcReportInfo->eCcReport == eCiPlus_CcReport_UriVersionNegotiationDone)
	{
		// 이들은 CAS/CI mgr 에서 사용하기 위한 용도로 여기서는 처리할 것이 없다.
		return ERR_OK;
	}

	pData = &s_stCcReportStatus[usSlotId];

	pData->eCcReportType = pstCcReportInfo->eCcReport;
	pData->enArcErrType = pstCcReportInfo->enArcErr;
	pData->ulVer = ++s_ulCcReportVersion;

	/*	p1 : Slot Id
		p2 : DxCAS_GroupId_e
		p3 : Data Version
	*/
	svc_cas_MgrSubUpdateCasEtcEvent(HANDLE_NULL, s_astCiCam[usSlotId].hCasDevice, eDxCAS_GROUPID_CAM, eSEVT_CAS_CIPLUS_CC_REPORT, (HUINT32)usSlotId, eDxCAS_GROUPID_CAM, pData->ulVer);

	return ERR_OK;
}

static int local_CiMgr_ReqPinCapaInfo(CI_SLOT_ID usSlotId)
{
	HERROR nResult = ERR_FAIL;

	if(usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	nResult = local_CiMgr_SendEvent(CI_PLUS_CAM_PIN_CAPA_INFO, (void *)&usSlotId, (unsigned long)sizeof(CI_SLOT_ID));
	if(ERR_OK!=nResult)
		HxLOG_Critical("Critical Error!!!\n");

	return ERR_OK;
}

/*
	CU Resource Handling
*/
static int local_CiMgr_RequestCiPlusCamUpgrade(CI_SLOT_ID usSlotId, CIPLUS_CamUpgradeInfo_t *pstCamUpgradeInfo)
{
	SvcCas_CiPlusUpgrade_t *pData = NULL;

	if(usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	if(pstCamUpgradeInfo == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	pData = &s_stCiPlusUpgradeStatus[usSlotId];

	pData->eCamUpgradeMode = pstCamUpgradeInfo->eCamUpgradeMode;
	pData->ucDownloadProgressInPercentage = pstCamUpgradeInfo->ucDownloadProgressInPercentage;
	pData->eCamUpgradeResetMode = pstCamUpgradeInfo->eCamUpgradeResetMode;
	pData->ulVer = ++s_ulCiPlusUpgradeVersion;

	HxLOG_Info("slot id (%d), UpgradeMode (%d), DownloadProgress (%d), ResetMode (%d), Version (%d) \n",
		usSlotId, pData->eCamUpgradeMode, pData->ucDownloadProgressInPercentage, pData->eCamUpgradeResetMode, pData->ulVer);

	/*	p1 : Slot Id
		p2 : DxCAS_GroupId_e
		p3 : Data Version
	*/
	svc_cas_MgrSubUpdateCasEtcEvent(HANDLE_NULL, s_astCiCam[usSlotId].hCasDevice, eDxCAS_GROUPID_CAM, eSEVT_CAS_CIPLUS_UPGRADE, (HUINT32)usSlotId, eDxCAS_GROUPID_CAM, pData->ulVer);

	return ERR_OK;
}

static int local_CiMgr_OperatorPrepareReq(CI_SLOT_ID usSlotId, CI_OP_REFRESH_REQUEST eOpRefreshReq, unsigned char *pszProfileName)
{
	CI_Context_t	*pstContext;
	HBOOL			bIsCamDescrabling = FALSE;
	HUINT32 		i = 0;

	HxLOG_Info("+local_CiMgr_OperatorPrepareReq()...\n");

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	s_astCiCam[usSlotId].stOpParam.usSlotId = usSlotId;
	s_astCiCam[usSlotId].stOpParam.pszProfileName = pszProfileName;
	s_astCiCam[usSlotId].stOpParam.eOpRefreshReq = eOpRefreshReq;

	for (i=0; i<NUM_VIEW_ACTION; i++)
	{
		pstContext = local_CiMgr_GetContextByActionId(i);
		if(pstContext->pCam)	// 현재 해당 CAM으로 Descramble 되고 있음을 나타 냄.
		{
			if(pstContext->pCam->usSlotId == usSlotId)
				bIsCamDescrabling = TRUE;
		}
	}

	// Operator Profile Search 시나리오를 타야하는 조건
	// 1. OP init이 되어있지 않아 init을 시작해야 하는데, 현재 service가 OP init을 요구하는 CAM을 타고 있을 때
	// 2. OP List를 업데이트 혹은 구성해야 하는데, 지금 하는 것이 아닌 예약을 걸어서 할 때 app쪽까지 바로 올려 예약시나리오를 탈때
	// 3. CIS에서 ciprof 값이 1이여서 Operator Profile 구성 정보가 CIS에 나와 있는데, Init이 되어 있지 않은 경우 일단 올리고 app 쪽에서도 CIS 체크하고 해당하는 시나리오 수행

	if(bIsCamDescrabling || eOpRefreshReq == CI_OP_REFRESH_REQUEST_RESERVED_TUNE || CI_OperatorProfileSupported(usSlotId))
	{
		if(CI_OperatorProfileSupported(usSlotId))
		{
			// TODO: 3번 시나리오 같은 경우에는 CAM 삽입한 상태로 켜졌을 때 Webapp이 준비되어있지 않아 문제가 발생하여, webapp준비 될 때까지 wating이 필요 함.
			//VK_TASK_Sleep(30000);
		}
		if(local_CiMgr_SendEvent(CI_PLUS_OPERATOR_PREPARE_REQ, (void *)&s_astCiCam[usSlotId].stOpParam, (unsigned long)sizeof(CI_PLUS_OPERATOR_PREPARE_PARAM)) != ERR_OK)
			HxLOG_Critical("Critical Error!!!\n");

		s_astCiCam[usSlotId].bOpSearchReq = FALSE;
	}
	else
	{
		s_astCiCam[usSlotId].bOpSearchReq = TRUE;
	}

	HxLOG_Info("-local_CiMgr_OperatorPrepareReq()...\n");
	return ERR_OK;
}


static int local_CiMgr_OperatorTune(CI_SLOT_ID usSlotId, unsigned char *pucDesLoop, unsigned short usDesLoopLen)
{
	CI_PLUS_OPERATOR_TUNE_PARAM		stParam;

	HxLOG_Info("+local_CiMgr_OperatorTune()...\n");

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	stParam.usSlotId		= usSlotId;
	stParam.usDesLoopLen	= usDesLoopLen;
	stParam.pucDesLoop		= (HUINT8 *)OxCAS_Malloc(usDesLoopLen);
	if(stParam.pucDesLoop)
	{
		HxSTD_memcpy(stParam.pucDesLoop, pucDesLoop, usDesLoopLen);
	}

	if(local_CiMgr_SendEvent(CI_PLUS_OPERATOR_TUNE, (void *)&stParam, (unsigned long)sizeof(CI_PLUS_OPERATOR_TUNE_PARAM)) != ERR_OK)
		HxLOG_Critical("Critical Error!!!\n");

	HxLOG_Info("-local_CiMgr_OperatorTune()...\n");

	return ERR_OK;
}

static int local_CiMgr_OperatorNit(CI_SLOT_ID usSlotId, unsigned char *pszProfileName, unsigned char* pucTextCoding, unsigned char *pucCiCamNit, unsigned short usCiCamNitLen)
{
	CI_PLUS_OPERATOR_NIT_PARAM stParam;

	HxLOG_Info("+local_CiMgr_OperatorNit()...\n");

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	stParam.usSlotId		= usSlotId;
	stParam.pszProfileName	= pszProfileName;
	HxSTD_memcpy(stParam.aucTextCoding, pucTextCoding, 7);
	stParam.usCiCamNitLen	= usCiCamNitLen;
	stParam.pucCiCamNit		= (HUINT8 *)OxCAS_Malloc(usCiCamNitLen);
	stParam.bSimpleNitChange = CI_CheckSimpleEntitlementChange(usSlotId);

	if(stParam.pucCiCamNit)
	{
		HxSTD_memcpy(stParam.pucCiCamNit, pucCiCamNit, usCiCamNitLen);
	}

	if(local_CiMgr_SendEvent(CI_PLUS_OPERATOR_NIT, (void *)&stParam, (unsigned long)sizeof(CI_PLUS_OPERATOR_NIT_PARAM)) != ERR_OK)
		HxLOG_Critical("Critical Error!!!\n");

	HxLOG_Info("-local_CiMgr_OperatorNit()...\n");

	return ERR_OK;
}

static HERROR local_CiMgr_RegisterCiPlusCb(void)
{
	CIPLUS_NOTIFY_FUNC_LIST notifyPlusList;

/* 콜백 함수 등록 (CI+ stack -> CI mgr)
*/
	VK_memset32(&notifyPlusList, 0, sizeof(CIPLUS_NOTIFY_FUNC_LIST));

	notifyPlusList.pfnNotifySetKey = local_CiMgr_ReqSetKey;
	notifyPlusList.pfnNotifyUriControl = local_CiMgr_ReqUriControl;
	notifyPlusList.pfnNotifySrmControl = local_CiMgr_ReqSrmControl;
	notifyPlusList.pfnNotifyHostTune = local_CiMgr_ReqHostTune;
	notifyPlusList.pfnNotifyPidReplace = local_CiMgr_ReqPidReplace;
	notifyPlusList.pfnNotifyCcReport = local_CiMgr_ReqUpdateCcResource; // 이건 CI mgr 로 notify 하기 위한 용도.
	notifyPlusList.pfnNotifyPinCapaInfo = local_CiMgr_ReqPinCapaInfo; // 이건 CI mgr 로 notify 하기 위한 용도.

#if defined(CONFIG_OP_CANALREADY)
	notifyPlusList.pfnNotifyCanalReadyInit = local_CiMgr_ReqCanalReadyInitNotify;
	notifyPlusList.pfnNotifyCanalReadyMoralLevel = local_CiMgr_ReqCanalReadyMoralityLevelNotify;
	notifyPlusList.pfnNotifyCanalReadyPinReset = local_CiMgr_ReqCanalReadyMoralityPinReset;
#endif

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	notifyPlusList.pfnNotifyCcPinStatus = local_CiMgr_ReqUpdatePinStatus;
	notifyPlusList.pfnNotifyCcCnfData = local_CiMgr_ReqUpdateCiPlusConfirmOperation;
	notifyPlusList.pfnNotifyCcLicense = local_CiMgr_ReqUpdateCcLicense;
	notifyPlusList.pfnNotifyCcPinEvent = local_CiMgr_ReqUpdateCcPinEvent;
#endif

	CIPLUS_AddNotifyFunc(notifyPlusList);

/* 콜백 함수 등록 (CI+ statck-> CAS Mgr)
*/
	VK_memset32(&notifyPlusList, 0, sizeof(CIPLUS_NOTIFY_FUNC_LIST));

	notifyPlusList.pfnNotifyCcReport = (CI_NOTIFY_CC_REPORT_FUNC)local_CiMgr_ReqCcReport; // 이건 APP 로 notify 하기 위한 용도.
	notifyPlusList.pfnNotifyCamUpgrade = (CI_NOTIFY_CAM_UPGRADE_FUNC)local_CiMgr_RequestCiPlusCamUpgrade; // 이건 APP 로 notify 하기 위한 용도.
	notifyPlusList.pfnNotifyOperatorPrepareReq = (CI_NOTIFY_OPERATOR_PREPARE_REQ_FUNC)local_CiMgr_OperatorPrepareReq;	// Operator Profile APDU가 시작됨을 알림
	notifyPlusList.pfnNotifyOperatorTune = (CI_NOTIFY_OPERATOR_TUNE_FUNC)local_CiMgr_OperatorTune;		// 전달된 delivery descriptor 값으로 Tune 요청
	notifyPlusList.pfnNotifyOperatorNit = (CI_NOTIFY_OPERATOR_NIT_FUNC)local_CiMgr_OperatorNit;		// 전달된 CICAM NIT으로 채널그룹 요청

	CIPLUS_AddNotifyFunc(notifyPlusList);

	return ERR_OK;
}

static void local_CiMgr_FreeCiDescList(void *pvData)
{
	CAS_CiDscInfo_t *pstData = NULL;

	pstData = (CAS_CiDscInfo_t *)pvData;
	if(pstData == NULL)
	{
		HxLOG_Error("null\n");
		return;
	}

	if (pstData->pDesRaw)
	{
		HxLOG_Info("svc_cas_SubSvcSiFreeDescriptor\n");
		svc_cas_SubSvcSiFreeDescriptor((void *)pstData->pDesRaw);
	}

	OxCAS_Free(pstData);
	pstData = NULL;

	return;
}

static HERROR local_CiMgr_InitSdtStatus(HUINT32 ulIdx)
{
	CAS_SdtStatus_t *pSdt = NULL;

	if(ulIdx >= CAS_MAX_NUM_OF_TS)
	{
		return ERR_FAIL;
	}

	pSdt = &s_stSdtStatus[ulIdx];

	pSdt->bUsed = CAS_NOT_USED;
	pSdt->ulActionId = CAS_RESOURCE_NONE;
	pSdt->ulDemuxId = CAS_RESOURCE_NONE;
	pSdt->usTsId = 0;
	pSdt->usUniqueId = 0;

	if(pSdt->pstList != NULL)
	{
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pSdt->pstList, local_CiMgr_FreeCiDescList);
	}

	pSdt->pstList = NULL;
	pSdt->ulSvcNum = 0;
	pSdt->ulFilterId = CAS_RESOURCE_NONE;
	pSdt->ucTableId = 0xFF;
	pSdt->usExtId = 0;
	pSdt->usPid = PID_NULL;
	pSdt->eFilterMode = 0;
	pSdt->ulTblState = 0;
	pSdt->bUseVerNum = 0;
	pSdt->bCrc = 0;
	pSdt->bUseExtId = FALSE;
	pSdt->bTableFilter = FALSE;

	return ERR_OK;
}

#endif

static HERROR local_CiMgr_RegisterCb(void)
{
	HERROR	hErr;

/* 콜백 함수 등록 (CI mgr -> CAS mgr) */
	hErr = local_CiMgr_RegisterCasMgrCb();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("local_CiMgr_RegisterCasMgrCb() failed\n");
		return ERR_FAIL;
	}

/* 콜백 함수 등록
	(CI stack -> CI mgr)
	(CI statck-> CAS Mgr) -> 요고 좀 이상하다.. 별로 좋아 보이지 않으이.. 쩝..
*/
	hErr = local_CiMgr_RegisterCiCb();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("local_CiMgr_RegisterCiCb() failed\n");
		return ERR_FAIL;
	}

/* CI PLUS 콜백 함수 등록
	(CI+ stack -> CI mgr)
	(CI+ statck-> CAS Mgr or AP) -> 요것도 역시 좀 이상하다.. 별로 좋아 보이지 않으이.. 쩝..
*/
#if defined(CONFIG_MW_CI_PLUS)
	hErr = local_CiMgr_RegisterCiPlusCb();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("local_CiMgr_RegisterCiPlusCb() failed\n");
		return ERR_FAIL;
	}
#endif

	return ERR_OK;
}

////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_CAPMT_AND_DESCRAMBLING___
////////////////////////////////////////////////////////////

static CI_CA_PMT_LIST_MANAGEMENT local_CiMgr_GetCplmCmd(Handle_t hAction, CI_CamInst_t *pCam)
{
	HUINT8 ucCaSysIdSpecifier = 0;
	CI_Context_t *pCiContext = NULL;

	pCiContext = local_CiMgr_GetContextByAction(hAction);
	if (pCiContext == NULL) {
		HxLOG_Error("invalid context\n");
		return CI_CPLM_ONLY;
	}

	if (pCam == NULL)
	{
		HxLOG_Error("\n");
		return CI_CPLM_ONLY;
	}

	/* CI_CPLM_UPDATE 에 문제가 있는 캠을 제외하곤 원칙적으로 모두 CI_CPLM_UPDATE 를 해야 맞음.
	   그러나 일부 캠에선 CI_CPLM_ONLY 외에는 문제가 되기도 하므로 이를 위한 예외 처리가 필요하다면 여기에 추가해 주세요. */
	if ((pCam->ausCaSysId != NULL) && (pCam->usManufacturer != 0))
	{
		/* <TODO_CI> : 특정 캠에 대한 예외 처리가 필요하다면...
		   Application Info 의 application_manufacturer / manufacturer_code / menu_string_length 및
		   CA Info 의 CA_system_id 을 모두 체크하도록 하자.
		   엉뚱한 캠에 영향을 주지 않도록... */
		ucCaSysIdSpecifier = (HUINT8)((pCam->usManufacturer >> 8) & 0x00ff); // 여기서는 usManufacturer 로, local_cimgr_GetCaPmt()에선 ausCaSysId 로 캠 예외 처리하고 있음. 수정 필요 !!
		switch (ucCaSysIdSpecifier)
		{
			case eCA_SYS_ID_ASTON:
			case SKYCIPLUS_CAM_MANUFACTURER:
//			case eCA_SYS_ID_CONAX: // local_CiMgr_GetCaPmt()에 있는 내용에 부합하려면 eCA_SYS_ID_CONAX 도 CI_CPLM_ONLY 로 리턴해야 할 듯... -> 캠을 인식하는 방법이 다르므로 일단 원래 코드대로 막아둠. 추후 확인 필요 !!
				return CI_CPLM_ONLY;

			case CRYPTOWORKS_CAM_MANUFACTURER:
				local_CiMgr_StopDescramble(pCiContext, pCam->usSlotId); // 이유가 뭐지?
				return CI_CPLM_ONLY;

			default :
				return CI_CPLM_UPDATE;
		}
	}

	return CI_CPLM_ONLY; // if ((pCam->ausCaSysId != NULL) && (pCam->usManufacturer != 0)) 조건에 부합되지 못하면 일단 정상적인 캠은 아니니 CI_CPLM_ONLY 로 리턴하는 것이 맞을 듯...
}

static HUINT8 local_CiMgr_GetLengthField(HUINT8 *pucLenField, unsigned short usLen)
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

static HBOOL local_CiMgr_CheckEsTypeValidityForEsInfo(EsType_t eType)
{
	if ( (eType == eEsType_Video) || (eType == eEsType_Audio) || (eType == eEsType_Subtitle)
		|| (eType == eEsType_Teletext) )//	|| (eType == eEsType_Data) )
	{
		return TRUE;
	}

	return FALSE;
}

static HERROR local_CiMgr_MakeEsInfo(	SvcCas_ActionType_e eActType,
											CI_PROGRAM_INFO *pInput,		/* in */
											CI_PROGRAM_INFO *pOutput,		/* out */
											CI_CA_PMT_LIST_MANAGEMENT nManage,
											CI_CA_PMT_CMD nCmd,
											HUINT8 ucCaSysIdSpecifier)
{
	HUINT32 i, j, k;

	/*
		만약 Es Info에 대한 Cam 별 exception이 공통이라면 이것으로 구현해야 함.
	*/

	if(pInput == NULL || pOutput == NULL)
	{
		HxLOG_Error("input parameter is null. \n");
		return ERR_FAIL;
	}

	switch (ucCaSysIdSpecifier)
	{

/* CA System ID specifier 별로 exception handling이 필요하면 밑에 추가하세요. 왜 필요한지 이유도 꼭 기입! */

//		case SOME_CA_SYS_ID:
//			...
//			break;

		default:
		{
			/*
				normal cam인 경우 pmt의 es를 기준으로 set dsc의 pid와 일치하는 es를 list up한다.
				ca pmt 작성 시 사용할 program info를 update한다.
				이곳에서 작성된 program info는 cam instance에 update 된다.
			*/
			k = 0;

			for (i=0; i<pInput->pPmtInfo->ulNumOfElement; i++)
			{
				for (j=0; j<pInput->ucNumEs; j++)
				{
					if ((pInput->pPmtInfo->stElementInfo[i].eEsType == pInput->aEsInfo[j].eEsType)
						&& (pInput->pPmtInfo->stElementInfo[i].usPid == pInput->aEsInfo[j].usEsPid))
					{
						pOutput->aEsInfo[k].eEsType = pInput->aEsInfo[j].eEsType;
						pOutput->aEsInfo[k].usEsPid = pInput->aEsInfo[j].usEsPid;
						pOutput->aEsInfo[k].pEs = &pInput->pPmtInfo->stElementInfo[i];
						k++;
					}
				}
			}

			pOutput->ucNumEs = k;
			HxLOG_Info(" num of es (%d) \n", k);

#if 0	// Debug
			HxLOG_Print("\n\n\n\n ------------- pid info for ca pmt ------------------------ \n\n");
			for(i=0; i<pOutput->ucNumEs; i++)
			{
				HxLOG_Print(" (%d)- pid(0x%x) (%s), es raw (%x)\n", i, pOutput->aEsInfo[i].usEsPid,
					svc_cas_DbgMakeStrEsType(pOutput->aEsInfo[i].eEsType), (HUINT32)pOutput->aEsInfo[i].pEs);
			}
			HxLOG_Print("\n\n\n\n ------------- end of pid info for ca pmt ------------------------ \n\n");
#endif

		}
		break;
	}

	return ERR_OK;
}

static HERROR local_CiMgr_MakeEsInfoForWatching(	CI_Context_t *pCiContext,			/* in */
														CI_PROGRAM_INFO *pProgramInfo,		/* out */
														CI_CA_PMT_LIST_MANAGEMENT nManage,
														CI_CA_PMT_CMD nCmd,
														HUINT8 ucCaSysIdSpecifier)
{
	HUINT32 i, k;

	if(pCiContext == NULL || pProgramInfo == NULL)
	{
		HxLOG_Error("input parameter is null. \n");
		return ERR_FAIL;
	}

	if (pCiContext->eActType == eCAS_ActionType_Recording
		|| pCiContext->eActType == eCAS_ActionType_Tsr_Rec
		|| pCiContext->eActType == eCAS_ActionType_Data)
	{
		HxLOG_Error("recording, tsr_rec, data is not supported.\n");
		return ERR_FAIL;
	}

	/* host에서 pmt update 여부 */
	pProgramInfo->pPmtInfo = pCiContext->stProgInfo.pPmtInfo;
	if(pProgramInfo->pPmtInfo == NULL)
	{
		HxLOG_Error("pmt info is null. \n");
		return ERR_FAIL;
	}

	/* 초기화 */
	HxSTD_memset(pCiContext->stProgInfo.aEsInfo, 0, sizeof(CI_ES_INFO) * CAS_MAX_NUM_OF_ES);
	pCiContext->stProgInfo.ucNumEs = 0;

	for (k=0, i=0; i<pCiContext->stProgInfo.pPmtInfo->ulNumOfElement; i++)
	{
		if ( (local_CiMgr_CheckEsTypeValidityForEsInfo(pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].eEsType) == TRUE)
			&& (pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].usPid != PID_NULL))
	{
				pCiContext->stProgInfo.aEsInfo[k].eEsType = pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].eEsType;
				pCiContext->stProgInfo.aEsInfo[k].usEsPid = pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].usPid;
				//pCiContext->stProgInfo.aEsInfo[k].pEs = &pCiContext->stProgInfo.pPmtInfo->stElementInfo[i];
				k++;
			}
	}

	pCiContext->stProgInfo.ucNumEs = k;
	HxLOG_Info(" num of es (%d) \n", k);

	local_CiMgr_MakeEsInfo(pCiContext->eActType,
							&pCiContext->stProgInfo,	/* in */
							pProgramInfo,			/* out */
							nManage,
							nCmd,
							ucCaSysIdSpecifier);

	return ERR_OK;
}

static HERROR local_CiMgr_MakeEsInfoForRecording(
													CI_Context_t *pCiContext,			/* in */
													CI_PROGRAM_INFO *pProgramInfo,		/* out */
													CI_CA_PMT_LIST_MANAGEMENT nManage,
													CI_CA_PMT_CMD nCmd,
													HUINT8 ucCaSysIdSpecifier
													)
{
	HUINT32 i, k;

	if(pCiContext == NULL || pProgramInfo == NULL)
	{
		HxLOG_Error("input parameter is null. \n");
		return ERR_FAIL;
	}

	if (pCiContext->eActType == eCAS_ActionType_Live
		|| pCiContext->eActType == eCAS_ActionType_Playback
		|| pCiContext->eActType == eCAS_ActionType_Tsr_Play)
	{
		HxLOG_Error("live, playback, tsr play is not supported in this function. \n");
		return ERR_FAIL;
	}
	else if (pCiContext->eActType == eCAS_ActionType_Data)
	{
		HxLOG_Error("data is not supported.\n");
		return ERR_FAIL;
	}

	/* host에서 pmt update 여부 */
	pProgramInfo->pPmtInfo = pCiContext->stProgInfo.pPmtInfo;
	if(pProgramInfo->pPmtInfo == NULL)
	{
		HxLOG_Error("pmt info is null. \n");
		return ERR_FAIL;
	}

	/* 초기화 */
	HxSTD_memset(pCiContext->stProgInfo.aEsInfo, 0, sizeof(CI_ES_INFO) * CAS_MAX_NUM_OF_ES);
	pCiContext->stProgInfo.ucNumEs = 0;

	for (k=0, i=0; i<pCiContext->stProgInfo.pPmtInfo->ulNumOfElement; i++)
	{
		if ( (local_CiMgr_CheckEsTypeValidityForEsInfo(pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].eEsType) == TRUE)
			&& (pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].usPid != PID_NULL))
			{
				pCiContext->stProgInfo.aEsInfo[k].eEsType = pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].eEsType;
				pCiContext->stProgInfo.aEsInfo[k].usEsPid = pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].usPid;
				pCiContext->stProgInfo.aEsInfo[k].pEs = &pCiContext->stProgInfo.pPmtInfo->stElementInfo[i];
				k++;
			}
	}

	pCiContext->stProgInfo.ucNumEs = k;
	HxLOG_Info(" num of es (%d) \n", k);

	local_CiMgr_MakeEsInfo(pCiContext->eActType,
							&pCiContext->stProgInfo,	/* in */
							pProgramInfo,				/* out */
							nManage,
							nCmd,
							ucCaSysIdSpecifier);

	return ERR_OK;
}

/* context의 pid & pmt 정보를 가지고 ca pmt를 만들기 위한 program info를 만든다. */
static HERROR local_CiMgr_MakeEsInfoForCaPmt(
													CI_Context_t *pCiContext,
													CI_PROGRAM_INFO *pProgram,
													HUINT8 ucCaSysIdSpecifier,
													CI_CA_PMT_LIST_MANAGEMENT nManage,
													CI_CA_PMT_CMD nCmd)
{
	HUINT32 nResult = 0;

	if(pCiContext == NULL || pProgram == NULL)
	{
		HxLOG_Error("input parameter is null. \n");
		return ERR_FAIL;
	}

	if (pCiContext->eActType == eCAS_ActionType_Live
		|| pCiContext->eActType == eCAS_ActionType_Playback
		|| pCiContext->eActType == eCAS_ActionType_Tsr_Play)
	{
		/* pid 6개 고정 */
		nResult = local_CiMgr_MakeEsInfoForWatching(pCiContext, pProgram, nManage, nCmd, ucCaSysIdSpecifier);
		if(nResult != ERR_OK)
		{
			HxLOG_Error(" local_cimgr_MakeEsInfoForWatching \n");
			return ERR_FAIL;
		}
	}
	else if (pCiContext->eActType == eCAS_ActionType_Recording
		|| pCiContext->eActType == eCAS_ActionType_Descramble_a_File
		|| pCiContext->eActType == eCAS_ActionType_Tsr_Rec)
	{
		/* pmt 기준으로 pid 개수 설정 가능. */
		nResult = local_CiMgr_MakeEsInfoForRecording(pCiContext, pProgram, nManage, nCmd, ucCaSysIdSpecifier);
		if(nResult != ERR_OK)
		{
			HxLOG_Error(" local_cimgr_MakeEsInfoForRecording \n");
			return ERR_FAIL;
		}
	}
	else if (pCiContext->eActType == eCAS_ActionType_Data)
	{
		/* pid 한개 고정 */
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_CiMgr_GetCaPmt(
										CI_CamInst_t *pCam,
										CI_Context_t *pCiContext,
										CI_CA_PMT_LIST_MANAGEMENT nManage,
										CI_CA_PMT_CMD nCmd,
										CI_CA_PMT_DATA *pCaPmt
										)
{
	HUINT8 *pucCaPmt = NULL;
	unsigned short usCaPmtLen = 0;
	unsigned short usProgInfoLen;
	unsigned short ausEsInfoLen[CAS_MAX_NUM_OF_ES];
	unsigned short usIdx;
	SvcCas_PmtInfo_t *pPmtInfo = NULL;
	CI_ES_INFO *aEsInfo = NULL;
	HUINT8 aucLenField[3];
	HUINT8 ucLenFieldSize;
	HUINT8 ucCaSysIdSpecifier = 0;
	CI_PROGRAM_INFO ciProgInfo; 			// pProgInfo 정보를 필요에 따라 가공하기 위해.
	HUINT32 i, j;
	#ifndef _Contains_all_the_CA_descriptors_ // org
	HUINT32 k;
	#endif

	HUINT32 nResult = 0;

	if (pCam == NULL)
	{
		HxLOG_Error("pCam is null\n");
		return ERR_FAIL;
	}
	if (pCam->hCasDevice == (Handle_t)HANDLE_NULL)
	{
		HxLOG_Error("hCasDevice is null\n");
		return ERR_FAIL;
	}

	HxSTD_memset(&ciProgInfo, 0x00, sizeof(CI_PROGRAM_INFO));

	/*-----------------------------------------------------------------------------------------------------*/
	/*--- ca_pmt () 생성에 앞서 캠에 따라 CI_PROGRAM_INFO 에 대한 예외 처리가 필요하다면 미리 작업해 둠 ---*/
	/*-----------------------------------------------------------------------------------------------------*/

	/*

		캠에 따라 문제가 될 수 있는 유형의 예 (모두 캠이 스펙 아웃인 경우이다) :

		1. programme level 에만 ca desc 가 존재하는 경우
		   -> 일반적으로 문제되지 않음.

		2. programme level 과 ES level 에 동일한 ca desc 가 존재하는 경우
		   -> 원칙적으로 이 경우 캠이 ES level 에 있는 ca desc 를 우선해야 하나 그렇지 못한 경우 문제가 될 수 있음.
		   -> programme level 에 있는 ca desc 를 제거하도록 예외 처리한다.
		   -> 그리고 만약 ES level 에 ca desc 가 없는 ES PID 가 존재한다면 programme level 에 있는 ca desc 를 추가하도록 한다.

		3. ES level 에만 ca desc 가 존재하는 경우
		   -> ES level 에 ca desc 가 없는 ES PID 가 존재한다면 ca desc 를 가지고 있는 ES PID 에서 문제가 되는 경우가 있다고 함 (Viaccess, Polsat)
		   -> ca desc 가 없는 ES PID 를 제거하도록 예외 처리한다 (local_cimgr_ProcSetDsc()에 Viaccess 캠에 대해 처리한 코드가 있으므로 일단 거기에 조건 추가하면 됨)

		4. 다수의 오디오를 갖는 경우 오디오 PID 를 한번에 모두 전달하면 일부 오디오가 나오지 않는 경우
		   -> 실제 출력 오디오만 포함하도록 예외 처리한다. 이 경우 현재 소스 구조를 추가 변경할 필요가 있음.

	    5. 다수의 ca desc 들을 가지는 H/E 에서 (simulcrypt) 캠이 가진 ca_system_id 에 매칭되지 않는 ca desc 를 함께 보내는 경우 (_Contains_all_the_CA_descriptors_ define 적용시)
	       -> 캠의 ca_system_id 에 해당 되는 ca desc 를 가진 것만 처리하도록 예외 처리를 하도록 한다.
	       -> 위의 2~4 번과 함께 고려하여 처리할 것.

		=> 가능한 문제를 우회할 수 있도록 OP의 H/E 수정을 유도하거나 H/E 수정으로 해결되지 못하는 것이거나 H/E 수정이 용이치 않은 경우 아래에 예외 처리를 하도록 한다.

	*/

	if (pCam->usCaInfoCnt > 0)
	{
		/* <TODO_CI> : 특정 캠에 대한 예외 처리가 필요하다면...
		   Application Info 의 application_manufacturer / manufacturer_code / menu_string_length 및
		   CA Info 의 CA_system_id 을 모두 체크하도록 하자.
		   엉뚱한 캠에 영향을 주지 않도록... */
		ucCaSysIdSpecifier = (HUINT8)((pCam->ausCaSysId[0] >> 8) & 0x00ff); // 여기서는 ausCaSysId 로, local_cimgr_GetCplmCmd()에선 usManufacturer 로 캠 예외 처리하고 있음. 수정 필요 !!
	}

	HxLOG_Info("ucCaSysIdSpecifier (0x%x)\n", ucCaSysIdSpecifier);

	/*
		pmt, set dsc 등 정보를 보고 ci Prog Info를 Update하여 준다.
		context의 정보는 원본 그대로 유지하고,
		ci Prog Info만을 수정한다.
	*/
	nResult = local_CiMgr_MakeEsInfoForCaPmt(pCiContext, &ciProgInfo, ucCaSysIdSpecifier, nManage, nCmd);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("local_cimgr_MakeEsInfoForCaPmt.\n");
		return ERR_FAIL;
	}


	/*---------------------------------------*/
	/*--- ca_pmt () 생성을 위한 길이 계산 ---*/
	/*---------------------------------------*/

	// calculate ca_pmt() length
	usCaPmtLen = 9;	// ca_pmt_tag + ca_pmt_list_management + program_number
					// + version_number + current_next_indicator + program_info_length

	/* pPmtInfo는 ci Mgr의 member임. pointer 변경 가능성은 없지만,  */
	pPmtInfo = ciProgInfo.pPmtInfo;
	if( pPmtInfo == NULL )
	{
		HxLOG_Error("pPmtInfo is null\n");
		return ERR_FAIL;
	}

	usProgInfoLen = 0;
	if (pPmtInfo->ulNumOfProgramCaDescriptor != 0)
	{
		#ifndef _Contains_all_the_CA_descriptors_ // org
		for (i=0; i<pPmtInfo->ulNumOfProgramCaDescriptor; i++)
		{
			for (j=0; j<pCam->usCaInfoCnt; j++)
			{
				if (pPmtInfo->stProgramCaDescriptor[i].usCaSysId == pCam->ausCaSysId[j])
				{
					if (usProgInfoLen == 0)
						usProgInfoLen++; // ca_pmt_cmd_id
					usProgInfoLen += pPmtInfo->stProgramCaDescriptor[i].ucRawLen;
					break;
				}
			}
		}
		#else // 이게 CI 및 CI+ 스펙에 맞다.
		for (i=0; i<pPmtInfo->ulNumOfProgramCaDescriptor; i++)
		{
			if (usProgInfoLen == 0)
				usProgInfoLen++; // ca_pmt_cmd_id
			usProgInfoLen += pPmtInfo->stProgramCaDescriptor[i].ucRawLen;
		}
		#endif
	}

	usCaPmtLen += usProgInfoLen;

	for (i=0; i<ciProgInfo.ucNumEs; i++)
	{
		usCaPmtLen += 5; // stream_type + elementary_PID + ES_info_length

		ausEsInfoLen[i] = 0;
		if (ciProgInfo.aEsInfo[i].pEs->ulNumOfCaDescriptor != 0)
		{
			#ifndef _Contains_all_the_CA_descriptors_ // org
			for (j=0; j<ciProgInfo.aEsInfo[i].pEs->ulNumOfCaDescriptor; j++)
			{
				for (k=0; k<pCam->usCaInfoCnt; k++)
				{
					if (ciProgInfo.aEsInfo[i].pEs->stElementCaDescriptor[j].usCaSysId == pCam->ausCaSysId[k])
					{
						if (ausEsInfoLen[i] == 0)
							ausEsInfoLen[i]++; // ca_pmt_cmd_id
						ausEsInfoLen[i] += ciProgInfo.aEsInfo[i].pEs->stElementCaDescriptor[j].ucRawLen;
						break;
					}
				}
			}
			#else // 이게 CI 및 CI+ 스펙에 맞다.
			for (j=0; j<ciProgInfo.aEsInfo[i].pEs->ulNumOfCaDescriptor; j++)
			{
				if (ausEsInfoLen[i] == 0)
					ausEsInfoLen[i]++; // ca_pmt_cmd_id
				ausEsInfoLen[i] += ciProgInfo.aEsInfo[i].pEs->stElementCaDescriptor[j].ucRawLen;
			}
			#endif
		}

		usCaPmtLen += ausEsInfoLen[i];
	}

#if defined(_Support_CA_PMT_in_Clear_) // hmkim : added / modified (이데토 캠에서 채널 변경시마다 Check Your Smartcard 나 This Channel is Scrambled 라는 MMI 가 계속 뜨는 문제가 있어서...)
#else
/* <TODO_CI> : 특정 캠에 대한 예외 처리가 필요하다면...
   Application Info 의 application_manufacturer / manufacturer_code / menu_string_length 및
   CA Info 의 CA_system_id 을 모두 체크하도록 하자.
   엉뚱한 캠에 영향을 주지 않도록... */
//	if (nCmd == CI_CPCI_NOT_SELECTED)
	if ((nCmd == CI_CPCI_NOT_SELECTED) && !((((pCam->usManufacturer >> 8) & 0x00ff) == eCA_SYS_ID_IRDETO) && (((pCam->ausCaSysId[0] >> 8) & 0x00ff) == eCA_SYS_ID_IRDETO)))
	{
		usCaPmtLen = 10;
		usProgInfoLen = 1;
	}
#endif

	ucLenFieldSize = local_CiMgr_GetLengthField(aucLenField, (unsigned short)(usCaPmtLen-3));
	usCaPmtLen += ucLenFieldSize;

	pucCaPmt = (HUINT8 *)OxCAS_Malloc(usCaPmtLen);
	if (pucCaPmt == NULL)
	{
		HxLOG_Error("pucCaPmt is null\n");
		return ERR_FAIL;
	}

	/*-------------------------------*/
	/*--- 이제부터 ca_pmt () 생성 ---*/
	/*-------------------------------*/

	usIdx = 0;

	/* ca_pmt_tag (24 bits) */
	pucCaPmt[usIdx++] = (HUINT8)((CI_CA_PMT >> 16) & 0xff);
	pucCaPmt[usIdx++] = (HUINT8)((CI_CA_PMT >> 8) & 0xff);
	pucCaPmt[usIdx++] = (HUINT8)(CI_CA_PMT & 0xff);

	/* length field */
	HxSTD_memcpy(&pucCaPmt[usIdx], aucLenField, ucLenFieldSize);
	usIdx += ucLenFieldSize;

	/* ca_pmt_list_managrment (8 bits) */
	pucCaPmt[usIdx++] = (HUINT8)nManage;

	/* program_number (16 bits) */
	pucCaPmt[usIdx++] = (HUINT8)((pPmtInfo->usServiceId >> 8) & 0xff);
	pucCaPmt[usIdx++] = (HUINT8)(pPmtInfo->usServiceId & 0xff);

	/* reserved (2 bits) + version_number (5 bits) + current_next_indicator (1 bit) */
	pucCaPmt[usIdx] = (HUINT8)((pPmtInfo->ucVerNum << 1) & 0x3e);
	pucCaPmt[usIdx++] |= (HUINT8)(pPmtInfo->bCurNextInd & 0x01);

	/* reserved (4 bits) + program_info_length (12 bits) */
	pucCaPmt[usIdx++] = (HUINT8)((usProgInfoLen >> 8) & 0x0f);
	pucCaPmt[usIdx++] = (HUINT8)(usProgInfoLen & 0xff);

#if defined(_Support_CA_PMT_in_Clear_) // hmkim : added / modified (이데토 캠에서 채널 변경시마다 Check Your Smartcard 나 This Channel is Scrambled 라는 MMI 가 계속 뜨는 문제가 있어서...)
#else
/* <TODO_CI> : 특정 캠에 대한 예외 처리가 필요하다면...
   Application Info 의 application_manufacturer / manufacturer_code / menu_string_length 및
   CA Info 의 CA_system_id 을 모두 체크하도록 하자.
   엉뚱한 캠에 영향을 주지 않도록... */
//	if (nCmd == CI_CPCI_NOT_SELECTED)
	if ((nCmd == CI_CPCI_NOT_SELECTED) && !((((pCam->usManufacturer >> 8) & 0x00ff) == eCA_SYS_ID_IRDETO) && (((pCam->ausCaSysId[0] >> 8) & 0x00ff) == eCA_SYS_ID_IRDETO)))
	{
		pucCaPmt[usIdx++] = (unsigned char)nCmd; // ca_pmt_cmd_id
	}
	else
	{
#endif

		// programme level
		if (usProgInfoLen != 0)
		{
			pucCaPmt[usIdx++] = (HUINT8)nCmd; // ca_pmt_cmd_id

			#if defined(_Contains_all_the_CA_descriptors_) // org

			// 이게 CI 및 CI+ 스펙에 맞다.
			for (i=0; i<pPmtInfo->ulNumOfProgramCaDescriptor; i++)
			{
				if (usCaPmtLen < (usIdx + pPmtInfo->stProgramCaDescriptor[i].ucRawLen))
				{
					OxCAS_Free(pucCaPmt);
					HxLOG_Error("\n");
					return ERR_FAIL;
				}

				HxSTD_memcpy(&pucCaPmt[usIdx], pPmtInfo->stProgramCaDescriptor[i].pucRaw, pPmtInfo->stProgramCaDescriptor[i].ucRawLen); // CA_descriptor
				usIdx += pPmtInfo->stProgramCaDescriptor[i].ucRawLen;
			}

			#else

			for (i=0; i<pPmtInfo->ulNumOfProgramCaDescriptor; i++)
			{
				for (j=0; j<pCam->usCaInfoCnt; j++)
				{
					if (pPmtInfo->stProgramCaDescriptor[i].usCaSysId == pCam->ausCaSysId[j])
					{
						if (usCaPmtLen < (usIdx + pPmtInfo->stProgramCaDescriptor[i].ucRawLen))
						{
							OxCAS_Free(pucCaPmt);
							HxLOG_Error("\n");
							return ERR_FAIL;
						}

						HxSTD_memcpy(&pucCaPmt[usIdx], pPmtInfo->stProgramCaDescriptor[i].pucRaw, pPmtInfo->stProgramCaDescriptor[i].ucRawLen); // CA_descriptor
						usIdx += pPmtInfo->stProgramCaDescriptor[i].ucRawLen;
						break;
					}
				}
			}
			#endif
		}

		// ES level
		for (i=0; i<ciProgInfo.ucNumEs; i++)
		{
			aEsInfo = ciProgInfo.aEsInfo;

			/* stream_type (8 bits) */
			pucCaPmt[usIdx++] = aEsInfo[i].pEs->ucStreamType;

			/* reserved (3 bits) + elementary_PID (13 bits) */
			pucCaPmt[usIdx++] = (HUINT8)((aEsInfo[i].pEs->usPid >> 8) & 0x1f);
			pucCaPmt[usIdx++] = (HUINT8)(aEsInfo[i].pEs->usPid & 0xff);

			/* reserved (4 bits) + ES_info_length (12 bits) */
			pucCaPmt[usIdx++] = (HUINT8)((ausEsInfoLen[i] >> 8) & 0x0f);
			pucCaPmt[usIdx++] = (HUINT8)(ausEsInfoLen[i] & 0xff);

			if (ausEsInfoLen[i] != 0)
			{
				pucCaPmt[usIdx++] = (HUINT8)nCmd; // ca_pmt_cmd_id

				#if defined(_Contains_all_the_CA_descriptors_) // org
				// 이게 CI 및 CI+ 스펙에 맞다.
				for (j=0; j<aEsInfo[i].pEs->ulNumOfCaDescriptor; j++)
				{
					if (usCaPmtLen < (usIdx+ aEsInfo[i].pEs->stElementCaDescriptor[j].ucRawLen))
					{
						OxCAS_Free(pucCaPmt);
						HxLOG_Error("\n");
						return ERR_FAIL;
					}

					HxSTD_memcpy(&pucCaPmt[usIdx], aEsInfo[i].pEs->stElementCaDescriptor[j].pucRaw, aEsInfo[i].pEs->stElementCaDescriptor[j].ucRawLen); // CA_descriptor
					usIdx += aEsInfo[i].pEs->stElementCaDescriptor[j].ucRawLen;
				}
				#else
				for (j=0; j<aEsInfo[i].pEs->ulNumOfCaDescriptor; j++)
				{
					for (k=0; k<pCam->usCaInfoCnt; k++)
					{
						if (aEsInfo[i].pEs->stElementCaDescriptor[j].usCaSysId == pCam->ausCaSysId[k])
						{
							if (usCaPmtLen < (usIdx + aEsInfo[i].pEs->stElementCaDescriptor[j].ucRawLen))
							{
								OxCAS_Free(pucCaPmt);
								HxLOG_Error("\n");
								return ERR_FAIL;
							}
#ifdef CONFIG_DEBUG
{
							HUINT32 iidx =0;
							HUINT32 ucRawLen = aEsInfo[i].pEs->stElementCaDescriptor[j].ucRawLen;

							HxLOG_Info("2 received pucRaw data[%d]: \n", ucRawLen);
							for ( iidx = 0; iidx < ucRawLen; iidx++)
							{
								if(pPmtInfo->stElementInfo[i].stElementCaDescriptor[0].pucRaw)
								{
									HxLOG_Print("%02X ", aEsInfo[i].pEs->stElementCaDescriptor[j].pucRaw[iidx]);
								}

								if (iidx % 16 == (16-1))	  {
									HxLOG_Print("\n");
								}
							}
							HxLOG_Print("\n");
}
#endif


							HxSTD_memcpy(&pucCaPmt[usIdx], aEsInfo[i].pEs->stElementCaDescriptor[j].pucRaw, aEsInfo[i].pEs->stElementCaDescriptor[j].ucRawLen); // CA_descriptor
							usIdx += aEsInfo[i].pEs->stElementCaDescriptor[j].ucRawLen;
							break;
						}
					}
				}
				#endif
			}
		}

		#if defined (CONFIG_MW_CI_PLUS)

		/* 초기화 */
		for(i=0; i<CAS_MAX_NUM_OF_ES; i++)
		{
			pCam->PidInfoForCaPmt[i].usPid = PID_NULL;
			pCam->PidInfoForCaPmt[i].esType = eEsType_None;
		}

		pCam->ulNumOfPidForCaPmt = ciProgInfo.ucNumEs;
		for(i=0; i<pCam->ulNumOfPidForCaPmt; i++)
		{
			pCam->PidInfoForCaPmt[i].usPid = ciProgInfo.aEsInfo[i].usEsPid;
			pCam->PidInfoForCaPmt[i].esType = ciProgInfo.aEsInfo[i].eEsType;
		}

		#endif

		if (usCaPmtLen != usIdx)
		{
			if (pucCaPmt != NULL)
				OxCAS_Free(pucCaPmt);
			HxLOG_Error("\n");
			return ERR_FAIL;
		}
#if defined(_Support_CA_PMT_in_Clear_) // hmkim : added.
#else
	}
#endif

#ifdef CONFIG_DEBUG
{
	HUINT32 iidx =0;
	HUINT32 ucRawLen = usCaPmtLen;

	HxLOG_Info("3 received pucRaw data[%d]: \n", ucRawLen);
	for ( iidx = 0; iidx < ucRawLen; iidx++)
	{
		HxLOG_Print("%02X ", pucCaPmt[iidx]);

		if (iidx % 16 == (16-1))	  {
			HxLOG_Print("\n");
		}
	}
	HxLOG_Print("\n");
}
#endif

	pCaPmt->pucRaw = pucCaPmt;
	pCaPmt->usRawLen = usCaPmtLen;

	return ERR_OK;
}

/* bIsUpdate : stProgInfo 가 갱신되어 update 하는 경우 */
/* bIsRetry  : stProgInfo 가 갱신되지 않은 상태에서 retry 만 하는 경우 */
/*      주의 : bIsUpdate 와 bIsRetry 가 같이 셋팅되는 경우는 없어야 한다 */
static HERROR local_CiMgr_StartDescramble(CI_Context_t *pCiContext, CI_SLOT_ID usSlotId, HBOOL bIsUpdate, HBOOL bIsRetry) // TODO : Update/Retry....내부적으로 하는게 맞지 않나.
{
	HBOOL						bNeedCaPmtSend = FALSE;
	HERROR						hErr = ERR_FAIL;
	CI_CamInst_t					*pCam = NULL;
	CI_CA_PMT_LIST_MANAGEMENT	eCplmCmd;

	HxLOG_Info("usSlotId = %d, bIsUpdate = %d, bIsRetry = %d\n", usSlotId, bIsUpdate, bIsRetry);

	if (pCiContext == NULL)
	{
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	pCam = pCiContext->pCam;
	if(pCam == NULL)
	{
		/* cam instance를 context에 할당 받지 못한 상태. */
		HxLOG_Error("sequence error. \n");
		return ERR_OK;
	}

	if (pCam->usSlotId != usSlotId)
	{
		HxLOG_Error("pCam->usSlotId = %x, usSlotId = %x\n", pCam->usSlotId, usSlotId);
		return ERR_FAIL;
	}

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	if (CARM_CI_CheckModule(usSlotId) == eCAS_DEV_CI_EXTRACTED)
	{
		HxLOG_Info("cam is extracted\n");
		return ERR_OK;
	}

	if (pCam->usCaInfoCnt == 0)
	{
		HxLOG_Error("CaInfoCnt (%d)\n", pCam->usCaInfoCnt);
		return ERR_FAIL;
	}

	if (pCiContext->stProgInfo.pPmtInfo == NULL)
	{
		HxLOG_Error("pmt info is null. \n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CI_PLUS)
	HxLOG_Info("pCiContext->eChType:[%d]\n", pCiContext->eChType);
	if (pCiContext->eChType == eCasChannel_FTA) // FTA 채널인 경우 이하 처리는 필요 없음.
	{
		CI_SetPlusMode(usSlotId, CI_PLUS_FREE_MODE);

		#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
		local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pCiContext->hAction), -1);
		#else
		svc_cas_DevCiSetTsPath(usSlotId, eCAS_DEV_CI_PASSTHROUGH);
		#endif

		return ERR_OK;
	}

#if 0
	/* ci plus descrambler setting. */
	local_CiMgr_ProcDscSetForCiPlus(pCam, pCiContext->ulDemuxId, bIsUpdate, bIsRetry, 1);
#endif

	if ( 		(pCiContext->eActType == eCAS_ActionType_Live)
		||	(pCiContext->eActType == eCAS_ActionType_Playback)
		||	(pCiContext->eActType == eCAS_ActionType_Tsr_Rec)
		||	(pCiContext->eActType == eCAS_ActionType_Tsr_Play))
	{
		svc_cas_MgrSubGetSiTableState(pCiContext->hAction, eSI_TableType_SDT_ACT, &pCiContext->eSdtTblState);

		#if defined(CONFIG_DEBUG)
		HxLOG_Info("NumOfDsc (%d), SdtTblState (%s), bSvcShunned (%d)\n",
			pCam->ulNumOfDsc[pCiContext->ulDemuxId], local_CiMgr_MakeStrSiTblState(pCiContext->eSdtTblState), pCiContext->bSvcShunned);
		#endif

#if 1	// TODO: 아래 Leagcy CI Shunning 동작 작업
		if((pCiContext->eSdtTblState == eCASSI_Table_Received || pCiContext->eSdtTblState == eCASSI_Table_Timeout || pCiContext->eSdtTblState == eCASSI_Table_Running)
			&& (pCiContext->bSvcShunned == FALSE))
		{
			HxLOG_Info("ci set ts path CARM_CI_PASSTHROUGH\n");

			#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
			local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pCiContext->hAction), pCiContext->pCam->usSlotId);
			#else
			svc_cas_DevCiSetTsPath(usSlotId, eCAS_DEV_CI_PASSTHROUGH);
			#endif
		}
#else	// Legacy CI CIv1 Shunning 관련하여 인증 테스트를 통과하기 위해서는 아래 루틴을 타야 한다. (그런데, 이 루틴을 타면 Scrambled Rec Contents Playback 시 Set path가 제대로 타지 않아 동작이 안되어 일단 기능 동작 우선으로 수정)
		if(pCiContext->eSdtTblState == eCASSI_Table_Received || pCiContext->eSdtTblState == eCASSI_Table_Running)
		{
			CAS_CiDscInfo_t *pDscInfo = NULL;

			// CAM이 제거된 상태로 zapping후 SDT를 받은 상태일 경우 local_CiMgr_ProcSdtChanged()에서 shun condition이 체크되지 않는다.
			// 따라서, descrambling 전에 다시한번 체크해 봐야 한다.
			if(local_CiMgr_GetCiDescInfo(pCiContext, &pDscInfo) != ERR_OK)
			{
				HxLOG_Error("get CI description info failed\n");
			}
			else
			{
				if(local_CiMgr_ProcCheckShunCondition(pCiContext->pCam->usSlotId, pDscInfo, &pCiContext->bSvcShunned) != ERR_OK)
				{
					HxLOG_Error("local_CiMgr_ProcCheckShunCondition() error\n");
				}
				else
				{
					if (pCiContext->bSvcShunned == TRUE)
					{
						HxLOG_Error("Shunning enable, Set Bypass\n");

						CI_SetPlusMode(pCiContext->pCam->usSlotId, CI_PLUS_PROTECTED_MODE);

				#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
						local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pCiContext->hAction), -1);
				#else
						svc_cas_DevCiSetTsPath(usSlotId, eCAS_DEV_CI_BYPASS);
				#endif
					}
					else
					{
						CI_SetPlusMode(pCiContext->pCam->usSlotId, CI_PLUS_FREE_MODE);

					#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
						local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pCiContext->hAction), pCiContext->pCam->usSlotId);
					#else
						svc_cas_DevCiSetTsPath(pCiContext->pCam->usSlotId, eCAS_DEV_CI_PASSTHROUGH);
					#endif
					}
				}
			}
		}
#endif
		else
		{
			/**
			  * 	SDT 7 day caching
			**/
			HBOOL				bNeedShun = FALSE;
			UNIXTIME			ulCurrentTime = 0;
			SvcCas_DbSvcInfo_t 	stSvcInfo;
			CAS_CiDscInfo_t		stCachedDscInfo;

			// 아직 SDT ACT를 받지 못함. DB를 뒤져보자
			hErr = svc_cas_SubSvcDbGetSvcInfoByServiceHandle(pCiContext->hSvc, &stSvcInfo);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("svc_cas_SubSvcDbGetSvcInfoByServiceHandle() Fail !!\n");
			}
			else
			{
				// 현재 시간을 가져오고
				hErr = (HERROR)VK_CLOCK_GetTime((unsigned long*)&ulCurrentTime);
				if((hErr != VK_OK) || (ulCurrentTime <= UNIX_FIRST_DAY))
				{
					HxLOG_Info("VK_CLOCK_GetTime() Fail !! Set unix first day... \n");
				}
				else
				{
					// time stamp의 시간이 valid 하다면
					if (stSvcInfo.ulTimeStamp + SECONDS_PER_WEEK >= ulCurrentTime )
					{
						// 조건 검사에 필요한 정보들을 채워서
						HxSTD_memset(&stCachedDscInfo, 0x00, sizeof(CAS_CiDscInfo_t));

						stCachedDscInfo.usSvcId = pCiContext->usSvcId;
						if (stSvcInfo.ulUserFlag1 & FreeCiMode)
						{
							stCachedDscInfo.bFreeCiFlag = TRUE;
							stCachedDscInfo.bCiDscFlag = TRUE;
						}
						else
						{
							stCachedDscInfo.bFreeCiFlag = FALSE;
							stCachedDscInfo.bCiDscFlag = FALSE;
						}

						stCachedDscInfo.ucNumOfBrandId = stSvcInfo.ucNumOfEntries;
						if (stCachedDscInfo.ucNumOfBrandId == 0)
						{
							stCachedDscInfo.bMatchBrandFlag = FALSE;
						}
						else
						{
							stCachedDscInfo.bMatchBrandFlag = TRUE;
							HxSTD_memcpy(stCachedDscInfo.usCiBrandId, stSvcInfo.ausCamBrandId, sizeof(stCachedDscInfo.usCiBrandId));
						}

						HxLOG_Info("Cached SDT Data :: usSvcId(0x%X) bFreeCiFlag(%d) bCiDscFlag(%d) #brandID(%d) matchFlag(%d) \n",
												stCachedDscInfo.usSvcId, stCachedDscInfo.bFreeCiFlag,  stCachedDscInfo.bCiDscFlag, stCachedDscInfo.ucNumOfBrandId, stCachedDscInfo.bMatchBrandFlag );

						// 다시 shun condition을 검사해 보자.
						hErr = local_CiMgr_ProcCheckShunCondition(usSlotId, &stCachedDscInfo, &bNeedShun);
						if (hErr != ERR_OK)
						{
							HxLOG_Error("local_cimgr_ProcCheckShunCondition() Fail !!\n");
							return ERR_FAIL;
						}

						// shun condition이 FALSE라면 풀자~
						if (bNeedShun != TRUE)
						{
							#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
							local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pCiContext->hAction), pCiContext->pCam->usSlotId);
							#else
							svc_cas_DevCiSetTsPath(usSlotId, eCAS_DEV_CI_PASSTHROUGH);
							#endif

							CI_SetPlusMode(usSlotId, CI_PLUS_FREE_MODE);

							#if defined(CONFIG_DEBUG)
							HxLOG_Info(" [CI+] Use cached SDT data. curTime(0x%X) savedTimeStamp(%d) \n", ulCurrentTime, stSvcInfo.ulTimeStamp);
							#endif
						}
					}
					else
					{
						HxLOG_Info("Invalid Timestamp. Now(0x%X) DB(0x%X) \n", ulCurrentTime, stSvcInfo.ulTimeStamp);

						// TODO: 아래 코드가 shunning 시나리오에 위배 되지 않는지 검증 필요 (CI+ Test Tool 통과만 하면 될듯)
						// SDT가 없는 서비스의 경우 service change만 하면 SDT가 업데이트 되지 않아 Set Path를 타지 않음.
						// 이러한 케이스를 위해 Shunning이 걸려있지 않고, SDT가 DB에 저장된 경우가 없다면(ulTimeStamp가 SDT가 저장된 시간을 들고 있음.)
						// SetPath 시나리오를 타도록 한다.

						if((pCiContext->bSvcShunned == FALSE) && (stSvcInfo.ulTimeStamp == 0))
						{
							HxLOG_Info("ci set ts path eCAS_DEV_CI_PASSTHROUGH\n");

							#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
							local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pCiContext->hAction), pCiContext->pCam->usSlotId);
							#else
							svc_cas_DevCiSetTsPath(usSlotId, eCAS_DEV_CI_PASSTHROUGH);
							#endif

							CI_SetPlusMode(usSlotId, CI_PLUS_FREE_MODE);
						}

					}
				}
			}
		}
	}
	else
	{
		#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
		local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pCiContext->hAction), pCiContext->pCam->usSlotId);
		#else
		svc_cas_DevCiSetTsPath(usSlotId, eCAS_DEV_CI_PASSTHROUGH);
		#endif
	}
#else
	HxLOG_Info("svc_cas_DevCiSetTsPath()  usSlotId = %d, eCAS_DEV_CI_PASSTHROUGH\n", usSlotId);
	svc_cas_DevCiSetTsPath(usSlotId, eCAS_DEV_CI_PASSTHROUGH);
#endif

local_CiMgr_CheckCaPmtSending(pCam, pCiContext, &bNeedCaPmtSend); // bIsRetry 가 true 인 경우도 불려야 한다...?

	local_CiMgr_AddActionToCamInstance(pCam, pCiContext->hAction);

	#ifdef DEBUG_ACT_LIST
	local_CiMgr_PrintActionList(pCam);
	#endif

	HxLOG_Info("bNeedCaPmtSend (%d), bIsRetry (%d) \n", bNeedCaPmtSend, bIsRetry);

	if(bIsRetry == 1)
		bNeedCaPmtSend = TRUE;

	if(bNeedCaPmtSend == TRUE)
	{
		#if defined(CONFIG_MW_CI_PLUS)
		CI_SetUriDefault(usSlotId, TRUE);
		#endif

		if (bIsUpdate == 0)
		{
			eCplmCmd = CI_CPLM_ONLY;
		}
		else
		{
			eCplmCmd = local_CiMgr_GetCplmCmd(pCiContext->hAction, pCam);
		}

		HxLOG_Info("eCplmCmd = %d\n", eCplmCmd);

		if (pCam->caPmtOkDsc.pucRaw != NULL)
		{
			HxLOG_Info("caPmtOkDsc is not null, make free previous caPmtOkDsc\n");
			OxCAS_Free(pCam->caPmtOkDsc.pucRaw);
			pCam->caPmtOkDsc.pucRaw = NULL;
			pCam->caPmtOkDsc.usRawLen = 0;
		}

		hErr = local_CiMgr_GetCaPmt(pCam, pCiContext, eCplmCmd, CI_CPCI_OK_DSC, &pCam->caPmtOkDsc);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("ERROR!! \n");
			return ERR_FAIL;
		}

		if (pCam->caPmtOkDsc.pucRaw != NULL)
		{
			#if defined(CONFIG_MW_CI_PLUS)
			if(bIsUpdate == FALSE)
			{
				if(local_CiMgr_CheckClearCaPmtCondition(pCiContext, TRUE) == TRUE)
				{
					local_CiMgr_SendClearCaPmt(usSlotId, pCiContext);
				}

				if(pCiContext->hSvc != HANDLE_NULL)
				{
					pCiContext->pCam->hLastSvc = pCiContext->hSvc;
				}
			}
			#endif

			HxLOG_Info("CI_SendCaPmt() \n");

			if (CI_SendCaPmt(usSlotId, pCam->caPmtOkDsc.pucRaw, pCam->caPmtOkDsc.usRawLen) != ERR_OK)
			{
				HxLOG_Error("ERROR!! \n");
//				return ERR_FAIL;
			}

			pCam->bClearCaPmt = FALSE;
		}

	  	HxLOG_Info("CI_SendCaPmt : slot[%d], pucRaw[0x%x], len[%d]\n", usSlotId, (HUINT32)pCam->caPmtOkDsc.pucRaw, pCam->caPmtOkDsc.usRawLen);
	}

	/* CA PMT 작성 완료. */
	pCiContext->eState = eCiMgrState_StartCaPmt;

#if defined(CONFIG_MW_CI_PLUS)
	/* ci plus descrambler setting. */
	local_CiMgr_ProcDscSetForCiPlus(pCam, pCiContext->ulDemuxId, bIsUpdate, bIsRetry, 1);
#endif

#if defined(CONFIG_SUPPORT_RETRY_CAPMT)
	if (pCiContext->eActType == eCAS_ActionType_Live && pCiContext->ulRetryTimerId == INVALID_TIMER_ID)
	{
		HxLOG_Print("ulRetryTimerId is (%lu)\n", pCiContext->ulRetryTimerId);
		local_CiMgr_StartRetrySvc(pCiContext);
	}
#endif

#if defined(CONFIG_MW_CI_CHECK_AV)
	local_CiMgr_StartCheckAv(pCiContext);
#endif

	return ERR_OK;
}

static HERROR local_CiMgr_StopDescramble(CI_Context_t *pstCiContext, CI_SLOT_ID usSlotId)
{
	HUINT32			i;
	HERROR			hErr = ERR_OK;
	CI_CamInst_t		*pstCam = NULL;

	if (pstCiContext == NULL)
	{
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	HxLOG_Info("hAction (0x%08x), usSlotId (%d)\n", pstCiContext->hAction, usSlotId);

	pstCam = pstCiContext->pCam;
	if(pstCam == NULL)
	{
		HxLOG_Error("Cam instance is null. (%d)\n", usSlotId);
		return ERR_OK;
	}

	if (pstCam->usSlotId != usSlotId)
	{
		HxLOG_Error("pstCam->usSlotId = %x, usSlotId = %x\n", pstCam->usSlotId, usSlotId);
		return ERR_FAIL;
	}

	#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
	#else
	svc_cas_DevCiSetTsPath(usSlotId, eCAS_DEV_CI_BYPASS);
	#endif

	/*  Cam 이 제거 된 경우....*/
	if (pstCam->usCaInfoCnt == 0)
	{
		HxLOG_Print("CAM is removed...\n");
		return ERR_OK;
	}

#if defined(CONFIG_MW_CI_PLUS)
	#if defined(CONFIG_DEBUG)
	HxLOG_Info("slot %d's NumOfDsc = %d, KeyInfoAvailable = %d\n",
		usSlotId, pstCam->ulNumOfDsc[pstCiContext->ulDemuxId], pstCam->bKeyInfoAvailable);
	HxLOG_Info("hAction = 0x%x, hSvc = 0x%x, action type = %s\n",
		pstCiContext->hAction, pstCiContext->hSvc, local_CiMgr_MakeStrActionType(pstCiContext->eActType));
	#endif

	if (pstCiContext->ulDemuxId != CAS_RESOURCE_NONE)
	{
		for (i = 0; i < pstCam->ulNumOfDsc[pstCiContext->ulDemuxId]; i++)
		{
			hErr = svc_cas_DevDscClose(pstCam->stDscInfo[pstCiContext->ulDemuxId][i].ulDscId);
			if(hErr != ERR_OK)
			{
				HxLOG_Critical("Critical Error!!!dsc id = %d\n", pstCam->stDscInfo[pstCiContext->ulDemuxId][i].ulDscId);
			}

			pstCam->stDscInfo[pstCiContext->ulDemuxId][i].ulDscId = CAS_RESOURCE_NONE;
			pstCam->stDscInfo[pstCiContext->ulDemuxId][i].usPid = PID_NULL;
		}
		pstCam->ulNumOfDsc[pstCiContext->ulDemuxId] = 0; // pCam->bKeyInfoAvailable 및 키 정보 값들은 리셋하지 않도록 한다.
	}
	else
	{
		HxLOG_Critical("\n\n");
	}

	pstCiContext->bSvcShunned = FALSE;

	CI_SetPlusMode(usSlotId, CI_PLUS_FREE_MODE);
#endif

	/* ca pmt raw 와 sync */
	/* 초기화 */
	for(i=0; i<CAS_MAX_NUM_OF_ES; i++)
	{
		pstCam->PidInfoForCaPmt[i].usPid = PID_NULL;
		pstCam->PidInfoForCaPmt[i].esType = eEsType_None;
	}
	pstCam->ulNumOfPidForCaPmt = 0;

	// cam에 포함된 service 관련 data를 없앰.
	if (pstCam->caPmtOkDsc.pucRaw != NULL)
	{
		OxCAS_Free(pstCam->caPmtOkDsc.pucRaw);
		pstCam->caPmtOkDsc.pucRaw = NULL;
		pstCam->caPmtOkDsc.usRawLen = 0;
	}

	pstCiContext->bSendCaPmt = FALSE;

	return hErr;
}

/* Not use anymore. Read the SVN log.
static void local_CiMgr_CallbackDelayedAddCaInst(unsigned long ulTimerId, void *pvParam)
{
	CI_SLOT_ID 					usSlotId;
	CI_UPDATE_CA_INST_PARAM		updateCaInst;
	HERROR nResult = ERR_FAIL;

	ulTimerId = 0;

	usSlotId = *((CI_SLOT_ID *)pvParam);

	HxLOG_Info("SlotId[%d]...\n", usSlotId);

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return;
	}

	updateCaInst.usSlotId = usSlotId;

	nResult = local_CiMgr_SendEvent(CI_ADD_DELAYED_CA_INST, (void *)&updateCaInst, (unsigned long)sizeof(CI_UPDATE_CA_INST_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

	return;
}
*/

static HINT32 local_CiMgr_ProcUpdateCasState(CI_UPDATE_CAS_STATE_PARAM *pUpdateCasSt)
{
	if (pUpdateCasSt == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	HxLOG_Info("SlotId[0x%x], state[%d]\n", pUpdateCasSt->usSlotId, pUpdateCasSt->eCasState);

	if (pUpdateCasSt->usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", pUpdateCasSt->usSlotId);
		return ERR_FAIL;
	}

	switch (pUpdateCasSt->eCasState) /* <TODO_CI/TODO_CI_PLUS> : 각 case 별도 구현 필요 */
	{
	case CI_CAS_STATE_CA_PMT_REPLIED:
		break;

	case CI_CAS_STATE_CA_PVR_PMT_REPLIED:
		break;

	case CI_CAS_STATE_CA_PVR_CAT_REPLIED:
		break;

	case CI_CAS_STATE_CA_PVR_EMM_REPLIED:
		break;

	case CI_CAS_STATE_CA_PVR_ECM_REPLIED:
		break;

	case CI_CAS_STATE_CA_PVR_PINCODE_REPLIED:
		break;

	case CI_CAS_STATE_ERROR:
		HxLOG_Error("CI_CAS_STATE_ERROR\n");
		break;

	default:
		HxLOG_Error("%d not supported\n", pUpdateCasSt->eCasState);
		break;
	}

	return ERR_OK;
}

////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_CI_PLUS_DESC___
////////////////////////////////////////////////////////////

#if defined(CONFIG_MW_CI_PLUS)

static HBOOL local_CiMgr_FilterCiDscList(void *pvItem, void *pvFilter)
{
	CAS_CiDscInfo_t *pData = (CAS_CiDscInfo_t *)pvItem;
	HUINT16 usSvcId = *(HUINT16*)pvFilter;

	if(pData->usSvcId == usSvcId)
	{
		return TRUE;
	}

	return FALSE;
}

static HERROR local_CiMgr_MakeCiDrmInfo(eDxCopyrightControl_CiPlusDrmType_e eCiPlusDrmType, void *pData, DxCopyrightControl_t **ppDrmInfo, eDxCopyrightControl_CiPlusDrmWriteInfo_e eCiPlusDrmWriteInfo)
{
	HxLOG_Info("Make CI+ DRM information... \n");

	if (pData == NULL)
	{
		HxLOG_Error("Received pData(DRM data) is NULL.\n");
		*ppDrmInfo = NULL;
		return ERR_FAIL;
	}

	if (s_eCiDrmWriteState[s_ucCiDrmInfoVer] == eDxCopyrightControl_CiPlusDrmWriteInfo_Writing)
	{
		// 아래 에러가 뜰 가능성은 거의 없지만 만약 나오면 DrmData 저장하는 배열을 늘리거나, linked list로 관리하도록!
		HxLOG_Critical("\nThe DRM DATA in buffer is not consumed!!\n");
//		return ERR_FAIL;
	}
	else
	{
		HxSTD_memset(&s_astCiDrmInfo[s_ucCiDrmInfoVer], 0, sizeof(DxCopyrightControl_t));
	}

	s_astCiDrmInfo[s_ucCiDrmInfoVer].stCasCcInfo.eCasCcType = eDxCopyrightControl_CasCcType_CiPlus;
	s_astCiDrmInfo[s_ucCiDrmInfoVer].stCasCcInfo.info.stCiPlusDrmInfo.ucCiPlusDrmInfoVer = s_ucCiDrmInfoVer;

	switch(eCiPlusDrmType)
	{
		case eDxCopyrightControl_CiPlusDrmType_Uri:
		{
			DxCopyrightControl_CiPlusUriInfo_t	*pstCiPlusUriInfo = NULL;
			pstCiPlusUriInfo = (DxCopyrightControl_CiPlusUriInfo_t *)pData;

			s_astCiDrmInfo[s_ucCiDrmInfoVer].stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType = eDxCopyrightControl_CiPlusDrmType_Uri;
			s_astCiDrmInfo[s_ucCiDrmInfoVer].stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusUriInfo = *pstCiPlusUriInfo;
			s_astCiDrmInfo[s_ucCiDrmInfoVer].bCasCc = TRUE;

			break;
		}
		case eDxCopyrightControl_CiPlusDrmType_RecCtrl:
		{
			DxCopyrightControl_CiPlusCcRecCtrl_t	*pstCiPlusRecCtrl = NULL;
			pstCiPlusRecCtrl = (DxCopyrightControl_CiPlusCcRecCtrl_t *)pData;

			s_astCiDrmInfo[s_ucCiDrmInfoVer].stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType = eDxCopyrightControl_CiPlusDrmType_RecCtrl;
			s_astCiDrmInfo[s_ucCiDrmInfoVer].stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusRecCtrl = *pstCiPlusRecCtrl;

			break;
		}
		case eDxCopyrightControl_CiPlusDrmType_License:
		{
			DxCopyrightControl_CiPlusLicenseInfo_t		*pstCiPlusLicenseInfo = NULL;
			HUINT16					usLicenseLeng;

			pstCiPlusLicenseInfo = (DxCopyrightControl_CiPlusLicenseInfo_t*)pData;
			usLicenseLeng = pstCiPlusLicenseInfo->usLicenseLength;

			if (usLicenseLeng == 0)
			{
				HxLOG_Error("Make License Info Error!! usLicenseLeng[%x].\n", pstCiPlusLicenseInfo->usLicenseLength);
				return ERR_FAIL;
			}

			s_astCiDrmInfo[s_ucCiDrmInfoVer].stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType = eDxCopyrightControl_CiPlusDrmType_License;
			s_astCiDrmInfo[s_ucCiDrmInfoVer].stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength = usLicenseLeng;
			s_astCiDrmInfo[s_ucCiDrmInfoVer].stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.usProgramNum = pstCiPlusLicenseInfo->usProgramNum;

			HxSTD_memcpy(s_astCiDrmInfo[s_ucCiDrmInfoVer].stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucLicenseData, pstCiPlusLicenseInfo->aucLicenseData, usLicenseLeng);
			HxSTD_memcpy(s_astCiDrmInfo[s_ucCiDrmInfoVer].stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucBindedCamId, pstCiPlusLicenseInfo->aucBindedCamId, 8);

			break;
		}
		case eDxCopyrightControl_CiPlusDrmType_ParentalCtrl:
		{
			DxCopyrightControl_CiPlusParentalCtrlInfo_t	*pstCiPlusParentalCtrlInfo = NULL;
			pstCiPlusParentalCtrlInfo = (DxCopyrightControl_CiPlusParentalCtrlInfo_t *)pData;

			// 배열 잘 복사해 가는지 Test 해볼 것!
			s_astCiDrmInfo[s_ucCiDrmInfoVer].stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType = eDxCopyrightControl_CiPlusDrmType_ParentalCtrl;
			s_astCiDrmInfo[s_ucCiDrmInfoVer].stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusParentalCtrlInfo = *pstCiPlusParentalCtrlInfo;

			break;
		}
		default:	return ERR_FAIL;
	}

	s_eCiDrmWriteState[s_ucCiDrmInfoVer] = eCiPlusDrmWriteInfo;

	HxLOG_Info("CI+ DRM information ver[%02x] is assigned with state[%02x].\n", s_ucCiDrmInfoVer, eCiPlusDrmWriteInfo);
	*ppDrmInfo = &s_astCiDrmInfo[s_ucCiDrmInfoVer];

	++s_ucCiDrmInfoVer;
	if(s_ucCiDrmInfoVer == CI_MGR_MAX_DRM_INFO)
	{
		s_ucCiDrmInfoVer = 0;
	}

	return ERR_OK;
}

static void local_CiMgr_SendCiDrmInfo(Handle_t hAction, Handle_t hCasDevice, eDxCopyrightControl_CiPlusDrmType_e eCiPlusDrmType, void *pData)
{
	HERROR		hErr;
	DxCopyrightControl_t	*pstDrmInfo = NULL;

	if(pData != NULL)
	{
		hErr = local_CiMgr_MakeCiDrmInfo(eCiPlusDrmType, pData, &pstDrmInfo, eDxCopyrightControl_CiPlusDrmWriteInfo_None);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("local_CiMgr_MakeCiDrmInfo() failed\n");
			return;
		}
		svc_cas_MgrSubUpdateDrmInfo(hAction, hCasDevice, (void *)pstDrmInfo, eDxCopyrightControl_CasCcType_CiPlus, pstDrmInfo->stCasCcInfo.info.stCiPlusDrmInfo.ucCiPlusDrmInfoVer);
		HxLOG_Info(" DRM Type[%02x]\n", pstDrmInfo->stCasCcInfo.eCasCcType);
	}
	else
	{
		svc_cas_MgrSubUpdateDrmInfo(hAction, hCasDevice, (void *)pstDrmInfo, eDxCopyrightControl_CasCcType_CiPlus, 0);
	}
}

static HERROR local_CiMgr_CheckUriControl(CI_Context_t *pCiContext, CI_CamInst_t *pCam)
{
	if (s_abUriSent[pCam->usSlotId] == TRUE)
	{
		local_CiMgr_SendCiDrmInfo(pCiContext->hAction, pCam->hCasDevice, eDxCopyrightControl_CiPlusDrmType_Uri, (void *)&s_stUriInfo[pCam->usSlotId]);
	}

	return ERR_OK;
}

static CAS_SdtStatus_t *local_CiMgr_GetSdtSlot(HUINT16 usUniqueId)
{
	HUINT32 i;

	for(i=0; i<CAS_MAX_NUM_OF_TS; i++)
	{
		if(s_stSdtStatus[i].bUsed == CAS_USED)
		{
			if(s_stSdtStatus[i].usUniqueId == usUniqueId)
			{
				return &s_stSdtStatus[i];
			}
		}
	}

	return NULL;
}

static HERROR local_CiMgr_GetCiDescInfo(CI_Context_t *pCiContext, CAS_CiDscInfo_t **pDscInfo)
{
	CAS_SdtStatus_t *pSdt = NULL;
	POINTER_LIST_T *pList = NULL;

	*pDscInfo = NULL;

	if(pCiContext == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	HxLOG_Info("service handle(0x%x), UniqueId (%x), pDscInfo(%x)\n", pCiContext->hSvc, pCiContext->usUniqueId, (HUINT32)pDscInfo);

	pSdt = local_CiMgr_GetSdtSlot(pCiContext->usUniqueId);
	if (pSdt == (CAS_SdtStatus_t *)NULL)
	{
		HxLOG_Error("UniqueId (0x%x)\n", pCiContext->usUniqueId);
		return ERR_FAIL;
	}

	if(pSdt->bUsed == CAS_NOT_USED)
	{
		HxLOG_Error("pSdt->bUsed (%d) \n", pSdt->bUsed);
		return ERR_FAIL;
	}

	#if defined(CONFIG_DEBUG)
	HxLOG_Info("(%s) \n", local_CiMgr_MakeStrSiTblState(pSdt->ulTblState) );
	#endif

	pCiContext->eSdtTblState = pSdt->ulTblState;

	if(pSdt->ulTblState == eCASSI_Table_Skipped
		|| pSdt->ulTblState == eCASSI_Table_Waiting
		|| pSdt->ulTblState == eCASSI_Table_Timeout
		|| pSdt->ulTblState == eCASSI_Table_Fail
		|| pSdt->ulSvcNum == 0)
	{
		HxLOG_Error("pSdt->ulSvcNum (%d)\n", pSdt->ulSvcNum);
		return ERR_FAIL;
	}

	pList = UTIL_LINKEDLIST_IsListItemInPointerList(pSdt->pstList, NULL, NULL, local_CiMgr_FilterCiDscList, (void *)&pCiContext->usSvcId);
	if(pList == NULL)
	{
		HxLOG_Error("service_id (%x) \n", pCiContext->usSvcId);
		*pDscInfo = NULL;
		return ERR_FAIL;
	}

	*pDscInfo = pList->pvContents;

	return ERR_OK;
}

/**
  *		CI+ CAM, CI+ service에서 shunning condition을 check하는 함수
  *
  *		불리는 곳은 아래와 같다.
  *		1. start descramble시 (normal하게 1회, sdt를 아직 못 받은 경우엔 db에 cached된 data로 1회)
  *		2. sdt update시
  *		3. cc resource open시 (CAS, CC 순서로 열리는 Neotion CAM 처리용)
**/
static HERROR local_CiMgr_ProcCheckShunCondition(CI_SLOT_ID usSlotId, CAS_CiDscInfo_t *pstCiDsc, HBOOL *bIsNeedShun)
{
	HBOOL bIsCiPlusCamWithValidBrandId = FALSE;
	HBOOL bIsMatched = FALSE;
//	CI_CamInst_t *pCam = NULL;
	HUINT16 usBindedCamBrandId = 0;
	HUINT32 i;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	if (pstCiDsc == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

//	pCam = &s_astCiCam[usSlotId];

	if(CI_GetBindedCamBrandId(usSlotId, &usBindedCamBrandId) == ERR_OK)
	{
		// 위 함수가 에러 리턴은 하지 않았지만 usBindedCamBrandId 가 0 인 경우에는 usBindedCamBrandId 가 없는 것임에 유의할 것 (CIv1 캠인지도 체크된다)
		HxLOG_Info("Cam BrandId (0x%X)\n", usBindedCamBrandId);
		if (usBindedCamBrandId > 0)
			bIsCiPlusCamWithValidBrandId = TRUE;
	}
	else
	{
		/* CIv1 캠도 항시 이런 상황이 되지만, SmarDTV old prototype & SMiT CI+ 캠과는 달리...
		   Neotion CI+ prototype 캠의 경우 캠 삽입시 CC 리소스가 이 뒤에 열리기 때문에 (CAS -> CC 순)
		   항시 캠 삽입하면 캠의 brand Id 를 적시에 가져오지 못하여 Host Service Shunning 이 되는 상황이 발생한다.
		   일단 이를 해결하기 위해 CONFIG_MW_CI_PLUS 적용시 local_cimgr__ProcUpdateCaInst()에서 항시 local_cimgr_CallbackDelayedAddCaInst()를 사용하도록 한다.
		   추후에 이 부분에 대한 근본적인 개선이 있어야 할 것이다 (eCAS_DEV_EVT_CamPlusInfo 이용할 계획임).
		   -> 위와 같이 개선함. */
		HxLOG_Info("Can't get BrandId from cam\n");
	}

	HxLOG_Info("Is this CI+ cam with valid BrandId ? (%d)\n", bIsCiPlusCamWithValidBrandId);
/*
	1. ci_protection_descriptor absent
	   and DVB CI and CI Plus CAM                               -> Host Service Shunning : in-active
	2. ci_protection_descriptor present and free_CI_mode is "0"
	   and DVB CI and CI Plus CAM                               -> Host Service Shunning : in-active
	3. ci_protection_descriptor present and free_CI_mode is "1"
	   and DVB CI CAM                                           -> Host Service Shunning : active
	4. ci_protection_descriptor present, free_CI_mode is "1"
	   and match_brand_flag = "0" or number_of_entries = "0"
	   and CI Plus CAM                                          -> Host Service Shunning : in-active
	5. ci_protection_descriptor present, free_CI_mode is "1",
	   match_brand_flag = "1" and number_of_entries ≠ "0"
	   and CICAM brand identifier not matched
	   and CI Plus CAM                                          -> Host Service Shunning : active
	6. ci_protection_descriptor present, free_CI_mode is "1",
	   match_brand_flag = "1" and number_of_entries ≠ "0"
	   and CICAM brand identifier matched
	   and CI Plus CAM                                          -> Host Service Shunning : in-active
*/
	if(pstCiDsc->bCiDscFlag == FALSE)
	{
		HxLOG_Info("Ci Descriptor absent -> no shunning\n");
		*bIsNeedShun = FALSE;
		return ERR_OK;
	}
	else
	{
		HxLOG_Info("Ci Descriptor present\n");
	}

	if(pstCiDsc->bFreeCiFlag == FALSE)
	{
		HxLOG_Info("FreeCiFlag in Ci Descriptor is FALSE -> no shunning\n");
		*bIsNeedShun = FALSE;
		return ERR_OK;
	}
	else
	{
		HxLOG_Info("FreeCiFlag in Ci Descriptor is TRUE\n");
		// TODO : CIPLUS 인증 서 만료 시 Normal Mode로 가는 경우 확인 필요.
		if(bIsCiPlusCamWithValidBrandId == FALSE)
		{
			HxLOG_Info("Cam with invalid BrandId -> shunning\n");
			*bIsNeedShun = TRUE;
			return ERR_OK;
		}
		else
		{
			if(pstCiDsc->bMatchBrandFlag == FALSE || pstCiDsc->ucNumOfBrandId == 0)
			{
				HxLOG_Info("MatchBrandFlag (%d) or NumOfBrandId (%d) -> no shunning\n", pstCiDsc->bMatchBrandFlag, pstCiDsc->ucNumOfBrandId);
				*bIsNeedShun = FALSE;
				return ERR_OK;
			}
			else // if (pstCiDsc->bMatchBrandFlag == TRUE && pstCiDsc->ucNumOfBrandId != 0)
			{
				HxLOG_Info("MatchBrandFlag (%d) and NumOfBrandId (%d)\n", pstCiDsc->bMatchBrandFlag, pstCiDsc->ucNumOfBrandId);
				for(i=0; i<pstCiDsc->ucNumOfBrandId; i++)
				{
					HxLOG_Info("Descriptor BrandId (0x%X) vs Cam BrandId (0x%X)\n", pstCiDsc->usCiBrandId[i], usBindedCamBrandId);
					if(pstCiDsc->usCiBrandId[i] == usBindedCamBrandId)
					{
						bIsMatched = TRUE;
						break;
					}
				}
				if(bIsMatched == TRUE)
				{
					*bIsNeedShun = FALSE;
					return ERR_OK;
				}
				else
				{
					*bIsNeedShun = TRUE;
					return ERR_OK;
				}
			}
		}
	}

	return ERR_OK;
}

static HERROR local_CiMgr_ProcDscSetForCiPlus(	CI_CamInst_t *pCam,
													HUINT32 ulDemuxId,
													HBOOL bIsUpdate,
													HBOOL bIsRetry,
													HBOOL bCalledByStartDescramble /* 이건 디버깅 용도 */)
{
	HUINT32 nResult = 0;
	HUINT32 i = 0;
	CI_CC_KEY_CIPHER enNegotiatedCckCipher = CI_CC_KEY_CIPHER_DES_56_ECB;
	SvcCas_DevDscType_e eDscType = eCAS_DSC_TYPE_NONEXDscType_None;
	HUINT32 ulNumOfDesc = 0;

	if(pCam == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	HxLOG_Info("pCam (%x), DmxId (%d), bIsUpdate (%d), bIsRetry (%d) - %s\n", (HUINT32)pCam, ulDemuxId, bIsUpdate, bIsRetry, bCalledByStartDescramble ? "ByStartDescr" : "ByUpdateCcRes");

	/*
		1. pmt updated.
		2. set dsc (pid changed)
		3. instance connected by host.

		start descrambler 이후가 될 것이다.
	*/

	if (pCam->ulNumOfPidForCaPmt == 0)
	{
		HxLOG_Error(" \n");
		return ERR_OK;
	}

	if (ulDemuxId >= CI_MAX_NUM_OF_DMX)
	{
		HxLOG_Error(" \n");
		return ERR_OK;
	}

	if (pCam->ulNumOfDsc[ulDemuxId] > CAS_MAX_NUM_OF_DSC)
	{
		HxLOG_Error("%d\n", pCam->ulNumOfDsc[ulDemuxId]);
	}

	/*	디스크램블러 Open / SetPid / SetKey :

		먼저 디스크램블러를 셋팅하기 위한 정보가 available 한지 확인한다.
		여기서 available 하지 못한 경우는 CIv1 캠이거나 아직 캠과 authentication 프로세스가 끝나지 않은 상태여서 CCK 가 아직 생성되지 못한 경우이다.
		CIv1 캠이면 여기 처리는 필요 없고,
		아직 캠과 authentication 프로세스가 끝나지 않은 상태면...
		1. 캠 삽입시 CC -> CAS 순으로 리소스가 오픈되는 캠에서는 위 상황은 발생하지 않고,
		2. 캠 삽입시 CAS -> CC 순으로 리소스가 오픈되는 캠에서는 authentication 프로세스가 끝나는 시점에서 eCAS_DEV_EVT_CamPlusInfo 가 발생되어 처리가 된다.
		따라서 결론적으로 nego 된 cipher 정보를 못가져오는 경우는 여기 처리가 필요 없다. */
	if ((pCam->bKeyInfoAvailable) || (CI_GetNegotiatedCckCipher(pCam->usSlotId, &enNegotiatedCckCipher) == ERR_OK))
	{
		if (pCam->bKeyInfoAvailable) // 이미 키 정보를 받은 상황
		{
			eDscType = pCam->eDscType;

			if (eDscType != eCAS_DSC_TYPE_DES_CLEAR && eDscType != eCAS_DSC_TYPE_AES_CLEAR)
			{
				HxLOG_Error("Invalid enKeyCipher (%d)\n", eDscType);
			}
		}
		else // 캠 삽입 후 CI+ 캠은 확인된 상태에서 아직 키 정보는 받지 못한 상황
		{
			HxLOG_Info("enNegotiatedCckCipher (%d) \n", enNegotiatedCckCipher);

			switch (enNegotiatedCckCipher)
			{
				case CI_CC_KEY_CIPHER_DES_56_ECB :
					eDscType = eCAS_DSC_TYPE_DES_CLEAR;
					break;

				case CI_CC_KEY_CIPHER_AES_128_CBC :
					eDscType = eCAS_DSC_TYPE_AES_CLEAR;
					break;

				default :
					HxLOG_Error("Invalid enKeyCipher (%d)\n", enNegotiatedCckCipher);
					break;
			}
		}
	}
	else
	{
		HxLOG_Info(" CCK Not Avaliable || CI_GetNegotiatedCckCipher is Failure \n");
		return ERR_OK;
	}

	/*
		1. descrambler open
		2. pid setting.
		3. key가 존재하면, key setting.
		bIsUpdate 가 TRUE 이면 처음 선택 된것 이기 때문에 pCam->ulNumOfPidForCaPmt에 나와 있는 pid를 모두 open한후 key를 setting한다.
	*/
	if(bIsUpdate != TRUE)
	{
		if(pCam->ulNumOfDsc[ulDemuxId] != 0)
		{
			// 아래는 일단 안전 장치...
			HxLOG_Error("slot %d's descrambler NOT closed (%d) - %s !!\n", pCam->usSlotId, pCam->ulNumOfDsc[ulDemuxId], bCalledByStartDescramble ? "ByStartDescr" : "ByUpdateCcRes");

			for (i = 0; i < pCam->ulNumOfDsc[ulDemuxId]; i++)
			{
				nResult = svc_cas_DevDscClose(pCam->stDscInfo[ulDemuxId][i].ulDscId);
				pCam->stDscInfo[ulDemuxId][i].ulDscId = CAS_RESOURCE_NONE;
				if(ERR_OK!= nResult)	HxLOG_Critical("Critical Error!!!\n");
			}

			pCam->ulNumOfDsc[ulDemuxId] = 0;
		}

		ulNumOfDesc = pCam->ulNumOfDsc[ulDemuxId];

		// For Debugging
		if(ulNumOfDesc != 0)
		{
			HxLOG_Critical("\n\n");
		}

		for (i = 0; i < pCam->ulNumOfPidForCaPmt; i++)
		{
			if (ulNumOfDesc < CAS_MAX_NUM_OF_DSC)
			{
				// 현재 디스크램블러를 운용하는 방식이 스트림 타입 정보는 don't care 이기 때문에 그냥 ePAL_DSC_DATATYPE_VIDEOPES 로 내려줌.
				nResult = svc_cas_DevDscOpen(ulDemuxId, eDscType, eCAS_DSC_DATA_TYPE_VIDEO_PES, &pCam->stDscInfo[ulDemuxId][ulNumOfDesc].ulDscId);
				if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

				pCam->stDscInfo[ulDemuxId][ulNumOfDesc].usPid = pCam->PidInfoForCaPmt[i].usPid;

				nResult = svc_cas_DevDscSetPid(pCam->stDscInfo[ulDemuxId][ulNumOfDesc].ulDscId, pCam->stDscInfo[ulDemuxId][ulNumOfDesc].usPid);
				if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

				HxLOG_Info("Open dsc - Dmx Id (%d), Desc Id (%d) : set pid - Pid (%x)\n",
					ulDemuxId, pCam->stDscInfo[ulDemuxId][ulNumOfDesc].ulDscId, pCam->stDscInfo[ulDemuxId][ulNumOfDesc].usPid);

				ulNumOfDesc++;
			}
			else
			{
				HxLOG_Error("see CAS_MAX_NUM_OF_ES / MAX_NUM_OF_DSC_ID / MAX_DESC_CH_NUM (DI_DSC_SLOTNUM_MAXIMUM) !!\n");
				break; // 일단 현재 가용한 리소스만이라도 처리할 수 있도록 에러 리턴하지는 않게 한다...
			}
		}

		HxLOG_Info("ulDemuxId = %d, ulNumOfDesc = %d, pCam->bKeyInfoAvailable = %d\n", ulDemuxId, ulNumOfDesc, pCam->bKeyInfoAvailable);
		pCam->ulNumOfDsc[ulDemuxId] = ulNumOfDesc;

		if (pCam->bKeyInfoAvailable)
		{
			#if 0 // org : CI+ 스택으로 키 셋팅을 요청함 -> 아래와 같이 CI mgr 에서 자체 처리할 수 있도록 개선함.
			CI_ReqSetCckToDsc(usSlotId);
			#else // 셋팅된 디스크램블러들에 대해 SetKey
			for (i = 0; i < pCam->ulNumOfDsc[ulDemuxId]; i++)
			{
				if (pCam->stDscInfo[ulDemuxId][i].ulDscId != CAS_RESOURCE_NONE)
				{
					#if defined(CONFIG_DEBUG)
					{
						HUINT32		j;

						HxLOG_Info("Cck = 0x[ ");

						for(j = 0; j < 16; j++)
						{
							HxLOG_Print("%x ", pCam->ucCck[j]);
						}
						HxLOG_Print("]\n");

						HxLOG_Info("Civ = 0x[ ");

						for(j = 0; j < 16; j++)
						{
							HxLOG_Print("%x ", pCam->ucCiv[j]);
						}
						HxLOG_Print("]\n");
					}
					#endif

					nResult = svc_cas_DevDscSetKeyForCiPlus(pCam->stDscInfo[ulDemuxId][i].ulDscId, eDscType, pCam->eParity,
						pCam->ucCck, (eDscType == eCAS_DSC_TYPE_DES_CLEAR) ? NULL : pCam->ucCiv);
					if(ERR_OK!=nResult)
					{
						HxLOG_Critical("Critical Error!!!\n");
					}
					else
					{
						HxLOG_Info("dsc set key for ciplus ok  : Dmx Id (%d), Desc Id (%d), eDscType (%d), eParity (%d)\n",
							ulDemuxId, pCam->stDscInfo[ulDemuxId][i].ulDscId, eDscType, pCam->eParity);
					}
				}
				else
				{
					HxLOG_Critical("\n");
				}
			}
			#endif
		}
	}
	else
	{
		/* stProgInfo 가 갱신되어 update 하는 경우 -> 디스크램블러 셋팅을 갱신해야 한다.
			-> 혹시 모르니 bIsRetry 루틴도 이를 타도록 추가함.
			bIsUpdate 가 false이면 update이기 때문에 pid가 없어진 경우는 삭제한다.
		*/

		HUINT32 i, j, cnt;
		HUINT32 ulNumOfDsc;
		HUINT32 ulDscId[CAS_MAX_NUM_OF_DSC];
		HUINT16 usEsPid[CAS_MAX_NUM_OF_DSC];

		if (bIsUpdate)
			HxLOG_Error("<< Update >>\n");
		if (bIsRetry)
			HxLOG_Info("<< Retry >>\n");

		if (pCam->ulNumOfDsc[ulDemuxId] == 0) // 플로우 상 있을 수 없는 경우로 확인 필요 !!
		{
			HxLOG_Error("slot %d's descrambler NOT opened - %s !!\n", pCam->usSlotId, bCalledByStartDescramble ? "ByStartDescr" : "ByUpdateCcRes");
		}

		// 갱신된 stProgInfo 와 현재 운용 중인 PID 들과 비교하여 삭제되거나 추가된 것이 있으면 처리를 해준다.

		// 1. 먼저 삭제된 것이 있는지 확인...

		ulNumOfDsc = pCam->ulNumOfDsc[ulDemuxId];
		for (i = 0; i < ulNumOfDsc; i++)
		{
			ulDscId[i] = pCam->stDscInfo[ulDemuxId][i].ulDscId;
			usEsPid[i] = pCam->stDscInfo[ulDemuxId][i].usPid;
		}
		for (i = ulNumOfDsc; i < CAS_MAX_NUM_OF_DSC; i++)
		{
			ulDscId[i] = CAS_RESOURCE_NONE;
			usEsPid[i] = 0;
		}

		for (i = 0; i < ulNumOfDsc; i++)
		{
			for (j = 0; j < pCam->ulNumOfPidForCaPmt; j++)
			{
				// 현재 디스크램블러를 운용하는 방식이 스트림 타입 정보는 don't care 이기 때문에 스트림 타입은 비교하지 않음.
				if (usEsPid[i] == pCam->PidInfoForCaPmt[j].usPid)
					break;
			}
			if (j == pCam->ulNumOfPidForCaPmt)
			{
				HxLOG_Error("EsPid 0x%x will be deleted\n", usEsPid[i]);

				nResult = svc_cas_DevDscClose(ulDscId[i]);
				ulDscId[i] = CAS_RESOURCE_NONE;
				if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

				pCam->ulNumOfDsc[ulDemuxId]--;
			}
		}

		cnt = 0;
		for (i = 0; i < CAS_MAX_NUM_OF_DSC; i++) // 삭제 및 그에 따른 디스크램블러 셋팅 모두 완료되었고 이제 리소스 정리로 마무리...
		{
			if (ulDscId[i] != CAS_RESOURCE_NONE)
			{
				pCam->stDscInfo[ulDemuxId][cnt].ulDscId = ulDscId[i];
				pCam->stDscInfo[ulDemuxId][cnt].usPid = usEsPid[i];
				cnt++;
			}
		}
		if (cnt != pCam->ulNumOfDsc[ulDemuxId])
		{
			HxLOG_Error("cnt %d not matched with ulNumOfDsc %d !!\n", cnt, pCam->ulNumOfDsc[ulDemuxId]);
		}

		// 2. 추가된 것이 있는지 확인...

		ulNumOfDsc = pCam->ulNumOfDsc[ulDemuxId];

		for (i = 0; i < pCam->ulNumOfPidForCaPmt; i++)
		{
			for (j = 0; j < ulNumOfDsc; j++)
			{
				// 현재 디스크램블러를 운용하는 방식이 스트림 타입 정보는 don't care 이기 때문에 스트림 타입은 비교하지 않음.
				if (pCam->PidInfoForCaPmt[i].usPid == usEsPid[j])
				{
					HxLOG_Info("same pid previous one \n");
					break;
				}
			}
			if (j == ulNumOfDsc)
			{
				HxLOG_Error("EsPid 0x%x will be added\n", pCam->PidInfoForCaPmt[i].usPid);

				ulNumOfDesc = pCam->ulNumOfDsc[ulDemuxId];
				if (ulNumOfDesc < CAS_MAX_NUM_OF_DSC)
				{
					// 현재 디스크램블러를 운용하는 방식이 스트림 타입 정보는 don't care 이기 때문에 그냥 ePAL_DSC_DATATYPE_VIDEOPES 로 내려줌.
					nResult = svc_cas_DevDscOpen((HUINT32)ulDemuxId, eDscType, eCAS_DSC_DATA_TYPE_VIDEO_PES, &pCam->stDscInfo[ulDemuxId][ulNumOfDesc].ulDscId);
					if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

					pCam->stDscInfo[ulDemuxId][ulNumOfDesc].usPid = pCam->PidInfoForCaPmt[i].usPid;

					HxLOG_Info(" pid of ca pmt (0x%x) \n", pCam->PidInfoForCaPmt[i].usPid);

					nResult = svc_cas_DevDscSetPid(pCam->stDscInfo[ulDemuxId][ulNumOfDesc].ulDscId, pCam->stDscInfo[ulDemuxId][ulNumOfDesc].usPid);
					if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

					HxLOG_Info("Open dsc - Dmx Id (%d), Desc Id (%d) : set pid - Pid (%x)\n",
						ulDemuxId, pCam->stDscInfo[ulDemuxId][ulNumOfDesc].ulDscId, pCam->stDscInfo[ulDemuxId][ulNumOfDesc].usPid);

					ulNumOfDesc++;
				}
				else
				{
					HxLOG_Error("see CAS_MAX_NUM_OF_ES / MAX_NUM_OF_DSC_ID / MAX_DESC_CH_NUM !!\n");
					break; // 일단 현재 가용한 리소스만이라도 처리할 수 있도록 에러 리턴하지는 않게 한다...
				}

				pCam->ulNumOfDsc[ulDemuxId] = ulNumOfDesc;
			}
		}

		if (pCam->bKeyInfoAvailable) // 새로 디스크램블링이 걸리는 경우와는 달리 여기서는 사실 이를 체크할 필요는 없을 것...
		{
			#if 0 // org : CI+ 스택으로 키 셋팅을 요청함 -> 아래와 같이 CI mgr 에서 자체 처리할 수 있도록 개선함.
			CI_ReqSetCckToDsc(usSlotId);
			#else // 셋팅된 디스크램블러들에 대해 SetKey
			for (i = 0; i < pCam->ulNumOfDsc[ulDemuxId]; i++)
			{
				if (pCam->stDscInfo[ulDemuxId][i].ulDscId != CAS_RESOURCE_NONE)
				{
					nResult = svc_cas_DevDscSetKeyForCiPlus(pCam->stDscInfo[ulDemuxId][i].ulDscId, eDscType, pCam->eParity,
						pCam->ucCck, (eDscType == eCAS_DSC_TYPE_DES_CLEAR) ? NULL : pCam->ucCiv);

					if(ERR_OK!=nResult)
					{
						HxLOG_Critical("Critical Error!!!\n");
					}
					else
					{
						HxLOG_Info("dsc set key for ciplus ok  : Dmx Id (%d), Desc Id (%d), eDscType (%d), eParity (%d)\n",
							ulDemuxId, pCam->stDscInfo[ulDemuxId][i].ulDscId, eDscType, pCam->eParity);
					}
				}
				else
				{
					HxLOG_Critical("\n");
				}
			}
			#endif
		}

	}


	return ERR_OK;
}

static HERROR local_CiMgr_ProcUpdateCcResource(CI_UPDATE_CA_INST_PARAM *pUpdateCcRes)
{
	CI_CamInst_t *pCam;
	CI_Context_t *pCiContext;
	HUINT32 i = 0;

	/*
		1. cas --> cc 인 경우.
		2. cc --> cas인 경우.

		1번의 경우 start descramble을 한번 더 해준다.
	*/

	if (pUpdateCcRes == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	if (pUpdateCcRes->usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", pUpdateCcRes->usSlotId);
		return ERR_FAIL;
	}

	HxLOG_Info(" slot id (%d) \n", pUpdateCcRes->usSlotId);

	/*
		1. 현재 slot id와 연결된 ci context 확인.
		2. pmt, set dsc 여부 확인.
		3. prog info 만들어서 set key 한다.
	*/
	/* cam instance 는 slot id로 관리 된다. */
	pCam = &s_astCiCam[pUpdateCcRes->usSlotId];

	if(pCam->usCaInfoCnt == 0)
	{
		HxLOG_Info(" usCaInfoCnt ZERO \n");
		return ERR_OK;
	}

	for(i=0; i<CI_MAX_NUM_OF_ACT; i++)
	{
		pCiContext = &s_astCiContext[i];

		if (pCiContext->pCam == pCam && pCiContext->hAction != HANDLE_NULL)
		{
			local_CiMgr_ProcDscSetForCiPlus(pCam, pCiContext->ulDemuxId, 0, 0, 0);

			/**
			  *		Neotion CAM의 경우, CAS -> CC 의 순서로 resource open되는데,
			  *		이 경우 start descramble시 하는 shun condition check에서 항상 shun enable이 되는 문제가 발생함 (아직 CC가 open되지 않았으므로 )
			  *		이를 막기위해 delayed cas update를 사용할 수도 있으나,
			  *		여기서는 CC update되는 이 시점에서 다시 한번 shun condition check를 해줌
			**/
			if (pCiContext->eChType != eCasChannel_FTA)
			{
				CAS_CiDscInfo_t *pDscInfo = NULL;
				HBOOL bIsNeedShun = FALSE;

				/**
				  *	cam 삽입 직후 UriDefault가 Failure 인 경우 다시 한번 해주어야 함.
				  *	Current ch type check를 위해서 여기로 이동함.
				**/
				HxLOG_Info(" set_uri_default (%d)  \n", pCam->usSlotId);
				CI_SetUriDefault(pCam->usSlotId, TRUE);

				/**
				  *	check shun condition
				**/
				if (pCam->usSlotId != pCiContext->pCam->usSlotId)
				{
					HxLOG_Info("invalid slot id - pCam->usSlotId(%d) !=  pCiContext->pCam->usSlotId(%d) \n", pCam->usSlotId, pCiContext->pCam->usSlotId);
				}
				else
				{
					if(local_CiMgr_GetCiDescInfo(pCiContext, &pDscInfo) != ERR_OK)
					{
						HxLOG_Info("get description info failed\n");
					}
					else
					{
						if(local_CiMgr_ProcCheckShunCondition(pCiContext->pCam->usSlotId, pDscInfo, &bIsNeedShun) != ERR_OK)
						{
							HxLOG_Error("\n");
						}
						else
						{
							if (bIsNeedShun == TRUE)
							{
								pCiContext->bSvcShunned = TRUE;

								CI_SetPlusMode(pCiContext->pCam->usSlotId, CI_PLUS_PROTECTED_MODE);

								#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
								local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pCiContext->hAction), -1);
								#else
								svc_cas_DevCiSetTsPath(pCiContext->pCam->usSlotId, eCAS_DEV_CI_BYPASS);
								#endif

								// 중요 log
								HxLOG_Error("Shunning enable, Set Bypass\n");
							}
							else
							{
								pCiContext->bSvcShunned = FALSE;

								CI_SetPlusMode(pCiContext->pCam->usSlotId, CI_PLUS_FREE_MODE);

								#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
								local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pCiContext->hAction), pCiContext->pCam->usSlotId);
								#else
								svc_cas_DevCiSetTsPath(pCiContext->pCam->usSlotId, eCAS_DEV_CI_PASSTHROUGH);
								#endif

								// 중요 log
								HxLOG_Error("Shunning Disable, Set Passthru\n");
							}
						}
					}
				}
			}
		}
	}

	return ERR_OK;
}

static HERROR local_CiMgr_ProcKeyChanged(CI_PLUS_KEY_CHANGE_PARAM *pParam)
{
	CI_SLOT_ID usSlotId;
	CI_CamInst_t * pCam = NULL;
	HERROR nResult = ERR_OK;
	HUINT32 i, j;

	if (pParam == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	usSlotId = pParam->usSlotId;
	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	pCam = &s_astCiCam[usSlotId];

	if (pCam->usSlotId != usSlotId) // 아직 캠 인스턴스가 초기화 되지 않았을 때 이럴 수가 있다. 해당 슬롯 아이디의 s_astCiCam[]에 키 정보만 셋팅하면 되므로 무시.
	{
		HxLOG_Warning("pCam->usSlotId = %x, usSlotId = %x\n", pCam->usSlotId, usSlotId);

		if (pCam->ulNumOfDsc[0] || pCam->bKeyInfoAvailable) // 그러나 이런 상황이면 무시하면 안된다. 뭔가 구현된 플로우 상에 문제가 있는 거다. 확인 필요 !!
			HxLOG_Error("\n");
	}

	pCam->eDscType = pParam->eDscType;
	pCam->eParity = pParam->eParity;

	switch (pCam->eDscType)
	{
	case eCAS_DSC_TYPE_DES_CLEAR :
		HxSTD_memcpy(pCam->ucCck, pParam->ucCck, 8);
		break;

	case eCAS_DSC_TYPE_AES_CLEAR :
		HxSTD_memcpy(pCam->ucCck, pParam->ucCck, 16);
		HxSTD_memcpy(pCam->ucCiv, pParam->ucCiv, 16);
		break;

	default :
		HxLOG_Error("Invalid eDscType (%d)\n", pCam->eDscType);
		return ERR_FAIL;
	}

	if (!pCam->bKeyInfoAvailable)
		pCam->bKeyInfoAvailable = 1;

	for (j=0; j<CI_MAX_NUM_OF_DMX; j++)
	{
		HxLOG_Info("[%d] slot %d's NumOfDsc = %d, KeyInfoAvailable = %d\n", j, usSlotId, pCam->ulNumOfDsc[j], pCam->bKeyInfoAvailable);

		if (pCam->ulNumOfDsc[j])
		{
			for (i = 0; i < pCam->ulNumOfDsc[j]; i++)
			{
				if (pCam->stDscInfo[j][i].ulDscId != CAS_RESOURCE_NONE)
				{
					HxLOG_Info("svc_cas_DevDscSetKeyForCiPlus() ulDscId = %d, eDscType = %d\n", j, pCam->stDscInfo[j][i].ulDscId, pCam->eDscType);

					#if  defined(CONFIG_DEBUG)
					{
						HUINT32		j;

						HxLOG_Info("Cck = 0x[ ");

						for(j = 0; j < 16; j++)
						{
							HxLOG_Print("%x ", pCam->ucCck[j]);
						}
						HxLOG_Print("]\n");

						HxLOG_Info("Civ = 0x[ ");

						for(j = 0; j < 16; j++)
						{
							HxLOG_Print("%x ", pCam->ucCiv[j]);
						}
						HxLOG_Print("]\n");
					}
					#endif

					nResult = svc_cas_DevDscSetKeyForCiPlus(pCam->stDscInfo[j][i].ulDscId, pCam->eDscType, pCam->eParity,
						pCam->ucCck, (pCam->eDscType == eCAS_DSC_TYPE_DES_CLEAR) ? NULL : pCam->ucCiv);
					if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");
				}
				else
				{
					HxLOG_Critical("\n");
				}
			}
		}
		else
		{
//			SmarDTV old prototype & SMiT CI+ 캠의 경우, 캠 삽입시 CC -> CAS 순으로 리소스가 오픈되기 때문에 캠 삽입시에 항시 이런 상황이 발생한다 (정상).
//			디스크램블러가 오픈되어 있지 않으면 키 셋팅하지 않으면 그만이므로 아래 로그는 일반적으로 찍히지 않도록 한다.
//			그리고 FTA 채널에서도 캠이 CCK 를 새로 트리거링 하면 이 상황이 발생할 수 있다 (FTA 채널에서는 디스크램블러를 운용하지 않기 때문. 이도 정상).
			HxLOG_Info("[%d] slot %d's descrambler NOT opened !!\n", j, usSlotId);
		}
	}

	return nResult;
}

static HERROR local_CiMgr_ProcUriControl(CI_PLUS_URI_CONTROL_PARAM *pParam) // Apply URI to External Output for "scramble" channel
{
	HBOOL			bFound = 0;
	HUINT32			i, j;
	Handle_t			hAction = HANDLE_NULL;
	CI_SLOT_ID		usSlotId;
	CI_PLUS_MODE	enPlusMode;
	CI_CamInst_t		*pCam = (CI_CamInst_t *)NULL;

	if (pParam == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	usSlotId = pParam->usSlotId;
	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	/* <TODO_CI_PLUS> : 외부 출력이 현재 해당 캠 슬롯에 할당되어 있는지 확인하고 그렇지 않으면 그냥 리턴하도록 하자... */

	// FTA 채널인데 이 함수가 실행된다면 구현된 플로우 상에 문제가 있거나 ( -> see local_cimgr_TurnOffUriControlForFTA() )
	// 채널 전환하면서 이전 채널의 것이 뒤늦게 온 경우이다.

	CI_GetPlusMode(usSlotId, &enPlusMode);

#if defined(CONFIG_DEBUG) // 중요 로그이므로 항시 찍어 주자.
	switch (pParam->stUriInfo.ucMode)
	{
	case CI_CC_URI_MODE_DEFAULT :
		Hx_Print(" > URI (Default) :");
		break;
	case CI_CC_URI_MODE_TIMEOUT :
		Hx_Print(" > URI (Timeout) :");
		break;
	case CI_CC_URI_MODE_FROMCAM :
		Hx_Print(" > URI (FromCAM #%d) :", usSlotId);
		break;
	default:
		Hx_Print(" > URI (Unknown) :");
		break;
	}
	Hx_Print(" APS (%x), EMI (%x), ICT (%x), RCT (%x), DOT (%x), RL (%ld min), PlusMode (%d)",
		pParam->stUriInfo.ucAps, pParam->stUriInfo.ucEmi, pParam->stUriInfo.ucIct, pParam->stUriInfo.ucRct, pParam->stUriInfo.ucDot, pParam->stUriInfo.ulMinutesForRL, enPlusMode);
	if (pParam->stUriInfo.ucMode == CI_CC_URI_MODE_FROMCAM)
		Hx_Print(" for 0x%x (%d)\n", pParam->stUriInfo.usServiceId, pParam->stUriInfo.usServiceId);
	else
		Hx_Print("\n");
#endif

	if (enPlusMode == CI_PLUS_PROTECTED_MODE)
	{
		// shunning 되어 있다면 URI control 할 필요가 없지...
		return ERR_OK;
	}

	s_stUriInfo[usSlotId].ucMode = pParam->stUriInfo.ucMode;
	s_stUriInfo[usSlotId].ucAps = pParam->stUriInfo.ucAps;
	s_stUriInfo[usSlotId].ucEmi = pParam->stUriInfo.ucEmi;
	s_stUriInfo[usSlotId].ucIct = pParam->stUriInfo.ucIct;
	s_stUriInfo[usSlotId].ucRct = pParam->stUriInfo.ucRct;
	s_stUriInfo[usSlotId].ucDot = pParam->stUriInfo.ucDot;
	s_stUriInfo[usSlotId].ulMinutesForRL = pParam->stUriInfo.ulMinutesForRL;

	pCam = &s_astCiCam[usSlotId];

	for (i=0; i<CI_MAX_NUM_OF_ACT; i++)
	{
		bFound = FALSE;
		if (s_astCiContext[i].pCam == pCam)
		{
			hAction = s_astCiContext[i].hAction;
			if(hAction == HANDLE_NULL)
			{
				continue;
			}

			for (j=0; j<CAS_MAX_NUM_OF_ACTION; j++)
			{
				if (pCam->hActList[j] == hAction)
				{
					HxLOG_Print("action handle (0x%x) hAction (0x%x)\n", pCam->hActList[j], hAction);
					bFound = TRUE;
				}
			}

			if (bFound == TRUE)
			{
				local_CiMgr_SendCiDrmInfo(hAction, pCam->hCasDevice, eDxCopyrightControl_CiPlusDrmType_Uri, (void *)&s_stUriInfo[usSlotId]);
				s_astCiContext[i].bRcvUri = TRUE;
			}
			else
			{
				local_CiMgr_SendCiDrmInfo(hAction, pCam->hCasDevice, eDxCopyrightControl_CiPlusDrmType_Uri, (void *)NULL);
			}
		}
	}

	s_abUriSent[pCam->usSlotId] = TRUE;
/*
	for (i=0; i<CAS_MAX_NUM_OF_ACTION; i++)
	{
		if (pCam->hActList[i] != HANDLE_NULL)
		{
			HxLOG_Print("action handle (0x%x) \n", pCam->hActList[i]);
			CASMGR_UpdateDrmInfo(pCam->hActList[i], pCam->hCasDevice, (void *)&s_stUriInfo[0]);
		}
	}
*/
	return ERR_OK;
}

static HERROR local_CiMgr_ProcSrmControl(CI_PLUS_SRM_CONTROL_PARAM *pParam) // Apply SRM data
{
	CI_SLOT_ID usSlotId;
	HERROR nResult = ERR_OK;

	if (pParam == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	usSlotId = pParam->usSlotId;
	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		OxCAS_Free(pParam->pucSrmData);
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	/* <TODO_CI_PLUS> : Apply SRM data (HDCP 모듈에 넘겨라) */

	OxCAS_Free(pParam->pucSrmData);

	return nResult;
}

static HERROR local_CiMgr_FreeSdtSlot(HUINT16 usUniqueId)
{
	HUINT32 i;

	for(i=0; i<CAS_MAX_NUM_OF_TS; i++)
	{
		if(s_stSdtStatus[i].bUsed == CAS_USED)
		{
			if(s_stSdtStatus[i].usUniqueId == usUniqueId)
			{
				if(s_stSdtStatus[i].pstList != NULL)
				{
					UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(s_stSdtStatus[i].pstList, local_CiMgr_FreeCiDescList);
				}

				HxSTD_memset(&s_stSdtStatus[i], 0x00, sizeof(CAS_SdtStatus_t));
				s_stSdtStatus[i].ulFilterId = CAS_FILTER_ID_NULL;
				s_stSdtStatus[i].bUsed = CAS_NOT_USED;

				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}

static CAS_SdtStatus_t *local_CiMgr_AllocSdtSlot(HUINT16 usUniqueId)
{
	HUINT32 i;

	for(i=0; i<CAS_MAX_NUM_OF_TS; i++)
	{
		if(s_stSdtStatus[i].bUsed == CAS_NOT_USED)
		{
			if(s_stSdtStatus[i].usUniqueId != 0)
			{
				HxLOG_Critical("\n");
			}

			s_stSdtStatus[i].usUniqueId = usUniqueId;
			s_stSdtStatus[i].bUsed = CAS_USED;

			return &s_stSdtStatus[i];
		}
	}

	return NULL;
}

static HERROR local_CiMgr_ParseSdtForCiPlus(CI_Context_t *pCiContext, HUINT8 *pSdtRaw)
{
	HUINT32 nResult = 0;
	CAS_SdtStatus_t *pSdt = NULL;
	CAS_CiDscInfo_t *pCiDes = NULL;
	HUINT8 *pCiDesRaw = NULL;
	HUINT32 j = 0;

	svcCas_SiSdtSectionInfo_t sdtInfo;
	SvcCas_StdServiceInfo_t sdtSvcInfo;

	HUINT32 ulSvcInfoCnt = 0;
	HUINT32 ulSvcDesCnt = 0;
	HUINT32 ulCiDscLength = 0;

	HUINT16 usUniqueId = 0;

	if(pCiContext == NULL || pSdtRaw == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	usUniqueId = pCiContext->usUniqueId;

	local_CiMgr_FreeSdtSlot(usUniqueId);
	pSdt = local_CiMgr_AllocSdtSlot(usUniqueId);
	if(pSdt == NULL)
	{
		HxLOG_Critical("\n");
		return ERR_FAIL;
	}

	svc_cas_SubSvcSiGetSdtSectionInfoFromRaw(pSdtRaw, &sdtInfo);

	#if defined(CONFIG_DEBUG)
	//#define CI_MGR_SDT_PRINT
	#endif

	#if defined(CI_MGR_SDT_PRINT)
	HxLOG_Info("===== SDT =====\n");
	HxLOG_Print(" Sdt TsId = 0x%x\n", sdtInfo.usTsId);
	HxLOG_Print(" Sdt ucVer = 0x%x\n", sdtInfo.ucVer);
	HxLOG_Print(" Sdt usOrgNetId = 0x%x\n", sdtInfo.usOrgNetId);
	HxLOG_Print(" Sdt ulCrc32 = 0x%x\n\n", sdtInfo.ulCrc32);
	#endif

	if( (pCiContext->usTsId == sdtInfo.usTsId) && (pCiContext->usOnId == sdtInfo.usOrgNetId) )
	{
		SIxTextEncoding_e eTextEnc;

		eTextEnc = svc_cas_MgrGetTextEncoding(svc_cas_SubSvcRmGetActionId(pCiContext->hAction));

		// 동일한 Triple Id 만.
		nResult = svc_cas_SubSvcSiCountSdtServiceFromRaw(pSdtRaw, &ulSvcInfoCnt);
		if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

		// SDT Status Update.
		#if defined(CI_MGR_SDT_PRINT)
		HxLOG_Print(" -> # of Service = %d\n", ulSvcInfoCnt);
		#endif
		pSdt->ulSvcNum = ulSvcInfoCnt;

		for(j=0; j<ulSvcInfoCnt; j++)
		{
			nResult = svc_cas_SubSvcSiGetSdtServiceInfoFromRaw(pSdtRaw, j, &sdtSvcInfo);

			#if defined(CI_MGR_SDT_PRINT)
			HxLOG_Print("\n\t --- service %d / %d ---\n\n", j+1, ulSvcInfoCnt);
			HxLOG_Print("\t Sdt SvcId = 0x%x\n", sdtSvcInfo.usSvcId);
			HxLOG_Print("\t Sdt bEitSched = 0x%x\n", sdtSvcInfo.bEitSched);
			HxLOG_Print("\t Sdt bEitPreFollow = 0x%x\n", sdtSvcInfo.bEitPreFollow);
			HxLOG_Print("\t Sdt ucRunningStatus = 0x%x\n", sdtSvcInfo.ucRunningStatus);
			HxLOG_Print("\t Sdt bCaFree = 0x%x\n", sdtSvcInfo.bCaFree);
			#endif

			pCiDes = (CAS_CiDscInfo_t *)OxCAS_Malloc(sizeof(CAS_CiDscInfo_t));
			if(pCiDes == NULL)
			{
				HxLOG_Critical("\n\n");
				return ERR_FAIL;
			}
			HxSTD_memset(pCiDes, 0x00, sizeof(CAS_CiDscInfo_t));
			pCiDes->usSvcId = sdtSvcInfo.usSvcId;

			nResult = svc_cas_SubSvcSiCountSdtServiceDescriptorFromRaw(pSdtRaw, j, TAG_CI_PROTECTION_DESCRIPTOR, &ulSvcDesCnt);
			if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

			#if defined(CI_MGR_SDT_PRINT)
			HxLOG_Print("\t # of Descriptor = %d\n\n", ulSvcDesCnt);
			#endif

			if(ulSvcDesCnt != 0)
			{
				nResult = svc_cas_SubSvcSiGetSdtServiceDescriptorFromRaw(pSdtRaw, eTextEnc, j, TAG_CI_PROTECTION_DESCRIPTOR, 0, (void**)&pCiDesRaw);
				if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");

				if(pCiDesRaw != NULL)
				{
					HUINT32 ulLoop = 0;
					HUINT8 *pDsc = NULL;

					pCiDes->pDesRaw = pCiDesRaw;

					if(pCiDes->pDesRaw[0] == TAG_CI_PROTECTION_DESCRIPTOR)
					{
						pCiDes->bCiDscFlag = TRUE;
					}
					else
					{
						HxLOG_Critical("\n");
					}

					ulCiDscLength = pCiDes->pDesRaw[1];

					if (ulCiDscLength == 0)
					{
						HxLOG_Critical("\n");
					}

					#if defined(CI_MGR_SDT_PRINT)
					HxLOG_Print("\t ci_protection_descriptor length = %d\n", ulCiDscLength);
					#endif

					pDsc = &pCiDes->pDesRaw[2];
					pCiDes->bFreeCiFlag = (*pDsc & 0x80) >> 7;
					pCiDes->bMatchBrandFlag = (*pDsc++ & 0x40) >> 6;

					#if defined(CI_MGR_SDT_PRINT)
					HxLOG_Print("\t free_ci_mode_flag = %d, match_brand_flag = %d\n", pCiDes->bFreeCiFlag, pCiDes->bMatchBrandFlag);
					#endif

					if(pCiDes->bMatchBrandFlag == 1 && ulCiDscLength > 1)
					{
						pCiDes->ucNumOfBrandId = *pDsc++;

						#if defined(CI_MGR_SDT_PRINT)
						HxLOG_Print("\t # of brand id = %d\n", pCiDes->ucNumOfBrandId);
						#endif

						for(ulLoop = 0; ulLoop < pCiDes->ucNumOfBrandId; ulLoop++)
						{
							pCiDes->usCiBrandId[ulLoop] = ((unsigned short)*pDsc++ << 8) & 0xff00;
							pCiDes->usCiBrandId[ulLoop] |= (unsigned short)*pDsc++;

							#if defined(CI_MGR_SDT_PRINT)
							HxLOG_Print("\t (%d)-th brand id = 0x%x\n", ulLoop+1, pCiDes->usCiBrandId[ulLoop]);
							#endif
						}
					}
				}
				else
				{
					HxLOG_Print("\t -> no ci_protection_descriptor (pCiDesRaw is NULL)\n");
					pCiDes->bCiDscFlag = FALSE;
				}
			}
			else
			{
				#if defined(CI_MGR_SDT_PRINT)
				HxLOG_Print("\t -> no ci_protection_descriptor (ulSvcDesCnt is 0)\n");
				#endif
				pCiDes->bCiDscFlag = FALSE;
			}

			pSdt->pstList = UTIL_LINKEDLIST_AppendListItemToPointerList(pSdt->pstList, pCiDes);
			if(pSdt->pstList == NULL)
			{
				HxLOG_Critical("\n");
				return ERR_FAIL;
			}
		}
	}

	pSdt->ulTblState = eCASSI_Table_Running;			/* Parsing Complete. */

	return ERR_OK;
}

static HERROR local_CiMgr_UpdateSdt2DB(Handle_t hSvcHandle, HBOOL bFreeCiFlag, HUINT8 ucNumOfBrandId, HUINT16 *pusCiBrandId)
{
	HERROR				hError = ERR_FAIL;
	HBOOL				bNeedSave = FALSE;
	UNIXTIME			ulCurrentTime = 0;
	SvcCas_DbSvcInfo_t 	stSvcInfo;

	// check validity
	if ( (hSvcHandle == HANDLE_NULL) || (hSvcHandle == 0) )
	{
		HxLOG_Error("Invalid service handle(0x%X)\n", hSvcHandle);
		return ERR_FAIL;
	}
	if (ucNumOfBrandId > CI_NUM_OF_BRAND_ID)
	{
		HxLOG_Error("Invalid ucNumOfBrandId(%d)\n", ucNumOfBrandId);
		ucNumOfBrandId = CI_NUM_OF_BRAND_ID;
	}

	// get current time
	hError = (HERROR)VK_CLOCK_GetTime((unsigned long*)&ulCurrentTime);
	if(hError != VK_OK || UNIX_FIRST_DAY >= ulCurrentTime)
	{
		HxLOG_Error("VK_CLOCK_GetTime() Fail ! \n");
		//ulCurrentTime = UNIX_FIRST_DAY;
		return ERR_FAIL;
	}

	// load saved data
	hError = svc_cas_SubSvcDbGetSvcInfoByServiceHandle(hSvcHandle, &stSvcInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Error("svc_cas_SubSvcDbGetSvcInfoByServiceHandle() Fail ! \n");
		return ERR_FAIL;
	}

	// check save condition
	if (		(stSvcInfo.ulTimeStamp + (7 * SECONDS_PER_DAY) < ulCurrentTime)	// time stamp expired
		||	(stSvcInfo.ucNumOfEntries != (HUINT32)ucNumOfBrandId))		// data changed
	{

		bNeedSave = TRUE;
	}
	else if (		(bFreeCiFlag == TRUE)
			&&	((stSvcInfo.ulUserFlag1 & FreeCiMode) == 0))
	{
		/* bFreeCiFlag enabled */
		bNeedSave = TRUE;
	}
	else if (		(bFreeCiFlag == FALSE)
			&&	((stSvcInfo.ulUserFlag1 & FreeCiMode) != 0))
	{
		/* bFreeCiFlag disable */
		bNeedSave = TRUE;
	}


	#if defined(CONFIG_DEBUG)
	HxLOG_Info("===== SDT =====\n");
	HxLOG_Print("========================= \n");
	HxLOG_Print(" SDT update - CI+ \n");
	HxLOG_Print("========================= \n");
	HxLOG_Print("  = DB = \n");
	HxLOG_Print("   TimeStamp(0x%X) \n", stSvcInfo.ulTimeStamp);
	HxLOG_Print("   ucNumOfEntries(0x%X) \n", stSvcInfo.ucNumOfEntries);
	HxLOG_Print("   ulUserFlag1(%d) \n", stSvcInfo.ulUserFlag1);
	HxLOG_Print("========================= \n");
	HxLOG_Print("  = SI = \n");
	HxLOG_Print("   Cur TimeStamp(0x%X) \n", ulCurrentTime);
	HxLOG_Print("   ucNumOfEntries(0x%X) \n", ucNumOfBrandId);
	HxLOG_Print("   bFreeCiFlag(%d) \n", bFreeCiFlag);
	HxLOG_Print("========================= \n");
	HxLOG_Print("  bNeedSave (%d) \n", bNeedSave);
	HxLOG_Print("========================= \n\n");
	#endif

	if (bNeedSave != FALSE)
	{
		stSvcInfo.ulTimeStamp = (HUINT32)ulCurrentTime;
		stSvcInfo.ucNumOfEntries = (HUINT32)ucNumOfBrandId;
		VK_memcpy32(stSvcInfo.ausCamBrandId, pusCiBrandId, sizeof(HUINT16)*ucNumOfBrandId);

		if (bFreeCiFlag != FALSE)
		{
			stSvcInfo.ulUserFlag1 |= FreeCiMode;
		}
		else
		{
			stSvcInfo.ulUserFlag1 &= ~FreeCiMode;
		}

		hError = svc_cas_SubSvcDbUpdateServiceInfo(hSvcHandle, &stSvcInfo);
		if(hError != ERR_OK)
		{
			HxLOG_Error("svc_cas_SubSvcDbUpdateServiceInfo() Fail ! \n");
			return ERR_FAIL;
		}

	}

	return ERR_OK;
}

static HERROR local_CiMgr_ProcSdtChanged(CI_SDT_CHANGED_PARAM *pParam)
{
	HBOOL			bIsNeedShun = FALSE;
	CI_Context_t		*pCiContext = NULL;
	CAS_CiDscInfo_t	*pDscInfo = NULL;

	if (pParam == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	if (pParam->pucTbl== NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	pCiContext = local_CiMgr_GetContextByAction(pParam->hAction);
	if (pCiContext == NULL)
	{
		HxLOG_Error("invalid context\n");
		OxCAS_Free(pParam->pucTbl);
		pParam->pucTbl = NULL;
		return ERR_FAIL;
	}

	local_CiMgr_ParseSdtForCiPlus(pCiContext, pParam->pucTbl);

	OxCAS_Free(pParam->pucTbl);
	pParam->pucTbl = NULL;

	// Get Sdt Raw Pool
	HxLOG_Info("ActionHandle(%x), UniqueId(%x)\n", pParam->hAction, pParam->usUniqueId);

	// Parsing.
	if(local_CiMgr_GetCiDescInfo(pCiContext, &pDscInfo) != ERR_OK)
	{
		HxLOG_Error("get description info failed\n");

		/*
			2가지 경우 ..

			1. SDT Timeout.
			2. Service Id에 해당하는 SLD가 없음. 이런 경우는 Multifeed...
			3. SDT에 SLD는 존재하나 CI Descriptor가 존재하지 않음.

			이 3 가지 경우에 대해서 일단 모두 timeout으로 처리 함.
		*/

		pCiContext->eSdtTblState = eCASSI_Table_Timeout;
		pCiContext->bSvcShunned = FALSE;

		if(pCiContext->pCam != NULL)
		{
			HxLOG_Info("Ts Path going to Passthrough\n");
			CI_SetPlusMode(pCiContext->pCam->usSlotId, CI_PLUS_FREE_MODE);

			#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
			local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pCiContext->hAction), pCiContext->pCam->usSlotId);
			#else
			svc_cas_DevCiSetTsPath(pCiContext->pCam->usSlotId, eCAS_DEV_CI_PASSTHROUGH);
			#endif
		}
		else
		{
			HxLOG_Print("pCam is NULL... \n");
		}

		return ERR_OK;
	}

	//pCiContext->eSdtTblState = eSdtTblState;

	#if defined(CONFIG_DEBUG)
	HxLOG_Info("Unique ID(0x%x) Svc ID (0x%x) (%s) \n", pCiContext->usUniqueId, pCiContext->usSvcId, local_CiMgr_MakeStrSiTblState(pCiContext->eSdtTblState));
	#endif

	/* Check unique id, svc id */
	if(pCiContext->usUniqueId != pParam->usUniqueId || pCiContext->usSvcId != pDscInfo->usSvcId)
	{
		HxLOG_Error("\n");
	}

	if(pCiContext->pCam == NULL)
	{
		#if defined(CONFIG_DEBUG)
		HxLOG_Error("eActType(%s) cam instance not allocated. \n", local_CiMgr_MakeStrActionType(pCiContext->eActType));
		#endif

		return ERR_OK;
	}
	else if (pCiContext->eChType != eCasChannel_FTA)
	{
		if(local_CiMgr_ProcCheckShunCondition(pCiContext->pCam->usSlotId, pDscInfo, &bIsNeedShun) != ERR_OK)
		{
			HxLOG_Error("\n");
		}
	}
	HxLOG_Info("bIsNeedShun (%d)\n", bIsNeedShun);

	/* Host Service Shunning 시의 컨트롤 구현 방식은 다음과 같이 여러 가지가 있을 수 있겠으나 아래에 명시된 방식을 사용하도록 한다.

	   1. A/V mute control --> LF7300과 같이 2-chip 구조에서는 서로 박자가 맞아야 하고 플랫폼마다 항시 체크가 필요해서 안좋다.
	   2. stop ca_pmt and DES/AES descrambling --> 나쁘지 않은 방식이나 shunning 조건 변경시에 오버로드가 있어서 안좋다.
	   3. ci path control --> 젤 좋을 듯... 여기 구현된 컨트롤 방식은 이것이다!

	*/
	if(bIsNeedShun == TRUE)
	{
		#if defined(CONFIG_DEBUG)
		HxLOG_Info("\n --- Host Service Shunning ON ---\n\n"); // 중요 로그이므로 항시 찍어 주자.
		#endif

		pCiContext->bSvcShunned = TRUE;
		CI_SetPlusMode(pCiContext->pCam->usSlotId, CI_PLUS_PROTECTED_MODE);

		if(pCiContext->pCam != NULL)
		{
			HxLOG_Info("Ts Path going to Bypass\n");
			#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
			local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pCiContext->hAction), -1);
			#else
			svc_cas_DevCiSetTsPath(pCiContext->pCam->usSlotId, eCAS_DEV_CI_BYPASS);
			#endif
		}
	}
	else
	{
		pCiContext->bSvcShunned = FALSE;
		CI_SetPlusMode(pCiContext->pCam->usSlotId, CI_PLUS_FREE_MODE);

		if((pCiContext->pCam != NULL))
		{
			HxLOG_Info("Ts Path going to Passthrough\n");

			#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
			local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pCiContext->hAction), pCiContext->pCam->usSlotId);
			#else
			svc_cas_DevCiSetTsPath(pCiContext->pCam->usSlotId, eCAS_DEV_CI_PASSTHROUGH);
			#endif
		}
	}

	// save sdt to db
	local_CiMgr_UpdateSdt2DB(pCiContext->hSvc, pDscInfo->bFreeCiFlag, pDscInfo->ucNumOfBrandId, pDscInfo->usCiBrandId);

	return ERR_OK;
}

static HBOOL local_CiMgr_CheckClearCaPmtCondition(CI_Context_t *pCiContext, HBOOL bNeedCam)
{
	HBOOL bClearCaPmt = FALSE;

	// 마지막에 보낸 CA PMT가 clear CA PMT이다. 연속으로 같은 Clear Ca Pmt를 보낼 필요가 없는데 보내는 경우가 있다.(CAM이 오동작 하는 경우가 있네)
	if(pCiContext->pCam->bClearCaPmt == TRUE)
	{
		HxLOG_Print("Already send clear ca pmt...(slot : %d)\n", pCiContext->pCam->usSlotId);
		return FALSE;
	}

	// TODO: 이 부분을 case by case가 아니라 한번에 정리 할 수 있는 조건을 찾아보자.
	if(bNeedCam == TRUE)
	{
		// 동일한 CI+ 서비스의 Action 전환시 CA PMT가 동일할 경우 URI값을 전달해 주지 않는 CAM이 있으며 이 때 URI timeout으로 인해 AV가 Mute된다.
		// 따라서 Action이 변경되었음을 알리는 용도로 Clear CA PMT를 전달한다.
		// EX> Live + TSR Rec / instant recording -> Chase / TSR Play 경우
		if( (pCiContext->eActType == eCAS_ActionType_Tsr_Play)
			|| (pCiContext->eActType == eCAS_ActionType_Playback) )
		{
			bClearCaPmt = TRUE;
		}

		// CI+ service Live에서 FTA or descrambled contents의 playback 시작 -> 종료 후의 Live service가 동일한 CI+ service일시 URI timeout
		// Service stop시마다 CAM에 공유된 service가 0개일 때로 조건처리 시도하였으나 channel change마다 보내게 되어서 아래와 같이 처리함.
		if( (local_CiMgr_GetNumberOfSharedSvc(pCiContext) == 0)
			&& (pCiContext->hSvc != HANDLE_NULL)
			&& (pCiContext->hSvc == pCiContext->pCam->hLastSvc) )
		{
			bClearCaPmt = TRUE;
		}
	}
	else
	{
		// 동일한 CI+ 서비스의 Action 전환시 CA PMT가 동일할 경우 URI값을 전달해 주지 않는 CAM이 있으며 이 때 URI timeout으로 인해 AV가 Mute된다.
		// 따라서 Action이 변경되었음을 알리는 용도로 Clear CA PMT를 전달한다.
		// EX> Chase / TSR Play -> Live + TSR Rec / instant recording 경우
		if( (pCiContext->eActType == eCAS_ActionType_Tsr_Play)
			|| (pCiContext->eActType == eCAS_ActionType_Playback)
			/*|| (pCiContext->eActType == eCAS_ActionType_Recording) */)	// 동일 event에 대해 연속으로 recording할시 record contents에 URI 정보 미포함
		{
			bClearCaPmt = TRUE;
		}
	}

	if(bClearCaPmt == TRUE)
	{
		pCiContext->pCam->bClearCaPmt = TRUE;
	}

	return bClearCaPmt;
}

static void local_CiMgr_SendClearCaPmt(CI_SLOT_ID usSlotId, CI_Context_t *pstContext)
{
	HUINT8				aucCaPmt[1024];
	HUINT8				aucLengthField[3];
	HUINT8				ucLengthFieldSize;
	HUINT16				usIdx = 0, i = 0;
	HUINT16 			usTotalLen = 0;
	SvcCas_PmtInfo_t		*pstPmtInfo;
	SvcCas_PmtElement_t	*pPmtElement;

	// CI+ CAM이 아닌데 보낼 필요는 없지...
	if(CI_IsCiPlusCam(usSlotId) > 0)
	{
		pstPmtInfo = pstContext->stProgInfo.pPmtInfo;

		//calculate ca pmt total length
		usTotalLen = 9 + (pstPmtInfo->ulNumOfElement * 5);
		ucLengthFieldSize = local_CiMgr_GetLengthField(aucLengthField, usTotalLen - 3); // -ca_pmt_tag 3
		usTotalLen += ucLengthFieldSize;

		/* ca_pmt_tag (24 bits) */
		aucCaPmt[usIdx++] = (HUINT8)((CI_CA_PMT >> 16) & 0xFF);
		aucCaPmt[usIdx++] = (HUINT8)((CI_CA_PMT >> 8) & 0xFF);
		aucCaPmt[usIdx++] = (HUINT8)(CI_CA_PMT & 0xFF);

		/* length field */
		HxSTD_memcpy(&aucCaPmt[usIdx], aucLengthField, ucLengthFieldSize);
		usIdx += ucLengthFieldSize;

		/* ca_pmt_list_managrment (8 bits) */
		aucCaPmt[usIdx++] = (HUINT8)CI_CPLM_ONLY;

		/* program number 16 bits*/
		aucCaPmt[usIdx++]  = (HUINT8)((pstPmtInfo->usServiceId  >> 8) & 0xFF);
		aucCaPmt[usIdx++]  = (HUINT8)(pstPmtInfo->usServiceId & 0xFF);

		/* reserved (2 bits) + version_number (5 bits) + current_next_indicator (1 bit) */
		aucCaPmt[usIdx] = (HUINT8)((pstPmtInfo->ucVerNum << 1) & 0x3e);
		aucCaPmt[usIdx++] |= (HUINT8)(pstPmtInfo->bCurNextInd & 0x01);

		/* reserved (4 bits) + program_info_length (12 bits) */
		aucCaPmt[usIdx++] = 0;
		aucCaPmt[usIdx++] = 0;

		for(i = 0; i < pstPmtInfo->ulNumOfElement && i < CAS_MAX_NUM_OF_ES; i++)
		{
			pPmtElement = &pstPmtInfo->stElementInfo[i];

			/* stream_type (8 bits) */
			aucCaPmt[usIdx++] = pPmtElement->ucStreamType;

			/* reserved (3 bits) + elementary_PID (13 bits) */
			aucCaPmt[usIdx++] = (HUINT8)((pPmtElement->usPid >> 8) & 0x1f);
			aucCaPmt[usIdx++] = (HUINT8)(pPmtElement->usPid & 0xff);

			/* reserved (4 bits) + ES_info_length (12 bits) */
			aucCaPmt[usIdx++] = 0;
			aucCaPmt[usIdx++] = 0;
		}

		if(CI_SendCaPmt(usSlotId, aucCaPmt, usTotalLen) != ERR_OK)
		{
			HxLOG_Error("[%d]-CAM CI_SendCaPmt() error\n", usSlotId);
		}
	}
}

HERROR local_CiMgr_SendOpListChangeApdu(CI_Context_t *pstCiContext, SvcCas_DbSvcInfo_t *pstSvcInfo)
{
	HUINT8 	aucBindedCamId[8];
	HUINT8 	aucTempEmptyCamId[8] = {0,};
	int 	i;

	if (pstSvcInfo == NULL)
	{
		return ERR_FAIL;
	}

	// OP List를 빠져 나왔을 때 Operator_Exit APDU 전송 시나리오
	if(HxSTD_memcmp(s_aucCiPrvSvcCamId, aucTempEmptyCamId, 8) == 0)
	{
		// 이전 service가 Operator Profile이랑 상관없는 service였으면 아무것도 하지 않아도 됨
	}
	else
	{
		if(HxSTD_memcmp(s_aucCiPrvSvcCamId, pstSvcInfo->aucCamId, 8) == 0)
		{
			// 이전 svc CAM ID와 현재 svc CAM ID를 비교해서 같으면 아무것도 하지 않음
		}
		else
		{
			for(i=0; i<	CARM_MAX_CISLOT_NUMBER; i++)
			{
				if(CI_GetBindedCamId(i, aucBindedCamId) == ERR_OK)
				{
					if(HxSTD_memcmp(s_aucCiPrvSvcCamId, aucBindedCamId, 8) == 0)
					{
						// CAM이 꽂혀있는 slot에 대해 새로 진입하는 svc와 이전 svc에 대한 OP CAM ID가 다르며,
						// 이전 svc에 대한 OP CAM ID가 해당 slot의 CAM ID와 동일할 때 Exit를 날림.
						CI_SendOperatorExitBySlotId(i);
						break;
					}
				}
			}
		}
	}

	// 새로 OP List에 진입 여부를 판단하고, 진입 하였을 경우 Operator_Status_Req APDU 전송 시나리오
	if(HxSTD_memcmp(pstSvcInfo->aucCamId, aucTempEmptyCamId, 8) == 0)
	{
		// 현재 service가 Operator Profile이랑 상관없는 service였으면 아무것도 하지 않아도 됨
	}
	else
	{
		if(HxSTD_memcmp(s_aucCiPrvSvcCamId, pstSvcInfo->aucCamId, 8) == 0)
		{
			// 이전 svc CAM ID와 현재 svc CAM ID를 비교해서 같으면 아무것도 하지 않음
		}
		else
		{
			// 현재 service와 이전 service가 서로 다른 OP list 이므로 req APDU를 전송해야 함.
			for(i=0; i<	CARM_MAX_CISLOT_NUMBER; i++)
			{
				if(CI_GetBindedCamId(i, aucBindedCamId) == ERR_OK)
				{
					if(HxSTD_memcmp(pstSvcInfo->aucCamId, aucBindedCamId, 8) == 0)
					{
						CI_SendOperatorStatusReqBySlotId(i);
						break;
					}
				}
			}
		}
	}

	// APDU 전송작업 완료 후 aucCiPrvSvcCamId를 현재 service로 바꿔 줌
	HxSTD_memcpy(s_aucCiPrvSvcCamId, pstSvcInfo->aucCamId, 8);

	return ERR_OK;
}


#if defined(CI_PLUS_PVR_ENABLE)
static HERROR local_CiMgr_ProcUpdateCaPvrInst(CI_UPDATE_CA_PVR_INST_PARAM *pUpdateCaPvrInst)
{
	CI_CamInst_t *pCam = NULL;

	if (pUpdateCaPvrInst == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	if (pUpdateCaPvrInst->usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		if (pUpdateCaPvrInst->ausCaPvrCaSysId != NULL)
		{
			OxCAS_Free(pUpdateCaPvrInst->ausCaPvrCaSysId);
		}
		if (pUpdateCaPvrInst->aucCaRecMode != NULL)
		{
			OxCAS_Free(pUpdateCaPvrInst->aucCaRecMode);
		}
		HxLOG_Error("input slot id (%d)\n", pUpdateCaPvrInst->usSlotId);
		return ERR_FAIL;
	}

	HxLOG_Info("SlotId (%d), CaInfoCnt (%d), hCasDevice (0x%x)\n",
					pUpdateCaPvrInst->usSlotId, pUpdateCaPvrInst->usCaPvrInfoCnt, s_astCiCam[pUpdateCaPvrInst->usSlotId].hCasDevice);

	pCam = &s_astCiCam[pUpdateCaPvrInst->usSlotId];

	/* <TODO_CI_PLUS> : pCam 의 usCaPvrInfoCnt, ausCaPvrCaSysId, aucCaRecMode 업데이트하고... */

	if (pUpdateCaPvrInst->ausCaPvrCaSysId != NULL)
	{
		OxCAS_Free(pUpdateCaPvrInst->ausCaPvrCaSysId);
	}
	if (pUpdateCaPvrInst->aucCaRecMode != NULL)
	{
		OxCAS_Free(pUpdateCaPvrInst->aucCaRecMode);
	}

	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
static HERROR local_CiMgr_OperateCCv2RecordStop(CI_MANAGE_CA_INST_PARAM *pParam, CI_Context_t *pCiContext)
{
	HERROR				hErr;

	HxLOG_Info("(+)Enter!!\n");

	// 1. 현재 여기에 진입된 이유가 recording 때문인지 판단 하고, recording 관련이 아니라면 그냥 진행. recording 관련일때만 아래 시나리오 타도록.
	if (pCiContext->eActType == eCAS_ActionType_Recording || pCiContext->eActType == eCAS_ActionType_Tsr_Rec )
	{
		HxLOG_Info("CI Recording Stop Operation...\n");
		// 2. 현재 삽입된 path를 타는 CAM이 CI+ ver 1.3 지원 하는 CAM인지 판단 필요. 1.3 지원 캠인경우 아래 단계 진행
		if (CI_IsCiPlusCam((unsigned short)pParam->unSlotId) > 0 // CI+ CAM 이고,
				&& CI_GetCcResVersion((unsigned short)pParam->unSlotId) == CI_CC_RES_V2_ID)	// 해당 Slot에 CI_CC_RES_V2_ID로 열린 Session이 있을 경우.
		{
			// 3. PIN capa 및 URI information 획득 후 recording procedure 로 진행이 필요한 것인지 판단
			HUINT8						ucCcRecordingStatus = (HUINT8)CI_CC_RECORDING_NOT_CONTROLED;

			HxLOG_Info("Enter CI+ CCv2 Recording Stop Process...\n");

			hErr = CI_GetRecordingStatus((unsigned short)pParam->unSlotId, &ucCcRecordingStatus);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("CI_GetRecordingStatus() failed\n");
				return ERR_FAIL;
			}

			HxLOG_Info("ucCcRecordingStatus: [%02x]\n", ucCcRecordingStatus);

			if(ucCcRecordingStatus == (HUINT8)CI_CC_RECORDING_STARTED)
			{
				HxLOG_Info("Record Stop...\n");
				hErr = CI_RecordStop((unsigned short)pParam->unSlotId, pCiContext->usSvcId);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("\n");
					return ERR_FAIL;
				}
			}
		}
	}

	HxLOG_Info("(-)Leave!!\n");

	return ERR_OK;
}

static HERROR local_CiMgr_OperateCCv2RecordProcess(CI_MANAGE_CA_INST_PARAM *pParam, CI_Context_t *pCiContext)
{
	CI_OPERATING_MODE	eOperatingMode;
	DxCopyrightControl_CiPlusCcRecCtrl_t		stCiPlusRecCtrl;
	DxCopyrightControl_t			*pstDrmInfo;
	HERROR				hErr;
/*
	if (pCiContext->eChType != eCasChannel_Scramble)
	{
		// CI+ content가 아니면 CCv2 Record 시나리오와 상관 없이 일반 recording process를 타도록 함.
		// 만약 CI+ content 인데도 이 루틴을 타거나, eCasChannel_Unknown이 여기로 들어온다면 CASSI_DefineChannelType 함수처럼 PMT info를 보고 처리 해야 함.
		HxLOG_Info("The service is not CI+ content. eChType[%02x].\n", pCiContext->eChType);

		stCiPlusRecCtrl.eCiPlusCcRecInfo = eDxCopyrightControl_CiPlusCcRecInfo_RecordOk;
		hErr = local_CiMgr_MakeCiDrmInfo(eDxCopyrightControl_CiPlusDrmType_RecCtrl, (void *)&stCiPlusRecCtrl, &pstDrmInfo, eDxCopyrightControl_CiPlusDrmWriteInfo_None);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("local_CiMgr_MakeCiDrmInfo() failed\n");
			return ERR_FAIL;
		}

		hErr = svc_cas_MgrSubUpdateCasEtcEventData(pParam->hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM, eSEVT_CAS_DRM_SIGNALLED, 0, 0, (void*)pstDrmInfo, sizeof(DxCopyrightControl_t));

		if(hErr != ERR_OK)
		{
			HxLOG_Error("\n");
			return ERR_FAIL;
		}

		return ERR_OK;
	}
*/
	// 1. 현재 여기에 진입된 이유가 recording 때문인지 판단 하고, recording 관련이 아니라면 그냥 진행. recording 관련일때만 아래 시나리오 타도록.
	if (pCiContext->eActType == eCAS_ActionType_Recording || pCiContext->eActType == eCAS_ActionType_Tsr_Rec )
	{
		// 2. 현재 삽입된 path를 타는 CAM이 CI+ ver 1.3 지원 하는 CAM인지 판단 필요. 1.3 지원 캠인경우 아래 단계 진행
		if (CI_IsCiPlusCam((unsigned short)pParam->unSlotId) > 0 // CI+ CAM 이고,
				&& CI_GetCcResVersion((unsigned short)pParam->unSlotId) == CI_CC_RES_V2_ID)	// 해당 Slot에 CI_CC_RES_V2_ID로 열린 Session이 있을 경우.
		{
			// 3. PIN capa 및 URI information 획득 후 recording procedure 로 진행이 필요한 것인지 판단
			CI_PIN_CAPA_INFO 	stPinCapaInfo;
			CIPLUS_UriInfo_t	stUriInfo;

			HxLOG_Info("Enter CI+ CCv2 Recording Process...\n");

			hErr = CI_GetPinCapaInfo((unsigned short)pParam->unSlotId, &stPinCapaInfo);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("CI_GetPinCapaInfo() failed\n");
				return ERR_FAIL;
			}

			hErr = CI_GetUriInfo((unsigned short)pParam->unSlotId, &stUriInfo);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("CI_GetUriInfo() failed\n");
				return ERR_FAIL;
			}

			if(((stPinCapaInfo.bPinCapaSet == TRUE) && (stPinCapaInfo.enPinCapa != CI_PIN_CAPABILITY_NONE)) || (stUriInfo.ucEmi == CI_CC_URI_EMI_COPY_NEVER))
			{
				stCiPlusRecCtrl.eCiPlusCcRecInfo = eDxCopyrightControl_CiPlusCcRecInfo_RecordWait;
			}
			else
			{
				stCiPlusRecCtrl.eCiPlusCcRecInfo = eDxCopyrightControl_CiPlusCcRecInfo_RecordOk;
			}
		}
		else
		{
			stCiPlusRecCtrl.eCiPlusCcRecInfo = eDxCopyrightControl_CiPlusCcRecInfo_RecordOk;
		}

		HxLOG_Info("stCiPlusRecCtrl.eCiPlusCcRecInfo:[%02x]\n", stCiPlusRecCtrl.eCiPlusCcRecInfo);

		hErr = local_CiMgr_MakeCiDrmInfo(eDxCopyrightControl_CiPlusDrmType_RecCtrl, (void *)&stCiPlusRecCtrl, &pstDrmInfo, eDxCopyrightControl_CiPlusDrmWriteInfo_None);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("local_CiMgr_MakeCiDrmInfo() failed\n");
			return ERR_FAIL;
		}

		/* TV manager (ap_recorder 쪽으로 녹화 정보를 올려 줌. */

		hErr = svc_cas_MgrSubUpdateCasEtcEventData(pParam->hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM, eSEVT_CAS_DRM_SIGNALLED, 0, 0, (void*)pstDrmInfo, sizeof(DxCopyrightControl_t));
		if(hErr != ERR_OK)
		{
			HxLOG_Error("\n");
			return ERR_FAIL;
		}

		/* CC resource v2의 Record Start Protocol을 이용하여 Rec 시작 해야 할 경우 CAM쪽으로 해당 내용을 전송 */
		/*
			DTVLab 인증 5.2절 FTA-CA Transitions while recording 테스트 내용을 위해 eDxCopyrightControl_CiPlusCcRecInfo_RecordOk 경우 추가.
			(FTA-CA Transition 시 이미 FTA content를 녹화중이라 변경구간에서 새롭게 record_start message가 CICAM으로 보내지지 않아,
			이후 시간에 실제 DOT=1이 들어오는 구간에서 analogue output이 disable되지 않는 문제가 발생하는 문제 해결)
		*/
		if ((stCiPlusRecCtrl.eCiPlusCcRecInfo == eDxCopyrightControl_CiPlusCcRecInfo_RecordWait) || (stCiPlusRecCtrl.eCiPlusCcRecInfo == eDxCopyrightControl_CiPlusCcRecInfo_RecordOk))
		{
			HUINT8	ucCcRecordingStatus = (HUINT8)CI_CC_RECORDING_NOT_CONTROLED;

			hErr = CI_GetRecordingStatus((unsigned short)pParam->unSlotId, &ucCcRecordingStatus);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("CI_GetRecordingStatus() failed\n");
				return ERR_FAIL;
			}

			HxLOG_Info("ucCcRecordingStatus: [%02x]\n", ucCcRecordingStatus);

			// 이 조건 테스트는 CICAM으로 record start 를 보내는 CiCc_SacTask() 타스크에서도 하고 있어 여기서 안해주어도 된다.
			// 오히려 5.4절 test시 릴리즈모드에서 녹화시작 안되는 문제가 발생한다. 그래서 이 조건 체크를 없애는 걸로....
			//if(ucCcRecordingStatus != (HUINT8)CI_CC_RECORDING_STARTED)
			{
				switch (pCiContext->eActType)
				{
					case eCAS_ActionType_Recording:
						eOperatingMode = CI_OPERATING_MODE_UNATTENDED_RECORDING;
						break;
					case eCAS_ActionType_Tsr_Rec:
						eOperatingMode = CI_OPERATING_MODE_TIMESHIFT;
						break;
					default:		// 이쪽에 진입 할수도 없고, 진입하면 안됨!!
						HxLOG_Error("Wrong CI Action Type!!\n");
						return ERR_FAIL;
				}

				hErr = CI_RecordStart((unsigned short)pParam->unSlotId, pParam->hAction, eOperatingMode, pCiContext->usSvcId);
				if(hErr != ERR_OK)
				{
					HxLOG_Error("\n");
					return ERR_FAIL;
				}
			}
		}
	}

	return ERR_OK;
}
#endif		// #if defined(CONFIG_MW_CI_PLUS_CC_V_2)

#endif		// #if defined(CONFIG_MW_CI_PLUS)

#if defined(CONFIG_SUPPORT_RETRY_CAPMT)
////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_RETRY_SERVICE___
////////////////////////////////////////////////////////////

static void local_CiMgr_CallbackRetrySvc(unsigned long ulTimerId, void *pvParam) // TODO : Retry Scheme 정리.
{
	CI_RETRY_SVC_PARAM *pRetrySvc;

	/* Not used */
	ulTimerId = 0;

	pRetrySvc = (CI_RETRY_SVC_PARAM *)pvParam;

	HxLOG_Info("hAction[0x%08x], hSvc[0x%08x]...\n", pRetrySvc->hAction, pRetrySvc->hSvc);

	if (local_CiMgr_SendEvent(CI_RETRY, (void *)pRetrySvc, (unsigned long)sizeof(CI_RETRY_SVC_PARAM)) != ERR_OK)
	{
		HxLOG_Critical("Critical Error!!!\n");
	}

	return;
}

static HERROR local_CiMgr_StartRetrySvc(CI_Context_t *pCiContext)
{
	CI_RETRY_SVC_PARAM retrySvc;

	if (pCiContext == NULL)
	{
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	if (pCiContext->ulRetryTimerId != INVALID_TIMER_ID)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	retrySvc.hAction = pCiContext->hAction;
	retrySvc.hSvc = pCiContext->hSvc;

	HxLOG_Info("retry -- hSvc[0x%08x], hAction[0x%08x]\n", retrySvc.hSvc, retrySvc.hAction);

	if (VK_TIMER_EventEvery(CIMGR_RETRY_PERIOD, local_CiMgr_CallbackRetrySvc, (void *)&retrySvc, sizeof(CI_RETRY_SVC_PARAM), &pCiContext->ulRetryTimerId) != VK_OK)
	{
		pCiContext->ulRetryTimerId = INVALID_TIMER_ID;
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_CiMgr_StopRetrySvc(CI_Context_t *pstCiContext)
{
	if (pstCiContext == NULL)
	{
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	if (pstCiContext->ulRetryTimerId == INVALID_TIMER_ID)
	{
		return ERR_OK;
	}

	if (VK_TIMER_Cancel(pstCiContext->ulRetryTimerId) != VK_OK)
	{
		HxLOG_Error("\n");
	}

	pstCiContext->ulRetryTimerId = INVALID_TIMER_ID;

#if defined(_LIMIT_CI_RETRY_COUNT_)
	pstCiContext->ucRetryCnt = 0;
#endif

	return ERR_OK;

}

static HERROR local_CiMgr_ProcRetrySvc(CI_RETRY_SVC_PARAM *pRetrySvc)
{
	CI_Context_t *pCiContext = NULL;
	HUINT32 nResult = 0;
	HBOOL bIsNeedRetry = FALSE;
	Handle_t hAction = HANDLE_NULL;
	HUINT32 i = 0;
	HBOOL bVideo = FALSE, bAudio = FALSE, bAState = FALSE, bVState = FALSE;

#if defined(SUPPORT_SMART_CI_MMI_PINCODE)
	unsigned long IsWaiting = 0;
#endif

	if (pRetrySvc == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	hAction = pRetrySvc->hAction;
	if(HANDLE_NULL == hAction)
	{
		HxLOG_Error("invalid handle\n");
		return ERR_FAIL;
	}

	pCiContext = local_CiMgr_GetContextByAction(hAction);
	if (pCiContext == NULL) {
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	HxLOG_Info("pRetrySvc->hAction(0x%08x)\n", hAction);

	if (pCiContext->pCam == NULL)
	{
		nResult = local_CiMgr_StopRetrySvc(pCiContext);
		if (nResult != ERR_OK)
		{
			HxLOG_Error("\n");
		}
		return ERR_OK;
	}

#if defined(_LIMIT_CI_RETRY_COUNT_)
	if (pCiContext->ucRetryCnt >= CIMGR_MAX_NUM_OF_RETRY)
	{
		nResult = local_CiMgr_StopRetrySvc(pCiContext);
		if (nResult != ERR_OK)
		{
			HxLOG_Error("\n");
		}
		return ERR_OK;
	}
	pCiContext->ucRetryCnt++;
#endif

	if (pCiContext->ulRetryTimerId == INVALID_TIMER_ID)
	{
		HxLOG_Error("\n");
		return ERR_OK;
	}

	if (pCiContext->pCam->caPmtOkDsc.pucRaw != NULL)
	{
		if (pCiContext->eActType == eCAS_ActionType_Live || pCiContext->eActType == eCAS_ActionType_Playback)
		{
			/* TV/RADIO 구분?....Video Pid 존재 시 Video Decoder State Check. Audio Pid 존재 시 Audio Decoder State Check. */
			for(i=0; i<pCiContext->pCam->ulNumOfPidForCaPmt; i++)
			{
				if (pCiContext->pCam->PidInfoForCaPmt[i].usPid != PID_NULL)
				{
					if (pCiContext->pCam->PidInfoForCaPmt[i].esType == eEsType_Video)
						bVideo = TRUE;

					if (pCiContext->pCam->PidInfoForCaPmt[i].esType == eEsType_Audio)
						bAudio = TRUE;
				}
			}

			if (bVideo == TRUE)
			{
				nResult = svc_cas_DevVideoIsRunning(0, &bVState);
				if (nResult == ERR_OK && bVState != TRUE)
				{
					bIsNeedRetry = TRUE;
				}
			}

			#if 0
			if (bAudio == TRUE)
			{
				nResult = CARM_AUDIO_GetDecoderState(0, &bAState);
				if (nResult == ERR_OK && bAState != TRUE)
				{
					bIsNeedRetry = TRUE;
				}
			}
			#endif
			HxLOG_Info("Retry?(%d) , video(%d):%d, audio(%d):%d \n", bIsNeedRetry, bVideo, bVState, bAudio, bAState);
		}
		else if (pCiContext->eActType == eCAS_ActionType_Recording)
		{

		}
		else if (pCiContext->eActType == eCAS_ActionType_Tsr_Rec)
		{

		}
		else if (pCiContext->eActType == eCAS_ActionType_Tsr_Play)
		{

		}
		else
		{

		}

	}


	if (bIsNeedRetry == TRUE)
	{
		if (CARM_CI_CheckModule(pCiContext->pCam->usSlotId) == eCAS_DEV_CI_EXTRACTED)
		{
			return ERR_OK;
		}

#if defined(SUPPORT_SMART_CI_MMI_PINCODE)
		nResult = ERR_OK;
		IsWaiting = 0;
		// Quinny : If mmi pincode is popup, then this could be cause of mmi window show-again problem.
		// usSsNo =  CiCas_GetSsNo(pCiSvc->pCam->usSlotId);
		nResult = CiMmi_CheckWaitingUserInput(pCiContext->pCam->usSlotId, &IsWaiting);
		/*
			BS_ADD
			g_bMmiOpened, MMI Session Open 시 CaPmt Send하지 않음
		*/
		if (!IsWaiting && nResult == ERR_OK && g_bMmiOpened == FALSE)
		{
			nResult = local_CiMgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 0, 1);
		}
#else
		nResult = local_CiMgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 0, 1);
#endif

	}
	else
	{
		nResult = local_CiMgr_StopRetrySvc(pCiContext);
		if (nResult != ERR_OK)
		{
			HxLOG_Error("\n");
		}
	}

	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_CI_CHECK_AV)
////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_CHECK_AV___
////////////////////////////////////////////////////////////

static HERROR local_CiMgr_UpdateSlotInfoToDBForSameTp(Handle_t hSvc, HUINT16 usSlotId)
{
	HINT32		i, nCnt;
	Handle_t	*phSvcList;
	SvcCas_DbSvcInfo_t	stSvcInfo;

	if(svc_cas_SubSvcDbGetSameTpSvcHandleListByServiceHandle(hSvc, &nCnt, &phSvcList) != ERR_OK)
	{
		HxLOG_Error("svc_cas_SvcPortDbGetSameTpSvcHandleListByServiceHandle(), hSvc (0x%x) error\n", hSvc);
		return ERR_FAIL;
	}

	for(i = 0; i < nCnt; i ++)
	{
		if(svc_cas_SubSvcDbGetSvcInfoByServiceHandle(phSvcList[i], &stSvcInfo) != ERR_OK)
		{
			HxLOG_Error("svc_cas_SubSvcDbGetSvcInfoByServiceHandle(), hSvc (0x%x) error\n", phSvcList[i]);
			continue;
		}

		//if(stSvcInfo.eInstType != eCasInst_CI || stSvcInfo.usSlotId != usSlotId)
		if(stSvcInfo.eInstType != eCasInst_CI)
		{
			stSvcInfo.eInstType	= eCasInst_CI;
			stSvcInfo.usSlotId	= usSlotId;

			if(svc_cas_SubSvcDbUpdateServiceInfo(phSvcList[i], &stSvcInfo) != ERR_OK)
			{
				HxLOG_Error("svc_cas_SubSvcDbUpdateServiceInfo(), hSvc (0x%x) error\n", phSvcList[i]);
			}
		}
	}

	svc_cas_SubSvcDbFreeServiceList(phSvcList);

	return ERR_OK;
}


static void local_CiMgr_CallbackCheckAv(unsigned long ulTimerId, void *pvParam) // TODO : Retry Scheme 정리.
{
	CI_CHECK_AV_PARAM	*pstCheckAv;
	CI_Context_t		*pstContext;

	pstCheckAv = (CI_CHECK_AV_PARAM *)pvParam;

	pstContext = local_CiMgr_GetContextByAction(pstCheckAv->hAction);
	if (pstContext == NULL)
	{
		HxLOG_Error("pstContext NULL!!\n");
		return;
	}

	if (pstContext->ulCheckAvTimerId != ulTimerId)
	{
		HxLOG_Warning("invalid timer id (%x, %x)\n", pstContext->ulCheckAvTimerId, ulTimerId);
		return;
	}

	if (local_CiMgr_SendEvent(CI_CHECK_AV, (void *)pstCheckAv, (unsigned long)sizeof(CI_CHECK_AV_PARAM)) != ERR_OK)
	{
		HxLOG_Critical("Critical Error!!!\n");
	}
}

static HERROR local_CiMgr_StopCheckAv(CI_Context_t *pstCiContext)
{
	if (pstCiContext->ulCheckAvTimerId != INVALID_TIMER_ID)
	{
		VK_TIMER_Cancel(pstCiContext->ulCheckAvTimerId);
		pstCiContext->ulCheckAvTimerId = INVALID_TIMER_ID;
	}

	pstCiContext->ucCheckAvCount = 0;

	return ERR_OK;
}

static HERROR local_CiMgr_StartCheckAv(CI_Context_t *pstCiContext)
{
	CI_CHECK_AV_PARAM stCheckAv;

	if(pstCiContext->eActType != eCAS_ActionType_Live && pstCiContext->eActType != eCAS_ActionType_Playback)
	{
		HxLOG_Print("check AV with only live or playback action.\n");
		return ERR_OK;
	}

#if 0
	//	현재 instance에서 Live와 Playback에서 underrun이 발생할 때 다른 instance가 Recording 중이라면 시도하지 말자고 정함. -> 아래와 같은 이유로 comment 처리
	/*	동일 CA system ID를 가진 CAM에서 권한이 다른 경우 Recording 서비스로 인해 Live가 instance를 못 빼앗는 현상이 있다.
		예) AlphaCrypt(SKY SD) + SKY CI+ (SKY HD+) 삽입된 상태에서 SKY CI+를 이용하여 Recording...
			이후 다른 SKY HD+ 채널로 이동시 SKY CI+ CAM을 Live에서 이용 안하게 된다. */
	if(local_CiMgr_GetOtherSvcContext(pstCiContext, eCAS_ActionType_Recording) != NULL
#if defined(CONFIG_MW_MM_DELAYED_REC)
		|| local_CiMgr_GetOtherSvcContext(pstCiContext, eCAS_ActionType_Tsr_Rec) != NULL
#endif
		)
	{
		HxLOG_Print("Other CAM has a recording service.\n");
		return ERR_OK;
	}
#endif

	local_CiMgr_StopCheckAv(pstCiContext);

	stCheckAv.hAction = pstCiContext->hAction;
	stCheckAv.hSvc = pstCiContext->hSvc;

	HxLOG_Info("check AV -- hSvc[0x%08x], hAction[0x%08x]\n", stCheckAv.hSvc, stCheckAv.hAction);

	if (VK_TIMER_EventEvery(CIMGR_CHECK_AV_PERIOD, local_CiMgr_CallbackCheckAv, (void *)&stCheckAv, sizeof(CI_CHECK_AV_PARAM), &pstCiContext->ulCheckAvTimerId) != VK_OK)
	{
		HxLOG_Error("VK_TIMER_EventAfter() error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_CiMgr_ProcCheckAv(CI_CHECK_AV_PARAM *pstCheckAv)
{
	HBOOL		bHasVideo = FALSE, bHasAudio = FALSE;
	HBOOL		bVideoDecoding = FALSE, bAudioDecoding = FALSE;
	HBOOL		bIsDescrambleOk = FALSE;
	HUINT32		i = 0;
	Handle_t		hAction = HANDLE_NULL;
	CI_Context_t	*pCiContext = NULL;

	if (pstCheckAv == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	hAction = pstCheckAv->hAction;
	if(HANDLE_NULL == hAction)
	{
		HxLOG_Error("invalid handle\n");
		return ERR_FAIL;
	}

	pCiContext = local_CiMgr_GetContextByAction(hAction);
	if (pCiContext == NULL)
	{
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	HxLOG_Info("pstCheckAv->hAction(0x%08x)\n", hAction);

	if(pCiContext->pCam == NULL)
	{
		HxLOG_Error("pCiContext->pCam is NULL\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CI_PLUS)
	if(CI_IsCiPlusCam(pCiContext->pCam->usSlotId) > 0)
	{
		/*	CCv2 작업으로 PIN 관련된 값들이 reporting되고 있는 상황으로 인해
			아래 방법으로는 AV check가 제대로 이루어 지지 않고 있다.
			CAM authentication의 경우 최초 CAM 삽입시에만 동작을 하니 아래 내용을
			local_CiMgr_ReqUpdateCcResource()로 옮긴 후 BOOL로 관리를 하도록 수정해 두었다. (최소한의 수정)
			추후 비슷한 issue 발생시에는 CC res에서 직접적으로 authentication step을 체크해야 할 수도 있다.
		*/
	#if 0
		switch(pCiContext->pCam->eCcReport)
		{
			case eCiPlus_CcReport_CanNotLoadHostLicenseConstants:
			case eCiPlus_CcReport_HostLicenseConstantsNotSaved:
			case eCiPlus_CcReport_UriVersionNegotiationDone:
			case eCiPlus_CcReport_AuthenticationFailed:
			case eCiPlus_CcReport_OperationFailed:
			case eCiPlus_CcReport_NoResponseFromCam:
				break;

			default:
				// CI+ authenciation 과정이 완료될 때 까지 AV 체크는 대기하도록 하자.
				return ERR_OK;
		}
	#else
		if(pCiContext->pCam->bAuthFinish == FALSE)
		{
			HxLOG_Print("CAM (%d) authentication in progress...\n", pCiContext->pCam->usSlotId);
			return ERR_OK;
		}
	#endif
	}
#endif

	/*	DI에서 video decoding status callback 주기를 5초 단위로 하고 있다.
		이보다 조금 더 늦게 해야 하니 7초 이후부터 확인하도록 하자.
		또한, CAMGR의 구조 변경작업이 필요하다. 위의 authentication 과정 중의 AV check 등의 이유로..*/
	if(++(pCiContext->ucCheckAvCount) < 7)
	{
		return ERR_OK;
	}

	if (pCiContext->pCam->caPmtOkDsc.pucRaw != NULL)
	{
		if (pCiContext->eActType == eCAS_ActionType_Live || pCiContext->eActType == eCAS_ActionType_Playback)
		{
			/* TV/RADIO 구분?....Video Pid 존재 시 Video Decoder State Check. Audio Pid 존재 시 Audio Decoder State Check. */
			for(i=0; i<pCiContext->pCam->ulNumOfPidForCaPmt; i++)
			{
				if (pCiContext->pCam->PidInfoForCaPmt[i].usPid != PID_NULL)
				{
					if (pCiContext->pCam->PidInfoForCaPmt[i].esType == eEsType_Video)
						bHasVideo = TRUE;

					if (pCiContext->pCam->PidInfoForCaPmt[i].esType == eEsType_Audio)
						bHasAudio = TRUE;
				}
			}

			if (bHasVideo == TRUE)
			{
				svc_cas_DevVideoIsRunning(0, &bVideoDecoding);
				if (bVideoDecoding == TRUE)
				{
					bIsDescrambleOk = TRUE;
				}
			}

			#if 0
			if (bHasAudio == TRUE)
			{
				CARM_AUDIO_GetDecoderState(0, &bAudioDecoding);
				if (bAudioDecoding == TRUE)
				{
					bIsDescrambleOk = TRUE;
				}
			}
			#endif

			HxLOG_Info("Check AV? (%d) , HasVideo(%d):Decoding(%d), HasAudio(%d):Decoding(%d)\n", bIsDescrambleOk, bHasVideo, bVideoDecoding, bHasAudio, bAudioDecoding);
		}
	}

	if(bIsDescrambleOk == FALSE)
	{
		HUINT32 ulAdditionalCount = 0;

		/*	CI+ CAM (HD+)의 경우 instance connect가 일어난 상태에서 CAM 삽입시 AV가 터지는데 약 12초 정도가 필요하다.
			정상적인 방법은 아니지만, CAS scenario를 수정할려면 대공사가 필요하다.	*/
		if(s_abCheckAvMoreCount[pCiContext->pCam->usSlotId] == TRUE)
			ulAdditionalCount = 5;

		if(pCiContext->ucCheckAvCount >= (CIMGR_CHECK_AV_COUNT + ulAdditionalCount))
		{
			HxLOG_Warning("AV decoding failed. Try next instance...\n");
			s_abCheckAvMoreCount[pCiContext->pCam->usSlotId] = FALSE;
			local_CiMgr_StopCheckAv(pCiContext);
			svc_cas_MgrSubUpdateCasMessage(hAction, eCAS_MSG_TYPE_Fail, eDxCAS_GROUPID_CAM, NULL, 0);
		}
	}
	else
	{
		/*	bIsDescrambleOk값이 TRUE가 될 수 있는 조건은 eCAS_ActionType_Live || eCAS_ActionType_Playback 일 때 이다. */
		if(pCiContext->eActType == eCAS_ActionType_Live)
		{
			SvcCas_DbSvcInfo_t stSvcInfo;

			if(svc_cas_SubSvcDbGetSvcInfoByServiceHandle(pCiContext->hSvc, &stSvcInfo) != ERR_OK)
			{
				HxLOG_Error("svc_cas_SubSvcDbGetSvcInfoByServiceHandle(), hSvc (0x%x) error\n", pCiContext->hSvc);
			}

			if(stSvcInfo.eInstType != eCasInst_CI || stSvcInfo.usSlotId != pCiContext->pCam->usSlotId)
			{
				stSvcInfo.eInstType	= eCasInst_CI;
				stSvcInfo.usSlotId	= pCiContext->pCam->usSlotId;

				if(svc_cas_SubSvcDbUpdateServiceInfo(pCiContext->hSvc, &stSvcInfo) != ERR_OK)
				{
					HxLOG_Error("svc_cas_SubSvcDbUpdateServiceInfo(), hSvc (0x%x) error\n", pCiContext->hSvc);
				}

				if(local_CiMgr_UpdateSlotInfoToDBForSameTp(pCiContext->hSvc, pCiContext->pCam->usSlotId) != ERR_OK)
				{
					HxLOG_Error("local_CiMgr_UpdateSlotInfoToDBForSameTp(), hSvc (0x%x) error\n", pCiContext->hSvc);
				}
			}
		}
		else
		{
			SvcCas_PvrSvcInfo_t stPvrInfo;

			if(svc_cas_SubSvcPvrGetPvrInfoFromMeta(pCiContext->hAction, &stPvrInfo) != ERR_OK)
			{
				HxLOG_Error("svc_cas_SubSvcPvrGetPvrInfoFromMeta() error\n");
			}

			if(stPvrInfo.eInstType != eCasInst_CI || stPvrInfo.usSlotId != pCiContext->pCam->usSlotId)
			{
				stPvrInfo.eInstType	= eCasInst_CI;
				stPvrInfo.usSlotId	= pCiContext->pCam->usSlotId;

				if(svc_cas_SubSvcPvrUpdatePvrInfoToMeta(pCiContext->hAction, &stPvrInfo) != ERR_OK)
				{
					HxLOG_Error("svc_cas_SubSvcPvrUpdatePvrInfoToMeta() error\n");
				}
			}
		}

		s_abCheckAvMoreCount[pCiContext->pCam->usSlotId] = FALSE;
		local_CiMgr_StopCheckAv(pCiContext);
	}

	return ERR_OK;
}


#endif

////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_PROC_EVENTS___
////////////////////////////////////////////////////////////

static HERROR local_CiMgr_ProcStart(CI_ADD_SVC_PARAM *pstAddSvc)
{
	Handle_t				hAction	= HANDLE_NULL;
	Handle_t				hSvc	= HANDLE_NULL;
	HERROR				hErr	= ERR_FAIL;
	CI_Context_t			*pstCiContext = NULL;
	SvcCas_DbSvcInfo_t	stSvcInfo;

	if (pstAddSvc == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	hAction	= pstAddSvc->hAction;
	hSvc	= pstAddSvc->hSvc;

	if(hAction == (Handle_t)NULL)
	{
		HxLOG_Error("invalid handle\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_MM_PVR)
	if(pstAddSvc->eActType != eCAS_ActionType_Playback && pstAddSvc->eActType != eCAS_ActionType_Tsr_Play)
	{
		if (hSvc == (Handle_t)NULL)
		{
			HxLOG_Error("invalid handle\n");
			return ERR_FAIL;
		}
	}
#endif

	pstCiContext = local_CiMgr_GetContextByAction(hAction);
	if(pstCiContext == NULL)
	{
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	pstCiContext->hAction = hAction;
	pstCiContext->hSvc = hSvc;
	pstCiContext->eActType = pstAddSvc->eActType;
	pstCiContext->eState = eCiMgrState_Started;
	pstCiContext->ulRetryTimerId = INVALID_TIMER_ID;

	hErr = local_CiMgr_GetResourceId(pstCiContext);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("Critical Error!!!\n");
	}

#if defined(_LIMIT_CI_RETRY_COUNT_)
	pstCiContext->ucRetryCnt = 0;
#endif

#if defined(CONFIG_MW_CI_PLUS)
	if(pstAddSvc->eActType != eCAS_ActionType_Playback && pstAddSvc->eActType != eCAS_ActionType_Tsr_Play)
	{
		hErr = svc_cas_SubSvcDbGetSvcInfoByServiceHandle(hSvc, &stSvcInfo);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_SubSvcDbGetSvcInfoByServiceHandle() Fail ! \n");
			return ERR_FAIL;
		}

		svc_cas_SubSvcDbGetTsInfoByServiceHandle(hSvc, &pstCiContext->usUniqueId, &pstCiContext->usTsId, &pstCiContext->usOnId);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("svc_cas_SubSvcDbGetTsInfoByServiceHandle() Fail ! \n");
			return ERR_FAIL;
		}

		pstCiContext->usSvcId = stSvcInfo.usSvcId;

		#if defined(CONFIG_MW_CI_PLUS_V_1_3)
		// Operator Profile 시나리오 중 OP list 이동에 관한 시나리오.
		if(pstAddSvc->eActType == eCAS_ActionType_Live)
		{
			// TODO: Operator Profile Resource가 열려 있을 경우에만 OpListChangeApdu를 수행
			// 아래 조건문 등으로 처리해야 함.
			// if (CI_IsCiPlusCam(unSlotId) > 0 && CI_GetOpResVersion(unSlotId) == CI_OP_RES_V1_ID)

			/* local_CiMgr_SendOpListChangeApdu(pstCiContext, &stSvcInfo); */
		}
		#endif
	}
	#if defined(CONFIG_MW_MM_PVR)
	else
	{
		SvcCas_PvrSvcInfo_t pvr_info;

		hErr = svc_cas_SubSvcPvrGetService(hAction, &pvr_info);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("MWC_PVR_GetService() failed\n");
			return ERR_FAIL;
		}

		pstCiContext->usUniqueId = svc_cas_SubSvcDbGetAllUniqueId();
		pstCiContext->usSvcId = pvr_info.usSvcId;
		pstCiContext->usOnId = pvr_info.usOnId;
		pstCiContext->usTsId = pvr_info.usTsId;
	}
	#endif

#endif

	return ERR_OK;
}

static HERROR local_CiMgr_ProcStop(CI_DELETE_SVC_PARAM *pstDeleteSvc)
{
	Handle_t 		hAction = HANDLE_NULL;
	CI_Context_t	*pstCiContext = NULL;

	if (pstDeleteSvc == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	hAction = pstDeleteSvc->hAction;

	HxLOG_Info("hAction(0x%08x), hNextSvc(0x%08x)\n", hAction, pstDeleteSvc->hNextSvc);

	pstCiContext = local_CiMgr_GetContextByAction(hAction);
	if (pstCiContext == NULL)
	{
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	HxLOG_Info("pCiContext->pCam(0x%x), pCiContext->eState(%d)\n", pstCiContext->pCam, pstCiContext->eState);

#if defined(CONFIG_MW_CI_PLUS)
	HxLOG_Print(" unique id (%x) \n", pstCiContext->usUniqueId);
	local_CiMgr_FreeSdtSlot(pstCiContext->usUniqueId);
#endif

	local_CiMgr_InitContext(svc_cas_SubSvcRmGetActionId(hAction));

//	HxLOG_Info(">> s_stCasTuneInfo free..\n");
//	HxSTD_memset(&s_stCasTuneInfo, 0x00, sizeof(SvcCas_TuneInfo_t));

	return ERR_OK;
}

static HERROR local_CiMgr_ProcPmtChanged(CI_PMT_CHANGED_PARAM *pPmtChanged)
{
	Handle_t hAction = HANDLE_NULL;
	HERROR nResult = ERR_FAIL;
	CI_Context_t *pCiContext = NULL;

	if (pPmtChanged == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	hAction = pPmtChanged->hAction;
	if(HANDLE_NULL == hAction)
	{
		HxLOG_Error("invalid handle\n");
		return ERR_FAIL;
	}

	HxLOG_Info("hAction(0x%08x)\n", hAction);

	pCiContext = local_CiMgr_GetContextByAction(hAction);
	if (pCiContext == NULL) {
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	/* cas mgr 에서 parsing 된 pmt info */
	pCiContext->stProgInfo.pPmtInfo = svc_cas_MgrSubGetPmtInfo(hAction);
	if(pCiContext->stProgInfo.pPmtInfo == NULL)
	{
		HxLOG_Error("PmtInfo not null\n");
		return ERR_FAIL;
	}

	/*
		pmt update가 발생 후. crc와 version number가 동일하면, update 처리를 하지 않도록 한다.
	*/
	if(pCiContext->stProgInfo.ulPrePmtCrc == pCiContext->stProgInfo.pPmtInfo->ulCrc &&
		pCiContext->stProgInfo.ucPrePmtVer == pCiContext->stProgInfo.pPmtInfo->ucVerNum)
	{
		HxLOG_Info("updated pmt's crc & version number same with previous one\n");
		return ERR_OK;
	}

	pCiContext->stProgInfo.ulPrePmtCrc = pCiContext->stProgInfo.pPmtInfo->ulCrc;
	pCiContext->stProgInfo.ucPrePmtVer = pCiContext->stProgInfo.pPmtInfo->ucVerNum;

	local_CiMgr_DefineChannelType(pCiContext->stProgInfo.pPmtInfo, &pCiContext->eChType);

	#if defined(_CI_DEBUG_)
	{
		SvcCas_PmtInfo_t *pPmtInfo = NULL;
		HUINT32 i = 0;

		pPmtInfo = pCiContext->stProgInfo.pPmtInfo;

		HxLOG_Print("\t===[CIMGR]:SvcCas_PmtInfo_t====\n");
		HxLOG_Print("\tService ID: [0x%x], PCR pid: [0x%x]\n", pPmtInfo->usServiceId, pPmtInfo->usPcrPid);
		HxLOG_Print("\tVer No: [%d], Curr/Next: [%d], CA Flag: [%d]\n", pPmtInfo->ucVerNum, pPmtInfo->bCurNextInd, pPmtInfo->bProgramCaFlag);
		HxLOG_Print("\t==Prog Level[%d]==\n", pCiContext->stProgInfo.pPmtInfo->ulNumOfProgramCaDescriptor);
		for (i=0; i<pPmtInfo->ulNumOfProgramCaDescriptor; i++)
		{
			HxLOG_Print("\t[%d] : CA_SYS_ID:[0x%x], CA_SYS_ID:[0x%x]\n", i, pPmtInfo->stProgramCaDescriptor[i].usCaSysId, pPmtInfo->stProgramCaDescriptor[i].usCaPid);
			HxLOG_Print("\t[%d] : pucRaw:[0x%p], ucRawLen:[%d]\n", i, pPmtInfo->stProgramCaDescriptor[i].pucRaw, pPmtInfo->stProgramCaDescriptor[i].ucRawLen);
		}
		HxLOG_Print("\t==ES Level[%d]==\n", pPmtInfo->ulNumOfElement);
		for (i=0; i<pPmtInfo->ulNumOfElement; i++)
		{
			HxLOG_Print("\t[%d] : StreamType:[0x%x], PID:[0x%x], CA Flag:[%d]\n",
				i, pPmtInfo->stElementInfo[i].ucStreamType, pPmtInfo->stElementInfo[i].usPid, pPmtInfo->stElementInfo[i].bElementCaFlag);

			HxLOG_Print("\t\t : usCaSysId:[0x%x] \n", pPmtInfo->stElementInfo[i].stElementCaDescriptor[0].usCaSysId);
			HxLOG_Print("\t\t : usCaPid:[0x%x] \n", pPmtInfo->stElementInfo[i].stElementCaDescriptor[0].usCaPid);

#if defined(_CI_DEBUG_)
{
			HUINT32 iidx =0;
			HUINT32 ucRawLen = pPmtInfo->stElementInfo[i].stElementCaDescriptor[0].ucRawLen;

			HxLOG_Print("1 received pucRaw data[%d]: \n", ucRawLen);
			for ( iidx = 0; iidx < ucRawLen; iidx++)
			{
			#if defined (CONFIG_OP_SES)
				if(pPmtInfo->stElementInfo[i].stElementCaDescriptor[0].pucRaw)
				{
					HxLOG_Print("%02X ", pPmtInfo->stElementInfo[i].stElementCaDescriptor[0].pucRaw[iidx]);
				}
			#else
				HxLOG_Print("%02X ", pPmtInfo->stElementInfo[i].stElementCaDescriptor[0].pucRaw[iidx]);
			#endif
				if (iidx % 16 == (16-1))	  {
					HxLOG_Print("\n");
				}
			}
			HxLOG_Print("\n");
}
#endif

		}
		HxLOG_Print("\t=======================\n");
	}
	#endif

	/*
		pmt update case

		1. session start 직후.
		2. descramble 중 pmt 변경.

		-- 현재 action 중 cas manager로 부터 manage instance 할당 된 context가 존재하는지 search 후. 존재하지 않으면, pCam에 상관없이
		ca pmt 전송.

		-- pCam을 기준으로 현재 전달 된 EsInfo를 가지고 ca pmt를 update 해야 함.
	*/

	if(pCiContext->pCam != NULL)
	{
		if(		(pCiContext->eState == eCiMgrState_Started)
			||	(pCiContext->eState == eCiMgrState_StartCaPmt))
		{
			if(pCiContext->pCam->caPmtOkDsc.pucRaw != NULL)
			{
				nResult = local_CiMgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 1, 0);
			}
			else
			{
				nResult = local_CiMgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 0, 0);
			}
		}
		else if(pCiContext->eState == eCiMgrState_Stopped)
		{
			HxLOG_Info("\n\n");
		}
	}
	else
	{
		/*
			instance connect는 되지 않지만, FTA인 경우에 한해서 Clear Ca pmt를 CAM에 전달한다.
		*/
		HxLOG_Info("pCam is not allocated \n");
	}

	return nResult;
}

static HERROR local_CiMgr_ProcPmtChangedLocation(CI_PMT_CHANGED_LOCATION_PARAM *pmtChangedLoc)
{
	Handle_t		hAction = HANDLE_NULL;
	HERROR			nResult = ERR_FAIL;
	CI_Context_t	*pCiContext = NULL;
	HUINT16			i = 0;
	HUINT32			unSlotId;

//	unsigned char	*pucTbl;
//	unsigned short	usTblLen;


	if (pmtChangedLoc == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	hAction = pmtChangedLoc->hAction;
	if(HANDLE_NULL == hAction)
	{
		HxLOG_Error("invalid handle\n");
		return ERR_FAIL;
	}

	unSlotId = pmtChangedLoc->unSlotId;

	HxLOG_Info("hAction(0x%08x)\n", hAction);
	HxLOG_Info("unSlotId(%d)\n", unSlotId);

	pCiContext = local_CiMgr_GetContextByAction(hAction);
	if (pCiContext == NULL) {
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	/* cas mgr 에서 parsing 된 pmt info */
	pCiContext->stProgInfo.pPmtInfo = svc_cas_MgrSubGetPmtInfo(hAction);
	if(pCiContext->stProgInfo.pPmtInfo == NULL)
	{
		HxLOG_Error("PmtInfo not null\n");
		return ERR_FAIL;
	}

	pCiContext->stProgInfo.pPmtInfo->usServiceId = pmtChangedLoc->pstPmtInfo->usServiceId;
	pCiContext->stProgInfo.pPmtInfo->usPcrPid = pmtChangedLoc->pstPmtInfo->usPcrPid;
	pCiContext->stProgInfo.pPmtInfo->ulNumOfElement = pmtChangedLoc->pstPmtInfo->usEleNum;

	for (i = 0; i < MAX_ES_NUM_FOR_TUNING; i++)
	{
		pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].ucStreamType = pmtChangedLoc->pstPmtInfo->stEleInfo[i].ucstream_type;
		pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].usPid = pmtChangedLoc->pstPmtInfo->stEleInfo[i].usElePid;
		// 아래 desc를 채워야 ci+인증시 pmt값을 올바르게 출력할 수 있다.
		HxSTD_memcpy(&pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].stElementCaDescriptor[0], &pmtChangedLoc->pstPmtInfo->stEleInfo[i].stElementCaDescriptor[0], sizeof(SvcCas_CaDescriptor_t));
	}

	pCiContext->stProgInfo.ulPrePmtCrc = pCiContext->stProgInfo.pPmtInfo->ulCrc;
	pCiContext->stProgInfo.ucPrePmtVer = pCiContext->stProgInfo.pPmtInfo->ucVerNum;

	local_CiMgr_DefineChannelType(pCiContext->stProgInfo.pPmtInfo, &pCiContext->eChType);

#if defined(_CI_DEBUG_)
	{
		SvcCas_PmtInfo_t *pPmtInfo = NULL;
		HUINT32 i = 0;

		pPmtInfo = pCiContext->stProgInfo.pPmtInfo;

		HxLOG_Print("\t===[CIMGR]:CAS_PmtInfo_t====\n");
		HxLOG_Print("\tService ID: [0x%x], PCR pid: [0x%x]\n", pPmtInfo->usServiceId, pPmtInfo->usPcrPid);
		HxLOG_Print("\tVer No: [%d], Curr/Next: [%d], CA Flag: [%d]\n", pPmtInfo->ucVerNum, pPmtInfo->bCurNextInd, pPmtInfo->bProgramCaFlag);
		HxLOG_Print("\t==Prog Level[%d]==\n", pCiContext->stProgInfo.pPmtInfo->ulNumOfProgramCaDescriptor);
		for (i=0; i<pPmtInfo->ulNumOfProgramCaDescriptor; i++)
		{
			HxLOG_Print("\t[%d] : CA_SYS_ID:[0x%x], CA_SYS_ID:[0x%x]\n", i, pPmtInfo->stProgramCaDescriptor[i].usCaSysId, pPmtInfo->stProgramCaDescriptor[i].usCaPid);
			HxLOG_Print("\t[%d] : pucRaw:[0x%p], ucRawLen:[%d]\n", i, pPmtInfo->stProgramCaDescriptor[i].pucRaw, pPmtInfo->stProgramCaDescriptor[i].ucRawLen);
		}
		HxLOG_Print("\t==ES Level[%d]==\n", pPmtInfo->ulNumOfElement);
		for (i=0; i<pPmtInfo->ulNumOfElement; i++)
		{
			HxLOG_Print("\t[%d] : StreamType:[0x%x], PID:[0x%x], CA Flag:[%d]\n",
				i, pPmtInfo->stElementInfo[i].ucStreamType, pPmtInfo->stElementInfo[i].usPid, pPmtInfo->stElementInfo[i].bElementCaFlag);

			HxLOG_Print("\t\t : usCaSysId:[0x%x] \n", pPmtInfo->stElementInfo[i].stElementCaDescriptor[0].usCaSysId);
			HxLOG_Print("\t\t : usCaPid:[0x%x] \n", pPmtInfo->stElementInfo[i].stElementCaDescriptor[0].usCaPid);

#ifdef CONFIG_DEBUG
{
			HUINT32 iidx =0;
			HUINT32 ucRawLen = pPmtInfo->stElementInfo[i].stElementCaDescriptor[0].ucRawLen;

			HxLOG_Print("1 received pucRaw data[%d]: \n", ucRawLen);
			for ( iidx = 0; iidx < ucRawLen; iidx++)
			{
				HxLOG_Print("%02X ", pPmtInfo->stElementInfo[i].stElementCaDescriptor[0].pucRaw[iidx]);

				if (iidx % 16 == (16-1))	  {
					HxLOG_Print("\n");
				}
			}
			HxLOG_Print("\n");
}
#endif
		}
		HxLOG_Print("\t=======================\n");
	}
	#endif

	/*
		pmt update case

		1. session start 직후.
		2. descramble 중 pmt 변경.

		-- 현재 action 중 cas manager로 부터 manage instance 할당 된 context가 존재하는지 search 후. 존재하지 않으면, pCam에 상관없이
		ca pmt 전송.

		-- pCam을 기준으로 현재 전달 된 EsInfo를 가지고 ca pmt를 update 해야 함.
	*/

	if(pCiContext->pCam != NULL)
	{
		if(		(pCiContext->eState == eCiMgrState_Started)
			||	(pCiContext->eState == eCiMgrState_StartCaPmt))
		{
			if(pCiContext->pCam->caPmtOkDsc.pucRaw != NULL)
			{
				nResult = local_CiMgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 1, 0);
			}
			else
			{
				nResult = local_CiMgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 0, 0);
			}
		}
		else if(pCiContext->eState == eCiMgrState_Stopped)
		{
			HxLOG_Info("\n\n");
		}
	}
	else
	{
		/*
			instance connect는 되지 않지만, FTA인 경우에 한해서 Clear Ca pmt를 CAM에 전달한다.
		*/
		HxLOG_Info("pCam is not allocated \n");
	}

	return nResult;
}

static HERROR local_CiMgr_ProcManageInstance(CI_MANAGE_CA_INST_PARAM *pParam)
{
	HUINT32 		i;
	HERROR			hErr = ERR_FAIL;
	CI_Context_t		*pCiContext = NULL;
	CI_CamInst_t		*pCamSelected = NULL;

	if (pParam == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	pCiContext = local_CiMgr_GetContextByAction(pParam->hAction);
	if (pCiContext == NULL)
	{
		HxLOG_Error("Sequence Error. 반드시 잡아라. \n");
		return ERR_FAIL;
	}

	for(i=0; i < CARM_MAX_CISLOT_NUMBER; i++)
	{
		if(s_astCiCam[i].hCasDevice == pParam->hCasDevice)
		{
			pCamSelected = &s_astCiCam[i];
			break;
		}
	}

	if(pCamSelected == NULL)
	{
		HxLOG_Error("pCamSelected is null\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Info("hAction(0x%08x), bConnect(%d), pCiContext->pCam(0x%x), eActType = %s\n",
			pParam->hAction, pParam->bConnect, (HUINT32)pCiContext->pCam, local_CiMgr_MakeStrActionType(pCiContext->eActType));
#endif

	if(pParam->bConnect == TRUE) // 캠이 꽂혀 있는 상황
	{
	#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
	#else
		HBOOL bIsNeedPathChange = FALSE;
	#endif

		/*	2009.07.21 추가.
			- cas manager에서 instance를 ci로 선택 시 application에게 "cas_ok"를 주는 trigger. 권한에 상관 없음. Descramble Try를 하면 반드시 ok 전달해야 함.
		*/
		svc_cas_MgrSubUpdateCasMessage(pParam->hAction, eCAS_MSG_TYPE_Ok, eDxCAS_GROUPID_CAM, NULL, 0);

		/* Instance Connect는 반드시 DisConnect 후에 Connect 되어야 함. */
		if(pCiContext->pCam != NULL)
		{
		#if 0 // #if defined(CONFIG_DEBUG) // 디버깅용 코드.
			CI_PrintCAMInfo();
			CI_PrintContextInfo();
		#endif

			HxLOG_Error("sequence error, 0x%x 0x%x\n", pCiContext->pCam, pCamSelected);
		}

		/* 이 시점에서 Ci Context에 실제 cam instance가 연결 되는 것이다.*/
		pCiContext->pCam = pCamSelected;

	#if defined (CONFIG_MW_CI_PLUS) && defined (CONFIG_MW_MM_PVR)
		{
			/*	PVR 에서 version 1 cam (일반 캠)을 사용하는 경우. 모든 Action에 대해서 Drm None을 알려줘야 한다.
			*/
			CI_CC_KEY_CIPHER enNegotiatedCckCipher = CI_CC_KEY_CIPHER_DES_56_ECB;

			if ((pCamSelected->bKeyInfoAvailable == FALSE) || (CI_GetNegotiatedCckCipher(pCamSelected->usSlotId, &enNegotiatedCckCipher) != ERR_OK))
			{
				local_CiMgr_SendCiDrmInfo(pCiContext->hAction, pCamSelected->hCasDevice, eDxCopyrightControl_CiPlusDrmType_Uri, (void *)NULL);
			}
		}
	#endif

#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
		local_CiMgr_ChangePathExtControl(pCiContext, pCiContext->pCam->usSlotId, TRUE);
	#if defined(CONFIG_MW_CI_PLUS)
		local_CiMgr_CheckUriControl(pCiContext, pCamSelected);
	#endif
#else
		local_CiMgr_CheckPathChanging(pCamSelected, pCiContext, &bIsNeedPathChange);
		HxLOG_Info("bIsNeedPathChange = %d\n", bIsNeedPathChange);

		if (bIsNeedPathChange == TRUE)
		{
			hErr = local_CiMgr_ChangePath(pCiContext, pCiContext->pCam->usSlotId, TRUE);
			if(hErr != ERR_OK)
			{
				HxLOG_Error("change path. slot id (%d) \n", pCiContext->pCam->usSlotId);
				return ERR_FAIL;
			}

			if (pCiContext->eActType == eCAS_ActionType_Live || pCiContext->eActType == eCAS_ActionType_Playback )//|| pCiContext->eActType == eCAS_ActionType_Tsr_Play)
			{
				HxLOG_Info("Live \n");
				local_CiMgr_SearchOtherActionForConnecting(pCamSelected, pCiContext);
			}

		#if defined(CONFIG_MW_CI_PLUS)
			local_CiMgr_CheckUriControl(pCiContext, pCamSelected);
		#endif
		}
#endif

	#ifdef DEBUG_ACT_LIST
		local_CiMgr_PrintActionList(pCamSelected);
	#endif

		/*
			여기서 action의 상태를 비교해서.
			Live와 Rec 등의 우선순위 정리를 해야 한다.

			1. Live Descramble 중.
			  1-1. 동일 서비스 Recording
			  1-2. 다른 서비스 Recording

			2. Rec Descramble 중.
			  2-1. 동일 서비스 Live
			  2-2. 다른 서비스 Live
			  2-3. 동일 서비스 Rec
			  2-4. 다른 서비스 Rec
		*/

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
		// TODO: CI+ 1.3 동작을 위한 시컨스 작업 (kkkim)
		// 아래 동작이 CI+ 1.3 CAM 이 아닌 다른 CAM이 삽입 되어 있는데 동작하면 곤란 함.
		// Operator Profile resource가 열려 있을 때에만 동작 하도록 if문 추가 할 것.
/*		if(pCamSelected->bOpSearchReq == TRUE && pCiContext->eActType == eCAS_ActionType_Live)
		{
			if(local_CiMgr_SendEvent(CI_PLUS_OPERATOR_PREPARE_REQ, (void *)&pCamSelected->stOpParam, (unsigned long)sizeof(CI_PLUS_OPERATOR_PREPARE_PARAM)) != ERR_OK)
				HxLOG_Critical("Critical Error!!!\n");
			pCamSelected->bOpSearchReq = FALSE;
		}
*/
		hErr = local_CiMgr_OperateCCv2RecordProcess(pParam, pCiContext);
		if (hErr != ERR_OK)
		{
			pCiContext->bRecordProcessDone = FALSE;
			HxLOG_Error("local_CiMgr_OperateCCv2RecordProcess() ERROR!!\n");
			return ERR_FAIL;
		}
		pCiContext->bRecordProcessDone = TRUE;

#endif

		/* pmt 를 받은 적이 없다면, Descramble Start 필요 없음. */
		if (pCiContext->stProgInfo.pPmtInfo == NULL)
		{
			HxLOG_Info("pmt not updated. \n");
			return ERR_OK;
		}

		hErr = local_CiMgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 0, 0);
		if(hErr != ERR_OK)
		{
			HxLOG_Critical("Critical Error!!!\n");
		}
	}
	else // 캠이 제거된 상황
	{
		// dis connect
		if (pCiContext->pCam == NULL) // SVC Instance에 등록되어 있는 CAM Instance의 Pointer도 Null인경우.
		{
			HxLOG_Error(" sequence error. 반드시 잡아야 한다.\n");
			return ERR_FAIL;
		}

		/*
			여기서 action의 상태를 비교해서.
			Live와 Rec 등의 우선순위 정리를 해야 한다.

			1. Live
			  1-1. 동일 서비스 Recording이 남은 경우.

			2. Rec
			  2-1. 동일 서비스 Live이 남은 경우.
			  2-3. 동일 서비스 Rec
			  2-4. 다른 서비스 Rec
		*/
		if(pCiContext->pCam->ulNumOfAct == 0)
		{
			HxLOG_Critical("\n");
			return ERR_FAIL;
		}

		if (local_CiMgr_CheckActionHandleInCamInstance(pCiContext->pCam, pCiContext) == FALSE)
		{
			HxLOG_Error("this action is not need stop descramble \n");
			return ERR_OK;
		}

	#ifdef DEBUG_ACT_LIST
		local_CiMgr_PrintActionList(pCiContext);
	#endif

		HxLOG_Info("pCiContext->pCam->ulNumOfAct = %d\n", pCiContext->pCam->ulNumOfAct);

		if(pCiContext->pCam->ulNumOfAct == 1)
		{
			hErr = local_CiMgr_StopDescramble(pCiContext, pCiContext->pCam->usSlotId);
			if(hErr != ERR_OK)			HxLOG_Critical("Critical Error!!!\n");

		#if defined(CONFIG_MW_CI_PLUS)
			CI_SetUriDefault(pCiContext->pCam->usSlotId, FALSE);
		#endif
		}
		else
		{

		#if defined(CONFIG_MW_CI_PLUS)

			HxLOG_Info("slot %d's NumOfDsc = %d, KeyInfoAvailable = %d\n",
				pCiContext->pCam->usSlotId, pCiContext->pCam->ulNumOfDsc[pCiContext->ulDemuxId], pCiContext->pCam->bKeyInfoAvailable);

			if (pCiContext->ulDemuxId != CAS_RESOURCE_NONE)
			{
				for (i = 0; i < pCiContext->pCam->ulNumOfDsc[pCiContext->ulDemuxId]; i++)
				{
					hErr = svc_cas_DevDscClose(pCiContext->pCam->stDscInfo[pCiContext->ulDemuxId][i].ulDscId);
					if(hErr != ERR_OK)			HxLOG_Critical("Critical Error!!!\n");

					pCiContext->pCam->stDscInfo[pCiContext->ulDemuxId][i].ulDscId = CAS_RESOURCE_NONE;
					pCiContext->pCam->stDscInfo[pCiContext->ulDemuxId][i].usPid = PID_NULL;
				}
				pCiContext->pCam->ulNumOfDsc[pCiContext->ulDemuxId] = 0; // pCam->bKeyInfoAvailable 및 키 정보 값들은 리셋하지 않도록 한다.
			}
			else
			{
				HxLOG_Critical("\n\n");
			}

		#endif
		}

#if defined(CONFIG_SUPPORT_RETRY_CAPMT)
		hErr = local_CiMgr_StopRetrySvc(pCiContext);
		if(hErr != ERR_OK)
		{
			HxLOG_Critical("Critical Error!!!\n");
		}
#endif

#if defined(CONFIG_MW_CI_CHECK_AV)
		hErr = local_CiMgr_StopCheckAv(pCiContext);
		if(hErr != ERR_OK)
		{
			HxLOG_Critical("Critical Error!!!\n");
		}
#endif

#if defined(CONFIG_MW_CI_PLUS)
		if(local_CiMgr_CheckClearCaPmtCondition(pCiContext, FALSE) == TRUE)
		{
			local_CiMgr_SendClearCaPmt(pCiContext->pCam->usSlotId, pCiContext);
		}
#endif
		pCiContext->eState = eCiMgrState_Stopped;

		hErr = local_CiMgr_DeleteActionToCamInstance(pCiContext->pCam, pCiContext->hAction);
		if(hErr != ERR_OK)			HxLOG_Critical("Critical Error!!!\n");

	#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
		hErr = local_CiMgr_ChangePathExtControl(pCiContext, pCiContext->pCam->usSlotId, FALSE);
	#else
		hErr = local_CiMgr_ChangePath(pCiContext, pCiContext->pCam->usSlotId, FALSE);
	#endif

		if(hErr != ERR_OK)			HxLOG_Critical("Critical Error!!!\n");

#if defined (CONFIG_MW_CI_PLUS_CC_V_2)
		hErr = local_CiMgr_OperateCCv2RecordStop(pParam, pCiContext);
		if (hErr != ERR_OK)
		{
			HxLOG_Error("local_CiMgr_OperateCCv2RecordStop() ERROR!!\n");
			return ERR_FAIL;
		}
		pCiContext->bRecordProcessDone = FALSE;
#endif

		/* context에서 instance 초기화. */
		pCiContext->pCam = NULL;

		return ERR_OK;
	}

	return ERR_OK;
}


static HERROR local_CiMgr_ProcUpdateCaInst(CI_UPDATE_CA_INST_PARAM *pUpdateCaInst)
{
	CI_CamInst_t *pCam = NULL;
	CI_APP_INFO *pAppInfo = NULL;
	HERROR nResult = ERR_FAIL;

	if (pUpdateCaInst == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	if (pUpdateCaInst->usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		if (pUpdateCaInst->ausCaSysId != NULL)
		{
			OxCAS_Free(pUpdateCaInst->ausCaSysId);
		}
		HxLOG_Error("input slot id (%d)\n", pUpdateCaInst->usSlotId);
		return ERR_FAIL;
	}

	HxLOG_Info("SlotId (%d), CaInfoCnt (%d), hCasDevice (0x%x)\n",
					pUpdateCaInst->usSlotId, pUpdateCaInst->usCaInfoCnt, s_astCiCam[pUpdateCaInst->usSlotId].hCasDevice);

	/* cam instance 는 slot id로 관리 된다. */
	pCam = &s_astCiCam[pUpdateCaInst->usSlotId];

	if (pCam->usCaInfoCnt == 0) // pCam 에 저장된 CaInfo 가 없는 경우
	{
		if ((pUpdateCaInst->usCaInfoCnt == 0) || (pUpdateCaInst->ausCaSysId == NULL))
		{
			HxLOG_Error("\n");
			return ERR_FAIL;
		}

		if (CI_GetAppInfo(pUpdateCaInst->usSlotId, &pAppInfo) != ERR_OK) // 통상 AI -> CAS 순으로 리소스가 오픈되므로 여기서 에러가 나는 일은 없겠지....
		{
			HxLOG_Error("\n");
			return ERR_FAIL;
		}

		pCam->usSlotId = pUpdateCaInst->usSlotId;
		pCam->usManufacturer = pAppInfo->usManufacturer;

		if (pCam->ausCaSysId != NULL) // 이런 경우는 없을 것이지만 확실을 기하기 위해서...
		{
			OxCAS_Free(pCam->ausCaSysId);
		}
		pCam->ausCaSysId = (unsigned short *)OxCAS_Malloc(sizeof(unsigned short) * pUpdateCaInst->usCaInfoCnt);
		if (pCam->ausCaSysId == NULL)
		{
			if (pUpdateCaInst->ausCaSysId != NULL)
			{
				OxCAS_Free(pUpdateCaInst->ausCaSysId);
			}
			HxLOG_Error("\n");
			return ERR_FAIL;
		}

		HxSTD_memcpy(pCam->ausCaSysId, pUpdateCaInst->ausCaSysId, sizeof(unsigned short) * pUpdateCaInst->usCaInfoCnt);
		OxCAS_Free(pUpdateCaInst->ausCaSysId);

		pCam->usCaInfoCnt = pUpdateCaInst->usCaInfoCnt;

		#if defined(CONFIG_MW_CI_PLUS)
		if (CI_IsCiPlusCam(pCam->usSlotId) > 0)
		{
			svc_cas_DevCiSetCamType(pCam->usSlotId, eCAS_CAM_TYPE_CI_PLUS);
		}
		else
		#endif
		{
			svc_cas_DevCiSetCamType(pCam->usSlotId, eCAS_CAM_TYPE_CI_V1);
		}
#if 0
		nResult = (HINT32)svc_cas_MgrSubUpdateCasInfo(pCam->hCasDevice, pCam->ausCaSysId[0] & 0xff00, pUpdateCaInst->usSlotId, pCam->ausCaSysId, pCam->usCaInfoCnt, 1, TRUE);
#else
		{
			unsigned long ulDelayTimeOut = 0;
			CI_SLOT_ID usSlotId;
			HULONG ulTimerId = 0;

			#if 0

			/* Host Service Shunning 때문에 추가된 코드로
			   자세한 것은 local_cimgr_ProcCheckShunCondition()에 있는 주석 참고할 것.
			   추후에 이 부분에 대한 근본적인 개선이 있어야 할 것이다 (eCAS_DEV_EVT_CamPlusInfo 이용할 계획임).
			   -> 위와 같이 개선하여 이 코드는 제거함. */

			ulDelayTimeOut = 5000; // Neotion CI+ 캠 기준으로 4000ms 까지는 문제가 된다.

			////////////////////

			// 아래 내용은 CAM이 풀수 있는 CA system ID가 6개보다 많을 때 Update CAS Info를 3초 딜레이 후 보내는데, 해당 부분이 화면이 순간적으로 깨지는 이슈를 야기 시킴. (Austria Neotion Irdeto CAM)
			// 2015년 기준의 시료는 다수의 CA System ID  포함시 Update CAS Info를 바로 발생 시켜도 퍼포먼스 문제 없이 정상 동작 하므로 해당 code는 들어 내도록 함.

			if (pCam->usCaInfoCnt > 6) // 특정 캠 따지지 말고 캠이 다수의 CaSysId 들을 가지고 있으면 무조건 딜레이 주도록 하자...
			{
				ulDelayTimeOut = 3000;
			}

			if (ulDelayTimeOut)
			{
				HxLOG_Info("ulDelayTimeOut is %lu ms\n", ulDelayTimeOut);
			}

			if (ulDelayTimeOut == 0)
			{
				nResult = (HINT32)svc_cas_MgrSubUpdateCasInfo(pCam->hCasDevice,
										//pCam->ausCaSysId[0] & 0xff00,
										eDxCAS_GROUPID_CAM,
										pUpdateCaInst->usSlotId,
										pCam->ausCaSysId,
										pCam->usCaInfoCnt,
										(HUINT8)1,
										TRUE);
			}
			else
			{
				usSlotId = pCam->usSlotId;
				nResult = VK_TIMER_EventAfter(ulDelayTimeOut, local_CiMgr_CallbackDelayedAddCaInst, (void *)&usSlotId, sizeof(CI_SLOT_ID), &ulTimerId);
			}
			#endif

			nResult = (HINT32)svc_cas_MgrSubUpdateCasInfo(pCam->hCasDevice,
											//pCam->ausCaSysId[0] & 0xff00,
											eDxCAS_GROUPID_CAM,
											pUpdateCaInst->usSlotId,
											pCam->ausCaSysId,
											pCam->usCaInfoCnt,
											(HUINT8)1,
											TRUE);

		}
#endif
		if (nResult != ERR_OK)
		{
			HxLOG_Error("\n");
			return nResult;
		}

		if (pCam->usCaInfoCnt > 0) // 위에서 pCam->usCaInfoCnt == 0 인 경우가 걸러지므로 이 조건 체크는 필요 없다...
		{
			g_aucIsCaSysId[pCam->usSlotId] = 1;
			HxLOG_Info("g_aucIsCaSysId[%d] is enabled\n", pCam->usSlotId);
		}
	}
	else // pCam 에 저장된 CaInfo 가 있는 경우
	{
		if (pUpdateCaInst->usCaInfoCnt == 0) // CA instance 가 release 된 상황 (즉, CAS 세션 종료로 CiCas_DeleteCaInst() 수행된 것) -> 캠이 제거된 것으로 보고 필요한 처리를 해준다.
		{
			if (pCam->usSlotId >= svc_cas_DevCiGetNumberOfModule())
			{
				HxLOG_Error("input slot id (%d)\n", pCam->usSlotId);
			}

			if (pCam->usSlotId != pUpdateCaInst->usSlotId)
			{
				HxLOG_Error("pCam->usSlotId = %x, pUpdateCaInst->usSlotId = %x\n", pCam->usSlotId, pUpdateCaInst->usSlotId);
			}

			HxLOG_Info("g_aucIsCaSysId[%d] = %d -> will be disabled\n", pUpdateCaInst->usSlotId, g_aucIsCaSysId[pUpdateCaInst->usSlotId]);
			g_aucIsCaSysId[pUpdateCaInst->usSlotId] = 0;

			// SVC Inst에서 CAM Inst 제거 (s_astCiContext 에서 pCam 제거)
#if 0
			nResult = local_cimgr_DeleteCamInContext(pCam);
			if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");
#endif
			// CAS mgr 에게도 알려준다.
			nResult = svc_cas_MgrSubUpdateCasInfo(pCam->hCasDevice, eDxCAS_GROUPID_CAM, pUpdateCaInst->usSlotId, NULL, 0, 0, FALSE);
			if (nResult != ERR_OK)
			{
				HxLOG_Error("\n");
				return nResult;
			}

			// 이하는 CAM Inst 초기화
			if (pCam->ausCaSysId != NULL)
			{
				OxCAS_Free(pCam->ausCaSysId);
				pCam->ausCaSysId = NULL;
			}

			if (pCam->caPmtOkDsc.pucRaw != NULL)
			{
				OxCAS_Free(pCam->caPmtOkDsc.pucRaw);
				pCam->caPmtOkDsc.pucRaw = NULL;
				pCam->caPmtOkDsc.usRawLen = 0;
			}

			//pCam->usSlotId = CARM_INVALID_CI_SLOT_ID;
			pCam->usManufacturer = 0;
			pCam->usCaInfoCnt = 0;
			#if 0
			{
						HUINT32 i = 0;
						for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
						{
							pCam->hActList[i] = HANDLE_NULL;
						}
						pCam->ulNumOfAct = 0;
			}
			#endif
		}
		else // pCam 에 이미 저장된 CaInfo 가 있는데 CA instance 가 새로 올라온 경우 -> 통상 일반적이지 않은 경우이나...
		{
			HxLOG_Error("\n");
		}
	}

	return ERR_OK;
}

static HERROR local_CiMgr_ProcAddDelayedCaInst(CI_UPDATE_CA_INST_PARAM *pAddCaInst)
{
	CI_CamInst_t *pCam = NULL;
	HERROR nResult = ERR_FAIL;

	if (pAddCaInst == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	HxLOG_Info("slot id (%d), ca system id cnt (%d) \n", pAddCaInst->usSlotId, pAddCaInst->usCaInfoCnt);

	if( pAddCaInst->usSlotId >= CARM_MAX_CISLOT_NUMBER )
	{
		HxLOG_Error("invalid slotId:%d\n", pAddCaInst->usSlotId );
		return ERR_FAIL;
	}

	pCam = &s_astCiCam[pAddCaInst->usSlotId];
	if (pCam->usCaInfoCnt == 0)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	/* ci 의 경우 cas group id는 NONE으로 정의 함. */
	nResult = svc_cas_MgrSubUpdateCasInfo(pCam->hCasDevice, eDxCAS_GROUPID_CAM, pCam->usSlotId, pCam->ausCaSysId, pCam->usCaInfoCnt, 1, TRUE);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HINT32 local_CiMgr_ProcUpdateCamState(CI_UPDATE_CAM_STATE_PARAM *pUpdateCamSt)
{
	CI_CamInst_t *pCam;

	if (pUpdateCamSt == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	HxLOG_Info("SlotId[0x%x], state[%d]\n", pUpdateCamSt->usSlotId, pUpdateCamSt->eCamState);

	if (pUpdateCamSt->usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", pUpdateCamSt->usSlotId);
		return ERR_FAIL;
	}

	pCam = &s_astCiCam[pUpdateCamSt->usSlotId];

	pCam->eCamState = pUpdateCamSt->eCamState;

	// ci path 수정 및 ci+ 의 경우 오픈된 디스크램블러가 있으면 닫아준다.
	if(pUpdateCamSt->eCamState == CI_CAM_EXTRACTED)
	{
		#if defined(CONFIG_MW_CI_PLUS)
		{
			HERROR nResult = ERR_FAIL;
			HUINT32 i, j;

			if (pCam->usSlotId != pUpdateCamSt->usSlotId) // 이미 캠 인스턴스가 초기화 되었을 때 이럴 수가 있다. 해당 슬롯 아이디의 s_astCiCam[]에 디스크램블러 정보만 리셋하면 되므로 무시.
			{
				HxLOG_Error("pCam->usSlotId = %x, pUpdateCamSt->usSlotId = %x\n", pCam->usSlotId, pUpdateCamSt->usSlotId);
			}

			/*
				존재하는 모든 DSC Close
			*/
			for (i = 0; i < CI_MAX_NUM_OF_DMX; i++)
			{
				HxLOG_Info("slot %d's NumOfDsc = %d, KeyInfoAvailable = %d\n",
					pUpdateCamSt->usSlotId, pCam->ulNumOfDsc[i], pCam->bKeyInfoAvailable);
				for (j = 0; j < pCam->ulNumOfDsc[i]; j++)
				{
					nResult = svc_cas_DevDscClose(pCam->stDscInfo[i][j].ulDscId);
					pCam->stDscInfo[i][j].ulDscId = CAS_RESOURCE_NONE;
					pCam->stDscInfo[i][j].usPid = PID_NULL;

					if(ERR_OK!=nResult)			HxLOG_Critical("Critical Error!!!\n");
				}

				pCam->ulNumOfDsc[i] = 0;
			}

			pCam->bKeyInfoAvailable = 0;
			pCam->hLastSvc		= HANDLE_NULL;
		#if 0
			pCam->eCcReport		= eCiPlus_CcReport_Max;
		#else
			pCam->bAuthFinish	= FALSE;
		#endif
			s_abUriSent[pCam->usSlotId] = FALSE;

//			HxLOG_Info(">> s_stCasTuneInfo free..\n");
//			HxSTD_memset(&s_stCasTuneInfo, 0x00, sizeof(SvcCas_TuneInfo_t));
		}
		#endif

		#if defined(CONFIG_MW_CI_USE_EXTERNAL_CONTROLLER)
		{
			HUINT32			i;
			CI_Context_t	*pstContext;

			for(i = 0; i < pCam->ulNumOfAct; i++)
			{
				local_CiMgr_PlugInCi(svc_cas_SubSvcRmGetActionId(pCam->hActList[i]), -1);
			}

			// PAL에서 관리하고 있는 TS path 정보 초기화.
			svc_cas_DevCiSetExtendedTsPath(-1, pUpdateCamSt->usSlotId, CAS_RESOURCE_NONE, eCAS_CI_PATH_BYPASS);

			// 다른 CAM이 삽입되어 있는 경우 NORMAL PATH로 연결한다.
			pstContext = local_CiMgr_GetCiContextBySlotId(pUpdateCamSt->usSlotId == 0 ? 1 : 0);
			if(pstContext != NULL)
			{
				svc_cas_DevCiSetExtendedTsPath(pstContext->ulTunerId, pstContext->pCam->usSlotId, pstContext->ulDemuxId, eCAS_CI_PATH_NORMAL_PASSTHROUGH);
			}
		}
		#else
		svc_cas_DevCiSetTsPath(pUpdateCamSt->usSlotId, eCAS_DEV_CI_BYPASS);
		#endif
	}

	if(pUpdateCamSt->eCamState == CI_CAM_INITIALIZED)
	{
		HUINT32		i;
		HERROR		hErr = ERR_FAIL;

		if(CI_IsCiPlusCam(pUpdateCamSt->usSlotId) > 0){
			s_abCheckAvMoreCount[pUpdateCamSt->usSlotId] = TRUE;
		}
		else{
			s_abCheckAvMoreCount[pUpdateCamSt->usSlotId] = FALSE;
		}

		for(i=eActionId_VIEW_FIRST; i<eActionId_VIEW_LAST; i++)
		{
			hErr = local_CiMgr_PlugInCi(i, pUpdateCamSt->usSlotId);
			if(hErr == ERR_OK)
			{
				break;
			}
		}

		if(hErr == ERR_FAIL)
		{
			HxLOG_Error("When a CAM is inserted, there is no available view action...\n");
		}
	}

	return ERR_OK;
}

#if defined(CONFIG_MW_CI_PLUS)
static HINT32 local_CiMgr_ProcCamTune(CI_PLUS_CAM_TUNE_PARAM *pCamTuneSt)
{
	SvcCas_CiPlusTuneInfo_t	*pstTuneInfo = &pCamTuneSt->stTuneInfo;
	Handle_t 			hAction = HANDLE_NULL;
	HERROR				hErr = ERR_OK;

	hAction = local_CiMgr_GetActionBySlotId(pCamTuneSt->usSlotId);

	if (pstTuneInfo->eTuneMode == eCiPlus_Tune_Start_v1_TuneByTripleId)
	{
		s_stCasTuneInfo.eMode = eCasTuneMode_Start_v1_TuneByTripleId;
		s_stCasTuneInfo.utInfo.stTripleId.usNetId	= pstTuneInfo->utInfo.stTripleId.usNetId;
		s_stCasTuneInfo.utInfo.stTripleId.usOnId	= pstTuneInfo->utInfo.stTripleId.usOnId;
		s_stCasTuneInfo.utInfo.stTripleId.usTsId	= pstTuneInfo->utInfo.stTripleId.usTsId;
		s_stCasTuneInfo.utInfo.stTripleId.usSvcId	= pstTuneInfo->utInfo.stTripleId.usSvcId;
		HxLOG_Info("Action Handle (0x%x), Mode (%d), NetId (0x%x), OnId (0x%x), TsId(0x%x), SvcId (0x%x)\n",
			hAction, s_stCasTuneInfo.eMode, s_stCasTuneInfo.utInfo.stTripleId.usNetId,s_stCasTuneInfo.utInfo.stTripleId.usOnId,s_stCasTuneInfo.utInfo.stTripleId.usTsId,s_stCasTuneInfo.utInfo.stTripleId.usSvcId);
	}
#if defined(CONFIG_MW_CI_PLUS_HCI_V_2)
	else if (pstTuneInfo->eTuneMode == eCiPlus_Tune_Start_v2_TuneByTripleId)
	{
		s_stCasTuneInfo.eMode = eCasTuneMode_Start_v2_TuneByTripleId;
		s_stCasTuneInfo.utInfo.stTripleId.usNetId	= pstTuneInfo->utInfo.stTripleId.usNetId;
		s_stCasTuneInfo.utInfo.stTripleId.usOnId	= pstTuneInfo->utInfo.stTripleId.usOnId;
		s_stCasTuneInfo.utInfo.stTripleId.usTsId	= pstTuneInfo->utInfo.stTripleId.usTsId;
		s_stCasTuneInfo.utInfo.stTripleId.usSvcId	= pstTuneInfo->utInfo.stTripleId.usSvcId;
		HxLOG_Info("Action Handle (0x%x), Mode (%d), NetId (0x%x), OnId (0x%x), TsId(0x%x), SvcId (0x%x)\n",
			hAction, s_stCasTuneInfo.eMode, s_stCasTuneInfo.utInfo.stTripleId.usNetId,s_stCasTuneInfo.utInfo.stTripleId.usOnId,s_stCasTuneInfo.utInfo.stTripleId.usTsId,s_stCasTuneInfo.utInfo.stTripleId.usSvcId);
	}
	else if (pstTuneInfo->eTuneMode == eCiPlus_Tune_Start_v2_TuneByLocation)
	{
		void				*pDesInfo = NULL;
		HUINT8				ucTag;
		HUINT16				ulDesclen = 0, ulTagLen = 0;
		HUINT16				ulCnt = 0;

		HxLOG_Info("Get into eCiPlus_Tune_Start_v2_TuneByLocation!!\n");
		s_stCasTuneInfo.eMode = eCasTuneMode_Start_v2_TuneByLocation;
		s_stCasTuneInfo.utInfo.stBroadcastReq.bPmtFlag			= pstTuneInfo->utInfo.stBroadcastReq.bPmtFlag;
		s_stCasTuneInfo.utInfo.stBroadcastReq.usServiceId		= pstTuneInfo->utInfo.stBroadcastReq.usServiceId;
		s_stCasTuneInfo.utInfo.stBroadcastReq.usDescriptorsLen	= pstTuneInfo->utInfo.stBroadcastReq.usDescriptorsLen;

		// TODO: 아래 주석 내용을 참고하여 구현사항 추후에 진행 필요.
		/********************************************************************************************/
		/* 추가 작업 사항에 대한 상세 내용															*/
		/* 1. pstTuneInfo->utInfo.stBroadcastReq.pucDescriptors Descriptor 설명(CI+ 1.3 Spec 내용)		*/
		/*	- 하나 이상의 delivery system descriptors가 전송										*/
		/*		+ 여러 개의 tuning location을 받았을 때에는 첫 번째 것만 사용하고 나머지는 무시 	*/
		/*	- Tuning 된 서비스에 대한 추가 정보가 전송 됨 (optionally) 								*/
		/*		+ User에게 service와 description 정보를 제공하기 위해 사용 							*/
		/*		+ Ex) channel banner나 info dialogues를 띄우기 위해 								*/
		/*		+ Host가 이러한 정보를 DVB stack에 보내는 방법은 정의되지 않음 						*/
		/* 2. 현재 구현되어 있는 사항 																*/
		/*	- 전달 받은 Descriptors 첫번째 descriptor 정보를 기반으로 Tunning					 	*/
		/*		+ 가장 첫번째 descriptor는 무조건 delivery system descriptor가 있다고 가정 함 		*/
		/*	- TER, SAT, CAB에 대해 각각 처리부분 구현 되어 있지만 T2, S2, C2에 대한 처리는 못함		*/
		/* 3. 추가 구현해야 할 사항																	*/
		/*	- T2, S2, C2에 대한 처리 필요															*/
		/*	- 전달 받은 Descriptors에 여러개의 descriptor가 있는 경우 각 descriptor 처리 필요		*/
		/*		+ usDescriptorsLen을 참고하여 뒷 부분에 desc가 더 있으면 처리 하도록 한다.			*/
		/*		+ Delivery System Descriptor를 일단 하나 처리 했으면 혹시라도 뒤에 존재하는 DSD 무시*/
		/*	- 추가 정보에 대한 descriptor 처리 필요													*/
		/*		+ service / short_event / component / parental_rating / content descriptors			*/
		/********************************************************************************************/

		ulDesclen = pstTuneInfo->utInfo.stBroadcastReq.usDescriptorsLen;
		HxLOG_Info("ulDesclen = %d \n", ulDesclen);

		while (ulDesclen != 0)
		{
			ucTag = *pstTuneInfo->utInfo.stBroadcastReq.pucDescriptors;
			ulTagLen = pstTuneInfo->utInfo.stBroadcastReq.pucDescriptors[1];
			HxLOG_Info("ucTag = 0x%x, ulTagLen = %d \n", ucTag, ulTagLen);

			pDesInfo = svc_cas_SubSvcSiParseDescriptor(svc_cas_MgrGetTextEncoding(svc_cas_SubSvcRmGetActionId(hAction)),
															pstTuneInfo->utInfo.stBroadcastReq.pucDescriptors);

			switch (ucTag)
			{
				case TAG_SATELLITE_DELIVERY_SYSTEM:	// 0x43
					HxLOG_Info("TAG_SATELLITE_DELIVERY_SYSTEM(0x%x) \n", ucTag);
					s_stCasTuneInfo.utInfo.stBroadcastReq.eDeliType = eDxDELIVERY_TYPE_SAT;
					hErr = svc_cas_SubSvcSiGetTuningInfoFromSatDSD(pDesInfo, &(s_stCasTuneInfo.utInfo.stBroadcastReq.stTuningDesInfo));
					if (hErr != ERR_OK)
						HxLOG_Error("ERROR!!\n");
					break;
				case TAG_TERRESTRIAL_DELIVERY_SYSTEM:	// 0x5A
					HxLOG_Info("TAG_TERRESTRIAL_DELIVERY_SYSTEM(0x%x) \n", ucTag);
					s_stCasTuneInfo.utInfo.stBroadcastReq.eDeliType = eDxDELIVERY_TYPE_TER;
					hErr = svc_cas_SubSvcSiGetTuningInfoFromTerDSD(pDesInfo, &(s_stCasTuneInfo.utInfo.stBroadcastReq.stTuningDesInfo));
					if (hErr != ERR_OK)
						HxLOG_Error("ERROR!!\n");
					break;
				case TAG_CABLE_DELIVERY_SYSTEM:		// 0x44
					HxLOG_Info("TAG_CABLE_DELIVERY_SYSTEM(0x%x) \n", ucTag);
					s_stCasTuneInfo.utInfo.stBroadcastReq.eDeliType = eDxDELIVERY_TYPE_CAB;
					hErr = svc_cas_SubSvcSiGetTuningInfoFromCabDSD(pDesInfo, &(s_stCasTuneInfo.utInfo.stBroadcastReq.stTuningDesInfo));
					if (hErr != ERR_OK)
						HxLOG_Error("ERROR!!\n");
					break;

				case TAG_S2_SATELLITE_DELIVERY_SYSTEM:	// 0x79
					HxLOG_Info("need to implement TAG_S2_SATELLITE_DELIVERY_SYSTEM(0x%x) \n", ucTag);
					break;

				case TAG_EXT_T2_DELIVERY_SYSTEM:	// 0x7F, 0x04
					HxLOG_Info("need to implement TAG_EXT_T2_DELIVERY_SYSTEM(0x%x) \n", ucTag);
					break;
/*
				case TAG_EXT_C2_DELIVERY_SYSTEM:	// 0x7F, 0x0D
					HxLOG_Info("need to implement TAG_EXT_C2_DELIVERY_SYSTEM(0x%x) \n", ucTag);
					break;
*/
				case TAG_SERVICE:	// 0x48
					HxLOG_Info("need to implement TAG_SERVICE(0x%x) \n", ucTag);
					break;

				case TAG_SHORT_EVENT:	// 0x4D
					HxLOG_Info("need to implement TAG_SHORT_EVENT(0x%x) \n", ucTag);
					break;

				case TAG_COMPONENT:	// 0x50
					HxLOG_Info("need to implement TAG_COMPONENT(0x%x) \n", ucTag);
					break;

				case TAG_PARENTAL_RATING:	// 0x55
					HxLOG_Info("need to implement TAG_PARENTAL_RATING(0x%x) \n", ucTag);
					break;

				case TAG_CONTENT:	// 0x54
					HxLOG_Info("need to implement TAG_CONTENT(0x%x) \n", ucTag);
					break;

				default:
					HxLOG_Info("not to support(0x%x) \n", ucTag);
					break;
			}

			// next descriptor
			pstTuneInfo->utInfo.stBroadcastReq.pucDescriptors = pstTuneInfo->utInfo.stBroadcastReq.pucDescriptors + ulTagLen + 2;
			ulDesclen = ulDesclen - (ulTagLen + 2);

			ulCnt++;
			if (ulCnt > 15)
			{
				HxLOG_Info("preventing endless loop !!!\n");
				break;
			}

			svc_cas_SubSvcSiFreeDescriptor (pDesInfo);
		}

		if (pstTuneInfo->utInfo.stBroadcastReq.bPmtFlag)
		{
			SvcCas_SubPmtProgInfo_t	stProgramInfo;
			HUINT32					nElemCnt;
			int						i;

			SvcCas_CaDescriptor_t	*pCaDescInfo = NULL;
			HUINT8 					*ppucDesRaw = NULL;
			HUINT32 				ulDesLen = 0;

			hErr = svc_cas_SubSvcSiGetPmtProgramInfoFromRaw(pstTuneInfo->utInfo.stBroadcastReq.pucPmt, &stProgramInfo);

			if (hErr != ERR_OK)
			{
				HxLOG_Error("Wrong PMT data.\n");
				return ERR_FAIL;
			}
			s_stCasTuneInfo.utInfo.stBroadcastReq.stPmtInfo.usServiceId = stProgramInfo.usProgNum;
			s_stCasTuneInfo.utInfo.stBroadcastReq.stPmtInfo.usPcrPid= stProgramInfo.usPCRPid;

			hErr = svc_cas_SubSvcSiCountPmtElementFromRaw(pstTuneInfo->utInfo.stBroadcastReq.pucPmt, &nElemCnt);

			if (hErr != ERR_OK)
			{
				HxLOG_Error("Wrong PMT data.\n");
				return ERR_FAIL;
			}
			s_stCasTuneInfo.utInfo.stBroadcastReq.stPmtInfo.usEleNum= nElemCnt;

			if (nElemCnt > MAX_ES_NUM_FOR_TUNING)
			{
				HxLOG_Error("There are too many Element IDs, so the value of MAX_ES_NUM_FOR_TUNING should be increased.\n");	// 이 에러 메시지가 출력될 정도면 동적으로 메모리 할당하고 ES data 정보를 저장해야 할 듯.
				return ERR_FAIL;
			}
			for ( i=0; i<nElemCnt; i++)
			{
				pCaDescInfo = &s_stCasTuneInfo.utInfo.stBroadcastReq.stPmtInfo.stEleInfo[i].stElementCaDescriptor[0];

				hErr = svc_cas_SubSvcSiGetPmtElementInfoFromRaw(pstTuneInfo->utInfo.stBroadcastReq.pucPmt, i, (SvcCas_SubPmtElemInfo_t *)&s_stCasTuneInfo.utInfo.stBroadcastReq.stPmtInfo.stEleInfo[i]);
				if (hErr != ERR_OK)
				{
					HxLOG_Error("Wrong PMT data.\n");
					return ERR_FAIL;
				}

				// pCaDescInfo->pucRaw 를 채운다.
				if (s_stCasTuneInfo.utInfo.stBroadcastReq.stPmtInfo.stEleInfo[i].ucstream_type == TAG_VIDEO_STREAM)
				{
					SIxTextEncoding_e	eTextEnc = svc_cas_MgrGetTextEncoding(svc_cas_SubSvcRmGetActionId(hAction));
					HUINT16 			usCaPid;
					HUINT16 			usCaSysId;

					hErr = svc_cas_SubSvcSiGetCaPidFromRaw(pstTuneInfo->utInfo.stBroadcastReq.pucPmt, eTextEnc, i, &usCaPid);
					if (hErr == ERR_OK)
					{
						pCaDescInfo->usCaPid = usCaPid;
					}

					hErr = svc_cas_SubSvcSiGetCaSysIdFromRaw(pstTuneInfo->utInfo.stBroadcastReq.pucPmt, eTextEnc, i, &usCaSysId);
					if (hErr == ERR_OK)
					{
						pCaDescInfo->usCaSysId = usCaSysId;
					}

					hErr = svc_cas_SubSvcSiGetPmtElementDesRawFromRaw(pstTuneInfo->utInfo.stBroadcastReq.pucPmt, i, TAG_CA, 0, &ulDesLen, &ppucDesRaw);
					if (hErr == ERR_OK && ppucDesRaw != NULL)
					{
						pCaDescInfo->pucRaw = (HUINT8 *)OxCAS_Malloc(sizeof(HUINT8) * ulDesLen);
						if(pCaDescInfo->pucRaw == NULL)
						{
							HxLOG_Critical("\n\n");
							svc_cas_SubSvcSiFreeDescriptorRaw(ppucDesRaw);
							return ERR_FAIL;
						}
						HxSTD_memcpy(pCaDescInfo->pucRaw, ppucDesRaw, sizeof(HUINT8) * ulDesLen);
						pCaDescInfo->ucRawLen = ulDesLen;

						svc_cas_SubSvcSiFreeDescriptorRaw(ppucDesRaw);
						ppucDesRaw = NULL;
						ulDesLen = 0;
					}

				}

				// pCaDescInfo->pucRaw 를 채운다.
				if (s_stCasTuneInfo.utInfo.stBroadcastReq.stPmtInfo.stEleInfo[i].ucstream_type == TAG_AUDIO_STREAM)
				{
					SIxTextEncoding_e	eTextEnc = svc_cas_MgrGetTextEncoding(svc_cas_SubSvcRmGetActionId(hAction));
					HUINT16 			usCaPid;
					HUINT16 			usCaSysId;

					hErr = svc_cas_SubSvcSiGetCaPidFromRaw(pstTuneInfo->utInfo.stBroadcastReq.pucPmt, eTextEnc, i, &usCaPid);
					if (hErr == ERR_OK)
					{
						pCaDescInfo->usCaPid = usCaPid;
					}

					hErr = svc_cas_SubSvcSiGetCaSysIdFromRaw(pstTuneInfo->utInfo.stBroadcastReq.pucPmt, eTextEnc, i, &usCaSysId);
					if (hErr == ERR_OK)
					{
						pCaDescInfo->usCaSysId = usCaSysId;
					}

					hErr = svc_cas_SubSvcSiGetPmtElementDesRawFromRaw(pstTuneInfo->utInfo.stBroadcastReq.pucPmt, i, TAG_CA, 0, &ulDesLen, &ppucDesRaw);
					if (hErr == ERR_OK && ppucDesRaw != NULL)
					{
						pCaDescInfo->pucRaw = (HUINT8 *)OxCAS_Malloc(sizeof(HUINT8) * ulDesLen);
						if(pCaDescInfo->pucRaw == NULL)
						{
							HxLOG_Critical("\n\n");
							svc_cas_SubSvcSiFreeDescriptorRaw(ppucDesRaw);
							return ERR_FAIL;
						}
						HxSTD_memcpy(pCaDescInfo->pucRaw, ppucDesRaw, sizeof(HUINT8) * ulDesLen);
						pCaDescInfo->ucRawLen = ulDesLen;

						svc_cas_SubSvcSiFreeDescriptorRaw(ppucDesRaw);
						ppucDesRaw = NULL;
						ulDesLen = 0;
					}
				}

#if defined(_CI_DEBUG_)
{
				HUINT32 iidx =0;

				HxLOG_Info("usCaSysId = 0x%x \n", pCaDescInfo->usCaSysId);
				HxLOG_Info("usCaPid = 0x%x \n", pCaDescInfo->usCaPid);

				HxLOG_Info("pucRaw = 0x%x \n", pCaDescInfo->pucRaw);
				HxLOG_Info("ucRawLen = %d \n", pCaDescInfo->ucRawLen);

				HxLOG_Info("received pucRaw data[%d]: \n", pCaDescInfo->ucRawLen);
				for ( iidx = 0; iidx < pCaDescInfo->ucRawLen; iidx++)
				{
					HxLOG_Print("%02X ", pCaDescInfo->pucRaw[iidx]);

					if (iidx % 16 == (16-1))	  {
						HxLOG_Print("\n");
					}
				}
				HxLOG_Print("\n");
}
#endif

				HxLOG_Info("s_stCasTuneInfo.utInfo.stBroadcastReq.stPmtInfo. \n");
				HxLOG_Info("\t stEleInfo[i].stElementCaDescriptor[0].usCaSysId = 0x%x \n", s_stCasTuneInfo.utInfo.stBroadcastReq.stPmtInfo.stEleInfo[i].stElementCaDescriptor[0].usCaSysId);
				HxLOG_Info("\t stEleInfo[i].stElementCaDescriptor[0].usCaPid = 0x%x \n", s_stCasTuneInfo.utInfo.stBroadcastReq.stPmtInfo.stEleInfo[i].stElementCaDescriptor[0].usCaPid);
				HxLOG_Info("\t stEleInfo[i].stElementCaDescriptor[0].pucRaw = 0x%x \n", s_stCasTuneInfo.utInfo.stBroadcastReq.stPmtInfo.stEleInfo[i].stElementCaDescriptor[0].pucRaw);
				HxLOG_Info("\t stEleInfo[i].stElementCaDescriptor[0].ucRawLen = %d \n", s_stCasTuneInfo.utInfo.stBroadcastReq.stPmtInfo.stEleInfo[i].stElementCaDescriptor[0].ucRawLen);

			}
			HxLOG_Info("Action Handle (0x%x), Mode (%d), bPmtFlag (%d), usServiceId (0x%x), usDescriptorsLen(%d)\n",
				hAction, s_stCasTuneInfo.eMode, s_stCasTuneInfo.utInfo.stBroadcastReq.bPmtFlag, s_stCasTuneInfo.utInfo.stBroadcastReq.usServiceId, s_stCasTuneInfo.utInfo.stBroadcastReq.usDescriptorsLen);
		}
	}
#endif
	else
	{
		HxLOG_Error("ERROR: pstTuneInfo->eTuneMode!!(%d)\n", pstTuneInfo->eTuneMode);
		s_stCasTuneInfo.eMode = eCasTuneMode_Cancel;
		return ERR_FAIL;
	}


	hErr = svc_cas_MgrSubUpdateCasEtcEventData(hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM, eSEVT_CAS_CAM_TUNE, pCamTuneSt->usSlotId, eDxCAS_GROUPID_CAM, (void*)&s_stCasTuneInfo, sizeof(SvcCas_TuneInfo_t));
	if(hErr != ERR_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HINT32 local_CiMgr_ProcPinCapaInfo(CI_SLOT_ID usSlotId)
{
	HERROR			hErr = ERR_FAIL;
	CI_MANAGE_CA_INST_PARAM stParam;
	int i;

	HxLOG_Print("usSlotId (%d)\n", usSlotId);

#if defined (CONFIG_MW_CI_PLUS_V_1_3)
	for(i = 0; i < CI_MAX_NUM_OF_ACT; i++)
	{
		if (s_astCiContext[i].eChType != eCasChannel_Scramble || s_astCiContext[i].bRecordProcessDone == TRUE)
		{
			continue;
		}

		HxLOG_Print("s_astCiContext[%d].pCam (0x%x)\n", i, s_astCiContext[i].pCam);
		if(s_astCiContext[i].pCam != NULL)
		{
			if(s_astCiContext[i].pCam->usSlotId == usSlotId)
			{
				if(s_astCiContext[i].eActType == eCAS_ActionType_Tsr_Rec || s_astCiContext[i].eActType == eCAS_ActionType_Recording)
				{
					stParam.unSlotId = (HUINT32)usSlotId;
					stParam.hAction = s_astCiContext[i].hAction;

					hErr = local_CiMgr_OperateCCv2RecordProcess(&stParam, &s_astCiContext[i]);
					if (hErr != ERR_OK)
					{
						s_astCiContext[i].bRecordProcessDone = FALSE;
						HxLOG_Error("local_CiMgr_OperateCCv2RecordProcess() ERROR!!\n");
						return ERR_FAIL;
					}
					s_astCiContext[i].bRecordProcessDone = TRUE;
					hErr = local_CiMgr_StartDescramble(&s_astCiContext[i], usSlotId, 0, 0);
					if(hErr != ERR_OK)
					{
						HxLOG_Critical("Critical Error!!!\n");
					}
				}
			}
		}
	}
#endif
	return hErr;
}

static void local_CiMgr_ParseCiPlusServiceDes (HUINT8 *p, CI_CiplusSvcDes_t *pDes)
{
//	HUINT8 ucDesLen;
	HUINT8 ucTemp;

	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	/** < descriptor tag (1byte) */
	p++;

	/** < descriptor length (1byte) */
//	ucDesLen = *p++;

	pDes->usSvcId = get16bit(p);
	p += 2;

	pDes->ucSvcType = *p++;

	ucTemp = *p;
	pDes->ucVisible = ((ucTemp >> 7) & 0x01);
	pDes->ucSelectable = ((ucTemp >> 6) & 0x01);

	pDes->usLcn = get14bit(p);
	p += 2;

	pDes->ucSvcProviderNameLen = *p;
	if(pDes->ucSvcProviderNameLen)
		HxSTD_memcpy(pDes->aucSvcProviderName, p+1, pDes->ucSvcProviderNameLen);
	p += (pDes->ucSvcProviderNameLen + 1);

	pDes->ucSvcNameLen = *p;
	if(pDes->ucSvcNameLen)
		HxSTD_memcpy(pDes->aucSvcName, p+1, pDes->ucSvcNameLen);
	p += (pDes->ucSvcNameLen + 1);

	HxLOG_Info("-[%s]...\n", __FUNCTION__);
}

static void local_CiMgr_FreeOperatorSvc(void *pvData)
{
	SvcCas_OperatorSvc_t *pstSvc = (SvcCas_OperatorSvc_t *)pvData;

	if(pstSvc == NULL)
	{
		HxLOG_Error("null\n");
		return;
	}

	OxCAS_Free(pstSvc);
}

static void local_CiMgr_FreeOperatorNitTsLoop(void *pvData)
{
	CI_OperatorNitTsLoop_t *pstTsLoop = (CI_OperatorNitTsLoop_t *)pvData;

	if(pstTsLoop == NULL)
	{
		HxLOG_Error("null\n");
		return;
	}

	pstTsLoop->pSvcList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pstTsLoop->pSvcList, local_CiMgr_FreeOperatorSvc);

	OxCAS_Free(pstTsLoop);
}

static void local_CiMgr_FreeOperatorNit(void *pvData)
{
	CI_OperatorNit_t *pstOpNit = (CI_OperatorNit_t *)pvData;

	if(pstOpNit == NULL)
	{
		HxLOG_Error("null\n");
		return;
	}

	if(pstOpNit->pucNitRaw)
		OxCAS_Free(pstOpNit->pucNitRaw);

	pstOpNit->pTsLoopList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pstOpNit->pTsLoopList, local_CiMgr_FreeOperatorNitTsLoop);

	OxCAS_Free(pstOpNit);
}

static void local_CiMgr_FreeOperatorTune(void *pvData)
{
	CI_OperatorTune_t *pstOpTune = NULL;

	pstOpTune = (CI_OperatorTune_t *)pvData;
	if(pstOpTune == NULL)
	{
		HxLOG_Error("null\n");
		return;
	}

	if(pstOpTune->pucDes)
		OxCAS_Free(pstOpTune->pucDes);

	OxCAS_Free(pstOpTune);
}

static void local_CiMgr_FreeCiCamNit(CI_OperatorProfile_t *pstContents)
{
	pstContents->pNitList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pstContents->pNitList, local_CiMgr_FreeOperatorNit);
}

static HERROR local_CiMgr_ParseCiCamNitToNitRaw(CI_PLUS_OPERATOR_NIT_PARAM *pstParam, CI_OperatorProfile_t *pstContents)
{
	HUINT16			usCiCamNitLen, usSecLen;
	HUINT8			*pucCiCamNit;

	CI_OperatorNit_t	*pstOpNit;

	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	if(pstParam->pucCiCamNit == NULL)
	{
		HxLOG_Error("pstParam->pucCiCamNit is NULL...\n");
		return ERR_FAIL;
	}

	local_CiMgr_FreeCiCamNit(pstContents);

	pucCiCamNit		= pstParam->pucCiCamNit;
	usCiCamNitLen	= pstParam->usCiCamNitLen;

	while(usCiCamNitLen > 0)
	{
		if(*pucCiCamNit != NIT_ACTUAL_TABLE_ID)
		{
			HxLOG_Error("incorrect NIT TABLE ID...(%x)\n", *pucCiCamNit);
			local_CiMgr_FreeCiCamNit(pstContents);
			if(pstParam->pucCiCamNit != NULL)
				OxCAS_Free(pstParam->pucCiCamNit);
			return ERR_FAIL;
		}

		pstOpNit = (CI_OperatorNit_t *)OxCAS_Malloc(sizeof(CI_OperatorNit_t));
		if(pstOpNit == NULL)
		{
			HxLOG_Error("OxCAS_Malloc()...\n");
			local_CiMgr_FreeCiCamNit(pstContents);
			if(pstParam->pucCiCamNit != NULL)
				OxCAS_Free(pstParam->pucCiCamNit);
			return ERR_FAIL;
		}

		pstContents->pNitList = UTIL_LINKEDLIST_AppendListItemToPointerList(pstContents->pNitList, (void *)pstOpNit);

		HxSTD_memset(pstOpNit, 0, sizeof(CI_OperatorNit_t));

		usSecLen = get12bit(pucCiCamNit + 1) + 3;
		pstOpNit->pucNitRaw = (HUINT8 *)OxCAS_Malloc(usSecLen);
		if(pstOpNit->pucNitRaw == NULL)
		{
			HxLOG_Error("pucNewNit NULL!!\n");
			local_CiMgr_FreeCiCamNit(pstContents);
			if(pstParam->pucCiCamNit != NULL)
				OxCAS_Free(pstParam->pucCiCamNit);
			return ERR_FAIL;
		}

		HxSTD_memcpy(pstOpNit->pucNitRaw, pucCiCamNit, usSecLen);

		pucCiCamNit += usSecLen;
		usCiCamNitLen -= usSecLen;
	}

	if(pstParam->pucCiCamNit != NULL)
		OxCAS_Free(pstParam->pucCiCamNit);

	HxLOG_Info("-[%s]...\n", __FUNCTION__);

	return ERR_OK;
}

static HERROR local_CiMgr_ParseNitRawForCam(CI_OperatorProfile_t *pstContents)
{
	HERROR				hErr = ERR_OK;
	HUINT8		*pucNitRaw, ucTag, ucDesLen;
	HUINT16 	usNetNum = 0, usNetLoopLen, usTsLoopLen, usTsDesLoopLen;
	HUINT32 	dwPrivateDataSpec = 0;

	CI_OperatorNit_t		*pstOpNit;
	SvcCas_OperatorSvc_t	*pstNewSvc;
	CI_CiplusSvcDes_t		stCiplusSvcDes;

	POINTER_LIST_T			*pNitList = NULL;
	CI_OperatorNitTsLoop_t	*pstTsLoop;

	void					*pvTuningDes = NULL;

	SiTxtEncodedType_t		eEncType;
	HUINT8					aucCodeTable[4] = {0,}, szIso639LangCode[4] = {0,}, *pszText = NULL;
	HUINT32 				nPrefixLen = 0;

	HINT32					 i;
	Handle_t				 hAction;
	SIxTextEncoding_e		 eTextEnc;

	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	hAction = local_CiMgr_GetActionBySlotId(pstContents->usSlotId);
	eTextEnc = svc_cas_MgrGetTextEncoding(svc_cas_SubSvcRmGetActionId(hAction));

	HxSTD_memcpy (aucCodeTable, &pstContents->aucTextCoding[0], 4);

	switch (aucCodeTable[0])
	{
	case 0x00:		nPrefixLen = 0; 	break;		// 6937
	case 0x1f:		nPrefixLen = 2; 	break;		// Described by encoding_type_id
	case 0x10:
		if (aucCodeTable[2] == 0x00)
		{
			/* 0x05 ~ 0x0F 까지는 1byte 로 변환해서 처리함. (-> local_mw_cp_ConvertUCS2To8859_X 함수 참조하면 1byte 로 동작하는 pagecode는 3byte 지원하지 않아 1byte로 변환해야함)*/
			switch (aucCodeTable[3])
			{
			case 0x05:	nPrefixLen = 1; 	aucCodeTable[0] = 0x01; 	break;
			case 0x06:	nPrefixLen = 1; 	aucCodeTable[0] = 0x02; 	break;
			case 0x07:	nPrefixLen = 1; 	aucCodeTable[0] = 0x03; 	break;
			case 0x08:	nPrefixLen = 1; 	aucCodeTable[0] = 0x04; 	break;
			case 0x09:	nPrefixLen = 1; 	aucCodeTable[0] = 0x05; 	break;
			case 0x0A:	nPrefixLen = 1; 	aucCodeTable[0] = 0x06; 	break;
			case 0x0B:	nPrefixLen = 1; 	aucCodeTable[0] = 0x07; 	break;
			case 0x0C:	nPrefixLen = 1; 	aucCodeTable[0] = 0x08; 	break;
			case 0x0D:	nPrefixLen = 1; 	aucCodeTable[0] = 0x09; 	break;
			case 0x0E:	nPrefixLen = 1; 	aucCodeTable[0] = 0x0A; 	break;
			case 0x0F:	nPrefixLen = 1; 	aucCodeTable[0] = 0x0B; 	break;
			default:	nPrefixLen = 3; // 8859-x
			}
		}
		break;

	default:	nPrefixLen = 1; // 8859-x & etc
	}

	HxSTD_memcpy (szIso639LangCode, &pstContents->aucTextCoding[4], 3);
	szIso639LangCode [3] = '\0';

	usNetNum = UTIL_LINKEDLIST_GetNumListItemsInPointerList(pstContents->pNitList, NULL, NULL);

	for(i = 0 ; i < usNetNum ; i++)
	{
		pNitList = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pstContents->pNitList, i, NULL, NULL);
		if(pNitList == NULL)
		{
			HxLOG_Error("[%s] - NIT list NULL\n", __FUNCTION__);
			return ERR_FAIL;
		}

		pstOpNit = (CI_OperatorNit_t *)UTIL_LINKEDLIST_GetContents(pNitList);
		pucNitRaw = pstOpNit->pucNitRaw;

		if(*pucNitRaw != NIT_ACTUAL_TABLE_ID)
		{
			HxLOG_Error("[%s] - incorrect NIT table id(%x)\n", __FUNCTION__, *pucNitRaw);
			return ERR_FAIL;
		}

		HxLOG_Warning("[%s] NIT (%d) section parsing...\n", __FUNCTION__, i);

		pucNitRaw++;		// table id
		pucNitRaw += 2; 	// section len

		pstOpNit->usNetId = get16bit(pucNitRaw);
		pucNitRaw += 2; 	// network id

		pucNitRaw += 3; 	// version, current next ind | section number | last section number

		usNetLoopLen = get12bit(pucNitRaw);
		pucNitRaw += 2;

		while(usNetLoopLen > 0)
		{
			ucTag		= *pucNitRaw;
			ucDesLen	= *(pucNitRaw + 1);

			switch(ucTag)
			{
				case TAG_NETWORK_NAME:
					HxLOG_Print("[%s] network loop - TAG_NETWORK_NAME!!!\n", __FUNCTION__);
					if(ucDesLen)
					{
						HxSTD_memcpy(pstOpNit->szNetName, pucNitRaw + 2, ucDesLen);
						pstOpNit->szNetName[ucDesLen] = '\0';
					}
					break;

				case TAG_LINKAGE:
					HxLOG_Print("[%s] network loop - TAG_LINKAGE!!!\n", __FUNCTION__);
					break;

				case TAG_CIPLUS_CONTENT_LABEL:
					HxLOG_Print("[%s] network loop - TAG_CIPLUS_CONTENT_LABEL!!!\n", __FUNCTION__);
					break;

				case TAG_PRIVATE_DATA_SPECIFIER:
					HxLOG_Print("[%s] network loop - TAG_PRIVATE_DATA_SPECIFIER!!!\n", __FUNCTION__);
					break;

				case TAG_EXTENSION:
					HxLOG_Print("[%s] network loop - TAG_EXTENSION!!!\n", __FUNCTION__);
					break;

				default:
					HxLOG_Print("[%s] - net loop descriptor tag(%x)\n", __FUNCTION__, ucTag);
					break;
			}

			usNetLoopLen -= (ucDesLen + 2);
			pucNitRaw += (ucDesLen + 2);
		}

		pucNitRaw += usNetLoopLen;

		usTsLoopLen = get12bit(pucNitRaw);
		pucNitRaw += 2;

		while(usTsLoopLen > 0)
		{
			pstTsLoop = (CI_OperatorNitTsLoop_t *)OxCAS_Malloc(sizeof(CI_OperatorNitTsLoop_t));
			if(pstTsLoop == NULL)
			{
				HxLOG_Error("[%s] ts loop - OxCAS_Malloc!!!\n", __FUNCTION__);
				return ERR_FAIL;
			}

			pstOpNit->pTsLoopList = UTIL_LINKEDLIST_AppendListItemToPointerList(pstOpNit->pTsLoopList, (void *)pstTsLoop);

			HxSTD_memset(pstTsLoop, 0, sizeof(CI_OperatorNitTsLoop_t));

			pstTsLoop->usTsId	= get16bit(pucNitRaw);
			pstTsLoop->usOnId	= get16bit(pucNitRaw + 2);
			usTsDesLoopLen		= get12bit(pucNitRaw + 4);

			usTsLoopLen -= 6;
			pucNitRaw += 6;

			usTsLoopLen -= usTsDesLoopLen;

			while(usTsDesLoopLen > 0)
			{
				ucTag = *pucNitRaw;
				ucDesLen = *(pucNitRaw + 1);

				HxLOG_Print("[%s] ts loop - tag (0x%x / %d)!!!\n", __FUNCTION__, ucTag, ucDesLen);

				switch(ucTag)
				{
					case TAG_SATELLITE_DELIVERY_SYSTEM:
						pvTuningDes = svc_cas_SubSvcSiParseDescriptor(eTextEnc, pucNitRaw);
						if(pvTuningDes == NULL)
						{
							return ERR_FAIL;
						}
						pstTsLoop->eDelivery = eDxDELIVERY_TYPE_SAT;
						hErr = svc_cas_SubSvcSiGetTuningInfoFromSatDSD(pvTuningDes, &pstTsLoop->stTuningDes);
						if (hErr != ERR_OK)
							HxLOG_Error("svc_cas_SubSvcSiGetTuningInfoFromSatDSD() ERROR!!\n");
						hErr = svc_cas_SubSvcSiFreeDescriptor(pvTuningDes);
						if (hErr != ERR_OK)
							HxLOG_Error("svc_cas_SubSvcSiFreeDescriptor() ERROR!!\n");
						break;

					case TAG_TERRESTRIAL_DELIVERY_SYSTEM:
						pvTuningDes = svc_cas_SubSvcSiParseDescriptor(eTextEnc, pucNitRaw);
						if(pvTuningDes == NULL)
						{
							return ERR_FAIL;
						}
						pstTsLoop->eDelivery = eDxDELIVERY_TYPE_TER;
						if(svc_cas_SubSvcSiGetTuningInfoFromTerDSD(pvTuningDes, &pstTsLoop->stTuningDes) != ERR_OK)
						{
							HxLOG_Error("ERROR!!\n");
						}
						hErr = svc_cas_SubSvcSiFreeDescriptor(pvTuningDes);
						if (hErr != ERR_OK)
							HxLOG_Error("svc_cas_SubSvcSiFreeDescriptor() ERROR!!\n");
						break;

					case TAG_CABLE_DELIVERY_SYSTEM:
						pvTuningDes = svc_cas_SubSvcSiParseDescriptor(eTextEnc, pucNitRaw);
						if(pvTuningDes == NULL)
						{
							return ERR_FAIL;
						}
						pstTsLoop->eDelivery = eDxDELIVERY_TYPE_CAB;
						if(svc_cas_SubSvcSiGetTuningInfoFromCabDSD(pvTuningDes, &pstTsLoop->stTuningDes) != ERR_OK)
						{
							HxLOG_Error("ERROR!!\n");
						}
						hErr = svc_cas_SubSvcSiFreeDescriptor(pvTuningDes);
						if (hErr != ERR_OK)
							HxLOG_Error("svc_cas_SubSvcSiFreeDescriptor() ERROR!!\n");
						break;

					case TAG_PRIVATE_DATA_SPECIFIER:
						dwPrivateDataSpec = get32bit (pucNitRaw + 2);
						HxLOG_Print("Private Data Spec [%d]\n", dwPrivateDataSpec);
						break;

					case TAG_CIPLUS_SERVICE:
						if (CI_PLUS_PRIVATE_DATA_VALUE == dwPrivateDataSpec)	// 만약 이 조건에 걸렸을 때만 parsing 하는 것이 문제가 되면 if 조건 제거 하도록.
						{
							local_CiMgr_ParseCiPlusServiceDes(pucNitRaw, &stCiplusSvcDes);
							pstNewSvc = (SvcCas_OperatorSvc_t *)OxCAS_Malloc(sizeof(SvcCas_OperatorSvc_t));
							if(pstNewSvc == NULL)
							{
								return ERR_FAIL;
							}

							pstTsLoop->pSvcList = UTIL_LINKEDLIST_AppendListItemToPointerList(pstTsLoop->pSvcList, (void *)pstNewSvc);

							HxSTD_memset(pstNewSvc, 0, sizeof(SvcCas_OperatorSvc_t));

							pstNewSvc->usSvcId		= stCiplusSvcDes.usSvcId;
							pstNewSvc->ucSvcType	= stCiplusSvcDes.ucSvcType;
							pstNewSvc->bVisible 	= stCiplusSvcDes.ucVisible;
							pstNewSvc->bSelectable	= stCiplusSvcDes.ucSelectable;
							pstNewSvc->usLcn		= stCiplusSvcDes.usLcn;

							// provider & service name은 UTF8  형식으로 저장 (base system의 정책에 따라 변경)
							MWC_UTIL_GetSiStringEncodedType(&eEncType); // backup string encoded type

							/* First byte value */
							if (aucCodeTable[0] == 0x00) // control byte가 0 이면 6937 을 default 사용
							{
								MWC_UTIL_SetSiStringEncodedType(eSITEXT_ENCODED_300468);
							}
							else
							{
								MWC_UTIL_SetSiStringEncodedType(eSITEXT_ENCODED_300468_EUROPE); // 0x00 이외에는 8859-9을 default 사용
							}

							if(stCiplusSvcDes.ucSvcProviderNameLen)
							{
								HUINT8	szName[256] = {0,};
								HUINT32 nNameLen, nTempNameLen;

								/*	버퍼 256 보다 크지 않게 하기 위함 */
								nTempNameLen = (stCiplusSvcDes.ucSvcProviderNameLen > (255 - nPrefixLen)) ? (255 - nPrefixLen) : stCiplusSvcDes.ucSvcProviderNameLen;

								/* add table code value*/
								if (nPrefixLen == 1)
								{
									szName[0] = aucCodeTable[0]; // First byte value
								}
								else if (nPrefixLen == 2)
								{
									szName[0] = aucCodeTable[0]; // Described by encoding_type_id
									szName[1] = aucCodeTable[1]; // encoding_type_id
								}
								else if (nPrefixLen == 3)
								{
									szName[0] = aucCodeTable[0]; // First byte value
									szName[1] = aucCodeTable[2]; // Second byte value
									szName[2] = aucCodeTable[3]; // Third byte value
								}

								HxSTD_memcpy(szName + nPrefixLen, stCiplusSvcDes.aucSvcProviderName, nTempNameLen);
								szName[nTempNameLen + nPrefixLen] = '\0';

								nTempNameLen = MWC_UTIL_DvbStrlen(szName);

								nNameLen = MWC_UTIL_ConvSiStringToUtf8(szIso639LangCode, nTempNameLen, szName, &pszText);
								if(pszText != NULL)
								{
									nNameLen = (nNameLen > 255) ? 255 : nNameLen;
									HxSTD_memcpy(pstNewSvc->szSvcPrvName, pszText, nNameLen);
									pstNewSvc->szSvcPrvName[nNameLen] = '\0';
									HLIB_STD_MemFree(pszText);
								}
							}

							if(stCiplusSvcDes.ucSvcNameLen)
							{
								HUINT8	szName[256] = {0,};
								HUINT32 nNameLen, nTempNameLen;

								/*	버퍼 256 보다 크지 않게 하기 위함 */
								nTempNameLen = (stCiplusSvcDes.ucSvcNameLen > (255 - nPrefixLen)) ? (255 - nPrefixLen) : stCiplusSvcDes.ucSvcNameLen;

								/* add table code value*/
								if (nPrefixLen == 1)
								{
									szName[0] = aucCodeTable[0]; // First byte value
								}
								else if (nPrefixLen == 2)
								{
									szName[0] = aucCodeTable[0]; // Described by encoding_type_id
									szName[1] = aucCodeTable[1]; // encoding_type_id
								}
								else if (nPrefixLen == 3)
								{
									szName[0] = aucCodeTable[0]; // First byte value
									szName[1] = aucCodeTable[2]; // Second byte value
									szName[2] = aucCodeTable[3]; // Third byte value
								}

								HxSTD_memcpy(szName + nPrefixLen, stCiplusSvcDes.aucSvcName, nTempNameLen);
								szName[nTempNameLen + nPrefixLen] = '\0';

								nTempNameLen = MWC_UTIL_DvbStrlen(szName);

								nNameLen = MWC_UTIL_ConvSiStringToUtf8(szIso639LangCode, nTempNameLen, szName, &pszText);
								if(pszText != NULL)
								{
									nNameLen = (nNameLen > 255) ? 255 : nNameLen;

									HxSTD_memcpy(pstNewSvc->szSvcName, pszText, nNameLen);
									pstNewSvc->szSvcName[nNameLen] = '\0';
									HLIB_STD_MemFree(pszText);
								}
							}

							MWC_UTIL_SetSiStringEncodedType(eEncType); // restore string encoded type

						}
						break;

					default:
						HxLOG_Print("[%s] - unknown ts loop descriptor tag(%x)\n", __FUNCTION__, ucTag);
						break;
				}

				usTsDesLoopLen -= (ucDesLen + 2);
				pucNitRaw += (ucDesLen + 2);
			}
		}
	}

	HxLOG_Info("-[%s]...\n", __FUNCTION__);

	return ERR_OK;
}


static HERROR local_CiMgr_ParseOperatorTune(CI_PLUS_OPERATOR_TUNE_PARAM *pstParam, CI_OperatorProfile_t *pstContents)
{
	CI_OperatorTune_t		*pstOpTune;
	HUINT16					usDesLoopLen;
	HUINT8					*pucDesLoop, ucDesLen;

	HxLOG_Info("+[%s]...\n", __FUNCTION__);

	if(pstParam->pucDesLoop == NULL)
	{
		HxLOG_Error("pstParam->pucDesLoop is NULL...\n");
		goto _ERROR;
	}

	pstContents->pTuneList	= UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pstContents->pTuneList, local_CiMgr_FreeOperatorTune);

	if(pstContents->pTuneList == NULL)
	{
		pucDesLoop		= pstParam->pucDesLoop;
		usDesLoopLen	= pstParam->usDesLoopLen;

		while(usDesLoopLen > 0)
		{
			ucDesLen = pucDesLoop[1] + 2;

			pstOpTune = (CI_OperatorTune_t *)OxCAS_Malloc(sizeof(CI_OperatorTune_t));
			if(pstOpTune == NULL)
			{
				HxLOG_Error("OxCAS_Malloc() error\n");
				goto _ERROR;
			}

			pstContents->pTuneList = UTIL_LINKEDLIST_AppendListItemToPointerList(pstContents->pTuneList, (void *)pstOpTune);

			pstOpTune->bTuned = FALSE;
			pstOpTune->ucDesLen = ucDesLen;
			pstOpTune->pucDes = (HUINT8 *)OxCAS_Malloc(ucDesLen);
			if(pstOpTune->pucDes == NULL)
			{
				HxLOG_Error("OxCAS_Malloc() error\n");
				goto _ERROR;
			}

			HxSTD_memcpy(pstOpTune->pucDes, pucDesLoop, ucDesLen);

			pucDesLoop		+= ucDesLen;
			usDesLoopLen	-= ucDesLen;
		}
	}

	if(pstParam->pucDesLoop != NULL)
		OxCAS_Free(pstParam->pucDesLoop);
	HxLOG_Info("-[%s]...\n", __FUNCTION__);
	return ERR_OK;

_ERROR:
	if(pstParam->pucDesLoop != NULL)
		OxCAS_Free(pstParam->pucDesLoop);
	pstContents->pTuneList = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pstContents->pTuneList, local_CiMgr_FreeOperatorTune);
	return ERR_FAIL;
}

static HUINT32 local_CiMgr_ProcOperatorPrepareReq(CI_PLUS_OPERATOR_PREPARE_PARAM *pstParam)
{
	Handle_t				hAction = HANDLE_NULL;
	CI_OperatorProfile_t	*pstContents;

	pstContents = &s_stOperatorProfile[pstParam->usSlotId];

	pstContents->usSlotId	= pstParam->usSlotId;
	HxSTD_MemCopy(pstContents->szProfName, pstParam->pszProfileName, 255);
	pstContents->pTuneList	= UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pstContents->pTuneList, local_CiMgr_FreeOperatorTune);
	pstContents->pNitList	= UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pstContents->pNitList, local_CiMgr_FreeOperatorNit);

	hAction = local_CiMgr_GetActionBySlotId(pstParam->usSlotId);
	if(svc_cas_MgrSubUpdateCasEtcEvent(hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM,
		eSEVT_CAS_CIPLUS_OPERATOR_PREPARE_REQ, pstParam->usSlotId, eDxCAS_GROUPID_CAM, pstParam->eOpRefreshReq) != ERR_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HUINT32 local_CiMgr_CheckNoSignalAtMiddle(CI_OperatorProfile_t *pstContents, CI_OperatorTune_t *pstOpTune)
{
	Handle_t 				hAction = HANDLE_NULL;
	void					*pTuningDesInfo = NULL;
	DbSvc_TsCondition_t		stTsCond;
	DbSvc_TsInfo_t			*pstTsInfoList;
	HERROR					hErr;
	HINT32					nDbTsCnt, nDbTsNum;
	HBOOL					bFound = FALSE;

	pstContents->stOpTune.pucDes		= pstOpTune->pucDes;
	pstContents->stOpTune.ucDesLen		= pstOpTune->ucDesLen;

	hAction = local_CiMgr_GetActionBySlotId(pstContents->usSlotId);
	pTuningDesInfo = svc_cas_SubSvcSiParseDescriptor(svc_cas_MgrGetTextEncoding(svc_cas_SubSvcRmGetActionId(hAction)), pstContents->stOpTune.pucDes);
	switch (pstContents->stOpTune.pucDes[0])
	{
		case TAG_SATELLITE_DELIVERY_SYSTEM:
			pstContents->stOpTune.eDeliType = eDxDELIVERY_TYPE_SAT;
			if(svc_cas_SubSvcSiGetTuningInfoFromSatDSD(pTuningDesInfo, &pstContents->stOpTune.stTuningDesInfo) != ERR_OK)
				HxLOG_Error("ERROR!!\n");
			break;

		case TAG_TERRESTRIAL_DELIVERY_SYSTEM:
			pstContents->stOpTune.eDeliType = eDxDELIVERY_TYPE_TER;
			if(svc_cas_SubSvcSiGetTuningInfoFromTerDSD(pTuningDesInfo, &pstContents->stOpTune.stTuningDesInfo) != ERR_OK)
				HxLOG_Error("ERROR!!\n");
			break;

		case TAG_CABLE_DELIVERY_SYSTEM:
			pstContents->stOpTune.eDeliType = eDxDELIVERY_TYPE_CAB;
			if(svc_cas_SubSvcSiGetTuningInfoFromCabDSD(pTuningDesInfo, &pstContents->stOpTune.stTuningDesInfo) != ERR_OK)
				HxLOG_Error("ERROR!!\n");
			break;
		default:
			break;
	}
	svc_cas_SubSvcSiFreeDescriptor (pTuningDesInfo);

	HxLOG_Print("Op frequency (%d)...\n", pstContents->stOpTune.stTuningDesInfo.sat.ulFrequency);

	if (pstContents->stOpTune.stTuningDesInfo.sat.ulFrequency > 0xFFFFF)
	{
		HxLOG_Print("INVALID PARAMETER !!! but need to be tuned.\n");
		return ERR_OK;
	}

	DB_SVC_InitTsFindCondition (&stTsCond);
	stTsCond.nDeliType	= eDxDELIVERY_TYPE_SAT;

	pstTsInfoList = NULL;

	hErr = DB_SVC_FindTsInfoList (&stTsCond, &nDbTsNum, &pstTsInfoList);
	if (hErr == ERR_OK && pstTsInfoList != NULL)
	{
		HxLOG_Print("nDbTsNum = %d\n", nDbTsNum);
		for (nDbTsCnt = 0; nDbTsCnt < nDbTsNum; nDbTsCnt++)
		{
			if (pstTsInfoList[nDbTsCnt].stTuningParam.sat.ulFrequency == pstContents->stOpTune.stTuningDesInfo.sat.ulFrequency
				&& pstTsInfoList[nDbTsCnt].stTuningParam.sat.ulSymbolRate== pstContents->stOpTune.stTuningDesInfo.sat.ulSymbolRate)
			{
				bFound = TRUE;
				break;
			}
		}

		DB_SVC_FreeTsInfoList (pstTsInfoList);
	}

	if (bFound == TRUE)
		return ERR_OK;
	else
		return ERR_FAIL;
}

static HUINT32 local_CiMgr_GetNextOperatorTune(CI_SLOT_ID usSlotId, HUINT8 ucNextDesNum)
{
	Handle_t 				hAction = HANDLE_NULL;
	void					*pTuningDesInfo = NULL;
	CI_OperatorProfile_t	*pstContents;
	CI_OperatorTune_t		*pstOpTune;
	POINTER_LIST_T			*pItem = NULL;
	HUINT16					usTpNum, usDesIdx;

	HxLOG_Print("+++++++++++++++++++++++++++++++++\n");

	pstContents = &s_stOperatorProfile[usSlotId];

	if(pstContents->pTuneList == NULL)
	{
		HxLOG_Error("pstContents->pTuneList = NULL\n");
		return ERR_FAIL;
	}

	usTpNum = UTIL_LINKEDLIST_GetNumListItemsInPointerList(pstContents->pTuneList, NULL, NULL);
	HxLOG_Print("usTpNum = %d\n", usTpNum);

	usDesIdx = ucNextDesNum++;
	if (usDesIdx >= usTpNum)
	{
		HxLOG_Print("last\n");
		return ERR_FAIL;
	}
	HxLOG_Print("usDesIdx = %d\n", usDesIdx);

	pItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pstContents->pTuneList, usDesIdx, NULL, NULL);
	if(pItem == NULL)
	{
		HxLOG_Error("UTIL_LINKEDLIST_FindNthListItemFromPointerList() error\n");
		return ERR_FAIL;
	}

	pstOpTune = (CI_OperatorTune_t *)UTIL_LINKEDLIST_GetContents(pItem);

	if(pstOpTune->bTuned == FALSE)
	{
		pstOpTune->bTuned = TRUE;
		HxLOG_Print("pstOpTune->bTuned = %d\n", pstOpTune->bTuned);
	}

	pstContents->stOpTune.pucDes		= pstOpTune->pucDes;
	pstContents->stOpTune.ucDesLen		= pstOpTune->ucDesLen;
	if( (usDesIdx + 1) == usTpNum) {
		pstContents->stOpTune.ucNextDesNum = 0xff;
		HxLOG_Print("1 pstContents->stOpTune.ucNextDesNum = 0x%x\n", pstContents->stOpTune.ucNextDesNum);
	} else {
		pstContents->stOpTune.ucNextDesNum = (HUINT8)((usDesIdx + 1) & 0xff);
		HxLOG_Print("2 pstContents->stOpTune.ucNextDesNum = 0x%x\n", pstContents->stOpTune.ucNextDesNum);
	}

	pTuningDesInfo = svc_cas_SubSvcSiParseDescriptor(svc_cas_MgrGetTextEncoding(svc_cas_SubSvcRmGetActionId(hAction)), pstContents->stOpTune.pucDes);
	switch (pstContents->stOpTune.pucDes[0])
	{
		case TAG_SATELLITE_DELIVERY_SYSTEM:
			pstContents->stOpTune.eDeliType = eDxDELIVERY_TYPE_SAT;
			if(svc_cas_SubSvcSiGetTuningInfoFromSatDSD(pTuningDesInfo, &pstContents->stOpTune.stTuningDesInfo) != ERR_OK)
				HxLOG_Error("ERROR!!\n");
			break;

		case TAG_TERRESTRIAL_DELIVERY_SYSTEM:
			pstContents->stOpTune.eDeliType = eDxDELIVERY_TYPE_TER;
			if(svc_cas_SubSvcSiGetTuningInfoFromTerDSD(pTuningDesInfo, &pstContents->stOpTune.stTuningDesInfo) != ERR_OK)
				HxLOG_Error("ERROR!!\n");
			break;

		case TAG_CABLE_DELIVERY_SYSTEM:
			pstContents->stOpTune.eDeliType = eDxDELIVERY_TYPE_CAB;
			if(svc_cas_SubSvcSiGetTuningInfoFromCabDSD(pTuningDesInfo, &pstContents->stOpTune.stTuningDesInfo) != ERR_OK)
				HxLOG_Error("ERROR!!\n");
			break;

		default:
			break;
	}
	svc_cas_SubSvcSiFreeDescriptor (pTuningDesInfo);

	hAction = local_CiMgr_GetActionBySlotId(usSlotId);
	if(svc_cas_MgrSubUpdateCasEtcEventData(hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM, eSEVT_CAS_CIPLUS_OPERATOR_TUNE, usSlotId, eDxCAS_GROUPID_CAM, (void*)&pstContents->stOpTune, sizeof(SvcCas_OperatorTune_t)) != ERR_OK)
	{
		HxLOG_Error("ERROR!! \n");
		return ERR_FAIL;
	}

	HxLOG_Print("-------------------------------------\n");

	return ERR_OK;
}


static HUINT32 local_CiMgr_ProcOperatorTune(CI_PLUS_OPERATOR_TUNE_PARAM *pstParam)
{
	Handle_t 				hAction = HANDLE_NULL;
	void					*pTuningDesInfo = NULL;
	CI_OperatorProfile_t	*pstContents;
	CI_OperatorTune_t		*pstOpTune;
	POINTER_LIST_T			*pItem = NULL;
	HUINT16					usTpNum, usDesIdx;
	HBOOL					bFound = FALSE;
	HERROR 					nResult = ERR_FAIL;

	pstContents = &s_stOperatorProfile[pstParam->usSlotId];

	local_CiMgr_ParseOperatorTune(pstParam, pstContents);

	usTpNum = UTIL_LINKEDLIST_GetNumListItemsInPointerList(pstContents->pTuneList, NULL, NULL);
	for(usDesIdx = 0 ; usDesIdx < usTpNum ; usDesIdx++)
	{
		pItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pstContents->pTuneList, usDesIdx, NULL, NULL);
		if(pItem == NULL)
		{
			HxLOG_Error("UTIL_LINKEDLIST_FindNthListItemFromPointerList() error\n");
			break;
		}

		pstOpTune = (CI_OperatorTune_t *)UTIL_LINKEDLIST_GetContents(pItem);

		// 가운데의 delivery descriptor가 no signal 인지 판단해야 한다. tp info 에 이 값이 존재하니?
		// 현시점에서 no signal 인걸 알 수 있는 방법이 없네. 차선책으로 TP에 없으면 없는걸로...
		if (usTpNum > 1)	// TP가 한개인 경우, 즉 마지막인 경우는 하지말자.
		{
			nResult = local_CiMgr_CheckNoSignalAtMiddle (pstContents, pstOpTune);
			if (nResult == ERR_FAIL)
			{
				HxLOG_Print("This TP is NO SIGNAL !!! -> continue...\n");
				continue;
			}
		}

		if(pstOpTune->bTuned == FALSE)
		{
			bFound = TRUE;
			pstOpTune->bTuned = TRUE;
			break;
		}
	}

	if(bFound == FALSE)
	{
		// list가 비어 있거나 마지막 list item 이다.
		return CI_OperatorTuneStatus(pstParam->usSlotId, 0, 0, 3, NULL, 0, 0xff);
	}

	pstContents->stOpTune.pucDes		= pstOpTune->pucDes;
	pstContents->stOpTune.ucDesLen		= pstOpTune->ucDesLen;
	if( (usDesIdx + 1) == usTpNum)
		pstContents->stOpTune.ucNextDesNum = 0xff;
	else
		pstContents->stOpTune.ucNextDesNum = (HUINT8)((usDesIdx + 1) & 0xff);

	hAction = local_CiMgr_GetActionBySlotId(pstParam->usSlotId);
	pTuningDesInfo = svc_cas_SubSvcSiParseDescriptor(svc_cas_MgrGetTextEncoding(svc_cas_SubSvcRmGetActionId(hAction)), pstContents->stOpTune.pucDes);
	switch (pstContents->stOpTune.pucDes[0])
	{
		case TAG_SATELLITE_DELIVERY_SYSTEM:
			pstContents->stOpTune.eDeliType = eDxDELIVERY_TYPE_SAT;
			if(svc_cas_SubSvcSiGetTuningInfoFromSatDSD(pTuningDesInfo, &pstContents->stOpTune.stTuningDesInfo) != ERR_OK)
				HxLOG_Error("ERROR!!\n");
			break;

		case TAG_TERRESTRIAL_DELIVERY_SYSTEM:
			pstContents->stOpTune.eDeliType = eDxDELIVERY_TYPE_TER;
			if(svc_cas_SubSvcSiGetTuningInfoFromTerDSD(pTuningDesInfo, &pstContents->stOpTune.stTuningDesInfo) != ERR_OK)
				HxLOG_Error("ERROR!!\n");
			break;

		case TAG_CABLE_DELIVERY_SYSTEM:
			pstContents->stOpTune.eDeliType = eDxDELIVERY_TYPE_CAB;
			if(svc_cas_SubSvcSiGetTuningInfoFromCabDSD(pTuningDesInfo, &pstContents->stOpTune.stTuningDesInfo) != ERR_OK)
				HxLOG_Error("ERROR!!\n");
			break;

		default:
			break;
	}
	svc_cas_SubSvcSiFreeDescriptor (pTuningDesInfo);

	if(svc_cas_MgrSubUpdateCasEtcEventData(hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM, eSEVT_CAS_CIPLUS_OPERATOR_TUNE, pstParam->usSlotId, eDxCAS_GROUPID_CAM, (void*)&pstContents->stOpTune, sizeof(SvcCas_OperatorTune_t)) != ERR_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HUINT32 local_CiMgr_ProcOperatorNit(CI_PLUS_OPERATOR_NIT_PARAM *pstParam)
{
	Handle_t				hAction = HANDLE_NULL;
	//POINTER_LIST_T			*pNitList = NULL;
	CI_OperatorProfile_t	*pstContents;

	hAction		= local_CiMgr_GetActionBySlotId(pstParam->usSlotId);
	pstContents	= &s_stOperatorProfile[pstParam->usSlotId];

	HxSTD_memcpy(pstContents->szProfName, pstParam->pszProfileName, 255);
	HxSTD_memcpy(pstContents->aucTextCoding, pstParam->aucTextCoding, 7);

	local_CiMgr_ParseCiCamNitToNitRaw(pstParam, pstContents);
	local_CiMgr_ParseNitRawForCam(pstContents);

#if defined(CONFIG_DEBUG)
	local_CiMgr_PrintOperatorNit(pstContents);
#endif

	if(svc_cas_MgrSubUpdateCasEtcEvent(hAction, (Handle_t)NULL, eDxCAS_GROUPID_CAM, eSEVT_CAS_CIPLUS_OPERATOR_NIT,
		pstParam->usSlotId, eDxCAS_GROUPID_CAM, pstParam->bSimpleNitChange) != ERR_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR CiMgr_GetOpNetNum(HUINT16 usSlotId, void *pvOut)
{
	CI_OperatorProfile_t	*pContents;
	HUINT16					*pusNetNum;

	if(usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("(%d)Slot Id \n", usSlotId);
		return ERR_FAIL;
	}

	pContents = &s_stOperatorProfile[usSlotId];

	pusNetNum = (HUINT16 *)pvOut;
	*pusNetNum = UTIL_LINKEDLIST_GetNumListItemsInPointerList(pContents->pNitList, NULL, NULL);

	return ERR_OK;
}

HERROR CiMgr_GetOpNetInfo(HUINT16 usSlotId, HUINT16 usNetIdx, void *pvOut)
{
	CI_OperatorProfile_t	*pContents;
	CI_OperatorNit_t		*pstOpNit ;
	SvcCas_OperatorNit_t		*pstNetInfo;
	POINTER_LIST_T			*pItem;

	if(usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("(%d)Slot Id \n", usSlotId);
		return ERR_FAIL;
	}

	pContents = &s_stOperatorProfile[usSlotId];

	pItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pContents->pNitList, usNetIdx, NULL, NULL);
	if(pItem == NULL)
	{
		HxLOG_Error("error\n");
		return ERR_FAIL;
	}

	pstOpNit = (CI_OperatorNit_t *)UTIL_LINKEDLIST_GetContents(pItem);
	pstNetInfo = (SvcCas_OperatorNit_t *)pvOut;

	pstNetInfo->usNetId = pstOpNit->usNetId;
	HxSTD_memcpy(pstNetInfo->szNetName, pstOpNit->szNetName, 256);

	return ERR_OK;
}

HERROR CiMgr_GetOpTsNum(HUINT16 usSlotId, HUINT16 usNetIdx, void *pvOut)
{
	CI_OperatorProfile_t	*pContents;
	CI_OperatorNit_t		*pstOpNit;
	POINTER_LIST_T			*pItem;
	HUINT16					*pusTsNum;

	if(usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("(%d)Slot Id \n", usSlotId);
		return ERR_FAIL;
	}

	pContents = &s_stOperatorProfile[usSlotId];

	pItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pContents->pNitList, usNetIdx, NULL, NULL);
	if(pItem == NULL)
	{
		HxLOG_Error("error\n");
		return ERR_FAIL;
	}

	pstOpNit = (CI_OperatorNit_t *)UTIL_LINKEDLIST_GetContents(pItem);
	pusTsNum = (HUINT16 *)pvOut;

	*pusTsNum = UTIL_LINKEDLIST_GetNumListItemsInPointerList(pstOpNit->pTsLoopList, NULL, NULL);

	return ERR_OK;
}

HERROR CiMgr_GetOpTsInfo(HUINT16 usSlotId, HUINT16 usNetIdx, HUINT16 usTsIdx, void *pvOut)
{
	CI_OperatorProfile_t	*pContents;
	CI_OperatorNit_t		*pstOpNit;
	CI_OperatorNitTsLoop_t	*pstTsLoop;
	SvcCas_OperatorNitTs_t		*pstTsInfo;
	POINTER_LIST_T			*pItem;

	if(usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("(%d)Slot Id \n", usSlotId);
		return ERR_FAIL;
	}

	pContents = &s_stOperatorProfile[usSlotId];

	pItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pContents->pNitList, usNetIdx, NULL, NULL);
	if(pItem == NULL)
	{
		HxLOG_Error("error\n");
		return ERR_FAIL;
	}

	pstOpNit = (CI_OperatorNit_t *)UTIL_LINKEDLIST_GetContents(pItem);

	pItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pstOpNit->pTsLoopList, usTsIdx, NULL, NULL);
	if(pItem == NULL)
	{
		HxLOG_Error("error\n");
		return ERR_FAIL;
	}

	pstTsLoop = (CI_OperatorNitTsLoop_t *)UTIL_LINKEDLIST_GetContents(pItem);
	pstTsInfo = (SvcCas_OperatorNitTs_t *)pvOut;

	pstTsInfo->usTsId		= pstTsLoop->usTsId;
	pstTsInfo->usOnId		= pstTsLoop->usOnId;
	pstTsInfo->eDeliType	= pstTsLoop->eDelivery;
	pstTsInfo->stTuningDes	= pstTsLoop->stTuningDes;

	return ERR_OK;
}

HERROR CiMgr_GetOpSvcNum(HUINT16 usSlotId, HUINT16 usNetIdx, HUINT16 usTsIdx, void *pvOut)
{
	CI_OperatorProfile_t	*pContents;
	CI_OperatorNit_t		*pstOpNit;
	CI_OperatorNitTsLoop_t	*pstTsLoop;
	POINTER_LIST_T			*pItem;
	HUINT16					*pusSvcNum;

	if(usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("(%d)Slot Id \n", usSlotId);
		return ERR_FAIL;
	}

	pContents = &s_stOperatorProfile[usSlotId];

	pItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pContents->pNitList, usNetIdx, NULL, NULL);
	if(pItem == NULL)
	{
		HxLOG_Error("error\n");
		return ERR_FAIL;
	}

	pstOpNit = (CI_OperatorNit_t *)UTIL_LINKEDLIST_GetContents(pItem);

	pItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pstOpNit->pTsLoopList, usTsIdx, NULL, NULL);
	if(pItem == NULL)
	{
		HxLOG_Error("error\n");
		return ERR_FAIL;
	}

	pstTsLoop = (CI_OperatorNitTsLoop_t *)UTIL_LINKEDLIST_GetContents(pItem);
	pusSvcNum = (HUINT16 *)pvOut;

	*pusSvcNum = UTIL_LINKEDLIST_GetNumListItemsInPointerList(pstTsLoop->pSvcList, NULL, NULL);

	return ERR_OK;
}

HERROR CiMgr_GetOpSvcInfo(HUINT16 usSlotId, HUINT16 usNetIdx, HUINT16 usTsIdx, void *pvOut)
{
	CI_OperatorProfile_t	*pContents;
	SvcCas_OperatorSvc_t		*pstSvcArray, *pstOpSvc;
	CI_OperatorNit_t		*pstOpNit;
	CI_OperatorNitTsLoop_t	*pstTsLoop;
	HUINT32					ulSvcNum, i;
	POINTER_LIST_T			*pItem;

	if(usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("(%d)Slot Id \n", usSlotId);
		return ERR_FAIL;
	}

	pContents = &s_stOperatorProfile[usSlotId];

	pItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pContents->pNitList, usNetIdx, NULL, NULL);
	if(pItem == NULL)
	{
		HxLOG_Error("error\n");
		return ERR_FAIL;
	}

	pstOpNit = (CI_OperatorNit_t *)UTIL_LINKEDLIST_GetContents(pItem);

	pItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pstOpNit->pTsLoopList, usTsIdx, NULL, NULL);
	if(pItem == NULL)
	{
		HxLOG_Error("error\n");
		return ERR_FAIL;
	}

	pstTsLoop = (CI_OperatorNitTsLoop_t *)UTIL_LINKEDLIST_GetContents(pItem);
	pstSvcArray = (SvcCas_OperatorSvc_t *)pvOut;

	ulSvcNum = UTIL_LINKEDLIST_GetNumListItemsInPointerList(pstTsLoop->pSvcList, NULL, NULL);
	for(i = 0  ; i < ulSvcNum ; i++)
	{
		pItem = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pstTsLoop->pSvcList, i, NULL, NULL);
		if(pItem == NULL)
		{
			HxLOG_Error("error\n");
			return ERR_FAIL;
		}

		pstOpSvc = (SvcCas_OperatorSvc_t *)UTIL_LINKEDLIST_GetContents(pItem);

		HxSTD_memcpy(&pstSvcArray[i], pstOpSvc, sizeof(SvcCas_OperatorSvc_t));
	}

	return ERR_OK;
}

HERROR CiMgr_GetOpName(HUINT16 usSlotId, HUINT8 *pucOpName)
{
	CI_OperatorProfile_t	*pContents;

	if(usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("(%d)Slot Id \n", usSlotId);
		return ERR_FAIL;
	}

	pContents = &s_stOperatorProfile[usSlotId];

	HxSTD_memcpy(pucOpName, pContents->szProfName, 256);

	return ERR_OK;
}

HERROR CiMgr_GetNextOperatorTune(CI_SLOT_ID usSlotId, HUINT8 ucNextDesNum)
{
	return local_CiMgr_GetNextOperatorTune(usSlotId, ucNextDesNum);
}

#endif


static void local_CiMgr_Task(void)
{
	void				*pvEvtParam;
	HUINT8			aucMsg[MW_CAS_CIMGR_MSG_SIZE];
	HERROR			hErr;
	CI_RCV_EVENT	eEvt;

	hErr = local_CiMgr_InitModule();
	if(hErr != ERR_OK)
	{
		HxLOG_Error("local_CiMgr_InitModule() failed\n");
		return;
	}

	for(;;)
	{
		if (VK_MSG_Receive(s_ulCiMgrMsgQId, aucMsg, MW_CAS_CIMGR_MSG_SIZE) != VK_OK)
		{
			HxLOG_Error("\n");
			continue;
		}

		HxSTD_memcpy(&eEvt, aucMsg, sizeof(CI_RCV_EVENT));
		pvEvtParam = &aucMsg[sizeof(CI_RCV_EVENT)];

		#if defined(CONFIG_DEBUG)
		HxLOG_Info("Event [%s]\n", local_CiMgr_Cmd2Str(eEvt));
		#endif

		switch (eEvt)
		{

/*--- events for service ---*/

		case CI_START:
			local_CiMgr_ProcStart((CI_ADD_SVC_PARAM *)pvEvtParam);
			break;

		case CI_STOP:
			local_CiMgr_ProcStop((CI_DELETE_SVC_PARAM *)pvEvtParam);
			break;

#if defined(CONFIG_SUPPORT_RETRY_CAPMT)
		case CI_RETRY:
			local_CiMgr_ProcRetrySvc((CI_RETRY_SVC_PARAM *)pvEvtParam);
			break;
#endif

#if defined(CONFIG_MW_CI_CHECK_AV)
		case CI_CHECK_AV:
			local_CiMgr_ProcCheckAv((CI_CHECK_AV_PARAM *)pvEvtParam);
			break;
#endif

/*--- events for SI ---*/

		case CI_PMT_CHANGED:
			local_CiMgr_ProcPmtChanged((CI_PMT_CHANGED_PARAM *)pvEvtParam);
			break;

		case CI_PMT_CHANGED_LOCATION:
			HxLOG_Info(">> CI_PMT_CHANGED_LOCATION:\n");
			 local_CiMgr_ProcPmtChangedLocation((CI_PMT_CHANGED_LOCATION_PARAM *)pvEvtParam);
			break;

#if defined(CONFIG_MW_CI_PLUS)
		case CI_SDT_CHANGED:
			local_CiMgr_ProcSdtChanged((CI_SDT_CHANGED_PARAM *)pvEvtParam);
			break;
#endif

/*--- events for instance management ---*/

		case CI_MANAGE_CA_INST:
			local_CiMgr_ProcManageInstance((CI_MANAGE_CA_INST_PARAM *)pvEvtParam);
			break;

		case CI_UPDATE_CA_INST:
			local_CiMgr_ProcUpdateCaInst((CI_UPDATE_CA_INST_PARAM *)pvEvtParam);
			break;

		case CI_ADD_DELAYED_CA_INST:
			local_CiMgr_ProcAddDelayedCaInst((CI_UPDATE_CA_INST_PARAM *)pvEvtParam);
			break;

#if defined (CONFIG_MW_CI_PLUS)
		case CI_UPDATE_CC:
			local_CiMgr_ProcUpdateCcResource((CI_UPDATE_CA_INST_PARAM *)pvEvtParam);
			break;
#endif

#if defined(CI_PLUS_PVR_ENABLE)
		case CI_UPDATE_CA_PVR_INST:
			local_CiMgr_ProcUpdateCaPvrInst((CI_UPDATE_CA_PVR_INST_PARAM *)pvEvtParam);
			break;
#endif

/*--- events for CAM state ---*/

		case CI_UPDATE_CAM_STATE:
			local_CiMgr_ProcUpdateCamState((CI_UPDATE_CAM_STATE_PARAM *)pvEvtParam);
			break;

/*--- events for CAS state ---*/

		case CI_UPDATE_CAS_STATE:
			local_CiMgr_ProcUpdateCasState((CI_UPDATE_CAS_STATE_PARAM *)pvEvtParam);
			break;

/*--- events for CI+ ---*/

#if defined(CONFIG_MW_CI_PLUS)
		case CI_PLUS_KEY_CHANGED:
			local_CiMgr_ProcKeyChanged((CI_PLUS_KEY_CHANGE_PARAM *)pvEvtParam);
			break;

		case CI_PLUS_URI_CONTROL:
			local_CiMgr_ProcUriControl((CI_PLUS_URI_CONTROL_PARAM *)pvEvtParam);
			break;

		case CI_PLUS_SRM_CONTROL:
			local_CiMgr_ProcSrmControl((CI_PLUS_SRM_CONTROL_PARAM *)pvEvtParam);
			break;

		case CI_PLUS_CAM_TUNE:
			local_CiMgr_ProcCamTune((CI_PLUS_CAM_TUNE_PARAM *)pvEvtParam);
			break;

		case CI_PLUS_CAM_PIN_CAPA_INFO:
			local_CiMgr_ProcPinCapaInfo(*((CI_SLOT_ID *)pvEvtParam));
			break;

	#if defined(CONFIG_OP_CANALREADY)
		case CI_PLUS_CANALREADY_INIT_NOTIFY:
			//local_cimgr_ProcCanalReadyInitNotify((CI_PLUS_CANALREADY_INIT_INFO_PARAM*)pvEvtParam);
			break;
	#endif	// CONFIG_OP_CANALREADY

	#if defined(CONFIG_MW_CI_PLUS_V_1_3)
		case CI_PLUS_OPERATOR_PREPARE_REQ:
			local_CiMgr_ProcOperatorPrepareReq((CI_PLUS_OPERATOR_PREPARE_PARAM *)pvEvtParam);
			break;

		case CI_PLUS_OPERATOR_TUNE:
			local_CiMgr_ProcOperatorTune((CI_PLUS_OPERATOR_TUNE_PARAM *)pvEvtParam);
			break;

		case CI_PLUS_OPERATOR_NIT:
			local_CiMgr_ProcOperatorNit((CI_PLUS_OPERATOR_NIT_PARAM *)pvEvtParam);
			break;
	#endif
#endif	// CONFIG_MW_CI_PLUS


		default:
			break;
		}
	}
}

/********************************************************************
* Module Internal Functions
* Naming rule : "CiMgr _ "Function Name"
********************************************************************/
#define ___07_Module_Internal_APIs___

HERROR CiMgr_GetMmiData(HUINT16 usSlotId, HUINT32 ulVersion, SvcCas_MmiData_t **stMmiData)
{
	HUINT32			ulFilter = 0;
	CI_MmiData_t		*pDataSet = NULL;
	POINTER_LIST_T	*pList = NULL;

	HxLOG_Info(" slot id (%d), version (%d) \n", usSlotId, ulVersion);

	if(usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("(%d)Slot Id \n", usSlotId);
		return ERR_FAIL;
	}

	if(stMmiData == NULL)
	{
		HxLOG_Error("input null \n");
		return ERR_FAIL;
	}

	ENTER_CRITICAL_MMI;

	pDataSet = local_CiMgr_GetListBySlotId(usSlotId);
	if(pDataSet == NULL)
	{
		HxLOG_Error("failure in getting data set \n");
		goto Ret_Err;
	}

	ulFilter = ulVersion;
	pList = UTIL_LINKEDLIST_IsListItemInPointerList(pDataSet->s_pstList, NULL, NULL, local_CiMgr_ListFilter, (void *)&ulFilter);
	if(pList == NULL)
	{
		HxLOG_Critical("\n");
		goto Ret_Err;
	}

	*stMmiData = pList->pvContents;

	ulFilter = --ulVersion;
	while(ulFilter)
	{
		pList = UTIL_LINKEDLIST_IsListItemInPointerList(pDataSet->s_pstList, NULL, NULL, local_CiMgr_ListFilter, (void *)&ulFilter);
		if(pList == NULL)
		{
			LEAVE_CRITICAL_MMI;
			return ERR_OK;
		}

		pDataSet->s_pstList = UTIL_LINKEDLIST_DeleteListItemFromPointerList(pDataSet->s_pstList, pList, local_CiMgr_DelMmiDataList);
		ulFilter = --ulVersion;
	}

	LEAVE_CRITICAL_MMI;
	return ERR_OK;

Ret_Err:

	LEAVE_CRITICAL_MMI;
	return ERR_FAIL;
}

HERROR CiMgr_GetCcReport(HUINT16 usSlotId, HUINT32 ulEvtVersion, SvcCas_CcReport_t **pData)
{
	SvcCas_CcReport_t *pstCcReportData = NULL;

	if(usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("(%d)Slot Id \n", usSlotId);
		return ERR_FAIL;
	}

	if(pData == NULL)
	{
		return ERR_FAIL;
	}

	pstCcReportData = &s_stCcReportStatus[usSlotId];

	if(pstCcReportData->ulVer != ulEvtVersion)
	{
		return ERR_FAIL;
	}

	*pData = pstCcReportData;

	return ERR_OK;
}

// CiMgr_GetSlotIdByActionId()
// 현재 Action ID에 대한 서비스를 위하여 사용되고 있는 CAM slot ID를 알려주는 function.
// return ERR_FAIL 일 경우 해당 Action ID에 대한 서비스가 CAM을 사용하지 않음을 나타냄.
HERROR CiMgr_GetSlotIdByActionId(HUINT32 ulActionId, HUINT16 *pusSlotId)
{
	CI_Context_t	*pstContext;

	pstContext = local_CiMgr_GetContextByActionId(ulActionId);

	if(pstContext->pCam == NULL)
	{
		// 해당 Action ID가 점유하는 CAM이 없음을 나타냄.
		HxLOG_Error("pCAM is NULL...\n");
		return ERR_FAIL;
	}
	else
	{
		if(pstContext->pCam->usSlotId == CARM_INVALID_CI_SLOT_ID)
		{
			return ERR_FAIL;
		}
		else
		{
			*pusSlotId = pstContext->pCam->usSlotId;
		}
	}
	return ERR_OK;
}

#if defined(CONFIG_MW_CI_PLUS)
HERROR CiMgr_ReleaseDrmInfo(HUINT8 ucVersion, HERROR hConsumedStatus)
{
	// 필요하다면 hConsumedStatus를 보고 CAM 쪽 혹은 CC resource 에 return 값을 넘겨 줄수 있음!
	if (hConsumedStatus != ERR_OK)
	{
		HxLOG_Critical("\nWhile DRM Infomation is writing, error is occured!!\n");
	}

	if (s_astCiDrmInfo[ucVersion].stCasCcInfo.info.stCiPlusDrmInfo.eCiPlusDrmType == eDxCopyrightControl_CiPlusDrmType_License)
	{
		HxSTD_memset(s_astCiDrmInfo[ucVersion].stCasCcInfo.info.stCiPlusDrmInfo.ciplusDrmInfo.stCiPlusLicenseInfo.aucLicenseData, 0, 1024);
	}

	HxLOG_Info("The state of CI+ DRM data before release:[%02x].\n", s_eCiDrmWriteState[ucVersion]);
	HxLOG_Info("CI+ DRM data ver[%02x] is released.\n", ucVersion);
	s_eCiDrmWriteState[ucVersion] = eDxCopyrightControl_CiPlusDrmWriteInfo_Done;

	return ERR_OK;
}

HERROR CiMgr_GetUpgradeStatus(HUINT16 usSlotId, SvcCas_CiPlusUpgrade_t **ppstUpdateStatus)
{
	if(usSlotId >= svc_cas_DevCiGetNumberOfModule() || ppstUpdateStatus == NULL)
	{
		return ERR_FAIL;
	}

	*ppstUpdateStatus = &s_stCiPlusUpgradeStatus[usSlotId];

	return ERR_OK;
}

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
HERROR CiMgr_NotifyHciPmtUse(Handle_t hAction, HUINT32 unSlotId)
{
	HxLOG_Info("%s \n", __FUNCTION__);

	if(unSlotId >= svc_cas_DevCiGetNumberOfModule() || hAction == HANDLE_NULL)
	{
		HxLOG_Error("Slot Id(%d), hAction(0x%x) \n", unSlotId, HANDLE_NULL);
		return ERR_FAIL;
	}

	return local_CiMgr_UpdatePmtLocation(hAction, unSlotId);
}
#endif // #if defined(CONFIG_MW_CI_PLUS_V_1_3)
#endif // #if defined(CONFIG_MW_CI_PLUS)

/********************************************************************
* Global Public Functions
* Naming rule : "XSVC"_"CiMgr" _ "Function Name"
********************************************************************/
#define ___08_Global_Public_APIs___

HERROR xsvc_cas_CiMgrInit(void)
{
	if (VK_TASK_Create((void*)local_CiMgr_Task, MW_CAS_CIMGR_TASK_PRIORITY, MW_CAS_CIMGR_TASK_STACK_SIZE, "tCIMGR", NULL, &s_ulCiMgrTaskId, 0) != VK_OK)
	{
		VK_MSG_Destroy(s_ulCiMgrMsgQId);
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	if (VK_MSG_Create(MW_CAS_CIMGR_MSGQ_SIZE, MW_CAS_CIMGR_MSG_SIZE, "qCIMGR", &s_ulCiMgrMsgQId, VK_SUSPENDTYPE_PRIORITY) != VK_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	if (VK_TASK_Start(s_ulCiMgrTaskId) != VK_OK)
	{
		VK_TASK_Destroy(s_ulCiMgrTaskId);
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	/*	cam이 삽입 된 상태에서 부팅하는 경우에 대해서 처리하는 방법을 수정 필요.
		driver에서 callback이 등록되기 전에 interrupt가 발생한 경우 처리가 simple하기에
		mw에서 부팅 시 driver의 state를 확인후 처리를 하도록 한다.
	*/
	CiAdapt_Init(); // 이 함수가 호출될 때까지 DRV_CI_Task() 함수는 계속 waiting 함.

	return ERR_OK;
}

HERROR xsvc_cas_SendCaPmt(HUINT16 usSlotId, HUINT8 *pucCaPmt, HUINT16 usCaPmtLen)
{
	HERROR		hErr;

	hErr = CI_SendCaPmt(usSlotId, pucCaPmt, usCaPmtLen);
	if(hErr != ERR_OK)
	{
		HxLOG_Error("CI_SendCaPmt() error.\n");
		return ERR_FAIL;
	}

	HxLOG_Print("Send ca pmt...(slot : %d)\n", usSlotId);

	return ERR_OK;
}

HERROR xsvc_cas_CiMgrAddMmiDataList(HUINT16 usSlotId, SvcCas_MmiData_t *pstMmiData)
{
	return local_CiMgr_AddMmiDataList(usSlotId, pstMmiData);
}

/********************************************************************
* For Debugging
********************************************************************/
#define ___09_Debugging_APIs___

#if defined(CONFIG_DEBUG)
static const HINT8 * local_CiMgr_Cmd2Str(CI_RCV_EVENT cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(CI_START);

		ENUM_TO_STR(CI_SETPID);
		ENUM_TO_STR(CI_STOP);

		ENUM_TO_STR(CI_PAUSE);
		ENUM_TO_STR(CI_RESUME);

		ENUM_TO_STR(CI_RETRY);
		ENUM_TO_STR(CI_CHECK_AV);

		ENUM_TO_STR(CI_PMT_CHANGED);
		ENUM_TO_STR(CI_SDT_CHANGED);
		ENUM_TO_STR(CI_PMT_CHANGED_LOCATION);

		ENUM_TO_STR(CI_MANAGE_CA_INST);

		ENUM_TO_STR(CI_UPDATE_CA_INST);
		ENUM_TO_STR(CI_UPDATE_CC);
		ENUM_TO_STR(CI_ADD_DELAYED_CA_INST);

#if defined(CI_PLUS_PVR_ENABLE)
		ENUM_TO_STR(CI_UPDATE_CA_PVR_INST);
#endif

		ENUM_TO_STR(CI_UPDATE_CAM_STATE);

#if 1 // #if defined(CONFIG_MW_CI_PLUS_PVR) : 당장은 CA PVR 지원을 위해 추가한 것이지만 Multiple CA application selection procedure 구현 같은 다른 용도로 확장해서 쓸 여지가 있으므로 일반적으로 정의해 둠.
		ENUM_TO_STR(CI_UPDATE_CAS_STATE);
#endif

#if defined(CONFIG_MW_CI_PLUS)
		ENUM_TO_STR(CI_PLUS_KEY_CHANGED);
		ENUM_TO_STR(CI_PLUS_URI_CONTROL);
		ENUM_TO_STR(CI_PLUS_SRM_CONTROL);
		ENUM_TO_STR(CI_PLUS_CAM_TUNE);
		ENUM_TO_STR(CI_PLUS_CAM_PIN_CAPA_INFO);
		ENUM_TO_STR(CI_PLUS_OPERATOR_PREPARE_REQ);
		ENUM_TO_STR(CI_PLUS_OPERATOR_TUNE);
		ENUM_TO_STR(CI_PLUS_OPERATOR_NIT);
#endif

#if defined(CONFIG_OP_CANALREADY)
		ENUM_TO_STR(CI_PLUS_CANALREADY_INIT_NOTIFY);
#endif

		ENUM_TO_STR(CI_STREAM_STATE_CHANGED);

		ENUM_TO_STR(EVT_CIREV_MAX);
	}

	return ("local_cimgr_Cmd2Str : Unknown state");
}

static const HINT8 *local_CiMgr_MakeStrActionType(SvcCas_ActionType_e cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(eCAS_ActionType_None);
		ENUM_TO_STR(eCAS_ActionType_Live);
		ENUM_TO_STR(eCAS_ActionType_Playback);
		ENUM_TO_STR(eCAS_ActionType_Recording);
		ENUM_TO_STR(eCAS_ActionType_Descramble_a_File);
		ENUM_TO_STR(eCAS_ActionType_Tsr_Play);
		ENUM_TO_STR(eCAS_ActionType_Tsr_Rec);
		ENUM_TO_STR(eCAS_ActionType_Data);
		ENUM_TO_STR(eCAS_ActionType_EngineeringTS);
		ENUM_TO_STR(eCAS_ActionType_RfVod);
		ENUM_TO_STR(eCAS_ActionType_Max);
	}
	return ("Unknown");
}

static const HINT8 *local_CiMgr_MakeStrCiMgrState(CiMgrState_t eState)
{
	switch (eState)
	{
		ENUM_TO_STR(eCiMgrState_Stopped);
		ENUM_TO_STR(eCiMgrState_Started);
		ENUM_TO_STR(eCiMgrState_StartCaPmt);
		ENUM_TO_STR(eCiMgrState_Max);
	}
	return ("Unknown");
}

#if defined(DEBUG_ACT_LIST)
static void local_CiMgr_PrintActionList(CI_CamInst_t *pCamSelected)
{
	HUINT32		i;

	HxLOG_Print("--------------- ca pmt state (%x) -------------- \n", pCamSelected->caPmtOkDsc.pucRaw);

	HxLOG_Print("pCamSelected->ulNumOfAct (%d)\n", pCamSelected->ulNumOfAct);

	HxLOG_Print("hAction List\n");
	for(i=0; i<CAS_MAX_NUM_OF_ACTION; i++)
	{
		HxLOG_Print("(0x%08x) ", pCamSelected->hActList[i]);
	}
	HxLOG_Print("\n");
}
#endif

static const HINT8 *local_CiMgr_MakeStrSiTblState(SvcCas_SiTableState_e eState)
{
	switch (eState)
	{
		ENUM_TO_STR(eCASSI_Table_Skipped);
		ENUM_TO_STR(eCASSI_Table_Waiting);
		ENUM_TO_STR(eCASSI_Table_Received);
		ENUM_TO_STR(eCASSI_Table_Timeout);
		ENUM_TO_STR(eCASSI_Table_Running);
		ENUM_TO_STR(eCASSI_Table_Fail);
	}
	return ("Unknown");
}

static const HINT8 *local_CiMgr_MakeStrEsType(EsType_t eType)
{
	switch (eType)
	{
		ENUM_TO_STR(eEsType_None);
		ENUM_TO_STR(eEsType_Video);
		ENUM_TO_STR(eEsType_Audio);
		ENUM_TO_STR(eEsType_PCR);
		ENUM_TO_STR(eEsType_Subtitle);
		ENUM_TO_STR(eEsType_Teletext);
		ENUM_TO_STR(eEsType_ClosedCaption);
		ENUM_TO_STR(eEsType_SuperImpose);
		ENUM_TO_STR(eEsType_Data);
		ENUM_TO_STR(eEsType_Unknown);
		ENUM_TO_STR(eEsType_Reserved);
		ENUM_TO_STR(eEsType_All);
		ENUM_TO_STR(eEstype_Max);
	}

	return ("Unknown");
}

static HERROR local_CiMgr_PrintPmtElementInfo(SvcCas_PmtInfo_t *pstPmtInfo)
{
	int i, j;
	HUINT32 ulEsCnt = 0;
	SvcCas_PmtElement_t *pElem = NULL;

	if(pstPmtInfo == NULL)
	{
		return ERR_FAIL;
	}

	ulEsCnt = pstPmtInfo->ulNumOfElement;

	HxLOG_Print("\nwe have (%d) pmt element information \n", ulEsCnt);

	for(i=0; i<ulEsCnt; i++)
	{
		pElem = &pstPmtInfo->stElementInfo[i];
		HxLOG_Print("\n(%d) - element ---\n\n", i+1);
		HxLOG_Print("StreamType			(0x%x) \n", pElem->ucStreamType);
		HxLOG_Print("EsType				(%s) \n", local_CiMgr_MakeStrEsType(pElem->eEsType));
		HxLOG_Print("Pid				(0x%x) \n", pElem->usPid);
		HxLOG_Print("IsCurrent			(%d) \n", pElem->bIsCurrent);
		HxLOG_Print("Flag of element ca flag		(%d) \n", pElem->bElementCaFlag);
		HxLOG_Print("number of element ca descriptor	(%d) \n", pElem->ulNumOfCaDescriptor);

		for(j=0; j<pElem->ulNumOfCaDescriptor; j++)
		{
			HxLOG_Print("    [%d] -ca sys id		(0x%x)\n", j, pElem->stElementCaDescriptor[j].usCaSysId);
			HxLOG_Print("    [%d]- ca pid		(0x%x)\n", j, pElem->stElementCaDescriptor[j].usCaPid);
		}
	}

	return ERR_OK;
}

static HERROR local_CiMgr_PrintPmtInfo(SvcCas_PmtInfo_t *pstPmtInfo)
{
	int i = 0;

	if(pstPmtInfo == NULL)
	{
		HxLOG_Error(" \n");
		return ERR_FAIL;
	}

	HxLOG_Print("\n=== pmt info ===\n\n");

	HxLOG_Print("section state			(%d) \n", pstPmtInfo->ulSectionState);
	HxLOG_Print("service id			(0x%x) \n", pstPmtInfo->usServiceId);
	HxLOG_Print("pcr pid				(0x%x) \n", pstPmtInfo->usPcrPid);
	HxLOG_Print("version number			(%d) \n", pstPmtInfo->ucVerNum);
	HxLOG_Print("current next indicator		(%d) \n", pstPmtInfo->bCurNextInd);

	HxLOG_Print("crc value			(0x%x) \n", pstPmtInfo->ulCrc);
	HxLOG_Print("flag of global ca descriptor	(%d) \n", pstPmtInfo->bProgramCaFlag);
	HxLOG_Print("number of global ca descriptor	(%d) \n", pstPmtInfo->ulNumOfProgramCaDescriptor);

	for(i=0; i<pstPmtInfo->ulNumOfProgramCaDescriptor; i++)
	{
		HxLOG_Print("global ca descriptor[%d], ca sys id (0x%x), ca pid (0x%x) \n", i, pstPmtInfo->stProgramCaDescriptor[i].usCaSysId, pstPmtInfo->stProgramCaDescriptor[i].usCaPid);
	}

	local_CiMgr_PrintPmtElementInfo(pstPmtInfo);

	HxLOG_Print("\n=== end of pmt info ===\n");

	return ERR_OK;
}

void CiMgr_PrintCAMInfo(void)
{
	CI_CamInst_t *pCam;
	HINT32 i, j;

	pCam = s_astCiCam;

	HxLOG_Print("\n");
	HxLOG_Print("==================== CI CAM INST ====================\n");

	for (i = 0; i < svc_cas_DevCiGetNumberOfModule(); i++)
	{
		HxLOG_Print("--------------------------------------\n");
		HxLOG_Print("pCam	: 0x%x\n", (HUINT32)&pCam[i]);

		HxLOG_Print("usSlotId : %d\n", pCam[i].usSlotId);
		HxLOG_Print("usManufacturer : 0x%04X\n", pCam[i].usManufacturer);
		HxLOG_Print("CaSysId (hex) :");
			for (j = 0; j < pCam[i].usCaInfoCnt; j++)
				HxLOG_Print(" 0x%04X", pCam[i].ausCaSysId[j]);
				HxLOG_Print("\n");

		HxLOG_Print("PidInfoForCaPmt (hex)	: %d", pCam[i].ulNumOfPidForCaPmt);
			for (j = 0; j < pCam[i].ulNumOfPidForCaPmt; j++)
				HxLOG_Print(" 0x%04X(%d)", pCam[i].PidInfoForCaPmt[j].usPid, pCam[i].PidInfoForCaPmt[j].esType);
				HxLOG_Print("\n");

		#if defined(CI_PLUS_PVR_ENABLE)
		HxLOG_Print("CaPvrCaSysId :");
			for (j = 0; j < pCam[i].usCaPvrInfoCnt; j++)
				HxLOG_Print(" %04X(%d)", pCam[i].ausCaPvrCaSysId[j], pCam[i].aucCaRecMode[j]);
				HxLOG_Print("\n");
		#endif

		HxLOG_Print("hCasDevice : 0x%x\n", pCam[i].hCasDevice);
		HxLOG_Print("caPmtOkDsc : ");
			for (j = 0; j < pCam[i].caPmtOkDsc.usRawLen; j++)
				HxLOG_Print(" %02X", pCam[i].caPmtOkDsc.pucRaw[j]);
				HxLOG_Print("\n");

		#if defined(CONFIG_MW_CI_PLUS)
		{
			HUINT32 k=0;
			for (k=0; k < CI_MAX_NUM_OF_DMX; k++)
			{
				if(pCam[i].ulNumOfDsc[k] != 0)
				{
					HxLOG_Print("Demux[%d] Opened Desc # : %d", k, pCam[i].ulNumOfDsc[k]);
					for (j = 0; j < pCam[i].ulNumOfDsc[k]; j++)
						HxLOG_Print(" [dsc id-%d: pid-0x%x]", pCam[i].stDscInfo[k][j].ulDscId, pCam[i].stDscInfo[k][j].usPid);
						HxLOG_Print("\n");
				}
			}
			HxLOG_Print("Is key alive?: %s \n", pCam[i].bKeyInfoAvailable ? "yes" : "no");
		}
		#endif

		HxLOG_Print("ulNumOfAct-(%d) :", pCam[i].ulNumOfAct);
		for (j = 0; j < CAS_MAX_NUM_OF_ACTION; j++)
		{
			if (pCam[i].hActList[j] != HANDLE_NULL)
				HxLOG_Print(" 0x%08X", pCam[i].hActList[j]);
		}

		HxLOG_Print("\n");
	}
	HxLOG_Print("==========================================================\n");
}

void CiMgr_PrintContextInfo(void)
{
	CI_Context_t *pCiContext = NULL;
	HUINT32 i, j;

	pCiContext = s_astCiContext;

	HxLOG_Print("\n");
	HxLOG_Print("==================== CI CONTEXT ====================\n");

	for (i=0; i<CI_MAX_NUM_OF_ACT; i++)
	{
		HxLOG_Print("--------------------------------------\n");
		HxLOG_Print("[%d] pCiContext : 0x%x\n", i, &pCiContext[i]);

		if(pCiContext[i].hAction == HANDLE_NULL)
		{
			continue;
		}

		HxLOG_Print("hAction : 0x%x\n", pCiContext[i].hAction);
		HxLOG_Print("eActType : %s\n", local_CiMgr_MakeStrActionType(pCiContext[i].eActType));
		HxLOG_Print("eState : %s\n", local_CiMgr_MakeStrCiMgrState(pCiContext[i].eState));
		HxLOG_Print("hSvc : 0x%x\n", pCiContext[i].hSvc);
		HxLOG_Print("pCam : 0x%x, usSlotId : %d\n", pCiContext[i].pCam, pCiContext[i].pCam != NULL ? pCiContext[i].pCam->usSlotId : -1);
		HxLOG_Print("ulTunerId : %d\n", pCiContext[i].ulTunerId);
		HxLOG_Print("ulDemuxId : %d\n", pCiContext[i].ulDemuxId);
		HxLOG_Print("bPathConnected : %d\n", pCiContext[i].bPathConnected);
		HxLOG_Print("bSendCaPmt : %d\n", pCiContext[i].bSendCaPmt);

		if (pCiContext[i].stProgInfo.pPmtInfo == NULL)
		{
			HxLOG_Print("program-level  : pPmtInfo is null \n");
		}
		else
		{
			local_CiMgr_PrintPmtInfo(pCiContext[i].stProgInfo.pPmtInfo);
		}

		HxLOG_Print("ucNumEs : %d\n", pCiContext[i].stProgInfo.ucNumEs);
		for (j = 0; j < pCiContext[i].stProgInfo.ucNumEs; j++)
		{
			HxLOG_Print("element-level : EsType(%d) EsPid(0x%x) \n",
						pCiContext[i].stProgInfo.aEsInfo[j].eEsType,
						pCiContext[i].stProgInfo.aEsInfo[j].usEsPid);
		}

		#if defined(CONFIG_MW_CI_PLUS)
		HxLOG_Print("eSdtTblState : %s\n", local_CiMgr_MakeStrSiTblState(pCiContext[i].eSdtTblState));
		HxLOG_Print("bSvcShunned : %d\n", pCiContext[i].bSvcShunned);
		#endif
	}
	HxLOG_Print("==========================================================\n");

}

static void local_CiMgr_PrintOperatorNit(CI_OperatorProfile_t *pstContents)
{
	POINTER_LIST_T	*pList;
	HUINT16			usListNum, usTsLoopNum, usSvcNum;
	HINT32			i, j, k;
	CI_OperatorNit_t		*pstOpNit;
	CI_OperatorNitTsLoop_t	*pstTsLoop;
	SvcCas_OperatorSvc_t		*pstSvc;

	HxLOG_Print("==========  CICAM NIT (%d) ==========\n", pstContents->usSlotId);
	HxLOG_Print("operator profile name : %s\n", pstContents->szProfName);

/*
	usListNum = UTIL_LINKEDLIST_GetNumListItemsInPointerList(pstContents->pTuneList, NULL, NULL);
	for(i = 0 ; i < usListNum ; i++)
	{
		pList = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pstContents->pTuneList, i, NULL, NULL);
		if(pList == NULL)
			continue;
	}
*/
	usListNum = UTIL_LINKEDLIST_GetNumListItemsInPointerList(pstContents->pNitList, NULL, NULL);
	for(i = 0 ; i < usListNum ; i++)
	{
		HxLOG_Print("   ==========  NIT (%d) ==========\n", i);

		pList = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pstContents->pNitList, i, NULL, NULL);
		if(pList == NULL)
			continue;

		pstOpNit = (CI_OperatorNit_t *)UTIL_LINKEDLIST_GetContents(pList);
		HxLOG_Print("   network id : %d\n", pstOpNit->usNetId);
		HxLOG_Print("   network name : %s\n", pstOpNit->szNetName);

		usTsLoopNum = UTIL_LINKEDLIST_GetNumListItemsInPointerList(pstOpNit->pTsLoopList, NULL, NULL);
		for(j = 0 ; j < usTsLoopNum ; j++)
		{
			HxLOG_Print("      ==========  TS (%d) ==========\n", j);

			pList = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pstOpNit->pTsLoopList, j, NULL, NULL);
			if(pList == NULL)
				continue;

			pstTsLoop = (CI_OperatorNitTsLoop_t *)UTIL_LINKEDLIST_GetContents(pList);

			HxLOG_Print("      ts id : %d\n", pstTsLoop->usTsId);
			HxLOG_Print("      on id : %d\n", pstTsLoop->usOnId);

			usSvcNum = UTIL_LINKEDLIST_GetNumListItemsInPointerList(pstTsLoop->pSvcList, NULL, NULL);
			for(k = 0 ; k < usSvcNum ; k++)
			{
				HxLOG_Print("         ==========  SVC (%d) ==========\n", k);

				pList = UTIL_LINKEDLIST_FindNthListItemFromPointerList(pstTsLoop->pSvcList, k, NULL, NULL);
				if(pList == NULL)
					continue;

				pstSvc = (SvcCas_OperatorSvc_t *)UTIL_LINKEDLIST_GetContents(pList);
				HxLOG_Print("         service id : %x\n", pstSvc->usSvcId);
				HxLOG_Print("         service type : %x\n", pstSvc->ucSvcType);
				HxLOG_Print("         visible flag : %d\n", pstSvc->bVisible);
				HxLOG_Print("         selectable flag : %d\n", pstSvc->bSelectable);
				HxLOG_Print("         logical channel number : %d\n", pstSvc->usLcn);
				HxLOG_Print("         service provider name : %s\n", pstSvc->szSvcPrvName);
				HxLOG_Print("         service name : %s\n", pstSvc->szSvcName);
			}
		}
	}

}

#endif
