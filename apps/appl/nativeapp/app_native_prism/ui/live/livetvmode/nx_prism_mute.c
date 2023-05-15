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
 * @file	  		nx_prism_mute.c
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
#include <nx_core.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/

#define	NX_PRISM_MUTE_ITEM_ID					eMenuFrame_Window_Mute

#define	NX_PRISM_MUTE_UPDATE_TIMER_ID			(NX_PRISM_MUTE_ITEM_ID + 1)
#define	NX_PRISM_MUTE_UPDATE_TIME				(1 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_MUTE_FRAME_X					1087
#define	NX_PRISM_MUTE_FRAME_Y					4
#define	NX_PRISM_MUTE_FRAME_W					170
#define	NX_PRISM_MUTE_FRAME_H					170

#define	NX_PRISM_MUTE_BG_ID						(NX_PRISM_MUTE_ITEM_ID + 2)
#define	NX_PRISM_MUTE_BG_X						0
#define	NX_PRISM_MUTE_BG_Y						0
#define	NX_PRISM_MUTE_BG_W						NX_PRISM_MUTE_FRAME_W
#define	NX_PRISM_MUTE_BG_H						NX_PRISM_MUTE_FRAME_H

#define	NX_PRISM_MUTE_ICON_ID					(NX_PRISM_MUTE_ITEM_ID + 3)
#define	NX_PRISM_MUTE_ICON_X					50
#define	NX_PRISM_MUTE_ICON_Y					49
#define	NX_PRISM_MUTE_ICON_W					70
#define	NX_PRISM_MUTE_ICON_H					70


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static HBOOL 			nx_mute_ShowBGUiObject(void);

static ONDK_Result_t 	nx_mute_SetUiObject(void);

static ONDK_Result_t 	nx_mute_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_mute_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_mute_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_mute_MsgGwmTimer(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_mute_MsgGwmHapiMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static HBOOL 	nx_mute_ShowBGUiObject(void)
{
	HBOOL			bShowBG = FALSE;
	HUINT32			ulUiCount = 0;
	ONDK_GWM_App_t	pGwmApp = NULL;

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_GUIDE_Proc);
	if(pGwmApp != NULL)
	{
		bShowBG = TRUE;
	}

	ulUiCount = ONDK_GWM_APP_CountAppByUiState(UISTATE_MENU);
	if (ulUiCount > 0)
	{
		bShowBG = TRUE;
	}

	// TODO: Add to here other Full OSD UI

	return	bShowBG;
}


static ONDK_Result_t 	nx_mute_SetUiObject(void)
{
	HBOOL			bShowBG = FALSE;
	ONDK_Rect_t 	stRect;


	ONDK_GWM_APP_SetAppArea(NX_PRISM_MUTE_FRAME_X, NX_PRISM_MUTE_FRAME_Y,
								NX_PRISM_MUTE_FRAME_W, NX_PRISM_MUTE_FRAME_H);

	stRect = ONDK_Rect(NX_PRISM_MUTE_BG_X, NX_PRISM_MUTE_BG_X,
						NX_PRISM_MUTE_BG_W, NX_PRISM_MUTE_BG_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_MUTE_BG_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_MUTE_BG_ID, RES_105_00_BG_UP_PNG);

	stRect = ONDK_Rect(NX_PRISM_MUTE_ICON_X, NX_PRISM_MUTE_ICON_Y,
						NX_PRISM_MUTE_ICON_W, NX_PRISM_MUTE_ICON_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_MUTE_ICON_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_MUTE_ICON_ID, RES_581_00_VOL_MUTE_PNG);

	bShowBG = nx_mute_ShowBGUiObject();
	ONDK_GWM_Obj_SetVisible(NX_PRISM_MUTE_BG_ID, bShowBG);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_mute_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_App_t	pGwmApp = NULL;

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_SYSTEMVolume_Proc);
	if(pGwmApp != NULL)
	{
		ONDK_GWM_APP_Destroy(NX_PRISM_SYSTEMVolume_Proc);
	}

	// Set Mute
	NX_SYSTEM_SetMute(TRUE);

	/* 절대 hide가 되지 않는다. */
	ONDK_GWM_APP_SetUIHideFlag(GWM_UI_Hide_Never);
	nx_mute_SetUiObject();
	ONDK_GWM_APP_SetAttribute(GWM_HiddenUI_App);

	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_mute_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_PRISM_MUTE_UPDATE_TIMER_ID);
	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_mute_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;

	switch (lKeyId)
	{
		case KEY_VOLUMEUP:
		case KEY_VOLUMEDOWN:
			ONDK_GWM_APP_Destroy(0);
			NX_SYSTEM_SetMute(FALSE);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_SYSTEMVolume_Proc", APP_INDEPENDENT_OSD_GRP_PRIORITY, NX_PRISM_SYSTEMVolume_Proc, 0, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case KEY_MUTE:
			ONDK_GWM_APP_Destroy(0);
			NX_SYSTEM_SetMute(FALSE);
			return ONDK_MESSAGE_BREAK;

		case KEY_MENU:
		case KEY_GUIDE:
			ONDK_GWM_SetTimer(NX_PRISM_MUTE_UPDATE_TIMER_ID, NX_PRISM_MUTE_UPDATE_TIME);
			return ONDK_MESSAGE_PASS;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			ONDK_MESSAGE_PASS;

		default:
			return ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);
}


static ONDK_Result_t 	nx_mute_MsgGwmTimer(HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL		bShowBG = FALSE;
	HUINT32		ulTimerID = p1;

	if (ulTimerID == NX_PRISM_MUTE_UPDATE_TIMER_ID)
	{
		bShowBG = nx_mute_ShowBGUiObject();
		ONDK_GWM_Obj_SetVisible(NX_PRISM_MUTE_BG_ID, bShowBG);

		return	ONDK_MESSAGE_BREAK;
	}

	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_mute_MsgGwmHapiMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);
	NX_COMMON_NOT_USED_PARAM(handle);

	return	ONDK_MESSAGE_PASS;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_MUTE_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult	= ONDK_RET_OK;

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_mute_MsgGwmCreate(p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_mute_MsgGwmKeyDown(p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			eResult = nx_mute_MsgGwmTimer(p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_mute_MsgGwmDestroy(p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_mute_MsgGwmHapiMsg(handle, p1, p2, p3);
			break;

		default:
			break;
	}

	if(eResult != ONDK_RET_OK)
	{
		return	eResult;
	}

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

