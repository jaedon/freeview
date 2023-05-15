/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_svc.h
	@brief	  service db

	Description: service/ts/network/provider/bouquet 등 PSI/SI를 가공하여 만들어지는 모든 service data는 이곳을 통하여 저장/삭제/업데이트 된다. \ㅜ
	             그리고 필요한 정보를 얻어갈 수 있는 API제공한다.
	Module: DB - SVC
	Remarks :

	Copyright (c) 2008 HUMAX Co., Ltd.
	All rights reserved.
*/

#ifndef	__DB_SVC_H__
#define	__DB_SVC_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <dapi.h>

#include <octo_common.h> /** @brief Handle_t를 사용하기 위함 */

#include <db_common.h>
#include <db_param.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*
	== defconfig에 정의되어 있는 값들 ==
	1. max service number : CONFIG_MAX_SERVICE_NUM를 사용하며, 위성 모델은 5000개까지 지원한다
	2. max service name len :
			#define CONFIG_DB_SVC_MAX_NAME_BYTES 34
			저장 시점에 satellite service는 길이를 제한한다.
	3. 아래 값들도 defconfig에 추가해도 되나 아직은 필요성 못느낌
*/

#define SERVICE_TYPE_TV_RADIO		(eDxSVC_TYPE_TV | eDxSVC_TYPE_RADIO)
#define SERVICE_TYPE_TV_RADIO_DATA	(eDxSVC_TYPE_TV | eDxSVC_TYPE_RADIO | eDxSVC_TYPE_DATA)
#ifdef CONFIG_MW_CI_PLUS
#define MAX_CAM_BRAND_NUM				255
#define MAX_CAM_OPERATOR_NAME_LEN		256
#define CICAM_ID_LEN					8
#endif

#define SUBTITLE_OFF_IDX				255
#define SUBTITLE_TEMP_OFF_IDX			254

#if defined (CONFIG_OP_SES)
//#define	DbSvc_SUPPORT_DXINFO
#else
#define	DbSvc_SUPPORT_DXINFO
#endif

// Svc Index
#define	DbSvc_GetSvcIdx(pstSvcInfo)							((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.uid)
#define	DbSvc_SetSvcIdx(pstSvcInfo, ulValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.uid = (HINT32)ulValue)
// Ts Index
#define	DbSvc_GetTsIdx(pstSvcInfo)							((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.tsuid)
#define	DbSvc_SetTsIdx(pstSvcInfo, ulValue)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.tsuid = (HINT32)ulValue)
// Prv Index
#define	DbSvc_GetPrvIdx(pstSvcInfo)							((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.prvuid)
#define	DbSvc_SetPrvIdx(pstSvcInfo, ulValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.prvuid = (HINT32)ulValue)
// Svc Id
#define	DbSvc_GetSvcId(pstSvcInfo)							((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.svcid)
#define	DbSvc_SetSvcId(pstSvcInfo, nValue)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.svcid = (HINT32)nValue)
// Ts Id
#define	DbSvc_GetTsId(pstSvcInfo)							((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.tsid)
#define	DbSvc_SetTsId(pstSvcInfo, nValue)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.tsid = (HINT32)nValue)
// On Id
#define	DbSvc_GetOnId(pstSvcInfo)							((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.onid)
#define	DbSvc_SetOnId(pstSvcInfo, nValue)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.onid = (HINT32)nValue)
// LCN
#define	DbSvc_GetLcn(pstSvcInfo)							((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.lcn)
#define	DbSvc_SetLcn(pstSvcInfo, nValue)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.lcn = (HINT32)nValue)
// LCN Flag
#define	DbSvc_GetLcnFlag(pstSvcInfo)						(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.lcnFlag)
#define	DbSvc_SetLcnFlag(pstSvcInfo, bValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.lcnFlag = bValue)
// Visible Flag
#define	DbSvc_GetVisibleFlag(pstSvcInfo)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.visibleFlag)
#define	DbSvc_SetVisibleFlag(pstSvcInfo, bValue)			(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.visibleFlag = bValue)
// Svc Name
#define	DbSvc_GetName(pstSvcInfo)							(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.name)
#define	DbSvc_SetName(pstSvcInfo, pszName)					HLIB_STD_StrUtf8NCpy(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.name, (const HCHAR *)pszName, DxNAME_LEN)
#define	DbSvc_GetNameLength()								(CONFIG_DB_SVC_MAX_NAME_BYTES)

#if defined(CONFIG_OP_NORDIG)
// Simulcast
#define DbSvc_GetSimulcastOnId(pstSvcInfo)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.simulcastOnid)
#define DbSvc_GetSimulcastTsId(pstSvcInfo)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.simulcastTsid)
#define DbSvc_GetSimulcastSvcId(pstSvcInfo)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.simulcastSvcid)
#define DbSvc_SetSimulcastOnId(pstSvcInfo, nValue)			(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.simulcastOnid = (HINT32)nValue)
#define DbSvc_SetSimulcastTsId(pstSvcInfo, nValue)			(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.simulcastTsid = (HINT32)nValue)
#define DbSvc_SetSimulcastSvcId(pstSvcInfo, nValue)			(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.simulcastSvcid = (HINT32)nValue)
#endif

/*	PES Info
 */
// PMT Pid
#define	DbSvc_GetPmtPid(pstSvcInfo)							((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.pmtPid)
#define	DbSvc_SetPmtPid(pstSvcInfo, nValue)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.pmtPid = (HINT32)nValue)
// PCR Pid
#define	DbSvc_GetPcrPid(pstSvcInfo)							((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.pcrPid)
#define	DbSvc_SetPcrPid(pstSvcInfo, nValue)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.pcrPid = (HINT32)nValue)
// Video Pid
#define	DbSvc_GetVideoPid(pstSvcInfo)						((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.videoPid)
#define	DbSvc_SetVideoPid(pstSvcInfo, nValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.videoPid = (HINT32)nValue)
// Audio Pid
#define	DbSvc_GetAudioPid(pstSvcInfo)						((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.audioPid)
#define	DbSvc_SetAudioPid(pstSvcInfo, nValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.audioPid = (HINT32)nValue)
// Audio Aux Pid
#define	DbSvc_GetAudioAuxPid(pstSvcInfo)					((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.audioAuxPid)
#define	DbSvc_SetAudioAuxPid(pstSvcInfo, nValue)			(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.audioAuxPid = (HINT32)nValue)
// Dolby Pid
#define	DbSvc_GetDolbyPid(pstSvcInfo)						((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.dolbyPid)
#define	DbSvc_SetDolbyPid(pstSvcInfo, nValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.dolbyPid = (HINT32)nValue)
// Teletext Pid
#define	DbSvc_GetTtxPid(pstSvcInfo)							((HUINT16)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ttxPid)
#define	DbSvc_SetTtxPid(pstSvcInfo, nValue)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.ttxPid = (HINT32)nValue)
// Subtitle Track
#define	DbSvc_GetSubttlIdx(pstSvcInfo)						((HUINT8)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.subtitleTrack)
#define	DbSvc_SetSubttlIdx(pstSvcInfo, nValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.subtitleTrack = (HINT32)nValue)
// Subtitle Flag
#define	DbSvc_GetSubttlFlag(pstSvcInfo)						(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.subtitle)
#define	DbSvc_SetSubttlFlag(pstSvcInfo, bValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.subtitle = bValue)
// Subtitle Language
#define	DbSvc_GetSubttlLanguage(pstSvcInfo)					((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.subtitleLanguage
#define	DbSvc_SetSubttlLanguage(pstSvcInfo, szValue)		HLIB_STD_StrNCpySafe(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.subtitleLanguage, szValue, DxLANG_CODE_LEN)
// Hard Of Hearing
#define	DbSvc_GetHohFlag(pstSvcInfo)						((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.hardOfHearing
#define	DbSvc_SetHohFlag(pstSvcInfo, bValue)				((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.hardOfHearing = (HBOOL)bValue
// Video Codec
#define	DbSvc_GetVideoCodec(pstSvcInfo)						(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.videoCodec)
#define	DbSvc_SetVideoCodec(pstSvcInfo, eValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.videoCodec = eValue)
// Audio Codec
#define	DbSvc_GetAudioCodec(pstSvcInfo)						(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.audioCodec)
#define	DbSvc_SetAudioCodec(pstSvcInfo, eValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.audioCodec = eValue)
// Audio Aux Codec
#define	DbSvc_GetAudioAuxCodec(pstSvcInfo)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.audioAuxCodec)
#define	DbSvc_SetAudioAuxCodec(pstSvcInfo, eValue)			(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.audioAuxCodec = eValue)
// Dolby Codec
#define	DbSvc_GetDolbyCodec(pstSvcInfo)						(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.dolbyCodec)
#define	DbSvc_SetDolbyCodec(pstSvcInfo, eValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.dolbyCodec = eValue)
// Dolby Flag
#define	DbSvc_GetDolbyFlag(pstSvcInfo)						(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.dolbyFlag)
#define	DbSvc_SetDolbyFlag(pstSvcInfo, bValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.dolbyFlag = bValue)
// Audio Description Flag
#define	DbSvc_GetAdFlag(pstSvcInfo)							(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.audioDescription)
#define	DbSvc_SetAdFlag(pstSvcInfo, bValue)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.audioDescription = bValue)
// Svc Type
#define	DbSvc_GetSvcType(pstSvcInfo)						(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.svcType)
#define	DbSvc_SetSvcType(pstSvcInfo,eValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.svcType = (DxSvcType_e)eValue)
// Original Service Type
#define	DbSvc_GetOrgSvcType(pstSvcInfo)						(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.orgSvcType)
#define	DbSvc_SetOrgSvcType(pstSvcInfo, eValue)			(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.orgSvcType = (DxOrgSvcType_e)eValue)
// Video Type
#define	DbSvc_GetVideoType(pstSvcInfo)						(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.highDefinition)
#define	DbSvc_SetVideoType(pstSvcInfo, eValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.highDefinition = (DxVideoType_e)eValue)
// Cas Type
#define	DbSvc_GetCasType(pstSvcInfo)							(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.casType)
#define	DbSvc_SetCasType(pstSvcInfo, eValue)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.casType = (DxCasType_b)eValue)
// Svc Section
#define	DbSvc_GetSvcSection(pstSvcInfo)						(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.svcSection)
#define	DbSvc_SetSvcSection(pstSvcInfo, eValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.svcSection = (DxSvcSection_e)eValue)
// Opertor Type
#define	DbSvc_GetOpType(pstSvcInfo)							(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.optype)
#define	DbSvc_SetOpType(pstSvcInfo, eValue)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.optype = (DxOperator_e)eValue)
// Delivery Type
#define	DbSvc_GetDeliType(stSvcInfo)						(stSvcInfo.stDxInfo.deliType)
#define	DbSvc_GetDeliTypePtr(pstSvcInfo)					(pstSvcInfo->stDxInfo.deliType)
#define	DbSvc_SetDeliType(stSvcInfo, eValue)				(stSvcInfo.stDxInfo.deliType = eValue)
#define	DbSvc_SetDeliTypePtr(pstSvcInfo, eValue)			(pstSvcInfo->stDxInfo.deliType = eValue)
// Antenna Index
#define	DbSvc_GetAntIdx(pstSvcInfo)							((HUINT8)((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.antuid)
#define	DbSvc_SetAntIdx(pstSvcInfo, nValue)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.antuid = (HINT32)nValue)
// Sat Type
#define	DbSvc_GetSatType(pstSvcInfo)						OTL_CONV_SatString2SVC_SatType(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.satType)
#define	DbSvc_SetSatType(pstSvcInfo, eValue)				HLIB_STD_StrNCpySafe(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.satType, OTL_CONV_SVC_SatType2String(eValue), DxANTINFO_NAME_LEN)
// Renamed
#define	DbSvc_GetRenamed(pstSvcInfo)							(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.renamed)
#define	DbSvc_SetRenamed(pstSvcInfo, bValue)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.renamed = (HBOOL)bValue)
// Locked
#define	DbSvc_GetLocked(pstSvcInfo)							(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.locked)
#define	DbSvc_SetLocked(pstSvcInfo, bValue)					(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.locked = (HBOOL)bValue)
// StereoSelect
#define	DbSvc_GetStereoSelect(pstSvcInfo)					((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.audioTrack
#define	DbSvc_SetStereoSelect(pstSvcInfo, eValue)			((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.audioTrack = (DxStereoSelect_e)eValue
// Dualmono Select
#define	DbSvc_GetDualmonoSelect(pstSvcInfo)					((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.dualMonoSelect
#define	DbSvc_SetDualmonoSelect(pstSvcInfo, eValue)			((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.dualMonoSelect = (DxDualMonoSelect_e)eValue
// Audio Language
#define	DbSvc_GetAudioLanguage(pstSvcInfo)					((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.audioLanguage
#define	DbSvc_SetAudioLanguage(pstSvcInfo, szValue)			HLIB_STD_StrNCpySafe(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.audioLanguage, szValue, DxLANG_CODE_LEN)
// AudUserSelected
#define	DbSvc_GetAudUserSelected(pstSvcInfo)				((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.audio
#define	DbSvc_SetAudUserSelected(pstSvcInfo, bValue)		((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.audio = (HBOOL)bValue
// RatingPassedEventId
#define	DbSvc_GetRatingPassedEvent(pstSvcInfo)				((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.ratingPassedEventId
#define	DbSvc_SetRatingPassedEvent(pstSvcInfo, ulValue)		((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.user.ratingPassedEventId = (HUINT32)ulValue
// Svc Removed
#define	DbSvc_GetSvcRemoved(pstSvcInfo)						(HBOOL)(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.removed)
#define	DbSvc_SetSvcRemoved(pstSvcInfo, bValue)				((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.removed = (HBOOL)bValue
// Svc Updated
#define	DbSvc_GetSvcUpdated(pstSvcInfo)					(HBOOL)(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.updateSvcFlag)
#define	DbSvc_SetSvcUpdated(pstSvcInfo, bValue)			(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.updateSvcFlag = (HBOOL)bValue)
// Bouquet
#define DbSvc_GetBouquetId(pstSvcInfo, ulIndex)				((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.bqtuids[ulIndex]
#define	DbSvc_SetBouquetId(pstSvcInfo, ulIndex, nValue)		((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.bqtuids[ulIndex] = (HINT32)nValue
// Group
#define DbSvc_GetGroupId(pstSvcInfo, ulIndex)				((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.grpuids[ulIndex]
#define	DbSvc_SetGroupId(pstSvcInfo, ulIndex, nValue)		((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.grpuids[ulIndex] = (HINT32)nValue


#if defined(CONFIG_MW_CI)
// CI Inst Type
#define	DbSvc_GetCiInstType(pstSvcInfo)						(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.cas.instType)
#define	DbSvc_SetCiInstType(pstSvcInfo, ulValue)			((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.cas.instType = (HUINT32)ulValue

// CAM Slot Id
#define	DbSvc_GetCamSlotId(pstSvcInfo)						(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.cas.slotId)
#define	DbSvc_SetCamSlotId(pstSvcInfo, ulValue)				((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.cas.slotId = (HUINT32)ulValue

#if defined(CONFIG_MW_CI_PLUS)
// CI Selectable Svc Flag
#define	DbSvc_GetSelectablSvcFlag(pstSvcInfo)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.cas.selectableFlag)
#define	DbSvc_SetSelectablSvcFlag(pstSvcInfo, bValue)		((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.cas.selectableFlag = (HBOOL)bValue

// CI OperatorName
#define	DbSvc_GetCamOperatorName(pstSvcInfo)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.cas.operatorName)
#define	DbSvc_SetCamOperatorName(pstSvcInfo, szValue)		HLIB_STD_StrNCpySafe(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.cas.operatorName, szValue, DxNAME_LEN)

// CAM ID
#define	DbSvc_GetCamId(pstSvcInfo)							(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.cas.camId)
#define	DbSvc_SetCamId(pstSvcInfo, aucValue)				HxSTD_MemCopy(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.cas.camId, aucValue, DxCAMID_LEN)
#endif
#endif

// Original LCN
#define	DbSvc_GetOrgLcn(pstSvcInfo)							(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.orglcn)
#define	DbSvc_SetOrgLcn(pstSvcInfo, usValue)				(((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.orglcn = usValue)


// Tmp
#define	DbSvc_GetXXX(pstSvcInfo)							((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.xxx)
#define	DbSvc_SetXXX(pstSvcInfo, value)						((DbSvc_Info_t *)pstSvcInfo)->stDxInfo.xxx = value)


#if !defined(CONFIG_MW_CI)
// CI Inst Type
#define	DbSvc_GetCiInstType(pstSvcInfo)
#define	DbSvc_SetCiInstType(pstSvcInfo, ulCiInstType)

// CAM Slot Id
#define	DbSvc_GetCamSlotId(pstSvcInfo)
#define	DbSvc_SetCamSlotId(pstSvcInfo, usCamSlotId)
#endif

#if defined(CONFIG_MW_CI_PLUS)
// CI Time Stamp
#define	DbSvc_GetCiTimeStamp(pstSvcInfo)					(((DbSvc_Info_t *)pstSvcInfo)->ulTimeStamp)
#define	DbSvc_SetCiTimeStamp(pstSvcInfo, ulValue)			(((DbSvc_Info_t *)pstSvcInfo)->ulTimeStamp = (HUINT32)ulValue)

// CI Number Of Entries
#define	DbSvc_GetCiNumOfEntries(pstSvcInfo)					(((DbSvc_Info_t *)pstSvcInfo)->ucNumOfEntries)
#define	DbSvc_SetCiNumOfEntries(pstSvcInfo, ucValue)		(((DbSvc_Info_t *)pstSvcInfo)->ucNumOfEntries = (HUINT8)ucValue)

// CAM Brand Id
#define	DbSvc_GetCamBrandId(pstSvcInfo)						(((DbSvc_Info_t *)pstSvcInfo)->ausCamBrandId)
#define	DbSvc_SetCamBrandId(pstSvcInfo, ausId, ulSize)		HxSTD_MemCopy(((DbSvc_Info_t *)pstSvcInfo)->ausCamBrandId, ausId, (((ulSize) > (sizeof(HUINT16)*MAX_CAM_BRAND_NUM)) ? (sizeof(HUINT16)*MAX_CAM_BRAND_NUM) : (ulSize)))
#else
// CI Time Stamp
#define	DbSvc_GetCiTimeStamp(pstSvcInfo)
#define	DbSvc_SetCiTimeStamp(pstSvcInfo, ulValue)

// CI Number Of Entries
#define	DbSvc_GetCiNumOfEntries(pstSvcInfo)
#define	DbSvc_SetCiNumOfEntries(pstSvcInfo, ucValue)

// CAM Brand Id
#define	DbSvc_GetCamBrandId(pstSvcInfo)
#define	DbSvc_SetCamBrandId(pstSvcInfo, ausValue, ulSize)

// CI Selectable Svc Flag
#define	DbSvc_GetSelectablSvcFlag(pstSvcInfo)
#define	DbSvc_SetSelectablSvcFlag(pstSvcInfo, bValue)

// CI OperatorName
#define	DbSvc_GetCamOperatorName(pstSvcInfo)
#define	DbSvc_SetCamOperatorName(pstSvcInfo, szValue)

// CAM ID
#define	DbSvc_GetCamId(pstSvcInfo)
#define	DbSvc_SetCamId(pstSvcInfo, aucValue)
#endif

// Free Ci Mode		eSvcUserFlag_11_FreeCiMode
#define	DbSvc_GetFreeCiModeFlag(pstSvcInfo)					(HBOOL)((((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 & eSvcUserFlag_11_FreeCiMode) ? TRUE : FALSE)
#define	DbSvc_SetFreeCiModeFlag(pstSvcInfo, bValue)			((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 = (bValue) ? (((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 | eSvcUserFlag_11_FreeCiMode) : (((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 & ~eSvcUserFlag_11_FreeCiMode)

// Volatile Service Flag 	eSvcUserFlag_13_VolatileSvc
#define	DbSvc_GetVolatileSvcFlag(pstSvcInfo)				(HBOOL)((((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 & eSvcUserFlag_13_VolatileSvc) ? TRUE : FALSE)
#define	DbSvc_SetVolatileSvcFlag(pstSvcInfo, bValue)		((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 = (bValue) ? (((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 | eSvcUserFlag_13_VolatileSvc) : (((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 & ~eSvcUserFlag_13_VolatileSvc)

// Parental Control Flag	eSvcUserFlag_20_ParentalControl
#define	DbSvc_GetParentalControlFlag(pstSvcInfo)			((HBOOL)(((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 & eSvcUserFlag_20_ParentalControl) ? TRUE : FALSE)
#define	DbSvc_SetParentalControlFlag(pstSvcInfo, bValue)	((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 = (bValue) ? (((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 | eSvcUserFlag_20_ParentalControl) : (((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 & ~eSvcUserFlag_20_ParentalControl)

// Svc Conflicted
#define	DbSvc_GetSvcConflictFlag(pstSvcInfo)				(HBOOL)((((DbSvc_Info_t*)pstSvcInfo)->eUserFlag1 & eSvcUserFlag_31_SvcConflictLcn) ? TRUE : FALSE)
#define	DbSvc_SetSvcConflictFlag(pstSvcInfo, bValue)		((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 = (bValue) ? (((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 | eSvcUserFlag_31_SvcConflictLcn) : (((DbSvc_Info_t *)pstSvcInfo)->eUserFlag1 & ~eSvcUserFlag_31_SvcConflictLcn)

#define	DbSvc_GetUser1Flag(bwUserFlag1, eField)				((bwUserFlag1 & eField) ? TRUE : FALSE)
#define	DbSvc_SetUser1Flag(bwUserFlag1, eField, bSet)		(bwUserFlag1 = (bSet) ? (bwUserFlag1 | eField) : (bwUserFlag1  & ~eField))


/*		Tuning Info
 */
#if 0
#if 0
#if defined(DbSvc_SUPPORT_DXINFO)
#define DbSvc_TuneGetDeliveryType(pstInfo)									(DxNetTuneInfo_t)(pstInfo->stDxInfo).eDeliType
#define DbSvc_TuneSetDeliveryType(pstInfo, eValue)							(DxNetTuneInfo_t)(pstInfo->stDxInfo).eDeliType = eValue

#define DbSvc_TuneGetTunerGroupId(pstInfo)									(DxNetTuneInfo_t)(pstInfo->stDxInfo).ulTunerId
#define DbSvc_TuneSetTunerGroupId(pstInfo, ulValue)							(DxNetTuneInfo_t)(pstInfo->stDxInfo).ulTunerId = ulValue
#else
#define DbSvc_TuneGetDeliveryType(pstInfo)
#define DbSvc_TuneSetDeliveryType(pstInfo, eValue)

