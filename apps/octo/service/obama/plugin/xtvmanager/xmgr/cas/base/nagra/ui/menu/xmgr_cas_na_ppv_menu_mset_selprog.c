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
	eCasNaMsetSelProg_Name,
	eCasNaMsetSelProg_ExpiryDate,
	eCasNaMsetSelProg_Credit,
	eCasNaMsetSelProg_Item_Max
} XmgrCas_NaSelProgItem_e;

#define	MAX_MSET_SELPROG_LIST_ROW_NUM					5
#define	MAX_MSET_SELPROG_LIST_EVTNAME_LENGTH			50

typedef struct tagSelProgramsSubItem
{
	TChar					SelProgEvtName[MAX_MSET_SELPROG_LIST_EVTNAME_LENGTH+1];
	TCalendarTime			SelProgPurchaseDate;
	TCreditAmount			SelProgCredit;
} XmgrCar_NaSelProgUiSubItem_t;

typedef struct tag_CAS_NA_MEDIASET_SP_Contents
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
} XmgrCas_NaSelProg_Mediaset_t;

#define	NAPPV_MEDIASET_SELPROG_WINDOW_WIDTH						OSD_SCREEN_WIDTH
#define	NAPPV_MEDIASET_SELPROG_WINDOW_HEIGHT					OSD_SCREEN_HEIGHT
#define	NAPPV_MEDIASET_SELPROG_WINDOW_X							0
#define	NAPPV_MEDIASET_SELPROG_WINDOW_Y							0
#define	NAPPV_MEDIASET_SELPROG_STRING_MARGIN_Y					18

#define	NAPPV_MEDIASET_SELPROG_APP_ID							eCasNaMsetSelProg_Name

/* ppv mediaset selprog background */
#define	NAPPV_MEDIASET_SELPROG_LOGOBACKGROUND_IMAGE_ID			(NAPPV_MEDIASET_SELPROG_APP_ID|0x0010)

#define	NAPPV_MEDIASET_SELPROG_STATE_BTN_ID						(NAPPV_MEDIASET_SELPROG_APP_ID|0x0020)
#define	NAPPV_MEDIASET_SELPROG_STATE_BTN_X						598
#define	NAPPV_MEDIASET_SELPROG_STATE_BTN_Y						189
#define	NAPPV_MEDIASET_SELPROG_STATE_BTN_WIDTH					546
#define	NAPPV_MEDIASET_SELPROG_STATE_BTN_HEIGHT					50

/* ppv mediaset selprog date */
#define	NAPPV_MEDIASET_SELPROG_DATE_TEXT_ID						(NAPPV_MEDIASET_SELPROG_APP_ID|0x0030)
#define	NAPPV_MEDIASET_SELPROG_DATE_TEXT_X						830
#define	NAPPV_MEDIASET_SELPROG_DATE_TEXT_Y						(122 - FONT_SIZE_S)
#define	NAPPV_MEDIASET_SELPROG_DATE_TEXT_WIDTH					300
#define	NAPPV_MEDIASET_SELPROG_DATE_TEXT_HEIGHT					FONT_SIZE_S

/* ppv mediaset selprog info text */
#define	NAPPV_MEDIASET_SELPROG_INFO_TEXT_ID						(NAPPV_MEDIASET_SELPROG_APP_ID|0x0040)
#define	NAPPV_MEDIASET_SELPROG_INFO_TEXT_X						146
#define	NAPPV_MEDIASET_SELPROG_INFO_TEXT_Y						(225 - FONT_SIZE_M) //253
#define	NAPPV_MEDIASET_SELPROG_INFO_TEXT_WIDTH					410
#define	NAPPV_MEDIASET_SELPROG_INFO_TEXT_HEIGHT					FONT_SIZE_M

/* ppv mediaset selprog credit */
#define	NAPPV_MEDIASET_SELPROG_CREDIT_BTN_ID					(NAPPV_MEDIASET_SELPROG_APP_ID|0x0050)
#define	NAPPV_MEDIASET_SELPROG_CREDIT_BTN_X						628
#define	NAPPV_MEDIASET_SELPROG_CREDIT_BTN_Y						225
#define	NAPPV_MEDIASET_SELPROG_CREDIT_BTN_WIDTH					476
#define	NAPPV_MEDIASET_SELPROG_CREDIT_BTN_HEIGHT				36

/* ppv mediaset selprog arrow button */
#define	NAPPV_MEDIASET_SELPROG_ARROW_BTN_ID						(NAPPV_MEDIASET_SELPROG_APP_ID|0x0070)
#define	NAPPV_MEDIASET_SELPROG_ARROW_BTN_X						1115
#define	NAPPV_MEDIASET_SELPROG_ARROW_BTN_Y						541
#define	NAPPV_MEDIASET_SELPROG_ARROW_BTN_WIDTH					30
#define	NAPPV_MEDIASET_SELPROG_ARROW_BTN_HEIGHT					12

/* ppv mediaset selprog back button */
#define	NAPPV_MEDIASET_SELPROG_BACK_BTN_ID						(NAPPV_MEDIASET_SELPROG_APP_ID|0x0080)
#define	NAPPV_MEDIASET_SELPROG_BACK_BTN_X						1079
#define	NAPPV_MEDIASET_SELPROG_BACK_BTN_Y						611
#define	NAPPV_MEDIASET_SELPROG_BACK_BTN_WIDTH					72
#define	NAPPV_MEDIASET_SELPROG_BACK_BTN_HEIGHT					32

/* ppv mediaset selprog event Info list page */
#define	NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_ID					(NAPPV_MEDIASET_SELPROG_APP_ID|0x00a0)
#define	NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_X					1060
#define	NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_Y					553
#define	NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_WIDTH				43
#define	NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_HEIGHT				21

#define	NAPPV_MEDIASET_SELPROG_STATUS_ID						(NAPPV_MEDIASET_SELPROG_APP_ID|0x00b0) //update 시 처리 부분.....

#define __________SUBITEM_LINE_OBJECTS_________________________________________
#define	NAPPV_MEDIASET_SELPROG_SUBBTN_LINE_HEIGHT					2
#define	NAPPV_MEDIASET_SELPROG_SUBBTN_LINE_LINESPACE				44

