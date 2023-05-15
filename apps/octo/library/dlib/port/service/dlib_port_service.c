
#include <hlib.h>
#include "_dlib_port_service.h"

typedef	struct
{
	DxField_t		field;
	HBOOL			(* getData) (DxField_t *field, const void *src, HUINT32 size);
} DxSVC_Field_t;

//	BitTable의 적용은 고민이 필요한듯..
#if	0
static	DxEnumStringTable_t	_stSVCTypeBitTable =
{
	(HCHAR*)"DxSvcType_e",
	{
		{	DxNAMEOF(eDxSVC_TYPE_All),		eDxSVC_TYPE_All	},
		{	DxNAMEOF(eDxSVC_TYPE_TV),		eDxSVC_TYPE_TV	},
		{	DxNAMEOF(eDxSVC_TYPE_RADIO),	eDxSVC_TYPE_RADIO	},
		{	DxNAMEOF(eDxSVC_TYPE_DATA),		eDxSVC_TYPE_DATA	},
		{	DxNAMEOF(eDxSVC_TYPE_SUBFEED),	eDxSVC_TYPE_SUBFEED	},
		{	DxNAMEOF(eDxSVC_TYPE_REPLACEMENT),	eDxSVC_TYPE_REPLACEMENT	},
		{	DxNAMEOF(eDxSVC_TYPE_HIDDEN),	eDxSVC_TYPE_HIDDEN	},
		{	DxNAMEOF(eDxSVC_TYPE_MHPTV),	eDxSVC_TYPE_MHPTV	},
		{	DxNAMEOF(eDxSVC_TYPE_ENGINEERING),	eDxSVC_TYPE_ENGINEERING	},
		{	DxNAMEOF(eDxSVC_TYPE_RFVOD),	eDxSVC_TYPE_RFVOD	},
		{	NULL,							0					}
	}
};
#endif

static	DxEnumStringTable_t	_stSVCUpdateTable =
{
	(HCHAR*)"DxSvcUpdateFlag_e",
	{
		{	"normal",	eDxSVC_UPDATE_NORMAL	},
		{	"new",		eDxSVC_UPDATE_NEWCHANNEL	},
		{	"update",	eDxSVC_UPDATE_UPDATECHANNEL	},
		{	"readonly",	eDxSVC_UPDATE_READONLY	},
		{	NULL,								0					}
	}
};

static	DxEnumStringTable_t	_stSVCOrgSvcTypeTable =
{
	(HCHAR*)"DxOrgSvcType_e",
	{
		{	"none",					eDxOrgSVC_TYPE_NONE},
		{	"tv",					eDxOrgSVC_TYPE_DIGITAL_TV	},
		{	"radio",				eDxOrgSVC_TYPE_DIGITAL_RADIO	},
		{	"teletext",				eDxOrgSVC_TYPE_TELETEXT	},
		{	"nvod-ref",				eDxOrgSVC_TYPE_NVOD_REFERENCE	},
		{	"nvod-timeshift",		eDxOrgSVC_TYPE_NVOD_TIMESHIFT	},
		{	"mosaic",				eDxOrgSVC_TYPE_MOSAIC	},
		{	"pal",					eDxOrgSVC_TYPE_PAL	},
		{	"secam",				eDxOrgSVC_TYPE_SECAM	},
		{	"dd2-mac",				eDxOrgSVC_TYPE_DD2_MAC	},
		{	"fm-radio",				eDxOrgSVC_TYPE_FM_RADIO	},
		{	"ntsc",					eDxOrgSVC_TYPE_NTSC	},
		{	"data-broadcast",		eDxOrgSVC_TYPE_DATA_BROADCAST	},
		{	"dvb-mhp",				eDxOrgSVC_TYPE_DVB_MHP	},
		{	"mpeg2-hd",				eDxOrgSVC_TYPE_MPEG2_HD	},
		{	"mpeg2-hd-nvod-timeshift",eDxOrgSVC_TYPE_MPEG2_HD_NVOD_TIMESHIFT	},
		{	"mpeg2-hd-nvod-ref",	eDxOrgSVC_TYPE_MPEG2_HD_NVOD_REFERENCE	},
		{	"aac-audio",			eDxOrgSVC_TYPE_AAC_AUDIO	},
		{	"aac-v2-audio",			eDxOrgSVC_TYPE_AAC_V2_AUDIO	},
		{	"avc-sd-dtv",			eDxOrgSVC_TYPE_AVC_SD_DIGITAL_TV	},
		{	"avc-sd-nvod-timeshift",eDxOrgSVC_TYPE_AVC_SD_NVOD_TIMESHIFT	},
		{	"avc-sd-nvod-ref",		eDxOrgSVC_TYPE_AVC_SD_NVOD_REFERENCE	},
		{	"avc_hd_dtv",			eDxOrgSVC_TYPE_AVC_HD_DIGITAL_TV	},
		{	"avc-hd-nvod-timeshift",eDxOrgSVC_TYPE_AVC_HD_NVOD_TIMESHIFT	},
		{	"avc-hd-nvoid-ref",		eDxOrgSVC_TYPE_AVC_HD_NVOD_REFERENCE	},
		{	"digiturk-down-start",	eDxOrgSVC_TYPE_DIGITURK_DOWN_START	},
		{	"digiturk-tv-no-pts",	eDxOrgSVC_TYPE_DIGITURK_TV_NO_PTS	},
		{	"digiturk-interactive",	eDxOrgSVC_TYPE_DIGITURK_INTERACTIVE	},
		{	"avc-hd-dtv2",			eDxOrgSVC_TYPE_AVC_HD_DIGITAL_TV2	},
		{	"digiturk-down-end",	eDxOrgSVC_TYPE_DIGITURK_DOWN_END	},
		{	"arib-tmp-video",		eDxOrgSVC_TYPE_ARIB_TEMP_VIDEO	},
		{	"arib-tmp-audio",		eDxOrgSVC_TYPE_ARIB_TEMP_AUDIO	},
		{	"arib-tmp-data",		eDxOrgSVC_TYPE_ARIB_TEMP_DATA	},
		{	"arib-engineering",		eDxOrgSVC_TYPE_ARIB_ENGINEERING	},
		{	"arib-promotion-video",	eDxOrgSVC_TYPE_ARIB_PROMOTION_VIDEO	},
		{	"arib-promotion-audio",	eDxOrgSVC_TYPE_ARIB_PROMOTION_AUDIO	},
		{	"arib-promotion-data", 	eDxOrgSVC_TYPE_ARIB_PROMOTION_DATA	},
		{	"arib-before-accumul-data", eDxOrgSVC_TYPE_ARIB_BEFORE_ACCUMUL_DATA	},
		{	"arib-accumul-data",	eDxOrgSVC_TYPE_ARIB_ACCUMUL_DATA	},
		{	"arib-bookmark-list",	eDxOrgSVC_TYPE_ARIB_BOOKMARK_LIST_DATA	},
		{	"data",					eDxOrgSVC_TYPE_DATA	},
		{	"subfeed",				eDxOrgSVC_TYPE_SUBFEED	},
		{	NULL,											0					}
	}
};