#define DbSvc_TuneGetTunerGroupId(pstInfo)
#define DbSvc_TuneSetTunerGroupId(pstInfo, ulValue)
#endif
#endif
#if defined(DbSvc_SUPPORT_DXINFO)
#define DbSvc_TuneSatGetRfInputId(pstInfo)									((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.ulRfInputId
#define DbSvc_TuneSatSetRfInputId(pstInfo, ulValue)							((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.ulRfInputId = ulValue

#define DbSvc_TuneSatGetAntennaType(pstInfo)								((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.antennaType
#define DbSvc_TuneSatSetAntennaType(pstInfo, eValue)						((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.antennaType = eValue

#define DbSvc_TuneSatGetFrequency(pstInfo)									((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.ulFrequency
#define DbSvc_TuneSatSetFrequency(pstInfo, ulValue)							((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.ulFrequency = ulValue

#define DbSvc_TuneSatGetSymbolRate(pstInfo)									((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.ulSymbolRate
#define DbSvc_TuneSatSetSymbolRate(pstInfo, ulValue)						((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.ulSymbolRate = ulValue

#define DbSvc_TuneSatGetFecCodeRate(pstInfo)								((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.eFecCodeRate
#define DbSvc_TuneSatSetFecCodeRate(pstInfo, eValue)						((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.eFecCodeRate = eValue

#define DbSvc_TuneSatGetPolarization(pstInfo)								((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.ePolarization
#define DbSvc_TuneSatSetPolarization(pstInfo, eValue)						((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.ePolarization = eValue

#define DbSvc_TuneSatGetTransSpec(pstInfo)									((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.eTransSpec
#define DbSvc_TuneSatSetTransSpec(pstInfo, eValue)							((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.eTransSpec = eValue

#define DbSvc_TuneSatGetPskModulation(pstInfo)								((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.ePskMod
#define DbSvc_TuneSatSetPskModulation(pstInfo, eValue)						((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.ePskMod = eValue

#define DbSvc_TuneSatGetPilot(pstInfo)										((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.ePilot
#define DbSvc_TuneSatSetPilot(pstInfo, eValue)								((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.ePilot = eValue

#define DbSvc_TuneSatGetRollOff(pstInfo)									((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.eRollOff
#define DbSvc_TuneSatSetRollOff(pstInfo, eValue)							((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.eRollOff = eValue

#define DbSvc_TuneSatGetSatType(pstInfo)									OTL_CONV_SatString2SVC_SatType((const HCHAR *)((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.satType)
#define DbSvc_TuneSatSetSatType(pstInfo, eValue)							HLIB_STD_StrNCpySafe ((HCHAR *)((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.satType, (const HCHAR *)OTL_CONV_SVC_SatType2String(eValue), DxANTINFO_NAME_LEN)

#define DbSvc_TuneSatGetAntId(pstInfo)										(HUINT8)((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.antuid
#define DbSvc_TuneSatSetAntId(pstInfo, ulValue)								((DbSvc_SatTuningInfo_t *)pstInfo)->stDxInfo.tuningInfo.antuid = (HUINT32)ulValue
#else
#define DbSvc_TuneSatGetRfInputId(pstInfo)
#define DbSvc_TuneSatSetRfInputId(pstInfo, ulValue)

#define DbSvc_TuneSatGetAntennaType(pstInfo)
#define DbSvc_TuneSatSetAntennaType(pstInfo, eValue)

#define DbSvc_TuneSatGetFrequency(pstInfo)									((DbSvc_SatTuningInfo_t *)pstInfo)->ulFrequency
#define DbSvc_TuneSatSetFrequency(pstInfo, ulValue)							((DbSvc_SatTuningInfo_t *)pstInfo)->ulFrequency = ulValue

#define DbSvc_TuneSatGetSymbolRate(pstInfo)									((DbSvc_SatTuningInfo_t *)pstInfo)->ulSymbolRate
#define DbSvc_TuneSatSetSymbolRate(pstInfo, ulValue)						((DbSvc_SatTuningInfo_t *)pstInfo)->ulSymbolRate = ulValue

#define DbSvc_TuneSatGetFecCodeRate(pstInfo)								((DbSvc_SatTuningInfo_t *)pstInfo)->eFecCodeRate
#define DbSvc_TuneSatSetFecCodeRate(pstInfo, eValue)						((DbSvc_SatTuningInfo_t *)pstInfo)->eFecCodeRate = eValue

#define DbSvc_TuneSatGetPolarization(pstInfo)								((DbSvc_SatTuningInfo_t *)pstInfo)->ePolarization
#define DbSvc_TuneSatSetPolarization(pstInfo, eValue)						((DbSvc_SatTuningInfo_t *)pstInfo)->ePolarization = eValue

#define DbSvc_TuneSatGetTransSpec(pstInfo)									((DbSvc_SatTuningInfo_t *)pstInfo)->eTransSpec
#define DbSvc_TuneSatSetTransSpec(pstInfo, eValue)							((DbSvc_SatTuningInfo_t *)pstInfo)->eTransSpec = eValue

#define DbSvc_TuneSatGetPskModulation(pstInfo)								((DbSvc_SatTuningInfo_t *)pstInfo)->ePskMod
#define DbSvc_TuneSatSetPskModulation(pstInfo, eValue)						((DbSvc_SatTuningInfo_t *)pstInfo)->ePskMod = eValue

#define DbSvc_TuneSatGetPilot(pstInfo)										((DbSvc_SatTuningInfo_t *)pstInfo)->ePilot
#define DbSvc_TuneSatSetPilot(pstInfo, eValue)								((DbSvc_SatTuningInfo_t *)pstInfo)->ePilot = eValue

#define DbSvc_TuneSatGetRollOff(pstInfo)									((DbSvc_SatTuningInfo_t *)pstInfo)->eRollOff
#define DbSvc_TuneSatSetRollOff(pstInfo, eValue)							((DbSvc_SatTuningInfo_t *)pstInfo)->eRollOff = eValue

#define DbSvc_TuneSatGetSatType(pstInfo)									((DbSvc_SatTuningInfo_t *)pstInfo)->eSatType
#define DbSvc_TuneSatSetSatType(pstInfo, eValue)							((DbSvc_SatTuningInfo_t *)pstInfo)->eSatType = eValue

#define DbSvc_TuneSatGetAntId(pstInfo)										((DbSvc_SatTuningInfo_t *)pstInfo)->ucAntId
#define DbSvc_TuneSatSetAntId(pstInfo, ucValue)								((DbSvc_SatTuningInfo_t *)pstInfo)->ucAntId = (HUINT8)ucValue
#endif

#if defined(DbSvc_SUPPORT_DXINFO)
#define DbSvc_TuneCabGetFrequency(pstInfo)									((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.ulFrequency
#define DbSvc_TuneCabSetFrequency(pstInfo, ulValue)							((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.ulFrequency = (HUINT32)ulValue

#define DbSvc_TuneCabGetSymbolRate(pstInfo)									((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.ulSymbolRate
#define DbSvc_TuneCabSetSymbolRate(pstInfo, ulValue)						((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.ulSymbolRate = (HUINT32)ulValue

#define DbSvc_TuneCabGetNetworkId(pstInfo)									((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.nNetId
#define DbSvc_TuneCabSetNetworkId(pstInfo, nValue)							((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.nNetId = (HINT32)nValue

#define DbSvc_TuneCabGetConstellation(pstInfo)								((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.constellation
#define DbSvc_TuneCabSetConstellation(pstInfo, eValue)						((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.constellation = (DxCab_Constellation_e)eValue

#define DbSvc_TuneCabGetSpectrum(pstInfo)									((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.spectrum
#define DbSvc_TuneCabSetSpectrum(pstInfo, eValue)							((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.spectrum = (DxCab_Spectrum_e)eValue

#define DbSvc_TuneCabGetTsId(pstInfo)										((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.nTsId
#define DbSvc_TuneCabSetTsId(pstInfo, nValue)								((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.nTsId = (HINT32)nValue
#else
#define DbSvc_TuneCabGetFrequency(pstInfo)									((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.ulFreq
#define DbSvc_TuneCabSetFrequency(pstInfo, ulValue)							((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.ulFreq = (HUINT32)ulValue

#define DbSvc_TuneCabGetSymbolRate(pstInfo)									((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.usSR
#define DbSvc_TuneCabSetSymbolRate(pstInfo, usValue)						((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.usSR = (HUINT16)usValue

#define DbSvc_TuneCabGetNetworkId(pstInfo)									((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.usNetworkId
#define DbSvc_TuneCabSetNetworkId(pstInfo, usValue)							((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.usNetworkId = (HUINT16)usValue

#define DbSvc_TuneCabGetConstellation(pstInfo)								((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.ucCon
#define DbSvc_TuneCabSetConstellation(pstInfo, ucValue)						((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.ucCon = (HUINT8)ucValue

#define DbSvc_TuneCabGetSpectrum(pstInfo)									((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.ucSpec
#define DbSvc_TuneCabSetSpectrum(pstInfo, ucValue)							((DbSvc_CabTuningInfo_t *)pstInfo)->stDxInfo.ucSpec = (HUINT8)ucValue

#define DbSvc_TuneCabGetTsId(pstInfo)
#define DbSvc_TuneCabSetTsId(pstInfo, ucValue)
#endif

#if defined(DbSvc_SUPPORT_DXINFO)
#define DbSvc_TuneTerGetFrequency(pstInfo)									((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.ulFrequency
#define DbSvc_TuneTerSetFrequency(pstInfo, ulValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.ulFrequency = ulValue

#define DbSvc_TuneTerGetBandWidth(pstInfo)									((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eBandWidth
#define DbSvc_TuneTerSetBandWidth(pstInfo, eValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eBandWidth = (DxTer_BandWidth_e)eValue

#define DbSvc_TuneTerGetConstellation(pstInfo)								((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eConstellation
#define DbSvc_TuneTerSetConstellation(pstInfo, eValue)						((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eConstellation = (DxTer_Constellation_e)eValue

#define DbSvc_TuneTerGetHierachy(pstInfo)									((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eHierachy
#define DbSvc_TuneTerSetHierachy(pstInfo, eValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eHierachy = (DxTer_Hierachy_e)eValue

#define DbSvc_TuneTerGetStream(pstInfo)										((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eStream
#define DbSvc_TuneTerSetStream(pstInfo, eValue)								((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eStream = (DxTer_Stream_e)eValue

#define DbSvc_TuneTerGetCodeRate(pstInfo)									((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eCodeRate
#define DbSvc_TuneTerSetCodeRate(pstInfo, eValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eCodeRate = (DxTer_CodeRate_e)eValue

#define DbSvc_TuneTerGetGuardInterval(pstInfo)								((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eGuardInterval
#define DbSvc_TuneTerSetGuardInterval(pstInfo, eValue)						((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eGuardInterval = (DxTer_GuardInterval_e)eValue

#define DbSvc_TuneTerGetTransmode(pstInfo)									((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eTransMode
#define DbSvc_TuneTerSetTransmode(pstInfo, eValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eTransMode = (DxTer_TransMode_e)eValue

#define DbSvc_TuneTerGetOffset(pstInfo)										((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eOffset
#define DbSvc_TuneTerSetOffset(pstInfo, eValue)								((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eOffset = eValue

	/* DVB-T2 */
#define DbSvc_TuneTerGetSystem(pstInfo)										((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eSystem
#define DbSvc_TuneTerSetSystem(pstInfo, eValue)								((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.eSystem = (DxTer_System_e)eValue

#define DbSvc_TuneTerGetPlpId(pstInfo)										((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.stT2Param.ulPlpId
#define DbSvc_TuneTerSetPlpId(pstInfo, ulValue)								((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.stT2Param.ulPlpId = ulValue

#define DbSvc_TuneTerGetPreambleFormat(pstInfo)								((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.stT2Param.preambleFormat
#define DbSvc_TuneTerSetPreambleFormat(pstInfo, eValue)						((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.stT2Param.preambleFormat = (DxTer_PreambleFormat_e)eValue

#define DbSvc_TuneTerGetMixed(pstInfo)										((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.stT2Param.mixed
#define DbSvc_TuneTerSetMixed(pstInfo, eValue)								((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.stT2Param.mixed = (DxTer_Mixed_e)eValue

#define DbSvc_TuneTerGetPilotPattern(pstInfo)								((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.stT2Param.pilotPattern
#define DbSvc_TuneTerSetPilotPattern(pstInfo, eValue)						((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.stT2Param.pilotPattern = (DxTer_Pilot_Pattern_e)eValue

#define DbSvc_TuneTerGetExtendedCarrier(pstInfo)							((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.stT2Param.extenedCarrier
#define DbSvc_TuneTerSetExtendedCarrier(pstInfo, eValue)					((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.stT2Param.extenedCarrier = (DxTer_ExtendedCarrier_e)eValue

#define DbSvc_TuneTerGetPAPRReduction(pstInfo)								((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.stT2Param.PAPRreduction
#define DbSvc_TuneTerSetPAPRReduction(pstInfo, eValue)						((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.stT2Param.PAPRreduction = (DxTer_PAPR_Reduction_e)eValue

#define DbSvc_TuneTerGetPlpBlockNum(pstInfo)								((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.stT2Param.numPlpBlocks
#define DbSvc_TuneTerSetPlpBlockNum(pstInfo, ulValue)						((DbSvc_TerTuningInfo_t *)pstInfo)->stDxInfo.stT2Param.numPlpBlocks = ulValue

#define DbSvc_TuneTerGetChanNum(pstInfo)									((DbSvc_TerTuningInfo_t *)pstInfo)->stCache.ucChanNum
#define DbSvc_TuneTerSetChanNum(pstInfo, ucValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->stCache.ucChanNum = (HUINT8)ucValue

#define DbSvc_TuneTerGetLevel(pstInfo)										((DbSvc_TerTuningInfo_t *)pstInfo)->stCache.ucLevel
#define DbSvc_TuneTerSetLevel(pstInfo, ucValue)								((DbSvc_TerTuningInfo_t *)pstInfo)->stCache.ucLevel = (HUINT8)ucValue

#define DbSvc_TuneTerGetQuality(pstInfo)									((DbSvc_TerTuningInfo_t *)pstInfo)->stCache.ucQuality
#define DbSvc_TuneTerSetQuality(pstInfo, ucValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->stCache.ucQuality = (HUINT8)ucValue
#else
#define DbSvc_TuneTerGetFrequency(pstInfo)									((DbSvc_TerTuningInfo_t *)pstInfo)->ulFreq 			/**< centre_frequency (mutiples of 10Hz) */
#define DbSvc_TuneTerSetFrequency(pstInfo, ulValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->ulFreq = (HUINT32)ulValue 			/**< centre_frequency (mutiples of 10Hz) */

#define DbSvc_TuneTerGetBandWidth(pstInfo)									((DbSvc_TerTuningInfo_t *)pstInfo)->ucBandwidth		/**< bandwidth	   : 8/7/6 MHz */
#define DbSvc_TuneTerSetBandWidth(pstInfo, ucValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->ucBandwidth = (HUINT8)ucValue		/**< bandwidth	   : 8/7/6 MHz */

#define DbSvc_TuneTerGetConstellation(pstInfo)								((DbSvc_TerTuningInfo_t *)pstInfo)->ucConstellation	/**< constellation : QPSK/16-QAM/64-QAM */
#define DbSvc_TuneTerSetConstellation(pstInfo, ucValue)						((DbSvc_TerTuningInfo_t *)pstInfo)->ucConstellation	= (HUINT8)ucValue	/**< constellation : QPSK/16-QAM/64-QAM */

#define DbSvc_TuneTerGetHierachy(pstInfo)									((DbSvc_TerTuningInfo_t *)pstInfo)->ucHierachy 		/**< hierachy information : signalling format for the alpha values */
#define DbSvc_TuneTerSetHierachy(pstInfo, ucValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->ucHierachy = (HUINT8)ucValue 		/**< hierachy information : signalling format for the alpha values */

#define DbSvc_TuneTerGetStream(pstInfo)										((DbSvc_TerTuningInfo_t *)pstInfo)->ucStream			/**< HP/LP */
#define DbSvc_TuneTerSetStream(pstInfo, ucValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->ucStream = (HUINT8)ucValue			/**< HP/LP */

#define DbSvc_TuneTerGetCodeRate(pstInfo)									((DbSvc_TerTuningInfo_t *)pstInfo)->ucCodeRate 		/**< FEC */
#define DbSvc_TuneTerSetCodeRate(pstInfo, ucValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->ucCodeRate = (HUINT8)ucValue 		/**< FEC */

#define DbSvc_TuneTerGetGuardInterval(pstInfo)								((DbSvc_TerTuningInfo_t *)pstInfo)->ucGuardInterval	/**< 1/32, 1/16, 1/8, 1/4 */
#define DbSvc_TuneTerSetGuardInterval(pstInfo, ucValue)						((DbSvc_TerTuningInfo_t *)pstInfo)->ucGuardInterval = (HUINT8)ucValue	/**< 1/32, 1/16, 1/8, 1/4 */

#define DbSvc_TuneTerGetTransmode(pstInfo)									((DbSvc_TerTuningInfo_t *)pstInfo)->ucTransMode		/**< 2k/8k */
#define DbSvc_TuneTerSetTransmode(pstInfo, ucValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->ucTransMode = (HUINT8)ucValue		/**< 2k/8k */

#define DbSvc_TuneTerGetOffset(pstInfo)										((DbSvc_TerTuningInfo_t *)pstInfo)->ucOffset
#define DbSvc_TuneTerSetOffset(pstInfo, ucValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->ucOffset = (HUINT8)ucValue

#if defined(CONFIG_MW_CH_DVB_T2)
#define DbSvc_TuneTerGetSystem(pstInfo)										((DbSvc_TerTuningInfo_t *)pstInfo)->ucSystem		/**< T/T2 */
#define DbSvc_TuneTerSetSystem(pstInfo, ucValue)							((DbSvc_TerTuningInfo_t *)pstInfo)->ucSystem = (HUINT8)ucValue	/**< T/T2 */

#define DbSvc_TuneTerGetPlpId(pstInfo)										((DbSvc_TerTuningInfo_t *)pstInfo)->stT2param.ulPlpId
#define DbSvc_TuneTerSetPlpId(pstInfo, ulValue)								((DbSvc_TerTuningInfo_t *)pstInfo)->stT2param.ulPlpId = ulValue

#define DbSvc_TuneTerGetPreambleFormat(pstInfo)								((DbSvc_TerTuningInfo_t *)pstInfo)->stT2param.ucPreambleFormat
#define DbSvc_TuneTerSetPreambleFormat(pstInfo, ucValue)					((DbSvc_TerTuningInfo_t *)pstInfo)->stT2param.ucPreambleFormat = (HUINT8)ucValue

#define DbSvc_TuneTerGetMixed(pstInfo)										((DbSvc_TerTuningInfo_t *)pstInfo)->stT2param.ucMixed
#define DbSvc_TuneTerSetMixed(pstInfo, ucValue)								((DbSvc_TerTuningInfo_t *)pstInfo)->stT2param.ucMixed = (HUINT8)ucValue

#define DbSvc_TuneTerGetPilotPattern(pstInfo)								((DbSvc_TerTuningInfo_t *)pstInfo)->stT2param.ucPilotPattern
#define DbSvc_TuneTerSetPilotPattern(pstInfo, ucValue)						((DbSvc_TerTuningInfo_t *)pstInfo)->stT2param.ucPilotPattern = (HUINT8)ucValue

#define DbSvc_TuneTerGetExtendedCarrier(pstInfo)							((DbSvc_TerTuningInfo_t *)pstInfo)->stT2param.ucExtenedCarrier
#define DbSvc_TuneTerSetExtendedCarrier(pstInfo, ucValue)					((DbSvc_TerTuningInfo_t *)pstInfo)->stT2param.ucExtenedCarrier = (HUINT8)ucValue

#define DbSvc_TuneTerGetPAPRReduction(pstInfo)								((DbSvc_TerTuningInfo_t *)pstInfo)->stT2param.ucPAPRreduction
#define DbSvc_TuneTerSetPAPRReduction(pstInfo, ucValue)						((DbSvc_TerTuningInfo_t *)pstInfo)->stT2param.ucPAPRreduction = (HUINT8)ucValue

#define DbSvc_TuneTerGetPlpBlockNum(pstInfo)								((DbSvc_TerTuningInfo_t *)pstInfo)->stT2param.ulNumPlpBlocks
#define DbSvc_TuneTerSetPlpBlockNum(pstInfo, ulValue)						((DbSvc_TerTuningInfo_t *)pstInfo)->stT2param.ulNumPlpBlocks = (HUINT32)ulValue
#endif

#define DbSvc_TuneTerGetChanNum(pstExtInfo)									((DbSvc_TerTuningInfo_t *)pstExtInfo)->ucChanNum
#define DbSvc_TuneTerSetChanNum(pstExtInfo, ucValue)						((DbSvc_TerTuningInfo_t *)pstExtInfo)->ucChanNum = (HUINT8)ucValue

#define DbSvc_TuneTerGetLevel(pstExtInfo)									((DbSvc_TerTuningInfo_t *)pstExtInfo)->ucLevel
#define DbSvc_TuneTerSetLevel(pstExtInfo, ucValue)							((DbSvc_TerTuningInfo_t *)pstExtInfo)->ucLevel = (HUINT8)ucValue

#define DbSvc_TuneTerGetQuality(pstExtInfo)									((DbSvc_TerTuningInfo_t *)pstExtInfo)->ucQuality
#define DbSvc_TuneTerSetQuality(pstExtInfo, ucValue)						((DbSvc_TerTuningInfo_t *)pstExtInfo)->ucQuality = (HUINT8)ucValue
#endif

/*		Tuning Info
 */
#define DbSvc_TuneReset(pstTune)											HxSTD_MemSet(pstTune, 0, sizeof(DbSvc_TuningInfo_t))

#if defined(DbSvc_SUPPORT_DXINFO
#define DbSvc_TuneGetSatInfo(pstTune)										((DxSatTuningInfo_t *)&((DbSvc_TuningInfo_t *)pstTune)->sat)
#define DbSvc_TuneGetCabInfo(pstTune)										((DxCabTuningInfo_t *)&((DbSvc_TuningInfo_t *)pstTune)->cab)
#define DbSvc_TuneGetTerInfo(pstTune)										((DxTerTuningInfo_t *)&((DbSvc_TuningInfo_t *)pstTune)->ter)

//	SAT
#define DbSvc_TuneGetSatAntId(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->sat.antInfo.uid
#define DbSvc_TuneSetSatAntId(pstTune, nValue)								((DbSvc_TuningInfo_t *)pstTune)->sat.antInfo.uid = (HINT32)nValue

