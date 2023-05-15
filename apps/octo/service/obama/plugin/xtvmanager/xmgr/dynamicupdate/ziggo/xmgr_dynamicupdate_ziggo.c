/**
	@file     xmgr_dynamicupdate_ziggo.c
	@brief    file_name & simple comment.

	Description: \n
	Module: \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n


	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#if defined(CONFIG_OP_ZIGGO)
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
typedef struct tagDynamicUpdateContent_Ziggo
{
	DynamicUpdate_Content_t		stBaseContents;

	// 향별 추가되는 filed는 여기에. 밑으로.
	DxSvcType_e					 eServiceTypeOption;

	HUINT32						ulTsNum;
	HUINT32						ulPrvNum;
	HUINT32						ulSvcNum;
	HBOOL						*pbNewTs;

	DbSvc_NetInfo_t 				*pstNetInfo;
	DbSvc_TsInfo_t				*pstTsInfo;
	DbSvc_Info_t					*pstSvcArray;
	DbSvc_PrvInfo_t 				*pstPrvArray;
}DynamicUpdate_Content_Ziggo_t;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC DynamicUpdate_Content_Ziggo_t	s_stDynamicUpdateContent_Ziggo;


/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/
STATIC BUS_Result_t	proc_dynamicupdate_Ziggo(HINT32 lMessage, Handle_t hAction, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
STATIC HERROR		xmgr_DynamicUpdate_InitContext_Ziggo(DynamicUpdate_Content_Ziggo_t *pstContents);
INLINE STATIC DynamicUpdate_Content_Ziggo_t *xmgr_DynamicUpdate_GetContent_Ziggo (void);


#define	________________Static_Function_________________________

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

INLINE STATIC DynamicUpdate_Content_Ziggo_t *xmgr_DynamicUpdate_GetContent_Ziggo (void)
{
	return &s_stDynamicUpdateContent_Ziggo;
}

STATIC HERROR	xmgr_DynamicUpdate_InitContext_Ziggo(DynamicUpdate_Content_Ziggo_t *pstContents)
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

	HxSTD_memset(pstContents, 0x00, sizeof(DynamicUpdate_Content_Ziggo_t));

	pstContents->eServiceTypeOption = eDxSVC_TYPE_All;

	pstContents->stBaseContents.stDynamicUpdateSpec.eActionKind = eSVCSI_ACT_KIND_DynamicUpdate;
	pstContents->stBaseContents.stDynamicUpdateSpec.ulActionSpec = eSVCSI_ACT_SPEC_Ziggo;

	(void)xmgr_dynamicupdate_checkOnOff(&(pstContents->stBaseContents));

	return hErr;
}

INLINE STATIC HBOOL xmgr_dynamicupdate_isIdenticalService(DbSvc_Info_t *sOld, DbSvc_Info_t *sNew)
{
	if ((DbSvc_GetSvcIdPtr(sOld) != DbSvc_GetSvcIdPtr(sNew)) || (DbSvc_GetTsIdPtr(sOld) != DbSvc_GetTsIdPtr(sNew)) || (DbSvc_GetOnIdPtr(sOld) != DbSvc_GetOnIdPtr(sNew)))
	{
		return FALSE;
	}
	if (DbSvc_GetSvcType(sOld) != DbSvc_GetSvcType(sNew))
	{
		return FALSE;
	}

	return TRUE;
}

INLINE STATIC HBOOL xmgr_dynamicupdate_isModifyedService(DbSvc_Info_t *sOld, DbSvc_Info_t *sNew)
{
	if ((DbSvc_GetSvcIdPtr(sOld) != DbSvc_GetSvcIdPtr(sNew)) || (DbSvc_GetTsIdPtr(sOld) != DbSvc_GetTsIdPtr(sNew)) || (DbSvc_GetOnIdPtr(sOld) != DbSvc_GetOnIdPtr(sNew)))
	{
		return TRUE;
	}
	if (DbSvc_GetSvcType(sOld) != DbSvc_GetSvcType(sNew))
	{
		return TRUE;
	}
#if (0)
	if ((sOld->bModifiedName == FALSE) && (MWC_UTIL_DvbStrncmp(DbSvc_GetNamePtr(sOld), DbSvc_GetNamePtr(sNew), /*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength()) != 0))
	{
		return TRUE;
	}
#endif

	return FALSE;
}

STATIC HERROR xmgr_dynamicupdate_saveResults_Ziggo (DynamicUpdate_Content_Ziggo_t* pstContents, HBOOL *pbUpdated)
{
	Handle_t			 	hSvc;
	DbSvc_Info_t			*pstSvcInfoTmp = NULL;
	DbSvc_Condition_t 	tCond = {0, };
	DbSvc_Info_t			*pstSvcInfoListNew = NULL, *pstSvcInfoListOld = NULL;
	Handle_t 			*phListOld = NULL;
	DYNAMICUPDATE_UpdateFlag_e	*peSvcFlagArrayNew = NULL, *peSvcFlagArrayOld = NULL;
	SRCHSVC_AppendFlag_t *peSvcFlagArrayForLcn = NULL;
	HUINT32				ulTsLoop, i, j;
	HUINT32				ulAdded, ulDeleted, ulUpdated, ulIdentical;
	HUINT32				ulCountNew = 0, ulCountOld = 0;
	HERROR				hErr = ERR_FAIL;

	HxLOG_Trace();
	if (pbUpdated == NULL)
	{
		HxLOG_Error("*pbUpdated is NULL \n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;

	}

	*pbUpdated = FALSE;
	if (pstContents->ulSvcNum == 0 || pstContents->pstSvcArray == NULL)
	{
		HxLOG_Debug("there is no new service. \n");
		hErr = ERR_OK;
		goto ERROR_FAULT_OCCURRED;
	}

	ulAdded = ulDeleted = ulUpdated = ulIdentical = 0;
	for (ulTsLoop = 0; ulTsLoop < pstContents->ulTsNum; ulTsLoop++)
	{
		ulCountNew = ulCountOld = 0;

		/* 전체 svc list에서 해당 TP의 svc list를 구한다. */
		for (i = 0; i < pstContents->ulSvcNum; i++)
		{
			if ( (pstContents->pstTsInfo[ulTsLoop].usOnId == pstContents->pstSvcArray[i].usOnId)
		 		&& (pstContents->pstTsInfo[ulTsLoop].usTsId == pstContents->pstSvcArray[i].usTsId) )
		 		ulCountNew++;
		}

		if (ulCountNew <= 0)
		{
			HxLOG_Error("cannot find new service list (0x%x, 0x%x)\n", pstContents->pstTsInfo[ulTsLoop].usOnId, pstContents->pstTsInfo[ulTsLoop].usTsId);
			continue;
		}

		if (pstSvcInfoListNew != NULL)
		{
			HLIB_STD_MemFree(pstSvcInfoListNew);
			pstSvcInfoListNew = NULL;
		}
		pstSvcInfoListNew = (DbSvc_Info_t *)HLIB_STD_MemCalloc (sizeof(DbSvc_Info_t) * ulCountNew);
		if (pstSvcInfoListNew == NULL)
		{
			hErr = ERR_FAIL;
			goto ERROR_FAULT_OCCURRED;
		}

		ulCountNew = 0;
		for (i = 0; i < pstContents->ulSvcNum; i++)
		{
			if ( (pstContents->pstTsInfo[ulTsLoop].usOnId == pstContents->pstSvcArray[i].usOnId)
		 		&& (pstContents->pstTsInfo[ulTsLoop].usTsId == pstContents->pstSvcArray[i].usTsId) )
			{
				HxSTD_MemCopy(&pstSvcInfoListNew[ulCountNew], &pstContents->pstSvcArray[i], sizeof(DbSvc_Info_t));
				ulCountNew++;
			}
		}

		/* 기존 저장된 svc list에서 해당 TP의 svc list를 구한다. */
		DB_SVC_InitSvcFindCondition(&tCond);
		HxLOG_Debug("pstContents->stBaseContents.stBgTuneInfo.eDeliType: 0x%x \n", pstContents->stBaseContents.stBgTuneInfo.eDeliType);
		tCond.nOrgDeliType = pstContents->stBaseContents.stBgTuneInfo.eDeliType;
		tCond.nOnId = pstContents->pstTsInfo[ulTsLoop].usOnId;
		tCond.nTsId = pstContents->pstTsInfo[ulTsLoop].usTsId;

		hErr = DB_SVC_FindSvcInfoList (&tCond, (HINT32*)&ulCountOld, &pstSvcInfoListOld);
		hErr |= DB_SVC_FindSvcHandleList (&tCond, (HINT32*)&ulCountOld, &phListOld);
		if (hErr == ERR_FAIL)
		{
			HxLOG_Error("cannot find old service list(0x%x, 0x%x)\n", pstContents->pstTsInfo[ulTsLoop].usOnId, pstContents->pstTsInfo[ulTsLoop].usTsId);
		}

		HxLOG_Debug ("================== [ CHECK DYNAMIC UPDATE ] ======================\n");
		HxLOG_Debug ("========================OldSvcCnt=(%02d), NewSvcCnt=(%02d)\n", ulCountOld, ulCountNew);

		/* 1.  기존 서비스 Flag 설정 */
		if (ulCountOld > 0)
		{
			if (peSvcFlagArrayOld != NULL)
			{
				HLIB_STD_MemFree(peSvcFlagArrayOld);
				peSvcFlagArrayOld = NULL;
			}
			peSvcFlagArrayOld = (DYNAMICUPDATE_UpdateFlag_e *)HLIB_STD_MemCalloc (sizeof(DYNAMICUPDATE_UpdateFlag_e) * ulCountOld);
			if (peSvcFlagArrayOld == NULL)
			{
				HxLOG_Error("alloc fail \n");
				hErr = ERR_FAIL;
				goto ERROR_FAULT_OCCURRED;
			}

			for (i = 0; i < ulCountOld; i++)
			{
				HBOOL bInit = FALSE;

				if (DbSvc_GetDeliType(pstSvcInfoListOld[i]) == pstContents->stBaseContents.stBgTuneInfo.eDeliType)
				{
					if (pstContents->stBaseContents.stBgTuneInfo.eDeliType == eDxDELIVERY_TYPE_CAB)
					{
						if (DbSvc_GetTsId(&pstSvcInfoListOld[i]) == pstContents->pstTsInfo[ulTsLoop].usTsId)
						{
							bInit = TRUE;
						}
					}
				}
				peSvcFlagArrayOld[i] = eDYNAMICUPDATE_EXCLUDE;
				if (bInit == TRUE)
				{
					HxLOG_Debug("OLD Service ==>[%20s],[%04d] [%X] \n",
									DbSvc_GetName(&pstSvcInfoListOld[i]), DbSvc_GetLcn(&pstSvcInfoListOld[i]), DB_SVC_GetUserFlag1(&pstSvcInfoListOld[i]));
					peSvcFlagArrayOld[i] = eDYNAMICUPDATE_INIT;
				}
			}
		}

		/* 2. 뉴 서비스 Flag 설정 */
		if (ulCountNew > 0)
		{
			if (peSvcFlagArrayNew != NULL)
			{
				HLIB_STD_MemFree(peSvcFlagArrayNew);
				peSvcFlagArrayNew = NULL;
			}
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
				HxLOG_Debug("NEW Service ==>[%20s],[%04d] \n",
									DbSvc_GetName(&pstSvcInfoListNew[i]), DbSvc_GetLcn(&pstSvcInfoListNew[i]));
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
						if (TRUE == xmgr_dynamicupdate_isIdenticalService(&pstSvcInfoListOld[i], &pstSvcInfoListNew[j]))
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
						if (TRUE == xmgr_dynamicupdate_isModifyedService(&(pstSvcInfoListOld[i]), &(pstSvcInfoListNew[j])))
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

		if (peSvcFlagArrayForLcn != NULL)
		{
			HLIB_STD_MemFree(peSvcFlagArrayForLcn);
			peSvcFlagArrayForLcn = NULL;
		}
		peSvcFlagArrayForLcn = (SRCHSVC_AppendFlag_t *)HLIB_STD_MemCalloc (sizeof(SRCHSVC_AppendFlag_t) * ulCountNew);
		if (peSvcFlagArrayForLcn == NULL)
		{
			HxLOG_Error("alloc fail \n");
			hErr = ERR_FAIL;
			goto ERROR_FAULT_OCCURRED;
		}

		hErr = SVC_SI_SetLcnServiceNumber(pstContents->stBaseContents.hBGAction, pstContents->pbNewTs[ulTsLoop], ulCountNew, pstSvcInfoListNew, peSvcFlagArrayForLcn);
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
					if ( (TRUE == xmgr_dynamicupdate_isIdenticalService(&(pstSvcInfoListOld[i]), &(pstSvcInfoListNew[j])))
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
				pstSvcInfoTmp = &(pstSvcInfoListNew[i]);
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
				xmgr_dynamicupdate_updateServiceInfo(hSvc, &(pstSvcInfoListOld[i]), &(pstSvcInfoListNew[j]));
				ulUpdated++;
				HxLOG_Debug("[UPDATE]: SvcName=(%s) \n", DbSvc_GetName(&pstSvcInfoListNew[j]));
			}
			/* 6. delete old service */
			else if (peSvcFlagArrayOld[i] == eDYNAMICUPDATE_DELETE)
			{
				hSvc = phListOld[i];
				DB_SVC_DeleteServiceInfo(hSvc);
				ulDeleted++;
				HxLOG_Debug("[DELETE]: SvcName=(%s) \n", DbSvc_GetName(&pstSvcInfoListOld[i]));
			}
			else if (peSvcFlagArrayOld[i] == eDYNAMICUPDATE_IDENTICAL)
			{
				ulIdentical++;
				HxLOG_Debug("[IDENTICAL]: SvcName=(%s) \n", DbSvc_GetName(&pstSvcInfoListOld[i]));
			}
		}
	}

	if (ulAdded > 0 || ulUpdated > 0 || ulDeleted > 0)
	{
		BUS_PostMessage (NULL, eSEVT_DB_NOTIFY_UPDATED, HANDLE_NULL, 0, 0, 0);
	}

	hErr = ERR_OK;

