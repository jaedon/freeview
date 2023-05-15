/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_db.c
	@brief

	Description:  									\n
	Module: APPKITv2			\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#include <dapi.h>
#include <apk.h>
#include "_apk_int.h"

#define	__DEFINITIONS____________
//	will be removed fields <-- for compatibility of previous version.

#define	ENTER_DB_CRITICAL		HxSEMT_Get(s_ulDBSemId)
#define	LEAVE_DB_CRITICAL		HxSEMT_Release(s_ulDBSemId)

#define	APPKIT_UDB_MODULE		"UDBMODULE"

#define	APKDB_NEW_IDENTIFIER	"udb_v"
#define	APKDB_VERSION			0x00010000

#define	APKDB_COMPATIBILITY_GROUP	8
#define ___INTERNAL_STRUCTURES___
typedef struct
{
	HCHAR			*pszProcName;
	HCHAR			*pszKey;
	HxObjectTag_e	eTag;
	union
	{
		HINT32	integer;
		HCHAR	*pszString;
	} u;
} apkDbData_t;

#define ___INTERNAL_VARIABLES___
STATIC HUINT32			s_ulDBSemId = 0;
STATIC HxHASH_t	*s_pstDBHashTable = NULL;
STATIC HCHAR			*s_pszCurProcName = NULL;
STATIC HINT32			s_nDBEncodedStream = 0;

