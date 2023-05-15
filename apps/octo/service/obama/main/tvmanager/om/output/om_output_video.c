/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_output_video.c
	@brief

	Description:  									\n
	Module: TVMANAGER / OAPI Manager				\n

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
#include <obama_rpcprotocol.h>
#include <oapi.h>
#include <octo_common.h>

#include <db_param.h>

#include <svc_output.h>
#include <svc_pipe.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_output.h>

#include <get_enum_str.h>

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
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HINT32					s_nReqId;
STATIC HINT32					s_nRpcHandle;
STATIC OxOutputVideo_Cache_t	s_stOmOutputVideoCache;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/


#define ___LOCAL_FUNCTIONS___

#define _____OAPI_CALLBACK_FUNCTIONS_____
STATIC void om_output_video_CheckStatusChange(void)
{
	HBOOL					bAvEnable;
	OutputStatus_t			stOutputStatus;
	DxScartFormat_e			eTvScartFormat, eVcrScartFormat;
	DxScartPath_e			eScartPath;
	DxResolutionSelect_e	eResolutionSelect;
	DxAspectRatio_e			eTvAspectRatio;
	DxDfcSelect_e			eDfcSelect;
	HUINT8					ucBgColor_R, ucBgColor_G, ucBgColor_B;
	HBOOL					bChanged = FALSE;

	SVC_OUT_GetAvEnable(&bAvEnable);
	if(bAvEnable != s_stOmOutputVideoCache.bAvEnable)
	{
		bChanged = TRUE;
		s_stOmOutputVideoCache.bAvEnable = bAvEnable;
	}

	SVC_OUT_GetTvScartFormat(&eTvScartFormat);
	if(s_stOmOutputVideoCache.eTvScartFormat != eTvScartFormat)
	{
		bChanged = TRUE;
		s_stOmOutputVideoCache.eTvScartFormat = eTvScartFormat;
	}

	SVC_OUT_GetVcrScartFormat(&eVcrScartFormat);
	if(s_stOmOutputVideoCache.eVcrScartFormat != eVcrScartFormat)
	{
		bChanged = TRUE;
		s_stOmOutputVideoCache.eVcrScartFormat = eVcrScartFormat;
	}

	SVC_OUT_GetScartPath(&eScartPath);
	if(s_stOmOutputVideoCache.eScartPath != eScartPath)
	{
		bChanged = TRUE;
		s_stOmOutputVideoCache.eScartPath = eScartPath;
	}

	SVC_OUT_GetResolutionSelect(&eResolutionSelect);
	if(s_stOmOutputVideoCache.eResolutionSelect != eResolutionSelect)
	{
		bChanged = TRUE;
		s_stOmOutputVideoCache.eResolutionSelect = eResolutionSelect;
	}

	SVC_OUT_GetTvAspectRatio(&eTvAspectRatio);
	if(s_stOmOutputVideoCache.eTvAspectRatio != eTvAspectRatio)
	{
		bChanged = TRUE;
		s_stOmOutputVideoCache.eTvAspectRatio = eTvAspectRatio;
	}

	SVC_OUT_GetDfcSelect(&eDfcSelect);
	if(s_stOmOutputVideoCache.eDfcSelect != eDfcSelect)
	{
		bChanged = TRUE;
		s_stOmOutputVideoCache.eDfcSelect = eDfcSelect;
	}

	SVC_OUT_GetBackgroundColor(&ucBgColor_R, &ucBgColor_G, &ucBgColor_B);

	if(s_stOmOutputVideoCache.ucBgColor_R != ucBgColor_R)
	{
		bChanged = TRUE;
		s_stOmOutputVideoCache.ucBgColor_R = ucBgColor_R;
	}

	if(s_stOmOutputVideoCache.ucBgColor_G != ucBgColor_G)
	{
		bChanged = TRUE;
		s_stOmOutputVideoCache.ucBgColor_G = ucBgColor_G;
	}

	if(s_stOmOutputVideoCache.ucBgColor_B != ucBgColor_B)
	{
		bChanged = TRUE;
		s_stOmOutputVideoCache.ucBgColor_B = ucBgColor_B;
	}

	SVC_OUT_GetOutputStatus(&stOutputStatus);

	if(s_stOmOutputVideoCache.eResolutionStatus != stOutputStatus.stOutputTiming.eResolution)
	{
		bChanged = TRUE;
		s_stOmOutputVideoCache.eResolutionStatus = stOutputStatus.stOutputTiming.eResolution;
	}

	if(s_stOmOutputVideoCache.ePictureRate != stOutputStatus.stOutputTiming.ePictureRate)
	{
		bChanged = TRUE;
		s_stOmOutputVideoCache.ePictureRate = stOutputStatus.stOutputTiming.ePictureRate;
	}

	if(s_stOmOutputVideoCache.ulDfcStatus != stOutputStatus.ulDfcStatus)
	{
		bChanged = TRUE;
		s_stOmOutputVideoCache.ulDfcStatus = stOutputStatus.ulDfcStatus;
	}

	if(s_stOmOutputVideoCache.eVideoBlankCopyRight!= (DxCopyRight_e)stOutputStatus.eVideoBlankCopyRight)
	{
		bChanged = TRUE;
		s_stOmOutputVideoCache.eVideoBlankCopyRight = (DxCopyRight_e)stOutputStatus.eVideoBlankCopyRight;
	}

	if(bChanged == TRUE)
	{ // 변경 사항이 생기면 oapi에 notify 한다.
		HLIB_RPC_NotifyWithRequestId(s_nRpcHandle, RPC_OBAMA_OUTPUT_VIDEO_onStatusChanged, s_nReqId, "b", &s_stOmOutputVideoCache, sizeof(OxOutputVideo_Cache_t));
	}
}

