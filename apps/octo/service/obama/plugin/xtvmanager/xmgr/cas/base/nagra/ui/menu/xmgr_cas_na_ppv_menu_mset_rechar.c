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
	eCasNaMsetReChar_Name,
	eCasNaMsetReChar_Date,
	eCasNaMsetReChar_Credit,
	eCasNaMsetReChar_Item_Max
} XmgrCas_NaReCharItem_e;

#define	MAX_MSET_RECHAR_LIST_ROW_NUM					5
#define	MAX_MSET_RECHAR_LIST_EVTNAME_LENGTH				50

typedef struct tagRecharHistorySubItem
{
	HUINT8*					ReCharReasonStr;
	TCalendarTime			ReCharDate;
	TCreditAmount			ReCharCredit;
} XmgrCas_NaReCharUiSubItem_t;

typedef struct tag_CAS_NA_MEDIASET_RH_Contents
{
	/**@breif Specific value */
	HUINT8					ucTitle[MAX_ELEMENT_TEXT_LENGTH]; 					///< title of app context
	HUINT8					ucCurDate[32];
	HUINT8					ucPageNum[16];
	HUINT8					ucCreditStr[16];

	HUINT16					ulNumOfUiSubItemEvents; /*item 갯수*/
	HUINT16					ulCurPage;
	HBOOL					bWaiting;

	POINTER_LIST_T			*pstRechargeDataListItem;
} XmgrCas_NaReChar_Mediaset_t;

#define	NAPPV_MEDIASET_RECHAR_WINDOW_WIDTH						OSD_SCREEN_WIDTH
#define	NAPPV_MEDIASET_RECHAR_WINDOW_HEIGHT						OSD_SCREEN_HEIGHT
#define	NAPPV_MEDIASET_RECHAR_WINDOW_X							0
#define	NAPPV_MEDIASET_RECHAR_WINDOW_Y							0
#define	NAPPV_MEDIASET_RECHAR_STRING_MARGIN_Y					18

#define	NAPPV_MEDIASET_RECHAR_APP_ID							eCasNaMsetReChar_Name

/* ppv mediaset rechar background */
#define	NAPPV_MEDIASET_RECHAR_LOGOBACKGROUND_IMAGE_ID			(NAPPV_MEDIASET_RECHAR_APP_ID|0x0010)

#define	NAPPV_MEDIASET_RECHAR_STATE_BTN_ID						(NAPPV_MEDIASET_RECHAR_APP_ID|0x0020)
#define	NAPPV_MEDIASET_RECHAR_STATE_BTN_X						598
#define	NAPPV_MEDIASET_RECHAR_STATE_BTN_Y						189
#define	NAPPV_MEDIASET_RECHAR_STATE_BTN_WIDTH					546
#define	NAPPV_MEDIASET_RECHAR_STATE_BTN_HEIGHT					50

/* ppv mediaset rechar date */
#define	NAPPV_MEDIASET_RECHAR_DATE_TEXT_ID						(NAPPV_MEDIASET_RECHAR_APP_ID|0x0030)
#define	NAPPV_MEDIASET_RECHAR_DATE_TEXT_X						830
#define	NAPPV_MEDIASET_RECHAR_DATE_TEXT_Y						(122 - FONT_SIZE_S)
#define	NAPPV_MEDIASET_RECHAR_DATE_TEXT_WIDTH					300
#define	NAPPV_MEDIASET_RECHAR_DATE_TEXT_HEIGHT					FONT_SIZE_S

/* ppv mediaset rechar info text */
#define	NAPPV_MEDIASET_RECHAR_INFO_TEXT_ID						(NAPPV_MEDIASET_RECHAR_APP_ID|0x0040)
#define	NAPPV_MEDIASET_RECHAR_INFO_TEXT_X						146
#define	NAPPV_MEDIASET_RECHAR_INFO_TEXT_Y						(225 - FONT_SIZE_M) //253
#define	NAPPV_MEDIASET_RECHAR_INFO_TEXT_WIDTH					410
#define	NAPPV_MEDIASET_RECHAR_INFO_TEXT_HEIGHT					FONT_SIZE_M

/* ppv mediaset rechar credit */
#define	NAPPV_MEDIASET_RECHAR_CREDIT_BTN_ID						(NAPPV_MEDIASET_RECHAR_APP_ID|0x0050)
#define	NAPPV_MEDIASET_RECHAR_CREDIT_BTN_X						628
#define	NAPPV_MEDIASET_RECHAR_CREDIT_BTN_Y						225
#define	NAPPV_MEDIASET_RECHAR_CREDIT_BTN_WIDTH					476
#define	NAPPV_MEDIASET_RECHAR_CREDIT_BTN_HEIGHT				36

/* ppv mediaset rechar arrow button */
#define	NAPPV_MEDIASET_RECHAR_ARROW_BTN_ID						(NAPPV_MEDIASET_RECHAR_APP_ID|0x0070)
#define	NAPPV_MEDIASET_RECHAR_ARROW_BTN_X						1115
#define	NAPPV_MEDIASET_RECHAR_ARROW_BTN_Y						541
#define	NAPPV_MEDIASET_RECHAR_ARROW_BTN_WIDTH					30
#define	NAPPV_MEDIASET_RECHAR_ARROW_BTN_HEIGHT					12