#define DbSvc_TuneGetSatFrequency(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.ulFrequency
#define DbSvc_TuneSetSatFrequency(pstTune, ulValue)							((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.ulFrequency = ulValue

#define DbSvc_TuneGetSatSymbolRate(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.ulSymbolRate
#define DbSvc_TuneSetSatSymbolRate(pstTune, ulValue)						((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.ulSymbolRate = ulValue

#define DbSvc_TuneGetSatPolarization(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.ePolarization
#define DbSvc_TuneSetSatPolarization(pstTune, eValue)						((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.ePolarization = eValue

#define DbSvc_TuneGetSatFecCodeRate(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.eFecCodeRate
#define DbSvc_TuneSetSatFecCodeRate(pstTune, eValue)						((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.eFecCodeRate = eValue

#define DbSvc_TuneGetSatTransSpec(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.eTransSpec
#define DbSvc_TuneSetSatTransSpec(pstTune, eValue)							((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.eTransSpec = eValue

#define DbSvc_TuneGetSatPskModulation(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.ePskMod
#define DbSvc_TuneSetSatPskModulation(pstTune, eValue)						((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.ePskMod = eValue

#define DbSvc_TuneGetSatPilot(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.ePilot
#define DbSvc_TuneSetSatPilot(pstTune, eValue)								((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.ePilot = eValue

#define DbSvc_TuneGetSatRollOff(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.eRollOff
#define DbSvc_TuneSetSatRollOff(pstTune, eValue)							((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.eRollOff = eValue

#define DbSvc_TuneGetSatType(pstTune)										OTL_CONV_SatString2SVC_SatType((const HCHAR *)((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.satType)
#define DbSvc_TuneSetSatType(pstTune, eValue)								HLIB_STD_StrNCpySafe((HCHAR *)((DbSvc_TuningInfo_t *)pstTune)->sat.tuningInfo.satType, (const HCHAR *)OTL_CONV_SVC_SatType2String(eValue), DxANTINFO_NAME_LEN)

// CAB
#define DbSvc_TuneGetCabFrequency(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->cab.ulFrequency
#define DbSvc_TuneSetCabFrequency(pstTune, ulValue)							((DbSvc_TuningInfo_t *)pstTune)->cab.ulFrequency = (HUINT32)ulValue

#define DbSvc_TuneGetCabSymbolRate(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->cab.ulSymbolRate
#define DbSvc_TuneSetCabSymbolRate(pstTune, ulValue)						((DbSvc_TuningInfo_t *)pstTune)->cab.ulSymbolRate = (HUINT32)ulValue

#define DbSvc_TuneGetCabNetworkId(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->cab.nNetId
#define DbSvc_TuneSetCabNetworkId(pstTune, nValue)							((DbSvc_TuningInfo_t *)pstTune)->cab.nNetId = (HINT32)nValue

#define DbSvc_TuneGetCabConstellation(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->cab.constellation
#define DbSvc_TuneSetCabConstellation(pstTune, eValue)						((DbSvc_TuningInfo_t *)pstTune)->cab.constellation = (DxCab_Constellation_e)eValue

#define DbSvc_TuneGetCabSpectrum(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->cab.spectrum
#define DbSvc_TuneSetCabSpectrum(pstTune, eValue)							((DbSvc_TuningInfo_t *)pstTune)->cab.spectrum = (DxCab_Spectrum_e)eValue

#define DbSvc_TuneGetCabTsId(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->cab.nTsId
#define DbSvc_TuneSetCabTsId(pstTune, nValue)								((DbSvc_TuningInfo_t *)pstTune)->cab.nTsId = (HINT32)nValue

//	TER
#define DbSvc_TuneGetTerFrequency(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->ter.ulFrequency
#define DbSvc_TuneSetTerFrequency(pstTune, ulValue)							((DbSvc_TuningInfo_t *)pstTune)->ter.ulFrequency = ulValue

#define DbSvc_TuneGetTerBandWidth(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->ter.eBandWidth
#define DbSvc_TuneSetTerBandWidth(pstTune, eValue)							((DbSvc_TuningInfo_t *)pstTune)->ter.eBandWidth = (DxTer_BandWidth_e)eValue

#define DbSvc_TuneGetTerConstellation(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->ter.eConstellation
#define DbSvc_TuneSetTerConstellation(pstTune, eValue)						((DbSvc_TuningInfo_t *)pstTune)->ter.eConstellation = (DxTer_Constellation_e)eValue

#define DbSvc_TuneGetTerHierachy(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->ter.eHierachy
#define DbSvc_TuneSetTerHierachy(pstTune, eValue)							((DbSvc_TuningInfo_t *)pstTune)->ter.eHierachy = (DxTer_Hierachy_e)eValue

#define DbSvc_TuneGetTerStream(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->ter.eStream
#define DbSvc_TuneSetTerStream(pstTune, eValue)								((DbSvc_TuningInfo_t *)pstTune)->ter.eStream = (DxTer_Stream_e)eValue

#define DbSvc_TuneGetTerCodeRate(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->ter.eCodeRate
#define DbSvc_TuneSetTerCodeRate(pstTune, eValue)							((DbSvc_TuningInfo_t *)pstTune)->ter.eCodeRate = (DxTer_CodeRate_e)eValue

#define DbSvc_TuneGetTerGuardInterval(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->ter.eGuardInterval
#define DbSvc_TuneSetTerGuardInterval(pstTune, eValue)						((DbSvc_TuningInfo_t *)pstTune)->ter.eGuardInterval = (DxTer_GuardInterval_e)eValue

#define DbSvc_TuneGetTerTransmode(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->ter.eTransMode
#define DbSvc_TuneSetTerTransmode(pstTune, eValue)							((DbSvc_TuningInfo_t *)pstTune)->ter.eTransMode = (DxTer_TransMode_e)eValue

#define DbSvc_TuneGetTerOffset(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->ter.eOffset
#define DbSvc_TuneSetTerOffset(pstTune, eValue)								((DbSvc_TuningInfo_t *)pstTune)->ter.eOffset = eValue

	/* DVB-T2 */
#define DbSvc_TuneGetTerSystem(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->ter.eSystem
#define DbSvc_TuneSetTerSystem(pstTune, eValue)								((DbSvc_TuningInfo_t *)pstTune)->ter.eSystem = (DxTer_System_e)eValue

#define DbSvc_TuneGetTerPlpId(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->ter.stT2Param.ulPlpId
#define DbSvc_TuneSetTerPlpId(pstTune, ulValue)								((DbSvc_TuningInfo_t *)pstTune)->ter.stT2Param.ulPlpId = ulValue

#define DbSvc_TuneGetTerPreambleFormat(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->ter.stT2Param.preambleFormat
#define DbSvc_TuneSetTerPreambleFormat(pstTune, eValue)						((DbSvc_TuningInfo_t *)pstTune)->ter.stT2Param.preambleFormat = (DxTer_PreambleFormat_e)eValue

#define DbSvc_TuneGetTerMixed(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->ter.stT2Param.mixed
#define DbSvc_TuneSetTerMixed(pstTune, eValue)								((DbSvc_TuningInfo_t *)pstTune)->ter.stT2Param.mixed = (DxTer_Mixed_e)eValue

#define DbSvc_TuneGetTerPilotPattern(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->ter.stT2Param.pilotPattern
#define DbSvc_TuneSetTerPilotPattern(pstTune, eValue)						((DbSvc_TuningInfo_t *)pstTune)->ter.stT2Param.pilotPattern = (DxTer_Pilot_Pattern_e)eValue

#define DbSvc_TuneGetTerExtendedCarrier(pstTune)							((DbSvc_TuningInfo_t *)pstTune)->ter.stT2Param.extenedCarrier
#define DbSvc_TuneSetTerExtendedCarrier(pstTune, eValue)					((DbSvc_TuningInfo_t *)pstTune)->ter.stT2Param.extenedCarrier = (DxTer_ExtendedCarrier_e)eValue

#define DbSvc_TuneGetTerPAPRReduction(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->ter.stT2Param.PAPRreduction
#define DbSvc_TuneSetTerPAPRReduction(pstTune, eValue)						((DbSvc_TuningInfo_t *)pstTune)->ter.stT2Param.PAPRreduction = (DxTer_PAPR_Reduction_e)eValue

#define DbSvc_TuneGetTerPlpBlockNum(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->ter.stT2Param.numPlpBlocks
#define DbSvc_TuneSetTerPlpBlockNum(pstTune, ulValue)						((DbSvc_TuningInfo_t *)pstTune)->ter.stT2Param.numPlpBlocks = ulValue

#define DbSvc_TuneGetTerChanNum(pstTune)									((DbSvc_TerTuningInfo_t *)pstTune)->stCache.ucChanNum
#define DbSvc_TuneSetTerChanNum(pstTune, ucValue)							((DbSvc_TerTuningInfo_t *)pstTune)->stCache.ucChanNum = (HUINT8)ucValue

#define DbSvc_TuneGetTerLevel(pstTune)										((DbSvc_TerTuningInfo_t *)pstTune)->stCache.ucLevel
#define DbSvc_TuneSetTerLevel(pstTune, ucValue)								((DbSvc_TerTuningInfo_t *)pstTune)->stCache.ucLevel = (HUINT8)ucValue

#define DbSvc_TuneGetTerQuality(pstTune)									((DbSvc_TerTuningInfo_t *)pstTune)->stCache.ucQuality
#define DbSvc_TuneSetTerQuality(pstTune, ucValue)							((DbSvc_TerTuningInfo_t *)pstTune)->stCache.ucQuality = (HUINT8)ucValue

#else
#define DbSvc_TuneGetSatInfo(pstTune)										((DbSvc_SatTuningInfo_t *)&((DbSvc_TuningInfo_t *)pstTune)->sat)
#define DbSvc_TuneGetCabInfo(pstTune)										((DbSvc_CabTuningInfo_t *)&((DbSvc_TuningInfo_t *)pstTune)->cab)
#define DbSvc_TuneGetTerInfo(pstTune)										((DbSvc_TerTuningInfo_t *)&((DbSvc_TuningInfo_t *)pstTune)->ter)

//	SAT
#define DbSvc_TuneGetSatAntId(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->sat.ucAntId
#define DbSvc_TuneSetSatAntId(pstTune, nValue)								((DbSvc_TuningInfo_t *)pstTune)->sat.ucAntId = (HUINT8)nValue

#define DbSvc_TuneGetSatFrequency(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->sat.ulFrequency
#define DbSvc_TuneSetSatFrequency(pstTune, ulValue)							((DbSvc_TuningInfo_t *)pstTune)->sat.ulFrequency = ulValue

#define DbSvc_TuneGetSatSymbolRate(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->sat.ulSymbolRate
#define DbSvc_TuneSetSatSymbolRate(pstTune, ulValue)						((DbSvc_TuningInfo_t *)pstTune)->sat.ulSymbolRate = ulValue

#define DbSvc_TuneGetSatPolarization(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->sat.ePolarization
#define DbSvc_TuneSetSatPolarization(pstTune, eValue)						((DbSvc_TuningInfo_t *)pstTune)->sat.ePolarization = eValue

#define DbSvc_TuneGetSatFecCodeRate(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->sat.eFecCodeRate
#define DbSvc_TuneSetSatFecCodeRate(pstTune, eValue)						((DbSvc_TuningInfo_t *)pstTune)->sat.eFecCodeRate = eValue

#define DbSvc_TuneGetSatTransSpec(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->sat.eTransSpec
#define DbSvc_TuneSetSatTransSpec(pstTune, eValue)							((DbSvc_TuningInfo_t *)pstTune)->sat.eTransSpec = eValue

#define DbSvc_TuneGetSatPskModulation(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->sat.ePskMod
#define DbSvc_TuneSetSatPskModulation(pstTune, eValue)						((DbSvc_TuningInfo_t *)pstTune)->sat.ePskMod = eValue

#define DbSvc_TuneGetSatPilot(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->sat.ePilot
#define DbSvc_TuneSetSatPilot(pstTune, eValue)								((DbSvc_TuningInfo_t *)pstTune)->sat.ePilot = eValue

#define DbSvc_TuneGetSatRollOff(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->sat.eRollOff
#define DbSvc_TuneSetSatRollOff(pstTune, eValue)							((DbSvc_TuningInfo_t *)pstTune)->sat.eRollOff = eValue

#define DbSvc_TuneGetSatType(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->sat.eSatType
#define DbSvc_TuneSetSatType(pstTune, eValue)								((DbSvc_TuningInfo_t *)pstTune)->sat.eSatType = eValue

// CAB
#define DbSvc_TuneGetCabFrequency(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->cab.ulFreq
#define DbSvc_TuneSetCabFrequency(pstTune, ulValue)							((DbSvc_TuningInfo_t *)pstTune)->cab.ulFreq = (HUINT32)ulValue

#define DbSvc_TuneGetCabSymbolRate(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->cab.usSR
#define DbSvc_TuneSetCabSymbolRate(pstTune, ulValue)						((DbSvc_TuningInfo_t *)pstTune)->cab.usSR = (HUINT32)ulValue

#define DbSvc_TuneGetCabNetworkId(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->cab.usNetworkId
#define DbSvc_TuneSetCabNetworkId(pstTune, nValue)							((DbSvc_TuningInfo_t *)pstTune)->cab.usNetworkId = (HINT32)nValue

#define DbSvc_TuneGetCabConstellation(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->cab.ucCon
#define DbSvc_TuneSetCabConstellation(pstTune, eValue)						((DbSvc_TuningInfo_t *)pstTune)->cab.ucCon = (DxCab_Constellation_e)eValue

#define DbSvc_TuneGetCabSpectrum(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->cab.ucSpec
#define DbSvc_TuneSetCabSpectrum(pstTune, eValue)							((DbSvc_TuningInfo_t *)pstTune)->cab.ucSpec = (DxCab_Spectrum_e)eValue

#define DbSvc_TuneGetCabTsId(pstTune)
#define DbSvc_TuneSetCabTsId(pstTune, nValue)

//	TER
#define DbSvc_TuneGetTerFrequency(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->ter.ulFreq 			/**< centre_frequency (mutiples of 10Hz) */
#define DbSvc_TuneSetTerFrequency(pstTune, ulValue)							((DbSvc_TuningInfo_t *)pstTune)->ter.ulFreq = (HUINT32)ulValue 			/**< centre_frequency (mutiples of 10Hz) */

#define DbSvc_TuneGetTerBandWidth(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->ter.ucBandwidth		/**< bandwidth	   : 8/7/6 MHz */
#define DbSvc_TuneSetTerBandWidth(pstTune, ucValue)							((DbSvc_TuningInfo_t *)pstTune)->ter.ucBandwidth = (HUINT8)ucValue		/**< bandwidth	   : 8/7/6 MHz */

#define DbSvc_TuneGetTerConstellation(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->ter.ucConstellation	/**< constellation : QPSK/16-QAM/64-QAM */
#define DbSvc_TuneSetTerConstellation(pstTune, ucValue)						((DbSvc_TuningInfo_t *)pstTune)->ter.ucConstellation	= (HUINT8)ucValue	/**< constellation : QPSK/16-QAM/64-QAM */

#define DbSvc_TuneGetTerHierachy(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->ter.ucHierachy 		/**< hierachy information : signalling format for the alpha values */
#define DbSvc_TuneSetTerHierachy(pstTune, ucValue)							((DbSvc_TuningInfo_t *)pstTune)->ter.ucHierachy = (HUINT8)ucValue 		/**< hierachy information : signalling format for the alpha values */

#define DbSvc_TuneGetTerStream(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->ter.ucStream			/**< HP/LP */
#define DbSvc_TuneSetTerStream(pstTune, ucValue)							((DbSvc_TuningInfo_t *)pstTune)->ter.ucStream = (HUINT8)ucValue			/**< HP/LP */

#define DbSvc_TuneGetTerCodeRate(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->ter.ucCodeRate 		/**< FEC */
#define DbSvc_TuneSetTerCodeRate(pstTune, ucValue)							((DbSvc_TuningInfo_t *)pstTune)->ter.ucCodeRate = (HUINT8)ucValue 		/**< FEC */

#define DbSvc_TuneGetTerGuardInterval(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->ter.ucGuardInterval	/**< 1/32, 1/16, 1/8, 1/4 */
#define DbSvc_TuneSetTerGuardInterval(pstTune, ucValue)						((DbSvc_TuningInfo_t *)pstTune)->ter.ucGuardInterval = (HUINT8)ucValue	/**< 1/32, 1/16, 1/8, 1/4 */

#define DbSvc_TuneGetTerTransmode(pstTune)									((DbSvc_TuningInfo_t *)pstTune)->ter.ucTransMode		/**< 2k/8k */
#define DbSvc_TuneSetTerTransmode(pstTune, ucValue)							((DbSvc_TuningInfo_t *)pstTune)->ter.ucTransMode = (HUINT8)ucValue		/**< 2k/8k */

#define DbSvc_TuneGetTerOffset(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->ter.ucOffset
#define DbSvc_TuneSetTerOffset(pstTune, ucValue)							((DbSvc_TuningInfo_t *)pstTune)->ter.ucOffset = (HUINT8)ucValue

#if defined(CONFIG_MW_CH_DVB_T2)
#define DbSvc_TuneGetTerSystem(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->ter.ucSystem		/**< T/T2 */
#define DbSvc_TuneSetTerSystem(pstTune, ucValue)							((DbSvc_TuningInfo_t *)pstTune)->ter.ucSystem = (HUINT8)ucValue	/**< T/T2 */

#define DbSvc_TuneGetTerPlpId(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->ter.stT2param.ulPlpId
#define DbSvc_TuneSetTerPlpId(pstTune, ulValue)								((DbSvc_TuningInfo_t *)pstTune)->ter.stT2param.ulPlpId = ulValue

#define DbSvc_TuneGetTerPreambleFormat(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->ter.stT2param.ucPreambleFormat
#define DbSvc_TuneSetTerPreambleFormat(pstTune, ucValue)					((DbSvc_TuningInfo_t *)pstTune)->ter.stT2param.ucPreambleFormat = (HUINT8)ucValue

#define DbSvc_TuneGetTerMixed(pstTune)										((DbSvc_TuningInfo_t *)pstTune)->ter.stT2param.ucMixed
#define DbSvc_TuneSetTerMixed(pstTune, ucValue)								((DbSvc_TuningInfo_t *)pstTune)->ter.stT2param.ucMixed = (HUINT8)ucValue

#define DbSvc_TuneGetTerPilotPattern(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->ter.stT2param.ucPilotPattern
#define DbSvc_TuneSetTerPilotPattern(pstTune, ucValue)						((DbSvc_TuningInfo_t *)pstTune)->ter.stT2param.ucPilotPattern = (HUINT8)ucValue

#define DbSvc_TuneGetTerExtendedCarrier(pstTune)							((DbSvc_TuningInfo_t *)pstTune)->ter.stT2param.ucExtenedCarrier
#define DbSvc_TuneSetTerExtendedCarrier(pstTune, ucValue)					((DbSvc_TuningInfo_t *)pstTune)->ter.stT2param.ucExtenedCarrier = (HUINT8)ucValue

#define DbSvc_TuneGetTerPAPRReduction(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->ter.stT2param.ucPAPRreduction
#define DbSvc_TuneSetTerPAPRReduction(pstTune, ucValue)						((DbSvc_TuningInfo_t *)pstTune)->ter.stT2param.ucPAPRreduction = (HUINT8)ucValue

#define DbSvc_TuneGetTerPlpBlockNum(pstTune)								((DbSvc_TuningInfo_t *)pstTune)->ter.stT2param.ulNumPlpBlocks
#define DbSvc_TuneSetTerPlpBlockNum(pstTune, ulValue)						((DbSvc_TuningInfo_t *)pstTune)->ter.stT2param.ulNumPlpBlocks = (HUINT32)ulValue
#endif

#define DbSvc_TuneGetTerChanNum(pstExtInfo)									((DbSvc_TerTuningInfo_t *)pstExtInfo)->ucChanNum
#define DbSvc_TuneSetTerChanNum(pstExtInfo, ucValue)						((DbSvc_TerTuningInfo_t *)pstExtInfo)->ucChanNum = (HUINT8)ucValue

#define DbSvc_TuneGetTerLevel(pstExtInfo)									((DbSvc_TerTuningInfo_t *)pstExtInfo)->ucLevel
#define DbSvc_TuneSetTerLevel(pstExtInfo, ucValue)							((DbSvc_TerTuningInfo_t *)pstExtInfo)->ucLevel = (HUINT8)ucValue

#define DbSvc_TuneGetTerQuality(pstExtInfo)									((DbSvc_TerTuningInfo_t *)pstExtInfo)->ucQuality
#define DbSvc_TuneSetTerQuality(pstExtInfo, ucValue)						((DbSvc_TerTuningInfo_t *)pstExtInfo)->ucQuality = (HUINT8)ucValue

#endif

/*		TS Info
 */
#define	DbSvc_CopyTuneInfo(pstDest, pstSource)								HxSTD_MemCopy(pstDest, pstSource, sizeof(DbSvc_TuningInfo_t))

#if defined(DbSvc_SUPPORT_DXINFO)
#define DbSvc_TsGetTuneInfo(pstTsInfo)										(&((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam)
#define DbSvc_TsGetTuneInfoExt(pstTsInfo)									(&((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt)
#define DbSvc_TsGetSatTuneInfo(pstTsInfo)									(&((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.sat)
#define DbSvc_TsGetSatTuneInfoExt(pstTsInfo)								(&((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.sat)
#define DbSvc_TsGetCabTuneInfo(pstTsInfo)									(&((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.cab)
#define DbSvc_TsGetCabTuneInfoExt(pstTsInfo)								(&((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.cab)
#define DbSvc_TsGetTerTuneInfo(pstTsInfo)									(&((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter)
#define DbSvc_TsGetTerTuneInfoExt(pstTsInfo)								(&((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.ter)
#define	DbSvc_TsCopySatTuneInfo(pstDest, pstSource)							HxSTD_MemCopy(pstDest, pstSource, sizeof(DxSatTuningInfo_t))
#define	DbSvc_TsCopyCabTuneInfo(pstDest, pstSource)							HxSTD_MemCopy(pstDest, pstSource, sizeof(DxCabTuningInfo_t))
#define	DbSvc_TsCopyTerTuneInfo(pstDest, pstSource)							HxSTD_MemCopy(pstDest, pstSource, sizeof(DxTerTuningInfo_t))
#define	DbSvc_TsCopyTerTuneInfoExt(pstDest, pstSource)						HxSTD_MemCopy(pstDest, pstSource, sizeof(DbSvc_TerTuningInfoExt_t))
#define DbSvc_TsCopyTuneInfo(pstTsDest, pstTsSource)						{ \
																				DxTuneParam_t *pstTuneDest = (&((DbSvc_TsInfo_t *)(pstTsDest))->stDxInfo.tuningParam);		\
																				DxTuneParam_t *pstTuneSource = (&((DbSvc_TsInfo_t *)(pstTsSource))->stDxInfo.tuningParam));	\
																				HxSTD_MemCopy(pstTuneDest, pstTuneSource, sizeof(DxTuneParam_t));	\
																				HxSTD_MemCopy(&(pstTsDest)->utTuningParamExt, &(pstTsSource)->utTuningParamExt, sizeof(DbSvc_TuningInfoExt_t));\
																			}
