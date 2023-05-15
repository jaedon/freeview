/**************************************************************
*	@file		ondk.c
*	Minimized Graphic Library for DirectFB
*
*	http://www.humaxdigital.com
*
*	@data			2012/09/17
*	@author			humax
*	@brief			Minimized Graphic Library for DirectFB
**************************************************************/


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
* ¨Ï 2011-2012 Humax Co., Ltd.
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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include 	<ondk.h>

#include 	"ondk_dfb.h"




/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
#if defined(ONDK_DFB_VIDEO_CALLBACK_TO_GWM_THREAD)
typedef struct
{
	ONDKVideoHandle	 vHandle;
	void			*pvCbFunc;
	void			*pvUserData;
} _defvideo_PlayCb_t;

STATIC HxList_t			*s_pstOndkDfbVideo_PlayCbLists = NULL;
#endif

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

static void _dfbvideo_Play( ONDKVideoHandle vhandle,
										ONDKSurfaceHandle targetSurface,
										ONDK_Rect_t *rt,
										void (* updateScreen)(const void *),
										void *userdata)
{
	DFBResult				err;
	IDirectFBVideoProvider	*pVideoProvider = (IDirectFBVideoProvider *)vhandle;
	IDirectFBSurface		*pSurface = (IDirectFBSurface *)targetSurface;

	if(rt != NULL)
	{
		DFBRectangle	rect = {rt->x, rt->y, rt->w, rt->h};
		DFBCHECK(pVideoProvider->PlayTo( pVideoProvider, pSurface, &rect, (void *)updateScreen, userdata ));
	}
	else
	{
		DFBCHECK(pVideoProvider->PlayTo( pVideoProvider, pSurface, NULL, (void *)updateScreen, userdata ));
	}
}

#if defined(ONDK_DFB_VIDEO_CALLBACK_TO_GWM_THREAD)
STATIC void _dfbvideo_CbByDfbPlay (const void *pvUserData)
{
	_defvideo_PlayCb_t			*pstCbInfo = (_defvideo_PlayCb_t *)pvUserData;

	if ((NULL != pstCbInfo) && (NULL != pstCbInfo->pvCbFunc) && (NULL != s_pstOndkDfbVideo_PlayCbLists))
	{
		ONDK_GWM_PostMessage(NULL, MSG_APP_EXECUTE_CALLBACK, HANDLE_NULL, (HINT32)pstCbInfo->pvCbFunc, (HINT32)pstCbInfo->pvUserData, 0);
	}
}

STATIC void _dfbvideo_PlayByCbInfo(ONDKVideoHandle vhandle,
										ONDKSurfaceHandle targetSurface,
										ONDK_Rect_t *rt,
										void (* updateScreen)(const void *),
										void *userdata)
{
	_defvideo_PlayCb_t			*pstCbInfo = NULL;

	pstCbInfo = (_defvideo_PlayCb_t *)HLIB_MEM_Calloc(sizeof(_defvideo_PlayCb_t));
	if (NULL != pstCbInfo)
	{
		pstCbInfo->vHandle = vhandle;
		pstCbInfo->pvCbFunc = (void *)updateScreen;
		pstCbInfo->pvUserData = (void *)userdata;
		s_pstOndkDfbVideo_PlayCbLists = HLIB_LIST_Append(s_pstOndkDfbVideo_PlayCbLists, (void *)pstCbInfo);

		_dfbvideo_Play(vhandle, targetSurface, rt, _dfbvideo_CbByDfbPlay, (void *)pstCbInfo);
	}
}

STATIC HBOOL _dfbvideo_CmpCbInfoByVideoHandle (void *userdata, void *data)
{
	ONDKVideoHandle				 vHandle = (ONDKVideoHandle)userdata;
	_defvideo_PlayCb_t			*pstCbInfo = (_defvideo_PlayCb_t *)data;

	if ((NULL != pstCbInfo) && (vHandle == pstCbInfo->vHandle))
	{
		return TRUE;
	}

	return FALSE;
}

