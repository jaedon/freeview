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


#define DEF_ONDK_TEST_O2ANI_X  0
#define DEF_ONDK_TEST_O2ANI_Y  0
#define DEF_ONDK_TEST_O2ANI_W  1280
#define DEF_ONDK_TEST_O2ANI_H  720

#define DEF_ONDK_TEST_O2ANI_DATA_X  20
#define DEF_ONDK_TEST_O2ANI_DATA_Y  200
#define DEF_ONDK_TEST_O2ANI_DATA_W  1220
#define DEF_ONDK_TEST_O2ANI_DATA_H  500

#define DEF_ONDK_TEST_O2ANI_BOX_X  40
#define DEF_ONDK_TEST_O2ANI_BOX_Y  600
#define DEF_ONDK_TEST_O2ANI_BOX_W  80
#define DEF_ONDK_TEST_O2ANI_BOX_H  80


#define DEF_ONDK_TEST_O2ANI_BTN_W  120
#define DEF_ONDK_TEST_O2ANI_BTN_H  60

#define DEF_ONDK_TEST_O2ANI_BTN1_X  10
#define DEF_ONDK_TEST_O2ANI_BTN1_Y  10

#define DEF_ONDK_TEST_O2ANI_BTN2_X  (DEF_ONDK_TEST_O2ANI_BTN1_X+DEF_ONDK_TEST_O2ANI_BTN_W+10)
#define DEF_ONDK_TEST_O2ANI_BTN2_Y  10

#define DEF_ONDK_TEST_O2ANI_BTN3_X  (DEF_ONDK_TEST_O2ANI_BTN2_X+DEF_ONDK_TEST_O2ANI_BTN_W+10)
#define DEF_ONDK_TEST_O2ANI_BTN3_Y  10

#define DEF_ONDK_TEST_O2ANI_BTN4_X  (DEF_ONDK_TEST_O2ANI_BTN3_X+DEF_ONDK_TEST_O2ANI_BTN_W+10)
#define DEF_ONDK_TEST_O2ANI_BTN4_Y  10

#define DEF_ONDK_TEST_O2ANI_BTN5_X  (DEF_ONDK_TEST_O2ANI_BTN4_X+DEF_ONDK_TEST_O2ANI_BTN_W+10)
#define DEF_ONDK_TEST_O2ANI_BTN5_Y  10

#define DEF_ONDK_TEST_BOX_O2ANI_PATH		"/usr/image/209_Wide_Option_169_Auto_C.png"
#define DEF_ONDK_TEST_SHOOT_O2ANI_PATH		"/usr/image/205_REC_Recording.png"

#define DEF_ONDK_TEST_O2ANI_O2ANI_ID		0x820610
#define DEF_ONDK_TEST_ANIMATION_O2ANI_ID	0x820611

#define DEF_ONDK_TEST_O2ANI_ITEM_ID			0x820610


typedef enum tagTESTO2ANI_Object
{
	eTESTO2ANI_Object_FRAME_Background = 0x991,
	eTESTO2ANI_Object_RECT_DATA,
	eTESTO2ANI_Object_RECT_DataOutLine,
	eTESTO2ANI_Object_BOX,

	eTESTO2ANI_Object_BTN_DrawLeft,
	eTESTO2ANI_Object_BTN_DrawRight,
	eTESTO2ANI_Object_BTN_DrawShoot,
	eTESTO2ANI_Object_BTN_DrawNone,

	eTESTO2ANI_Object_BTN_MAX
}TESTO2ANI_Object_e;


typedef struct tagTESTO2ANI_Contents_t
{
	HCHAR			szText[2048];

	ONDK_Rect_t 	boxRect;
	HINT32			boxAlpha;
	ONDKImageHandle	hBoxImg;

	HUINT32			sysLastTick;
} TESTO2ANI_Contents_t;

STATIC TESTO2ANI_Contents_t *s_pstTestFontContents = NULL;

/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/


STATIC TESTO2ANI_Contents_t *local_testo2ani_GetContents(void)
{
	if(NULL == s_pstTestFontContents)
	{
		s_pstTestFontContents = ONDK_Calloc(sizeof(TESTO2ANI_Contents_t));
	}
	return s_pstTestFontContents;
}


STATIC void local_testo2ani_SetUiObject_CreateButton(HUINT32 id, HCHAR *name, ONDK_Rect_t rect)
{
	ONDK_GWM_COM_Button_Create(id , &rect, name);
	ONDK_GWM_COM_Button_SetTitleSize(id , 14);
	ONDK_GWM_COM_Button_SetFontColor(id , 0xFF00FF00, 0xFF00FF00);
	ONDK_GWM_COM_Button_SetButtonColor(id , 0xFF0000A0, 0xFF000060, 0xFF0000FF, 0xFF000020);
	ONDK_GWM_COM_Button_SetAlign (id, ONDKSTF_CENTER);
}