#else
#define DbSvc_TsGetTuneInfo(pstTsInfo)										(&((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam)
#define DbSvc_TsGetTuneInfoExt(pstTsInfo)
#define DbSvc_TsGetSatTuneInfo(pstTsInfo)									(&((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat)
#define DbSvc_TsGetSatTuneInfoExt(pstTsInfo)								(&((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat)
#define DbSvc_TsGetCabTuneInfo(pstTsInfo)									(&((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.cab)
#define DbSvc_TsGetCabTuneInfoExt(pstTsInfo)								(&((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.cab)
#define DbSvc_TsGetTerTuneInfo(pstTsInfo)									(&((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter)
#define DbSvc_TsGetTerTuneInfoExt(pstTsInfo)								(&((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter)
#define	DbSvc_TsCopySatTuneInfo(pstDest, pstSource)							HxSTD_MemCopy(pstDest, pstSource, sizeof(DbSvc_SatTuningInfo_t))
#define	DbSvc_TsCopyCabTuneInfo(pstDest, pstSource)							HxSTD_MemCopy(pstDest, pstSource, sizeof(DbSvc_CabTuningInfo_t))
#define	DbSvc_TsCopyTerTuneInfo(pstDest, pstSource)							HxSTD_MemCopy(pstDest, pstSource, sizeof(DbSvc_TerTuningInfo_t))
#define	DbSvc_TsCopyTerTuneInfoExt(pstDest, pstSource)						HxSTD_MemCopy(pstDest, pstSource, sizeof(DbSvc_TerTuningInfo_t))
#define DbSvc_TsCopyTuneInfo(pstTsDest, pstTsSource)						{\
																				DxTuneParam_t	*pstTuneDest = (&((DbSvc_TsInfo_t *)(pstTsDest))->utTuningParam);		\
																				DxTuneParam_t	*pstTuneSource = (&((DbSvc_TsInfo_t *)(pstTsSource))->utTuningParam);	\
																				HxSTD_MemCopy(pstTuneDest, pstTuneSource, sizeof(DxTuneParam_t));		\
																				HxSTD_MemCopy(&(pstTsDest)->utTuningParamExt, &(pstTsSource)->utTuningParamExt, sizeof(DbSvc_TuningInfoExt_t));\
																			}
#endif

#define	DbSvc_TsResetInfo(pstTsInfo)										HxSTD_MemSet(pstTsInfo, 0, sizeof(DbSvc_TsInfo_t));
#define	DbSvc_TsCopyInfo(pstDest, pstSource)								HxSTD_MemCopy(pstDest, pstSource, sizeof(DbSvc_TsInfo_t))

#if defined(DbSvc_SUPPORT_DXINFO)
#define	DbSvc_TsGetTsIdx(pstTsInfo)											((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.uid
#define	DbSvc_TsSetTsIdx(pstTsInfo, nValue)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.uid = nValue

#define	DbSvc_TsGetUniqueId(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.uid
#define	DbSvc_TsSetUniqueId(pstTsInfo, nValue)								//((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.uid = (HINT32)nValue

#define	DbSvc_TsGetNetIdx(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.netuid
#define	DbSvc_TsSetNetIdx(pstTsInfo, nValue)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.netuid = (HUINT32)nValue

#define	DbSvc_TsGetTsId(pstTsInfo)											((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tsid
#define	DbSvc_TsSetTsId(pstTsInfo, nValue)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tsid = (HINT32)nValue

#define	DbSvc_TsGetOnId(pstTsInfo)											((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.onid
#define	DbSvc_TsSetOnId(pstTsInfo, nValue)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.onid = (HINT32)nValue

#define	DbSvc_TsGetTunerId(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tunerid
#define	DbSvc_TsSetTunerId(pstTsInfo, nValue)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tunerid = (HINT32)nValue

#define	DbSvc_TsGetDeliveryType(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.deliType
#define	DbSvc_TsSetDeliveryType(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.deliType = eValue
#else
#define	DbSvc_TsGetTsIdx(pstTsInfo)											((DbSvc_TsInfo_t *)pstTsInfo)->usTsIdx
#define	DbSvc_TsSetTsIdx(pstTsInfo, usValue)								((DbSvc_TsInfo_t *)pstTsInfo)->usTsIdx = (HUINT16)usValue

#define	DbSvc_TsGetUniqueId(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->usTsIdx	//((DbSvc_TsInfo_t *)pstTsInfo)->usUniqueId
#define	DbSvc_TsSetUniqueId(pstTsInfo, usValue)								//((DbSvc_TsInfo_t *)pstTsInfo)->usUniqueId = (HUINT16)usValue

#define	DbSvc_TsGetNetIdx(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->usNetIdx
#define	DbSvc_TsSetNetIdx(pstTsInfo, usValue)								((DbSvc_TsInfo_t *)pstTsInfo)->usNetIdx = (HUINT16)usValue

#define	DbSvc_TsGetTsId(pstTsInfo)											((DbSvc_TsInfo_t *)pstTsInfo)->usTsId
#define	DbSvc_TsSetTsId(pstTsInfo, usValue)									((DbSvc_TsInfo_t *)pstTsInfo)->usTsId = (HUINT16)usValue

#define	DbSvc_TsGetOnId(pstTsInfo)											((DbSvc_TsInfo_t *)pstTsInfo)->usOnId
#define	DbSvc_TsSetOnId(pstTsInfo, usValue)									((DbSvc_TsInfo_t *)pstTsInfo)->usOnId = (HUINT16)usValue

#define	DbSvc_TsGetTunerId(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->ucTunerId
#define	DbSvc_TsSetTunerId(pstTsInfo, ucValue)								((DbSvc_TsInfo_t *)pstTsInfo)->ucTunerId = (HUINT8)ucValue

#define	DbSvc_TsGetDeliveryType(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->eDeliType
#define	DbSvc_TsSetDeliveryType(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->eDeliType = eValue
#endif

#if defined(DbSvc_SUPPORT_DXINFO)
// SAT
#define DbSvc_TsGetSatFrequency(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.sat.tuningInfo.ulFrequency
#define DbSvc_TsSetSatFrequency(pstTsInfo, ulValue)							((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.sat.tuningInfo.ulFrequency = ulValue

#define DbSvc_TsGetSatSymbolRate(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.sat.tuningInfo.ulSymbolRate
#define DbSvc_TsSetSatSymbolRate(pstTsInfo, ulValue)						((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.sat.tuningInfo.ulSymbolRate = ulValue

#define DbSvc_TsGetSatFecCodeRate(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.sat.tuningInfo.eFecCodeRate
#define DbSvc_TsSetSatFecCodeRate(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.sat.tuningInfo.eFecCodeRate = eValue

#define DbSvc_TsGetSatAntId(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.sat.tuningInfo.antuid
#define DbSvc_TsSetSatAntId(pstTsInfo, ulValue)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.sat.tuningInfo.antuid = ulValue

#define DbSvc_TsGetSatPolarization(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.sat.tuningInfo.ePolarization
#define DbSvc_TsSetSatPolarization(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.sat.tuningInfo.ePolarization = eValue

#define DbSvc_TsGetSatTransSpec(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.tuningInfo.eTransSpec
#define DbSvc_TsSetSatTransSpec(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.tuningInfo.eTransSpec = eValue

#define DbSvc_TsGetSatPskModulation(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.tuningInfo.ePskMod
#define DbSvc_TsSetSatPskModulation(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.tuningInfo.ePskMod = eValue

#define DbSvc_TsGetSatPilot(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.tuningInfo.ePilot
#define DbSvc_TsSetSatPilot(pstTsInfo, eValue)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.tuningInfo.ePilot = eValue

#define DbSvc_TsGetSatRollOff(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.tuningInfo.eRollOff
#define DbSvc_TsSetSatRollOff(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.tuningInfo.eRollOff = eValue

#define DbSvc_TsGetSatSatType(pstTsInfo)									OTL_CONV_SatString2SVC_SatType((const HCHAR *)((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.tuningInfo.satType)
#define DbSvc_TsSetSatSatType(pstTsInfo, eValue)							HLIB_STD_StrNCpySafe((HCHAR *)((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.sat.tuningInfo.satType, (const HCHAR *)OTL_CONV_SVC_SatType2String(eValue), DxANTINFO_NAME_LEN)

//	CAB
#define DbSvc_TsGetCabFrequency(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.cab.ulFrequency
#define DbSvc_TsSetCabFrequency(pstTsInfo, ulValue)							((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.cab.ulFrequency = ulValue

#define DbSvc_TsGetCabConstellation(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.cab.constellation
#define DbSvc_TsSetCabConstellation(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.cab.constellation = eValue

#define DbSvc_TsGetCabSymbolRate(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.cab.ulSymbolRate
#define DbSvc_TsSetCabSymbolRate(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.cab.ulSymbolRate = eValue

#define DbSvc_TsGetCabNetworkId(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.cab.nNetId
#define DbSvc_TsSetCabNetworkId(pstTsInfo, nValue)							((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.cab.nNetId = nValue

#define	DbSvc_TsGetCabSpectrum(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.cab.spectrum
#define	DbSvc_TsSetCabSpectrum(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.cab.spectrum = eValue

#define DbSvc_TsGetCabTsId(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.cab.nTsId
#define DbSvc_TsSetCabTsId(pstTsInfo, nValue)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.cab.nTsId = (HINT32)nValue

// TER
#define DbSvc_TsGetTerFrequency(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.ulFrequency
#define DbSvc_TsGetTerFrequency(pstTsInfo, ulValue)							((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.ulFrequency = ulValue

#define DbSvc_TsGetTerBandWidth(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eBandWidth
#define DbSvc_TsSetTerBandWidth(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eBandWidth = (DxTer_BandWidth_e)eValue

#define DbSvc_TsGetTerConstellation(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eConstellation
#define DbSvc_TsSetTerConstellation(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eConstellation = (DxTer_Constellation_e)eValue

#define DbSvc_TsGetTerHierachy(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eHierachy
#define DbSvc_TsSetTerHierachy(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eHierachy = (DxTer_Hierachy_e)eValue

#define DbSvc_TsGetTerStream(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eStream
#define DbSvc_TsSetTerStream(pstTsInfo, eValue)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eStream = (DxTer_Stream_e)eValue

#define DbSvc_TsGetTerCodeRate(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eCodeRate
#define DbSvc_TsSetTerCodeRate(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eCodeRate = (DxTer_CodeRate_e)eValue

#define DbSvc_TsGetTerGuardInterval(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eGuardInterval
#define DbSvc_TsSetTerGuardInterval(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eGuardInterval = (DxTer_GuardInterval_e)eValue

#define DbSvc_TsGetTerTransmode(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eTransMode
#define DbSvc_TsSetTerTransmode(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eTransMode = (DxTer_TransMode_e)eValue

#define DbSvc_TsGetTerOffset(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eOffset
#define DbSvc_TsSetTerOffset(pstTsInfo, eValue)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eOffset = (DxTer_Offset_e)eValue

	/* DVB-T2 */
#define DbSvc_TsGetTerSystem(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eSystem
#define DbSvc_TsSetTerSystem(pstTsInfo, eValue)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.eSystem = (DxTer_System_e)eValue

#define DbSvc_TsGetTerPlpId(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.stT2Param.ulPlpId
#define DbSvc_TsSetTerPlpId(pstTsInfo, ulValue)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.stT2Param.ulPlpId = ulValue

#define DbSvc_TsGetTerPreambleFormat(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.stT2Param.preambleFormat
#define DbSvc_TsSetTerPreambleFormat(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.stT2Param.preambleFormat = (DxTer_PreambleFormat_e)eValue

#define DbSvc_TsGetTerMixed(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.stT2Param.mixed
#define DbSvc_TsSetTerMixed(pstTsInfo, eValue)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.stT2Param.mixed = (DxTer_Mixed_e)eValue

#define DbSvc_TsGetTerPilotPattern(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.stT2Param.pilotPattern
#define DbSvc_TsSetTerPilotPattern(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.stT2Param.pilotPattern = (DxTer_Pilot_Pattern_e)eValue

#define DbSvc_TsGetTerExtendedCarrier(pstTsInfo)							((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.stT2Param.extenedCarrier
#define DbSvc_TsSetTerExtendedCarrier(pstTsInfo, eValue)					((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.stT2Param.extenedCarrier = (DxTer_ExtendedCarrier_e)eValue

#define DbSvc_TsGetTerPAPRReduction(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.stT2Param.PAPRreduction
#define DbSvc_TsSetTerPAPRReduction(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.stT2Param.PAPRreduction = (DxTer_PAPR_Reduction_e)eValue

#define DbSvc_TsGetTerPlpBlockNum(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.stT2Param.numPlpBlocks
#define DbSvc_TsSetTerPlpBlockNum(pstTsInfo, ulValue)						((DbSvc_TsInfo_t *)pstTsInfo)->stDxInfo.tuningParam.ter.stT2Param.numPlpBlocks = ulValue

#define DbSvc_TsGetTerChanNum(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.ter.ucChanNum
#define DbSvc_TsSetTerChanNum(pstTsInfo, ucValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.ter.ucChanNum = (HUINT8)ucValue

#define DbSvc_TsGetTerLevel(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.ter.ucLevel
#define DbSvc_TsSetTerLevel(pstTsInfo, ucValue)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.ter.ucLevel = (HUINT8)ucValue

#define DbSvc_TsGetTerQuality(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.ter.ucQuality
#define DbSvc_TsSetTerQuality(pstTsInfo, ucValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.ter.ucQuality = (HUINT8)ucValue
#else
//	SAT
#define DbSvc_TsGetSatFrequency(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.ulFrequency
#define DbSvc_TsSetSatFrequency(pstTsInfo, ulValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.ulFrequency = ulValue

#define DbSvc_TsGetSatSymbolRate(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.ulSymbolRate
#define DbSvc_TsSetSatSymbolRate(pstTsInfo, ulValue)						((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.ulSymbolRate = ulValue

#define DbSvc_TsGetSatFecCodeRate(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.eFecCodeRate
#define DbSvc_TsSetSatFecCodeRate(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.eFecCodeRate = eValue

#define DbSvc_TsGetSatAntId(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.antuid
#define DbSvc_TsSetSatAntId(pstTsInfo, ulValue)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.antuid = ulValue

#define DbSvc_TsGetSatPolarization(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.ePolarization
#define DbSvc_TsSetSatPolarization(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.ePolarization = eValue

#define DbSvc_TsGetSatTransSpec(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.eTransSpec
#define DbSvc_TsSetSatTransSpec(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.eTransSpec = eValue

#define DbSvc_TsGetSatPskModulation(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.ePskMod
#define DbSvc_TsSetSatPskModulation(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.ePskMod = eValue

#define DbSvc_TsGetSatPilot(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.ePilot
#define DbSvc_TsSetSatPilot(pstTsInfo, eValue)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.ePilot = eValue

#define DbSvc_TsGetSatRollOff(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.eRollOff
#define DbSvc_TsSetSatRollOff(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.eRollOff = eValue

#define DbSvc_TsGetSatSatType(pstTsInfo)									OTL_CONV_SatString2SVC_SatType((const HCHAR *)((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.tuningInfo.satType)
#define DbSvc_TsSetSatSatType(pstTsInfo, eValue)							HLIB_STD_StrNCpySafe((HCHAR *)((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.sat.tuningInfo.satType, (const HCHAR *)OTL_CONV_SVC_SatType2String(eValue), DxANTINFO_NAME_LEN)

// CAB
#define DbSvc_TsGetCabFrequency(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.cab.ulFrequency
#define DbSvc_TsSetCabFrequency(pstTsInfo, ulValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.cab.ulFrequency = ulValue

#define DbSvc_TsGetCabConstellation(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.cab.constellation
#define DbSvc_TsSetCabConstellation(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.cab.constellation = eValue

#define DbSvc_TsGetCabSymbolRate(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.cab.ulSymbolRate
#define DbSvc_TsSetCabSymbolRate(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.cab.ulSymbolRate = eValue

#define DbSvc_TsGetCabNetworkId(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.cab.nNetId
#define DbSvc_TsSetCabNetworkId(pstTsInfo, nValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.cab.nNetId = nValue

#define	DbSvc_TsGetCabSpectrum(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.cab.spectrum
#define	DbSvc_TsSetCabSpectrum(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.cab.spectrum = eValue

#define DbSvc_TsGetCabTsId(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->tuningParam.cab.nTsId
#define DbSvc_TsSetCabTsId(pstTsInfo, nValue)								((DbSvc_TsInfo_t *)pstTsInfo)->tuningParam.cab.nTsId = (HINT32)nValue

// TER
#define DbSvc_TsGetTerFrequency(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.ulFrequency
#define DbSvc_TsGetTerFrequency(pstTsInfo, ulValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.ulFrequency = ulValue

#define DbSvc_TsGetTerBandWidth(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eBandWidth
#define DbSvc_TsSetTerBandWidth(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eBandWidth = (DxTer_BandWidth_e)eValue

#define DbSvc_TsGetTerConstellation(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eConstellation
#define DbSvc_TsSetTerConstellation(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eConstellation = (DxTer_Constellation_e)eValue

#define DbSvc_TsGetTerHierachy(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eHierachy
#define DbSvc_TsSetTerHierachy(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eHierachy = (DxTer_Hierachy_e)eValue

#define DbSvc_TsGetTerStream(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eStream
#define DbSvc_TsSetTerStream(pstTsInfo, eValue)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eStream = (DxTer_Stream_e)eValue

#define DbSvc_TsGetTerCodeRate(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eCodeRate
#define DbSvc_TsSetTerCodeRate(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eCodeRate = (DxTer_CodeRate_e)eValue

#define DbSvc_TsGetTerGuardInterval(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eGuardInterval
#define DbSvc_TsSetTerGuardInterval(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eGuardInterval = (DxTer_GuardInterval_e)eValue

#define DbSvc_TsGetTerTransmode(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eTransMode
#define DbSvc_TsSetTerTransmode(pstTsInfo, eValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eTransMode = (DxTer_TransMode_e)eValue

#define DbSvc_TsGetTerOffset(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eOffset
#define DbSvc_TsSetTerOffset(pstTsInfo, eValue)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eOffset = (DxTer_Offset_e)eValue

	/* DVB-T2 */
#define DbSvc_TsGetTerSystem(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eSystem
#define DbSvc_TsSetTerSystem(pstTsInfo, eValue)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.eSystem = (DxTer_System_e)eValue

#define DbSvc_TsGetTerPlpId(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.stT2Param.ulPlpId
#define DbSvc_TsSetTerPlpId(pstTsInfo, ulValue)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.stT2Param.ulPlpId = ulValue

#define DbSvc_TsGetTerPreambleFormat(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.stT2Param.preambleFormat
#define DbSvc_TsSetTerPreambleFormat(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.stT2Param.preambleFormat = (DxTer_PreambleFormat_e)eValue

#define DbSvc_TsGetTerMixed(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.stT2Param.mixed
#define DbSvc_TsSetTerMixed(pstTsInfo, eValue)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.stT2Param.mixed = (DxTer_Mixed_e)eValue

#define DbSvc_TsGetTerPilotPattern(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.stT2Param.pilotPattern
#define DbSvc_TsSetTerPilotPattern(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.stT2Param.pilotPattern = (DxTer_Pilot_Pattern_e)eValue

#define DbSvc_TsGetTerExtendedCarrier(pstTsInfo)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.stT2Param.extenedCarrier
#define DbSvc_TsSetTerExtendedCarrier(pstTsInfo, eValue)					((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.stT2Param.extenedCarrier = (DxTer_ExtendedCarrier_e)eValue

#define DbSvc_TsGetTerPAPRReduction(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.stT2Param.PAPRreduction
#define DbSvc_TsSetTerPAPRReduction(pstTsInfo, eValue)						((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.stT2Param.PAPRreduction = (DxTer_PAPR_Reduction_e)eValue

#define DbSvc_TsGetTerPlpBlockNum(pstTsInfo)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.stT2Param.numPlpBlocks
#define DbSvc_TsSetTerPlpBlockNum(pstTsInfo, ulValue)						((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParam.ter.stT2Param.numPlpBlocks = ulValue

#define DbSvc_TsGetTerChanNum(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.ter.ucChanNum
#define DbSvc_TsSetTerChanNum(pstTsInfo, ucValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.ter.ucChanNum = (HUINT8)ucValue

#define DbSvc_TsGetTerLevel(pstTsInfo)										((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.ter.ucLevel
#define DbSvc_TsSetTerLevel(pstTsInfo, ucValue)								((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.ter.ucLevel = (HUINT8)ucValue

#define DbSvc_TsGetTerQuality(pstTsInfo)									((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.ter.ucQuality
#define DbSvc_TsSetTerQuality(pstTsInfo, ucValue)							((DbSvc_TsInfo_t *)pstTsInfo)->utTuningParamExt.ter.ucQuality = (HUINT8)ucValue
#endif
#endif

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#if 0
typedef enum
{
	eDbSvcMsg_LoadPreProgrammedChannel = 0
	,eDbSvcMsg_CreateIndex
	,eDbSvcMsg_LoadToMemory
	,eDbSvcMsg_SyncToFile
	,eDbSvcMsg_ImportPreProgrammedChannel
	,eDbSvcMsg_ExportPreProgrammedChannel
	,eDbSvcMsg_FindServiceList
} DbSvc_MsgType_e;

typedef	struct
{
	DbSvc_MsgType_e		eMsg;
	Handle_t			hAction;	/**< action handle */
	HUINT32				ulParam1;	/**< arg 1 */
	HUINT32				ulParam2;	/**< arg 2 */
	HUINT32				ulParam3;	/**< arg 3 */
	HUINT32				ulParam4;	/**< arg 4 */
									/**< arg N */
} DbSvc_Msg_t;
#endif

typedef enum
{
	eSvcTable_Service,
	eSvcTable_Transponder,
	eSvcTable_Provider,
	eSvcTable_Network,
	eSvcTable_Group,
	eSvcTable_Bouquet,
	eSvcTable_Antenna,

	eSvcTable_MAX
} DbSvc_Table_e;

