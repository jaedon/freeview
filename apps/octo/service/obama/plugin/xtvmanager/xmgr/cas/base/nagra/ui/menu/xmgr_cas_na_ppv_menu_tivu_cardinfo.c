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
typedef struct tag_CAS_NA_TIVU_MENU_SCI_Contents
{
	HUINT8		szSmartCardErrorDateStr[32];
	HUINT8		*pszCardNumberTitle;
	HUINT8		*pszCaVersionTitle;
	HUINT8 		szSmartCardNumber[32];
	HUINT8		szCaVersion[32];
} XmgrCas_NaMenuCardInfo_Tivu_t;

#define NAPPV_TIVU_MENU_SCINFO_WINDOW_WIDTH											OSD_SCREEN_WIDTH
#define NAPPV_TIVU_MENU_SCINFO_WINDOW_HEIGHT										OSD_SCREEN_HEIGHT
#define NAPPV_TIVU_MENU_SCINFO_WINDOW_X												(OSD_SCREEN_WIDTH/2-NAPPV_TIVU_MENU_SCINFO_WINDOW_WIDTH/2)
#define NAPPV_TIVU_MENU_SCINFO_WINDOW_Y												(OSD_SCREEN_HEIGHT/2-NAPPV_TIVU_MENU_SCINFO_WINDOW_HEIGHT/2)

#define	NAPPV_TIVU_MENU_SCINFO_APP_ID												0

/* ppv tivu scinfo msg background*/
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_ID									(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x0010)
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_X										0
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_Y										0
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_WIDTH									OSD_SCREEN_WIDTH
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_HEIGHT								OSD_SCREEN_HEIGHT

#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEUP_ID						(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x0020)
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEUP_X						432
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEUP_Y						316
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEUP_WIDTH					848
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEUP_HEIGHT					8

#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEDOWN_ID					(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x0030)
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEDOWN_X						432
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEDOWN_Y						491
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEDOWN_WIDTH					848
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEDOWN_HEIGHT				8

/* ppv tivu scinfo date */
#define	NAPPV_TIVU_MENU_SCINFO_MSG_DATE_TITLE_TEXT_ID								(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x0040)
#define	NAPPV_TIVU_MENU_SCINFO_MSG_DATE_TITLE_TEXT_X								944
#define	NAPPV_TIVU_MENU_SCINFO_MSG_DATE_TITLE_TEXT_Y								129
#define	NAPPV_TIVU_MENU_SCINFO_MSG_DATE_TITLE_TEXT_WIDTH							140
#define	NAPPV_TIVU_MENU_SCINFO_MSG_DATE_TITLE_TEXT_HEIGHT							24

#define	NAPPV_TIVU_MENU_SCINFO_MSG_DATE_LINE_ID										(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x0050)
#define	NAPPV_TIVU_MENU_SCINFO_MSG_DATE_LINE_X										943
#define	NAPPV_TIVU_MENU_SCINFO_MSG_DATE_LINE_Y										134
#define	NAPPV_TIVU_MENU_SCINFO_MSG_DATE_LINE_WIDTH									337
#define	NAPPV_TIVU_MENU_SCINFO_MSG_DATE_LINE_HEIGHT									9

#define	NAPPV_TIVU_MENU_SCINFO_MSG_PRESENT_DATE_TEXT_ID								(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x0060)
#define	NAPPV_TIVU_MENU_SCINFO_MSG_PRESENT_DATE_TEXT_X								945
#define	NAPPV_TIVU_MENU_SCINFO_MSG_PRESENT_DATE_TEXT_Y								167
#define	NAPPV_TIVU_MENU_SCINFO_MSG_PRESENT_DATE_TEXT_WIDTH							140
#define	NAPPV_TIVU_MENU_SCINFO_MSG_PRESENT_DATE_TEXT_HEIGHT							25

