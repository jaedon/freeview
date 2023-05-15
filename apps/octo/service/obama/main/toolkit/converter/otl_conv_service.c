
#include <dlib.h>
#include	<otl.h>
#include	<octo_common.h>
#include	<db_param.h>
#include	<svc_ch.h>

#define ___LOCAL_VARIABLES___


#define ___LOCAL_FUNCTIONS___
STATIC HERROR otl_conv_SvcSetDefaultAuth(DxService_t *pstDxSvc, const HCHAR *aucDefAuth)
{
	HERROR	hErr = ERR_FAIL;

	if (aucDefAuth)
	{
#if defined(CONFIG_OP_FREESAT)
		HLIB_STD_StrNCpySafe(pstDxSvc->ex.freesat.defaultAuth, aucDefAuth, DxDEFAULT_AUTHORITY_LEN);
		hErr = ERR_OK;
#elif defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		HLIB_STD_StrNCpySafe(pstDxSvc->ex.tva.szDefaultAuth, aucDefAuth, DxDEFAULT_AUTHORITY_LEN);
		hErr = ERR_OK;
#endif

	}

	return hErr;
}

STATIC HERROR otl_conv_SvcGetDefaultAuth(const DxService_t *pstDxSvc, HCHAR **paucDefAuth)
{
	HERROR		hErr = ERR_FAIL;

	if (paucDefAuth)
	{
#if defined(CONFIG_OP_FREESAT)
		*paucDefAuth = (HUINT8 *)pstDxSvc->ex.freesat.defaultAuth;
		hErr = ERR_OK;
#elif defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		*paucDefAuth = (HUINT8 *)pstDxSvc->ex.tva.szDefaultAuth;
		hErr = ERR_OK;
#endif
	}

	return hErr;
}

STATIC HERROR otl_conv_TsSetDefaultAuth(DxTransponder_t *pstDxTs, const HCHAR *aucDefAuth)
{
	HERROR	hErr = ERR_FAIL;

	if (aucDefAuth)
	{
#if defined(CONFIG_OP_FREESAT)
		HLIB_STD_StrNCpySafe(pstDxTs->ex.freesat.defaultAuth, aucDefAuth, DxDEFAULT_AUTHORITY_LEN);
		hErr = ERR_OK;
#elif defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		HLIB_STD_StrNCpySafe(pstDxTs->ex.tva.defaultAuth, aucDefAuth, DxDEFAULT_AUTHORITY_LEN);
		hErr = ERR_OK;
#endif
	}

	return hErr;
}

STATIC HERROR otl_conv_TsGetDefaultAuth(const DxTransponder_t *pstDxTs, HCHAR **paucDefAuth)
{
	HERROR		hErr = ERR_FAIL;

	if (paucDefAuth)
	{
#if defined(CONFIG_OP_FREESAT)
		*paucDefAuth = (HUINT8 *)pstDxTs->ex.freesat.defaultAuth;
		hErr = ERR_OK;
#elif defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		*paucDefAuth = (HUINT8 *)pstDxTs->ex.tva.defaultAuth;
		hErr = ERR_OK;
#endif
	}

	return hErr;
}

STATIC HERROR otl_conv_NetSetDefaultAuth(DxNetwork_t *pstDxNet, const HCHAR *aucDefAuth)
{
	HERROR	hErr = ERR_FAIL;

	if (aucDefAuth)
	{
#if defined(CONFIG_OP_FREESAT)
		HLIB_STD_StrNCpySafe(pstDxNet->ex.freesat.defaultAuth, aucDefAuth, DxDEFAULT_AUTHORITY_LEN);
		hErr = ERR_OK;
#elif defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		HLIB_STD_StrNCpySafe(pstDxNet->ex.tva.defaultAuth, aucDefAuth, DxDEFAULT_AUTHORITY_LEN);
		hErr = ERR_OK;
#endif
	}

	return hErr;
}

STATIC HERROR otl_conv_NetGetDefaultAuth(const DxNetwork_t *pstDxNet, HCHAR **paucDefAuth)
{
	HERROR		hErr = ERR_FAIL;

	if (paucDefAuth)
	{
#if defined(CONFIG_OP_FREESAT)
		*paucDefAuth = (HUINT8 *)pstDxNet->ex.freesat.defaultAuth;
		hErr = ERR_OK;
#elif defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		*paucDefAuth = (HUINT8 *)pstDxNet->ex.tva.defaultAuth;
		hErr = ERR_OK;
#endif
	}

	return hErr;
}

STATIC HERROR otl_conv_SvcSetFtaContentMgr(DxService_t *pstDxSvc, HUINT32	ulFTAContentMgr)
{
	HERROR		hErr = ERR_FAIL;

#if defined(CONFIG_OP_FREESAT)
	pstDxSvc->ex.freesat.contentMgr = ulFTAContentMgr;
	hErr = ERR_OK;
#elif defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	pstDxSvc->ex.tva.ulFTAContentMgr = ulFTAContentMgr;
	hErr = ERR_OK;
#endif

	return hErr;
}


STATIC HERROR otl_conv_SvcGetFtaContentMgr(const DxService_t *pstDxSvc, HUINT32	*pulFTAContentMgr)
{
	HERROR		hErr = ERR_FAIL;

#if defined(CONFIG_OP_FREESAT)
	*pulFTAContentMgr = pstDxSvc->ex.freesat.contentMgr ;
	hErr = ERR_OK;
#elif defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	*pulFTAContentMgr = pstDxSvc->ex.tva.ulFTAContentMgr ;
	hErr = ERR_OK;
#endif

	return hErr;
}

STATIC HERROR otl_conv_SvcSetGuidanceType(DxService_t *pstDxSvc, HUINT8 ucGuidenceType)
{
	HERROR		hErr = ERR_FAIL;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	pstDxSvc->ex.tva.ucGuidanceType = ucGuidenceType;
	hErr = ERR_OK;
#endif

	return hErr;
}


