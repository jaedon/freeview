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
#include "ap_cas_ppv_menu.h"
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
	eCasNaPgeaPurcProd_Name,
	eCasNaPgeaPurcProd_ExpiryDate,
	eCasNaPgeaPurcProd_Credit,
	eCasNaPgeaPurcProd_Item_Max
} XmgrCas_NaPurcProdItem_e;

#define	MAX_PGEA_PURCPROD_LIST_ROW_NUM					4
#define	MAX_PGEA_PURCPROD_LIST_EVTNAME_LENGTH			50

typedef struct tagPurcProductsSubItem
{
	TChar					PurcProdEvtName[MAX_PGEA_PURCPROD_LIST_EVTNAME_LENGTH+1];
	TCalendarTime			PurcProdPurchaseDate;
	TCreditAmount			PurcProdCredit;
} XmgrCas_NaPurcProdUiSubItem_t;

typedef struct tag_CAS_NA_PANGEA_PP_Contents
{
	/**@breif Specific value */
	HUINT8					ucTitle[MAX_ELEMENT_TEXT_LENGTH]; 					///< title of app context
	HUINT8					ucCurDate[32];
	HUINT8					ucPageNum[16];
	HUINT8					ucCreditStr[16];

	HINT32					ulNumOfUiSubItemEvents; /*item 갯수*/
	HINT32					ulCurPage;
	HBOOL					bWaiting;

	POINTER_LIST_T			*pstPurchaseHistoryListItem;
} XmgrCas_NaPurcProd_Pangea_t;

#define	NAPPV_PANGEA_PURCPROD_WINDOW_WIDTH						OSD_SCREEN_WIDTH
#define	NAPPV_PANGEA_PURCPROD_WINDOW_HEIGHT						OSD_SCREEN_HEIGHT
#define	NAPPV_PANGEA_PURCPROD_WINDOW_X							0
#define	NAPPV_PANGEA_PURCPROD_WINDOW_Y							0
#define	NAPPV_PANGEA_PURCPROD_STRING_MARGIN_Y					18

#define	NAPPV_PANGEA_PURCPROD_APP_ID							eCasNaPgeaPurcProd_Name

/* ppv pangea purcprod background */
#define	NAPPV_PANGEA_PURCPROD_LOGOBACKGROUND_IMAGE_ID			(NAPPV_PANGEA_PURCPROD_APP_ID|0x0010)

/* ppv pangea purcprod date */
#define	NAPPV_PANGEA_PURCPROD_DATE_TEXT_ID						(NAPPV_PANGEA_PURCPROD_APP_ID|0x0020)
#define	NAPPV_PANGEA_PURCPROD_DATE_TEXT_X						780
#define	NAPPV_PANGEA_PURCPROD_DATE_TEXT_Y						(196-FONT_SIZE_XS)
#define	NAPPV_PANGEA_PURCPROD_DATE_TEXT_WIDTH					300
#define	NAPPV_PANGEA_PURCPROD_DATE_TEXT_HEIGHT					31

/* ppv pangea purcprod info text */
#define	NAPPV_PANGEA_PURCPROD_INFO_TEXT_ID						(NAPPV_PANGEA_PURCPROD_APP_ID|0x0030)
#define	NAPPV_PANGEA_PURCPROD_INFO_TEXT_X						310
#define	NAPPV_PANGEA_PURCPROD_INFO_TEXT_Y						(196 - FONT_SIZE_S)
#define	NAPPV_PANGEA_PURCPROD_INFO_TEXT_WIDTH					500
#define	NAPPV_PANGEA_PURCPROD_INFO_TEXT_HEIGHT					31

/* ppv pangea purcprod state button */
#define	NAPPV_PANGEA_PURCPROD_STATE_BTN_ID						(NAPPV_PANGEA_PURCPROD_APP_ID|0x0040)
#define	NAPPV_PANGEA_PURCPROD_STATE_BTN_X						636
#define	NAPPV_PANGEA_PURCPROD_STATE_BTN_Y						497
#define	NAPPV_PANGEA_PURCPROD_STATE_BTN_WIDTH					644
#define	NAPPV_PANGEA_PURCPROD_STATE_BTN_HEIGHT					66

/* ppv pangea purcprod credit */
#define	NAPPV_PANGEA_PURCPROD_CREDIT_TEXT_ID					(NAPPV_PANGEA_PURCPROD_APP_ID|0x0050)
#define	NAPPV_PANGEA_PURCPROD_CREDIT_TEXT_X						689
#define	NAPPV_PANGEA_PURCPROD_CREDIT_TEXT_Y						537
#define	NAPPV_PANGEA_PURCPROD_CREDIT_TEXT_WIDTH					450
#define	NAPPV_PANGEA_PURCPROD_CREDIT_TEXT_HEIGHT				31

/* ppv pangea purcprod arrow button */
#define	NAPPV_PANGEA_PURCPROD_ARROW_BTN_ID						(NAPPV_PANGEA_PURCPROD_APP_ID|0x0070)
#define	NAPPV_PANGEA_PURCPROD_ARROW_BTN_X						140
#define	NAPPV_PANGEA_PURCPROD_ARROW_BTN_Y						609
#define	NAPPV_PANGEA_PURCPROD_ARROW_BTN_WIDTH					15
#define	NAPPV_PANGEA_PURCPROD_ARROW_BTN_HEIGHT					31

/* ppv pangea purcprod event Info list page */
#define	NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_ID					(NAPPV_PANGEA_PURCPROD_APP_ID|0x0080)
#define	NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_X					167
#define	NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_Y					635
#define	NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_WIDTH				100
#define	NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_HEIGHT				41

/* ppv pangea purcprod back button */
#define	NAPPV_PANGEA_PURCPROD_BACK_BTN_ID						(NAPPV_PANGEA_PURCPROD_APP_ID|0x0090)
#define	NAPPV_PANGEA_PURCPROD_BACK_BTN_X						450
#define	NAPPV_PANGEA_PURCPROD_BACK_BTN_Y						607
#define	NAPPV_PANGEA_PURCPROD_BACK_BTN_WIDTH					86
#define	NAPPV_PANGEA_PURCPROD_BACK_BTN_HEIGHT					44

#define	NAPPV_PANGEA_PURCPROD_BACK_TEXT_ID						(NAPPV_PANGEA_PURCPROD_APP_ID|0x00a0)
#define	NAPPV_PANGEA_PURCPROD_BACK_TEXT_X						543
#define	NAPPV_PANGEA_PURCPROD_BACK_TEXT_Y						(636 - FONT_SIZE_XS)
#define	NAPPV_PANGEA_PURCPROD_BACK_TEXT_WIDTH					200
#define	NAPPV_PANGEA_PURCPROD_BACK_TEXT_HEIGHT					44

