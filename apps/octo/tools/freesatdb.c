
#include <hlib.h>
#include <dapi.h>
#include <dlib.h>
#include <clib.h>
#include <apk.h>

#ifndef	CONFIG_OP_FREESAT
//#define	CONFIG_OP_FREESAT
#endif


#define	FSAT_MWC_PARAM_AUDIO_LANG_MAX		2
#define	FSAT_MWC_PARAM_SUBTITLE_LANG_MAX	2
#define	FSAT_MWC_PARAM_FSAT_POSTCODE_MAX	9
#define	FSAT_MWC_MAX_PINCODE_LEN			32

typedef struct
{
	HUINT8	a;
	HUINT8	r;
	HUINT8	g;
	HUINT8	b;
} fsat_VIDEO_COLOR_t;

typedef struct
{
	HINT32				eOpType;
	HxLANG_Id_e		eMainLanguage;
	HxLANG_Id_e		eAudioLanguage[FSAT_MWC_PARAM_AUDIO_LANG_MAX];
	HxLANG_Id_e		eSubtitleLanguage[FSAT_MWC_PARAM_SUBTITLE_LANG_MAX];
	HxLANG_Id_e		eEventLanguage;

	HBOOL				bHardOfHearing;
	HBOOL				bAudioDescription;

	HINT32				eMaturityRating;
	HINT32				eAudioDOutMode;
	HINT32				eAudioDOutCodec;

	HUINT32				ulSkipForwardSec;
	HUINT32				ulInstantReplaySec;
	HINT32				eCountryIdx;

	HUINT32				ulPaddingTimeStart;
	HUINT32				ulPaddingTimeStop;
	HBOOL				bTsrOnOff;
	HBOOL				bAutoDelete;

	// 일반적으로 개별 서비스별로 동작한다거나
	// Language에 따른 Sound Mode가 자동 선택되지만... 메뉴값을 따를 경우를 위해서 존재한다.
	HINT32				eSysSoundMode;

	HBOOL				bInvisibleSearch;
	fsat_VIDEO_COLOR_t		stVideoBkColor;

	HCHAR				szPinCode[FSAT_MWC_MAX_PINCODE_LEN+1];

	HINT32				eAntConnectionType;
	HBOOL				bChannelSetupPIN;
	HINT32 				eGuidancePolicy;
	HBOOL				bSubtitleSetting;
//#if defined(CONFIG_OP_FREESAT)
	HCHAR				szPostcode[FSAT_MWC_PARAM_FSAT_POSTCODE_MAX];
	HINT32				eAdultChannels;
//#endif
} fsat_MWC_Param_t;


/**
 * 아래 두 파일은 oapi_types.h/oapi_types.c에서 아래의 변환을 함
 * - fsat_xyz.h/c로 이름 변경
 * - #include <header> 를 #include "header"로 변경
 * - DxXYZ를 모두 fsatDxXYZ로 변경
 */
#include "fsat/fsat_types.h"
#include "fsat/fsat_types.c"


static HBOOL	s_DEBUG;

static void dump_binary (FILE *fp, const HUINT8 *bin, HUINT32 size, HINT32 depth)
{
	HUINT8	line[16];
	HINT32	i, n;
	HINT32	len;
	HCHAR	tab[32];

	if (fp == NULL)
		fp = stdout;

	for (i = 0 ; i < 32 && i < depth ; i++)
		tab[i] = '\t';
	tab[i] = '\0';

	if (HLIB_JSON_IsJSON((const HCHAR *)bin, size))
	{
		fprintf(fp, "%s%s\n", tab, bin);
		return;
	}

	for (len = (HINT32)size ; len > 0 ;)
	{
		if (len > 16)
			n = 16;
		else
			n = len;

		memcpy(line, bin, n);
		len -= n;
		bin += n;

		fprintf(fp, "%s", tab);
		for (i = 0 ; i < 16 ; i++)
		{
			if (i < n)
				fprintf(fp, "%02X ", line[i]);
			else
				fprintf(fp, "   ");
		}
		fprintf(fp, "\t");
		for (i = 0 ; i < 16 ; i++)
		{
			if (i < n)
			{
				if (isprint(line[i]))
					fprintf(fp, "%c", line[i]);
				else
					fprintf(fp, ".");
			}
			else
				fprintf(fp, " ");
		}
		fprintf(fp, "\n");
	}
}