ONDK_Result_t local_testo2ani_DrawBox (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	ONDK_Rect_t 	boxRect;
	HINT32			alpha = 0;
	ONDK_Result_t	oResult = ONDK_RET_OK;

	TESTO2ANI_Contents_t 	*pstContents = local_testo2ani_GetContents();

	oResult = ONDK_GWM_O2ANI_GetValue(DEF_ONDK_TEST_O2ANI_ITEM_ID, &boxRect, &alpha);
	if(ONDK_RET_OK != oResult)
	{
		boxRect = pstContents->boxRect;
		alpha = pstContents->boxAlpha;
	}
	ONDK_IMG_StretchBlitAlpha(pstContents->hBoxImg, screen, boxRect, alpha);

	return	ONDK_RET_OK;
}

ONDK_Result_t local_testo2ani_DrawMoveFPS (ONDKSurfaceHandle screen, ONDK_Region_t *area,
								ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	return	ONDK_RET_OK;
}


STATIC void local_testo2ani_SetUiObject(TESTO2ANI_Contents_t *pstContents)
{
	ONDK_Rect_t 		rect;
	ONDK_Size_t			size;
	ONDKImageHandle		hImg;

	ONDK_GWM_APP_SetAppArea(DEF_ONDK_TEST_O2ANI_X, DEF_ONDK_TEST_O2ANI_Y, DEF_ONDK_TEST_O2ANI_W, DEF_ONDK_TEST_O2ANI_H);
#if 0
	rect = ONDK_Rect(DEF_ONDK_TEST_O2ANI_X, DEF_ONDK_TEST_O2ANI_Y, DEF_ONDK_TEST_O2ANI_W, DEF_ONDK_TEST_O2ANI_H);
	ONDK_GWM_COM_Rect_Create(eTESTO2ANI_Object_FRAME_Background, &rect, 0xFFFFFFFF);
#endif
	rect = ONDK_Rect(DEF_ONDK_TEST_O2ANI_BTN1_X, DEF_ONDK_TEST_O2ANI_BTN1_Y, DEF_ONDK_TEST_O2ANI_BTN_W, DEF_ONDK_TEST_O2ANI_BTN_H);
	local_testo2ani_SetUiObject_CreateButton(eTESTO2ANI_Object_BTN_DrawLeft, "<", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_O2ANI_BTN2_X, DEF_ONDK_TEST_O2ANI_BTN2_Y, DEF_ONDK_TEST_O2ANI_BTN_W, DEF_ONDK_TEST_O2ANI_BTN_H);
	local_testo2ani_SetUiObject_CreateButton(eTESTO2ANI_Object_BTN_DrawRight, ">", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_O2ANI_BTN3_X, DEF_ONDK_TEST_O2ANI_BTN3_Y, DEF_ONDK_TEST_O2ANI_BTN_W, DEF_ONDK_TEST_O2ANI_BTN_H);
	local_testo2ani_SetUiObject_CreateButton(eTESTO2ANI_Object_BTN_DrawShoot, "^", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_O2ANI_BTN4_X, DEF_ONDK_TEST_O2ANI_BTN4_Y, DEF_ONDK_TEST_O2ANI_BTN_W, DEF_ONDK_TEST_O2ANI_BTN_H);
	local_testo2ani_SetUiObject_CreateButton(eTESTO2ANI_Object_BTN_DrawNone, "-", rect);

	rect = ONDK_Rect(DEF_ONDK_TEST_O2ANI_DATA_X, DEF_ONDK_TEST_O2ANI_DATA_Y, DEF_ONDK_TEST_O2ANI_DATA_W, DEF_ONDK_TEST_O2ANI_DATA_H);
	ONDK_GWM_COM_Rect_Create(eTESTO2ANI_Object_RECT_DataOutLine, &rect, 0xFF333333);
	ONDK_GWM_APP_SetObjectDrawMethod(eTESTO2ANI_Object_RECT_DataOutLine, local_testo2ani_DrawMoveFPS);

	hImg = ONDK_IMG_CreateSelf(DEF_ONDK_TEST_BOX_O2ANI_PATH);
	ONDK_Assert(hImg);
	pstContents->hBoxImg = hImg;
	size = ONDK_IMG_GetSize(hImg);
	rect = ONDK_Rect(DEF_ONDK_TEST_O2ANI_BOX_X, DEF_ONDK_TEST_O2ANI_BOX_Y, size.w, size.h);
	pstContents->boxRect = rect;
	pstContents->boxAlpha = 255;

	rect = ONDK_Rect(DEF_ONDK_TEST_O2ANI_DATA_X, DEF_ONDK_TEST_O2ANI_DATA_Y, DEF_ONDK_TEST_O2ANI_DATA_W, DEF_ONDK_TEST_O2ANI_DATA_H);
	ONDK_GWM_COM_Rect_Create(eTESTO2ANI_Object_BOX, &rect, 0xFF000000);
	ONDK_GWM_APP_SetObjectDrawMethod(eTESTO2ANI_Object_BOX, local_testo2ani_DrawBox);


	ONDK_GWM_Obj_SetFocus (eTESTO2ANI_Object_BTN_DrawLeft);
}