#define	NAPPV_PANGEA_PURCPROD_STATUS_ID							(NAPPV_PANGEA_PURCPROD_APP_ID|0x00b0) //update 시 처리 부분.....

#define __________SUBITEM_OBJECTS_________________________________________
#define	NAPPV_PANGEA_PURCPROD_SUBBTN_ID							(NAPPV_PANGEA_PURCPROD_APP_ID|0x0200)
#define	NAPPV_PANGEA_PURCPROD_SUBBTN_WIDTH						939
#define	NAPPV_PANGEA_PURCPROD_SUBBTN_HEIGHT						55
#define	NAPPV_PANGEA_PURCPROD_SUBBTN_X							141
#define	NAPPV_PANGEA_PURCPROD_SUBBTN_Y							(295 - 55)
#define	NAPPV_PANGEA_PURCPROD_SUBBTN_LINESPACE					55

/* ppv pangea purcprod btn 01 position */
#define	NAPPV_PANGEA_PURCPROD_SUBBTN01_ID						(NAPPV_PANGEA_PURCPROD_APP_ID|0x0210)
#define	NAPPV_PANGEA_PURCPROD_SUBBTN01_WIDTH					380
#define	NAPPV_PANGEA_PURCPROD_SUBBTN01_HEIGHT					NAPPV_PANGEA_PURCPROD_SUBBTN_HEIGHT
#define	NAPPV_PANGEA_PURCPROD_SUBBTN01_X						141
#define	NAPPV_PANGEA_PURCPROD_SUBBTN01_Y						295

/* ppv pangea purcprod btn 02 position */
#define	NAPPV_PANGEA_PURCPROD_SUBBTN02_ID						(NAPPV_PANGEA_PURCPROD_APP_ID|0x0220)
#define	NAPPV_PANGEA_PURCPROD_SUBBTN02_WIDTH					300
#define	NAPPV_PANGEA_PURCPROD_SUBBTN02_HEIGHT					NAPPV_PANGEA_PURCPROD_SUBBTN_HEIGHT
#define	NAPPV_PANGEA_PURCPROD_SUBBTN02_X						570
#define	NAPPV_PANGEA_PURCPROD_SUBBTN02_Y						295

/* ppv pangea purcprod btn 03 position */
#define	NAPPV_PANGEA_PURCPROD_SUBBTN03_ID						(NAPPV_PANGEA_PURCPROD_APP_ID|0x0230)
#define	NAPPV_PANGEA_PURCPROD_SUBBTN03_WIDTH					160
#define	NAPPV_PANGEA_PURCPROD_SUBBTN03_HEIGHT					NAPPV_PANGEA_PURCPROD_SUBBTN_HEIGHT
#define	NAPPV_PANGEA_PURCPROD_SUBBTN03_X						920
#define	NAPPV_PANGEA_PURCPROD_SUBBTN03_Y						295

#define	NAPPV_PANGEA_PURCPROD_NO_ITEM_TEXT						(NAPPV_PANGEA_PURCPROD_APP_ID|0x0240)

#define NAPPV_PANGEA_PURCPROD_TS_ERROR_TEXT_ID					(NAPPV_PANGEA_PURCPROD_APP_ID|0x0250)
#define NAPPV_PANGEA_PURCPROD_TS_ERROR_TEXT_X					271
#define NAPPV_PANGEA_PURCPROD_TS_ERROR_TEXT_Y					295
#define NAPPV_PANGEA_PURCPROD_TS_ERROR_TEXT_WIDTH				679
#define NAPPV_PANGEA_PURCPROD_TS_ERROR_TEXT_HEIGHT				236

#define NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_ID					(NAPPV_PANGEA_PURCPROD_APP_ID|0x0260)
#define NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_X					450
#define NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_Y					607
#define NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_WIDTH				86
#define NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HEIGHT				44

#define NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HELP_ID				(NAPPV_PANGEA_PURCPROD_APP_ID|0x0270)
#define NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HELP_X				543
#define NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HELP_Y				(636-FONT_SIZE_XS)
#define NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HELP_WIDTH			200
#define NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HELP_HEIGHT			44

#define NAPPV_PANGEA_PURCPROD_PLEASE_WAIT_ID					(NAPPV_PANGEA_PURCPROD_APP_ID|0x0280)
#define	NAPPV_PANGEA_PURCPROD_PLEASE_WAIT_WIDTH					853
#define	NAPPV_PANGEA_PURCPROD_PLEASE_WAIT_HEIGHT				34
#define	NAPPV_PANGEA_PURCPROD_PLEASE_WAIT_X						141
#define	NAPPV_PANGEA_PURCPROD_PLEASE_WAIT_Y						(350 - FONT_SIZE_S)


STATIC Item_t s_Pangea_PurcProd_NaviSubItem[eCasNaPgeaPurcProd_Item_Max] =
{
/* 01 */	{NAPPV_PANGEA_PURCPROD_SUBBTN01_ID, { NAPPV_PANGEA_PURCPROD_SUBBTN01_X, NAPPV_PANGEA_PURCPROD_SUBBTN01_Y, NAPPV_PANGEA_PURCPROD_SUBBTN01_WIDTH, NAPPV_PANGEA_PURCPROD_SUBBTN01_HEIGHT}},
/* 02 */	{NAPPV_PANGEA_PURCPROD_SUBBTN02_ID, { NAPPV_PANGEA_PURCPROD_SUBBTN02_X, NAPPV_PANGEA_PURCPROD_SUBBTN02_Y, NAPPV_PANGEA_PURCPROD_SUBBTN02_WIDTH, NAPPV_PANGEA_PURCPROD_SUBBTN02_HEIGHT}},
/* 03 */	{NAPPV_PANGEA_PURCPROD_SUBBTN03_ID, { NAPPV_PANGEA_PURCPROD_SUBBTN03_X, NAPPV_PANGEA_PURCPROD_SUBBTN03_Y, NAPPV_PANGEA_PURCPROD_SUBBTN03_WIDTH, NAPPV_PANGEA_PURCPROD_SUBBTN03_HEIGHT}},
};

#ifdef WIN32
#define	MAX_PGEA_PURCPROD_LIST_EVTITEM_NUM				20

