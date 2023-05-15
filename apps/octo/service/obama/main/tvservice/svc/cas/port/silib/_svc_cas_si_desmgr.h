/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  layer_module.h File Name을 적습니다.
	@brief	  file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include "_svc_cas_si_des.h"

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

/* Public Function 마다 필히 기재하도록 함 */
/**
  @brief     LAYER_MODULE_DoSomething 함수.

  Function Name:
  Function Description:
  Parameter:
  type: (Sync or Async)
  Return Value:
*/

HUINT8*	svc_cas_SiAllocMem(HUINT32 ulStrLen);
void	svc_cas_SiFreeMem(HUINT8 *pucStr);
HUINT8*	svc_cas_SiDupMem(const HUINT8 *pucMem, HUINT32 ulSize);

void	svc_cas_SiSetPrivateDataInfo(svcCas_PrivateDataInfo_t *pstPrivateDataInfo, HUINT8 *pucDesData, HUINT16 usPrivateDataLen);
void 	svc_cas_SiSetVideoStmDes(svcCas_VideoStmDes_t *pstVideoStmDes, HBOOL bMultiFrameRate, HUINT8 ucFrameRateCode, HBOOL bMPEG2, HBOOL bConstrainParam, HBOOL bStillPic);
void 	svc_cas_SiSetAudioStmDes(svcCas_AudioStmDes_t *pstAudioStmDes, HBOOL bFreeFormat, HUINT8 ucId, HUINT8 ucLayer, HUINT8 ucVarRateAudioIndicator);
void 	svc_cas_SiSetHierDes(svcCas_HierDes_t *pstHierDes, HUINT8 ucHierType, HUINT8 ucHierLayerIndex, HUINT8 ucHierEmbedLayerIndex, HUINT8 ucHierChannel);
void 	svc_cas_SiSetRegDes(svcCas_RegDes_t *pstRegDes, HUINT32 ulFormatIdent, svcCas_PrivateDataInfo_t *pstPrivateDataInfo);
void 	svc_cas_SiSetDataStmAlignDes(svcCas_DataStmAlignDes_t *pstDataStmAlignDes, HUINT8 ucAlignType);
void 	svc_cas_SiSetBackGridDes(svcCas_BackGridDes_t *pstBackGridDes, HUINT16 usHoriSize, HUINT16 usVertSize, HUINT8 ucAspectRatio);
void 	svc_cas_SiSetVideoWindowDes(svcCas_VideoWindowDes_t *pstVideoWindowDes, HUINT16 usHoriOff, HUINT16 usVertOff, HUINT8 ucWindowPriority);
void 	svc_cas_SiSetCaDes(svcCas_CaDes_t *pstCaDes, HUINT16 usCaSysId, HUINT16 usCaPid, svcCas_PrivateDataInfo_t *pstPrivateDataInfo);

void 	svc_cas_SiSetLangInfo(svcCas_LangInfo_t *pstLangInfo, HUINT8 *pszIso639LangCode, HUINT8 ucAudioType);
svcCas_LangInfo_t 	*svc_cas_SiGetPrevLangInfo(svcCas_Iso639LangDes_t *pstIso639LangDes);
void 	svc_cas_SiLinkLangInfoHead(svcCas_Iso639LangDes_t *pstIso639LangDes, svcCas_LangInfo_t *pstNewLangInfo);
void 	svc_cas_SiLinkLangInfoNode(svcCas_LangInfo_t *pstNewLangInfo, svcCas_LangInfo_t *pstNextLangInfo, svcCas_LangInfo_t *pstPrevLangInfo);