#if	0
static	DxEnumStringTable_t	_stSVCUserFlagBitTable =
{
	(HCHAR*)"DxUserFlag_t",
	{
		{	DxNAMEOF(eDxSvcUserFlag_All),	eDxSvcUserFlag_All	},
		{	DxNAMEOF(eDxSvcUserFlag_01_FsatUserCustom),	eDxSvcUserFlag_01_FsatUserCustom	},
		{	DxNAMEOF(eDxSvcUserFlag_02_Selectable),	eDxSvcUserFlag_02_Selectable	},
		{	DxNAMEOF(eDxSvcUserFlag_03_SvcDeletedByUser),	eDxSvcUserFlag_03_SvcDeletedByUser	},
		{	DxNAMEOF(eDxSvcUserFlag_04_CanalreadyTerLcn),	eDxSvcUserFlag_04_CanalreadyTerLcn	},
		{	DxNAMEOF(eDxSvcUserFlag_05_reserved),	eDxSvcUserFlag_05_reserved	},
		{	DxNAMEOF(eDxSvcUserFlag_06_TemporaryAlive),	eDxSvcUserFlag_06_TemporaryAlive	},
		{	DxNAMEOF(eDxSvcUserFlag_07_SkipChUpDown),	eDxSvcUserFlag_07_SkipChUpDown	},
		{	DxNAMEOF(eDxSvcUserFlag_08_RegionalSvc),	eDxSvcUserFlag_08_RegionalSvc	},
		{	DxNAMEOF(eDxSvcUserFlag_09_AdultSvc),	eDxSvcUserFlag_09_AdultSvc	},
		{	DxNAMEOF(eDxSvcUserFlag_10_NationalSvc),	eDxSvcUserFlag_10_NationalSvc	},
		{	DxNAMEOF(eDxSvcUserFlag_11_FreeCiMode),	eDxSvcUserFlag_11_FreeCiMode	},
		{	DxNAMEOF(eDxSvcUserFlag_12_IsExistFreqList),	eDxSvcUserFlag_12_IsExistFreqList	},
		{	DxNAMEOF(eDxSvcUserFlag_13_VolatileSvc),	eDxSvcUserFlag_13_VolatileSvc	},
		{	DxNAMEOF(eDxSvcUserFlag_14_NoRecTsr),	eDxSvcUserFlag_14_NoRecTsr	},
		{	DxNAMEOF(eDxSvcUserFlag_15_TsrOnly),	eDxSvcUserFlag_15_TsrOnly	},
		{	DxNAMEOF(eDxSvcUserFlag_16_QuasiStaticAlive),	eDxSvcUserFlag_16_QuasiStaticAlive	},
		{	DxNAMEOF(eDxSvcUserFlag_17_TivuSatLcn),	eDxSvcUserFlag_17_TivuSatLcn	},
		{	DxNAMEOF(eDxSvcUserFlag_18_DigiturkEutelSat),	eDxSvcUserFlag_18_DigiturkEutelSat	},
		{	DxNAMEOF(eDxSvcUserFlag_19_DigiturkTurkSat),	eDxSvcUserFlag_19_DigiturkTurkSat	},
		{	DxNAMEOF(eDxSvcUserFlag_20_ParentalControl),	eDxSvcUserFlag_20_ParentalControl	},
		{	DxNAMEOF(eDxSvcUserFlag_21_LockSvcByUser),	eDxSvcUserFlag_21_LockSvcByUser	},
		{	DxNAMEOF(eDxSvcUserFlag_22_SkipSvcByUser),	eDxSvcUserFlag_22_SkipSvcByUser	},
		{	DxNAMEOF(eDxSvcUserFlag_23_PreferSvcByUser),	eDxSvcUserFlag_23_PreferSvcByUser	},
		{	DxNAMEOF(eDxSvcUserFlag_24_TempSvc),	eDxSvcUserFlag_24_TempSvc	},
		{	DxNAMEOF(eDxSvcUserFlag_25_MaskCh),	eDxSvcUserFlag_25_MaskCh	},
		{	DxNAMEOF(eDxSvcUserFlag_26_NordigLcnV2),	eDxSvcUserFlag_26_NordigLcnV2	},
		{	DxNAMEOF(eDxSvcUserFlag_27_reserved),	eDxSvcUserFlag_27_reserved	},
		{	DxNAMEOF(eDxSvcUserFlag_28_reserved),	eDxSvcUserFlag_28_reserved	},
		{	DxNAMEOF(eDxSvcUserFlag_29_reserved),	eDxSvcUserFlag_29_reserved	},
		{	DxNAMEOF(eDxSvcUserFlag_30_reserved),	eDxSvcUserFlag_30_reserved	},
		{	DxNAMEOF(eDxSvcUserFlag_31_reserved),	eDxSvcUserFlag_31_reserved	},
		{	DxNAMEOF(eDxSvcUserFlag_32_NewService),	eDxSvcUserFlag_32_NewService	},
		{	NULL,							0					}
	}
};
#endif

