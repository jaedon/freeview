/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <ray_popupui_coordinate.h>

#include <na_mgr_cak_ext.h>
#include <na_mgr_cak_ppv.h>
#include <settings.h>
#include <osd_gfx.h>
#include <ap_epg_main.h>
#include <svc_epg.h>

#include <namgr_main.h>

#include "../../local_include/_xmgr_cas_na_util.h"
#include "../../local_include/_xmgr_cas_na_adapt.h"
#include "../../local_include/_xmgr_cas_na.h"


#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define XMGR_CAS_NAPPV_PANGEA_PARAM_WARRING_REMOVE(arg1, arg2, arg3, arg4)				\
			HxLOG_Info("\n");			\
			{															\
			(void)arg1;													\
			(void)arg2;													\
			(void)arg3;													\
			(void)arg4;					 								\
			}
#define XMGR_CAS_NAPPV_PANGEA_POINTER_INVALID_CHECK(pArg, returnValue)					\
			HxLOG_Info("\n");			\
			if(pArg == NULL)													\
			{																	\
			HxLOG_Critical("\n\n");												\
			return returnValue;													\
			}
#define XMGR_CAS_NAPPV_PANGEA_HERROR_INVALID_CHECK(pArg, returnValue)					\
			HxLOG_Info("\n");			\
			if(pArg != ERR_BUS_NO_ERROR)									\
			{															\
			HxLOG_Critical("\n\n");												\
			return returnValue;											\
			}


#define	CAS_NA_PPV_PANGEA_APP_NAME							"CasNaPPVPangea"

#define CAS_NA_PPV_PANGEA_WINDOW_WIDTH						870
#define CAS_NA_PPV_PANGEA_WINDOW_HEIGHT						245
#define CAS_NA_PPV_PANGEA_WINDOW_X							205
#define CAS_NA_PPV_PANGEA_WINDOW_Y							406
#define CAS_NA_PPV_PANGEA_COLOR_YELLOW						C_T_MList_Yellow
#define CAS_NA_PPV_PANGEA_COLOR_WHITE						C_T_Cursor
#define CAS_NA_PPV_PANGEA_COLOR_GRAY						C_T_Editbox
#define CAS_NA_PPV_PANGEA_COLOR_BLACK						C_W_BG01
#define CAS_NA_PPV_PANGEA_COLOR_GREEN						C_T_PList_Green
#define CAS_NA_PPV_PANGEA_COLOR_DISABLE						C_T_Wzd_Disable

#define CAS_NA_PPV_PANGEA_FRAME_ID							0x0A00

#define CAS_NA_PPV_PANGEA_FRAME_WIDTH						CAS_NA_PPV_PANGEA_WINDOW_WIDTH
#define CAS_NA_PPV_PANGEA_FRAME_HEIGHT						CAS_NA_PPV_PANGEA_WINDOW_HEIGHT
#define CAS_NA_PPV_PANGEA_FRAME_X							CAS_NA_PPV_PANGEA_WINDOW_X
#define CAS_NA_PPV_PANGEA_FRAME_Y							CAS_NA_PPV_PANGEA_WINDOW_Y

#define CAS_NA_PPV_PANGEA_EVENT_NAME_STR_ID					(CAS_NA_PPV_PANGEA_FRAME_ID + 0x01)
#define CAS_NA_PPV_PANGEA_EVENT_NAME_STR_X					245
#define CAS_NA_PPV_PANGEA_EVENT_NAME_STR_Y					441
#define CAS_NA_PPV_PANGEA_EVENT_NAME_STR_W					700
#define CAS_NA_PPV_PANGEA_EVENT_NAME_STR_H					(FONT_SIZE_M+4)

#define CAS_NA_PPV_PANGEA_START_TIME_ID						(CAS_NA_PPV_PANGEA_FRAME_ID + 0x02)
#define CAS_NA_PPV_PANGEA_START_TIME_X						649
#define CAS_NA_PPV_PANGEA_START_TIME_Y						485
#define CAS_NA_PPV_PANGEA_START_TIME_W						350
#define CAS_NA_PPV_PANGEA_START_TIME_H						(FONT_SIZE_XS+4)

#define CAS_NA_PPV_PANGEA_START_TIME_STR_ID					(CAS_NA_PPV_PANGEA_FRAME_ID + 0x03)

#define CAS_NA_PPV_PANGEA_PRICE_ID							(CAS_NA_PPV_PANGEA_FRAME_ID + 0x04)
#define CAS_NA_PPV_PANGEA_PRICE_X							245
#define CAS_NA_PPV_PANGEA_PRICE_Y							485
#define CAS_NA_PPV_PANGEA_PRICE_W							350
#define CAS_NA_PPV_PANGEA_PRICE_H							(FONT_SIZE_XS+4)

#define CAS_NA_PPV_PANGEA_PRICE_STR_ID						(CAS_NA_PPV_PANGEA_FRAME_ID + 0x05)
#define CAS_NA_PPV_PANGEA_SLICETIME_STR_ID					(CAS_NA_PPV_PANGEA_FRAME_ID + 0x06)

#define CAS_NA_PPV_PANGEA_COLOR_BTN_ID						(CAS_NA_PPV_PANGEA_FRAME_ID + 0x07)
#define CAS_NA_PPV_PANGEA_COLOR_BTN_X						649
#define CAS_NA_PPV_PANGEA_COLOR_BTN_Y						495
#define CAS_NA_PPV_PANGEA_COLOR_BTN_W						45
#define CAS_NA_PPV_PANGEA_COLOR_BTN_H						31

#define CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_ID					(CAS_NA_PPV_PANGEA_FRAME_ID + 0x08)
#define CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_X					704
#define CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_Y					519
#define CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_W					300
#define CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_H					(FONT_SIZE_XS+4)

#define CAS_NA_PPV_PANGEA_REMAIN_CREDIT_ID					(CAS_NA_PPV_PANGEA_FRAME_ID + 0x09)
#define CAS_NA_PPV_PANGEA_REMAIN_CREDIT_X					245
#define CAS_NA_PPV_PANGEA_REMAIN_CREDIT_Y					519
#define CAS_NA_PPV_PANGEA_REMAIN_CREDIT_W					350
#define CAS_NA_PPV_PANGEA_REMAIN_CREDIT_H					(FONT_SIZE_XS+4)

#define CAS_NA_PPV_PANGEA_REMAIN_CREDIT_STR_ID				(CAS_NA_PPV_PANGEA_FRAME_ID + 0x10)

#define CAS_NA_PPV_PANGEA_OK_BTN_ID							(CAS_NA_PPV_PANGEA_FRAME_ID + 0x99)
#define CAS_NA_PPV_PANGEA_OK_BTN_X							245
#define CAS_NA_PPV_PANGEA_OK_BTN_Y							597
#define CAS_NA_PPV_PANGEA_OK_BTN_W							86
#define CAS_NA_PPV_PANGEA_OK_BTN_H							44

#define CAS_NA_PPV_PANGEA_OK_BTN_HELP_ID					(CAS_NA_PPV_PANGEA_FRAME_ID + 0x12)
#define CAS_NA_PPV_PANGEA_OK_BTN_HELP_X						338
#define CAS_NA_PPV_PANGEA_OK_BTN_HELP_Y						626
#define CAS_NA_PPV_PANGEA_OK_BTN_HELP_W						250
#define CAS_NA_PPV_PANGEA_OK_BTN_HELP_H						(FONT_SIZE_XS+4)

#define CAS_NA_PPV_PANGEA_BACK_BTN_ID						(CAS_NA_PPV_PANGEA_FRAME_ID + 0x13)
#define CAS_NA_PPV_PANGEA_BACK_BTN_X						649
#define CAS_NA_PPV_PANGEA_BACK_BTN_Y						597
#define CAS_NA_PPV_PANGEA_BACK_BTN_W						86
#define CAS_NA_PPV_PANGEA_BACK_BTN_H						44

#define CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID					(CAS_NA_PPV_PANGEA_FRAME_ID + 0x14)
#define CAS_NA_PPV_PANGEA_BACK_BTN_HELP_X					742
#define CAS_NA_PPV_PANGEA_BACK_BTN_HELP_Y					626
#define CAS_NA_PPV_PANGEA_BACK_BTN_HELP_W					250
#define CAS_NA_PPV_PANGEA_BACK_BTN_HELP_H					(FONT_SIZE_XS+4)

#define CAS_NA_PPV_PANGEA_SELECTED_BACK_ID					(CAS_NA_PPV_PANGEA_FRAME_ID + 0x15)
#define CAS_NA_PPV_PANGEA_SELECTED_BACK_X					219
#define CAS_NA_PPV_PANGEA_SELECTED_BACK_Y					368
#define CAS_NA_PPV_PANGEA_SELECTED_BACK_W					842
#define CAS_NA_PPV_PANGEA_SELECTED_BACK_H					40

#define CAS_NA_PPV_PANGEA_EVENT_SELECTED_ID					(CAS_NA_PPV_PANGEA_FRAME_ID + 0x16)
#define CAS_NA_PPV_PANGEA_EVENT_SELECTED_X					245
#define CAS_NA_PPV_PANGEA_EVENT_SELECTED_Y					399
#define CAS_NA_PPV_PANGEA_EVENT_SELECTED_W					760
#define CAS_NA_PPV_PANGEA_EVENT_SELECTED_H					(FONT_SIZE_XS+4)

#define CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID				(CAS_NA_PPV_PANGEA_FRAME_ID + 0x17)
#define CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_X					245
#define CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_Y					570
#define CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_W					760
#define CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_H					(FONT_SIZE_XS+4)

#define CAS_NA_PPV_PANGEA_ERR_MESSAGE_STR_ID				(CAS_NA_PPV_PANGEA_FRAME_ID + 0x18)
#define CAS_NA_PPV_PANGEA_ERR_MESSAGE_STR_X					141
#define CAS_NA_PPV_PANGEA_ERR_MESSAGE_STR_Y					545
#define CAS_NA_PPV_PANGEA_ERR_MESSAGE_STR_W					700
#define CAS_NA_PPV_PANGEA_ERR_MESSAGE_STR_H					(FONT_SIZE_XS+4)

#define CAS_NA_PPV_PANGEA_EXTEND_BACK_ID					(CAS_NA_PPV_PANGEA_FRAME_ID + 0x19)
#define CAS_NA_PPV_PANGEA_EXTEND_BACK_X						216
#define CAS_NA_PPV_PANGEA_EXTEND_BACK_Y						112
#define CAS_NA_PPV_PANGEA_EXTEND_BACK_W						850
#define CAS_NA_PPV_PANGEA_EXTEND_BACK_H						292

#define CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_ID				(CAS_NA_PPV_PANGEA_FRAME_ID + 0x20)
#define CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_X				245
#define CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_Y				148
#define CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_W				760
#define CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_H				(FONT_SIZE_L+4)

#define CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID				(CAS_NA_PPV_PANGEA_FRAME_ID + 0x21)
#define CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_X					245
#define CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_Y					198
#define CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_W					736
#define CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_H					210 //263 ??

#define CAS_NA_PPV_PANGEA_EXTEND_INFO_SCROLL_UP_X			1005
#define CAS_NA_PPV_PANGEA_EXTEND_INFO_SCROLL_UP_Y			179
#define CAS_NA_PPV_PANGEA_EXTEND_INFO_SCROLL_DOWN_X			1005
#define CAS_NA_PPV_PANGEA_EXTEND_INFO_SCROLL_DOWN_Y			343

#define CAS_NA_PPV_PANGEA_EURO_ICON_MARGIN_Y				17
#define CAS_NA_PPV_PANGEA_PRICE_EURO_ICON_ID				(CAS_NA_PPV_PANGEA_FRAME_ID + 0x22)
#define CAS_NA_PPV_PANGEA_CREDIT_EURO_ICON_ID				(CAS_NA_PPV_PANGEA_FRAME_ID + 0x23)

#define MAX_LANGCODE_LEN				3
#define CHAR_VALUE_LINEFEED				0x0a
#define CHAR_VALUE_CARRIAGERETURN		0x0d

#define	CAS_NA_PPV_PANGEA_PPV_CHECK_TIMER_ID					(0x11050148)
#define	CAS_NA_PPV_PANGEA_PPV_CHECK_PERIOD					(1000)	/* 1sec */


typedef enum
{
	eNA_PPV_EXT_MODE_NONE = 0,
	eNA_PPV_EXT_MODE_PPV_INFO,
	eNA_PPV_EXT_MODE_PPV_PURCHASE_INFO,
} XmgrCas_NaPppvExtMode_Pangea_t;

