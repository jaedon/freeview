
/**
	@file     ap_cas_cx_statuslist.c
	@brief    file_name & simple comment.

	Description: for CONAX MENU Status List UI \n
	Module: Application(Layer), Menu(module), CAS_CX(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/24		modified					jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <db_svc.h>
#include <db_param.h>
#include <svc_sys.h>
#include <svc_cas.h>
#include <isosvc.h>
#include <osd_gfx.h>
#include <osd_font.h>

#include <conax.h>
#include <cxcore_sc.h>
#include <cx_common.h>
#include <xmgr_cas_res_str.h>
#include <xmgr_cas_util_dslib.h>

#include <app_manager_cas.h>
#include <mgr_cas.h>

#include "../../local_include/_xmgr_cas_cx_adapt.h"
#include "../../local_include/_xmgr_cas_cx_util.h"
#include "../../local_include/_xmgr_cas_cx.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

//STATIC HUINT8		s_szTitle[TEXT_SIZE128];

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define	CAS_CX_STATUSLIST_ITEM_TEXT_LENGTH			32
#define	CAS_CX_STATUSLIST_TEXT_WDITH				300

#define	CAS_CX_STATUSLIST_DEF_COLOR					0x00000000
#define	CAS_CX_STATUSLIST_TRANSPARENCY				100

#define	CAS_CX_STATUSLIST_ITEM_MAX					9

#define	CAX_CX_MAX_ITEM_STRLEN						64

#define	D_TEXT_LINE_LENGTH							54

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
#define	CAS_CX_MAIL_BOX_X							143
#define	CAS_CX_MAIL_BOX_Y							178
#define	CAS_CX_MAIL_BOX_W							987
#define	CAS_CX_MAIL_BOX_H							478

#define	CAS_CX_MAIL_CURSOR_X						20
#define	CAS_CX_MAIL_CURSOR_Y						15
#define	CAS_CX_MAIL_CURSOR_W						110

#define	CAS_CX_MAIL_ITEM_X							20
#define	CAS_CX_MAIL_ITEM_Y							20
#define	CAS_CX_MAIL_ITEM_IMG_GAP_X					12
#define	CAS_CX_MAIL_ITEM_IMG_GAP_Y					12
#define	CAS_CX_MAIL_ITEM_IMG_GAP_W					15
#define	CAS_CX_MAIL_ITEM_DATE_GAP_W					15
#define	CAS_CX_MAIL_ITEM_TIME_GAP_W					30
#define	CAS_CX_MAIL_ITEM_TEXT_GAP_W					30
#define	CAS_CX_MAIL_ITEM_TEXT_W						550

#define	CAS_CX_MAIL_ITEM_SCROLL_H					420

#define	CAS_CX_SCROLL_W								9
#define	CAS_CX_SCROLL_H				  				400
#define	CAS_CX_SCROLL_X				 			  	10
#define	CAS_CX_SCROLL_Y				   				23
#define	CAS_CX_SCROLLBALL_H							20

#define	CAS_CX_MAIL_BOX_X							143
#define	CAS_CX_MAIL_BOX_Y							178
#define	CAS_CX_MAIL_BOX_W							987
#define	CAS_CX_MAIL_BOX_H							478

#define	CAS_CX_MAIL_FMT_X							(CAS_CX_MAIL_BOX_X + 30)
#define	CAS_CX_MAIL_FMT_Y							(CAS_CX_MAIL_BOX_Y + 20)
#define	CAS_CX_MAIL_FMT_W							(CAS_CX_MAIL_BOX_W - 50)
#define	CAS_CX_MAIL_FMT_H							(CAS_CX_MAIL_BOX_H - 40)

#define	CAS_CX_MAIL_TITLE_X							521
#define	CAS_CX_MAIL_TITLE_Y							134
#define	CAS_CX_MAIL_TITLE_W							(CAS_CX_MAIL_TITLE_X + 250)
#define	CAS_CX_MAIL_TITLE_H							(CAS_CX_MAIL_TITLE_Y + 50)
#define	CAS_CX_MAIL_TITLE_DATE_GAP_W				15
#define	CAS_CX_MAIL_TITLE_DATE_GAP_H				15

#define	CAS_CX_MAIL_TITLE_TIME_GAP_W				25
#define	CAS_CX_MAIL_TITLE_DATE_GAP_Y				CAS_CX_MAIL_TITLE_DATE_GAP_H

#define	CAS_CX_MAIL_SCROLL_GAP_W					15
#define	CAS_CX_MAIL_SCROLL_GAP_Y					5
#define	CAS_CX_MAIL_SCROLL_GAP_H					30


#define	CAS_CX_MAIL_BOX_CREATE_COL					0x00000000

#endif


/*******************************************************************/
/********************      Struture        *************************/
/*******************************************************************/

typedef enum
{
	eCasCx_StatusListItem_ListTitle,	/* List Title Item */
	eCasCx_StatusListItem_Max,
} XmgrCas_CxStatusListItem_e;



typedef enum
{
	eCasCXMenuItem_Debit = eCasCXMenuItem_Max,
	eCasCXMenuItem_Credit,
	eCasCXMenuItem_Add_Max,
} XmgrCas_CxMenuItemAdd_e;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
typedef struct CxListMenuItem
{
	XmgrCas_CxCaMenuItem_e	mainBtnId;
	CxPurseStatus_t			stPurseStatus;		// tokens , debit
	Custom_list_t			stTextListBox;
	BUS_Callback_t			pfnUiCallback;
} XmgrCas_CxListMenuItem_t;

typedef struct tag_StatusList_content
{
	HUINT32			ulTotalNum;
//	SettingsLeafSubItem_t	*subItems;

	HUINT8				szTitle[MAX_ELEMENT_TEXT_LENGTH];
	HUINT8				szColumnTitle[MAX_ELEMENT_TEXT_LENGTH];
	HINT32				lCasCxCAItemID;
	CxPurseStatus_t		stPurseStatus;		// tokens , debit
	Custom_list_t		stTextListBox;
	BUS_Callback_t		pfnCallerCallback;
//	XmgrCas_CxListMenuItem_t	*astListItem;
} XmgrCas_CxStatusListContent_t;

#if 0 // not use for octo2.0
typedef struct tag_StatusList_content
{
	/* for drawing application window */
	HUINT32			ulCurFocus;
	HUINT32			ulTotalNum, ulMaxNumOfSubItem;
	XmgrCas_CxSettingsLeafSubItem_t	*subItems;

	/* running info for app */
	HUINT8			szTitle[MAX_ELEMENT_TEXT_LENGTH];

	HINT32			lCasCxCAItemID;
	HINT32			lToUpper;

	CxPurseStatus_t	stPurseStatus;

	/* List Item */
//	HBOOL			bDrawCursor;
//	Custom_list_t	stTextListBox;
//	GWM_ObjDraw_t	objDrawFunc;

	/* Dialog*/
//	AP_DialogInstance_t		stDialogInstance;
} XmgrCas_CxStatusListContent_t;
#endif

#if defined(CONFIG_MW_CAS_CONAX_TEXT)

typedef enum
{
	eMail_MSG_BOX_ID = 0,
	eMail_MSG_ITEM,
	eMail_MSG_ICON_ID,
	eMail_MSG_FMT_ID,
	eMail_MSG_HELP_ID,
	eMail_MSG_ID_MAX
} XmgrCas_CxMai_MsgIds_e;


typedef struct	tagCXMailItem
{
	HBOOL	bRead;
	HBOOL	bCheck;
	HINT32	nMailIdx;
	HUINT8	strReceivedDate[6];
	HUINT8	strReceivedTime[9];
	HINT32	nMailType;
	HUINT8	strMailTxt[CX_MAX_USER_TEXT_LEN + 1]; // this data has normal msg  & attributed msg property, then it must have biggest size.
} XmgrCas_CxMailItemData_t;

typedef struct	tag_CAS_CXMailList_content
{
	/* for drawing application window */
	HUINT32 		ulTotalNum;

	/* running info for app */
	HUINT8			ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	HINT32			lCasItemID;
	HINT32			lToUpper;
} XmgrCas_CxMailListContent_t;

STATIC XmgrCas_CxMailListContent_t	s_CXMailMsgContent;
STATIC XmgrCas_CxMailItemData_t		s_pstCXMailItemData[CX_MAX_USERTEXT_SLOT_NUM];

STATIC XmgrCas_CxSettingsNaviContext_t	s_CXMailMsgTextContent;

//STATIC HUINT32					s_CXMailCurFocus = 0;
#endif

STATIC XmgrCas_CxStatusListContent_t	s_CasCxStatusListContents;




/*******************************************************************/
/********************      	Static Prototype	********************/
/*******************************************************************/
STATIC XmgrCas_CxStatusListContent_t	*xmgr_cas_CxStatusListGetLocalContents(void);
#if 0 // not use for octo2.0
STATIC BUS_Result_t		xmgr_cas_CxStatusListDestroyUI(XmgrCas_CxStatusListContent_t *pstContents);
#endif
STATIC BUS_Result_t		xmgr_cas_CxStatusListInitStatusList(XmgrCas_CxStatusListContent_t *pstContents, HINT32 p1, HINT32 lToUpper, HINT32 lItemObjID);
STATIC HERROR			xmgr_cas_CxStatusListInitListTitle(XmgrCas_CxStatusListContent_t *pstContents);

STATIC void				xmgr_cas_CxStatusListInitListItems(XmgrCas_CxStatusListContent_t *pstContents);
STATIC HERROR			xmgr_cas_CxStatusListLoadItemData(XmgrCas_CxStatusListContent_t *pstContents);
STATIC HERROR			xmgr_cas_CxStatusListLoadItem_SubscriptionData(XmgrCas_CxStatusListContent_t *pstContents);
STATIC HERROR			xmgr_cas_CxStatusListLoadItem_EventData(XmgrCas_CxStatusListContent_t *pstContents);
STATIC HERROR			xmgr_cas_CxStatusListLoadItem_TokensData(XmgrCas_CxStatusListContent_t *pstContents);
STATIC HERROR			xmgr_cas_CxStatusListLoadItem_DebitData(XmgrCas_CxStatusListContent_t *pstContents, CxPurseStatus_t *pstPurseStatus);
STATIC HERROR			xmgr_cas_CxStatusListLoadItem_CreditData(XmgrCas_CxStatusListContent_t *pstContents, CxPurseStatus_t *pstPurseStatus);
STATIC HERROR			xmgr_cas_CxStatusListLoadItem_MessagesData(XmgrCas_CxStatusListContent_t *pstContents);


STATIC HERROR			xmgr_cas_CxStatusListResetLists(XmgrCas_CxStatusListContent_t *pstContents);
STATIC HERROR			xmgr_cas_CxStatusListFreeItemData(XmgrCas_CxStatusListContent_t *pstContents);

#if defined(CONFIG_MW_CAS_CONAX_TEXT) && defined(CONFIG_MW_CAS_CONAX_MAIL)
STATIC XmgrCas_CxMailListContent_t	*xmgr_cas_CxMailMessageGetContents(void);

STATIC HERROR		xmgr_cas_CxMailMessageLoadMailList(XmgrCas_CxMailListContent_t *pstContents);
// TODO: Blocking....
#if 0 // not use for octo2.0
STATIC BUS_Result_t	xmgr_cas_CxMailMessageDrawAllItemList(OSD_Screen_t *screen, OSD_Area_t *area,
										OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1);

STATIC void 		xmgr_cas_CxMailMessageDrawData(XmgrCas_CxMailListContent_t *pstContents);
#endif
STATIC HERROR		xmgr_cas_CxMailMessageIsValidMailMessage(HUINT16 ulDuration);
STATIC HERROR		xmgr_cas_CxMailMessageCheckReadAllMailMessage(void);
STATIC HERROR		xmgr_cas_CxMailMessageDestroyUI(XmgrCas_CxMailListContent_t *pstContents);


STATIC XmgrCas_CxSettingsNaviContext_t	*xmgr_cas_CxMailTextGetContentsText(void);
STATIC HERROR		xmgr_cas_CxMailTextIsValidMailText(HUINT16 ulDuration);
STATIC HERROR		xmgr_cas_CxMailTextCheckReadAllMailMessage(void);
STATIC void			xmgr_cas_CxMailTextLoadMailText(XmgrCas_CxSettingsNaviContext_t *pstContents, HINT32 lSlotIdx, HINT32 lToUpper);
// TODO: Blocking....
#if 0 // not use for octo2.0
STATIC void			xmgr_cas_CxMailTextDrawText(XmgrCas_CxSettingsNaviContext_t *pstContents);
STATIC BUS_Result_t	xmgr_cas_CxMailTextDrawItems(int lId, OSD_Screen_t *pScreen, FMT_DrawItem_t *pDrawItem);
STATIC BUS_Result_t	xmgr_cas_CxMailTextDrawScroll(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, FMT_OBJECT pObject);
STATIC BUS_Result_t	xmgr_cas_CxMailTextDrawTitle(OSD_Screen_t *screen, OSD_Area_t *area,
										OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1);
STATIC HERROR		xmgr_cas_CxMailTextDestroyUIText(XmgrCas_CxSettingsNaviContext_t *pstContents);
#endif
#endif

STATIC HERROR xmgr_cas_CxStatusListSendData(XmgrCas_CxStatusListContent_t *pstContext);

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

#if defined(CONFIG_MW_CAS_CONAX_TEXT) && defined(CONFIG_MW_CAS_CONAX_MAIL)

#define _________________________NORMAL_MAIL_TEXT______________________________