static void	process_MWCPARAM_set_int(HINT32 id, HINT32 value)
{
	DxDBSetupItem_t		*item;

	item = (DxDBSetupItem_t *)HLIB_STD_MemCalloc(sizeof(DxDBSetupItem_t));
	if (item)
	{
		item->uid = id;
		item->eItemType = eDxTYPE_INTEGER;
		item->nValue = value;
		if (!s_DEBUG)
			DAPI_Set(DxDBSETUP_SYSTEM, id, item, sizeof(DxDBSetupItem_t));
		else
			printf("\tid:0x%08X, value(i):%d(0x%X)\n", id, value, value);
		HLIB_STD_MemFree(item);
	}
}

static void	process_MWCPARAM_set_str(HINT32 id, const HCHAR *value)
{
	DxDBSetupItem_t		*item;

	item = (DxDBSetupItem_t *)HLIB_STD_MemCalloc(sizeof(DxDBSetupItem_t) + strlen(value) + 1);
	if (item)
	{
		item->uid = id;
		item->eItemType = eDxTYPE_STRING;
		item->nSize = strlen(value) + 1;
		HLIB_STD_StrNCpySafe(item->pucData, value, item->nSize);
		if (!s_DEBUG)
			DAPI_Set(DxDBSETUP_SYSTEM, id, item, item->nSize);
		else
			printf("\tid:0x%08X, value(s):%s\n", id, value);
		HLIB_STD_MemFree(item);
	}
}

static void	process_MWCPARAM_set_bin(HINT32 id, const void *value, HUINT32 size)
{
	DxDBSetupItem_t		*item;

	item = (DxDBSetupItem_t *)HLIB_STD_MemCalloc(sizeof(DxDBSetupItem_t) + size);
	if (item)
	{
		item->uid = id;
		item->eItemType = eDxTYPE_BINARY;
		item->nSize = size;
		memcpy(item->pucData, value, size);
		if (!s_DEBUG)
			DAPI_Set(DxDBSETUP_SYSTEM, id, item, item->nSize);
		else
		{
			printf("\tid:0x%08X, value(b):\n", id);
			dump_binary(stdout, value, size, 2);
		}
		HLIB_STD_MemFree(item);
	}
}

static int	process_MWCPARAM (const HCHAR *name, const HCHAR *path)
{
	void				*data;
	HUINT32				size;
	fsat_MWC_Param_t	*param;
	HINT32				i;

	if (HLIB_FILE_ReadFile((const HUINT8 *)path, &data, &size) != ERR_OK)
	{
		fprintf(stderr, "Failed to read file:%s!\n", path);
		return -1;
	}

	if (size != sizeof(fsat_MWC_Param_t))
	{
		fprintf(stderr, "Size mismatch! (file:%u, fsat_MWC_Param_t:%u)\n", size, sizeof(fsat_MWC_Param_t));
		return -1;
	}

	param = (fsat_MWC_Param_t *)data;

	DAPI_BatchBegin(DxDBSETUP_SYSTEM);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_OPERATOR, param->eOpType);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_MAIN_LANGUAGE, param->eMainLanguage);
		for (i = 0 ; i < FSAT_MWC_PARAM_AUDIO_LANG_MAX ; i++)
			process_MWCPARAM_set_int(eDxSETUP_SYSTEM_AUDIO_LANGUAGE + i, param->eAudioLanguage[i]);
		for (i = 0 ; i < FSAT_MWC_PARAM_SUBTITLE_LANG_MAX ; i++)
			process_MWCPARAM_set_int(eDxSETUP_SYSTEM_SUBTITLE_LANGUAGE + i, param->eSubtitleLanguage[i]);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_EVENT_LANGUAGE, param->eEventLanguage);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_HARDOFHEARING, param->bHardOfHearing);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_AUDIODESCRIPTION, param->bAudioDescription);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_MATURITYRATING, param->eMaturityRating);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_SKIP_FORWARD_SEC, param->ulSkipForwardSec);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_COUNTRY_INDEX, param->eCountryIdx);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_PADDING_START_TIME, param->ulPaddingTimeStart);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_PADDING_END_TIME, param->ulPaddingTimeStop);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_TSR_ON_OFF, param->bTsrOnOff);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_AUTO_DELETE, param->bAutoDelete);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_STEREO_SELECT, param->eSysSoundMode);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_INVISIBLE_SEARCH, param->bInvisibleSearch);
		process_MWCPARAM_set_bin(eDxSETUP_SYSTEM_VIDEO_BK_COLOR, &(param->stVideoBkColor), sizeof(fsat_VIDEO_COLOR_t));
		process_MWCPARAM_set_str(eDxSETUP_SECURE_SYSTEM_PINCODE, param->szPinCode);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_ANTENNA_CONNECTION, param->eAntConnectionType);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_CHANNEL_SETUP_PIN, param->bChannelSetupPIN);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_GUIDANCE_POLICY, param->eGuidancePolicy);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_SUBTITLE_SETTING, param->bSubtitleSetting);
