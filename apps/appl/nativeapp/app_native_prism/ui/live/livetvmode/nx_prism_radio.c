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
 * @file	  		nx_prism_dolby.c
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
#include <nx_core_metadata_pf.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/
#define	NX_PRISM_RADIO_ITEM_ID					eMenuFrame_Window_Radio

#define	NX_PRISM_RADIO_FRAME_X					0
#define	NX_PRISM_RADIO_FRAME_Y					0
#define	NX_PRISM_RADIO_FRAME_W					1280
#define	NX_PRISM_RADIO_FRAME_H					720

#define	NX_PRISM_RADIO_ICON_ID					(NX_PRISM_RADIO_ITEM_ID + 2)
#define	NX_PRISM_RADIO_ICON_X					0
#define	NX_PRISM_RADIO_ICON_Y					0
#define	NX_PRISM_RADIO_ICON_W					1280
#define	NX_PRISM_RADIO_ICON_H					720

#define	NX_PRISM_RADIO_NAME_ID					(NX_PRISM_RADIO_ITEM_ID + 3)
#define	NX_PRISM_RADIO_NAME_X					440
#define	NX_PRISM_RADIO_NAME_Y					429
#define	NX_PRISM_RADIO_NAME_W					400
#define	NX_PRISM_RADIO_NAME_H					40

