
#include <hlib.h>
#include "_dlib_port_antinfo.h"

static	DxEnumStringTable_t	_stANTLNBVoltTable =
{
	(HCHAR*)"DxSAT_LnbVoltage_e",
	{
		{	DxNAMEOF(eDxLNB_VOLT_STD),		eDxLNB_VOLT_STD 	},
		{	DxNAMEOF(eDxLNB_VOLT_HIGH), 	eDxLNB_VOLT_HIGH	},
		{	NULL,													0									}
	}
};

static	DxEnumStringTable_t	_stANTDiSEqCVerTable =
{
	(HCHAR*)"DxSAT_DiseqcVer_e",
	{
		{	DxNAMEOF(eDxDISEQC_VER_1_0),	eDxDISEQC_VER_1_0	},
		{	DxNAMEOF(eDxDISEQC_VER_1_1),	eDxDISEQC_VER_1_1	},
		{	DxNAMEOF(eDxDISEQC_VER_1_2),	eDxDISEQC_VER_1_2	},
		{	DxNAMEOF(eDxDISEQC_VER_1_3),	eDxDISEQC_VER_1_3	},
		{	DxNAMEOF(eDxDISEQC_VER_2_0),	eDxDISEQC_VER_2_0	},
		{	NULL,							0					}
	}
};

static	DxEnumStringTable_t	_stANTDiSEqCInputTable =
{
	(HCHAR*)"DxSAT_DiseqcInput_e",
	{
		{	DxNAMEOF(eDxDISEQC_INPUT_NONE), eDxDISEQC_INPUT_NONE	},
		{	DxNAMEOF(eDxDISEQC_INPUT_A),	eDxDISEQC_INPUT_A	},
		{	DxNAMEOF(eDxDISEQC_INPUT_B),	eDxDISEQC_INPUT_B	},
		{	DxNAMEOF(eDxDISEQC_INPUT_C),	eDxDISEQC_INPUT_C	},
		{	DxNAMEOF(eDxDISEQC_INPUT_D),	eDxDISEQC_INPUT_D	},
		{	NULL,							0					}
	}
};

static	DxEnumStringTable_t	_stANTSCDInputTable =
{
	(HCHAR*)"DxSAT_ScdInput_e",
	{
		{	DxNAMEOF(eDxSCD_INPUT_NONE),	eDxSCD_INPUT_NONE	},
		{	DxNAMEOF(eDxSCD_INPUT_SCD_A),	eDxSCD_INPUT_SCD_A	},
		{	DxNAMEOF(eDxSCD_INPUT_SCD_B),	eDxSCD_INPUT_SCD_B	},
		{	NULL,							0					}
	}
};

static	DxEnumStringTable_t	_stANTToneTable =
{
	(HCHAR*)"DxSAT_ToneBurst_e",
	{
		{	DxNAMEOF(eDxTONE_BURST_NONE),	eDxTONE_BURST_NONE	},
		{	DxNAMEOF(eDxTONE_BURST_A),		eDxTONE_BURST_A	},
		{	DxNAMEOF(eDxTONE_BURST_B),		eDxTONE_BURST_B	},
		{	NULL,							0					}
	}
};