#if defined(CONFIG_OP_FREESAT)
		process_MWCPARAM_set_str(eDxSETUP_SYSTEM_FREESAT_POSTCODE, param->szPostcode);
		process_MWCPARAM_set_int(eDxSETUP_SYSTEM_FREESAT_ADULTCHANNELS, param->eAdultChannels);
#endif
	DAPI_BatchEnd(DxDBSETUP_SYSTEM);

	HLIB_STD_MemFree(data);
	return 0;
}

static int	process_UDBMODULE (const HCHAR *name, const HCHAR *path)
{
	void	*data;
	HUINT32	size;

	if (HLIB_FILE_ReadFile((const HUINT8 *)path, &data, &size) != ERR_OK)
	{
		fprintf(stderr, "Failed to read file:%s!\n", path);
		return -1;
	}

	if (!s_DEBUG)
		DAPI_Set(name,0/*eUDBGROUP_MAIN*/, data, size);
	else
	{
		dump_binary(stdout, data, size, 1);
	}
	HLIB_STD_MemFree(data);

	return 0;
}

static void	convert_DxAntInfo_t (DxAntInfo_t *dst, const fsatDxAntInfo_t *src)
{
	memset(dst, 0, sizeof(DxAntInfo_t));

	dst->uid = src->uid;
	HLIB_STD_StrNCpySafe(dst->satType, src->satType, DxANTINFO_NAME_LEN);
	HLIB_STD_StrNCpySafe(dst->satName, src->satName, DxANTINFO_NAME_LEN);
	dst->antType = src->antType;

	switch (dst->antType)
	{
	case eDxANT_TYPE_LNB_ONLY:
		memcpy(&(dst->info.lnb), &(src->info.lnb), sizeof(DxLnbAntenna_t));
		break;

	case eDxANT_TYPE_DISEQC:
		memcpy(&(dst->info.diseqc), &(src->info.diseqc), sizeof(DxDiseqcAntenna_t));
		break;

	case eDxANT_TYPE_SCD:
		dst->info.scd.ulLnbFreq = src->info.scd.ulLnbFreq;
		dst->info.scd.lnbVoltage = src->info.scd.lnbVoltage;
		dst->info.scd.b22kTone = src->info.scd.b22kTone;
		dst->info.scd.scdInput = src->info.scd.scdInput;
		dst->info.scd.ulScdUserBandNum = src->info.scd.ulScdUserBandNum;
		dst->info.scd.ulScdUserBandFreq = src->info.scd.ulScdUserBandFreq;
		dst->info.scd.ulScdUserBandNum1 = src->info.scd.ulScdUserBandNum1;
		dst->info.scd.ulScdUserBandFreq1 = src->info.scd.ulScdUserBandFreq1;
		break;

	default:
		break;
	}
}

