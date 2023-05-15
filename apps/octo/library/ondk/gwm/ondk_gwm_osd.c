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
*/
/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include 	<ondk.h>




/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/

/**********************************************************************************************************/
#define ____DEBUG_FUNCTIONS_______________________
/**********************************************************************************************************/
#if defined(CONFIG_DEBUG)
STATIC HBOOL	s_bIsInvalidRectColor = FALSE;
#endif


void	DBG_ONDK_GWM_GFX_UpdateScreenShowLine(void)
{
#if defined(CONFIG_DEBUG)
	s_bIsInvalidRectColor = !s_bIsInvalidRectColor;
	if(s_bIsInvalidRectColor)
	{
		HLIB_CMD_Printf("Enable Rect Color\r\n");
	}
	else
	{
		HLIB_CMD_Printf("Disable Rect Color\r\n");
	}
#endif
}


/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/


void	INT_ONDK_GWM_SetClip(ONDKSurfaceHandle scr, HINT32 x1, HINT32 y1, HINT32 x2, HINT32 y2)
{
	ONDK_Assert(scr);
	ONDK_SURFACE_SetClip(scr, ONDK_Region(x1, y1, x2, y2));
}


void	INT_ONDK_GWM_GetClipElement(ONDKSurfaceHandle scr, HINT32 *x1, HINT32 *y1, HINT32 *x2, HINT32 *y2)
{
	ONDK_Assert(scr);
	ONDK_Assert(x1);
	ONDK_Assert(y1);
	ONDK_Assert(x2);
	ONDK_Assert(y2);
	ONDK_SURFACE_GetClipElement(scr, x1, y1, x2, y2);
}

void	INT_ONDK_GWM_GetClipRect(ONDKSurfaceHandle scr, ONDK_Rect_t *pstOsdRect)
{
	ONDK_Region_t region;

	ONDK_Assert(scr);
	ONDK_Assert(pstOsdRect);

	ONDK_SURFACE_GetClip(scr, &region);

	*pstOsdRect = ONDK_RegionToRect(region);
}

HBOOL	INT_ONDK_GWM_IsClippedArea(ONDKSurfaceHandle scr, int x1, int y1, int x2, int y2)
{
	ONDK_Region_t clip_region;
	ONDK_Region_t in_region = ONDK_Region(x1,y1,x2,y2);

	ONDK_Assert(scr);

	ONDK_SURFACE_GetClip(scr, &clip_region);

	return	!ONDK_IsOverlappedRegion(&in_region, &clip_region);
}


void 	ONDK_GWM_GFX_ClearScreenByRegion(ONDKSurfaceHandle pixmap, ONDK_Region_t *area)
{
	ONDK_Print("[%s:%d] Clear area(%d, %d, %d, %d)\r\n", __FUNCTION__, __LINE__, area->x1, area->y1, area->y1, area->y2);
	//ONDK_WINDOW_SetShow(pixmap, FALSE);
 	//ONDK_DRAW_FillRect(pixmap, ONDK_RegionToRect(*area), ONDK_Color(0,0,0,0));
 	ONDK_SURFACE_SetClip(pixmap, *area);
 	ONDK_SURFACE_Clear(pixmap);
}

ONDK_Result_t	ONDK_GWM_GFX_UpdateScreen(ONDK_WINDOW_Position_e ePosition,ONDK_Region_t Area)
{
	ONDK_Rect_t rt = ONDK_RegionToRect(Area);
	ONDK_Print("[%s:%d] Update Screen (%d) Rect(%d, %d, %d, %d) \r\n", __FUNCTION__, __LINE__, HLIB_STD_GetSystemTick(), rt.x, rt.y, rt.w, rt.h);

#if defined(CONFIG_DEBUG)
	if(s_bIsInvalidRectColor)
	{
		HUINT32 ulColor = 0xFF000000;

		ulColor |= (HLIB_MATH_Random()  * HLIB_MATH_Random());
		ONDK_DRAW_Rect(ONDK_GWM_GetSurface(ePosition), rt, ulColor);
	}
#endif

	ONDK_WINDOW_SetShow(ONDK_GWM_GetWindow(ePosition), TRUE);
	ONDK_WINDOW_Update(ONDK_GWM_GetWindow(ePosition), &rt);

	//ONDK_WINDOW_Blit(ONDK_GWM_GetWindow(), pstOsdScr, rt, pArea->x1, pArea->y1);
	//ONDK_WINDOW_Update(ONDK_GWM_GetWindow(), NULL);


	//ONDK_SURFACE_FlipBlit(pstOsdScr, &rt);
	//ONDK_WINDOW_UpdateFlipOnSync(ONDK_GWM_GetWindow(), &rt);

	return ONDK_RET_OK;
}