typedef enum
{
	eNA_PPV_COLOR_ICON_GREEN = 0,
	eNA_PPV_COLOR_ICON_BLUE,
	eNA_PPV_COLOR_ICON_DONE
} XmgrCas_NaPpvColorBtnType_Pangea_e;

typedef struct CasNaPPVPangea_Contents
{
//	HWND							hExtWnd;
	XmgrCas_NaPppvExtMode_Pangea_t 					nExtMode;
	XmgrCas_NaPpvColorBtnType_Pangea_e		nExtButtonType;

	HUINT8							*pszSelectStr;
	HUINT8							*pszEvtTimeStr;			// Started at or ElapedTime
	HUINT8							*pszEvtPriceStr;
	HUINT8							*pszRemainCreditStr;
	HUINT8							*pszErrHelpStr;			// Smartcard Error, Not Enough Credit Error, Purchase preiod over etc.
	HUINT8							*pszConfirmStr;
	HUINT8							*pszBtnOKHelpStr;
	HUINT8							*pszBtnBackHelpStr;
	HUINT8							*pszBtnColorHelpStr;

	TPurchasableEventType			nPPVType;				// IPPV or PPT
	HINT8							szEventName[64];
	HINT8							szStartTime[16];			// StartTime of Purchasable event
//	UNIXTIME						utEndTime;				// EndTime of Purchasble event
	HINT8							szEventPrice[16];		// slice Cost 와 공용하자
	HINT8							szRemainCredit[16];
	HINT8							szSliceCost[16];			// Cost per slice	only PPT use
	HINT8							szSliceTime[16];			// Minute			only PPT use
	HINT8							szExtendTitle[64];

	UNIXTIME						nPurchaseEndTime;
	UNIXTIME						nElapsedTime;
	HUINT32							nCardCredit;
	HUINT32							nEventCredit;
	HBOOL							bNotPurchasable;
	TCaRequest						*pCaRequest;
	Handle_t						hSvcHandle;
} XmgrCas_NaPpvContents_Pangea_t;

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC XmgrCas_NaPpvContents_Pangea_t s_CasNaPPVPangea_Contents;


/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
STATIC HERROR xmgr_cas_NaPpvBannerMakeStrings(XmgrCas_NaPpvContents_Pangea_t *pstContents);
STATIC XmgrCas_NaPpvContents_Pangea_t* xmgr_cas_NaPpvBannerGetContents_Pangea(void);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerInitContents_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents, HINT32 p1, HINT32 p2);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawDefaultUi_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawColorButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawEuroIconButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawOKButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawBackButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawSelectedBackground_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawExtendedBackground_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerAddLongStringWithWordwrap_Pangea(HUINT32 ulFmtObjID, OSD_Font_t *pFont, HUINT32 ulFmtObjWidth,HUINT8 *pszString, HUINT32 ulStrLen, Text_Property_t *pstTextProperty);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerExtendInfoDrawScroll_Pangea(int ID, OSD_Screen_t *pScreen, FMT_DrawItem_t *pDrawItem);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerExtendInfoDrawScroll_Pangea(	OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, FMT_OBJECT pObject);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerMakeExtendedInfo_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerGetStringWidth_Pangea(HUINT8* string, HUINT32 ulFontSize, HUINT32 *ulWidth);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerUpdateSubItemsByColorBtnType_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerRemoveExtendWindow_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerUpdatePpvWindow_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents);
STATIC HERROR		xmgr_cas_NaPpvBannerMsgCreated_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC HERROR		xmgr_cas_NaPpvBannerMsgDestroy_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerMsgKeyDown_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
STATIC BUS_Result_t xmgr_cas_NaPpvBannerMsgTimer_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents, HINT32 nTimerId);


#define _____XMGR_CAS_NA_UI_PPV_PGEA_LOCAL_FUNCTION______________________________________________________

STATIC HERROR xmgr_cas_NaPpvBannerMakeStrings(XmgrCas_NaPpvContents_Pangea_t *pstContents)
{
	// 1. Selected event
	pstContents->pszSelectStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_SELECTED_PROGRAM_ID);

	// 2. Start time
	if (pstContents->nPPVType == CA_EVENT_SPECIAL_EVENT)
	{
		pstContents->pszEvtTimeStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_START_TIME_ID);
	}
	else		// PPT
	{
		pstContents->pszEvtTimeStr = NULL;
	}

	// 3. Price
	pstContents->pszEvtPriceStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_PRICE_ID);

	// 4. Remain credit
	pstContents->pszRemainCreditStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_RESIDUAL_CREDIT_ID);

	// 5. Err message
	pstContents->pszErrHelpStr = NULL;

	// 6. Confirm message
	pstContents->pszConfirmStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_PURCHASE_CONFIRM_ID);

	// 7. OK button
	pstContents->pszBtnOKHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_IPPV_NAME_SELECT_EVENT_ID);	// 검토

	// 8. Back button
	pstContents->pszBtnBackHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_CANCEL_ID);

	// 9. Color button help
	pstContents->pszBtnColorHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_SHOW_INFORMATION_ID);

	return ERR_BUS_NO_ERROR;
}

STATIC XmgrCas_NaPpvContents_Pangea_t*	xmgr_cas_NaPpvBannerGetContents_Pangea(void)
{
	return &s_CasNaPPVPangea_Contents;
}

