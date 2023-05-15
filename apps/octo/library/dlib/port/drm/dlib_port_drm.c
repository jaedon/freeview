#if 0  //
#include <hlib.h>
#include <silib.h>

#include "_dlib_port_drm.h"

#define	ERROR_IF(expr, err)	do{if(expr){HxLOG_Error("%s failed!!\n", #expr);return (err);}}while(0)

int	dlib_port_drm_test (int c, ...);

void			DLIB_DRM_Init (void)
{
	static DxEnumStringTable_t s_DxDRM_Type_e =
	{
		(HCHAR*)DxNAMEOF(DxDRM_Type_e),
		{
			{"DRM_V1",  eDxDRM_TYPE_V1},
			{"DRM_CI+", eDxDRM_TYPE_CIPLUS},
			{"DRM_ARIB_CP_INFO", eDxDRM_TYPE_ARIB_CPINFO},
			{NULL, 0}
		}
	};
	static DxEnumStringTable_t s_DxDRM_CIPLUS_Write_e =
	{
		(HCHAR*)DxNAMEOF(DxDRM_CIPLUS_Write_e),
		{
			{"DRM_CI+WRITE_NONE", eDxDRM_CIPLUS_WRITE_NONE},
			{"DRM_CI+WRITE_ING" , eDxDRM_CIPLUS_WRITE_WRITING},
			{"DRM_CI+WRITE_DONE", eDxDRM_CIPLUS_WRITE_DONE},
			{NULL, 0}
		}
	};
	static DxEnumStringTable_t s_DxDRM_CIPLUS_Record_e =
	{
		(HCHAR*)DxNAMEOF(DxDRM_CIPLUS_Record_e),
		{
			{"DRM_CI+REC_OK"   , eDxDRM_CIPLUS_CCV1_RECORD_OK},
			{"DRM_CI+REC_WAIT" , eDxDRM_CIPLUS_CCV2_RECORD_WAIT},
			{"DRM_CI+REC_START", eDxDRM_CIPLUS_CCV2_RECORD_START},
			{"DRM_CI+REC_ERROR", eDxDRM_CIPLUS_CCV2_RECORD_ERROR},
			{NULL, 0}
		}
	};
	static DxEnumStringTable_t s_DxDRM_CIPLUS_DrmType_t =
	{
		(HCHAR*)DxNAMEOF(DxDRM_CIPLUS_DrmType_e),
		{
			{"DRM_CI+URI"    , eDxDRM_CIPLUS_DRM_TYPE_URI},
			{"DRM_CI+RECCTRL", eDxDRM_CIPLUS_DRM_TYPE_RECCTRL},
			{"DRM_CI+LICENSE", eDxDRM_CIPLUS_DRM_TYPE_LICENSE},
			{"DRM_CI+RATING" , eDxDRM_CIPLUS_DRM_TYPE_PARENTALCTRL},
			{NULL, 0}
		}
	};
	static HBOOL	s_initDone;

	if (!s_initDone)
	{
		s_initDone = TRUE;
		dlib_RegisterEnumTable(&s_DxDRM_Type_e);
		dlib_RegisterEnumTable(&s_DxDRM_CIPLUS_Write_e);
		dlib_RegisterEnumTable(&s_DxDRM_CIPLUS_Record_e);
		dlib_RegisterEnumTable(&s_DxDRM_CIPLUS_DrmType_t);
	}
}

HERROR			DLIB_DRM_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// TODO:
	return ERR_FAIL;
}

HERROR			DLIB_DRM_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	// TODO:
	return ERR_FAIL;
}

HERROR			DLIB_DRM_MakeType (HUID uid, const void *src, HUINT32 srcSize, void **type)
{

	return ERR_FAIL;
}

void			DLIB_DRM_FreeType (void *type)
{
	DxDRM_Delete(type);
}

void			DLIB_DRM_Print (FILE *fp, const void *t)
{
	DxDRM_t	*drm;

	drm = (DxDRM_t *)t;
	if (drm)
	{
		fprintf(fp,
			"\tDxDRM_t(0x%X) type:%s\n"
			, (int)drm
			, dlib_ConvertEnum2Str(DxNAMEOF(DxDRM_Type_e), drm->type)
		);
	}
}


