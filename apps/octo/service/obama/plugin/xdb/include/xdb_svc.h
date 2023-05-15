/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xdb_svc.h
	@brief	  freesat 을 위한 DB 과련 header file.

	Description: 		\n
	Module: 		 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__XDB_SVC_H__
#define	__XDB_SVC_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
/* 다음 검색 조건을 여기 저기서 사용하는 것보다, 아래와 같이 공통 macro처리 해두면, Japan Network 관련 추가 작업 시 편리함 */
#define MACRO_IS_JAPAN_DT_NETWORK(usNetId)		( (usNetId & 0xF000) == 0x7000 )

#if defined(DbSvc_SUPPORT_DXINFO)
/* FreeSat */
// FreeSat : Default Authority
#define XdbSvc_FreeSatGetDefaultAuthority(stSvcInfo)										stSvcInfo.stDxInfo.ex.freesat.defaultAuth
#define XdbSvc_FreeSatGetDefaultAuthorityPtr(pstSvcInfo)									pstSvcInfo->stDxInfo.ex.freesat.defaultAuth
#define XdbSvc_FreeSatSetDefaultAuthority(stSvcInfo, aucAuthority)							HLIB_STD_StrNCpySafe(stSvcInfo.stDxInfo.ex.freesat.defaultAuth, aucAuthority, sizeof(HUINT8)*DxDEFAULT_AUTHORITY_LEN)
#define XdbSvc_FreeSatSetDefaultAuthorityPtr(pstSvcInfo, aucAuthority)						HLIB_STD_StrNCpySafe(pstSvcInfo->stDxInfo.ex.freesat.defaultAuth, aucAuthority, sizeof(HUINT8)*DxDEFAULT_AUTHORITY_LEN)

// FreeSat : Guidance Type
#define XdbSvc_FreeSatGetGuidanceType(stSvcInfo)											stSvcInfo.stDxInfo.ex.freesat.ucGuidanceType
#define XdbSvc_FreeSatGetGuidanceTypePtr(pstSvcInfo)										pstSvcInfo->stDxInfo.ex.freesat.ucGuidanceType
#define XdbSvc_FreeSatSetGuidanceType(stSvcInfo, ucType)									stSvcInfo.stDxInfo.ex.freesat.ucGuidanceType = (HUINT8)ucType
#define XdbSvc_FreeSatSetGuidanceTypePtr(pstSvcInfo, ucType)								pstSvcInfo->stDxInfo.ex.freesat.ucGuidanceType = (HUINT8)ucType

// FreeSat : Guidance Mode
#define XdbSvc_FreeSatGetGuidanceMode(stSvcInfo)											stSvcInfo.stDxInfo.ex.freesat.ucGuidanceMode
#define XdbSvc_FreeSatGetGuidanceModePtr(pstSvcInfo)										pstSvcInfo->stDxInfo.ex.freesat.ucGuidanceMode
#define XdbSvc_FreeSatSetGuidanceMode(stSvcInfo, ucMode)									stSvcInfo.stDxInfo.ex.freesat.ucGuidanceMode = (HUINT8)ucMode
#define XdbSvc_FreeSatSetGuidanceModePtr(pstSvcInfo, ucMode)								pstSvcInfo->stDxInfo.ex.freesat.ucGuidanceMode = (HUINT8)ucMode

// FreeSat : Guidance String
#define XdbSvc_FreeSatGetGuidanceStr(stSvcInfo)												stSvcInfo.stDxInfo.ex.freesat.szGuidanceStr
#define XdbSvc_FreeSatGetGuidanceStrPtr(pstSvcInfo)											pstSvcInfo->stDxInfo.ex.freesat.szGuidanceStr
#define XdbSvc_FreeSatSetGuidanceStr(stSvcInfo, szStr)										HLIB_STD_StrNCpySafe(stSvcInfo.stDxInfo.ex.freesat.szGuidanceStr, szStr, sizeof(HUINT8)*DxMAX_GUIDANCE_INFO_LEN)
#define XdbSvc_FreeSatSetGuidanceStrPtr(pstSvcInfo, szStr)									HLIB_STD_StrNCpySafe(pstSvcInfo->stDxInfo.ex.freesat.szGuidanceStr, szStr, sizeof(HUINT8)*DxMAX_GUIDANCE_INFO_LEN)
#define XdbSvc_FreeSatResetGuidanceStr(stSvcInfo)											HxSTD_MemSet(stSvcInfo.stDxInfo.ex.freesat.szGuidanceStr, 0x00, sizeof(HUINT8)*DxMAX_GUIDANCE_INFO_LEN)
#define XdbSvc_FreeSatResetGuidanceStrPtr(pstSvcInfo)										HxSTD_MemSet(pstSvcInfo->stDxInfo.ex.freesat.szGuidanceStr, 0x00, sizeof(HUINT8)*DxMAX_GUIDANCE_INFO_LEN)

// FreeSat : Contents Mgr
#define	XdbSvc_FreeSatGetContentMgr(stSvcInfo)												stSvcInfo.utExt.stFreeSat.ulFTAContentMgr
#define	XdbSvc_FreeSatGetContentMgrPtr(pstSvcInfo)											pstSvcInfo->utExt.stFreeSat.ulFTAContentMgr
#define	XdbSvc_FreeSatSetContentMgr(stSvcInfo, ulContentMgr)								stSvcInfo.utExt.stFreeSat.ulFTAContentMgr = (HUINT32)ulContentMgr
#define	XdbSvc_FreeSatSetContentMgrPtr(pstSvcInfo, ulContentMgr)							pstSvcInfo->utExt.stFreeSat.ulFTAContentMgr = (HUINT32)ulContentMgr

// FreeSat : Svc Identifier
#define	XdbSvc_FreeSatGetSvcIdentifier(stSvc)									stSvc.stDxInfo.ex.freesat.svcId
#define	XdbSvc_FreeSatGetSvcIdentifierPtr(pstSvc)								pstSvc->stDxInfo.ex.freesat.svcId
#define	XdbSvc_FreeSatSetSvcIdentifier(stSvc, usValue)							stSvc.stDxInfo.ex.freesat.svcId = usValue
#define	XdbSvc_FreeSatSetSvcIdentifierPtr(pstSvc, usValue)						pstSvc->stDxInfo.ex.freesat.svcId = usValue

// FreeSat : Sat Region Id
#define	XdbSvc_FreeSatGetSatRegionId(stSvc)										stSvc.stDxInfo.ex.freesat.regionId
#define	XdbSvc_FreeSatGetSatRegionIdPtr(pstSvc)									pstSvc->stDxInfo.ex.freesat.regionId
#define	XdbSvc_FreeSatSetSatRegionId(stSvc, usValue)							stSvc.stDxInfo.ex.freesat.regionId = (HUINT16)usValue
#define	XdbSvc_FreeSatSetSatRegionIdPtr(pstSvc, usValue)						pstSvc->stDxInfo.ex.freesat.regionId = (HUINT16)usValue

// FreeSat : Sat Resent Follow
#define	XdbSvc_FreeSatGetSatPresentFollow(stSvc)								stSvc.stDxInfo.ex.freesat.ucPresentFollow
#define	XdbSvc_FreeSatGetSatPresentFollowPtr(pstSvc)							pstSvc->stDxInfo.ex.freesat.ucPresentFollow
#define	XdbSvc_FreeSatSetSatPresentFollow(stSvc, ucValue)						stSvc.stDxInfo.ex.freesat.ucPresentFollow = ucValue
#define	XdbSvc_FreeSatSetSatPresentFollowPtr(pstSvc, ucValue)					pstSvc->stDxInfo.ex.freesat.ucPresentFollow = ucValue

// FreeSat : Service Short Name
#define	XdbSvc_FreeSatGetSvcShortName(stSvc)									stSvc.stDxInfo.ex.freesat.szShortName
#define	XdbSvc_FreeSatGetSvcShortNamePtr(pstSvc)								pstSvc->stDxInfo.ex.freesat.szShortName

// FreeSat : Ts Info
#define	XdbSvc_FreeSatTsGetFTAContentMgr(stTs)									stTs.utExt.stFreeSat.ulFTAContentMgr
#define	XdbSvc_FreeSatTsGetFTAContentMgrPtr(pstTs)								pstTs->utExt.stFreeSat.ulFTAContentMgr
#define	XdbSvc_FreeSatTsSetFTAContentMgr(stTs, ulValue)							stTs.utExt.stFreeSat.ulFTAContentMgr = ulValue
#define	XdbSvc_FreeSatTsSetFTAContentMgrPtr(pstTs, ulValue)						pstTs->utExt.stFreeSat.ulFTAContentMgr = ulValue

