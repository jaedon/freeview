/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _svc_si_lib.h
	@brief

	Description:  									\n
	Module: SVC / SI_NEW					\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#ifndef ___INT_SVC_SI_LIB_H___
#define ___INT_SVC_SI_LIB_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define MAX_LANG_PER_ONE_TRACK				4				// 한 트랙이 제공하는 최대 언어 수

#define INITIAL_TELETEXT					0x01
#define TELETEXT_SUBTITLE 					0x02
#define ADDITIONAL_INFO_PAGE				0x03
#define PROG_SCHEDULE_PAGE					0x04
#define TTX_SBTL_PAGE_HERARING_IMPAIRED		0x05

// Service Type temp
//#define		SERVICE_VIDEO				0
#define SERVICE_TYPE_DIGITAL_TV		0x01
#define SERVICE_TYPE_DIGITAL_RADIO	0x02
#define SERVICE_TYPE_TELETEXT		0x03
#define SERVICE_TYPE_NVOD_REFERENCE	0x04
#define SERVICE_TYPE_NVOD_TIMESHIFT	0x05
#define SERVICE_TYPE_MOSAIC			0x06
#define SERVICE_TYPE_PAL			0x07
#define SERVICE_TYPE_SECAM			0x08
#define SERVICE_TYPE_DD2_MAC		0x09
#define SERVICE_TYPE_FM_RADIO		0x0A
#define SERVICE_TYPE_NTSC			0x0B
#define SERVICE_TYPE_DATA_BROADCAST	0x0C
#define SERVICE_TYPE_DVB_MHP		0x10

#define	SERVICE_TYPE_MPEG2_HD					0x11
#define	SERVICE_TYPE_MPEG2_HD_NVOD_TIMESHIFT	0x12
#define	SERVICE_TYPE_MPEG2_HD_NVOD_REFERENCE	0x13
#define	SERVICE_TYPE_AAC_AUDIO					0x14
#define	SERVICE_TYPE_AAC_V2_AUDIO				0x15
#define	SERVICE_TYPE_AVC_SD_DIGITAL_TV			0x16
#define	SERVICE_TYPE_AVC_SD_NVOD_TIMESHIFT		0x17
#define	SERVICE_TYPE_AVC_SD_NVOD_REFERENCE		0x18
#define	SERVICE_TYPE_AVC_HD_DIGITAL_TV			0x19
#define	SERVICE_TYPE_AVC_HD_NVOD_TIMESHIFT		0x1A
#define	SERVICE_TYPE_AVC_HD_NVOD_REFERENCE		0x1B
#define	SERVICE_TYPE_HEVC_DIGITAL_TV			0x1F
#define	SERVICE_TYPE_AVC_HD_DIGITAL_TV2			0x86
#define	SERVICE_TYPE_SUBFEED						0xD3


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

// ----- PMT INFO ----- //
// !!!! 주의 !!!!! //
// 모든 Element들은 반드시 usPid와 ucComponentTag를 앞에 세우고 다른 정보를 뒤로 놓도록 한다.
// 추후, 공통 코드를 위한 기본적인 설계이다.
typedef struct
{
	HUINT16				 usPid;
	HUINT8				 ucComponentTag;
} svcSi_BasicEs_t;

typedef struct
{
	HUINT16				 usPid;						/** < Video PID */
	HUINT8				 ucComponentTag;			/** < componet tag value */

	DxVideoCodec_e		 eVideoCodec;				/** < Video Codec */
	HBOOL				 bCaDesFound;
	HBOOL				 bIsHd;
} svcSi_VideoEs_t;

typedef struct
{
	AUDIO_Type_t		 eAudType;
	HUINT8				 szIso639LangCode[4];
} svcSi_Iso639_t;		 /* MwIso639LangInfo_t 과 svcSi_Iso639_t 는 동일해야 함 */

typedef struct
{
	HUINT16				 usPid;
	HUINT8				 ucComponentTag;

	DxAudioCodec_e		 eAudCodec;					/** < Audio Codec */
	SvcSi_AudCpnt_e		eAudCompType;
	svcSi_Iso639_t		 astIsoLangInfo[MAX_LANG_PER_ONE_TRACK];
	HBOOL				 bCaDesFound;
	HUINT8				 ucChannelNum;

	SUPPLEMENTARY_AUDIO_Type_t		eSuppAudioEditorial;
	HUINT8				ucMixType;
} svcSi_AudioEs_t;

