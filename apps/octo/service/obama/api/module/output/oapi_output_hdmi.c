/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  oapi_output_hdmi.c
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
STATIC HULONG					s_ulOapiOutputHdmiSemId = 0x0L;
STATIC OxOutputHdmi_Cache_t		s_stOapiOutputHdmiCache = {0, };

#define	ENTER_OUTPUT_HDMI_CACHE	VK_SEM_Get(s_ulOapiOutputHdmiSemId)
#define	LEAVE_OUTPUT_HDMI_CACHE	VK_SEM_Release(s_ulOapiOutputHdmiSemId)

#define HDMI_CHECK_GET_INIT	{if(s_bOutputHdmiCacheInited != TRUE) return ERR_FAIL;}
#define HDMI_CHECK_SET_INIT	{if(s_bOutputHdmiCacheInited != TRUE) return ERR_FAIL;}

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HBOOL					s_bOutputHdmiInited = FALSE;
STATIC HBOOL					s_bOutputHdmiCacheInited = FALSE;

STATIC OxOutputAudio_Notifier_t	s_pfOutputHdmiApkNotifier = NULL;
STATIC HINT32					s_nReqId = -1;
STATIC HINT32					s_nRpcHandle = 0;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/
#define ___LOCAL_FUNCTIONS___


#define _____NOTIFICATION_CALLBACKS_____

STATIC HERROR oapi_output_hdmi_onInited(HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLog_ExprCheckReturnValue(nArgc != 1, ERR_FAIL);
	HxLog_ExprCheckReturnValue((HxOBJECT_BIN_SIZE(apArgv[0]) != sizeof(OxOutputHdmi_Cache_t)), ERR_FAIL);

	ENTER_OUTPUT_HDMI_CACHE;
	s_stOapiOutputHdmiCache = *(OxOutputHdmi_Cache_t *)HxOBJECT_BIN_DATA(apArgv[0]);
	s_bOutputHdmiCacheInited = TRUE;
	LEAVE_OUTPUT_HDMI_CACHE;

	HxLOG_Debug("==========> OAPI_OUTPUT_HDMI status cache refreshed. Good to go ==============>\n");
	return ERR_OK;
}

STATIC HERROR oapi_output_hdmi_onStatusChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HBOOL	bIsMyRequest = FALSE;

	HxLog_ExprCheckReturnValue(nArgc != 1, ERR_FAIL);
	HxLog_ExprCheckReturnValue((HxOBJECT_BIN_SIZE(apArgv[0]) != sizeof(OxOutputHdmi_Cache_t)), ERR_FAIL);

	/*
		Only in last rpc-call in my module's consecutive calls.
		In case updated by other module.
	*/
	bIsMyRequest = HLIB_RPC_IsMyRequest(nRequestId);
	if(nRequestId == s_nReqId || bIsMyRequest == FALSE)
	{
		ENTER_OUTPUT_HDMI_CACHE;
		s_stOapiOutputHdmiCache = *(OxOutputHdmi_Cache_t *)HxOBJECT_BIN_DATA(apArgv[0]);
		s_bOutputHdmiCacheInited = TRUE;
		LEAVE_OUTPUT_HDMI_CACHE;

		if(s_pfOutputHdmiApkNotifier != NULL)
		{
			(s_pfOutputHdmiApkNotifier)();
		}
	}

	HxLOG_Debug("==========> OAPI_OUTPUT_HDMI status cache refreshed. Good to go ==============>\n");
	return ERR_OK;
}

