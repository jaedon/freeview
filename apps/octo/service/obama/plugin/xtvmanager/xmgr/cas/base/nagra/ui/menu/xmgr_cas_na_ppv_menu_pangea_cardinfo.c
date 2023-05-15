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

#include "_xmgr_cas_na_smartcard_info_coordinate.h"
#include <ap_util_osd.h>

#include <namgr_main.h>

#include "../../local_include/_xmgr_cas_na_util.h"
#include "../../local_include/_xmgr_cas_na_adapt.h"
#include "../../local_include/_xmgr_cas_na.h"


#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
//******************************************************************************
//	DEFINE
//******************************************************************************




typedef enum
{
	eCasNaPgeaScInfo_Number,
	eCasNaPgeaScInfo_ExpiryDate,
	eCasNaPgeaScInfo_Credit,
	eCasNaPgeaScInfo_CaVer,
	eCasNaPgeaScInfo_Item_Max
} XmgrCas_NaScInfoItem_e;

typedef struct tag_CAS_NA_PANGEA_SC_Contents
{
	/**@breif Public value */
	HUINT32					ulCurFocus;
	HUINT32					ulNumOfSubItem, ulMaxNumOfSubItem;
	SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HINT8					ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	HUINT8					*pszCardNumberTitle;
	HUINT8					*pszCardExpiryTitle;
	HUINT8					*pszCardRemainCreditTitle;
	HUINT8					*pszCaVersionTitle;
	HUINT8					*pszBtnBackHelp;
	HUINT8					*pszExpiredCard;

	HINT8 					szSmartCardNumber[32];
	HINT8					szSmartCardExpiryDate[16];
	HINT8					szRemainedCreditValue[16];
	HINT8					szCaVersion[32];
	HINT8					szCurrentDateStr[32];					///< title of app context
} XmgrCas_NaCardInfo_Pangea_t;

#define	NAPPV_PANGEA_SCINFO_LIST_ITEM_NUM						4

#define	NAPPV_PANGEA_SCINFO_WINDOW_WIDTH						OSD_SCREEN_WIDTH
#define	NAPPV_PANGEA_SCINFO_WINDOW_HEIGHT						OSD_SCREEN_HEIGHT
#define	NAPPV_PANGEA_SCINFO_WINDOW_X							0
#define	NAPPV_PANGEA_SCINFO_WINDOW_Y							0
#define	NAPPV_PANGEA_SCINFO_STRING_MARGIN_Y						18

#define	NAPPV_PANGEA_SCINFO_APP_ID								eCasNaPgeaScInfo_Number

/* ppv pangea scinfo background */
#define	NAPPV_PANGEA_SCINFO_LOGOBACKGROUND_IMAGE_ID				(NAPPV_PANGEA_SCINFO_APP_ID|0x0010)

/* ppv pangea scinfo date */
#define	NAPPV_PANGEA_SCINFO_DATE_TEXT_ID						(NAPPV_PANGEA_SCINFO_APP_ID|0x0030)
#define	NAPPV_PANGEA_SCINFO_DATE_TEXT_X							780
#define	NAPPV_PANGEA_SCINFO_DATE_TEXT_Y							(196-FONT_SIZE_XS)
#define	NAPPV_PANGEA_SCINFO_DATE_TEXT_WIDTH						300
#define	NAPPV_PANGEA_SCINFO_DATE_TEXT_HEIGHT					31

/* ppv pangea scinfo info text */
#define	NAPPV_PANGEA_SCINFO_INFO_TEXT_ID						(NAPPV_PANGEA_SCINFO_APP_ID|0x0040)
#define	NAPPV_PANGEA_SCINFO_INFO_TEXT_X							310
#define	NAPPV_PANGEA_SCINFO_INFO_TEXT_Y							(196 - FONT_SIZE_S)
#define	NAPPV_PANGEA_SCINFO_INFO_TEXT_WIDTH						500
#define	NAPPV_PANGEA_SCINFO_INFO_TEXT_HEIGHT					31

/* ppv pangea scinfo state button */
#define	NAPPV_PANGEA_SCINFO_STATE_BTN_ID						(NAPPV_PANGEA_SCINFO_APP_ID|0x0050)
#define	NAPPV_PANGEA_SCINFO_STATE_BTN_X							636
#define	NAPPV_PANGEA_SCINFO_STATE_BTN_Y							497
#define	NAPPV_PANGEA_SCINFO_STATE_BTN_WIDTH						644
#define	NAPPV_PANGEA_SCINFO_STATE_BTN_HEIGHT					66

/* ppv pangea scinfo state text */
#define	NAPPV_PANGEA_SCINFO_STATE_TEXT_ID						(NAPPV_PANGEA_SCINFO_APP_ID|0x0060)
#define	NAPPV_PANGEA_SCINFO_STATE_TEXT_X						710
#define	NAPPV_PANGEA_SCINFO_STATE_TEXT_Y						516
#define	NAPPV_PANGEA_SCINFO_STATE_TEXT_WIDTH					500
#define	NAPPV_PANGEA_SCINFO_STATE_TEXT_HEIGHT					31

