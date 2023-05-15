/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>
#include <db_svc.h>

#include <gwm.h>
#include <epgui_coor.h>
#include <ap_resource.h>
#include "ap_epg_main.h"

#include "osd_gfx.h"
#include "osd_font.h"


// ap_epg_detailinfo.c 를 그대로 사용 한 것 임
// 문제 발생시 위 파일 참조

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define PPVDETAIL_TITLE_LEN							(64)
#define PPVDETAIL_CONTENTS_FONT_SIZE				FONT_SIZE_S
#define PPVDETAIL_FMT_MARGIN						10

#if	defined(CONFIG_MW_EPG_TVTV)
#define HIDDEN_TVTV_DELETE							0x0000000b1	//1457
#define HIDDEN_TVTV_INFO							0x00000002a	//135
#endif

typedef enum
{
	ePPVDETAIL_FRAME_ID								= 0,
	ePPVDETAIL_TXT_TITLE_ID,
	ePPVDETAIL_FMT_ID,
	ePPVDETAIL_IMAGE_ID,
	ePPVDETAIL_FMT_SCROLL_ID,					// no create object, use dimension only

	ePPVDETAIL_ID_MAX
} XmgrCas_NaPpvDetailIds_e;


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct tagXmgrCas_NaPpvDetailInfoContext_t
{
	// object titles
	HUINT8				szFrameTitle[PPVDETAIL_TITLE_LEN];

	// local var.
	EPG_APEVENT_t		stApEvent;

	// color
	OSD_Color_t			cShortEvtInfo;
	OSD_Color_t			cDetailEvtInfo;
#if	defined(CONFIG_MW_EPG_TVTV)
	HBOOL				bImage;
	HUINT8				*pucBitmap;
#endif
} XmgrCas_NaPpvDetailInfoContext_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_NaPpvDetailInfoContext_t	s_PPVDetailInfo_Context;

#if	defined(CONFIG_MW_EPG_TVTV)
STATIC HUINT32						s_ulTvtvHiddenNumber = 0;
#endif
/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define _________PROTO_TYPE__________
STATIC BUS_Result_t		xproc_cas_NaPpvDetailProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
#if defined(CONFIG_APUI_SHOW_NO_EIT_INFO_MESSAGE)
STATIC BUS_Result_t 	xmgr_cas_NaPpvDetailAddNoEITInfo(EPG_APEVENT_t *pEvt);
#endif
STATIC BUS_Result_t		xmgr_cas_NaPpvDetailInitAppData(HINT32 lApEvent);
STATIC BUS_Result_t		xmgr_cas_NaPpvDetailAddOneLineText(HUINT8	*pszStr);
STATIC BUS_Result_t		xmgr_cas_NaPpvDetailInitAppObj(void);
STATIC BUS_Result_t		xmgr_cas_NaPpvDetailDestroyApp(void);

STATIC BUS_Result_t		xmgr_cas_NaPpvDetailDrawFrame(	OSD_Screen_t *pScreen,
														OSD_Area_t *pArea,
														OSD_Rect_t *pRect,
														GWM_GuiObject_t pObject,
														void *pArg,
														int lParam1);

STATIC BUS_Result_t		xmgr_cas_NaPpvDetailDrawItems(int lId, OSD_Screen_t *pScreen, FMT_DrawItem_t *pDrawItem);
STATIC BUS_Result_t		xmgr_cas_NaPpvDetailGetTokenValue(HUINT8 *pszContens, HUINT8 *pszToken, HINT32 *plMove);
STATIC BUS_Result_t		xmgr_cas_NaPpvDetailDrawScroll(	OSD_Screen_t *pScreen,
														OSD_Area_t *pArea,
														OSD_Rect_t *pRect,
														FMT_OBJECT pObject);

#if	defined(CONFIG_MW_EPG_TVTV)
STATIC BUS_Result_t			xmgr_cas_NaPpvDetailDrawImage(	OSD_Screen_t *pScreen,
															OSD_Area_t *pArea,
															OSD_Rect_t *pRect,
															GWM_GuiObject_t pObject,
															void *pArg,
															int lParam1);

#define ____HIDDEN_FUNC____
STATIC void					xmgr_cas_NaPpvDetailCreateTvtvInfo(void);
STATIC BUS_Result_t			xmgr_cas_NaPpvDetailTvtvInfoProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t			xmgr_cas_NaPpvDetailInitTvtvInfoObj(void);
STATIC BUS_Result_t			xmgr_cas_NaPpvDetailDestroyTvtvInfoApp(void);
STATIC BUS_Result_t			xmgr_cas_NaPpvDetailTvtvInfoDrawFrame(	OSD_Screen_t *pScreen,
																	OSD_Area_t *pArea,
																	OSD_Rect_t *pRect,
																	GWM_GuiObject_t pObject,
																	void *pArg,
																	int lParam1);
#endif

#define _________GLOBAL_FUNC__________
/**
  *	usage:
  *
  *	lParam1: event structure pointer (EPG_APEVENT_t *)
  *	lParam2: N.A.
  *	lParam3: N.A.
**/
BUS_Result_t		xproc_cas_NaPpvDetailInfo_Rcs(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
	return xproc_cas_NaPpvDetailProc(lMessage, hHandle, lParam1, lParam2, lParam3);
}


#define _________LOCAL_FUNC__________

