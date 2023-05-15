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
 * @file	  		nx_prism_subtitle_ui.c
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

#define	NX_PRISM_SUBTTL_UI_ITEM_ID					eMenuFrame_Window_SubttlUI

#define	NX_PRISM_SUBTTL_UI_EXPIRE_TIMER_ID			(NX_PRISM_SUBTTL_UI_ITEM_ID + 1)
#define	NX_PRISM_SUBTTL_UI_EXPIRE_TIME				(3 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_SUBTTL_UI_WINDOW_X					290
#define	NX_PRISM_SUBTTL_UI_WINDOW_Y					0
#define	NX_PRISM_SUBTTL_UI_WINDOW_W					700
#define	NX_PRISM_SUBTTL_UI_WINDOW_H					101

#define	NX_PRISM_SUBTTL_UI_FRAME_ID					(NX_PRISM_SUBTTL_UI_ITEM_ID + 2)
#define	NX_PRISM_SUBTTL_UI_FRAME_X					0
#define	NX_PRISM_SUBTTL_UI_FRAME_Y					0
#define	NX_PRISM_SUBTTL_UI_FRAME_W					NX_PRISM_SUBTTL_UI_WINDOW_W
#define	NX_PRISM_SUBTTL_UI_FRAME_H					NX_PRISM_SUBTTL_UI_WINDOW_H

#define	NX_PRISM_SUBTTL_UI_TEXT_ITEM_ID				(NX_PRISM_SUBTTL_UI_ITEM_ID + 3)
#define	NX_PRISM_SUBTTL_UI_TEXT_ITEM_X				0
#define	NX_PRISM_SUBTTL_UI_TEXT_ITEM_Y				49
#define	NX_PRISM_SUBTTL_UI_TEXT_ITEM_W				NX_PRISM_SUBTTL_UI_FRAME_W
#define	NX_PRISM_SUBTTL_UI_TEXT_ITEM_H				(NX_PRISM_FONT_SIZE_22 + NX_PRISM_FONT_VSPACE)

#define	NX_PRISM_SUBTTL_UI_ICON_ID					(NX_PRISM_SUBTTL_UI_ITEM_ID + 4)
#define	NX_PRISM_SUBTTL_UI_ICON_X					0
#define	NX_PRISM_SUBTTL_UI_ICON_Y					49
#define	NX_PRISM_SUBTTL_UI_ICON_W					28
#define	NX_PRISM_SUBTTL_UI_ICON_H					28

#define	NX_PRISM_SUBTTL_UI_OFF_ITEM_NUM				1
#define	NX_PRISM_SUBTTL_UI_OFF_ITEM_INDEX			0
#define	NX_PRISM_SUBTTL_UI_OFF_ITEM_TYPE			0xFF

#define	NX_PRISM_SUBTTL_UI_IMG_MG					10
#define	NX_PRISM_SUBTTL_UI_IMG_W					28
#define	NX_PRISM_SUBTTL_UI_TEXT_MG					10


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct NX_SUBTITEL_UI_Contents
{
	HUINT32 					ulUserSelectIdx;
	HUINT32						ulViewId;
	HBOOL						bAddOffItem;
	OxMediaPlay_MediaType_e		eMediaType;
	HCHAR						szStrBuf[NX_TEXT_SIZE_64];
	NX_Media_SBTList_t			stSubtList;
} Nx_SubttlUIContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_SubttlUIContent_t			s_stSubttlUiContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_subttlUi_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
static void				nx_subttlUi_CreateResponseDialouge(void);

static inline Nx_SubttlUIContent_t* nx_subttlUi_GetContents(void);

static ONDK_Result_t	nx_subttlUi_ActionChangeSubtitle(Nx_SubttlUIContent_t *pstContents);

static ONDK_Result_t	nx_subttlUi_MakeSubtitleList(Nx_SubttlUIContent_t *pstContents, NX_Media_SBTList_t *stComponetSubtitle);
static ONDK_Result_t	nx_subttlUi_LoadItemsValue(Nx_SubttlUIContent_t *pstContents);

static ONDK_Result_t 	nx_subttlUi_UpdateUiObject(Nx_SubttlUIContent_t *pstContents);
static ONDK_Result_t 	nx_subttlUi_SetUiObject(Nx_SubttlUIContent_t *pstContents);
static ONDK_Result_t	nx_subttlUi_Initialize(Nx_SubttlUIContent_t *pstContents);

