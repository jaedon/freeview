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


#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
//******************************************************************************
//	DEFINE
//******************************************************************************




#define NAPPV_PANGEA_WINDOW_WIDTH					OSD_SCREEN_WIDTH
#define NAPPV_PANGEA_WINDOW_HEIGHT					OSD_SCREEN_HEIGHT
#define NAPPV_PANGEA_WINDOW_X						(OSD_SCREEN_WIDTH/2-NAPPV_PANGEA_WINDOW_WIDTH/2)
#define NAPPV_PANGEA_WINDOW_Y						(OSD_SCREEN_HEIGHT/2-NAPPV_PANGEA_WINDOW_HEIGHT/2)

typedef struct tag_AP_PPV_PANGEA_CONTENT
{
	/**@breif Public value */
	HUINT32			ulCurFocus;
	HUINT32			ulNumOfSubItem, ulMaxNumOfSubItem;
	SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8			*pTitle;

} XmgrCas_NaPpvPangea_t;

typedef enum
{
	eNaPpvPangeaItemIdx_ProdCatal,
	eNaPpvPangeaItemIdx_PurcProd,
	eNaPpvPangeaItemIdx_ScInfo,
	eNaPpvPangeaItemIdx_RecharInfo,
	eNaPpvPangeaItemIdx_MAX,
} XmgrCas_NaPpvMenuPangeaItem_e;

#define	NAPPV_PANGEA_LIST_ITEM_NUM						4

#define	NAPPV_PANGEA_APP_ID								eNaPpvPangeaItemIdx_ScInfo

/* ppv pangea background */
#define	NAPPV_PANGEA_LOGOBACKGROUND_IMAGE_ID			(NAPPV_PANGEA_APP_ID|0x0010)

/* ppv pangea date */
#define	NAPPV_PANGEA_DATE_TEXT_ID						(NAPPV_PANGEA_APP_ID|0x0020)
#define	NAPPV_PANGEA_DATE_TEXT_X						780
#define	NAPPV_PANGEA_DATE_TEXT_Y						196
#define	NAPPV_PANGEA_DATE_TEXT_WIDTH					300
#define	NAPPV_PANGEA_DATE_TEXT_HEIGHT					31

/* ppv pangea ok button */
#define	NAPPV_PANGEA_OK_BTN_ID							(NAPPV_PANGEA_APP_ID|0x0030)
#define	NAPPV_PANGEA_OK_BTN_X							139
#define	NAPPV_PANGEA_OK_BTN_Y							607
#define	NAPPV_PANGEA_OK_BTN_WIDTH						86
#define	NAPPV_PANGEA_OK_BTN_HEIGHT						44

#define	NAPPV_PANGEA_OK_TEXT_ID							(NAPPV_PANGEA_APP_ID|0x0040)
#define	NAPPV_PANGEA_OK_TEXT_X							232
#define	NAPPV_PANGEA_OK_TEXT_Y							(636 - FONT_SIZE_XS)
#define	NAPPV_PANGEA_OK_TEXT_WIDTH						200
#define	NAPPV_PANGEA_OK_TEXT_HEIGHT						44

/* ppv pangea back button */
#define	NAPPV_PANGEA_BACK_BTN_ID						(NAPPV_PANGEA_APP_ID|0x0050)
#define	NAPPV_PANGEA_BACK_BTN_X							450
#define	NAPPV_PANGEA_BACK_BTN_Y							607
#define	NAPPV_PANGEA_BACK_BTN_WIDTH						86
#define	NAPPV_PANGEA_BACK_BTN_HEIGHT					44

#define	NAPPV_PANGEA_BACK_TEXT_ID						(NAPPV_PANGEA_APP_ID|0x0060)
#define	NAPPV_PANGEA_BACK_TEXT_X						543
#define	NAPPV_PANGEA_BACK_TEXT_Y						(636 - FONT_SIZE_XS)
#define	NAPPV_PANGEA_BACK_TEXT_WIDTH					200
#define	NAPPV_PANGEA_BACK_TEXT_HEIGHT					44

