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


#define DEF_ONDK_TEST_TIMER_X  0
#define DEF_ONDK_TEST_TIMER_Y  0
#define DEF_ONDK_TEST_TIMER_W  1280
#define DEF_ONDK_TEST_TIMER_H  720

#define DEF_ONDK_TEST_TIMER_DATA_X  20
#define DEF_ONDK_TEST_TIMER_DATA_Y  200
#define DEF_ONDK_TEST_TIMER_DATA_W  1220
#define DEF_ONDK_TEST_TIMER_DATA_H  500

#define DEF_ONDK_TEST_TIMER_BOX_X  40
#define DEF_ONDK_TEST_TIMER_BOX_Y  600
#define DEF_ONDK_TEST_TIMER_BOX_W  80
#define DEF_ONDK_TEST_TIMER_BOX_H  80


#define DEF_ONDK_TEST_TIMER_BTN_W  120
#define DEF_ONDK_TEST_TIMER_BTN_H  60

#define DEF_ONDK_TEST_TIMER_BTN1_X  10
#define DEF_ONDK_TEST_TIMER_BTN1_Y  10

#define DEF_ONDK_TEST_TIMER_BTN2_X  (DEF_ONDK_TEST_TIMER_BTN1_X+DEF_ONDK_TEST_TIMER_BTN_W+10)
#define DEF_ONDK_TEST_TIMER_BTN2_Y  10

#define DEF_ONDK_TEST_TIMER_BTN3_X  (DEF_ONDK_TEST_TIMER_BTN2_X+DEF_ONDK_TEST_TIMER_BTN_W+10)
#define DEF_ONDK_TEST_TIMER_BTN3_Y  10

#define DEF_ONDK_TEST_TIMER_BTN4_X  (DEF_ONDK_TEST_TIMER_BTN3_X+DEF_ONDK_TEST_TIMER_BTN_W+10)
#define DEF_ONDK_TEST_TIMER_BTN4_Y  10

#define DEF_ONDK_TEST_TIMER_BTN5_X  (DEF_ONDK_TEST_TIMER_BTN4_X+DEF_ONDK_TEST_TIMER_BTN_W+10)
#define DEF_ONDK_TEST_TIMER_BTN5_Y  10

#define DEF_ONDK_TEST_BOX_TIMER_PATH		"/usr/image/209_Wide_Option_169_Auto_C.png"
#define DEF_ONDK_TEST_SHOOT_TIMER_PATH		"/usr/image/205_REC_Recording.png"

#define DEF_ONDK_TEST_TIMER_TIMER_ID		0x820610
#define DEF_ONDK_TEST_ANIMATION_TIMER_ID	0x820611


typedef enum tagTESTTIMER_Object
{
	eTESTTIMER_Object_FRAME_Background = 0x991,
	eTESTTIMER_Object_RECT_DATA,
	eTESTTIMER_Object_RECT_DataOutLine,
	eTESTTIMER_Object_BOX,

	eTESTTIMER_Object_BTN_DrawLeft,
	eTESTTIMER_Object_BTN_DrawRight,
	eTESTTIMER_Object_BTN_DrawShoot,
	eTESTTIMER_Object_BTN_DrawNone,

	eTESTTIMER_Object_BTN_MAX
}TESTTIMER_Object_e;

typedef struct tagTESTTIMER_Shooting_t
{
	HINT32 x, y;
	HINT32	startY;
	HINT32	speed;
	HUINT32 startTime;

} TESTTIMER_Shooting_t;

typedef struct tagTESTTIMER_Contents_t
{
	HxList_t	*pList; /* TESTTIMER_Shooting_t */
	HCHAR			szText[2048];

	HUINT32 		startTime;
	ONDK_Rect_t 	orgRect;
	ONDK_Rect_t 	targetRect;
	ONDK_Rect_t 	boxRect;


	ONDK_Rect_t 	shootRect;
	ONDKImageHandle	hBoxImg;
	ONDKImageHandle	hShootImg;
	HUINT32			sysLastTick;
} TESTTIMER_Contents_t;