static int	process_DxAntInfo_t (const HCHAR *name, const HCHAR *path)
{
	void		*data;
	HUINT32		size;
	HUINT32		n;
	fsatDxAntInfo_t	*fsat;
	DxAntInfo_t			ant;

	if (HLIB_FILE_ReadFile((const HUINT8 *)path, &data, &size) != ERR_OK)
	{
		fprintf(stderr, "Failed to read file:%s!\n", path);
		return -1;
	}

	if (size % sizeof(fsatDxAntInfo_t))
	{
		fprintf(stderr, "Invalid data size:%u(fsatDxAntInfo_t:%u)\n", size, sizeof(fsatDxAntInfo_t));
		HLIB_STD_MemFree(data);
		return -1;
	}

	n = size / sizeof(fsatDxAntInfo_t);
	for (fsat = (fsatDxAntInfo_t *)data ; n-- ; fsat++)
	{
		convert_DxAntInfo_t(&ant, fsat);

		if (!s_DEBUG)
			DAPI_Set(name, ant.uid, &ant, sizeof(DxAntInfo_t));
		else
			DLIB_Print(name, stdout, &ant);
	}
	HLIB_STD_MemFree(data);
	return 0;
}


static int	process_DxService_t (const HCHAR *name, const HCHAR *path)
{
	HINT32		n;
	void		*data;
	HUINT8		*list;
	HUINT32		size;
	HUINT32		retsize;
	fsatDxService_t	*fsat;
	DxService_t		item;

	if (HLIB_FILE_ReadFile((const HUINT8 *)path, &data, &size) != ERR_OK)
	{
		fprintf(stderr, "Failed to read file:%s!\n", path);
		return -1;
	}

	list = (HUINT8 *)fsatDxXXX_DecodeFile(fsatDxNAMEOF(fsatDxService_t), data, &retsize);
	HLIB_STD_MemFree(data);

	if (!list)
	{
		fprintf(stderr, "Failed to decode json file(%s)\n", path);
		return -1;
	}

	n = retsize / sizeof(fsatDxService_t);
	DAPI_BatchBegin(name);
	for (fsat = (fsatDxService_t *)list ; n-- ; fsat++)
	{
		memset(&item, 0, sizeof(DxService_t));

		item.uid = fsat->uid;
		item.version = fsat->version;
		item.tsuid = fsat->tsuid;
		item.prvuid = fsat->prvuid;
		item.antuid = fsat->antuid;
		HLIB_STD_MemCpySafe(item.grpuids, sizeof(HINT32) * DxGROUP_LEN, fsat->grpuids, sizeof(HINT32) * fsatDxGROUP_LEN);
		HLIB_STD_MemCpySafe(item.bqtuids, sizeof(HINT32) * DxBOUQUET_LEN, fsat->bqtuids, sizeof(HINT32) * fsatDxBOUQUET_LEN);
		item.svcid = fsat->svcid;
		item.tsid = fsat->tsid;
		item.onid = fsat->onid;
		item.lcn = fsat->lcn;
		HLIB_STD_StrNCpySafe(item.name, fsat->name, DxNAME_LEN);
		item.svcType = fsat->svcType;
		item.orgSvcType = fsat->orgSvcType;
		item.optype = fsat->optype;
		item.deliType = fsat->deliType;
		HLIB_STD_StrNCpySafe(item.satType, fsat->satType, DxANTINFO_NAME_LEN);
		item.visibleFlag = fsat->visibleFlag;
		item.locked = fsat->locked;
		item.removed = fsat->removed;
		item.pmtPid = fsat->pmtPid;

		item.pmtPid = fsat->pmtPid;
		item.pcrPid = fsat->pcrPid;
		item.videoPid = fsat->videoPid;
		item.audioPid = fsat->audioPid;
		item.audioAuxPid = fsat->audioAuxPid;
		item.dolbyPid = fsat->dolbyPid;
		item.highDefinition = fsat->highDefinition;
		item.casType = fsat->casType;
		item.dolbyFlag = fsat->dolbyFlag;
		item.videoCodec = fsat->videoCodec;
		item.audioCodec = fsat->audioCodec;
		item.audioAuxCodec = fsat->audioAuxCodec;
		item.dolbyCodec = fsat->dolbyCodec;
		item.lcnFlag = fsat->lcnFlag;

		item.updateSvcFlag = fsat->updateSvcFlag;
#if defined(CONFIG_OP_FREESAT)
		memcpy(&(item.ex.freesat), &(fsat->ex.freesat), sizeof(DxFreesatService_t));
#endif
		item.renamed = FALSE;
		item.typeOf = DxNAMEOF(DxService_t);

		if (!s_DEBUG)
			DAPI_Set(name, item.uid, &item, sizeof(DxService_t));
		else
			DLIB_Print(name, stdout, &item);
	}
	DAPI_BatchEnd(name);
	HLIB_STD_MemFree(list);

	return 0;
}