#define _____OAPI_COMMAND_FUNCTIONS_____

STATIC HERROR om_output_video_Init(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HxLOG_Debug("\t%s(reqID:0x%x)\n", __FUNCTION__, nRequestId);
	HLIB_RPC_Response(s_nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", &s_stOmOutputVideoCache, sizeof(OxOutputVideo_Cache_t));

	return ERR_OK;
}

STATIC HERROR om_output_video_SetTvScartFormat(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetTvScartFormat(HxOBJECT_INT(apArgv[0]));
	DB_PARAM_SetItem(eDB_PARAM_ITEM_TVSCART_FORMAT, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_video_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_video_SetVcrScartFormat(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetVcrScartFormat(HxOBJECT_INT(apArgv[0]));
	DB_PARAM_SetItem(eDB_PARAM_ITEM_VCRSCART_FORMAT, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_video_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_video_SetScartPath(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetScartPath(HxOBJECT_INT(apArgv[0]));
	DB_PARAM_SetItem(eDB_PARAM_ITEM_SCART_PATH, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_video_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_video_SetResolutionSelect(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetResolutionSelect(HxOBJECT_INT(apArgv[0]), TRUE);
	DB_PARAM_SetItem(eDB_PARAM_ITEM_RESOLUTION_SELECT, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_video_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_video_SetTvAspectRatio(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetTvAspectRatio(HxOBJECT_INT(apArgv[0]), TRUE);
	DB_PARAM_SetItem(eDB_PARAM_ITEM_TV_ASPECTRATIO, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_video_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_video_SetDfcSelect(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetDfcSelect(HxOBJECT_INT(apArgv[0]), TRUE);
	DB_PARAM_SetItem(eDB_PARAM_ITEM_DFC_SELECT, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_video_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_video_SetBackGroundColor(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetBackgroundColor(HxOBJECT_INT(apArgv[0]), HxOBJECT_INT(apArgv[1]), HxOBJECT_INT(apArgv[2]));
	DB_PARAM_SetItem(eDB_PARAM_ITEM_BACKGROUND_COLOR_R, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);
	DB_PARAM_SetItem(eDB_PARAM_ITEM_BACKGROUND_COLOR_G, (HUINT32)HxOBJECT_INT(apArgv[1]), 0);
	DB_PARAM_SetItem(eDB_PARAM_ITEM_BACKGROUND_COLOR_B, (HUINT32)HxOBJECT_INT(apArgv[2]), 0);

	om_output_video_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_video_SetSupportVideoFrame(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetSupportScalerByVideoFrame(HxOBJECT_INT(apArgv[0]));

	return ERR_OK;
}

STATIC HERROR om_output_video_SetAvEnable(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetAvEnable(HxOBJECT_INT(apArgv[0]));
	om_output_video_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_video_SetVideoBlankCopyRight(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32 		ulActionId;
	DxCopyRight_e	CopyRight;

	s_nReqId = nRequestId;
	CopyRight = (DxCopyRight_e)HxOBJECT_INT(apArgv[0]);
	ulActionId = MGR_ACTION_GetMainActionId();

	SVC_OUT_SetVideoBlankCopyRight(ulActionId,CopyRight);
	om_output_video_CheckStatusChange();
	HxLOG_Debug("DxCopyRight_e: %d \n",CopyRight);

	return ERR_OK;
}


#if 0
STATIC HERROR om_output_video_SetCompositeStandard(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetCompositeStandard(HxOBJECT_INT(apArgv[0]));
	om_output_video_CheckStatusChange();

	return ERR_OK;
}
#endif

#define _____PROCEDURE_____
STATIC BUS_Result_t proc_om_output_video(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch (message)
	{
		case eMEVT_BUS_CREATE:
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			break;

		case eSEVT_OUT_CHECK_VIDEO:
			HxLOG_Debug("%s(eSEVT_OUT_CHECK_VIDEO)\n", __FUNCTION__);
			om_output_video_CheckStatusChange();

			break;

		default:
			break;
	}

	return MESSAGE_PASS;
}

#if defined(CONFIG_DEBUG)
STATIC void om_output_video_PrintStatus(void)
{
	HLIB_CMD_Printf("================== OM_OUTPUT_VIDEO STATUS =================\n");

	HLIB_CMD_Printf("\tbSupportComposite : %d\n", s_stOmOutputVideoCache.bSupportComposite);
	HLIB_CMD_Printf("\tbSupportTvScart : %d\n", s_stOmOutputVideoCache.bSupportTvScart);
	HLIB_CMD_Printf("\tbSupportVcrScart : %d\n", s_stOmOutputVideoCache.bSupportVcrScart);
	HLIB_CMD_Printf("\tulSupportTvScartFormat : 0x%x\n", s_stOmOutputVideoCache.ulSupportTvScartFormat);
	HLIB_CMD_Printf("\tulSupportVcrScartFormat : 0x%x\n", s_stOmOutputVideoCache.ulSupportVcrScartFormat);
	HLIB_CMD_Printf("\tbAvEnable : %d\n", s_stOmOutputVideoCache.bAvEnable);
	HLIB_CMD_Printf("\teTvScartFormat : %s\n", OTL_ENUM2STR_ScartFormat(s_stOmOutputVideoCache.eTvScartFormat));
	HLIB_CMD_Printf("\teVcrScartFormat : %s\n", OTL_ENUM2STR_ScartFormat(s_stOmOutputVideoCache.eVcrScartFormat));
	HLIB_CMD_Printf("\teScartPath : %s\n", OTL_ENUM2STR_ScartPath(s_stOmOutputVideoCache.eScartPath));
	HLIB_CMD_Printf("\teResolutionSelect : %s\n", OTL_ENUM2STR_ResolutionSelect(s_stOmOutputVideoCache.eResolutionSelect));
	HLIB_CMD_Printf("\teTvAspectRatio : %s\n", OTL_ENUM2STR_AspectRatio(s_stOmOutputVideoCache.eTvAspectRatio));
	HLIB_CMD_Printf("\teDfcSelect : %s\n", OTL_ENUM2STR_DfcSelect(s_stOmOutputVideoCache.eDfcSelect));
	HLIB_CMD_Printf("\tBG color : %d/%d/%d\n", s_stOmOutputVideoCache.ucBgColor_R, s_stOmOutputVideoCache.ucBgColor_G, s_stOmOutputVideoCache.ucBgColor_B);
	HLIB_CMD_Printf("\teResolutionStatus : %s\n", OTL_ENUM2STR_ResolutionStatus(s_stOmOutputVideoCache.eResolutionStatus));
	HLIB_CMD_Printf("\tePictureRate : %s\n", OTL_ENUM2STR_VideoPictureRate(s_stOmOutputVideoCache.ePictureRate));
	HLIB_CMD_Printf("\tulDfcStatus : 0x%x\n", s_stOmOutputVideoCache.ulDfcStatus);
	HLIB_CMD_Printf("\teCompositeStandard : %s\n", OTL_ENUM2STR_CompositeStandard(s_stOmOutputVideoCache.eCompositeStandard));

	HLIB_CMD_Printf("===========================================================\n");
}

STATIC int om_output_video_ProcessCommand_status(void *szArgList)
{
	om_output_video_PrintStatus();

	return HxCMD_OK;
}

STATIC void om_output_video_RegisterCommand(void)
{
#define	hCmdHandle		"om_output_video"

	HLIB_CMD_RegisterWord(hCmdHandle,
						om_output_video_ProcessCommand_status,
						"status",
						"Print om_output_video status.",
						"status");
}
#endif

#define ___GLOBAL_FUNCTIONS___
HERROR OM_OUTPUT_VIDEO_Init(void)
{
	HERROR			err;
	OutputStatus_t	stOutputStatus;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	err = OBAMA_RpcProtocol_GetRpcHandle(&s_nRpcHandle);
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RpcProtocol_GetRpcHandle() ERROR !!! ERROR !!! ERROR !!! ERROR !!!\n");
	}

// Initialize OAPI cache data

#if defined(CONFIG_MW_OUTPUT_COMPOSITE)
	s_stOmOutputVideoCache.bSupportComposite = TRUE;

	#if defined(CONFIG_MW_OUTPUT_SCART)
		s_stOmOutputVideoCache.bSupportTvScart = MGR_OUTPUT_SupportTvScart();
		s_stOmOutputVideoCache.bSupportVcrScart = MGR_OUTPUT_SupportVcrScart();
		s_stOmOutputVideoCache.ulSupportTvScartFormat = MGR_OUTPUT_GetSupportTvScartFormat();
		s_stOmOutputVideoCache.ulSupportVcrScartFormat = MGR_OUTPUT_GetSupportVcrScartFormat();
	#else
		s_stOmOutputVideoCache.bSupportTvScart = FALSE;
		s_stOmOutputVideoCache.bSupportVcrScart = FALSE;
		s_stOmOutputVideoCache.ulSupportTvScartFormat = eDxSCART_FORMAT_UNKNOWN;
		s_stOmOutputVideoCache.ulSupportVcrScartFormat = eDxSCART_FORMAT_UNKNOWN;
	#endif
#else
	s_stOmOutputVideoCache.bSupportComposite = FALSE;
	s_stOmOutputVideoCache.bSupportTvScart = FALSE;
	s_stOmOutputVideoCache.bSupportVcrScart = FALSE;
	s_stOmOutputVideoCache.ulSupportTvScartFormat = eDxSCART_FORMAT_UNKNOWN;
	s_stOmOutputVideoCache.ulSupportVcrScartFormat = eDxSCART_FORMAT_UNKNOWN;
#endif

	SVC_OUT_GetAvEnable(&s_stOmOutputVideoCache.bAvEnable);
	SVC_OUT_GetTvScartFormat(&s_stOmOutputVideoCache.eTvScartFormat);
	SVC_OUT_GetVcrScartFormat(&s_stOmOutputVideoCache.eVcrScartFormat);
	SVC_OUT_GetScartPath(&s_stOmOutputVideoCache.eScartPath);

	SVC_OUT_GetResolutionSelect(&s_stOmOutputVideoCache.eResolutionSelect);
	SVC_OUT_GetTvAspectRatio(&s_stOmOutputVideoCache.eTvAspectRatio);
	SVC_OUT_GetDfcSelect(&s_stOmOutputVideoCache.eDfcSelect);

	SVC_OUT_GetBackgroundColor(&s_stOmOutputVideoCache.ucBgColor_R,
							&s_stOmOutputVideoCache.ucBgColor_G,
							&s_stOmOutputVideoCache.ucBgColor_B);

	SVC_OUT_GetCompositeStandard(&s_stOmOutputVideoCache.eCompositeStandard);

	SVC_OUT_GetOutputStatus(&stOutputStatus);
	s_stOmOutputVideoCache.eResolutionStatus = stOutputStatus.stOutputTiming.eResolution;
	s_stOmOutputVideoCache.ePictureRate = stOutputStatus.stOutputTiming.ePictureRate;
	s_stOmOutputVideoCache.ulDfcStatus = stOutputStatus.ulDfcStatus;
	s_stOmOutputVideoCache.eVideoBlankCopyRight	= stOutputStatus.eVideoBlankCopyRight;

	// Create event handler proc
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_output_video, 0, 0, 0, 0);

	//	-------   RPC Methods   -------	//
	err = HLIB_RPC_RegisterRequest(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_Init, NULL, om_output_video_Init, NULL,
							"Activate the output.video module: (void)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tHLIB_RPC_RegisterRequest(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_VIDEO_Init, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetTvScartFormat, "i", om_output_video_SetTvScartFormat, NULL,
							"Set TV scart format : (DxScartFormat_e eScartFormat)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_VIDEO_SetTvScartFormat, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetVcrScartFormat, "i", om_output_video_SetVcrScartFormat, NULL,
							"Set VCR scart format : (DxScartFormat_e eScartFormat)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_VIDEO_SetVcrScartFormat, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetScartPath, "i", om_output_video_SetScartPath, NULL,
							"Set scart path : (DxScartPath_e eScartPath)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_VIDEO_SetScartPath, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetResolutionSelect, "i", om_output_video_SetResolutionSelect, NULL,
							"Set resolution select option : (DxResolutionSelect_e eResolutionSelect)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_VIDEO_SetResolutionSelect, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetTvAspectRatio, "i", om_output_video_SetTvAspectRatio, NULL,
							"Set TV aspect ratio option : (DxAspectRatio_e eTvAspectRatio)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_VIDEO_SetTvAspectRatio, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetDfcSelect, "i", om_output_video_SetDfcSelect, NULL,
							"Set display format conversion option : (DxDfcSelect_e eDfcSelect)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_VIDEO_SetDfcSelect, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetBackGroundColor, "iii", om_output_video_SetBackGroundColor, NULL,
							"Set video back ground color : (HUINT8 ucR, HUINT8 ucG, HUINT8 ucB)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_VIDEO_SetBackGroundColor, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetAvEnable, "i", om_output_video_SetAvEnable, NULL,
							"Set STB AV output on/off : (HBOOL bEnable)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_VIDEO_SetAvEnable, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SetVideoBlankCopyRight, "i", om_output_video_SetVideoBlankCopyRight, NULL,
							"Set STB Video Blank CopyRight : (DxCopyRight_e copyright)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_VIDEO_SetVideoBlankCopyRight, err);
	}


	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_VIDEO_SupportVideoFrame, "i", om_output_video_SetSupportVideoFrame, NULL,
							"Support video Frame : (HBOOL bEnable)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_VIDEO_SupportVideoFrame, err);
	}


	//	-------   Notifier  Callbakcs------- //
	err = HLIB_RPC_CreateNotification(s_nRpcHandle, RPC_OBAMA_OUTPUT_VIDEO_onStatusChanged, "b",
							"Video output parameters and status changed.\n"
							"\t   - (OxOutputVideo_Cache_t *pstOutputVideoStatus)\n"
							);
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterCallback(%s) returned error(%d)\n", RPC_OBAMA_OUTPUT_VIDEO_onStatusChanged, err);
	}

#if defined(CONFIG_DEBUG)
	om_output_video_RegisterCommand();
#endif

	return ERR_OK;
}