/* ppv tivu scinfo info */
#define	NAPPV_TIVU_MENU_SCINFO_MSG_INFO_TEXT_ID										(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x0070)
#define	NAPPV_TIVU_MENU_SCINFO_MSG_INFO_TEXT_X										468
#define	NAPPV_TIVU_MENU_SCINFO_MSG_INFO_TEXT_Y										308
#define	NAPPV_TIVU_MENU_SCINFO_MSG_INFO_TEXT_WIDTH									352
#define	NAPPV_TIVU_MENU_SCINFO_MSG_INFO_TEXT_HEIGHT									31

/* ppv tivu back button image */
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACK_BUTTON_ID									(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x0080)
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACK_BUTTON_X									943
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACK_BUTTON_Y									595
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACK_BUTTON_WIDTH								337
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACK_BUTTON_HEIGHT								31

#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACK_TEXT_ID										(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x0090)
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACK_TEXT_X										955
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACK_TEXT_Y										648
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACK_TEXT_WIDTH									200
#define	NAPPV_TIVU_MENU_SCINFO_MSG_BACK_TEXT_HEIGHT									24

/* ppv tivu scinfo tivusat icon */
#define	NAPPV_TIVU_MENU_SCINFO_MSG_TIVUSAT_ICON_ID									(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x00a0)

/* ppv tivu scinfo msg */
#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_TEXT_ID									(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x00b0)
#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_TEXT_X									459
#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_TEXT_Y									378
#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_TEXT_WIDTH								270
#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_TEXT_HEIGHT								29

#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_LINE_ID									(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x00c0)
#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_LINE_X									432
#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_LINE_Y									383
#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_LINE_WIDTH								300
#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_LINE_HEIGHT								2

#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_MSG_TEXT_ID									(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x00d0)
#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_MSG_TEXT_X									739
#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_MSG_TEXT_Y									378
#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_MSG_TEXT_WIDTH								480
#define	NAPPV_TIVU_MENU_SCINFO_NUMBER_MSG_TEXT_HEIGHT								29

#define	NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_TEXT_ID								(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x00e0)
#define	NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_TEXT_X									459
#define	NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_TEXT_Y									446
#define	NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_TEXT_WIDTH								270
#define	NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_TEXT_HEIGHT							29

#define	NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_LINE_ID								(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x00f0)
#define	NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_LINE_X									432
#define	NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_LINE_Y									383 + 69
#define	NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_LINE_WIDTH								300
#define	NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_LINE_HEIGHT							2

#define	NAPPV_TIVU_MENU_SCINFO_VERSION_MSG_TEXT_ID									(NAPPV_TIVU_MENU_SCINFO_APP_ID|0x0100)
#define	NAPPV_TIVU_MENU_SCINFO_VERSION_MSG_TEXT_X									739
#define	NAPPV_TIVU_MENU_SCINFO_VERSION_MSG_TEXT_Y									446
#define	NAPPV_TIVU_MENU_SCINFO_VERSION_MSG_TEXT_WIDTH								480
#define	NAPPV_TIVU_MENU_SCINFO_VERSION_MSG_TEXT_HEIGHT								29

//******************************************************************************
//	STATIC VARIABLES
//******************************************************************************
STATIC XmgrCas_NaMenuCardInfo_Tivu_t	s_stApNaPpvMenuCardInfoTivu;

//******************************************************************************
//	PROTOTYPE FUNCTIONS
//******************************************************************************
#define ________XMGR_CAS_NA_PpvTIVU_MENU_STATIC___________________________________________
STATIC inline XmgrCas_NaMenuCardInfo_Tivu_t * xmgr_cas_NaPpvMenuScInfoGetLocalContents_Tivu(void);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoInitContents_Tivu(XmgrCas_NaMenuCardInfo_Tivu_t *pstContents);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoGetStringWidth_Tivu(HUINT8* string, HUINT32 ulFontSize, HUINT32 *ulWidth);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawMsgBackgroundImage_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawMsgPresentDateLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawMsgPresentDate_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawMsgBackgroundImageShortLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawMsgTivuSatImageIcon_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawMsgBackButton_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawSmartCardInfo_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawSmartCardMsgLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawDafaultUi_Tivu(XmgrCas_NaMenuCardInfo_Tivu_t *pstContents);
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDestroyUI_Tivu(XmgrCas_NaMenuCardInfo_Tivu_t *pstContents);

