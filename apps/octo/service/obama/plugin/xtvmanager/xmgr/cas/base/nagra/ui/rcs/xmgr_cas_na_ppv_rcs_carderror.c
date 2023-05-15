#if 0

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

#include <timeconv.h>

//******************************************************************************
//	DEFINE
//******************************************************************************
#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	DOMAIN_TVMGR_CAS_NAGRA


#define	MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH				128

typedef struct tag_CAS_NA_RCS_BAN_SCE_Contents
{
	HUINT8		bDisplayMode;
	HINT32		cardStatus;
	HINT8		szSmartCardErrorMsgText[MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH];
	HINT8		szSmartCardErrorMsgDetailText[MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH];
} XmgrCas_NaBanCardError_Rcs_t;

#define	NAPPV_RCS_BAN_SCERROR_WINDOW_WIDTH					OSD_SCREEN_WIDTH
#define	NAPPV_RCS_BAN_SCERROR_WINDOW_HEIGHT					260//OSD_SCREEN_HEIGHT
#define	NAPPV_RCS_BAN_SCERROR_WINDOW_X						0
#define	NAPPV_RCS_BAN_SCERROR_WINDOW_Y						117//0

#define	NAPPV_RCS_BAN_SCERROR_APP_ID							0

/* ppv scerror msg background*/
#define	NAPPV_RCS_BAN_SCERROR_MSG_BACKGROUND_ID				(NAPPV_RCS_BAN_SCERROR_APP_ID|0x0010)
#define	NAPPV_RCS_BAN_SCERROR_MSG_BACKGROUND_X				0
#define	NAPPV_RCS_BAN_SCERROR_MSG_BACKGROUND_Y				(117-117)
#define	NAPPV_RCS_BAN_SCERROR_MSG_BACKGROUND_WIDTH			1280
#define	NAPPV_RCS_BAN_SCERROR_MSG_BACKGROUND_HEIGHT			260

/* ppv scerror msg */
#define	NAPPV_RCS_BAN_SCERROR_MSG_TEXT_ID						(NAPPV_RCS_BAN_SCERROR_APP_ID|0x0020)
#define	NAPPV_RCS_BAN_SCERROR_MSG_TEXT_X						190
#define	NAPPV_RCS_BAN_SCERROR_MSG_TEXT_Y						(257-117)
#define	NAPPV_RCS_BAN_SCERROR_MSG_TEXT_WIDTH					900
#define	NAPPV_RCS_BAN_SCERROR_MSG_TEXT_HEIGHT					34

/* ppv scerror msg */
#define	NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_ID				(NAPPV_RCS_BAN_SCERROR_APP_ID|0x0030)
#define	NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_X				190
#define	NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_Y				(313-117)
#define	NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_WIDTH			900
#define	NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_HEIGHT			34 * 2


//******************************************************************************
//	STATIC VARIABLES
//******************************************************************************
STATIC XmgrCas_NaBanCardError_Rcs_t 			s_stNaRCSBanCardError;


//******************************************************************************
//	PROTOTYPE FUNCTIONS
//******************************************************************************
#define ________XMGR_CAS_NA_PpvRCS_BANNER_STATIC___________________________________________
STATIC XmgrCas_NaBanCardError_Rcs_t* xmgr_cas_NaPpvBanScErrorGetLocalContents_Rcs(void);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorInitContents_Rcs(XmgrCas_NaBanCardError_Rcs_t *pstContents, HINT32 cardStatus);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage_Rcs(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect, GWM_GuiObject_t pObject, void *pArg, int param1);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Rcs(XmgrCas_NaBanCardError_Rcs_t *pContents);
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDestroyUI_Rcs(XmgrCas_NaBanCardError_Rcs_t *pContents);