#if 0
/** @brief service section:
		all 그룹 내에 중복 채널이 존재할 수 있는데, 이 때 구분되는 단위가 section이다.
		0부터 15사이의 값을 갖는다.
		service handle의 상위 4비트를 사용하는 것으로, 아래 정의한 값 외에는 추가 불가.
*/
typedef enum
{
	eSvcSection_All					= 0,
	eSvcSection_PreProgrammed		= 0x01,
	eSvcSection_Lcn					= 0x02,
	eSvcSection_NonLcn_Priority1	= 0x10,
	eSvcSection_NonLcn_Priority2	= 0x20,
	eSvcSection_NonLcn_Priority3	= 0x40,
	eSvcSection_NonLcn_Priority4	= 0x80
	/* 더 이상 추가할 수 없음 !!!!!!!!! */
} DbSvc_Section_e, DbSvc_Section_b;
#endif

/** @brief DB내부에 Table단위로 구분되는 서비스 그룹 - All, Fav1/2/3/4/5, Recent, Most */
typedef enum
{
	eSvcGroup_All		= 0,
	eSvcGroup_Fav_1		= 1,
	eSvcGroup_Fav_2		= 2,
	eSvcGroup_Fav_3		= 3,
	eSvcGroup_Fav_4		= 4,
	eSvcGroup_Fav_5		= 5,
	eSvcGroup_Recent	= 6,
} DbSvc_Group_e;

#if 0
/** @brief [service db 속성 - SvcType] 휴맥스가 정의한 service type으로 기본적으로 SDT의 service type에 정의된 종류 중 지원하는 type이 여기 포함된다. */
typedef enum
{ /* [WARN] forbid to change a db-saving enum value, [WARN] 절대 ifdef로 추가 하지 말것. 보이면 바로 지울 것임 */
	eSvcType_All			= 0,
	eSvcType_Tv				= 0x0001,
	eSvcType_Radio			= 0x0002,
	eSvcType_Data			= 0x0004,
	eSvcType_Subfeed		= 0x0008,	// Multifeed Service 의 Subfeed 들
	eSvcType_Replacement	= 0x0010,	// Nordic 사양의 Replacement Service
	eSvcType_Hidden   		= 0x0020,
	eSvcType_MHPTV			= 0x0040,
	eSvcType_Engineering	= 0x0080,
	eSvcType_Rfvod			= 0x0100,	// for rfvod service
} DbSvc_Type_e, DbSvc_Type_b;
#endif
#if 0
/** @brief DbSvc_Type_e은 주로 service db에서 검색 목적으로 사용되지만,
			service type 개수로 loop 도는 경우를 위해 define으로 만들어 둔 것이라, DbSvc_Type_e의 type 개수와 sync 맞춰야 함.
			다음 값은 eSvcType_All을 제외한 type 개수임 */
#define MAX_SVCTYPE_NUM		8
#endif

#if 0
/** @brief [service db 속성 - VideoType] */
typedef enum
{ /* [WARN] forbid to change a db-saving enum value, [WARN] 절대 ifdef로 추가 하지 말것. 보이면 바로 지울 것임 */
	eVideoType_All		= 0,
	eVideoType_Sd		= 1,
	eVideoType_Hd		= 2,
} DbSvc_VideoType_e;

/** @brief [service db 속성 - CasType] */
typedef enum
{
	eCasType_All			= 0,

	eCasType_Fta			= 0x0001,
	eCasType_Nagra			= 0x0002,
	eCasType_Irdeto			= 0x0004,
	eCasType_Nds			= 0x0008,
	eCasType_Viaccess		= 0x0010,
	eCasType_Conax			= 0x0020,
	eCasType_MediaGuard		= 0x0040,
	eCasType_Cryptoworks	= 0x0080,
	eCasType_JpBCas			= 0x0100,
	eCasType_JpCCas			= 0x0200,
	eCasType_CiPlus			= 0x0400,
	eCasType_AllCas			= 0x1000
} DbSvc_CasType_e, DbSvc_CasType_b;
#endif

typedef enum
{
	eSortType_NUMBER 					= 0,	/**< eOrgDeliType가 아닌, ucChListPriority로 ordering을 함. priority는 configuration되어야한다 */
	eSortType_ALPHABET					= 1,	/**< szSvcName 우선이고, 이름 같으면, number 순으로 */
	eSortType_NUMBER_AND_INDEX			= 2,	/**< LCN이 지정되어 있으면 LCN 우선, 아니라면 저장된 순서대로 */
	eSortType_NUMBER_AND_SVCTYPE		= 3,	/**< { 모든 TV 채널 } -> { 모든 Radio 채널 } 순서 */
	eSortType_NUMBER_AND_SVCTYPE_ONLY	= 4,	/**< eSortType_NUMBER_AND_SVCTYPE에서 ucChListPriority 제한을 없앤 사양, ucChListPriority는 delivery 순서를 결정하는 기능이다 */
	eSortType_FAV_INDEX_AND_SVCTYPE		= 5,		/**< fav group에서, {Tv 전체} --> {Radio 전체} 로 보이기 위한 사양 */
	eSortType_INDEX
} DbSvc_SortType_e;

typedef enum
{
	eLcnVisibleSvc_Only 	= 0,
	eLcnVisibleSvc_FindAll	= 1
} DbSvc_VisibleType_e;


typedef enum
{
	eChAlphabetGroup_AlphabetAll	=	0,		/** 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetA		=	0x41,	/** A로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetB		=	0x42,	/** B로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetC		=	0x43,	/** C로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetD		=	0x44,	/** D로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetE		=	0x45,	/** E로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetF		=	0x46,	/** F로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetG		=	0x47,	/** G로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetH		=	0x48,	/** H로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetI		=	0x49,	/** I로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetJ		=	0x4A,	/** J로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetK		=	0x4B,	/** K로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetL		=	0x4C,	/** L로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetM		=	0x4D,	/** M로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetN		=	0x4E,	/** N로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetO		=	0x4F,	/** O로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetP		=	0x50,	/** P로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetQ		=	0x51,	/** Q로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetR		=	0x52,	/** R로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetS		=	0x53,	/** S로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetT		=	0x54,	/** T로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetU		=	0x55,	/** U로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetV		=	0x56,	/** V로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetW		=	0x57,	/** W로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetX		=	0x58,	/** X로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetY		=	0x59,	/** Y로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetZ		=	0x5A,	/** Z로 시작하는 모든 채널을 보여준다. */
	eChAlphabetGroup_AlphabetDigit	=	0x5B,	/** 0 ~ 9 로 시작하는 채널을 보여준다. */
	eChAlphabetGroup_AlphabetEtc	=	0x5C	/** A-Z, 0-9 이외의 채널을 보여준다.  */
} DbSvc_AlphabetGroupType_e;


/*
	[service user flag] 가이드라인

	1. user flag의 의미
		- service table내에서 boolean type 필드 개수를 줄일 목적으로 user flag을 둔다. 값은 비트 연산에의해 0 또는 1이 될 것이다.

	2. 주의 사항
		- 32개가 모자랄 경우, SVC_UserFlag2_t를 정의해야 한다.

	3. 사용법
		- eSvcUserFlag_8을 세팅:
			stSvcInfo.eUserFlag1 |= eSvcUserFlag_8;
		- eSvcUserFlag_8이 세팅 되었는지 확인하려면:
			if(0 != (stSvcInfo.eUserFlag1 & eSvcUserFlag_8))
			{
				// eSvcUserFlag_8이 세팅 되었음
			}
			else
			{
				// eSvcUserFlag_8이 세팅 되지 않았음
			}

	4. user flag의 종류 - "all or incluing this condition"와 "exclusive condition" (or complementary set)의 차이점
		- all or incluing this condition: 조건이 걸리지 않으면, flag이 true/false인 놈을 모두 찾는다. 조건이 걸리면 걸린 놈만 찾는다.
		- exclusive condition: 조건이 true면 true인 놈만 찾고, false면 false인 놈만 찾는다.
			-> 이 경우, 여기 enum 정의에, 이것을 명시해 주고, 필요에 따라, local_dbsvc_MakeSvcUserFlag1ConditionStr()에 조건을 추가해준다.
*/
typedef enum
{
	eSvcUserFlag_All					= 0,
	eSvcUserFlag_01_FsatUserCustom		= 0x00000001,	/**< all or incluing this condition	, CONFIG_OP_FREESAT */
	eSvcUserFlag_02_Selectable			= 0x00000002,	/**< all or incluing this condition	, CONFIG_OP_FREESAT or CONFIG_OP_UK_DTT */
	eSvcUserFlag_03_SvcDeletedByUser	= 0x00000004,	/**< exclusive condition			, Italy DTT에서 사용 중(CONFIG_DB_SVC_TRASH)이나 같은 요구 사항 내에서 사용 가능 */
	eSvcUserFlag_04_Reserved			= 0x00000008,
	eSvcUserFlag_05_Reserved			= 0x00000010,
	eSvcUserFlag_06_Reserved			= 0x00000020,
	eSvcUserFlag_07_Reserved			= 0x00000040,
	eSvcUserFlag_08_Reserved			= 0x00000080,
	eSvcUserFlag_09_AdultSvc			= 0x00000100,	/**< exclusive condition			, CONFIG_OP_FREESAT */
	eSvcUserFlag_10_Reserved			= 0x00000200,
	eSvcUserFlag_11_FreeCiMode			= 0x00000400,	/**< exclusive condition			, CI+ Model only */
	eSvcUserFlag_12_Reserved			= 0x00000800,
	eSvcUserFlag_13_VolatileSvc			= 0x00001000,	/**< exclusive condition    	    , 휘발성 Service로 저장하지 않는다. Tune Action에서 사용 */
	eSvcUserFlag_14_Reserved			= 0x00002000,
	eSvcUserFlag_15_Reserved			= 0x00004000,
	eSvcUserFlag_16_Reserved			= 0x00008000,
	eSvcUserFlag_17_Reserved			= 0x00010000,
	eSvcUserFlag_18_Reserved			= 0x00020000,
	eSvcUserFlag_19_Reserved			= 0x00040000,
	eSvcUserFlag_20_ParentalControl		= 0x00080000,	/**< all condistion					, Parental control이 있었던 서비스를 marking. 이 mark가 없으면 wtv가 parental check를 skip한다. */
	eSvcUserFlag_21_Reserved			= 0x00100000,
	eSvcUserFlag_22_Reserved			= 0x00200000,
	eSvcUserFlag_23_Reserved			= 0x00400000,
	eSvcUserFlag_24_Reserved			= 0x00800000,
	eSvcUserFlag_25_Reserved			= 0x01000000,
	eSvcUserFlag_26_Reserved			= 0x02000000,
	eSvcUserFlag_27_VideoCodecH264		= 0x04000000,	/**< for freesat */
	eSvcUserFlag_28_Reserved			= 0x08000000,
	eSvcUserFlag_29_Reserved			= 0x10000000,
	eSvcUserFlag_30_Reserved			= 0x20000000,
	eSvcUserFlag_31_SvcConflictLcn		= 0x40000000,	/**< exclusive condition			, Italy DTT에서 사용 중 automatic service numbering 시 conflict 발생 service 에 설정*/
	eSvcUserFlag_32_NewService			= 0x80000000	/**< all or including this condition , 새로운 Channel이 발견 되었을때 해당 플래그를 세워준다. OCTO 2.0기준 해당 플래그는 OCTO가 APPL로 줄수만 있고 APPL은 해당 플래그를 세워 줘도 OCTO가 무시한다.*/
} DbSvc_UserFlag1_e, DbSvc_UserFlag1_b;

/** @brief [service db 속성] */
typedef struct
{
	DxSvcType_e				eSvcType;
	DxVideoType_e			eVideoType;
	SVC_SatType_t			eSatType;
	DxCasType_e				eCasType;
	DxDeliveryType_e		eOrgDeliType;
	/* additional attributes can be added below */
	DbSvc_SortType_e		eSortType;
	DbSvc_VisibleType_e		eVisibleType;
	HINT32					nBouqIdx;
	HINT32					nGenreIdx;
	HUINT8					ucAlphabetIdx;
	DxSvcSection_e			eSection;
	DxOperator_e				eSvcOpType;
	DxOperator_e				eNotSvcOpType;
	HBOOL					bNewSvc;
	DbSvc_UserFlag1_e		eUserFlag1, eUserMask1;
	HINT32					eJpNetType;
} DbSvc_Attr_t;

/** @brief channel db 검색 조건 */
typedef enum
{
	eSvcFindDirection_Current = 0,
	eSvcFindDirection_Position = 1,

	/* find 에서 사용되는 경우 */
	eSvcFindDirection_Prev,
	eSvcFindDirection_Next,
	eSvcFindDirection_First,
	eSvcFindDirection_Last,
	eSvcFindDirection_PrevBySortNum2,
	eSvcFindDirection_NextBySortNum2,

	/* find list에서 사용되는 경우 - chlist에서 사용하는 검색의 의미를 명확히 하고자, 명칭을 재정의 하였다 */
	eSvcFindDirection_SearchedOrder,
	eSvcFindDirection_Foward,
	eSvcFindDirection_Backward,
	eSvcFindDirection_FowardFromStart,
	eSvcFindDirection_BackwardFromLast,

	/* Find list에서 사용하며, lcn외에 특정 수로 sorting할 목적임. */
	eSvcFindDirection_FowardFromStartBySortNum,
	eSvcFindDirection_BackwardFromLastBySortNum,
	eSvcFindDirection_FowardFromStartBySortNum2,
	eSvcFindDirection_BackwardFromLastBySortNum2,
} DbSvc_FindDirection_e;

typedef struct
{
	DbSvc_Group_e 			eSvcGroup;
	HUINT32 				ulMaxFindNum;
	Handle_t 				hOffsetSvc;
	DbSvc_FindDirection_e 	eDirection;
	DbSvc_Attr_t 			stAttr;
} SVC_FindCondition_t;

/**
	@brief fav service가 어느 group에 소속되어 있는 지를 나타내는 값.
			service info add하는 것과는 API가 구분되어 있으므로, fav group의 의미를 명확히 하기 위해, 아래와 같은 type을 두었다
			fav group 끼리 OR도 가능
*/
typedef enum
{
	eFavGroup_All = 0,
	eFavGroup_1   = 0x01,
	eFavGroup_2   = 0x02,
	eFavGroup_3   = 0x04,
	eFavGroup_4   = 0x08,
	eFavGroup_5   = 0x10
} DbSvc_FavGroup_e;

typedef enum
{
	eAudio_No_UserSet_MPEG = 0,
	eAudio_No_UserSet_Dolby,
	eAudio_UserSet_MPEG,
	eAudio_UserSet_Dolby
} DbSvc_AudUserSet_e;

/** 필드별로 update 여부를 지정가능하다 */
typedef enum tagSvcFieldList
{
	eSvcField_All 						= 0,
	eSvcField_01_usLcn					= 0x00000001,
	eSvcField_02_szSvcName				= 0x00000002,
	eSvcField_03_bModifiedName			= 0x00000004,
	eSvcField_04_bLocked				= 0x00000008,
	eSvcField_05_eUserFlag1				= 0x00000010,
	eSvcField_06_ulRatingPassedEventId	= 0x00000020,
	eSvcField_07_bAudUserSetFlag		= 0x00000040,
	eSvcField_08_bSubttlUserFlag		= 0x00000080,

	/* 사용하지 않는 것(하지만, 가까운 미래에 사용될 수도 있는)은 아래처럼 reserved로 잡아둠 */
	/*
	eSvcField_rr_bLcnFlag,
	eSvcField_rr_bVisibleSvcFlag,
	eSvcField_rr_usPmtPid,
	eSvcField_rr_usPcrPid,
	eSvcField_rr_usTtxPid,
	eSvcField_rr_usVideoPid,
	eSvcField_rr_eVideoCodec,
	eSvcField_rr_usAudioPid,
	eSvcField_rr_eAudioCodec,
	eSvcField_rr_usAudioAuxPid,
	eSvcField_rr_eAudioAuxCodec,
	eSvcField_rr_usDolbyPid,
	eSvcField_rr_eDolbyCodec,
	eSvcField_rr_bDolbyFlag,
	eSvcField_rr_eSvcType,
	eSvcField_rr_eOrgSvcType,
	eSvcField_rr_eVideoType,
	eSvcField_rr_eCasType,
	eSvcField_rr_eSection,
	eSvcField_rr_ucSubttlIdx,
	eSvcField_rr_eSoundMode,
#ifdef CONFIG_OP_FREESAT
	eSvcField_rr_usFSatSvcIdentifier,
	eSvcField_rr_usFSatReginId,
#endif
#ifdef CONFIG_DB_SVC_SVCGROUP
	eSvcField_rr_szGroupList,
#endif
	*/

	eSvcField_32_max_reserved		= 0x80000000
} DbSvc_FieldList_e, DbSvc_FieldList_b;

/* SVC_FieldList_t의 확장 버젼으로 SVC_FieldList_t의 32개 필드를 모두 사용한 경우, 여기에 추가로 채워나간다 */
typedef enum tagSvcFieldListExt
{
	eSvcField_Ext_All 				= 0,
	eSvcField_Ext_Not_Used			= eSvcField_Ext_All,
	eSvcField_33_reserved 			= 0x00000001,

	/* ... */

	eSvcField_64_max_reserved 		= 0x80000000
} DbSvc_FieldListExt_e, DbSvc_FieldListExt_b;

/** @brief target region info structure */
typedef struct
{
	HUINT8				szCountryCode[4];
	HUINT8				ucRegionDepth; // 0xFF is Invalid mark
	HUINT32				ulRegionCode; // Primari(8) + Second(8) + Teritary(16)
} DbSvc_T_RegionInfo_t;

typedef struct
{
	HUINT8				szCountryCode[4];
	HUINT8				ucRegionDepth; // SVC_TRegion_Level_t
	HUINT32				ulRegionCode; // Primari(8) + Second(8) + Teritary(16)
	HUINT8				szTRegionName[DxMAX_T_REGION_NAME_LEN];
} DbSvc_T_RegionNameInfo_t;


typedef struct
{
	HUINT8				ucGuidanceType;
	HUINT8				ucGuidanceMode;
	HUINT8				szGuidanceStr[DxMAX_GUIDANCE_INFO_LEN];
} DbSvc_GuidanceInfo_t;

typedef struct
{
	HUINT16 					usFSatSvcIdentifier;
	HUINT16 					usFSatReginId;
	HUINT8					szSvcShortName[DxMAX_FSAT_SVC_SHORT_NAME_LEN];
	HUINT32 					ulFTAContentMgr;
	DbSvc_GuidanceInfo_t		stGuidanceInfo;
	HUINT8					ucPresentFollow;
	HUINT8					aucDefaultAuthority[DxMAX_DEFAULT_AUTHORITY_LEN];
} DbSvc_InfoFreeSat_t;

typedef struct
{
	HUINT16 			usSesVirtualSvcId;		/**< SES LCN에서 정의하는 Virtual Service ID를 의미한다. */
	HBOOL				bNewSvc;				/**< SES LCN에서 New Service로 올라오는 argument. */
} DbSvc_InfoSes_t;


typedef struct
{
	DbSvc_GuidanceInfo_t		stGuidanceInfo;
	HUINT32 					ulFTAContentMgr;
	HUINT8						aucDefaultAuthority[DxMAX_DEFAULT_AUTHORITY_LEN];
	DbSvc_T_RegionInfo_t		stRegionInfo[DxMAX_T_REGION_NUM];
	HUINT16						usLcnSub;

} DbSvc_InfoTvAnyTime_t;

typedef struct
{
	HUINT8						aucDefaultAuthority[DxMAX_DEFAULT_AUTHORITY_LEN];
} DbSvc_InfoAusDtt_t;

typedef struct
{
        DbSvc_GuidanceInfo_t            stGuidanceInfo;
        HUINT32                                         ulFTAContentMgr;
        HUINT8                                          aucDefaultAuthority[DxMAX_DEFAULT_AUTHORITY_LEN];
        DbSvc_T_RegionInfo_t            stRegionInfo[DxMAX_T_REGION_NUM];
        HUINT16                                         usLcnSub;

} DbSvc_InfoUkDtt_t;

