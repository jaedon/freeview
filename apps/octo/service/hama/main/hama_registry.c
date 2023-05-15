/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  hama_registry.c
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
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
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

#include	"hama_int.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define	HAMA_REGISTRY_DATA		CONFIG_PRODUCT_DATA_DIR"/registry.reg"
#define	HAMA_REGISTRY_BKDATA	CONFIG_PRODUCT_BACKUPDATA_DIR"/registry.reg"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HCHAR	*pszKey;
	HCHAR	*pszProcessName;

	HxObjectTag_e	eType;
	union
	{
		HINT32	integer;
		HCHAR	*string;
	} u;
} HAMA_REGINFO_t;
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
static HxHASH_t	*s_pstRegistry = NULL;
static HINT32			s_nEncodedStream = 0;
static HBOOL			s_bRegistryUpdated = FALSE;

/*******************************************************************/
/********************      Functions     ***************************/
/*******************************************************************/
static HAMA_REGINFO_t*	hama_registry_UpdateItem(HAMA_t *pstHama, const HCHAR *pszProcessName, const HCHAR *pszKey, HxObjectTag_e eType, void *pvData);

static void		hama_registry_MakeJSONItemProc(HINT32 nEncodedStream, HAMA_REGINFO_t *regInfo)
{
	HxJSONWRITER_Object(nEncodedStream, regInfo->pszKey);

	HxJSONWRITER_String(nEncodedStream, "owner", regInfo->pszProcessName);

	if (regInfo->eType == HxOBJECT_TYPE_INT)
		HxJSONWRITER_Integer(nEncodedStream, "value", regInfo->u.integer);
	else if (regInfo->eType == HxOBJECT_TYPE_STR)
		HxJSONWRITER_String(nEncodedStream,  "value", regInfo->u.string);

	HxJSONWRITER_ObjectEnd(nEncodedStream);
}


//static HAMA_REGINFO_t*	hama_registry_UpdateItem(HAMA_t *pstHama, const HCHAR *pszProcessName, const HCHAR *pszKey, HxObjectTag_e eType, void *pvData)
static HxJSON_t	hama_registry_LoadInfo(HAMA_t *pstHama, const HCHAR *pszPath)
{
	HCHAR		*pszData;
	HUINT32		ulSize;
	HxJSON_t	js;

	if ((HLIB_FILE_ReadFile(pszPath, (void**)&pszData, &ulSize) == ERR_OK) && (pszData))
	{
		js = HLIB_JSON_Decode(pszData);
		HLIB_STD_MemFree(pszData);

		return js;
	}

	return NULL;
}

static HERROR	hama_registry_Load(HAMA_t *pstHama)
{
	HxJSON_t	jsRoot;
	void		*iter;

	jsRoot    = hama_registry_LoadInfo(pstHama, HAMA_REGISTRY_DATA);
	if (jsRoot == NULL)
	{
		jsRoot= hama_registry_LoadInfo(pstHama, HAMA_REGISTRY_BKDATA);
		if (jsRoot == NULL)
		{
			HxLOG_Error("<----- fail to load registry file!\n");
			return ERR_FAIL;
		}

		HLIB_FILE_Copy(HAMA_REGISTRY_BKDATA, HAMA_REGISTRY_DATA);
	} else
	{
		HLIB_FILE_Copy(HAMA_REGISTRY_DATA, HAMA_REGISTRY_BKDATA);
	}

	for ( iter = HLIB_JSON_Object_Iterator(jsRoot)
		; iter
		; iter = HLIB_JSON_Object_NextIterator(jsRoot, iter) )
	{
		const HCHAR		*key   = HLIB_JSON_Object_IteratorKey(iter);
		const HxJSON_t	value  = HLIB_JSON_Object_IteratorValue(iter);
		const HxJSON_t	jsProc = HxJSON_Object_Get(value, "owner");
		const HxJSON_t	jsValue= HxJSON_Object_Get(value, "value");

		if (HxJSON_IsInteger(jsValue))
			hama_registry_UpdateItem(pstHama, HxJSON_String_Get(jsProc), key, HxOBJECT_TYPE_INT, (void*)HxJSON_Integer_Get(jsValue));
		else
		if (HxJSON_IsString(jsValue))
			hama_registry_UpdateItem(pstHama, HxJSON_String_Get(jsProc), key, HxOBJECT_TYPE_STR, (void*)HxJSON_String_Get(jsValue));
	}
	HLIB_JSON_Delete(jsRoot);

	return ERR_OK;
}

