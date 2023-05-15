/**
	@file     ap_cas_va_op_message.c
	@brief    file_name & simple comment.

	Description: \n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/09/10		initiated					mhoh\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

//******************************************************************************
//	INCLUDE
//******************************************************************************
#include <svc_sys.h>
#include <db_svc.h>
#include <db_param.h>
#include <isosvc.h>

#include <gwm.h>
#include <osd_gfx.h>
#include <osd_font.h>
#include <mgr_action.h>
#include <ap_resource.h>
#include <popupui_coor.h>
#include "settings.h"
#include "ap_menu_contents.h"
#include "ap_system_coordinate.h"
#include <ap_util_osd.h>
#include <surferui_coor.h>
#include <util_dslib.h>

#include "ap_cas_va.h"
#include "va_ui_adapt.h"


//******************************************************************************
//	DEFINE
//******************************************************************************
#ifdef CONFIG_DEBUG
HUINT32 g_ApVaOpMsg_Level = (DBG_ASSERT)|0xff;
HUINT32 g_VaOpMsg_DbgColor = 30;

#define VaOpMsgPrint(level, message)	(((level) & g_ApVaOpMsg_Level) ? (VK_Print("\033[01;%dm", g_VaOpMsg_DbgColor), VK_Print("[App:System]"), VK_Print message, VK_Print("\033[00m")) : 0)
#define VaOpMsgAssert(c) 				((c) ? 0 : (VK_Print("\nSystem assertion on %s: line%d\n\n", __FILE__, __LINE__)))
#define VaOpMsgData(level, message)		(((level) & g_ApVaOpMsg_Level) ? ( VK_Print message) : 0)
#else
#define VaOpMsgPrint(level, message)
#define VaOpMsgAssert(c)
#define VaOpMsgData(level, message)
#endif


#define	VA_CA_MAILLIST_INDEX_NUM 				20
#define	VA_CA_MAILLIST_DISPLAY_ITEM_NUM			9

#define	VA_CA_MAILLIST_CHECK_OFF				0
#define	VA_CA_MAILLIST_CHECK_ON					1

#define	VA_CA_MAILLIST_CHECK_ALL_TRUE			1
#define	VA_CA_MAILLIST_CHECK_ALL_FALSE			0


#define	VA_OPMSG_TEXT_WIN_X						209
#define	VA_OPMSG_TEXT_WIN_Y						143
#define	VA_OPMSG_TEXT_WIN_W 					758
#define	VA_OPMSG_TEXT_WIN_H 					400

#define	VA_OPMSG_TEXT_TITILE_TEXT_X 			VA_OPMSG_TEXT_WIN_X
#define	VA_OPMSG_TEXT_TITILE_TEXT_Y 			(VA_OPMSG_TEXT_WIN_Y + 13)
#define	VA_OPMSG_TEXT_TITILE_TEXT_W 			VA_OPMSG_TEXT_WIN_W
#define	VA_OPMSG_TEXT_TITILE_TEXT_H 			58

#define	VA_OPMSG_TEXT_TITILE_IMAGE_W 			55
#define	VA_OPMSG_TEXT_TITILE_IMAGE_X			(VA_OPMSG_TEXT_WIN_X + 200 + VA_OPMSG_TEXT_TITILE_IMAGE_W)
#define	VA_OPMSG_TEXT_TITILE_IMAGE_Y			(VA_OPMSG_TEXT_TITILE_TEXT_Y - 2)

#define	VA_OPMSG_TEXT_FMT_X		  				38
#define	VA_OPMSG_TEXT_FMT_Y		  				80
#define	VA_OPMSG_TEXT_FMT_W		  				705
#define	VA_OPMSG_TEXT_FMT_H		  				(VA_OPMSG_TEXT_WIN_H - 150)
#define	VA_OPMSG_TEXT_FTM_SCROLL_X				730
#define	VA_OPMSG_TEXT_FTM_SCROLL_Y				80


#define	VA_OPMSG_TEXT_HELP_X		 			VA_OPMSG_TEXT_WIN_X
#define	VA_OPMSG_TEXT_HELP_Y		  			(VA_OPMSG_TEXT_WIN_Y + VA_OPMSG_TEXT_FMT_Y + VA_OPMSG_TEXT_FMT_H)
#define	VA_OPMSG_TEXT_HELP_W		  			VA_OPMSG_TEXT_FMT_W
#define	VA_OPMSG_TEXT_HELP_H		  			50

#define	VA_OPMSG_TEXT_HELP_IMAGE_X		  		(VA_OPMSG_TEXT_HELP_X + 30)
#define	VA_OPMSG_TEXT_HELP_IMAGE_Y		  		(VA_OPMSG_TEXT_HELP_Y + 10)
#define	VA_OPMSG_TEXT_HELP_IMAGE_W		 		65
#define	VA_OPMSG_TEXT_HELP_IMAGE_H		  		30
#define	VA_OPMSG_TEXT_HELP_TEXT_GAP		  		100

#define	VA_OPMSG_LIST_BOX_X					143
#define	VA_OPMSG_LIST_BOX_Y					178
#define	VA_OPMSG_LIST_BOX_W					987
#define	VA_OPMSG_LIST_BOX_H					478

#define	VA_OPMSG_LIST_FMT_X					(VA_OPMSG_LIST_BOX_X + 30)
#define	VA_OPMSG_LIST_FMT_Y					(VA_OPMSG_LIST_BOX_Y + 20)
#define	VA_OPMSG_LIST_FMT_W					(VA_OPMSG_LIST_BOX_W - 50)
#define	VA_OPMSG_LIST_FMT_H					(VA_OPMSG_LIST_BOX_H - 40)

#define	VA_OPMSG_LIST_TITLE_X					521
#define	VA_OPMSG_LIST_TITLE_Y					134
#define	VA_OPMSG_LIST_TITLE_W					(VA_OPMSG_LIST_TITLE_X + 250)
#define	VA_OPMSG_LIST_TITLE_H					(VA_OPMSG_LIST_TITLE_Y + 50)
#define	VA_OPMSG_LIST_TITLE_DATE_GAP_W		15
#define	VA_OPMSG_LIST_TITLE_DATE_GAP_H		15

#define	VA_OPMSG_LIST_TITLE_TIME_GAP_W		25
#define	VA_OPMSG_LIST_TITLE_DATE_GAP_Y		VA_OPMSG_LIST_TITLE_DATE_GAP_H
#define	VA_OPMSG_LIST_SCROLL_GAP_W			15
#define	VA_OPMSG_LIST_SCROLL_GAP_Y			5
#define	VA_OPMSG_LIST_SCROLL_GAP_H			30

#define	VA_OPMSG_LIST_HELP_X					(VA_OPMSG_LIST_BOX_X + 20)
#define	VA_OPMSG_LIST_HELP_Y					(VA_OPMSG_LIST_BOX_Y + VA_OPMSG_LIST_BOX_H)
#define	VA_OPMSG_LIST_HELP_W					(VA_OPMSG_LIST_HELP_X + VA_OPMSG_LIST_BOX_W)
#define	VA_OPMSG_LIST_HELP_H					(VA_OPMSG_LIST_HELP_Y + 30)
#define	VA_OPMSG_LIST_HELP_TXT_IMG_GAP		5
#define	VA_OPMSG_LIST_HELE_ITEM_GAP			80

#define	VA_OPMSG_LIST_CURSOR_X				20
#define	VA_OPMSG_LIST_CURSOR_Y				15
#define	VA_OPMSG_LIST_CURSOR_W				110

#define	VA_OPMSG_LIST_ITEM_X					20
#define	VA_OPMSG_LIST_ITEM_Y					20
#define	VA_OPMSG_LIST_ITEM_IMG_GAP_X			12
#define	VA_OPMSG_LIST_ITEM_IMG_GAP_Y			12
#define	VA_OPMSG_LIST_ITEM_IMG_GAP_W			15
#define	VA_OPMSG_LIST_ITEM_DATE_GAP_W			15
#define	VA_OPMSG_LIST_ITEM_TIME_GAP_W			30
#define	VA_OPMSG_LIST_ITEM_TEXT_GAP_W			30
#define	VA_OPMSG_LIST_ITEM_TEXT_W				550

#define	VA_OPMSG_LIST_ITEM_SCROLL_H			420

#define	VA_OPMSG_LIST_BOX_CREATE_COL			0x00000000


/* 	Attributed mail Message	*/
// Safe Area
#define	VA_OPMSG_LIST_OSD_SCREEN_SAFE_X					96
#define	VA_OPMSG_LIST_OSD_SCREEN_SAFE_Y					54
#define	VA_OPMSG_LIST_OSD_SCREEN_SAFE_W					1088
#define	VA_OPMSG_LIST_OSD_SCREEN_SAFE_H					612

#define	CAS_VA_SCROLL_W					9
#define	CAS_VA_SCROLL_H				   400//432
#define	CAS_VA_SCROLL_X				   510
#define	CAS_VA_SCROLL_Y				   23
#define	CAS_VA_SCROLLBALL_H				20

#define	VA_OPMSG_LIST_APP_WINDOW_WIDTH			OSD_SCREEN_WIDTH
#define	VA_OPMSG_LIST_APP_WINDOW_HEIGHT		OSD_SCREEN_HEIGHT
#define	VA_OPMSG_LIST_APP_WINDOW_X			0
#define	VA_OPMSG_LIST_APP_WINDOW_Y			0

typedef enum
{
	eOpMsg_List_BOX_ID = 0,
	eOpMsg_List_ITEM,
	eOpMsg_List_ICON_ID,
	eOpMsg_List_FMT_ID,
	eOpMsg_List_HELP_ID,
	eOpMsg_List_MSG_ID_MDX
} OpMsg_ListIds;

typedef enum
{
	eOpMsg_Text_FRAME_ID = 0,
	eOpMsg_Text_FRAME_TITLE_ID,
	eOpMsg_Text_FMT_ID,
	eOpMsg_Text_HELP_ID,
	eOpMsg_Text_ID_MAX
} OpMsg_TextIds;


//******************************************************************************
//	Type Definition
//******************************************************************************
typedef struct tagMailItem
{
	HBOOL	bCheck;
	HUINT8	strReceivedDate[6];
	HUINT8	strReceivedTime[9];
	VA_UI_OP_MESSAGE	stOpMsg;
} CAS_VA_OPMSGITEM_DATA;


typedef struct tag_CAS_VaOpMsg_content
{
	/* for drawing application window */
	HUINT32 		ulCurFocus;
	HUINT32 		ulNumOfSubItem, ulMaxNumOfSubItem;
	SettingsLeafSubItem_t	*subItems;

	/* running info for app */
	HUINT8			ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	HINT32			lCasItemID;

	/* List Item */
	HBOOL			bDrawCursor;
	Custom_list_t	stTextListBox;
	GWM_ObjDraw_t	objDrawFunc;

	/* Dialog*/
	AP_DialogInstance_t 	stDialogInstance;
} CasVaList_Content_t;

