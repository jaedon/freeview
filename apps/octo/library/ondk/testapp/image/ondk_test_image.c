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






/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
#define ONDK_TEST_PARAM_WARRING_REMOVE(arg1, arg2, arg3, arg4)			\
			{															\
			(void)arg1;													\
			(void)arg2;													\
			(void)arg3;													\
			(void)arg4;							 						\
			}


#define DEF_ONDK_TEST_IMAGE_X  0
#define DEF_ONDK_TEST_IMAGE_Y  0
#define DEF_ONDK_TEST_IMAGE_W  1280
#define DEF_ONDK_TEST_IMAGE_H  720

#define DEF_ONDK_TEST_IMAGE_DATA_X  20
#define DEF_ONDK_TEST_IMAGE_DATA_Y  200
#define DEF_ONDK_TEST_IMAGE_DATA_W  1220
#define DEF_ONDK_TEST_IMAGE_DATA_H  500

#define DEF_ONDK_TEST_IMAGE_BTN_X  10
#define DEF_ONDK_TEST_IMAGE_BTN_Y  10
#define DEF_ONDK_TEST_IMAGE_BTN_W  120
#define DEF_ONDK_TEST_IMAGE_BTN_H  60


#define DEF_ONDK_TEST_IMAGE_371_H_SET_C_PNG    "/usr/image/371_H_SET_C.png"
#define DEF_ONDK_TEST_IMAGE_371_H_SET_N_PNG    "/usr/image/371_H_SET_N.png"

#define DEF_ONDK_TEST_IMAGE_VLINE_PNG		"/usr/image/104_Background_R.png"
#define DEF_ONDK_TEST_IMAGE_HLINE_PNG		"/usr/image/105_BG_Bottom_90.png"


#define DEF_ONDK_TEST_IMAGE_TIMER_ID	0x820610

typedef enum tagTESTIMAGE_Object
{
	eTESTIMAGE_Object_FRAME_Background = 0x991,
	eTESTIMAGE_Object_RECT_FRAME,
	eTESTIMAGE_Object_RECT_Data,

	eTESTIMAGE_Object_BTN_START,
	eTESTIMAGE_Object_BTN_SCALE,
	eTESTIMAGE_Object_BTN_CLIP,
	eTESTIMAGE_Object_BTN_END,

	eTESTIMAGE_Object_IMAGE_START,
	eTESTIMAGE_Object_IMAGE_END,

	eTESTIMAGE_Object_BTN_MAX
}TESTIMAGE_Object_e;


typedef struct tagTESTIMAGE_Contents_t
{
	HxList_t	*pList; /* TESTIMAGE_Shooting_t */
	HCHAR			szText[2048];
	ONDK_Rect_t 	boxRect;
	ONDK_Rect_t 	shootRect;
	ONDKImageHandle	hBoxImg;
	ONDKImageHandle	hShootImg;
	HUINT32			sysLastTick;
} TESTIMAGE_Contents_t;

STATIC TESTIMAGE_Contents_t *s_pstTestFontContents = NULL;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/


STATIC TESTIMAGE_Contents_t *local_testimage_GetContents(void)
{
	if(NULL == s_pstTestFontContents)
	{
		s_pstTestFontContents = ONDK_Calloc(sizeof(TESTIMAGE_Contents_t));
	}
	return s_pstTestFontContents;
}


STATIC void local_testimage_SetUiObject_CreateButton(HUINT32 id, HCHAR *name, ONDK_Rect_t rect)
{
	ONDK_GWM_COM_Button_Create(id , &rect, name);
	ONDK_GWM_COM_Button_SetTitleSize(id , 14);
	ONDK_GWM_COM_Button_SetFontColor(id , 0xFF00FF00, 0xFF00FF00);
	ONDK_GWM_COM_Button_SetButtonColor(id , 0xFF0000A0, 0xFF000060, 0xFF0000FF, 0xFF000020);
	ONDK_GWM_COM_Button_SetAlign (id, ONDKSTF_CENTER);
}


