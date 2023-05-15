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

#include "../../local_include/_xmgr_cas_na.h"
#include "../../local_include/_xmgr_cas_na_util.h"
#include "../../local_include/_xmgr_cas_na_adapt.h"


#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
//******************************************************************************
//	DEFINE
//******************************************************************************

#define	MAX_MSET_SMARTCARD_ERROR_MSG_LENGTH				128

typedef struct tag_CAS_NA_MEDIASET_SCE_Contents
{
	HINT32		cardStatus;

	HUINT8		*pszBtnBackStr;

	HUINT8		szSmartCardErrorDateStr[32];
	HUINT8		*pszSmartCardErrorMsgText;
	HUINT8		*pszSmartCardErrorMsgDetailText;
	HUINT8		title[MAX_ELEMENT_TEXT_LENGTH];
} XmgrCas_NaCardError_Mediaset_t;

#define	NAPPV_MEDIASET_SCERROR_WINDOW_WIDTH						OSD_SCREEN_WIDTH
#define	NAPPV_MEDIASET_SCERROR_WINDOW_HEIGHT					OSD_SCREEN_HEIGHT
#define	NAPPV_MEDIASET_SCERROR_WINDOW_X							0
#define	NAPPV_MEDIASET_SCERROR_WINDOW_Y							0

#define	NAPPV_MEDIASET_SCERROR_APP_ID							0

/* ppv mediaset scerror background */
#define	NAPPV_MEDIASET_SCERROR_LOGOBACKGROUND_IMAGE_ID			(NAPPV_MEDIASET_SCERROR_APP_ID|0x0010)

#define	NAPPV_MEDIASET_SCERROR_MENUBACKGROUND_ID				(NAPPV_MEDIASET_SCERROR_APP_ID|0x0020)
#define	NAPPV_MEDIASET_SCERROR_MENUBACKGROUND_X					0
#define	NAPPV_MEDIASET_SCERROR_MENUBACKGROUND_Y					166
#define	NAPPV_MEDIASET_SCERROR_MENUBACKGROUND_WIDTH				1120
#define	NAPPV_MEDIASET_SCERROR_MENUBACKGROUND_HEIGHT			554

/* ppv mediaset scerror date */
#define	NAPPV_MEDIASET_SCERROR_DATE_TEXT_ID						(NAPPV_MEDIASET_SCERROR_APP_ID|0x0030)
#define	NAPPV_MEDIASET_SCERROR_DATE_TEXT_X						830
#define	NAPPV_MEDIASET_SCERROR_DATE_TEXT_Y						(122 - FONT_SIZE_S)
#define	NAPPV_MEDIASET_SCERROR_DATE_TEXT_WIDTH					300
#define	NAPPV_MEDIASET_SCERROR_DATE_TEXT_HEIGHT					25

/* ppv mediaset scerror msg */
#define	NAPPV_MEDIASET_SCERROR_MSG_TEXT_ID						(NAPPV_MEDIASET_SCERROR_APP_ID|0x0050)
#define	NAPPV_MEDIASET_SCERROR_MSG_TEXT_X						190
#define	NAPPV_MEDIASET_SCERROR_MSG_TEXT_Y						308
#define	NAPPV_MEDIASET_SCERROR_MSG_TEXT_WIDTH					900
#define	NAPPV_MEDIASET_SCERROR_MSG_TEXT_HEIGHT					(36 * 2)

/* ppv mediaset scerror msg */
#define	NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_ID				(NAPPV_MEDIASET_SCERROR_APP_ID|0x0060)
#define	NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_X				190
#define	NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_Y				308 + (36 * 3)
#define	NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_WIDTH			900
#define	NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_HEIGHT			(34 * 2)

/* ppv mediaset scinfo back button */
#define	NAPPV_MEDIASET_SCERROR_BACK_BTN_ID						(NAPPV_MEDIASET_SCERROR_APP_ID|0x0070)
#define	NAPPV_MEDIASET_SCERROR_BACK_BTN_X						1079
#define	NAPPV_MEDIASET_SCERROR_BACK_BTN_Y						611
#define	NAPPV_MEDIASET_SCERROR_BACK_BTN_WIDTH					72
#define	NAPPV_MEDIASET_SCERROR_BACK_BTN_HEIGHT					32

//******************************************************************************
//	STATIC VARIABLES
//******************************************************************************
STATIC XmgrCas_NaCardError_Mediaset_t 			s_stNaMsetCardError;


