/**
	@file     db_svc.c
	@brief     관련 API

	Description: db별, 모델 별 달라지는 default 값을 처리하는 API 모음 \n
	Module: DB/default \n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <db_svc.h>

#include <_db_svc_transponder.h>
#include <_db_svc_service.h>
#include <_db_svc.h>

#include "../local_include/_db_main.h"
#include "_db_svc_common.h"

#include <pdb_svc.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC HUINT32			s_ulSemId;

/*******************************************************************/
/********************      Macros          *************************/
/*******************************************************************/
#define	DB_SM_LOCK		VK_SEM_Get(s_ulSemId)
#define	DB_SM_UNLOCK	VK_SEM_Release(s_ulSemId)

#define	DB_SM_SYNC_RETURN(type,expr)		do{type ret; HxLOG_Trace(); DB_SM_LOCK; ret = expr; DB_SM_UNLOCK; return ret;}while(0)
#define	DB_SM_SYNC_VOID(expr)				do{HxLOG_Trace(); DB_SM_LOCK; expr; DB_SM_UNLOCK;}while(0)
#define	DB_SM_SYNC(expr)					DB_SM_SYNC_RETURN(HERROR, expr)

#define	__DO_NOT_USE_VOID()	\
	do {HxLOG_Warning("[%s:%d] DO NOT USE this function (%s).\n", __HxFILE__, __HxLINE__, __FUNCTION__);}while(0)

#define	__DO_NOT_USE(r)	\
	do {\
		HxLOG_Warning("[%s:%d] DO NOT USE this function (%s).\n", __HxFILE__, __HxLINE__, __FUNCTION__);\
		return r;\
	} while (0)

#define	__NOT_IMPLEMENTED_YET()	\
	do{HxLOG_Trace(); HxLOG_Info("[%s] not implemented yet...\n", __FUNCTION__);}while(0)


/*******************************************************************/
/********************	   Typedef		   *************************/
/*******************************************************************/

/*******************************************************************/
/********************	   Static Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/**************    local function prototypes	********************/
/*******************************************************************/

/*******************************************************************/
/********************	   Functions	   *************************/
/*******************************************************************/

HUINT32		DB_SVC_GetSmSem(void)
{
	return s_ulSemId;
}

HERROR		db_svc_Init(void)
{
	HERROR	hErr;

	//HLIB_LOG_SetLevel("meta", HxLOG_ALL);
	//HLIB_LOG_SetLevel("oxdbsvc", HxLOG_ALL);
	HxLOG_Trace();

	(void) VK_SEM_Create((unsigned long *)&s_ulSemId, "OxSM", VK_SUSPENDTYPE_PRIORITY);

	hErr  = db_svc_MgrInit(s_ulSemId);
	DB_SVC_ClearAllInfo();
	hErr |= pdb_svc_DamaAdaptorInitAntennaDB();
	hErr |= pdb_svc_DamaAdaptorInitServiceDB();
	hErr |= pdb_svc_DamaAdaptorInitTransponderDB();
	hErr |= pdb_svc_DamaAdaptorInitNetworkDB();
	hErr |= pdb_svc_DamaAdaptorInitGroupDB();
	hErr |= pdb_svc_DamaAdaptorInitProviderDB();
	hErr |= pdb_svc_DamaAdaptorInitBouquetDB();
	return hErr;
}

void		DB_SVC_Commit (void)
{
	DB_SM_SYNC_VOID(db_svc_MgrCommit(7
			, eSvcTable_Service
			, eSvcTable_Transponder
			, eSvcTable_Bouquet
			, eSvcTable_Group
			, eSvcTable_Network
			, eSvcTable_Provider
			, eSvcTable_Antenna
		));
}

// DAMA에서 service 변경사항을 알수 있는 Notifier, API 이름엔 DAMA라는 명칭은 뺐다.
void		DB_SVC_SetNotifier (HBOOL bSet)
{
	pdb_svc_DamaAdaptorRegisterNotifier(bSet);
}

HBOOL		DB_SVC_HasChanged (void)
{
	DB_SM_SYNC_RETURN(HBOOL, pdb_svc_HasChanged() );
}

HERROR		DB_SVC_GetChanged (DbSvc_Table_e	 	 	   eTable,
		 	 	 	 	 	 	 	 	 void	 	 	 **apvList,
		 	 	 	 	 	 	 	 	 HINT32	 	 	  *pnUpdateNum,
		 	 	 	 	 	 	 	 	 void	 	 	 **apvRemoveList,
		 	 	 	 	 	 	 	 	 HINT32	 	 	  *pnRemoveNum)
{
	DB_SM_SYNC(db_svc_MgrGetChanged(eTable, apvList, pnUpdateNum, apvRemoveList, pnRemoveNum));
}

HINT32		DB_SVC_GetUID (DbSvc_Table_e	  eTable,
		 	 	 	 	 	 const void	 	 *pvInfo)
{
	switch(eTable)
	{
	case eSvcTable_Service:
		return (HINT32)DbSvc_GetSvcIdx(((const DbSvc_Info_t *)pvInfo));
	case eSvcTable_Transponder:
		return (HINT32)(((const DbSvc_TsInfo_t *)pvInfo))->usTsIdx;
	case eSvcTable_Provider:
		return (HINT32)(((const DbSvc_PrvInfo_t *)pvInfo))->usPrvIdx;
	case eSvcTable_Network:
		return (HINT32)(((const DbSvc_NetInfo_t *)pvInfo))->usNetIdx;
	case eSvcTable_Group:
		return (HINT32)(((const DbSvc_GroupInfo_t *)pvInfo))->usGroupIdx;
	case eSvcTable_Bouquet:
		return (HINT32)(((const DbSvc_BouqInfo_t *)pvInfo))->usBouqIdx;
	case eSvcTable_Antenna:
	default:
		break;
	}
	return 0;
}

#if 0
HBOOL		DB_SVC_GetUserFlag1(DbSvc_Info_t	 	 	 *pstSvcInfo,
								DbSvc_UserFlag1_e	 	  eUserFlag1)
{
	if(pstSvcInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return FALSE;
	}

	return (pstSvcInfo->eUserFlag1 & eUserFlag1) ? TRUE : FALSE;
}

void		DB_SVC_SetUserFlag1(DbSvc_Info_t	 	 	 *pstSvcInfo,
		 	 	 	 	 	 	DbSvc_UserFlag1_e	 	  eUserFlag1,
		 	 	 	 	 	 	HBOOL	 	 	 	 	  bSetValue)
{
	if(pstSvcInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	if(bSetValue == TRUE)
	{
		pstSvcInfo->eUserFlag1 |= eUserFlag1;
	}
	else
	{
		pstSvcInfo->eUserFlag1 &= ~eUserFlag1;
	}
}
#endif

HUINT32		DB_SVC_GetUserFlag1(const DbSvc_Info_t *pstSvcInfo)
{
	if(pstSvcInfo == NULL)
	{
		HxLOG_Critical("\n\n");
		return 0;
	}

	return (HUINT32)pstSvcInfo->eUserFlag1;
}

HERROR		DB_SVC_Sync(ApiSyncMode_t eApiSync)
{
	HxLOG_Trace();
#if 0
	DB_SM_LOCK;

	if (db_svc_MgrHasChanged(FALSE, 1, eSvcTable_Service))
	{
		if (eApiSync == eSyncMode)
			DAPI_Sync(DxNAMEOF(DxService_t));
		else
			DAPI_LazySync(DxNAMEOF(DxService_t), 300);
	}
	if (db_svc_MgrHasChanged(FALSE, 1, eSvcTable_Transponder))
	{
		if (eApiSync == eSyncMode)
			DAPI_Sync(DxNAMEOF(DxTransponder_t));
		else
			DAPI_LazySync(DxNAMEOF(DxTransponder_t), 300);
	}
	if (db_svc_MgrHasChanged(FALSE, 1, eSvcTable_Provider))
	{
		if (eApiSync == eSyncMode)
			DAPI_Sync(DxNAMEOF(DxProvider_t));
		else
			DAPI_LazySync(DxNAMEOF(DxProvider_t), 300);
	}
	if (db_svc_MgrHasChanged(FALSE, 1, eSvcTable_Network))
	{
		if (eApiSync == eSyncMode)
			DAPI_Sync(DxNAMEOF(DxNetwork_t));
		else
			DAPI_LazySync(DxNAMEOF(DxNetwork_t), 300);
	}
	if (db_svc_MgrHasChanged(FALSE, 1, eSvcTable_Group))
	{
		if (eApiSync == eSyncMode)
			DAPI_Sync(DxNAMEOF(DxGroup_t));
		else
			DAPI_LazySync(DxNAMEOF(DxGroup_t), 300);
	}
	if (db_svc_MgrHasChanged(FALSE, 1, eSvcTable_Bouquet))
	{
		if (eApiSync == eSyncMode)
			DAPI_Sync(DxNAMEOF(DxBouquet_t));
		else
			DAPI_LazySync(DxNAMEOF(DxBouquet_t), 300);
	}
	DB_SM_UNLOCK;
#endif

	return ERR_OK;
}

void		DB_SVC_SyncHash (void)
{
	DB_SVC_Sync(eAsyncMode);
}

void		DB_SVC_Close(void)
{
	HxLOG_Trace();

	DB_SVC_ClearAllInfo();
}

HERROR		DB_SVC_LoadPreProgrammedChannel(HxCountry_e eCountryId)
{
	DB_SM_SYNC( db_svc_PrechLoad(eCountryId));
}

void		DB_SVC_ClearAllInfo(void)
{
	HxLOG_Trace();

	DB_SM_LOCK;

	db_svc_MgrClearAllInfo();

	DB_SM_UNLOCK;
}

HBOOL		DB_SVC_IsMemoryLoadingDone(void){__NOT_IMPLEMENTED_YET(); return TRUE;}

HERROR		DB_SVC_InitServiceInfo (DbSvc_Info_t *pstSvcInfo)
{
	HxLOG_Trace();

	if (pstSvcInfo == NULL)
		return ERR_FAIL;

	(void) db_svc_ServiceInitData(pstSvcInfo);
	return ERR_OK;
}

HERROR DB_SVC_ServiceSetUserFlag(DbSvc_Info_t *pstSvcInfo, const DbSvc_UserFlag1_e bwFlag)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	return pdb_svc_ServiceSetUserFlag(pstSvcInfo, bwFlag);
}

HERROR DB_SVC_ServiceGetUserFlag(DbSvc_UserFlag1_b *pbwFlag, const DbSvc_Info_t *pstSvcInfo)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	if (NULL == pbwFlag)
		return ERR_FAIL;

	return pdb_svc_ServiceGetUserFlag(pbwFlag, pstSvcInfo);
}

HERROR DB_SVC_ServiceCopyUserFlag(DbSvc_Info_t *pstDest, const DbSvc_Info_t *pstSource)
{
	if (NULL == pstDest)
		return ERR_FAIL;

	if (NULL == pstSource)
		return ERR_FAIL;

	return pdb_svc_ServiceCopyUserFlag(pstDest, pstSource);
}


HERROR		DB_SVC_AddServiceInfo(DbSvc_Info_t	 	 *pstSvcInfo,
		 	 	 	 	 	 	 	 Handle_t	 	 *phSvc)
{
	DB_SM_SYNC( db_svc_MgrAddServiceInfo(pstSvcInfo, phSvc) );
}

HERROR		DB_SVC_AddNetworkInfo(DbSvc_NetInfo_t	 	 *pstNetworkInfo,
		 	 	 	 	 	 	 	 	 HUINT16	 	 *pusNetIdx)
{
	DB_SM_SYNC( db_svc_MgrAddNetworkInfo(pstNetworkInfo, pusNetIdx) );
}

HERROR		DB_SVC_AddTsInfo(DbSvc_TsInfo_t	 	 *pstTsInfo,
		 	 	 	 	 	 	 	 HUINT16	 *pusTsIdx)
{
	DB_SM_SYNC( db_svc_MgrAddTsInfo(pstTsInfo, pusTsIdx) );
}

HERROR		DB_SVC_AddProviderInfo(DbSvc_PrvInfo_t	 	 *pstPrvInfo,
		 	 	 	 	 	 	 	 	 HUINT16	 	 *pusPrvIdx)
{
	DB_SM_SYNC( db_svc_MgrAddProviderInfo(pstPrvInfo, pusPrvIdx) );
}

/*
  NOTE) Put과 Add의 차이점
	 Put은 이미 Add를 통해 key가 존재하는 경우이다.
	 DAMA에서 service list 를 가져와서 OBAMA DB에 추가할때 사용한다.
*/
HUINT32		DB_SVC_PutServiceInfoList (const DbSvc_Info_t	 	 *pstList,
		 	 	 	 	 	 	 	 	 	 	 HUINT32	 	  nListNum)
{
	DB_SM_SYNC_RETURN(HUINT32, db_svc_MgrPutServiceInfoList(pstList, nListNum) );
}

