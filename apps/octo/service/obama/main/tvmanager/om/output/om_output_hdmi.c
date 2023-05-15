/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_oapi_output_hdmi.c
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

#include <db_common.h>
#include <db_param.h>

#include <svc_pipe.h>
#include <svc_output.h>
#include <svc_cec.h>
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
STATIC OxOutputHdmi_Cache_t		s_stOmOutputHdmiCache;

/*******************************************************************/
/********************      Functions     ********************/
/*******************************************************************/


#define ___LOCAL_FUNCTIONS___
STATIC HBOOL om_output_hdmi_isUsedCecVolume(void)
{
  	HERROR				hErr;
	HBOOL				bEnable = FALSE;

	HxLOG_Trace();

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_VOLUME_ENABLE, (HUINT32 *)&bEnable, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_VOLUME_ENABLE) Error!!!\n");
	}

	if (bEnable == FALSE)
	{
		HxLOG_Print("[%s:%d] : CEC Volume control off state\n", __FUNCTION__, __LINE__);
		return FALSE;
	}

    return TRUE;
}

#define _____OAPI_CALLBACK_FUNCTIONS_____
STATIC void om_output_hdmi_CheckStatusChange(void)
{
	HERROR					err;
	OutputStatus_t			stOutputStatus;
	DxHdcpStatus_e			eHdcpStatus;
	DxHdmi3DSelect_e		eHdmi3DSelect;
	DxVideo3DFormat_b		ul3DFormatCapability;
	HBOOL					bCecEnable;
    HBOOL                   bVolumeControlOfCec;
	HUINT32					ulEdidVideoCount, i;
	VIDEO_Timing_t			*paEdidVideoTimingList;
	DxResolutionStatus_b	ulResolutionCapability;
	DxResolutionStatus_e	eResolution;
	DxCompositeStandard_e 	eCompStandard;
	HBOOL					bChanged = FALSE;
	HBOOL					bPlugIn;
    DxNflixDrmStatus_e      eNflixDrmStatus;
	DxHdcpVersion_e			eHdcpVer;

	SVC_OUT_GetHdmiPlugStatus(&bPlugIn);
	if(bPlugIn != s_stOmOutputHdmiCache.bPlugIn)
	{
		bChanged = TRUE;
		s_stOmOutputHdmiCache.bPlugIn = bPlugIn;
	}

	SVC_OUT_GetHdcpStatus(&eHdcpStatus);
	if(eHdcpStatus != s_stOmOutputHdmiCache.eHdcpStatus)
	{
		bChanged = TRUE;
		s_stOmOutputHdmiCache.eHdcpStatus = eHdcpStatus;
	}

	SVC_OUT_GetNetflixDrmStatus(&eNflixDrmStatus);
	if(eNflixDrmStatus != s_stOmOutputHdmiCache.eNflixDrmStatus)
	{
		bChanged = TRUE;
		s_stOmOutputHdmiCache.eNflixDrmStatus = eNflixDrmStatus;
	}

	SVC_OUT_GetHdcp_Version(&eHdcpVer);
	if(eHdcpVer != s_stOmOutputHdmiCache.eHdcpVer)
	{
		bChanged = TRUE;
		s_stOmOutputHdmiCache.eHdcpVer = eHdcpVer;
	}

	SVC_OUT_GetHdmi3DSelect(&eHdmi3DSelect);
	if(eHdmi3DSelect != s_stOmOutputHdmiCache.eHdmi3DSelect)
	{
		bChanged = TRUE;
		s_stOmOutputHdmiCache.eHdmi3DSelect = eHdmi3DSelect;
	}

	SVC_OUT_GetHdmi3DFormatCapability(&ul3DFormatCapability);
	if(ul3DFormatCapability != s_stOmOutputHdmiCache.ul3DFormatCapability)
	{
		bChanged = TRUE;
		s_stOmOutputHdmiCache.ul3DFormatCapability = ul3DFormatCapability;
	}

	SVC_OUT_GetOutputStatus(&stOutputStatus);
	if(stOutputStatus.eHdmi3DStatus != s_stOmOutputHdmiCache.eHdmi3DStatus)
	{
		bChanged = TRUE;
		s_stOmOutputHdmiCache.eHdmi3DStatus = stOutputStatus.eHdmi3DStatus;
	}

	HxSTD_MemCopy(&s_stOmOutputHdmiCache.stHdmiEdidInfo, &stOutputStatus.stHdmiEdidInfo, sizeof(stOutputStatus.stHdmiEdidInfo));

	ulResolutionCapability = 0;
	err = SVC_OUT_GetSupportHdmiVideoTimingList(&ulEdidVideoCount, &paEdidVideoTimingList);
	if (err == ERR_OK)
	{
		SVC_OUT_GetCompositeStandard(&eCompStandard);

		for (i = 0; i < ulEdidVideoCount; i++)
		{
			eResolution = paEdidVideoTimingList[i].eResolution;

			if(eCompStandard == eDxCOMPOSITE_STANDARD_NTSC)
			{
				if( (eResolution == eDxRESOLUTION_STATUS_576I) || (eResolution == eDxRESOLUTION_STATUS_576P) )
				{
					continue;
				}
			}
			else
			{
				if( (eResolution == eDxRESOLUTION_STATUS_480I) || (eResolution == eDxRESOLUTION_STATUS_480P) )
				{
					continue;
				}
			}

			ulResolutionCapability |= eResolution;
		}
	}

	if(ulResolutionCapability != s_stOmOutputHdmiCache.ulResolutionCapability)
	{
		bChanged = TRUE;
		s_stOmOutputHdmiCache.ulResolutionCapability = ulResolutionCapability;
	}

//	bCecEnable = MWC_PARAM_GetHdmiCecEnable();
	err = DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_ENABLE, (HUINT32 *)&bCecEnable, 0);
	if (ERR_OK != err)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_ENABLE) Error!!!\n");
	}

	if(bCecEnable != s_stOmOutputHdmiCache.bCecEnable)
	{
		bChanged = TRUE;
		s_stOmOutputHdmiCache.bCecEnable = bCecEnable;
	}

    err = DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_VOLUME_ENABLE, (HUINT32 *)&bVolumeControlOfCec, 0);
	if (ERR_OK != err)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_VOLUME_ENABLE) Error!!!\n");
	}

	if(bVolumeControlOfCec != s_stOmOutputHdmiCache.bCecVolumeCtrlEnable)
	{
		bChanged = TRUE;
		s_stOmOutputHdmiCache.bCecVolumeCtrlEnable = bVolumeControlOfCec;
	}

	if(bChanged == TRUE)
	{
		HLIB_RPC_NotifyWithRequestId(s_nRpcHandle, RPC_OBAMA_OUTPUT_HDMI_onStatusChanged, s_nReqId, "b", &s_stOmOutputHdmiCache, sizeof(OxOutputHdmi_Cache_t));
	}
}

