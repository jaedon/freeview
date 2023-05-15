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

#include "ap_menu_contents.h"
#include "../local_include/_xmgr_cas_na.h"
#include "../local_include/_xmgr_cas_na_util.h"
#include "../local_include/_xmgr_cas_na_adapt.h"


#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
//******************************************************************************
//	DEFINE
//******************************************************************************




#define NAPPV_MEDIASET_WINDOW_WIDTH						OSD_SCREEN_WIDTH
#define NAPPV_MEDIASET_WINDOW_HEIGHT					OSD_SCREEN_HEIGHT
#define NAPPV_MEDIASET_WINDOW_X							(OSD_SCREEN_WIDTH/2-NAPPV_MEDIASET_WINDOW_WIDTH/2)
#define NAPPV_MEDIASET_WINDOW_Y							(OSD_SCREEN_HEIGHT/2-NAPPV_MEDIASET_WINDOW_HEIGHT/2)

typedef struct tag_AP_PPV_MEDIASET_CONTENT
{
	/**@breif Public value */
	HUINT32			ulCurFocus;
	HUINT32			ulNumOfSubItem, ulMaxNumOfSubItem;
	SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8			*pTitle;

} AP_NaPpvMediaset_t;

typedef enum
{
	eNaPpvMediasetItemIdx_ScInfo,
	eNaPpvMediasetItemIdx_SelProg,
	eNaPpvMediasetItemIdx_RechargeHistory,
	eNaPpvMediasetItemIdx_PackagePurchase,
	eNaPpvMediasetItemIdx_MAX,
} XmgrCas_NaPpvMenuItem_Mediaset_t;

#define	NAPPV_MEDIASET_LIST_ITEM_NUM					4

#define	NAPPV_MEDIASET_APP_ID							eNaPpvMediasetItemIdx_ScInfo

/* ppv mediaset background */
#define	NAPPV_MEDIASET_LOGOBACKGROUND_IMAGE_ID			(NAPPV_MEDIASET_APP_ID|0x0010)


/* ppv mediaset date */
#define	NAPPV_MEDIASET_DATE_TEXT_ID						(NAPPV_MEDIASET_APP_ID|0x0030)
#define	NAPPV_MEDIASET_DATE_TEXT_X						830
#define	NAPPV_MEDIASET_DATE_TEXT_Y						122
#define	NAPPV_MEDIASET_DATE_TEXT_WIDTH					300
#define	NAPPV_MEDIASET_DATE_TEXT_HEIGHT					25

/* ppv mediaset arrow */
#define	NAPPV_MEDIASET_ARROW_BTN_ID						(NAPPV_MEDIASET_APP_ID|0x0040)
#define	NAPPV_MEDIASET_ARROW_BTN_X						1115
#define	NAPPV_MEDIASET_ARROW_BTN_Y						541
#define	NAPPV_MEDIASET_ARROW_BTN_WIDTH					30
#define	NAPPV_MEDIASET_ARROW_BTN_HEIGHT					12

/* ppv mediaset ok button */
#define	NAPPV_MEDIASET_OK_BTN_ID						(NAPPV_MEDIASET_APP_ID|0x0060)
#define	NAPPV_MEDIASET_OK_BTN_X							1017
#define	NAPPV_MEDIASET_OK_BTN_Y							611
#define	NAPPV_MEDIASET_OK_BTN_WIDTH						52
#define	NAPPV_MEDIASET_OK_BTN_HEIGHT					32

/* ppv mediaset back button */
#define	NAPPV_MEDIASET_BACK_BTN_ID						(NAPPV_MEDIASET_APP_ID|0x0080)
#define	NAPPV_MEDIASET_BACK_BTN_X						1079
#define	NAPPV_MEDIASET_BACK_BTN_Y						611
#define	NAPPV_MEDIASET_BACK_BTN_WIDTH					72
#define	NAPPV_MEDIASET_BACK_BTN_HEIGHT					32