STATIC BUS_Result_t		xproc_cas_NaPpvDetailProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
	BUS_Result_t 		lResult = ERR_FAIL;

	switch (lMessage)
	{
		case MSG_GWM_CREATE :
			/* set application data init */
			lResult = xmgr_cas_NaPpvDetailInitAppData(lParam1);
			if(lResult != ERR_BUS_NO_ERROR)
			{
				HxLOG_Print("[xproc_cas_NaPpvDetailProc] xmgr_cas_NaPpvDetailInitAppData fail \n");
			}

			/* drawing application draw objects */
			lResult = xmgr_cas_NaPpvDetailInitAppObj();
			if(lResult != ERR_BUS_NO_ERROR)
			{
				HxLOG_Print("[xproc_cas_NaPpvDetailProc] xmgr_cas_NaPpvDetailInitAppObj fail \n");
#if defined(CONFIG_APUI_SHOW_NO_EIT_INFO_MESSAGE)
				xmgr_cas_NaPpvDetailAddNoEITInfo((EPG_APEVENT_t*)lParam1);
#endif

			}
			break;

		case MSG_GWM_KEYDOWN :
			AP_KeyConv_UiCommon(&lParam1);
			switch (lParam1)
			{
				case KEY_MENU :
				case KEY_GUIDE:
					BUS_MGR_Destroy(0);
					break;

				case KEY_EXIT :
				case KEY_BACK:
				case KEY_OK:
					BUS_MGR_Destroy(0);
					return MESSAGE_BREAK;

				case KEY_ARROWLEFT :
					lParam1 = KEY_ARROWUP;
					break;

				case KEY_ARROWRIGHT :
					lParam1 = KEY_ARROWDOWN;
					break;

				case KEY_PAGEUP :
					GWM_FMT_ScrollUpPage(ePPVDETAIL_FMT_ID);
					return MESSAGE_BREAK;

				case KEY_PAGEDOWN :
					GWM_FMT_ScrollDownPage(ePPVDETAIL_FMT_ID);
					return MESSAGE_BREAK;

				case KEY_RED :
				case KEY_YELLOW :
				case KEY_GREEN :
				case KEY_BLUE :
				case KEY_0:
				case KEY_1:
				case KEY_2:
				case KEY_3:
				case KEY_4:
				case KEY_5:
				case KEY_6:
				case KEY_7:
				case KEY_8:
				case KEY_9:
				case KEY_TVRADIO:
				case KEY_EXT_AV:
				case KEY_COMBO:
				case KEY_TEXT:
				case KEY_TTX_HOLD:	/* 이하 ch, vol 하단의 버튼 처리 */
				case KEY_TTX_SUBPAGE:
				case KEY_AUDIO:
				case KEY_SUBTITLE:
				case KEY_3D:
				case KEY_EXT_AV_PLUS:
				case KEY_EXT_AV_MINUS:
				case KEY_INFO :
#if				defined(CONFIG_MW_MM_PVR)
				case KEY_RECORD :
				case KEY_PLAY:
				case KEY_STOP:
				case KEY_PAUSE:
				case KEY_SLOW:
				case KEY_ADD_BOOKMARK:
				case KEY_BOOKMARK_LIST:
				case KEY_PREV :
 				case KEY_NEXT:
				case KEY_FORWARD:
				case KEY_BACKWARD:
#endif
#if					defined(CONFIG_MW_EPG_TVTV)
					if(lParam1 == KEY_0){s_ulTvtvHiddenNumber = 0;}
					else if(lParam1 == KEY_1){s_ulTvtvHiddenNumber |= 1 << (lParam1 - KEY_0);}
					else if(lParam1 == KEY_2){s_ulTvtvHiddenNumber |= 1 << (lParam1 - KEY_0);}
					else if(lParam1 == KEY_3){s_ulTvtvHiddenNumber |= 1 << (lParam1 - KEY_0);}
					else if(lParam1 == KEY_4){s_ulTvtvHiddenNumber |= 1 << (lParam1 - KEY_0);}
					else if(lParam1 == KEY_5){s_ulTvtvHiddenNumber |= 1 << (lParam1 - KEY_0);}
					else if(lParam1 == KEY_6){s_ulTvtvHiddenNumber |= 1 << (lParam1 - KEY_0);}
					else if(lParam1 == KEY_7){s_ulTvtvHiddenNumber |= 1 << (lParam1 - KEY_0);}
					else if(lParam1 == KEY_8){s_ulTvtvHiddenNumber |= 1 << (lParam1 - KEY_0);}
					else if(lParam1 == KEY_9){s_ulTvtvHiddenNumber |= 1 << (lParam1 - KEY_0);}

					if(s_ulTvtvHiddenNumber == HIDDEN_TVTV_INFO){xmgr_cas_NaPpvDetailCreateTvtvInfo();}
#endif
					AP_NotAvailable_Show();
					return MESSAGE_BREAK;

				case KEY_WIDE:
					GWM_DirectRedrawAll();
					return MESSAGE_PASS;

				default :
					break;
			}
			break;

		case MSG_GWM_DESTROY :
			lResult = xmgr_cas_NaPpvDetailDestroyApp();
			if(lResult != ERR_BUS_NO_ERROR)
			{
				HxLOG_Print("[xproc_cas_NaPpvDetailProc] xmgr_cas_NaPpvDetailDestroyApp fail \n");
			}
			return MESSAGE_BREAK;
	}

	return BUS_ProcessMessageDefault(lMessage, hHandle, lParam1, lParam2, lParam3);

}


#if defined(CONFIG_APUI_SHOW_NO_EIT_INFO_MESSAGE)
STATIC BUS_Result_t 	xmgr_cas_NaPpvDetailAddNoEITInfo(EPG_APEVENT_t *pEvt)
{
	HERROR			hError = ERR_FAIL;
	DbSvc_Info_t		svcInfo;
	HUINT8			*pszBuf = NULL;
	HINT32			nSvcNum = 0;

	if(pEvt != NULL)
	{
		pszBuf = GetTextBuffer(256);
		hError = DB_SVC_GetServiceInfo(pEvt->hSvcHandle, &svcInfo);
		#if 0
		DB_SVC_GetServiceNumber(pEvt->hSvcHandle, &nSvcNum);
		#else
		nSvcNum = DbSvc_GetLcn(&svcInfo);
		#endif
		if (hError == ERR_OK && nSvcNum != 0)
		{
			sprintf(pszBuf, (const HUINT8*)"%d %s", nSvcNum, DbSvc_GetName(&svcInfo));
			xmgr_cas_NaPpvDetailAddOneLineText(pszBuf);
		}
	}

	xmgr_cas_NaPpvDetailAddOneLineText(GetStrRsc(LOC_IPLATE_NO_INFORMATION_TEXT_ID));

	return ERR_BUS_NO_ERROR;
}
#endif


STATIC BUS_Result_t		xmgr_cas_NaPpvDetailInitAppData(HINT32 lApEvent)
{
	BUS_Result_t 		lResult = ERR_FAIL;
	EPG_APEVENT_t		*pApEvent = NULL;

	pApEvent = (EPG_APEVENT_t*)lApEvent;

	VK_memset32(&s_PPVDetailInfo_Context, 0x00, sizeof(XmgrCas_NaPpvDetailInfoContext_t));

	MWC_UTIL_DvbStrcpy(s_PPVDetailInfo_Context.szFrameTitle, GetStrRsc(LOC_EPG_PROGINFO_POPUP_TITLE_ID));

	if (pApEvent != NULL)
	{
		HxSTD_memcpy(&s_PPVDetailInfo_Context.stApEvent, pApEvent, sizeof(EPG_APEVENT_t));
	}


#if	defined(CONFIG_MW_EPG_TVTV)
	if(pApEvent->stEventInfo.stEventInfo.ucEpgDataType == EPG_DATA_TYPE_TVTV)
	{
		if(pApEvent->stEventInfo.stEventInfo.stTvtvEventInfo.ucIsImage != 0)
		{
			HUINT8		aucFullName[128];
			HUINT32		ulReadSize = 0;
			HERROR		hError = ERR_FAIL;
			HUINT8		*pBuffer = NULL;

			sprintf(aucFullName, "%s%s", EPG_FS_TVTV_IMAGE_PATH, pApEvent->stEventInfo.stEventInfo.stTvtvEventInfo.aucImageName);

			HxLOG_Info("[xmgr_cas_NaPpvDetailInitAppData] aucFullName(%s)!!\n", aucFullName);

			hError = HLIB_FILE_ReadFile((const HUINT8 *)aucFullName, &pBuffer, &ulReadSize);
			if(hError != ERR_OK || pBuffer == NULL)
			{
				HxLOG_Error("[xmgr_cas_NaPpvDetailInitAppData] HLIB_FILE_ReadFile fail!!\n");
			}
			else
			{
				s_PPVDetailInfo_Context.bImage = TRUE;
				if (s_PPVDetailInfo_Context.pucBitmap)
				{
					OxAP_Free(s_PPVDetailInfo_Context.pucBitmap);
					s_PPVDetailInfo_Context.pucBitmap = NULL;
				}
				s_PPVDetailInfo_Context.pucBitmap = pBuffer;
			}
		}
	}
#endif


	lResult = ERR_BUS_NO_ERROR;


	return lResult;

}

