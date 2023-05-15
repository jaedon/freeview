/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_output_hdmi.c
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

#include <oapi.h>
#include <apk.h>

#define ___INTERNAL_STRUCTURES___

#define ___INTERNAL_VARIABLES___
STATIC APLK_OUTPUT_HDMI_Notifier_t	s_pfAppNotifier = NULL;

#define ___PRIVATE_FUNCTION___
#define _____NOTIFIER_FUNCTION_____
STATIC void oapi_output_hdmi_onStatusChanged(void)
{
	if(s_pfAppNotifier != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};

		HxLOG_Warning("[APK EMIT]HDMI s_pfAppNotifier [%p]\n", s_pfAppNotifier);
		// TODO: Queue에 넣는 방식으로 고쳐야 함.

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 1;
		stApkEvtArgs.apArgV[0] = (void *)s_pfAppNotifier;
		APK_EVENT_Emit(&stApkEvtArgs);
	}
}

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
HERROR APK_OUTPUT_HDMI_Init(void)
{
	HxLOG_Debug("%s()\n", __FUNCTION__);

	OAPI_OUTPUT_HDMI_Init(oapi_output_hdmi_onStatusChanged);

	return ERR_OK;
}

HERROR APK_OUTPUT_HDMI_DeInit(void)
{
	HxLOG_Debug("%s()\n", __FUNCTION__);

	OAPI_OUTPUT_HDMI_DeInit();

	if (s_pfAppNotifier != NULL)
	{
		s_pfAppNotifier = NULL;
	}
	return ERR_OK;
}

HERROR APK_OUTPUT_HDMI_RegisterNotifier(APLK_OUTPUT_HDMI_Notifier_t fnNotifier)
{
	HxLOG_Debug("%s()\n", __FUNCTION__);

	if(NULL == fnNotifier)
	{
		HxLOG_Error ("[%s:%d] fnNotifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else
	{
		s_pfAppNotifier = fnNotifier;
	}

	return ERR_OK;
}

HERROR APK_OUTPUT_HDMI_UnregisterNotifier(void)
{
	HxLOG_Debug("%s()\n", __FUNCTION__);

	s_pfAppNotifier = NULL;

	return ERR_OK;
}

HERROR APK_OUTPUT_HDMI_SupportCec(HBOOL *pbSupport)
{
	HERROR					err;
	OxOutputHdmi_Cache_t	stStatus;

	err = OAPI_OUTPUT_HDMI_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pbSupport = stStatus.bSupportCec;
	}
	else
	{
		*pbSupport = FALSE;
	}

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, *pbSupport);

	return err;
}

HERROR APK_OUTPUT_HDMI_GetHdcpVersion(DxHdcpVersion_e *peVersion)
{
	HERROR					err;
	OxOutputHdmi_Cache_t	stStatus;

	err = OAPI_OUTPUT_HDMI_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peVersion = stStatus.eHdcpVer;
	}
	else
	{
		*peVersion = eDxHDCP_VERSION_MAX;
	}
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, *peVersion);
	return err;
}


HERROR APK_OUTPUT_HDMI_GetHdcpStatus(DxHdcpStatus_e *peHdcpStatus)
{
	HERROR					err;
	OxOutputHdmi_Cache_t	stStatus;

	err = OAPI_OUTPUT_HDMI_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peHdcpStatus = stStatus.eHdcpStatus;
	}
	else
	{
		*peHdcpStatus = eDxHDCPSTATUS_OFF;
	}

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, *peHdcpStatus);

	return err;
}

HERROR APK_OUTPUT_HDMI_GetNetflixDrmStatus(DxNflixDrmStatus_e *peNflixDrmStatus)
{
	HERROR					err;
	OxOutputHdmi_Cache_t	stStatus;

	err = OAPI_OUTPUT_HDMI_GetStatus(&stStatus);
	if(err == ERR_OK)
	{
		*peNflixDrmStatus = stStatus.eNflixDrmStatus;
	}
	else
	{
		*peNflixDrmStatus = eNFLIX_DRM_NA;
	}
	HxLOG_Debug("(%d)\n", *peNflixDrmStatus);

	return err;
}

HERROR APK_OUTPUT_HDMI_SetNetflixDrmStatus(DxNflixDrmStatus_e eNflixDrmStatus)
{
	return OAPI_OUTPUT_HDMI_SetNetflixDrmStatus(eNflixDrmStatus);
}

