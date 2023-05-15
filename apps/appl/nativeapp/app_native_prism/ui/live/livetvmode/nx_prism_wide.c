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
 * @file	  		nx_prism_wide.c
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

#define	NX_PRISM_WIDE_TEXT_LENGTH				64
#define	NX_PRISM_WIDE_ITEM_ID					(eMenuFrame_Window_Wide)

#define	NX_PRISM_WIDE_EXPIRE_TIMER_ID			(NX_PRISM_WIDE_ITEM_ID + 1)
#define	NX_PRISM_WIDE_EXPIRE_TIME				(3 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_WIDE_WINDOW_X					960
#define	NX_PRISM_WIDE_WINDOW_Y					50
#define	NX_PRISM_WIDE_WINDOW_W					240
#define	NX_PRISM_WIDE_WINDOW_H					155

#define	NX_PRISM_WIDE_FRAME_ID					(NX_PRISM_WIDE_ITEM_ID + 2)
#define	NX_PRISM_WIDE_FRAME_X					0
#define	NX_PRISM_WIDE_FRAME_Y					0
#define	NX_PRISM_WIDE_FRAME_W					NX_PRISM_WIDE_WINDOW_W
#define	NX_PRISM_WIDE_FRAME_H					NX_PRISM_WIDE_WINDOW_H

#define	NX_PRISM_WIDE_ICON_ID					(NX_PRISM_WIDE_ITEM_ID + 3)
#define	NX_PRISM_WIDE_ICON_X					32
#define	NX_PRISM_WIDE_ICON_Y					15
#define	NX_PRISM_WIDE_ICON_W					176
#define	NX_PRISM_WIDE_ICON_H					100

#define	NX_PRISM_WIDE_TEXT_ID					(NX_PRISM_WIDE_ITEM_ID + 4)
#define	NX_PRISM_WIDE_TEXT_X					15
#define	NX_PRISM_WIDE_TEXT_Y					(140 - NX_PRISM_FONT_SIZE_20)
#define	NX_PRISM_WIDE_TEXT_W					210
#define	NX_PRISM_WIDE_TEXT_H					(NX_PRISM_FONT_SIZE_20 + NX_PRISM_FONT_VSPACE)


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct NX_WIDE_Contents
{
	HCHAR		szDisplayFormat[NX_PRISM_WIDE_TEXT_LENGTH];

	DxAspectRatio_e		eScreenRatio;
	DxDfcSelect_e		eDispFormat;
} Nx_WideContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_WideContent_t			s_stWideContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static inline Nx_WideContent_t* nx_wide_GetContents(void);

static HCHAR*			nx_wide_GetDisplayFormatImage(Nx_WideContent_t *pstContents);
static HCHAR*			nx_wide_GetDisplayFormatString(Nx_WideContent_t *pstContents);
static ONDK_Result_t	nx_wide_ActionChangeWideMode(Nx_WideContent_t *pstContents);

static ONDK_Result_t	nx_wide_LoadItemsValue(Nx_WideContent_t *pstContents);

static ONDK_Result_t 	nx_wide_UpdateUiObject(Nx_WideContent_t *pstContents);
static ONDK_Result_t 	nx_wide_SetUiObject(Nx_WideContent_t *pstContents);
static ONDK_Result_t	nx_wide_Initialize(Nx_WideContent_t *pstContents);