ONDK_Result_t local_testimage_DrawScale (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t 		imgRect, dstRect;
	ONDK_Size_t			size;
	ONDKImageHandle		hImg = NULL, hHLineImg = NULL, hVLineImg = NULL;
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0;
	ONDKFontHandle	*hFontHandle;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	x = x1;
	y = y1;
	w = rect->w;
	h = rect->h;

	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, 20);
	if (hFontHandle == NULL)
	{
		ONDK_Assert(0);
		return	ONDK_RET_FAIL;
	}


	{
		hImg = ONDK_IMG_CreateSelf(DEF_ONDK_TEST_IMAGE_371_H_SET_C_PNG);
		if(NULL == hImg)
		{
			return ONDK_RET_FAIL;
		}

		x += 20;
		y += 20;
		size = ONDK_IMG_GetSize(hImg);
		imgRect = ONDK_Rect(0, 0, size.w, size.h);

		ONDK_IMG_Blit(hImg, screen, x, y);
		dstRect = ONDK_Rect(x, y, size.w, size.h);
		ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

		x += (size.w + 10);
		dstRect = ONDK_Rect(x, y, (size.w + 50), size.h);
		ONDK_IMG_StretchBlit(hImg, screen, dstRect);
		ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

		x += (size.w + 60);
		dstRect = ONDK_Rect(x, y, (size.w), (size.h+50));
		ONDK_IMG_StretchBlit(hImg, screen, dstRect);
		ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

		x += (size.w + 10);
		dstRect = ONDK_Rect(x, y, (size.w+50), (size.h+50));
		ONDK_IMG_StretchBlit(hImg, screen, dstRect);
		ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

		x += (size.w + 60);
		dstRect = ONDK_Rect(x, y, (size.w/2), (size.h/2));
		ONDK_IMG_StretchBlit(hImg, screen, dstRect);
		ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

		x += (size.w/2 + 10);
		dstRect = ONDK_Rect(x, y, (size.w/2), size.h);
		ONDK_IMG_StretchBlit(hImg, screen, dstRect);
		ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

		x += (size.w/2 + 10);
		dstRect = ONDK_Rect(x, y, size.w, (size.h/2));
		ONDK_IMG_StretchBlit(hImg, screen, dstRect);
		ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

		x += (size.w + 10);
		dstRect = ONDK_Rect(x, y, size.w, size.h);
		ONDK_IMG_BlitAlpha(hImg, screen, x, y, 50);
		ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

		x += (size.w + 10);
		dstRect = ONDK_Rect(x, y, (size.w/2), (size.h/2));
		ONDK_IMG_StretchBlitAlpha(hImg, screen, dstRect, 50);
		ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

		ONDK_IMG_Destroy(hImg);
		hImg = NULL;
	}


	{
		hHLineImg = ONDK_IMG_CreateSelf(DEF_ONDK_TEST_IMAGE_HLINE_PNG);
		if(NULL == hHLineImg)
		{
			return ONDK_RET_FAIL;
		}

		y += (size.h + 70);
		x = x1 + 20;
		dstRect = ONDK_Rect(x, y, 310, 90);
		ONDK_IMG_StretchBlit(hHLineImg, screen, dstRect);
		ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);
		ONDK_FONT_DrawStringAlign(screen, hFontHandle, "H Line", x+10, y+10, -1, 0xFF991991, ONDKSTF_LEFT);

		ONDK_IMG_Destroy(hHLineImg);
	}

	{
		hVLineImg = ONDK_IMG_CreateSelf(DEF_ONDK_TEST_IMAGE_VLINE_PNG);
		if(NULL == hVLineImg)
		{
			return ONDK_RET_FAIL;
		}

		x += 320;
		dstRect = ONDK_Rect(x, y, 310, 90);
		ONDK_IMG_StretchBlit(hVLineImg, screen, dstRect);
		ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);
		ONDK_FONT_DrawStringAlign(screen, hFontHandle, "V Line", x+10, y+10, -1, 0xFF991991, ONDKSTF_LEFT);

		ONDK_IMG_Destroy(hVLineImg);
		hVLineImg = NULL;
	}

	if (hFontHandle)
		ONDK_FONT_Release (hFontHandle);

	return	ONDK_RET_OK;
}

