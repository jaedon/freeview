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
 * @file	  		nx_prism_shahid.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/
#if defined(CONFIG_OP_SHAHID_APPS)
/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_prism_menu_contents.h>

#include <nx_prism_shahid.h>



/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_SHAHID_ITEM_ID						eMenuFrame_Window_Shahid

#define	NX_PRISM_SHAHID_WINDOW_X					0
#define	NX_PRISM_SHAHID_WINDOW_Y					0
#define	NX_PRISM_SHAHID_WINDOW_W					NX_PRISM_OSD_HORIZONTAL
#define	NX_PRISM_SHAHID_WINDOW_H					NX_PRISM_OSD_VERTICAL

#define NX_PRISM_SHAHID_DIM_ID						NX_PRISM_SHAHID_ITEM_ID+1

#define NX_PRISM_SHAHID_LOGO_IMG_ID					NX_PRISM_SHAHID_ITEM_ID+2
#define NX_PRISM_SHAHID_LOGO_IMG_X					57
#define NX_PRISM_SHAHID_LOGO_IMG_Y					33
#define NX_PRISM_SHAHID_LOGO_IMG_W					202
#define NX_PRISM_SHAHID_LOGO_IMG_H					45

#define NX_PRISM_SHAHID_MENU_ID						NX_PRISM_SHAHID_ITEM_ID+3
#define NX_PRISM_SHAHID_MENU_X						60
#define NX_PRISM_SHAHID_MENU_Y						103
#define NX_PRISM_SHAHID_MENU_W						260
#define NX_PRISM_SHAHID_MENU_H						584
#define NX_PRISM_SHAHID_MENU_CELL_H					52
#define NX_PRISM_SHAHID_MENU_ITEM_COUNT				11

#define NX_PRISM_SHAHID_CONTENTS_ID					NX_PRISM_SHAHID_ITEM_ID+4
#define NX_PRISM_SHAHID_CONTENTS_X					366
#define NX_PRISM_SHAHID_CONTENTS_Y					99
#define NX_PRISM_SHAHID_CONTENTS_W					854
#define NX_PRISM_SHAHID_CONTENTS_H					621 //- 90
#define NX_PRISM_SHAHID_CONTENTS_SQUARE_CELL_W		181+36
#define NX_PRISM_SHAHID_CONTENTS_SQUARE_CELL_H		205+55
#define NX_PRISM_SHAHID_CONTENTS_16BY9_CELL_W		179+38
#define NX_PRISM_SHAHID_CONTENTS_16BY9_CELL_H		149+24
#define NX_PRISM_SHAHID_CONTENTS_POSTER_CELL_W		126+50
#define NX_PRISM_SHAHID_CONTENTS_POSTER_CELL_H		226+37

#define NX_PRISM_SHAHID_BTN_SEEMORE_ID				NX_PRISM_SHAHID_ITEM_ID+5
#define NX_PRISM_SHAHID_BTN_SEEMORE_X				370
#define NX_PRISM_SHAHID_BTN_SEEMORE_Y				623
#define NX_PRISM_SHAHID_BTN_SEEMORE_W				833
#define NX_PRISM_SHAHID_BTN_SEEMORE_H				54














#if 0
#define	NX_PRISM_RECORDINGS_FRAME_ID				(NX_PRISM_RECORDINGS_ITEM_ID + 1)
#define	NX_PRISM_RECORDINGS_FRAME_X					0
#define	NX_PRISM_RECORDINGS_FRAME_Y					0
#define	NX_PRISM_RECORDINGS_FRAME_W					NX_PRISM_RECORDINGS_WINDOW_W
#define	NX_PRISM_RECORDINGS_FRAME_H					NX_PRISM_RECORDINGS_WINDOW_H

#endif

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

#if 0
typedef struct tagShahidContentsInfo{
	HCHAR	*programName;
	HCHAR	*szThumbnailPath;
	HCHAR	*szThumbnailURL;
	HBOOL	isNew;
	HBOOL	isPlus;
}ContentsInfo_t;
#endif