/* ppv pangea round button */
#define	NAPPV_PANGEA_ROUND_BTN_ID						(NAPPV_PANGEA_APP_ID|0x0070)
#define	NAPPV_PANGEA_ROUND_BTN_X						866
#define	NAPPV_PANGEA_ROUND_BTN_Y						607
#define	NAPPV_PANGEA_ROUND_BTN_WIDTH					86
#define	NAPPV_PANGEA_ROUND_BTN_HEIGHT					44



#define __________ITEM_OBJECTS_________________________________________
#define	NAPPV_PANGEA_BTN_WIDTH							331
#define	NAPPV_PANGEA_BTN_HEIGHT							103
#define	NAPPV_PANGEA_BTN_WIDTHSPACE						400
#define	NAPPV_PANGEA_BTN_HIGHTSPACE						132

/* pangea btn 01 position */
#define	NAPPV_PANGEA_BTN01_ID							(NAPPV_PANGEA_APP_ID|0x0110)
#define	NAPPV_PANGEA_BTN01_WIDTH						NAPPV_PANGEA_BTN_WIDTH
#define	NAPPV_PANGEA_BTN01_HEIGHT						NAPPV_PANGEA_BTN_HEIGHT
#define	NAPPV_PANGEA_BTN01_X							140
#define	NAPPV_PANGEA_BTN01_Y							273

/* pangea btn 02 position */
#define	NAPPV_PANGEA_BTN02_ID							(NAPPV_PANGEA_APP_ID|0x0120)
#define	NAPPV_PANGEA_BTN02_WIDTH						NAPPV_PANGEA_BTN_WIDTH
#define	NAPPV_PANGEA_BTN02_HEIGHT						NAPPV_PANGEA_BTN_HEIGHT
#define	NAPPV_PANGEA_BTN02_X							(NAPPV_PANGEA_BTN01_X + NAPPV_PANGEA_BTN_WIDTHSPACE)
#define	NAPPV_PANGEA_BTN02_Y							NAPPV_PANGEA_BTN01_Y

/* pangea btn 03 position */
#define	NAPPV_PANGEA_BTN03_ID							(NAPPV_PANGEA_APP_ID|0x0130)
#define	NAPPV_PANGEA_BTN03_WIDTH						NAPPV_PANGEA_BTN_WIDTH
#define	NAPPV_PANGEA_BTN03_HEIGHT						NAPPV_PANGEA_BTN_HEIGHT
#define	NAPPV_PANGEA_BTN03_X							NAPPV_PANGEA_BTN01_X
#define	NAPPV_PANGEA_BTN03_Y							(NAPPV_PANGEA_BTN01_Y + NAPPV_PANGEA_BTN_HIGHTSPACE)

/* pangea btn 04 position */
#define	NAPPV_PANGEA_BTN04_ID							(NAPPV_PANGEA_APP_ID|0x0140)
#define	NAPPV_PANGEA_BTN04_WIDTH						NAPPV_PANGEA_BTN_WIDTH
#define	NAPPV_PANGEA_BTN04_HEIGHT						NAPPV_PANGEA_BTN_HEIGHT
#define	NAPPV_PANGEA_BTN04_X							(NAPPV_PANGEA_BTN01_X + NAPPV_PANGEA_BTN_WIDTHSPACE)
#define	NAPPV_PANGEA_BTN04_Y							(NAPPV_PANGEA_BTN01_Y + NAPPV_PANGEA_BTN_HIGHTSPACE)


STATIC Item_t	s_Pangea_NaviMainItem[NAPPV_PANGEA_LIST_ITEM_NUM]={
/* 01 */	{NAPPV_PANGEA_BTN01_ID, { NAPPV_PANGEA_BTN01_X, NAPPV_PANGEA_BTN01_Y, NAPPV_PANGEA_BTN01_WIDTH, NAPPV_PANGEA_BTN01_HEIGHT}},
/* 02 */	{NAPPV_PANGEA_BTN02_ID, { NAPPV_PANGEA_BTN02_X, NAPPV_PANGEA_BTN02_Y, NAPPV_PANGEA_BTN02_WIDTH, NAPPV_PANGEA_BTN02_HEIGHT}},
/* 03 */	{NAPPV_PANGEA_BTN03_ID, { NAPPV_PANGEA_BTN03_X, NAPPV_PANGEA_BTN03_Y, NAPPV_PANGEA_BTN03_WIDTH, NAPPV_PANGEA_BTN03_HEIGHT}},
/* 04 */	{NAPPV_PANGEA_BTN04_ID, { NAPPV_PANGEA_BTN04_X, NAPPV_PANGEA_BTN04_Y, NAPPV_PANGEA_BTN04_WIDTH, NAPPV_PANGEA_BTN04_HEIGHT}},
};