STATIC XmgrCas_NaPurcProdUiSubItem_t s_stPurcProdListSubItem[MAX_PGEA_PURCPROD_LIST_EVTITEM_NUM] =
{
	{	"1PURCPROD SPORT MALAN",		{0x09, 0x09, 0x1E, 0, 0, 0},	0x0001	},
	{	"2PURCPROD SPORT JUVE",			{0x09, 0x09, 0x1C, 0, 0, 0},	0x0003	},
	{	"3PURCPROD SPORT INTER",		{0x09, 0x09, 0x1B, 0, 0, 0},	0x0005	},
	{	"4PURCPROD SPORT MCRO",			{0x09, 0x09, 0x1A, 0, 0, 0},	0x0007	},
	{	"5PURCPROD SPORT ITEFT",		{0x09, 0x09, 0x19, 0, 0, 0},	0x0009	},

	{	"6PURCPROD CINAMA MALAN",		{0x09, 0x09, 0x18, 0, 0, 0},	0x000a	},
	{	"7PURCPROD CINAMA JUVE",		{0x09, 0x09, 0x17, 0, 0, 0},	0x000b	},
	{	"8PURCPROD CINAMA INTER",		{0x09, 0x09, 0x16, 0, 0, 0},	0x000c	},
	{	"9PURCPROD CINAMA MCRO",		{0x09, 0x09, 0x15, 0, 0, 0},	0x000d	},
	{	"10PURCPROD CINAMA ITEFT",		{0x09, 0x09, 0x14, 0, 0, 0},	0x000e	},

	{	"11PURCPROD OTHER MALAN",		{0x09, 0x09, 0x13, 0, 0, 0},	0x000f	},
	{	"12PURCPROD OTHER JUVE",		{0x09, 0x09, 0x12, 0, 0, 0},	0x0010	},
	{	"13PURCPROD OTHER INTER",		{0x09, 0x09, 0x11, 0, 0, 0},	0x0011	},
	{	"14PURCPROD OTHER MCRO",		{0x09, 0x09, 0x10, 0, 0, 0},	0x0012	},
	{	"15PURCPROD OTHER ITEFT",		{0x09, 0x09, 0x0F, 0, 0, 0},	0x0013	},

	{	"16PURCPROD 1111111111",		{0x09, 0x09, 0x0E, 0, 0, 0},	0x0014	},
	{	"17PURCPROD 2222222222",		{0x09, 0x09, 0x0C, 0, 0, 0},	0x0015	},
	{	"18PURCPROD 3333333333",		{0x09, 0x09, 0x0B, 0, 0, 0},	0x0016	},
	{	"19PURCPROD 4444444444",		{0x09, 0x09, 0x0A, 0, 0, 0},	0x0017	},
	{	"20PURCPROD 5555555555",		{0x09, 0x09, 0x09, 0, 0, 0},	0x0018	},
};
#endif

//******************************************************************************
//	STATIC VARIABLES
//******************************************************************************
STATIC XmgrCas_NaPurcProd_Pangea_t 			s_stNaPgeaPurcProd;


//******************************************************************************
//	PROTOTYPE FUNCTIONS
//******************************************************************************
#define ________XMGR_CAS_NA_PpvPANGEA_MENU_STATIC___________________________________________
STATIC XmgrCas_NaPurcProd_Pangea_t* xmgr_cas_NaPpvPurcprodGetLocalContents_Pangea(void);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodUpdateRemainedCredit_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents, TCreditAmount nCredit);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodSetMainTitle_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodScrollPage_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents, KEY_Code_t eKeyCode);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodInitUiConfiguration_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodRequestPurchaseHistory_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents);
STATIC XmgrCas_NaPurcProdUiSubItem_t *xmgr_cas_NaPpvPurcprodLinkTraverseListItembyIndex_Pangea (POINTER_LIST_T **pRoot, HUINT16 usTotalItemIdx, HUINT16 usItemIdx);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodDrawPurchasedProductsItem_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t tObject, void *pvArg, int nParam1);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodDrawBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodGetPageStr_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodDrawCredit_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodDrawCreditBg_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodDrawUpDownArrow_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodDrawBackButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodSetUiObjects_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodDestroyUI_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents);
STATIC void			xmgr_cas_NaPpvPurcprodLinkFreeListItemMethod_Pangea(void *pvContents);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodFreePurchaseHistory_Pangea(POINTER_LIST_T **pRoot);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodUpdatePurchaseHistory_Pangea(HUINT32 xStatus, HUINT32 xNumberOfItems, const PPV_PURCHASE_LIST_ITEM* pxPurchaseList, XmgrCas_NaPurcProd_Pangea_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodEvtlistDrawTsErrorMessage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodEvtlistDrawTsErrorExitBtn_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);