ONDK_Result_t local_testimage_DrawClip (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t 		srcRect, dstRect;
	ONDK_Size_t			size;
	ONDKImageHandle		hImg = NULL;
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32		x = 0, y = 0, w = 0, h = 0;

	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	x = x1;
	y = y1;
	w = rect->w;
	h = rect->h;

	hImg = ONDK_IMG_CreateSelf(DEF_ONDK_TEST_IMAGE_371_H_SET_C_PNG);
	if(NULL == hImg)
	{
		return ONDK_RET_FAIL;
	}

	x += 20;
	y += 20;
	size = ONDK_IMG_GetSize(hImg);
	srcRect = ONDK_Rect(0, 0, size.w, size.h);

	ONDK_IMG_Blit(hImg, screen, x, y);
	dstRect = ONDK_Rect(x, y, size.w, size.h);
	ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

	x += (size.w + 10);
	srcRect = ONDK_Rect((size.w/2), 0, (size.w/2), size.h);
	dstRect = ONDK_Rect(x, y, (size.w/2), size.h);
	ONDK_IMG_StretchBlitPartRect(hImg, screen, srcRect, dstRect);
	ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

	x += ((size.w/2) + 10);
	srcRect = ONDK_Rect(0, (size.h/2), size.w, (size.h/2));
	dstRect = ONDK_Rect(x, y, size.w, (size.h/2));
	ONDK_IMG_StretchBlitPartRect(hImg, screen, srcRect, dstRect);
	ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

	x += ((size.w) + 10);
	srcRect = ONDK_Rect(0, (size.h/2), size.w, (size.h/2));
	dstRect = ONDK_Rect(x, y, size.w, size.h);
	ONDK_IMG_StretchBlitPartRect(hImg, screen, srcRect, dstRect);
	ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

	x += ((size.w) + 10);
	srcRect = ONDK_Rect((size.w/2), 0, (size.w/2), size.h);
	dstRect = ONDK_Rect(x, y, size.w, size.h);
	ONDK_IMG_StretchBlitPartRect(hImg, screen, srcRect, dstRect);
	ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

	x += ((size.w) + 10);
	srcRect = ONDK_Rect((size.w/2), (size.h/2), (size.w/2), (size.h/2));
	dstRect = ONDK_Rect(x, y, size.w, size.h);
	ONDK_IMG_StretchBlitPartRect(hImg, screen, srcRect, dstRect);
	ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

	x += ((size.w) + 10);
	srcRect = ONDK_Rect((size.w/2), (size.h/2), (size.w/2), (size.h/2));
	dstRect = ONDK_Rect(x, y, size.w, size.h);
	ONDK_IMG_StretchBlitPartRectAlpha(hImg, screen, srcRect, dstRect, 50);
	ONDK_DRAW_Rect(screen, dstRect, 0xFF820610);

	ONDK_IMG_Destroy(hImg);

	return ONDK_RET_OK;
}

STATIC void local_testimage_SetUiObject(TESTIMAGE_Contents_t *pstContents)
{
	ONDK_Rect_t 		rect;
	HUINT32				ulBtn = 0;

	ONDK_GWM_APP_SetAppArea(DEF_ONDK_TEST_IMAGE_X, DEF_ONDK_TEST_IMAGE_Y, DEF_ONDK_TEST_IMAGE_W, DEF_ONDK_TEST_IMAGE_H);

	rect = ONDK_Rect(DEF_ONDK_TEST_IMAGE_DATA_X, DEF_ONDK_TEST_IMAGE_DATA_Y, DEF_ONDK_TEST_IMAGE_DATA_W, DEF_ONDK_TEST_IMAGE_DATA_H);
	ONDK_GWM_COM_Rect_Create(eTESTIMAGE_Object_RECT_FRAME, &rect, 0xFFAAAAAA);

	rect = ONDK_Rect(DEF_ONDK_TEST_IMAGE_BTN_X, DEF_ONDK_TEST_IMAGE_BTN_Y, DEF_ONDK_TEST_IMAGE_BTN_W, DEF_ONDK_TEST_IMAGE_BTN_H);
	ulBtn = eTESTIMAGE_Object_BTN_SCALE;
	local_testimage_SetUiObject_CreateButton(ulBtn, "Scale & Alpha", rect);

	rect.x += (DEF_ONDK_TEST_IMAGE_BTN_W+10);
	ulBtn++;
	local_testimage_SetUiObject_CreateButton(ulBtn, "Part Blend", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_IMAGE_DATA_X, DEF_ONDK_TEST_IMAGE_DATA_Y, DEF_ONDK_TEST_IMAGE_DATA_W, DEF_ONDK_TEST_IMAGE_DATA_H);
	ONDK_GWM_COM_Rect_Create(eTESTIMAGE_Object_RECT_Data, &rect, 0x00000000);

	ONDK_GWM_Obj_SetFocus (eTESTIMAGE_Object_BTN_SCALE);
}

