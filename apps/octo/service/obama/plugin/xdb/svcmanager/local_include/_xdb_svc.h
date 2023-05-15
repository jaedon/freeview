/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xdb_svc.h
	@brief	  freesat 을 위한 DB 과련 header file.

	Description: 		\n
	Module: 		 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __XDB_SVC_INT_H__
#define __XDB_SVC_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include	<db_svc.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
extern HBOOL	xdb_svc_Match_Base (const dbSvc_Service_t *pstSvc, const dbSvc_ServiceFilter_t *pstFilter);
extern HBOOL	xdb_svc_HasChanged_Base (void);
extern HBOOL	xdb_svc_IsAntChanged_Base (void);
extern HERROR	xdb_svc_ServiceSetUserFlag(DbSvc_Info_t *pstSvcInfo, const DbSvc_UserFlag1_e bwFlag);
extern HERROR	xdb_svc_ServiceGetUserFlag(DbSvc_UserFlag1_b *pbwFlag, const DbSvc_Info_t *pstSvcInfo);
extern HERROR	xdb_svc_ServiceCopyUserFlag(DbSvc_Info_t *pstDest, const DbSvc_Info_t *pstSource);
extern HERROR	xdb_svc_ServiceGetDefaultAuthority(const DbSvc_Info_t *pstSvcInfo, HUINT8 **ppucDefaultAuthority);
extern HERROR	xdb_svc_NetworkGetDefaultAuthority(const DbSvc_NetInfo_t *pstNetworkInfo, HUINT8 **ppucDefaultAuthority);
extern HERROR	xdb_svc_TsGetDefaultAuthority(const DbSvc_TsInfo_t *pstTsInfo, HUINT8 **ppucDefaultAuthority);
extern HERROR	xdb_svc_ServiceSetDefaultAuthority(DbSvc_Info_t *pstSvcInfo, const HUINT8 *pucDefaultAuthority);
extern HERROR	xdb_svc_NetworkSetDefaultAuthority(DbSvc_NetInfo_t *pstNetworkInfo, const HUINT8 *pucDefaultAuthority);
extern HERROR	xdb_svc_TsSetDefaultAuthority(DbSvc_TsInfo_t *pstTsInfo, const HUINT8 *pucDefaultAuthority);
extern HERROR	xdb_svc_ServiceResetDefaultAuthority(DbSvc_Info_t *pstSvcInfo);
extern HERROR	xdb_svc_NetworkResetDefaultAuthority(DbSvc_NetInfo_t *pstNetworkInfo);
extern HERROR	xdb_svc_TsResetDefaultAuthority(DbSvc_TsInfo_t *pstTsInfo);
extern HERROR	xdb_svc_ServiceGetFTAContentMgr(const DbSvc_Info_t *pstSvcInfo, HUINT32 *pnFTAContentMgr);
extern HERROR	xdb_svc_NetworkGetFTAContentMgr(const DbSvc_NetInfo_t *pstNetworkInfo, HUINT32 *pnFTAContentMgr);
extern HERROR	xdb_svc_TsGetFTAContentMgr(const DbSvc_TsInfo_t *pstTsInfo, HUINT32 *pnFTAContentMgr);
extern HERROR	xdb_svc_ServiceSetFTAContentMgr(DbSvc_Info_t *pstSvcInfo, const HUINT32 nFTAContentMgr);
extern HERROR	xdb_svc_NetworkSetFTAContentMgr(DbSvc_NetInfo_t *pstNetworkInfo, const HUINT32 nFTAContentMgr);
extern HERROR	xdb_svc_TsSetFTAContentMgr(DbSvc_TsInfo_t *pstTsInfo, const HUINT32 nFTAContentMgr);

extern HERROR	xdb_svc_ServiceGetGuidanceType(const DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceType);
extern HERROR	xdb_svc_ServiceSetGuidanceType(DbSvc_Info_t *pstSvcInfo, HUINT8 ucGuidanceType);
extern HERROR	xdb_svc_ServiceGetGuidanceMode(const DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceMode);
extern HERROR	xdb_svc_ServiceSetGuidanceMode(DbSvc_Info_t *pstSvcInfo, HUINT8 ucGuidanceMode);
extern HERROR	xdb_svc_ServiceGetGuidanceStr(const DbSvc_Info_t *pstSvcInfo, HUINT8 **ppucGuidanceStr);
extern HERROR	xdb_svc_ServiceSetGuidanceStr( DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceStr);

extern HERROR	xdb_svc_ServiceGetVirtualSvcId(const DbSvc_Info_t *pstSvcInfo, HUINT16 *pusVirtualSvcId);
extern HERROR	xdb_svc_ServiceGetNewSvc(const DbSvc_Info_t *pstSvcInfo, HBOOL *pbNewSvc);
extern HERROR	xdb_svc_ServiceSetVirtualSvcId(DbSvc_Info_t *pstSvcInfo, HUINT16 usVirtualSvcId);
extern HERROR	xdb_svc_ServiceSetNewSvc(DbSvc_Info_t *pstSvcInfo, HBOOL bNewSvc);

extern HERROR xdb_svc_NetworkGetUriPrefix(const DbSvc_NetInfo_t *pstNetworkInfo, HUINT8 **ppucUriPrefix);
extern HERROR xdb_svc_NetworkSetUriPrefix(DbSvc_NetInfo_t *pstNetworkInfo, const HUINT8 *pucUriPrefix);


extern HERROR	xdb_svc_DamaAdaptorRegisterNotifier_Base(HBOOL bReg);
extern HERROR	xdb_svc_DamaAdaptorInitAntennaDB_Base(void);
extern HERROR	xdb_svc_DamaAdaptorInitServiceDB_Base(void);
extern HERROR	xdb_svc_DamaAdaptorInitTransponderDB_Base(void);
extern HERROR	xdb_svc_DamaAdaptorInitNetworkDB_Base(void);
extern HERROR	xdb_svc_DamaAdaptorInitGroupDB_Base(void);
extern HERROR	xdb_svc_DamaAdaptorInitProviderDB_Base(void);
extern HERROR	xdb_svc_DamaAdaptorInitBouquetDB_Base(void);

extern HERROR	xdb_svc_DamaAdaptorUpdateSvcInfo_Base(const DbSvc_Info_t *info);
extern HERROR	xdb_svc_DamaAdaptorLoadServiceInfoDB_Base (void);
extern HERROR	xdb_svc_DamaAdaptorLoadAntennaDB_Base (void);
extern HERROR	xdb_svc_DamaAdaptorSyncAntennaDB_Base (void);

#endif	/* !__XDB_SVC_INT_H__ */