static HBOOL	dlib_svc_Encode (HINT32 jw, const DxService_t *service)
{
	HINT32	i;

#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	DxTvAnyTime_RegionInfo_t	*pstRegionInfo;
#endif

	HxJSONWRITER_ObjectBegin(jw);
		HxJSONWRITER_Integer(jw, "uid", service->uid);
		HxJSONWRITER_Integer(jw, "version", service->version);
		HxJSONWRITER_Integer(jw, "tsuid", service->tsuid);
		HxJSONWRITER_Integer(jw, "prvuid", service->prvuid);
		HxJSONWRITER_Integer(jw, "antuid", service->antuid);

		HxJSONWRITER_Array(jw, "grpuids");
		for (i = 0; i < DxGROUP_LEN; i++)
			HxJSONWRITER_IntegerValue(jw, service->grpuids[i]);
		HxJSONWRITER_ArrayEnd(jw);

		HxJSONWRITER_Array(jw, "bqtuids");
		for (i = 0; i < DxBOUQUET_LEN; i++)
			HxJSONWRITER_IntegerValue(jw, service->bqtuids[i]);
		HxJSONWRITER_ArrayEnd(jw);

		HxJSONWRITER_Integer(jw, "svcid", service->svcid);
		HxJSONWRITER_Integer(jw, "tsid", service->tsid);
		HxJSONWRITER_Integer(jw, "onid", service->onid);
		HxJSONWRITER_Integer(jw, "lcn", service->lcn);
		HxJSONWRITER_String (jw, "name", service->name);

		//	Bit Table DxSvcType_e( _stSVCTypeBitTable )
		HxJSONWRITER_Integer(jw, "svcType", service->svcType);
		HxJSONWRITER_String (jw, "orgSvcType", dlib_ConvertEnum2Str(DxNAMEOF(DxOrgSvcType_e), service->orgSvcType));
		HxJSONWRITER_Integer(jw, "optype", service->optype);
		HxJSONWRITER_String(jw, "deliType", dlib_ConvertEnum2Str(DxNAMEOF(DxDeliveryType_e), service->deliType));
		HxJSONWRITER_String(jw, "satType", service->satType);
		HxJSONWRITER_Boolean(jw, "visibleFlag", service->visibleFlag);
		HxJSONWRITER_Boolean(jw, "locked", service->locked);
		HxJSONWRITER_Boolean(jw, "removed", service->removed);
		HxJSONWRITER_Boolean(jw, "renamed", service->renamed);

		HxJSONWRITER_Integer(jw, "pmtPid", service->pmtPid);
		HxJSONWRITER_Integer(jw, "pcrPid", service->pcrPid);
		HxJSONWRITER_Integer(jw, "videoPid", service->videoPid);
		HxJSONWRITER_Integer(jw, "audioPid", service->audioPid);
		HxJSONWRITER_Integer(jw, "ttxPid", service->ttxPid);
		HxJSONWRITER_Integer(jw, "audioAuxPid", service->audioAuxPid);
		HxJSONWRITER_Integer(jw, "dolbyPid", service->dolbyPid);

		HxJSONWRITER_String (jw, "highDefinition", dlib_ConvertEnum2Str(DxNAMEOF(DxVideoType_e), service->highDefinition));
		//	Bit Table (DxCasType_e)
		HxJSONWRITER_Integer(jw, "casType", service->casType);

		HxJSONWRITER_Boolean(jw, "dolbyFlag", service->dolbyFlag);
		HxJSONWRITER_String (jw, "videoCodec",    dlib_ConvertEnum2Str(DxNAMEOF(DxVideoCodec_e), service->videoCodec));
		HxJSONWRITER_String (jw, "audioCodec",    dlib_ConvertEnum2Str(DxNAMEOF(DxAudioCodec_e), service->audioCodec));
		HxJSONWRITER_String (jw, "audioAuxCodec", dlib_ConvertEnum2Str(DxNAMEOF(DxAudioCodec_e), service->audioAuxCodec));
		HxJSONWRITER_String (jw, "dolbyCodec",    dlib_ConvertEnum2Str(DxNAMEOF(DxAudioCodec_e), service->dolbyCodec));

		HxJSONWRITER_Boolean(jw, "lcnFlag", service->lcnFlag);
		HxJSONWRITER_Integer(jw, "svcSection", service->svcSection);

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
		HxJSONWRITER_Integer(jw, "instType", service->cas.instType);
		HxJSONWRITER_Integer(jw, "slotId", service->cas.slotId);
		HxJSONWRITER_Boolean(jw, "selectableFlag", service->cas.selectableFlag);
		HxJSONWRITER_String (jw, "operatorName", service->cas.operatorName);
		HxJSONWRITER_Array(jw, "camId");
		for (i = 0; i < 8; i++)
		{
			HxJSONWRITER_IntegerValue(jw, service->cas.camId[i]);
		}
		HxJSONWRITER_ArrayEnd(jw);
#endif
		HxJSONWRITER_Object(jw, "user");
		HxJSONWRITER_Boolean(jw, "subtitle", service->user.subtitle);
#if defined(SVC_SUBTITLE_LANGUAGE_CODE)
		HxJSONWRITER_String(jw, "subtitleLanguage", service->user.subtitleLanguage);
		HxJSONWRITER_Boolean(jw, "hardOfHearing", service->user.hardOfHearing);
#endif
		HxJSONWRITER_Integer(jw, "subtitleTrack", service->user.subtitleTrack);
		HxJSONWRITER_Boolean(jw, "audio", service->user.audio);
#if defined(SVC_AUDIO_LANGUAGE_CODE)
		HxJSONWRITER_String(jw, "audioLanguage", service->user.audioLanguage);
		HxJSONWRITER_Boolean(jw, "audioDescription", service->user.audioDescription);
		HxJSONWRITER_String(jw, "dualMonoSelect", dlib_ConvertEnum2Str(DxNAMEOF(DxDualMonoSelect_e), service->user.dualMonoSelect));
#endif
		HxJSONWRITER_String (jw, "audioTrack",   dlib_ConvertEnum2Str(DxNAMEOF(DxStereoSelect_e), service->user.audioTrack));
		HxJSONWRITER_Integer(jw, "ratingPassedEventId", service->user.ratingPassedEventId);
		HxJSONWRITER_ObjectEnd(jw);

		//	Bit Table(???)
		HxJSONWRITER_Integer(jw, "updateSvcFlag", service->updateSvcFlag);

		#if	defined(CONFIG_OP_FREESAT)
			HxJSONWRITER_Object (jw, "freesat");
			HxJSONWRITER_Integer(jw, "svcId", service->ex.freesat.svcId);
			HxJSONWRITER_Integer(jw, "regionId", service->ex.freesat.regionId);
			HxJSONWRITER_String (jw, "defaultAuth", service->ex.freesat.defaultAuth);
			HxJSONWRITER_Integer(jw, "contentMgr", service->ex.freesat.contentMgr);
			HxJSONWRITER_String (jw, "szShortName", service->ex.freesat.szShortName);
			HxJSONWRITER_Integer(jw, "ucUserCustomizable", service->ex.freesat.ucUserCustomizable);
			HxJSONWRITER_Integer(jw, "ucUserSelectable", service->ex.freesat.ucUserSelectable);
			HxJSONWRITER_Integer(jw, "ucAdultSvc", service->ex.freesat.ucAdultSvc);
			HxJSONWRITER_Integer(jw, "ucGuidanceType", service->ex.freesat.ucGuidanceType);
			HxJSONWRITER_Integer(jw, "ucGuidanceMode", service->ex.freesat.ucGuidanceMode);
			HxJSONWRITER_String (jw, "szGuidanceStr", service->ex.freesat.szGuidanceStr);
			HxJSONWRITER_Integer(jw, "ucPresentFollow", service->ex.freesat.ucPresentFollow);
			HxJSONWRITER_ObjectEnd(jw);
		#endif

		#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
			HxJSONWRITER_Object (jw, "tva");
			HxJSONWRITER_Integer(jw, "ucGuidanceType", service->ex.tva.ucGuidanceType);
			HxJSONWRITER_Integer(jw, "ucGuidanceMode", service->ex.tva.ucGuidanceMode);
			HxJSONWRITER_String (jw, "szGuidanceStr", service->ex.tva.szGuidanceStr);
			HxJSONWRITER_Integer(jw, "contentMgr", service->ex.tva.ulFTAContentMgr);
			HxJSONWRITER_String (jw, "defaultAuth", service->ex.tva.szDefaultAuth);

			HxJSONWRITER_Array(jw, "regionInfo");
			for (i = 0; i < DxMAX_T_REGION_NUM; i++)
			{
				pstRegionInfo = (DxTvAnyTime_RegionInfo_t *)&service->ex.tva.stRegionInfo[i];
				HxJSONWRITER_ObjectBegin (jw);
				HxJSONWRITER_String(jw, "countrycode",pstRegionInfo->szCountryCode);
				HxJSONWRITER_Integer(jw, "regiondepth",pstRegionInfo->ucRegionDepth);
				HxJSONWRITER_Integer(jw, "regioncode",pstRegionInfo->ulRegionCode);
				HxJSONWRITER_ObjectEnd(jw);
			}
			HxJSONWRITER_ArrayEnd(jw);


			HxJSONWRITER_Integer(jw, "lcnsub",service->ex.tva.usLcnSub);
			HxJSONWRITER_ObjectEnd(jw);
		#endif

		#if defined(CONFIG_OP_ITALY_DBOOK)
		HxJSONWRITER_Integer(jw, "ucUserSelectable", service->ex.itaDtt.ucUserSelectable);
		HxJSONWRITER_Integer(jw, "ucUserDeleted", service->ex.itaDtt.ucUserDeletedByUser);
		#endif

		#if defined(CONFIG_OP_SES)
			HxJSONWRITER_Object (jw, "ses");
			HxJSONWRITER_Integer(jw, "virtualSvcId", service->ex.ses.usVirtualSvcId);
//			HxJSONWRITER_Integer(jw, "newSvc", service->ex.ses.bNewSvc);
			HxJSONWRITER_ObjectEnd(jw);
		#endif

	HxJSONWRITER_ObjectEnd(jw);

	return HLIB_JSONWRITER_Error(jw) ? FALSE : TRUE;
}