typedef struct tagShahidContext {
	HINT32	currentIndex;
	ContentsInfo_t	stArrayOfContentsInfo[16];

	ShahidShow		*stArrayOfShahidShow;
	HINT32			nShowCount;
	//ShahidShowList *showList;
	ShowListInfo 	nReqInfo;
} Nx_ShahidUiContext_t;



/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_ShahidUiContext_t		s_stShahidContext;
static HLONG					s_TestSemId = 0;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_ShahidUiContext_t	*nx_shahid_GetContext(void);
static ONDK_Result_t	nx_shahid_SetUiObject(Nx_ShahidUiContext_t *pstContext);
static ONDK_Result_t	nx_shahid_DrawMenuFunc(ONDKSurfaceHandle *screen, ONDK_Region_t *area, ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static HCHAR*			nx_shahid_GetMenuString(HINT32 index);
static ONDK_Result_t	nx_shahid_DrawContentsFunc(HINT32 id, ONDKSurfaceHandle screen, GRID_DrawItem_t	*drawItem, HINT32 nColIndex);
static ONDK_Result_t	nx_shahid_ArrowRight(void);
static ONDK_Result_t	nx_shahid_ArrowLeft(void);
static ONDK_Result_t	nx_shahid_KeyOk(void);
static ONDK_Result_t	nx_shahid_DrawHomeContents(Nx_ShahidUiContext_t *pstContext);
static ONDK_Result_t	nx_shahid_DrawSeriesContents(Nx_ShahidUiContext_t *pstContext);
static ONDK_Result_t	nx_shahid_DrawShowsContents(Nx_ShahidUiContext_t *pstContext);
static ONDK_Result_t	nx_shahid_DrawChannelContents(Nx_ShahidUiContext_t *pstContext);
static ONDK_Result_t	nx_shahid_DrawMovieContents(Nx_ShahidUiContext_t *pstContext);
static ONDK_Result_t	nx_shahid_DrawVideoClipsContents(Nx_ShahidUiContext_t *pstContext);








/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


void setContentsData(Nx_ShahidUiContext_t *pstContext)
{


#if 1
	HERROR ret;

	//nReqInfo.pageNumber = 0;
	//nReqInfo.pageSize = 8;

	pstContext->nReqInfo.pageNumber = 0;
	pstContext->nReqInfo.pageSize = 16;
	//ret = NX_SHAHID_ReqShowList(&nReqInfo);
	ret = NX_SHAHID_ReqShowList(&(pstContext->nReqInfo));
	if(ret == ERR_FAIL)
	{
		HxLOG_Print("curl request fail \n");

	}

	pstContext->stArrayOfShahidShow = NULL;

#endif


}

static inline Nx_ShahidUiContext_t	*nx_shahid_GetContext(void)
{
	return	&s_stShahidContext;
}

static HCHAR*			nx_shahid_GetMenuString(HINT32 index)
{
	switch(index)
	{
		case 0:
			return "HOME";
		case 1:
			return "SERIES";
		case 2:
			return "SHOWS";
		case 3:
			return "CHANNEL";
		case 4:
			return "MOVIE";
		case 5:
			return "VIDEO CLIPS";
		default:
			break;
	}
	return NULL;
}

static ONDK_Result_t	nx_shahid_DrawMenuFunc(ONDKSurfaceHandle * screen,ONDK_Region_t * area,
	ONDK_Rect_t * rect,ONDK_GWM_GuiObject_t object,void * arg,int bUiTop)
{
	HUINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HUINT32		x = 0, y = 0, w = 0, h = 0, lCursorMg = 8;
	HUINT32		i, lCellHeight = 0, lNumOfPage = 0;
	HUINT32		lTotal = 0, lStart = 0, lCurrent = 0, lSelect = 0, lCursor_y_off = 0;
	HUINT32		lText_x_offset = 20, lText_y_offset = 19;
	HUINT32		lineThickness= 0;

	HUINT32		ulStatus = 0, ulFontSize = 0, lItem = 0;
	HUINT8		*title = NULL;
	HBOOL		bFocus = FALSE;

	ONDK_Color_t	title_fontColor = 0x00000000, value_fontColor = 0x00000000, cursorColor = 0x00000000;
	ONDK_FontId_t	eFontId = eFont_SystemNormal;
	ONDKFontHandle	*hFontHandle = NULL;


	LIST_OBJECT 		obj = (LIST_OBJECT)object;


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	ONDK_GWM_List_GetProperty(obj, &lTotal, &lStart, &lCurrent, &lSelect, &lNumOfPage);
	NX_APP_Info("lTotal[%d], stat[%d], lCurrent[%d], lSelect[%d], lNumOfPage[%d]\n", lTotal, lStart, lCurrent, lSelect, lNumOfPage);
	if (lTotal == 0)
	{
		NX_APP_Error("ERROR!!! (lTotal == 0) !!\n");
		return	ONDK_RET_FAIL;
	}

	ulFontSize = NX_PRISM_FONT_SIZE_22;
	lCursorMg = 0;
	lineThickness = 2;

	ulStatus = ONDK_GWM_List_GetStatus(obj);
	lCellHeight = ONDK_GWM_List_GetCellHeight(ONDK_GWM_Obj_GetObjectID(object));

	/* active, front most */
	for (i = 0; i < lTotal; i++)
	{
		lItem = i;//(HUINT32)(lStart + i);

		title = nx_shahid_GetMenuString(i);

		x = x1;
		y = y1 + lCursor_y_off;

		if (lItem == lCurrent)
		{
			if(ulStatus == GUIOBJ_HILIGHTED)
			{
				/* get focused */
				bFocus = TRUE;
				title_fontColor = COL (C_T_Shahid_Black);
				value_fontColor = COL (C_T_Shahid_Black);
				cursorColor = COL(C_G_Shahid_03);
				eFontId = eFont_SystemBold;
				ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight, cursorColor);
			}
			else
			{
				/* selected */
				bFocus = TRUE;
				title_fontColor = COL (C_G_Shahid_03);
				value_fontColor = COL (C_G_Shahid_03);
				cursorColor = COL(C_T_Shahid_Black);
				eFontId = eFont_SystemBold;

				ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight, cursorColor);
				ONDK_DRAW_FillRectElement(screen, x, y, w, lineThickness, COL(C_G_Shahid_03));
				ONDK_DRAW_FillRectElement(screen, x, y+lCellHeight-lineThickness, w, lineThickness, COL(C_G_Shahid_03));
			}

		}
		else
		{
			/* Normal Item cursor */
			bFocus = FALSE;
			title_fontColor = COL (C_T_Gong02);
			value_fontColor = COL (C_T_Gong02);
			cursorColor = COL(C_T_Shahid_Black);
			eFontId = eFont_SystemNormal;
			ONDK_DRAW_FillRectElement(screen, x, y, w, lCellHeight, cursorColor);

			if(i==0)
			{
				ONDK_DRAW_FillRectElement(screen, x, y, w, lineThickness, COL(C_G_Whistle_20));
			}
			else
			{
				ONDK_DRAW_FillRectElement(screen, x, y, w, lineThickness /2, COL(C_G_Whistle_20));
			}

			if(i == (lTotal - 1))
			{
				ONDK_DRAW_FillRectElement(screen, x, y+lCellHeight-lineThickness, w, lineThickness, COL(C_G_Whistle_20));
			}
			else
			{
				ONDK_DRAW_FillRectElement(screen, x, y+lCellHeight-lineThickness /2, w, lineThickness /2, COL(C_G_Whistle_20));
			}
		}

		hFontHandle = ONDK_FONT_CreateSystemFont(eFontId, ulFontSize);
		if (hFontHandle)
		{
			// SubItem - Title
			x = x1 + lText_x_offset;
			y = y1 + lCursor_y_off + lText_y_offset;
			ONDK_FONT_DrawStringAlignAbbr(screen, hFontHandle, (char*)title, x, y-5, 220, title_fontColor, ONDKSTF_LEFT);
			ONDK_FONT_Release(hFontHandle);
		}


//		lCursor_y_off += (NX_PRISM_RECORDINGS_BTN_MG_18 - NX_PRISM_RECORDINGS_BTN_MG_8);
		lCursor_y_off += lCellHeight;
	}

	NX_COMMON_NOT_USED_ARGUMENT(screen, area, rect, object);
	NX_COMMON_NOT_USED_PARAM(arg);
	NX_COMMON_NOT_USED_PARAM(bUiTop);

	return	ONDK_RET_OK;

}

