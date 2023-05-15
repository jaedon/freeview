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



#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
//******************************************************************************
//	DEFINE
//******************************************************************************

#define	MAX_PGEA_BAN_SMARTCARD_ERROR_MSG_LENGTH				128

typedef struct tag_CAS_NA_PANGEA_BAN_SCE_Contents
{
	HUINT8		bDisplayMode;
	HINT32		cardStatus;
	HUINT8		*pszSmartCardErrorMsgText;
	HUINT8		*pszSmartCardErrorMsgDetailText;
} XmgrCas_NaBanCardError_Pangea_t;

#define	NAPPV_PANGEA_BAN_SCERROR_WINDOW_WIDTH							1280//OSD_SCREEN_WIDTH
#define	NAPPV_PANGEA_BAN_SCERROR_WINDOW_HEIGHT							67//OSD_SCREEN_HEIGHT
#define	NAPPV_PANGEA_BAN_SCERROR_WINDOW_X								0
#define	NAPPV_PANGEA_BAN_SCERROR_WINDOW_Y								227//0

#define	NAPPV_PANGEA_BAN_SCERROR_APP_ID									0

/* ppv pangea scerror msg background*/
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_TITLE_BACKGROUND_ID				(NAPPV_PANGEA_BAN_SCERROR_APP_ID|0x0010)
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_TITLE_BACKGROUND_X					0
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_TITLE_BACKGROUND_Y					(227-227)
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_TITLE_BACKGROUND_WIDTH				1280
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_TITLE_BACKGROUND_HEIGHT			67

#define	NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_ID						(NAPPV_PANGEA_BAN_SCERROR_APP_ID|0x0020)
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_X						0
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_Y						(294-227)
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_WIDTH					1280
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_HEIGHT					121

/* ppv pangea scerror msg */
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_ID							(NAPPV_PANGEA_BAN_SCERROR_APP_ID|0x0030)
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_X								310
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_Y								(265-227)
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_WIDTH							500
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_HEIGHT						34

/* ppv pangea scerror msg */
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_ID						(NAPPV_PANGEA_BAN_SCERROR_APP_ID|0x0040)
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_X						240
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_Y						(345-227)
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_WIDTH					800
#define	NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_HEIGHT					(34 * 2)

//******************************************************************************
//	STATIC VARIABLES
//******************************************************************************
STATIC XmgrCas_NaBanCardError_Pangea_t 			s_stNaPgeaBanCardError;