// FreeSat : Network Info
#define	XdbSvc_FreeSatNetGetFTAContentMgr(stNet)								stNet.utExt.stFreeSat.ulFTAContentMgr
#define	XdbSvc_FreeSatNetGetFTAContentMgrPtr(pstNet)							pstNet->utExt.stFreeSat.ulFTAContentMgr
#define	XdbSvc_FreeSatNetSetFTAContentMgr(stNet, ulValue)						stNet.utExt.stFreeSat.ulFTAContentMgr = ulValue
#define	XdbSvc_FreeSatNetSetFTAContentMgrPtr(pstNet, ulValue)					pstNet->utExt.stFreeSat.ulFTAContentMgr = ulValue

// FreeSat Group : Group Info
#define	XdbSvc_FreeSatGroupGetGroupType(stGroup)								stGroup.utExt.stFreeSat.ucGroupType
#define	XdbSvc_FreeSatGroupSetGroupType(stGroup, ucValue)						stGroup.utExt.stFreeSat.ucGroupType = ucValue
#define	XdbSvc_FreeSatGroupGetGroupTypePtr(pstGroup)							pstGroup->utExt.stFreeSat.ucGroupType
#define	XdbSvc_FreeSatGroupSetGroupTypePtr(pstGroup, ucValue)					pstGroup->utExt.stFreeSat.ucGroupType = ucValue

// FreeSat Group : Nondestructive Tune Flag
#define	XdbSvc_FreeSatGroupGetNondestructiveTuneFlag(stGroup)					stGroup.utExt.stFreeSat.ucNondestructiveTuneFlag
#define	XdbSvc_FreeSatGroupSetNondestructiveTuneFlag(stGroup, ucValue)			stGroup.utExt.stFreeSat.ucNondestructiveTuneFlag = ucValue
#define	XdbSvc_FreeSatGroupGetNondestructiveTuneFlagPtr(pstGroup)				pstGroup->utExt.stFreeSat.ucNondestructiveTuneFlag
#define	XdbSvc_FreeSatGroupSetNoneDestructiveTuneFlagPtr(pstGroup, ucValue)		pstGroup->utExt.stFreeSat.ucNondestructiveTuneFlag = ucValue

// FreeSat Group : Return Channel Access Flag
#define	XdbSvc_FreeSatGroupGetReturnChannelAccessFlag(stGroup)					stGroup.utExt.stFreeSat.ucReturnChannelAccessFlag
#define	XdbSvc_FreeSatGroupGetReturnChannelAccessFlagPtr(pstGroup)				pstGroup->utExt.stFreeSat.ucReturnChannelAccessFlag
#define	XdbSvc_FreeSatGroupSetReturnChannelAccessFlag(stGroup, ucValue)			stGroup.utExt.stFreeSat.ucReturnChannelAccessFlag = ucValue
#define	XdbSvc_FreeSatGroupSetReturnChannelAccessFlagPtr(pstGroup, ucValue)		pstGroup->utExt.stFreeSat.ucReturnChannelAccessFlag = ucValue

// FreeSat Group : Iso639 Language Code
#define	XdbSvc_FreeSatDB_SVC_GroupGetIso639LangCode(stGroup)							stGroup.utExt.stFreeSat.szIso639LangCode
#define	XdbSvc_FreeSatGroupGetIso639LangCodePtr(pstGroup)						pstGroup->utExt.stFreeSat.szIso639LangCode

// FreeSat Group : G2Extension Flag
#define	XdbSvc_FreeSatGroupGetG2ExtensionFlag(stGroup)							stGroup.utExt.stFreeSat.ucG2ExtensionFlag
#define	XdbSvc_FreeSatGroupGetG2ExtensionFlagPtr(pstGroup)						pstGroup->utExt.stFreeSat.ucG2ExtensionFlag
#define	XdbSvc_FreeSatGroupSetG2ExtensionFlag(stGroup, ucValue)					stGroup.utExt.stFreeSat.ucG2ExtensionFlag = ucValue
#define	XdbSvc_FreeSatGroupSetG2ExtensionFlagPtr(pstGroup, ucValue)				pstGroup->utExt.stFreeSat.ucG2ExtensionFlag = ucValue

// FreeSat Group : Default Visible Flag
#define	XdbSvc_FreeSatGroupGetDefaultVisibleFlag(stGroup)						stGroup.utExt.stFreeSat.ucDefaultVisableFlag
#define	XdbSvc_FreeSatGroupGetDefaultVisibleFlagPtr(pstGroup)					pstGroup->utExt.stFreeSat.ucDefaultVisableFlag
#define	XdbSvc_FreeSatGroupSetDefaultVisibleFlag(stGroup, ucValue)				stGroup.utExt.stFreeSat.ucDefaultVisableFlag = ucValue
#define	XdbSvc_FreeSatGroupSetDefaultVisibleFlagPtr(pstGroup, ucValue)			pstGroup->utExt.stFreeSat.ucDefaultVisableFlag = ucValue

// FreeSat Group : Adult Channels Group Flag
#define	XdbSvc_FreeSatGroupGetAdultChannelsGroupFlag(stGroup)					stGroup.utExt.stFreeSat.ucAdultChannelsGroupFlag
#define	XdbSvc_FreeSatGroupGetAdultChannelsGroupFlagPtr(pstGroup)				pstGroup->utExt.stFreeSat.ucAdultChannelsGroupFlag
#define	XdbSvc_FreeSatGroupSetAdultChannelsGroupFlag(stGroup, ucValue)			stGroup.utExt.stFreeSat.ucAdultChannelsGroupFlag = ucValue
#define	XdbSvc_FreeSatGroupSetAdultChannelsGroupFlagPtr(pstGroup, ucValue)		pstGroup->utExt.stFreeSat.ucAdultChannelsGroupFlag = ucValue

// FreeSat Group : Ip Channels Groups Flags
#define	XdbSvc_FreeSatGroupGetIpChannelsGroupFlag(stGroup)						stGroup.utExt.stFreeSat.ucIpChannelsGroupFlag
#define	XdbSvc_FreeSatGroupGetIpChannelsGroupFlagPtr(pstGroup)					pstGroup->utExt.stFreeSat.ucIpChannelsGroupFlag
#define	XdbSvc_FreeSatGroupSetIpChannelsGroupFlag(stGroup, ucValue)				stGroup.utExt.stFreeSat.ucIpChannelsGroupFlag = ucValue
#define	XdbSvc_FreeSatGroupSetIpChannelsGroupFlagPtr(pstGroup, ucValue)			pstGroup->utExt.stFreeSat.ucIpChannelsGroupFlag = ucValue

// FreeSat Group : Ca Channels Group Flag
#define	XdbSvc_FreeSatGroupGetCaChannelsGroupFlag(stGroup)						stGroup.utExt.stFreeSat.ucCaChannelsGroupFlag
#define	XdbSvc_FreeSatGroupGetCaChannelsGroupFlagPtr(pstGroup)					pstGroup->utExt.stFreeSat.ucCaChannelsGroupFlag
#define	XdbSvc_FreeSatGroupSetCaChannelsGroupFlag(stGroup, ucValue)				stGroup.utExt.stFreeSat.ucCaChannelsGroupFlag = ucValue
#define	XdbSvc_FreeSatGroupSetCaChannelsGroupFlagPtr(pstGroup, ucValue)			pstGroup->utExt.stFreeSat.ucCaChannelsGroupFlag = ucValue

/* SES */
// SES : Virtual Svc Id
#define XdbSvc_SesGetVirtualSvcId(stSvc)									stSvc.stDxInfo.ex.ses.usVirtualSvcId
#define XdbSvc_SesSetVirtualSvcId(stSvc, usValue)							stSvc.stDxInfo.ex.ses.usVirtualSvcId = usValue
#define XdbSvc_SesGetVirtualSvcIdPtr(pstSvc)								pstSvc->stDxInfo.ex.ses.usVirtualSvcId
#define XdbSvc_SesSetVirtualSvcIdPtr(pstSvc, usValue)						pstSvc->stDxInfo.ex.ses.usVirtualSvcId = usValue

// SES : New Svc
#define XdbSvc_SesGetNewSvc(stSvc)											stSvc.stDxInfo.ex.ses.bNewSvc
#define XdbSvc_SesSetNewSvc(stSvc, bValue)									stSvc.stDxInfo.ex.ses.bNewSvc = bValue
#define XdbSvc_SesGetNewSvcPtr(pstSvc)										pstSvc->stDxInfo.ex.ses.bNewSvc
#define XdbSvc_SesSetNewSvcPtr(pstSvc, bValue)								pstSvc->stDxInfo.ex.ses.bNewSvc = bValue

/* TV-AnyTime */
// TV-AnyTime : Default Authority
#define XdbSvc_TvAnyTimeGetDefaultAuthority(pstSvcInfo)											pstSvcInfo->stDxInfo.ex.tva.szDefaultAuth
#define XdbSvc_TvAnyTimeSetDefaultAuthority(pstSvcInfo, aucAuthority)							HLIB_STD_StrNCpySafe(pstSvcInfo->stDxInfo.ex.tva.szDefaultAuth, aucAuthority, sizeof(HUINT8)*DxDEFAULT_AUTHORITY_LEN)

