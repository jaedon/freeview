
#include <hlib.h>
#include "_dlib_port_udbmodule.h"

#define DLIB_UDB_OWNER		"owner"
#define DLIB_UDB_VALUE		"value"
#define DLIB_UDB_PROCESS	"process"
#define DLIB_UDB_DATA		"data"


STATIC HxHASH_t	*s_pstDBHashTable = NULL;
STATIC HINT32	s_nDBEncodedStream = 0;

static HBOOL	dlib_udbmodule_updateItem(const HCHAR *pszProcName, const HCHAR *pszKey, HxObjectTag_e eTag, void *pvData)
{
	dlib_udbmodule_item_t		*object;

	if (eTag != HxOBJECT_TYPE_STR && eTag != HxOBJECT_TYPE_INT)
		return FALSE;

	HxLOG_Debug("update item [%s : %s : %d]\n", pszProcName, pszKey, eTag);

	object = (dlib_udbmodule_item_t*)HLIB_HASH_Lookup(s_pstDBHashTable, (void*)pszKey);
	if (object == NULL)
	{
		object = DLIB_Calloc(sizeof(dlib_udbmodule_item_t));
		if(pszProcName) object->pszProcName = DLIB_StrDup(pszProcName);
		else pszProcName = NULL;
		object->pszKey      = DLIB_StrDup(pszKey);
		object->eTag        = eTag;
		if (object->eTag == HxOBJECT_TYPE_STR)
			object->u.pszString = DLIB_StrDup((HCHAR*)pvData);
		else
		if (object->eTag == HxOBJECT_TYPE_INT)
			object->u.integer = (HINT32)pvData;

		HLIB_HASH_Insert(s_pstDBHashTable, (void*)object->pszKey, (void*)object);

		return TRUE;
	}
	else
	{
		if (pszProcName && HxSTD_StrCmp(object->pszProcName, pszProcName) != 0)
		{
			HxLOG_Warning("owner will be changed [%s --> %s]\n", object->pszProcName, pszProcName);
			DLIB_Free(object->pszProcName);
			object->pszProcName = DLIB_StrDup(pszProcName);
		}

		if (eTag != object->eTag)
		{
			HxLOG_Warning("data type will be changed [%d --> %d]\n", object->eTag, eTag);
		} else
		{
			if ((object->eTag == HxOBJECT_TYPE_INT) && (object->u.integer == (HINT32)pvData))
				return FALSE;
			if ((object->eTag == HxOBJECT_TYPE_STR) && (HxSTD_StrCmp(object->u.pszString, (HCHAR*)pvData) == 0))
				return FALSE;
		}

		if (object->eTag == HxOBJECT_TYPE_STR)
			DLIB_Free(object->u.pszString);

		object->eTag = eTag;
		if (eTag == HxOBJECT_TYPE_STR)
			object->u.pszString = DLIB_StrDup((HCHAR*)pvData);
		else
		if (eTag == HxOBJECT_TYPE_INT)
			object->u.integer = (HINT32)pvData;

		return TRUE;
	}
}