#define ________XMGR_CAS_NA_PpvPANGEA_MENU_PUBLIC___________________________________________
BUS_Result_t xproc_cas_NaPurchasedProducts_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#define	______
#define	_________________________STATIC_BODY______________________________
#define	______
/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodGetLocalContents_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaPurcProd_Pangea_t* xmgr_cas_NaPpvPurcprodGetLocalContents_Pangea(void)
{
	return &s_stNaPgeaPurcProd;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodUpdateRemainedCredit_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodUpdateRemainedCredit_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents, TCreditAmount nCredit)
{
	GWM_APP_InvalidateObject(NAPPV_PANGEA_PURCPROD_CREDIT_TEXT_ID);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodSetMainTitle_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodSetMainTitle_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents)
{
	HxLOG_Print("+++++++++++++++\n");

	/* set, draw background */
	Settings_DrawBack(FALSE);

	/* title */
	MWC_UTIL_DvbStrcpy(pContents->ucTitle, GetStrRsc(LOC_CAS_NAGRA_PANGEA_PURCHASED_PRODUCTS_TEXT_ID));
	Settings_SetWindowTitle(SETTING_GET_AP_DEPTH(eSetting_NagraPpvPangeaPurcProd), LOC_CAS_NAGRA_PANGEA_PURCHASED_PRODUCTS_TEXT_ID, pContents->ucTitle);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodScrollPage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodScrollPage_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents, KEY_Code_t eKeyCode)
{
	HINT32			nOldDrawIndex = 0, nNumPerPage = 0;
	BUS_Result_t	hRes = ERR_BUS_NO_ERROR;

	if (pContents == NULL)
	{
		return ERR_NO_OBJECT;
	}

	nNumPerPage = GWM_List_GetNumOfPage(NAPPV_PANGEA_PURCPROD_SUBBTN_ID);
	nOldDrawIndex = GWM_List_GetCurrentIndex(NAPPV_PANGEA_PURCPROD_SUBBTN_ID);

	switch(eKeyCode)
	{
		case KEY_ARROWUP:
		case KEY_PAGEUP:
			hRes = GWM_List_SetCurrentIndex(NAPPV_PANGEA_PURCPROD_SUBBTN_ID, nOldDrawIndex-nNumPerPage);
			if (hRes == ERR_BUS_NO_ERROR)
			{
				pContents->ulCurPage --;
				xmgr_cas_NaPpvPurcprodGetPageStr_Pangea(pContents);
				GWM_Text_SetText(NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_ID, pContents->ucPageNum);
			}
			break;
		case KEY_ARROWDOWN:
		case KEY_PAGEDOWN:
			hRes = GWM_List_SetCurrentIndex(NAPPV_PANGEA_PURCPROD_SUBBTN_ID, nOldDrawIndex+nNumPerPage);
			if (hRes == ERR_BUS_NO_ERROR)
			{
				pContents->ulCurPage ++;
				xmgr_cas_NaPpvPurcprodGetPageStr_Pangea(pContents);
				GWM_Text_SetText(NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_ID, pContents->ucPageNum);
			}
			break;

		case KEY_OK:
		default:
			break;
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodInitUiConfiguration_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodInitUiConfiguration_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents)
{
	HUINT8 					szDate[32];

	HxLOG_Print("++++++++++++++++++++\n");
	HxSTD_memset(pContents, 0, sizeof(XmgrCas_NaPurcProd_Pangea_t));

	pContents->ulNumOfUiSubItemEvents = 0;
	pContents->pstPurchaseHistoryListItem = NULL;
	xmgr_cas_NaGetCurrentDateStr(szDate);
	sprintf(pContents->ucCurDate, "%s: %s", (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_DATE_TEXT_ID), szDate);
	xmgr_cas_NaGetScCreditStr(pContents->ucCreditStr);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodRequestPurchaseHistory_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodRequestPurchaseHistory_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents)
{
#ifdef WIN32
	PPV_PURCHASE_LIST_ITEM	*s_testItems = (PPV_PURCHASE_LIST_ITEM*)OxAP_Malloc(MAX_PGEA_PURCPROD_LIST_EVTITEM_NUM*sizeof(PPV_PURCHASE_LIST_ITEM));
	HUINT16 i = 0;

	for (i = 0; i < MAX_PGEA_PURCPROD_LIST_EVTITEM_NUM; i++)
	{
		s_testItems[i].Cost = s_stPurcProdListSubItem[i].PurcProdCredit;
		s_testItems[i].PurchaseTime = s_stPurcProdListSubItem[i].PurcProdPurchaseDate;
		s_testItems[i].EvtNameLength = MWC_UTIL_DvbStrlen(s_stPurcProdListSubItem[i].PurcProdEvtName);
		HxSTD_memset(s_testItems[i].EvtName, 0, MAX_PPV_PURCHASE_LIST_NAME_LENGTH+1);
		MWC_UTIL_DvbStrncpy(s_testItems[i].EvtName, s_stPurcProdListSubItem[i].PurcProdEvtName, MAX_PPV_PURCHASE_LIST_NAME_LENGTH);
	}
	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_HISTORY, (Handle_t)0, eDxCAS_GROUPID_NA, (HINT32)MAX_PGEA_PURCPROD_LIST_EVTITEM_NUM, (HINT32)s_testItems);
#else
	xmgr_cas_NaGetPurchaseHistory(NA_OTHER_PROVIDER_ID);
#endif
	pContents->bWaiting = TRUE;

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodLinkTraverseListItembyIndex_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaPurcProdUiSubItem_t *xmgr_cas_NaPpvPurcprodLinkTraverseListItembyIndex_Pangea (POINTER_LIST_T **pRoot, HUINT16 usTotalItemIdx, HUINT16 usItemIdx)
{
	POINTER_LIST_T 	*tmp = NULL;

	if (usItemIdx < usTotalItemIdx)
	{
		tmp = UTIL_LINKEDLIST_FindNthListItemFromPointerList (*pRoot, usItemIdx, NULL, NULL);
		if (tmp == NULL)
		{
			HxLOG_Print("Index = %d\n", usItemIdx);
			HxLOG_Critical("Critical Eror!!!\n");
			return NULL;
		}
		return ((XmgrCas_NaPurcProdUiSubItem_t *)tmp->pvContents);
	}
	else
	{

	}

	return NULL;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodDrawPurchasedProductsItem_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodDrawPurchasedProductsItem_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t tObject, void *pvArg, int nParam1)
{
	OSD_StockImage_t	 	stImage;

	XmgrCas_NaPurcProdUiSubItem_t	*pDrawSubItemInfo = NULL;

	OSD_Rect_t				rect_list, rect_img;
	HINT32				 	nX1, nY1, nW1;
	HINT8 					szDate[128];

	//credit icon
	OSD_Rect_t				rect_icon;

	HINT32					nListItemNum, nListFocusIndex, nItemNum;
	HUINT16					i = 0;
	XmgrCas_NaPurcProd_Pangea_t	*pContents = xmgr_cas_NaPpvPurcprodGetLocalContents_Pangea();

	GWM_List_GetTotalItem(GWM_Obj_GetObjectID(tObject), &nListItemNum);
	nItemNum = GWM_List_GetNumOfPage(GWM_Obj_GetObjectID(tObject));
	nListFocusIndex = GWM_List_GetCurrentIndex(GWM_Obj_GetObjectID(tObject));
	pDrawSubItemInfo = (XmgrCas_NaPurcProdUiSubItem_t*)GWM_List_GetArrayPtr((LIST_OBJECT)tObject);

	if (pDrawSubItemInfo)
	{
		for (i = 0; i < nItemNum; i++)
		{
			pDrawSubItemInfo = xmgr_cas_NaPpvPurcprodLinkTraverseListItembyIndex_Pangea(&pContents->pstPurchaseHistoryListItem, pContents->ulNumOfUiSubItemEvents, nListFocusIndex+i);
			if (pDrawSubItemInfo)
			{
				// setting text style of the evtname info list
				OSD_GFX_SetRect(&rect_list, s_Pangea_PurcProd_NaviSubItem[0].rect.x, s_Pangea_PurcProd_NaviSubItem[0].rect.y + (i*NAPPV_PANGEA_PURCPROD_SUBBTN_LINESPACE),
											s_Pangea_PurcProd_NaviSubItem[0].rect.w, s_Pangea_PurcProd_NaviSubItem[0].rect.h);
				if (MWC_UTIL_DvbStrlenStrOnly(pDrawSubItemInfo->PurcProdEvtName) > 0)
				{
					OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(tObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, rect_list.x, rect_list.y, rect_list.w,
												(HINT8 *)pDrawSubItemInfo->PurcProdEvtName, COL(C_W_BG01));
				}
				else
				{
					OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(tObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, rect_list.x, rect_list.y, rect_list.w,
												(HINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_PURCHASE_BLANK_EVENT_NAME_ID), COL(C_T_MList_Yellow));
				}

				// setting text style of the expirydate info list
				OSD_GFX_SetRect(&rect_list, s_Pangea_PurcProd_NaviSubItem[1].rect.x, s_Pangea_PurcProd_NaviSubItem[1].rect.y + (i*NAPPV_PANGEA_PURCPROD_SUBBTN_LINESPACE),
											s_Pangea_PurcProd_NaviSubItem[1].rect.w, s_Pangea_PurcProd_NaviSubItem[1].rect.h);
				snprintf(szDate, 128, "%02d/%02d/%02d\n", pDrawSubItemInfo->PurcProdPurchaseDate.day, pDrawSubItemInfo->PurcProdPurchaseDate.month, (pDrawSubItemInfo->PurcProdPurchaseDate.year+1900)%100);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(tObject), eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_list.x, rect_list.y, rect_list.w,
												(HINT8 *)szDate, COL(C_W_BG01));


				// setting text style of the credit info list
				OSD_GFX_SetRect(&rect_list, s_Pangea_PurcProd_NaviSubItem[2].rect.x, s_Pangea_PurcProd_NaviSubItem[2].rect.y + (i*NAPPV_PANGEA_PURCPROD_SUBBTN_LINESPACE),
											s_Pangea_PurcProd_NaviSubItem[2].rect.w, s_Pangea_PurcProd_NaviSubItem[2].rect.h);

				/* credit */
				if (pDrawSubItemInfo->PurcProdCredit > 0)
				{
					/* credit icon */
					if (!AP_StockImg_GetImage (e911_4_24_Euro_Font_Black_B, &stImage))
					{
						nX1 = rect_list.x + rect_list.w - stImage.width;
						nY1 = rect_list.y - NAPPV_PANGEA_PURCPROD_STRING_MARGIN_Y;
						OSD_GFX_SetRect (&rect_icon, nX1, nY1, stImage.width, stImage.height);
						OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &rect_icon);
					}

					nW1 = rect_list.w - stImage.width;
					snprintf(szDate, 128, "%d,%02d", pDrawSubItemInfo->PurcProdCredit/100, pDrawSubItemInfo->PurcProdCredit%100);
				}
				else
				{
					nW1 = rect_list.w;
					snprintf(szDate, 128, "n.d.");
				}
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(tObject), eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_list.x, rect_list.y, nW1,
												(HINT8 *)szDate, COL(C_W_BG01));
			}
		}
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodDrawBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodDrawBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
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
 Function		: xmgr_cas_NaPpvPurcprodGetPageStr_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodGetPageStr_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents)
{
	HINT32	nCurPage, nTotalPage;

	nCurPage = pContents->ulCurPage + 1;
	nTotalPage = pContents->ulNumOfUiSubItemEvents / MAX_PGEA_PURCPROD_LIST_ROW_NUM + 1;
	if ((pContents->ulNumOfUiSubItemEvents % MAX_PGEA_PURCPROD_LIST_ROW_NUM) == 0)
	{
		nTotalPage--;
	}

	snprintf(pContents->ucPageNum, 16, "%d/%d", nCurPage, nTotalPage);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodDrawCredit_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodDrawCredit_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT8 					szDate[128];
	HINT32					nObjectId, i, nRemainCredit = 0;
	OSD_Rect_t				rect_text;

	//credit icon
	OSD_StockImage_t	 	stImage;
	OSD_Rect_t				rect_icon;
	HINT32					nX, nY, nStrLen = 0;

	OSD_Font_t				*pFont;

	XmgrCas_NaPurcProd_Pangea_t	*pContents = xmgr_cas_NaPpvPurcprodGetLocalContents_Pangea();

	//----------------------------------------------------------------
	rect_text.x = pArea->x1 + pRect->x;
	rect_text.y = pArea->y1 + pRect->y;
	rect_text.w = pRect->w;
	rect_text.h = pRect->h;
	//----------------------------------------------------------------

	/* for compiler */
	(void)pArg;
	(void)param1;

	nObjectId = GWM_Obj_GetObjectID(pObject);

	/* na ppv pangea purcprod credit text */
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, rect_text.x, rect_text.y, rect_text.w,
										(HINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_CREDIT_TEXT_ID), COL(C_T_PList_Orange));


	/* na ppv pangea purcprod credit */
	pFont = OSD_FONT_Get(eFont_SystemNormal);
	nStrLen = OSD_GFX_GetStringWidth(pFont, (HINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_CREDIT_TEXT_ID));
	nX = rect_text.x + nStrLen;
	if (pFont != NULL)
		OSD_FONT_Release(pFont);

	xmgr_cas_NaGetScCredit(&nRemainCredit);
	snprintf(szDate, 128, "%d,%02d", nRemainCredit/100, nRemainCredit%100);
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, nX, rect_text.y, rect_text.w,
										(HINT8 *)szDate, COL(C_T_Cursor));


	/* na ppv pangea purcprod credit icon */
	pFont = OSD_FONT_Get(eFont_SystemNormal);
	nStrLen = OSD_GFX_GetStringWidth(pFont, (HINT8 *)szDate);
	nX = nX + nStrLen;
	nY = rect_text.y - NAPPV_PANGEA_PURCPROD_STRING_MARGIN_Y;
	if (pFont != NULL)
		OSD_FONT_Release(pFont);

	if (!AP_StockImg_GetImage (e911_4_24_Euro_Font_White_B, &stImage))
	{
		OSD_GFX_SetRect (&rect_icon, nX, nY, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &rect_icon);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodDrawCreditBg_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodDrawCreditBg_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvPurcprodDrawUpDownArrow_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodDrawUpDownArrow_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 	nX1, nY1;
	OSD_Rect_t			 	stRect;
	OSD_StockImage_t	 	stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e911_2_24_Arrow_W_3UD, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodDrawBackButton_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodDrawBackButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 	nX1, nY1;
	OSD_Rect_t			 	stRect;
	OSD_StockImage_t	 	stImage;

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
 Function		: xmgr_cas_NaPpvPurcprodSetUiObjects_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodSetUiObjects_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents)
{
	OSD_Rect_t				rect = {0, };
	HINT32					nColIndex, nRowIndex, nListItemX, nListItemY, nListItemId;

	/* na ppv pangea purcprod background */
	Settings_DrawBack(FALSE);
	OSD_GFX_SetRect(&rect, 0, 0, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT);
	GWM_Rect_Create(NAPPV_PANGEA_PURCPROD_LOGOBACKGROUND_IMAGE_ID, &rect, 0x00000000);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_PURCPROD_LOGOBACKGROUND_IMAGE_ID, xmgr_cas_NaPpvPurcprodDrawBackgroundImage_Pangea);

	/* na ppv pangea purcprod date text*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_PURCPROD_DATE_TEXT_X, NAPPV_PANGEA_PURCPROD_DATE_TEXT_Y, NAPPV_PANGEA_PURCPROD_DATE_TEXT_WIDTH, NAPPV_PANGEA_PURCPROD_DATE_TEXT_HEIGHT);
	GWM_Text_Create(NAPPV_PANGEA_PURCPROD_DATE_TEXT_ID, &rect, pContents->ucCurDate);
	GWM_Text_SetAlign (NAPPV_PANGEA_PURCPROD_DATE_TEXT_ID, TEXT_ALIGN_RIGHT);
	GWM_Text_SetSize (NAPPV_PANGEA_PURCPROD_DATE_TEXT_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor (NAPPV_PANGEA_PURCPROD_DATE_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv pangea purcprod infotext*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_PURCPROD_INFO_TEXT_X, NAPPV_PANGEA_PURCPROD_INFO_TEXT_Y, NAPPV_PANGEA_PURCPROD_INFO_TEXT_WIDTH, NAPPV_PANGEA_PURCPROD_INFO_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_PURCPROD_INFO_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_PURCHASED_PRODUCTS_TEXT_ID));
	GWM_Text_SetAlign (NAPPV_PANGEA_PURCPROD_INFO_TEXT_ID, TEXT_ALIGN_RIGHT);
	GWM_Text_SetSize (NAPPV_PANGEA_PURCPROD_INFO_TEXT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor(NAPPV_PANGEA_PURCPROD_INFO_TEXT_ID, COL (C_T_Cursor), 0x00);

	/* na ppv pangea purcprod credit bg */
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_PURCPROD_STATE_BTN_X, NAPPV_PANGEA_PURCPROD_STATE_BTN_Y, NAPPV_PANGEA_PURCPROD_STATE_BTN_WIDTH, NAPPV_PANGEA_PURCPROD_STATE_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_PURCPROD_STATE_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_PURCPROD_STATE_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPurcprodDrawCreditBg_Pangea);

	/* na ppv pangea purcprod credit*/
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_PURCPROD_CREDIT_TEXT_X, NAPPV_PANGEA_PURCPROD_CREDIT_TEXT_Y, NAPPV_PANGEA_PURCPROD_CREDIT_TEXT_WIDTH, NAPPV_PANGEA_PURCPROD_CREDIT_TEXT_HEIGHT);
	GWM_Text_Create(NAPPV_PANGEA_PURCPROD_CREDIT_TEXT_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_PURCPROD_CREDIT_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPurcprodDrawCredit_Pangea);

	/* na ppv pangea purcprod back button */
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_PURCPROD_BACK_BTN_X, NAPPV_PANGEA_PURCPROD_BACK_BTN_Y, NAPPV_PANGEA_PURCPROD_BACK_BTN_WIDTH, NAPPV_PANGEA_PURCPROD_BACK_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_PANGEA_PURCPROD_BACK_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_PURCPROD_BACK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPurcprodDrawBackButton_Pangea);

	/* na ppv pangea purcprod back button text*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_PURCPROD_BACK_TEXT_X, NAPPV_PANGEA_PURCPROD_BACK_TEXT_Y, NAPPV_PANGEA_PURCPROD_BACK_TEXT_WIDTH, NAPPV_PANGEA_PURCPROD_BACK_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_PURCPROD_BACK_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BACK_MENU_TEXT_ID));
	GWM_Text_SetAlign (NAPPV_PANGEA_PURCPROD_BACK_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_PURCPROD_BACK_TEXT_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor(NAPPV_PANGEA_PURCPROD_BACK_TEXT_ID, COL (C_T_Cursor), 0x00);

	HxLOG_Print(" --- \n");

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodDestroyUI_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodDestroyUI_Pangea(XmgrCas_NaPurcProd_Pangea_t *pContents)
{
	if (pContents->pstPurchaseHistoryListItem)
	{
		xmgr_cas_NaPpvPurcprodFreePurchaseHistory_Pangea(&pContents->pstPurchaseHistoryListItem);
		pContents->pstPurchaseHistoryListItem = NULL;
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodLinkFreeListItemMethod_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC void	xmgr_cas_NaPpvPurcprodLinkFreeListItemMethod_Pangea(void *pvContents)
{
	if(pvContents == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	OxAP_Free(pvContents);
}

/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodFreePurchaseHistory_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodFreePurchaseHistory_Pangea(POINTER_LIST_T **pRoot)
{
	*pRoot = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(*pRoot, xmgr_cas_NaPpvPurcprodLinkFreeListItemMethod_Pangea);

	if(*pRoot != NULL)
	{
		HxLOG_Critical("\n\n");
	}

	return ERR_BUS_NO_ERROR;
}


/********************************************************************
 Function		: xmgr_cas_NaPpvPurcprodUpdatePurchaseHistory_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodUpdatePurchaseHistory_Pangea(HUINT32 xStatus, HUINT32 xNumberOfItems, const PPV_PURCHASE_LIST_ITEM* pxPurchaseList, XmgrCas_NaPurcProd_Pangea_t *pContents)
{
	HUINT16					index = 0;
	XmgrCas_NaPurcProdUiSubItem_t	*ptItem;
	OSD_Rect_t				rect;

	if (pContents->bWaiting != TRUE)
	{
		return MESSAGE_BREAK;
	}

	pContents->bWaiting = FALSE;

	if (pContents->pstPurchaseHistoryListItem)
	{
		xmgr_cas_NaPpvPurcprodFreePurchaseHistory_Pangea(&pContents->pstPurchaseHistoryListItem);
		pContents->pstPurchaseHistoryListItem = NULL;
		pContents->ulNumOfUiSubItemEvents = 0;
		pContents->ulCurPage = 0;
	}

	if (xStatus == CA_EVENT_SUCCESS)
	{
		if (xNumberOfItems > 0 && pxPurchaseList != NULL)
		{
			pContents->ulNumOfUiSubItemEvents = xNumberOfItems;

			for (index = 0; index < xNumberOfItems; index ++)
			{
				ptItem = (XmgrCas_NaPurcProdUiSubItem_t *)OxAP_Malloc(sizeof(XmgrCas_NaPurcProdUiSubItem_t));
				HxSTD_memset(ptItem->PurcProdEvtName, 0, MAX_PGEA_PURCPROD_LIST_EVTNAME_LENGTH + 1);
				MWC_UTIL_DvbStrncpy(ptItem->PurcProdEvtName, pxPurchaseList[index].EvtName, MAX_PGEA_PURCPROD_LIST_EVTNAME_LENGTH);
				ptItem->PurcProdPurchaseDate = pxPurchaseList[index].PurchaseTime;
				ptItem->PurcProdCredit = pxPurchaseList[index].Cost;
				pContents->pstPurchaseHistoryListItem = UTIL_LINKEDLIST_AppendListItemToPointerList(pContents->pstPurchaseHistoryListItem, ptItem);
			}
		}

		/* purchase history List Item을 생성한다. */
		OSD_GFX_SetRect(&rect, NAPPV_PANGEA_PURCPROD_SUBBTN_X, NAPPV_PANGEA_PURCPROD_SUBBTN_Y, NAPPV_PANGEA_PURCPROD_SUBBTN_WIDTH, (NAPPV_PANGEA_PURCPROD_SUBBTN_HEIGHT * MAX_PGEA_PURCPROD_LIST_ROW_NUM) + 16);
		GWM_List_Create(NAPPV_PANGEA_PURCPROD_SUBBTN_ID, &rect, NULL, NAPPV_PANGEA_PURCPROD_SUBBTN_HEIGHT, pContents->ulNumOfUiSubItemEvents, /* page count */ MAX_PGEA_PURCPROD_LIST_ROW_NUM, ((GWM_LIST_ITEMS)pContents->pstPurchaseHistoryListItem));
		GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_PURCPROD_SUBBTN_ID, xmgr_cas_NaPpvPurcprodDrawPurchasedProductsItem_Pangea);

		if (pContents->ulNumOfUiSubItemEvents)
		{
			/* na ppv pangea purcprod arrow */
			OSD_GFX_SetRect(&rect, NAPPV_PANGEA_PURCPROD_ARROW_BTN_X, NAPPV_PANGEA_PURCPROD_ARROW_BTN_Y, NAPPV_PANGEA_PURCPROD_ARROW_BTN_WIDTH, NAPPV_PANGEA_PURCPROD_ARROW_BTN_HEIGHT);
			GWM_Frame_Create(NAPPV_PANGEA_PURCPROD_ARROW_BTN_ID, &rect, NULL);
			GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_PURCPROD_ARROW_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPurcprodDrawUpDownArrow_Pangea);

			/* na ppv pangea purcprod list page button */
			xmgr_cas_NaPpvPurcprodGetPageStr_Pangea(pContents);
			OSD_GFX_SetRect(&rect, NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_X, NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_Y-FONT_SIZE_XS, NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_WIDTH, NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_HEIGHT);
			GWM_Text_Create(NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_ID, &rect, pContents->ucPageNum);
			GWM_Text_SetAlign (NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_ID, TEXT_ALIGN_LEFT);
			GWM_Text_SetSize (NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_ID, FONT_SIZE_XS);
			GWM_Text_ChangeColor(NAPPV_PANGEA_PURCPROD_LISTPAGE_TEXT_ID, COL (C_T_Cursor), 0x00);
		}
		else
		{
			// no item string display
			OSD_GFX_SetRect(&rect, NAPPV_PANGEA_PURCPROD_SUBBTN01_X, NAPPV_PANGEA_PURCPROD_SUBBTN01_Y, NAPPV_PANGEA_PURCPROD_SUBBTN01_WIDTH, NAPPV_PANGEA_PURCPROD_SUBBTN01_HEIGHT);
			GWM_Text_Create(NAPPV_PANGEA_PURCPROD_NO_ITEM_TEXT, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_MENU_MSG_NO_PURCHASE_HISTORY_ID));
			GWM_Text_SetAlign (NAPPV_PANGEA_PURCPROD_NO_ITEM_TEXT, TEXT_ALIGN_LEFT);
			GWM_Text_SetSize (NAPPV_PANGEA_PURCPROD_NO_ITEM_TEXT, FONT_SIZE_M);
			GWM_Text_ChangeColor(NAPPV_PANGEA_PURCPROD_NO_ITEM_TEXT, COL (C_W_BG01), 0x00);
		}