/* ppv mediaset rechar back button */
#define	NAPPV_MEDIASET_RECHAR_BACK_BTN_ID						(NAPPV_MEDIASET_RECHAR_APP_ID|0x0080)
#define	NAPPV_MEDIASET_RECHAR_BACK_BTN_X						1079
#define	NAPPV_MEDIASET_RECHAR_BACK_BTN_Y						611
#define	NAPPV_MEDIASET_RECHAR_BACK_BTN_WIDTH					72
#define	NAPPV_MEDIASET_RECHAR_BACK_BTN_HEIGHT					32

/* ppv mediaset rechar event Info list page */
#define	NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_ID					(NAPPV_MEDIASET_RECHAR_APP_ID|0x00a0)
#define	NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_X					1060
#define	NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_Y					553
#define	NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_WIDTH				43
#define	NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_HEIGHT				21

#define	NAPPV_MEDIASET_RECHAR_STATUS_ID							(NAPPV_MEDIASET_RECHAR_APP_ID|0x00b0) //update 시 처리 부분.....

#define __________SUBITEM_LINE_OBJECTS_________________________________________
#define	NAPPV_MEDIASET_RECHAR_SUBBTN_LINE_HEIGHT					2
#define	NAPPV_MEDIASET_RECHAR_SUBBTN_LINE_LINESPACE				44

/* ppv mediaset rechar line position */
#define	NAPPV_MEDIASET_RECHAR_SUBBTN01_LINE_ID					(NAPPV_MEDIASET_RECHAR_APP_ID|0x0110)
#define	NAPPV_MEDIASET_RECHAR_SUBBTN01_LINE_WIDTH				1008
#define	NAPPV_MEDIASET_RECHAR_SUBBTN01_LINE_HEIGHT				NAPPV_MEDIASET_RECHAR_SUBBTN_LINE_HEIGHT
#define	NAPPV_MEDIASET_RECHAR_SUBBTN01_LINE_X					136
#define	NAPPV_MEDIASET_RECHAR_SUBBTN01_LINE_Y					309 - 44

#define __________SUBITEM_OBJECTS_________________________________________
#define	NAPPV_MEDIASET_RECHAR_SUBBTN_ID							(NAPPV_MEDIASET_RECHAR_APP_ID|0x0200)
#define	NAPPV_MEDIASET_RECHAR_SUBBTN_WIDTH						1008
#define	NAPPV_MEDIASET_RECHAR_SUBBTN_HEIGHT						44
#define	NAPPV_MEDIASET_RECHAR_SUBBTN_X							136
#define	NAPPV_MEDIASET_RECHAR_SUBBTN_Y							(299 - NAPPV_MEDIASET_RECHAR_SUBBTN_HEIGHT)
#define	NAPPV_MEDIASET_RECHAR_SUBBTN_LINESPACE					44

/* ppv mediaset rechar btn 01 position */
#define	NAPPV_MEDIASET_RECHAR_SUBBTN01_ID						(NAPPV_MEDIASET_RECHAR_APP_ID|0x0210)
#define	NAPPV_MEDIASET_RECHAR_SUBBTN01_WIDTH					634
#define	NAPPV_MEDIASET_RECHAR_SUBBTN01_HEIGHT				NAPPV_MEDIASET_RECHAR_SUBBTN_HEIGHT
#define	NAPPV_MEDIASET_RECHAR_SUBBTN01_X						146
#define	NAPPV_MEDIASET_RECHAR_SUBBTN01_Y						(299 - NAPPV_MEDIASET_RECHAR_SUBBTN_HEIGHT)

/* ppv mediaset rechar btn 02 position */
#define	NAPPV_MEDIASET_RECHAR_SUBBTN02_ID						(NAPPV_MEDIASET_RECHAR_APP_ID|0x0220)
#define	NAPPV_MEDIASET_RECHAR_SUBBTN02_WIDTH					140
#define	NAPPV_MEDIASET_RECHAR_SUBBTN02_HEIGHT				NAPPV_MEDIASET_RECHAR_SUBBTN_HEIGHT
#define	NAPPV_MEDIASET_RECHAR_SUBBTN02_X						800
#define	NAPPV_MEDIASET_RECHAR_SUBBTN02_Y						(299 - NAPPV_MEDIASET_RECHAR_SUBBTN_HEIGHT)

/* ppv mediaset rechar btn 03 position */
#define	NAPPV_MEDIASET_RECHAR_SUBBTN03_ID						(NAPPV_MEDIASET_RECHAR_APP_ID|0x0230)
#define	NAPPV_MEDIASET_RECHAR_SUBBTN03_WIDTH					174
#define	NAPPV_MEDIASET_RECHAR_SUBBTN03_HEIGHT					NAPPV_MEDIASET_RECHAR_SUBBTN_HEIGHT
#define	NAPPV_MEDIASET_RECHAR_SUBBTN03_X						960
#define	NAPPV_MEDIASET_RECHAR_SUBBTN03_Y						(299 - NAPPV_MEDIASET_RECHAR_SUBBTN_HEIGHT)

#define NAPPV_MEDIASET_RECHAR_NO_ITEM_TEXT						(NAPPV_MEDIASET_RECHAR_APP_ID|0x0240)
#define NAPPV_MEDIASET_RECHAR_NO_ITEM_LINE						(NAPPV_MEDIASET_RECHAR_APP_ID|0x0250)

