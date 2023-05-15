/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk2_main.c
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

#include <apk.h>
#include <_apk_int.h>

#define ___INTERNAL_STRUCTURES___

#define ___INTERNAL_VARIABLES___
STATIC HBOOL			 s_bAppkitMain_Inited = FALSE;
STATIC HUINT32			 s_ulAppkitMain_AppkitId;

#define ___PRIVATE_FUNCTION___

#if 0
STATIC void apk_OapiOnAppkitActivated (HERROR hRet, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	// main.onAppkitActivated : (int appkitId)
	s_ulAppkitMain_AppkitId	= HxOBJECT_INT (argv[0]);
	s_bAppkitMain_Inited	= TRUE;

	// Start the other APPKIT modules:
	// APK2_MEDIAPLAY_Init();
	// APK2_MEDIAREC_Init();
}
#endif

STATIC HERROR apk_InitOapiRpcSetting (void)
{
	HINT32					 nRpcHandle = 0;
	HERROR					 hErr = ERR_FAIL;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("[%s:%d] OAPI_RpcProtocol_GetRpcHandle() failed.. \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

//	HLIB_RPC_RegisterNotifyCallback (nRpcHandle, "main.onAppkitActivated", NULL, NULL, (HxRPC_Noti_t)apk_OapiOnAppkitActivated);
//	HLIB_RPC_Call (nRpcHandle, "main.appkitActivate", NULL);

	return ERR_OK;
}

#define ___MEMBER_FUNCTION___
HERROR APKI_GetAppkitId (HUINT32 *pulAppkitId)
{
	if (TRUE == s_bAppkitMain_Inited)
	{
		*pulAppkitId = s_ulAppkitMain_AppkitId;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HBOOL APKI_IsAppkitMainInited (void)
{
	return s_bAppkitMain_Inited;
}

#define ___API_FUNCTION___
HERROR APK_Init (void)
{
	apk_InitOapiRpcSetting();

	return ERR_OK;
}

#ifdef	CONFIG_DEBUG
HERROR	APK_CMD_Init(void)
{
	return ERR_OK;
}
#endif

/* end of file */
