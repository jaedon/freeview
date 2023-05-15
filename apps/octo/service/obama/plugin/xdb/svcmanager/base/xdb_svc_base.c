/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xdb_svc_base.c
	@brief

	Description: main main procedure 관련 내용들을 분류한 모듈이다. \n
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
#include <_db_svc.h>
#include <xdb_svc.h>

#include <pdb_svc.h>

#include <otl.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#if 0
// exception handling...
#define	__THROW_LOG(expr,log)	\
	do {\
		if (!(expr)) {\
			HxLOG_Debug("[%s] failed to operation(%s). %s\n", __FUNCTION__, #expr, log);\
			goto __CATCH;\
		}\
	} while (0)
#undef	__THROW
#define	__THROW(expr)	__THROW_LOG(expr,"")
#define	__TRY
#define	__RETURN_IF(expr,err)	\
	do {\
		if (expr) {\
			HxLOG_Debug("[%s] failed to operation(%s).\n", __FUNCTION__, #expr);\
			return (err);\
		}\
	} while (0)

#define __RETURN_IF_VOID(expr)	\
	do {\
		if (expr) {\
			HxLOG_Debug("[%s] failed to operation(%s).\n", __FUNCTION__, #expr);\
			return;\
		}\
	} while (0)

#define C_RETURN_IF(expr,err)	\
	do {\
		if (expr) {\
			HxLOG_Critical("[%s] failed to operation(%s).\n", __FUNCTION__, #expr);\
			return (err);\
		}\
	} while (0)
#define C_RETURN_IF_VOID(expr)	\
	do {\
		if (expr) {\
			HxLOG_Critical("[%s] failed to operation(%s).\n", __FUNCTION__, #expr);\
			return;\
		}\
	} while (0)
#endif

/*******************************************************************/
/********************	   Typedef		   *************************/
/*******************************************************************/

/*******************************************************************/
/********************	   Static Variables 	********************/
/*******************************************************************/

/*******************************************************************/
/********************	   Functions	   *************************/
/*******************************************************************/

HBOOL xdb_svc_Match_Base (const dbSvc_Service_t *pstSvc, const dbSvc_ServiceFilter_t *pstFilter)
{
	const DbSvc_Info_t *pstData;

	if (pstSvc == NULL || pstFilter == NULL)
		return FALSE;

	pstData = &pstSvc->stData;

	if (pstFilter->eSvcType != eDxSVC_TYPE_All)
	{
		if (!(pstFilter->eSvcType & DbSvc_GetSvcType(pstData)))
			return FALSE;
	}
	if (pstFilter->eVideoType != eDxVIDEO_TYPE_ALL)
	{
		if ((pstFilter->eVideoType != DbSvc_GetVideoType(pstData)))
			return FALSE;
	}
	if (pstFilter->eSatType != eSatType_ALL)
	{
		if ((pstFilter->eSatType != DbSvc_GetSatType(pstData)))
			return FALSE;
	}
	if (pstFilter->eCasType != eDxCAS_TYPE_All)
	{
		if (!(pstFilter->eCasType & DbSvc_GetCasType(pstData)))
			return FALSE;
	}
	if (pstFilter->eOrgDeliType != eDxDELIVERY_TYPE_ALL)
	{
		if (!(pstFilter->eOrgDeliType & DbSvc_GetDeliTypePtr(pstData)))
			return FALSE;
	}
	if (pstFilter->eSection != eDxSvcSection_All)
	{
		if (!(pstFilter->eSection & DbSvc_GetSvcSection(pstData)))
			return FALSE;
	}
	if (pstFilter->eSvcOpType != eDxOPERATOR_All)
	{
		if ((pstFilter->eSvcOpType != DbSvc_GetOpType(pstData)))
			return FALSE;
	}
	if (pstFilter->nAntIdx >= 0)
	{
		if (pstFilter->nAntIdx != (HINT32)DbSvc_GetAntIdx(pstData))
			return FALSE;
	}
	if (pstFilter->nOnId >= 0)
	{
		if (pstFilter->nOnId != (HINT32)DbSvc_GetOnId(pstData))
			return FALSE;
	}
	if (pstFilter->nTsId >= 0)
	{
		if (pstFilter->nTsId != (HINT32)DbSvc_GetTsId(pstData))
			return FALSE;
	}
	if (pstFilter->nSvcId >= 0)
	{
		if (pstFilter->nSvcId != (HINT32)DbSvc_GetSvcId(pstData))
			return FALSE;
	}
	if (pstFilter->nTsIdx >= 0)
	{
		if (pstFilter->nTsIdx != (HINT32)DbSvc_GetTsIdx(pstData))
			return FALSE;
	}
	if (pstFilter->nPrvIdx >= 0)
	{
		if (pstFilter->nPrvIdx != (HINT32)DbSvc_GetPrvIdx(pstData))
			return FALSE;
	}
	if (pstFilter->nLcnFlag >= 0)
	{
		if (pstFilter->nLcnFlag != (HINT32)DbSvc_GetLcnFlag(pstData))
			return FALSE;
	}
	if (pstFilter->nStartLcn >= 0 || pstFilter->nEndLcn >= 0)
	{
		if (pstFilter->nStartLcn >= 0 && (HINT32)DbSvc_GetLcn(pstData) < pstFilter->nStartLcn)
			return FALSE;
		if (  pstFilter->nEndLcn >= 0 && (HINT32)DbSvc_GetLcn(pstData) > pstFilter->nEndLcn)
			return FALSE;
	}
	if (pstFilter->bNewSvc)
	{
#ifdef CONFIG_APS
		if (!XdbSvc_SesGetNewSvcPtr(pstData))
			return FALSE;
#endif
	}
	if (pstFilter->bwUserMask1 != eSvcUserFlag_All)
	{
		DbSvc_UserFlag1_e bwflag = pstFilter->bwUserFlag1 & pstFilter->bwUserMask1;
		DbSvc_UserFlag1_e bwSvcFlag = 0;

//		DbSvc_GetUserFlagPtr(bwSvcFlag, pstData);
//		DB_SVC_ServiceGetUserFlag((DbSvc_UserFlag1_b *)&bwSvcFlag, (const DbSvc_Info_t *)pstData);

		(void)DB_SVC_ServiceGetUserFlag((DbSvc_UserFlag1_b *)&bwSvcFlag, (const DbSvc_Info_t *)pstData);

		if ((bwSvcFlag & pstFilter->bwUserMask1) != bwflag)
			return FALSE;
	}

#if defined(CONFIG_DB_SVC_SVCGROUP)
	if (pstFilter->nGenreIdx > 0)
	{
		HBOOL		 bFoundGroup = FALSE;
		HUINT32		 ulGroupIdx;

		for (ulGroupIdx = 0; ulGroupIdx < DxGROUP_LEN; ulGroupIdx++)
		{
			if (DbSvc_GetGroupId(pstData, ulGroupIdx) == pstFilter->nGenreIdx)
			{
				bFoundGroup = TRUE;
				break;
			}
		}

		if (TRUE != bFoundGroup)
		{
			return FALSE;
		}
	}
#endif

	// TODO: DbSvc_SortType_e 		eSortType;
	// TODO: DbSvc_VisibleType_e		eVisibleType;
	// TODO: HINT32 				nBouqIdx;
	// TODO: HUINT8 				ucAlphabetIdx;

	return TRUE;
}

HBOOL xdb_svc_HasChanged_Base (void)
{
	HBOOL	bCheckCount = FALSE;

	return db_svc_MgrHasChanged(bCheckCount
		, 6
		, eSvcTable_Service
		, eSvcTable_Transponder
		, eSvcTable_Bouquet
		, eSvcTable_Group
		, eSvcTable_Network
		, eSvcTable_Provider
	);
}

HBOOL xdb_svc_IsAntChanged_Base (void)
{
	HBOOL	bCheckCount = FALSE;

	return db_svc_MgrHasChanged(bCheckCount, 1, eSvcTable_Antenna);
}

HERROR	xdb_svc_ServiceSetUserFlag(DbSvc_Info_t *pstSvcInfo, const DbSvc_UserFlag1_b bwFlag)
{
	if (bwFlag & eSvcUserFlag_01_FsatUserCustom)		XdbSvc_SetCustomizableFlagPtr(pstSvcInfo, TRUE);
	if (bwFlag & eSvcUserFlag_02_Selectable)			XdbSvc_SetSelectableFlagPtr(pstSvcInfo, TRUE);
	if (bwFlag & eSvcUserFlag_03_SvcDeletedByUser)		DbSvc_SetSvcRemoved(pstSvcInfo, TRUE);
	if (bwFlag & eSvcUserFlag_09_AdultSvc)			XdbSvc_SetAdultSvcFlagPtr(pstSvcInfo, TRUE);
	if (bwFlag & eSvcUserFlag_11_FreeCiMode)			DbSvc_SetFreeCiModeFlag(pstSvcInfo, TRUE);
	if (bwFlag & eSvcUserFlag_13_VolatileSvc)			DbSvc_SetVolatileSvcFlag(pstSvcInfo, TRUE);
	if (bwFlag & eSvcUserFlag_20_ParentalControl)		DbSvc_SetParentalControlFlag(pstSvcInfo, TRUE);
	if (bwFlag & eSvcUserFlag_27_VideoCodecH264)		XdbSvc_SetVidCodecH264FlagPtr(pstSvcInfo, TRUE);
	if (bwFlag & eSvcUserFlag_31_SvcConflictLcn)		DbSvc_SetSvcConflictFlag(pstSvcInfo, TRUE);
	if (bwFlag & eSvcUserFlag_32_NewService)			DbSvc_SetSvcUpdated(pstSvcInfo, TRUE);

	return ERR_OK;
}

HERROR	xdb_svc_ServiceGetUserFlag(DbSvc_UserFlag1_b *pbwFlag, const DbSvc_Info_t *pstSvcInfo)
{
	HUINT32			ulFlag = 0;

	if (XdbSvc_GetCustomizableFlagPtr(pstSvcInfo))		ulFlag |= eSvcUserFlag_01_FsatUserCustom;
	if (XdbSvc_GetSelectableFlagPtr(pstSvcInfo)) 			ulFlag |= eSvcUserFlag_02_Selectable;
	if (DbSvc_GetSvcRemoved(pstSvcInfo)) 			ulFlag |= eSvcUserFlag_03_SvcDeletedByUser;
	if (XdbSvc_GetAdultSvcFlagPtr(pstSvcInfo))			ulFlag |= eSvcUserFlag_09_AdultSvc;
	if (DbSvc_GetFreeCiModeFlag(pstSvcInfo))	 			ulFlag |= eSvcUserFlag_11_FreeCiMode;
	if (DbSvc_GetVolatileSvcFlag(pstSvcInfo))				ulFlag |= eSvcUserFlag_13_VolatileSvc;
	if (DbSvc_GetParentalControlFlag(pstSvcInfo))			ulFlag |= eSvcUserFlag_20_ParentalControl;
	if (XdbSvc_GetVidCodecH264FlagPtr(pstSvcInfo))		ulFlag |= eSvcUserFlag_27_VideoCodecH264;
	if (DbSvc_GetSvcConflictFlag(pstSvcInfo)) 				ulFlag |= eSvcUserFlag_31_SvcConflictLcn;
	if (DbSvc_GetSvcUpdated(pstSvcInfo)) 				ulFlag |= eSvcUserFlag_32_NewService;

	*pbwFlag = ulFlag;

	return ERR_OK;
}

HERROR	xdb_svc_ServiceCopyUserFlag(DbSvc_Info_t *pstDest, const DbSvc_Info_t *pstSource)
{
	XdbSvc_SetCustomizableFlagPtr(pstDest, XdbSvc_GetCustomizableFlagPtr(pstSource));
	XdbSvc_SetSelectableFlagPtr(pstDest, XdbSvc_GetSelectableFlagPtr(pstSource));
	DbSvc_SetSvcRemoved(pstDest, DbSvc_GetSvcRemoved(pstSource));
	DbSvc_SetSvcConflictFlag(pstDest, DbSvc_GetSvcConflictFlag(pstSource));
	XdbSvc_SetAdultSvcFlagPtr(pstDest, XdbSvc_GetAdultSvcFlagPtr(pstSource));
	DbSvc_SetFreeCiModeFlag(pstDest, DbSvc_GetFreeCiModeFlag(pstSource));
	DbSvc_SetVolatileSvcFlag(pstDest, DbSvc_GetVolatileSvcFlag(pstSource));
	DbSvc_SetParentalControlFlag(pstDest, DbSvc_GetParentalControlFlag(pstSource));
	XdbSvc_SetVidCodecH264FlagPtr(pstDest, XdbSvc_GetVidCodecH264FlagPtr(pstSource));
	DbSvc_SetSvcUpdated(pstDest, DbSvc_GetSvcUpdated(pstSource));
	pstDest->eUserFlag1 = pstSource->eUserFlag1;

	return ERR_OK;
}

HERROR xdb_svc_ServiceGetDefaultAuthority(const DbSvc_Info_t *pstSvcInfo, HUINT8 **ppucDefaultAuthority)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	*ppucDefaultAuthority = (HUINT8 *)XdbSvc_TvAnyTimeGetDefaultAuthority(pstSvcInfo);
	hErr = ERR_OK;
#elif defined(CONFIG_OP_FREESAT)
	*ppucDefaultAuthority = (HUINT8 *)XdbSvc_FreeSatGetDefaultAuthorityPtr(pstSvcInfo);
	hErr = ERR_OK;
#else
	*ppucDefaultAuthority = NULL;
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_NetworkGetDefaultAuthority(const DbSvc_NetInfo_t *pstNetworkInfo, HUINT8 **ppucDefaultAuthority)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	*ppucDefaultAuthority = (HUINT8 *)pstNetworkInfo->utExt.stTvAnyTime.aucDefaultAuthority;
	hErr = ERR_OK;
#else
	*ppucDefaultAuthority = NULL;
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_TsGetDefaultAuthority(const DbSvc_TsInfo_t *pstTsInfo, HUINT8 **ppucDefaultAuthority)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	*ppucDefaultAuthority = (HUINT8 *)pstTsInfo->utExt.stTvAnyTime.aucDefaultAuthority;
	hErr = ERR_OK;
#else
	*ppucDefaultAuthority = NULL;
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_ServiceSetDefaultAuthority(DbSvc_Info_t *pstSvcInfo, const HUINT8 *pucDefaultAuthority)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	XdbSvc_TvAnyTimeSetDefaultAuthority(pstSvcInfo, pucDefaultAuthority);
	hErr = ERR_OK;
#elif defined(CONFIG_OP_FREESAT)
	XdbSvc_FreeSatSetDefaultAuthorityPtr(pstSvcInfo, pucDefaultAuthority);
	hErr = ERR_OK;
#else
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_NetworkSetDefaultAuthority(DbSvc_NetInfo_t *pstNetworkInfo, const HUINT8 *pucDefaultAuthority)
{
	HERROR		hErr;

	if (NULL == pstNetworkInfo)
		return ERR_FAIL;

	if (NULL == pucDefaultAuthority)
		return ERR_FAIL;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HLIB_STD_StrNCpySafe(pstNetworkInfo->utExt.stTvAnyTime.aucDefaultAuthority, pucDefaultAuthority, DxDEFAULT_AUTHORITY_LEN);
	hErr = ERR_OK;
#else
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_TsSetDefaultAuthority(DbSvc_TsInfo_t *pstTsInfo, const HUINT8 *pucDefaultAuthority)
{
	HERROR		hErr;

	if (NULL == pstTsInfo)
		return ERR_FAIL;

	if (NULL == pucDefaultAuthority)
		return ERR_FAIL;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HLIB_STD_StrNCpySafe(pstTsInfo->utExt.stTvAnyTime.aucDefaultAuthority, pucDefaultAuthority, DxDEFAULT_AUTHORITY_LEN);
	hErr = ERR_OK;
#else
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_ServiceResetDefaultAuthority(DbSvc_Info_t *pstSvcInfo)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HxSTD_MemSet(XdbSvc_TvAnyTimeGetDefaultAuthority(pstSvcInfo), 0x00, DxDEFAULT_AUTHORITY_LEN);
	hErr = ERR_OK;
#elif defined(CONFIG_OP_FREESAT)
	HxSTD_MemSet(XdbSvc_FreeSatGetDefaultAuthorityPtr(pstSvcInfo), 0x00, DxDEFAULT_AUTHORITY_LEN);
	hErr = ERR_OK;
#else
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_NetworkResetDefaultAuthority(DbSvc_NetInfo_t *pstNetworkInfo)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HxSTD_MemSet(pstNetworkInfo->utExt.stTvAnyTime.aucDefaultAuthority, 0x00, DxDEFAULT_AUTHORITY_LEN);
	hErr = ERR_OK;
#else
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_TsResetDefaultAuthority(DbSvc_TsInfo_t *pstTsInfo)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HxSTD_MemSet(pstTsInfo->utExt.stTvAnyTime.aucDefaultAuthority, 0x00, DxDEFAULT_AUTHORITY_LEN);
	hErr = ERR_OK;
#else
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_ServiceGetFTAContentMgr(const DbSvc_Info_t *pstSvcInfo, HUINT32 *pnFTAContentMgr)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	*pnFTAContentMgr = XdbSvc_TvAnyTimeGetContentMgr(pstSvcInfo);
	hErr = ERR_OK;
#else
	*pnFTAContentMgr = 0;
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_NetworkGetFTAContentMgr(const DbSvc_NetInfo_t *pstNetworkInfo, HUINT32 *pnFTAContentMgr)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	*pnFTAContentMgr = pstNetworkInfo->utExt.stTvAnyTime.ulFTAContentMgr;
	hErr = ERR_OK;
#else
	*pnFTAContentMgr = 0;
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_TsGetFTAContentMgr(const DbSvc_TsInfo_t *pstTsInfo, HUINT32 *pnFTAContentMgr)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	*pnFTAContentMgr = pstTsInfo->utExt.stTvAnyTime.ulFTAContentMgr;
	hErr = ERR_OK;
#else
	*pnFTAContentMgr = 0;
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_ServiceSetFTAContentMgr(DbSvc_Info_t *pstSvcInfo, const HUINT32 ulFTAContentMgr)
{
	HERROR		hErr;


#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	XdbSvc_TvAnyTimeSetContentMgr(pstSvcInfo, ulFTAContentMgr);
	hErr = ERR_OK;
#else

	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_NetworkSetFTAContentMgr(DbSvc_NetInfo_t *pstNetworkInfo, const HUINT32 ulFTAContentMgr)
{
	HERROR		hErr;

	if (NULL == pstNetworkInfo)
		return ERR_FAIL;


#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	pstNetworkInfo->utExt.stTvAnyTime.ulFTAContentMgr = ulFTAContentMgr;
	hErr = ERR_OK;
#else
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_TsSetFTAContentMgr(DbSvc_TsInfo_t *pstTsInfo, const HUINT32 ulFTAContentMgr)
{
	HERROR		hErr;

	if (NULL == pstTsInfo)
		return ERR_FAIL;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	pstTsInfo->utExt.stTvAnyTime.ulFTAContentMgr = ulFTAContentMgr;
	hErr = ERR_OK;
#else
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR	xdb_svc_ServiceGetGuidanceType(const DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceType)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	*pucGuidanceType = XdbSvc_TvAnyTimeGetGuidanceType(pstSvcInfo);
	hErr = ERR_OK;
#else
	*pucGuidanceType = 0;
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR	xdb_svc_ServiceSetGuidanceType(DbSvc_Info_t *pstSvcInfo, HUINT8 ucGuidanceType)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	XdbSvc_TvAnyTimeSetGuidanceType(pstSvcInfo, ucGuidanceType);
	hErr = ERR_OK;
#else
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_ServiceGetGuidanceMode(const DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceMode)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	*pucGuidanceMode = XdbSvc_TvAnyTimeGetGuidanceMode(pstSvcInfo);
	hErr = ERR_OK;
#else
	*pucGuidanceMode = 0;
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_ServiceSetGuidanceMode(DbSvc_Info_t *pstSvcInfo, HUINT8 ucGuidanceMode)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	XdbSvc_TvAnyTimeSetGuidanceMode(pstSvcInfo, ucGuidanceMode);
	hErr = ERR_OK;
#else
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_ServiceGetGuidanceStr(const DbSvc_Info_t *pstSvcInfo, HUINT8 **ppucGuidanceStr)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	*ppucGuidanceStr = (HUINT8 *)XdbSvc_TvAnyTimeGetGuidanceStr(pstSvcInfo);
	hErr = ERR_OK;
#else
	*ppucGuidanceStr = NULL;
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_ServiceSetGuidanceStr( DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceStr)
{
	HERROR		hErr;
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	XdbSvc_TvAnyTimeSetGuidanceStr(pstSvcInfo, pucGuidanceStr);
	hErr = ERR_OK;
#else
	hErr = ERR_FAIL;
#endif
	return hErr;
}

HERROR xdb_svc_ServiceGetVirtualSvcId(const DbSvc_Info_t *pstSvcInfo, HUINT16 *pusVirtualSvcId)
{
	HERROR		hErr;

#if defined(CONFIG_OP_SES)
	*pusVirtualSvcId = XdbSvc_SesGetVirtualSvcIdPtr(pstSvcInfo);
	hErr = ERR_OK;
#else
	*pusVirtualSvcId = 0;
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_ServiceGetNewSvc(const DbSvc_Info_t *pstSvcInfo, HBOOL *pbNewSvc)
{
	HERROR		hErr;

#if defined(CONFIG_OP_SES)
	*pbNewSvc = XdbSvc_SesGetNewSvcPtr(pstSvcInfo);
	hErr = ERR_OK;
#else
	*pbNewSvc = FALSE;
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_ServiceSetVirtualSvcId(DbSvc_Info_t *pstSvcInfo, HUINT16 usVirtualSvcId)
{
	HERROR		hErr;

#if defined(CONFIG_OP_SES)
	XdbSvc_SesSetVirtualSvcIdPtr(pstSvcInfo, usVirtualSvcId);
	hErr = ERR_OK;
#else
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_ServiceSetNewSvc(DbSvc_Info_t *pstSvcInfo, HBOOL bNewSvc)
{
	HERROR		hErr;

#if defined(CONFIG_OP_SES)
	XdbSvc_SesSetNewSvcPtr(pstSvcInfo, bNewSvc);
	hErr = ERR_OK;
#else
	hErr = ERR_FAIL;
#endif

	return hErr;
}

HERROR xdb_svc_NetworkGetUriPrefix(const DbSvc_NetInfo_t *pstNetworkInfo, HUINT8 **ppucUriPrefix)
{
	HERROR		hErr;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	*ppucUriPrefix = (HUINT8 *)pstNetworkInfo->utExt.stTvAnyTime.stUriPrefix.aucUriPrefix;
	hErr = ERR_OK;
#else
	*ppucUriPrefix = NULL;
	hErr = ERR_FAIL;
#endif

	return hErr;
}
HERROR xdb_svc_NetworkSetUriPrefix(DbSvc_NetInfo_t *pstNetworkInfo, const HUINT8 *pucUriPrefix)
{
	HERROR		hErr;

	if (NULL == pstNetworkInfo)
		return ERR_FAIL;

	if (NULL == pucUriPrefix)
		return ERR_FAIL;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HLIB_STD_StrNCpySafe(pstNetworkInfo->utExt.stTvAnyTime.stUriPrefix.aucUriPrefix, pucUriPrefix, DxMAX_URI_PREFIX_INFO_LEN);
	hErr = ERR_OK;
#else
	hErr = ERR_FAIL;
#endif

	return hErr;
}