HERROR APK_OUTPUT_HDMI_IsPlugIn(HBOOL *pbPlugInOut)
{
	HERROR					err;
	OxOutputHdmi_Cache_t	stStatus;

	err = OAPI_OUTPUT_HDMI_GetStatus(&stStatus);
	if(err == ERR_OK)
	{
		*pbPlugInOut = stStatus.bPlugIn;
	}
	else
	{
		*pbPlugInOut = FALSE;
	}
	HxLOG_Debug("(%d)\n", *pbPlugInOut);

	return err;
}

HERROR APK_OUTPUT_HDMI_Set3DSelect(DxHdmi3DSelect_e eHdmi3DSelect)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, eHdmi3DSelect);

	return OAPI_OUTPUT_HDMI_Set3DSelect(eHdmi3DSelect);
}

HERROR APK_OUTPUT_HDMI_Get3DSelect(DxHdmi3DSelect_e *peHdmi3DSelect)
{
	HERROR					err;
	OxOutputHdmi_Cache_t	stStatus;

	err = OAPI_OUTPUT_HDMI_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peHdmi3DSelect = stStatus.eHdmi3DSelect;
	}
	else
	{
		*peHdmi3DSelect = eDxHDMI_3DSELECT_OFF;
	}

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, *peHdmi3DSelect);

	return err;
}


HERROR APK_OUTPUT_HDMI_GetEdid3DFormatCapability(DxVideo3DFormat_b *pul3DFormatCapability)
{
	HERROR					err;
	OxOutputHdmi_Cache_t	stStatus;

	err = OAPI_OUTPUT_HDMI_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pul3DFormatCapability = stStatus.ul3DFormatCapability;
	}
	else
	{
		*pul3DFormatCapability = eDxVIDEO_3DFORMAT_NONE;
	}

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, *pul3DFormatCapability);

	return err;
}


HERROR APK_OUTPUT_HDMI_Get3DStatus(DxVideo3DFormat_e *peHdmi3DStatus)
{
	HERROR					err;
	OxOutputHdmi_Cache_t	stStatus;

	err = OAPI_OUTPUT_HDMI_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peHdmi3DStatus = stStatus.eHdmi3DStatus;
	}
	else
	{
		*peHdmi3DStatus = eDxVIDEO_3DFORMAT_NONE;
	}

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, *peHdmi3DStatus);

	return err;
}


HERROR APK_OUTPUT_HDMI_GetEdidResolutionCapability(DxResolutionStatus_b *pulResolutionCapability)
{
	HERROR					err;
	OxOutputHdmi_Cache_t	stStatus;

	err = OAPI_OUTPUT_HDMI_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pulResolutionCapability = stStatus.ulResolutionCapability;
	}
	else
	{
		*pulResolutionCapability = eDxRESOLUTION_STATUS_UNKNOWN;
	}

	HxLOG_Debug("%s(0x%x)\n", __FUNCTION__, *pulResolutionCapability);

	return err;
}

HERROR	APK_OUTPUT_HDMI_SetCecEnable(HBOOL bEnable)
{
	HxLOG_Debug("%s(%d)\n", __FUNCTION__, bEnable);

	return OAPI_OUTPUT_HDMI_SetCecEnable(bEnable);
}

HERROR	APK_OUTPUT_HDMI_GetCecEnable(HBOOL *pbEnable)
{
	HERROR					err;
	OxOutputHdmi_Cache_t	stStatus;

	err = OAPI_OUTPUT_HDMI_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pbEnable = stStatus.bCecEnable;
	}
	else
	{
		*pbEnable = FALSE;
	}

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, *pbEnable);

	return err;
}

HERROR APK_OUTPUT_HDMI_SetCecVolumeCtrlEnable(HBOOL bEnable)
{
    HxLOG_Debug("%s(%d)\n", __FUNCTION__, bEnable);

	return OAPI_OUTPUT_HDMI_SetCecVolumeCtrlEnable(bEnable);
}


HERROR	APK_OUTPUT_HDMI_GetCecVolumeCtrlEnable(HBOOL *pbEnable)
{
	HERROR					err;
	OxOutputHdmi_Cache_t	stStatus;

	err = OAPI_OUTPUT_HDMI_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pbEnable = stStatus.bCecVolumeCtrlEnable;
	}
	else
	{
		*pbEnable = FALSE;
	}

	HxLOG_Debug("%s(%d)\n", __FUNCTION__, *pbEnable);

	return err;
}

