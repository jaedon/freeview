/**************************************************************
*	@file	homma_interop.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			HOMMA / interoperation supporting instead
*					of using APK
**************************************************************/

/*
* ⓒ 2011-2012 Humax Co., Ltd.
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
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

/* ------------------------------------------------------------- //
 * Author : tjjang
 * Description : This is for replacing apk methods
 *  - HOMMA_INTEROP prefix is used for external methods
 *  - s_HOMMA_INTEROP prefix is used to represent static methods
 */

#include <homma_interop.h>

/*
 * methods
 *  - static & external
 */


/* ############################################################################
___  ___           _  _                 _
|  \/  |          | |(_)               | |
| .  . |  ___   __| | _   __ _   _ __  | |  __ _  _   _
| |\/| | / _ \ / _` || | / _` | | '_ \ | | / _` || | | |
| |  | ||  __/| (_| || || (_| | | |_) || || (_| || |_| |
\_|  |_/ \___| \__,_||_| \__,_| | .__/ |_| \__,_| \__, |
                                | |                __/ |
                                |_|               |___/
 xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  media play  xxxxxxxxx
############################################################################ */

// unwilling to use apps/octo/appkit/
#ifdef HOMMA_USE_APPKIT_DIRECTLY

HERROR HOMMA_INTEROP_MEDIA_PLAY_Init (void)
{
	return APK_MEDIA_PLAY_Init();
}

HERROR HOMMA_INTEROP_MEDIA_PLAY_GetRequestId(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, HUINT32 *pulRequestId)
{
	return APK_MEDIA_PLAY_GetRequestId(ulViewId, eType, pulRequestId);
}

HERROR HOMMA_INTEROP_MEDIA_PLAY_GetPlayType (HUINT32 ulViewId, OxMediaPlay_MediaType_e *peType)
{
	return APK_MEDIA_PLAY_GetPlayType (ulViewId, peType);
}

HERROR HOMMA_INTEROP_MEDIA_PLAY_GetPlayState (HUINT32 ulViewId, OxMediaPlay_PlayState_e *peState)
{
	return APK_MEDIA_PLAY_GetPlayState (ulViewId, peState);
}

HERROR HOMMA_INTEROP_MEDIA_PLAY_GetURI (HUINT32 ulViewId, HCHAR *pszURI)
{
	return APK_MEDIA_PLAY_GetURI(ulViewId, pszURI);
}

HERROR HOMMA_INTEROP_MEDIA_PLAY_GetMainViewId (HUINT32 *pulViewId)
{
	return APK_MEDIA_PLAY_GetMainViewId(pulViewId);
}

HERROR HOMMA_INTEROP_MEDIA_PLAY_SetPlayPosition (HUINT32 ulViewId, HUINT32 ulPosition)
{
	return APK_MEDIA_PLAY_SetPlayPosition(ulViewId, ulPosition);
}

HERROR HOMMA_INTEROP_MEDIA_PLAY_RegisterNotifier (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfNotifier, void *pvUserData)
{
	return APK_MEDIA_PLAY_RegisterNotifier(ulViewId, pfNotifier, pvUserData);
}

HERROR HOMMA_INTEROP_MEDIA_PLAY_UnregisterNotifier (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfNotifier, void *pvUserData)
{
	return APK_MEDIA_PLAY_UnregisterNotifier(ulViewId, pfNotifier, pvUserData);
}

#else // #ifdef HOMMA_USE_APPKIT_DIRECTLY else

//APK_MEDIA_PLAY_Init();
HERROR HOMMA_INTEROP_MEDIA_PLAY_Init (OxMediaPlay_ActivatedCb_t pfActivated)
{
	OAPI_MEDIA_PLAY_Init(pfActivated);
	return ERR_OK;
}

//APK_MEDIA_PLAY_GetRequestId (ulViewId, eOxMP_MEDIATYPE_LIVE, &ulSvcUID);
HERROR HOMMA_INTEROP_MEDIA_PLAY_GetRequestId(HUINT32 ulViewId, OxMediaPlay_MediaType_e eType, HUINT32 *pulRequestId)
{
	HERROR hErr;

	hErr = OAPI_MEDIA_PLAY_GetRequestId (ulViewId, eType, pulRequestId);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_PLAY_GetRequestId err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	return ERR_OK;
}

//APK_MEDIA_PLAY_GetPlayType (ulViewId, &eMediaType);
HERROR HOMMA_INTEROP_MEDIA_PLAY_GetPlayType (HUINT32 ulViewId, OxMediaPlay_MediaType_e *peType)
{
	OxMediaPlay_MediaType_e	  eOapiType;
	HERROR					   hErr;

	hErr = OAPI_MEDIA_PLAY_GetPlayType (ulViewId, &eOapiType);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_PLAY_GetPlayType err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	if (NULL != peType)
	{
		*peType = eOapiType;
	}

	return ERR_OK;
}

//APK_MEDIA_PLAY_GetPlayState (ulViewId, &eState);
HERROR HOMMA_INTEROP_MEDIA_PLAY_GetPlayState (HUINT32 ulViewId, OxMediaPlay_PlayState_e *peState)
{
	HERROR hErr;

	if (NULL == peState)
	{
		return ERR_FAIL;
	}

	hErr = OAPI_MEDIA_PLAY_GetPlayState (ulViewId, peState);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_PLAY_GetPlayState err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

//APK_MEDIA_PLAY_GetURI(ulViewId, szPlayingUrl);
HERROR HOMMA_INTEROP_MEDIA_PLAY_GetURI (HUINT32 ulViewId, HCHAR *pszURI)
{
	HERROR hErr;

	hErr = OAPI_MEDIA_PLAY_GetURI (ulViewId, pszURI);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_PLAY_GetURI err:\n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	return ERR_OK;
}

//APK_MEDIA_PLAY_GetMainViewId(&ulViewId);
HERROR HOMMA_INTEROP_MEDIA_PLAY_GetMainViewId (HUINT32 *pulViewId)
{
	if (NULL != pulViewId)
	{
		*pulViewId = OAPI_MEDIA_PLAY_GetMainViewId();
	}

	return ERR_OK;
}

//APK_MEDIA_PLAY_SetPlayPosition(ulViewId, p2);
HERROR HOMMA_INTEROP_MEDIA_PLAY_SetPlayPosition (HUINT32 ulViewId, HUINT32 ulPosition)
{
	HERROR hErr;

	hErr = OAPI_MEDIA_PLAY_SetPlayPosition (ulViewId, ulPosition);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("[%s:%d] OAPI_MEDIA_PLAY_SetPlayPosition err: ViewId=%d\n", __FUNCTION__, __LINE__, ulViewId);
		return ERR_FAIL;
	}

	return ERR_OK;
}

//APK_MEDIA_PLAY_RegisterNotifier(ulViewId, __hom_dmr_media_info_notifier, NULL);
HERROR HOMMA_INTEROP_MEDIA_PLAY_RegisterNotifier (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfNotifier, void *pvUserData)
{
	(void)OAPI_MEDIA_PLAY_SetListener(ulViewId, pfNotifier, NULL);

	return ERR_OK;
}

// APK_MEDIA_PLAY_UnregisterNotifier();
HERROR HOMMA_INTEROP_MEDIA_PLAY_UnregisterNotifier (HUINT32 ulViewId, OxMediaPlay_Notifier_t pfNotifier, void *pvUserData)
{
	(void)OAPI_MEDIA_PLAY_ResetListener(ulViewId, pfNotifier, NULL);

	return ERR_OK;
}
#endif // #ifdef HOMMA_USE_APPKIT_DIRECTLY endif