STATIC HERROR otl_conv_SvcGetGuidanceType(const DxService_t *pstDxSvc, HUINT8	*pucGuidanceMode)
{
	HERROR		hErr = ERR_FAIL;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	*pucGuidanceMode = pstDxSvc->ex.tva.ucGuidanceMode ;
	hErr = ERR_OK;
#endif

	return hErr;
}

STATIC HERROR otl_conv_SvcSetGuidanceMode(DxService_t *pstDxSvc, HUINT32	ucGuidanceMode)
{
	HERROR		hErr = ERR_FAIL;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	pstDxSvc->ex.tva.ucGuidanceMode= ucGuidanceMode;
	hErr = ERR_OK;
#endif

	return hErr;
}

STATIC HERROR otl_conv_SvcGetGuidanceMode(const DxService_t *pstDxSvc, HUINT8 *pucGuidanceMode)
{
	HERROR		hErr = ERR_FAIL;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	*pucGuidanceMode = pstDxSvc->ex.tva.ucGuidanceType;
	hErr = ERR_OK;
#endif

	return hErr;
}

STATIC HERROR otl_conv_SvcSetGuidanceStr(DxService_t *pstDxSvc, const HCHAR *aucGuidanceStr)
{
	HERROR		hErr = ERR_FAIL;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HLIB_STD_StrUtf8NCpy(pstDxSvc->ex.tva.szGuidanceStr , aucGuidanceStr, DxMAX_GUIDANCE_INFO_LEN);
	hErr = ERR_OK;
#endif

	return hErr;
}

STATIC HERROR otl_conv_SvcGetGuidanceStr(const DxService_t *pstDxSvc, HUINT8 **pauGuidanceStr)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pauGuidanceStr)
		return ERR_FAIL;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	*pauGuidanceStr = (HUINT8 *)pstDxSvc->ex.tva.szGuidanceStr;
	hErr = ERR_OK;
#endif

	return hErr;
}

STATIC HERROR otl_conv_SvcGetVirtualSvcId(const DxService_t *pstDxSvc, HUINT16	*pusVirtualSvcId)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pusVirtualSvcId)
		return ERR_FAIL;

#if defined(CONFIG_OP_SES)
	*pusVirtualSvcId = (HUINT16)pstDxSvc->ex.ses.usVirtualSvcId;
	hErr = ERR_OK;
#endif

	return hErr;
}

STATIC HERROR otl_conv_SvcGetNewSvc(const DxService_t *pstDxSvc, HBOOL *pbNewSvc)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pbNewSvc)
		return ERR_FAIL;

#if defined(CONFIG_OP_SES)
	*pbNewSvc = (HBOOL)pstDxSvc->ex.ses.bNewSvc;
	hErr = ERR_OK;
#endif

	return hErr;
}

STATIC HERROR otl_conv_SvcSetVirtualSvcId(DxService_t *pstDxSvc, HUINT16	usVirtualSvcId)
{
	HERROR		hErr = ERR_FAIL;

#if defined(CONFIG_OP_SES)
	pstDxSvc->ex.ses.usVirtualSvcId= usVirtualSvcId;
	hErr = ERR_OK;
#endif

	return hErr;
}

STATIC HERROR otl_conv_SvcSetNewSvc(DxService_t *pstDxSvc, HBOOL bNewSvc)
{
	HERROR		hErr = ERR_FAIL;

#if defined(CONFIG_OP_SES)
	pstDxSvc->ex.ses.bNewSvc= bNewSvc;
	hErr = ERR_OK;
#endif

	return hErr;
}

STATIC HERROR otl_conv_NetSetUriPrefix(DxNetwork_t *pstDxNet, const HCHAR *aucUriPrefix)
{
	HERROR	hErr = ERR_FAIL;

	if (aucUriPrefix)
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		HLIB_STD_StrNCpySafe(pstDxNet->ex.tva.UriPrefix, aucUriPrefix, DxMAX_URI_PREFIX_INFO_LEN);
		hErr = ERR_OK;
#endif
	}

	return hErr;
}

STATIC HERROR otl_conv_NetGetUriPrefix(const DxNetwork_t *pstDxNet, HCHAR **paucUriPrefix)
{
	HERROR		hErr = ERR_FAIL;

	if (paucUriPrefix)
	{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		*paucUriPrefix = (HUINT8 *)pstDxNet->ex.tva.UriPrefix;
		hErr = ERR_OK;
#endif
	}

	return hErr;
}


#define ___API_FUNCTIONS___
DxService_t *	OTL_CONV_SVC_Info2DxService (DxService_t *d, const DbSvc_Info_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DxService_t *)NULL;
	}
	HxSTD_MemSet(d, 0, sizeof(DxService_t));

	HxSTD_MemCopy(d, &s->stDxInfo, sizeof(DxService_t));
	return d;
}

DbSvc_Info_t *	OTL_CONV_DxService2SVC_Info(DbSvc_Info_t *d, const DxService_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DbSvc_Info_t *)NULL;
	}

	(void) DB_SVC_InitServiceInfo(d);

	HxSTD_MemCopy(&d->stDxInfo, s, sizeof(DxService_t));

	return d;
}


DxSatTuningInfo_t *	OTL_CONV_SVC_SatTuningInfo2DxSatTuningInfo (DxSatTuningInfo_t *d, const DbSvc_SatTuningInfo_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DxSatTuningInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DxSatTuningInfo_t));
	d->tuningInfo.ulFrequency	=  s->ulFrequency;
	d->tuningInfo.ulSymbolRate	=  s->ulSymbolRate;
	d->tuningInfo.ePolarization	=  s->ePolarization;
	d->tuningInfo.eFecCodeRate	=  s->eFecCodeRate;
	d->tuningInfo.eTransSpec	=  s->eTransSpec;
	d->tuningInfo.ePskMod		=  s->ePskMod;
	d->tuningInfo.ePilot		=  s->ePilot;
	d->tuningInfo.eRollOff		=  s->eRollOff;
	d->tuningInfo.antuid		=  s->ucAntId;

	HLIB_STD_StrNCpySafe(d->tuningInfo.satType, OTL_CONV_SVC_SatType2String(s->eSatType), DxANTINFO_NAME_LEN);

	//	antInfo.uid를 사용하는 부분이 필요한가???
	d->antInfo.uid = s->ucAntId;
	d->deliveryType = eDxDELIVERY_TYPE_SAT;

	return d;
}