static HERROR	dlib_antinfo_Encoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HINT32			jw;
	DxAntInfo_t		*pstInfo = (DxAntInfo_t*)src;

	HxLOG_Assert(sizeof(DxAntInfo_t) == srcSize);

	jw = HLIB_JSONWRITER_Open();
	HxJSONWRITER_ObjectBegin(jw);
		HxJSONWRITER_Integer(jw, "uid", pstInfo->uid);
		HxJSONWRITER_String (jw, "satType", pstInfo->satType);
		HxJSONWRITER_String (jw, "satName", pstInfo->satName);
		HxJSONWRITER_String (jw, "antType", dlib_ConvertEnum2Str(DxNAMEOF(DxAntennaType_e),pstInfo->antType));
		switch (pstInfo->antType)
		{
			case eDxANT_TYPE_LNB_ONLY:
				{
					HxJSONWRITER_Object(jw, "lnbonly");
					HxJSONWRITER_Integer(jw, "ulLnbFreq", pstInfo->info.lnb.ulLnbFreq);
					HxJSONWRITER_String (jw, "lnbVoltage", dlib_ConvertEnum2Str(DxNAMEOF(DxSAT_LnbVoltage_e), pstInfo->info.lnb.lnbVoltage));
					HxJSONWRITER_ObjectEnd(jw);
				}
				break;
			case eDxANT_TYPE_DISEQC:
				{
					HxJSONWRITER_Object(jw, "diseqc");
					HxJSONWRITER_Integer(jw, "ulLnbFreq", pstInfo->info.diseqc.ulLnbFreq);
					HxJSONWRITER_String (jw, "lnbVoltage", dlib_ConvertEnum2Str(DxNAMEOF(DxSAT_LnbVoltage_e), pstInfo->info.diseqc.lnbVoltage));
					HxJSONWRITER_Boolean(jw, "b22kTone", pstInfo->info.diseqc.b22kTone);
					HxJSONWRITER_String (jw, "diseqcVersion", dlib_ConvertEnum2Str(DxNAMEOF(DxSAT_DiseqcVer_e), pstInfo->info.diseqc.diseqcVersion));
					HxJSONWRITER_String (jw, "diseqcInput", dlib_ConvertEnum2Str(DxNAMEOF(DxSAT_DiseqcInput_e), pstInfo->info.diseqc.diseqcInput));
					HxJSONWRITER_String (jw, "toneBurst", dlib_ConvertEnum2Str(DxNAMEOF(DxSAT_ToneBurst_e), pstInfo->info.diseqc.toneBurst));
					HxJSONWRITER_Integer(jw, "ucMotorPosition", pstInfo->info.diseqc.ucMotorPosition);
					HxJSONWRITER_Integer(jw, "sSatLonggitude", pstInfo->info.diseqc.sSatLonggitude);
					HxJSONWRITER_ObjectEnd(jw);
				}
				break;
			case eDxANT_TYPE_SCD:
				{
					HxJSONWRITER_Object(jw, "scd");
					HxJSONWRITER_Integer(jw, "ulLnbFreq", pstInfo->info.scd.ulLnbFreq);
					HxJSONWRITER_String(jw, "lnbVoltage", dlib_ConvertEnum2Str(DxNAMEOF(DxSAT_LnbVoltage_e), pstInfo->info.scd.lnbVoltage));
					HxJSONWRITER_Boolean(jw, "b22kTone", pstInfo->info.scd.b22kTone);
					HxJSONWRITER_String(jw, "scdInput", dlib_ConvertEnum2Str(DxNAMEOF(DxSAT_ScdInput_e), pstInfo->info.scd.scdInput));
					HxJSONWRITER_Integer(jw, "ulScdUserBandNum", pstInfo->info.scd.ulScdUserBandNum);
					HxJSONWRITER_Integer(jw, "ulScdUserBandFreq", pstInfo->info.scd.ulScdUserBandFreq);
					HxJSONWRITER_Integer(jw, "ulScdUserBandNum1", pstInfo->info.scd.ulScdUserBandNum1);
					HxJSONWRITER_Integer(jw, "ulScdUserBandFreq1", pstInfo->info.scd.ulScdUserBandFreq1);
					HxJSONWRITER_Integer(jw, "ulScdUserBandNum2", pstInfo->info.scd.ulScdUserBandNum2); // MOON_SCD
					HxJSONWRITER_Integer(jw, "ulScdUserBandFreq2", pstInfo->info.scd.ulScdUserBandFreq2);
					HxJSONWRITER_Integer(jw, "ulScdUserBandNum3", pstInfo->info.scd.ulScdUserBandNum3);
					HxJSONWRITER_Integer(jw, "ulScdUserBandFreq3", pstInfo->info.scd.ulScdUserBandFreq3);

					HxJSONWRITER_ObjectEnd(jw);
				}
				break;
			case eDxANT_TYPE_SMATV:
			case eDxANT_TYPE_MONOBLOCK_LNB:
			default:
				HxLOG_Critical("Not supported Antenna Type [%s]\n", dlib_ConvertEnum2Str(DxNAMEOF(DxAntennaType_e), pstInfo->antType));
				break;
		}
	HxJSONWRITER_ObjectEnd(jw);

	if (HLIB_JSONWRITER_Error(jw))
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