HUINT32		DB_SVC_PutNetworkInfoList (const DbSvc_NetInfo_t	 	 *pstList,
		 	 	 	 	 	 	 	 	 	 	 	 HUINT32	 	  nListNum)
{
	DB_SM_SYNC_RETURN(HUINT32, db_svc_MgrPutNetworkInfoList(pstList, nListNum) );
}

HUINT32		DB_SVC_PutTsInfoList (const DbSvc_TsInfo_t	 	 	 *pstList,
		 	 	 	 	 	 	 	 	 	 	 HUINT32	 	  nListNum)
{
	DB_SM_SYNC_RETURN(HUINT32, db_svc_MgrPutTsInfoList(pstList, nListNum) );
}

HUINT32		DB_SVC_PutProviderInfoList (const DbSvc_PrvInfo_t	 	 *pstList,
		 	 	 	 	 	 	 	 	 	 	 	 HUINT32	 	  nListNum)
{
	DB_SM_SYNC_RETURN(HUINT32, db_svc_MgrPutProviderInfoList(pstList, nListNum) );
}

HUINT32		DB_SVC_PutGroupInfoList (const DbSvc_GroupInfo_t	 	 *pstList,
		 	 	 	 	 	 	 	 	 	 	 	 HUINT32	 	  nListNum)
{
#if defined(CONFIG_DB_SVC_SVCGROUP)
	DB_SM_SYNC_RETURN(HUINT32, db_svc_MgrPutGroupInfoList(pstList, nListNum) );
#else
	return 0;
#endif
}

HUINT32		DB_SVC_PutBouquetInfoList (const DbSvc_BouqInfo_t	 	 *pstList,
		 	 	 	 	 	 	 	 	 	 	 	 HUINT32	 	  nListNum)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	DB_SM_SYNC_RETURN(HUINT32, db_svc_MgrPutBouquetInfoList(pstList, nListNum) );
#else
	return 0;
#endif
}

HERROR		DB_SVC_DeleteServiceInfo(Handle_t hSvc)
{
	DB_SM_SYNC( db_svc_MgrDeleteServiceInfo(hSvc) );
}

HERROR		DB_SVC_DeleteAllServiceInfo(void) // by moon
{
	DB_SM_SYNC( db_svc_MgrDeleteAllServiceInfo() );
}

HERROR		DB_SVC_DeleteAllNetworkInfo(void) // by Moon
{
	DB_SM_SYNC( db_svc_MgrDeleteAllNetworkInfo() );
}

HERROR		DB_SVC_DeleteTsInfo(HUINT16 usTsIdx)
{
	DB_SM_SYNC( db_svc_MgrDeleteTsInfo(usTsIdx) );
}

HERROR		DB_SVC_DeleteAllTsInfo(void) // by Moon
{
	DB_SM_SYNC( db_svc_MgrDeleteAllTsInfo() );
}

HERROR		DB_SVC_DeleteProviderInfo(HUINT16 usPrvIdx)
{
	DB_SM_SYNC( db_svc_MgrDeleteProviderInfo(usPrvIdx) );
}

HERROR		DB_SVC_DeleteAllProviderInfo(void) // by Moon
{
	DB_SM_SYNC( db_svc_MgrDeleteAllProviderInfo() );
}

void       DB_SVC_DeleteServiceList(DbSvc_Condition_t *pstCond)
{
	DB_SM_SYNC_VOID( db_svc_MgrDeleteServiceList(pstCond) );
}

void       DB_SVC_DeleteAllSearchingService(void)
{
	DB_SM_SYNC_VOID( db_svc_MgrDeleteAllSearchingService() );
}

void       DB_SVC_RemoveSearchFlag(void)
{
	DB_SM_SYNC_VOID( db_svc_MgrRemoveSearchFlag() );
}

void       DB_SVC_SetTemporaryAliveFlag(DxSvcSection_e eSection, DxSvcType_e eSvcType){__DO_NOT_USE_VOID();}
void       DB_SVC_DeleteTemporaryAliveService(void){__DO_NOT_USE_VOID();}

HERROR     DB_SVC_DeleteAllNetworkService(DxDeliveryType_e eOrgDeliType)
{
	DB_SM_SYNC( db_svc_MgrDeleteAllNetworkService(eOrgDeliType) );
}

HERROR     DB_SVC_UpdateServiceInfo(Handle_t	 	  hSvc,
		 	 	 	 	 	 	 DbSvc_Info_t	 	 *pstSvcInfo)
{
	DB_SM_SYNC( db_svc_MgrUpdateServiceInfo(hSvc, pstSvcInfo) );
}

HERROR     DB_SVC_UpdateNetworkInfo(HUINT16	 	 	  usNetIdx,
		 	 	 	 	 	 	 DbSvc_NetInfo_t	 *pstNetworkInfo)
{
	DB_SM_SYNC( db_svc_MgrUpdateNetworkInfo(usNetIdx, pstNetworkInfo) );
}

HERROR     DB_SVC_UpdateTsInfo(HUINT16	 	 	 	  usTsIdx,
		 	 	 	 	 	 	 DbSvc_TsInfo_t	 	 *pstTsInfo)
{
	DB_SM_SYNC( db_svc_MgrUpdateTsInfo(usTsIdx, pstTsInfo) );
}

HERROR     DB_SVC_UpdateProviderInfo(HUINT16	 	 	 	  usPrvIdx,
		 	 	 	 	 	 	 DbSvc_PrvInfo_t	 	 	 *pstPrvInfo)
{
	DB_SM_SYNC( db_svc_MgrUpdateProviderInfo(usPrvIdx, pstPrvInfo) );
}

HERROR     DB_SVC_UpdateServiceInfoEx(Handle_t	 	 	 	  hSvc,
		 	 	 	 	 	 	 	 DbSvc_FieldList_e	 	  eSvcField,
		 	 	 	 	 	 	 	 DbSvc_FieldListExt_e	  eSvcFieldExt,
		 	 	 	 	 	 	 	 DbSvc_Info_t	 	 	 *pstSvcInfo)
{
	DB_SM_SYNC( db_svc_MgrUpdateServiceInfoEx(hSvc, eSvcField, eSvcFieldExt, pstSvcInfo) );
}

HERROR     DB_SVC_GetServiceInfo(Handle_t	 	 	  hSvc,
		 	 	 	 	 	 	 DbSvc_Info_t	 	 *pstSvcInfo)
{
	DB_SM_SYNC( db_svc_MgrGetServiceInfo(hSvc, pstSvcInfo) );
}

HERROR     DB_SVC_GetSearchResultInfoLowFreq(HUINT16 usOnId,HUINT32 *pulFreq,
			HUINT32 *pulSnr,HUINT32 *pulBer, HINT32 *pnStrength ) {__DO_NOT_USE(ERR_FAIL);}


HERROR     DB_SVC_GetTsInfo(HUINT16	 	 	  usTsIdx,
		 	 	 	 	 DbSvc_TsInfo_t	 	 *pstTsInfo)
{
	DB_SM_SYNC( db_svc_MgrGetTsInfo(usTsIdx, pstTsInfo) );
}

HERROR     DB_SVC_GetTsInfoByUniqueId(HUINT16	 	 	  usUniqueId,
		 	 	 	 	 	 	 	 DbSvc_TsInfo_t	 	 *pstTsInfo)
{
	return DB_SVC_GetTsInfo(usUniqueId, pstTsInfo);
}

HERROR     DB_SVC_GetTsInfoByServiceHandle(Handle_t	 	  hSvc,
		 	 	 	 	 	 	 	 	 DbSvc_TsInfo_t	 *pstTsInfo)
{
	DbSvc_Info_t	stInfo;

	HxLOG_Trace();

	if (DB_SVC_GetServiceInfo(hSvc, &stInfo) == ERR_OK)
	{
		return DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&stInfo), pstTsInfo);
	}
	return ERR_FAIL;
}

void       DB_SVC_InitTsFindCondition(DbSvc_TsCondition_t *pstCond)
{
	HxLOG_Trace();

	(void) db_svc_TransponderInitFilter(NULL, pstCond);
}


HERROR     DB_SVC_FindTsInfoList(DbSvc_TsCondition_t	  *pstCond,
		 	 	 	 	 	 	 	 HINT32	 	 	 	  *pnCnt,
		 	 	 	 	 	 	 	 DbSvc_TsInfo_t	 	 **ppstInfoList)
{
	DB_SM_SYNC( db_svc_MgrFindTsInfoList(pstCond, pnCnt, ppstInfoList) );
}

void       DB_SVC_FreeTsInfoList(DbSvc_TsInfo_t *pstInfoList)
{
	if (pstInfoList)
	{
		OxDB_Free(pstInfoList);
//		pstInfoList = NULL;
	}
}

HINT32		DB_SVC_GetDeliTypeIndex(DxDeliveryType_e eDeliType)
{
	switch(eDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:			return 0;
		case eDxDELIVERY_TYPE_CAB:			return 1;
		case eDxDELIVERY_TYPE_TER:			return 2;
		default:							break;
	}
	return -1;
}


HERROR     DB_SVC_GetNetworkInfo(HUINT16	 	 	  usNetIdx,
		 	 	 	 	 	 	 DbSvc_NetInfo_t	 *pstNetworkInfo)
{
	DB_SM_SYNC( db_svc_MgrGetNetworkInfo(usNetIdx, pstNetworkInfo) );
}

HERROR     DB_SVC_GetNetworkInfoByServiceHandle(Handle_t	 	 	  hSvc,
		 	 	 	 	 	 	 	 	 	 	 DbSvc_NetInfo_t	 *pstNetworkInfo)
{
	DbSvc_TsInfo_t	stInfo;

	if (DB_SVC_GetTsInfoByServiceHandle(hSvc, &stInfo) == ERR_OK)
	{
		return DB_SVC_GetNetworkInfo(stInfo.usNetIdx, pstNetworkInfo);
	}
	return ERR_FAIL;
}

HERROR     DB_SVC_GetPrvInfo(HUINT16	 	 	  usPrvIdx,
		 	 	 	 	 	 DbSvc_PrvInfo_t	 *pstPrvInfo)
{
	DB_SM_SYNC( db_svc_MgrGetPrvInfo(usPrvIdx, pstPrvInfo) );
}

HERROR     DB_SVC_GetPrvInfoByServiceHandle(Handle_t	 	  hSvc,
		 	 	 	 	 	 	 	 	 DbSvc_PrvInfo_t	 *pstPrvInfo)
{
	DbSvc_Info_t	stInfo;

	if (DB_SVC_GetServiceInfo(hSvc, &stInfo) == ERR_OK)
	{
		return DB_SVC_GetPrvInfo(DbSvc_GetPrvIdx(&stInfo), pstPrvInfo);
	}
	return ERR_FAIL;
}

HERROR     DB_SVC_GetServiceCount(
								DbSvc_Group_e 		 eSvcGroup, 	/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 		*pstAttr, 	/**< [in]  filtering 조건 2 - 서비스 속성 */
								HINT32 				*pnCnt)
{
	DB_SM_SYNC( db_svc_MgrGetServiceCount(eSvcGroup, pstAttr, pnCnt) );
}

HERROR     DB_SVC_GetServiceNumber(Handle_t	 	 hSvc,
		 	 	 	 	 	 	 	 HINT32	 	 *pnChNum)
{
	DbSvc_Info_t	stInfo;

	if (DB_SVC_GetServiceInfo(hSvc, &stInfo) == ERR_OK && pnChNum)
	{
		*pnChNum = (HINT32)DbSvc_GetLcn(&stInfo);
		return ERR_OK;
	}
	return ERR_FAIL;
}


HERROR	   DB_SVC_GetServiceHandleBySvcIdx(HUINT16	 	  usSvcIdx,
		 	 	 	 	 	 	 	 	 	 Handle_t	 *phSvc)
{
	DB_SM_SYNC( db_svc_MgrGetServiceHandleBySvcIdx(usSvcIdx, phSvc) );
}

HERROR     DB_SVC_GetServiceHandleByNumber(DxDeliveryType_e	 	  eOrgDeliType,
		 	 	 	 						DxSvcType_e	 	  eSvcType,
		 	 	 	 						HINT32	 	 	 	  nChNum,
		 	 	 	 						Handle_t	 	 	 *phSvc)
{
	DB_SM_SYNC( db_svc_MgrGetServiceHandleByNumber(eOrgDeliType, eSvcType, nChNum, phSvc) );
}

