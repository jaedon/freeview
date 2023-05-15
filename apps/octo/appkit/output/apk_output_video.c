/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  apk_output_video.c
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
STATIC APK_OUTPUT_VIDEO_Notifier_t	s_fAppNotifier = NULL;

#define ___PRIVATE_FUNCTION___
#define _____NOTIFIER_FUNCTION_____
STATIC void apk_output_video_onStatusChanged(void)
{
	if(s_fAppNotifier != NULL)
	{
		APK_EVENT_EMIT_ARGS	stApkEvtArgs = {0,};
		HxLOG_Warning("[APK EMIT]VIDEO s_fAppNotifier [%p]\n", s_fAppNotifier);

		// TODO: Queue에 넣는 방식으로 고쳐야 함.

		stApkEvtArgs.pfnFreeFunc = NULL;
		stApkEvtArgs.ulArgCount = 1;
		stApkEvtArgs.apArgV[0] = (void *)s_fAppNotifier;
		APK_EVENT_Emit(&stApkEvtArgs);
	}
}

#define ___MEMBER_FUNCTION___

#define ___API_FUNCTION___
HERROR APK_OUTPUT_VIDEO_Init(void)
{
	OAPI_OUTPUT_VIDEO_Init(apk_output_video_onStatusChanged);

	return ERR_OK;
}

HERROR APK_OUTPUT_VIDEO_DeInit(void)
{
	OAPI_OUTPUT_VIDEO_DeInit();

	if (s_fAppNotifier != NULL)
	{
		s_fAppNotifier = NULL;
	}
	return ERR_OK;
}

HERROR APK_OUTPUT_VIDEO_RegisterNotifier(APK_OUTPUT_VIDEO_Notifier_t fnNotifier)
{
	if(NULL == fnNotifier)
	{
		HxLOG_Error ("[%s:%d] fnNotifier NULL \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}
	else
	{
		s_fAppNotifier = fnNotifier;
	}

	return ERR_OK;
}

HERROR APK_OUTPUT_VIDEO_UnregisterNotifier(void)
{
	s_fAppNotifier = NULL;

	return ERR_OK;
}

HERROR APK_OUTPUT_VIDEO_SupportComposite(HBOOL *pbSupport)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pbSupport = stStatus.bSupportComposite;
	}
	else
	{
		*pbSupport = FALSE;
	}

	return err;
}

HERROR APK_OUTPUT_VIDEO_SupportTvScart(HBOOL *pbSupport)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pbSupport = stStatus.bSupportTvScart;
	}
	else
	{
		*pbSupport = FALSE;
	}

	return err;
}

HERROR APK_OUTPUT_VIDEO_SupportVcrScart(HBOOL *pbSupport)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pbSupport = stStatus.bSupportVcrScart;
	}
	else
	{
		*pbSupport = FALSE;
	}

	return err;
}

HERROR APK_OUTPUT_VIDEO_GetSupportTvScartFormat(DxScartFormat_b *pulScartFormat)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pulScartFormat = stStatus.ulSupportTvScartFormat;
	}
	else
	{
		*pulScartFormat = eDxSCART_FORMAT_UNKNOWN;
	}

	return err;
}


HERROR APK_OUTPUT_VIDEO_GetSupportVcrScartFormat(DxScartFormat_b *pulScartFormat)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pulScartFormat = stStatus.ulSupportVcrScartFormat;
	}
	else
	{
		*pulScartFormat = eDxSCART_FORMAT_UNKNOWN;
	}

	return err;
}


HERROR APK_OUTPUT_VIDEO_SetTvScartFormat(DxScartFormat_e eScartFormat)
{
	return OAPI_OUTPUT_VIDEO_SetTvScartFormat(eScartFormat);
}

HERROR APK_OUTPUT_VIDEO_GetTvScartFormat(DxScartFormat_e *peScartFormat)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peScartFormat = stStatus.eTvScartFormat;
	}
	else
	{
		*peScartFormat = eDxSCART_FORMAT_UNKNOWN;
	}

	return err;
}

HERROR APK_OUTPUT_VIDEO_SetVcrScartFormat(DxScartFormat_e eScartFormat)
{
	return OAPI_OUTPUT_VIDEO_SetVcrScartFormat(eScartFormat);
}

HERROR APK_OUTPUT_VIDEO_GetVcrScartFormat(DxScartFormat_e *peScartFormat)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peScartFormat = stStatus.eVcrScartFormat;
	}
	else
	{
		*peScartFormat = eDxSCART_FORMAT_UNKNOWN;
	}

	return err;
}

HERROR APK_OUTPUT_VIDEO_SetScartPath(DxScartPath_e eScartPath)
{
	return OAPI_OUTPUT_VIDEO_SetScartPath(eScartPath);
}

HERROR APK_OUTPUT_VIDEO_GetScartPath(DxScartPath_e *peScartPath)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peScartPath = stStatus.eScartPath;
	}
	else
	{
		*peScartPath = eDxSCART_PATH_UNKNOWN;
	}

	return err;
}