static ONDK_Result_t	nx_wide_MsgGwmCreate(Nx_WideContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_wide_MsgGwmDestroy(Nx_WideContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_wide_MsgGwmHapiMsg(Nx_WideContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_wide_MsgGwmKeyDown(Nx_WideContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_wide_MsgGwmTimer(Nx_WideContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_WideContent_t* nx_wide_GetContents(void)
{
	return	&s_stWideContent;
}


static HCHAR*	nx_wide_GetDisplayFormatImage(Nx_WideContent_t *pstContents)
{
	HCHAR		*szImageRes = NULL;


	switch (pstContents->eDispFormat)
	{
		case eDxDFC_SELECT_DEFAULT:
			if (pstContents->eScreenRatio == eDxASPECTRATIO_4X3)
			{
				szImageRes = RES_209_00_WIDE_169_AUTO_PNG;
			}
			else if (pstContents->eScreenRatio == eDxASPECTRATIO_16X9)
			{
				szImageRes = RES_209_00_WIDE_43_AUTO_PNG;
			}
			else
			{
				NX_APP_Error("[%s:%d] ERROR!!! Not Support Screen Ratio:[%d] \n", __FUNCTION__, __HxLINE__, pstContents->eScreenRatio);
			}
			break;
		case eDxDFC_SELECT_LETTERBOX:
			szImageRes = RES_209_00_WIDE_43_LETTER_PNG;
			break;
		case eDxDFC_SELECT_CENTERCUTOUT:
			szImageRes = RES_209_00_WIDE_43_CENTER_PNG;
			break;
		case eDxDFC_SELECT_PILLARBOX:
			szImageRes = RES_209_00_WIDE_169_PILLAR_PNG;
			break;
		case eDxDFC_SELECT_ZOOM:
			szImageRes = RES_209_00_WIDE_169_ZOOM_PNG;
			break;
		case eDxDFC_SELECT_FULLSCREEN:
			szImageRes = RES_209_00_WIDE_169_FULL_PNG;
			break;

		// TODO: Add other info

		default:
			szImageRes = NULL;
			NX_APP_Error("[%s:%d] ERROR!!! Not Support Display Format :[%d] \n", __FUNCTION__, __HxLINE__, pstContents->eDispFormat);
			break;
	}

	return	szImageRes;
}


static HCHAR*	nx_wide_GetDisplayFormatString(Nx_WideContent_t *pstContents)
{
	HCHAR		*szStringRes = RES_UNKNOWN_ID;

	switch (pstContents->eDispFormat)
	{
		case eDxDFC_SELECT_DEFAULT:
			if (pstContents->eScreenRatio == eDxASPECTRATIO_4X3)
			{
				szStringRes = RES_AUTO_ID;
			}
			else if (pstContents->eScreenRatio == eDxASPECTRATIO_16X9)
			{
				szStringRes = RES_AUTO_ID;
			}
			else
			{
				szStringRes = RES_UNKNOWN_ID;
				NX_APP_Error("[%s:%d] ERROR!!! Not Support Screen Ratio:[%d] \n", __FUNCTION__, __HxLINE__, pstContents->eScreenRatio);
			}
			break;
		case eDxDFC_SELECT_LETTERBOX:
			szStringRes = RES_LETTERBOX_ID;
			break;
		case eDxDFC_SELECT_CENTERCUTOUT:
			szStringRes = RES_CENTRE_ID;
			break;
		case eDxDFC_SELECT_PILLARBOX:
			szStringRes = RES_PILLARBOX_ID;
			break;
		case eDxDFC_SELECT_ZOOM:
			szStringRes = RES_ZOOM_ID;
			break;
		case eDxDFC_SELECT_FULLSCREEN:
			szStringRes = RES_FULL_ID;
			break;
		default:
			szStringRes = RES_UNKNOWN_ID;
			NX_APP_Error("[%s:%d] ERROR!!! Not Support Display Format :[%d] \n", __FUNCTION__, __HxLINE__, pstContents->eDispFormat);
			break;
	}

	return	szStringRes;
}


static ONDK_Result_t	nx_wide_ActionChangeWideMode(Nx_WideContent_t *pstContents)
{
	HINT32				lNextDispFormat = eDxDFC_SELECT_UNKNOWN;
	DxDfcSelect_e		eNextDispFormat = eDxDFC_SELECT_UNKNOWN;

	NX_PRISM_UTIL_FindNextVideoFormatByRatioDisplayFormat(pstContents->eScreenRatio, pstContents->eDispFormat, &lNextDispFormat);
	eNextDispFormat = (DxDfcSelect_e)lNextDispFormat;
	if (eNextDispFormat != eDxDFC_SELECT_UNKNOWN)
	{
		NX_SYSTEM_SetDisplayFormat(eNextDispFormat);
		pstContents->eDispFormat = eNextDispFormat;
		nx_wide_UpdateUiObject(pstContents);
	}
	else
	{
		NX_APP_Error("[%s:%d] ERROR!!! Next eNextDispFormat are eDxDFC_SELECT_UNKNOWN!!  \n", __FUNCTION__, __HxLINE__);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_wide_LoadItemsValue(Nx_WideContent_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	DxAspectRatio_e		eScreenRatio = eDxASPECTRATIO_4X3;
	DxDfcSelect_e		eDispFormat = eDxDFC_SELECT_DEFAULT;

	/* Get Screen Ratio */
	hError = NX_SYSTEM_GetTvAspectRatio(&eScreenRatio);
	if (hError != ERR_OK)
	{
		NX_APP_Error("[%s:%d] ERROR!!! NX_SYSTEM_GetTvAspectRatio() \n", __FUNCTION__, __HxLINE__);
	}
	pstContents->eScreenRatio = eScreenRatio;

	/* Get Display Format */
	hError = NX_SYSTEM_GetDisplayFormat(&eDispFormat);
	if (hError != ERR_OK)
	{
		NX_APP_Error("[%s:%d] ERROR!!! NX_SYSTEM_GetDisplayFormat() \n", __FUNCTION__, __HxLINE__);
	}
	pstContents->eDispFormat = eDispFormat;


	NX_APP_Info("ERROR!!! eScreenRatio:[%d], eDispFormat:[%d] \n", eScreenRatio, eDispFormat);

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_wide_UpdateUiObject(Nx_WideContent_t *pstContents)
{
	/* Draw Display Format */
	HCHAR		*szDisplyImage = NULL, *szDisplyStrId = NULL;

	szDisplyImage = nx_wide_GetDisplayFormatImage(pstContents);
	if (szDisplyImage != NULL)
	{
		ONDK_GWM_COM_Image_SetImage(NX_PRISM_WIDE_ICON_ID, szDisplyImage);
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_WIDE_ICON_ID);
	}

	szDisplyStrId = nx_wide_GetDisplayFormatString(pstContents);
	if (szDisplyStrId)
	{
		HxSTD_sprintf(pstContents->szDisplayFormat, "%s", (HCHAR*)ONDK_GetString(szDisplyStrId));
		ONDK_GWM_COM_Text_SetText(NX_PRISM_WIDE_TEXT_ID, pstContents->szDisplayFormat);
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t 	nx_wide_SetUiObject(Nx_WideContent_t *pstContents)
{
	HCHAR			*szDisplyImage = NULL;
	ONDK_Rect_t 	stRect;


	ONDK_GWM_APP_SetAppArea(NX_PRISM_WIDE_WINDOW_X, NX_PRISM_WIDE_WINDOW_Y,
							NX_PRISM_WIDE_WINDOW_W, NX_PRISM_WIDE_WINDOW_H);

	/* Draw BG Frame */
	stRect = ONDK_Rect(NX_PRISM_WIDE_FRAME_X, NX_PRISM_WIDE_FRAME_Y,
						NX_PRISM_WIDE_FRAME_W, NX_PRISM_WIDE_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_WIDE_FRAME_ID, &stRect, COL(C_G_Bangk_75));

	/* Draw Display Format */
	szDisplyImage = nx_wide_GetDisplayFormatImage(pstContents);
	stRect = ONDK_Rect(NX_PRISM_WIDE_ICON_X, NX_PRISM_WIDE_ICON_Y,
						NX_PRISM_WIDE_ICON_W, NX_PRISM_WIDE_ICON_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_WIDE_ICON_ID, &stRect);
	if (szDisplyImage != NULL)
	{
		ONDK_GWM_COM_Image_SetImage(NX_PRISM_WIDE_ICON_ID, szDisplyImage);
	}

	/* Draw Display Format TEXT */
	stRect = ONDK_Rect(NX_PRISM_WIDE_TEXT_X, NX_PRISM_WIDE_TEXT_Y,
						NX_PRISM_WIDE_TEXT_W, NX_PRISM_WIDE_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_WIDE_TEXT_ID, &stRect, (HCHAR*)pstContents->szDisplayFormat);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_WIDE_TEXT_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_WIDE_TEXT_ID, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_WIDE_TEXT_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_WIDE_TEXT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_wide_Initialize(Nx_WideContent_t *pstContents)
{
	HCHAR		*szDisplyStrId = NULL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_INITFAIL;
	}

	// Init
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_WideContent_t));
	pstContents->szDisplayFormat[0] = '\0';

	// Load Value
	nx_wide_LoadItemsValue(pstContents);

	// Make Display format Text
	szDisplyStrId = nx_wide_GetDisplayFormatString(pstContents);
	if (szDisplyStrId)
	{
		HxSTD_sprintf(pstContents->szDisplayFormat, "%s", (HCHAR*)ONDK_GetString(szDisplyStrId));
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_wide_MsgGwmCreate(Nx_WideContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	nx_wide_Initialize(pstContents);
	nx_wide_SetUiObject(pstContents);
	ONDK_GWM_SetTimer(NX_PRISM_WIDE_EXPIRE_TIMER_ID, NX_PRISM_WIDE_EXPIRE_TIME);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_wide_MsgGwmDestroy(Nx_WideContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_PRISM_WIDE_EXPIRE_TIMER_ID);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_wide_MsgGwmHapiMsg(Nx_WideContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_wide_MsgGwmKeyDown(Nx_WideContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;

	switch (lKeyId)
	{
		case KEY_BACK:
		case KEY_EXIT:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_BREAK;

		case KEY_WIDE:
			ONDK_GWM_ResetTimer(NX_PRISM_WIDE_EXPIRE_TIMER_ID);
			nx_wide_ActionChangeWideMode(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
		case KEY_MENU:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			return ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
}


static ONDK_Result_t 	nx_wide_MsgGwmTimer(Nx_WideContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulTimerID = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	if (ulTimerID == NX_PRISM_WIDE_EXPIRE_TIMER_ID)
	{
		ONDK_GWM_APP_Destroy(0); //mute 상태는 숨지 않는다.
		return	ONDK_MESSAGE_BREAK;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_WIDE_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_WideContent_t	*pstContents = NULL;

	pstContents = nx_wide_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_wide_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_wide_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_wide_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_wide_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			eResult = nx_wide_MsgGwmTimer(pstContents, p1, p2, p3);
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


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