static HBOOL	dlib_udbmodule_addHash(const HCHAR *pszJsonText)
{
	HxJSON_t	jsonRoot;
	HxJSON_t	jsonObject;
	const HCHAR *pProcName = NULL;
	void		*iter;

	jsonRoot = HLIB_JSON_Decode(pszJsonText);
	if (jsonRoot == NULL)
	{
		HxLOG_Print("[%s;%d] data is not valid [%s]\n", __FUNCTION__, __LINE__, pszJsonText);
		return FALSE;
	}
	jsonObject = HxJSON_Object_Get(jsonRoot, DLIB_UDB_OWNER);
	if (jsonObject == NULL)
	{
		HxLOG_Print("[%s;%d] procees is not valid \n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	pProcName = HxJSON_String_Get(jsonObject);


	if(pProcName)
	{
		HxHASH_TableIter_t	hashIter;
		void	*pszIterKey;
		dlib_udbmodule_item_t	*pstIterInfo;

		HxLOG_Print("[%s;%d] procees name is [%s] \n", __FUNCTION__, __LINE__, pProcName);

		HLIB_HASH_IterInit(&hashIter, s_pstDBHashTable);
		while (HLIB_HASH_IterNext(&hashIter, (void **)&pszIterKey, (void **)&pstIterInfo))
		{
			if (pstIterInfo->pszProcName && HxSTD_StrCmp(pstIterInfo->pszProcName, pProcName) == 0)
				HLIB_HASH_IterRemove(&hashIter);
		}
 	}
	else
	{
		HxLOG_Warning("procees name is NULL \n");
	}

	jsonObject = HxJSON_Object_Get(jsonRoot, DLIB_UDB_DATA);



	if(pProcName && jsonObject)
	{
		for ( iter = HLIB_JSON_Object_Iterator(jsonObject)
				; iter
				; iter = HLIB_JSON_Object_NextIterator(jsonObject, iter) )
			{
				const HCHAR 	*key   = HLIB_JSON_Object_IteratorKey(iter);
				const HxJSON_t	jsValue  = HLIB_JSON_Object_IteratorValue(iter);

				HxLOG_Print("[%s;%d] key %s process %s", __FUNCTION__, __LINE__, key, pProcName);

				if (HxJSON_IsInteger(jsValue))
				{
					HxLOG_Print("%d\n", HxJSON_Integer_Get(jsValue));
					dlib_udbmodule_updateItem(pProcName, key, HxOBJECT_TYPE_INT, (void*)HxJSON_Integer_Get(jsValue));
				}
				else if (HxJSON_IsString(jsValue))
				{
					HxLOG_Print("%s\n", HxJSON_String_Get(jsValue));
					dlib_udbmodule_updateItem(pProcName, key, HxOBJECT_TYPE_STR, (void*)HxJSON_String_Get(jsValue));
				}
				else
				{
					HxLOG_Warning("No Data\n");
				}
			}
	}
	else
	{

		HxLOG_Print("[%s;%d] There in no data\n", __FUNCTION__, __LINE__);
	}

	for ( iter = HLIB_JSON_Object_Iterator(jsonRoot)
		; iter
		; iter = HLIB_JSON_Object_NextIterator(jsonRoot, iter) )
	{
		const HCHAR 	*key   = HLIB_JSON_Object_IteratorKey(iter);
		const HxJSON_t	jsValue  = HLIB_JSON_Object_IteratorValue(iter);

		if (HxSTD_StrCmp(DLIB_UDB_OWNER, key) == 0
			|| HxSTD_StrCmp(DLIB_UDB_DATA, key) == 0)
			continue;

		HxLOG_Print("[%s;%d] key %s process NULL", __FUNCTION__, __LINE__, key);


		if (HxJSON_IsInteger(jsValue))
		{
			HxLOG_Print("%d\n", HxJSON_Integer_Get(jsValue));
			dlib_udbmodule_updateItem(NULL, key, HxOBJECT_TYPE_INT, (void*)HxJSON_Integer_Get(jsValue));
		}
		else if (HxJSON_IsString(jsValue))
		{
			HxLOG_Print("%s\n", HxJSON_String_Get(jsValue));
			dlib_udbmodule_updateItem(NULL, key, HxOBJECT_TYPE_STR, (void*)HxJSON_String_Get(jsValue));
		}
		else
		{
			HxLOG_Warning("No Data\n");
		}
	}

	HLIB_JSON_Delete(jsonRoot);

	return TRUE;
}


static HBOOL	dlib_udbmodule_loadHash(const HCHAR *pszJsonText)
{
	HxJSON_t	jsonRoot;
	HBOOL		hErr = FALSE;

	jsonRoot = HLIB_JSON_Decode(pszJsonText);
	if (jsonRoot == NULL)
	{
		HxLOG_Error("data is not valid [%s]\n", pszJsonText);

		return FALSE;
	}

	//	loading data to hash.
	{
		void		*iter;

		for ( iter = HLIB_JSON_Object_Iterator(jsonRoot)
			; iter
			; iter = HLIB_JSON_Object_NextIterator(jsonRoot, iter) )
		{
			const HCHAR		*key   = HLIB_JSON_Object_IteratorKey(iter);
			const HxJSON_t	value  = HLIB_JSON_Object_IteratorValue(iter);
			const HxJSON_t	jsProc = HxJSON_Object_Get(value, DLIB_UDB_OWNER);
			const HxJSON_t	jsValue= HxJSON_Object_Get(value, DLIB_UDB_VALUE);

			HxLOG_Print("[%s;%d] key %s ", __FUNCTION__, __LINE__, key);

			if(HxJSON_IsObject(value))
			{
				HxLOG_Print("process %s\n", HxJSON_String_Get(jsProc));
				if (HxJSON_IsInteger(jsValue))
					dlib_udbmodule_updateItem(HxJSON_String_Get(jsProc), key, HxOBJECT_TYPE_INT, (void*)HxJSON_Integer_Get(jsValue));
				else
				if (HxJSON_IsString(jsValue))
					dlib_udbmodule_updateItem(HxJSON_String_Get(jsProc), key, HxOBJECT_TYPE_STR, (void*)HxJSON_String_Get(jsValue));
			}
			else
			{
				HxLOG_Print("process is NULL[%s]\n", key);
				if (HxJSON_IsInteger(value))
					dlib_udbmodule_updateItem(NULL, key, HxOBJECT_TYPE_INT, (void*)HxJSON_Integer_Get(value));
				else
				if (HxJSON_IsString(value))
					dlib_udbmodule_updateItem(NULL, key, HxOBJECT_TYPE_STR, (void*)HxJSON_String_Get(value));
			}
		}
		HLIB_JSON_Delete(jsonRoot);
	}

	return hErr;
}

static HERROR		dlib_udbmodule_save(void)
{
	dlib_udbmodule_item_t	*dbInfo = NULL;
	HxHASH_TableIter_t		iter;

	HLIB_JSONWRITER_Clear(s_nDBEncodedStream);
	HxJSONWRITER_ObjectBegin(s_nDBEncodedStream);
	HLIB_HASH_IterInit(&iter, s_pstDBHashTable);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&dbInfo))
	{
		if(dbInfo)
		{
			HxLOG_Print("dlib_udbmodule_save key[%s] proc[%s] tag[%d]\n", dbInfo->pszKey, dbInfo->pszProcName?dbInfo->pszProcName:"NULL", dbInfo->eTag);
			if(dbInfo->pszProcName)
			{
				HxJSONWRITER_Object(s_nDBEncodedStream, dbInfo->pszKey);
				HxJSONWRITER_String(s_nDBEncodedStream, "owner", dbInfo->pszProcName);

				if (dbInfo->eTag == HxOBJECT_TYPE_INT)
					HxJSONWRITER_Integer(s_nDBEncodedStream, "value", dbInfo->u.integer);
				else if (dbInfo->eTag == HxOBJECT_TYPE_STR)
					HxJSONWRITER_String(s_nDBEncodedStream,  "value", dbInfo->u.pszString);

				HxJSONWRITER_ObjectEnd(s_nDBEncodedStream);
			}
			else
			{
				if (dbInfo->eTag == HxOBJECT_TYPE_INT)
					HxJSONWRITER_Integer(s_nDBEncodedStream, dbInfo->pszKey, dbInfo->u.integer);
				else if (dbInfo->eTag == HxOBJECT_TYPE_STR)
					HxJSONWRITER_String(s_nDBEncodedStream,  dbInfo->pszKey, dbInfo->u.pszString);
			}
		}
	}
	HxJSONWRITER_ObjectEnd(s_nDBEncodedStream);

	HxLOG_Print("[%s;%d] dbEncoded : (%d)[%s]\n", __FUNCTION__, __LINE__, HLIB_JSONWRITER_GetLength(s_nDBEncodedStream) + 1, HLIB_JSONWRITER_GetJSON(s_nDBEncodedStream));
	return ERR_OK;

}

