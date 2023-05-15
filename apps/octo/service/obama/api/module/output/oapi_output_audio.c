/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_output_audio.c
	@brief

	Description:  									\n
	Module: OAPI / API					\n

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
#include <vkernel.h>
#include <hlib.h>
#include <oapi.h>
#include <obama_int.h>

#include "../../oapi_mem.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
#define OAPI_OUTPUT_AUDIO_JOIN_TIMEOUT		(RPC_DEFAULT_TIMEOUT)

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC HULONG					s_ulOapiOutputAudioSemId = 0x0L;
STATIC OxOutputAudio_Cache_t	s_stOapiOutputAudioCache = {0, };

#define	ENTER_OUTPUT_AUDIO_CACHE	VK_SEM_Get(s_ulOapiOutputAudioSemId)
#define	LEAVE_OUTPUT_AUDIO_CACHE	VK_SEM_Release(s_ulOapiOutputAudioSemId)

#define AUDIO_CHECK_GET_INIT	{if(s_bOutputAudioCacheInited != TRUE) return ERR_FAIL;}
#define AUDIO_CHECK_SET_INIT	{if(s_bOutputAudioCacheInited != TRUE) return ERR_FAIL;}

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL					s_bOutputAudioInited = FALSE;
STATIC HBOOL					s_bOutputAudioCacheInited = FALSE;

STATIC OxOutputAudio_Notifier_t	s_fnOutputAudioApkNotifier;
STATIC HINT32					s_nReqId = -1;
STATIC HINT32					s_nRpcHandle = 0x0;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___


#define _____NOTIFICATION_CALLBACKS_____

STATIC HERROR oapi_output_audio_onInited(HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLog_ExprCheckReturnValue(nArgc != 1, ERR_FAIL);
	HxLog_ExprCheckReturnValue((HxOBJECT_BIN_SIZE(apArgv[0]) != sizeof(OxOutputAudio_Cache_t)), ERR_FAIL);

	ENTER_OUTPUT_AUDIO_CACHE;
	s_stOapiOutputAudioCache = *(OxOutputAudio_Cache_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	s_bOutputAudioCacheInited = TRUE;
	LEAVE_OUTPUT_AUDIO_CACHE;

	HxLOG_Debug("==========> OAPI_OUTPUT_AUDIO status cache refreshed. Good to go ==============>\n");
	return ERR_OK;
}

STATIC HERROR oapi_output_audio_onStatusChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HBOOL	bIsMyRequest = FALSE;

	HxLog_ExprCheckReturnValue(nArgc != 1, ERR_FAIL);
	HxLog_ExprCheckReturnValue((HxOBJECT_BIN_SIZE(apArgv[0]) != sizeof(OxOutputAudio_Cache_t)), ERR_FAIL);

	/*
		Only in last rpc-call in my module's consecutive calls.
		In case updated by other module.
	*/
	bIsMyRequest = HLIB_RPC_IsMyRequest(nRequestId);
	if(nRequestId == s_nReqId || bIsMyRequest == FALSE)
	{
		ENTER_OUTPUT_AUDIO_CACHE;
		s_stOapiOutputAudioCache = *(OxOutputAudio_Cache_t *)HxOBJECT_BIN_DATA(apArgv[0]);
		s_bOutputAudioCacheInited = TRUE;
		LEAVE_OUTPUT_AUDIO_CACHE;

		if(s_fnOutputAudioApkNotifier != NULL)
		{
			(s_fnOutputAudioApkNotifier)();
		}
	}
	HxLOG_Debug("==========> OAPI_OUTPUT_AUDIO status cache refreshed. Good to go ==============>\n");
	return ERR_OK;
}

STATIC void opai_output_audio_checkCacheInited(void)
{
	/* Sema ? It't not important. */
	if (s_bOutputAudioCacheInited == TRUE)
	{
		return;
	}

	if (s_nReqId >= 0)
	{
		HERROR hErr;
#if defined(CONFIG_DEBUG) || defined(CONFIG_DEBUG_RELEASE_LOG)
		HUINT32 sTick, eTick;

		sTick = HLIB_STD_GetSystemTick();
		HxLOG_Critical("==========> Required RPC Join (%s) \n", HLIB_STD_GetCurProcessName());
#endif

		hErr = HLIB_RPC_Join(s_nRpcHandle, s_nReqId, OAPI_OUTPUT_AUDIO_JOIN_TIMEOUT);
		if (hErr == ERR_FAIL)
		{
			HxLOG_Critical(HxANSI_COLOR_RED("JOIN TIMED OUT")"\n");
		}
#if defined(CONFIG_DEBUG) || defined(CONFIG_DEBUG_RELEASE_LOG)
		eTick = HLIB_STD_GetSystemTick();
		HxLOG_Critical("==========> Join Tick=(%09d)\n", eTick-sTick);
#endif
	}
}