STATIC TESTTIMER_Contents_t *s_pstTestFontContents = NULL;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/


STATIC TESTTIMER_Contents_t *local_testtimer_GetContents(void)
{
	if(NULL == s_pstTestFontContents)
	{
		s_pstTestFontContents = ONDK_Calloc(sizeof(TESTTIMER_Contents_t));
	}
	return s_pstTestFontContents;
}


STATIC void local_testtimer_SetUiObject_CreateButton(HUINT32 id, HCHAR *name, ONDK_Rect_t rect)
{
	ONDK_GWM_COM_Button_Create(id , &rect, name);
	ONDK_GWM_COM_Button_SetTitleSize(id , 14);
	ONDK_GWM_COM_Button_SetFontColor(id , 0xFF00FF00, 0xFF00FF00);
	ONDK_GWM_COM_Button_SetButtonColor(id , 0xFF0000A0, 0xFF000060, 0xFF0000FF, 0xFF000020);
	ONDK_GWM_COM_Button_SetAlign (id, ONDKSTF_CENTER);
}


ONDK_Result_t local_testtimer_DrawBox (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HxList_t	*pList = NULL;

	TESTTIMER_Shooting_t	*pShooting = NULL;
	TESTTIMER_Contents_t 	*pstContents = local_testtimer_GetContents();


	ONDK_IMG_StretchBlit(pstContents->hBoxImg, screen, pstContents->boxRect);

	pList = pstContents->pList;
	while(pList)
	{
		pShooting = HLIB_LIST_Data(pList);
		ONDK_Assert(pShooting);

		ONDK_IMG_Blit(pstContents->hShootImg, screen, pShooting->x, pShooting->y);

		pList = HLIB_LIST_Next(pList);
	}

	return	ONDK_RET_OK;
}

ONDK_Result_t local_testtimer_DrawMoveFPS (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
#if 0
	ONDKFontHandle 			hFontHandle = NULL;
	TESTTIMER_Contents_t 	*pstContents = local_testtimer_GetContents();

	hFontHandle = ONDK_FONT_CreateSystemFont(eFont_SystemNormal, 20);
	if (hFontHandle == NULL)
	{
		ONDK_Assert(0);
		return	ONDK_RET_FAIL;
	}

	ONDK_FONT_DrawStringAlign(screen, hFontHandle, pstContents->szText, DEF_ONDK_TEST_TIMER_DATA_X, DEF_ONDK_TEST_TIMER_DATA_Y,DEF_ONDK_TEST_TIMER_DATA_W, 0xFFAAAAAA, ONDKSTF_LEFT);

	if (hFontHandle)
		ONDK_FONT_Release (hFontHandle);
#endif
	return	ONDK_RET_OK;
}