// TV-AnyTime : Guidance Type
#define XdbSvc_TvAnyTimeGetGuidanceType(pstSvcInfo)												((HUINT8)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ex.tva.ucGuidanceType)
#define XdbSvc_TvAnyTimeSetGuidanceType(pstSvcInfo, ucType)										(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ex.tva.ucGuidanceType = (HUINT8)ucType)

// TV-AnyTime : Guidance Mode
#define XdbSvc_TvAnyTimeGetGuidanceMode(pstSvcInfo)												((HUINT8)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ex.tva.ucGuidanceMode)
#define XdbSvc_TvAnyTimeSetGuidanceMode(pstSvcInfo, ucMode)									(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ex.tva.ucGuidanceMode = (HUINT8)ucMode)

// TV-AnyTime : Guidance String
#define XdbSvc_TvAnyTimeGetGuidanceStr(pstSvcInfo)				 							(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ex.tva.szGuidanceStr)
#define XdbSvc_TvAnyTimeSetGuidanceStr(pstSvcInfo, szStr)									HLIB_STD_StrUtf8NCpy(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ex.tva.szGuidanceStr, szStr, sizeof(HUINT8)*DxMAX_GUIDANCE_INFO_LEN)
#define XdbSvc_TvAnyTimeResetGuidanceStr(pstSvcInfo)											HxSTD_MemSet(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ex.tva.szGuidanceStr, 0x00, sizeof(HUINT8)*DxMAX_GUIDANCE_INFO_LEN)

// TV-AnyTime : Content Mgr
#define	XdbSvc_TvAnyTimeGetContentMgr(pstSvcInfo)											((HUINT32)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ex.tva.ulFTAContentMgr)
#define	XdbSvc_TvAnyTimeSetContentMgr(pstSvcInfo, ulContentMgr)								(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ex.tva.ulFTAContentMgr = (HUINT32)ulContentMgr)

// TV-AnyTime : LcnSub
#define XdbSvc_TvAnyTimeGetLcnSub(pstSvcInfo)												((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ex.tva.usLcnSub)
#define XdbSvc_TvAnyTimeSetLcnSub(pstSvcInfo, usLcnSubX)										(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ex.tva.usLcnSub = (HUINT16)usLcnSubX)

// TV-AnyTime : Svc Region Info
#define XdbSvc_TvAnyTimeGetRegionInfo(pstSvcInfo, nIndex)										((DbSvc_T_RegionInfo_t *)&(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ex.tva.stRegionInfo[nIndex]))
#define XdbSvc_TvAnyTimeSetRegionInfo(pstSvcInfo, nIndex, pstValue)								HxSTD_MemCopy(&((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ex.tva.stRegionInfo[nIndex], pstValue, sizeof(DxTvAnyTime_RegionInfo_t))

// TV-AnyTime : Ts Info
#define	XdbSvc_TvAnyTimeTsGetFTAContentMgr(pstTs)								((HUINT32)((DbSvc_TsInfo_t*)pstTs)->utExt.stTvAnyTime.ulFTAContentMgr)
#define	XdbSvc_TvAnyTimeTsSetFTAContentMgr(pstTs, ulValue)						(((DbSvc_TsInfo_t *)pstTs)->utExt.stTvAnyTime.ulFTAContentMgr = ulValue)

// TV-AnyTime : Ts Region Info
#define XdbSvc_TvAnyTimeTsGetRegionInfo(pstTs, nIndex)										((DbSvc_T_RegionInfo_t *)&(((DbSvc_TsInfo_t*)pstTs)->utExt.stTvAnyTime.stRegionInfo[nIndex]))
#define XdbSvc_TvAnyTimeTsSetRegionInfo(pstTs, nIndex, pstValue)								HxSTD_MemCopy(&((DbSvc_TsInfo_t*)pstTs)->utExt.stTvAnyTime.stRegionInfo[nIndex], pstValue, sizeof(DxTvAnyTime_RegionInfo_t))

// TV-AnyTime : Network Info
#define	XdbSvc_TvAnyTimeNetGetFTAContentMgr(pstNet)							((HUINT32)((DbSvc_NetInfo_t*)pstNet)->utExt.stTvAnyTime.ulFTAContentMgr)
#define	XdbSvc_TvAnyTimeNetSetFTAContentMgr(pstNet, ulValue)					(((DbSvc_NetInfo_t*)pstNet)->utExt.stTvAnyTime.ulFTAContentMgr = ulValue)

// TV-AnyTime : Network Region Info
#define XdbSvc_TvAnyTimeNetGetRegionInfo(pstNet, nIndex)										((DbSvc_T_RegionInfo_t *)&(((DbSvc_NetInfo_t*)pstNet)->utExt.stTvAnyTime.stRegionInfo[nIndex]))
#define XdbSvc_TvAnyTimeNetSetRegionInfo(pstNet, nIndex, pstValue)								HxSTD_MemCopy(&((DbSvc_NetInfo_t*)pstNet)->utExt.stTvAnyTime.stRegionInfo[nIndex], pstValue, sizeof(DxTvAnyTime_RegionInfo_t))


#else
/* FreeSat */
// FreeSat : Default Authority
#define XdbSvc_FreeSatGetDefaultAuthority(stSvcInfo)										stSvcInfo.utExt.stFreeSat.aucDefaultAuthority
#define XdbSvc_FreeSatGetDefaultAuthorityPtr(pstSvcInfo)									pstSvcInfo->utExt.stFreestFreeSat.aucDefaultAuthority
#define XdbSvc_FreeSatSetDefaultAuthority(stSvcInfo, aucAuthority)							HLIB_STD_StrNCpySafe(stSvcInfo.utExt.stFreeSat.aucDefaultAuthority, aucAuthority, sizeof(HUINT8)*DxDEFAULT_AUTHORITY_LEN)
#define XdbSvc_FreeSatSetDefaultAuthorityPtr(pstSvcInfo, aucAuthority)						HLIB_STD_StrNCpySafe(pstSvcInfo->utExt.stFreeSat.aucDefaultAuthority, aucAuthority, sizeof(HUINT8)*DxDEFAULT_AUTHORITY_LEN)

// FreeSat : Guidance Type
#define XdbSvc_FreeSatGetGuidanceType(stSvcInfo)											stSvcInfo.utExt.stFreeSat.stGuidanceInfo.ucGuidanceType
#define XdbSvc_FreeSatGetGuidanceTypePtr(pstSvcInfo)										pstSvcInfo->utExt.stFreeSat.stGuidanceInfo.ucGuidanceType
#define XdbSvc_FreeSatSetGuidanceType(stSvcInfo, ucType)									stSvcInfo.utExt.stFreeSat.stGuidanceInfo.ucGuidanceType = (HUINT8)ucType
#define XdbSvc_FreeSatSetGuidanceTypePtr(pstSvcInfo, ucType)								pstSvcInfo->utExt.stFreeSat.stGuidanceInfo.ucGuidanceType = (HUINT8)ucType

// FreeSat : Guidance Mode
#define XdbSvc_FreeSatGetGuidanceMode(stSvcInfo)											stSvcInfo.utExt.stFreeSat.stGuidanceInfo.ucGuidanceMode
#define XdbSvc_FreeSatGetGuidanceModePtr(pstSvcInfo)										pstSvcInfo->utExt.stFreeSat.stGuidanceInfo.ucGuidanceMode
#define XdbSvc_FreeSatSetGuidanceMode(stSvcInfo, ucMode)									stSvcInfo.utExt.stFreeSat.stGuidanceInfo.ucGuidanceMode = (HUINT8)ucMode
#define XdbSvc_FreeSatSetGuidanceModePtr(pstSvcInfo, ucMode)								pstSvcInfo->utExt.stFreeSat.stGuidanceInfo.ucGuidanceMode = (HUINT8)ucMode

// FreeSat : Guidance String
#define XdbSvc_FreeSatGetGuidanceStr(stSvcInfo)												stSvcInfo.utExt.stFreeSat.stGuidanceInfo.szGuidanceStr
#define XdbSvc_FreeSatGetGuidanceStrPtr(pstSvcInfo)											pstSvcInfo->utExt.stFreeSat.stGuidanceInfo.szGuidanceStr
#define XdbSvc_FreeSatSetGuidanceStr(stSvcInfo, szStr)										HLIB_STD_StrNCpySafe(stSvcInfo.utExt.stFreeSat.stGuidanceInfo.szGuidanceStr, szStr, sizeof(HUINT8)*DxMAX_GUIDANCE_INFO_LEN)
#define XdbSvc_FreeSatSetGuidanceStrPtr(pstSvcInfo, szStr)									HLIB_STD_StrNCpySafe(pstSvcInfo->utExt.stFreeSat.stGuidanceInfo.szGuidanceStr, szStr, sizeof(HUINT8)*DxMAX_GUIDANCE_INFO_LEN)
#define XdbSvc_FreeSatResetGuidanceStr(stSvcInfo)											HxSTD_MemSet(stSvcInfo.utExt.stFreeSat.stGuidanceInfo.szGuidanceStr, 0x00, sizeof(HUINT8)*DxMAX_GUIDANCE_INFO_LEN)
#define XdbSvc_FreeSatResetGuidanceStrPtr(pstSvcInfo)										HxSTD_MemSet(pstSvcInfo->utExt.stFreeSat.stGuidanceInfo.szGuidanceStr, 0x00, sizeof(HUINT8)*DxMAX_GUIDANCE_INFO_LEN)

