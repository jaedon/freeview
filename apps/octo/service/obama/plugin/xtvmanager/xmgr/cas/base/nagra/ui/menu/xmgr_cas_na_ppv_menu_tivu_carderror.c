//******************************************************************************
//	INCLUDE
//******************************************************************************
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

#include <menuui_coor.h>
#include <settings.h>

#include <ap_util_osd.h>

#include <namgr_main.h>

#include "../../local_include/_xmgr_cas_na_util.h"
#include "../../local_include/_xmgr_cas_na_adapt.h"
#include "../../local_include/_xmgr_cas_na.h"

#include "ap_menu_contents.h"

#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
//******************************************************************************
//	DEFINE
//******************************************************************************




typedef struct tag_CAS_NA_TIVU_MENU_SCE_Contents
{
	HUINT8		bDisplayMode;
	HINT32		cardStatus;
	HUINT8		szSmartCardErrorDateStr[32];
	HUINT8		*pszSmartCardErrorMsgText;
	HUINT8		*pszSmartCardErrorMsgDetailText;
} XmgrCas_NaMenuCardError_Tivu_t;

typedef enum
{
	eNaPpvTivuItemIdx_ScInfo,
	eNaPpvTivuItemIdx_MAX,
} XmgrCas_NaPpvMenuItem_Tivu_e;

#define NAPPV_TIVU_MENU_SCERROR_WINDOW_WIDTH										OSD_SCREEN_WIDTH
#define NAPPV_TIVU_MENU_SCERROR_WINDOW_HEIGHT										OSD_SCREEN_HEIGHT
#define NAPPV_TIVU_MENU_SCERROR_WINDOW_X											(OSD_SCREEN_WIDTH/2-NAPPV_TIVU_MENU_SCERROR_WINDOW_WIDTH/2)
#define NAPPV_TIVU_MENU_SCERROR_WINDOW_Y											(OSD_SCREEN_HEIGHT/2-NAPPV_TIVU_MENU_SCERROR_WINDOW_HEIGHT/2)

#define	NAPPV_TIVU_MENU_SCERROR_APP_ID												0

/* ppv tivu scerror msg background*/
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_ID									(NAPPV_TIVU_MENU_SCERROR_APP_ID|0x0010)
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_X									0
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_Y									0
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_WIDTH								OSD_SCREEN_WIDTH
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_HEIGHT								OSD_SCREEN_HEIGHT

#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_ID						(NAPPV_TIVU_MENU_SCERROR_APP_ID|0x0020)
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_X						432
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_Y						316
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_WIDTH					848
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_HEIGHT					8

#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_ID					(NAPPV_TIVU_MENU_SCERROR_APP_ID|0x0030)
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_X					432
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_Y					491
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_WIDTH				848
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_HEIGHT				8

/* ppv tivu scerror date */
#define	NAPPV_TIVU_MENU_SCERROR_MSG_DATE_TITLE_TEXT_ID								(NAPPV_TIVU_MENU_SCERROR_APP_ID|0x0040)
#define	NAPPV_TIVU_MENU_SCERROR_MSG_DATE_TITLE_TEXT_X								944
#define	NAPPV_TIVU_MENU_SCERROR_MSG_DATE_TITLE_TEXT_Y								129
#define	NAPPV_TIVU_MENU_SCERROR_MSG_DATE_TITLE_TEXT_WIDTH							140
#define	NAPPV_TIVU_MENU_SCERROR_MSG_DATE_TITLE_TEXT_HEIGHT							24

#define	NAPPV_TIVU_MENU_SCERROR_MSG_DATE_LINE_ID									(NAPPV_TIVU_MENU_SCERROR_APP_ID|0x0050)
#define	NAPPV_TIVU_MENU_SCERROR_MSG_DATE_LINE_X										943
#define	NAPPV_TIVU_MENU_SCERROR_MSG_DATE_LINE_Y										134
#define	NAPPV_TIVU_MENU_SCERROR_MSG_DATE_LINE_WIDTH									337
#define	NAPPV_TIVU_MENU_SCERROR_MSG_DATE_LINE_HEIGHT								9

#define	NAPPV_TIVU_MENU_SCERROR_MSG_PRESENT_DATE_TEXT_ID							(NAPPV_TIVU_MENU_SCERROR_APP_ID|0x0060)
#define	NAPPV_TIVU_MENU_SCERROR_MSG_PRESENT_DATE_TEXT_X								945
#define	NAPPV_TIVU_MENU_SCERROR_MSG_PRESENT_DATE_TEXT_Y								167
#define	NAPPV_TIVU_MENU_SCERROR_MSG_PRESENT_DATE_TEXT_WIDTH							140
#define	NAPPV_TIVU_MENU_SCERROR_MSG_PRESENT_DATE_TEXT_HEIGHT						25

/* ppv tivu scerror warning */
#define	NAPPV_TIVU_MENU_SCERROR_MSG_WARNING_TEXT_ID									(NAPPV_TIVU_MENU_SCERROR_APP_ID|0x0070)
#define	NAPPV_TIVU_MENU_SCERROR_MSG_WARNING_TEXT_X									468
#define	NAPPV_TIVU_MENU_SCERROR_MSG_WARNING_TEXT_Y									308
#define	NAPPV_TIVU_MENU_SCERROR_MSG_WARNING_TEXT_WIDTH								110
#define	NAPPV_TIVU_MENU_SCERROR_MSG_WARNING_TEXT_HEIGHT								24