/* just add one line word (don't care multyline) */
STATIC BUS_Result_t		xmgr_cas_NaPpvDetailAddOneLineText(HUINT8	*pszStr)
{
	Text_Property_t 	stTextProperty = {0, };

	if (pszStr == NULL)
	{
		return ERR_TARGET_NULL;
	}

	stTextProperty.ulFontSize	= PPVDETAIL_CONTENTS_FONT_SIZE;
	stTextProperty.ulFontAlign	= TEXT_ALIGN_LEFT;
	stTextProperty.cFontColor 	= COL(C_T_Info_Detail);

	return GWM_FMT_AddItem(ePPVDETAIL_FMT_ID, (char*)pszStr, &stTextProperty);
}

STATIC BUS_Result_t		xmgr_cas_NaPpvDetailInitAppObj(void)
{
	BUS_Result_t 				lResult = ERR_FAIL;
	EPG_UI_COMPONENT_t			*pstUiComp = NULL;
	OSD_Rect_t					stRect = {0, };
	Text_Property_t 				stTextPropertyHead = {0, };
	Text_Property_t 				stTextPropertyTail = {0, };

/*****************************************************************************************
  *
  * set app area
  *
  *****************************************************************************************/
	lResult = EPGUISET_GetDetailInfoUiComponent(ePPVDETAIL_FRAME_ID, &pstUiComp);
	if (lResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailInitAppObj] EPGUISET_GetDetailInfoUiComponent() fail \n");
		goto EXIT_PPVDETAIL_FUNC;
	}
	GWM_APP_SetAppArea (pstUiComp->stRect.x, pstUiComp->stRect.y, pstUiComp->stRect.w, pstUiComp->stRect.h);

/*****************************************************************************************
  *
  * frame
  *
  *****************************************************************************************/
	OSD_GFX_SetRect (&stRect, 0, 0, pstUiComp->stRect.w, pstUiComp->stRect.h);
	GWM_Frame_Create (ePPVDETAIL_FRAME_ID, &stRect, "AP_DetailInfo_Frame");
	GWM_APP_SetObjectDrawMethod(ePPVDETAIL_FRAME_ID, xmgr_cas_NaPpvDetailDrawFrame);

/*****************************************************************************************
  *
  * title
  *
  *****************************************************************************************/
	lResult = EPGUISET_GetDetailInfoUiComponent(ePPVDETAIL_TXT_TITLE_ID, &pstUiComp);
	if (lResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailInitAppObj] EPGUISET_GetDetailInfoUiComponent() fail \n");
		goto EXIT_PPVDETAIL_FUNC;
	}
	OSD_GFX_SetRect (&stRect, pstUiComp->stRect.x, pstUiComp->stRect.y, pstUiComp->stRect.w, pstUiComp->stRect.h);
	GWM_Text_Create(ePPVDETAIL_TXT_TITLE_ID, &stRect, (char*)s_PPVDetailInfo_Context.szFrameTitle );
	GWM_Text_ChangeColor(ePPVDETAIL_TXT_TITLE_ID, pstUiComp->cTextNormal, 0x00);
	GWM_Text_SetSize(ePPVDETAIL_TXT_TITLE_ID, FONT_SIZE_M);
	GWM_Text_SetAlign(ePPVDETAIL_TXT_TITLE_ID, (TEXT_ALIGN_CENTER | TEXT_ALIGN_MIDDLE));

/*****************************************************************************************
  *
  * fmt
  *
  *****************************************************************************************/
	lResult = EPGUISET_GetDetailInfoUiComponent(ePPVDETAIL_FMT_ID, &pstUiComp);
	if (lResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailInitAppObj] EPGUISET_GetDetailInfoUiComponent() fail \n");
		goto EXIT_PPVDETAIL_FUNC;
	}
	OSD_GFX_SetRect(&stRect,	pstUiComp->stRect.x,
								pstUiComp->stRect.y + PPVDETAIL_FMT_MARGIN,
								pstUiComp->stRect.w,
								pstUiComp->stRect.h - (PPVDETAIL_FMT_MARGIN*2));
	GWM_FMT_Create(ePPVDETAIL_FMT_ID, &stRect, FMT_OWN_DRAWITEM | FMT_SCROLL_ON, 12);
	GWM_FMT_ChangeItemDrawMethod(ePPVDETAIL_FMT_ID, xmgr_cas_NaPpvDetailDrawItems);
	GWM_FMT_ChangeScrollDrawMethod(ePPVDETAIL_FMT_ID, xmgr_cas_NaPpvDetailDrawScroll);

	s_PPVDetailInfo_Context.cShortEvtInfo = pstUiComp->cTextNormal;
	s_PPVDetailInfo_Context.cDetailEvtInfo = pstUiComp->cTextFocus;

/*****************************************************************************************
  *
  * load event
  *
  *****************************************************************************************/
	stTextPropertyHead.ulFontSize = stTextPropertyTail.ulFontSize =PPVDETAIL_CONTENTS_FONT_SIZE;
	stTextPropertyHead.ulFontAlign = stTextPropertyTail.ulFontAlign =TEXT_ALIGN_LEFT;

	stTextPropertyHead.cFontColor = COL(C_T_Info_Detail);
	stTextPropertyTail.cFontColor = COL(C_T_Dialog_MSG);
	lResult = AP_EPGMAIN_LoadDefaultFmtData(	ePPVDETAIL_FMT_ID,
												pstUiComp->stRect.w - 20,
												(void*)&s_PPVDetailInfo_Context.stApEvent,
												&stTextPropertyHead,
												&stTextPropertyTail,
												0);
	if (lResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailInitAppObj] AP_EPGMAIN_LoadDefaultFmtData() fail \n");
		goto EXIT_PPVDETAIL_FUNC;
	}