// FreeSat : Contents Mgr
#define	XdbSvc_FreeSatGetContentMgr(stSvcInfo)												stSvcInfo.stDxInfo.ex.freesat.contentMgr
#define	XdbSvc_FreeSatGetContentMgrPtr(pstSvcInfo)											pstSvcInfo->stDxInfo.ex.freesat.contentMgr
#define	XdbSvc_FreeSatSetContentMgr(stSvcInfo, ulContentMgr)								stSvcInfo.stDxInfo.ex.freesat.contentMgr = (HUINT32)ulContentMgr
#define	XdbSvc_FreeSatSetContentMgrPtr(pstSvcInfo, ulContentMgr)							pstSvcInfo->stDxInfo.ex.freesat.contentMgr = (HUINT32)ulContentMgr

// FreeSat Svc Identifier
#define	XdbSvc_FreeSatGetSvcIdentifier(stSvc)									stSvc.utExt.stFreeSat.usFSatSvcIdentifier
#define	XdbSvc_FreeSatSetSvcIdentifier(stSvc, usValue)							stSvc.utExt.stFreeSat.usFSatSvcIdentifier = usValue
#define	XdbSvc_FreeSatGetSvcIdentifierPtr(pstSvc)								pstSvc->utExt.stFreeSat.usFSatSvcIdentifier
#define	XdbSvc_FreeSatSetSvcIdentifierPtr(pstSvc, usValue)						pstSvc->utExt.stFreeSat.usFSatSvcIdentifier = usValue
// FreeSat Sat Region Id
#define	XdbSvc_FreeSatGetSatRegionId(stSvc)										stSvc.utExt.stFreeSat.usFSatReginId
#define	XdbSvc_FreeSatGetSatRegionIdPtr(pstSvc)									pstSvc->utExt.stFreeSat.usFSatReginId
#define	XdbSvc_FreeSatSetSatRegionId(stSvc, usValue)							stSvc.utExt.stFreeSat.usFSatReginId = (HUINT16)usValue
#define	XdbSvc_FreeSatSetSatRegionIdPtr(pstSvc, usValue)						pstSvc->utExt.stFreeSat.usFSatReginId = (HUINT16)usValue

// FreeSat Sat Resent Follow
#define	XdbSvc_FreeSatGetSatPresentFollow(stSvc)								stSvc.utExt.stFreeSat.ucPresentFollow
#define	XdbSvc_FreeSatGetSatPresentFollowPtr(pstSvc)							pstSvc->utExt.stFreeSat.ucPresentFollow
#define	XdbSvc_FreeSatSetSatPresentFollow(stSvc, ucValue)						stSvc.utExt.stFreeSat.ucPresentFollow = ucValue
#define	XdbSvc_FreeSatSetSatPresentFollowPtr(pstSvc, ucValue)					pstSvc->utExt.stFreeSat.ucPresentFollow = ucValue

// FreeSat Service Short Name
#define	XdbSvc_FreeSatGetSvcShortName(stSvc)									stSvc.utExt.stFreeSat.szSvcShortName
#define	XdbSvc_FreeSatGetSvcShortNamePtr(pstSvc)								pstSvc->utExt.stFreeSat.szSvcShortName

// Ts Info
#define	XdbSvc_FreeSatTsGetFTAContentMgr(stTs)									stTs.utExt.stFreeSat.ulFTAContentMgr
#define	XdbSvc_FreeSatTsGetFTAContentMgrPtr(pstTs)								pstTs->utExt.stFreeSat.ulFTAContentMgr
#define	XdbSvc_FreeSatTsSetFTAContentMgr(stTs, ulValue)							stTs.utExt.stFreeSat.ulFTAContentMgr = ulValue
#define	XdbSvc_FreeSatTsSetFTAContentMgrPtr(pstTs, ulValue)						pstTs->utExt.stFreeSat.ulFTAContentMgr = ulValue

// Network Info
#define	XdbSvc_FreeSatNetGetFTAContentMgr(stNet)								stNet.utExt.stFreeSat.ulFTAContentMgr
#define	XdbSvc_FreeSatNetGetFTAContentMgrPtr(pstNet)							pstNet->utExt.stFreeSat.ulFTAContentMgr
#define	XdbSvc_FreeSatNetSetFTAContentMgr(stNet, ulValue)						stNet.utExt.stFreeSat.ulFTAContentMgr = ulValue
#define	XdbSvc_FreeSatNetSetFTAContentMgrPtr(pstNet, ulValue)					pstNet->utExt.stFreeSat.ulFTAContentMgr = ulValue

// FreeSat Group : Group Info
#define	XdbSvc_FreeSatGroupGetGroupType(stGroup)								stGroup.utExt.stFreeSat.ucGroupType
#define	XdbSvc_FreeSatGroupGetGroupTypePtr(pstGroup)							pstGroup->utExt.stFreeSat.ucGroupType
#define	XdbSvc_FreeSatGroupSetGroupType(stGroup, ucValue)						stGroup.utExt.stFreeSat.ucGroupType = ucValue
#define	XdbSvc_FreeSatGroupSetGroupTypePtr(pstGroup, ucValue)					pstGroup->utExt.stFreeSat.ucGroupType = ucValue

// FreeSat Group : Nondestructive Tune Flag
#define	XdbSvc_FreeSatGroupGetNondestructiveTuneFlag(stGroup)					stGroup.utExt.stFreeSat.ucNondestructiveTuneFlag
#define	XdbSvc_FreeSatGroupGetNondestructiveTuneFlagPtr(pstGroup)				pstGroup->utExt.stFreeSat.ucNondestructiveTuneFlag
#define	XdbSvc_FreeSatGroupSetNondestructiveTuneFlag(stGroup, ucValue)			stGroup.utExt.stFreeSat.ucNondestructiveTuneFlag = ucValue
#define	XdbSvc_FreeSatGroupSetNoneDestructiveTuneFlagPtr(pstGroup, ucValue)		pstGroup->utExt.stFreeSat.ucNondestructiveTuneFlag = ucValue

// FreeSat Group : Return Channel Access Flag
#define	XdbSvc_FreeSatGroupGetReturnChannelAccessFlag(stGroup)					stGroup.utExt.stFreeSat.ucReturnChannelAccessFlag
#define	XdbSvc_FreeSatGroupGetReturnChannelAccessFlagPtr(pstGroup)				pstGroup->utExt.stFreeSat.ucReturnChannelAccessFlag
#define	XdbSvc_FreeSatGroupSetReturnChannelAccessFlag(stGroup, ucValue)			stGroup.utExt.stFreeSat.ucReturnChannelAccessFlag = ucValue
#define	XdbSvc_FreeSatGroupSetReturnChannelAccessFlagPtr(pstGroup, ucValue)		pstGroup->utExt.stFreeSat.ucReturnChannelAccessFlag = ucValue

// FreeSat Group : Iso639 Language Code
#define	XdbSvc_FreeSatGroupGetIso639LangCode(stGroup)							stGroup.utExt.stFreeSat.szIso639LangCode
#define	XdbSvc_FreeSatGroupGetIso639LangCodePtr(pstGroup)						pstGroup->utExt.stFreeSat.szIso639LangCode

// FreeSat Group : G2Extension Flag
#define	XdbSvc_FreeSatGroupGetG2ExtensionFlag(stGroup)							stGroup.utExt.stFreeSat.ucG2ExtensionFlag
#define	XdbSvc_FreeSatGroupGetG2ExtensionFlagPtr(pstGroup)						pstGroup->utExt.stFreeSat.ucG2ExtensionFlag
#define	XdbSvc_FreeSatGroupSetG2ExtensionFlag(stGroup, ucValue)					stGroup.utExt.stFreeSat.ucG2ExtensionFlag = ucValue
#define	XdbSvc_FreeSatGroupSetG2ExtensionFlagPtr(pstGroup, ucValue)				pstGroup->utExt.stFreeSat.ucG2ExtensionFlag = ucValue

// FreeSat Group : Default Visible Flag
#define	XdbSvc_FreeSatGroupGetDefaultVisibleFlag(stGroup)						stGroup.utExt.stFreeSat.ucDefaultVisableFlag
#define	XdbSvc_FreeSatGroupGetDefaultVisibleFlagPtr(pstGroup)					pstGroup->utExt.stFreeSat.ucDefaultVisableFlag
#define	XdbSvc_FreeSatGroupSetDefaultVisibleFlag(stGroup, ucValue)				stGroup.utExt.stFreeSat.ucDefaultVisableFlag = ucValue
#define	XdbSvc_FreeSatGroupSetDefaultVisibleFlagPtr(pstGroup, ucValue)			pstGroup->utExt.stFreeSat.ucDefaultVisableFlag = ucValue