/** @brief service info structure, 반드시 usTsIdx와 usPrvIdx가 지정되어야 한다. */
#if defined (CONFIG_OP_SES)
typedef struct
{
	DxService_t			stDxInfo;

	// 임시 추가: CAS 담당자와 의논 후 DxService_t에 넣을 것인지 다르게 처리할 것인지 결정하도록 한다.
	// ToDo: Start
#if defined(DbSvc_SUPPORT_DXINFO)
#else
		HUINT16 			usSvcIdx;
		/////////////////////////////////////////
		//	변경 할수 없는 값들. 서치 시 결정됨
		HUINT16 			usTsIdx;		   /**< 서치시 할당됨. 변경 불가. service가 속한 TBL_TS.tsIdx -> 현재의 unique_id 임 */
		HUINT16 			usPrvIdx;		   /**< 서치시 할당됨. 변경 불가. service가 속한 TBL_PRV.prvIdx */

		HUINT16 			usSvcId;		   /**< 서치시 할당됨. 변경 불가. service id */
		HUINT16 			usTsId; 		   /**< 서치시 할당됨. 변경 불가. transport stream id */
		HUINT16 			usOnId; 		   /**< 서치시 할당됨. 변경 불가. original network id */
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		HUINT8				szSvcName[/*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength()];	/**< service name. 이 값은 SDT에서 지정되거나 사용자가 수정할 수 있다 */
		HUINT16 			usLcn;							/**< retail이라도 humax rule에 따라 기본 LCN을 만든다. search index와 같더라도 lcn을 사용함 */
		HBOOL				bLcnFlag;						/**< LCN descriptor와 같은 LCN사양에 의해 channel number가 할당되었다면, TRUE로 저장 */
		HBOOL				bVisibleSvcFlag;				/**< LCD syntax에 존재하는 경우, 그 내용에 따라 TRUE/FALSE 값이 할당 가능하고, LCD syntax에 없다면 항상 TRUE */

		/////////////////////////////////////////
		//	PID & CODEC
		HUINT16 			usPmtPid;				/**< pmt pid, pat에 정의됨 */
		HUINT16 			usPcrPid;				/**< pcr pid from pmt */
		HUINT16 			usTtxPid;				/**< teletext pid from pmt */

		HUINT16 			usVideoPid; 			/**< video pid from pmt */
		DxVideoCodec_e		eVideoCodec;			/**< 해당 es의 video codec, db file에 1 byte로 저장됨 */

		HUINT16 			usAudioPid; 			/**< audio pid from pmt */
		DxAudioCodec_e		eAudioCodec;			/**< 해당 es의 audio codec, db file에 1 byte로 저장됨 */

		HUINT16 			usAudioAuxPid;			/**< for audio descriptor or dolby */
		DxAudioCodec_e		eAudioAuxCodec; 		/**< 해당 es의 audio codec, db file에 1 byte로 저장됨 */

		HUINT16 			usDolbyPid; 			/**< dolby audio pid from pmt */
		DxAudioCodec_e		eDolbyCodec;			// TODO: 설명 추가
		HBOOL				bDolbyFlag; 			/**< main audio가 dolby인 경우 TRUE로 세팅 */
													/*----------------------------------------------------------------------
														bDolbyFlag이 TRUE가 되는 경우는,
														(1) sound track 창에서 dolby audio를 선택하거나,
														(2) menu의 dolby priority가 설정되고, dolby audio가 들어오는 서비스
															-> MW SI에서 판단하여 설정됨
													-----------------------------------------------------------------------*/

		/////////////////////////////////////////
		//	다음은 service 속성들이다.
		//---------------------------------------
		/*
			~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			[eSvcType vs. eOrgSvcType]의 개념 설명, 2009-05-15
			~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				- eOrgSvcType은 나중에 추가된 개념이다.
				- SDT에 들어오는 service_descriptor를 기반으로 Tv/Radio/Data와 같은 service_type이 결정되나, 필요에 따라 Data service를 Tv로 구분할 필요가 있다.
				- 주로 chlist에서 DB검색을 위해 Data를 Tv로 분류하는데, Tv 채널 검색 조건으로 주는 경우, Data까지 같이 검색하서 주는 식이다.
				- eSvcType이 TV인지 Data인지는 service time에 SI모듈에서 결정되며,
				- service_descriptor의 내용은 eOrgSvcType에 저장하고, eSvcType은 Humax 시나리오에 맞게 재정의 한다.

			물론, DxSvcType_e의 service type값을 OR해서 검색 조건을 줄 수도 있으나, Tv+Data를 구분지어 처리하는 경우, ui application의 복잡도가 증가하게 된다.
			이에 대한 타협점으로, DB에 eOrgSvcType을 둔 것이다.
		*/
		DxSvcType_e 			eSvcType;
		DxSvcType_e 			eOrgSvcType;
		DxVideoType_e			eVideoType; 			/**< all/sd/hd */
		DxCasType_e 			eCasType;				/**< ca system id에 따라 미리 정의된 값으로 OR 해서 여러개의 CAS가 동시에 지정될 수 있다. all/fta/nagra/irdeto/nds/ ... */
		DxSvcSection_e			eSection;				/**< channel list section type - pre-programmed/Lcn/Non-Lcn */
		DxOperator_e			eSvcOpType;

#if defined(CONFIG_DB_SVC_LCN_RENUMBER) || defined(CONFIG_DB_SVC_SAVE_ORG_LCN)
		HUINT16 			usOrgLcn;
#endif

#if defined(CONFIG_MW_CI)
		HUINT32 			ulInstType;
		HUINT16 			usSlotId;
	#if defined(CONFIG_MW_CI_PLUS)
		HUINT32 			ulTimeStamp;							/** CI protection descriptor save time **/
		HUINT8				ucNumOfEntries; 						/** number_of_entries, match_brand_flag가 false라면 이 값은 0 **/
		HUINT16 			ausCamBrandId[MAX_CAM_BRAND_NUM];		/** cicam_brand_identifier **/
		HBOOL				bSelectableSvcFlag;
		HUINT8				szOperatorName[MAX_CAM_OPERATOR_NAME_LEN];
		HUINT8				aucCamId[CICAM_ID_LEN];
	#endif
#endif

		/*
			~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			[eOrgDeliType, original delivery type]의 개념 설명
			~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				- octo에서 처음 사용됨, 080625
				- 실제 보여지는 위성/지상파/채널/아날로그 채널 군으로 서비스가 속한 채널의 delivery 속성을 나타낸다.
				- 채널리스트에 보여질 때 활용됨.
				- tuning parameter와는 별도의 의미로 사용된다.
				- 만약 케이블 스트림에 지상파 채널이 들어 있다면 tuning param의 delievery system은 케이블이 되고,
				- 서비스의 original delievery system은 지상파가 되는 것이다.
		*/
		DxDeliveryType_e		eOrgDeliType;
		SVC_SatType_t			eSatType;				/**< 위성전용값, antenna info의 satellite type (note: antenna info가 setup.db에 있기 때문에, satellite filter로 검색 시 너무 오래 걸렸었음. 080928) */
		HUINT8					ucAntIdx;				/**< 위성전용값, delivery type이 satellite인 경우, unique service의 id로 antenna index가 필요. ts info의 ant idx와 같은 값을 갖는다 */
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		/////////////////////////////////////////
		//	user에 의한 설정값들
		//---------------------------------------
		HBOOL					bModifiedName;			/**< service 명이 사용자에 의해 수정된 경우 TRUE */
		HBOOL					bLocked;				/**< lock by user */
		HUINT8					ucSubttlIdx;			/**< user에 의해 선택 된 Subtitle Index */
		HBOOL					bSubttlUserFlag;		/**< user에 의해 선택 되었는지를 나타내는 Flag */
		DxStereoSelect_e		eSoundMode; 			/**< sound mode : STEREO, MONO L, MONO R, MONO L/R */
		DxDualMonoSelect_e		eDualmonoSelect;		/**< dualmono mode : BOTH, MONO L, MONO R */
		HUINT8					aucAudioLanguage[DxLANG_CODE_LEN];	/**< Audio Language User Selection */
		HBOOL					bAudUserSetFlag;		/**< sound track menu에서 user가 다른 언어나 dolby audio, audio description 등을 선택했는지 안했는 지의 여부를 나타냄 */
		HBOOL					bAudioDescription;		/**< aduio description : User Select Audio Description 적용 여부 */
		HUINT32 				ulRatingPassedEventId;	/**< rating check pass한 경우 service info에 event id를 저장해둔다.
															pass하지 않은 경우, 0xffffffff값을 갖는다.
															event_id는 0~0xffff 사이의 16비트 값을 가지며, memory db에만 저장되는 내용으로 power reset후 날라가는 값 */
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		/////////////////////////////////////////
		//	service 속성 중 HBOOL type을 묶어서 사용한다
		//---------------------------------------
		DbSvc_UserFlag1_e			eUserFlag1; 			/**< 32개 까지 지정 가능한 공용 user flag - enum값 기반으로 OR 연산하면됨 */
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		union
		{
			DbSvc_InfoFreeSat_t 		stFreeSat;
			DbSvc_InfoSes_t 			stSes;
			DbSvc_InfoAusDtt_t			stAusDtt;
			DbSvc_InfoUkDtt_t			stUkDtt;
			// <-- Add Here
		} utExt;

#endif

#ifdef CONFIG_DB_SVC_SVCBOUQUET
		HUINT8				szBouquetList[DxBOUQUET_LIST_BUF_LEN];	/**< service가 속한 bouquet의 종류들을 가리킴 */
#endif

#if defined(CONFIG_DB_SVC_SVCGROUP)
		HUINT8				szGroupList[DxGROUP_LIST_BUF_LEN];			/**< service가 속한 Group 의 종류들을 가리킴 */
#endif

} DbSvc_Info_t;
#else
typedef struct
{
	DxService_t			stDxInfo;

	// 임시 추가: CAS 담당자와 의논 후 DxService_t에 넣을 것인지 다르게 처리할 것인지 결정하도록 한다.
	// ToDo: Start
#if defined(CONFIG_MW_CI)
	#if defined(CONFIG_MW_CI_PLUS)
		HUINT32 			ulTimeStamp;							/** CI protection descriptor save time **/
		HUINT8				ucNumOfEntries; 						/** number_of_entries, match_brand_flag가 false라면 이 값은 0 **/
		HUINT16 			ausCamBrandId[MAX_CAM_BRAND_NUM];		/** cicam_brand_identifier **/
		HBOOL				bSelectableSvcFlag;
	#endif
#endif

	/////////////////////////////////////////
	//	service 속성 중 HBOOL type을 묶어서 사용한다
	//---------------------------------------
	DbSvc_UserFlag1_e			eUserFlag1; 			/**< 32개 까지 지정 가능한 공용 user flag - enum값 기반으로 OR 연산하면됨 */
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
} DbSvc_Info_t;
#endif

#if 0
/* LCN 추가 시 필요한 info */
typedef struct
{
	Handle_t 			hSvc;
	HUINT16				usSvcId;
	HBOOL				bVisibleSvcFlag;
	HBOOL				bDeleteSvcFlag;
	HBOOL				bMoveSvcFlag;
	HINT32				nSearchIdx;
	HINT32				nLcn;
	HINT32				nUserNum;
	DxDeliveryType_e	eOrgDeliType;
} DbSvc_LcnInfo_t;
#endif


typedef struct
{
	HUINT32						ulFrequency;
	HUINT32						ulSymbolRate;
	DxSat_Polarization_e		ePolarization;
	DxSat_CodeRate_e			eFecCodeRate;

	/** @brief DVB-S2 modulation system's parameters */
	DxSat_TransportSpec_e		eTransSpec;
	DxSat_PskModulation_e		ePskMod;
	DxSat_Pilot_e				ePilot;
	DxSat_RollOff_e				eRollOff;

	/**< satellite type이 필요한 이유는,
		pre-defined chlist와 같이 antenna info가 나중에 정해지는 경우,
		antenna index와 mapping 시킬 수 있는 유용한 정보가 필요하기 때문 */
	SVC_SatType_t				eSatType;

	HUINT8						ucAntId;

	DxSat_TuningTPInfo_t		stDxInfo;
} DbSvc_SatTuningInfo_t;

typedef struct
{ // TODO: enum 변수명 변경
	/* not properly defined at this time */
	HUINT32						ulFreq;
	HUINT16						usSR;
	HUINT16						usNetworkId;
	DxCab_Constellation_e		ucCon;
	DxCab_Spectrum_e			ucSpec;
} DbSvc_CabTuningInfo_t;

#if defined(CONFIG_MW_CH_DVB_T2)
typedef struct
{
	HUINT32							ulPlpId;
	DxTer_PreambleFormat_e			ucPreambleFormat;
	DxTer_Mixed_e					ucMixed;
	DxTer_Pilot_Pattern_e			ucPilotPattern;
	DxTer_ExtendedCarrier_e			ucExtenedCarrier;
	DxTer_PAPR_Reduction_e			ucPAPRreduction;
	HUINT32							ulNumPlpBlocks;
} DbSvc_TerT2TuningInfo_t;
#endif

typedef struct
{ // TODO: enum 변수명 변경
	HUINT32					ulFreq;				/**< centre_frequency (mutiples of 10Hz) */
	DxTer_BandWidth_e		ucBandwidth;		/**< bandwidth     : 8/7/6 MHz */
	DxTer_Constellation_e	ucConstellation;	/**< constellation : QPSK/16-QAM/64-QAM */
	DxTer_Hierachy_e		ucHierachy;			/**< hierachy information : signalling format for the alpha values */
	DxTer_Stream_e			ucStream;			/**< HP/LP */
	DxTer_CodeRate_e		ucCodeRate;			/**< FEC */
	DxTer_GuardInterval_e	ucGuardInterval;	/**< 1/32, 1/16, 1/8, 1/4 */
	DxTer_TransMode_e		ucTransMode;		/**< 2k/8k */
	DxTer_Offset_e			ucOffset;
	HUINT8					ucChanNum;
	HUINT8					ucLevel;
	HUINT8					ucQuality;
#if defined(CONFIG_MW_CH_DVB_T2)
	DxTer_System_e			ucSystem;		/**< T/T2 */
	DbSvc_TerT2TuningInfo_t	stT2param;
#endif
} DbSvc_TerTuningInfo_t;

typedef struct
{ // TODO: enum 변수명 변경
	HUINT32					ulFreq;			/**< analog channel tuning frequency - what Hz? */
	HUINT8					usProgNum;		/**< Program의 Number of Current Display */
	SVC_CH_ANA_TunerBand_e	ucBand;			/**< Band (Frequency에 따라 결정.) of Current Display */
	HUINT8					ucChanNum;		/**< Channel Number (Frequency에 따라 결정.) of Current Display */
	HUINT8					ucOffset;		/**< Fine Tune Offset (유저 조정 값) of Current Display */
	ANA_VideoStandard_t		ucColorSystem;	/**< Color System : PAL, SECAM, NTSC... */
	ANA_AudioStandard_t		ucAudioSystem;	/**< Audio System : BG, DK, L... */
	HUINT8					ucFineTuneFlag;	/**< for manual finetune 0 : not fine tuned, 1 : find tuned */
} DbSvc_AnaTuningInfo_t;

typedef struct
{
	HUINT32 			ulFTAContentMgr;
} DbSvc_TsInfoFreeSat_t;

typedef struct
{
	HUINT32 				ulFTAContentMgr;
	HBOOL 					bFreqListed;
	HUINT8					aucDefaultAuthority[DxMAX_DEFAULT_AUTHORITY_LEN];
	DbSvc_T_RegionInfo_t	stRegionInfo[DxMAX_T_REGION_NUM];
} DbSvc_TsInfoTvAnyTime_t;

typedef struct
{
	HBOOL 				bFreqListed;
	HUINT8				aucDefaultAuthority[DxMAX_DEFAULT_AUTHORITY_LEN];
} DbSvc_TsInfoItaDtt_t;

typedef struct
{
	HBOOL 				bFreqListed;
	HUINT8				aucDefaultAuthority[DxMAX_DEFAULT_AUTHORITY_LEN];
} DbSvc_TsInfoNorDig_t;
/** @brief tuning parameter */
typedef union
{
	DbSvc_SatTuningInfo_t		sat;
	DbSvc_CabTuningInfo_t		cab;
	DbSvc_TerTuningInfo_t		ter;
	DbSvc_AnaTuningInfo_t		ana;
} DbSvc_TuningInfo_t;

/** @brief ts info structure */
typedef struct
{
	HUINT16				usTsIdx;			/**< db에 저장 시 부여되는 primary key */
	HUINT16             usUniqueId;			/**< tp를 unique하게 구분 짓는 id. 1부터 순차적으로 할당됨. DB사용할 경우 ts index로 대체 가능함 */
	HUINT16             usNetIdx;			/**< ts가 속한 network를 가리킴 */
	HUINT16             usTsId;				/**< ts id */
	HUINT16             usOnId;				/**< original network id */
 	HUINT8              ucTunerId;			/**< tuner id : 0, 1, 2, ... */
 	DxDeliveryType_e  	eDeliType;			/**< tuner가 받아 들이는 신호방식을 의미한다. 케이블을 통해서 지상파가 재전송이 된 경우라면 케이블로 설정해야 함. */
	DbSvc_TuningInfo_t    stTuningParam;		/**< tuning parameter */

	union
	{
		DbSvc_TsInfoFreeSat_t		stFreeSat;
		DbSvc_TsInfoTvAnyTime_t		stTvAnyTime;
		DbSvc_TsInfoItaDtt_t		stItaDtt;
	} utExt;
} DbSvc_TsInfo_t;

/** @brief provider info structure */
typedef struct
{
	HUINT16				usPrvIdx;			/**< db에 저장 시 부여되는 primary key */
	HUINT8				szPrvName[DxMAX_PRV_NAME_LEN];
} DbSvc_PrvInfo_t;

typedef struct
{
	HUINT32 			ulFTAContentMgr;
} DbSvc_NetInfoFreeSat_t;

typedef struct
{
	HUINT16				ulmin_polling_interval;
	HUINT8				aucUriPrefix[DxMAX_URI_PREFIX_INFO_LEN]; // 0xFF is Invalid mark
} DbSvc_T_UriPrefixInfo_t;

typedef struct
{
	HUINT32 				ulFTAContentMgr;
	HUINT8					aucDefaultAuthority[DxMAX_DEFAULT_AUTHORITY_LEN];
	DbSvc_T_RegionInfo_t	stRegionInfo[DxMAX_T_REGION_NUM];
	DbSvc_T_UriPrefixInfo_t	stUriPrefix;
} DbSvc_NetInfoTvAnyTime_t;

/** @brief network info structure */
typedef struct
{
	HUINT16				usNetIdx;			/**< db에 저장 시 부여되는 primary key */
	DxDeliveryType_e	eOrgDeliType;		/**< Ter+Sat 등과 같은 combo 모델의 경우, 중복되는 network을 구분하기 위해서 original delivery type도 같이 저장한다. dynamic update를 위해서도 유용함 */
	HUINT16      		usNetId;
	HUINT8       		szNetName[DxMAX_NET_NAME_LEN];

	union
	{
		DbSvc_NetInfoFreeSat_t		stFreeSat;
		DbSvc_NetInfoTvAnyTime_t	stTvAnyTime;
	} utExt;

} DbSvc_NetInfo_t;

/** @brief bouquet info structure */
typedef struct
{
	HUINT16				usBouqIdx;			/**< db에 저장 시 부여되는 primary key */
	HUINT16				usBouqId;
	HUINT8				szBouqName[DxMAX_BOUQ_NAME_LEN];

#if defined(CONFIG_DB_SVC_SVCBOUQUET_MULTILANG_NAME)
	HUINT32				ulMultiBouqNameNum;
	DxMultiBouqName_t 	astMultiBouqName[DxMAX_MULTI_BOUQ_NAME_NUM];
#endif
} DbSvc_BouqInfo_t;

typedef struct
{
	HUINT8				ucGroupType;
	HUINT8				ucNondestructiveTuneFlag;
	HUINT8				ucReturnChannelAccessFlag;
	HUINT8				szIso639LangCode[DxLANG_CODE_LEN];
// CONFIG_OP_FREESAT_GEN2
	HUINT8				ucG2ExtensionFlag;
	HUINT8				ucDefaultVisableFlag;
	HUINT8				ucAdultChannelsGroupFlag;
	HUINT8				ucIpChannelsGroupFlag;
	HUINT8				ucCaChannelsGroupFlag;
} DbSvc_GroupInfoFreeSat_t;

/** @brief genre info structure */
typedef struct
{
	HUINT16				usGroupIdx;			/**< db에 저장 시 부여되는 primary key */
	HUINT32				ulGroupId;
	HUINT8				szGroupName[DxMAX_GROUP_NAME_LEN];
	union
	{
		DbSvc_GroupInfoFreeSat_t	stFreeSat;
	} utExt;
} DbSvc_GroupInfo_t;

/** triple id structure */
typedef struct
{
	HUINT16				usOnId;		/**< originl network id */
	HUINT16				usTsId;		/**< ts id */
	HUINT16				usSvcId;	/**< service id */
} DbSvc_TripleId_t;

#if 0
/** nit's network ts info structure */
typedef struct
{
	HUINT16             usNetIdx;		/**< ts가 속한 network를 가리킴 */
	HUINT16             usTsId;			/**< ts id */
	HUINT16             usOnId;			/**< original network id */
 	DxDeliveryType_e  	eDeliType;		/**< tuner가 받아 들이는 신호방식을 의미한다. 케이블을 통해서 지상파가 들어온다면 지상파가 맞는 타입임. --> QPSK, QAM, OFDM으로 나누는 게 맞는 거 아닐까? */
} DbSvc_NetTsInfo_t;
#endif

/** svc info search condition
	svc info의 일부 field를 지원하고, 검색조건에 포함시키지 않는 경우 enum 값의 경우 all을 그외에 값은 -1을 부여하면 된다 */
typedef struct
{
	HINT32				nAntIdx;
	HINT32				nOnId;			/**< original network id */
	HINT32				nTsId;
	HINT32				nSvcId;
	HINT32				nSvcType;
	HINT32				nVideoType;
	HINT32				nCasType;
	HINT32				nOrgDeliType;
	HINT32				nTsIdx;
	HINT32				nPrvIdx;
	HINT32				nLcnFlag;
	HINT32				nStartLcn;
	HINT32				nEndLcn;
	HINT32				nSection;
	HINT32				eSvcOpType;
	HINT32				eSatType;
	DbSvc_UserFlag1_e		eUserMask1;// same as DbSvc_Attr_t.eUserMask1
} DbSvc_Condition_t;

/** ts info search condition
	ts info의 대부분의 field를 지원하고, 검색조건에 포함시키지 않는 경우 enum 값의 경우 all을 그외에 값은 -1을 부여하면 된다 */
typedef struct
{
	HINT32				nUniqueid;
	HINT32				nNetIdx;
	HINT32				nTsId;
	HINT32				nOnId;
	HINT32				nTunerId;
	HINT32				nDeliType;
} DbSvc_TsCondition_t;


#define MAX_ANT_NAME      						40

typedef enum
{ /* [WARN] forbid to change a db-saving enum value */
	eWord_Disable 				= 0,
	eWord_Enable				= 1,
	eWord_MAX
} DbSvc_ConfirmWord_e;

typedef enum
{ /* [WARN] forbid to change a db-saving enum value */
	eDymicUpdate_Disable 		= 0,
	eDymicUpdate_Enable			= 1,
	eDymicUpdate_MAX
} DbSvc_DynamicUpdate_e;

/* itemValue : Use below enum */
typedef enum
{ /* [WARN] forbid to change a db-saving enum value */
	eUiAntType_LNB_Only			= 0,
	eUiAntType_DiSEqC_1_0		= 1,
	eUiAntType_DiSEqC_2_0		= 2,
	eUiAntType_Multiswitch		= 3,
	eUiAntType_SCD				= 4,
	eUiAntType_Motor			= 5,			// diSEqC 1.2
/*	eUiAntType_SMATV,	*/
	eUiAntType_Monoblock_LNB	= 6,			// diSEqC 1.2
	eUiAntType_MAX
} DbSvc_UiAntType_e;

typedef struct
{
	/* LNB Parameters */
	HUINT32 						ulLnbFreq;				/**< LNB frequency 또는 CH_UNIVERSAL_LNB_FREQ(0xffffffff) */
	DxSAT_LnbVoltage_e 				eLnbVoltage; 			/**< Voltage drop compensation */
} DbSvc_LnbAntInfo_t;

typedef struct
{
	/* LNB Parameters */
	HUINT32 						ulLnbFreq;				/**< LNB frequency 또는 CH_UNIVERSAL_LNB_FREQ(0xffffffff) */
	DxSAT_LnbVoltage_e 				eLnbVoltage; 			/**< Voltage drop compensation */
	HBOOL							b22kTone;				/**< 22KHz Tone : On/Off */

	/* Diseqc Switch Parameters */
	DxSAT_DiseqcVer_e				eDiseqcVer;				/**< Diseqc Ver. 1.0, 1.1, 1.2, 1.3, 2.0 */
	DxSAT_DiseqcInput_e				eDiseqcInput;			/**< Diseqc A, B, C, D */
	DxSAT_ToneBurst_e				eToneBurst;				/**< NONE, A, B */

	/* Diseqc 1.2, 1.3 motor parameters */
	HUINT32							ucMotorPosition;		/**< for DiSEqC 1.2 */
	HINT32							sSatLonggitude; 		/**< for DiSEqC 1.3 */
} DbSvc_DiseqcAntInfo_t;

typedef struct
{
	HUINT32 						ulLnbFreq;
	DxSAT_LnbVoltage_e 				eLnbVoltage;
	HBOOL							b22kTone;

	DxSAT_ScdInput_e				eScdInput;
	HUINT32 						ulScdUserBandNum;		// For Tuner 0
	HUINT32 						ulScdUserBandFreq;
	HUINT32							ulScdUserBandNum1;		// For Tuner 1
	HUINT32							ulScdUserBandFreq1;
	HUINT32 						ulScdUserBandNum2;		/**< Tuner 2용 SCD user band number. 설정에서 읽어서 넣어라. */ // MOON_SCD
	HUINT32 						ulScdUserBandFreq2;		/**< Tuner 2용 SCD user band frequency. 설정에서 읽어서 넣어라. */
	HUINT32 						ulScdUserBandNum3;		/**< Tuner 3용 SCD user band number. 설정에서 읽어서 넣어라. */
	HUINT32 						ulScdUserBandFreq3;		/**< Tuner 3용 SCD user band frequency. 설정에서 읽어서 넣어라. */
} DbSvc_ScdAntInfo_t;

typedef struct
{
	SVC_SatType_t				 eSatType;
	HUINT8 						 szName[MAX_ANT_NAME];

	DxAntennaType_e		 		 eAntennaType;
	union
	{
		DbSvc_LnbAntInfo_t		 stLnbAnt;
		DbSvc_DiseqcAntInfo_t	 stDiseqcAnt;
		DbSvc_ScdAntInfo_t		 stScdAnt;
	} utAntInfo;

} DbSvc_AntInfo_t;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/**
  @description	: service db module의 내부 변수 초기화. 부팅 단계에서 한번만 불린다.
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - 초기화 실패
*/
void		DB_SVC_Commit (void);
HERROR		DB_SVC_GetChanged (DbSvc_Table_e eTable, void **apvList, HINT32 *pnUpdateNum, void **apvRemoveList, HINT32 *pnRemoveNum);
void		DB_SVC_SetNotifier (HBOOL bSet);

#if 0
/**
  @description	: enum값 기반인 user flag 값을 알아 볼 목적
  @type			: Inline
*/
HBOOL		DB_SVC_GetUserFlag1(DbSvc_Info_t *pstSvcInfo, DbSvc_UserFlag1_e eUserFlag1);

/**
  @description	: enum값 기반인 user flag 값을 setting할 목적
  @type			: Inline
*/
void		DB_SVC_SetUserFlag1(DbSvc_Info_t *pstSvcInfo, DbSvc_UserFlag1_e eUserFlag1, HBOOL bSetValue);
#endif
/**
  @description	: enum값 기반인 user flag 값을 알아 볼 목적
  @type			: Inline
*/
HUINT32		DB_SVC_GetUserFlag1(const DbSvc_Info_t *pstSvcInfo);



/**
  @description	: cache된 service db를 flash또는 hdd와 sync시킨다.
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - sync 실패
*/
HERROR		DB_SVC_Sync(ApiSyncMode_t eApiSync);


/**
  @description	: service handle 관련 hash table init
  @type			: Inline
*/
void		DB_SVC_SyncHash (void);

/**
  @description	: memory db를 포함하여 모든 service db를 해제한다.
  @type			: Inline
*/
void		DB_SVC_Close(void);

/**
  @description	: pre-programmed channel list를 memory db로 로딩한다.
  				  pre-chlist는 현재 csv(comma-seperated..) 파일을 사용 중이며, 국가코드로 분리되어 있다.
				  antenna 정보가 미리 세팅되어 있어야 채널을 로드할 수 있음
  @type			: sync(INLINE) or async as ApiSyncMode_t
  @return	ERR_OK : 완료
 			ERR_FAIL : 실패
*/

HERROR		DB_SVC_LoadPreProgrammedChannel(HxCountry_e eCountryId);


/**
  @description	: 모든 service/ts/provider/network info 등 service db의 모든 record 삭제
  @type			: Inline
  @return		: 없음
*/
void		DB_SVC_ClearAllInfo(void);

/*
  @description	: file db의 내용을 memory db로 로딩 완료 후 TRUE를 return 한다. file db 사용 상태일 때는 FALSE 리턴
  @type			: Inline
*/
HBOOL		DB_SVC_IsMemoryLoadingDone(void);

/**
  @description	: DB_SVC_AddXXXInfo() 각 table에 info(record 하나)를 추가하는 API.
  @type			: Inline
  @param 1		: [in]  XXXinfo structure to be added
  @param 2		: [out] service handle
  @return		: ERR_OK
				: ERR_FAIL - add 실패
*/
HERROR		DB_SVC_InitServiceInfo (DbSvc_Info_t *pstSvcInfo);
HERROR		DB_SVC_ServiceSetUserFlag(DbSvc_Info_t *pstSvcInfo, const DbSvc_UserFlag1_e bwFlag);
HERROR		DB_SVC_ServiceGetUserFlag(DbSvc_UserFlag1_b *pbwFlag, const DbSvc_Info_t *pstSvcInfo);
HERROR		DB_SVC_ServiceCopyUserFlag(DbSvc_Info_t *pstDest, const DbSvc_Info_t *pstSource);

HERROR		DB_SVC_AddServiceInfo(DbSvc_Info_t *pstSvcInfo, Handle_t *phSvc);
HERROR		DB_SVC_AddNetworkInfo(DbSvc_NetInfo_t *pstNetworkInfo, HUINT16 *pusNetIdx);
HERROR		DB_SVC_AddTsInfo(DbSvc_TsInfo_t *pstTsInfo, HUINT16 *pusTsIdx);
HERROR		DB_SVC_AddProviderInfo(DbSvc_PrvInfo_t *pstPrvInfo, HUINT16 *pusPrvIdx);

HUINT32		DB_SVC_PutServiceInfoList (const DbSvc_Info_t *pstList, HUINT32 nListNum);
HUINT32		DB_SVC_PutNetworkInfoList (const DbSvc_NetInfo_t *pstList, HUINT32 nListNum);
HUINT32		DB_SVC_PutTsInfoList (const DbSvc_TsInfo_t *pstList, HUINT32 nListNum);
HUINT32		DB_SVC_PutProviderInfoList (const DbSvc_PrvInfo_t *pstList, HUINT32 nListNum);
HUINT32		DB_SVC_PutGroupInfoList (const DbSvc_GroupInfo_t *pstList, HUINT32 nListNum);
HUINT32		DB_SVC_PutBouquetInfoList (const DbSvc_BouqInfo_t *pstList, HUINT32 nListNum);

/**
  @description	: DB_SVC_DeleteXXXInfo() 각 table에 info(record 하나)를 지우는 API.
  @type			: Inline
  @param 1		: [in]  XXXinfo structure to be deleted
  @param 2		: [out] index of added info (record)
  @DB_SVC_DeleteServiceInfo()의 경우 param1은 index가 아니라 service handle이다.

  @return		: ERR_OK
				: ERR_FAIL - delete 실패
*/

HERROR		DB_SVC_DeleteServiceInfo(Handle_t hSvc);
HERROR		DB_SVC_DeleteAllServiceInfo(void); // by moon
HERROR		DB_SVC_DeleteAllNetworkInfo(void); // by Moon
HERROR		DB_SVC_DeleteTsInfo(HUINT16 usTsIdx);
HERROR		DB_SVC_DeleteAllTsInfo(void); // by Moon
HERROR		DB_SVC_DeleteProviderInfo(HUINT16 usPrvIdx);
HERROR		DB_SVC_DeleteAllProviderInfo(void); // by Moon


/** @description	:  all service list에서 검색 조건에 해당하는 모든 서비스를 지운다 */
void		DB_SVC_DeleteServiceList(DbSvc_Condition_t *pstCond);

/** @description	:  서치 중 cancle 되면 서치 중이던 모든 service를 지운다  */
void		DB_SVC_DeleteAllSearchingService(void);

/** @description	:  search 가 완료되면 search flag을 지워준다 */
void		DB_SVC_RemoveSearchFlag(void);

/** @description	:  해당 section의 temporary alive flag을 세팅한다 */
void		DB_SVC_SetTemporaryAliveFlag(DxSvcSection_e eSection, DxSvcType_e eSvcType);

/** @description	:  temporary alive flag에 해당되는 서비스를 모두 지워준다 */
void		DB_SVC_DeleteTemporaryAliveService(void);

/** @description	: 	original delivery type에 해당하는 모든 서비스를 지운다 */
HERROR		DB_SVC_DeleteAllNetworkService(DxDeliveryType_e eOrgDeliType);

/**
  @description	: DB_SVC_UpdateXXXInfo() 각 table에 info를 업데이트하는 API.
  @type			: Inline
  @param 1		: [in] index of info to be updated.
  @param 2		: [in] XXX info structure to be updated.
  @DB_SVC_UpdateXXXInfo()의 경우 param1은 index가 아니라 service handle이다.

  @return		: ERR_OK
				: ERR_FAIL - 업데이트 실패
*/
HERROR		DB_SVC_UpdateServiceInfo(Handle_t hSvc, DbSvc_Info_t *pstSvcInfo);
HERROR		DB_SVC_UpdateNetworkInfo(HUINT16 usNetIdx, DbSvc_NetInfo_t *pstNetworkInfo);
HERROR		DB_SVC_UpdateTsInfo(HUINT16 usTsIdx, DbSvc_TsInfo_t *pstTsInfo);
HERROR		DB_SVC_UpdateProviderInfo(HUINT16 usPrvIdx, DbSvc_PrvInfo_t *pstPrvInfo);

/* DB_SVC_UpdateServiceInfo에서 field 별로 업데이트 가능하게 구성한 함수 */
HERROR		DB_SVC_UpdateServiceInfoEx(Handle_t hSvc, DbSvc_FieldList_e eSvcField, DbSvc_FieldListExt_e eSvcFieldExt, DbSvc_Info_t *pstSvcInfo);

/**
  @description	: 주어진 service handle로 service uid를 읽어오는 API
  @type			: Inline
*/
#define		DB_SVC_GetServiceUID(hSvc)		((HUINT16)(hSvc & 0xFFFF))

/**
  @description	: 주어진 service handle로 service info를 읽어오는 API
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - Get 실패
*/
HERROR		DB_SVC_GetServiceInfo(
								Handle_t 		 hSvc, 			/**< [in]  가져올 service handle */
								DbSvc_Info_t 	*pstSvcInfo);	/**< [out] svcinfo를 복사할 structure 포인터 */

/**
  @description	: 주어진 Original network Id 로 해당 네트워크의 가장 적은 주파수 정보를 가진 svc의 정보를  읽어오는 API
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - Get 실패
*/
HERROR		DB_SVC_GetSearchResultInfoLowFreq(HUINT16		 usOnId,			/* [in] 검색할 네트워크 */
												HUINT32		*pulFreq,	  		/* [out] 해당 최소 Frequency */
												HUINT32		*pulSnr, 	   		/* [out] Snr 정보 */
												HUINT32		*pulBer,    		/* [out] ber 정보 */
												HINT32		*pnStrength );		/* [out] strength 정보 */


/**
  @description	: GetXXXInfo 함수류. 해당 index의 XXXinfo를 얻어오는 API
  @type			: Inline
  @param1		: [in]  가져올 XXXinfo의 index
				  또는 service handle - DB_SVC_GetTsInfoByServiceHandle(), DB_SVC_GetNetworkInfoByServiceHandle(), DB_SVC_GetPrvInfoByServiceHandle()
				  또는 unique id - DB_SVC_GetTsInfoByUniqueId()
  @param2		: [out] XXXinfo를 복사할 structure 포인터
  @return		: ERR_OK
				: ERR_FAIL - Get 실패
*/
HERROR		DB_SVC_GetTsInfo(HUINT16 usTsIdx, DbSvc_TsInfo_t *pstTsInfo);
HERROR		DB_SVC_GetTsInfoByUniqueId(HUINT16 usUniqueId, DbSvc_TsInfo_t *pstTsInfo);
HERROR		DB_SVC_GetTsInfoByServiceHandle(Handle_t hSvc, DbSvc_TsInfo_t *pstTsInfo);

/**
  @description	: 주어진 조건을 만족하는 모든 TS를 찾는 API
				  ts info를 alloc 해서 주기 때문에 사용후에는 반드시 DB_SVC_FreeTsInfoList()로 free해줘야 한다
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find 실패
*/
void		DB_SVC_InitTsFindCondition(DbSvc_TsCondition_t *pstCond);
HERROR		DB_SVC_FindTsInfoList(DbSvc_TsCondition_t *pstCond, HINT32 *pnCnt, DbSvc_TsInfo_t **ppstInfoList);
void		DB_SVC_FreeTsInfoList(DbSvc_TsInfo_t *pstInfoList);
HINT32		DB_SVC_GetDeliTypeIndex(DxDeliveryType_e eDeliType);


HERROR		DB_SVC_GetNetworkInfo(HUINT16 usNetIdx, DbSvc_NetInfo_t *pstNetworkInfo);
HERROR		DB_SVC_GetNetworkInfoByServiceHandle(Handle_t hSvc, DbSvc_NetInfo_t *pstNetworkInfo);
HERROR		DB_SVC_GetPrvInfo(HUINT16 usPrvIdx, DbSvc_PrvInfo_t *pstPrvInfo);
HERROR		DB_SVC_GetPrvInfoByServiceHandle(Handle_t hSvc, DbSvc_PrvInfo_t *pstPrvInfo);

/**
  @description	: 주어진 조건을 만족하는 service 개수를 얻어오는 API
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - Get 실패
*/
HERROR		DB_SVC_GetServiceCount(
								DbSvc_Group_e	 	 eSvcGroup, 	/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 		*pstAttr,	 	/**< [in]  filtering 조건 2 - 서비스 속성 */
								HINT32 				*pnCnt);		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */

/**
  @description	: 주어진 service handle로 channel number를 읽어오는 API
  @type			: Inline
  @param1		: [in]  service handle
  @param2		: [out] channel number를 담아올 포인터
  @return		: ERR_OK
				: ERR_FAIL - Get 실패
*/
HERROR		DB_SVC_GetServiceNumber(Handle_t hSvc, HINT32 *pnChNum);

HERROR		DB_SVC_GetServiceHandleBySvcIdx(HUINT16 usSvcIdx, Handle_t *phSvc);

/**
  @description	: 주어진 channel number로 service handle을 읽어오는 API
  @type			: Inline
  @param1		: [in]  original delivery type (hybrid 제품의 경우, delivery 간에  중복 channel number가 존재할 수 있기에 delivery type이 필요함)
  @param2		: [in]  service type (같은 delivery라도 service type간 중복 채널 넘버가 존재 할 수 있으므로, 필요하면 service type을 지정해 줄 것)
  @param3		: [in]  channel number
  @param4		: [out] service handle을 담아올 포인터
  @return		: ERR_OK
				: ERR_FAIL - Get 실패
*/
HERROR		DB_SVC_GetServiceHandleByNumber(DxDeliveryType_e eOrgDeliType,
							DxSvcType_e eSvcType, HINT32 nChNum, Handle_t *phSvc);

/**
  @description	: service info로 부터 필요한 필드의 정보를 얻어온다
  @type			: Inline
  @param1		: [in]  service handle
  @param2		: [out] field 값을 얻어올 포인터
  @return		: ERR_OK
				: ERR_FAIL - Get 실패
*/
HERROR		DB_SVC_GetServiceIdFromSvcHandle(Handle_t hSvc, HUINT16 *pusSvcId);
HERROR		DB_SVC_GetUniqueIdFromSvcHandle(Handle_t hSvc, HUINT16 *pusUniqueId);
HERROR		DB_SVC_GetServiceHandle(DxSvcSection_e eSvcSection, HUINT16 usTsIdx, HUINT16 usSvcId, Handle_t *phSvc);
HERROR		DB_SVC_GetServiceHandleByOnIdAndSvcId(DxSvcSection_e eSvcSection, HUINT16 usOnId, HUINT16 usSvcId, Handle_t *phSvc);

/**
  @description	: 같은 channel number를 갖는 모든 서비스 리스트를 얻어온다
  @type			: Inline
  @param4		: [out]
  @return		: ERR_OK
				: ERR_FAIL - Get 실패
*/
HERROR		DB_SVC_FindServiceListByNumber(DxDeliveryType_e 	  eOrgDeliType,		/**< [in]  filtering 조건 1 - delivery type: original delivery type, all 포함 sat/ter/.. 등 OR 가능 */
										DxSvcType_e 			  eSvcType, 		/**< [in]  filtering 조건 2 - service type */
										DbSvc_VisibleType_e 	  eVisibleType, 	/**< [in]  filtering 조건 3 - visible type */
										HINT32 					  nChNum, 			/**< [in]  filtering 조건 4 - 검색할 channel number */
										HUINT32 				 *pulSvcCount, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
										Handle_t 				**pphSvcList);		/**< [out] service handle list를 담아올 포인터. 받드시 DB_SVC_FreeServiceList()로 free 해줘야함 */

/**
  @description	: 조건에 맞는 ts info의 개수를 읽어온다.
  @type			: Inline
  @param1		: [in] 검색 조건, NULL이면 아무런 조건이 없이 전체 ts 개수를 읽어오고, 조건이 없는 경우, 구조체 필드값이 -1이다
  @param2		: [out] ts 개수를 복사할 포인터
  @return		: ERR_OK
				: ERR_FAIL - Get 실패
*/
HERROR		DB_SVC_GetTsCount(DbSvc_TsCondition_t *pstCond, HINT32 *pnCnt);

/**
  @description	: 존재하는 모든 XXXinfo의 개수를 얻어오는 API
  @type			: Inline
  @param1		: [out] network 개수를 복사할 포인터
  @return		: ERR_OK
				: ERR_FAIL - Get 실패
*/
HERROR		DB_SVC_GetNetworkCount(HINT32 *pnCnt);
HERROR		DB_SVC_GetProviderCount(HINT32 *pnCnt);


/**
  @description	: 주어진 조건을 만족하는 service를 찾는 API
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find 실패
*/
HERROR		DB_SVC_FindServiceHandleNearbyLcn(DxDeliveryType_e eOrgDeliType, HUINT16 usLcn, HUINT32 ulPrivateMode, Handle_t* phPrevSvc, Handle_t* phUpperSvc);



/**
  @description	: 주어진 조건을 만족하는 service를 찾는 API
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find 실패
*/
HERROR		DB_SVC_FindServiceHandle(
								DbSvc_Group_e 			 eSvcGroup, 	/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 			*pstAttr, 		/**< [in]  filtering 조건 2 - 서비스 속성 */
								Handle_t 				 hOffsetSvc, 	/**< [in]  filtering 조건 3 - 기준되는 service handle. 만약 next를 찾고자하면 current service에 해당됨. */
								DbSvc_FindDirection_e	 eDirection, 	/**< [in]  filtering 조건 4 - 방향에 관한 조건 */
								Handle_t 				*phSvc);		/**< [out] 해당 조건을 만족하는 service handle을 복사할 포인터 */

HERROR		DB_SVC_FindServiceHandleByCondition (
													const DbSvc_Condition_t		*pstCond,
													Handle_t					*phSvc
												);

/**
  @description	: 주어진 delivery type, triple id 조건을 만족하는 service를 찾는 API
				  ter/cab 만 사용한다. 위성의 경우추가적인 정보가 필요함 (antid)
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find 실패
*/
HERROR		DB_SVC_FindServiceHandleByTripleId(
												DxDeliveryType_e 	 eOrgDeliType, 	/**< [in]  서치 시 할당된 orginal delivery type */
												DbSvc_TripleId_t 	 stTripleId, 	/**< [in]  triple id (onid, tsid, svcid) */
												Handle_t 			*phSvc);		/**< [out] 해당 조건을 만족하는 service handle을 복사할 포인터 */

HERROR		DB_SVC_FindSvcHandleListWhereSearchFlagTrue(HINT32 *pnCnt, Handle_t **pphSvcList);
/**
  @description	: 주어진 조건을 만족하는 모든 service를 찾는 API
				  ulMaxFindNum이 0인 경우, hOffsetSvc과 eCondition는 무시된다.

				  [note] miniocto에서는 다음 세 개의 파라미터를 사용하지 않고, 다음 값으로 설정된 효과를 낸다.
					ulMaxFindNum = 0;
					hOffsetSvc = HANDLE_NULL;
					eDirection = eSvcFindDirection_FowardFromStart;

  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find 실패
*/
 HERROR		DB_SVC_FindServiceList(
								DbSvc_Group_e 			  eSvcGroup, 		/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 			 *pstAttr, 			/**< [in]  filtering 조건 2 - 서비스 속성 */
								HUINT32					  ulMaxFindNum, 	/**< [in]  filtering 조건 3 - 가져올 최대 서비스 개수, 0이면 LIMIT이 없다 */
								Handle_t				  hOffsetSvc, 		/**< [in]  filtering 조건 4 - (Find 시작) 기준되는 hSvc */
								DbSvc_FindDirection_e	  eDirection, 		/**< [in]  filtering 조건 5 - 방향에 관한 조건 */
								HUINT32 				 *pulCount, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 				**pphSvcList);		/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */

/**
  @description	: 주어진 조건을 만족하는 모든 service를 찾는 API
				  DeliveryType + Service_id를 입력으로 주면 해당 serice_id에 맞는 service handle을 알려 주는 함수. 만일 복수개일 경우 복수의service handle과 이것의 갯수
				  사용 후 DB_SVC_FreeServiceList()를 통해 handle list를 free해준다.
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find 실패
*/
HERROR		DB_SVC_FindServiceListBySvcId(
								DxDeliveryType_e 	  eOrgDeliType, 	/**< [in]  filtering 조건 1 - original delivery type */
								HUINT16 			  usSvcId, 			/**< [in]  filtering 조건 2 - service id */
								HINT32 				 *pnSvcCnt, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 			**pphSvcList);		/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */


HERROR		DB_SVC_FindServiceListByNationalSvcId(DxDeliveryType_e		  eOrgDeliType, 		/**< [in]  filtering 조건 1 - original delivery type */
													HUINT16				  usNationalSvcId, 		/**< [in]  filtering 조건 2 - national service id - Canal Ready Only */
													HINT32				 *pnSvcCnt, 			/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
													Handle_t			**pphSvcList);			/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */

/**
  @description	: 주어진 조건을 만족하는 모든 service를 찾는 API
				  DeliveryType + triple_id를 입력으로 주면 해당 triple_id에 맞는 service handle을 알려 주는 함수. 만일 복수개일 경우 복수의service handle과 이것의 갯수
				  사용 후 DB_SVC_FreeServiceList()를 통해 handle list를 free해준다.
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find 실패
*/
HERROR		DB_SVC_FindServiceListByTripleId(
								DxDeliveryType_e 	  eOrgDeliType, 	/**< [in]  filtering 조건 1 - original delivery type */
								DbSvc_TripleId_t 	  stTripleId,	 	/**< [in]  filtering 조건 2 - triple id */
								HINT32 				 *pnSvcCnt, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 			**pphSvcList);		/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */

/**
  @description	: 주어진 조건을 만족하는 모든 service를 찾는 API
  				  사용 후 DB_SVC_FreeServiceInfoList()를 통해 service handle과 service info list를 free해줘야 한다
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find 실패
*/
HERROR		DB_SVC_FindServiceListWithInfo(
								DbSvc_Group_e 		 eSvcGroup, 		/**< [in]  filtering 조건 1 - 서비스 그룹 */
								DbSvc_Attr_t 		 *pstAttr, 			/**< [in]  filtering 조건 2 - 서비스 속성 */
								HUINT32 			 *pulCount, 		/**< [out] 해당 조건을 만족하는 service 개수를 복사할 포인터 */
								Handle_t 			**pphSvcList,		/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */
								DbSvc_Info_t		**ppstSvcInfoList);	/**< [out] 해당 조건을 만족하는 service list를 복사할 포인터 */

/**
  @description	: DB_SVC_FindServiceList()에서 할당된 service list를 free하는 API
  @type			: Inline
  @return		: 없음
*/
void		DB_SVC_FreeServiceList(Handle_t *phSvcList);         /**< [in]  service list */

void		DB_SVC_FreeServiceInfoList(HINT32 *pnLcnList, Handle_t *phSvcList, DbSvc_Info_t *pSvcInfoList);

/**
  @description	: 주어진 조건을 만족하는 모든 TS를 찾는 API
				  ts info를 alloc 해서 주기 때문에 사용후에는 반드시 DB_SVC_FreeTsInfoList()로 free해줘야 한다
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find 실패
*/
void		DB_SVC_InitSvcAttribute (DbSvc_Attr_t *pstAttr);
void		DB_SVC_InitSvcFindCondition(DbSvc_Condition_t *pstCond);
HERROR		DB_SVC_FindSvcInfoList(DbSvc_Condition_t *pstCond, HINT32 *pnCnt, DbSvc_Info_t **ppstInfoList);
HERROR		DB_SVC_FindSvcHandleList(DbSvc_Condition_t *pstCond, HINT32 *pnCnt, Handle_t **pphSvcList);

HINT32		DB_SVC_FindSvcUidList (const DbSvc_Condition_t *pstCond, HINT32 *pnUids, HINT32 n);

/**
  @description	: 주어진 조건을 만족하는 triple id list를 얻어온다
				  사용 후에는 free해야됨
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find 실패
*/
HERROR		DB_SVC_FindTripleIdAndSvcTypeList(DxDeliveryType_e eOrgDeliType, HUINT8 ucAntIdx, HINT32 *pnCnt, DbSvc_TripleId_t **ppstTripleIdList, DxSvcType_e **ppeSvcTypeList, Handle_t **pphSvcList);
void		DB_SVC_FreeTripleIdAndSvcTypeList(DbSvc_TripleId_t *pstTripleIdList, DxSvcType_e *peSvcTypeList);

/**
  @description	: 서비스 핸들이 유효한지 체크하는 API
				  DB_SVC_FindServiceHandle()로도 존재하는 서비스인지 알 수 있지만 사용의 편의성을 위해 만든 API이다.
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - invalid 서비스 핸들
*/
HERROR		DB_SVC_CheckValidServiceHandle(Handle_t hSvc);		/**< [in] 검사할 서비스 핸들 */

/**
  @description	: Audio User set Flag를 reset하는 함수.
  @type 		: Inline
  @return		: 없음
*/
void		DB_SVC_ResetAudioUserFlag(void);

/**
  @description	: Subtitle User set Flag를 reset하는 함수.
  @type			: Inline
  @return		: 없음
*/
void		DB_SVC_ResetSubtitleUserFlag(void);

/**
  @description	: memory db의 field 중 sync되지 않는 필드의 내용을 지우는 API
  @type			: Inline
  @return		: 없음
*/
void		DB_SVC_ResetNoSyncFields(void);

/**
  @description	: memory db의 field의 sync되지 않는 필드중 rating passed event ID필드를 모두 지우는 API
  @type			: Inline
  @return		: 없음
*/
void		DB_SVC_ResetRatingPassedEventId(void);

/**
  @description	: recent channel list에 서비스를 추가하는 API
				  새로 추가된 서비스가 list의 맨 위로 올라간다. 최대 개수 만큼(=20)만 추가되고, 직전과 같은 서비스는 추가되지 않는다
  @type			: Inline
  @return		: 없음
*/
void		DB_SVC_AddRecentService(Handle_t hSvc);

HUINT16		DB_SVC_GetAllUniqueId(void);
HUINT16		DB_SVC_GetTempUniqueId(void);
HUINT16		DB_SVC_GetPlaybackUniqueId(void);

HERROR		DB_SVC_FindProviderIndexByName(HUINT8 *pszName, HUINT16 *pusPrvIdx);
HERROR		DB_SVC_FindProvideInfoList(HINT32 *pnCnt, DbSvc_PrvInfo_t **ppstPrvInfo);
void		DB_SVC_FreeProviderInfoList(DbSvc_PrvInfo_t *pstPrvInfo);

HERROR		DB_SVC_FindNetworkIndexById(DxDeliveryType_e eOrgDeliType, HUINT16 usNetId, HUINT16 *pusNetworkIdx);
HERROR		DB_SVC_FindNetworkIndexByName(HUINT16 usNetId, HUINT8 *pszName, HUINT16 *pusNetworkIdx);
HERROR		DB_SVC_FindNetworkInfoByDeliType(DxDeliveryType_e eOrgDeliType, HINT32 *pulCnt, DbSvc_NetInfo_t **ppstNetworkInfo);
void		DB_SVC_FreeNetInfoList(DbSvc_NetInfo_t *pstNetInfoList);
HERROR		DB_SVC_GetDefaultAuthority(Handle_t hSvc, HUINT8 *pszDefAuth);

/**
  @description	: service / video type에 따라 모든 서비스의 CAS 종류를 OR해서 얻어오는 API
  					(channel list filter에서 주로 사용하는 기능)
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - get 실패
*/
HERROR		DB_SVC_GetAllCasType(DxSvcType_e eSvcType, DxVideoType_e eVideoType, DxCasType_b *pbwCasType);

/**
  @description	: LCN영역 내의 조건에 부합하는 서비스에 대해 re-numbering을 한다.
  				  pre chlist 용도로 사용하려고 하지만, delivery type등의 option도 두어 다른 용도로도 활용 가능하게 한다.
	  				 * 조건1 - delivery type
	  				 * 조건2 - service section (ex. pre-chlist)
	  				 * 조건3 - service type
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - re-numbering 실패
*/
HERROR		DB_SVC_RenumberRankingToLcn(DxDeliveryType_e eOrgDeliType, DxSvcSection_e eSvcSection, DxSvcType_e eSvcType, HINT32 nStartChNum, HINT32 nEndChNum);

/**
  @description	: 기준 되는 base channel number에 따라, non-LCN채널들을 저장된 순서대로 re-numbering을 한다.
  				  (저장순서, 즉, svcIdx가 order number가 됨)
	  				 * 조건1 - delivery type
	  				 * 조건2 - service type
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - re-numbering 실패
*/
HERROR		DB_SVC_RenumberSavingOrderToChNum(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HINT32 nBaseStartChNum);


/**
  @description	: non-LCN 영역의 채널을 LCN에 따라 re-numbering 한다. LCN이 ranking의 역할을 하는 것임
  				  (LCN값이 order number가 됨)
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - 실패
*/
HERROR		DB_SVC_RenumberLcnOrderToChNum(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HINT32 nBaseChNum);

/**
  @description	: LCN 사양 중 non-LCN 영역에 대한 spec이 없는 경우, 이 함수를 사용하여 LCN 마지막에 이어서 numbering을 하게 한다.
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - 실패
*/
HERROR		DB_SVC_MakeNonLcnNumber(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType);

/**
  @description	: 이 함수는 non-LCN 사양이나, LCN이 없는 경우 넘버를 부여할 때 활용될 수 있다. 조건에 맞는 영역의 service 개수를 돌려준다.
  @type			: Inline
*/
HERROR		DB_SVC_GetLcnOrderNumber(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HUINT16 *pusLcn);
HERROR		DB_SVC_GetLcnOrderNumberFromTVsAndRadios(DxDeliveryType_e eOrgDeliType, HUINT16 *pusLcn);

HERROR     DB_SVC_ReplaceFavoriteServiceHandle(DbSvc_FavGroup_e eFavGroup, Handle_t hOrgFavSvc, Handle_t hNewFavSvc);

#define _____BOUQUET_______________________________
// TODO: add comments
HERROR		DB_SVC_AddBouquetInfo(DbSvc_BouqInfo_t *pBouquetInfo, HUINT16 *pusBouqIdx);
HERROR		DB_SVC_DeleteBouquetInfo(HUINT16 usBouqIdx);
HERROR		DB_SVC_DeleteAllBouquetInfo(void);
HERROR		DB_SVC_UpdateBouquetInfo(HUINT16 usBouqIdx, DbSvc_BouqInfo_t *pBouquetInfo);
HERROR		DB_SVC_GetBouquetCount(HINT32 *pnCnt);
HERROR		DB_SVC_GetBouquetInfo(HUINT16 usBouqIdx, DbSvc_BouqInfo_t *pBouquetInfo);
HERROR		DB_SVC_FindBouquetIndexById(HUINT16 usBouqId, HUINT16 *pusBouqIdx);
HERROR		DB_SVC_FindBouquetIndexByName(HUINT8 *pszName, HUINT16 *pusBouqIdx);
HERROR		DB_SVC_FindBouquetIndexList(HINT32 *pnBouqNum, HUINT16 **ppusBouqIdxList);
void		DB_SVC_FreeBouquetIndexList(HUINT16 *pusBouqIdxList);
HERROR		DB_SVC_AddBouquetIndexToSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT16 usBqIdx);
HERROR		DB_SVC_CountBouquetInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 *pulBqNum);
HERROR		DB_SVC_GetBouquetIndexInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 ulIndex, HUINT16 *pusBqIdx);
HERROR		DB_SVC_GetBouquetIndexArrayInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 ulBqArrayMaxNum, HUINT32 *pulBqIdxNum, HUINT16 *pusBqIdxArray);
HERROR		DB_SVC_ClearBouquetInfoInSvcInfo (DbSvc_Info_t *pstSvcInfo);