static HERROR	hama_registry_Save(HAMA_t *pstHama)
{
	HxLOG_Debug("bRegistryUpdated : [%d]\n", s_bRegistryUpdated);
	if (s_bRegistryUpdated)
	{
		HAMA_REGINFO_t		*regInfo;
		HxHASH_TableIter_t	iter;
		const HCHAR         *json;

		HLIB_JSONWRITER_Clear(s_nEncodedStream);
		HxJSONWRITER_ObjectBegin(s_nEncodedStream);
		HLIB_HASH_IterInit(&iter, s_pstRegistry);
		while (HLIB_HASH_IterNext(&iter, NULL, (void **)&regInfo))
		{
			hama_registry_MakeJSONItemProc(s_nEncodedStream, regInfo);
		}
		HxJSONWRITER_ObjectEnd(s_nEncodedStream);

		HLIB_FILE_Move(HAMA_REGISTRY_DATA, HAMA_REGISTRY_BKDATA);
		json = HLIB_JSONWRITER_GetJSON(s_nEncodedStream);

        /*
			Factory default 시도시 hama registry 를 저장 동작에서 이슈 발생
			간헐적으로 이미 삭제된 데이타를 접근 하려한다.
			결론적으로 file wirte 에서  data 를 0x0 으로 넣으려는는 동작이 발생해, crash 가 발생해 방어코드를 삽입
			http://svn.humaxdigital.com:3000/issues/104732
            */
        if(HxSTD_StrEmpty(json) == FALSE)
			HLIB_FILE_WriteFile(HAMA_REGISTRY_DATA, (void*)json, HLIB_JSONWRITER_GetLength(s_nEncodedStream) + 1);
	}

	s_bRegistryUpdated = FALSE;

	return ERR_OK;
}

static void		hama_registry_freeInfo(void *pvData)
{
	HAMA_REGINFO_t	*regInfo = (HAMA_REGINFO_t*)pvData;

	if (regInfo->pszProcessName)
		HAMA_Free(regInfo->pszProcessName);
	if (regInfo->pszKey)
		HAMA_Free(regInfo->pszKey);
	if (regInfo->eType == HxOBJECT_TYPE_STR && regInfo->u.string)
		HAMA_Free(regInfo->u.string);

	HAMA_Free(regInfo);
}

