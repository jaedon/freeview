
#if defined(CONFIG_MW_CAS_NAGRA)
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


#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
//******************************************************************************
//	DEFINE
//******************************************************************************

typedef struct tag_CAS_NA_TIVU_BAN_SCE_Contents
{
	HUINT8		bDisplayMode;
	HINT32		cardStatus;
	HUINT8		szSmartCardErrorDateStr[32];
	HUINT8		*pszSmartCardErrorMsgText;
	HUINT8		*pszSmartCardErrorMsgDetailText;
} XmgrCas_NaBanCardError_Tivu_t;

#define	NAPPV_TIVU_BAN_SCERROR_WINDOW_WIDTH										OSD_SCREEN_WIDTH
#define	NAPPV_TIVU_BAN_SCERROR_WINDOW_HEIGHT									OSD_SCREEN_HEIGHT
#define	NAPPV_TIVU_BAN_SCERROR_WINDOW_X											0
#define	NAPPV_TIVU_BAN_SCERROR_WINDOW_Y											0

#define	NAPPV_TIVU_BAN_SCERROR_APP_ID											0

/* ppv tivu scerror msg background*/
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_ID								(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x0010)
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_X									0
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_Y									0
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_WIDTH								OSD_SCREEN_WIDTH
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_HEIGHT							OSD_SCREEN_HEIGHT

#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_UP_LINE_ID						(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x0020)
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_UP_LINE_X							0
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_UP_LINE_Y							316
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_UP_LINE_WIDTH						OSD_SCREEN_WIDTH
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_UP_LINE_HEIGHT					8

#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_DOWN_LINE_ID						(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x0030)
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_DOWN_LINE_X						0
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_DOWN_LINE_Y						461
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_DOWN_LINE_WIDTH					OSD_SCREEN_WIDTH
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_DOWN_LINE_HEIGHT					8

#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_ID					(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x0040)
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_X					432
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_Y					316
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_WIDTH				848
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_HEIGHT				8

#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_ID				(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x0050)
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_X					432
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_Y					491
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_WIDTH	848
#define	NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_HEIGHT			8

/* ppv tivu scerror date */
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_ID							(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x0060)
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_X							944
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_Y							129
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_WIDTH						140
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_HEIGHT						24

#define	NAPPV_TIVU_BAN_SCERROR_MSG_DATE_LINE_ID									(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x0070)
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DATE_LINE_X									943
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DATE_LINE_Y									134
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DATE_LINE_WIDTH								337
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DATE_LINE_HEIGHT								9

#define	NAPPV_TIVU_BAN_SCERROR_MSG_PRESENT_DATE_TEXT_ID							(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x0080)
#define	NAPPV_TIVU_BAN_SCERROR_MSG_PRESENT_DATE_TEXT_X							945
#define	NAPPV_TIVU_BAN_SCERROR_MSG_PRESENT_DATE_TEXT_Y							167
#define	NAPPV_TIVU_BAN_SCERROR_MSG_PRESENT_DATE_TEXT_WIDTH						140
#define	NAPPV_TIVU_BAN_SCERROR_MSG_PRESENT_DATE_TEXT_HEIGHT						25

/* ppv tivu scerror warning */
#define	NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_ID								(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x0090)
#define	NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_X								468
#define	NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_Y								308
#define	NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_WIDTH							110
#define	NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_HEIGHT							24

#define	NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_ID							(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x00a0)
#define	NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_X							468
#define	NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_Y							308
#define	NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_WIDTH						110
#define	NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_HEIGHT						24

/* ppv tivu scerror tivusat icon */
#define	NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID								(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x00b0)

/* ppv tivu scerror msg */
#define	NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_ID										(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x00c0)
#define	NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_X										459
#define	NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_Y										406
#define	NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_WIDTH									630
#define	NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_HEIGHT									42

#define	NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_ID								(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x00d0)
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_X								459
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_Y								546
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_WIDTH							435
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_HEIGHT							42

