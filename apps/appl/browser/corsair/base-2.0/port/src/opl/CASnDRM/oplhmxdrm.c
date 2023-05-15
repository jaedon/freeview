/*
 * (c) 2011-2012 Humax Co., Ltd.
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
/**************************************************************************************/
/**
 * @file oplhmxdrm.c
 * defines APIs's body
 *
 * APIs for Humax DRM service
 *
 * @author  Jongwon Kim(jwkim@humaxdigital.com)
 * @date    2012/02/08
 * @attention   Copyright (c) 2012 Humax - All rights reserved.
 * @update rename hmxdrm from phxx by dhkim2
**************************************************************************************
**/
#include <hlib.h>
#include <apk.h>
#include "oplhmxdrm.h"
#include <oapi_media_play.h>

#define OPL_HMXDRM_MAX_DRM_RIGHT_ERROR_SIZE	256

eOPL_HMXDRM_SYSTEM_STATUS_t g_eHmxdrmSystemStatus = eOPL_HMXDRM_SYSTEM_STATUS_UNKNOWN;

char szMsg[OPL_HMXDRM_MAX_DRM_RIGHT_ERROR_SIZE] = {0, };


eOPL_HMXDRM_Error OPL_HMXDRM_Set_VMX_Cas_Info(const char *pszCasUrl, const char *pszVendorName)
{
	eOPL_HMXDRM_Error	err = eOPL_HMXDRM_ERR_OK;
	HUINT32 				ulViewId;
	HERROR				hErr;

	HxLOG_Debug(" + \n");

	hErr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

#if 1//def CONFIG_MW_VERIMATRIX --> mw config 사용할 수 없음 (global config만..)

	g_eHmxdrmSystemStatus = eOPL_HMXDRM_SYSTEM_STATUS_INITIALISING;

	APK_MEDIA_PLAY_SetVmxCas(ulViewId, (HCHAR*)pszCasUrl, (HCHAR*)pszVendorName);

#else
	err = eOPL_HMXDRM_ERR_NOT_SUPPORTED;
#endif

	HxLOG_Debug(" - \n");

	return err;
}

eOPL_HMXDRM_Error OPL_HMXDRM_Set_Playready_Drm(const char *pszLaUrl,
		const char *pszDeviceId, int pszControlFlag) {
		
	eOPL_HMXDRM_Error err = eOPL_HMXDRM_ERR_OK;
	/*
	HUINT32 				ulViewId;
	HERROR				hErr;
	hErr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
	
	hErr = APK_MEDIA_PLAY_SetPlayReadyInfo (ulViewId, (HCHAR*)pszLaUrl, (HCHAR*)pszDeviceId, pszControlFlag);
	
	if (hErr != ERR_OK) {
        HxLOG_Error(" Fail to set playready drm err=%d\n", hErr);
        err = eOPL_HMXDRM_ERR_FAIL;
        return err;
    }*/
	return err;
}

eOPL_HMXDRM_Error OPL_HMXDRM_Get_VMX_Identifier(char **pszIdentifier)
{
	eOPL_HMXDRM_Error err = eOPL_HMXDRM_ERR_OK;

	HxLOG_Debug(" + \n");

#if 1//def CONFIG_MW_VERIMATRIX
	{
		HERROR hErr = ERR_FAIL;

		hErr = APK_MEDIA_PLAY_GetVmxIdentifier((HUINT8**)pszIdentifier);
		if (hErr != ERR_OK)
		{
			HxLOG_Error(" Fail to get UniqueIdentifier err=%d\n", hErr);
			err = eOPL_HMXDRM_ERR_FAIL;
			goto exit;
		}

		HxLOG_Debug(" GetUniqueIdentifier=%s\n", *pszIdentifier);
		err = eOPL_HMXDRM_ERR_OK;
	}
exit:

#else
	err = eOPL_HMXDRM_ERR_NOT_SUPPORTED;
#endif

	HxLOG_Debug(" - \n");
	return err;
}


