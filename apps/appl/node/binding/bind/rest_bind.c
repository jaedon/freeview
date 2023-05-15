/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 rest_bind.c
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <dlib.h>
#include <hapi.h>
#include <apk.h>
#include <rest_maintask.h>
#include <rest_datatype.h>
#include <rest_bind.h>

/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/
#define	hCmdHandle		"rest_bind"

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HINT32				 s_nRestBind_JsonWriter = 0, s_nRestBind_RwStreamWriter = 0;

#define ___LOCAL_FUNCTIONS___
#ifdef CONFIG_DEBUG
STATIC int rest_bind_DebugCmdDummy (void *szArgList)
{
	return HxCMD_OK;
}

STATIC HERROR rest_bind_CmdInit (void)
{
	HLIB_CMD_RegisterWord(hCmdHandle,  rest_bind_DebugCmdDummy,
						"dummy",				/* keyword */
						"dummy",				/* help   */
						"dummy");				/* usage  */

	return ERR_OK;
}
#endif

#define ___MEMBER_FUNCTIONS___
HINT32 rest_bind_OpenJsonWriter (void)
{
	if (0 == s_nRestBind_JsonWriter)
	{
		s_nRestBind_JsonWriter = HLIB_JSONWRITER_Open();
		if (0 == s_nRestBind_JsonWriter)
		{
			HxLOG_Error("HLIB_JSONWRITER_Open err:\n");
			return 0;
		}

		HLIB_JSONWRITER_SetEncodeExceptionToken(s_nRestBind_JsonWriter, ":/ ");
	}

	HLIB_JSONWRITER_Clear(s_nRestBind_JsonWriter);
	return s_nRestBind_JsonWriter;
}

HCHAR *rest_bind_CloseJsonWriter (HINT32 nJsonWriter)
{
	HUINT32			 ulSrcLen;
	HCHAR			*szSrcString, *szDstString = NULL;
	
	if (nJsonWriter != s_nRestBind_JsonWriter)
	{
		HxLOG_Error("nJsonWriter(%d) isn't the handle form 'rest_bind_OpenJsonWriter'\n");
		return NULL;
	}

	szSrcString = (HCHAR *)HLIB_JSONWRITER_GetJSON(nJsonWriter);
	if (NULL == szSrcString)
	{
		HxLOG_Error("HLIB_JSONWRITER_GetJSON error:\n");
		goto END_FUNC;
	}

	ulSrcLen = HxSTD_StrLen(szSrcString);
	szDstString = (HCHAR *)HLIB_STD_MemAlloc(ulSrcLen + 1);
	if (NULL == szDstString)
	{
		HxLOG_Error("Memory allocation failed:\n");
		goto END_FUNC;
	}

	HxSTD_MemCopy(szDstString, szSrcString, ulSrcLen);
	szDstString[ulSrcLen] = '\0';

END_FUNC:
	HLIB_JSONWRITER_Clear(s_nRestBind_JsonWriter);
	return szDstString;
}

HINT32 rest_bind_OpenRwstreamWriter (void)
{
	if (0 == s_nRestBind_RwStreamWriter)
	{
		s_nRestBind_RwStreamWriter = HLIB_RWSTREAM_Open(NULL, 0);
		if (0 == s_nRestBind_RwStreamWriter)
		{
			HxLOG_Error("HLIB_RWSTREAM_Open err:\n");
			return 0;
		}
	}
	
	HLIB_RWSTREAM_Rewind(s_nRestBind_RwStreamWriter);
	return s_nRestBind_RwStreamWriter;
}

HCHAR *rest_bind_CloseRwstreamWriter (HINT32 nRwstream)
{
	HUINT32			 ulResultLen;
	HCHAR			*szResult;
	
	if (nRwstream != s_nRestBind_RwStreamWriter)
	{
		HxLOG_Error("nRwstream(%d) isn't the handle form 'rest_bind_OpenRwstreamWriter'\n");
		return NULL;
	}

	ulResultLen = HLIB_RWSTREAM_GetOffset(nRwstream);
	szResult = HLIB_STD_MemAlloc(ulResultLen + 1);
	if (NULL == szResult)
	{
		HxLOG_Error("Memory allocation failed:\n");
		return NULL;
	}

	HxSTD_MemCopy(szResult, HLIB_RWSTREAM_GetBuf(nRwstream), ulResultLen);
	szResult[ulResultLen] = '\0';

	HLIB_RWSTREAM_Rewind(nRwstream);
	return szResult;
}

HCHAR *rest_bind_MakeErrorString (HINT32 nErrCode, HCHAR *szErrString, HBOOL bMakeObject)
{
	HCHAR				*szOpener, *szCloser;
	HCHAR				*szResult;

	szResult = (HCHAR *)HLIB_STD_MemAlloc(sizeof(HCHAR) * 128);
	if (NULL == szResult)
	{
		HxLOG_Critical("Memory allocation failed:\n");
		return NULL;
	}

	if (TRUE == bMakeObject)
	{
		szOpener = "{";
		szCloser = "}";
	}
	else
	{
		szOpener = "";
		szCloser = "";
	}

	HxSTD_PrintToStrN(szResult, 127, "%s\"error\":{ \"errCode\":%d, \"errString\":\"%s\" }%s", 
						szOpener, 
							nErrCode, szErrString, 
						szCloser);

	return szResult;
}

