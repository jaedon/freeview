/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hapi_registry.c
	@brief

	Description:  									\n
	Module: HaMa Event Handler						\n

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

/*
	//	File Detail.

	--- OWNER1 --- item1 --- key
	                     --- value (string)
	               item2 --- key
	                     --- value (string)
    --- OWNER2 --- item1 --- ...
*/
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include	<hlib.h>

#include	"hapi_int.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static void	hxapi_freeRegistry(void *pvData)
{
	HREGINFO_t	*object = (HREGINFO_t*)pvData;

	if (object->pszKey)
		HAMA_Free(object->pszKey);

	if (object->eType == HxOBJECT_TYPE_STR)
		HAMA_Free(object->u.pszString);

	HAMA_Free(object);
}

static HREGINFO_t*	hapi_registry_MakeObject(const HCHAR *pszKey, HxObjectTag_e eTag, const void *pvData)
{
	HREGINFO_t	*object;

	object= (HREGINFO_t*)HAMA_Calloc(sizeof(HREGINFO_t));
	object->pszKey = HAMA_StrDup(pszKey);
	object->eType  = eTag;
	if (eTag == HxOBJECT_TYPE_INT)
		object->u.integer = (HINT32)pvData;
	else if (eTag == HxOBJECT_TYPE_STR)
		object->u.pszString = HAMA_StrDup(pvData);

	return object;
}

static HREGINFO_t*	hapi_registry_AddRegistry(HAPI_t *hapi, const HCHAR *pszKey, HxObjectTag_e eTag, const void *pvData)
{
	HREGINFO_t	*object;

	if (eTag == HxOBJECT_TYPE_STR && pvData == NULL)
	{
		HxLOG_Error("Invalid Data \n");

		return NULL;
	}

	if (!(eTag == HxOBJECT_TYPE_INT || eTag == HxOBJECT_TYPE_STR))
	{
		HxLOG_Error("not supported data type! \n");

		return NULL;
	}

	object = HLIB_HASH_Lookup(hapi->pstRegistry, (void*)pszKey);
	if (object == NULL)
	{
		object = hapi_registry_MakeObject(pszKey, eTag, pvData);
		HLIB_HASH_Insert(hapi->pstRegistry, (void*)object->pszKey, (void*)object);
	} else
	{
		if (object->eType != eTag)
		{
			HxLOG_Error("error : data type is not matched! [%s]\n", pszKey);

			return NULL;
		}

		if (eTag == HxOBJECT_TYPE_STR)
		{
			if (HxSTD_StrCmp(object->u.pszString, (HCHAR*)pvData) == 0)
			{
				HxLOG_Debug("same data [%s : %s]\n", pszKey, pvData);
				return NULL;
			}

			HAMA_Free(object->u.pszString);
			object->u.pszString = HAMA_StrDup(pvData);
		} else
		if (eTag == HxOBJECT_TYPE_INT)
		{
			if (object->u.integer == (HINT32)pvData)
			{
				HxLOG_Debug("same data [%d : %d]\n", pszKey, pvData);
				return NULL;
			}

			object->u.integer = (HINT32)pvData;
		}
	}

	return object;
}

//	apArgv[0] : nSession or nRequestId
//	apArgv[1] : JSON String
static HERROR	hapi_onRegistryChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAPI_t		*hapi;
	HxJSON_t	jsGKey;
	HBOOL		bSessionRemoved;
	HCHAR		*pszData = NULL;

	HxLOG_Assert(nArgc == 3);

	HxLOG_Debug("HxOBJECT_INT(apArgv[1]) [%d] \n", HxOBJECT_INT(apArgv[1]));
	bSessionRemoved = (HxOBJECT_INT(apArgv[1]) == 1 /*eHSIGPRAM_REG_NOTI_REMOVED*/) ? TRUE : FALSE;