/* ppv mediaset selprog line position */
#define	NAPPV_MEDIASET_SELPROG_SUBBTN01_LINE_ID					(NAPPV_MEDIASET_SELPROG_APP_ID|0x0110)
#define	NAPPV_MEDIASET_SELPROG_SUBBTN01_LINE_WIDTH				1008
#define	NAPPV_MEDIASET_SELPROG_SUBBTN01_LINE_HEIGHT				NAPPV_MEDIASET_SELPROG_SUBBTN_LINE_HEIGHT
#define	NAPPV_MEDIASET_SELPROG_SUBBTN01_LINE_X					136
#define	NAPPV_MEDIASET_SELPROG_SUBBTN01_LINE_Y					309 - 44

#define __________SUBITEM_OBJECTS_________________________________________
#define	NAPPV_MEDIASET_SELPROG_SUBBTN_ID						(NAPPV_MEDIASET_SELPROG_APP_ID|0x0200)
#define	NAPPV_MEDIASET_SELPROG_SUBBTN_WIDTH						1008
#define	NAPPV_MEDIASET_SELPROG_SUBBTN_HEIGHT						44
#define	NAPPV_MEDIASET_SELPROG_SUBBTN_X							136
#define	NAPPV_MEDIASET_SELPROG_SUBBTN_Y							(299 - NAPPV_MEDIASET_SELPROG_SUBBTN_HEIGHT)
#define	NAPPV_MEDIASET_SELPROG_SUBBTN_LINESPACE					44

/* ppv mediaset selprog btn 01 position */
#define	NAPPV_MEDIASET_SELPROG_SUBBTN01_ID						(NAPPV_MEDIASET_SELPROG_APP_ID|0x0210)
#define	NAPPV_MEDIASET_SELPROG_SUBBTN01_WIDTH					634
#define	NAPPV_MEDIASET_SELPROG_SUBBTN01_HEIGHT				NAPPV_MEDIASET_SELPROG_SUBBTN_HEIGHT
#define	NAPPV_MEDIASET_SELPROG_SUBBTN01_X						146
#define	NAPPV_MEDIASET_SELPROG_SUBBTN01_Y						(299 - NAPPV_MEDIASET_SELPROG_SUBBTN_HEIGHT)

/* ppv mediaset selprog btn 02 position */
#define	NAPPV_MEDIASET_SELPROG_SUBBTN02_ID						(NAPPV_MEDIASET_SELPROG_APP_ID|0x0220)
#define	NAPPV_MEDIASET_SELPROG_SUBBTN02_WIDTH					140
#define	NAPPV_MEDIASET_SELPROG_SUBBTN02_HEIGHT				NAPPV_MEDIASET_SELPROG_SUBBTN_HEIGHT
#define	NAPPV_MEDIASET_SELPROG_SUBBTN02_X						800
#define	NAPPV_MEDIASET_SELPROG_SUBBTN02_Y						(299 - NAPPV_MEDIASET_SELPROG_SUBBTN_HEIGHT)

/* ppv mediaset selprog btn 03 position */
#define	NAPPV_MEDIASET_SELPROG_SUBBTN03_ID						(NAPPV_MEDIASET_SELPROG_APP_ID|0x0230)
#define	NAPPV_MEDIASET_SELPROG_SUBBTN03_WIDTH					174
#define	NAPPV_MEDIASET_SELPROG_SUBBTN03_HEIGHT				NAPPV_MEDIASET_SELPROG_SUBBTN_HEIGHT
#define	NAPPV_MEDIASET_SELPROG_SUBBTN03_X						960
#define	NAPPV_MEDIASET_SELPROG_SUBBTN03_Y						(299 - NAPPV_MEDIASET_SELPROG_SUBBTN_HEIGHT)

#define NAPPV_MEDIASET_SELPROG_NO_ITEM_TEXT						(NAPPV_MEDIASET_SELPROG_APP_ID|0x0240)
#define NAPPV_MEDIASET_SELPROG_NO_ITEM_LINE						(NAPPV_MEDIASET_SELPROG_APP_ID|0x0250)

#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BACK_ID					(NAPPV_MEDIASET_SELPROG_APP_ID|0x0370)
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BACK_X					0
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BACK_Y					260
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BACK_WIDTH				1280
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BACK_HEIGHT				349

#define NAPPV_MEDIASET_SELPROG_TS_ERROR_TEXT_ID					(NAPPV_MEDIASET_SELPROG_APP_ID|0x0380)
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_TEXT_X					190
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_TEXT_Y					309
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_TEXT_WIDTH				900
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_TEXT_HEIGHT				310
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_TEXT_LINESPACE			36

#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_ID					(NAPPV_MEDIASET_SELPROG_APP_ID|0x0390)
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_X					1079
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_Y					611
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_WIDTH				72
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_HEIGHT				32

#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_HELP_ID				(NAPPV_MEDIASET_SELPROG_APP_ID|0x03A0)
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_HELP_X				664
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_HELP_Y				(434-FONT_SIZE_XS)
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_HELP_WIDTH			250
#define NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_HELP_HEIGHT			41


STATIC Item_t s_Mediaset_SelProg_NaviSubItem[eCasNaMsetSelProg_Item_Max] =
{
/* 01 */	{NAPPV_MEDIASET_SELPROG_SUBBTN01_ID, { NAPPV_MEDIASET_SELPROG_SUBBTN01_X, NAPPV_MEDIASET_SELPROG_SUBBTN01_Y, NAPPV_MEDIASET_SELPROG_SUBBTN01_WIDTH, NAPPV_MEDIASET_SELPROG_SUBBTN01_HEIGHT}},
/* 02 */	{NAPPV_MEDIASET_SELPROG_SUBBTN02_ID, { NAPPV_MEDIASET_SELPROG_SUBBTN02_X, NAPPV_MEDIASET_SELPROG_SUBBTN02_Y, NAPPV_MEDIASET_SELPROG_SUBBTN02_WIDTH, NAPPV_MEDIASET_SELPROG_SUBBTN02_HEIGHT}},
/* 03 */	{NAPPV_MEDIASET_SELPROG_SUBBTN03_ID, { NAPPV_MEDIASET_SELPROG_SUBBTN03_X, NAPPV_MEDIASET_SELPROG_SUBBTN03_Y, NAPPV_MEDIASET_SELPROG_SUBBTN03_WIDTH, NAPPV_MEDIASET_SELPROG_SUBBTN03_HEIGHT}},
};