/* ppv tivu ok button image */
#define	NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_ID									(NAPPV_TIVU_MENU_SCERROR_APP_ID|0x0080)
#define	NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_X										738
#define	NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_Y										595
#define	NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_WIDTH									192
#define	NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_HEIGHT								31

#define	NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_ID										(NAPPV_TIVU_MENU_SCERROR_APP_ID|0x0090)
#define	NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_X										750
#define	NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_Y										648
#define	NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_WIDTH									180
#define	NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_HEIGHT									24

/* ppv tivu back button image */
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACK_BUTTON_ID									(NAPPV_TIVU_MENU_SCERROR_APP_ID|0x00a0)
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACK_BUTTON_X									943
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACK_BUTTON_Y									595
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACK_BUTTON_WIDTH								337
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACK_BUTTON_HEIGHT								31

#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACK_TEXT_ID									(NAPPV_TIVU_MENU_SCERROR_APP_ID|0x00b0)
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACK_TEXT_X										955
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACK_TEXT_Y										648
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACK_TEXT_WIDTH									200
#define	NAPPV_TIVU_MENU_SCERROR_MSG_BACK_TEXT_HEIGHT								24

/* ppv tivu scerror tivusat icon */
#define	NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID									(NAPPV_TIVU_MENU_SCERROR_APP_ID|0x00c0)

/* ppv tivu scerror msg */
#define	NAPPV_TIVU_MENU_SCERROR_MSG_TEXT_ID											(NAPPV_TIVU_MENU_SCERROR_APP_ID|0x00d0)
#define	NAPPV_TIVU_MENU_SCERROR_MSG_TEXT_X											459
#define	NAPPV_TIVU_MENU_SCERROR_MSG_TEXT_Y											389
#define	NAPPV_TIVU_MENU_SCERROR_MSG_TEXT_WIDTH										700
#define	NAPPV_TIVU_MENU_SCERROR_MSG_TEXT_HEIGHT										42

#define	NAPPV_TIVU_MENU_SCERROR_MSG_DETAIL_TEXT_ID									(NAPPV_TIVU_MENU_SCERROR_APP_ID|0x00e0)
#define	NAPPV_TIVU_MENU_SCERROR_MSG_DETAIL_TEXT_X									437
#define	NAPPV_TIVU_MENU_SCERROR_MSG_DETAIL_TEXT_Y									531
#define	NAPPV_TIVU_MENU_SCERROR_MSG_DETAIL_TEXT_WIDTH								700
#define	NAPPV_TIVU_MENU_SCERROR_MSG_DETAIL_TEXT_HEIGHT								29

//#define TEST_TIVU_UI_MENU
#ifdef TEST_TIVU_UI_MENU
STATIC HINT32 gTestMenuCardStatus = 0x0;
#endif

//******************************************************************************
//	STATIC VARIABLES
//******************************************************************************
STATIC XmgrCas_NaMenuCardError_Tivu_t	s_stApNaPpvMenuCardErrorTivu;

//******************************************************************************
//	PROTOTYPE FUNCTIONS
//******************************************************************************
#define ________XMGR_CAS_NA_PpvTIVU_MENU_STATIC___________________________________________
STATIC inline XmgrCas_NaMenuCardError_Tivu_t * xmgr_cas_NaPpvMenuScErrorGetLocalContents_Tivu(void);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorContents_Tivu(XmgrCas_NaMenuCardError_Tivu_t *pstContents);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorGetStringWidth_Tivu(HUINT8* string, HUINT32 ulFontSize, HUINT32 *ulWidth);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawMsgBackgroundImage_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawMsgBackgroundImageShortLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawMsgTivuSatImageIcon_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawMsgPresentDateLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawMsgPresentDate_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawMsgOKButton_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawMsgBackButton_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawSmartCardMsgTitle_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawSmartCardMsgDetail_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawDafaultUi_Tivu(XmgrCas_NaMenuCardError_Tivu_t *pstContents);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDestroyUI_Tivu(XmgrCas_NaMenuCardError_Tivu_t *pstContents);

