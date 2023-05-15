/**
	@file     _svc_cas_mgr_scenario.c
	@brief   _svc_cas_mgr_scenario.c (CAS Service)

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

#include <svc_cas.h>
#include <_svc_cas_sub_api.h>

#include <_svc_cas_mgr_common.h>
#include <_svc_cas_mgr_main.h>
#include <_svc_cas_mgr_instance.h>
#include <_svc_cas_mgr_scenario.h>

#include <psvc_cas.h>
/********************************************************************
* For Debugging
********************************************************************/
#define ___02_Debugging___


/********************************************************************
* Local Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Local_Definitions___


#define MW_CAS_MAX_PRIORITY_GROUP_ITEM_COUNT 10

/**/
STATIC HUINT32					s_ulNumOfPriorityTable = 0;
STATIC SvcCas_GroupOfPriority_t	s_astPriorityTable[MW_CAS_MAX_PRIORITY_GROUP_ITEM_COUNT];

STATIC HUINT32						s_ulNumOfPriorityNode;
STATIC SvcCas_MgrPriorityTreeItem_t	s_astCasMgrPriorityNode[CAS_MAX_NUM_OF_INSTANCE+1]; //혹시 0개가 될까봐 1개를 +했다.

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___04_Extern_Vars_Func_Prototypes___

/********************************************************************
* Static Variables / Function Prototypes
********************************************************************/
#define ___05_Local_Vars_Func_Prototypes___

#if defined(CONFIG_DEBUG)
STATIC const HINT8 *svc_cas_ScenarioMakeStrActionType(SvcCas_ActionType_e cmd);
STATIC void svc_cas_ScenarioPrintCasMgrPriorityTree(HUINT32 unLevel, SvcCas_MgrPriorityTreeItem_t *pstCasMgrPriorityTree);
STATIC HINT8 *svc_cas_ScenarioMakeStrPriorityTable(SvcCas_ItemOfPriority_e eItemOfPriority);
#endif

/********************************************************************
* Static Functions
* Naming rule : "local"_"CasMgrScenario" _ "Function Name"
********************************************************************/
#define ___06_Local_APIs___
STATIC SvcCas_InstanceType_e svc_cas_scenario_GetInstType(SvcCas_ItemOfPriority_e eItemOfPriority, HUINT32 *pulSlot)
{
	switch(eItemOfPriority)
	{
	case eItemOfPriority_SC0:		*pulSlot = 0; return eCasInst_Embedded;
	case eItemOfPriority_SC1:		*pulSlot = 1; return eCasInst_Embedded;
	case eItemOfPriority_CAM0:		*pulSlot = 0; return eCasInst_CI;
	case eItemOfPriority_CAM1:		*pulSlot = 1; return eCasInst_CI;
	default: 						*pulSlot = 0; return eCasInst_None;
	}
}

STATIC void svc_cas_scenario_InitPriorityTreeNode(void)
{
	s_ulNumOfPriorityNode = 0;
}


STATIC HUINT32 svc_cas_scenario_CountPriorityTreeItemFromSameLevel(SvcCas_MgrPriorityTreeItem_t *pstPriorityTree)
{
	HUINT32		ulNumOfItem = 0;

	while(pstPriorityTree != NULL)
	{
		ulNumOfItem++;
		pstPriorityTree = pstPriorityTree->pstNextSilbing;
	}

	return ulNumOfItem;
}

STATIC HUINT32 svc_cas_scenario_GetNumOfDefSlot(SvcCas_MgrPriorityTreeItem_t *pstItem, SvcCas_ItemOfPriority_e eItemOfPriority)
{
	HUINT32					ulNumOfDefSlot = 0;
	HERROR					hErr;
	HUINT32					ulSlot;
	SvcCas_InstanceType_e	eCasInstType;

	eCasInstType = svc_cas_scenario_GetInstType(eItemOfPriority, &ulSlot);

	HxLOG_Info("eCasInstType = %d, ulSlot = %d\n", eCasInstType, ulSlot);

	while(pstItem)
	{
		hErr = svc_cas_ScenarioIsSameDefSlot(pstItem, eCasInstType, ulSlot);
		if(hErr == ERR_FAIL)
		{
			ulNumOfDefSlot++;
		}

		pstItem = pstItem->pstNextSilbing;
	}

	return ulNumOfDefSlot;
}

STATIC SvcCas_MgrPriorityTreeItem_t *svc_cas_scenario_GetEmptyPriorityTreeNode(void)
{
	SvcCas_MgrPriorityTreeItem_t		*pstCasMgrPriorityTree;

	if(s_ulNumOfPriorityNode >= CAS_MAX_NUM_OF_INSTANCE+1)
	{
		return NULL;
	}

	pstCasMgrPriorityTree = &s_astCasMgrPriorityNode[s_ulNumOfPriorityNode];
	s_ulNumOfPriorityNode++;

	VK_memset32(pstCasMgrPriorityTree, 0, sizeof(SvcCas_MgrPriorityTreeItem_t));
	return pstCasMgrPriorityTree;
}

STATIC SvcCas_ItemOfPriority_e svc_cas_scenario_GetPriorityItemIdByCasInstType(SvcCas_InstanceType_e eCasInst)
{
	switch(eCasInst)
	{
	case eCasInst_Embedded:	return eItemOfPriority_ECAS;
	case eCasInst_CI:			return eItemOfPriority_CI;
	default :					return eItemOfPriority_End;
	}
}

STATIC SvcCas_ItemOfPriority_e svc_cas_scenario_GetKindOfActionItemByActionId(SvcCas_ActionType_e eCAS_ActionType)
{
	switch(eCAS_ActionType)
	{
	case eCAS_ActionType_Live:				return eItemOfPriority_Live;
	case eCAS_ActionType_Playback:			return eItemOfPriority_PlayBack;
	case eCAS_ActionType_Recording:			return eItemOfPriority_Recording;
	case eCAS_ActionType_Descramble_a_File:	return eItemOfPriority_Descramble_a_File;
	case eCAS_ActionType_Tsr_Play:			return eItemOfPriority_Tsr_Play;
	case eCAS_ActionType_Tsr_Rec:				return eItemOfPriority_Tsr_Rec;
	default: 									return eItemOfPriority_End;
	}
}

STATIC HUINT32 svc_cas_scenario_GetPriorityByActionType(SvcCas_GroupOfPriority_t *pstGroupOfPriority, SvcCas_ActionType_e eCAS_ActionType)
{
	HUINT32				i;
	SvcCas_ItemOfPriority_e		eItemOfPriority;

	if(pstGroupOfPriority->eItemOfPriority_G != eItemOfPriority_G_KindOfAction)
	{
		return 0;
	}

	eItemOfPriority = svc_cas_scenario_GetKindOfActionItemByActionId(eCAS_ActionType);

	for(i = 0; i < pstGroupOfPriority->ulNumOfPriority; i++)
	{
		if(eItemOfPriority == pstGroupOfPriority->aeItemOfPriority[i])
		{
			return pstGroupOfPriority->aulPriority[i];
		}
	}

	return 0;
}

#if 0
STATIC HERROR svc_cas_scenario_CheckMatchCaDescriptorByPmt(SvcCas_PmtInfo_t *pPmtInfo1, SvcCas_PmtInfo_t *pPmtInfo2, HUINT16 usCasIdMask)
{
	HUINT32				i, j, k, l;
	HUINT16				usCaSysId1;
	HUINT16				usCaSysId2;
	SvcCas_PmtElement_t		*pstPmtElement1;
	SvcCas_PmtElement_t		*pstPmtElement2;

	/* check parameters */
	if(pPmtInfo1== NULL)
	{
		HxLOG_Error("pPmtInfo1 is invalid (NULL) \n");
		return ERR_FAIL;
	}

	if(pPmtInfo2 == NULL)
	{
		HxLOG_Error("pPmtInfo2 is invalid (NULL) \n");
		return ERR_FAIL;
	}

	if(pPmtInfo1->bProgramCaFlag == TRUE)
	{
		for(i = 0; i < pPmtInfo1->ulNumOfProgramCaDescriptor; i++)
		{
			usCaSysId1 = pPmtInfo1->stProgramCaDescriptor[i].usCaSysId;

			if(pPmtInfo2->bProgramCaFlag == TRUE)
			{
				for(j = 0; j < pPmtInfo2->ulNumOfProgramCaDescriptor; j++)
				{
					usCaSysId2 = pPmtInfo2->stProgramCaDescriptor[j].usCaSysId;
					if( (usCaSysId1  & usCasIdMask) == (usCaSysId2 & usCasIdMask))
					{
						return ERR_OK;
					}
				}
			}

			for(j = 0; j < pPmtInfo2->ulNumOfElement; j++)
			{
				pstPmtElement2 = &pPmtInfo2->stElementInfo[j];

				for(k = 0; k < pstPmtElement2->ulNumOfCaDescriptor; k++)
				{
					usCaSysId2 = pstPmtElement2->stElementCaDescriptor[k].usCaSysId;
					if( (usCaSysId1  & usCasIdMask) == (usCaSysId2 & usCasIdMask))
					{
						return ERR_OK;
					}

				}
			}
		}
	}

	for(i = 0; i < pPmtInfo1->ulNumOfElement; i++)
	{
		pstPmtElement1 = &pPmtInfo1->stElementInfo[i];
		for(j = 0; j < pstPmtElement1->ulNumOfCaDescriptor; j++)
		{
			usCaSysId1 = pstPmtElement1->stElementCaDescriptor[j].usCaSysId;

			if(pPmtInfo2->bProgramCaFlag == TRUE)
			{
				for(k = 0; k < pPmtInfo2->ulNumOfProgramCaDescriptor; k++)
				{
					usCaSysId2 = pPmtInfo2->stProgramCaDescriptor[k].usCaSysId;
					if( (usCaSysId1  & usCasIdMask) == (usCaSysId2 & usCasIdMask))
					{
						return ERR_OK;
					}
				}
			}

			for(k = 0; k < pPmtInfo2->ulNumOfElement; k++)
			{
				pstPmtElement2 = &pPmtInfo2->stElementInfo[k];

				for(l = 0; l < pstPmtElement2->ulNumOfCaDescriptor; l++)
				{
					usCaSysId2 = pstPmtElement2->stElementCaDescriptor[l].usCaSysId;
					if( (usCaSysId1  & usCasIdMask) == (usCaSysId2 & usCasIdMask) )
					{
						return ERR_OK;
					}

				}
			}
		}
	}

	return ERR_FAIL;
}
#endif

