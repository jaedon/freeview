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

#include "../../local_include/_xmgr_cas_na.h"
#include "../../local_include/_xmgr_cas_na_util.h"
#include "../../local_include/_xmgr_cas_na_adapt.h"



#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
//******************************************************************************
//	DEFINE
//******************************************************************************




#define	MAX_MSET_BAN_SMARTCARD_ERROR_MSG_LENGTH				128

typedef struct tag_CAS_NA_MEDIASET_BAN_SCE_Contents
{
	HUINT8		bDisplayMode;
	HINT32		cardStatus;
	HUINT8		*pszSmartCardErrorMsgText;
	HUINT8		*pszSmartCardErrorMsgDetailText;
} XmgrCas_NaBanCardError_Mediaset_t;

#define	NAPPV_MEDIASET_BAN_SCERROR_WINDOW_WIDTH						1280//OSD_SCREEN_WIDTH
#define	NAPPV_MEDIASET_BAN_SCERROR_WINDOW_HEIGHT					260//OSD_SCREEN_HEIGHT
#define	NAPPV_MEDIASET_BAN_SCERROR_WINDOW_X							0
#define	NAPPV_MEDIASET_BAN_SCERROR_WINDOW_Y							117//0

#define	NAPPV_MEDIASET_BAN_SCERROR_APP_ID							0

/* ppv mediaset scerror msg background*/
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_BACKGROUND_ID				(NAPPV_MEDIASET_BAN_SCERROR_APP_ID|0x0010)
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_BACKGROUND_X					0
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_BACKGROUND_Y					(117-117)
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_BACKGROUND_WIDTH				1280
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_BACKGROUND_HEIGHT			260

/* ppv mediaset scerror msg */
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_ID						(NAPPV_MEDIASET_BAN_SCERROR_APP_ID|0x0020)
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_X						190
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_Y						(257-117)
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_WIDTH					900
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_HEIGHT					34

/* ppv mediaset scerror msg */
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_ID				(NAPPV_MEDIASET_BAN_SCERROR_APP_ID|0x0030)
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_X				190
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_Y				(313-117)
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_WIDTH			900
#define	NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_HEIGHT			(34 * 2)


//******************************************************************************
//	STATIC VARIABLES
//******************************************************************************
STATIC XmgrCas_NaBanCardError_Mediaset_t 			s_stNaMsetBanCardError;