#define	NX_PRISM_RADIO_EVENT_ID					(NX_PRISM_RADIO_ITEM_ID + 4)
#define	NX_PRISM_RADIO_EVENT_X					440
#define	NX_PRISM_RADIO_EVENT_Y					463
#define	NX_PRISM_RADIO_EVENT_W					400
#define	NX_PRISM_RADIO_EVENT_H					30

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct NxPrismRadio_Context
{
	HUINT32 		svcUid;
	HUINT8			aucSvcName[NX_TEXT_SIZE_256];
	HUINT8			aucEvent[NX_TEXT_SIZE_256];
} NxPrismRadio_Context_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxPrismRadio_Context_t		s_stPrismRadio_Contents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t 	nx_prism_radio_MsgGwmCreate(NxPrismRadio_Context_t	*pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_prism_radio_MsgGwmDestroy(NxPrismRadio_Context_t	*pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static NxPrismRadio_Context_t*	nx_prism_radio_GetContents(void)
{
	return &s_stPrismRadio_Contents;
}

static ONDK_Result_t	nx_prism_radio_MsgGwmCreate(NxPrismRadio_Context_t	*pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Rect_t 	stRect;

	// init local data
	HxSTD_MemSet(pstContents, 0, sizeof(NxPrismRadio_Context_t));

	// frame
	ONDK_GWM_APP_SetAppArea(NX_PRISM_RADIO_FRAME_X, NX_PRISM_RADIO_FRAME_Y,
								NX_PRISM_RADIO_FRAME_W, NX_PRISM_RADIO_FRAME_H);

	// bg
	stRect = ONDK_Rect(NX_PRISM_RADIO_ICON_X, NX_PRISM_RADIO_ICON_Y,
						NX_PRISM_RADIO_ICON_W, NX_PRISM_RADIO_ICON_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_RADIO_ICON_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_RADIO_ICON_ID, RES_530_00_RADIOSTILL_PNG);

	// service
	stRect = ONDK_Rect(NX_PRISM_RADIO_NAME_X, NX_PRISM_RADIO_NAME_Y,
						NX_PRISM_RADIO_NAME_W, NX_PRISM_RADIO_NAME_H);
    ONDK_GWM_COM_Text_Create(NX_PRISM_RADIO_NAME_ID, &stRect, pstContents->aucSvcName);
    ONDK_GWM_COM_Text_SetFont(NX_PRISM_RADIO_NAME_ID, eFont_SystemBold);
    ONDK_GWM_COM_Text_SetSize(NX_PRISM_RADIO_NAME_ID, NX_PRISM_FONT_SIZE_26);
    ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_RADIO_NAME_ID, COL(C_T_Whistle_100), COL(C_T_Whistle_100));
    ONDK_GWM_COM_Text_SetAlign(NX_PRISM_RADIO_NAME_ID, TEXT_ALIGN_CENTER);
    ONDK_GWM_Obj_SetVisible(NX_PRISM_RADIO_NAME_ID, TRUE);

	// event
	stRect = ONDK_Rect(NX_PRISM_RADIO_EVENT_X, NX_PRISM_RADIO_EVENT_Y,
						NX_PRISM_RADIO_EVENT_W, NX_PRISM_RADIO_EVENT_H);
    ONDK_GWM_COM_Text_Create(NX_PRISM_RADIO_EVENT_ID, &stRect, pstContents->aucEvent);
    ONDK_GWM_COM_Text_SetFont(NX_PRISM_RADIO_EVENT_ID, eFont_SystemNormal);
    ONDK_GWM_COM_Text_SetSize(NX_PRISM_RADIO_EVENT_ID, NX_PRISM_FONT_SIZE_20);
    ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_RADIO_EVENT_ID, COL(C_T_Gong04), COL(C_T_Gong04));
    ONDK_GWM_COM_Text_SetAlign(NX_PRISM_RADIO_EVENT_ID, TEXT_ALIGN_CENTER);
    ONDK_GWM_Obj_SetVisible(NX_PRISM_RADIO_EVENT_ID, TRUE);

	/* 절대 hide가 되지 않는다. */
	ONDK_GWM_APP_SetUIHideFlag(GWM_UI_Hide_Never);
	ONDK_GWM_APP_SetAttribute(GWM_HiddenUI_App);

	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_prism_radio_MsgGwmDestroy(NxPrismRadio_Context_t	*pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_prism_radio_MsgPFUpdate(NxPrismRadio_Context_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				hErr = ERR_FAIL;
	DxService_t 		*pService = NULL;
	NX_Channel_t		*pCurChannel = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	// get SVC name
	pCurChannel = NX_PRISM_LIVE_GetMasterChannel();
	if(pCurChannel != NULL)
	{
		pstContents->svcUid = pCurChannel->svcUid;
		pService = NX_CHANNEL_FindServiceBySvcUid(pstContents->svcUid);
		if(pService != NULL)
		{
			HxSTD_StrNCpy(pstContents->aucSvcName, pService->name, NX_TEXT_SIZE_256);
		}
	}
	ONDK_GWM_COM_Text_SetText(NX_PRISM_RADIO_NAME_ID, (HCHAR*)pstContents->aucSvcName);

	// get event
	HxSTD_StrNCpy(pstContents->aucEvent, "Playing...", NX_TEXT_SIZE_256);
	hErr = NX_PF_PROGRAMMES_GetName(pstContents->svcUid, pstContents->aucEvent);
	if(hErr == ERR_OK)
	{
		ONDK_GWM_COM_Text_SetText(NX_PRISM_RADIO_EVENT_ID, (HCHAR*)pstContents->aucEvent);
	}

	ONDK_GWM_APP_InvalidateObject(NX_PRISM_RADIO_NAME_ID);
	ONDK_GWM_APP_InvalidateObject(NX_PRISM_RADIO_EVENT_ID);

	return	ONDK_MESSAGE_PASS;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_RADIO_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult	= ONDK_RET_OK;
	NxPrismRadio_Context_t	*pstContents = nx_prism_radio_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_prism_radio_MsgGwmCreate(pstContents, p1, p2, p3);
			eResult = nx_prism_radio_MsgPFUpdate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			break;
		case MSG_GWM_TIMER:
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_prism_radio_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_APP_SYSTEM_SVC_CHANGED:
			eResult = nx_prism_radio_MsgPFUpdate(pstContents, p1, p2, p3);
			break;
		case MSG_CORE_SERVICE_PF_CHANGED_PROGRAMMES:
			eResult = nx_prism_radio_MsgPFUpdate(pstContents, p1, p2, p3);
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


HERROR		NX_PRISM_RADIO_CreateStill(void)
{
	ONDK_GWM_App_t	pGwmApp = NULL;


	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_RADIO_Proc);
	if (pGwmApp != NULL)
	{
		ONDK_GWM_APP_Destroy(NX_PRISM_RADIO_Proc);
	}

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_RADIO_Proc", APP_BGIMAGE_PRIORITY, NX_PRISM_RADIO_Proc, 0, 0, 0, 0);

	return	ERR_OK;
}

HERROR		NX_PRISM_RADIO_CloseStill(void)
{
	ONDK_GWM_App_t	pGwmApp = NULL;


	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_RADIO_Proc);
	if (pGwmApp != NULL)
	{
		ONDK_GWM_APP_Destroy(NX_PRISM_RADIO_Proc);
	}

	return	ERR_OK;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