static HAMA_REGINFO_t*	hama_registry_UpdateItem(HAMA_t *pstHama, const HCHAR *pszProcessName, const HCHAR *pszKey, HxObjectTag_e eType, void *pvData)
{
	HAMA_REGINFO_t	*object;

	if (eType != HxOBJECT_TYPE_STR && eType != HxOBJECT_TYPE_INT)
		return NULL;

	object = (HAMA_REGINFO_t*)HLIB_HASH_Lookup(s_pstRegistry, (void*)pszKey);
	if (object == NULL)
	{
		object = (HAMA_REGINFO_t*)HAMA_Calloc(sizeof(HAMA_REGINFO_t));
		object->pszProcessName = HAMA_StrDup(pszProcessName);
		object->pszKey         = HAMA_StrDup(pszKey);
		object->eType          = eType;
		if (object->eType == HxOBJECT_TYPE_INT)
			object->u.integer = (HINT32)pvData;
		else
		if (object->eType == HxOBJECT_TYPE_STR)
			object->u.string  = HAMA_StrDup(pvData);

		HLIB_HASH_Insert(s_pstRegistry, (void*)object->pszKey, (void*)object);
	} else
	{
		if (HxSTD_StrCmp(object->pszProcessName, pszProcessName))
		{
			HxLOG_Error("Owner is not same ! key (%s) [%s : %s]\n", pszKey, object->pszProcessName, pszProcessName);

			return NULL;
		}

		if (object->eType != eType)
		{
			HxLOG_Error("data type is not matched!\n");

			return NULL;
		}
		if (eType == HxOBJECT_TYPE_INT)
		{
			if (object->u.integer == (HINT32)pvData)
			{
				HxLOG_Error("data is same!!\n");

				return NULL;
			}

			object->u.integer = (HINT32)pvData;
		} else
		if (eType == HxOBJECT_TYPE_STR)
		{
			if ((object->u.string) && (HxSTD_StrCmp(object->u.string, (HCHAR*)pvData) == 0))
			{
				HxLOG_Error("data is same!!\n");

				return NULL;
			}

			if (object->u.string)
				HAMA_Free(object->u.string);
			object->u.string = HAMA_StrDup((HCHAR*)pvData);
		}
	}

	return object;
}

static void		hama_registry_MakeJSONItem(HINT32 nEncodedStream, HAMA_REGINFO_t *object)
{
	if (object->eType == HxOBJECT_TYPE_INT)
		HxJSONWRITER_Integer(nEncodedStream, object->pszKey, object->u.integer);
	else if (object->eType == HxOBJECT_TYPE_STR)
		HxJSONWRITER_String(nEncodedStream, object->pszKey, object->u.string);
}

static HERROR	hama_registry_SetValue(HAMA_t *pstHama, HINT32 nClientHandle, HINT32 nProgId, const HCHAR *pszJsonString, HINT32 nRequestId)
{
	HCHAR		*pszProcessName;
	HxJSON_t	jsGRoot;
	void		*iter;
	HamaProcessInfo_t	*pstProcessInfo;

	HAMA_REGINFO_t	*object;

	pstProcessInfo = HAMA_TOOL_GetProcessInfoByClient(pstHama->pstAttachedProcessList, nClientHandle);
	if (pstProcessInfo == NULL)
	{
		HxLOG_Critical("can't find process info with client handle (%d) , pid (%d)\n", nClientHandle, nProgId);
		return ERR_FAIL;
	}
	pszProcessName = pstProcessInfo->szProcessName;
//	pszProcessName = HLIB_STD_GetProcessName(nProgId);
	if (HxSTD_StrEmpty(pszProcessName))
	{
		HxLOG_Error("Invalid program id [%d] - [%s]\n", nProgId, pszJsonString);

		return ERR_FAIL;
	}

	jsGRoot = HLIB_JSON_Decode(pszJsonString);
	if (jsGRoot == NULL)
	{
		HxLOG_Error("Parse error [%s]\n", pszJsonString);
		return ERR_FAIL;
	}

	for ( iter = HLIB_JSON_Object_Iterator(jsGRoot)
		; iter
		; iter = HLIB_JSON_Object_NextIterator(jsGRoot, iter) )
	{
		const HCHAR		*key = HLIB_JSON_Object_IteratorKey(iter);
		const HxJSON_t	value = HLIB_JSON_Object_IteratorValue(iter);

		if (HxJSON_IsInteger(value))
			object = hama_registry_UpdateItem(pstHama, pszProcessName, key, HxOBJECT_TYPE_INT, (void*)HxJSON_Integer_Get(value));
		else
		if (HxJSON_IsString(value))
			object = hama_registry_UpdateItem(pstHama, pszProcessName, key, HxOBJECT_TYPE_STR, (void*)HxJSON_String_Get(value));
		else
		{
			HxLOG_Error("Invalid data type\n");
			object = NULL;
		}

		if (object)
		{
			s_bRegistryUpdated = TRUE;
			HLIB_JSONWRITER_Clear(s_nEncodedStream);
			HxJSONWRITER_ObjectBegin(s_nEncodedStream);
			hama_registry_MakeJSONItem(s_nEncodedStream, object);
			HxJSONWRITER_ObjectEnd(s_nEncodedStream);
			HLIB_RPC_Notify(pstHama->nRPCHandle, "hama:onRegistryChanged", "iis", nRequestId, eHSIGPRAM_REG_NOTI_CHANGED, HLIB_JSONWRITER_GetJSON(s_nEncodedStream));

			HAMA_TOOL_SendWakeup(pstHama);
		}
	}
	HLIB_JSON_Delete(jsGRoot);

	return ERR_OK;
}