static ONDK_Result_t	nx_shahid_MsgGwmCreate(Nx_ShahidUiContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{

	if (pstContext == NULL)
	{
		return ONDK_RET_FAIL;
	}

	if ( VK_SEM_Create(&s_TestSemId, "ShahidSemapore", VK_SUSPENDTYPE_PRIORITY ) != VK_OK )
	{
		return ONDK_RET_FAIL;
	}

	HxSTD_MemSet(pstContext, 0, sizeof(Nx_ShahidUiContext_t));

	//Set Data
	setContentsData(pstContext);


	ONDK_GWM_APP_SetAppArea(NX_PRISM_SHAHID_WINDOW_X, NX_PRISM_SHAHID_WINDOW_Y,
							NX_PRISM_SHAHID_WINDOW_W, NX_PRISM_SHAHID_WINDOW_H);
	//Set UI object
	nx_shahid_SetUiObject(pstContext);

	ONDK_GWM_Obj_SetFocus(NX_PRISM_SHAHID_MENU_ID);


	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_MsgGwmDestroy(Nx_ShahidUiContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{

	NX_COMMON_NOT_USED_PARAM(pstContext);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_shahid_MsgGwmHapiMsg(Nx_ShahidUiContext_t *pstContext, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContext);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_shahid_MsgGwmKeyDown(Nx_ShahidUiContext_t *pstContext, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t res = ONDK_RET_FAIL;

	if (pstContext == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
	NX_COMMON_NOT_USED_ARGUMENT(pstContext, p1, p2, p3);

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

		case KEY_OK:
			nx_shahid_KeyOk();
			break;
		case KEY_ARROWLEFT:
			nx_shahid_ArrowLeft();
			break;
		case KEY_ARROWRIGHT:
			res = nx_shahid_ArrowRight();
			if(res != ONDK_RET_OK)
			{
				return ONDK_MESSAGE_BREAK;
			}
			break;

		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		default:
			NX_APP_Print("nx_recordings_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", p1);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_SetUiObject(Nx_ShahidUiContext_t *pstContext)
{

	ONDK_Rect_t stRect;

	if(pstContext == NULL)
	{
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}
	//Dim color
	stRect = ONDK_Rect(NX_PRISM_SHAHID_WINDOW_X, NX_PRISM_SHAHID_WINDOW_Y, NX_PRISM_SHAHID_WINDOW_W, NX_PRISM_SHAHID_WINDOW_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_SHAHID_DIM_ID, &stRect, COL(C_G_Bang_85));

	//Logo image
	stRect = ONDK_Rect(NX_PRISM_SHAHID_LOGO_IMG_X, NX_PRISM_SHAHID_LOGO_IMG_Y, NX_PRISM_SHAHID_LOGO_IMG_W, NX_PRISM_SHAHID_LOGO_IMG_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_SHAHID_LOGO_IMG_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_SHAHID_LOGO_IMG_ID, RES_602_11_SHAHID_LOGO_PNG);

	//Menu area
	stRect = ONDK_Rect(NX_PRISM_SHAHID_MENU_X, NX_PRISM_SHAHID_MENU_Y, NX_PRISM_SHAHID_MENU_W, NX_PRISM_SHAHID_MENU_H);
	ONDK_GWM_List_Create(NX_PRISM_SHAHID_MENU_ID, &stRect, "NxShahidMenuList", NX_PRISM_SHAHID_MENU_CELL_H,
							NX_PRISM_SHAHID_MENU_ITEM_COUNT, 0, 0);
	ONDK_GWM_List_SetFontHandle(NX_PRISM_SHAHID_MENU_ID, eFont_SystemBold, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_List_SetCurrentIndex(NX_PRISM_SHAHID_MENU_ID, 0);
	ONDK_GWM_List_SetSelectedIndex(NX_PRISM_SHAHID_MENU_ID, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SHAHID_MENU_ID, (GWM_ObjDraw_t)nx_shahid_DrawMenuFunc);

	//Contents area
	stRect = ONDK_Rect(NX_PRISM_SHAHID_CONTENTS_X, NX_PRISM_SHAHID_CONTENTS_Y, NX_PRISM_SHAHID_CONTENTS_W, NX_PRISM_SHAHID_CONTENTS_H);
	ONDK_GWM_COM_GridList_Create(NX_PRISM_SHAHID_CONTENTS_ID, &stRect, "NxShahidContents", 30, 30, 22, 3, 2, 0);
	//draw Home contents
	nx_shahid_DrawHomeContents(pstContext);

	//SeeMore button
	stRect = ONDK_Rect(NX_PRISM_SHAHID_BTN_SEEMORE_X, NX_PRISM_SHAHID_BTN_SEEMORE_Y, NX_PRISM_SHAHID_BTN_SEEMORE_W, NX_PRISM_SHAHID_BTN_SEEMORE_H);
	ONDK_GWM_COM_Button_Create(NX_PRISM_SHAHID_BTN_SEEMORE_ID, &stRect, "SEE MORE");
	ONDK_GWM_COM_Button_ChangeImage(NX_PRISM_SHAHID_BTN_SEEMORE_ID, RES_602_11_SHAHID_ARROW_DOWN_N_PNG,RES_602_11_SHAHID_ARROW_DOWN_C_PNG,NULL,NULL);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_SHAHID_BTN_SEEMORE_ID, NX_SHAHID_COM_DrawBtnSeeMore);

	return	ONDK_RET_OK;

}

static ONDK_Result_t	nx_shahid_ArrowLeft(void)
{
	if(ONDK_GWM_COM_GridList_IsFirstColumn(NX_PRISM_SHAHID_CONTENTS_ID) == FALSE)
	{
		//ONDK_GWM_Obj_SetFocus(NX_PRISM_RECORDINGS_CONTENTS_GRID_ID);
	}
	else
	{
		ONDK_GWM_Obj_DisableObject(NX_PRISM_SHAHID_CONTENTS_ID);
		ONDK_GWM_Obj_EnableObject(NX_PRISM_SHAHID_MENU_ID);
		ONDK_GWM_Obj_SetFocus(NX_PRISM_SHAHID_MENU_ID);
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_ArrowRight(void)
{
	HINT32 objStatus;
	objStatus = ONDK_GWM_Obj_GetObjectStatus(ONDK_GWM_Obj_GetObjectByID(NX_PRISM_SHAHID_MENU_ID));
	if(objStatus == GUIOBJ_HILIGHTED)
	{
		ONDK_GWM_Obj_DisableObject(NX_PRISM_SHAHID_MENU_ID);
		ONDK_GWM_Obj_EnableObject(NX_PRISM_SHAHID_CONTENTS_ID);
		ONDK_GWM_Obj_SetFocus(NX_PRISM_SHAHID_CONTENTS_ID);
		return ONDK_RET_FAIL;
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_KeyOk(void)
{
	HINT32 curIndex;
	Nx_ShahidUiContext_t *pstContext;
	pstContext = nx_shahid_GetContext();
	curIndex = ONDK_GWM_List_GetCurrentIndex(NX_PRISM_SHAHID_MENU_ID);

	switch(curIndex)
	{
		case 0:
			nx_shahid_DrawHomeContents(pstContext);
			break;
		case 1:
			nx_shahid_DrawSeriesContents(pstContext);
			break;
		case 2:
			nx_shahid_DrawShowsContents(pstContext);
			break;
		case 3:
			nx_shahid_DrawChannelContents(pstContext);
			break;
		case 4:
			nx_shahid_DrawMovieContents(pstContext);
			break;
		case 5:
			nx_shahid_DrawVideoClipsContents(pstContext);
			break;
		default:
			HxLOG_Print("Unknown list index\n");
			break;

	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_DrawHomeContents(Nx_ShahidUiContext_t *pstContext)
{
	NX_SHAHID_COM_SetContentsItem(pstContext->stArrayOfContentsInfo);
	ONDK_GWM_COM_GridList_ChangeProperty(NX_PRISM_SHAHID_CONTENTS_ID, NX_PRISM_SHAHID_CONTENTS_SQUARE_CELL_W,
		NX_PRISM_SHAHID_CONTENTS_SQUARE_CELL_H, 16,4,2,0);
	ONDK_GWM_COM_GridList_SetCurrentIndex(NX_PRISM_SHAHID_CONTENTS_ID, 0);
	ONDK_GWM_COM_GridList_SetItemDrawMethod(NX_PRISM_SHAHID_CONTENTS_ID, (GRID_ItemDraw_t)NX_SHAHID_COM_DrawSquareTemplate);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_DrawSeriesContents(Nx_ShahidUiContext_t *pstContext)
{
	NX_SHAHID_COM_SetContentsItem(pstContext->stArrayOfContentsInfo);
	ONDK_GWM_COM_GridList_ChangeProperty(NX_PRISM_SHAHID_CONTENTS_ID, NX_PRISM_SHAHID_CONTENTS_16BY9_CELL_W,
		NX_PRISM_SHAHID_CONTENTS_16BY9_CELL_H, 16,4,3,0);
	ONDK_GWM_COM_GridList_SetCurrentIndex(NX_PRISM_SHAHID_CONTENTS_ID, 0);
	ONDK_GWM_COM_GridList_SetItemDrawMethod(NX_PRISM_SHAHID_CONTENTS_ID, (GRID_ItemDraw_t)NX_SHAHID_COM_Draw16by9Template);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_DrawShowsContents(Nx_ShahidUiContext_t *pstContext)
{
	NX_SHAHID_COM_SetContentsItem(pstContext->stArrayOfContentsInfo);
	ONDK_GWM_COM_GridList_ChangeProperty(NX_PRISM_SHAHID_CONTENTS_ID, NX_PRISM_SHAHID_CONTENTS_16BY9_CELL_W,
		NX_PRISM_SHAHID_CONTENTS_16BY9_CELL_H, 16,4,3,0);
	ONDK_GWM_COM_GridList_SetCurrentIndex(NX_PRISM_SHAHID_CONTENTS_ID, 0);
	ONDK_GWM_COM_GridList_SetItemDrawMethod(NX_PRISM_SHAHID_CONTENTS_ID, (GRID_ItemDraw_t)NX_SHAHID_COM_Draw16by9Template);
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_DrawChannelContents(Nx_ShahidUiContext_t *pstContext)
{
	NX_SHAHID_COM_SetContentsItem(pstContext->stArrayOfContentsInfo);
	ONDK_GWM_COM_GridList_ChangeProperty(NX_PRISM_SHAHID_CONTENTS_ID, NX_PRISM_SHAHID_CONTENTS_16BY9_CELL_W,
		NX_PRISM_SHAHID_CONTENTS_16BY9_CELL_H, 16,4,3,0);
	ONDK_GWM_COM_GridList_SetCurrentIndex(NX_PRISM_SHAHID_CONTENTS_ID, 0);
	ONDK_GWM_COM_GridList_SetItemDrawMethod(NX_PRISM_SHAHID_CONTENTS_ID, (GRID_ItemDraw_t)NX_SHAHID_COM_Draw16by9Template);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_DrawMovieContents(Nx_ShahidUiContext_t *pstContext)
{

	NX_SHAHID_COM_SetContentsItem(pstContext->stArrayOfContentsInfo);
	ONDK_GWM_COM_GridList_ChangeProperty(NX_PRISM_SHAHID_CONTENTS_ID, NX_PRISM_SHAHID_CONTENTS_POSTER_CELL_W,
		NX_PRISM_SHAHID_CONTENTS_POSTER_CELL_H, 16,5,2,0);
	ONDK_GWM_COM_GridList_SetCurrentIndex(NX_PRISM_SHAHID_CONTENTS_ID, 0);
	ONDK_GWM_COM_GridList_SetItemDrawMethod(NX_PRISM_SHAHID_CONTENTS_ID, (GRID_ItemDraw_t)NX_SHAHID_COM_DrawPosterTemplate);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_shahid_DrawVideoClipsContents(Nx_ShahidUiContext_t *pstContext)
{
	NX_SHAHID_COM_SetContentsItem(pstContext->stArrayOfContentsInfo);
	ONDK_GWM_COM_GridList_ChangeProperty(NX_PRISM_SHAHID_CONTENTS_ID, NX_PRISM_SHAHID_CONTENTS_16BY9_CELL_W,
		NX_PRISM_SHAHID_CONTENTS_16BY9_CELL_H, 16,4,3,0);
	ONDK_GWM_COM_GridList_SetCurrentIndex(NX_PRISM_SHAHID_CONTENTS_ID, 0);
	ONDK_GWM_COM_GridList_SetItemDrawMethod(NX_PRISM_SHAHID_CONTENTS_ID, (GRID_ItemDraw_t)NX_SHAHID_COM_Draw16by9Template);

	return ONDK_RET_OK;
}



/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_SHAHID_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	Nx_ShahidUiContext_t	*pstContext = NULL;

	pstContext = nx_shahid_GetContext();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_shahid_MsgGwmCreate(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_shahid_MsgGwmKeyDown(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_shahid_MsgGwmDestroy(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			//NX_APP_Message("MSG_GWM_HAPI_MESSAGE!\n");
			//eResult = nx_recordings_MsgGwmHapiMsg(pstContext, handle, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
			//eResult = nx_recordings_MsgGwmClick(pstContext, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			//NX_APP_Message("MSG_GWM_TIMER!\n");
			//eResult = nx_recordings_MsgGwmTimer(pstContext, p1, p2, p3);
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			//NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			//if (p1 != eNX_UCT_Record)
			//{
			//	ONDK_GWM_APP_Destroy(0);
			//}
			break;

		case MSG_CORE_CONTENTS_SHAHID_DATA_DB_UPDATED:
		{
			ShahidShowList *showList;
			int i, cnt; //temporary use
			showList = NX_SHAHID_GetShowList(&(pstContext->nReqInfo));
			pstContext->stArrayOfShahidShow = (ShahidShow*)malloc(sizeof(ShahidShow)*(showList->length));
			pstContext->nShowCount = showList->length;
			pstContext->stArrayOfShahidShow = showList->stArrayOfShahidShow;

//			VK_SEM_Get(s_TestSemId);

			for(i = 0; i < pstContext->nShowCount; i++)
			{
				cnt = 0;
				pstContext->stArrayOfContentsInfo[i].isNew = TRUE;
				pstContext->stArrayOfContentsInfo[i].isPlus = TRUE;
				pstContext->stArrayOfContentsInfo[i].pszProgramName = NULL;
		#if defined(CONFIG_PROD_LANG_ARABIC)
				//pstContext->stArrayOfContentsInfo[i].pszProgramName = HLIB_STD_StrDup(NX_PRISM_ARABIC_SUPPORT_ConvertToArabicStr(pstContext->stArrayOfShahidShow[i].name));
		#endif
				if(pstContext->stArrayOfContentsInfo[i].pszProgramName == NULL)
				{
					pstContext->stArrayOfContentsInfo[i].pszProgramName = HLIB_STD_StrDup(pstContext->stArrayOfShahidShow[i].name);
				}
				pstContext->stArrayOfContentsInfo[i].pszThumbnailURL = HLIB_STD_StrDup(pstContext->stArrayOfShahidShow[i].thumbnailUrl);

#if !defined(CONFIG_PROD_OS_EMULATOR)
				pstContext->stArrayOfContentsInfo[i].pszThumbnailPath = HLIB_STD_StrDup(THAPI_GetThumbnailURL((HCHAR*)pstContext->stArrayOfContentsInfo[i].pszThumbnailURL, 0, "/var/lib/humaxtv_user/shahid", TRUE));
#endif
			}

//			VK_SEM_Release(s_TestSemId);

			nx_shahid_DrawHomeContents(pstContext);


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

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

#endif
