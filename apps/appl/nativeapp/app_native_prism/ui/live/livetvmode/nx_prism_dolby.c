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


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/
#define	NX_PRISM_DOLBY_ITEM_ID					eMenuFrame_Window_Dolby

#define	NX_PRISM_DOLBY_EXPIRE_TIMER_ID			(NX_PRISM_DOLBY_ITEM_ID + 1)
#define	NX_PRISM_DOLBY_EXPIRE_TIME				(5 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_DOLBY_FRAME_X					1030
#define	NX_PRISM_DOLBY_FRAME_Y					55
#define	NX_PRISM_DOLBY_FRAME_W					191
#define	NX_PRISM_DOLBY_FRAME_H					28

#define	NX_PRISM_DOLBY_ICON_ID					(NX_PRISM_DOLBY_ITEM_ID + 2)
#define	NX_PRISM_DOLBY_ICON_X					0
#define	NX_PRISM_DOLBY_ICON_Y					0
#define	NX_PRISM_DOLBY_ICON_W					NX_PRISM_DOLBY_FRAME_W
#define	NX_PRISM_DOLBY_ICON_H					NX_PRISM_DOLBY_FRAME_H


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t 	nx_dolby_SetUiObject(HINT32 p1);

static ONDK_Result_t 	nx_dolby_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_dolby_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_dolby_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_dolby_MsgGwmTimer(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_dolby_MsgGwmHapiMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t 	nx_dolby_SetUiObject(HINT32 p1)
{
	ONDK_Rect_t 	stRect;
	HCHAR			*szDolbyImage = NULL;
	DxAudioCodec_e	eAudCodec = (DxAudioCodec_e)p1;


	ONDK_GWM_APP_SetAppArea(NX_PRISM_DOLBY_FRAME_X, NX_PRISM_DOLBY_FRAME_Y,
								NX_PRISM_DOLBY_FRAME_W, NX_PRISM_DOLBY_FRAME_H);

	switch (eAudCodec)
	{
		case eDxAUDIO_CODEC_DOLBY_AC3:
		case eDxAUDIO_CODEC_DOLBY_AC3P:
			szDolbyImage = RES_208_00_DOLBY_PNG;
			break;
		default:
			szDolbyImage = NULL;
			NX_APP_Error("[%s:%d] ERROR!!! Please check Audio Format!! - eAudCodec :[%d] \n", __FUNCTION__, __HxLINE__, eAudCodec);
			break;
	}

	if (szDolbyImage != NULL)
	{
		stRect = ONDK_Rect(NX_PRISM_DOLBY_ICON_X, NX_PRISM_DOLBY_ICON_Y,
							NX_PRISM_DOLBY_ICON_W, NX_PRISM_DOLBY_ICON_H);
		ONDK_GWM_COM_Image_Create(NX_PRISM_DOLBY_ICON_ID, &stRect);
		ONDK_GWM_COM_Image_SetImage(NX_PRISM_DOLBY_ICON_ID, szDolbyImage);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_dolby_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3)
{
	/* 절대 hide가 되지 않는다. */
	nx_dolby_SetUiObject(p1);
	ONDK_GWM_APP_SetAttribute(GWM_HiddenUI_App);
	ONDK_GWM_SetTimer(NX_PRISM_DOLBY_EXPIRE_TIMER_ID, NX_PRISM_DOLBY_EXPIRE_TIME);

	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_dolby_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_PRISM_DOLBY_EXPIRE_TIMER_ID);
	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_dolby_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;

	switch (lKeyId)
	{
		case KEY_BACK:
		case KEY_EXIT:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		default:
			return	ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);
}


static ONDK_Result_t 	nx_dolby_MsgGwmTimer(HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulTimerID = p1;

	if (ulTimerID == NX_PRISM_DOLBY_EXPIRE_TIMER_ID)
	{
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_MESSAGE_BREAK;
	}

	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_dolby_MsgGwmHapiMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMediaPlay_MediaType_e		eType = p2;

	switch (eType)
	{
		case eOxMP_MEDIATYPE_LIVE:
			break;
		default:
			break;
	}

	NX_COMMON_NOT_USED_ARGUMENT(NULL, p1, p2, p3);
	NX_COMMON_NOT_USED_PARAM(handle);

	return	ONDK_MESSAGE_PASS;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_DOLBY_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult	= ONDK_RET_OK;

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_dolby_MsgGwmCreate(p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_dolby_MsgGwmKeyDown(p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			eResult = nx_dolby_MsgGwmTimer(p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_dolby_MsgGwmDestroy(p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_dolby_MsgGwmHapiMsg(handle, p1, p2, p3);
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


HERROR		NX_PRISM_DOLBY_ShowDolbyLogo(DxAudioCodec_e eAudCodec)
{
#if defined(CONFIG_MW_AV_DOLBY_DECODE)
	ONDK_GWM_App_t	pGwmApp = NULL;


	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_DOLBY_Proc);
	if (pGwmApp != NULL)
	{
		ONDK_GWM_APP_Destroy(NX_PRISM_DOLBY_Proc);
	}

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_DOLBY_Proc", APP_INFOHUB_PRIORITY, NX_PRISM_DOLBY_Proc, 0, (HINT32)eAudCodec, 0, 0);
#endif

	return	ERR_OK;
}


HERROR		NX_PRISM_DOLBY_CloseDolbyLogo(void)
{
#if defined(CONFIG_MW_AV_DOLBY_DECODE)
	ONDK_GWM_App_t	pGwmApp = NULL;


	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_DOLBY_Proc);
	if (pGwmApp != NULL)
	{
		ONDK_GWM_APP_Destroy(NX_PRISM_DOLBY_Proc);
	}
#endif

	return	ERR_OK;
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

