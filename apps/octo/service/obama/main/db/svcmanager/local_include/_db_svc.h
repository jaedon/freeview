/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _db_svc.h
	@brief

	Description:  \n
	Module: DB/ \n
	Remarks :										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __DB_SVCMANAGER_H__
#define __DB_SVCMANAGER_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include	<octo_common.h>
#include	<db_svc.h>
#include	<_db_svc_table.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR		db_svc_MgrInit(HUINT32 ulSemId);
void		db_svc_MgrCommit (HINT32 n, ...);
HBOOL		db_svc_MgrHasChanged (HBOOL bCheckItemCount, HINT32 n, ...);
HERROR		db_svc_MgrGetChanged (DbSvc_Table_e eTable, void **apvUpdateList, HINT32 *pnUpdateNum, void **apvRemoveList, HINT32 *pnRemoveNum);
void		db_svc_MgrClearAllInfo(void);
HERROR		db_svc_MgrAddServiceInfo(DbSvc_Info_t *pstSvcInfo, Handle_t *phSvc);
HERROR		db_svc_MgrAddNetworkInfo(DbSvc_NetInfo_t *pstNetworkInfo, HUINT16 *pusNetIdx);
HERROR		db_svc_MgrAddTsInfo(DbSvc_TsInfo_t *pstTsInfo, HUINT16 *pusTsIdx);
HERROR		db_svc_MgrAddProviderInfo(DbSvc_PrvInfo_t *pstPvrInfo, HUINT16 *pusPrvIdx);
HUINT32		db_svc_MgrPutServiceInfoList (const DbSvc_Info_t *pstList, HUINT32 nListNum);
HUINT32		db_svc_MgrPutNetworkInfoList (const DbSvc_NetInfo_t *pstList, HUINT32 nListNum);
HUINT32		db_svc_MgrPutTsInfoList (const DbSvc_TsInfo_t *pstList, HUINT32 nListNum);
HUINT32		db_svc_MgrPutProviderInfoList (const DbSvc_PrvInfo_t *pstList, HUINT32 nListNum);
HUINT32		db_svc_MgrPutGroupInfoList (const DbSvc_GroupInfo_t *pstList, HUINT32 nListNum);
HUINT32		db_svc_MgrPutBouquetInfoList (const DbSvc_BouqInfo_t *pstList, HUINT32 nListNum);
void		db_svc_MgrDeleteServiceList(DbSvc_Condition_t *pstCond);
void		db_svc_MgrDeleteAllSearchingService(void);
void		db_svc_MgrRemoveSearchFlag(void);
HERROR		db_svc_MgrDeleteAllNetworkService(DxDeliveryType_e eOrgDeliType);
HERROR		db_svc_MgrDeleteServiceInfoOnly(Handle_t hSvc);
HERROR		db_svc_MgrDeleteServiceInfo(Handle_t hSvc);
HERROR		db_svc_MgrDeleteAllServiceInfo(void); // bt Moon
HERROR		db_svc_MgrDeleteNetworkInfoOnly(HUINT16 usNetIdx);
HERROR		db_svc_MgrDeleteAllNetworkInfo(void); // bt Moon
HERROR		db_svc_MgrDeleteTsInfo(HUINT16 usTsIdx);
HERROR		db_svc_MgrDeleteAllTsInfo(void); // bt Moon
HERROR		db_svc_MgrDeleteProviderInfo(HUINT16 usPrvIdx);
HERROR		db_svc_MgrDeleteAllProviderInfo(void); // bt Moon
HERROR		db_svc_MgrUpdateServiceInfo(Handle_t hSvc, DbSvc_Info_t *pstSvcInfo);
HERROR		db_svc_MgrUpdateNetworkInfo(HUINT16 usNetIdx, DbSvc_NetInfo_t *pstNetworkInfo);
HERROR		db_svc_MgrUpdateTsInfo(HUINT16 usTsIdx, DbSvc_TsInfo_t *pstTsInfo);
HERROR		db_svc_MgrUpdateProviderInfo(HUINT16 usPrvIdx, DbSvc_PrvInfo_t *pstPvrInfo);
HERROR		db_svc_MgrUpdateServiceInfoEx(Handle_t hSvc, DbSvc_FieldList_b bwSvcField, DbSvc_FieldListExt_b bwSvcFieldExt, DbSvc_Info_t *pstSvcInfo);
HERROR		db_svc_MgrGetServiceInfo(Handle_t hSvc, DbSvc_Info_t *pstSvcInfo);
HERROR		db_svc_MgrGetTsInfo(HUINT16 usTsIdx, DbSvc_TsInfo_t *pstTsInfo);
HERROR		db_svc_MgrFindTsInfoList(DbSvc_TsCondition_t *pstCond, HINT32 *pnCnt, DbSvc_TsInfo_t **ppstInfoList);
HERROR		db_svc_MgrGetNetworkInfo(HUINT16 usNetIdx, DbSvc_NetInfo_t *pstNetworkInfo);
HERROR		db_svc_MgrGetPrvInfo(HUINT16 usPrvIdx, DbSvc_PrvInfo_t *pstPvrInfo);
HERROR		db_svc_MgrGetPrvInfoByServiceHandle(Handle_t hSvc, DbSvc_PrvInfo_t *pstPvrInfo);
HERROR		db_svc_MgrGetServiceCount(
								DbSvc_Group_e 		 eSvcGroup, 	/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 		*pstAttr, 		/**< [in]  filtering 조건 2 - 서비스 속성 */
								HINT32 				*pnCnt);