DbSvc_SatTuningInfo_t *	OTL_CONV_DxSatTuningInfo2SVC_SatTuningInfo(DbSvc_SatTuningInfo_t *d, const DxSatTuningInfo_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DbSvc_SatTuningInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DbSvc_SatTuningInfo_t));
	d->ulFrequency			= s->tuningInfo.ulFrequency;
	d->ulSymbolRate			= s->tuningInfo.ulSymbolRate;
	d->ePolarization		= s->tuningInfo.ePolarization;
	d->eFecCodeRate			= s->tuningInfo.eFecCodeRate;
	d->eTransSpec			= s->tuningInfo.eTransSpec;
	d->ePskMod				= s->tuningInfo.ePskMod;
	d->ePilot				= s->tuningInfo.ePilot;
	d->eRollOff				= s->tuningInfo.eRollOff;
	d->eSatType				= OTL_CONV_SatString2SVC_SatType(s->tuningInfo.satType);

	d->ucAntId				= s->tuningInfo.antuid;

	//	antInfo.uid를 사용하는 부분이 필요한가???
#if 0//ndef	DAMA_NEW
	d->ucAntId = s->antInfo.uid;
#endif
	return d;
}


DxCabTuningInfo_t *	OTL_CONV_SVC_CabTuningInfo2DxCabTuningInfo (DxCabTuningInfo_t *d, const DbSvc_CabTuningInfo_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DxCabTuningInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DxCabTuningInfo_t));
	d->ulFrequency		= s->ulFreq;
	d->constellation	= s->ucCon;
	d->spectrum 		= s->ucSpec;
	d->ulSymbolRate 	= s->usSR;
	d->nNetId			= s->usNetworkId;
	d->deliveryType 	= eDxDELIVERY_TYPE_CAB;

	return d;
}

DbSvc_CabTuningInfo_t *	OTL_CONV_DxCabTuningInfo2SVC_CabTuningInfo(DbSvc_CabTuningInfo_t   *d, const DxCabTuningInfo_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DbSvc_CabTuningInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DbSvc_CabTuningInfo_t));
	d->ulFreq		= s->ulFrequency;
	d->ucCon		= s->constellation;
	d->ucSpec		= s->spectrum;
	d->usSR 		= s->ulSymbolRate;
	d->usNetworkId	= s->nNetId;

	return d;
}

DxTerTuningInfo_t *	OTL_CONV_SVC_TerTuningInfo2DxTerTuningInfo (DxTerTuningInfo_t *d, const DbSvc_TerTuningInfo_t*s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DxTerTuningInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DxTerTuningInfo_t));
	d->ulFrequency		= s->ulFreq;
	d->eBandWidth		= s->ucBandwidth;
	d->eConstellation	= s->ucConstellation;
	d->eHierachy		= s->ucHierachy;
	d->eStream			= s->ucStream;
	d->eCodeRate		= s->ucCodeRate;
	d->eGuardInterval	= s->ucGuardInterval;
	d->eTransMode		= s->ucTransMode;
	d->eOffset			= s->ucOffset;
#if defined(CONFIG_MW_CH_DVB_T2)
	d->eSystem						= s->ucSystem;
	d->stT2Param.extenedCarrier		= s->stT2param.ucExtenedCarrier;
	d->stT2Param.mixed				= s->stT2param.ucMixed;
	d->stT2Param.PAPRreduction		= s->stT2param.ucPAPRreduction;
	d->stT2Param.pilotPattern			= s->stT2param.ucPilotPattern;
	d->stT2Param.preambleFormat 		= s->stT2param.ucPreambleFormat;
	d->stT2Param.numPlpBlocks			= s->stT2param.ulNumPlpBlocks;
	d->stT2Param.ulPlpId				= s->stT2param.ulPlpId;
#endif
#if 0
	s->ucChanNum;
	s->ucLevel;
	s->ucQuality;
#endif
	d->deliveryType		= eDxDELIVERY_TYPE_TER;

	return d;
}

DbSvc_TerTuningInfo_t *	OTL_CONV_DxTerTuningInfo2SVC_TerTuningInfo(DbSvc_TerTuningInfo_t *d, const DxTerTuningInfo_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DbSvc_TerTuningInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DbSvc_TerTuningInfo_t));
	d->ulFreq 				= s->ulFrequency;
	d->ucBandwidth			= s->eBandWidth;
	d->ucConstellation	 	= s->eConstellation;
	d->ucHierachy			= s->eHierachy;
	d->ucStream				= s->eStream;
	d->ucCodeRate 			= s->eCodeRate;
	d->ucGuardInterval		= s->eGuardInterval;
	d->ucTransMode			= s->eTransMode;
	d->ucOffset				= s->eOffset;
#if defined(CONFIG_MW_CH_DVB_T2)
	d->ucSystem						= s->eSystem;
	d->stT2param.ucExtenedCarrier	= s->stT2Param.extenedCarrier;
	d->stT2param.ucMixed			= s->stT2Param.mixed;
	d->stT2param.ucPAPRreduction	= s->stT2Param.PAPRreduction;
	d->stT2param.ucPilotPattern		= s->stT2Param.pilotPattern;
	d->stT2param.ucPreambleFormat	= s->stT2Param.preambleFormat;
	d->stT2param.ulNumPlpBlocks		= s->stT2Param.numPlpBlocks;
	d->stT2param.ulPlpId			= s->stT2Param.ulPlpId;
#endif

	return d;
}

