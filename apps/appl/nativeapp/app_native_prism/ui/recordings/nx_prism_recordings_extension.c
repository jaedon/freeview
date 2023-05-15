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
 * @file	  		nx_prism_recordings.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#if defined(CONFIG_MW_MM_PVR)
/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_prism_menu_contents.h>

#include "nx_prism_recordings.h"

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_RECORDINGS_EXT_WINDOW_X				0
#define	NX_RECORDINGS_EXT_WINDOW_Y				0
#define	NX_RECORDINGS_EXT_WINDOW_W				NX_PRISM_OSD_HORIZONTAL
#define	NX_RECORDINGS_EXT_WINDOW_H				NX_PRISM_OSD_VERTICAL

#define NX_RECORDINGS_EXT_FILL_BG_ID			0xba07

#define NX_RECORDINGS_EXT_TITLE_ID				0xba08
#define NX_RECORDINGS_EXT_TITLE_TEXT_X			235
#define NX_RECORDINGS_EXT_TITLE_TEXT_Y			57
#define NX_RECORDINGS_EXT_TITLE_TEXT_W			824
#define NX_RECORDINGS_EXT_TITLE_TEXT_H			50

#define NX_RECORDINGS_EXT_TOPLINE_ID			0xba09
#define NX_RECORDINGS_EXT_TOPLINE_X				235
#define NX_RECORDINGS_EXT_TOPLINE_Y				97
#define NX_RECORDINGS_EXT_TOPLINE_W				824
#define NX_RECORDINGS_EXT_TOPLINE_H				1

#define NX_RECORDINGS_EXT_CONTENT_FRAME_ID		0xba0a
#define NX_RECORDINGS_EXT_CONTENT_FRAME_X		250
#define NX_RECORDINGS_EXT_CONTENT_FRAME_Y		103
#define NX_RECORDINGS_EXT_CONTENT_FRAME_W		810
#define NX_RECORDINGS_EXT_CONTENT_FRAME_H		617

#define NX_RECORDINGS_EXT_CONTENT_GRIDLIST_ID	0xba0b
#define NX_RECORDINGS_EXT_CONTENT_GRIDLIST_X	NX_RECORDINGS_EXT_CONTENT_FRAME_X + 5
#define NX_RECORDINGS_EXT_CONTENT_GRIDLIST_Y	NX_RECORDINGS_EXT_CONTENT_FRAME_Y
#define NX_RECORDINGS_EXT_CONTENT_GRIDLIST_W	NX_RECORDINGS_EXT_CONTENT_FRAME_W - 10
#define NX_RECORDINGS_EXT_CONTENT_GRIDLIST_H	NX_RECORDINGS_EXT_CONTENT_FRAME_H - 11
#define NX_RECORDINGS_EXT_CONTENT_GRIDLIST_COL	4
#define NX_RECORDINGS_EXT_CONTENT_GRIDLIST_ROW	3
#define NX_RECORDINGS_EXT_CONTENT_ITEM_W		(170 + 10 + 10) // 190
#define NX_RECORDINGS_EXT_CONTENT_ITEM_H		(164 + 11 + 27)	// 202