STATIC HERROR svc_cas_scenario_CheckMatchCaDescriptorByCaInst(SvcCas_DevInstance_t *pCaInst, SvcCas_PmtInfo_t *pPmtInfo, HUINT16 usCasIdMask)
{
	HUINT32		ulESIdx = 0;
	HUINT32		ulSlotIdx = 0;
	HUINT32		ulCaSysIdIdx = 0;
	HUINT32		ulESCaDescIdx = 0;
	HUINT32		ulCaDescIdx = 0;

	/* check parameters */
	if(pCaInst == NULL)
	{
		HxLOG_Error("instance is invalid (NULL) \n");
		return ERR_FAIL;
	}

	if(pPmtInfo == NULL)
	{
		HxLOG_Error("pPmtInfo is invalid (NULL) \n");
		return ERR_FAIL;
	}

	/* compare descriptor & caInst sys id */
	if(pPmtInfo->bProgramCaFlag == TRUE)
	{
		HxLOG_Info("number of program ca descriptor (%d)  \n", pPmtInfo->ulNumOfProgramCaDescriptor);
		for (ulCaDescIdx = 0; ulCaDescIdx < pPmtInfo->ulNumOfProgramCaDescriptor; ulCaDescIdx++)
		{
			for (ulSlotIdx = 0; ulSlotIdx < CAS_MAX_NUM_OF_SLOT; ulSlotIdx++)
			{
				for (ulCaSysIdIdx = 0; ulCaSysIdIdx < pCaInst->ulNumCaSysId[ulSlotIdx]; ulCaSysIdIdx++)
				{
					SvcCas_CaDescriptor_t	*pstProgramCaDescriptor = &pPmtInfo->stProgramCaDescriptor[ulCaDescIdx];
					HUINT16				usInstCaSysId			= pCaInst->ausCaSysId[ulSlotIdx][ulCaSysIdIdx];

					HxLOG_Info("Instance ca sys id(0x%x), pmt global ca sys id(0x%x) \n"
						, usInstCaSysId, pPmtInfo->stProgramCaDescriptor[ulCaDescIdx].usCaSysId);

					if ((usInstCaSysId & usCasIdMask) == (pstProgramCaDescriptor->usCaSysId & usCasIdMask))
					{
						return ERR_OK;
					}
				}	// end for (ulCaSysIdIdx
			}	// end for (ulSlotIdx
		}	// end for (ulCaDescIdx
	}

	/* compare descriptor & caInst sys id */
	HxLOG_Info("number of element ca descriptor (%d)  \n", pPmtInfo->ulNumOfElement);

	for (ulESIdx = 0; ulESIdx < pPmtInfo->ulNumOfElement; ulESIdx++)
	{
		HxLOG_Info("es idx (%d), es ca flag(%d) \n", ulESIdx, pPmtInfo->stElementInfo[ulESIdx].bElementCaFlag);
		if(pPmtInfo->stElementInfo[ulESIdx].bElementCaFlag == FALSE)
			continue;

		for (ulSlotIdx = 0; ulSlotIdx < CAS_MAX_NUM_OF_SLOT; ulSlotIdx++)
		{
			if(pCaInst->bHaveDevice[ulSlotIdx] == FALSE)
			{
				HxLOG_Info("continue\n");
				continue;
			}

			for (ulCaSysIdIdx = 0; ulCaSysIdIdx < pCaInst->ulNumCaSysId[ulSlotIdx]; ulCaSysIdIdx++)
			{
				HxLOG_Info("in\n");

				for (ulESCaDescIdx = 0; ulESCaDescIdx < pPmtInfo->stElementInfo[ulESIdx].ulNumOfCaDescriptor; ulESCaDescIdx++)
				{
					SvcCas_CaDescriptor_t	*pstElementCaDescriptor = &pPmtInfo->stElementInfo[ulESIdx].stElementCaDescriptor[ulESCaDescIdx];
					HUINT16				usInstCaSysId			= pCaInst->ausCaSysId[ulSlotIdx][ulCaSysIdIdx];

					HxLOG_Info("Instance ca sys id(0x%x), pmt partial ca sys id(0x%x) \n", usInstCaSysId, pstElementCaDescriptor->usCaSysId);

					if(	(usInstCaSysId & usCasIdMask) == (pstElementCaDescriptor->usCaSysId & usCasIdMask) )
					{
						return ERR_OK;
					}
				}	// for(ulESCaDescIdx
			}	// for(ulCaSysIdIdx
		}	// for(ulSlotIdx
	}	// for (ulESIdx

	return ERR_FAIL;
}

