
#include <hlib.h>
#include "_dlib_port_transponder.h"

static HERROR	dlib_transponder_Encoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HINT32			jw;
	DxTransponder_t		*pstInfo = (DxTransponder_t*)src;
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HINT32			            i;
	DxTvAnyTime_RegionInfoTs_t	*pstRegionInfo;
#endif

	HxLOG_Assert(sizeof(DxTransponder_t) == srcSize);

	jw = HLIB_JSONWRITER_Open();
	HxJSONWRITER_ObjectBegin(jw);
		HxJSONWRITER_Integer(jw, "uid", pstInfo->uid);
		HxJSONWRITER_Integer(jw, "version", pstInfo->version);
		HxJSONWRITER_Integer(jw, "netuid", pstInfo->netuid);
		HxJSONWRITER_Integer(jw, "tsid", pstInfo->tsid);
		HxJSONWRITER_Integer(jw, "onid", pstInfo->onid);
		HxJSONWRITER_Integer(jw, "tunerid", pstInfo->tunerid);
		HxJSONWRITER_String (jw, "deliType", dlib_ConvertEnum2Str(DxNAMEOF(DxDeliveryType_e), pstInfo->deliType));

		//	tuneparam
		{
			DxDECFunc	encoder;

			encoder = dlib_GetDECModule(DxNAMEOF(DxTuneParam_t), TRUE);
			if (encoder)
			{
				HCHAR	*dstString;
				HUINT32	dstStringSize;

				if (encoder((void*)&pstInfo->tuningParam, sizeof(DxTuneParam_t), (void**)&dstString, &dstStringSize) == ERR_OK)
				{
					HxJSONWRITER_UserJSON(jw, "tuningParam", dstString);
					DLIB_Free(dstString);
				}
			}
		}

		#if defined(CONFIG_OP_FREESAT)
		HxJSONWRITER_Object(jw, "freesat");
		HxJSONWRITER_String(jw, "defaultAuth", pstInfo->ex.freesat.defaultAuth);
		HxJSONWRITER_ObjectEnd(jw);
		#endif


		#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
		HxJSONWRITER_Object(jw, "tva");
		HxJSONWRITER_String(jw, "defaultAuth", pstInfo->ex.tva.defaultAuth);
		HxJSONWRITER_Array(jw, "regionInfo");
		for (i = 0; i < DxMAX_T_REGION_NUM; i++)
		{
			pstRegionInfo = (DxTvAnyTime_RegionInfoTs_t *)&pstInfo->ex.tva.stRegionInfo[i];
			HxJSONWRITER_ObjectBegin (jw);
			HxJSONWRITER_String(jw, "countrycode",pstRegionInfo->szCountryCode);
			HxJSONWRITER_Integer(jw, "regiondepth",pstRegionInfo->ucRegionDepth);
			HxJSONWRITER_Integer(jw, "regioncode",pstRegionInfo->ulRegionCode);
			HxJSONWRITER_ObjectEnd(jw);
		}
		HxJSONWRITER_ArrayEnd(jw);

		HxJSONWRITER_ObjectEnd(jw);
		#endif


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

