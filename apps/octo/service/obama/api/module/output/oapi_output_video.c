/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_output_video.c
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
#define OAPI_OUTPUT_VIDEO_JOIN_TIMEOUT		(RPC_DEFAULT_TIMEOUT)


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HULONG					s_ulOapiOutputVideoSemId = 0x0L;

STATIC HBOOL					s_bOutputVideoInited = FALSE;
STATIC HBOOL					s_bOutputVideoCacheInited = FALSE;

STATIC OxOutputVideo_Cache_t	s_stOapiOutputVideoCache = {0, };
STATIC OxOutputVideo_Notifier_t	s_fOutputVideoApkNotifier = NULL;
STATIC HINT32					s_nReqId = -1;
STATIC HINT32					s_nRpcHandle = 0;

#define	ENTER_OUTPUT_VIDEO_CACHE	VK_SEM_Get(s_ulOapiOutputVideoSemId)
#define	LEAVE_OUTPUT_VIDEO_CACHE	VK_SEM_Release(s_ulOapiOutputVideoSemId)

#define VIDEO_CHECK_GET_INIT	{if(s_bOutputVideoCacheInited != TRUE) return ERR_FAIL;}
#define VIDEO_CHECK_SET_INIT	{if(s_bOutputVideoCacheInited != TRUE) return ERR_FAIL;}

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___


#define _____NOTIFICATION_CALLBACKS_____

STATIC HERROR oapi_output_video_onInited(HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLog_ExprCheckReturnValue((nArgc != 1), ERR_FAIL);
	HxLog_ExprCheckReturnValue((HxOBJECT_BIN_SIZE(apArgv[0]) != sizeof(OxOutputVideo_Cache_t)), ERR_FAIL);

	ENTER_OUTPUT_VIDEO_CACHE;
	s_stOapiOutputVideoCache = *(OxOutputVideo_Cache_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	s_bOutputVideoCacheInited = TRUE;
	LEAVE_OUTPUT_VIDEO_CACHE;

	return ERR_OK;
}

STATIC HERROR oapi_output_video_onStatusChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HBOOL	bIsMyRequest = FALSE;

	HxLog_ExprCheckReturnValue((nArgc != 1), ERR_FAIL);
	HxLog_ExprCheckReturnValue((HxOBJECT_BIN_SIZE(apArgv[0]) != sizeof(OxOutputVideo_Cache_t)), ERR_FAIL);

	/*
		Only in last rpc-call in my module's consecutive calls.
		In case updated by other module.
	*/
	bIsMyRequest = HLIB_RPC_IsMyRequest(nRequestId);
	if (nRequestId == s_nReqId || bIsMyRequest == FALSE)
	{
		ENTER_OUTPUT_VIDEO_CACHE;
		s_stOapiOutputVideoCache = *(OxOutputVideo_Cache_t *)HxOBJECT_BIN_DATA(apArgv[0]);
		s_bOutputVideoCacheInited = TRUE;
		LEAVE_OUTPUT_VIDEO_CACHE;

		if (s_fOutputVideoApkNotifier != NULL)
		{
			(s_fOutputVideoApkNotifier)();
		}
	}
	return ERR_OK;
}