#define ________XMGR_CAS_NA_PpvTIVU_MENU_PUBLIC___________________________________________
BUS_Result_t xproc_cas_NaPpvMenuSmartCardError_Tivu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#define	_________________________STATIC_BODY______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScErrorGetLocalContents_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC inline XmgrCas_NaMenuCardError_Tivu_t * xmgr_cas_NaPpvMenuScErrorGetLocalContents_Tivu(void)
{
	return &s_stApNaPpvMenuCardErrorTivu;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScErrorContents_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorContents_Tivu(XmgrCas_NaMenuCardError_Tivu_t *pstContents)
{
	XmgrCas_NaAlamType_e 		eScAlarm;

	xmgr_cas_NaGetSmartCardStatus(&eScAlarm);
	HxLOG_Print("eScAlarm = %d\n", eScAlarm);

	if (eScAlarm == eCAS_NA_CA_SMARTCARD_OK)
	{
		if (!xmgr_cas_NaCheckTivuCardSystemId())
		{
			eScAlarm = eCAS_NA_CA_SMARTCARD_INVALID;
		}
	}

#ifdef TEST_TIVU_UI_MENU
	eScAlarm = gTestMenuCardStatus;
#endif

	memset(pstContents, 0, sizeof(XmgrCas_NaMenuCardError_Tivu_t));

	/*status error msg*/
	pstContents->bDisplayMode = 1;
	pstContents->cardStatus = eScAlarm;

	/*Title , Detail error msg*/
	if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_OK)
	{
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_INFORMATION_1_TEXT_ID);
		pstContents->pszSmartCardErrorMsgDetailText = NULL;
	}
	else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_REMOVED)
	{
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_INSERT_1_TEXT_ID);
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_SCRAMBLED_DETAIL_TEXT_ID);
	}
	else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_SUSPENDED)
	{
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_BLOCKED_TEXT_ID);
		pstContents->pszSmartCardErrorMsgDetailText = NULL;
	}
	else if (pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_BLACKLISTED)
	{
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_BLACKLIST_1_TEXT_ID);
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_BLACKLIST_DETAIL_TEXT_ID);
	}
	else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_INVALID
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_COM_ERROR
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_MUTE
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NEVER_PAIRED
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
	{
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_UNREADABLE_1_TEXT_ID);
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_UNREADABLE_DETAIL_TEXT_ID);
	}
	else		// expired 등 예외 상황
	{
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_UNREADABLE_1_TEXT_ID);
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_UNREADABLE_DETAIL_TEXT_ID);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScErrorGetStringWidth_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorGetStringWidth_Tivu(HUINT8* string, HUINT32 ulFontSize, HUINT32 *ulWidth)
{
	OSD_StringInfo_t		stringInfo = {0, };
	OSD_CheckFontInfo_t 	fontInfo;

	stringInfo.eEncoding = MWC_UTIL_GetEncodingByLocaleCode(string);
	stringInfo.strData = (HINT8*)string;
	stringInfo.strLength = SvcCas_UtilStrLen((const HUINT8 *)stringInfo.strData);

	fontInfo.fontId = eFont_SystemNormal;
	fontInfo.fontSize = (HINT32)ulFontSize;
	fontInfo.fontWidthRatio = 100;	// Normal

	*ulWidth = OSD_FONT_GetStringWidth(&fontInfo, &stringInfo);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScErrorDrawMsgBackgroundImage_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawMsgBackgroundImage_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 nX1, nY1;
	OSD_Rect_t			 stRect;
	OSD_StockImage_t	 stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e931_1_41_CAS_BG02, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScErrorDrawMsgBackgroundImageShortLine_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawMsgBackgroundImageShortLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 nX1, nY1;
	OSD_Rect_t			 stRect;
	OSD_StockImage_t	 stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e931_1_41_CAS_Line02, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScErrorDrawMsgTivuSatImageIcon_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawMsgTivuSatImageIcon_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 nX1, nY1;
	OSD_Rect_t			 stRect;
	OSD_StockImage_t	 stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e931_1_41_CAS_Boxlogo, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScErrorDrawMsgPresentDateLine_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawMsgPresentDateLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 nX1, nY1;
	OSD_Rect_t			 stRect;
	OSD_StockImage_t	 stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e931_1_41_CAS_Data, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScErrorDrawMsgPresentDate_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawMsgPresentDate_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	OSD_Rect_t				rect_msg;

	UNIXTIME				ulCurTime= 0;
	BUS_Result_t			hRes;
	HxDATETIME_t 			stCurDateTime;

	XmgrCas_NaMenuCardError_Tivu_t	*pstContents = xmgr_cas_NaPpvMenuScErrorGetLocalContents_Tivu();

	/* for compiler */
	(void)pArg;
	(void)param1;

	memset(pstContents->szSmartCardErrorDateStr, 0, 32);
	rect_msg.x = pArea->x1 + pRect->x;
	rect_msg.y = pArea->x1 + pRect->y;
	rect_msg.w = pRect->w;
	rect_msg.h = pRect->h;

	hRes = VK_CLOCK_GetTime((unsigned long *)&ulCurTime);
	if(hRes != VK_OK || UNIX_FIRST_DAY >= ulCurTime)
	{
		ulCurTime = UNIX_FIRST_DAY;
	}

	hError =HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurTime, &stCurDateTime);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[xmgr_cas_NaPpvMenuScErrorDrawMsgPresentDate_Tivu] ConvertUnixTimeToDateTime fail!!\n");
		return ERR_FAIL;
	}

	sprintf(pstContents->szSmartCardErrorDateStr, "%02d.%02d.%02d\n", stCurDateTime.stDate.ucDay, stCurDateTime.stDate.ucMonth, stCurDateTime.stDate.usYear);
	OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XS, rect_msg.x, rect_msg.y, rect_msg.w,
								pstContents->szSmartCardErrorDateStr, COL(C_T_Cursor));

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScErrorDrawMsgOKButton_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawMsgOKButton_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 nX1, nY1;
	OSD_Rect_t			 stRect;
	OSD_StockImage_t	 stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e931_1_41_CAS_OK, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScErrorDrawMsgBackButton_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawMsgBackButton_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 nX1, nY1;
	OSD_Rect_t			 stRect;
	OSD_StockImage_t	 stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e931_1_41_CAS_Back, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScErrorDrawSmartCardMsgTitle_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawSmartCardMsgTitle_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hErr;
	OSD_Rect_t				rect_msg;
	OSD_Rect_t				rect_image;
	HUINT32					prevStringWidth = 0;
	HxLANG_Id_e			eMenuLang = eLangID_English;

	XmgrCas_NaMenuCardError_Tivu_t	*pstContents = xmgr_cas_NaPpvMenuScErrorGetLocalContents_Tivu();

	/* for compiler */
	(void)pArg;
	(void)param1;

	rect_msg.x = pArea->x1 + pRect->x;
	rect_msg.y = pArea->x1 + pRect->y;
	rect_msg.w = pRect->w;
	rect_msg.h = pRect->h;

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID);
	}

	if (MWC_UTIL_DvbStrlenStrOnly(pstContents->pszSmartCardErrorMsgText) > 0)
	{
		if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_OK)
		{
//			eMenuLang = MWC_PARAM_GetMainLanguage();
			hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32 *)&eMenuLang, 0);
			if (ERR_OK != hErr)
			{
				HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN) Error!!\n");
			}
			if((eMenuLang == eLangID_Italian) ||(eMenuLang == eLangID_Czech) || (eMenuLang == eLangID_Spanish))
			{
				/* 1.Draw Text */
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_INFORMATION_1_TEXT_ID), COL(C_T_Cursor));

				/* 2.Draw Text */
				rect_msg.y += (8 + FONT_SIZE_XXL);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_INFORMATION_2_TEXT_ID), COL(C_T_Cursor));
			}
			else //eng
			{
				/* 1.Draw Text */
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_INFORMATION_2_TEXT_ID), COL(C_T_Cursor));

				/* 2.Draw Text */
				rect_msg.y += (8 + FONT_SIZE_XXL);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_INFORMATION_1_TEXT_ID), COL(C_T_Cursor));
			}
		}
		else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_REMOVED)
		{
			xmgr_cas_NaPpvMenuScErrorGetStringWidth_Tivu((HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_INSERT_1_TEXT_ID), FONT_SIZE_XXL, &prevStringWidth);

//			eMenuLang = MWC_PARAM_GetMainLanguage();
			hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32 *)&eMenuLang, 0);
			if (ERR_OK != hErr)
			{
				HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN) Error!!\n");
			}
			if((eMenuLang == eLangID_Italian) ||(eMenuLang == eLangID_Czech) || (eMenuLang == eLangID_Spanish))
			{
				/* 1.Draw Text */
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_INSERT_1_TEXT_ID), COL(C_T_Cursor));

				/* 2.Draw Text */
				rect_msg.y += (8 + FONT_SIZE_XXL);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_INSERT_2_TEXT_ID), COL(C_T_Cursor));

				/* 3.Draw Tivu Icon*/
				xmgr_cas_NaPpvMenuScErrorGetStringWidth_Tivu((HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_INSERT_2_TEXT_ID), FONT_SIZE_XXL, &prevStringWidth);
				rect_msg.x += (8 + prevStringWidth);
				rect_image.x = rect_msg.x;
				rect_image.y = rect_msg.y - (FONT_SIZE_XXL+6);
				rect_image.w = rect_msg.w;
				rect_image.h = rect_msg.h;
				GWM_Frame_Create (NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID, &rect_image, NULL);
				GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawMsgTivuSatImageIcon_Tivu);
			}
			else if(eMenuLang == eLangID_German)
			{
				/* 1.Draw Text */
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_INSERT_2_TEXT_ID), COL(C_T_Cursor));

				/* 2.Draw Tivu Icon*/
				rect_msg.y += (8 + FONT_SIZE_XXL);
				rect_image.x = rect_msg.x;
				rect_image.y = rect_msg.y - (FONT_SIZE_XXL+6);
				rect_image.w = rect_msg.w;
				rect_image.h = rect_msg.h;
				GWM_Frame_Create (NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID, &rect_image, NULL);
				GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawMsgTivuSatImageIcon_Tivu);

				/* 3.Draw Text */
				rect_msg.x += (8 + 197);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_INSERT_1_TEXT_ID), COL(C_T_Cursor));
			}
			else //eng
			{
				/* 1.Draw Text */
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_INSERT_1_TEXT_ID), COL(C_T_Cursor));

				/* 2.Draw Tivu Icon*/
				rect_msg.y += (8 + FONT_SIZE_XXL);
				rect_image.x = rect_msg.x;
				rect_image.y = rect_msg.y - (FONT_SIZE_XXL+6);
				rect_image.w = rect_msg.w;
				rect_image.h = rect_msg.h;
				GWM_Frame_Create (NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID, &rect_image, NULL);
				GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawMsgTivuSatImageIcon_Tivu);

				/* 3.Draw Text */
				rect_msg.x += (8 + 197);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_INSERT_2_TEXT_ID), COL(C_T_Cursor));
			}
		}
		else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_BLACKLISTED)
		{
			xmgr_cas_NaPpvMenuScErrorGetStringWidth_Tivu((HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_BLACKLIST_1_TEXT_ID), FONT_SIZE_XXL, &prevStringWidth);

//			eMenuLang = MWC_PARAM_GetMainLanguage();
			hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32 *)&eMenuLang, 0);
			if (ERR_OK != hErr)
			{
				HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN) Error!!\n");
			}
			if((eMenuLang == eLangID_Italian) ||(eMenuLang == eLangID_Czech) || (eMenuLang == eLangID_Spanish) || (eMenuLang == eLangID_French))
			{
				/* 1.Draw Text */
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_BLACKLIST_1_TEXT_ID), COL(C_T_Cursor));

				/* 2.Draw Tivu Icon*/
				rect_msg.x += (8 + prevStringWidth);
				rect_image.x = rect_msg.x;
				rect_image.y = rect_msg.y - (FONT_SIZE_XXL+6);
				rect_image.w = rect_msg.w;
				rect_image.h = rect_msg.h;
				GWM_Frame_Create (NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID, &rect_image, NULL);
				GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawMsgTivuSatImageIcon_Tivu);

				/* 3.Draw Text */
				rect_msg.x -= (8 + prevStringWidth);
				rect_msg.y += (8 + FONT_SIZE_XXL);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_BLACKLIST_2_TEXT_ID), COL(C_T_Cursor));

			}
			else if(eMenuLang == eLangID_Greek)
			{
				/* 1.Draw Text */
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_BLACKLIST_1_TEXT_ID), COL(C_T_Cursor));

				/* 2.Draw Text */
				rect_msg.x += (8 + prevStringWidth);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_BLACKLIST_2_TEXT_ID), COL(C_T_Cursor));

				/* 3.Draw Tivu Icon*/
				rect_msg.x -= (8 + prevStringWidth);
				rect_msg.y += (8 + FONT_SIZE_XXL);
				rect_image.x = rect_msg.x;
				rect_image.y = rect_msg.y - (FONT_SIZE_XXL+6);
				rect_image.w = rect_msg.w;
				rect_image.h = rect_msg.h;
				GWM_Frame_Create (NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID, &rect_image, NULL);
				GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawMsgTivuSatImageIcon_Tivu);

			}
			else //eng
			{
				/* 1.Draw Tivu Icon*/
				rect_image.x = rect_msg.x;
				rect_image.y = rect_msg.y - (FONT_SIZE_XXL+6);
				rect_image.w = rect_msg.w;
				rect_image.h = rect_msg.h;
				GWM_Frame_Create (NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID, &rect_image, NULL);
				GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawMsgTivuSatImageIcon_Tivu);

				/* 2.Draw Text */
				rect_msg.x += (8 + 197);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_BLACKLIST_1_TEXT_ID), COL(C_T_Cursor));

				/* 3.Draw Text */
				rect_msg.x -= (8 + 197);
				rect_msg.y += (8 + FONT_SIZE_XXL);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_BLACKLIST_2_TEXT_ID), COL(C_T_Cursor));
			}
		}
		else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_SUSPENDED)
		{
			/* 1.Draw Text */
			OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
										(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_BLOCKED_TEXT_ID), COL(C_T_Cursor));
		}
		else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_INVALID
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_COM_ERROR
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_MUTE
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NEVER_PAIRED
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
		{
			/* 1.Draw Text */
			OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
										(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_UNREADABLE_1_TEXT_ID), COL(C_T_Cursor));

			/* 2.Draw Text */
			rect_msg.y += (8 + FONT_SIZE_XXL);
			OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
										(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_UNREADABLE_2_TEXT_ID), COL(C_T_Cursor));
		}
		else		// expired 등 예외 상황
		{
			/* 1.Draw Text */
			OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
										(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_UNREADABLE_1_TEXT_ID), COL(C_T_Cursor));

			/* 2.Draw Text */
			rect_msg.y += (8 + FONT_SIZE_XXL);
			OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
										(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CARD_UNREADABLE_2_TEXT_ID), COL(C_T_Cursor));
		}
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScErrorDrawSmartCardMsgDetail_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawSmartCardMsgDetail_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	OSD_Rect_t				rect_msg;
	Text_Property_t			stTextProperty;

	XmgrCas_NaMenuCardError_Tivu_t	*pstContents = xmgr_cas_NaPpvMenuScErrorGetLocalContents_Tivu();

	/* for compiler */
	(void)pObject;
	(void)pArg;

	rect_msg.x = pArea->x1 + pRect->x;
	rect_msg.y = pArea->x1 + pRect->y;
	rect_msg.w = pRect->w;
	rect_msg.h = pRect->h;

	if (MWC_UTIL_DvbStrlenStrOnly(pstContents->pszSmartCardErrorMsgDetailText) > 0)
	{
#if 0
		if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_REMOVED
				|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_COM_ERROR
				|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_BLACKLISTED)