STATIC BUS_Result_t xmgr_cas_NaPpvBannerInitContents_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents, HINT32 p1, HINT32 p2)
{
	BUS_Result_t		eResult = ERR_BUS_NO_ERROR;
	HERROR				hErr = ERR_OK;
	PPV_EVENT_INFO	 	stNaPPVInfo;
	HxDATETIME_Time_t			stTime;
	UNIXTIME			nCurrTime = 0;
	HUINT32				nRemainCredit;

	XMGR_CAS_NAPPV_PANGEA_POINTER_INVALID_CHECK(pstContents, ERR_FAIL);

	memset(pstContents, 0, sizeof(XmgrCas_NaPpvContents_Pangea_t));

	hErr = VK_CLOCK_GetTime(&nCurrTime);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	//hErr = PPV_EVENT_TYPE(NULL, &stNaPPVInfo); // stNaPPVInfo를 인자로 받아야 함
	// 일단은 임시로 구현
	///////////////////////////////////////////////////////////////////////
#ifdef WIN32 // 향후 CAK porting이 끝나면 WIN32로 막을 예정
	if (p2 == 0)	// IPPV
	{
		stNaPPVInfo.xEventID = 100;
		stNaPPVInfo.xPurchaseType = CA_EVENT_SPECIAL_EVENT;		// PPV or PPT
		stNaPPVInfo.xEventType = CA_EVENT_TYPE_SCRAMBLED_IPPV; 		// PPV or IPPV
		stNaPPVInfo.xEventStartTime = 0;	// start time in EIT
		stNaPPVInfo.duration = 2*3600;			// duration in EIT
		stNaPPVInfo.xCost = 2400;				// cost
		stNaPPVInfo.xPurchaseStartTime = 0; // EIT(same as xEventStartTime), PDT(real purchase start time)
		stNaPPVInfo.xPurchaseEndTime = 2*3600;	// EIT(same as xEventStartTime + duration), PDT(real purchase end time)
		stNaPPVInfo.xPreviewTime = 120;		// Preview time
		stNaPPVInfo.xSliceTime = 0; 		// PPT (slice time)
		sprintf(stNaPPVInfo.xEvtName, "FIORENTINA - SAMPDORIA"); // Event Name(EIT, PDT)
		stNaPPVInfo.xProductId = 0x1111;
	}
	else	// PPT
	{
		stNaPPVInfo.xEventID = 100;
		stNaPPVInfo.xPurchaseType = CA_EVENT_PPT;				// PPV or PPT
		stNaPPVInfo.xEventType = CA_EVENT_TYPE_SCRAMBLED_PPV; 		// PPV or IPPV
		stNaPPVInfo.xEventStartTime = 0;	// start time in EIT
		stNaPPVInfo.duration = 2*3600;			// duration in EIT
		stNaPPVInfo.xCost = 250;				// cost
		stNaPPVInfo.xPurchaseStartTime = 0; // EIT(same as xEventStartTime), PDT(real purchase start time)
		stNaPPVInfo.xPurchaseEndTime = 0;	// EIT(same as xEventStartTime + duration), PDT(real purchase end time)
		stNaPPVInfo.xPreviewTime = 120;		// Preview time
		stNaPPVInfo.xSliceTime = 2; 		// PPT (slice time)
		sprintf(stNaPPVInfo.xEvtName, "FIORENTINA - PPT"); // Event Name(EIT, PDT)
		stNaPPVInfo.xProductId = 0x1111;
	}
	///////////////////////////////////////////////////////////////////////
#else
	HxSTD_memcpy(&stNaPPVInfo, p2, sizeof(PPV_EVENT_INFO));
#endif

	// Sub Items Init
	eResult = AP_Contents_InitSubitemArray ((void *)pstContents);
	XMGR_CAS_NAPPV_PANGEA_HERROR_INVALID_CHECK(eResult, eResult);

//	pstContents->hExtWnd = NULL;
	pstContents->nExtMode = eNA_PPV_EXT_MODE_NONE;
	pstContents->nExtButtonType = eNA_PPV_COLOR_ICON_GREEN;
	pstContents->nPPVType = stNaPPVInfo.xPurchaseType;

	// event name
	memset(pstContents->szEventName, 0, 64);
	MWC_UTIL_DvbStrncpy(pstContents->szEventName, stNaPPVInfo.xEvtName, 64-1);

	// start time
	if (pstContents->nPPVType == CA_EVENT_SPECIAL_EVENT)
	{
		HLIB_DATETIME_ConvertUnixTimeToTime(stNaPPVInfo.xEventStartTime, &stTime);
		snprintf(pstContents->szStartTime, 16, "%02d:%02d", stTime.ucHour, stTime.ucMinute);

		// elapsed timer
		HxLOG_Print("nCurrTime = %d, stNaPPVInfo.xEventStartTime = %d\n", nCurrTime, stNaPPVInfo.xEventStartTime);
		pstContents->nElapsedTime = stNaPPVInfo.xEventStartTime;
		if (stNaPPVInfo.xEventStartTime < nCurrTime)
		{
			UNIXTIME nElapsedTime = 0;

			nElapsedTime = nCurrTime - pstContents->nElapsedTime;
			HxLOG_Print("nElapsedTime = %d\n", nElapsedTime);
			if ( nElapsedTime >= 3600 )
			{
				snprintf((char *)pstContents->szStartTime, 16, "%dh %dmin", nElapsedTime / 3600, (nElapsedTime / 60) % 60);
			}
			else
			{
				snprintf((char *)pstContents->szStartTime, 16, "%d min", nElapsedTime / 60);
			}

			pstContents->pszEvtTimeStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_ELAPSED_TIME_ID);
		}

		// event price
		snprintf(pstContents->szEventPrice, 16, "%d,%02d", stNaPPVInfo.xCost/100, stNaPPVInfo.xCost%100);
	}
	else		// PPT
	{
		// event price
		snprintf(pstContents->szSliceCost, 16, "%d,%02d", stNaPPVInfo.xCost/100, stNaPPVInfo.xCost%100);
		if (stNaPPVInfo.xSliceTime % 60 == 0)
		{
			snprintf(pstContents->szSliceTime, 16, "/ %d min", stNaPPVInfo.xSliceTime / 60);
		}
		else if (stNaPPVInfo.xSliceTime / 60 == 0)
		{
			snprintf(pstContents->szSliceTime, 16, "/ %d sec", stNaPPVInfo.xSliceTime);
		}
		else
		{
			snprintf(pstContents->szSliceTime, 16, "/ %d min %d sec", stNaPPVInfo.xSliceTime / 60, stNaPPVInfo.xSliceTime % 60);
		}
	}

	// remained credit
	xmgr_cas_NaGetScCreditStr(pstContents->szRemainCredit);
	xmgr_cas_NaGetScCredit(&nRemainCredit);

	HxLOG_Print("stNaPPVInfo.xCost = %d, nRemainCredit = %d\n", stNaPPVInfo.xCost, nRemainCredit);

	pstContents->nCardCredit = nRemainCredit;
	pstContents->nEventCredit = stNaPPVInfo.xCost;

	xmgr_cas_NaPpvBannerMakeStrings(pstContents);

	// purchase disable timer
	HxLOG_Print("nCurrTime = %d, stNaPPVInfo.xPurchaseEndTime = %d\n", nCurrTime, stNaPPVInfo.xPurchaseEndTime);
	pstContents->nPurchaseEndTime = stNaPPVInfo.xPurchaseEndTime;

	pstContents->pCaRequest = stNaPPVInfo.pCaRequest;
	pstContents->hSvcHandle = (Handle_t)p1;

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t	xmgr_cas_NaPpvBannerDrawDefaultUi_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents)
{
	OSD_Rect_t	 			stRect, imgRect;
	SettingsLeafSubItem_t 	*pstSubItem = NULL;
	HUINT32					prevStringWidth;

	XMGR_CAS_NAPPV_PANGEA_POINTER_INVALID_CHECK(pstContents, ERR_FAIL);

	/* na ppv pangea ippv/ppt banner background */
	OSD_GFX_SetRect(&stRect, CAS_NA_PPV_PANGEA_FRAME_X, CAS_NA_PPV_PANGEA_FRAME_Y, CAS_NA_PPV_PANGEA_FRAME_WIDTH, CAS_NA_PPV_PANGEA_FRAME_HEIGHT);
	GWM_Frame_Create(CAS_NA_PPV_PANGEA_FRAME_ID, &stRect, NULL);
	GWM_APP_SetObjectDrawMethod(CAS_NA_PPV_PANGEA_FRAME_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBannerDrawBackgroundImage_Pangea);

	// Draw Event Name
	OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_EVENT_NAME_STR_X, CAS_NA_PPV_PANGEA_EVENT_NAME_STR_Y - FONT_SIZE_M, CAS_NA_PPV_PANGEA_EVENT_NAME_STR_W, CAS_NA_PPV_PANGEA_EVENT_NAME_STR_H);
	GWM_Text_Create (CAS_NA_PPV_PANGEA_EVENT_NAME_STR_ID, &stRect, pstContents->szEventName);
	GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_EVENT_NAME_STR_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (CAS_NA_PPV_PANGEA_EVENT_NAME_STR_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_EVENT_NAME_STR_ID, COL(CAS_NA_PPV_PANGEA_COLOR_WHITE), 0x00);

	if (pstContents->nPPVType == CA_EVENT_SPECIAL_EVENT)
	{
		// Draw Start time
		OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_START_TIME_X, CAS_NA_PPV_PANGEA_START_TIME_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_START_TIME_W, CAS_NA_PPV_PANGEA_START_TIME_H);
		GWM_Text_Create (CAS_NA_PPV_PANGEA_START_TIME_ID, &stRect, pstContents->pszEvtTimeStr);
		GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_START_TIME_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (CAS_NA_PPV_PANGEA_START_TIME_ID, FONT_SIZE_XS);
		GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_START_TIME_ID, COL(CAS_NA_PPV_PANGEA_COLOR_BLACK), 0x00);

		// Draw Start time value
		xmgr_cas_NaPpvBannerGetStringWidth_Pangea(pstContents->pszEvtTimeStr, FONT_SIZE_XS, &prevStringWidth);
		OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_START_TIME_X+prevStringWidth+10, CAS_NA_PPV_PANGEA_START_TIME_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_START_TIME_W, CAS_NA_PPV_PANGEA_START_TIME_H);
		GWM_Text_Create (CAS_NA_PPV_PANGEA_START_TIME_STR_ID, &stRect, pstContents->szStartTime);
		GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_START_TIME_STR_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (CAS_NA_PPV_PANGEA_START_TIME_STR_ID, FONT_SIZE_XS);
		GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_START_TIME_STR_ID, COL(CAS_NA_PPV_PANGEA_COLOR_GREEN), 0x00);

		// Draw Event price
		OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_PRICE_X, CAS_NA_PPV_PANGEA_PRICE_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_PRICE_W, CAS_NA_PPV_PANGEA_PRICE_H);
		GWM_Text_Create (CAS_NA_PPV_PANGEA_PRICE_ID, &stRect, pstContents->pszEvtPriceStr);
		GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_PRICE_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (CAS_NA_PPV_PANGEA_PRICE_ID, FONT_SIZE_XS);
		GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_PRICE_ID, COL(CAS_NA_PPV_PANGEA_COLOR_BLACK), 0x00);

		// Draw Event price value
		xmgr_cas_NaPpvBannerGetStringWidth_Pangea(pstContents->pszEvtPriceStr, FONT_SIZE_XS, &prevStringWidth);
		OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_PRICE_X+prevStringWidth+10, CAS_NA_PPV_PANGEA_PRICE_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_PRICE_W, CAS_NA_PPV_PANGEA_PRICE_H);
		GWM_Text_Create (CAS_NA_PPV_PANGEA_PRICE_STR_ID, &stRect, pstContents->szEventPrice);
		GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_PRICE_STR_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (CAS_NA_PPV_PANGEA_PRICE_STR_ID, FONT_SIZE_XS);
		GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_PRICE_STR_ID, COL(CAS_NA_PPV_PANGEA_COLOR_GREEN), 0x00);

		// Draw Event price euro icon
		xmgr_cas_NaPpvBannerGetStringWidth_Pangea((HUINT8*)pstContents->szEventPrice, FONT_SIZE_XS, &prevStringWidth);
		OSD_GFX_SetRect (&imgRect, stRect.x+prevStringWidth, stRect.y+FONT_SIZE_XS-CAS_NA_PPV_PANGEA_EURO_ICON_MARGIN_Y, stRect.w, stRect.h);
		GWM_Button_Create(CAS_NA_PPV_PANGEA_PRICE_EURO_ICON_ID, &imgRect, NULL);
		GWM_APP_SetObjectDrawMethod(CAS_NA_PPV_PANGEA_PRICE_EURO_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBannerDrawEuroIconButton_Pangea);
	}
	else
	{
		// Draw Event price
		OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_PRICE_X, CAS_NA_PPV_PANGEA_PRICE_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_PRICE_W, CAS_NA_PPV_PANGEA_PRICE_H);
		GWM_Text_Create (CAS_NA_PPV_PANGEA_PRICE_ID, &stRect, pstContents->pszEvtPriceStr);
		GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_PRICE_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (CAS_NA_PPV_PANGEA_PRICE_ID, FONT_SIZE_XS);
		GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_PRICE_ID, COL(CAS_NA_PPV_PANGEA_COLOR_BLACK), 0x00);

		// Draw Event price value - slice cost
		xmgr_cas_NaPpvBannerGetStringWidth_Pangea(pstContents->pszEvtPriceStr, FONT_SIZE_XS, &prevStringWidth);
		OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_PRICE_X+prevStringWidth+10, CAS_NA_PPV_PANGEA_PRICE_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_PRICE_W, CAS_NA_PPV_PANGEA_PRICE_H);
		GWM_Text_Create (CAS_NA_PPV_PANGEA_PRICE_STR_ID, &stRect, pstContents->szSliceCost);
		GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_PRICE_STR_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (CAS_NA_PPV_PANGEA_PRICE_STR_ID, FONT_SIZE_XS);
		GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_PRICE_STR_ID, COL(CAS_NA_PPV_PANGEA_COLOR_GREEN), 0x00);

		// Draw Event price euro icon
		xmgr_cas_NaPpvBannerGetStringWidth_Pangea((HUINT8*)pstContents->szSliceCost, FONT_SIZE_XS, &prevStringWidth);
		OSD_GFX_SetRect (&imgRect, stRect.x+prevStringWidth, stRect.y+FONT_SIZE_XS-CAS_NA_PPV_PANGEA_EURO_ICON_MARGIN_Y, stRect.w, stRect.h);
		GWM_Button_Create(CAS_NA_PPV_PANGEA_PRICE_EURO_ICON_ID, &imgRect, NULL);
		GWM_APP_SetObjectDrawMethod(CAS_NA_PPV_PANGEA_PRICE_EURO_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBannerDrawEuroIconButton_Pangea);

		// Draw Event price value - slice time
		xmgr_cas_NaPpvBannerGetStringWidth_Pangea((HUINT8*)pstContents->szSliceCost, FONT_SIZE_XS, &prevStringWidth);
		stRect.x += (prevStringWidth+10+10);
		GWM_Text_Create (CAS_NA_PPV_PANGEA_SLICETIME_STR_ID, &stRect, pstContents->szSliceTime);
		GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_SLICETIME_STR_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (CAS_NA_PPV_PANGEA_SLICETIME_STR_ID, FONT_SIZE_XS);
		GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_SLICETIME_STR_ID, COL(CAS_NA_PPV_PANGEA_COLOR_GREEN), 0x00);
	}

	// Draw Remained credit
	OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_REMAIN_CREDIT_X, CAS_NA_PPV_PANGEA_REMAIN_CREDIT_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_REMAIN_CREDIT_W, CAS_NA_PPV_PANGEA_REMAIN_CREDIT_H);
	GWM_Text_Create (CAS_NA_PPV_PANGEA_REMAIN_CREDIT_ID, &stRect, pstContents->pszRemainCreditStr);
	GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_REMAIN_CREDIT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (CAS_NA_PPV_PANGEA_REMAIN_CREDIT_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_REMAIN_CREDIT_ID, COL(CAS_NA_PPV_PANGEA_COLOR_BLACK), 0x00);

	// Draw Remained credit value
	xmgr_cas_NaPpvBannerGetStringWidth_Pangea(pstContents->pszRemainCreditStr, FONT_SIZE_XS, &prevStringWidth);
	OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_REMAIN_CREDIT_X+prevStringWidth+10, CAS_NA_PPV_PANGEA_REMAIN_CREDIT_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_REMAIN_CREDIT_W, CAS_NA_PPV_PANGEA_REMAIN_CREDIT_H);
	GWM_Text_Create (CAS_NA_PPV_PANGEA_REMAIN_CREDIT_STR_ID, &stRect, pstContents->szRemainCredit);
	GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_REMAIN_CREDIT_STR_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (CAS_NA_PPV_PANGEA_REMAIN_CREDIT_STR_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_REMAIN_CREDIT_STR_ID, COL(CAS_NA_PPV_PANGEA_COLOR_GREEN), 0x00);

	// Draw Remained credit euro icon
	xmgr_cas_NaPpvBannerGetStringWidth_Pangea((HUINT8*)pstContents->szRemainCredit, FONT_SIZE_XS, &prevStringWidth);
	OSD_GFX_SetRect (&imgRect, stRect.x+prevStringWidth, stRect.y+FONT_SIZE_XS-CAS_NA_PPV_PANGEA_EURO_ICON_MARGIN_Y, stRect.w, stRect.h);
	GWM_Button_Create(CAS_NA_PPV_PANGEA_CREDIT_EURO_ICON_ID, &imgRect, NULL);
	GWM_APP_SetObjectDrawMethod(CAS_NA_PPV_PANGEA_CREDIT_EURO_ICON_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBannerDrawEuroIconButton_Pangea);

	// Color Button
	OSD_GFX_SetRect(&stRect, CAS_NA_PPV_PANGEA_COLOR_BTN_X, CAS_NA_PPV_PANGEA_COLOR_BTN_Y, CAS_NA_PPV_PANGEA_COLOR_BTN_W, CAS_NA_PPV_PANGEA_COLOR_BTN_H);
	GWM_Button_Create (CAS_NA_PPV_PANGEA_COLOR_BTN_ID, &stRect, NULL);
	GWM_APP_SetObjectDrawMethod (CAS_NA_PPV_PANGEA_COLOR_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBannerDrawColorButton_Pangea);

	// Color Button help
	OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_X, CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_W, CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_H);
	GWM_Text_Create (CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_ID, &stRect, pstContents->pszBtnColorHelpStr);
	GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_ID, COL(CAS_NA_PPV_PANGEA_COLOR_DISABLE), 0x00);

	if (pstContents->nCardCredit < pstContents->nEventCredit)	// credit 이 부족한 경우 OK 버튼 없이 에러 메시지 표시
	{
		pstContents->pszConfirmStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_ERR_NOT_ENOUGH_CREDIT_ID);
		// Confirm message
		OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_X, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_Y, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_W, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_H);
		GWM_Text_Create (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, &stRect, pstContents->pszConfirmStr);
		GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, FONT_SIZE_XS);
		GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, COL(CAS_NA_PPV_PANGEA_COLOR_GREEN), 0x00);
	}
	else
	{
		// OK Button
		OSD_GFX_SetRect(&stRect, CAS_NA_PPV_PANGEA_OK_BTN_X, CAS_NA_PPV_PANGEA_OK_BTN_Y, CAS_NA_PPV_PANGEA_OK_BTN_W, CAS_NA_PPV_PANGEA_OK_BTN_H);
		GWM_Button_Create (CAS_NA_PPV_PANGEA_OK_BTN_ID, &stRect, NULL);
		GWM_APP_SetObjectDrawMethod (CAS_NA_PPV_PANGEA_OK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBannerDrawOKButton_Pangea);

		// OK Button help
		OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_OK_BTN_HELP_X, CAS_NA_PPV_PANGEA_OK_BTN_HELP_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_OK_BTN_HELP_W, CAS_NA_PPV_PANGEA_OK_BTN_HELP_H);
		GWM_Text_Create (CAS_NA_PPV_PANGEA_OK_BTN_HELP_ID, &stRect, pstContents->pszBtnOKHelpStr);
		GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_OK_BTN_HELP_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (CAS_NA_PPV_PANGEA_OK_BTN_HELP_ID, FONT_SIZE_XS);
		GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_OK_BTN_HELP_ID, COL(CAS_NA_PPV_PANGEA_COLOR_WHITE), 0x00);
	}

