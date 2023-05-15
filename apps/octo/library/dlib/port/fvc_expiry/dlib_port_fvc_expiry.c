
#include <hlib.h>
#include "_dlib_port_fvc_expiry.h"

#if defined(CONFIG_PROD_OS_EMULATOR)
#include <dlib_fvc_expiry.h>
#endif

typedef struct
{
	DxField_t		field;
	HBOOL			(* getData) (DxField_t *field, const void *src, HUINT32 size);
} DxFvcExpiry_Field_t;

#define	RETURN_IF(expr, err)	do{if(expr)return (err);}while(0)

static HBOOL	dlib_fvc_expiry_Onid (DxField_t *field, const void *src, HUINT32 size);
static HBOOL	dlib_fvc_expiry_StartTime (DxField_t *field, const void *src, HUINT32 size);
static HBOOL	dlib_fvc_expiry_ExpiryTime (DxField_t *field, const void *src, HUINT32 size);
static HBOOL	dlib_fvc_expiry_Type (DxField_t *field, const void *src, HUINT32 size);
static HBOOL	dlib_fvc_expiry_StatusCode (DxField_t *field, const void *src, HUINT32 size);


#define __________________________STATIC_FUNCTION______________________________

static const DxFvcExpiry_Field_t	s_FVC_EXPIRY_FIELDS[] =
{
	{{eDxTYPE_INTEGER	, "onid"			, FALSE, {0}	}, dlib_fvc_expiry_Onid}
	, {{eDxTYPE_INTEGER	, "startTime"	, FALSE, {0}	}, dlib_fvc_expiry_StartTime}
	, {{eDxTYPE_INTEGER	, "expiryTime"	, FALSE, {0}	}, dlib_fvc_expiry_ExpiryTime}
	, {{eDxTYPE_INTEGER	, "type"		, FALSE, {0}	}, dlib_fvc_expiry_Type}
	, {{eDxTYPE_INTEGER	, "statusCode"		, FALSE, {0}	}, dlib_fvc_expiry_StatusCode}
};