#endif
		{
			/* 1.Draw Text */
			stTextProperty.ulFontSize = FONT_SIZE_M;
			stTextProperty.ulFontAlign = TEXT_ALIGN_LEFT;
			stTextProperty.cFontColor = COL(C_T_Cursor);

			rect_msg.y -= FONT_SIZE_M;
			hError = AP_LNF_DrawLongStringWithWordwrap(pScreen, pArea, &rect_msg, pstContents->pszSmartCardErrorMsgDetailText,
											stTextProperty, rect_msg.w, param1); // 2 line..
			if (hError != ERR_OK)
			{
				HxLOG_Error(" AP_LNF_DrawLongStringWithWordwrap Error!\n");
				return ERR_FAIL;
			}
		}
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScErrorDrawDafaultUi_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDrawDafaultUi_Tivu(XmgrCas_NaMenuCardError_Tivu_t *pstContents)
{
	OSD_Rect_t				rect = {0, };

	/* for compiler */
	(void)pstContents;

	/* na ppv tivu scerror background */
	Settings_DrawBack(FALSE);

	/* na ppv tivu scerror card error background*/
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_X, NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_Y, NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_WIDTH, NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_HEIGHT);
	GWM_Frame_Create (NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawMsgBackgroundImage_Tivu);

	/* Draw Panel Line*/
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_X, NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_Y, NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_WIDTH, NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_HEIGHT);
	GWM_Frame_Create (NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawMsgBackgroundImageShortLine_Tivu);

	if(pstContents->pszSmartCardErrorMsgDetailText != NULL)
	{
		/* Draw Panel Line*/
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_X, NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_Y, NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_WIDTH, NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_HEIGHT);
		GWM_Frame_Create (NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawMsgBackgroundImageShortLine_Tivu);
	}
	else
	{
		if(GWM_Obj_GetObjectByID(NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_ID))
		{
			GWM_Obj_Destroy(NAPPV_TIVU_MENU_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_ID);
		}
	}

	/* na ppv tivu date text */
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCERROR_MSG_DATE_TITLE_TEXT_X, NAPPV_TIVU_MENU_SCERROR_MSG_DATE_TITLE_TEXT_Y - FONT_SIZE_XXS, NAPPV_TIVU_MENU_SCERROR_MSG_DATE_TITLE_TEXT_WIDTH, NAPPV_TIVU_MENU_SCERROR_MSG_DATE_TITLE_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCERROR_MSG_DATE_TITLE_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_DATE_TEXT_ID)  );
	GWM_Text_SetAlign (NAPPV_TIVU_MENU_SCERROR_MSG_DATE_TITLE_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_TIVU_MENU_SCERROR_MSG_DATE_TITLE_TEXT_ID, FONT_SIZE_XXS);
	GWM_Text_ChangeColor (NAPPV_TIVU_MENU_SCERROR_MSG_DATE_TITLE_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv tivu date line */
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCERROR_MSG_DATE_LINE_X, NAPPV_TIVU_MENU_SCERROR_MSG_DATE_LINE_Y, NAPPV_TIVU_MENU_SCERROR_MSG_DATE_LINE_WIDTH, NAPPV_TIVU_MENU_SCERROR_MSG_DATE_LINE_HEIGHT);
	GWM_Frame_Create (NAPPV_TIVU_MENU_SCERROR_MSG_DATE_LINE_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_DATE_LINE_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawMsgPresentDateLine_Tivu);

	/* na ppv tivu present date text */
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCERROR_MSG_PRESENT_DATE_TEXT_X, NAPPV_TIVU_MENU_SCERROR_MSG_PRESENT_DATE_TEXT_Y, NAPPV_TIVU_MENU_SCERROR_MSG_PRESENT_DATE_TEXT_WIDTH, NAPPV_TIVU_MENU_SCERROR_MSG_PRESENT_DATE_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCERROR_MSG_PRESENT_DATE_TEXT_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_PRESENT_DATE_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawMsgPresentDate_Tivu);

	/* na ppv tivu warning text */
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCERROR_MSG_WARNING_TEXT_X, NAPPV_TIVU_MENU_SCERROR_MSG_WARNING_TEXT_Y - FONT_SIZE_XXS, NAPPV_TIVU_MENU_SCERROR_MSG_WARNING_TEXT_WIDTH, NAPPV_TIVU_MENU_SCERROR_MSG_WARNING_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCERROR_MSG_WARNING_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_ATTENTION_TEXT_ID)  );
	GWM_Text_SetAlign (NAPPV_TIVU_MENU_SCERROR_MSG_WARNING_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_TIVU_MENU_SCERROR_MSG_WARNING_TEXT_ID, FONT_SIZE_XXS);
	GWM_Text_ChangeColor (NAPPV_TIVU_MENU_SCERROR_MSG_WARNING_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv tivu scerror card error text*/
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCERROR_MSG_TEXT_X, NAPPV_TIVU_MENU_SCERROR_MSG_TEXT_Y, NAPPV_TIVU_MENU_SCERROR_MSG_TEXT_WIDTH, NAPPV_TIVU_MENU_SCERROR_MSG_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCERROR_MSG_TEXT_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawSmartCardMsgTitle_Tivu);

	if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_OK)
	{
		/* na ppv tivu ok button*/
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_X, NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_Y, NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_WIDTH, NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_HEIGHT);
		GWM_Frame_Create (NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawMsgOKButton_Tivu);

		/* na ppv tivu confirm text*/
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_X, NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_Y- FONT_SIZE_XXS, NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_WIDTH, NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_HEIGHT);
		GWM_Text_Create (NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_CONFIRM_TEXT_ID));
		GWM_Text_SetAlign (NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_ID, FONT_SIZE_XXS);
		GWM_Text_ChangeColor (NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_ID, COL(C_T_Cursor), 0x00);
	}
	else
	{
		if(GWM_Obj_GetObjectByID(NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_ID))
		{
			GWM_Obj_Destroy(NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_ID);
		}

		if(GWM_Obj_GetObjectByID(NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_ID))
		{
			GWM_Obj_Destroy(NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_ID);
		}
	}

	if(pstContents->pszSmartCardErrorMsgDetailText != NULL)
	{
		/* na ppv tivu scerror card error detail text*/
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCERROR_MSG_DETAIL_TEXT_X, NAPPV_TIVU_MENU_SCERROR_MSG_DETAIL_TEXT_Y, NAPPV_TIVU_MENU_SCERROR_MSG_DETAIL_TEXT_WIDTH, NAPPV_TIVU_MENU_SCERROR_MSG_DETAIL_TEXT_HEIGHT);
		GWM_Text_Create (NAPPV_TIVU_MENU_SCERROR_MSG_DETAIL_TEXT_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_DETAIL_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawSmartCardMsgDetail_Tivu);
	}

	/* na ppv tivu back button*/
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCERROR_MSG_BACK_BUTTON_X, NAPPV_TIVU_MENU_SCERROR_MSG_BACK_BUTTON_Y, NAPPV_TIVU_MENU_SCERROR_MSG_BACK_BUTTON_WIDTH, NAPPV_TIVU_MENU_SCERROR_MSG_BACK_BUTTON_HEIGHT);
	GWM_Frame_Create (NAPPV_TIVU_MENU_SCERROR_MSG_BACK_BUTTON_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCERROR_MSG_BACK_BUTTON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScErrorDrawMsgBackButton_Tivu);

	/* na ppv tivu return menu text*/
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCERROR_MSG_BACK_TEXT_X, NAPPV_TIVU_MENU_SCERROR_MSG_BACK_TEXT_Y- FONT_SIZE_XXS, NAPPV_TIVU_MENU_SCERROR_MSG_BACK_TEXT_WIDTH, NAPPV_TIVU_MENU_SCERROR_MSG_BACK_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCERROR_MSG_BACK_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_ERR_RETURN_TO_MENU_TEXT_ID));
	GWM_Text_SetAlign (NAPPV_TIVU_MENU_SCERROR_MSG_BACK_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_TIVU_MENU_SCERROR_MSG_BACK_TEXT_ID, FONT_SIZE_XXS);
	GWM_Text_ChangeColor (NAPPV_TIVU_MENU_SCERROR_MSG_BACK_TEXT_ID, COL(C_T_Cursor), 0x00);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScErrorDestroyUI_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScErrorDestroyUI_Tivu(XmgrCas_NaMenuCardError_Tivu_t *pstContents)
{
	/* for compiler */
	(void)pstContents;

	return ERR_BUS_NO_ERROR;
}


