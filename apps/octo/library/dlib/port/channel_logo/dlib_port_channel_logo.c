
#include <hlib.h>
#include "_dlib_port_channel_logo.h"


typedef struct
{
	DxField_t		field;
	HBOOL			(* getData) (DxField_t *field, const void *src, HUINT32 size);
} DxChannelLogo_Field_t;

#define	RETURN_IF(expr, err)	do{if(expr)return (err);}while(0)

static HBOOL	dlib_channel_logo_Svcuid (DxField_t *field, const void *src, HUINT32 size);
static HBOOL	dlib_channel_logo_ServerLogoUrl (DxField_t *field, const void *src, HUINT32 size);
static HBOOL	dlib_channel_logo_LocalLogoUrl (DxField_t *field, const void *src, HUINT32 size);


#define __________________________STATIC_FUNCTION______________________________

static const DxChannelLogo_Field_t	s_CHANNEL_LOGO_FIELDS[] =
{
	{{eDxTYPE_INTEGER	, "svcuid"			, FALSE, {0}	}, dlib_channel_logo_Svcuid}
	, {{eDxTYPE_STRING	, "ucServerLogoUrl"	, FALSE, {0}	}, dlib_channel_logo_ServerLogoUrl}
	, {{eDxTYPE_STRING	, "ucLocalLogoUrl"	, FALSE, {0}	}, dlib_channel_logo_LocalLogoUrl}
};

static HBOOL	dlib_channel_logo_Svcuid (DxField_t *field, const void *src, HUINT32 size)
{
	DxChannelLogo_t * pData = (DxChannelLogo_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pData->svcuid;
	return TRUE;
}


static HBOOL	dlib_channel_logo_ServerLogoUrl (DxField_t *field, const void *src, HUINT32 size)
{
	DxChannelLogo_t * pData = (DxChannelLogo_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.string =	DLIB_StrDup(pData->ucServerLogoUrl);
	return TRUE;
}

static HBOOL	dlib_channel_logo_LocalLogoUrl (DxField_t *field, const void *src, HUINT32 size)
{
	DxChannelLogo_t * pData = (DxChannelLogo_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.string =	DLIB_StrDup(pData->ucLocalLogoUrl);
	return TRUE;
}


static HERROR	dlib_channel_logo_Encoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HINT32			jw;
	DxChannelLogo_t		*pstInfo = (DxChannelLogo_t*)src;

	HxLOG_Assert(sizeof(DxChannelLogo_t) == srcSize);

	jw = HLIB_JSONWRITER_Open();

	HxJSONWRITER_ObjectBegin(jw);
	HxJSONWRITER_Integer(jw, "svcuid", pstInfo->svcuid);
	HxJSONWRITER_String (jw, "ucServerLogoUrl", pstInfo->ucServerLogoUrl);
	HxJSONWRITER_String (jw, "ucLocalLogoUrl", pstInfo->ucLocalLogoUrl);
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

static HERROR	dlib_channel_logo_Decoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HxJSON_t		root;
	DxChannelLogo_t		*pstInfo;

	HxLOG_Assert(srcSize && src);

	root = HLIB_JSON_Decode(src);
	HxLOG_Assert(root);

	pstInfo = (DxChannelLogo_t*)DLIB_Calloc(sizeof(DxChannelLogo_t));
	pstInfo->svcuid = HLIB_JSON_Object_GetInteger(root, "svcuid");
	HxSTD_StrNCpy(pstInfo->ucServerLogoUrl, HLIB_JSON_Object_GetString(root, "ucServerLogoUrl"), CHANNEL_LOGO_URL_LENGTH - 1);
	HxSTD_StrNCpy(pstInfo->ucLocalLogoUrl, HLIB_JSON_Object_GetString(root, "ucLocalLogoUrl"), CHANNEL_LOGO_URL_LENGTH - 1);

	HLIB_JSON_Delete(root);

	*dst = (void*)pstInfo;
	*dstSize = sizeof(DxChannelLogo_t);

	return ERR_OK;
}

#define __________________________GLOBAL_FUNCTION______________________________


void			DLIB_CHANNEL_LOGO_Init(void)
{
	dlib_RegisterDECModule(DxNAMEOF(DxChannelLogo_t), dlib_channel_logo_Encoder, dlib_channel_logo_Decoder);
}

#define DLIB_CHANNELLOGO_DBFILE CONFIG_PRODUCT_USERDATA_DIR "/ipepg/DxChannelLogo_t.db"
const HCHAR *	DLIB_CHANNEL_LOGO_GetDBFullName (HINT32 dupIndex)
{
	if(dupIndex == 0)
		return DLIB_CHANNELLOGO_DBFILE;
	else
		return NULL;
}


HINT32			DLIB_CHANNEL_LOGO_GetFieldCount (void)
{
	return sizeof(s_CHANNEL_LOGO_FIELDS) / sizeof(s_CHANNEL_LOGO_FIELDS[0]);
}

DxField_t *		DLIB_CHANNEL_LOGO_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	DxField_t field;

	if (DLIB_CHANNEL_LOGO_GetFieldCount() <= index)
	{
		HxLOG_Warning("%s(index) index(index) out of range~!\n", __FUNCTION__, index);
		return NULL;
	}
	memcpy(&field, &(s_CHANNEL_LOGO_FIELDS[index].field), sizeof(DxField_t));

	if (src && srcSize && s_CHANNEL_LOGO_FIELDS[index].getData)
	{
		if (!s_CHANNEL_LOGO_FIELDS[index].getData(&field, src, srcSize))
		{
			HxLOG_Warning("%s(%s) Cannot get data\n", __FUNCTION__, s_CHANNEL_LOGO_FIELDS[index].field.name);
			return NULL;
		}
	}

	// 메모리 함수는 HxSTD_MemXXX를 사용한다.
	return (DxField_t *)DLIB_MemDup(&field, sizeof(DxField_t));

}

HERROR			DLIB_CHANNEL_LOGO_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_channel_logo_Encoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_CHANNEL_LOGO_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_channel_logo_Decoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_CHANNEL_LOGO_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 데이터 전송 시, 효율을 높이기 위해 압축이 필요한 경우 이 함수 호출됨
	// 압축 방식은 진짜 압축이 아닌, 그냥 string을 binary로 바꾸는 등
	// 아무튼 데이터를 줄이기만 하면 됨
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}

HERROR			DLIB_CHANNEL_LOGO_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// 압축된 데이터 원복
	// dst는 메모리 할당을 해서 전달해야 함 (HxSTD_MemXXX를 사용)

	return ERR_FAIL;
}


HERROR			DLIB_CHANNEL_LOGO_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	return ERR_FAIL;
}

void			DLIB_CHANNEL_LOGO_FreeType (void *type)
{
}