STATIC void svc_cas_scenario_MakePTFromGKindOfAction(SvcCas_MgrPriorityTreeMgr_t 		*pstTreeMgr,
														SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
														SvcCas_GroupOfPriority_t		*pstGroupOfPriority,
														HUINT32 						 ulRequestPriority,
														SvcCas_PmtInfo_t 				*pstRequestPmtInfo,
														HUINT32 						 ulReqActionId)
{
	HUINT16					usCasIdMask = 0xFFFF;
	HUINT32					i;
	HUINT32					ulExistingActionPriority;
	HUINT32					ulStopActionId;
	HERROR					hErr;
	SvcCas_Context_t			*pstCurCasContext = NULL;
	SvcCas_Context_t			*pstCasContext = NULL;
	SvcCas_DevInstance_t		*pstCasInst = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpFirstItem  = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstStopItem = NULL;

	if(pstTreeMgr == NULL || pstFirstItem == NULL)
	{
		HxLOG_Info("pstTreeMgr = 0x%x, pstFirstItem = 0x%x\n", pstTreeMgr, pstFirstItem);
		return;
	}

	pstCurCasContext = svc_cas_MgrGetActionContextByActionId(ulReqActionId);
	if(pstCurCasContext == NULL)
	{
		HxLOG_Info("pstCurCasContext is NULL \n");
		return;
	}

	pstTmpFirstItem = pstFirstItem;

	pstTmpItem = pstTmpFirstItem;
	while(pstTmpItem)
	{
		HxLOG_Info("pstTmpItem->unCasInstTableId = %d\n", pstTmpItem->unCasInstTableId);
		pstCasInst = svc_cas_InstanceGetCasDevInst(pstTmpItem->unCasInstTableId);
		if(pstCasInst == NULL)
		{
			pstTmpItem = pstTmpItem->pstNextSilbing;
			HxLOG_Error("get cas dev inst fail\n");
			continue;
		}

		if(pstCasInst->ulNumOfMaxSvc == 0)
		{
			pstTmpItem = pstTmpItem->pstNextSilbing;
			HxLOG_Info("pstCasInst->ulNumOfMaxSvc is zero\n");
			continue;
		}

		/*	리소스가 없는 경우다. 리소스가 있으면 priority에 상관없이 걍.. 선택되면 된다.. 하지만 없으면 뺏길수도 있다능..
			-> 기존에 play되고 있는 녀석중에 ca system id가 같고 우선 순위가 낮은 녀석을 가지고 온다.
			-> 기존에 play되고 있던 녀석(ulExistingActionPriority)의 priority가 더 낮으면 뺏어 오도록 하자.
			-> 기존에 play되고 있던 녀석이 descramble가 제대로 되고 있지 않은 상태이면 걍 뺏어 오도록 하자.
			-> 같은 서비스가 CI에서 이미 descramble되고 있다면 리소스가 없어도 선택이 될수 있다.
		*/
		HxLOG_Info("pstCasInst->ulNumOfMaxSvc = %d, pstCasInst->ulCurRefCnt = %d\n", pstCasInst->ulNumOfMaxSvc, pstCasInst->ulCurRefCnt);
		if(pstCasInst->ulNumOfMaxSvc <= pstCasInst->ulCurRefCnt)
		{
			//같은 서비스가 CI에서 이미 descramble되고 있다면 리소스가 없어도 선택이 될수 있다.
			HxLOG_Info("pstCasInst->eCasInstType = %d \n", pstCasInst->eCasInstType);
			if(pstCasInst->eCasInstType == eCasInst_CI)
			{
#if defined(CONFIG_DEBUG)
				HxLOG_Info("pstCurCasContext->eActType = %s\n", svc_cas_ScenarioMakeStrActionType(pstCurCasContext->eActType));
#endif
#if 0
				/*	TSR Play일 경우는 핸들이 -1이다. 어차피 TSR REC과 공유 해야 하기 때문에 지나가도록 한다.
					-> 이렇게 될 경우 CAM 입장에서 보았을 때는 서로 다른 입력 2개 서비스를 하는 것과 같아진다. */
				if(pstCurCasContext->eActType == eCAS_ActionType_Tsr_Play)
				{
					pstTmpItem = pstTmpItem->pstNextSilbing;
					continue;
				}
#endif
				//pstCasInst->aulActionId[0] 밖에 안보는 이유는 어차피 CI는 TS 하나만 입력 받는다. 해서 0 ~ CAS_MAX_NUM_OF_ACTION 까지 모두 같은 TS 여야 한다.
				HxLOG_Info("ulReqActionId = %d, pstCasInst->aulActionId[0] = %d\n", ulReqActionId, pstCasInst->aulActionId[0]);
				pstCasContext = svc_cas_MgrGetActionContextByActionId(pstCasInst->aulActionId[0]);
				if(pstCasContext != NULL)
				{
					HxLOG_Info("pstCurCasContext->hSvc = %d, pstCasContext->hSvc = %d\n", pstCurCasContext->hSvc, pstCasContext->hSvc);
					if(pstCurCasContext->hSvc == pstCasContext->hSvc)
					{
						pstTmpItem = pstTmpItem->pstNextSilbing;
						continue;
					}
				}
			}

			for(i = 0; i < pstCasInst->ulCurRefCnt; i++)
			{
				if(ulReqActionId == pstCasInst->aulActionId[i])
				{
					continue;
				}

				pstCasContext = svc_cas_MgrGetActionContextByActionId(pstCasInst->aulActionId[i]);
				if(pstCasContext == NULL)
				{
					continue;
				}

				/*	기존에 FTA가 선택되어서 play되고 있다면 뺏어올수 있다.. 당근 FTA는 리소스를 안잡아 먹어도 되니깐..
				 	FTA가 play되는것도 sub CAS에서 알기를 원하는 경우도 있어서 아래와 같은 상황이 나올수 있다.
				 */
				if((pstCurCasContext->eChType == eCasChannel_Scramble) && (pstCasContext->eChType == eCasChannel_FTA))
				{
					ulStopActionId = pstCasInst->aulActionId[i];
					pstStopItem = pstTmpItem;
					break;
				}

#if 0
				if((pstCurCasContext->eChType == eCasChannel_FTA) && (pstCasContext->eChType == eCasChannel_FTA))
				{
					ulStopActionId = pstCasInst->aulActionId[i];
					pstStopItem = pstTmpItem;
					break;
				}
#endif

				// 뺏어 오려면 먼저 기존에 play되고 있는 cas context와 연결되 CAS와  현재 요청된 녀석의 Ca system id가 일치 해야 한다.
				//hErr = svc_cas_InstanceCheckMatchCaDescriptorByPmt( pstRequestPmtInfo, &pstCasContext->stPmtInfo, usCasIdMask);
				hErr = svc_cas_scenario_CheckMatchCaDescriptorByCaInst(pstCasInst, pstRequestPmtInfo, usCasIdMask);
				if(hErr != ERR_OK)
				{
					continue;
				}

				ulExistingActionPriority = svc_cas_scenario_GetPriorityByActionType(pstGroupOfPriority, pstCasContext->eActType);

				if((ulExistingActionPriority < ulRequestPriority) || (pstCasContext->state == eCAS_State_CasFail))
				{
					ulStopActionId = pstCasInst->aulActionId[i];
					pstStopItem = pstTmpItem;
					break;
				}

#if defined(CONFIG_MW_MM_DELAYED_REC)
				if(pstCurCasContext->eActType == eCAS_ActionType_Tsr_Rec && pstCasContext->eActType == eCAS_ActionType_Tsr_Rec)
				{
					ulStopActionId = pstCasInst->aulActionId[i];
					pstStopItem = pstTmpItem;
					break;
				}
#endif
			}

			/*  찾았으면 stop 될 녀석이다. 못찾으면 삭제 된다. -> 리소스가 없으면 sub CAS에 전달하지 않는 시나리오임.
				pstStopItem가 NULL이면 못찾은 거다.
			*/
			if(pstStopItem == NULL)
			{
				pstTmpFirstItem = svc_cas_ScenarioDeletePriorityTreeItem(pstTreeMgr, pstTmpFirstItem, pstTmpItem);
			}
			else
			{
				pstStopItem->bStop = TRUE;
				pstStopItem->ulStopActionId = ulStopActionId;
			}
		}

		pstTmpItem = pstTmpItem->pstNextSilbing;
	}

/* 다음 레벨로 이동.*/
	if(pstTmpFirstItem != NULL)
	{
		svc_cas_scenario_MakePTFromGKindOfAction(	pstTreeMgr,
														pstTmpFirstItem->pstChild,
														pstGroupOfPriority,
														ulRequestPriority,
														pstRequestPmtInfo,
														ulReqActionId);
	}

	return;
}


STATIC HERROR svc_cas_scenario_IsSameDefSlotFromItem(SvcCas_MgrPriorityTreeItem_t *pstItem, SvcCas_ItemOfPriority_e eItemOfPriority)
{
	HUINT32				ulSlot;
	SvcCas_InstanceType_e	eCasInstType;

	eCasInstType = svc_cas_scenario_GetInstType(eItemOfPriority, &ulSlot);

	return svc_cas_ScenarioIsSameDefSlot(pstItem, eCasInstType, ulSlot);
}

STATIC void svc_cas_scenario_MakePTFromGSlotByDB (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
													SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
													SvcCas_ItemOfPriority_e			 eItemOfPriority)
{
	HUINT32 					ulNumOfItem;
	HUINT32 					ulNumOfMoveItem;
	HERROR					hErr;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpFirstItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstTmpItem = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstMoveItems = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstDeleteItem = NULL;

	if(pstTreeMgr == NULL || pstFirstItem == NULL)
	{
		HxLOG_Info("pstTreeMgr = 0x%x, pstFirstItem = 0x%x\n", pstTreeMgr, pstFirstItem);
		return;
	}

/* 현재 tree level에 우선 순위가 낮아서 아래로 옮겨야 할 Item이 있는가?
	없으면 다음 level에서 찾도록 recursive 호출을 하도록 하자.
	옮겨야할 아이템과 개수와  현재 level의 아이템 개수가 같으면 마찬 가지로  recursive 호출
*/
	pstTmpFirstItem = pstFirstItem;
	ulNumOfItem = svc_cas_scenario_CountPriorityTreeItemFromSameLevel(pstTmpFirstItem);
	ulNumOfMoveItem = svc_cas_scenario_GetNumOfDefSlot(pstTmpFirstItem, eItemOfPriority);

	HxLOG_Info("ulNumOfItem = %d, ulNumOfMoveItem = %d\n", ulNumOfItem, ulNumOfMoveItem);
	if(ulNumOfMoveItem == 0 || ulNumOfItem == ulNumOfMoveItem)
	{
		/* 다음 레벨로 이동.*/
		svc_cas_scenario_MakePTFromGSlotByDB(pstTreeMgr, pstTmpFirstItem->pstChild, eItemOfPriority);
		return;
	}

/* 옮겨야 할 item을 찾는다.
	우선 순위에 따라서 slot을 정렬 하도록 한다.
*/
	pstTmpItem = pstTmpFirstItem;
	while(pstTmpItem)
	{
		hErr = svc_cas_scenario_IsSameDefSlotFromItem(pstTmpItem, eItemOfPriority);
		if(hErr != ERR_OK)
		{
			pstDeleteItem = pstTmpItem;
		}
		pstTmpItem = pstTmpItem->pstNextSilbing;

		if(pstDeleteItem != NULL)
		{
			/*	match 되지 않으면 tree에서 제거후 pstMoveItems 으로 옮긴 뒤 후에 한꺼 번에 아래 level로 옮긴다.
			*/
			pstTmpFirstItem = svc_cas_ScenarioMakeMoveItem( pstTreeMgr,
															pstTmpFirstItem,
															pstDeleteItem,
															&pstMoveItems);
			pstDeleteItem = NULL;
		}
	}

/* 옮길 아이템들을 바로 아래 tree로 붙인다. - 실제 옮기는 작업임..*/
	if(pstMoveItems != NULL)
	{
		pstTmpFirstItem = svc_cas_ScenarioMovePriorityTreeItems(pstTmpFirstItem, pstMoveItems);
	}

/* 다음 레벨로 이동.*/
	if(pstTmpFirstItem != NULL)
	{
		svc_cas_scenario_MakePTFromGSlotByDB(pstTreeMgr, pstTmpFirstItem->pstChild, eItemOfPriority);
	}

	return;
}