#define __________ITEM_OBJECTS_________________________________________
#define	NAPPV_MEDIASET_BTN_WIDTH						1009 //770 +98
#define	NAPPV_MEDIASET_BTN_HEIGHT						40
#define	NAPPV_MEDIASET_BTN_LINESPACE					55

/* mediaset btn 01 position */
#define	NAPPV_MEDIASET_BTN01_ID							(NAPPV_MEDIASET_APP_ID|0x0210)
#define	NAPPV_MEDIASET_BTN01_WIDTH						NAPPV_MEDIASET_BTN_WIDTH
#define	NAPPV_MEDIASET_BTN01_HEIGHT						NAPPV_MEDIASET_BTN_HEIGHT
#define	NAPPV_MEDIASET_BTN01_X							136 //238
#define	NAPPV_MEDIASET_BTN01_Y							205 //271

/* mediaset btn 02 position */
#define	NAPPV_MEDIASET_BTN02_ID							(NAPPV_MEDIASET_APP_ID|0x0230)
#define	NAPPV_MEDIASET_BTN02_WIDTH						NAPPV_MEDIASET_BTN_WIDTH
#define	NAPPV_MEDIASET_BTN02_HEIGHT						NAPPV_MEDIASET_BTN_HEIGHT
#define	NAPPV_MEDIASET_BTN02_X							NAPPV_MEDIASET_BTN01_X
#define	NAPPV_MEDIASET_BTN02_Y							(NAPPV_MEDIASET_BTN01_Y+NAPPV_MEDIASET_BTN_LINESPACE)

/* mediaset btn 03 position */
#define	NAPPV_MEDIASET_BTN03_ID							(NAPPV_MEDIASET_APP_ID|0x0250)
#define	NAPPV_MEDIASET_BTN03_WIDTH						NAPPV_MEDIASET_BTN_WIDTH
#define	NAPPV_MEDIASET_BTN03_HEIGHT						NAPPV_MEDIASET_BTN_HEIGHT
#define	NAPPV_MEDIASET_BTN03_X							NAPPV_MEDIASET_BTN01_X
#define	NAPPV_MEDIASET_BTN03_Y							(NAPPV_MEDIASET_BTN02_Y+NAPPV_MEDIASET_BTN_LINESPACE)

/* mediaset btn 04 position */
#define	NAPPV_MEDIASET_BTN04_ID							(NAPPV_MEDIASET_APP_ID|0x0270)
#define	NAPPV_MEDIASET_BTN04_WIDTH						NAPPV_MEDIASET_BTN_WIDTH
#define	NAPPV_MEDIASET_BTN04_HEIGHT						NAPPV_MEDIASET_BTN_HEIGHT
#define	NAPPV_MEDIASET_BTN04_X							NAPPV_MEDIASET_BTN01_X
#define	NAPPV_MEDIASET_BTN04_Y							(NAPPV_MEDIASET_BTN03_Y+NAPPV_MEDIASET_BTN_LINESPACE)


STATIC Item_t	s_Mediaset_NaviMainItem[NAPPV_MEDIASET_LIST_ITEM_NUM]={
/* 01 */	{NAPPV_MEDIASET_BTN01_ID, { NAPPV_MEDIASET_BTN01_X, NAPPV_MEDIASET_BTN01_Y, NAPPV_MEDIASET_BTN01_WIDTH, NAPPV_MEDIASET_BTN01_HEIGHT}},
/* 02 */	{NAPPV_MEDIASET_BTN02_ID, { NAPPV_MEDIASET_BTN02_X, NAPPV_MEDIASET_BTN02_Y, NAPPV_MEDIASET_BTN02_WIDTH, NAPPV_MEDIASET_BTN02_HEIGHT}},
/* 03 */	{NAPPV_MEDIASET_BTN03_ID, { NAPPV_MEDIASET_BTN03_X, NAPPV_MEDIASET_BTN03_Y, NAPPV_MEDIASET_BTN03_WIDTH, NAPPV_MEDIASET_BTN03_HEIGHT}},
/* 04 */	{NAPPV_MEDIASET_BTN04_ID, { NAPPV_MEDIASET_BTN04_X, NAPPV_MEDIASET_BTN04_Y, NAPPV_MEDIASET_BTN04_WIDTH, NAPPV_MEDIASET_BTN04_HEIGHT}},
};
STATIC SettingsNaviContext_t	s_stApNaPpvMenuMediaset;

