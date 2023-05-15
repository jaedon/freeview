//******************************************************************************
//	INCLUDE
//******************************************************************************
#include <dlih.h>

#include <db_svc.h>
#include <db_param.h>
#include <svc_sys.h>
#include <svc_pipe.h>
#include <svc_cas.h>
#include <isosvc.h>
#include <osd_gfx.h>
#include <osd_font.h>

#include <gwm.h>
#include <mgr_action.h>
#include <mgr_live.h>

#include <menuui_coor.h>
#include <settings.h>

#include <ap_util_osd.h>

#include <surferui_coor.h>

#include <ap_epg_main.h>

#include <namgr_main.h>

#include "../../local_include/_xmgr_cas_na_util.h"
#include "../../local_include/_xmgr_cas_na_adapt.h"
#include "../../local_include/_xmgr_cas_na.h"



/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#ifdef CONFIG_DEBUG
HUINT32 g_ApPPVBP = 0x00;
#endif
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



#define PPV_BPOP_BTN_MAX				3
#define PPV_BPOP_BTN_EVT_STR_SIZE		24
#define PPV_BPOP_BTN_EVT_TIME_SIZE		24

// dimension
#define D_PPV_BPOP_BTN_LINE_SPACE		44

// window
#define D_PPV_BPOP_DIALOG_X			326
#define D_PPV_BPOP_DIALOG_Y			175
#define D_PPV_BPOP_DIALOG_W			628
#define D_PPV_BPOP_DIALOG_H			370

// frame
#define D_PPV_BPOP_DIALOG_FRAME_X	0
#define D_PPV_BPOP_DIALOG_FRAME_Y	0
#define D_PPV_BPOP_DIALOG_FRAME_W	D_PPV_BPOP_DIALOG_W
#define D_PPV_BPOP_DIALOG_FRAME_H	D_PPV_BPOP_DIALOG_H

// inside box
#define D_PPV_BPOP_D_INNER_B_X		36
#define D_PPV_BPOP_D_INNER_B_Y		80
#define D_PPV_BPOP_D_INNER_B_W		556
#define D_PPV_BPOP_D_INNER_B_H		193

// text
#define D_PPV_BPOP_D_TXT_B_X			D_PPV_BPOP_DIALOG_X
#define D_PPV_BPOP_D_TXT_B_Y			D_PPV_BPOP_DIALOG_Y
#define D_PPV_BPOP_D_TXT_B_W			D_PPV_BPOP_DIALOG_W
#define D_PPV_BPOP_D_TXT_B_H			D_PPV_BPOP_DIALOG_H

#define D_PPV_BPOP_D_TXT1_B_X			65
#define D_PPV_BPOP_D_TXT1_B_Y			116
#define D_PPV_BPOP_D_TXT1_B_W		498
#define D_PPV_BPOP_D_TXT1_B_H			34

#define D_PPV_BPOP_D_TXT2_B_X			65
#define D_PPV_BPOP_D_TXT2_B_Y			D_PPV_BPOP_D_TXT1_B_Y + D_PPV_BPOP_D_TXT1_B_H
#define D_PPV_BPOP_D_TXT2_B_W		498
#define D_PPV_BPOP_D_TXT2_B_H			34

#define D_PPV_BPOP_D_TXT3_B_X			65
#define D_PPV_BPOP_D_TXT3_B_Y			D_PPV_BPOP_D_TXT2_B_Y + D_PPV_BPOP_D_TXT2_B_H
#define D_PPV_BPOP_D_TXT3_B_W		498
#define D_PPV_BPOP_D_TXT3_B_H			34

#define D_PPV_BPOP_D_TXT4_B_X			65
#define D_PPV_BPOP_D_TXT4_B_Y			D_PPV_BPOP_D_TXT3_B_Y + D_PPV_BPOP_D_TXT3_B_H
#define D_PPV_BPOP_D_TXT4_B_W		498
#define D_PPV_BPOP_D_TXT4_B_H			34

#define D_PPV_BPOP_D_TXT5_B_X			65
#define D_PPV_BPOP_D_TXT5_B_Y			D_PPV_BPOP_D_TXT4_B_Y + D_PPV_BPOP_D_TXT4_B_H
#define D_PPV_BPOP_D_TXT5_B_W		498
#define D_PPV_BPOP_D_TXT5_B_H			34

// button
#define D_PPV_BPOP_D_BTN1_B_X			34-5
#define D_PPV_BPOP_D_BTN1_B_Y			298-5
#define D_PPV_BPOP_D_BTN1_B_W		180+10
#define D_PPV_BPOP_D_BTN1_B_H			44+10

#define D_PPV_BPOP_D_BTN2_B_X			224-5
#define D_PPV_BPOP_D_BTN2_B_Y			298-5
#define D_PPV_BPOP_D_BTN2_B_W		180+10
#define D_PPV_BPOP_D_BTN2_B_H			44+10