// FreeSat Group : Adult Channels Group Flag
#define	XdbSvc_FreeSatGroupGetAdultChannelsGroupFlag(stGroup)					stGroup.utExt.stFreeSat.ucAdultChannelsGroupFlag
#define	XdbSvc_FreeSatGroupGetAdultChannelsGroupFlagPtr(pstGroup)				pstGroup->utExt.stFreeSat.ucAdultChannelsGroupFlag
#define	XdbSvc_FreeSatGroupSetAdultChannelsGroupFlag(stGroup, ucValue)			stGroup.utExt.stFreeSat.ucAdultChannelsGroupFlag = ucValue
#define	XdbSvc_FreeSatGroupSetAdultChannelsGroupFlagPtr(pstGroup, ucValue)		pstGroup->utExt.stFreeSat.ucAdultChannelsGroupFlag = ucValue

// FreeSat Group : Ip Channels Groups Flags
#define	XdbSvc_FreeSatGroupGetIpChannelsGroupFlag(stGroup)						stGroup.utExt.stFreeSat.ucIpChannelsGroupFlag
#define	XdbSvc_FreeSatGroupGetIpChannelsGroupFlagPtr(pstGroup)					pstGroup->utExt.stFreeSat.ucIpChannelsGroupFlag
#define	XdbSvc_FreeSatGroupSetIpChannelsGroupFlag(stGroup, ucValue)				stGroup.utExt.stFreeSat.ucIpChannelsGroupFlag = ucValue
#define	XdbSvc_FreeSatGroupSetIpChannelsGroupFlagPtr(pstGroup, ucValue)			pstGroup->utExt.stFreeSat.ucIpChannelsGroupFlag = ucValue

// FreeSat Group : Ca Channels Group Flag
#define	XdbSvc_FreeSatGroupGetCaChannelsGroupFlag(stGroup)						stGroup.utExt.stFreeSat.ucCaChannelsGroupFlag
#define	XdbSvc_FreeSatGroupGetCaChannelsGroupFlagPtr(pstGroup)					pstGroup->utExt.stFreeSat.ucCaChannelsGroupFlag
#define	XdbSvc_FreeSatGroupSetCaChannelsGroupFlag(stGroup, ucValue)				stGroup.utExt.stFreeSat.ucCaChannelsGroupFlag = ucValue
#define	XdbSvc_FreeSatGroupSetCaChannelsGroupFlagPtr(pstGroup, ucValue)			pstGroup->utExt.stFreeSat.ucCaChannelsGroupFlag = ucValue

/* SES */
// SES : Virtual Svc Id
#define XdbSvc_SesGetVirtualSvcId(stSvc)									stSvc.stDxInfo.ex.ses.usVirtualSvcId
#define XdbSvc_SesSetVirtualSvcId(stSvc, usValue)							stSvc.stDxInfo.ex.ses.usVirtualSvcId = usValue
#define XdbSvc_SesGetVirtualSvcIdPtr(pstSvc)								pstSvc->stDxInfo.ex.ses.usVirtualSvcId
#define XdbSvc_SesSetVirtualSvcIdPtr(pstSvc, usValue)						pstSvc->stDxInfo.ex.ses.usVirtualSvcId = usValue

// SES : New Svc
#define XdbSvc_SesGetNewSvc(stSvc)											stSvc.stDxInfo.ex.ses.bNewSvc
#define XdbSvc_SesSetNewSvc(stSvc, bValue)									stSvc.stDxInfo.ex.ses.bNewSvc = bValue
#define XdbSvc_SesGetNewSvcPtr(pstSvc)										pstSvc->stDxInfo.ex.ses.bNewSvc
#define XdbSvc_SesSetNewSvcPtr(pstSvc, bValue)								pstSvc->stDxInfo.ex.ses.bNewSvc = bValue

/* UK DTT */
// UK DTT : Default Authority
#define XdbSvc_UkDttGetDefaultAuthority(pstSvcInfo)											((DbSvc_Info_t*)pstSvcInfo)->utExt.stTvAnyTime.aucDefaultAuthority
#define XdbSvc_UkDttSetDefaultAuthority(pstSvcInfo, aucAuthority)							HLIB_STD_StrNCpySafe(((DbSvc_Info_t*)pstSvcInfo)->utExt.stTvAnyTime.aucDefaultAuthority, aucAuthority, sizeof(HUINT8)*DxDEFAULT_AUTHORITY_LEN)

// UK DTT : Guidance Type
#define XdbSvc_UkDttGetGuidanceType(pstSvcInfo)												((DbSvc_Info_t*)pstSvcInfo)->utExt.stTvAnyTime.stGuidanceInfo.ucGuidanceType
#define XdbSvc_UkDttSetGuidanceType(pstSvcInfo, ucType)										((DbSvc_Info_t*)pstSvcInfo)->utExt.stTvAnyTime.stGuidanceInfo.ucGuidanceType = (HUINT8)ucType

// UK DTT : Guidance Mode
#define XdbSvc_UkDttGetGuidanceMode(pstSvcInfo)											((DbSvc_Info_t*)pstSvcInfo)->utExt.stTvAnyTime.stGuidanceInfo.ucGuidanceMode
#define XdbSvc_UkDttSetGuidanceMode(pstSvcInfo, ucMode)									((DbSvc_Info_t*)pstSvcInfo)->utExt.stTvAnyTime.stGuidanceInfo.ucGuidanceMode = (HUINT8)ucMode

// UK DTT : Guidance String
#define XdbSvc_UkDttGetGuidanceStr(pstSvcInfo)											((DbSvc_Info_t*)pstSvcInfo)->utExt.stTvAnyTime.stGuidanceInfo.szGuidanceStr
#define XdbSvc_UkDttSetGuidanceStr(pstSvcInfo, szStr)									HLIB_STD_StrUtf8NCpy(((DbSvc_Info_t*)pstSvcInfo)->utExt.stTvAnyTime.stGuidanceInfo.szGuidanceStr, szStr, sizeof(HUINT8)*DxMAX_GUIDANCE_INFO_LEN)
#define XdbSvc_UkDttResetGuidanceStr(pstSvcInfo)											HxSTD_MemSet(((DbSvc_Info_t*)pstSvcInfo)->utExt.stTvAnyTime.stGuidanceInfo.szGuidanceStr, 0x00, sizeof(HUINT8)*DxMAX_GUIDANCE_INFO_LEN)

// UK DTT : Content Mgr
#define	XdbSvc_UkDttGetContentMgr(pstSvcInfo)											((DbSvc_Info_t*)pstSvcInfo)->utExt.stTvAnyTime.ulFTAContentMgr
#define	XdbSvc_UkDttSetContentMgr(pstSvcInfo, ulContentMgr)								((DbSvc_Info_t*)pstSvcInfo)->utExt.stTvAnyTime.ulFTAContentMgr = (HUINT32)ulContentMgr

// UK DTT : LcnSub
#define XdbSvc_UkDttGetLcnSub(pstSvcInfo)												((DbSvc_Info_t*)pstSvcInfo)->utExt.stTvAnyTime.usLcnSub
#define XdbSvc_UkDttSetLcnSub(pstSvcInfo, usLcn)											((DbSvc_Info_t*)pstSvcInfo)->utExt.stTvAnyTime.usLcnSub = (HUINT16)usLcn

// UK DTT : Region Info
#define XdbSvc_UkDttGetRegionInfo(pstSvcInfo, nIndex)										((DbSvc_T_RegionInfo_t *)&(((DbSvc_Info_t *)pstSvcInfo)->utExt.stTvAnyTime.stRegionInfo[nIndex]))
#define XdbSvc_UkDttSetRegionInfo(pstSvcInfo, nIndex, pstValue)								HxSTD_MemCopy(&((DbSvc_Info_t *)pstSvcInfo)->utExt.stTvAnyTime.stRegionInfo[nIndex], pstValue, sizeof(DbSvc_T_RegionInfo_t))

// UK DTT : Ts Info
#define	XdbSvc_UkDttTsGetFTAContentMgr(pstTs)											((DbSvc_TsInfo_t*)pstTs)->utExt.stTvAnyTime.ulFTAContentMgr
#define	XdbSvc_UkDttTsSetFTAContentMgr(pstTs, ulValue)									((DbSvc_TsInfo_t*)pstTs)->utExt.stTvAnyTime.ulFTAContentMgr = ulValue

// UK DTT : Ts Region Info
#define XdbSvc_UkDttTsGetRegionInfo(pstTs, nIndex)										((DbSvc_T_RegionInfo_t *)&(((DbSvc_TsInfo_t*)pstTs)->utExt.stTvAnyTime.stRegionInfo[nIndex]))
#define XdbSvc_UkDttTsSetRegionInfo(pstTs, nIndex, pstValue)								HxSTD_MemCopy(&((DbSvc_TsInfo_t*)pstTs)->utExt.stTvAnyTime.stRegionInfo[nIndex], pstValue, sizeof(DxUkDtt_RegionInfo_t))

