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
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


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

/**********************************************************************************************************/
#define ____O2ANIMATION__FUNCTION____
/**********************************************************************************************************/
#include <ondk.h>
#include <ondk_gfx.h>
#include <math.h>

#include "ondk_gwm_internal.h"

#if defined(CONFIG_GWM_O2ANIMATION)
#define CONFIG_GWM_O2V2_ANIMATION
#endif

#if defined(CONFIG_GWM_O2V2_ANIMATION)
typedef double		ANI_DOUBLE_t;

#ifndef ABS
#define ABS(s) 		((s) < 0 ? -(s) : (s))
#endif

#define ANI_PI			((ANI_DOUBLE_t)3.14159265)
#define ANI_PI_ANGLE	((ANI_DOUBLE_t)((ANI_PI) / 180.0f))
#define ANI_MOVE_MIN(v)	((v) < 1 ? 1 : v)
#define ANI_MIN(a,b) 	(((a)<(b))?(a):(b))
#define ANI_MOVE_SIN(f)	((ANI_DOUBLE_t)sin(ANI_PI_ANGLE*(ANI_DOUBLE_t)f))
#define ANI_MOVE_COS(f)	((ANI_DOUBLE_t)cos(ANI_PI_ANGLE*(ANI_DOUBLE_t)f))
#define GWM_O2ANI_MOVE_CALC(cur, start, target, angle)	((start!=target) ? ((((ANI_DOUBLE_t)ABS(start-cur))*angle) / ((ANI_DOUBLE_t)(start-target))) : angle)

#define	ANI_TYPEUSER_DEF_ENDVAL				150
#define ANI_TYPEUSER_DEF_INTERVAL			10

typedef struct	tagGWM_O2AniMgr_t
{
	ONDK_Rect_t		rtOSD;
	ONDK_Region_t	rtOSDArea;
	HxList_t	*pAniList; /* GWM_O2AniItem_t */
} GWM_O2AniMgr_t;

STATIC GWM_O2AniMgr_t	*s_pstGWMO2AniMgr = NULL;


#define ______O2ANI_PRIVATE_FUNC__

STATIC GWM_O2AniMgr_t *local_gwmo2ani_GetO2AniMgr(void)
{
	return s_pstGWMO2AniMgr;
}

STATIC HINT32 local_gwmo2ani_MovePointSin(HINT32 cur, HINT32	st, HINT32 end, HINT32 speed, HINT32 tick)
{
	ANI_DOUBLE_t cSpeed, cPer;
	ANI_DOUBLE_t value = 0.0f;

	if(end != st && cur != end)
	{
		cPer = GWM_O2ANI_MOVE_CALC(cur, st, end, 180.0f);
		cSpeed = ANI_MOVE_SIN(cPer);
		value = (cSpeed*tick*speed) / 100;
		//value = ANI_MOVE_MIN(value);

		value = ANI_MIN((value), (ABS(end-cur)));
		if(end > st)
		{
			return value;
		}
		else
		{
			return (-(value));
		}
	}

	return 0;

}

STATIC ANI_DOUBLE_t local_gwmo2ani_MovePointCos(ANI_DOUBLE_t cur, ANI_DOUBLE_t	st, ANI_DOUBLE_t end, HINT32 speed, HINT32 tick)
{
	ANI_DOUBLE_t cSpeed, cPer;
	ANI_DOUBLE_t value = 0.0f;

	if(end != st && cur != end)
	{
		cPer = GWM_O2ANI_MOVE_CALC(cur, st, end, 90.0f);
		cSpeed = ANI_MOVE_COS(cPer);
		value = (cSpeed*tick*speed) / 100;
		//value = ANI_MOVE_MIN(value);

		value = ANI_MIN((value), (ABS(end-cur)));
		if(end > st)
		{
			return value;
		}
		else
		{
			return (-(value));
		}
	}

	return 0;
}