void 	svc_cas_SiSetSysClockDes(svcCas_SysClockDes_t *pstSysClockDes, HUINT8 ucExtClockRef, HUINT8 ucClockAccuInt, HUINT8 ucClockAccuExp);
void 	svc_cas_SiSetMultiplexBufUtilDes(svcCas_MultiplexBufUtilDes_t *pstMultiplexBufUtilDes, HBOOL bBoundValid, HUINT16 usLtwOffLowBound, HUINT16 usLtwOffUpBound);
void 	svc_cas_SiSetCopyrightDes(svcCas_CopyRightDes_t *pstCopyrightDes, HUINT32 ulCopyrightIdent, svcCas_PrivateDataInfo_t *pstPrivateDataInfo);
void 	svc_cas_SiSetMaxBitrateDes(svcCas_MaxBitrateDes_t *pstMaxBitrateDes, HUINT32 ulMaxBitrate);
void 	svc_cas_SiSetPrivateDataIndicatorDes(svcCas_PrivateDataIndicatorDes_t *pstPrivateDataIndicatorDes, HUINT32 ulPrivateDataIndicatorDes);
void 	svc_cas_SiSetSmoothBufDes(svcCas_SmoothBufDes_t *pstSmoothBufDes, HUINT32 ulLeakRate, HUINT32 ulSize);
void 	svc_cas_SiSetStdDes(svcCas_StdDes_t *pstStdDes, HBOOL bLeakValid);
void 	svc_cas_SiSetCarouselIdDesInfo(svcCas_CarouselIdDesInfo_t *pstCarouselIdDesInfo
	, HUINT8 ucModuleVersion, HUINT16 usModuleId,HUINT8 usBlockSize
	, HUINT32 ulModuleSize,HUINT8 ucCompressionMethod, HUINT32 ulOriginalSize
	, HUINT8 ucTimeout,HUINT8 aucObjectKeyData[]);
void 	svc_cas_SiSetCarouselIdDes(svcCas_CarouselIdDes_t *pstCarouselIdDes, HUINT32 ulCarouselId
	, HUINT8 ucFormatId, svcCas_CarouselIdDesInfo_t *pstCarouselIDesInfo);
void 	svc_cas_SiSetAssocTagDes(svcCas_AssocTagDes_t *pstAssocTagDes, HUINT16 usAssocTag
	, HUINT16 usUse, HUINT32 ulTransactionId, HUINT32 ulTimeout);
void 	svc_cas_SiSetDeferAssocTagDes(svcCas_DeferAssocTagDes_t *pstDeferAssocTagDes
	, HUINT8 ucAssocTagSize, HUINT16 *pusAssocTags, HUINT16 usTransportStreamId
	, HUINT16 usProgNum, HUINT16 usOriginalNetworkId);
void 	svc_cas_SiSetNetNameDes(svcCas_NetNameDes_t *pstNetNameDes, HUINT8 *pszNetName);
void 	svc_cas_SiSetServListInfo(svcCas_SvcListInfo_t *pstServListInfo, HUINT16 usServId, HUINT8 ucServType);
svcCas_SvcListInfo_t 	*svc_cas_SiGetPrevServListInfo(svcCas_SvcListDes_t *pstServListDes);
void 	svc_cas_SiLinkServListInfoHead(svcCas_SvcListDes_t *pstServListDes, svcCas_SvcListInfo_t *pstServListInfo);
void 	svc_cas_SiLinkServListInfoNode(svcCas_SvcListInfo_t *pstNewServListInfo, svcCas_SvcListInfo_t *pstNextServListInfo, svcCas_SvcListInfo_t *pstPrevServListInfo);
void 	svc_cas_SiSetSatDeliSystemDes(svcCas_SatDeliSystemDes_t *pstSatDeliSystemDes, HUINT32 ulFreq, HUINT16 usOrbitPos, HBOOL bWestEastFlag, HUINT8 ucPolar, HUINT8 ucModul, HUINT8 ucModType, HUINT32 ulSymbolRate, HUINT8 ucFECInner, HUINT8 ucTransSpec, HUINT8 ucRollOff);
void 	svc_cas_SiSetCabDeliSystemDes(svcCas_CabDeliSystemDes_t *pstCabDeliSystemDes, HUINT32 ulFreq, HUINT8 ucFECOuter, HUINT8 ucModul, HUINT32 ulSymbolRate, HUINT8 ucFECInner);
void 	svc_cas_SiSetBouqNameDes(svcCas_BouqNameDes_t *pstBouqNameDes, HUINT8 *pszBouqName);
void 	svc_cas_SiSetServDes(svcCas_SvcDes_t *pstServDes, HUINT8 ucServType, HUINT8 *pszProviderName, HUINT8 *pszServName);
void 	svc_cas_SiSetCountryAvailInfo(svcCas_CountryAvailInfo_t *pstCountryAvailInfo, HUINT8 *pszCountryAvailCode);
svcCas_CountryAvailInfo_t 	*svc_cas_SiGetPrevCountryAvailInfo(svcCas_CountryAvailDes_t *pstCountryAvailDes);
void 	svc_cas_SiLinkCountryAvailInfoHead(svcCas_CountryAvailDes_t *pstCountryAvailDes, svcCas_CountryAvailInfo_t *pstCountryAvailInfo);
void 	svc_cas_SiLinkCountryAvailInfoNode(svcCas_CountryAvailInfo_t *pstNewCountryAvailInfo, svcCas_CountryAvailInfo_t *pstNextCountryAvailInfo, svcCas_CountryAvailInfo_t *pstPrevCountryAvailInfo);
void 	svc_cas_SiSetNvodRefInfo(svcCas_NvodRefInfo_t *pstNvodRefInfo, HUINT16 usTsId
	, HUINT16 usOrgNetId, HUINT16 usSvcId);