STATIC HERROR	xmgr_cas_CxMailMessageLoadMailList(XmgrCas_CxMailListContent_t *pstContents)
{
	HERROR				hError = ERR_FAIL;
	HINT32 				i;
	HUINT32 			ulCnt = 0, ulNumMsg;
	CxUserTextList_t 	pstList;
	XmgrCas_CxMailItemData_t	*pMailItemData = NULL;

	HxLOG_Info("xmgr_cas_CxMailMessageLoadMailList() +\n");

	VK_memset32(pstContents, 0, sizeof(XmgrCas_CxMailListContent_t));
	for( i = 0; i < CX_MAX_USERTEXT_SLOT_NUM; i++ )
	{
		HxSTD_memset(&s_pstCXMailItemData[i], 0, sizeof(XmgrCas_CxMailItemData_t));
	}

	hError = xmgr_cas_CxGetUserText(&ulNumMsg, &pstList);
	if ((hError != ERR_OK) || (ulNumMsg < 1))
	{
		HxLOG_Critical("\n\n");
		pstContents->ulTotalNum = 0;
		//pstContents->ulCurFocus = 0;
		return	ERR_FAIL;
	}
	else
	{
		//pstContents->ulCurFocus = 0;
	}

	HxLOG_Print("Mail Load Count = %d\n", ulNumMsg);
	ulCnt = 0;
	for (i=0 ; i< ulNumMsg ; i++)
	{
		hError = xmgr_cas_CxMailMessageIsValidMailMessage(pstList.pstUserText[i].usDuration);
		if (hError == ERR_OK && (strlen(pstList.pstUserText[i].szText) > 0))
		{
			if (pMailItemData == NULL)
			{
				pMailItemData = (XmgrCas_CxMailItemData_t*)OxAP_Malloc(sizeof(XmgrCas_CxMailItemData_t));
			}
			if (pMailItemData == NULL)
			{
				HxLOG_Critical("\n\n");
				continue;
			}

			HxSTD_memset(pMailItemData, 0, sizeof(XmgrCas_CxMailItemData_t));

			pMailItemData->bRead		= pstList.pstUserText[i].ucDisplayed;
			pMailItemData->nMailIdx		= pstList.pstUserText[i].ucSeqNo;
			HxSTD_memcpy(pMailItemData->strMailTxt, pstList.pstUserText[i].szText, CX_MAX_USER_TEXT_LEN);
			pMailItemData->strMailTxt[CX_MAX_USER_TEXT_LEN] = '\0';

			// prevent fix
			snprintf((char*)pMailItemData->strReceivedDate, sizeof(pMailItemData->strReceivedDate), "%02d/%02d", pstList.pstUserText[i].stStartDataTime.stDate.ucDay, pstList.pstUserText[i].stStartDataTime.stDate.ucMonth);
			snprintf((char*)pMailItemData->strReceivedTime, sizeof(pMailItemData->strReceivedTime), "%02d:%02d",
				pstList.pstUserText[i].stStartDataTime.stTime.ucHour, pstList.pstUserText[i].stStartDataTime.stTime.ucMinute);

			HxSTD_memcpy(&s_pstCXMailItemData[ulCnt], pMailItemData, sizeof(XmgrCas_CxMailItemData_t));
			if (pMailItemData != NULL)
			{
				OxAP_Free(pMailItemData);
				pMailItemData = NULL;
			}

// TODO: Blocking....
#if 0 // not use for octo2.0
			hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), s_pstCXMailItemData[ulCnt].strMailTxt);
			if (hError != ERR_OK)
			{
				HxLOG_Critical("\n\n");
			}
#endif
			ulCnt++;
		}
	}

	pstContents->ulTotalNum = ulCnt;
	HxLOG_Print("Mail Count = %d\n", ulCnt);

	return	ERR_OK;
}


STATIC XmgrCas_CxMailListContent_t	*xmgr_cas_CxMailMessageGetContents(void)
{
	return &s_CXMailMsgContent;
}