STATIC void opai_output_video_checkCacheInited(void)
{
	/* Sema ? It't not important. */
	if (s_bOutputVideoCacheInited == TRUE)
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

		hErr = HLIB_RPC_Join(s_nRpcHandle, s_nReqId, OAPI_OUTPUT_VIDEO_JOIN_TIMEOUT);
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

#if defined(CONFIG_DEBUG)
STATIC void oapi_output_video_PrintStatus(void)
{
	HLIB_CMD_Printf("================== OM_OUTPUT_VIDEO STATUS =================\n");

	HLIB_CMD_Printf("\tbSupportComposite : %d\n", s_stOapiOutputVideoCache.bSupportComposite);
	HLIB_CMD_Printf("\tbSupportTvScart : %d\n", s_stOapiOutputVideoCache.bSupportTvScart);
	HLIB_CMD_Printf("\tbSupportVcrScart : %d\n", s_stOapiOutputVideoCache.bSupportVcrScart);
	HLIB_CMD_Printf("\tulSupportTvScartFormat : 0x%x\n", s_stOapiOutputVideoCache.ulSupportTvScartFormat);
	HLIB_CMD_Printf("\tulSupportVcrScartFormat : 0x%x\n", s_stOapiOutputVideoCache.ulSupportVcrScartFormat);
	HLIB_CMD_Printf("\tbAvEnable : %d\n", s_stOapiOutputVideoCache.bAvEnable);
	HLIB_CMD_Printf("\teTvScartFormat : %d\n", s_stOapiOutputVideoCache.eTvScartFormat);
	HLIB_CMD_Printf("\teVcrScartFormat : %d\n", s_stOapiOutputVideoCache.eVcrScartFormat);
	HLIB_CMD_Printf("\teScartPath : %d\n", s_stOapiOutputVideoCache.eScartPath);
	HLIB_CMD_Printf("\teResolutionSelect : %d\n", s_stOapiOutputVideoCache.eResolutionSelect);
	HLIB_CMD_Printf("\teTvAspectRatio : %d\n", s_stOapiOutputVideoCache.eTvAspectRatio);
	HLIB_CMD_Printf("\teDfcSelect : %d\n", s_stOapiOutputVideoCache.eDfcSelect);
	HLIB_CMD_Printf("\tBG color : %d/%d/%d\n", s_stOapiOutputVideoCache.ucBgColor_R, s_stOapiOutputVideoCache.ucBgColor_G, s_stOapiOutputVideoCache.ucBgColor_B);
	HLIB_CMD_Printf("\teResolutionStatus : %d\n", s_stOapiOutputVideoCache.eResolutionStatus);
	HLIB_CMD_Printf("\tePictureRate : %d\n", s_stOapiOutputVideoCache.ePictureRate);
	HLIB_CMD_Printf("\tulDfcStatus : 0x%x\n", s_stOapiOutputVideoCache.ulDfcStatus);
	HLIB_CMD_Printf("\teCompositeStandard : %d\n", s_stOapiOutputVideoCache.eCompositeStandard);

	HLIB_CMD_Printf("===========================================================\n");
}

STATIC int oapi_output_video_ProcessCommand_status(void *szArgList)
{
	HLIB_CMD_Printf("Ping pal_scaler API semaphore ...\n");

	oapi_output_video_PrintStatus();

	return HxCMD_OK;
}

STATIC void oapi_output_video_RegisterCommand(void)
{
#define	hCmdHandle		"oapi_output_video"

	HLIB_CMD_RegisterWord(hCmdHandle,
						oapi_output_video_ProcessCommand_status,
						"status",
						"Print oapi_output_video status.",
						"status");
}
#endif

#define ___GLOBAL_FUNCTIONS___

HERROR OAPI_OUTPUT_VIDEO_Init(OxOutputVideo_Notifier_t fnNotifier)
{
	HERROR	hErr;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	if (s_bOutputVideoInited == FALSE)
	{
		if(VK_SEM_Create(&s_ulOapiOutputVideoSemId, "OapiOutputVideoSema", VK_SUSPENDTYPE_PRIORITY) != VK_OK)
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

		s_fOutputVideoApkNotifier = fnNotifier;

		hErr = HLIB_RPC_RegisterNotifyCallback(s_nRpcHandle, RPC_OBAMA_OUTPUT_VIDEO_onStatusChanged, "b", NULL, oapi_output_video_onStatusChanged);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("==========> ERROR !!!! %s() has HLIB_RPC_RegisterNotifyCallback() ERROR !!!!\n\n", __FUNCTION__);
			return ERR_FAIL;
		}

		s_nReqId = HLIB_RPC_Request(s_nRpcHandle, (HxRPC_Response_t)oapi_output_video_onInited, NULL, RPC_OAPI_OUTPUT_VIDEO_Init, NULL);

		s_bOutputVideoInited = TRUE;
	}

#if defined(CONFIG_DEBUG)
	oapi_output_video_RegisterCommand();
#endif

	return ERR_OK;
}

HERROR OAPI_OUTPUT_VIDEO_DeInit(void)
{
	ENTER_OUTPUT_VIDEO_CACHE;
	if (s_bOutputVideoInited == TRUE)
	{
		/* RCP_Close() 하지 않고,
		   OAPI 종료가 필요한 Process가 APK_DeInit() 통해 OAPI RPC/socket/task 정리한다 */
		if (s_fOutputVideoApkNotifier != NULL)
		{
			s_fOutputVideoApkNotifier = NULL;
		}

		HxSTD_MemSet(&s_stOapiOutputVideoCache, 0x0, sizeof(OxOutputVideo_Cache_t));
		s_bOutputVideoInited = FALSE;
	}
	LEAVE_OUTPUT_VIDEO_CACHE;
	VK_SEM_Destroy(s_ulOapiOutputVideoSemId); s_ulOapiOutputVideoSemId = 0x0;
	return ERR_OK;
}

HERROR OAPI_OUTPUT_VIDEO_GetStatus(OxOutputVideo_Cache_t *pstStatus)
{
	opai_output_video_checkCacheInited();
	VIDEO_CHECK_GET_INIT;

	ENTER_OUTPUT_VIDEO_CACHE;
	*pstStatus = s_stOapiOutputVideoCache;
	LEAVE_OUTPUT_VIDEO_CACHE;

	return ERR_OK;
}