#if 0	// Back 버튼은 Blue 모드에서 나타남
	// BACK Button
	OSD_GFX_SetRect(&stRect, CAS_NA_PPV_PANGEA_BACK_BTN_X, CAS_NA_PPV_PANGEA_BACK_BTN_Y, CAS_NA_PPV_PANGEA_BACK_BTN_W, CAS_NA_PPV_PANGEA_BACK_BTN_H);
	GWM_Button_Create (CAS_NA_PPV_PANGEA_BACK_BTN_ID, &stRect, NULL);
	GWM_APP_SetObjectDrawMethod (CAS_NA_PPV_PANGEA_BACK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBannerDrawBackButton_Pangea);

	// BACK Button help
	OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_BACK_BTN_HELP_X, CAS_NA_PPV_PANGEA_BACK_BTN_HELP_Y, CAS_NA_PPV_PANGEA_OK_BTN_HELP_W, CAS_NA_PPV_PANGEA_OK_BTN_HELP_H);
	GWM_Text_Create (CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID, &stRect, pstContents->pszBtnBackHelpStr);
	GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID, COL(CAS_NA_PPV_PANGEA_COLOR_WHITE), 0x00);
#endif

	GWM_Obj_SetFocus (CAS_NA_PPV_PANGEA_OK_BTN_ID);

	return ERR_BUS_NO_ERROR;

}

/********************************************************************
 Function		: xmgr_cas_NaPpvBannerDrawEuroIconButton_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawEuroIconButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e911_4_24_Euro_Font_Green, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBannerDrawColorButton_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawColorButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32							nX1, nY1;
	OSD_Rect_t						stRect;
	OSD_StockImage_t				stImage;
	HUINT32							nImageBtnID;
	XmgrCas_NaPpvContents_Pangea_t		*pstContents = xmgr_cas_NaPpvBannerGetContents_Pangea();

	(void)pObject;
	(void)pArg;
	(void)param1;

	if (pstContents->nExtButtonType == eNA_PPV_COLOR_ICON_BLUE)
	{
		nImageBtnID = e921_2_24_Help_02Blue;
	}
	else
	{
		nImageBtnID = e921_2_24_Help_01Green;
	}

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (nImageBtnID, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBannerDrawOKButton_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawOKButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32			nX1, nY1;
	OSD_Rect_t		stRect;
	OSD_StockImage_t	stImage;

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
 Function		: xmgr_cas_NaPpvBannerDrawBackButton_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawBackButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvBannerDrawSelectedBackground_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawSelectedBackground_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32			nX1, nY1;
	OSD_Rect_t		stRect;
	OSD_StockImage_t	stImage;

	(void)pObject;
	(void)pArg;
	(void)param1;

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (e922_1_24_Banner2_Title, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBannerDrawBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e922_1_24_Banner2, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBannerDrawExtendedBackground_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBannerDrawExtendedBackground_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HINT32			nX1, nY1;
	OSD_Rect_t		stRect;
	OSD_StockImage_t	stImage;
	HUINT32			nImageBtnID;
	XmgrCas_NaPpvContents_Pangea_t		*pstContents = xmgr_cas_NaPpvBannerGetContents_Pangea();

	(void)pObject;
	(void)pArg;
	(void)param1;

	if (pstContents->nExtMode == eNA_PPV_EXT_MODE_PPV_INFO)
	{
		nImageBtnID = e922_1_24_Banner2_01Info;
	}
	else
	{
		nImageBtnID = e922_1_24_Banner2_01Info_S;
	}

	nX1 = pArea->x1 + pRect->x;
	nY1 = pArea->y1 + pRect->y;

	if (!AP_StockImg_GetImage (nImageBtnID, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		if (pstContents->nExtMode == eNA_PPV_EXT_MODE_PPV_INFO)
		{
			OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
		}
		else
		{
			OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height-40, pScreen, &stRect);
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t			xmgr_cas_NaPpvBannerAddLongStringWithWordwrap_Pangea(HUINT32 ulFmtObjID, OSD_Font_t *pFont, HUINT32 ulFmtObjWidth,HUINT8 *pszString, HUINT32 ulStrLen, Text_Property_t *pstTextProperty)
{
	BUS_Result_t				lResult = ERR_INITIAL_FAILED;
	HERROR					hError = ERR_FAIL;
	HUINT8					*pszBuf;
	HUINT8					*pucTraverse;
	HUINT32					ulCharNumPerLine = 1;
	HUINT8					aucLangCode[MAX_LANGCODE_LEN] = {0, };
	HUINT32 					ulLangCodeLen = 0;
	HINT32 					lSingleLineStrLen = 0;

	if(pszString == NULL || pFont == NULL || ulStrLen <= 0)
	{
		HxLOG_Print("[xmgr_cas_NaPpvBannerAddLongStringWithWordwrap_Pangea] pszString null \n");
		goto EXIT_EPGMAINAP_FUNC;
	}

	pszBuf = GetTextBuffer(TEXT_SIZE512);

	ulLangCodeLen = MWC_UTIL_GetLocaleCodeLen(pszString);
	HxSTD_memcpy(aucLangCode, pszString, ulLangCodeLen*sizeof(HUINT8));

/*****************************************************************************************
  *
  *  string이 길면 width에 맞게 여러번 잘라서 입력하기 위해, traversing한다.
  *
  *****************************************************************************************/
	pucTraverse = pszString + ulLangCodeLen;
	OSD_FONT_SetSize(pFont, pstTextProperty->ulFontSize);

	while (pucTraverse || ulCharNumPerLine > 0)
	{
		switch(pucTraverse[0])
		{
			case CHAR_VALUE_LINEFEED :
				// LF
				GWM_FMT_AddItem(ulFmtObjID, " ", pstTextProperty);
				pucTraverse++;
				break;
			case CHAR_VALUE_CARRIAGERETURN :
				if(pucTraverse[1] == CHAR_VALUE_LINEFEED)
				{
					// CR/LF
					GWM_FMT_AddItem(ulFmtObjID, " ", pstTextProperty);
					pucTraverse += 2;
				}
				break;
			default :
				break;
		}

/*****************************************************************************************
  *
  * pszLine = width내에 해당하는 string 조각
  * ulCharNumPerLine = width에 맞게 잘린 string의 char 갯수
  *
  *****************************************************************************************/
		lSingleLineStrLen = (HINT32)SvcCas_UtilStrLen(pucTraverse);
		hError = OSD_FONT_CountChByWidth_300468(	pFont,
													ulFmtObjWidth,
													aucLangCode,
													ulLangCodeLen,
													lSingleLineStrLen,
													pucTraverse,
													&ulCharNumPerLine);

		if(ulCharNumPerLine == 0)
		{
			break;
		}

		if (ulCharNumPerLine >= TEXT_SIZE512 - MAX_LANGCODE_LEN - 1)
		{
			HxLOG_Error("[xmgr_cas_NaPpvBannerAddLongStringWithWordwrap_Pangea] too long length per line!! max 508!! curLen(%d) \n", ulCharNumPerLine);
			goto EXIT_EPGMAINAP_FUNC;
		}

		HxSTD_memset(pszBuf, 0x00, TEXT_SIZE512);
		HxSTD_memcpy(pszBuf, aucLangCode, ulLangCodeLen);
		HxSTD_memcpy(&pszBuf[ulLangCodeLen], pucTraverse, ulCharNumPerLine);
		GWM_FMT_AddItem(ulFmtObjID, pszBuf, pstTextProperty);

		if(ulCharNumPerLine >= lSingleLineStrLen)
		{
			break;
		}
		pucTraverse += ulCharNumPerLine;
	}

	lResult = ERR_BUS_NO_ERROR;

EXIT_EPGMAINAP_FUNC:

	return lResult;
}

STATIC BUS_Result_t	xmgr_cas_NaPpvBannerExtendInfoDrawScroll_Pangea(int ID, OSD_Screen_t *pScreen, FMT_DrawItem_t *pDrawItem)
{
	BUS_Result_t			hRet = ERR_INITIAL_FAILED;
	FMT_OBJECT			obj = NULL;
	OSD_Rect_t			*pstRect;
	OSD_Font_t			*pFont = OSD_FONT_Get(eFont_SystemNormal);
	HUINT8				*pStr = NULL;
	HBOOL				bImg = FALSE;
	HUINT8				ucMainGenreIndex = 0;
	TEXT_DRAW_INFO_t	stTextInfo;

	obj = (FMT_OBJECT)BUS_MGR_GetObject(ID);

	if (obj == NULL)
	{
		hRet = ERR_NO_OBJECT;
		goto EXIT_EXTENDINFO_DRAWITEMS_FUNC;
	}

	if (pDrawItem == NULL)
	{
		hRet = ERR_FAIL;
		goto EXIT_EXTENDINFO_DRAWITEMS_FUNC;
	}

	if (pDrawItem->str == NULL)
	{
		hRet = ERR_INITIAL_FAILED;
		goto EXIT_EXTENDINFO_DRAWITEMS_FUNC;
	}

	OSD_FONT_SetSize(pFont, pDrawItem->stTextProperty.ulFontSize);
	pstRect = &pDrawItem->itemRect;
	pStr = pDrawItem->str;

	while(*pStr)
	{
		/* ==========================================================
		 *		Check Custom tag
		 * ========================================================= */
		if (*pStr == FMT_CUSTOMTAG_ON)
		{
			pStr++;

			if (*pStr == FMT_CUSTOMTAG_IMAGE)
			{
				if ( (pStr[1] == FMT_CUSTOMTAG_DATA_START) && (pStr[3] == FMT_CUSTOMTAG_DATA_END))
				{
					bImg = TRUE;
					ucMainGenreIndex = pStr[2];
				}
				pStr += 4;
			}
		}
		else
		{
			/* ==========================================================
			 *		Draw text and image
			 * ========================================================= */
			stTextInfo.ulFontSize = pDrawItem->stTextProperty.ulFontSize;
			stTextInfo.ulTextColor = pDrawItem->stTextProperty.cFontColor;
			stTextInfo.ulDrawMode = TEXTMODE_NORMAL;
			stTextInfo.ulXAlign = TEXT_ALIGN_LEFT;
			stTextInfo.ulYAlign = TEXT_ALIGN_MIDDLE;
			AP_UTIL_DrawFormattedText(pScreen, pFont, pstRect, &stTextInfo, pStr);

			break;
		}
	}

	hRet = ERR_BUS_NO_ERROR;

EXIT_EXTENDINFO_DRAWITEMS_FUNC:
	OSD_FONT_Release(pFont);

	return hRet;
}