#define	NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_ID									(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x00e0)
#define	NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_X									459
#define	NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_Y									389
#define	NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_WIDTH								700
#define	NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_HEIGHT								42

#define	NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_02_ID							(NAPPV_TIVU_BAN_SCERROR_APP_ID|0x00f0)
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_02_X								437
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_02_Y								531
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_02_WIDTH							700
#define	NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_02_HEIGHT						84

//#define TEST_TIVU_UI_BANNER
#ifdef TEST_TIVU_UI_BANNER
STATIC HINT32 gTestBanCardStatus = 0;
#endif

//******************************************************************************
//	STATIC VARIABLES
//******************************************************************************
STATIC XmgrCas_NaBanCardError_Tivu_t 			s_stNaTivuBanCardError;


//******************************************************************************
//	PROTOTYPE FUNCTIONS
//******************************************************************************
#define ________XMGR_CAS_NA_PpvTIVU_BANNER_STATIC___________________________________________
STATIC inline XmgrCas_NaBanCardError_Tivu_t* xmgr_cas_NaPpvBanScErrorGetLocalContents_Tivu(void);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorInitContents_Tivu(XmgrCas_NaBanCardError_Tivu_t *pstContents);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorGetStringWidth_Tivu(HUINT8* string, HUINT32 ulFontSize, HUINT32 *ulWidth);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage01_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage02_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImageLongLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImageShortLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgTivuSatImageIcon_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgPresentDateLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgPresentDate_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgTitle_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgDetail_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Tivu(XmgrCas_NaBanCardError_Tivu_t *pstContents);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDestroyUI_Tivu(XmgrCas_NaBanCardError_Tivu_t *pstContents);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDestroyObject_Tivu(viod);