static int	process_DxGroup_t (const HCHAR *name, const HCHAR *path)
{
	HINT32		n;
	void		*data;
	HUINT8		*list;
	HUINT32		size;
	HUINT32		retsize;
	fsatDxGroup_t	*fsat;
	DxGroup_t		item;

	if (HLIB_FILE_ReadFile((const HUINT8 *)path, &data, &size) != ERR_OK)
	{
		fprintf(stderr, "Failed to read file:%s!\n", path);
		return -1;
	}

	list = (HUINT8 *)fsatDxXXX_DecodeFile(fsatDxNAMEOF(fsatDxGroup_t), data, &retsize);
	HLIB_STD_MemFree(data);

	if (!list)
	{
		fprintf(stderr, "Failed to decode json file(%s)\n", path);
		return -1;
	}

	n = retsize / sizeof(fsatDxGroup_t);
	DAPI_BatchBegin(name);
	for (fsat = (fsatDxGroup_t *)list ; n-- ; fsat++)
	{
		memset(&item, 0, sizeof(DxGroup_t));

		item.typeOf = DxNAMEOF(DxGroup_t);
		item.uid = fsat->uid;
		item.version = fsat->version;
		item.id = fsat->id;
		HLIB_STD_StrNCpySafe(item.name, fsat->name, DxNAME_LEN);
#if defined(CONFIG_OP_FREESAT)
		memcpy(&(item.ex.freesat), &(fsat->ex.freesat), sizeof(DxFreesatGroup_t));
#endif
		if (!s_DEBUG)
			DAPI_Set(name, item.uid, &item, sizeof(DxGroup_t));
		else
			DLIB_Print(name, stdout, &item);
	}
	DAPI_BatchEnd(name);
	HLIB_STD_MemFree(list);

	return 0;
}


static int	process_DxNetwork_t (const HCHAR *name, const HCHAR *path)
{
	HINT32		n;
	void		*data;
	HUINT8		*list;
	HUINT32		size;
	HUINT32		retsize;
	fsatDxNetwork_t	*fsat;
	DxNetwork_t		item;

	if (HLIB_FILE_ReadFile((const HUINT8 *)path, &data, &size) != ERR_OK)
	{
		fprintf(stderr, "Failed to read file:%s!\n", path);
		return -1;
	}

	list = (HUINT8 *)fsatDxXXX_DecodeFile(fsatDxNAMEOF(fsatDxNetwork_t), data, &retsize);
	HLIB_STD_MemFree(data);

	if (!list)
	{
		fprintf(stderr, "Failed to decode json file(%s)\n", path);
		return -1;
	}

	n = retsize / sizeof(fsatDxNetwork_t);
	DAPI_BatchBegin(name);
	for (fsat = (fsatDxNetwork_t *)list ; n-- ; fsat++)
	{
		memset(&item, 0, sizeof(DxNetwork_t));

		item.typeOf = DxNAMEOF(DxNetwork_t);
		item.uid = fsat->uid;
		item.version = fsat->version;
		item.onid = fsat->onid;
		HLIB_STD_StrNCpySafe(item.name, fsat->name, DxNAME_LEN);
		item.deliType = fsat->deliType;
#if defined(CONFIG_OP_FREESAT)
		memcpy(&(item.ex.freesat), &(fsat->ex.freesat), sizeof(DxFreesatNet_t));
#endif
		if (!s_DEBUG)
			DAPI_Set(name, item.uid, &item, sizeof(DxNetwork_t));
		else
			DLIB_Print(name, stdout, &item);
	}
	DAPI_BatchEnd(name);
	HLIB_STD_MemFree(list);

	return 0;
}