typedef struct
{
	HUINT16		usPid;						/** < Subtitle PID */
	HUINT8		ucComponentTag; 			/** < componet tag value */

	HUINT8		ucType;						/** < Teletext: 0x00~0x05, Subtitle: 0x10~0x13, 0x20~0x23 */

	HUINT16		usCompositionPageId;		/** < for DVB Subtitle */
	HUINT16		usAncillaryPageId;			/** < for DVB Subtitle */

	HUINT8		ucMagazineNumber;			/** < for EBU Subtitle */
	HUINT8		ucPageNumber;				/** < for EBU Subtitle */
	HUINT8		aucLangCode [4];
	HBOOL		bCaDesFound;

} svcSi_SubtitleEs_t;

typedef struct
{
	HUINT16		usPid;
	HUINT8		ucComponentTag;

	HUINT8		ucTeltxtType;
	HUINT8		ucMagazineNumber;
	HUINT8		ucPageNumber;
	HUINT8		aucLangCode [4];
	HBOOL		bCaDesFound;
} svcSi_TeletextEs_t;

typedef struct
{
	HUINT16		usPid;
	HUINT8		ucComponentTag;

	HUINT8		aucLangCode[8]; /* CAPTION_SUPER_LANG_MAX랑 같은 값 */ // NOT USED
} svcSi_CaptionEs_t;

typedef struct
{
	HUINT16		usPid;
	HUINT8		ucComponentTag;
} svcSi_BmlEs_t;

typedef struct
{
	HUINT16		usPid;
	HUINT16		usCaSysId;
} svcSi_EcmEs_t;

typedef struct
{
	HUINT16			 usServiceId;						/** < Service ID */
	HUINT16			 usPcrPid;							/** < PCR PID */

	HUINT32			 ulPmtCRC;							/** PMT CRC_32 */
	HUINT32			 ulCasType;							/** < it's same value with DbSvc_CasType_e */

	HxList_t		*pstVideoTrack;						/** < svcSi_VideoEs_t */
	HxList_t		*pstAudioTrack;						/** < svcSi_AudioEs_t */
	HxList_t		*pstSubttlTrack;					/** < svcSi_SubtitleEs_t */
	HxList_t		*pstTeltxtTrack;					/** < svcSi_TeletextEs_t */
	HxList_t		*pstCaptionTrack;					/** < svcSi_CaptionEs_t */
	HxList_t		*pstBmlTrack;						/** < svcSi_BmlEs_t */
	HxList_t		*pstEcmTrack;						/** < svcSi_EcmEs_t */
} svcSi_PmtEsInfo_t;

typedef struct
{
	HBOOL					 bFound;
	HINT32					 nEsIdx;					// 선택되지 않았으면 -1
} svc_si_SelectedEs_t;

typedef struct
{
	HBOOL					 bFound;
	HINT32					 nMpegAudioIdx;				// 선택되지 않았으면 -1
	HINT32					 nDolbyIdx;					// 선택되지 않았으면 -1
	HINT32					 nMainIdx;					// 선택되지 않았으면 -1
	HINT32					 nAdIdx;					// 선택되지 않았으면 -1
	DxStereoSelect_e	 	 eSoundMode;				// eStereoSelect;
	DxDualMonoSelect_e		 eDualMonoSelect;			// eDualMonoSelect;
} svc_si_SelectedAudio_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      API Functions     ********************/
/*******************************************************************/

/*
	svc_si_lib_es.c
*/
extern HERROR		svc_si_LibGetAudioLanguageCodeFromPmt(HUINT32 ulActionId, SIxTable_2ndLoop_t *pstPmtElemLoop, HUINT32 ulLangMax, svcSi_Iso639_t *pstIsoLangInfo, HUINT32 *pulLangNum);
extern HERROR 		svc_si_LibGetSupplementaryAudioLanguageCodeFromPmt (HUINT32 ulActionId, SIxTable_2ndLoop_t *pstPmtElemLoop, HUINT32 ulLangMax, svcSi_AudioEs_t *pstAudioEs, HUINT32 *pulLangNum);
extern HERROR		svc_si_LibGetEcmInfoList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_EcmList_t *pstEcmList);
extern HERROR		svc_si_LibGetPmtPidFromPat(HUINT32 ulActionId, HUINT16 usSvcId, HUINT16 *pusPid);
extern HERROR		svc_si_LibGetAllAitPidFromPmt (HUINT32 ulActionId, HxList_t **ppAitPidList);