#define ________XMGR_CAS_NA_PpvTIVU_BANNER_PUBLIC___________________________________________
BUS_Result_t xproc_cas_NaBannerSmartCardError_Tivu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#define	_________________________STATIC_BODY______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorGetLocalContents_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC inline XmgrCas_NaBanCardError_Tivu_t* xmgr_cas_NaPpvBanScErrorGetLocalContents_Tivu(void)
{
	return &s_stNaTivuBanCardError;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorInitContents_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorInitContents_Tivu(XmgrCas_NaBanCardError_Tivu_t *pstContents)
{
	XmgrCas_NaAlamType_e 		eScAlarm;

	xmgr_cas_NaGetSmartCardStatus(&eScAlarm);
	HxLOG_Print("eScAlarm = %x\n", eScAlarm);
#ifdef TEST_TIVU_UI_BANNER
	eScAlarm = gTestBanCardStatus;
#endif

	memset(pstContents, 0, sizeof(XmgrCas_NaBanCardError_Tivu_t));

	/*status error msg*/
	pstContents->bDisplayMode = 1;
	pstContents->cardStatus = eScAlarm;

	/*Title , Detail error msg*/
	if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_REMOVED)
	{
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_SCRAMBLED_TEXT_ID);
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_INSERT_1_TEXT_ID);
	}
	else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_SUSPENDED)
	{
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_BLOCKED_TEXT_ID);
		pstContents->pszSmartCardErrorMsgDetailText = NULL;
	}
	else if (pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_BLACKLISTED)
	{
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_BLACKLIST_1_TEXT_ID);
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_BLACKLIST_DETAIL_TEXT_ID);
	}
	else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_INVALID
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_COM_ERROR
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_MUTE
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NEVER_PAIRED
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
	{
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_UNREADABLE_1_TEXT_ID);
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_UNREADABLE_DETAIL_TEXT_ID);
	}
	else
	{
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_UNREADABLE_1_TEXT_ID);
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_UNREADABLE_DETAIL_TEXT_ID);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorGetStringWidth_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorGetStringWidth_Tivu(HUINT8* string, HUINT32 ulFontSize, HUINT32 *ulWidth)
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
 Function		: xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage01_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage01_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e931_1_41_CAS_BG01, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage02_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage02_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImageLongLine_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImageLongLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e931_1_41_CAS_Line01, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImageShortLine_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImageShortLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvBanScErrorDrawMsgTivuSatImageIcon_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgTivuSatImageIcon_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvBanScErrorDrawMsgPresentDateLine_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgPresentDateLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvBanScErrorDrawMsgPresentDate_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgPresentDate_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	OSD_Rect_t				rect_msg;

	UNIXTIME				ulCurTime= 0;
	BUS_Result_t			hRes;
	HxDATETIME_t 			stCurDateTime;

	XmgrCas_NaBanCardError_Tivu_t	*pstContents = xmgr_cas_NaPpvBanScErrorGetLocalContents_Tivu();

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
		HxLOG_Print("[xmgr_cas_NaPpvBanScErrorDrawMsgPresentDate_Tivu] ConvertUnixTimeToDateTime fail!!\n");
		return ERR_FAIL;
	}

	sprintf(pstContents->szSmartCardErrorDateStr, "%02d.%02d.%02d\n", stCurDateTime.stDate.ucDay, stCurDateTime.stDate.ucMonth, stCurDateTime.stDate.usYear);
	OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XS, rect_msg.x, rect_msg.y, rect_msg.w,
								pstContents->szSmartCardErrorDateStr, COL(C_T_Cursor));

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgTitle_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgTitle_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hErr;
	OSD_Rect_t				rect_msg;
	OSD_Rect_t				rect_image;
	HUINT32					prevStringWidth = 0;
	HxLANG_Id_e			eMenuLang = eLangID_English;

	XmgrCas_NaBanCardError_Tivu_t	*pstContents = xmgr_cas_NaPpvBanScErrorGetLocalContents_Tivu();

	/* for compiler */
	(void)pArg;
	(void)param1;

	rect_msg.x = pArea->x1 + pRect->x;
	rect_msg.y = pArea->x1 + pRect->y;
	rect_msg.w = pRect->w;
	rect_msg.h = pRect->h;

	if (MWC_UTIL_DvbStrlenStrOnly(pstContents->pszSmartCardErrorMsgText) > 0)
	{
		if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_REMOVED)
		{
			/* 1.Draw Text */
			OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
										(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_SCRAMBLED_TEXT_ID), COL(C_T_Cursor));
		}
		else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_BLACKLISTED)
		{
			xmgr_cas_NaPpvBanScErrorGetStringWidth_Tivu( (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_BLACKLIST_1_TEXT_ID), FONT_SIZE_XXL, &prevStringWidth);

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
											 (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_BLACKLIST_1_TEXT_ID), COL(C_T_Cursor));

				/* 2.Draw Tivu Icon*/
				rect_msg.x += (8 + prevStringWidth);
				rect_image.x = rect_msg.x;
				rect_image.y = rect_msg.y - (FONT_SIZE_XXL+6);
				rect_image.w = rect_msg.w;
				rect_image.h = rect_msg.h;
				GWM_Frame_Create (NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID, &rect_image, NULL);
				GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgTivuSatImageIcon_Tivu);

				/* 3.Draw Text */
				rect_msg.x -= (8 + prevStringWidth);
				rect_msg.y += (8 + FONT_SIZE_XXL);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											 (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_BLACKLIST_2_TEXT_ID), COL(C_T_Cursor));

			}
			else if(eMenuLang == eLangID_Greek)
			{
				/* 1.Draw Text */
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											 (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_BLACKLIST_1_TEXT_ID), COL(C_T_Cursor));

				/* 2.Draw Text */
				rect_msg.x += (8 + prevStringWidth);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_BLACKLIST_2_TEXT_ID), COL(C_T_Cursor));

				/* 3.Draw Tivu Icon*/
				rect_msg.x -= (8 + prevStringWidth);
				rect_msg.y += (8 + FONT_SIZE_XXL);
				rect_image.x = rect_msg.x;
				rect_image.y = rect_msg.y - (FONT_SIZE_XXL+6);
				rect_image.w = rect_msg.w;
				rect_image.h = rect_msg.h;
				GWM_Frame_Create (NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID, &rect_image, NULL);
				GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgTivuSatImageIcon_Tivu);
			}
			else // eng
			{
				/* 1.Draw Tivu Icon*/
				rect_image.x = rect_msg.x;
				rect_image.y = rect_msg.y - (FONT_SIZE_XXL+6);
				rect_image.w = rect_msg.w;
				rect_image.h = rect_msg.h;
				GWM_Frame_Create (NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID, &rect_image, NULL);
				GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgTivuSatImageIcon_Tivu);

				/* 2.Draw Text */
				rect_msg.x += (8 + 197);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											 (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_BLACKLIST_1_TEXT_ID), COL(C_T_Cursor));

				/* 3.Draw Text */
				rect_msg.x -= (8 + 197);
				rect_msg.y += (8 + FONT_SIZE_XXL);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_BLACKLIST_2_TEXT_ID), COL(C_T_Cursor));
			}
		}
		else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_SUSPENDED)
		{
			/* 1.Draw Text */
			OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
										(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_BLOCKED_TEXT_ID), COL(C_T_Cursor));
		}
		else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_INVALID
			|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_COM_ERROR
			|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_MUTE
			|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NEVER_PAIRED
			|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
		{
			/* 1.Draw Text */
			OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
										(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_UNREADABLE_1_TEXT_ID), COL(C_T_Cursor));

			/* 2.Draw Text */
			rect_msg.y += (8 + FONT_SIZE_XXL);
			OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
										(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_UNREADABLE_2_TEXT_ID), COL(C_T_Cursor));
		}
		else
		{
			/* 1.Draw Text */
			OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
										(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_UNREADABLE_1_TEXT_ID), COL(C_T_Cursor));

			/* 2.Draw Text */
			rect_msg.y += (8 + FONT_SIZE_XXL);
			OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
										(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_UNREADABLE_2_TEXT_ID), COL(C_T_Cursor));
		}
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgDetail_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgDetail_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	OSD_Rect_t				rect_msg;
	OSD_Rect_t				rect_image;
	HUINT32					prevStringWidth = 0;
	Text_Property_t			stTextProperty;
	HxLANG_Id_e			eMenuLang = eLangID_English;

	XmgrCas_NaBanCardError_Tivu_t	*pstContents = xmgr_cas_NaPpvBanScErrorGetLocalContents_Tivu();

	/* for compiler */
	(void)pArg;

	rect_msg.x = pArea->x1 + pRect->x;
	rect_msg.y = pArea->x1 + pRect->y;
	rect_msg.w = pRect->w;
	rect_msg.h = pRect->h;

	if (MWC_UTIL_DvbStrlenStrOnly(pstContents->pszSmartCardErrorMsgDetailText) > 0)
	{
		if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_REMOVED)
		{
			xmgr_cas_NaPpvBanScErrorGetStringWidth_Tivu((HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_INSERT_1_TEXT_ID), FONT_SIZE_XXL, &prevStringWidth);

//			eMenuLang = MWC_PARAM_GetMainLanguage();
			hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32 *)&eMenuLang, 0);
			if (ERR_OK != hError)
			{
				HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN) Error!!\n");
			}
			if((eMenuLang == eLangID_Italian) ||(eMenuLang == eLangID_Czech) || (eMenuLang == eLangID_Spanish))
			{
				/* 1.Draw Text */
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_INSERT_1_TEXT_ID), COL(C_T_Cursor));

				/* 2.Draw Text */
				rect_msg.x += (8 + prevStringWidth);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_INSERT_2_TEXT_ID), COL(C_T_Cursor));

				/* 3.Draw Tivu Icon*/
				xmgr_cas_NaPpvBanScErrorGetStringWidth_Tivu((HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_INSERT_2_TEXT_ID), FONT_SIZE_XXL, &prevStringWidth);
				rect_msg.x += (8 + prevStringWidth);
				rect_image.x = rect_msg.x;
				rect_image.y = rect_msg.y - (FONT_SIZE_XXL+6);
				rect_image.w = rect_msg.w;
				rect_image.h = rect_msg.h;
				GWM_Frame_Create (NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID, &rect_image, NULL);
				GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgTivuSatImageIcon_Tivu);
			}
			else if(eMenuLang == eLangID_German)
			{
				/* 1.Draw Tivu Icon*/
				rect_image.x = rect_msg.x;
				rect_image.y = rect_msg.y - (FONT_SIZE_XXL+6);
				rect_image.w = rect_msg.w;
				rect_image.h = rect_msg.h;
				GWM_Frame_Create (NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID, &rect_image, NULL);
				GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgTivuSatImageIcon_Tivu);

				/* 2.Draw Text */
				rect_msg.x += (8 + 197);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_INSERT_2_TEXT_ID), COL(C_T_Cursor));

				/* 3.Draw Text */
				xmgr_cas_NaPpvBanScErrorGetStringWidth_Tivu((HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_INSERT_2_TEXT_ID), FONT_SIZE_XXL, &prevStringWidth);
				rect_msg.x += (8 + prevStringWidth);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_INSERT_1_TEXT_ID), COL(C_T_Cursor));
			}
			else
			{
				/* 1.Draw Text */
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_INSERT_1_TEXT_ID), COL(C_T_Cursor));

				/* 2.Draw Tivu Icon*/
				rect_msg.x += (8 + prevStringWidth);
				rect_image.x = rect_msg.x;
				rect_image.y = rect_msg.y - (FONT_SIZE_XXL+6);
				rect_image.w = rect_msg.w;
				rect_image.h = rect_msg.h;
				GWM_Frame_Create (NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID, &rect_image, NULL);
				GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgTivuSatImageIcon_Tivu);

				/* 3.Draw Text */
				rect_msg.x += (8 + 197);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XXL, rect_msg.x, rect_msg.y, rect_msg.w,
											(HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_CARD_INSERT_2_TEXT_ID), COL(C_T_Cursor));
			}
		}
		else
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
 Function		: xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Tivu(XmgrCas_NaBanCardError_Tivu_t *pstContents)
{
	OSD_Rect_t				rect = {0, };

	/* for compiler */
	(void)pstContents;

	xmgr_cas_NaPpvBanScErrorDestroyObject_Tivu();

#if 0
	if (xmgr_cas_NaTIVU_CheckStream() != ERR_OK)
	{
		HxLOG_Print("don't draw\n");

		/* na ppv tivu scerror card error text*/
		pstContents->cardStatus = eCAS_NA_CA_SMARTCARD_REMOVED;
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_X, NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_Y, NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_HEIGHT);
		GWM_Text_Create (NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgTitle_Tivu);

		return ERR_BUS_NO_ERROR;
	}
