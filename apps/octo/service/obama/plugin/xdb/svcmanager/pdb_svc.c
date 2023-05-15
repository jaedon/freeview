/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pdb_svc.c
	@brief

	Description: 									 \n
	Module:
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>

#include <db_svc.h>
#include <pdb_svc.h>

#include "./local_include/_xdb_svc.h"

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
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR pdb_svc_Init(void)
{
	HERROR	hErr = ERR_OK;

	return hErr;
}

HBOOL pdb_svc_HasChanged (void)
{
	return xdb_svc_HasChanged_Base();
}

HBOOL pdb_svc_IsAntChanged (void)
{
	return xdb_svc_IsAntChanged_Base();
}

HERROR pdb_svc_ServiceSetUserFlag(DbSvc_Info_t *pstSvcInfo, const DbSvc_UserFlag1_e bwFlag)
{
	return xdb_svc_ServiceSetUserFlag(pstSvcInfo, bwFlag);
}

HERROR pdb_svc_ServiceGetUserFlag(DbSvc_UserFlag1_b *pbwFlag, const DbSvc_Info_t *pstSvcInfo)
{
	return xdb_svc_ServiceGetUserFlag(pbwFlag, pstSvcInfo);
}

HERROR pdb_svc_ServiceCopyUserFlag(DbSvc_Info_t *pstDest, const DbSvc_Info_t *pstSource)
{
	return xdb_svc_ServiceCopyUserFlag(pstDest, pstSource);
}

HERROR pdb_svc_ServiceGetDefaultAuthority(const DbSvc_Info_t *pstSvcInfo, HUINT8 **ppucDefaultAuthority)
{
	return xdb_svc_ServiceGetDefaultAuthority(pstSvcInfo, ppucDefaultAuthority);
}

HERROR pdb_svc_NetworkGetDefaultAuthority(const DbSvc_NetInfo_t *pstNetInfo, HUINT8 **ppucDefaultAuthority)
{
	return xdb_svc_NetworkGetDefaultAuthority(pstNetInfo, ppucDefaultAuthority);
}

HERROR pdb_svc_TsGetDefaultAuthority(const DbSvc_TsInfo_t *pstTsInfo, HUINT8 **ppucDefaultAuthority)
{
	return xdb_svc_TsGetDefaultAuthority(pstTsInfo, ppucDefaultAuthority);
}

HERROR pdb_svc_ServiceSetDefaultAuthority(DbSvc_Info_t *pstSvcInfo, const HUINT8 *pucDefaultAuthority)
{
	return xdb_svc_ServiceSetDefaultAuthority(pstSvcInfo, pucDefaultAuthority);
}

HERROR pdb_svc_NetworkSetDefaultAuthority(DbSvc_NetInfo_t *pstNetInfo, const HUINT8 *pucDefaultAuthority)
{
	return xdb_svc_NetworkSetDefaultAuthority(pstNetInfo, pucDefaultAuthority);
}

HERROR pdb_svc_TsSetDefaultAuthority(DbSvc_TsInfo_t *pstTsInfo, const HUINT8 *pucDefaultAuthority)
{
	return xdb_svc_TsSetDefaultAuthority(pstTsInfo, pucDefaultAuthority);
}

HERROR pdb_svc_ServiceResetDefaultAuthority(DbSvc_Info_t *pstSvcInfo)
{
	return xdb_svc_ServiceResetDefaultAuthority(pstSvcInfo);
}

HERROR pdb_svc_NetworkResetDefaultAuthority(DbSvc_NetInfo_t *pstNetworkInfo)
{
	return xdb_svc_NetworkResetDefaultAuthority(pstNetworkInfo);
}

HERROR pdb_svc_TsResetDefaultAuthority(DbSvc_TsInfo_t *pstTsInfo)
{
	return xdb_svc_TsResetDefaultAuthority(pstTsInfo);
}

HERROR pdb_svc_ServiceGetFTAContentMgr(const DbSvc_Info_t *pstSvcInfo, HUINT32 *pncFTAContentMgr)
{
	return xdb_svc_ServiceGetFTAContentMgr(pstSvcInfo, pncFTAContentMgr);
}

HERROR pdb_svc_NetworkGetFTAContentMgr(const DbSvc_NetInfo_t *pstNetInfo, HUINT32 *pncFTAContentMgr)
{
	return xdb_svc_NetworkGetFTAContentMgr(pstNetInfo, pncFTAContentMgr);
}

HERROR pdb_svc_TsGetFTAContentMgr(const DbSvc_TsInfo_t *pstTsInfo, HUINT32 *pncFTAContentMgr)
{
	return xdb_svc_TsGetFTAContentMgr(pstTsInfo, pncFTAContentMgr);
}
HERROR pdb_svc_ServiceSetFTAContentMgr(DbSvc_Info_t *pstSvcInfo, const HUINT32 nFTAContentMgr)
{
	return xdb_svc_ServiceSetFTAContentMgr(pstSvcInfo, nFTAContentMgr);
}

HERROR pdb_svc_NetworkSetFTAContentMgr(DbSvc_NetInfo_t *pstNetInfo, const HUINT32 nFTAContentMgr)
{
	return xdb_svc_NetworkSetFTAContentMgr(pstNetInfo, nFTAContentMgr);
}