STATIC BUS_Result_t	xmgr_cas_NaPpvBannerExtendInfoDrawScroll_Pangea(	OSD_Screen_t *pScreen,
															OSD_Area_t *pArea,
															OSD_Rect_t *pRect,
															FMT_OBJECT pObject)
{
	HINT32			nX1, nY1;
	OSD_Rect_t		stRect;
	OSD_StockImage_t	stImage;
	XmgrCas_NaPpvContents_Pangea_t 	*pstContents = xmgr_cas_NaPpvBannerGetContents_Pangea();

	(void)pObject;

	if (pstContents->nExtMode == eNA_PPV_EXT_MODE_PPV_INFO)
	{
		nX1 = CAS_NA_PPV_PANGEA_EXTEND_INFO_SCROLL_UP_X;
		nY1 = CAS_NA_PPV_PANGEA_EXTEND_INFO_SCROLL_UP_Y;

		if (!AP_StockImg_GetImage (e922_2_24_Arrow_1Up, &stImage))
		{
			OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
			OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
		}

		nX1 = CAS_NA_PPV_PANGEA_EXTEND_INFO_SCROLL_DOWN_X;
		nY1 = CAS_NA_PPV_PANGEA_EXTEND_INFO_SCROLL_DOWN_Y;

		if (!AP_StockImg_GetImage (e922_2_24_Arrow_2Down, &stImage))
		{
			OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
			OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
		}
	}
	else if(pstContents->nExtMode == eNA_PPV_EXT_MODE_PPV_PURCHASE_INFO)
	{
		nX1 = CAS_NA_PPV_PANGEA_EXTEND_INFO_SCROLL_UP_X;
		nY1 = CAS_NA_PPV_PANGEA_EXTEND_INFO_SCROLL_UP_Y;

		if (!AP_StockImg_GetImage (e922_2_24_Arrow_1Up, &stImage))
		{
			OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
			OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
		}

		nX1 = CAS_NA_PPV_PANGEA_EXTEND_INFO_SCROLL_DOWN_X;
		nY1 = CAS_NA_PPV_PANGEA_EXTEND_INFO_SCROLL_DOWN_Y - 60;

		if (!AP_StockImg_GetImage (e922_2_24_Arrow_2Down, &stImage))
		{
			OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
			OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
		}
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t	xmgr_cas_NaPpvBannerMakeExtendedInfo_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents)
{
	OSD_Rect_t	stRect;
	OSD_Font_t	*pFont = OSD_FONT_Get(eFont_SystemNormal);
	Text_Property_t 	stTextProperty;
	HUINT32		ulSize = 0;
	HUINT8		*strInfo = NULL;
	HERROR		hErr = ERR_OK;
	UNIXTIME	nEvtStartTime = pstContents->nElapsedTime;
	Handle_t	hCurSvcHandle = pstContents->hSvcHandle;

//	HUINT8* szShortInfo = "Events :\nSPORT IS CAMPIONATO MALAN.\n1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890 1234567890........";
//	HUINT8* szExtInfo = "LINE01\nLINE02\nLINE03\nLINE04\nLINE05\nLINE06\nLINE07\nLINE08\nLINE09\nLINE10\nLINE11\nLINE12\nLINE13\nLINE14\nLINE15\nLINE16\nLINE17\nLINE18\nLINE19\nLINE20\n";

	if (pstContents->nExtMode == eNA_PPV_EXT_MODE_PPV_PURCHASE_INFO)
	{
		// Extend info background
		OSD_GFX_SetRect(&stRect, CAS_NA_PPV_PANGEA_EXTEND_BACK_X, CAS_NA_PPV_PANGEA_EXTEND_BACK_Y, CAS_NA_PPV_PANGEA_EXTEND_BACK_W, CAS_NA_PPV_PANGEA_EXTEND_BACK_H-40);
		GWM_Button_Create (CAS_NA_PPV_PANGEA_EXTEND_BACK_ID, &stRect, NULL);
		GWM_APP_SetObjectDrawMethod (CAS_NA_PPV_PANGEA_EXTEND_BACK_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBannerDrawExtendedBackground_Pangea);

		memset(pstContents->szExtendTitle, 0, 64);
		MWC_UTIL_DvbStrncpy(pstContents->szExtendTitle, GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_TERM_CONDITION_ID), 64-1);
		OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_X, CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_Y - FONT_SIZE_L, CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_W, CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_H);
		GWM_Text_Create (CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_ID, &stRect, pstContents->pszBtnColorHelpStr);
		GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_ID, FONT_SIZE_L);
		GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_ID, COL(CAS_NA_PPV_PANGEA_COLOR_WHITE), 0x00);

		stTextProperty.ulFontSize = FONT_SIZE_S;
		stTextProperty.ulFontAlign = TEXT_ALIGN_LEFT;
		stTextProperty.cFontColor = COL(CAS_NA_PPV_PANGEA_COLOR_BLACK);

		OSD_GFX_SetRect(&stRect, CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_X, CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_Y - FONT_SIZE_S, CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_W, CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_H-60);
		GWM_FMT_Create(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID, &stRect, FMT_OWN_DRAWITEM | FMT_SCROLL_ON, 5);
		GWM_FMT_ChangeItemDrawMethod(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID, xmgr_cas_NaPpvBannerExtendInfoDrawScroll_Pangea);
		GWM_FMT_ChangeScrollDrawMethod(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID, xmgr_cas_NaPpvBannerExtendInfoDrawScroll_Pangea);
		hErr = xmgr_cas_NaRequestPpvGetProductDisclaimer_Mediaset(&ulSize, &strInfo);
		if (hErr != ERR_OK)
		{
			SvcEpg_FindParam_t		stParam = {0, };
			POINTER_LIST_T			*pEventList = NULL;
			POINTER_LIST_T			*pList = NULL;
			SvcEpg_ExtEvent_t			*pstExtEvt = NULL;
			BUS_Result_t			lResult = ERR_BUS_NO_ERROR;
			HUINT8 					i = 0, *pszBuf, *pszBuf_Temp;
			HUINT8					*pucConvertString = NULL;
			HINT32					lStrLen = 0;

			pszBuf = GetTextBuffer(TEXT_SIZE512);
			pszBuf_Temp = GetTextBuffer(TEXT_SIZE512);
			HxSTD_memset(pszBuf, 0x00, sizeof(HUINT8)*TEXT_SIZE512);
			HxSTD_memset(pszBuf_Temp, 0x00, sizeof(HUINT8)*TEXT_SIZE512);

			HxSTD_memset(&stParam, 0x00, sizeof(SvcEpg_FindParam_t));
			stParam.ulFindMask = (EPG_FIND_MASK_SCH | EPG_FIND_MASK_PF | EPG_FIND_MASK_TIME);
			stParam.hSvcHandle = hCurSvcHandle;
			stParam.ulStartTime = nEvtStartTime;
			stParam.ulEndTime = nEvtStartTime+1;

			// TODO: EPG 소스 정리로 새로 구현하세요..
			//hErr = SVC_EPG_GetExtEvent(&stParam, &pEventList);
			if (hErr == ERR_OK && pEventList != NULL)
			{
				pstExtEvt = (SvcEpg_ExtEvent_t *)UTIL_LINKEDLIST_GetContents(pEventList);

				HxSTD_memset(pszBuf, 0x00, sizeof(HUINT8)*TEXT_SIZE512);
				HxSTD_memset(pszBuf_Temp, 0x00, sizeof(HUINT8)*TEXT_SIZE512);

				if (pstExtEvt)
				{
#if 0
					if (pstExtEvt->astExtInfo[0].astItem[i].pItemDescription != NULL)
					{
						pucConvertString = NULL;
						hErr = SVC_EPG_ConvertEpgTextToString(pstExtEvt->astExtInfo[0].astItem[i].pItemDescription, &pucConvertString);
						if(hErr != ERR_OK)
						{
							HxLOG_Print("SVC_EPG_ConvertEpgTextToString fail \n");
						}
						MWC_UTIL_DvbStrcpy(pszBuf, pucConvertString);
						MWC_UTIL_DvbStrcpy(pszBuf_Temp, pucConvertString);

						if(pstExtEvt->astExtInfo[0].astItem[i].pItemChar != NULL)
						{
							pucConvertString = NULL;
							hErr = SVC_EPG_ConvertEpgTextToString(pstExtEvt->astExtInfo[0].astItem[i].pItemChar, &pucConvertString);
							if(hErr != ERR_OK)
							{
								HxLOG_Print("SVC_EPG_ConvertEpgTextToString fail \n");
							}
							sprintf(pszBuf, "%s : %s", pszBuf_Temp, pucConvertString);
						}
						lStrLen = SvcCas_UtilStrLen(pszBuf);
						if(lStrLen > 0)
						{
							xmgr_cas_NaPpvBannerAddLongStringWithWordwrap_Pangea(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID, pFont, CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_W, pszBuf, lStrLen, &stTextProperty);
						}

					}
					else if(pstExtEvt->astExtInfo[0].astItem[i].pItemChar != NULL)
					{
						pucConvertString = NULL;
						hErr = SVC_EPG_ConvertEpgTextToString(pstExtEvt->astExtInfo[0].astItem[i].pItemChar, &pucConvertString);
						if(hErr != ERR_OK)
						{
							HxLOG_Print("SVC_EPG_ConvertEpgTextToString fail \n");
						}
						MWC_UTIL_DvbStrcpy(pszBuf, pucConvertString);
						lStrLen = SvcCas_UtilStrLen(pszBuf);
						if(lStrLen > 0)
						{
							xmgr_cas_NaPpvBannerAddLongStringWithWordwrap_Pangea(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID, pFont, CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_W, pszBuf, lStrLen, &stTextProperty);
						}
					}
#endif
					if(pstExtEvt->astExtInfo[0].pstExtText != NULL)
					{
						pucConvertString = NULL;
						// TODO: EPG 소스 정리로 인해 새로 구현하세요.
//						hErr = SVC_EPG_ConvertEpgTextToString(pstExtEvt->astExtInfo[0].pstExtText, &pucConvertString);
						if(hErr != ERR_OK)
						{
							HxLOG_Print("SVC_EPG_ConvertEpgTextToString fail \n");
						}
						else
						{
							HxLOG_Print("pucConvertString = %s\n", pucConvertString);
						}

						lStrLen = SvcCas_UtilStrLen(pucConvertString);

						xmgr_cas_NaPpvBannerAddLongStringWithWordwrap_Pangea(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID, pFont, CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_W, pucConvertString, lStrLen, &stTextProperty);
					}
				}

				// TODO: EPG 소스 정리로 인해 새로 구현하세요.
				//hErr = SVC_EPG_ReleaseExtEventList(pEventList);
			}
			else
			{
				HxLOG_Print("SVC_EPG_GetEventList fail!!\n");
			}

		}
		else
		{
			xmgr_cas_NaPpvBannerAddLongStringWithWordwrap_Pangea(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID, pFont, CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_W, strInfo, ulSize, &stTextProperty);
		}
	}
	else if (pstContents->nExtMode == eNA_PPV_EXT_MODE_PPV_INFO)
	{
		// Extend info background
		OSD_GFX_SetRect(&stRect, CAS_NA_PPV_PANGEA_EXTEND_BACK_X, CAS_NA_PPV_PANGEA_EXTEND_BACK_Y, CAS_NA_PPV_PANGEA_EXTEND_BACK_W, CAS_NA_PPV_PANGEA_EXTEND_BACK_H);
		GWM_Button_Create (CAS_NA_PPV_PANGEA_EXTEND_BACK_ID, &stRect, NULL);
		GWM_APP_SetObjectDrawMethod (CAS_NA_PPV_PANGEA_EXTEND_BACK_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBannerDrawExtendedBackground_Pangea);

		memset(pstContents->szExtendTitle, 0, 64);
		MWC_UTIL_DvbStrncpy(pstContents->szExtendTitle, pstContents->szEventName, 64-1);
		OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_X, CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_Y - FONT_SIZE_L, CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_W, CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_H);
		GWM_Text_Create (CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_ID, &stRect, pstContents->szExtendTitle);
		GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_ID, TEXT_ALIGN_LEFT);
		GWM_Text_SetSize (CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_ID, FONT_SIZE_L);
		GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_ID, COL(CAS_NA_PPV_PANGEA_COLOR_WHITE), 0x00);

		stTextProperty.ulFontSize = FONT_SIZE_S;
		stTextProperty.ulFontAlign = TEXT_ALIGN_LEFT;
		stTextProperty.cFontColor = COL(CAS_NA_PPV_PANGEA_COLOR_BLACK);

		OSD_GFX_SetRect(&stRect, CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_X, CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_Y - FONT_SIZE_S, CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_W, CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_H);
		GWM_FMT_Create(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID, &stRect, FMT_OWN_DRAWITEM | FMT_SCROLL_ON, 7);
		GWM_FMT_ChangeItemDrawMethod(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID, xmgr_cas_NaPpvBannerExtendInfoDrawScroll_Pangea);
		GWM_FMT_ChangeScrollDrawMethod(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID, xmgr_cas_NaPpvBannerExtendInfoDrawScroll_Pangea);
		hErr = xmgr_cas_NaRequestPpvGetProductDescription_Mediaset(&ulSize, &strInfo);
		if (hErr != ERR_OK)
		{
			SvcEpg_FindParam_t		stParam = {0, };
			POINTER_LIST_T			*pEventList = NULL;
			POINTER_LIST_T			*pList = NULL;
			SvcEpg_Event_t				*pMwEvent = NULL;
			EPG_APEVENT_t			*pApEvent = NULL;
			BUS_Result_t			lResult = ERR_BUS_NO_ERROR;

			HxSTD_memset(&stParam, 0x00, sizeof(SvcEpg_FindParam_t));
			stParam.ulFindMask = (EPG_FIND_MASK_SCH | EPG_FIND_MASK_PF | EPG_FIND_MASK_TIME);
			stParam.hSvcHandle = hCurSvcHandle;
			stParam.ulStartTime = nEvtStartTime;
			stParam.ulEndTime = nEvtStartTime+1;

			// TODO: EPG 소스 정리로 인해 새로 구현하세요.
//			hErr = SVC_EPG_GetEventList(&stParam, &pEventList);
			if (hErr == ERR_OK && pEventList != NULL)
			{
				pMwEvent = (SvcEpg_Event_t *)UTIL_LINKEDLIST_GetContents(pEventList);
				if (pMwEvent)
				{
					HxLOG_Print("EvtId (%d), StTime (0x%x), Dur (0x%x)\n", pMwEvent->stEventInfo.stEitEventInfo.nEventId, pMwEvent->ulStartTime, pMwEvent->ulDuration);

					pApEvent = (EPG_APEVENT_t*)OxAP_Malloc(sizeof(EPG_APEVENT_t));
					if(pApEvent)
					{
						VK_memset32(pApEvent, 0x00, sizeof(EPG_APEVENT_t));
						lResult = AP_EPGMAIN_ConvertEventMW2AP(pMwEvent, pApEvent);
						if(lResult == ERR_BUS_NO_ERROR)
						{
//							ulSize = SvcCas_UtilStrLen(pApEvent->aucEventText);
							xmgr_cas_NaPpvBannerAddLongStringWithWordwrap_Pangea(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID, pFont, CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_W, pApEvent->aucEventText, ulSize, &stTextProperty);
						}
						else
						{
							HxLOG_Print("AP_EPGMAIN_ConvertEventMW2AP ERROR\n");
						}
						OxAP_Free(pApEvent);
					}
					else
					{
						HxLOG_Print("pApEvent NULL\n");
					}
				}
				else
				{
					HxLOG_Print("pMwEvent NULL\n");
				}
				SVC_EPG_ReleaseEventList(pEventList);
			}
			else
			{
				HxLOG_Print("SVC_EPG_GetEventList fail!!\n");
			}
		}
		else
		{
			xmgr_cas_NaPpvBannerAddLongStringWithWordwrap_Pangea(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID, pFont, CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_W, strInfo, ulSize, &stTextProperty);
		}
	}

	if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID))
	{
		GWM_APP_PreventDraw_LowPriority(APP_CAS_PPV_PRIORITY);
	}

	if(pFont != NULL)
	{
		OSD_FONT_Release(pFont);
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_cas_NaPpvBannerGetStringWidth_Pangea(HUINT8* string, HUINT32 ulFontSize, HUINT32 *ulWidth)
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

STATIC BUS_Result_t	xmgr_cas_NaPpvBannerUpdateSubItemsByColorBtnType_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents)
{
	OSD_Rect_t			stRect;

	if (pstContents->nExtButtonType == eNA_PPV_COLOR_ICON_BLUE)
	{
		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_BACK_BTN_ID) == NULL)
		{
			// BACK Button
			OSD_GFX_SetRect(&stRect, CAS_NA_PPV_PANGEA_BACK_BTN_X, CAS_NA_PPV_PANGEA_BACK_BTN_Y, CAS_NA_PPV_PANGEA_BACK_BTN_W, CAS_NA_PPV_PANGEA_BACK_BTN_H);
			GWM_Button_Create (CAS_NA_PPV_PANGEA_BACK_BTN_ID, &stRect, NULL);
			GWM_APP_SetObjectDrawMethod (CAS_NA_PPV_PANGEA_BACK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBannerDrawBackButton_Pangea);

				// BACK Button help
			OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_BACK_BTN_HELP_X, CAS_NA_PPV_PANGEA_BACK_BTN_HELP_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_OK_BTN_HELP_W, CAS_NA_PPV_PANGEA_OK_BTN_HELP_H);
			GWM_Text_Create (CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID, &stRect, pstContents->pszBtnBackHelpStr);
			GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID, TEXT_ALIGN_LEFT);
			GWM_Text_SetSize (CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID, FONT_SIZE_XS);
			GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID, COL(CAS_NA_PPV_PANGEA_COLOR_WHITE), 0x00);
		}

		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_SELECTED_BACK_ID) == NULL)
		{
			// Selected background
			OSD_GFX_SetRect(&stRect, CAS_NA_PPV_PANGEA_SELECTED_BACK_X, CAS_NA_PPV_PANGEA_SELECTED_BACK_Y, CAS_NA_PPV_PANGEA_SELECTED_BACK_W, CAS_NA_PPV_PANGEA_SELECTED_BACK_H);
			GWM_Button_Create (CAS_NA_PPV_PANGEA_SELECTED_BACK_ID, &stRect, NULL);
			GWM_APP_SetObjectDrawMethod (CAS_NA_PPV_PANGEA_SELECTED_BACK_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBannerDrawSelectedBackground_Pangea);
		}

		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_EVENT_SELECTED_ID) == NULL)
		{
			// Selected event title
			OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_EVENT_SELECTED_X, CAS_NA_PPV_PANGEA_EVENT_SELECTED_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_EVENT_SELECTED_W, CAS_NA_PPV_PANGEA_EVENT_SELECTED_H);
			GWM_Text_Create (CAS_NA_PPV_PANGEA_EVENT_SELECTED_ID, &stRect, pstContents->pszSelectStr);
			GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_EVENT_SELECTED_ID, TEXT_ALIGN_LEFT);
			GWM_Text_SetSize (CAS_NA_PPV_PANGEA_EVENT_SELECTED_ID, FONT_SIZE_XS);
			GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_EVENT_SELECTED_ID, COL(CAS_NA_PPV_PANGEA_COLOR_WHITE), 0x00);
		}

		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID) == NULL)
		{
			// Confirm message
			OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_X, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_W, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_H);
			GWM_Text_Create (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, &stRect, pstContents->pszConfirmStr);
			GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, TEXT_ALIGN_LEFT);
			GWM_Text_SetSize (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, FONT_SIZE_XS);
			GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, COL(CAS_NA_PPV_PANGEA_COLOR_GREEN), 0x00);
		}
	}
	else if (pstContents->nExtButtonType == eNA_PPV_COLOR_ICON_GREEN)
	{
		if(pstContents->nExtMode == eNA_PPV_EXT_MODE_PPV_INFO)
		{
			if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_BACK_BTN_ID) == NULL)
			{
				// BACK Button
				OSD_GFX_SetRect(&stRect, CAS_NA_PPV_PANGEA_BACK_BTN_X, CAS_NA_PPV_PANGEA_BACK_BTN_Y, CAS_NA_PPV_PANGEA_BACK_BTN_W, CAS_NA_PPV_PANGEA_BACK_BTN_H);
				GWM_Button_Create (CAS_NA_PPV_PANGEA_BACK_BTN_ID, &stRect, NULL);
				GWM_APP_SetObjectDrawMethod (CAS_NA_PPV_PANGEA_BACK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBannerDrawBackButton_Pangea);

					// BACK Button help
				OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_BACK_BTN_HELP_X, CAS_NA_PPV_PANGEA_BACK_BTN_HELP_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_OK_BTN_HELP_W, CAS_NA_PPV_PANGEA_OK_BTN_HELP_H);
				GWM_Text_Create (CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID, &stRect, pstContents->pszBtnBackHelpStr);
				GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID, TEXT_ALIGN_LEFT);
				GWM_Text_SetSize (CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID, FONT_SIZE_XS);
				GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID, COL(CAS_NA_PPV_PANGEA_COLOR_WHITE), 0x00);
			}
		}
		else
		{
			// BACK Button 제거
			if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_BACK_BTN_ID))
			{
				GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_BACK_BTN_ID);
				GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID);
			}
		}

		// Selected background 제거
		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_SELECTED_BACK_ID))
		{
			GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_SELECTED_BACK_ID);
		}

		// Seleted event title 제거
		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_EVENT_SELECTED_ID))
		{
			GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_EVENT_SELECTED_ID);
		}

		// Confirm message 제거
		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID))
		{
			if (pstContents->pszConfirmStr == GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_PURCHASE_CONFIRM_ID))
			{
				GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID);
			}
		}
	}
	else
	{
		// Remained credit 제거
		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_REMAIN_CREDIT_ID))
		{
			GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_REMAIN_CREDIT_ID);
			GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_REMAIN_CREDIT_STR_ID);
			GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_CREDIT_EURO_ICON_ID);
		}

		// OK Button 제거
		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_OK_BTN_ID))
		{
			GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_OK_BTN_ID);
			GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_OK_BTN_HELP_ID);
		}

		// BACK Button 제거
		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_BACK_BTN_ID))
		{
			GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_BACK_BTN_ID);
			GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID);
		}

		// Selected background 제거
		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_SELECTED_BACK_ID))
		{
			GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_SELECTED_BACK_ID);
		}

		// Seleted event title 제거
		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_EVENT_SELECTED_ID))
		{
			GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_EVENT_SELECTED_ID);
		}

		// Confirm message 갱신
		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID))
		{
			GWM_Text_SetText(CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, pstContents->pszConfirmStr);
		}
		else
		{
			// Error message 표시
			OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_X, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_Y, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_W, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_H);
			GWM_Text_Create (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, &stRect, pstContents->pszConfirmStr);
			GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, TEXT_ALIGN_LEFT);
			GWM_Text_SetSize (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, FONT_SIZE_XS);
			GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, COL(CAS_NA_PPV_PANGEA_COLOR_GREEN), 0x00);
		}


	}
}