//******************************************************************************
//	PROTOTYPE FUNCTIONS
//******************************************************************************
#define ________XMGR_CAS_NA_PpvMEDIASET_BANNER_STATIC___________________________________________
STATIC XmgrCas_NaBanCardError_Mediaset_t* xmgr_cas_NaPpvBanScErrorGetLocalContents_Mediaset(void);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorInitContents_Mediaset(XmgrCas_NaBanCardError_Mediaset_t *pstContents);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgDetail_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Mediaset(XmgrCas_NaBanCardError_Mediaset_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDestroyUi_Mediaset(XmgrCas_NaBanCardError_Mediaset_t *pContents);

#define ________XMGR_CAS_NA_PpvMEDIASET_BANNER_PUBLIC___________________________________________
BUS_Result_t xproc_cas_NaBannerSmartCardError_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#define	_________________________STATIC_BODY______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorGetLocalContents_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaBanCardError_Mediaset_t* xmgr_cas_NaPpvBanScErrorGetLocalContents_Mediaset(void)
{
	return &s_stNaMsetBanCardError;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorInitContents_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorInitContents_Mediaset(XmgrCas_NaBanCardError_Mediaset_t *pstContents)
{
	XmgrCas_NaAlamType_e 		eScAlarm;

	xmgr_cas_NaGetSmartCardStatus(&eScAlarm);
	HxLOG_Print("eScAlarm = %d\n", eScAlarm);

	if (eScAlarm != eCAS_NA_CA_SMARTCARD_REMOVED && eScAlarm != eCAS_NA_CA_SMARTCARD_MUTE && eScAlarm != eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
	{
		if (!xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(eNAGRA_OP_TYPE_MEDIASET))
		{
			HxLOG_Print("xmgr_cas_NaMatchCardStatusWithOpType_Mediaset Mediaset FALSE!!!\n");

			if (xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(eNAGRA_OP_TYPE_PANGEA))
			{
				HxLOG_Print("xmgr_cas_NaMatchCardStatusWithOpType_Mediaset Pangea FALSE!!!\n");
				eScAlarm = eCAS_NA_CA_SMARTCARD_INVALID;
			}
		}
	}

	memset(pstContents, 0, sizeof(XmgrCas_NaBanCardError_Mediaset_t));

	/*status error msg*/
	pstContents->bDisplayMode = 1;
	pstContents->cardStatus = eScAlarm;
	pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_ERR_SVC_SCRAMBLED_ID);

	if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_REMOVED)
	{
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_ERR_INSERT_CARD_ID);
	}
	else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_SUSPENDED)
	{
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_ERR_BLOCKED_CARD_ID);
	}
	else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_INVALID
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_COM_ERROR
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_MUTE
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NEVER_PAIRED
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
	{
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_ERR_UNREADABLE_CARD_ID);
	}
	else if (pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_BLACKLISTED)
	{
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_ERR_BLACKLIST_CARD_ID);
	}
	else if (pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_EXPIRED)
	{
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_ERR_EXPIRED_CARD_ID);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e913_3_24_WIN3_260, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgDetail_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgDetail_Mediaset(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	OSD_Rect_t				rect_msg;
	Text_Property_t			stTextProperty;

	XmgrCas_NaBanCardError_Mediaset_t	*pstContents = xmgr_cas_NaPpvBanScErrorGetLocalContents_Mediaset();

	rect_msg.x = pArea->x1 + pRect->x;
	rect_msg.y = pArea->x1 + pRect->y;
	rect_msg.w = pRect->w;
	rect_msg.h = pRect->h;

	stTextProperty.ulFontSize = FONT_SIZE_M;
	stTextProperty.ulFontAlign = TEXT_ALIGN_CENTER;
	stTextProperty.cFontColor = COL(C_T_Cursor);

	if (MWC_UTIL_DvbStrlenStrOnly(pstContents->pszSmartCardErrorMsgDetailText) > 0)
	{
		hError = AP_LNF_DrawLongStringWithWordwrap(pScreen, pArea, &rect_msg, pstContents->pszSmartCardErrorMsgDetailText,
										stTextProperty, rect_msg.w, param1);

		if (hError != ERR_OK)
		{
			HxLOG_Error(" AP_LNF_DrawLongStringWithWordwrap Error!\n");
			return ERR_FAIL;
		}
	}
	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Mediaset(XmgrCas_NaBanCardError_Mediaset_t *pContents)
{
	OSD_Rect_t				rect = {0, };

	/* for compiler */
	(void)pContents;

	/* na ppv mediaset scerror background */
	Settings_DrawBack(FALSE);

	/* na ppv mediaset scerror card error background*/
	OSD_GFX_SetRect (&rect, NAPPV_MEDIASET_BAN_SCERROR_MSG_BACKGROUND_X, NAPPV_MEDIASET_BAN_SCERROR_MSG_BACKGROUND_Y, NAPPV_MEDIASET_BAN_SCERROR_MSG_BACKGROUND_WIDTH, NAPPV_MEDIASET_BAN_SCERROR_MSG_BACKGROUND_HEIGHT);
	GWM_Frame_Create (NAPPV_MEDIASET_BAN_SCERROR_MSG_BACKGROUND_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_BAN_SCERROR_MSG_BACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage_Mediaset);

	/* na ppv mediaset scerror card error text*/
	OSD_GFX_SetRect (&rect, NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_X, NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_Y - FONT_SIZE_M, NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_WIDTH, NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_ID, &rect, pContents->pszSmartCardErrorMsgText);
	GWM_Text_SetAlign (NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_ID, TEXT_ALIGN_CENTER);
	GWM_Text_SetSize (NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor (NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv mediaset scerror card error detail text*/
	OSD_GFX_SetRect (&rect, NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_X, NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_Y - FONT_SIZE_M, NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_WIDTH, NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgDetail_Mediaset);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDestroyUi_Mediaset
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDestroyUi_Mediaset(XmgrCas_NaBanCardError_Mediaset_t *pContents)
{
	return ERR_BUS_NO_ERROR;
}

#define	_________________________PUBLIC_BODY______________________________
/******************************************************************************/
/**
*   \fn     xproc_cas_NaBannerSmartCardError_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaBannerSmartCardError_Mediaset(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaBanCardError_Mediaset_t	*pstContents = xmgr_cas_NaPpvBanScErrorGetLocalContents_Mediaset();
	BUS_Result_t						ulResult = ERR_BUS_NO_ERROR;

	switch(message)
	{
		case MSG_GWM_CREATE:
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "xproc_cas_NaBannerSmartCardError_Mediaset");
			if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Pangea))
			{
				XmgrCas_NaAlamType_e 		eScAlarm;

				xmgr_cas_NaGetSmartCardStatus(&eScAlarm);
				HxLOG_Print("eScAlarm = %d\n", eScAlarm);

				if (eScAlarm != eCAS_NA_CA_SMARTCARD_REMOVED && eScAlarm != eCAS_NA_CA_SMARTCARD_MUTE && eScAlarm != eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
				{
					if (xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(eNAGRA_OP_TYPE_PANGEA))
					{
						// Pangea가 맞을 경우 유지
						BUS_MGR_Destroy(NULL);
						return MESSAGE_BREAK;
					}
				}

				BUS_MGR_Destroy(xproc_cas_NaBannerSmartCardError_Pangea);
			}

			/* set app area */
			GWM_APP_SetAppArea(NAPPV_MEDIASET_BAN_SCERROR_WINDOW_X, NAPPV_MEDIASET_BAN_SCERROR_WINDOW_Y,
								NAPPV_MEDIASET_BAN_SCERROR_WINDOW_WIDTH, NAPPV_MEDIASET_BAN_SCERROR_WINDOW_HEIGHT);

			// Init the contents
			ulResult = xmgr_cas_NaPpvBanScErrorInitContents_Mediaset(pstContents);
			if (ulResult != ERR_BUS_NO_ERROR)
			{
				break;
			}

			// draw UI object
			xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Mediaset(pstContents);

			break;

		case MSG_GWM_FAIL_RECREATE_NEW_INSTANCE:
			HxLOG_Print("MSG_GWM_FAIL_RECREATE_NEW_INSTANCE, p1 = %d, p2 = %d\n", p1, p2);
			if (p1 == CA_EVENT_SUCCESS)
			{
				ulResult = xmgr_cas_NaPpvBanScErrorInitContents_Mediaset(pstContents);
				if (ulResult != ERR_BUS_NO_ERROR)
				{
					break;
				}
				GWM_Text_SetText(NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_ID, pstContents->pszSmartCardErrorMsgText);
				GWM_APP_InvalidateObject(NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_ID);
			}
			else if (p1 == CA_EVENT_ERROR)
			{
				XmgrCas_NaAlamType_e 		eScAlarm;

				xmgr_cas_NaGetSmartCardStatus(&eScAlarm);
				HxLOG_Print("xmgr_cas_NaGetSmartCardStatus : eScAlarm(%d)", eScAlarm);
				if (eScAlarm != eCAS_NA_CA_SMARTCARD_REMOVED && eScAlarm != eCAS_NA_CA_SMARTCARD_MUTE && eScAlarm != eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
				{
					if (!xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(eNAGRA_OP_TYPE_MEDIASET))
					{
						HxLOG_Print("xmgr_cas_NaMatchCardStatusWithOpType_Mediaset FALSE!!!\n");
						eScAlarm = eCAS_NA_CA_SMARTCARD_INVALID;
					}
				}

				if (eScAlarm == eCAS_NA_CA_SMARTCARD_OK)
				{
					if (pstContents->pszSmartCardErrorMsgDetailText == (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_ERR_PURCASE_PERIOD_END_ID)
						|| pstContents->pszSmartCardErrorMsgDetailText == (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_ERR_BLACKLIST_CARD_ID) // CARD Activation 처리
						)
					{
						pstContents->pszSmartCardErrorMsgDetailText = NULL;
						GWM_APP_InvalidateObject(NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_ID);
					}
				}
			}
			else if (p1 == CA_EVENT_PURCHASE_PERIOD_OVER)
			{
				if (pstContents->pszSmartCardErrorMsgDetailText != (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_ERR_PURCASE_PERIOD_END_ID))
				{
					pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_ERR_PURCASE_PERIOD_END_ID);
					GWM_APP_InvalidateObject(NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_ID);
				}
			}
			break;

		case MSG_APP_UPDATE_SMARTCARD_INFO:
			if(p1 == eDxCAS_GROUPID_NA)
			{
				HxLOG_Print("MSG_APP_UPDATE_SMARTCARD_INFO\n");
				ulResult = xmgr_cas_NaPpvBanScErrorInitContents_Mediaset(pstContents);
				if (ulResult != ERR_BUS_NO_ERROR)
				{
					break;
				}
				GWM_Text_SetText(NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_ID, pstContents->pszSmartCardErrorMsgText);
				GWM_APP_InvalidateObject(NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_ID);
			}
			break;

		case eSEVT_CAS_SC_INSERTED:
		case eSEVT_CAS_SC_REMOVED:
			{
				HxLOG_Print("eSEVT_CAS_SC_INSERTED or eSEVT_CAS_SC_REMOVED\n");
				ulResult = xmgr_cas_NaPpvBanScErrorInitContents_Mediaset(pstContents);
				if (ulResult != ERR_BUS_NO_ERROR)
				{
					break;
				}
				GWM_Text_SetText(NAPPV_MEDIASET_BAN_SCERROR_MSG_TEXT_ID, pstContents->pszSmartCardErrorMsgText);
				GWM_APP_InvalidateObject(NAPPV_MEDIASET_BAN_SCERROR_MSG_DETAIL_TEXT_ID);
			}
			break;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_TIMER:
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			xmgr_cas_NaPpvBanScErrorDestroyUi_Mediaset(pstContents);

			break;

		case MSG_GWM_QUIT_UI:
			HxLOG_Print("MESSAGE_PASS: MSG_GWM_QUIT_UI\n");
			return MESSAGE_PASS;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}



#endif //#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