#define _____GENRE_______________________________
HERROR		DB_SVC_AddGroupInfo (DbSvc_GroupInfo_t *pstGroupInfo, HUINT16 *pusGroupIdx);
HERROR		DB_SVC_DeleteGroupInfo (HUINT16 usGroupIdx);
HERROR		DB_SVC_DeleteAllGroupInfo(void);
HERROR		DB_SVC_UpdateGroupInfo (HUINT16 usGroupIdx, DbSvc_GroupInfo_t *pstGroupInfo);
HERROR		DB_SVC_CountGroup (HINT32 *pnCnt);
HERROR		DB_SVC_GetGroupInfo (HUINT16 usGroupIdx, DbSvc_GroupInfo_t *pstGroupInfo);
HERROR		DB_SVC_GetGroupInfoByGroupId (HUINT32 ulGroupId, DbSvc_GroupInfo_t *pstGroupInfo);
HERROR		DB_SVC_FindGroupIndexByGroupId (HUINT32 ulGroupId, HUINT16 *pusGroupIdx);
HERROR		DB_SVC_FindGroupIndexByName (HUINT8 *pszName, HUINT16 *pusGroupIdx);
HERROR		DB_SVC_GetAllGroupIndexList (HINT32 *pnGroupNum, HUINT16 **ppusGroupIdxList);
HERROR		DB_SVC_GetAllGroupInfoList (HINT32 *pnGroupNum, DbSvc_GroupInfo_t **pstGroupInfoList);
void		DB_SVC_FreeGroupIndexList (HUINT16 *pusGroupIdxList);
void 		DB_SVC_FreeGroupInfoList (DbSvc_GroupInfo_t *pstGroupInfoList);
HERROR		DB_SVC_AddGroupIndexToSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT16 usGroupIdx);
HERROR		DB_SVC_CountGroupInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 *pulGroupNum);
HERROR		DB_SVC_GetGroupIndexInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 ulIndex, HUINT16 *pusGroupIdx);
HERROR		DB_SVC_GetGroupIndexArrayInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT32 ulGroupArrayMaxNum, HUINT32 *pulGroupIdxNum, HUINT16 *pusGroupIdxArray);
HERROR		DB_SVC_DeleteGroupIndexInSvcInfo (DbSvc_Info_t *pstSvcInfo, HUINT16 usGroupIdx);
HERROR		DB_SVC_ClearGroupInSvcInfo (DbSvc_Info_t *pstSvcInfo);
HUINT32		DB_SVC_GetSmSem(void);
void		DB_SVC_DumpAllSvcInfo (const HCHAR *pszFileName);