STATIC HBOOL local_gwmo2ani_CalcMove(ONDK_GWM_O2Ani_Item_t *pO2Item, const ONDK_Rect_t *pDrawMaxRect)
{
	HINT32		speed = 0;
	HINT32		tick = 0;
	HINT32		endCnt = 0;
	HBOOL		isEndAni = FALSE;
	ANI_DOUBLE_t	value = 0.0f;
	ANI_DOUBLE_t	per = 0.0f;
	ANI_DOUBLE_t	gap = 0.0f;
	HINT32		oldEnd = 0, oldStart = 0;

	if(ONDK_CmpRect(&pO2Item->stSettings.rtStart, &pO2Item->stSettings.rtEnd))
	{
		return TRUE;
	}

	speed = pO2Item->stSettings.nSpeed;
	tick = ((VK_TIMER_GetSystemTick() - pO2Item->utStartTime));

	/* x */
	oldEnd = pO2Item->stSettings.rtEnd.x;	oldStart = pO2Item->stSettings.rtStart.x;
	value = local_gwmo2ani_MovePointCos(pO2Item->x, pO2Item->stSettings.rtStart.x, pO2Item->stSettings.rtEnd.x, speed, tick);
	if(0 == value)
	{
		endCnt++;
	}
	else
	{
		pO2Item->x += value;
		pO2Item->rtCurrent.x = (HINT32)(pO2Item->x+0.5f);
	}

	/* y */
	if(0 != value && oldEnd != oldStart)
	{
		gap = (pO2Item->stSettings.rtEnd.y - pO2Item->stSettings.rtStart.y);
		per = (ABS(value) * gap) / ABS(oldEnd-oldStart);
		if(0 != per)
		{
			pO2Item->y += per;
			pO2Item->rtCurrent.y = (HINT32)(pO2Item->y+0.5f);
		}
	}
	else
	{
		oldEnd = pO2Item->stSettings.rtEnd.y;	oldStart = pO2Item->stSettings.rtStart.y;
		value = local_gwmo2ani_MovePointCos(pO2Item->y, pO2Item->stSettings.rtStart.y, pO2Item->stSettings.rtEnd.y, speed, tick);
		if(0 == value)
		{
			endCnt++;
		}
		else
		{
			pO2Item->y += value;
			pO2Item->rtCurrent.y = (HINT32)(pO2Item->y+0.5f);
		}
	}

	/* w */
	if(0 != value && oldEnd != oldStart)
	{
		gap = (pO2Item->stSettings.rtEnd.w - pO2Item->stSettings.rtStart.w);
		per = (ABS(value) * gap) / ABS(oldEnd-oldStart);
		if(0 != per)
		{

			pO2Item->w += per;
			pO2Item->rtCurrent.w = (HINT32)(pO2Item->w+0.5f);
		}
	}
	else
	{
		oldEnd = pO2Item->stSettings.rtEnd.w;	oldStart = pO2Item->stSettings.rtStart.w;
		value = local_gwmo2ani_MovePointCos(pO2Item->w, pO2Item->stSettings.rtStart.w, pO2Item->stSettings.rtEnd.w, speed, tick);
		if(0 == value)
		{
			endCnt++;
		}
		else
		{
			pO2Item->w += value;
			pO2Item->rtCurrent.w = (HINT32)(pO2Item->w+0.5f);
		}
	}

	/* h */
	if(0 != value && oldEnd != oldStart)
	{
		gap = (pO2Item->stSettings.rtEnd.h - pO2Item->stSettings.rtStart.h);
		per = (ABS(value) * gap) / ABS(oldEnd-oldStart);
		if(0 != per)
		{
			pO2Item->h += per;
			pO2Item->rtCurrent.h = (HINT32)(pO2Item->h+0.5f);
		}
	}
	else
	{
		value = local_gwmo2ani_MovePointCos(pO2Item->h, pO2Item->stSettings.rtStart.h, pO2Item->stSettings.rtEnd.h, speed, tick);
		if(0 == value)
		{
			endCnt++;
		}
		else
		{
			pO2Item->h += value;
			pO2Item->rtCurrent.h = (HINT32)(pO2Item->h+0.5f);
		};
	}

	if(ONDK_CmpRect(&pO2Item->rtCurrent, &pO2Item->stSettings.rtEnd))
	{
		isEndAni = TRUE;
	}

	return isEndAni;
}


STATIC void  local_gwmo2ani_UpdateAniItem(ONDK_GWM_O2Ani_Item_t	*pO2Item, const ONDK_Rect_t *pRect)
{
	ONDK_Region_t		base, current;
	ONDK_Rect_t 		updateRect;
	GWM_O2AniMgr_t		*pO2Mgr = local_gwmo2ani_GetO2AniMgr();

	updateRect = ONDK_IntersectRect(pRect, &pO2Mgr->rtOSD);
	if(ONDK_IsOverlapped(&updateRect, &pO2Mgr->rtOSD))
	{
		base = INT_ONDK_GWM_GetUpdateArea(pO2Item->ePosition);
		current = ONDK_RectToRegion(*pRect);

		INT_ONDK_GWM_SetUpdateArea(pO2Item->ePosition, ONDK_UnionRegion(&base, &current));
		INT_ONDK_GWM_SetHaveToRedraw(pO2Item->ePosition,TRUE);
	}
}


