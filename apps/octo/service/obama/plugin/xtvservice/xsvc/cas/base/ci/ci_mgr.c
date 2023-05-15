/**
	@file     ci_mgr.c
	@brief    ci_mgr.c (CAS Service)

	Description: CAS Manager�� Interface �� CI Stack ���� ���� ���� CI Manager.					\n
	Module: PAL/CAS									\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <ci.h>
#include <ci_reshandler.h>
#if defined(CONFIG_MW_CI_PLUS)
#include <ci_plus.h>
#endif
#include <ci_mgr.h>

//#include <_svc_cas_err.h>
#include <_svc_cas_mgr.h>
//#include <_svc_cas_mgr_common.h>
#include <_svc_cas_mgr_port.h>
#include <_svc_cas_sub_svc_api.h>

//#include <svc_cas.h>

//#include <pal_dsc.h>
//#include <pal_audio.h>
//#include <pal_video.h>

//#include <db_param.h>	// for local_cimgr_GetServiceId()
#include <db_svc.h>

#include <util.h>
#include <linkedlist.h>


#if defined(CONFIG_MW_MM_PVR)
//#include <svc_fs.h>
#include <svc_rec.h>
#include <svc_pb.h>
#include <svc_meta.h>

#endif


#include "ci_mgr_int.h"

#include "../../local_include/_xsvc_cas.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      For Debugging   *************************/
/*******************************************************************/





//#define DEBUG_ACT_LIST


typedef struct
{
	HBOOL					bUsed;					// Sdt slot flag
	HUINT32					ulActionId;				// Action Id
	HUINT32					ulDemuxId;				// current demux id /* HUINT32 */
	HUINT16					usTsId;					// current network ts id
	HUINT16					usUniqueId;				// octo's unique id

	//Section Info
	POINTER_LIST_T			*pstList;				// descriptor list
	HUINT32 				ulSvcNum;				// number of service in sdt act.

	//FilterParam
	HUINT32 				ulFilterId;				// sdt filter id.
	HUINT8					ucTableId;				// sdt table id.
	HUINT16					usExtId;				// ts id.
	HUINT16					usPid;					// sdt pid.
	HUINT32					eFilterMode;			// PalSef_FilterMode_e (once, monitor, continue)
	SvcCas_FilterHead_e		eHead;					// head.
	SvcCas_SiTableState_e	ulTblState;				// Filter state.
	HBOOL					bUseVerNum;				// filtering condition.
	HBOOL					bCrc;					// filtering condition.
	HBOOL					bUseExtId;				// filtering condition.
	HBOOL					bTableFilter;			// filtering condition.
} CAS_SdtStatus_t;

/***************************************************************
* external variables and functions
***************************************************************/

//2009.02.06 BSPARK. Mmi Open �� Retry ���� �ʵ��� ����.
extern HBOOL g_bMmiOpened;

#if defined(SUPPORT_SMART_CI_MMI_PINCODE)
extern int CiMmi_CheckWaitingUserInput(CI_SLOT_ID usSlotId, unsigned long* IsWaiting);
#endif
extern int CiAdapt_Init(void);
extern const HINT8 *svc_cas_DbgMakeStrActionType(SvcCas_ActionType_e cmd);
extern const HINT8 *svc_cas_DbgMakeStrSiTblState(SvcCas_SiTableState_e eState);
extern const HINT8 *svc_cas_DbgMakeStrEsType(EsType_t eType);

/***************************************************************
* global variables and structures
***************************************************************/

// ca system id �� �����ϴ����� Ȯ��.
HUINT8 g_aucIsCaSysId[CARM_MAX_CISLOT_NUMBER];	/* BS : trans.c���� ���. */

/***************************************************************
* STATIC variables and structures
***************************************************************/

STATIC CI_CamInst_t s_astCiCam[CARM_MAX_CISLOT_NUMBER];
STATIC CI_Context_t s_astCiContext[CI_MAX_NUM_OF_ACT];

STATIC HULONG s_ulCiMgrTaskId;
STATIC HULONG s_ulCiMgrMsgQId;

STATIC HUINT32 s_ulNumOfCiSlot; /* platform ���� �����ϴ� capability�� h/w�� slot �����̸�, ci mgr������ Config�� ���ǵ� slot ������ ����Ͽ� ó���Ѵ�. */

#if defined (CONFIG_MW_CI_PLUS)
STATIC DxCopyrightControl_CiPlusUriInfo_t s_stUriInfo[CARM_MAX_CISLOT_NUMBER];
#endif

//CI Plus �� SDT Status
STATIC CAS_SdtStatus_t s_stSdtStatus[CAS_MAX_NUM_OF_TS];

STATIC HBOOL s_bUriSent;

/***************************************************************
* STATIC function prototypes
***************************************************************/

/* For Debug */
STATIC const HINT8	* local_cimgr_Cmd2Str(CI_RCV_EVENT cmd);

/* For Descrambling */
STATIC HERROR	local_cimgr_StartDescramble(CI_Context_t *pCiContext, CI_SLOT_ID usSlotId, HBOOL bIsUpdate, HBOOL bIsRetry);
STATIC HERROR	local_cimgr_StopDescramble(CI_Context_t *pCiContext, CI_SLOT_ID usSlotId);

/* For Retry */
STATIC HERROR	local_cimgr_StartRetrySvc(CI_Context_t *pCiContext);
STATIC HERROR	local_cimgr_StopRetrySvc(CI_Context_t *pCiContext);

#if defined (CONFIG_MW_CI_PLUS)
/* For Ci Plus Descramble */
STATIC HERROR local_cimgr_ProcDscSetForCiPlus(CI_CamInst_t *pCam, HUINT32 ulDemuxId, HBOOL bIsUpdate, HBOOL bIsRetry, HBOOL bCalledByStartDescramble /* �̰� ����� �뵵 */);
STATIC HERROR local_cimgr_CheckUriControl(CI_Context_t *pCiContext, CI_CamInst_t *pCam);
STATIC HERROR local_cimgr_ProcCheckShunCondition(CI_SLOT_ID usSlotId, CAS_CiDscInfo_t *pstCiDsc, HBOOL *bIsNeedShun);
#endif

STATIC HERROR local_cimgr_ChangePath(CI_Context_t *pContext, HUINT16 usSlotId, HBOOL bConnect);

/***************************************************************
* function bodies
***************************************************************/

////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_ETC___
////////////////////////////////////////////////////////////

STATIC void local_cimgr_InitContext(HUINT32 contextId)
{
	HUINT32 i = 0;

	HxSTD_memset(&s_astCiContext[contextId], 0x00, sizeof(CI_Context_t));

	s_astCiContext[contextId].hAction = HANDLE_NULL;
	s_astCiContext[contextId].hSvc = HANDLE_NULL;
	s_astCiContext[contextId].eActType = eCAS_ActionType_None;
	s_astCiContext[contextId].eState = eCiMgrState_Stopped;
	s_astCiContext[contextId].bPathConnected = FALSE;
	s_astCiContext[contextId].bSendCaPmt = FALSE;

	s_astCiContext[contextId].ulRetryTimerId = INVALID_TIMER_ID;

#if defined(_LIMIT_CI_RETRY_COUNT_)
	s_astCiContext[contextId].ucRetryCnt = 0;
#endif

	s_astCiContext[contextId].pCam = NULL;

	s_astCiContext[contextId].ulSrcId = CAS_RESOURCE_NONE;
	s_astCiContext[contextId].ulDemuxId = CAS_RESOURCE_NONE;

#if defined(CONFIG_MW_CI_PLUS)
	s_astCiContext[contextId].usUniqueId = 0;
	s_astCiContext[contextId].usSvcId = 0;

	s_astCiContext[contextId].eSdtTblState = eCASSI_Table_Skipped;
#if defined(_USE_SHUNNING_STATE_)
	s_astCiContext[contextId].eShunState = eCiPlus_ShunnedFlag_None;
#endif
	s_astCiContext[contextId].bSvcShunned = FALSE;
	s_astCiContext[contextId].bRcvUri = FALSE;
#endif

	for(i=0; i<CAS_MAX_NUM_OF_ES; i++)
	{
		s_astCiContext[contextId].stProgInfo.aEsInfo[i].usEsPid = PID_NULL;
	}

	return;
}

STATIC void local_cimgr_InitContexts(void)
{
	HUINT32 i;

	for (i = 0; i < CI_MAX_NUM_OF_ACT; i++)
	{
		local_cimgr_InitContext(i);
	}

	return;
}

STATIC void local_cimgr_InitCamInst(HUINT16 usSlotId)
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
{
	HUINT32 j = 0;

	for (i = 0; i < CI_MAX_NUM_OF_DMX; i++)
	{
		for (j = 0; j < CAS_MAX_NUM_OF_DSC; j++)
	{
			s_astCiCam[usSlotId].stDscInfo[i][j].ulDscId = CAS_RESOURCE_NONE;
			s_astCiCam[usSlotId].stDscInfo[i][j].usPid = PID_NULL;
		}
	}
}
#endif

	return;
}

STATIC void local_cimgr_InitCamInsts(void)
{
	HUINT32 i;

	for (i = 0; i < CARM_MAX_CISLOT_NUMBER; i++)
	{
		local_cimgr_InitCamInst(i);
	}

	return;
}


/*
context�� action id ������ �����Ѵ�.
get context id�� �ݵ�� Proc������ �ϵ��� �Ѵ�.
request������ �Ұ���.
*/
STATIC CI_Context_t * local_cimgr_GetContextByActionId(Handle_t hAction)
{
	HUINT32 ulActionId = 0;

	if (hAction == HANDLE_NULL)
	{
		HxLOG_Error("handle is null. \n");
		return (CI_Context_t *)NULL;
	}

	ulActionId = GET_ACTION_ID(hAction);
	if(ulActionId >= CI_MAX_NUM_OF_ACT)
	{
		HxLOG_Error("action id is wrong. \n");
		return (CI_Context_t *)NULL;
	}

	return &s_astCiContext[ulActionId];

}

////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_CI_PLUS_DESC___
////////////////////////////////////////////////////////////

#ifdef CONFIG_MW_CI_PLUS

STATIC void local_cimgr_FreeCiDescList(void *pvData)
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

STATIC HBOOL local_cimgr_FilterCiDscList(void *pvItem, void *pvFilter)
{
	CAS_CiDscInfo_t *pData = (CAS_CiDscInfo_t *)pvItem;
	HUINT16 usSvcId = *(HUINT16*)pvFilter;

	if(pData->usSvcId == usSvcId)
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HERROR local_cimgr_InitSdtStatus(HUINT32 ulIdx)
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
#ifdef CONFIG_MW_CI_PLUS
	if(pSdt->pstList != NULL)
	{
		UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(pSdt->pstList, local_cimgr_FreeCiDescList);
	}
#endif
	pSdt->pstList = NULL;
	pSdt->ulSvcNum = 0;
	pSdt->ulFilterId = CAS_RESOURCE_NONE;
	pSdt->ucTableId = 0xFF;
	pSdt->usExtId = 0;
	pSdt->usPid = PID_NULL;
	pSdt->eFilterMode = 0;
	pSdt->eHead = 2;
	pSdt->ulTblState = 0;
	pSdt->bUseVerNum = 0;
	pSdt->bCrc = 0;
	pSdt->bUseExtId = FALSE;
	pSdt->bTableFilter = FALSE;

	return ERR_OK;
}

STATIC CAS_SdtStatus_t *local_cimgr_AllocSdtSlot(HUINT16 usUniqueId)
{
	HUINT32 i;

	for(i=0; i<CAS_MAX_NUM_OF_TS; i++)
	{
		if(s_stSdtStatus[i].bUsed == CAS_NOT_USED)
		{
			if(s_stSdtStatus[i].usUniqueId != 0)
			{
				HxLOG_Critical("\n\n");
			}

			s_stSdtStatus[i].usUniqueId = usUniqueId;
			s_stSdtStatus[i].bUsed = CAS_USED;

			return &s_stSdtStatus[i];
		}
	}

	return NULL;
}

STATIC CAS_SdtStatus_t *local_cimgr_GetSdtSlot(HUINT16 usUniqueId)
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

STATIC HERROR local_cimgr_FreeSdtSlot(HUINT16 usUniqueId)
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
					UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(s_stSdtStatus[i].pstList, local_cimgr_FreeCiDescList);
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