// UK DTT : Network Info
#define	XdbSvc_UkDttNetGetFTAContentMgr(pstNet)											((DbSvc_NetInfo_t*)pstNet)->utExt.stTvAnyTime.ulFTAContentMgr
#define	XdbSvc_UkDttNetSetFTAContentMgr(pstNet, ulValue)									((DbSvc_NetInfo_t*)pstNet)->utExt.stTvAnyTime.ulFTAContentMgr = ulValue

// UK DTT : Ts Region Info
#define XdbSvc_UkDttNetGetRegionInfo(pstNet, nIndex)										((DbSvc_T_RegionInfo_t *)&(((DbSvc_NetInfo_t*)pstNet)->utExt.stTvAnyTime.stRegionInfo[nIndex]))
#define XdbSvc_UkDttNetSetRegionInfo(pstNet, nIndex, pstValue)								HxSTD_MemCopy(&((DbSvc_NetInfo_t*)pstNet)->utExt.stTvAnyTime.stRegionInfo[nIndex], pstValue, sizeof(DxUkDtt_RegionInfo_t))

#endif

#if defined(CONFIG_OP_FREESAT)
#if defined(DbSvc_SUPPORT_DXINFO)
// User Customizable		eSvcUserFlag_01_FsatUserCustom
#define	XdbSvc_GetCustomizableFlag(stSvcInfo)								(HBOOL)(stSvcInfo.stDxInfo.ex.freesat.ucUserCustomizable)
#define	XdbSvc_GetCustomizableFlagPtr(pstSvcInfo)							(HBOOL)(pstSvcInfo->stDxInfo.ex.freesat.ucUserCustomizable)
#define	XdbSvc_SetCustomizableFlag(stSvcInfo, bCustomizableFlag)			(stSvcInfo.stDxInfo.ex.freesat.ucUserCustomizable = bCustomizableFlag)
#define	XdbSvc_SetCustomizableFlagPtr(pstSvcInfo, bCustomizableFlag)		(pstSvcInfo->stDxInfo.ex.freesat.ucUserCustomizable = bCustomizableFlag)
// User Selectable			eSvcUserFlag_02_Selectable
#define	XdbSvc_GetSelectableFlag(stSvcInfo)									(HBOOL)(stSvcInfo.stDxInfo.ex.freesat.ucUserSelectable)
#define	XdbSvc_GetSelectableFlagPtr(pstSvcInfo)								(HBOOL)(pstSvcInfo->stDxInfo.ex.freesat.ucUserSelectable)
#define	XdbSvc_SetSelectableFlag(stSvcInfo, bSelectableFlag)				(stSvcInfo.stDxInfo.ex.freesat.ucUserSelectable = bSelectableFlag)
#define	XdbSvc_SetSelectableFlagPtr(pstSvcInfo, bSelectableFlag)			(pstSvcInfo->stDxInfo.ex.freesat.ucUserSelectable = bSelectableFlag)
// Adult Service		eSvcUserFlag_09_AdultSvc
#define	XdbSvc_GetAdultSvcFlag(stSvcInfo)									(HBOOL)(stSvcInfo.stDxInfo.ex.freesat.ucAdultSvc)
#define	XdbSvc_GetAdultSvcFlagPtr(pstSvcInfo)								(HBOOL)(pstSvcInfo->stDxInfo.ex.freesat.ucAdultSvc)
#define	XdbSvc_SetAdultSvcFlag(stSvcInfo, bAdultSvcFlag)					(stSvcInfo.stDxInfo.ex.freesat.ucAdultSvc = bAdultSvcFlag)
#define	XdbSvc_SetAdultSvcFlagPtr(pstSvcInfo, bAdultSvcFlag)				(pstSvcInfo->stDxInfo.ex.freesat.ucAdultSvc = bAdultSvcFlag)
// Video Codec H264		eSvcUserFlag_27_VideoCodecH264
#define	XdbSvc_GetVidCodecH264Flag(stSvcInfo)								(HBOOL)(stSvcInfo.stDxInfo.ex.freesat.ucVideoCodecH264)
#define	XdbSvc_GetVidCodecH264FlagPtr(pstSvcInfo)							(HBOOL)(pstSvcInfo->stDxInfo.ex.freesat.ucVideoCodecH264)
#define	XdbSvc_SetVidCodecH264Flag(stSvcInfo, bVidCodecH264Flag)			(stSvcInfo.stDxInfo.ex.freesat.ucVideoCodecH264 = bVidCodecH264Flag)
#define	XdbSvc_SetVidCodecH264FlagPtr(pstSvcInfo, bVidCodecH264Flag)		(pstSvcInfo->stDxInfo.ex.freesat.ucVideoCodecH264 = bVidCodecH264Flag)
#else
// User Customizable		eSvcUserFlag_01_FsatUserCustom
#define	XdbSvc_GetCustomizableFlag(stSvcInfo)								(HBOOL)((stSvcInfo.eUserFlag1 & eSvcUserFlag_01_FsatUserCustom) ? TRUE : FALSE)
#define	XdbSvc_GetCustomizableFlagPtr(pstSvcInfo)							(HBOOL)((pstSvcInfo->eUserFlag1 & eSvcUserFlag_01_FsatUserCustom) ? TRUE : FALSE)
#define	XdbSvc_SetCustomizableFlag(stSvcInfo, bCustomizableFlag)			stSvcInfo.eUserFlag1 = (bCustomizableFlag) ? (stSvcInfo.eUserFlag1 | eSvcUserFlag_01_FsatUserCustom) : (stSvcInfo.eUserFlag1 & ~eSvcUserFlag_01_FsatUserCustom)
#define	XdbSvc_SetCustomizableFlagPtr(pstSvcInfo, bCustomizableFlag)		pstSvcInfo->eUserFlag1 = (bCustomizableFlag) ? (pstSvcInfo->eUserFlag1 | eSvcUserFlag_01_FsatUserCustom) : (pstSvcInfo->eUserFlag1 & ~eSvcUserFlag_01_FsatUserCustom)
// User Selectable			eSvcUserFlag_02_Selectable
#define	XdbSvc_GetSelectableFlag(stSvcInfo)									(HBOOL)((stSvcInfo.eUserFlag1 & eSvcUserFlag_02_Selectable) ? TRUE : FALSE)
#define	XdbSvc_GetSelectableFlagPtr(pstSvcInfo)								(HBOOL)((pstSvcInfo->eUserFlag1 & eSvcUserFlag_02_Selectable) ? TRUE : FALSE)
#define	XdbSvc_SetSelectableFlag(stSvcInfo, bSelectableFlag)				stSvcInfo.eUserFlag1 = (bSelectableFlag) ? (stSvcInfo.eUserFlag1 | eSvcUserFlag_02_Selectable) : (stSvcInfo.eUserFlag1 & ~eSvcUserFlag_02_Selectable)
#define	XdbSvc_SetSelectableFlagPtr(pstSvcInfo, bSelectableFlag)			pstSvcInfo->eUserFlag1 = (bSelectableFlag) ? (pstSvcInfo->eUserFlag1 | eSvcUserFlag_02_Selectable) : (pstSvcInfo->eUserFlag1 & ~eSvcUserFlag_02_Selectable)
// Adult Service		eSvcUserFlag_09_AdultSvc
#define	XdbSvc_GetAdultSvcFlag(stSvcInfo)									(HBOOL)((stSvcInfo.eUserFlag1 & eSvcUserFlag_09_AdultSvc) ? TRUE : FALSE)
#define	XdbSvc_GetAdultSvcFlagPtr(pstSvcInfo)								(HBOOL)((pstSvcInfo->eUserFlag1 & eSvcUserFlag_09_AdultSvc) ? TRUE : FALSE)
#define	XdbSvc_SetAdultSvcFlag(stSvcInfo, bAdultSvcFlag)					stSvcInfo.eUserFlag1 = (bAdultSvcFlag) ? (stSvcInfo.eUserFlag1 | eSvcUserFlag_09_AdultSvc) : (stSvcInfo.eUserFlag1 & ~eSvcUserFlag_09_AdultSvc)
#define	XdbSvc_SetAdultSvcFlagPtr(pstSvcInfo, bAdultSvcFlag)				pstSvcInfo->eUserFlag1 = (bAdultSvcFlag) ? (pstSvcInfo->eUserFlag1 | eSvcUserFlag_09_AdultSvc) : (pstSvcInfo->eUserFlag1 & ~eSvcUserFlag_09_AdultSvc)
// Video Codec H264		eSvcUserFlag_27_VideoCodecH264
#define	XdbSvc_GetVidCodecH264Flag(stSvcInfo)								(HBOOL)((stSvcInfo.eUserFlag1 & eSvcUserFlag_27_VideoCodecH264) ? TRUE : FALSE)
#define	XdbSvc_GetVidCodecH264FlagPtr(pstSvcInfo)							(HBOOL)((pstSvcInfo->eUserFlag1 & eSvcUserFlag_27_VideoCodecH264) ? TRUE : FALSE)
#define	XdbSvc_SetVidCodecH264Flag(stSvcInfo, bVidCodecH264Flag)			stSvcInfo.eUserFlag1 = (bVidCodecH264Flag) ? (stSvcInfo.eUserFlag1 | eSvcUserFlag_27_VideoCodecH264) : (stSvcInfo.eUserFlag1 & ~eSvcUserFlag_27_VideoCodecH264)
#define	XdbSvc_SetVidCodecH264FlagPtr(pstSvcInfo, bVidCodecH264Flag)		pstSvcInfo->eUserFlag1 = (bVidCodecH264Flag) ? (pstSvcInfo->eUserFlag1 | eSvcUserFlag_27_VideoCodecH264) : (pstSvcInfo->eUserFlag1 & ~eSvcUserFlag_27_VideoCodecH264)
#endif
#elif defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
#if defined(DbSvc_SUPPORT_DXINFO)
// User Customizable		eSvcUserFlag_01_FsatUserCustom
#define	XdbSvc_GetCustomizableFlag(stSvcInfo)								FALSE
#define	XdbSvc_GetCustomizableFlagPtr(pstSvcInfo)							FALSE
#define	XdbSvc_SetCustomizableFlag(stSvcInfo, bCustomizableFlag)
#define	XdbSvc_SetCustomizableFlagPtr(pstSvcInfo, bCustomizableFlag)
// User Selectable			eSvcUserFlag_02_Selectable
#define	XdbSvc_GetSelectableFlag(stSvcInfo)									(HBOOL)(stSvcInfo.stDxInfo.ex.tva.ucUserSelectable)
#define	XdbSvc_GetSelectableFlagPtr(pstSvcInfo)								(HBOOL)(pstSvcInfo->stDxInfo.ex.tva.ucUserSelectable)
#define	XdbSvc_SetSelectableFlag(stSvcInfo, bSelectableFlag)					(stSvcInfo.stDxInfo.ex.tva.ucUserSelectable = bSelectableFlag)
#define	XdbSvc_SetSelectableFlagPtr(pstSvcInfo, bSelectableFlag)					(pstSvcInfo->stDxInfo.ex.tva.ucUserSelectable = bSelectableFlag)
// Adult Service		eSvcUserFlag_09_AdultSvc
#define	XdbSvc_GetAdultSvcFlag(stSvcInfo)									FALSE
#define	XdbSvc_GetAdultSvcFlagPtr(pstSvcInfo)								FALSE
#define	XdbSvc_SetAdultSvcFlag(stSvcInfo, bAdultSvcFlag)
#define	XdbSvc_SetAdultSvcFlagPtr(pstSvcInfo, bAdultSvcFlag)
// Video Codec H264		eSvcUserFlag_27_VideoCodecH264
#define	XdbSvc_GetVidCodecH264Flag(stSvcInfo)								FALSE
#define	XdbSvc_GetVidCodecH264FlagPtr(pstSvcInfo)							FALSE
#define	XdbSvc_SetVidCodecH264Flag(stSvcInfo, bVidCodecH264Flag)
#define	XdbSvc_SetVidCodecH264FlagPtr(pstSvcInfo, bVidCodecH264Flag)
#else
// User Customizable		eSvcUserFlag_01_FsatUserCustom
#define	XdbSvc_GetCustomizableFlag(stSvcInfo)								FALSE
#define	XdbSvc_GetCustomizableFlagPtr(pstSvcInfo)							FALSE
#define	XdbSvc_SetCustomizableFlag(stSvcInfo, bCustomizableFlag)
#define	XdbSvc_SetCustomizableFlagPtr(pstSvcInfo, bCustomizableFlag)
// User Selectable			eSvcUserFlag_02_Selectable
#define	XdbSvc_GetSelectableFlag(stSvcInfo)									(HBOOL)((stSvcInfo.eUserFlag1 & eSvcUserFlag_02_Selectable) ? TRUE : FALSE)
#define	XdbSvc_GetSelectableFlagPtr(pstSvcInfo)								(HBOOL)((pstSvcInfo->eUserFlag1 & eSvcUserFlag_02_Selectable) ? TRUE : FALSE)
#define	XdbSvc_SetSelectableFlag(stSvcInfo, bSelectableFlag)				stSvcInfo.eUserFlag1 = (bSelectableFlag) ? (stSvcInfo.eUserFlag1 | eSvcUserFlag_02_Selectable) : (stSvcInfo.eUserFlag1 & ~eSvcUserFlag_02_Selectable)
#define	XdbSvc_SetSelectableFlagPtr(pstSvcInfo, bSelectableFlag)			pstSvcInfo->eUserFlag1 = (bSelectableFlag) ? (pstSvcInfo->eUserFlag1 | eSvcUserFlag_02_Selectable) : (pstSvcInfo->eUserFlag1 & ~eSvcUserFlag_02_Selectable)
// Adult Service		eSvcUserFlag_09_AdultSvc
#define	XdbSvc_GetAdultSvcFlag(stSvcInfo)									FALSE
#define	XdbSvc_GetAdultSvcFlagPtr(pstSvcInfo)								FALSE
#define	XdbSvc_SetAdultSvcFlag(stSvcInfo, bAdultSvcFlag)
#define	XdbSvc_SetAdultSvcFlagPtr(pstSvcInfo, bAdultSvcFlag)
// Video Codec H264		eSvcUserFlag_27_VideoCodecH264
#define	XdbSvc_GetVidCodecH264Flag(stSvcInfo)								FALSE
#define	XdbSvc_GetVidCodecH264FlagPtr(pstSvcInfo)							FALSE
#define	XdbSvc_SetVidCodecH264Flag(stSvcInfo, bVidCodecH264Flag)
#define	XdbSvc_SetVidCodecH264FlagPtr(pstSvcInfo, bVidCodecH264Flag)
#endif
#elif defined(CONFIG_OP_ITALY_DBOOK)
#if defined(DbSvc_SUPPORT_DXINFO)
// User Customizable		eSvcUserFlag_01_FsatUserCustom
#define	XdbSvc_GetCustomizableFlag(stSvcInfo)								FALSE
#define	XdbSvc_GetCustomizableFlagPtr(pstSvcInfo)							FALSE
#define	XdbSvc_SetCustomizableFlag(stSvcInfo, bCustomizableFlag)
#define	XdbSvc_SetCustomizableFlagPtr(pstSvcInfo, bCustomizableFlag)
// User Selectable			eSvcUserFlag_02_Selectable
#define	XdbSvc_GetSelectableFlag(stSvcInfo)									(HBOOL)(stSvcInfo.stDxInfo.ex.itaDtt.ucUserSelectable)
#define	XdbSvc_GetSelectableFlagPtr(pstSvcInfo)								(HBOOL)(pstSvcInfo->stDxInfo.ex.itaDtt.ucUserSelectable)
#define	XdbSvc_SetSelectableFlag(stSvcInfo, bSelectableFlag)					(stSvcInfo.stDxInfo.ex.itaDtt.ucUserSelectable = bSelectableFlag)
#define	XdbSvc_SetSelectableFlagPtr(pstSvcInfo, bSelectableFlag)					(pstSvcInfo->stDxInfo.ex.itaDtt.ucUserSelectable = bSelectableFlag)
// Adult Service		eSvcUserFlag_09_AdultSvc
#define	XdbSvc_GetAdultSvcFlag(stSvcInfo)									FALSE
#define	XdbSvc_GetAdultSvcFlagPtr(pstSvcInfo)								FALSE
#define	XdbSvc_SetAdultSvcFlag(stSvcInfo, bAdultSvcFlag)
#define	XdbSvc_SetAdultSvcFlagPtr(pstSvcInfo, bAdultSvcFlag)
// Video Codec H264		eSvcUserFlag_27_VideoCodecH264
#define	XdbSvc_GetVidCodecH264Flag(stSvcInfo)								FALSE
#define	XdbSvc_GetVidCodecH264FlagPtr(pstSvcInfo)							FALSE
#define	XdbSvc_SetVidCodecH264Flag(stSvcInfo, bVidCodecH264Flag)
#define	XdbSvc_SetVidCodecH264FlagPtr(pstSvcInfo, bVidCodecH264Flag)
#else
// User Customizable		eSvcUserFlag_01_FsatUserCustom
#define	XdbSvc_GetCustomizableFlag(stSvcInfo)								FALSE
#define	XdbSvc_GetCustomizableFlagPtr(pstSvcInfo)							FALSE
#define	XdbSvc_SetCustomizableFlag(stSvcInfo, bCustomizableFlag)
#define	XdbSvc_SetCustomizableFlagPtr(pstSvcInfo, bCustomizableFlag)
// User Selectable			eSvcUserFlag_02_Selectable
#define	XdbSvc_GetSelectableFlag(stSvcInfo)									(HBOOL)((stSvcInfo.eUserFlag1 & eSvcUserFlag_02_Selectable) ? TRUE : FALSE)
#define	XdbSvc_GetSelectableFlagPtr(pstSvcInfo)								(HBOOL)((pstSvcInfo->eUserFlag1 & eSvcUserFlag_02_Selectable) ? TRUE : FALSE)
#define	XdbSvc_SetSelectableFlag(stSvcInfo, bSelectableFlag)				stSvcInfo.eUserFlag1 = (bSelectableFlag) ? (stSvcInfo.eUserFlag1 | eSvcUserFlag_02_Selectable) : (stSvcInfo.eUserFlag1 & ~eSvcUserFlag_02_Selectable)
#define	XdbSvc_SetSelectableFlagPtr(pstSvcInfo, bSelectableFlag)			pstSvcInfo->eUserFlag1 = (bSelectableFlag) ? (pstSvcInfo->eUserFlag1 | eSvcUserFlag_02_Selectable) : (pstSvcInfo->eUserFlag1 & ~eSvcUserFlag_02_Selectable)
// Adult Service		eSvcUserFlag_09_AdultSvc
#define	XdbSvc_GetAdultSvcFlag(stSvcInfo)									FALSE
#define	XdbSvc_GetAdultSvcFlagPtr(pstSvcInfo)								FALSE
#define	XdbSvc_SetAdultSvcFlag(stSvcInfo, bAdultSvcFlag)
#define	XdbSvc_SetAdultSvcFlagPtr(pstSvcInfo, bAdultSvcFlag)
// Video Codec H264		eSvcUserFlag_27_VideoCodecH264
#define	XdbSvc_GetVidCodecH264Flag(stSvcInfo)								FALSE
#define	XdbSvc_GetVidCodecH264FlagPtr(pstSvcInfo)							FALSE
#define	XdbSvc_SetVidCodecH264Flag(stSvcInfo, bVidCodecH264Flag)
#define	XdbSvc_SetVidCodecH264FlagPtr(pstSvcInfo, bVidCodecH264Flag)
#endif
#else
// User Customizable		eSvcUserFlag_01_FsatUserCustom
#define	XdbSvc_GetCustomizableFlag(stSvcInfo)								FALSE
#define	XdbSvc_GetCustomizableFlagPtr(pstSvcInfo)							FALSE
#define	XdbSvc_SetCustomizableFlag(stSvcInfo, bCustomizableFlag)
#define	XdbSvc_SetCustomizableFlagPtr(pstSvcInfo, bCustomizableFlag)
// User Selectable			eSvcUserFlag_02_Selectable
#define	XdbSvc_GetSelectableFlag(stSvcInfo)									FALSE
#define	XdbSvc_GetSelectableFlagPtr(pstSvcInfo)								FALSE
#define	XdbSvc_SetSelectableFlag(stSvcInfo, bSelectableFlag)
#define	XdbSvc_SetSelectableFlagPtr(pstSvcInfo, bSelectableFlag)
// Adult Service		eSvcUserFlag_09_AdultSvc
#define	XdbSvc_GetAdultSvcFlag(stSvcInfo)									FALSE
#define	XdbSvc_GetAdultSvcFlagPtr(pstSvcInfo)								FALSE
#define	XdbSvc_SetAdultSvcFlag(stSvcInfo, bAdultSvcFlag)
#define	XdbSvc_SetAdultSvcFlagPtr(pstSvcInfo, bAdultSvcFlag)
// Video Codec H264		eSvcUserFlag_27_VideoCodecH264
#define	XdbSvc_GetVidCodecH264Flag(stSvcInfo)								FALSE
#define	XdbSvc_GetVidCodecH264FlagPtr(pstSvcInfo)							FALSE
#define	XdbSvc_SetVidCodecH264Flag(stSvcInfo, bVidCodecH264Flag)
#define	XdbSvc_SetVidCodecH264FlagPtr(pstSvcInfo, bVidCodecH264Flag)
#endif



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*
	다음 값은 channel list priority 와 관련 있는 값이다. 값이 작을 수록 channel list 상에 먼저 보여진다
	순서 바꾸지 말 것, network 순서는 다음 함수와도 관련이 있다.
		local_dbsvc_GetPrevJapanNetworkPriority()
		local_dbsvc_GetNextJapanNetworkPriority()
