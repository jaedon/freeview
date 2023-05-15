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
 * @file	  		nx_prism_not_available.c
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

#include <nx_common.h>
#include <nx_prism_app.h>
#include <nx_port.h>
#include <nx_port_system.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/

#define	NX_PRISM_RECSTART_UI_ITEM_ID					eMenuFrame_Window_SubttlUI

#define	NX_PRISM_RECSTART_UI_EXPIRE_TIMER_ID			(NX_PRISM_RECSTART_UI_ITEM_ID + 1)
#define	NX_PRISM_RECSTART_UI_EXPIRE_TIME				(3 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_RECSTART_UI_WINDOW_X				290
#define	NX_PRISM_RECSTART_UI_WINDOW_Y				0
#define	NX_PRISM_RECSTART_UI_WINDOW_W				700
#define	NX_PRISM_RECSTART_UI_WINDOW_H				133

#define	NX_PRISM_RECSTART_UI_FRAME_ID					(NX_PRISM_RECSTART_UI_ITEM_ID + 2)
#define	NX_PRISM_RECSTART_UI_FRAME_X					0
#define	NX_PRISM_RECSTART_UI_FRAME_Y					0
#define	NX_PRISM_RECSTART_UI_FRAME_W					NX_PRISM_RECSTART_UI_WINDOW_W
#define	NX_PRISM_RECSTART_UI_FRAME_H					NX_PRISM_RECSTART_UI_WINDOW_H

#define	NX_PRISM_RECSTART_UI_TEXT_ITEM_ID				(NX_PRISM_RECSTART_UI_ITEM_ID + 3)
#define	NX_PRISM_RECSTART_UI_TEXT_ITEM_X				130
#define	NX_PRISM_RECSTART_UI_TEXT_ITEM_Y				(71 - 22)
#define	NX_PRISM_RECSTART_UI_TEXT_ITEM_W				540
#define	NX_PRISM_RECSTART_UI_TEXT_ITEM_H				(32)

#define	NX_PRISM_RECSTART_UI_TEXT2_ITEM_ID			(NX_PRISM_RECSTART_UI_ITEM_ID + 4)
#define	NX_PRISM_RECSTART_UI_TEXT2_ITEM_X				130
#define	NX_PRISM_RECSTART_UI_TEXT2_ITEM_Y				(NX_PRISM_RECSTART_UI_TEXT_ITEM_Y + 32)
#define	NX_PRISM_RECSTART_UI_TEXT2_ITEM_W			540
#define	NX_PRISM_RECSTART_UI_TEXT2_ITEM_H				(32)

#define	NX_PRISM_RECSTART_UI_ICON_ID					(NX_PRISM_RECSTART_UI_ITEM_ID + 5)
#define	NX_PRISM_RECSTART_UI_ICON_X					80
#define	NX_PRISM_RECSTART_UI_ICON_Y					60
#define	NX_PRISM_RECSTART_UI_ICON_W					38
#define	NX_PRISM_RECSTART_UI_ICON_H					38

#define	NX_PRISM_RECSTART_UI_OFF_ITEM_NUM			1
#define	NX_PRISM_RECSTART_UI_OFF_ITEM_INDEX			0
#define	NX_PRISM_RECSTART_UI_OFF_ITEM_TYPE			0xFF

#define	NX_PRISM_RECSTART_UI_IMG_MG					10
#define	NX_PRISM_RECSTART_UI_IMG_W					28
#define	NX_PRISM_RECSTART_UI_TEXT_MG					10


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct NX_RecStart_UI_Contents
{
	HUINT32						ulSvcUid;
	HUINT32						ulSessionId;
	HCHAR						szStrBuf[NX_TEXT_SIZE_128];
} Nx_RecStartlUIContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_RecStartlUIContent_t			s_stRecStartlUiContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_RecStartlUIContent_t* nx_recStartUi_GetContents(void);

static ONDK_Result_t 	nx_recStartUi_SetUiObject(Nx_RecStartlUIContent_t *pstContents);