STATIC Item_t s_Mediaset_SelProg_NaviSubItemLine[1] =
{
/* 01 */	{NAPPV_MEDIASET_SELPROG_SUBBTN01_LINE_ID, { NAPPV_MEDIASET_SELPROG_SUBBTN01_LINE_X, NAPPV_MEDIASET_SELPROG_SUBBTN01_LINE_Y, NAPPV_MEDIASET_SELPROG_SUBBTN01_LINE_WIDTH, NAPPV_MEDIASET_SELPROG_SUBBTN01_LINE_HEIGHT}},
};

#ifdef WIN32
#define	MAX_MSET_SELPROG_LIST_EVTITEM_NUM				20

STATIC XmgrCar_NaSelProgUiSubItem_t s_stSelProgListSubItem[MAX_MSET_SELPROG_LIST_EVTITEM_NUM] =
{
	{	"1SELPROG SPORT MALgq",	{109, 0x09, 0x1E, 0, 0, 0},	0x0001	},
	{	"2SELPROG SPORT JUVE",	{109, 0x09, 0x1C, 0, 0, 0},	0x0003	},
	{	"3SELPROG SPORT INTER",	{109, 0x09, 0x1B, 0, 0, 0},	0x0005	},
	{	"4SELPROG SPORT MCRO",	{109, 0x09, 0x1A, 0, 0, 0},	0x0007	},
	{	"5SELPROG SPORT ITEFT",	{109, 0x09, 0x19, 0, 0, 0},	0x0009	},

	{	"6SELPROG CINAMA MALAN",{109, 0x09, 0x18, 0, 0, 0},	0x000a	},
	{	"7SELPROG CINAMA JUVE",	{109, 0x09, 0x17, 0, 0, 0},	0x000b	},
	{	"8SELPROG CINAMA INTER",{109, 0x09, 0x16, 0, 0, 0},	0x000c	},
	{	"9SELPROG CINAMA MCRO",	{109, 0x09, 0x15, 0, 0, 0},	0x000d	},
	{	"10SELPROG CINAMA ITEFT",{109, 0x09, 0x14, 0, 0, 0},	0x000e	},

	{	"11SELPROG OTHER MALAN",{109, 0x09, 0x13, 0, 0, 0},	0x000f	},
	{	"12SELPROG OTHER JUVE",	{109, 0x09, 0x12, 0, 0, 0},	0x0010	},
	{	"13SELPROG OTHER INTER",{109, 0x09, 0x11, 0, 0, 0},	0x0011	},
	{	"14SELPROG OTHER MCRO",	{109, 0x09, 0x10, 0, 0, 0},	0x0012	},
	{	"15SELPROG OTHER ITEFT",{109, 0x09, 0x0F, 0, 0, 0},	0x0013	},

	{	"16SELPROG 1111111111",	{109, 0x09, 0x0E, 0, 0, 0},	0x0014	},
	{	"17SELPROG 2222222222",	{109, 0x09, 0x0C, 0, 0, 0},	0x0015	},
	{	"18SELPROG 3333333333",	{109, 0x09, 0x0B, 0, 0, 0},	0x0016	},
	{	"19SELPROG 4444444444",	{109, 0x09, 0x0A, 0, 0, 0},	0x0017	},
	{	"20SELPROG 5555555555",	{109, 0x09, 0x09, 0, 0, 0},	0x0018	},
};
#endif

//******************************************************************************
//	STATIC VARIABLES
//******************************************************************************
STATIC XmgrCas_NaSelProg_Mediaset_t 			s_stNaMsetSelProg;


//******************************************************************************
//	PROTOTYPE FUNCTIONS
//******************************************************************************
#define ________XMGR_CAS_NA_PPV_MEDIASET_MENU_STATIC___________________________________________
STATIC XmgrCas_NaSelProg_Mediaset_t* xmgr_cas_NaPpvSelProgGetLocalContents_Mediaset(void);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgSetMainTitle_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgScrollPage_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents, KEY_Code_t eKeyCode);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgInitUiConfiguration_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgRequestPurchaseHistory_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents);
STATIC XmgrCar_NaSelProgUiSubItem_t* xmgr_cas_NaPpvSelProgLinkTraverseListItembyIndex_Mediaset (POINTER_LIST_T **pRoot, HUINT16 usTotalItemIdx, HUINT16 usItemIdx);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawSeletedProgramsItem_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t tObject, void *pvArg, int nParam1);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawStateButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgGetPageStr_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawCredit_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawUpDownArrow_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawBackButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawNoItemLine_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgSetUiObjects_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDestroyUI_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents);
STATIC void			xmgr_cas_NaPpvSelProgLinkFreeListItemMethod_Mediaset(void *pvContents);
STATIC BUS_Result_t	xmgr_cas_NaPpvSelProgFreePurchaseHistory_Mediaset(POINTER_LIST_T **pRoot);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawTsErrorMessage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea,	OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawTsErrorExitBtn_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgUpdatePurchaseHistory_Mediaset(HUINT32 xStatus, HUINT32 xNumberOfItems, const PPV_PURCHASE_LIST_ITEM* pxPurchaseList, XmgrCas_NaSelProg_Mediaset_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgUpdateRemainedCredit_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents, TCreditAmount nCredit);


#define ________XMGR_CAS_NA_PPV_MEDIASET_MENU_PUBLIC___________________________________________