svcCas_NvodRefInfo_t 	*svc_cas_SiGetPrevNvodRefInfo(svcCas_NvodRefDes_t *pstNvodRefDes);
void 	svc_cas_SiLinkNvodRefInfoHead(svcCas_NvodRefDes_t *pstNvodRefDes, svcCas_NvodRefInfo_t *pstNvodRefInfo);
void 	svc_cas_SiLinkNvodRefInfoNode(svcCas_NvodRefInfo_t *pstNewNvodRefInfo, svcCas_NvodRefInfo_t *pstNextNvodRefInfo, svcCas_NvodRefInfo_t *pstPrevNvodRefInfo);
void 	svc_cas_SiSetTimeShiftServDes(svcCas_TimeShiftSvcDes_t *pstTimeShiftServDes, HUINT16 usRefServId);
void 	svc_cas_SiSetShortEvtDes(svcCas_ShortEventDes_t *pstShortEvtDes, HUINT8 *pszIso639LangCode, HUINT8 *pszEvtName, HUINT8 *pszText);
void 	svc_cas_SiSetExtEvtInfo(svcCas_ExtEvtInfo_t *pstExtEvtInfo, HUINT8 *pszDescription, HUINT8 *pszItem);
svcCas_ExtEvtInfo_t 	*svc_cas_SiGetPrevExtEvtInfo(svcCas_ExtEvtDes_t *pstExtEvtDes);
void 	svc_cas_SiLinkExtEvtInfoHead(svcCas_ExtEvtDes_t *pstExtEvtDes, svcCas_ExtEvtInfo_t *pstExtEvtInfo);
void 	svc_cas_SiLinkExtEvtInfoNode(svcCas_ExtEvtInfo_t *pstNewExtEvtInfo, svcCas_ExtEvtInfo_t *pstNextExtEvtInfo, svcCas_ExtEvtInfo_t *pstPrevExtEvtInfo);
void 	svc_cas_SiSetExtEvtDes(svcCas_ExtEvtDes_t *pstExtEvtDes, HUINT8 ucDesNum, HUINT8 ucLastDesNum, HUINT8 *pszIso639LangCode, HUINT8 *pszChar);
void 	svc_cas_SiSetTimeShiftEvtDes(svcCas_TimeShiftEvtDes_t *pstTimeShiftEvtDes, HUINT16 usRefServId, HUINT16 usRefEvtId);
void 	svc_cas_SiSetCompDes(svcCas_CompDes_t *pstCompDes, HUINT8 ucStmContent, HUINT8 ucCompType, HUINT8 ucCompTag, HUINT8 *pszIso639LangCode, HUINT8 *pszText);
void 	svc_cas_SiSetStmIdentDes(svcCas_StmIdentDes_t *pstStmIdentDes, HUINT8 ucCompTag);
void 	svc_cas_SiSetCaIdentInfo(svcCas_CaIdentInfo_t *pstCaIdentInfo, HUINT16 usCaStmId);
svcCas_CaIdentInfo_t 	*svc_cas_SiGetPrevCaIdentInfo(svcCas_CaIdentDes_t *pstCaIdentDes);
void 	svc_cas_SiLinkCaIdentInfoHead(svcCas_CaIdentDes_t *pstCaIdentDes, svcCas_CaIdentInfo_t *pstCaIdentInfo);
void 	svc_cas_SiLinkCaIdentInfoNode(svcCas_CaIdentInfo_t *pstNewCaIdentInfo, svcCas_CaIdentInfo_t *pstNextCaIdentInfo, svcCas_CaIdentInfo_t *pstPrevCaIdentInfo);
void 	svc_cas_SiSetContentInfo(svcCas_ContentInfo_t *pstContentInfo, HUINT8 ucContentNibble, HUINT8 ucUserNibble);
svcCas_ContentInfo_t 	*svc_cas_SiGetPrevContentInfo(svcCas_ContentDes_t *pstContentDes);
void 	svc_cas_SiLinkContentInfoHead(svcCas_ContentDes_t *pstContentDes, svcCas_ContentInfo_t *pstContentInfo);
void 	svc_cas_SiLinkContentInfoNode(svcCas_ContentInfo_t *pstNewContentInfo, svcCas_ContentInfo_t *pstNextContentInfo, svcCas_ContentInfo_t *pstPrevContentInfo);
void 	svc_cas_SiSetParentInfo(svcCas_ParentInfo_t *pstParentInfo, HUINT8 *pszCountryCode, HUINT8 ucRate);
svcCas_ParentInfo_t 	*svc_cas_SiGetPrevParentInfo(svcCas_ParentDes_t *pstParentDes);
void 	svc_cas_SiLinkParentInfoHead(svcCas_ParentDes_t *pstParentDes, svcCas_ParentInfo_t *pstParentInfo);
void 	svc_cas_SiLinkParentInfoNode(svcCas_ParentInfo_t *pstNewParentInfo, svcCas_ParentInfo_t *pstNextParentInfo, svcCas_ParentInfo_t *pstPrevParentInfo);
void 	svc_cas_SiSetTeletextInfo(svcCas_TeletextInfo_t *pstTeletextInfo, HUINT8 *pszIso639LangCode, HUINT8 ucType, HUINT8 ucMagazineNum, HUINT8 ucPageNum);
svcCas_TeletextInfo_t 	*svc_cas_SiGetPrevTeletextInfo(svcCas_TeletextDes_t *pstTeletextDes);
void 	svc_cas_SiLinkTeletextInfoHead(svcCas_TeletextDes_t *pstTeletextDes, svcCas_TeletextInfo_t *pstTeletextInfo);
void 	svc_cas_SiLinkTeletextInfoNode(svcCas_TeletextInfo_t *pstNewTeletextInfo, svcCas_TeletextInfo_t *pstNextTeletextInfo, svcCas_TeletextInfo_t *pstPrevTeletextInfo);
void 	svc_cas_SiSetTelephoneDes(svcCas_TelephoneDes_t *pstTelephoneDes, HUINT8 ucForeignAvail, HUINT8 ucConnectionType, HUINT8 *pszCountryPrefix, HUINT8 *pszInternationalAreaCode, HUINT8 *pszOperatorCode, HUINT8 *pszNationalAreaCode, HUINT8 *pszCoreNumber);
void 	svc_cas_SiSetLocalTimeInfo(svcCas_LocalTimeInfo_t *pstLocalTimeInfo, HUINT8 *pszCountryCode, HUINT8 ucRegionId, HUINT8 ucLocalTimeOffPolar, HUINT8 *pucLocalTimeOff, UNIXTIME timeOfChange, HUINT8 *pucNextTimeOffset);
svcCas_LocalTimeInfo_t 	*svc_cas_SiGetPrevLocalTimeInfo(svcCas_LocalTimeDes_t *pstLocalTimeDes);
void 	svc_cas_SiLinkLocalTimeInfoHead(svcCas_LocalTimeDes_t *pstLocalTimeDes, svcCas_LocalTimeInfo_t *pstLocalTimeInfo);
void 	svc_cas_SiLinkLocalTimeInfoNode(svcCas_LocalTimeInfo_t *pstNewLocalTimeInfo, svcCas_LocalTimeInfo_t *pstNextLocalTimeInfo, svcCas_LocalTimeInfo_t *pstPrevLocalTimeInfo);
void 	svc_cas_SiSetSubTitleInfo(svcCas_SubTitleInfo_t *pstSubTitleInfo, HUINT8 *pszIso639LangCode, HUINT8 ucSubTitleType, HUINT16 usCompositionPageId, HUINT16 usAncillaryPageId);
svcCas_SubTitleInfo_t 	*svc_cas_SiGetPrevSubTitleInfo(svcCas_SubTitleDes_t *pstSubTitleDes);
void 	svc_cas_SiLinkSubTitleInfoHead(svcCas_SubTitleDes_t *pstSubTitleDes, svcCas_SubTitleInfo_t *pstSubTitleInfo);
void 	svc_cas_SiLinkSubTitleInfoNode(svcCas_SubTitleInfo_t *pstNewSubTitleInfo, svcCas_SubTitleInfo_t *pstNextSubTitleInfo, svcCas_SubTitleInfo_t *pstPrevSubTitleInfo);
void 	svc_cas_SiSetTerDeliSystemDes(svcCas_TerDeliSystemDes_t *pstTerDeliSystemDes, HUINT32 ulFreq
	, HUINT8 ucBandWidth, HUINT8 ucPriority, HUINT8 ucConstellation, HUINT8 ucHierInfo
	, HUINT8 ucCodeRateHPStream, HUINT8 ucCodeRateLPStream, HUINT8 ucGuardInterval
	, HUINT8 ucTransmissionMode, HBOOL bOtherFreqFlag);
