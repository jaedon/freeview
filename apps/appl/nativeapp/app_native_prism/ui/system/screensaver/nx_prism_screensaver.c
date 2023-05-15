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

/******************************************************************************/
/**
 * @file	  		nx_prism_screensaver.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_main.h>
#include <nx_core.h>
#include <nx_port.h>

#include <nx_common.h>
#include <nx_core_message.h>
#include <nx_prism_app.h>

#include "nx_prism_definition.h"


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/

#define	NX_SCREENSAVER_ITEM_ID								eMenuFrame_Window_System_ScreenSaver	// Statue Message 0 ~ 49

#define	NX_SCREENSAVER_IMG_SCREEN_TIMER_ID					(NX_SCREENSAVER_ITEM_ID + 1)
#define	NX_SCREENSAVER_IMG_SCREEN_TIMER1_INTERVAL			30 //30ms
#define	NX_SCREENSAVER_IMG_SCREEN_TIMER2_INTERVAL			400 //400ms

#define	NX_SCREENSAVER_IMG_DISPLAY_TIME_TIMER_ID			(NX_SCREENSAVER_ITEM_ID + 2)
#define	NX_SCREENSAVER_IMG_DISPLAY_TIME_TIMER_INTERVAL		1000 //1000ms

#define	NX_SCREENSAVER_IMG_POSITION_TIMER_ID				(NX_SCREENSAVER_ITEM_ID + 14)
#define	NX_SCREENSAVER_IMG_POSITION_TIMER_INTERVAL			(1000 * 5) // 5 second


#define	NX_SCREENSAVER_FRAME_ID								(NX_SCREENSAVER_ITEM_ID + 3)
#define	NX_SCREENSAVER_IMG_SCREEN_ID						(NX_SCREENSAVER_ITEM_ID + 4)
#define	NX_SCREENSAVER_IMG_DIM_ID							(NX_SCREENSAVER_ITEM_ID + 5)
#define	NX_SCREENSAVER_IMG_COLON_ID							(NX_SCREENSAVER_ITEM_ID + 6) // Used 6~7
#define	NX_SCREENSAVER_IMG_HOUR_FIRST_NUM					(NX_SCREENSAVER_ITEM_ID + 8)
#define	NX_SCREENSAVER_IMG_HOUR_SECOND_NUM					(NX_SCREENSAVER_ITEM_ID + 9)
#define	NX_SCREENSAVER_IMG_MIN_FIRST_NUM					(NX_SCREENSAVER_ITEM_ID + 10)
#define	NX_SCREENSAVER_IMG_MIN_SECOND_NUM					(NX_SCREENSAVER_ITEM_ID + 11)
#define	NX_SCREENSAVER_IMG_SEC_FIRST_NUM					(NX_SCREENSAVER_ITEM_ID + 12)
#define	NX_SCREENSAVER_IMG_SEC_SECOND_NUM					(NX_SCREENSAVER_ITEM_ID + 13)

#define	NX_SCREENSAVER_FRAME_X								0
#define	NX_SCREENSAVER_FRAME_Y								0
#define	NX_SCREENSAVER_FRAME_W								NX_PRISM_OSD_HORIZONTAL
#define	NX_SCREENSAVER_FRAME_H								NX_PRISM_OSD_VERTICAL

#define	NX_SCREENSAVER_POS_MIN_X							(NX_SCREENSAVER_FRAME_X + 50)
#define	NX_SCREENSAVER_POS_MIN_Y							(NX_SCREENSAVER_FRAME_Y + 50)
#define	NX_SCREENSAVER_POS_MAX_X							(NX_SCREENSAVER_FRAME_W - 50)
#define	NX_SCREENSAVER_POS_MAX_Y							(NX_SCREENSAVER_FRAME_H - 50)

#define	NX_SCREENSAVER_IMG_WHOLE_W							295
#define	NX_SCREENSAVER_IMG_WHOLE_H							67

#define	NX_SCREENSAVER_IMG_SCREEN_W 						67
#define	NX_SCREENSAVER_IMG_SCREEN_H 						67

#define	NX_SCREENSAVER_IMG_DIM_W							218
#define	NX_SCREENSAVER_IMG_DIM_H							47

#define	NX_SCREENSAVER_IMG_BNUMBER_W						30
#define	NX_SCREENSAVER_IMG_BNUMBER_H						47

#define	NX_SCREENSAVER_IMG_NNUMBER_W						27
#define	NX_SCREENSAVER_IMG_NNUMBER_H						47

#define	NX_SCREENSAVER_IMG_COLON_W							15
#define	NX_SCREENSAVER_IMG_COLON_H							47

#define	NX_SCREENSAVER_DEFAULT_IMG_SCREEN_NUM				21
#define	NX_SCREENSAVER_DEFAULT_NUM_SCREEM_NUM 				10


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eSCREENSAVER_HOUR,
	eSCREENSAVER_MIN,
	eSCREENSAVER_SEC
} NxScreenSaver_DisPlayType_e;

typedef struct NxScreenSaver_Contents
{
	HUINT32		ulCurImgScreen;
	HUINT32		ulMaxNumOfImgScreen;
} NxScreenSaver_Contents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxScreenSaver_Contents_t	s_stNxScreenSaver_Contents;

static HCHAR* s_NxScreenSaver_ArrayofImgScreens[NX_SCREENSAVER_DEFAULT_IMG_SCREEN_NUM]={
	RES_511_00_SCREEN00_PNG, RES_511_00_SCREEN01_PNG, RES_511_00_SCREEN02_PNG,RES_511_00_SCREEN03_PNG,
	RES_511_00_SCREEN04_PNG, RES_511_00_SCREEN05_PNG, RES_511_00_SCREEN06_PNG,RES_511_00_SCREEN07_PNG,
	RES_511_00_SCREEN08_PNG, RES_511_00_SCREEN09_PNG, RES_511_00_SCREEN10_PNG,RES_511_00_SCREEN11_PNG,
	RES_511_00_SCREEN12_PNG, RES_511_00_SCREEN13_PNG, RES_511_00_SCREEN14_PNG,RES_511_00_SCREEN15_PNG,
	RES_511_00_SCREEN16_PNG, RES_511_00_SCREEN17_PNG, RES_511_00_SCREEN18_PNG,RES_511_00_SCREEN19_PNG,
	RES_511_00_SCREEN20_PNG
};

static HCHAR* s_NxScreenSaver_ArrayofBNumScreens[NX_SCREENSAVER_DEFAULT_NUM_SCREEM_NUM]={
	RES_510_00_SCREEN_NUM_B0_PNG, RES_510_00_SCREEN_NUM_B1_PNG, RES_510_00_SCREEN_NUM_B2_PNG,
	RES_510_00_SCREEN_NUM_B3_PNG,RES_510_00_SCREEN_NUM_B4_PNG,RES_510_00_SCREEN_NUM_B5_PNG,
	RES_510_00_SCREEN_NUM_B6_PNG,RES_510_00_SCREEN_NUM_B7_PNG,RES_510_00_SCREEN_NUM_B8_PNG,
	RES_510_00_SCREEN_NUM_B9_PNG
};

static HCHAR* s_NxScreenSaver_ArrayofNNumScreens[NX_SCREENSAVER_DEFAULT_NUM_SCREEM_NUM]={
	RES_510_00_SCREEN_NUM_N0_PNG, RES_510_00_SCREEN_NUM_N1_PNG, RES_510_00_SCREEN_NUM_N2_PNG,
	RES_510_00_SCREEN_NUM_N3_PNG,RES_510_00_SCREEN_NUM_N4_PNG,RES_510_00_SCREEN_NUM_N5_PNG,
	RES_510_00_SCREEN_NUM_N6_PNG,RES_510_00_SCREEN_NUM_N7_PNG,RES_510_00_SCREEN_NUM_N8_PNG,
	RES_510_00_SCREEN_NUM_N9_PNG
};

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static NxScreenSaver_Contents_t* nx_ScreenSaver_GetContents(void);
static ONDK_Result_t	nx_ScreenSaver_MsgGwmCreate(NxScreenSaver_Contents_t* pstContents);
static ONDK_Result_t	nx_ScreenSaver_MsgGwmTimer(NxScreenSaver_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_ScreenSaver_MsgGwmKeyDown(NxScreenSaver_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);;
static ONDK_Result_t	nx_ScreenSaver_APKMessage(NxScreenSaver_Contents_t *pstContents, Handle_t hAct,HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3);
static ONDK_Result_t	nx_ScreenSaver_MsgGwmDestroy(NxScreenSaver_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_ScreenSaver_Initialize(NxScreenSaver_Contents_t *pstContents);
static ONDK_Result_t	nx_ScreenSaver_UpdatePosition(NxScreenSaver_Contents_t *pstContents);
static ONDK_Result_t 	nx_ScreenSaver_GenerateRandomPosition(ONDK_Point_t *pstPosition);
static ONDK_Result_t 	nx_ScreenSaver_DisplayCurImgScreen(NxScreenSaver_Contents_t* pstContents);
static ONDK_Result_t	nx_ScreenSaver_AddItemFrame(void);
static	ONDK_Result_t	nx_ScreenSaver_DrawFrame(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t 	nx_ScreenSaver_SetTimerImgScreen(NxScreenSaver_Contents_t* pstContents);
static ONDK_Result_t 	nx_ScreenSaver_SetTimerDisplayTime(void);
static ONDK_Result_t 	nx_ScreenSaver_SetTimerPosition(void);
static ONDK_Result_t	nx_ScreenSaver_Destroy(NxScreenSaver_Contents_t *pstContents);
static ONDK_Result_t 	nx_ScreenSaver_DisplayTime(void);
static ONDK_Result_t 	nx_ScreenSaver_DisplayNumber(NxScreenSaver_DisPlayType_e eDisplayType,HUINT32 ulNumber);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static NxScreenSaver_Contents_t* nx_ScreenSaver_GetContents(void)
{
	return &s_stNxScreenSaver_Contents;
}


static ONDK_Result_t	nx_ScreenSaver_MsgGwmCreate(NxScreenSaver_Contents_t* pstContents)
{
	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	nx_ScreenSaver_AddItemFrame();

	nx_ScreenSaver_Initialize(pstContents);

	nx_ScreenSaver_DisplayCurImgScreen(pstContents);

	nx_ScreenSaver_DisplayTime();

	nx_ScreenSaver_SetTimerImgScreen(pstContents);

	nx_ScreenSaver_SetTimerDisplayTime();

	nx_ScreenSaver_SetTimerPosition();

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_ScreenSaver_MsgGwmTimer(NxScreenSaver_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	if (pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL!\n");
		return ERR_FAIL;
	}

	switch(p1)
	{
		case NX_SCREENSAVER_IMG_SCREEN_TIMER_ID:
			{
				//HxLOG_Print("\n\n[%s:%d] Image Screen Timer fired\n\n",__FUNCTION__,__LINE__);
				pstContents->ulCurImgScreen++;
				if (pstContents->ulCurImgScreen >= pstContents->ulMaxNumOfImgScreen)
				{
					pstContents->ulCurImgScreen = 0;
				}

				if ((pstContents->ulCurImgScreen == 0) || (pstContents->ulCurImgScreen == pstContents->ulMaxNumOfImgScreen -1))
				{
					ONDK_GWM_KillTimer(NX_SCREENSAVER_IMG_SCREEN_TIMER_ID);
					nx_ScreenSaver_SetTimerImgScreen(pstContents);
				}
				nx_ScreenSaver_DisplayCurImgScreen(pstContents);
			}
			break;
		 case NX_SCREENSAVER_IMG_DISPLAY_TIME_TIMER_ID:
		 	nx_ScreenSaver_DisplayTime();
		 	break;
		case NX_SCREENSAVER_IMG_POSITION_TIMER_ID:
			nx_ScreenSaver_UpdatePosition(pstContents);
			break;
		default:
			break;
	}

	return GWM_RET_OK;
}

static ONDK_Result_t	nx_ScreenSaver_MsgGwmKeyDown(NxScreenSaver_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);

	switch (p1)
	{
		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);//destroy ScreenSaver
			return GWM_MESSAGE_PASS;

		default:
			HxLOG_Error("\nDestroy !! NX_PRISM_SCREENSAVER_Proc!!! \n");
			ONDK_GWM_APP_Destroy(0);//destroy ScreenSaver
			return GWM_MESSAGE_BREAK;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
}

static ONDK_Result_t	nx_ScreenSaver_APKMessage(NxScreenSaver_Contents_t *pstContents, Handle_t hAct, HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	ONDK_Result_t				ret		  = ONDK_RET_OK;
	GWM_APK_MSG_Type_e			eAPK_Type = (GWM_APK_MSG_Type_e)hAct;
	OxMediaPlay_Event_e			eEvent	  =	(ulParam1 & 0x00FF);

	if (eAPK_Type == GWM_APK_MEDIA)
	{
		switch(eEvent)
		{
			case eOxMP_EVENT_TUNE_Locked:
				HxLOG_Error("\nHave the Signal \n\n");
				ONDK_GWM_APP_Destroy(0);//destroy ScreenSaver
				break;
			default:
				break;
		}
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, ulParam1, ulParam2, ulParam3);

	return ret;
}


static ONDK_Result_t	nx_ScreenSaver_Initialize(NxScreenSaver_Contents_t *pstContents)
{
	ONDK_Rect_t 	stRectN = { 0, 0, 0, 0 };
	ONDK_Point_t	pPosition = {0,0};

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(NxScreenSaver_Contents_t));

	pstContents->ulMaxNumOfImgScreen	= NX_SCREENSAVER_DEFAULT_IMG_SCREEN_NUM;
	pstContents->ulCurImgScreen 		= 0;

	//generate the random postrion
	nx_ScreenSaver_GenerateRandomPosition(&pPosition);

	//display image Screens
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_SCREEN_W, NX_SCREENSAVER_IMG_SCREEN_H);
	ONDK_GWM_COM_Image_Create(NX_SCREENSAVER_IMG_SCREEN_ID, &stRectN);
	ONDK_GWM_COM_Image_SetImage(NX_SCREENSAVER_IMG_SCREEN_ID, (HCHAR*)(s_NxScreenSaver_ArrayofImgScreens[pstContents->ulCurImgScreen]));
	ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_SCREEN_ID, TRUE);

	//display Dim image
	pPosition.x += (NX_SCREENSAVER_IMG_SCREEN_W) + 10;
	pPosition.y += 3;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_DIM_W, NX_SCREENSAVER_IMG_DIM_H);
	ONDK_GWM_COM_Image_Create(NX_SCREENSAVER_IMG_DIM_ID, &stRectN);
	ONDK_GWM_COM_Image_SetImage(NX_SCREENSAVER_IMG_DIM_ID, (HCHAR*)RES_510_00_SCREEN_DIM_PNG);
	ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_DIM_ID, FALSE);

	//display Hour Image
	//first number
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_BNUMBER_W, NX_SCREENSAVER_IMG_BNUMBER_H);
	ONDK_GWM_COM_Image_Create(NX_SCREENSAVER_IMG_HOUR_FIRST_NUM, &stRectN);
	ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_HOUR_FIRST_NUM, TRUE);

	//second number
	pPosition.x += NX_SCREENSAVER_IMG_BNUMBER_W + 1;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_BNUMBER_W, NX_SCREENSAVER_IMG_BNUMBER_H);
	ONDK_GWM_COM_Image_Create(NX_SCREENSAVER_IMG_HOUR_SECOND_NUM, &stRectN);
	ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_HOUR_SECOND_NUM, TRUE);

	//display two colon 1 image
	pPosition.x += NX_SCREENSAVER_IMG_BNUMBER_W + 1;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_COLON_W, NX_SCREENSAVER_IMG_COLON_H);
	ONDK_GWM_COM_Image_Create(NX_SCREENSAVER_IMG_COLON_ID, &stRectN);
	ONDK_GWM_COM_Image_SetImage(NX_SCREENSAVER_IMG_COLON_ID, (HCHAR*)RES_510_00_SCREEN_COLON_PNG);
	ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_COLON_ID, TRUE);

	//display Minute Image
	//first number
	pPosition.x += NX_SCREENSAVER_IMG_COLON_W + 1;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_BNUMBER_W, NX_SCREENSAVER_IMG_BNUMBER_H);
	ONDK_GWM_COM_Image_Create(NX_SCREENSAVER_IMG_MIN_FIRST_NUM, &stRectN);
	ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_MIN_FIRST_NUM, TRUE);

	//second number
	pPosition.x += NX_SCREENSAVER_IMG_BNUMBER_W + 1;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_BNUMBER_W, NX_SCREENSAVER_IMG_BNUMBER_H);
	ONDK_GWM_COM_Image_Create(NX_SCREENSAVER_IMG_MIN_SECOND_NUM, &stRectN);
	ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_MIN_SECOND_NUM, TRUE);


	//display colon 2 image
	pPosition.x += NX_SCREENSAVER_IMG_BNUMBER_W + 1;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_COLON_W, NX_SCREENSAVER_IMG_COLON_H);
	ONDK_GWM_COM_Image_Create(NX_SCREENSAVER_IMG_COLON_ID + 1, &stRectN);
	ONDK_GWM_COM_Image_SetImage(NX_SCREENSAVER_IMG_COLON_ID + 1, (HCHAR*)RES_510_00_SCREEN_COLON_PNG);
	ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_COLON_ID + 1, TRUE);

	//display Second Image
	//first number
	pPosition.x += NX_SCREENSAVER_IMG_COLON_W + 1;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_NNUMBER_W, NX_SCREENSAVER_IMG_NNUMBER_H);
	ONDK_GWM_COM_Image_Create(NX_SCREENSAVER_IMG_SEC_FIRST_NUM, &stRectN);
	ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_SEC_FIRST_NUM, TRUE);

	//second number
	pPosition.x += NX_SCREENSAVER_IMG_NNUMBER_W + 1;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_NNUMBER_W, NX_SCREENSAVER_IMG_NNUMBER_H);
	ONDK_GWM_COM_Image_Create(NX_SCREENSAVER_IMG_SEC_SECOND_NUM, &stRectN);
	ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_SEC_SECOND_NUM, TRUE);

	return ONDK_RET_OK;

}

static ONDK_Result_t	nx_ScreenSaver_UpdatePosition(NxScreenSaver_Contents_t *pstContents)
{
	ONDK_Rect_t 		stRectN = { 0, 0, 0, 0 };
	ONDK_Point_t	pPosition = {0,0};

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}

	//generate the random postrion
	nx_ScreenSaver_GenerateRandomPosition(&pPosition);

	//display image Screens
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_SCREEN_W, NX_SCREENSAVER_IMG_SCREEN_H);
	ONDK_GWM_COM_Image_ChangeRect(NX_SCREENSAVER_IMG_SCREEN_ID, &stRectN);

	//display Dim image
	pPosition.x += (NX_SCREENSAVER_IMG_SCREEN_W) + 10;
	pPosition.y += 3;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_DIM_W, NX_SCREENSAVER_IMG_DIM_H);
	ONDK_GWM_COM_Image_ChangeRect(NX_SCREENSAVER_IMG_DIM_ID, &stRectN);

	//display Hour Image
	//first number
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_BNUMBER_W, NX_SCREENSAVER_IMG_BNUMBER_H);
	ONDK_GWM_COM_Image_ChangeRect(NX_SCREENSAVER_IMG_HOUR_FIRST_NUM, &stRectN);

	//second number
	pPosition.x += NX_SCREENSAVER_IMG_BNUMBER_W + 1;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_BNUMBER_W, NX_SCREENSAVER_IMG_BNUMBER_H);
	ONDK_GWM_COM_Image_ChangeRect(NX_SCREENSAVER_IMG_HOUR_SECOND_NUM, &stRectN);

	//display two colon 1 image
	pPosition.x += NX_SCREENSAVER_IMG_BNUMBER_W + 1;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_COLON_W, NX_SCREENSAVER_IMG_COLON_H);
	ONDK_GWM_COM_Image_ChangeRect(NX_SCREENSAVER_IMG_COLON_ID, &stRectN);

	//display Minute Image
	//first number
	pPosition.x += NX_SCREENSAVER_IMG_COLON_W + 1;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_BNUMBER_W, NX_SCREENSAVER_IMG_BNUMBER_H);
	ONDK_GWM_COM_Image_ChangeRect(NX_SCREENSAVER_IMG_MIN_FIRST_NUM, &stRectN);

	//second number
	pPosition.x += NX_SCREENSAVER_IMG_BNUMBER_W + 1;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_BNUMBER_W, NX_SCREENSAVER_IMG_BNUMBER_H);
	ONDK_GWM_COM_Image_ChangeRect(NX_SCREENSAVER_IMG_MIN_SECOND_NUM, &stRectN);

	//display colon 2 image
	pPosition.x += NX_SCREENSAVER_IMG_BNUMBER_W + 1;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_COLON_W, NX_SCREENSAVER_IMG_COLON_H);
	ONDK_GWM_COM_Image_ChangeRect(NX_SCREENSAVER_IMG_COLON_ID + 1, &stRectN);

	//display Second Image
	//first number
	pPosition.x += NX_SCREENSAVER_IMG_COLON_W + 1;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_NNUMBER_W, NX_SCREENSAVER_IMG_NNUMBER_H);
	ONDK_GWM_COM_Image_ChangeRect(NX_SCREENSAVER_IMG_SEC_FIRST_NUM, &stRectN);

	//second number
	pPosition.x += NX_SCREENSAVER_IMG_NNUMBER_W + 1;
	stRectN = ONDK_Rect(pPosition.x, pPosition.y, NX_SCREENSAVER_IMG_NNUMBER_W, NX_SCREENSAVER_IMG_NNUMBER_H);
	ONDK_GWM_COM_Image_ChangeRect(NX_SCREENSAVER_IMG_SEC_SECOND_NUM, &stRectN);

	return ONDK_RET_OK;

}


static ONDK_Result_t	nx_ScreenSaver_GenerateRandomPosition(ONDK_Point_t *pstPosition)
{
	HUINT32 ulPosMinX = NX_SCREENSAVER_POS_MIN_X;
	HUINT32 ulPosMaxX = (NX_SCREENSAVER_POS_MAX_X - NX_SCREENSAVER_IMG_WHOLE_W);
	HUINT32 ulPosMinY = NX_SCREENSAVER_POS_MIN_Y;
	HUINT32 ulPosMaxY = (NX_SCREENSAVER_POS_MAX_Y - NX_SCREENSAVER_IMG_WHOLE_H);

	pstPosition->x = (HLIB_MATH_Random()%(ulPosMaxX - ulPosMinX)) + ulPosMinX;
	pstPosition->y = (HLIB_MATH_Random()%(ulPosMaxY - ulPosMinY)) + ulPosMinY;

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_ScreenSaver_DisplayCurImgScreen(NxScreenSaver_Contents_t* pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);

		return ONDK_RET_INITFAIL;
	}
	ONDK_GWM_COM_Image_SetImage(NX_SCREENSAVER_IMG_SCREEN_ID, (HCHAR*)(s_NxScreenSaver_ArrayofImgScreens[pstContents->ulCurImgScreen]));
    ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_SCREEN_ID, TRUE);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_ScreenSaver_DisplayTime(void)
{
	HERROR				hError = ERR_FAIL;
	HUINT32 			ulUnixtime = 0;
	HxDATETIME_t 		DateTime;

	hError = NX_SYSTEM_GetTime(&ulUnixtime);
	if (hError != ERR_OK)
	{
		HxLOG_Print("\n\n[%s:%d] cannot get system time\n\n",__FUNCTION__,__LINE__);
		ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_DIM_ID, TRUE);
		ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_COLON_ID, FALSE);
		ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_COLON_ID + 1, FALSE);

		return ONDK_RET_FAIL;
	}
	ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_DIM_ID, FALSE);
	ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_COLON_ID, TRUE);
	ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_COLON_ID + 1, TRUE);

	HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUnixtime, &DateTime);

	nx_ScreenSaver_DisplayNumber(eSCREENSAVER_HOUR, DateTime.stTime.ucHour);
	nx_ScreenSaver_DisplayNumber(eSCREENSAVER_MIN, DateTime.stTime.ucMinute);
	nx_ScreenSaver_DisplayNumber(eSCREENSAVER_SEC, DateTime.stTime.ucSecond);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_ScreenSaver_DisplayNumber(NxScreenSaver_DisPlayType_e eDisplayType,HUINT32 ulNumber)
{
	HUINT32		ulFirstNumber = 0;
	HUINT32 	ulSecondNumber = 0;

	ulFirstNumber = ulNumber / 10;
	ulSecondNumber = ulNumber - (ulFirstNumber * 10);

	switch (eDisplayType)
	{
		case eSCREENSAVER_HOUR:
			ONDK_GWM_COM_Image_SetImage(NX_SCREENSAVER_IMG_HOUR_FIRST_NUM, (HCHAR*)(s_NxScreenSaver_ArrayofBNumScreens[ulFirstNumber]));
   		    ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_HOUR_FIRST_NUM, TRUE);

			ONDK_GWM_COM_Image_SetImage(NX_SCREENSAVER_IMG_HOUR_SECOND_NUM, (HCHAR*)(s_NxScreenSaver_ArrayofBNumScreens[ulSecondNumber]));
    		ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_HOUR_SECOND_NUM, TRUE);
			break;
		case eSCREENSAVER_MIN:
			ONDK_GWM_COM_Image_SetImage(NX_SCREENSAVER_IMG_MIN_FIRST_NUM, (HCHAR*)(s_NxScreenSaver_ArrayofBNumScreens[ulFirstNumber]));
    		ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_MIN_FIRST_NUM, TRUE);

			ONDK_GWM_COM_Image_SetImage(NX_SCREENSAVER_IMG_MIN_SECOND_NUM, (HCHAR*)(s_NxScreenSaver_ArrayofBNumScreens[ulSecondNumber]));
    		ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_MIN_SECOND_NUM, TRUE);
			break;
		case eSCREENSAVER_SEC:
			ONDK_GWM_COM_Image_SetImage(NX_SCREENSAVER_IMG_SEC_FIRST_NUM, (HCHAR*)(s_NxScreenSaver_ArrayofNNumScreens[ulFirstNumber]));
    		ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_SEC_FIRST_NUM, TRUE);

			ONDK_GWM_COM_Image_SetImage(NX_SCREENSAVER_IMG_SEC_SECOND_NUM, (HCHAR*)(s_NxScreenSaver_ArrayofNNumScreens[ulSecondNumber]));
    		ONDK_GWM_Obj_SetVisible(NX_SCREENSAVER_IMG_SEC_SECOND_NUM, TRUE);
			break;
		default:
			break;

	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_ScreenSaver_AddItemFrame(void)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	// Set - Frame
	ONDK_GWM_APP_SetAppArea(NX_SCREENSAVER_FRAME_X, NX_SCREENSAVER_FRAME_Y, NX_SCREENSAVER_FRAME_W, NX_SCREENSAVER_FRAME_H);

	// Draw - Background Frame
	stRect = ONDK_Rect(NX_SCREENSAVER_FRAME_X, NX_SCREENSAVER_FRAME_Y, NX_SCREENSAVER_FRAME_W, NX_SCREENSAVER_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_SCREENSAVER_FRAME_ID, &stRect, COL(C_G_Bang_100));
	//ONDK_GWM_APP_SetObjectDrawMethod(NX_SCREENSAVER_FRAME_ID, nx_ScreenSaver_DrawFrame);

	return	ONDK_RET_OK;
}


static	ONDK_Result_t	nx_ScreenSaver_DrawFrame(ONDKSurfaceHandle screen, ONDK_Region_t *area,
									ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };

	stRect.x = area->x1 + rect->x;
	stRect.y = area->y1 + rect->y;
	stRect.w = rect->w;
	stRect.h = rect->h;


	//ONDK_GUICOM_HBox(screen, stRect, RES_105_00_BG_BOTTOM_400_PNG, RES_105_00_BG_BOTTOM_400_PNG, RES_105_00_BG_BOTTOM_400_PNG);

	ONDK_DRAW_FillRect(screen, stRect, ONDK_Color(255, 0, 0, 0));

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_ScreenSaver_SetTimerImgScreen(NxScreenSaver_Contents_t* pstContents)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		HxLOG_Error("pstContents is NULL!\n");
		return ERR_FAIL;
	}

	if (pstContents->ulCurImgScreen == (pstContents->ulMaxNumOfImgScreen - 1))
	{
		//set 400ms for timer
		retTimer = ONDK_GWM_SetTimer(NX_SCREENSAVER_IMG_SCREEN_TIMER_ID, NX_SCREENSAVER_IMG_SCREEN_TIMER2_INTERVAL);
		if (retTimer == GWM_RET_LINK_CONFLICT)
		{
			ONDK_GWM_ResetTimer(NX_SCREENSAVER_IMG_SCREEN_TIMER_ID);
		}
	}
	else
	{
		//set 300ms for timer
		retTimer = ONDK_GWM_SetTimer(NX_SCREENSAVER_IMG_SCREEN_TIMER_ID, NX_SCREENSAVER_IMG_SCREEN_TIMER1_INTERVAL);
		if (retTimer == GWM_RET_LINK_CONFLICT)
		{
			ONDK_GWM_ResetTimer(NX_SCREENSAVER_IMG_SCREEN_TIMER_ID);
		}
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_ScreenSaver_SetTimerDisplayTime(void)
{
	ONDK_Result_t	retTimer = ONDK_RET_FAIL;

	//set 100ms for timer
	retTimer = ONDK_GWM_SetTimer(NX_SCREENSAVER_IMG_DISPLAY_TIME_TIMER_ID, NX_SCREENSAVER_IMG_DISPLAY_TIME_TIMER_INTERVAL);
	if (retTimer == GWM_RET_LINK_CONFLICT)
	{
		ONDK_GWM_ResetTimer(NX_SCREENSAVER_IMG_DISPLAY_TIME_TIMER_ID);
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t nx_ScreenSaver_SetTimerPosition()
{

	ONDK_Result_t	retTimer = ONDK_RET_FAIL;

	//set 100ms for timer
	retTimer = ONDK_GWM_SetTimer(NX_SCREENSAVER_IMG_POSITION_TIMER_ID, NX_SCREENSAVER_IMG_POSITION_TIMER_INTERVAL);
	if (retTimer == GWM_RET_LINK_CONFLICT)
	{
		ONDK_GWM_ResetTimer(NX_SCREENSAVER_IMG_POSITION_TIMER_ID);
	}


	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_ScreenSaver_MsgGwmDestroy(NxScreenSaver_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	nx_ScreenSaver_Destroy(pstContents);
}


static ONDK_Result_t	nx_ScreenSaver_Destroy(NxScreenSaver_Contents_t *pstContents)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	ONDK_GWM_KillTimer(NX_SCREENSAVER_IMG_SCREEN_TIMER_ID); //kill image screen timer
	ONDK_GWM_KillTimer(NX_SCREENSAVER_IMG_DISPLAY_TIME_TIMER_ID); //kill Display Time timer
	ONDK_GWM_KillTimer(NX_SCREENSAVER_IMG_POSITION_TIMER_ID); //kill Position timer

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_SCREENSAVER_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	NxScreenSaver_Contents_t	*pstContents = nx_ScreenSaver_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE\n");
			eResult = nx_ScreenSaver_MsgGwmCreate(pstContents);
			break;
		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER\n");
			eResult = nx_ScreenSaver_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY\n");
			eResult = nx_ScreenSaver_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_ScreenSaver_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		default:
			break;

	}
	if (eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