#if	defined(CONFIG_MW_EPG_TVTV)
/*****************************************************************************************
  *
  * image
  *
  *****************************************************************************************/
	lResult = EPGUISET_GetDetailInfoUiComponent(ePPVDETAIL_IMAGE_ID, &pstUiComp);
	if (lResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailInitAppObj] EPGUISET_GetDetailInfoUiComponent() fail \n");
		goto EXIT_PPVDETAIL_FUNC;
	}
	OSD_GFX_SetRect (&stRect, pstUiComp->stRect.x, pstUiComp->stRect.y, pstUiComp->stRect.w, pstUiComp->stRect.h);
	GWM_Frame_Create (ePPVDETAIL_IMAGE_ID, &stRect, "AP_DetailInfo_Image");
	GWM_APP_SetObjectDrawMethod(ePPVDETAIL_IMAGE_ID, xmgr_cas_NaPpvDetailDrawImage);
#endif

	GWM_Obj_SetFocus(ePPVDETAIL_FMT_ID);

EXIT_PPVDETAIL_FUNC:

	return lResult;
}

STATIC BUS_Result_t		xmgr_cas_NaPpvDetailDestroyApp(void)
{
	BUS_Result_t 		hRet = ERR_INITIAL_FAILED;

#if	defined(CONFIG_MW_EPG_TVTV)
	if(s_PPVDetailInfo_Context.bImage == TRUE)
	{
		OxAP_Free(s_PPVDetailInfo_Context.pucBitmap);

		s_PPVDetailInfo_Context.bImage = FALSE;
		s_PPVDetailInfo_Context.pucBitmap = NULL;
	}
#endif

	GWM_FMT_Delete(ePPVDETAIL_FMT_ID);
	GWM_Obj_Destroy(ePPVDETAIL_FMT_ID);

	GWM_Obj_Destroy(ePPVDETAIL_FRAME_ID);

	GWM_Obj_Destroy(ePPVDETAIL_TXT_TITLE_ID);

	return hRet;
}

#define _________DRAW_FUNC__________
STATIC BUS_Result_t			xmgr_cas_NaPpvDetailDrawFrame(	OSD_Screen_t *pScreen,
															OSD_Area_t *pArea,
															OSD_Rect_t *pRect,
															GWM_GuiObject_t pObject,
															void *pArg,
															int lParam1)
{
	BUS_Result_t				lResult = ERR_INITIAL_FAILED;
	EPG_UI_IMAGE_t			*pstImageInfo = NULL;
	OSD_StockImage_t		stLeftImg = {0, };
	OSD_StockImage_t		stCenterImg = {0, };
	OSD_StockImage_t		stRightImg = {0, };

	lResult = EPGUISET_GetBlockImage(EPG_IMG_WINDOW_TD517_ID, &pstImageInfo);
	if(lResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailDrawFrame] EPGUISET_GetBlockImage fail!!\n");
		goto EXIT_PPVDETAIL_FUNC;
	}

	if(AP_StockImg_GetImage(pstImageInfo->lImage1, &stLeftImg))
	{
 		HxLOG_Print("[xmgr_cas_NaPpvDetailDrawFrame] AP_StockImg_GetImage fail!!\n");
 		goto EXIT_PPVDETAIL_FUNC;
	}
	if(AP_StockImg_GetImage(pstImageInfo->lImage2, &stCenterImg))
	{
 		HxLOG_Print("[xmgr_cas_NaPpvDetailDrawFrame] AP_StockImg_GetImage fail!!\n");
 		goto EXIT_PPVDETAIL_FUNC;
	}
	if(AP_StockImg_GetImage(pstImageInfo->lImage3, &stRightImg))
	{
 		HxLOG_Print("[xmgr_cas_NaPpvDetailDrawFrame] AP_StockImg_GetImage fail!!\n");
 		goto EXIT_PPVDETAIL_FUNC;
	}

	OSD_GFX_BlendStockFrame(	pScreen,
								pArea->x1+pRect->x,
								pArea->y1+pRect->y,
								pArea->x1+pRect->x+pRect->w,
								pArea->y1+pRect->y+pRect->h,
								&stLeftImg,
								&stCenterImg,
								&stRightImg,
								0);

EXIT_PPVDETAIL_FUNC:

	return lResult;
}


#define PPVDETAIL_ITEM_IMAGE_OFFSET_Y			2
#define PPVDETAIL_ITEM_IMAGE_MARGIN_W			5
#define EXTPLATE_ITEM_IMAGE_L_MARGIN_W			20
#define MAX_DETAIL_LANGCODE						3
STATIC BUS_Result_t			xmgr_cas_NaPpvDetailDrawItems(int lId, OSD_Screen_t *pScreen, FMT_DrawItem_t *pDrawItem)
{
	BUS_Result_t 			lResult = ERR_FAIL;
	HERROR					hError = ERR_FAIL;
	FMT_OBJECT				pObject = NULL;
	OSD_Rect_t				stItemRect = {0, };
	OSD_Font_t				*pFont = OSD_FONT_Get(eFont_SystemNormal);
	HUINT8					*pucToken = NULL;
	HUINT8					*pucTempString = NULL;
	HUINT32					ulCurrentX = 0;
	HINT32					lTokenMove = 0;
	HUINT8					ucMainGenreValue = 0;
	HINT32					nImgId = eImg_NULL;
	HINT32					lTokenValue = 0;
	TEXT_DRAW_INFO_t		stTextInfo = {0, };
	EPG_UI_IMAGE_t			*pstImageInfo = NULL;
	HUINT8					*pucHead = NULL;
	HUINT8					*pucTail = NULL;
	HUINT8					*pucTraverse = NULL;
	HUINT8					ucCustomTag = 0;
	HUINT8					aucLangCode[MAX_DETAIL_LANGCODE] = {0, };
	HINT32					lLangCodeLen = 0;
	HUINT32					ulBeforeStringLength = 0;
	HUINT32					ulTotalLength = 0;
	HINT32					lIsStringMatched = 0;
	HUINT32					ulStartGenernIconIdx;
	OSD_Rect_t				stRect = {0, };
	OSD_Rect_t				stTextRect = {0, };
	OSD_StockImage_t		stStockImg = {0, };

#if defined(CONFIG_OP_UK_DTT)
	ulStartGenernIconIdx = EPG_IMG_GENRE_UNCLASSIFIED_C_ID;
#else
	ulStartGenernIconIdx = EPG_IMG_GENRE_MOVIE_C_ID;
#endif
	pucToken = GetTextBuffer(TEXT_SIZE64);

	pObject = (FMT_OBJECT)BUS_MGR_GetObject(lId);
	if(pObject == NULL)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailDrawItems] BUS_MGR_GetObject fail \n");
		goto EXIT_PPVDETAIL_FUNC;
	}
	if(pDrawItem == NULL)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailDrawItems] pDrawItem == NULL \n");
		goto EXIT_PPVDETAIL_FUNC;
	}
	if(pDrawItem->str == NULL)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailDrawItems] pDrawItem->str == NULL \n");
		goto EXIT_PPVDETAIL_FUNC;
	}

	pucTempString = pDrawItem->str;

	hError = AP_UTIL_GetLanguageCode(pucTempString, aucLangCode, &lLangCodeLen);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailDrawItems] AP_UTIL_GetLanguageCode fail \n");
	}