STATIC Item_t s_Mediaset_ReChar_NaviSubItem[eCasNaMsetReChar_Item_Max] =
{
/* 01 */	{NAPPV_MEDIASET_RECHAR_SUBBTN01_ID, { NAPPV_MEDIASET_RECHAR_SUBBTN01_X, NAPPV_MEDIASET_RECHAR_SUBBTN01_Y, NAPPV_MEDIASET_RECHAR_SUBBTN01_WIDTH, NAPPV_MEDIASET_RECHAR_SUBBTN01_HEIGHT}},
/* 02 */	{NAPPV_MEDIASET_RECHAR_SUBBTN02_ID, { NAPPV_MEDIASET_RECHAR_SUBBTN02_X, NAPPV_MEDIASET_RECHAR_SUBBTN02_Y, NAPPV_MEDIASET_RECHAR_SUBBTN02_WIDTH, NAPPV_MEDIASET_RECHAR_SUBBTN02_HEIGHT}},
/* 03 */	{NAPPV_MEDIASET_RECHAR_SUBBTN03_ID, { NAPPV_MEDIASET_RECHAR_SUBBTN03_X, NAPPV_MEDIASET_RECHAR_SUBBTN03_Y, NAPPV_MEDIASET_RECHAR_SUBBTN03_WIDTH, NAPPV_MEDIASET_RECHAR_SUBBTN03_HEIGHT}},
};

STATIC Item_t s_Mediaset_ReChar_NaviSubItemLine[1] =
{
/* 01 */	{NAPPV_MEDIASET_RECHAR_SUBBTN01_LINE_ID, { NAPPV_MEDIASET_RECHAR_SUBBTN01_LINE_X, NAPPV_MEDIASET_RECHAR_SUBBTN01_LINE_Y, NAPPV_MEDIASET_RECHAR_SUBBTN01_LINE_WIDTH, NAPPV_MEDIASET_RECHAR_SUBBTN01_LINE_HEIGHT}},
};

#ifdef WIN32
#define	MAX_MSET_RECHAR_LIST_EVTITEM_NUM				20

STATIC RECHARGE_LIST_ITEM s_stReCharListSubItem[MAX_MSET_RECHAR_LIST_EVTITEM_NUM] =
{
	{	0,	{109, 0x09, 0x1E, 0, 0, 0},	0x0010	},
	{	1,	{109, 0x09, 0x1C, 0, 0, 0},	0x0020	},
	{	2,	{109, 0x09, 0x1B, 0, 0, 0},	0x0030	},
	{	0,	{109, 0x09, 0x1A, 0, 0, 0},	0x0040	},
	{	1,	{109, 0x09, 0x19, 0, 0, 0},	0x0050	},

	{	2,	{109, 0x09, 0x18, 0, 0, 0},	0x0060	},
	{	0,	{109, 0x09, 0x17, 0, 0, 0},	0x0070	},
	{	1,	{109, 0x09, 0x16, 0, 0, 0},	0x0080	},
	{	2,	{109, 0x09, 0x15, 0, 0, 0},	0x0090	},
	{	0,	{109, 0x09, 0x14, 0, 0, 0},	0x0100	},

	{	1,	{109, 0x09, 0x13, 0, 0, 0},	0x0110	},
	{	2,	{109, 0x09, 0x12, 0, 0, 0},	0x0120	},
	{	0,	{109, 0x09, 0x11, 0, 0, 0},	0x0130	},
	{	1,	{109, 0x09, 0x10, 0, 0, 0},	0x0140	},
	{	2,	{109, 0x09, 0x0F, 0, 0, 0},	0x0150	},

	{	0,	{109, 0x09, 0x0E, 0, 0, 0},	0x0160	},
	{	1,	{109, 0x09, 0x0C, 0, 0, 0},	0x0170	},
	{	2,	{109, 0x09, 0x0B, 0, 0, 0},	0x0180	},
	{	0,	{109, 0x09, 0x0A, 0, 0, 0},	0x0190	},
	{	1,	{109, 0x09, 0x09, 0, 0, 0},	0x7A120	},
};
#endif
//******************************************************************************
//	STATIC VARIABLES
//******************************************************************************
STATIC XmgrCas_NaReChar_Mediaset_t 			s_stNaMsetReChar;


//******************************************************************************
//	PROTOTYPE FUNCTIONS
//******************************************************************************
#define ________XMGR_CAS_NA_PpvMEDIASET_MENU_STATIC___________________________________________
STATIC XmgrCas_NaReChar_Mediaset_t* xmgr_cas_NaPpvRecharGetLocalContents_Mediaset(void);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharSetMainTitle_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharScrollPage_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents, KEY_Code_t eKeyCode);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharInitUiConfiguration_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharRequestRechargeList_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents);
STATIC XmgrCas_NaReCharUiSubItem_t* xmgr_cas_NaPpvRecharLinkTraverseListItemByIndex_Mediaset (POINTER_LIST_T **pRoot, HUINT16 usTotalItemIdx, HUINT16 usItemIdx);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDrawRechargeListItem_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDrawBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDrawStateButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharGetPageStr_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDrawCredit_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDrawUpDownArrow_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDrawBackButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDrawNoItemLine_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharSetUiObjects_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDestpruUi_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents);
STATIC void			xmgr_cas_NaPpvRecharLinkFreeListItemMethod_Mediaset(void *pvContents);
STATIC BUS_Result_t	xmgr_cas_NaPpvRecharFreeRechargeList_Mediaset(POINTER_LIST_T **pRoot);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharUpdateRechargeList_Mediaset(HUINT32 xStatus, HUINT32 xNumberOfItems, const RECHARGE_LIST_ITEM* pxRechargeList, XmgrCas_NaReChar_Mediaset_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvRecharUpdateRemainedCredit_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents, TCreditAmount nCredit);