STATIC void opai_output_hdmi_checkCacheInited(void)
{
	/* Sema ? It't not important. */
	if (s_bOutputHdmiCacheInited == TRUE)
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
STATIC void oapi_output_hdmi_PrintStatus(void)
{
	HUINT32		i;

	HLIB_CMD_Printf("================== OAPI_OUTPUT_HDMI STATUS =================\n");
	HLIB_CMD_Printf("\tbSupportCec : %d\n", s_stOapiOutputHdmiCache.bSupportCec);
	HLIB_CMD_Printf("\tbCecEnable : %d\n", s_stOapiOutputHdmiCache.bCecEnable);
	HLIB_CMD_Printf("\teHdcpStatus : %d\n", s_stOapiOutputHdmiCache.eHdcpStatus);
	HLIB_CMD_Printf("\teHdmi3DSelect : %d\n", s_stOapiOutputHdmiCache.eHdmi3DSelect);
	HLIB_CMD_Printf("\teHdcpVer : %d\n", s_stOapiOutputHdmiCache.eHdcpVer);
	HLIB_CMD_Printf("\tul3DFormatCapability\n");

	for(i = 0; i < 5; i++)
	{
		if((s_stOapiOutputHdmiCache.ul3DFormatCapability & (1 << i)) != 0)
		{
			HLIB_CMD_Printf("\t\t0x%x\n", (1 << i));
		}
	}

	HLIB_CMD_Printf("\teHdmi3DStatus : %d\n", s_stOapiOutputHdmiCache.eHdmi3DStatus);
	HLIB_CMD_Printf("\tul3DFormatCapability\n");

	for(i = 0; i < 32; i++)
	{
		if((s_stOapiOutputHdmiCache.ulResolutionCapability & (1 << i)) != 0)
		{
			HLIB_CMD_Printf("\t\t0x%x\n", (1 << i));
		}
	}
	HLIB_CMD_Printf("==================================================\n");
}

STATIC int oapi_output_hdmi_ProcessCommand_status(void *szArgList)
{
	oapi_output_hdmi_PrintStatus();

	return HxCMD_OK;
}

STATIC void oapi_output_hdmi_RegisterCommand(void)
{
#define	hCmdHandle		"oapi_output_hdmi"

	HLIB_CMD_RegisterWord(hCmdHandle,
						oapi_output_hdmi_ProcessCommand_status,
						"status",
						"Print oapi_output_hdmi status.",
						"status");
}
#endif

#define ___GLOBAL_FUNCTIONS___

HERROR OAPI_OUTPUT_HDMI_Init(OxOutputHdmi_Notifier_t fnNotifier)
{
	HERROR	hErr;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	if (s_bOutputHdmiInited != TRUE)
	{
		if(VK_SEM_Create(&s_ulOapiOutputHdmiSemId, "OapiOutputAudioSema", VK_SUSPENDTYPE_PRIORITY) != VK_OK)
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

		s_pfOutputHdmiApkNotifier = fnNotifier;

		hErr = HLIB_RPC_RegisterNotifyCallback(s_nRpcHandle, RPC_OBAMA_OUTPUT_HDMI_onStatusChanged, "b", NULL, oapi_output_hdmi_onStatusChanged);
		if(hErr != ERR_OK)
		{
			HxLOG_Error("==========> ERROR !!!! %s() has HLIB_RPC_RegisterNotifyCallback() ERROR !!!!\n\n", __FUNCTION__);
			return ERR_FAIL;
		}

		s_nReqId = HLIB_RPC_Request(s_nRpcHandle, (HxRPC_Response_t)oapi_output_hdmi_onInited, NULL, RPC_OAPI_OUTPUT_HDMI_Init, NULL);
		s_bOutputHdmiInited = TRUE;

#if defined(CONFIG_DEBUG)
		oapi_output_hdmi_RegisterCommand();
#endif
	}

	return ERR_OK;
}

HERROR OAPI_OUTPUT_HDMI_DeInit(void)
{
	ENTER_OUTPUT_HDMI_CACHE;
	if (s_bOutputHdmiInited == TRUE)
	{
		/* RCP_Close() 하지 않고,
		   OAPI 종료가 필요한 Process가 APK_DeInit() 통해 OAPI RPC/socket/task 정리한다 */
		if (s_pfOutputHdmiApkNotifier != NULL)
		{
			s_pfOutputHdmiApkNotifier = NULL;
		}

		HxSTD_MemSet(&s_stOapiOutputHdmiCache, 0x0, sizeof(OxOutputHdmi_Cache_t));
		s_bOutputHdmiInited = FALSE;
	}
	LEAVE_OUTPUT_HDMI_CACHE;
	VK_SEM_Destroy(s_ulOapiOutputHdmiSemId); s_ulOapiOutputHdmiSemId = 0x0;
	return ERR_OK;
}

HERROR OAPI_OUTPUT_HDMI_Set3DSelect(DxHdmi3DSelect_e eHdmi3DSelect)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, eHdmi3DSelect);

	opai_output_hdmi_checkCacheInited();
	HDMI_CHECK_SET_INIT;

	ENTER_OUTPUT_HDMI_CACHE;
	s_stOapiOutputHdmiCache.eHdmi3DSelect = eHdmi3DSelect;
	LEAVE_OUTPUT_HDMI_CACHE;

	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_HDMI_Set3DSelect, "i", eHdmi3DSelect);

	return ERR_OK;
}