#if defined(SvcSi_SUPPORT_LIB_ES_NEW)
extern HERROR		svc_si_LibGetEsInfo(SvcSi_EsUpdateType_e eType, Handle_t hEsInfoList, HINT32 nIndex, HUINT16 *pusPid, HUINT32 *pulCodec, HUINT8 *pucComponentTag, HUINT32 *pulParam);
extern HERROR		svc_si_LibCheckDefaultPidUpdated2(HUINT32 ulActionId, SIxTable_PsiTable_t *pstPmtTable, SIxTable_PsiTable_t *pstEitTable, const SvcSi_UpdateEs_t *pstUpdate, HBOOL *pbUpdated);
extern HERROR		svc_si_LibGetServiceInfoPid2(HUINT32 ulActionId, HUINT32 ulSvcId, const SvcSi_UpdateEs_t *pstUpdate, HBOOL *pbChanged);
#endif
extern HERROR		svc_si_LibCheckDefaultPidUpdated(HUINT32 ulActionId, SIxTable_PsiTable_t *pstPmtTable, DbSvc_Info_t *pstOrgSvcInfo, DbSvc_Info_t *pstNewSvcInfo, const SvcSi_Config_t *pstConfig, HBOOL *pbUpdated);
extern HERROR		svc_si_LibGetServiceInfoPid(HUINT32 ulActionId, HUINT32 ulSvcId, DbSvc_Info_t *pstOrgSvcInfo, DbSvc_Info_t *pstDstSvcInfo, const SvcSi_Config_t *pstConfig, HBOOL *pbChanged);
extern HERROR 		svc_si_LibUpdateAllService_inSDT(HUINT32 ulActionId, const DbSvc_Info_t *pstSvcInfo, HBOOL *pbChanged);
extern HERROR		svc_si_LibGetServiceInfo(HUINT32 ulActionId, HUINT32 ulSvcId, DbSvc_Info_t *pstOrgSvcInfo, DbSvc_Info_t *pstDstSvcInfo, HBOOL *pbChanged);
extern HERROR		svc_si_LibFreePmtEsInfo(svcSi_PmtEsInfo_t *pstPmtEsInfo);

extern HERROR		svc_si_LibGetVideoInfoList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_VideoList_t *pstVideoList);
extern HERROR 		svc_si_LibGetAudioInfoList(HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_AudioList_t *pstAudioList);
extern HERROR 		svc_si_LibGetSubtitleInfoList(HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_SubtitleList_t *pstSubtitleList);
extern HERROR 		svc_si_LibGetSuperCcInfoList(HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_SubtitleList_t *pstSuperCcList);
extern HERROR		svc_si_LibFreeVideoList (SvcSi_VideoList_t *pstVideoList);
extern HERROR 		svc_si_LibFreeAudioList(SvcSi_AudioList_t *pstAudioList);
extern HERROR 		svc_si_LibFreeSubtitleList(SvcSi_SubtitleList_t *pstSubtitleList);
extern HERROR		svc_si_LibFreeEcmList (SvcSi_EcmList_t *pstEcmList);

extern HERROR		svc_si_LibSvcEsSelectAudio (const svcSi_PmtEsInfo_t *pstPmtEsInfo, const SvcSi_Config_t *pstConfig, SvcSi_SelectedEs_t *pstSelectedAudio);
extern void			svc_si_LibSvcEsPrintPriorityTable (void);
extern HBOOL		svc_si_LibSvcEsGetLangInfo (const svcSi_AudioEs_t *pstAudioEs, HxLANG_Id_e *peLangId1, HxLANG_Id_e *peLangId2, HBOOL *pbDualMono);
extern HBOOL		svc_si_LibSvcEsGetAudioTypeInfo (const svcSi_AudioEs_t *pstAudioEs, AUDIO_Type_t *peAudioType1, AUDIO_Type_t *peAudioType2, HBOOL *pbDualMono);

extern HERROR		svc_si_LibSvcEsSelectSubtitle (const svcSi_PmtEsInfo_t *pstPmtEsInfo, const SvcSi_Config_t *pstConfig, SvcSi_SelectedEs_t *pstSelectedSubtitle);

/*
	svc_si_lib_basic.c
*/
extern HERROR		svc_si_LibGetEventInfoFromEitPf(HUINT32 ulActionId, DbSvc_TripleId_t stTripleId, HBOOL bIsPresent, HBOOL bUsePmt, SvcSi_Event_t *pstEvent, SvcSi_ExtEvent_t *pstExtEvent);
extern HERROR		svc_si_LibFreeEventInfo (SvcSi_Event_t *pstEvent);
extern HERROR		svc_si_LibFreeExtEventInfo (SvcSi_ExtEvent_t *pstExtEvent);

extern HERROR 		svc_si_LibGetEmmInfoList (HUINT32 ulActionId, HUINT16 usSvcId, SvcSi_EmmList_t *pstEmmList);
extern HERROR 		svc_si_LibFreeEmmList (SvcSi_EmmList_t *pstEmmList);