DxTransponder_t *	OTL_CONV_SVC_TsInfo2DxTransponder (DxTransponder_t *d,  const DbSvc_TsInfo_t *s)
{
	HERROR	 hErr;
	HUINT8	*aucDefAuth = NULL;

	if ((d == NULL) || (s == NULL))
	{
		return (DxTransponder_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DxTransponder_t));
	d->uid 		= s->usTsIdx;
	d->netuid 	= s->usNetIdx;
	d->tsid		= s->usTsId;
	d->onid 	= s->usOnId;
	d->tunerid  = s->ucTunerId;

	d->deliType = s->eDeliType;

	switch(s->eDeliType)
	{
	case eDxDELIVERY_TYPE_SAT:		OTL_CONV_SVC_SatTuningInfo2DxSatTuningInfo(&(d->tuningParam.sat), &(s->stTuningParam.sat));
								break;
	case eDxDELIVERY_TYPE_CAB:		OTL_CONV_SVC_CabTuningInfo2DxCabTuningInfo(&(d->tuningParam.cab), &(s->stTuningParam.cab));
								break;
	case eDxDELIVERY_TYPE_TER:		OTL_CONV_SVC_TerTuningInfo2DxTerTuningInfo(&(d->tuningParam.ter), &(s->stTuningParam.ter));
								break;
	default:					return NULL;

	}

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	d->ex.tva.bFreqListed = s->utExt.stTvAnyTime.bFreqListed;
#endif

	hErr = DB_SVC_GetTsDefaultAuthority((const DbSvc_TsInfo_t *)s, &aucDefAuth);
	if (ERR_OK == hErr && NULL != aucDefAuth)
	{
		otl_conv_TsSetDefaultAuth(d, (const HCHAR *)aucDefAuth);
	}

#ifdef	CONFIG_OP_PVR_BASED_ON_TVANYTIME
	{
		HUINT32 i;

		DxTvAnyTimeTs_t	*tvAnyTime = &(d->ex.tva);

		for(i=0;i<DxMAX_T_REGION_NUM;i++)
		{
			HLIB_STD_StrUtf8NCpy(tvAnyTime->stRegionInfo[i].szCountryCode, s->utExt.stTvAnyTime.stRegionInfo[i].szCountryCode, 4);
			tvAnyTime->stRegionInfo[i].ucRegionDepth = s->utExt.stTvAnyTime.stRegionInfo[i].ucRegionDepth;
			tvAnyTime->stRegionInfo[i].ulRegionCode = s->utExt.stTvAnyTime.stRegionInfo[i].ulRegionCode;
		}
	}
#endif

	return d;
}

DbSvc_TsInfo_t * OTL_CONV_DxTransponder2SVC_TsInfo(DbSvc_TsInfo_t *d, const DxTransponder_t *s)
{
	HUINT8	*aucDefAuth = NULL;
	HERROR	 hErr;

	if ((d == NULL) || (s == NULL))
	{
		return (DbSvc_TsInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DbSvc_TsInfo_t));
	d->usTsIdx		 =  s->uid;
	d->usNetIdx 	 =	s->netuid;
	d->usTsId 		 =	s->tsid;
	d->usOnId 		 =	s->onid;
	d->ucTunerId 	 =	s->tunerid;

	d->eDeliType = s->deliType;

	switch(s->deliType)
	{
	case eDxDELIVERY_TYPE_SAT:	OTL_CONV_DxSatTuningInfo2SVC_SatTuningInfo(&(d->stTuningParam.sat), &(s->tuningParam.sat));
								break;
	case eDxDELIVERY_TYPE_CAB:	OTL_CONV_DxCabTuningInfo2SVC_CabTuningInfo(&(d->stTuningParam.cab), &(s->tuningParam.cab));
								break;
	case eDxDELIVERY_TYPE_TER:	OTL_CONV_DxTerTuningInfo2SVC_TerTuningInfo(&(d->stTuningParam.ter), &(s->tuningParam.ter));
								break;
	default:					return NULL;
	}

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	 d->utExt.stTvAnyTime.bFreqListed = s->ex.tva.bFreqListed;
#endif

	hErr = otl_conv_TsGetDefaultAuth(s, (HCHAR **)&aucDefAuth);
	if (ERR_OK == hErr && NULL != aucDefAuth)
	{
		DB_SVC_SetTsDefaultAuthority((DbSvc_TsInfo_t *)d, (const HUINT8 *)aucDefAuth);
	}
#ifdef	CONFIG_OP_PVR_BASED_ON_TVANYTIME
	{
		HUINT32 i;
		const DxTvAnyTimeTs_t *tvAnyTime = &(s->ex.tva);

		for(i=0;i<DxMAX_T_REGION_NUM;i++)
		{
			HLIB_STD_StrUtf8NCpy(d->utExt.stTvAnyTime.stRegionInfo[i].szCountryCode,tvAnyTime->stRegionInfo[i].szCountryCode,4);
			d->utExt.stTvAnyTime.stRegionInfo[i].ucRegionDepth = tvAnyTime->stRegionInfo[i].ucRegionDepth;
			d->utExt.stTvAnyTime.stRegionInfo[i].ulRegionCode = tvAnyTime->stRegionInfo[i].ulRegionCode;
		}
	}
#endif

	return d;
}

