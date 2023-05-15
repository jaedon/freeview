/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  om_output_audio.c
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
#include <obama_rpcprotocol.h>
#include <oapi.h>

#include <octo_common.h>

#include <db_param.h>
#include <svc_pipe.h>
#include <svc_output.h>
#include <svc_av.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_output.h>



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
STATIC OxOutputAudio_Cache_t	s_stOmOutputAudioCache;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/


#define ___LOCAL_FUNCTIONS___

#define _____OAPI_CALLBACK_FUNCTIONS_____
STATIC void om_output_audio_CheckStatusChange(void)
{
	HERROR					hErr;
	HBOOL					bMasterMute;
	HUINT32					ulVolume, ulDecoderVolume;
	HUINT32					ulLipSyncDelaymsec;
	DxDigitalAudioFormat_e	eHdmiAudioFormat;
	DxDigitalAudioFormat_e	eSpdifAudioFormat;
	HBOOL					bAudioDescription;
	DxStereoSelect_e		eStereoSelect;
	DxDualMonoSelect_e		eDualMonoSelect;
	HBOOL					bChanged = FALSE;

	bMasterMute = SVC_OUT_GetMasterAudioMute();
	if(bMasterMute != s_stOmOutputAudioCache.bMasterMute)
	{
		HxLOG_Debug("\n\n\n\t\t\t %s(bMasterMute %d => %d)\n\n\n", __FUNCTION__, s_stOmOutputAudioCache.bMasterMute, bMasterMute);
		bChanged = TRUE;
		s_stOmOutputAudioCache.bMasterMute = bMasterMute;
	}

	SVC_OUT_GetVolume(&ulVolume);
	if(ulVolume != s_stOmOutputAudioCache.ulVolume)
	{
		HxLOG_Debug("\n\n\n\t\t\t %s(ulVolume %d => %d)\n\n\n", __FUNCTION__, s_stOmOutputAudioCache.ulVolume, ulVolume);
		bChanged = TRUE;
		s_stOmOutputAudioCache.ulVolume = ulVolume;
	}

	SVC_OUT_GetDecoderVolume(&ulDecoderVolume);
	if(ulDecoderVolume != s_stOmOutputAudioCache.ulDecoderVolume)
	{
		HxLOG_Debug("\n\n\n\t\t\t %s(ulVolume %d => %d)\n\n\n", __FUNCTION__, s_stOmOutputAudioCache.ulDecoderVolume, ulDecoderVolume);
		bChanged = TRUE;
		s_stOmOutputAudioCache.ulDecoderVolume = ulDecoderVolume;
	}

	SVC_AV_GetAudioLipSyncDelay(&ulLipSyncDelaymsec);
	if(ulLipSyncDelaymsec != s_stOmOutputAudioCache.ulLipSyncDelaymsec)
	{
		HxLOG_Debug("\n\n\n\t\t\t %s(ulLipSyncDelaymsec %d => %d)\n\n\n", __FUNCTION__, s_stOmOutputAudioCache.ulLipSyncDelaymsec, ulLipSyncDelaymsec);
		bChanged = TRUE;
		s_stOmOutputAudioCache.ulLipSyncDelaymsec = ulLipSyncDelaymsec;
	}

	SVC_OUT_GetHdmiAudioFormat(&eHdmiAudioFormat);
	if(eHdmiAudioFormat != s_stOmOutputAudioCache.eHdmiAudioFormat)
	{
		HxLOG_Debug("\n\n\n\t\t\t %s(eHdmiAudioFormat %d => %d)\n\n\n", __FUNCTION__, s_stOmOutputAudioCache.eHdmiAudioFormat, eHdmiAudioFormat);
		bChanged = TRUE;
		s_stOmOutputAudioCache.eHdmiAudioFormat = eHdmiAudioFormat;
	}

	SVC_OUT_GetSpdifAudioFormat(&eSpdifAudioFormat);
	if(eSpdifAudioFormat != s_stOmOutputAudioCache.eSpdifAudioFormat)
	{
		HxLOG_Debug("\n\n\n\t\t\t %s(eSpdifAudioFormat %d => %d)\n\n\n", __FUNCTION__, s_stOmOutputAudioCache.eSpdifAudioFormat, eSpdifAudioFormat);
		bChanged = TRUE;
		s_stOmOutputAudioCache.eSpdifAudioFormat = eSpdifAudioFormat;
	}

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_AUDIO_DESCRIPTION, (HUINT32 *)&bAudioDescription, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_AUDIO_DESCRIPTION) Error!!!\n");
	}
	if(bAudioDescription != s_stOmOutputAudioCache.bAudioDescription)
	{
		HxLOG_Debug("\n\n\n\t\t\t %s(bAudioDescription %d => %d)\n\n\n", __FUNCTION__, s_stOmOutputAudioCache.bAudioDescription, bAudioDescription);
		bChanged = TRUE;
		s_stOmOutputAudioCache.bAudioDescription = bAudioDescription;
	}

	SVC_AV_GetStereoSelect(MGR_ACTION_GetMainActionId(), &eStereoSelect);
	if(eStereoSelect != s_stOmOutputAudioCache.eStereoSelect)
	{
		HxLOG_Debug("\n\n\n\t\t\t %s(eStereoSelect %d => %d)\n\n\n", __FUNCTION__, s_stOmOutputAudioCache.eStereoSelect, eStereoSelect);
		bChanged = TRUE;
		s_stOmOutputAudioCache.eStereoSelect = eStereoSelect;
	}

	SVC_AV_GetDualMonoSelect(MGR_ACTION_GetMainActionId(), &eDualMonoSelect);
	if(eDualMonoSelect != s_stOmOutputAudioCache.eDualMonoSelect)
	{
		HxLOG_Debug("\n\n\n\t\t\t %s(eDualMonoSelect %d => %d)\n\n\n", __FUNCTION__, s_stOmOutputAudioCache.eDualMonoSelect, eDualMonoSelect);
		bChanged = TRUE;
		s_stOmOutputAudioCache.eDualMonoSelect = eDualMonoSelect;
	}

	if(bChanged == TRUE)
	{
		HxLOG_Debug("\n\n\n\t\t\t %s(Notify with reqID:0x%x)\n\n\n", __FUNCTION__, s_nReqId);
		HLIB_RPC_NotifyWithRequestId(s_nRpcHandle, RPC_OBAMA_OUTPUT_AUDIO_onStatusChanged, s_nReqId, "b", &s_stOmOutputAudioCache, sizeof(OxOutputAudio_Cache_t));
	}
}

