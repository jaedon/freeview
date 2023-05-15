/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.
	Description: File에 대한 설명을 적습니다.			\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 									\n
	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <stdlib.h>
#include <octo_common.h>

#include "linkedlist.h"
#include <pal_sef.h>

#include <db_param.h>
#include <db_svc.h>
#include "isosvc.h"

#include <svc_epg.h>
#include "../local_include/_svc_epg_common.h"
#include "../local_include/_svc_epg_common_lib.h"
#include "../local_include/_svc_epg_db_raw_lib.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/






/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/******************************************************************
  * servivce list for valid check
  *****************************************************************/
STATIC SvcEpg_CheckSvcListRaw_t		s_stCheckRawServiceList = {0, };


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define ____LOCAL_FUNC____
STATIC HERROR svc_epgservice_GetSem(void)
{
	if(s_stCheckRawServiceList.ulSemId == 0)
	{
		if (VK_SEM_Create((unsigned long*)&s_stCheckRawServiceList.ulSemId, "epgdb_checksvclist", VK_SUSPENDTYPE_PRIORITY) != VK_OK)
		{
			return ERR_FAIL;
		}
	}

	if(VK_SEM_Get(s_stCheckRawServiceList.ulSemId) != VK_OK)
		return ERR_FAIL;

	return ERR_OK;
}

STATIC HERROR svc_epgservice_ReleaseSem(void)
{
	if(VK_SEM_Release(s_stCheckRawServiceList.ulSemId) != VK_OK)
		return ERR_FAIL;
	return ERR_OK;
}


STATIC HINT32	svc_epgservice_CompareService (const SvcEpg_SvcCheckInfo_t *lhs, const SvcEpg_SvcCheckInfo_t *rhs)
{
	HINT32 diff = lhs->deliType - rhs->deliType;
	if(diff == 0)
	{
		diff = lhs->antuid - rhs->antuid;
		if(diff == 0)
		{
			diff = lhs->tsuid - rhs->tsuid;
			if(diff == 0)
			{
				diff = lhs->tsid - rhs->tsid;
				if(diff == 0)
				{
					diff = lhs->onid - rhs->onid;
					if(diff == 0)
					{
						diff = lhs->tsid - rhs->tsid;
						if(diff == 0)
						{
							diff = lhs->svcid - rhs->svcid;
							if(diff == 0)
								diff = lhs->svcuid - rhs->svcuid;
						}
					}
				}
			}
		}
	}
	return diff;
}