#define _____OAPI_COMMAND_FUNCTIONS_____

STATIC HERROR om_output_hdmi_Init(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HxLOG_Debug("\t%s(reqID:0x%x)\n", __FUNCTION__, nRequestId);
	HLIB_RPC_Response(s_nRpcHandle, nClientHandle, nRequestId, ERR_OK, "b", &s_stOmOutputHdmiCache, sizeof(OxOutputHdmi_Cache_t));

	return ERR_OK;
}

STATIC HERROR om_output_hdmi_Set3DSelect(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	s_nReqId = nRequestId;

	SVC_OUT_SetHdmi3DSelect(HxOBJECT_INT(apArgv[0]));
	DB_PARAM_SetItem(eDB_PARAM_ITEM_3D_SELECT, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);

	om_output_hdmi_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_hdmi_SetCecEnable(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR		hErr;

	s_nReqId = nRequestId;

	hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_HDMI_CEC_ENABLE, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_SetItem(eDB_PARAM_ITEM_HDMI_CEC_ENABLE) Error!!!\n");
	}

	om_output_hdmi_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_hdmi_SetCecVolumeEnable(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HERROR		hErr;

	s_nReqId = nRequestId;

	hErr = DB_PARAM_SetItem(eDB_PARAM_ITEM_HDMI_CEC_VOLUME_ENABLE, (HUINT32)HxOBJECT_INT(apArgv[0]), 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_SetItem(eDB_PARAM_ITEM_HDMI_CEC_ENABLE) Error!!!\n");
	}

	om_output_hdmi_CheckStatusChange();

	return ERR_OK;
}

STATIC HERROR om_output_hdmi_SetNetflixDrmStatus(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
	HUINT32             ulActionId;
	DxNflixDrmStatus_e	eNflixDrmStatus;

	eNflixDrmStatus = (DxNflixDrmStatus_e)HxOBJECT_INT(apArgv[0]);
	ulActionId = MGR_ACTION_GetMainActionId();
	HxLOG_Debug("NetflixDrmStatus:%d \n",eNflixDrmStatus);
	SVC_OUT_SetNetflixDrmStatus(ulActionId, eNflixDrmStatus);

	return ERR_OK;
}

STATIC HERROR om_output_hdmi_CecUserCtrlPressed(HINT32 nClientHandle, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserdata)
{
#if defined(CONFIG_MW_CEC)
    DxHdmiUserCtrlPressed_e eParam;
    eParam = (HUINT32)HxOBJECT_INT(apArgv[0]);
    switch(eParam)
    {
    case eDxHDMI_USER_CTRL_VOLUME_UP:
    case eDxHDMI_USER_CTRL_VOLUME_DOWN:
    case eDxHDMI_USER_CTRL_VOLUME_MUTE:
        if(om_output_hdmi_isUsedCecVolume()== TRUE)
        {

            SVC_CEC_UserCtrl_Pressed(eParam);

        }
        break;
    default:
        HxLOG_Warning("Not used CEC (user control pressed) parammeter  : 0x%x \n",eParam);
        break;
    }

#endif
	return ERR_OK;
}


#define _____PROCEDURE_____
STATIC BUS_Result_t proc_om_output_hdmi(HINT32 message, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch (message)
	{
		case eMEVT_BUS_CREATE:
			BUS_MGR_SetMgrName (BUS_MGR_GetThis(), (HINT8*) __FUNCTION__);
			break;

		case eSEVT_OUT_CHECK_HDMI:
			HxLOG_Debug("%s(eSEVT_OUT_CHECK_HDMI)\n", __FUNCTION__);
			om_output_hdmi_CheckStatusChange();
			SVC_OUT_CheckPriorityOutput();
			break;

		case eSEVT_OUT_CHECK_HDCP:
			HxLOG_Debug("%s(eSEVT_OUT_CHECK_HDCP)\n", __FUNCTION__);
			om_output_hdmi_CheckStatusChange();
			break;

		default:
			break;
	}

	return MESSAGE_PASS;
}

#if defined(CONFIG_DEBUG)
STATIC void om_output_hdmi_PrintStatus(void)
{
	HUINT32		i;

	HLIB_CMD_Printf("================== OM_OUTPUT_HDMI STATUS =================\n");
	HLIB_CMD_Printf("\tbSupportCec : %d\n", s_stOmOutputHdmiCache.bSupportCec);
	HLIB_CMD_Printf("\tbCecEnable : %d\n", s_stOmOutputHdmiCache.bCecEnable);
	HLIB_CMD_Printf("\teHdcpStatus : %s\n", OTL_ENUM2STR_HdcpStatus(s_stOmOutputHdmiCache.eHdcpStatus));
	HLIB_CMD_Printf("\teHdmi3DSelect : %s\n", OTL_ENUM2STR_Video3DSelect(s_stOmOutputHdmiCache.eHdmi3DSelect));
	HLIB_CMD_Printf("\tul3DFormatCapability\n");

	for(i = 0; i < 5; i++)
	{
		if((s_stOmOutputHdmiCache.ul3DFormatCapability & (1 << i)) != 0)
		{
			HLIB_CMD_Printf("\t\t%s\n", OTL_ENUM2STR_Video3DFormat(1 << i));
		}
	}

	HLIB_CMD_Printf("\teHdmi3DStatus : %s\n", OTL_ENUM2STR_Video3DFormat(s_stOmOutputHdmiCache.eHdmi3DStatus));
	HLIB_CMD_Printf("\tul3DFormatCapability\n");

	for(i = 0; i < 32; i++)
	{
		if((s_stOmOutputHdmiCache.ulResolutionCapability & (1 << i)) != 0)
		{
			HLIB_CMD_Printf("\t\t%s\n", OTL_ENUM2STR_ResolutionStatus(1 << i));
		}
	}
	HLIB_CMD_Printf("==========================================================\n");
}

STATIC int om_output_hdmi_ProcessCommand_status(void *szArgList)
{
	om_output_hdmi_PrintStatus();

	return HxCMD_OK;
}

STATIC void om_output_hdmi_RegisterCommand(void)
{
#define	hCmdHandle		"om_output_hdmi"

	HLIB_CMD_RegisterWord(hCmdHandle,
						om_output_hdmi_ProcessCommand_status,
						"status",
						"Print om_output_hdmi status.",
						"status");
}
#endif

#define ___GLOBAL_FUNCTIONS___
HERROR OM_OUTPUT_HDMI_Init(void)
{
	HERROR					err;
	OutputStatus_t			stOutputStatus;
	HUINT32					ulEdidVideoCount, i;
	VIDEO_Timing_t			*paEdidVideoTimingList;
	DxResolutionStatus_b	ulResolutionCapability;
	DxResolutionStatus_e	eResolution;
	DxCompositeStandard_e 	eCompStandard;

	HxLOG_Debug("%s()\n", __FUNCTION__);

	err = OBAMA_RpcProtocol_GetRpcHandle(&s_nRpcHandle);
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RpcProtocol_GetRpcHandle() ERROR !!! ERROR !!! ERROR !!! ERROR !!!\n");
	}

	// Initialize OAPI cache data
	SVC_OUT_GetHdmiPlugStatus(&s_stOmOutputHdmiCache.bPlugIn);
	s_stOmOutputHdmiCache.bSupportCec = MGR_OUTPUT_SupportCec();
	SVC_OUT_GetHdcpStatus(&s_stOmOutputHdmiCache.eHdcpStatus);
	SVC_OUT_GetHdmi3DSelect(&s_stOmOutputHdmiCache.eHdmi3DSelect);
	SVC_OUT_GetHdmi3DFormatCapability(&s_stOmOutputHdmiCache.ul3DFormatCapability);
	SVC_OUT_GetOutputStatus(&stOutputStatus);
	s_stOmOutputHdmiCache.eHdmi3DStatus = stOutputStatus.eHdmi3DStatus;
	HxSTD_MemCopy(&s_stOmOutputHdmiCache.stHdmiEdidInfo, &stOutputStatus.stHdmiEdidInfo, sizeof(stOutputStatus.stHdmiEdidInfo));
//	s_stOmOutputHdmiCache.bCecEnable = MWC_PARAM_GetHdmiCecEnable();
	err = DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_ENABLE, (HUINT32 *)&s_stOmOutputHdmiCache.bCecEnable, 0);
	if (ERR_OK != err)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_ENABLE) Error!!!\n");
	}

	ulResolutionCapability = 0;
	err = SVC_OUT_GetSupportHdmiVideoTimingList(&ulEdidVideoCount, &paEdidVideoTimingList);
	if (err == ERR_OK)
	{
		SVC_OUT_GetCompositeStandard(&eCompStandard);

		for (i = 0; i < ulEdidVideoCount; i++)
		{
			eResolution = paEdidVideoTimingList[i].eResolution;

			if(eCompStandard == eDxCOMPOSITE_STANDARD_NTSC)
			{
				if( (eResolution == eDxRESOLUTION_STATUS_576I) || (eResolution == eDxRESOLUTION_STATUS_576P) )
				{
					continue;
				}
			}
			else
			{
				if( (eResolution == eDxRESOLUTION_STATUS_480I) || (eResolution == eDxRESOLUTION_STATUS_480P) )
				{
					continue;
				}
			}

			ulResolutionCapability |= eResolution;
		}
	}

	s_stOmOutputHdmiCache.ulResolutionCapability = ulResolutionCapability;

	// Create event handler proc
	BUS_MGR_Create (NULL, APP_ACTION_PRIORITY, proc_om_output_hdmi, 0, 0, 0, 0);

	//	-------   RPC Methods   -------	//
	err = HLIB_RPC_RegisterRequest(s_nRpcHandle, RPC_OAPI_OUTPUT_HDMI_Init, NULL, om_output_hdmi_Init, NULL,
							"Activate the output.hdmi module: (void)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tHLIB_RPC_RegisterRequest(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_HDMI_Init, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_HDMI_Set3DSelect, "i", om_output_hdmi_Set3DSelect, NULL,
							"Set HDMI 3D format: (DxHdmi3DSelect_e eHdmi3DSelect)");

	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_HDMI_Set3DSelect, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_HDMI_SetCecEnable, "i", om_output_hdmi_SetCecEnable, NULL,
							"Set HDMI CEC enable: (HBOOL bEnable)");

	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_HDMI_SetCecEnable, err);
	}


	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_HDMI_CEC_SetVolumeEnable, "i", om_output_hdmi_SetCecVolumeEnable, NULL,
							"Set Volume Control of CEC: (HBOOL bEnable)");

	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_HDMI_CEC_SetVolumeEnable, err);
	}

	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_HDMI_CEC_UserCtrlPressed, "i", om_output_hdmi_CecUserCtrlPressed, NULL,
							"Send TV Cec UserCtrlPressed: (DxHdmiUserCtrlPressed_e eParam)");
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_HDMI_CEC_UserCtrlPressed, err);
	}


	err = HLIB_RPC_RegisterCall(s_nRpcHandle, RPC_OAPI_OUTPUT_HDMI_SetNflixDrmStatus, "i", om_output_hdmi_SetNetflixDrmStatus, NULL,
							"Set Netflix DRM status");

	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterMethod(%s) returned error(%d)\n", RPC_OAPI_OUTPUT_HDMI_SetNflixDrmStatus, err);
	}
	//	-------   Notifier  Callbakcs------- //
	err = HLIB_RPC_CreateNotification(s_nRpcHandle, RPC_OBAMA_OUTPUT_HDMI_onStatusChanged, "b",
							"HDMI output parameters and status changed.\n"
							"\t   - (OxOutputHdmi_Cache_t *pstOutputHdmiStatus)\n"
							);
	if(err != ERR_OK)
	{
		HxLOG_Error("\tOBAMA_RegisterCallback(%s) returned error(%d)\n", RPC_OBAMA_OUTPUT_HDMI_onStatusChanged, err);
	}

#if defined(CONFIG_DEBUG)
	om_output_hdmi_RegisterCommand();
#endif

	return ERR_OK;
}