HERROR     DB_SVC_GetServiceIdFromSvcHandle(Handle_t	 	  hSvc,
		 	 	 	 	 	 	 	 	 	 HUINT16	 	 *pusSvcId)
{
	DbSvc_Info_t	stInfo;

	if (DB_SVC_GetServiceInfo(hSvc, &stInfo) == ERR_OK && pusSvcId)
	{
		*pusSvcId = (HINT32)DbSvc_GetSvcId(&stInfo);
		return ERR_OK;
	}
	return ERR_FAIL;
}

HERROR     DB_SVC_GetServiceHandleByOnIdAndSvcId(DxSvcSection_e	  eSvcSection,
		 	 	 	 	 	 	 	 	 	 	 HUINT16	 	 	  usOnId,
		 	 	 	 	 	 	 	 	 	 	 HUINT16	 	 	  usSvcId,
		 	 	 	 	 	 	 	 	 	 	 Handle_t	 	 	 *phSvc)
{
	DB_SM_SYNC( db_svc_MgrGetServiceHandleByOnIdAndSvcId(eSvcSection, usOnId, usSvcId, phSvc) );
}

HERROR     DB_SVC_GetUniqueIdFromSvcHandle(Handle_t	 	  hSvc,
		 	 	 	 	 	 	 	 	 	 HUINT16	 *pusUniqueId)
{
	DB_SM_SYNC( db_svc_MgrGetUniqueIdFromSvcHandle(hSvc, pusUniqueId) );
}

HERROR     DB_SVC_GetServiceHandle(DxSvcSection_e	  eSvcSection,
		 	 	 	 	 	 	 	 HUINT16	 	  usTsIdx,
		 	 	 	 	 	 	 	 HUINT16	 	  usSvcId,
		 	 	 	 	 	 	 	 Handle_t	 	 *phSvc)
{
	DB_SM_SYNC( db_svc_MgrGetServiceHandle(eSvcSection, usTsIdx, usSvcId, phSvc) );
}


HERROR     DB_SVC_FindServiceListByNumber(DxDeliveryType_e 		  eOrgDeliType,	/**< [in]  filtering 조건 1 - delivery type: original delivery type, all 포함 sat/ter/.. 등 OR 가능 */
										DxSvcType_e 			  eSvcType, 	/**< [in]  filtering 조건 2 - service type */
										DbSvc_VisibleType_e 	  eVisibleType, /**< [in]  filtering 조건 3 - visible type */
										HINT32 					  nChNum, 		/**< [in]  filtering 조건 4 - 검색할 channel number */
										HUINT32 				 *pulSvcCount, 	/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
										Handle_t 				**pphSvcList)	/**< [out] service handle list를 담아올 포인터. 받드시 DB_SVC_FreeServiceList()로 free 해줘야함 */
{
	DB_SM_SYNC( db_svc_MgrFindServiceListByNumber(eOrgDeliType, eSvcType, eVisibleType, nChNum, pulSvcCount, pphSvcList) );
}

HERROR     DB_SVC_GetTsCount(DbSvc_TsCondition_t *pstCond, HINT32 *pnCnt)
{
	DB_SM_SYNC( db_svc_MgrGetTsCount(pstCond, pnCnt) );
}

HERROR     DB_SVC_GetNetworkCount(HINT32 *pnCnt)
{
	DB_SM_SYNC( db_svc_MgrGetNetworkCount(pnCnt) );
}

HERROR     DB_SVC_GetProviderCount(HINT32 *pnCnt)
{
	DB_SM_SYNC( db_svc_MgrGetProviderCount(pnCnt) );
}

HERROR DB_SVC_FindServiceHandleNearbyLcn(DxDeliveryType_e eOrgDeliType, HUINT16 usLcn, HUINT32 ulPrivateMode, Handle_t* phPrevSvc, Handle_t* phUpperSvc)
{__DO_NOT_USE(ERR_FAIL);}


HERROR     DB_SVC_FindServiceHandle(
								DbSvc_Group_e 			 eSvcGroup, 	/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 			*pstAttr, 		/**< [in]  filtering 조건 2 - 서비스 속성 */
								Handle_t 				 hOffsetSvc, 	/**< [in]  filtering 조건 3 - 기준되는 service handle. 만약 next를 찾고자하면 current service에 해당됨. */
								DbSvc_FindDirection_e	 eDirection, 	/**< [in]  filtering 조건 4 - 방향에 관한 조건 */
								Handle_t 				*phSvc)			/**< [out] 해당 조건을 만족하는 service handle을 복사할 포인터 */
{
	DB_SM_SYNC( db_svc_MgrFindServiceHandle(eSvcGroup, pstAttr, hOffsetSvc, eDirection, phSvc) );
}

HERROR     DB_SVC_FindServiceHandleByCondition (
											const DbSvc_Condition_t		*pstCond,
											Handle_t				 	*phSvc
										)
{
	DB_SM_SYNC( db_svc_MgrFindServiceHandleByCondition(pstCond, phSvc) );
}

HERROR     DB_SVC_FindServiceHandleByTripleId(
								DxDeliveryType_e 	 eOrgDeliType, 	/**< [in]  서치 시 할당된 orginal delivery type */
								DbSvc_TripleId_t 	 stTripleId, 	/**< [in]  triple id (onid, tsid, svcid) */
								Handle_t 			*phSvc)			/**< [out] 해당 조건을 만족하는 service handle을 복사할 포인터 */
{
	DB_SM_SYNC( db_svc_MgrFindServiceHandleByTripleId(eOrgDeliType, stTripleId, phSvc) );
}

HERROR     DB_SVC_FindSvcHandleListWhereSearchFlagTrue(HINT32 	 	  *pnCnt,
		 	 	 	 	 	 	 	 	 	 	 	 	 Handle_t	 **pphSvcList)
{
	DB_SM_SYNC( db_svc_MgrFindSvcHandleListWhereSearchFlagTrue(pnCnt, pphSvcList) );
}

HERROR     DB_SVC_FindServiceList(
								DbSvc_Group_e 			  eSvcGroup, 		/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 			 *pstAttr, 		/**< [in]  filtering 조건 2 - 서비스 속성 */
								HUINT32					  ulMaxFindNum, 	/**< [in]  filtering 조건 3 - 가져올 최대 서비스 개수, 0이면 LIMIT이 없다 */
								Handle_t				  hOffsetSvc, 	/**< [in]  filtering 조건 4 - (Find 시작) 기준되는 hSvc */
								DbSvc_FindDirection_e	  eDirection, 	/**< [in]  filtering 조건 5 - 방향에 관한 조건 */
								HUINT32 				 *pulCount, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 				**pphSvcList)	/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
{
	DB_SM_SYNC( db_svc_MgrFindServiceList(eSvcGroup, pstAttr, ulMaxFindNum, hOffsetSvc, eDirection, pulCount, pphSvcList) );
}

HERROR     DB_SVC_FindServiceListBySvcId(
								DxDeliveryType_e 	  eOrgDeliType, /**< [in]  filtering 조건 1 - original delivery type */
								HUINT16 			  usSvcId, 		/**< [in]  filtering 조건 2 - service id */
								HINT32 				 *pnSvcCnt, 	/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 			**pphSvcList)	/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
{
	DB_SM_SYNC( db_svc_MgrFindServiceListBySvcId(eOrgDeliType, usSvcId, pnSvcCnt, pphSvcList) );
}

HERROR     DB_SVC_FindServiceListByNationalSvcId(DxDeliveryType_e	  eOrgDeliType, 	/**< [in]  filtering 조건 1 - original delivery type */
													HUINT16			  usNationalSvcId, 	/**< [in]  filtering 조건 2 - national service id - Canal Ready Only */
													HINT32			 *pnSvcCnt, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
													Handle_t		**pphSvcList)		/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
{__DO_NOT_USE(ERR_FAIL);}

HERROR     DB_SVC_FindServiceListByTripleId(
								DxDeliveryType_e 	  eOrgDeliType,	/**< [in]  filtering 조건 1 - original delivery type */
								DbSvc_TripleId_t 	  stTripleId, 	/**< [in]  filtering 조건 2 - triple id */
								HINT32 				 *pnSvcCnt, 	/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 			**pphSvcList)	/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
{
	DB_SM_SYNC( db_svc_MgrFindServiceListByTripleId(eOrgDeliType, stTripleId, pnSvcCnt, pphSvcList) );
}

HERROR     DB_SVC_FindServiceListWithInfo(
								DbSvc_Group_e 		  eSvcGroup, 		/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 		 *pstAttr, 			/**< [in]  filtering 조건 2 - 서비스 속성 */
								HUINT32 			 *pulCount, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 			**pphSvcList,		/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
								DbSvc_Info_t		**ppstSvcInfoList)	/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
{
	DB_SM_SYNC( db_svc_MgrFindServiceListWithInfo(eSvcGroup, pstAttr, pulCount, pphSvcList, ppstSvcInfoList) );
}

void       DB_SVC_FreeServiceList(Handle_t *phSvcList)
{
	if (phSvcList)
		OxDB_Free(phSvcList);
}

void       DB_SVC_FreeServiceInfoList(HINT32	 	 	 *pnLcnList,
		 	 	 	 	 	 	 	 Handle_t	 	 	 *phSvcList,
		 	 	 	 	 	 	 	 DbSvc_Info_t	 	 *pstSvcInfoList)
{
	if (pnLcnList)
		OxDB_Free(pnLcnList);
	if (phSvcList)
		OxDB_Free(phSvcList);
	if (pstSvcInfoList)
		OxDB_Free(pstSvcInfoList);
}


void	   DB_SVC_InitSvcAttribute (DbSvc_Attr_t *pstAttr)
{
	if (pstAttr)
	{
		HxSTD_MemSet (pstAttr, 0, sizeof(DbSvc_Attr_t));

		pstAttr->nBouqIdx = -1;
		pstAttr->nGenreIdx = -1;
		pstAttr->ucAlphabetIdx = -1;
		pstAttr->eSvcOpType = eDxOPERATOR_All;
		pstAttr->eNotSvcOpType = eDxOPERATOR_All;
		pstAttr->eUserMask1 = eSvcUserFlag_All;
	}
}

void       DB_SVC_InitSvcFindCondition(DbSvc_Condition_t *pstCond)
{
	if (pstCond)
	{
		pstCond->nAntIdx = -1;
		pstCond->nOnId   = -1;
		pstCond->nTsId   = -1;
		pstCond->nSvcId  = -1;
		pstCond->nTsIdx  = -1;
		pstCond->nPrvIdx = -1;
		pstCond->nLcnFlag = -1;
		pstCond->nStartLcn = -1;
		pstCond->nEndLcn = -1;

		pstCond->nOrgDeliType	= (DxDeliveryType_e)eDxDELIVERY_TYPE_ALL;
		pstCond->nSvcType 	 	= (DxSvcType_e)eDxSVC_TYPE_All;
		pstCond->nCasType 	 	= (DxCasType_e)eDxCAS_TYPE_All;
		pstCond->nVideoType	 	= (DxVideoType_e)eDxVIDEO_TYPE_ALL;
		pstCond->nSection	  	= (DxSvcSection_e)eDxSvcSection_All;
		pstCond->eSvcOpType	 	= (DxOperator_e)eDxOPERATOR_All;
		pstCond->eSatType	 	= (SVC_SatType_t) eSatType_ALL;

		pstCond->eUserMask1 = eSvcUserFlag_All;
	}
}

HERROR     DB_SVC_FindSvcInfoList(DbSvc_Condition_t	 	  *pstCond,
		 	 	 	 	 	 	 	 HINT32	 	 	 	  *pnCnt,
		 	 	 	 	 	 	 	 DbSvc_Info_t	 	 **ppstInfoList)
{
	DB_SM_SYNC( db_svc_MgrFindSvcInfoList(pstCond, pnCnt, ppstInfoList) );
}

HINT32     DB_SVC_FindSvcUidList (const DbSvc_Condition_t	 *pstCond,
		 	 	 	 	 	 	 	 	 	 	 HINT32	 	 *pnUids,
		 	 	 	 	 	 	 	 	 	 	 HINT32	 	  n)
{
	DB_SM_SYNC_RETURN(HINT32, db_svc_MgrFindSvcUidList(pstCond, pnUids, n) );
}

HERROR     DB_SVC_FindSvcHandleList(DbSvc_Condition_t	  *pstCond,
									HINT32	 	 	 	  *pnCnt,
									Handle_t	 	 	 **pphSvcList)
{
	DB_SM_SYNC( db_svc_MgrFindSvcHandleList(pstCond, pnCnt, pphSvcList) );
}


HERROR     DB_SVC_FindTripleIdAndSvcTypeList(DxDeliveryType_e		   eOrgDeliType,
												HUINT8	 	 		   ucAntIdx,
												HINT32	 	 	 	  *pnCnt,
												DbSvc_TripleId_t	 **ppTripleIdList,
												DxSvcType_e	 	 **ppeSvcTypeList,
												Handle_t	 	 	 **pphSvcList
											)
{
	DB_SM_SYNC( db_svc_MgrFindTripleIdAndSvcTypeList(eOrgDeliType, ucAntIdx, pnCnt, ppTripleIdList, ppeSvcTypeList, pphSvcList) );
}