HERROR APK_OUTPUT_VIDEO_SetResolutionSelect(DxResolutionSelect_e eResolutionSelect)
{
	return OAPI_OUTPUT_VIDEO_SetResolutionSelect(eResolutionSelect);
}

HERROR APK_OUTPUT_VIDEO_GetResolutionSelect(DxResolutionSelect_e *peResolutionSelect)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peResolutionSelect = stStatus.eResolutionSelect;
	}
	else
	{
		*peResolutionSelect = eDxRESOLUTION_SELECT_UNKNOWN;
	}

	return err;
}

HERROR APK_OUTPUT_VIDEO_GetResolutionStatus(DxResolutionStatus_e *peResolutionStatus)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peResolutionStatus = stStatus.eResolutionStatus;
	}
	else
	{
		*peResolutionStatus = eDxRESOLUTION_STATUS_UNKNOWN;
	}

	return err;
}

HERROR APK_OUTPUT_VIDEO_SetTvAspectRatio(DxAspectRatio_e eTvAspectRatio)
{
	return OAPI_OUTPUT_VIDEO_SetTvAspectRatio(eTvAspectRatio);
}

HERROR APK_OUTPUT_VIDEO_GetTvAspectRatio(DxAspectRatio_e *peTvAspectRatio)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peTvAspectRatio = stStatus.eTvAspectRatio;
	}
	else
	{
		*peTvAspectRatio = eDxASPECTRATIO_UNKNOWN;
	}

	return err;
}

HERROR APK_OUTPUT_VIDEO_GetPictureRate(DxPictureRate_e *pePictureRate)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pePictureRate = stStatus.ePictureRate;
	}
	else
	{
		*pePictureRate = eDxPICTURERATE_UNKNOWN;
	}

	return err;
}


HERROR APK_OUTPUT_VIDEO_SetDfcSelect(DxDfcSelect_e eDfcSelect)
{
	return OAPI_OUTPUT_VIDEO_SetDfcSelect(eDfcSelect);
}

HERROR APK_OUTPUT_VIDEO_GetDfcSelect(DxDfcSelect_e *peDfcSelect)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peDfcSelect = stStatus.eDfcSelect;
	}
	else
	{
		*peDfcSelect = eDxDFC_SELECT_UNKNOWN;
	}

	return err;
}


HERROR APK_OUTPUT_VIDEO_GetDfcStatus(DxDfcStatus_b *pulDfcStatus)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pulDfcStatus = stStatus.ulDfcStatus;
	}
	else
	{
		*pulDfcStatus = eDxDFC_STATUS_FULL;
	}

	return err;
}


HERROR APK_OUTPUT_VIDEO_SetBackGroundColor(HUINT8 R, HUINT8 G, HUINT8 B)
{
	return OAPI_OUTPUT_VIDEO_SetBackGroundColor(R, G, B);
}

HERROR APK_OUTPUT_VIDEO_GetBackGroundColor(HUINT8 *pR, HUINT8 *pG, HUINT8 *pB)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pR = stStatus.ucBgColor_R;
		*pG = stStatus.ucBgColor_G;
		*pB = stStatus.ucBgColor_B;
	}
	else
	{
		*pR = 0;
		*pG = 0;
		*pB = 0;
	}

	return err;
}


HERROR APK_OUTPUT_VIDEO_SetAvEnable(HBOOL bEnable)
{
	return OAPI_OUTPUT_VIDEO_SetAvEnable(bEnable);
}

HERROR APK_OUTPUT_VIDEO_GetAvEnable(HBOOL *pbEnable)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*pbEnable = stStatus.bAvEnable;
	}
	else
	{
		*pbEnable = FALSE;
	}

	return err;
}


HERROR APK_OUTPUT_VIDEO_GetCompositeStandard(DxCompositeStandard_e *peCompositeStandard)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peCompositeStandard = stStatus.eCompositeStandard;
	}
	else
	{
		*peCompositeStandard = eDxCOMPOSITE_STANDARD_UNKNOWN;
	}

	return err;
}

HERROR APK_OUTPUT_VIDEO_SetSupportVideoFrame(HBOOL bEnable)
{
	return OAPI_OUTPUT_VIDEO_SetSupportVdieoFrame(bEnable);
}

HERROR APK_OUTPUT_VIDEO_GetVideoBlankCopyRight(DxCopyRight_e *peVideoBlankCopyRight)
{
	HERROR					err;
	OxOutputVideo_Cache_t	stStatus;

	err = OAPI_OUTPUT_VIDEO_GetStatus(&stStatus);

	if(err == ERR_OK)
	{
		*peVideoBlankCopyRight = stStatus.eVideoBlankCopyRight;
	}
	else
	{
		*peVideoBlankCopyRight = eDxCOPYRIGHT_COPY_NEVER;
	}

	return err;
}

HERROR APK_OUTPUT_VIDEO_SetVideoBlankCopyRight(DxCopyRight_e eVideoBlankCopyRight)
{
	return  OAPI_OUTPUT_VIDEO_SetVideoBlankCopyRight(eVideoBlankCopyRight);
}



/* end of file */