#define D_PPV_BPOP_D_BTN3_B_X			414-5
#define D_PPV_BPOP_D_BTN3_B_Y			298-5
#define D_PPV_BPOP_D_BTN3_B_W		180+10
#define D_PPV_BPOP_D_BTN3_B_H			44+10


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/**@brief Reminder Conflict BTN enum. */
typedef enum
{
	eBTN_NUMBER_1 = 0,
	eBTN_NUMBER_2,
	eBTN_NUMBER_3
} eBtnNumber_t;


/**@brief Reminder Conflict UI enum. */
typedef enum
{
	PPV_BPOP_MAIN_FRAME_ID = 0,
	PPV_BPOP_FRAME_ID,
	PPV_BPOP_DIALOG_INNER_B_ID,
	PPV_BPOP_TEXT_ID,
	PPV_BPOP_TEXT1_ID,
	PPV_BPOP_TEXT2_ID,
	PPV_BPOP_TEXT3_ID,
	PPV_BPOP_TEXT4_ID,
	PPV_BPOP_TEXT5_ID,
	PPV_BPOP_BTN_1_BASE_ID,
	PPV_BPOP_BTN_2_BASE_ID,
	PPV_BPOP_BTN_3_BASE_ID,
	PPV_BPOP_ID_MAX,
} eRemcftUiCompIds;


/** @brief Reminder Conflict BTN Content structure. */
typedef struct tag_XmgrCas_NaPpvBPopBtn_t
{
	HUINT32		ulSlot;
	HUINT8		ucString[PPV_BPOP_BTN_EVT_STR_SIZE];
	HUINT8		ucTime[PPV_BPOP_BTN_EVT_TIME_SIZE];
} XmgrCas_NaPpvBPopBtn_t;


/** @brief Reminder Conflict Contents structure. */
typedef struct tag_XmgrCas_NaPpvBPopContent_t
{
	HUINT8			*pDlgMsg;			/**< Dialog Message. */
	HUINT32			ulBtnCnt;			/**< Btn Count. */
	EPG_APEVENT_t	ApEvt;
	HBOOL			bDisableOK;
	PPV_EVENT_INFO	*pstPPVEvtInfo;
	TCaRequest		*pRequest;
	TCaProduct		*pProduct;
	TCreditAmount	Price;
	TCreditAmount	Credit;
	TCreditStatus		CreditStatus;
	XmgrCas_NaPpvBPopBtn_t		stBtn[PPV_BPOP_BTN_MAX];
} XmgrCas_NaPpvBPopContent_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
const STATIC OSD_Rect_t		s_stRemCftUiRectComponent[] =
{
		{	D_PPV_BPOP_DIALOG_X,				D_PPV_BPOP_DIALOG_Y, 				D_PPV_BPOP_DIALOG_W,				D_PPV_BPOP_DIALOG_H			},
		{	D_PPV_BPOP_DIALOG_FRAME_X,			D_PPV_BPOP_DIALOG_FRAME_Y,			D_PPV_BPOP_DIALOG_FRAME_W,			D_PPV_BPOP_DIALOG_FRAME_H	},
		{	D_PPV_BPOP_D_INNER_B_X,				D_PPV_BPOP_D_INNER_B_Y,				D_PPV_BPOP_D_INNER_B_W,			D_PPV_BPOP_D_INNER_B_H		},
		{	D_PPV_BPOP_D_TXT_B_X,				D_PPV_BPOP_D_TXT_B_Y,				D_PPV_BPOP_D_TXT_B_W,				D_PPV_BPOP_D_TXT_B_H		},
		{	D_PPV_BPOP_D_TXT1_B_X,				D_PPV_BPOP_D_TXT1_B_Y,				D_PPV_BPOP_D_TXT1_B_W,				D_PPV_BPOP_D_TXT1_B_H		},
		{	D_PPV_BPOP_D_TXT2_B_X,				D_PPV_BPOP_D_TXT2_B_Y,				D_PPV_BPOP_D_TXT2_B_W,				D_PPV_BPOP_D_TXT2_B_H		},
		{	D_PPV_BPOP_D_TXT3_B_X,				D_PPV_BPOP_D_TXT3_B_Y,				D_PPV_BPOP_D_TXT3_B_W,				D_PPV_BPOP_D_TXT3_B_H		},
		{	D_PPV_BPOP_D_TXT4_B_X,				D_PPV_BPOP_D_TXT4_B_Y,				D_PPV_BPOP_D_TXT4_B_W,				D_PPV_BPOP_D_TXT4_B_H		},
		{	D_PPV_BPOP_D_TXT5_B_X,				D_PPV_BPOP_D_TXT5_B_Y,				D_PPV_BPOP_D_TXT5_B_W,				D_PPV_BPOP_D_TXT5_B_H		},
		{	D_PPV_BPOP_D_BTN1_B_X,				D_PPV_BPOP_D_BTN1_B_Y,				D_PPV_BPOP_D_BTN1_B_W,				D_PPV_BPOP_D_BTN1_B_H		},
		{	D_PPV_BPOP_D_BTN2_B_X,				D_PPV_BPOP_D_BTN2_B_Y,				D_PPV_BPOP_D_BTN2_B_W,				D_PPV_BPOP_D_BTN2_B_H		},
		{	D_PPV_BPOP_D_BTN3_B_X,				D_PPV_BPOP_D_BTN3_B_Y,				D_PPV_BPOP_D_BTN3_B_W,				D_PPV_BPOP_D_BTN3_B_H		},
};