void       DB_SVC_FreeTripleIdAndSvcTypeList(DbSvc_TripleId_t	*pstTripleIdList,
												DxSvcType_e	*peSvcTypeList)
{
	if (pstTripleIdList)
		OxDB_Free(pstTripleIdList);
	if (peSvcTypeList)
		OxDB_Free(peSvcTypeList);
}



HERROR     DB_SVC_CheckValidServiceHandle(Handle_t hSvc)
{
	DB_SM_SYNC( db_svc_MgrCheckValidServiceHandle(hSvc) );
}


void     DB_SVC_ResetAudioUserFlag(void)
{
	DB_SM_SYNC_VOID( db_svc_MgrResetAudioUserFlag() );
}

void     DB_SVC_ResetSubtitleUserFlag(void)
{
	DB_SM_SYNC_VOID( db_svc_MgrResetSubtitleUserFlag() );
}

/**< memory db의 field 중 sync되지 않는 필드의 내용을 지운다 */
void     DB_SVC_ResetNoSyncFields(void){ __DO_NOT_USE_VOID();}

void	DB_SVC_ResetRatingPassedEventId(void)
{
	DB_SM_SYNC_VOID( db_svc_MgrResetRatingPassedEventId() );
}

void       DB_SVC_AddRecentService(Handle_t hSvc){ __DO_NOT_USE_VOID();}

HUINT16    DB_SVC_GetAllUniqueId(void)
{
	return DxMAX_TS_NUM;
}

HUINT16    DB_SVC_GetTempUniqueId(void)
{
	return DxMAX_TS_NUM + 1;
}

HUINT16    DB_SVC_GetPlaybackUniqueId(void)
{
	return DxMAX_TS_NUM + 2;
}

HERROR     DB_SVC_FindProviderIndexByName(HUINT8	*pszName,
										HUINT16		*pusPrvIdx)
{
	DB_SM_SYNC( db_svc_MgrFindProviderIndexByName(pszName, pusPrvIdx) );
}

HERROR     DB_SVC_FindProvideInfoList(HINT32			 *pnCnt,
									DbSvc_PrvInfo_t		**ppstPrvInfo)
{
	HxLOG_Trace();
	HxLOG_Assert(pnCnt && ppstPrvInfo);

	*ppstPrvInfo = DB_SVC_GetAllPrvInfo(pnCnt);
	if (*ppstPrvInfo)
		return ERR_OK;

	return ERR_FAIL;
}

void	DB_SVC_FreeProviderInfoList(DbSvc_PrvInfo_t *pstPrvInfo)
{
	if (pstPrvInfo)
		OxDB_Free(pstPrvInfo);
}



HERROR     DB_SVC_FindNetworkIndexById(DxDeliveryType_e		 eOrgDeliType,
										HUINT16				 usNetId,
										HUINT16				*pusNetworkIdx)
{
	DB_SM_SYNC( db_svc_MgrFindNetworkIndexById(eOrgDeliType, usNetId, pusNetworkIdx) );
}

HERROR     DB_SVC_FindNetworkIndexByName(HUINT16		 usNetId,
										HUINT8			*pszName,
										HUINT16			*pusNetworkIdx)
{
	DB_SM_SYNC( db_svc_MgrFindNetworkIndexByName(usNetId, pszName, pusNetworkIdx) );
}

HERROR     DB_SVC_FindNetworkInfoByDeliType(DxDeliveryType_e	  eOrgDeliType,
											HINT32				 *pnCnt,
											DbSvc_NetInfo_t		**ppstNetworkInfo)
{
	DB_SM_SYNC( db_svc_MgrFindNetworkInfoByDeliType(eOrgDeliType, pnCnt, ppstNetworkInfo) );
}

void		DB_SVC_FreeNetInfoList(DbSvc_NetInfo_t *pstNetInfoList)
{
	if (pstNetInfoList)
		OxDB_Free(pstNetInfoList);
}

HERROR	DB_SVC_GetDefaultAuthority(Handle_t		 hSvc,
									HUINT8		*pszDefAuth)
{
#if 1
	HERROR				hErr;
	DbSvc_Info_t		stSvcInfo;
	DbSvc_TsInfo_t		stTsInfo;
	DbSvc_NetInfo_t		stNetInfo;
	HBOOL				bFound = FALSE;
	HUINT32 			i, ulLen;
	HUINT8				*aucTemp;

	aucTemp = NULL;

	if(pszDefAuth == NULL)
		return ERR_FAIL;

	if(DB_SVC_GetServiceInfo(hSvc, &stSvcInfo) == ERR_OK)
	{
		hErr = DB_SVC_GetServiceDefaultAuthority((const DbSvc_Info_t *)&stSvcInfo, &aucTemp);
		if(ERR_OK == hErr && HxSTD_StrLen((char *)aucTemp) > 0)
		{
			HxSTD_StrNCpy((char *)pszDefAuth, (char *)aucTemp, EPG_TVA_CRID_AUTH_LEN);
			bFound = TRUE;
		}
		else if(DB_SVC_GetTsInfo(DbSvc_GetTsIdx(&stSvcInfo), &stTsInfo) == ERR_OK)
		{
			hErr = DB_SVC_GetTsDefaultAuthority((const DbSvc_TsInfo_t *)&stTsInfo, &aucTemp);
			if(ERR_OK == hErr && HxSTD_StrLen((char *)aucTemp) > 0)
			{
				HxSTD_StrNCpy((char *)pszDefAuth, (char *)aucTemp, EPG_TVA_CRID_AUTH_LEN);
				bFound = TRUE;
			}
			else if(DB_SVC_GetNetworkInfo(stTsInfo.usNetIdx, &stNetInfo) == ERR_OK)
			{
				hErr = DB_SVC_GetNetworkDefaultAuthority((const DbSvc_NetInfo_t *)&stNetInfo, &aucTemp);
				if(ERR_OK == hErr && HxSTD_StrLen((char *)aucTemp) > 0)
				{
					HxSTD_StrNCpy((char *)pszDefAuth, (char *)aucTemp, EPG_TVA_CRID_AUTH_LEN);
					bFound = TRUE;
				}
			}
		}

		if(bFound)
		{
			ulLen = HxSTD_StrLen(pszDefAuth);
			for(i = 0; i < ulLen; ++i) if(pszDefAuth[i] >= 'a' && pszDefAuth[i] <= 'z') pszDefAuth[i] = 'A' + pszDefAuth[i] - 'a';

			return ERR_OK;
		}
	}

#else

#endif

	return ERR_FAIL;
}

HERROR     DB_SVC_GetAllCasType(DxSvcType_e		 eSvcType,
								DxVideoType_e	 eVideoType,
								DxCasType_e		*pbwCasType)
{
	DB_SM_SYNC( db_svc_MgrGetAllCasType(eSvcType, eVideoType, pbwCasType) );
}



HERROR     DB_SVC_GetMaxLcnInLcnServices (DxDeliveryType_e	 eOrgDeliType,
											DxSvcType_e	 eSvcType,
											HINT32			*pnLcn)
{
	DB_SM_SYNC( db_svc_MgrGetMaxLcnInLcnServices(eOrgDeliType, eSvcType, pnLcn) );
}

HERROR     DB_SVC_GetMaxLcnInLcnInInterval (DxDeliveryType_e	 eOrgDeliType,
											DxSvcType_e	 eSvcType,
											HINT32 nStartNum,
											HINT32 nEndNum,
											HINT32			*pnLcn)
{
	DB_SM_SYNC( db_svc_MgrGetMaxLcnInInterval(eOrgDeliType, eSvcType, nStartNum, nEndNum, pnLcn) );
}


HERROR     DB_SVC_RenumberRankingToLcn(DxDeliveryType_e eOrgDeliType, DxSvcSection_e eSvcSection, DxSvcType_e eSvcType, HINT32 nStartChNum, HINT32 nEndChNum)
{__DO_NOT_USE(ERR_FAIL);}


HERROR     DB_SVC_RenumberSavingOrderToChNum(DxDeliveryType_e	 eOrgDeliType,
											DxSvcType_e		 eSvcType,
											HINT32				 nBaseStartChNum)
{
	DB_SM_SYNC( db_svc_MgrRenumberSavingOrderToChNum(eOrgDeliType, eSvcType, nBaseStartChNum) );
}

HERROR     DB_SVC_RenumberLcnOrderToChNum(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HINT32 nBaseChNum){__DO_NOT_USE(ERR_FAIL);}

HERROR 	DB_SVC_MakeNonLcnNumber(DxDeliveryType_e		eOrgDeliType,
								DxSvcType_e			eSvcType)
{
	HERROR	err;
	HINT32	nLastLcn;

	HxLOG_Trace();

	err = DB_SVC_GetMaxLcnInLcnServices(eOrgDeliType, eSvcType, &nLastLcn);
	if (err != ERR_OK)
	{
		HxLOG_Debug("[%s:%d] Cannot find last lcn number~!!\n", __HxFILE__, __HxLINE__);
		return err;
	}

	return DB_SVC_RenumberSavingOrderToChNum(eOrgDeliType, eSvcType, nLastLcn + 1);
}

HERROR DB_SVC_GetLcnOrderNumber(DxDeliveryType_e	 eOrgDeliType,
									DxSvcType_e	 eSvcType,
									HUINT16			*pusLcn)
{
	DB_SM_SYNC( db_svc_MgrGetLcnOrderNumber(eOrgDeliType, eSvcType, pusLcn) );
}

HERROR DB_SVC_GetLcnOrderNumberFromTVsAndRadios(DxDeliveryType_e	 eOrgDeliType,
												HUINT16				*pusLcn)
{

	return DB_SVC_GetLcnOrderNumber(eOrgDeliType, eDxSVC_TYPE_TV | eDxSVC_TYPE_RADIO, pusLcn);
}
HERROR     DB_SVC_ReplaceFavoriteServiceHandle(DbSvc_FavGroup_e eFavGroup, Handle_t hOrgFavSvc, Handle_t hNewFavSvc){__DO_NOT_USE(ERR_FAIL);}

HERROR     DB_SVC_AddBouquetInfo(DbSvc_BouqInfo_t		*pstBouquetInfo,
									HUINT16				*pusBouqIdx)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	DB_SM_SYNC( db_svc_MgrAddBouquetInfo(pstBouquetInfo, pusBouqIdx) );
#else
	return ERR_FAIL;
#endif
}
HERROR     DB_SVC_DeleteBouquetInfo(HUINT16 usBouqIdx)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	DB_SM_SYNC( db_svc_MgrDeleteBouquetInfo(usBouqIdx) );
#else
	return ERR_FAIL;
#endif
}

HERROR     DB_SVC_DeleteAllBouquetInfo(void)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	DB_SM_SYNC( db_svc_MgrDeleteAllBouquetInfo() );
#else
	return ERR_FAIL;
#endif
}

HERROR     DB_SVC_UpdateBouquetInfo(HUINT16			 usBouqIdx,
								DbSvc_BouqInfo_t	*pBouquetInfo)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	DB_SM_SYNC( db_svc_MgrUpdateBouquetInfo(usBouqIdx, pBouquetInfo) );
#else
	return ERR_FAIL;
#endif
}

HERROR     DB_SVC_GetBouquetCount(HINT32 *pnCnt)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	DB_SM_SYNC( db_svc_MgrGetBouquetCount(pnCnt) );
#else
	return ERR_FAIL;
#endif
}

HERROR     DB_SVC_GetBouquetInfo(HUINT16			 usBouqIdx,
								DbSvc_BouqInfo_t	*pBouquetInfo)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	DB_SM_SYNC( db_svc_MgrGetBouquetInfo(usBouqIdx, pBouquetInfo) );
#else
	return ERR_FAIL;
#endif
}

HERROR     DB_SVC_FindBouquetIndexById(HUINT16		 usBouqId,
										HUINT16		*pusBouqIdx)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	DB_SM_SYNC( db_svc_MgrFindBouquetIndexById(usBouqId, pusBouqIdx) );
#else
	return ERR_FAIL;
#endif
}

HERROR     DB_SVC_FindBouquetIndexByName(HUINT8		*pszName,
										HUINT16		*pusBouqIdx)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	DB_SM_SYNC( db_svc_MgrFindBouquetIndexByName((HCHAR *)pszName, pusBouqIdx) );
#else
	return ERR_FAIL;
#endif
}

HERROR     DB_SVC_FindBouquetIndexList(HINT32		 *pnBouqNum,
										HUINT16		**ppusBouqIdxList)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	DB_SM_SYNC( db_svc_MgrFindBouquetIndexList(pnBouqNum, ppusBouqIdxList) );