HERROR OTL_CONV_DxTuneParam2DbTuningInfo(const DxTuneParam_t *pstDxTuneParam, DbSvc_TuningInfo_t *pstSvcTuningInfo)
{
	if((NULL == pstDxTuneParam) || (NULL == pstSvcTuningInfo))
	{
		HxLOG_Error("pstSvcTuningInfo:0x%X, pstDxTuneParam:0x%X!!!\n",
					pstSvcTuningInfo, pstDxTuneParam);
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstSvcTuningInfo, 0, sizeof(DbSvc_TuningInfo_t));

	switch(pstDxTuneParam->eDeliType)
	{
		case eDxDELIVERY_TYPE_SAT:
		{
			(void)OTL_CONV_DxSatTuningInfo2SVC_SatTuningInfo((DbSvc_SatTuningInfo_t *)pstSvcTuningInfo, (const DxSatTuningInfo_t *)pstDxTuneParam);
			break;
		}

		case eDxDELIVERY_TYPE_TER:
		{
			(void)OTL_CONV_DxTerTuningInfo2SVC_TerTuningInfo((DbSvc_TerTuningInfo_t *)pstSvcTuningInfo, (const DxTerTuningInfo_t *)pstDxTuneParam);
			break;
		}

		case eDxDELIVERY_TYPE_CAB:
		{
			(void)OTL_CONV_DxCabTuningInfo2SVC_CabTuningInfo((DbSvc_CabTuningInfo_t *)pstSvcTuningInfo, (const DxCabTuningInfo_t *)pstDxTuneParam);
			break;
		}

		default:
			/* 현재 지원하지 않는 delivery system */
			HxLOG_Error("\n\n\tpstDxTuneParam->deliveryType:0x%X unknown!!!\n\n", pstDxTuneParam->eDeliType);
			return ERR_FAIL;
	}

	return ERR_OK;
}

HERROR OTL_CONV_DbTuningInfo2DxTuneParam(const DbSvc_TsInfo_t *pstSvcTsInfo, DxTuneParam_t *pstChTuningParam)
{
	if((NULL == pstSvcTsInfo) || (NULL == pstChTuningParam))
	{
		HxLOG_Error("pstSvcTsInfo:0x%X, pstChTuningParam:0x%X!!!\n",pstSvcTsInfo,pstChTuningParam);
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstChTuningParam, 0, sizeof(DxTuneParam_t));

	switch(pstSvcTsInfo->eDeliType)
	{
		HERROR	hErr;

		case eDxDELIVERY_TYPE_SAT:
		{
			DbSvc_AntInfo_t 		 stAntInfo;
			DbSvc_SatTuningInfo_t	*pstSvcTuningInfoSat = (DbSvc_SatTuningInfo_t *)&(pstSvcTsInfo->stTuningParam.sat);
			DxSatTuningInfo_t		*pstChTuningInfoSat = (DxSatTuningInfo_t *)&(pstChTuningParam->sat);

			hErr = DB_SVC_GetAntInfo(pstSvcTuningInfoSat->ucAntId, &stAntInfo);
			if(ERR_OK != hErr)
				HxLOG_Critical("DB_SVC_GetAntInfo ret:0x%X(AntId:%d)\n", hErr, pstSvcTuningInfoSat->ucAntId);

			pstChTuningInfoSat->ulTunerGroupId	= (HUINT32)pstSvcTsInfo->ucTunerId;
			pstChTuningInfoSat->ulRfInputId		= (HUINT32)-1;
			pstChTuningInfoSat->antennaType		= stAntInfo.eAntennaType;

			(void)OTL_CONV_SVC_SatTuningInfo2DxSatTuningInfo(pstChTuningInfoSat, (const DbSvc_SatTuningInfo_t *)pstSvcTuningInfoSat);
			(void)OTL_CONV_DbAntInfo2DxAntInfo(&pstChTuningInfoSat->antInfo, (const DbSvc_AntInfo_t *)&stAntInfo);
			pstChTuningInfoSat->antennaType		= pstChTuningInfoSat->antInfo.antType;
			break;
		}

		case eDxDELIVERY_TYPE_TER:
		{
			DxTerTuningInfo_t		*pstChTuningInfoTer = (DxTerTuningInfo_t *)&pstChTuningParam->ter;

			(void)OTL_CONV_SVC_TerTuningInfo2DxTerTuningInfo(pstChTuningInfoTer, (const DbSvc_TerTuningInfo_t *)&pstSvcTsInfo->stTuningParam.ter);
			pstChTuningInfoTer->ulTunerGroupId	= RxRSCID_NULL; /* 케이블은 DB parameter에서는 tuner ID를 지정하지 않는다. */
			break;
		}

		case eDxDELIVERY_TYPE_CAB:
		{
			DxCabTuningInfo_t		*pstChTuningInfoCab = (DxCabTuningInfo_t *)&pstChTuningParam->cab;

			(void)OTL_CONV_SVC_CabTuningInfo2DxCabTuningInfo(pstChTuningInfoCab, (const DbSvc_CabTuningInfo_t *)&pstSvcTsInfo->stTuningParam.cab);
			pstChTuningInfoCab->ulTunerGroupId	= RxRSCID_NULL; /* 케이블은 DB parameter에서는 tuner ID를 지정하지 않는다. */
			break;
		}

		default:
			/* 현재 지원하지 않는 delivery system */
			HxLOG_Error("\n\n\tpstSvcTsInfo->eDeliType:0x%X unknown!!!\n\n",pstSvcTsInfo->eDeliType);
			return ERR_FAIL;
	}

	pstChTuningParam->eDeliType = pstSvcTsInfo->eDeliType;

	return ERR_OK;
}


DxProvider_t *	OTL_CONV_SVC_PrvInfo2DxProvider (DxProvider_t *d,  const DbSvc_PrvInfo_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DxProvider_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DxProvider_t));
	d->uid =  s->usPrvIdx;
	HLIB_STD_StrUtf8NCpy(d->name, s->szPrvName, DxNAME_LEN);
	return d;
}

DbSvc_PrvInfo_t * OTL_CONV_DxProvider2SVC_PrvInfo(DbSvc_PrvInfo_t *d, const DxProvider_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DbSvc_PrvInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DbSvc_PrvInfo_t));
	d->usPrvIdx = s->uid;

	HLIB_STD_StrUtf8NCpy(d->szPrvName, s->name, DxMAX_PRV_NAME_LEN);
	return d;
}