static HERROR	dlib_antinfo_Decoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HxJSON_t		root;
	DxAntInfo_t		*pstInfo;

	HxLOG_Assert(srcSize && src);

	root = HLIB_JSON_Decode(src);
	HxLOG_Assert(root);

	pstInfo = (DxAntInfo_t*)DLIB_Calloc(sizeof(DxAntInfo_t));
	pstInfo->uid = HLIB_JSON_Object_GetInteger(root, "uid");
	HxSTD_StrNCpy(pstInfo->satType, HLIB_JSON_Object_GetString(root, "satType"), DxANTINFO_NAME_LEN - 1);
	HxSTD_StrNCpy(pstInfo->satName, HLIB_JSON_Object_GetString(root, "satName"), DxANTINFO_NAME_LEN - 1);
	pstInfo->antType = dlib_ConvertStr2Enum(DxNAMEOF(DxAntennaType_e), HLIB_JSON_Object_GetString(root, "antType"));
	switch (pstInfo->antType)
	{
		case eDxANT_TYPE_LNB_ONLY:
			{
				HxJSON_t	lnbonly;

				lnbonly = HxJSON_Object_Get(root, "lnbonly");
				pstInfo->info.lnb.ulLnbFreq = HLIB_JSON_Object_GetInteger(lnbonly, "ulLnbFreq");
				pstInfo->info.lnb.lnbVoltage = dlib_ConvertStr2Enum(DxNAMEOF(DxSAT_LnbVoltage_e), HLIB_JSON_Object_GetString(lnbonly, "lnbVoltage"));
			}
			break;
		case eDxANT_TYPE_DISEQC:
			{
				HxJSON_t	diseqc;

				diseqc = HxJSON_Object_Get(root, "diseqc");
				pstInfo->info.diseqc.ulLnbFreq = HLIB_JSON_Object_GetInteger(diseqc, "ulLnbFreq");
				pstInfo->info.diseqc.lnbVoltage = dlib_ConvertStr2Enum(DxNAMEOF(DxSAT_LnbVoltage_e), HLIB_JSON_Object_GetString(diseqc, "lnbVoltage"));
				pstInfo->info.diseqc.b22kTone = HLIB_JSON_Object_GetBoolean(diseqc, "b22kTone");
				pstInfo->info.diseqc.diseqcVersion = dlib_ConvertStr2Enum(DxNAMEOF(DxSAT_DiseqcVer_e), HLIB_JSON_Object_GetString(diseqc, "diseqcVersion"));
				pstInfo->info.diseqc.diseqcInput = dlib_ConvertStr2Enum(DxNAMEOF(DxSAT_DiseqcInput_e), HLIB_JSON_Object_GetString(diseqc, "diseqcInput"));
				pstInfo->info.diseqc.toneBurst = dlib_ConvertStr2Enum(DxNAMEOF(DxSAT_ToneBurst_e), HLIB_JSON_Object_GetString(diseqc, "toneBurst"));
				pstInfo->info.diseqc.ucMotorPosition = HLIB_JSON_Object_GetInteger(diseqc, "ucMotorPosition");
				pstInfo->info.diseqc.sSatLonggitude = HLIB_JSON_Object_GetInteger(diseqc, "sSatLonggitude");
			}
			break;
		case eDxANT_TYPE_SCD:
			{
				HxJSON_t	scd;

				scd = HxJSON_Object_Get(root, "scd");
				pstInfo->info.scd.ulLnbFreq = HLIB_JSON_Object_GetInteger(scd, "ulLnbFreq");
				pstInfo->info.scd.lnbVoltage = dlib_ConvertStr2Enum(DxNAMEOF(DxSAT_LnbVoltage_e), HLIB_JSON_Object_GetString(scd, "lnbVoltage"));
				pstInfo->info.scd.b22kTone = HLIB_JSON_Object_GetBoolean(scd, "b22kTone");
				pstInfo->info.scd.scdInput = dlib_ConvertStr2Enum(DxNAMEOF(DxSAT_ScdInput_e), HLIB_JSON_Object_GetString(scd, "scdInput"));
				pstInfo->info.scd.ulScdUserBandNum = HLIB_JSON_Object_GetInteger(scd, "ulScdUserBandNum");
				pstInfo->info.scd.ulScdUserBandFreq = HLIB_JSON_Object_GetInteger(scd, "ulScdUserBandFreq");
				pstInfo->info.scd.ulScdUserBandNum1 = HLIB_JSON_Object_GetInteger(scd, "ulScdUserBandNum1");
				pstInfo->info.scd.ulScdUserBandFreq1 = HLIB_JSON_Object_GetInteger(scd, "ulScdUserBandFreq1");
				pstInfo->info.scd.ulScdUserBandNum2 = HLIB_JSON_Object_GetInteger(scd, "ulScdUserBandNum2"); // MOON_SCD
				pstInfo->info.scd.ulScdUserBandFreq2 = HLIB_JSON_Object_GetInteger(scd, "ulScdUserBandFreq2");
				pstInfo->info.scd.ulScdUserBandNum3 = HLIB_JSON_Object_GetInteger(scd, "ulScdUserBandNum3");
				pstInfo->info.scd.ulScdUserBandFreq3 = HLIB_JSON_Object_GetInteger(scd, "ulScdUserBandFreq3");
			}
			break;
		case eDxANT_TYPE_SMATV:
		case eDxANT_TYPE_MONOBLOCK_LNB:
		default:
			HxLOG_Critical("Not supported Antenna Type [%s]\n", dlib_ConvertEnum2Str(DxNAMEOF(DxAntennaType_e), pstInfo->antType));
			break;
	}

	HLIB_JSON_Delete(root);

	*dst = (void*)pstInfo;
	*dstSize = sizeof(DxAntInfo_t);

	return ERR_OK;
}

