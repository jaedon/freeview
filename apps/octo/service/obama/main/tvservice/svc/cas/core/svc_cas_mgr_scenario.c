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
STATIC SvcCas_MgrPriorityTreeItem_t	s_astCasMgrPriorityNode[CAS_MAX_NUM_OF_INSTANCE+1]; //Ȥ�� 0���� �ɱ�� 1���� +�ߴ�.

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

		/*	���ҽ��� ���� ����. ���ҽ��� ������ priority�� ������� ��.. ���õǸ� �ȴ�.. ������ ������ ������� �ִٴ�..
			-> ������ play�ǰ� �ִ� �༮�߿� ca system id�� ���� �켱 ������ ���� �༮�� ������ �´�.
			-> ������ play�ǰ� �ִ� �༮(ulExistingActionPriority)�� priority�� �� ������ ���� ������ ����.
			-> ������ play�ǰ� �ִ� �༮�� descramble�� ����� �ǰ� ���� ���� �����̸� �� ���� ������ ����.
			-> ���� ���񽺰� CI���� �̹� descramble�ǰ� �ִٸ� ���ҽ��� ��� ������ �ɼ� �ִ�.
		*/
		HxLOG_Info("pstCasInst->ulNumOfMaxSvc = %d, pstCasInst->ulCurRefCnt = %d\n", pstCasInst->ulNumOfMaxSvc, pstCasInst->ulCurRefCnt);
		if(pstCasInst->ulNumOfMaxSvc <= pstCasInst->ulCurRefCnt)
		{
			//���� ���񽺰� CI���� �̹� descramble�ǰ� �ִٸ� ���ҽ��� ��� ������ �ɼ� �ִ�.
			HxLOG_Info("pstCasInst->eCasInstType = %d \n", pstCasInst->eCasInstType);
			if(pstCasInst->eCasInstType == eCasInst_CI)
			{
#if defined(CONFIG_DEBUG)
				HxLOG_Info("pstCurCasContext->eActType = %s\n", svc_cas_ScenarioMakeStrActionType(pstCurCasContext->eActType));
#endif
#if 0
				/*	TSR Play�� ���� �ڵ��� -1�̴�. ������ TSR REC�� ���� �ؾ� �ϱ� ������ ���������� �Ѵ�.
					-> �̷��� �� ��� CAM ���忡�� ������ ���� ���� �ٸ� �Է� 2�� ���񽺸� �ϴ� �Ͱ� ��������. */
				if(pstCurCasContext->eActType == eCAS_ActionType_Tsr_Play)
				{
					pstTmpItem = pstTmpItem->pstNextSilbing;
					continue;
				}
#endif
				//pstCasInst->aulActionId[0] �ۿ� �Ⱥ��� ������ ������ CI�� TS �ϳ��� �Է� �޴´�. �ؼ� 0 ~ CAS_MAX_NUM_OF_ACTION ���� ��� ���� TS ���� �Ѵ�.
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

				/*	������ FTA�� ���õǾ play�ǰ� �ִٸ� ����ü� �ִ�.. ��� FTA�� ���ҽ��� ����� �Ծ �Ǵϱ�..
				 	FTA�� play�Ǵ°͵� sub CAS���� �˱⸦ ���ϴ� ��쵵 �־ �Ʒ��� ���� ��Ȳ�� ���ü� �ִ�.
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

				// ���� ������ ���� ������ play�ǰ� �ִ� cas context�� ����� CAS��  ���� ��û�� �༮�� Ca system id�� ��ġ �ؾ� �Ѵ�.
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

			/*  ã������ stop �� �༮�̴�. ��ã���� ���� �ȴ�. -> ���ҽ��� ������ sub CAS�� �������� �ʴ� �ó�������.
				pstStopItem�� NULL�̸� ��ã�� �Ŵ�.
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

/* ���� ������ �̵�.*/
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

/* ���� tree level�� �켱 ������ ���Ƽ� �Ʒ��� �Űܾ� �� Item�� �ִ°�?
	������ ���� level���� ã���� recursive ȣ���� �ϵ��� ����.
	�Űܾ��� �����۰� ������  ���� level�� ������ ������ ������ ���� ������  recursive ȣ��
*/
	pstTmpFirstItem = pstFirstItem;
	ulNumOfItem = svc_cas_scenario_CountPriorityTreeItemFromSameLevel(pstTmpFirstItem);
	ulNumOfMoveItem = svc_cas_scenario_GetNumOfDefSlot(pstTmpFirstItem, eItemOfPriority);

	HxLOG_Info("ulNumOfItem = %d, ulNumOfMoveItem = %d\n", ulNumOfItem, ulNumOfMoveItem);
	if(ulNumOfMoveItem == 0 || ulNumOfItem == ulNumOfMoveItem)
	{
		/* ���� ������ �̵�.*/
		svc_cas_scenario_MakePTFromGSlotByDB(pstTreeMgr, pstTmpFirstItem->pstChild, eItemOfPriority);
		return;
	}

/* �Űܾ� �� item�� ã�´�.
	�켱 ������ ���� slot�� ���� �ϵ��� �Ѵ�.
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
			/*	match ���� ������ tree���� ������ pstMoveItems ���� �ű� �� �Ŀ� �Ѳ� ���� �Ʒ� level�� �ű��.
			*/
			pstTmpFirstItem = svc_cas_ScenarioMakeMoveItem( pstTreeMgr,
															pstTmpFirstItem,
															pstDeleteItem,
															&pstMoveItems);
			pstDeleteItem = NULL;
		}
	}