//******************************************************************************
//	STATIC VARIABLES
//******************************************************************************
STATIC CasVaList_Content_t		s_VaOpMsgContent;
STATIC SettingsNaviContext_t	s_VaOpMsgTextContent;
STATIC CAS_VA_OPMSGITEM_DATA		s_stVaOpMsgItemData[VA_CA_MAILLIST_INDEX_NUM];

STATIC HUINT32					s_VaOpMsgCurFocus = 0;

STATIC AP_DialogInstance_t 		s_cas_Va_OpMsg_dInstance;

STATIC BUS_Result_t	local_cas_va_OpMsgList_delete_MsgDlgClicked(SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t	local_cas_va_OpMsgList_delete_CreateDialog(HUINT32 ulDLGFlag, LOC_ID_t eLocId);
STATIC BUS_Result_t	local_cas_va_OpMsgList_delete_DestroyDialog(void);
STATIC BUS_Result_t	local_cas_va_OpMsgList_delete_Dialog_Proc(HINT32 nMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);


//******************************************************************************
//	CODE START
//******************************************************************************


#define	_________________________BODY______________________________



STATIC void local_cas_va_OpMsgList_LoadMailList(CasVaList_Content_t *pstContents)
{
	HINT32 					i;
	HUINT32 					ulCnt;
	HxDATETIME_t		stDateTime;
	HERROR			hErr;
	CAS_VA_OPMSGITEM_DATA	*pMailItemData = NULL;

	VaOpMsgPrint(DBG_TRACE, ("local_cas_va_OpMsgList_LoadMailList() +\n"));

	VK_memset32(pstContents, 0, sizeof(CasVaList_Content_t));

	ulCnt = VA_UI_Adapt_GetOpMessageCount();
	if( ulCnt > 0 )
	{
		pstContents->ulNumOfSubItem = ulCnt;
		pstContents->ulCurFocus = s_VaOpMsgCurFocus;
	}
	else
	{
		pstContents->ulNumOfSubItem = 0;
		pstContents->ulCurFocus = 0;
	}

	VaOpMsgPrint(DBG_PRINT, ("Mail Count = %d\n", pstContents->ulNumOfSubItem));

	for (i=0 ; i< pstContents->ulNumOfSubItem ; i++)
	{
		if (VA_UI_Adapt_GetOpMessage(&s_stVaOpMsgItemData[i].stOpMsg, i) == ERR_OK)
		{
			s_stVaOpMsgItemData[i].bCheck = VA_CA_MAILLIST_CHECK_OFF;

			HLIB_DATETIME_ConvertUnixTimeToDateTime(s_stVaOpMsgItemData[i].stOpMsg.stUnixTime, &stDateTime);

			snprintf(s_stVaOpMsgItemData[i].strReceivedDate, 6, "%02d/%02d", stDateTime.stDate.ucDay, stDateTime.stDate.ucMonth);
			snprintf(s_stVaOpMsgItemData[i].strReceivedTime, 9, "%02d:%02d:%02d", stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond);
/*		strMailTxt(%s) ---- warning
			VaOpMsgPrint(DBG_PRINT, ("bRead(%d), nMailIdx(%d), strMailTxt(%s), strReceivedDate(%c%c%c%c%c), strReceivedTime(%c%c:%c%c:%c%c)\n",
							pMailItemData->bRead, pMailItemData->nMailIdx, pMailItemData->strMailTxt[MailInfo.msg_length],
							pMailItemData->strReceivedDate[0], pMailItemData->strReceivedDate[1], pMailItemData->strReceivedDate[2],
							pMailItemData->strReceivedDate[3], pMailItemData->strReceivedDate[4],
							pMailItemData->strReceivedTime[0], pMailItemData->strReceivedTime[1],
							pMailItemData->strReceivedTime[3], pMailItemData->strReceivedTime[4],
							pMailItemData->strReceivedTime[6], pMailItemData->strReceivedTime[7]));
*/

			hErr = UTIL_LL_AddItem(&(pstContents->stTextListBox), s_stVaOpMsgItemData[i].stOpMsg.szMsg);
			if(hErr != ERR_OK)
			{
				VaOpMsgAssert(0);
			}
		}
	}
}


STATIC CasVaList_Content_t	*local_cas_va_OpMsgList_GetContents(void)
{
	return &s_VaOpMsgContent;
}

STATIC BUS_Result_t	local_cas_va_OpMsgList_DrawHelp(OSD_Screen_t *screen, OSD_Area_t *area,
										OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1)
{
	HINT32				nStrW;
	OSD_Font_t			*pFont = OSD_FONT_Get(eFont_SystemNormal);
	OSD_StockImage_t	Img;
	HUINT8				*buf = GetTextBuffer(TEXT_SIZE64);
	OSD_Rect_t			stRect;

	OSD_FONT_SetSize(pFont, FONT_SIZE_S);

	// select
	if (!AP_StockImg_GetImage (e181_1_00_Help1_Red, &Img))
	{
		OSD_GFX_SetRect (&stRect, rect->x, rect->y, Img.width, Img.height);
		OSD_GFX_FB_Blend (Img.hBuffer, Img.width, Img.height, screen, &stRect);
	}

	VK_memset32(buf, 0, TEXT_SIZE64);
	MWC_UTIL_DvbStrcpy(buf, (HUINT8 *)GetStrRsc(LOC_CAS_VA_OP_MSG_SELECT_ID));
	OSD_GFX_SetRect(&stRect, rect->x + Img.width + VA_OPMSG_LIST_HELP_TXT_IMG_GAP, rect->y , 0, 0);
	OSD_GFX_DrawString(screen, pFont, stRect.x , stRect.y, (char*)buf, COL(C_T_Red_Key01));

	// select all
	nStrW = OSD_GFX_GetStringWidth(pFont, (char*)buf);
	OSD_GFX_SetRect(&stRect, stRect.x + nStrW + VA_OPMSG_LIST_HELE_ITEM_GAP, stRect.y, 0, 0);

	if (!AP_StockImg_GetImage (e181_1_00_Help2_Green, &Img))
	{
		OSD_GFX_FB_Blend (Img.hBuffer, Img.width, Img.height, screen, &stRect);
	}

	VK_memset32(buf, 0, TEXT_SIZE64);
	MWC_UTIL_DvbStrcpy(buf, (HUINT8 *)GetStrRsc(LOC_CAS_VA_OP_MSG_SELECT_ALL_ID));
	OSD_GFX_SetRect(&stRect, stRect.x + Img.width + VA_OPMSG_LIST_HELP_TXT_IMG_GAP, stRect.y, 0, 0);
	OSD_GFX_DrawString(screen, pFont, stRect.x , stRect.y, (char*)buf, COL(C_T_Green_Key01));

	// delete
	nStrW = OSD_GFX_GetStringWidth(pFont, (char*)buf);
	OSD_GFX_SetRect(&stRect, stRect.x + nStrW + VA_OPMSG_LIST_HELE_ITEM_GAP, stRect.y, 0, 0);

	if (!AP_StockImg_GetImage (e181_1_00_Help3_Yellow, &Img))
	{
		OSD_GFX_FB_Blend (Img.hBuffer, Img.width, Img.height, screen, &stRect);
	}

	VK_memset32(buf, 0, TEXT_SIZE64);
	MWC_UTIL_DvbStrcpy(buf, (HUINT8 *)GetStrRsc(LOC_CAS_VA_OP_MSG_DELETE_ID));
	OSD_GFX_SetRect(&stRect, stRect.x + Img.width + VA_OPMSG_LIST_HELP_TXT_IMG_GAP, stRect.y, 0, 0);
	OSD_GFX_DrawString(screen, pFont, stRect.x , stRect.y, (char*)buf, COL(C_T_Yellow_Key01));

	OSD_FONT_Release(pFont);

	return ERR_BUS_NO_ERROR;

}

BUS_Result_t	local_cas_va_OpMsgList_DrawAllItemList(OSD_Screen_t *screen, OSD_Area_t *area,
										OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1)
{
	int 				w;
	OSD_Font_t			*pFont = NULL;
	HUINT32 			objid = 0, i = 0;
	int 				totnum = 0;
	OSD_Color_t 		fontcolor;
	OSD_StockImage_t	leftImg, centImg, rightImg, Img;
	//OSD_Rect_t			Rect;
	int 				start=0, curr=0, select=0;
	GWM_GuiObject_t 	obj;
	HUINT8				*buf = GetTextBuffer(TEXT_SIZE64);
	HBOOL				bNeedScrollBar = FALSE;
	HINT32				lastIdx=0;
	HINT32				lAllPage = 0;
	HINT32				lScrollBall_height = 0, lScroll_y_offset = 0;
	OSD_StockImage_t	stImgTop, stImgMid, stImgBot;
	CasVaList_Content_t	*pstContents = local_cas_va_OpMsgList_GetContents();
	OSD_Rect_t			stRect;
	HINT32				nStockImgId;

	NOT_USED_PARAM(arg);
	NOT_USED_PARAM(param1);

	if(pstContents->ulNumOfSubItem == 0)
	{
		return ERR_OK;
	}

	VK_memset32(&Img, 0x00, sizeof(OSD_StockImage_t));
	VK_memset32(&rightImg, 0x00, sizeof(OSD_StockImage_t));
	VK_memset32(&centImg, 0x00, sizeof(OSD_StockImage_t));
	VK_memset32(&leftImg, 0x00, sizeof(OSD_StockImage_t));

	pFont = OSD_FONT_Get(eFont_SystemNormal);

	objid 	= GWM_Obj_GetObjectID(object);
	obj		= BUS_MGR_GetObject(objid);

	GWM_List_GetProperty((LIST_OBJECT)obj, &totnum, &start, &curr, &select);
	GWM_List_GetTotalItem(objid, &totnum);

	if (VA_CA_MAILLIST_DISPLAY_ITEM_NUM < totnum)
	{
		lastIdx = VA_CA_MAILLIST_DISPLAY_ITEM_NUM;
		bNeedScrollBar = TRUE;
	}
	else
	{
		lastIdx = totnum;
	}

	for(i=0; i < lastIdx; i++)
	{
		fontcolor = C_T_Enable;

		if(i + start == curr)
		{
			fontcolor = C_T_Cursor;

			if (!AP_StockImg_GetImage(e111_1_00_Cursor_S1_1L, &leftImg)
				&& !AP_StockImg_GetImage(e111_1_00_Cursor_S1_2C, &centImg)
				&& !AP_StockImg_GetImage(e111_1_00_Cursor_S1_3R, &rightImg))
			{
				OSD_GFX_BlendStockFrame(screen, rect->x + VA_OPMSG_LIST_CURSOR_X, rect->y + VA_OPMSG_LIST_CURSOR_Y + (i*MENU_MAINITEM_CURSOR_LINESPACE), rect->w + VA_OPMSG_LIST_CURSOR_W, rect->h,
								&leftImg, &centImg, &rightImg, 100);
			}
		}

		// Icon - check image
		if(i + start == curr)
		{
			if(s_stVaOpMsgItemData[start+i].bCheck == VA_CA_MAILLIST_CHECK_ON)
			{
				nStockImgId = e167_1_00_Check_B1_C_1On;
			}
			else
			{
				nStockImgId = e167_1_00_Check_B1_C_2Off;
			}
		}
		else
		{
			if(s_stVaOpMsgItemData[start+i].bCheck == VA_CA_MAILLIST_CHECK_ON)
			{
				nStockImgId = e167_1_00_Check_B1_N_1On;
			}
			else
			{
				nStockImgId = e167_1_00_Check_B1_N_2Off;
			}
		}

		// draw check image icon
		if (!AP_StockImg_GetImage (nStockImgId, &Img))
		{
			OSD_GFX_SetRect (&stRect, rect->x + VA_OPMSG_LIST_ITEM_X + VA_OPMSG_LIST_ITEM_IMG_GAP_X, rect->y + VA_OPMSG_LIST_ITEM_Y + VA_OPMSG_LIST_ITEM_IMG_GAP_Y + (i*MENU_MAINITEM_CURSOR_LINESPACE), Img.width, Img.height);
			OSD_GFX_FB_Blend (Img.hBuffer, Img.width, Img.height, screen, &stRect);
		}
		else
		{
			OSD_GFX_SetRect (&stRect, rect->x + VA_OPMSG_LIST_ITEM_X + VA_OPMSG_LIST_ITEM_IMG_GAP_X, rect->y + VA_OPMSG_LIST_ITEM_Y + VA_OPMSG_LIST_ITEM_IMG_GAP_Y + (i*MENU_MAINITEM_CURSOR_LINESPACE), Img.width, Img.height);
		}

		// Icon - check mail
		if(s_stVaOpMsgItemData[start+i].stOpMsg.ucRead)
		{
			nStockImgId = e470_1_46_Mail_Messages_03;
		}
		else
		{
			nStockImgId = e470_1_46_Mail_Messages_02;
		}

		// draw mail image icon
		if (!AP_StockImg_GetImage (nStockImgId, &Img))
		{
			OSD_GFX_SetRect (&stRect, stRect.x + stRect.w + VA_OPMSG_LIST_ITEM_IMG_GAP_W, stRect.y - VA_OPMSG_LIST_ITEM_IMG_GAP_Y - 5 , Img.width, Img.height);
			OSD_GFX_FB_Blend (Img.hBuffer, Img.width, Img.height, screen, &stRect);
		}
		else
		{
			OSD_GFX_SetRect (&stRect, stRect.x + stRect.w + VA_OPMSG_LIST_ITEM_IMG_GAP_W, stRect.y - VA_OPMSG_LIST_ITEM_IMG_GAP_Y - 5 , Img.width, Img.height);
		}

		//draw text
		if(ERR_OK == OSD_FONT_SetSize(pFont, FONT_SIZE_M))
		{
			// Received Date
			VK_memset32 (buf, 0, TEXT_SIZE64);
			MWC_UTIL_DvbStrcpy(buf, s_stVaOpMsgItemData[start+i].strReceivedDate);
			OSD_GFX_SetRect (&stRect, stRect.x +Img.width + VA_OPMSG_LIST_ITEM_DATE_GAP_W, stRect.y + 12, 0, 0);
			OSD_GFX_DrawString(screen, pFont, stRect.x , stRect.y, (char*)buf, COL(fontcolor));

			// Received Time
			w = OSD_GFX_GetStringWidth(pFont, (char*)buf);

			VK_memset32 (buf, 0, TEXT_SIZE64);
			MWC_UTIL_DvbStrcpy(buf, s_stVaOpMsgItemData[start+i].strReceivedTime);
			OSD_GFX_SetRect (&stRect, stRect.x +w + VA_OPMSG_LIST_ITEM_TIME_GAP_W, stRect.y, 0, 0);
			OSD_GFX_DrawString(screen, pFont, stRect.x , stRect.y, (char*)buf, COL(fontcolor));

			// Text
			w = OSD_GFX_GetStringWidth(pFont, (char*)buf);

			VK_memset32 (buf, 0, TEXT_SIZE64);
			MWC_UTIL_DvbStrcpy(buf, s_stVaOpMsgItemData[start+i].stOpMsg.szMsg);
			OSD_GFX_SetRect (&stRect, stRect.x + w + VA_OPMSG_LIST_ITEM_TEXT_GAP_W, stRect.y, 0, 0);
			OSD_GFX_DrawStringWithAbbr(screen, pFont, stRect.x, stRect.y, VA_OPMSG_LIST_ITEM_TEXT_W, (char*)buf, COL(fontcolor));

		}
	}

	OSD_FONT_Release(pFont);

	if (bNeedScrollBar == TRUE)
	{
		HINT32	nScrollBall_step = 0, nHeight = 0, tmpTotal = totnum, dividedStep = 0;

		lAllPage = (totnum / VA_CA_MAILLIST_DISPLAY_ITEM_NUM);
		lAllPage += (totnum % VA_CA_MAILLIST_DISPLAY_ITEM_NUM) ? 1 : 0;

		lScrollBall_height = VA_OPMSG_LIST_ITEM_SCROLL_H / lAllPage;
		if (lScrollBall_height < CAS_VA_SCROLLBALL_H)
		{
			lScrollBall_height = CAS_VA_SCROLLBALL_H;
		}

		nHeight = (VA_OPMSG_LIST_ITEM_SCROLL_H - lScrollBall_height);
		for (i = 2; tmpTotal > nHeight; i++)
		{
			tmpTotal = totnum / i;
		} //while (ulTotal > (460 - lScrollBall_height));
		dividedStep = i - 1;

		nScrollBall_step = nHeight / (tmpTotal-1);
		lScroll_y_offset = (curr / dividedStep) * nScrollBall_step;
		lScrollBall_height += (nHeight - ((tmpTotal-1) * nScrollBall_step));

		OSD_GFX_SetRect (&stRect, rect->x + rect->w - 30, rect->y + 25, 0, 0);

		// BG
		if ((!AP_StockImg_GetImage (e166_1_00_Scroll_B1_1T, &stImgTop))
			&& (!AP_StockImg_GetImage (e166_1_00_Scroll_B1_2M, &stImgMid))
			&& (!AP_StockImg_GetImage (e166_1_00_Scroll_B1_3B, &stImgBot)))
		{
			OSD_GFX_BlendStockVFrame (screen, stRect.x, stRect.y + lScroll_y_offset, stRect.x + stImgTop.width - 1, stRect.y + lScroll_y_offset + lScrollBall_height,
									&stImgTop, &stImgMid, &stImgBot, 100);
		}

	}

	return ERR_OK;
}


STATIC void 				local_cas_va_OpMsgList_DrawData(CasVaList_Content_t *pstContents)
{
	OSD_Rect_t				stRect = {0, };
	HINT32					objid = eOpMsg_List_ITEM;

	/* set, draw background */
	Settings_DrawBack(FALSE);

	/* title */
	MWC_UTIL_DvbStrcpy(pstContents->ucTitle, GetStrRsc(LOC_CAS_VA_MSG_OPMESSAGE_ID));
	Settings_SetWindowTitle(SETTING_GET_AP_DEPTH(eSetting_ConditionalAccess), LOC_CAS_VA_MSG_OPMESSAGE_ID, pstContents->ucTitle);


	// BOX
	OSD_GFX_SetRect(&stRect, VA_OPMSG_LIST_BOX_X, VA_OPMSG_LIST_BOX_Y, VA_OPMSG_LIST_BOX_W, VA_OPMSG_LIST_BOX_H);
	GWM_Rect_Create(eOpMsg_List_BOX_ID, &stRect, VA_OPMSG_LIST_BOX_CREATE_COL);
	GWM_APP_SetObjectDrawMethod(eOpMsg_List_BOX_ID, AP_LNF_DrawBoxD);

	if((pstContents->ulNumOfSubItem > 0))
	{
		// item
		GWM_List_Create(objid, &stRect, "mail msg list", MENU_MAINITEM_CURSOR_LINESPACE, pstContents->ulNumOfSubItem, VA_CA_MAILLIST_DISPLAY_ITEM_NUM, (GWM_LIST_ITEMS)NULL);
		GWM_APP_SetObjectDrawMethod(objid, local_cas_va_OpMsgList_DrawAllItemList);
		GWM_Obj_SetFocus (objid);

		//help
		OSD_GFX_SetRect(&stRect, VA_OPMSG_LIST_HELP_X, VA_OPMSG_LIST_HELP_Y, VA_OPMSG_LIST_HELP_W, VA_OPMSG_LIST_HELP_H);
		GWM_Rect_Create(eOpMsg_List_HELP_ID, &stRect, VA_OPMSG_LIST_BOX_CREATE_COL);
		GWM_APP_SetObjectDrawMethod(eOpMsg_List_HELP_ID, local_cas_va_OpMsgList_DrawHelp);

	}
}


/******************************************************************************/
/**
*   \fn     local_cas_va_OpMsgList_DestroyUI(SettingsNaviContext_t *pstContents)
*   \brief  main/sub items memory free
*
*   @param[in]  pstContents
*   @return     ERR_OK
*   @ingroup    SETTINGS
*/
/******************************************************************************/
STATIC HERROR		local_cas_va_OpMsgList_DestroyUI(CasVaList_Content_t *pstContents)
{
	/* if dynamic allocated data are, free */
	//AP_NaviCntxt_FreeMainItemArray(pstContents);

	return ERR_OK;
}

STATIC HERROR	local_cas_va_OpMsgList_IsAllCheckMail(void)
{
	HUINT32		i;

	for(i = 0; i < s_VaOpMsgContent.ulNumOfSubItem; i++)
	{
		if(s_stVaOpMsgItemData[i].bCheck == VA_CA_MAILLIST_CHECK_OFF)
		{
			return ERR_FAIL;
		}
	}

	return ERR_OK;
}

STATIC HERROR	local_cas_va_OpMsgList_ExistCheckMail(void)
{
	HUINT32		i;

	for(i = 0; i < s_VaOpMsgContent.ulNumOfSubItem; i++)
	{
		if(s_stVaOpMsgItemData[i].bCheck == VA_CA_MAILLIST_CHECK_ON)
		{
			return ERR_OK;
		}
	}

	return ERR_FAIL;
}

STATIC HERROR	local_cas_va_OpMsgList_SelectAllCheckMail(HBOOL bAllCheck)
{
	HUINT32		i;

	if(bAllCheck == VA_CA_MAILLIST_CHECK_ALL_TRUE)
	{
		for(i = 0; i < s_VaOpMsgContent.ulNumOfSubItem; i++)
		{
			s_stVaOpMsgItemData[i].bCheck = VA_CA_MAILLIST_CHECK_ON;
		}
	}
	else
	{
		for(i = 0; i < s_VaOpMsgContent.ulNumOfSubItem; i++)
		{
			s_stVaOpMsgItemData[i].bCheck = VA_CA_MAILLIST_CHECK_OFF;
		}
	}

	return ERR_OK;
}


STATIC HUINT32	local_cas_va_OpMsgList_DelCheckMail(void)
{
	HUINT32		i, ulDelCount = 0;

	for(i = s_VaOpMsgContent.ulNumOfSubItem; i > 0; i--)
	{
		if(s_stVaOpMsgItemData[i - 1].bCheck == VA_CA_MAILLIST_CHECK_ON)
		{
			VA_UI_Adapt_DeleteOpMessage(i - 1);
			ulDelCount++;
		}
	}

	return	ulDelCount;
}


STATIC HERROR	local_cas_va_OpMsgList_ColorKeyProc(HINT32 nKey, CasVaList_Content_t *pContext)
{
	HBOOL	hRes;


	if(pContext == NULL)
	{
		return ERR_FAIL;
	}

	switch(nKey)
	{
		case KEY_RED:
			if(s_stVaOpMsgItemData[s_VaOpMsgCurFocus].bCheck == VA_CA_MAILLIST_CHECK_OFF)
			{
				 s_stVaOpMsgItemData[s_VaOpMsgCurFocus].bCheck = VA_CA_MAILLIST_CHECK_ON;
			}
			else
			{
				s_stVaOpMsgItemData[s_VaOpMsgCurFocus].bCheck = VA_CA_MAILLIST_CHECK_OFF;
			}

			if( pContext->ulCurFocus == pContext->ulNumOfSubItem - 1 )
			{
				pContext->ulCurFocus = 0;
			}
			else
			{
				pContext->ulCurFocus ++;
			}

			GWM_List_SetCurrentIndex(eOpMsg_List_ITEM, pContext->ulCurFocus);
			s_VaOpMsgCurFocus = pContext->ulCurFocus;
			GWM_APP_InvalidateRect(VA_OPMSG_LIST_APP_WINDOW_X, VA_OPMSG_LIST_APP_WINDOW_Y,
									VA_OPMSG_LIST_APP_WINDOW_WIDTH, VA_OPMSG_LIST_APP_WINDOW_HEIGHT);
			break;

		case KEY_GREEN:
			hRes = local_cas_va_OpMsgList_IsAllCheckMail();

			if(hRes == ERR_OK)
			{
				local_cas_va_OpMsgList_SelectAllCheckMail(VA_CA_MAILLIST_CHECK_ALL_FALSE);
			}
			else
			{
				local_cas_va_OpMsgList_SelectAllCheckMail(VA_CA_MAILLIST_CHECK_ALL_TRUE);
			}

			GWM_APP_InvalidateRect(VA_OPMSG_LIST_APP_WINDOW_X, VA_OPMSG_LIST_APP_WINDOW_Y,
									VA_OPMSG_LIST_APP_WINDOW_WIDTH, VA_OPMSG_LIST_APP_WINDOW_HEIGHT);

			break;

		case KEY_YELLOW:
		{
			HUINT32			ulDelCount = 0;
			GWM_GuiObject_t	object;

			if (pContext->ulNumOfSubItem > 0 )
			{
				hRes = local_cas_va_OpMsgList_ExistCheckMail();

				if (hRes == ERR_OK)
				{
					ulDelCount = local_cas_va_OpMsgList_DelCheckMail();
				}
				else
				{
					VA_UI_Adapt_DeleteOpMessage(s_VaOpMsgCurFocus);
					ulDelCount = 1;
				}

				local_cas_va_OpMsgList_LoadMailList(pContext);

				if(s_VaOpMsgCurFocus != 0 && s_VaOpMsgCurFocus >= pContext->ulNumOfSubItem)
				{
					s_VaOpMsgCurFocus -= ulDelCount;
					pContext->ulCurFocus = s_VaOpMsgCurFocus;
					GWM_List_SetCurrentIndex(eOpMsg_List_ITEM, pContext->ulCurFocus);
				}

				if (pContext->ulNumOfSubItem == 0)
				{
					if (BUS_MGR_Get(AP_CAS_VA_OpMessageIcon_Proc) != NULL)
					{
						BUS_MGR_Destroy(AP_CAS_VA_OpMessageIcon_Proc);
					}
				}

				object = BUS_MGR_GetObject(eOpMsg_List_ITEM);
				GWM_List_ChangeItems((LIST_OBJECT)object, pContext->ulNumOfSubItem, (GWM_LIST_ITEMS)NULL);
				GWM_APP_InvalidateRect(VA_OPMSG_LIST_APP_WINDOW_X, VA_OPMSG_LIST_APP_WINDOW_Y,
										VA_OPMSG_LIST_APP_WINDOW_WIDTH, VA_OPMSG_LIST_APP_WINDOW_HEIGHT);
			}
		}
			break;
		default:
			break;
	}

	return ERR_OK;
}


/******************************************************************************/
/**
*   \fn     AP_CAS_VA_OpMessageList_Proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  settings procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SETRTINGS
*/
/******************************************************************************/
BUS_Result_t		AP_CAS_VA_OpMessageList_Proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32				errResult = ERR_FAIL;
	HUINT8				nCnt = 0;
	CasVaList_Content_t	*pContext = NULL;
	GWM_GuiObject_t		object;

	VaOpMsgPrint(DBG_TRACE, ("AP_CAS_VA_OpMessageList_Proc() message(0x%x)\n", message));

	/* get running param */
	pContext = local_cas_va_OpMsgList_GetContents();

	switch(message)
	{
		case MSG_GWM_CREATE:

			/* Mail List 진입시 Normal Mail Received Icon 은 삭제한다. */
			if(BUS_MGR_Get(AP_CAS_VA_OpMessageIcon_Proc) != NULL)
			{
				BUS_MGR_Destroy(AP_CAS_VA_OpMessageIcon_Proc);
			}

			/* init application area */
			GWM_APP_SetAppArea(SYSETM_APP_WINDOW_X, SYSETM_APP_WINDOW_Y, SYSETM_APP_WINDOW_WIDTH, SYSETM_APP_WINDOW_HEIGHT);

			// update cas information
			local_cas_va_OpMsgList_LoadMailList(pContext);
			// draw data
			local_cas_va_OpMsgList_DrawData(pContext);

			pContext->ulCurFocus = s_VaOpMsgCurFocus;

			GWM_List_SetCurrentIndex(eOpMsg_List_ITEM, pContext->ulCurFocus);

			BUS_SetTimer(MENU_TIMEOUT_ID, MENU_TIMEOUT);

			/* set front pannel */
			//AP_SPEC_DisplayMenuPanelString(pContext->ucTitle, eDxPANEL_ALIGN_CENTER);
			break;

		case MSG_GWM_KEYUP:
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN:
			/* settings이 전반적으로 처리해야 할 키들 */
			AP_KeyConv_UiCommon(&p1);

			GWM_ResetTimer(MENU_TIMEOUT_ID);

			errResult = Settings_CheckKeyPass(p1, BUS_MGR_GetAppCallback(BUS_MGR_GetThis()));
			if(errResult != ERR_OK)
			{
				return errResult;
			}

			switch(p1)
			{
				case KEY_EXIT:
				case KEY_MENU:
					/* reset Title depth */
					Settings_ClearWindowTitle();
					s_VaOpMsgCurFocus=0;

					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				case KEY_ARROWLEFT:
				case KEY_BACK:
					/* 상위 메뉴로 올라갈 때 focus를 기억하지 않는 것이 편할 수도 있다. */
					//Settings_InitFocus(pstContents);

					BUS_MGR_Destroy(NULL);
					s_VaOpMsgCurFocus=0;

					/* to upper depth */
					BUS_MGR_Create("AP_System_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (GWM_Callback_t)AP_System_Proc, 0, 0, 0, 0);
					return MESSAGE_BREAK;

				case KEY_PAGEUP:
				case KEY_ARROWUP:
					if( pContext->ulCurFocus == 0 )
					{
						pContext->ulCurFocus = pContext->ulNumOfSubItem - 1;
					}
					else
					{
						pContext->ulCurFocus --;
					}
					GWM_List_SetCurrentIndex(eOpMsg_List_ITEM, pContext->ulCurFocus);
					s_VaOpMsgCurFocus = pContext->ulCurFocus;
					GWM_APP_InvalidateRect(VA_OPMSG_LIST_APP_WINDOW_X, VA_OPMSG_LIST_APP_WINDOW_Y,
											VA_OPMSG_LIST_APP_WINDOW_WIDTH, VA_OPMSG_LIST_APP_WINDOW_HEIGHT);
					return MESSAGE_BREAK;

				case KEY_PAGEDOWN:
				case KEY_ARROWDOWN:
					if( pContext->ulCurFocus == pContext->ulNumOfSubItem - 1 )
					{
						pContext->ulCurFocus = 0;
					}
					else
					{
						pContext->ulCurFocus ++;
					}
					GWM_List_SetCurrentIndex(eOpMsg_List_ITEM, pContext->ulCurFocus);
					s_VaOpMsgCurFocus = pContext->ulCurFocus;
					GWM_APP_InvalidateRect(VA_OPMSG_LIST_APP_WINDOW_X, VA_OPMSG_LIST_APP_WINDOW_Y,
											VA_OPMSG_LIST_APP_WINDOW_WIDTH, VA_OPMSG_LIST_APP_WINDOW_HEIGHT);
					return MESSAGE_BREAK;

				case KEY_ARROWRIGHT:
				case KEY_OK:
					if (pContext->ulNumOfSubItem == 0)
					{
						VaOpMsgPrint(DBG_TRACE, ("There is no saved mail\n"));
						return MESSAGE_BREAK;
					}
					BUS_MGR_Destroy(NULL);

					s_stVaOpMsgItemData[s_VaOpMsgCurFocus].stOpMsg.ucRead = TRUE;
					VA_UI_Adapt_UpdateOpMessage(&s_stVaOpMsgItemData[s_VaOpMsgCurFocus].stOpMsg, s_VaOpMsgCurFocus);

					BUS_MGR_Create("AP_CAS_VA_OpMsgListText_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (GWM_Callback_t)AP_CAS_VA_OpMsgListText_Proc, 0, 0, s_VaOpMsgCurFocus, 0);
					return MESSAGE_BREAK;
				case KEY_RED:
				case KEY_GREEN:
				case KEY_YELLOW:
					local_cas_va_OpMsgList_ColorKeyProc(p1, pContext);
					break;
				default:
					AP_NotAvailable_Show();
					return MESSAGE_BREAK;
			}
			break;

		case MSG_GWM_TIMER:
			if(p1 == MENU_TIMEOUT_ID)
			{
				/* reset Title depth */
				Settings_ClearWindowTitle();

				BUS_MGR_Destroy(0);
			}
			return MESSAGE_BREAK;

		case XMGR_CAS_UPDATE_MENU:
			local_cas_va_OpMsgList_LoadMailList(pContext);
			object = BUS_MGR_GetObject(eOpMsg_List_ITEM);

			GWM_List_ChangeItems((LIST_OBJECT)object, pContext->ulNumOfSubItem, (GWM_LIST_ITEMS)NULL);
			GWM_List_SetCurrentIndex(eOpMsg_List_ITEM, pContext->ulCurFocus);
			GWM_APP_InvalidateRect(VA_OPMSG_LIST_APP_WINDOW_X, VA_OPMSG_LIST_APP_WINDOW_Y,
										VA_OPMSG_LIST_APP_WINDOW_WIDTH, VA_OPMSG_LIST_APP_WINDOW_HEIGHT);
			break;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			local_cas_va_OpMsgList_DestroyUI(pContext);

			/* kill timer */
			BUS_KillTimer(MENU_TIMEOUT_ID);

			/* clear front PANEL */
			//AP_SPEC_ClearMenuPanelString();
			break;

		default:
			break;
	}

	VaOpMsgPrint(DBG_TRACE, ("AP_CAS_VA_OpMessageList_Proc() handle(0x%x)\n", hAction));

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


#define _________________________NORMAL_MAIL_TEXT______________________________

STATIC inline SettingsNaviContext_t *local_cas_va_OpMsg_GetContentsText(void)
{
	return &s_VaOpMsgTextContent;
}

STATIC BUS_Result_t	local_cas_va_OpMsgList_text_DrawItems(int lId, OSD_Screen_t *pScreen, FMT_DrawItem_t *pDrawItem)
{
	OSD_Rect_t				stItemRect;
	OSD_Font_t				*pFont = OSD_FONT_Get(eFont_SystemNormal);
	TEXT_DRAW_INFO_t		stTextInfo;

	stTextInfo.ulTextColor = pDrawItem->stTextProperty.cFontColor;
	stTextInfo.ulFontSize = pDrawItem->stTextProperty.ulFontSize;
	stTextInfo.ulDrawMode = TEXTMODE_FORCE_FIT;
	stTextInfo.ulXAlign = TEXT_ALIGN_LEFT;
	stTextInfo.ulYAlign = TEXT_ALIGN_MIDDLE;
	OSD_GFX_CopyRect(&stItemRect, &(pDrawItem->itemRect));

	AP_UTIL_DrawFormattedText(pScreen, pFont, &stItemRect, &stTextInfo, (HUINT8*)pDrawItem->str);

	OSD_FONT_Release (pFont);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t	local_cas_va_OpMsgList_text_DrawScroll(	OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, FMT_OBJECT pObject)
{
	HINT32					lX = 0;
	HINT32					lY = 0;
	HINT32					lH = 0;
	HINT32					lTotalPage = 0;
	HINT32					lIndex = 0;
	HUINT32					ulAttrib = 0;
	HUINT32					ulStartIndex = 0;
	HUINT32					ulLinePerPage = 0;
	HUINT32					ulItemNum = 0;
	HUINT32					ulThumbHeight = 0;
	HUINT32					ulThumbY = 0;
	HINT32					lBgHeight = 0;
	OSD_StockImage_t		stThumbHeadImg = {0, };
	OSD_StockImage_t		stThumbBodyImg = {0, };
	OSD_StockImage_t		stThumbTailImg = {0, };
	OSD_StockImage_t		stBgImg = {0, };

	GWM_FMT_GetProperty(pObject, &ulAttrib, &ulStartIndex, &ulLinePerPage, &ulItemNum);
	if (ulItemNum <= ulLinePerPage)
	{
		return ERR_BUS_NO_ERROR;
	}

	if(AP_StockImg_GetImage(e166_2_00_Scroll_W1_1T, &stThumbHeadImg))
	{
		return ERR_BUS_NO_ERROR;
	}
	if(AP_StockImg_GetImage(e166_2_00_Scroll_W1_2M, &stThumbBodyImg))
	{
		return ERR_BUS_NO_ERROR;
	}
	if(AP_StockImg_GetImage(e166_2_00_Scroll_W1_3B, &stThumbTailImg))
	{
		return ERR_BUS_NO_ERROR;
	}
	if(AP_StockImg_GetImage(e166_2_00_Scroll_W1_4BG, &stBgImg))
	{
		return ERR_BUS_NO_ERROR;
	}

	lBgHeight = stBgImg.height;

	lX = pRect->x + pRect->w - VA_OPMSG_LIST_SCROLL_GAP_W;
	lY = pRect->y + VA_OPMSG_LIST_SCROLL_GAP_Y;
	lH = pRect->h - VA_OPMSG_LIST_SCROLL_GAP_H;

	// support line move & page move
	if (ulItemNum < ulLinePerPage)
	{
		lTotalPage = 1;
	}
	else
	{
		lTotalPage = (ulItemNum/ulLinePerPage) + (ulItemNum%ulLinePerPage ? 1 : 0);
	}


	ulThumbHeight = (HUINT32)(lH / lTotalPage);
	if (ulThumbHeight < 20)
	{
		ulThumbHeight = 20;
	}
	else if (ulThumbHeight > (HUINT32)(lH / 2))
	{
		ulThumbHeight = (HUINT32)(lH / 2);
	}

	if (ulStartIndex > 0)
	{
		if(ulItemNum > ulStartIndex + ulLinePerPage)
		{
			ulThumbY = lY + ( (lH / ulItemNum) * ulStartIndex);
		}
		else
		{
			ulThumbY = lY + (lH - ulThumbHeight);
		}
	}
	else
	{
		ulThumbY = lY;
	}

	lIndex = lY;
	while(lIndex < lY + lH)
	{

		if (lIndex != ulThumbY)
		{
			lIndex += lBgHeight;
		}
		else
		{
			// draw thumb

			OSD_GFX_BlendStockVFrame (pScreen, lX, lIndex, lX + stThumbHeadImg.width, lIndex + ulThumbHeight,
									&stThumbHeadImg, &stThumbBodyImg, &stThumbTailImg, 100);
			lIndex += ulThumbHeight;
		}
	}

	return ERR_BUS_NO_ERROR;

}

STATIC BUS_Result_t	local_cas_va_OpMsgList_text_DrawTitle(OSD_Screen_t *screen, OSD_Area_t *area,
										OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1)
{
	OSD_Font_t			*pFont = NULL;
	OSD_StockImage_t	Img;
	OSD_Rect_t			Rect;
	HUINT8				*buf = GetTextBuffer(TEXT_SIZE512);
	HUINT8				ucCnt = 0;
	HINT32				nImageID;
	HERROR				hError = ERR_FAIL;
	SettingsNaviContext_t	*pstContents = NULL;


	pstContents = local_cas_va_OpMsg_GetContentsText();

	if (pstContents->ulNumOfMainItem < 1)
	{
		return	ERR_INITIAL_FAILED;
	}

	if (!AP_StockImg_GetImage (e470_1_46_Mail_Messages_03, &Img))
	{
		OSD_GFX_SetRect (&Rect, rect->x , rect->y, Img.width, Img.height);
		OSD_GFX_FB_Blend (Img.hBuffer, Img.width, Img.height, screen, &Rect);
	}

	pFont = OSD_FONT_Get(eFont_SystemNormal);

	if(OSD_FONT_SetSize(pFont, FONT_SIZE_M) == ERR_OK)
	{
		// Date
		HxSTD_memset (buf, 0, TEXT_SIZE512);
		MWC_UTIL_DvbStrcpy(buf, s_stVaOpMsgItemData[pstContents->nCurFocus].strReceivedDate);
		OSD_GFX_SetRect(&Rect, rect->x + Img.width + VA_OPMSG_LIST_TITLE_DATE_GAP_W, rect->y + VA_OPMSG_LIST_TITLE_DATE_GAP_H, 0, 0);
		OSD_GFX_DrawString(screen, pFont, Rect.x, Rect.y, (char*)buf, COL(C_T_Cursor));

		// Time
		Rect.w = OSD_GFX_GetStringWidth(pFont, (char*)buf);

		HxSTD_memset (buf, 0, TEXT_SIZE512);
		MWC_UTIL_DvbStrcpy(buf, s_stVaOpMsgItemData[pstContents->nCurFocus].strReceivedTime);
		OSD_GFX_SetRect(&Rect, Rect.x + Rect.w + VA_OPMSG_LIST_TITLE_TIME_GAP_W, rect->y + VA_OPMSG_LIST_TITLE_DATE_GAP_Y, 0, 0);
		OSD_GFX_DrawString(screen, pFont, Rect.x, Rect.y, (char*)buf, COL(C_T_Cursor));

	}

	OSD_FONT_Release(pFont);

	return ERR_BUS_NO_ERROR;

}

STATIC BUS_Result_t	local_cas_va_OpMsgList_text_DrawHelp(OSD_Screen_t *screen, OSD_Area_t *area,
										OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1)
{
	HINT32				nStrW;
	OSD_Font_t			*pFont = OSD_FONT_Get(eFont_SystemNormal);
	OSD_StockImage_t	Img;
	HUINT8				*buf = GetTextBuffer(TEXT_SIZE64);
	OSD_Rect_t			stRect;

	OSD_FONT_SetSize(pFont, FONT_SIZE_S);

	// select
	if (!AP_StockImg_GetImage (e182_1_00_Help1_Red_D, &Img))
	{
		OSD_GFX_SetRect (&stRect, rect->x, rect->y, Img.width, Img.height);
		OSD_GFX_FB_Blend (Img.hBuffer, Img.width, Img.height, screen, &stRect);
	}

	VK_memset32(buf, 0, TEXT_SIZE64);
	MWC_UTIL_DvbStrcpy(buf, (HUINT8 *)GetStrRsc(LOC_CAS_VA_OP_MSG_SELECT_ID));
	OSD_GFX_SetRect(&stRect, rect->x + Img.width + VA_OPMSG_LIST_HELP_TXT_IMG_GAP, rect->y , 0, 0);
	OSD_GFX_DrawString(screen, pFont, stRect.x , stRect.y, (char*)buf, COL(C_T_Disable));

	// select all
	nStrW = OSD_GFX_GetStringWidth(pFont, (char*)buf);
	OSD_GFX_SetRect(&stRect, stRect.x + nStrW + VA_OPMSG_LIST_HELE_ITEM_GAP, stRect.y, 0, 0);

	if (!AP_StockImg_GetImage (e182_1_00_Help2_Green_D, &Img))
	{
		OSD_GFX_FB_Blend (Img.hBuffer, Img.width, Img.height, screen, &stRect);
	}

	VK_memset32(buf, 0, TEXT_SIZE64);
	MWC_UTIL_DvbStrcpy(buf, (HUINT8 *)GetStrRsc(LOC_CAS_VA_OP_MSG_SELECT_ALL_ID));
	OSD_GFX_SetRect(&stRect, stRect.x + Img.width + VA_OPMSG_LIST_HELP_TXT_IMG_GAP, stRect.y, 0, 0);
	OSD_GFX_DrawString(screen, pFont, stRect.x , stRect.y, (char*)buf, COL(C_T_Disable));

	// delete
	nStrW = OSD_GFX_GetStringWidth(pFont, (char*)buf);
	OSD_GFX_SetRect(&stRect, stRect.x + nStrW + VA_OPMSG_LIST_HELE_ITEM_GAP, stRect.y, 0, 0);

	if (!AP_StockImg_GetImage (e181_1_00_Help3_Yellow, &Img))
	{
		OSD_GFX_FB_Blend (Img.hBuffer, Img.width, Img.height, screen, &stRect);
	}


	VK_memset32(buf, 0, TEXT_SIZE64);
	MWC_UTIL_DvbStrcpy(buf, (HUINT8 *)GetStrRsc(LOC_CAS_VA_OP_MSG_DELETE_ID));
	OSD_GFX_SetRect(&stRect, stRect.x + Img.width + VA_OPMSG_LIST_HELP_TXT_IMG_GAP, stRect.y, 0, 0);
	OSD_GFX_DrawString(screen, pFont, stRect.x , stRect.y, (char*)buf, COL(C_T_Yellow_Key01));

	OSD_FONT_Release(pFont);

	return ERR_BUS_NO_ERROR;

}

STATIC void	local_cas_va_OpMsgList_text_DrawText(SettingsNaviContext_t *pstContents)
{

	HUINT8					*pBuf1 = GetTextBuffer(TEXT_SIZE512);
	HUINT8					*pBuf2 = GetTextBuffer(TEXT_SIZE128);
	HUINT32					nNumOfChForOneLine;
	OSD_Font_t				*pFont = OSD_FONT_Get(eFont_SystemNormal);
	OSD_Rect_t				stRect = {0, };
	eHxCP_CodePage_e			encoding;
	Text_Property_t 		stTextProperty;
	HUINT8				ucCnt = 0;
	HERROR				hError = ERR_FAIL;

	// set, draw background
	Settings_DrawBack(FALSE);

	// title
	MWC_UTIL_DvbStrcpy(pstContents->title, GetStrRsc(LOC_CAS_VA_MSG_OPMESSAGE_ID));
	Settings_SetWindowTitle(SETTING_GET_AP_DEPTH(eSetting_ConditionalAccess), LOC_CAS_VA_MSG_OPMESSAGE_ID, pstContents->title);

	// FTM Text Box
	if((pstContents->ulNumOfMainItem > 0))
	{
		//draw icon & date & time
		OSD_GFX_SetRect (&stRect, VA_OPMSG_LIST_TITLE_X, VA_OPMSG_LIST_TITLE_Y, VA_OPMSG_LIST_TITLE_W, VA_OPMSG_LIST_TITLE_H);
		GWM_Frame_Create (eOpMsg_List_ICON_ID, &stRect, (char *)NULL);
		GWM_APP_SetObjectDrawMethod (eOpMsg_List_ICON_ID, local_cas_va_OpMsgList_text_DrawTitle);

		// BOX
		OSD_GFX_SetRect(&stRect, VA_OPMSG_LIST_BOX_X, VA_OPMSG_LIST_BOX_Y, VA_OPMSG_LIST_BOX_W, VA_OPMSG_LIST_BOX_H);
		GWM_Rect_Create(eOpMsg_List_BOX_ID, &stRect, VA_OPMSG_LIST_BOX_CREATE_COL);
		GWM_APP_SetObjectDrawMethod(eOpMsg_List_BOX_ID, AP_LNF_DrawBoxD);


		// FMT Text
		OSD_GFX_SetRect(&stRect, VA_OPMSG_LIST_FMT_X, VA_OPMSG_LIST_FMT_Y, VA_OPMSG_LIST_FMT_W, VA_OPMSG_LIST_FMT_H);
#if 1
		GWM_Text_Create(eOpMsg_List_FMT_ID, &stRect, s_stVaOpMsgItemData[pstContents->nCurFocus].stOpMsg.szMsg);
		GWM_Text_SetSize(eOpMsg_List_FMT_ID, FONT_SIZE_M);
		GWM_Text_ChangeColor(eOpMsg_List_FMT_ID, COL(C_T_Enable), 0);
		GWM_APP_SetObjectDrawMethod(eOpMsg_List_FMT_ID, AP_LNF_PlainMultilineText_Draw);
#else
		GWM_FMT_Create(eOpMsg_List_FMT_ID, &stRect, FMT_OWN_DRAWITEM | FMT_SCROLL_ON, 12);


		stTextProperty.ulFontSize	= FONT_SIZE_S;
		stTextProperty.ulFontAlign	= TEXT_ALIGN_LEFT;
		stTextProperty.cFontColor	= COL(C_T_Enable);


		if (pstContents->ulNumOfMainItem < 1)
		{
			return;
		}

		HxSTD_memset(pBuf1, 0, TEXT_SIZE512);
		snprintf(pBuf1,TEXT_SIZE512,  "%s",  s_stVaOpMsgItemData[pstContents->nCurFocus].stOpMsg.szMsg);
		encoding= MWC_UTIL_GetEncodingByLocaleCode((const HUINT8 *)pBuf1);

		do
		{
			HxSTD_memset(pBuf2, 0, TEXT_SIZE128);

			OSD_FONT_CountChByWidth_SingleByte( pFont, stRect.w, encoding, strlen(pBuf1), pBuf1, &nNumOfChForOneLine);
			strncpy(pBuf2, pBuf1, nNumOfChForOneLine);

			pBuf1 += nNumOfChForOneLine;

			GWM_FMT_AddItem(eOpMsg_List_FMT_ID, (char *)pBuf2, &stTextProperty);

		} while(nNumOfChForOneLine != 0);

		GWM_FMT_ChangeItemDrawMethod(eOpMsg_List_FMT_ID, local_cas_va_OpMsgList_text_DrawItems);
		GWM_FMT_ChangeScrollDrawMethod(eOpMsg_List_FMT_ID, local_cas_va_OpMsgList_text_DrawScroll);
#endif

		GWM_Obj_SetFocus(eOpMsg_List_FMT_ID);

	}

	OSD_GFX_SetRect(&stRect, VA_OPMSG_LIST_HELP_X, VA_OPMSG_LIST_HELP_Y, VA_OPMSG_LIST_HELP_W, VA_OPMSG_LIST_HELP_H);
	GWM_Rect_Create(eOpMsg_List_HELP_ID, &stRect, VA_OPMSG_LIST_BOX_CREATE_COL);
	GWM_APP_SetObjectDrawMethod(eOpMsg_List_HELP_ID, local_cas_va_OpMsgList_text_DrawHelp);

	OSD_FONT_Release(pFont);

}

STATIC HERROR		local_cas_va_OpMsgList_text_DestroyUIText(SettingsNaviContext_t *pstContents)
{
	HUINT8	i = 0;

	/* if dynamic allocated data are, free */
	AP_NaviCntxt_FreeMainItemArray(pstContents);

	return ERR_OK;
}


/******************************************************************************/
/**
*	\fn 	AP_CAS_VA_OpMsgText_Proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*	\brief	settings procedure
*
*	@param[in]	gwm msg/action handler
*	@return 	BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*	@ingroup	SETRTINGS
*/
/******************************************************************************/
BUS_Result_t		AP_CAS_VA_OpMsgListText_Proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 				errResult = ERR_FAIL;
	SettingsNaviContext_t	*pNaviContext = NULL;
	HUINT8					nCnt;

	VaOpMsgPrint(DBG_TRACE, ("AP_CAS_VA_OpMsgListText_Proc() message(0x%x)\n", message));

	/* get running param */
	pNaviContext = local_cas_va_OpMsg_GetContentsText();

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* init application area */
			GWM_APP_SetAppArea(SYSETM_APP_WINDOW_X, SYSETM_APP_WINDOW_Y, SYSETM_APP_WINDOW_WIDTH, SYSETM_APP_WINDOW_HEIGHT);

			pNaviContext->nCurFocus = p2;

			// update cas information
			//local_cas_va_OpMsgList_text_LoadMailText(pNaviContext);
			pNaviContext->ulNumOfMainItem = VA_UI_Adapt_GetOpMessageCount();
			// draw data
			if(pNaviContext->ulNumOfMainItem != 0)
			{
				local_cas_va_OpMsgList_text_DrawText(pNaviContext);
			}

			/* set front pannel */
			//AP_SPEC_DisplayMenuPanelString(pNaviContext->title, eDxPANEL_ALIGN_CENTER);
			break;

		case MSG_GWM_KEYUP:
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN:
			/* settings이 전반적으로 처리해야 할 키들 */
			AP_KeyConv_UiCommon(&p1);

			GWM_ResetTimer(MENU_TIMEOUT_ID);

			errResult = Settings_CheckKeyPass(p1, BUS_MGR_GetAppCallback(BUS_MGR_GetThis()));
			if(errResult != ERR_OK)
			{
				return errResult;
			}

			switch(p1)
			{
				case KEY_EXIT:
				case KEY_MENU:
					/* reset Title depth */
					Settings_ClearWindowTitle();

					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				case KEY_ARROWLEFT:
				case KEY_OK:
				case KEY_BACK:
					BUS_MGR_Destroy(NULL);
					//s_VaOpMsgCurFocus = 0;
					/* to upper depth */
					BUS_MGR_Create("AP_CAS_VA_OpMessageList_Proc", APP_DEFAULT_PRIORITY, (GWM_Callback_t)AP_CAS_VA_OpMessageList_Proc, 0, 0, 0, 0);
					return MESSAGE_BREAK;

				case KEY_YELLOW:	// Delete button
					if (pNaviContext->ulNumOfMainItem == 0)
					{
						VaOpMsgPrint(DBG_TRACE, ("There is no saved mail\n"));
						return MESSAGE_BREAK;
					}

					// TODO: string 변경해야 함. - gblee

					local_cas_va_OpMsgList_delete_CreateDialog(DLG_YES_NO|DLG_FOCUS_NO|DLG_TYPE_MESSAGE, LOC_MUSIC_POPUP_DELETE_CONFIRM_MSG_ID);
					return MESSAGE_BREAK;

				case	KEY_ARROWUP:
				case	KEY_ARROWDOWN:
					break;

				default:
					AP_NotAvailable_Show();
					return MESSAGE_BREAK;
			}
			break;

		case MSG_GWM_TIMER:
			if(p1 == MENU_TIMEOUT_ID)
			{
				/* reset Title depth */
				Settings_ClearWindowTitle();

				BUS_MGR_Destroy(0);
			}
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			local_cas_va_OpMsgList_text_DestroyUIText(pNaviContext);
//			GWM_FMT_Delete(eOpMsg_List_FMT_ID);
			GWM_Obj_Destroy(eOpMsg_List_FMT_ID);
			/* kill timer */
			BUS_KillTimer(MENU_TIMEOUT_ID);

			/* clear front PANEL */
			//AP_SPEC_ClearMenuPanelString();
			break;

		case MSG_DLG_CLICKED:
			errResult = local_cas_va_OpMsgList_delete_MsgDlgClicked(pNaviContext, p1, p2, p3);
			break;

		default:
			break;
	}

	VaOpMsgPrint(DBG_TRACE, ("AP_CAS_VA_OpMsgText_Proc() handle(0x%x)\n", hAction));

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

#define _________________________Delete_Dialog_Box______________________________

STATIC BUS_Result_t local_cas_va_OpMsgList_delete_MsgDlgClicked(SettingsNaviContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DLG_ClickedButton_t eClickButton = (DLG_ClickedButton_t)p1;

	switch(eClickButton)
	{
	case eDLG_CLICKED_BTN_LEFT: 		// YES
		local_cas_va_OpMsgList_delete_DestroyDialog();
		if (pstContents->ulNumOfMainItem == 0)
		{
			VaOpMsgPrint(DBG_TRACE, ("There is no saved mail\n"));
			return MESSAGE_BREAK;
		}
		VA_UI_Adapt_DeleteOpMessage(s_VaOpMsgCurFocus);

		BUS_MGR_Destroy(NULL);
		s_VaOpMsgCurFocus = 0;
		/* to upper depth */
		BUS_MGR_Create("AP_CAS_VA_OpMessageList_Proc", APP_DEFAULT_PRIORITY, (GWM_Callback_t)AP_CAS_VA_OpMessageList_Proc, 0, 0, 0, 0);
		break;
	case eDLG_CLICKED_BTN_RIGHT:	// NO
		local_cas_va_OpMsgList_delete_DestroyDialog();
		break;
	default:
		break;
	}

	return MESSAGE_BREAK;

	//	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t	local_cas_va_OpMsgList_delete_CreateDialog(HUINT32 ulDLGFlag, LOC_ID_t eLocId)
{
	AP_Dialog_SetDialog(&s_cas_Va_OpMsg_dInstance,	BUS_MGR_GetAppCallback(BUS_MGR_GetThis ()), ulDLGFlag, 0, (HUINT8 *)GetStrRsc(eLocId));
	BUS_MGR_Create("local_option_Dialog_Proc",	APP_DIALOGBOX_PRIORITY,	local_cas_va_OpMsgList_delete_Dialog_Proc,	0, 0, 0, 0);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t local_cas_va_OpMsgList_delete_DestroyDialog(void)
{
	BUS_MGR_Destroy(local_cas_va_OpMsgList_delete_Dialog_Proc);
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t	local_cas_va_OpMsgList_delete_Dialog_Proc(HINT32 nMessage, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch (nMessage)
	{
	case MSG_GWM_KEYDOWN:
		AP_KeyConv_UiCommon(&p1);
		switch (p1)
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
		case KEY_IP_BROWSER:
		case KEY_TEXT:
		case KEY_TTX_HOLD:
		case KEY_TTX_SUBPAGE:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_WIDE:
		case KEY_3D:
		case KEY_INFO:
		case KEY_EXT_AV_PLUS:
		case KEY_EXT_AV_MINUS:
			return MESSAGE_BREAK;

		case KEY_BACK:
			BUS_MGR_Destroy(0);
			return MESSAGE_BREAK;

		default:
			break;
		}
		break;
	}
	return AP_Dialog_ProtoProc(&s_cas_Va_OpMsg_dInstance, nMessage, hAction,p1, p2, p3);
}

#define _________________________MAIL_TEXT_ON_LIVE______________________________

STATIC BUS_Result_t	local_cas_va_OpMsg_text_DrawTitle(OSD_Screen_t *pOsdScreen, OSD_Area_t *appArea,
										OSD_Rect_t *objRect, GWM_GuiObject_t object, void *arg, int bUiTop)
{
	OSD_Font_t			*pFont = NULL;
	OSD_Rect_t		 	stRect;
	TEXT_DRAW_INFO_t 	stInfo;
	OSD_StockImage_t	stStockImage;
	SettingsNaviContext_t	*pstContents = NULL;
	HUINT8				*buf = GetTextBuffer(TEXT_SIZE64);
	HUINT8				ucCnt = 0;
	HERROR				hError = ERR_FAIL;
	HxDATETIME_t		stDateTime;

	pstContents = local_cas_va_OpMsg_GetContentsText();

	// mail Icon
	VK_memset32(&stStockImage, 0x00, sizeof(OSD_StockImage_t));

	if (pstContents->ulNumOfMainItem < 1)
	{
		return	ERR_INITIAL_FAILED;
	}

	if (AP_StockImg_GetImage(e470_1_46_Mail_Messages_03, &stStockImage) != ERR_OK)
	{
		return	ERR_INITIAL_FAILED;
	}

	OSD_GFX_SetRect(&stRect, VA_OPMSG_TEXT_TITILE_IMAGE_X, VA_OPMSG_TEXT_TITILE_IMAGE_Y, stStockImage.width, stStockImage.height);
	OSD_GFX_FB_Blend(stStockImage.hBuffer, stStockImage.width, stStockImage.height, pOsdScreen, &stRect);

	// date & time
	pFont = OSD_FONT_Get (eFont_SystemNormal);

	stInfo.ulTextColor 	= COL(C_T_Cursor);
	stInfo.ulFontSize 	= FONT_SIZE_M;
	stInfo.ulXAlign 	= TEXT_ALIGN_CENTER;
	stInfo.ulYAlign 	= TEXT_ALIGN_MIDDLE;
	stInfo.ulDrawMode 	= TEXTMODE_NORMAL;
	OSD_GFX_CopyRect(&stRect, objRect);
	stRect.x += VA_OPMSG_TEXT_TITILE_IMAGE_W;

	HLIB_DATETIME_ConvertUnixTimeToDateTime(s_stVaOpMsgItemData[pstContents->nCurFocus].stOpMsg.stUnixTime, &stDateTime);

	snprintf(buf, TEXT_SIZE64, "%02d/%02d    %02d:%02d:%02d", stDateTime.stDate.ucDay, stDateTime.stDate.ucMonth,
						stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond);

	AP_UTIL_DrawFormattedText(pOsdScreen, pFont, &stRect, &stInfo, buf);

	OSD_FONT_Release (pFont);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t	local_cas_va_OpMsg_text_DrawHelp(OSD_Screen_t *pOsdScreen, OSD_Area_t *appArea,
										OSD_Rect_t *objRect, GWM_GuiObject_t object, void *arg, int bUiTop)
{
	OSD_Font_t			*pFont = NULL;
	OSD_Rect_t		 	stRect;
	TEXT_DRAW_INFO_t 	stInfo;
	OSD_StockImage_t	stStockImage;
	HUINT8				*buf = GetTextBuffer(TEXT_SIZE64);

	// mail Icon
	VK_memset32(&stStockImage, 0x00, sizeof(OSD_StockImage_t));

	if (AP_StockImg_GetImage(e181_1_46_Help9_EXIT, &stStockImage) != ERR_OK)
	{
		return ERR_INITIAL_FAILED;
	}

	OSD_GFX_SetRect(&stRect, VA_OPMSG_TEXT_HELP_IMAGE_X, VA_OPMSG_TEXT_HELP_IMAGE_Y, stStockImage.width, stStockImage.height);
	OSD_GFX_FB_Blend(stStockImage.hBuffer, stStockImage.width, stStockImage.height, pOsdScreen, &stRect);


	// close
	pFont = OSD_FONT_Get (eFont_SystemNormal);

	stInfo.ulTextColor 	= COL(C_T_Dialog_Enable);
	stInfo.ulFontSize 	= FONT_SIZE_S;
	stInfo.ulXAlign 	= TEXT_ALIGN_LEFT;
	stInfo.ulYAlign 	= TEXT_ALIGN_MIDDLE;
	stInfo.ulDrawMode 	= TEXTMODE_NORMAL;
	OSD_GFX_CopyRect(&stRect, objRect);
	stRect.x += VA_OPMSG_TEXT_HELP_TEXT_GAP;

	snprintf(buf,TEXT_SIZE64, "%s", GetStrRsc(LOC_CAS_VA_CLOSE_ID));

	AP_UTIL_DrawFormattedText(pOsdScreen, pFont, &stRect, &stInfo, buf);

	OSD_FONT_Release (pFont);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t	local_cas_va_OpMsg_text_DrawItems(int lId, OSD_Screen_t *pScreen, FMT_DrawItem_t *pDrawItem)
{
	OSD_Rect_t				stItemRect;
	OSD_Font_t				*pFont = OSD_FONT_Get(eFont_SystemNormal);
	TEXT_DRAW_INFO_t		stTextInfo;

	stTextInfo.ulTextColor = pDrawItem->stTextProperty.cFontColor;
	stTextInfo.ulFontSize = pDrawItem->stTextProperty.ulFontSize;
	stTextInfo.ulDrawMode = TEXTMODE_FORCE_FIT;
	stTextInfo.ulXAlign = TEXT_ALIGN_LEFT;
	stTextInfo.ulYAlign = TEXT_ALIGN_MIDDLE;
	OSD_GFX_CopyRect(&stItemRect, &(pDrawItem->itemRect));

	AP_UTIL_DrawFormattedText(pScreen, pFont, &stItemRect, &stTextInfo, (HUINT8*)pDrawItem->str);

	OSD_FONT_Release (pFont);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t	local_cas_va_OpMsg_text_DrawScroll(	OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, FMT_OBJECT pObject)
{
	HINT32					lX = 0;
	HINT32					lY = 0;
	HINT32					lW = 0;
	HINT32					lH = 0;
	HINT32					lTotalPage = 0;
	HINT32					lIndex = 0;
	HUINT32					ulAttrib = 0;
	HUINT32					ulStartIndex = 0;
	HUINT32					ulLinePerPage = 0;
	HUINT32					ulItemNum = 0;
	HUINT32					ulThumbHeight = 0;
	HUINT32					ulThumbY = 0;
	HINT32					lBgHeight = 0;
	OSD_StockImage_t		stThumbHeadImg = {0, };
	OSD_StockImage_t		stThumbBodyImg = {0, };
	OSD_StockImage_t		stThumbTailImg = {0, };
	OSD_StockImage_t		stBgImg = {0, };
	OSD_Rect_t				stRect = {0, };


	GWM_FMT_GetProperty(pObject, &ulAttrib, &ulStartIndex, &ulLinePerPage, &ulItemNum);
	if (ulItemNum <= ulLinePerPage)
	{
		return ERR_BUS_NO_ERROR;
	}

	if(AP_StockImg_GetImage(e166_2_00_Scroll_W1_1T, &stThumbHeadImg))
	{
		return ERR_BUS_NO_ERROR;
	}
	if(AP_StockImg_GetImage(e166_2_00_Scroll_W1_2M, &stThumbBodyImg))
	{
		return ERR_BUS_NO_ERROR;
	}
	if(AP_StockImg_GetImage(e166_2_00_Scroll_W1_3B, &stThumbTailImg))
	{
		return ERR_BUS_NO_ERROR;
	}
	if(AP_StockImg_GetImage(e166_2_00_Scroll_W1_4BG, &stBgImg))
	{
		return ERR_BUS_NO_ERROR;
	}

	lBgHeight = stBgImg.height;

	lX = pArea->x1;
	lY = pArea->y1;
	lW = pArea->x2;
	lH = pArea->y2;

	lX = pArea->x1 + VA_OPMSG_TEXT_FTM_SCROLL_X;
	lY = pArea->y1 + VA_OPMSG_TEXT_FTM_SCROLL_Y;

	lH = VA_OPMSG_TEXT_FMT_H;


	// support line move & page move
	if (ulItemNum < ulLinePerPage)
	{
		lTotalPage = 1;
	}
	else
	{
		lTotalPage = (ulItemNum/ulLinePerPage) + (ulItemNum%ulLinePerPage ? 1 : 0);
	}


	ulThumbHeight = (HUINT32)(lH / lTotalPage);
	if (ulThumbHeight < 20)
	{
		ulThumbHeight = 20;
	}
	else if (ulThumbHeight > (HUINT32)(lH / 2))
	{
		ulThumbHeight = (HUINT32)(lH / 2);
	}

	if (ulStartIndex > 0)
	{
		if(ulItemNum > ulStartIndex + ulLinePerPage)
		{
			ulThumbY = lY + ( (lH / ulItemNum) * ulStartIndex);
		}
		else
		{
			ulThumbY = lY + (lH - ulThumbHeight);
		}
	}
	else
	{
		ulThumbY = lY;
	}

	lIndex = lY;
	while(lIndex < lY + lH)
	{

		if (lIndex != ulThumbY)
		{
			// draw bg

			OSD_GFX_SetRect(&stRect, lX, lIndex, stBgImg.width, stBgImg.height);
			OSD_GFX_FB_Blend(stBgImg.hBuffer, stBgImg.width, stBgImg.height, pScreen, &stRect);
			lIndex += lBgHeight;
		}
		else
		{
			// draw thumb

			OSD_GFX_BlendStockVFrame (pScreen, lX, lIndex, lX + stThumbHeadImg.width, lIndex + ulThumbHeight,
									&stThumbHeadImg, &stThumbBodyImg, &stThumbTailImg, 100);
			lIndex += ulThumbHeight;
		}
	}

	return ERR_BUS_NO_ERROR;

}

STATIC void	local_cas_va_OpMsg_text_DrawText(SettingsNaviContext_t *pstContents)
{
	OSD_Rect_t			stRect;
	Text_Property_t 	stTextProperty;
	OSD_Font_t			*pFont = OSD_FONT_Get(eFont_SystemNormal);
	HUINT8				*pBuf = GetTextBuffer(TEXT_SIZE512);
	HUINT8				*pBuf2 = GetTextBuffer(TEXT_SIZE64);
	eHxCP_CodePage_e		encoding;
	HUINT32				nNumOfChForOneLine;
	HUINT8				ucCnt = 0;
	HERROR				hError = ERR_FAIL;

	/* Set Area. */
	GWM_APP_SetAppArea (VA_OPMSG_TEXT_WIN_X, VA_OPMSG_TEXT_WIN_Y, VA_OPMSG_TEXT_WIN_W, VA_OPMSG_TEXT_WIN_H);

	OSD_GFX_SetRect (&stRect, 0, 0, VA_OPMSG_TEXT_WIN_W, VA_OPMSG_TEXT_WIN_H);
	GWM_Frame_Create (eOpMsg_Text_FRAME_ID, &stRect, (char *)NULL);
	GWM_APP_SetObjectDrawMethod (eOpMsg_Text_FRAME_ID, AP_LNF_WindowSF3Frame_Draw);

	OSD_GFX_SetRect (&stRect, VA_OPMSG_TEXT_TITILE_TEXT_X, VA_OPMSG_TEXT_TITILE_TEXT_Y, VA_OPMSG_TEXT_TITILE_TEXT_W, VA_OPMSG_TEXT_TITILE_TEXT_H);
	GWM_Frame_Create (eOpMsg_Text_FRAME_TITLE_ID, &stRect, (char *)NULL);
	GWM_APP_SetObjectDrawMethod (eOpMsg_Text_FRAME_TITLE_ID, local_cas_va_OpMsg_text_DrawTitle);


	OSD_GFX_SetRect(&stRect, VA_OPMSG_TEXT_FMT_X, VA_OPMSG_TEXT_FMT_Y, VA_OPMSG_TEXT_FMT_W, VA_OPMSG_TEXT_FMT_H);

#if 1
	GWM_Text_Create(eOpMsg_Text_FMT_ID, &stRect, s_stVaOpMsgItemData[pstContents->nCurFocus].stOpMsg.szMsg);
	GWM_Text_SetSize(eOpMsg_Text_FMT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor(eOpMsg_Text_FMT_ID, COL(C_T_Dialog_Enable), 0);
	GWM_APP_SetObjectDrawMethod(eOpMsg_Text_FMT_ID, AP_LNF_PlainMultilineText_Draw);
#else
	GWM_FMT_Create(eOpMsg_Text_FMT_ID, &stRect, FMT_OWN_DRAWITEM | FMT_SCROLL_ON, 5);

	stTextProperty.ulFontSize 	= FONT_SIZE_S;
	stTextProperty.ulFontAlign 	= TEXT_ALIGN_LEFT;
	stTextProperty.cFontColor	= COL(C_T_Dialog_Enable);

	OSD_FONT_SetSize(pFont, FONT_SIZE_S);

	if (pstContents->ulNumOfMainItem < 1)
	{
		return;
	}

	HxSTD_memset(pBuf, 0, TEXT_SIZE512);
	snprintf(pBuf, TEXT_SIZE512, "%s", s_stVaOpMsgItemData[pstContents->nCurFocus].stOpMsg.szMsg);

	encoding= MWC_UTIL_GetEncodingByLocaleCode((const HUINT8 *)pBuf);

	do
	{
		HxSTD_memset(pBuf2, 0, TEXT_SIZE64);

		OSD_FONT_CountChByWidth_SingleByte( pFont, stRect.w, encoding, strlen(pBuf), pBuf, &nNumOfChForOneLine);

		strncpy(pBuf2, pBuf, nNumOfChForOneLine);

		pBuf += nNumOfChForOneLine;

		GWM_FMT_AddItem(eOpMsg_Text_FMT_ID, (char *)pBuf2, &stTextProperty);

	} while(nNumOfChForOneLine != 0);


	GWM_FMT_ChangeItemDrawMethod(eOpMsg_Text_FMT_ID, local_cas_va_OpMsg_text_DrawItems);
	GWM_FMT_ChangeScrollDrawMethod(eOpMsg_Text_FMT_ID, local_cas_va_OpMsg_text_DrawScroll);
#endif

	OSD_GFX_SetRect (&stRect, VA_OPMSG_TEXT_HELP_X, VA_OPMSG_TEXT_HELP_Y, VA_OPMSG_TEXT_HELP_W, VA_OPMSG_TEXT_HELP_H);
	GWM_Frame_Create (eOpMsg_Text_HELP_ID, &stRect, (char *)NULL);
	GWM_APP_SetObjectDrawMethod (eOpMsg_Text_HELP_ID, local_cas_va_OpMsg_text_DrawHelp);

	GWM_Obj_SetFocus(eOpMsg_Text_FMT_ID);

	OSD_FONT_Release (pFont);

}

/******************************************************************************/
/**
*	\fn 	AP_CAS_VA_OpMsgText_Proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*	\brief	settings procedure
*
*	@param[in]	gwm msg/action handler
*	@return 	BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*	@ingroup	SETRTINGS
*/
/******************************************************************************/
BUS_Result_t		AP_CAS_VA_OpMsgText_Proc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 				errResult = ERR_FAIL;
	SettingsNaviContext_t	*pNaviContext = NULL;
	HUINT8					nCnt;

	VaOpMsgPrint(DBG_TRACE, ("AP_CAS_VA_OpMsgText_Proc() message(0x%x), p1(0x%x), p2(0x%x), p3(0x%x)\n", message, p1,p2,p3));

	/* get running param */
	pNaviContext = local_cas_va_OpMsg_GetContentsText();

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* init application area */
			GWM_APP_SetAppArea(SYSETM_APP_WINDOW_X, SYSETM_APP_WINDOW_Y, SYSETM_APP_WINDOW_WIDTH, SYSETM_APP_WINDOW_HEIGHT);

			// 마지막 메시지가 맨 앞에 있으므로 항상 0
			VA_UI_Adapt_GetOpMessage(&s_stVaOpMsgItemData[0].stOpMsg, 0);
			pNaviContext->nCurFocus = 0;

			// update cas information
			pNaviContext->ulNumOfMainItem = VA_UI_Adapt_GetOpMessageCount();
			// draw data
			if(pNaviContext->ulNumOfMainItem != 0)
			{
				local_cas_va_OpMsg_text_DrawText(pNaviContext);
			}

			/* set front pannel */
			//AP_SPEC_DisplayMenuPanelString(pNaviContext->title, eDxPANEL_ALIGN_CENTER);

			// 읽은 것으로 표
			s_stVaOpMsgItemData[0].stOpMsg.ucRead = TRUE;
			VA_UI_Adapt_UpdateOpMessage(&s_stVaOpMsgItemData[0].stOpMsg, 0);
			break;

		case MSG_GWM_KEYUP:
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN:
			/* settings이 전반적으로 처리해야 할 키들 */
			AP_KeyConv_UiCommon(&p1);

			GWM_ResetTimer(MENU_TIMEOUT_ID);

			errResult = Settings_CheckKeyPass(p1, BUS_MGR_GetAppCallback(BUS_MGR_GetThis()));
			if(errResult != ERR_OK)
			{
				return errResult;
			}

			switch(p1)
			{
				case KEY_OK:
				case KEY_BACK:
				case KEY_EXIT:
				case KEY_MENU:
					/* reset Title depth */
					Settings_ClearWindowTitle();

					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;
/*
				case KEY_RED:	// Delete button
					if (pNaviContext->ulNumOfMainItem == 0)
					{
						VaOpMsgPrint(DBG_TRACE, ("There is no saved mail\n"));
						return MESSAGE_BREAK;
					}

					VA_UI_Adapt_DeleteOpMessage(pNaviContext->nCurFocus);

					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;
*/
				case KEY_ARROWLEFT:
					p1 = KEY_ARROWUP;
					break;

				case KEY_ARROWRIGHT :
					p1 = KEY_ARROWDOWN;
					break;

			}
			break;

		case MSG_GWM_TIMER:
			if(p1 == MENU_TIMEOUT_ID)
			{
				/* reset Title depth */
				Settings_ClearWindowTitle();

				BUS_MGR_Destroy(0);
			}
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			local_cas_va_OpMsgList_text_DestroyUIText(pNaviContext);
//			GWM_FMT_Delete(eOpMsg_Text_FMT_ID);
			GWM_Obj_Destroy(eOpMsg_Text_FMT_ID);

			/* kill timer */
			BUS_KillTimer(MENU_TIMEOUT_ID);

			/* clear front PANEL */
			//AP_SPEC_ClearMenuPanelString();
			break;

		default:
			break;
	}

	VaOpMsgPrint(DBG_TRACE, ("AP_CAS_VA_OpMsgText_Proc() handle(0x%x)\n", hAction));

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


/* end of file */