/* ppv pangea scinfo back button */
#define	NAPPV_PANGEA_SCINFO_BACK_BTN_ID							(NAPPV_PANGEA_SCINFO_APP_ID|0x0070)
#define	NAPPV_PANGEA_SCINFO_BACK_BTN_X							450
#define	NAPPV_PANGEA_SCINFO_BACK_BTN_Y							607
#define	NAPPV_PANGEA_SCINFO_BACK_BTN_WIDTH						86
#define	NAPPV_PANGEA_SCINFO_BACK_BTN_HEIGHT						44

#define	NAPPV_PANGEA_SCINFO_BACK_TEXT_ID						(NAPPV_PANGEA_SCINFO_APP_ID|0x0080)
#define	NAPPV_PANGEA_SCINFO_BACK_TEXT_X							543
#define	NAPPV_PANGEA_SCINFO_BACK_TEXT_Y							(636 - FONT_SIZE_XS)
#define	NAPPV_PANGEA_SCINFO_BACK_TEXT_WIDTH						200
#define	NAPPV_PANGEA_SCINFO_BACK_TEXT_HEIGHT					44

#define	NAPPV_PANGEA_SCINFO_STATUS_ID							(NAPPV_PANGEA_SCINFO_APP_ID|0x0090)

#define __________MAINITEM_OBJECTS_________________________________________
#define	NAPPV_PANGEA_SCINFO_MAINBTN_WIDTH						230
#define	NAPPV_PANGEA_SCINFO_MAINBTN_HEIGHT						37
#define	NAPPV_PANGEA_SCINFO_MAINBTN_LINESPACE					55

#define	NAPPV_PANGEA_SCINFO_MAINBTN_LINE_WIDTH					227
#define	NAPPV_PANGEA_SCINFO_MAINBTN_LINE_HEIGHT					3
#define	NAPPV_PANGEA_SCINFO_MAINBTN_LINE_LINESPACE				55

/* ppv pangea scinfo btn 01 position */
#define	NAPPV_PANGEA_SCINFO_MAINBTN01_CARD_NUMBER_ID			(NAPPV_PANGEA_SCINFO_APP_ID|0x0100)
#define	NAPPV_PANGEA_SCINFO_MAINBTN01_WIDTH						NAPPV_PANGEA_SCINFO_MAINBTN_WIDTH
#define	NAPPV_PANGEA_SCINFO_MAINBTN01_HEIGHT					NAPPV_PANGEA_SCINFO_MAINBTN_HEIGHT
#define	NAPPV_PANGEA_SCINFO_MAINBTN01_X							141
#define	NAPPV_PANGEA_SCINFO_MAINBTN01_Y							(295-FONT_SIZE_M)

#define	NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_ID					(NAPPV_PANGEA_SCINFO_APP_ID|0x0110)
#define	NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_WIDTH				NAPPV_PANGEA_SCINFO_MAINBTN_LINE_WIDTH
#define	NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_HEIGHT				NAPPV_PANGEA_SCINFO_MAINBTN_LINE_HEIGHT
#define	NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_X					381
#define	NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_Y					292

/* ppv pangea scinfo btn 02 position */
#define	NAPPV_PANGEA_SCINFO_MAINBTN02_REMAIN_CREDIT_ID			(NAPPV_PANGEA_SCINFO_APP_ID|0x0120)
#define	NAPPV_PANGEA_SCINFO_MAINBTN02_WIDTH						NAPPV_PANGEA_SCINFO_MAINBTN_WIDTH
#define	NAPPV_PANGEA_SCINFO_MAINBTN02_HEIGHT					NAPPV_PANGEA_SCINFO_MAINBTN_HEIGHT
#define	NAPPV_PANGEA_SCINFO_MAINBTN02_X							NAPPV_PANGEA_SCINFO_MAINBTN01_X
#define	NAPPV_PANGEA_SCINFO_MAINBTN02_Y							(NAPPV_PANGEA_SCINFO_MAINBTN01_Y+NAPPV_PANGEA_SCINFO_MAINBTN_LINESPACE)

#define	NAPPV_PANGEA_SCINFO_MAINBTN02_LINE_ID					(NAPPV_PANGEA_SCINFO_APP_ID|0x0130)
#define	NAPPV_PANGEA_SCINFO_MAINBTN02_LINE_WIDTH				NAPPV_PANGEA_SCINFO_MAINBTN_LINE_WIDTH
#define	NAPPV_PANGEA_SCINFO_MAINBTN02_LINE_HEIGHT				NAPPV_PANGEA_SCINFO_MAINBTN_LINE_HEIGHT
#define	NAPPV_PANGEA_SCINFO_MAINBTN02_LINE_X					NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_X
#define	NAPPV_PANGEA_SCINFO_MAINBTN02_LINE_Y					(NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_Y+NAPPV_PANGEA_SCINFO_MAINBTN_LINE_LINESPACE)