STATIC BUS_Result_t	xmgr_cas_NaPpvBannerRemoveExtendWindow_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents)
{
	pstContents->nExtMode = eNA_PPV_EXT_MODE_NONE;

	// Extended info 제거
	if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID))
	{
		GWM_FMT_Delete(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID);
		GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID);

		GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_EXTEND_BACK_ID);
		GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_EXTEND_INFO_TITLE_ID);

		GWM_APP_DisablePreventDraw();
	}
}

STATIC BUS_Result_t	xmgr_cas_NaPpvBannerUpdatePpvWindow_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents)
{
	if (pstContents->nPPVType == CA_EVENT_SPECIAL_EVENT)
	{
		// Draw Start time value
		GWM_Text_SetText(CAS_NA_PPV_PANGEA_START_TIME_STR_ID, (HUINT8*)pstContents->szStartTime);

		// Draw Event price value
		GWM_Text_SetText(CAS_NA_PPV_PANGEA_PRICE_STR_ID, (HUINT8*)pstContents->szEventPrice);
	}
	else	// PPT
	{
		// Draw Event price value - slice cost
		GWM_Text_SetText(CAS_NA_PPV_PANGEA_PRICE_STR_ID, (HUINT8*)pstContents->szSliceCost);

		// Draw Event price value - slice time
		GWM_Text_SetText(CAS_NA_PPV_PANGEA_SLICETIME_STR_ID, (HUINT8*)pstContents->szSliceTime);
	}

	if (pstContents->nExtMode == eNA_PPV_EXT_MODE_PPV_INFO)
	{
		xmgr_cas_NaPpvBannerMakeExtendedInfo_Pangea(pstContents);
		pstContents->pszBtnColorHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_HIDE_INFORMATION_ID);
		pstContents->pszBtnOKHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_IPPV_NAME_SELECT_EVENT_ID);	// 검토
	}
	else if (pstContents->nExtMode == eNA_PPV_EXT_MODE_PPV_PURCHASE_INFO)
	{
		xmgr_cas_NaPpvBannerMakeExtendedInfo_Pangea(pstContents);
		pstContents->pszBtnColorHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_CLOSE_ID);
		pstContents->pszBtnOKHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_CONFIRM_SELECT_ID);	// 검토
	}
	else
	{
		if (pstContents->nExtButtonType == eNA_PPV_COLOR_ICON_BLUE)
		{
			pstContents->pszBtnColorHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_TERM_CONDITION_ID);
			pstContents->pszBtnOKHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_CONFIRM_SELECT_ID);	// 검토
		}
		else
		{
			pstContents->pszBtnColorHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_SHOW_INFORMATION_ID);
			pstContents->pszBtnOKHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_IPPV_NAME_SELECT_EVENT_ID);	// 검토
		}
	}

	GWM_Text_SetText(CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_ID, pstContents->pszBtnColorHelpStr);
	GWM_Text_SetText(CAS_NA_PPV_PANGEA_OK_BTN_HELP_ID, pstContents->pszBtnOKHelpStr);

	return ERR_BUS_NO_ERROR;
}