#else
	return ERR_FAIL;
#endif
}

void       DB_SVC_FreeBouquetIndexList(HUINT16 *pusBouqIdxList)
{
	if (pusBouqIdxList)
		OxDB_Free(pusBouqIdxList);
}

HERROR     DB_SVC_AddBouquetIndexToSvcInfo (DbSvc_Info_t	*pstSvcInfo,
												HUINT16		 usBqIdx)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	DB_SM_SYNC( db_svc_MgrAddBouquetIndexToSvcInfo(pstSvcInfo, usBqIdx) );
#else
	return ERR_FAIL;
#endif
}

HERROR     DB_SVC_CountBouquetInSvcInfo (DbSvc_Info_t		*pstSvcInfo,
											HUINT32			*pulBqNum)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	DB_SM_SYNC( db_svc_MgrCountBouquetInSvcInfo(pstSvcInfo, pulBqNum) );
#else
	return ERR_FAIL;
#endif
}

HERROR     DB_SVC_GetBouquetIndexInSvcInfo (DbSvc_Info_t	*pstSvcInfo,
											HUINT32			 ulIndex,
											HUINT16			*pusBqIdx)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	DB_SM_SYNC( db_svc_MgrGetBouquetIndexInSvcInfo(pstSvcInfo, ulIndex, pusBqIdx) );
#else
	return ERR_FAIL;
#endif
}

HERROR     DB_SVC_GetBouquetIndexArrayInSvcInfo (DbSvc_Info_t	*pstSvcInfo,
												HUINT32			 ulBqArrayMaxNum,
												HUINT32			*pulBqIdxNum,
												HUINT16			*pusBqIdxArray)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	return db_svc_MgrGetBouquetIndexArrayInSvcInfo(pstSvcInfo, ulBqArrayMaxNum, pulBqIdxNum, pusBqIdxArray);
#else
	return ERR_FAIL;
#endif
}

HERROR     DB_SVC_ClearBouquetInfoInSvcInfo (DbSvc_Info_t *pstSvcInfo)
{
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
	DB_SM_SYNC( db_svc_MgrClearBouquetInfoInSvcInfo(pstSvcInfo) );
#else
	return ERR_FAIL;
#endif
}


HERROR		DB_SVC_AddGroupInfo (DbSvc_GroupInfo_t		*pstGroupInfo,
									HUINT16				*pusGroupIdx)
{
	DB_SM_SYNC( db_svc_MgrAddGroupInfo(pstGroupInfo, pusGroupIdx) );
}

HERROR		DB_SVC_DeleteGroupInfo (HUINT16 usGroupIdx)
{
	DB_SM_SYNC( db_svc_MgrDeleteGroupInfo(usGroupIdx) );
}

HERROR		DB_SVC_DeleteAllGroupInfo(void)
{
	DB_SM_SYNC( db_svc_MgrDeleteAllGroupInfo() );
}

HERROR		DB_SVC_UpdateGroupInfo (HUINT16			 usGroupIdx,
								DbSvc_GroupInfo_t	*pstGroupInfo)
{
	DB_SM_SYNC( db_svc_MgrUpdateGroupInfo(usGroupIdx, pstGroupInfo) );
}

HERROR		DB_SVC_CountGroup (HINT32 *pnCnt)
{
	DB_SM_SYNC( db_svc_MgrCountGroup(pnCnt) );
}

HERROR		DB_SVC_GetGroupInfo (HUINT16		 usGroupIdx,
							DbSvc_GroupInfo_t	*pstGroupInfo)
{
	DB_SM_SYNC( db_svc_MgrGetGroupInfo(usGroupIdx, pstGroupInfo) );
}

HERROR		DB_SVC_GetGroupInfoByGroupId (HUINT32		 ulGroupId,
									DbSvc_GroupInfo_t	*pstGroupInfo)
{
	DB_SM_SYNC( db_svc_MgrGetGroupInfoByGroupId(ulGroupId, pstGroupInfo) );
}

HERROR		DB_SVC_FindGroupIndexByGroupId (HUINT32		 ulGroupId,
											HUINT16		*pusGroupIdx)
{
	HERROR				hErr;
	DbSvc_GroupInfo_t	stInfo;

	hErr = DB_SVC_GetGroupInfoByGroupId(ulGroupId, &stInfo);
	if (hErr == ERR_OK && pusGroupIdx)
	{
		*pusGroupIdx = stInfo.usGroupIdx;
	}
	return ERR_FAIL;
}

HERROR		DB_SVC_FindGroupIndexByName (HUINT8		*pszName,
										HUINT16		*pusGroupIdx)
{
	DB_SM_SYNC( db_svc_MgrFindGroupIndexByName(pszName, pusGroupIdx) );
}

HERROR		DB_SVC_GetAllGroupIndexList (HINT32		 *pnGroupNum,
										HUINT16		**ppusGroupIdxList)
{
	DB_SM_SYNC( db_svc_MgrGetAllGroupIndexList(pnGroupNum, ppusGroupIdxList) );
}

HERROR     	DB_SVC_GetAllGroupInfoList (HINT32			 *pnGroupNum,
									DbSvc_GroupInfo_t	**pstGroupInfoList)
{
	DB_SM_SYNC( db_svc_MgrGetAllGroupInfoList(pnGroupNum, pstGroupInfoList) );
}

void		DB_SVC_FreeGroupIndexList (HUINT16 *pusGroupIdxList)
{
	if (pusGroupIdxList) OxDB_Free(pusGroupIdxList);
}

void 		DB_SVC_FreeGroupInfoList (DbSvc_GroupInfo_t *pstGroupInfoList)
{
	if (pstGroupInfoList) OxDB_Free(pstGroupInfoList);
}

HERROR		DB_SVC_AddGroupIndexToSvcInfo (DbSvc_Info_t		*pstSvcInfo,
										HUINT16				 usGroupIdx)
{
#if defined(CONFIG_DB_SVC_SVCGROUP)
	DB_SM_SYNC( db_svc_MgrAddGroupIndexToSvcInfo(pstSvcInfo, usGroupIdx) );
#else
	return ERR_FAIL;
#endif
}

HERROR		DB_SVC_CountGroupInSvcInfo (DbSvc_Info_t	*pstSvcInfo,
										HUINT32			*pulGroupNum)
{
	return DB_SVC_GetGroupIndexArrayInSvcInfo(pstSvcInfo, 0, pulGroupNum, NULL);
}

HERROR		DB_SVC_GetGroupIndexInSvcInfo (DbSvc_Info_t		*pstSvcInfo,
										HUINT32				 ulIndex,
										HUINT16				*pusGroupIdx)
{
#if defined(CONFIG_DB_SVC_SVCGROUP)
	DB_SM_SYNC( db_svc_MgrGetGroupIndexInSvcInfo(pstSvcInfo, ulIndex, pusGroupIdx) );
#else
	return ERR_FAIL;
#endif
}

HERROR		DB_SVC_GetGroupIndexArrayInSvcInfo (DbSvc_Info_t	*pstSvcInfo,
											HUINT32				 ulGroupArrayMaxNum,
											HUINT32				*pulGroupIdxNum,
											HUINT16				*pusGroupIdxArray)
{
#if defined(CONFIG_DB_SVC_SVCGROUP)
	return db_svc_MgrGetGroupIndexArrayInSvcInfo(pstSvcInfo, ulGroupArrayMaxNum, pulGroupIdxNum, pusGroupIdxArray);
#else
	return ERR_FAIL;
#endif
}

HERROR		DB_SVC_DeleteGroupIndexInSvcInfo (DbSvc_Info_t	*pstSvcInfo,
											HUINT16			 usGroupIdx)
{
#if defined(CONFIG_DB_SVC_SVCGROUP)
	DB_SM_SYNC( db_svc_MgrDeleteGroupIndexInSvcInfo(pstSvcInfo, usGroupIdx) );
#else
	return ERR_FAIL;
#endif
}

HERROR		DB_SVC_ClearGroupInSvcInfo (DbSvc_Info_t *pstSvcInfo)
{
#if defined(CONFIG_DB_SVC_SVCGROUP)
	HUINT32			 ulIndex;

	HxLOG_Trace();
	HxLOG_Assert(pstSvcInfo);

	for (ulIndex = 0; ulIndex < DxGROUP_LEN; ulIndex++)
	{
		DbSvc_SetGroupId(pstSvcInfo, ulIndex, 0);
	}
#endif
	return ERR_OK;
}


DbSvc_TsInfo_t *	DB_SVC_GetAllTsInfo (HINT32 *pnCount)
{
	DB_SM_SYNC_RETURN( DbSvc_TsInfo_t *, db_svc_MgrGetAllTsInfo(pnCount) );
}

DbSvc_NetInfo_t *	DB_SVC_GetAllNetInfo (HINT32 *pnCount)
{
	DB_SM_SYNC_RETURN( DbSvc_NetInfo_t *, db_svc_MgrGetAllNetInfo(pnCount) );
}

DbSvc_PrvInfo_t *	DB_SVC_GetAllPrvInfo (HINT32 *pnCount)
{
	DB_SM_SYNC_RETURN( DbSvc_PrvInfo_t *, db_svc_MgrGetAllPrvInfo(pnCount) );
}

DbSvc_Info_t *	DB_SVC_GetAllSvcInfo (HINT32 *pnCount)
{
	DB_SM_SYNC_RETURN( DbSvc_Info_t *, db_svc_MgrGetAllSvcInfo(pnCount) );
}

DbSvc_BouqInfo_t *	DB_SVC_GetAllBouqInfo (HINT32 *pnCount)
{
	DB_SM_SYNC_RETURN( DbSvc_BouqInfo_t *, db_svc_MgrGetAllBouqInfo(pnCount) );
}


HERROR	DB_SVC_PutAntInfo (HUINT16			 usAntIdx,
					const DbSvc_AntInfo_t	*pstAntInfo)
{
	DB_SM_SYNC( db_svc_MgrPutAntInfo(usAntIdx, pstAntInfo) );
}


HERROR DB_SVC_AddAntInfo(
					DbSvc_AntInfo_t 	*pstAntInfo, 	/**< 추가할 antenna info */
					HUINT16 			*pusAntIdx		/**< db에 추가 후 자동 설정된 index */
					)
{
	DB_SM_SYNC( db_svc_MgrAddAntInfo(pstAntInfo, pusAntIdx) );
}

HERROR DB_SVC_DeleteAntInfo(
					HUINT16 	usAntIdx		/**< 지우고자 하는 antenna index */
					)
{
	DB_SM_SYNC( db_svc_MgrDeleteAntInfo(usAntIdx) );
}

HERROR DB_SVC_DeleteAllAntInfo(void)
{
	DB_SM_SYNC( db_svc_MgrDeleteAllAntInfo() );
}

HERROR DB_SVC_UpdateAntInfo(
					HUINT16 			 usAntIdx,		/**< 업데이트 할 antenna index */
					DbSvc_AntInfo_t 	*pstAntInfo 	/**< 업데이트 내용 */
					)
{
	DB_SM_SYNC( db_svc_MgrUpdateAntInfo(usAntIdx, pstAntInfo) );
}

HERROR DB_SVC_GetAntInfo(
					HUINT16 			 usAntIdx,		/**< 읽고자 하는 antenna index */
					DbSvc_AntInfo_t 	*pstAntInfo 	/**< 담아올 구조체 */
					)
{
	DB_SM_SYNC( db_svc_MgrGetAntInfo(usAntIdx, pstAntInfo) );
}

HERROR DB_SVC_GetAntCount(HINT32 *pnCnt)
{
	DB_SM_SYNC( db_svc_MgrGetAntCount(pnCnt) );
}

HERROR DB_SVC_FindAllAntIdxList(HINT32	 *pnCnt,
								HINT32	**ppnAntIdxList)
{
	DB_SM_SYNC( db_svc_MgrFindAllAntIdxList(pnCnt, ppnAntIdxList) );
}

HBOOL	DB_SVC_IsAntChanged (void)
{
	DB_SM_SYNC_RETURN(HBOOL, pdb_svc_IsAntChanged());
}

void	DB_SVC_CommitAllAntInfo (void)
{
	DB_SM_SYNC_VOID( db_svc_MgrCommit(1, eSvcTable_Antenna) );
}

void	DB_SVC_CommitAllSvcInfo (void)
{
	DB_SM_SYNC_VOID( db_svc_MgrCommit(1, eSvcTable_Service) );
}

void	DB_SVC_CommitAllTsInfo (void)
{
	DB_SM_SYNC_VOID( db_svc_MgrCommit(1, eSvcTable_Transponder) );
}

void	DB_SVC_CommitAllProviderInfo (void)
{
	DB_SM_SYNC_VOID( db_svc_MgrCommit(1, eSvcTable_Provider) );
}