/********************************************************************
* Static Functions
* Naming rule : "CasMgrScenario" _ "Function Name"
********************************************************************/
#define ___07_Internal_APIs___

SvcCas_MgrPriorityTreeItem_t *svc_cas_ScenarioDeletePriorityTreeItem(SvcCas_MgrPriorityTreeMgr_t *pstTreeMgr, SvcCas_MgrPriorityTreeItem_t *pstFirst, SvcCas_MgrPriorityTreeItem_t *pstDeleteItem)
{
	if(pstFirst == NULL || pstDeleteItem == NULL)
	{
		HxLOG_Error("arg error pstFirst(0x%x), pstMoveItem(0x%x)\n", pstFirst, pstDeleteItem);
		return NULL;
	}

	if(pstFirst == pstDeleteItem)
	{
		if(pstFirst->pstParent == NULL)
		{
			if(pstFirst->pstNextSilbing == NULL)
			{
				/* 이곳으로 들어 오면 pstTreeMgr->pstHead 가 NULL이 될수도 있다.*/
				pstTreeMgr->pstHead = pstFirst->pstChild;
				if(pstFirst->pstChild != NULL)
				{
					pstFirst->pstChild->pstParent = NULL;
				}
			}
			else
			{
				pstTreeMgr->pstHead = pstFirst->pstNextSilbing;
				pstFirst->pstNextSilbing->pstParent = NULL;
				pstFirst->pstNextSilbing->pstPrevSilbing = NULL;
				pstFirst->pstNextSilbing->pstChild = pstFirst->pstChild;
			}
			return pstTreeMgr->pstHead;
		}
		else
		{
			if(pstFirst->pstNextSilbing == NULL)
			{
				pstFirst->pstParent->pstChild = pstFirst->pstChild;

				if(pstFirst->pstChild != NULL)
				{
					pstFirst->pstChild->pstParent = pstFirst->pstParent;
				}
				return pstFirst->pstChild;
			}
			else
			{
				pstFirst->pstParent->pstChild = pstFirst->pstNextSilbing;
				pstFirst->pstNextSilbing->pstPrevSilbing = NULL;
				pstFirst->pstNextSilbing->pstChild = pstFirst->pstChild;
				return pstFirst->pstNextSilbing;
			}
		}
	}
	else
	{
		pstDeleteItem->pstPrevSilbing->pstNextSilbing = pstDeleteItem->pstNextSilbing;
	}

	return pstFirst;
}

SvcCas_MgrPriorityTreeItem_t *svc_cas_ScenarioMakeMoveItem (SvcCas_MgrPriorityTreeMgr_t		 *pstTreeMgr,
															SvcCas_MgrPriorityTreeItem_t	 *pstFirstItem,
															SvcCas_MgrPriorityTreeItem_t	 *pstDeleteItem,
															SvcCas_MgrPriorityTreeItem_t	**ppstMoveItem)
{
	SvcCas_MgrPriorityTreeItem_t *pstRetFirstItem = pstFirstItem;

	if(pstTreeMgr == NULL || pstFirstItem == NULL || pstDeleteItem == NULL || ppstMoveItem == NULL)
	{
		return pstRetFirstItem;
	}

	pstRetFirstItem = svc_cas_ScenarioDeletePriorityTreeItem(pstTreeMgr, pstFirstItem, pstDeleteItem);

	pstDeleteItem->pstParent		= NULL;
	pstDeleteItem->pstNextSilbing	= NULL;
	pstDeleteItem->pstPrevSilbing	= NULL;
	pstDeleteItem->pstChild		= NULL;

	if(*ppstMoveItem == NULL)
	{
		*ppstMoveItem = pstDeleteItem;
	}
	else
	{
		pstDeleteItem->pstNextSilbing = *ppstMoveItem;
		*ppstMoveItem = pstDeleteItem;
	}

	return pstRetFirstItem;
}

SvcCas_MgrPriorityTreeItem_t *svc_cas_ScenarioMakeFirstPriorityTree(HUINT32 ulSearchCasInstId, SvcCas_Context_t *pstContext, HBOOL bSearchAll)
{
	HUINT32					i;
	SvcCas_MgrPriorityTreeItem_t	*pstFirstPriorityTree = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstPrevPriorityTree = NULL;
	SvcCas_MgrPriorityTreeItem_t	*pstPriorityTree = NULL;
	SvcCas_DevInstance_t		*pstCasDevInst = NULL;

	if(pstContext == NULL)
	{
		HxLOG_Error("pstContext is NULL \n");
		return NULL;
	}

	svc_cas_scenario_InitPriorityTreeNode();

	if(ulSearchCasInstId != HANDLE_NULL)
	{
		pstFirstPriorityTree = svc_cas_scenario_GetEmptyPriorityTreeNode();
		if(pstFirstPriorityTree == NULL)
		{
			HxLOG_Error("get empty priotity tree node fail \n");
			return NULL;
		}

		pstFirstPriorityTree->unCasInstTableId = ulSearchCasInstId;

		return pstFirstPriorityTree;
	}

	for(i = 0; i < CAS_MAX_NUM_OF_INSTANCE; i++)
	{
		/* search all이 아닌 경우는 기존에 시도해본 녀석은 pass하도록 한다. */
		if(bSearchAll == FALSE && pstContext->abTryCasInst[i] == TRUE)
		{
			continue;
		}

		pstCasDevInst = svc_cas_InstanceGetCasDevInst(i);
		if(pstCasDevInst == NULL)
		{
			HxLOG_Error("get cas dev inst fail \n");
			return pstFirstPriorityTree;
		}

		pstPriorityTree = svc_cas_scenario_GetEmptyPriorityTreeNode();
		if(pstPriorityTree == NULL)
		{
			HxLOG_Error("get empty priotity tree node fail \n");
			return NULL;
		}

		pstPriorityTree->unCasInstTableId = i;

		if(pstPrevPriorityTree == NULL)
		{
			pstFirstPriorityTree	= pstPriorityTree;
			pstPrevPriorityTree	= pstPriorityTree;
		}
		else
		{
			pstPrevPriorityTree->pstNextSilbing = pstPriorityTree;
			pstPriorityTree->pstPrevSilbing = pstPrevPriorityTree;
			pstPrevPriorityTree = pstPriorityTree;
		}
	}

	return pstFirstPriorityTree;
}

SvcCas_MgrPriorityTreeItem_t *svc_cas_ScenarioMovePriorityTreeItems(SvcCas_MgrPriorityTreeItem_t *pstFirst, SvcCas_MgrPriorityTreeItem_t *pstMoveItems)
{
	if(pstFirst == NULL || pstMoveItems == NULL)
	{
		HxLOG_Error("arg error pstFirst(0x%x), pstMoveItem(0x%x)\n", pstFirst, pstMoveItems);
		return NULL;
	}

	pstMoveItems->pstChild = pstFirst->pstChild;
	pstMoveItems->pstParent = pstFirst;
	pstFirst->pstChild = pstMoveItems;

	return pstMoveItems;
}

/*	등록된 순서대로 우선 순위가 매겨진다.
*/
HERROR svc_cas_ScenarioRegisterPriorityTable(SvcCas_GroupOfPriority_t *pstGroupOfPriority)
{
	if(pstGroupOfPriority == NULL)
	{
		HxLOG_Error("arg error \n");
		return ERR_FAIL;
	}

	if(s_ulNumOfPriorityTable >= MW_CAS_MAX_PRIORITY_GROUP_ITEM_COUNT)
	{
		HxLOG_Error("register count over, s_unNumOfPriorityTable is (%d) \n", s_ulNumOfPriorityTable);
		return ERR_FAIL;
	}

	s_astPriorityTable[s_ulNumOfPriorityTable] = *pstGroupOfPriority;
	s_ulNumOfPriorityTable++;

	return ERR_OK;
}