#define ________XMGR_CAS_NA_PpvTIVU_MENU_PUBLIC___________________________________________
BUS_Result_t xmgr_cas_NaPpvMenuSmartCardInfo_Tivu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#define	_________________________STATIC_BODY______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScInfoGetLocalContents_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC inline XmgrCas_NaMenuCardInfo_Tivu_t * xmgr_cas_NaPpvMenuScInfoGetLocalContents_Tivu(void)
{
	return &s_stApNaPpvMenuCardInfoTivu;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScInfoInitContents_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoInitContents_Tivu(XmgrCas_NaMenuCardInfo_Tivu_t *pstContents)
{
	memset(pstContents, 0, sizeof(XmgrCas_NaMenuCardInfo_Tivu_t));

	pstContents->pszCardNumberTitle = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_SCINFO_CARD_NUMBER_TEXT_ID);
	pstContents->pszCaVersionTitle = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_SCINFO_CARD_VERSION_TEXT_ID);
	xmgr_cas_NaGetCardSerialNumber(pstContents->szSmartCardNumber);
	xmgr_cas_NaGetCAVersion(pstContents->szCaVersion);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScInfoGetStringWidth_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoGetStringWidth_Tivu(HUINT8* string, HUINT32 ulFontSize, HUINT32 *ulWidth)
{
	OSD_StringInfo_t		stringInfo = {0, };
	OSD_CheckFontInfo_t 	fontInfo;

	stringInfo.eEncoding = MWC_UTIL_GetEncodingByLocaleCode(string);
	stringInfo.strData = (HINT8*)string;
	stringInfo.strLength = MWC_UTIL_DvbStrlen((const HUINT8 *)stringInfo.strData);

	fontInfo.fontId = eFont_SystemNormal;
	fontInfo.fontSize = (HINT32)ulFontSize;
	fontInfo.fontWidthRatio = 100;	// Normal

	*ulWidth = OSD_FONT_GetStringWidth(&fontInfo, &stringInfo);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScInfoDrawMsgBackgroundImage_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawMsgBackgroundImage_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvMenuScInfoDrawMsgPresentDateLine_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawMsgPresentDateLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvMenuScInfoDrawMsgPresentDate_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawMsgPresentDate_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	OSD_Rect_t				rect_msg;

	UNIXTIME				ulCurTime= 0;
	BUS_Result_t			hRes;
	HxDATETIME_t 			stCurDateTime;

	XmgrCas_NaMenuCardInfo_Tivu_t	*pstContents = xmgr_cas_NaPpvMenuScInfoGetLocalContents_Tivu();

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
		HxLOG_Print("[xmgr_cas_NaPpvMenuScInfoDrawMsgPresentDate_Tivu] ConvertUnixTimeToDateTime fail!!\n");
		return ERR_FAIL;
	}

	sprintf(pstContents->szSmartCardErrorDateStr, "%02d.%02d.%02d\n", stCurDateTime.stDate.ucDay, stCurDateTime.stDate.ucMonth, stCurDateTime.stDate.usYear);
	OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_XS, rect_msg.x, rect_msg.y, rect_msg.w,
								pstContents->szSmartCardErrorDateStr, COL(C_T_Cursor));

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScInfoDrawMsgBackgroundImageShortLine_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawMsgBackgroundImageShortLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvMenuScInfoDrawMsgTivuSatImageIcon_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawMsgTivuSatImageIcon_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e931_1_41_CAS_Logo, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScInfoDrawMsgBackButton_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawMsgBackButton_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvMenuScInfoDrawSmartCardInfo_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawSmartCardInfo_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hErr;
	OSD_Rect_t				rect_msg;
	OSD_Rect_t				rect_image;
	HUINT32					prevStringWidth = 0;
	HxLANG_Id_e			eMenuLang = eLangID_English;

	/* for compiler */
	(void)pArg;
	(void)param1;

	rect_msg.x = pArea->x1 + pRect->x;
	rect_msg.y = pArea->x1 + pRect->y;
	rect_msg.w = pRect->w;
	rect_msg.h = pRect->h;