HERROR		db_svc_MgrGetServiceHandleBySvcIdx(HUINT16 usSvcIdx, Handle_t *phSvc);
HERROR		db_svc_MgrGetServiceHandleByNumber(DxDeliveryType_e eOrgDeliType,
							DxSvcType_e eSvcType, HINT32 nChNum, Handle_t *phSvc);
HERROR		db_svc_MgrGetServiceIdFromSvcHandle(Handle_t hSvc, HUINT16 *pusSvcId);
HERROR		db_svc_MgrGetServiceHandleByOnIdAndSvcId(DxSvcSection_e eSvcSection, HUINT16 usOnId, HUINT16 usSvcId, Handle_t *phSvc);
HERROR		db_svc_MgrGetUniqueIdFromSvcHandle(Handle_t hSvc, HUINT16 *pusUniqueId);
HERROR		db_svc_MgrGetServiceHandle(DxSvcSection_e eSvcSection, HUINT16 usTsIdx, HUINT16 usSvcId, Handle_t *phSvc);
HERROR		db_svc_MgrFindServiceListByNumber(DxDeliveryType_e 	  eOrgDeliType,		/**< [in]  filtering 조건 1 - delivery type: original delivery type, all 포함 sat/ter/.. 등 OR 가능 */
										DxSvcType_e 		  eSvcType, 		/**< [in]  filtering 조건 2 - service type */
										DbSvc_VisibleType_e   eVisibleType, 	/**< [in]  filtering 조건 3 - visible type */
										HINT32 				  nChNum, 			/**< [in]  filtering 조건 4 - 검색할 channel number */
										HUINT32 			 *pulSvcCount, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
										Handle_t 			**pphSvcList);		/**< [out] service handle list를 담아올 포인터. 받드시 db_svc_FreeServiceList()로 free 해줘야함 */
HERROR		db_svc_MgrGetTsCount(DbSvc_TsCondition_t *pstCond, HINT32 *pnCnt);
HERROR		db_svc_MgrGetNetworkCount(HINT32 *pnCnt);
HERROR		db_svc_MgrGetProviderCount(HINT32 *pnCnt);
HERROR		db_svc_MgrFindServiceHandle(
								DbSvc_Group_e 			 eSvcGroup,		/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 			*pstAttr, 		/**< [in]  filtering 조건 2 - 서비스 속성 */
								Handle_t 				 hOffsetSvc, 	/**< [in]  filtering 조건 3 - 기준되는 service handle. 만약 next를 찾고자하면 current service에 해당됨. */
								DbSvc_FindDirection_e	 eDirection, 	/**< [in]  filtering 조건 4 - 방향에 관한 조건 */
								Handle_t 				*phSvc);		/**< [out] 해당 조건을 만족하는 service handle을 복사할 포인터 */