STATIC HERROR local_cimgr_ParseSdtForCiPlus(CI_Context_t *pCiContext, HUINT8 *pSdtRaw)
{
	HUINT32 nResult = 0;
	CAS_SdtStatus_t *pSdt = NULL;
	CAS_CiDscInfo_t *pCiDes = NULL;
	HUINT8 *pCiDesRaw = NULL;
	HUINT32 i = 0, j = 0;

	svcCas_SiSdtSectionInfo_t sdtInfo;
	SvcCas_StdServiceInfo_t sdtSvcInfo;

	HUINT32 ulSecCnt = 0;
	HUINT32 ulSvcInfoCnt = 0;
	HUINT32 ulSvcDesCnt = 0;
	HUINT32 ulCiDscLength = 0;

	HUINT16 usUniqueId = 0;

	if(pCiContext == NULL || pSdtRaw == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	usUniqueId = pCiContext->usUniqueId;

	local_cimgr_FreeSdtSlot(usUniqueId);
	pSdt = local_cimgr_AllocSdtSlot(usUniqueId);
	if(pSdt == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	svc_cas_SubSvcSiGetSdtSectionInfoFromRaw(pSdtRaw, &sdtInfo);
	HxLOG_Print("\n --- section %d / %d ---\n\n", i+1, ulSecCnt);
	HxLOG_Print(" Sdt TsId = 0x%x\n", sdtInfo.usTsId);
	HxLOG_Print(" Sdt ucVer = 0x%x\n", sdtInfo.ucVer);
	HxLOG_Print(" Sdt usOrgNetId = 0x%x\n", sdtInfo.usOrgNetId);
	HxLOG_Print(" Sdt ulCrc32 = 0x%x\n\n", sdtInfo.ulCrc32);

	if( (pCiContext->usTsId == sdtInfo.usTsId) && (pCiContext->usOnId == sdtInfo.usOrgNetId) )
	{
		SIxTextEncoding_e eTextEnc;

		eTextEnc = svc_cas_MgrGetTextEncoding(svc_cas_SubSvcRmGetActionId(pCiContext->hAction))

		// ������ Triple Id ��.
		nResult = svc_cas_SubSvcSiCountSdtServiceFromRaw(pSdtRaw, &ulSvcInfoCnt);
		if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

		// SDT Status Update.
		HxLOG_Print(" -> # of Service = %d\n", ulSvcInfoCnt);
		pSdt->ulSvcNum = ulSvcInfoCnt;

		for(j=0; j<ulSvcInfoCnt; j++)
		{
			nResult = svc_cas_SubSvcSiGetSdtServiceInfoFromRaw(pSdtRaw, j, &sdtSvcInfo);

			HxLOG_Print("\n\t --- service %d / %d ---\n\n", j+1, ulSvcInfoCnt);
			HxLOG_Print("\t Sdt SvcId = 0x%x\n", sdtSvcInfo.usSvcId);
			HxLOG_Print("\t Sdt bEitSched = 0x%x\n", sdtSvcInfo.bEitSched);
			HxLOG_Print("\t Sdt bEitPreFollow = 0x%x\n", sdtSvcInfo.bEitPreFollow);
			HxLOG_Print("\t Sdt ucRunningStatus = 0x%x\n", sdtSvcInfo.ucRunningStatus);
			HxLOG_Print("\t Sdt bCaFree = 0x%x\n", sdtSvcInfo.bCaFree);

			pCiDes = (CAS_CiDscInfo_t *)OxCAS_Malloc(sizeof(CAS_CiDscInfo_t));
			if(pCiDes == NULL)
			{
				HxLOG_Critical("\n\n");
				return ERR_FAIL;
			}
			HxSTD_memset(pCiDes, 0x00, sizeof(CAS_CiDscInfo_t));
			pCiDes->usSvcId = sdtSvcInfo.usSvcId;

			nResult = svc_cas_SubSvcSiCountSdtServiceDescriptorFromRaw(pSdtRaw, j, TAG_CI_PROTECTION_DESCRIPTOR, &ulSvcDesCnt);
			if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

			HxLOG_Print("\t # of Descriptor = %d\n\n", ulSvcDesCnt);

			if(ulSvcDesCnt != 0)
			{
				nResult = svc_cas_SubSvcSiGetSdtServiceDescriptorFromRaw(pSdtRaw, eTextEnc, j, TAG_CI_PROTECTION_DESCRIPTOR, 0, (void**)&pCiDesRaw);
				if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

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
						HxLOG_Critical("\n\n");
					}

					ulCiDscLength = pCiDes->pDesRaw[1];

					if (ulCiDscLength == 0)
					{
						HxLOG_Critical("\n\n");
					}

					HxLOG_Print("\t ci_protection_descriptor length = %d\n", ulCiDscLength);

					pDsc = &pCiDes->pDesRaw[2];
					pCiDes->bFreeCiFlag = (*pDsc & 0x80) >> 7;
					pCiDes->bMatchBrandFlag = (*pDsc++ & 0x40) >> 6;

					HxLOG_Print("\t free_ci_mode_flag = %d, match_brand_flag = %d\n", pCiDes->bFreeCiFlag, pCiDes->bMatchBrandFlag);

					if(pCiDes->bMatchBrandFlag == 1 && ulCiDscLength > 1)
					{
						pCiDes->ucNumOfBrandId = *pDsc++;

						HxLOG_Print("\t # of brand id = %d\n", pCiDes->ucNumOfBrandId);

						for(ulLoop = 0; ulLoop < pCiDes->ucNumOfBrandId; ulLoop++)
						{
							pCiDes->usCiBrandId[ulLoop] = ((unsigned short)*pDsc++ << 8) & 0xff00;
							pCiDes->usCiBrandId[ulLoop] |= (unsigned short)*pDsc++;

							HxLOG_Print("\t (%d)-th brand id = 0x%x\n", ulLoop+1, pCiDes->usCiBrandId[ulLoop]);
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
				HxLOG_Print("\t -> no ci_protection_descriptor (ulSvcDesCnt is 0)\n");
				pCiDes->bCiDscFlag = FALSE;
			}

			pSdt->pstList = UTIL_LINKEDLIST_AppendListItemToPointerList(pSdt->pstList, pCiDes);
			if(pSdt->pstList == NULL)
			{
				HxLOG_Critical("\n\n");
				return ERR_FAIL;
			}
		}
	}

	pSdt->ulTblState = eCASSI_Table_Running;			/* Parsing Complete. */

	return ERR_OK;
}

HERROR local_cimgr_GetCiDescInfo(CI_Context_t *pCiContext, CAS_CiDscInfo_t **pDscInfo)
{
	CAS_SdtStatus_t *pSdt = NULL;
	POINTER_LIST_T *pList = NULL;

	*pDscInfo = NULL;

	if(pCiContext == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxLOG_Print("service handle(0x%x), UniqueId (%x), pDscInfo(%x)\n", pCiContext->hSvc, pCiContext->usUniqueId, (HUINT32)pDscInfo);

	pSdt = local_cimgr_GetSdtSlot(pCiContext->usUniqueId);
	if (pSdt == (CAS_SdtStatus_t *)NULL)
	{
		HxLOG_Print("UniqueId (0x%x)\n", pCiContext->usUniqueId);
		return ERR_FAIL;
	}

	if(pSdt->bUsed == CAS_NOT_USED)
	{
		HxLOG_Error("pSdt->bUsed (%d) \n", pSdt->bUsed);
		return ERR_FAIL;
	}

	HxLOG_Print("(%s) \n", svc_cas_DbgMakeStrSiTblState(pSdt->ulTblState) );

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

	pList = UTIL_LINKEDLIST_IsListItemInPointerList(pSdt->pstList, NULL, NULL, local_cimgr_FilterCiDscList, (void *)&pCiContext->usSvcId);
	if(pList == NULL)
	{
		HxLOG_Error("service_id (%x) \n", pCiContext->usSvcId);
		*pDscInfo = NULL;
		return ERR_FAIL;
	}

	*pDscInfo = pList->pvContents;

	return ERR_OK;
}

#endif // #ifdef CONFIG_MW_CI_PLUS


STATIC CAS_SdtStatus_t *local_cimgr_GetSdtSlotByIndex(HUINT32 ulIdx)
{
	if(ulIdx >= CAS_MAX_NUM_OF_TS)
	{
		return NULL;
	}

	return &s_stSdtStatus[ulIdx];
}


////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_INSTANCE_MANAGEMENT___
////////////////////////////////////////////////////////////

STATIC HBOOL local_cimgr_CheckActionHandleInCamInstance(CI_CamInst_t *pCam, CI_Context_t *pCiContext)
{
	HUINT32 i = 0;

	if (pCam == NULL || pCiContext == NULL)
	{
		HxLOG_Critical("\n\n");
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

STATIC HERROR local_cimgr_ClearPath(CI_CamInst_t *pCam)
{
	HUINT32 i = 0;
	CI_Context_t *pTemp = NULL;

	if (pCam == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		if (pCam->hActList[i] != HANDLE_NULL)
		{
			HxLOG_Print("hAction (0x%08x)\n", pCam->hActList[i]);

			pTemp = local_cimgr_GetContextByActionId(pCam->hActList[i]);
			if(pTemp == NULL)
			{
				HxLOG_Critical("\n\n");
				continue;
			}

			HxLOG_Print("hAction (0x%08x) is going to clean. \n", pCam->hActList[i]);

			local_cimgr_ChangePath(pTemp, pCam->usSlotId, FALSE);

			pTemp->bSendCaPmt = FALSE;
		}
	}

	return ERR_OK;
}

STATIC HERROR local_cimgr_AddActionToCamInstance(CI_CamInst_t *pCam, Handle_t hAction)
{
	HUINT32 i = 0;
	HBOOL bFound = FALSE;

	if (pCam == NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		if (pCam->hActList[i] == hAction)
		{
			return ERR_OK;
		}
	}

	for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
	{
		if (pCam->hActList[i] == HANDLE_NULL)
		{
			bFound = TRUE;
			pCam->hActList[i] = hAction;
			pCam->ulNumOfAct++;
			break;
		}
	}

	if (pCam->ulNumOfAct >= CAS_MAX_NUM_OF_ACTION)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if(bFound != TRUE)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR local_cimgr_DeleteActionToCamInstance(CI_CamInst_t *pCam, Handle_t hAction)
{
	HUINT32 i = 0;
	HBOOL bFound = FALSE;

	if (pCam == NULL || hAction == HANDLE_NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (pCam->ulNumOfAct == 0)
	{
		HxLOG_Critical("\n\n");
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
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR local_cimgr_SearchOtherActionForConnecting(CI_CamInst_t *pCam, CI_Context_t *pCurrent)
{
	HUINT32 i = 0;

	if (pCam == NULL || pCurrent == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if ( (pCurrent->eActType == eCAS_ActionType_Live || pCurrent->eActType == eCAS_ActionType_Playback || pCurrent->eActType == eCAS_ActionType_Tsr_Play)
		!= TRUE)
	{
		HxLOG_Critical("\n\n");
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
					local_cimgr_ChangePath(&s_astCiContext[i], s_astCiContext[i].pCam->usSlotId, TRUE);

					local_cimgr_AddActionToCamInstance(pCurrent->pCam, s_astCiContext[i].hAction);
					break;
				}

				if (s_astCiContext[i].hSvc == pCurrent->hSvc)
				{
					if (s_astCiContext[i].bPathConnected == FALSE)
					{
						HxLOG_Print("hAction (0x%08x) \n", s_astCiContext[i].hAction);
						local_cimgr_ChangePath(&s_astCiContext[i], s_astCiContext[i].pCam->usSlotId, TRUE);

						local_cimgr_AddActionToCamInstance(pCurrent->pCam, s_astCiContext[i].hAction);
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
								svc_cas_DevDscClose(pCam->stDscInfo[ulDemuxId][j].ulDscId);
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

STATIC HERROR local_cimgr_GetResourceId(CI_Context_t *pContext)
{
	HUINT32 ulTunerId = RxRSCID_NULL;
	HUINT32 nResult = 0;
	Handle_t hAction = HANDLE_NULL;

	if (pContext == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	hAction = pContext->hAction;

	nResult = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eCAS_RM_ITEM_DEMUX, &pContext->ulDemuxId);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	if(pContext->eActType == eCAS_ActionType_Live)
	{
		nResult = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eCAS_RM_ITEM_TUNER_GROUP, &ulTunerId);
		if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

		pContext->ulSrcId = ulTunerId;
	}
	else if(pContext->eActType == eCAS_ActionType_Recording)
	{
		nResult = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eCAS_RM_ITEM_TUNER_GROUP, &ulTunerId);
		if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

		pContext->ulSrcId = ulTunerId;
	}
	else if(pContext->eActType == eCAS_ActionType_Playback)
	{
		// 0�� ����.
		pContext->ulSrcId = 0;
	}
	else if(pContext->eActType == eCAS_ActionType_Tsr_Rec)
	{
		nResult = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eCAS_RM_ITEM_TUNER_GROUP, &ulTunerId);
		if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

		pContext->ulSrcId = ulTunerId;
	}
	else if(pContext->eActType == eCAS_ActionType_Tsr_Play)
	{
//		nResult = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hAction), eCAS_RM_ITEM_HDD_REC, &nRecId);
//		if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

		pContext->ulSrcId = 0;
	}
	else if(pContext->eActType == eCAS_ActionType_Data)
	{
		if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
	}
	else
	{
		if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
	}

	return ERR_OK;
}

STATIC HERROR local_cimgr_ChangePath(CI_Context_t *pContext, HUINT16 usSlotId, HBOOL bConnect)
{
	HUINT32			 ulActionId;

	if (NULL == pContext)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (TRUE == bConnect)
	{
		svc_cas_DevCiPlugInCi (ulActionId, (HINT32)usSlotId);
	}
	else
	{
		svc_cas_DevCiPlugInCi (ulActionId, -1);
	}

	pContext->bPathConnected = bConnect;

	return ERR_OK;
}

STATIC HERROR local_cimgr_CheckPathChanging(CI_CamInst_t *pCam, CI_Context_t *pCiContext, HBOOL *bIsNeedPathChange)
{
	HUINT32 i = 0;
	CI_Context_t *pTemp = (CI_Context_t *)NULL;
	HBOOL bIsSameSvc = FALSE;

	if (pCam == NULL || pCiContext == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	*bIsNeedPathChange = FALSE;

	HxLOG_Print("hAction (0x%08x)\n", pCiContext->hAction);

#ifdef DEBUG_ACT_LIST
	HxLOG_Print("pCam->ulNumOfAct (%d)\n", pCam->ulNumOfAct);

	HxLOG_Print("hAction List\n");
	for(i=0; i<CAS_MAX_NUM_OF_ACTION; i++)
	{
		HxLOG_Print("(0x%08x) ", pCam->hActList[i]);
	}
	HxLOG_Print("\n");
#endif

	if (pCam->ulNumOfAct == 0)
	{
		HxLOG_Print(" ulNumOfAct is zero \n");
		*bIsNeedPathChange = TRUE;
	}
	else if (pCiContext->eActType == eCAS_ActionType_Descramble_a_File)
	{
		*bIsNeedPathChange = TRUE;
	}
	else
	{
		for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
		{
			if (pCam->hActList[i] != HANDLE_NULL)
			{
				pTemp = local_cimgr_GetContextByActionId(pCam->hActList[i]);
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
					HxLOG_Print("Temp->hAction (0x%x), Temp->hSvc (0x%x), pCiContext->hAction (0x%x), pCiContext->hSvc (0x%x), pTemp->bSendCaPmt (%d)\n",
						pTemp->hAction, pTemp->hSvc, pCiContext->hAction, pCiContext->hSvc, pTemp->bSendCaPmt);

					if (pTemp->bPathConnected == TRUE)
					{
						bIsSameSvc = TRUE;
						break;
					}
				}
			}
		}

		HxLOG_Print("bIsSameSvc (%d) \n", bIsSameSvc);

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
				HxLOG_Print(" pCiContext->eActType = %d \n", pCiContext->eActType);
				/* ķ�� ����� �ٸ� path release�ؾ� ��. */
				local_cimgr_ClearPath(pCam);
				*bIsNeedPathChange = TRUE;
			}
		}
	}
	HxLOG_Print(" bIsNeedPathChange (%d)\n", *bIsNeedPathChange);

#ifdef DEBUG_ACT_LIST
	HxLOG_Print("pCam->ulNumOfAct (%d)\n", pCam->ulNumOfAct);

	HxLOG_Print("hAction List\n");
	for(i=0; i<CAS_MAX_NUM_OF_ACTION; i++)
	{
		HxLOG_Print("(0x%08x) ", pCam->hActList[i]);
	}
	HxLOG_Print("\n");
#endif

	return ERR_OK;
}


STATIC HERROR local_cimgr_CheckCaPmtSending(CI_CamInst_t *pCam, CI_Context_t *pCiContext, HBOOL *bIsNeedSend)
{
	HUINT32 i = 0;
	CI_Context_t *pTemp = (CI_Context_t *)NULL;
	HBOOL bIsSameSvc = FALSE;

	if (pCam == NULL || pCiContext == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	*bIsNeedSend = FALSE;

	HxLOG_Print("hAction (0x%08x)\n", pCiContext->hAction);

#ifdef DEBUG_ACT_LIST
	HxLOG_Print("pCam->ulNumOfAct (%d)\n", pCam->ulNumOfAct);

	HxLOG_Print("hAction List\n");
	for(i=0; i<CAS_MAX_NUM_OF_ACTION; i++)
	{
		HxLOG_Print("(0x%08x) ", pCam->hActList[i]);
	}
	HxLOG_Print("\n");
#endif

	if (pCam->ulNumOfAct == 0)
	{
		HxLOG_Print(" ulNumOfAct is zero \n");
		*bIsNeedSend = TRUE;
		pCiContext->bSendCaPmt = TRUE;
		pCam->ucCurPmtVer = pCiContext->stProgInfo.ucPrePmtVer;
		pCam->ulCurPmtCrc = pCiContext->stProgInfo.ulPrePmtCrc;

	}
	else
	{
		for(i = 0; i < CAS_MAX_NUM_OF_ACTION; i++)
		{
			if (pCam->hActList[i] != HANDLE_NULL)
			{
				pTemp = local_cimgr_GetContextByActionId(pCam->hActList[i]);
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
					HxLOG_Print("Temp->hAction (0x%x), Temp->hSvc (0x%x), pCiContext->hAction (0x%x), pCiContext->hSvc (0x%x), pTemp->bSendCaPmt (%d)\n",
						pTemp->hAction, pTemp->hSvc, pCiContext->hAction, pCiContext->hSvc, pTemp->bSendCaPmt);

					if (pTemp->bSendCaPmt == TRUE)
					{
						bIsSameSvc = TRUE;
						break;
					}
				}
			}
		}

		HxLOG_Print("bIsSameSvc (%d) \n", bIsSameSvc);

		if (bIsSameSvc == TRUE)
		{
			pCiContext->bSendCaPmt = TRUE;

#if 1
			/* same service ����, pmt version�� ���� �� ��쿡�� ca pmt�� ������. */
			if (*bIsNeedSend == FALSE)
			{
				HxLOG_Print("cam version (%x) context version(%x) cam crc (%x) context crc (%x)\n",
					pCam->ucCurPmtVer, pCiContext->stProgInfo.ucPrePmtVer, pCam->ulCurPmtCrc, pCiContext->stProgInfo.ulPrePmtCrc);

				if((pCam->ucCurPmtVer != pCiContext->stProgInfo.ucPrePmtVer)
					&& (pCam->ulCurPmtCrc != pCiContext->stProgInfo.ulPrePmtCrc))
				{
					pCiContext->bSendCaPmt = TRUE;
					*bIsNeedSend = TRUE;
					pCam->ucCurPmtVer = pCiContext->stProgInfo.ucPrePmtVer;
					pCam->ulCurPmtCrc = pCiContext->stProgInfo.ulPrePmtCrc;
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
				pCam->ucCurPmtVer = pCiContext->stProgInfo.ucPrePmtVer;
				pCam->ulCurPmtCrc = pCiContext->stProgInfo.ulPrePmtCrc;

			}
			else
			{
				pCiContext->bSendCaPmt = FALSE;
			}
		}
	}
	HxLOG_Print(" bIsNeedSend (%d)\n", *bIsNeedSend);

#ifdef DEBUG_ACT_LIST
	HxLOG_Print("pCam->ulNumOfAct (%d)\n", pCam->ulNumOfAct);

	HxLOG_Print("hAction List\n");
	for(i=0; i<CAS_MAX_NUM_OF_ACTION; i++)
	{
		HxLOG_Print("(0x%08x) ", pCam->hActList[i]);
	}
	HxLOG_Print("\n");
#endif

	return ERR_OK;
}

////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_CAPMT_AND_DESCRAMBLING___
////////////////////////////////////////////////////////////

STATIC CI_CA_PMT_LIST_MANAGEMENT local_cimgr_GetCplmCmd(Handle_t hAction, CI_CamInst_t *pCam)
{
	HUINT8 ucCaSysIdSpecifier = 0;
	CI_Context_t *pCiContext = NULL;

	pCiContext = local_cimgr_GetContextByActionId(hAction);
	if (pCiContext == NULL) {
		HxLOG_Error("invalid context\n");
		return CI_CPLM_ONLY;
	}

	if (pCam == NULL)
	{
		HxLOG_Error("\n");
		return CI_CPLM_ONLY;
	}

	/* CI_CPLM_UPDATE �� ������ �ִ� ķ�� �����ϰ� ��Ģ������ ��� CI_CPLM_UPDATE �� �ؾ� ����.
	   �׷��� �Ϻ� ķ���� CI_CPLM_ONLY �ܿ��� ������ �Ǳ⵵ �ϹǷ� �̸� ���� ���� ó���� �ʿ��ϴٸ� ���⿡ �߰��� �ּ���. */
	if ((pCam->ausCaSysId != NULL) && (pCam->usManufacturer != 0))
	{
		/* <TODO_CI> : Ư�� ķ�� ���� ���� ó���� �ʿ��ϴٸ�...
		   Application Info �� application_manufacturer / manufacturer_code / menu_string_length ��
		   CA Info �� CA_system_id �� ��� üũ�ϵ��� ����.
		   ������ ķ�� ������ ���� �ʵ���... */
		ucCaSysIdSpecifier = (HUINT8)((pCam->usManufacturer >> 8) & 0x00ff); // ���⼭�� usManufacturer ��, local_cimgr_GetCaPmt()���� ausCaSysId �� ķ ���� ó���ϰ� ����. ���� �ʿ� !!
		switch (ucCaSysIdSpecifier)
		{
			case eCA_SYS_ID_ASTON:
			case SKYCIPLUS_CAM_MANUFACTURER:
//			case eCA_SYS_ID_CONAX: // local_cimgr_GetCaPmt()�� �ִ� ���뿡 �����Ϸ��� eCA_SYS_ID_CONAX �� CI_CPLM_ONLY �� �����ؾ� �� ��... -> ķ�� �ν��ϴ� ����� �ٸ��Ƿ� �ϴ� ���� �ڵ��� ���Ƶ�. ���� Ȯ�� �ʿ� !!
				return CI_CPLM_ONLY;

			case CRYPTOWORKS_CAM_MANUFACTURER:
				local_cimgr_StopDescramble(pCiContext, pCam->usSlotId); // ������ ����?
				return CI_CPLM_ONLY;

			default :
				return CI_CPLM_UPDATE;
		}
	}

	return CI_CPLM_ONLY; // if ((pCam->ausCaSysId != NULL) && (pCam->usManufacturer != 0)) ���ǿ� ���յ��� ���ϸ� �ϴ� �������� ķ�� �ƴϴ� CI_CPLM_ONLY �� �����ϴ� ���� ���� ��...
}

STATIC HUINT8 local_cimgr_GetLengthField(HUINT8 *pucLenField, unsigned short usLen)
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
	else // if (usLen <= 65535) : ca_pmt �� ���� �� �� �̻��� ó���� ��� ���ʿ��ϹǷ� ������� �ʴ´�.
	{
		*pucLenField++ = 0x82;
		*pucLenField++ = (HUINT8)((usLen >> 8) & 0x00ff);
		*pucLenField = (HUINT8)(usLen & 0x00ff);
		return 3;
	}
}

STATIC HERROR local_cimgr_MakeEsInfo(SvcCas_ActionType_e eActType,
										CI_PROGRAM_INFO *pInput,		/* in */
										CI_PROGRAM_INFO *pOutput,		/* out */
										CI_CA_PMT_LIST_MANAGEMENT nManage,
										CI_CA_PMT_CMD nCmd,
										HUINT8 ucCaSysIdSpecifier)
{
	HUINT32 i, j, k;

	/*
		���� Es Info�� ���� Cam �� exception�� �����̶�� �̰����� �����ؾ� ��.
	*/

	if(pInput == NULL || pOutput == NULL)
	{
		HxLOG_Error("input parameter is null. \n");
		return ERR_FAIL;
	}

	switch (ucCaSysIdSpecifier)
	{
		#if 0
		case eCA_SYS_ID_CONAX:
		{
			CI_ES_INFO aEsTemp[CAS_MAX_NUM_OF_ES];	// pProgInfo ������ �ʿ信 ���� �����ϱ� ����.
			// Conax Cam �� ��� update �� �������� �ʴ� ���...
			if (nManage == CI_CPLM_UPDATE)
			{
				HxLOG_Error("eCA_SYS_ID_CONAX\n");
				return ERR_CI_CMD_IGNORED;
			}

			// Conax Cam Audio �� ������ Issue �����̶�� �� : In case of CONAX CAM, CI must send CA PMT including all audio PID
			for (i=0; i<pInput->ucNumEs; i++)
			{
				if (pInput->aEsInfo[i].eEsType == eEsType_Audio)
				{
					break;
				}
			}

			if (i < pInput->ucNumEs)
			{
				HxSTD_memset(aEsTemp, 0x00, sizeof(aEsTemp));

				j = 0;
				for (i=0; i<pInput->ucNumEs; i++)
				{
					if (j >= CAS_MAX_NUM_OF_ES)
					{
						break;
					}

					if (pInput->aEsInfo[i].eEsType == eEsType_Video)
					{
						HxSTD_memcpy(&aEsTemp[j++], &pInput->aEsInfo[i], sizeof(CI_ES_INFO));
					}
				}

				for (i=0; i<pInput->pPmtInfo->ulNumOfElement; i++)
				{
					if (j >= CAS_MAX_NUM_OF_ES)
					{
						break;
					}
					if (pInput->pPmtInfo->stElementInfo[i].eEsType == eEsType_Video)
					{
						aEsTemp[j].eEsType = pInput->pPmtInfo->stElementInfo[i].eEsType;
						aEsTemp[j].usEsPid = pInput->pPmtInfo->stElementInfo[i].usPid;
						aEsTemp[j].pEs = &pInput->pPmtInfo->stElementInfo[i];
						j++;
					}

					if (pInput->pPmtInfo->stElementInfo[i].eEsType == eEsType_Audio)
					{
						aEsTemp[j].eEsType = pInput->pPmtInfo->stElementInfo[i].eEsType;
						aEsTemp[j].usEsPid = pInput->pPmtInfo->stElementInfo[i].usPid;
						aEsTemp[j].pEs = &pInput->pPmtInfo->stElementInfo[i];
						j++;
					}
				}

				for (i=0; i<pInput->ucNumEs; i++)
				{
					if (j >= CAS_MAX_NUM_OF_ES)
					{
						break;
					}

					if ((pInput->aEsInfo[i].eEsType != eEsType_Video)
						&& (pInput->aEsInfo[i].eEsType != eEsType_Audio))
					{
						HxSTD_memcpy(&aEsTemp[j++], &pInput->aEsInfo[i], sizeof(CI_ES_INFO));
					}
				}

				pOutput->ucNumEs = (HUINT8)j;
				HxSTD_memcpy(pOutput->aEsInfo, aEsTemp, sizeof(CI_ES_INFO) * pOutput->ucNumEs);
			}
		}
		break;
#endif

/* CA System ID specifier ���� exception handling�� �ʿ��ϸ� �ؿ� �߰��ϼ���. �� �ʿ����� ������ �� ����! */

//		case SOME_CA_SYS_ID:
//			...
//			break;

		default:
		{
			/*
				normal cam�� ��� pmt�� es�� �������� set dsc�� pid�� ��ġ�ϴ� es�� list up�Ѵ�.
				ca pmt �ۼ� �� ����� program info�� update�Ѵ�.
				�̰����� �ۼ��� program info�� cam instance�� update �ȴ�.
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
			HxLOG_Print(" num of es (%d) \n", k);

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

STATIC HBOOL local_cimgr_CheckEsTypeValidityForEsInfo(EsType_t eType)
{
	if ( (eType == eEsType_Video) || (eType == eEsType_Audio) || (eType == eEsType_Subtitle)
		|| (eType == eEsType_Teletext) )//	|| (eType == eEsType_Data) )
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HERROR local_cimgr_MakeEsInfoForRecording(
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

	/* host���� pmt update ���� */
	pProgramInfo->pPmtInfo = pCiContext->stProgInfo.pPmtInfo;
	if(pProgramInfo->pPmtInfo == NULL)
	{
		HxLOG_Error("pmt info is null. \n");
		return ERR_FAIL;
	}

	/* �ʱ�ȭ */
	HxSTD_memset(pCiContext->stProgInfo.aEsInfo, 0, sizeof(CI_ES_INFO) * CAS_MAX_NUM_OF_ES);
	pCiContext->stProgInfo.ucNumEs = 0;

	for (k=0, i=0; i<pCiContext->stProgInfo.pPmtInfo->ulNumOfElement; i++)
	{
		if ( (local_cimgr_CheckEsTypeValidityForEsInfo(pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].eEsType) == TRUE)
			&& (pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].usPid != PID_NULL))
			{
				pCiContext->stProgInfo.aEsInfo[k].eEsType = pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].eEsType;
				pCiContext->stProgInfo.aEsInfo[k].usEsPid = pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].usPid;
				pCiContext->stProgInfo.aEsInfo[k].pEs = &pCiContext->stProgInfo.pPmtInfo->stElementInfo[i];
				k++;
			}
	}

	pCiContext->stProgInfo.ucNumEs = k;
	HxLOG_Print(" num of es (%d) \n", k);

	local_cimgr_MakeEsInfo(pCiContext->eActType,
							&pCiContext->stProgInfo,	/* in */
							pProgramInfo,				/* out */
							nManage,
							nCmd,
							ucCaSysIdSpecifier);

	return ERR_OK;
}


STATIC HERROR local_cimgr_MakeEsInfoForWatching(
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

	if (pCiContext->eActType == eCAS_ActionType_Recording
		|| pCiContext->eActType == eCAS_ActionType_Tsr_Rec
		|| pCiContext->eActType == eCAS_ActionType_Data)
	{
		HxLOG_Error("recording, tsr_rec, data is not supported.\n");
		return ERR_FAIL;
	}

	/* host���� pmt update ���� */
	pProgramInfo->pPmtInfo = pCiContext->stProgInfo.pPmtInfo;
	if(pProgramInfo->pPmtInfo == NULL)
	{
		HxLOG_Error("pmt info is null. \n");
		return ERR_FAIL;
	}

	/* �ʱ�ȭ */
	HxSTD_memset(pCiContext->stProgInfo.aEsInfo, 0, sizeof(CI_ES_INFO) * CAS_MAX_NUM_OF_ES);
	pCiContext->stProgInfo.ucNumEs = 0;

	for (k=0, i=0; i<pCiContext->stProgInfo.pPmtInfo->ulNumOfElement; i++)
	{
		if ( (local_cimgr_CheckEsTypeValidityForEsInfo(pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].eEsType) == TRUE)
			&& (pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].usPid != PID_NULL))
	{
				pCiContext->stProgInfo.aEsInfo[k].eEsType = pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].eEsType;
				pCiContext->stProgInfo.aEsInfo[k].usEsPid = pCiContext->stProgInfo.pPmtInfo->stElementInfo[i].usPid;
				//pCiContext->stProgInfo.aEsInfo[k].pEs = &pCiContext->stProgInfo.pPmtInfo->stElementInfo[i];
				k++;
			}
	}

	pCiContext->stProgInfo.ucNumEs = k;
	HxLOG_Print(" num of es (%d) \n", k);

	local_cimgr_MakeEsInfo(pCiContext->eActType,
							&pCiContext->stProgInfo,	/* in */
							pProgramInfo,				/* out */
							nManage,
							nCmd,
							ucCaSysIdSpecifier);

	return ERR_OK;
}

/* context�� pid & pmt ������ ������ ca pmt�� ����� ���� program info�� �����. */
STATIC HERROR local_cimgr_MakeEsInfoForCaPmt(
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
		/* pid 6�� ���� */
		nResult = local_cimgr_MakeEsInfoForWatching(pCiContext, pProgram, nManage, nCmd, ucCaSysIdSpecifier);
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
		/* pmt �������� pid ���� ���� ����. */
		nResult = local_cimgr_MakeEsInfoForRecording(pCiContext, pProgram, nManage, nCmd, ucCaSysIdSpecifier);
		if(nResult != ERR_OK)
		{
			HxLOG_Error(" local_cimgr_MakeEsInfoForRecording \n");
			return ERR_FAIL;
		}
	}
	else if (pCiContext->eActType == eCAS_ActionType_Data)
	{
		/* pid �Ѱ� ���� */
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR local_cimgr_GetCaPmt(
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
	CI_PROGRAM_INFO ciProgInfo; 			// pProgInfo ������ �ʿ信 ���� �����ϱ� ����.
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
	/*--- ca_pmt () ������ �ռ� ķ�� ���� CI_PROGRAM_INFO �� ���� ���� ó���� �ʿ��ϴٸ� �̸� �۾��� �� ---*/
	/*-----------------------------------------------------------------------------------------------------*/

	/*

		ķ�� ���� ������ �� �� �ִ� ������ �� (��� ķ�� ���� �ƿ��� ����̴�) :

		1. programme level ���� ca desc �� �����ϴ� ���
		   -> �Ϲ������� �������� ����.

		2. programme level �� ES level �� ������ ca desc �� �����ϴ� ���
		   -> ��Ģ������ �� ��� ķ�� ES level �� �ִ� ca desc �� �켱�ؾ� �ϳ� �׷��� ���� ��� ������ �� �� ����.
		   -> programme level �� �ִ� ca desc �� �����ϵ��� ���� ó���Ѵ�.
		   -> �׸��� ���� ES level �� ca desc �� ���� ES PID �� �����Ѵٸ� programme level �� �ִ� ca desc �� �߰��ϵ��� �Ѵ�.

		3. ES level ���� ca desc �� �����ϴ� ���
		   -> ES level �� ca desc �� ���� ES PID �� �����Ѵٸ� ca desc �� ������ �ִ� ES PID ���� ������ �Ǵ� ��찡 �ִٰ� �� (Viaccess, Polsat)
		   -> ca desc �� ���� ES PID �� �����ϵ��� ���� ó���Ѵ� (local_cimgr_ProcSetDsc()�� Viaccess ķ�� ���� ó���� �ڵ尡 �����Ƿ� �ϴ� �ű⿡ ���� �߰��ϸ� ��)

		4. �ټ��� ������� ���� ��� ����� PID �� �ѹ��� ��� �����ϸ� �Ϻ� ������� ������ �ʴ� ���
		   -> ���� ��� ������� �����ϵ��� ���� ó���Ѵ�. �� ��� ���� �ҽ� ������ �߰� ������ �ʿ䰡 ����.

	    5. �ټ��� ca desc ���� ������ H/E ���� (simulcrypt) ķ�� ���� ca_system_id �� ��Ī���� �ʴ� ca desc �� �Բ� ������ ��� (_Contains_all_the_CA_descriptors_ define �����)
	       -> ķ�� ca_system_id �� �ش� �Ǵ� ca desc �� ���� �͸� ó���ϵ��� ���� ó���� �ϵ��� �Ѵ�.
	       -> ���� 2~4 ���� �Բ� ����Ͽ� ó���� ��.

		=> ������ ������ ��ȸ�� �� �ֵ��� OP�� H/E ������ �����ϰų� H/E �������� �ذ���� ���ϴ� ���̰ų� H/E ������ ����ġ ���� ��� �Ʒ��� ���� ó���� �ϵ��� �Ѵ�.

	*/

	if (pCam->usCaInfoCnt > 0)
	{
		/* <TODO_CI> : Ư�� ķ�� ���� ���� ó���� �ʿ��ϴٸ�...
		   Application Info �� application_manufacturer / manufacturer_code / menu_string_length ��
		   CA Info �� CA_system_id �� ��� üũ�ϵ��� ����.
		   ������ ķ�� ������ ���� �ʵ���... */
		ucCaSysIdSpecifier = (HUINT8)((pCam->ausCaSysId[0] >> 8) & 0x00ff); // ���⼭�� ausCaSysId ��, local_cimgr_GetCplmCmd()���� usManufacturer �� ķ ���� ó���ϰ� ����. ���� �ʿ� !!
	}

	HxLOG_Print("ucCaSysIdSpecifier (0x%x)\n", ucCaSysIdSpecifier);

	/*
		pmt, set dsc �� ������ ���� ci Prog Info�� Update�Ͽ� �ش�.
		context�� ������ ���� �״�� �����ϰ�,
		ci Prog Info���� �����Ѵ�.
	*/
	nResult = local_cimgr_MakeEsInfoForCaPmt(pCiContext, &ciProgInfo, ucCaSysIdSpecifier, nManage, nCmd);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("local_cimgr_MakeEsInfoForCaPmt.\n");
		return ERR_FAIL;
	}


	/*---------------------------------------*/
	/*--- ca_pmt () ������ ���� ���� ��� ---*/
	/*---------------------------------------*/

	// calculate ca_pmt() length
	usCaPmtLen = 9;	// ca_pmt_tag + ca_pmt_list_management + program_number
					// + version_number + current_next_indicator + program_info_length

	/* pPmtInfo�� ci Mgr�� member��. pointer ���� ���ɼ��� ������,  */
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
		#else // �̰� CI �� CI+ ���忡 �´�.
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
			#else // �̰� CI �� CI+ ���忡 �´�.
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

#ifndef _Support_CA_PMT_in_Clear_ // hmkim : added / modified (�̵��� ķ���� ä�� ����ø��� Check Your Smartcard �� This Channel is Scrambled ��� MMI �� ��� �ߴ� ������ �־...)
/* <TODO_CI> : Ư�� ķ�� ���� ���� ó���� �ʿ��ϴٸ�...
   Application Info �� application_manufacturer / manufacturer_code / menu_string_length ��
   CA Info �� CA_system_id �� ��� üũ�ϵ��� ����.
   ������ ķ�� ������ ���� �ʵ���... */
//	if (nCmd == CI_CPCI_NOT_SELECTED)
	if ((nCmd == CI_CPCI_NOT_SELECTED) && !((((pCam->usManufacturer >> 8) & 0x00ff) == eCA_SYS_ID_IRDETO) && (((pCam->ausCaSysId[0] >> 8) & 0x00ff) == eCA_SYS_ID_IRDETO)))
	{
		usCaPmtLen = 10;
		usProgInfoLen = 1;
	}
#endif

	ucLenFieldSize = local_cimgr_GetLengthField(aucLenField, (unsigned short)(usCaPmtLen-3));
	usCaPmtLen += ucLenFieldSize;

	pucCaPmt = (HUINT8 *)OxCAS_Malloc(usCaPmtLen);
	if (pucCaPmt == NULL)
	{
		HxLOG_Error("pucCaPmt is null\n");
		return ERR_FAIL;
	}

	/*-------------------------------*/
	/*--- �������� ca_pmt () ���� ---*/
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

#ifndef _Support_CA_PMT_in_Clear_ // hmkim : added / modified (�̵��� ķ���� ä�� ����ø��� Check Your Smartcard �� This Channel is Scrambled ��� MMI �� ��� �ߴ� ������ �־...)
/* <TODO_CI> : Ư�� ķ�� ���� ���� ó���� �ʿ��ϴٸ�...
   Application Info �� application_manufacturer / manufacturer_code / menu_string_length ��
   CA Info �� CA_system_id �� ��� üũ�ϵ��� ����.
   ������ ķ�� ������ ���� �ʵ���... */
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

		#ifndef _Contains_all_the_CA_descriptors_ // org
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
		#else // �̰� CI �� CI+ ���忡 �´�.
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

			#ifndef _Contains_all_the_CA_descriptors_ // org
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

						HxSTD_memcpy(&pucCaPmt[usIdx], aEsInfo[i].pEs->stElementCaDescriptor[j].pucRaw, aEsInfo[i].pEs->stElementCaDescriptor[j].ucRawLen); // CA_descriptor
						usIdx += aEsInfo[i].pEs->stElementCaDescriptor[j].ucRawLen;
						break;
					}
				}
			}
			#else // �̰� CI �� CI+ ���忡 �´�.
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
			#endif
		}
	}

#if defined (CONFIG_MW_CI_PLUS)

	/* �ʱ�ȭ */
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
#ifndef _Support_CA_PMT_in_Clear_ // hmkim : added.
	}
#endif

	pCaPmt->pucRaw = pucCaPmt;
	pCaPmt->usRawLen = usCaPmtLen;

	return ERR_OK;
}

#if 0 // 2010�⵵ ���̽� �ڵ忡���� ���̻� ������ �ʾ� ����. org is #if defined(_Support_CA_PMT_in_Clear_) || defined(_Contains_all_the_CA_descriptors_)
/* �� �Լ��� local_cimgr_StartDescramble() �Լ��ʹ� �޸� FTA ä�� �� Ÿ ī�� ä�ο��� ����ϱ� ���� �뵵��
   ���� PMT �κ��� �ִ� �״���� ca_pmt �� �ش� ���� ķ�� ������ ���� ������� ����. */