static HERROR		dlib_udbmodule_set(HUID uid, const void *src, HUINT32 size, void **dst, HUINT32 *dstSize)
{
	HCHAR	*pszJsonData;

	if(uid != 0)
		return ERR_FAIL;

	pszJsonData = DLIB_Calloc(size + 1);
	HxSTD_MemCopy(pszJsonData, src, size);
	dlib_udbmodule_addHash((const HCHAR*)pszJsonData);
	DLIB_Free(pszJsonData);

	HxLOG_Print("[%s;%d] src : (%d)[%s]\n", __FUNCTION__, __LINE__, size, src);

	dlib_udbmodule_save();
	*dst = DLIB_StrDup(HLIB_JSONWRITER_GetJSON(s_nDBEncodedStream));
	*dstSize = HLIB_JSONWRITER_GetLength(s_nDBEncodedStream);
	HxLOG_Print("[%s;%d] dst : (%d)[%s]\n", __FUNCTION__, __LINE__, *dstSize, *dst);
	return ERR_OK;
}

static HERROR		dlib_udbmodule_load(HUID uid, const void *src, HUINT32 size)
{
	HCHAR	*pszJsonData;

	static HBOOL loaded = FALSE;

	if(uid != 0)
		return ERR_FAIL;

	if(loaded != TRUE)
	{
		pszJsonData = DLIB_Calloc(size + 1);
		HxSTD_MemCopy(pszJsonData, src, size);

		dlib_udbmodule_loadHash((const HCHAR*)pszJsonData);
		DLIB_Free(pszJsonData);
		loaded = TRUE;
	}
	return ERR_OK;
}

static void		dlib_udbmodule_freeInfo(void *src)
{
	dlib_udbmodule_item_t	*pstInfo = (dlib_udbmodule_item_t*)src;

	if (pstInfo->pszKey)
		DLIB_Free(pstInfo->pszKey);
	if (pstInfo->eTag == HxOBJECT_TYPE_STR)
		DLIB_Free(pstInfo->u.pszString);
	if (pstInfo->pszProcName)
		DLIB_Free(pstInfo->pszProcName);
	DLIB_Free(pstInfo);
}


void			DLIB_UDBMODULE_Init(void)
{
	s_pstDBHashTable = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, NULL, dlib_udbmodule_freeInfo);
	s_nDBEncodedStream = HLIB_JSONWRITER_OpenEx(DLIB_Malloc_CB, DLIB_Free_CB);
}


HERROR			DLIB_UDBMODULE_Encode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HxLOG_Print("[%s;%d] dbDecoded : (%d)[%s]\n", __FUNCTION__, __LINE__, srcSize, src);

	return dlib_udbmodule_set(uid,src, srcSize, dst, dstSize);
}

HERROR			DLIB_UDBMODULE_Decode (HUID uid, const void *src, HUINT32 srcSize, void **dst, HUINT32 *dstSize)
{
	HxLOG_Print("[%s;%d] dbDecoded : (%d)[%s]\n", __FUNCTION__, __LINE__, srcSize, src);

	if (srcSize == 0 || src == NULL)
	{
		return ERR_FAIL;
	}
	dlib_udbmodule_load(uid,src, srcSize);
	return ERR_FAIL;
}