HERROR		db_svc_MgrFindServiceHandleByCondition (
								const DbSvc_Condition_t	*pstCond,
								Handle_t				*phSvc
							);
HERROR		db_svc_MgrFindServiceHandleByTripleId(
								DxDeliveryType_e 		 eOrgDeliType, 	/**< [in]  서치 시 할당된 orginal delivery type */
								DbSvc_TripleId_t 		 stTripleId, 	/**< [in]  triple id (onid, tsid, svcid) */
								Handle_t 				*phSvc);		/**< [out] 해당 조건을 만족하는 service handle을 복사할 포인터 */
HERROR		db_svc_MgrFindSvcHandleListWhereSearchFlagTrue(HINT32 *pnCnt, Handle_t **pphSvcList);
HERROR		db_svc_MgrFindServiceList(
								DbSvc_Group_e 			  eSvcGroup, 	/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 			 *pstAttr, 		/**< [in]  filtering 조건 2 - 서비스 속성 */
								HUINT32					  ulMaxFindNum, /**< [in]  filtering 조건 3 - 가져올 최대 서비스 개수, 0이면 LIMIT이 없다 */
								Handle_t				  hOffsetSvc, 	/**< [in]  filtering 조건 4 - (Find 시작) 기준되는 hSvc */
								DbSvc_FindDirection_e	  eDirection, 	/**< [in]  filtering 조건 5 - 방향에 관한 조건 */
								HUINT32 				 *pulCount, 	/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 				**pphSvcList);	/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
HERROR		db_svc_MgrFindServiceListBySvcId(
								DxDeliveryType_e 	  eOrgDeliType, 	/**< [in]  filtering 조건 1 - original delivery type */
								HUINT16 			  usSvcId, 			/**< [in]  filtering 조건 2 - service id */
								HINT32 				 *pnSvcCnt, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 			**pphSvcList);		/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
HERROR		db_svc_MgrFindServiceListByTripleId(
								DxDeliveryType_e 	  eOrgDeliType, 	/**< [in]  filtering 조건 1 - original delivery type */
								DbSvc_TripleId_t 	  stTripleId, 		/**< [in]  filtering 조건 2 - triple id */
								HINT32 				 *pnSvcCnt, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 			**pphSvcList);		/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
HERROR		db_svc_MgrFindServiceListWithInfo(
								DbSvc_Group_e 		  eSvcGroup, 		/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 		 *pAttr, 			/**< [in]  filtering 조건 2 - 서비스 속성 */
								HUINT32 			 *pulCount, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 			**pphSvcList,		/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
								DbSvc_Info_t		**ppstSvcInfoList);	/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
HERROR		db_svc_MgrFindSvcInfoList (DbSvc_Condition_t *pstCond, HINT32 *pnCnt, DbSvc_Info_t **ppInfoList);
HINT32		db_svc_MgrFindSvcUidList (const DbSvc_Condition_t *pstCond, HINT32 *pnUids, HINT32 n);
HERROR		db_svc_MgrFindSvcHandleList(DbSvc_Condition_t *pstCond, HINT32 *pnCnt, Handle_t **pphSvcList);
HERROR		db_svc_MgrFindTripleIdAndSvcTypeList(
								  DxDeliveryType_e	  eOrgDeliType
								, HUINT8			  ucAntIdx
								, HINT32			 *pnCnt
								, DbSvc_TripleId_t	**ppTripleIdList
								, DxSvcType_e		**ppeSvcTypeList
								, Handle_t			**pphSvcList
							);