/*****************************************************************************************
  *
  * costom tag parsing
  *
  *****************************************************************************************/
	pucTraverse = &pucTempString[lLangCodeLen];
	switch(*pucTraverse)
	{
		case FMT_CUSTOMTAG_ON :
			pucTraverse++;
			switch(*pucTraverse)
			{
				case FMT_CUSTOMTAG_IMAGE :
					if( *(pucTraverse + 1) == FMT_CUSTOMTAG_DATA_START)
					{
						pucTraverse++;

						lTokenMove = 0;
						lResult = xmgr_cas_NaPpvDetailGetTokenValue(pucTraverse, pucToken, &lTokenMove);
						if(lResult != ERR_BUS_NO_ERROR)
						{
							HxLOG_Print("[xmgr_cas_NaPpvDetailDrawItems] xmgr_cas_NaPpvDetailGetTokenValue fail \n");
							goto EXIT_PPVDETAIL_FUNC;
						}
						sscanf((char*)pucToken, "%d", &lTokenValue);
						ucMainGenreValue = (HUINT8)lTokenValue;

						ucCustomTag = FMT_CUSTOMTAG_IMAGE;

						pucTempString = pucTraverse + lTokenMove;
						pucTraverse = NULL;
					}
					break;

				case FMT_CUSTOMTAG_IMAGE_ID:
					if( *(pucTraverse + 1) == FMT_CUSTOMTAG_DATA_START)
					{
						pucTraverse++;

						lTokenMove = 0;
						lResult = xmgr_cas_NaPpvDetailGetTokenValue(pucTraverse, pucToken, &lTokenMove);
						if(lResult != ERR_BUS_NO_ERROR)
						{
							HxLOG_Print("[xmgr_cas_NaPpvDetailDrawItems] xmgr_cas_NaPpvDetailGetTokenValue fail \n");
							goto EXIT_PPVDETAIL_FUNC;
						}
						sscanf((char*)pucToken, "%d", &lTokenValue);
						nImgId = lTokenValue;

						ucCustomTag = FMT_CUSTOMTAG_IMAGE_ID;

						pucTempString = pucTraverse + lTokenMove;
						pucTraverse = NULL;
					}
					break;
				default :
					break;

			}
			break;

		default :
			break;

	}


	OSD_GFX_CopyRect(&stItemRect, &(pDrawItem->itemRect));

	stTextInfo.ulTextColor = pDrawItem->stTextProperty.cFontColor;
	stTextInfo.ulFontSize = pDrawItem->stTextProperty.ulFontSize;
//	stTextInfo.ulFontSize = PPVDETAIL_CONTENTS_FONT_SIZE;
	stTextInfo.ulDrawMode = TEXTMODE_NORMAL;
	stTextInfo.ulXAlign = TEXT_ALIGN_LEFT;
	stTextInfo.ulYAlign = TEXT_ALIGN_MIDDLE;

	if(ucCustomTag == FMT_CUSTOMTAG_IMAGE || ucCustomTag == FMT_CUSTOMTAG_IMAGE_ID)
	{
		pucHead = GetTextBuffer(TEXT_SIZE128);
		pucTail = GetTextBuffer(TEXT_SIZE128);

		HxSTD_memset(pucHead, 0x00, TEXT_SIZE128);
		HxSTD_memset(pucTail, 0x00, TEXT_SIZE128);

		ulTotalLength = SvcCas_UtilStrLen(pucTempString);
		if(ulTotalLength == 0)
		{
			HxLOG_Print("[xmgr_cas_NaPpvDetailDrawItems] ulTotalLength == 0 \n");
			goto EXIT_PPVDETAIL_FUNC;
		}

		pucTraverse = pucTempString;
		while(*pucTraverse)
		{
			lIsStringMatched = MWC_UTIL_DvbStrncmp(pucTraverse,	FMT_CUSTOMTAG_IMAGE_INDICATOR,
															FMT_CUSTOMTAG_IMAGE_INDICATOR_LEN);
			if(lIsStringMatched != 0)
			{
				goto STRING_LOOP_END;
			}

/*****************************************************************************************
*
* 머리 문자열을 buf에 복사하고 문자열의 width를 구함
*
*****************************************************************************************/
			ulBeforeStringLength = (HUINT8)(pucTraverse - pucTempString);
			MWC_UTIL_DvbStrncpy(pucHead, pucTempString, ulBeforeStringLength);
			OSD_FONT_SetSize(pFont, stTextInfo.ulFontSize);
			ulCurrentX = OSD_FONT_GetStringPixelWidth (pFont, eHxCP_UNICODE_UTF8, pucHead, ulBeforeStringLength);

/*****************************************************************************************
*
* 머리 문자열 그리고
*
*****************************************************************************************/
			OSD_GFX_CopyRect(&stTextRect, &stItemRect);
 			AP_UTIL_DrawFormattedText(pScreen, pFont, &stTextRect, &stTextInfo, pucHead);

/*****************************************************************************************
*
* 아이콘 그리고
*
*****************************************************************************************/
			if(ucCustomTag == FMT_CUSTOMTAG_IMAGE)
			{
#if defined(CONFIG_OP_UK_DTT)
				lResult = EPGUISET_GetGenreImage(ulStartGenernIconIdx + ucMainGenreValue, &pstImageInfo);
#else
				lResult = EPGUISET_GetGenreImage(ulStartGenernIconIdx + ucMainGenreValue - 1, &pstImageInfo);
#endif
				if (lResult != ERR_BUS_NO_ERROR)
				{
					HxLOG_Print("[xmgr_cas_NaPpvDetailDrawItems] EPGUISET_GetGenreImage fail!!\n");
					goto EXIT_PPVDETAIL_FUNC;
				}
				if(AP_StockImg_GetImage(pstImageInfo->lImage1, &stStockImg))
				{
					HxLOG_Print("[xmgr_cas_NaPpvDetailDrawItems] AP_StockImg_GetImage fail!!\n");
					goto EXIT_PPVDETAIL_FUNC;
				}
			}
			else if(ucCustomTag == FMT_CUSTOMTAG_IMAGE_ID && nImgId != eImg_NULL)
			{
				if(AP_StockImg_GetImage(nImgId, &stStockImg))
				{
					HxLOG_Print("[xmgr_cas_NaPpvDetailDrawItems] AP_StockImg_GetImage fail!!\n");
					goto EXIT_PPVDETAIL_FUNC;
				}
			}
			else
			{
				HxLOG_Print("[xmgr_cas_NaPpvDetailDrawItems] Tag parsing error. ucCustomTag(%d) nImgId(%d)\n", ucCustomTag, nImgId);
				goto EXIT_PPVDETAIL_FUNC;
			}

			OSD_GFX_SetRect(&stRect,	stTextRect.x + ulCurrentX,
										stTextRect.y + PPVDETAIL_ITEM_IMAGE_OFFSET_Y,
										stStockImg.width,
										stStockImg.height);
#if defined(CONFIG_OP_UK_DTT)
			if(ucCustomTag == FMT_CUSTOMTAG_IMAGE_ID && (nImgId == e172_1_26_HD_Simulcast || nImgId == e172_1_26_SD_Simulcast))
			{
				stTextInfo.ulFontSize = FONT_SIZE_XS;
				stTextInfo.ulTextColor = COL(C_T_HD_S);

				ulCurrentX += EXTPLATE_ITEM_IMAGE_L_MARGIN_W;
				OSD_GFX_SetRect(&stRect,	stTextRect.x + ulCurrentX,
											stTextRect.y,
											stStockImg.width,
											stStockImg.height);
			}
#endif

			OSD_GFX_FB_Blend(stStockImg.hBuffer, stStockImg.width, stStockImg.height, pScreen, &stRect);

			ulCurrentX += (stStockImg.width + PPVDETAIL_ITEM_IMAGE_MARGIN_W);

/*****************************************************************************************
*
* 꼬랑지 문자열을 그림.
*
*****************************************************************************************/
			if(ulTotalLength -ulBeforeStringLength - FMT_CUSTOMTAG_IMAGE_INDICATOR_LEN == 0)
			{
				break;
			}

			// UTF-8 type으로만 와야 함
			pucTail[0] = 0x15;

			MWC_UTIL_DvbStrncpy(&pucTail[1],	pucTempString+ulBeforeStringLength + FMT_CUSTOMTAG_IMAGE_INDICATOR_LEN,
										ulTotalLength -ulBeforeStringLength - FMT_CUSTOMTAG_IMAGE_INDICATOR_LEN);
			OSD_GFX_SetRect(&stTextRect,	stTextRect.x + ulCurrentX,
										stTextRect.y,
										stTextRect.w-ulCurrentX,
										stTextRect.h);
			AP_UTIL_DrawFormattedText(pScreen, pFont, &stTextRect, &stTextInfo, pucTail);

STRING_LOOP_END :
			pucTraverse++;
		}
	}
	else
	{
		OSD_GFX_CopyRect(&stTextRect, &stItemRect);
		AP_UTIL_DrawFormattedText(pScreen, pFont, &stTextRect, &stTextInfo, pucTempString);
	}


	lResult= ERR_BUS_NO_ERROR;