void 	svc_cas_SiSetMultiNetNameInfo(svcCas_MultiNetNameInfo_t *pstMultiNetNameInfo
	, HUINT8 *pszIso639LangCode, HUINT8 *pszNetName);
svcCas_MultiNetNameInfo_t 	*svc_cas_SiGetPrevMultiNetNameInfo(svcCas_MultiNetNameDes_t *pstMultiNetNameDes);
void 	svc_cas_SiLinkMultiNetNameInfoHead(svcCas_MultiNetNameDes_t *pstMultiNetNameDes
	, svcCas_MultiNetNameInfo_t *pstMultiNetNameInfo);
void 	svc_cas_SiLinkMultiNetNameInfoNode(svcCas_MultiNetNameInfo_t *pstNewMultiNetNameInfo
	, svcCas_MultiNetNameInfo_t *pstNextMultiNetNameInfo
	, svcCas_MultiNetNameInfo_t *pstPrevMultiNetNameInfo);
void 	svc_cas_SiSetMultiBouqNameInfo(svcCas_MultiBouqNameInfo_t *pstMultiBouqNameInfo, HUINT8 *pszIso639LangCode, HUINT8 *pszBouqName);
svcCas_MultiBouqNameInfo_t 	*svc_cas_SiGetPrevMultiBouqNameInfo(svcCas_MultiBouqNameDes_t *pstMultiBouqNameDes);
void 	svc_cas_SiLinkMultiBouqNameInfoHead(svcCas_MultiBouqNameDes_t *pstMultiBouqNameDes, svcCas_MultiBouqNameInfo_t *pstMultiBouqNameInfo);
void 	svc_cas_SiLinkMultiBouqNameInfoNode(svcCas_MultiBouqNameInfo_t *pstNewMultiBouqNameInfo, svcCas_MultiBouqNameInfo_t *pstNextMultiBouqNameInfo, svcCas_MultiBouqNameInfo_t *pstPrevMultiBouqNameInfo);
void 	svc_cas_SiSetMultiServNameInfo(svcCas_MultiSvcNameInfo_t *pstMultiServNameInfo
	, HUINT8 *pszIso639LangCode, HUINT8 *pszServProviderName, HUINT8 *pszServName);