DxGroup_t *	OTL_CONV_SVC_GroupInfo2DxGroup (DxGroup_t *d,const DbSvc_GroupInfo_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DxGroup_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DxGroup_t));
	d->uid = s->usGroupIdx;
	d->id  = s->ulGroupId;

	HLIB_STD_StrUtf8NCpy(d->name, s->szGroupName, DxNAME_LEN);

#if defined(CONFIG_OP_FREESAT)
	d->ex.freesat.ucGroupType				= s->utExt.stFreeSat.ucGroupType;
	d->ex.freesat.ucNondestructiveTuneFlag	= s->utExt.stFreeSat.ucNondestructiveTuneFlag;
	d->ex.freesat.ucReturnChannelAccessFlag	= s->utExt.stFreeSat.ucReturnChannelAccessFlag;
	HLIB_STD_StrNCpySafe(d->ex.freesat.szIso639LangCode, s->utExt.stFreeSat.szIso639LangCode, DxFSAT_LANG_CODE_LEN+1);
#if defined(CONFIG_OP_FREESAT_GEN2)
	d->ex.freesat.ucG2ExtensionFlag			= s->utExt.stFreeSat.ucG2ExtensionFlag;
	d->ex.freesat.ucDefaultVisableFlag		= s->utExt.stFreeSat.ucDefaultVisableFlag;
	d->ex.freesat.ucAdultChannelsGroupFlag	= s->utExt.stFreeSat.ucAdultChannelsGroupFlag;
	d->ex.freesat.ucIpChannelsGroupFlag		= s->utExt.stFreeSat.ucIpChannelsGroupFlag;
	d->ex.freesat.ucCaChannelsGroupFlag		= s->utExt.stFreeSat.ucCaChannelsGroupFlag;
#endif
#endif

	return d;
}

DbSvc_GroupInfo_t*	OTL_CONV_DxGroup2SVC_GroupInfo(DbSvc_GroupInfo_t *d, const DxGroup_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DbSvc_GroupInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DbSvc_GroupInfo_t));
	d->usGroupIdx = s->uid;
	d->ulGroupId  = s->id;

	HLIB_STD_StrNCpySafe(d->szGroupName, s->name, DxMAX_GROUP_NAME_LEN); //64 ? 36?

#if defined(CONFIG_OP_FREESAT)
	d->utExt.stFreeSat.ucGroupType = s->ex.freesat.ucGroupType;

	d->utExt.stFreeSat.ucNondestructiveTuneFlag = s->ex.freesat.ucNondestructiveTuneFlag;
	d->utExt.stFreeSat.ucReturnChannelAccessFlag = s->ex.freesat.ucReturnChannelAccessFlag;

	HLIB_STD_StrNCpySafe(d->utExt.stFreeSat.szIso639LangCode, s->ex.freesat.szIso639LangCode, DxFSAT_LANG_CODE_LEN +1);
#if defined(CONFIG_OP_FREESAT_GEN2)
	d->utExt.stFreeSat.ucG2ExtensionFlag = s->ex.freesat.ucG2ExtensionFlag;
	d->utExt.stFreeSat.ucDefaultVisableFlag = s->ex.freesat.ucDefaultVisableFlag;
	d->utExt.stFreeSat.ucAdultChannelsGroupFlag = s->ex.freesat.ucAdultChannelsGroupFlag;
	d->utExt.stFreeSat.ucIpChannelsGroupFlag = s->ex.freesat.ucIpChannelsGroupFlag;
	d->utExt.stFreeSat.ucCaChannelsGroupFlag = s->ex.freesat.ucCaChannelsGroupFlag;
#endif
#endif

#if 0
	JSON_INT(json, HUINT8, data->ucChanNum,	"chnum");
	JSON_INT(json, HUINT8, data->ucLevel,	"level");
	JSON_INT(json, HUINT8, data->ucQuality,	"quality");
#endif

	return d;

}



DxBouquet_t *	OTL_CONV_SVC_BouqInfo2DxBouquet (DxBouquet_t *d,  const DbSvc_BouqInfo_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DxBouquet_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DxBouquet_t));
	d->id  = s->usBouqId;
	d->uid = s->usBouqIdx;

	HLIB_STD_StrUtf8NCpy(d->name, s->szBouqName, DxNAME_LEN);

#if defined(CONFIG_DB_SVC_SVCBOUQUET_MULTILANG_NAME)
	{
		HUINT32 i;
		for (i = 0; i < DxMAX_MULTI_BOUQ_NAME_NUM; i++)
		{
			DxMultiBouqName_t		*pstSrc = (DxMultiBouqName_t *)&s->astMultiBouqName[i];
			DxMultiBouqName_t		*pstDst = (DxMultiBouqName_t *)&d->ex.astMultiBouqName[i];

			HLIB_STD_StrUtf8NCpy(pstDst->szLangCode, pstSrc->szLangCode, DxMAX_BOUQ_LANG_LEN);
			HLIB_STD_StrUtf8NCpy(pstDst->szBouqName, pstSrc->szBouqName, DxMAX_BOUQ_NAME_LEN);
		}
	}
#endif

	return d;
}

DbSvc_BouqInfo_t* OTL_CONV_DxBouquet2SVC_BouqInfo(DbSvc_BouqInfo_t *d, const DxBouquet_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DbSvc_BouqInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DbSvc_BouqInfo_t));
	d->usBouqId  = s->id;
	d->usBouqIdx = s->uid;

	HLIB_STD_StrUtf8NCpy(d->szBouqName, s->name, DxMAX_BOUQ_NAME_LEN);

#if defined(CONFIG_DB_SVC_SVCBOUQUET_MULTILANG_NAME)
	{
		HUINT32 i;
		for (i = 0; i < DxMAX_MULTI_BOUQ_NAME_NUM; i++)
		{
			DxMultiBouqName_t		*pstSrc = (DxMultiBouqName_t *)&s->ex.astMultiBouqName[i];
			DxMultiBouqName_t		*pstDst = (DxMultiBouqName_t *)&d->astMultiBouqName[i];

			HLIB_STD_StrUtf8NCpy(pstDst->szLangCode, pstSrc->szLangCode, DxMAX_BOUQ_LANG_LEN);
			HLIB_STD_StrUtf8NCpy(pstDst->szBouqName, pstSrc->szBouqName, DxMAX_BOUQ_NAME_LEN);
		}
	}