EXIT_PPVDETAIL_FUNC:
	if(pFont != NULL)
	{
		OSD_FONT_Release(pFont);
	}

	return lResult;
}


STATIC BUS_Result_t			xmgr_cas_NaPpvDetailGetTokenValue(HUINT8 *pszContens, HUINT8 *pszToken, HINT32 *plMove)
{
	BUS_Result_t 			lResult = ERR_FAIL;
	HINT32					lIndex = 0;
	HUINT8					*pszTemp = NULL;

	if(pszContens == NULL || pszToken == NULL || plMove == NULL)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailGetTokenValue] param fail \n");
		goto EXIT_PPVDETAIL_FUNC;
	}

	pszTemp = pszToken;
	*plMove = 0;

	if(*pszContens == FMT_CUSTOMTAG_DATA_START)
	{
		pszContens++;

		lIndex++;

		while(*pszContens != FMT_CUSTOMTAG_DATA_END)
		{
			if(IS_SPACECHAR(*pszContens))
			{
				goto TOKEN_LOOP_END;
			}

			*pszTemp = *pszContens;
			pszTemp++;

TOKEN_LOOP_END :
			pszContens++;
			lIndex++;
		}
		lIndex++;
	}

	*pszTemp ='\0';
	*plMove = lIndex;

	lResult = ERR_BUS_NO_ERROR;

EXIT_PPVDETAIL_FUNC :

	return lResult;

}




#define PPVDETAIL_SCROLL_THOMB_MIN_H				20