static HERROR	dlib_svc_Encoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HINT32			jw;
	DxService_t		*service = (DxService_t*)src;

	HxLOG_Assert(sizeof(DxService_t) == srcSize);

	jw = HLIB_JSONWRITER_Open();

	if (!dlib_svc_Encode(jw, service))
	{
		HxLOG_Error("JSON Error : (%s)\n", HLIB_JSONWRITER_Error(jw));

		*dstSize = 0;
		*dst = NULL;
	} else
	{
		*dstSize = HLIB_JSONWRITER_GetLength(jw);
		*dst     = DLIB_StrDup(HLIB_JSONWRITER_GetJSON(jw));
		HLIB_JSONWRITER_Close(jw);
	}

	if (*dstSize > 0)
		return ERR_OK;

	return ERR_FAIL;
}

static HBOOL	dlib_svc_Decode (HxJSON_t json, DxService_t *service)
{
	HxJSON_t array, item;

	service->typeOf = DxNAMEOF(DxService_t);
	service->uid = HLIB_JSON_Object_GetInteger(json, "uid");
	service->version = HLIB_JSON_Object_GetInteger(json, "version");
	service->tsuid  = HLIB_JSON_Object_GetInteger(json, "tsuid");
	service->prvuid = HLIB_JSON_Object_GetInteger(json, "prvuid");
	service->antuid = HLIB_JSON_Object_GetInteger(json, "antuid");
	array = HLIB_JSON_Object_GetArray(json, "grpuids");
	if (array)
	{
		HINT32	i = 0;
		HxJSON_ARRAY_EACH(array, item,{
			if (!item || !HxJSON_IsInteger(item))
				continue;
			if (i == DxGROUP_LEN)
				break;
			service->grpuids[i++] = HxJSON_Integer_Get(item);
		});
	}
	array = HLIB_JSON_Object_GetArray(json, "bqtuids");
	if (array)
	{
		HINT32	i = 0;
		HxJSON_ARRAY_EACH(array, item,{
			if (!item || !HxJSON_IsInteger(item))
				continue;
			if (i == DxBOUQUET_LEN)
				break;
			service->bqtuids[i++] = HxJSON_Integer_Get(item);
		});
	}
	service->svcid = HLIB_JSON_Object_GetInteger(json, "svcid");
	service->tsid = HLIB_JSON_Object_GetInteger(json, "tsid");
	service->onid = HLIB_JSON_Object_GetInteger(json, "onid");
	service->lcn  = HLIB_JSON_Object_GetInteger(json, "lcn");
	HxSTD_StrNCpy(service->name, HLIB_JSON_Object_GetString(json, "name"), DxNAME_LEN - 1);

	//	Bit Table (DxSvcType_e)
	service->svcType = HLIB_JSON_Object_GetInteger(json, "svcType");
	service->orgSvcType = dlib_ConvertStr2Enum(DxNAMEOF(DxOrgSvcType_e), HLIB_JSON_Object_GetString(json, "orgSvcType"));
	service->optype = HLIB_JSON_Object_GetInteger(json, "optype");
	service->deliType = dlib_ConvertStr2Enum(DxNAMEOF(DxDeliveryType_e), HLIB_JSON_Object_GetString(json, "deliType"));
	HxSTD_StrNCpy(service->satType, HLIB_JSON_Object_GetString(json, "satType"), DxANTINFO_NAME_LEN - 1);

	service->visibleFlag = HLIB_JSON_Object_GetBoolean(json, "visibleFlag");
	service->locked = HLIB_JSON_Object_GetBoolean(json, "locked");
	service->removed = HLIB_JSON_Object_GetBoolean(json, "removed");
	service->renamed = HLIB_JSON_Object_GetBoolean(json, "renamed");

	service->pmtPid = HLIB_JSON_Object_GetInteger(json, "pmtPid");
	service->pcrPid = HLIB_JSON_Object_GetInteger(json, "pcrPid");
	service->videoPid = HLIB_JSON_Object_GetInteger(json, "videoPid");
	service->audioPid = HLIB_JSON_Object_GetInteger(json, "audioPid");
	service->ttxPid = HLIB_JSON_Object_GetInteger(json, "ttxPid");
	service->audioAuxPid = HLIB_JSON_Object_GetInteger(json, "audioAuxPid");
	service->dolbyPid = HLIB_JSON_Object_GetInteger(json, "dolbyPid");
	service->highDefinition = dlib_ConvertStr2Enum(DxNAMEOF(DxVideoType_e), HLIB_JSON_Object_GetString(json, "highDefinition"));

	//	Bit Table (DxCasType_e)
	service->casType = HLIB_JSON_Object_GetInteger(json, "casType");

	service->dolbyFlag = HLIB_JSON_Object_GetBoolean(json, "dolbyFlag");
	service->videoCodec = dlib_ConvertStr2Enum(DxNAMEOF(DxVideoCodec_e), HLIB_JSON_Object_GetString(json, "videoCodec"));
	service->audioCodec = dlib_ConvertStr2Enum(DxNAMEOF(DxAudioCodec_e), HLIB_JSON_Object_GetString(json, "audioCodec"));
	service->audioAuxCodec = dlib_ConvertStr2Enum(DxNAMEOF(DxAudioCodec_e), HLIB_JSON_Object_GetString(json, "audioAuxCodec"));
	service->dolbyCodec = dlib_ConvertStr2Enum(DxNAMEOF(DxAudioCodec_e), HLIB_JSON_Object_GetString(json, "dolbyCodec"));

	service->lcnFlag = HLIB_JSON_Object_GetBoolean(json, "lcnFlag");
	service->svcSection= HLIB_JSON_Object_GetInteger(json, "svcSection");

#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	service->cas.instType = HLIB_JSON_Object_GetInteger(json, "instType");
	service->cas.slotId = HLIB_JSON_Object_GetInteger(json, "slotId");
	service->cas.selectableFlag = HLIB_JSON_Object_GetBoolean(json, "selectableFlag");
	HxSTD_StrNCpy(service->cas.operatorName, HLIB_JSON_Object_GetString(json, "operatorName"), DxNAME_LEN-1);

	array = HLIB_JSON_Object_GetArray(json, "camId");
	if (array)
	{
		HINT32	i = 0;
		HxJSON_ARRAY_EACH(array, item,{
			if (!item || !HxJSON_IsInteger(item))
				continue;
			if (i == 8)
				break;
			service->cas.camId[i++] = HxJSON_Integer_Get(item);
		});
	}
#endif

	{
		HxJSON_t	user = HLIB_JSON_Object_GetObject(json, "user");
		if (user)
		{
			const HCHAR	*track;
#if defined(SVC_AUDIO_LANGUAGE_CODE)
			HCHAR *audioLanguage = NULL;
#endif
#if defined(SVC_SUBTITLE_LANGUAGE_CODE)
			HCHAR *subtitleLanguage = NULL;
#endif
			service->user.subtitle = HLIB_JSON_Object_GetBoolean(user, "subtitle");
#if defined(SVC_SUBTITLE_LANGUAGE_CODE)
			subtitleLanguage = (HCHAR*)HLIB_JSON_Object_GetString(user, "subtitleLanguage");
			if(subtitleLanguage != NULL)
			{
				HxSTD_StrNCpy(service->user.subtitleLanguage, subtitleLanguage, DxLANG_CODE_LEN);
				service->user.subtitleLanguage[DxLANG_CODE_LEN-1] = 0;
			}

			service->user.hardOfHearing = HLIB_JSON_Object_GetBoolean(user, "hardOfHearing");
#endif
			service->user.subtitleTrack = HLIB_JSON_Object_GetInteger(user, "subtitleTrack");

			service->user.audio = HLIB_JSON_Object_GetBoolean(user, "audio");
#if defined(SVC_AUDIO_LANGUAGE_CODE)
			audioLanguage = (HCHAR*)HLIB_JSON_Object_GetString(user, "audioLanguage");
			if(audioLanguage != NULL)
			{
				HxSTD_StrNCpy(service->user.audioLanguage, audioLanguage, DxLANG_CODE_LEN);
				service->user.audioLanguage[DxLANG_CODE_LEN-1] = 0;
			}

			service->user.audioDescription = HLIB_JSON_Object_GetBoolean(user, "audioDescription");
			service->user.dualMonoSelect = dlib_ConvertStr2Enum(DxNAMEOF(DxDualMonoSelect_e), HLIB_JSON_Object_GetString(user, "dualMonoSelect"));
#endif
			track = HLIB_JSON_Object_GetString(user, "audioTrack");
			if (track)
				service->user.audioTrack = dlib_ConvertStr2Enum(DxNAMEOF(DxStereoSelect_e), track);
			service->user.ratingPassedEventId = HLIB_JSON_Object_GetInteger(user, "ratingPassedEventId");
		}
	}

	//	Bit Table(???)
	service->updateSvcFlag = HLIB_JSON_Object_GetInteger(json, "updateSvcFlag");

	#if	defined(CONFIG_OP_FREESAT)
	{
		HxJSON_t	freesat;

		freesat = HxJSON_Object_Get(json, "freesat");
		if (freesat)
		{
			service->ex.freesat.svcId = HLIB_JSON_Object_GetInteger(freesat, "svcId");
			service->ex.freesat.regionId = HLIB_JSON_Object_GetInteger(freesat, "regionId");
			HxSTD_StrNCpy(service->ex.freesat.defaultAuth, HxJSON_Object_Get(freesat, "defaultAuth"), DxDEFAULT_AUTHORITY_LEN - 1);
			service->ex.freesat.contentMgr = HLIB_JSON_Object_GetInteger(freesat, "contentMgr");
			HxSTD_StrNCpy(service->ex.freesat.szShortName, HxJSON_Object_Get(freesat, "szShortName"), DxFSAT_SVC_SHORT_NAME_LEN - 1);
			service->ex.freesat.ucUserCustomizable = HLIB_JSON_Object_GetInteger(freesat, "ucUserCustomizable");
			service->ex.freesat.ucUserSelectable = HLIB_JSON_Object_GetInteger(freesat, "ucUserSelectable");
			service->ex.freesat.ucAdultSvc = HLIB_JSON_Object_GetInteger(freesat, "ucAdultSvc");
			service->ex.freesat.ucGuidanceType = HLIB_JSON_Object_GetInteger(freesat, "ucGuidanceType");
			service->ex.freesat.ucGuidanceMode = HLIB_JSON_Object_GetInteger(freesat, "ucGuidanceMode");
			HxSTD_StrNCpy(service->ex.freesat.szGuidanceStr, HxJSON_Object_Get(freesat, "szGuidanceStr"), DxFSAT_SVC_GUIDANCE_LEN - 1);
			service->ex.freesat.ucPresentFollow = HLIB_JSON_Object_GetInteger(freesat, "ucPresentFollow");
		}
	}
	#endif


#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	{
		HxJSON_t	tvAnyTime;

		tvAnyTime = HxJSON_Object_Get(json, "tva");
		if (tvAnyTime)
		{
			HxSTD_StrNCpy(service->ex.tva.szDefaultAuth, HLIB_JSON_Object_GetString(tvAnyTime, "defaultAuth"), DxDEFAULT_AUTHORITY_LEN - 1);
			service->ex.tva.ucGuidanceType = HLIB_JSON_Object_GetInteger(tvAnyTime, "ucGuidanceType");
			service->ex.tva.ucGuidanceMode = HLIB_JSON_Object_GetInteger(tvAnyTime, "ucGuidanceMode");
			HxSTD_StrNCpy(service->ex.tva.szGuidanceStr, HLIB_JSON_Object_GetString(tvAnyTime, "szGuidanceStr"), DxMAX_GUIDANCE_INFO_LEN -1);


			array = HLIB_JSON_Object_GetArray(tvAnyTime, "regioninfo");
			if (array && HLIB_JSON_GetSize(array) > 0)
			{
				HINT32	i = 0;
				const HCHAR	*value = NULL;

				HxJSON_ARRAY_EACH(array, item, {
					value = HLIB_JSON_Object_GetString(item, "countrycode");
					if(value != NULL)
						 HLIB_STD_StrNCpySafe((HCHAR*)&service->ex.tva.stRegionInfo[i].szCountryCode, value, 4);
					service->ex.tva.stRegionInfo[i].ucRegionDepth = HLIB_JSON_Object_GetInteger(item, "regiondepth");
					service->ex.tva.stRegionInfo[i].ulRegionCode = HLIB_JSON_Object_GetInteger(item,"regioncode");
					if (++i == DxMAX_T_REGION_NUM)
						break;
				});
			}

			service->ex.tva.usLcnSub = HLIB_JSON_Object_GetInteger(json, "lcnsub");
		}
	}