static int	process_DxProvider_t (const HCHAR *name, const HCHAR *path)
{
	HINT32		n;
	void		*data;
	HUINT8		*list;
	HUINT32		size;
	HUINT32		retsize;
	fsatDxProvider_t	*fsat;
	DxProvider_t		item;

	if (HLIB_FILE_ReadFile((const HUINT8 *)path, &data, &size) != ERR_OK)
	{
		fprintf(stderr, "Failed to read file:%s!\n", path);
		return -1;
	}

	list = (HUINT8 *)fsatDxXXX_DecodeFile(fsatDxNAMEOF(fsatDxProvider_t), data, &retsize);
	HLIB_STD_MemFree(data);

	if (!list)
	{
		fprintf(stderr, "Failed to decode json file(%s)\n", path);
		return -1;
	}

	n = retsize / sizeof(fsatDxProvider_t);
	DAPI_BatchBegin(name);
	for (fsat = (fsatDxProvider_t *)list ; n-- ; fsat++)
	{
		memset(&item, 0, sizeof(DxProvider_t));

		item.typeOf = DxNAMEOF(DxProvider_t);
		item.uid = fsat->uid;
		item.version = fsat->version;
		HLIB_STD_StrNCpySafe(item.name, fsat->name, DxNAME_LEN);

		if (!s_DEBUG)
			DAPI_Set(name, item.uid, &item, sizeof(DxProvider_t));
		else
			DLIB_Print(name, stdout, &item);
	}
	DAPI_BatchEnd(name);
	HLIB_STD_MemFree(list);

	return 0;
}



static int	process_DxTransponder_t (const HCHAR *name, const HCHAR *path)
{
	HINT32		n;
	void		*data;
	HUINT8		*list;
	HUINT32		size;
	HUINT32		retsize;
	fsatDxTransponder_t	*fsat;
	DxTransponder_t		item;

	if (HLIB_FILE_ReadFile((const HUINT8 *)path, &data, &size) != ERR_OK)
	{
		fprintf(stderr, "Failed to read file:%s!\n", path);
		return -1;
	}

	list = (HUINT8 *)fsatDxXXX_DecodeFile(fsatDxNAMEOF(fsatDxTransponder_t), data, &retsize);
	HLIB_STD_MemFree(data);

	if (!list)
	{
		fprintf(stderr, "Failed to decode json file(%s)\n", path);
		return -1;
	}

	n = retsize / sizeof(fsatDxTransponder_t);
	DAPI_BatchBegin(name);
	for (fsat = (fsatDxTransponder_t *)list ; n-- ; fsat++)
	{
		memset(&item, 0, sizeof(DxTransponder_t));

		item.typeOf = DxNAMEOF(DxTransponder_t);
		item.uid = fsat->uid;
		item.version = fsat->version;
		item.netuid = fsat->netuid;
		item.tsid = fsat->tsid;
		item.onid = fsat->onid;
		item.tunerid = fsat->tunerid;
		item.deliType = fsat->deliType;
		switch (fsat->tuningParam.eDeliType)
		{
		case efsatDxDELIVERY_SAT:
		{
			fsatDxSatTuningInfo_t *fsattune = &(fsat->tuningParam.sat);
			DxSatTuningInfo_t *tune = &(item.tuningParam.sat);

			tune->deliveryType = fsattune->deliveryType;
			tune->ulTunerGroupId = fsattune->ulTunerGroupId;
			tune->antennaType = fsattune->antennaType;
			HLIB_STD_MemCpySafe(&(tune->tuningInfo), sizeof(DxSat_TuningTPInfo_t), &(fsattune->tuningInfo), sizeof(fsatDxSat_TuningTPInfo_t));
			convert_DxAntInfo_t(&(tune->antInfo), &(fsattune->antInfo));
			break;
		}

		default:	// freesat은 SAT만!!
			break;
		}

		if (!s_DEBUG)
			DAPI_Set(name, item.uid, &item, sizeof(DxTransponder_t));
		else
			DLIB_Print(name, stdout, &item);
	}
	DAPI_BatchEnd(name);
	HLIB_STD_MemFree(list);

	return 0;
}