STATIC void _dfbvideo_RemoveCbInfoByVideoHandle (ONDKVideoHandle vhandle)
{
	s_pstOndkDfbVideo_PlayCbLists = HLIB_LIST_RemoveIf(s_pstOndkDfbVideo_PlayCbLists, (void *)vhandle, _dfbvideo_CmpCbInfoByVideoHandle, HLIB_STD_MemFree_CB);
}
#endif

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDKVideoHandle ONDK_VIDEO_Create( const char *a_pFile )
{
	DFBResult				err;
	IDirectFB				*pDirectFB = NULL;
	IDirectFBVideoProvider	*pVideoProvider = NULL;

	pDirectFB = ONDK_DFB_GetIDirectFB();
	ONDK_Assert(pDirectFB);


	/* Create video provider */
	DFBCHECK(pDirectFB->CreateVideoProvider( pDirectFB, a_pFile, &pVideoProvider ));

	return (ONDKVideoHandle)pVideoProvider;
}

ONDK_Result_t ONDK_VIDEO_PlayOnece( 	ONDKVideoHandle vhandle,
										ONDKSurfaceHandle targetSurface,
										ONDK_Rect_t *rt,
										void (* updateScreen)(const void *),
										void *userdata)
{
	DFBResult				err;
	IDirectFBVideoProvider	*pVideoProvider = (IDirectFBVideoProvider *)vhandle;
	ONDK_Result_t			oErr = ONDK_RET_OK;

	oErr = ONDK_VIDEO_IsBusy(vhandle);
	if(oErr != ONDK_RET_OK)
	{
		return oErr;
	}

	DFBCHECK(pVideoProvider->SetPlaybackFlags( pVideoProvider, DVPLAY_NOFX));	//DVPLAY_LOOPING
#if defined(ONDK_DFB_VIDEO_CALLBACK_TO_GWM_THREAD)
	_dfbvideo_PlayByCbInfo(vhandle, targetSurface, rt, updateScreen, userdata);
#else
	_dfbvideo_Play(vhandle, targetSurface, rt, updateScreen, userdata);
#endif

	return ONDK_RET_OK;
}

ONDK_Result_t ONDK_VIDEO_PlayLoop( 	ONDKVideoHandle vhandle,
										ONDKSurfaceHandle targetSurface,
										ONDK_Rect_t *rt,
										void (* updateScreen)(const void *),
										void *userdata)
{
	DFBResult				err;
	IDirectFBVideoProvider	*pVideoProvider = (IDirectFBVideoProvider *)vhandle;
	ONDK_Result_t			oErr = ONDK_RET_OK;

	oErr = ONDK_VIDEO_IsBusy(vhandle);
	if(oErr != ONDK_RET_OK)
	{
		return oErr;
	}

	DFBCHECK(pVideoProvider->SetPlaybackFlags( pVideoProvider, DVPLAY_LOOPING));	//DVPLAY_LOOPING
#if defined(ONDK_DFB_VIDEO_CALLBACK_TO_GWM_THREAD)
	_dfbvideo_PlayByCbInfo(vhandle, targetSurface, rt, updateScreen, userdata);
#else
	_dfbvideo_Play(vhandle, targetSurface, rt, updateScreen, userdata);
#endif

	return ONDK_RET_OK;
}

ONDK_Result_t ONDK_VIDEO_PlayRewind( 	ONDKVideoHandle vhandle,
										ONDKSurfaceHandle targetSurface,
										ONDK_Rect_t *rt,
										void (* updateScreen)(const void *),
										void *userdata)
{
	DFBResult				err;
	IDirectFBVideoProvider	*pVideoProvider = (IDirectFBVideoProvider *)vhandle;
	ONDK_Result_t			oErr = ONDK_RET_OK;

	oErr = ONDK_VIDEO_IsBusy(vhandle);
	if(oErr != ONDK_RET_OK)
	{
		return oErr;
	}

	DFBCHECK(pVideoProvider->SetPlaybackFlags( pVideoProvider, DVPLAY_REWIND));	//DVPLAY_LOOPING
#if defined(ONDK_DFB_VIDEO_CALLBACK_TO_GWM_THREAD)
	_dfbvideo_PlayByCbInfo(vhandle, targetSurface, rt, updateScreen, userdata);
#else
	_dfbvideo_Play(vhandle, targetSurface, rt, updateScreen, userdata);
#endif

	return ONDK_RET_OK;
}