// MSG_GWM_CREATE
STATIC HERROR xmgr_cas_NaPpvBannerMsgCreated_Pangea (XmgrCas_NaPpvContents_Pangea_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		 hErr;

	// i-plate가 있으면 닫아주자
	BUS_MGR_Destroy(AP_IPlate_Proc);
	BUS_MGR_Destroy(AP_ExtPlate_Proc);

	// Init the contents
	hErr = xmgr_cas_NaPpvBannerInitContents_Pangea (pstContents, p1, p2);
	if (hErr != ERR_BUS_NO_ERROR)
	{
		HxLOG_Critical("\n\n");
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}

	// Draw the windows
	hErr = xmgr_cas_NaPpvBannerDrawDefaultUi_Pangea (pstContents);
	if (hErr != ERR_BUS_NO_ERROR)
	{
		HxLOG_Critical("\n\n");
		BUS_MGR_Destroy (NULL);
		return MESSAGE_BREAK;
	}

	if (pstContents->nPPVType == CA_EVENT_SPECIAL_EVENT)
	{
		BUS_SetTimer(CAS_NA_PPV_PANGEA_PPV_CHECK_TIMER_ID, CAS_NA_PPV_PANGEA_PPV_CHECK_PERIOD);
	}

	return ERR_BUS_NO_ERROR;
}

// MSG_GWM_DESTROY
STATIC HERROR xmgr_cas_NaPpvBannerMsgDestroy_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents)
{


	return ERR_BUS_NO_ERROR;
}

//MSG_GWM_KEYDOWN
STATIC BUS_Result_t xmgr_cas_NaPpvBannerMsgKeyDown_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			nKeyId = p1;
	HINT32			nFocusID = 0;

	XMGR_CAS_NAPPV_PANGEA_POINTER_INVALID_CHECK(pstContents, ERR_FAIL);

	nFocusID = GWM_GetFocusedObjectID();

	AP_KeyConv_UiCommon(&nKeyId);

	switch (nKeyId)
	{
	case KEY_OK:
		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_OK_BTN_ID))
		{
			if (pstContents->nExtButtonType == eNA_PPV_COLOR_ICON_GREEN)
			{
				pstContents->nExtButtonType = eNA_PPV_COLOR_ICON_BLUE;
				GWM_APP_InvalidateObject(CAS_NA_PPV_PANGEA_COLOR_BTN_ID);

				pstContents->pszBtnColorHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_TERM_CONDITION_ID);

				xmgr_cas_NaPpvBannerUpdateSubItemsByColorBtnType_Pangea(pstContents);
			}
			else if (pstContents->nExtButtonType == eNA_PPV_COLOR_ICON_BLUE)
			{
				// IPPV or PPT purchase 실행
				pstContents->nExtButtonType = eNA_PPV_COLOR_ICON_DONE;
				GWM_APP_InvalidateObject(CAS_NA_PPV_PANGEA_COLOR_BTN_ID);

				pstContents->pszBtnColorHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_SHOW_INFORMATION_ID);

				if (xmgr_cas_NaRequestEventPurchase_Mediaset() == ERR_FAIL)
				{
					pstContents->pszConfirmStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_ERR_DAMAGED_CARD_DETAIL_ID);
					xmgr_cas_NaPpvBannerUpdateSubItemsByColorBtnType_Pangea(pstContents);
				}

				// IPPV confirm 단계에서 OK를 누를 경우 예약된 recharge/oppv 체크
				BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_POPUP_CLOSED, (Handle_t)0, eDxCAS_GROUPID_NA, (HINT32)0, (HINT32)0);
			}

			xmgr_cas_NaPpvBannerRemoveExtendWindow_Pangea(pstContents);

			xmgr_cas_NaPpvBannerUpdatePpvWindow_Pangea(pstContents);
		}
		return MESSAGE_BREAK;
	case KEY_BACK:
		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_BACK_BTN_ID))
		{
			if (pstContents->nExtMode == eNA_PPV_EXT_MODE_NONE)
			{
				if (pstContents->nExtButtonType == eNA_PPV_COLOR_ICON_BLUE)
				{
					pstContents->nExtButtonType = eNA_PPV_COLOR_ICON_GREEN;
					GWM_APP_InvalidateObject(CAS_NA_PPV_PANGEA_COLOR_BTN_ID);

					pstContents->pszBtnColorHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_SHOW_INFORMATION_ID);

					xmgr_cas_NaPpvBannerUpdateSubItemsByColorBtnType_Pangea(pstContents);

					// IPPV confirm 단계에서 BACK를 누를 경우 예약된 recharge/oppv 체크
					if (xmgr_cas_NaIsPopupReserved_Mediaset())
					{
						BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_POPUP_CLOSED, (Handle_t)0, eDxCAS_GROUPID_NA, (HINT32)0, (HINT32)0);
					}
				}
			}
			else if (pstContents->nExtMode == eNA_PPV_EXT_MODE_PPV_INFO)
			{
				xmgr_cas_NaPpvBannerRemoveExtendWindow_Pangea(pstContents);
				xmgr_cas_NaPpvBannerUpdateSubItemsByColorBtnType_Pangea(pstContents);
			}
			else		// eNA_PPV_EXT_MODE_PPV_PURCHASE_INFO
			{
				xmgr_cas_NaPpvBannerRemoveExtendWindow_Pangea(pstContents);
			}
			xmgr_cas_NaPpvBannerUpdatePpvWindow_Pangea(pstContents);
		}
		return MESSAGE_BREAK;
	case KEY_GREEN:
		if (pstContents->nExtButtonType != eNA_PPV_COLOR_ICON_BLUE)
		{
			if (pstContents->nExtMode == eNA_PPV_EXT_MODE_NONE)
			{
				pstContents->nExtMode = eNA_PPV_EXT_MODE_PPV_INFO;
				xmgr_cas_NaPpvBannerUpdateSubItemsByColorBtnType_Pangea(pstContents);
			}
			else if (pstContents->nExtMode == eNA_PPV_EXT_MODE_PPV_INFO)
			{
				xmgr_cas_NaPpvBannerRemoveExtendWindow_Pangea(pstContents);
				xmgr_cas_NaPpvBannerUpdateSubItemsByColorBtnType_Pangea(pstContents);
			}
			else
			{
				// 절대 이럴 순 없음 <== dykim
				HxLOG_Critical("\n\n");
			}
			xmgr_cas_NaPpvBannerUpdatePpvWindow_Pangea(pstContents);
		}
		return MESSAGE_BREAK;
	case KEY_BLUE:
		if (pstContents->nExtButtonType == eNA_PPV_COLOR_ICON_BLUE)
		{
			if (pstContents->nExtMode == eNA_PPV_EXT_MODE_NONE)
			{
				pstContents->nExtMode = eNA_PPV_EXT_MODE_PPV_PURCHASE_INFO;
			}
			else if (pstContents->nExtMode == eNA_PPV_EXT_MODE_PPV_PURCHASE_INFO)
			{
				xmgr_cas_NaPpvBannerRemoveExtendWindow_Pangea(pstContents);
			}
			else
			{
				// 절대 이럴 순 없음 <== dykim
				HxLOG_Critical("\n\n");
			}
			xmgr_cas_NaPpvBannerUpdatePpvWindow_Pangea(pstContents);
		}
		return MESSAGE_BREAK;

	case KEY_ARROWUP:
		if (pstContents->nExtMode != eNA_PPV_EXT_MODE_NONE)
		{
			GWM_FMT_ScrollUpPageStrict(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID);
		}
		return MESSAGE_BREAK;

	case KEY_ARROWDOWN:
		if (pstContents->nExtMode != eNA_PPV_EXT_MODE_NONE)
		{
			GWM_FMT_ScrollDownPageStrict(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID);
		}
		return MESSAGE_BREAK;

	case KEY_MENU:
	case KEY_GUIDE:
	case KEY_CHLIST:
	case KEY_OPT:
	case KEY_MYEPG:
		xmgr_cas_NaDisposeCaRequest(pstContents->pCaRequest);
		BUS_MGR_Destroy(NULL);
		break;

	case KEY_INFO:
		return MESSAGE_BREAK;

	default:
		break;
	}

	XMGR_CAS_NAPPV_PANGEA_PARAM_WARRING_REMOVE(pstContents, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

// MSG_GWM_TIMER:
STATIC BUS_Result_t xmgr_cas_NaPpvBannerMsgTimer_Pangea(XmgrCas_NaPpvContents_Pangea_t *pstContents, HINT32 nTimerId)
{
	UNIXTIME			nCurrTime = 0, nElapsedTime = 0;
	HERROR				hErr = ERR_OK;
	HUINT32				prevStringWidth = 0;
	OSD_Rect_t			stRect;
	HUINT8				tempStr[16];
	HxDATETIME_Time_t			stTime;

	switch (nTimerId)
	{
		case CAS_NA_PPV_PANGEA_PPV_CHECK_TIMER_ID:
			// kill timer 하지 않는다.. keep going..
			hErr = VK_CLOCK_GetTime(&nCurrTime);
			if (hErr != ERR_OK)
			{
				return ERR_FAIL;
			}

			if (pstContents == NULL)
			{
				return MESSAGE_BREAK;
			}

			if (nCurrTime > pstContents->nElapsedTime && nCurrTime < pstContents->nPurchaseEndTime)
			{
				nElapsedTime = nCurrTime - pstContents->nElapsedTime;
				HxLOG_Print("nElapsedTime = %d\n", nElapsedTime);

				if ( nElapsedTime >= 3600 )
				{
					snprintf((char *)tempStr, 16, "%dh %dmin", nElapsedTime / 3600, (nElapsedTime / 60) % 60);
				}
				else
				{
					snprintf((char *)tempStr, 16, "%d min", nElapsedTime / 60);
				}

				if (pstContents->pszEvtTimeStr != (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_ELAPSED_TIME_ID))
				{
					// start time title
					pstContents->pszEvtTimeStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_ELAPSED_TIME_ID);
					GWM_Text_SetText(CAS_NA_PPV_PANGEA_START_TIME_ID, pstContents->pszEvtTimeStr);

					if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_START_TIME_STR_ID))
					{
						GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_START_TIME_STR_ID);
					}

					// start time value
					MWC_UTIL_DvbStrncpy((HUINT8*)pstContents->szStartTime, tempStr, 16);
					xmgr_cas_NaPpvBannerGetStringWidth_Pangea(pstContents->pszEvtTimeStr, FONT_SIZE_XS, &prevStringWidth);
					OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_START_TIME_X+prevStringWidth+10, CAS_NA_PPV_PANGEA_START_TIME_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_START_TIME_W, CAS_NA_PPV_PANGEA_START_TIME_H);
					GWM_Text_Create (CAS_NA_PPV_PANGEA_START_TIME_STR_ID, &stRect, (HUINT8*)pstContents->szStartTime);
					GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_START_TIME_STR_ID, TEXT_ALIGN_LEFT);
					GWM_Text_SetSize (CAS_NA_PPV_PANGEA_START_TIME_STR_ID, FONT_SIZE_XS);
					GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_START_TIME_STR_ID, COL(CAS_NA_PPV_PANGEA_COLOR_GREEN), 0x00);
				}
				else
				{
					if (MWC_UTIL_DvbStrncmp(pstContents->szStartTime, tempStr, 16))
					{
						MWC_UTIL_DvbStrncpy((HUINT8*)pstContents->szStartTime, tempStr, 16);
						GWM_Text_SetText(CAS_NA_PPV_PANGEA_START_TIME_STR_ID, (HUINT8*)pstContents->szStartTime);
					}
				}
			}

			if (nCurrTime > pstContents->nPurchaseEndTime)
			{
				if (!pstContents->bNotPurchasable)
				{
					pstContents->bNotPurchasable = TRUE;
					HxLOG_Print("nCurrTime - pstContents->nPurchaseEndTime = %d\n", nCurrTime - pstContents->nPurchaseEndTime);

					// OK Button 제거
					if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_OK_BTN_ID))
					{
						GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_OK_BTN_ID);
						GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_OK_BTN_HELP_ID);
					}

					// BACK Button 제거
					if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_BACK_BTN_ID))
					{
						GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_BACK_BTN_ID);
						GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_BACK_BTN_HELP_ID);
					}

					// Selected background 제거
					if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_SELECTED_BACK_ID))
					{
						GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_SELECTED_BACK_ID);
					}

					// Seleted event title 제거
					if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_EVENT_SELECTED_ID))
					{
						GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_EVENT_SELECTED_ID);
					}

					// extend window remove
					if (pstContents->nExtMode != eNA_PPV_EXT_MODE_NONE)
					{
						xmgr_cas_NaPpvBannerRemoveExtendWindow_Pangea(pstContents);
					}

					// color button green으로
					if (pstContents->nExtButtonType != eNA_PPV_COLOR_ICON_GREEN)
					{
						pstContents->nExtButtonType = eNA_PPV_COLOR_ICON_GREEN;
						GWM_APP_InvalidateObject(CAS_NA_PPV_PANGEA_COLOR_BTN_ID);

						// IPPV confirm 단계에서 BACK를 누를 경우 예약된 recharge/oppv 체크
						BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_NOTIFY_PPV_POPUP_CLOSED, (Handle_t)0, eDxCAS_GROUPID_NA, (HINT32)0, (HINT32)0);
					}

					// color button help - show information으로
					if (pstContents->pszBtnColorHelpStr != (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_SHOW_INFORMATION_ID))
					{
						pstContents->pszBtnColorHelpStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_IPPV_SHOW_INFORMATION_ID);
						GWM_Text_SetText(CAS_NA_PPV_PANGEA_COLOR_BTN_HELP_ID, pstContents->pszBtnColorHelpStr);
					}

					// elapsed time을 start time으로 다시 표시
					pstContents->pszEvtTimeStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_IPPV_END_TIME_ID);
					GWM_Text_SetText(CAS_NA_PPV_PANGEA_START_TIME_ID, pstContents->pszEvtTimeStr);

					if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_START_TIME_STR_ID))
					{
						GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_START_TIME_STR_ID);
					}
					HLIB_DATETIME_ConvertUnixTimeToTime(pstContents->nPurchaseEndTime, &stTime);
					snprintf(pstContents->szStartTime, 16, "%02d:%02d", stTime.ucHour, stTime.ucMinute);
					xmgr_cas_NaPpvBannerGetStringWidth_Pangea(pstContents->pszEvtTimeStr, FONT_SIZE_XS, &prevStringWidth);
					OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_START_TIME_X+prevStringWidth+10, CAS_NA_PPV_PANGEA_START_TIME_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_START_TIME_W, CAS_NA_PPV_PANGEA_START_TIME_H);
					GWM_Text_Create (CAS_NA_PPV_PANGEA_START_TIME_STR_ID, &stRect, (HUINT8*)pstContents->szStartTime);
					GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_START_TIME_STR_ID, TEXT_ALIGN_LEFT);
					GWM_Text_SetSize (CAS_NA_PPV_PANGEA_START_TIME_STR_ID, FONT_SIZE_XS);
					GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_START_TIME_STR_ID, COL(CAS_NA_PPV_PANGEA_COLOR_GREEN), 0x00);

					// ok/back button 등 초기화
					xmgr_cas_NaPpvBannerUpdateSubItemsByColorBtnType_Pangea(pstContents);


					// Error Message create
					pstContents->pszConfirmStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_ERR_PURCASE_PERIOD_END_ID);
					if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID) == NULL)
					{
						OSD_Rect_t stRect;

						// Confirm message
						OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_X, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_Y - FONT_SIZE_XS, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_W, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_H);
						GWM_Text_Create (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, &stRect, pstContents->pszConfirmStr);
						GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, TEXT_ALIGN_LEFT);
						GWM_Text_SetSize (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, FONT_SIZE_XS);
						GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, COL(CAS_NA_PPV_PANGEA_COLOR_GREEN), 0x00);
					}
					else
					{
						GWM_Text_SetText(CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, pstContents->pszConfirmStr);
					}
				}
			}

			return MESSAGE_BREAK;

		default:
			break;
	}

	return MESSAGE_PASS;
}