//******************************************************************************
//	PROTOTYPE FUNCTIONS
//******************************************************************************
#define ________XMGR_CAS_NA_PPV_MEDIASET_MENU_STATIC___________________________________________
STATIC XmgrCas_NaCardError_Mediaset_t* xmgr_cas_NaPpvScErrorGetLocalContents_Mediaset(void);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorSetText_Mediaset(XmgrCas_NaCardError_Mediaset_t *pstContents, HINT32 cardStatus);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawSmartCardMsg_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawSmartCardMsgDetail_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawBackButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawDafaultUi_Mediaset(XmgrCas_NaCardError_Mediaset_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDestroyUI_Mediaset(XmgrCas_NaCardError_Mediaset_t *pContents);

#define ________XMGR_CAS_NA_PPV_MEDIASET_MENU_PUBLIC___________________________________________
BUS_Result_t xproc_cas_NaPpvScError_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#define	_________________________STATIC_BODY______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvScErrorGetLocalContents_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaCardError_Mediaset_t* xmgr_cas_NaPpvScErrorGetLocalContents_Mediaset(void)
{
	return &s_stNaMsetCardError;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScErrorSetText_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorSetText_Mediaset(XmgrCas_NaCardError_Mediaset_t *pstContents, HINT32 cardStatus)
{
	HUINT8	szDate[32];

	HxSTD_memset(szDate, 0, 32);
	HxSTD_memset(pstContents, 0, sizeof(XmgrCas_NaCardError_Mediaset_t));

	HxLOG_Print("cardStatus = %d\n", cardStatus);

	if (cardStatus != eCAS_NA_CA_SMARTCARD_REMOVED && cardStatus != eCAS_NA_CA_SMARTCARD_MUTE && cardStatus != eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
	{
		if (!xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(eNAGRA_OP_TYPE_MEDIASET))
		{
			HxLOG_Print("xmgr_cas_NaMatchCardStatusWithOpType_Mediaset FALSE!!!\n");
			cardStatus = eCAS_NA_CA_SMARTCARD_INVALID;
		}
	}

	/*status error msg*/
	pstContents->cardStatus = cardStatus;
	if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_REMOVED)
	{   // not inserted
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_MENU_ERR_INSERT_CARD_ID);
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_MENU_ERR_INSERT_CARD_DETAIL_ID);
	}
	else if(cardStatus == eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED
		|| cardStatus == eCAS_NA_CA_SMARTCARD_SUSPENDED)
	{	// blocked
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_MENU_ERR_BLOCKED_CARD_ID);
	}
	else if(cardStatus == eCAS_NA_CA_SMARTCARD_INVALID
		|| cardStatus == eCAS_NA_CA_SMARTCARD_COM_ERROR
		|| cardStatus == eCAS_NA_CA_SMARTCARD_MUTE)
	{	// damaged
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_MENU_ERR_DAMAGED_CARD_ID);
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_MENU_ERR_DAMAGED_CARD_DETAIL_ID);
	}
	else if (cardStatus == eCAS_NA_CA_SMARTCARD_BLACKLISTED)
	{	// blacklisted
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_MENU_ERR_PPV_BLACKLIST_CARD_ID);
	}

	xmgr_cas_NaGetCurrentDateStr(szDate);
	snprintf(pstContents->szSmartCardErrorDateStr, 32, "%s: %s", GetStrRsc(LOC_CAS_NAGRA_MEDIASET_DATE_TEXT_ID), szDate);

	pstContents->pszBtnBackStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BACK_MENU_TEXT_ID);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScErrorDrawBackgroundImage_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
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
 Function		: xmgr_cas_NaPpvScErrorDrawBackButton_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawBackButton_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvScErrorDrawDafaultUi_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawDafaultUi_Mediaset(XmgrCas_NaCardError_Mediaset_t *pContents)
{
	OSD_Rect_t				rect = {0, };

	/* for compiler */
	(void)pContents;

	MWC_UTIL_DvbStrcpy(pContents->title ,(HUINT8 *)GetStrRsc(LOC_CAS_MEDIASET_PREMIUM_TEXT_ID));

	/* na ppv mediaset scerror background */
	Settings_DrawBack(FALSE);
	OSD_GFX_SetRect(&rect, 0, 0, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT);
	GWM_Rect_Create(NAPPV_MEDIASET_SCERROR_LOGOBACKGROUND_IMAGE_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_SCERROR_LOGOBACKGROUND_IMAGE_ID, xmgr_cas_NaPpvScErrorDrawBackgroundImage_Mediaset);

	/* na ppv mediaset scerror date text*/
	OSD_GFX_SetRect (&rect, NAPPV_MEDIASET_SCERROR_DATE_TEXT_X, NAPPV_MEDIASET_SCERROR_DATE_TEXT_Y, NAPPV_MEDIASET_SCERROR_DATE_TEXT_WIDTH, NAPPV_MEDIASET_SCERROR_DATE_TEXT_HEIGHT);
	GWM_Text_Create(NAPPV_MEDIASET_SCERROR_DATE_TEXT_ID, &rect, pContents->szSmartCardErrorDateStr);
	GWM_Text_SetAlign (NAPPV_MEDIASET_SCERROR_DATE_TEXT_ID, TEXT_ALIGN_RIGHT);
	GWM_Text_SetSize (NAPPV_MEDIASET_SCERROR_DATE_TEXT_ID, FONT_SIZE_S);
	GWM_Text_ChangeColor (NAPPV_MEDIASET_SCERROR_DATE_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv mediaset scerror card error text*/
	if (pContents->pszSmartCardErrorMsgDetailText)
	{
		OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_SCERROR_MSG_TEXT_X, NAPPV_MEDIASET_SCERROR_MSG_TEXT_Y - FONT_SIZE_L, NAPPV_MEDIASET_SCERROR_MSG_TEXT_WIDTH, NAPPV_MEDIASET_SCERROR_MSG_TEXT_HEIGHT);
	}
	else
	{
		OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_SCERROR_MSG_TEXT_X, NAPPV_MEDIASET_SCERROR_MSG_TEXT_Y + FONT_SIZE_L, NAPPV_MEDIASET_SCERROR_MSG_TEXT_WIDTH, NAPPV_MEDIASET_SCERROR_MSG_TEXT_HEIGHT);
	}
	GWM_Text_Create (NAPPV_MEDIASET_SCERROR_MSG_TEXT_ID, &rect, pContents->pszSmartCardErrorMsgText);
	GWM_Text_SetAlign (NAPPV_MEDIASET_SCERROR_MSG_TEXT_ID, TEXT_ALIGN_CENTER | TEXT_MULTILINED);
	GWM_Text_SetSize (NAPPV_MEDIASET_SCERROR_MSG_TEXT_ID, FONT_SIZE_L);
	GWM_Text_ChangeColor (NAPPV_MEDIASET_SCERROR_MSG_TEXT_ID, COL(C_T_MList_Yellow), 0x00);

	/* na ppv mediaset scerror card error detail text*/
	OSD_GFX_SetRect (&rect, NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_X, NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_Y - FONT_SIZE_M, NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_WIDTH, NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_ID, &rect, pContents->pszSmartCardErrorMsgDetailText);
	GWM_Text_SetAlign (NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_ID, TEXT_ALIGN_CENTER | TEXT_MULTILINED);
	GWM_Text_SetSize (NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor (NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv mediaset scerror back button */
	OSD_GFX_SetRect(&rect, NAPPV_MEDIASET_SCERROR_BACK_BTN_X, NAPPV_MEDIASET_SCERROR_BACK_BTN_Y, NAPPV_MEDIASET_SCERROR_BACK_BTN_WIDTH, NAPPV_MEDIASET_SCERROR_BACK_BTN_HEIGHT);
	GWM_Button_Create(NAPPV_MEDIASET_SCERROR_BACK_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_SCERROR_BACK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvScErrorDrawBackButton_Mediaset);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScErrorDestroyUI_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDestroyUI_Mediaset(XmgrCas_NaCardError_Mediaset_t *pContents)
{

	return ERR_BUS_NO_ERROR;
}

#define	_________________________PUBLIC_BODY______________________________
/******************************************************************************/
/**
*   \fn     xproc_cas_NaPpvScError_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaPpvScError_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaCardError_Mediaset_t			*pstContents = xmgr_cas_NaPpvScErrorGetLocalContents_Mediaset();
	HUINT32							nResult = ERR_FAIL;
	OSD_Rect_t						stRect;

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_MEDIASET_SCERROR_WINDOW_X, NAPPV_MEDIASET_SCERROR_WINDOW_Y,
								NAPPV_MEDIASET_SCERROR_WINDOW_WIDTH, NAPPV_MEDIASET_SCERROR_WINDOW_HEIGHT);

			// Init the contents
			xmgr_cas_NaPpvScErrorSetText_Mediaset(pstContents, p1);

			// draw UI object
			xmgr_cas_NaPpvScErrorDrawDafaultUi_Mediaset(pstContents);

			/* Display Panel - PanelÀº Only English? */
			//AP_SPEC_DisplayMenuPanelString(pstContents->title, eDxPANEL_ALIGN_CENTER);
			break;

		case MSG_APP_UPDATE_SMARTCARD_INFO:
			if(p1 == eDxCAS_GROUPID_NA)
			{
				XmgrCas_NaAlamType_e	eAlarmType;

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

				if (eAlarmType == eCAS_NA_CA_SMARTCARD_OK || eAlarmType == eCAS_NA_CA_SMARTCARD_EXPIRED)
				{
					BUS_MGR_Destroy(NULL);

					/* to upper depth */
					BUS_MGR_Create("xproc_cas_NaPpvMenu_Mediaset", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xproc_cas_NaPpvMenu_Mediaset, 0, 0, 0, 0);

					return MESSAGE_PASS;
				}
				else
				{
					xmgr_cas_NaPpvScErrorSetText_Mediaset(pstContents, eAlarmType);

					if (pstContents->pszSmartCardErrorMsgDetailText)
					{
						OSD_GFX_SetRect(&stRect, NAPPV_MEDIASET_SCERROR_MSG_TEXT_X, NAPPV_MEDIASET_SCERROR_MSG_TEXT_Y - FONT_SIZE_L, NAPPV_MEDIASET_SCERROR_MSG_TEXT_WIDTH, NAPPV_MEDIASET_SCERROR_MSG_TEXT_HEIGHT);
					}
					else
					{
						OSD_GFX_SetRect(&stRect, NAPPV_MEDIASET_SCERROR_MSG_TEXT_X, NAPPV_MEDIASET_SCERROR_MSG_TEXT_Y + FONT_SIZE_L, NAPPV_MEDIASET_SCERROR_MSG_TEXT_WIDTH, NAPPV_MEDIASET_SCERROR_MSG_TEXT_HEIGHT);
					}
					GWM_Obj_Resize(NAPPV_MEDIASET_SCERROR_MSG_TEXT_ID, &stRect);

					GWM_Text_SetText(NAPPV_MEDIASET_SCERROR_MSG_TEXT_ID, pstContents->pszSmartCardErrorMsgText);
					GWM_Text_SetText(NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_ID, pstContents->pszSmartCardErrorMsgDetailText);
				}
			}
			break;

		case eSEVT_CAS_SC_INSERTED:
		case eSEVT_CAS_SC_REMOVED:
			{
				XmgrCas_NaAlamType_e	eAlarmType;

				xmgr_cas_NaGetSmartCardStatus(&eAlarmType);
				HxLOG_Print("eSEVT_CAS_SC_INSERTED or eSEVT_CAS_SC_REMOVED... eAlarmType = %d, p1 = 0x%x\n", eAlarmType, p1);

				if (eAlarmType == eCAS_NA_CA_SMARTCARD_INVALID || eAlarmType == eCAS_NA_CA_SMARTCARD_REMOVED)
				{
					xmgr_cas_NaPpvScErrorSetText_Mediaset(pstContents, eAlarmType);
					HxLOG_Print("%s %s\n", pstContents->pszSmartCardErrorMsgText, pstContents->pszSmartCardErrorMsgDetailText);

					if (pstContents->pszSmartCardErrorMsgDetailText)
					{
						OSD_GFX_SetRect(&stRect, NAPPV_MEDIASET_SCERROR_MSG_TEXT_X, NAPPV_MEDIASET_SCERROR_MSG_TEXT_Y - FONT_SIZE_L, NAPPV_MEDIASET_SCERROR_MSG_TEXT_WIDTH, NAPPV_MEDIASET_SCERROR_MSG_TEXT_HEIGHT);
					}
					else
					{
						OSD_GFX_SetRect(&stRect, NAPPV_MEDIASET_SCERROR_MSG_TEXT_X, NAPPV_MEDIASET_SCERROR_MSG_TEXT_Y + FONT_SIZE_L, NAPPV_MEDIASET_SCERROR_MSG_TEXT_WIDTH, NAPPV_MEDIASET_SCERROR_MSG_TEXT_HEIGHT);
					}
					GWM_Obj_Resize(NAPPV_MEDIASET_SCERROR_MSG_TEXT_ID, &stRect);

					GWM_Text_SetText(NAPPV_MEDIASET_SCERROR_MSG_TEXT_ID, pstContents->pszSmartCardErrorMsgText);
					GWM_Text_SetText(NAPPV_MEDIASET_SCERROR_MSG_DETAIL_TEXT_ID, pstContents->pszSmartCardErrorMsgDetailText);
				}
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
					BUS_MGR_Destroy(NULL);

					/* to upper depth */
					BUS_MGR_Create("AP_CAS_PpvMenu_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)AP_CAS_PpvMenu_Proc, 0, 0, 0, 0);
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
			xmgr_cas_NaPpvScErrorDestroyUI_Mediaset(pstContents);

			/* clear front PANEL */
			//AP_SPEC_ClearMenuPanelString();
			break;

		default:
			break;
	}

	HxLOG_Info("xproc_cas_NaPpvScError_Mediaset handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif	//end of #if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
/* End of File */