static HERROR	dlib_transponder_Decoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HxJSON_t		root;
	DxTransponder_t		*pstInfo;

	HxLOG_Assert(srcSize && src);

	root = HLIB_JSON_Decode(src);
	HxLOG_Assert(root);

	pstInfo = (DxTransponder_t*)DLIB_Calloc(sizeof(DxTransponder_t));
	pstInfo->typeOf = DxNAMEOF(DxTransponder_t);
	pstInfo->uid = HLIB_JSON_Object_GetInteger(root, "uid");
	pstInfo->version = HLIB_JSON_Object_GetInteger(root, "version");
	pstInfo->netuid = HLIB_JSON_Object_GetInteger(root, "netuid");
	pstInfo->tsid = HLIB_JSON_Object_GetInteger(root, "tsid");
	pstInfo->onid = HLIB_JSON_Object_GetInteger(root, "onid");
	pstInfo->tunerid = HLIB_JSON_Object_GetInteger(root, "tunerid");
	pstInfo->deliType = dlib_ConvertStr2Enum(DxNAMEOF(DxDeliveryType_e), HLIB_JSON_Object_GetString(root, "deliType"));

	//	tuneparam
	{
		HxJSON_t	tuneParam;

		tuneParam = HxJSON_Object_Get(root, "tuningParam");
		if (tuneParam)
		{
			HCHAR		*encoded;
			DxDECFunc	decoder;

			decoder = dlib_GetDECModule(DxNAMEOF(DxTuneParam_t), FALSE);
			if (decoder)
			{
				encoded = HLIB_JSON_Encode(tuneParam, NULL, 0);
				if (encoded)
				{
					HUINT32 dstDataSize;
					void	*pvVoid = NULL;

					decoder(encoded, HxSTD_StrLen(encoded), (void**)&pvVoid, &dstDataSize);
					HxLOG_Assert(dstDataSize == sizeof(DxTuneParam_t));
					if (pvVoid)
					{
						HxSTD_MemCopy(&pstInfo->tuningParam, pvVoid, dstDataSize);
						DLIB_Free(pvVoid);
					}
					HLIB_JSON_FreeMemory(encoded);
				}
			}
		}
	}

	#if	defined(CONFIG_OP_FREESAT)
	{
		HxJSON_t	freesat;

		freesat = HxJSON_Object_Get(root, "freesat");
		if (freesat)
		{
			HxSTD_StrNCpy(pstInfo->ex.freesat.defaultAuth, HLIB_JSON_Object_GetString(freesat, "defaultAuth"), DxDEFAULT_AUTHORITY_LEN - 1);
		}
	}
	#endif

	#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	{
		HxJSON_t	tvAnyTime;
		HxJSON_t    array, item;

		tvAnyTime = HxJSON_Object_Get(root, "tva");
		if (tvAnyTime)
		{
			HxSTD_StrNCpy(pstInfo->ex.tva.defaultAuth, HLIB_JSON_Object_GetString(tvAnyTime, "defaultAuth"), DxDEFAULT_AUTHORITY_LEN - 1);
			array = HLIB_JSON_Object_GetArray(tvAnyTime, "regioninfo");
			if (array && HLIB_JSON_GetSize(array) > 0)
			{
				HINT32	i = 0;
				const HCHAR	*value = NULL;

				HxJSON_ARRAY_EACH(array, item, {
					value = HLIB_JSON_Object_GetString(item, "countrycode");
					if(value != NULL)
						 HLIB_STD_StrNCpySafe((HCHAR*)&pstInfo->ex.tva.stRegionInfo[i].szCountryCode, value, 4);
					pstInfo->ex.tva.stRegionInfo[i].ucRegionDepth = HLIB_JSON_Object_GetInteger(item, "regiondepth");
					pstInfo->ex.tva.stRegionInfo[i].ulRegionCode = HLIB_JSON_Object_GetInteger(item,"regioncode");
					if (++i == DxMAX_T_REGION_NUM)
						break;
				});
			}

		}
	}
	#endif


	HLIB_JSON_Delete(root);

	*dst = (void*)pstInfo;
	*dstSize = sizeof(DxTransponder_t);

	return ERR_OK;
}

void			DLIB_TRANSPONDER_Init(void)
{
	dlib_RegisterDECModule(DxNAMEOF(DxTransponder_t), dlib_transponder_Encoder, dlib_transponder_Decoder);
}


HINT32			DLIB_TRANSPONDER_GetFieldCount (void)
{
	return 0;
}

DxField_t *		DLIB_TRANSPONDER_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	return NULL;
}

HERROR			DLIB_TRANSPONDER_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_transponder_Encoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_TRANSPONDER_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_transponder_Decoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_TRANSPONDER_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 데이터 전송 시, 효율을 높이기 위해 압축이 필요한 경우 이 함수 호출됨
	// 압축 방식은 진짜 압축이 아닌, 그냥 string을 binary로 바꾸는 등
	// 아무튼 데이터를 줄이기만 하면 됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}

HERROR			DLIB_TRANSPONDER_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 압축된 데이터 원복
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}


HERROR			DLIB_TRANSPONDER_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	DxTransponder_t	*dst;

	dst = (DxTransponder_t *)DLIB_MemDup(src, srcSize);
	if (dst)
	{
		dst->typeOf = DxNAMEOF(DxTransponder_t);
		*type = (void *)dst;
		return ERR_OK;
	}
	return ERR_FAIL;
}

void			DLIB_TRANSPONDER_FreeType (void *type)
{
	DLIB_Free(type);
}


void			DLIB_TRANSPONDER_Print (FILE *fp, const DxTransponder_t *t)
{
	fprintf(fp, "TRANSPONDER[%d]\n"
		, t->uid
	);
	DxLIB_PRINT_INT(t->version);
	DxLIB_PRINT_INT(t->netuid);
	DxLIB_PRINT_HEX(t->tsid);
	DxLIB_PRINT_HEX(t->onid);
	DxLIB_PRINT_HEX(t->tunerid);
	DxLIB_PRINT_ENUM(t->deliType);
	DxLIB_PRINT_BYTE_DUMP(t->tuningParam, sizeof(DxTuneParam_t));

#if defined(CONFIG_OP_FREESAT)
	DxLIB_PRINT_STR(t->ex.freesat.defaultAuth);
#elif	defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	DxLIB_PRINT_STR(t->ex.tva.defaultAuth);
#endif


}


