/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  casmgr_defScenario.h						\n
	@brief	  cas module header file		\n

	Description: cas module. \n
	Module: MW/CAS									 					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__CAS_MGR_SCENARIO_H__
#define	__CAS_MGR_SCENARIO_H__

/********************************************************************
* Header Files :  헤더 파일은 가능한 .c 에서 include 할 것 !
********************************************************************/
#define ___01_Header_Files___
#include <octo_common.h>
#include <_svc_cas_mgr_main.h>

/********************************************************************
* Extern Variables / Function Prototypes
********************************************************************/
#define ___02_Extern_Vars_Func_Prototypes___

/********************************************************************
* Module Internal Definitions : #define / enum / struct / union
********************************************************************/
#define ___03_Module_Internal_Definitions___

/********************************************************************
* Module Internal Function Prototypes
********************************************************************/
#define ___04_Module_Internal_Func_Prototypes___

SvcCas_MgrPriorityTreeItem_t *svc_cas_ScenarioDeletePriorityTreeItem(SvcCas_MgrPriorityTreeMgr_t *pstTreeMgr,
																	SvcCas_MgrPriorityTreeItem_t *pstFirst,
																	SvcCas_MgrPriorityTreeItem_t *pstDeleteItem);
SvcCas_MgrPriorityTreeItem_t *svc_cas_ScenarioMakeMoveItem (SvcCas_MgrPriorityTreeMgr_t		 *pstTreeMgr,
															SvcCas_MgrPriorityTreeItem_t	 *pstFirstItem,
															SvcCas_MgrPriorityTreeItem_t	 *pstDeleteItem,
															SvcCas_MgrPriorityTreeItem_t	**ppstMoveItem);
SvcCas_MgrPriorityTreeItem_t *svc_cas_ScenarioMovePriorityTreeItems(SvcCas_MgrPriorityTreeItem_t		*pstFirst,
																		SvcCas_MgrPriorityTreeItem_t	*pstMoveItems);

SvcCas_MgrPriorityTreeItem_t *svc_cas_ScenarioMakeFirstPriorityTree(HUINT32 ulSearchCasInstId, SvcCas_Context_t *pstContext, HBOOL bSearchAll);
HERROR svc_cas_ScenarioRegisterPriorityTable(SvcCas_GroupOfPriority_t *pstGroupOfPriority);
HERROR svc_cas_ScenarioIsExistDevice(SvcCas_MgrPriorityTreeItem_t *pstPriorityTree, SvcCas_InstanceType_e *peCasInstType);
SvcCas_GroupOfPriority_t *svc_cas_ScenarioGetPriorityTable(HUINT32 unTableId);
HERROR	svc_cas_ScenarioIsCasSystemIdMatch(SvcCas_MgrPriorityTreeItem_t *pstItem, SvcCas_Context_t *pstCasContext);
HERROR	svc_cas_ScenarioIsCamIdMatch(SvcCas_MgrPriorityTreeItem_t *pstItem, SvcCas_Context_t *pstCasContext);
DxCAS_GroupId_e svc_cas_ScenarioGetCasGroupIdFromCasInst(HUINT32 ulCasInstId);
HERROR svc_cas_ScenarioIsDeviceMatchItem(SvcCas_MgrPriorityTreeItem_t *pstFirstItem, SvcCas_ItemOfPriority_e eItem);
HERROR svc_cas_ScenarioIsSameDefSlot(SvcCas_MgrPriorityTreeItem_t *pstItem, SvcCas_InstanceType_e eCasInstType, HUINT32 ulSlot);
HUINT32  svc_cas_ScenarioGetLargestResourceCount(SvcCas_MgrPriorityTreeItem_t *pstPriorityTree);
HUINT32	svc_cas_ScenarioGetNumOfLessItemFromLargestResourceCount(SvcCas_MgrPriorityTreeItem_t *pstItem, HUINT32 ulLargestNum);
HERROR svc_cas_ScenarioIsLessThenInputArg(SvcCas_MgrPriorityTreeItem_t *pstItem, HUINT32 ulLargestNum);
HERROR svc_cas_ScenarioSearchCasInstance(SvcCas_Context_t *pstContext, HUINT32 ulSearchCasInstId, HUINT32 *pulCasInstId, HUINT32 *pulStopActionId,HBOOL bSearchAll);
void svc_cas_ScenarioMakePTFromGSlotByDB(SvcCas_MgrPriorityTreeMgr_t	*pstTreeMgr,
										SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
										HUINT32							 unPriorityTableId,
										HUINT32							 ulReqActionId);
void svc_cas_ScenarioMakePTFromGKindOfAction(SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
											SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
											HUINT32							 ulPriorityTableId,
											HUINT32							 ulReqActionId);
HERROR svc_cas_ScenarioIsLastLiveInst(SvcCas_MgrPriorityTreeItem_t *pstItem, SvcCas_Context_t *pstCasContext);
#if defined(CONFIG_MW_CAS) && defined(CONFIG_MW_CAS_NAGRA) && defined(CONFIG_MW_CAS_NAGRA_OP_SES)
HERROR svc_cas_PostProcessAfterScenarioSearchInstance(HUINT32 *pulCasInstId);
#endif
#if (CONFIG_DEBUG)
void svc_cas_ScenarioPrintPriorityTable(void);
#endif

#endif /* !__CAS_MGR_SCENARIO_H__ */