eOPL_HMXDRM_Error OPL_HMXDRM_Drm_System_Status(int *pnStatus)
{
	eOPL_HMXDRM_Error err = eOPL_HMXDRM_ERR_FAIL;
	HUINT32 				ulViewId;
	HERROR				hErr;

	HxLOG_Debug(" + \n");

	hErr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

#if 1// def CONFIG_MW_VERIMATRIX
	if (g_eHmxdrmSystemStatus == eOPL_HMXDRM_SYSTEM_STATUS_UNKNOWN)
	{
		HxLOG_Error(" drm system is unknown or not init\n");
		err = eOPL_HMXDRM_ERR_FAIL;
		goto exit;
	}

	err = APK_MEDIA_PLAY_GetVmxStatus(ulViewId, pnStatus);

	if (err != eOPL_HMXDRM_ERR_OK)
	{
		HxLOG_Error(" Fail to get system_status err=%d\n", err);
		goto exit;
	}
	g_eHmxdrmSystemStatus = *pnStatus;

	err = eOPL_HMXDRM_ERR_OK;
exit:

#else
	err = eOPL_HMXDRM_ERR_NOT_SUPPORTED;
#endif

	HxLOG_Debug(" - \n");

	return err;
}

eOPL_HMXDRM_Error	OPL_HMXDRM_Drm_Rights_Error(char **pszErrStatus)
{
	eOPL_HMXDRM_Error err = eOPL_HMXDRM_ERR_FAIL;
	HUINT32 				ulViewId;
	HERROR	hErr = eOPL_HMXDRM_ERR_FAIL;
	HINT32 	nVmxState, nVmxErrCode;

	HxLOG_Debug(" + \n");

	hErr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

#if 1 //def CONFIG_MW_VERIMATRIX

	if (g_eHmxdrmSystemStatus == eOPL_HMXDRM_SYSTEM_STATUS_UNKNOWN)
	{
		HxLOG_Error(" drm system is unknown or not init\n");
		err = eOPL_HMXDRM_ERR_FAIL;
		goto exit;
	}

	hErr = APK_MEDIA_PLAY_GetVmxStatus(ulViewId, &nVmxState);
	if (hErr != ERR_OK)
	{
		HxLOG_Error(" Fail to get status err=%d\n", hErr);
		err = eOPL_HMXDRM_ERR_FAIL;
		goto exit;
	}
	hErr = APK_MEDIA_PLAY_GetVmxError(ulViewId, &nVmxErrCode);
	if (hErr != ERR_OK)
	{
		HxLOG_Error(" Fail to get errorCode err=%d\n", hErr);
		err = eOPL_HMXDRM_ERR_FAIL;
		goto exit;
	}
	/*다음과 같이 출력하여야 한다.
	{"systemid":"urn:dvb:casystemid:22017", "status":Verimatrix_Status_t, "errcode":VRWebClientError_t}*/
	HxSTD_snprintf(szMsg, OPL_HMXDRM_MAX_DRM_RIGHT_ERROR_SIZE, "{\"systemid\":\"urn:dvb:casystemid:22017\", \"status\":%d, \"errcode\":%d}",
				nVmxState,nVmxErrCode);

	HxLOG_Debug(" DRM_RIGHT_ERROR_STATUS_MESSAGE = [%s]\n", szMsg);
	err = eOPL_HMXDRM_ERR_OK;

	*pszErrStatus = szMsg;
	HxLOG_Debug(" weberror_status =%s\n", *pszErrStatus);
	err = eOPL_HMXDRM_ERR_OK;
	exit:

#else
	err = eOPL_HMXDRM_ERR_NOT_SUPPORTED;
#endif

	HxLOG_Debug(" - \n");

	return err;
}

eOPL_HMXDRM_Error OPL_HMXDRM_Unset_VMX_Cas_Info(void)
{
	eOPL_HMXDRM_Error	err = eOPL_HMXDRM_ERR_OK;
	HUINT32 				ulViewId;
	HERROR				hErr;

	HxLOG_Debug(" + \n");

	hErr = APK_MEDIA_PLAY_GetMainViewId(&ulViewId);

	g_eHmxdrmSystemStatus = eOPL_HMXDRM_SYSTEM_STATUS_UNKNOWN;

	APK_MEDIA_PLAY_UnsetVmxCas(ulViewId);

	HxLOG_Debug(" - \n");

	return err;
}