ONDK_Result_t ONDK_VIDEO_SeekTo(ONDKVideoHandle vhandle, HINT32 seconds)
{
	DFBResult				err;
	IDirectFBVideoProvider	*pVideoProvider = (IDirectFBVideoProvider *)vhandle;

	DFBCHECK(pVideoProvider->SeekTo(pVideoProvider, seconds));
	return ONDK_RET_OK;
}

ONDK_Result_t ONDK_VIDEO_GetPos(ONDKVideoHandle vhandle, HINT32 *ret_seconds)
{
	DFBResult				err;
	IDirectFBVideoProvider	*pVideoProvider = (IDirectFBVideoProvider *)vhandle;
	double					seconds = 0;

	DFBCHECK(pVideoProvider->GetPos(pVideoProvider, &seconds));
	*ret_seconds = (HINT32)seconds;

	return ONDK_RET_OK;
}

ONDK_Result_t ONDK_VIDEO_GetLength(ONDKVideoHandle vhandle, HINT32 *ret_seconds)
{
	DFBResult				err;
	IDirectFBVideoProvider	*pVideoProvider = (IDirectFBVideoProvider *)vhandle;
	double					seconds = 0;

	DFBCHECK(pVideoProvider->GetLength(pVideoProvider, &seconds));
	*ret_seconds = (HINT32)seconds;
	return ONDK_RET_OK;
}

ONDK_Result_t ONDK_VIDEO_IsBusy(ONDKVideoHandle vhandle)
{
	DFBResult				err;
	IDirectFBVideoProvider	*pVideoProvider = (IDirectFBVideoProvider *)vhandle;
	DFBVideoProviderStatus	status;

	DFBCHECK(pVideoProvider->GetStatus(pVideoProvider, &status));

	if(status == DVSTATE_PLAY || status == DVSTATE_BUFFERING)
	{
		return ONDK_RET_BUSY;
	}

	return ONDK_RET_OK;
}


ONDK_Result_t ONDK_VIDEO_Stop( ONDKVideoHandle vhandle)
{
	DFBResult				err;
	IDirectFBVideoProvider	*pVideoProvider = (IDirectFBVideoProvider *)vhandle;
	ONDK_Result_t oErr = ONDK_RET_OK;

	oErr = ONDK_VIDEO_IsBusy(vhandle);
	if(oErr == ONDK_RET_OK)
	{
		return ONDK_RET_OK;
	}

	DFBCHECK(pVideoProvider->Stop(pVideoProvider));

	return ONDK_RET_OK;
}

ONDK_Result_t ONDK_VIDEO_Release( ONDKVideoHandle vhandle)
{
	DFBResult				err;
	IDirectFBVideoProvider	*pVideoProvider = (IDirectFBVideoProvider *)vhandle;

	ONDK_VIDEO_Stop(vhandle);
	DFBCHECK(pVideoProvider->Release(pVideoProvider));

#if defined(ONDK_DFB_VIDEO_CALLBACK_TO_GWM_THREAD)
	_dfbvideo_RemoveCbInfoByVideoHandle(vhandle);
#endif

	return ONDK_RET_OK;
}


ONDK_Result_t ONDK_VIDEO_SetSpeed(ONDKVideoHandle vhandle, HFLOAT64 speed) //nomal==1.0
{
	DFBResult				err;
	IDirectFBVideoProvider	*pVideoProvider = (IDirectFBVideoProvider *)vhandle;

	DFBCHECK(pVideoProvider->SetSpeed(pVideoProvider, speed));
	return ONDK_RET_OK;
}


ONDK_Result_t ONDK_VIDEO_GetSpeed(ONDKVideoHandle vhandle, HFLOAT64 *speed)
{
	DFBResult				err;
	IDirectFBVideoProvider	*pVideoProvider = (IDirectFBVideoProvider *)vhandle;

	DFBCHECK(pVideoProvider->GetSpeed(pVideoProvider, speed));
	return ONDK_RET_OK;
}


ONDK_Result_t  ONDK_DFB_VIDEO_Init(HINT32 argc, HCHAR **argv)
{
	//ONDK_Result_t oErr = ONDK_RET_OK;



	return ONDK_RET_OK;
}