HERROR	APK_OUTPUT_HDMI_CecUserCtrlPressed(DxHdmiUserCtrlPressed_e eParam)
{
    HxLOG_Debug("%s(%d)\n", __FUNCTION__, eParam);

	return OAPI_OUTPUT_HDMI_CEC_UserCtrlPressed(eParam);
}

HERROR	APK_OUTPUT_HDMI_GetEdidInfo(HCHAR *pszEdidInfo)
{
	HERROR					err;
	OxOutputHdmi_Cache_t	stStatus;
	HINT32					nStream;
	HUINT32					ulSerialNum = 0;

	err = OAPI_OUTPUT_HDMI_GetStatus(&stStatus);
	if (err != ERR_OK)
	{
		HxLOG_Error("OAPI_OUTPUT_HDMI_GetStatus failed. \n");
		return ERR_FAIL;
	}

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 512, HLIB_STD_MemAlloc_CB, HLIB_STD_MemFree_CB);
	if (!nStream)
	{
		HxLOG_Error("HLIB_RWSTREAM_OpenEx failed. \n");
		return ERR_FAIL;
	}

	stStatus.stHdmiEdidInfo.monitorName[14-1] = '\0';
	ulSerialNum = stStatus.stHdmiEdidInfo.serialNum[0]*1000
				+ stStatus.stHdmiEdidInfo.serialNum[1]*100
				+ stStatus.stHdmiEdidInfo.serialNum[2]*10
				+ stStatus.stHdmiEdidInfo.serialNum[3];

	HLIB_RWSTREAM_Print(nStream,"{\"HDMI_EDID_INFO\":");
	HLIB_RWSTREAM_Print(nStream,"{\"VENDOR_ID\":\"%d,%d\",", stStatus.stHdmiEdidInfo.vendorID[0], stStatus.stHdmiEdidInfo.vendorID[1]);
	HLIB_RWSTREAM_Print(nStream,"\"PRODUCT_ID\":\"%d,%d\",", stStatus.stHdmiEdidInfo.productID[0], stStatus.stHdmiEdidInfo.productID[1]);
	HLIB_RWSTREAM_Print(nStream,"\"SERIAL_NUM\":%d,",ulSerialNum);
	HLIB_RWSTREAM_Print(nStream,"\"MONITOR_NAME\":\"%s\",", stStatus.stHdmiEdidInfo.monitorName);
	HLIB_RWSTREAM_Print(nStream,"\"MANUF_WEEK\":%d,", stStatus.stHdmiEdidInfo.manufWeek);
	HLIB_RWSTREAM_Print(nStream,"\"MANUF_YEAR\":%d,", stStatus.stHdmiEdidInfo.manufYear);
	HLIB_RWSTREAM_Print(nStream,"\"EDID_VERSION\":%d,", stStatus.stHdmiEdidInfo.edidVersion);
	HLIB_RWSTREAM_Print(nStream,"\"EDID_REVISION\":%d,", stStatus.stHdmiEdidInfo.edidRevision);
	HLIB_RWSTREAM_Print(nStream,"\"FIRST_VIDEO_ID\":%d}}", stStatus.stHdmiEdidInfo.firstVideoId);

	HxSTD_snprintf(pszEdidInfo,512,"%s",(HCHAR*)HLIB_RWSTREAM_GetBuf(nStream));
	HLIB_RWSTREAM_Close(nStream);

	return ERR_OK;
}

HERROR APK_OUTPUT_HDMI_GetEdidDisplayResolution(HUINT32 *pulDisplayResX, HUINT32 *pulDisplayResY)
{
	HERROR					err;
	OxOutputHdmi_Cache_t	stStatus;
	HINT32					nStream;

	err = OAPI_OUTPUT_HDMI_GetStatus(&stStatus);
	if (err != ERR_OK)
	{
		HxLOG_Error("OAPI_OUTPUT_HDMI_GetStatus failed. \n");
		return ERR_FAIL;
	}
	*pulDisplayResX = stStatus.stHdmiEdidInfo.maxHorizSize;
	*pulDisplayResY = stStatus.stHdmiEdidInfo.maxVertSize;
	return ERR_OK;
}


/* end of file */