STATIC HBOOL local_gwmo2ani_CalcAlpha(ONDK_GWM_O2Ani_Item_t *pO2Item)
{
	HINT32			speed = 0;
	HINT32			tick = 0;
	HBOOL			isEndAni = FALSE;
	ANI_DOUBLE_t	value = 0;

	if(pO2Item->stSettings.nEndAlpha == pO2Item->stSettings.nStartAlpha)
	{
		return TRUE;
	}

	speed = pO2Item->stSettings.nSpeed;
	tick = ((VK_TIMER_GetSystemTick() - pO2Item->utStartTime));

	value = local_gwmo2ani_MovePointCos(pO2Item->a, pO2Item->stSettings.nStartAlpha, pO2Item->stSettings.nEndAlpha, speed, tick);
	if(value)
	{
		pO2Item->a += value;
		pO2Item->nCurAlpha = (HINT32)(pO2Item->a+0.5f);
	}


	if((HINT32)pO2Item->nCurAlpha == pO2Item->stSettings.nEndAlpha)
	{
		isEndAni = TRUE;
	}

	return isEndAni;

}

STATIC HBOOL local_gwmo2ani_CalcUser(ONDK_GWM_O2Ani_Item_t *pO2Item)
{
	pO2Item->stSettings.nUserEndVal -= pO2Item->stSettings.nUserInterval;

	if (pO2Item->nAniType & eONDK_O2ANITYPE_User)
	{
		if(pO2Item->stSettings.nUserEndVal <= 0)
			return TRUE;
		else
			return FALSE;
	}
	else
		return TRUE;		// user animation 없다면, 무조건 end animation 이라고 return.
}

STATIC HBOOL local_gwmo2ani_FindAniItem(void *user_data,void *list_data)
{
	HINT32					o2aniId = (HINT32)user_data;
	ONDK_GWM_O2Ani_Item_t	*pO2Item = list_data;

	if(pO2Item->nAniId == o2aniId
		&& pO2Item->pApp == g_pstCurrentApp)
	{
		return TRUE;
	}

	return FALSE;
}

STATIC HBOOL local_gwmo2ani_FindAniItemByApp(void *user_data, void *list_data)
{
	ONDK_GWM_App_t		pApp = (ONDK_GWM_App_t)user_data;
	ONDK_GWM_O2Ani_Item_t	*pstAniItem = list_data;

	if (pstAniItem->pApp == pApp)
		return TRUE;

	return FALSE;
}

STATIC ONDK_Result_t	local_gwmo2ani_Remove(HINT32 o2aniId)
{
	HxList_t				*pList = NULL;
	ONDK_GWM_O2Ani_Item_t	*pO2Item = NULL;
	GWM_O2AniMgr_t			*pO2Mgr = local_gwmo2ani_GetO2AniMgr();
	ONDK_Rect_t 			rt;

	pList = HLIB_LIST_FindEx(pO2Mgr->pAniList, (void*)o2aniId, local_gwmo2ani_FindAniItem);
	if(NULL == pList)
	{
		return ONDK_RET_INVARG;
	}

	pO2Item = HLIB_LIST_Data(pList);
	ONDK_Assert(pO2Item);

	rt = ONDK_UnionRect(&pO2Item->stSettings.rtStart, &pO2Item->stSettings.rtEnd);

	local_gwmo2ani_UpdateAniItem(pO2Item, &rt);

	pO2Mgr->pAniList = HLIB_LIST_Remove(pO2Mgr->pAniList, pO2Item);
	ONDK_GWM_SendMessage(ONDK_GWM_APP_GetAppCallback(pO2Item->pApp), MSG_GWM_O2ANIMATION_END, 0, pO2Item->nLastKey, pO2Item->nAniId, 0);
	ONDK_Free(pO2Item);

	return GWM_RET_OK;
}

