
#include <hlib.h>
#include "_dlib_port_network.h"

static HERROR	dlib_network_Encoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HINT32			jw;
	DxNetwork_t		*pstInfo = (DxNetwork_t*)src;
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	HINT32			            i;
	DxTvAnyTime_RegionInfoNet_t	*pstRegionInfo;
#endif

	HxLOG_Assert(sizeof(DxNetwork_t) == srcSize);

	jw = HLIB_JSONWRITER_Open();
	HxJSONWRITER_ObjectBegin(jw);
		HxJSONWRITER_Integer(jw, "uid", pstInfo->uid);
		HxJSONWRITER_Integer(jw, "version", pstInfo->version);
		HxJSONWRITER_Integer(jw, "onid",  pstInfo->onid);
		HxJSONWRITER_String (jw, "name", pstInfo->name);
		HxJSONWRITER_String (jw, "deliType", dlib_ConvertEnum2Str(DxNAMEOF(DxDeliveryType_e), (HINT32)pstInfo->deliType));

		#if	defined(CONFIG_OP_FREESAT)
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
			pstRegionInfo = (DxTvAnyTime_RegionInfoNet_t *)&pstInfo->ex.tva.stRegionInfo[i];
			HxJSONWRITER_ObjectBegin (jw);
			HxJSONWRITER_String(jw, "countrycode",pstRegionInfo->szCountryCode);
			HxJSONWRITER_Integer(jw, "regiondepth",pstRegionInfo->ucRegionDepth);
			HxJSONWRITER_Integer(jw, "regioncode",pstRegionInfo->ulRegionCode);
			HxJSONWRITER_ObjectEnd(jw);
		}
		HxJSONWRITER_ArrayEnd(jw);
		HxJSONWRITER_String(jw, "uriPrefix", pstInfo->ex.tva.UriPrefix);

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

static HERROR	dlib_network_Decoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HxJSON_t		root;
	DxNetwork_t		*pstInfo;

	HxLOG_Assert(srcSize && src);

	root = HLIB_JSON_Decode(src);
	HxLOG_Assert(root);

	pstInfo = (DxNetwork_t*)DLIB_Calloc(sizeof(DxNetwork_t));
	pstInfo->typeOf = DxNAMEOF(DxNetwork_t);
	pstInfo->uid	= HLIB_JSON_Object_GetInteger(root, "uid");
	pstInfo->version= HLIB_JSON_Object_GetInteger(root, "version");
	pstInfo->onid		= HLIB_JSON_Object_GetInteger(root, "onid");
	HxSTD_StrNCpy(pstInfo->name, HLIB_JSON_Object_GetString(root, "name"), DxNAME_LEN - 1);
	pstInfo->deliType = (DxDeliveryType_e)dlib_ConvertStr2Enum(DxNAMEOF(DxDeliveryType_e), HLIB_JSON_Object_GetString(root, "deliType"));

	#if defined(CONFIG_OP_FREESAT)
	{
		HxJSON_t	freesat;

		freesat = HxJSON_Object_Get(root, "freesat");
		if (freesat)
		{
			HxSTD_StrNCpy(pstInfo->ex.freesat.defaultAuth, HLIB_JSON_Object_GetString(freesat, "defaultAuth"), DxDEFAULT_AUTHORITY_LEN - 1);
		} else
			HxLOG_Warning("freesat flag is NULL\n");
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
			HxSTD_StrNCpy(pstInfo->ex.tva.UriPrefix, HLIB_JSON_Object_GetString(tvAnyTime, "uriPrefix"), DxMAX_URI_PREFIX_INFO_LEN - 1);

		} else
			HxLOG_Warning("ukDtt flag is NULL\n");
	}
	#endif

	HLIB_JSON_Delete(root);

	*dst = (void*)pstInfo;
	*dstSize = sizeof(DxNetwork_t);

	return ERR_OK;
}

void			DLIB_NETWORK_Init(void)
{
	dlib_RegisterDECModule(DxNAMEOF(DxNetwork_t), dlib_network_Encoder, dlib_network_Decoder);
}


HINT32			DLIB_NETWORK_GetFieldCount (void)
{
	return 0;
}

DxField_t *		DLIB_NETWORK_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	return NULL;
}

HERROR			DLIB_NETWORK_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_network_Encoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_NETWORK_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_network_Decoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_NETWORK_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 데이터 전송 시, 효율을 높이기 위해 압축이 필요한 경우 이 함수 호출됨
	// 압축 방식은 진짜 압축이 아닌, 그냥 string을 binary로 바꾸는 등
	// 아무튼 데이터를 줄이기만 하면 됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}

HERROR			DLIB_NETWORK_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 압축된 데이터 원복
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}


HERROR			DLIB_NETWORK_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	DxNetwork_t	*dst;

	dst = (DxNetwork_t *)DLIB_MemDup(src, srcSize);
	if (dst)
	{
		dst->typeOf = DxNAMEOF(DxNetwork_t);
		*type = (void *)dst;
		return ERR_OK;
	}
	return ERR_FAIL;
}

void			DLIB_NETWORK_FreeType (void *type)
{
	DLIB_Free(type);
}

void			DLIB_NETWORK_Print (FILE *fp, const DxNetwork_t *t)
{
	fprintf(fp, "NETWORK[%d] %s\n"
		, t->uid
		, t->name
	);
	DxLIB_PRINT_INT(t->version);
	DxLIB_PRINT_HEX(t->onid);
	DxLIB_PRINT_ENUM(t->deliType);

#ifdef	CONFIG_OP_FREESAT
	DxLIB_PRINT_STR(t->ex.freesat.defaultAuth);
#endif

#ifdef	CONFIG_OP_PVR_BASED_ON_TVANYTIME
	DxLIB_PRINT_STR(t->ex.tva.defaultAuth);
#endif

}