#endif

	/* na ppv tivu scerror background */
	Settings_DrawBack(FALSE);

	if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_REMOVED)
	{
		/* na ppv tivu scerror card error background 01*/
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_X, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_Y, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_HEIGHT);
		GWM_Frame_Create (NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage01_Tivu);

		/* Draw Panel Line*/
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_UP_LINE_X, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_UP_LINE_Y, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_UP_LINE_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_UP_LINE_HEIGHT);
		GWM_Frame_Create (NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_UP_LINE_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_UP_LINE_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImageLongLine_Tivu);

		/* Draw Panel Line*/
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_DOWN_LINE_X, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_DOWN_LINE_Y, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_DOWN_LINE_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_DOWN_LINE_HEIGHT);
		GWM_Frame_Create (NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_DOWN_LINE_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_DOWN_LINE_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImageLongLine_Tivu);


		/* na ppv tivu warning text */
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_X, NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_Y - FONT_SIZE_XXS, NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_HEIGHT);
		GWM_Text_Create (NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_ATTENTION_TEXT_ID));
		GWM_Text_SetAlign (NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_ID, FONT_SIZE_XXS);
		GWM_Text_ChangeColor (NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_ID, COL(C_T_Cursor), 0x00);

		/* na ppv tivu scerror card error text*/
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_X, NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_Y, NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_HEIGHT);
		GWM_Text_Create (NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgTitle_Tivu);

		/* na ppv tivu scerror card error detail text*/
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_X, NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_Y, NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_HEIGHT);
		GWM_Text_Create (NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgDetail_Tivu);

	}
	else
	{
		/* na ppv tivu scerror card error background 02*/
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_X, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_Y, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_HEIGHT);
		GWM_Frame_Create (NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage02_Tivu);

		/* Draw Panel Line*/
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_X, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_Y, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_HEIGHT);
		GWM_Frame_Create (NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImageShortLine_Tivu);

		if(pstContents->pszSmartCardErrorMsgDetailText != NULL)
		{
			/* Draw Panel Line*/
			OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_X, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_Y, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_HEIGHT);
			GWM_Frame_Create (NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_ID, &rect, NULL);
			GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImageShortLine_Tivu);
		}

		/* na ppv tivu date text */
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_X, NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_Y - FONT_SIZE_XXS, NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_HEIGHT);
		GWM_Text_Create (NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_DATE_TEXT_ID));
		GWM_Text_SetAlign (NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_ID, FONT_SIZE_XXS);
		GWM_Text_ChangeColor (NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_ID, COL(C_T_Cursor), 0x00);

		/* na ppv tivu date line */
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_DATE_LINE_X, NAPPV_TIVU_BAN_SCERROR_MSG_DATE_LINE_Y, NAPPV_TIVU_BAN_SCERROR_MSG_DATE_LINE_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_DATE_LINE_HEIGHT);
		GWM_Frame_Create (NAPPV_TIVU_BAN_SCERROR_MSG_DATE_LINE_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_DATE_LINE_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgPresentDateLine_Tivu);

		/* na ppv tivu present date text */
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_PRESENT_DATE_TEXT_X, NAPPV_TIVU_BAN_SCERROR_MSG_PRESENT_DATE_TEXT_Y, NAPPV_TIVU_BAN_SCERROR_MSG_PRESENT_DATE_TEXT_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_PRESENT_DATE_TEXT_HEIGHT);
		GWM_Text_Create (NAPPV_TIVU_BAN_SCERROR_MSG_PRESENT_DATE_TEXT_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_PRESENT_DATE_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgPresentDate_Tivu);


		/* na ppv tivu warning text */
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_X, NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_Y - FONT_SIZE_XXS, NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_HEIGHT);
		GWM_Text_Create (NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_BANNER_ERR_ATTENTION_TEXT_ID));
		GWM_Text_SetAlign (NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_ID, FONT_SIZE_XXS);
		GWM_Text_ChangeColor (NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_ID, COL(C_T_Cursor), 0x00);

		/* na ppv tivu scerror card error text*/
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_X, NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_Y, NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_HEIGHT);
		GWM_Text_Create (NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgTitle_Tivu);

		/* na ppv tivu scerror card error detail text*/
		OSD_GFX_SetRect (&rect, NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_02_X, NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_02_Y, NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_02_WIDTH, NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_02_HEIGHT);
		GWM_Text_Create (NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_02_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_02_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgDetail_Tivu);

	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDestroyUI_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDestroyUI_Tivu(XmgrCas_NaBanCardError_Tivu_t *pstContents)
{
	/* for compiler */
	(void)pstContents;

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDestroyObject_Tivu(viod)
{
	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_ID);
	}

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_UP_LINE_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_UP_LINE_ID);
	}

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_DOWN_LINE_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_DOWN_LINE_ID);
	}

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_ID);
	}

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_ID);
	}

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_ID);
	}

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_TEXT_02_ID);
	}

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_02_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_DETAIL_TEXT_02_ID);
	}

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEUP_ID);
	}

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_BACKGROUND_NORMAL_LINEDOWN_ID);
	}

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_DATE_TITLE_TEXT_ID);
	}

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_DATE_LINE_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_DATE_LINE_ID);
	}

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_PRESENT_DATE_TEXT_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_PRESENT_DATE_TEXT_ID);
	}

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_WARNING_TEXT_02_ID);
	}

	if(GWM_Obj_GetObjectByID(NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID))
	{
		GWM_Obj_Destroy(NAPPV_TIVU_BAN_SCERROR_MSG_TIVUSAT_ICON_ID);
	}

	return ERR_BUS_NO_ERROR;
}