STATIC BUS_Result_t			xmgr_cas_NaPpvDetailDrawScroll(	OSD_Screen_t *pScreen,
															OSD_Area_t *pArea,
															OSD_Rect_t *pRect,
															FMT_OBJECT pObject)
{
	BUS_Result_t 			lResult = ERR_BUS_NO_ERROR;
	HINT32					lX = 0;
	HINT32					lY = 0;
	HINT32					lW = 0;
	HINT32					lH = 0;
	HINT32					lTotalPage = 0;
	HINT32					lIndex = 0;
//	HINT32					lThumbIndex = 0;
	HUINT32					ulAttrib = 0;
	HUINT32					ulStartIndex = 0;
	HUINT32					ulLinePerPage = 0;
	HUINT32					ulItemNum = 0;
	HUINT32					ulThumbHeight = 0;
	HUINT32					ulThumbY = 0;
	HINT32					lHeadHeight = 0;
	HINT32					lBodyHeight = 0;
	HINT32					lTailHeight = 0;
	HINT32					lBgHeight = 0;
	OSD_StockImage_t		stThumbHeadImg = {0, };
	OSD_StockImage_t		stThumbBodyImg = {0, };
	OSD_StockImage_t		stThumbTailImg = {0, };
	OSD_StockImage_t		stBgImg = {0, };
	OSD_Rect_t				stRect = {0, };
	EPG_UI_COMPONENT_t		*pstUiComp;
	EPG_UI_IMAGE_t			*pstImageInfo;


	GWM_FMT_GetProperty(pObject, &ulAttrib, &ulStartIndex, &ulLinePerPage, &ulItemNum);
	if (ulItemNum <= ulLinePerPage)
	{
		/* skip case */
		goto EXIT_PPVDETAIL_FUNC;
	}

	lResult = EPGUISET_GetDetailInfoUiComponent(ePPVDETAIL_FMT_SCROLL_ID, &pstUiComp);
	if (lResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailDrawScroll] EPGUISET_GetDetailInfoUiComponent() fail \n");
		goto EXIT_PPVDETAIL_FUNC;
	}
	lResult = EPGUISET_GetBlockImage(EPG_IMG_SCROLL_B_ID, &pstImageInfo);
	if(lResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailDrawScroll] EPGUISET_GetBlockImage fail!!\n");
		goto EXIT_PPVDETAIL_FUNC;
	}

	if(AP_StockImg_GetImage(pstImageInfo->lImage1, &stThumbHeadImg))
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailDrawScroll] AP_StockImg_GetImage fail!!\n");
		goto EXIT_PPVDETAIL_FUNC;
	}
	if(AP_StockImg_GetImage(pstImageInfo->lImage2, &stThumbBodyImg))
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailDrawScroll] AP_StockImg_GetImage fail!!\n");
		goto EXIT_PPVDETAIL_FUNC;
	}
	if(AP_StockImg_GetImage(pstImageInfo->lImage3, &stThumbTailImg))
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailDrawScroll] AP_StockImg_GetImage fail!!\n");
		goto EXIT_PPVDETAIL_FUNC;
	}
	if(AP_StockImg_GetImage(pstImageInfo->lImage4, &stBgImg))
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailDrawScroll] AP_StockImg_GetImage fail!!\n");
		goto EXIT_PPVDETAIL_FUNC;
	}

	lHeadHeight = stThumbHeadImg.height;
	lBodyHeight = stThumbBodyImg.height;
	lTailHeight = stThumbTailImg.height;
	lBgHeight = stBgImg.height;

	lX = pArea->x1+ pstUiComp->stRect.x;
	lY = pArea->y1+ pstUiComp->stRect.y + PPVDETAIL_FMT_MARGIN;
	lW = pstUiComp->stRect.w;
	lH = pstUiComp->stRect.h - (PPVDETAIL_FMT_MARGIN*2);

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
	if (ulThumbHeight < PPVDETAIL_SCROLL_THOMB_MIN_H)
	{
		ulThumbHeight = PPVDETAIL_SCROLL_THOMB_MIN_H;
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
/*****************************************************************************************
  *
  * draw bg
  *
  *****************************************************************************************/
		if (lIndex != ulThumbY)
		{
			OSD_GFX_SetRect(&stRect, lX, lIndex, stBgImg.width, stBgImg.height);
			OSD_GFX_FB_Blend(stBgImg.hBuffer, stBgImg.width, stBgImg.height, pScreen, &stRect);
			lIndex += lBgHeight;
		}
/*****************************************************************************************
  *
  *  draw thumb
  *
  *****************************************************************************************/
		else
		{
			OSD_GFX_BlendStockVFrame (pScreen, lX, lIndex, lX + stThumbHeadImg.width, lIndex + ulThumbHeight,
									&stThumbHeadImg, &stThumbBodyImg, &stThumbTailImg, 100);
#if 0
			OSD_GFX_SetRect(&stRect, lX, lIndex, stThumbHeadImg.width, stThumbHeadImg.height);
			OSD_GFX_FB_Blend(stThumbHeadImg.hBuffer, stThumbHeadImg.width, stThumbHeadImg.height, pScreen, &stRect);

			for (lThumbIndex = lIndex + lHeadHeight; lThumbIndex < lIndex + ulThumbHeight - lTailHeight; lThumbIndex++)
			{
				OSD_GFX_SetRect(&stRect, lX, lThumbIndex, stThumbBodyImg.width, stThumbBodyImg.height);
				OSD_GFX_FB_Blend(stThumbBodyImg.hBuffer, stThumbBodyImg.width, stThumbBodyImg.height, pScreen, &stRect);
			}

			OSD_GFX_SetRect(&stRect, lX, lThumbIndex, stThumbTailImg.width, stThumbTailImg.height);
			OSD_GFX_FB_Blend(stThumbTailImg.hBuffer, stThumbTailImg.width, stThumbTailImg.height, pScreen, &stRect);
#endif
			lIndex += ulThumbHeight;
		}
	}

	lResult = ERR_BUS_NO_ERROR;

EXIT_PPVDETAIL_FUNC :

	return lResult;

}


#if	defined(CONFIG_MW_EPG_TVTV)
STATIC BUS_Result_t			xmgr_cas_NaPpvDetailDrawImage(	OSD_Screen_t *pScreen,
															OSD_Area_t *pArea,
															OSD_Rect_t *pRect,
															GWM_GuiObject_t pObject,
															void *pArg,
															int lParam1)
{
	BUS_Result_t				lResult = ERR_INITIAL_FAILED;
//	EPG_APEVENT_t			*pEvent = NULL;
	FMT_OBJECT				pFmtObject = NULL;
	HUINT32					ulAttrib = 0;
	HUINT32					ulStartIndex = 0;
	HUINT32					ulLinePerPage = 0;
	HUINT32					ulItemNum = 0;
	OSD_Rect_t				stBaseRect = {0, };

	if(s_PPVDetailInfo_Context.bImage == FALSE)
	{
		HxLOG_Info("[xmgr_cas_NaPpvDetailDrawImage] s_PPVDetailInfo_Context.bImage == FALSE\n");
		goto EXIT_PPVDETAIL_FUNC;
	}

	pFmtObject =  (FMT_OBJECT)BUS_MGR_GetObject(ePPVDETAIL_FMT_ID);
	if(pFmtObject == NULL)
	{
		HxLOG_Info("[xmgr_cas_NaPpvDetailDrawScroll] GWM_Obj_GetObjectByID fail!!\n");
		goto EXIT_PPVDETAIL_FUNC;
	}

	GWM_FMT_GetProperty(pFmtObject, &ulAttrib, &ulStartIndex, &ulLinePerPage, &ulItemNum);
	if(ulStartIndex != 0)
	{
		HxLOG_Info("[xmgr_cas_NaPpvDetailDrawImage] ulStartIndex != 0\n");
		goto EXIT_PPVDETAIL_FUNC;
	}

	stBaseRect.x = pArea->x1 + pRect->x;
	stBaseRect.y = pArea->y1 + pRect->y;
	stBaseRect.w = pRect->w;
	stBaseRect.h = pRect->h;

HxLOG_Info("[xmgr_cas_NaPpvDetailDrawImage] OSD_GFX_DrawBitmapWithScale!!\n");

	OSD_GFX_DrawBitmapWithScale(	pScreen,
									stBaseRect.x,
									stBaseRect.y,
									stBaseRect.w,
									stBaseRect.h,
									(unsigned char*)s_PPVDetailInfo_Context.pucBitmap,
									eBmpBlend_COPY);

EXIT_PPVDETAIL_FUNC:

	return lResult;
}













#define _________HIDDEN_FUNC__________

STATIC void				xmgr_cas_NaPpvDetailCreateTvtvInfo(void)
{
	BUS_MGR_Create(	"xmgr_cas_NaPpvDetailTvtvInfoProc",
						APP_DIALOGBOX_PRIORITY,
						xmgr_cas_NaPpvDetailTvtvInfoProc,
						0,0,0,0);

	return;
}