static HCHAR *	dlib_drm_Encrypt (const HCHAR *json, const HxCryptKey_t *key)
{
	HUINT8	*encrypted;
	HUINT32	size;
	HCHAR	*base64;

	ERROR_IF(HxSTD_StrEmpty(json), NULL);

	size      = strlen(json) + 1;
	ERROR_IF(!HLIB_JSON_IsJSON(json, size - 1), NULL);

	encrypted = (HUINT8 *)DLIB_Malloc(strlen(json) + 1);
	ERROR_IF(encrypted == NULL, NULL);

	switch (key->type)
	{
	case HxCRYPT_DES:
		HLIB_CRYPT_DES_EncryptData((char *)key->u.DES, (HUINT8 *)json, encrypted, (HINT32)size);
		break;

	case HxCRYPT_3DES:
		HLIB_CRYPT_DES3_EncryptData((char *)key->u.DES3, (HUINT8 *)json, encrypted, (HINT32)size);
		break;

	case HxCRYPT_AES:
	default:
		HxLOG_Error("%s():%d unknown type:%d\n", __FUNCTION__, __LINE__, key->type);
		DLIB_Free(encrypted);
		return NULL;
	}

	base64 = HLIB_MATH_EncodeBase64Easy(encrypted, size);
	DLIB_Free(encrypted);

	HxLOG_Debug("%s(%s,%s) -> %s\n", __FUNCTION__
		, json
		, key->type == HxCRYPT_DES  ? "DES"
		: key->type == HxCRYPT_3DES ? "3DES"
		: key->type == HxCRYPT_AES  ? "AES" : "Unknown"
		, base64
	);
	return base64;
}

static HCHAR *	dlib_drm_Decrypt (const HCHAR *base64, const HxCryptKey_t *key)
{
	HUINT8	*encrypted;
	HUINT8	*decrypted;
	HUINT32	size;

	ERROR_IF(!HLIB_MATH_IsBase64(base64), NULL);

	encrypted = (HUINT8 *)HLIB_MATH_DecodeBase64Easy(base64, &size);
	ERROR_IF(encrypted == NULL || size == 0, NULL);
	decrypted = (HUINT8 *)DLIB_Calloc(size);
	if (decrypted == NULL)
	{
		HLIB_MATH_FreeBase64(encrypted);
		return NULL;
	}

	switch (key->type)
	{
	case HxCRYPT_DES:
		HLIB_CRYPT_DES_DecryptData((char *)key->u.DES, encrypted, decrypted, (HINT32)size);
		break;

	case HxCRYPT_3DES:
		HLIB_CRYPT_DES3_DecryptData((char *)key->u.DES3, encrypted, decrypted, (HINT32)size);
		break;

	case HxCRYPT_AES:
	default:
		HxLOG_Error("%s():%d unknown type:%d\n", __FUNCTION__, __LINE__, key->type);
		DLIB_Free(decrypted);
		HLIB_MATH_FreeBase64(encrypted);
		return NULL;
	}
	HLIB_MATH_FreeBase64(encrypted);

	if (!HLIB_JSON_IsJSON(decrypted, size - 1))
	{
		HxLOG_Error("%s():%d Decrypt fail\n", __FUNCTION__, __LINE__);
		HLIB_LOG_Dump(decrypted, size, 0, TRUE);
		DLIB_Free(decrypted);
		return NULL;
	}

	HxLOG_Debug("%s(%s,%s) -> %s\n", __FUNCTION__
		, base64
		, key->type == HxCRYPT_DES  ? "DES"
		: key->type == HxCRYPT_3DES ? "3DES"
		: key->type == HxCRYPT_AES  ? "AES" : "Unknown"
		, decrypted
	);
	return (HCHAR *)decrypted;
}

static HERROR	dlib_drm_EncodeAribCpInfo (HINT32 writer, const DxDRM_AribCpInfo_t *arib)
{
	HxJSONWRITER_Object(writer, "AribCpInfo");

	HxJSONWRITER_Integer(writer, "usSvcType", arib->usSvcType);
	HxJSONWRITER_Integer(writer, "ucCopyControlType", arib->ucCopyControlType);
	HxJSONWRITER_Integer(writer, "ucDigitalRecording", arib->ucDigitalRecording);
	HxJSONWRITER_Integer(writer, "ucAnalogRecording", arib->ucAnalogRecording);
	HxJSONWRITER_Integer(writer, "ucCopyRestrictionMode", arib->ucCopyRestrictionMode);
	HxJSONWRITER_Integer(writer, "ucImageContraintToken", arib->ucImageContraintToken);
	HxJSONWRITER_Integer(writer, "ucEncryptionMode", arib->ucEncryptionMode);
	HxJSONWRITER_Integer(writer, "ucRetentionMode", arib->ucRetentionMode);
	HxJSONWRITER_Integer(writer, "ucRetentionState", arib->ucRetentionState);

	HxJSONWRITER_ObjectEnd(writer);

	return ERR_OK;
}

