/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  db_svc.h
	@brief	  service db

	Description: service/ts/network/provider/bouquet �� PSI/SI�� �����Ͽ� ��������� ��� service data�� �̰��� ���Ͽ� ����/����/������Ʈ �ȴ�. \��
	             �׸��� �ʿ��� ������ �� �� �ִ� API�����Ѵ�.
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

#include <octo_common.h> /** @brief Handle_t�� ����ϱ� ���� */

#include <db_common.h>
#include <db_param.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*
	== defconfig�� ���ǵǾ� �ִ� ���� ==
	1. max service number : CONFIG_MAX_SERVICE_NUM�� ����ϸ�, ���� ���� 5000������ �����Ѵ�
	2. max service name len :
			#define CONFIG_DB_SVC_MAX_NAME_BYTES 34
			���� ������ satellite service�� ���̸� �����Ѵ�.
	3. �Ʒ� ���鵵 defconfig�� �߰��ص� �ǳ� ������ �ʿ伺 ������
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
		all �׷� ���� �ߺ� ä���� ������ �� �ִµ�, �� �� ���еǴ� ������ section�̴�.
		0���� 15������ ���� ���´�.
		service handle�� ���� 4��Ʈ�� ����ϴ� ������, �Ʒ� ������ �� �ܿ��� �߰� �Ұ�.
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
	/* �� �̻� �߰��� �� ���� !!!!!!!!! */
} DbSvc_Section_e, DbSvc_Section_b;
#endif

/** @brief DB���ο� Table������ ���еǴ� ���� �׷� - All, Fav1/2/3/4/5, Recent, Most */
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
/** @brief [service db �Ӽ� - SvcType] �޸ƽ��� ������ service type���� �⺻������ SDT�� service type�� ���ǵ� ���� �� �����ϴ� type�� ���� ���Եȴ�. */
typedef enum
{ /* [WARN] forbid to change a db-saving enum value, [WARN] ���� ifdef�� �߰� ���� ����. ���̸� �ٷ� ���� ���� */
	eSvcType_All			= 0,
	eSvcType_Tv				= 0x0001,
	eSvcType_Radio			= 0x0002,
	eSvcType_Data			= 0x0004,
	eSvcType_Subfeed		= 0x0008,	// Multifeed Service �� Subfeed ��
	eSvcType_Replacement	= 0x0010,	// Nordic ����� Replacement Service
	eSvcType_Hidden   		= 0x0020,
	eSvcType_MHPTV			= 0x0040,
	eSvcType_Engineering	= 0x0080,
	eSvcType_Rfvod			= 0x0100,	// for rfvod service
} DbSvc_Type_e, DbSvc_Type_b;
#endif
#if 0
/** @brief DbSvc_Type_e�� �ַ� service db���� �˻� �������� ��������,
			service type ������ loop ���� ��츦 ���� define���� ����� �� ���̶�, DbSvc_Type_e�� type ������ sync ����� ��.
			���� ���� eSvcType_All�� ������ type ������ */
#define MAX_SVCTYPE_NUM		8
#endif

#if 0
/** @brief [service db �Ӽ� - VideoType] */
typedef enum
{ /* [WARN] forbid to change a db-saving enum value, [WARN] ���� ifdef�� �߰� ���� ����. ���̸� �ٷ� ���� ���� */
	eVideoType_All		= 0,
	eVideoType_Sd		= 1,
	eVideoType_Hd		= 2,
} DbSvc_VideoType_e;

/** @brief [service db �Ӽ� - CasType] */
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
	eSortType_NUMBER 					= 0,	/**< eOrgDeliType�� �ƴ�, ucChListPriority�� ordering�� ��. priority�� configuration�Ǿ���Ѵ� */
	eSortType_ALPHABET					= 1,	/**< szSvcName �켱�̰�, �̸� ������, number ������ */
	eSortType_NUMBER_AND_INDEX			= 2,	/**< LCN�� �����Ǿ� ������ LCN �켱, �ƴ϶�� ����� ������� */
	eSortType_NUMBER_AND_SVCTYPE		= 3,	/**< { ��� TV ä�� } -> { ��� Radio ä�� } ���� */
	eSortType_NUMBER_AND_SVCTYPE_ONLY	= 4,	/**< eSortType_NUMBER_AND_SVCTYPE���� ucChListPriority ������ ���� ���, ucChListPriority�� delivery ������ �����ϴ� ����̴� */
	eSortType_FAV_INDEX_AND_SVCTYPE		= 5,		/**< fav group����, {Tv ��ü} --> {Radio ��ü} �� ���̱� ���� ��� */
	eSortType_INDEX
} DbSvc_SortType_e;

typedef enum
{
	eLcnVisibleSvc_Only 	= 0,
	eLcnVisibleSvc_FindAll	= 1
} DbSvc_VisibleType_e;


typedef enum
{
	eChAlphabetGroup_AlphabetAll	=	0,		/** ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetA		=	0x41,	/** A�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetB		=	0x42,	/** B�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetC		=	0x43,	/** C�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetD		=	0x44,	/** D�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetE		=	0x45,	/** E�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetF		=	0x46,	/** F�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetG		=	0x47,	/** G�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetH		=	0x48,	/** H�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetI		=	0x49,	/** I�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetJ		=	0x4A,	/** J�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetK		=	0x4B,	/** K�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetL		=	0x4C,	/** L�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetM		=	0x4D,	/** M�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetN		=	0x4E,	/** N�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetO		=	0x4F,	/** O�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetP		=	0x50,	/** P�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetQ		=	0x51,	/** Q�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetR		=	0x52,	/** R�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetS		=	0x53,	/** S�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetT		=	0x54,	/** T�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetU		=	0x55,	/** U�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetV		=	0x56,	/** V�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetW		=	0x57,	/** W�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetX		=	0x58,	/** X�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetY		=	0x59,	/** Y�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetZ		=	0x5A,	/** Z�� �����ϴ� ��� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetDigit	=	0x5B,	/** 0 ~ 9 �� �����ϴ� ä���� �����ش�. */
	eChAlphabetGroup_AlphabetEtc	=	0x5C	/** A-Z, 0-9 �̿��� ä���� �����ش�.  */
} DbSvc_AlphabetGroupType_e;