// TODO: Blocking....
#if 0 // not use for octo2.0
STATIC BUS_Result_t	xmgr_cas_CxMailMessageDrawAllItemList(OSD_Screen_t *screen, OSD_Area_t *area,
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
	XmgrCas_CxMailListContent_t	*pstContents = xmgr_cas_CxMailMessageGetContents();
	OSD_Rect_t			stRect;
	HINT32				nStockImgId;

	NOT_USED_PARAM(arg);
	NOT_USED_PARAM(param1);
	NOT_USED_PARAM(area);

	if(pstContents->ulTotalNum == 0)
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

	if (CAS_CX_STATUSLIST_ITEM_MAX < totnum)
	{
		lastIdx = CAS_CX_STATUSLIST_ITEM_MAX;
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
				OSD_GFX_BlendStockFrame(screen, rect->x + CAS_CX_MAIL_CURSOR_X, rect->y + CAS_CX_MAIL_CURSOR_Y + (i*MENU_MAINITEM_CURSOR_LINESPACE), rect->w + CAS_CX_MAIL_CURSOR_W, rect->h,
								&leftImg, &centImg, &rightImg, 100);
			}
		}

		if(s_pstCXMailItemData[start+i].bRead)
		{
			nStockImgId = e178_3_25_Icon_Mail_Open;
		}
		else
		{
			nStockImgId = e178_3_25_Icon_Mail_Closed;
		}

		// draw mail image icon
		if (!AP_StockImg_GetImage (nStockImgId, &Img))
		{
			OSD_GFX_SetRect (&stRect, rect->x + CAS_CX_MAIL_ITEM_X + CAS_CX_MAIL_ITEM_IMG_GAP_X, rect->y + CAS_CX_MAIL_ITEM_IMG_GAP_Y + (i*MENU_MAINITEM_CURSOR_LINESPACE), Img.width, Img.height);
			OSD_GFX_FB_Blend (Img.hBuffer, Img.width, Img.height, screen, &stRect);
		}
		else
		{
			OSD_GFX_SetRect (&stRect, rect->x + CAS_CX_MAIL_ITEM_X + CAS_CX_MAIL_ITEM_IMG_GAP_X, rect->y +  CAS_CX_MAIL_ITEM_IMG_GAP_Y + (i*MENU_MAINITEM_CURSOR_LINESPACE), 0, 0);
		}

		//draw text
		if (ERR_OK == OSD_FONT_SetSize(pFont, FONT_SIZE_M))
		{
			// Received Date
			VK_memset32 (buf, 0, TEXT_SIZE64);
			MWC_UTIL_DvbStrcpy(buf, s_pstCXMailItemData[start+i].strReceivedDate);
			OSD_GFX_SetRect (&stRect, stRect.x +Img.width + CAS_CX_MAIL_ITEM_DATE_GAP_W, stRect.y + 12, 0, 0);
			OSD_GFX_DrawString(screen, pFont, stRect.x , stRect.y, (HUINT8*)buf, COL(fontcolor));

			// Received Time
			w = OSD_GFX_GetStringWidth(pFont, (HUINT8*)buf);

			VK_memset32 (buf, 0, TEXT_SIZE64);
			MWC_UTIL_DvbStrcpy(buf, s_pstCXMailItemData[start+i].strReceivedTime);
			OSD_GFX_SetRect (&stRect, stRect.x +w + CAS_CX_MAIL_ITEM_TIME_GAP_W, stRect.y, 0, 0);
			OSD_GFX_DrawString(screen, pFont, stRect.x , stRect.y, (HUINT8*)buf, COL(fontcolor));

			// Text
			w = OSD_GFX_GetStringWidth(pFont, (HUINT8*)buf);

			VK_memset32 (buf, 0, TEXT_SIZE64);
			MWC_UTIL_DvbStrcpy(buf, s_pstCXMailItemData[start+i].strMailTxt);
			OSD_GFX_SetRect (&stRect, stRect.x + w + CAS_CX_MAIL_ITEM_TEXT_GAP_W, stRect.y, 0, 0);
			OSD_GFX_DrawStringWithAbbr(screen, pFont, stRect.x, stRect.y, CAS_CX_MAIL_ITEM_TEXT_W, (HUINT8*)buf, COL(fontcolor));

		}
	}

	OSD_FONT_Release(pFont);

	if (bNeedScrollBar == TRUE)
	{
		HINT32	nScrollBall_step = 0, nHeight = 0, tmpTotal = totnum, dividedStep = 0;

		lAllPage = (totnum / CAS_CX_STATUSLIST_ITEM_MAX);
		lAllPage += (totnum % CAS_CX_STATUSLIST_ITEM_MAX) ? 1 : 0;

		lScrollBall_height = CAS_CX_MAIL_ITEM_SCROLL_H / lAllPage;
		if (lScrollBall_height < CAS_CX_SCROLLBALL_H)
		{
			lScrollBall_height = CAS_CX_SCROLLBALL_H;
		}

		nHeight = (CAS_CX_MAIL_ITEM_SCROLL_H - lScrollBall_height);
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


STATIC void	xmgr_cas_CxMailMessageDrawData(XmgrCas_CxMailListContent_t *pstContents)
{
	HINT32			objid = eMail_MSG_ITEM;
	OSD_Rect_t		stRect = {0, };

	/* set, draw background */
	Settings_DrawBack(FALSE);

	/* title */
	MWC_UTIL_DvbStrcpy(pstContents->ucTitle, AP_CasResStr_GetStrRsc(LOC_CAS_CX_MESSAGE_TITLE_ID));
	Settings_SetWindowTitle(SETTING_GET_AP_DEPTH(eSetting_CasCxMailMessage), LOC_CAS_CX_MESSAGE_TITLE_ID, pstContents->ucTitle);

	// BOX
	OSD_GFX_SetRect(&stRect, CAS_CX_MAIL_BOX_X, CAS_CX_MAIL_BOX_Y, CAS_CX_MAIL_BOX_W, CAS_CX_MAIL_BOX_H);
	GWM_Rect_Create(eMail_MSG_BOX_ID, &stRect, CAS_CX_MAIL_BOX_CREATE_COL);
	GWM_APP_SetObjectDrawMethod(eMail_MSG_BOX_ID, AP_LNF_DrawBoxD);

	if((pstContents->ulTotalNum > 0))
	{
		// item
		GWM_List_Create(objid, &stRect, "mail msg list", MENU_MAINITEM_CURSOR_LINESPACE, pstContents->ulTotalNum, CAS_CX_STATUSLIST_ITEM_MAX, (GWM_LIST_ITEMS)NULL);
		GWM_APP_SetObjectDrawMethod(objid, xmgr_cas_CxMailMessageDrawAllItemList);
		GWM_Obj_SetFocus (objid);
	}
}
#endif

/******************************************************************************/
/**
*   \fn     xmgr_cas_CxMailMessageDestroyUI(XmgrCas_CxSettingsNaviContext_t *pstContents)
*   \brief  main/sub items memory free
*
*   @param[in]  pstContents
*   @return     ERR_OK
*   @ingroup    SETTINGS
*/
/******************************************************************************/
STATIC HERROR		xmgr_cas_CxMailMessageDestroyUI(XmgrCas_CxMailListContent_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

	NOT_USED_PARAM(pstContents);

	/* if dynamic allocated data are, free */
	//AP_NaviCntxt_FreeMainItemArray(pstContents);

	hError = xmgr_cas_CxMailMessageCheckReadAllMailMessage();
	if (hError == ERR_OK)
	{
		#if 0
		if (BUS_MGR_Get(xproc_cas_CxMainIcon) != NULL)
		{
			BUS_MGR_Destroy(xproc_cas_CxMainIcon);
		}
		#endif
	}

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxMailMessageIsValidMailMessage(HUINT16 ulDuration)
{
	if (ulDuration == 0)
	{
		return	ERR_OK;
	}
	else
	{
		return	ERR_FAIL;
	}
}


STATIC HERROR	xmgr_cas_CxMailMessageCheckReadAllMailMessage(void)
{
	HERROR				hError = ERR_FAIL;
	HINT32 				i;
	HUINT32 			ulCnt = 0, ulNumMsg;
	CxUserTextList_t 	pstList;

	HxLOG_Info("xmgr_cas_CxMailMessageCheckReadAllMailMessage() +\n");

	for( i = 0; i < CX_MAX_USERTEXT_SLOT_NUM; i++ )
	{
		HxSTD_memset(&s_pstCXMailItemData[i], 0, sizeof(XmgrCas_CxMailItemData_t));
	}

	hError = xmgr_cas_CxGetUserText(&ulNumMsg, &pstList);
	if ((hError != ERR_OK) || (ulNumMsg < 1))
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	HxLOG_Print("Mail Load Count = %d\n", ulNumMsg);
	ulCnt = 0;
	for (i=0 ; i< ulNumMsg ; i++)
	{
		hError = xmgr_cas_CxMailMessageIsValidMailMessage(pstList.pstUserText[i].usDuration);
		if ((hError == ERR_OK) && (pstList.pstUserText[i].ucDisplayed == 0) && (strlen(pstList.pstUserText[i].szText) > 0))
		{
			ulCnt++;
		}
	}

	HxLOG_Print("Not read Mail Count = %d\n", ulCnt);
	if (ulCnt > 0)
	{
		return	ERR_FAIL;
	}
	else
	{
		return	ERR_OK;
	}
}


BUS_Result_t	xproc_cas_CxMailMessage(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
//	BUS_Result_t		busResult = ERR_BUS_INITIAL_FAILED;
//	HINT32				lToUppermenu =CAS_CX_MENU_APP_TOUPPER_ON;
//	HUINT32				ulCursor;
//	GWM_GuiObject_t		object;
	XmgrCas_CxMailListContent_t	*pContext = NULL;
	HERROR		hError = ERR_FAIL;

	HxLOG_Info("xproc_cas_CxMailMessage() message(0x%x)\n", message);

	/* get running param */
	pContext = xmgr_cas_CxMailMessageGetContents();

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			// update cas information
			xmgr_cas_CxMailMessageLoadMailList(pContext);

			// TODO: Blocking...
			#if 0 // not use for octo2.0
			/* init application area */
			GWM_APP_SetAppArea(SYSETM_APP_WINDOW_X, SYSETM_APP_WINDOW_Y, SYSETM_APP_WINDOW_WIDTH, SYSETM_APP_WINDOW_HEIGHT);

			// update cas information
			xmgr_cas_CxMailMessageLoadMailList(pContext);
			// draw data
			xmgr_cas_CxMailMessageDrawData(pContext);

			pContext->ulCurFocus = 0;
			pContext->lToUpper = p2;

			GWM_List_SetCurrentIndex(eMail_MSG_ITEM, pContext->ulCurFocus);

			BUS_SetTimer(MENU_TIMEOUT_ID, MENU_TIMEOUT);

			/* set front pannel */
			//AP_SPEC_DisplayMenuPanelString(pContext->ucTitle, eDxPANEL_ALIGN_CENTER);
			AP_SPEC_DisplayMenuPanelString((HUINT8 *)SETTING_PANELSTR_MAIL_MESSAGE, eDxPANEL_ALIGN_CENTER);
			#endif
			break;
		case XMGR_CAS_UPDATE_MENU:
			if (p3 == eCxUi_GroupMessage_Display)
			{
				xmgr_cas_CxMailMessageLoadMailList(pContext);
				// TODO: Blocking...
				#if 0 // not use for octo2.0
				object = BUS_MGR_GetObject(eMail_MSG_ITEM);

				GWM_List_ChangeItems((LIST_OBJECT)object, pContext->ulTotalNum, (GWM_LIST_ITEMS)NULL);
				GWM_List_SetCurrentIndex(eMail_MSG_ITEM, pContext->ulCurFocus);
						GWM_APP_InvalidateRect(SYSETM_APP_WINDOW_X, SYSETM_APP_WINDOW_Y,
												SYSETM_APP_WINDOW_WIDTH, SYSETM_APP_WINDOW_HEIGHT);
				#endif
			}
			break;

		case eMEVT_BUS_DESTROY:
			hError = xmgr_cas_CxMailMessageCheckReadAllMailMessage();
			if (hError == ERR_OK)
			{
				//1 PPV UI는 사라져야 할듯 한데. 필요한 것들도 Close 되도록 하자.
				if (BUS_MGR_Get(xproc_cas_CxMainIcon) != NULL)
				{
					hError = BUS_SendMessage(xproc_cas_CxMainIcon, message, hAction, p1, p2, p3);
				}
			}
			#if 0
			/* destroy UI */
			xmgr_cas_CxMailMessageDestroyUI(pContext);
			#endif

			/* kill timer */
			BUS_KillTimer(MENU_TIMEOUT_ID);

			/* clear front PANEL */
			//AP_SPEC_ClearMenuPanelString();
			break;

		default:
			break;
		// TODO: Blocking...
		#if 0 // not use for octo2.0

		case MSG_BUS_KEYUP:
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_BUS_KEYDOWN:
			/* settings이 전반적으로 처리해야 할 키들 */
			AP_KeyConv_UiCommon(&p1);

			GWM_ResetTimer(MENU_TIMEOUT_ID);

			busResult = Settings_CheckKeyPass(p1, BUS_MGR_GetAppCallback(BUS_MGR_GetThis()));
			if(busResult != ERR_BUS_NO_ERROR)
			{
				return	busResult;
			}

			switch(p1)
			{
				case KEY_STANDBY:
				case KEY_VOLUMEUP:
				case KEY_VOLUMEDOWN:
				case KEY_MUTE:
					return MESSAGE_PASS;

				case KEY_EXIT:
				case KEY_MENU:
					/* reset Title depth */
					Settings_ClearWindowTitle();

					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				case KEY_ARROWLEFT:
				case KEY_BACK:
					/* 상위 메뉴로 올라갈 때 focus를 기억하지 않는 것이 편할 수도 있다. */
					//Settings_InitFocus(pstContents);
					lToUppermenu = pContext->lToUpper;
					BUS_MGR_Destroy(NULL);

					if (lToUppermenu == CAS_CX_MENU_APP_TOUPPER_ON)
					{
						BUS_MGR_Create((char*)"xproc_cas_CxSmartCardMenu", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xproc_cas_CxSmartCardMenu, 0, 0, 0, 0);
					}
					return MESSAGE_BREAK;

				case KEY_PAGEUP:
				case KEY_CH_PLUS:
				case KEY_ARROWUP:
					if( pContext->ulCurFocus == 0 )
					{
						pContext->ulCurFocus = pContext->ulTotalNum - 1;
					}
					else
					{
						pContext->ulCurFocus --;
					}
					GWM_List_SetCurrentIndex(eMail_MSG_ITEM, pContext->ulCurFocus);
					GWM_APP_InvalidateRect(SYSETM_APP_WINDOW_X, SYSETM_APP_WINDOW_Y,
											SYSETM_APP_WINDOW_WIDTH, SYSETM_APP_WINDOW_HEIGHT);
					return MESSAGE_BREAK;

				case KEY_PAGEDOWN:
				case KEY_CH_MINUS:
				case KEY_ARROWDOWN:
					if( pContext->ulCurFocus == pContext->ulTotalNum - 1 )
					{
						pContext->ulCurFocus = 0;
					}
					else
					{
						pContext->ulCurFocus ++;
					}
					GWM_List_SetCurrentIndex(eMail_MSG_ITEM, pContext->ulCurFocus);
					GWM_APP_InvalidateRect(SYSETM_APP_WINDOW_X, SYSETM_APP_WINDOW_Y,
											SYSETM_APP_WINDOW_WIDTH, SYSETM_APP_WINDOW_HEIGHT);
					return MESSAGE_BREAK;

				case KEY_ARROWRIGHT:
				case KEY_OK:
					if (pContext->ulTotalNum > 0)
					{
						ulCursor = pContext->ulCurFocus;
						lToUppermenu = pContext->lToUpper;
						//BUS_MGR_Destroy(NULL);
						xmgr_cas_CxSetUserTextDisplayed(s_pstCXMailItemData[ulCursor].nMailIdx,eCxUTDisplayCntOnce);
						BUS_MGR_Create("xproc_cas_CxMailMsgText", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xproc_cas_CxMailMsgText, 0, 0, s_pstCXMailItemData[ulCursor].nMailIdx, lToUppermenu);
					}
					return MESSAGE_BREAK;

				case KEY_RED:
				case KEY_GREEN:
				case KEY_YELLOW:
					break;
				default:
					AP_NotAvailable_Show();
					return MESSAGE_BREAK;
			}
			break;

		case eMEVT_BUS_TIMER:
			if(p1 == MENU_TIMEOUT_ID)
			{
				/* reset Title depth */
				Settings_ClearWindowTitle();

				BUS_MGR_Destroy(0);
			}
			return MESSAGE_BREAK;
		#endif

	}

	HxLOG_Info("xproc_cas_CxMailMessage() handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


#define _________________________VIEW_MAIL_TEXT______________________________

STATIC XmgrCas_CxSettingsNaviContext_t *xmgr_cas_CxMailTextGetContentsText(void)
{
	return &s_CXMailMsgTextContent;
}


STATIC HERROR	xmgr_cas_CxMailTextIsValidMailText(HUINT16 ulDuration)
{
	if (ulDuration == 0)
	{
		return	ERR_OK;
	}
	else
	{
		return	ERR_FAIL;
	}
}


STATIC HERROR	xmgr_cas_CxMailTextCheckReadAllMailMessage(void)
{
	HERROR				hError = ERR_FAIL;
	HINT32 				i;
	HUINT32 			ulCnt = 0, ulNumMsg;
	CxUserTextList_t 	pstList;

	HxLOG_Info("xmgr_cas_CxMailMessageCheckReadAllMailMessage() +\n");

//여기에서 초기화하면, MailBox메뉴로 돌아갔을 경우 데이터가 보이지 않는다.
/*

	for( i = 0; i < CX_MAX_USERTEXT_SLOT_NUM; i++ )
	{
		HxSTD_memset(&s_pstCXMailItemData[i], 0, sizeof(XmgrCas_CxMailItemData_t));
	}
*/
	hError = xmgr_cas_CxGetUserText(&ulNumMsg, &pstList);
	if ((hError != ERR_OK) || (ulNumMsg < 1))
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	HxLOG_Print("Mail Load Count = %d\n", ulNumMsg);
	ulCnt = 0;
	for (i=0 ; i< ulNumMsg ; i++)
	{
		hError = xmgr_cas_CxMailMessageIsValidMailMessage(pstList.pstUserText[i].usDuration);
		if ((hError == ERR_OK) && (pstList.pstUserText[i].ucDisplayed == 0) && (strlen(pstList.pstUserText[i].szText) > 0))
		{
			ulCnt++;
		}
	}

	HxLOG_Print("Not read Mail Count = %d\n", ulCnt);
	if (ulCnt > 0)
	{
		return	ERR_FAIL;
	}
	else
	{
		return	ERR_OK;
	}
}


STATIC void	xmgr_cas_CxMailTextLoadMailText(XmgrCas_CxSettingsNaviContext_t *pstContents, HINT32 lSlotIdx, HINT32 lToUpper)
{
	HERROR				hError = ERR_FAIL;
	HUINT32 			ulNumMsg, i, ulCnt =0;
	CxUserTextList_t 	pstList;
	XmgrCas_CxMailItemData_t	*pMailItemData = NULL;

	HxLOG_Info("xmgr_cas_CxMailMessageLoadMailList() +\n");
	VK_memset32(pstContents, 0, sizeof(XmgrCas_CxMailListContent_t));

	pstContents->nCurFocus = lSlotIdx;
	pstContents->lToUpper = lToUpper;

	hError = xmgr_cas_CxGetUserText(&ulNumMsg, &pstList);
	if ((hError != ERR_OK) || (ulNumMsg < 1))
	{
		HxLOG_Critical("\n\n");
		pstContents->ulNumOfMainItem = 0;
	}

	HxLOG_Print("Mail Load Count = %d\n", ulNumMsg);
	ulCnt = 0;
	for (i=0 ; i<ulNumMsg ; i++)
	{
		hError = xmgr_cas_CxMailTextIsValidMailText(pstList.pstUserText[i].usDuration);
		if (hError == ERR_OK && (strlen(pstList.pstUserText[i].szText) > 0))
		{
			//선택한 MailMessage만 control
			if(lSlotIdx != pstList.pstUserText[i].ucSeqNo)
			{
				ulCnt++;
				continue;
			}

			if (pMailItemData == NULL)
			{
				pMailItemData = (XmgrCas_CxMailItemData_t*)OxAP_Malloc(sizeof(XmgrCas_CxMailItemData_t));
			}
			if (pMailItemData == NULL)
			{
				HxLOG_Critical("\n\n");
				continue;
			}

			HxSTD_memset(pMailItemData, 0, sizeof(XmgrCas_CxMailItemData_t));

			pMailItemData->bRead		= pstList.pstUserText[i].ucDisplayed;
			pMailItemData->nMailIdx		= pstList.pstUserText[i].ucSeqNo;
			HxSTD_memcpy(pMailItemData->strMailTxt, pstList.pstUserText[i].szText, CX_MAX_USER_TEXT_LEN);
			pMailItemData->strMailTxt[CX_MAX_USER_TEXT_LEN] = '\0';

			// prevent fix
			snprintf((char*)pMailItemData->strReceivedDate, sizeof(pMailItemData->strReceivedDate), "%02d/%02d", pstList.pstUserText[i].stStartDataTime.stDate.ucDay, pstList.pstUserText[i].stStartDataTime.stDate.ucMonth);
			snprintf((char*)pMailItemData->strReceivedTime, sizeof(pMailItemData->strReceivedTime), "%02d:%02d",
				pstList.pstUserText[i].stStartDataTime.stTime.ucHour, pstList.pstUserText[i].stStartDataTime.stTime.ucMinute);

			HxSTD_memcpy(&s_pstCXMailItemData[ulCnt], pMailItemData, sizeof(XmgrCas_CxMailItemData_t));
			if (pMailItemData != NULL)
			{
				OxAP_Free(pMailItemData);
				pMailItemData = NULL;
			}
			ulCnt++;
		}
	}

	pstContents->ulNumOfMainItem = ulCnt;
	HxLOG_Print("Mail Count = %d\n", ulCnt);

}

// TODO: Blocking....
#if 0 // not use for octo2.0
STATIC BUS_Result_t	xmgr_cas_CxMailTextDrawItems(int lId, OSD_Screen_t *pScreen, FMT_DrawItem_t *pDrawItem)
{
	OSD_Rect_t				stItemRect;
	OSD_Font_t				*pFont = OSD_FONT_Get(eFont_SystemNormal);
	TEXT_DRAW_INFO_t		stTextInfo;

	NOT_USED_PARAM(lId);

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


STATIC BUS_Result_t	xmgr_cas_CxMailTextDrawScroll(	OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, FMT_OBJECT pObject)
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

	NOT_USED_PARAM(pArea);

	GWM_FMT_GetProperty(pObject, &ulAttrib, &ulStartIndex, &ulLinePerPage, &ulItemNum);
	if (ulItemNum <= ulLinePerPage)
	{
		return ERR_BUS_NO_ERROR;
	}

	if (AP_StockImg_GetImage(e166_2_00_Scroll_W1_1T, &stThumbHeadImg))
	{
		return ERR_BUS_NO_ERROR;
	}
	if (AP_StockImg_GetImage(e166_2_00_Scroll_W1_2M, &stThumbBodyImg))
	{
		return ERR_BUS_NO_ERROR;
	}
	if (AP_StockImg_GetImage(e166_2_00_Scroll_W1_3B, &stThumbTailImg))
	{
		return ERR_BUS_NO_ERROR;
	}
	if (AP_StockImg_GetImage(e166_2_00_Scroll_W1_4BG, &stBgImg))
	{
		return ERR_BUS_NO_ERROR;
	}

	lBgHeight = stBgImg.height;

	lX = pRect->x + pRect->w - CAS_CX_MAIL_SCROLL_GAP_W;
	lY = pRect->y + CAS_CX_MAIL_SCROLL_GAP_Y;
	lH = pRect->h - CAS_CX_MAIL_SCROLL_GAP_H;

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


STATIC BUS_Result_t	xmgr_cas_CxMailTextDrawTitle(OSD_Screen_t *screen, OSD_Area_t *area,
										OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1)
{
	OSD_Font_t			*pFont = NULL;
	OSD_StockImage_t	Img;
	OSD_Rect_t			Rect;
	HUINT8				*buf = GetTextBuffer(TEXT_SIZE512);
	HUINT8				*bufDate = GetTextBuffer(TEXT_SIZE64);
	HUINT8				*bufTime = GetTextBuffer(TEXT_SIZE64);
	HUINT8				i;
	XmgrCas_CxSettingsNaviContext_t	*pstContents = NULL;

	NOT_USED_PARAM(param1);
	NOT_USED_PARAM(arg);
	NOT_USED_PARAM(object);
	NOT_USED_PARAM(area);

	pstContents = xmgr_cas_CxMailTextGetContentsText();

	// Icon
	if (!AP_StockImg_GetImage (e178_3_25_Icon_Mail_Open, &Img))
	{
		OSD_GFX_SetRect (&Rect, rect->x , rect->y, Img.width, Img.height);
		OSD_GFX_FB_Blend (Img.hBuffer, Img.width, Img.height, screen, &Rect);
	}

	pFont = OSD_FONT_Get(eFont_SystemNormal);

	for( i = 0; i < pstContents->ulNumOfMainItem; i++ )
	{
		if (s_pstCXMailItemData[i].nMailIdx == pstContents->nCurFocus)
		{
			snprintf((char*)bufDate, CX_MAX_USER_TEXT_LEN, "%s", s_pstCXMailItemData[i].strReceivedDate);	// prevent fix
			snprintf((char*)bufTime, CX_MAX_USER_TEXT_LEN, "%s", s_pstCXMailItemData[i].strReceivedTime);	// prevent fix
			break;
		}
	}

	if (OSD_FONT_SetSize(pFont, FONT_SIZE_M) == ERR_OK)
	{
		// Date
		HxSTD_memset (buf, 0, TEXT_SIZE512);
		MWC_UTIL_DvbStrcpy(buf, bufDate);
		OSD_GFX_SetRect(&Rect, rect->x + Img.width + CAS_CX_MAIL_TITLE_DATE_GAP_W, rect->y + CAS_CX_MAIL_TITLE_DATE_GAP_H, 0, 0);
		OSD_GFX_DrawString(screen, pFont, Rect.x, Rect.y, (HUINT8*)buf, COL(C_T_Cursor));

		// Time
		Rect.w = OSD_GFX_GetStringWidth(pFont, (HUINT8*)buf);

		HxSTD_memset (buf, 0, TEXT_SIZE512);
		MWC_UTIL_DvbStrcpy(buf, bufTime);
		OSD_GFX_SetRect(&Rect, Rect.x + Rect.w + CAS_CX_MAIL_TITLE_TIME_GAP_W, rect->y + CAS_CX_MAIL_TITLE_DATE_GAP_Y, 0, 0);
		OSD_GFX_DrawString(screen, pFont, Rect.x, Rect.y, (HUINT8*)buf, COL(C_T_Cursor));
	}

	OSD_FONT_Release(pFont);

	return ERR_BUS_NO_ERROR;

}


STATIC void	xmgr_cas_CxMailTextDrawText(XmgrCas_CxSettingsNaviContext_t *pstContents)
{
	HUINT8				*pBuf1 = NULL;
	HUINT8				*pTemp = NULL;
	HUINT8				*pBuf2 = GetTextBuffer(256);
	HUINT32				nNumOfChForOneLine;
	OSD_Font_t			*pFont = OSD_FONT_Get(eFont_SystemNormal);
	OSD_Rect_t			stRect = {0, };
	eHxCP_CodePage_e		encoding;
	Text_Property_t 	stTextProperty;
	HUINT8				i;
	HUINT8				locCode[3]={0,};
	HINT32				locLen=0;

	pBuf1 = (HUINT8 *)OxAP_Malloc(CX_MAX_USER_TEXT_LEN * sizeof(HUINT8));
	pTemp = pBuf1;

	// set, draw background
	Settings_DrawBack(FALSE);

	// title
	MWC_UTIL_DvbStrcpy(pstContents->title, AP_CasResStr_GetStrRsc(LOC_CAS_CX_MAIL_MSG_TITLE_ID));
	Settings_SetWindowTitle(SETTING_GET_AP_DEPTH(eSetting_CasCxMailMsg), LOC_CAS_CX_MAIL_MSG_TITLE_ID, pstContents->title);

	// FTM Text Box
	if((pstContents->ulNumOfMainItem > 0))
	{
		//draw icon & date & time
		OSD_GFX_SetRect (&stRect, CAS_CX_MAIL_TITLE_X, CAS_CX_MAIL_TITLE_Y, CAS_CX_MAIL_TITLE_W, CAS_CX_MAIL_TITLE_H);
		GWM_Frame_Create (eMail_MSG_ICON_ID, &stRect, (char *)NULL);
		GWM_APP_SetObjectDrawMethod (eMail_MSG_ICON_ID, xmgr_cas_CxMailTextDrawTitle);

		// BOX
		OSD_GFX_SetRect(&stRect, CAS_CX_MAIL_BOX_X, CAS_CX_MAIL_BOX_Y, CAS_CX_MAIL_BOX_W, CAS_CX_MAIL_BOX_H);
		GWM_Rect_Create(eMail_MSG_BOX_ID, &stRect, CAS_CX_MAIL_BOX_CREATE_COL);
		GWM_APP_SetObjectDrawMethod(eMail_MSG_BOX_ID, AP_LNF_DrawBoxD);

		// FMT Text
		OSD_GFX_SetRect(&stRect, CAS_CX_MAIL_FMT_X, CAS_CX_MAIL_FMT_Y, CAS_CX_MAIL_FMT_W, CAS_CX_MAIL_FMT_H);
		GWM_FMT_Create(eMail_MSG_FMT_ID, &stRect, FMT_OWN_DRAWITEM | FMT_SCROLL_ON, 12);

		stTextProperty.ulFontSize	= FONT_SIZE_S;
		stTextProperty.ulFontAlign	= TEXT_ALIGN_LEFT;
		stTextProperty.cFontColor	= COL(C_T_Enable);

		HxSTD_memset(pBuf1, 0, CX_MAX_USER_TEXT_LEN);

		//for( i = 0; i < pstContents->ulNumOfMainItem; i++ )
		for( i = 0; i < CX_MAX_USERTEXT_SLOT_NUM; i++ )
		{
			if (s_pstCXMailItemData[i].nMailIdx == pstContents->nCurFocus)
			{
				snprintf((char*)pBuf1, CX_MAX_USER_TEXT_LEN, "%s", s_pstCXMailItemData[i].strMailTxt);	// prevent fix
				break;
			}
		}

		encoding= MWC_UTIL_GetEncodingByLocaleCode((const HUINT8 *)pBuf1);
		locLen	= (HINT32)MWC_UTIL_GetLocaleCode((const HUINT8 *)pBuf1, locCode);
		pBuf1 += locLen;

		do
		{
			HxSTD_memset(pBuf2, 0, 256);

			OSD_FONT_CountChByWidth_SingleByte( pFont, stRect.w, encoding, strlen(pBuf1), (char*)pBuf1, &nNumOfChForOneLine);

			MWC_UTIL_DvbStrncpy(pBuf2, locCode, locLen);
			MWC_UTIL_DvbStrncat(pBuf2+locLen, pBuf1, nNumOfChForOneLine);
			pBuf1 += nNumOfChForOneLine;

			GWM_FMT_AddItem(eMail_MSG_FMT_ID, (char *)pBuf2, &stTextProperty);

		} while(nNumOfChForOneLine != 0);

		GWM_FMT_ChangeItemDrawMethod(eMail_MSG_FMT_ID, xmgr_cas_CxMailTextDrawItems);
		GWM_FMT_ChangeScrollDrawMethod(eMail_MSG_FMT_ID, xmgr_cas_CxMailTextDrawScroll);

		GWM_Obj_SetFocus(eMail_MSG_FMT_ID);

	}

	OxAP_Free(pTemp);

	OSD_FONT_Release(pFont);

}


STATIC HERROR	xmgr_cas_CxMailTextDestroyUIText(XmgrCas_CxSettingsNaviContext_t *pstContents)
{
	HERROR		hError = ERR_FAIL;
	/* if dynamic allocated data are, free */
	AP_NaviCntxt_FreeMainItemArray(pstContents);

	return ERR_OK;
}
#endif
STATIC void	xmgr_cas_CxMailTextRemoveMailIconOnAV(void)
{
	HERROR					hError = ERR_FAIL;

	hError = xmgr_cas_CxMailTextCheckReadAllMailMessage();
	if (hError == ERR_OK)
	{
		#if 0
		if (BUS_MGR_Get(xproc_cas_CxMainIcon) != NULL)
		{
			BUS_MGR_Destroy(xproc_cas_CxMainIcon);
		}
		#endif
	}
}

BUS_Result_t	xproc_cas_CxMailMsgText(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
//	HUINT32 				errResult = ERR_FAIL;
//	HINT32					lToUppermenu =CAS_CX_MENU_APP_TOUPPER_ON;
	XmgrCas_CxSettingsNaviContext_t	*pNaviContext = NULL;

	HxLOG_Info("xproc_cas_CxMailMsgText() message(0x%x)\n", message);

	/* get running param */
	pNaviContext = xmgr_cas_CxMailTextGetContentsText();

	switch(message)
	{
		case eMEVT_BUS_CREATE:
			// update cas information
			xmgr_cas_CxMailTextLoadMailText(pNaviContext, p2, p3);
			// TODO: Blocking...
			#if 0 // not use for octo2.0
			/* init application area */
			GWM_APP_SetAppArea(SYSETM_APP_WINDOW_X, SYSETM_APP_WINDOW_Y, SYSETM_APP_WINDOW_WIDTH, SYSETM_APP_WINDOW_HEIGHT);

			// update cas information
			xmgr_cas_CxMailTextLoadMailText(pNaviContext, p2, p3);
			// draw data
			if(pNaviContext->ulNumOfMainItem != 0)
			{
				xmgr_cas_CxMailTextDrawText(pNaviContext);

				// 읽지 않는 메시지 체크한 후, Mail Icon Destroy 결정
				xmgr_cas_CxMailTextRemoveMailIconOnAV();
			}

			/* set front pannel */
			AP_SPEC_DisplayMenuPanelString(pNaviContext->title, eDxPANEL_ALIGN_CENTER);
			#endif
			break;
		// TODO: Blocking...
		#if 0 // not use for octo2.0
		case MSG_BUS_KEYUP:
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_BUS_KEYDOWN:
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
				case KEY_STANDBY:
				case KEY_VOLUMEUP:
				case KEY_VOLUMEDOWN:
				case KEY_MUTE:
					return MESSAGE_PASS;

				case KEY_EXIT:
				case KEY_MENU:
					/* reset Title depth */
					Settings_ClearWindowTitle();

					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				case KEY_ARROWLEFT:
				case KEY_OK:
				case KEY_BACK:
					lToUppermenu = pNaviContext->lToUpper;
					BUS_MGR_Destroy(NULL);
					//s_IrMailCurFocus = 0;
					/* to upper depth */
					BUS_MGR_Create("xproc_cas_CxMailMessage", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xproc_cas_CxMailMessage, 0, 0, lToUppermenu, 0);
					return MESSAGE_BREAK;

				case	KEY_ARROWUP:
				case	KEY_ARROWDOWN:
					break;

				default:
					AP_NotAvailable_Show();
					return MESSAGE_BREAK;
			}
			break;

		case eMEVT_BUS_TIMER:
			if(p1 == MENU_TIMEOUT_ID)
			{
				/* reset Title depth */
				Settings_ClearWindowTitle();
				BUS_MGR_Destroy(0);
			}
			return MESSAGE_BREAK;
		#endif
		case XMGR_CAS_UPDATE_MENU:
			// TODO: Blocking...
			#if 0 // not use for octo2.0

			if (p3 == eCxUi_GroupMessage_Display)
			{
				BUS_MGR_Destroy(NULL);
				BUS_MGR_Create("xproc_cas_CxMailMessage", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xproc_cas_CxMailMessage, 0, 0, 0, 0);
			}
			#endif
			break;

		case eMEVT_BUS_DESTROY:
			// TODO: Blocking...
			#if 0 // not use for octo2.0
			/* destroy UI */
			xmgr_cas_CxMailTextDestroyUIText(pNaviContext);
			GWM_FMT_Delete(eMail_MSG_FMT_ID);
			GWM_Obj_Destroy(eMail_MSG_FMT_ID);
			#endif
			/* kill timer */
			BUS_KillTimer(MENU_TIMEOUT_ID);

			/* clear front PANEL */
			//AP_SPEC_ClearMenuPanelString();
			break;

		default:
			break;
	}

	HxLOG_Info("xproc_cas_CxMailMsgText() handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


#endif // eEnd of defined(CONFIG_MW_CAS_CONAX_TEXT)


#define _________________UI__________________________________

STATIC XmgrCas_CxStatusListContent_t	*xmgr_cas_CxStatusListGetLocalContents(void)
{
	return	&s_CasCxStatusListContents;
}

// TODO: Blocking....
#if 0 // not use for octo2.0
STATIC BUS_Result_t		xmgr_cas_CxStatusListDestroyUI(XmgrCas_CxStatusListContent_t *pstContents)
{
	/* free mainitem */
	if(pstContents->subItems != NULL)
	{
		OxAP_Free(pstContents->subItems);
		pstContents->subItems = NULL;
	}


	return ERR_OK;
}
#endif

#define	________________Action_Func________________________
// TODO: Blocking....

STATIC BUS_Result_t		xmgr_cas_CxStatusListMsgGwmListSelected(XmgrCas_CxStatusListContent_t	*pstContents, HINT32 lIndex)
{
	HUINT8		*szTmp = NULL;
	HERROR		hError = ERR_FAIL;
	HUINT8		szStr[CAX_CX_MAX_ITEM_STRLEN];
	HINT32		lRef;

	if (pstContents->lCasCxCAItemID == eCasCXMenuItem_Tokens)
	{
		// Get Select Item Data - Purse Debit Status
		hError = CAS_UTIL_LL_ReadIndexNthItem(&(pstContents->stTextListBox), lIndex, (void**)&szTmp);
		if (hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
			return	ERR_BUS_NO_ERROR;
		}

		// Label
		xmgr_cas_CxGetWordFromString(szStr, szTmp, 0);
		MWC_UTIL_DvbStrcpy(pstContents->stPurseStatus.szLabel, szStr);

		// Reference Value
		xmgr_cas_CxGetWordFromString(szStr, szTmp, 2);
		lRef = atoi((char*)szStr);
		pstContents->stPurseStatus.usPurseRef = (HUINT16)lRef;

		// Init Data
		pstContents->lCasCxCAItemID = eCasCXMenuItem_Debit;

		xmgr_cas_CxStatusListInitListTitle(pstContents);
		xmgr_cas_CxStatusListFreeItemData(pstContents);
		xmgr_cas_CxStatusListResetLists(pstContents);

	//	GWM_Obj_Destroy(CAS_CX_STATUSLIST_LIST_ID);
	//	GWM_Obj_Destroy(CAS_CX_STATUSLIST_TEXTLIST_TITLE01_ID);
	//	GWM_Obj_Destroy(CAS_CX_STATUSLIST_TEXTLIST_TITLE02_ID);

		xmgr_cas_CxStatusListLoadItem_DebitData(pstContents, &pstContents->stPurseStatus);

		/* send to ui */
		xmgr_cas_CxStatusListSendData(pstContents);
	//	xmgr_cas_CxStatusListDrawDefaultUI(pstContents);
	}
	else if (pstContents->lCasCxCAItemID == eCasCXMenuItem_Debit)
	{
		// Init Data - Purse Credit Status
		pstContents->lCasCxCAItemID = eCasCXMenuItem_Credit;

		xmgr_cas_CxStatusListInitListTitle(pstContents);
		xmgr_cas_CxStatusListFreeItemData(pstContents);
		xmgr_cas_CxStatusListResetLists(pstContents);

	//	GWM_Obj_Destroy(CAS_CX_STATUSLIST_LIST_ID);
	//	GWM_Obj_Destroy(CAS_CX_STATUSLIST_TEXTLIST_TITLE01_ID);
	//	GWM_Obj_Destroy(CAS_CX_STATUSLIST_TEXTLIST_TITLE02_ID);
	//	GWM_Obj_Destroy(CAS_CX_STATUSLIST_TEXTLIST_TITLE03_ID);

		xmgr_cas_CxStatusListLoadItem_CreditData(pstContents, &pstContents->stPurseStatus);

		/* send to ui */
		xmgr_cas_CxStatusListSendData(pstContents);

	//	xmgr_cas_CxStatusListDrawDefaultUI(pstContents);
	}
	else
	{
		HxLOG_Print("==[xmgr_cas_CxStatusListMsgGwmListSelected]== This is not select Menu Item.\n");
	}

	return	ERR_BUS_NO_ERROR;
}

#define	________________Init_Func____________________________
STATIC HERROR	xmgr_cas_CxStatusListInitMenuTitle(XmgrCas_CxStatusListContent_t *pstContents)
{

	switch (pstContents->lCasCxCAItemID)
	{
		case eCasCXMenuItem_Subscription:
			MWC_UTIL_DvbStrcpy(pstContents->szTitle, AP_CasResStr_GetStrRsc(STR_SUBSCRIPTION_STATUS_ID));
			break;

		case eCasCXMenuItem_Event:
			MWC_UTIL_DvbStrcpy(pstContents->szTitle, AP_CasResStr_GetStrRsc(STR_EVENT_STATUS_ID));
			break;

		case eCasCXMenuItem_Tokens:
			MWC_UTIL_DvbStrcpy(pstContents->szTitle, AP_CasResStr_GetStrRsc(STR_TOKENS_STATUS_ID));
			break;

		case eCasCXMenuItem_Debit:
			MWC_UTIL_DvbStrcpy(pstContents->szTitle, AP_CasResStr_GetStrRsc(STR_PURSE_DEBIT_STATUS_ID));
			break;

		case eCasCXMenuItem_Credit:
			MWC_UTIL_DvbStrcpy(pstContents->szTitle, AP_CasResStr_GetStrRsc(STR_PURSE_CREDIT_STATUS_ID));
			break;

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
		case eCasCXMenuItem_Text_View:
			MWC_UTIL_DvbStrcpy(pstContents->szTitle, AP_CasResStr_GetStrRsc(STR_MESSAGES_ID));
			break;
#endif
		default:
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
	}

	return ERR_OK;
}
STATIC BUS_Result_t	xmgr_cas_CxStatusListInitStatusList(XmgrCas_CxStatusListContent_t *pstContents, HINT32 p1, HINT32 lToUpper, HINT32 lItemObjID)
{
	HxLOG_Print("[Enter]\r\n");
	pstContents->lCasCxCAItemID = lItemObjID;
	pstContents->pfnCallerCallback = xproc_cas_CxSmartCardMenu;//(BUS_Callback_t)p1;
	xmgr_cas_CxStatusListInitMenuTitle(pstContents);
	xmgr_cas_CxStatusListInitListTitle(pstContents);
	xmgr_cas_CxStatusListInitListItems(pstContents);
	xmgr_cas_CxStatusListLoadItemData(pstContents);
	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxStatusListInitListTitle(XmgrCas_CxStatusListContent_t *pstContents)
{
	HxLOG_Print("[Enter]\r\n");
//	HUINT8		szColumnType[TEXT_SIZE128];
//	HxSTD_memset(szColumnType, 0, TEXT_SIZE128);
	memset(pstContents->szColumnTitle, 0, MAX_ELEMENT_TEXT_LENGTH);
	switch (pstContents->lCasCxCAItemID)
	{
		case eCasCXMenuItem_Subscription:
			//MWC_UTIL_DvbUtf8Sprintf(pstContents->szColumnTitle, (HUINT8*)"%s %s %s", (HUINT8*)AP_CasResStr_GetStrRsc(STR_NAME_ID),
			//	(HUINT8*)AP_CasResStr_GetStrRsc(STR_START_DATE_ID), (HUINT8*)AP_CasResStr_GetStrRsc(STR_END_DATE_ID));
			HxSTD_snprintf(pstContents->szColumnTitle, sizeof(pstContents->szColumnTitle), (HUINT8*)"%s %s %s", (HUINT8*)AP_CasResStr_GetStrRsc(STR_NAME_ID),
				(HUINT8*)AP_CasResStr_GetStrRsc(STR_START_DATE_ID), (HUINT8*)AP_CasResStr_GetStrRsc(STR_END_DATE_ID));

			break;

		case eCasCXMenuItem_Event:
			HxSTD_snprintf(pstContents->szColumnTitle, sizeof(pstContents->szColumnTitle), (HUINT8*)"%s %s/%s %s", (HUINT8 *)AP_CasResStr_GetStrRsc(STR_NAME_ID),
				(HUINT8 *)AP_CasResStr_GetStrRsc(STR_START_DATE_ID), (HUINT8 *)AP_CasResStr_GetStrRsc(STR_END_DATE_ID),
				(HUINT8 *)AP_CasResStr_GetStrRsc(STR_MINUTES_CREDIT_ID));
			break;

		case eCasCXMenuItem_Tokens:
			HxSTD_snprintf(pstContents->szColumnTitle, sizeof(pstContents->szColumnTitle), (HUINT8*)"%s %s", (HUINT8 *)AP_CasResStr_GetStrRsc(STR_PURSE_ID),
				(HUINT8 *)AP_CasResStr_GetStrRsc(STR_BALANCETOKENS_ID));
			break;

		case eCasCXMenuItem_Debit:
			HxSTD_snprintf(pstContents->szColumnTitle, sizeof(pstContents->szColumnTitle), (HUINT8*)"%s %s %s", (HUINT8 *)AP_CasResStr_GetStrRsc(STR_EVENT_ID),
				(HUINT8 *)AP_CasResStr_GetStrRsc(STR_TOKENS_ID), (HUINT8 *)AP_CasResStr_GetStrRsc(STR_DATE_ID));
			break;

		case eCasCXMenuItem_Credit:
			HxSTD_snprintf(pstContents->szColumnTitle, sizeof(pstContents->szColumnTitle), (HUINT8*)"%s %s", (HUINT8 *)AP_CasResStr_GetStrRsc(STR_LABEL_ID),
				(HUINT8 *)AP_CasResStr_GetStrRsc(STR_TOKENS_ID));
			break;

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
		case eCasCXMenuItem_Text_View:
			HxSTD_snprintf(pstContents->szColumnTitle, sizeof(pstContents->szColumnTitle), (HUINT8*)"%s  %s", (HUINT8 *)AP_CasResStr_GetStrRsc(STR_SLOT_ID),
							(HUINT8 *)AP_CasResStr_GetStrRsc(STR_TEXT_ID));
			break;
#endif	// End of defined(CONFIG_MW_CAS_CONAX_TEXT)
		default:
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
	}

	//HxSTD_memcpy(pstContents->szColumnTitle, szColumnType, sizeof(TEXT_SIZE128));
	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}

STATIC void	xmgr_cas_CxStatusListInitListItems(XmgrCas_CxStatusListContent_t *pstContents)
{
	//pstContents->ulCurFocus = 0;
	pstContents->ulTotalNum = 0;

	CAS_UTIL_LL_InitList(&(pstContents->stTextListBox));
}


STATIC HERROR	xmgr_cas_CxStatusListResetLists(XmgrCas_CxStatusListContent_t *pstContents)
{
	/* tmp move list들은 current grp에 대해서만 사용되므로
		이미 curList를 edit완료로 보고 list만 초기화 하고, content는 지우면 안된다. */

	CAS_UTIL_LL_SetListEmpty(&(pstContents->stTextListBox), (ll_deleteitem_method)NULL);

	return ERR_OK;
}

STATIC HERROR	xmgr_cas_CxStatusListLoadItemData(XmgrCas_CxStatusListContent_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

	HxLOG_Print("[Enter]\r\n");

	switch (pstContents->lCasCxCAItemID)
	{
		case eCasCXMenuItem_Subscription:
			hError = xmgr_cas_CxStatusListLoadItem_SubscriptionData(pstContents);
			break;

		case eCasCXMenuItem_Event:
			hError = xmgr_cas_CxStatusListLoadItem_EventData(pstContents);
			break;

		case eCasCXMenuItem_Tokens:
			hError = xmgr_cas_CxStatusListLoadItem_TokensData(pstContents);
			break;

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
		case eCasCXMenuItem_Text_View:
			hError = xmgr_cas_CxStatusListLoadItem_MessagesData(pstContents);
			break;
#endif
		default:
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
	}

	HxLOG_Print("[Exit]\r\n");

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxStatusListLoadItem_SubscriptionData(XmgrCas_CxStatusListContent_t *pstContents)
{
	CxStatusList_t *pstStatusList = NULL, *pstStatusListTmp = NULL;
	CxStatusList_t 	stList;
	pstStatusList = &stList;
	HERROR		hError = ERR_FAIL;
	HUINT8		*szText = NULL, szStartDate[TEXT_SIZE64] = {0,}, szEndDate[TEXT_SIZE64] = {0,};
	HUINT8		*pucEntitlement;
	HUINT8		szTemp[TEXT_SIZE128] = {0,}, szStr1[TEXT_SIZE128] = {0,};
	HxDATETIME_Date_t	stStartDate, stEndDate;

	HxLOG_Print("[Enter]\r\n");

// TODO: Blocking....
	// Dialog Create : Please Wait

	// Get Status Info - Subscription
	pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllSubscription, CX_SC_STATUS_REF);
	if (pstStatusList == NULL)
	{
		// Retry Get Status Info - Subscription
		pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllSubscription, CX_SC_STATUS_REF);
		if (pstStatusList == NULL)
		{
			HxLOG_Print("==[xmgr_cas_CxStatusListLoadItem_SubscriptionData]== xmgr_cas_CxReqStatusInfo() Empty Data .\n");
			return ERR_FAIL;
		}
	}

	pstStatusListTmp = pstStatusList;
	HxLOG_Print("\t=====================================\n");
	HxLOG_Print("\t Name         Start Date / End Date \n");

	/*
		============ CONAX Spec ============
		- Four octer_str_byte : Displayed in hex nataion
		- Subscription_ref : Not be displayed
		- Supoort a minimun of 40 entries.
	*/

	// make String
	while (pstStatusListTmp)
	{
		/* __________________________________________________________________*/
		// Label
		MWC_UTIL_DvbStrcpy(szStr1, pstStatusListTmp->stStatus.stSubscription.szLabel);

		// Start Date
		stStartDate.usYear = pstStatusListTmp->stStatus.stSubscription.stStartDate[0].usYear;
		stStartDate.ucMonth = pstStatusListTmp->stStatus.stSubscription.stStartDate[0].ucMonth;
		stStartDate.ucDay = pstStatusListTmp->stStatus.stSubscription.stStartDate[0].ucDay;

		// Get Start Date Month String
		//resResult = AP_UTIL_GetMonthString(0, stStartDate.ucMonth, &szStartDate);
		hError = xmgr_cas_CxGetMonthString(stStartDate.ucMonth, szStartDate);
		if( hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}

		// End Date
		stEndDate.usYear = pstStatusListTmp->stStatus.stSubscription.stEndDate[0].usYear;
		stEndDate.ucMonth = pstStatusListTmp->stStatus.stSubscription.stEndDate[0].ucMonth;
		stEndDate.ucDay = pstStatusListTmp->stStatus.stSubscription.stEndDate[0].ucDay;

		// Get End Date Month String
		//resResult = AP_UTIL_GetMonthString(0, stEndDate.ucMonth, &szEndDate);
		hError = xmgr_cas_CxGetMonthString(stEndDate.ucMonth, szEndDate);
		if( hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}

		// Entitlement 1
		pucEntitlement = pstStatusListTmp->stStatus.stSubscription.szEntitlemnet[0];

		// 1ST Info
		//MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%02d %s %04d|%02d %s %04d|%02X%02X%02X%02X",
		//		szStr1, stStartDate.ucDay, szStartDate, stStartDate.usYear, stEndDate.ucDay, szEndDate, stEndDate.usYear,
		//		pucEntitlement[0], pucEntitlement[1], pucEntitlement[2], pucEntitlement[3]);
		HxSTD_snprintf(szTemp, sizeof(szTemp), (HUINT8*)"%s|%02d %s %04d|%02d %s %04d|%02X%02X%02X%02X",
				szStr1, stStartDate.ucDay, szStartDate, stStartDate.usYear, stEndDate.ucDay, szEndDate, stEndDate.usYear,
				pucEntitlement[0], pucEntitlement[1], pucEntitlement[2], pucEntitlement[3]);
		szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(szText, szTemp);

		HxLOG_Print("1\t %s|%02d %s %04d|%02d %s %04d|%02X%02X%02X%02X\n", szStr1, stStartDate.ucDay, szStartDate, stStartDate.usYear, stEndDate.ucDay, szEndDate, stEndDate.usYear,
				pucEntitlement[0], pucEntitlement[1], pucEntitlement[2], pucEntitlement[3]);
		/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
		if(hError != ERR_OK)
		{
			/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		/* __________________________________________________________________*/
		szText = NULL;

		/* __________________________________________________________________*/
		// Start Date
		stStartDate.usYear = pstStatusListTmp->stStatus.stSubscription.stStartDate[1].usYear;
		stStartDate.ucMonth = pstStatusListTmp->stStatus.stSubscription.stStartDate[1].ucMonth;
		stStartDate.ucDay = pstStatusListTmp->stStatus.stSubscription.stStartDate[1].ucDay;

		// Get Start Date Month String
		//resResult = AP_UTIL_GetMonthString(0, stStartDate.ucMonth, &szStartDate);
		hError = xmgr_cas_CxGetMonthString(stStartDate.ucMonth, szStartDate);
		if( hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}

		// End Date
		stEndDate.usYear = pstStatusListTmp->stStatus.stSubscription.stEndDate[1].usYear;
		stEndDate.ucMonth = pstStatusListTmp->stStatus.stSubscription.stEndDate[1].ucMonth;
		stEndDate.ucDay = pstStatusListTmp->stStatus.stSubscription.stEndDate[1].ucDay;

		// Get End Date Month String
		//resResult = AP_UTIL_GetMonthString(0, stEndDate.ucMonth, &szEndDate);
		hError = xmgr_cas_CxGetMonthString(stEndDate.ucMonth, szEndDate);
		if( hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}

		// Entitlement 2
		pucEntitlement = pstStatusListTmp->stStatus.stSubscription.szEntitlemnet[1];

		// 2nd Info
		#if 0
		MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%s|%02d %s %04d|%02d %s %04d|%02X%02X%02X%02X",
				(HUINT8*)"  ", stStartDate.ucDay, szStartDate, stStartDate.usYear, stEndDate.ucDay, szEndDate, stEndDate.usYear,
				pucEntitlement[0], pucEntitlement[1], pucEntitlement[2], pucEntitlement[3]);
		#else
		HxSTD_snprintf(szTemp, sizeof(szTemp), (HUINT8*)"%s|%02d %s %04d|%02d %s %04d|%02X%02X%02X%02X",
				(HUINT8*)"  ", stStartDate.ucDay, szStartDate, stStartDate.usYear, stEndDate.ucDay, szEndDate, stEndDate.usYear,
				pucEntitlement[0], pucEntitlement[1], pucEntitlement[2], pucEntitlement[3]);
		#endif
		szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(szText, szTemp);

		HxLOG_Print("\t %s|%02d %s %04d|%02d %s %04d|%02X%02X%02X%02X\n", (HUINT8*)"  ", stStartDate.ucDay, szStartDate, stStartDate.usYear, stEndDate.ucDay, szEndDate, stEndDate.usYear,
				pucEntitlement[0], pucEntitlement[1], pucEntitlement[2], pucEntitlement[3]);
		/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
		if(hError != ERR_OK)
		{
			/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}
		/* __________________________________________________________________*/

		szText = NULL;

		pstStatusListTmp = pstStatusListTmp->pstNext;
	}

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxStatusListLoadItem_EventData(XmgrCas_CxStatusListContent_t *pstContents)
{
	CxStatusList_t *pstStatusList = NULL, *pstStatusListTmp = NULL;
	CxStatusList_t 	stList;
	pstStatusList = &stList;
	HERROR		hError = ERR_FAIL;
	HUINT8		*szText = NULL, szStartDate[TEXT_SIZE64] = {0,}, szEndDate[TEXT_SIZE64] = {0,};
	HUINT8		szTemp[TEXT_SIZE128] = {0,}, szStr1[TEXT_SIZE128] = {0,}, szStr2[TEXT_SIZE128] = {0,};
	HxDATETIME_Date_t	stStartDate, stEndDate;
	HxDATETIME_Time_t	stSTTime, stEDTime;

	HxLOG_Print("[Enter]\r\n");

	// Dialog Create : Please Wait

	// Get Status Info - Event
	pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllPpvEvent, CX_SC_STATUS_REF);
	if (pstStatusList == NULL)
	{
		// Retry Get Status Info - Event
		pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllPpvEvent, CX_SC_STATUS_REF);
		if (pstStatusList == NULL)
		{
			HxLOG_Print("==[xmgr_cas_CxStatusListLoadItem_EventData]== xmgr_cas_CxReqStatusInfo() Empty Data .\n");
			return ERR_FAIL;
		}
	}

	pstStatusListTmp = pstStatusList;

	/*
		============ CONAX Spec ============
		- auth_type : 0x00(Time Limited Access), 0x01(Unlimited Access), 0x02(Crdeit base Access)
		- Support a minimun of 100 entries.
	*/
	// make String
	while (pstStatusListTmp)
	{
		// Label
		MWC_UTIL_DvbStrcpy(szStr1, pstStatusListTmp->stStatus.stEvent.szLabel);

		// Start Date
		stStartDate.usYear = pstStatusListTmp->stStatus.stEvent.stStartDate.usYear;
		stStartDate.ucMonth = pstStatusListTmp->stStatus.stEvent.stStartDate.ucMonth;
		stStartDate.ucDay = pstStatusListTmp->stStatus.stEvent.stStartDate.ucDay;
		stSTTime.ucHour = pstStatusListTmp->stStatus.stEvent.stStartDate.ucHour;
		stSTTime.ucMinute = pstStatusListTmp->stStatus.stEvent.stStartDate.ucMinute;
		// Get Start Date Month String
		//resResult = AP_UTIL_GetMonthString(0, stStartDate.ucMonth, &szStartDate);
		hError = xmgr_cas_CxGetMonthString(stStartDate.ucMonth, szStartDate);
		if( hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}
		// Minutes of Credits Left
		if (pstStatusListTmp->stStatus.stEvent.ucAuthType == eCxAuth_TimeLimitedAccess)
		{
			HxSTD_snprintf(szStr2, sizeof(szStr2), (HUINT8*)"%d %s", pstStatusListTmp->stStatus.stEvent.usLeft, (HUINT8 *)AP_CasResStr_GetStrRsc(STR_MINUTES_LEFT_ID));
		}
		else if (pstStatusListTmp->stStatus.stEvent.ucAuthType == eCxAuth_CreditBasedAccess)
		{
			HxSTD_snprintf(szStr2, sizeof(szStr2), (HUINT8*)"%d", pstStatusListTmp->stStatus.stEvent.usLeft);
		}
		else
		{
			// Unlimited Access
			szStr2[0] = 0 ;
		}
		HxSTD_snprintf(szTemp, sizeof(szTemp), (HUINT8*)"%s|%02d.%s.%04d %02d:%02d|%s",
				szStr1,stStartDate.ucDay, szStartDate, stStartDate.usYear, stSTTime.ucHour, stSTTime.ucMinute, szStr2);
		szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(szText, szTemp);
		/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
		if(hError != ERR_OK)
		{
			/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}
		/* ____________________________________________________________________ */
		// End Date
		stEndDate.usYear = pstStatusListTmp->stStatus.stEvent.stEndDate.usYear;
		stEndDate.ucMonth = pstStatusListTmp->stStatus.stEvent.stEndDate.ucMonth;
		stEndDate.ucDay = pstStatusListTmp->stStatus.stEvent.stEndDate.ucDay;
		stEDTime.ucHour = pstStatusListTmp->stStatus.stEvent.stEndDate.ucHour;
		stEDTime.ucMinute = pstStatusListTmp->stStatus.stEvent.stEndDate.ucMinute;

		// Get End Date Month String
		//resResult = AP_UTIL_GetMonthString(0, stEndDate.ucMonth, &szEndDate);
		hError = xmgr_cas_CxGetMonthString(stEndDate.ucMonth, szEndDate);
		if( hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}
		HxSTD_snprintf(szTemp, sizeof(szTemp), (HUINT8*)"%s|%02d.%s.%04d %02d:%02d|%s", (HUINT8*)" ",
			stEndDate.ucDay, szEndDate, stEndDate.usYear, stEDTime.ucHour, stEDTime.ucMinute, (HUINT8*)" ");
		szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(szText, szTemp);
		/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
		if(hError != ERR_OK)
		{
			/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}
		/* ____________________________________________________________________ */
		// Empty Data
		HxSTD_snprintf(szTemp, sizeof(szTemp), (HUINT8*)"%s|%s|%s", (HUINT8*)" ", (HUINT8*)" ", (HUINT8*)" ");
		szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(szText, szTemp);
		/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
		if(hError != ERR_OK)
		{
			/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}
		szText = NULL;

		pstStatusListTmp = pstStatusListTmp->pstNext;
	}

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxStatusListLoadItem_TokensData(XmgrCas_CxStatusListContent_t *pstContents)
{
	CxStatusList_t *pstStatusList = NULL, *pstStatusListTmp = NULL;
	CxStatusList_t 	stList;
	pstStatusList = &stList;
	HERROR		hError = ERR_FAIL;

	HUINT8		*szText = NULL;
	HUINT8		szTemp[TEXT_SIZE128] = {0,}, szStr1[TEXT_SIZE128] = {0,}, szStr2[TEXT_SIZE128] = {0,};
	HUINT32 	ulBalance = 0;

	HxLOG_Print("[Enter]\r\n");
	// Dialog Create : Please Wait

	// Get Status Info - Tokens(Purse)
	pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllPurse, CX_SC_STATUS_REF);
	if (pstStatusList == NULL)
	{
		// Retry Get Status Info - Tokens(Purse)
		pstStatusList = (CxStatusList_t*)xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_AllPurse, CX_SC_STATUS_REF);
		if (pstStatusList == NULL)
		{
			HxLOG_Print("==[xmgr_cas_CxStatusListLoadItem_TokensData]== xmgr_cas_CxReqStatusInfo() Empty Data .\n");
			return ERR_FAIL;
		}
	}

	pstStatusListTmp = pstStatusList;

	/*
		============ CONAX Spec ============
		- Purse_status : Menu above displays two PURSE_STATUS parameters.
		- Select a purse by highlighting it. and then display the purse's credit or debit status.
	*/
	// make String
	while (pstStatusListTmp)
	{

		// Label
		ulBalance = pstStatusListTmp->stStatus.stPurse.ulBalance;
		szStr1[0] = 0 ;
		if (ulBalance)
		{
			HxSTD_snprintf(szStr1, sizeof(szStr1), (HUINT8*)"%s", pstStatusListTmp->stStatus.stPurse.szLabel);
		}

		HxSTD_snprintf(szStr2, sizeof(szStr2), (HUINT8*)"%s", (ulBalance>1) ? AP_CasResStr_GetStrRsc(STR_TOKENS_ID) : AP_CasResStr_GetStrRsc(STR_TOKENS_ID));
		if (ulBalance < 1000)
		{
			HxSTD_snprintf((HUINT8*)szTemp, sizeof(szTemp), (HUINT8*)"%s|%d %s|%d",
							szStr1, ulBalance, szStr2, pstStatusListTmp->stStatus.stPurse.usPurseRef);
		}
		else if (ulBalance < 1000000)
		{
			HxSTD_snprintf((HUINT8*)szTemp, sizeof(szTemp), (HUINT8*)"%s|%d,%03d %s|%d",
							szStr1, ulBalance/1000, ulBalance%1000, szStr2, pstStatusListTmp->stStatus.stPurse.usPurseRef);
		}
		else
		{
			HxSTD_snprintf((HUINT8*)szTemp, sizeof(szTemp), (HUINT8*)"%s|%d,%03d,%03d %s|%d",
							szStr1, ulBalance/1000000, (ulBalance/1000)%1000, ulBalance%1000, szStr2, pstStatusListTmp->stStatus.stPurse.usPurseRef);
		}

		szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(szText, szTemp);

		/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
		if(hError != ERR_OK)
		{
			/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		szText = NULL;

		pstStatusListTmp = pstStatusListTmp->pstNext;
	}

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxStatusListLoadItem_DebitData(XmgrCas_CxStatusListContent_t *pstContents, CxPurseStatus_t *pstPurseStatus)
{
	CxStatusList_t *pstStatusList = NULL, *pstStatusListTmp = NULL;
	CxStatusList_t 	stList;
	pstStatusList = &stList;
	HERROR		hError = ERR_FAIL;
	HUINT8		*szText = NULL, szTemp[TEXT_SIZE128] = {0,};
	HxDATETIME_Date_t	stDate;
	HxDATETIME_Time_t	stTime;

	// Dialog Create : Please Wait

	// Get Status Info - Purse Debit
	pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_Debit, pstPurseStatus->usPurseRef);
	if (pstStatusList == NULL)
	{
		// Get Status Info - Purse Debit
		pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_Debit, pstPurseStatus->usPurseRef);
		if (pstStatusList == NULL)
		{
			HxLOG_Print("==[xmgr_cas_CxStatusListLoadItem_DebitData]== xmgr_cas_CxReqStatusInfo() Empty Data .\n");
			return ERR_FAIL;
		}
	}

	pstStatusListTmp = pstStatusList;

	// Purse Status Item Title
	HxSTD_snprintf(szTemp, sizeof(szTemp), (HUINT8*)"%s: %s", AP_CasResStr_GetStrRsc(STR_PURSE_ID), pstPurseStatus->szLabel);
	szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
	MWC_UTIL_DvbStrcpy(szText, szTemp);
	/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
	hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
	if(hError != ERR_OK)
	{
		/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}
	szText = NULL;


	/*
		============ CONAX Spec ============
		- Support a minimun of 100 entries.
	*/

	// make String
	while (pstStatusListTmp)
	{

		stDate.usYear = pstStatusListTmp->stStatus.stDebit.stLastAccessTime.usYear;
		stDate.ucMonth = pstStatusListTmp->stStatus.stDebit.stLastAccessTime.ucMonth;
		stDate.ucDay = pstStatusListTmp->stStatus.stDebit.stLastAccessTime.ucDay;
		stTime.ucHour = pstStatusListTmp->stStatus.stDebit.stLastAccessTime.ucHour;
		stTime.ucMinute = pstStatusListTmp->stStatus.stDebit.stLastAccessTime.ucMinute;

		HxSTD_snprintf((char*)szTemp, TEXT_SIZE128, (char*)"%s|%ld|%02d.%02d.%04d %02d:%02d",
						 (HUINT8*)pstStatusListTmp->stStatus.stDebit.szLabel, pstStatusListTmp->stStatus.stDebit.ulConsumedTokens,
						stDate.ucDay, stDate.ucMonth, stDate.usYear, stTime.ucHour, stTime.ucMinute);

		szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(szText, szTemp);

		/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
		if(hError != ERR_OK)
		{
			/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		szText = NULL;

		pstStatusListTmp = pstStatusListTmp->pstNext;
	}

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxStatusListLoadItem_CreditData(XmgrCas_CxStatusListContent_t *pstContents, CxPurseStatus_t *pstPurseStatus)
{
	CxStatusList_t *pstStatusList = NULL, *pstStatusListTmp = NULL;
	CxStatusList_t 	stList;
	pstStatusList = &stList;
	HERROR		hError = ERR_FAIL;
	HUINT8		*szText = NULL, szTemp[TEXT_SIZE128] = {0,};
	// Dialog Create : Please Wait

	// Get Status Info - Purse Credit
	pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_Credit, pstPurseStatus->usPurseRef);
	if (pstStatusList == NULL)
	{
		// Retry Get Status Info - Purse Credit
		pstStatusList = xmgr_cas_CxReqStatusInfo(CX_SC_SLOT_0, eCxCaStatus_Credit, pstPurseStatus->usPurseRef);
	}

	pstStatusListTmp = pstStatusList;

	HxSTD_snprintf(szTemp, sizeof(szTemp), (HUINT8*)"%s: %s|", AP_CasResStr_GetStrRsc(STR_PURSE_ID), pstPurseStatus->szLabel);
	szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
	MWC_UTIL_DvbStrcpy(szText, szTemp);

	/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
	hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
	if(hError != ERR_OK)
	{
		/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	szText = NULL;

	/*
		============ CONAX Spec ============
		- Support a minimun of 20 entries.
	*/

	// make String
	while (pstStatusListTmp)
	{

		HxSTD_snprintf((HUINT8*)szTemp, sizeof(szTemp), (HUINT8*)"%s|%d", pstStatusListTmp->stStatus.stCredit.szLabel, pstStatusListTmp->stStatus.stCredit.ulCreditTokens);

		szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
		MWC_UTIL_DvbStrcpy(szText, szTemp);

		/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
		hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
		if(hError != ERR_OK)
		{
			/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
			HxLOG_Critical("\n\n");
			return ERR_FAIL;
		}

		szText = NULL;

		pstStatusListTmp = pstStatusListTmp->pstNext;
	}

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxStatusListLoadItem_MessagesData(XmgrCas_CxStatusListContent_t *pstContents)
{
	HERROR		hError = ERR_FAIL;
	HUINT8		*szText = NULL;
	HUINT8		szTemp[TEXT_SIZE128] = {0,}, szStr1[TEXT_SIZE128] = {0,};
	HUINT32 	ulNumMsg, i, ulStrlen;
	CxUserTextList_t pstList;


	HxLOG_Print("[Enter]\r\n");

	// Dialog Create : Please Wait

	hError = xmgr_cas_CxGetUserText(&ulNumMsg, &pstList);
	if ((hError != ERR_OK) || (ulNumMsg < 1))
	{
		HxLOG_Critical("\n\n");
		return	ERR_FAIL;
	}

	for (i = 0; i<ulNumMsg; i++)
	{
		ulStrlen = 0;
		ulStrlen = SvcCas_UtilStrLen(pstList.pstUserText[i].szText);
		if (ulStrlen)
		{
			int idx = 0;

			while (idx < ulStrlen)
			{
				memcpy(szStr1, pstList.pstUserText[i].szText + idx, D_TEXT_LINE_LENGTH);
				szStr1[D_TEXT_LINE_LENGTH] = 0;

				if (idx == 0)
					MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)"%d|%s", i, szStr1);	// indexed from 0 to 15
				else
					MWC_UTIL_DvbUtf8Sprintf(szTemp, (HUINT8*)" |%s", szStr1);

				idx += D_TEXT_LINE_LENGTH;

				szText = (HUINT8*)OxAP_Malloc(SvcCas_UtilStrLen(szTemp) + 1);
				MWC_UTIL_DvbStrcpy(szText, szTemp);

				/* linked list에 배열의 원소를 하나하나 리스트로 포인팅 시킨다. */
				hError = CAS_UTIL_LL_AddItem(&(pstContents->stTextListBox), szText);
				if (hError != ERR_OK)
				{
					/* 시스템에 의한 메모리 조작이 아니면 절대 발생하지 않을 듯, Linux only */
					HxLOG_Critical("\n\n");
				}
			}
		}
	}

	return ERR_OK;
}


STATIC HERROR	xmgr_cas_CxStatusListFreeItemData(XmgrCas_CxStatusListContent_t *pstContents)
{

	HINT32		i, lNumOfList = 0;
	HUINT8		*szTmp = NULL;
	HERROR		hError = ERR_FAIL;

	lNumOfList = CAS_UTIL_LL_GetNumOfListItem(&(pstContents->stTextListBox));

	for (i = 0; i < lNumOfList; i++)
	{
		hError = CAS_UTIL_LL_ReadIndexNthItem(&(pstContents->stTextListBox), i, (void**)&szTmp);
		if (hError != ERR_OK)
		{
			HxLOG_Critical("\n\n");
		}
		else
		{
			OxAP_Free(szTmp);
		}
	}
	return ERR_OK;
}
#if 1
STATIC HERROR xmgr_cas_CxStatusListSetSendData(XmgrCas_CxStatusListContent_t *pstContext)
{

}
STATIC HERROR xmgr_cas_CxStatusListSendData(XmgrCas_CxStatusListContent_t *pstContext)
{
	HERROR					hErr = ERR_OK;
	XmgrCas_CxStatusListContent_t			*pstItem;
	OxMgrCasUiEvtGrid_t		stEvent;
	OxMgrCasUiGrid_t		*pstGridList;
	OxMgrCasUiGridItem_t	*pstGridItem;
	HUINT32					ulStrLen, i, y;
	HUINT8					szStr[4][CAX_CX_MAX_ITEM_STRLEN];

	HUINT8		*szTmp = NULL;
	HxSTD_memset(&stEvent, 0, sizeof(OxMgrCasUiEvtGrid_t));
	stEvent.ulUiHandle = (HUINT32)pstContext->pfnCallerCallback;

	pstGridList = &stEvent.stGrid;

	pstGridList->stHeader.ulAttr = 0;
	MWC_UTIL_DvbStrncpy(pstGridList->stHeader.szItemAlign, "left|right", MGR_CAS_STR_LENGTH_LONG);
	pstGridList->stHeader.ulFocusType = eMgrCasUiFocusType_Column;
	pstGridList->stHeader.stFocus.ulHorIdx = 0;
	pstGridList->stHeader.stFocus.ulVerIdx = 0;
	pstGridList->stHeader.stStartPos.ulHorIdx = 0;
	pstGridList->stHeader.stStartPos.ulVerIdx = 0;

	ulStrLen = SvcCas_UtilStrLen(pstContext->szTitle);
	if(ulStrLen != 0)
	{
		MWC_UTIL_DvbStrncpy(pstGridList->stHeader.szTitle, pstContext->szTitle, MGR_CAS_STR_LENGTH_LONG);
		pstGridList->stHeader.ulAttr |= eMgrCasUiAttr_Title;
		HxLOG_Print("Menu Title [%s]\n", pstGridList->stHeader.szTitle);
	}

	ulStrLen = SvcCas_UtilStrLen(pstContext->szColumnTitle);
	if(ulStrLen != 0)
	{
		MWC_UTIL_DvbStrncpy(pstGridList->stHeader.szSubTitle, pstContext->szColumnTitle, MGR_CAS_STR_LENGTH_LONG);
		pstGridList->stHeader.ulAttr |= eMgrCasUiAttr_Subtitle;
		HxLOG_Print("Sub Title [%s]\n", pstGridList->stHeader.szSubTitle);
	}

	pstContext->ulTotalNum = CAS_UTIL_LL_GetNumOfListItem(&(pstContext->stTextListBox));

	pstGridList->ulTotalNum = pstContext->ulTotalNum;
	HxLOG_Print("Item Count[%d]\n", pstGridList->ulTotalNum);

	pstGridList->astGridItem = (OxMgrCasUiGridItem_t *)OxAP_Malloc(sizeof(OxMgrCasUiGridItem_t) * (pstGridList->ulTotalNum));
	if(pstGridList->astGridItem == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	HxSTD_memset(pstGridList->astGridItem, 0, sizeof(OxMgrCasUiGridItem_t) * (pstGridList->ulTotalNum));

	for (i = 0; i < pstGridList->ulTotalNum; i++)
	{
		pstGridItem = &pstGridList->astGridItem[i];
		pstGridItem->ulIndex = i;
		pstGridItem->ulAttr = 0;

		for(y=0; y<4; y++)
		{
			szStr[y][0]='\0';
		}

		hErr = CAS_UTIL_LL_ReadIndexNthItem(&(pstContext->stTextListBox), i, (void**)&szTmp);
		if (hErr != ERR_OK)
		{
			continue;
		}
		if (szTmp != NULL)
		{
			switch (pstContext->lCasCxCAItemID)
			{
				case eCasCXMenuItem_Subscription:
					pstGridList->stHeader.ulNumOfColumn = 3;
					xmgr_cas_CxGetWordFromString(szStr[0], szTmp, 0);
					xmgr_cas_CxGetWordFromString(szStr[1], szTmp, 1);
					xmgr_cas_CxGetWordFromString(szStr[2], szTmp, 2);
					xmgr_cas_CxGetWordFromString(szStr[3], szTmp, 3);

					MWC_UTIL_DvbStrcat(szStr[1], (HUINT8 *)" / ");
					MWC_UTIL_DvbStrcat(szStr[1], szStr[2]);

					HxSTD_snprintf((char *)pstGridItem->szString, MGR_CAS_STR_LENGTH_LONG, "%s|%s|%s", szStr[0], szStr[1], szStr[3]);
 					pstGridItem->szString[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';
				 	pstGridItem->ulStringLength = SvcCas_UtilStrLen(pstGridItem->szString);

					//xmgr_cas_CxStatusListDrawListItem_Subcription(screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
					break;

				case eCasCXMenuItem_Event:
					pstGridList->stHeader.ulNumOfColumn = 3;
					xmgr_cas_CxGetWordFromString(szStr[0], szTmp, 0);
					xmgr_cas_CxGetWordFromString(szStr[1], szTmp, 1);
					xmgr_cas_CxGetWordFromString(szStr[2], szTmp, 2);

					HxSTD_snprintf((char *)pstGridItem->szString, MGR_CAS_STR_LENGTH_LONG, "%s|%s|%s", szStr[0], szStr[1], szStr[2]);
 					pstGridItem->szString[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';
				 	pstGridItem->ulStringLength = SvcCas_UtilStrLen(pstGridItem->szString);

					//xmgr_cas_CxStatusListDrawListItem_Event(screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
					break;

				case eCasCXMenuItem_Tokens:
					pstGridList->stHeader.ulNumOfColumn = 3;
					xmgr_cas_CxGetWordFromString(szStr[0], szTmp, 0);
					xmgr_cas_CxGetWordFromString(szStr[1], szTmp, 1);

					HxSTD_snprintf((char *)pstGridItem->szString, MGR_CAS_STR_LENGTH_LONG, "%s|%s|", szStr[0], szStr[1]);
 					pstGridItem->szString[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';
				 	pstGridItem->ulStringLength = SvcCas_UtilStrLen(pstGridItem->szString);

					//xmgr_cas_CxStatusListDrawListItem_Tokens(screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
					break;

				case eCasCXMenuItem_Debit:
					pstGridList->stHeader.ulNumOfColumn = 3;
					xmgr_cas_CxGetWordFromString(szStr[0], szTmp, 0);
					xmgr_cas_CxGetWordFromString(szStr[1], szTmp, 1);
					xmgr_cas_CxGetWordFromString(szStr[2], szTmp, 2);

					HxSTD_snprintf((char *)pstGridItem->szString, MGR_CAS_STR_LENGTH_LONG, "%s|%s|%s", szStr[0], szStr[1], szStr[2]);
 					pstGridItem->szString[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';
				 	pstGridItem->ulStringLength = SvcCas_UtilStrLen(pstGridItem->szString);

					//xmgr_cas_CxStatusListDrawListItem_Debit(screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
					break;

				case eCasCXMenuItem_Credit:
					pstGridList->stHeader.ulNumOfColumn = 3;
					xmgr_cas_CxGetWordFromString(szStr[0], szTmp, 0);
					xmgr_cas_CxGetWordFromString(szStr[1], szTmp, 1);

					HxSTD_snprintf((char *)pstGridItem->szString, MGR_CAS_STR_LENGTH_LONG, "%s|%s|", szStr[0], szStr[1]);
 					pstGridItem->szString[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';
				 	pstGridItem->ulStringLength = SvcCas_UtilStrLen(pstGridItem->szString);

					//xmgr_cas_CxStatusListDrawListItem_Credit(screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
					break;

#if defined(CONFIG_MW_CAS_CONAX_TEXT)
				case eCasCXMenuItem_Text_View:
					pstGridList->stHeader.ulNumOfColumn = 2;
					xmgr_cas_CxGetWordFromString(szStr[0], szTmp, 0);
					xmgr_cas_CxGetWordFromString(szStr[1], szTmp, 1);

					HxSTD_snprintf((char *)pstGridItem->szString, MGR_CAS_STR_LENGTH_LONG, "%s|%s", szStr[0], szStr[1]);
 					pstGridItem->szString[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';
				 	pstGridItem->ulStringLength = SvcCas_UtilStrLen(pstGridItem->szString);

					//xmgr_cas_CxStatusListDrawListItem_Message(screen, font, x1, y, lText_x_off, szTmp, ulfontTextColor);
					break;
#endif	// End of defined(CONFIG_MW_CAS_CONAX_TEXT)
				default:
					HxLOG_Critical("\n\n");
					break;
			}
		}
#if 0
		if(SvcCas_UtilStrLen(pstItem->szItemName) != 0)
		{
			snprintf((char *)pstGridItem->szString, MGR_CAS_STR_LENGTH_LONG, "%s|%s", pstItem->szItemName, pstItem->szItemValue);
		}
		else
		{
			snprintf((char *)pstGridItem->szString, MGR_CAS_STR_LENGTH_LONG, "%s| ", pstItem->szItemName);
		}
		pstGridItem->szString[MGR_CAS_STR_LENGTH_LONG - 1] = '\0';
	 	pstGridItem->ulStringLength = SvcCas_UtilStrLen(pstGridItem->szString);
#endif
		HxLOG_Print("Item [%d][%s]\n", i, pstGridItem->szString);
	}

	/* send data to UI */
	hErr = MGR_APPMGR_MODULE_SendUiEvent (eDxCAS_GROUPID_CX, 0/*ulSlotId*/, EVT_CASUI_GRID, &stEvent);
	if(hErr != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

	if(pstGridList->astGridItem != NULL)
	{
		OxAP_Free(pstGridList->astGridItem);
	}

	return hErr;
}
#endif
STATIC HERROR		xmgr_cas_CxStatusListUiRequestSessionStop(XmgrCas_CxStatusListContent_t *pContext)
{
	OxMgrCasUiEvtReqSessionStop_t stEvent;

	stEvent.ulSessionType = (HUINT32)eMgrCasUiSession_Menu;
	if(MGR_APPMGR_MODULE_SendUiEvent (eDxCAS_GROUPID_CX, 0, eMEVT_CASUI_REQ_CLOSE_SESSION, &stEvent) != ERR_OK)
	{
		HxLOG_Error("\n");
	}

	return ERR_OK;
}
STATIC BUS_Result_t xmgr_cas_CxStatusListUiMsgInputNotify(XmgrCas_CxStatusListContent_t *pstContext, HINT32 message, HINT32 p1, HINT32 p2, HINT32 p3)
{
	OxMgrCasInputType_e 	eInputType = (OxMgrCasInputType_e)p2;
	OxMgrCasUiInputNotify_t	*pstCasInputData = (OxMgrCasUiInputNotify_t *)p3;
	BUS_Callback_t			pfnUiCallback;



#if 1
	//if(pstContext->lCasCxCAItemID != eCasCXMenuItem_Max)
	if(pstContext->lCasCxCAItemID < eCasCXMenuItem_Add_Max)
	{
		if(eInputType == eMgrCasInputType_Etc)
		{
			if(pstCasInputData->stEtc.eMgrCasEtcType == eMgrCasEtc_Previous)
			{
				//HxLOG_Print("[local_cas_cx_ui_camenu_MsgInputNotify] : eMgrCasEtc_Previous ");
				xmgr_cas_CxStatusListUiRequestSessionStop(pstContext);
			}
			else if(pstCasInputData->stEtc.eMgrCasEtcType == eMgrCasEtc_Exit)
			{
				//HxLOG_Print("[local_cas_cx_ui_camenu_MsgInputNotify] : eMgrCasEtc_Exit ");
				xmgr_cas_CxStatusListUiRequestSessionStop(pstContext);
			}

		}
		else if(eInputType == eMgrCasInputType_Selection)
		{
			HxLOG_Print("[xmgr_cas_CxStatusListUiMsgInputNotify] [%s] index:[%d], menu(%d)\n", "eMgrCasInputType_Selection",
															pstCasInputData->stSelection.ulItemIdx,pstContext->lCasCxCAItemID);

			xmgr_cas_CxStatusListInitListTitle(pstContext);

			xmgr_cas_CxStatusListMsgGwmListSelected(pstContext, pstCasInputData->stSelection.ulItemIdx);
			#if 0
			if(pstCasInputData->stSelection.ulItemIdx >= eCasCXMenuItem_Max)
				return ERR_BUS_OUT_OF_RANGE;

			pfnUiCallback = pstContext->astListItem[pstCasInputData->stSelection.ulItemIdx].pfnUiCallback;
			if(pfnUiCallback != NULL)
			{
				pstContext->lCasCxCAItemID = pstCasInputData->stSelection.ulItemIdx;
				BUS_MGR_Create(NULL, APP_CAS_PRIORITY, pfnUiCallback, 0, (HINT32)xproc_cas_CxStatusList/*p1*/, p2, p3);
			}
			#endif
		}
		else if(eInputType == eMgrCasInputType_Number)
		{
			;
		}
	}
	else
	{
		#if 0
		if (BUS_MGR_Get(pstContext->astListItem[pstContext->eActiveItem].pfnUiCallback) != NULL)
		{
			HxLOG_Print("[xmgr_cas_CxStatusListUiMsgInputNotify] Send Message\n");
			BUS_SendMessage(pstContext->astListItem[pstContext->eActiveItem].pfnUiCallback, message, 0, p1, p2, p3);
		}
		#endif
	}
#else
	//HxLOG_Print("[local_cas_cx_ui_camenu_MsgInputNotify] InputType[%d]", eInputType);
	if(eInputType == eMgrCasInputType_Etc)
	{
		if(pstCasInputData->stEtc.eMgrCasEtcType == eMgrCasEtc_Previous)
		{
			//HxLOG_Print("[local_cas_cx_ui_camenu_MsgInputNotify] : eMgrCasEtc_Previous ");
			xmgr_cas_CxStatusListUiRequestSessionStop(pstContext);
		}
		else if(pstCasInputData->stEtc.eMgrCasEtcType == eMgrCasEtc_Exit)
		{
			//HxLOG_Print("[local_cas_cx_ui_camenu_MsgInputNotify] : eMgrCasEtc_Exit ");
			xmgr_cas_CxStatusListUiRequestSessionStop(pstContext);
		}

	}
#endif
	return ERR_BUS_MESSAGE_BREAK;
}
#define	________________Global_Function_______________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/
BUS_Result_t	xproc_cas_CxStatusList(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
//	HINT32				lToUppermenu =CAS_CX_MENU_APP_TOUPPER_ON;
	BUS_Result_t		eBusResult = ERR_BUS_NO_ERROR;
	XmgrCas_CxStatusListContent_t	*pstContents = xmgr_cas_CxStatusListGetLocalContents();

	HxLOG_Info("==[xproc_cas_CxStatusList]== handle(0x%x)\n", hAction);

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Print("== eMEVT_BUS_CREATE : Object ID : %d\r\n", p3);

			//xmgr_cas_CxStatusListInitListTitle(pstContents);
			xmgr_cas_CxStatusListInitStatusList(pstContents, p1, p2, p3);

			/* send to ui */
			xmgr_cas_CxStatusListSendData(pstContents);
			break;
		case MSG_OAPI_CASUI_SESSION_START:
			break;

		case MSG_OAPI_CASUI_SESSION_STOP:
			break;

		case MSG_OAPI_CASUI_INPUT_NOTIFY:
			eBusResult = xmgr_cas_CxStatusListUiMsgInputNotify(pstContents, message, p1, p2, p3);
			if(eBusResult != ERR_OK)
			{
				return eBusResult;
			}
			break;

		case MSG_OAPI_CASUI_CHANGE_LANGUAGE:
			break;

		case eMEVT_BUS_DESTROY:
			/* destroy UI */
			xmgr_cas_CxStatusListFreeItemData(pstContents);
			xmgr_cas_CxStatusListResetLists(pstContents);
			// TODO: Destory 하면 죽는다....
			#if 0
			if (BUS_MGR_Get(xproc_cas_CxStatusList) != NULL)
			{
				BUS_MGR_Destroy(xproc_cas_CxStatusList);
			}
			BUS_MGR_Destroy(0);
			#endif

			/* kill timer */
			BUS_KillTimer(MENU_TIMEOUT_ID);

			// TODO: Blocking...
			#if 0 // not use for octo2.0
			xmgr_cas_CxStatusListDestroyUI(pstContents);

			/* clear front PANEL */
			//AP_SPEC_ClearMenuPanelString();
			#endif
			break;

		default:
			break;

		// TODO: Blocking...
		#if 0 // not use for octo2.0

		case MSG_DLG_INPUTED_KEYDOWN:
			GWM_ResetTimer(MENU_TIMEOUT_ID);
			return MESSAGE_BREAK;

		case MSG_BUS_LIST_CHANGED:
			GWM_APP_InvalidateObject(CAS_CX_STATUSLIST_LIST_ID);
			return MESSAGE_BREAK;

		case MSG_BUS_LIST_SELECTED:
			xmgr_cas_CxStatusListMsgGwmListSelected(pstContents, p2);
			return MESSAGE_BREAK;

		case MSG_BUS_KEYUP:
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_BUS_KEYDOWN:
			AP_KeyConv_UiCommon(&p1);
			GWM_ResetTimer(MENU_TIMEOUT_ID);

			eBusResult = Settings_CheckKeyPass(p1, BUS_MGR_GetAppCallback(BUS_MGR_GetThis()));
			if (eBusResult != ERR_BUS_NO_ERROR)
			{
				return eBusResult;
			}

			switch (p1)
			{
				case KEY_EXIT:
				case KEY_MENU:
					/* reset Title depth */
					Settings_ClearWindowTitle();
					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

			//	case KEY_OK:		/* 단순히 화면을 보는 것이므로 */
				case KEY_BACK:
				case KEY_ARROWLEFT:
					lToUppermenu = pstContents->lToUpper;

					/* to upper */
					BUS_MGR_Destroy(NULL);
					if (lToUppermenu == CAS_CX_MENU_APP_TOUPPER_ON)
					{
						BUS_MGR_Create((char*)"xproc_cas_CxSmartCardMenu", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xproc_cas_CxSmartCardMenu, 0, 0, 0, 0);
					}
					return MESSAGE_BREAK;

				case KEY_ARROWRIGHT:
					{
						HINT32	lCurIdx = 0, lNumOfList= 0;

						// Purse Debit Status 는 OK 동작 과 동일 하게 처리
						if ((pstContents->lCasCxCAItemID == eCasCXMenuItem_Debit) || (pstContents->lCasCxCAItemID == eCasCXMenuItem_Tokens))
						{
							lCurIdx = GWM_List_GetCurrentIndex(CAS_CX_STATUSLIST_LIST_ID);
							lNumOfList = CAS_UTIL_LL_GetNumOfListItem(&(pstContents->stTextListBox));
							if (lNumOfList > 0)
							{
								xmgr_cas_CxStatusListMsgGwmListSelected(pstContents, lCurIdx);
								return MESSAGE_BREAK;
							}
						}
					}
					break;

				// STB info 에서는 arrow up/down을 page up/down으로 mapping
				case KEY_CH_PLUS:
				case KEY_CH_MINUS:
					if (!(pstContents->lCasCxCAItemID == eCasCXMenuItem_Subscription || pstContents->lCasCxCAItemID == eCasCXMenuItem_Event))
					{
						break;
					}
					// 현재 focus가 list obj이고 page가 1페이지 이상이라면 page up/down으로 변경
					// 아니라면 not available
					if (GUIOBJ_LIST == GWM_Obj_GetObjectType(GWM_GetFocusedObjectID()))
					{
						HUINT32 ulListTotalItemNum, ulListNumOfPage;

						GWM_List_GetTotalItem(GWM_GetFocusedObjectID(), &ulListTotalItemNum);
						ulListNumOfPage = GWM_List_GetNumOfPage(GWM_GetFocusedObjectID());

						if (ulListNumOfPage < ulListTotalItemNum)	// 1page 이상이면 page로 변환
						{
							if ((p1 == KEY_CH_PLUS) || (p1 == KEY_ARROWUP))
							{
								p1 = KEY_PAGEUP;
							}
							else
							{
								p1 = KEY_PAGEDOWN;
							}
							break;
						}
					}
					/* launch NOT Available application */
					AP_NotAvailable_Show();
					return MESSAGE_BREAK;

				default:
					break;
			}
			break;
		case eMEVT_BUS_TIMER:
			if (p1 == MENU_TIMEOUT_ID)
			{
				/* reset Title depth */
				Settings_ClearWindowTitle();
				BUS_MGR_Destroy(0);
			}
			return MESSAGE_BREAK;

		case eSEVT_CAS_SC_REMOVED:
			HxLOG_Print(" SC Event : eSEVT_CAS_SC_REMOVED!\n");
			GWM_SendMessage(xproc_cas_CxStatusList, MSG_BUS_KEYDOWN, hAction, KEY_MENU, 0, 0);
			break;
		#endif

	}

	HxLOG_Info("==[xproc_cas_CxStatusList]== handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

// End of file