void	DB_SVC_CommitAllNetInfo (void)
{
	DB_SM_SYNC_VOID( db_svc_MgrCommit(1, eSvcTable_Network) );
}

void	DB_SVC_CommitAllGroupInfo (void)
{
	DB_SM_SYNC_VOID( db_svc_MgrCommit(1, eSvcTable_Group) );
}

void	DB_SVC_CommitAllBouquetInfo (void)
{
	DB_SM_SYNC_VOID( db_svc_MgrCommit(1, eSvcTable_Bouquet) );
}

void DB_SVC_FreeAntList(HINT32 *pnAntIdxList)
{
	if (pnAntIdxList)
		OxDB_Free(pnAntIdxList);
}

HERROR	DB_SVC_ReloadAllAntInfo (void)
{
	return pdb_svc_DamaAdaptorLoadAntennaDB();
}

HERROR	DB_SVC_AntennaSync(void)
{
	return pdb_svc_DamaAdaptorSyncAntennaDB();
}

void		DB_SVC_DumpSvcInfo (const DbSvc_Info_t *pstInfo)
{
	DB_SM_SYNC_VOID( db_svc_MgrDumpSvcInfo(pstInfo) );
}

void		DB_SVC_DumpAllSvcInfo (const HCHAR *pszFileName)
{
	STATIC HINT32	s_nSerial;
	HCHAR	szBuf[128];

	if (pszFileName == NULL)
	{
		HxSTD_PrintToStrN(szBuf, 128, "dump.%d.svc", s_nSerial++);
		pszFileName = (const HCHAR *)szBuf;
	}
	DB_SM_SYNC_VOID( db_svc_MgrDumpAllSvcInfo(pszFileName) );
}

HERROR DB_SVC_GetServiceDefaultAuthority(const DbSvc_Info_t *pstSvcInfo, HUINT8 **ppucDefaultAuthority)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	if (NULL == ppucDefaultAuthority)
		return ERR_FAIL;

	return pdb_svc_ServiceGetDefaultAuthority(pstSvcInfo, ppucDefaultAuthority);
}

HERROR DB_SVC_GetNetworkDefaultAuthority(const DbSvc_NetInfo_t *pstNetInfo, HUINT8 **ppucDefaultAuthority)
{
	if (NULL == pstNetInfo)
		return ERR_FAIL;

	if (NULL == ppucDefaultAuthority)
		return ERR_FAIL;

	return pdb_svc_NetworkGetDefaultAuthority(pstNetInfo, ppucDefaultAuthority);
}

HERROR DB_SVC_GetTsDefaultAuthority(const DbSvc_TsInfo_t *pstTsInfo, HUINT8 **ppucDefaultAuthority)
{
	if (NULL == pstTsInfo)
		return ERR_FAIL;

	if (NULL == ppucDefaultAuthority)
		return ERR_FAIL;

	return pdb_svc_TsGetDefaultAuthority(pstTsInfo, ppucDefaultAuthority);
}

HERROR DB_SVC_SetServiceDefaultAuthority(DbSvc_Info_t *pstSvcInfo, const HUINT8 *pucDefaultAuthority)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	if (NULL == pucDefaultAuthority)
		return ERR_FAIL;

	return pdb_svc_ServiceSetDefaultAuthority(pstSvcInfo, pucDefaultAuthority);
}

HERROR DB_SVC_SetNetworkDefaultAuthority(DbSvc_NetInfo_t *pstNetInfo, const HUINT8 *pucDefaultAuthority)
{
	if (NULL == pstNetInfo)
		return ERR_FAIL;

	if (NULL == pucDefaultAuthority)
		return ERR_FAIL;

	return pdb_svc_NetworkSetDefaultAuthority(pstNetInfo, pucDefaultAuthority);
}

HERROR DB_SVC_SetTsDefaultAuthority(DbSvc_TsInfo_t *pstTsInfo, const HUINT8 *pucDefaultAuthority)
{
	if (NULL == pstTsInfo)
		return ERR_FAIL;

	if (NULL == pucDefaultAuthority)
		return ERR_FAIL;

	return pdb_svc_TsSetDefaultAuthority(pstTsInfo, pucDefaultAuthority);
}

HERROR	DB_SVC_ResetServiceDefaultAuthority(DbSvc_Info_t *pstSvcInfo)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	return pdb_svc_ServiceResetDefaultAuthority(pstSvcInfo);
}

HERROR	DB_SVC_ResetNetworkDefaultAuthority(DbSvc_NetInfo_t *pstNetworkInfo)
{
	if (NULL == pstNetworkInfo)
		return ERR_FAIL;

	return pdb_svc_NetworkResetDefaultAuthority(pstNetworkInfo);
}

HERROR	DB_SVC_ResetTsDefaultAuthority(DbSvc_TsInfo_t *pstTsInfo)
{
	if (NULL == pstTsInfo)
		return ERR_FAIL;

	return pdb_svc_TsResetDefaultAuthority(pstTsInfo);
}


HERROR DB_SVC_GetServiceFTAContentMgr(const DbSvc_Info_t *pstSvcInfo, HUINT32 *pulFTAContentMgr)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	return pdb_svc_ServiceGetFTAContentMgr(pstSvcInfo, pulFTAContentMgr);
}

HERROR DB_SVC_GetNetworkFTAContentMgr(const DbSvc_NetInfo_t *pstNetInfo, HUINT32 *pulFTAContentMgr)
{
	if (NULL == pstNetInfo)
		return ERR_FAIL;


	return pdb_svc_NetworkGetFTAContentMgr(pstNetInfo, pulFTAContentMgr);
}

HERROR DB_SVC_GetTsFTAContentMgr(const DbSvc_TsInfo_t *pstTsInfo, HUINT32 *pulFTAContentMgr)
{
	if (NULL == pstTsInfo)
		return ERR_FAIL;


	return pdb_svc_TsGetFTAContentMgr(pstTsInfo, pulFTAContentMgr);
}
HERROR DB_SVC_SetServiceFTAContentMgr(DbSvc_Info_t *pstSvcInfo, const HUINT32 ulFTAContentMgr)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;


	return pdb_svc_ServiceSetFTAContentMgr(pstSvcInfo, ulFTAContentMgr);
}

HERROR DB_SVC_SetNetworkFTAContentMgr(DbSvc_NetInfo_t *pstNetInfo, const HUINT32 ulFTAContentMgr)
{
	if (NULL == pstNetInfo)
		return ERR_FAIL;

	return pdb_svc_NetworkSetFTAContentMgr(pstNetInfo, ulFTAContentMgr);
}

HERROR DB_SVC_SetTsFTAContentMgr(DbSvc_TsInfo_t *pstTsInfo, const HUINT32 ulFTAContentMgr)
{
	if (NULL == pstTsInfo)
		return ERR_FAIL;

	return pdb_svc_TsSetFTAContentMgr(pstTsInfo, ulFTAContentMgr);
}

HERROR DB_SVC_GetServiceGuidanceType(const DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceType)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	return pdb_svc_ServiceGetGuidanceType(pstSvcInfo, pucGuidanceType);
}

HERROR DB_SVC_GetServiceGuidanceMode(const DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceMode)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	return pdb_svc_ServiceGetGuidanceMode(pstSvcInfo, pucGuidanceMode);
}

HERROR DB_SVC_GetServiceGuidanceStr(const DbSvc_Info_t *pstSvcInfo, HUINT8 **ppucGuidanceStr)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	return pdb_svc_ServiceGetGuidanceStr(pstSvcInfo, ppucGuidanceStr);
}


HERROR DB_SVC_SetServiceGuidanceType(DbSvc_Info_t *pstSvcInfo, HUINT8 ucGuidanceType)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	return pdb_svc_ServiceSetGuidanceType(pstSvcInfo, ucGuidanceType);
}

HERROR DB_SVC_SetServiceGuidanceMode(DbSvc_Info_t *pstSvcInfo, HUINT8 ucGuidanceMode)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	return pdb_svc_ServiceSetGuidanceMode(pstSvcInfo, ucGuidanceMode);
}

HERROR DB_SVC_SetServiceGuidanceStr(DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceStr)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	return pdb_svc_ServiceSetGuidanceStr(pstSvcInfo, pucGuidanceStr);
}

HERROR DB_SVC_GetServiceVirtualSvcId(const DbSvc_Info_t *pstSvcInfo, HUINT16 *pusVirtualSvcId)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	return pdb_svc_ServiceGetVirtualSvcId(pstSvcInfo, pusVirtualSvcId);
}

HERROR DB_SVC_GetServiceNewSvc(const DbSvc_Info_t *pstSvcInfo, HBOOL *pbNewSvc)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	return pdb_svc_ServiceGetNewSvc(pstSvcInfo, pbNewSvc);
}

HERROR DB_SVC_SetServiceVirtualSvcId(DbSvc_Info_t *pstSvcInfo, HUINT16 usVirtualSvcId)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	return pdb_svc_ServiceSetVirtualSvcId(pstSvcInfo, usVirtualSvcId);
}

HERROR DB_SVC_SetServiceNewSvc(DbSvc_Info_t *pstSvcInfo, HBOOL bNewSvc)
{
	if (NULL == pstSvcInfo)
		return ERR_FAIL;

	return pdb_svc_ServiceSetNewSvc(pstSvcInfo, bNewSvc);
}
HERROR DB_SVC_GetNetworkUriPrefix(const DbSvc_NetInfo_t *pstNetInfo, HUINT8 **ppucUriPrefix)
{
	if (NULL == pstNetInfo)
		return ERR_FAIL;

	if (NULL == ppucUriPrefix)
		return ERR_FAIL;

	return pdb_svc_NetworkGetUriPrefix(pstNetInfo, ppucUriPrefix);
}
HERROR DB_SVC_SetNetworkUriPrefix(DbSvc_NetInfo_t *pstNetInfo, const HUINT8 *pucUriPrefix)
{
	if (NULL == pstNetInfo)
		return ERR_FAIL;

	if (NULL == pucUriPrefix)
		return ERR_FAIL;

	return pdb_svc_NetworkSetUriPrefix(pstNetInfo, pucUriPrefix);
}