/* ppv pangea scinfo btn 03 position */
#define	NAPPV_PANGEA_SCINFO_MAINBTN03_EXPIRY_DATE_ID			(NAPPV_PANGEA_SCINFO_APP_ID|0x0140)
#define	NAPPV_PANGEA_SCINFO_MAINBTN03_WIDTH						NAPPV_PANGEA_SCINFO_MAINBTN_WIDTH
#define	NAPPV_PANGEA_SCINFO_MAINBTN03_HEIGHT					NAPPV_PANGEA_SCINFO_MAINBTN_HEIGHT
#define	NAPPV_PANGEA_SCINFO_MAINBTN03_X							NAPPV_PANGEA_SCINFO_MAINBTN01_X
#define	NAPPV_PANGEA_SCINFO_MAINBTN03_Y							(NAPPV_PANGEA_SCINFO_MAINBTN02_Y+NAPPV_PANGEA_SCINFO_MAINBTN_LINESPACE)

#define	NAPPV_PANGEA_SCINFO_MAINBTN03_LINE_ID					(NAPPV_PANGEA_SCINFO_APP_ID|0x0150)
#define	NAPPV_PANGEA_SCINFO_MAINBTN03_LINE_WIDTH				NAPPV_PANGEA_SCINFO_MAINBTN_LINE_WIDTH
#define	NAPPV_PANGEA_SCINFO_MAINBTN03_LINE_HEIGHT				NAPPV_PANGEA_SCINFO_MAINBTN_LINE_HEIGHT
#define	NAPPV_PANGEA_SCINFO_MAINBTN03_LINE_X					NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_X
#define	NAPPV_PANGEA_SCINFO_MAINBTN03_LINE_Y					(NAPPV_PANGEA_SCINFO_MAINBTN02_LINE_Y+NAPPV_PANGEA_SCINFO_MAINBTN_LINE_LINESPACE)

/* ppv pangea scinfo btn 04 position */
#define	NAPPV_PANGEA_SCINFO_MAINBTN04_CA_VERSION_ID				(NAPPV_PANGEA_SCINFO_APP_ID|0x0160)
#define	NAPPV_PANGEA_SCINFO_MAINBTN04_WIDTH						NAPPV_PANGEA_SCINFO_MAINBTN_WIDTH
#define	NAPPV_PANGEA_SCINFO_MAINBTN04_HEIGHT					NAPPV_PANGEA_SCINFO_MAINBTN_HEIGHT
#define	NAPPV_PANGEA_SCINFO_MAINBTN04_X							NAPPV_PANGEA_SCINFO_MAINBTN01_X
#define	NAPPV_PANGEA_SCINFO_MAINBTN04_Y							(NAPPV_PANGEA_SCINFO_MAINBTN03_Y+NAPPV_PANGEA_SCINFO_MAINBTN_LINESPACE)

#define	NAPPV_PANGEA_SCINFO_MAINBTN04_LINE_ID					(NAPPV_PANGEA_SCINFO_APP_ID|0x0170)
#define	NAPPV_PANGEA_SCINFO_MAINBTN04_LINE_WIDTH				NAPPV_PANGEA_SCINFO_MAINBTN_LINE_WIDTH
#define	NAPPV_PANGEA_SCINFO_MAINBTN04_LINE_HEIGHT				NAPPV_PANGEA_SCINFO_MAINBTN_LINE_HEIGHT
#define	NAPPV_PANGEA_SCINFO_MAINBTN04_LINE_X					NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_X
#define	NAPPV_PANGEA_SCINFO_MAINBTN04_LINE_Y					(NAPPV_PANGEA_SCINFO_MAINBTN03_LINE_Y+NAPPV_PANGEA_SCINFO_MAINBTN_LINE_LINESPACE)

#define __________SUBITEM_OBJECTS_________________________________________
#define	NAPPV_PANGEA_SCINFO_SUBBTN_WIDTH						350
#define	NAPPV_PANGEA_SCINFO_SUBBTN_HEIGHT						37
#define	NAPPV_PANGEA_SCINFO_SUBBTN_LINESPACE					55

/* ppv pangea scinfo btn 01 position */
#define	NAPPV_PANGEA_SCINFO_SUBBTN01_CARD_NUMBER_ID				(NAPPV_PANGEA_SCINFO_APP_ID|0x0200)
#define	NAPPV_PANGEA_SCINFO_SUBBTN01_WIDTH						NAPPV_PANGEA_SCINFO_SUBBTN_WIDTH
#define	NAPPV_PANGEA_SCINFO_SUBBTN01_HEIGHT						NAPPV_PANGEA_SCINFO_SUBBTN_HEIGHT
#define	NAPPV_PANGEA_SCINFO_SUBBTN01_X							638
#define	NAPPV_PANGEA_SCINFO_SUBBTN01_Y							(295-FONT_SIZE_M)

/* ppv pangea scinfo btn 02 position */
#define	NAPPV_PANGEA_SCINFO_SUBBTN02_REMAIN_CREDIT_ID			(NAPPV_PANGEA_SCINFO_APP_ID|0x0210)
#define	NAPPV_PANGEA_SCINFO_SUBBTN02_WIDTH						NAPPV_PANGEA_SCINFO_SUBBTN_WIDTH
#define	NAPPV_PANGEA_SCINFO_SUBBTN02_HEIGHT						NAPPV_PANGEA_SCINFO_SUBBTN_HEIGHT
#define	NAPPV_PANGEA_SCINFO_SUBBTN02_X							NAPPV_PANGEA_SCINFO_SUBBTN01_X
#define	NAPPV_PANGEA_SCINFO_SUBBTN02_Y							(NAPPV_PANGEA_SCINFO_SUBBTN01_Y+NAPPV_PANGEA_SCINFO_SUBBTN_LINESPACE)