//	eMenuLang = MWC_PARAM_GetMainLanguage();
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN, (HUINT32 *)&eMenuLang, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_LANG_MAIN) Error!!\n");
	}
	if((eMenuLang == eLangID_Italian))
	{
		/* 1.Draw Text */
		OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_L, rect_msg.x, rect_msg.y, rect_msg.w,
										GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_SCINFO_CARD_INFORMATION_TEXT_ID), COL(C_T_Cursor));

		/* 2.Draw Tivu Icon*/
		xmgr_cas_NaPpvMenuScInfoGetStringWidth_Tivu(GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_SCINFO_CARD_INFORMATION_TEXT_ID), FONT_SIZE_L, &prevStringWidth);
		rect_msg.x += (8 + prevStringWidth);
		rect_image.x = rect_msg.x;
		rect_image.y = rect_msg.y - (FONT_SIZE_L+4);
		rect_image.w = rect_msg.w;
		rect_image.h = rect_msg.h;
		GWM_Frame_Create (NAPPV_TIVU_MENU_SCINFO_MSG_TIVUSAT_ICON_ID, &rect_image, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCINFO_MSG_TIVUSAT_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScInfoDrawMsgTivuSatImageIcon_Tivu);
	}
	else
	{
		/* 1.Draw Tivu Icon*/
		rect_image.x = rect_msg.x;
		rect_image.y = rect_msg.y - (FONT_SIZE_L+4);
		rect_image.w = rect_msg.w;
		rect_image.h = rect_msg.h;
		GWM_Frame_Create (NAPPV_TIVU_MENU_SCINFO_MSG_TIVUSAT_ICON_ID, &rect_image, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCINFO_MSG_TIVUSAT_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScInfoDrawMsgTivuSatImageIcon_Tivu);

		/* 2.Draw Text */
		rect_msg.x += (8 + 125);
		OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_L, rect_msg.x, rect_msg.y, rect_msg.w,
										GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_SCINFO_CARD_INFORMATION_TEXT_ID), COL(C_T_Cursor));

	}


	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScInfoDrawSmartCardMsgLine_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawSmartCardMsgLine_Tivu(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e931_1_41_CAS_Boxline, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScInfoDrawDafaultUi_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDrawDafaultUi_Tivu(XmgrCas_NaMenuCardInfo_Tivu_t *pstContents)
{
	OSD_Rect_t				rect = {0, };

	/* for compiler */
	(void)pstContents;

	/* na ppv tivu scinfo background */
	Settings_DrawBack(FALSE);

	/* na ppv tivu scinfo card error background*/
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_X, NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_Y, NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_WIDTH, NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_HEIGHT);
	GWM_Frame_Create (NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScInfoDrawMsgBackgroundImage_Tivu);

	/* Draw Panel Line*/
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEUP_X, NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEUP_Y, NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEUP_WIDTH, NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEUP_HEIGHT);
	GWM_Frame_Create (NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEUP_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCINFO_MSG_BACKGROUND_NORMAL_LINEUP_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScInfoDrawMsgBackgroundImageShortLine_Tivu);

	/* na ppv tivu date text */
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCINFO_MSG_DATE_TITLE_TEXT_X, NAPPV_TIVU_MENU_SCINFO_MSG_DATE_TITLE_TEXT_Y - FONT_SIZE_XXS, NAPPV_TIVU_MENU_SCINFO_MSG_DATE_TITLE_TEXT_WIDTH, NAPPV_TIVU_MENU_SCINFO_MSG_DATE_TITLE_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCINFO_MSG_DATE_TITLE_TEXT_ID, &rect, GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_SCINFO_DATE_TEXT_ID));
	GWM_Text_SetAlign (NAPPV_TIVU_MENU_SCINFO_MSG_DATE_TITLE_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_TIVU_MENU_SCINFO_MSG_DATE_TITLE_TEXT_ID, FONT_SIZE_XXS);
	GWM_Text_ChangeColor (NAPPV_TIVU_MENU_SCINFO_MSG_DATE_TITLE_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv tivu date line */
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCINFO_MSG_DATE_LINE_X, NAPPV_TIVU_MENU_SCINFO_MSG_DATE_LINE_Y, NAPPV_TIVU_MENU_SCINFO_MSG_DATE_LINE_WIDTH, NAPPV_TIVU_MENU_SCINFO_MSG_DATE_LINE_HEIGHT);
	GWM_Frame_Create (NAPPV_TIVU_MENU_SCINFO_MSG_DATE_LINE_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCINFO_MSG_DATE_LINE_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScInfoDrawMsgPresentDateLine_Tivu);

	/* na ppv tivu present date text */
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCINFO_MSG_PRESENT_DATE_TEXT_X, NAPPV_TIVU_MENU_SCINFO_MSG_PRESENT_DATE_TEXT_Y, NAPPV_TIVU_MENU_SCINFO_MSG_PRESENT_DATE_TEXT_WIDTH, NAPPV_TIVU_MENU_SCINFO_MSG_PRESENT_DATE_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCINFO_MSG_PRESENT_DATE_TEXT_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCINFO_MSG_PRESENT_DATE_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScInfoDrawMsgPresentDate_Tivu);

	/* na ppv tivu info text */
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCINFO_MSG_INFO_TEXT_X, NAPPV_TIVU_MENU_SCINFO_MSG_INFO_TEXT_Y, NAPPV_TIVU_MENU_SCINFO_MSG_INFO_TEXT_WIDTH, NAPPV_TIVU_MENU_SCINFO_MSG_INFO_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCINFO_MSG_INFO_TEXT_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCINFO_MSG_INFO_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScInfoDrawSmartCardInfo_Tivu);

	/* na ppv tivu back button*/
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCINFO_MSG_BACK_BUTTON_X, NAPPV_TIVU_MENU_SCINFO_MSG_BACK_BUTTON_Y, NAPPV_TIVU_MENU_SCINFO_MSG_BACK_BUTTON_WIDTH, NAPPV_TIVU_MENU_SCINFO_MSG_BACK_BUTTON_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCINFO_MSG_BACK_BUTTON_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCINFO_MSG_BACK_BUTTON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScInfoDrawMsgBackButton_Tivu);

	/* na ppv tivu return menu text*/
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCINFO_MSG_BACK_TEXT_X, NAPPV_TIVU_MENU_SCINFO_MSG_BACK_TEXT_Y- FONT_SIZE_XXS, NAPPV_TIVU_MENU_SCINFO_MSG_BACK_TEXT_WIDTH, NAPPV_TIVU_MENU_SCINFO_MSG_BACK_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCINFO_MSG_BACK_TEXT_ID, &rect, GetStrRsc(LOC_CAS_NAGRA_TIVU_MENU_SCINFO_RETURN_TO_MENU_TEXT_ID));
	GWM_Text_SetAlign (NAPPV_TIVU_MENU_SCINFO_MSG_BACK_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_TIVU_MENU_SCINFO_MSG_BACK_TEXT_ID, FONT_SIZE_XXS);
	GWM_Text_ChangeColor (NAPPV_TIVU_MENU_SCINFO_MSG_BACK_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv tivu info number text */
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_TEXT_X, NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_TEXT_Y - FONT_SIZE_M, NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_TEXT_WIDTH, NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_TEXT_ID, &rect, pstContents->pszCardNumberTitle);
	GWM_Text_SetAlign (NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_TEXT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor (NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_TEXT_ID, COL(C_T_Cursor), 0x00);

	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_LINE_X, NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_LINE_Y, NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_LINE_WIDTH, NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_LINE_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_LINE_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCINFO_NUMBER_TITLE_LINE_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScInfoDrawSmartCardMsgLine_Tivu);

	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCINFO_NUMBER_MSG_TEXT_X, NAPPV_TIVU_MENU_SCINFO_NUMBER_MSG_TEXT_Y - FONT_SIZE_M, NAPPV_TIVU_MENU_SCINFO_NUMBER_MSG_TEXT_WIDTH, NAPPV_TIVU_MENU_SCINFO_NUMBER_MSG_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCINFO_NUMBER_MSG_TEXT_ID, &rect, pstContents->szSmartCardNumber);
	GWM_Text_SetAlign (NAPPV_TIVU_MENU_SCINFO_NUMBER_MSG_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_TIVU_MENU_SCINFO_NUMBER_MSG_TEXT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor (NAPPV_TIVU_MENU_SCINFO_NUMBER_MSG_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv tivu info version text */
	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_TEXT_X, NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_TEXT_Y - FONT_SIZE_M, NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_TEXT_WIDTH, NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_TEXT_ID, &rect, pstContents->pszCaVersionTitle);
	GWM_Text_SetAlign (NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_TEXT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor (NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_TEXT_ID, COL(C_T_Cursor), 0x00);

	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_LINE_X, NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_LINE_Y, NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_LINE_WIDTH, NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_LINE_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_LINE_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_TIVU_MENU_SCINFO_VERSION_TITLE_LINE_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvMenuScInfoDrawSmartCardMsgLine_Tivu);

	OSD_GFX_SetRect (&rect, NAPPV_TIVU_MENU_SCINFO_VERSION_MSG_TEXT_X, NAPPV_TIVU_MENU_SCINFO_VERSION_MSG_TEXT_Y - FONT_SIZE_M, NAPPV_TIVU_MENU_SCINFO_VERSION_MSG_TEXT_WIDTH, NAPPV_TIVU_MENU_SCINFO_VERSION_MSG_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_TIVU_MENU_SCINFO_VERSION_MSG_TEXT_ID, &rect, pstContents->szCaVersion);
	GWM_Text_SetAlign (NAPPV_TIVU_MENU_SCINFO_VERSION_MSG_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_TIVU_MENU_SCINFO_VERSION_MSG_TEXT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor (NAPPV_TIVU_MENU_SCINFO_VERSION_MSG_TEXT_ID, COL(C_T_Cursor), 0x00);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvMenuScInfoDestroyUI_Tivu
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvMenuScInfoDestroyUI_Tivu(XmgrCas_NaMenuCardInfo_Tivu_t *pstContents)
{
	/* for compiler */
	(void)pstContents;

	return ERR_BUS_NO_ERROR;
}


#define	_________________________PUBLIC_BODY______________________________
/******************************************************************************/
/**
*   \fn     xmgr_cas_NaPpvMenuSmartCardInfo_Tivu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xmgr_cas_NaPpvMenuSmartCardInfo_Tivu(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaMenuCardInfo_Tivu_t	*pstContents = xmgr_cas_NaPpvMenuScInfoGetLocalContents_Tivu();
	BUS_Result_t				ulResult = ERR_BUS_NO_ERROR;

	switch(message)
	{
		case MSG_GWM_CREATE:
			HxLOG_Info("MSG_GWM_CREATE!\n");

			/* set app area */
			GWM_APP_SetAppArea(NAPPV_TIVU_MENU_SCINFO_WINDOW_X, NAPPV_TIVU_MENU_SCINFO_WINDOW_Y,
								NAPPV_TIVU_MENU_SCINFO_WINDOW_WIDTH, NAPPV_TIVU_MENU_SCINFO_WINDOW_HEIGHT);

			/* Init the contents */
			ulResult = xmgr_cas_NaPpvMenuScInfoInitContents_Tivu(pstContents);
			if (ulResult != ERR_BUS_NO_ERROR)
			{
				break;
			}
			if(ulResult != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

			/* draw UI object */
			xmgr_cas_NaPpvMenuScInfoDrawDafaultUi_Tivu(pstContents);

			break;

		case MSG_GWM_FAIL_RECREATE_NEW_INSTANCE:
			break;

		case MSG_APP_UPDATE_SMARTCARD_INFO:
			if(p1 == eDxCAS_GROUPID_NA)
			{
				XmgrCas_NaAlamType_e	eAlarmType;

				xmgr_cas_NaGetSmartCardStatus(&eAlarmType);
				HxLOG_Print("MSG_APP_UPDATE_SMARTCARD_INFO... eAlarmType = %d\n", eAlarmType);
				if (eAlarmType != eCAS_NA_CA_SMARTCARD_OK && eAlarmType != eCAS_NA_CA_SMARTCARD_EXPIRED)
				{
					BUS_MGR_Destroy(NULL);

					/* to upper depth */
					BUS_MGR_Create("xproc_cas_NaPpvMenuSmartCardError_Tivu", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (GWM_Callback_t)xproc_cas_NaPpvMenuSmartCardError_Tivu, 0, eAlarmType, 0, 0);

					return MESSAGE_PASS;
				}
				else
				{
					// Card 정상 삽입 상태인데.. 이쪽으로 왜 올까?
				}
			}
			break;

		case eSEVT_CAS_SC_INSERTED:
			break;
		case eSEVT_CAS_SC_REMOVED:
#if 0
			if (p2 == eDxCAS_GROUPID_NA)
			{
				BUS_MGR_Destroy(NULL);
				BUS_MGR_Create("xproc_cas_NaPpvMenuSmartCardError_Tivu",
								APP_CAS_PRIORITY,
								xproc_cas_NaPpvMenuSmartCardError_Tivu,
								SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
								1,
								eCAS_NA_CA_SMARTCARD_REMOVED,
								0);
			}
#endif
			break;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_KEYUP :
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN :
			AP_KeyConv_UiCommon(&p1);

			ulResult = Settings_CheckKeyPass(p1, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
			if(ulResult != ERR_OK)
			{
				return ulResult;
			}

			switch(p1)
			{

				case KEY_EXIT:
				case KEY_MENU:
					Settings_ClearWindowTitle();

					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				case KEY_BACK:
					BUS_MGR_Destroy(NULL);

					/* to upper depth */
					BUS_MGR_Create("xproc_cas_NaPpvMenuSmartCardError_Tivu",
							APP_CAS_PRIORITY,
							xproc_cas_NaPpvMenuSmartCardError_Tivu,
							SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
							1,
							eCAS_NA_CA_SMARTCARD_OK,
							0);
					//BUS_MGR_Create("AP_CAS_PpvMenu_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (GWM_Callback_t)AP_CAS_PpvMenu_Proc, 0, 0, 0, 0);
					return MESSAGE_BREAK;

				case KEY_OK:
					return MESSAGE_BREAK;

			}
			break;


		case MSG_GWM_TIMER:
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			xmgr_cas_NaPpvMenuScInfoDestroyUI_Tivu(pstContents);

			break;

		default:
			break;
	}

	HxLOG_Info("xmgr_cas_NaPpvMenuSmartCardInfo_Tivu handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif	//end of #if defined(CONFIG_MW_CAS_NAGRA_OP_TIVU)
/* End of File */