svcCas_MultiSvcNameInfo_t *svc_cas_SiGetPrevMultiServNameInfo(svcCas_MultiSvcNameDes_t *pstMultiServNameDes);
void 	svc_cas_SiLinkMultiServNameInfoHead(svcCas_MultiSvcNameDes_t *pstMultiServNameDes
	, svcCas_MultiSvcNameInfo_t *pstMultiServNameInfo);
void 	svc_cas_SiLinkMultiServNameInfoNode(svcCas_MultiSvcNameInfo_t *pstNewMultiServNameInfo
	, svcCas_MultiSvcNameInfo_t *pstNextMultiServNameInfo
	, svcCas_MultiSvcNameInfo_t *pstPrevMultiServNameInfo);
void 	svc_cas_SiSetMultiCompInfo(svcCas_MultiCompInfo_t *pstMultiCompInfo, HUINT8 *pszIso639LangCode, HUINT8 *pszTextDes);
svcCas_MultiCompInfo_t 	*svc_cas_SiGetPrevMultiCompInfo(svcCas_MultiCompDes_t *pstMultiCompDes);
void 	svc_cas_SiLinkMultiCompInfoHead(svcCas_MultiCompDes_t *pstMultiCompDes, svcCas_MultiCompInfo_t *pstMultiCompInfo);
void 	svc_cas_SiLinkMultiCompInfoNode(svcCas_MultiCompInfo_t *pstNewMultiCompInfo
	, svcCas_MultiCompInfo_t *pstNextMultiCompInfo, svcCas_MultiCompInfo_t *pstPrevMultiCompInfo);