STATIC	void	local_gwmo2ani_CheckSettingsByAniType(ONDK_GWM_O2Ani_Settings_t *pstSettings)
{
	if (pstSettings->nAniType & eONDK_O2ANITYPE_Proc)
	{
		// 시작 및 마지막 의 alpha 가 0 이면 안됨.. ondk blend 에서 에러처리함.
		if (pstSettings->nStartAlpha <= 0)
			pstSettings->nStartAlpha = 1;
		pstSettings->nSpeed = 17;
	}

	if (pstSettings->nAniType & eONDK_O2ANITYPE_ExitProc)
	{
		// 시작 및 마지막 의 alpha 가 0 이면 안됨.. ondk blend 에서 에러처리함.
		if (pstSettings->nEndAlpha <= 0)
			pstSettings->nEndAlpha = 1;
		pstSettings->nSpeed = 17;
	}
}

STATIC void		local_gwmo2ani_CheckAniItemByAniType(ONDK_GWM_O2Ani_Item_t *pstAddItem)
{
	if ((pstAddItem->nAniType & eONDK_O2ANITYPE_Proc) || (pstAddItem->nAniType & eONDK_O2ANITYPE_ExitProc))
	{
		ONDK_GWM_O2Ani_Item_t *pstFoundItem = INT_ONDK_GWM_O2ANI_GetProcAnimationItem(pstAddItem->pApp);
		// 중간에 치고 들어오는 경우..
		if (pstFoundItem != NULL)	// 이미 proc 이나 exit proc ani 가 실행중이라면, return false
		{
			ONDK_GWM_Callback_t gwmCallback = NULL;
			GWM_O2AniMgr_t		*pO2Mgr = local_gwmo2ani_GetO2AniMgr();

			pstAddItem->stSettings.drawProcRect = pstFoundItem->stSettings.drawProcRect;
			pstAddItem->nCurAlpha = pstFoundItem->nCurAlpha;
			pstAddItem->stSettings.nStartAlpha = pstFoundItem->stSettings.nStartAlpha;
			pstAddItem->stSettings.nEndAlpha = pstFoundItem->stSettings.nEndAlpha;
			pstAddItem->a = pstFoundItem->a;

			gwmCallback = ONDK_GWM_APP_GetAppCallback(pstFoundItem->pApp);
			pO2Mgr->pAniList = HLIB_LIST_Remove(pO2Mgr->pAniList, pstFoundItem);
			ONDK_GWM_PostMessage(gwmCallback, MSG_GWM_O2ANIMATION_END, 0, pstFoundItem->nLastKey, pstFoundItem->nAniId, 0);
			ONDK_Free(pstFoundItem);
		}
	}
}

#endif

#define ______O2ANI_INTERNAL_FUNC__

#if defined(CONFIG_GWM_O2V2_ANIMATION)

HBOOL INT_ONDK_GWM_O2ANI_IsAppAnimation(void)
{
	HBOOL				isMoveComplete 	= FALSE;
	HBOOL				isAlphaComplete = FALSE;
	HBOOL				isUserComplete 	= FALSE;
	HxList_t			*pList = NULL;
	GWM_O2AniMgr_t		*pO2Mgr = local_gwmo2ani_GetO2AniMgr();
	ONDK_Rect_t 		rt;
	ONDK_GWM_Callback_t	gwmCallback = NULL;
	HUINT32				utStart = 0;
	ONDK_GWM_O2Ani_Item_t	*pO2Item = NULL;
#if defined(CONFIG_DEBUG) && defined(CONFIG_OS_PROD_LINUX)
	HUINT32				utEnd = 1000;
#endif

	pList = pO2Mgr->pAniList;
	if(NULL == pList)
	{
		return FALSE;
	}

	while(pList)
	{
		isMoveComplete = FALSE;
		isAlphaComplete = FALSE;
		pO2Item = HLIB_LIST_Data(pList);
		ONDK_Assert(pO2Item);

		rt = (pO2Item->rtCurrent);

		isMoveComplete = local_gwmo2ani_CalcMove(pO2Item, &pO2Mgr->rtOSD);
		isAlphaComplete = local_gwmo2ani_CalcAlpha(pO2Item);
		isUserComplete = local_gwmo2ani_CalcUser(pO2Item);

		rt = ONDK_UnionRect(&rt, &pO2Item->rtCurrent);

		local_gwmo2ani_UpdateAniItem(pO2Item, &rt);

		pList = HLIB_LIST_Next(pList);

		gwmCallback = ONDK_GWM_APP_GetAppCallback(pO2Item->pApp);
		if(NULL == gwmCallback)
		{
			continue;
		}

		if(pO2Item->stSettings.bNotify)
		{
			utStart = VK_TIMER_GetSystemTick();
			ONDK_GWM_SendMessage(gwmCallback, MSG_GWM_O2ANIMATION_UPDATE, 0, pO2Item->nAniId, (HINT32)&pO2Item->rtCurrent, pO2Item->nCurAlpha);
			#if defined(CONFIG_DEBUG) && defined(CONFIG_OS_PROD_LINUX)
			utEnd = VK_TIMER_GetSystemTick();
			if(utStart + 20 < utEnd)
			{
				ONDK_Error("\r\n\r\n============================================================\r\n");
				ONDK_Error("[%s:%d] Check Message Time!! app(%s, 0x%x) ani_id(%d)\r\n", __FUNCTION__, __LINE__, ONDK_GWM_APP_GetAppName(pO2Item->pApp), (HUINT32)gwmCallback, pO2Item->nAniId);
				ONDK_Error("=============================================================\r\n\r\n");
				ONDK_Assert(0);
			}
			#endif
		}

		if(isMoveComplete && isAlphaComplete && isUserComplete)
		{
			pO2Mgr->pAniList = HLIB_LIST_Remove(pO2Mgr->pAniList, pO2Item);
			ONDK_GWM_PostMessage(gwmCallback, MSG_GWM_O2ANIMATION_END, 0, pO2Item->nLastKey, pO2Item->nAniId, 0);
			ONDK_Free(pO2Item);
		}
	}

	return TRUE;
}

