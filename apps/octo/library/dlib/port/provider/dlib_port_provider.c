
#include <hlib.h>
#include "_dlib_port_provider.h"

static HERROR	dlib_provider_Encoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HINT32			jw;
	DxProvider_t		*pstInfo = (DxProvider_t*)src;

	HxLOG_Assert(sizeof(DxProvider_t) == srcSize);

	jw = HLIB_JSONWRITER_Open();
	HxJSONWRITER_ObjectBegin(jw);
		HxJSONWRITER_Integer(jw, "uid", pstInfo->uid);
		HxJSONWRITER_Integer(jw, "version", pstInfo->version);
		HxJSONWRITER_String (jw, "name", pstInfo->name);
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

static HERROR	dlib_provider_Decoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HxJSON_t		root;
	DxProvider_t		*pstInfo;

	HxLOG_Assert(srcSize && src);

	root = HLIB_JSON_Decode(src);
	HxLOG_Assert(root);

	pstInfo = (DxProvider_t*)DLIB_Calloc(sizeof(DxProvider_t));
	pstInfo->typeOf = DxNAMEOF(DxProvider_t);
	pstInfo->uid	= HLIB_JSON_Object_GetInteger(root, "uid");
	pstInfo->version= HLIB_JSON_Object_GetInteger(root, "version");
	HxSTD_StrNCpy(pstInfo->name, HLIB_JSON_Object_GetString(root, "name"), DxNAME_LEN - 1);

	HLIB_JSON_Delete(root);

	*dst = (void*)pstInfo;
	*dstSize = sizeof(DxProvider_t);

	return ERR_OK;
}

void			DLIB_PROVIDER_Init(void)
{
	dlib_RegisterDECModule(DxNAMEOF(DxProvider_t), dlib_provider_Encoder, dlib_provider_Decoder);
}

HINT32			DLIB_PROVIDER_GetFieldCount (void)
{
	return 0;
}

DxField_t *		DLIB_PROVIDER_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	return NULL;
}

HERROR			DLIB_PROVIDER_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_provider_Encoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_PROVIDER_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_provider_Decoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_PROVIDER_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 데이터 전송 시, 효율을 높이기 위해 압축이 필요한 경우 이 함수 호출됨
	// 압축 방식은 진짜 압축이 아닌, 그냥 string을 binary로 바꾸는 등
	// 아무튼 데이터를 줄이기만 하면 됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}

HERROR			DLIB_PROVIDER_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 압축된 데이터 원복
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}


HERROR			DLIB_PROVIDER_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	DxProvider_t	*dst;

	dst = (DxProvider_t *)DLIB_MemDup(src, srcSize);
	if (dst)
	{
		dst->typeOf = DxNAMEOF(DxProvider_t);
		*type = (void *)dst;
		return ERR_OK;
	}
	return ERR_FAIL;
}

void			DLIB_PROVIDER_FreeType (void *type)
{
	DLIB_Free(type);
}

void			DLIB_PROVIDER_Print (FILE *fp, const DxProvider_t *t)
{
	fprintf(fp, "PROVIDER[%d] %s\n"
		, t->uid, t->name
	);
	DxLIB_PRINT_INT(t->version);
}