/* �ű� �����۵��� �ٷ� �Ʒ� tree�� ���δ�. - ���� �ű�� �۾���..*/
	if(pstMoveItems != NULL)
	{
		pstTmpFirstItem = svc_cas_ScenarioMovePriorityTreeItems(pstTmpFirstItem, pstMoveItems);
	}

/* ���� ������ �̵�.*/
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
				/* �̰����� ��� ���� pstTreeMgr->pstHead �� NULL�� �ɼ��� �ִ�.*/
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
		/* search all�� �ƴ� ���� ������ �õ��غ� �༮�� pass�ϵ��� �Ѵ�. */
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

/*	��ϵ� ������� �켱 ������ �Ű�����.
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
// �Ʒ� �ó������� 1CAS + 1CI���� CI�� pulCasInstId�� NULL�ΰ�� eSEVT_CAS_DRM_NONED�� ����������
// 1CAS NAGRA�� eSEVT_CAS_DRM_NONE�� �����ָ� �ȵȴ�.
// SES�� DRM ������ �������� RECó���� ���־�� ������ pulCasInstId�� ���� �����־���Ѵ�.
// �׷��� eSEVT_SI_DRM_SIGNALLED�� ������ �ȴ�.
// ����> cas ä�� live�� ��û�� �ٸ� cas�� rec�� �Ұ�쿡 �߻��Ǵ� �̽���
//       ����� 1CAS + 1CI �ó������� ���� ���Ǹ� ��Ȯ�� �ľ��� scenario�� ����Ǿ����.
//       ( ���� ���� ���� )
HERROR svc_cas_PostProcessAfterScenarioSearchInstance(HUINT32 *pulCasInstId)
{
	HUINT32 				i = 0;
	SvcCas_DevInstance_t	*pstCasInstance = NULL;
	HUINT8					bCamState = FALSE;

	HxLOG_Debug("\n");

#if defined (CONFIG_OP_SES)// TODO : �Ʒ� ������ field���� pstCasInstance->ulCurRefCnt > 0 �� �κ��� ���ְ� �׽�Ʈ�� ���� ������ ��������.
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

		// embeded cas�� �����Ҽ� �ִ���  �� nagra�� ���� ���� check �Ѵ�.
		if(pstCasInstance->eCasInstType == eCasInst_Embedded && pstCasInstance->eCasId == eDxCAS_GROUPID_NA && pstCasInstance->eCamState == eCAS_CAM_STATE_Removed)
		{
#if defined (CONFIG_OP_SES) // TODO : �Ʒ� ������ field���� pstCasInstance->ulCurRefCnt > 0 �� �κ��� ���ְ� �׽�Ʈ�� ���� ������ ��������.
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

	/* ��ϵ� priority table�� ������ error
			�̷� ��찡 ������ �ȵȴ�. ����� �Ұ�.
	*/
	if(s_ulNumOfPriorityTable == 0)
	{
		HxLOG_Error("s_unNumOfPriorityTable(%d) \n", s_ulNumOfPriorityTable);
		goto Return_Err;
	}