BUS_Result_t xproc_cas_NaSeletedPrograms_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#define	______
#define	_________________________STATIC_BODY______________________________
#define	______
/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgGetLocalContents_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaSelProg_Mediaset_t* xmgr_cas_NaPpvSelProgGetLocalContents_Mediaset(void)
{
	return &s_stNaMsetSelProg;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgSetMainTitle_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgSetMainTitle_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents)
{
	HxLOG_Print("+++++++++++++++\n");

	/* set, draw background */
	Settings_DrawBack(FALSE);

	/* title */
	MWC_UTIL_DvbStrcpy(pContents->ucTitle, GetStrRsc(LOC_CAS_NAGRA_MEDIASET_SELETED_PROGRAMS_TEXT_ID));
	Settings_SetWindowTitle(SETTING_GET_AP_DEPTH(eSetting_NagraPpvMediasetSelProg), LOC_CAS_NAGRA_MEDIASET_SELETED_PROGRAMS_TEXT_ID, pContents->ucTitle);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgScrollPage_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgScrollPage_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents, KEY_Code_t eKeyCode)
{
	HINT32			nOldDrawIndex = 0, nNumPerPage = 0;
	BUS_Result_t	hRes = ERR_BUS_NO_ERROR;

	if (pContents == NULL)
	{
		return ERR_NO_OBJECT;
	}

	nNumPerPage = GWM_List_GetNumOfPage(NAPPV_MEDIASET_SELPROG_SUBBTN_ID);
	nOldDrawIndex = GWM_List_GetCurrentIndex(NAPPV_MEDIASET_SELPROG_SUBBTN_ID);

	switch(eKeyCode)
	{
		case KEY_ARROWUP:
		case KEY_PAGEUP:
			hRes = GWM_List_SetCurrentIndex(NAPPV_MEDIASET_SELPROG_SUBBTN_ID, nOldDrawIndex-nNumPerPage);
			if (hRes == ERR_BUS_NO_ERROR)
			{
				pContents->ulCurPage --;
				xmgr_cas_NaPpvSelProgGetPageStr_Mediaset(pContents);
				GWM_Text_SetText(NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_ID, pContents->ucPageNum);
			}
			break;
		case KEY_ARROWDOWN:
		case KEY_PAGEDOWN:
			hRes = GWM_List_SetCurrentIndex(NAPPV_MEDIASET_SELPROG_SUBBTN_ID, nOldDrawIndex+nNumPerPage);
			if (hRes == ERR_BUS_NO_ERROR)
			{
				pContents->ulCurPage ++;
				xmgr_cas_NaPpvSelProgGetPageStr_Mediaset(pContents);
				GWM_Text_SetText(NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_ID, pContents->ucPageNum);
			}
			break;

		case KEY_OK:
		default:
			break;
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgInitUiConfiguration_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgInitUiConfiguration_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents)
{
	HUINT8 					szDate[32];

	HxLOG_Print("++++++++++++++++++++\n");
	HxSTD_memset(pContents, 0, sizeof(XmgrCas_NaSelProg_Mediaset_t));

	pContents->ulNumOfUiSubItemEvents = 0;
	pContents->pstPurchaseHistoryListItem = NULL;
	xmgr_cas_NaGetCurrentDateStr(szDate);
	sprintf(pContents->ucCurDate, "%s: %s", (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_DATE_TEXT_ID), szDate);
	xmgr_cas_NaGetScCreditStr(pContents->ucCreditStr);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgRequestPurchaseHistory_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgRequestPurchaseHistory_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents)
{
#ifdef WIN32
	PPV_PURCHASE_LIST_ITEM	*s_testItems = (PPV_PURCHASE_LIST_ITEM*)OxAP_Malloc(MAX_MSET_SELPROG_LIST_EVTITEM_NUM*sizeof(PPV_PURCHASE_LIST_ITEM));
	HUINT16 i = 0;

	for (i = 0; i < MAX_MSET_SELPROG_LIST_EVTITEM_NUM; i++)
	{
		s_testItems[i].Cost = s_stSelProgListSubItem[i].SelProgCredit;
		s_testItems[i].PurchaseTime = s_stSelProgListSubItem[i].SelProgPurchaseDate;
		s_testItems[i].EvtNameLength = SvcCas_UtilStrLen(s_stSelProgListSubItem[i].SelProgEvtName);
		HxSTD_memset(s_testItems[i].EvtName, 0, MAX_PPV_PURCHASE_LIST_NAME_LENGTH+1);
		MWC_UTIL_DvbStrncpy(s_testItems[i].EvtName, s_stSelProgListSubItem[i].SelProgEvtName, MAX_PPV_PURCHASE_LIST_NAME_LENGTH);
	}
	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_HISTORY, (Handle_t)0, eDxCAS_GROUPID_NA, (HINT32)MAX_MSET_SELPROG_LIST_EVTITEM_NUM, (HINT32)s_testItems);
#else
	xmgr_cas_NaGetPurchaseHistory(NA_MEDIASET_PROVIDER_ID);
#endif
	pContents->bWaiting = TRUE;

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgLinkTraverseListItembyIndex_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
XmgrCar_NaSelProgUiSubItem_t 	*xmgr_cas_NaPpvSelProgLinkTraverseListItembyIndex_Mediaset (POINTER_LIST_T **pRoot, HUINT16 usTotalItemIdx, HUINT16 usItemIdx)
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
		return ((XmgrCar_NaSelProgUiSubItem_t *)tmp->pvContents);
	}
	else
	{

	}

	return NULL;
}