#define	_________________________PUBLIC_BODY______________________________
/******************************************************************************/
/**
*   \fn     xproc_cas_NaPpvMenuSmartCardError_Tivu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaPpvMenuSmartCardError_Tivu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaMenuCardError_Tivu_t	*pstContents = xmgr_cas_NaPpvMenuScErrorGetLocalContents_Tivu();
	BUS_Result_t					ulResult = ERR_BUS_NO_ERROR;
	XmgrCas_NaAlamType_e			eScAlarm;


	switch(message)
	{
		case MSG_GWM_CREATE:
			HxLOG_Info("MSG_GWM_CREATE!\n");
#ifdef TEST_TIVU_UI_MENU
			HxLOG_Info("p1 = 0x%x, p2 = 0x%x\n", p1, p2);
			if(p2 != eCAS_NA_CA_SMARTCARD_REMOVED
				&& p2 != eCAS_NA_CA_SMARTCARD_COM_ERROR
				&& p2 != eCAS_NA_CA_SMARTCARD_SUSPENDED
				&& p2 != eCAS_NA_CA_SMARTCARD_BLACKLISTED )
			{
				p2 = eCAS_NA_CA_SMARTCARD_OK;
			}
			gTestMenuCardStatus = p2;
#endif
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_TIVU_MENU_SCERROR_WINDOW_X, NAPPV_TIVU_MENU_SCERROR_WINDOW_Y,
								NAPPV_TIVU_MENU_SCERROR_WINDOW_WIDTH, NAPPV_TIVU_MENU_SCERROR_WINDOW_HEIGHT);

			/* Init the contents */
			ulResult = xmgr_cas_NaPpvMenuScErrorContents_Tivu(pstContents);
			if (ulResult != ERR_BUS_NO_ERROR)
			{
				break;
			}
			if(ulResult != ERR_BUS_NO_ERROR)			HxLOG_Critical("Critical Eror!!!\n");

			/* draw UI object */
			xmgr_cas_NaPpvMenuScErrorDrawDafaultUi_Tivu(pstContents);
			break;

		case MSG_GWM_FAIL_RECREATE_NEW_INSTANCE:
			break;

		case MSG_APP_UPDATE_SMARTCARD_INFO:
			if (p1 == eDxCAS_GROUPID_NA)
			{
				ulResult = xmgr_cas_NaPpvMenuScErrorContents_Tivu(pstContents);
				if (ulResult != ERR_BUS_NO_ERROR)
				{
					break;
				}
				HxLOG_Print("pstContents->cardStatus = %x\n", pstContents->cardStatus);

				xmgr_cas_NaPpvMenuScErrorDrawDafaultUi_Tivu(pstContents);

				GWM_APP_InvalidateObject(NAPPV_TIVU_MENU_SCERROR_MSG_TEXT_ID);
				GWM_APP_InvalidateObject(NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID);
				GWM_APP_InvalidateObject(NAPPV_TIVU_MENU_SCERROR_MSG_DETAIL_TEXT_ID);
				GWM_APP_InvalidateObject(NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_ID);
				GWM_APP_InvalidateObject(NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_ID);
			}
			break;

		case eSEVT_CAS_SC_INSERTED:
		case eSEVT_CAS_SC_REMOVED:
			if (p2 == eDxCAS_GROUPID_NA)
			{
				ulResult = xmgr_cas_NaPpvMenuScErrorContents_Tivu(pstContents);
				if (ulResult != ERR_BUS_NO_ERROR)
				{
					break;
				}
				HxLOG_Print("pstContents->cardStatus = %x\n", pstContents->cardStatus);

				xmgr_cas_NaPpvMenuScErrorDrawDafaultUi_Tivu(pstContents);

				GWM_APP_InvalidateObject(NAPPV_TIVU_MENU_SCERROR_MSG_TEXT_ID);
				GWM_APP_InvalidateObject(NAPPV_TIVU_MENU_SCERROR_MSG_TIVUSAT_ICON_ID);
				GWM_APP_InvalidateObject(NAPPV_TIVU_MENU_SCERROR_MSG_DETAIL_TEXT_ID);
				GWM_APP_InvalidateObject(NAPPV_TIVU_MENU_SCERROR_MSG_OK_BUTTON_ID);
				GWM_APP_InvalidateObject(NAPPV_TIVU_MENU_SCERROR_MSG_OK_TEXT_ID);
			}
			break;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_KEYUP :
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN :
			AP_KeyConv_UiCommon(&p1);