#define ________XMGR_CAS_NA_PPV_MEDIASET_MENU_STATIC___________________________________________
STATIC inline SettingsNaviContext_t * xmgr_cas_NaPpvGetLocalContents_Mediaset(void);
STATIC inline HINT32 xmgr_cas_NaPpvGetDefaultFocus_Mediaset(void);
STATIC HINT32 xmgr_cas_NaPpvConvertObjIdToItemIdx_Mediaset(HINT32 nObjId);
STATIC HINT32 xmgr_cas_NaPpvConvertItemIdxToObjId_Mediaset(HINT32 nItemIdx);
STATIC BUS_Result_t xmgr_cas_NaPpvInitItemCardInfo_Mediaset(SettingsNaviContext_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvInitItemSelProg_Mediaset(SettingsNaviContext_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvInitItemRechargeHistory_Mediaset(SettingsNaviContext_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvInitItemPackagePurchase_Mediaset(SettingsNaviContext_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvInitMediasetItems_Mediaset(SettingsNaviContext_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvDrawBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvGetCurrentTime_Mediaset(UNIXTIME *pulCurrentTime);
STATIC BUS_Result_t xmgr_cas_NaPpvDrawCurrentDate_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvDrawUpDownArrow_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvDrawOKButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvDrawBackButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);

#define	_________________________BODY______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvGetLocalContents_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC inline SettingsNaviContext_t * xmgr_cas_NaPpvGetLocalContents_Mediaset(void)
{
	return &s_stApNaPpvMenuMediaset;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvGetDefaultFocus_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC inline HINT32 xmgr_cas_NaPpvGetDefaultFocus_Mediaset(void)
{
	return eNaPpvMediasetItemIdx_ScInfo;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvConvertObjIdToItemIdx_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC HINT32 xmgr_cas_NaPpvConvertObjIdToItemIdx_Mediaset(HINT32 nObjId)
{
	HINT32	nItemId = 0;

	nItemId = (nObjId & ~eSetting_PpvMediaset);

	if((nItemId < 0) || (nItemId >= eNaPpvMediasetItemIdx_MAX))
	{
		return -1;
	}

	return nItemId;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvConvertItemIdxToObjId_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC HINT32 xmgr_cas_NaPpvConvertItemIdxToObjId_Mediaset(HINT32 nItemIdx)
{
	if((nItemIdx < 0) || (nItemIdx >= eNaPpvMediasetItemIdx_MAX))
	{
		return 0;
	}

	return (eSetting_PpvMediaset | nItemIdx);
}

/********************************************************************
 Function		: xmgr_cas_NaPpvConvertItemIdxToObjId_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvInitItemCardInfo_Mediaset(SettingsNaviContext_t *pContents)
{
	SettingsNaviMainItem_t		stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = xmgr_cas_NaPpvConvertItemIdxToObjId_Mediaset(eNaPpvMediasetItemIdx_ScInfo);

	/* set title, Loc Id */
	stMainItem.nTitleLocId = LOC_CAS_NAGRA_MEDIASET_CARD_INFORMATION_TEXT_ID;
	MWC_UTIL_DvbStrcpy(stMainItem.title, GetStrRsc(LOC_CAS_NAGRA_MEDIASET_CARD_INFORMATION_TEXT_ID));

	/*
		Function to launch :
		Arguments: nothing
	*/
	MWC_UTIL_DvbStrcpy (stMainItem.appName, (HUINT8 *)"xmgr_cas_NaMSET_SmartCardInfo_Proc");
	stMainItem.appToLaunch = xmgr_cas_NaMSET_SmartCardInfo_Proc;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem (pContents, &stMainItem);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvInitItemSelProg_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvInitItemSelProg_Mediaset(SettingsNaviContext_t *pContents)
{
	SettingsNaviMainItem_t		stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = xmgr_cas_NaPpvConvertItemIdxToObjId_Mediaset(eNaPpvMediasetItemIdx_SelProg);

	/* set title, Loc Id */
	stMainItem.nTitleLocId = LOC_CAS_NAGRA_MEDIASET_SELETED_PROGRAMS_TEXT_ID;
	MWC_UTIL_DvbStrcpy(stMainItem.title, GetStrRsc(LOC_CAS_NAGRA_MEDIASET_SELETED_PROGRAMS_TEXT_ID));

	/*
		Function to launch :
		Arguments: nothing
	*/
	MWC_UTIL_DvbStrcpy (stMainItem.appName, (HUINT8 *)"xmgr_cas_NaMSET_SeletedPrograms_Proc");
	stMainItem.appToLaunch = xmgr_cas_NaMSET_SeletedPrograms_Proc;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem (pContents, &stMainItem);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvInitItemRechargeHistory_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvInitItemRechargeHistory_Mediaset(SettingsNaviContext_t *pContents)
{
	SettingsNaviMainItem_t		stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = xmgr_cas_NaPpvConvertItemIdxToObjId_Mediaset(eNaPpvMediasetItemIdx_RechargeHistory);

	/* set title, Loc Id */
	stMainItem.nTitleLocId = LOC_CAS_NAGRA_MEDIASET_RECHARGE_HISTORY_TEXT_ID;
	MWC_UTIL_DvbStrcpy(stMainItem.title, GetStrRsc(LOC_CAS_NAGRA_MEDIASET_RECHARGE_HISTORY_TEXT_ID));

	/*
		Function to launch :
		Arguments: nothing
	*/
	MWC_UTIL_DvbStrcpy (stMainItem.appName, (HUINT8 *)"xmgr_cas_NaMSET_RechargeHistory_Proc");
	stMainItem.appToLaunch = xmgr_cas_NaMSET_RechargeHistory_Proc;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem (pContents, &stMainItem);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvInitItemPackagePurchase_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvInitItemPackagePurchase_Mediaset(SettingsNaviContext_t *pContents)
{
	SettingsNaviMainItem_t		stMainItem = {0, };

	AP_NaviCntxt_InitNaviMainItem (&stMainItem);

	/* Set the object ID */
	stMainItem.mainBtnId = xmgr_cas_NaPpvConvertItemIdxToObjId_Mediaset(eNaPpvMediasetItemIdx_PackagePurchase);

	/* set title, Loc Id */
	stMainItem.nTitleLocId = LOC_CAS_NAGRA_MEDIASET_PACKAGE_PURCHASE_TEXT_ID;
	MWC_UTIL_DvbStrcpy(stMainItem.title, GetStrRsc(LOC_CAS_NAGRA_MEDIASET_PACKAGE_PURCHASE_TEXT_ID));

	/*
		Function to launch :
		Arguments: nothing
	*/
	MWC_UTIL_DvbStrcpy (stMainItem.appName, (HUINT8 *)"xmgr_cas_NaMSET_PackagePurchase_Proc");
	stMainItem.appToLaunch = xmgr_cas_NaMSET_PackagePurchase_Proc;

	/* Register the main item to the contents */
	AP_NaviCntxt_AppendMainItem (pContents, &stMainItem);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvInitMediasetItems_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvInitMediasetItems_Mediaset(SettingsNaviContext_t *pContents)
{
	HERROR		hErr;

	/* param check */
	if(pContents == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	/* alloc mainItems, mainItem has each infos of sub menu to launch */
	hErr = AP_NaviCntxt_InitMainItemArray (pContents);
	if ( hErr != ERR_OK )
	{
		HxLOG_Critical("\n\n");
		return hErr;
	}

	/* settings default focus는 제일 첫 아이템 */
	pContents->nDefaultFocusIdx = xmgr_cas_NaPpvGetDefaultFocus_Mediaset();
	pContents->pConvFunc = xmgr_cas_NaPpvConvertObjIdToItemIdx_Mediaset;

	/* Make SubItems */
	xmgr_cas_NaPpvInitItemCardInfo_Mediaset(pContents);
	xmgr_cas_NaPpvInitItemSelProg_Mediaset(pContents);
	xmgr_cas_NaPpvInitItemRechargeHistory_Mediaset(pContents);
	xmgr_cas_NaPpvInitItemPackagePurchase_Mediaset(pContents);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvDrawBackgroundImage_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvDrawBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
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
	if (AP_StockImg_GetImage(e914_1_24_BG, &img) == ERR_OK)
	{
		OSD_GFX_FB_Copy((unsigned long)img.hBuffer, img.width, img.height, pScreen, pRect);
	}

	return ERR_BUS_NO_ERROR;
}


/********************************************************************
 Function		: xmgr_cas_NaPpvGetCurrentTime_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvGetCurrentTime_Mediaset(UNIXTIME *pulCurrentTime)
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
 Function		: xmgr_cas_NaPpvDrawCurrentDate_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvDrawCurrentDate_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
	xmgr_cas_NaPpvGetCurrentTime_Mediaset((UNIXTIME*)&ulSystemTime);

	hError = HLIB_DATETIME_ConvertUnixTimeToDateTime(ulSystemTime, &stDateTime);
	if(hError != ERR_OK)
	{
		HxLOG_Print("[xmgr_cas_NaPpvDrawCurrentDate_Mediaset] ConvertUnixTimeToDateTime fail!!\n");
		return ERR_FAIL;
	}

	sprintf(szDate, "%s: %02d/%02d/%04d", GetStrRsc(LOC_CAS_NAGRA_MEDIASET_DATE_TEXT_ID), stDateTime.stDate.ucDay, stDateTime.stDate.ucMonth, stDateTime.stDate.usYear);
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_S, pRect->x, pRect->y, pRect->w,
										(HINT8 *)szDate, COL(C_T_Cursor));

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvDrawUpDownArrow_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvDrawUpDownArrow_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e914_2_24_Arrow_W_3UD, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvDrawOKButton_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvDrawOKButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e914_1_24_BTN_1OK, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvDrawBackButton_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvDrawBackButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e914_1_24_BTN_2Back, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvDrawNaviMainItemMethod_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvDrawNaviMainItemMethod_Mediaset(OSD_Screen_t *pOsdScreen, OSD_Area_t *appArea,
							OSD_Rect_t *objRect, GWM_GuiObject_t pObject, void *pArg, int bUiTop)
{
	HINT32	x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	HINT32	x = 0, y = 0, w = 0, h = 0;
	HINT32	text_x_offset = 0, text_y_offset = 0;
	HINT32	stringPixelLength = 0;
	HUINT32	status = 0;
	HUINT32	fontSize = 0;

	HINT8	*title = NULL;
	HINT8	*value_text = NULL;
	HINT32	ulIndex=0;

	OSD_Font_t		*pFont = NULL;
	OSD_Color_t		fontcolor = 0x00000000;
	OSD_Color_t		value_fontcolor = 0;
	BUTTON_OBJECT	btn = (BUTTON_OBJECT)pArg;

	OSD_StockImage_t	stNumImage;
	OSD_Rect_t			ImageRect;

	SettingsNaviContext_t	*pContents = NULL;

	/* for compiler */
	(void)pObject;

	/* init coordinate */
	x1 = appArea->x1 + objRect->x;
	y1 = appArea->y1 + objRect->y;
	x2 = appArea->x1 + objRect->x + objRect->w - 1;
	y2 = appArea->y1 + objRect->y + objRect->h - 1;
	w = objRect->w;
	h = objRect->h;

	pContents = xmgr_cas_NaPpvGetLocalContents_Mediaset();
	ulIndex = pContents->nCurFocus;

	/* get text */
	title = GWM_Button_GetTitlePtr(btn);
	if(title == NULL)
	{
		HxLOG_Critical("\n\n");

		return ERR_FAIL;
	}

	value_text = GWM_Button_GetExtraTextPtr(btn);

	status = GWM_Button_GetStatus(btn);

	/* basic font color */
	fontSize = FONT_SIZE_M;
	fontcolor = COL (C_T_Cursor);
	value_fontcolor = COL (C_T_Cursor);

	/* app is top most */
	if (bUiTop > 0)
	{
		if (status & GUIOBJ_HILIGHTED)
		{
			/* cursor */
			fontcolor = COL (C_T_Cursor);
			value_fontcolor = COL (C_T_Cursor);

			/* cursor number image */
			OSD_GFX_SetRect(&ImageRect, s_Mediaset_NaviMainItem[ulIndex].rect.x, s_Mediaset_NaviMainItem[ulIndex].rect.y, s_Mediaset_NaviMainItem[ulIndex].rect.w, s_Mediaset_NaviMainItem[ulIndex].rect.h);
			if (!AP_StockImg_GetImage (e914_5_24_M_4Cursor, &stNumImage))
			{
				if(s_Mediaset_NaviMainItem[ulIndex].Id == NAPPV_MEDIASET_BTN01_ID)
				{
					OSD_GFX_FB_Blend (stNumImage.hBuffer, stNumImage.width, stNumImage.height, pOsdScreen, &ImageRect);
				}
				else if(s_Mediaset_NaviMainItem[ulIndex].Id == NAPPV_MEDIASET_BTN02_ID)
				{
					OSD_GFX_FB_Blend (stNumImage.hBuffer, stNumImage.width, stNumImage.height, pOsdScreen, &ImageRect);
				}
				else if(s_Mediaset_NaviMainItem[ulIndex].Id == NAPPV_MEDIASET_BTN03_ID)
				{
					OSD_GFX_FB_Blend (stNumImage.hBuffer, stNumImage.width, stNumImage.height, pOsdScreen, &ImageRect);
				}
				else if(s_Mediaset_NaviMainItem[ulIndex].Id == NAPPV_MEDIASET_BTN04_ID)
				{
					OSD_GFX_FB_Blend (stNumImage.hBuffer, stNumImage.width, stNumImage.height, pOsdScreen, &ImageRect);
				}
			}
		}
	}

	/* custom value for subitem text for right alignment*/
	/* move x coordinate to text position */
	text_x_offset = 10; //48;
	text_y_offset = 0;

	pFont = OSD_FONT_Get(eFont_SystemNormal);
	if((pFont != NULL) && (!OSD_FONT_SetSize(pFont, fontSize)))
	{
		HUINT32	hh = 0;
		OSD_FONT_GetFontHeight(pFont, &hh);

		/* cursor objRect->y + 5 => text objRect y */
		y = y1 + text_y_offset + ((NAPPV_MEDIASET_BTN_HEIGHT - hh)>>1);		/* y-axis mid-aligned */

		x = x1 + text_x_offset;
		OSD_GFX_DrawStringWithAbbr(pOsdScreen, pFont, x, y, w, (HINT8 *)title, fontcolor);

		text_x_offset += 10;

		/* for abbreviation treat */
		stringPixelLength = OSD_GFX_GetStringWidth(pFont, (HINT8 *)value_text);
		x = x1 + w - stringPixelLength - text_x_offset;	/* x-axis right- aligned */
		OSD_GFX_DrawStringWithAbbr(pOsdScreen, pFont, x, y, w, (HINT8 *)value_text, value_fontcolor);
	}
	if (pFont != NULL)
		OSD_FONT_Release(pFont);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvInitNaviMainItems_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvInitNaviMainItems_Mediaset(SettingsNaviMainItem_t *mainItemList, HUINT32 maxMainItem, GWM_ObjDraw_t drawMethod)
{
	HUINT32					i = 0;
	OSD_Rect_t				rect = {0, };
	SettingsNaviMainItem_t	*pMainItem = NULL;

	/* init subitems until max count */
	for(i = 0; i < maxMainItem; i++)
	{
		pMainItem = &(mainItemList[i]);
		//pMainItem->mainBtnId = s_NaviMainItem[i].Id;
		OSD_GFX_SetRect(&rect, s_Mediaset_NaviMainItem[i].rect.x, s_Mediaset_NaviMainItem[i].rect.y, s_Mediaset_NaviMainItem[i].rect.w, s_Mediaset_NaviMainItem[i].rect.h);
		GWM_Button_Create(pMainItem->mainBtnId, &rect, (HINT8 *)pMainItem->title);
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
 Function		: xmgr_cas_NaPpvDrawDefaultUI_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvDrawDefaultUI_Mediaset(SettingsNaviContext_t *pContents)
{
	OSD_Rect_t	rect;

	/* Title */
	MWC_UTIL_DvbStrcpy(pContents->title ,(HUINT8 *)GetStrRsc(LOC_CAS_MEDIASET_PREMIUM_TEXT_ID));
	Settings_SetWindowTitle(SETTING_GET_AP_DEPTH(eSetting_PpvMediaset), LOC_CAS_MEDIASET_PREMIUM_TEXT_ID, pContents->title);

	/* na ppv mediaset background */
	Settings_DrawBack(FALSE);
	OSD_GFX_SetRect(&rect, 0, 0, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT);
	GWM_Rect_Create(NAPPV_MEDIASET_LOGOBACKGROUND_IMAGE_ID, &rect, 0x00000000);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_LOGOBACKGROUND_IMAGE_ID, xmgr_cas_NaPpvDrawBackgroundImage_Mediaset);

	/* na ppv mediaset date text*/
	OSD_GFX_SetRect (&rect, NAPPV_MEDIASET_DATE_TEXT_X, NAPPV_MEDIASET_DATE_TEXT_Y, NAPPV_MEDIASET_DATE_TEXT_WIDTH, NAPPV_MEDIASET_DATE_TEXT_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_DATE_TEXT_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_DATE_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvDrawCurrentDate_Mediaset);

	/* na ppv mediaset arrow */
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_ARROW_BTN_X, NAPPV_MEDIASET_ARROW_BTN_Y, NAPPV_MEDIASET_ARROW_BTN_WIDTH, NAPPV_MEDIASET_ARROW_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_ARROW_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_ARROW_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvDrawUpDownArrow_Mediaset);

	/* na ppv mediaset button */
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_OK_BTN_X, NAPPV_MEDIASET_OK_BTN_Y, NAPPV_MEDIASET_OK_BTN_WIDTH, NAPPV_MEDIASET_OK_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_OK_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_OK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvDrawOKButton_Mediaset);

	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_BACK_BTN_X, NAPPV_MEDIASET_BACK_BTN_Y, NAPPV_MEDIASET_BACK_BTN_WIDTH, NAPPV_MEDIASET_BACK_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_BACK_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_BACK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvDrawBackButton_Mediaset);
	/* init mainItems */
	xmgr_cas_NaPpvInitNaviMainItems_Mediaset(pContents->mainItems, pContents->ulNumOfMainItem, (GWM_ObjDraw_t)xmgr_cas_NaPpvDrawNaviMainItemMethod_Mediaset);

	/* init sub items */
	Settings_InitNaviSubItemList(pContents->mainItems, pContents->nCurFocus, TRUE);

	/* Set current position */
	GWM_Obj_SetFocus(pContents->mainItems[pContents->nCurFocus].mainBtnId);

	return ERR_BUS_NO_ERROR;
}

#define ________XMGR_CAS_NA_PPV_MEDIASET_MENU_PUBLIC___________________________________________
/********************************************************************
 Function		: xproc_cas_NaPpvMenu_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
BUS_Result_t xproc_cas_NaPpvMenu_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32					nResult = ERR_FAIL;
	SettingsNaviContext_t		*pContents = xmgr_cas_NaPpvGetLocalContents_Mediaset();

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
					if (!xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(eNAGRA_OP_TYPE_MEDIASET))
					{
						HxLOG_Print("xmgr_cas_NaMatchCardStatusWithOpType_Mediaset FALSE!!!\n");
						eAlarmType = eCAS_NA_CA_SMARTCARD_INVALID;
					}
				}

				if (eAlarmType != eCAS_NA_CA_SMARTCARD_OK && eAlarmType != eCAS_NA_CA_SMARTCARD_EXPIRED)
				{
					BUS_MGR_Destroy(NULL);

					/* to upper depth */
					BUS_MGR_Create("xmgr_cas_NaMSET_MenuSmartCardError_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (GWM_Callback_t)xmgr_cas_NaMSET_MenuSmartCardError_Proc, 0, eAlarmType, 0, 0);

					return MESSAGE_BREAK;
				}
			}
#endif

			/* init application area */
			GWM_APP_SetAppArea(NAPPV_MEDIASET_WINDOW_X, NAPPV_MEDIASET_WINDOW_Y,
								NAPPV_MEDIASET_WINDOW_WIDTH, NAPPV_MEDIASET_WINDOW_HEIGHT);

			/* Initialiaze Items */
			nResult = xmgr_cas_NaPpvInitMediasetItems_Mediaset(pContents);
			if(nResult != ERR_OK)			HxLOG_Critical("Critical Eror!!!\n");

			/* Draw UI */
			xmgr_cas_NaPpvDrawDefaultUI_Mediaset(pContents);

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
					BUS_MGR_Create("xmgr_cas_NaMSET_MenuSmartCardError_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (GWM_Callback_t)xmgr_cas_NaMSET_MenuSmartCardError_Proc, 0, eAlarmType, 0, 0);

					return MESSAGE_PASS;
				}
				else
				{
					// Card 정상 삽입 상태인데.. 이쪽으로 왜 올까?
				}
			}
			break;

		case MSG_GWM_OBJ_FOCUS_CHANGED:
			Settings_Navi_ReflectFocus(pContents, p1, FALSE, FindFocus_Current);
			return MESSAGE_BREAK;

		case MSG_GWM_SCROLLRIGHT:
		case MSG_GWM_CLICKED:
		{
			SettingsNaviMainItem_t mainItem;

			if(pContents->mainItems[pContents->nCurFocus].appToLaunch != NULL)
			{
				HxSTD_memcpy(&mainItem, &(pContents->mainItems[pContents->nCurFocus]), sizeof(SettingsNaviMainItem_t));
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

			nResult = Settings_CheckKeyPass(p1, BUS_MGR_GetAppCallback(BUS_MGR_GetThis()));
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
					//Settings_InitFocus(pstContents);

					BUS_MGR_Destroy(NULL);

					/* to upper depth */
					BUS_MGR_Create("AP_CAS_PpvMenu_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (GWM_Callback_t)AP_CAS_PpvMenu_Proc, 0, 0, 0, 0);
					return MESSAGE_BREAK;

				case KEY_1:
				case KEY_2:
				case KEY_3:
				case KEY_4:
				case KEY_0:
				case KEY_5:
				case KEY_6:
				case KEY_7:
				case KEY_8:
				case KEY_9:
					AP_NotAvailable_Show();
					return MESSAGE_BREAK;

				default:
					break;
			}
			break;

		/* wrap around */
		case MSG_GWM_SCROLLUP:
			/* wrap around */
			Settings_Navi_ReflectFocus(pContents, p1, TRUE, FindFocus_Up);
			return MESSAGE_BREAK;

		case MSG_GWM_SCROLLDOWN:
			/* wrap around */
			Settings_Navi_ReflectFocus(pContents, p1, TRUE, FindFocus_Down);
			return MESSAGE_BREAK;

		case MSG_GWM_TIMER:
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			//AP_SPEC_ClearMenuPanelString();
			break;

		default:
			break;
	}

	HxLOG_Info("xproc_cas_NaPpvMenu_Mediaset handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif	//end of #if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
/* End of File */