//	HxLOG_Debug("registry updated [%s]\n", HxOBJECT_STR(apArgv[2]));
	if ((bSessionRemoved == FALSE) && (HxRPC_GetProgramIdByRequestId(HxOBJECT_INT(apArgv[0])) == HLIB_STD_GetPID()))
	{
		HxLOG_Debug("Ignore hama:onRegistryChanged event : from my changes\n");
		return ERR_OK;
	}

	pszData = HxOBJECT_STR(apArgv[2]);
	if (NULL == pszData)
	{
		HxLOG_Critical("NULL == pszData >>>\n");
		return ERR_OK;
	}

	hapi = hapi_getInstance();

	jsGKey = HLIB_JSON_Decode(pszData);
	if (jsGKey)
	{
		void		*iter;

		for ( iter = HLIB_JSON_Object_Iterator(jsGKey)
			; iter
			; iter = HLIB_JSON_Object_NextIterator(jsGKey, iter) )
		{
			const HCHAR		*key = HLIB_JSON_Object_IteratorKey(iter);
			const HxJSON_t	value = HLIB_JSON_Object_IteratorValue(iter);

			if (HxJSON_IsInteger(value))
				hapi_registry_AddRegistry(hapi, key, HxOBJECT_TYPE_INT, (void*)HxJSON_Integer_Get(value));
			else
			if (HxJSON_IsString(value))
				hapi_registry_AddRegistry(hapi, key, HxOBJECT_TYPE_STR, (void*)HxJSON_String_Get(value));
			else
			{
				HxLOG_Error("Invalid data type [%s]\n", key);
			}
		}
		HLIB_JSON_Delete(jsGKey);
	}

	{
		HxList_t	*list;
		HREGHandler_t	*pstHandler;

		list = hapi->pstRegHandlerList;
		while (list)
		{
			pstHandler = (HREGHandler_t*)HLIB_LIST_Data(list);
//			HxLOG_Debug("pstHandler [%p] >>>\n", pstHandler );
			if (pstHandler && pstHandler->pfnHandler)
			{
				pstHandler->pfnHandler(pstHandler->pvUser, bSessionRemoved);
			}
			list = list->next;
		}
	}

	if (hapi->bRegistryLoaded == FALSE)
		hapi->bRegistryLoaded = TRUE;

	hapi_releaseInstance(hapi);

	return ERR_OK;
}

static const HCHAR*		hapi_registry_MakeJsonItem(HAPI_t *hapi, HREGINFO_t *pstObject)
{
	HLIB_JSONWRITER_Clear(hapi->nEncodeStream);
	HxJSONWRITER_ObjectBegin(hapi->nEncodeStream);

	if (pstObject->eType == HxOBJECT_TYPE_INT)
		HxJSONWRITER_Integer(hapi->nEncodeStream, pstObject->pszKey, pstObject->u.integer);
	else if (pstObject->eType == HxOBJECT_TYPE_STR)
		HxJSONWRITER_String(hapi->nEncodeStream, pstObject->pszKey, pstObject->u.pszString);

	HxJSONWRITER_ObjectEnd(hapi->nEncodeStream);

	return HLIB_JSONWRITER_GetJSON(hapi->nEncodeStream);
}

HERROR	HxAPI_RegistryInit(HAPI_t *hapi)
{
	hapi->pstRegistry = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, NULL, hxapi_freeRegistry);
	hapi->nEncodeStream = HLIB_JSONWRITER_OpenEx(HAMA_Malloc_CB, HAMA_Free_CB);

	HLIB_RPC_RegisterNotifyCallback(hapi->nRPCHandle, "hama:onRegistryChanged", "iis", (void*)hapi, hapi_onRegistryChanged);
	HLIB_RPC_Call(hapi->nRPCHandle, "hama.signal.buffered", "s", "hama:onRegistryChanged");

	return ERR_OK;
}

HERROR	HxAPI_SetRegistryListener(HAPI_t *hapi, void *pvUser, CBHAPIRegHandler pfnRegHandler)
{
	HREGHandler_t	*pstHandler;

	pstHandler = (HREGHandler_t*)HAMA_Calloc(sizeof(HREGHandler_t));
	pstHandler->pvUser = pvUser;
	pstHandler->pfnHandler = pfnRegHandler;

	hapi->pstRegHandlerList = HLIB_LIST_Append(hapi->pstRegHandlerList, (void*)pstHandler);

	if (hapi->bRegistryLoaded)
	{
		pfnRegHandler(pvUser, FALSE);
	}

	return ERR_OK;
}

HERROR	HxAPI_SetRegistryEx(HAPI_t *hapi, const HCHAR *pszKey, HxObjectTag_e eTag, const void *pvData)
{
	const HCHAR	*pszEncoded;
	HREGINFO_t	*object;

	object = hapi_registry_AddRegistry(hapi, pszKey, eTag, pvData);
	if (object)
	{
		pszEncoded = hapi_registry_MakeJsonItem(hapi, object);
		HLIB_RPC_Call(hapi->nRPCHandle, "hama.registry.set", "is", HLIB_STD_GetPID(), pszEncoded);
	}

	return ERR_OK;
}

HERROR	HxAPI_GetRegistryEx(HAPI_t *hapi, const HCHAR *pszKey, HxObjectTag_e eType, void *pvData, HINT32 nMax)
{
	HREGINFO_t	*object;

	HxLOG_Assert(pszKey && pvData);
	if (!(eType == HxOBJECT_TYPE_INT || eType == HxOBJECT_TYPE_STR))
	{
		HxLOG_Error("not supported data type! \n");

		return ERR_FAIL;
	}

	object = (HREGINFO_t*)HLIB_HASH_Lookup(hapi->pstRegistry, (void*)pszKey);
	if (object == NULL)
	{
		HxLOG_Error("error : key is not valid! [%s]\n", pszKey);

		return ERR_FAIL;
	}

	if (object->eType != eType)
	{
		HxLOG_Error("error : value type is not same! [%s] (%d)\n", pszKey, object->eType);

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