/**************************************************************************************************/
#define _________AP_Volume_Private_Proc_MSG_RUNNIG__________________________________________________________
/**************************************************************************************************/


//MSG_GWM_CREATE
STATIC ONDK_Result_t local_testo2ani_MsgGwmCreate(TESTO2ANI_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	local_testo2ani_SetUiObject(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}

#define DEF_TEST_MOVE_POS	100
#define DEF_TEST_MOVE_SPEED	10

//MSG_GWM_KEYDOWN
STATIC ONDK_Result_t local_testo2ani_MsgGwmKeyDown(TESTO2ANI_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 nKeyId = p1;

	ONDK_GWM_O2Ani_Settings_t 		stO2AniSettings;

	switch(nKeyId)
	{
	case KEY_EXIT:
	case KEY_BACK:
		ONDK_GWM_APP_Destroy(NULL);
		return GWM_MESSAGE_BREAK;

	case KEY_ARROWRIGHT:
		ONDK_GWM_O2ANI_GetDefaultSettings((eONDK_O2ANITYPE_MoveTo | eONDK_O2ANITYPE_Fade), &stO2AniSettings, &pstContents->boxRect);
		stO2AniSettings.rtEnd.x += DEF_TEST_MOVE_POS;
		stO2AniSettings.nSpeed 	= DEF_TEST_MOVE_SPEED;
		ONDK_GWM_O2ANI_AddItem(DEF_ONDK_TEST_O2ANI_ITEM_ID, &stO2AniSettings, DEF_O2ANI_EFFECT_IGNORE_KEYPROCESS);
		pstContents->boxRect = stO2AniSettings.rtEnd;
		return GWM_MESSAGE_BREAK;

	case KEY_ARROWLEFT:
		ONDK_GWM_O2ANI_GetDefaultSettings((eONDK_O2ANITYPE_MoveTo | eONDK_O2ANITYPE_Fade), &stO2AniSettings, &pstContents->boxRect);
		stO2AniSettings.rtEnd.x -= DEF_TEST_MOVE_POS;
		stO2AniSettings.nSpeed 	= DEF_TEST_MOVE_SPEED;
		ONDK_GWM_O2ANI_AddItem(DEF_ONDK_TEST_O2ANI_ITEM_ID, &stO2AniSettings, DEF_O2ANI_EFFECT_IGNORE_KEYPROCESS);
		pstContents->boxRect = stO2AniSettings.rtEnd;
		return GWM_MESSAGE_BREAK;

	case KEY_ARROWDOWN:
		ONDK_GWM_O2ANI_GetDefaultSettings((eONDK_O2ANITYPE_MoveTo | eONDK_O2ANITYPE_Fade), &stO2AniSettings, &pstContents->boxRect);
		stO2AniSettings.rtEnd.y += DEF_TEST_MOVE_POS;
		stO2AniSettings.rtEnd.w -= 20;
//		stO2AniSettings.rtEnd.h -= 20;
		stO2AniSettings.nSpeed 	= DEF_TEST_MOVE_SPEED;
		ONDK_GWM_O2ANI_AddItem(DEF_ONDK_TEST_O2ANI_ITEM_ID, &stO2AniSettings, DEF_O2ANI_EFFECT_IGNORE_KEYPROCESS);
		pstContents->boxRect = stO2AniSettings.rtEnd;
		return GWM_MESSAGE_BREAK;

	case KEY_ARROWUP:
		ONDK_GWM_O2ANI_GetDefaultSettings((eONDK_O2ANITYPE_MoveTo | eONDK_O2ANITYPE_Fade), &stO2AniSettings, &pstContents->boxRect);
		stO2AniSettings.rtEnd.y -= DEF_TEST_MOVE_POS;
		stO2AniSettings.rtEnd.w += 20;
//		stO2AniSettings.rtEnd.h += 20;
		stO2AniSettings.nSpeed 	= DEF_TEST_MOVE_SPEED;
		ONDK_GWM_O2ANI_AddItem(DEF_ONDK_TEST_O2ANI_ITEM_ID, &stO2AniSettings, DEF_O2ANI_EFFECT_IGNORE_KEYPROCESS);
		pstContents->boxRect = stO2AniSettings.rtEnd;
		return GWM_MESSAGE_BREAK;

	case KEY_OK:
		ONDK_GWM_O2ANI_GetDefaultSettings((eONDK_O2ANITYPE_MoveTo | eONDK_O2ANITYPE_Fade), &stO2AniSettings, &pstContents->boxRect);
		stO2AniSettings.nSpeed 	= DEF_TEST_MOVE_SPEED;
		stO2AniSettings.nStartAlpha = 100;
		stO2AniSettings.nEndAlpha = 255;
		ONDK_GWM_O2ANI_AddItem(DEF_ONDK_TEST_O2ANI_ITEM_ID, &stO2AniSettings, nKeyId);
		pstContents->boxAlpha = 255;
		return GWM_MESSAGE_BREAK;

	default:
		break;
	}
	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}


//MSG_GWM_KEYDOWN
STATIC ONDK_Result_t local_testo2ani_MsgGwmClicked(TESTO2ANI_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	//HUINT32 nCmdId = (HUINT32)p1;
	HUINT32 nCmdId = (HUINT32)p1;

	ONDK_Error("[%s:%d] click (%d, %s) \r\n", __FUNCTION__, __LINE__, nCmdId, ONDK_GWM_COM_Button_GetTitle(nCmdId));

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testo2ani_MsgGwmDestroy(TESTO2ANI_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);


	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testo2ani_MsgGwmScrollRight(TESTO2ANI_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	ONDK_GWM_Obj_SetFocus (eTESTO2ANI_Object_BTN_DrawLeft);

	return GWM_RET_OK;
}

//MSG_GWM_DESTROY
STATIC ONDK_Result_t local_testo2ani_MsgGwmScrollLeft(TESTO2ANI_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Assert(pstContents);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	ONDK_GWM_Obj_SetFocus ((eTESTO2ANI_Object_BTN_MAX-1));

	return GWM_RET_OK;
}

//MSG_GWM_O2ANIMATION_END
STATIC ONDK_Result_t local_testo2ani_MsgGwmO2AniEffectEnd(TESTO2ANI_Contents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32 nLastKey = p1;
	HINT32 o2aniId = p2;

	ONDK_Assert(pstContents);

	/*
		ONDK_Result_t	 ONDK_GWM_O2ANI_AddItem(HINT32 o2aniId, ONDK_GWM_O2Ani_Settings_t *pstO2AniSettings, HINT32 nLastKey)
	*/

	ONDK_Error("[%s:%d] effect end nLastKey(0x%x), o2aniId(%d) \r\n", __FUNCTION__, __LINE__, nLastKey, o2aniId);

	ONDK_TEST_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}

/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/

ONDK_Result_t ONDK_TEST_O2ANI_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult	= GWM_RET_OK;
	TESTO2ANI_Contents_t 	*pstContents = local_testo2ani_GetContents();

	switch (nMessage)
	{
	case MSG_GWM_CREATE:
		eResult = local_testo2ani_MsgGwmCreate(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_KEYDOWN:
		eResult = local_testo2ani_MsgGwmKeyDown(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_CLICKED:
		eResult = local_testo2ani_MsgGwmClicked(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_DESTROY:
		eResult = local_testo2ani_MsgGwmDestroy(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_SCROLLRIGHT:
		eResult = local_testo2ani_MsgGwmScrollRight(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_SCROLLLEFT:
		eResult = local_testo2ani_MsgGwmScrollLeft(pstContents, p1, p2, p3);
		break;
	case MSG_GWM_O2ANIMATION_END:
		eResult = local_testo2ani_MsgGwmO2AniEffectEnd(pstContents, p1, p2, p3);
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

void ONDK_TEST_O2ANI_Start(void)
{
	if(NULL == ONDK_GWM_APP_Get(ONDK_TEST_O2ANI_Proc))
	{
		HLIB_CMD_Printf("[%s:%d] Create \r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "ONDK_TEST_O2ANI_Proc", APP_INDEPENDENT_OSD_GRP_PRIORITY, ONDK_TEST_O2ANI_Proc, 0, 0, 0, 0);
	}
	else
	{
		HLIB_CMD_Printf("[%s:%d] Destroy\r\n", __FUNCTION__, __LINE__);
		ONDK_GWM_APP_Destroy(ONDK_TEST_O2ANI_Proc);
	}
}