ONDK_Result_t	  INT_ONDK_GWM_O2ANI_Init(void)
{
	if(s_pstGWMO2AniMgr)
	{
		ONDK_Error("[%s:%d] Duplication Init\r\n", __FUNCTION__, __LINE__);
		return GWM_RET_OK;
	}

	s_pstGWMO2AniMgr = ONDK_Calloc(sizeof(GWM_O2AniMgr_t));
	ONDK_Assert(s_pstGWMO2AniMgr);

	s_pstGWMO2AniMgr->rtOSD = ONDK_Rect(0, 0, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT);

	return ONDK_RET_OK;
}

ONDK_GWM_O2Ani_Item_t *	INT_ONDK_GWM_O2ANI_GetProcAnimationItem(ONDK_GWM_App_t pApp)
{
	HxList_t				*pList = NULL;
	ONDK_GWM_O2Ani_Item_t	*pstAniItem = NULL;
	GWM_O2AniMgr_t			*pO2Mgr = local_gwmo2ani_GetO2AniMgr();

	pList = HLIB_LIST_FindEx(pO2Mgr->pAniList, (void*)pApp, local_gwmo2ani_FindAniItemByApp);

	if(NULL == pList)
	{
		return NULL;
	}

	pstAniItem = HLIB_LIST_Data(pList);

	if (pstAniItem != NULL &&
		((pstAniItem->nAniType & eONDK_O2ANITYPE_Proc) || (pstAniItem->nAniType & eONDK_O2ANITYPE_ExitProc)))
	{
		return pstAniItem;
	}
	return NULL;
}