#ifdef WIN32
		OxAP_Free(pxPurchaseList);
#endif

		GWM_Obj_SetFocus(NAPPV_PANGEA_PURCPROD_SUBBTN_ID);
	}
	else
	{
		if (GWM_Obj_GetObjectByID(NAPPV_PANGEA_PURCPROD_DATE_TEXT_ID))
		{
			GWM_Obj_Destroy(NAPPV_PANGEA_PURCPROD_DATE_TEXT_ID);
		}
		if (GWM_Obj_GetObjectByID(NAPPV_PANGEA_PURCPROD_INFO_TEXT_ID))
		{
			GWM_Obj_Destroy(NAPPV_PANGEA_PURCPROD_INFO_TEXT_ID);
		}
		if (GWM_Obj_GetObjectByID(NAPPV_PANGEA_PURCPROD_STATE_BTN_ID))
		{
			GWM_Obj_Destroy(NAPPV_PANGEA_PURCPROD_STATE_BTN_ID);
		}
		if (GWM_Obj_GetObjectByID(NAPPV_PANGEA_PURCPROD_CREDIT_TEXT_ID))
		{
			GWM_Obj_Destroy(NAPPV_PANGEA_PURCPROD_CREDIT_TEXT_ID);
		}
		if (GWM_Obj_GetObjectByID(NAPPV_PANGEA_PURCPROD_BACK_BTN_ID))
		{
			GWM_Obj_Destroy(NAPPV_PANGEA_PURCPROD_BACK_BTN_ID);
		}
		if (GWM_Obj_GetObjectByID(NAPPV_PANGEA_PURCPROD_BACK_TEXT_ID))
		{
			GWM_Obj_Destroy(NAPPV_PANGEA_PURCPROD_BACK_TEXT_ID);
		}

		// message
		OSD_GFX_SetRect(&rect, NAPPV_PANGEA_PURCPROD_TS_ERROR_TEXT_X, NAPPV_PANGEA_PURCPROD_TS_ERROR_TEXT_Y, NAPPV_PANGEA_PURCPROD_TS_ERROR_TEXT_WIDTH, NAPPV_PANGEA_PURCPROD_TS_ERROR_TEXT_HEIGHT);
		GWM_Text_Create(NAPPV_PANGEA_PURCPROD_TS_ERROR_TEXT_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_PURCPROD_TS_ERROR_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPurcprodEvtlistDrawTsErrorMessage_Pangea);

		// Exit button
		OSD_GFX_SetRect(&rect, NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_X, NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_Y, NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_WIDTH, NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HEIGHT);
		GWM_Frame_Create(NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvPurcprodEvtlistDrawTsErrorExitBtn_Pangea);

		// Exit help
		OSD_GFX_SetRect(&rect, NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HELP_X, NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HELP_Y, NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HELP_WIDTH, NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HELP_HEIGHT);
		GWM_Text_Create(NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HELP_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_EXIT_TEXT_ID));
		GWM_Text_SetAlign (NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HELP_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HELP_ID, FONT_SIZE_XS);
		GWM_Text_ChangeColor(NAPPV_PANGEA_PURCPROD_TS_ERROR_BTN_HELP_ID, COL (C_T_Cursor), 0x00);
	}
}

STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodEvtlistDrawTsErrorMessage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32					nObjectId;
	OSD_Rect_t				rect_text;

	OSD_Font_t				*pFont;

	//----------------------------------------------------------------
	rect_text.x = pRect->x;
	rect_text.y = pRect->y - NAPPV_PANGEA_PURCPROD_STRING_MARGIN_Y;
	rect_text.w = pRect->w;
	rect_text.h = pRect->h;
	//----------------------------------------------------------------

	/* for compiler */
	(void)pArg;
	(void)param1;

	nObjectId = GWM_Obj_GetObjectID(pObject);
	pFont = OSD_FONT_Get(eFont_SystemNormal);
	if (pFont == NULL)
	{
		return ERR_FAIL;
	}
	OSD_FONT_SetSize (pFont, FONT_SIZE_M);

	OSD_GFX_DrawMultiLineStringAlign(pScreen, pFont, rect_text.x, rect_text.y, rect_text.w, rect_text.h, (HINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_MENU_ERR_PACKAGE_TS_ID),
		NAPPV_PANGEA_PURCPROD_SUBBTN_LINESPACE-FONT_SIZE_M, COL(C_W_BG01), TEXT_ALIGN_CENTER|TEXT_MULTILINED);

	if (pFont != NULL)
	{
		OSD_FONT_Release(pFont);
	}

	return ERR_BUS_NO_ERROR;
}