/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgDrawSeletedProgramsItem_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawSeletedProgramsItem_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t tObject, void *pvArg, int nParam1)
{
	OSD_StockImage_t	 	stImage;

	XmgrCar_NaSelProgUiSubItem_t		*pDrawSubItemInfo = NULL;

	OSD_Rect_t				rect_list, rect_img;
	HINT32				 	nX1, nY1, nW1;
	HINT8 					szDate[128];

	//credit icon
	OSD_Rect_t				rect_icon;

	HINT32					nListItemNum, nListFocusIndex, nItemNum;
	HUINT16					i = 0;
	XmgrCas_NaSelProg_Mediaset_t	*pContents = xmgr_cas_NaPpvSelProgGetLocalContents_Mediaset();


	GWM_List_GetTotalItem(GWM_Obj_GetObjectID(tObject), &nListItemNum);
	nItemNum = GWM_List_GetNumOfPage(GWM_Obj_GetObjectID(tObject));
	nListFocusIndex = GWM_List_GetCurrentIndex(GWM_Obj_GetObjectID(tObject));
	pDrawSubItemInfo = (XmgrCar_NaSelProgUiSubItem_t*)GWM_List_GetArrayPtr((LIST_OBJECT)tObject);

	if (pDrawSubItemInfo)
	{
		for (i = 0; i < nItemNum; i++)
		{
			pDrawSubItemInfo = xmgr_cas_NaPpvSelProgLinkTraverseListItembyIndex_Mediaset(&pContents->pstPurchaseHistoryListItem, pContents->ulNumOfUiSubItemEvents, nListFocusIndex+i);
			if (pDrawSubItemInfo)
			{
				// setting text style of the evtname info list
				OSD_GFX_SetRect(&rect_list, s_Mediaset_SelProg_NaviSubItem[0].rect.x, s_Mediaset_SelProg_NaviSubItem[0].rect.y + ((i+1)*NAPPV_MEDIASET_SELPROG_SUBBTN_LINESPACE),
											s_Mediaset_SelProg_NaviSubItem[0].rect.w, s_Mediaset_SelProg_NaviSubItem[0].rect.h);
				if (MWC_UTIL_DvbStrlenStrOnly(pDrawSubItemInfo->SelProgEvtName) > 0)
				{
					OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(tObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, rect_list.x, rect_list.y, rect_list.w,
												(HINT8 *)pDrawSubItemInfo->SelProgEvtName, COL(C_T_MList_Yellow));
				}
				else
				{
					OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(tObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, rect_list.x, rect_list.y, rect_list.w,
												(HINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_PURCHASE_BLANK_EVENT_NAME_ID), COL(C_T_MList_Yellow));
				}

				// setting text style of the expirydate info list
				OSD_GFX_SetRect(&rect_list, s_Mediaset_SelProg_NaviSubItem[1].rect.x, s_Mediaset_SelProg_NaviSubItem[1].rect.y + ((i+1)*NAPPV_MEDIASET_SELPROG_SUBBTN_LINESPACE),
											s_Mediaset_SelProg_NaviSubItem[1].rect.w, s_Mediaset_SelProg_NaviSubItem[1].rect.h);
				snprintf(szDate, 128, "%02d/%02d/%02d\n", pDrawSubItemInfo->SelProgPurchaseDate.day, pDrawSubItemInfo->SelProgPurchaseDate.month, (pDrawSubItemInfo->SelProgPurchaseDate.year+1900)%100);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(tObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, rect_list.x, rect_list.y, rect_list.w,
												(HINT8 *)szDate, COL(C_T_Cursor));

				// setting text style of the credit info list
				OSD_GFX_SetRect(&rect_list, s_Mediaset_SelProg_NaviSubItem[2].rect.x, s_Mediaset_SelProg_NaviSubItem[2].rect.y + ((i+1)*NAPPV_MEDIASET_SELPROG_SUBBTN_LINESPACE),
											s_Mediaset_SelProg_NaviSubItem[2].rect.w, s_Mediaset_SelProg_NaviSubItem[2].rect.h);


				/* credit */
				if (pDrawSubItemInfo->SelProgCredit > 0)
				{
					/* credit icon */
					if (!AP_StockImg_GetImage (e911_4_24_Euro_Font_White_B, &stImage))
					{
						nX1 = rect_list.x + rect_list.w - stImage.width;
						nY1 = rect_list.y - NAPPV_MEDIASET_SELPROG_STRING_MARGIN_Y;
						OSD_GFX_SetRect (&rect_icon, nX1, nY1, stImage.width, stImage.height);
						OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &rect_icon);
					}
					nW1 = rect_list.w - stImage.width - 10;
					snprintf(szDate, 128, "%d,%02d", pDrawSubItemInfo->SelProgCredit/100, pDrawSubItemInfo->SelProgCredit%100);
				}
				else
				{
					nW1 = rect_list.w;
					snprintf(szDate, 128, "n.d.");
				}
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(tObject), eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_list.x, rect_list.y, nW1,
												(HINT8 *)szDate, COL(C_T_Cursor));

				// setting text style of the line
				OSD_GFX_SetRect(&rect_img, s_Mediaset_SelProg_NaviSubItemLine[0].rect.x, s_Mediaset_SelProg_NaviSubItemLine[0].rect.y + ((i+1)*NAPPV_MEDIASET_SELPROG_SUBBTN_LINE_LINESPACE),
											s_Mediaset_SelProg_NaviSubItemLine[0].rect.w, s_Mediaset_SelProg_NaviSubItemLine[0].rect.h);
				nX1 = pArea->x1 + rect_img.x;
				nY1 = pArea->y1 + rect_img.y;
				if (!AP_StockImg_GetImage (e914_6_24_Line2, &stImage))
				{
					OSD_GFX_SetRect (&rect_img, nX1, nY1, stImage.width, stImage.height);
					OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &rect_img);
				}
			}
		}
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgDrawBackgroundImage_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
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
 Function		: xmgr_cas_NaPpvSelProgDrawStateButton_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawStateButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e914_5_24_M_3Title_Blue, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgGetPageStr_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgGetPageStr_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents)
{
	HINT32	nCurPage, nTotalPage;

	nCurPage = pContents->ulCurPage + 1;
	nTotalPage = pContents->ulNumOfUiSubItemEvents / MAX_MSET_SELPROG_LIST_ROW_NUM + 1;
	if ((pContents->ulNumOfUiSubItemEvents % MAX_MSET_SELPROG_LIST_ROW_NUM) == 0)
	{
		nTotalPage--;
	}

	snprintf(pContents->ucPageNum, 16, "%d/%d", nCurPage, nTotalPage);

	return ERR_BUS_NO_ERROR;
}


/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgDrawCredit_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawCredit_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT8 					szDate[128];
	HINT32					nObjectId, nRemainCredit = 0;
	OSD_Rect_t				rect_text;

	//credit icon
	OSD_StockImage_t	 	stImage;
	OSD_Rect_t				rect_icon;
	HINT32					nX, nY, nW, nStrLen = 0;

	OSD_Font_t				*pFont;

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

	/* na ppv mediaset selprog credit icon */
	if (!AP_StockImg_GetImage (e915_1_24_EUR, &stImage))
	{
		nX = rect_text.x + rect_text.w - stImage.width;
		nY = rect_text.y - NAPPV_MEDIASET_SELPROG_STRING_MARGIN_Y;
		OSD_GFX_SetRect (&rect_icon, nX, nY, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &rect_icon);
	}