#endif

	return d;
}


DxNetwork_t *	OTL_CONV_SVC_NetInfo2DxNetwork (DxNetwork_t *d, const DbSvc_NetInfo_t *s)
{
	HUINT8	*aucDefAuth = NULL;
	HERROR	 hErr;

	if ((d == NULL) || (s == NULL))
	{
		return (DxNetwork_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DxNetwork_t));
	d->uid  = s->usNetIdx;
	d->onid = s->usNetId;
	HLIB_STD_StrUtf8NCpy(d->name, s->szNetName, DxNAME_LEN);

	d->deliType =  s->eOrgDeliType;

	hErr = DB_SVC_GetNetworkDefaultAuthority((const DbSvc_NetInfo_t *)s, &aucDefAuth);
	if (ERR_OK == hErr && NULL != aucDefAuth)
	{
		otl_conv_NetSetDefaultAuth(d, (const HCHAR *)aucDefAuth);
	}
#ifdef	CONFIG_OP_PVR_BASED_ON_TVANYTIME
	{
		HUINT32 i;

		DxTvAnyTimeNet_t *tvAnyTime = &(d->ex.tva);

		for(i=0;i<DxMAX_T_REGION_NUM;i++)
		{
			HLIB_STD_StrUtf8NCpy(tvAnyTime->stRegionInfo[i].szCountryCode, s->utExt.stTvAnyTime.stRegionInfo[i].szCountryCode, 4);
			tvAnyTime->stRegionInfo[i].ucRegionDepth = s->utExt.stTvAnyTime.stRegionInfo[i].ucRegionDepth;
			tvAnyTime->stRegionInfo[i].ulRegionCode = s->utExt.stTvAnyTime.stRegionInfo[i].ulRegionCode;
		}
	}
	{
		HUINT8	*aucUriPrefix = NULL;

		hErr = DB_SVC_GetNetworkUriPrefix((const DbSvc_NetInfo_t *)s, &aucUriPrefix);
		if (ERR_OK == hErr && NULL != aucUriPrefix)
		{
			otl_conv_NetSetUriPrefix(d, (const HCHAR *)aucUriPrefix);
		}
	}
#endif


	return d;
}

DbSvc_NetInfo_t*	OTL_CONV_DxNetwork2SVC_NetInfo(DbSvc_NetInfo_t *d, const DxNetwork_t *s)
{
	HUINT8	*aucDefAuth;
	HERROR	 hErr;

	if ((d == NULL) || (s == NULL))
	{
		return (DbSvc_NetInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DbSvc_NetInfo_t));
	d->usNetIdx = s->uid;
	d->usNetId  = s->onid;

	HLIB_STD_StrUtf8NCpy(d->szNetName, s->name, DxMAX_NET_NAME_LEN);

	d->eOrgDeliType = s->deliType;

	aucDefAuth = NULL;
	hErr = otl_conv_NetGetDefaultAuth((const DxNetwork_t *)d, (HCHAR **)&aucDefAuth);
	if (ERR_OK == hErr && NULL != aucDefAuth)
	{
		DB_SVC_SetNetworkDefaultAuthority(d, (const HUINT8 *)aucDefAuth);
	}
#ifdef	CONFIG_OP_PVR_BASED_ON_TVANYTIME
	{
		HUINT32 i;
		const DxTvAnyTimeNet_t *tvAnyTime = &(s->ex.tva);

		for(i=0;i<DxMAX_T_REGION_NUM;i++)
		{
			HLIB_STD_StrUtf8NCpy(d->utExt.stTvAnyTime.stRegionInfo[i].szCountryCode,tvAnyTime->stRegionInfo[i].szCountryCode,4);
			d->utExt.stTvAnyTime.stRegionInfo[i].ucRegionDepth = tvAnyTime->stRegionInfo[i].ucRegionDepth;
			d->utExt.stTvAnyTime.stRegionInfo[i].ulRegionCode = tvAnyTime->stRegionInfo[i].ulRegionCode;
		}
	}

	{
		HUINT8	*aucUriPrefix;
		aucUriPrefix = NULL;
		hErr = otl_conv_NetGetUriPrefix((const DxNetwork_t *)d, (HCHAR **)&aucUriPrefix);
		if (ERR_OK == hErr && NULL != aucUriPrefix)
		{
			DB_SVC_SetNetworkUriPrefix(d, (const HUINT8 *)aucUriPrefix);
		}
	}

#endif




	return d;
}

DxLnbAntenna_t *OTL_CONV_DbLnbAntInfo2DxLnbAntenna (DxLnbAntenna_t *d, const DbSvc_LnbAntInfo_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DxLnbAntenna_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DxLnbAntenna_t));
	d->ulLnbFreq		= s->ulLnbFreq;
	d->lnbVoltage		= s->eLnbVoltage;

	return d;
}

DbSvc_LnbAntInfo_t *OTL_CONV_DxLnbAntenna2DbLnbAntInfo (DbSvc_LnbAntInfo_t *d, const DxLnbAntenna_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DbSvc_LnbAntInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DbSvc_LnbAntInfo_t));
	d->ulLnbFreq		= s->ulLnbFreq;
	d->eLnbVoltage		= s->lnbVoltage;

	return d;
}

DxDiseqcAntenna_t *OTL_CONV_DbDiseqcAntInfo2DxDiseqcAntenna (DxDiseqcAntenna_t *d, const DbSvc_DiseqcAntInfo_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DxDiseqcAntenna_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DxDiseqcAntenna_t));
	d->ulLnbFreq		= s->ulLnbFreq;
	d->lnbVoltage		= s->eLnbVoltage;
	d->b22kTone			= s->b22kTone;
	d->diseqcVersion	= s->eDiseqcVer;
	d->diseqcInput		= s->eDiseqcInput;
	d->toneBurst		= s->eToneBurst;
	d->ucMotorPosition	= s->ucMotorPosition;
	d->sSatLonggitude	= s->sSatLonggitude;

	return d;
}