static HERROR	hama_registry_NotifyRegistry(HAMA_t *pstHama, HINT32 nClientHandle, HUINT32 ulNotifyType)
{
	HAMA_REGINFO_t		*regInfo;
	HxHASH_TableIter_t	iter;

	HxLOG_Debug("Notifying --- nClientHandle [%d]\n", nClientHandle);

	HLIB_JSONWRITER_Clear(s_nEncodedStream);
	HxJSONWRITER_ObjectBegin(s_nEncodedStream);
	HLIB_HASH_IterInit(&iter, s_pstRegistry);
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&regInfo))
	{
		hama_registry_MakeJSONItem(s_nEncodedStream, regInfo);
	}
	HxJSONWRITER_ObjectEnd(s_nEncodedStream);

	HxLOG_Debug("Notifying all nClient[%d] Data [%s]\n", nClientHandle, HLIB_JSONWRITER_GetJSON(s_nEncodedStream));
	HLIB_RPC_CallbackSpecific(pstHama->nRPCHandle, nClientHandle, "hama:onRegistryChanged", "iis", 0, ulNotifyType, HLIB_JSONWRITER_GetJSON(s_nEncodedStream));

	return ERR_OK;
}

//	apArgv[0] = HINT32 nProgId;
//	apArgv[1] = HCHAR *jsonString;
static HERROR	hama_rpc_SetRegistry(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;
	HINT32	nProgId;
	HCHAR	*jsonString;

	HERROR	ret;

	HAMA_ASSERT(nArgc == 2);

	pstHama = HAMA_GetInstance();

	nProgId = HxOBJECT_INT(apArgv[0]);
	jsonString = HxOBJECT_STR(apArgv[1]);

	ret = hama_registry_SetValue(pstHama, nClientHandle, nProgId, jsonString, nRequestId);

	HAMA_ReleaseInstance(pstHama);

	return ret;
}

static HERROR	hama_rpc_RemoveRegistry(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HAMA_t	*pstHama;
	HCHAR	*pszProcName;
	HamaProcessInfo_t	*pstProcessInfo;

	HAMA_ASSERT(nArgc == 1);

	pstHama = HAMA_GetInstance();

	pstProcessInfo = HAMA_TOOL_GetProcessInfoByClient(pstHama->pstAttachedProcessList, nClientHandle);
	if (pstProcessInfo == NULL)
	{
		HxLOG_Critical("can't find process info with client handle (%d), pid (%d)\n", nClientHandle, HxOBJECT_INT(apArgv[0]));
		HAMA_ReleaseInstance(pstHama);
		return ERR_FAIL;
	}
//	pszProcName = HLIB_STD_GetProcessName(HxOBJECT_INT(apArgv[0]));
	pszProcName = pstProcessInfo->szProcessName;
	if (pszProcName == NULL)
	{
		HxLOG_Error("registry : procid is not valid!! [%d]\n", HxOBJECT_INT(apArgv[0]));
		HAMA_ReleaseInstance(pstHama);
		return ERR_FAIL;
	}

	{
		HxHASH_TableIter_t	iter;
		void	*pszKey;
		HAMA_REGINFO_t	*pstRegInfo;

		HLIB_HASH_IterInit(&iter, s_pstRegistry);
		while (HLIB_HASH_IterNext(&iter, (void **)&pszKey, (void **)&pstRegInfo))
		{
			if (HxSTD_StrCmp(pstRegInfo->pszProcessName, pszProcName) == 0)
				HLIB_HASH_IterRemove(&iter);
		}
	}

	hama_registry_NotifyRegistry(pstHama, nClientHandle, eHSIGPRAM_REG_NOTI_REMOVED);
	s_bRegistryUpdated = TRUE;

	HAMA_ReleaseInstance(pstHama);

	return hama_registry_Save(pstHama);
}