HERROR rest_bind_GetNumberValueFromJson (HxJSON_t jsObj, HCHAR *szItem, HINT32 *pnNumber)
{
	HxJSON_t		 jsItem;
	
	if ((NULL == jsObj) || (NULL == pnNumber))
	{
		return ERR_FAIL;
	}

	if ((NULL == szItem) || ('\0' == *szItem))
	{
		jsItem = jsObj;
	}
	else
	{
		jsItem = HxJSON_Object_Get(jsObj, szItem);
		if (NULL == jsItem)
		{
			return ERR_FAIL;
		}
	}

	if (HxJSON_IsInteger(jsItem))
	{
		*pnNumber = HxJSON_Integer_Get(jsItem);
		return ERR_OK;
	}
	else if (HxJSON_IsString(jsItem))
	{
		const HCHAR		*szString = HxJSON_String_Get(jsItem);

		if (HLIB_STD_IsInteger(szString))
		{
			*pnNumber = HLIB_STD_StrToINT32(szString);
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HERROR rest_bind_GetStringValueFromJson (HxJSON_t jsObj, HCHAR *szItem, HCHAR **pszString)
{
	HxJSON_t		 jsItem;
	
	if ((NULL == jsObj) || (NULL == pszString))
	{
		return ERR_FAIL;
	}

	if ((NULL == szItem) || ('\0' == *szItem))
	{
		jsItem = jsObj;
	}
	else
	{
		jsItem = HxJSON_Object_Get(jsObj, szItem);
		if (NULL == jsItem)
		{
			return ERR_FAIL;
		}
	}

	if (HxJSON_IsString(jsItem))
	{
		HCHAR		*szString = (HCHAR *)HxJSON_String_Get(jsItem);

		if ((NULL != szString) && ('\0' != *szString))
		{
			*pszString = szString;
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

HINT32 rest_bind_MakeErrorJsWriter (HINT32 nJsWriter, HINT32 nErrCode, HCHAR *szErrString)
{
	HINT32		 nWriter;

	nWriter = (0 == nJsWriter) ? HLIB_JSONWRITER_Open() : nJsWriter;
	if (0 == nWriter)
	{
		HxLOG_Error("HLIB_JSONWRITER_Open failed\n");
		return 0;
	}

	HxJSONWRITER_Object(nWriter, "error");
	{
		HxJSONWRITER_Integer(nWriter, "errCode", nErrCode);
		HxJSONWRITER_String(nWriter, "errString", szErrString);
	}
	HxJSONWRITER_ObjectEnd(nWriter);
	
	return nWriter;
}

#define ___API_FUNCTIONS___
int REST_BIND_Init (void)
{
	STATIC HBOOL	 s_bInited = FALSE;
	HERROR			 hErr, hResult = ERR_FAIL;

	if (FALSE != s_bInited)
	{
		return ERR_OK;
	}

#ifdef	CONFIG_DEBUG
	HLIB_DBG_Init();
#endif

	// Make the thread
	hErr = rest_task_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("rest_task_Init failed:\n");
		goto END_FUNC;
	}
	
	HAPI_Init();

	// Data Type:
	hErr = rest_datatype_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("rest_datatype_Init failed:\n");
		goto END_FUNC;
	}
	
	// Common Functions:
	hErr = rest_bind_common_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("rest_bind_common_Init failed:\n");
		goto END_FUNC;
	}

	// Channel List:
	hErr = rest_bind_chan_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("rest_bind_chan_Init failed:\n");
		goto END_FUNC;
	}

	// Player:
	hErr = rest_bind_play_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("rest_bind_play_Init failed:\n");
		goto END_FUNC;
	}

	// Channel Scanner:
	hErr = rest_bind_scan_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("rest_bind_scan_Init failed:\n");
		goto END_FUNC;
	}

	// EPG
	hErr = rest_bind_epg_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("rest_bind_epg_Init failed:\n");
		goto END_FUNC;
	}

	// Schedule
	hErr = rest_bind_schedule_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("rest_bind_schedule_Init failed:\n");
		goto END_FUNC;
	}

	// Recordings
	hErr = rest_bind_record_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("rest_bind_record_Init failed:\n");
		goto END_FUNC;
	}

	// Recorders
	hErr = rest_bind_recorder_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("rest_bind_recorder_Init failed:\n");
		goto END_FUNC;
	}

	// Settings
	hErr = rest_bind_setting_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("rest_bind_setting_Init failed:\n");
		goto END_FUNC;
	}

	// Pin Code
	hErr = rest_bind_pincode_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("rest_bind_pincode_Init failed:\n");
		goto END_FUNC;
	}

	// Tuner
	hErr = rest_bind_tuner_Init();
	if (ERR_OK != hErr)
	{
		HxLOG_Critical("rest_bind_tuner_Init failed:\n");
		goto END_FUNC;
	}
	
	s_bInited = TRUE;
	HAPI_SendReadySignal(HLIB_STD_GetPID());

#ifdef CONFIG_DEBUG
	// Debug Command:
	(void)rest_bind_CmdInit();
#endif

	hResult = ERR_OK;

END_FUNC:
	return hResult;
}