#define ________XMGR_CAS_NA_PpvMEDIASET_MENU_PUBLIC___________________________________________
BUS_Result_t xproc_cas_NaRechargeHistory_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#define	______
#define	_________________________STATIC_BODY______________________________
#define	______
/********************************************************************
 Function		: xmgr_cas_NaPpvRecharGetLocalContents_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaReChar_Mediaset_t* xmgr_cas_NaPpvRecharGetLocalContents_Mediaset(void)
{
	return &s_stNaMsetReChar;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvRecharSetMainTitle_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharSetMainTitle_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents)
{
	HxLOG_Print("+++++++++++++++\n");

	/* set, draw background */
	Settings_DrawBack(FALSE);

	/* title */
	MWC_UTIL_DvbStrcpy(pContents->ucTitle, GetStrRsc(LOC_CAS_NAGRA_MEDIASET_RECHARGE_HISTORY_TEXT_ID));
	Settings_SetWindowTitle(SETTING_GET_AP_DEPTH(eSetting_NagraPpvMediasetReChar), LOC_CAS_NAGRA_MEDIASET_RECHARGE_HISTORY_TEXT_ID, pContents->ucTitle);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvRecharScrollPage_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharScrollPage_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents, KEY_Code_t eKeyCode)
{
	HINT32			nOldDrawIndex = 0, nOldListIndex = 0, nNumPerPage = 0;
	BUS_Result_t	hRes = ERR_BUS_NO_ERROR;

	if (pContents == NULL)
	{
		return ERR_NO_OBJECT;
	}

	nNumPerPage = GWM_List_GetNumOfPage(NAPPV_MEDIASET_RECHAR_SUBBTN_ID);
	nOldDrawIndex = GWM_List_GetCurrentIndex(NAPPV_MEDIASET_RECHAR_SUBBTN_ID);

	switch(eKeyCode)
	{
		case KEY_ARROWUP:
		case KEY_PAGEUP:
			hRes = GWM_List_SetCurrentIndex(NAPPV_MEDIASET_RECHAR_SUBBTN_ID, nOldDrawIndex-nNumPerPage);
			if (hRes == ERR_BUS_NO_ERROR)
			{
				pContents->ulCurPage --;
				xmgr_cas_NaPpvRecharGetPageStr_Mediaset(pContents);
				GWM_Text_SetText(NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_ID, pContents->ucPageNum);
			}
			break;
		case KEY_ARROWDOWN:
		case KEY_PAGEDOWN:
			hRes = GWM_List_SetCurrentIndex(NAPPV_MEDIASET_RECHAR_SUBBTN_ID, nOldDrawIndex+nNumPerPage);
			if (hRes == ERR_BUS_NO_ERROR)
			{
				pContents->ulCurPage ++;
				xmgr_cas_NaPpvRecharGetPageStr_Mediaset(pContents);
				GWM_Text_SetText(NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_ID, pContents->ucPageNum);
			}
			break;

		case KEY_OK:
		default:
			break;
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvRecharInitUiConfiguration_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharInitUiConfiguration_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents)
{
	HUINT8 					szDate[32];

	HxLOG_Print("++++++++++++++++++++\n");
	HxSTD_memset(pContents, 0, sizeof(XmgrCas_NaReChar_Mediaset_t));

	pContents->ulNumOfUiSubItemEvents = 0;
	pContents->pstRechargeDataListItem = NULL;
	xmgr_cas_NaGetCurrentDateStr(szDate);
	snprintf(pContents->ucCurDate, 32, "%s: %s", (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_DATE_TEXT_ID), szDate);
	xmgr_cas_NaGetScCreditStr(pContents->ucCreditStr);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvRecharRequestRechargeList_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharRequestRechargeList_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents)
{
#ifdef WIN32
	RECHARGE_LIST_ITEM	*s_testItems = (RECHARGE_LIST_ITEM*)OxAP_Malloc(MAX_MSET_RECHAR_LIST_EVTITEM_NUM*sizeof(RECHARGE_LIST_ITEM));
	HUINT16 i = 0;

	for (i = 0; i < MAX_MSET_RECHAR_LIST_EVTITEM_NUM; i++)
	{
		s_testItems[i].Amount = s_stReCharListSubItem[i].Amount;
		s_testItems[i].Date = s_stReCharListSubItem[i].Date;
		s_testItems[i].Reason = s_stReCharListSubItem[i].Reason;
	}
	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_RECHARGE_LIST, (Handle_t)0, eDxCAS_GROUPID_NA, (HINT32)MAX_MSET_RECHAR_LIST_EVTITEM_NUM, (HINT32)s_testItems);
#else
	xmgr_cas_NaGetRechargeList();
#endif

	pContents->bWaiting = TRUE;

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvRecharLinkTraverseListItemByIndex_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
XmgrCas_NaReCharUiSubItem_t 	*xmgr_cas_NaPpvRecharLinkTraverseListItemByIndex_Mediaset (POINTER_LIST_T **pRoot, HUINT16 usTotalItemIdx, HUINT16 usItemIdx)
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
		return ((XmgrCas_NaReCharUiSubItem_t *)tmp->pvContents);
	}
	else
	{

	}

	return NULL;
}