#if 0
	/* na ppv mediaset selprog credit  */
	for(i=0; i<pContents->ulNumOfUiSubItemEvents; i++)
	{
		if(pContents->pstUiSubItem[i].SelProgCredit > 0)
		{
			nAllCredit += pContents->pstUiSubItem[i].SelProgCredit;
		}
	}
#endif
	xmgr_cas_NaGetScCredit(&nRemainCredit);
	nW = rect_text.w - stImage.width -10;
	snprintf(szDate, 128, " %02d,%02d", nRemainCredit/100, nRemainCredit%100);
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_text.x, rect_text.y, nW,
										(HINT8 *)szDate, COL(C_T_MList_Yellow));

	/* na ppv mediaset selprog credit text*/
	//OSD_GFX_SetRect (&rect_text, NAPPV_MEDIASET_SELPROG_STATE_TEXT_X, NAPPV_MEDIASET_SELPROG_STATE_TEXT_Y, NAPPV_MEDIASET_SELPROG_STATE_TEXT_WIDTH, NAPPV_MEDIASET_SELPROG_STATE_TEXT_HEIGHT);
	pFont = OSD_FONT_Get(eFont_SystemNormal);
	nStrLen = OSD_GFX_GetStringWidth(pFont, szDate);
	nW = nW - nStrLen;
	if (pFont != NULL)
		OSD_FONT_Release(pFont);

	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_text.x, rect_text.y, nW,
										(HINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_CREDIT_TEXT_ID), COL(C_T_Cursor));

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgDrawUpDownArrow_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawUpDownArrow_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e914_2_24_Arrow_W_3UD, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgDrawBackButton_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawBackButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e914_1_24_BTN_2Back, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawNoItemLine_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32				 	nX1, nY1;
	OSD_Rect_t			 	stRect;
	OSD_StockImage_t	 	stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e914_6_24_Line2, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}