HERROR	HAMA_REGISTRY_NotifyRegistry(HAMA_t *pstHama, HINT32 nClientHandle)
{
	return hama_registry_NotifyRegistry(pstHama, nClientHandle, eHSIGPRAM_REG_NOTI_CHANGED);
}

HERROR	HAMA_REGISTRY_SaveData(HAMA_t *pstHama)
{
	return hama_registry_Save(pstHama);
}

HERROR	HAMA_REGISTRY_Init(HAMA_t *pstHama)
{
	s_nEncodedStream = HLIB_JSONWRITER_OpenEx(HAMA_Malloc_CB, HAMA_Free_CB);
	s_pstRegistry = HLIB_HASH_NewFull(HLIB_HASH_GetStringKey, HLIB_HASH_IsStringEqual, NULL, hama_registry_freeInfo);

	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.registry.set",    "is", hama_rpc_SetRegistry, NULL, "HaMa Registry Set (HINT32 progId, HCHAR *jsonString)");
	HLIB_RPC_RegisterCall(pstHama->nRPCHandle, "hama.registry.remove", "i",  hama_rpc_RemoveRegistry, NULL, "remove all data in registry with process id");

	HLIB_RPC_CreateNotification(pstHama->nRPCHandle, "hama:onRegistryChanged", "iis", "notify and send registry info");

	hama_registry_Load(pstHama);

	return ERR_OK;
}

void HAMA_REGISTRY_GetAutomaticPowerdown(HUINT32* pTimeout)
{
	HAMA_REGINFO_t *object;
#define	OAPI_SCENARIO_AUTOMATIC_POWERDOWN			"automaticpowerdown" //oapi 를 include 하기가..

	object = (HAMA_REGINFO_t*)HLIB_HASH_Lookup(s_pstRegistry, (void*)OAPI_SCENARIO_AUTOMATIC_POWERDOWN);
	if(object == NULL)
	{
		HxLOG_Debug("Can't find registry-[%s]!! \n",OAPI_SCENARIO_AUTOMATIC_POWERDOWN);
		return;
	}

	*pTimeout = object->u.integer;
	HxLOG_Info("pTimeout:%d\n",*pTimeout);
}

#ifdef	CONFIG_DEBUG
HERROR	HAMA_REGISTRY_PrintCMD(HAMA_t *pstHama)
{
	HAMA_REGINFO_t		*regInfo;
	HxHASH_TableIter_t	iter;

	HLIB_HASH_IterInit(&iter, s_pstRegistry);
	HLIB_CMD_Printf("=== HAMA REGISTRY ===\n");
	while (HLIB_HASH_IterNext(&iter, NULL, (void **)&regInfo))
	{
		if (regInfo->eType == HxOBJECT_TYPE_INT)
			HLIB_CMD_Printf("[%16s] [%20s] (%d)\n", regInfo->pszProcessName, regInfo->pszKey, regInfo->u.integer);
		if (regInfo->eType == HxOBJECT_TYPE_STR)
			HLIB_CMD_Printf("[%16s] [%20s] (%s)\n", regInfo->pszProcessName, regInfo->pszKey, regInfo->u.string);
	}

	return ERR_OK;
}
#endif