STATIC SettingsNaviContext_t	s_stApNaPpvMenuPangea;

#define ________XMGR_CAS_NA_PpvPANGEA_MENU_STATIC___________________________________________
STATIC inline SettingsNaviContext_t * xmgr_cas_NaPpvGetLocalContents_Pangea(void);
STATIC inline HINT32 	xmgr_cas_NaPpvGetDefaultFocus_Pangea(void);
STATIC HINT32			xmgr_cas_NaPpvConvertObjIdToItemIdx_Pangea(HINT32 nObjId);
STATIC HINT32			xmgr_cas_NaPpvConvertItemIdxToObjId_Pangea(HINT32 nItemIdx);
STATIC BUS_Result_t		xmgr_cas_NaPpvInitItemSmartCardInformation_Pangea(SettingsNaviContext_t *pContents);
STATIC BUS_Result_t		xmgr_cas_NaPpvInitItemPurchasedProducts_Pangea(SettingsNaviContext_t *pContents);
STATIC BUS_Result_t		xmgr_cas_NaPpvInitItemRechargeInformation_Pangea(SettingsNaviContext_t *pContents);
STATIC BUS_Result_t		xmgr_cas_NaPpvInitItemProductsCatalogue_Pangea(SettingsNaviContext_t *pContents);
STATIC BUS_Result_t		xmgr_cas_NaPpvInitPangeaItems_Pangea(SettingsNaviContext_t *pContents);
STATIC BUS_Result_t		xmgr_cas_NaPpvDrawBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t		xmgr_cas_NaPpvGetCurrentTime_Pangea(UNIXTIME *pulCurrentTime);
STATIC BUS_Result_t		xmgr_cas_NaPpvDrawCurrentDate_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t		xmgr_cas_NaPpvDrawOKButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t		xmgr_cas_NaPpvDrawBackButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t		xmgr_cas_NaPpvDrawRoundButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t		xmgr_cas_NaPpvDrawNaviMainItemMethod_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int bUiTop);
STATIC BUS_Result_t		xmgr_cas_NaPpvInitNaviMainItems_Pangea(SettingsNaviMainItem_t *mainItemList, HUINT32 maxMainItem, GWM_ObjDraw_t drawMethod);
STATIC BUS_Result_t		xmgr_cas_NaPpvDrawDefaultUI_Pangea(SettingsNaviContext_t *pContents);


