/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_datatype.c
	@brief

	Description:  									\n
	Module: node/binding/datatype		\n

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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <apk.h>
#include <rest_datatype.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/
#define MAX_TEMP_STRING_NUM			5

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct
{
	HxHASH_t		*pstEnum2Str;
	HxHASH_t		*pstStr2Enum;
} restDataConvMgr_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC restDataConvMgr_t		*s_pstRestDataType_ObjConvMgr = NULL;
STATIC HxCBUF_t					 s_pstRestDataType_CBuffer = NULL;
STATIC HxList_t					*s_pstRestDataType_TempStrList = NULL;

#define ___LOCAL_FUNCTIONS___
STATIC HERROR rest_datatype_InitManager (void)
{
	restDataConvMgr_t		*pstMgr = NULL;

	pstMgr = (restDataConvMgr_t *)HLIB_MEM_Calloc(sizeof(restDataConvMgr_t));

	if (NULL != pstMgr)
	{
		pstMgr->pstEnum2Str = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, (_HxDestroyNotify)HLIB_MEM_Free_CB, NULL);
		pstMgr->pstStr2Enum = HLIB_HASH_NewFull(HLIB_HASH_StrHash, HLIB_HASH_StrEqual, (_HxDestroyNotify)HLIB_MEM_Free_CB, NULL);

		if ((NULL == pstMgr->pstEnum2Str) || (NULL == pstMgr->pstStr2Enum))
		{
			goto ERROR;
		}

		s_pstRestDataType_ObjConvMgr = pstMgr;
	}

	return ERR_OK;

ERROR:
	if (NULL != pstMgr)
	{
		if (NULL != pstMgr->pstEnum2Str)		{ HLIB_HASH_Destroy(pstMgr->pstEnum2Str); }
		if (NULL != pstMgr->pstStr2Enum)		{ HLIB_HASH_Destroy(pstMgr->pstStr2Enum); }
		HLIB_MEM_Free(pstMgr);
	}

	return ERR_FAIL;
}

STATIC restDataConvMgr_t *rest_datatype_GetManager (void)
{
	if (NULL == s_pstRestDataType_ObjConvMgr)
	{
		(void)rest_datatype_InitManager();
	}

	return s_pstRestDataType_ObjConvMgr;
}

#define ___MEMBER_FUNCTIONS___
HERROR rest_datatype_Init (void)
{
	if (NULL == s_pstRestDataType_ObjConvMgr)
	{
		(void)rest_datatype_InitManager();
	}

	s_pstRestDataType_CBuffer = HLIB_CBUF_New(MAX_CBUFFER_SIZE);
	if (NULL == s_pstRestDataType_CBuffer)
	{
		HxLOG_Critical("rest_task_Init failed:\n");
		return ERR_FAIL;
	}

	(void)rest_datatype_InitChannel();
	(void)rest_datatype_InitPlayer();
	(void)rest_datatype_InitEpg();
	(void)rest_datatype_InitSchedule();
	(void)rest_datatype_InitScan();
	(void)rest_datatype_InitRecord();
	(void)rest_datatype_InitSettings();
	(void)rest_datatype_InitTuner();
	(void)rest_datatype_InitRecorder();

	return ERR_OK;
}

HERROR rest_datatype_RegisterEnumTable (RestEnumStringTable_t *pstTable)
{
	HCHAR				 szNameValue[128];
	HCHAR				 szKeyValue [128];
	RestEnumItem_t		*pstEnumItem;
	restDataConvMgr_t		*pstMgr = rest_datatype_GetManager();

	if (NULL == pstTable)
	{
		HxLOG_Error(">> pstTable NULL\n");
		return ERR_FAIL;
	}

	if (NULL == pstMgr)
	{
		HxLOG_Error(">> rest_datatype_GetManager err!!!\n");
		return ERR_FAIL;
	}

	pstEnumItem = (RestEnumItem_t *)pstTable->astEnumItems;
	while (pstEnumItem->szEnumStr)
	{
		HxSTD_PrintToStrN(szKeyValue , 127, "%s.%d", pstTable->szEnumType, pstEnumItem->nEnumValue);
		HxSTD_PrintToStrN(szNameValue, 127, "%s.%s", pstTable->szEnumType, pstEnumItem->szEnumStr);

		HLIB_HASH_Insert(pstMgr->pstEnum2Str, (void*)HLIB_MEM_StrDup(szKeyValue) , (void*)pstEnumItem->szEnumStr);
		HLIB_HASH_Insert(pstMgr->pstStr2Enum, (void*)HLIB_MEM_StrDup(szNameValue), (void*)pstEnumItem->nEnumValue);

		pstEnumItem++;
	}

	return ERR_OK;
}