/********************************************************************
 Function		: xmgr_cas_Nappv_mset_rechar_DrawRechargeHistoryItem
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDrawRechargeListItem_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
										OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
{
	OSD_StockImage_t	 	stImage;

	XmgrCas_NaReCharUiSubItem_t		*pDrawSubItemInfo = NULL;

	OSD_Rect_t				rect_list, rect_img;
	HINT32				 	nX1, nY1, nW1;
	HINT8 					szDate[128];

	//credit icon
	OSD_Rect_t				rect_icon;

	HINT32					nListItemNum, nListFocusIndex, nItemNum;
	HUINT16					i = 0;
	XmgrCas_NaReChar_Mediaset_t	*pContents = xmgr_cas_NaPpvRecharGetLocalContents_Mediaset();


	GWM_List_GetTotalItem(GWM_Obj_GetObjectID(pObject), &nListItemNum);
	nItemNum = GWM_List_GetNumOfPage(GWM_Obj_GetObjectID(pObject));
	nListFocusIndex = GWM_List_GetCurrentIndex(GWM_Obj_GetObjectID(pObject));
	pDrawSubItemInfo = (XmgrCas_NaReCharUiSubItem_t*)GWM_List_GetArrayPtr((LIST_OBJECT)pObject);

	if (pDrawSubItemInfo)
	{
		for (i = 0; i < nItemNum; i++)
		{
			pDrawSubItemInfo = xmgr_cas_NaPpvRecharLinkTraverseListItemByIndex_Mediaset(&pContents->pstRechargeDataListItem, pContents->ulNumOfUiSubItemEvents, nListFocusIndex+i);
			if (pDrawSubItemInfo)
			{
				if (MWC_UTIL_DvbStrlenStrOnly(pDrawSubItemInfo->ReCharReasonStr) > 0)
				{
					// setting text style of the evtname info list
					OSD_GFX_SetRect(&rect_list, s_Mediaset_ReChar_NaviSubItem[0].rect.x, s_Mediaset_ReChar_NaviSubItem[0].rect.y + ((i+1)*NAPPV_MEDIASET_RECHAR_SUBBTN_LINESPACE),
												s_Mediaset_ReChar_NaviSubItem[0].rect.w, s_Mediaset_ReChar_NaviSubItem[0].rect.h);
					OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, rect_list.x, rect_list.y, rect_list.w,
												(HINT8 *)pDrawSubItemInfo->ReCharReasonStr, COL(C_T_MList_Yellow));
				}

				// setting text style of the expirydate info list
				OSD_GFX_SetRect(&rect_list, s_Mediaset_ReChar_NaviSubItem[1].rect.x, s_Mediaset_ReChar_NaviSubItem[1].rect.y + ((i+1)*NAPPV_MEDIASET_RECHAR_SUBBTN_LINESPACE),
											s_Mediaset_ReChar_NaviSubItem[1].rect.w, s_Mediaset_ReChar_NaviSubItem[1].rect.h);
				snprintf(szDate, 128, "%02d/%02d/%02d\n", pDrawSubItemInfo->ReCharDate.day, pDrawSubItemInfo->ReCharDate.month, (pDrawSubItemInfo->ReCharDate.year+1900)%100);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Left, FONT_SIZE_M, rect_list.x, rect_list.y, rect_list.w,
												(HINT8 *)szDate, COL(C_T_Cursor));

				// setting text style of the credit info list
				OSD_GFX_SetRect(&rect_list, s_Mediaset_ReChar_NaviSubItem[2].rect.x, s_Mediaset_ReChar_NaviSubItem[2].rect.y + ((i+1)*NAPPV_MEDIASET_RECHAR_SUBBTN_LINESPACE),
											s_Mediaset_ReChar_NaviSubItem[2].rect.w, s_Mediaset_ReChar_NaviSubItem[2].rect.h);

				/* credit icon */
				if (!AP_StockImg_GetImage (e911_4_24_Euro_Font_White_B, &stImage))
				{
					nX1 = rect_list.x + rect_list.w - stImage.width;
					nY1 = rect_list.y - NAPPV_MEDIASET_RECHAR_STRING_MARGIN_Y;
					OSD_GFX_SetRect (&rect_icon, nX1, nY1, stImage.width, stImage.height);
					OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &rect_icon);
				}
				/* credit */
				nW1 = rect_list.w - stImage.width - 10;
				snprintf(szDate, 128, "%d,%02d", pDrawSubItemInfo->ReCharCredit/100, pDrawSubItemInfo->ReCharCredit%100);
				OSD_GFX_DrawTextByLimit(pScreen, GWM_Obj_GetObjectID(pObject), eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_list.x, rect_list.y, nW1,
												(HINT8 *)szDate, COL(C_T_Cursor));

				// setting text style of the line
				OSD_GFX_SetRect(&rect_img, s_Mediaset_ReChar_NaviSubItemLine[0].rect.x, s_Mediaset_ReChar_NaviSubItemLine[0].rect.y + ((i+1)*NAPPV_MEDIASET_RECHAR_SUBBTN_LINE_LINESPACE),
											s_Mediaset_ReChar_NaviSubItemLine[0].rect.w, s_Mediaset_ReChar_NaviSubItemLine[0].rect.h);
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
 Function		: xmgr_cas_NaPpvRecharDrawBackgroundImage_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDrawBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
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
 Function		: xmgr_cas_Nappv_mset_selprog_DrawStateButton
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDrawStateButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvRecharGetPageStr_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharGetPageStr_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents)
{
	HINT32	nCurPage, nTotalPage;

	nCurPage = pContents->ulCurPage + 1;
	nTotalPage = pContents->ulNumOfUiSubItemEvents / MAX_MSET_RECHAR_LIST_ROW_NUM + 1;
	if ((pContents->ulNumOfUiSubItemEvents % MAX_MSET_RECHAR_LIST_ROW_NUM) == 0)
	{
		nTotalPage--;
	}

	snprintf(pContents->ucPageNum, 16, "%d/%d", nCurPage, nTotalPage);

	return ERR_BUS_NO_ERROR;
}