#define	_________________________BODY______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvGetLocalContents_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC inline SettingsNaviContext_t * xmgr_cas_NaPpvGetLocalContents_Pangea(void)
{
	return &s_stApNaPpvMenuPangea;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvGetDefaultFocus_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC inline HINT32 xmgr_cas_NaPpvGetDefaultFocus_Pangea(void)
{
	return eNaPpvPangeaItemIdx_ProdCatal;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvConvertObjIdToItemIdx_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC HINT32 xmgr_cas_NaPpvConvertObjIdToItemIdx_Pangea(HINT32 nObjId)
{
	HINT32	nItemId = 0;

	nItemId = (nObjId & ~eSetting_PpvPangea);

	if((nItemId < 0) || (nItemId >= eNaPpvPangeaItemIdx_MAX))
	{
		return -1;
	}

	return nItemId;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvConvertItemIdxToObjId_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC HINT32 xmgr_cas_NaPpvConvertItemIdxToObjId_Pangea(HINT32 nItemIdx)
{
	if((nItemIdx < 0) || (nItemIdx >= eNaPpvPangeaItemIdx_MAX))
	{
		return 0;
	}

	return (eSetting_PpvPangea | nItemIdx);
}

/********************************************************************
 Function		: xmgr_cas_NaPpvInitItemSmartCardInformation_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvInitItemSmartCardInformation_Pangea(SettingsNaviContext_t *pContents)
{
	SettingsNaviMainItem_t		stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = xmgr_cas_NaPpvConvertItemIdxToObjId_Pangea(eNaPpvPangeaItemIdx_ScInfo);

	/* set title, Loc Id */
	stMainItem.nTitleLocId = LOC_CAS_NAGRA_PANGEA_CARD_INFORMATION_TEXT_ID;
	MWC_UTIL_DvbStrcpy(stMainItem.title, GetStrRsc(LOC_CAS_NAGRA_PANGEA_CARD_INFORMATION_TEXT_ID));

	/*
		Function to launch :
		Arguments: nothing
	*/
	MWC_UTIL_DvbStrcpy (stMainItem.appName, (HUINT8 *)"xproc_cas_NaScInfo_Pangea");
	stMainItem.appToLaunch = xproc_cas_NaScInfo_Pangea;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem(pContents, &stMainItem);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvInitItemPurchasedProducts_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvInitItemPurchasedProducts_Pangea(SettingsNaviContext_t *pContents)
{
	SettingsNaviMainItem_t		stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = xmgr_cas_NaPpvConvertItemIdxToObjId_Pangea(eNaPpvPangeaItemIdx_PurcProd);

	/* set title, Loc Id */
	stMainItem.nTitleLocId = LOC_CAS_NAGRA_PANGEA_PURCHASED_PRODUCTS_TEXT_ID;
	MWC_UTIL_DvbStrcpy(stMainItem.title, GetStrRsc(LOC_CAS_NAGRA_PANGEA_PURCHASED_PRODUCTS_TEXT_ID));

	/*
		Function to launch :
		Arguments: nothing
	*/
	MWC_UTIL_DvbStrcpy (stMainItem.appName, (HUINT8 *)"xproc_cas_NaPurchasedProducts_Pangea");
	stMainItem.appToLaunch = xproc_cas_NaPurchasedProducts_Pangea;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem(pContents, &stMainItem);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvInitItemRechargeInformation_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvInitItemRechargeInformation_Pangea(SettingsNaviContext_t *pContents)
{
	SettingsNaviMainItem_t		stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = xmgr_cas_NaPpvConvertItemIdxToObjId_Pangea(eNaPpvPangeaItemIdx_RecharInfo);

	/* set title, Loc Id */
	stMainItem.nTitleLocId = LOC_CAS_NAGRA_PANGEA_RECHARGE_INFORMATION_TEXT_ID;
	MWC_UTIL_DvbStrcpy(stMainItem.title, GetStrRsc(LOC_CAS_NAGRA_PANGEA_RECHARGE_INFORMATION_TEXT_ID));

	/*
		Function to launch :
		Arguments: nothing
	*/
	MWC_UTIL_DvbStrcpy (stMainItem.appName, (HUINT8 *)"xproc_cas_NaRechargeInformation_Pangea");
	stMainItem.appToLaunch = xproc_cas_NaRechargeInformation_Pangea;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem(pContents, &stMainItem);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvInitItemProductsCatalogue_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvInitItemProductsCatalogue_Pangea(SettingsNaviContext_t *pContents)
{
	SettingsNaviMainItem_t		stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = xmgr_cas_NaPpvConvertItemIdxToObjId_Pangea(eNaPpvPangeaItemIdx_ProdCatal);

	/* set title, Loc Id */
	stMainItem.nTitleLocId = LOC_CAS_NAGRA_PANGEA_PRODUCTS_CATALOGUE_TEXT_ID;
	MWC_UTIL_DvbStrcpy(stMainItem.title, GetStrRsc(LOC_CAS_NAGRA_PANGEA_PRODUCTS_CATALOGUE_TEXT_ID));

	/*
		Function to launch :
		Arguments: nothing
	*/
	MWC_UTIL_DvbStrcpy (stMainItem.appName, (HUINT8 *)"xmgr_cas_NaPANGEA_ProductsCatalogue_Proc");
	stMainItem.appToLaunch = xmgr_cas_NaPANGEA_ProductsCatalogue_Proc;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem(pContents, &stMainItem);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvInitPangeaItems_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvInitPangeaItems_Pangea(SettingsNaviContext_t *pContents)
{
	HERROR		hErr;

	/* param check */
	if(pContents == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	/* Init the sub items */
	hErr = AP_NaviCntxt_InitMainItemArray (pContents);
	if ( hErr != ERR_BUS_NO_ERROR )
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	/* settings default focus는 제일 첫 아이템 */
	pContents->nDefaultFocusIdx = xmgr_cas_NaPpvGetDefaultFocus_Pangea();
	pContents->pConvFunc = xmgr_cas_NaPpvConvertObjIdToItemIdx_Pangea;

	/* Make SubItems */
	xmgr_cas_NaPpvInitItemProductsCatalogue_Pangea(pContents);
	xmgr_cas_NaPpvInitItemPurchasedProducts_Pangea(pContents);
	xmgr_cas_NaPpvInitItemSmartCardInformation_Pangea(pContents);
	xmgr_cas_NaPpvInitItemRechargeInformation_Pangea(pContents);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvDrawBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvDrawBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
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
 Function		: xmgr_cas_NaPpvGetCurrentTime_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvGetCurrentTime_Pangea(UNIXTIME *pulCurrentTime)
{
	BUS_Result_t	lResult = ERR_INITIAL_FAILED;
	HUINT32			ulSysTime = 0;
	HERROR			hError = ERR_FAIL;

	hError = (HERROR)VK_CLOCK_GetTime((unsigned long*)&ulSysTime);
	if(hError != VK_OK || UNIX_FIRST_DAY >= ulSysTime)
	{
		ulSysTime = UNIX_FIRST_DAY;
	}

	*pulCurrentTime = (UNIXTIME)ulSysTime;

	lResult = ERR_BUS_NO_ERROR;

	return lResult;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvDrawCurrentDate_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvDrawCurrentDate_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR			hError = ERR_FAIL;
	UNIXTIME		ulSystemTime = 0;
	HxDATETIME_t 	stDateTime;

	HINT8 			szDate[128];
	HINT32			nObjectId;

	/* for compiler */
	(void)pArea;
	(void)pArg;
	(void)param1;

	nObjectId = GWM_Obj_GetObjectID(pObject);
	xmgr_cas_NaPpvGetCurrentTime_Pangea((UNIXTIME*)&ulSystemTime);

	hError = HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSystemTime, &stDateTime);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDrawCurrentDate_Pangea] ConvertUnixTimeToDateTime fail!!\n");
		return ERR_FAIL;
	}

	sprintf(szDate, "%s: %02d/%02d/%04d", GetStrRsc(LOC_CAS_NAGRA_PANGEA_DATE_TEXT_ID), stDateTime.stDate.ucDay, stDateTime.stDate.ucMonth, stDateTime.stDate.usYear);
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_XS, pRect->x, pRect->y, pRect->w,
										(HINT8 *)szDate, COL(C_T_Cursor));

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvDrawOKButton_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvDrawOKButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e921_1_24_BTN_1OK, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvDrawBackButton_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvDrawBackButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvDrawRoundButton_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvDrawRoundButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e921_1_24_BTN_3LRUD, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvDrawNaviMainItemMethod_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvDrawNaviMainItemMethod_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
							OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int bUiTop)
{
	HERROR					hError;

	HINT32					x1 = 0, y1 = 0;
	HINT32					w = 0, h = 0;
	HINT32					text_x_offset = 0, text_y_offset = 0, text_w_offset = 0, text_h_offset = 0;
	HUINT32					status = 0;

	HINT8					*title = NULL;

	BUTTON_OBJECT			btn = (BUTTON_OBJECT)pArg;
	OSD_StockImage_t		stCursorImage;
	OSD_Rect_t				StrRect, ImageRect;
	Text_Property_t			stTextProperty;

	/* init coordinate */
	x1 = pArea->x1 + pRect->x;
	y1 = pArea->y1 + pRect->y;
	w = pRect->w;
	h = pRect->h;

	/* for compiler */
	(void)pObject;

	/* get text */
	title = GWM_Button_GetTitlePtr(btn);
	if(title == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	/* focus status */
	status = GWM_Button_GetStatus(btn);

	/* basic font color */
	stTextProperty.ulFontSize = FONT_SIZE_M;
	stTextProperty.ulFontAlign = TEXT_ALIGN_CENTER;
	stTextProperty.cFontColor = COL(C_W_BG01);

	/* app is top most */
	if (bUiTop > 0)
	{
		/* na ppv pangea item box image */
		if (status & GUIOBJ_HILIGHTED)
		{
			/* focus font color */

			/* focus image */
			OSD_GFX_SetRect(&ImageRect, x1, y1, w, h);
			if (!AP_StockImg_GetImage (e923_1_24_Cursor1_B_On, &stCursorImage))
			{
				OSD_GFX_FB_Blend (stCursorImage.hBuffer, stCursorImage.width, stCursorImage.height, pScreen, &ImageRect);
			}
		}
		else
		{
			/* Normal image */
			OSD_GFX_SetRect(&ImageRect, x1, y1, w, h);
			if (!AP_StockImg_GetImage (e923_1_24_Cursor1_B_Off, &stCursorImage))
			{
				OSD_GFX_FB_Blend (stCursorImage.hBuffer, stCursorImage.width, stCursorImage.height, pScreen, &ImageRect);
			}
		}
	}

	/* item index */
	text_x_offset = x1 + 46;
	text_y_offset = y1 + 15;
	text_w_offset = w - 118;
	text_h_offset = h - 0;
	OSD_GFX_SetRect(&StrRect, text_x_offset, text_y_offset, text_w_offset, text_h_offset);
	hError = AP_LNF_DrawLongStringWithWordwrap(pScreen, pArea, &StrRect, (HUINT8*)title,
													stTextProperty, StrRect.w, bUiTop);
	if (hError != ERR_OK)
	{
		HxLOG_Error(" AP_LNF_DrawLongStringWithWordwrap Error!\n");
		return ERR_FAIL;
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvInitNaviMainItems_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvInitNaviMainItems_Pangea(SettingsNaviMainItem_t *mainItemList, HUINT32 maxMainItem, GWM_ObjDraw_t drawMethod)
{
	HUINT32					i = 0;
	OSD_Rect_t				rect = {0, };
	SettingsNaviMainItem_t	*pMainItem = NULL;

	/* init subitems until max count */
	for(i = 0; i < maxMainItem; i++)
	{
		pMainItem = &(mainItemList[i]);

		OSD_GFX_SetRect(&rect, s_Pangea_NaviMainItem[i].rect.x, s_Pangea_NaviMainItem[i].rect.y, s_Pangea_NaviMainItem[i].rect.w, s_Pangea_NaviMainItem[i].rect.h);
		GWM_Button_Create(pMainItem->mainBtnId, &rect, pMainItem->title);
		GWM_Button_SetExtraText(pMainItem->mainBtnId, (HUINT8 *)pMainItem->subItemString);
		if(pMainItem->bDisable == TRUE)
		{
			GWM_Obj_DisableObject(pMainItem->mainBtnId);
		}
		GWM_APP_SetObjectDrawMethod(pMainItem->mainBtnId, drawMethod);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvDrawDefaultUI_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvDrawDefaultUI_Pangea(SettingsNaviContext_t *pContents)
{
	OSD_Rect_t	rect;

	/* Title */
	MWC_UTIL_DvbStrcpy(pContents->title ,(HUINT8 *)GetStrRsc(LOC_CAS_PANGEA_TEXT_ID));
	Settings_SetWindowTitle(SETTING_GET_AP_DEPTH(eSetting_PpvPangea), LOC_CAS_PANGEA_TEXT_ID, pContents->title);

	/* na ppv pangea background */
	Settings_DrawBack(FALSE);
	OSD_GFX_SetRect(&rect, 0, 0, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT);
	GWM_Rect_Create(NAPPV_PANGEA_LOGOBACKGROUND_IMAGE_ID, &rect, 0x00000000);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_LOGOBACKGROUND_IMAGE_ID, xmgr_cas_NaPpvDrawBackgroundImage_Pangea);

	/* na ppv pangea date text*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_DATE_TEXT_X, NAPPV_PANGEA_DATE_TEXT_Y, NAPPV_PANGEA_DATE_TEXT_WIDTH, NAPPV_PANGEA_DATE_TEXT_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_DATE_TEXT_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_DATE_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvDrawCurrentDate_Pangea);

	/* na ppv pangea ok button */
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_OK_BTN_X, NAPPV_PANGEA_OK_BTN_Y, NAPPV_PANGEA_OK_BTN_WIDTH, NAPPV_PANGEA_OK_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_OK_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_OK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvDrawOKButton_Pangea);

	/* na ppv pangea ok button text*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_OK_TEXT_X, NAPPV_PANGEA_OK_TEXT_Y, NAPPV_PANGEA_OK_TEXT_WIDTH, NAPPV_PANGEA_OK_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_OK_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_CONFIRM_TEXT_ID));
	GWM_Text_SetAlign (NAPPV_PANGEA_OK_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_OK_TEXT_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor(NAPPV_PANGEA_OK_TEXT_ID, COL (C_T_Cursor), 0x00);

	/* na ppv pangea back button */
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_BACK_BTN_X, NAPPV_PANGEA_BACK_BTN_Y, NAPPV_PANGEA_BACK_BTN_WIDTH, NAPPV_PANGEA_BACK_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_BACK_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_BACK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvDrawBackButton_Pangea);

	/* na ppv pangea backbutton text*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_BACK_TEXT_X, NAPPV_PANGEA_BACK_TEXT_Y, NAPPV_PANGEA_BACK_TEXT_WIDTH, NAPPV_PANGEA_BACK_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_BACK_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_EXIT_TEXT_ID));
	GWM_Text_SetAlign (NAPPV_PANGEA_BACK_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_BACK_TEXT_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor(NAPPV_PANGEA_BACK_TEXT_ID, COL (C_T_Cursor), 0x00);

	/* na ppv pangea round button */
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_ROUND_BTN_X, NAPPV_PANGEA_ROUND_BTN_Y, NAPPV_PANGEA_ROUND_BTN_WIDTH, NAPPV_PANGEA_ROUND_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_ROUND_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_ROUND_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvDrawRoundButton_Pangea);

	/* Init sub item objects */
	xmgr_cas_NaPpvInitNaviMainItems_Pangea(pContents->mainItems, pContents->ulNumOfMainItem, (GWM_ObjDraw_t)xmgr_cas_NaPpvDrawNaviMainItemMethod_Pangea);

	/* init sub items */
	Settings_InitNaviSubItemList(pContents->mainItems, pContents->nCurFocus, TRUE);

	/* Set current position */
	GWM_Obj_SetFocus(pContents->mainItems[pContents->nCurFocus].mainBtnId);

	return ERR_BUS_NO_ERROR;
}

#define ________XMGR_CAS_NA_PpvPANGEA_MENU_PUBLIC___________________________________________
/********************************************************************
 Function		: xproc_cas_NaPpvMenu_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
BUS_Result_t xproc_cas_NaPpvMenu_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					nResult = ERR_FAIL;
	SettingsNaviContext_t		*pContents = xmgr_cas_NaPpvGetLocalContents_Pangea();

	switch(message)
	{
		case MSG_GWM_CREATE:
			HxLOG_Info("MSG_GWM_CREATE!\n");

#if !defined(WIN32)
			// Check SmartCard Error Check
			{
				XmgrCas_NaAlamType_e	eAlarmType;

				xmgr_cas_NaCheckPauseResume_Mediaset(TRUE);

				xmgr_cas_NaGetSmartCardStatus(&eAlarmType);
				HxLOG_Print("MSG_APP_UPDATE_SMARTCARD_INFO... eAlarmType = %d\n", eAlarmType);

				if (eAlarmType != eCAS_NA_CA_SMARTCARD_REMOVED && eAlarmType != eCAS_NA_CA_SMARTCARD_MUTE && eAlarmType != eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
				{
					if (!xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(eNAGRA_OP_TYPE_PANGEA))
					{
						HxLOG_Print("xmgr_cas_NaMatchCardStatusWithOpType_Mediaset FALSE!!!\n");
						eAlarmType = eCAS_NA_CA_SMARTCARD_INVALID;
					}
				}

				if (eAlarmType != eCAS_NA_CA_SMARTCARD_OK && eAlarmType != eCAS_NA_CA_SMARTCARD_EXPIRED)
				{
					BUS_MGR_Destroy(NULL);

					/* to upper depth */
					BUS_MGR_Create("xrpoc_cas_NaMenuScError_Pangea", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xrpoc_cas_NaMenuScError_Pangea, 0, eAlarmType, 0, 0);

					return MESSAGE_BREAK;
				}
			}
#endif

			/* init application area */
			GWM_APP_SetAppArea(NAPPV_PANGEA_WINDOW_X, NAPPV_PANGEA_WINDOW_Y,
								NAPPV_PANGEA_WINDOW_WIDTH, NAPPV_PANGEA_WINDOW_HEIGHT);

			/* Initialiaze Items */
			nResult = xmgr_cas_NaPpvInitPangeaItems_Pangea(pContents);
			if(nResult != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

			/* Draw UI */
			xmgr_cas_NaPpvDrawDefaultUI_Pangea(pContents);

			/* Set menu timer, for TV products */
			BUS_SetTimer(MENU_TIMEOUT_ID, MENU_TIMEOUT);

			/* Display Panel - Panel은 Only English? */
			//AP_SPEC_DisplayMenuPanelString(pContents->title, eDxPANEL_ALIGN_CENTER);

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
					BUS_MGR_Create("xrpoc_cas_NaMenuScError_Pangea", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xrpoc_cas_NaMenuScError_Pangea, 0, eAlarmType, 0, 0);

					return MESSAGE_PASS;
				}
				else
				{
					// Card 정상 삽입 상태인데.. 이쪽으로 왜 올까?
				}
			}
			break;

		case MSG_GWM_SCROLLRIGHT:
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_CLICKED:
			//xmgr_cas_NaPpvProcessClicked(pContents, p1/* objId */);
		{
			SettingsNaviMainItem_t 	mainItem;
			HINT32					nItemIdx = -1;

			nItemIdx = xmgr_cas_NaPpvConvertObjIdToItemIdx_Pangea(p1);

			if(pContents->mainItems[nItemIdx].appToLaunch != NULL)
			{
				HxSTD_memcpy(&mainItem, &(pContents->mainItems[nItemIdx]), sizeof(SettingsNaviMainItem_t));
				BUS_MGR_Destroy(NULL);

				BUS_MGR_Create(mainItem.appName,
								(APP_DEFAULT_PRIORITY | UISTATE_MENU),
								mainItem.appToLaunch,
								mainItem.hAction,
								mainItem.appParam1,
								mainItem.appParam2,
								mainItem.appParam3);

				return MESSAGE_BREAK;
			}
		}
			return MESSAGE_BREAK;

		case MSG_GWM_KEYUP :
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN:
			/* settings이 전반적으로 처리해야 할 키들 */
			AP_KeyConv_UiCommon(&p1);

			GWM_ResetTimer(MENU_TIMEOUT_ID);

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

				case KEY_BACK:
					/* 상위 메뉴로 올라갈 때 focus를 기억하지 않는 것이 편할 수도 있다. */
					BUS_MGR_Destroy(NULL);

					/* to upper depth */
					BUS_MGR_Create("AP_CAS_PpvMenu_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)AP_CAS_PpvMenu_Proc, 0, 0, 0, 0);
					return MESSAGE_BREAK;
			}
			break;

		case MSG_GWM_TIMER:
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			/* clear front PANEL */
			//AP_SPEC_ClearMenuPanelString();
			break;

		default:
			break;
	}

	HxLOG_Info("xproc_cas_NaPpvMenu_Pangea handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif	//end of #if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
/* End of File */
