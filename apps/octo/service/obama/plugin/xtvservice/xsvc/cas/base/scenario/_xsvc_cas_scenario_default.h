/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xsvc_cas_scenario_default.h						\n
	@brief	  	\n

	Description: cas module. \n
	Module: MW/CAS									 					\n

	Copyright (c) 2012 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __CASCENARIO_DEFAULT_H__
#define	__CASCENARIO_DEFAULT_H__

/********************************************************************
*   type definition
********************************************************************/

void xsvc_cas_DefScenarioMakePTFromGExistDevice (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
													SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
													HUINT32							 ulPriorityTableId,
													HUINT32							 ulActionId);

void xsvc_cas_DefScenarioMakePTFromGCamIdMatch (SvcCas_MgrPriorityTreeMgr_t			*pstTreeMgr,
												SvcCas_MgrPriorityTreeItem_t		*pstFirstItem,
												HUINT32								 ulPriorityTableId,
												HUINT32								 ulActionId);

void xsvc_cas_DefScenarioMakePTFromGCasSystemIdMatch (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
														SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
														HUINT32							 ulPriorityTableId,
														HUINT32							 ulActionId);

void xsvc_cas_DefScenarioMakePTFromGKindOfCas (SvcCas_MgrPriorityTreeMgr_t			*pstTreeMgr,
												SvcCas_MgrPriorityTreeItem_t		*pstFirstItem,
												HUINT32								 ulPriorityTableId,
												HUINT32								 ulActionId);

void xsvc_cas_DefScenarioMakePTFromGResource (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
												SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
												HUINT32							 ulPriorityTableId,
												HUINT32							 ulActionId);

void xsvc_cas_DefScenarioMakePTFromGKindOfDevice (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
													SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
													HUINT32							 ulPriorityTableId,
													HUINT32							 ulActionId);

void xsvc_cas_DefScenarioMakePTFromGSlot (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
											SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
											HUINT32							 ulPriorityTableId,
											HUINT32							 ulActionId);

void xsvc_cas_DefScenarioMakePTFromGSlotByDB (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
												SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
												HUINT32							 ulPriorityTableId,
												HUINT32							 ulReqActionId);

void xsvc_cas_DefScenarioMakePTFromGKindOfAction (SvcCas_MgrPriorityTreeMgr_t		*pstTreeMgr,
													SvcCas_MgrPriorityTreeItem_t	*pstFirstItem,
													HUINT32							 ulPriorityTableId,
													HUINT32							 ulReqActionId);
#endif