#define ___PRIVATE_FUNCTION___
#define _____NOTIFIER_FUNCTION_____

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
static HBOOL	apk_db_updateItem(const HCHAR *pszProcName, const HCHAR *pszKey, HxObjectTag_e eTag, void *pvData)
{
	apkDbData_t		*object;

	if (eTag != HxOBJECT_TYPE_STR && eTag != HxOBJECT_TYPE_INT)
		return FALSE;

	HxLOG_Debug("update item [%s : %s : %d]\n", pszProcName, pszKey, eTag);

	object = (apkDbData_t*)HLIB_HASH_Lookup(s_pstDBHashTable, (void*)pszKey);
	if (object == NULL)
	{
		object = APK_Calloc(sizeof(apkDbData_t));
		object->pszProcName = APK_StrDup(pszProcName);
		object->pszKey      = APK_StrDup(pszKey);
		object->eTag        = eTag;
		if (object->eTag == HxOBJECT_TYPE_STR)
			object->u.pszString = APK_StrDup((HCHAR*)pvData);
		else
		if (object->eTag == HxOBJECT_TYPE_INT)
			object->u.integer = (HINT32)pvData;

		HLIB_HASH_Insert(s_pstDBHashTable, (void*)object->pszKey, (void*)object);

		return TRUE;
	} else
	{
		if (HxSTD_StrCmp(object->pszProcName, pszProcName))
		{
			HxLOG_Warning("owner will be changed [%s --> %s]\n", object->pszProcName, pszProcName);
			APK_Free(object->pszProcName);
			object->pszProcName = APK_StrDup(pszProcName);
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
			APK_Free(object->u.pszString);

		object->eTag = eTag;
		if (eTag == HxOBJECT_TYPE_STR)
			object->u.pszString = APK_StrDup((HCHAR*)pvData);
		else
		if (eTag == HxOBJECT_TYPE_INT)
			object->u.integer = (HINT32)pvData;

		return TRUE;
	}
}

static HINT32 snDeleteGroupId = 0;

static HBOOL	apk_db_updateOldUserDB(HINT32 nGroupId, HxJSON_t jsonRoot)
{
	const HCHAR	*szProcName[APKDB_COMPATIBILITY_GROUP] = {"", "corsair", "ipepg", "", "", "", "", ""};
	void	*iter;

	for ( iter = HLIB_JSON_Object_Iterator(jsonRoot)
		; iter
		; iter = HLIB_JSON_Object_NextIterator(jsonRoot, iter) )
	{
		const HCHAR		*key   = HLIB_JSON_Object_IteratorKey(iter);
		const HxJSON_t	jsValue  = HLIB_JSON_Object_IteratorValue(iter);

		if (HxJSON_IsInteger(jsValue))
			apk_db_updateItem(szProcName[nGroupId], key, HxOBJECT_TYPE_INT, (void*)HxJSON_Integer_Get(jsValue));
		else
		if (HxJSON_IsString(jsValue))
			apk_db_updateItem(szProcName[nGroupId], key, HxOBJECT_TYPE_STR, (void*)HxJSON_String_Get(jsValue));
	}

	if(HxSTD_StrCmp(szProcName[nGroupId], s_pszCurProcName) == 0)
	{
		snDeleteGroupId = nGroupId;
	}


	return TRUE;
}

static HBOOL	apk_db_updateHash(HINT32 nGroupId, const HCHAR *pszJsonText, HBOOL bMyUpdate)
{
	HxJSON_t	jsonRoot;
	HxJSON_t	jsonObject;
	HBOOL		hErr = FALSE;

	jsonRoot = HLIB_JSON_Decode(pszJsonText);
	if (jsonRoot == NULL)
	{
		HxLOG_Error("data is not valid [%s]\n", pszJsonText);

		return FALSE;
	}

	jsonObject = HxJSON_Object_Get(jsonRoot, APKDB_NEW_IDENTIFIER);
	if (jsonObject == NULL)
	{
		//	TODO: old udb data --> converting it....
		return apk_db_updateOldUserDB(nGroupId, jsonRoot);
	}

	if (HxJSON_Integer_Get(jsonObject) != APKDB_VERSION)
		return FALSE;

	//	loading data to hash.
	{
		void		*iter;

		for ( iter = HLIB_JSON_Object_Iterator(jsonRoot)
			; iter
			; iter = HLIB_JSON_Object_NextIterator(jsonRoot, iter) )
		{
			const HCHAR		*key   = HLIB_JSON_Object_IteratorKey(iter);
			const HxJSON_t	value  = HLIB_JSON_Object_IteratorValue(iter);
			const HxJSON_t	jsProc = HxJSON_Object_Get(value, "owner");
			const HxJSON_t	jsValue= HxJSON_Object_Get(value, "value");

			if (HxSTD_StrCmp(key, APKDB_NEW_IDENTIFIER) == 0)
				continue;

			//	ignore myupdate notifying.
			if(HxSTD_StrCmp(HxJSON_String_Get(jsProc), s_pszCurProcName) == 0)
			{
				hErr = TRUE;
			}

			if (bMyUpdate && HxSTD_StrCmp(HxJSON_String_Get(jsProc), s_pszCurProcName) == 0)
			{
				HxLOG_Debug("Notifying by me --> ignore data.(%s : %s)\n", HxJSON_String_Get(jsProc), s_pszCurProcName);
				continue;
			}

			if (HxJSON_IsInteger(jsValue))
				apk_db_updateItem(HxJSON_String_Get(jsProc), key, HxOBJECT_TYPE_INT, (void*)HxJSON_Integer_Get(jsValue));
			else
			if (HxJSON_IsString(jsValue))
				apk_db_updateItem(HxJSON_String_Get(jsProc), key, HxOBJECT_TYPE_STR, (void*)HxJSON_String_Get(jsValue));
		}
		HLIB_JSON_Delete(jsonRoot);
	}

	return hErr;
}

static HERROR	apk_db_getHash(const HCHAR *pszKey, HxObjectTag_e eType, void *pvData, HINT32 nMax)
{
	apkDbData_t	*object;

	HxLOG_Assert(pszKey && pvData);
	if (!(eType == HxOBJECT_TYPE_INT || eType == HxOBJECT_TYPE_STR))
	{
		HxLOG_Error("not supported data type! \n");

		return ERR_FAIL;
	}

	object = (apkDbData_t*)HLIB_HASH_Lookup(s_pstDBHashTable, (void*)pszKey);
	if (object == NULL)
	{
		HxLOG_Error("error : key is not valid! [%s]\n", pszKey);

		return ERR_FAIL;
	}

	if (object->eTag != eType)
	{
		HxLOG_Error("error : value type is not same! [%s] (%d)\n", pszKey, object->eTag);

		return ERR_FAIL;
	}

	if (eType == HxOBJECT_TYPE_STR)
	{
		HLIB_STD_StrNCpySafe((HCHAR*)pvData, object->u.pszString, nMax);
	} else
	if (eType == HxOBJECT_TYPE_INT)
	{
		*(HINT32*)pvData = object->u.integer;
	} else
	{
		HxLOG_Error("error : Unsupported Type [%s]\n", pszKey);

		return ERR_FAIL;
	}

	return ERR_OK;
}

static void		apk_db_updateData(const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *userdata, HBOOL bMyUpdate)
{
	HBOOL	bSucceed;
	HBOOL	*pbLoaded = (HBOOL*)userdata;
	DAxIterator_t	iter;

	ENTER_DB_CRITICAL;

	if (size == 0 || pvData == NULL)
	{
		HxLOG_Warning("Can't get data from DAMA\n");
		if (pbLoaded)
			*pbLoaded = FALSE;
		LEAVE_DB_CRITICAL;

		return;
	}

	if (pbLoaded)
		*pbLoaded = FALSE;

	DAPI_InitIterator(&iter, pvData, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		HCHAR	*pszJsonData;

		HxLOG_Assert(iter.uid < APKDB_COMPATIBILITY_GROUP);
		pszJsonData = APK_Calloc(iter.size + 1);
		HxSTD_MemCopy(pszJsonData, iter.data, iter.size);
		bSucceed = apk_db_updateHash(iter.uid, (const HCHAR*)pszJsonData, bMyUpdate);
		APK_Free(pszJsonData);
		if (bSucceed == TRUE)
		{
			if (pbLoaded)
				*pbLoaded = TRUE;
		}
	}

	LEAVE_DB_CRITICAL;
}

static void		apk_db_syncDB(HBOOL bLazy)
{
	apkDbData_t			*dbInfo = NULL;
	HxHASH_TableIter_t	iter;

	HLIB_JSONWRITER_Clear(s_nDBEncodedStream);
	HxJSONWRITER_ObjectBegin(s_nDBEncodedStream);
	HxJSONWRITER_String(s_nDBEncodedStream, "owner", s_pszCurProcName);
	HxJSONWRITER_Object(s_nDBEncodedStream, "data");

	HLIB_HASH_IterInit(&iter, s_pstDBHashTable);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&dbInfo))
	{
		if (HxSTD_StrCmp(dbInfo->pszProcName, s_pszCurProcName) == 0)
		{
			if (dbInfo->eTag == HxOBJECT_TYPE_INT)
				HxJSONWRITER_Integer(s_nDBEncodedStream, dbInfo->pszKey, dbInfo->u.integer);
			else if (dbInfo->eTag == HxOBJECT_TYPE_STR)
				HxJSONWRITER_String(s_nDBEncodedStream,  dbInfo->pszKey, dbInfo->u.pszString);
		}
	}
	HxJSONWRITER_ObjectEnd(s_nDBEncodedStream);

	HxJSONWRITER_Integer(s_nDBEncodedStream, APKDB_NEW_IDENTIFIER, APKDB_VERSION);
	HxJSONWRITER_ObjectEnd(s_nDBEncodedStream);