HERROR pdb_svc_TsSetFTAContentMgr(DbSvc_TsInfo_t *pstTsInfo, const HUINT32 nFTAContentMgr)
{
	return xdb_svc_TsSetFTAContentMgr(pstTsInfo, nFTAContentMgr);
}

HERROR pdb_svc_ServiceGetGuidanceType(const DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceType)
{
	return xdb_svc_ServiceGetGuidanceType(pstSvcInfo, pucGuidanceType);
}

HERROR pdb_svc_ServiceSetGuidanceType(DbSvc_Info_t *pstSvcInfo, HUINT8 ucGuidanceType)
{
	return xdb_svc_ServiceSetGuidanceType(pstSvcInfo, ucGuidanceType);
}

HERROR pdb_svc_ServiceGetGuidanceMode(const DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceMode)
{
	return xdb_svc_ServiceGetGuidanceMode(pstSvcInfo, pucGuidanceMode);
}
HERROR pdb_svc_ServiceSetGuidanceMode(DbSvc_Info_t *pstSvcInfo, HUINT8 ucGuidanceMode)
{
	return xdb_svc_ServiceSetGuidanceMode(pstSvcInfo, ucGuidanceMode);
}
HERROR pdb_svc_ServiceGetGuidanceStr(const DbSvc_Info_t *pstSvcInfo, HUINT8 **ppucGuidanceStr)
{
	return xdb_svc_ServiceGetGuidanceStr(pstSvcInfo, ppucGuidanceStr);
}
HERROR pdb_svc_ServiceSetGuidanceStr(DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceStr)
{
	return xdb_svc_ServiceSetGuidanceStr(pstSvcInfo, pucGuidanceStr);
}
HERROR pdb_svc_ServiceGetVirtualSvcId(const DbSvc_Info_t *pstSvcInfo, HUINT16 *pusVirtualSvcId)
{
	return xdb_svc_ServiceGetVirtualSvcId(pstSvcInfo, pusVirtualSvcId);
}
HERROR pdb_svc_ServiceGetNewSvc(const DbSvc_Info_t *pstSvcInfo, HBOOL *pbNewSvc)
{
	return xdb_svc_ServiceGetNewSvc(pstSvcInfo, pbNewSvc);
}
HERROR pdb_svc_ServiceSetVirtualSvcId(DbSvc_Info_t *pstSvcInfo, HUINT16 usVirtualSvcId)
{
	return xdb_svc_ServiceSetVirtualSvcId(pstSvcInfo, usVirtualSvcId);
}
HERROR pdb_svc_ServiceSetNewSvc(DbSvc_Info_t *pstSvcInfo, HBOOL bNewSvc)
{
	return xdb_svc_ServiceSetNewSvc(pstSvcInfo, bNewSvc);
}
HERROR pdb_svc_NetworkGetUriPrefix(const DbSvc_NetInfo_t *pstNetworkInfo, HUINT8 **ppucUriPrefix)
{
	return xdb_svc_NetworkGetUriPrefix(pstNetworkInfo, ppucUriPrefix);
}
HERROR pdb_svc_NetworkSetUriPrefix(DbSvc_NetInfo_t *pstNetworkInfo, const HUINT8 *pucUriPrefix)
{
	return xdb_svc_NetworkSetUriPrefix(pstNetworkInfo, pucUriPrefix);
}


HERROR pdb_svc_DamaAdaptorRegisterNotifier(HBOOL bReg)
{
	return xdb_svc_DamaAdaptorRegisterNotifier_Base(bReg);
}

HERROR pdb_svc_DamaAdaptorInitAntennaDB (void)
{
	return xdb_svc_DamaAdaptorInitAntennaDB_Base();
}

HERROR pdb_svc_DamaAdaptorInitServiceDB(void)
{
	xdb_svc_DamaAdaptorRegisterNotifier_Base(TRUE);
	return xdb_svc_DamaAdaptorInitServiceDB_Base();
}
HERROR pdb_svc_DamaAdaptorInitTransponderDB(void)
{
	return xdb_svc_DamaAdaptorInitTransponderDB_Base();
}
HERROR pdb_svc_DamaAdaptorInitNetworkDB(void)
{
	return xdb_svc_DamaAdaptorInitNetworkDB_Base();
}
HERROR pdb_svc_DamaAdaptorInitGroupDB(void)
{
	return xdb_svc_DamaAdaptorInitGroupDB_Base();
}
HERROR pdb_svc_DamaAdaptorInitProviderDB(void)
{
	return xdb_svc_DamaAdaptorInitProviderDB_Base();
}
HERROR pdb_svc_DamaAdaptorInitBouquetDB(void)
{
	return xdb_svc_DamaAdaptorInitBouquetDB_Base();
}


HERROR pdb_svc_DamaAdaptorUpdateSvcInfo (const DbSvc_Info_t *pstInfo)
{
	return xdb_svc_DamaAdaptorUpdateSvcInfo_Base(pstInfo);
}

HERROR pdb_svc_DamaAdaptorLoadAntennaDB (void)
{
	return xdb_svc_DamaAdaptorLoadAntennaDB_Base();
}

HERROR pdb_svc_DamaAdaptorSyncAntennaDB (void)
{
	return xdb_svc_DamaAdaptorSyncAntennaDB_Base();
}

HBOOL pdb_svc_Match(const dbSvc_Service_t *pstSvc, const dbSvc_ServiceFilter_t *pstFilter)
{
	return xdb_svc_Match_Base (pstSvc, pstFilter);
}