#define ___GLOBAL_FUNCTIONS___

HERROR OAPI_OUTPUT_AUDIO_Init(OxOutputAudio_Notifier_t fnNotifier)
{
	HERROR	hErr;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	if (s_bOutputAudioInited != TRUE)
	{
		if(VK_SEM_Create(&s_ulOapiOutputAudioSemId, "OapiOutputAudioSema", VK_SUSPENDTYPE_PRIORITY)  != VK_OK)
		{
			HxLOG_Error("[%s] Error in VK_SEM_Create()",  __FUNCTION__);
			return ERR_FAIL;
		}

		hErr = OAPI_RpcProtocol_GetRpcHandle(&s_nRpcHandle);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("==========> ERROR !!!! %s() has OAPI_RpcProtocol_GetRpcHandle() ERROR !!!!\n\n", __FUNCTION__);
			return ERR_FAIL;
		}

		s_fnOutputAudioApkNotifier = fnNotifier;

		hErr = HLIB_RPC_RegisterNotifyCallback(s_nRpcHandle, RPC_OBAMA_OUTPUT_AUDIO_onStatusChanged, "b", NULL, oapi_output_audio_onStatusChanged);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("==========> ERROR !!!! %s() has HLIB_RPC_RegisterNotifyCallback() ERROR !!!!\n\n", __FUNCTION__);
			return ERR_FAIL;
		}

		s_nReqId = HLIB_RPC_Request(s_nRpcHandle, (HxRPC_Response_t)oapi_output_audio_onInited, NULL, RPC_OAPI_OUTPUT_AUDIO_Init, NULL);
		s_bOutputAudioInited = TRUE;
	}

	return ERR_OK;
}

HERROR OAPI_OUTPUT_AUDIO_DeInit(void)
{
	ENTER_OUTPUT_AUDIO_CACHE;
	if (s_bOutputAudioInited == TRUE)
	{
		/* RCP_Close() 하지 않고,
		   OAPI 종료가 필요한 Process가 APK_DeInit() 통해 OAPI RPC/socket/task 정리한다 */
		if (s_fnOutputAudioApkNotifier != NULL)
		{
			s_fnOutputAudioApkNotifier = NULL;
		}

		HxSTD_MemSet(&s_stOapiOutputAudioCache, 0x0, sizeof(OxOutputAudio_Cache_t));
		s_bOutputAudioInited = FALSE;
	}
	LEAVE_OUTPUT_AUDIO_CACHE;
	VK_SEM_Destroy(s_ulOapiOutputAudioSemId); s_ulOapiOutputAudioSemId = 0x0;
	return ERR_OK;
}