#endif

#if defined(CONFIG_OP_SES)
	{
		HxJSON_t	ses;

		ses = HxJSON_Object_Get(json, "ses");
		if(ses)
		{
			service->ex.ses.usVirtualSvcId = HLIB_JSON_Object_GetInteger(ses, "virtualSvcId");
//			service->ex.ses.bNewSvc = HLIB_JSON_Object_GetInteger(ses, "newSvc");
		}
	}
#endif

#if defined(CONFIG_OP_ITALY_DBOOK)
	{
		HxJSON_t	itaDtt;

		itaDtt = HxJSON_Object_Get(json, "itaDtt");
		if (itaDtt)
		{
			service->ex.itaDtt.ucUserSelectable = HLIB_JSON_Object_GetInteger(itaDtt, "ucUserSelectable");
			service->ex.itaDtt.ucUserDeletedByUser = HLIB_JSON_Object_GetInteger(itaDtt, "ucUserDeletedByUser");
		}
	}
#endif

	return TRUE;
}

static HERROR	dlib_svc_Decoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HxJSON_t		root;
	DxService_t		*service;

	HxLOG_Assert(srcSize && src);

	root = HLIB_JSON_Decode(src);
	HxLOG_Assert(root);

	service = (DxService_t*)DLIB_Calloc(sizeof(DxService_t));

	(void) dlib_svc_Decode(root, service);

	HLIB_JSON_Delete(root);

	*dst = (void*)service;
	*dstSize = sizeof(DxService_t);

	return ERR_OK;
}