STATIC void local_testtimer_SetUiObject(TESTTIMER_Contents_t *pstContents)
{
	ONDK_Rect_t 		rect;
	ONDK_Size_t			size;
	ONDKImageHandle		hImg;

	ONDK_GWM_APP_SetAppArea(DEF_ONDK_TEST_TIMER_X, DEF_ONDK_TEST_TIMER_Y, DEF_ONDK_TEST_TIMER_W, DEF_ONDK_TEST_TIMER_H);
#if 0
	rect = ONDK_Rect(DEF_ONDK_TEST_TIMER_X, DEF_ONDK_TEST_TIMER_Y, DEF_ONDK_TEST_TIMER_W, DEF_ONDK_TEST_TIMER_H);
	ONDK_GWM_COM_Rect_Create(eTESTTIMER_Object_FRAME_Background, &rect, 0xFFFFFFFF);
#endif
	rect = ONDK_Rect(DEF_ONDK_TEST_TIMER_BTN1_X, DEF_ONDK_TEST_TIMER_BTN1_Y, DEF_ONDK_TEST_TIMER_BTN_W, DEF_ONDK_TEST_TIMER_BTN_H);
	local_testtimer_SetUiObject_CreateButton(eTESTTIMER_Object_BTN_DrawLeft, "<", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_TIMER_BTN2_X, DEF_ONDK_TEST_TIMER_BTN2_Y, DEF_ONDK_TEST_TIMER_BTN_W, DEF_ONDK_TEST_TIMER_BTN_H);
	local_testtimer_SetUiObject_CreateButton(eTESTTIMER_Object_BTN_DrawRight, ">", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_TIMER_BTN3_X, DEF_ONDK_TEST_TIMER_BTN3_Y, DEF_ONDK_TEST_TIMER_BTN_W, DEF_ONDK_TEST_TIMER_BTN_H);
	local_testtimer_SetUiObject_CreateButton(eTESTTIMER_Object_BTN_DrawShoot, "^", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_TIMER_BTN4_X, DEF_ONDK_TEST_TIMER_BTN4_Y, DEF_ONDK_TEST_TIMER_BTN_W, DEF_ONDK_TEST_TIMER_BTN_H);
	local_testtimer_SetUiObject_CreateButton(eTESTTIMER_Object_BTN_DrawNone, "-", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_TIMER_DATA_X, DEF_ONDK_TEST_TIMER_DATA_Y, DEF_ONDK_TEST_TIMER_DATA_W, DEF_ONDK_TEST_TIMER_DATA_H);
	ONDK_GWM_COM_Rect_Create(eTESTTIMER_Object_RECT_DataOutLine, &rect, 0xFF333333);
	ONDK_GWM_APP_SetObjectDrawMethod(eTESTTIMER_Object_RECT_DataOutLine, local_testtimer_DrawMoveFPS);


	hImg = ONDK_IMG_CreateSelf(DEF_ONDK_TEST_SHOOT_TIMER_PATH);
	ONDK_Assert(hImg);
	pstContents->hShootImg = hImg;
	size = ONDK_IMG_GetSize(hImg);
	pstContents->shootRect = ONDK_Rect(DEF_ONDK_TEST_TIMER_BOX_X, DEF_ONDK_TEST_TIMER_BOX_Y, size.w, size.h);

	hImg = ONDK_IMG_CreateSelf(DEF_ONDK_TEST_BOX_TIMER_PATH);
	ONDK_Assert(hImg);
	pstContents->hBoxImg = hImg;
	size = ONDK_IMG_GetSize(hImg);
	rect = ONDK_Rect(DEF_ONDK_TEST_TIMER_BOX_X, DEF_ONDK_TEST_TIMER_BOX_Y, size.w, size.h);
	pstContents->boxRect = rect;
	rect = ONDK_Rect(DEF_ONDK_TEST_TIMER_X, DEF_ONDK_TEST_TIMER_Y, DEF_ONDK_TEST_TIMER_W, DEF_ONDK_TEST_TIMER_H);
	ONDK_GWM_COM_Rect_Create(eTESTTIMER_Object_BOX, &rect, 0xFF000000);
	ONDK_GWM_APP_SetObjectDrawMethod(eTESTTIMER_Object_BOX, local_testtimer_DrawBox);


	ONDK_GWM_Obj_SetFocus (eTESTTIMER_Object_BTN_DrawLeft);
}

/**************************************************************************************************/
#define _________AP_Volume_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/


