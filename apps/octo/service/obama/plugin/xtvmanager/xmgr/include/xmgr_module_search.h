/**
	@file     xmgr_module_search.h
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/06/03		initiated					wjmoh\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XMGR_MODULE_SEARCH_H__
#define	__XMGR_MODULE_SEARCH_H__

#define SESLCN_SVC_STARTNUM			100
#define SESLCN_CHLIST_MAX			32

#if defined (CONFIG_OP_SES)
// APS HD+ Functions Mode
typedef enum
{
	eSesHdFuncMode_Normal = 0,
	eSesHdFuncMode_ColdBoot = 1,
	eSesHdFuncMode_StandBy = 2,

	eApsHdFuncMode_MAX
} SesHdFuncMode_t;

typedef enum
{
	eSesLcnSrchMode_None = 0,
	eSesLcnSrchMode_Update,
	eSesLcnSrchMode_Change,
	eSesLcnSrchMode_ForceFirstList,			// Test 를 위해 LCN 리스트를 받아서 무조건 첫번째 List로 업데이트하는 방식

	eSesLcnSrchMode_MAX
} SesLcnSearchMode_t;

typedef struct
{
	Handle_t		 hSdSvcHandle;			// Simulcast 되어야 하는 Service (Pre-programmed Service)
	Handle_t		 hHdSimulSvcHandle;		// Simulcast 되는 내용 Service (SdSvc 가 HdSimulSvc 로 Update 되어야 함)
} SesLcnReplaceInfo_t;

#if 0		// yjlim
// LCN Procedure :
BUS_Result_t AP_SesLcnSearch_Proc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
BUS_Result_t AP_SesLcnChList_Proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

// LCN Functions :
HERROR XMGR_SEARCH_LCNUPDATE_CountLcnSvcList (HUINT32 *pulListNum);
HERROR XMGR_SEARCH_LCNUPDATE_GetLcnSvcListInfo (HUINT32 ulListIdx, HUINT16 *pusSvcListId, HUINT8 *pucSvcListVer, HUINT8 *szSvcListName);
HERROR XMGR_SEARCH_LCNUPDATE_SimulcastPreferredList (HINT32 nCasMessage);

#if defined(CONFIG_MW_CAS_NAGRA_SES_CSC_DOWNLOAD)

// CSC Download
BUS_Result_t AP_CSCDownloadSearch_Proc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

// Common SES Functions :
HERROR	XMGR_SEARCH_INFO_GetSesHdFuncMode (SesHdFuncMode_t *peMode);
HERROR	XMGR_SEARCH_INFO_SetSesHdFuncMode (SesHdFuncMode_t eMode);
HERROR	XMGR_SEARCH_INFO_GetAstraAntIdx (HUINT32 *pulAntIdx);
HERROR	XMGR_SEARCH_INFO_InitNetworkInfo (void);
HERROR	XMGR_SEARCH_INFO_AddNetworkInfo (DbSvc_NetInfo_t *pstNetInfo);
HUINT32	XMGR_SEARCH_INFO_CountNetworkInfo (void);
HERROR	XMGR_SEARCH_INFO_GetNetworkInfo (HUINT32 ulIndex, DbSvc_NetInfo_t *pstNetInfo);
HUINT32	XMGR_SEARCH_INFO_CountAnchorTsInfo (void);
HERROR	XMGR_SEARCH_INFO_GetAnchorTsInfo (HUINT32 ulTsCnt, DbSvc_TsInfo_t *pstTsInfo);
HERROR	XMGR_SEARCH_INFO_InitDeliveryTsInfo (void);
HERROR	XMGR_SEARCH_INFO_AddDeliveryTsInfo (DbSvc_TsInfo_t *pstTsInfo);
HUINT32	XMGR_SEARCH_INFO_CountDeliveryTsInfo (void);
HERROR	XMGR_SEARCH_INFO_GetDeliveryTsInfo (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo);
HERROR	XMGR_SEARCH_INFO_InitAstraReferenceTsInfo (void);
HERROR	XMGR_SEARCH_INFO_AddAstraReferenceTsInfo (DbSvc_TsInfo_t *pstTsInfo);
HUINT32	XMGR_SEARCH_INFO_CountAstraReferenceTsInfo (void);
HERROR	XMGR_SEARCH_INFO_GetAstraReferenceTsInfo (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo);
HERROR	XMGR_SEARCH_INFO_InitAstraSgtTsInfo (void);
HERROR	XMGR_SEARCH_INFO_AddAstraSgtTsInfo (DbSvc_TsInfo_t *pstTsInfo, DbSvc_TripleId_t *pstTripleId);
HUINT32	XMGR_SEARCH_INFO_CountAstraSgtTsInfo (void);
HERROR	XMGR_SEARCH_INFO_GetAstraSgtTsInfo (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo, DbSvc_TripleId_t *pstTripleId);
#if defined(CONFIG_OP_SES_ASTRA_PORTAL)
HERROR XMGR_SEARCH_INFO_InitAstraPortalReferenceTsInfo (void);
HERROR XMGR_SEARCH_INFO_AddAstraPortalReferenceTsInfo (DbSvc_TsInfo_t *pstTsInfo);
HUINT32 XMGR_SEARCH_INFO_CountAstraPortalReferenceTsInfo (void);
HERROR XMGR_SEARCH_INFO_GetAstraPortalReferenceTsInfo (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo);
HERROR XMGR_SEARCH_INFO_InitAstraPortalTsInfo (void);
HERROR XMGR_SEARCH_INFO_AddAstraPortalTsInfo (DbSvc_TsInfo_t *pstTsInfo, DbSvc_TripleId_t *pstTripleId);
HUINT32 XMGR_SEARCH_INFO_CountAstraPortalTsInfo (void);
HERROR XMGR_SEARCH_INFO_GetAstraPortalTsInfo (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo, DbSvc_TripleId_t *pstTripleId);
HERROR XMGR_SEARCH_INFO_InitAstraRegionalPortalTsInfo (void);
HERROR XMGR_SEARCH_INFO_AddAstraRegionalPortalTsInfo (DbSvc_TsInfo_t *pstTsInfo, DbSvc_TripleId_t *pstTripleId);
HUINT32 XMGR_SEARCH_INFO_CountAstraRegionalPortalTsInfo (void);
HERROR XMGR_SEARCH_INFO_GetAstraRegionalPortalTsInfo (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo, DbSvc_TripleId_t *pstTripleId);
#endif

HERROR XMGR_SEARCH_INFO_GetSesLcnAllSearchFlag (HBOOL *pbAllFlag);
HERROR XMGR_SEARCH_INFO_SetSesLcnAllSearchFlag (HBOOL bAllFlag);
HERROR XMGR_SEARCH_INFO_GetSesLcnServiceListIdFromDb (HUINT16 *pusSvcListId);
HERROR XMGR_SEARCH_INFO_SetSesLcnServiceListIdToDb (HUINT16 usSvcListId);
HERROR XMGR_SEARCH_INFO_GetSesLcnServiceListVerFromDb (HUINT8 *pucSvcListVer);
HERROR XMGR_SEARCH_INFO_SetSesLcnServiceListVerToDb (HUINT8 ucSvcListVer);
HERROR XMGR_SEARCH_INFO_GetSesLcnServiceListNameFromDb (HUINT8 *szSvcListName);
HERROR XMGR_SEARCH_INFO_SetSesLcnServiceListNameToDb (HUINT8 *szSvcListName);
HERROR XMGR_SEARCH_INFO_GetSesLcnServiceNumberFromDb (HUINT32 *pulTvNum, HUINT32 *pulRadioNum);
HERROR XMGR_SEARCH_INFO_SetSesLcnServiceNumberToDb (HUINT32 ulTvNum, HUINT32 ulRadioNum);
HERROR XMGR_SEARCH_INFO_GetSesLcnLastCheckedTimeFromDb (UNIXTIME *putTime);
HERROR XMGR_SEARCH_INFO_SetSesLcnLastCheckedTimeToDb (UNIXTIME utTime);
HERROR XMGR_SEARCH_INFO_GetSesLcnLastUpdatedTimeFromDb (UNIXTIME *putTime);
HERROR XMGR_SEARCH_INFO_SetSesLcnLastUpdatedTimeToDb (UNIXTIME utTime);
HERROR XMGR_SEARCH_INFO_GetSesLcnColdBootResultFromDb (HUINT32 *pulValue);
HERROR XMGR_SEARCH_INFO_SetSesLcnColdBootResultToDb (HUINT32 ulValue);
HERROR XMGR_SEARCH_INFO_ClearSesLcnRelatedInfo (HBOOL bSyncDb);

#if defined(CONFIG_MW_CAS_NAGRA_SES_CSC_DOWNLOAD)

HERROR XMGR_SEARCH_INFO_MakeCscDownloadInfo(void *pData);
HERROR XMGR_SEARCH_INFO_GetCscDownloadInfo(HUINT32 *pulDataIdx, HUINT32 *pulCscTimeOut, HUINT32 *pulEMMTimeOut, DbSvc_TsInfo_t *pstTs);
HERROR XMGR_SEARCH_INFO_GetCscDownloadCSCRev(HUINT16 usSCID, HUINT8 *pucRev);
HERROR XMGR_SEARCH_INFO_ClearCscDownloadInfo(void);
#endif

#endif		// #if defined (CONFIG_OP_SES)

#endif /* __XMGR_MODULE_SEARCH_H__ */