DbSvc_TsInfo_t		*DB_SVC_GetAllTsInfo (HINT32 *pnCount);
DbSvc_NetInfo_t		*DB_SVC_GetAllNetInfo (HINT32 *pnCount);
DbSvc_PrvInfo_t		*DB_SVC_GetAllPrvInfo (HINT32 *pnCount);
DbSvc_Info_t		*DB_SVC_GetAllSvcInfo (HINT32 *pnCount);
DbSvc_BouqInfo_t	*DB_SVC_GetAllBouqInfo (HINT32 *pnCount);

HERROR		DB_SVC_PutAntInfo (HUINT16 usAntIdx, const DbSvc_AntInfo_t *pstAntInfo);


#define _____ANTENNA_______________________________

/**
  @remark   설정DB에 antenna info를 추가하는 API
  @remark	type : Inline

  @return	ERR_OK : 완료 \n
 			ERR_FAIL : 실패
*/
HERROR	DB_SVC_AddAntInfo (
					DbSvc_AntInfo_t 	*pstAntInfo, 	/**< 추가할 antenna info */
					HUINT16 			*pusAntIdx		/**< db에 추가 후 자동 설정된 index */
					);
/**
  @remark   설정DB에 주어진 antenna index에 해당하는 antenna info를 지우는 API
  @remark	type : Inline

  @return	ERR_OK : 완료 \n
 			ERR_FAIL : 실패
*/
HERROR	DB_SVC_DeleteAntInfo (
					HUINT16 	usAntIdx		/**< 지우고자 하는 antenna index */
					);
/**
  @remark   설정DB에 모든 antenna info를 지우는 API
  @remark	type : Inline

  @return	ERR_OK : 완료 \n
 			ERR_FAIL : 실패
*/
HERROR	DB_SVC_DeleteAllAntInfo (void);
/**
  @remark   설정DB에 주어진 antenna index에 해당하는 antenna info를 업데이트 하는 API
  @remark	type : Inline

  @return	ERR_OK : 완료 \n
 			ERR_FAIL : 실패
*/
HERROR	DB_SVC_UpdateAntInfo (
					HUINT16 			 usAntIdx,		/**< 업데이트 할 antenna index */
					DbSvc_AntInfo_t 	*pstAntInfo 	/**< 업데이트 내용 */
					);
/**
  @remark   설정DB에 주어진 antenna index에 해당하는 antenna info를 복사해오는 API
  @remark	type : Inline

  @return	ERR_OK : 완료 \n
 			ERR_FAIL : 실패
*/
HERROR	DB_SVC_GetAntInfo (
					HUINT16 			 usAntIdx,		/**< 읽고자 하는 antenna index */
					DbSvc_AntInfo_t 	*pstAntInfo 	/**< 담아올 구조체 */
					);
/**
  @remark   저장된 antenna 개수를 얻어오는 API
  @remark	type : Inline

  @return	ERR_OK : 완료 \n
 			ERR_FAIL : 실패
*/
HERROR	DB_SVC_GetAntCount (HINT32 *pnCnt);		/**< out: 담아올 구조체 */

/**
  @remark   저장된 모든 안테나의 antIdx와 개수를 얻어오는 API
  @remark	type : Inline

  @return	ERR_OK : find 성공 \n
 			ERR_FAIL : 실패 또는 안테나 정보 없음
*/
HERROR	DB_SVC_FindAllAntIdxList (HINT32 *pnCnt, HINT32 **ppnAntIdxList);
HBOOL	DB_SVC_IsAntChanged (void);
void	DB_SVC_CommitAllAntInfo (void);
void	DB_SVC_CommitAllSvcInfo (void);
void	DB_SVC_CommitAllTsInfo (void);
void	DB_SVC_CommitAllProviderInfo (void);
void	DB_SVC_CommitAllNetInfo (void);
void	DB_SVC_CommitAllGroupInfo (void);
void	DB_SVC_CommitAllBouquetInfo (void);

/**
  @remark   DB_SVC_FindAllAntIdxList() 함수를 통하여 가져온 antenna index list를 free하는 API
  @remark	type : Inline

  @return	없음
*/
void	DB_SVC_FreeAntList (HINT32 *pnAntIdxList);
HERROR	DB_SVC_ReloadAllAntInfo (void);
HERROR	DB_SVC_AntennaSync(void);


/**
  @remark   Default Authority관련 함수
  @remark	type :

  @return	없음
*/
HERROR	DB_SVC_GetServiceDefaultAuthority(const DbSvc_Info_t *pstSvcInfo, HUINT8 **ppucDefaultAuthority);
HERROR	DB_SVC_GetNetworkDefaultAuthority(const DbSvc_NetInfo_t *pstNetInfo, HUINT8 **ppucDefaultAuthority);
HERROR	DB_SVC_GetTsDefaultAuthority(const DbSvc_TsInfo_t *pstTsInfo, HUINT8 **ppucDefaultAuthority);
HERROR	DB_SVC_SetServiceDefaultAuthority(DbSvc_Info_t *pstSvcInfo, const HUINT8 *pucDefaultAuthority);
HERROR	DB_SVC_SetNetworkDefaultAuthority(DbSvc_NetInfo_t *pstNetInfo, const HUINT8 *pucDefaultAuthority);
HERROR	DB_SVC_SetTsDefaultAuthority(DbSvc_TsInfo_t *pstTsInfo, const HUINT8 *pucDefaultAuthority);
HERROR	DB_SVC_ResetServiceDefaultAuthority(DbSvc_Info_t *pstSvcInfo);
HERROR	DB_SVC_ResetNetworkDefaultAuthority(DbSvc_NetInfo_t *pstNetworkInfo);
HERROR	DB_SVC_ResetTsDefaultAuthority(DbSvc_TsInfo_t *pstTsInfo);
HERROR 	DB_SVC_GetServiceFTAContentMgr(const DbSvc_Info_t *pstSvcInfo, HUINT32 *pulFTAContentMgr);
HERROR 	DB_SVC_GetNetworkFTAContentMgr(const DbSvc_NetInfo_t *pstNetInfo, HUINT32 *pulFTAContentMgr);
HERROR 	DB_SVC_GetTsFTAContentMgr(const DbSvc_TsInfo_t *pstTsInfo, HUINT32 *pulFTAContentMgr);
HERROR 	DB_SVC_SetServiceFTAContentMgr(DbSvc_Info_t *pstSvcInfo, const HUINT32 ulFTAContentMgr);
HERROR 	DB_SVC_SetNetworkFTAContentMgr(DbSvc_NetInfo_t *pstNetInfo, const HUINT32 ulFTAContentMgr);
HERROR 	DB_SVC_SetTsFTAContentMgr(DbSvc_TsInfo_t *pstTsInfo, const HUINT32 ulFTAContentMgr);

HERROR DB_SVC_GetServiceGuidanceType(const DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceType);
HERROR DB_SVC_GetServiceGuidanceMode(const DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceMode);
HERROR DB_SVC_GetServiceGuidanceStr(const DbSvc_Info_t *pstSvcInfo, HUINT8 **ppucGuidanceStr);
HERROR DB_SVC_SetServiceGuidanceType(DbSvc_Info_t *pstSvcInfo, HUINT8 ucGuidanceType);
HERROR DB_SVC_SetServiceGuidanceMode(DbSvc_Info_t *pstSvcInfo, HUINT8 ucGuidanceMode);
HERROR DB_SVC_SetServiceGuidanceStr(DbSvc_Info_t *pstSvcInfo, HUINT8 *pucGuidanceStr);

HERROR DB_SVC_GetServiceVirtualSvcId(const DbSvc_Info_t *pstSvcInfo, HUINT16 *pusVirtualSvcId);
HERROR DB_SVC_GetServiceNewSvc(const DbSvc_Info_t *pstSvcInfo, HBOOL *pbNewSvc);
HERROR DB_SVC_SetServiceVirtualSvcId(DbSvc_Info_t *pstSvcInfo, HUINT16 usVirtualSvcId);
HERROR DB_SVC_SetServiceNewSvc(DbSvc_Info_t *pstSvcInfo, HBOOL bNewSvc);
HERROR DB_SVC_GetNetworkUriPrefix(const DbSvc_NetInfo_t *pstNetInfo, HUINT8 **ppucUriPrefix);
HERROR DB_SVC_SetNetworkUriPrefix(DbSvc_NetInfo_t *pstNetInfo, const HUINT8 *pucUriPrefix);

HINT32	DB_SVC_UtilCompareDbTuningParam(HUINT32 ulDeliType, DbSvc_TuningInfo_t *pstTune1, DbSvc_TuningInfo_t *pstTune2);
HINT32	DB_SVC_UtilCompareChTuningParam(DxTuneParam_t *pstTune1, DxTuneParam_t *pstTune2);

HERROR	DB_SVC_GetMaxLcnInLcnServices (DxDeliveryType_e	 eOrgDeliType,
										DxSvcType_e	 eSvcType,
										HINT32			*pnLcn);

HERROR     DB_SVC_GetMaxLcnInLcnInInterval (DxDeliveryType_e	 eOrgDeliType,
											DxSvcType_e	 eSvcType,
											HINT32 nStartNum,
											HINT32 nEndNum,
											HINT32			*pnLcn);

#endif /* !__DB_SVC_H__ */