static DxDRM_AribCpInfo_t * dlib_drm_DecodeAribCpInfo (DxDRM_AribCpInfo_t *arib, HxJSON_t json)
{
	arib->usSvcType =  (HUINT16)HLIB_JSON_Object_GetInteger(json, "usSvcType");
	arib->ucCopyControlType =  (HUINT8)HLIB_JSON_Object_GetInteger(json, "ucCopyControlType");
	arib->ucDigitalRecording =  (HUINT8)HLIB_JSON_Object_GetInteger(json, "ucDigitalRecording");
	arib->ucAnalogRecording =  (HUINT8)HLIB_JSON_Object_GetInteger(json, "ucAnalogRecording");
	arib->ucCopyRestrictionMode =  (HUINT8)HLIB_JSON_Object_GetInteger(json, "ucCopyRestrictionMode");
	arib->ucImageContraintToken =  (HUINT8)HLIB_JSON_Object_GetInteger(json, "ucImageContraintToken");
	arib->ucEncryptionMode =  (HUINT8)HLIB_JSON_Object_GetInteger(json, "ucEncryptionMode");
	arib->ucRetentionMode =  (HUINT8)HLIB_JSON_Object_GetInteger(json, "ucRetentionMode");
	arib->ucRetentionState =  (HUINT8)HLIB_JSON_Object_GetInteger(json, "ucRetentionState");

	return arib;
}


static HERROR	dlib_drm_EncodeCIPLUS (HINT32 writer, const DxDRM_CiPlus_t *ciplus)
{
	HxJSONWRITER_Object(writer, "CI+");
		HxJSONWRITER_String (writer, "eCiPlusDrmType", dlib_ConvertEnum2Str(DxNAMEOF(DxDRM_CIPLUS_DrmType_e), ciplus->eCiPlusDrmType));
		HxJSONWRITER_Integer(writer, "ucCiPlusDrmInfoVer", ciplus->ucCiPlusDrmInfoVer);
		if (!HxSTD_StrEmpty(ciplus->stCiPlusCamID.aucBindedCamId))
		{
			HxJSONWRITER_Object(writer, "stCiPlusCamID");
				HxJSONWRITER_Boolean(writer, "bCamNeedContent", ciplus->stCiPlusCamID.bCamNeedContent);
				HxJSONWRITER_Binary(writer, "aucBindedCamId", ciplus->stCiPlusCamID.aucBindedCamId, DxDRM_CIPLUS_CAMID_LEN);
			HxJSONWRITER_ObjectEnd(writer);
		}
		switch (ciplus->eCiPlusDrmType)
		{
		case eDxDRM_CIPLUS_DRM_TYPE_URI:
			HxJSONWRITER_Object(writer, "stCiPlusUriInfo");
				HxJSONWRITER_Integer(writer, "ucMode", ciplus->ciplusDrmInfo.stCiPlusUriInfo.ucMode);
				HxJSONWRITER_Integer(writer, "ucAps", ciplus->ciplusDrmInfo.stCiPlusUriInfo.ucAps);
				HxJSONWRITER_Integer(writer, "ucEmi", ciplus->ciplusDrmInfo.stCiPlusUriInfo.ucEmi);
				HxJSONWRITER_Integer(writer, "ucIct", ciplus->ciplusDrmInfo.stCiPlusUriInfo.ucIct);
				HxJSONWRITER_Integer(writer, "ucRct", ciplus->ciplusDrmInfo.stCiPlusUriInfo.ucRct);
				HxJSONWRITER_Integer(writer, "ucDot", ciplus->ciplusDrmInfo.stCiPlusUriInfo.ucDot);
				HxJSONWRITER_Integer(writer, "ulMinutesForRL", ciplus->ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL);
			HxJSONWRITER_ObjectEnd(writer);
			break;

		case eDxDRM_CIPLUS_DRM_TYPE_RECCTRL:
			HxJSONWRITER_Object(writer, "stCiPlusRecCtrl");
				HxJSONWRITER_String(writer, "eCiPlusCcRecInfo"
					, dlib_ConvertEnum2Str(DxNAMEOF(DxDRM_CIPLUS_Record_e)
						, ciplus->ciplusDrmInfo.stCiPlusRecCtrl.eCiPlusCcRecInfo
					)
				);
			HxJSONWRITER_ObjectEnd(writer);
			break;

		case eDxDRM_CIPLUS_DRM_TYPE_LICENSE:
			HxJSONWRITER_Object(writer, "stCiPlusLicenseInfo");
				HxJSONWRITER_Integer(writer, "usProgramNum", ciplus->ciplusDrmInfo.stCiPlusLicenseInfo.usProgramNum);
				if (ciplus->ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData && ciplus->ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength)
				{
					HxJSONWRITER_Binary(writer, "pucLicenseData"
						, ciplus->ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData
						, ciplus->ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength
					);
				}
				HxJSONWRITER_Binary(writer, "aucBindedCamId", ciplus->ciplusDrmInfo.stCiPlusLicenseInfo.aucBindedCamId, 8);
			HxJSONWRITER_ObjectEnd(writer);
			break;

		case eDxDRM_CIPLUS_DRM_TYPE_PARENTALCTRL:
			HxJSONWRITER_Object(writer, "stCiPlusParentalCtrlInfo");
				HxJSONWRITER_Integer(writer, "utTimeStamp", ciplus->ciplusDrmInfo.stCiPlusParentalCtrlInfo.utTimeStamp);
				HxJSONWRITER_Integer(writer, "usProgramNum", ciplus->ciplusDrmInfo.stCiPlusParentalCtrlInfo.usProgramNum);
				HxJSONWRITER_Integer(writer, "ucAgeLimit", ciplus->ciplusDrmInfo.stCiPlusParentalCtrlInfo.ucAgeLimit);
				HxJSONWRITER_Binary(writer, "aucPrivateData", ciplus->ciplusDrmInfo.stCiPlusParentalCtrlInfo.aucPrivateData, 15);
				HxJSONWRITER_Binary(writer, "aucBindedCamId", ciplus->ciplusDrmInfo.stCiPlusParentalCtrlInfo.aucBindedCamId, 8);
			HxJSONWRITER_ObjectEnd(writer);
			break;

		default:
			break;
		}
	HxJSONWRITER_ObjectEnd(writer);
	return ERR_OK;
}