#define _____OAPI_COMMAND_FUNCTIONS_____

STATIC HERROR om_output_audio_Init(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HxLOG_Debug("\t%s(reqID:0x%x)\n", __FUNCTION__, nRequestId);
	HLIB_RPC_Response(s_nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", &s_stOmOutputAudioCache, sizeof(OxOutputAudio_Cache_t));

	return ERR_OK;
}

STATIC HERROR om_output_audio_SetMasterMute(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetMasterAudioMute(HxOBJECT_INT(apArgv[0]));
	om_output_audio_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_audio_SetVolume(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetVolume(HxOBJECT_INT(apArgv[0]));
	DB_PARAM_SetItem(eDB_PARAM_ITEM_LAST_VOLUME, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_audio_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_audio_SetDecoderVolume(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetDecoderVolume(HxOBJECT_INT(apArgv[0]));
	DB_PARAM_SetItem(eDB_PARAM_ITEM_LAST_DECODER_VOLUME, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_audio_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_audio_SetLipSyncDelay(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_AV_SetAudioLipSyncDelay(HxOBJECT_INT(apArgv[0]));
	DB_PARAM_SetItem(eDB_PARAM_ITEM_LIPSYNC_DELAY, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_audio_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_audio_SetHdmiFormat(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetHdmiAudioFormat(HxOBJECT_INT(apArgv[0]));
	DB_PARAM_SetItem(eDB_PARAM_ITEM_HDMI_AUDIO_FORMAT, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_audio_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_audio_SetSpdifFormat(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetSpdifAudioFormat(HxOBJECT_INT(apArgv[0]));
	DB_PARAM_SetItem(eDB_PARAM_ITEM_SPDIF_AUDIO_FORMAT, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_audio_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_audio_SetAudioDescription(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	DB_PARAM_SetItem(eDB_PARAM_ITEM_AUDIO_DESCRIPTION, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_audio_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_audio_SetStereoSelect(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_AV_SetStereoSelect(MGR_ACTION_GetMainActionId(), HxOBJECT_INT(apArgv[0]));
	DB_PARAM_SetItem(eDB_PARAM_ITEM_STEREO_SELECT, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_audio_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_audio_SetDualMonoSelect(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR			hErr;

	s_nReqId = nRequestId;

	hErr = SVC_AV_SetDualMonoSelect(MGR_ACTION_GetMainActionId(), HxOBJECT_INT(apArgv[0]));
	if (ERR_OK != hErr)
	{
		HxLOG_Error("\tom_output_audio_SetDualMonoSelect ERROR !!! ERROR !!! ERROR !!! ERROR !!!\n");
	}

	DB_PARAM_SetItem(eDB_PARAM_ITEM_DUALMONO_SELECT, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_audio_CheckStatusChange();

	return ERR_OK;
}


#define _____PROCEDURE_____
STATIC BUS_Result_t proc_om_output_audio(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch (message)
	{
		case eMEVT_BUS_CREATE:
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			break;

		case eSEVT_OUT_CHECK_AUDIO:
			HxLOG_Debug("%s(eSEVT_OUT_CHECK_AUDIO)\n", __FUNCTION__);
			om_output_audio_CheckStatusChange();
			break;

		default:
			break;
	}

	return MESSAGE_PASS;
}

#define ___GLOBAL_FUNCTIONS___
HERROR OM_OUTPUT_AUDIO_Init(void)
{
	HERROR			err;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	err = OBAMA_RpcProtocol_GetRpcHandle(&s_nRpcHandle);
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RpcProtocol_GetRpcHandle() ERROR !!! ERROR !!! ERROR !!! ERROR !!!\n");
	}

	s_stOmOutputAudioCache.bSupportSpdif = MGR_OUTPUT_SupportSpdif();

	// Initialize OAPI cache data
	SVC_OUT_GetVolume(&s_stOmOutputAudioCache.ulVolume);
	SVC_OUT_GetDecoderVolume(&s_stOmOutputAudioCache.ulDecoderVolume);
	SVC_OUT_GetVolumeRange(&s_stOmOutputAudioCache.ulMaxVolume);
	SVC_AV_GetAudioLipSyncDelay(&s_stOmOutputAudioCache.ulLipSyncDelaymsec);
	SVC_OUT_GetHdmiAudioFormat(&s_stOmOutputAudioCache.eHdmiAudioFormat);
	SVC_OUT_GetSpdifAudioFormat(&s_stOmOutputAudioCache.eSpdifAudioFormat);
	err = DB_PARAM_GetItem(eDB_PARAM_ITEM_AUDIO_DESCRIPTION, (HUINT32 *)&s_stOmOutputAudioCache.bAudioDescription, 0);
	if (ERR_OK != err)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_AUDIO_DESCRIPTION) Error!!!\n");
	}

	err = DB_PARAM_GetItem(eDB_PARAM_ITEM_STEREO_SELECT, (HUINT32 *)&s_stOmOutputAudioCache.eStereoSelect, 0);
	if (ERR_OK != err)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_STEREO_SELECT) Error!!!\n");
	}

	err = DB_PARAM_GetItem(eDB_PARAM_ITEM_DUALMONO_SELECT, (HUINT32 *)&s_stOmOutputAudioCache.eDualMonoSelect, 0);
	if (ERR_OK != err)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_DUALMONO_SELECT) Error!!!\n");
	}

	//SVC_AV_GetStereoSelect(MGR_ACTION_GetMainActionId(), &s_stOmOutputAudioCache.eStereoSelect);
	//SVC_AV_GetDualMonoSelect(MGR_ACTION_GetMainActionId(), &s_stOmOutputAudioCache.eDualMonoSelect);

	// Create event handler proc
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_output_audio, 0, 0, 0, 0);

	//	-------   RPC Methods   -------	//
	err = HLIB_RPC_RegisterRequest(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_Init, NULL, om_output_audio_Init, NULL,
							"Activate the output.audio module: (void)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tHLIB_RPC_RegisterRequest(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_AUDIO_Init, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetMasterMute, "i", om_output_audio_SetMasterMute, NULL,
							"Set audio master mute: (HBOOL bMute)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tHLIB_RPC_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_AUDIO_SetMasterMute, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetVolume, "i", om_output_audio_SetVolume, NULL,
							"Set audio volume: (HUINT32 ulVolume)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tHLIB_RPC_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_AUDIO_SetVolume, err);
	}
	
	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetDecoderVolume, "i", om_output_audio_SetDecoderVolume, NULL,
							"Set audio decoder volume: (HUINT32 ulVolume)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tHLIB_RPC_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_AUDIO_SetDecoderVolume, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetLipSyncDelay, "i", om_output_audio_SetLipSyncDelay, NULL,
							"Set audio lip sync delay in msec: (HUINT32 *pulmsec)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tHLIB_RPC_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_AUDIO_SetLipSyncDelay, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetHdmiFormat, "i", om_output_audio_SetHdmiFormat, NULL,
							"Set HDMI audio format: (DxDigitalAudioFormat_e eDigitalAudioFormat)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tHLIB_RPC_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_AUDIO_SetHdmiFormat, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetSpdifFormat, "i", om_output_audio_SetSpdifFormat, NULL,
							"Set SPDIF audio format: (DxDigitalAudioFormat_e eDigitalAudioFormat)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tHLIB_RPC_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_AUDIO_SetSpdifFormat, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetAudioDescription, "i", om_output_audio_SetAudioDescription, NULL,
							"Set audio description: (HBOOL *pbEnable)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tHLIB_RPC_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_AUDIO_SetAudioDescription, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetStereoSelect, "i", om_output_audio_SetStereoSelect, NULL,
							"Set audio stereo select: (DxStereoSelect_e eStereoSelect)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tHLIB_RPC_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_AUDIO_SetStereoSelect, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_AUDIO_SetDualMonoSelect, "i", om_output_audio_SetDualMonoSelect, NULL,
							"Set audio stereo select: (DxDualMonoSelect_e eDualMonoSelect)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_AUDIO_SetDualMonoSelect, err);
	}


	//	-------   Notifier  Callbakcs------- //
	err = HLIB_RPC_CreateNotification(s_nRpcHandle, RPC_OBAMA_OUTPUT_AUDIO_onStatusChanged, "b",
							"Audio output parameters and status changed.\n"
							"\t   - (OxOutputAudio_Cache_t *pstOutputAudioStatus)\n"
							);
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterCallback(%s) returned error(%d)\n", RPC_OBAMA_OUTPUT_AUDIO_onStatusChanged, err);
	}

	return ERR_OK;
}