/********************************************************************
 Function		: xmgr_cas_NaPpvRecharDrawCredit_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDrawCredit_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT8 					szDate[128];
	HINT32					nObjectId;
	HUINT32 				nRemainCredit = 0;
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

	/* na ppv mediaset rechar credit icon */
	if (!AP_StockImg_GetImage (e915_1_24_EUR, &stImage))
	{
		nX = rect_text.x + rect_text.w - stImage.width;
		nY = rect_text.y - NAPPV_MEDIASET_RECHAR_STRING_MARGIN_Y;
		OSD_GFX_SetRect (&rect_icon, nX, nY, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &rect_icon);
	}

#if 0
	/* na ppv mediaset rechar credit  */
	for(i=0; i<pContents->ulNumOfUiSubItemEvents; i++)
	{
		if(pContents->pstUiSubItem[i].ReCharCredit > 0)
		{
			nAllCredit += pContents->pstUiSubItem[i].ReCharCredit;
		}
	}
#endif
	xmgr_cas_NaGetScCredit(&nRemainCredit);
	nW = rect_text.w - stImage.width -10;
	snprintf(szDate, 128, "%d,%02d", nRemainCredit/100, nRemainCredit%100);
	OSD_GFX_DrawTextByLimit(pScreen, nObjectId, eFont_SystemNormal, eFontAlign_Right, FONT_SIZE_M, rect_text.x, rect_text.y, nW,
										(HINT8 *)szDate, COL(C_T_MList_Yellow));

	/* na ppv mediaset rechar credit text*/
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
 Function		: xmgr_cas_NaPpvRecharDrawUpDownArrow_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDrawUpDownArrow_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvRecharDrawBackButton_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDrawBackButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

STATIC BUS_Result_t xmgr_cas_NaPpvRecharDrawNoItemLine_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1)
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
 Function		: xmgr_cas_NaPpvRecharSetUiObjects_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharSetUiObjects_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents)
{
	OSD_Rect_t				rect = {0, };

	/* na ppv mediaset rechar background */
	Settings_DrawBack(FALSE);
	OSD_GFX_SetRect(&rect, 0, 0, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT);
	GWM_Rect_Create(NAPPV_MEDIASET_RECHAR_LOGOBACKGROUND_IMAGE_ID, &rect, 0x00000000);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_RECHAR_LOGOBACKGROUND_IMAGE_ID, xmgr_cas_NaPpvRecharDrawBackgroundImage_Mediaset);

	/* na ppv mediaset rechar date text*/
	OSD_GFX_SetRect (&rect, NAPPV_MEDIASET_RECHAR_DATE_TEXT_X, NAPPV_MEDIASET_RECHAR_DATE_TEXT_Y, NAPPV_MEDIASET_RECHAR_DATE_TEXT_WIDTH, NAPPV_MEDIASET_RECHAR_DATE_TEXT_HEIGHT);
	GWM_Text_Create(NAPPV_MEDIASET_RECHAR_DATE_TEXT_ID, &rect, pContents->ucCurDate);
	GWM_Text_SetAlign (NAPPV_MEDIASET_RECHAR_DATE_TEXT_ID, TEXT_ALIGN_RIGHT);
	GWM_Text_SetSize (NAPPV_MEDIASET_RECHAR_DATE_TEXT_ID, FONT_SIZE_S);
	GWM_Text_ChangeColor (NAPPV_MEDIASET_RECHAR_DATE_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv mediaset selprog credit button */
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_RECHAR_STATE_BTN_X, NAPPV_MEDIASET_RECHAR_STATE_BTN_Y, NAPPV_MEDIASET_RECHAR_STATE_BTN_WIDTH, NAPPV_MEDIASET_RECHAR_STATE_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_RECHAR_STATE_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_RECHAR_STATE_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvRecharDrawStateButton_Mediaset);

	/* na ppv mediaset rechar infotext*/
	OSD_GFX_SetRect (&rect, NAPPV_MEDIASET_RECHAR_INFO_TEXT_X, NAPPV_MEDIASET_RECHAR_INFO_TEXT_Y, NAPPV_MEDIASET_RECHAR_INFO_TEXT_WIDTH, NAPPV_MEDIASET_RECHAR_INFO_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_MEDIASET_RECHAR_INFO_TEXT_ID, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_RECHARGE_HISTORY_TEXT_ID));
	GWM_Text_SetAlign (NAPPV_MEDIASET_RECHAR_INFO_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_MEDIASET_RECHAR_INFO_TEXT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor(NAPPV_MEDIASET_RECHAR_INFO_TEXT_ID, COL (C_T_Cursor), 0x00);

	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_RECHAR_CREDIT_BTN_X, NAPPV_MEDIASET_RECHAR_CREDIT_BTN_Y, NAPPV_MEDIASET_RECHAR_CREDIT_BTN_WIDTH, NAPPV_MEDIASET_RECHAR_CREDIT_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_RECHAR_CREDIT_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_RECHAR_CREDIT_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvRecharDrawCredit_Mediaset);

	/* na ppv mediaset rechar back button */
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_RECHAR_BACK_BTN_X, NAPPV_MEDIASET_RECHAR_BACK_BTN_Y, NAPPV_MEDIASET_RECHAR_BACK_BTN_WIDTH, NAPPV_MEDIASET_RECHAR_BACK_BTN_HEIGHT);
	GWM_Frame_Create(NAPPV_MEDIASET_RECHAR_BACK_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_RECHAR_BACK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvRecharDrawBackButton_Mediaset);

	HxLOG_Print(" --- \n");

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvRecharDestpruUi_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharDestpruUi_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents)
{
	if (pContents->pstRechargeDataListItem)
	{
		xmgr_cas_NaPpvRecharFreeRechargeList_Mediaset(&pContents->pstRechargeDataListItem);
		pContents->pstRechargeDataListItem = NULL;
	}

	return ERR_BUS_NO_ERROR;
}