/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgSetUiObjects_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgSetUiObjects_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents)
{
	OSD_Rect_t				rect = {0, };

	/* na ppv mediaset selprog background */
	Settings_DrawBack(FALSE);
	OSD_GFX_SetRect(&rect, 0, 0, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT);
	GWM_Rect_Create(NAPPV_MEDIASET_SELPROG_LOGOBACKGROUND_IMAGE_ID, &rect, 0x00000000);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_SELPROG_LOGOBACKGROUND_IMAGE_ID, xmgr_cas_NaPpvSelProgDrawBackgroundImage_Mediaset);

	/* na ppv mediaset selprog date text*/
	OSD_GFX_SetRect (&rect, NAPPV_MEDIASET_SELPROG_DATE_TEXT_X, NAPPV_MEDIASET_SELPROG_DATE_TEXT_Y, NAPPV_MEDIASET_SELPROG_DATE_TEXT_WIDTH, NAPPV_MEDIASET_SELPROG_DATE_TEXT_HEIGHT);
	GWM_Text_Create(NAPPV_MEDIASET_SELPROG_DATE_TEXT_ID, &rect, pContents->ucCurDate);
	GWM_Text_SetAlign (NAPPV_MEDIASET_SELPROG_DATE_TEXT_ID, TEXT_ALIGN_RIGHT);
	GWM_Text_SetSize (NAPPV_MEDIASET_SELPROG_DATE_TEXT_ID, FONT_SIZE_S);
	GWM_Text_ChangeColor (NAPPV_MEDIASET_SELPROG_DATE_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv mediaset selprog infotext*/
	OSD_GFX_SetRect (&rect, NAPPV_MEDIASET_SELPROG_INFO_TEXT_X, NAPPV_MEDIASET_SELPROG_INFO_TEXT_Y, NAPPV_MEDIASET_SELPROG_INFO_TEXT_WIDTH, NAPPV_MEDIASET_SELPROG_INFO_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_MEDIASET_SELPROG_INFO_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_SELETED_PROGRAMS_TEXT_ID));
	GWM_Text_SetAlign (NAPPV_MEDIASET_SELPROG_INFO_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_MEDIASET_SELPROG_INFO_TEXT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor(NAPPV_MEDIASET_SELPROG_INFO_TEXT_ID, COL (C_T_Cursor), 0x00);

	/* na ppv mediaset selprog credit button */
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_SELPROG_STATE_BTN_X, NAPPV_MEDIASET_SELPROG_STATE_BTN_Y, NAPPV_MEDIASET_SELPROG_STATE_BTN_WIDTH, NAPPV_MEDIASET_SELPROG_STATE_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_SELPROG_STATE_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_SELPROG_STATE_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvSelProgDrawStateButton_Mediaset);

	/* na ppv mediaset selprog credit */
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_SELPROG_CREDIT_BTN_X, NAPPV_MEDIASET_SELPROG_CREDIT_BTN_Y, NAPPV_MEDIASET_SELPROG_CREDIT_BTN_WIDTH, NAPPV_MEDIASET_SELPROG_CREDIT_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_SELPROG_CREDIT_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_SELPROG_CREDIT_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvSelProgDrawCredit_Mediaset);

	/* na ppv mediaset selprog back button */
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_SELPROG_BACK_BTN_X, NAPPV_MEDIASET_SELPROG_BACK_BTN_Y, NAPPV_MEDIASET_SELPROG_BACK_BTN_WIDTH, NAPPV_MEDIASET_SELPROG_BACK_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_SELPROG_BACK_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_SELPROG_BACK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvSelProgDrawBackButton_Mediaset);

	HxLOG_Print(" --- \n");

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgDestroyUI_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDestroyUI_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents)
{
	if (pContents->pstPurchaseHistoryListItem)
	{
		xmgr_cas_NaPpvSelProgFreePurchaseHistory_Mediaset(&pContents->pstPurchaseHistoryListItem);
		pContents->pstPurchaseHistoryListItem = NULL;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC void	xmgr_cas_NaPpvSelProgLinkFreeListItemMethod_Mediaset(void *pvContents)
{
	if(pvContents == NULL)
	{
		HxLOG_Critical("Critical Eror!!!\n");
		return;
	}

	OxAP_Free(pvContents);
}

STATIC BUS_Result_t	xmgr_cas_NaPpvSelProgFreePurchaseHistory_Mediaset(POINTER_LIST_T **pRoot)
{
	*pRoot = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(*pRoot, xmgr_cas_NaPpvSelProgLinkFreeListItemMethod_Mediaset);

	if(*pRoot != NULL)
	{
		HxLOG_Critical("\n\n");
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawTsErrorMessage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32					nObjectId;
	OSD_Rect_t				rect_text;

	OSD_Font_t				*pFont;

	//----------------------------------------------------------------
	rect_text.x = pRect->x;
	rect_text.y = pRect->y - NAPPV_MEDIASET_SELPROG_STRING_MARGIN_Y;
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

	OSD_GFX_DrawMultiLineStringAlign(pScreen, pFont, rect_text.x, rect_text.y, rect_text.w, rect_text.h, (HINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_MENU_MSG_NO_PACKAGE_NAME_ID),
		NAPPV_MEDIASET_SELPROG_TS_ERROR_TEXT_LINESPACE-FONT_SIZE_M, COL(C_T_Cursor), TEXT_ALIGN_CENTER|TEXT_MULTILINED);
	if (pFont != NULL)
	{
		OSD_FONT_Release(pFont);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgDrawTsErrorExitBtn_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgDrawTsErrorExitBtn_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e914_1_24_BTN_3Exit, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}


/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgUpdatePurchaseHistory_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgUpdatePurchaseHistory_Mediaset(HUINT32 xStatus, HUINT32 xNumberOfItems, const PPV_PURCHASE_LIST_ITEM* pxPurchaseList, XmgrCas_NaSelProg_Mediaset_t *pContents)
{
	HUINT16					index = 0;
	XmgrCar_NaSelProgUiSubItem_t		*ptItem;
	OSD_Rect_t				rect;

	if (pContents->bWaiting != TRUE)
	{
		return MESSAGE_BREAK;
	}

	pContents->bWaiting = FALSE;

	if (pContents->pstPurchaseHistoryListItem)
	{
		xmgr_cas_NaPpvSelProgFreePurchaseHistory_Mediaset(&pContents->pstPurchaseHistoryListItem);
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
				ptItem = (XmgrCar_NaSelProgUiSubItem_t *)OxAP_Malloc(sizeof(XmgrCar_NaSelProgUiSubItem_t));
				HxSTD_memset(ptItem->SelProgEvtName, 0, MAX_MSET_SELPROG_LIST_EVTNAME_LENGTH + 1);
				MWC_UTIL_DvbStrncpy(ptItem->SelProgEvtName, pxPurchaseList[index].EvtName, MAX_MSET_SELPROG_LIST_EVTNAME_LENGTH);
				ptItem->SelProgPurchaseDate = pxPurchaseList[index].PurchaseTime;
				ptItem->SelProgCredit = pxPurchaseList[index].Cost;
				pContents->pstPurchaseHistoryListItem = UTIL_LINKEDLIST_AppendListItemToPointerList(pContents->pstPurchaseHistoryListItem, ptItem);
			}

			/* purchase history List Item을 생성한다. */
			OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_SELPROG_SUBBTN_X, NAPPV_MEDIASET_SELPROG_SUBBTN_Y, NAPPV_MEDIASET_SELPROG_SUBBTN_WIDTH, (NAPPV_MEDIASET_SELPROG_SUBBTN_HEIGHT * MAX_MSET_SELPROG_LIST_ROW_NUM) + 16);
			GWM_List_Create(NAPPV_MEDIASET_SELPROG_SUBBTN_ID, &rect, NULL, NAPPV_MEDIASET_SELPROG_SUBBTN_HEIGHT, pContents->ulNumOfUiSubItemEvents, /* page count */ MAX_MSET_SELPROG_LIST_ROW_NUM, ((GWM_LIST_ITEMS)pContents->pstPurchaseHistoryListItem));
			GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_SELPROG_SUBBTN_ID, xmgr_cas_NaPpvSelProgDrawSeletedProgramsItem_Mediaset);

			if (pContents->ulNumOfUiSubItemEvents)
			{
				/* na ppv mediaset selprog arrow */
				OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_SELPROG_ARROW_BTN_X, NAPPV_MEDIASET_SELPROG_ARROW_BTN_Y, NAPPV_MEDIASET_SELPROG_ARROW_BTN_WIDTH, NAPPV_MEDIASET_SELPROG_ARROW_BTN_HEIGHT);
				GWM_Frame_Create(NAPPV_MEDIASET_SELPROG_ARROW_BTN_ID, &rect, NULL);
				GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_SELPROG_ARROW_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvSelProgDrawUpDownArrow_Mediaset);

				/* na ppv mediaset selprog list page button */
				xmgr_cas_NaPpvSelProgGetPageStr_Mediaset(pContents);
				OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_X, NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_Y-FONT_SIZE_XS, NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_WIDTH, NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_HEIGHT);
				GWM_Text_Create(NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_ID, &rect, pContents->ucPageNum);
				GWM_Text_SetAlign (NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_ID, TEXT_ALIGN_RIGHT);
				GWM_Text_SetSize (NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_ID, FONT_SIZE_XS);
				GWM_Text_ChangeColor(NAPPV_MEDIASET_SELPROG_LISTPAGE_TEXT_ID, COL (C_T_Cursor), 0x00);
			}
			else
			{
				// no item string display
			}

#ifdef WIN32
			OxAP_Free(pxPurchaseList);