STATIC BUS_Result_t		xmgr_cas_NaPpvDetailTvtvInfoProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(message)
	{
		case MSG_GWM_CREATE :
			xmgr_cas_NaPpvDetailInitTvtvInfoObj();
			return MESSAGE_BREAK;

		case MSG_GWM_KEYDOWN:
			switch(p1)
			{
				case KEY_0:
				case KEY_1:
				case KEY_2:
				case KEY_3:
				case KEY_4:
				case KEY_5:
				case KEY_6:
				case KEY_7:
				case KEY_8:
				case KEY_9:
				case KEY_ARROWLEFT:
				case KEY_ARROWRIGHT:
					return MESSAGE_BREAK;

				case KEY_EXIT :
				case KEY_BACK :
					BUS_MGR_Destroy((GWM_Callback_t)0);
					return MESSAGE_BREAK;
				default:
					break;
			}
			break;

		case MSG_GWM_DESTROY :
			xmgr_cas_NaPpvDetailDestroyTvtvInfoApp();
			return MESSAGE_BREAK;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


STATIC BUS_Result_t		xmgr_cas_NaPpvDetailInitTvtvInfoObj(void)
{
	BUS_Result_t 				lResult = ERR_FAIL;
	EPG_UI_COMPONENT_t			*pstUiComp = NULL;
	OSD_Rect_t					stRect = {0, };


/*****************************************************************************************
  *
  * set app area
  *
  *****************************************************************************************/
	lResult = EPGUISET_GetDetailInfoUiComponent(ePPVDETAIL_FRAME_ID, &pstUiComp);
	if (lResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailInitTvtvInfoObj] EPGUISET_GetDetailInfoUiComponent() fail \n");
		goto EXIT_PPVDETAIL_FUNC;
	}
	GWM_APP_SetAppArea (pstUiComp->stRect.x, pstUiComp->stRect.y, pstUiComp->stRect.w, pstUiComp->stRect.h);

/*****************************************************************************************
  *
  * frame
  *
  *****************************************************************************************/
	OSD_GFX_SetRect (&stRect, 0, 0, pstUiComp->stRect.w, pstUiComp->stRect.h);
	GWM_Frame_Create (ePPVDETAIL_FRAME_ID, &stRect, "AP_DetailInfo_Frame");
	GWM_APP_SetObjectDrawMethod(ePPVDETAIL_FRAME_ID, xmgr_cas_NaPpvDetailTvtvInfoDrawFrame);

EXIT_PPVDETAIL_FUNC:

	return lResult;
}


STATIC BUS_Result_t		xmgr_cas_NaPpvDetailDestroyTvtvInfoApp(void)
{
	BUS_Result_t 		hRet = ERR_INITIAL_FAILED;

	GWM_Obj_Destroy(ePPVDETAIL_FRAME_ID);

	return hRet;
}

STATIC BUS_Result_t			xmgr_cas_NaPpvDetailTvtvInfoDrawFrame(	OSD_Screen_t *pScreen,
															OSD_Area_t *pArea,
															OSD_Rect_t *pRect,
															GWM_GuiObject_t pObject,
															void *pArg,
															int lParam1)
{
	BUS_Result_t				lResult = ERR_INITIAL_FAILED;
	EPG_UI_IMAGE_t			*pstImageInfo = NULL;
	OSD_StockImage_t		stLeftImg = {0, };
	OSD_StockImage_t		stCenterImg = {0, };
	OSD_StockImage_t		stRightImg = {0, };
	OSD_Font_t				*pFont = OSD_FONT_Get(eFont_SystemNormal);
	TEXT_DRAW_INFO_t		stTextInfo = {0, };
//	HUINT32					ulIndex = 0;
	OSD_Rect_t				stTextRect = {0, };

	stTextRect.x = pArea->x1 + pRect->x + 20;
	stTextRect.y = pArea->x1 + pRect->y + 60;
	stTextRect.w = pRect->w - 60;
	stTextRect.h = 35;

	lResult = EPGUISET_GetBlockImage(EPG_IMG_WINDOW_TD517_ID, &pstImageInfo);
	if(lResult != ERR_BUS_NO_ERROR)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDetailTvtvInfoDrawFrame] EPGUISET_GetBlockImage fail!!\n");
		goto EXIT_PPVDETAIL_FUNC;
	}

	if(AP_StockImg_GetImage(pstImageInfo->lImage1, &stLeftImg))
	{
 		HxLOG_Print("[xmgr_cas_NaPpvDetailTvtvInfoDrawFrame] AP_StockImg_GetImage fail!!\n");
 		goto EXIT_PPVDETAIL_FUNC;
	}
	if(AP_StockImg_GetImage(pstImageInfo->lImage2, &stCenterImg))
	{
 		HxLOG_Print("[xmgr_cas_NaPpvDetailTvtvInfoDrawFrame] AP_StockImg_GetImage fail!!\n");
 		goto EXIT_PPVDETAIL_FUNC;
	}
	if(AP_StockImg_GetImage(pstImageInfo->lImage3, &stRightImg))
	{
 		HxLOG_Print("[xmgr_cas_NaPpvDetailTvtvInfoDrawFrame] AP_StockImg_GetImage fail!!\n");
 		goto EXIT_PPVDETAIL_FUNC;
	}

	OSD_GFX_BlendStockFrame(	pScreen,
								pArea->x1+pRect->x,
								pArea->y1+pRect->y,
								pArea->x1+pRect->x+pRect->w,
								pArea->y1+pRect->y+pRect->h,
								&stLeftImg,
								&stCenterImg,
								&stRightImg,
								0);


{
	HUINT8					*pucFileName = NULL;
	SvcEpg_TvtvLogTable_t			*pstLog = NULL;
	HUINT8					*pucFile = NULL;
	HUINT32					ulSize = 0;
	HUINT32					ulIndex = 0;


	pucFileName = GetTextBuffer(TEXT_SIZE64);

	sprintf((char*)pucFileName, "%s%s",EPG_FS_HD1, "tvtv.log");

	HLIB_FILE_ReadFile((const HUINT8 *)pucFileName, &pucFile, &ulSize);
	if(pucFile == NULL)
	{
		HxLOG_Info("[xmgr_cas_NaPpvDetailDrawImage] ulStartIndex != 0\n");

//		OxAP_Free(pucFile);
		goto EXIT_PPVDETAIL_FUNC;
	}
	pstLog = (SvcEpg_TvtvLogTable_t*)pucFile;


	stTextInfo.ulTextColor = COL(C_T_Dialog_MSG);
	stTextInfo.ulFontSize = FONT_SIZE_XXS;
	stTextInfo.ulDrawMode = TEXTMODE_FORCE_FIT;
	stTextInfo.ulXAlign = TEXT_ALIGN_LEFT;
	stTextInfo.ulYAlign = TEXT_ALIGN_TOP;

HxLOG_Info("[xmgr_cas_NaPpvDetailTvtvInfoDrawFrame] pstLog->aucLog(%s)!!\n", pstLog->aucLog[0]);
	for(ulIndex = 0; ulIndex < 5; ulIndex++)
	{
		AP_UTIL_DrawFormattedText(pScreen, pFont, &stTextRect, &stTextInfo, pstLog->aucLog[ulIndex]);
		stTextRect.y += 35;
	}
	OxAP_Free(pucFile);
}

EXIT_PPVDETAIL_FUNC:

	if(pFont != NULL)
	{
		OSD_FONT_Release(pFont);
	}

	return lResult;
}


#endif



/*********************** End of File ******************************/