HCHAR *rest_datatype_ConvertEnum2Str (HCHAR *szEnumType, HINT32 nEnumValue)
{
	HCHAR				 szKeyValue [128];
	HCHAR				*szEnumName;
	restDataConvMgr_t		*pstMgr = rest_datatype_GetManager();

	if (NULL == pstMgr)
	{
		HxLOG_Error(">> rest_datatype_GetManager err!!!\n");
		return "error";
	}

	HxSTD_PrintToStrN(szKeyValue , 127, "%s.%d", szEnumType, nEnumValue);

	szEnumName = (HCHAR*)HLIB_HASH_Lookup(pstMgr->pstEnum2Str, (void*)szKeyValue);
	if (NULL == szEnumName)
	{
		HxLOG_Error("Can't find enumValue [%s](%d)\n", szEnumType, nEnumValue);
		return "error";
	}

	return szEnumName;
}

HERROR rest_datatype_ConvertStr2Enum (HCHAR *szEnumType, HCHAR *szEnumName, HINT32 *pnEnumValue)
{
	HINT32					 nEnumValue = 0;
	HCHAR					 szNameValue[128];
	restDataConvMgr_t		*pstMgr = rest_datatype_GetManager();

	if ((NULL == pstMgr) || (NULL == pnEnumValue))
	{
		HxLOG_Error(">> rest_datatype_GetManager err!!!\n");
		return ERR_FAIL;
	}

	HxSTD_PrintToStrN(szNameValue, 127, "%s.%s", szEnumType, szEnumName);

	if (HLIB_HASH_LookupExtended(pstMgr->pstStr2Enum, (void*)szNameValue, NULL, (void **)&nEnumValue) == TRUE)
	{
		*pnEnumValue = nEnumValue;
		return ERR_OK;
	}

	HxLOG_Error(">> not found value: EnumType(%s), EnumName(%s)\n", szEnumType, szEnumName);
	return ERR_FAIL;
}

INLINE HBOOL rest_datatype_CheckWriteSelected (HCHAR *szKey, HCHAR *szSelect)
{
#if 0
	HCHAR		*szNotSelect = (HCHAR *)rest_datatype_GetTempBuffer(256);

	if (szNotSelect)
	{
		if ((NULL != szKey) && ('\0' != *szKey))
		{
			szNotSelect[0] = '~';
			HxSTD_StrNCpy(szNotSelect + 1, szKey, 255);
			szNotSelect[255] = '\0';
		}
		else
		{
			*szNotSelect = '\0';
		}
	}
#endif
	STATIC HCHAR	 *s_szLastSelect = NULL, *s_szLastTokenBuf = NULL;
	STATIC HUINT32	  s_ulLastTokenNum = 0;
	STATIC HCHAR	**s_pszLastTokens = NULL;
	HBOOL			  bUpdateToken = FALSE;
	HUINT32			  ulCnt, bAll = TRUE;
	HUINT32			  ulSelectLen, ulTokenNum;

	if ((NULL == szSelect) || ('\0' == *szSelect))
	{
		return TRUE;
	}

	if (NULL != s_szLastSelect)
	{
		if (HxSTD_StrCmp(s_szLastSelect, szSelect) != 0)
		{
			bUpdateToken = TRUE;
		}
	}
	else
	{
		bUpdateToken = TRUE;
	}

	if (TRUE == bUpdateToken)
	{
		HCHAR			*szToken, *szSave;

		ulSelectLen = HxSTD_StrLen(szSelect);

		if (NULL != s_szLastSelect)
		{
			HLIB_STD_MemFree(s_szLastSelect);
			s_szLastSelect = NULL;
		}

		if (NULL != s_szLastTokenBuf)
		{
			HLIB_STD_MemFree(s_szLastTokenBuf);
			s_szLastTokenBuf = NULL;
		}

		if (NULL != s_pszLastTokens)
		{
			HLIB_STD_MemFree(s_pszLastTokens);
			s_pszLastTokens = NULL;
		}

		s_szLastSelect = HLIB_STD_MemCalloc(ulSelectLen + 1);
		s_szLastTokenBuf = HLIB_STD_MemCalloc(ulSelectLen + 1);
		if ((NULL == s_szLastSelect) || (NULL == s_szLastTokenBuf))
		{
			HxLOG_Error("memory allocation failed:\n");
			return TRUE;
		}

		ulTokenNum = 1;
		for (ulCnt = 0; ulCnt < ulSelectLen; ulCnt++)
		{
			if ((',' == szSelect[ulCnt]) || (';' == szSelect[ulCnt]))
			{
				ulTokenNum++;
			}
		}

		s_pszLastTokens = (HCHAR **)HLIB_STD_MemCalloc(sizeof(HCHAR *) * ulTokenNum);
		if (NULL == s_pszLastTokens)
		{
			HxLOG_Error("memory allocation failed:\n");
			return TRUE;
		}

		HxSTD_StrNCpy(s_szLastSelect, szSelect, ulSelectLen);
		HxSTD_StrNCpy(s_szLastTokenBuf, szSelect, ulSelectLen);
		s_ulLastTokenNum = ulTokenNum;

		ulCnt = 0;
		szToken = HxSTD_StrTok(s_szLastTokenBuf, ",;", &szSave);
		while ((szToken) && (ulCnt < ulTokenNum))
		{
			s_pszLastTokens[ulCnt] = szToken;
			ulCnt ++;
			szToken = HxSTD_StrTok(NULL, ",;", &szSave);
		}
	}

	for (ulCnt = 0; ulCnt < s_ulLastTokenNum; ulCnt++)
	{
		if ('~' == s_pszLastTokens[ulCnt][0])
		{
			if (HxSTD_StrCmp(s_pszLastTokens[ulCnt] + 1, szKey) == 0)
			{
				return FALSE;
			}
		}
		else
		{
			bAll = FALSE;
			if (HxSTD_StrCmp(s_pszLastTokens[ulCnt], szKey) == 0)
			{
				return TRUE;
			}
		}
	}

	return bAll;
}