#endif

			GWM_Obj_SetFocus(NAPPV_MEDIASET_SELPROG_SUBBTN_ID);
		}
		else
		{
			// no item string display
			OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_SELPROG_SUBBTN01_X, NAPPV_MEDIASET_SELPROG_SUBBTN01_Y + NAPPV_MEDIASET_SELPROG_SUBBTN01_HEIGHT - FONT_SIZE_M, NAPPV_MEDIASET_SELPROG_SUBBTN01_WIDTH, NAPPV_MEDIASET_SELPROG_SUBBTN01_HEIGHT);
			GWM_Text_Create(NAPPV_MEDIASET_SELPROG_NO_ITEM_TEXT, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_SELPROG_NODATA_TEXT_ID));
			GWM_Text_SetAlign (NAPPV_MEDIASET_SELPROG_NO_ITEM_TEXT, TEXT_ALIGN_LEFT);
			GWM_Text_SetSize (NAPPV_MEDIASET_SELPROG_NO_ITEM_TEXT, FONT_SIZE_M);
			GWM_Text_ChangeColor(NAPPV_MEDIASET_SELPROG_NO_ITEM_TEXT, COL (C_T_MList_Yellow), 0x00);

			// under line
			OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_SELPROG_SUBBTN01_LINE_X, NAPPV_MEDIASET_SELPROG_SUBBTN01_LINE_Y + NAPPV_MEDIASET_SELPROG_SUBBTN_LINE_LINESPACE, NAPPV_MEDIASET_SELPROG_SUBBTN01_LINE_WIDTH, NAPPV_MEDIASET_SELPROG_SUBBTN01_LINE_HEIGHT);
			GWM_Frame_Create(NAPPV_MEDIASET_SELPROG_NO_ITEM_LINE, &rect, NULL);
			GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_SELPROG_NO_ITEM_LINE, (GWM_ObjDraw_t)xmgr_cas_NaPpvSelProgDrawNoItemLine_Mediaset);
		}
	}
	else
	{
		if (GWM_Obj_GetObjectByID(NAPPV_MEDIASET_SELPROG_STATE_BTN_ID))
		{
			GWM_Obj_Destroy(NAPPV_MEDIASET_SELPROG_STATE_BTN_ID);
		}
		if (GWM_Obj_GetObjectByID(NAPPV_MEDIASET_SELPROG_INFO_TEXT_ID))
		{
			GWM_Obj_Destroy(NAPPV_MEDIASET_SELPROG_INFO_TEXT_ID);
		}
		if (GWM_Obj_GetObjectByID(NAPPV_MEDIASET_SELPROG_CREDIT_BTN_ID))
		{
			GWM_Obj_Destroy(NAPPV_MEDIASET_SELPROG_CREDIT_BTN_ID);
		}
		if (GWM_Obj_GetObjectByID(NAPPV_MEDIASET_SELPROG_BACK_BTN_ID))
		{
			GWM_Obj_Destroy(NAPPV_MEDIASET_SELPROG_BACK_BTN_ID);
		}

		// ts error string display
		OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_SELPROG_TS_ERROR_TEXT_X, NAPPV_MEDIASET_SELPROG_TS_ERROR_TEXT_Y, NAPPV_MEDIASET_SELPROG_TS_ERROR_TEXT_WIDTH, NAPPV_MEDIASET_SELPROG_TS_ERROR_TEXT_HEIGHT);
		GWM_Text_Create(NAPPV_MEDIASET_SELPROG_TS_ERROR_TEXT_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_SELPROG_TS_ERROR_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvSelProgDrawTsErrorMessage_Mediaset);

		// ts error exit button
		OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_X, NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_Y, NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_WIDTH, NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_HEIGHT);
		GWM_Frame_Create(NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_ID, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_SELPROG_TS_ERROR_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvSelProgDrawTsErrorExitBtn_Mediaset);
	}
}

/********************************************************************
 Function		: xmgr_cas_NaPpvSelProgUpdateRemainedCredit_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvSelProgUpdateRemainedCredit_Mediaset(XmgrCas_NaSelProg_Mediaset_t *pContents, TCreditAmount nCredit)
{
	GWM_APP_InvalidateObject(NAPPV_MEDIASET_SELPROG_CREDIT_BTN_ID);

	return ERR_BUS_NO_ERROR;
}


#define	______
#define	_________________________PUBLIC_BODY______________________________
#define	______
/******************************************************************************/
/**
*   \fn     xproc_cas_NaSeletedPrograms_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaSeletedPrograms_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaSelProg_Mediaset_t	*pContents = xmgr_cas_NaPpvSelProgGetLocalContents_Mediaset();
	HUINT32					nResult = ERR_FAIL;

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_MEDIASET_SELPROG_WINDOW_X, NAPPV_MEDIASET_SELPROG_WINDOW_Y,
								NAPPV_MEDIASET_SELPROG_WINDOW_WIDTH, NAPPV_MEDIASET_SELPROG_WINDOW_HEIGHT);

			xmgr_cas_NaPpvSelProgInitUiConfiguration_Mediaset(pContents);
			xmgr_cas_NaPpvSelProgSetMainTitle_Mediaset(pContents);

			// update selprog information
			xmgr_cas_NaPpvSelProgRequestPurchaseHistory_Mediaset(pContents);

			// draw UI object
			xmgr_cas_NaPpvSelProgSetUiObjects_Mediaset(pContents);

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
				GWM_APP_InvalidateObject(NAPPV_MEDIASET_SELPROG_STATUS_ID);
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
					BUS_MGR_Create("xmgr_cas_NaMSET_MenuSmartCardError_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xmgr_cas_NaMSET_MenuSmartCardError_Proc, 0, eAlarmType, 0, 0);

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
				xmgr_cas_NaPpvSelProgUpdatePurchaseHistory_Mediaset((HUINT32)hAction, (HUINT32)p2, p3, pContents);
			}
			return MESSAGE_BREAK;

		case eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT:
			HxLOG_Print("eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT...\n");
			if(p1 == eDxCAS_GROUPID_NA)
			{
				xmgr_cas_NaPpvSelProgUpdateRemainedCredit_Mediaset(pContents, (TCreditAmount)p3);
			}
			break;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_KEYUP :
			AP_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN :
			AP_KeyConv_UiCommon(&p1);

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
					BUS_MGR_Create("xproc_cas_NaPpvMenu_Mediaset", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xproc_cas_NaPpvMenu_Mediaset, 0, 0, 0, 0);
					return MESSAGE_BREAK;

				case KEY_ARROWUP:
				case KEY_ARROWDOWN:
				case KEY_PAGEUP:
				case KEY_PAGEDOWN:
					xmgr_cas_NaPpvSelProgScrollPage_Mediaset(pContents, p1);
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
			xmgr_cas_NaPpvSelProgDestroyUI_Mediaset(pContents);

			/* clear front PANEL */
			//AP_SPEC_ClearMenuPanelString();
			break;

		default:
			break;
	}

	HxLOG_Info("xproc_cas_NaSeletedPrograms_Mediaset handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif	//end of #if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
/* End of File */