HERROR svc_cas_ScenarioIsExistDevice(SvcCas_MgrPriorityTreeItem_t *pstPriorityTree, SvcCas_InstanceType_e *peCasInstType)
{
	HBOOL				bFindDevice;
	HUINT32				i;
	SvcCas_DevInstance_t	*pstCasInst;

	pstCasInst = svc_cas_InstanceGetCasDevInst(pstPriorityTree->unCasInstTableId);
	if(pstCasInst == NULL)
	{
		return ERR_FAIL;
	}
	*peCasInstType = pstCasInst->eCasInstType;

	if(pstCasInst->eCasInstType == eCasInst_CI)
	{
		if(pstCasInst->eCamState == eCAS_CAM_STATE_Initialized)
		{
			return ERR_OK;
		}
	}
	else
	{
		bFindDevice = FALSE;
		for(i = 0; i <= CAS_MAX_NUM_OF_SLOT; i++)
		{
			if(pstCasInst->bHaveDevice[i] == TRUE)
			{
				bFindDevice = TRUE;
				break;
			}
		}

		if(bFindDevice == TRUE)
		{
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

SvcCas_GroupOfPriority_t *svc_cas_ScenarioGetPriorityTable(HUINT32 unTableId)
{
	if (unTableId >= s_ulNumOfPriorityTable)
	{
		return NULL;
	}

	return &s_astPriorityTable[unTableId];
}

HERROR	svc_cas_ScenarioIsCasSystemIdMatch(SvcCas_MgrPriorityTreeItem_t *pstItem, SvcCas_Context_t *pstCasContext)
{
	HUINT16				usCasIdMask = 0xFFFF;
	SvcCas_DevInstance_t	*pstCasInst;

	pstCasInst = svc_cas_InstanceGetCasDevInst(pstItem->unCasInstTableId);
	if(pstCasInst == NULL)
	{
		return ERR_FAIL;
	}

	return  svc_cas_scenario_CheckMatchCaDescriptorByCaInst( pstCasInst, &pstCasContext->stPmtInfo, usCasIdMask);
}

HERROR	svc_cas_ScenarioIsCamIdMatch(SvcCas_MgrPriorityTreeItem_t *pstItem, SvcCas_Context_t *pstCasContext)
{
#if defined(CONFIG_MW_CI)
	HERROR				hErr;
	HUINT8				aucCurrentCamId[8];
	SvcCas_DevInstance_t	*pstCasInst;

	if(pstItem == NULL || pstCasContext == NULL)
	{
		return ERR_FAIL;
	}

	if(pstCasContext->stCasStartData.eCasGroupId != eDxCAS_GROUPID_CAM)
	{
		return ERR_FAIL;
	}

	pstCasInst = svc_cas_InstanceGetCasDevInst(pstItem->unCasInstTableId);
	if(pstCasInst == NULL)
	{
		return ERR_FAIL;
	}

	if(pstCasInst->eCasId != eDxCAS_GROUPID_CAM)
	{
		return ERR_FAIL;
	}

	hErr = psvc_cas_CiGetBindedCamId(pstCasInst->ulSlot, aucCurrentCamId);
	if(hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	if(HxSTD_memcmp(aucCurrentCamId, pstCasContext->stCasStartData.uData.stCasCamStartData.aucCAMID, 8) == 0)
	{
		return ERR_OK;
	}
#endif
	return ERR_FAIL;
}


DxCAS_GroupId_e svc_cas_ScenarioGetCasGroupIdFromCasInst(HUINT32 ulCasInstId)
{
	SvcCas_DevInstance_t	*pstCasInst;

	pstCasInst = svc_cas_InstanceGetCasDevInst(ulCasInstId);

	if(pstCasInst == NULL)
	{
		return eDxCAS_GROUPID_NONE;
	}

	return pstCasInst->eCasId;
}

HERROR svc_cas_ScenarioIsDeviceMatchItem(SvcCas_MgrPriorityTreeItem_t *pstFirstItem, SvcCas_ItemOfPriority_e eItem)
{
	SvcCas_ItemOfPriority_e		eCompareItem;
	SvcCas_DevInstance_t	*pstCasInst;

	pstCasInst = svc_cas_InstanceGetCasDevInst(pstFirstItem->unCasInstTableId);
	if(pstCasInst == NULL)
	{
		return ERR_FAIL;
	}

	eCompareItem = svc_cas_scenario_GetPriorityItemIdByCasInstType(pstCasInst->eCasInstType);
	if(eItem == eCompareItem)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR svc_cas_ScenarioIsSameDefSlot(SvcCas_MgrPriorityTreeItem_t *pstItem, SvcCas_InstanceType_e eCasInstType, HUINT32 ulSlot)
{
	HUINT32				i;
	SvcCas_DevInstance_t	*pstCasInst;

	pstCasInst = svc_cas_InstanceGetCasDevInst(pstItem->unCasInstTableId);
	if(pstCasInst == NULL)
	{
		HxLOG_Error("get cas dev inst fail\n");
		return ERR_FAIL;
	}

	if(eCasInstType == pstCasInst->eCasInstType)
	{
		for(i = 0; i <= CAS_MAX_NUM_OF_SLOT; i++)
		{
			if((pstCasInst->bHaveDevice[i] == TRUE) && (i == ulSlot))
			{
				HxLOG_Info("It find same slot \n");
				return ERR_OK;
			}
		}
	}

	HxLOG_Info("It doesn't find same slot \n");

	return ERR_FAIL;
}

HUINT32  svc_cas_ScenarioGetLargestResourceCount(SvcCas_MgrPriorityTreeItem_t *pstPriorityTree)
{
	HUINT32				unRetNumOfPossibleDes = 0;
	HINT32				nNumOfPossibleDes;
	SvcCas_DevInstance_t	*pstCasInst;

	while(pstPriorityTree)
	{
		pstCasInst = svc_cas_InstanceGetCasDevInst(pstPriorityTree->unCasInstTableId);
		if(pstCasInst != NULL)
		{
			nNumOfPossibleDes = pstCasInst->ulNumOfMaxSvc - pstCasInst->ulCurRefCnt;
			if(unRetNumOfPossibleDes < nNumOfPossibleDes)
			{
				unRetNumOfPossibleDes = nNumOfPossibleDes;
			}
		}

		pstPriorityTree = pstPriorityTree->pstNextSilbing;
	}

	return unRetNumOfPossibleDes;
}

HUINT32	svc_cas_ScenarioGetNumOfLessItemFromLargestResourceCount(SvcCas_MgrPriorityTreeItem_t *pstItem, HUINT32 ulLargestNum)
{
	HUINT32				ulRetNumOfLessItem = 0;
	SvcCas_DevInstance_t	*pstCasInst;

	while(pstItem)
	{
		pstCasInst = svc_cas_InstanceGetCasDevInst(pstItem->unCasInstTableId);
		if(pstCasInst == NULL)
		{
			return ulRetNumOfLessItem;
		}

		if(ulLargestNum > pstCasInst->ulNumOfMaxSvc - pstCasInst->ulCurRefCnt)
		{
			ulRetNumOfLessItem++;
		}

		pstItem = pstItem->pstNextSilbing;
	}

	return ulRetNumOfLessItem;
}

HERROR svc_cas_ScenarioIsLessThenInputArg(SvcCas_MgrPriorityTreeItem_t *pstItem, HUINT32	ulLargestNum)
{
	SvcCas_DevInstance_t	*pstCasInst;

	pstCasInst = svc_cas_InstanceGetCasDevInst(pstItem->unCasInstTableId);
	if(pstCasInst == NULL)
	{
		return ERR_FAIL;
	}

	if(ulLargestNum > pstCasInst->ulNumOfMaxSvc - pstCasInst->ulCurRefCnt)
	{
		return ERR_OK;
	}

	return ERR_FAIL;
}

#if defined(CONFIG_MW_CAS) && defined(CONFIG_MW_CAS_NAGRA) && defined(CONFIG_MW_CAS_NAGRA_OP_SES)
// 아래 시나리오는 1CAS + 1CI에서 CI는 pulCasInstId가 NULL인경우 eSEVT_CAS_DRM_NONED을 보내주지만
// 1CAS NAGRA는 eSEVT_CAS_DRM_NONE로 보내주면 안된다.
// SES는 DRM 정보를 받은다음 REC처리를 해주어야 함으로 pulCasInstId를 만들어서 보내주어야한다.
// 그래야 eSEVT_SI_DRM_SIGNALLED가 전달이 된다.
// 참고> cas 채널 live를 시청중 다른 cas를 rec를 할경우에 발생되는 이슈임
//       현재는 1CAS + 1CI 시나리오에 대한 정의를 정확히 파악후 scenario에 적용되어야함.
//       ( 추후 삭제 예정 )
HERROR svc_cas_PostProcessAfterScenarioSearchInstance(HUINT32 *pulCasInstId)
{
	HUINT32 				i = 0;
	SvcCas_DevInstance_t	*pstCasInstance = NULL;
	HUINT8					bCamState = FALSE;

	HxLOG_Debug("\n");

#if defined (CONFIG_OP_SES)// TODO : 아래 사항은 field에서 pstCasInstance->ulCurRefCnt > 0 이 부분을 없애고 테스트시 문제 없으면 제거하자.
	for(i=0; i<CAS_MAX_NUM_OF_INSTANCE; i++)
	{
		pstCasInstance = svc_cas_InstanceGetCasDevInst(i);

		if(pstCasInstance == NULL)
		{
			continue;
		}

		HxLOG_Debug("\t * [%d] eCasInstType(%d), eCasId(0x%08x), eCamState(%d)\n", i, pstCasInstance->eCasInstType, pstCasInstance->eCasId, pstCasInstance->eCamState);

		if(pstCasInstance->eCasInstType == eCasInst_CI &&  pstCasInstance->eCasId == eDxCAS_GROUPID_CAM && pstCasInstance->eCamState != eCAS_CAM_STATE_Removed)
		{
			HxLOG_Debug("CAM is inserted\n");
			bCamState = TRUE;
			break;
		}
	}

	HxLOG_Debug("\t * bCamState(%d)\n", bCamState);
#endif

	for(i=0; i<CAS_MAX_NUM_OF_INSTANCE; i++)
	{
		pstCasInstance = svc_cas_InstanceGetCasDevInst(i);

		if(pstCasInstance == NULL)
		{
			continue;
		}

		// embeded cas로 동작할수 있는지  즉 nagra에 대해 먼저 check 한다.
		if(pstCasInstance->eCasInstType == eCasInst_Embedded && pstCasInstance->eCasId == eDxCAS_GROUPID_NA && pstCasInstance->eCamState == eCAS_CAM_STATE_Removed)
		{
#if defined (CONFIG_OP_SES) // TODO : 아래 사항은 field에서 pstCasInstance->ulCurRefCnt > 0 이 부분을 없애고 테스트시 문제 없으면 제거하자.
			if(bCamState == TRUE)
			{
				if(pstCasInstance->ulCurRefCnt > 0)
				{
					*pulCasInstId = i;
				}
			}
			else
			{
				*pulCasInstId = i;
			}
#else
			if(pstCasInstance->ulCurRefCnt > 0)
			{
				*pulCasInstId = i;
			}
#endif
			break;
		}
	}

	HxLOG_Debug("pulCasInstId = %d\n",* pulCasInstId);

	return ERR_OK;
}
#endif

HERROR svc_cas_ScenarioSearchCasInstance(SvcCas_Context_t *pstContext, HUINT32 ulSearchCasInstId, HUINT32 *pulCasInstId, HUINT32 *pulStopActionId,HBOOL bSearchAll)
{
	HUINT32					i;
	SvcCas_GroupOfPriority_t			*pstGroupOfPriority;
	SvcCas_MgrPriorityTreeMgr_t	stTreeMgr;

/* Check Error
*/
	if(pstContext == NULL || pulCasInstId == NULL || pulStopActionId == NULL)
	{
		HxLOG_Error("arg error \n");
		return ERR_FAIL;
	}

	/* 등록된 priority table이 없으면 error
			이런 경우가 있으면 안된다. 디버깅 할것.
	*/
	if(s_ulNumOfPriorityTable == 0)
	{
		HxLOG_Error("s_unNumOfPriorityTable(%d) \n", s_ulNumOfPriorityTable);
		goto Return_Err;
	}

/* 등록된 우선 순위 table에서 cb를 실행 하여서 priority tree를 만든다.
	tree에 가장 첫번째 놈이 우선 순위가 가장 높은 녀석임.
*/
	/* 우선 순위 table에 넘길 기본 tree를 만든다. 그냥 등록된 녀석을 차례로 가져온다. */
	stTreeMgr.pstHead = svc_cas_ScenarioMakeFirstPriorityTree(ulSearchCasInstId, pstContext, bSearchAll);

	if(stTreeMgr.pstHead == NULL)
	{
		HxLOG_Error("can't make first priority tree \n");
		goto Return_Err;
	}

	#if defined(CONFIG_DEBUG)
	HxLOG_Print("==================================\n");
	HxLOG_Print("Print - find cas dev instance item\n");
	HxLOG_Print("==================================\n");
	HxLOG_Print("default priority tree\n");
	svc_cas_ScenarioPrintCasMgrPriorityTree(0, stTreeMgr.pstHead);
	#endif

	/* loop를 돌면서 우선 순위 table 실행 */
	for(i = 0; i < s_ulNumOfPriorityTable; i++)
	{
		pstGroupOfPriority = svc_cas_ScenarioGetPriorityTable(i);
		if(pstGroupOfPriority == NULL || pstGroupOfPriority->pfnFindCasInst == NULL)
		{
			HxLOG_Error("(%d) can't find priority table \n", i);
			goto Return_Err;
		}

		pstGroupOfPriority->pfnFindCasInst(&stTreeMgr, stTreeMgr.pstHead, i, pstContext->ulActionId);

		/* 우선 순위 table에서 cb를 실행 하였는데 아무 것도 선택이 되지 못했다.
			예1)  CAM 2개 있는 system에서 cam slot 두개 모두 없는 경우.
			예2) FTA channel을 CAS에서 선택하지 않은 경우. -> CAS 측에서 FTA는 control안 하겠다라고 하는 경우임.
		*/
		if(stTreeMgr.pstHead == NULL)
		{
			HxLOG_Info("can't find cas inst \n");
			goto Return_Err;
		}

		#if defined(CONFIG_DEBUG)
		HxLOG_Print("priority level = %d\n", i);
		svc_cas_ScenarioPrintCasMgrPriorityTree(0, stTreeMgr.pstHead);
		#endif
	}

	if(stTreeMgr.pstHead->unCasInstTableId >= CAS_MAX_NUM_OF_INSTANCE)
	{
		HxLOG_Info("wrong cas inst table id \n");
		goto Return_Err;
	}

	*pulCasInstId = stTreeMgr.pstHead->unCasInstTableId;

	if(stTreeMgr.pstHead->bStop == TRUE)
	{
		*pulStopActionId = stTreeMgr.pstHead->ulStopActionId;
	}
	else
	{
		*pulStopActionId = HANDLE_NULL;
	}

	return ERR_OK;

Return_Err:
	*pulCasInstId		= HANDLE_NULL;
	*pulStopActionId	= HANDLE_NULL;

	if(bSearchAll == TRUE)
	{
		stTreeMgr.pstHead = svc_cas_ScenarioMakeFirstPriorityTree(ulSearchCasInstId, pstContext, bSearchAll);
		if(stTreeMgr.pstHead == NULL)
		{
			HxLOG_Info("can't find cas inst \n");
			return ERR_FAIL;
		}

		*pulCasInstId = stTreeMgr.pstHead->unCasInstTableId;

		return ERR_OK;
	}

	return ERR_FAIL;
}

void svc_cas_ScenarioMakePTFromGSlotByDB(	SvcCas_MgrPriorityTreeMgr_t	*pstTreeMgr,
															SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
															HUINT32					unPriorityTableId,
															HUINT32					ulReqActionId)
{
	HUINT32 				i, ulNumOfPriority = 0;
	SvcCas_GroupOfPriority_t		*pstGroupOfPriority = NULL;
	SvcCas_ItemOfPriority_e		aeItemOfPriority[MW_CAS_MAX_PRIORITY_ITEM_COUNT], eItemOfPriority = 0;
	SvcCas_Context_t 			*pstReqCasContext;
	SvcCas_DbSvcInfo_t		 	stSvcInfo;
	SvcCas_PvrSvcInfo_t			stCasPvrInfo;
	SvcCas_InstanceType_e		eInstType;
	HUINT16						usSlotId;

	HxLOG_Info("++++++++++++++++++++++++++++++++\n");
	HxLOG_Info("svc_cas_ScenarioMakePTFromGSlotByDB() +\n");
	HxLOG_Info("++++++++++++++++++++++++++++++++\n");

	if(pstTreeMgr == NULL || pstFirstItem == NULL)
	{
		HxLOG_Error("pstTreeMgr = 0x%x, pstFirstItem = 0x%x", pstTreeMgr, pstFirstItem);
		return;
	}

	pstReqCasContext = svc_cas_MgrGetActionContextByActionId(ulReqActionId);
	if(pstReqCasContext == NULL)
	{
		HxLOG_Info("pstReqCasContext is NULL \n");
		return;
	}

/* priority table를 가져오도록 한다.*/
	pstGroupOfPriority = svc_cas_MgrSubGetPriorityTable(unPriorityTableId);
	if(pstGroupOfPriority == NULL)
	{
		HxLOG_Error("get group priority fail\n");
		return;
	}

	switch(pstReqCasContext->eActType)
	{
		case eCAS_ActionType_Playback:
		case eCAS_ActionType_Tsr_Play:
			svc_cas_SubSvcPvrGetPvrInfoFromMeta(pstReqCasContext->hAction, &stCasPvrInfo);
			eInstType	= stCasPvrInfo.eInstType;
			usSlotId	= stCasPvrInfo.usSlotId;
			break;

		default:
			if(svc_cas_SubSvcDbGetSvcInfoByServiceHandle(pstReqCasContext->hSvc, &stSvcInfo) != ERR_OK)
			{
				HxLOG_Warning("[svc_cas_SubSvcDbGetSvcInfoByServiceHandle] error\n");
			}
			eInstType	= stSvcInfo.eInstType;
			usSlotId	= stSvcInfo.usSlotId;
			break;
	}

	if(eInstType == eCasInst_Embedded || eInstType == eCasInst_CI)
	{
		if(eInstType == eCasInst_Embedded)
			eItemOfPriority = usSlotId == 0 ? eItemOfPriority_SC0 : eItemOfPriority_SC1;
		else
			eItemOfPriority = usSlotId == 0 ? eItemOfPriority_CAM0 : eItemOfPriority_CAM1;

		aeItemOfPriority[ulNumOfPriority++] = eItemOfPriority;
	}

	for(i = 0 ; i < pstGroupOfPriority->ulNumOfPriority ; i++)
	{
		if(pstGroupOfPriority->aeItemOfPriority[i] == eItemOfPriority)
			continue;

		aeItemOfPriority[ulNumOfPriority++] = pstGroupOfPriority->aeItemOfPriority[i];
	}

	if(pstGroupOfPriority->ulNumOfPriority != ulNumOfPriority)
	{
		HxLOG_Warning("pstGroupOfPriority->ulNumOfPriority (%d), ulNumOfPriority (%d)\n", pstGroupOfPriority->ulNumOfPriority, ulNumOfPriority);
	}

/* priority 아이템 수 만큼 tree를 반복해서 확인한다.
*/
	for(i = 0; i < ulNumOfPriority; i++)
	{
		svc_cas_scenario_MakePTFromGSlotByDB(pstTreeMgr, pstFirstItem, aeItemOfPriority[i]);
	}

	HxLOG_Info("svc_cas_ScenarioMakePTFromGSlotByDB() -\n");
}

void svc_cas_ScenarioMakePTFromGKindOfAction(SvcCas_MgrPriorityTreeMgr_t	*pstTreeMgr,
											SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
											HUINT32						 	 ulPriorityTableId,
											HUINT32							 ulReqActionId)
{
	HUINT32							 ulRequestPriority;
	SvcCas_PmtInfo_t				*pstRequestPmtInfo = NULL;
	SvcCas_Context_t				*pstRequestCasContext = NULL;
	SvcCas_GroupOfPriority_t		*pstGroupOfPriority = NULL;
#if defined(CONFIG_MW_CI)
	SvcCas_MgrPriorityTreeItem_t	*pstTreeRoot;
	SvcCas_DevInstance_t			*pstCasInstance;
	SvcCas_Context_t				*pstCasContext;
	HUINT32							 ulActionPriority, i;
	HBOOL							 bFoundInstance = FALSE;
#endif

	HxLOG_Info("++++++++++++++++++++++++++++++++\n");
	HxLOG_Info("svc_cas_ScenarioMakePTFromGKindOfAction() +\n");
	HxLOG_Info("++++++++++++++++++++++++++++++++\n");

	if(pstTreeMgr == NULL || pstFirstItem == NULL)
	{
		HxLOG_Error("pstTreeMgr = 0x%x, pstFirstItem = 0x%x", pstTreeMgr, pstFirstItem);
		return;
	}

	pstRequestCasContext = svc_cas_MgrGetActionContextByActionId(ulReqActionId);
	if(pstRequestCasContext == NULL)
	{
		HxLOG_Error("get action context fail\n");
		return;
	}

	/* priority table를 가져오도록 한다.*/
	pstGroupOfPriority = svc_cas_ScenarioGetPriorityTable(ulPriorityTableId);
	if(pstGroupOfPriority == NULL)
	{
		HxLOG_Error("get group priority fail\n");
		return;
	}

	ulRequestPriority = svc_cas_scenario_GetPriorityByActionType(pstGroupOfPriority, pstRequestCasContext->eActType);
	pstRequestPmtInfo = &pstRequestCasContext->stPmtInfo;

	svc_cas_scenario_MakePTFromGKindOfAction(pstTreeMgr, pstFirstItem, pstGroupOfPriority, ulRequestPriority, pstRequestPmtInfo, ulReqActionId);

#if defined(CONFIG_MW_CAS) && defined(CONFIG_MW_CI)
	// CAM slot 1개와  S/C slot이1개 있는 경우
	// Head 즉, 이전 시점까지 선택된 priority가 높은 CAS Instance가 embeddedd CAS 이고, descramble 할 수 있는 상황이라면 바로 return을 해주도록 한다.
	// svc_cas_ScenarioMakePTFromGKindOfAction 시나리오가 많이 꼬여있어 추후 정리가 필요 함.
	pstTreeRoot = pstTreeMgr->pstHead;

	if(pstTreeRoot)
	{
		pstCasInstance = svc_cas_InstanceGetCasDevInst(pstTreeRoot->unCasInstTableId);
		if(pstCasInstance != NULL)
		{
			if(pstCasInstance->eCasInstType == eCasInst_Embedded && pstCasInstance->ulCurRefCnt < pstCasInstance->ulNumOfMaxSvc)
			{
				HxLOG_Info("svc_cas_ScenarioMakePTFromGKindOfAction() -\n");
				return;
			}
		}
	}
#endif

#if defined(CONFIG_MW_CI)
	// TODO: 2-CI가 삽입된 상태에서의 시나리오, 실제로는 CAS scenario에 들어가야 하는 내용이다. 일종의 WA code 이다. scenario 보강작업 이후 삭제
	/*	삽입된 CAM들 중에서 현재 요청한 서비스와 동일한 서비스를 이미 descramble 하고 있는지 확인한다. (Slot priority의 보정)
		-> CAM #0 (Live + Recording) + CAM #1 (Recording) 상태에서 CAM #1(Recording)으로 서비스 전환시 Live가 CAM #0에 할당된다. */
	if(bFoundInstance == FALSE)
	{
		pstTreeRoot = pstTreeMgr->pstHead;
		while(pstTreeRoot)
		{
			pstCasInstance = svc_cas_InstanceGetCasDevInst(pstTreeRoot->unCasInstTableId);
			if(pstCasInstance != NULL)
			{
				for(i = 0; i < pstCasInstance->ulCurRefCnt; i++)
				{
					pstCasContext = svc_cas_MgrGetActionContextByActionId(pstCasInstance->aulActionId[i]);
					if(pstRequestCasContext->hSvc == pstCasContext->hSvc)
					{
						HxLOG_Print("[%s:%d] By hSvc - FOUND...\n", __FUNCTION__, __HxLINE__);
						break;
					}
				}

				if(i < pstCasInstance->ulCurRefCnt)
				{
					if(pstTreeMgr->pstHead != pstTreeRoot)
					{
						HxLOG_Print("[%s:%d] By hSvc - CHANGE (%d -> %d)...\n", __FUNCTION__, __HxLINE__, pstTreeMgr->pstHead->unCasInstTableId, pstTreeRoot->unCasInstTableId);
						pstTreeMgr->pstHead = pstTreeRoot;
					}

					bFoundInstance = TRUE;
					pstTreeMgr->pstHead->bStop = FALSE;
					break;
				}
			}

			if(pstTreeRoot->pstNextSilbing)
				pstTreeRoot = pstTreeRoot->pstNextSilbing;
			else if(pstTreeRoot->pstChild)
				pstTreeRoot = pstTreeRoot->pstChild;
			else
				pstTreeRoot = NULL;
		}
	}

	/* 요청된 서비스의 descramble이 가능한 다른 CAM을 찾아본다. (Action priority의 보정)
		-> CAM #0 (Live + Recording) + CAM #1 (None) 상태에서 서비스 전환시
		CAM #0 (Recording) + CAM #1(Live + TSR)이 되어야 하나, CAM #0 (None) + CAM #0 (Live + TSR) 구조로 된다. */
	if(bFoundInstance == FALSE)
	{
		pstTreeRoot = pstTreeMgr->pstHead;
		while(pstTreeRoot)
		{
			pstCasInstance = svc_cas_InstanceGetCasDevInst(pstTreeRoot->unCasInstTableId);
			if(pstCasInstance != NULL)
			{
// TODO: swlee3 should be deleted
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
				// CAS와 CAM이 동일하게 사용 가능한경우 CAS를 우선순위로 둔다.
				// ex) SKY CAM과 NAGRA CAS가 삽입된경우 RTL HD로 녹화후 VOX HD로 채널전환하면 CAM으로 path가 설정되는 문제 해결..
				if(
					/*pstTreeRoot->unCasInstTableId == 0 						&& */
					pstCasInstance->ulCurRefCnt > 0 						&&
					pstCasInstance->eCamState == eCAS_CAM_STATE_Removed 	&&
					pstCasInstance->eCasInstType == eCasInst_Embedded 		&&
					pstCasInstance->eCasId == eDxCAS_GROUPID_NA )
				{
					break;
				}
#endif
				if(pstCasInstance->ulCurRefCnt == 0)
				{
					HxLOG_Print("[%s:%d] By ulCurRefCnt - FOUND...\n", __FUNCTION__, __HxLINE__);
					if(pstTreeMgr->pstHead != pstTreeRoot)
					{
						HxLOG_Print("[%s:%d] By ulCurRefCnt - CHANGE (%d -> %d)...\n", __FUNCTION__, __HxLINE__, pstTreeMgr->pstHead->unCasInstTableId, pstTreeRoot->unCasInstTableId);
						pstTreeMgr->pstHead = pstTreeRoot;
					}

					bFoundInstance = TRUE;
					break;
				}
			}

			if(pstTreeRoot->pstNextSilbing)
				pstTreeRoot = pstTreeRoot->pstNextSilbing;
			else if(pstTreeRoot->pstChild)
				pstTreeRoot = pstTreeRoot->pstChild;
			else
				pstTreeRoot = NULL;
		}
	}

	// TODO: CAS instance scenario issue가 있는 듯 하다. 코드리뷰 후 삭제.
	if(pstTreeMgr->pstHead != NULL)
	{
		HxLOG_Print("[%s] Check CAS instance (%d)...\n", __FUNCTION__, pstTreeMgr->pstHead->unCasInstTableId);

		pstCasInstance = svc_cas_InstanceGetCasDevInst(pstTreeMgr->pstHead->unCasInstTableId);
		if(pstCasInstance != NULL)
		{
			if(pstCasInstance->ulCurRefCnt == 0)
			{
				return;
			}

			for(i = 0; i < pstCasInstance->ulCurRefCnt; i++)
			{
				pstCasContext = svc_cas_MgrGetActionContextByActionId(pstCasInstance->aulActionId[i]);
				if(pstRequestCasContext->hSvc == pstCasContext->hSvc)
				{
					if(pstCasContext->eActType == eCAS_ActionType_Tsr_Rec && pstRequestCasContext->eActType == eCAS_ActionType_Tsr_Rec)
					{
						pstTreeMgr->pstHead->ulStopActionId = pstCasInstance->aulActionId[i];
						pstTreeMgr->pstHead->bStop = TRUE;
					}
					return;
				}

				ulActionPriority = svc_cas_scenario_GetPriorityByActionType(pstGroupOfPriority, pstCasContext->eActType);
				if(ulActionPriority < ulRequestPriority || pstCasContext->state == eCAS_State_CasFail)
				{
					return;
				}
			}

			HxLOG_Warning("[%s] CAS scenario error..., Replace NULL instance.\n", __FUNCTION__);
			pstTreeMgr->pstHead = NULL;
		}
	}
#endif
	HxLOG_Info("svc_cas_ScenarioMakePTFromGKindOfAction() -\n");
	return;
}

/********************************************************************
* For Debugging
********************************************************************/
#define ___08_Debugging_APIs___

#if defined(CONFIG_DEBUG)
STATIC const HINT8 *svc_cas_ScenarioMakeStrActionType(SvcCas_ActionType_e cmd)
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

STATIC void svc_cas_ScenarioPrintCasMgrPriorityTree(HUINT32 unLevel, SvcCas_MgrPriorityTreeItem_t *pstItem)
{
	HUINT32					i;
	SvcCas_MgrPriorityTreeItem_t	*pstSibling;

	if(pstItem == NULL)
	{
		HxLOG_Error("pstItem is NULL, unLevel = %d\n", unLevel);
		return;
	}

	HxLOG_Print("unLevel = %d : ", unLevel);
	for(i = 0; i < unLevel; i++)
	{
		HxLOG_Print("\t");
	}

	HxLOG_Print("(CasInstId = %d, bStop = %d, StopActionId = %d)", pstItem->unCasInstTableId, pstItem->bStop, pstItem->ulStopActionId);

	for(pstSibling = pstItem->pstNextSilbing; pstSibling != NULL; pstSibling = pstSibling->pstNextSilbing)
	{
		HxLOG_Print(", (CasInstId = %d, bStop = %d, StopActionId = %d)", pstSibling->unCasInstTableId, pstSibling->bStop, pstSibling->ulStopActionId);
	}

	HxLOG_Print("\n");

	if(pstItem->pstChild != NULL)
	{
		svc_cas_ScenarioPrintCasMgrPriorityTree(unLevel+1, pstItem->pstChild);
	}
}

STATIC HINT8 *svc_cas_ScenarioMakeStrPriorityTable(SvcCas_ItemOfPriority_e eItemOfPriority)
{
	switch (eItemOfPriority)
	{
	ENUM_TO_STR(eItemOfPriority_G_CasSystemIdMatch);
		ENUM_TO_STR(eItemOfPriority_G_DeleteNotMatchCaSystemId);

	ENUM_TO_STR(eItemOfPriority_G_ExistDevice);

	ENUM_TO_STR(eItemOfPriority_G_KindOfCas);
		ENUM_TO_STR(eItemOfPriority_UnknownCas);
		ENUM_TO_STR(eItemOfPriority_BCas);
		ENUM_TO_STR(eItemOfPriority_Conax);
		ENUM_TO_STR(eItemOfPriority_Irdeto);
		ENUM_TO_STR(eItemOfPriority_Nagra);
		ENUM_TO_STR(eItemOfPriority_satsa);
		ENUM_TO_STR(eItemOfPriority_Viaccess);
		ENUM_TO_STR(eItemOfPriority_MG);
		ENUM_TO_STR(eItemOfPriority_CR);
		ENUM_TO_STR(eItemOfPriority_ND);
		ENUM_TO_STR(eItemOfPriority_CAM);

	ENUM_TO_STR(eItemOfPriority_G_Resource);

	ENUM_TO_STR(eItemOfPriority_G_KindOfDevice);
		ENUM_TO_STR(eItemOfPriority_ECAS);
		ENUM_TO_STR(eItemOfPriority_CI);

	ENUM_TO_STR(eItemOfPriority_G_OrderOfSlot);
		ENUM_TO_STR(eItemOfPriority_SC0);
		ENUM_TO_STR(eItemOfPriority_SC1);
		ENUM_TO_STR(eItemOfPriority_CAM0);
		ENUM_TO_STR(eItemOfPriority_CAM1);

	ENUM_TO_STR(eItemOfPriority_G_KindOfAction);
		ENUM_TO_STR(eItemOfPriority_Live);
		ENUM_TO_STR(eItemOfPriority_PlayBack);
		ENUM_TO_STR(eItemOfPriority_Tsr_Play);
		ENUM_TO_STR(eItemOfPriority_Recording);
		ENUM_TO_STR(eItemOfPriority_Descramble_a_File);
		ENUM_TO_STR(eItemOfPriority_Tsr_Rec);

	ENUM_TO_STR(eItemOfPriority_G_CamIdMatch);

	ENUM_TO_STR(eItemOfPriority_unKnownItem);
	ENUM_TO_STR(eItemOfPriority_End);
	}
	return ("Unknown");
}

STATIC HINT8 *svc_cas_scenario_MakeStrCamState(SvcCas_CamState_e eCAS_CamState)
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


void svc_cas_ScenarioPrintPriorityTable(void)
{
	HINT8					*pcStr;
	HUINT32					i, j;

	HLIB_CMD_Printf ("\n===================== print priority table for all ========================== \n");

	for(i = 0; i < s_ulNumOfPriorityTable; i++)
	{
		HLIB_CMD_Printf("--------------\n");
		pcStr = svc_cas_ScenarioMakeStrPriorityTable(s_astPriorityTable[i].eItemOfPriority_G);
		HLIB_CMD_Printf("group : %s \n", pcStr);
		HLIB_CMD_Printf("pfnFindCasInst : 0x%x \n", s_astPriorityTable[i].pfnFindCasInst);

		for(j = 0; j < s_astPriorityTable[i].ulNumOfPriority; j++)
		{
			pcStr = svc_cas_scenario_MakeStrCamState(s_astPriorityTable[i].aeItemOfPriority[j]);
			HLIB_CMD_Printf ("\titem : %s \n", pcStr);
		}
		HLIB_CMD_Printf("--------------\n");
	}

	HLIB_CMD_Printf("\n===================== end of print priority table for all =================== \n");

}

#endif

/*********************** End of File ******************************/