HERROR rest_datatype_GetJsonInteger (HxJSON_t jsData, HCHAR *szKey, HINT32 *pnValue)
{
	HxJSON_t		 jsItem;

	if ((NULL == jsData) || (NULL == szKey) || (NULL == pnValue))
	{
		HxLOG_Error(">>> Argument NULL: pstJson(0x%08x), szKey(0x%08x), pnValue(0x%08x)\n", (HUINT32)jsData, (HUINT32)szKey, (HUINT32)pnValue);
		return ERR_FAIL;
	}

	if ('\0' == *szKey)
	{
		return ERR_FAIL;
	}

	jsItem = HxJSON_Object_Get(jsData, szKey);
	if (NULL == jsItem)
	{
		HxLOG_Error(">>> No key (%s) in pstJson\n", szKey);
		return ERR_FAIL;
	}

	if (HxJSON_IsInteger(jsItem))
	{
		*pnValue = HxJSON_Integer_Get(jsItem);
		return ERR_OK;
	}
	else if (HxJSON_IsString(jsItem))
	{
		const HCHAR		*szString = HxJSON_String_Get(jsItem);

		if (HLIB_STD_IsInteger(szString))
		{
			*pnValue = HLIB_STD_StrToINT32(szString);
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR rest_datatype_SetJsonInteger (HINT32 nWriter, HCHAR *szKey, HINT32 nValue, HCHAR *szSelect)
{
	HERROR			 hErr = ERR_OK;

	if ((0 == nWriter) || (NULL == szKey))
	{
		HxLOG_Error(">>> Argument NULL: nWriter(0x%08x), szKey(0x%08x)\n", nWriter, (HUINT32)szKey);
		return ERR_FAIL;
	}

	if (rest_datatype_CheckWriteSelected(szKey, szSelect))
	{
		hErr = HxJSONWRITER_Integer(nWriter, szKey, nValue);
	}

	return hErr;
}

HERROR rest_datatype_GetJsonUnsigned (HxJSON_t jsData, HCHAR *szKey, HUINT32 *pulValue)
{
	HxJSON_t		 jsItem;

	if ((NULL == jsData) || (NULL == szKey) || (NULL == pulValue))
	{
		HxLOG_Error(">>> Argument NULL: jsData(0x%08x), szKey(0x%08x), pulValue(0x%08x)\n", (HUINT32)jsData, (HUINT32)szKey, (HUINT32)pulValue);
		return ERR_FAIL;
	}

	if ('\0' == *szKey)
	{
		return ERR_FAIL;
	}

	jsItem = HxJSON_Object_Get(jsData, szKey);
	if (NULL == jsItem)
	{
		HxLOG_Error(">>> No key (%s) in pstJson\n", szKey);
		return ERR_FAIL;
	}

	if (HxJSON_IsInteger(jsItem))
	{
		*pulValue = (HUINT32)HxJSON_Integer_Get(jsItem);
		return ERR_OK;
	}
	else if (HxJSON_IsString(jsItem))
	{
		const HCHAR		*szString = HxJSON_String_Get(jsItem);

		if (HLIB_STD_IsInteger(szString))
		{
			*pulValue = HLIB_STD_StrToUINT32(szString);
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR rest_datatype_SetJsonUnsigned (HINT32 nWriter, HCHAR *szKey, HUINT32 ulValue, HCHAR *szSelect)
{
	HERROR			 hErr = ERR_OK;

	if ((0 == nWriter) || (NULL == szKey))
	{
		HxLOG_Error(">>> Argument NULL: nWriter(0x%08x), szKey(0x%08x)\n", nWriter, (HUINT32)szKey);
		return ERR_FAIL;
	}

	if (rest_datatype_CheckWriteSelected(szKey, szSelect))
	{
		hErr = HxJSONWRITER_Unsigned(nWriter, szKey, ulValue);
	}

	return hErr;
}

HERROR rest_datatype_GetJsonString (HxJSON_t *pstJson, HCHAR *szKey, HCHAR *szString, HUINT32 ulStrLen)
{
	HCHAR			*szJsonString;

	if ((NULL == pstJson) || (NULL == szKey) || (NULL == szString))
	{
		HxLOG_Error(">>> Argument NULL: pstJson(0x%08x), szKey(0x%08x), szString(0x%08x)\n", (HUINT32)pstJson, (HUINT32)szKey, (HUINT32)szString);
		return ERR_FAIL;
	}

	if (HLIB_JSON_Get(pstJson, szKey) == NULL)
	{
		HxLOG_Error(">>> No key (%s) in pstJson\n", szKey);
		return ERR_FAIL;
	}

	szJsonString = (HCHAR *)HLIB_JSON_Object_GetString(pstJson, szKey);
	if (NULL == szJsonString)
	{
		HxLOG_Error(">>> HLIB_JSON_Object_GetString failed\n");
		return ERR_FAIL;
	}

	HxSTD_StrNCpy(szString, szJsonString, ulStrLen);
	return ERR_OK;
}

HERROR rest_datatype_SetJsonString (HINT32 nWriter, HCHAR *szKey, HCHAR *szString, HCHAR *szSelect)
{
	HERROR			 hErr = ERR_OK;

	if ((0 == nWriter) || (NULL == szKey) || (NULL == szString))
	{
		HxLOG_Error(">>> Argument NULL: nWriter(0x%08x), szKey(0x%08x), szString(0x%08x)\n", nWriter, (HUINT32)szKey, (HUINT32)szString);
		return ERR_FAIL;
	}

	if (rest_datatype_CheckWriteSelected(szKey, szSelect))
	{
		hErr = HxJSONWRITER_String(nWriter, szKey, szString);
	}

	return hErr;
}

HERROR rest_datatype_GetJsonEnum (HxJSON_t *pstJson, HCHAR *szKey, HCHAR *szEnumType, HINT32 *pnValue)
{
	HINT32			 nEnumValue = -1;
	HCHAR			*szJsonString;
	HERROR			 hErr;

	if ((NULL == pstJson) || (NULL == szKey) || (NULL == szEnumType) || (NULL == pnValue))
	{
		HxLOG_Error(">>> Argument NULL: pstJson(0x%08x), szKey(0x%08x), szString(0x%08x)\n", (HUINT32)pstJson, (HUINT32)szKey, (HUINT32)szEnumType);
		return ERR_FAIL;
	}

	if (HLIB_JSON_Get(pstJson, szKey) == NULL)
	{
		HxLOG_Error(">>> No key (%s) in pstJson\n", szKey);
		return ERR_FAIL;
	}

	szJsonString = (HCHAR *)HLIB_JSON_Object_GetString(pstJson, szKey);
	if (NULL == szJsonString)
	{
		HxLOG_Error(">>> HLIB_JSON_Object_GetString failed\n");
		return ERR_FAIL;
	}

	hErr = rest_datatype_ConvertStr2Enum(szEnumType, szJsonString, &nEnumValue);
	if (ERR_OK != hErr)
	{
		HxLOG_Error(">> rest_datatype_ConvertStr2Enum err: szEnumType(%s), szJsonString(%s)\n", szEnumType, szJsonString);
		return ERR_FAIL;
	}

	*pnValue = nEnumValue;
	return ERR_OK;
}

HERROR rest_datatype_SetJsonEnum (HINT32 nWriter, HCHAR *szKey, HCHAR *szEnumType, HINT32 nValue, HCHAR *szSelect)
{
	HERROR			 hErr = ERR_OK;

	if ((0 == nWriter) || (NULL == szKey) || (NULL == szEnumType))
	{
		HxLOG_Error(">>> Argument NULL: nWriter(0x%08x), szKey(0x%08x), szEnumType(0x%08x)\n", nWriter, (HUINT32)szKey, (HUINT32)szEnumType);
		return ERR_FAIL;
	}

	if (rest_datatype_CheckWriteSelected(szKey, szSelect))
	{
		hErr = HxJSONWRITER_String(nWriter, szKey, rest_datatype_ConvertEnum2Str(szEnumType, nValue));
	}

	return hErr;
}

HERROR rest_datatype_SetJsonBitwise (HINT32 nWriter, HCHAR *szKey, HCHAR *szEnumType, HINT32 nValue, HCHAR *szSelect)
{
	restDataConvMgr_t	*pstMgr = rest_datatype_GetManager();
	HERROR				 hErr = ERR_OK;

	if ((0 == nWriter) || (NULL == szKey) || (NULL == szEnumType))
	{
		HxLOG_Error(">>> Argument NULL: nWriter(0x%08x), szKey(0x%08x), szEnumType(0x%08x)\n", nWriter, (HUINT32)szKey, (HUINT32)szEnumType);
		return ERR_FAIL;
	}

	if (NULL == pstMgr)
	{
		HxLOG_Error(">> rest_datatype_GetManager err!!!\n");
		return ERR_FAIL;
	}

	if (rest_datatype_CheckWriteSelected(szKey, szSelect))
	{
		HxJSONWRITER_Array(nWriter, szKey);
		{
			HBOOL				 bItemFound = FALSE;
			HUINT32				 ulCnt, ulValue = 0x0001;
			HCHAR				 szKeyValue[128];
			HCHAR				*szEnumName;

			for (ulCnt = 0, ulValue = 0x0001; ulCnt < 32; ulCnt++, ulValue <<= 1)
			{
				HxSTD_PrintToStrN(szKeyValue , 127, "%s.%d", szEnumType, ulValue);
				szEnumName = (HCHAR*)HLIB_HASH_Lookup(pstMgr->pstEnum2Str, (void*)szKeyValue);

				if ((NULL != szEnumName) && ('\0' != *szEnumName))
				{
					bItemFound = TRUE;
					(void)HxJSONWRITER_StringValue(nWriter, szEnumName);
				}

				if (0x80000000 == ulValue)					{ break; }
			}

			if (TRUE != bItemFound)
			{
				HxSTD_PrintToStrN(szKeyValue , 127, "%s.%d", szEnumType, 0);
				szEnumName = (HCHAR*)HLIB_HASH_Lookup(pstMgr->pstEnum2Str, (void*)szKeyValue);
				if (NULL == szEnumName)				{ szEnumName = "none"; }

				(void)HxJSONWRITER_StringValue(nWriter, szEnumName);
			}
		}
		HxJSONWRITER_ArrayEnd(nWriter);
	}

	return hErr;
}


HUINT8 *rest_datatype_GetTempBuffer (HUINT32 ulSize)
{
	HUINT8			 *pucBuffer = NULL;

	if ((0 == ulSize) || (NULL == s_pstRestDataType_CBuffer))
	{
		return NULL;
	}

	if (ulSize > MAX_TEMP_BUFFER_SIZE)			// Temp Buffer 요구가 CBuffer의 반 이상을 차지:
	{
		pucBuffer = (HUINT8 *)HLIB_MEM_Calloc(ulSize);
		if (NULL != pucBuffer)
		{
			rest_datatype_AppendStringBufferToTemporary((HCHAR *)pucBuffer);
		}
	}
	else
	{
		HINT32		 n4ByteAlign = (((ulSize - 1) >> 2) + 1) << 2;

		pucBuffer = HLIB_CBUF_GetBuffer(s_pstRestDataType_CBuffer, n4ByteAlign);
		if (NULL != pucBuffer)
		{
			HxSTD_MemSet(pucBuffer, 0, n4ByteAlign);
		}
	}

	return pucBuffer;
}

void rest_datatype_AppendStringBufferToTemporary (HCHAR *szString)
{
	if (NULL == szString)			{ return; }

	s_pstRestDataType_TempStrList = HLIB_LIST_Append(s_pstRestDataType_TempStrList, (void *)szString);
	if (HLIB_LIST_Length(s_pstRestDataType_TempStrList) > MAX_TEMP_STRING_NUM)
	{
		HCHAR			*szFirst = HLIB_LIST_Data(s_pstRestDataType_TempStrList);

		if (NULL != szFirst)
		{
			s_pstRestDataType_TempStrList = HLIB_LIST_Remove(s_pstRestDataType_TempStrList, (const void *)szFirst);
			HLIB_MEM_Free(szFirst);
		}
	}
}