/**************************************************************************************************/
#define _________AP_Volume_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/


//MSG_GWM_CREATE
STATIC ONDK_Result_t local_testimage_MsgGwmCreate(TESTIMAGE_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	local_testimage_SetUiObject(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}

//MSG_GWM_KEYDOWN
STATIC ONDK_Result_t local_testimage_MsgGwmKeyDown(TESTIMAGE_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 nKeyId = p1;

	switch(nKeyId)
	{
	case KEY_EXIT:
	case KEY_BACK:
		ONDK_GWM_APP_Destroy(NULL);
		return GWM_MESSAGE_BREAK;

	default:
		break;
	}

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}


//MSG_GWM_KEYDOWN
STATIC ONDK_Result_t local_testimage_MsgGwmClicked(TESTIMAGE_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 nCmdId = (HUINT32)p1;


	switch(nCmdId)
	{
	case eTESTIMAGE_Object_BTN_SCALE:
		ONDK_GWM_APP_SetObjectDrawMethod(eTESTIMAGE_Object_RECT_Data, local_testimage_DrawScale);
		ONDK_GWM_APP_InvalidateObject (eTESTIMAGE_Object_RECT_Data);
		break;

	case eTESTIMAGE_Object_BTN_CLIP:
		ONDK_GWM_APP_SetObjectDrawMethod(eTESTIMAGE_Object_RECT_Data, local_testimage_DrawClip);
		ONDK_GWM_APP_InvalidateObject (eTESTIMAGE_Object_RECT_Data);
		break;

	default:
		break;
	}



	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testimage_MsgGwmDestroy(TESTIMAGE_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);


	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testimage_MsgGwmScrollRight(TESTIMAGE_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	ONDK_GWM_Obj_SetFocus ((eTESTIMAGE_Object_BTN_START+1));

	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testimage_MsgGwmScrollLeft(TESTIMAGE_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	ONDK_GWM_Obj_SetFocus ((eTESTIMAGE_Object_BTN_END-1));

	return GWM_RET_OK;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t ONDK_TEST_IMAGE_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult	= GWM_RET_OK;
	TESTIMAGE_Contents_t 	*pstContents = local_testimage_GetContents();

	switch (nMessage)
	{
	case MSG_GWM_CREATE:
		eResult = local_testimage_MsgGwmCreate(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_KEYDOWN:
		eResult = local_testimage_MsgGwmKeyDown(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_CLICKED:
		eResult = local_testimage_MsgGwmClicked(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_DESTROY:
		eResult = local_testimage_MsgGwmDestroy(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_SCROLLRIGHT:
		eResult = local_testimage_MsgGwmScrollRight(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_SCROLLLEFT:
		eResult = local_testimage_MsgGwmScrollLeft(pstContents, p1, p2, p3);
		break;
	default:
		break;
	}

	if(eResult != GWM_RET_OK)
	{
		return eResult;
	}
	return ONDK_GWM_ProcessMessageDefault(nMessage, handle, p1, p2, p3);
}

void ONDK_TEST_IMAGE_Start(void)
{
	if(NULL == ONDK_GWM_APP_Get(ONDK_TEST_IMAGE_Proc))
	{
		HLIB_CMD_Printf("[%s:%d] Create \r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "ONDK_TEST_IMAGE_Proc", APP_INDEPENDENT_OSD_GRP_PRIORITY, ONDK_TEST_IMAGE_Proc, 0, 0, 0, 0);
	}
	else
	{
		HLIB_CMD_Printf("[%s:%d] Destroy\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_Destroy(ONDK_TEST_IMAGE_Proc);
	}
}