static int process_dlogger (const HCHAR *dlogger_path)
{
	HxJSON_t	dlogger;
	void		*iter;
	HINT32		ret;
	HINT32		success = 0;
	HCHAR		*file;
	HCHAR		*dir;
	HCHAR		*path;

	dlogger = HLIB_JSON_DecodeFile(dlogger_path);
	if (!dlogger)
	{
		fprintf(stderr, "Failed to decode json file(%s)\n", dlogger_path);
		return -1;
	}

	file = (HCHAR *)HLIB_STD_MemAlloc(HxPATH_MAX);
	dir  = (HCHAR *)HLIB_STD_MemAlloc(HxPATH_MAX);

	HLIB_PATH_DirName(dlogger_path, dir, HxPATH_MAX);

	// dlogger.json 파일에 적혀 있는 내용을 기준으로 .dat 파일들을 분석한다.
	for ( iter = HLIB_JSON_Object_Iterator(dlogger)
		; iter
		; iter = HLIB_JSON_Object_NextIterator(dlogger, iter) )
	{
		const HCHAR *key = HLIB_JSON_Object_IteratorKey(iter);

		HxSTD_PrintToStrN(file, HxPATH_MAX, "%s-0.dat", key);

		path = HLIB_PATH_JoinPathes(dir, "dlogger", file, NULL);
		if (path == NULL)
			continue;

		printf(HxANSI_COLOR_YELLOW(">>> %s")":\'%s\'\n", key, path);
		if (!strcmp(key, "MWCPARAM"))
		{
			ret = process_MWCPARAM(key, path);
		}
		else if (!strcmp(key, "UDBMODULE"))
		{
			ret = process_UDBMODULE(key, path);
		}
		else if (!strcmp(key, "DxAntInfo_t"))
		{
			ret = process_DxAntInfo_t(key, path);
		}
		else if (!strcmp(key, "DxGroup_t"))
		{
			ret = process_DxGroup_t(key, path);
		}
		else if (!strcmp(key, "DxNetwork_t"))
		{
			ret = process_DxNetwork_t(key, path);
		}
		else if (!strcmp(key, "DxProvider_t"))
		{
			ret = process_DxProvider_t(key, path);
		}
		else if (!strcmp(key, "DxService_t"))
		{
			ret = process_DxService_t(key, path);
		}
		else if (!strcmp(key, "DxTransponder_t"))
		{
			ret = process_DxTransponder_t(key, path);
		}
		else
		{
			ret = -1;
		}
		HLIB_PATH_MemFree(path);

		if (ret == 0)
			success++;
	}

	HLIB_STD_MemFree(file);
	HLIB_STD_MemFree(dir);
	HLIB_JSON_Delete(dlogger);

	printf("success:%d\n", success);
	return success;
}

int freesatdb (int argc, char *argv[])
{
	while (argc--)
	{
		/**
		 * $freesatdb /var/lib/humaxtv/dlogger.json /var/lib/humaxtv_backup/dlogger.json
		 * 위와 같이 명령을 내린 경우,
		 * /var/lib/humaxtv/dlogger.json 을 처리해 보고, 만약 실패하면, 다음 인자를 처리하는 식..
		 */
		if (process_dlogger(*argv++) > 0)
			return 0;
	}
	return -1;
}


int main(int argc, char *argv[])
{
	if (argc == 1 || HLIB_STD_StrCmpV(HxSTD_StrCmp, argv[1], "-h", "--help", NULL) >= 0)
	{
		goto HELP;
	}
	argc--;
	argv++;

	VK_Init();
	HLIB_DBG_Init();
	DLIB_Init();

	/**
	 * freesatdb -d /var/lib/humaxtv/dlogger.json 이런식으로 명령을 내리면
	 * 실제 DAPI_Set을 하지 않고, 동작에 대한 로그만 뿌려요.
	 * 디버깅 하실 수 있어요.
	 */
	if (HLIB_STD_StrCmpV(HxSTD_StrCmp, *argv, "-d", "-D", "--debug", NULL) >= 0)
	{
		argc--;
		argv++;
		s_DEBUG = TRUE;
	}
	return freesatdb(argc, argv);

HELP:
	printf(
		"USAGE: %s [OPTION] [PATH...]\n"
		"       -h    help\n"
		"       \uCF54\uB4DC\uC5D0 \uC8FC\uC11D \uB9CE\uC774 \uB2EC\uC544\uB1A8\uC5B4\uC694.  \uCF54\uB4DC \uBCF4\uC154\uC694.\n"
		"     example\n"
		"       $%s /var/lib/humaxtv/dlogger.json\n"
		"       $%s /var/lib/humaxtv/dlogger.json /var/lib/humaxtv_backup/dlogger.json\n"
		, *argv
		, *argv
		, *argv
	);
	return 0;
}

