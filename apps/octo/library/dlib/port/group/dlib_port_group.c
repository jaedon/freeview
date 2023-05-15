
#include <hlib.h>
#include "_dlib_port_group.h"

static HERROR	dlib_group_Encoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HINT32			jw;
	DxGroup_t		*pstInfo = (DxGroup_t*)src;

	HxLOG_Assert(sizeof(DxGroup_t) == srcSize);

	jw = HLIB_JSONWRITER_Open();
	HxJSONWRITER_ObjectBegin(jw);
		HxJSONWRITER_Integer(jw, "uid", pstInfo->uid);
		HxJSONWRITER_Integer(jw, "version", pstInfo->version);
		HxJSONWRITER_Integer(jw, "id",  pstInfo->id);
		HxJSONWRITER_String (jw, "name", pstInfo->name);

		#if	defined(CONFIG_OP_FREESAT)
		HxJSONWRITER_Object (jw, "freesat");
		HxJSONWRITER_Integer(jw, "ucGroupType", pstInfo->ex.freesat.ucGroupType);
		HxJSONWRITER_Integer(jw, "ucNondestructiveTuneFlag", pstInfo->ex.freesat.ucNondestructiveTuneFlag);
		HxJSONWRITER_Integer(jw, "ucReturnChannelAccessFlag", pstInfo->ex.freesat.ucReturnChannelAccessFlag);
		HxJSONWRITER_String (jw, "szIso639LangCode", pstInfo->ex.freesat.szIso639LangCode);
		HxJSONWRITER_Integer(jw, "ucG2ExtensionFlag", pstInfo->ex.freesat.ucG2ExtensionFlag);
		HxJSONWRITER_Integer(jw, "ucDefaultVisableFlag", pstInfo->ex.freesat.ucDefaultVisableFlag);
		HxJSONWRITER_Integer(jw, "ucAdultChannelsGroupFlag", pstInfo->ex.freesat.ucAdultChannelsGroupFlag);
		HxJSONWRITER_Integer(jw, "ucIpChannelsGroupFlag", pstInfo->ex.freesat.ucIpChannelsGroupFlag);
		HxJSONWRITER_Integer(jw, "ucCaChannelsGroupFlag", pstInfo->ex.freesat.ucCaChannelsGroupFlag);
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

static HERROR	dlib_group_Decoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HxJSON_t		root;
	DxGroup_t		*pstInfo;

	HxLOG_Assert(srcSize && src);

	root = HLIB_JSON_Decode(src);
	HxLOG_Assert(root);

	pstInfo = (DxGroup_t*)DLIB_Calloc(sizeof(DxGroup_t));
	pstInfo->typeOf = DxNAMEOF(DxGroup_t);
	pstInfo->uid	= HLIB_JSON_Object_GetInteger(root, "uid");
	pstInfo->version= HLIB_JSON_Object_GetInteger(root, "version");
	pstInfo->id		= HLIB_JSON_Object_GetInteger(root, "id");
	HxSTD_StrNCpy(pstInfo->name, HLIB_JSON_Object_GetString(root, "name"), DxNAME_LEN - 1);

	#if	defined(CONFIG_OP_FREESAT)
	{
		HxJSON_t	freesat;

		freesat = HxJSON_Object_Get(root, "freesat");
		if (freesat)
		{
			pstInfo->ex.freesat.ucGroupType = HLIB_JSON_Object_GetInteger(freesat, "ucGroupType");
			pstInfo->ex.freesat.ucNondestructiveTuneFlag = HLIB_JSON_Object_GetInteger(freesat, "ucNondestructiveTuneFlag");
			pstInfo->ex.freesat.ucReturnChannelAccessFlag = HLIB_JSON_Object_GetInteger(freesat, "ucReturnChannelAccessFlag");
			HxSTD_StrNCpy(pstInfo->ex.freesat.szIso639LangCode, HLIB_JSON_Object_GetString(freesat, "szIso639LangCode"), DxFSAT_LANG_CODE_LEN);
			pstInfo->ex.freesat.ucGroupType = HLIB_JSON_Object_GetInteger(freesat, "ucGroupType");
			pstInfo->ex.freesat.ucG2ExtensionFlag = HLIB_JSON_Object_GetInteger(freesat, "ucG2ExtensionFlag");
			pstInfo->ex.freesat.ucDefaultVisableFlag = HLIB_JSON_Object_GetInteger(freesat, "ucDefaultVisableFlag");
			pstInfo->ex.freesat.ucAdultChannelsGroupFlag = HLIB_JSON_Object_GetInteger(freesat, "ucAdultChannelsGroupFlag");
			pstInfo->ex.freesat.ucIpChannelsGroupFlag = HLIB_JSON_Object_GetInteger(freesat, "ucIpChannelsGroupFlag");
			pstInfo->ex.freesat.ucCaChannelsGroupFlag = HLIB_JSON_Object_GetInteger(freesat, "ucCaChannelsGroupFlag");
		}
	}
	#endif

	HLIB_JSON_Delete(root);

	*dst = (void*)pstInfo;
	*dstSize = sizeof(DxGroup_t);

	return ERR_OK;
}

void			DLIB_GROUP_Init(void)
{
	dlib_RegisterDECModule(DxNAMEOF(DxGroup_t), dlib_group_Encoder, dlib_group_Decoder);
}


HINT32			DLIB_GROUP_GetFieldCount (void)
{
	return 0;
}

DxField_t *		DLIB_GROUP_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	return NULL;
}

HERROR			DLIB_GROUP_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_group_Encoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_GROUP_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_group_Decoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_GROUP_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 데이터 전송 시, 효율을 높이기 위해 압축이 필요한 경우 이 함수 호출됨
	// 압축 방식은 진짜 압축이 아닌, 그냥 string을 binary로 바꾸는 등
	// 아무튼 데이터를 줄이기만 하면 됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}

HERROR			DLIB_GROUP_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 압축된 데이터 원복
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}


HERROR			DLIB_GROUP_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	DxGroup_t	*dst;

	dst = (DxGroup_t *)DLIB_MemDup(src, srcSize);
	if (dst)
	{
		dst->typeOf = DxNAMEOF(DxGroup_t);
		*type = (void *)dst;
		return ERR_OK;
	}
	return ERR_FAIL;
}

void			DLIB_GROUP_FreeType (void *type)
{
	DLIB_Free(type);
}

void			DLIB_GROUP_Print (FILE *fp, const DxGroup_t *t)
{
	fprintf(fp, "GROUP[%d] id:%d %s\n"
		, t->uid
		, t->id
		, t->name
	);
	DxLIB_PRINT_INT(t->version);

#ifdef	CONFIG_OP_FREESAT
	{
		const DxFreesatGroup_t *freesat = &(t->ex.freesat);

		DxLIB_PRINT_HEX(freesat->ucGroupType);
		DxLIB_PRINT_HEX(freesat->ucNondestructiveTuneFlag);
		DxLIB_PRINT_HEX(freesat->ucReturnChannelAccessFlag);
		DxLIB_PRINT_STR(freesat->szIso639LangCode);
		DxLIB_PRINT_HEX(freesat->ucG2ExtensionFlag);
		DxLIB_PRINT_HEX(freesat->ucDefaultVisableFlag);
		DxLIB_PRINT_HEX(freesat->ucAdultChannelsGroupFlag);
		DxLIB_PRINT_HEX(freesat->ucIpChannelsGroupFlag);
		DxLIB_PRINT_HEX(freesat->ucCaChannelsGroupFlag);
	}
#endif
}