//	HxLOG_Warning("[%s;%d] dbEncoded : (%d)[%s]\n", __FUNCTION__, __LINE__, HLIB_JSONWRITER_GetLength(s_nDBEncodedStream) + 1, HLIB_JSONWRITER_GetJSON(s_nDBEncodedStream));
    if(bLazy == TRUE)
    {
	    DAPI_LazySet(APPKIT_UDB_MODULE, (HUID)0, (void*)HLIB_JSONWRITER_GetJSON(s_nDBEncodedStream), HLIB_JSONWRITER_GetLength(s_nDBEncodedStream) + 1, 1000);
    }
    else
    {
   	    DAPI_Set(APPKIT_UDB_MODULE, (HUID)0, (void*)HLIB_JSONWRITER_GetJSON(s_nDBEncodedStream), HLIB_JSONWRITER_GetLength(s_nDBEncodedStream) + 1);
        DAPI_Sync(APPKIT_UDB_MODULE);
    }
	if(snDeleteGroupId)
	{
		DAPI_Reset(APPKIT_UDB_MODULE, snDeleteGroupId);
		snDeleteGroupId	= 0;
	}
}

static void		apk_db_updateDataOnly(const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *userdata)
{
	apk_db_updateData(name, uid, pvData, size, userdata, TRUE);
}