/*
	[service user flag] ���̵����

	1. user flag�� �ǹ�
		- service table������ boolean type �ʵ� ������ ���� �������� user flag�� �д�. ���� ��Ʈ ���꿡���� 0 �Ǵ� 1�� �� ���̴�.

	2. ���� ����
		- 32���� ���ڶ� ���, SVC_UserFlag2_t�� �����ؾ� �Ѵ�.

	3. ����
		- eSvcUserFlag_8�� ����:
			stSvcInfo.eUserFlag1 |= eSvcUserFlag_8;
		- eSvcUserFlag_8�� ���� �Ǿ����� Ȯ���Ϸ���:
			if(0 != (stSvcInfo.eUserFlag1 & eSvcUserFlag_8))
			{
				// eSvcUserFlag_8�� ���� �Ǿ���
			}
			else
			{
				// eSvcUserFlag_8�� ���� ���� �ʾ���
			}

	4. user flag�� ���� - "all or incluing this condition"�� "exclusive condition" (or complementary set)�� ������
		- all or incluing this condition: ������ �ɸ��� ������, flag�� true/false�� ���� ��� ã�´�. ������ �ɸ��� �ɸ� �� ã�´�.
		- exclusive condition: ������ true�� true�� �� ã��, false�� false�� �� ã�´�.
			-> �� ���, ���� enum ���ǿ�, �̰��� ����� �ְ�, �ʿ信 ����, local_dbsvc_MakeSvcUserFlag1ConditionStr()�� ������ �߰����ش�.
*/
typedef enum
{
	eSvcUserFlag_All					= 0,
	eSvcUserFlag_01_FsatUserCustom		= 0x00000001,	/**< all or incluing this condition	, CONFIG_OP_FREESAT */
	eSvcUserFlag_02_Selectable			= 0x00000002,	/**< all or incluing this condition	, CONFIG_OP_FREESAT or CONFIG_OP_UK_DTT */
	eSvcUserFlag_03_SvcDeletedByUser	= 0x00000004,	/**< exclusive condition			, Italy DTT���� ��� ��(CONFIG_DB_SVC_TRASH)�̳� ���� �䱸 ���� ������ ��� ���� */
	eSvcUserFlag_04_Reserved			= 0x00000008,
	eSvcUserFlag_05_Reserved			= 0x00000010,
	eSvcUserFlag_06_Reserved			= 0x00000020,
	eSvcUserFlag_07_Reserved			= 0x00000040,
	eSvcUserFlag_08_Reserved			= 0x00000080,
	eSvcUserFlag_09_AdultSvc			= 0x00000100,	/**< exclusive condition			, CONFIG_OP_FREESAT */
	eSvcUserFlag_10_Reserved			= 0x00000200,
	eSvcUserFlag_11_FreeCiMode			= 0x00000400,	/**< exclusive condition			, CI+ Model only */
	eSvcUserFlag_12_Reserved			= 0x00000800,
	eSvcUserFlag_13_VolatileSvc			= 0x00001000,	/**< exclusive condition    	    , �ֹ߼� Service�� �������� �ʴ´�. Tune Action���� ��� */
	eSvcUserFlag_14_Reserved			= 0x00002000,
	eSvcUserFlag_15_Reserved			= 0x00004000,
	eSvcUserFlag_16_Reserved			= 0x00008000,
	eSvcUserFlag_17_Reserved			= 0x00010000,
	eSvcUserFlag_18_Reserved			= 0x00020000,
	eSvcUserFlag_19_Reserved			= 0x00040000,
	eSvcUserFlag_20_ParentalControl		= 0x00080000,	/**< all condistion					, Parental control�� �־��� ���񽺸� marking. �� mark�� ������ wtv�� parental check�� skip�Ѵ�. */
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
	eSvcUserFlag_31_SvcConflictLcn		= 0x40000000,	/**< exclusive condition			, Italy DTT���� ��� �� automatic service numbering �� conflict �߻� service �� ����*/
	eSvcUserFlag_32_NewService			= 0x80000000	/**< all or including this condition , ���ο� Channel�� �߰� �Ǿ����� �ش� �÷��׸� �����ش�. OCTO 2.0���� �ش� �÷��״� OCTO�� APPL�� �ټ��� �ְ� APPL�� �ش� �÷��׸� ���� �൵ OCTO�� �����Ѵ�.*/
} DbSvc_UserFlag1_e, DbSvc_UserFlag1_b;

/** @brief [service db �Ӽ�] */
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

