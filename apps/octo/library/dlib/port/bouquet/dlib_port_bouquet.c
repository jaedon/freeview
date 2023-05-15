
#include <hlib.h>
#include "_dlib_port_bouquet.h"

static HERROR	dlib_bouquet_Encoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HINT32			jw;
	DxBouquet_t		*pstInfo = (DxBouquet_t*)src;

	HxLOG_Assert(sizeof(DxBouquet_t) == srcSize);

	jw = HLIB_JSONWRITER_Open();
	HxJSONWRITER_ObjectBegin(jw);
		HxJSONWRITER_Integer(jw, "uid", pstInfo->uid);
		HxJSONWRITER_Integer(jw, "version", pstInfo->version);
		HxJSONWRITER_Integer(jw, "id",  pstInfo->id);
		HxJSONWRITER_String (jw, "name", pstInfo->name);

#if defined(CONFIG_DB_SVC_SVCBOUQUET_MULTILANG_NAME)
		{
			HUINT32 			 i;
			DxMultiBouqName_t	*pstData;

			HxJSONWRITER_Array(jw, "name_ext");
			for (i = 0; i < DxMAX_MULTI_BOUQ_NAME_NUM; i++)
			{
				pstData = (DxMultiBouqName_t *)&pstInfo->ex.astMultiBouqName[i];

				HxJSONWRITER_ObjectBegin (jw);
				//if (pstData->szLangCode[0] != '\0')
				{
					HxJSONWRITER_String(jw, "lang", pstData->szLangCode);
					HxJSONWRITER_String(jw, "name", pstData->szBouqName);
				}
				HxJSONWRITER_ObjectEnd(jw);
			}
			HxJSONWRITER_ArrayEnd(jw);
		}
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

static HERROR	dlib_bouquet_Decoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HxJSON_t		root;
	DxBouquet_t		*pstInfo;

	HxLOG_Assert(srcSize && src);

	root = HLIB_JSON_Decode(src);
	HxLOG_Assert(root);

	pstInfo = (DxBouquet_t*)DLIB_Calloc(sizeof(DxBouquet_t));
	pstInfo->typeOf = DxNAMEOF(DxBouquet_t);
	pstInfo->uid	= HLIB_JSON_Object_GetInteger(root, "uid");
	pstInfo->version= HLIB_JSON_Object_GetInteger(root, "version");
	pstInfo->id		= HLIB_JSON_Object_GetInteger(root, "id");
	HxSTD_StrNCpy(pstInfo->name, HLIB_JSON_Object_GetString(root, "name"), DxNAME_LEN - 1);

#if defined(CONFIG_DB_SVC_SVCBOUQUET_MULTILANG_NAME)
	{
		HxJSON_t	array, item;

		array = HLIB_JSON_Object_GetArray(root, "name_ext");
		if (array && HLIB_JSON_GetSize(array) > 0)
		{
			HCHAR		*pucString = NULL;
			HUINT32 	i = 0;

			HxJSON_ARRAY_EACH(array, item, {
				if (i == DxMAX_MULTI_BOUQ_NAME_NUM) 	{ break; }

				pucString = (HCHAR *)HLIB_JSON_Object_GetString(item, "lang");
				if (pucString != NULL)
				{
					HLIB_STD_StrNCpySafe((HCHAR*)pstInfo->ex.astMultiBouqName[i].szLangCode, pucString, DxMAX_BOUQ_LANG_LEN);
				}

				pucString = (HCHAR *)HLIB_JSON_Object_GetString(item, "name");
				if (pucString != NULL)
				{
					HLIB_STD_StrNCpySafe((HCHAR*)pstInfo->ex.astMultiBouqName[i].szBouqName, pucString, DxMAX_BOUQ_NAME_LEN);
				}
				i++;
				});
		}
	}
#endif

	HLIB_JSON_Delete(root);

	*dst = (void*)pstInfo;
	*dstSize = sizeof(DxBouquet_t);

	return ERR_OK;
}

void			DLIB_BOUQUET_Init(void)
{
	dlib_RegisterDECModule(DxNAMEOF(DxBouquet_t), dlib_bouquet_Encoder, dlib_bouquet_Decoder);
}


HINT32			DLIB_BOUQUET_GetFieldCount (void)
{
	return 0;
}

DxField_t *		DLIB_BOUQUET_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	return NULL;
}

HERROR			DLIB_BOUQUET_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_bouquet_Encoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_BOUQUET_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_bouquet_Decoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_BOUQUET_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 데이터 전송 시, 효율을 높이기 위해 압축이 필요한 경우 이 함수 호출됨
	// 압축 방식은 진짜 압축이 아닌, 그냥 string을 binary로 바꾸는 등
	// 아무튼 데이터를 줄이기만 하면 됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}

HERROR			DLIB_BOUQUET_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 압축된 데이터 원복
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}


HERROR			DLIB_BOUQUET_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	DxBouquet_t	*dst;

	dst = (DxBouquet_t *)DLIB_MemDup(src, srcSize);
	if (dst)
	{
		dst->typeOf = DxNAMEOF(DxBouquet_t);
		*type = (void *)dst;
		return ERR_OK;
	}
	return ERR_FAIL;
}

void			DLIB_BOUQUET_FreeType (void *type)
{
	DLIB_Free(type);
}

void			DLIB_BOUQUET_Print (FILE *fp, const DxBouquet_t *t)
{
	fprintf(fp, "BOUQUET[%d] id:%d %s\n"
		, t->uid
		, t->id
		, t->name
	);
	DxLIB_PRINT_INT(t->version);
}