HERROR OAPI_OUTPUT_HDMI_SetCecEnable(HBOOL bEnable)
{
	HERROR		err;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bEnable);

	opai_output_hdmi_checkCacheInited();
	HDMI_CHECK_SET_INIT;

	ENTER_OUTPUT_HDMI_CACHE;
	if(s_stOapiOutputHdmiCache.bSupportCec == TRUE)
	{
		s_stOapiOutputHdmiCache.bCecEnable = bEnable;
		s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_HDMI_SetCecEnable, "i", bEnable);
		err = ERR_OK;
	}
	else
	{
		s_stOapiOutputHdmiCache.bCecEnable = FALSE;
		err = ERR_FAIL;
	}
	LEAVE_OUTPUT_HDMI_CACHE;

	return err;
}

HERROR OAPI_OUTPUT_HDMI_SetCecVolumeCtrlEnable(HBOOL bEnable)
{
	HERROR		err = ERR_FAIL;

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bEnable);

	opai_output_hdmi_checkCacheInited();
	HDMI_CHECK_SET_INIT;

	ENTER_OUTPUT_HDMI_CACHE;

	if(s_stOapiOutputHdmiCache.bSupportCec == TRUE)
	{
        if(s_stOapiOutputHdmiCache.bCecEnable == TRUE)
        {
            s_stOapiOutputHdmiCache.bCecVolumeCtrlEnable = bEnable;
    	    s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_HDMI_CEC_SetVolumeEnable, "i", bEnable);
        }
        else
        {
            s_stOapiOutputHdmiCache.bCecVolumeCtrlEnable = FALSE;
			s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_HDMI_CEC_SetVolumeEnable, "i", FALSE);
        }
		err = ERR_OK;
	}

	LEAVE_OUTPUT_HDMI_CACHE;

	return err;
}


HERROR OAPI_OUTPUT_HDMI_CEC_UserCtrlPressed(DxHdmiUserCtrlPressed_e eParam)
{
    HxLOG_Trace();

	opai_output_hdmi_checkCacheInited();
	HDMI_CHECK_SET_INIT;

	ENTER_OUTPUT_HDMI_CACHE;
    if(s_stOapiOutputHdmiCache.bSupportCec == FALSE)
    {
        HxLOG_Warning("bSupportCec(%d),bCecEnable(%d)\n",s_stOapiOutputHdmiCache.bSupportCec,s_stOapiOutputHdmiCache.bCecEnable);
		LEAVE_OUTPUT_HDMI_CACHE;
        return ERR_FAIL;
    }
    s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_HDMI_CEC_UserCtrlPressed, "i", eParam);
	LEAVE_OUTPUT_HDMI_CACHE;

	return ERR_OK;


}

HERROR OAPI_OUTPUT_HDMI_GetStatus(OxOutputHdmi_Cache_t *pstStatus)
{
	opai_output_hdmi_checkCacheInited();
	HDMI_CHECK_GET_INIT;

	ENTER_OUTPUT_HDMI_CACHE;
	*pstStatus = s_stOapiOutputHdmiCache;
	LEAVE_OUTPUT_HDMI_CACHE;

	return ERR_OK;
}

HERROR OAPI_OUTPUT_HDMI_GetNetflixDrmStatus(DxNflixDrmStatus_e *peNflixDrmStatus)
{
	opai_output_hdmi_checkCacheInited();
	HDMI_CHECK_GET_INIT;

	ENTER_OUTPUT_HDMI_CACHE;
	*peNflixDrmStatus = s_stOapiOutputHdmiCache.eNflixDrmStatus;
	LEAVE_OUTPUT_HDMI_CACHE;

    HxLOG_Debug("%s(%d)\n", __FUNCTION__, *peNflixDrmStatus);

	return ERR_OK;
}

HERROR OAPI_OUTPUT_HDMI_SetNetflixDrmStatus(DxNflixDrmStatus_e eNflixDrmStatus)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, eNflixDrmStatus);

	opai_output_hdmi_checkCacheInited();
	HDMI_CHECK_SET_INIT;

	ENTER_OUTPUT_HDMI_CACHE;
	s_stOapiOutputHdmiCache.eNflixDrmStatus = eNflixDrmStatus;
	s_nReqId = HLIB_RPC_Call(s_nRpcHandle, RPC_OAPI_OUTPUT_HDMI_SetNflixDrmStatus, "i", eNflixDrmStatus);
	LEAVE_OUTPUT_HDMI_CACHE;

	return ERR_OK;
}