static DxDRM_CiPlus_t * dlib_drm_DecodeCIPLUS (DxDRM_CiPlus_t *ciplus, HxJSON_t json)
{
	const HCHAR	*text;
	HxJSON_t	obj;

	ERROR_IF(!json, NULL);

	text = HLIB_JSON_Object_GetString(json, "eCiPlusDrmType");
	ERROR_IF(text == NULL, NULL);
	ciplus->eCiPlusDrmType = dlib_ConvertStr2Enum(DxNAMEOF(DxDRM_CIPLUS_DrmType_e), text);
	ciplus->ucCiPlusDrmInfoVer = (HUINT8)HLIB_JSON_Object_GetInteger(json, "ucCiPlusDrmInfoVer");

	if ((obj = HLIB_JSON_Object_GetObject(json, "stCiPlusCamID")) != NULL)
	{
		ciplus->stCiPlusCamID.bCamNeedContent = HLIB_JSON_Object_GetBoolean(obj, "bCamNeedContent");
		HLIB_JSON_Object_GetBinary(obj, "aucBindedCamId", ciplus->stCiPlusCamID.aucBindedCamId, DxDRM_CIPLUS_CAMID_LEN);
	}

	switch (ciplus->eCiPlusDrmType)
	{
	case eDxDRM_CIPLUS_DRM_TYPE_URI:
		if ((obj = HLIB_JSON_Object_GetObject(json, "stCiPlusUriInfo")) != NULL)
		{
			ciplus->ciplusDrmInfo.stCiPlusUriInfo.ucMode = (HUINT8)HLIB_JSON_Object_GetInteger(obj, "ucMode");
			ciplus->ciplusDrmInfo.stCiPlusUriInfo.ucAps = (HUINT8)HLIB_JSON_Object_GetInteger(obj, "ucAps");
			ciplus->ciplusDrmInfo.stCiPlusUriInfo.ucEmi = (HUINT8)HLIB_JSON_Object_GetInteger(obj, "ucEmi");
			ciplus->ciplusDrmInfo.stCiPlusUriInfo.ucIct = (HUINT8)HLIB_JSON_Object_GetInteger(obj, "ucIct");
			ciplus->ciplusDrmInfo.stCiPlusUriInfo.ucRct = (HUINT8)HLIB_JSON_Object_GetInteger(obj, "ucRct");
			ciplus->ciplusDrmInfo.stCiPlusUriInfo.ucDot = (HUINT8)HLIB_JSON_Object_GetInteger(obj, "ucDot");
			ciplus->ciplusDrmInfo.stCiPlusUriInfo.ulMinutesForRL = HLIB_JSON_Object_GetInteger(obj, "ulMinutesForRL");
		}
		break;

	case eDxDRM_CIPLUS_DRM_TYPE_RECCTRL:
		if ((obj = HLIB_JSON_Object_GetObject(json, "stCiPlusRecCtrl")) != NULL)
		{
			text = HLIB_JSON_Object_GetString(obj, "eCiPlusCcRecInfo");
			if (text)
				ciplus->ciplusDrmInfo.stCiPlusRecCtrl.eCiPlusCcRecInfo = dlib_ConvertStr2Enum(DxNAMEOF(DxDRM_CIPLUS_Record_e), text);
		}
		break;

	case eDxDRM_CIPLUS_DRM_TYPE_LICENSE:
		if ((obj = HLIB_JSON_Object_GetObject(json, "stCiPlusLicenseInfo")) != NULL)
		{
			HUINT32	size = 0;

			ciplus->ciplusDrmInfo.stCiPlusLicenseInfo.usProgramNum   = (HUINT16)HLIB_JSON_Object_GetInteger(obj, "usProgramNum");
			ciplus->ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData = (HUINT8*)HLIB_JSON_Object_GetBinaryDup(obj, "pucLicenseData", &size);
			ciplus->ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength= (HUINT16)size;
			HLIB_JSON_Object_GetBinary(obj, "aucBindedCamId", ciplus->ciplusDrmInfo.stCiPlusLicenseInfo.aucBindedCamId, 8);
		}
		break;

	case eDxDRM_CIPLUS_DRM_TYPE_PARENTALCTRL:
		if ((obj = HLIB_JSON_Object_GetObject(json, "stCiPlusParentalCtrlInfo")) != NULL)
		{
			ciplus->ciplusDrmInfo.stCiPlusParentalCtrlInfo.utTimeStamp  = HLIB_JSON_Object_GetInteger(obj, "utTimeStamp");
			ciplus->ciplusDrmInfo.stCiPlusParentalCtrlInfo.usProgramNum = (HUINT16)HLIB_JSON_Object_GetInteger(obj, "usProgramNum");
			ciplus->ciplusDrmInfo.stCiPlusParentalCtrlInfo.ucAgeLimit   = (HUINT8 )HLIB_JSON_Object_GetInteger(obj, "ucAgeLimit");
			HLIB_JSON_Object_GetBinary(obj, "aucPrivateData", ciplus->ciplusDrmInfo.stCiPlusParentalCtrlInfo.aucPrivateData, 15);
			HLIB_JSON_Object_GetBinary(obj, "aucBindedCamId", ciplus->ciplusDrmInfo.stCiPlusParentalCtrlInfo.aucBindedCamId, 8);
		}
		break;

	default:
		HxLOG_Error("Unknown type!\n");
		return NULL;
	}
	return ciplus;
}