HERROR		db_svc_MgrCheckValidServiceHandle(Handle_t hSvc);
void  		db_svc_MgrResetAudioUserFlag(void);
void  		db_svc_MgrResetSubtitleUserFlag(void);
void  		db_svc_MgrResetRatingPassedEventId(void);
HERROR		db_svc_MgrFindProviderIndexByName(HUINT8 *pszName, HUINT16 *pusPrvIdx);
HERROR		db_svc_MgrFindNetworkIndexById(DxDeliveryType_e eOrgDeliType, HUINT16 usNetId, HUINT16 *pusNetworkIdx);
HERROR		db_svc_MgrFindNetworkIndexByName(HUINT16 usNetId, HUINT8 *pszName, HUINT16 *pusNetworkIdx);
HERROR		db_svc_MgrFindNetworkInfoByDeliType(DxDeliveryType_e eOrgDeliType, HINT32 *pnCnt, DbSvc_NetInfo_t **ppstNetworkInfo);
HERROR		db_svc_MgrGetAllCasType(DxSvcType_e eSvcType, DxVideoType_e eVideoType, DxCasType_b *pbwCasType);
HERROR		db_svc_MgrIsLcnNetwork(DxDeliveryType_e eOrgDeliType, HBOOL *pbIsLcnNetwork);
HERROR		db_svc_MgrGetMaxLcnInLcnServices (DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HINT32 *pnLcn);
HERROR    	db_svc_MgrGetMaxLcnInInterval(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HINT32 nStartNum, HINT32 nEndNum, HINT32 *pnLcn);
HERROR		db_svc_MgrRenumberSavingOrderToChNum(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HINT32 nBaseStartChNum);
HERROR		db_svc_MgrGetLcnOrderNumber(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HUINT16 *pusLcn);
#if defined(CONFIG_DB_SVC_SVCBOUQUET)
HERROR		db_svc_MgrAddBouquetInfo(DbSvc_BouqInfo_t *pBouquetInfo, HUINT16 *pusBouqIdx);
HERROR		db_svc_MgrDeleteBouquetInfo(HUINT16 usBouqIdx);
HERROR		db_svc_MgrDeleteAllBouquetInfo(void);
HERROR		db_svc_MgrUpdateBouquetInfo(HUINT16 usBouqIdx, DbSvc_BouqInfo_t *pBouquetInfo);
HERROR		db_svc_MgrGetBouquetCount(HINT32 *pnCnt);
HERROR		db_svc_MgrGetBouquetInfo(HUINT16 usBouqIdx, DbSvc_BouqInfo_t *pBouquetInfo);
HERROR		db_svc_MgrFindBouquetIndexById(HUINT16 usBouqId, HUINT16 *pusBouqIdx);
HERROR		db_svc_MgrFindBouquetIndexByName(const HCHAR *pszName, HUINT16 *pusBouqIdx);
HERROR		db_svc_MgrFindBouquetIndexList(HINT32 *pnBouqNum, HUINT16 **ppusBouqIdxList);
HERROR		db_svc_MgrAddBouquetIndexToSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT16 usBqIdx);
HERROR		db_svc_MgrCountBouquetInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 *pulBqNum);
HERROR		db_svc_MgrGetBouquetIndexInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 ulIndex, HUINT16 *pusBqIdx);
HERROR		db_svc_MgrGetBouquetIndexArrayInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 ulBqArrayMaxNum, HUINT32 *pulBqIdxNum, HUINT16 *pusBqIdxArray);
HERROR		db_svc_MgrClearBouquetInfoInSvcInfo (DbSvc_Info_t *pstSvcInfo);
#endif
HERROR		db_svc_MgrAddGroupInfo (DbSvc_GroupInfo_t *pstGroupInfo, HUINT16 *pusGroupIdx);
HERROR		db_svc_MgrDeleteGroupInfo (HUINT16 usGroupIdx);
HERROR		db_svc_MgrDeleteAllGroupInfo(void);
HERROR		db_svc_MgrUpdateGroupInfo (HUINT16 usGroupIdx, DbSvc_GroupInfo_t *pstGroupInfo);
HERROR		db_svc_MgrCountGroup (HINT32 *pnCnt);
HERROR		db_svc_MgrGetGroupInfo (HUINT16 usGroupIdx, DbSvc_GroupInfo_t *pstGroupInfo);
HERROR		db_svc_MgrGetGroupInfoByGroupId (HUINT32 ulGroupId, DbSvc_GroupInfo_t *pstGroupInfo);
HERROR		db_svc_MgrFindGroupIndexByName (HUINT8 *pszName, HUINT16 *pusGroupIdx);
HERROR		db_svc_MgrGetAllGroupIndexList (HINT32 *pnGroupNum, HUINT16 **ppusGroupIdxList);
HERROR     	db_svc_MgrGetAllGroupInfoList (HINT32 *pnGroupNum, DbSvc_GroupInfo_t **pstGroupInfoList);
#if defined(CONFIG_DB_SVC_SVCGROUP)
HERROR		db_svc_MgrCountGroupIndexInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 *pulGroupNum);
HERROR		db_svc_MgrAddGroupIndexToSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT16 usGroupIdx);
HERROR		db_svc_MgrGetGroupIndexArrayInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 ulGroupArrayMaxNum, HUINT32 *pulGroupIdxNum, HUINT16 *pusGroupIdxArray);
HERROR		db_svc_MgrGetGroupIndexInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 ulIndex, HUINT16 *pusGroupIdx);
HERROR		db_svc_MgrDeleteGroupIndexInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT16 usGroupIdx);
HERROR		db_svc_MgrClearGroupInSvcInfo (DbSvc_Info_t *pstSvcInfo);
#endif
DbSvc_TsInfo_t		*db_svc_MgrGetAllTsInfo (HINT32 *pnCount);
DbSvc_NetInfo_t		*db_svc_MgrGetAllNetInfo (HINT32 *pnCount);
DbSvc_PrvInfo_t		*db_svc_MgrGetAllPrvInfo (HINT32 *pnCount);
DbSvc_Info_t		*db_svc_MgrGetAllSvcInfo (HINT32 *pnCount);
DbSvc_BouqInfo_t	*db_svc_MgrGetAllBouqInfo (HINT32 *pnCount);
HERROR		db_svc_MgrPutAntInfo (HUINT16 usAntIdx, const DbSvc_AntInfo_t *pstAntInfo);
HERROR		db_svc_MgrAddAntInfo(
						DbSvc_AntInfo_t 	*pstAntInfo, 	/**< 추가할 antenna info */
						HUINT16 			*pusAntIdx		/**< db에 추가 후 자동 설정된 index */
						);