DbSvc_DiseqcAntInfo_t *OTL_CONV_DxDiseqcAntenna2DbDiseqcAntInfo (DbSvc_DiseqcAntInfo_t *d, const DxDiseqcAntenna_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DbSvc_DiseqcAntInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DbSvc_DiseqcAntInfo_t));
	d->ulLnbFreq		= s->ulLnbFreq;
	d->eLnbVoltage		= s->lnbVoltage;
	d->b22kTone			= s->b22kTone;
	d->eDiseqcVer		= s->diseqcVersion;
	d->eDiseqcInput		= s->diseqcInput;
	d->eToneBurst		= s->toneBurst;
	d->ucMotorPosition	= s->ucMotorPosition;
	d->sSatLonggitude	= s->sSatLonggitude;

	return d;
}

DxScdAntenna_t *OTL_CONV_DbScdAntInfo2DxScdAntenna (DxScdAntenna_t *d, const DbSvc_ScdAntInfo_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DxScdAntenna_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DxScdAntenna_t));
	d->ulLnbFreq			= s->ulLnbFreq;
	d->lnbVoltage			= s->eLnbVoltage;
	d->b22kTone				= s->b22kTone;
	d->scdInput				= s->eScdInput;
	d->ulScdUserBandNum		= s->ulScdUserBandNum;
	d->ulScdUserBandFreq	= s->ulScdUserBandFreq;
	d->ulScdUserBandNum1	= s->ulScdUserBandNum1;
	d->ulScdUserBandFreq1	= s->ulScdUserBandFreq1;
	d->ulScdUserBandNum2	= s->ulScdUserBandNum2;
	d->ulScdUserBandFreq2	= s->ulScdUserBandFreq2;
	d->ulScdUserBandNum3	= s->ulScdUserBandNum3;
	d->ulScdUserBandFreq3	= s->ulScdUserBandFreq3;

	return d;
}

DbSvc_ScdAntInfo_t *OTL_CONV_DxScdAntenna2DbScdAntInfo (DbSvc_ScdAntInfo_t *d, const DxScdAntenna_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DbSvc_ScdAntInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DbSvc_ScdAntInfo_t));
	d->ulLnbFreq			= s->ulLnbFreq;
	d->eLnbVoltage			= s->lnbVoltage;
	d->b22kTone				= s->b22kTone;
	d->eScdInput			= s->scdInput;
	d->ulScdUserBandNum		= s->ulScdUserBandNum;
	d->ulScdUserBandFreq	= s->ulScdUserBandFreq;
	d->ulScdUserBandNum1	= s->ulScdUserBandNum1;
	d->ulScdUserBandFreq1	= s->ulScdUserBandFreq1;
	d->ulScdUserBandNum2	= s->ulScdUserBandNum2;
	d->ulScdUserBandFreq2	= s->ulScdUserBandFreq2;
	d->ulScdUserBandNum3	= s->ulScdUserBandNum3;
	d->ulScdUserBandFreq3	= s->ulScdUserBandFreq3;

	return d;
}

DxAntInfo_t *OTL_CONV_DbAntInfo2DxAntInfo (DxAntInfo_t *d, const DbSvc_AntInfo_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DxAntInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DxAntInfo_t));
	HLIB_STD_StrNCpySafe (d->satType, OTL_CONV_SVC_SatType2String(s->eSatType), DxANTINFO_NAME_LEN);
	HLIB_STD_StrNCpySafe (d->satName, s->szName, DxANTINFO_NAME_LEN);

	d->antType = s->eAntennaType;

	switch (s->eAntennaType)
	{
	case eDxANT_TYPE_LNB_ONLY:
		OTL_CONV_DbLnbAntInfo2DxLnbAntenna (&(d->info.lnb), &(s->utAntInfo.stLnbAnt));
		break;

	case eDxANT_TYPE_DISEQC:
		OTL_CONV_DbDiseqcAntInfo2DxDiseqcAntenna (&(d->info.diseqc), &(s->utAntInfo.stDiseqcAnt));
		break;

	case eDxANT_TYPE_SCD:
		OTL_CONV_DbScdAntInfo2DxScdAntenna (&(d->info.scd), &(s->utAntInfo.stScdAnt));
		break;

	default:
		return NULL;
	}

	return d;
}

DbSvc_AntInfo_t *OTL_CONV_DxAntInfo2DbAntInfo (DbSvc_AntInfo_t *d, const DxAntInfo_t *s)
{
	if ((d == NULL) || (s == NULL))
	{
		return (DbSvc_AntInfo_t *)NULL;
	}

	HxSTD_MemSet(d, 0, sizeof(DbSvc_AntInfo_t));
	d->eSatType = OTL_CONV_SatString2SVC_SatType ((const HCHAR *)s->satType);
	HLIB_STD_StrNCpySafe (d->szName, s->satName, DxANTINFO_NAME_LEN);
	d->eAntennaType = s->antType;

	switch (s->antType)
	{
	case eDxANT_TYPE_LNB_ONLY:
		OTL_CONV_DxLnbAntenna2DbLnbAntInfo (&(d->utAntInfo.stLnbAnt), &(s->info.lnb));
		break;

	case eDxANT_TYPE_DISEQC:
		OTL_CONV_DxDiseqcAntenna2DbDiseqcAntInfo (&(d->utAntInfo.stDiseqcAnt), &(s->info.diseqc));
		break;

	case eDxANT_TYPE_SCD:
		OTL_CONV_DxScdAntenna2DbScdAntInfo (&(d->utAntInfo.stScdAnt), &(s->info.scd));
		break;

	default:
		return NULL;
	}

	return d;
}

/* end of file */