#define	_________________________PUBLIC_BODY______________________________
/******************************************************************************/
/**
*   \fn     xproc_cas_NaBannerSmartCardError_Tivu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaBannerSmartCardError_Tivu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaBanCardError_Tivu_t			*pstContents = xmgr_cas_NaPpvBanScErrorGetLocalContents_Tivu();
	BUS_Result_t						ulResult = ERR_BUS_NO_ERROR;

	switch(message)
	{
		case MSG_GWM_CREATE:
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_NaBannerSmartCardError_Tivu");
			HxLOG_Info("MSG_GWM_CREATE!\n");
#ifdef TEST_TIVU_UI_BANNER
			HxLOG_Info("p1 = 0x%x, p2 = 0x%x\n", p1, p2);
			if(p2 != eCAS_NA_CA_SMARTCARD_REMOVED
				&& p2 != eCAS_NA_CA_SMARTCARD_COM_ERROR
				&& p2 != eCAS_NA_CA_SMARTCARD_SUSPENDED
				&& p2 != eCAS_NA_CA_SMARTCARD_BLACKLISTED )
			{
				p2 = eCAS_NA_CA_SMARTCARD_OK;
			}
			gTestBanCardStatus = p2;
#endif
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_TIVU_BAN_SCERROR_WINDOW_X, NAPPV_TIVU_BAN_SCERROR_WINDOW_Y,
								NAPPV_TIVU_BAN_SCERROR_WINDOW_WIDTH, NAPPV_TIVU_BAN_SCERROR_WINDOW_HEIGHT);

			/* Init the contents */
			if (BUS_MGR_GetThis())
			{
				ulResult = xmgr_cas_NaPpvBanScErrorInitContents_Tivu(pstContents);
				if (ulResult != ERR_BUS_NO_ERROR)
				{
					break;
				}
				if(ulResult != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

				/* draw UI object */
				xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Tivu(pstContents);
			}
			break;

		case MSG_GWM_FAIL_RECREATE_NEW_INSTANCE:
			break;

		case MSG_APP_UPDATE_SMARTCARD_INFO:
			if (p2 == eDxCAS_GROUPID_NA && BUS_MGR_GetThis())
			{
				ulResult = xmgr_cas_NaPpvBanScErrorInitContents_Tivu(pstContents);
				if (ulResult != ERR_BUS_NO_ERROR)
				{
					break;
				}
				if(ulResult != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

				HxLOG_Print("pstContents->cardStatus = %x\n", pstContents->cardStatus);

				/* draw UI object */
				xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Tivu(pstContents);
			}
			break;

		case eSEVT_CAS_SC_REMOVED:
		case eSEVT_CAS_SC_INSERTED:
			if (p2 == eDxCAS_GROUPID_NA && BUS_MGR_GetThis())
			{
				ulResult = xmgr_cas_NaPpvBanScErrorInitContents_Tivu(pstContents);
				if (ulResult != ERR_BUS_NO_ERROR)
				{
					break;
				}
				if(ulResult != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

				HxLOG_Print("pstContents->cardStatus = %x\n", pstContents->cardStatus);

				/* draw UI object */
				xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Tivu(pstContents);
			}
			break;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_KEYUP :
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_TIMER:
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			xmgr_cas_NaPpvBanScErrorDestroyUI_Tivu(pstContents);

			break;

		case MSG_GWM_QUIT_UI:
			HxLOG_Print("MESSAGE_PASS: MSG_GWM_QUIT_UI\n");
			return MESSAGE_PASS;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

#endif //#if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)

#endif //CONFIG_MW_CAS_NAGRA