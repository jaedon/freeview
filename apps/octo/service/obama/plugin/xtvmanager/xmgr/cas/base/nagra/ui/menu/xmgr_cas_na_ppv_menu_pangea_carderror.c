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




#define	MAX_PGEA_SMARTCARD_ERROR_MSG_LENGTH				128

typedef struct tag_CAS_NA_PANGEA_SCE_Contents
{
	HINT32		cardStatus;

	HUINT8		*pszBtnBackStr;

	HUINT8		szSmartCardErrorDateStr[32];
	HUINT8		*pszSmartCardErrorMsgText;
	HUINT8		*pszSmartCardErrorMsgDetailText;
	HUINT8		title[MAX_ELEMENT_TEXT_LENGTH];
} XmgrCas_NaCardError_Pangea_t;

#define	NAPPV_PANGEA_SCERROR_WINDOW_WIDTH						OSD_SCREEN_WIDTH
#define	NAPPV_PANGEA_SCERROR_WINDOW_HEIGHT						OSD_SCREEN_HEIGHT
#define	NAPPV_PANGEA_SCERROR_WINDOW_X							0
#define	NAPPV_PANGEA_SCERROR_WINDOW_Y							0

#define	NAPPV_PANGEA_SCERROR_APP_ID								0

/* ppv pangea scerror background */
#define	NAPPV_PANGEA_SCERROR_LOGOBACKGROUND_IMAGE_ID			(NAPPV_PANGEA_SCERROR_APP_ID|0x0010)

/* ppv pangea scerror date */
#define	NAPPV_PANGEA_SCERROR_DATE_TEXT_ID						(NAPPV_PANGEA_SCERROR_APP_ID|0x0030)
#define	NAPPV_PANGEA_SCERROR_DATE_TEXT_X						780
#define	NAPPV_PANGEA_SCERROR_DATE_TEXT_Y						196
#define	NAPPV_PANGEA_SCERROR_DATE_TEXT_WIDTH					300
#define	NAPPV_PANGEA_SCERROR_DATE_TEXT_HEIGHT					31

/* ppv pangea scerror msg */
#define	NAPPV_PANGEA_SCERROR_MSG_TEXT_ID						(NAPPV_PANGEA_SCERROR_APP_ID|0x0050)
#define	NAPPV_PANGEA_SCERROR_MSG_TEXT_X							190
#define	NAPPV_PANGEA_SCERROR_MSG_TEXT_Y							341
#define	NAPPV_PANGEA_SCERROR_MSG_TEXT_WIDTH						780
#define	NAPPV_PANGEA_SCERROR_MSG_TEXT_HEIGHT					(34 * 2)

/* ppv pangea scerror msg */
#define	NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_ID					(NAPPV_PANGEA_SCERROR_APP_ID|0x0060)
#define	NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_X					190
#define	NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_Y					417
#define	NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_WIDTH				780
#define	NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_HEIGHT				(34 * 2)

/* ppv pangea scinfo back button */
#define	NAPPV_PANGEA_SCERROR_BACK_BTN_ID						(NAPPV_PANGEA_SCERROR_APP_ID|0x0070)
#define	NAPPV_PANGEA_SCERROR_BACK_BTN_X							450
#define	NAPPV_PANGEA_SCERROR_BACK_BTN_Y							607
#define	NAPPV_PANGEA_SCERROR_BACK_BTN_WIDTH						86
#define	NAPPV_PANGEA_SCERROR_BACK_BTN_HEIGHT					44

#define	NAPPV_PANGEA_SCERROR_BACK_TEXT_ID						(NAPPV_PANGEA_SCERROR_APP_ID|0x0080)
#define	NAPPV_PANGEA_SCERROR_BACK_TEXT_X						543
#define	NAPPV_PANGEA_SCERROR_BACK_TEXT_Y						636
#define	NAPPV_PANGEA_SCERROR_BACK_TEXT_WIDTH					200
#define	NAPPV_PANGEA_SCERROR_BACK_TEXT_HEIGHT					44

//******************************************************************************
//	STATIC VARIABLES
//******************************************************************************
STATIC XmgrCas_NaCardError_Pangea_t 			s_stNaPgeaCardError;