/** @brief channel db �˻� ���� */
typedef enum
{
	eSvcFindDirection_Current = 0,
	eSvcFindDirection_Position = 1,

	/* find ���� ���Ǵ� ��� */
	eSvcFindDirection_Prev,
	eSvcFindDirection_Next,
	eSvcFindDirection_First,
	eSvcFindDirection_Last,
	eSvcFindDirection_PrevBySortNum2,
	eSvcFindDirection_NextBySortNum2,

	/* find list���� ���Ǵ� ��� - chlist���� ����ϴ� �˻��� �ǹ̸� ��Ȯ�� �ϰ���, ��Ī�� ������ �Ͽ��� */
	eSvcFindDirection_SearchedOrder,
	eSvcFindDirection_Foward,
	eSvcFindDirection_Backward,
	eSvcFindDirection_FowardFromStart,
	eSvcFindDirection_BackwardFromLast,

	/* Find list���� ����ϸ�, lcn�ܿ� Ư�� ���� sorting�� ������. */
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
	@brief fav service�� ��� group�� �ҼӵǾ� �ִ� ���� ��Ÿ���� ��.
			service info add�ϴ� �Ͱ��� API�� ���еǾ� �����Ƿ�, fav group�� �ǹ̸� ��Ȯ�� �ϱ� ����, �Ʒ��� ���� type�� �ξ���
			fav group ���� OR�� ����
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

/** �ʵ庰�� update ���θ� ���������ϴ� */
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

	/* ������� �ʴ� ��(������, ����� �̷��� ���� ���� �ִ�)�� �Ʒ�ó�� reserved�� ��Ƶ� */
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

/* SVC_FieldList_t�� Ȯ�� �������� SVC_FieldList_t�� 32�� �ʵ带 ��� ����� ���, ���⿡ �߰��� ä�������� */
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
	HUINT16 			usSesVirtualSvcId;		/**< SES LCN���� �����ϴ� Virtual Service ID�� �ǹ��Ѵ�. */
	HBOOL				bNewSvc;				/**< SES LCN���� New Service�� �ö���� argument. */
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

/** @brief service info structure, �ݵ�� usTsIdx�� usPrvIdx�� �����Ǿ�� �Ѵ�. */
#if defined (CONFIG_OP_SES)
typedef struct
{
	DxService_t			stDxInfo;

	// �ӽ� �߰�: CAS ����ڿ� �ǳ� �� DxService_t�� ���� ������ �ٸ��� ó���� ������ �����ϵ��� �Ѵ�.
	// ToDo: Start
#if defined(DbSvc_SUPPORT_DXINFO)
#else
		HUINT16 			usSvcIdx;
		/////////////////////////////////////////
		//	���� �Ҽ� ���� ����. ��ġ �� ������
		HUINT16 			usTsIdx;		   /**< ��ġ�� �Ҵ��. ���� �Ұ�. service�� ���� TBL_TS.tsIdx -> ������ unique_id �� */
		HUINT16 			usPrvIdx;		   /**< ��ġ�� �Ҵ��. ���� �Ұ�. service�� ���� TBL_PRV.prvIdx */

		HUINT16 			usSvcId;		   /**< ��ġ�� �Ҵ��. ���� �Ұ�. service id */
		HUINT16 			usTsId; 		   /**< ��ġ�� �Ҵ��. ���� �Ұ�. transport stream id */
		HUINT16 			usOnId; 		   /**< ��ġ�� �Ҵ��. ���� �Ұ�. original network id */
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		HUINT8				szSvcName[/*CONFIG_DB_SVC_MAX_NAME_BYTES*/DbSvc_GetNameLength()];	/**< service name. �� ���� SDT���� �����ǰų� ����ڰ� ������ �� �ִ� */
		HUINT16 			usLcn;							/**< retail�̶� humax rule�� ���� �⺻ LCN�� �����. search index�� ������ lcn�� ����� */
		HBOOL				bLcnFlag;						/**< LCN descriptor�� ���� LCN��翡 ���� channel number�� �Ҵ�Ǿ��ٸ�, TRUE�� ���� */
		HBOOL				bVisibleSvcFlag;				/**< LCD syntax�� �����ϴ� ���, �� ���뿡 ���� TRUE/FALSE ���� �Ҵ� �����ϰ�, LCD syntax�� ���ٸ� �׻� TRUE */

		/////////////////////////////////////////
		//	PID & CODEC
		HUINT16 			usPmtPid;				/**< pmt pid, pat�� ���ǵ� */
		HUINT16 			usPcrPid;				/**< pcr pid from pmt */
		HUINT16 			usTtxPid;				/**< teletext pid from pmt */

		HUINT16 			usVideoPid; 			/**< video pid from pmt */
		DxVideoCodec_e		eVideoCodec;			/**< �ش� es�� video codec, db file�� 1 byte�� ����� */

		HUINT16 			usAudioPid; 			/**< audio pid from pmt */
		DxAudioCodec_e		eAudioCodec;			/**< �ش� es�� audio codec, db file�� 1 byte�� ����� */

		HUINT16 			usAudioAuxPid;			/**< for audio descriptor or dolby */
		DxAudioCodec_e		eAudioAuxCodec; 		/**< �ش� es�� audio codec, db file�� 1 byte�� ����� */

		HUINT16 			usDolbyPid; 			/**< dolby audio pid from pmt */
		DxAudioCodec_e		eDolbyCodec;			// TODO: ���� �߰�
		HBOOL				bDolbyFlag; 			/**< main audio�� dolby�� ��� TRUE�� ���� */
													/*----------------------------------------------------------------------
														bDolbyFlag�� TRUE�� �Ǵ� ����,
														(1) sound track â���� dolby audio�� �����ϰų�,
														(2) menu�� dolby priority�� �����ǰ�, dolby audio�� ������ ����
															-> MW SI���� �Ǵ��Ͽ� ������
													-----------------------------------------------------------------------*/

		/////////////////////////////////////////
		//	������ service �Ӽ����̴�.
		//---------------------------------------
		/*
			~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			[eSvcType vs. eOrgSvcType]�� ���� ����, 2009-05-15
			~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				- eOrgSvcType�� ���߿� �߰��� �����̴�.
				- SDT�� ������ service_descriptor�� ������� Tv/Radio/Data�� ���� service_type�� �����ǳ�, �ʿ信 ���� Data service�� Tv�� ������ �ʿ䰡 �ִ�.
				- �ַ� chlist���� DB�˻��� ���� Data�� Tv�� �з��ϴµ�, Tv ä�� �˻� �������� �ִ� ���, Data���� ���� �˻��ϼ� �ִ� ���̴�.
				- eSvcType�� TV���� Data������ service time�� SI��⿡�� �����Ǹ�,
				- service_descriptor�� ������ eOrgSvcType�� �����ϰ�, eSvcType�� Humax �ó������� �°� ������ �Ѵ�.

			����, DxSvcType_e�� service type���� OR�ؼ� �˻� ������ �� ���� ������, Tv+Data�� �������� ó���ϴ� ���, ui application�� ���⵵�� �����ϰ� �ȴ�.
			�̿� ���� Ÿ��������, DB�� eOrgSvcType�� �� ���̴�.
		*/
		DxSvcType_e 			eSvcType;
		DxSvcType_e 			eOrgSvcType;
		DxVideoType_e			eVideoType; 			/**< all/sd/hd */
		DxCasType_e 			eCasType;				/**< ca system id�� ���� �̸� ���ǵ� ������ OR �ؼ� �������� CAS�� ���ÿ� ������ �� �ִ�. all/fta/nagra/irdeto/nds/ ... */
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
		HUINT8				ucNumOfEntries; 						/** number_of_entries, match_brand_flag�� false��� �� ���� 0 **/
		HUINT16 			ausCamBrandId[MAX_CAM_BRAND_NUM];		/** cicam_brand_identifier **/
		HBOOL				bSelectableSvcFlag;
		HUINT8				szOperatorName[MAX_CAM_OPERATOR_NAME_LEN];
		HUINT8				aucCamId[CICAM_ID_LEN];
	#endif
#endif

		/*
			~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			[eOrgDeliType, original delivery type]�� ���� ����
			~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				- octo���� ó�� ����, 080625
				- ���� �������� ����/������/ä��/�Ƴ��α� ä�� ������ ���񽺰� ���� ä���� delivery �Ӽ��� ��Ÿ����.
				- ä�θ���Ʈ�� ������ �� Ȱ���.
				- tuning parameter�ʹ� ������ �ǹ̷� ���ȴ�.
				- ���� ���̺� ��Ʈ���� ������ ä���� ��� �ִٸ� tuning param�� delievery system�� ���̺��� �ǰ�,
				- ������ original delievery system�� �����İ� �Ǵ� ���̴�.
		*/
		DxDeliveryType_e		eOrgDeliType;
		SVC_SatType_t			eSatType;				/**< �������밪, antenna info�� satellite type (note: antenna info�� setup.db�� �ֱ� ������, satellite filter�� �˻� �� �ʹ� ���� �ɷȾ���. 080928) */
		HUINT8					ucAntIdx;				/**< �������밪, delivery type�� satellite�� ���, unique service�� id�� antenna index�� �ʿ�. ts info�� ant idx�� ���� ���� ���´� */
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		/////////////////////////////////////////
		//	user�� ���� ��������
		//---------------------------------------
		HBOOL					bModifiedName;			/**< service ���� ����ڿ� ���� ������ ��� TRUE */
		HBOOL					bLocked;				/**< lock by user */
		HUINT8					ucSubttlIdx;			/**< user�� ���� ���� �� Subtitle Index */
		HBOOL					bSubttlUserFlag;		/**< user�� ���� ���� �Ǿ������� ��Ÿ���� Flag */
		DxStereoSelect_e		eSoundMode; 			/**< sound mode : STEREO, MONO L, MONO R, MONO L/R */
		DxDualMonoSelect_e		eDualmonoSelect;		/**< dualmono mode : BOTH, MONO L, MONO R */
		HUINT8					aucAudioLanguage[DxLANG_CODE_LEN];	/**< Audio Language User Selection */
		HBOOL					bAudUserSetFlag;		/**< sound track menu���� user�� �ٸ� �� dolby audio, audio description ���� �����ߴ��� ���ߴ� ���� ���θ� ��Ÿ�� */
		HBOOL					bAudioDescription;		/**< aduio description : User Select Audio Description ���� ���� */
		HUINT32 				ulRatingPassedEventId;	/**< rating check pass�� ��� service info�� event id�� �����صд�.
															pass���� ���� ���, 0xffffffff���� ���´�.
															event_id�� 0~0xffff ������ 16��Ʈ ���� ������, memory db���� ����Ǵ� �������� power reset�� ���󰡴� �� */
		//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		/////////////////////////////////////////
		//	service �Ӽ� �� HBOOL type�� ��� ����Ѵ�
		//---------------------------------------
		DbSvc_UserFlag1_e			eUserFlag1; 			/**< 32�� ���� ���� ������ ���� user flag - enum�� ������� OR �����ϸ�� */
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
		HUINT8				szBouquetList[DxBOUQUET_LIST_BUF_LEN];	/**< service�� ���� bouquet�� �������� ����Ŵ */
#endif

#if defined(CONFIG_DB_SVC_SVCGROUP)
		HUINT8				szGroupList[DxGROUP_LIST_BUF_LEN];			/**< service�� ���� Group �� �������� ����Ŵ */
#endif

} DbSvc_Info_t;
#else
typedef struct
{
	DxService_t			stDxInfo;

	// �ӽ� �߰�: CAS ����ڿ� �ǳ� �� DxService_t�� ���� ������ �ٸ��� ó���� ������ �����ϵ��� �Ѵ�.
	// ToDo: Start
#if defined(CONFIG_MW_CI)
	#if defined(CONFIG_MW_CI_PLUS)
		HUINT32 			ulTimeStamp;							/** CI protection descriptor save time **/
		HUINT8				ucNumOfEntries; 						/** number_of_entries, match_brand_flag�� false��� �� ���� 0 **/
		HUINT16 			ausCamBrandId[MAX_CAM_BRAND_NUM];		/** cicam_brand_identifier **/
		HBOOL				bSelectableSvcFlag;
	#endif
#endif

	/////////////////////////////////////////
	//	service �Ӽ� �� HBOOL type�� ��� ����Ѵ�
	//---------------------------------------
	DbSvc_UserFlag1_e			eUserFlag1; 			/**< 32�� ���� ���� ������ ���� user flag - enum�� ������� OR �����ϸ�� */
	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
} DbSvc_Info_t;
#endif

#if 0
/* LCN �߰� �� �ʿ��� info */
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

	/**< satellite type�� �ʿ��� ������,
		pre-defined chlist�� ���� antenna info�� ���߿� �������� ���,
		antenna index�� mapping ��ų �� �ִ� ������ ������ �ʿ��ϱ� ���� */
	SVC_SatType_t				eSatType;

	HUINT8						ucAntId;

	DxSat_TuningTPInfo_t		stDxInfo;
} DbSvc_SatTuningInfo_t;