STATIC void	xmgr_cas_NaPpvRecharLinkFreeListItemMethod_Mediaset(void *pvContents)
{
	if(pvContents == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}

	OxAP_Free(pvContents);
}

STATIC BUS_Result_t	xmgr_cas_NaPpvRecharFreeRechargeList_Mediaset(POINTER_LIST_T **pRoot)
{
	*pRoot = UTIL_LINKEDLIST_FreeWholeListItemsFromPointerList(*pRoot, xmgr_cas_NaPpvRecharLinkFreeListItemMethod_Mediaset);

	if(*pRoot != NULL)
	{
		HxLOG_Critical("\n\n");
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvRecharUpdateRechargeList_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharUpdateRechargeList_Mediaset(HUINT32 xStatus, HUINT32 xNumberOfItems, const RECHARGE_LIST_ITEM* pxRechargeList, XmgrCas_NaReChar_Mediaset_t *pContents)
{
	HUINT16					index = 0;
	XmgrCas_NaReCharUiSubItem_t		*ptItem;
	OSD_Rect_t				rect;

	if (pContents->bWaiting != TRUE)
	{
		return MESSAGE_BREAK;
	}

	pContents->bWaiting = FALSE;

	if (pContents->pstRechargeDataListItem)
	{
		xmgr_cas_NaPpvRecharFreeRechargeList_Mediaset(&pContents->pstRechargeDataListItem);
		pContents->pstRechargeDataListItem = NULL;
		pContents->ulNumOfUiSubItemEvents = 0;
		pContents->ulCurPage = 0;
	}

	if (xStatus == CA_EVENT_SUCCESS && xNumberOfItems > 0 && pxRechargeList != NULL)
	{
		pContents->ulNumOfUiSubItemEvents = (HUINT16)xNumberOfItems;

		for (index = 0; index < xNumberOfItems; index ++)
		{
			ptItem = (XmgrCas_NaReCharUiSubItem_t *)OxAP_Malloc(sizeof(XmgrCas_NaReCharUiSubItem_t));
			ptItem->ReCharCredit = pxRechargeList[index].Amount;
			ptItem->ReCharDate = pxRechargeList[index].Date;
			switch(pxRechargeList[index].Reason)
			{
				case CA_RECHARGE_REASON_USER_REQUEST:
					ptItem->ReCharReasonStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_TYPE_USER_REQUEST_ID);
					break;
				case CA_RECHARGE_REASON_PROMOTION:
					ptItem->ReCharReasonStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_TYPE_PROMOTIONAL_ID);
					break;
				case CA_RECHARGE_REASON_REFUND:
					ptItem->ReCharReasonStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_TYPE_REFUND_ID);
					break;
				default:		// 이상한 값이면 그냥 Recharge로 표시하자
					ptItem->ReCharReasonStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_TYPE_USER_REQUEST_ID);
					break;
			}

			pContents->pstRechargeDataListItem = UTIL_LINKEDLIST_AppendListItemToPointerList(pContents->pstRechargeDataListItem, ptItem);
		}

		/* recharge List Item을 생성한다. */
		OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_RECHAR_SUBBTN_X, NAPPV_MEDIASET_RECHAR_SUBBTN_Y, NAPPV_MEDIASET_RECHAR_SUBBTN_WIDTH, (NAPPV_MEDIASET_RECHAR_SUBBTN_HEIGHT * MAX_MSET_RECHAR_LIST_ROW_NUM) + 16);
		GWM_List_Create(NAPPV_MEDIASET_RECHAR_SUBBTN_ID, &rect, NULL, NAPPV_MEDIASET_RECHAR_SUBBTN_HEIGHT, pContents->ulNumOfUiSubItemEvents, /* page count */ MAX_MSET_RECHAR_LIST_ROW_NUM, ((GWM_LIST_ITEMS)pContents->pstRechargeDataListItem));
		GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_RECHAR_SUBBTN_ID, xmgr_cas_NaPpvRecharDrawRechargeListItem_Mediaset);

		if (pContents->ulNumOfUiSubItemEvents)
		{
			/* na ppv mediaset selprog arrow */
			OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_RECHAR_ARROW_BTN_X, NAPPV_MEDIASET_RECHAR_ARROW_BTN_Y, NAPPV_MEDIASET_RECHAR_ARROW_BTN_WIDTH, NAPPV_MEDIASET_RECHAR_ARROW_BTN_HEIGHT);
			GWM_Frame_Create(NAPPV_MEDIASET_RECHAR_ARROW_BTN_ID, &rect, NULL);
			GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_RECHAR_ARROW_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvRecharDrawUpDownArrow_Mediaset);

			/* na ppv mediaset selprog list page button */
			xmgr_cas_NaPpvRecharGetPageStr_Mediaset(pContents);
			OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_X, NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_Y-FONT_SIZE_XS, NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_WIDTH, NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_HEIGHT);
			GWM_Text_Create(NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_ID, &rect, pContents->ucPageNum);
			GWM_Text_SetAlign (NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_ID, TEXT_ALIGN_RIGHT);
			GWM_Text_SetSize (NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_ID, FONT_SIZE_XS);
			GWM_Text_ChangeColor(NAPPV_MEDIASET_RECHAR_LISTPAGE_TEXT_ID, COL (C_T_Cursor), 0x00);
		}
		else
		{
			// no item string display
		}