ONDK_Result_t	ONDK_GWM_GFX_SyncScreen(ONDKSurfaceHandle pstOsdScr, ONDK_Rect_t *rc)
{
	ONDK_SURFACE_FlipWithForSync(pstOsdScr, rc);

	return ONDK_RET_OK;
}

ONDK_Color_t 		ONDK_GWM_GFX_GetSysDimmedColor(ONDKSurfaceHandle pstScreen, ONDK_Color_t srcColor, HINT32 level)
{
	HINT32 alpha = COL_A(srcColor);
	HINT32 r = COL_R(srcColor);
	HINT32 g = COL_G(srcColor);
	HINT32 b = COL_B(srcColor);
#if 0
	alpha = alpha * (100 + level) / 100 ;
	if(alpha > 255) alpha = 255;
#endif
	r = r * (level) / 100 ;
	if(r > 255) r = 255;

	g = g * (level) / 100 ;
	if(g > 255) g = 255;

	b = b * (level) / 100 ;
	if(b > 255) b = 255;

	return (ONDK_Color_t)(((alpha&0xFF)<<24) | ((r&0xFF)<<16) | ((g&0xFF)<<8) | (b&0xFF));
}

ONDK_Color_t 		ONDK_GWM_GFX_GetSysDimmedAlpha(ONDKSurfaceHandle pstScreen, ONDK_Color_t srcColor, HUINT8 darkness)
{
	HINT32 alpha = COL_A(srcColor);

	alpha = alpha * darkness / 100;
	if(alpha > 255) alpha = 255;

	return (ONDK_Color_t)(((alpha&0xFF)<<24) | (srcColor & 0x00FFFFFF));
}

HINT32		ONDK_GWM_GFX_CountMultiLine(ONDKFontHandle font, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HUINT8 *string, HINT32 vspace)
{
	HINT32 out_area_width = 0;
	HINT32 out_area_height = 0;

	return ONDK_FONT_GetWidthMultiLine(font, string, w,vspace,  &out_area_width, &out_area_height);
}

HINT32		ONDK_GWM_GFX_DrawMultiLineString(ONDKSurfaceHandle  pixmap, ONDKFontHandle font, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HCHAR *string, HUINT32 vspace, ONDK_Color_t color)
{
	return ONDK_FONT_DrawStringForMultiLine(pixmap, font, x, y, w, h, string, vspace, color);
}

HINT32		ONDK_GWM_GFX_DrawSingleLineString(ONDKSurfaceHandle  pixmap, ONDKFontHandle font, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HCHAR *string, HUINT32 vspace, ONDK_Color_t color)
{
	ONDK_FONT_DrawStringAbbr(pixmap, font, string, x, y, w, color);
	return	ONDK_RET_OK;
}

HINT32		ONDK_GWM_GFX_GetStringWidth(ONDKFontHandle font, const HCHAR *string)
{
    return ONDK_FONT_GetStringWidth(font, (HCHAR *)string);
}

void			ONDK_GWM_GFX_DrawFrame(ONDKSurfaceHandle pixmap, HINT32 x, HINT32 y, HINT32 w, HINT32 h, HINT32 bright, HINT32 face, HINT32 shadow)
{
	ONDK_DRAW_FillRectElement(pixmap, x, y, w, h, bright);
	ONDK_DRAW_FillRectElement(pixmap, x+3, y+3, w-5, h-5, face);
	ONDK_DRAW_FillRectElement(pixmap, (x+2), (y+h-3), (w-3), 2, shadow);
	ONDK_DRAW_FillRectElement(pixmap, (x+w-3), (y+2), 2, (h-3), shadow);
}


HINT32				ONDK_GWM_GFX_DrawBitmap(ONDKSurfaceHandle pixmap, HINT32 x, HINT32 y, HUINT8 *bmp_ptr, OSD_BmpBlendMode_t mode)
{
	ONDK_IMG_Blit(pixmap, bmp_ptr, x, y);
	return ONDK_RET_OK;
}


/****************************************************************************************************/
/* FONT 모듈 재 구현 후 api 정리 예정 */
/****************************************************************************************************/



void		OSD_GFX_DrawStringWithAbbr(ONDKSurfaceHandle  pixmap, ONDKFontHandle font, HINT32 x, HINT32 y, HINT32 w, HUINT8 *string, ONDK_Color_t color)
{
	ONDK_FONT_DrawStringAbbr(pixmap, font, string, x, y, w, color);
}