static ONDK_Result_t	nx_recStartUi_MsgGwmCreate(Nx_RecStartlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_recStartUi_MsgGwmDestroy(Nx_RecStartlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_recStartUi_MsgGwmKeyDown(Nx_RecStartlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_recStartUi_MsgGwmTimer(Nx_RecStartlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static inline Nx_RecStartlUIContent_t* nx_recStartUi_GetContents(void)
{
	return	&s_stRecStartlUiContent;
}

static ONDK_Result_t 	nx_recStartUi_SetUiObject(Nx_RecStartlUIContent_t *pstContents)
{
	ONDK_Rect_t 	stRect;


	ONDK_GWM_APP_SetAppArea(NX_PRISM_RECSTART_UI_WINDOW_X, NX_PRISM_RECSTART_UI_WINDOW_Y,
							NX_PRISM_RECSTART_UI_WINDOW_W, NX_PRISM_RECSTART_UI_WINDOW_H);

	/* Draw BG Frame */
	stRect = ONDK_Rect(NX_PRISM_RECSTART_UI_FRAME_X, NX_PRISM_RECSTART_UI_FRAME_Y,
						NX_PRISM_RECSTART_UI_FRAME_W, NX_PRISM_RECSTART_UI_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_RECSTART_UI_FRAME_ID, &stRect, COL(C_G_Bang_90));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_RECSTART_UI_FRAME_ID, NX_PRISM_COMP_DrawFrameDialogueWindowBG_BottomLine);

	/* Draw Start record TEXT */
	stRect = ONDK_Rect(NX_PRISM_RECSTART_UI_TEXT_ITEM_X, NX_PRISM_RECSTART_UI_TEXT_ITEM_Y,
						NX_PRISM_RECSTART_UI_TEXT_ITEM_W, NX_PRISM_RECSTART_UI_TEXT_ITEM_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_RECSTART_UI_TEXT_ITEM_ID, &stRect, (HCHAR*)ONDK_GetString(STR_START_RECORDING_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_RECSTART_UI_TEXT_ITEM_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_RECSTART_UI_TEXT_ITEM_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_RECSTART_UI_TEXT_ITEM_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_RECSTART_UI_TEXT_ITEM_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Draw event TEXT */
	stRect = ONDK_Rect(NX_PRISM_RECSTART_UI_TEXT2_ITEM_X, NX_PRISM_RECSTART_UI_TEXT2_ITEM_Y,
						NX_PRISM_RECSTART_UI_TEXT2_ITEM_W, NX_PRISM_RECSTART_UI_TEXT2_ITEM_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_RECSTART_UI_TEXT2_ITEM_ID, &stRect, (HCHAR*)ONDK_GetString(STR_START_RECORDING_ID));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_RECSTART_UI_TEXT2_ITEM_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_RECSTART_UI_TEXT2_ITEM_ID, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_RECSTART_UI_TEXT2_ITEM_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_RECSTART_UI_TEXT2_ITEM_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Draw recording Icon */
	stRect = ONDK_Rect(NX_PRISM_RECSTART_UI_ICON_X, NX_PRISM_RECSTART_UI_ICON_Y,
						NX_PRISM_RECSTART_UI_ICON_W, NX_PRISM_RECSTART_UI_ICON_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_RECSTART_UI_ICON_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_RECSTART_UI_ICON_ID, RES_394_04_REC_PNG);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_recStartUi_MsgGwmCreate(Nx_RecStartlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxSTD_MemSet(pstContents, 0, sizeof(Nx_RecStartlUIContent_t));
	pstContents->ulSvcUid = (HUINT32)p1;
	pstContents->ulSessionId = (HUINT32)p2;

	nx_recStartUi_SetUiObject(pstContents);
	ONDK_GWM_SetTimer(NX_PRISM_RECSTART_UI_EXPIRE_TIMER_ID, NX_PRISM_RECSTART_UI_EXPIRE_TIME);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_recStartUi_MsgGwmDestroy(Nx_RecStartlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_PRISM_RECSTART_UI_EXPIRE_TIMER_ID);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_recStartUi_MsgGwmKeyDown(Nx_RecStartlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;


	NX_PRISM_UTIL_KeyConv_UiCommon(&lKeyId);
	switch (lKeyId)
	{
		case KEY_RED:
		case KEY_GREEN:
		case KEY_YELLOW:
		case KEY_BLUE:
		case KEY_ARROWUP :
		case KEY_ARROWDOWN:
		case KEY_TVRADIO:
		case KEY_EXT_AV:
		case KEY_COMBO:
		case KEY_TEXT:
		case KEY_TTX_HOLD:
		case KEY_TTX_SUBPAGE:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_WIDE:
		case KEY_3D:
		case KEY_INFO:
		case KEY_BACK:
		case KEY_EXT_AV_PLUS:
		case KEY_EXT_AV_MINUS:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
			ONDK_GWM_APP_Destroy(0);
			break;

		default:
			break;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t 	nx_recStartUi_MsgGwmTimer(Nx_RecStartlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulTimerID = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	if (ulTimerID == NX_PRISM_RECSTART_UI_EXPIRE_TIMER_ID)
	{
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_MESSAGE_BREAK;
	}
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_RECSTART_UI_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_RecStartlUIContent_t	*pstContents = NULL;

	pstContents = nx_recStartUi_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_recStartUi_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_recStartUi_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_recStartUi_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			eResult = nx_recStartUi_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_APP_SYSTEM_SVC_CHANGED:
			ONDK_GWM_APP_Destroy(0);
			break;
		default:
			break;
	}
	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


void		NX_PRISM_START_RECORD_CreateResponseDialouge(HINT32 nSvcUid, HINT32 nSessionId)
{
	if (ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_RECSTART_UI_Proc) != NULL)
	{
		ONDK_GWM_APP_Destroy(NX_PRISM_RECSTART_UI_Proc);
	}

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "NX_PRISM_RECSTART_UI_Proc", APP_DEFAULT_PRIORITY,
						NX_PRISM_RECSTART_UI_Proc, 0, nSvcUid, nSessionId, 0);
}



/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