static HBOOL	dlib_svc_GetName (DxField_t *field, const void *src, HUINT32 size)
{
	const DxService_t	*svc = (DxService_t*)src;

	HxLOG_Assert(src && size);
	field->u.string = DLIB_StrDup(svc->name);

	return TRUE;
}

static HBOOL	dlib_svc_GetTsID (DxField_t *field, const void *src, HUINT32 size)
{
	const DxService_t	*svc = (DxService_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = svc->tsid;

	return TRUE;
}

static HBOOL	dlib_svc_GetSID (DxField_t *field, const void *src, HUINT32 size)
{
	const DxService_t	*svc = (DxService_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = svc->svcid;

	return TRUE;
}

static HBOOL	dlib_svc_GetOnID (DxField_t *field, const void *src, HUINT32 size)
{
	const DxService_t	*svc = (DxService_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = svc->onid;

	return TRUE;
}

static HBOOL	dlib_svc_GetLcn (DxField_t *field, const void *src, HUINT32 size)
{
	const DxService_t	*svc = (DxService_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = svc->lcn;

	return TRUE;
}

static HBOOL	dlib_svc_GetSvcType (DxField_t *field, const void *src, HUINT32 size)
{
	const DxService_t	*svc = (DxService_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = svc->svcType;

	return TRUE;
}

static HBOOL	dlib_svc_GetCasType (DxField_t *field, const void *src, HUINT32 size)
{
	const DxService_t	*svc = (DxService_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = svc->casType;

	return TRUE;
}

static HBOOL	dlib_svc_GetHighDefinition (DxField_t *field, const void *src, HUINT32 size)
{
	const DxService_t	*svc = (DxService_t*)src;

	HxLOG_Assert(src && size);
	field->u.integer = svc->highDefinition;

	return TRUE;
}

static const DxSVC_Field_t	s_SVC_FIELDS[] =
{
	  {{eDxTYPE_STRING , "name"      , TRUE , {0}}, dlib_svc_GetName}
	, {{eDxTYPE_INTEGER, "tsid"      , FALSE, {0}}, dlib_svc_GetTsID}
	, {{eDxTYPE_INTEGER, "sid"       , FALSE, {0}}, dlib_svc_GetSID}
	, {{eDxTYPE_INTEGER, "onid"      , FALSE, {0}}, dlib_svc_GetOnID}
	, {{eDxTYPE_INTEGER, "lcn"       , FALSE, {0}}, dlib_svc_GetLcn}
	, {{eDxTYPE_INTEGER, "svcType"   , FALSE, {0}}, dlib_svc_GetSvcType}
	, {{eDxTYPE_INTEGER, "casType"   , FALSE, {0}}, dlib_svc_GetCasType}
	, {{eDxTYPE_INTEGER, "hd"        , FALSE, {0}}, dlib_svc_GetHighDefinition}
};

void			DLIB_SERVICE_Init(void)
{
//	dlib_RegisterEnumTable(&_stSVCTypeBitTable);
	dlib_RegisterEnumTable(&_stSVCUpdateTable);
	dlib_RegisterEnumTable(&_stSVCOrgSvcTypeTable);
//	dlib_RegisterEnumTable(&_stSVCUserFlagBitTable);

	dlib_RegisterDECModule(DxNAMEOF(DxService_t), dlib_svc_Encoder, dlib_svc_Decoder);
}

HINT32			DLIB_SERVICE_GetFieldCount (void)
{
	return sizeof(s_SVC_FIELDS) / sizeof(s_SVC_FIELDS[0]);
}

DxField_t *		DLIB_SERVICE_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	DxField_t field;

	if (DLIB_SERVICE_GetFieldCount() <= index)
	{
		HxLOG_Warning("%s(index) index(index) out of range~!\n", __FUNCTION__, index);
		return NULL;
	}
	memcpy(&field, &(s_SVC_FIELDS[index].field), sizeof(DxField_t));

	if (src && srcSize && s_SVC_FIELDS[index].getData)
	{
		if (!s_SVC_FIELDS[index].getData(&field, src, srcSize))
		{
			HxLOG_Warning("%s(%s) Cannot get data\n", __FUNCTION__, s_SVC_FIELDS[index].field.name);
			return NULL;
		}
	}

	// 메모리 함수는 HxSTD_MemXXX를 사용한다.
	return (DxField_t *)DLIB_MemDup(&field, sizeof(DxField_t));
}

HERROR			DLIB_SERVICE_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_svc_Encoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_SERVICE_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_svc_Decoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_SERVICE_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 데이터 전송 시, 효율을 높이기 위해 압축이 필요한 경우 이 함수 호출됨
	// 압축 방식은 진짜 압축이 아닌, 그냥 string을 binary로 바꾸는 등
	// 아무튼 데이터를 줄이기만 하면 됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}

HERROR			DLIB_SERVICE_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 압축된 데이터 원복
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}


HERROR			DLIB_SERVICE_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	DxService_t	*dst;

	dst = (DxService_t *)DLIB_MemDup(src, srcSize);
	if (dst)
	{
		dst->typeOf = DxNAMEOF(DxService_t);
		*type = (void *)dst;
		return ERR_OK;
	}
	return ERR_FAIL;
}

void			DLIB_SERVICE_FreeType (void *type)
{
	DLIB_Free(type);
}

void			DLIB_SERVICE_Print (FILE *fp, const DxService_t *t)
{
	fprintf(fp, "SERVICE[%d] %d.%s\n"
		, t->uid
		, t->lcn
		, t->name
	);
	DxLIB_PRINT_INT(t->version);
	DxLIB_PRINT_INT(t->tsuid);
	DxLIB_PRINT_INT(t->prvuid);
	DxLIB_PRINT_INT(t->antuid);
	DxLIB_PRINT_INT_ARRAY(t->grpuids, DxGROUP_LEN);
	DxLIB_PRINT_INT_ARRAY(t->bqtuids, DxBOUQUET_LEN);
	DxLIB_PRINT_HEX(t->svcid);
	DxLIB_PRINT_HEX(t->tsid);
	DxLIB_PRINT_HEX(t->onid);
	DxLIB_PRINT_ENUM(t->svcType);
	DxLIB_PRINT_HEX(t->orgSvcType);
	DxLIB_PRINT_INT(t->optype);
	DxLIB_PRINT_ENUM(t->deliType);
	DxLIB_PRINT_STR(t->satType);
	DxLIB_PRINT_BOOL(t->visibleFlag);
	DxLIB_PRINT_BOOL(t->locked);
	DxLIB_PRINT_BOOL(t->removed);
	DxLIB_PRINT_BOOL(t->renamed);
	DxLIB_PRINT_HEX(t->pmtPid);
	DxLIB_PRINT_HEX(t->pcrPid);
	DxLIB_PRINT_HEX(t->videoPid);
	DxLIB_PRINT_HEX(t->audioPid);
	DxLIB_PRINT_HEX(t->ttxPid);
	DxLIB_PRINT_HEX(t->audioAuxPid);
	DxLIB_PRINT_HEX(t->dolbyPid);
	DxLIB_PRINT_ENUM(t->highDefinition);
	DxLIB_PRINT_ENUM(t->casType);
	DxLIB_PRINT_BOOL(t->dolbyFlag);
	DxLIB_PRINT_ENUM(t->videoCodec);
	DxLIB_PRINT_ENUM(t->audioCodec);
	DxLIB_PRINT_ENUM(t->audioAuxCodec);
	DxLIB_PRINT_ENUM(t->dolbyCodec);
	DxLIB_PRINT_BOOL(t->lcnFlag);
	DxLIB_PRINT_ENUM(t->svcSection);
#if defined(CONFIG_MW_CI_PLUS_V_1_3)
	DxLIB_PRINT_INT(t->cas.instType);
	DxLIB_PRINT_INT(t->cas.slotId);
	DxLIB_PRINT_BOOL(t->cas.selectableFlag);
	DxLIB_PRINT_STR(t->cas.operatorName);
	DxLIB_PRINT_STR(t->cas.camId);
#endif
	DxLIB_PRINT_BOOL(t->user.subtitle);
#if defined(SVC_SUBTITLE_LANGUAGE_CODE)
	DxLIB_PRINT_STR	(t->user.subtitleLanguage);
	DxLIB_PRINT_BOOL(t->user.hardOfHearing);
#endif
	DxLIB_PRINT_INT(t->user.subtitleTrack);
	DxLIB_PRINT_BOOL(t->user.audio);
#if defined(SVC_AUDIO_LANGUAGE_CODE)
	DxLIB_PRINT_STR	(t->user.audioLanguage);
	DxLIB_PRINT_BOOL(t->user.audioDescription);
	DxLIB_PRINT_ENUM(t->user.dualMonoSelect);
#endif

	DxLIB_PRINT_ENUM(t->user.audioTrack);
	DxLIB_PRINT_INT(t->user.ratingPassedEventId);
	DxLIB_PRINT_ENUM(t->updateSvcFlag);

#ifdef	CONFIG_OP_FREESAT
	{
		const DxFreesatService_t	*freesat = &(t->ex.freesat);
		DxLIB_PRINT_HEX(freesat->svcId);
		DxLIB_PRINT_HEX(freesat->regionId);
		DxLIB_PRINT_STR(freesat->defaultAuth);
		DxLIB_PRINT_HEX(freesat->contentMgr);
		DxLIB_PRINT_STR(freesat->szShortName);
		DxLIB_PRINT_HEX(freesat->ucUserCustomizable);
		DxLIB_PRINT_HEX(freesat->ucUserSelectable);
		DxLIB_PRINT_HEX(freesat->ucAdultSvc);
		DxLIB_PRINT_HEX(freesat->ucGuidanceType);
		DxLIB_PRINT_HEX(freesat->ucGuidanceMode);
		DxLIB_PRINT_STR(freesat->szGuidanceStr);
	}
#endif


#ifdef	CONFIG_OP_PVR_BASED_ON_TVANYTIME
	{
		DxLIB_PRINT_HEX(t->ex.tva.ucGuidanceType);
		DxLIB_PRINT_HEX(t->ex.tva.ucGuidanceMode);
		DxLIB_PRINT_STR(t->ex.tva.szGuidanceStr);
		DxLIB_PRINT_HEX(t->ex.tva.ulFTAContentMgr);
		DxLIB_PRINT_STR(t->ex.tva.szDefaultAuth);
	}
#endif

#ifdef CONFIG_OP_SES
	{
		DxLIB_PRINT_INT(t->ex.ses.usVirtualSvcId);
//		DxLIB_PRINT_BOOL(t->ex.ses.bNewSvc);
	}
#endif

#ifdef	CONFIG_OP_ITALY_DBOOK
	{
		const DxItaDttService_t	*itaDtt = &(t->ex.itaDtt);
		DxLIB_PRINT_HEX(itaDtt->ucUserSelectable);
		DxLIB_PRINT_HEX(itaDtt->ucUserDeletedByUser);
	}
#endif

}



HCHAR *			DxSERVICE_Encode (HINT32 jsonWriter, const DxService_t *service)
{
	HINT32	writer;
	HCHAR	*json;

	if (jsonWriter)
		writer = jsonWriter;
	else
		writer = HLIB_JSONWRITER_Open();

	if (!dlib_svc_Encode(writer, service))
	{
		if (!jsonWriter)
			HLIB_JSONWRITER_Close(writer);
		return NULL;
	}

	if (jsonWriter)
		return NULL;

	json = DLIB_StrDup(HLIB_JSONWRITER_GetJSON(writer));
	HLIB_JSONWRITER_Close(writer);

	return json;
}

DxService_t *	DxSERVICE_Decode (DxService_t *service, HxJSON_t json)
{
	DxService_t	*s;

	if (service)
		s = service;
	else
		s = (DxService_t *)DLIB_Calloc(sizeof(DxService_t));

	if (!dlib_svc_Decode(json, s))
	{
		if (!service)
			DLIB_Free(s);
		return NULL;
	}
	return s;
}

DxService_t	*	DxSERVICE_Clone (const DxService_t *service)
{
	return (DxService_t *)HLIB_STD_MemDupEx(service, sizeof(DxService_t), DLIB_Malloc_CB);
}

void			DxSERVICE_Delete (DxService_t *service)
{
	DLIB_Free(service);
}