ONDK_Result_t	INT_ONDK_GWM_O2ANI_UpdateProcAnimation(ONDK_GWM_O2Ani_Item_t *pstProcAniItem, ONDKSurfaceHandle srcSurface, ONDKSurfaceHandle dstSurface)
{
	HBOOL	bExitProcEndAni = FALSE;
	ONDK_Rect_t drawRect = ONDK_Rect(0, 0, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT);

	if (pstProcAniItem == NULL)
		return ONDK_RET_FAIL;

	if (pstProcAniItem->nAniType & eONDK_O2ANITYPE_Proc)
	{
		pstProcAniItem->stSettings.drawProcRect.x -= 64;
		pstProcAniItem->stSettings.drawProcRect.y -= 36;
		pstProcAniItem->stSettings.drawProcRect.w += 128;
		pstProcAniItem->stSettings.drawProcRect.h += 72;

		if (pstProcAniItem->stSettings.drawProcRect.x <= 0)
		{
			pstProcAniItem->nCurAlpha = pstProcAniItem->stSettings.nEndAlpha;
			pstProcAniItem->stSettings.drawProcRect = ONDK_Rect(0, 0, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT);
		}
	}
	else if (pstProcAniItem->nAniType & eONDK_O2ANITYPE_ExitProc)
	{
		pstProcAniItem->stSettings.drawProcRect.x += 64;
		pstProcAniItem->stSettings.drawProcRect.y += 36;
		pstProcAniItem->stSettings.drawProcRect.w -= 128;
		pstProcAniItem->stSettings.drawProcRect.h -= 72;

		if (pstProcAniItem->stSettings.drawProcRect.x >= 576)
		{
			pstProcAniItem->nCurAlpha = 1; // 0 으로 하면 안된다... ONDK Blend 가 에러처리함.
			pstProcAniItem->stSettings.drawProcRect = ONDK_Rect((640-64), (360-36), 64, 36);
			bExitProcEndAni = TRUE;
		}
	}

	ONDK_SURFACE_Clear(dstSurface);
	ONDK_SURFACE_Blitflag_SetAlphaBlend(dstSurface, pstProcAniItem->nCurAlpha);
	ONDK_SURFACE_StretchBlitWithoutBlend(srcSurface, dstSurface, drawRect, pstProcAniItem->stSettings.drawProcRect);

	// exit proc ani 의 경우, main loop 보다 미리 proc 종료를 위해...
	if (bExitProcEndAni)
	{
		ONDK_GWM_Callback_t gwmCallback = ONDK_GWM_APP_GetAppCallback(pstProcAniItem->pApp);
		GWM_O2AniMgr_t			*pO2Mgr = local_gwmo2ani_GetO2AniMgr();
		pO2Mgr->pAniList = HLIB_LIST_Remove(pO2Mgr->pAniList, pstProcAniItem);
		ONDK_GWM_PostMessage(gwmCallback, MSG_GWM_O2ANIMATION_END, 0, pstProcAniItem->nLastKey, pstProcAniItem->nAniId, 0);
		ONDK_Free(pstProcAniItem);
	}

	return ONDK_RET_OK;
}


#else

HBOOL INT_ONDK_GWM_O2ANI_IsAppAnimation(void)
{
	return FALSE;
}
ONDK_Result_t	  INT_ONDK_GWM_O2ANI_Init(void)
{
	return ONDK_RET_OK;
}

ONDK_GWM_O2Ani_Item_t *	INT_ONDK_GWM_O2ANI_GetProcAnimationItem(ONDK_GWM_App_t pApp)
{
	return NULL;
}

ONDK_Result_t	INT_ONDK_GWM_O2ANI_UpdateProcAnimation(ONDK_GWM_O2Ani_Item_t *pstProcAniItem, ONDKSurfaceHandle srcSurface, ONDKSurfaceHandle dstSurface)
{
	return ONDK_RET_OK;
}

#endif

#define ______O2ANI_PUBLIC_FUNC__
ONDK_Result_t	  ONDK_GWM_O2ANI_GetValue(HINT32 o2aniId, ONDK_Rect_t *ret_pDrawRect, HINT32 *ret_alpha)
{
#if defined(CONFIG_GWM_O2V2_ANIMATION)
	HxList_t				*pList = NULL;
	GWM_O2AniMgr_t			*pO2Mgr = local_gwmo2ani_GetO2AniMgr();
	ONDK_GWM_O2Ani_Item_t	*pO2Item = NULL;

	pList = HLIB_LIST_FindEx(pO2Mgr->pAniList, (void*)o2aniId, local_gwmo2ani_FindAniItem);
	if(NULL == pList)
	{
		return GWM_RET_NO_OBJECT;
	}

	pO2Item = HLIB_LIST_Data(pList);
	ONDK_Assert(pO2Item);

	if(ret_pDrawRect)
	{
		*ret_pDrawRect = pO2Item->rtCurrent;
	}

	if(ret_alpha)
	{
		*ret_alpha = (pO2Item->nCurAlpha);
	}

	return ONDK_RET_OK;
#else
	(void)o2aniId;
	(void)ret_pDrawRect;
	(void)ret_alpha;
	return GWM_RET_FAIL;
#endif
}


ONDK_Result_t	 ONDK_GWM_O2ANI_Remove(HINT32 o2aniId)
{
#if defined(CONFIG_GWM_O2V2_ANIMATION)
	return local_gwmo2ani_Remove(o2aniId);
#else
	(void)o2aniId;
	return GWM_RET_FAIL;
#endif
}