#ifdef WIN32
		OxAP_Free(pxRechargeList);
#endif

		GWM_Obj_SetFocus(NAPPV_MEDIASET_RECHAR_SUBBTN_ID);
	}
	else
	{
		// no item string display
		OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_RECHAR_SUBBTN01_X, NAPPV_MEDIASET_RECHAR_SUBBTN01_Y + NAPPV_MEDIASET_RECHAR_SUBBTN01_HEIGHT - FONT_SIZE_M, NAPPV_MEDIASET_RECHAR_SUBBTN01_WIDTH, NAPPV_MEDIASET_RECHAR_SUBBTN01_HEIGHT);
		GWM_Text_Create(NAPPV_MEDIASET_RECHAR_NO_ITEM_TEXT, &rect, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_RECAHR_NODATA_TEXT_ID));
		GWM_Text_SetAlign (NAPPV_MEDIASET_RECHAR_NO_ITEM_TEXT, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (NAPPV_MEDIASET_RECHAR_NO_ITEM_TEXT, FONT_SIZE_M);
		GWM_Text_ChangeColor(NAPPV_MEDIASET_RECHAR_NO_ITEM_TEXT, COL (C_T_MList_Yellow), 0x00);

		// under line
		OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_RECHAR_SUBBTN01_LINE_X, NAPPV_MEDIASET_RECHAR_SUBBTN01_LINE_Y + NAPPV_MEDIASET_RECHAR_SUBBTN_LINE_LINESPACE, NAPPV_MEDIASET_RECHAR_SUBBTN01_LINE_WIDTH, NAPPV_MEDIASET_RECHAR_SUBBTN01_LINE_HEIGHT);
		GWM_Frame_Create(NAPPV_MEDIASET_RECHAR_NO_ITEM_LINE, &rect, NULL);
		GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_RECHAR_NO_ITEM_LINE, (GWM_ObjDraw_t)xmgr_cas_NaPpvRecharDrawNoItemLine_Mediaset);
	}
}

/********************************************************************
 Function		: xmgr_cas_NaPpvRecharUpdateRemainedCredit_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvRecharUpdateRemainedCredit_Mediaset(XmgrCas_NaReChar_Mediaset_t *pContents, TCreditAmount nCredit)
{
	GWM_APP_InvalidateObject(NAPPV_MEDIASET_RECHAR_CREDIT_BTN_ID);

	return ERR_BUS_NO_ERROR;
}

#define	______
#define	_________________________PUBLIC_BODY______________________________
#define	______
/******************************************************************************/
/**
*   \fn     xproc_cas_NaRechargeHistory_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaRechargeHistory_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaReChar_Mediaset_t	*pContents = xmgr_cas_NaPpvRecharGetLocalContents_Mediaset();
	HUINT32					nResult = ERR_FAIL;

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_MEDIASET_RECHAR_WINDOW_X, NAPPV_MEDIASET_RECHAR_WINDOW_Y,
								NAPPV_MEDIASET_RECHAR_WINDOW_WIDTH, NAPPV_MEDIASET_RECHAR_WINDOW_HEIGHT);

			xmgr_cas_NaPpvRecharInitUiConfiguration_Mediaset(pContents);
			xmgr_cas_NaPpvRecharSetMainTitle_Mediaset(pContents);

			// update selprog information
			xmgr_cas_NaPpvRecharRequestRechargeList_Mediaset(pContents);

			// draw UI object
			xmgr_cas_NaPpvRecharSetUiObjects_Mediaset(pContents);

			// set front panel
			//AP_SPEC_DisplayMenuPanelString(pContents->ucTitle, eDxPANEL_ALIGN_CENTER);
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
					BUS_MGR_Create("xmgr_cas_NaMSET_MenuSmartCardError_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xmgr_cas_NaMSET_MenuSmartCardError_Proc, 0, eAlarmType, 0, 0);

					return MESSAGE_PASS;
				}
				else
				{
					// Card 정상 삽입 상태인데.. 이쪽으로 왜 올까?
				}
			}
			break;

		case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_RECHARGE_LIST:
			if(p1 == eDxCAS_GROUPID_NA)
			{
				xmgr_cas_NaPpvRecharUpdateRechargeList_Mediaset((HUINT32)hAction, (HUINT32)p2, (RECHARGE_LIST_ITEM*)p3, pContents);
			}
			return MESSAGE_BREAK;

		case eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT:
			HxLOG_Print("eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT...\n");
			if(p1 == eDxCAS_GROUPID_NA)
			{
				xmgr_cas_NaPpvRecharUpdateRemainedCredit_Mediaset(pContents, (TCreditAmount)p3);
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
					xmgr_cas_NaPpvRecharScrollPage_Mediaset(pContents, p1);
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
			xmgr_cas_NaPpvRecharDestpruUi_Mediaset(pContents);

			/* clear front PANEL */
			//AP_SPEC_ClearMenuPanelString();
			break;

		default:
			break;
	}

	HxLOG_Info("xproc_cas_NaRechargeHistory_Mediaset handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif	//end of #if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
/* End of File */