HINT32 DB_SVC_UtilCompareDbTuningParam(HUINT32 ulDeliType, DbSvc_TuningInfo_t *pstTune1, DbSvc_TuningInfo_t *pstTune2)
{
	HINT32		 nDiff;

	if (pstTune1 == NULL || pstTune2 == NULL)							{ return 1; }

	switch (ulDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:
		// Antenna Id
		if (pstTune1->sat.ucAntId != pstTune2->sat.ucAntId)				{ return -1; }
		else if (pstTune1->sat.ucAntId > pstTune2->sat.ucAntId)			{ return  1; }

		// Symbol Rate
		if (pstTune1->sat.ulSymbolRate < pstTune2->sat.ulSymbolRate)					{ return -1; }
		else if (pstTune1->sat.ulSymbolRate > pstTune2->sat.ulSymbolRate)				{ return  1; }

		//////////////// Frequency 비교 ////////////////
		//		45~18Ms/s	=	+/- (8MHz) search
		//		18 ~ 11Ms/s =	+/- (6MHz) search
		//		11 ~ 8Ms/s	=	+/- (5MHz) search
		//		8 ~ 4Ms/s	=	+/- (4MHz) search
		//		4 ~ 2Ms/s	=	+/- (3.8MHz) search
		//	=> 2003.09.09, 권기호 과장으로부터 입수

		// Frequency
		nDiff = (HINT32)pstTune1->sat.ulFrequency - (HINT32)pstTune2->sat.ulFrequency;
		if (pstTune1->sat.ulSymbolRate >= 18000)
		{
			if (nDiff <= -8)					{ return -1; }
			else if (nDiff >= 8)				{ return  1; }
		}
		else if (pstTune1->sat.ulSymbolRate >= 11000)
		{
			if (nDiff <= -6)					{ return -1; }
			else if (nDiff >= 6)				{ return  1; }
		}
		else if (pstTune1->sat.ulSymbolRate >= 8000)
		{
			if (nDiff <= -5)					{ return -1; }
			else if (nDiff >= 5)				{ return  1; }
		}
/*
		else if (pstTune1->sat.ulSymbolRate >= 4000)
		{
			if (nDiff <= -4)					{ return -1; }
			else if (nDiff >= 4)				{ return  1; }
		}
*/
		else if (pstTune1->sat.ulSymbolRate >= 2000)
		{
			if (nDiff <= -4)					{ return -1; }
			else if (nDiff >= 4)				{ return  1; }
		}
		else
		{
			if (nDiff < 0)						{ return -1; }
			else if (nDiff > 0)					{ return  1; }
		}

		// Polarization
		if (pstTune1->sat.ePolarization != eDxSAT_POLAR_AUTO && pstTune2->sat.ePolarization != eDxSAT_POLAR_AUTO)
		{
			if (pstTune1->sat.ePolarization < pstTune2->sat.ePolarization)			{ return -1; }
			else if (pstTune1->sat.ePolarization > pstTune2->sat.ePolarization)		{ return  1; }
		}

		// FEC
		if (pstTune1->sat.eFecCodeRate != eDxSAT_CODERATE_AUTO && pstTune2->sat.eFecCodeRate != eDxSAT_CODERATE_AUTO)
		{
			if (pstTune1->sat.eFecCodeRate < pstTune2->sat.eFecCodeRate)							{ return -1; }
			else if (pstTune1->sat.eFecCodeRate > pstTune2->sat.eFecCodeRate)						{ return  1; }
		}

		// Transmission
		if (pstTune1->sat.eTransSpec < pstTune2->sat.eTransSpec)					{ return -1; }
		else if (pstTune1->sat.eTransSpec > pstTune2->sat.eTransSpec)				{ return  1; }

		// Modulation
		if (pstTune1->sat.ePskMod != eDxSAT_PSK_AUTO && pstTune2->sat.ePskMod != eDxSAT_PSK_AUTO)
		{
			if (pstTune1->sat.ePskMod < pstTune2->sat.ePskMod)		{ return -1; }
			else if (pstTune1->sat.ePskMod > pstTune2->sat.ePskMod)	{ return  1; }
		}

		// Pilot On Off
		if (pstTune1->sat.ePilot != eDxSAT_PILOT_AUTO && pstTune2->sat.ePilot != eDxSAT_PILOT_AUTO)
		{
			if (pstTune1->sat.ePilot < pstTune2->sat.ePilot)				{ return -1; }
			else if (pstTune1->sat.ePilot > pstTune2->sat.ePilot)			{ return  1; }
		}

#if 0	//Pal_ch_sat.c에서 모두 DI_CH_ROLL_035로 처리하므로 비교 할 필요 없음.
		// Roll Off
		if (pstTune1->sat.eRollOff < pstTune2->sat.eRollOff)						{ return -1; }
		else if (pstTune1->sat.eRollOff > pstTune2->sat.eRollOff)					{ return  1; }
#endif

		// Same TP
		return 0;

	case eDxDELIVERY_TYPE_CAB:
		// Symbol Rate
		if (pstTune1->cab.usSR < pstTune2->cab.usSR)					{ return -1; }
		else if (pstTune1->cab.usSR > pstTune2->cab.usSR)				{ return  1; }

		// Constellation
		if (pstTune1->cab.ucCon != eDxCAB_CONSTELLATION_AUTO && pstTune2->cab.ucCon != eDxCAB_CONSTELLATION_AUTO)
		{
			if (pstTune1->cab.ucCon < pstTune2->cab.ucCon)				{ return -1; }
			else if (pstTune1->cab.ucCon > pstTune2->cab.ucCon)			{ return  1; }
		}

		// Spectrum
		if (pstTune1->cab.ucSpec != eDxCAB_SPECTRUM_AUTO && pstTune2->cab.ucSpec != eDxCAB_SPECTRUM_AUTO)
		{
			if (pstTune1->cab.ucSpec < pstTune2->cab.ucSpec)			{ return -1; }
			else if (pstTune1->cab.ucSpec > pstTune2->cab.ucSpec)		{ return  1; }
		}

		// Frequency
		nDiff = (HINT32)pstTune1->cab.ulFreq - (HINT32)pstTune2->cab.ulFreq;
		if (nDiff < -32)					{ return -1; }
		if (nDiff > 32)						{ return  1; }

		// Same TP
		return 0;

	case eDxDELIVERY_TYPE_TER:
		// Trams mode
		if (pstTune1->ter.ucTransMode != eDxTER_TRANSMODE_AUTO && pstTune2->ter.ucTransMode != eDxTER_TRANSMODE_AUTO)
		{
			if (pstTune1->ter.ucTransMode < pstTune2->ter.ucTransMode)				{ return -1; }
			else if (pstTune1->ter.ucTransMode > pstTune2->ter.ucTransMode)			{ return  1; }
		}

		// Guard Interval
		if (pstTune1->ter.ucGuardInterval != eDxTER_GUARDINTERVAL_AUTO && pstTune2->ter.ucGuardInterval != eDxTER_GUARDINTERVAL_AUTO)
		{
			if (pstTune1->ter.ucGuardInterval < pstTune2->ter.ucGuardInterval)				{ return -1; }
			else if (pstTune1->ter.ucGuardInterval > pstTune2->ter.ucGuardInterval)			{ return  1; }
		}

		// Bandwidth
		if (pstTune1->ter.ucBandwidth != pstTune2->ter.ucBandwidth)
		{
			if (pstTune1->ter.ucBandwidth < pstTune2->ter.ucBandwidth)				{ return -1; }
			else if (pstTune1->ter.ucBandwidth > pstTune2->ter.ucBandwidth)			{ return  1; }
		}

#if defined(CONFIG_MW_CH_DVB_T2)
		// PLP ID
		//different transponder if system is different despite of same frequency.
		if(pstTune1->ter.ucSystem != pstTune2->ter.ucSystem)
		{
			return 1;
		}
		else
		{
			if((pstTune1->ter.stT2param.ulPlpId != 0x0000FFFF) &&
			 (pstTune2->ter.stT2param.ulPlpId != 0x0000FFFF)
			&& (pstTune1->ter.stT2param.ulPlpId != pstTune2->ter.stT2param.ulPlpId ))
			{
				return 1;
			}
		}
#endif
		// Frequency
		nDiff = (HINT32)pstTune1->ter.ulFreq - (HINT32)pstTune2->ter.ulFreq;
		if (nDiff < -3000)					{ return -1; }
		if (nDiff > 3000)					{ return  1; }

		// Same TP
		return 0;

	default:
		break;
	}

	return 1;
}

STATIC HINT32 db_svc_UtilCompareLnbAntInfo(DxLnbAntenna_t *pstAnt1, DxLnbAntenna_t *pstAnt2)
{
	if (pstAnt1->ulLnbFreq > pstAnt2->ulLnbFreq)						{ return 1; }
	else if (pstAnt1->ulLnbFreq < pstAnt2->ulLnbFreq)					{ return -1; }

	if (pstAnt1->lnbVoltage > pstAnt2->lnbVoltage)						{ return 1; }
	else if (pstAnt1->lnbVoltage < pstAnt2->lnbVoltage)					{ return -1; }

	return 0;
}

STATIC HINT32 db_svc_UtilCompareDiseqcAntInfo(DxDiseqcAntenna_t *pstAnt1, DxDiseqcAntenna_t *pstAnt2)
{
	if (pstAnt1->ulLnbFreq > pstAnt2->ulLnbFreq)						{ return 1; }
	else if (pstAnt1->ulLnbFreq < pstAnt2->ulLnbFreq)					{ return -1; }

	if (pstAnt1->lnbVoltage > pstAnt2->lnbVoltage)						{ return 1; }
	else if (pstAnt1->lnbVoltage < pstAnt2->lnbVoltage)					{ return -1; }

	if (pstAnt1->b22kTone > pstAnt2->b22kTone)							{ return 1; }
	else if (pstAnt1->b22kTone < pstAnt2->b22kTone)						{ return -1; }

	if (pstAnt1->diseqcVersion > pstAnt2->diseqcVersion)				{ return 1; }
	else if (pstAnt1->diseqcVersion < pstAnt2->diseqcVersion)			{ return -1; }

	if (pstAnt1->diseqcInput > pstAnt2->diseqcInput)					{ return 1; }
	else if (pstAnt1->diseqcInput < pstAnt2->diseqcInput)				{ return -1; }

	if (pstAnt1->toneBurst > pstAnt2->toneBurst)						{ return 1; }
	else if (pstAnt1->toneBurst < pstAnt2->toneBurst)					{ return -1; }

	if (pstAnt1->ucMotorPosition > pstAnt2->ucMotorPosition)			{ return 1; }
	else if (pstAnt1->ucMotorPosition < pstAnt2->ucMotorPosition)		{ return -1; }

	if (pstAnt1->sSatLonggitude > pstAnt2->sSatLonggitude)				{ return 1; }
	else if (pstAnt1->sSatLonggitude < pstAnt2->sSatLonggitude)			{ return -1; }

	return 0;
}

STATIC HINT32 db_svc_UtilCompareScdAntInfo(DxScdAntenna_t *pstAnt1, DxScdAntenna_t *pstAnt2)
{
	if (pstAnt1->ulLnbFreq > pstAnt2->ulLnbFreq)						{ return 1; }
	else if (pstAnt1->ulLnbFreq < pstAnt2->ulLnbFreq)					{ return -1; }

	if (pstAnt1->lnbVoltage > pstAnt2->lnbVoltage)						{ return 1; }
	else if (pstAnt1->lnbVoltage < pstAnt2->lnbVoltage)					{ return -1; }

	if (pstAnt1->b22kTone > pstAnt2->b22kTone)							{ return 1; }
	else if (pstAnt1->b22kTone < pstAnt2->b22kTone)						{ return -1; }

	if (pstAnt1->scdInput > pstAnt2->scdInput)							{ return 1; }
	else if (pstAnt1->scdInput < pstAnt2->scdInput)						{ return -1; }

	if (pstAnt1->ulScdUserBandNum > pstAnt2->ulScdUserBandNum)			{ return 1; }
	else if (pstAnt1->ulScdUserBandNum < pstAnt2->ulScdUserBandNum)		{ return -1; }

	if (pstAnt1->ulScdUserBandFreq > pstAnt2->ulScdUserBandFreq)		{ return 1; }
	else if (pstAnt1->ulScdUserBandFreq < pstAnt2->ulScdUserBandFreq)	{ return -1; }

	if (pstAnt1->ulScdUserBandNum1 > pstAnt2->ulScdUserBandNum1)		{ return 1; }
	else if (pstAnt1->ulScdUserBandNum1 < pstAnt2->ulScdUserBandNum1)	{ return -1; }

	if (pstAnt1->ulScdUserBandFreq1 > pstAnt2->ulScdUserBandFreq1)		{ return 1; }
	else if (pstAnt1->ulScdUserBandFreq1 < pstAnt2->ulScdUserBandFreq1)	{ return -1; }

	return 0;
}