//******************************************************************************
//	PROTOTYPE FUNCTIONS
//******************************************************************************
#define ________XMGR_CAS_NA_PpvPANGEA_MENU_STATIC___________________________________________
STATIC XmgrCas_NaCardError_Pangea_t* xmgr_cas_NaPpvScErrorGetLocalContents_Pangea(void);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorSetText_Pangea(XmgrCas_NaCardError_Pangea_t *pstContents, HINT32 cardStatus);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawMenuBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawMsgBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawSmartCardMsg_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawSmartCardMsgDetail_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawBackButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawDafaultUi_Pangea(XmgrCas_NaCardError_Pangea_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDestroyUI_Pangea(XmgrCas_NaCardError_Pangea_t *pContents);

#define ________XMGR_CAS_NA_PpvPANGEA_MENU_PUBLIC___________________________________________
BUS_Result_t xrpoc_cas_NaMenuScError_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#define	_________________________STATIC_BODY______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvScErrorGetLocalContents_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaCardError_Pangea_t* xmgr_cas_NaPpvScErrorGetLocalContents_Pangea(void)
{
	return &s_stNaPgeaCardError;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScErrorSetText_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorSetText_Pangea(XmgrCas_NaCardError_Pangea_t *pstContents, HINT32 cardStatus)
{
	HUINT8	szDate[32];

	HxSTD_memset(szDate, 0, 32);
	HxSTD_memset(pstContents, 0, sizeof(XmgrCas_NaCardError_Pangea_t));

	HxLOG_Print("cardStatus = %d\n", cardStatus);

	if (cardStatus != eCAS_NA_CA_SMARTCARD_REMOVED && cardStatus != eCAS_NA_CA_SMARTCARD_MUTE && cardStatus != eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
	{
		if (!xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(eNAGRA_OP_TYPE_PANGEA))
		{
			HxLOG_Print("xmgr_cas_NaMatchCardStatusWithOpType_Mediaset FALSE!!!\n");
			cardStatus = eCAS_NA_CA_SMARTCARD_INVALID;
		}
	}

	/*status error msg*/
	pstContents->cardStatus = cardStatus;
	if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_REMOVED)
	{   // not inserted
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_MENU_ERR_INSERT_CARD_ID);
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_MENU_ERR_INSERT_CARD_DETAIL_ID);
	}
	else if(cardStatus == eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED
		|| cardStatus == eCAS_NA_CA_SMARTCARD_SUSPENDED)
	{	// blocked
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_MENU_ERR_BLOCKED_CARD_ID);
	}
	else if(cardStatus == eCAS_NA_CA_SMARTCARD_INVALID
		|| cardStatus == eCAS_NA_CA_SMARTCARD_COM_ERROR
		|| cardStatus == eCAS_NA_CA_SMARTCARD_MUTE)
	{	// damaged
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_MENU_ERR_DAMAGED_CARD_ID);
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_MENU_ERR_DAMAGED_CARD_DETAIL_ID);
	}
	else if (cardStatus == eCAS_NA_CA_SMARTCARD_BLACKLISTED)
	{	// blacklisted
		pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_MENU_ERR_BLACKLIST_CARD_ID);
	}

	xmgr_cas_NaGetCurrentDateStr(szDate);
	snprintf(pstContents->szSmartCardErrorDateStr, 32, "%s : %s", GetStrRsc(LOC_CAS_NAGRA_PANGEA_DATE_TEXT_ID), szDate);

	pstContents->pszBtnBackStr = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BACK_MENU_TEXT_ID);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScErrorDrawBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea,
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
 Function		: xmgr_cas_NaPpvScErrorDrawMenuBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawMenuBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvScErrorDrawMsgBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawMsgBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e913_3_24_WIN2_181, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScErrorDrawBackButton_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawBackButton_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
 Function		: xmgr_cas_NaPpvScErrorDrawDafaultUi_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDrawDafaultUi_Pangea(XmgrCas_NaCardError_Pangea_t *pstContents)
{
	OSD_Rect_t				rect = {0, };

	/* for compiler */
	(void)pstContents;

	MWC_UTIL_DvbStrcpy(pstContents->title ,(HUINT8 *)GetStrRsc(LOC_CAS_PANGEA_TEXT_ID));

	/* na ppv Pangea scerror background */
	Settings_DrawBack(FALSE);
	OSD_GFX_SetRect(&rect, 0, 0, OSD_SCREEN_WIDTH, OSD_SCREEN_HEIGHT);
	GWM_Rect_Create(NAPPV_PANGEA_SCERROR_LOGOBACKGROUND_IMAGE_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_SCERROR_LOGOBACKGROUND_IMAGE_ID, xmgr_cas_NaPpvScErrorDrawBackgroundImage_Pangea);

	/* na ppv Pangea scerror date*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCERROR_DATE_TEXT_X, NAPPV_PANGEA_SCERROR_DATE_TEXT_Y-FONT_SIZE_XS, NAPPV_PANGEA_SCERROR_DATE_TEXT_WIDTH, NAPPV_PANGEA_SCERROR_DATE_TEXT_HEIGHT);
	GWM_Text_Create(NAPPV_PANGEA_SCERROR_DATE_TEXT_ID, &rect, pstContents->szSmartCardErrorDateStr);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCERROR_DATE_TEXT_ID, TEXT_ALIGN_RIGHT);
	GWM_Text_SetSize (NAPPV_PANGEA_SCERROR_DATE_TEXT_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor (NAPPV_PANGEA_SCERROR_DATE_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv Pangea scerror card error text*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCERROR_MSG_TEXT_X, NAPPV_PANGEA_SCERROR_MSG_TEXT_Y - FONT_SIZE_M, NAPPV_PANGEA_SCERROR_MSG_TEXT_WIDTH, NAPPV_PANGEA_SCERROR_MSG_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_SCERROR_MSG_TEXT_ID, &rect, pstContents->pszSmartCardErrorMsgText);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCERROR_MSG_TEXT_ID, TEXT_ALIGN_CENTER | TEXT_MULTILINED);
	GWM_Text_SetSize (NAPPV_PANGEA_SCERROR_MSG_TEXT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor (NAPPV_PANGEA_SCERROR_MSG_TEXT_ID, COL(C_T_MList_Blue02), 0x00);

	/* na ppv Pangea scerror card error detail text*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_X, NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_Y - FONT_SIZE_M, NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_WIDTH, NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_ID, &rect, pstContents->pszSmartCardErrorMsgDetailText);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_ID, TEXT_ALIGN_CENTER | TEXT_MULTILINED);
	GWM_Text_SetSize (NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor (NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_ID, COL(C_T_MList_Blue02), 0x00);

	/* na ppv Pangea scerror back button */
	OSD_GFX_SetRect(&rect, NAPPV_PANGEA_SCERROR_BACK_BTN_X, NAPPV_PANGEA_SCERROR_BACK_BTN_Y, NAPPV_PANGEA_SCERROR_BACK_BTN_WIDTH, NAPPV_PANGEA_SCERROR_BACK_BTN_HEIGHT);
	GWM_Button_Create(NAPPV_PANGEA_SCERROR_BACK_BTN_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_SCERROR_BACK_BTN_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvScErrorDrawBackButton_Pangea);

	/* na ppv Pangea scerror back button text*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_SCERROR_BACK_TEXT_X, NAPPV_PANGEA_SCERROR_BACK_TEXT_Y - FONT_SIZE_XS, NAPPV_PANGEA_SCERROR_BACK_TEXT_WIDTH, NAPPV_PANGEA_SCERROR_BACK_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_SCERROR_BACK_TEXT_ID, &rect, pstContents->pszBtnBackStr);
	GWM_Text_SetAlign (NAPPV_PANGEA_SCERROR_BACK_TEXT_ID, TEXT_ALIGN_LEFT);
	GWM_Text_SetSize (NAPPV_PANGEA_SCERROR_BACK_TEXT_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor(NAPPV_PANGEA_SCERROR_BACK_TEXT_ID, COL (C_T_Cursor), 0x00);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvScErrorDestroyUI_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvScErrorDestroyUI_Pangea(XmgrCas_NaCardError_Pangea_t *pContents)
{

	return ERR_BUS_NO_ERROR;
}

#define	_________________________PUBLIC_BODY______________________________
/******************************************************************************/
/**
*   \fn     xrpoc_cas_NaMenuScError_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xrpoc_cas_NaMenuScError_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaCardError_Pangea_t			*pstContents = xmgr_cas_NaPpvScErrorGetLocalContents_Pangea();
	HUINT32							nResult = ERR_FAIL;

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_PANGEA_SCERROR_WINDOW_X, NAPPV_PANGEA_SCERROR_WINDOW_Y,
								NAPPV_PANGEA_SCERROR_WINDOW_WIDTH, NAPPV_PANGEA_SCERROR_WINDOW_HEIGHT);

			// Init the contents
			xmgr_cas_NaPpvScErrorSetText_Pangea(pstContents, p1);

			// draw UI object
			xmgr_cas_NaPpvScErrorDrawDafaultUi_Pangea(pstContents);

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
					if (!xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(eNAGRA_OP_TYPE_PANGEA))
					{
						HxLOG_Print("xmgr_cas_NaMatchCardStatusWithOpType_Mediaset FALSE!!!\n");
						eAlarmType = eCAS_NA_CA_SMARTCARD_INVALID;
					}
				}

				if (eAlarmType == eCAS_NA_CA_SMARTCARD_OK || eAlarmType == eCAS_NA_CA_SMARTCARD_EXPIRED)
				{
					BUS_MGR_Destroy(NULL);

					/* to upper depth */
					BUS_MGR_Create("xmgr_cas_NaPpvMenu_Pangea_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)xmgr_cas_NaPpvMenu_Pangea_Proc, 0, 0, 0, 0);

					return MESSAGE_PASS;
				}
				else
				{
					xmgr_cas_NaPpvScErrorSetText_Pangea(pstContents, eAlarmType);
					GWM_Text_SetText(NAPPV_PANGEA_SCERROR_MSG_TEXT_ID, pstContents->pszSmartCardErrorMsgText);
					GWM_Text_SetText(NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_ID, pstContents->pszSmartCardErrorMsgDetailText);
				}
			}
			break;

		case eSEVT_CAS_SC_INSERTED:
		case eSEVT_CAS_SC_REMOVED:
			{
				XmgrCas_NaAlamType_e	eAlarmType;
				OSD_Rect_t			stRect;

				xmgr_cas_NaGetSmartCardStatus(&eAlarmType);
				HxLOG_Print("eSEVT_CAS_SC_INSERTED or eSEVT_CAS_SC_REMOVED... eAlarmType = %d, p1 = 0x%x\n", eAlarmType, p1);

				if (eAlarmType == eCAS_NA_CA_SMARTCARD_INVALID || eAlarmType == eCAS_NA_CA_SMARTCARD_REMOVED)
				{
					xmgr_cas_NaPpvScErrorSetText_Pangea(pstContents, eAlarmType);
					HxLOG_Print("%s %s\n", pstContents->pszSmartCardErrorMsgText, pstContents->pszSmartCardErrorMsgDetailText);

					if (pstContents->pszSmartCardErrorMsgDetailText)
					{
						OSD_GFX_SetRect(&stRect, NAPPV_PANGEA_SCERROR_MSG_TEXT_X, NAPPV_PANGEA_SCERROR_MSG_TEXT_Y - FONT_SIZE_L, NAPPV_PANGEA_SCERROR_MSG_TEXT_WIDTH, NAPPV_PANGEA_SCERROR_MSG_TEXT_HEIGHT);
					}
					else
					{
						OSD_GFX_SetRect(&stRect, NAPPV_PANGEA_SCERROR_MSG_TEXT_X, NAPPV_PANGEA_SCERROR_MSG_TEXT_Y + FONT_SIZE_L, NAPPV_PANGEA_SCERROR_MSG_TEXT_WIDTH, NAPPV_PANGEA_SCERROR_MSG_TEXT_HEIGHT);
					}
					GWM_Obj_Resize(NAPPV_PANGEA_SCERROR_MSG_TEXT_ID, &stRect);

					GWM_Text_SetText(NAPPV_PANGEA_SCERROR_MSG_TEXT_ID, pstContents->pszSmartCardErrorMsgText);
					GWM_Text_SetText(NAPPV_PANGEA_SCERROR_MSG_DETAIL_TEXT_ID, pstContents->pszSmartCardErrorMsgDetailText);
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
					BUS_MGR_Destroy(NULL);

					/* to upper depth */
					BUS_MGR_Create("AP_CAS_PpvMenu_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU), (BUS_Callback_t)AP_CAS_PpvMenu_Proc, 0, 0, 0, 0);
					return MESSAGE_BREAK;

				case KEY_OK:
				case KEY_ARROWUP:
				case KEY_ARROWDOWN:
					return MESSAGE_BREAK;

			}
			break;


		case MSG_GWM_TIMER:
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			xmgr_cas_NaPpvScErrorDestroyUI_Pangea(pstContents);

			/* clear front PANEL */
			//AP_SPEC_ClearMenuPanelString();
			break;

		default:
			break;
	}

	HxLOG_Info("xrpoc_cas_NaMenuScError_Pangea handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}
#endif	//end of #if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
/* End of File */