#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
#define	NX_RECORDINGS_EXT_LIST_ANI_ID					0xbab1
#define NX_RECORDINGS_EXT_LIST_ANI_TIMEOUT				10
#endif


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct tagRecordingsContext {
	// recording data
	HCHAR		szTitle[256];
	HxVector_t	*recList;
	HINT32		nRecListNum;
	Nx_RecordGroupType_e	eGroupType;
} Nx_RecordingsExtContext_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static	Nx_RecordingsExtContext_t	s_stRecordingsExtContent;
/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_RecordingsExtContext_t	*nx_recordingsExt_GetContents(void);
static ONDK_Result_t	nx_recordingsExt_MsgGwmCreate(Nx_RecordingsExtContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_recordingsExt_MsgGwmDestroy(Nx_RecordingsExtContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_recordingsExt_MsgGwmKeyDown(Nx_RecordingsExtContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_recordingsExt_SetUiObject(Nx_RecordingsExtContext_t *pstContents);
#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
static ONDK_Result_t	nx_recordingsExt_SetAnimationCtl(void);
#endif
static ONDK_Result_t	nx_recordingsExt_DrawContentsItemFunc(HINT32 id, ONDKSurfaceHandle screen, GRID_DrawItem_t	*drawItem, HINT32 nColIndex);




/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static inline Nx_RecordingsExtContext_t	*nx_recordingsExt_GetContents(void)
{
	return	&s_stRecordingsExtContent;
}

static ONDK_Result_t	nx_recordingsExt_MsgGwmCreate(Nx_RecordingsExtContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_RecordingsExtContext_t));

	ONDK_GWM_APP_SetAppArea(NX_RECORDINGS_EXT_WINDOW_X, NX_RECORDINGS_EXT_WINDOW_Y,
							NX_RECORDINGS_EXT_WINDOW_W, NX_RECORDINGS_EXT_WINDOW_H);

	pstContents->eGroupType = (Nx_RecordGroupType_e)p1;
	HxSTD_snprintf(pstContents->szTitle, 256, "%s", (HCHAR*)p2);

	switch(pstContents->eGroupType)
	{
	case eNxRecGroup_CHANNEL:
		{
			Nx_RecGroupByChannel_t *pstChannelGroup = (Nx_RecGroupByChannel_t*)p3;
			pstContents->nRecListNum 	= HLIB_VECTOR_Length(pstChannelGroup->recItemList);
			pstContents->recList		= pstChannelGroup->recItemList;
		}
		break;

	default:
		break;
	}

	nx_recordingsExt_SetUiObject(pstContents);

	ONDK_GWM_Obj_EnableObject(NX_RECORDINGS_EXT_CONTENT_GRIDLIST_ID);
	ONDK_GWM_Obj_SetFocus(NX_RECORDINGS_EXT_CONTENT_GRIDLIST_ID);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordingsExt_MsgGwmDestroy(Nx_RecordingsExtContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	ONDK_GWM_PostMessage(NULL, MSG_APP_RECORDINGS_EXT_DESTROY, 0, 0, 0, 0);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordingsExt_MsgGwmKeyDown(Nx_RecordingsExtContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (p1)
	{
		/* keys to exit */
		case KEY_BACK:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_STANDBY:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
		case KEY_CH_PLUS:
		case KEY_CH_MINUS:
			break;

		case KEY_OK:
			break;

		default:
			NX_APP_Print("nx_recordings_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", p1);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_recordingsExt_SetUiObject(Nx_RecordingsExtContext_t *pstContents)
{
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };

	// fill bg
	stRect = ONDK_Rect(NX_RECORDINGS_EXT_WINDOW_X, NX_RECORDINGS_EXT_WINDOW_Y,
						NX_RECORDINGS_EXT_WINDOW_W, NX_RECORDINGS_EXT_WINDOW_H);
	ONDK_GWM_COM_Rect_Create(NX_RECORDINGS_EXT_FILL_BG_ID, &stRect, COL(C_G_Bang_80));

	/* Draw Title TEXT */
	stRect = ONDK_Rect(NX_RECORDINGS_EXT_TITLE_TEXT_X, NX_RECORDINGS_EXT_TITLE_TEXT_Y,
						NX_RECORDINGS_EXT_TITLE_TEXT_W, NX_RECORDINGS_EXT_TITLE_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_RECORDINGS_EXT_TITLE_ID, &stRect, pstContents->szTitle);
	ONDK_GWM_COM_Text_SetFont(NX_RECORDINGS_EXT_TITLE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_RECORDINGS_EXT_TITLE_ID, NX_PRISM_FONT_SIZE_36);
	ONDK_GWM_COM_Text_SetAlign(NX_RECORDINGS_EXT_TITLE_ID, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_RECORDINGS_EXT_TITLE_ID, COL(C_T_Gong01), COL(C_T_Bang_100));

	/* Draw TOP Line */
	stRect = ONDK_Rect(NX_RECORDINGS_EXT_TOPLINE_X, NX_RECORDINGS_EXT_TOPLINE_Y,
						NX_RECORDINGS_EXT_TOPLINE_W, NX_RECORDINGS_EXT_TOPLINE_H);
	ONDK_GWM_COM_Rect_Create(NX_RECORDINGS_EXT_TOPLINE_ID, &stRect, COL(C_T_Gong01));

#if 0
	/* Draw Content Frame */
	stRect = ONDK_Rect(NX_RECORDINGS_EXT_CONTENT_FRAME_X, NX_RECORDINGS_EXT_CONTENT_FRAME_Y,
						NX_RECORDINGS_EXT_CONTENT_FRAME_W, NX_RECORDINGS_EXT_CONTENT_FRAME_H);
	ONDK_GWM_COM_Rect_Create(NX_RECORDINGS_EXT_CONTENT_FRAME_ID, &stRect, COL(C_T_Bang_100));

#endif
	/* Setup Grid List */
HxLOG_Debug("rec ---->> listNum :%d\n\n", pstContents->nRecListNum);
	stRect = ONDK_Rect(NX_RECORDINGS_EXT_CONTENT_GRIDLIST_X, NX_RECORDINGS_EXT_CONTENT_GRIDLIST_Y,
		NX_RECORDINGS_EXT_CONTENT_GRIDLIST_W, NX_RECORDINGS_EXT_CONTENT_ITEM_H*NX_RECORDINGS_EXT_CONTENT_GRIDLIST_ROW);
	ONDK_GWM_COM_GridList_Create(NX_RECORDINGS_EXT_CONTENT_GRIDLIST_ID, &stRect, "Recording_contents_grid",
						NX_RECORDINGS_EXT_CONTENT_ITEM_W,
						NX_RECORDINGS_EXT_CONTENT_ITEM_H, pstContents->nRecListNum,
						NX_RECORDINGS_EXT_CONTENT_GRIDLIST_COL,
						NX_RECORDINGS_EXT_CONTENT_GRIDLIST_ROW, 0);
	ONDK_GWM_COM_GridList_SetCurrentIndex(NX_RECORDINGS_EXT_CONTENT_GRIDLIST_ID, 0);
#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
	ONDK_GWM_COM_GridList_SetAnimationCtl(NX_RECORDINGS_EXT_CONTENT_GRIDLIST_ID, nx_recordingsExt_SetAnimationCtl);
#endif
	ONDK_GWM_COM_GridList_SetItemDrawMethod(NX_RECORDINGS_EXT_CONTENT_GRIDLIST_ID, (GRID_ItemDraw_t)nx_recordingsExt_DrawContentsItemFunc);

	return	ONDK_RET_OK;
}


#if defined(CONFIG_APUI_SUPPORT_ANIMATION)
static ONDK_Result_t	nx_recordingsExt_SetAnimationCtl(void)
{
	ONDK_GWM_SetTimer(NX_RECORDINGS_EXT_LIST_ANI_ID, NX_RECORDINGS_EXT_LIST_ANI_TIMEOUT);
	return ONDK_RET_OK;
}
#endif

static ONDK_Result_t	nx_recordingsExt_DrawContentsItemFunc(HINT32 id, ONDKSurfaceHandle screen, GRID_DrawItem_t	*drawItem, HINT32 nColIndex)
{
	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	hFontHandle;
	HUINT32			ulFontSize;
	HINT32			currentIndex=0;

#if 1
	// mykim
	HINT32			nSurfaceX = nColIndex * NX_RECORDINGS_EXT_CONTENT_ITEM_W;
	ONDK_Rect_t		rectMainImg = {nSurfaceX + 10, 11, 170, 96};

	Nx_RecordingsExtContext_t	*pstContents = nx_recordingsExt_GetContents();

	Nx_RecordItem_t *pstItem = NULL;

	/* get focused */
	title_fontColor = COL (C_T_Bang_100);
	value_fontColor = COL (C_T_Bang_100);

	currentIndex = ONDK_GWM_COM_GridList_GetCurrentIndexByObjID(id);

	pstItem = (Nx_RecordItem_t*)HLIB_VECTOR_ItemAt(pstContents->recList, drawItem->item_index);
	// draw image
	if((pstItem != NULL) && (pstItem->bIsExistThumbnail == FALSE))
	{
		if (HLIB_FILE_Exist(pstItem->szThumbnailPath) == TRUE )
		{
			ONDK_Rect_t srcRect;
			ONDK_Size_t size;
			ONDKImageHandle hThum = NULL;

			hThum = ONDK_IMG_CreateSelf((HUINT8*)pstItem->szThumbnailPath);

			size = ONDK_IMG_GetSize(hThum);
			srcRect.x = 0;
			srcRect.y = 0;
			srcRect.w = size.w;
			srcRect.h = size.h;
			ONDK_SURFACE_StretchBlit(hThum, screen, srcRect, rectMainImg);

			pstItem->bIsExistThumbnail == TRUE;
		}
		else
		{
			ONDK_GUICOM_IMG_Blit(screen, rectMainImg, RES_212_04_THUMB_REC02_PNG);
		}
	}
	else
	{
		if ((pstItem != NULL) && (HLIB_FILE_Exist(pstItem->szThumbnailPath) == TRUE))
		{
			ONDK_Rect_t srcRect;
			ONDK_Size_t size;
			ONDKImageHandle hThum = NULL;

			hThum = ONDK_IMG_CreateSelf((HUINT8*)pstItem->szThumbnailPath);

			size = ONDK_IMG_GetSize(hThum);
			srcRect.x = 0;
			srcRect.y = 0;
			srcRect.w = size.w;
			srcRect.h = size.h;
			ONDK_SURFACE_StretchBlit(hThum, screen, srcRect, rectMainImg);
		}
		else
		{
			ONDK_GUICOM_IMG_Blit(screen, rectMainImg, RES_212_04_THUMB_REC02_PNG);
		}
	}

	ulFontSize = NX_PRISM_FONT_SIZE_24;
	eFontId = eFont_SystemBold;
	title_fontColor = COL (C_T_Whistle_100);

HxLOG_Debug("rec ===> drawitem index (%d)\n\n", drawItem->item_index);
	if (currentIndex == drawItem->item_index)
	{
		NX_PRISM_COMP_DrawFrameCursor_D1(screen, NULL, &rectMainImg, NULL, NULL, 0);

		title_fontColor = COL (C_T_Base01);
	}


	hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
	if (hFontHandle)
	{
		ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)pstItem->szName,
						rectMainImg.x + 5, (rectMainImg.y + rectMainImg.h + 5), rectMainImg.w - 30, title_fontColor, ONDKSTF_LEFT);
		ONDK_FONT_Release(hFontHandle);
	}

#endif
	return ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_RECORDINGS_Ext_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_RecordingsExtContext_t	*pstContents = NULL;

	pstContents = nx_recordingsExt_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_recordingsExt_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_recordingsExt_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_recordingsExt_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			//eResult = nx_recordings_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
			//eResult = nx_recordings_MsgGwmClick(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			//eResult = nx_recordings_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Record)
			{
				ONDK_GWM_APP_Destroy(0);
			}
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

#endif
/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