static HCHAR *	dlib_drm_Encode (HINT32 jsonWriter, const DxDRM_t *drm)
{
	HINT32	writer;

	if (jsonWriter)
		writer = jsonWriter;
	else
		writer = HLIB_JSONWRITER_Open();
	if (!writer)
		return NULL;

	HxJSONWRITER_ObjectBegin(writer);
		HxJSONWRITER_String(writer, "type", dlib_ConvertEnum2Str(DxNAMEOF(DxDRM_Type_e), drm->type));
		switch (drm->type)
		{
		case eDxDRM_TYPE_CIPLUS:
			(void) dlib_drm_EncodeCIPLUS(writer, &(drm->u.ciplus));
			break;

		case eDxDRM_TYPE_ARIB_CPINFO:
			(void) dlib_drm_EncodeAribCpInfo(writer, &(drm->u.AribCpInfo));
			break;

		case eDxDRM_TYPE_V1:
		default:
			HxJSONWRITER_Object(writer, "v1");
			HxJSONWRITER_Integer(writer, "type", drm->u.v1.type);
			HxJSONWRITER_Binary(writer, "data", drm->u.v1.reserved, 60);
			HxJSONWRITER_ObjectEnd(writer);
			break;
		}
	HxJSONWRITER_ObjectEnd(writer);

	if (!jsonWriter)
	{
		HCHAR	*json = DLIB_StrDup(HLIB_JSONWRITER_GetJSON(writer));

		HLIB_JSONWRITER_Close(writer);
		return json;
	}
	return NULL;
}


static DxDRM_t *dlib_drm_Decode (DxDRM_t *drm, HxJSON_t json)
{
	const HCHAR *text;
	HxJSON_t	v1;

	text = HLIB_JSON_Object_GetString(json, "type");
	ERROR_IF(text == NULL, NULL);
	drm->type = dlib_ConvertStr2Enum(DxNAMEOF(DxDRM_Type_e), text);

	switch (drm->type)
	{
	case eDxDRM_TYPE_CIPLUS:
		ERROR_IF(dlib_drm_DecodeCIPLUS(&(drm->u.ciplus), HLIB_JSON_Object_GetObject(json, "CI+")) == NULL, NULL);
		break;

	case eDxDRM_TYPE_ARIB_CPINFO:
		ERROR_IF(dlib_drm_DecodeAribCpInfo(&(drm->u.AribCpInfo), HLIB_JSON_Object_GetObject(json, "AribCpInfo")) == NULL, NULL);
		break;

	case eDxDRM_TYPE_V1:
	default:
		v1 = HLIB_JSON_Object_GetObject(json, "v1");
		if (v1)
		{
			drm->u.v1.type = HLIB_JSON_Object_GetInteger(v1, "type");
			ERROR_IF(HLIB_JSON_Object_GetBinary(v1, "data", drm->u.v1.reserved, 60) == 0, NULL);
		}
		break;
	}
	return drm;
}