#define	_________________________STATIC_BODY______________________________
/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorGetLocalContents_Rcs
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC XmgrCas_NaBanCardError_Rcs_t* xmgr_cas_NaPpvBanScErrorGetLocalContents_Rcs(void)
{
	return &s_stNaRCSBanCardError;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorInitContents_Rcs
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorInitContents_Rcs(XmgrCas_NaBanCardError_Rcs_t *pstContents, HINT32 cardStatus)
{
	XmgrCas_NaAlamType_e 		eScAlarm;

	xmgr_cas_NaGetSmartCardStatus(&eScAlarm);

	VK_MEM_Memset(pstContents, 0, sizeof(XmgrCas_NaBanCardError_Rcs_t));

	/*status error msg*/
	pstContents->bDisplayMode = 1;
	pstContents->cardStatus = eScAlarm;

	HxSTD_memset(pstContents->szSmartCardErrorMsgText, 0, MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH);

	switch(pstContents->cardStatus)
	{
	case eCAS_NA_CA_SMARTCARD_REMOVED:
		MWC_UTIL_DvbStrncpy(pstContents->szSmartCardErrorMsgDetailText, (HUINT8 *)GetStrRsc(LOC_CAS_NA_RCS_SMARTCARD_REMOVED), MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH-1);
		break;

	case eCAS_NA_CA_SMARTCARD_NOT_CERTIFIED:
		MWC_UTIL_DvbStrncpy(pstContents->szSmartCardErrorMsgDetailText, (HUINT8 *)GetStrRsc(LOC_CAS_NA_RCS_SMARTCARD_NOT_CERTIFIED), MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH-1);
		break;

	case eCAS_NA_CA_SMARTCARD_SUSPENDED:
		MWC_UTIL_DvbStrncpy(pstContents->szSmartCardErrorMsgDetailText, (HUINT8 *)GetStrRsc(LOC_CAS_NA_RCS_SMARTCARD_SUSPENDED), MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH-1);
		break;

	case eCAS_NA_CA_SMARTCARD_INVALID:
		MWC_UTIL_DvbStrncpy(pstContents->szSmartCardErrorMsgDetailText, (HUINT8 *)GetStrRsc(LOC_CAS_NA_RCS_SMARTCARD_INVALID), MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH-1);
		break;

	case eCAS_NA_CA_SMARTCARD_COM_ERROR:
		MWC_UTIL_DvbStrncpy(pstContents->szSmartCardErrorMsgDetailText, (HUINT8 *)GetStrRsc(LOC_CAS_NA_RCS_SMARTCARD_COM_ERROR), MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH-1);

		break;

	case eCAS_NA_CA_SMARTCARD_MUTE:
		MWC_UTIL_DvbStrncpy(pstContents->szSmartCardErrorMsgDetailText, (HUINT8 *)GetStrRsc(LOC_CAS_NA_RCS_SMARTCARD_MUTE), MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH-1);
		break;

	case eCAS_NA_CA_SMARTCARD_NEVER_PAIRED:
		MWC_UTIL_DvbStrncpy(pstContents->szSmartCardErrorMsgDetailText, (HUINT8 *)GetStrRsc(LOC_CAS_NA_RCS_SMARTCARD_NEVER_PAIRED), MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH-1);
		break;

	case eCAS_NA_CA_SMARTCARD_NOT_PAIRED:
		MWC_UTIL_DvbStrncpy(pstContents->szSmartCardErrorMsgDetailText, (HUINT8 *)GetStrRsc(LOC_CAS_NA_RCS_SMARTCARD_NOT_PAIRED), MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH-1);
		break;

	case eCAS_NA_CA_SMARTCARD_BLACKLISTED:
		MWC_UTIL_DvbStrncpy(pstContents->szSmartCardErrorMsgDetailText, (HUINT8 *)GetStrRsc(LOC_CAS_NA_RCS_SMARTCARD_BLACKLISTED), MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH-1);
		break;

	case eCAS_NA_CA_SMARTCARD_EXPIRED:
		MWC_UTIL_DvbStrncpy(pstContents->szSmartCardErrorMsgDetailText, (HUINT8 *)GetStrRsc(LOC_CAS_NA_RCS_SMARTCARD_EXPIRED), MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH-1);
		break;


	}

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage_Rcs
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage_Rcs(OSD_Screen_t *pScreen, OSD_Area_t *pArea, OSD_Rect_t *pRect,
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
	// TODO: dwcho
/*
	if (!AP_StockImg_GetImage (e913_3_24_WIN3_260, &stImage))
	{
		OSD_GFX_SetRect (&stRect, nX1, nY1, stImage.width, stImage.height);
		OSD_GFX_FB_Blend (stImage.hBuffer, stImage.width, stImage.height, pScreen, &stRect);
	}
*/
	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Rcs
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Rcs(XmgrCas_NaBanCardError_Rcs_t *pContents)
{
	OSD_Rect_t				rect = {0, };

	/* for compiler */
	(void)pContents;

	/* na ppv scerror background */
	Settings_DrawBack(FALSE);

	/* na ppv card error background*/
	OSD_GFX_SetRect (&rect, NAPPV_RCS_BAN_SCERROR_MSG_BACKGROUND_X, NAPPV_RCS_BAN_SCERROR_MSG_BACKGROUND_Y, NAPPV_RCS_BAN_SCERROR_MSG_BACKGROUND_WIDTH, NAPPV_RCS_BAN_SCERROR_MSG_BACKGROUND_HEIGHT);
	GWM_Frame_Create (NAPPV_RCS_BAN_SCERROR_MSG_BACKGROUND_ID, &rect, NULL);
	GWM_APP_SetObjectDrawMethod(NAPPV_RCS_BAN_SCERROR_MSG_BACKGROUND_ID, (GWM_ObjDraw_t)xmgr_cas_NaPpvBanScErrorDrawMsgBackgroundImage_Rcs);

	/* na ppv card error text*/
	OSD_GFX_SetRect (&rect, NAPPV_RCS_BAN_SCERROR_MSG_TEXT_X, NAPPV_RCS_BAN_SCERROR_MSG_TEXT_Y - FONT_SIZE_M, NAPPV_RCS_BAN_SCERROR_MSG_TEXT_WIDTH, NAPPV_RCS_BAN_SCERROR_MSG_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_RCS_BAN_SCERROR_MSG_TEXT_ID, &rect, pContents->szSmartCardErrorMsgText);
	GWM_Text_SetAlign (NAPPV_RCS_BAN_SCERROR_MSG_TEXT_ID, TEXT_ALIGN_CENTER);
	GWM_Text_SetSize (NAPPV_RCS_BAN_SCERROR_MSG_TEXT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor (NAPPV_RCS_BAN_SCERROR_MSG_TEXT_ID, COL(C_T_Cursor), 0x00);

	/* na ppv card error detail text*/
	OSD_GFX_SetRect (&rect, NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_X, NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_Y - FONT_SIZE_M, NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_WIDTH, NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_HEIGHT);
	GWM_Text_Create (NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_ID, &rect, pContents->szSmartCardErrorMsgDetailText);
	GWM_Text_SetAlign (NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_ID, TEXT_ALIGN_CENTER);
	GWM_Text_SetSize (NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_ID, FONT_SIZE_M);
	GWM_Text_ChangeColor (NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_ID, COL(C_T_Cursor), 0x00);

	return ERR_BUS_NO_ERROR;
}

/********************************************************************
 Function		: xmgr_cas_NaPpvBanScErrorDestroyUI_Rcs
 Description	:
 Input		:
 Output 		:
 Return 		:
 ********************************************************************/
STATIC BUS_Result_t xmgr_cas_NaPpvBanScErrorDestroyUI_Rcs(XmgrCas_NaBanCardError_Rcs_t *pContents)
{
	return ERR_BUS_NO_ERROR;
}

#define	_________________________PUBLIC_BODY______________________________
/******************************************************************************/
/**
*   \fn     xproc_cas_NaBannerSmartCardError_Rcs(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
*   \brief  system status procedure
*
*   @param[in]  gwm msg/action handler
*   @return     BUS_ProcessMessageDefault(message, hAction, p1, p2, p3)
*   @ingroup    SYSTEM STATUS
*/
/******************************************************************************/
BUS_Result_t xproc_cas_NaBannerSmartCardError_Rcs(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	XmgrCas_NaBanCardError_Rcs_t			*pstContents = xmgr_cas_NaPpvBanScErrorGetLocalContents_Rcs();
	HUINT32							nResult = ERR_FAIL;

	switch(message)
	{
		case MSG_GWM_CREATE:
			/* set app area */
			GWM_APP_SetAppArea(NAPPV_RCS_BAN_SCERROR_WINDOW_X, NAPPV_RCS_BAN_SCERROR_WINDOW_Y,
								NAPPV_RCS_BAN_SCERROR_WINDOW_WIDTH, NAPPV_RCS_BAN_SCERROR_WINDOW_HEIGHT);

			// Init the contents
			xmgr_cas_NaPpvBanScErrorInitContents_Rcs(pstContents, p1);

			// draw UI object
			xmgr_cas_NaPpvBanScErrorDrawDafaultUi_Rcs(pstContents);
			break;

		case MSG_GWM_FAIL_RECREATE_NEW_INSTANCE:
			if(p1 == eDxCAS_GROUPID_NA)
			{
				if (p2 == CA_EVENT_SUCCESS)
				{
					xmgr_cas_NaPpvBanScErrorInitContents_Rcs(pstContents, p1);
					GWM_Text_SetText(NAPPV_RCS_BAN_SCERROR_MSG_TEXT_ID, pstContents->szSmartCardErrorMsgText);
					GWM_Text_SetText(NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_ID, pstContents->szSmartCardErrorMsgDetailText);
				}
				else if (p2 == CA_EVENT_ERROR)
				{
					HxSTD_memset(pstContents->szSmartCardErrorMsgDetailText, 0, MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH);
					GWM_Text_SetText(NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_ID, pstContents->szSmartCardErrorMsgDetailText);
				}
				else if (p2 == CA_EVENT_PURCHASE_PERIOD_OVER)
				{
					MWC_UTIL_DvbStrncpy(pstContents->szSmartCardErrorMsgDetailText, (HUINT8 *)GetStrRsc(LOC_CAS_NAGRA_MEDIASET_BANNER_ERR_PURCASE_PERIOD_END_ID), MAX_RCS_BAN_SMARTCARD_ERROR_MSG_LENGTH);
					GWM_Text_SetText(NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_ID, pstContents->szSmartCardErrorMsgDetailText);
				}
			}
			break;

		case MSG_APP_UPDATE_SMARTCARD_INFO:
			if(p1 == eDxCAS_GROUPID_NA)
			{
				xmgr_cas_NaPpvBanScErrorInitContents_Rcs(pstContents, p1);
				GWM_Text_SetText(NAPPV_RCS_BAN_SCERROR_MSG_TEXT_ID, pstContents->szSmartCardErrorMsgText);
				GWM_Text_SetText(NAPPV_RCS_BAN_SCERROR_MSG_DETAIL_TEXT_ID, pstContents->szSmartCardErrorMsgDetailText);
			}
			break;

		case MSG_GWM_CLICKED :
			break;

		case MSG_GWM_TIMER:
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			/* destroy UI */
			xmgr_cas_NaPpvBanScErrorDestroyUI_Rcs(pstContents);
			break;

		case MSG_GWM_QUIT_UI:
			HxLOG_Print("MESSAGE_PASS: MSG_GWM_QUIT_UI\n");
			return MESSAGE_PASS;

		default:
			break;
	}

	HxLOG_Info("xproc_cas_NaBannerSmartCardError_Rcs handle(0x%x)\n", hAction);

	return BUS_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

#endif

/* end of file */