#define	NAPPV_PANGEA_SCINFO_SUBBTN02_REMAIN_CREDIT_EURO_ID		(NAPPV_PANGEA_SCINFO_APP_ID|0x0220)

/* ppv pangea scinfo btn 03 position */
#define	NAPPV_PANGEA_SCINFO_SUBBTN03_EXPIRY_DATE_ID				(NAPPV_PANGEA_SCINFO_APP_ID|0x0230)
#define	NAPPV_PANGEA_SCINFO_SUBBTN03_WIDTH						NAPPV_PANGEA_SCINFO_SUBBTN_WIDTH
#define	NAPPV_PANGEA_SCINFO_SUBBTN03_HEIGHT						NAPPV_PANGEA_SCINFO_SUBBTN_HEIGHT
#define	NAPPV_PANGEA_SCINFO_SUBBTN03_X							NAPPV_PANGEA_SCINFO_SUBBTN01_X
#define	NAPPV_PANGEA_SCINFO_SUBBTN03_Y							(NAPPV_PANGEA_SCINFO_SUBBTN02_Y+NAPPV_PANGEA_SCINFO_SUBBTN_LINESPACE)

/* ppv pangea scinfo btn 04 position */
#define	NAPPV_PANGEA_SCINFO_SUBBTN04_CA_VERSION_ID				(NAPPV_PANGEA_SCINFO_APP_ID|0x0240)
#define	NAPPV_PANGEA_SCINFO_SUBBTN04_WIDTH						NAPPV_PANGEA_SCINFO_SUBBTN_WIDTH
#define	NAPPV_PANGEA_SCINFO_SUBBTN04_HEIGHT						NAPPV_PANGEA_SCINFO_SUBBTN_HEIGHT
#define	NAPPV_PANGEA_SCINFO_SUBBTN04_X							NAPPV_PANGEA_SCINFO_SUBBTN01_X
#define	NAPPV_PANGEA_SCINFO_SUBBTN04_Y							(NAPPV_PANGEA_SCINFO_SUBBTN03_Y+NAPPV_PANGEA_SCINFO_SUBBTN_LINESPACE)

//******************************************************************************
//	STATIC VARIABLES
//******************************************************************************
STATIC XmgrCas_NaCardInfo_Pangea_t 			s_stNaPgeaCardInfo;