HCHAR *		DxDRM_Encode (HINT32 jsonWriter, const DxDRM_t *drm, const HxCryptKey_t *key)
{
	HINT32	writer;
	HCHAR	*jsonTemp;
	const HCHAR	*text;

	HxLOG_Assert(drm);

	if (jsonWriter)
		writer = jsonWriter;
	else
		writer = HLIB_JSONWRITER_Open();

	if (key)	// key°¡ ÀÖÀ¸¸é encryptionÇÑ´Ù
	{
		HCHAR *encrypted;

		jsonTemp = dlib_drm_Encode(0, drm);
		encrypted = dlib_drm_Encrypt(jsonTemp, key);
		if (jsonTemp)
			DLIB_Free(jsonTemp);

		if (encrypted == NULL)
		{
			HxLOG_Error("%s():%d error\n", __FUNCTION__, __LINE__);
			if (!jsonWriter)
				HLIB_JSONWRITER_Close(writer);
			return NULL;
		}
		HxJSONWRITER_ObjectBegin(writer);
			HxJSONWRITER_String(writer, "drm", encrypted);
		HxJSONWRITER_ObjectEnd(writer);
		DLIB_Free(encrypted);
	}
	else
	{
		//	write´Â Ç×»ó Á¸ÀçÇÑ´Ù.
		jsonTemp = dlib_drm_Encode(writer, drm);
		if (jsonTemp)
			DLIB_Free(jsonTemp);
	}

	text = HLIB_JSONWRITER_Error(writer);
	if (text)
	{
		HxLOG_Error("%s():%d error:%s\n", __FUNCTION__, __LINE__, text);
		if (!jsonWriter)
			HLIB_JSONWRITER_Close(writer);
		return NULL;
	}
	if (!jsonWriter)
	{
		HCHAR	*json = DLIB_StrDup(HLIB_JSONWRITER_GetJSON(writer));

		HLIB_JSONWRITER_Close(writer);
		return json;
	}
	return NULL;
}


DxDRM_t *	DxDRM_Decode (DxDRM_t *drm, HxJSON_t json, const HxCryptKey_t *key)
{
	DxDRM_t		*d;
	DxDRM_t		*ret;
	HxJSON_t	obj;

	HxLOG_Assert(json);

	if (key)
	{
		const HCHAR *base64;
		HCHAR *decrypted;

		base64 = HLIB_JSON_Object_GetString(json, "drm");
		ERROR_IF(base64 == NULL, NULL);

		decrypted = dlib_drm_Decrypt(base64, key);
		ERROR_IF(decrypted == NULL, NULL);

		obj = HLIB_JSON_Decode(decrypted);
		DLIB_Free(decrypted);
	} else obj = json;

	if (drm == NULL)
		d = (DxDRM_t *)DLIB_Calloc(sizeof(DxDRM_t));
	else
		d = drm;

	if (d == NULL)
	{
		if (key)
			HLIB_JSON_Delete(obj);
		return NULL;
	}

	ret = dlib_drm_Decode(d, obj);
	if (key)
		HLIB_JSON_Delete(obj);
	if (ret == NULL)
	{
		if (drm == NULL)
			DLIB_Free(d);
	}
	return ret;

}

DxDRM_t	*	DxDRM_Clone  (const DxDRM_t *drm)
{
	DxDRM_t *clone;

	clone = HLIB_STD_MemDupEx(drm, sizeof(DxDRM_t), DLIB_Malloc_CB);
	if (clone)
	{
		switch(drm->type)
		{
		case eDxDRM_TYPE_CIPLUS:
			if (drm->u.ciplus.eCiPlusDrmType == eDxDRM_CIPLUS_DRM_TYPE_LICENSE
				&& drm->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData
				&& drm->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength)
			{
				clone->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData = (HUINT8 *)HLIB_STD_MemDupEx(
											drm->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData
											, drm->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength
											, DLIB_Malloc_CB
										);
			}
			break;

		default:
			break;
		}
	}
	return clone;
}

void		DxDRM_Delete (DxDRM_t *drm)
{
	if (drm == NULL)
		return;

	switch(drm->type)
	{
	case eDxDRM_TYPE_CIPLUS:
		if (drm->u.ciplus.eCiPlusDrmType == eDxDRM_CIPLUS_DRM_TYPE_LICENSE
			&& drm->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData)
		{
			DLIB_Free(drm->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData);
		}
		break;

	default:
		break;
	}
	DLIB_Free(drm);
}

HINT32	DxDRM_Compare  (const DxDRM_t *lhs, const DxDRM_t *rhs)
{
	HINT32	diff;

	if (lhs == rhs)
		return 0;

	if (lhs == NULL)
		return -1;

	if (rhs == NULL)
		return +1;

	diff = lhs->type - rhs->type;
	if (diff == 0)
		return 0;

	switch (lhs->type)
	{
	case eDxDRM_TYPE_CIPLUS:
		if (lhs->u.ciplus.eCiPlusDrmType == eDxDRM_CIPLUS_DRM_TYPE_LICENSE
			&& lhs->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData
			&& lhs->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength
			&& rhs->u.ciplus.eCiPlusDrmType == eDxDRM_CIPLUS_DRM_TYPE_LICENSE
			&& rhs->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData
			&& rhs->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength
			&& lhs->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength
			== rhs->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength)
		{
			diff = memcmp(
					  lhs->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData
					, rhs->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.pucLicenseData
					, lhs->u.ciplus.ciplusDrmInfo.stCiPlusLicenseInfo.usLicenseLength
				);
			if (diff != 0)
				return diff;
		}
	default:
		break;
	}
	return memcmp(lhs, rhs, sizeof(DxDRM_t));
}