extern HERROR		svc_si_LibGetAllPmtProgramsFromPat(HUINT32 ulActionId, HUINT32 *pulNumPmt, SIxTable_PatProgram_t **ppstProgramArray);
extern HERROR		svc_si_LibCheckServiceNameBySvcDes(HINT32 nNameSizeOfSvcDes, HUINT8 *pucSvcDesName, HUINT8 *szSvcInfoName, HBOOL *pbNameUpdate);
extern HERROR		svc_si_LibComposeServiceInfoInTp(HUINT32 ulActionId, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray);
extern HERROR 		svc_si_LibGetRctInfo(Handle_t hAction, Handle_t hSvc, DxService_RctInfo_t *pstRctInfo, HBOOL bMhegInterruptibleFlag);
extern HERROR		svc_si_LibCheckRctInfo(Handle_t hAction, Handle_t hSvc);

extern HERROR		svc_si_LibUpdateTimeByTdt(HUINT32 ulActionId, Handle_t hService, SIxTable_PsiTable_t *pstTdtTable);
extern HERROR 		svc_si_LibUpdateTimeByTot(HUINT32 ulActionId);
extern HERROR 	svc_si_LibSvcEsComposeServiceInfoBySdt (HUINT32 ulActionId, SIxTable_PsiTable_t *pstSdtTable, HUINT32 *pulSvcNum, DbSvc_Info_t **ppstSvcArray, HUINT32 *pulPrvNum, DbSvc_PrvInfo_t **ppstPrvArray);

extern HERROR 		svc_si_LibMakeSiSection (HUINT32 ulActionId, SvcSi_MakeSectionSpec_t *pstSpec, SvcSi_MakeSecArg_t *punArg, HUINT8 *pucRawSec, HUINT32 *pulSecLen);

extern HERROR 		svc_si_LibAddBqInfoToBqInfoTable (DbSvc_BouqInfo_t *pstBqInfo, HBOOL bCheckBouqId, SvcSi_BouquetInfoTable_t * pstBqInfoTable, HUINT16 *pusBqIndex);
extern HERROR 		svc_si_LibAddSvcInfoToSvcInfoTable (DbSvc_Info_t *pstSvcInfo, HBOOL bIsSameTp, SvcSi_ServiceInfoTable_t *pstSvcInfoTable, HUINT32 *pulSvcIndex);

/* debug functions */
extern HUINT8*		svc_si_LibGetTableTypeName(SIxTable_Type_e eTable);

/*
	svc_si_lib_delivery.c
*/
extern HERROR		svc_si_LibGetAllDeliveryTsInfoFromNit(HUINT32 ulActionId, HBOOL bNitAct, HBOOL bIncludeFreqList, HUINT32 *pulTsNum, DbSvc_TsInfo_t **ppstTsArray);
extern HERROR		svc_si_LibGetTuningInfoFromSatDsd(SIxSatDeliSystemDes_t *pstDeliDes, DbSvc_TuningInfo_t *pstTuningParam);
extern HERROR		svc_si_LibGetTuningInfoFromCabDsd(SIxCabDeliSystemDes_t *pstDeliDes, DbSvc_TuningInfo_t *pstTuningParam);
extern HERROR		svc_si_LibGetTuningInfoFromTerDsd(SIxTerDeliSystemDes_t *pstDeliDes, DbSvc_TuningInfo_t *pstTuningParam);
extern HERROR		svc_si_LibGetDeliveryTsInfoFromDsd (HUINT32 ulActionId, HUINT8 *pucDesRaw, DbSvc_TsInfo_t *pstTsInfo);

/*
	svc_si_lib_conv.c
*/
extern HERROR		svc_si_LibConvertSiCasIdToCasType(HUINT32 ulCasId, DxCasType_e *peCasType);
extern HERROR		svc_si_LibConvertSiStreamTypeToVideoCodec(HUINT32 ulStreamType, DxVideoCodec_e *peCodec);
extern HERROR		svc_si_LibConvertSiStreamTypeToAudioCodec(HUINT32 ulStreamType, DxAudioCodec_e *peCodec);
extern HERROR		svc_si_LibConvertSiAacTypeToAudioCodec(HUINT8 ucAacType, DxAudioCodec_e *peAudioCodec);
extern HERROR		svc_si_LibConvertSiAacProfileLevelToChannelNum(HUINT8 ucProfileAndLevel, HUINT8 *pucChanNum);
extern HERROR		svc_si_LibConvertSiAc3ComponentTypeToChannelNum(HUINT8 ucAc3CompType, HUINT8 *pucChanNum);
extern HERROR		svc_si_LibGetSvcAndVideoTypeFromDvbSvcType(HUINT32 ulActionId, HUINT32 ulDvbSvcType, DxSvcType_e *peSvcType, DxVideoType_e *peVideoType);



#endif	// ___INT_SVC_SI_LIB_H___
/* end of file */