STATIC HERROR			svc_epgservice_CreateCheckServiceList(void)
{
	HERROR						hError = ERR_EPG_FAIL;

	if(svc_epgservice_GetSem() == ERR_FAIL)
		return ERR_FAIL;

	if(s_stCheckRawServiceList.pstServices != NULL)
	{
		void 					*iter = NULL;
		SvcEpg_SvcCheckInfo_t	*pstEpgSvcInfo = NULL;				
		HxVector_t				*pstDeleteSvcList = NULL;
		
		for (iter = HLIB_TREE_Iterator(s_stCheckRawServiceList.pstServices) ; iter ; iter = HLIB_TREE_NextIterator(iter))
		{
			pstEpgSvcInfo = (SvcEpg_SvcCheckInfo_t *)HLIB_TREE_IteratorKey(iter);
			if(pstEpgSvcInfo)
			{
				Handle_t	hSvc = HANDLE_NULL;
				HBOOL bIsSameSvc = FALSE;
				if (DB_SVC_GetServiceHandleBySvcIdx(pstEpgSvcInfo->svcuid, &hSvc) == ERR_OK)
				{
					DbSvc_Info_t	stSvcInfo;
					HxSTD_memset(&stSvcInfo, 0, sizeof(DbSvc_Info_t));

					if (DB_SVC_GetServiceInfo(hSvc, &stSvcInfo) == ERR_OK)
					{
						// Don't delete following code, Inspite of existing the pstSvcInfo, it is not same svc.
						if(pstEpgSvcInfo->deliType == DbSvc_GetDeliType(stSvcInfo)
							&& pstEpgSvcInfo->antuid == DbSvc_GetAntIdx(&stSvcInfo)
							&& pstEpgSvcInfo->tsuid == DbSvc_GetTsIdx(&stSvcInfo)
							&& pstEpgSvcInfo->onid == DbSvc_GetOnId(&stSvcInfo)
							&& pstEpgSvcInfo->tsid == DbSvc_GetTsId(&stSvcInfo)
							&& pstEpgSvcInfo->svcid == DbSvc_GetSvcId(&stSvcInfo))
						{
							HxLOG_Print(" SAME ==================== Svc List <==>  svcuid[%d] name[%s]\n", pstEpgSvcInfo->svcuid, DbSvc_GetName(&stSvcInfo));							
							bIsSameSvc = TRUE;
						}
					}
				}

				if(bIsSameSvc == FALSE)
				{
					if(pstDeleteSvcList == NULL) pstDeleteSvcList = HLIB_VECTOR_New(NULL, NULL);
					HLIB_VECTOR_Add(pstDeleteSvcList, HLIB_MEM_MemDup(pstEpgSvcInfo,sizeof(SvcEpg_SvcCheckInfo_t)));
				}

				HxLOG_Print("==================== Svc List <==>  svcuid[%d]\n", pstEpgSvcInfo->svcuid);	
			}
		}

		if(pstDeleteSvcList)
		{
			HINT32 nDelCount = HLIB_VECTOR_Length(pstDeleteSvcList);
			if(nDelCount > 0)
			{
				HINT32 nEpgSvcCount = HLIB_TREE_Nnodes(s_stCheckRawServiceList.pstServices);
				HxLOG_Print("==================== Delete Count[%d] Epg Svc Count[%d]\n", nDelCount, nEpgSvcCount);	
				if(nDelCount == nEpgSvcCount)
				{
					DAPI_ResetTable(DxEPG_NAMEOF(eDxEPG_TYPE_PF));
					DAPI_ResetTable(DxEPG_NAMEOF(eDxEPG_TYPE_SC));					
					HLIB_TREE_RemoveAll(s_stCheckRawServiceList.pstServices);
				}
				else
				{
					HINT32 nCount = 0;
					for(nCount = 0; nCount < nDelCount ; nCount++)
					{
						HCHAR query[256]={0,};			
						pstEpgSvcInfo = HLIB_VECTOR_ItemAt(pstDeleteSvcList, nCount);
						HLIB_TREE_Remove(s_stCheckRawServiceList.pstServices , pstEpgSvcInfo);
						snprintf(query, 256, "DELETE FROM %s WHERE svcuid = %d",DxEPG_NAMEOF(eDxEPG_TYPE_PF), pstEpgSvcInfo->svcuid);
						DAPI_Query(query ,NULL, NULL);
						snprintf(query, 256, "DELETE FROM %s WHERE svcuid = %d",DxEPG_NAMEOF(eDxEPG_TYPE_SC), pstEpgSvcInfo->svcuid);
						DAPI_Query(query ,NULL, NULL);
						HxLOG_Print("==================== Delete Svc <==  svcuid[%d]\n", pstEpgSvcInfo->svcuid);	
					}
				}
			}
			HLIB_VECTOR_Delete(pstDeleteSvcList);
		}
	}	
	
	if(svc_epgservice_ReleaseSem() == ERR_FAIL)
		return ERR_FAIL;
	

	return hError;
}