void 	svc_cas_SiSetMultiCompDes(svcCas_MultiCompDes_t *pstMultiCompDes, HUINT8 ucCompTag);
void 	svc_cas_SiSetPrivateDataSpecDes(svcCas_PrivateDataSpecDes_t *pstPrivateDataSpecDes
	, HUINT32 ulPrivateDataSpec);
void 	svc_cas_SiSetServMoveDes(svcCas_SvcMoveDes_t *pstServMoveDes, HUINT16 usNewOrgNetId
	, HUINT16 usNewTsId, HUINT16 usNewServId);
void 	svc_cas_SiSetShortSmoothBufDes(svcCas_ShortSmoothBufDes_t *pstShortSmoothBufDes
	, HUINT8 ucSbSize, HUINT8 ucSbLeakRate, svcCas_PrivateDataInfo_t *pstPrivateDataInfo);
void 	svc_cas_SiSetDataBroadcastInfo(svcCas_DataCarouselInfo_t *pstDataBroadcastInfo
	, HUINT8 ucCarouselId, HUINT32 ulTransactionId, HUINT32 ulTimeOutValueDSI
	, HUINT32 ulTimeOutValueDII, HUINT32 ulLeakRate);
void 	svc_cas_SiSetObjectCarouselInfo(svcCas_ObjectCarouselInfo_t *pstObjCarouselInfo
	, HUINT8 ucCarouselId, HUINT32 ulTransactionId, HUINT32 ulTimeOutValueDSI
	, HUINT32 ulTimeOutValueDII, HUINT32 ulLeakRate
	, HUINT8 *pszIso639LangCode, HUINT8 *pszObjName);
void 	svc_cas_SiSetDataBroadcastDes(svcCas_DataBroadcastDes_t *pstDataBroadcastDes
	, HUINT16 usDataBroadcastId, HUINT8 ucCompTag, HUINT8 *pszSelector
	, HUINT8 *pszIso639LangCode, HUINT8 *pszText );

HUINT8 	*svc_cas_SiAllocDes(HINT32 lDesTag, HUINT8 ucDesLen);
void 	svc_cas_SiFreeDes(HUINT8 *pucDesPtr);

HUINT8 	*svc_cas_SiAllocDesInfo(HINT32 lDesTag, HUINT16 usSubTag);
void 	svc_cas_SiFreeDesInfo(HINT32 lDesTag, HUINT16 usSubTag, HUINT8 *pstDesInfoPtr);

svcCas_SubCellListInfo_t *svc_cas_SiAllocSubCellListInfo(void);