void	ONDK_GWM_O2ANI_GetDefaultSettings(HINT32 nAniType/*ONDK_GWM_O2Ani_Type_e*/, ONDK_GWM_O2Ani_Settings_t *pstO2AniSettings, ONDK_Rect_t *pOrgRect)
{
#if defined(CONFIG_GWM_O2V2_ANIMATION)
	ONDK_Assert(pstO2AniSettings);

	HxSTD_MemSet(pstO2AniSettings, 0, sizeof(ONDK_GWM_O2Ani_Settings_t));
	if(pOrgRect)
	{
		pstO2AniSettings->rtStart	= *pOrgRect;
		pstO2AniSettings->rtEnd 	= *pOrgRect;
	}
	pstO2AniSettings->nSpeed	= 7;
	pstO2AniSettings->nEndAlpha = 0xFF;
	pstO2AniSettings->nStartAlpha = 0xFF;
	pstO2AniSettings->bNotify = TRUE;

	pstO2AniSettings->nAniType = nAniType;

	if (nAniType & eONDK_O2ANITYPE_User)
	{
		pstO2AniSettings->nUserEndVal  	= ANI_TYPEUSER_DEF_ENDVAL;
		pstO2AniSettings->nUserInterval = ANI_TYPEUSER_DEF_INTERVAL;
	}

	if (nAniType & eONDK_O2ANITYPE_Proc)
	{
		pstO2AniSettings->drawProcRect = ONDK_Rect(640, 360, 0, 0);
	}

	if (nAniType & eONDK_O2ANITYPE_ExitProc)
	{
		pstO2AniSettings->drawProcRect = ONDK_Rect(0, 0, GWM_SCREEN_WIDTH, GWM_SCREEN_HEIGHT);
	}

#else
	(void)pstO2AniSettings;
	(void)pOrgRect;
#endif
}

ONDK_Result_t	 ONDK_GWM_O2ANI_AddItem(HINT32 o2aniId, ONDK_GWM_O2Ani_Settings_t *pstO2AniSettings, HINT32 nLastKey)
{
#if defined(CONFIG_GWM_O2V2_ANIMATION)
	GWM_O2AniMgr_t			*pO2Mgr = local_gwmo2ani_GetO2AniMgr();
	ONDK_GWM_O2Ani_Item_t	*pO2Item = NULL;

	ONDK_Assert(g_pstCurrentApp);
	ONDK_Assert(pstO2AniSettings);

	local_gwmo2ani_Remove(o2aniId);

	pO2Item = (ONDK_GWM_O2Ani_Item_t *)ONDK_Calloc(sizeof(ONDK_GWM_O2Ani_Item_t));
	if(NULL == pO2Item)
	{
		ONDK_Assert(pO2Item);
		return GWM_RET_OUT_OF_MEMORY;
	}

	local_gwmo2ani_CheckSettingsByAniType(pstO2AniSettings);

	HxSTD_MemCopy(&pO2Item->stSettings, pstO2AniSettings, sizeof(ONDK_GWM_O2Ani_Settings_t));
	pO2Item->pApp = g_pstCurrentApp;
	pO2Item->nAniId = o2aniId;
	pO2Item->nCurAlpha = pstO2AniSettings->nStartAlpha;
	pO2Item->utStartTime = VK_TIMER_GetSystemTick();
	pO2Item->rtCurrent = pstO2AniSettings->rtStart;
	pO2Item->x = pO2Item->rtCurrent.x;
	pO2Item->y = pO2Item->rtCurrent.y;
	pO2Item->w = pO2Item->rtCurrent.w;
	pO2Item->h = pO2Item->rtCurrent.h;
	pO2Item->a = pstO2AniSettings->nStartAlpha;
	pO2Item->nLastKey = nLastKey;
	pO2Item->ePosition = ONDK_GWM_GetCurPosition();

	pO2Item->nAniType = pstO2AniSettings->nAniType;

	local_gwmo2ani_CheckAniItemByAniType(pO2Item);

	pO2Mgr->pAniList = HLIB_LIST_Append(pO2Mgr->pAniList, pO2Item);

	ONDK_GWM_APP_InvalidateONDKRect(&pO2Item->rtCurrent);

	return GWM_RET_OK;
#else
	(void)o2aniId;
	(void)pstO2AniSettings;
	(void)nLastKey;
	return GWM_RET_FAIL;
#endif
}