HCHAR *	DxDRM_Encrypt (const HCHAR *json, const HxCryptKey_t *key)
{
	HxLOG_Assert(json && key);

	return dlib_drm_Encrypt(json, key);
}

HCHAR *	DxDRM_Decrypt (const HCHAR *base64, const HxCryptKey_t *key)
{
	HxLOG_Assert(base64 && key);

	return dlib_drm_Decrypt(base64, key);
}

void	DxDRM_FreeData(void *p)
{
	HLIB_MATH_FreeBase64(p);
}

#ifdef	CONFIG_DEBUG
static DxDRM_t *	dlib_port_drm_make_drm (DxDRM_t *drm, HINT32 i, const HCHAR **desc)
{
	DxDRM_CiPlus_t				*cip = &(drm->u.ciplus);
	DxDRM_CIPLUS_CamID_t		*cam = &(cip->stCiPlusCamID);
	DxDRM_CIPLUS_URI_t			*uri = &(cip->ciplusDrmInfo.stCiPlusUriInfo);
	DxDRM_CIPLUS_RecordCtrl_t	*rec = &(cip->ciplusDrmInfo.stCiPlusRecCtrl);
	DxDRM_CIPLUS_License_t		*lic = &(cip->ciplusDrmInfo.stCiPlusLicenseInfo);
	DxDRM_CIPLUS_Rating_t		*rat = &(cip->ciplusDrmInfo.stCiPlusParentalCtrlInfo);

	memset(drm, 0, sizeof(DxDRM_t));


	switch (i)
	{
	case 0: *desc = "v1: basic";
		drm->type = eDxDRM_TYPE_V1;
		strcpy(drm->u.v1.reserved, "drm->1.u.v1.reserved");
		return drm;

	case 1: *desc = "CI+: URI";
		drm->type = eDxDRM_TYPE_CIPLUS;
		cip->eCiPlusDrmType = eDxDRM_CIPLUS_DRM_TYPE_URI;
		uri->ucMode = 1;
		uri->ucAps  = 2;
		uri->ucEmi  = 3;
		uri->ucIct  = 4;
		uri->ucRct  = 5;
		uri->ucDot  = 6;
		uri->ulMinutesForRL = 77;
		return drm;

	case 2: *desc = "CI+: RECCTRL; Record OK";
		drm->type = eDxDRM_TYPE_CIPLUS;
		cip->eCiPlusDrmType = eDxDRM_CIPLUS_DRM_TYPE_RECCTRL;
		rec->eCiPlusCcRecInfo = eDxDRM_CIPLUS_CCV1_RECORD_OK;
		return drm;

	case 3: *desc = "CI+: RECCTRL; Record Wait";
		drm->type = eDxDRM_TYPE_CIPLUS;
		cip->eCiPlusDrmType = eDxDRM_CIPLUS_DRM_TYPE_RECCTRL;
		rec->eCiPlusCcRecInfo = eDxDRM_CIPLUS_CCV2_RECORD_WAIT;
		return drm;

	case 4: *desc = "CI+: RECCTRL; Record Start";
		drm->type = eDxDRM_TYPE_CIPLUS;
		cip->eCiPlusDrmType = eDxDRM_CIPLUS_DRM_TYPE_RECCTRL;
		rec->eCiPlusCcRecInfo = eDxDRM_CIPLUS_CCV2_RECORD_START;
		return drm;

	case 5: *desc = "CI+: RECCTRL; Record Error";
		drm->type = eDxDRM_TYPE_CIPLUS;
		cip->eCiPlusDrmType = eDxDRM_CIPLUS_DRM_TYPE_RECCTRL;
		rec->eCiPlusCcRecInfo = eDxDRM_CIPLUS_CCV2_RECORD_ERROR;
		return drm;

	case 6: *desc = "CI+: LICENSE; has license data";
		drm->type = eDxDRM_TYPE_CIPLUS;
		cip->eCiPlusDrmType = eDxDRM_CIPLUS_DRM_TYPE_LICENSE;
		lic->usProgramNum = 88;
		lic->pucLicenseData  = (HUINT8 *)DLIB_StrDup("ciplus-license-test");
		lic->usLicenseLength = strlen("ciplus-license-test") + 1;
		strcpy(lic->aucBindedCamId, "cam:2");
		return drm;

	case 7: *desc = "CI+: LICENSE; no license data";
		drm->type = eDxDRM_TYPE_CIPLUS;
		cip->eCiPlusDrmType = eDxDRM_CIPLUS_DRM_TYPE_LICENSE;
		lic->usProgramNum = 88;
		strcpy(lic->aucBindedCamId, "cam:2");
		return drm;

	case 8: *desc = "CI+: RATING; basic";
		drm->type = eDxDRM_TYPE_CIPLUS;
		cip->eCiPlusDrmType = eDxDRM_CIPLUS_DRM_TYPE_PARENTALCTRL;
		rat->utTimeStamp = 0x1234;
		rat->usProgramNum = 88;
		rat->ucAgeLimit   = 19;
		strcpy(rat->aucPrivateData, "private");
		strcpy(rat->aucBindedCamId, "cam:1");
		return drm;

	case 9: *desc = "CI+: URI; with CAM ID";
		drm->type = eDxDRM_TYPE_CIPLUS;
		cip->eCiPlusDrmType = eDxDRM_CIPLUS_DRM_TYPE_URI;
		uri->ucMode = 1;
		uri->ucAps  = 2;
		uri->ucEmi  = 3;
		uri->ucIct  = 4;
		uri->ucRct  = 5;
		uri->ucDot  = 6;
		uri->ulMinutesForRL = 77;
		cam->bCamNeedContent = TRUE;
		strcpy(cam->aucBindedCamId, "cam:X");
		return drm;

	case 10: *desc = "Japan : AribCpInfo";

		drm->type = eDxDRM_TYPE_ARIB_CPINFO;
		drm->u.AribCpInfo.usSvcType = 0x1;
		drm->u.AribCpInfo.ucCopyControlType = 0x2;
		drm->u.AribCpInfo.ucDigitalRecording = 0x3;
		drm->u.AribCpInfo.ucAnalogRecording = 0x4;
		drm->u.AribCpInfo.ucCopyRestrictionMode = 0x5;
		drm->u.AribCpInfo.ucImageContraintToken = 0x6;
		drm->u.AribCpInfo.ucEncryptionMode = 0x7;
		drm->u.AribCpInfo.ucRetentionMode = 0x8;
		drm->u.AribCpInfo.ucRetentionState = 0x9;
		return drm;

	default:
		break;
	}
	return NULL;
}