ERROR_FAULT_OCCURRED:

	DB_SVC_FreeServiceInfoList(NULL, phListOld, pstSvcInfoListOld);

	if (pstContents->pstNetInfo != NULL)
	{
		HLIB_STD_MemFree (pstContents->pstNetInfo);
		pstContents->pstNetInfo = NULL;
	}

	if (pstContents->pbNewTs != NULL)
	{
		HLIB_STD_MemFree (pstContents->pbNewTs);
		pstContents->pbNewTs = NULL;
	}

	if (pstContents->pstTsInfo != NULL)
	{
		HLIB_STD_MemFree (pstContents->pstTsInfo);
		pstContents->pstTsInfo = NULL;
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

	if (pstSvcInfoListNew != NULL)
	{
		HLIB_STD_MemFree (pstSvcInfoListNew);
		pstSvcInfoListNew = NULL;
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
		HxLOG_Debug("        Added=(%02d), Updated=(%02d), Deleted=(%02d), Identical=(%d)\n",ulAdded, ulUpdated, ulDeleted, ulIdentical);
		HxLOG_Debug("================== [ DYNAMIC UPDATE RESULT] ======================\n\n");
	}
	return hErr;
}

STATIC HERROR xmgr_dynamicupdate_retrieveResults_Ziggo (DynamicUpdate_Content_Ziggo_t* pstContents)
{
	HERROR					hErr = ERR_FAIL;
	HUINT32					ulIndex, ulSubIndex;
	HUINT32					ulItemNum, ulInfoNum = 0;
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

	if (pstContents->pstNetInfo != NULL)
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

	pstContents->pstNetInfo->eOrgDeliType = eDxDELIVERY_TYPE_CAB;

	//------------------------------------------------------------------------------
	// 1_1. adding/updating a network
	//------------------------------------------------------------------------------
	if (pstContents->pstNetInfo != NULL)
	{
		// Network Info가 이미 등록되어 있는 것인지 체크한다.
		hErr = DB_SVC_FindNetworkIndexById(pstContents->pstNetInfo->eOrgDeliType, pstContents->pstNetInfo->usNetId, &usNetIdx);
		if (hErr != ERR_OK)
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
	// 2. Provider Info
	hErr = SVC_SI_CountSearchedResult(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Provider, &(pstContents->ulPrvNum));
	if (hErr != ERR_OK)					{ pstContents->ulPrvNum = 0; }
	if (pstContents->ulPrvNum > 0)
	{
		if (pstContents->pstPrvArray != NULL)
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

	//------------------------------------------------------------------------------
	// 2_1. adding/updating a provider
	//------------------------------------------------------------------------------
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
	// 3. TS Info
	// TS Info를 얻어오는 것이 제일 중요하다.
	// 여기서 TS Info를 얻어오지 못하면 search fail로 간주한다.
	hErr = SVC_SI_CountSearchedResult(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Transponder, &(pstContents->ulTsNum));
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
	pstContents->pstTsInfo = (DbSvc_TsInfo_t *)HLIB_STD_MemCalloc (sizeof(DbSvc_TsInfo_t) * (pstContents->ulTsNum));
	if (pstContents->pstTsInfo == NULL)
	{
		HxLOG_Error("alloc fail \n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	if (pstContents->pbNewTs != NULL)
	{
		HLIB_STD_MemFree(pstContents->pbNewTs);
		pstContents->pbNewTs = NULL;
	}
	pstContents->pbNewTs= (HBOOL *)HLIB_STD_MemCalloc (sizeof(HBOOL) * (pstContents->ulTsNum));
	if (pstContents->pbNewTs == NULL)
	{
		HxLOG_Error("alloc fail \n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Transponder, pstContents->ulTsNum, &ulInfoNum, (void *)pstContents->pstTsInfo);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("cannot get dynamic update result, eSI_SRCH_INFOTYPE_Transponder \n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	for (ulIndex=0; ulIndex<pstContents->ulTsNum; ulIndex++)
	{
		pstContents->pbNewTs[ulIndex] = TRUE;
		pstContents->pstTsInfo[ulIndex].eDeliType = eDxDELIVERY_TYPE_CAB;

		HxLOG_Print("  usTsIdx:0x%X usUniqueId:0x%X usNetIdx: 0x%X usTsId:0x%X usOnId:0x%X ucTunerId:%d eDeliType: %d\n",
			pstContents->pstTsInfo[ulIndex].usTsIdx,
			pstContents->pstTsInfo[ulIndex].usUniqueId,
			pstContents->pstTsInfo[ulIndex].usNetIdx,
			pstContents->pstTsInfo[ulIndex].usTsId,
			pstContents->pstTsInfo[ulIndex].usOnId,
			pstContents->pstTsInfo[ulIndex].ucTunerId,
			pstContents->pstTsInfo[ulIndex].eDeliType);
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

	if (pstContents->pstSvcArray != NULL)
	{
		HLIB_STD_MemFree(pstContents->pstSvcArray);
		pstContents->pstSvcArray = NULL;
	}
	pstContents->pstSvcArray = (DbSvc_Info_t *)HLIB_STD_MemCalloc (sizeof(DbSvc_Info_t) * (pstContents->ulSvcNum));
	if (pstContents->pstSvcArray  == NULL)
	{
		HxLOG_Error("alloc fail \n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	hErr = SVC_SI_GetSearchedResults(pstContents->stBaseContents.ulBGActionId, eSI_SRCH_INFOTYPE_Service, pstContents->ulSvcNum, &ulInfoNum, (void *)pstContents->pstSvcArray);
	if (pstContents->pstSvcArray  == NULL)
	{
		HxLOG_Error("cannot get dynamic update result,  eSI_SRCH_INFOTYPE_Service\n");
		hErr = ERR_FAIL;
		goto ERROR_FAULT_OCCURRED;
	}

	for (ulIndex = 0; ulIndex < pstContents->ulSvcNum; ulIndex++)
	{
		DbSvc_SetDeliType(pstContents->pstSvcArray[ulIndex], eDxDELIVERY_TYPE_CAB);
	}


	usTvChNum = usRadioChNum = 0;

	//------------------------------------------------------------------------------
	// 4_1. adding/updating tps and service info
	//------------------------------------------------------------------------------
	if ( (pstContents->pstTsInfo != NULL) && (pstContents->ulTsNum > 0) )
	{
		for (ulIndex = 0; ulIndex < pstContents->ulTsNum; ulIndex++)
		{
			pstContents->pstTsInfo[ulIndex].usNetIdx = usNetIdx;

			// TS Info가 OnId, TsId로 등록되어 있는 지 확인한다.
			DB_SVC_InitTsFindCondition (&stTsCond);
			stTsCond.nDeliType	= pstContents->pstTsInfo[ulIndex].eDeliType;
			stTsCond.nOnId		= (HINT32)pstContents->pstTsInfo[ulIndex].usOnId;
			stTsCond.nTsId		= (HINT32)pstContents->pstTsInfo[ulIndex].usTsId;

			HxLOG_Print("  usTsIdx:0x%X usUniqueId:0x%X usNetIdx: 0x%X usTsId:0x%X usOnId:0x%X ucTunerId:%d eDeliType: %d\n",
							pstContents->pstTsInfo[ulIndex].usTsIdx,
							pstContents->pstTsInfo[ulIndex].usUniqueId,
							pstContents->pstTsInfo[ulIndex].usNetIdx,
							pstContents->pstTsInfo[ulIndex].usTsId,
							pstContents->pstTsInfo[ulIndex].usOnId,
							pstContents->pstTsInfo[ulIndex].ucTunerId,
							pstContents->pstTsInfo[ulIndex].eDeliType);

			{
				nDbTsNum = 0;
				pstTsInfoArray = NULL;

				hErr = DB_SVC_FindTsInfoList (&stTsCond, &nDbTsNum, &pstTsInfoArray);
				if (hErr == ERR_OK && nDbTsNum > 0 && pstTsInfoArray != NULL)
				{
					HxLOG_Print("\tFind Same Ts (OnId:0x%X, TsId:0x%X)\n", stTsCond.nOnId, stTsCond.nTsId);
					usTsIdx = pstTsInfoArray[0].usTsIdx;
					pstContents->pbNewTs[ulIndex] = FALSE;
#if (1)
					if (DB_SVC_UtilCompareDbTuningParam(pstTsInfoArray[0].eDeliType, &(pstContents->pstTsInfo[ulIndex].stTuningParam), &(pstTsInfoArray[0].stTuningParam)) != 0)
					{
						// TODO: should be checked. usTsIdx
						DB_SVC_UpdateTsInfo (usTsIdx, pstContents->pstTsInfo);
						HxLOG_Print("\tDB_SVC_UpdateTsInfo(TsIdx:0x%X)\n",usTsIdx);
					}
#endif
				}
				else
				{
					DB_SVC_AddTsInfo (pstContents->pstTsInfo, &usTsIdx);
					HxLOG_Print("\tDB_SVC_AddTsInfo(TsIdx:0x%X)\n", usTsIdx);
				}

				if (pstTsInfoArray != NULL)
				{
					DB_SVC_FreeTsInfoList (pstTsInfoArray);
					pstTsInfoArray = NULL;
				}
			}

			// 4-1. Service Info
			for (ulSubIndex = 0; ulSubIndex < pstContents->ulSvcNum; ulSubIndex++)
			{
				if ( (pstContents->pstTsInfo[ulIndex].usOnId == DbSvc_GetOnId(&pstContents->pstSvcArray[ulSubIndex]) )
					 && (pstContents->pstTsInfo[ulIndex].usTsId == DbSvc_GetTsId(&pstContents->pstSvcArray[ulSubIndex])) )
				{
					if (xmgr_dynamicupdate_checkSvcTypeOption (pstContents->eServiceTypeOption, DbSvc_GetSvcType(&pstContents->pstSvcArray[ulSubIndex])) == TRUE)
					{
						HUINT16	 usPrvIdx;
						DbSvc_SetOnId(&pstContents->pstSvcArray[ulSubIndex], pstContents->pstTsInfo[ulIndex].usOnId);
						DbSvc_SetTsId(&pstContents->pstSvcArray[ulSubIndex], pstContents->pstTsInfo[ulIndex].usTsId);
						DbSvc_SetTsIdx(&pstContents->pstSvcArray[ulSubIndex], usTsIdx);
						usPrvIdx = DbSvc_GetPrvIdx(&pstContents->pstSvcArray[ulSubIndex]);

						if ((pstContents->pstPrvArray != NULL) && (usPrvIdx < pstContents->ulPrvNum))
						{
							DbSvc_SetPrvIdx(&pstContents->pstSvcArray[ulSubIndex], pstContents->pstPrvArray[usPrvIdx].usPrvIdx);
						}
						else
						{
							DbSvc_SetPrvIdx(&pstContents->pstSvcArray[ulSubIndex], 0);
						}
					}
				}
			}
		}
	}

	hErr = ERR_OK;

ERROR_FAULT_OCCURRED:

	return hErr;
}

// eSEVT_SI_DYNAMIC_UPDATE_DONE:
STATIC HERROR xmgr_dynamicupdate_evtSiDynamicUpdateDone_Ziggo (DynamicUpdate_Content_Ziggo_t *pstContents, HBOOL *pbUpdated)
{
	HERROR				 hErr = ERR_FAIL;

	HxLOG_Trace();

	hErr = xmgr_dynamicupdate_retrieveResults_Ziggo (pstContents);

	if (hErr == ERR_OK)
	{
		hErr = xmgr_dynamicupdate_saveResults_Ziggo (pstContents, pbUpdated);
	}

	return hErr;
}

// eSEVT_SI_DYNAMIC_UPDATE_FAIL:
STATIC HERROR xmgr_dynamicupdate_evtSiDynamicUpdateFail_Ziggo (DynamicUpdate_Content_Ziggo_t *pstContents)
{
	HERROR				 hErr = ERR_FAIL;

	HxLOG_Trace();

	hErr = ERR_OK;

	return hErr;
}

STATIC BUS_Result_t	proc_dynamicupdate_Ziggo(HINT32 lMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DynamicUpdate_Content_Ziggo_t		*pstContents = xmgr_DynamicUpdate_GetContent_Ziggo();
	HERROR							hErr = ERR_FAIL;
	HBOOL							bUpdated = FALSE;

	switch(lMessage)
	{
		case eMEVT_BUS_CREATE :
			HxLOG_Print("eMEVT_BUS_CREATE\n");
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			(void)xmgr_DynamicUpdate_InitContext_Ziggo(pstContents);
			break;

		case eMEVT_BUS_DESTROY :
			HxLOG_Print("eMEVT_BUS_DESTROY !!\n");
			HxLOG_Debug("pstContents->stBaseContents.bStartedDyUpdate = %d\n", pstContents->stBaseContents.bStartedDyUpdate);
			xmgr_dynamicupdate_releaseBackGroundAction(&(pstContents->stBaseContents));
			(void)xmgr_DynamicUpdate_InitContext_Ziggo(pstContents);
			break;

		case eMEVT_LIVE_NOTIFY_SVC_STOPPED:
			HxLOG_Print("eMEVT_LIVE_NOTIFY_SVC_STOPPED !!\n");			
			hErr = xmgr_dynamicupdate_mevtLiveNotifySvcStopped(pstContents, hAction, p1, p2, p3);
			if(hErr == ERR_OK)
			{
				(void)xmgr_DynamicUpdate_InitContext_Ziggo(pstContents);
			}
			break;

		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			HxLOG_Print("eMEVT_LIVE_NOTIFY_SVC_CHANGED !!\n");			
			hErr = xmgr_dynamicupdate_mevtLiveNotifySvcStopped(pstContents, hAction, p1, p2, p3);
			if(hErr == ERR_OK)
			{
				(void)xmgr_DynamicUpdate_InitContext_Ziggo(pstContents);
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
			hErr = xmgr_dynamicupdate_evtSiDynamicUpdateDone_Ziggo (pstContents, &bUpdated);
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

			hErr = xmgr_dynamicupdate_evtSiDynamicUpdateFail_Ziggo (pstContents);
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

HERROR	xmgr_dynamicupdate_Start_Ziggo(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_Create((char*)"proc_dynamicupdate", APP_ACTION_PRIORITY, proc_dynamicupdate_Ziggo, HANDLE_NULL, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

HERROR	xmgr_dynamicupdate_Stop_Ziggo(void)
{
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	eRes = BUS_MGR_DestroyWithReason(proc_dynamicupdate_Ziggo, eDESTROY_BUS_MYSELF, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

/*********************** End of File ******************************/

#endif