/* ��ϵ� �켱 ���� table���� cb�� ���� �Ͽ��� priority tree�� �����.
	tree�� ���� ù��° ���� �켱 ������ ���� ���� �༮��.
*/
	/* �켱 ���� table�� �ѱ� �⺻ tree�� �����. �׳� ��ϵ� �༮�� ���ʷ� �����´�. */
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

	/* loop�� ���鼭 �켱 ���� table ���� */
	for(i = 0; i < s_ulNumOfPriorityTable; i++)
	{
		pstGroupOfPriority = svc_cas_ScenarioGetPriorityTable(i);
		if(pstGroupOfPriority == NULL || pstGroupOfPriority->pfnFindCasInst == NULL)
		{
			HxLOG_Error("(%d) can't find priority table \n", i);
			goto Return_Err;
		}

		pstGroupOfPriority->pfnFindCasInst(&stTreeMgr, stTreeMgr.pstHead, i, pstContext->ulActionId);

		/* �켱 ���� table���� cb�� ���� �Ͽ��µ� �ƹ� �͵� ������ ���� ���ߴ�.
			��1)  CAM 2�� �ִ� system���� cam slot �ΰ� ��� ���� ���.
			��2) FTA channel�� CAS���� �������� ���� ���. -> CAS ������ FTA�� control�� �ϰڴٶ�� �ϴ� �����.
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

/* priority table�� ���������� �Ѵ�.*/
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

/* priority ������ �� ��ŭ tree�� �ݺ��ؼ� Ȯ���Ѵ�.
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

	/* priority table�� ���������� �Ѵ�.*/
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
	// CAM slot 1����  S/C slot��1�� �ִ� ���
	// Head ��, ���� �������� ���õ� priority�� ���� CAS Instance�� embeddedd CAS �̰�, descramble �� �� �ִ� ��Ȳ�̶�� �ٷ� return�� ���ֵ��� �Ѵ�.
	// svc_cas_ScenarioMakePTFromGKindOfAction �ó������� ���� �����־� ���� ������ �ʿ� ��.
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
	// TODO: 2-CI�� ���Ե� ���¿����� �ó�����, �����δ� CAS scenario�� ���� �ϴ� �����̴�. ������ WA code �̴�. scenario �����۾� ���� ����
	/*	���Ե� CAM�� �߿��� ���� ��û�� ���񽺿� ������ ���񽺸� �̹� descramble �ϰ� �ִ��� Ȯ���Ѵ�. (Slot priority�� ����)
		-> CAM #0 (Live + Recording) + CAM #1 (Recording) ���¿��� CAM #1(Recording)���� ���� ��ȯ�� Live�� CAM #0�� �Ҵ�ȴ�. */
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

	/* ��û�� ������ descramble�� ������ �ٸ� CAM�� ã�ƺ���. (Action priority�� ����)
		-> CAM #0 (Live + Recording) + CAM #1 (None) ���¿��� ���� ��ȯ��
		CAM #0 (Recording) + CAM #1(Live + TSR)�� �Ǿ�� �ϳ�, CAM #0 (None) + CAM #0 (Live + TSR) ������ �ȴ�. */
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
				// CAS�� CAM�� �����ϰ� ��� �����Ѱ�� CAS�� �켱������ �д�.
				// ex) SKY CAM�� NAGRA CAS�� ���ԵȰ�� RTL HD�� ��ȭ�� VOX HD�� ä����ȯ�ϸ� CAM���� path�� �����Ǵ� ���� �ذ�..
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

	// TODO: CAS instance scenario issue�� �ִ� �� �ϴ�. �ڵ帮�� �� ����.
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