*/
typedef enum
{
	eJpNetType_ALL			= 0,		/* DT, BS, i-HITS, JC-HITS, CS1, CS2 */

	eJpNetType_DT			= 0x0001,	/* J-COM, Digital Terrestrial */
	eJpNetType_BS			= 0x0002,	/* J-COM, BS Satellite */
	eJpNetType_CATV			= 0x0004,	/* J-COM, CATV, Remux(i-HITS) */
	eJpNetType_JCHITS		= 0x0008,	/* JC-HITS */
	eJpNetType_CS1			= 0x0010,	/* CS110 1 */
	eJpNetType_CS2			= 0x0020,	/* CS110 2 */
	eJpNetType_UNKNOWN		= 0x0040,	/* Unknown Network without NIT and SDT */
	eJpNetType_SKAPA		= 0x0080,
} XdbSvc_JapanNetworkType_e;

typedef enum
{
	eJpMediaType_TV				= 0,
	eJpMediaType_RADIO_DATA		= 1,
	eJpMediaType_RADIO 			= 2,
	eJpMediaType_DATA				= 3,
	eJpMediaType_UNKNOWN			= 4,
} XdbSvc_JapanMediaType_e;

typedef enum
{
	eJpRestrictType_NONE 		= 0x0000,
	eJpRestrictType_LOCK		= 0x0001,		/* Channel Lock */
	eJpRestrictType_SKIP		= 0x0002,		/* Channel Skip */
} XdbSvc_JapanRestrictType_e;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