/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_UI_PPV_PGEA_PUBLIC_FUNCTION______________________________________________________

HBOOL	xmgr_cas_NaPpvBannerCheckStepConfirm_Pangea(void)
{
	XmgrCas_NaPpvContents_Pangea_t		*pstContents = xmgr_cas_NaPpvBannerGetContents_Pangea();

	if (pstContents == NULL || pstContents->nExtButtonType != eNA_PPV_COLOR_ICON_BLUE)
	{
		return FALSE;
	}

	return TRUE;
}

BUS_Result_t	xproc_cas_NaPpvBanner_Pangea(HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t					eResult	= ERR_BUS_NO_ERROR;
	XmgrCas_NaPpvContents_Pangea_t		*pstContents = xmgr_cas_NaPpvBannerGetContents_Pangea();
	HERROR							hErr = ERR_OK;

	switch(message)
	{
	case MSG_GWM_CREATE :
		eResult = xmgr_cas_NaPpvBannerMsgCreated_Pangea(pstContents, p1, p2, p3);
		break;

	case MSG_GWM_KEYDOWN:
		eResult = xmgr_cas_NaPpvBannerMsgKeyDown_Pangea(pstContents, p1, p2, p3);
		if (eResult == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case MSG_GWM_DESTROY :
		if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_EXTEND_INFO_STR_ID))
		{
			GWM_APP_DisablePreventDraw();
		}
		break;

	case MSG_GWM_TIMER:
		HxLOG_Info("\t[MSG_GWM_TIMER]\n\n");
		hErr = xmgr_cas_NaPpvBannerMsgTimer_Pangea (pstContents, p1);	/* p1: HINT32 nTimerId */
		if (hErr == MESSAGE_BREAK)					{ return MESSAGE_BREAK; }
		break;

	case eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT:
		HxLOG_Print("eMEVT_XMGR_CAS_NA_NOTIFY_SMARTCARD_CREDIT...\n");
		if(p1 == eDxCAS_GROUPID_NA)
		{
			HUINT32		prevStringWidth = 0, currStringWidth = 0;
			OSD_Rect_t	stRect;

			xmgr_cas_NaPpvBannerGetStringWidth_Pangea((HUINT8*)pstContents->szRemainCredit, FONT_SIZE_XS, &prevStringWidth);
			snprintf(pstContents->szRemainCredit, 16, "%d,%02d", p3/100, p3%100);
			xmgr_cas_NaPpvBannerGetStringWidth_Pangea((HUINT8*)pstContents->szRemainCredit, FONT_SIZE_XS, &currStringWidth);
			GWM_Text_SetText(CAS_NA_PPV_PANGEA_REMAIN_CREDIT_STR_ID, (HUINT8*)pstContents->szRemainCredit);
			if (prevStringWidth != currStringWidth)
			{
				eResult = GWM_Obj_GetObjectRect(CAS_NA_PPV_PANGEA_CREDIT_EURO_ICON_ID, &stRect);
				if (eResult == ERR_BUS_NO_ERROR)
				{
					stRect.x += (currStringWidth - prevStringWidth);
					GWM_Obj_Resize(CAS_NA_PPV_PANGEA_CREDIT_EURO_ICON_ID, &stRect);
				}
			}

			xmgr_cas_NaGetScCredit(&pstContents->nCardCredit);
			if (pstContents->nCardCredit < pstContents->nEventCredit)	// credit 이 부족한 경우 OK 버튼 없이 에러 메시지 표시
			{
				if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_OK_BTN_ID))
				{
					GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_OK_BTN_ID);
					GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_OK_BTN_HELP_ID);
				}

				pstContents->pszConfirmStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_ERR_NOT_ENOUGH_CREDIT_ID);
				// Confirm message
				if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID) == NULL)
				{
					OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_X, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_Y, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_W, CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_H);
					GWM_Text_Create (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, &stRect, pstContents->pszConfirmStr);
					GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, TEXT_ALIGN_LEFT);
					GWM_Text_SetSize (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, FONT_SIZE_XS);
					GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID, COL(CAS_NA_PPV_PANGEA_COLOR_GREEN), 0x00);
				}
			}
			else
			{
				if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID))
				{
					if (pstContents->pszConfirmStr == GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_ERR_NOT_ENOUGH_CREDIT_ID))
					{
						GWM_Obj_Destroy(CAS_NA_PPV_PANGEA_CONFIRM_MESSAGE_ID);
					}
				}

				if (GWM_Obj_GetObjectByID(CAS_NA_PPV_PANGEA_OK_BTN_ID) == NULL)
				{
					// OK Button
					OSD_GFX_SetRect(&stRect, CAS_NA_PPV_PANGEA_OK_BTN_X, CAS_NA_PPV_PANGEA_OK_BTN_Y, CAS_NA_PPV_PANGEA_OK_BTN_W, CAS_NA_PPV_PANGEA_OK_BTN_H);
					GWM_Button_Create (CAS_NA_PPV_PANGEA_OK_BTN_ID, &stRect, NULL);
					GWM_APP_SetObjectDrawMethod (CAS_NA_PPV_PANGEA_OK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBannerDrawOKButton_Pangea);

						// OK Button help
					OSD_GFX_SetRect (&stRect, CAS_NA_PPV_PANGEA_OK_BTN_HELP_X, CAS_NA_PPV_PANGEA_OK_BTN_HELP_Y, CAS_NA_PPV_PANGEA_OK_BTN_HELP_W, CAS_NA_PPV_PANGEA_OK_BTN_HELP_H);
					GWM_Text_Create (CAS_NA_PPV_PANGEA_OK_BTN_HELP_ID, &stRect, pstContents->pszBtnOKHelpStr);
					GWM_Text_SetAlign (CAS_NA_PPV_PANGEA_OK_BTN_HELP_ID, TEXT_ALIGN_LEFT);
					GWM_Text_SetSize (CAS_NA_PPV_PANGEA_OK_BTN_HELP_ID, FONT_SIZE_XS);
					GWM_Text_ChangeColor (CAS_NA_PPV_PANGEA_OK_BTN_HELP_ID, COL(CAS_NA_PPV_PANGEA_COLOR_WHITE), 0x00);
				}
			}
		}
		break;

	case eMEVT_XMGR_CAS_NA_CLOSE_PPV_WND:
		HxLOG_Print("eMEVT_XMGR_CAS_NA_CLOSE_PPV_WND...\n");
		if(p1 == eDxCAS_GROUPID_NA)
		{
			BUS_MGR_Destroy(NULL);
		}
		return MESSAGE_BREAK;

	case eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_RESULT:
		HxLOG_Print("eMEVT_XMGR_CAS_NA_NOTIFY_PPV_PURCHASE_RESULT...status(%d), groupId(%d), purchase type(%d)\n", handle, p1, p2);
		if(p1 == eDxCAS_GROUPID_NA)
		{
			if ((PURCHASE_RESULT_TYPE)handle != eNA_PPV_PURCHASE_RESULT_TYPE_SUCCESS)	// purchase success
			{
				// close ppv window and display result window
				HxLOG_Print("don't close ppv window and display purchase error... hAction(%d)\n", handle);

				switch((PURCHASE_RESULT_TYPE)handle)
				{
					case CA_PROCESSING_ERROR:
						pstContents->pszConfirmStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_ERR_DAMAGED_CARD_DETAIL_ID);
						break;

					default:
						break;
				}

				xmgr_cas_NaPpvBannerUpdateSubItemsByColorBtnType_Pangea(pstContents);

				return MESSAGE_PASS;
			}
			else
			{
				xmgr_cas_NaDisposeCaRequest(pstContents->pCaRequest);
				BUS_MGR_Destroy(NULL);

				return MESSAGE_PASS;
			}
		}
		break;

	case MSG_APP_UPDATE_SMARTCARD_INFO:
		HxLOG_Print("MSG_APP_UPDATE_SMARTCARD_INFO\n");
		if(p1 == eDxCAS_GROUPID_NA)
		{
			XmgrCas_NaAlamType_e	eAlarmType;

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

			if (eAlarmType != eCAS_NA_CA_SMARTCARD_OK)
			{
				xmgr_cas_NaDisposeCaRequest(pstContents->pCaRequest);
				BUS_MGR_Destroy(NULL);

				return MESSAGE_PASS;
			}
		}
		break;

	default:
		break;
	}

	return BUS_ProcessMessageDefault(message, handle, p1, p2, p3);
}


#endif