static HBOOL	dlib_fvc_expiry_Onid (DxField_t *field, const void *src, HUINT32 size)
{
	DxFvcExpiry_t * pData = (DxFvcExpiry_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pData->onid;
	return TRUE;
}

static HBOOL	dlib_fvc_expiry_StartTime (DxField_t *field, const void *src, HUINT32 size)
{
	DxFvcExpiry_t * pData = (DxFvcExpiry_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pData->startTime;
	return TRUE;
}

static HBOOL	dlib_fvc_expiry_ExpiryTime (DxField_t *field, const void *src, HUINT32 size)
{
	DxFvcExpiry_t * pData = (DxFvcExpiry_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pData->expiryTime;
	return TRUE;
}

static HBOOL	dlib_fvc_expiry_Type (DxField_t *field, const void *src, HUINT32 size)
{
	DxFvcExpiry_t * pData = (DxFvcExpiry_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pData->type;
	return TRUE;
}

static HBOOL	dlib_fvc_expiry_StatusCode (DxField_t *field, const void *src, HUINT32 size)
{
	DxFvcExpiry_t * pData = (DxFvcExpiry_t *)src;
	RETURN_IF(field == NULL || src == NULL || size == 0, FALSE);
	field->u.integer = pData->statusCode;
	return TRUE;
}


static HERROR	dlib_fvc_expiry_Encoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HINT32			jw;
	DxFvcExpiry_t		*pstInfo = (DxFvcExpiry_t*)src;

	HxLOG_Assert(sizeof(DxFvcExpiry_t) == srcSize);

	jw = HLIB_JSONWRITER_Open();

	HxJSONWRITER_ObjectBegin(jw);
	HxJSONWRITER_Binary (jw, "uid", &pstInfo->uid, sizeof(HUID));
	HxJSONWRITER_Integer(jw, "onid", pstInfo->onid);
	HxJSONWRITER_Integer(jw, "startTime", pstInfo->startTime);
	HxJSONWRITER_Integer(jw, "expiryTime", pstInfo->expiryTime);
	HxJSONWRITER_Integer(jw, "statusCode", pstInfo->statusCode);
	HxJSONWRITER_Integer(jw, "queryStampTime", pstInfo->queryStampTime);
	HxJSONWRITER_Integer(jw, "type", pstInfo->type);
	HxJSONWRITER_String(jw, "lastModifiedTime", pstInfo->lastModifiedTime);
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

static HERROR	dlib_fvc_expiry_Decoder(const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HxJSON_t		root;
	DxFvcExpiry_t		*pstInfo;

	HxLOG_Assert(srcSize && src);

	root = HLIB_JSON_Decode(src);
	HxLOG_Assert(root);

	pstInfo = (DxFvcExpiry_t*)DLIB_Calloc(sizeof(DxFvcExpiry_t));
	HLIB_JSON_Object_GetBinary(root, "uid", &pstInfo->uid, sizeof(HUID));
	pstInfo->onid = HLIB_JSON_Object_GetInteger(root, "onid");
	pstInfo->startTime = HLIB_JSON_Object_GetInteger(root, "startTime");
	pstInfo->expiryTime = HLIB_JSON_Object_GetInteger(root, "expiryTime");
	pstInfo->statusCode = HLIB_JSON_Object_GetInteger(root, "statusCode");
	pstInfo->queryStampTime = HLIB_JSON_Object_GetInteger(root, "queryStampTime");
	pstInfo->type = HLIB_JSON_Object_GetInteger(root, "type");
	HxSTD_StrNCpy(pstInfo->lastModifiedTime, HLIB_JSON_Object_GetString(root, "lastModifiedTime"), FVC_EXPIRY_DATE_LENGTH - 1);

	HLIB_JSON_Delete(root);

	*dst = (void*)pstInfo;
	*dstSize = sizeof(DxFvcExpiry_t);

	return ERR_OK;
}

#define __________________________GLOBAL_FUNCTION______________________________


void			DLIB_FVC_EXPIRY_Init(void)
{
	dlib_RegisterDECModule(DxNAMEOF(DxFvcExpiry_t), dlib_fvc_expiry_Encoder, dlib_fvc_expiry_Decoder);
}

#define DLIB_FVCEXPIRE_DBFILE CONFIG_PRODUCT_USERDATA_DIR "/ipepg/DxFvcExpiry_t.db"
const HCHAR *	DLIB_FVC_EXPIRY_GetDBFullName (HINT32 dupIndex)
{
	if(dupIndex == 0)
		return DLIB_FVCEXPIRE_DBFILE;
	else
		return NULL;
}


HINT32			DLIB_FVC_EXPIRY_GetFieldCount (void)
{
	return sizeof(s_FVC_EXPIRY_FIELDS) / sizeof(s_FVC_EXPIRY_FIELDS[0]);
}

DxField_t *		DLIB_FVC_EXPIRY_GetField (HINT32 index, const void *src, HUINT32 srcSize)
{
	DxField_t field;

	if (DLIB_FVC_EXPIRY_GetFieldCount() <= index)
	{
		HxLOG_Warning("%s(index) index(index) out of range~!\n", __FUNCTION__, index);
		return NULL;
	}
	memcpy(&field, &(s_FVC_EXPIRY_FIELDS[index].field), sizeof(DxField_t));

	if (src && srcSize && s_FVC_EXPIRY_FIELDS[index].getData)
	{
		if (!s_FVC_EXPIRY_FIELDS[index].getData(&field, src, srcSize))
		{
			HxLOG_Warning("%s(%s) Cannot get data\n", __FUNCTION__, s_FVC_EXPIRY_FIELDS[index].field.name);
			return NULL;
		}
	}

	// �޸� �Լ��� HxSTD_MemXXX�� ����Ѵ�.
	return (DxField_t *)DLIB_MemDup(&field, sizeof(DxField_t));

}

HERROR			DLIB_FVC_EXPIRY_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_fvc_expiry_Encoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_FVC_EXPIRY_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	(void)(uid);

	return dlib_fvc_expiry_Decoder(src, srcSize, dst, dstSize);
}

HERROR			DLIB_FVC_EXPIRY_Compress (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// ������ ���� ��, ȿ���� ���̱� ���� ������ �ʿ��� ��� �� �Լ� ȣ���
	// ���� ����� ��¥ ������ �ƴ�, �׳� string�� binary�� �ٲٴ� ��
	// �ƹ�ư �����͸� ���̱⸸ �ϸ� ��
	// dst�� �޸� �Ҵ��� �ؼ� �����ؾ� �� (HxSTD_MemXXX�� ���)

	return ERR_FAIL;
}

HERROR			DLIB_FVC_EXPIRY_Extract (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// ����� ������ ����
	// dst�� �޸� �Ҵ��� �ؼ� �����ؾ� �� (HxSTD_MemXXX�� ���)

	return ERR_FAIL;
}

HERROR			DLIB_FVC_EXPIRY_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{
	DxFvcExpiry_t	*dst;

	dst = (DxFvcExpiry_t *)DLIB_MemDup(src, srcSize);
	if (dst)
	{
		*type = (void *)dst;
		return ERR_OK;
	}
	return ERR_FAIL;
}

void			DLIB_FVC_EXPIRY_FreeType (void *type)
{
	DLIB_Free(type);
}