typedef struct
{ // TODO: enum ������ ����
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
{ // TODO: enum ������ ����
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
{ // TODO: enum ������ ����
	HUINT32					ulFreq;			/**< analog channel tuning frequency - what Hz? */
	HUINT8					usProgNum;		/**< Program�� Number of Current Display */
	SVC_CH_ANA_TunerBand_e	ucBand;			/**< Band (Frequency�� ���� ����.) of Current Display */
	HUINT8					ucChanNum;		/**< Channel Number (Frequency�� ���� ����.) of Current Display */
	HUINT8					ucOffset;		/**< Fine Tune Offset (���� ���� ��) of Current Display */
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
	HUINT16				usTsIdx;			/**< db�� ���� �� �ο��Ǵ� primary key */
	HUINT16             usUniqueId;			/**< tp�� unique�ϰ� ���� ���� id. 1���� ���������� �Ҵ��. DB����� ��� ts index�� ��ü ������ */
	HUINT16             usNetIdx;			/**< ts�� ���� network�� ����Ŵ */
	HUINT16             usTsId;				/**< ts id */
	HUINT16             usOnId;				/**< original network id */
 	HUINT8              ucTunerId;			/**< tuner id : 0, 1, 2, ... */
 	DxDeliveryType_e  	eDeliType;			/**< tuner�� �޾� ���̴� ��ȣ����� �ǹ��Ѵ�. ���̺��� ���ؼ� �����İ� �������� �� ����� ���̺�� �����ؾ� ��. */
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
	HUINT16				usPrvIdx;			/**< db�� ���� �� �ο��Ǵ� primary key */
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
	HUINT16				usNetIdx;			/**< db�� ���� �� �ο��Ǵ� primary key */
	DxDeliveryType_e	eOrgDeliType;		/**< Ter+Sat ��� ���� combo ���� ���, �ߺ��Ǵ� network�� �����ϱ� ���ؼ� original delivery type�� ���� �����Ѵ�. dynamic update�� ���ؼ��� ������ */
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
	HUINT16				usBouqIdx;			/**< db�� ���� �� �ο��Ǵ� primary key */
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
	HUINT16				usGroupIdx;			/**< db�� ���� �� �ο��Ǵ� primary key */
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
	HUINT16             usNetIdx;		/**< ts�� ���� network�� ����Ŵ */
	HUINT16             usTsId;			/**< ts id */
	HUINT16             usOnId;			/**< original network id */
 	DxDeliveryType_e  	eDeliType;		/**< tuner�� �޾� ���̴� ��ȣ����� �ǹ��Ѵ�. ���̺��� ���ؼ� �����İ� ���´ٸ� �����İ� �´� Ÿ����. --> QPSK, QAM, OFDM���� ������ �� �´� �� �ƴұ�? */
} DbSvc_NetTsInfo_t;
#endif

/** svc info search condition
	svc info�� �Ϻ� field�� �����ϰ�, �˻����ǿ� ���Խ�Ű�� �ʴ� ��� enum ���� ��� all�� �׿ܿ� ���� -1�� �ο��ϸ� �ȴ� */
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
	ts info�� ��κ��� field�� �����ϰ�, �˻����ǿ� ���Խ�Ű�� �ʴ� ��� enum ���� ��� all�� �׿ܿ� ���� -1�� �ο��ϸ� �ȴ� */
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
	HUINT32 						ulLnbFreq;				/**< LNB frequency �Ǵ� CH_UNIVERSAL_LNB_FREQ(0xffffffff) */
	DxSAT_LnbVoltage_e 				eLnbVoltage; 			/**< Voltage drop compensation */
} DbSvc_LnbAntInfo_t;

typedef struct
{
	/* LNB Parameters */
	HUINT32 						ulLnbFreq;				/**< LNB frequency �Ǵ� CH_UNIVERSAL_LNB_FREQ(0xffffffff) */
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
	HUINT32 						ulScdUserBandNum2;		/**< Tuner 2�� SCD user band number. �������� �о �־��. */ // MOON_SCD
	HUINT32 						ulScdUserBandFreq2;		/**< Tuner 2�� SCD user band frequency. �������� �о �־��. */
	HUINT32 						ulScdUserBandNum3;		/**< Tuner 3�� SCD user band number. �������� �о �־��. */
	HUINT32 						ulScdUserBandFreq3;		/**< Tuner 3�� SCD user band frequency. �������� �о �־��. */
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
  @description	: service db module�� ���� ���� �ʱ�ȭ. ���� �ܰ迡�� �ѹ��� �Ҹ���.
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - �ʱ�ȭ ����
*/
void		DB_SVC_Commit (void);
HERROR		DB_SVC_GetChanged (DbSvc_Table_e eTable, void **apvList, HINT32 *pnUpdateNum, void **apvRemoveList, HINT32 *pnRemoveNum);
void		DB_SVC_SetNotifier (HBOOL bSet);

#if 0
/**
  @description	: enum�� ����� user flag ���� �˾� �� ����
  @type			: Inline
*/
HBOOL		DB_SVC_GetUserFlag1(DbSvc_Info_t *pstSvcInfo, DbSvc_UserFlag1_e eUserFlag1);

/**
  @description	: enum�� ����� user flag ���� setting�� ����
  @type			: Inline
*/
void		DB_SVC_SetUserFlag1(DbSvc_Info_t *pstSvcInfo, DbSvc_UserFlag1_e eUserFlag1, HBOOL bSetValue);
#endif
/**
  @description	: enum�� ����� user flag ���� �˾� �� ����
  @type			: Inline
*/
HUINT32		DB_SVC_GetUserFlag1(const DbSvc_Info_t *pstSvcInfo);



/**
  @description	: cache�� service db�� flash�Ǵ� hdd�� sync��Ų��.
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - sync ����
*/
HERROR		DB_SVC_Sync(ApiSyncMode_t eApiSync);


/**
  @description	: service handle ���� hash table init
  @type			: Inline
*/
void		DB_SVC_SyncHash (void);

/**
  @description	: memory db�� �����Ͽ� ��� service db�� �����Ѵ�.
  @type			: Inline
*/
void		DB_SVC_Close(void);

/**
  @description	: pre-programmed channel list�� memory db�� �ε��Ѵ�.
  				  pre-chlist�� ���� csv(comma-seperated..) ������ ��� ���̸�, �����ڵ�� �и��Ǿ� �ִ�.
				  antenna ������ �̸� ���õǾ� �־�� ä���� �ε��� �� ����
  @type			: sync(INLINE) or async as ApiSyncMode_t
  @return	ERR_OK : �Ϸ�
 			ERR_FAIL : ����
*/

HERROR		DB_SVC_LoadPreProgrammedChannel(HxCountry_e eCountryId);


/**
  @description	: ��� service/ts/provider/network info �� service db�� ��� record ����
  @type			: Inline
  @return		: ����
*/
void		DB_SVC_ClearAllInfo(void);

/*
  @description	: file db�� ������ memory db�� �ε� �Ϸ� �� TRUE�� return �Ѵ�. file db ��� ������ ���� FALSE ����
  @type			: Inline
*/
HBOOL		DB_SVC_IsMemoryLoadingDone(void);

/**
  @description	: DB_SVC_AddXXXInfo() �� table�� info(record �ϳ�)�� �߰��ϴ� API.
  @type			: Inline
  @param 1		: [in]  XXXinfo structure to be added
  @param 2		: [out] service handle
  @return		: ERR_OK
				: ERR_FAIL - add ����
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
  @description	: DB_SVC_DeleteXXXInfo() �� table�� info(record �ϳ�)�� ����� API.
  @type			: Inline
  @param 1		: [in]  XXXinfo structure to be deleted
  @param 2		: [out] index of added info (record)
  @DB_SVC_DeleteServiceInfo()�� ��� param1�� index�� �ƴ϶� service handle�̴�.

  @return		: ERR_OK
				: ERR_FAIL - delete ����
*/

HERROR		DB_SVC_DeleteServiceInfo(Handle_t hSvc);
HERROR		DB_SVC_DeleteAllServiceInfo(void); // by moon
HERROR		DB_SVC_DeleteAllNetworkInfo(void); // by Moon
HERROR		DB_SVC_DeleteTsInfo(HUINT16 usTsIdx);
HERROR		DB_SVC_DeleteAllTsInfo(void); // by Moon
HERROR		DB_SVC_DeleteProviderInfo(HUINT16 usPrvIdx);
HERROR		DB_SVC_DeleteAllProviderInfo(void); // by Moon


/** @description	:  all service list���� �˻� ���ǿ� �ش��ϴ� ��� ���񽺸� ����� */
void		DB_SVC_DeleteServiceList(DbSvc_Condition_t *pstCond);

/** @description	:  ��ġ �� cancle �Ǹ� ��ġ ���̴� ��� service�� �����  */
void		DB_SVC_DeleteAllSearchingService(void);

/** @description	:  search �� �Ϸ�Ǹ� search flag�� �����ش� */
void		DB_SVC_RemoveSearchFlag(void);

/** @description	:  �ش� section�� temporary alive flag�� �����Ѵ� */
void		DB_SVC_SetTemporaryAliveFlag(DxSvcSection_e eSection, DxSvcType_e eSvcType);

/** @description	:  temporary alive flag�� �ش�Ǵ� ���񽺸� ��� �����ش� */
void		DB_SVC_DeleteTemporaryAliveService(void);

/** @description	: 	original delivery type�� �ش��ϴ� ��� ���񽺸� ����� */
HERROR		DB_SVC_DeleteAllNetworkService(DxDeliveryType_e eOrgDeliType);

/**
  @description	: DB_SVC_UpdateXXXInfo() �� table�� info�� ������Ʈ�ϴ� API.
  @type			: Inline
  @param 1		: [in] index of info to be updated.
  @param 2		: [in] XXX info structure to be updated.
  @DB_SVC_UpdateXXXInfo()�� ��� param1�� index�� �ƴ϶� service handle�̴�.

  @return		: ERR_OK
				: ERR_FAIL - ������Ʈ ����
*/
HERROR		DB_SVC_UpdateServiceInfo(Handle_t hSvc, DbSvc_Info_t *pstSvcInfo);
HERROR		DB_SVC_UpdateNetworkInfo(HUINT16 usNetIdx, DbSvc_NetInfo_t *pstNetworkInfo);
HERROR		DB_SVC_UpdateTsInfo(HUINT16 usTsIdx, DbSvc_TsInfo_t *pstTsInfo);
HERROR		DB_SVC_UpdateProviderInfo(HUINT16 usPrvIdx, DbSvc_PrvInfo_t *pstPrvInfo);

/* DB_SVC_UpdateServiceInfo���� field ���� ������Ʈ �����ϰ� ������ �Լ� */
HERROR		DB_SVC_UpdateServiceInfoEx(Handle_t hSvc, DbSvc_FieldList_e eSvcField, DbSvc_FieldListExt_e eSvcFieldExt, DbSvc_Info_t *pstSvcInfo);

/**
  @description	: �־��� service handle�� service uid�� �о���� API
  @type			: Inline
*/
#define		DB_SVC_GetServiceUID(hSvc)		((HUINT16)(hSvc & 0xFFFF))

/**
  @description	: �־��� service handle�� service info�� �о���� API
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - Get ����
*/
HERROR		DB_SVC_GetServiceInfo(
								Handle_t 		 hSvc, 			/**< [in]  ������ service handle */
								DbSvc_Info_t 	*pstSvcInfo);	/**< [out] svcinfo�� ������ structure ������ */

/**
  @description	: �־��� Original network Id �� �ش� ��Ʈ��ũ�� ���� ���� ���ļ� ������ ���� svc�� ������  �о���� API
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - Get ����
*/
HERROR		DB_SVC_GetSearchResultInfoLowFreq(HUINT16		 usOnId,			/* [in] �˻��� ��Ʈ��ũ */
												HUINT32		*pulFreq,	  		/* [out] �ش� �ּ� Frequency */
												HUINT32		*pulSnr, 	   		/* [out] Snr ���� */
												HUINT32		*pulBer,    		/* [out] ber ���� */
												HINT32		*pnStrength );		/* [out] strength ���� */


/**
  @description	: GetXXXInfo �Լ���. �ش� index�� XXXinfo�� ������ API
  @type			: Inline
  @param1		: [in]  ������ XXXinfo�� index
				  �Ǵ� service handle - DB_SVC_GetTsInfoByServiceHandle(), DB_SVC_GetNetworkInfoByServiceHandle(), DB_SVC_GetPrvInfoByServiceHandle()
				  �Ǵ� unique id - DB_SVC_GetTsInfoByUniqueId()
  @param2		: [out] XXXinfo�� ������ structure ������
  @return		: ERR_OK
				: ERR_FAIL - Get ����
*/
HERROR		DB_SVC_GetTsInfo(HUINT16 usTsIdx, DbSvc_TsInfo_t *pstTsInfo);
HERROR		DB_SVC_GetTsInfoByUniqueId(HUINT16 usUniqueId, DbSvc_TsInfo_t *pstTsInfo);
HERROR		DB_SVC_GetTsInfoByServiceHandle(Handle_t hSvc, DbSvc_TsInfo_t *pstTsInfo);

/**
  @description	: �־��� ������ �����ϴ� ��� TS�� ã�� API
				  ts info�� alloc �ؼ� �ֱ� ������ ����Ŀ��� �ݵ�� DB_SVC_FreeTsInfoList()�� free����� �Ѵ�
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find ����
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
  @description	: �־��� ������ �����ϴ� service ������ ������ API
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - Get ����
*/
HERROR		DB_SVC_GetServiceCount(
								DbSvc_Group_e	 	 eSvcGroup, 	/**< [in]  filtering ���� 1 - ���� �׷� */
								DbSvc_Attr_t 		*pstAttr,	 	/**< [in]  filtering ���� 2 - ���� �Ӽ� */
								HINT32 				*pnCnt);		/**< [out] �ش� ������ �����ϴ� service ������ ������ ������ */

/**
  @description	: �־��� service handle�� channel number�� �о���� API
  @type			: Inline
  @param1		: [in]  service handle
  @param2		: [out] channel number�� ��ƿ� ������
  @return		: ERR_OK
				: ERR_FAIL - Get ����
*/
HERROR		DB_SVC_GetServiceNumber(Handle_t hSvc, HINT32 *pnChNum);

HERROR		DB_SVC_GetServiceHandleBySvcIdx(HUINT16 usSvcIdx, Handle_t *phSvc);

/**
  @description	: �־��� channel number�� service handle�� �о���� API
  @type			: Inline
  @param1		: [in]  original delivery type (hybrid ��ǰ�� ���, delivery ����  �ߺ� channel number�� ������ �� �ֱ⿡ delivery type�� �ʿ���)
  @param2		: [in]  service type (���� delivery�� service type�� �ߺ� ä�� �ѹ��� ���� �� �� �����Ƿ�, �ʿ��ϸ� service type�� ������ �� ��)
  @param3		: [in]  channel number
  @param4		: [out] service handle�� ��ƿ� ������
  @return		: ERR_OK
				: ERR_FAIL - Get ����
*/
HERROR		DB_SVC_GetServiceHandleByNumber(DxDeliveryType_e eOrgDeliType,
							DxSvcType_e eSvcType, HINT32 nChNum, Handle_t *phSvc);

/**
  @description	: service info�� ���� �ʿ��� �ʵ��� ������ ���´�
  @type			: Inline
  @param1		: [in]  service handle
  @param2		: [out] field ���� ���� ������
  @return		: ERR_OK
				: ERR_FAIL - Get ����
*/
HERROR		DB_SVC_GetServiceIdFromSvcHandle(Handle_t hSvc, HUINT16 *pusSvcId);
HERROR		DB_SVC_GetUniqueIdFromSvcHandle(Handle_t hSvc, HUINT16 *pusUniqueId);
HERROR		DB_SVC_GetServiceHandle(DxSvcSection_e eSvcSection, HUINT16 usTsIdx, HUINT16 usSvcId, Handle_t *phSvc);
HERROR		DB_SVC_GetServiceHandleByOnIdAndSvcId(DxSvcSection_e eSvcSection, HUINT16 usOnId, HUINT16 usSvcId, Handle_t *phSvc);

/**
  @description	: ���� channel number�� ���� ��� ���� ����Ʈ�� ���´�
  @type			: Inline
  @param4		: [out]
  @return		: ERR_OK
				: ERR_FAIL - Get ����
*/
HERROR		DB_SVC_FindServiceListByNumber(DxDeliveryType_e 	  eOrgDeliType,		/**< [in]  filtering ���� 1 - delivery type: original delivery type, all ���� sat/ter/.. �� OR ���� */
										DxSvcType_e 			  eSvcType, 		/**< [in]  filtering ���� 2 - service type */
										DbSvc_VisibleType_e 	  eVisibleType, 	/**< [in]  filtering ���� 3 - visible type */
										HINT32 					  nChNum, 			/**< [in]  filtering ���� 4 - �˻��� channel number */
										HUINT32 				 *pulSvcCount, 		/**< [out] �ش� ������ �����ϴ� service ������ ������ ������ */
										Handle_t 				**pphSvcList);		/**< [out] service handle list�� ��ƿ� ������. �޵�� DB_SVC_FreeServiceList()�� free ������� */

/**
  @description	: ���ǿ� �´� ts info�� ������ �о�´�.
  @type			: Inline
  @param1		: [in] �˻� ����, NULL�̸� �ƹ��� ������ ���� ��ü ts ������ �о����, ������ ���� ���, ����ü �ʵ尪�� -1�̴�
  @param2		: [out] ts ������ ������ ������
  @return		: ERR_OK
				: ERR_FAIL - Get ����
*/
HERROR		DB_SVC_GetTsCount(DbSvc_TsCondition_t *pstCond, HINT32 *pnCnt);

/**
  @description	: �����ϴ� ��� XXXinfo�� ������ ������ API
  @type			: Inline
  @param1		: [out] network ������ ������ ������
  @return		: ERR_OK
				: ERR_FAIL - Get ����
*/
HERROR		DB_SVC_GetNetworkCount(HINT32 *pnCnt);
HERROR		DB_SVC_GetProviderCount(HINT32 *pnCnt);


/**
  @description	: �־��� ������ �����ϴ� service�� ã�� API
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find ����
*/
HERROR		DB_SVC_FindServiceHandleNearbyLcn(DxDeliveryType_e eOrgDeliType, HUINT16 usLcn, HUINT32 ulPrivateMode, Handle_t* phPrevSvc, Handle_t* phUpperSvc);



/**
  @description	: �־��� ������ �����ϴ� service�� ã�� API
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find ����
*/
HERROR		DB_SVC_FindServiceHandle(
								DbSvc_Group_e 			 eSvcGroup, 	/**< [in]  filtering ���� 1 - ���� �׷� */
								DbSvc_Attr_t 			*pstAttr, 		/**< [in]  filtering ���� 2 - ���� �Ӽ� */
								Handle_t 				 hOffsetSvc, 	/**< [in]  filtering ���� 3 - ���صǴ� service handle. ���� next�� ã�����ϸ� current service�� �ش��. */
								DbSvc_FindDirection_e	 eDirection, 	/**< [in]  filtering ���� 4 - ���⿡ ���� ���� */
								Handle_t 				*phSvc);		/**< [out] �ش� ������ �����ϴ� service handle�� ������ ������ */

HERROR		DB_SVC_FindServiceHandleByCondition (
													const DbSvc_Condition_t		*pstCond,
													Handle_t					*phSvc
												);

/**
  @description	: �־��� delivery type, triple id ������ �����ϴ� service�� ã�� API
				  ter/cab �� ����Ѵ�. ������ ����߰����� ������ �ʿ��� (antid)
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find ����
*/
HERROR		DB_SVC_FindServiceHandleByTripleId(
												DxDeliveryType_e 	 eOrgDeliType, 	/**< [in]  ��ġ �� �Ҵ�� orginal delivery type */
												DbSvc_TripleId_t 	 stTripleId, 	/**< [in]  triple id (onid, tsid, svcid) */
												Handle_t 			*phSvc);		/**< [out] �ش� ������ �����ϴ� service handle�� ������ ������ */

HERROR		DB_SVC_FindSvcHandleListWhereSearchFlagTrue(HINT32 *pnCnt, Handle_t **pphSvcList);
/**
  @description	: �־��� ������ �����ϴ� ��� service�� ã�� API
				  ulMaxFindNum�� 0�� ���, hOffsetSvc�� eCondition�� ���õȴ�.

				  [note] miniocto������ ���� �� ���� �Ķ���͸� ������� �ʰ�, ���� ������ ������ ȿ���� ����.
					ulMaxFindNum = 0;
					hOffsetSvc = HANDLE_NULL;
					eDirection = eSvcFindDirection_FowardFromStart;

  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find ����
*/
 HERROR		DB_SVC_FindServiceList(
								DbSvc_Group_e 			  eSvcGroup, 		/**< [in]  filtering ���� 1 - ���� �׷� */
								DbSvc_Attr_t 			 *pstAttr, 			/**< [in]  filtering ���� 2 - ���� �Ӽ� */
								HUINT32					  ulMaxFindNum, 	/**< [in]  filtering ���� 3 - ������ �ִ� ���� ����, 0�̸� LIMIT�� ���� */
								Handle_t				  hOffsetSvc, 		/**< [in]  filtering ���� 4 - (Find ����) ���صǴ� hSvc */
								DbSvc_FindDirection_e	  eDirection, 		/**< [in]  filtering ���� 5 - ���⿡ ���� ���� */
								HUINT32 				 *pulCount, 		/**< [out] �ش� ������ �����ϴ� service ������ ������ ������ */
								Handle_t 				**pphSvcList);		/**< [out] �ش� ������ �����ϴ� service list�� ������ ������ */

/**
  @description	: �־��� ������ �����ϴ� ��� service�� ã�� API
				  DeliveryType + Service_id�� �Է����� �ָ� �ش� serice_id�� �´� service handle�� �˷� �ִ� �Լ�. ���� �������� ��� ������service handle�� �̰��� ����
				  ��� �� DB_SVC_FreeServiceList()�� ���� handle list�� free���ش�.
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find ����
*/
HERROR		DB_SVC_FindServiceListBySvcId(
								DxDeliveryType_e 	  eOrgDeliType, 	/**< [in]  filtering ���� 1 - original delivery type */
								HUINT16 			  usSvcId, 			/**< [in]  filtering ���� 2 - service id */
								HINT32 				 *pnSvcCnt, 		/**< [out] �ش� ������ �����ϴ� service ������ ������ ������ */
								Handle_t 			**pphSvcList);		/**< [out] �ش� ������ �����ϴ� service list�� ������ ������ */


HERROR		DB_SVC_FindServiceListByNationalSvcId(DxDeliveryType_e		  eOrgDeliType, 		/**< [in]  filtering ���� 1 - original delivery type */
													HUINT16				  usNationalSvcId, 		/**< [in]  filtering ���� 2 - national service id - Canal Ready Only */
													HINT32				 *pnSvcCnt, 			/**< [out] �ش� ������ �����ϴ� service ������ ������ ������ */
													Handle_t			**pphSvcList);			/**< [out] �ش� ������ �����ϴ� service list�� ������ ������ */

/**
  @description	: �־��� ������ �����ϴ� ��� service�� ã�� API
				  DeliveryType + triple_id�� �Է����� �ָ� �ش� triple_id�� �´� service handle�� �˷� �ִ� �Լ�. ���� �������� ��� ������service handle�� �̰��� ����
				  ��� �� DB_SVC_FreeServiceList()�� ���� handle list�� free���ش�.
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find ����
*/
HERROR		DB_SVC_FindServiceListByTripleId(
								DxDeliveryType_e 	  eOrgDeliType, 	/**< [in]  filtering ���� 1 - original delivery type */
								DbSvc_TripleId_t 	  stTripleId,	 	/**< [in]  filtering ���� 2 - triple id */
								HINT32 				 *pnSvcCnt, 		/**< [out] �ش� ������ �����ϴ� service ������ ������ ������ */
								Handle_t 			**pphSvcList);		/**< [out] �ش� ������ �����ϴ� service list�� ������ ������ */

/**
  @description	: �־��� ������ �����ϴ� ��� service�� ã�� API
  				  ��� �� DB_SVC_FreeServiceInfoList()�� ���� service handle�� service info list�� free����� �Ѵ�
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find ����
*/
HERROR		DB_SVC_FindServiceListWithInfo(
								DbSvc_Group_e 		 eSvcGroup, 		/**< [in]  filtering ���� 1 - ���� �׷� */
								DbSvc_Attr_t 		 *pstAttr, 			/**< [in]  filtering ���� 2 - ���� �Ӽ� */
								HUINT32 			 *pulCount, 		/**< [out] �ش� ������ �����ϴ� service ������ ������ ������ */
								Handle_t 			**pphSvcList,		/**< [out] �ش� ������ �����ϴ� service list�� ������ ������ */
								DbSvc_Info_t		**ppstSvcInfoList);	/**< [out] �ش� ������ �����ϴ� service list�� ������ ������ */

/**
  @description	: DB_SVC_FindServiceList()���� �Ҵ�� service list�� free�ϴ� API
  @type			: Inline
  @return		: ����
*/
void		DB_SVC_FreeServiceList(Handle_t *phSvcList);         /**< [in]  service list */

void		DB_SVC_FreeServiceInfoList(HINT32 *pnLcnList, Handle_t *phSvcList, DbSvc_Info_t *pSvcInfoList);

/**
  @description	: �־��� ������ �����ϴ� ��� TS�� ã�� API
				  ts info�� alloc �ؼ� �ֱ� ������ ����Ŀ��� �ݵ�� DB_SVC_FreeTsInfoList()�� free����� �Ѵ�
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find ����
*/
void		DB_SVC_InitSvcAttribute (DbSvc_Attr_t *pstAttr);
void		DB_SVC_InitSvcFindCondition(DbSvc_Condition_t *pstCond);
HERROR		DB_SVC_FindSvcInfoList(DbSvc_Condition_t *pstCond, HINT32 *pnCnt, DbSvc_Info_t **ppstInfoList);
HERROR		DB_SVC_FindSvcHandleList(DbSvc_Condition_t *pstCond, HINT32 *pnCnt, Handle_t **pphSvcList);

HINT32		DB_SVC_FindSvcUidList (const DbSvc_Condition_t *pstCond, HINT32 *pnUids, HINT32 n);

/**
  @description	: �־��� ������ �����ϴ� triple id list�� ���´�
				  ��� �Ŀ��� free�ؾߵ�
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - find ����
*/
HERROR		DB_SVC_FindTripleIdAndSvcTypeList(DxDeliveryType_e eOrgDeliType, HUINT8 ucAntIdx, HINT32 *pnCnt, DbSvc_TripleId_t **ppstTripleIdList, DxSvcType_e **ppeSvcTypeList, Handle_t **pphSvcList);
void		DB_SVC_FreeTripleIdAndSvcTypeList(DbSvc_TripleId_t *pstTripleIdList, DxSvcType_e *peSvcTypeList);

/**
  @description	: ���� �ڵ��� ��ȿ���� üũ�ϴ� API
				  DB_SVC_FindServiceHandle()�ε� �����ϴ� �������� �� �� ������ ����� ���Ǽ��� ���� ���� API�̴�.
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - invalid ���� �ڵ�
*/
HERROR		DB_SVC_CheckValidServiceHandle(Handle_t hSvc);		/**< [in] �˻��� ���� �ڵ� */

/**
  @description	: Audio User set Flag�� reset�ϴ� �Լ�.
  @type 		: Inline
  @return		: ����
*/
void		DB_SVC_ResetAudioUserFlag(void);

/**
  @description	: Subtitle User set Flag�� reset�ϴ� �Լ�.
  @type			: Inline
  @return		: ����
*/
void		DB_SVC_ResetSubtitleUserFlag(void);

/**
  @description	: memory db�� field �� sync���� �ʴ� �ʵ��� ������ ����� API
  @type			: Inline
  @return		: ����
*/
void		DB_SVC_ResetNoSyncFields(void);

/**
  @description	: memory db�� field�� sync���� �ʴ� �ʵ��� rating passed event ID�ʵ带 ��� ����� API
  @type			: Inline
  @return		: ����
*/
void		DB_SVC_ResetRatingPassedEventId(void);

/**
  @description	: recent channel list�� ���񽺸� �߰��ϴ� API
				  ���� �߰��� ���񽺰� list�� �� ���� �ö󰣴�. �ִ� ���� ��ŭ(=20)�� �߰��ǰ�, ������ ���� ���񽺴� �߰����� �ʴ´�
  @type			: Inline
  @return		: ����
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
  @description	: service / video type�� ���� ��� ������ CAS ������ OR�ؼ� ������ API
  					(channel list filter���� �ַ� ����ϴ� ���)
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - get ����
*/
HERROR		DB_SVC_GetAllCasType(DxSvcType_e eSvcType, DxVideoType_e eVideoType, DxCasType_b *pbwCasType);

/**
  @description	: LCN���� ���� ���ǿ� �����ϴ� ���񽺿� ���� re-numbering�� �Ѵ�.
  				  pre chlist �뵵�� ����Ϸ��� ������, delivery type���� option�� �ξ� �ٸ� �뵵�ε� Ȱ�� �����ϰ� �Ѵ�.
	  				 * ����1 - delivery type
	  				 * ����2 - service section (ex. pre-chlist)
	  				 * ����3 - service type
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - re-numbering ����
*/
HERROR		DB_SVC_RenumberRankingToLcn(DxDeliveryType_e eOrgDeliType, DxSvcSection_e eSvcSection, DxSvcType_e eSvcType, HINT32 nStartChNum, HINT32 nEndChNum);

/**
  @description	: ���� �Ǵ� base channel number�� ����, non-LCNä�ε��� ����� ������� re-numbering�� �Ѵ�.
  				  (�������, ��, svcIdx�� order number�� ��)
	  				 * ����1 - delivery type
	  				 * ����2 - service type
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - re-numbering ����
*/
HERROR		DB_SVC_RenumberSavingOrderToChNum(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HINT32 nBaseStartChNum);


/**
  @description	: non-LCN ������ ä���� LCN�� ���� re-numbering �Ѵ�. LCN�� ranking�� ������ �ϴ� ����
  				  (LCN���� order number�� ��)
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - ����
*/
HERROR		DB_SVC_RenumberLcnOrderToChNum(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType, HINT32 nBaseChNum);

/**
  @description	: LCN ��� �� non-LCN ������ ���� spec�� ���� ���, �� �Լ��� ����Ͽ� LCN �������� �̾ numbering�� �ϰ� �Ѵ�.
  @type			: Inline
  @return		: ERR_OK
				: ERR_FAIL - ����
*/
HERROR		DB_SVC_MakeNonLcnNumber(DxDeliveryType_e eOrgDeliType, DxSvcType_e eSvcType);

/**
  @description	: �� �Լ��� non-LCN ����̳�, LCN�� ���� ��� �ѹ��� �ο��� �� Ȱ��� �� �ִ�. ���ǿ� �´� ������ service ������ �����ش�.
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
  @remark   ����DB�� antenna info�� �߰��ϴ� API
  @remark	type : Inline

  @return	ERR_OK : �Ϸ� \n
 			ERR_FAIL : ����
*/
HERROR	DB_SVC_AddAntInfo (
					DbSvc_AntInfo_t 	*pstAntInfo, 	/**< �߰��� antenna info */
					HUINT16 			*pusAntIdx		/**< db�� �߰� �� �ڵ� ������ index */
					);
/**
  @remark   ����DB�� �־��� antenna index�� �ش��ϴ� antenna info�� ����� API
  @remark	type : Inline

  @return	ERR_OK : �Ϸ� \n
 			ERR_FAIL : ����
*/
HERROR	DB_SVC_DeleteAntInfo (
					HUINT16 	usAntIdx		/**< ������� �ϴ� antenna index */
					);
/**
  @remark   ����DB�� ��� antenna info�� ����� API
  @remark	type : Inline

  @return	ERR_OK : �Ϸ� \n
 			ERR_FAIL : ����
*/
HERROR	DB_SVC_DeleteAllAntInfo (void);
/**
  @remark   ����DB�� �־��� antenna index�� �ش��ϴ� antenna info�� ������Ʈ �ϴ� API
  @remark	type : Inline

  @return	ERR_OK : �Ϸ� \n
 			ERR_FAIL : ����
*/
HERROR	DB_SVC_UpdateAntInfo (
					HUINT16 			 usAntIdx,		/**< ������Ʈ �� antenna index */
					DbSvc_AntInfo_t 	*pstAntInfo 	/**< ������Ʈ ���� */
					);
/**
  @remark   ����DB�� �־��� antenna index�� �ش��ϴ� antenna info�� �����ؿ��� API
  @remark	type : Inline

  @return	ERR_OK : �Ϸ� \n
 			ERR_FAIL : ����
*/
HERROR	DB_SVC_GetAntInfo (
					HUINT16 			 usAntIdx,		/**< �а��� �ϴ� antenna index */
					DbSvc_AntInfo_t 	*pstAntInfo 	/**< ��ƿ� ����ü */
					);
/**
  @remark   ����� antenna ������ ������ API
  @remark	type : Inline

  @return	ERR_OK : �Ϸ� \n
 			ERR_FAIL : ����
*/
HERROR	DB_SVC_GetAntCount (HINT32 *pnCnt);		/**< out: ��ƿ� ����ü */

/**
  @remark   ����� ��� ���׳��� antIdx�� ������ ������ API
  @remark	type : Inline

  @return	ERR_OK : find ���� \n
 			ERR_FAIL : ���� �Ǵ� ���׳� ���� ����
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
  @remark   DB_SVC_FindAllAntIdxList() �Լ��� ���Ͽ� ������ antenna index list�� free�ϴ� API
  @remark	type : Inline

  @return	����
*/
void	DB_SVC_FreeAntList (HINT32 *pnAntIdxList);
HERROR	DB_SVC_ReloadAllAntInfo (void);
HERROR	DB_SVC_AntennaSync(void);


/**
  @remark   Default Authority���� �Լ�
  @remark	type :

  @return	����
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