//MSG_GWM_CREATE
STATIC ONDK_Result_t local_testtimer_MsgGwmCreate(TESTTIMER_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	local_testtimer_SetUiObject(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}

#define DEF_TEST_MOVE_TIMER_X	100
//MSG_GWM_KEYDOWN
STATIC ONDK_Result_t local_testtimer_MsgGwmKeyDown(TESTTIMER_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 nKeyId = p1;
	HBOOL	isTimer = FALSE;
	TESTTIMER_Shooting_t	*pShooting = NULL;

	switch(nKeyId)
	{
	case KEY_EXIT:
	case KEY_BACK:
		ONDK_GWM_APP_Destroy(NULL);
		return GWM_MESSAGE_BREAK;

	case KEY_ARROWRIGHT:
		isTimer = ONDK_GWM_IsTimer(DEF_ONDK_TEST_ANIMATION_TIMER_ID);
		if(TRUE == isTimer)
		{
			ONDK_GWM_APP_InvalidateONDKRect(&pstContents->boxRect);
			ONDK_GWM_APP_InvalidateONDKRect(&pstContents->targetRect);
			pstContents->boxRect = pstContents->targetRect;
		}
		pstContents->startTime = VK_TIMER_GetSystemTick();
		pstContents->orgRect = pstContents->boxRect;
		pstContents->targetRect = pstContents->boxRect;
		pstContents->targetRect.x += DEF_TEST_MOVE_TIMER_X;
		pstContents->targetRect.w += DEF_TEST_MOVE_TIMER_X;


		if(TRUE == isTimer)
		{
			ONDK_GWM_ResetTimer(DEF_ONDK_TEST_ANIMATION_TIMER_ID);
		}
		else
		{
			ONDK_GWM_SetTimer(DEF_ONDK_TEST_ANIMATION_TIMER_ID, 10);
		}
		return GWM_MESSAGE_BREAK;

	case KEY_ARROWDOWN:
		isTimer = ONDK_GWM_IsTimer(DEF_ONDK_TEST_ANIMATION_TIMER_ID);
		if(TRUE == isTimer)
		{
			ONDK_GWM_APP_InvalidateONDKRect(&pstContents->boxRect);
			ONDK_GWM_APP_InvalidateONDKRect(&pstContents->targetRect);
			pstContents->boxRect = pstContents->targetRect;
		}

		pstContents->startTime = VK_TIMER_GetSystemTick();
		pstContents->orgRect = pstContents->boxRect;
		pstContents->targetRect = pstContents->boxRect;
		pstContents->targetRect.y += DEF_TEST_MOVE_TIMER_X;
		if(pstContents->targetRect.w > DEF_TEST_MOVE_TIMER_X)
		{
			pstContents->targetRect.w -= DEF_TEST_MOVE_TIMER_X;
		}

		if(TRUE == isTimer)
		{
			ONDK_GWM_ResetTimer(DEF_ONDK_TEST_ANIMATION_TIMER_ID);
		}
		else
		{
			ONDK_GWM_SetTimer(DEF_ONDK_TEST_ANIMATION_TIMER_ID, 10);
		}

		return GWM_MESSAGE_BREAK;

	case KEY_ARROWUP:
		isTimer = ONDK_GWM_IsTimer(DEF_ONDK_TEST_ANIMATION_TIMER_ID);
		if(TRUE == isTimer)
		{
			ONDK_GWM_APP_InvalidateONDKRect(&pstContents->boxRect);
			ONDK_GWM_APP_InvalidateONDKRect(&pstContents->targetRect);
			pstContents->boxRect = pstContents->targetRect;
		}

		pstContents->startTime = VK_TIMER_GetSystemTick();
		pstContents->orgRect = pstContents->boxRect;
		pstContents->targetRect = pstContents->boxRect;
		pstContents->targetRect.y -= DEF_TEST_MOVE_TIMER_X;
		pstContents->targetRect.w += DEF_TEST_MOVE_TIMER_X;

		if(TRUE == isTimer)
		{
			ONDK_GWM_ResetTimer(DEF_ONDK_TEST_ANIMATION_TIMER_ID);
		}
		else
		{
			ONDK_GWM_SetTimer(DEF_ONDK_TEST_ANIMATION_TIMER_ID, 10);
		}
		return GWM_MESSAGE_BREAK;

	case KEY_ARROWLEFT:
		isTimer = ONDK_GWM_IsTimer(DEF_ONDK_TEST_ANIMATION_TIMER_ID);
		if(TRUE == isTimer)
		{
			ONDK_GWM_APP_InvalidateONDKRect(&pstContents->boxRect);
			ONDK_GWM_APP_InvalidateONDKRect(&pstContents->targetRect);
			pstContents->boxRect = pstContents->targetRect;
		}

		pstContents->startTime = VK_TIMER_GetSystemTick();
		pstContents->orgRect = pstContents->boxRect;
		pstContents->targetRect = pstContents->boxRect;
		pstContents->targetRect.x -= DEF_TEST_MOVE_TIMER_X;
		pstContents->targetRect.w -= DEF_TEST_MOVE_TIMER_X;
		if(TRUE == isTimer)
		{
			ONDK_GWM_ResetTimer(DEF_ONDK_TEST_ANIMATION_TIMER_ID);
		}
		else
		{
			ONDK_GWM_SetTimer(DEF_ONDK_TEST_ANIMATION_TIMER_ID, 10);
		}
		return GWM_MESSAGE_BREAK;

	case KEY_OK:
		pShooting = ONDK_Calloc(sizeof(TESTTIMER_Shooting_t));
		ONDK_Assert(pShooting);
		pShooting->x = pstContents->boxRect.x + 25;
		pShooting->y = pstContents->boxRect.y + 10;
		pShooting->speed = 10;
		pShooting->startTime = VK_TIMER_GetSystemTick();
		pShooting->startY = pShooting->y-DEF_ONDK_TEST_TIMER_DATA_Y;
		pstContents->pList = HLIB_LIST_Append(pstContents->pList, pShooting);
		ONDK_GWM_SetTimer(DEF_ONDK_TEST_TIMER_TIMER_ID, 10);
		return GWM_MESSAGE_BREAK;

	default:
		break;
	}

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}


//MSG_GWM_KEYDOWN
STATIC ONDK_Result_t local_testtimer_MsgGwmClicked(TESTTIMER_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//HUINT32 nCmdId = (HUINT32)p1;

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}

#include <math.h>
//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testtimer_MsgGwmTimer(TESTTIMER_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxList_t	*pList = NULL;
	HINT32		fpsTick = 0;
	double		fps = 0.0f;

	TESTTIMER_Shooting_t	*pShooting = NULL;

	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	switch(p1)
	{
	case DEF_ONDK_TEST_ANIMATION_TIMER_ID:
		{
			double xSpeed, ySpeed, wSpeed, hSpeed, xPer, yPer, wPer, hPer;// = 320 + 300 * sin(VK_TIMER_GetSystemTick()), y = 300 * cos(VK_TIMER_GetSystemTick());
			double pi = 3.14159265/180;
			ONDK_Rect_t r1;
			ONDK_Rect_t r2;
			ONDK_Rect_t org;
			ONDK_Rect_t ut;
			HINT32 tick = 0;
			HINT32	x, y, w, h;

			org = pstContents->orgRect;
			r1 = pstContents->boxRect;
			r2 = pstContents->targetRect;
			ut = ONDK_UnionRect(&org, &r2);

			if((r2.x-r1.x) < 0)
			{
				xPer = (double)((org.x-r1.x)*90) / (org.x-r2.x);
			}
			else if((r2.x-r1.x) > 0)
			{
				xPer = (double)((r1.x-org.x)*90) / (r2.x-org.x);
			}
			else
			{
				xPer = 90.0f;
			}

			if((r2.y-r1.y) < 0)
			{
				yPer = (double)((org.y-r1.y)*90) / (org.y-r2.y);
			}
			else if((r2.y-r1.y) > 0)
			{
				yPer = (double)((r1.y-org.y)*90) / (r2.y-org.y);
			}
			else
			{
				yPer = 90.0f;
			}

			if((r2.w-r1.w) < 0)
			{
				wPer = (double)((org.w-r1.w)*90) / (org.w-r2.w);
			}
			else if((r2.w-r1.w) > 0)
			{
				wPer = (double)((r1.w-org.w)*90) / (r2.w-org.w);
			}
			else
			{
				wPer = 90.0f;
			}

			if((r2.h-r1.h) < 0)
			{
				hPer = (double)((org.h-r1.h)*90) / (org.h-r2.h);
			}
			else if((r2.h-r1.h) > 0)
			{
				hPer = (double)((r1.h-org.h)*90) / (r2.h-org.h);
			}
			else
			{
				hPer = 90.0f;
			}


			ySpeed = cos(yPer*pi);
			hSpeed = cos(hPer*pi);
			xSpeed = cos(xPer*pi);
			wSpeed = cos(wPer*pi);

			tick = ((VK_TIMER_GetSystemTick()-pstContents->startTime)/100);
			x = (xSpeed*tick*10);
			y = (ySpeed*tick*10);
			w = (wSpeed*tick*10);
			h = (hSpeed*tick*10);

			if(1 > y) y = 1;
			if(1 > w) w = 1;
			if(1 > h) h = 1;
			if(1 > x) x = 1;


			if((r2.x-r1.x) < 0)
			{
				if(pstContents->boxRect.x > r2.x + x)
				{
					pstContents->boxRect.x -= x;

				}
				else
				{
					pstContents->boxRect.x = r2.x;
					ONDK_GWM_KillTimer(p1);
				}
			}
			else if((r2.x-r1.x) > 0)
			{
				if(pstContents->boxRect.x + x < r2.x)
				{
					pstContents->boxRect.x += x;
				}
				else
				{
					pstContents->boxRect.x = r2.x;
					ONDK_GWM_KillTimer(p1);
				}
			}

			if((r2.w-r1.w) < 0)
			{
				if(pstContents->boxRect.w > r2.w + w)
				{
					pstContents->boxRect.w -= w;

				}
				else
				{
					pstContents->boxRect.w = r2.w;
					ONDK_GWM_KillTimer(p1);
				}
			}
			else if((r2.w-r1.w) > 0)
			{
				if(pstContents->boxRect.w + w < r2.w)
				{
					pstContents->boxRect.w += w;
				}
				else
				{
					pstContents->boxRect.w = r2.w;
					ONDK_GWM_KillTimer(p1);
				}
			}

			if((r2.h-r1.h) < 0)
			{
				if(pstContents->boxRect.h > r2.h + h)
				{
					pstContents->boxRect.h -= h;

				}
				else
				{
					pstContents->boxRect.h = r2.h;
					ONDK_GWM_KillTimer(p1);
				}
			}
			else if((r2.h-r1.h) > 0)
			{
				if(pstContents->boxRect.h + h < r2.h)
				{
					pstContents->boxRect.h += h;
				}
				else
				{
					pstContents->boxRect.h = r2.h;
					ONDK_GWM_KillTimer(p1);
				}
			}

			if((r2.y-r1.y) < 0)
			{
				if(pstContents->boxRect.y > r2.y + y)
				{
					pstContents->boxRect.y -= y;
				}
				else
				{
					pstContents->boxRect.y = r2.y;
					ONDK_GWM_KillTimer(p1);
				}
			}
			else if((r2.y-r1.y) > 0)
			{
				if(pstContents->boxRect.y + y < r2.y)
				{
					pstContents->boxRect.y += y;
				}
				else
				{
					pstContents->boxRect.y = r2.y;
					ONDK_GWM_KillTimer(p1);
				}
			}

			ONDK_GWM_APP_InvalidateRect(ut.x, ut.y, ut.w, ut.h);
		}
		break;

	case DEF_ONDK_TEST_TIMER_TIMER_ID:
		{
			double speed, per;// = 320 + 300 * sin(VK_TIMER_GetSystemTick()), y = 300 * cos(VK_TIMER_GetSystemTick());
			double pi = 3.14159265/180;
			HINT32 y = 0, tick;

			pList = pstContents->pList;
			while(pList)
			{
				pShooting = HLIB_LIST_Data(pList);
				ONDK_Assert(pShooting);

				per = (double)((pShooting->y-DEF_ONDK_TEST_TIMER_DATA_Y)*180) / (pShooting->startY);
				speed = sin(per*pi);
				tick = ((VK_TIMER_GetSystemTick()-pShooting->startTime)/100);
				y = abs(speed*tick*(pShooting->speed/2));
				if(1 > y)
				{
					y = 1;
				}

				if(pShooting->y > (DEF_ONDK_TEST_TIMER_DATA_Y+y))
				{
					pShooting->y -= y;
					pList = HLIB_LIST_Next(pList);
				}
				else
				{
					pList = HLIB_LIST_Next(pList);
					pstContents->pList = HLIB_LIST_Remove(pstContents->pList, pShooting);
					ONDK_Free(pShooting);
				}
			}

			if(pstContents->pList == NULL)
			{
				ONDK_GWM_KillTimer(p1);
			}

			fpsTick = ((VK_TIMER_GetSystemTick() - pstContents->sysLastTick));
			pstContents->sysLastTick = VK_TIMER_GetSystemTick();
			fps = 1000.0f / ((double)fpsTick+0.001f);
			HxSTD_snprintf(pstContents->szText, 2048, "FPS : %f", fps);
			ONDK_GWM_APP_InvalidateRect(DEF_ONDK_TEST_TIMER_DATA_X, DEF_ONDK_TEST_TIMER_DATA_Y, DEF_ONDK_TEST_TIMER_DATA_W, DEF_ONDK_TEST_TIMER_DATA_H);
		}
		break;

	default:
		break;
	}


	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testtimer_MsgGwmDestroy(TESTTIMER_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);


	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testtimer_MsgGwmScrollRight(TESTTIMER_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	ONDK_GWM_Obj_SetFocus (eTESTTIMER_Object_BTN_DrawLeft);

	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testtimer_MsgGwmScrollLeft(TESTTIMER_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	ONDK_GWM_Obj_SetFocus ((eTESTTIMER_Object_BTN_MAX-1));

	return GWM_RET_OK;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t ONDK_TEST_TIMER_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult	= GWM_RET_OK;
	TESTTIMER_Contents_t 	*pstContents = local_testtimer_GetContents();

	switch (nMessage)
	{
	case MSG_GWM_CREATE:
		eResult = local_testtimer_MsgGwmCreate(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_KEYDOWN:
		eResult = local_testtimer_MsgGwmKeyDown(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_CLICKED:
		eResult = local_testtimer_MsgGwmClicked(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_TIMER:
		eResult = local_testtimer_MsgGwmTimer(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_DESTROY:
		eResult = local_testtimer_MsgGwmDestroy(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_SCROLLRIGHT:
		eResult = local_testtimer_MsgGwmScrollRight(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_SCROLLLEFT:
		eResult = local_testtimer_MsgGwmScrollLeft(pstContents, p1, p2, p3);
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

void ONDK_TEST_TIMER_Start(void)
{
	if(NULL == ONDK_GWM_APP_Get(ONDK_TEST_TIMER_Proc))
	{
		HLIB_CMD_Printf("[%s:%d] Create \r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "ONDK_TEST_TIMER_Proc", APP_INDEPENDENT_OSD_GRP_PRIORITY, ONDK_TEST_TIMER_Proc, 0, 0, 0, 0);
	}
	else
	{
		HLIB_CMD_Printf("[%s:%d] Destroy\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_Destroy(ONDK_TEST_TIMER_Proc);
	}
}