#ifndef TEST_TIVU_UI_MENU
			ulResult = Settings_CheckKeyPass(p1, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
			if(ulResult != ERR_OK)
			{
				return ulResult;
			}
#endif

			switch(p1)
			{
#ifdef TEST_TIVU_UI_MENU
				case KEY_1:
					BUS_MGR_Destroy(NULL);
					BUS_MGR_Create("xproc_cas_NaPpvMenuSmartCardError_Tivu",
									APP_CAS_PRIORITY,
									xproc_cas_NaPpvMenuSmartCardError_Tivu,
									SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
									1,
									0x100a/*eCAS_NA_CA_SMARTCARD_REMOVED*/,
									0);
					return MESSAGE_BREAK;
				case KEY_2:
					BUS_MGR_Destroy(NULL);
					BUS_MGR_Create("xproc_cas_NaPpvMenuSmartCardError_Tivu",
									APP_CAS_PRIORITY,
									xproc_cas_NaPpvMenuSmartCardError_Tivu,
									SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
									1,
									0x1001/*eCAS_NA_CA_SMARTCARD_COM_ERROR*/,
									0);
					return MESSAGE_BREAK;
				case KEY_3:
					BUS_MGR_Destroy(NULL);
					BUS_MGR_Create("xproc_cas_NaPpvMenuSmartCardError_Tivu",
									APP_CAS_PRIORITY,
									xproc_cas_NaPpvMenuSmartCardError_Tivu,
									SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
									1,
									0x1004/*eCAS_NA_CA_SMARTCARD_BLACKLISTED*/,
									0);
					return MESSAGE_BREAK;
				case KEY_4:
					BUS_MGR_Destroy(NULL);
					BUS_MGR_Create("xproc_cas_NaPpvMenuSmartCardError_Tivu",
									APP_CAS_PRIORITY,
									xproc_cas_NaPpvMenuSmartCardError_Tivu,
									SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
									1,
									0x1005/*eCAS_NA_CA_SMARTCARD_SUSPENDED*/,
									0);
					return MESSAGE_BREAK;
#endif

				case KEY_EXIT:
				case KEY_MENU:
					Settings_ClearWindowTitle();

					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				case KEY_BACK:
					BUS_MGR_Destroy(NULL);

					/* to upper depth */
					BUS_MGR_Create("AP_CAS_PpvMenu_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)AP_CAS_PpvMenu_Proc, 0, 0, 0, 0);
					return MESSAGE_BREAK;

				case KEY_OK:
					if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_OK)
					{
						BUS_MGR_Destroy(NULL);
						BUS_MGR_Create("xmgr_cas_NaPpvMenuSmartCardInfo_Tivu",
										APP_CAS_PRIORITY,
										xmgr_cas_NaPpvMenuSmartCardInfo_Tivu,
										SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
										0,
										0,
										0);
					}
					return MESSAGE_BREAK;

			}
			break;


		case MSG_GWM_TIMER:
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			xmgr_cas_NaPpvMenuScErrorDestroyUI_Tivu(pstContents);

			break;

		default:
			break;
	}

	HxLOG_Info("xproc_cas_NaPpvMenuSmartCardError_Tivu handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif	//end of #if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
/* End of File */
