/**************************************************************
*	http://www.humaxdigital.com
*	@author			humax
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


#include <htype.h>
#include "gwm_animation.h"

#ifdef CONFIG_DEBUG
HUINT32 g_GwmAniDebug_Level = (DBG_ASSERT); //DBG_TRACE|DBG_PRINT|DBG_DATA;
HUINT32 g_GwmAniDebug_Color =	0;

#define GwmAniPrint(level, message)		(((level) & g_GwmAniDebug_Level) ? (HxLOG_Print("\033[01;%dm", g_GwmAniDebug_Color), HxLOG_Print("[GWM:Animation]"), HxLOG_Print message, HxLOG_Print("\033[0m")) : 0)
#define GwmAniAssert(c) 				((c) ? 0 : (HxLOG_Print("\033[01;%dm", g_GwmAniDebug_Color), HxLOG_Print("\nAnimation assertion on %s: line%d\n\n", __FILE__, __LINE__), HxLOG_Print("\033[00m")))
#define GwmAniData(level, message)		(((level) & g_GwmAniDebug_Level) ? ( HxLOG_Print message) : 0)
#else
#define GwmAniPrint(level, message)
#define GwmAniAssert(c)
#define GwmAniData(level, message)
#endif

static HBOOL	s_bAnimationGo;
static HINT32	s_nCountOfOnGoingAnimation;
//static HBOOL	s_bAnimationInstanceInfo[];

void	gwm_ani_InitAnimation(void)
{
	s_bAnimationGo = FALSE;
	s_nCountOfOnGoingAnimation = 0;
}

ONDK_Result_t	gwm_ani_RunTimingFunc(hViewHandle_t hCurrentView)
{
	ONDK_Result_t				errResult = ERR_FAIL;
	GWM_Animation_View_t		*pView = NULL;

	pView = (GWM_Animation_View_t *)hCurrentView;

	if(pView->focus != TRUE)
	{
		GwmAniAssert(0);
		return GWT_RET_ALREADY_STOP;
	}

	if(pView->pfnTimingFunc != NULL)
	{
		errResult = (pView->pfnTimingFunc)(hCurrentView, 0);
		if(errResult != GWM_RET_OK)
		{
			GwmAniAssert(0);
		}

		/* test */
		errResult = GWM_RET_OK;
	}

	return errResult;
}

ONDK_Result_t	gwm_ani_RunTransFormFunc(hViewHandle_t hCurrentView)
{
	ONDK_Result_t				errResult = ERR_FAIL;
	GWM_Animation_View_t		*pView = NULL;

	pView = (GWM_Animation_View_t *)hCurrentView;

	if(pView->focus != TRUE)
	{
		GwmAniAssert(0);
		return GWT_RET_ALREADY_STOP;
	}

	if(pView->pfnTransFunc != NULL)
	{
		errResult = (pView->pfnTransFunc)(hCurrentView, 0);
		if(errResult != GWM_RET_OK)
		{
			GwmAniAssert(0);
		}

		/* test */
		errResult = GWM_RET_OK;
	}

	return errResult;
}

ONDK_Result_t	gwm_ani_RunPathFunc(hViewHandle_t hCurrentView)
{
	ONDK_Result_t				errResult = ERR_FAIL;
	GWM_Animation_View_t		*pView = NULL;

	pView = (GWM_Animation_View_t *)hCurrentView;

	if(pView->focus != TRUE)
	{
		GwmAniAssert(0);
		return GWT_RET_ALREADY_STOP;
	}

	if(pView->pfnTransFunc != NULL)
	{
		errResult = (pView->pfnPathFunc)(hCurrentView, 0);
		if(errResult != GWM_RET_OK)
		{
			GwmAniAssert(0);
		}

		/* test */
		errResult = GWM_RET_OK;
	}

	return errResult;
}

ONDK_Result_t	gwm_ani_DrawingObjToCanvas(hViewHandle_t hCurrentView)
{
	ONDK_Rect_t					objRect;
	ONDK_Region_t					appArea;

	ONDK_Result_t				errResult = ERR_FAIL;
	GWM_Animation_View_t		*pView = NULL;
	ONDK_GWM_GuiObject_t				pObj = 0;
	GWM_ObjDraw_t				pfnObjCustomDrawMethod = 0;

	HINT32						nObjId = 0;

	pView = (GWM_Animation_View_t *)hCurrentView;

	if(pView->focus != TRUE)
	{
		GwmAniAssert(0);
		return GWT_RET_ALREADY_STOP;
	}

	if(pView->ObjList != NULL)
	{
		pObj = pView->ObjList;
		while(pObj)
		{
			nObjId = ONDK_GWM_Obj_GetObjectID(pObj);
			ONDK_GWM_Obj_GetObjectRect(nObjId, &objRect);
			ONDK_GWM_APP_GetAppArea(ONDK_GWM_APP_GetThis(), &appArea);
			pfnObjCustomDrawMethod = GWM_Obj_GetCustomDrawMethod(pObj);
			pfnObjCustomDrawMethod(pView->Canvas, &appArea, &objRect, pObj, pObj, 1);
		}
	}

	return errResult;
}

HINT32		GWM_Ani_GetAnimationCount(void)
{
	return s_nCountOfOnGoingAnimation;
}

HBOOL		GWM_Ani_IsAnimation(void)
{
	return s_bAnimationGo;
}

ONDK_Result_t		GWM_Ani_SetDoAnimationStart(void)
{
	if(s_bAnimationGo != FALSE)
	{
		return GWT_RET_ALREADY_START;
	}

	s_bAnimationGo = TRUE;

	return ERR_OK;
}

ONDK_Result_t		GWM_Ani_SetDoAnimationStop(void)
{
	if(s_bAnimationGo != FALSE)
	{
		return GWT_RET_ALREADY_STOP;
	}

	s_bAnimationGo = FALSE;

	return ERR_OK;
}

hViewHandle_t		GWM_Ani_GetAniView(ONDK_GWM_App_t app)
{
	return gwm_app_GetAppView(app);
}

hViewHandle_t		GWM_Ani_GetCurAppAniView(void)
{
	return gwm_app_GetAppView(ONDK_GWM_APP_GetThis());
}

ONDK_Result_t	GWM_Ani_DefaultAnimationProc(HINT32 methodMsg, HINT32 param1, HINT32 param2, HINT32 param3)
{
	switch(methodMsg)
	{
		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			switch(param1)
			{
				case KEY_ARROWLEFT:
					break;

				case KEY_ARROWRIGHT:
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}

	return ERR_OK;
}

ONDK_Result_t	GWM_Ani_ReleaseView(GWM_Animation_View_t *pView)
{
	gwm_app_ReleaseAppView((hViewHandle_t)pView);
	s_nCountOfOnGoingAnimation --;

	return ERR_OK;
}

ONDK_Result_t	GWM_Ani_SetView(GWM_Animation_View_t *pView)
{
	GWM_Animation_View_t	*pTmpView = NULL;

	gwm_app_SetAppView(ONDK_GWM_APP_GetThis(), (hViewHandle_t)pView);
	s_nCountOfOnGoingAnimation ++;
}

ONDK_Result_t	GWM_Ani_RegisterAtObj(GWM_Animation_View_t *pView, HINT32 nGWMObjId)
{
	ONDK_GWM_GuiObject_t objListOfCurApp;

	objListOfCurApp = INT_ONDK_GWM_APP_GetObjectList(ONDK_GWM_APP_GetThis());
	pView->ObjList = INT_ONDK_GWM_OBJ_FindObject(objListOfCurApp, nGWMObjId);
	if(pView->ObjList == NULL)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

/* end of file */