static void dlib_port_drm_test_run (const HxCryptKey_t *key)
{
	DxDRM_t 	drmBuf;
	DxDRM_t		*drm;
	DxDRM_t		*decoded;
	HCHAR		*encoded;
	HxJSON_t	json;
	HINT32		i;
	HINT32		diff;
	const HCHAR	*desc;

	for (i = 0 ; (drm = dlib_port_drm_make_drm(&drmBuf, i, &desc)) != NULL ; i++)
	{
		encoded = DxDRM_Encode(0, drm, key);
		if (encoded == NULL)
		{
			printf("\t<[01;31mFAIL[00;39;49m> %s; DxDRM_Encode() failed.\n", desc);
			continue;
		}

		json = HLIB_JSON_Decode(encoded);
		DLIB_Free(encoded);
		if (!json)
		{
			printf("\t<[01;31mFAIL[00;39;49m> %s; encoded text is invalid JSON\n", desc);
			continue;
		}

		decoded = DxDRM_Decode(NULL, json, key);
		HLIB_JSON_Delete(json);
		if (decoded == NULL)
		{
			printf("\t<[01;31mFAIL[00;39;49m> %s; DxDRM_Decode() failed.\n", desc);
			continue;
		}

		diff = DxDRM_Compare(drm, decoded);
		if (diff != 0)
		{
			printf("\t<[01;31mFAIL[00;39;49m> %s; DxDRM_Decode() failed.\n", desc);
			HLIB_LOG_Dump((HUINT8 *)drm, sizeof(DxDRM_t), 0, TRUE);
			HLIB_LOG_Dump((HUINT8 *)decoded, sizeof(DxDRM_t), 0, TRUE);
			DxDRM_Delete(decoded);
			continue;
		}
		DxDRM_Delete(decoded);

		printf("\t<[01;32m OK [00;39;49m> %s\n", desc);
	}
}

int	dlib_port_drm_test (int c, ...)
{
	HINT32			i;
	HxCryptKey_t	key;

	printf("[01;34m %s TEST [00;39;49m \n", __HxFILE__);
	printf("[01;33m TEST DxDRM_Encode(), DxDRM_Decode() without crypt [00;39;49m \n");
	dlib_port_drm_test_run(NULL);

	printf("[01;33m TEST DxDRM_Encode(), DxDRM_Decode() with DES key(\"012345678\")[00;39;49m \n");
	memset(&key, 0, sizeof(HxCryptKey_t));
	key.type = HxCRYPT_DES;
	for (i = 0 ; i < 8 ; i++)
		key.u.DES[i] = '0' + i;
	dlib_port_drm_test_run(&key);

	printf("[01;33m TEST DxDRM_Encode(), DxDRM_Decode() with 3DES key(\"ABCDEFGHIJKLMNOPQRSTUVWX\")[00;39;49m \n");
	memset(&key, 0, sizeof(HxCryptKey_t));
	key.type = HxCRYPT_3DES;
	for (i = 0 ; i < 24 ; i++)
		key.u.DES3[i] = 'A' + i;
	dlib_port_drm_test_run(&key);

	return 0;
}
#endif
#endif