#if defined(CONFIG_OP_FREESAT)
extern HERROR	XDB_SVC_DeleteANotUsedTsInfo_Freesat(void);
extern HUINT32	XDB_SVC_GetFtaContentMgrValue_Freesat(Handle_t hSvc);
extern HERROR	XDB_SVC_GetHomeAntInfo_Freesat(DbSvc_AntInfo_t *pstAntInfo);
extern HERROR	XDB_SVC_FindServiceListByFsatIdf_Freesat(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, DbSvc_VisibleType_e eVisibleType, HUINT16 usFSatIdf, HUINT32 *pulSvcCount, Handle_t **pphSvcList);
extern HERROR	XDB_SVC_UpdateGroupInfoByGroupIdAndLangCode_Freesat (HUINT32 ulGroupId, HUINT8 *szIso639LangCode, DbSvc_GroupInfo_t *pstGroupInfo);
extern HERROR	XDB_SVC_GetGroupInfoByGroupIdAndLangCode_Freesat(HUINT32 ulGroupId, HUINT8 *szIso639LangCode, DbSvc_GroupInfo_t *pstGroupInfo);
extern HERROR	XDB_SVC_CountGroup_Freesat (HINT32 *pnCnt);
extern HERROR	XDB_SVC_GetAllGroupInfoList_Freesat (HUINT8 *szIso639LangCode[], HINT32 nLangCount, HINT32 *pnGroupNum, DbSvc_GroupInfo_t **pstGroupInfoList);
extern HERROR	XDB_SVC_GetAllCatchupGroupInfoList_Freesat (HUINT8 *szIso639LangCode[], HINT32 nLangCount, HINT32 *pnGroupNum, DbSvc_GroupInfo_t **pstGroupInfoList);
extern HERROR	XDB_SVC_CountCatchupGroup_Freesat (HINT32 *pnCnt);
extern HERROR	XDB_SVC_GetSvcIdentifier_Freesat(Handle_t hSvc, HUINT16 *pusFsatSvcId);

extern HERROR	XDB_SVC_DuUpdateGroupInfoByGroupIdAndLangCode_Freesat(HUINT32 ulGroupId, HUINT8 *szIso639LangCode, DbSvc_GroupInfo_t *pstGroupInfo);
extern HERROR	XDB_SVC_DuGetGroupInfoByGroupIdAndLangCode_Freesat(HUINT32 ulGroupId, HUINT8 *szIso639LangCode, DbSvc_GroupInfo_t *pstGroupInfo);
extern HERROR	XDB_SVC_GetGroupInfoByGroupId_Freesat (HUINT32 ulGroupId, DbSvc_GroupInfo_t *pstGroupInfo);
extern HERROR	XDB_SVC_AddGroupInfo_Freesat (DbSvc_GroupInfo_t *pstGroupInfo, HUINT16 *pusGroupIdx);
extern HERROR	XDB_SVC_DeleteAllGroupInfo_Freesat(void);
extern HERROR	XDB_SVC_DoUpdateGroupInfo_Freesat (HBOOL* bUpdated);
extern HERROR	XDB_SVC_UpdateGroupInfo_Freesat (HUINT16 usGroupIdx, DbSvc_GroupInfo_t *pstGroupInfo);
extern HUINT16	XDB_SVC_GetLangPriority_Freesat(HUINT8 *szLangCodeSelect[], HINT32 nLangCount, HUINT8 *szLangCodeData);
#endif

#endif /* !__XDB_SVC_H__ */