STATIC XmgrCas_NaPpvBPopContent_t		s_stBPOPContents;
STATIC HBOOL					s_BPPOPIsFirst = TRUE;
/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define _________EDIT_FUNTIONS____________
STATIC XmgrCas_NaPpvBPopContent_t * xmgr_cas_NaPpvGetBPOPContents(void)
{
	return &s_stBPOPContents;
}


STATIC void xmgr_cas_NaPpvGetBPOPUiPosition(eRemcftUiCompIds eUiId, OSD_Rect_t *rect)
{
	if ( eUiId >= PPV_BPOP_ID_MAX )
	{
		HxSTD_memcpy(rect, &s_stRemCftUiRectComponent[PPV_BPOP_ID_MAX -1], sizeof(OSD_Rect_t));
	}
	else
	{
		HxSTD_memcpy(rect, &s_stRemCftUiRectComponent[eUiId], sizeof(OSD_Rect_t));
	}
}



STATIC eBtnNumber_t xmgr_cas_NaPpvConvertObjIdToBtnNumber(HINT32 ulObjId)
{
	switch (ulObjId)
	{
		case PPV_BPOP_BTN_1_BASE_ID :
			return  eBTN_NUMBER_1;

		case PPV_BPOP_BTN_2_BASE_ID :
			return eBTN_NUMBER_2;

		case PPV_BPOP_BTN_3_BASE_ID :
			return eBTN_NUMBER_3;

		default :
			HxLOG_Critical("\n\n");
			return eBTN_NUMBER_1;
	}
}