STATIC HERROR			svc_epgservice_CheckServiceValidation(HUINT32 ulOrgDeliType, HUINT8 ucAntId, DbSvc_TripleId_t *pSvcTripleId)
{
	HERROR					hError = ERR_FAIL;
	DbSvc_Condition_t			stCondition = {0,};
	HINT32					nSvcCount = 0;
	DbSvc_Info_t				*pstSvcInfoList = NULL;
	SvcEpg_SvcCheckInfo_t	stEpgSvc = {0,};


	if(pSvcTripleId == NULL || svc_epgservice_GetSem() == ERR_FAIL)		return hError;

	if(s_stCheckRawServiceList.pstServices == NULL)
	{
		s_stCheckRawServiceList.pstServices = HLIB_TREE_New((HxCompareFunc)svc_epgservice_CompareService);
	}

	DB_SVC_InitSvcFindCondition(&stCondition);

	stCondition.nOrgDeliType = ulOrgDeliType;
	stCondition.nAntIdx 	= ucAntId;
	stCondition.nOnId		= pSvcTripleId->usOnId;
	stCondition.nTsId		= pSvcTripleId->usTsId;
	stCondition.nSvcId		= pSvcTripleId->usSvcId;
	
	if(DB_SVC_FindSvcInfoList (&stCondition, &nSvcCount, &pstSvcInfoList) == ERR_OK)
	{
		//HxLOG_Print("====================== Check SvcInfo D[%x] A[%x] O[%x] T[%x] S[%x] \n", stCondition.nOrgDeliType, stCondition.nAntIdx, stCondition.nOnId, stCondition.nTsId, stCondition.nSvcId);	
		if(nSvcCount != 0)
		{
			HINT32 uIndex = 0;
			for (uIndex = 0; uIndex < nSvcCount; uIndex++)
			{
				stEpgSvc.deliType	= DbSvc_GetDeliType(pstSvcInfoList[uIndex]);
				stEpgSvc.antuid	= DbSvc_GetAntIdx(&pstSvcInfoList[uIndex]);
				stEpgSvc.tsuid	= DbSvc_GetTsIdx(&pstSvcInfoList[uIndex]);
				stEpgSvc.onid		= DbSvc_GetOnId(&pstSvcInfoList[uIndex]);
				stEpgSvc.tsid		= DbSvc_GetTsId(&pstSvcInfoList[uIndex]);
				stEpgSvc.svcid	= DbSvc_GetSvcId(&pstSvcInfoList[uIndex]);
				stEpgSvc.svcuid	= DbSvc_GetSvcIdx(&pstSvcInfoList[uIndex]);


				if(s_stCheckRawServiceList.pstServices 
					&& HLIB_TREE_LookupExtended(s_stCheckRawServiceList.pstServices, &stEpgSvc, NULL, NULL) == FALSE)
				{
					//If There is No Same Service, Add the service into the tree
					HLIB_TREE_Insert(s_stCheckRawServiceList.pstServices,	 HLIB_MEM_MemDup(&stEpgSvc,sizeof(SvcEpg_SvcCheckInfo_t)), NULL);
					HxLOG_Print("====== Add Svc ==>  svcuid[%d] name[%s]\n", stEpgSvc.svcuid, DbSvc_GetName(&pstSvcInfoList[uIndex])?(HCHAR *)DbSvc_GetName(&pstSvcInfoList[uIndex]):"NULL");	
				}
				else
				{
					//HxLOG_Print("====================== No Svc ==>	svcuid[%d]\n", stEpgSvc.svcuid);	
				}
			}
			hError = ERR_OK;
		}
		else
		{
			HxLOG_Print("====================== No Svc In Obama DB\n");	
		}
	}

	if(pstSvcInfoList) DB_SVC_FreeServiceInfoList (NULL, NULL, pstSvcInfoList);

	if(svc_epgservice_ReleaseSem() == ERR_FAIL)
		return ERR_FAIL;
	
	return hError;
}

#if defined(CONFIG_DEBUG)
void svc_epgservice_Print(void)
{
	if(svc_epgservice_GetSem() == ERR_FAIL)
		return;

	if(s_stCheckRawServiceList.pstServices != NULL)
	{
		void 					*iter = NULL;
		SvcEpg_SvcCheckInfo_t	*pstEpgSvcInfo = NULL;				
		
		for (iter = HLIB_TREE_Iterator(s_stCheckRawServiceList.pstServices) ; iter ; iter = HLIB_TREE_NextIterator(iter))
		{
			pstEpgSvcInfo = (SvcEpg_SvcCheckInfo_t *)HLIB_TREE_IteratorKey(iter);
			if(pstEpgSvcInfo)
			{
				Handle_t	hSvc = HANDLE_NULL;
				if (DB_SVC_GetServiceHandleBySvcIdx(pstEpgSvcInfo->svcuid, &hSvc) == ERR_OK)
				{
					DbSvc_Info_t pstSvcInfo;
					memset(&pstSvcInfo, 0, sizeof(DbSvc_Info_t));

					if (DB_SVC_GetServiceInfo(hSvc, &pstSvcInfo) == ERR_OK)
					{
						HLIB_CMD_Printf("==================== Svc List <==>  svcuid[%d] name[%s]\n", pstEpgSvcInfo->svcuid, DbSvc_GetName(&pstSvcInfo));							
					}
				}
			}
		}
	}

	if(svc_epgservice_ReleaseSem() == ERR_FAIL)
		return;
}
#endif

#define ____GLOBAL_FUNC____


HERROR			SVC_EPGSERVICE_ValidSvcCreateCheckServiceList(void)
{
	return svc_epgservice_CreateCheckServiceList();
}

HERROR	SVC_EPGSERVICE_ValidSvcCheckServiceValidation(HUINT32 ulOrgDeliType, HUINT8 ucAntId, DbSvc_TripleId_t *pSvcTripleId)
{
	return svc_epgservice_CheckServiceValidation(ulOrgDeliType, ucAntId, pSvcTripleId);
}
void 	SVC_EPGSERVICE_Print(void)
{
#if defined(CONFIG_DEBUG)
	svc_epgservice_Print();
#endif
}


/*********************** End of File *******************************/