//******************************************************************************
//	PROTOTYPE FUNCTIONS
//******************************************************************************
#define ________XMGR_CAS_NA_PpvPANGEA_BANNER_STATIC___________________________________________
STATIC XmgrCas_NaBanCardError_Pangea_t* xmgr_cas_NaPpvBanScErrorGetLocalContents_Pangea(void);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorInitContents_Pangea(XmgrCas_NaBanCardError_Pangea_t *pstContents);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgTitleBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgDetail_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Pangea(XmgrCas_NaBanCardError_Pangea_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDestroyUI_Pangea(XmgrCas_NaBanCardError_Pangea_t *pContents);

#define ________XMGR_CAS_NA_PpvPANGEA_BANNER_PUBLIC___________________________________________
BUS_Result_t xproc_cas_NaBannerSmartCardError_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

#define	_________________________STATIC_BODY______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorGetLocalContents_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaBanCardError_Pangea_t* xmgr_cas_NaPpvBanScErrorGetLocalContents_Pangea(void)
{
	return &s_stNaPgeaBanCardError;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorInitContents_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorInitContents_Pangea(XmgrCas_NaBanCardError_Pangea_t *pstContents)
{
	XmgrCas_NaAlamType_e 		eScAlarm;

	xmgr_cas_NaGetSmartCardStatus(&eScAlarm);
	HxLOG_Print("eScAlarm = %d\n", eScAlarm);

	if (eScAlarm != eCAS_NA_CA_SMARTCARD_REMOVED && eScAlarm != eCAS_NA_CA_SMARTCARD_MUTE && eScAlarm != eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
	{
		if (!xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(eNAGRA_OP_TYPE_PANGEA))
		{
			HxLOG_Print("xmgr_cas_NaMatchCardStatusWithOpType_Mediaset FALSE!!!\n");
			eScAlarm = eCAS_NA_CA_SMARTCARD_INVALID;
		}
	}
	else
	{
		BUS_MGR_Destroy(NULL);

		BUS_MGR_Create("xproc_cas_NaBannerSmartCardError_Mediaset",
						APP_CAS_PRIORITY,
						xproc_cas_NaBannerSmartCardError_Mediaset,
						SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()),
						eScAlarm,
						0,
						0);

		return ERR_FAIL;
	}

	memset(pstContents, 0, sizeof(XmgrCas_NaBanCardError_Pangea_t));

	/*status error msg*/
	pstContents->bDisplayMode = 1;
	pstContents->cardStatus = eScAlarm;
	pstContents->pszSmartCardErrorMsgText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_ERR_SVC_SCRAMBLED_ID);

	if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_REMOVED)
	{
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_ERR_INSERT_CARD_ID);
	}
	else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_SUSPENDED)
	{
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_ERR_BLOCKED_CARD_ID);
	}
	else if(pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_INVALID
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_COM_ERROR
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_MUTE
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NEVER_PAIRED
		|| pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
	{
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_ERR_UNREADABLE_CARD_ID);
	}
	else if (pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_BLACKLISTED)
	{
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_ERR_BLACKLIST_CARD_ID);
	}
	else if (pstContents->cardStatus == eCAS_NA_CA_SMARTCARD_EXPIRED)
	{
		pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_ERR_EXPIRED_CARD_ID);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDrawMsgTitleBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgTitleBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e922_1_24_Banner1_Title, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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

	if (!AP_StockImg_GetImage (e922_1_24_Banner1, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgDetail_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgDetail_Pangea(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
											GWM_GuiObject_t pObject, void *pArg, int param1)
{
	HERROR					hError;
	OSD_Rect_t				rect_msg;
	Text_Property_t			stTextProperty;

	XmgrCas_NaBanCardError_Pangea_t	*pstContents = xmgr_cas_NaPpvBanScErrorGetLocalContents_Pangea();

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
 Function		: xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Pangea(XmgrCas_NaBanCardError_Pangea_t *pContents)
{
	OSD_Rect_t				rect = {0, };

	/* for compiler */
	(void)pContents;

	/* na ppv pangea scerror background */
	Settings_DrawBack(FALSE);

	/* na ppv pangea scerror card error title background*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_BAN_SCERROR_MSG_TITLE_BACKGROUND_X, NAPPV_PANGEA_BAN_SCERROR_MSG_TITLE_BACKGROUND_Y, NAPPV_PANGEA_BAN_SCERROR_MSG_TITLE_BACKGROUND_WIDTH, NAPPV_PANGEA_BAN_SCERROR_MSG_TITLE_BACKGROUND_HEIGHT);
	GWM_Frame_Create (NAPPV_PANGEA_BAN_SCERROR_MSG_TITLE_BACKGROUND_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_BAN_SCERROR_MSG_TITLE_BACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgTitleBackgroundImage_Pangea);

	if (pContents->pszSmartCardErrorMsgDetailText)
	{
	/* na ppv pangea scerror card error msg background*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_X, NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_Y, NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_WIDTH, NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_HEIGHT);
	GWM_Frame_Create (NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage_Pangea);
	}

	/* na ppv pangea scerror card error text*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_X, NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_Y - FONT_SIZE_S, NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_WIDTH, NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_ID, &rect, pContents->pszSmartCardErrorMsgText);
	GWM_Text_SetAlign (NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_ID, TEXT_ALIGN_RIGHT);
	GWM_Text_SetSize (NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_ID, FONT_SIZE_S);
	GWM_Text_ChangeColor (NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv pangea scerror card error detail text*/
	OSD_GFX_SetRect (&rect, NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_X, NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_Y - FONT_SIZE_M, NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_WIDTH, NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawSmartCardMsgDetail_Pangea);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDestroyUI_Pangea
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDestroyUI_Pangea(XmgrCas_NaBanCardError_Pangea_t *pContents)
{
	BUS_PostMessage(NULL, eMEVT_XMGR_CAS_NA_CHANGE_PPV_OPTYPE, (Handle_t)NULL, eDxCAS_GROUPID_NA, eNAGRA_OP_TYPE_MEDIASET, 0); // 원위치

	return ERR_BUS_NO_ERROR;
}

#define	_________________________PUBLIC_BODY______________________________
/******************************************************************************/
/**
*   \fn     xproc_cas_NaBannerSmartCardError_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaBannerSmartCardError_Pangea(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaBanCardError_Pangea_t			*pstContents = xmgr_cas_NaPpvBanScErrorGetLocalContents_Pangea();

	switch(message)
	{
		case MSG_GWM_CREATE:

			if (BUS_MGR_Get(xproc_cas_NaBannerSmartCardError_Mediaset))
			{
				BUS_MGR_Destroy(xproc_cas_NaBannerSmartCardError_Mediaset);
			}

			/* set app area */
			GWM_APP_SetAppArea(NAPPV_PANGEA_BAN_SCERROR_WINDOW_X, NAPPV_PANGEA_BAN_SCERROR_WINDOW_Y,
								NAPPV_PANGEA_BAN_SCERROR_WINDOW_WIDTH, NAPPV_PANGEA_BAN_SCERROR_WINDOW_HEIGHT);

			// Init the contents
			if (xmgr_cas_NaPpvBanScErrorInitContents_Pangea(pstContents) == ERR_FAIL)
			{
				return MESSAGE_BREAK;
			}

			// draw UI object
			xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Pangea(pstContents);

			break;

		case MSG_GWM_FAIL_RECREATE_NEW_INSTANCE:
			HxLOG_Print("MSG_GWM_FAIL_RECREATE_NEW_INSTANCE, p1 = %d, p2 = %d\n", p1, p2);
			if (p1 == CA_EVENT_SUCCESS)
			{
				if (xmgr_cas_NaPpvBanScErrorInitContents_Pangea(pstContents) == ERR_FAIL)
				{
					return MESSAGE_BREAK;
				}

				GWM_Text_SetText(NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_ID, pstContents->pszSmartCardErrorMsgText);
				GWM_APP_InvalidateObject(NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_ID);

				if (!pstContents->pszSmartCardErrorMsgDetailText)
				{
					if (GWM_Obj_GetObjectByID(NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_ID))
					{
						GWM_Obj_Destroy(NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_ID);
					}
				}
			}
			else if (p1 == CA_EVENT_ERROR)
			{
				XmgrCas_NaAlamType_e 		eScAlarm;

				xmgr_cas_NaGetSmartCardStatus(&eScAlarm);
				HxLOG_Print("xmgr_cas_NaGetSmartCardStatus : eScAlarm(%d)", eScAlarm);
				if (eScAlarm != eCAS_NA_CA_SMARTCARD_REMOVED && eScAlarm != eCAS_NA_CA_SMARTCARD_MUTE && eScAlarm != eCAS_NA_CA_SMARTCARD_NOT_PAIRED)
				{
					if (!xmgr_cas_NaMatchCardStatusWithOpType_Mediaset(eNAGRA_OP_TYPE_PANGEA))
					{
						HxLOG_Print("xmgr_cas_NaMatchCardStatusWithOpType_Mediaset FALSE!!!\n");
						eScAlarm = eCAS_NA_CA_SMARTCARD_INVALID;
					}
				}

				if (eScAlarm == eCAS_NA_CA_SMARTCARD_OK)
				{
					if (pstContents->pszSmartCardErrorMsgDetailText == (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_ERR_PURCASE_PERIOD_END_ID)
						|| pstContents->pszSmartCardErrorMsgDetailText == (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_ERR_BLACKLIST_CARD_ID) // CARD Activation 처리
						)
					{
						pstContents->pszSmartCardErrorMsgDetailText = NULL;
						GWM_APP_InvalidateObject(NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_ID);

						if (GWM_Obj_GetObjectByID(NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_ID))
						{
							GWM_Obj_Destroy(NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_ID);
						}
					}
				}
			}
			else if (p1 == CA_EVENT_PURCHASE_PERIOD_OVER)
			{
				if (pstContents->pszSmartCardErrorMsgDetailText != (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_ERR_PURCASE_PERIOD_END_ID))
				{
					OSD_Rect_t	rect;

					pstContents->pszSmartCardErrorMsgDetailText = (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_PANGEA_BANNER_ERR_PURCASE_PERIOD_END_ID);
					GWM_APP_InvalidateObject(NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_ID);

					if (GWM_Obj_GetObjectByID(NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_ID) == NULL)
					{
						/* na ppv pangea scerror card error msg background*/
						OSD_GFX_SetRect (&rect, NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_X, NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_Y, NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_WIDTH, NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_HEIGHT);
						GWM_Frame_Create (NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_ID, &rect, NULL);
						GWM_APP_SetObjectDrawMethod(NAPPV_PANGEA_BAN_SCERROR_MSG_BACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage_Pangea);
					}
				}
			}
			break;

		case MSG_APP_UPDATE_SMARTCARD_INFO:
			if(p1 == eDxCAS_GROUPID_NA)
			{
				HxLOG_Print("MSG_APP_UPDATE_SMARTCARD_INFO\n");
				if (xmgr_cas_NaPpvBanScErrorInitContents_Pangea(pstContents) == ERR_FAIL)
				{
					return MESSAGE_PASS;
				}

				GWM_Text_SetText(NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_ID, pstContents->pszSmartCardErrorMsgText);
				GWM_APP_InvalidateObject(NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_ID);
			}
			break;

		case eSEVT_CAS_SC_INSERTED:
		case eSEVT_CAS_SC_REMOVED:
			{
				HxLOG_Print("eSEVT_CAS_SC_INSERTED or eSEVT_CAS_SC_REMOVED\n");
				if (xmgr_cas_NaPpvBanScErrorInitContents_Pangea(pstContents) == ERR_FAIL)
				{
					return MESSAGE_PASS;
				}

				GWM_Text_SetText(NAPPV_PANGEA_BAN_SCERROR_MSG_TEXT_ID, pstContents->pszSmartCardErrorMsgText);
				GWM_APP_InvalidateObject(NAPPV_PANGEA_BAN_SCERROR_MSG_DETAIL_TEXT_ID);
			}
			break;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_TIMER:
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			xmgr_cas_NaPpvBanScErrorDestroyUI_Pangea(pstContents);

			break;

		case MSG_GWM_QUIT_UI:
			HxLOG_Print("MESSAGE_PASS: MSG_GWM_QUIT_UI\n");
			return MESSAGE_PASS;

		default:
			break;
	}

	HxLOG_Info("xproc_cas_NaBannerSmartCardError_Pangea handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}



#endif //#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)