static void		apk_db_updateDataFull(const HCHAR *name, HUID uid, const void *pvData, HUINT32 size, void *userdata)
{
	apk_db_updateData(name, uid, pvData, size, userdata, FALSE);
}

static void		apk_db_freeInfo(void *pvData)
{
	apkDbData_t	*pstInfo = (apkDbData_t*)pvData;

	if (pstInfo->pszKey)
		APK_Free(pstInfo->pszKey);
	if (pstInfo->eTag == HxOBJECT_TYPE_STR)
		APK_Free(pstInfo->u.pszString);

	APK_Free(pstInfo);
}

static void	apk_db_notifyDB(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DAPI_GetAll(APPKIT_UDB_MODULE, apk_db_updateDataOnly, NULL);
}

//	if return is ERR_FAIL, please update your own default setting data.
HERROR	APK_DB_Init(void)
{
	HUINT32	ulReqId;
	HBOOL	bLoaded = FALSE;

	HxSEMT_Create(&s_ulDBSemId, "apkdb", HxSEMT_FIFO);
	s_pstDBHashTable = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, NULL, apk_db_freeInfo);
	s_pszCurProcName = APK_StrDup(HLIB_STD_GetCurProcessName());
	s_nDBEncodedStream = HLIB_JSONWRITER_OpenEx(APK_Malloc_CB, APK_Free_CB);

	ulReqId = DAPI_GetAll(APPKIT_UDB_MODULE, apk_db_updateDataFull, (void*)&bLoaded);
	DAPI_Wait(60000, ulReqId);

	DAPI_AddTableNotifier(APPKIT_UDB_MODULE, apk_db_notifyDB, NULL);

	if (bLoaded == FALSE)
		return ERR_FAIL;

	return ERR_OK;
}

//	remove key with current process
HERROR	APK_DB_Remove(const HCHAR *pszKey)
{
	ENTER_DB_CRITICAL;

	if (pszKey == NULL)
	{
		//	remove all keys in current process db.
		HxHASH_TableIter_t	iter;
		void	*pszIterKey;
		apkDbData_t	*pstIterInfo;

		HLIB_HASH_IterInit(&iter, s_pstDBHashTable);
		while (HLIB_HASH_IterNext(&iter, (void **)&pszIterKey, (void **)&pstIterInfo))
		{
			if (HxSTD_StrCmp(pstIterInfo->pszProcName, s_pszCurProcName) == 0)
				HLIB_HASH_IterRemove(&iter);
		}
	}
	else
	{
		HLIB_HASH_Remove(s_pstDBHashTable, (void*)pszKey);
	}

	apk_db_syncDB(FALSE);

	LEAVE_DB_CRITICAL;

	return ERR_OK;
}

HERROR	APK_DB_SetInt(const HCHAR *pszKey, HINT32 nValue)
{
	ENTER_DB_CRITICAL;

	if (apk_db_updateItem(s_pszCurProcName, pszKey, HxOBJECT_TYPE_INT, (void*)nValue) == TRUE)
		apk_db_syncDB(FALSE);

	LEAVE_DB_CRITICAL;

	return ERR_OK;
}

HERROR	APK_DB_GetInt(const HCHAR *pszKey, HINT32 *pnValue)
{
	HERROR	err;

	ENTER_DB_CRITICAL;

	err = apk_db_getHash(pszKey, HxOBJECT_TYPE_INT, (void*)pnValue, 4);

	LEAVE_DB_CRITICAL;

	return err;
}

HERROR	APK_DB_SetStr(const HCHAR *pszKey, const HCHAR *pszString)
{
	ENTER_DB_CRITICAL;

	if (apk_db_updateItem(s_pszCurProcName, pszKey, HxOBJECT_TYPE_STR, (void*)pszString) == TRUE)
		apk_db_syncDB(FALSE);

	LEAVE_DB_CRITICAL;

	return ERR_OK;
}

HERROR	APK_DB_GetStr(const HCHAR *pszKey, HCHAR *pszString, HINT32 nMax)
{
	HERROR	err;

	ENTER_DB_CRITICAL;

	err = apk_db_getHash(pszKey, HxOBJECT_TYPE_STR, (void*)pszString, nMax);

	LEAVE_DB_CRITICAL;

	return err;
}
/* end of file */

