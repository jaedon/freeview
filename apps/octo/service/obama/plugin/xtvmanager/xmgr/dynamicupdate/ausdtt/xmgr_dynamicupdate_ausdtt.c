/**
	@file     xmgr_dynamicupdate_ausdtt.c
	@brief    file_name & simple comment.

	Description: \n
	Module: \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n


	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <otl.h>

#include <mgr_common.h>
#include <svc_pipe.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <svc_si.h>

#include <xsvc_si.h>

#include <mgr_search.h>

#include "../local_include/_xmgr_dynamicupdate.h"
#include <_mgr_dynamicupdate.h>

#include <db_svc.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagDynamicUpdateContent_Ausdtt
{
	DynamicUpdate_Content_t		stBaseContents;

	// 향별 추가되는 filed는 여기에. 밑으로.
	DxSvcType_e					 eServiceTypeOption;

	HUINT32						ulPrvNum;
	HUINT32						ulSvcNum;
	HBOOL						bNewTs;

	DbSvc_PrvInfo_t 				*pstPrvArray;
	DbSvc_TsInfo_t				*pstTsInfo;
	DbSvc_NetInfo_t 				*pstNetInfo;
	DbSvc_Info_t					*pstSvcArray;
}DynamicUpdate_Content_Ausdtt_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC DynamicUpdate_Content_Ausdtt_t	s_stDynamicUpdateContent_Ausdtt;


/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
STATIC BUS_Result_t	proc_dynamicupdate_Ausdtt(HINT32 lMessage, Handle_t hAction, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
STATIC HERROR		xmgr_DynamicUpdate_InitContext_Ausdtt(DynamicUpdate_Content_Ausdtt_t *pstContents);
INLINE STATIC DynamicUpdate_Content_Ausdtt_t *xmgr_DynamicUpdate_GetContent_Ausdtt (void);


#define	________________Static_Function_________________________

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

INLINE STATIC DynamicUpdate_Content_Ausdtt_t *xmgr_DynamicUpdate_GetContent_Ausdtt (void)
{
	return &s_stDynamicUpdateContent_Ausdtt;
}

STATIC HERROR	xmgr_DynamicUpdate_InitContext_Ausdtt(DynamicUpdate_Content_Ausdtt_t *pstContents)
{
	HERROR	hErr = ERR_FAIL;

	HxLOG_Trace();

	if(pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL \n");
		return ERR_FAIL;
	}

	if(pstContents->pstNetInfo != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstNetInfo);
		pstContents->pstNetInfo = NULL;
	}

	if(pstContents->pstTsInfo != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstTsInfo);
		pstContents->pstTsInfo = NULL;
	}

	if(pstContents->pstPrvArray != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstPrvArray);
		pstContents->pstPrvArray = NULL;
	}

	if(pstContents->pstSvcArray != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstSvcArray);
		pstContents->pstSvcArray = NULL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(DynamicUpdate_Content_Ausdtt_t));

	pstContents->eServiceTypeOption = eDxSVC_TYPE_All;

	pstContents->stBaseContents.stDynamicUpdateSpec.eActionKind = eSVCSI_ACT_KIND_DynamicUpdate;
	pstContents->stBaseContents.stDynamicUpdateSpec.ulActionSpec = eSVCSI_ACT_SPEC_AusDtt;

	(void)xmgr_dynamicupdate_checkOnOff(&(pstContents->stBaseContents));

	return hErr;
}


#define DYNAMIC_UPDATE_TER_SIGNAL_QUALITY_THRESHOLD_AUSDTT		5
#define DYNAMIC_UPDATE_TER_SIGNAL_LEVEL_THRESHOLD_AUSDTT			5
STATIC HERROR xmgr_dynamicupdate_IsSignalStrongerThanDbTsInfo_Ausdtt (DynamicUpdate_Content_Ausdtt_t* pstContents, DbSvc_TsInfo_t *pstNewTsInfo, DbSvc_TsInfo_t *pstDbTsInfo, HBOOL *pbStronger)
{
	DbSvc_TerTuningInfo_t		*pstNewTuneInfo, *pstDbTuneInfo;

	pstNewTuneInfo	= &(pstNewTsInfo->stTuningParam.ter);
	pstDbTuneInfo	= &(pstDbTsInfo->stTuningParam.ter);

	// Quality 가 높으면 TRUE
	if ((HUINT32)pstNewTuneInfo->ucQuality > ((HUINT32)pstDbTuneInfo->ucQuality + DYNAMIC_UPDATE_TER_SIGNAL_QUALITY_THRESHOLD_AUSDTT))
	{
		*pbStronger = TRUE;
		return ERR_OK;
	}

	// Quality 가 낮으면 FALSE
	if (((HUINT32)pstNewTuneInfo->ucQuality + DYNAMIC_UPDATE_TER_SIGNAL_QUALITY_THRESHOLD_AUSDTT) > (HUINT32)pstDbTuneInfo->ucQuality)
	{
		*pbStronger = FALSE;
		return ERR_OK;
	}

	// Quality가 거의 동일
	// Level 이 높으면 TRUE
	if ((HUINT32)pstNewTuneInfo->ucLevel > ((HUINT32)pstDbTuneInfo->ucLevel + DYNAMIC_UPDATE_TER_SIGNAL_LEVEL_THRESHOLD_AUSDTT))
	{
		*pbStronger = TRUE;
		return ERR_OK;
	}

	// 그 외에는 FALSE
	*pbStronger = FALSE;

	return ERR_OK;
}

INLINE STATIC HBOOL xmgr_dynamicupdate_isIdenticalService(DbSvc_Info_t *sOld, DbSvc_Info_t *sNew)
{
	if ((DbSvc_GetSvcId(sOld) != DbSvc_GetSvcId(sNew)) || (DbSvc_GetTsId(sOld) != DbSvc_GetTsId(sNew)) || (DbSvc_GetOnId(sOld) != DbSvc_GetOnId(sNew)))
	{
		return FALSE;
	}
	if (DbSvc_GetOrgSvcType(sOld) != DbSvc_GetOrgSvcType(sNew))
	{
		return FALSE;
	}

	return TRUE;
}

INLINE STATIC HBOOL xmgr_dynamicupdate_isModifyedService(DbSvc_Info_t *sOld, DbSvc_Info_t *sNew)
{
	if ((DbSvc_GetSvcId(sOld) != DbSvc_GetSvcId(sNew)) || (DbSvc_GetTsId(sOld) != DbSvc_GetTsId(sNew)) || (DbSvc_GetOnId(sOld) != DbSvc_GetOnId(sNew)))
	{
		return TRUE;
	}
	if (DbSvc_GetSvcType(sOld) != DbSvc_GetSvcType(sNew))
	{
		return TRUE;
	}
#if (0)
	if ((sOld->bModifiedName == FALSE) && (MWC_UTIL_DvbStrncmp(DbSvc_GetName(sOld), DbSvc_GetName(sNew), /*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength()) != 0))
	{
		return TRUE;
	}
#endif

	return FALSE;
}

STATIC HERROR xmgr_dynamicupdate_saveResults_Ausdtt (DynamicUpdate_Content_Ausdtt_t* pstContents, HBOOL *pbUpdated)
{
	Handle_t			 hSvc;
	DYNAMICUPDATE_UpdateFlag_e	*peSvcFlagArrayNew = NULL, *peSvcFlagArrayOld = NULL;
	HERROR			hErr = ERR_FAIL;
	HUINT32			ulAdded = 0, ulDeleted = 0, ulUpdated = 0, ulIdentical = 0;
	HUINT32			ulCountNew = 0, ulCountOld = 0;
	DbSvc_Info_t		*astSvcInfoListNew = NULL, *astSvcInfoListOld = NULL;
	DbSvc_Info_t		*pstSvcInfoTmp = NULL;
	DbSvc_Attr_t	  	tAttr = {0, };
	Handle_t 		*phListOld = NULL;
	HUINT32			i = 0, j = 0;
	SRCHSVC_AppendFlag_t *peSvcFlagArrayForLcn = NULL;

	HxLOG_Trace();
	if (pbUpdated == NULL)
	{
		HxLOG_Error("*pbUpdated is NULL \n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;

	}
	*pbUpdated = FALSE;

	ulAdded = ulDeleted = ulUpdated = ulIdentical = 0;

	ulCountNew = pstContents->ulSvcNum;
	astSvcInfoListNew = pstContents->pstSvcArray;

	if (ulCountNew == 0 || astSvcInfoListNew == NULL)
	{
		HxLOG_Debug("there is no new service. \n");
		hErr = ERR_OK;
		goto ERROR_FAULT_OCCURRED;
	}

	DB_SVC_InitSvcAttribute(&tAttr);

	// 현재 tuning 된 정보. (si가 아니라..), 어차피 TER 일지라도.
	HxLOG_Debug("pstContents->stBaseContents.stBgTuneInfo.eDeliType: 0x%x \n", pstContents->stBaseContents.stBgTuneInfo.eDeliType);
	tAttr.eOrgDeliType	= pstContents->stBaseContents.stBgTuneInfo.eDeliType;
	tAttr.eVisibleType	= eLcnVisibleSvc_FindAll;
//	tAttr.eUserMask1 	&= ~eSvcUserFlag_03_SvcDeletedByUser;
	DbSvc_SetUser1Flag(tAttr.eUserMask1, eSvcUserFlag_03_SvcDeletedByUser, FALSE);

	hErr = DB_SVC_FindServiceListWithInfo(eSvcGroup_All, &tAttr, &ulCountOld, &phListOld, &astSvcInfoListOld);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("cannot find old service list \n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;

	}

	HxLOG_Debug ("================== [ DYNAMIC UPDATE ] ======================\n");
	HxLOG_Debug("========================OldSvcCnt=(%02d),NewSvcCnt=(%02d)  \n", ulCountOld, ulCountNew);

	/* 1.  기존 서비스 Flag 설정 */
	if (ulCountOld > 0)
	{
		peSvcFlagArrayOld = (DYNAMICUPDATE_UpdateFlag_e *)HLIB_STD_MemCalloc (sizeof(DYNAMICUPDATE_UpdateFlag_e) * ulCountOld);
		if (peSvcFlagArrayOld == NULL)
		{
			HxLOG_Error("alloc fail \n");
			hErr = ERR_FAIL;
			goto ERROR_FAULT_OCCURRED;
		}

		for (i = 0; i < ulCountOld; i++)
		{
			HBOOL	bInit = FALSE;

			if (DbSvc_SetDeliType(astSvcInfoListOld[i], pstContents->stBaseContents.stBgTuneInfo.eDeliType))
			{
				if (pstContents->stBaseContents.stBgTuneInfo.eDeliType == eDxDELIVERY_TYPE_TER)
				{
					if (DbSvc_GetTsId(&astSvcInfoListOld[i]) == pstContents->pstTsInfo->usTsId)
					{
						bInit = TRUE;
					}
				}
			}
			peSvcFlagArrayOld[i] = eDYNAMICUPDATE_EXCLUDE;
			if (bInit == TRUE)
			{
				HxLOG_Debug("OLD Service ==>[%20s],[%04d] [%X] \n",
								DbSvc_GetName(&astSvcInfoListOld[i]), DbSvc_GetLcn(&astSvcInfoListOld[i]), DB_SVC_GetUserFlag1(&astSvcInfoListOld[i]));
				peSvcFlagArrayOld[i] = eDYNAMICUPDATE_INIT;
			}
		}
	}

	/* 2. 뉴 서비스 Flag 설정 */
	if (ulCountNew > 0)
	{
		peSvcFlagArrayNew = (DYNAMICUPDATE_UpdateFlag_e *)HLIB_STD_MemCalloc (sizeof(DYNAMICUPDATE_UpdateFlag_e) * ulCountNew);

		if (peSvcFlagArrayNew == NULL)
		{
			HxLOG_Error("alloc fail \n");
			hErr = ERR_FAIL;
			goto ERROR_FAULT_OCCURRED;
		}

		for (i = 0; i < ulCountNew; i++)
		{
			peSvcFlagArrayNew[i] = eDYNAMICUPDATE_INIT;
			HxLOG_Debug("NEW Service ==>[%20s],[%04d,%04d] \n",
								DbSvc_GetName(&astSvcInfoListNew[i]), DbSvc_GetLcn(&astSvcInfoListNew[i]));
		}
	}

	/* 3. 동일 서비스인지 확인 */
	for (i = 0; i < ulCountOld; i++)
	{
		if (peSvcFlagArrayOld[i] == eDYNAMICUPDATE_INIT)
		{
			for (j = 0; j < ulCountNew; j++)
			{
				if (peSvcFlagArrayNew[j] == eDYNAMICUPDATE_INIT)
				{
					if (TRUE == xmgr_dynamicupdate_isIdenticalService(&(astSvcInfoListOld[i]), &(astSvcInfoListNew[j])))
					{
						peSvcFlagArrayOld[i] = eDYNAMICUPDATE_IDENTICAL;
						peSvcFlagArrayNew[j] = eDYNAMICUPDATE_IDENTICAL;
					}
				}
			}
		}
	}

	/* 4. Update 서비스인지 확인 - 뭔가 정보가 바뀌는 경우 */
	for (i = 0; i < ulCountOld; i++)
	{
		if (peSvcFlagArrayOld[i] == eDYNAMICUPDATE_INIT)
		{
			for (j = 0; j < ulCountNew; j++)
			{
				if (peSvcFlagArrayNew[j] == eDYNAMICUPDATE_INIT)
				{
					if (TRUE == xmgr_dynamicupdate_isModifyedService(&(astSvcInfoListOld[i]), &(astSvcInfoListNew[j])))
					{
						peSvcFlagArrayOld[i] = eDYNAMICUPDATE_UPDATE + j;
						peSvcFlagArrayNew[j] = eDYNAMICUPDATE_UPDATE + i;
					}
				}
			}
		}
	}

	for (i = 0; i < ulCountOld; i++)
	{
		if (peSvcFlagArrayOld[i] == eDYNAMICUPDATE_INIT)
		{
			peSvcFlagArrayOld[i] = eDYNAMICUPDATE_DELETE;
		}
	}
	for (i = 0; i < ulCountNew; i++)
	{
		if (peSvcFlagArrayNew[i] == eDYNAMICUPDATE_INIT)
		{
			peSvcFlagArrayNew[i] = eDYNAMICUPDATE_ADD;
		}
	}


	peSvcFlagArrayForLcn = (SRCHSVC_AppendFlag_t *)HLIB_STD_MemCalloc (sizeof(SRCHSVC_AppendFlag_t) * ulCountNew);
	if (peSvcFlagArrayForLcn == NULL)
	{
		HxLOG_Error("alloc fail \n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_SetLcnServiceNumber(pstContents->stBaseContents.hBGAction, pstContents->bNewTs, pstContents->ulSvcNum, astSvcInfoListNew, peSvcFlagArrayForLcn);
	if (hErr != ERR_OK)
	{
		HxLOG_Critical("SVC_SI_SetLcnServiceNumber fail \n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	/* LCN변경시 적용 */
	for (i = 0; i < ulCountOld; i++)
	{
		if (peSvcFlagArrayOld[i] == eDYNAMICUPDATE_IDENTICAL)
		{
			for (j = 0; j < ulCountNew; j++)
			{
				if ( (TRUE == xmgr_dynamicupdate_isIdenticalService(&(astSvcInfoListOld[i]), &(astSvcInfoListNew[j])))
					&& (peSvcFlagArrayForLcn[j] == eSRCHSVC_UPDATE) )
				{
					peSvcFlagArrayOld[i] = eDYNAMICUPDATE_UPDATE + j;
					peSvcFlagArrayNew[j] = eDYNAMICUPDATE_UPDATE + i;
				}
			}
		}
	}


	/* 5. add new service */
	for (i = 0; i < ulCountNew; i++)
	{
		if (peSvcFlagArrayNew[i] == eDYNAMICUPDATE_ADD)
		{
			pstSvcInfoTmp = &(astSvcInfoListNew[i]);
			DB_SVC_AddServiceInfo(pstSvcInfoTmp, NULL);
			ulAdded++;

			HxLOG_Debug("[ADD]: SvcName=(%s) \n", DbSvc_GetName(pstSvcInfoTmp));
		}
	}

	for (i = 0; i < ulCountOld; i++)
	{
		/* 4. update old service */
		if (peSvcFlagArrayOld[i] >= eDYNAMICUPDATE_UPDATE)
		{
			hSvc = phListOld[i];
			j = peSvcFlagArrayOld[i] - eDYNAMICUPDATE_UPDATE;
			xmgr_dynamicupdate_updateServiceInfo(hSvc, &(astSvcInfoListOld[i]), &(astSvcInfoListNew[j]));
			ulUpdated++;
			HxLOG_Debug("[UPDATE]: SvcName=(%s) \n", DbSvc_GetName(&astSvcInfoListNew[j]));
		}
		/* 6. delete old service */
		else if (peSvcFlagArrayOld[i] == eDYNAMICUPDATE_DELETE)
		{
			hSvc = phListOld[i];
			DB_SVC_DeleteServiceInfo(hSvc);
			ulDeleted++;
			HxLOG_Debug("[DELETE]: SvcName=(%s) \n", DbSvc_GetName(&astSvcInfoListOld[i]));
		}
		else if (peSvcFlagArrayOld[i] == eDYNAMICUPDATE_IDENTICAL)
		{
			ulIdentical++;
			HxLOG_Debug("[IDENTICAL]: SvcName=(%s) \n", DbSvc_GetName(&astSvcInfoListOld[i]));
		}
	}

	if (ulAdded > 0 || ulUpdated > 0 || ulDeleted > 0)
	{
		BUS_PostMessage (NULL, eSEVT_DB_NOTIFY_UPDATED, HANDLE_NULL, 0, 0, 0);
	}

	hErr = ERR_OK;

ERROR_FAULT_OCCURRED:

	DB_SVC_FreeServiceInfoList(NULL, phListOld, astSvcInfoListOld);

	if (pstContents->pstTsInfo != NULL)
	{
		HLIB_STD_MemFree (pstContents->pstTsInfo);
		pstContents->pstTsInfo = NULL;
	}

	if (pstContents->pstNetInfo != NULL)
	{
		HLIB_STD_MemFree (pstContents->pstNetInfo);
		pstContents->pstNetInfo = NULL;
	}

	if (pstContents->pstSvcArray != NULL)
	{
		HLIB_STD_MemFree (pstContents->pstSvcArray);
		pstContents->pstSvcArray = NULL;
	}

	if (pstContents->pstPrvArray != NULL)
	{
		HLIB_STD_MemFree (pstContents->pstPrvArray);
		pstContents->pstPrvArray = NULL;
	}

	if (peSvcFlagArrayNew != NULL)
	{
		HLIB_STD_MemFree (peSvcFlagArrayNew);
		peSvcFlagArrayNew = NULL;
	}

	if (peSvcFlagArrayForLcn!= NULL)
	{
		HLIB_STD_MemFree (peSvcFlagArrayForLcn);
		peSvcFlagArrayForLcn = NULL;
	}

	if (peSvcFlagArrayOld != NULL)
	{
		HLIB_STD_MemFree (peSvcFlagArrayOld);
		peSvcFlagArrayOld = NULL;
	}

	if (ulAdded > 0)
	{
		DB_SVC_RemoveSearchFlag();
	}

	if (ulAdded > 0 || ulUpdated > 0 || ulDeleted > 0)
	{
		*pbUpdated = TRUE;
		HxLOG_Debug("================== [ DYNAMIC UPDATE RESULT] ======================\n");
		HxLOG_Debug("        Added=(%02d), Updated=(%02d), Deleted=(%02d), Identical=(%d)\n", ulAdded, ulUpdated, ulDeleted, ulIdentical);
		HxLOG_Debug("================== [ DYNAMIC UPDATE RESULT] ======================\n\n");
	}
	return hErr;
}

STATIC HERROR xmgr_dynamicupdate_retrieveResults_Ausdtt (DynamicUpdate_Content_Ausdtt_t* pstContents)
{
	HUINT32					ulIndex, ulItemNum;
	HERROR					hErr = ERR_FAIL;
	SvcSi_ModuleInfo_t		stModuleInfo;
	HUINT32					ulInfoNum = 0;
	HUINT16 				usNetIdx, usTsIdx = 0, usPrvIdx, usTvChNum, usRadioChNum;
	DbSvc_NetInfo_t 		 	stNetInfoInDb;
	DbSvc_TsCondition_t 	 	stTsCond;
	DbSvc_TsInfo_t			*pstTsInfoArray = NULL;
	DxDeliveryType_e		 	eCurrDeliType;
	CH_SignalInfo_t 		 	stChSignalInfo;
	HINT32					nDbTsNum;
	HBOOL					bSignalStrong;

	//------------------------------------------------------------------------------
	// 1. Network Info
	// 현재 Network는 단 하나일 수 밖에 없다.
	hErr = SVC_SI_CountSearchedResult(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Network, &ulItemNum);
	if (hErr != ERR_OK)					{ ulItemNum = 0; }

	HxLOG_Debug("ulItemNum: %d \n", ulItemNum);
	if (ulItemNum == 0)
	{
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	if(pstContents->pstNetInfo != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstNetInfo);
		pstContents->pstNetInfo = NULL;
	}
	pstContents->pstNetInfo = (DbSvc_NetInfo_t *)HLIB_STD_MemCalloc (sizeof(DbSvc_NetInfo_t));
	if (pstContents->pstNetInfo == NULL)
	{
		HxLOG_Error("alloc fail \n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Network, 1, &ulInfoNum, (void *)pstContents->pstNetInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("cannot get dynamic update result, eSI_SRCH_INFOTYPE_Network \n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	pstContents->pstNetInfo->eOrgDeliType = eDxDELIVERY_TYPE_TER;

	//------------------------------------------------------------------------------
	// 1_1. adding/updating a network
	//------------------------------------------------------------------------------
	if (pstContents->pstNetInfo != NULL)
	{
		// Network Info가 이미 등록되어 있는 것인지 체크한다.
		hErr = DB_SVC_FindNetworkIndexById(pstContents->pstNetInfo->eOrgDeliType, pstContents->pstNetInfo->usNetId, &usNetIdx);
		if (hErr == ERR_OK)
		{
			if (pstContents->pstNetInfo->szNetName[0] != '\0')
			{
				HBOOL	bCompDefAuth, bIsSameDefAuth;
				HUINT8	*aucDefAuthNew = NULL, *aucDefAuth = NULL;

				DB_SVC_GetNetworkInfo (usNetIdx, &stNetInfoInDb);

				(void)DB_SVC_GetNetworkDefaultAuthority((const DbSvc_NetInfo_t *)pstContents->pstNetInfo, &aucDefAuthNew);
				(void)DB_SVC_GetNetworkDefaultAuthority((const DbSvc_NetInfo_t *)&stNetInfoInDb, &aucDefAuth);

				bIsSameDefAuth = TRUE;
				bCompDefAuth = ((NULL != aucDefAuthNew) && (NULL != aucDefAuth)) ? TRUE : FALSE;
				if (bCompDefAuth)
				{
					bIsSameDefAuth = (0 != MWC_UTIL_DvbStrcmp ((const HCHAR*)aucDefAuthNew, (const HCHAR*)aucDefAuth)) ? FALSE : TRUE;
				}

				if ((MWC_UTIL_DvbStrcmp (pstContents->pstNetInfo->szNetName, stNetInfoInDb.szNetName) != 0) ||
					(TRUE == bCompDefAuth && FALSE == bIsSameDefAuth))
				{
					// Network Name이 달라졌으면 Update 한다.
					DB_SVC_UpdateNetworkInfo (usNetIdx, pstContents->pstNetInfo);
					HxLOG_Print("\t= [NET] netIdx = %d --> update\n", usNetIdx);
				}
			}
		}
		else
		{
			// 등록되어 있지 않으므로 새로 추가한다.
			DB_SVC_AddNetworkInfo (pstContents->pstNetInfo, &usNetIdx);
			HxLOG_Print("\t= [NET] netIdx = %d --> add\n", usNetIdx);
		}
	}

	HxLOG_Print("  OrgDeliType:0x%X usNetIdx:0x%X usNetId:0x%X \n",
		pstContents->pstNetInfo->eOrgDeliType,
		pstContents->pstNetInfo->usNetIdx,
		pstContents->pstNetInfo->usNetId);



	//------------------------------------------------------------------------------
	// 2. TS Info
	// TS Info를 얻어오는 것이 제일 중요하다.
	// 여기서 TS Info를 얻어오지 못하면 search fail로 간주한다.
	hErr = SVC_SI_CountSearchedResult(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Transponder, &ulItemNum);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("cannot get dynamic update result, eSI_SRCH_INFOTYPE_Transponder \n");
		hErr = ERR_BUS_NO_OBJECT;
		goto ERROR_FAULT_OCCURRED;
	}

	if (pstContents->pstTsInfo != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstTsInfo);
		pstContents->pstTsInfo = NULL;
	}
	// 현재 TS는 단 하나일 수 밖에 없다.
	pstContents->pstTsInfo = (DbSvc_TsInfo_t *)HLIB_STD_MemCalloc (sizeof(DbSvc_TsInfo_t));
	if (pstContents->pstTsInfo == NULL)
	{
		HxLOG_Error("alloc fail \n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Transponder, 1, &ulInfoNum, (void *)pstContents->pstTsInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("cannot get dynamic update result, eSI_SRCH_INFOTYPE_Transponder \n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	pstContents->pstTsInfo->eDeliType = eDxDELIVERY_TYPE_TER;

	HxLOG_Print("  usTsIdx:0x%X usUniqueId:0x%X usNetIdx: 0x%X usTsId:0x%X usOnId:0x%X ucTunerId:%d eDeliType: %d\n",
		pstContents->pstTsInfo->usTsIdx,
		pstContents->pstTsInfo->usUniqueId,
		pstContents->pstTsInfo->usNetIdx,
		pstContents->pstTsInfo->usTsId,
		pstContents->pstTsInfo->usOnId,
		pstContents->pstTsInfo->ucTunerId,
		pstContents->pstTsInfo->eDeliType);


	usTvChNum = usRadioChNum = 0;
	pstContents->bNewTs = TRUE;
	//------------------------------------------------------------------------------
	// 2_1. adding/updating a ts
	//------------------------------------------------------------------------------
	if (pstContents->pstTsInfo != NULL)
	{
		pstContents->pstTsInfo->usNetIdx = usNetIdx;

		// Signal의 Quality를 기록해야 한다.
		eCurrDeliType = eDxDELIVERY_TYPE_TER;
		hErr = SVC_CH_GetSignalInfo (pstContents->stBaseContents.ulBGActionId, &eCurrDeliType, &stChSignalInfo);
		if (hErr != ERR_OK)
		{
			stChSignalInfo.signalLevel = 0;
			stChSignalInfo.signalQuality = 0;
		}

		HxLOG_Print("[SVC_SEARCH] SIGNAL Strength (%d:%d)\n", stChSignalInfo.signalLevel, stChSignalInfo.signalQuality);
		pstContents->pstTsInfo->stTuningParam.ter.ucLevel = (HUINT8)stChSignalInfo.signalLevel;
		pstContents->pstTsInfo->stTuningParam.ter.ucQuality = (HUINT8)stChSignalInfo.signalQuality;

		// TS Info가 OnId, TsId로 등록되어 있는 지 확인한다.
		DB_SVC_InitTsFindCondition (&stTsCond);
		stTsCond.nDeliType	= pstContents->pstTsInfo->eDeliType;
		stTsCond.nOnId		= (HINT32)pstContents->pstTsInfo->usOnId;
		stTsCond.nTsId		= (HINT32)pstContents->pstTsInfo->usTsId;

		HxLOG_Print("  usTsIdx:%d usUniqueId:%d usNetIdx:%d usTsId:%d usOnId:%d ucTunerId:%d eDeliType: %d\n",
			pstContents->pstTsInfo->usTsIdx,
			pstContents->pstTsInfo->usUniqueId,
			pstContents->pstTsInfo->usNetIdx,
			pstContents->pstTsInfo->usTsId,
			pstContents->pstTsInfo->usOnId,
			pstContents->pstTsInfo->ucTunerId,
			pstContents->pstTsInfo->eDeliType);

		{
			nDbTsNum = 0;
			pstTsInfoArray = NULL;

			hErr = DB_SVC_FindTsInfoList (&stTsCond, &nDbTsNum, &pstTsInfoArray);
			if (hErr == ERR_OK && nDbTsNum > 0 && pstTsInfoArray != NULL)
			{
				HxLOG_Print("\tFind Same Ts (OnId:0x%X, TsId:0x%X)\n", stTsCond.nOnId, stTsCond.nTsId);
				usTsIdx = pstTsInfoArray[0].usTsIdx;
				pstContents->bNewTs = FALSE;
#if (0)
				if (DB_SVC_UtilCompareDbTuningParam(pstTsInfoArray[0].eDeliType, &(pstContents->pstTsInfo->stTuningParam), &(pstTsInfoArray[0].stTuningParam)) != 0)
				{
					HxLOG_Print("\tBut different Tunning Param\n");
#if defined(CONFIG_MW_CH_DVB_T2)
					if(pstContents->pstTsInfo->stTuningParam.ter.ucSystem && pstContents->pstTsInfo->stTuningParam.ter.stT2param.ulPlpId != 0x0000FFFF && pstContents->pstTsInfo->stTuningParam.ter.stT2param.ulPlpId != pstTsInfoArray[0].stTuningParam.ter.stT2param.ulPlpId)
					{
						DB_SVC_AddTsInfo (pstContents->pstTsInfo, &usTsIdx);
						HxLOG_Print("\tAdd New TP (TsIdx:0x%X)\n",usTsIdx);
						pstContents->bNewTs = TRUE;
					}
					else
#endif
					{
						// Same On/Ts Id, but the different Tuning Info :
						// FLD 개념에 의거, Signal이 훨씬 세면 Update, 그렇지 않으면 이 TP 자체를 무시한다.
						// 이사 갔는데, 다시 서치하는 경우, 기존 채널이 있으면 서치가 안 될 수도 있음
						bSignalStrong = TRUE;
						xmgr_dynamicupdate_IsSignalStrongerThanDbTsInfo_Ausdtt (pstContents, pstContents->pstTsInfo, &(pstTsInfoArray[0]), &bSignalStrong);
						if (bSignalStrong == TRUE)
						{
							// TODO: should be checked. usTsIdx
							HxLOG_Print("\tDB_SVC_UpdateTsInfo(TsIdx:0x%X)\n",usTsIdx);
							DB_SVC_UpdateTsInfo (usTsIdx, pstContents->pstTsInfo);
						}
						else
						{
							HxLOG_Error("bSignalStrong is FALSE \n");
						}
					}
				}
#endif
			}
			else
			{
				DB_SVC_AddTsInfo (pstContents->pstTsInfo, &usTsIdx);
				HxLOG_Print("\tDB_SVC_AddTsInfo(TsIdx:0x%X)\n", usTsIdx);
				HxLOG_Print("\t\t= [TS] tsIdx = %d --> add\n", usTsIdx);
			}

			if (pstTsInfoArray != NULL)
			{
				DB_SVC_FreeTsInfoList (pstTsInfoArray);
				pstTsInfoArray = NULL;
			}
		}
	}


	//------------------------------------------------------------------------------
	// 3. Provider Info
	hErr = SVC_SI_CountSearchedResult(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Provider, &(pstContents->ulPrvNum));
	if (hErr != ERR_OK)					{ pstContents->ulPrvNum = 0; }
	if (pstContents->ulPrvNum > 0)
	{
		if(pstContents->pstPrvArray != NULL)
		{
			HLIB_STD_MemFree(pstContents->pstPrvArray);
			pstContents->pstPrvArray = NULL;
		}
		pstContents->pstPrvArray = (DbSvc_PrvInfo_t *)HLIB_STD_MemCalloc (sizeof(DbSvc_PrvInfo_t) * (pstContents->ulPrvNum));
		if (pstContents->pstPrvArray != NULL)
		{
			hErr = SVC_SI_GetSearchedResults(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Provider, pstContents->ulPrvNum, &ulInfoNum, (void *)pstContents->pstPrvArray);
			if (hErr != ERR_OK)
			{
				pstContents->ulPrvNum = 0;
				HLIB_STD_MemFree (pstContents->pstPrvArray);
				pstContents->pstPrvArray = NULL;
			}
		}
	}


	// 6-4. Provider Info
	// Svc Info에 들어가는 Provider Index의 재계산을 위해 Provider Info가 새로 들어가는 위치를 미리 얻어낸다.
	if (pstContents->pstPrvArray != NULL)
	{
		for (ulIndex = 0; ulIndex < pstContents->ulPrvNum; ulIndex++)
		{
			//------------------------------------------------------------------------------
			// 3_1. adding/updating providers
			//------------------------------------------------------------------------------
			// Provider Info가 미리 등록되어 있는지 확인한다.
			hErr = DB_SVC_FindProviderIndexByName (pstContents->pstPrvArray[ulIndex].szPrvName, &usPrvIdx);
			if (hErr != ERR_OK)
			{
				// 등록되어 있지 않으면 추가한다.
				DB_SVC_AddProviderInfo (&(pstContents->pstPrvArray[ulIndex]), &usPrvIdx);
				HxLOG_Print("\t\t= [PRV] prvIdx = %d --> add\n", usPrvIdx);
			}
			pstContents->pstPrvArray[ulIndex].usPrvIdx = usPrvIdx;
		}
	}


	//------------------------------------------------------------------------------
	// 4. Service Info
	hErr = SVC_SI_CountSearchedResult(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Service, &(pstContents->ulSvcNum));
	if (hErr != ERR_OK || pstContents->ulSvcNum == 0)
	{
		HxLOG_Error("cannot get dynamic update result,  eSI_SRCH_INFOTYPE_Service\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	if(pstContents->pstSvcArray != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstSvcArray);
		pstContents->pstSvcArray = NULL;
	}
	pstContents->pstSvcArray  = (DbSvc_Info_t *)HLIB_STD_MemCalloc (sizeof(DbSvc_Info_t) * (pstContents->ulSvcNum));
	if (pstContents->pstSvcArray  == NULL)
	{
		HxLOG_Error("alloc fail \n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Service, pstContents->ulSvcNum, &ulInfoNum, (void *)pstContents->pstSvcArray );
	if (pstContents->pstSvcArray  == NULL)
	{
		HxLOG_Error("cannot get dynamic update result,  eSI_SRCH_INFOTYPE_Service\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	for (ulIndex = 0; ulIndex < pstContents->ulSvcNum; ulIndex++)
	{
		DbSvc_SetDeliType(pstContents->pstSvcArray[ulIndex], eDxDELIVERY_TYPE_TER);
	}

	// 4-1. Service Info
	for (ulIndex = 0; ulIndex < pstContents->ulSvcNum; ulIndex++)
	{
		if (xmgr_dynamicupdate_checkSvcTypeOption (pstContents->eServiceTypeOption, DbSvc_GetSvcType(&pstContents->pstSvcArray[ulIndex])) == TRUE)
		{
			HUINT16		usPrvIdx;
			DbSvc_SetOnId(&pstContents->pstSvcArray[ulIndex], pstContents->pstTsInfo->usOnId);
			DbSvc_SetTsId(&pstContents->pstSvcArray[ulIndex], pstContents->pstTsInfo->usTsId);
			DbSvc_SetTsIdx(&pstContents->pstSvcArray[ulIndex], usTsIdx);
			usPrvIdx = DbSvc_GetPrvIdx(&pstContents->pstSvcArray[ulIndex]);

			if ((pstContents->pstPrvArray != NULL) && (usPrvIdx < pstContents->ulPrvNum))
			{
				DbSvc_SetPrvIdx(&pstContents->pstSvcArray[ulIndex], pstContents->pstPrvArray[usPrvIdx].usPrvIdx);
			}
			else
			{
				DbSvc_SetPrvIdx(&pstContents->pstSvcArray[ulIndex], 0);
			}

			DbSvc_SetTsIdx(&pstContents->pstSvcArray[ulIndex], usTsIdx);
		}

		if (eDxDELIVERY_TYPE_TER == stModuleInfo.stSpecifier.eDeliType && eCountryID_GBR == stModuleInfo.stSpecifier.eCountryId)
		{
			MWC_UTIL_EliminateShyAndControlCharacter(DbSvc_GetName(&pstContents->pstSvcArray[ulIndex]), MWC_UTIL_DvbStrlen(DbSvc_GetName(&pstContents->pstSvcArray[ulIndex])));
		}
	}

	hErr = ERR_OK;

ERROR_FAULT_OCCURRED:

	return hErr;
}

// eSEVT_SI_DYNAMIC_UPDATE_DONE:
STATIC HERROR xmgr_dynamicupdate_evtSiDynamicUpdateDone_Ausdtt (DynamicUpdate_Content_Ausdtt_t *pstContents, HBOOL *pbUpdated)
{
	HERROR				 hErr = ERR_FAIL;

	HxLOG_Trace();

	hErr = xmgr_dynamicupdate_retrieveResults_Ausdtt (pstContents);

	if (hErr == ERR_OK)
	{
		hErr = xmgr_dynamicupdate_saveResults_Ausdtt (pstContents, pbUpdated);
	}

	return hErr;
}

// eSEVT_SI_DYNAMIC_UPDATE_FAIL:
STATIC HERROR xmgr_dynamicupdate_evtSiDynamicUpdateFail_Ausdtt (DynamicUpdate_Content_Ausdtt_t *pstContents)
{
	HERROR				 hErr = ERR_FAIL;

	HxLOG_Trace();

	hErr = ERR_OK;

	return hErr;
}

STATIC BUS_Result_t	proc_dynamicupdate_Ausdtt(HINT32 lMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DynamicUpdate_Content_Ausdtt_t		*pstContents = xmgr_DynamicUpdate_GetContent_Ausdtt();
	HERROR							hErr = ERR_FAIL;
	HBOOL							bUpdated = FALSE;

	switch(lMessage)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("eMEVT_BUS_CREATE\n");
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			(void)xmgr_DynamicUpdate_InitContext_Ausdtt(pstContents);
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Print("eMEVT_BUS_DESTROY !!\n");
			HxLOG_Debug("pstContents->stBaseContents.bStartedDyUpdate = %d\n", pstContents->stBaseContents.bStartedDyUpdate);
			xmgr_dynamicupdate_releaseBackGroundAction(pstContents);
			(void)xmgr_DynamicUpdate_InitContext_Ausdtt(pstContents);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_STOPPED:
			HxLOG_Print("eMEVT_LIVE_NOTIFY_SVC_STOPPED !!\n");
			hErr = xmgr_dynamicupdate_mevtLiveNotifySvcStopped(pstContents, hAction, p1, p2, p3);
			if(hErr == ERR_OK)
			{
				(void)xmgr_DynamicUpdate_InitContext_Ausdtt(pstContents);
			}
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			HxLOG_Print("eMEVT_LIVE_NOTIFY_SVC_CHANGED !!\n");
			hErr = xmgr_dynamicupdate_mevtLiveNotifySvcStopped(pstContents, hAction, p1, p2, p3);
			if(hErr == ERR_OK)
			{
				(void)xmgr_DynamicUpdate_InitContext_Ausdtt(pstContents);
			}

			hErr = xmgr_dynamicupdate_mevtLiveNotifySvcChanged(pstContents, hAction, p1, p2, p3);
			break;

		// channel event.
		case eSEVT_CH_LOCKED :
			HxLOG_Print("eSEVT_CH_LOCKED !!\n");
			hErr = xmgr_dynamicupdate_evtChLocked (&(pstContents->stBaseContents), hAction, p1, p2, p3);
//			if (hErr == ERR_OK)
//				pstContents->stBaseContents.bStartedDyUpdate = TRUE;
			break;

		// si dynamic update event.
		case eSEVT_SI_DYNAMIC_UPDATE_DONE:
			HxLOG_Info("\t[eSEVT_SI_DYNAMIC_UPDATE_DONE]\n\n");
			hErr = xmgr_dynamicupdate_checkActionId (&(pstContents->stBaseContents), hAction);
			if (hErr == ERR_FAIL)
			{
				return MESSAGE_PASS;
			}
			hErr = xmgr_dynamicupdate_evtSiDynamicUpdateDone_Ausdtt (pstContents, &bUpdated);
			HxLOG_Debug("hErr: %d, bUpdated: %d \n", hErr, bUpdated);
			hErr = xmgr_dynamicupdate_evtSiDynamicUpdateDone(&(pstContents->stBaseContents), bUpdated);
//			if (hErr == ERR_OK)
//				pstContents->stBaseContents.bStartedDyUpdate = TRUE;
			return MESSAGE_BREAK;

		case eSEVT_SI_DYNAMIC_UPDATE_FAIL:
			HxLOG_Info("\t[eSEVT_SI_DYNAMIC_UPDATE_FAIL]\n\n");

			hErr = xmgr_dynamicupdate_checkActionId (&(pstContents->stBaseContents), hAction);
			if (hErr == ERR_FAIL)
			{
				return MESSAGE_PASS;
			}

			hErr = xmgr_dynamicupdate_evtSiDynamicUpdateFail_Ausdtt (pstContents);
			if (hErr == ERR_OK)
			{
				return MESSAGE_BREAK;
			}
			break;
		case eSEVT_DB_PARAM_UPDATED:
			HxLOG_Debug("eSEVT_DB_PARAM_UPDATED \n");
			if(p1 == eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING)
			{

				(void)xmgr_dynamicupdate_checkOnOff(pstContents);

				if(pstContents->stBaseContents.bOnOff == TRUE)
				{
					if(pstContents->stBaseContents.bStartedDyUpdate == FALSE)
					{
						hErr = xmgr_dynamicupdate_setupBackGroundAction(pstContents);
						HxLOG_Debug("xmgr_dynamicupdate_setupBackGroundAction() retrun hErr: %d \n", hErr);
					}
					else
					{
						HxLOG_Debug("pstContents->stBaseContents.bStartedDyUpdate is already TRUE \n");
					}

				}
				else
				{
					if(pstContents->stBaseContents.bStartedDyUpdate == TRUE)
					{
						(void)xmgr_dynamicupdate_releaseBackGroundAction(pstContents);
					}
					else
					{
						HxLOG_Debug("pstContents->stBaseContents.bStartedDyUpdate is already FALSE \n");
					}

				}

			}
			break;

	}

	return BUS_ProcessMessageDefault(lMessage, hAction, p1, p2, p3);
}


#define	________________Global_Function_________________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

HERROR	xmgr_dynamicupdate_Start_Ausdtt(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_Create((char*)"proc_dynamicupdate", APP_ACTION_PRIORITY, proc_dynamicupdate_Ausdtt, HANDLE_NULL, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	xmgr_dynamicupdate_Stop_Ausdtt(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_DestroyWithReason(proc_dynamicupdate_Ausdtt, eDESTROY_BUS_MYSELF, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

// for batch
BUS_Callback_t xmgr_dynamicupdate_GetProc_Ausdtt(void)
{
	return proc_dynamicupdate_Ausdtt;
}

/*********************** End of File ******************************/

