/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pdb_svc.h
	@brief

	Description:  \n
	Module: DB/DEFAULT \n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__PDB_SVC_H__
#define	__PDB_SVC_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <db_svc.h>
#include <_db_svc_service.h>


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

/*	DB Service Manager
 *
 */
HERROR pdb_svc_Init (void);

/*	DB SVC
 *
 */
HBOOL pdb_svc_HasChanged (void);

/*	DB Antenna
 *
 */
HBOOL pdb_svc_IsAntChanged (void);

HERROR pdb_svc_ServiceSetUserFlag(DbSvc_Info_t *pstSvcInfo, const DbSvc_UserFlag1_e bwFlag);
HERROR pdb_svc_ServiceGetUserFlag(DbSvc_UserFlag1_b *pbwFlag, const DbSvc_Info_t *pstSvcInfo);
HERROR pdb_svc_ServiceCopyUserFlag(DbSvc_Info_t *pstDest, const DbSvc_Info_t *pstSource);
HERROR pdb_svc_ServiceGetDefaultAuthority(const DbSvc_Info_t *pstSvcInfo, HUINT8 **ppucDefaultAuthority);
HERROR pdb_svc_NetworkGetDefaultAuthority(const DbSvc_NetInfo_t *pstNetInfo, HUINT8 **ppucDefaultAuthority);
HERROR pdb_svc_TsGetDefaultAuthority(const DbSvc_TsInfo_t *pstTsInfo, HUINT8 **ppucDefaultAuthority);
HERROR pdb_svc_ServiceSetDefaultAuthority(DbSvc_Info_t *pstSvcInfo, const HUINT8 *pucDefaultAuthority);
HERROR pdb_svc_NetworkSetDefaultAuthority(DbSvc_NetInfo_t *pstNetInfo, const HUINT8 *pucDefaultAuthority);
HERROR pdb_svc_TsSetDefaultAuthority(DbSvc_TsInfo_t *pstTsInfo, const HUINT8 *pucDefaultAuthority);
HERROR pdb_svc_ServiceResetDefaultAuthority(DbSvc_Info_t *pstSvcInfo);
HERROR pdb_svc_NetworkResetDefaultAuthority(DbSvc_NetInfo_t *pstNetworkInfo);
HERROR pdb_svc_TsResetDefaultAuthority(DbSvc_TsInfo_t *pstTsInfo);


HERROR pdb_svc_ServiceGetFTAContentMgr(const DbSvc_Info_t *pstSvcInfo, HUINT32 *pncFTAContentMgr);
HERROR pdb_svc_NetworkGetFTAContentMgr(const DbSvc_NetInfo_t *pstNetInfo, HUINT32 *pncFTAContentMgr);
HERROR pdb_svc_TsGetFTAContentMgr(const DbSvc_TsInfo_t *pstTsInfo, HUINT32 *pncFTAContentMgr);
HERROR pdb_svc_ServiceSetFTAContentMgr(DbSvc_Info_t *pstSvcInfo, const HUINT32 nFTAContentMgr);
HERROR pdb_svc_NetworkSetFTAContentMgr(DbSvc_NetInfo_t *pstNetInfo, const HUINT32 nFTAContentMgr);
HERROR pdb_svc_TsSetFTAContentMgr(DbSvc_TsInfo_t *pstTsInfo, const HUINT32 nFTAContentMgr);


HERROR pdb_svc_ServiceGetGuidanceType(const DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceType);
HERROR pdb_svc_ServiceGetGuidanceMode(const DbSvc_Info_t *pstSvcInfo, HUINT8*pucGuidanceMode);
HERROR pdb_svc_ServiceGetGuidanceStr(const DbSvc_Info_t *pstSvcInfo, HUINT8 **ppucGuidanceStr);
HERROR pdb_svc_ServiceSetGuidanceType(DbSvc_Info_t *pstSvcInfo, HUINT8 ucGuidanceType);
HERROR pdb_svc_ServiceSetGuidanceMode(DbSvc_Info_t *pstSvcInfo, HUINT8 ucGuidanceMode);
HERROR pdb_svc_ServiceSetGuidanceStr(DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceStr);

HERROR pdb_svc_ServiceGetVirtualSvcId(const DbSvc_Info_t *pstSvcInfo, HUINT16 *pusVirtualSvcId);
HERROR pdb_svc_ServiceGetNewSvc(const DbSvc_Info_t *pstSvcInfo, HBOOL *pbNewSvc);
HERROR pdb_svc_ServiceSetVirtualSvcId(DbSvc_Info_t *pstSvcInfo, HUINT16 usVirtualSvcId);
HERROR pdb_svc_ServiceSetNewSvc(DbSvc_Info_t *pstSvcInfo, HBOOL bNewSvc);
HERROR pdb_svc_NetworkGetUriPrefix(const DbSvc_NetInfo_t *pstNetworkInfo, HUINT8 **ppucUriPrefix);
HERROR pdb_svc_NetworkSetUriPrefix(DbSvc_NetInfo_t *pstNetworkInfo, const HUINT8 *pucUriPrefix);


/*	DB DAMA Adaptor
 *
 */
HERROR pdb_svc_DamaAdaptorInitAntennaDB(void);
HERROR pdb_svc_DamaAdaptorInitServiceDB(void);
HERROR pdb_svc_DamaAdaptorInitTransponderDB(void);
HERROR pdb_svc_DamaAdaptorInitNetworkDB(void);
HERROR pdb_svc_DamaAdaptorInitGroupDB(void);
HERROR pdb_svc_DamaAdaptorInitProviderDB(void);
HERROR pdb_svc_DamaAdaptorInitBouquetDB(void);


HERROR pdb_svc_DamaAdaptorSyncAntennaDB (void);
HERROR pdb_svc_DamaAdaptorLoadAntennaDB (void);
HERROR pdb_svc_DamaAdaptorUpdateSvcInfo (const DbSvc_Info_t *pstInfo);
HERROR pdb_svc_DamaAdaptorRegisterNotifier (HBOOL bReg);

HBOOL pdb_svc_Match (const dbSvc_Service_t *pstSvc, const dbSvc_ServiceFilter_t *pstFilter);


/*	DB Command
 *
 */
void pdb_cmd_Register (void);



#endif /* !__PDB_SVC_H__ */