HERROR		db_svc_MgrDeleteAntInfo(
						HUINT16 			usAntIdx		/**< 지우고자 하는 antenna index */
						);
HERROR		db_svc_MgrDeleteAllAntInfo(void);
HERROR		db_svc_MgrUpdateAntInfo(
						HUINT16 			usAntIdx,		/**< 업데이트 할 antenna index */
						DbSvc_AntInfo_t 	*pstAntInfo 	/**< 업데이트 내용 */
						);
HERROR		db_svc_MgrGetAntInfo(
						HUINT16 			usAntIdx,		/**< 읽고자 하는 antenna index */
						DbSvc_AntInfo_t 	*pstAntInfo 	/**< 담아올 구조체 */
						);
HERROR		db_svc_MgrGetAntCount(HINT32 *pnCnt);
HERROR		db_svc_MgrFindAllAntIdxList(HINT32 *pnCnt, HINT32 **ppnAntIdxList);
void		db_svc_MgrDumpSvcInfo (const DbSvc_Info_t *pstInfo);
void		db_svc_MgrDumpAllSvcInfo (const HCHAR *pszFileName);

dbSvc_TableList_t *	db_svc_MgrGetList(DbSvc_Table_e eTableIdx);
Handle_t	*db_svc_MgrMakeServiceList(HxVector_t *pstVec, HUINT32 *pulCount);
HERROR		db_svc_PrechLoad(HxCountry_e eCountryId);

#endif //__DB_SVCMANAGER_H__