void			DLIB_ANTINFO_Init(void)
{
	dlib_RegisterEnumTable(&_stANTLNBVoltTable);
	dlib_RegisterEnumTable(&_stANTDiSEqCVerTable);
	dlib_RegisterEnumTable(&_stANTDiSEqCInputTable);
	dlib_RegisterEnumTable(&_stANTSCDInputTable);
	dlib_RegisterEnumTable(&_stANTToneTable);

	dlib_RegisterDECModule(DxNAMEOF(DxAntInfo_t), dlib_antinfo_Encoder, dlib_antinfo_Decoder);
}

HINT32			DLIB_ANTINFO_GetFieldCount (void)
{
	return 0;
}

DxField_t *		DLIB_ANTINFO_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	return NULL;
}

HERROR			DLIB_ANTINFO_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_antinfo_Encoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_ANTINFO_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_antinfo_Decoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_ANTINFO_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 데이터 전송 시, 효율을 높이기 위해 압축이 필요한 경우 이 함수 호출됨
	// 압축 방식은 진짜 압축이 아닌, 그냥 string을 binary로 바꾸는 등
	// 아무튼 데이터를 줄이기만 하면 됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}

HERROR			DLIB_ANTINFO_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 압축된 데이터 원복
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}


HERROR			DLIB_ANTINFO_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	return ERR_FAIL;
}

void			DLIB_ANTINFO_FreeType (void *type)
{
}

void			DLIB_ANTINFO_Print (FILE *fp, const void *v)
{
	DxAntInfo_t *t = (DxAntInfo_t *)v;

	fprintf(fp, "DxAntInfo_t[%d] %s.%s\n"
		, t->uid
		, t->satType
		, t->satName
	);

	switch (t->antType)
	{
	case eDxANT_TYPE_LNB_ONLY:
		fprintf(fp
			, "\tLNB Only\n"
			  "\t    frequency:%u\n"
			  "\t  LNB Voltage:%s\n"
			, t->info.lnb.ulLnbFreq
			, dlib_ConvertEnum2Str(DxNAMEOF(DxSAT_LnbVoltage_e), t->info.lnb.lnbVoltage)
		);
		break;

	case eDxANT_TYPE_DISEQC:
		fprintf(fp,
			"\tDiSEqC\n"
			"\t    frequency:%u\n"
			"\t  LNB Voltage:%s\n"
			"\t   22Khz Tone:%s\n"
			"\t      Version:%s\n"
			"\t        Input:%s\n"
			"\t   Tone burst:%s\n"
			"\t    Motor Pos:%u\n"
			"\t   Longgitude:%d\n"
			, t->info.diseqc.ulLnbFreq
			, dlib_ConvertEnum2Str(DxNAMEOF(DxSAT_LnbVoltage_e), t->info.diseqc.lnbVoltage)
			, t->info.diseqc.b22kTone ? "ON" : "OFF"
			, dlib_ConvertEnum2Str(DxNAMEOF(DxSAT_DiseqcVer_e), t->info.diseqc.diseqcVersion)
			, dlib_ConvertEnum2Str(DxNAMEOF(DxSAT_DiseqcInput_e), t->info.diseqc.diseqcInput)
			, dlib_ConvertEnum2Str(DxNAMEOF(DxSAT_ToneBurst_e), t->info.diseqc.toneBurst)
			, t->info.diseqc.ucMotorPosition
			, t->info.diseqc.sSatLonggitude
		);
		break;

	case eDxANT_TYPE_SCD:
		fprintf(fp, "\tSCD(Single Cable Distribution)\n");
		break;

	default:
		break;
	}
}