HERROR OAPI_OUTPUT_VIDEO_SetTvScartFormat(DxScartFormat_e eScartFormat)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, eScartFormat);

	opai_output_video_checkCacheInited();
	VIDEO_CHECK_SET_INIT;

	ENTER_OUTPUT_VIDEO_CACHE;
	s_stOapiOutputVideoCache.eTvScartFormat = eScartFormat;
	LEAVE_OUTPUT_VIDEO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetTvScartFormat, "i", eScartFormat);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_VIDEO_SetVcrScartFormat(DxScartFormat_e eScartFormat)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, eScartFormat);

	opai_output_video_checkCacheInited();
	VIDEO_CHECK_SET_INIT;

	ENTER_OUTPUT_VIDEO_CACHE;
	s_stOapiOutputVideoCache.eVcrScartFormat = eScartFormat;
	LEAVE_OUTPUT_VIDEO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetVcrScartFormat, "i", eScartFormat);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_VIDEO_SetScartPath(DxScartPath_e eScartPath)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, eScartPath);

	opai_output_video_checkCacheInited();
	VIDEO_CHECK_SET_INIT;

	ENTER_OUTPUT_VIDEO_CACHE;
	s_stOapiOutputVideoCache.eScartPath = eScartPath;
	LEAVE_OUTPUT_VIDEO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetScartPath, "i", eScartPath);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_VIDEO_SetResolutionSelect(DxResolutionSelect_e eResolutionSelect)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, eResolutionSelect);

	opai_output_video_checkCacheInited();
	VIDEO_CHECK_SET_INIT;

	ENTER_OUTPUT_VIDEO_CACHE;
	s_stOapiOutputVideoCache.eResolutionSelect = eResolutionSelect;
	LEAVE_OUTPUT_VIDEO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetResolutionSelect, "i", eResolutionSelect);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_VIDEO_SetTvAspectRatio(DxAspectRatio_e eTvAspectRatio)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, eTvAspectRatio);

	opai_output_video_checkCacheInited();
	VIDEO_CHECK_SET_INIT;

	ENTER_OUTPUT_VIDEO_CACHE;
	s_stOapiOutputVideoCache.eTvAspectRatio = eTvAspectRatio;
	LEAVE_OUTPUT_VIDEO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetTvAspectRatio, "i", eTvAspectRatio);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_VIDEO_SetDfcSelect(DxDfcSelect_e eDfcSelect)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, eDfcSelect);

	opai_output_video_checkCacheInited();
	VIDEO_CHECK_SET_INIT;

	ENTER_OUTPUT_VIDEO_CACHE;
	s_stOapiOutputVideoCache.eDfcSelect = eDfcSelect;
	LEAVE_OUTPUT_VIDEO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetDfcSelect, "i", eDfcSelect);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_VIDEO_SetBackGroundColor(HUINT8 ucR, HUINT8 ucG, HUINT8 ucB)
{
	HxLOG_Debug("%s(%d|%d|%d)\n", __FUNCTION__, ucR, ucG, ucB);

	opai_output_video_checkCacheInited();
	VIDEO_CHECK_SET_INIT;

	ENTER_OUTPUT_VIDEO_CACHE;
	s_stOapiOutputVideoCache.ucBgColor_R = ucR;
	s_stOapiOutputVideoCache.ucBgColor_G = ucG;
	s_stOapiOutputVideoCache.ucBgColor_B = ucB;
	LEAVE_OUTPUT_VIDEO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetBackGroundColor, "iii", ucR, ucG, ucB);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_VIDEO_SetAvEnable(HBOOL bEnable)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bEnable);

	opai_output_video_checkCacheInited();
	VIDEO_CHECK_SET_INIT;

	ENTER_OUTPUT_VIDEO_CACHE;
	s_stOapiOutputVideoCache.bAvEnable = bEnable;
	LEAVE_OUTPUT_VIDEO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetAvEnable, "i", bEnable);
	return ERR_OK;
}



HERROR OAPI_OUTPUT_VIDEO_SetSupportVdieoFrame(HBOOL bEnable)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bEnable);

	opai_output_video_checkCacheInited();
	VIDEO_CHECK_SET_INIT;

	ENTER_OUTPUT_VIDEO_CACHE;
	s_stOapiOutputVideoCache.bSupportVideoFrame = bEnable;
	LEAVE_OUTPUT_VIDEO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SupportVideoFrame, "i", bEnable);
	return ERR_OK;
}

HERROR OAPI_OUTPUT_VIDEO_SetVideoBlankCopyRight(DxCopyRight_e eVideoBlankCopyRight)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, eVideoBlankCopyRight);

	opai_output_video_checkCacheInited();
	VIDEO_CHECK_SET_INIT;

	ENTER_OUTPUT_VIDEO_CACHE;
	s_stOapiOutputVideoCache.eVideoBlankCopyRight = eVideoBlankCopyRight;
	LEAVE_OUTPUT_VIDEO_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetVideoBlankCopyRight, "i", eVideoBlankCopyRight);
	return ERR_OK;
}