HERROR OAPI_OUTPUT_AUDIO_SetMasterMute(HBOOL bMute)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bMute);
	opai_output_audio_checkCacheInited();
	AUDIO_CHECK_SET_INIT;

	ENTER_OUTPUT_AUDIO_CACHE;
	s_stOapiOutputAudioCache.bMasterMute = bMute;
	LEAVE_OUTPUT_AUDIO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetMasterMute, "i", bMute);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_AUDIO_SetVolume(HUINT32 ulVolume)
{
	HxLOG_Debug("%s(ulVolume : %d)\n", __FUNCTION__, ulVolume);
	opai_output_audio_checkCacheInited();
	AUDIO_CHECK_SET_INIT;

	ENTER_OUTPUT_AUDIO_CACHE;
	s_stOapiOutputAudioCache.ulVolume = ulVolume;
	LEAVE_OUTPUT_AUDIO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetVolume, "i", ulVolume);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_AUDIO_SetDecoderVolume(HUINT32 ulVolume)
{
	HxLOG_Debug("%s(ulVolume : %d)\n", __FUNCTION__, ulVolume);
	opai_output_audio_checkCacheInited();
	AUDIO_CHECK_SET_INIT;

	ENTER_OUTPUT_AUDIO_CACHE;
	s_stOapiOutputAudioCache.ulDecoderVolume = ulVolume;
	LEAVE_OUTPUT_AUDIO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetDecoderVolume, "i", ulVolume);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_AUDIO_SetLipSyncDelay(HUINT32 ulmsec)
{
	HxLOG_Debug("%s(ulmsec : %d)\n", __FUNCTION__, ulmsec);
	opai_output_audio_checkCacheInited();
	AUDIO_CHECK_SET_INIT;

	ENTER_OUTPUT_AUDIO_CACHE;
	s_stOapiOutputAudioCache.ulLipSyncDelaymsec = ulmsec;
	LEAVE_OUTPUT_AUDIO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetLipSyncDelay, "i", ulmsec);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_AUDIO_SetHdmiFormat(DxDigitalAudioFormat_e eDigitalAudioFormat)
{
	HxLOG_Debug("%s(eDigitalAudioFormat : %d)\n", __FUNCTION__, eDigitalAudioFormat);
	opai_output_audio_checkCacheInited();
	AUDIO_CHECK_SET_INIT;

	ENTER_OUTPUT_AUDIO_CACHE;
	s_stOapiOutputAudioCache.eHdmiAudioFormat = eDigitalAudioFormat;
	LEAVE_OUTPUT_AUDIO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetHdmiFormat, "i", eDigitalAudioFormat);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_AUDIO_SetSpdifFormat(DxDigitalAudioFormat_e eDigitalAudioFormat)
{
	HxLOG_Debug("%s(eDigitalAudioFormat : %d)\n", __FUNCTION__, eDigitalAudioFormat);
	opai_output_audio_checkCacheInited();
	AUDIO_CHECK_SET_INIT;

	ENTER_OUTPUT_AUDIO_CACHE;
	s_stOapiOutputAudioCache.eSpdifAudioFormat = eDigitalAudioFormat;
	LEAVE_OUTPUT_AUDIO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetSpdifFormat, "i", eDigitalAudioFormat);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_AUDIO_SetAudioDescription(HBOOL bEnable)
{
	HxLOG_Debug("%s (bEnable : %d)\n", __FUNCTION__, bEnable);
	opai_output_audio_checkCacheInited();
	AUDIO_CHECK_SET_INIT;

	ENTER_OUTPUT_AUDIO_CACHE;
	s_stOapiOutputAudioCache.bAudioDescription = bEnable;
	LEAVE_OUTPUT_AUDIO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetAudioDescription, "i", bEnable);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_AUDIO_SetStereoSelect(DxStereoSelect_e eStereoSelect)
{
	HxLOG_Debug("%s(eStereoSelect : %d)\n", __FUNCTION__, eStereoSelect);
	opai_output_audio_checkCacheInited();
	AUDIO_CHECK_SET_INIT;

	ENTER_OUTPUT_AUDIO_CACHE;
	s_stOapiOutputAudioCache.eStereoSelect = eStereoSelect;
	LEAVE_OUTPUT_AUDIO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetStereoSelect, "i", eStereoSelect);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_AUDIO_SetDualMonoSelect(DxDualMonoSelect_e eDualMonoSelect)
{
	HxLOG_Debug("%s (eDualMonoSelect : %d)\n", __FUNCTION__, eDualMonoSelect);
	opai_output_audio_checkCacheInited();
	AUDIO_CHECK_SET_INIT;

	ENTER_OUTPUT_AUDIO_CACHE;
	s_stOapiOutputAudioCache.eDualMonoSelect = eDualMonoSelect;
	LEAVE_OUTPUT_AUDIO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetDualMonoSelect, "i", eDualMonoSelect);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_AUDIO_GetStatus(OxOutputAudio_Cache_t *pstStatus)
{
	HxLOG_Debug("%s\n", __FUNCTION__);
	opai_output_audio_checkCacheInited();
	AUDIO_CHECK_GET_INIT;

	ENTER_OUTPUT_AUDIO_CACHE;
	if (pstStatus != NULL)
	{
		*pstStatus = s_stOapiOutputAudioCache;
	}
	LEAVE_OUTPUT_AUDIO_CACHE;

	return ERR_OK;
}