//******************************************************************************
//	PROTOTYPE FUNCTIONS
//******************************************************************************
#define ________XMGR_CAS_NA_PpvPANGEA_MENU_STATIC___________________________________________
STATIC XmgrCas_NaCardInfo_Pangea_t* xmgr_cas_NaPpvScInfoGetLocalContents(void);
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoUpdateRemainedCredit_Pangea(XmgrCas_NaCardInfo_Pangea_t *pContents, TCreditAmount nCredit);
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoSetMainTitle_Pangea(XmgrCas_NaCardInfo_Pangea_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoDrawMainTextLine_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoDrawBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoDrawMenuBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoDrawStateButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoDrawBackButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoDrawEuroIcon_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoSetUIObjects_Pangea(XmgrCas_NaCardInfo_Pangea_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoDestroyUI_Pangea(XmgrCas_NaCardInfo_Pangea_t *pContents);

#define ________XMGR_CAS_NA_PpvPANGEA_MENU_PUBLIC___________________________________________
BUS_Result_t xproc_cas_NaScInfo_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#define	_________________________STATIC_BODY______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvScInfoGetLocalContents
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaCardInfo_Pangea_t* xmgr_cas_NaPpvScInfoGetLocalContents(void)
{
	return &s_stNaPgeaCardInfo;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScInfoUpdateRemainedCredit_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoUpdateRemainedCredit_Pangea(XmgrCas_NaCardInfo_Pangea_t *pContents, TCreditAmount nCredit)
{
	OSD_Rect_t	rect;
	OSD_Font_t 	*pFont = OSD_FONT_Get(eFont_SystemNormal);

	snprintf(pContents->szRemainedCreditValue, 16, "%d,%02d", nCredit/100, nCredit%100);
	GWM_Text_SetText(NAPPV_PANGEA_SCINFO_SUBBTN02_REMAIN_CREDIT_ID, pContents->szRemainedCreditValue);

	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCINFO_SUBBTN02_X, NAPPV_PANGEA_SCINFO_SUBBTN02_Y, NAPPV_PANGEA_SCINFO_SUBBTN02_WIDTH, NAPPV_PANGEA_SCINFO_SUBBTN02_HEIGHT);
	rect.x += (OSD_GFX_GetStringWidth(pFont, (HINT8 *)pContents->szRemainedCreditValue)+4);
	rect.y += 5;

	if(pFont != NULL)
	{
		OSD_FONT_Release(pFont);
	}

	if (GWM_Obj_GetObjectByID(NAPPV_PANGEA_SCINFO_SUBBTN02_REMAIN_CREDIT_EURO_ID))
	{
		GWM_Obj_Destroy(NAPPV_PANGEA_SCINFO_SUBBTN02_REMAIN_CREDIT_EURO_ID);
	}
	GWM_Frame_Create(NAPPV_PANGEA_SCINFO_SUBBTN02_REMAIN_CREDIT_EURO_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_SCINFO_SUBBTN02_REMAIN_CREDIT_EURO_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvScInfoDrawEuroIcon_Pangea);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScInfoSetMainTitle_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoSetMainTitle_Pangea(XmgrCas_NaCardInfo_Pangea_t *pContents)
{
	HUINT8 					szDate[32];
	SettingsLeafSubItem_t	*pSubItem = NULL;
	HBOOL					bExpired = FALSE;

	HxLOG_Print("+++++++++++++++\n");

	/* set, draw background */
	Settings_DrawBack(FALSE);

	/* title */
	MWC_UTIL_DvbStrcpy((HUINT8*)pContents->ucTitle, GetStrRsc(LOC_CAS_NAGRA_PANGEA_CARD_INFORMATION_TEXT_ID));
	Settings_SetWindowTitle(SETTING_GET_AP_DEPTH(eSetting_NagraPpvPangeaSmartCard), LOC_CAS_NAGRA_PANGEA_CARD_INFORMATION_TEXT_ID, (HUINT8*)pContents->ucTitle);

	pContents->pszCardNumberTitle = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_CARD_NUMBER_TEXT_ID);
	pContents->pszCardExpiryTitle = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_EXPIRYDATE_TEXT_ID);
	pContents->pszCardRemainCreditTitle = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_CREDIT_TEXT_ID);
	pContents->pszCaVersionTitle = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_CARD_VERSION_TEXT_ID);
	pContents->pszBtnBackHelp = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BACK_MENU_TEXT_ID);
	pContents->pszExpiredCard = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_MENU_ERR_EXPIRED_CARD_ID);	// CARD EXPIRED일 경우에만 표시

	xmgr_cas_NaGetCurrentDateStr(szDate);
	sprintf(pContents->szCurrentDateStr, "%s: %s", (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_DATE_TEXT_ID), szDate);
	xmgr_cas_NaGetCardSerialNumber((HUINT8*)pContents->szSmartCardNumber);
	xmgr_cas_NaGetScExpireDateStr((HUINT8*)pContents->szSmartCardExpiryDate, &bExpired);
	xmgr_cas_NaGetScCreditStr((HUINT8*)pContents->szRemainedCreditValue);
	xmgr_cas_NaGetCAVersion((HUINT8*)pContents->szCaVersion);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScInfoDrawMainTextLine_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoDrawMainTextLine_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e923_2_24_Line_Dot, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScInfoDrawBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoDrawBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HUINT32					x1, y1;
	OSD_StockImage_t		img;

	/* for compiler */
	NOT_USED_PARAM(pObject);
	NOT_USED_PARAM(pArg);
	NOT_USED_PARAM(param1);

	/* absolute coordinate */
	x1=pArea->x1+pRect->x;
	y1=pArea->y1+pRect->y;

	if (AP_StockImg_GetImage(e923_1_24_1P_1Logo_BG, &img) == ERR_OK)
	{
		OSD_GFX_FB_Copy((unsigned long)img.hBuffer, img.width, img.height, pScreen, pRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScInfoDrawMenuBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoDrawMenuBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e912_1_24_M_2Menu, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScInfoDrawStateButton_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoDrawStateButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e923_2_24_Info_Bar, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScInfoDrawBackButton_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoDrawBackButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e921_1_24_BTN_2Back, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScInfoDrawEuroIcon_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoDrawEuroIcon_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e911_4_24_Euro_Font_Black_B, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScInfoSetUIObjects_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoSetUIObjects_Pangea(XmgrCas_NaCardInfo_Pangea_t *pContents)
{
	OSD_Rect_t				rect = {0, };
	XmgrCas_NaAlamType_e		eAlarmType = eCAS_NA_CA_SMARTCARD_OK;
	OSD_Font_t				*pFont = NULL;

	/* for compiler */
	(void)pContents;

	HxLOG_Print(" +++ \n");

	/* na ppv pangea scinfo background */
	Settings_DrawBack(FALSE);
	OSD_GFX_SetRect(&rect, 0, 0, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT);
	GWM_Rect_Create(NAPPV_PANGEA_SCINFO_LOGOBACKGROUND_IMAGE_ID, &rect, 0x00000000);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_SCINFO_LOGOBACKGROUND_IMAGE_ID, xmgr_cas_NaPpvScInfoDrawBackgroundImage_Pangea);

	/* na ppv pangea scinfo date text*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCINFO_DATE_TEXT_X, NAPPV_PANGEA_SCINFO_DATE_TEXT_Y, NAPPV_PANGEA_SCINFO_DATE_TEXT_WIDTH, NAPPV_PANGEA_SCINFO_DATE_TEXT_HEIGHT);
	GWM_Text_Create(NAPPV_PANGEA_SCINFO_DATE_TEXT_ID, &rect, (HUINT8*)pContents->szCurrentDateStr);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCINFO_DATE_TEXT_ID, TEXT_ALIGN_RIGHT);
	GWM_Text_SetSize (NAPPV_PANGEA_SCINFO_DATE_TEXT_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor (NAPPV_PANGEA_SCINFO_DATE_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv pangea scinfo infotext*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCINFO_INFO_TEXT_X, NAPPV_PANGEA_SCINFO_INFO_TEXT_Y, NAPPV_PANGEA_SCINFO_INFO_TEXT_WIDTH, NAPPV_PANGEA_SCINFO_INFO_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_SCINFO_INFO_TEXT_ID, &rect, (HUINT8*)pContents->ucTitle);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCINFO_INFO_TEXT_ID, TEXT_ALIGN_RIGHT);
	GWM_Text_SetSize (NAPPV_PANGEA_SCINFO_INFO_TEXT_ID, FONT_SIZE_S);
	GWM_Text_ChangeColor(NAPPV_PANGEA_SCINFO_INFO_TEXT_ID, COL (C_T_Cursor), 0x00);

	// SMARTCARD EXPIRED일 경우에만 표시
	xmgr_cas_NaGetSmartCardStatus(&eAlarmType);
	HxLOG_Print("xmgr_cas_NaGetSmartCardStatus... eAlarmType = %d\n", eAlarmType);
	if (eAlarmType == eCAS_NA_CA_SMARTCARD_EXPIRED)
	{
		/* na ppv pangea scinfo state button */
		OSD_GFX_SetRect(&rect, NAPPV_PANGEA_SCINFO_STATE_BTN_X, NAPPV_PANGEA_SCINFO_STATE_BTN_Y, NAPPV_PANGEA_SCINFO_STATE_BTN_WIDTH, NAPPV_PANGEA_SCINFO_STATE_BTN_HEIGHT);
		GWM_Frame_Create(NAPPV_PANGEA_SCINFO_STATE_BTN_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_SCINFO_STATE_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvScInfoDrawStateButton_Pangea);

		/* na ppv pangea scinfo state text*/
		OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCINFO_STATE_TEXT_X, NAPPV_PANGEA_SCINFO_STATE_TEXT_Y, NAPPV_PANGEA_SCINFO_STATE_TEXT_WIDTH, NAPPV_PANGEA_SCINFO_STATE_TEXT_HEIGHT);
		GWM_Text_Create (NAPPV_PANGEA_SCINFO_STATE_TEXT_ID, &rect, pContents->pszExpiredCard);
		GWM_Text_SetAlign (NAPPV_PANGEA_SCINFO_STATE_TEXT_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (NAPPV_PANGEA_SCINFO_STATE_TEXT_ID, FONT_SIZE_S);
		GWM_Text_ChangeColor(NAPPV_PANGEA_SCINFO_STATE_TEXT_ID, COL (C_T_Cursor), 0x00);
	}

	/* na ppv pangea scinfo back button */
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_SCINFO_BACK_BTN_X, NAPPV_PANGEA_SCINFO_BACK_BTN_Y, NAPPV_PANGEA_SCINFO_BACK_BTN_WIDTH, NAPPV_PANGEA_SCINFO_BACK_BTN_HEIGHT);
	GWM_Button_Create(NAPPV_PANGEA_SCINFO_BACK_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_SCINFO_BACK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvScInfoDrawBackButton_Pangea);

	/* na ppv pangea scinfo back button text*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCINFO_BACK_TEXT_X, NAPPV_PANGEA_SCINFO_BACK_TEXT_Y, NAPPV_PANGEA_SCINFO_BACK_TEXT_WIDTH, NAPPV_PANGEA_SCINFO_BACK_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_SCINFO_BACK_TEXT_ID, &rect, pContents->pszBtnBackHelp);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCINFO_BACK_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_SCINFO_BACK_TEXT_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor(NAPPV_PANGEA_SCINFO_BACK_TEXT_ID, COL (C_T_Cursor), 0x00);

	/* Card Number */
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCINFO_MAINBTN01_X, NAPPV_PANGEA_SCINFO_MAINBTN01_Y, NAPPV_PANGEA_SCINFO_MAINBTN01_WIDTH, NAPPV_PANGEA_SCINFO_MAINBTN01_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_SCINFO_MAINBTN01_CARD_NUMBER_ID, &rect, pContents->pszCardNumberTitle);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCINFO_MAINBTN01_CARD_NUMBER_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_SCINFO_MAINBTN01_CARD_NUMBER_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor(NAPPV_PANGEA_SCINFO_MAINBTN01_CARD_NUMBER_ID, COL (C_W_BG01), 0x00);

	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCINFO_SUBBTN01_X, NAPPV_PANGEA_SCINFO_SUBBTN01_Y, NAPPV_PANGEA_SCINFO_SUBBTN01_WIDTH, NAPPV_PANGEA_SCINFO_SUBBTN01_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_SCINFO_SUBBTN01_CARD_NUMBER_ID, &rect, (HUINT8*)pContents->szSmartCardNumber);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCINFO_SUBBTN01_CARD_NUMBER_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_SCINFO_SUBBTN01_CARD_NUMBER_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor(NAPPV_PANGEA_SCINFO_SUBBTN01_CARD_NUMBER_ID, COL (C_W_BG01), 0x00);

	/* Residual credit */
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCINFO_MAINBTN02_X, NAPPV_PANGEA_SCINFO_MAINBTN02_Y, NAPPV_PANGEA_SCINFO_MAINBTN02_WIDTH, NAPPV_PANGEA_SCINFO_MAINBTN02_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_SCINFO_MAINBTN02_REMAIN_CREDIT_ID, &rect, pContents->pszCardRemainCreditTitle);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCINFO_MAINBTN02_REMAIN_CREDIT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_SCINFO_MAINBTN02_REMAIN_CREDIT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor(NAPPV_PANGEA_SCINFO_MAINBTN02_REMAIN_CREDIT_ID, COL (C_W_BG01), 0x00);

	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCINFO_SUBBTN02_X, NAPPV_PANGEA_SCINFO_SUBBTN02_Y, NAPPV_PANGEA_SCINFO_SUBBTN02_WIDTH, NAPPV_PANGEA_SCINFO_SUBBTN02_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_SCINFO_SUBBTN02_REMAIN_CREDIT_ID, &rect, (HUINT8*)pContents->szRemainedCreditValue);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCINFO_SUBBTN02_REMAIN_CREDIT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_SCINFO_SUBBTN02_REMAIN_CREDIT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor(NAPPV_PANGEA_SCINFO_SUBBTN02_REMAIN_CREDIT_ID, COL (C_W_BG01), 0x00);

	pFont = OSD_FONT_Get(eFont_SystemNormal);
	rect.x += (OSD_GFX_GetStringWidth(pFont, (HINT8 *)pContents->szRemainedCreditValue)+4);
	rect.y += 5;
	if(pFont != NULL)
	{
		OSD_FONT_Release(pFont);
	}
	GWM_Frame_Create(NAPPV_PANGEA_SCINFO_SUBBTN02_REMAIN_CREDIT_EURO_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_SCINFO_SUBBTN02_REMAIN_CREDIT_EURO_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvScInfoDrawEuroIcon_Pangea);

	/* Card Expiry date */
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCINFO_MAINBTN03_X, NAPPV_PANGEA_SCINFO_MAINBTN03_Y, NAPPV_PANGEA_SCINFO_MAINBTN03_WIDTH, NAPPV_PANGEA_SCINFO_MAINBTN03_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_SCINFO_MAINBTN03_EXPIRY_DATE_ID, &rect, pContents->pszCardExpiryTitle);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCINFO_MAINBTN03_EXPIRY_DATE_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_SCINFO_MAINBTN03_EXPIRY_DATE_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor(NAPPV_PANGEA_SCINFO_MAINBTN03_EXPIRY_DATE_ID, COL (C_W_BG01), 0x00);

	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCINFO_SUBBTN03_X, NAPPV_PANGEA_SCINFO_SUBBTN03_Y, NAPPV_PANGEA_SCINFO_SUBBTN03_WIDTH, NAPPV_PANGEA_SCINFO_SUBBTN03_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_SCINFO_SUBBTN03_EXPIRY_DATE_ID, &rect, (HUINT8*)pContents->szSmartCardExpiryDate);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCINFO_SUBBTN03_EXPIRY_DATE_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_SCINFO_SUBBTN03_EXPIRY_DATE_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor(NAPPV_PANGEA_SCINFO_SUBBTN03_EXPIRY_DATE_ID, COL (C_W_BG01), 0x00);

	/* CA version */
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCINFO_MAINBTN04_X, NAPPV_PANGEA_SCINFO_MAINBTN04_Y, NAPPV_PANGEA_SCINFO_MAINBTN04_WIDTH, NAPPV_PANGEA_SCINFO_MAINBTN04_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_SCINFO_MAINBTN04_CA_VERSION_ID, &rect, pContents->pszCaVersionTitle);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCINFO_MAINBTN04_CA_VERSION_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_SCINFO_MAINBTN04_CA_VERSION_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor(NAPPV_PANGEA_SCINFO_MAINBTN04_CA_VERSION_ID, COL (C_W_BG01), 0x00);

	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCINFO_SUBBTN04_X, NAPPV_PANGEA_SCINFO_SUBBTN04_Y, NAPPV_PANGEA_SCINFO_SUBBTN04_WIDTH, NAPPV_PANGEA_SCINFO_SUBBTN04_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_SCINFO_SUBBTN04_CA_VERSION_ID, &rect, (HUINT8*)pContents->szCaVersion);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCINFO_SUBBTN04_CA_VERSION_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_SCINFO_SUBBTN04_CA_VERSION_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor(NAPPV_PANGEA_SCINFO_SUBBTN04_CA_VERSION_ID, COL (C_W_BG01), 0x00);

	/* na ppv pangea scinfo item line */
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_X, NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_Y, NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_WIDTH, NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_SCINFO_MAINBTN01_LINE_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvScInfoDrawMainTextLine_Pangea);

	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_SCINFO_MAINBTN02_LINE_X, NAPPV_PANGEA_SCINFO_MAINBTN02_LINE_Y, NAPPV_PANGEA_SCINFO_MAINBTN02_LINE_WIDTH, NAPPV_PANGEA_SCINFO_MAINBTN02_LINE_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_SCINFO_MAINBTN02_LINE_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_SCINFO_MAINBTN02_LINE_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvScInfoDrawMainTextLine_Pangea);

	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_SCINFO_MAINBTN03_LINE_X, NAPPV_PANGEA_SCINFO_MAINBTN03_LINE_Y, NAPPV_PANGEA_SCINFO_MAINBTN03_LINE_WIDTH, NAPPV_PANGEA_SCINFO_MAINBTN03_LINE_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_SCINFO_MAINBTN03_LINE_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_SCINFO_MAINBTN03_LINE_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvScInfoDrawMainTextLine_Pangea);

	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_SCINFO_MAINBTN04_LINE_X, NAPPV_PANGEA_SCINFO_MAINBTN04_LINE_Y, NAPPV_PANGEA_SCINFO_MAINBTN04_LINE_WIDTH, NAPPV_PANGEA_SCINFO_MAINBTN04_LINE_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_SCINFO_MAINBTN04_LINE_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_SCINFO_MAINBTN04_LINE_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvScInfoDrawMainTextLine_Pangea);

	HxLOG_Print(" --- \n");

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScInfoDestroyUI_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScInfoDestroyUI_Pangea(XmgrCas_NaCardInfo_Pangea_t *pContents)
{
	if (pContents->subItems != NULL)
	{
		HxSTD_memset(pContents->subItems, 0, sizeof(SettingsLeafSubItem_t) * (pContents->ulNumOfSubItem));
		OxAP_Free(pContents->subItems);
	}

	return ERR_BUS_NO_ERROR;
}