STATIC BUS_Result_t xmgr_cas_NaPpvdrawText(OSD_Screen_t *screen, OSD_Area_t *area,
								OSD_Rect_t *rect, GWM_GuiObject_t object, void *arg, int param1)
{
	HERROR 				hErr = ERR_OK;
	int					x, y, w, h;
	OSD_Rect_t		 	stRect;
	OSD_StockImage_t		stStockImg;
	HUINT32 				ulCol1 = 	COL(C_T_Info_Dialog2);
	HUINT32 				ulCol2 = 	COL(C_T_Calendar01);
	OSD_Font_t			*font = NULL;
	HUINT32				ulFontSize = FONT_SIZE_M;
	HINT32 				nObjectId = 0;
	XmgrCas_NaPpvBPopContent_t	*pContents = xmgr_cas_NaPpvGetBPOPContents();
	HUINT8				*pStr = NULL;
	HxDATETIME_t		stDT;

	x = area->x1 + rect->x;
	y = area->y1 + rect->y;
	w = rect->w;
	h = rect->h;

	NOT_USED_PARAM(arg);
	NOT_USED_PARAM(param1);

	nObjectId = GWM_Obj_GetObjectID(object);

	// programme name
	xmgr_cas_NaPpvGetBPOPUiPosition(PPV_BPOP_TEXT1_ID,  &stRect);
	stRect.x += area->x1;
	stRect.y += area->y1;
	OSD_GFX_DrawTextByLimit(screen, nObjectId, eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_S,
												stRect.x, stRect.y, stRect.w,	GetStrRsc(LOC_CAS_NA_RCS_PROGRAMME), ulCol1);
	stRect.x = stRect.x + stRect.w/2;
	stRect.w = stRect.w / 2;
	OSD_GFX_DrawTextByLimit(screen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_S,
												stRect.x, stRect.y, stRect.w, 	pContents->ApEvt.aucShortEventName, ulCol2);

	// start time
	xmgr_cas_NaPpvGetBPOPUiPosition(PPV_BPOP_TEXT2_ID,  &stRect);
	stRect.x += area->x1;
	stRect.y += area->y1;
	OSD_GFX_DrawTextByLimit(screen, nObjectId, eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_S,
												stRect.x, stRect.y, stRect.w,
												GetStrRsc(LOC_RSV_EDIT_POP_UP_START_TIME_ITEM_ID), ulCol1);
	pStr = GetTextBuffer(TEXT_SIZE128);
	HxSTD_memset(pStr, 0, TEXT_SIZE128);
	HLIB_DATETIME_ConvertUnixTimeToDateTime(pContents->ApEvt.stEventInfo.ulStartTime, &stDT);
	sprintf(pStr, "%02d/%02d %02d:%02d", stDT.stDate.ucDay, stDT.stDate.ucMonth, stDT.stTime.ucHour, stDT.stTime.ucMinute);
	OSD_GFX_DrawTextByLimit(screen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_S,
												stRect.x, stRect.y, stRect.w,	pStr, ulCol2);

	// end time
	xmgr_cas_NaPpvGetBPOPUiPosition(PPV_BPOP_TEXT3_ID,  &stRect);
	stRect.x += area->x1;
	stRect.y += area->y1;
	OSD_GFX_DrawTextByLimit(screen, nObjectId, eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_S,
												stRect.x, stRect.y, stRect.w,
												GetStrRsc(LOC_RSV_EDIT_POP_UP_END_TIME_ITEM_ID), ulCol1);
	pStr = GetTextBuffer(TEXT_SIZE128);
	HxSTD_memset(pStr, 0, TEXT_SIZE128);
	HLIB_DATETIME_ConvertUnixTimeToDateTime(pContents->ApEvt.stEventInfo.ulStartTime+pContents->ApEvt.stEventInfo.ulDuration, &stDT);
	sprintf(pStr, "%02d/%02d %02d:%02d", stDT.stDate.ucDay, stDT.stDate.ucMonth, stDT.stTime.ucHour, stDT.stTime.ucMinute);
	OSD_GFX_DrawTextByLimit(screen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_S,
												stRect.x, stRect.y, stRect.w,	pStr, ulCol2);

	// price
	xmgr_cas_NaPpvGetBPOPUiPosition(PPV_BPOP_TEXT4_ID,  &stRect);
	stRect.x += area->x1;
	stRect.y += area->y1;
	OSD_GFX_DrawTextByLimit(screen, nObjectId, eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_S,
												stRect.x, stRect.y, stRect.w,	GetStrRsc(LOC_CAS_NA_RCS_PRICE), ulCol1);
	pStr = GetTextBuffer(TEXT_SIZE128);
	HxSTD_memset(pStr, 0, TEXT_SIZE128);
 	sprintf(pStr, "%d.%02d %s", pContents->Price/100, pContents->Price%100, GetStrRsc(LOC_CAS_NA_RCS_CREDIT));
	OSD_GFX_DrawTextByLimit(screen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_S,
												stRect.x, stRect.y, stRect.w, 	pStr, ulCol2);

	// current credit
	xmgr_cas_NaPpvGetBPOPUiPosition(PPV_BPOP_TEXT5_ID,  &stRect);
	stRect.x += area->x1;
	stRect.y += area->y1;
	OSD_GFX_DrawTextByLimit(screen, nObjectId, eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_S,
												stRect.x, stRect.y, stRect.w,	GetStrRsc(LOC_CAS_NA_RCS_CURRENT_CREDIT), ulCol1);
	pStr = GetTextBuffer(TEXT_SIZE128);
	HxSTD_memset(pStr, 0, TEXT_SIZE128);
	sprintf(pStr, "%d.%02d %s", pContents->Credit/100, pContents->Credit%100, GetStrRsc(LOC_CAS_NA_RCS_CREDIT));
	OSD_GFX_DrawTextByLimit(screen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_S,
												stRect.x, stRect.y, stRect.w,	pStr, ulCol2);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t		xmgr_cas_NaPpvLNF_DrawBox(OSD_Screen_t *pOsdScreen, OSD_Area_t *appArea,
										OSD_Rect_t *objRect, GWM_GuiObject_t object, void *arg, int bUiTop)
{
	int x1=0, y1=0, x2=0, y2=0;
	OSD_StockImage_t	stImgLeft, stImgCenter, stImgRight;

	x1=appArea->x1+objRect->x;
	y1=appArea->y1+objRect->y;
	x2=appArea->x1+objRect->x+objRect->w - 1;
	y2=appArea->y1+objRect->y+objRect->h - 1;

	// draw frame background
	if (!AP_StockImg_GetImage(e163_2_31_Box_W7_1L, &stImgLeft) &&
		!AP_StockImg_GetImage(e163_2_31_Box_W7_2C, &stImgCenter) &&
		!AP_StockImg_GetImage(e163_2_31_Box_W7_3R, &stImgRight))
	{
		OSD_GFX_BlendStockFrame (pOsdScreen, x1, y1, x2, y2, &stImgLeft, &stImgCenter, &stImgRight, 100);
	}

	return ERR_OK;
}

STATIC BUS_Result_t			xmgr_cas_NaPpvLNF_DialogDisableButton_Draw(OSD_Screen_t *pOsdScreen, OSD_Area_t *appArea,
										OSD_Rect_t *objRect, GWM_GuiObject_t object, void *arg, int bUiTop)
{
	BUTTON_OBJECT	btn = (BUTTON_OBJECT)arg;
	int				x, y, x1, y1, x2, y2, w, h;
	HUINT32			status, length = 0;
	OSD_Font_t		*pFont = NULL;
	char 			*str = NULL;
	OSD_Color_t		txtCol = COLOR_WHITE;
	HUINT32			fontSize = FONT_SIZE_M;
	OSD_StockImage_t		img;
	OSD_Rect_t				dstRect;

	x1 = appArea->x1 + objRect->x;
	y1 = appArea->y1 + objRect->y;
	x2 = appArea->x1 + objRect->x + objRect->w;
	y2 = appArea->y1 + objRect->y + objRect->h;
	w = x2 - x1;
	h = y2 - y1;

	status = GWM_Button_GetStatus(btn);
	if (status & GUIOBJ_DISABLED)
	{
		x1 += 3;
		w -= 6;
		y1 += 3;
		h -= 6;

		if (!AP_StockImg_GetImage(e123_1_00_BTN_D1_N, &img))
		{
			OSD_GFX_SetRect(&dstRect, x1, y1, img.width, img.height);
			OSD_GFX_FB_Blend((unsigned long)img.hBuffer, img.width, img.height, pOsdScreen, &dstRect);
		}
		txtCol = COL(C_T_Disable);
		fontSize = FONT_SIZE_S;
	}
	else if (status & GUIOBJ_HILIGHTED)
	{	/* got focused */
		if (!AP_StockImg_GetImage(e123_1_00_BTN_D1_C, &img))
		{
			if (status & GUIOBJ_PUSHED)
			{
				OSD_GFX_SetRect(&dstRect, x1, y1 + 2, img.width, img.height);
			}
			else
			{
				OSD_GFX_SetRect(&dstRect, x1, y1, img.width, img.height);
			}

			OSD_GFX_FB_Blend((unsigned long)img.hBuffer, img.width, img.height, pOsdScreen, &dstRect);
		}
		txtCol = COL(C_T_Enable);
		fontSize = FONT_SIZE_M;
	}
	else
	{	/* normal, encrypted */
		x1 += 3;
		w -= 6;
		y1 += 3;
		h -= 6;

		if (!AP_StockImg_GetImage(e123_1_00_BTN_D1_N, &img))
		{
			OSD_GFX_SetRect(&dstRect, x1, y1, img.width, img.height);
			OSD_GFX_FB_Blend((unsigned long)img.hBuffer, img.width, img.height, pOsdScreen, &dstRect);
		}
		txtCol = COLOR_WHITE;
		fontSize = FONT_SIZE_S;
	}

	pFont = OSD_FONT_Get(eFont_SystemNormal);
	if (!OSD_FONT_SetSize(pFont, fontSize))
	{
		y = y1 + ((h - pFont->height)>>1);//+ (h /2 - pFont->height / 2);		/* mid & center aligned */

		str = GWM_Button_GetTitlePtr(btn);
		if (str != NULL)
		{
			length = OSD_GFX_GetStringWidth(pFont, str);
			x =  x1 + ((w - length)>>1);
			OSD_GFX_DrawString(pOsdScreen, pFont, x, y, str, txtCol);
		}
	}


	OSD_FONT_Release (pFont);

	return ERR_OK;
}

STATIC void xmgr_cas_NaPpvInitObj(XmgrCas_NaPpvBPopContent_t *pContents)
{
	OSD_Rect_t		 	stRect;

	/* Set Area. */
	xmgr_cas_NaPpvGetBPOPUiPosition(PPV_BPOP_MAIN_FRAME_ID,  &stRect);
	GWM_APP_SetAppArea (stRect.x, stRect.y, stRect.w, stRect.h);

	/* frame. */
	xmgr_cas_NaPpvGetBPOPUiPosition(PPV_BPOP_FRAME_ID, &stRect);
	GWM_Frame_Create (PPV_BPOP_FRAME_ID, &stRect, (HUINT8 *)GetStrRsc(LOC_CAS_NA_RCS_PAYPERVIEWCHANNEL));
	GWM_APP_SetObjectDrawMethod (PPV_BPOP_FRAME_ID, AP_LNF_WindowTD370Frame_Draw);

	/* box. */
	xmgr_cas_NaPpvGetBPOPUiPosition(PPV_BPOP_DIALOG_INNER_B_ID,  &stRect);
	GWM_Rect_Create(PPV_BPOP_DIALOG_INNER_B_ID, &stRect, 0);
	GWM_APP_SetObjectDrawMethod (PPV_BPOP_DIALOG_INNER_B_ID, xmgr_cas_NaPpvLNF_DrawBox);

	/* text */
	xmgr_cas_NaPpvGetBPOPUiPosition(PPV_BPOP_TEXT_ID,  &stRect);
	GWM_Rect_Create(PPV_BPOP_TEXT_ID, &stRect, 0);
	GWM_APP_SetObjectDrawMethod (PPV_BPOP_TEXT_ID, xmgr_cas_NaPpvdrawText);

	/* Button 1 */
	xmgr_cas_NaPpvGetBPOPUiPosition(PPV_BPOP_BTN_1_BASE_ID,  &stRect);
	GWM_Button_Create (PPV_BPOP_BTN_1_BASE_ID, &stRect,  (HUINT8 *)GetStrRsc(LOC_DIALOG_OK_BUTTON_TEXT_ID));
	GWM_APP_SetObjectDrawMethod (PPV_BPOP_BTN_1_BASE_ID, xmgr_cas_NaPpvLNF_DialogDisableButton_Draw);

	/* Button 2 */
	xmgr_cas_NaPpvGetBPOPUiPosition(PPV_BPOP_BTN_2_BASE_ID,  &stRect);
	GWM_Button_Create (PPV_BPOP_BTN_2_BASE_ID, &stRect, (HUINT8 *)GetStrRsc(LOC_DIALOG_CANCEL_BUTTON_TEXT_ID));
	GWM_APP_SetObjectDrawMethod (PPV_BPOP_BTN_2_BASE_ID, xmgr_cas_NaPpvLNF_DialogDisableButton_Draw);

	/* Button 3 */
	xmgr_cas_NaPpvGetBPOPUiPosition(PPV_BPOP_BTN_3_BASE_ID,  &stRect);
	GWM_Button_Create (PPV_BPOP_BTN_3_BASE_ID, &stRect, (HUINT8 *)GetStrRsc(LOC_CAS_NA_RCS_INFO));
	GWM_APP_SetObjectDrawMethod (PPV_BPOP_BTN_3_BASE_ID, xmgr_cas_NaPpvLNF_DialogDisableButton_Draw);

	// 5분 이상 남은 이벤트만 구입 가능
	if(pContents->bDisableOK)
	{
		GWM_Obj_DisableObject(PPV_BPOP_BTN_1_BASE_ID );
		/* Set focus */
		GWM_Obj_SetFocus (PPV_BPOP_BTN_2_BASE_ID);
	}
	else
	{
		/* Set focus */
		GWM_Obj_SetFocus (PPV_BPOP_BTN_1_BASE_ID);
	}
}

STATIC BUS_Result_t xmgr_cas_NaPpvMsgGwmDestroy(XmgrCas_NaPpvBPopContent_t *pContents)
{
	HxSTD_memset(pContents, 0, sizeof(XmgrCas_NaPpvBPopContent_t));

	// remove detail info
	if(BUS_MGR_Get(xproc_cas_NaPpvDetailInfo_Rcs) != NULL)
	{
		BUS_MGR_Destroy(xproc_cas_NaPpvDetailInfo_Rcs);
	}

	return ERR_BUS_NO_ERROR;
}


STATIC BUS_Result_t xmgr_cas_NaPpvMsgGwmClicked(XmgrCas_NaPpvBPopContent_t *pContents, HINT32 p1)
{
	HBOOL		bNeedDestroy = FALSE;
	HBOOL		bNeedCreditWnd = FALSE;
	TCaProcessingStatus xProStatus = CA_PROCESSING_ERROR;
	HERROR		hErr = ERR_FAIL;
	STATIC RSV_Item_t rsvItem;

	switch (p1)
	{
		case PPV_BPOP_BTN_1_BASE_ID :
			// buy
			hErr = xmgr_cas_NaPpvRequestPurchaseEventByEpg(CA_EVENT_SPECIAL_EVENT, pContents->pRequest, pContents->pProduct, &xProStatus);
			if(hErr != ERR_OK  )
			{
				// failure
				HxLOG_Print("[%s] failure to purchase [%d]!!\n", __FUNCTION__, xProStatus);
			}
			else
			{
				// success
				HxLOG_Print("[%s] success to purchase!!\n", __FUNCTION__);
				xProStatus = CA_PROCESSING_NO_ERROR;
			}

			bNeedDestroy = TRUE;
			bNeedCreditWnd = TRUE;
			break;

		case PPV_BPOP_BTN_2_BASE_ID :
			// cancel
			bNeedDestroy = TRUE;
			break;

		case PPV_BPOP_BTN_3_BASE_ID :
			BUS_MGR_Create ("xproc_cas_NaPpvDetailInfo_Rcs",
								APP_DIALOGBOX_PRIORITY,
								xproc_cas_NaPpvDetailInfo_Rcs,
								NULL,
								(HINT32)&pContents->ApEvt,
								0,
								0);
			break;

		default :
			HxLOG_Critical("\n\n");
			break;
	}

	// send ppv dlg clicked
	if(bNeedDestroy == TRUE)
	{
		HxSTD_memset(&rsvItem, 0x00, sizeof(RSV_Item_t));
		rsvItem.ulSlot = DxRSV_SLOT_NULL;
		rsvItem.hSvc = pContents->ApEvt.hSvcHandle;
		rsvItem.utStTime = pContents->ApEvt.stEventInfo.ulStartTime;
		rsvItem.utDuration = pContents->ApEvt.stEventInfo.ulDuration;

		// destroy window
		BUS_MGR_Destroy(NULL);

		// show PPV credit window
		if(bNeedCreditWnd == TRUE)
		{
			BUS_MGR_Create( "xmgr_cas_NaRCS_PPVCreditPopup_Proc",
								APP_DIALOGBOX_PRIORITY,
								xmgr_cas_NaRCS_PPVCreditPopup_Proc,
								HANDLE_NULL,
								(HINT32)xProStatus,
								(HUINT32)&rsvItem,
								0);
		}
	}




	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_cas_NaPpvMsgGwmKeyDown(XmgrCas_NaPpvBPopContent_t *pContents, HINT32 p1)
{
	HERROR 		hErr;
	HINT32 		nObjID = GWM_GetFocusedObjectID();

	AP_KeyConv_UiCommon(&p1);

	switch (p1)
	{
		case KEY_INFO :
		case KEY_GUIDE :
		case KEY_MENU :
		case KEY_OPT :
		case KEY_MEDIA :
			hErr = BUS_MGR_Destroy (0);
			if ( hErr != ERR_OK )
			{
				HxLOG_Print("[xmgr_cas_NaPpvMsgGwmKeyDown] BUS_MGR_Destroy fail!!\n");
				break;
			}
			return MESSAGE_PASS;

		case KEY_BACK :
		case KEY_EXIT :
			hErr = BUS_MGR_Destroy (0);
			if ( hErr != ERR_OK )
			{
				HxLOG_Print("[xmgr_cas_NaPpvMsgGwmKeyDown] BUS_MGR_Destroy fail!!\n");
				break;
			}
			return MESSAGE_BREAK;

		case KEY_ARROWLEFT :
			// ok 가 disable 인 경우
			if(pContents->bDisableOK == TRUE)
			{
				if(nObjID == PPV_BPOP_BTN_3_BASE_ID)
				{
					GWM_Obj_SetFocus(nObjID-1);
				}
				else
				{
					GWM_Obj_SetFocus(PPV_BPOP_BTN_3_BASE_ID);
				}
				return MESSAGE_BREAK;
			}

			if(nObjID == PPV_BPOP_BTN_1_BASE_ID)
			{
				GWM_Obj_SetFocus(PPV_BPOP_BTN_3_BASE_ID);
			}
			else
			{
				GWM_Obj_SetFocus(nObjID-1);
			}
			return MESSAGE_BREAK;
		case KEY_ARROWRIGHT :
			// ok 가 disable 인 경우
			if(pContents->bDisableOK == TRUE)
			{
				if(nObjID == PPV_BPOP_BTN_2_BASE_ID)
				{
					GWM_Obj_SetFocus(nObjID+1);
				}
				else
				{
					GWM_Obj_SetFocus(PPV_BPOP_BTN_2_BASE_ID);
				}
				return MESSAGE_BREAK;
			}

			if(nObjID == PPV_BPOP_BTN_3_BASE_ID)
			{
				GWM_Obj_SetFocus(PPV_BPOP_BTN_1_BASE_ID);
			}
			else
			{
				GWM_Obj_SetFocus(nObjID+1);
			}
			return MESSAGE_BREAK;
		case KEY_PLAY :
		case KEY_PAUSE :
		case KEY_STOP :
		case KEY_RECORD :
		case KEY_FORWARD :
		case KEY_BACKWARD :
		case KEY_PREV :
		case KEY_NEXT :
		case KEY_REPLAY :
		case KEY_SKIP :
		case KEY_ADD_BOOKMARK :
		case KEY_BOOKMARK_LIST :
		case KEY_SLOW :
			return MESSAGE_BREAK;

		default :
			break;
	}

	return ERR_BUS_NO_ERROR;
}


STATIC BUS_Result_t xmgr_cas_NaPpvMsgGwmCreate(XmgrCas_NaPpvBPopContent_t *pContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				hError = ERR_FAIL;
	HUINT32				ulSysTime = 0;
	PPV_EVENT_INFO		*pstPPVEvtInfo = NULL;
	TCaRequest			*pRequest = NULL;
	TCaProduct			*pProduct = NULL;

	// 1. init
	if(s_BPPOPIsFirst == TRUE)
	{
		HxSTD_memset(pContents, 0, sizeof(XmgrCas_NaPpvBPopContent_t));
		s_BPPOPIsFirst = FALSE;
	}

	if(p1 == NULL && p2 == NULL)
	{
		HxLOG_Print("[ERROR] [%s] ERROR not found events  !\n", __FUNCTION__);
		goto jumpBuyingPop;
	}

	// 2. copy data
	if(p1 != NULL)
	{
		// EPG_APEVENT_t 로 받은 경우
		HxSTD_memcpy(&pContents->ApEvt, p1, sizeof(EPG_APEVENT_t));
	}
	else if (p2 != NULL)
	{
		// SvcEpg_Event_t 로 받은 경우
		AP_EPGMAIN_ConvertEventMW2AP((SvcEpg_Event_t*)&p2, &pContents->ApEvt);
		pContents->ApEvt.hSvcHandle = (Handle_t)p3;
		pContents->ApEvt.ulEventState = MODE_EPGAPP_EVENT_VALID;
	}

	// 3. 5분 이상 남은 이벤트만 구입 가능
	hError = VK_CLOCK_GetTime(&ulSysTime);
	if(hError == VK_OK
		&& (HUINT32)pContents->ApEvt.stEventInfo.ulStartTime < (HUINT32)(ulSysTime + (5*SECONDS_PER_MIN)))
	{
		HxLOG_Print("[%s] time + 5min is over  !\n", __FUNCTION__);
		pContents->bDisableOK = TRUE;
	}
	else
	{
		pContents->bDisableOK = FALSE;
	}

	// 4. load price
	hError = xmgr_cas_NaPpvRequestEventInfoByEpg(pContents->ApEvt.hSvcHandle, pContents->ApEvt.stEventInfo.stEventInfo.stEitEventInfo.nEventId, &pRequest, &pstPPVEvtInfo, &pProduct);
	if(hError == ERR_FAIL || pstPPVEvtInfo == NULL || pRequest == NULL || pProduct == NULL)
	{
		HxLOG_Print("[ERROR] [%s] ERROR not found PPV Event info  !\n", __FUNCTION__);
		goto jumpBuyingPop;
	}
	pContents->pstPPVEvtInfo = pstPPVEvtInfo;
	pContents->	pRequest = pRequest;
	pContents->	pProduct	= pProduct;
	pContents->Price = pstPPVEvtInfo->xCost;

	// 5. load card credit
	hError = xmgr_cas_NaPpvGetCreditSimple(&pContents->Credit, &pContents->CreditStatus);
	if(hError != ERR_OK)
	{
		pContents->Credit = 0;
	}

	// 6. init OBJ
	xmgr_cas_NaPpvInitObj(pContents);

	return ERR_BUS_NO_ERROR;


// data를 제대로 못 얻는경우 종료 해버린다.
jumpBuyingPop:
	BUS_MGR_Destroy(0);

	return ERR_BUS_NO_ERROR;

}


BUS_Result_t xmgr_cas_NaIppvBuyingPopup_Rcs (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t		eResult = ERR_BUS_NO_ERROR;
	XmgrCas_NaPpvBPopContent_t	*pContents = NULL;
	HERROR			hErr = ERR_FAIL;
	HBOOL			bPurchased = FALSE;

	pContents = xmgr_cas_NaPpvGetBPOPContents();

	switch (message)
	{
		/**********************************************************************f
			GWM MESSAGES
		**********************************************************************/

		case MSG_GWM_CREATE :
			eResult = xmgr_cas_NaPpvMsgGwmCreate(pContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYDOWN :
			eResult = xmgr_cas_NaPpvMsgGwmKeyDown(pContents, p1);
			break;

		case MSG_GWM_TIMER :
			break;

		case MSG_GWM_CLICKED :
			eResult = xmgr_cas_NaPpvMsgGwmClicked(pContents, p1);
			break;

		case MSG_GWM_DESTROY :
			eResult = xmgr_cas_NaPpvMsgGwmDestroy(pContents);
			break;

		/**********************************************************************
			APP MESSAGES
		**********************************************************************/
		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			// 채널 체인지 돼면 close
			hErr = BUS_MGR_Destroy (0);
			if ( hErr != ERR_OK )
			{
				HxLOG_Print("[xmgr_cas_NaIppvBuyingPopup_Rcs] BUS_MGR_Destroy fail!!\n");
				break;
			}
			break;

		case eMEVT_XMGR_CAS_NA_NOTIFY_RCS_PURCHASE_EVENT:
			// 이미 구매한 이벤트라면 종료
			hErr = xmgr_cas_NaPpvIsPurchasedEvent((void*)&pContents->ApEvt.stEventInfo, pContents->ApEvt.aucShortEventName, &bPurchased);
			if(hErr != ERR_OK)
			{
				bPurchased = FALSE;
			}

			if(bPurchased == TRUE)
			{
				hErr = BUS_MGR_Destroy (0);
				if ( hErr != ERR_OK )
				{
					HxLOG_Print("[xmgr_cas_NaIppvBuyingPopup_Rcs] BUS_MGR_Destroy fail!!\n");
					break;
				}
			}
			break;

		default :
			break;
	}

	if ( eResult != ERR_BUS_NO_ERROR )
	{
		return eResult;
	}

	return BUS_ProcessMessageDefault (message, hAction, p1, p2, p3);
}

/*********************** End of File ******************************/