static ONDK_Result_t	nx_subttlUi_MsgGwmCreate(Nx_SubttlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_subttlUi_MsgGwmDestroy(Nx_SubttlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_subttlUi_MsgGwmHapiMsg(Nx_SubttlUIContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_subttlUi_MsgGwmKeyDown(Nx_SubttlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_subttlUi_MsgGwmTimer(Nx_SubttlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_subttlUi_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
	switch (lMessage)
	{
		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon (&lParam1);
			switch (lParam1)
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
					return	ONDK_MESSAGE_BREAK;
				default:
					break;
			}
			break;

		case MSG_APP_SYSTEM_SVC_CHANGED :
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		default :
			break;
	}

	return	NX_PRISM_DIALOGUE_ResponseMessage_Proc(lMessage, hHandle, lParam1, lParam2, lParam3);
}


static void		nx_subttlUi_CreateResponseDialouge(void)
{
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_subttlUi_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_subttlUi_ResponseDialogProc, 0, (HINT32)ONDK_GetString(RES_NO_SUBTITLE_DATA_ID), 0, 0);
}


static inline Nx_SubttlUIContent_t* nx_subttlUi_GetContents(void)
{
	return	&s_stSubttlUiContent;
}


static ONDK_Result_t	nx_subttlUi_ActionChangeSubtitle(Nx_SubttlUIContent_t *pstContents)
{
	HINT32		lIndex = -1;
	HUINT32		ulSelectIdx = 0;

	ulSelectIdx = (++pstContents->ulUserSelectIdx) % pstContents->stSubtList.ulSubtitleNum;
	pstContents->ulUserSelectIdx = ulSelectIdx;

	NX_PRISM_UTIL_MakeSubtitleString(pstContents->stSubtList, pstContents->ulUserSelectIdx, pstContents->szStrBuf);
	nx_subttlUi_UpdateUiObject(pstContents);

	if (pstContents->bAddOffItem == TRUE)
	{
		lIndex = (pstContents->ulUserSelectIdx == NX_PRISM_SUBTTL_UI_OFF_ITEM_INDEX) ? -1 : (pstContents->ulUserSelectIdx - NX_PRISM_SUBTTL_UI_OFF_ITEM_NUM) ;
	}
	else
	{
		lIndex = pstContents->ulUserSelectIdx;
	}

	NX_MEDIA_PLAY_SetComponentIndex(pstContents->ulViewId, eOxMP_COMPONENT_SUBTITLE, lIndex);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_subttlUi_MakeSubtitleList(Nx_SubttlUIContent_t *pstContents, NX_Media_SBTList_t *stComponetSubtitle)
{
	HUINT32		i = 0, ulCnt = 0, lOffItemCnt = 0;
	OxMediaPlay_SubtitleComp_t	*pListInfo = NULL;


	/* Add Off Item */
	if (pstContents->bAddOffItem == TRUE)
	{
		pListInfo = &pstContents->stSubtList.pstSubtitlelInfo[NX_PRISM_SUBTTL_UI_OFF_ITEM_INDEX];
		pListInfo->eCompType = eOxMP_COMPONENT_SUBTITLE;
		pListInfo->nPid = NX_CORE_PID_NULL;
		pListInfo->eSubtitleType = eOxMP_SUBTITLETYPE_Unknown;
		lOffItemCnt = NX_PRISM_SUBTTL_UI_OFF_ITEM_NUM;
	}
	else
	{
		lOffItemCnt = 0;
	}

	ulCnt = stComponetSubtitle->ulSubtitleNum;
	if (stComponetSubtitle->ulSubtitleNum >= NX_CORE_SUBTTL_MAX_NUM)
	{
		ulCnt = (NX_CORE_SUBTTL_MAX_NUM - 1);
	}

	/* Add Subtitle Item */
	for (i = 0; i < ulCnt; i++)
	{
		pListInfo = &pstContents->stSubtList.pstSubtitlelInfo[lOffItemCnt + i];
		HxSTD_memcpy(pListInfo, &stComponetSubtitle->pstSubtitlelInfo[i], sizeof(OxMediaPlay_SubtitleComp_t));
	}

	/* Add Subtitle Count + Add Off Item */
	pstContents->stSubtList.ulSubtitleNum = ulCnt + lOffItemCnt;

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_subttlUi_LoadItemsValue(Nx_SubttlUIContent_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	ONDK_Result_t		eResult = ONDK_RET_FAIL;
	NX_Media_SBTList_t	stComponetSubtitle;


	stComponetSubtitle.pstSubtitlelInfo = NULL;
	stComponetSubtitle.pstSubtitlelInfo = (OxMediaPlay_SubtitleComp_t*)NX_APP_Calloc(sizeof(OxMediaPlay_SubtitleComp_t) * NX_CORE_SUBTTL_MAX_NUM);
	if (stComponetSubtitle.pstSubtitlelInfo == NULL)
	{
		NX_APP_Error(" Error!!! NX_APP_Calloc() Fail !!\n");
		return	ONDK_RET_NOLOCALMEMORY;
	}

	hError = NX_MEDIA_PLAY_GetComponentListForSubtitle(pstContents->ulViewId, eOxMP_COMPONENT_SUBTITLE, &stComponetSubtitle);
	if (hError != ERR_OK)
	{
		NX_APP_Error("ERROR!!! NX_MEDIA_PLAY_GetComponentListForSubtitle() Fail!!\n");
		if (stComponetSubtitle.pstSubtitlelInfo)
		{
			NX_APP_Free(stComponetSubtitle.pstSubtitlelInfo);
			stComponetSubtitle.pstSubtitlelInfo = NULL;
		}
		return	ONDK_RET_INITFAIL;
	}
	else
	{
		if (stComponetSubtitle.ulSubtitleNum < 1)
		{
			NX_APP_Info("NX_MEDIA_PLAY_GetComponentListForSubtitle() - No Subtile Item !!\n");
			if (stComponetSubtitle.pstSubtitlelInfo)
			{
				NX_APP_Free(stComponetSubtitle.pstSubtitlelInfo);
				stComponetSubtitle.pstSubtitlelInfo = NULL;
			}
			return	ONDK_RET_INVARG;
		}
		else
		{
			eResult = nx_subttlUi_MakeSubtitleList(pstContents, &stComponetSubtitle);
			if (stComponetSubtitle.pstSubtitlelInfo)
			{
				NX_APP_Free(stComponetSubtitle.pstSubtitlelInfo);
				stComponetSubtitle.pstSubtitlelInfo = NULL;
			}
			return	eResult;
		}
	}

}


static ONDK_Result_t 	nx_subttlUi_UpdateUiObject(Nx_SubttlUIContent_t *pstContents)
{
	HCHAR		*szDisplyImage = NULL;
	HINT32		lStrWidth = 0, x = 0, lTempW = 0, lImageW = 0;
	ONDK_Rect_t stRect;


	ONDK_GWM_COM_Text_SetText(NX_PRISM_SUBTTL_UI_TEXT_ITEM_ID, pstContents->szStrBuf);

	szDisplyImage = NX_PRISM_UTIL_GetSubtitleIconResource(pstContents->stSubtList.pstSubtitlelInfo[pstContents->ulUserSelectIdx].eSubtitleType);

	lStrWidth = ONDK_GWM_COM_Text_GetStringWidth(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, pstContents->szStrBuf);
	lTempW = (NX_PRISM_SUBTTL_UI_FRAME_W - lStrWidth) / 2;
	lImageW = (NX_PRISM_SUBTTL_UI_IMG_W + (NX_PRISM_SUBTTL_UI_IMG_MG *2));
	if (szDisplyImage != NULL)
	{
		x = NX_PRISM_SUBTTL_UI_ICON_X + lTempW;
		if (x > lImageW)
		{
			x -= lImageW;
		}
	}
	else
	{
		x = NX_PRISM_SUBTTL_UI_ICON_X;
	}

	stRect = ONDK_Rect(x, NX_PRISM_SUBTTL_UI_ICON_Y, NX_PRISM_SUBTTL_UI_ICON_W, NX_PRISM_SUBTTL_UI_ICON_H);
	if (szDisplyImage != NULL)
	{
		ONDK_GWM_COM_Image_ChangeRect(NX_PRISM_SUBTTL_UI_ICON_ID, &stRect);
		ONDK_GWM_COM_Image_SetImage(NX_PRISM_SUBTTL_UI_ICON_ID, szDisplyImage);
		ONDK_GWM_Obj_SetHide(NX_PRISM_SUBTTL_UI_ICON_ID, FALSE);
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_SUBTTL_UI_ICON_ID);
	}
	else
	{
		ONDK_GWM_Obj_SetHide(NX_PRISM_SUBTTL_UI_ICON_ID, TRUE);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_subttlUi_SetUiObject(Nx_SubttlUIContent_t *pstContents)
{
	HCHAR			*szDisplyImage = NULL;
	HINT32			lStrWidth = 0, x = 0, lTempW = 0, lImageW = 0;
	ONDK_Rect_t 	stRect;


	ONDK_GWM_APP_SetAppArea(NX_PRISM_SUBTTL_UI_WINDOW_X, NX_PRISM_SUBTTL_UI_WINDOW_Y,
							NX_PRISM_SUBTTL_UI_WINDOW_W, NX_PRISM_SUBTTL_UI_WINDOW_H);

	/* Draw BG Frame */
	stRect = ONDK_Rect(NX_PRISM_SUBTTL_UI_FRAME_X, NX_PRISM_SUBTTL_UI_FRAME_Y,
						NX_PRISM_SUBTTL_UI_FRAME_W, NX_PRISM_SUBTTL_UI_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_SUBTTL_UI_FRAME_ID, &stRect, COL(C_G_Bang_90));
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SUBTTL_UI_FRAME_ID, NX_PRISM_COMP_DrawFrameDialogueWindowBG_BottomLine);

	NX_PRISM_UTIL_MakeSubtitleString(pstContents->stSubtList, pstContents->ulUserSelectIdx, pstContents->szStrBuf);

	/* Draw Subtitle TEXT */
	stRect = ONDK_Rect(NX_PRISM_SUBTTL_UI_TEXT_ITEM_X, NX_PRISM_SUBTTL_UI_TEXT_ITEM_Y,
						NX_PRISM_SUBTTL_UI_TEXT_ITEM_W, NX_PRISM_SUBTTL_UI_TEXT_ITEM_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_SUBTTL_UI_TEXT_ITEM_ID, &stRect, (HCHAR*)pstContents->szStrBuf);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_SUBTTL_UI_TEXT_ITEM_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_SUBTTL_UI_TEXT_ITEM_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_SUBTTL_UI_TEXT_ITEM_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_SUBTTL_UI_TEXT_ITEM_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Draw Subtitle Icon */
	szDisplyImage = NX_PRISM_UTIL_GetSubtitleIconResource(pstContents->stSubtList.pstSubtitlelInfo[pstContents->ulUserSelectIdx].eSubtitleType);

	lStrWidth = ONDK_GWM_COM_Text_GetStringWidth(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, pstContents->szStrBuf);
	lTempW = (NX_PRISM_SUBTTL_UI_FRAME_W - lStrWidth) / 2;
	lImageW = (NX_PRISM_SUBTTL_UI_IMG_W + (NX_PRISM_SUBTTL_UI_IMG_MG *2));
	if (szDisplyImage)
	{
		x = NX_PRISM_SUBTTL_UI_ICON_X + lTempW;
		if (x > lImageW)
		{
			x -= lImageW;
		}
	}
	else
	{
		x = NX_PRISM_SUBTTL_UI_ICON_X;
	}
	stRect = ONDK_Rect(x, NX_PRISM_SUBTTL_UI_ICON_Y,
						NX_PRISM_SUBTTL_UI_ICON_W, NX_PRISM_SUBTTL_UI_ICON_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_SUBTTL_UI_ICON_ID, &stRect);
	if (szDisplyImage)
	{
		ONDK_GWM_COM_Image_SetImage(NX_PRISM_SUBTTL_UI_ICON_ID, szDisplyImage);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_subttlUi_Initialize(Nx_SubttlUIContent_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HINT32			lSubttlTrackIndex = -1, lOffItemCnt = 0;
	HUINT32			ulViewId = 0;
	OxMediaPlay_MediaType_e		eMediaType = eOxMP_MEDIATYPE_NONE;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_INITFAIL;
	}

	// Init
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_SubttlUIContent_t));
	pstContents->szStrBuf[0] = '\0';

	NX_MEDIA_PLAY_GetMainViewId(&ulViewId);
	pstContents->ulViewId = ulViewId;
	NX_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType);
	pstContents->eMediaType = eMediaType;

	// Set Add Off Item
	pstContents->bAddOffItem = TRUE;
	lOffItemCnt = (pstContents->bAddOffItem == TRUE) ? NX_PRISM_SUBTTL_UI_OFF_ITEM_NUM : 0 ;


	pstContents->stSubtList.pstSubtitlelInfo = NULL;
	pstContents->stSubtList.pstSubtitlelInfo = (OxMediaPlay_SubtitleComp_t*)NX_APP_Calloc(sizeof(OxMediaPlay_SubtitleComp_t) * NX_CORE_SUBTTL_MAX_NUM);
	if (pstContents->stSubtList.pstSubtitlelInfo == NULL)
	{
		NX_APP_Error(" Error!!! NX_APP_Calloc() Fail !!\n");
		return	ONDK_RET_NOLOCALMEMORY;
	}

	// Load Value
	hError = nx_subttlUi_LoadItemsValue(pstContents);
	if (hError != ERR_OK)
	{
		nx_subttlUi_CreateResponseDialouge();
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_INITFAIL;
	}

	hError = NX_MEDIA_PLAY_GetComponentIndex(ulViewId, eOxMP_COMPONENT_SUBTITLE, &lSubttlTrackIndex, NULL);
	if (hError != ERR_OK)
	{
		pstContents->ulUserSelectIdx = NX_PRISM_SUBTTL_UI_OFF_ITEM_INDEX;
	}
	else
	{
		if ((lSubttlTrackIndex < 0) || (lSubttlTrackIndex == -1))
		{
			pstContents->ulUserSelectIdx = NX_PRISM_SUBTTL_UI_OFF_ITEM_INDEX;
		}
		else
		{
			pstContents->ulUserSelectIdx = lSubttlTrackIndex + lOffItemCnt;
		}
	}

	HxLOG_Info("NX_MEDIA_PLAY_GetComponentIndex() lSubttlTrackIndex :[0x%x][%d], ulUserSelectIdx:[%d]\n", lSubttlTrackIndex, lSubttlTrackIndex, pstContents->ulUserSelectIdx);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_subttlUi_MsgGwmCreate(Nx_SubttlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		hResult  = ONDK_RET_FAIL;

	hResult = nx_subttlUi_Initialize(pstContents);
	if (hResult != ONDK_RET_OK)
	{
		return	ONDK_RET_FAIL;
	}

	nx_subttlUi_SetUiObject(pstContents);
	ONDK_GWM_SetTimer(NX_PRISM_SUBTTL_UI_EXPIRE_TIMER_ID, NX_PRISM_SUBTTL_UI_EXPIRE_TIME);

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_subttlUi_MsgGwmDestroy(Nx_SubttlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_PRISM_SUBTTL_UI_EXPIRE_TIMER_ID);

	if (pstContents->stSubtList.pstSubtitlelInfo)
	{
		NX_APP_Free(pstContents->stSubtList.pstSubtitlelInfo);
		pstContents->stSubtList.pstSubtitlelInfo = NULL;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_subttlUi_MsgGwmHapiMsg(Nx_SubttlUIContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_subttlUi_MsgGwmKeyDown(Nx_SubttlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;


	NX_PRISM_UTIL_KeyConv_UiCommon(&lKeyId);
	switch (lKeyId)
	{
		case KEY_BACK:
		case KEY_EXIT:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_BREAK;

		case KEY_SUBTITLE:
			ONDK_GWM_ResetTimer(NX_PRISM_SUBTTL_UI_EXPIRE_TIMER_ID);
			nx_subttlUi_ActionChangeSubtitle(pstContents);
			return ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			return ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
}


static ONDK_Result_t 	nx_subttlUi_MsgGwmTimer(Nx_SubttlUIContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32		ulTimerID = p1;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	ONDK_RET_FAIL;
	}

	if (ulTimerID == NX_PRISM_SUBTTL_UI_EXPIRE_TIMER_ID)
	{
		NX_APP_Info("nx_subttlUi_MsgGwmTimer() - Destory window by timer of NX_PRISM_SUBTTL_UI_EXPIRE_TIMER_ID !!\n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_MESSAGE_BREAK;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_SUBTTL_UI_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_SubttlUIContent_t	*pstContents = NULL;

	pstContents = nx_subttlUi_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_subttlUi_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_subttlUi_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_subttlUi_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_subttlUi_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_TIMER:
			eResult = nx_subttlUi_MsgGwmTimer(pstContents, p1, p2, p3);
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


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