#define	_________________________PUBLIC_BODY______________________________
/******************************************************************************/
/**
*   \fn     xproc_cas_NaScInfo_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaScInfo_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaCardInfo_Pangea_t			*pContents = xmgr_cas_NaPpvScInfoGetLocalContents();
	HUINT32							nResult = ERR_FAIL;

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_PANGEA_SCINFO_WINDOW_X, NAPPV_PANGEA_SCINFO_WINDOW_Y,
								NAPPV_PANGEA_SCINFO_WINDOW_WIDTH, NAPPV_PANGEA_SCINFO_WINDOW_HEIGHT);

			xmgr_cas_NaPpvScInfoSetMainTitle_Pangea(pContents);

			// draw UI object
			xmgr_cas_NaPpvScInfoSetUIObjects_Pangea(pContents);

			// set front panel
			//AP_SPEC_DisplayMenuPanelString(pContents->ucTitle, eDxPANEL_ALIGN_CENTER);
			break;

		/*
		Menu가 Display되고 있는 상태에서 CAK에서 Update된 Cas System Info를 AP로 Notify
		=> Menu를 update 해줘야한다.
		*/
		case eMEVT_CAS_SYSINFO_UPDATED:
			HxLOG_Print("eMEVT_CAS_SYSINFO_UPDATED...\n");
			if(p1 == eDxCAS_GROUPID_NA)
			{
				// update cas information
//				xmgr_cas_Nasystem_info_UpdateInformation(pContents);
				GWM_APP_InvalidateObject(NAPPV_PANGEA_SCINFO_STATUS_ID);
			}
			return MESSAGE_BREAK;

		/*
		Menu가 Display되고 있는 상태에서 CAK에서 Update된 SC Info를 Ap로 Notify
		=> Menu를 update 해줘야한다.
		*/
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
					BUS_MGR_Create("xrpoc_cas_NaMenuScError_Pangea", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xrpoc_cas_NaMenuScError_Pangea, 0, eAlarmType, 0, 0);

					return MESSAGE_PASS;
				}
				else
				{
					// Card 정상 삽입 상태인데.. 이쪽으로 왜 올까?
				}
			}
			break;

		case eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT:
			HxLOG_Print("eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT...\n");
			if(p1 == eDxCAS_GROUPID_NA)
			{
				xmgr_cas_NaPpvScInfoUpdateRemainedCredit_Pangea(pContents, (TCreditAmount)p3);
			}
			break;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_KEYUP :
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN :
			AP_KeyConv_UiCommon(&p1);

			nResult = Settings_CheckKeyPass(p1, BUS_MGR_GetMgrCallback(BUS_MGR_GetThis()));
			if(nResult != ERR_OK)
			{
				return nResult;
			}

			switch(p1)
			{
				case KEY_EXIT:
				case KEY_MENU:
					Settings_ClearWindowTitle();

					BUS_MGR_Destroy(NULL);
					return MESSAGE_BREAK;

				//case KEY_ARROWLEFT:
				case KEY_BACK:
					/* 상위 메뉴로 올라갈 때 focus를 기억하지 않는 것이 편할 수도 있다. */
					//Settings_InitFocus(pstContents);

					BUS_MGR_Destroy(NULL);

					/* to upper depth */
					BUS_MGR_Create("xmgr_cas_NaPpvMenu_Pangea_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xmgr_cas_NaPpvMenu_Pangea_Proc, 0, 0, 0, 0);
					return MESSAGE_BREAK;

				case KEY_OK:
				case KEY_ARROWDOWN:
				case KEY_ARROWUP:
					return MESSAGE_BREAK;

			}
			break;


		case MSG_GWM_TIMER:
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			xmgr_cas_NaPpvScInfoDestroyUI_Pangea(pContents);

			/* clear front PANEL */
			//AP_SPEC_ClearMenuPanelString();
			break;

		default:
			break;
	}

	HxLOG_Info("xproc_cas_NaScInfo_Pangea handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif	//end of #if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
/* End of File */