HINT32 DB_SVC_UtilCompareChTuningParam(DxTuneParam_t *pstTune1, DxTuneParam_t *pstTune2)
{
	HINT32		 nDiff;

	if (pstTune1 == NULL || pstTune2 == NULL)					{ return 1; }
	if (pstTune1->eDeliType != pstTune2->eDeliType)		{ return 1; }

	switch (pstTune1->eDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:
		// Antenna Type
		if (pstTune1->sat.antennaType != pstTune2->sat.antennaType)		{ return 1; }
		switch (pstTune1->sat.antennaType)
		{
		case eDxANT_TYPE_LNB_ONLY:
			nDiff = db_svc_UtilCompareLnbAntInfo(&(pstTune1->sat.antInfo.info.lnb), &(pstTune2->sat.antInfo.info.lnb));
			break;

		case eDxANT_TYPE_DISEQC:
			nDiff = db_svc_UtilCompareDiseqcAntInfo(&(pstTune1->sat.antInfo.info.diseqc), &(pstTune2->sat.antInfo.info.diseqc));
			break;

		case eDxANT_TYPE_SCD:
			nDiff = db_svc_UtilCompareScdAntInfo(&(pstTune1->sat.antInfo.info.scd), &(pstTune2->sat.antInfo.info.scd));
			break;

		default:
			nDiff = 0;
			break;
		}

		if (nDiff != 0)					{ return nDiff; }

		// Symbol Rate
		if (pstTune1->sat.tuningInfo.ulSymbolRate < pstTune2->sat.tuningInfo.ulSymbolRate)					{ return -1; }
		else if (pstTune1->sat.tuningInfo.ulSymbolRate > pstTune2->sat.tuningInfo.ulSymbolRate)				{ return  1; }

		//////////////// Frequency 비교 ////////////////
		//		45~18Ms/s	=	+/- (8MHz) search
		//		18 ~ 11Ms/s =	+/- (6MHz) search
		//		11 ~ 8Ms/s	=	+/- (5MHz) search
		//		8 ~ 4Ms/s	=	+/- (4MHz) search
		//		4 ~ 2Ms/s	=	+/- (3.8MHz) search
		//	=> 2003.09.09, 권기호 과장으로부터 입수

		// Frequency
		nDiff = (HINT32)pstTune1->sat.tuningInfo.ulFrequency - (HINT32)pstTune2->sat.tuningInfo.ulFrequency;
		if (pstTune1->sat.tuningInfo.ulSymbolRate >= 18000)
		{
			if (nDiff <= -8)					{ return -1; }
			else if (nDiff >= 8)				{ return  1; }
		}
		else if (pstTune1->sat.tuningInfo.ulSymbolRate >= 11000)
		{
			if (nDiff <= -6)					{ return -1; }
			else if (nDiff >= 6)				{ return  1; }
		}
		else if (pstTune1->sat.tuningInfo.ulSymbolRate >= 8000)
		{
			if (nDiff <= -5)					{ return -1; }
			else if (nDiff >= 5)				{ return  1; }
		}
/*
		else if (pstTune1->sat.ulSymbolRate >= 4000)
		{
			if (nDiff <= -4)					{ return -1; }
			else if (nDiff >= 4)				{ return  1; }
		}
*/
		else if (pstTune1->sat.tuningInfo.ulSymbolRate >= 2000)
		{
			if (nDiff <= -4)					{ return -1; }
			else if (nDiff >= 4)				{ return  1; }
		}
		else
		{
			if (nDiff < 0)						{ return -1; }
			else if (nDiff > 0)					{ return  1; }
		}

		// Polarization
		if (pstTune1->sat.tuningInfo.ePolarization != eDxSAT_POLAR_AUTO && pstTune2->sat.tuningInfo.ePolarization != eDxSAT_POLAR_AUTO)
		{
			if (pstTune1->sat.tuningInfo.ePolarization < pstTune2->sat.tuningInfo.ePolarization)			{ return -1; }
			else if (pstTune1->sat.tuningInfo.ePolarization > pstTune2->sat.tuningInfo.ePolarization)		{ return  1; }
		}

		// FEC
		if (pstTune1->sat.tuningInfo.eFecCodeRate != eDxSAT_CODERATE_AUTO && pstTune2->sat.tuningInfo.eFecCodeRate != eDxSAT_CODERATE_AUTO)
		{
			if (pstTune1->sat.tuningInfo.eFecCodeRate < pstTune2->sat.tuningInfo.eFecCodeRate)							{ return -1; }
			else if (pstTune1->sat.tuningInfo.eFecCodeRate > pstTune2->sat.tuningInfo.eFecCodeRate)						{ return  1; }
		}

		// Transmission
		if (pstTune1->sat.tuningInfo.eTransSpec < pstTune2->sat.tuningInfo.eTransSpec)					{ return -1; }
		else if (pstTune1->sat.tuningInfo.eTransSpec > pstTune2->sat.tuningInfo.eTransSpec)				{ return  1; }

		// Modulation
		if (pstTune1->sat.tuningInfo.ePskMod != eDxSAT_PSK_AUTO && pstTune2->sat.tuningInfo.ePskMod != eDxSAT_PSK_AUTO)
		{
			if (pstTune1->sat.tuningInfo.ePskMod < pstTune2->sat.tuningInfo.ePskMod)		{ return -1; }
			else if (pstTune1->sat.tuningInfo.ePskMod > pstTune2->sat.tuningInfo.ePskMod)	{ return  1; }
		}

		// Pilot On Off
		if (pstTune1->sat.tuningInfo.ePilot != eDxSAT_PILOT_AUTO && pstTune2->sat.tuningInfo.ePilot != eDxSAT_PILOT_AUTO)
		{
			if (pstTune1->sat.tuningInfo.ePilot < pstTune2->sat.tuningInfo.ePilot)				{ return -1; }
			else if (pstTune1->sat.tuningInfo.ePilot > pstTune2->sat.tuningInfo.ePilot)			{ return  1; }
		}

#if 0	//Pal_ch_sat.c에서 모두 DI_CH_ROLL_035로 처리하므로 비교 할 필요 없음.
		// Roll Off
		if (pstTune1->sat.eRollOff < pstTune2->sat.eRollOff)						{ return -1; }
		else if (pstTune1->sat.eRollOff > pstTune2->sat.eRollOff)					{ return  1; }
#endif

		// Same TP
		return 0;

	case eDxDELIVERY_TYPE_CAB:
		// Symbol Rate
		if (pstTune1->cab.ulSymbolRate < pstTune2->cab.ulSymbolRate)				{ return -1; }
		else if (pstTune1->cab.ulSymbolRate > pstTune2->cab.ulSymbolRate)			{ return  1; }

		// Constellation
		if (pstTune1->cab.constellation != eDxCAB_CONSTELLATION_AUTO && pstTune2->cab.constellation != eDxCAB_CONSTELLATION_AUTO)
		{
			if (pstTune1->cab.constellation < pstTune2->cab.constellation)				{ return -1; }
			else if (pstTune1->cab.constellation > pstTune2->cab.constellation)			{ return  1; }
		}

		// Spectrum
		if (pstTune1->cab.spectrum != eDxCAB_SPECTRUM_AUTO && pstTune2->cab.spectrum != eDxCAB_SPECTRUM_AUTO)
		{
			if (pstTune1->cab.spectrum < pstTune2->cab.spectrum)			{ return -1; }
			else if (pstTune1->cab.spectrum > pstTune2->cab.spectrum)		{ return  1; }
		}

		// Frequency
		nDiff = (HINT32)pstTune1->cab.ulFrequency - (HINT32)pstTune2->cab.ulFrequency;
		if (nDiff < -32)					{ return -1; }
		if (nDiff > 32)						{ return  1; }

		// Same TP
		return 0;

	case eDxDELIVERY_TYPE_TER:
		// Trams mode
		if (pstTune1->ter.eTransMode != eDxTER_TRANSMODE_AUTO && pstTune2->ter.eTransMode != eDxTER_TRANSMODE_AUTO)
		{
			if (pstTune1->ter.eTransMode < pstTune2->ter.eTransMode)				{ return -1; }
			else if (pstTune1->ter.eTransMode > pstTune2->ter.eTransMode)			{ return  1; }
		}

		// Guard Interval
		if (pstTune1->ter.eGuardInterval != eDxTER_GUARDINTERVAL_AUTO && pstTune2->ter.eGuardInterval != eDxTER_GUARDINTERVAL_AUTO)
		{
			if (pstTune1->ter.eGuardInterval < pstTune2->ter.eGuardInterval)				{ return -1; }
			else if (pstTune1->ter.eGuardInterval > pstTune2->ter.eGuardInterval)			{ return  1; }
		}

#if defined(CONFIG_MW_CH_DVB_T2)
		// PLP ID
		if (pstTune1->ter.eSystem && (pstTune1->ter.stT2Param.ulPlpId != 0x0000FFFF) &&
			pstTune2->ter.eSystem && (pstTune2->ter.stT2Param.ulPlpId != 0x0000FFFF)
			       && (pstTune1->ter.stT2Param.ulPlpId != pstTune2->ter.stT2Param.ulPlpId ))
		{
			{ return  1; }
		}

#endif
		// Frequency
		nDiff = (HINT32)pstTune1->ter.ulFrequency - (HINT32)pstTune2->ter.ulFrequency;
		if (nDiff < -3000)					{ return -1; }
		if (nDiff > 3000)					{ return  1; }

		// Same TP
		return 0;

	default:
		break;
	}

	return 1;
}

#if 0
STATIC void test(void)
{
	DbSvc_SatTuningInfo_t	*pstTsInfo, stTsInfo;

#if defined(DbSvc_SUPPORT_DXINFO)
#define DbSvc_TsSatGetFrequency(pstTsInfo)									((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.ulFrequency
#define DbSvc_TsSatSetFrequency(pstTsInfo, ulValue)							((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.ulFrequency = ulValue

#define DbSvc_TsSatGetSymbolRate(pstTsInfo)									((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.ulSymbolRate
#define DbSvc_TsSatSetSymbolRate(pstTsInfo, ulValue)						((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.ulSymbolRate = ulValue

#define DbSvc_TsSatGetFecCodeRate(pstTsInfo)								((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.eFecCodeRate
#define DbSvc_TsSatSetFecCodeRate(pstTsInfo, eValue)						((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.eFecCodeRate

#define DbSvc_TsSatGetPolarization(pstTsInfo)								((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.ePolarization
#define DbSvc_TsSatSetPolarization(pstTsInfo, eValue)						((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.ePolarization

#define DbSvc_TsSatGetTransSpec(pstTsInfo)									((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.eTransSpec
#define DbSvc_TsSatSetTransSpec(pstTsInfo, eValue)							((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.eTransSpec

#define DbSvc_TsSatGetPskMode(pstTsInfo)									((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.ePskMod
#define DbSvc_TsSatSetPskMode(pstTsInfo, eValue)							((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.ePskMod

#define DbSvc_TsSatGetPilot(pstTsInfo)										((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.ePilot
#define DbSvc_TsSatSetPilot(pstTsInfo, eValue)								((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.ePilot

#define DbSvc_TsSatGetRollOff(pstTsInfo)									((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.eRollOff
#define DbSvc_TsSatSetRollOff(pstTsInfo, eValue)							((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.eRollOff

#define DbSvc_TsSatGetSatType(pstTsInfo)									OTL_CONV_SatString2SVC_SatType((const HCHAR *)((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.satType)
#define DbSvc_TsSatSetSatType(pstTsInfo, eValue)							HLIB_STD_StrNCpySafe (((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.satType, OTL_CONV_SVC_SatType2String(eValue), DxANTINFO_NAME_LEN);

#define DbSvc_TsSatGetAntId(pstTsInfo)										(HUINT8)((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.antuid
#define DbSvc_TsSatSetAntId(pstTsInfo, ulValue)								((DbSvc_SatTuningInfo_t *)pstTsInfo)->stDxInfo.antuid = ulValue
#else
#define DbSvc_TsSatGetFrequency(pstTsInfo)									((DbSvc_SatTuningInfo_t *)pstTsInfo)->ulFrequency
#define DbSvc_TsSatSetFrequency(pstTsInfo, ulValue)							((DbSvc_SatTuningInfo_t *)pstTsInfo)->ulFrequency = ulValue

#define DbSvc_TsSatGetSymbolRate(pstTsInfo)									((DbSvc_SatTuningInfo_t *)pstTsInfo)->ulSymbolRate
#define DbSvc_TsSatSetSymbolRate(pstTsInfo, ulValue)						((DbSvc_SatTuningInfo_t *)pstTsInfo)->ulSymbolRate = ulValue

#define DbSvc_TsSatGetFecCodeRate(pstTsInfo)								((DbSvc_SatTuningInfo_t *)pstTsInfo)->eFecCodeRate
#define DbSvc_TsSatSetFecCodeRate(pstTsInfo, eValue)						((DbSvc_SatTuningInfo_t *)pstTsInfo)->eFecCodeRate

#define DbSvc_TsSatGetPolarization(pstTsInfo)								((DbSvc_SatTuningInfo_t *)pstTsInfo)->ePolarization
#define DbSvc_TsSatSetPolarization(pstTsInfo, eValue)						((DbSvc_SatTuningInfo_t *)pstTsInfo)->ePolarization

#define DbSvc_TsSatGetTransSpec(pstTsInfo)									((DbSvc_SatTuningInfo_t *)pstTsInfo)->eTransSpec
#define DbSvc_TsSatSetTransSpec(pstTsInfo, eValue)							((DbSvc_SatTuningInfo_t *)pstTsInfo)->eTransSpec

#define DbSvc_TsSatGetPskMode(pstTsInfo)									((DbSvc_SatTuningInfo_t *)pstTsInfo)->ePskMod
#define DbSvc_TsSatSetPskMode(pstTsInfo, eValue)							((DbSvc_SatTuningInfo_t *)pstTsInfo)->ePskMod

#define DbSvc_TsSatGetPilot(pstTsInfo)										((DbSvc_SatTuningInfo_t *)pstTsInfo)->ePilot
#define DbSvc_TsSatSetPilot(pstTsInfo, eValue)								((DbSvc_SatTuningInfo_t *)pstTsInfo)->ePilot

#define DbSvc_TsSatGetRollOff(pstTsInfo)									((DbSvc_SatTuningInfo_t *)pstTsInfo)->eRollOff
#define DbSvc_TsSatSetRollOff(pstTsInfo, eValue)							((DbSvc_SatTuningInfo_t *)pstTsInfo)->eRollOff

#define DbSvc_TsSatGetSatType(pstTsInfo)									((DbSvc_SatTuningInfo_t *)pstTsInfo)->eSatType
#define DbSvc_TsSatSetSatType(pstTsInfo, eValue)							((DbSvc_SatTuningInfo_t *)pstTsInfo)->eSatType = eValue

#define DbSvc_TsSatGetAntId(pstTsInfo)										(HUINT8)((DbSvc_SatTuningInfo_t *)pstTsInfo)->ucAntId
#define DbSvc_TsSatSetAntId(pstTsInfo, ucValue)								((DbSvc_SatTuningInfo_t *)pstTsInfo)->ucAntId = (HUINT8)ucValue
#endif

	DbSvc_TsSatGetSymbolRate(&stTsInfo);
	DbSvc_TsSatSetSymbolRate(&stTsInfo, 1000);
}
#endif