/********************************************************************
 Function		: xmgr_cas_Nappv_pgea_prodcatal_evtlist_DrawTsErrorExitBtn
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvPurcprodEvtlistDrawTsErrorExitBtn_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 	nX1, nY1;
	OSD_Rect_t			 	stRect;
	OSD_StockImage_t	 	stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e921_1_24_BTN_5Exit, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}


#define	______
#define	_________________________PUBLIC_BODY______________________________
#define	______
/******************************************************************************/
/**
*   \fn     xproc_cas_NaPurchasedProducts_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaPurchasedProducts_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaPurcProd_Pangea_t	*pContents = xmgr_cas_NaPpvPurcprodGetLocalContents_Pangea();
	HUINT32					nResult = ERR_FAIL;

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_PANGEA_PURCPROD_WINDOW_X, NAPPV_PANGEA_PURCPROD_WINDOW_Y,
								NAPPV_PANGEA_PURCPROD_WINDOW_WIDTH, NAPPV_PANGEA_PURCPROD_WINDOW_HEIGHT);

			xmgr_cas_NaPpvPurcprodInitUiConfiguration_Pangea(pContents);
			xmgr_cas_NaPpvPurcprodSetMainTitle_Pangea(pContents);

			// update purcprod information
			xmgr_cas_NaPpvPurcprodRequestPurchaseHistory_Pangea(pContents);

			// draw UI object
			xmgr_cas_NaPpvPurcprodSetUiObjects_Pangea(pContents);

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
				GWM_APP_InvalidateObject(NAPPV_PANGEA_PURCPROD_STATUS_ID);
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

		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_HISTORY:
			if(p1 == eDxCAS_GROUPID_NA)
			{
				xmgr_cas_NaPpvPurcprodUpdatePurchaseHistory_Pangea((HUINT32)hAction, (HUINT32)p2, (PPV_PURCHASE_LIST_ITEM*)p3, pContents);
			}
			return MESSAGE_BREAK;

		case eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT:
			HxLOG_Print("eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT...\n");
			if(p1 == eDxCAS_GROUPID_NA)
			{
				xmgr_cas_NaPpvPurcprodUpdateRemainedCredit_Pangea(pContents, (TCreditAmount)p3);
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

				case KEY_BACK:
					/* 상위 메뉴로 올라갈 때 focus를 기억하지 않는 것이 편할 수도 있다. */
					//Settings_InitFocus(pstContents);

					BUS_MGR_Destroy(NULL);

					/* to upper depth */
					BUS_MGR_Create("xmgr_cas_NaPpvMenu_Pangea_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xmgr_cas_NaPpvMenu_Pangea_Proc, 0, 0, 0, 0);
					return MESSAGE_BREAK;

				case KEY_ARROWUP:
				case KEY_ARROWDOWN:
				case KEY_PAGEUP:
				case KEY_PAGEDOWN:
					xmgr_cas_NaPpvPurcprodScrollPage_Pangea(pContents, p1);
					return MESSAGE_BREAK;

				case KEY_OK:
					return MESSAGE_BREAK;

			}
			break;

		case MSG_GWM_SCROLLUP:
			return MESSAGE_BREAK;

		case MSG_GWM_SCROLLDOWN:
			return MESSAGE_BREAK;

		case MSG_GWM_TIMER:
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			xmgr_cas_NaPpvPurcprodDestroyUI_Pangea(pContents);

			/* clear front PANEL */
			//AP_SPEC_ClearMenuPanelString();
			break;

		default:
			break;
	}

	HxLOG_Info("xproc_cas_NaPurchasedProducts_Pangea handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif	//end of #if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
/* End of File */