/* STATIC */ HERROR local_cimgr_SendCaPmt(CI_SLOT_ID usSlotId, SvcCas_PmtInfo_t *pstPmtInfo)
{
	CI_CamInst_t *pCam = NULL;
	CI_PROGRAM_INFO stProgInfo;
	HERROR nResult = ERR_FAIL;
	int i;

	HxLOG_Print("usSlotId = %d\n", usSlotId);

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	if (CARM_CI_CheckModule(usSlotId) == eCAS_DEV_CI_EXTRACTED)
	{
		HxLOG_Print("cam is extracted\n");
		return ERR_OK;
	}

	pCam = &s_astCiCam[usSlotId];

	if (pCam->usSlotId != usSlotId)
	{
		/*
			FTA �� ����. CI Init�� �Ϸ���� ���� ���¶�� �׻� ���� ���� ..
		*/
		HxLOG_Error("pCam->usSlotId = %x, usSlotId = %x\n", pCam->usSlotId, usSlotId);
	}

	if (pCam->usCaInfoCnt == 0)
	{
		HxLOG_Error("CaInfoCnt (%d)\n", pCam->usCaInfoCnt);
		return ERR_FAIL;
	}

	stProgInfo.pPmtInfo = pstPmtInfo;
	HxSTD_memset(stProgInfo.aEsInfo, 0, sizeof(CI_ES_INFO) * CAS_MAX_NUM_OF_ES);
	stProgInfo.ucNumEs = 0;

	for (i = 0; i < pstPmtInfo->ulNumOfElement && i < CAS_MAX_NUM_OF_ES; i++)
	{
		stProgInfo.aEsInfo[i].eEsType = pstPmtInfo->stElementInfo[i].eEsType;
		stProgInfo.aEsInfo[i].usEsPid = pstPmtInfo->stElementInfo[i].usPid;
		stProgInfo.aEsInfo[i].pEs = &pstPmtInfo->stElementInfo[i];
		stProgInfo.ucNumEs++;
	}

	if (pCam->caPmtOkDsc.pucRaw != NULL)
	{
		HxLOG_Print("caPmtOkDsc is not null, make free previous caPmtOkDsc\n");
		OxCAS_Free(pCam->caPmtOkDsc.pucRaw);
		pCam->caPmtOkDsc.pucRaw = NULL;
		pCam->caPmtOkDsc.usRawLen = 0;
	}

	nResult = local_cimgr_GetCaPmt(pCam, &stProgInfo, CI_CPLM_ONLY, CI_CPCI_OK_DSC, &pCam->caPmtOkDsc);
	if (nResult != ERR_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	if (pCam->caPmtOkDsc.pucRaw != NULL)
	{
		if (CI_SendCaPmt(usSlotId, pCam->caPmtOkDsc.pucRaw, pCam->caPmtOkDsc.usRawLen) != ERR_OK)
		{
			HxLOG_Error("\n");
//			return ERR_FAIL;
		}
	}

  	HxLOG_Print("CI_SendCaPmt : slot[%d], pucRaw[0x%x], len[%d]\n", usSlotId, (HUINT32)pCam->caPmtOkDsc.pucRaw, pCam->caPmtOkDsc.usRawLen);

	return ERR_OK;
}
#endif

/* �� �Լ��� "Invalid Service" ä�ο��� clear ca_pmt �� ������ ���� �뵵�� ������� ����. #if !defined(_Send_Not_Selected_CA_PMT_on_Zapping_or_RetrySvc_) �� �����Ǵ� �ڵ���. */
#if 0 //defined but not used
STATIC HERROR local_cimgr_SendCaPmt_for_InvalidService(CI_SLOT_ID usSlotId)
{
	HUINT8 pucCaPmt[20];
	unsigned short usCaPmtLen = 10;	// ca_pmt_tag(3) + length_field(1) + ca_pmt_list_management(1) + program_number(2) + reserved/version_number/current_next_indicator(1) + reserved/program_info_length(2)
	unsigned short usIdx = 0;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	if (CARM_CI_CheckModule(usSlotId) == eCAS_DEV_CI_EXTRACTED)
	{
		HxLOG_Print("cam is extracted\n");
		return ERR_OK;
	}

	// PMT �� ��ȿ���� �����Ƿ� ���Ƿ� �����Ѵ� : 9F 80 32 06 03 00 01 03 00 00 (no elementary stream)

	/* ca_pmt_tag (24 bits) */
	pucCaPmt[usIdx++] = (HUINT8)((CI_CA_PMT >> 16) & 0xff);
	pucCaPmt[usIdx++] = (HUINT8)((CI_CA_PMT >> 8) & 0xff);
	pucCaPmt[usIdx++] = (HUINT8)(CI_CA_PMT & 0xff);

	/* length field */
	pucCaPmt[usIdx++] = 0x06;

	/* ca_pmt_list_managrment (8 bits) */
	pucCaPmt[usIdx++] = CI_CPLM_ONLY;

	/* program_number (16 bits) */ /* TODO : program_number �� ������ ������ �ϸ� ���ڴ� */
	pucCaPmt[usIdx++] = 0xff;
	pucCaPmt[usIdx++] = 0xff;

	/* reserved (2 bits) + version_number (5 bits) + current_next_indicator (1 bit) */
	pucCaPmt[usIdx++] = 0x03;

	/* reserved (4 bits) + program_info_length (12 bits) */
	pucCaPmt[usIdx++] = 0x00;
	pucCaPmt[usIdx++] = 0x00;

	if (CI_SendCaPmt(usSlotId, pucCaPmt, usCaPmtLen) != ERR_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif
/* bIsUpdate : stProgInfo �� ���ŵǾ� update �ϴ� ��� */
/* bIsRetry  : stProgInfo �� ���ŵ��� ���� ���¿��� retry �� �ϴ� ��� */
/*      ���� : bIsUpdate �� bIsRetry �� ���� ���õǴ� ���� ����� �Ѵ� */
STATIC HERROR local_cimgr_StartDescramble(CI_Context_t *pCiContext, CI_SLOT_ID usSlotId, HBOOL bIsUpdate, HBOOL bIsRetry) // TODO : Update/Retry....���������� �ϴ°� ���� �ʳ�.
{
	CI_CamInst_t *pCam = NULL;
	CI_CA_PMT_LIST_MANAGEMENT eCplmCmd;
	HERROR nResult = ERR_FAIL;
	HBOOL bNeedCaPmtSend = FALSE;

	HxLOG_Print("usSlotId = %d, bIsUpdate = %d, bIsRetry = %d\n", usSlotId, bIsUpdate, bIsRetry);

	if (pCiContext == NULL)
	{
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	pCam = pCiContext->pCam;
	if(pCam == NULL)
	{
		/* cam instance�� context�� �Ҵ� ���� ���� ����. */
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
		HxLOG_Print("cam is extracted\n");
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

	local_cimgr_CheckCaPmtSending(pCam, pCiContext, &bNeedCaPmtSend); // bIsRetry �� true �� ��쵵 �ҷ��� �Ѵ�...?

	local_cimgr_AddActionToCamInstance(pCam, pCiContext->hAction);

#ifdef DEBUG_ACT_LIST
{
	HUINT32 i = 0;
	HxLOG_Print("--------------- ca pmt state (%x) -------------- \n", pCam->caPmtOkDsc.pucRaw);
	HxLOG_Print("\n\n number of act (%d) \n\n", pCam->ulNumOfAct);
	for(i=0; i<CAS_MAX_NUM_OF_ACTION; i++)
	{
		HxLOG_Print("[%d]-th hAction List (0x%08x)\n", i, pCam->hActList[i]);
	}
}
#endif

	HxLOG_Print("bNeedCaPmtSend (%d), bIsRetry (%d) \n", bNeedCaPmtSend, bIsRetry);

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
			eCplmCmd = local_cimgr_GetCplmCmd(pCiContext->hAction, pCam);
		}

		HxLOG_Print("eCplmCmd = %d\n", eCplmCmd);

		if (pCam->caPmtOkDsc.pucRaw != NULL)
		{
			HxLOG_Print("caPmtOkDsc is not null, make free previous caPmtOkDsc\n");
			OxCAS_Free(pCam->caPmtOkDsc.pucRaw);
			pCam->caPmtOkDsc.pucRaw = NULL;
			pCam->caPmtOkDsc.usRawLen = 0;
		}

		nResult = local_cimgr_GetCaPmt(pCam, pCiContext, eCplmCmd, CI_CPCI_OK_DSC, &pCam->caPmtOkDsc);
		if (nResult != ERR_OK)
		{
			HxLOG_Error("\n");
			return ERR_FAIL;
		}

#if defined(_Support_Not_Selected_CA_PMT_)
		if (pCam->caPmtNotSel.pucRaw != NULL)
		{
			HxLOG_Print("caPmtNotSel is not null, make free previous caPmtNotSel\n");
			OxCAS_Free(pCam->caPmtNotSel.pucRaw);
			pCam->caPmtNotSel.pucRaw = NULL;
			pCam->caPmtNotSel.usRawLen = 0;
		}

		nResult = local_cimgr_GetCaPmt(pCam, pCiContext, eCplmCmd, CI_CPCI_NOT_SELECTED, &pCam->caPmtNotSel);
		if (nResult != ERR_OK)
		{
			HxLOG_Error("\n");
			return ERR_FAIL;
		}
#endif

		if (pCam->caPmtOkDsc.pucRaw != NULL)
		{
			if (CI_SendCaPmt(usSlotId, pCam->caPmtOkDsc.pucRaw, pCam->caPmtOkDsc.usRawLen) != ERR_OK)
			{
				HxLOG_Error("\n");
//				return ERR_FAIL;
			}
		}

	  	HxLOG_Print("CI_SendCaPmt : slot[%d], pucRaw[0x%x], len[%d]\n", usSlotId, (HUINT32)pCam->caPmtOkDsc.pucRaw, pCam->caPmtOkDsc.usRawLen);
	}

	/* CA PMT �ۼ� �Ϸ�. */
	pCiContext->eState = eCiMgrState_StartCaPmt;

#if defined(CONFIG_MW_CI_PLUS)
{
	if (pCiContext->eChType == eCasChannel_FTA) // FTA ä���� ��� ���� ó���� �ʿ� ����.
	{
#if defined(_USE_SHUNNING_STATE_)
		pCiContext->eShunState = eCiPlus_ShunnedFlag_Disable;
		HxLOG_Info("Shun State flag  Off\n");
#endif
		CI_SetPlusMode(usSlotId, CI_PLUS_FREE_MODE);
		return ERR_OK;
	}

#if 1
	/* ci plus descrambler setting. */
	(void)local_cimgr_ProcDscSetForCiPlus(pCam, pCiContext->ulDemuxId, bIsUpdate, bIsRetry, 1);
#else
	// ret value
	// positive value = CI+ Cam, 0 = Civ1 Cam, -1 = No Cam
	if (CI_IsCiPlusCam(usSlotId) > 0)
	{
		/* ci plus descrambler setting. */
		(void)local_cimgr_ProcDscSetForCiPlus(pCam, pCiContext->ulDemuxId, bIsUpdate, bIsRetry, 1);
	}
	else
	{
		HxLOG_Print("Do not dsc setting for ci+. usSlotId (%d), CI_IsCiPlusCam (%d) \n", usSlotId, CI_IsCiPlusCam(usSlotId));
	}
#endif

	if (pCiContext->eActType == eCAS_ActionType_Live || pCiContext->eActType == eCAS_ActionType_Playback)
	{
		CASMGR_GetSiTableState(pCiContext->hAction, eSI_TableType_SDT_ACT, &pCiContext->eSdtTblState);

	  	HxLOG_Print("NumOfDsc (%d), SdtTblState (%s), bSvcShunned (%d)\n",
			pCam->ulNumOfDsc[pCiContext->ulDemuxId], svc_cas_DbgMakeStrSiTblState(pCiContext->eSdtTblState), pCiContext->bSvcShunned);

#if defined(_USE_SHUNNING_STATE_)
		// ���� shun condition check�� ������ ���⿡�� �Ѵ�
		// SDT change -> PMT change -> Cam instance update -> Start descramble �� ���� ��� shunning check�� �ȴ�
		if (pCiContext->eShunState == eCiPlus_ShunnedFlag_None)
		{
			CAS_CiDscInfo_t *pDscInfo = NULL;
			HBOOL bIsNeedShun = FALSE;

			if(local_cimgr_GetCiDescInfo(pCiContext, &pDscInfo) != ERR_OK)
			{
				HxLOG_Print("get description info failed\n");
			}
			else
			{
				if(local_cimgr_ProcCheckShunCondition(usSlotId, pDscInfo, &bIsNeedShun) != ERR_OK)
				{
					HxLOG_Error("\n");
				}

				if (bIsNeedShun == TRUE)
				{
					pCiContext->eShunState = eCiPlus_ShunnedFlag_Enable;
					HxLOG_Info("Shun State flag  On \n");

					CI_SetPlusMode(usSlotId, CI_PLUS_PROTECTED_MODE);
				}
				else
				{
					pCiContext->eShunState = eCiPlus_ShunnedFlag_Disable;
					HxLOG_Info("Shun State flag  Off \n");

					CI_SetPlusMode(usSlotId, CI_PLUS_FREE_MODE);
				}
			}
		}

		if((pCiContext->eSdtTblState == eCASSI_Table_Received || pCiContext->eSdtTblState == eCASSI_Table_Timeout || pCiContext->eSdtTblState == eCASSI_Table_Running)
			&& (pCiContext->eShunState == eCiPlus_ShunnedFlag_Disable))
#else
		if((pCiContext->eSdtTblState == eCASSI_Table_Received || pCiContext->eSdtTblState == eCASSI_Table_Timeout || pCiContext->eSdtTblState == eCASSI_Table_Running)
			&& (pCiContext->bSvcShunned == FALSE))
#endif
		{
			svc_cas_DevCiSetTsPath(usSlotId, eCAS_DEV_CI_PASSTHROUGH);
		}
		else
		{
			/**
			  *		SDT 7 day caching
			**/
			DbSvc_Info_t		stSvcInfo;
			UNIXTIME			ulCurrentTime = 0;
			CAS_CiDscInfo_t	stCachedDscInfo;
			HBOOL			bNeedShun = FALSE;

			// ���� SDT ACT�� ���� ����. DB�� ��������
			nResult = DB_SVC_GetServiceInfo(pCiContext->hSvc, &stSvcInfo);
			if(nResult != ERR_OK)
			{
				HxLOG_Error("DB_SVC_GetServiceInfo() Fail !!\n");
			}
			else
			{
				// ���� �ð��� ��������
				nResult = (HERROR)VK_CLOCK_GetTime((unsigned long*)&ulCurrentTime);
				if((nResult != VK_OK) || (ulCurrentTime <= UNIX_FIRST_DAY))
				{
					HxLOG_Info("VK_CLOCK_GetTime() Fail !! Set unix first day... \n");
				}
				else
				{

					// time stamp�� �ð��� valid �ϴٸ�
					if (DbSvc_GetCiTimeStamp(&stSvcInfo) + SECONDS_PER_WEEK >= ulCurrentTime )
					{
						// ���� �˻翡 �ʿ��� �������� ä����
						HxSTD_memset(&stCachedDscInfo, 0x00, sizeof(CAS_CiDscInfo_t));

						stCachedDscInfo.usSvcId = pCiContext->usSvcId;
						if (DbSvc_GetFreeCiModeFlag(&stSvcInfo))
						{
							stCachedDscInfo.bFreeCiFlag = TRUE;
							stCachedDscInfo.bCiDscFlag = TRUE;
						}
						else
						{
							stCachedDscInfo.bFreeCiFlag = FALSE;
							stCachedDscInfo.bCiDscFlag = FALSE;
						}

						stCachedDscInfo.ucNumOfBrandId = DbSvc_GetCiNumOfEntries(&stSvcInfo);
						if (stCachedDscInfo.ucNumOfBrandId == 0)
						{
							stCachedDscInfo.bMatchBrandFlag = FALSE;
						}
						else
						{
							stCachedDscInfo.bMatchBrandFlag = TRUE;
							HxSTD_memcpy(stCachedDscInfo.usCiBrandId, DbSvc_GetCamBrandId(&stSvcInfo), sizeof(stCachedDscInfo.usCiBrandId));
						}

						HxLOG_Info("Cached SDT Data :: usSvcId(0x%X) bFreeCiFlag(%d) bCiDscFlag(%d) #brandID(%d) matchFlag(%d) \n",
												stCachedDscInfo.usSvcId, stCachedDscInfo.bFreeCiFlag,  stCachedDscInfo.bCiDscFlag, stCachedDscInfo.ucNumOfBrandId, stCachedDscInfo.bMatchBrandFlag );

						// �ٽ� shun condition�� �˻��� ����.
						nResult = local_cimgr_ProcCheckShunCondition(usSlotId, &stCachedDscInfo, &bNeedShun);
						if (nResult != ERR_OK)
						{
							HxLOG_Error("local_cimgr_ProcCheckShunCondition() Fail !!\n");
							return ERR_FAIL;
						}

						// shun condition�� FALSE��� Ǯ��~
						if (bNeedShun != TRUE)
						{
							svc_cas_DevCiSetTsPath(usSlotId, eCAS_DEV_CI_PASSTHROUGH);
#if defined(_USE_SHUNNING_STATE_)
							pCiContext->eShunState = eCiPlus_ShunnedFlag_Disable;
							HxLOG_Info("Shun State flag  Off\n");
#endif
							CI_SetPlusMode(usSlotId, CI_PLUS_FREE_MODE);

#if defined(CONFIG_DEBUG)
							HxLOG_Print(" [CI+] Use cached SDT data. curTime(0x%X) savedTimeStamp(%d) \n", ulCurrentTime, DbSvc_GetCiTimeStamp(&stSvcInfo));
#endif
						}
					}
					else
					{
						HxLOG_Info("Invalid Timestamp. Now(0x%X) DB(0x%X) \n", ulCurrentTime, DbSvc_GetCiTimeStamp(&stSvcInfo));
					}
				}
			}
		}
	}
	else
	{
		svc_cas_DevCiSetTsPath(usSlotId, eCAS_DEV_CI_PASSTHROUGH);
	}
}
#else
	svc_cas_DevCiSetTsPath(usSlotId, eCAS_DEV_CI_PASSTHROUGH);
#endif // #if defined(CONFIG_MW_CI_PLUS)

	if (pCiContext->eActType == eCAS_ActionType_Live && pCiContext->ulRetryTimerId == INVALID_TIMER_ID)
	{
		HxLOG_Print("ulRetryTimerId is (%lu)\n", pCiContext->ulRetryTimerId);
		local_cimgr_StartRetrySvc(pCiContext);
	}

	return ERR_OK;
}

#if defined(_Send_Not_Selected_CA_PMT_on_Zapping_or_RetrySvc_)
// caPmtStop �� ������ ���� ��� �Ϻ� ķ������ ��Ǯ���� CI ä�η� ���ٰ� ���� TP�� free ä�η� ä�� ��ȯ�� CI "No access" MMI â �ߴ� ������ �߻��Ѵ�.
// �׷��Ƿ� Ȯ���� caPmtStop �� ������ �ʾƵ� �Ǵ� ķ�̳� ������ ������ ����� ķ�� ���ؼ��� caPmtStop �� ������ �ʵ��� ó���ϵ��� �Ѵ�...
// -> �̰��� _Support_CA_PMT_in_Clear_ �����ϸ� �ڿ� �ذ�� ��...
STATIC HERROR local_cimgr_IsNeedNotSelectCmd(CI_CamInst_t *pCam, HUINT8 *pucIsNeed)
{
	if (pCam == NULL)
	{
		return ERR_CI_INVALID_CAM_INSTANCE;
	}

	/* <TODO_CI> : Ư�� ķ�� ���� ���� ó���� �ʿ��ϴٸ�...
	   Application Info �� application_manufacturer / manufacturer_code / menu_string_length ��
	   CA Info �� CA_system_id �� ��� üũ�ϵ��� ����.
	   ������ ķ�� ������ ���� �ʵ���... */
	if ((pCam->usManufacturer != 0) && (pCam->ausCaSysId != NULL))
	{
		if (((pCam->usManufacturer >> 8) & 0x00ff) == CRYPTOWORKS_CAM_MANUFACTURER)
		{
			/******************************************/
			/* COMBO-9000������ CRYPTOWORKS CAM�� STOP   */
			/* CA PMT�� ������ �� MMI Message�� �߻��ϴ� �̽���  */
			/* �߻��Ͽ� �Ʒ��� ���� �ּ����� ó����. 2007.08.16       */
			/******************************************/
			*pucIsNeed = 0;
			return ERR_OK;
		}
		else if ((((pCam->usManufacturer >> 8) & 0x00ff) == eCA_SYS_ID_IRDETO) && (((pCam->ausCaSysId[0] >> 8) & 0x00ff) == eCA_SYS_ID_IRDETO))
		{
			/*
			�̵��� ķ�� ��� caPmtStop �� ������ ��ũ���� ����¡�� ������ ����ٰ� �Ͽ�...
			============================================
			NotifyAppInfo
			         nType = 0x0001
			         usManufacturer = 0x06FF
			         usManufacturerCode = 0x06FF
			         szText = Irdeto Access
			         ucIsInitialized = 1
			============================================
			< CI CAM INST >
			usManufacturer : 0x06FF
			ausCaSysId	   : 0x0601 0x0602 0x0603 0x0604 0x0605 0x0606 0x0607 0x0608 0x0621 0x0622 0x0623 0x0624 0x0625 0x0626 0x0627 0x0628
			*/
			*pucIsNeed = 0;
			return ERR_OK;
		}
		/* �ڼ��� ������ �Բ� �߰��� �ּ���...
		else if (...)
		{
			*pucIsNeed = 0;
			return ERR_OK;
		}
		*/
	}

	/*
		���ϴ� Ȯ���� caPmtStop �� �����߸� �ϴ� ķ��...

		*** eCA_SYS_ID_ASTON

		*** �ɶ��� �������� ������ conax ķ�� (caPmtStop �� �Ⱥ����� ��Ǯ���� CI ä�η� ���ٰ� ���� TP�� free ä�η� ä�� ��ȯ�� CI "No access" MMI â �ߴ� ���� �߻���)

			�׳� Conax ķ
			============================================
					   WM_CAS_CI_APP_INFO
			pInfo->nType = 0x0001
			pInfo->usManufacturer = 0xCAFE
			pInfo->usManufacturerCode = 0xBABE
			pInfo->szText = Conax Conditional Access
			pInfo->ucIsInitialized = 0x01
			============================================

			TechniSat ķ
			============================================
					   WM_CAS_CI_APP_INFO
			pInfo->nType = 0x0001
			pInfo->usManufacturer = 0x0000
			pInfo->usManufacturerCode = 0x033D
			pInfo->szText = TSD Crypt Conax
			pInfo->ucIsInitialized = 0x01
			============================================

		*** �ڼ��� ������ �Բ� �߰��� �ּ���...
	*/

	*pucIsNeed = 1; // *pucIsNeed = 0;
	return ERR_OK;
}
#endif

STATIC HERROR local_cimgr_StopDescramble(CI_Context_t *pCiContext, CI_SLOT_ID usSlotId)
{
	CI_CamInst_t *pCam = NULL;
	HERROR nResult = ERR_OK;
	HUINT32 i;

	if (pCiContext == NULL)
	{
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	HxLOG_Print("hAction (0x%08x), usSlotId (%d)\n", pCiContext->hAction, usSlotId);

	pCam = pCiContext->pCam;
	if(pCam == NULL)
	{
		HxLOG_Error("instance is null. (%d)\n", usSlotId);
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

	svc_cas_DevCiSetTsPath(usSlotId, eCAS_DEV_CI_BYPASS);

	/*  Cam �� ���� �� ���....*/
	if (pCam->usCaInfoCnt == 0)
	{
		HxLOG_Print("\n");
		return ERR_OK;
	}

#if defined(_Send_Not_Selected_CA_PMT_on_Zapping_or_RetrySvc_)
{
	HUINT8 ucIsNeed = 0;

	if (pCam->caPmtNotSel.pucRaw == NULL)
	{
		HxLOG_Error("\n");
//		return ERR_CI_INVALID_CAM_INSTANCE; // �Ʒ� CI+ ��ƾ�� ����ǵ��� ���⼭ �������� �ʵ��� �Ѵ�. �׸��� �̷� ��Ȳ�� �߻��ص� ���� �������� �ʾƵ� �ȴ�.
	}
	else
	{
		nResult = local_cimgr_IsNeedNotSelectCmd(pCam, &ucIsNeed);
		if (nResult == ERR_OK)
		{
			if (ucIsNeed)
			{
				if (pCam->caPmtNotSel.pucRaw != NULL)
				{
					#if 0 // #if defined(_CI_DEBUG_) // "ci info" Ŀ�ǵ� �̿��� �� ����
					{
						HUINT32 idx = 0;
						HxLOG_Print("******** caPmtNotSel() *********\n");
						for (idx=0; idx<pCam->caPmtNotSel.usRawLen; idx++)
						{
							HxLOG_Print("0x%02x ", pCam->caPmtNotSel.pucRaw[idx]);
							if ((idx%10) == 9)
								HxLOG_Print("\n");
						}
						HxLOG_Print("\n**************************\n");
					}
					#endif

					if (CI_SendCaPmt(usSlotId, pCam->caPmtNotSel.pucRaw, pCam->caPmtNotSel.usRawLen) != ERR_OK)
					{
						HxLOG_Error("\n");
//						return ERR_FAIL;
					}

					HxLOG_Print("send caPmtNotSel : slot[%d], pucRaw[0x%x], len[%d]\n", usSlotId, (HUINT32)pCam->caPmtNotSel.pucRaw, pCam->caPmtNotSel.usRawLen);
				}
			}
		}
	}
}
#endif

#if defined(CONFIG_MW_CI_PLUS)
{
	HxLOG_Print("slot %d's NumOfDsc = %d, KeyInfoAvailable = %d\n",
		usSlotId, pCam->ulNumOfDsc[pCiContext->ulDemuxId], pCam->bKeyInfoAvailable);

	if (pCiContext->ulDemuxId != CAS_RESOURCE_NONE)
	{
		for (i = 0; i < pCam->ulNumOfDsc[pCiContext->ulDemuxId]; i++)
		{
			nResult = svc_cas_DevDscClose(pCam->stDscInfo[pCiContext->ulDemuxId][i].ulDscId);
			if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

			pCam->stDscInfo[pCiContext->ulDemuxId][i].ulDscId = CAS_RESOURCE_NONE;
			pCam->stDscInfo[pCiContext->ulDemuxId][i].usPid = PID_NULL;
		}
	}
	else
	{
		HxLOG_Critical("\n\n");
	}

	pCam->ulNumOfDsc[pCiContext->ulDemuxId] = 0; // pCam->bKeyInfoAvailable �� Ű ���� ������ �������� �ʵ��� �Ѵ�.

	pCiContext->bSvcShunned = FALSE;
#if defined(_USE_SHUNNING_STATE_)
	pCiContext->eShunState = eCiPlus_ShunnedFlag_None;
#endif

	CI_SetPlusMode(usSlotId, CI_PLUS_FREE_MODE);
}
#endif

	/* ca pmt raw �� sync */
	/* �ʱ�ȭ */
	for(i=0; i<CAS_MAX_NUM_OF_ES; i++)
	{
		pCam->PidInfoForCaPmt[i].usPid = PID_NULL;
		pCam->PidInfoForCaPmt[i].esType = eEsType_None;
	}
	pCam->ulNumOfPidForCaPmt = 0;

	// cam�� ���Ե� service ���� data�� ����.
	if (pCam->caPmtOkDsc.pucRaw != NULL)
	{
		OxCAS_Free(pCam->caPmtOkDsc.pucRaw);
		pCam->caPmtOkDsc.pucRaw = NULL;
		pCam->caPmtOkDsc.usRawLen = 0;
	}
#if defined(_Support_Not_Selected_CA_PMT_)
	if (pCam->caPmtNotSel.pucRaw != NULL)
	{
		OxCAS_Free(pCam->caPmtNotSel.pucRaw);
		pCam->caPmtNotSel.pucRaw = NULL;
		pCam->caPmtNotSel.usRawLen = 0;
	}
#endif

	// disconnect TS Input
	//KJW - 2004.01.21, TS input path will be controlled by DDI_PATH
	//DDI_CI_SetTsInPath(usSlotId, 0); -> if need, use PAL_CI_SetSrcPath()

	pCiContext->bSendCaPmt = FALSE;

	return nResult;
}

////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_REQ_EVENTS_OR_CB___
////////////////////////////////////////////////////////////

/* �Ʒ� �Լ��� Memory Copy�� Crash�� ���Ͽ� size�� parameter�� �ֵ��� ������ �ڵ� ��. thyou. 2007.08.23 */
STATIC HINT32 local_cimgr_SendEvent(CI_RCV_EVENT eEvt, void *pvEvtParam, unsigned long ulSize)
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

STATIC void local_cimgr_ReqStartSession(Handle_t hAction, Handle_t hSvc, SvcCas_ActionType_e eActType)
{
	CI_ADD_SVC_PARAM param;
	HERROR nResult = ERR_FAIL;

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("invalid handle\n");
		return;
	}

	param.hAction = hAction;
	param.hSvc = hSvc; // pvr�� ��� hSvc�� HANDLE_NULL ��.
	param.eActType = eActType;

	HxLOG_Print("hAction(0x%08x), hSvc(0x%08x), eActType(%s)\n",
		(HUINT32)hAction, (HUINT32)hSvc, svc_cas_DbgMakeStrActionType(eActType));

	nResult = local_cimgr_SendEvent(CI_START, (void *)&param, (unsigned long)sizeof(CI_ADD_SVC_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

STATIC void local_cimgr_ReqStopSession(Handle_t hAction, Handle_t hNextSvc) // TODO : Stop/Delete�� ����...Manage Instance�� Sequence �� ����.
{
	CI_DELETE_SVC_PARAM stopSvc;
	HERROR nResult = ERR_FAIL;

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("invalid handle\n");
		return;
	}

	HxLOG_Print("hAction(0x%08x), hNextSvc(0x%08x)\n",
		(HUINT32)hAction, (HUINT32)hNextSvc);

	stopSvc.hAction = hAction;
	stopSvc.hNextSvc = hNextSvc;

	nResult = local_cimgr_SendEvent(CI_STOP, (void *)&stopSvc, (unsigned long)sizeof(CI_DELETE_SVC_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

STATIC void local_cimgr_ReqPauseSession(Handle_t hAction)
{
	CI_COMMAND_PARAM cmd;
	HERROR nResult = ERR_FAIL;

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("invalid handle\n");
		return;
	}

	cmd.hAction = hAction;

	nResult = local_cimgr_SendEvent(CI_PAUSE, (void *)&cmd, (unsigned long)sizeof(CI_COMMAND_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

STATIC void local_cimgr_ReqResumeSession(Handle_t hAction)
{
	CI_COMMAND_PARAM cmd;
	HERROR nResult = ERR_FAIL;

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("invalid handle\n");
		return;
	}

	cmd.hAction = hAction;

	nResult = local_cimgr_SendEvent(CI_RESUME, (void *)&cmd, (unsigned long)sizeof(CI_COMMAND_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

STATIC void local_cimgr_CallbackRetrySvc(unsigned long ulTimerId, void *pvParam) // TODO : Retry Scheme ����.
{
	CI_RETRY_SVC_PARAM *pRetrySvc;

	/* Not used */
	ulTimerId = 0;

	pRetrySvc = (CI_RETRY_SVC_PARAM *)pvParam;

	HxLOG_Print("hAction[0x%08x], hSvc[0x%08x]...\n", pRetrySvc->hAction, pRetrySvc->hSvc);

	local_cimgr_SendEvent(CI_RETRY, (void *)pRetrySvc, (unsigned long)sizeof(CI_RETRY_SVC_PARAM));

	return;
}

STATIC void local_cimgr_ReqUpdatePmt(Handle_t hAction, HUINT8 *pucPmt)
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

	pmtChanged.hAction = hAction;
	/*
		pmt raw�� ����ϴ� ���, pmt pointer ���� ������ Length Ȯ���ؼ� mem alloc �� copy �ϵ��� �Ѵ�.

		����� ������� ����.
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

	HxLOG_Print("hAction (0x%08x)\n", hAction);

	nResult = local_cimgr_SendEvent(CI_PMT_CHANGED, (void *)&pmtChanged, (unsigned long)sizeof(CI_PMT_CHANGED_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

#if defined(CONFIG_MW_CI_PLUS)

// SDT Raw�� ���� �޾ƶ�.
STATIC void local_cimgr_ReqUpdateSdt(Handle_t hAction, HUINT16 usUniqueId, HUINT8 *pSdtRaw)
{
	CI_SDT_CHANGED_PARAM param;
	HERROR nResult = ERR_FAIL;
	HUINT32 ulLen = 0;

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("invalid handle\n");
		return;
	}

	if(usUniqueId == 0)
	{
		HxLOG_Error("invalid UniqueId\n");
		return;
	}

	HxLOG_Print("ActionHandle(0x%08x), UniqueId(%x)\n", hAction, usUniqueId);

	param.hAction = hAction;
	param.usUniqueId = usUniqueId;

	ulLen = get12bit(pSdtRaw+1) + 3;
	param.pucTbl = (HUINT8 *)OxCAS_Malloc(ulLen);
	if (param.pucTbl == NULL )
	{
		HxLOG_Error("Action:0x%x, can not allocate memory. \n", hAction);
		return;
	}

	HxSTD_memcpy(param.pucTbl, pSdtRaw, ulLen);

	nResult = local_cimgr_SendEvent(CI_SDT_CHANGED, (void *)&param, (unsigned long)sizeof(CI_SDT_CHANGED_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}
#endif

STATIC void local_cimgr_ReqManageInstance(
												Handle_t hAction,				/* Action id */
												HUINT32 ulCaInstanceId,			/* Instatnce id�� �����ϴ� �ǵ��� CI Slot ������ Instance Id�� �ٸ��� */
												HBOOL bConnect)					/* TRUE : Descramble �õ� ��û, FALSE : Descramble ���� ��û */
{
	CI_MANAGE_CA_INST_PARAM param;
	HERROR nResult = ERR_FAIL;

	if(hAction == HANDLE_NULL)
	{
		HxLOG_Error("invalid handle\n");
		return;
	}

	HxLOG_Print("hAction(0x%08x), CaInstanceId(0x%x), connect(%d)\n",
		(HUINT32)hAction, (HUINT32)ulCaInstanceId, bConnect);

	param.hAction = hAction;
	param.hCasDevice = ulCaInstanceId;
	param.bConnect = bConnect;

	nResult = local_cimgr_SendEvent(CI_MANAGE_CA_INST, (void *)&param, (unsigned long)sizeof(CI_MANAGE_CA_INST_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

STATIC HINT32 local_cimgr_ReqUpdateCaInst(CI_SLOT_ID usSlotId, unsigned short usCaInfoCnt, unsigned short *ausCaSysId)
{
	CI_UPDATE_CA_INST_PARAM updateCaInst;
	HERROR nResult = ERR_FAIL;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	updateCaInst.usSlotId = usSlotId;

	if (usCaInfoCnt == 0) // CA instance �� release �� ��Ȳ (��, CAS ���� ����� CiCas_DeleteCaInst() ����� ��)
	{
		updateCaInst.ausCaSysId = NULL;
		updateCaInst.usCaInfoCnt = 0;
	}
	else
	{
		if (ausCaSysId == NULL) // CA instance �� release �� ��Ȳ�� ������ �Ÿ��� ������ ���⼭ �̷� ������ �߻��ϸ� ���...
		{
			HxLOG_Error("\n");
			return ERR_FAIL;
		}

		updateCaInst.ausCaSysId = (HUINT16 *)OxCAS_Malloc(sizeof(HUINT16) * usCaInfoCnt); // local_cimgr_ProcUpdateCaInst()���� free.
		if (updateCaInst.ausCaSysId == NULL)
		{
			HxLOG_Error("mem alloc failure\n");
			return ERR_FAIL;
		}
		HxSTD_memcpy(updateCaInst.ausCaSysId, ausCaSysId, sizeof(HUINT16) * usCaInfoCnt);

		updateCaInst.usCaInfoCnt = usCaInfoCnt;
	}

	nResult = local_cimgr_SendEvent(CI_UPDATE_CA_INST, (void *)&updateCaInst, (unsigned long)sizeof(CI_UPDATE_CA_INST_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;
}

/* �� �ʿ� */
STATIC void local_cimgr_CallbackDelayedAddCaInst(unsigned long ulTimerId, void *pvParam)
{
	CI_SLOT_ID 					usSlotId;
	CI_UPDATE_CA_INST_PARAM		updateCaInst;
	HERROR nResult = ERR_FAIL;

	/* Not used */
	ulTimerId = 0;

	usSlotId = *((CI_SLOT_ID *)pvParam);

	HxLOG_Print("SlotId[%d]...\n", usSlotId);

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return;
	}

	updateCaInst.usSlotId = usSlotId;

	nResult = local_cimgr_SendEvent(CI_ADD_DELAYED_CA_INST, (void *)&updateCaInst, (unsigned long)sizeof(CI_UPDATE_CA_INST_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}

#if defined(CI_PLUS_PVR_ENABLE)
STATIC HINT32 local_cimgr_ReqUpdateCaPvrInst(CI_SLOT_ID usSlotId, unsigned short usCaPvrInfoCnt, unsigned short *ausCaSysId, CI_CA_PVR_REC_MODE *aucCaRecMode)
{
	CI_UPDATE_CA_PVR_INST_PARAM updateCaPvrInst;
	HERROR nResult = ERR_FAIL;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

	updateCaPvrInst.usSlotId = usSlotId;

	if (usCaPvrInfoCnt == 0) // CA PVR instance �� release �� ��Ȳ (��, CA PVR ���� ����� CiCaPvr_DeleteCaPvrInst() ����� ��)
	{
		updateCaPvrInst.ausCaPvrCaSysId = NULL;
		updateCaPvrInst.aucCaRecMode = NULL;
		updateCaPvrInst.usCaPvrInfoCnt = 0;
	}
	else
	{
		if (ausCaSysId == NULL || aucCaRecMode == NULL) // CA PVR instance �� release �� ��Ȳ�� ������ �Ÿ��� ������ ���⼭ �̷� ������ �߻��ϸ� ���...
		{
			HxLOG_Error("\n");
			return ERR_FAIL;
		}

		updateCaPvrInst.ausCaPvrCaSysId = (HUINT16 *)OxCAS_Malloc(sizeof(HUINT16) * usCaPvrInfoCnt); // local_cimgr_ProcUpdateCaPvrInst()���� free.
		if (updateCaPvrInst.ausCaPvrCaSysId == NULL)
		{
			HxLOG_Error("mem alloc failure for ausCaPvrCaSysId\n");
			return ERR_FAIL;
		}
		HxSTD_memcpy(updateCaPvrInst.ausCaPvrCaSysId, ausCaSysId, sizeof(HUINT16) * usCaPvrInfoCnt);

		updateCaPvrInst.aucCaRecMode = (CI_CA_PVR_REC_MODE *)OxCAS_Malloc(sizeof(CI_CA_PVR_REC_MODE) * usCaPvrInfoCnt); // local_cimgr_ProcUpdateCaPvrInst()���� free.
		if (updateCaPvrInst.aucCaRecMode == NULL)
		{
			HxLOG_Error("mem alloc failure for aucCaRecMode\n");
			return ERR_FAIL;
		}
		HxSTD_memcpy(updateCaPvrInst.aucCaRecMode, aucCaRecMode, sizeof(CI_CA_PVR_REC_MODE) * usCaPvrInfoCnt);

		updateCaPvrInst.usCaPvrInfoCnt = usCaPvrInfoCnt;
	}

	nResult = local_cimgr_SendEvent(CI_UPDATE_CA_PVR_INST, (void *)&updateCaPvrInst, (unsigned long)sizeof(CI_UPDATE_CA_PVR_INST_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;
}
#endif

STATIC HINT32 local_cimgr_ReqUpdateCamState(CI_SLOT_ID usSlotId, CI_CAM_STATE eCamState)
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

	nResult = local_cimgr_SendEvent(CI_UPDATE_CAM_STATE, (void *)&updateCamSt, (unsigned long)sizeof(CI_UPDATE_CAM_STATE_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;
}

#if 1 // #if defined(CONFIG_MW_CI_PLUS_PVR) : ������ CA PVR ������ ���� �߰��� �������� Multiple CA application selection procedure ���� ���� �ٸ� �뵵�� Ȯ���ؼ� �� ������ �����Ƿ� �Ϲ������� ������ ��.
STATIC HINT32 local_cimgr_ReqUpdateCasState(CI_SLOT_ID usSlotId, CI_CAS_STATE eCasState)
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

	nResult = local_cimgr_SendEvent(CI_UPDATE_CAS_STATE, (void *)&updateCasSt, (unsigned long)sizeof(CI_UPDATE_CAS_STATE_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;
}
#endif

#if defined(CONFIG_MW_CI_PLUS)

// �� �ݹ� �Լ����� ���� ��ũ������ SetKey �� �ϰ� �Ǹ� racing condition �� ķ�� confirm �� �ִ� ���� �ʾ��� ������ �߻��� �� �����Ƿ� CI mgr Ÿ��ũ���� ó���ϵ��� �޽��� �����ؾ� �Ѵ�!
STATIC HINT32 local_cimgr_ReqSetKey(CI_SLOT_ID usSlotId, CIPLUS_KeyInfo_t *pstKeyInfo)
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

	nResult = local_cimgr_SendEvent(CI_PLUS_KEY_CHANGED, (void *)&param, (unsigned long)sizeof(CI_PLUS_KEY_CHANGE_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;
}

// �� �ݹ� �Լ����� ���� URI control �� �ϰ� �Ǹ� ķ�� confirm �� �ִ� ���� �ʾ��� ������ �߻��� �� �����Ƿ� CI mgr Ÿ��ũ���� ó���ϵ��� �޽��� �����ؾ� �Ѵ�!
STATIC HINT32 local_cimgr_ReqUriControl(CI_SLOT_ID usSlotId, CIPLUS_UriInfo_t *pstUriInfo)
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

	nResult = local_cimgr_SendEvent(CI_PLUS_URI_CONTROL, (void *)&param, (unsigned long)sizeof(CI_PLUS_URI_CONTROL_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;
}

// �� �ݹ� �Լ����� ���� SRM control �� �ϰ� �Ǹ� ķ�� confirm �� �ִ� ���� �ʾ��� ������ �߻��� �� �����Ƿ� CI mgr Ÿ��ũ���� ó���ϵ��� �޽��� �����ؾ� �Ѵ�!
STATIC HINT32 local_cimgr_ReqSrmControl(CI_SLOT_ID usSlotId, unsigned char *pucSrmData, unsigned short usSrmDataLen)
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
	param.pucSrmData = (unsigned char *)OxCAS_Malloc(usSrmDataLen); // local_cimgr_ProcSrmControl()���� free.
	HxSTD_memcpy(param.pucSrmData, pucSrmData, usSrmDataLen);

	nResult = local_cimgr_SendEvent(CI_PLUS_SRM_CONTROL, (void *)&param, (unsigned long)sizeof(CI_PLUS_SRM_CONTROL_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;
}

STATIC HINT32 local_cimgr_ReqHostTune(CI_SLOT_ID usSlotId, SvcCas_CiPlusTuneInfo_t *pstTuneInfo)
{
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

	// ���⼭ �ٷ� Event ����.

	nResult = CASMGR_RequestTuneByCiPlus((HUINT32)usSlotId, pstTuneInfo);
	if(nResult!=ERR_OK)
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HINT32 local_cimgr_ReqPidReplace(CI_SLOT_ID usSlotId, CIPLUS_PidInfo_t *pstPidInfo) // <TODO_CI_PLUS> : ���� �������� �ʾ���. ���� Ȯ�� �ʿ� !!
{
	CI_Context_t *pCiContext = NULL;
	CI_CamInst_t *pCam = NULL;
	HBOOL bFound = FALSE;
	HERROR nResult = ERR_FAIL;
	HUINT32 i = 0;

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

	pCam = &s_astCiCam[usSlotId];
	if(pCam == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	HxLOG_Print("mode (%d), OrgPid (%x), TarPid (%x)\n", pstPidInfo->eReplaceMode, pstPidInfo->usOriginalPid, pstPidInfo->usTargetPId);

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
				HxLOG_Print("Original Pid(%x) Found\n", pstPidInfo->usOriginalPid);
				bFound = TRUE;
				pCiContext->stProgInfo.aEsInfo[i].usEsPid = pstPidInfo->usTargetPId;
			}
		}
		else if(eCiPlus_Replace_Stop == pstPidInfo->eReplaceMode || eCiPlus_Replace_Cancel == pstPidInfo->eReplaceMode)
		{
			if(pstPidInfo->usTargetPId == pCiContext->stProgInfo.aEsInfo[i].usEsPid)
			{
				HxLOG_Print("usTargetPId(%x) Found\n", pstPidInfo->usTargetPId);
				bFound = TRUE;
				pCiContext->stProgInfo.aEsInfo[i].usEsPid = pstPidInfo->usOriginalPid;
			}
		}
		else
		{
			HxLOG_Error("Unknown Pid Replacement mode (%d)\n", pstPidInfo->eReplaceMode);
		}
	}

	if(bFound == TRUE)
	{
		nResult = local_cimgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 1, 0);
		if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
	}

	return ERR_OK;
}

STATIC int local_cimgr_ReqUpdateCcResource(CI_SLOT_ID usSlotId, CIPLUS_CcReportInfo_t *pstCcReportInfo)
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

	if (pstCcReportInfo->eCcReport != eCiPlus_CcReport_AuthenticationDoneAndStartSacEstablishment
		&& pstCcReportInfo->eCcReport != eCiPlus_CcReport_UriVersionNegotiationDone)
	{
		// �̵��� UI ó���� ���� �뵵�� ���⼭�� ó���� ���� ����.
		return ERR_OK;
	}

	HxLOG_Print("eCcReport (%d)	\n", pstCcReportInfo->eCcReport);

	updateCcInst.usSlotId = usSlotId;

	nResult = local_cimgr_SendEvent(CI_UPDATE_CC, (void *)&updateCcInst, (unsigned long)sizeof(CI_UPDATE_CA_INST_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return ERR_OK;
}

#if defined(CONFIG_MW_CI_PLUS_CC_V_2)

STATIC SvcCas_Context_t *local_cimgr_GetActionContextByCiSlotId(HUINT16 usSlotId)
{
	HUINT32 i = 0;
	SvcCas_Context_t *pContext = NULL;
	SvcCas_DevInstance_t *pInst = NULL;

	for(i=0; i<CAS_MAX_NUM_OF_ACTION; i++)
	{
		pContext = CASMAIN_GetActionContextByActionId(i);
		if(pContext == NULL)
		{
			continue;
		}

		if(pContext->hAction == HANDLE_NULL || pContext->state == eCAS_State_Idle || pContext->ulCasInstanceId == HANDLE_NULL)
		{
			continue;
		}

		pInst = CASINST_GetCasDeviceInstance(pContext->ulCasInstanceId);
		if(pInst == NULL)
		{
			continue;
		}

		if(pInst->eCasInstType == eCasInst_CI && pInst->bHaveDevice[usSlotId] == TRUE)
		{
			HxLOG_Print("find context. by ci slot id \n");
			return pContext;
		}
	}

	return NULL;
}


// TODO: �Ʒ� �Լ����� ���� ������Ʈ�� �ϴµ�, WatchTV �� Playback �ó��������� AV blanking �� ��Ʈ�� �ϴ� �κ� �߰� ���� �ʿ�
STATIC int local_cimgr_ReqUpdatePinStatus(CI_SLOT_ID usSlotId, CIPLUS_CcPinStatusInfo_t *pstCcPinStatusInfo)
{
	CAS_PinStatus_t ePinStatus;
	SvcCas_MsgType_e	eMsgType = eCAS_MSG_TYPE_Command;
	DxCAS_GroupId_e	eCasId = eDxCAS_GROUPID_CAM;
	SvcCas_ShowType_e 	eShowType = eCasShowType_None;
	SvcCas_Context_t 	*pContext = NULL;
	Handle_t 		hAction = HANDLE_NULL;
	HERROR 			nResult = ERR_FAIL;

	pContext = local_cimgr_GetActionContextByCiSlotId(usSlotId);

	if (pContext != NULL)
	{
		hAction = pContext->hAction;
		HxLOG_Print("hAction value of CI :: [%d]\n", hAction);
	}


	if (usSlotId >= PAL_CI_GetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d) \n", usSlotId);
		return ERR_FAIL;
	}

	if (pstCcPinStatusInfo == NULL)
	{
		HxLOG_Assert(0);
		return ERR_FAIL;
	}

	HxLOG_Print("ePinStatus (%d)	\n", pstCcPinStatusInfo->ePinStatus);

	switch (pstCcPinStatusInfo->ePinStatus)
	{
		case CI_PIN_STATUS_BAD_PIN :
			ePinStatus = eCasPinStatus_BAD_PIN;
			eShowType = eCasShowType_None;
			break;
		case CI_PIN_STATUS_CICAM_BUSY :
			ePinStatus = eCasPinStatus_CICAM_BUSY;
			eShowType = eCasShowType_None;
			break;
		case CI_PIN_STATUS_PIN_CORRECT :
			ePinStatus = eCasPinStatus_PIN_CORRECT;
			eShowType = eCasShowType_Both;
			break;
		case CI_PIN_STATUS_PIN_UNCONFIRMED :
			ePinStatus = eCasPinStatus_PIN_UNCONFIRMED;
			eShowType = eCasShowType_None;
			break;
		case CI_PIN_STATUS_VIDEO_BLAKING_NOT_REQUIRED :
			ePinStatus = eCasPinStatus_VIDEO_BLAKING_NOT_REQUIRED;
			eShowType = eCasShowType_Both;
			break;
		case CI_PIN_STATUS_CONTENT_CA_SCRAMBLED :
			ePinStatus = eCasPinStatus_CONTENT_CA_SCRAMBLED;
			eShowType = eCasShowType_None;
			break;
		default:
			break;
	}

	if (CASMGR_UpdateCasMessage(hAction, eMsgType, eCasId, (void*)&ePinStatus, eShowType) != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;


	/*
	nResult = CASMGR_RequestUpdatePinStatus((HUINT16)usSlotId, pstCcPinStatusInfo);
	CiMgrAssert(nResult == ERR_OK);

	return ERR_OK;
	*/
}
#endif // #if defined(CONFIG_MW_CI_PLUS_CC_V_2)

#endif // #if defined(CONFIG_MW_CI_PLUS)

#if 0
STATIC void local_cimgr_ReqStreamStateChanged(HUINT32 ulDevId, HUINT16 usPid, PalVideo_Event_e eEvt)
{
	CI_STREAM_STATE_CHANGED_PARAM param;
	HERROR nResult = ERR_FAIL;

	if(usPid >= PID_NULL)
	{
		HxLOG_Error("pid is null (0x%x)\n", usPid);
		return;
	}

	HxLOG_Print("event is (%s)\n", PAL_VIDEO_MakeStrDeviceEvt(eEvt));

	if(eEvt == eVIDEO_EVENT_FRAME_START)
	{
		param.eStreamState = CI_STREAM_PLAYED;
	}
	else if(eEvt == eVIDEO_EVENT_UNDERRUN)
	{
		param.eStreamState = CI_STREAM_SCRAMBLED;
	}
	else
	{
		return;
	}

	param.ulDevId = ulDevId;
	param.usPid = usPid;

	nResult = local_cimgr_SendEvent(CI_STREAM_STATE_CHANGED, (void *)&param, (unsigned long)sizeof(CI_MANAGE_CA_INST_PARAM));
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	return;
}
#endif

////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_PROC_EVENTS_OR_CB___
////////////////////////////////////////////////////////////


STATIC HERROR local_cimgr_ProcStart(CI_ADD_SVC_PARAM *pStart)
{
	CI_Context_t *pCiContext = NULL;
	Handle_t hAction = HANDLE_NULL, hSvc = HANDLE_NULL;
	HERROR nResult = ERR_FAIL;

	if (pStart == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	hAction = pStart->hAction;
	hSvc = pStart->hSvc; // pvr�� ��� hSvc�� NULL��.

	HxLOG_Info("hAction(0x%08x) hSvc(0x%08x) (%s)\n",
		hAction, hSvc, svc_cas_DbgMakeStrActionType(pStart->eActType));

	if(hAction == (Handle_t)NULL)
	{
		HxLOG_Error("invalid handle\n");
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_MM_PVR)
	if(pStart->eActType != eCAS_ActionType_Playback && pStart->eActType != eCAS_ActionType_Tsr_Play)
	{
		if (hSvc == (Handle_t)NULL)
		{
			HxLOG_Error("invalid handle\n");
			return ERR_FAIL;
		}
	}
#endif

	pCiContext = local_cimgr_GetContextByActionId(hAction);
	if(pCiContext == NULL)
	{
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	pCiContext->hAction = hAction;
	pCiContext->hSvc = hSvc;
	pCiContext->eActType = pStart->eActType;
	pCiContext->eState = eCiMgrState_Started;
	pCiContext->ulRetryTimerId = INVALID_TIMER_ID;

	nResult = local_cimgr_GetResourceId(pCiContext);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

#if defined(_LIMIT_CI_RETRY_COUNT_)
	pCiContext->ucRetryCnt = 0;
#endif

#if defined(CONFIG_MW_CI_PLUS)
{
	DbSvc_TsInfo_t TsInfo;

	if(pStart->eActType != eCAS_ActionType_Playback && pStart->eActType != eCAS_ActionType_Tsr_Play)
	{
		DB_SVC_GetServiceIdFromSvcHandle(hSvc, &pCiContext->usSvcId);
		DB_SVC_GetUniqueIdFromSvcHandle(hSvc, &pCiContext->usUniqueId);
		DB_SVC_GetTsInfoByServiceHandle(hSvc, &TsInfo);

		pCiContext->usOnId = TsInfo.usOnId;
		pCiContext->usTsId = TsInfo.usTsId;
	}
	else
	{
#if defined(CONFIG_MW_MM_PVR)
		SvcMeta_Service_t pvr_info;

		/* get a service by hAction */
		{
			HUINT32 ulContentId;

			nResult = SVC_META_GetContentIdByhAction( hAction, &ulContentId );
			if( ERR_OK == nResult )
				nResult = SVC_META_GetServiceInfo( ulContentId, &pvr_info );
		}

		if(nResult != ERR_OK)
		{
			HxLOG_Error("MWC_PVR_GetService() failed\n");
			return ERR_FAIL;
		}

		pCiContext->usUniqueId = DB_SVC_GetAllUniqueId();
		pCiContext->usSvcId = pvr_info.usSvcId;

		pCiContext->usOnId = pvr_info.usOnId;
		pCiContext->usTsId = pvr_info.usTsId;
#endif
	}
}
#endif


	return ERR_OK;
}

STATIC HERROR local_cimgr_ProcStop(CI_DELETE_SVC_PARAM *pDeleteSvc)
{
	CI_Context_t *pCiContext = NULL;
	Handle_t hAction = HANDLE_NULL;
	HERROR nResult = ERR_FAIL;

	if (pDeleteSvc == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	hAction = pDeleteSvc->hAction;

	HxLOG_Info("hAction(0x%08x), hNextSvc(0x%08x)\n", hAction, pDeleteSvc->hNextSvc);

	pCiContext = local_cimgr_GetContextByActionId(hAction);
	if (pCiContext == NULL)
	{
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	HxLOG_Info("pCiContext->pCam(%p), pCiContext->eState(%d)\n", pCiContext->pCam, pCiContext->eState);

	if (pCiContext->pCam != NULL)
	{
		// pCiContext->pCam �� �̹� NULL �� �Ǿ� ���⼭ �̰� �Ҹ��� ���Ѵ�. �Ͽ� �ӽ÷� �Ʒ� else ��ƾ�� �߰���.

		if (pCiContext->eState != eCiMgrState_StopCaPmt)
		{
			nResult = local_cimgr_StopDescramble(pCiContext, pCiContext->pCam->usSlotId);
			if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
		}
	}

	nResult = local_cimgr_StopRetrySvc(pCiContext);
	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");


#if defined(CONFIG_MW_CI_PLUS)
	HxLOG_Print(" unique id (%x) \n", pCiContext->usUniqueId);
	nResult = local_cimgr_FreeSdtSlot(pCiContext->usUniqueId);
//	if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
#endif

/* instance �ʱ�ȭ */
	if (pCiContext->pCam != NULL)
	{
		if (pCiContext->pCam->caPmtOkDsc.pucRaw != NULL)
		{
			OxCAS_Free(pCiContext->pCam->caPmtOkDsc.pucRaw);
			pCiContext->pCam->caPmtOkDsc.pucRaw = NULL;
			pCiContext->pCam->caPmtOkDsc.usRawLen = 0;
		}

#if defined(_Support_Not_Selected_CA_PMT_)
		if (pCiContext->pCam->caPmtNotSel.pucRaw != NULL)
		{
			OxCAS_Free(pCiContext->pCam->caPmtNotSel.pucRaw);
			pCiContext->pCam->caPmtNotSel.pucRaw = NULL;
			pCiContext->pCam->caPmtNotSel.usRawLen = 0;
		}
#endif
	}

	local_cimgr_InitContext(svc_cas_SubSvcRmGetActionId(hAction));

	return ERR_OK;
}

void local_cimgr_DefineChannelType(SvcCas_PmtInfo_t *pstPmtInfo, SvcCas_ChannelType_e *eChType)
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

STATIC HERROR local_cimgr_ProcPmtChanged(CI_PMT_CHANGED_PARAM *pPmtChanged)
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

	pCiContext = local_cimgr_GetContextByActionId(hAction);
	if (pCiContext == NULL) {
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	/* cas mgr ���� parsing �� pmt info */
	pCiContext->stProgInfo.pPmtInfo = CASMGR_GetPmtInfo(hAction);
	if(pCiContext->stProgInfo.pPmtInfo == NULL)
	{
		HxLOG_Error("PmtInfo not null\n");
		return ERR_FAIL;
	}

	/*
		pmt update�� �߻� ��. crc�� version number�� �����ϸ�, update ó���� ���� �ʵ��� �Ѵ�.
	*/
	if(pCiContext->stProgInfo.ulPrePmtCrc == pCiContext->stProgInfo.pPmtInfo->ulCrc &&
		pCiContext->stProgInfo.ucPrePmtVer == pCiContext->stProgInfo.pPmtInfo->ucVerNum)
	{
		HxLOG_Print("updated pmt's crc & version number same with previous one\n");
		return ERR_OK;
	}

	pCiContext->stProgInfo.ulPrePmtCrc = pCiContext->stProgInfo.pPmtInfo->ulCrc;
	pCiContext->stProgInfo.ucPrePmtVer = pCiContext->stProgInfo.pPmtInfo->ucVerNum;

	local_cimgr_DefineChannelType(pCiContext->stProgInfo.pPmtInfo, &pCiContext->eChType);

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
	}
	HxLOG_Print("\t=======================\n");
}
#endif

	/*
		pmt update case

		1. session start ����.
		2. descramble �� pmt ����.

		-- ���� action �� cas manager�� ���� manage instance �Ҵ� �� context�� �����ϴ��� search ��. �������� ������, pCam�� �������
		ca pmt ����.

		-- pCam�� �������� ���� ���� �� EsInfo�� ������ ca pmt�� update �ؾ� ��.
	*/

	if(pCiContext->pCam != NULL)
	{
		if(pCiContext->eState == eCiMgrState_StartCaPmt)
		{
			if(pCiContext->pCam->caPmtOkDsc.pucRaw != NULL)
			{
				nResult = local_cimgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 1, 0);
			}
			else
			{
				nResult = local_cimgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 0, 0);
			}
		}
		else if(pCiContext->eState == eCiMgrState_Stopped)
		{
			HxLOG_Critical("\n\n");
		}
		else
		{
			nResult = local_cimgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 0, 0);
		}
	}
	else
	{
		/*
			instance connect�� ���� ������, FTA�� ��쿡 ���ؼ� Clear Ca pmt�� CAM�� �����Ѵ�.
		*/
		HxLOG_Print("pCam is not allocated \n");
	}

	return nResult;
}

STATIC HERROR local_cimgr_ProcRetrySvc(CI_RETRY_SVC_PARAM *pRetrySvc)
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

	pCiContext = local_cimgr_GetContextByActionId(hAction);
	if (pCiContext == NULL) {
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	HxLOG_Info("pRetrySvc->hAction(0x%08x)\n", hAction);

	if (pCiContext->pCam == NULL)
	{
		nResult = local_cimgr_StopRetrySvc(pCiContext);
		if (nResult != ERR_OK)
		{
			HxLOG_Error("\n");
		}
		return ERR_OK;
	}

#if defined(_LIMIT_CI_RETRY_COUNT_)

	if (pCiContext->ucRetryCnt >= CIMGR_MAX_NUM_OF_RETRY)
	{
		nResult = local_cimgr_StopRetrySvc(pCiContext);
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
			/* TV/RADIO ����?....Video Pid ���� �� Video Decoder State Check. Audio Pid ���� �� Audio Decoder State Check. */
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
				nResult = CARM_VIDEO_GetDecodingState(0, &bVState);
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
			HxLOG_Print("Retry?(%d) , video(%d):%d, audio(%d):%d \n", bIsNeedRetry, bVideo, bVState, bAudio, bAState);
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

#if defined(_Send_Not_Selected_CA_PMT_on_Zapping_or_RetrySvc_)
{
		// new aston cam���� MMIȭ���� �����ؼ� display ���� �ʵ���...
		/* <TODO_CI> : Ư�� ķ�� ���� ���� ó���� �ʿ��ϴٸ�...
		   Application Info �� application_manufacturer / manufacturer_code / menu_string_length ��
		   CA Info �� CA_system_id �� ��� üũ�ϵ��� ����.
		   ������ ķ�� ������ ���� �ʵ���... */
		HUINT8 ucCaSysIdSpecifier = (HUINT8)(s_astCiCam[pCiContext->pCam->usSlotId].ausCaSysId[0] >> 8) & 0xff;
		if (ucCaSysIdSpecifier == eCA_SYS_ID_ASTON)
		{
			if ((s_astCiCam[pCiContext->pCam->usSlotId].caPmtNotSel.usRawLen != 0) && (s_astCiCam[pCiContext->pCam->usSlotId].caPmtNotSel.pucRaw != NULL))
			{
				HxLOG_Print("eCA_SYS_ID_ASTON, caPmtNotSel sending...\n");
				if (CI_SendCaPmt(s_astCiCam[pCiContext->pCam->usSlotId].usSlotId, s_astCiCam[pCiContext->pCam->usSlotId].caPmtNotSel.pucRaw, s_astCiCam[pCiContext->pCam->usSlotId].caPmtNotSel.usRawLen) != ERR_OK)
				{
					HxLOG_Error("\n");
//					return ERR_FAIL;
				}
			}
		}
}
#endif

#if defined(SUPPORT_SMART_CI_MMI_PINCODE)
		nResult = ERR_OK;
		IsWaiting = 0;
		// Quinny : If mmi pincode is popup, then this could be cause of mmi window show-again problem.
		// usSsNo =  CiCas_GetSsNo(pCiSvc->pCam->usSlotId);
		nResult = CiMmi_CheckWaitingUserInput(pCiContext->pCam->usSlotId, &IsWaiting);
		/*
			BS_ADD
			g_bMmiOpened, MMI Session Open �� CaPmt Send���� ����
		*/
		if (!IsWaiting && nResult == ERR_OK && g_bMmiOpened == FALSE)
		{
			nResult = local_cimgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 0, 1);
		}
#else
		nResult = local_cimgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 0, 1);
#endif

	}
	else
	{
		nResult = local_cimgr_StopRetrySvc(pCiContext);
		if (nResult != ERR_OK)
		{
			HxLOG_Error("\n");
		}
	}

#if 0
	switch (pCiContext->eSvcState)
	{
	case CI_SVC_NOT_EXIST:
		break;

	case CI_SVC_STOPPED:
		break;

	case CI_SVC_NOT_DESCRAMBLING:
#if defined(_LIMIT_CI_RETRY_COUNT_)
		if ((pCiContext->pCam == NULL) || (pCiContext->ucRetryCnt >= CIMGR_MAX_NUM_OF_RETRY))
#else
		if (pCiContext->pCam == NULL)
#endif
		{
			nResult = local_cimgr_StopRetrySvc(pCiContext);
			if (nResult != ERR_OK)
			{
				HxLOG_Error("\n");
			}
			return ERR_OK;
		}

		//if ((pCiContext->eTsState == CAS_TS_VALID) && (pCiContext->ulRetryTimerId != INVALID_TIMER_ID))
		//<BS_ADD> Pal channel interface ������ ���� ����. OCTO ���� ��. TS�� State�� ���� �Ǹ� Sevice Stop�� ȣ��ȴ�.
		if(pCiContext->ulRetryTimerId != INVALID_TIMER_ID)
		{
			pCamAvailable = NULL;
			for (i=0; i<svc_cas_DevCiGetNumberOfModule(); i++)
			{
				if (s_astCiCam[i].usCaInfoCnt == 0)
				{
					continue;
				}
				if (&s_astCiCam[i] == pCiContext->pCam)
				{
					continue;
				}
				pCamAvailable = &s_astCiCam[i];
			}

			if (pCamAvailable != NULL)
			{
				/* BS : ���� ������ �Ѵ�. */
				for (j=0; j<CI_MAX_NUM_OF_ACT; j++)
				{
					if (s_astCiContext[j].hSvc == (Handle_t)NULL)
					{
						continue;
					}
					if (s_astCiContext[j].pCam == pCamAvailable)
					{
						pCamAvailable = NULL;
						break;
					}
				}
			}

			if (pCamAvailable != NULL)
			{
				if (local_cimgr_IsMatchedCaSysId(pCiContext, pCamAvailable->usSlotId) == ERR_OK)
				{
					// TODO : �� Null�� Return? -> �ڵ尡 ��Ȯġ �����Ƿ� ���� �ʿ�.
					pCamAvailable = NULL;
				}
			}

			if (pCamAvailable != NULL)
			{
#if defined(_LIMIT_CI_RETRY_COUNT_)
				pCiContext->ucRetryCnt++;
#endif
			}
			else
			{
				CI_SLOT_ID usSlotId = pCiContext->pCam->usSlotId;

				if (CARM_CI_CheckModule(usSlotId) == eCAS_DEV_CI_EXTRACTED)
				{
					return ERR_OK;
				}

				#if defined(_Send_Not_Selected_CA_PMT_on_Zapping_or_RetrySvc_)
				{
					// new aston cam���� MMIȭ���� �����ؼ� display ���� �ʵ���...
					/* <TODO_CI> : Ư�� ķ�� ���� ���� ó���� �ʿ��ϴٸ�...
					   Application Info �� application_manufacturer / manufacturer_code / menu_string_length ��
					   CA Info �� CA_system_id �� ��� üũ�ϵ��� ����.
					   ������ ķ�� ������ ���� �ʵ���... */
					HUINT8 ucCaSysIdSpecifier = (HUINT8)(s_astCiCam[usSlotId].ausCaSysId[0] >> 8) & 0xff;
					if (ucCaSysIdSpecifier == eCA_SYS_ID_ASTON)
					{
						if ((s_astCiCam[usSlotId].caPmtNotSel.usRawLen != 0) && (s_astCiCam[usSlotId].caPmtNotSel.pucRaw != NULL))
						{
							HxLOG_Print("eCA_SYS_ID_ASTON, caPmtNotSel sending...\n");
							if (CI_SendCaPmt(s_astCiCam[usSlotId].usSlotId, s_astCiCam[usSlotId].caPmtNotSel.pucRaw, s_astCiCam[usSlotId].caPmtNotSel.usRawLen) != ERR_OK)
							{
								HxLOG_Error("\n");
//								return ERR_FAIL;
							}
						}
					}
				}
				#endif

#if defined(SUPPORT_SMART_CI_MMI_PINCODE)
				nRet = ERR_OK;
				IsWaiting = 0;
				// Quinny : If mmi pincode is popup, then this could be cause of mmi window show-again problem.
				// usSsNo =  CiCas_GetSsNo(pCiSvc->pCam->usSlotId);
				nRet = CiMmi_CheckWaitingUserInput(usSlotId, &IsWaiting);

				/*
					BS_ADD
					g_bMmiOpened, MMI Session Open �� CaPmt Send���� ����
				*/
				if (!IsWaiting && nRet == ERR_OK && g_bMmiOpened == FALSE)
				{
					nRet = local_cimgr_StartDescramble(pCiContext, usSlotId, 0, 1);
				}
#else
				nRet = local_cimgr_StartDescramble(pCiContext, usSlotId, 0, 1);
#endif

#if defined(_LIMIT_CI_RETRY_COUNT_)
				pCiContext->ucRetryCnt++;
#endif
			}
		}
		break;

	case CI_SVC_DESCRAMBLING:
		nRet = local_cimgr_StopRetrySvc(pCiContext);
		if (nRet != ERR_OK)
		{
			HxLOG_Error("\n");
		}
		break;

	default:
		break;
	}
#endif

	return ERR_OK;
}

#if defined(CONFIG_MW_CI_PLUS)
STATIC HERROR local_cimgr_UpdateSdt2DB(Handle_t hSvcHandle, HBOOL bFreeCiFlag, HUINT8 ucNumOfBrandId, HUINT16 *pusCiBrandId)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bNeedSave = FALSE;
	UNIXTIME			ulCurrentTime = 0;

	DbSvc_Info_t		stSvcInfo;
//	HUINT32			i;

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
		HxLOG_Print("VK_CLOCK_GetTime() Fail ! \n");
		//ulCurrentTime = UNIX_FIRST_DAY;
		return ERR_FAIL;
	}

	// load saved data
	hError = DB_SVC_GetServiceInfo(hSvcHandle, &stSvcInfo);
	if(hError != ERR_OK)
	{
		HxLOG_Print("DB_SVC_GetServiceInfo() Fail ! \n");
		return ERR_FAIL;
	}

	// check save condition
	if ( (DbSvc_GetCiTimeStamp(&stSvcInfo) + (7 * SECONDS_PER_DAY) < ulCurrentTime)	// time stamp expired
		|| (DbSvc_GetCiNumOfEntries(&stSvcInfo) != (HUINT32)ucNumOfBrandId))		// data changed
	{

		bNeedSave = TRUE;
	}
	else if ( (bFreeCiFlag == TRUE)
			&& (DbSvc_GetFreeCiModeFlag(&stSvcInfo) == FALSE))
	{
		/* bFreeCiFlag enabled */
		bNeedSave = TRUE;
	}
	else if ( (bFreeCiFlag == FALSE)
			&& (DbSvc_GetFreeCiModeFlag(&stSvcInfo)) == TRUE))
	{
		/* bFreeCiFlag disable */
		bNeedSave = TRUE;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("========================= \n");
	HxLOG_Print(" SDT update - CI+ \n");
	HxLOG_Print("========================= \n");
	HxLOG_Print("  = DB = \n");
	HxLOG_Print("   TimeStamp(0x%X) \n", DbSvc_GetCiTimeStamp(&stSvcInfo));
	HxLOG_Print("   ucNumOfEntries(0x%X) \n", DbSvc_GetCiNumOfEntries(&stSvcInfo));
	HxLOG_Print("   bFreeCiFlag(%d) \n", DbSvc_GetFreeCiModeFlag(&stSvcInfo));
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
		DbSvc_SetCiTimeStamp(&stSvcInfo, ulCurrentTime);
		DbSvc_SetCiNumOfEntries(&stSvcInfo, ucNumOfBrandId);
		DbSvc_SetCamBrandId(&stSvcInfo, pusCiBrandId, sizeof(HUINT16)*ucNumOfBrandId);
		if (bFreeCiFlag != FALSE)
		{
			DbSvc_GetFreeCiModeFlag(&stSvcInfo, TRUE);
		}
		else
		{
			DbSvc_GetFreeCiModeFlag(&stSvcInfo, FALSE);
		}

		hError = DB_SVC_UpdateServiceInfo(hSvcHandle, &stSvcInfo);
		if(hError != ERR_OK)
		{
			HxLOG_Print("DB_SVC_UpdateServiceInfo() Fail ! \n");
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

/**
  *		CI+ CAM, CI+ service���� shunning condition�� check�ϴ� �Լ�
  *
  *		�Ҹ��� ���� �Ʒ��� ����.
  *		1. start descramble�� (normal�ϰ� 1ȸ, sdt�� ���� �� ���� ��쿣 db�� cached�� data�� 1ȸ)
  *		2. sdt update��
  *		3. cc resource open�� (CAS, CC ������ ������ Neotion CAM ó����)
**/
STATIC HERROR local_cimgr_ProcCheckShunCondition(CI_SLOT_ID usSlotId, CAS_CiDscInfo_t *pstCiDsc, HBOOL *bIsNeedShun)
{
	HBOOL bIsCiPlusCamWithValidBrandId = FALSE;
	HBOOL bIsMatched = FALSE;
	CI_CamInst_t *pCam = NULL;
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

	pCam = &s_astCiCam[usSlotId];
	if(pCam == NULL)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	if(CI_GetBindedCamBrandId(usSlotId, &usBindedCamBrandId) == ERR_OK)
	{
		// �� �Լ��� ���� ������ ���� �ʾ����� usBindedCamBrandId �� 0 �� ��쿡�� usBindedCamBrandId �� ���� ���ӿ� ������ �� (CIv1 ķ������ üũ�ȴ�)
		HxLOG_Print("Cam BrandId (0x%X)\n", usBindedCamBrandId);
		if (usBindedCamBrandId > 0)
			bIsCiPlusCamWithValidBrandId = TRUE;
	}
	else
	{
		/* CIv1 ķ�� �׽� �̷� ��Ȳ�� ������, SmarDTV old prototype & SMiT CI+ ķ���� �޸�...
		   Neotion CI+ prototype ķ�� ��� ķ ���Խ� CC ���ҽ��� �� �ڿ� ������ ������ (CAS -> CC ��)
		   �׽� ķ �����ϸ� ķ�� brand Id �� ���ÿ� �������� ���Ͽ� Host Service Shunning �� �Ǵ� ��Ȳ�� �߻��Ѵ�.
		   �ϴ� �̸� �ذ��ϱ� ���� CONFIG_MW_CI_PLUS ����� local_cimgr__ProcUpdateCaInst()���� �׽� local_cimgr_CallbackDelayedAddCaInst()�� ����ϵ��� �Ѵ�.
		   ���Ŀ� �� �κп� ���� �ٺ����� ������ �־�� �� ���̴� (eCAS_DEV_EVT_CamPlusInfo �̿��� ��ȹ��).
		   -> ���� ���� ������. */
		HxLOG_Print("Can't get BrandId from cam\n");
	}

	HxLOG_Print("Is this CI+ cam with valid BrandId ? (%d)\n", bIsCiPlusCamWithValidBrandId);
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
	   match_brand_flag = "1" and number_of_entries �� "0"
	   and CICAM brand identifier not matched
	   and CI Plus CAM                                          -> Host Service Shunning : active
	6. ci_protection_descriptor present, free_CI_mode is "1",
	   match_brand_flag = "1" and number_of_entries �� "0"
	   and CICAM brand identifier matched
	   and CI Plus CAM                                          -> Host Service Shunning : in-active
*/
	if(pstCiDsc->bCiDscFlag == FALSE)
	{
		HxLOG_Print("Ci Descriptor absent -> no shunning\n");
		*bIsNeedShun = FALSE;
		return ERR_OK;
	}
	else
	{
		HxLOG_Print("Ci Descriptor present\n");
	}

	if(pstCiDsc->bFreeCiFlag == FALSE)
	{
		HxLOG_Print("FreeCiFlag in Ci Descriptor is FALSE -> no shunning\n");
		*bIsNeedShun = FALSE;
		return ERR_OK;
	}
	else
	{
		HxLOG_Print("FreeCiFlag in Ci Descriptor is TRUE\n");
		// TODO : CIPLUS ���� �� ���� �� Normal Mode�� ���� ��� Ȯ�� �ʿ�.
		if(bIsCiPlusCamWithValidBrandId == FALSE)
		{
			HxLOG_Print("Cam with invalid BrandId -> shunning\n");
			*bIsNeedShun = TRUE;
			return ERR_OK;
		}
		else
		{
			if(pstCiDsc->bMatchBrandFlag == FALSE || pstCiDsc->ucNumOfBrandId == 0)
			{
				HxLOG_Print("MatchBrandFlag (%d) or NumOfBrandId (%d) -> no shunning\n", pstCiDsc->bMatchBrandFlag, pstCiDsc->ucNumOfBrandId);
				*bIsNeedShun = FALSE;
				return ERR_OK;
			}
			else // if (pstCiDsc->bMatchBrandFlag == TRUE && pstCiDsc->ucNumOfBrandId != 0)
			{
				HxLOG_Print("MatchBrandFlag (%d) and NumOfBrandId (%d)\n", pstCiDsc->bMatchBrandFlag, pstCiDsc->ucNumOfBrandId);
				for(i=0; i<pstCiDsc->ucNumOfBrandId; i++)
				{
					HxLOG_Print("Descriptor BrandId (0x%X) vs Cam BrandId (0x%X)\n", pstCiDsc->usCiBrandId[i], usBindedCamBrandId);
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

STATIC HERROR local_cimgr_ProcSdtChanged(CI_SDT_CHANGED_PARAM *pParam)
{
	CI_Context_t *pCiContext = NULL;
	CAS_CiDscInfo_t *pDscInfo = NULL;
	HBOOL bIsNeedShun = FALSE;
//	SvcCas_SiTableState_e eSdtTblState = eCASSI_Table_Skipped;		// 2009.03.28

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

	pCiContext = local_cimgr_GetContextByActionId(pParam->hAction);
	if (pCiContext == NULL) {
		HxLOG_Error("invalid context\n");
		OxCAS_Free(pParam->pucTbl);
		pParam->pucTbl = NULL;
		return ERR_FAIL;
	}

#if 0
	if (pCiContext->eActType == eCAS_ActionType_Tsr_Rec || pCiContext->eActType == eCAS_ActionType_Recording)
	{
		OxCAS_Free(pParam->pucTbl);
		pParam->pucTbl = NULL;
		return ERR_OK;
	}
#endif

	local_cimgr_ParseSdtForCiPlus(pCiContext, pParam->pucTbl);
	OxCAS_Free(pParam->pucTbl);
	pParam->pucTbl = NULL;

	// Get Sdt Raw Pool
	HxLOG_Print("ActionHandle(%x), UniqueId(%x)\n", pParam->hAction, pParam->usUniqueId);

	// Parsing.
	if(local_cimgr_GetCiDescInfo(pCiContext, &pDscInfo) != ERR_OK)
	{
		HxLOG_Error("get description info failed\n");

		/*
			2���� ��� ..

			1. SDT Timeout.
			2. Service Id�� �ش��ϴ� SLD�� ����. �̷� ���� Multifeed...
			3. SDT�� SLD�� �����ϳ� CI Descriptor�� �������� ����.

			�� 3 ���� ��쿡 ���ؼ� �ϴ� ��� timeout���� ó�� ��.
		*/

		pCiContext->eSdtTblState = eCASSI_Table_Timeout;

#if defined(_USE_SHUNNING_STATE_)
		pCiContext->eShunState = eCiPlus_ShunnedFlag_None;
#endif
		pCiContext->bSvcShunned = FALSE;

		if(pCiContext->pCam != NULL)
		{
			HxLOG_Print("Ts Path going to Passthrough\n");
			CI_SetPlusMode(pCiContext->pCam->usSlotId, CI_PLUS_FREE_MODE);
			svc_cas_DevCiSetTsPath(pCiContext->pCam->usSlotId, eCAS_DEV_CI_PASSTHROUGH);
		}
		else
		{
			HxLOG_Print("\n");
		}

		return ERR_OK;
	}

	//pCiContext->eSdtTblState = eSdtTblState;

	HxLOG_Print("Unique ID(0x%x) Svc ID (0x%x) (%s) \n", pCiContext->usUniqueId, pCiContext->usSvcId, svc_cas_DbgMakeStrSiTblState(pCiContext->eSdtTblState));

	/* Check unique id, svc id */
	if(pCiContext->usUniqueId != pParam->usUniqueId || pCiContext->usSvcId != pDscInfo->usSvcId)
	{
		HxLOG_Error("\n");
	}

	if(pCiContext->pCam == NULL)
	{
		HxLOG_Print("cam instance not allocated. \n");
		return ERR_OK;
	}
	else if (pCiContext->eChType != eCasChannel_FTA)
	{
		if(local_cimgr_ProcCheckShunCondition(pCiContext->pCam->usSlotId, pDscInfo, &bIsNeedShun) != ERR_OK)
		{
			HxLOG_Error("\n");
		}
	}
	HxLOG_Print("bIsNeedShun (%d)\n", bIsNeedShun);

	/* Host Service Shunning ���� ��Ʈ�� ���� ����� ������ ���� ���� ������ ���� �� �ְ����� �Ʒ��� ��õ� ����� ����ϵ��� �Ѵ�.

	   1. A/V mute control --> LF7300�� ���� 2-chip ���������� ���� ���ڰ� �¾ƾ� �ϰ� �÷������� �׽� üũ�� �ʿ��ؼ� ������.
	   2. stop ca_pmt and DES/AES descrambling --> ������ ���� ����̳� shunning ���� ����ÿ� �����ε尡 �־ ������.
	   3. ci path control --> �� ���� ��... ���� ������ ��Ʈ�� ����� �̰��̴�!

	*/
	if(bIsNeedShun == TRUE)
	{
#if defined(CONFIG_DEBUG)
		HxLOG_Print("\n --- Host Service Shunning ON ---\n\n"); // �߿� �α��̹Ƿ� �׽� ��� ����.
#endif
#if defined(_USE_SHUNNING_STATE_)
		pCiContext->eShunState = eCiPlus_ShunnedFlag_Enable;
		HxLOG_Info("Shun State flag  On\n");
#endif
		pCiContext->bSvcShunned = TRUE;
		CI_SetPlusMode(pCiContext->pCam->usSlotId, CI_PLUS_PROTECTED_MODE);

		if(pCiContext->pCam != NULL)
		{
			HxLOG_Print("Ts Path going to Bypass\n");
			svc_cas_DevCiSetTsPath(pCiContext->pCam->usSlotId, eCAS_DEV_CI_BYPASS);
		}
	}
	else
	{
#if defined(_USE_SHUNNING_STATE_)
		pCiContext->eShunState = eCiPlus_ShunnedFlag_Disable;
		HxLOG_Info("Shun State flag  Off\n");
#endif
		pCiContext->bSvcShunned = FALSE;
		CI_SetPlusMode(pCiContext->pCam->usSlotId, CI_PLUS_FREE_MODE);

		if((pCiContext->pCam != NULL))
		{
			HxLOG_Print("Ts Path going to Passthrough\n");
			svc_cas_DevCiSetTsPath(pCiContext->pCam->usSlotId, eCAS_DEV_CI_PASSTHROUGH);
		}
	}

	// save sdt to db
	local_cimgr_UpdateSdt2DB(pCiContext->hSvc, pDscInfo->bFreeCiFlag, pDscInfo->ucNumOfBrandId, pDscInfo->usCiBrandId);

	return ERR_OK;
}

#endif // #if defined(CONFIG_MW_CI_PLUS)

STATIC HERROR local_cimgr_ProcManageInstance(CI_MANAGE_CA_INST_PARAM *pParam)
{
	CI_Context_t *pCiContext = NULL;
	CI_CamInst_t *pCamSelected = NULL;
	HERROR nResult = ERR_FAIL;
	HUINT32 i;

	if (pParam == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	pCiContext = local_cimgr_GetContextByActionId(pParam->hAction);
	if (pCiContext == NULL)
	{
		HxLOG_Error("Sequence Error. �ݵ�� ��ƶ�. \n");
		return ERR_FAIL;
	}

	for(i=0; i<CARM_MAX_CISLOT_NUMBER; i++)
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

	HxLOG_Info("hAction(0x%08x), bConnect(%d), pCiContext->pCam(0x%x)\n", pParam->hAction, pParam->bConnect, (HUINT32)pCiContext->pCam);

	if(pParam->bConnect == TRUE) // ķ�� ���� �ִ� ��Ȳ
	{
		HBOOL bIsNeedPathChange = FALSE;

		/*
		2009.07.21 �߰�.
			- cas manager���� instance�� ci�� ���� �� application���� "cas_ok"�� �ִ� trigger. ���ѿ� ��� ����. Descramble Try�� �ϸ� �ݵ�� ok �����ؾ� ��.
		*/
		CASMGR_UpdateCasMessage(pParam->hAction, eCAS_MSG_TYPE_Ok, eDxCAS_GROUPID_CAM, NULL, 0);

		/* Instance Connect�� �ݵ�� DisConnect �Ŀ� Connect �Ǿ�� ��. */
		if(pCiContext->pCam != NULL)
		{
			#if 0 // #if defined(CONFIG_DEBUG) // ������ �ڵ�.
			CI_PrintCAMInfo();
			CI_PrintContextInfo();
			#endif

			HxLOG_Error("sequence error, 0x%x 0x%x\n", (HUINT32)pCiContext->pCam, (HUINT32)pCamSelected);
		}

		/* �� �������� Ci Context�� ���� cam instance�� ���� �Ǵ� ���̴�.*/
		pCiContext->pCam = pCamSelected;

#if defined (CONFIG_MW_CI_PLUS) && defined (CONFIG_MW_MM_PVR)
{
		/*
			PVR ���� version 1 cam (�Ϲ� ķ)�� ����ϴ� ���. ��� Action�� ���ؼ� Drm None�� �˷���� �Ѵ�.
		*/
		CI_CC_KEY_CIPHER enNegotiatedCckCipher = CI_CC_KEY_CIPHER_DES_56_ECB;

		if ((pCamSelected->bKeyInfoAvailable) || (CI_GetNegotiatedCckCipher(pCamSelected->usSlotId, &enNegotiatedCckCipher) != ERR_OK))
		{
			CASMGR_UpdateDrmInfo(pCiContext->hAction, pCamSelected->hCasDevice, (void *)NULL);
		}
}
#endif

		local_cimgr_CheckPathChanging(pCamSelected, pCiContext, &bIsNeedPathChange);

		if (bIsNeedPathChange == TRUE)
		{
			nResult = local_cimgr_ChangePath(pCiContext, pCiContext->pCam->usSlotId, TRUE);
			if(nResult != ERR_OK)
			{
				HxLOG_Error("change path. slot id (%d) \n", pCiContext->pCam->usSlotId);
				return ERR_FAIL;
			}

			if (pCiContext->eActType == eCAS_ActionType_Live || pCiContext->eActType == eCAS_ActionType_Playback )//|| pCiContext->eActType == eCAS_ActionType_Tsr_Play)
			{
				HxLOG_Print("Live \n");
				local_cimgr_SearchOtherActionForConnecting(pCamSelected, pCiContext);
			}

#if defined(CONFIG_MW_CI_PLUS)
			local_cimgr_CheckUriControl(pCiContext, pCamSelected);
#endif
		}

#ifdef DEBUG_ACT_LIST
		HxLOG_Print("pCamSelected->ulNumOfAct (%d)\n", pCamSelected->ulNumOfAct);

		HxLOG_Print("hAction List\n");
		for(i=0; i<CAS_MAX_NUM_OF_ACTION; i++)
		{
			HxLOG_Print("(0x%08x) ", pCamSelected->hActList[i]);
		}
		HxLOG_Print("\n");
#endif

#if defined(_USE_SHUNNING_STATE_)
		pCiContext->eShunState = eCiPlus_ShunnedFlag_None;
#endif

/*
#if defined(CONFIG_MW_CI_PLUS)
		CI_SetUriDefault(pCiContext->pCam->usSlotId, TRUE);
#endif
*/

		/*
			���⼭ action�� ���¸� ���ؼ�.
			Live�� Rec ���� �켱���� ������ �ؾ� �Ѵ�.

			1. Live Descramble ��.
			  1-1. ���� ���� Recording
			  1-2. �ٸ� ���� Recording

			2. Rec Descramble ��.
			  2-1. ���� ���� Live
			  2-2. �ٸ� ���� Live
			  2-3. ���� ���� Rec
			  2-4. �ٸ� ���� Rec
		*/

		/* pmt �� ���� ���� ���ٸ�, Descramble Start �ʿ� ����. */
		if (pCiContext->stProgInfo.pPmtInfo == NULL)
		{
			HxLOG_Print("pmt not updated. \n");
			return ERR_OK;
		}

		nResult = local_cimgr_StartDescramble(pCiContext, pCiContext->pCam->usSlotId, 0, 0);
		if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

	}
	else // ķ�� ���ŵ� ��Ȳ
	{
		// dis connect
		if (pCiContext->pCam == NULL) // SVC Instance�� ��ϵǾ� �ִ� CAM Instance�� Pointer�� Null�ΰ��.
		{
			HxLOG_Error(" sequence error. �ݵ�� ��ƾ� �Ѵ�.\n");
			return ERR_FAIL;
		}
		else
		{
			/*
				���⼭ action�� ���¸� ���ؼ�.
				Live�� Rec ���� �켱���� ������ �ؾ� �Ѵ�.

				1. Live
				  1-1. ���� ���� Recording�� ���� ���.

				2. Rec
				  2-1. ���� ���� Live�� ���� ���.
				  2-3. ���� ���� Rec
				  2-4. �ٸ� ���� Rec
			*/

			if(pCiContext->pCam->ulNumOfAct == 0)
			{
				HxLOG_Critical("\n\n");
				return ERR_FAIL;
			}

			if (local_cimgr_CheckActionHandleInCamInstance(pCiContext->pCam, pCiContext) == FALSE)
			{
				HxLOG_Error("this action is not need stop descramble \n");
				return ERR_OK;
			}

#ifdef DEBUG_ACT_LIST
			HxLOG_Print("pCiContext->pCam->ulNumOfAct (%d)\n", pCiContext->pCam->ulNumOfAct);

			HxLOG_Print("hAction List\n");
			for(i=0; i<CAS_MAX_NUM_OF_ACTION; i++)
			{
				HxLOG_Print("(0x%08x) ", pCiContext->pCam->hActList[i]);
			}
			HxLOG_Print("\n");
#endif

			if(pCiContext->pCam->ulNumOfAct == 1)
			{
				nResult = local_cimgr_StopDescramble(pCiContext, pCiContext->pCam->usSlotId);
				if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

#if defined(CONFIG_MW_CI_PLUS)
				CI_SetUriDefault(pCiContext->pCam->usSlotId, FALSE);
#endif
			}
			else
			{
#if defined(CONFIG_MW_CI_PLUS)
				HxLOG_Print("slot %d's NumOfDsc = %d, KeyInfoAvailable = %d\n",
					pCiContext->pCam->usSlotId, pCiContext->pCam->ulNumOfDsc[pCiContext->ulDemuxId], pCiContext->pCam->bKeyInfoAvailable);

				if (pCiContext->ulDemuxId != CAS_RESOURCE_NONE)
				{
					for (i = 0; i < pCiContext->pCam->ulNumOfDsc[pCiContext->ulDemuxId]; i++)
					{
						nResult = svc_cas_DevDscClose(pCiContext->pCam->stDscInfo[pCiContext->ulDemuxId][i].ulDscId);
						if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

						pCiContext->pCam->stDscInfo[pCiContext->ulDemuxId][i].ulDscId = CAS_RESOURCE_NONE;
						pCiContext->pCam->stDscInfo[pCiContext->ulDemuxId][i].usPid = PID_NULL;
					}
				}
				else
				{
					HxLOG_Critical("\n\n");
				}

				pCiContext->pCam->ulNumOfDsc[pCiContext->ulDemuxId] = 0; // pCam->bKeyInfoAvailable �� Ű ���� ������ �������� �ʵ��� �Ѵ�.

				//pCiContext->bSvcShunned = FALSE;
#endif
			}

			pCiContext->eState = eCiMgrState_StopCaPmt;

			nResult = local_cimgr_DeleteActionToCamInstance(pCiContext->pCam, pCiContext->hAction);
			if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

			nResult = local_cimgr_ChangePath(pCiContext, pCiContext->pCam->usSlotId, FALSE);
			if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

			/* context���� instance �ʱ�ȭ. */
			pCiContext->pCam = NULL;

			return ERR_OK;
		}
	}

	return ERR_OK;
}

STATIC HERROR local_cimgr_ProcUpdateCaInst(CI_UPDATE_CA_INST_PARAM *pUpdateCaInst)
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

	HxLOG_Print("SlotId (%d), CaInfoCnt (%d), hCasDevice (0x%x)\n",
					pUpdateCaInst->usSlotId, pUpdateCaInst->usCaInfoCnt, s_astCiCam[pUpdateCaInst->usSlotId].hCasDevice);

	/* cam instance �� slot id�� ���� �ȴ�. */
	pCam = &s_astCiCam[pUpdateCaInst->usSlotId];

	if (pCam->usCaInfoCnt == 0) // pCam �� ����� CaInfo �� ���� ���
	{
		if ((pUpdateCaInst->usCaInfoCnt == 0) || (pUpdateCaInst->ausCaSysId == NULL))
		{
			HxLOG_Error("\n");
			return ERR_FAIL;
		}

		if (CI_GetAppInfo(pUpdateCaInst->usSlotId, &pAppInfo) != ERR_OK) // ��� AI -> CAS ������ ���ҽ��� ���µǹǷ� ���⼭ ������ ���� ���� ������....
		{
			HxLOG_Error("\n");
			return ERR_FAIL;
		}

		pCam->usSlotId = pUpdateCaInst->usSlotId;
		pCam->usManufacturer = pAppInfo->usManufacturer;

		if (pCam->ausCaSysId != NULL) // �̷� ���� ���� �������� Ȯ���� ���ϱ� ���ؼ�...
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
		nResult = (HINT32)CASMGR_UpdateCasInfo(pCam->hCasDevice, pCam->ausCaSysId[0] & 0xff00, pUpdateCaInst->usSlotId, pCam->ausCaSysId, pCam->usCaInfoCnt, 1, TRUE);
#else
		{
			unsigned long ulDelayTimeOut = 0;
			CI_SLOT_ID usSlotId;
			HULONG ulTimerId = 0;

			#if 0

			/* Host Service Shunning ������ �߰��� �ڵ��
			   �ڼ��� ���� local_cimgr_ProcCheckShunCondition()�� �ִ� �ּ� ������ ��.
			   ���Ŀ� �� �κп� ���� �ٺ����� ������ �־�� �� ���̴� (eCAS_DEV_EVT_CamPlusInfo �̿��� ��ȹ��).
			   -> ���� ���� �����Ͽ� �� �ڵ�� ������. */

			ulDelayTimeOut = 5000; // Neotion CI+ ķ �������� 4000ms ������ ������ �ȴ�.

			#else

			if (pCam->usCaInfoCnt > 6) // Ư�� ķ ������ ���� ķ�� �ټ��� CaSysId ���� ������ ������ ������ ������ �ֵ��� ����...
			{
				ulDelayTimeOut = 3000;
			}

			#endif

			if (ulDelayTimeOut)
			{
				HxLOG_Print("ulDelayTimeOut is %lu ms\n", ulDelayTimeOut);
			}

			if (ulDelayTimeOut == 0)
			{
				nResult = (HINT32)CASMGR_UpdateCasInfo(pCam->hCasDevice,
//												pCam->ausCaSysId[0] & 0xff00,
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
				nResult = VK_TIMER_EventAfter(ulDelayTimeOut, local_cimgr_CallbackDelayedAddCaInst, (void *)&usSlotId, sizeof(CI_SLOT_ID), &ulTimerId);
			}
		}
#endif
		if (nResult != ERR_OK)
		{
			HxLOG_Error("\n");
			return nResult;
		}

		if (pCam->usCaInfoCnt > 0) // ������ pCam->usCaInfoCnt == 0 �� ��찡 �ɷ����Ƿ� �� ���� üũ�� �ʿ� ����...
		{
			g_aucIsCaSysId[pCam->usSlotId] = 1;
			HxLOG_Print("g_aucIsCaSysId[%d] is enabled\n", pCam->usSlotId);
		}
	}
	else // pCam �� ����� CaInfo �� �ִ� ���
	{
		if (pUpdateCaInst->usCaInfoCnt == 0) // CA instance �� release �� ��Ȳ (��, CAS ���� ����� CiCas_DeleteCaInst() ����� ��) -> ķ�� ���ŵ� ������ ���� �ʿ��� ó���� ���ش�.
		{
			if (pCam->usSlotId >= svc_cas_DevCiGetNumberOfModule())
			{
				HxLOG_Error("input slot id (%d)\n", pCam->usSlotId);
			}

			if (pCam->usSlotId != pUpdateCaInst->usSlotId)
			{
				HxLOG_Error("pCam->usSlotId = %x, pUpdateCaInst->usSlotId = %x\n", pCam->usSlotId, pUpdateCaInst->usSlotId);
			}

			HxLOG_Print("g_aucIsCaSysId[%d] = %d -> will be disabled\n", pUpdateCaInst->usSlotId, g_aucIsCaSysId[pUpdateCaInst->usSlotId]);
			g_aucIsCaSysId[pUpdateCaInst->usSlotId] = 0;

			// SVC Inst���� CAM Inst ���� (s_astCiContext ���� pCam ����)
#if 0
			nResult = local_cimgr_DeleteCamInContext(pCam);
			if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
#endif
			// CAS mgr ���Ե� �˷��ش�.
			nResult = CASMGR_UpdateCasInfo(pCam->hCasDevice, eDxCAS_GROUPID_CAM, pUpdateCaInst->usSlotId, NULL, 0, 0, FALSE);
			if (nResult != ERR_OK)
			{
				HxLOG_Error("\n");
				return nResult;
			}

			// ���ϴ� CAM Inst �ʱ�ȭ
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
#if defined(_Support_Not_Selected_CA_PMT_)
			if (pCam->caPmtNotSel.pucRaw != NULL)
			{
				OxCAS_Free(pCam->caPmtNotSel.pucRaw);
				pCam->caPmtNotSel.pucRaw = NULL;
				pCam->caPmtNotSel.usRawLen = 0;
			}
#endif

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
		else // pCam �� �̹� ����� CaInfo �� �ִµ� CA instance �� ���� �ö�� ��� -> ��� �Ϲ������� ���� ����̳�...
		{
			HxLOG_Error("\n");
		}
	}

	return ERR_OK;
}

STATIC HERROR local_cimgr_ProcAddDelayedCaInst(CI_UPDATE_CA_INST_PARAM *pAddCaInst)
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
	if (pCam == NULL || pCam->usCaInfoCnt == 0)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	/* ci �� ��� cas group id�� NONE���� ���� ��. */
	nResult = CASMGR_UpdateCasInfo(pCam->hCasDevice, eDxCAS_GROUPID_CAM, pCam->usSlotId, pCam->ausCaSysId, pCam->usCaInfoCnt, 1, TRUE);
	if(nResult != ERR_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

#if defined(CI_PLUS_PVR_ENABLE)
STATIC HERROR local_cimgr_ProcUpdateCaPvrInst(CI_UPDATE_CA_PVR_INST_PARAM *pUpdateCaPvrInst)
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

	HxLOG_Print("SlotId (%d), CaInfoCnt (%d), hCasDevice (0x%x)\n",
					pUpdateCaPvrInst->usSlotId, pUpdateCaPvrInst->usCaPvrInfoCnt, s_astCiCam[pUpdateCaPvrInst->usSlotId].hCasDevice);

	pCam = &s_astCiCam[pUpdateCaPvrInst->usSlotId];

	/* <TODO_CI_PLUS> : pCam �� usCaPvrInfoCnt, ausCaPvrCaSysId, aucCaRecMode ������Ʈ�ϰ�... */

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

STATIC HINT32 local_cimgr_ProcUpdateCamState(CI_UPDATE_CAM_STATE_PARAM *pUpdateCamSt)
{
	if (pUpdateCamSt == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	HxLOG_Print("SlotId[0x%x], state[%d]\n", pUpdateCamSt->usSlotId, pUpdateCamSt->eCamState);

	if (pUpdateCamSt->usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", pUpdateCamSt->usSlotId);
		return ERR_FAIL;
	}

	// ci path ���� �� ci+ �� ��� ���µ� ��ũ������ ������ �ݾ��ش�.
	if(pUpdateCamSt->eCamState == CI_CAM_EXTRACTED)
	{
		#if defined(CONFIG_MW_CI_PLUS)
		{
			CI_CamInst_t *pCam = &s_astCiCam[pUpdateCamSt->usSlotId];
			HERROR nResult = ERR_FAIL;
			HUINT32 i, j;

			if (pCam->usSlotId != pUpdateCamSt->usSlotId) // �̹� ķ �ν��Ͻ��� �ʱ�ȭ �Ǿ��� �� �̷� ���� �ִ�. �ش� ���� ���̵��� s_astCiCam[]�� ��ũ���� ������ �����ϸ� �ǹǷ� ����.
			{
				HxLOG_Error("pCam->usSlotId = %x, pUpdateCamSt->usSlotId = %x\n", pCam->usSlotId, pUpdateCamSt->usSlotId);
			}

			/*
				�����ϴ� ��� DSC Close
			*/
			for (i = 0; i < CI_MAX_NUM_OF_DMX; i++)
			{
				HxLOG_Print("slot %d's NumOfDsc = %d, KeyInfoAvailable = %d\n",
					pUpdateCamSt->usSlotId, pCam->ulNumOfDsc[i], pCam->bKeyInfoAvailable);
				for (j = 0; j < pCam->ulNumOfDsc[i]; j++)
				{
					nResult = svc_cas_DevDscClose(pCam->stDscInfo[i][j].ulDscId);
					pCam->stDscInfo[i][j].ulDscId = CAS_RESOURCE_NONE;
					pCam->stDscInfo[i][j].usPid = PID_NULL;

					if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
				}

				pCam->ulNumOfDsc[i] = 0;
			}

			pCam->bKeyInfoAvailable = 0;
			s_bUriSent = FALSE;
		}
		#endif

		//BS_ADD
		svc_cas_DevCiSetTsPath(pUpdateCamSt->usSlotId, eCAS_DEV_CI_BYPASS);
	}

	return ERR_OK;
}

#if 1 // #if defined(CONFIG_MW_CI_PLUS_PVR) : ������ CA PVR ������ ���� �߰��� �������� Multiple CA application selection procedure ���� ���� �ٸ� �뵵�� Ȯ���ؼ� �� ������ �����Ƿ� �Ϲ������� ������ ��.
STATIC HINT32 local_cimgr_ProcUpdateCasState(CI_UPDATE_CAS_STATE_PARAM *pUpdateCasSt)
{
	if (pUpdateCasSt == NULL)
	{
		HxLOG_Error("invalid param\n");
		return ERR_FAIL;
	}

	HxLOG_Print("SlotId[0x%x], state[%d]\n", pUpdateCasSt->usSlotId, pUpdateCasSt->eCasState);

	if (pUpdateCasSt->usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", pUpdateCasSt->usSlotId);
		return ERR_FAIL;
	}

	switch (pUpdateCasSt->eCasState) /* <TODO_CI/TODO_CI_PLUS> : �� case ���� ���� �ʿ� */
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
#endif

#if defined(CONFIG_MW_CI_PLUS)

STATIC HERROR local_cimgr_ProcKeyChanged(CI_PLUS_KEY_CHANGE_PARAM *pParam)
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

	if (pCam->usSlotId != usSlotId) // ���� ķ �ν��Ͻ��� �ʱ�ȭ ���� �ʾ��� �� �̷� ���� �ִ�. �ش� ���� ���̵��� s_astCiCam[]�� Ű ������ �����ϸ� �ǹǷ� ����.
	{
		HxLOG_Error("pCam->usSlotId = %x, usSlotId = %x\n", pCam->usSlotId, usSlotId);

		if (pCam->ulNumOfDsc[0] || pCam->bKeyInfoAvailable) // �׷��� �̷� ��Ȳ�̸� �����ϸ� �ȵȴ�. ���� ������ �÷ο� �� ������ �ִ� �Ŵ�. Ȯ�� �ʿ� !!
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
		HxLOG_Print("[%d] slot %d's NumOfDsc = %d, KeyInfoAvailable = %d\n", j, usSlotId, pCam->ulNumOfDsc[j], pCam->bKeyInfoAvailable);

		if (pCam->ulNumOfDsc[j])
		{
			for (i = 0; i < pCam->ulNumOfDsc[j]; i++)
			{
				if (pCam->stDscInfo[j][i].ulDscId != CAS_RESOURCE_NONE)
				{
					nResult = svc_cas_DevDscSetKeyForCiPlus(pCam->stDscInfo[j][i].ulDscId, pCam->eDscType, pCam->eParity,
						pCam->ucCck, (pCam->eDscType == eCAS_DSC_TYPE_DES_CLEAR) ? NULL : pCam->ucCiv);
					if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
				}
				else
				{
					HxLOG_Critical("\n\n");
				}
			}
		}
		else
		{
//			SmarDTV old prototype & SMiT CI+ ķ�� ���, ķ ���Խ� CC -> CAS ������ ���ҽ��� ���µǱ� ������ ķ ���Խÿ� �׽� �̷� ��Ȳ�� �߻��Ѵ� (����).
//			��ũ������ ���µǾ� ���� ������ Ű �������� ������ �׸��̹Ƿ� �Ʒ� �α״� �Ϲ������� ������ �ʵ��� �Ѵ�.
//			�׸��� FTA ä�ο����� ķ�� CCK �� ���� Ʈ���Ÿ� �ϸ� �� ��Ȳ�� �߻��� �� �ִ� (FTA ä�ο����� ��ũ������ ������� �ʱ� ����. �̵� ����).
			HxLOG_Print("[%d] slot %d's descrambler NOT opened !!\n", j, usSlotId);
		}
	}

	return nResult;
}

#if 0
STATIC HERROR local_cimgr_GetServiceId(HUINT16 *pusServiceId)
{
	Handle_t	hCurSvc;
	DbSvc_Info_t	svcInfo;
	HERROR		err;

	err = DB_PARAM_GetItem(eDB_PARAM_ITEM_CUR_SVC, (HUINT32 *)&hCurSvc, 0);
	if (err == ERR_OK)
	{
		err = DB_SVC_CheckValidServiceHandle(hCurSvc);
		if (err == ERR_OK)
		{
			err = DB_SVC_GetServiceInfo(hCurSvc, &svcInfo);
			if (err == ERR_OK)
			{
				*pusServiceId = svcInfo.usSvcId;
				return ERR_OK;
			}
		}
	}

	return ERR_FAIL;
}
#endif

/* STATIC */ HINT32 local_cimgr_TurnOffUriControlForFTA(CI_SLOT_ID usSlotId) // for FTA channel
{
#if 0

	HERROR nResult = ERR_OK;

	if (usSlotId >= svc_cas_DevCiGetNumberOfModule())
	{
		HxLOG_Error("input slot id (%d)\n", usSlotId);
		return ERR_FAIL;
	}

#if defined(CONFIG_DOUGLAS)

	// EMI & APS : CI_CC_URI_EMI_COPY_FREE and CI_CC_URI_APS_OFF
	#if defined(URI_CONTROL_USE_AV_BLOCK)
	nResult |= PALTV_PM_ScartMute(FALSE, 0x20);
	nResult |= PALTV_SOUND_SetMute(FALSE, ePALTV_AUD_SPDIF);
	#else
	nResult |= PAL_VIDEO_ControlApsMode(0, eVidApsMode_Off);
	nResult |= PAL_AUDIO_ControlScmsForDigitalOutput(0, eCOPYRIGHT_COPY_FREE);
	#endif

	/* <TODO_CI_PLUS> : �ش� �𵨿��� �߰� �۾� �ʿ��� (LF7300/LH3300/HB1�� ��쿣 ICT, RCT, RL control ���ʿ�) */

	// ICT : CI_CC_URI_ICT_OFF
	// ...

	// RCT : CI_CC_URI_RCT_OFF
	// ...

#else

	// TODO : �ű� �÷������� ���� �ʿ�

#endif

	// cancel URI timer if it is running
	CI_SetUriDefault(usSlotId, FALSE);

	return nResult;

#endif

	return 0;

}

#if defined(CONFIG_MW_CI_PLUS)
STATIC HERROR local_cimgr_CheckUriControl(CI_Context_t *pCiContext, CI_CamInst_t *pCam)
{
	if (s_bUriSent == TRUE)
	{
		CASMGR_UpdateDrmInfo(pCiContext->hAction, pCam->hCasDevice, (void *)&s_stUriInfo[0]);
	}

	return ERR_OK;
}
#endif

STATIC HERROR local_cimgr_ProcUriControl(CI_PLUS_URI_CONTROL_PARAM *pParam) // Apply URI to External Output for "scramble" channel
{
	CI_SLOT_ID usSlotId;
	CI_PLUS_MODE enPlusMode;
//	unsigned short usCurrentServiceId = 0;
	HERROR nResult = ERR_OK;

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

	/* <TODO_CI_PLUS> : �ܺ� ����� ���� �ش� ķ ���Կ� �Ҵ�Ǿ� �ִ��� Ȯ���ϰ� �׷��� ������ �׳� �����ϵ��� ����... */

	// FTA ä���ε� �� �Լ��� ����ȴٸ� ������ �÷ο� �� ������ �ְų� ( -> see local_cimgr_TurnOffUriControlForFTA() )
	// ä�� ��ȯ�ϸ鼭 ���� ä���� ���� �ڴʰ� �� ����̴�.

	CI_GetPlusMode(usSlotId, &enPlusMode);

#if defined(CONFIG_DEBUG) // �߿� �α��̹Ƿ� �׽� ��� ����.

	switch (pParam->stUriInfo.ucMode)
	{
		case CI_CC_URI_MODE_DEFAULT :
			HxLOG_Print(" > URI (Default) :");
			break;
		case CI_CC_URI_MODE_TIMEOUT :
			HxLOG_Print(" > URI (Timeout) :");
			break;
		case CI_CC_URI_MODE_FROMCAM :
			HxLOG_Print(" > URI (FromCAM) :");
			break;
		default:
			HxLOG_Print(" > URI (Unknown) :");
			break;
	}
	HxLOG_Print(" APS (%x), EMI (%x), ICT (%x), RCT (%x), DOT (%x), RL (%ld min), PlusMode (%d)",
		pParam->stUriInfo.ucAps, pParam->stUriInfo.ucEmi, pParam->stUriInfo.ucIct, pParam->stUriInfo.ucRct, pParam->stUriInfo.ucDot, pParam->stUriInfo.ulMinutesForRL, enPlusMode);
	if (pParam->stUriInfo.ucMode == CI_CC_URI_MODE_FROMCAM)
		HxLOG_Print(" for 0x%x (%d)\n", pParam->stUriInfo.usServiceId, pParam->stUriInfo.usServiceId);
	else
		HxLOG_Print("\n");

#endif

	if (enPlusMode == CI_PLUS_PROTECTED_MODE)
	{
		// shunning �Ǿ� �ִٸ� URI control �� �ʿ䰡 ����...
		return ERR_OK;
	}

	if (pParam->stUriInfo.ucMode != CI_CC_URI_MODE_FROMCAM)
	{
		goto DO_URI_CONTROL;
	}

#if 0
	if (local_cimgr_GetServiceId(&usCurrentServiceId) != ERR_OK)
	{
		// �̷� ��찡 ������ �ȵǰ����� �ϴ��� URI control �ϵ��� ���� -> ����� �ʿ�!
		HxLOG_Error("Can't get the current service id !!\n");
		goto DO_URI_CONTROL;
	}

	if (pParam->stUriInfo.usServiceId != usCurrentServiceId)
	{
		// �ϴ� ERR_OK �����ϵ��� �ϱ�� ������ �̷� ��� �� �̻��� ��Ȳ�̴� -> ����� �ʿ�! (��, ä�� ��ȯ�ϸ鼭 ���� ä���� ���� �ڴʰ� �� ���� �����Ƿ�
		HxLOG_Error("Strange or previous service id 0x%x (%d) received from CAM\n", pParam->stUriInfo.usServiceId, pParam->stUriInfo.usServiceId);
		return ERR_OK;
	}
#endif

DO_URI_CONTROL :

#if defined(CONFIG_DOUGLAS)

	// EMI & APS
	switch (pParam->stUriInfo.ucEmi)
	{
		case CI_CC_URI_EMI_COPY_FREE :
			// emi is 0 0
			// make scms 1 0
			#if defined(URI_CONTROL_USE_AV_BLOCK)
				nResult |= PALTV_PM_ScartMute(FALSE, 0x20);
				nResult |= PALTV_SOUND_SetMute(FALSE, ePALTV_AUD_SPDIF);
			#else
				nResult |= PAL_VIDEO_ControlApsMode(0, eVidApsMode_Off);
				nResult |= PAL_AUDIO_ControlScmsForDigitalOutput(0, eCOPYRIGHT_COPY_FREE);
			#endif
		break;

		case CI_CC_URI_EMI_COPY_NO_MORE :
			#if defined(URI_CONTROL_USE_AV_BLOCK)
				if (pParam->stUriInfo.ucAps == CI_CC_URI_APS_OFF)
					nResult |= PALTV_PM_ScartMute(FALSE, 0x20);
				else
					nResult |= PALTV_PM_ScartMute(TRUE, 0x20);
				nResult |= PALTV_SOUND_SetMute(TRUE, ePALTV_AUD_SPDIF);
			#else
				if (pParam->stUriInfo.ucAps == CI_CC_URI_APS_OFF)
					nResult |= PAL_VIDEO_ControlApsMode(0, eVidApsMode_Off);
				else if (pParam->stUriInfo.ucAps == CI_CC_URI_APS_ON_1)
					nResult |= PAL_VIDEO_ControlApsMode(0, eVidApsMode_On_1);
				else if (pParam->stUriInfo.ucAps == CI_CC_URI_APS_ON_2)
					nResult |= PAL_VIDEO_ControlApsMode(0, eVidApsMode_On_2);
				else if (pParam->stUriInfo.ucAps == CI_CC_URI_APS_ON_3)
					nResult |= PAL_VIDEO_ControlApsMode(0, eVidApsMode_On_3);
				nResult |= PAL_AUDIO_ControlScmsForDigitalOutput(0, eCOPYRIGHT_COPY_NOMORE);
			#endif
		break;

		case CI_CC_URI_EMI_COPY_ONCE :
			#if defined(URI_CONTROL_USE_AV_BLOCK)
				nResult |= PALTV_PM_ScartMute(FALSE, 0x20);
				nResult |= PALTV_SOUND_SetMute(TRUE, ePALTV_AUD_SPDIF);
			#else
				nResult |= PAL_VIDEO_ControlApsMode(0, eVidApsMode_Off);
				nResult |= PAL_AUDIO_ControlScmsForDigitalOutput(0, eCOPYRIGHT_COPY_ONCE);
			#endif
		break;

		case CI_CC_URI_EMI_COPY_NEVER :
			#if defined(URI_CONTROL_USE_AV_BLOCK)
				if (pParam->stUriInfo.ucAps == CI_CC_URI_APS_OFF)
					nResult |= PALTV_PM_ScartMute(FALSE, 0x20);
				else
					nResult |= PALTV_PM_ScartMute(TRUE, 0x20);
				nResult |= PALTV_SOUND_SetMute(TRUE, ePALTV_AUD_SPDIF);
			#else
				if (pParam->stUriInfo.ucAps == CI_CC_URI_APS_OFF)
					nResult |= PAL_VIDEO_ControlApsMode(0, eVidApsMode_Off);
				else if (pParam->stUriInfo.ucAps == CI_CC_URI_APS_ON_1)
					nResult |= PAL_VIDEO_ControlApsMode(0, eVidApsMode_On_1);
				else if (pParam->stUriInfo.ucAps == CI_CC_URI_APS_ON_2)
					nResult |= PAL_VIDEO_ControlApsMode(0, eVidApsMode_On_2);
				else if (pParam->stUriInfo.ucAps == CI_CC_URI_APS_ON_3)
					nResult |= PAL_VIDEO_ControlApsMode(0, eVidApsMode_On_3);
				nResult |= PAL_AUDIO_ControlScmsForDigitalOutput(0, eCOPYRIGHT_COPY_NEVER);
			#endif
		break;

		default:
			HxLOG_Error("Invalid ucEmi (0x%x) !!\n", pParam->stUriInfo.ucEmi);
		break;
	}

	/* <TODO_CI_PLUS> : �ش� �𵨿��� �߰� �۾� �ʿ��� (LF7300/LH3300/HB1�� ��쿣 ICT, RCT, RL control ���ʿ�) */

	// ICT
	if (pParam->stUriInfo.ucIct == CI_CC_URI_ICT_OFF)
	{

	}
	else
	{

	}

	// RCT
	if (pParam->stUriInfo.ucRct == CI_CC_URI_RCT_OFF)
	{

	}
	else
	{

	}

	// DOT
	if (pParam->stUriInfo.ucDot == CI_CC_URI_DOT_OFF)
	{

	}
	else
	{

	}

	// RL
	if (pParam->stUriInfo.ulMinutesForRL == 0)
	{

	}
	else
	{

	}

#else
{
	CI_CamInst_t *pCam = (CI_CamInst_t *)NULL;
	HUINT32 i, j;
	Handle_t hAction = HANDLE_NULL;
	HBOOL bFound = 0;

	s_stUriInfo[0].ucMode = pParam->stUriInfo.ucMode;
	s_stUriInfo[0].ucAps = pParam->stUriInfo.ucAps;
	s_stUriInfo[0].ucEmi = pParam->stUriInfo.ucEmi;
	s_stUriInfo[0].ucIct = pParam->stUriInfo.ucIct;
	s_stUriInfo[0].ucRct = pParam->stUriInfo.ucRct;
	s_stUriInfo[0].ucDot = pParam->stUriInfo.ucDot;
	s_stUriInfo[0].ulMinutesForRL = pParam->stUriInfo.ulMinutesForRL;

	pCam = &s_astCiCam[usSlotId];
	if(pCam == NULL)
	{
		return ERR_FAIL;
	}

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
				CASMGR_UpdateDrmInfo(hAction, pCam->hCasDevice, (void *)&s_stUriInfo[0]);
				s_astCiContext[i].bRcvUri = TRUE;
			}
			else
			{
				CASMGR_UpdateDrmInfo(hAction, pCam->hCasDevice, (void *)NULL);
			}
		}
	}

	s_bUriSent = TRUE;
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
}
#endif	//#if defined(CONFIG_DOUGLAS)

	return nResult;

}

STATIC HERROR local_cimgr_ProcSrmControl(CI_PLUS_SRM_CONTROL_PARAM *pParam) // Apply SRM data
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

	/* <TODO_CI_PLUS> : Apply SRM data (HDCP ��⿡ �Ѱܶ�) */

	OxCAS_Free(pParam->pucSrmData);

	return nResult;
}

STATIC HERROR local_cimgr_ProcUpdateCcResource(CI_UPDATE_CA_INST_PARAM *pUpdateCcRes)
{
	CI_CamInst_t *pCam = (CI_CamInst_t *)NULL;
	CI_Context_t *pCiContext = (CI_Context_t *)NULL;
	HUINT32 i = 0;

//	HERROR nResult = ERR_FAIL;

/*
		1. cas --> cc �� ���.
		2. cc --> cas�� ���.

		1���� ��� start descramble�� �ѹ� �� ���ش�.
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
		1. ���� slot id�� ����� ci context Ȯ��.
		2. pmt, set dsc ���� Ȯ��.
		3. prog info ���� set key �Ѵ�.
	*/
	/* cam instance �� slot id�� ���� �ȴ�. */
	pCam = &s_astCiCam[pUpdateCcRes->usSlotId];

#if 0
	for(i=0; i<CI_MAX_NUM_OF_ACT; i++)
	{
		if (s_astCiContext[i].pCam == pCam)
		{
			pCiContext = &s_astCiContext[i];
			break;
		}
	}

	if (pCiContext == NULL)
	{
		HxLOG_Print(" \n");
		return ERR_OK;
	}

	if (pCiContext->hAction == HANDLE_NULL)
	{
		HxLOG_Print(" \n");
		return ERR_OK;
	}

	if(pCam->usCaInfoCnt != 0)
	{
		HxLOG_Print(" cam cas resource availble & action connect with cam \n");
		local_cimgr_ProcDscSetForCiPlus(pCam, pCiContext->ulDemuxId, 0, 0);
	}
	else
	{
		HxLOG_Print(" cam cas resource not availble  \n");
	}
#endif

	if(pCam->usCaInfoCnt == 0)
	{
		HxLOG_Print(" usCaInfoCnt ZERO \n");
		return ERR_OK;
	}

	for(i=0; i<CI_MAX_NUM_OF_ACT; i++)
	{
		if (s_astCiContext[i].pCam == pCam)
		{
			pCiContext = &s_astCiContext[i];
			if (pCiContext->hAction == HANDLE_NULL)
			{
				HxLOG_Print(" \n");
				continue;
			}

			local_cimgr_ProcDscSetForCiPlus(pCam, pCiContext->ulDemuxId, 0, 0, 0);
			break;
		}
	}

	/**
	  *		Neotion CAM�� ���, CAS -> CC �� ������ resource open�Ǵµ�,
	  *		�� ��� start descramble�� �ϴ� shun condition check���� �׻� shun enable�� �Ǵ� ������ �߻��� (���� CC�� open���� �ʾ����Ƿ� )
	  *		�̸� �������� delayed cas update�� ����� ���� ������,
	  *		���⼭�� CC update�Ǵ� �� �������� �ٽ� �ѹ� shun condition check�� ����
	**/
	if ((pCiContext == NULL) || (pCiContext->pCam == NULL))
	{
		HxLOG_Print(" CC resoure updated! No ci context => skip shun condition check   \n");
	}
	else if (pCiContext->eChType != eCasChannel_FTA)
	{
		CAS_CiDscInfo_t *pDscInfo = NULL;
		HBOOL bIsNeedShun = FALSE;

		/**
		  *	cam ���� ���� UriDefault�� Failure �� ��� �ٽ� �ѹ� ���־�� ��.
		  *	Current ch type check�� ���ؼ� ����� �̵���.
		**/
		HxLOG_Print(" set_uri_default (%d)  \n", pCam->usSlotId);
		CI_SetUriDefault(pCam->usSlotId, TRUE);

		/**
		  *	check shun condition
		**/
		if (pCam->usSlotId != pCiContext->pCam->usSlotId)
		{
			HxLOG_Print("invalid slot id - pCam->usSlotId(%d) !=  pCiContext->pCam->usSlotId(%d) \n", pCam->usSlotId, pCiContext->pCam->usSlotId);
		}
		else
		{
			if(local_cimgr_GetCiDescInfo(pCiContext, &pDscInfo) != ERR_OK)
			{
				HxLOG_Print("get description info failed\n");
			}
			else
			{
				if(local_cimgr_ProcCheckShunCondition(pCiContext->pCam->usSlotId, pDscInfo, &bIsNeedShun) != ERR_OK)
				{
					HxLOG_Error("\n");
				}
				else
				{
					if (bIsNeedShun == TRUE)
					{
#if defined(_USE_SHUNNING_STATE_)
						pCiContext->eShunState = eCiPlus_ShunnedFlag_Enable;
						HxLOG_Info("Shun State flag  On\n");
#endif
						pCiContext->bSvcShunned = TRUE;

						CI_SetPlusMode(pCiContext->pCam->usSlotId, CI_PLUS_PROTECTED_MODE);
						svc_cas_DevCiSetTsPath(pCiContext->pCam->usSlotId, eCAS_DEV_CI_BYPASS);

						// �߿� log
						HxLOG_Error("Shunning enable, Set Bypass\n");
					}
					else
					{
#if defined(_USE_SHUNNING_STATE_)
						pCiContext->eShunState = eCiPlus_ShunnedFlag_Disable;
						HxLOG_Info("Shun State flag  Off\n");
#endif
						pCiContext->bSvcShunned = FALSE;

						CI_SetPlusMode(pCiContext->pCam->usSlotId, CI_PLUS_FREE_MODE);
						svc_cas_DevCiSetTsPath(pCiContext->pCam->usSlotId, eCAS_DEV_CI_PASSTHROUGH);

						// �߿� log
						HxLOG_Error("Shunning Disable, Set Passthru\n");
					}
				}
			}
		}
	}

	return ERR_OK;
}

STATIC HERROR local_cimgr_ProcDscSetForCiPlus(
												CI_CamInst_t *pCam,
												HUINT32 ulDemuxId,
												HBOOL bIsUpdate,
												HBOOL bIsRetry,
												HBOOL bCalledByStartDescramble /* �̰� ����� �뵵 */)
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

		start descrambler ���İ� �� ���̴�.
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

	/*	��ũ���� Open / SetPid / SetKey :

		���� ��ũ������ �����ϱ� ���� ������ available ���� Ȯ���Ѵ�.
		���⼭ available ���� ���� ���� CIv1 ķ�̰ų� ���� ķ�� authentication ���μ����� ������ ���� ���¿��� CCK �� ���� �������� ���� ����̴�.
		CIv1 ķ�̸� ���� ó���� �ʿ� ����,
		���� ķ�� authentication ���μ����� ������ ���� ���¸�...
		1. ķ ���Խ� CC -> CAS ������ ���ҽ��� ���µǴ� ķ������ �� ��Ȳ�� �߻����� �ʰ�,
		2. ķ ���Խ� CAS -> CC ������ ���ҽ��� ���µǴ� ķ������ authentication ���μ����� ������ �������� eCAS_DEV_EVT_CamPlusInfo �� �߻��Ǿ� ó���� �ȴ�.
		���� ��������� nego �� cipher ������ ���������� ���� ���� ó���� �ʿ� ����. */
	if ((pCam->bKeyInfoAvailable) || (CI_GetNegotiatedCckCipher(pCam->usSlotId, &enNegotiatedCckCipher) == ERR_OK))
	{
		if (pCam->bKeyInfoAvailable) // �̹� Ű ������ ���� ��Ȳ
		{
			eDscType = pCam->eDscType;

			if (eDscType != eCAS_DSC_TYPE_DES_CLEAR && eDscType != eCAS_DSC_TYPE_AES_CLEAR)
			{
				HxLOG_Error("Invalid enKeyCipher (%d)\n", eDscType);
			}
		}
		else // ķ ���� �� CI+ ķ�� Ȯ�ε� ���¿��� ���� Ű ������ ���� ���� ��Ȳ
		{
			HxLOG_Print("enNegotiatedCckCipher (%d) \n", enNegotiatedCckCipher);

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
		HxLOG_Print(" CCK Not Avaliable || CI_GetNegotiatedCckCipher is Failure \n");
		return ERR_OK;
	}

	/*
		1. descrambler open
		2. pid setting.
		3. key�� �����ϸ�, key setting.
	*/
	if(bIsUpdate != TRUE)
	{
		HxLOG_Error("<< Start >>\n");

		if(pCam->ulNumOfDsc[ulDemuxId] != 0)
		{
			// �Ʒ��� �ϴ� ���� ��ġ...
			HxLOG_Error("slot %d's descrambler NOT closed (%d) - %s !!\n", pCam->usSlotId, pCam->ulNumOfDsc[ulDemuxId], bCalledByStartDescramble ? "ByStartDescr" : "ByUpdateCcRes");

			for (i = 0; i < pCam->ulNumOfDsc[ulDemuxId]; i++)
			{
				nResult = svc_cas_DevDscClose(pCam->stDscInfo[ulDemuxId][i].ulDscId);
				pCam->stDscInfo[ulDemuxId][i].ulDscId = CAS_RESOURCE_NONE;
				if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
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
				// ���� ��ũ������ ����ϴ� ����� ��Ʈ�� Ÿ�� ������ don't care �̱� ������ �׳� ePAL_DSC_DATATYPE_VIDEOPES �� ������.
				nResult = svc_cas_DevDscOpen(ulDemuxId, eDscType, eCAS_DSC_DATA_TYPE_VIDEO_PES, &pCam->stDscInfo[ulDemuxId][ulNumOfDesc].ulDscId);
				if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

				pCam->stDscInfo[ulDemuxId][ulNumOfDesc].usPid = pCam->PidInfoForCaPmt[i].usPid;

				nResult = svc_cas_DevDscSetPid(pCam->stDscInfo[ulDemuxId][ulNumOfDesc].ulDscId, pCam->stDscInfo[ulDemuxId][ulNumOfDesc].usPid);
				if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

				HxLOG_Print("Dmx Id (%d), Desc Id (%d), Pid (%x)\n",
					ulDemuxId, pCam->stDscInfo[ulDemuxId][ulNumOfDesc].ulDscId, pCam->stDscInfo[ulDemuxId][ulNumOfDesc].usPid);

				ulNumOfDesc++;
			}
			else
			{
				HxLOG_Error("see CAS_MAX_NUM_OF_ES / MAX_NUM_OF_DSC_ID / MAX_DESC_CH_NUM (DI_DSC_SLOTNUM_MAXIMUM) !!\n");
				break; // �ϴ� ���� ������ ���ҽ����̶� ó���� �� �ֵ��� ���� ���������� �ʰ� �Ѵ�...
			}
		}

		pCam->ulNumOfDsc[ulDemuxId] = ulNumOfDesc;

		if (pCam->bKeyInfoAvailable)
		{
			#if 0 // org : CI+ �������� Ű ������ ��û�� -> �Ʒ��� ���� CI mgr ���� ��ü ó���� �� �ֵ��� ������.
			CI_ReqSetCckToDsc(usSlotId);
			#else // ���õ� ��ũ�����鿡 ���� SetKey
			for (i = 0; i < pCam->ulNumOfDsc[ulDemuxId]; i++)
			{
				if (pCam->stDscInfo[ulDemuxId][i].ulDscId != CAS_RESOURCE_NONE)
				{
					nResult = svc_cas_DevDscSetKeyForCiPlus(pCam->stDscInfo[ulDemuxId][i].ulDscId, eDscType, pCam->eParity,
						pCam->ucCck, (eDscType == eCAS_DSC_TYPE_DES_CLEAR) ? NULL : pCam->ucCiv);
					if(ERR_OK!=nResult)
					{
						HxLOG_Critical("Critical Eror!!!\n");
					}
					else
					{
						HxLOG_Print("dsc set key for ciplus ok  : Dmx Id (%d), Desc Id (%d), eDscType (%d)\n",
							ulDemuxId, pCam->stDscInfo[ulDemuxId][i].ulDscId, eDscType);
					}
				}
				else
				{
					HxLOG_Critical("\n\n");
				}
			}
			#endif
		}

	}
	else
	{
		// stProgInfo �� ���ŵǾ� update �ϴ� ��� -> ��ũ���� ������ �����ؾ� �Ѵ�.
		// -> Ȥ�� �𸣴� bIsRetry ��ƾ�� �̸� Ÿ���� �߰���.

		HUINT32 i, j, cnt;
		HUINT32 ulNumOfDsc;
		HUINT32 ulDscId[CAS_MAX_NUM_OF_DSC];
		HUINT16 usEsPid[CAS_MAX_NUM_OF_DSC];

		if (bIsUpdate)
			HxLOG_Error("<< Update >>\n");
		if (bIsRetry)
			HxLOG_Print("<< Retry >>\n");

		if (pCam->ulNumOfDsc[ulDemuxId] == 0) // �÷ο� �� ���� �� ���� ���� Ȯ�� �ʿ� !!
		{
			HxLOG_Error("slot %d's descrambler NOT opened - %s !!\n", pCam->usSlotId, bCalledByStartDescramble ? "ByStartDescr" : "ByUpdateCcRes");
		}

		// ���ŵ� stProgInfo �� ���� ��� ���� PID ��� ���Ͽ� �����ǰų� �߰��� ���� ������ ó���� ���ش�.

		// 1. ���� ������ ���� �ִ��� Ȯ��...

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
				// ���� ��ũ������ ����ϴ� ����� ��Ʈ�� Ÿ�� ������ don't care �̱� ������ ��Ʈ�� Ÿ���� ������ ����.
				if (usEsPid[i] == pCam->PidInfoForCaPmt[j].usPid)
					break;
			}
			if (j == pCam->ulNumOfPidForCaPmt)
			{
				HxLOG_Error("EsPid 0x%x will be deleted\n", usEsPid[i]);

				nResult = svc_cas_DevDscClose(ulDscId[i]);
				ulDscId[i] = CAS_RESOURCE_NONE;
				if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

				pCam->ulNumOfDsc[ulDemuxId]--;
			}
		}

		cnt = 0;
		for (i = 0; i < CAS_MAX_NUM_OF_DSC; i++) // ���� �� �׿� ���� ��ũ���� ���� ��� �Ϸ�Ǿ��� ���� ���ҽ� ������ ������...
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

		// 2. �߰��� ���� �ִ��� Ȯ��...

		ulNumOfDsc = pCam->ulNumOfDsc[ulDemuxId];

		for (i = 0; i < pCam->ulNumOfPidForCaPmt; i++)
		{
			for (j = 0; j < ulNumOfDsc; j++)
			{
				// ���� ��ũ������ ����ϴ� ����� ��Ʈ�� Ÿ�� ������ don't care �̱� ������ ��Ʈ�� Ÿ���� ������ ����.
				if (pCam->PidInfoForCaPmt[i].usPid == usEsPid[j])
				{
					HxLOG_Print("same pid previous one \n");
					break;
				}
			}
			if (j == ulNumOfDsc)
			{
				HxLOG_Error("EsPid 0x%x will be added\n", pCam->PidInfoForCaPmt[i].usPid);

				ulNumOfDesc = pCam->ulNumOfDsc[ulDemuxId];
				if (ulNumOfDesc < CAS_MAX_NUM_OF_DSC)
				{
					// ���� ��ũ������ ����ϴ� ����� ��Ʈ�� Ÿ�� ������ don't care �̱� ������ �׳� ePAL_DSC_DATATYPE_VIDEOPES �� ������.
					nResult = svc_cas_DevDscOpen((HUINT32)ulDemuxId, eDscType, eCAS_DSC_DATA_TYPE_VIDEO_PES, &pCam->stDscInfo[ulDemuxId][ulNumOfDesc].ulDscId);
					if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

					pCam->stDscInfo[ulDemuxId][ulNumOfDesc].usPid = pCam->PidInfoForCaPmt[i].usPid;

					HxLOG_Print(" pid of ca pmt (0x%x) \n", pCam->PidInfoForCaPmt[i].usPid);

					nResult = svc_cas_DevDscSetPid(pCam->stDscInfo[ulDemuxId][ulNumOfDesc].ulDscId, pCam->stDscInfo[ulDemuxId][ulNumOfDesc].usPid);
					if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");

					HxLOG_Print("Dmx Id (%d), Desc Id (%d), Pid (%x)\n",
						ulDemuxId, pCam->stDscInfo[ulDemuxId][ulNumOfDesc].ulDscId, pCam->stDscInfo[ulDemuxId][ulNumOfDesc].usPid);

					ulNumOfDesc++;
				}
				else
				{
					HxLOG_Error("see CAS_MAX_NUM_OF_ES / MAX_NUM_OF_DSC_ID / MAX_DESC_CH_NUM !!\n");
					break; // �ϴ� ���� ������ ���ҽ����̶� ó���� �� �ֵ��� ���� ���������� �ʰ� �Ѵ�...
				}

				pCam->ulNumOfDsc[ulDemuxId] = ulNumOfDesc;
			}
		}

		if (pCam->bKeyInfoAvailable) // ���� ��ũ������ �ɸ��� ���ʹ� �޸� ���⼭�� ��� �̸� üũ�� �ʿ�� ���� ��...
		{
			#if 0 // org : CI+ �������� Ű ������ ��û�� -> �Ʒ��� ���� CI mgr ���� ��ü ó���� �� �ֵ��� ������.
			CI_ReqSetCckToDsc(usSlotId);
			#else // ���õ� ��ũ�����鿡 ���� SetKey
			for (i = 0; i < pCam->ulNumOfDsc[ulDemuxId]; i++)
			{
				if (pCam->stDscInfo[ulDemuxId][i].ulDscId != CAS_RESOURCE_NONE)
				{
					nResult = svc_cas_DevDscSetKeyForCiPlus(pCam->stDscInfo[ulDemuxId][i].ulDscId, eDscType, pCam->eParity,
						pCam->ucCck, (eDscType == eCAS_DSC_TYPE_DES_CLEAR) ? NULL : pCam->ucCiv);
					if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
				}
				else
				{
					HxLOG_Critical("\n\n");
				}

//				nResult = svc_cas_DevDscSetKeyForCiPlus(pCam->stDscInfo[i].ulDscId, eDscType,
//					pCam->eParity, pCam->ucCck, (eDscType == eCAS_DSC_TYPE_DES_CLEAR) ? NULL : pCam->ucCiv);
//				if(ERR_OK!=nResult)			HxLOG_Critical("Critical Eror!!!\n");
			}
			#endif
		}

	}


	return ERR_OK;
}
#endif // #if defined(CONFIG_MW_CI_PLUS)

STATIC HINT32 local_cimgr_ProcStreamStateChanged(CI_STREAM_STATE_CHANGED_PARAM *pStreamStChanged)
{
	/*
		1. channel�� descramble
	*/
	return ERR_OK;
}

////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_RETRY_SERVICE___
////////////////////////////////////////////////////////////

STATIC HERROR local_cimgr_StartRetrySvc(CI_Context_t *pCiContext)
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

	HxLOG_Print("retry -- hSvc[0x%08x], hAction[0x%08x]\n", retrySvc.hSvc, retrySvc.hAction);

	if (VK_TIMER_EventEvery(CIMGR_RETRY_PERIOD, local_cimgr_CallbackRetrySvc, (void *)&retrySvc, sizeof(CI_RETRY_SVC_PARAM), &pCiContext->ulRetryTimerId) != VK_OK)
	{
		pCiContext->ulRetryTimerId = INVALID_TIMER_ID;
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC HERROR local_cimgr_StopRetrySvc(CI_Context_t *pCiContext)
{
	HxLOG_Info("\n");

	if (pCiContext == NULL)
	{
		HxLOG_Error("invalid context\n");
		return ERR_FAIL;
	}

	if (pCiContext->ulRetryTimerId == INVALID_TIMER_ID)
	{
		return ERR_OK;
	}

	if (VK_TIMER_Cancel(pCiContext->ulRetryTimerId) != VK_OK)
	{
		HxLOG_Error("\n");
	}

	pCiContext->ulRetryTimerId = INVALID_TIMER_ID;

#if defined(_LIMIT_CI_RETRY_COUNT_)
	pCiContext->ucRetryCnt = 0;
#endif

	return ERR_OK;

}

////////////////////////////////////////////////////////////
#define ___LOCAL_FUNCS_FOR_TASK___
////////////////////////////////////////////////////////////

STATIC HERROR local_cimgr_InitModule(void)
{
	CI_NOTIFY_FUNC_LIST notifyList;
#if defined(CONFIG_MW_CI_PLUS)
	CIPLUS_NOTIFY_FUNC_LIST notifyPlusList;
#endif
	SvcCas_CbFuncList_t cbList;
	Handle_t hCasDevice = HANDLE_NULL;
	HERROR nResult = ERR_FAIL;
	HUINT32 i = 0;

	HxLOG_Info("\n");

	/* CI mgr ���� ����ϴ� �۷ι�, ���� ���� �ʱ�ȭ */
	HxSTD_memset(g_aucIsCaSysId, 0, sizeof(HUINT8) * CARM_MAX_CISLOT_NUMBER);

	local_cimgr_InitCamInsts();

	local_cimgr_InitContexts();

	/* �ݹ� �Լ� ��� (CI mgr <-> CAS mgr) */

	HxSTD_memset(&cbList, 0x00, sizeof(SvcCas_CbFuncList_t));
	cbList.pfnCbStartSession = local_cimgr_ReqStartSession;
	cbList.pfnCbStopSession = local_cimgr_ReqStopSession;
	//cbList.pfnCbSetDSC = local_cimgr_ReqSetDsc;
	cbList.pfnCbSetDSC = NULL;
	cbList.pfnCbPauseSession = local_cimgr_ReqPauseSession;
	cbList.pfnCbResumeSession = local_cimgr_ReqResumeSession;
	cbList.pfnCbUpdatePmt = local_cimgr_ReqUpdatePmt;
#if defined(CONFIG_MW_CI_PLUS)
	cbList.pfnCbUpdateSdt = local_cimgr_ReqUpdateSdt;
#endif
	cbList.pfnCbInstSelected = local_cimgr_ReqManageInstance;

	for (i=0; i<svc_cas_DevCiGetNumberOfModule(); i++)
	{
		nResult = CASMGR_RegisterSubCasCb(eCasInst_CI, eDxCAS_GROUPID_CAM, &cbList, &hCasDevice);
		if (nResult != ERR_OK)
		{
			HxLOG_Error("\n");
			return ERR_FAIL;
		}

		s_astCiCam[i].hCasDevice = hCasDevice; // Instance Table Index
		HxLOG_Print("Slot Id (%d), Cam Instance (%d) + Cas Instance (%d)\n", i, i, hCasDevice);
	}

	/* �ݹ� �Լ� ��� (CI mgr <-> CI stack) */

	HxSTD_memset(&notifyList, 0, sizeof(CI_NOTIFY_FUNC_LIST));

	notifyList.pfnNotifyCamState = local_cimgr_ReqUpdateCamState; // �̰� CI mgr �� notify �ϱ� ���� �뵵.
	notifyList.pfnNotifyCaInfo = local_cimgr_ReqUpdateCaInst;
#if defined(CI_PLUS_PVR_ENABLE)
	notifyList.pfnNotifyCaPvrInfo = local_cimgr_ReqUpdateCaPvrInst;
#endif
#if 1 // #if defined(CONFIG_MW_CI_PLUS_PVR) : ������ CA PVR ������ ���� �߰��� �������� Multiple CA application selection procedure ���� ���� �ٸ� �뵵�� Ȯ���ؼ� �� ������ �����Ƿ� �Ϲ������� ������ ��.
	notifyList.pfnNotifyCasState = local_cimgr_ReqUpdateCasState;
#endif

	CI_AddNotifyFunc(notifyList);

#if defined(CONFIG_MW_CI_PLUS)

	/* �ݹ� �Լ� ��� (CI mgr <-> CI+ stack) */

	HxSTD_memset(&notifyPlusList, 0, sizeof(CIPLUS_NOTIFY_FUNC_LIST));

	notifyPlusList.pfnNotifySetKey = local_cimgr_ReqSetKey;
	notifyPlusList.pfnNotifyUriControl = local_cimgr_ReqUriControl;
	notifyPlusList.pfnNotifySrmControl = local_cimgr_ReqSrmControl;
	notifyPlusList.pfnNotifyHostTune = local_cimgr_ReqHostTune;
	notifyPlusList.pfnNotifyPidReplace = local_cimgr_ReqPidReplace;
	notifyPlusList.pfnNotifyCcReport = local_cimgr_ReqUpdateCcResource; // �̰� CI mgr �� notify �ϱ� ���� �뵵.

#if defined(CONFIG_OP_CANALREADY)
	notifyPlusList.pfnNotifyCanalReadyInit = local_cimgr_ReqCanalReadyInitNotify;
	notifyPlusList.pfnNotifyCanalReadyMoralLevel = local_cimgr_ReqCanalReadyMoralityLevelNotify;
	notifyPlusList.pfnNotifyCanalReadyPinReset = local_cimgr_ReqCanalReadyMoralityPinReset;
#endif
#if defined(CONFIG_MW_CI_PLUS_CC_V_2)
	notifyPlusList.pfnNotifyCcPinStatus = local_cimgr_ReqUpdatePinStatus;
#endif


	CIPLUS_AddNotifyFunc(notifyPlusList);

	/* Host Service Shunning �� ���� �ʱ�ȭ */

	for(i = 0; i<CAS_MAX_NUM_OF_TS; i++)
	{
		local_cimgr_InitSdtStatus(i);
	}
#endif

	return ERR_OK;
}

STATIC void local_cimgr_Task(void)
{
	HUINT8 aucMsg[MW_CAS_CIMGR_MSG_SIZE];
	CI_RCV_EVENT eEvt;
	void *pvEvtParam;

	local_cimgr_InitModule();

	while (1)
	{
		if (VK_MSG_Receive(s_ulCiMgrMsgQId, aucMsg, MW_CAS_CIMGR_MSG_SIZE) != VK_OK)
		{
			HxLOG_Error("\n");
			continue;
		}

		HxSTD_memcpy(&eEvt, aucMsg, sizeof(CI_RCV_EVENT));
		pvEvtParam = &aucMsg[sizeof(CI_RCV_EVENT)];

		HxLOG_Print("Event [%s]\n", local_cimgr_Cmd2Str(eEvt));

		switch (eEvt)
		{

/*--- events for service ---*/

		case CI_START:
			local_cimgr_ProcStart((CI_ADD_SVC_PARAM *)pvEvtParam);
			break;

		case CI_SETPID:
			//local_cimgr_ProcSetDsc((CI_SETDSC_PARAM *)pvEvtParam);
			break;

		case CI_STOP:
			local_cimgr_ProcStop((CI_DELETE_SVC_PARAM *)pvEvtParam);
			break;

		case CI_RETRY:
			local_cimgr_ProcRetrySvc((CI_RETRY_SVC_PARAM *)pvEvtParam);
			break;

/*--- events for SI ---*/

		case CI_PMT_CHANGED:
			local_cimgr_ProcPmtChanged((CI_PMT_CHANGED_PARAM *)pvEvtParam);
			break;

#if defined(CONFIG_MW_CI_PLUS)
		case CI_SDT_CHANGED:
			local_cimgr_ProcSdtChanged((CI_SDT_CHANGED_PARAM *)pvEvtParam);
			break;
#endif

/*--- events for instance management ---*/

		case CI_MANAGE_CA_INST:
			local_cimgr_ProcManageInstance((CI_MANAGE_CA_INST_PARAM *)pvEvtParam);
			break;

		case CI_UPDATE_CA_INST:
			local_cimgr_ProcUpdateCaInst((CI_UPDATE_CA_INST_PARAM *)pvEvtParam);
			break;

		case CI_ADD_DELAYED_CA_INST:
			local_cimgr_ProcAddDelayedCaInst((CI_UPDATE_CA_INST_PARAM *)pvEvtParam);
			break;

#if defined (CONFIG_MW_CI_PLUS)
		case CI_UPDATE_CC:
			local_cimgr_ProcUpdateCcResource((CI_UPDATE_CA_INST_PARAM *)pvEvtParam);
			break;
#endif

#if defined(CI_PLUS_PVR_ENABLE)
		case CI_UPDATE_CA_PVR_INST:
			local_cimgr_ProcUpdateCaPvrInst((CI_UPDATE_CA_PVR_INST_PARAM *)pvEvtParam);
			break;
#endif

/*--- events for CAM state ---*/

		case CI_UPDATE_CAM_STATE:
			local_cimgr_ProcUpdateCamState((CI_UPDATE_CAM_STATE_PARAM *)pvEvtParam);
			break;

/*--- events for CAS state ---*/

#if 1 // #if defined(CONFIG_MW_CI_PLUS_PVR) : ������ CA PVR ������ ���� �߰��� �������� Multiple CA application selection procedure ���� ���� �ٸ� �뵵�� Ȯ���ؼ� �� ������ �����Ƿ� �Ϲ������� ������ ��.
		case CI_UPDATE_CAS_STATE:
			local_cimgr_ProcUpdateCasState((CI_UPDATE_CAS_STATE_PARAM *)pvEvtParam);
			break;
#endif

/*--- events for CI+ ---*/

#if defined(CONFIG_MW_CI_PLUS)
		case CI_PLUS_KEY_CHANGED:
			local_cimgr_ProcKeyChanged((CI_PLUS_KEY_CHANGE_PARAM *)pvEvtParam);
			break;

		case CI_PLUS_URI_CONTROL:
			local_cimgr_ProcUriControl((CI_PLUS_URI_CONTROL_PARAM *)pvEvtParam);
			break;

		case CI_PLUS_SRM_CONTROL:
			local_cimgr_ProcSrmControl((CI_PLUS_SRM_CONTROL_PARAM *)pvEvtParam);
			break;

#if defined(CONFIG_OP_CANALREADY)
		case CI_PLUS_CANALREADY_INIT_NOTIFY:
			local_cimgr_ProcCanalReadyInitNotify((CI_PLUS_CANALREADY_INIT_INFO_PARAM*)pvEvtParam);
			break;
#endif	// CONFIG_OP_CANALREADY
#endif	// CONFIG_MW_CI_PLUS

/*--- events for stream state ---*/

		case CI_STREAM_STATE_CHANGED:
			local_cimgr_ProcStreamStateChanged((CI_STREAM_STATE_CHANGED_PARAM *)pvEvtParam);
			break;

		default:
			break;
		}

	}
}

////////////////////////////////////////////////////////////
#define ___PUBLIC_FUNCS___
////////////////////////////////////////////////////////////

HERROR xsvc_cas_CiMgrInit(void)
{
	HxLOG_Info("\n");

	if (VK_MSG_Create(MW_CAS_CIMGR_MSGQ_SIZE, MW_CAS_CIMGR_MSG_SIZE, "qCIMGR", &s_ulCiMgrMsgQId, VK_SUSPENDTYPE_PRIORITY) != VK_OK)
	{
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	if (VK_TASK_Create((void*)local_cimgr_Task, MW_CAS_CIMGR_TASK_PRIORITY, MW_CAS_CIMGR_TASK_STACK_SIZE, "tCIMGR", NULL, &s_ulCiMgrTaskId, 0) != VK_OK)
	{
		VK_MSG_Destroy(s_ulCiMgrMsgQId);
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	if (VK_TASK_Start(s_ulCiMgrTaskId) != VK_OK)
	{
		VK_TASK_Destroy(s_ulCiMgrTaskId);
		HxLOG_Error("\n");
		return ERR_FAIL;
	}

	/*
	cam�� ���� �� ���¿��� �����ϴ� ��쿡 ���ؼ� ó���ϴ� ����� ���� �ʿ�.
	driver���� callback�� ��ϵǱ� ���� interrupt�� �߻��� ��� ó���� simple�ϱ⿡
	mw���� ���� �� driver�� state�� Ȯ���� ó���� �ϵ��� �Ѵ�.
	*/
	CiAdapt_Init(); // �� �Լ��� ȣ��� ������ DRV_CI_Task() �Լ��� ��� waiting ��.

	/* _svc_cas_mgr_common.h ���� ������ app ci slot number */
	s_ulNumOfCiSlot = CAS_MAX_NUM_OF_CAM;

	return ERR_OK;
}

////////////////////////////////////////////////////////////
#define ___DEBUG_FUNCS___
////////////////////////////////////////////////////////////

STATIC CI_CamInst_t * local_cimgr_GetCamList(void)
{
	return s_astCiCam;
}

STATIC CI_Context_t * local_cimgr_GetContextList(void)
{
	return s_astCiContext;
}

void CI_PrintCAMInfo(void)
{
	CI_CamInst_t *pCam;
	HINT32 i, j;

	pCam = local_cimgr_GetCamList();

	HxLOG_Print("\n");
	HxLOG_Print("< CI CAM INST >");

	for (i = 0; i < svc_cas_DevCiGetNumberOfModule(); i++)
	{
		HxLOG_Print("\n");
		HxLOG_Print("pCam	: 0x%x\n", (HUINT32)&pCam[i]);

		HxLOG_Print("usSlotId	: %d\n", pCam[i].usSlotId);
		HxLOG_Print("usManufacturer	: 0x%04X\n", pCam[i].usManufacturer);
		HxLOG_Print("CaSysId (hex)	:");
		for (j = 0; j < pCam[i].usCaInfoCnt; j++)
			HxLOG_Print(" %04X", pCam[i].ausCaSysId[j]);

		HxLOG_Print("\n");
		HxLOG_Print("PidInfoForCaPmt (hex)	: %d\n", pCam[i].ulNumOfPidForCaPmt);
		for (j = 0; j < pCam[i].ulNumOfPidForCaPmt; j++)
		{
			HxLOG_Print(" %04X(%d)", pCam[i].PidInfoForCaPmt[j].usPid, pCam[i].PidInfoForCaPmt[j].esType);
		}
#if defined(CI_PLUS_PVR_ENABLE)
		HxLOG_Print("\n");
		HxLOG_Print("CaPvrCaSysId   :");
		for (j = 0; j < pCam[i].usCaPvrInfoCnt; j++)
			HxLOG_Print(" %04X(%d)", pCam[i].ausCaPvrCaSysId[j], pCam[i].aucCaRecMode[j]);
#endif
		HxLOG_Print("hCasDevice		: 0x%x\n", pCam[i].hCasDevice);
		HxLOG_Print("caPmtOkDsc		:\n");
		for (j = 0; j < pCam[i].caPmtOkDsc.usRawLen; j++)
			HxLOG_Print(" %02X", pCam[i].caPmtOkDsc.pucRaw[j]);
		HxLOG_Print("\n");
#if defined(_Support_Not_Selected_CA_PMT_)
		HxLOG_Print("caPmtNotSel 	   :");
		for (j = 0; j < pCam[i].caPmtNotSel.usRawLen; j++)
			HxLOG_Print(" %02X", pCam[i].caPmtNotSel.pucRaw[j]);

		HxLOG_Print("\n");
#endif

#if defined(CONFIG_MW_CI_PLUS)
{
		HUINT32 k=0;
		for (k=0; k < CI_MAX_NUM_OF_DMX; k++)
		{
			if(pCam[i].ulNumOfDsc[k] != 0)
			{
				HxLOG_Print("\n Demux[%d] Opened Desc # : %d", k, pCam[i].ulNumOfDsc[k]);
				for (j = 0; j < pCam[i].ulNumOfDsc[k]; j++)
					HxLOG_Print(" [%d:%x]", pCam[i].stDscInfo[k][j].ulDscId, pCam[i].stDscInfo[k][j].usPid);

				HxLOG_Print("\n");
			}
		}
		HxLOG_Print("\n Is key alive ?		: %s", pCam[i].bKeyInfoAvailable ? "yes" : "no");
}
#endif
		HxLOG_Print("\n ulNumOfAct		: %d", pCam[i].ulNumOfAct);
		for (j = 0; j < CAS_MAX_NUM_OF_ACTION; j++)
		{
			if (pCam[i].hActList[j] != HANDLE_NULL)
				HxLOG_Print(" %08X", pCam[i].hActList[j]);
		}

		HxLOG_Print("\n\n");
	}
}

void CI_PrintContextInfo(void)
{
	CI_Context_t *pCiContext = NULL;
	HUINT32 i, j, k;

	pCiContext = local_cimgr_GetContextList();

	HxLOG_Print("\n");
	HxLOG_Print("< CI CONTEXT >");
	HxLOG_Print("\n");

	for (i=0; i<CI_MAX_NUM_OF_ACT; i++)
	{
		//HxLOG_Print("\n pCiContext  	: 0x%x", i, (HUINT32)&pCiContext[i]);

		if(pCiContext[i].hAction == HANDLE_NULL)
		{
			continue;
		}

		HxLOG_Print("\n hAction        : 0x%08x", pCiContext[i].hAction);
		HxLOG_Print("\n eActType       : %d", pCiContext[i].eActType);
		HxLOG_Print("\n hSvc           : 0x%08x", pCiContext[i].hSvc);
		HxLOG_Print("\n eState         : %d", pCiContext[i].eState);
		HxLOG_Print("\n bPathConnected : %d", pCiContext[i].bPathConnected);
		HxLOG_Print("\n bSendCaPmt	: %d", pCiContext[i].bSendCaPmt);

		if (pCiContext[i].stProgInfo.pPmtInfo == NULL)
		{
			HxLOG_Print("\n program-level  : pPmtInfo is null");
		}
		else
		{
			HxLOG_Print("\n program-level  : ServiceId(0x%x) PcrPid(0x%x) ProgramCaFlag(%d) #ProgramCaDesc(%d) #Element(%d)",
				pCiContext[i].stProgInfo.pPmtInfo->usServiceId,
				pCiContext[i].stProgInfo.pPmtInfo->usPcrPid,
				pCiContext[i].stProgInfo.pPmtInfo->bProgramCaFlag,
				pCiContext[i].stProgInfo.pPmtInfo->ulNumOfProgramCaDescriptor,
				pCiContext[i].stProgInfo.pPmtInfo->ulNumOfElement);
			for (k = 0; k < pCiContext[i].stProgInfo.pPmtInfo->ulNumOfProgramCaDescriptor; k++)
			{
				HxLOG_Print(" [%04X:%04X]",
					pCiContext[i].stProgInfo.pPmtInfo->stProgramCaDescriptor[k].usCaSysId, pCiContext[i].stProgInfo.pPmtInfo->stProgramCaDescriptor[k].usCaPid);
			}
		}

		HxLOG_Print("ucNumEs        : %d\n", pCiContext[i].stProgInfo.ucNumEs);
		for (j = 0; j < pCiContext[i].stProgInfo.ucNumEs; j++)
		{
			HxLOG_Print("element-level	: EsType(%d) EsPid(0x%x) \n",
				pCiContext[i].stProgInfo.aEsInfo[j].eEsType,
				pCiContext[i].stProgInfo.aEsInfo[j].usEsPid);

#if 0
			if (pCiContext[i].stProgInfo.aEsInfo[j].pEs == NULL)
			{
				HxLOG_Print("\n                : pEs is null");
			}
			else
			{
				HxLOG_Print("\n                : StreamType(0x%x) EsType(%d) Pid(0x%x) ElementCaFlag(%d) #ElementCaDesc(%d)",
					pCiContext[i].stProgInfo.aEsInfo[j].pEs->ucStreamType,
					pCiContext[i].stProgInfo.aEsInfo[j].pEs->eEsType,
					pCiContext[i].stProgInfo.aEsInfo[j].pEs->usPid,
					pCiContext[i].stProgInfo.aEsInfo[j].pEs->bElementCaFlag,
					pCiContext[i].stProgInfo.aEsInfo[j].pEs->ulNumOfCaDescriptor);
				for (k = 0; k < pCiContext[i].stProgInfo.aEsInfo[j].pEs->ulNumOfCaDescriptor; k++)
				{
					HxLOG_Print(" [%04X:%04X]",
						pCiContext[i].stProgInfo.aEsInfo[j].pEs->stElementCaDescriptor[k].usCaSysId, pCiContext[i].stProgInfo.aEsInfo[j].pEs->stElementCaDescriptor[k].usCaPid);
				}
			}
#endif
		}

		HxLOG_Print("pCam           : 0x%x\n", (HUINT32)pCiContext[i].pCam);
#if defined(CONFIG_MW_CI_PLUS)
		HxLOG_Print("eSdtTblState   : %d\n", pCiContext[i].eSdtTblState);
		HxLOG_Print("bSvcShunned    : %d\n", pCiContext[i].bSvcShunned);
#if defined(_USE_SHUNNING_STATE_)
		HxLOG_Print("eShunState     : %d\n", pCiContext[i].eShunState);
#endif
#endif
		HxLOG_Print("\n");
	}

	HxLOG_Print("\n\n");
}


HERROR CI_PrintSdtStatus(void)
{
	HUINT32 i = 0;
	POINTER_LIST_T *pList = NULL;
	CAS_CiDscInfo_t *pDsc = NULL;
	CAS_SdtStatus_t *pSdt = NULL;

	HxLOG_Print ("\n========================= print sdt status ============================= \n\n");

	pSdt = local_cimgr_GetSdtSlotByIndex(i);
	for(i=0; i<CAS_MAX_NUM_OF_TS; i++)
	{
		pSdt = local_cimgr_GetSdtSlotByIndex(i);
		if(pSdt == NULL)
		{
			continue;
		}

		if(pSdt->bUsed == CAS_USED)
		{
			HxLOG_Print("ulFilterId 			: %d \n", pSdt->ulFilterId);
			HxLOG_Print("ulActionId 			: %x \n", pSdt->ulActionId);
			HxLOG_Print("usTsId 				: %x \n", pSdt->usTsId);
			HxLOG_Print("usUniqueId 			: %x \n", pSdt->usUniqueId);
			HxLOG_Print("unDmxId 			: %d \n", pSdt->ulDemuxId);
			HxLOG_Print("usExtId 			: %x \n", pSdt->usExtId);
			HxLOG_Print("usPid 				: %x \n", pSdt->usPid);
			HxLOG_Print("eFilterMode 			: %d \n", pSdt->eFilterMode);
			HxLOG_Print("eHead 				: %d \n", pSdt->eHead);
			HxLOG_Print("ulTblState 			: %d \n", pSdt->ulTblState);
			HxLOG_Print("bUseVerNum 			: %d \n", pSdt->bUseVerNum);
			HxLOG_Print("bCrc 				: %d \n", pSdt->bCrc);
			HxLOG_Print("bUseExtId 			: %d \n", pSdt->bUseExtId);
			HxLOG_Print("bTableFilter 			: %d \n", pSdt->bTableFilter);

			HxLOG_Print("Number of Service Id 		: %d \n", pSdt->ulSvcNum);
			pList = pSdt->pstList;
			while(pList)
			{
				pDsc = (CAS_CiDscInfo_t *)pList->pvContents;
				HxLOG_Print("Service Id 			: %x \n", pDsc->usSvcId);

				pList = UTIL_LINKEDLIST_GetPrevPointer(pList);
			}

			HxLOG_Print("\n\n\n");
		}
	}

	HxLOG_Print ("\n===================== end of print sdt status ========================== \n");

	return ERR_OK;
}

void CI_PrintUriStatus(HUINT16 usSlotIdToCheck)
{
#if defined(CONFIG_DEBUG) && defined(CONFIG_MW_CI_PLUS)
	DxCopyrightControl_CiPlusUriInfo_t 	*pstUriInfo = &s_stUriInfo[usSlotIdToCheck];

	HxLOG_Print ("\n========================= print URI status ============================= \n\n");

	// Mode
	HxLOG_Print("\t s_stUriInfo = (%d) ", pstUriInfo->ucMode);
	switch(pstUriInfo->ucMode)
	{
		case CI_CC_URI_MODE_DEFAULT:		HxLOG_Print("CI_CC_URI_MODE_DEFAULT ");		break;
		case CI_CC_URI_MODE_TIMEOUT:		HxLOG_Print("CI_CC_URI_MODE_TIMEOUT ");		break;
		case CI_CC_URI_MODE_FROMCAM:	HxLOG_Print("CI_CC_URI_MODE_FROMCAM ");		break;

		default:
			HxLOG_Print("Invalid URI Mode ");
			break;
	}
	HxLOG_Print("\n");

	// APS
	HxLOG_Print("\t ucAps = (%d) ", pstUriInfo->ucAps);
	switch(pstUriInfo->ucAps)
	{
		case CI_CC_URI_APS_OFF:		HxLOG_Print("CI_CC_URI_APS_OFF ");		break;
		case CI_CC_URI_APS_ON_1:		HxLOG_Print("CI_CC_URI_APS_ON_1 ");		break;
		case CI_CC_URI_APS_ON_2:		HxLOG_Print("CI_CC_URI_APS_ON_2 ");		break;
		case CI_CC_URI_APS_ON_3:		HxLOG_Print("CI_CC_URI_APS_ON_3 ");		break;

		default:
			HxLOG_Print("Invalid APS ");
			break;
	}
	HxLOG_Print("\n");

	// EMI
	HxLOG_Print("\t ucEmi = (%d) ", pstUriInfo->ucEmi);
	switch(pstUriInfo->ucEmi)
	{
		case CI_CC_URI_EMI_COPY_FREE:			HxLOG_Print("CI_CC_URI_EMI_COPY_FREE ");		break;
		case CI_CC_URI_EMI_COPY_NO_MORE:		HxLOG_Print("CI_CC_URI_EMI_COPY_NO_MORE ");	break;
		case CI_CC_URI_EMI_COPY_ONCE:		HxLOG_Print("CI_CC_URI_EMI_COPY_ONCE ");		break;
		case CI_CC_URI_EMI_COPY_NEVER:		HxLOG_Print("CI_CC_URI_EMI_COPY_NEVER ");		break;

		default:
			HxLOG_Print("Invalid EMI ");
			break;
	}
	HxLOG_Print("\n");

	// ICT
	HxLOG_Print("\t ucIct = (%d) ", pstUriInfo->ucIct);
	switch(pstUriInfo->ucIct)
	{
		case CI_CC_URI_ICT_OFF:			HxLOG_Print("CI_CC_URI_ICT_OFF ");			break;
		case CI_CC_URI_ICT_ON:			HxLOG_Print("CI_CC_URI_ICT_ON ");			break;

		default:
			HxLOG_Print("Invalid ICT ");
			break;
	}
	HxLOG_Print("\n");

	// RCT
	HxLOG_Print("\t ucRct = (%d) ", pstUriInfo->ucRct);
	switch(pstUriInfo->ucRct)
	{
		case CI_CC_URI_RCT_OFF:			HxLOG_Print("CI_CC_URI_RCT_OFF ");		break;
		case CI_CC_URI_RCT_ON:			HxLOG_Print("CI_CC_URI_RCT_ON ");			break;

		default:
			HxLOG_Print("Invalid RCT ");
			break;
	}
	HxLOG_Print("\n");

	// DOT
	HxLOG_Print("\t ucDot = (%d) ", pstUriInfo->ucDot);
	switch(pstUriInfo->ucDot)
	{
		case CI_CC_URI_DOT_OFF:			HxLOG_Print("CI_CC_URI_DOT_OFF ");		break;
		case CI_CC_URI_DOT_ON:			HxLOG_Print("CI_CC_URI_DOT_ON ");			break;

		default:
			HxLOG_Print("Invalid DOT ");
			break;
	}
	HxLOG_Print("\n");

	// Minutes For RL
	HxLOG_Print("\t ulMinutesForRL = (%ld) \n", pstUriInfo->ulMinutesForRL);

	HxLOG_Print("\n\n");
#endif /* #ifdef CONFIG_DEBUG */

	return;
}

#if defined(CONFIG_DEBUG) && defined(CONFIG_MW_CI_PLUS)
void CI_Debug_SetUriStatus(void *pstVoidUriInfo)
{
	CIPLUS_UriInfo_t *pstUriInfo = (CIPLUS_UriInfo_t*)pstVoidUriInfo;

	if (pstUriInfo != NULL)
	{
		local_cimgr_ReqUriControl(0, pstUriInfo);
	}

	return;
}
#endif /* #ifdef CONFIG_DEBUG */

STATIC const HINT8 * local_cimgr_Cmd2Str(CI_RCV_EVENT cmd)
{
	switch (cmd)
	{
		ENUM_TO_STR(CI_START);

		ENUM_TO_STR(CI_SETPID);
		ENUM_TO_STR(CI_STOP);

		ENUM_TO_STR(CI_PAUSE);
		ENUM_TO_STR(CI_RESUME);

		ENUM_TO_STR(CI_RETRY);

		ENUM_TO_STR(CI_PMT_CHANGED);
		ENUM_TO_STR(CI_SDT_CHANGED);

		ENUM_TO_STR(CI_MANAGE_CA_INST);

		ENUM_TO_STR(CI_UPDATE_CA_INST);
		ENUM_TO_STR(CI_UPDATE_CC);
		ENUM_TO_STR(CI_ADD_DELAYED_CA_INST);

#if defined(CI_PLUS_PVR_ENABLE)
		ENUM_TO_STR(CI_UPDATE_CA_PVR_INST);
#endif

		ENUM_TO_STR(CI_UPDATE_CAM_STATE);

#if 1 // #if defined(CONFIG_MW_CI_PLUS_PVR) : ������ CA PVR ������ ���� �߰��� �������� Multiple CA application selection procedure ���� ���� �ٸ� �뵵�� Ȯ���ؼ� �� ������ �����Ƿ� �Ϲ������� ������ ��.
		ENUM_TO_STR(CI_UPDATE_CAS_STATE);
#endif

#if defined(CONFIG_MW_CI_PLUS)
		ENUM_TO_STR(CI_PLUS_KEY_CHANGED);
		ENUM_TO_STR(CI_PLUS_URI_CONTROL);
		ENUM_TO_STR(CI_PLUS_SRM_CONTROL);
#endif

#if defined(CONFIG_OP_CANALREADY)
		ENUM_TO_STR(CI_PLUS_CANALREADY_INIT_NOTIFY);
#endif

		ENUM_TO_STR(CI_STREAM_STATE_CHANGED);

		ENUM_TO_STR(EVT_CIREV_MAX);
	}

	return ("local_cimgr_Cmd2Str : Unknown state");
}

