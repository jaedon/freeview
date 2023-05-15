//******************************************************************************
//	INCLUDE
//******************************************************************************
#include <dlib.h>

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
#include <mgr_live.h>

#include <menuui_coor.h>
#include <settings.h>

#include <ap_util_osd.h>

#include <surferui_coor.h>
#include <ap_epg_main.h>

#include <namgr_main.h>

#include "../../local_include/_xmgr_cas_na_util.h"
#include "../../local_include/_xmgr_cas_na_adapt.h"
#include "../../local_include/_xmgr_cas_na.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
// dimension
// window
#define D_PPV_IPOP_DIALOG_X			276
#define D_PPV_IPOP_DIALOG_Y			150
#define D_PPV_IPOP_DIALOG_W			728
#define D_PPV_IPOP_DIALOG_H			419

// frame
#define D_PPV_IPOP_DIALOG_FRAME_X	0
#define D_PPV_IPOP_DIALOG_FRAME_Y	0
#define D_PPV_IPOP_DIALOG_FRAME_W	D_PPV_IPOP_DIALOG_W
#define D_PPV_IPOP_DIALOG_FRAME_H	D_PPV_IPOP_DIALOG_H

// text
#define D_PPV_IPOP_D_TXT_B_X			36
#define D_PPV_IPOP_D_TXT_B_Y			80
#define D_PPV_IPOP_D_TXT_B_W			656
#define D_PPV_IPOP_D_TXT_B_H			242

// button
#define D_PPV_IPOP_D_BTN_B_X			274-5
#define D_PPV_IPOP_D_BTN_B_Y			347-5
#define D_PPV_IPOP_D_BTN_B_W			180+10
#define D_PPV_IPOP_D_BTN_B_H			44+10


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/**@brief Reminder Conflict UI enum. */
typedef enum
{
	ePPV_IPOP_MAIN_FRAME_ID = 0,
	ePPV_IPOP_FRAME_ID,
	ePPV_IPOP_TEXT_ID,
	ePPV_IPOP_BTN_BASE_ID,
	ePPV_IPOP_ID_MAX,
} XmgrCas_NaRemcftUiCompIds_e;

typedef struct tagXmgrCas_NaPpvInfoPopContent_t
{
	EPG_APEVENT_t	ApEvt;
	RSV_Item_t 		RsvItem;
	HBOOL			IsThereRev;
} XmgrCas_NaPpvInfoPopContent_t;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
const STATIC OSD_Rect_t		s_stIPOPRect[] =
{
		{	D_PPV_IPOP_DIALOG_X,				D_PPV_IPOP_DIALOG_Y, 				D_PPV_IPOP_DIALOG_W,				D_PPV_IPOP_DIALOG_H			},
		{	D_PPV_IPOP_DIALOG_FRAME_X,			D_PPV_IPOP_DIALOG_FRAME_Y,			D_PPV_IPOP_DIALOG_FRAME_W,			D_PPV_IPOP_DIALOG_FRAME_H	},
		{	D_PPV_IPOP_D_TXT_B_X,				D_PPV_IPOP_D_TXT_B_Y,				D_PPV_IPOP_D_TXT_B_W,				D_PPV_IPOP_D_TXT_B_H		},
		{	D_PPV_IPOP_D_BTN_B_X,				D_PPV_IPOP_D_BTN_B_Y,				D_PPV_IPOP_D_BTN_B_W,				D_PPV_IPOP_D_BTN_B_H		},
};
STATIC XmgrCas_NaPpvInfoPopContent_t s_stPPVInfoPopContents;

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define _________EDIT_FUNTIONS____________

STATIC inline XmgrCas_NaPpvInfoPopContent_t * xmgr_cas_NaPpvInfoPopContents(void)
{
	return &s_stPPVInfoPopContents;
}


STATIC void xmgr_cas_NaPpvInfoPopGetUiPosition(XmgrCas_NaRemcftUiCompIds_e eUiId, OSD_Rect_t *rect)
{
	if ( eUiId >= ePPV_IPOP_ID_MAX )
	{
		HxSTD_memcpy(rect, &s_stIPOPRect[ePPV_IPOP_ID_MAX -1], sizeof(OSD_Rect_t));
	}
	else
	{
		HxSTD_memcpy(rect, &s_stIPOPRect[eUiId], sizeof(OSD_Rect_t));
	}
}

STATIC void xmgr_cas_NaPpvInfoPopInitObj(void)
{
	OSD_Rect_t		 	stRect;

	/* Set Area. */
	xmgr_cas_NaPpvInfoPopGetUiPosition(ePPV_IPOP_MAIN_FRAME_ID,  &stRect);
	GWM_APP_SetAppArea (stRect.x, stRect.y, stRect.w, stRect.h);

	/* frame. */
	xmgr_cas_NaPpvInfoPopGetUiPosition(ePPV_IPOP_FRAME_ID, &stRect);
	GWM_Frame_Create (ePPV_IPOP_FRAME_ID, &stRect, (char*)GetStrRsc(LOC_CAS_NA_RCS_PAYPERVIEWCHANNEL));
	GWM_APP_SetObjectDrawMethod (ePPV_IPOP_FRAME_ID, AP_LNF_WindowTD419Frame_Draw);

	/* text */
	xmgr_cas_NaPpvInfoPopGetUiPosition(ePPV_IPOP_TEXT_ID,  &stRect);
	GWM_Text_Create(ePPV_IPOP_TEXT_ID, &stRect, (HUINT8*)GetStrRsc(LOC_CAS_NA_RCS_THIS_IS_A_PPV_SERVICE));
	GWM_Text_SetSize(ePPV_IPOP_TEXT_ID, FONT_SIZE_XS);
	GWM_Text_ChangeColor(ePPV_IPOP_TEXT_ID, COL(C_T_Dialog_MSG), 0);
	GWM_APP_SetObjectDrawMethod(ePPV_IPOP_TEXT_ID, AP_LNF_PlainMultilineText_Draw);

	/* Button 2 */
	xmgr_cas_NaPpvInfoPopGetUiPosition(ePPV_IPOP_BTN_BASE_ID,  &stRect);
	GWM_Button_Create (ePPV_IPOP_BTN_BASE_ID, &stRect, (char*)GetStrRsc(LOC_DIALOG_OK_BUTTON_TEXT_ID));
	GWM_APP_SetObjectDrawMethod (ePPV_IPOP_BTN_BASE_ID, AP_LNF_DialogButton_Draw);

	/* Set focus */
	GWM_Obj_SetFocus (ePPV_IPOP_BTN_BASE_ID);
}

STATIC BUS_Result_t xmgr_cas_NaPpvInfoPopMsgDestroy(void)
{
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t xmgr_cas_NaPpvInfoPopMsgKeyDown(HINT32 p1)
{
	HERROR 		hErr = ERR_FAIL;

	AP_KeyConv_UiCommon(&p1);

	switch (p1)
	{
		case KEY_INFO :
		case KEY_GUIDE :
		case KEY_MENU :
		case KEY_OPT :
		case KEY_MEDIA :
			hErr = BUS_MGR_Destroy (0);
			if ( hErr != ERR_OK )
			{
				HxLOG_Print("[local_PPV_NA_MsgKeyDown] BUS_MGR_Destroy fail!!\n");
				break;
			}
			return MESSAGE_PASS;

		case KEY_BACK :
		case KEY_EXIT :
			hErr = BUS_MGR_Destroy (0);
			if ( hErr != ERR_OK )
			{
				HxLOG_Print("[local_PPV_NA_MsgKeyDown] BUS_MGR_Destroy fail!!\n");
				break;
			}
			return MESSAGE_BREAK;

		case KEY_ARROWLEFT :
		case KEY_ARROWRIGHT :
		case KEY_PLAY :
		case KEY_PAUSE :
		case KEY_STOP :
		case KEY_RECORD :
		case KEY_FORWARD :
		case KEY_BACKWARD :
		case KEY_PREV :
		case KEY_NEXT :
		case KEY_REPLAY :
		case KEY_SKIP :
		case KEY_ADD_BOOKMARK :
		case KEY_BOOKMARK_LIST :
		case KEY_SLOW :
			return MESSAGE_BREAK;

		default :
			break;
	}

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xproc_cas_NaPpvInfoPopup_Rcs (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR 					hErr = ERR_FAIL;
	BUS_Result_t				eResult = ERR_BUS_NO_ERROR;
	XmgrCas_NaPpvInfoPopContent_t 	*pContents = NULL;
	HBOOL					bPurchased = FALSE;

	pContents = (XmgrCas_NaPpvInfoPopContent_t*)xmgr_cas_NaPpvInfoPopContents();

	switch (message)
	{
		/**********************************************************************f
			GWM MESSAGES
		**********************************************************************/

		case MSG_GWM_CREATE :
			if(p1)
			{
				HxSTD_memcpy(&pContents->ApEvt, p1, sizeof(EPG_APEVENT_t));
			}
			else if (p2)
			{
				// SvcEpg_Event_t 로 받은 경우
				AP_EPGMAIN_ConvertEventMW2AP((SvcEpg_Event_t*)&p2, &pContents->ApEvt);
				pContents->ApEvt.hSvcHandle = (Handle_t)p3;
				pContents->ApEvt.ulEventState = MODE_EPGAPP_EVENT_VALID;
			}
			if(pContents->ApEvt.hSvcHandle != HANDLE_NULL)
			{
				HxSTD_memset(&pContents->RsvItem, 0x00, sizeof(RSV_Item_t));
				pContents->RsvItem.ulSlot = DxRSV_SLOT_NULL;
				pContents->RsvItem.hSvc = pContents->ApEvt.hSvcHandle;
				pContents->RsvItem.utStTime = pContents->ApEvt.stEventInfo.ulStartTime;
				pContents->RsvItem.utDuration = pContents->ApEvt.stEventInfo.ulDuration;
				pContents->IsThereRev = TRUE;
			}
			else
			{
				pContents->IsThereRev = FALSE;
			}


			xmgr_cas_NaPpvInfoPopInitObj();
			break;

		case MSG_GWM_KEYDOWN :
			eResult = xmgr_cas_NaPpvInfoPopMsgKeyDown(p1);
			break;

		case MSG_GWM_TIMER :
			break;

		case MSG_GWM_CLICKED :
			if(p1 == ePPV_IPOP_BTN_BASE_ID)
			{
				hErr = BUS_MGR_Destroy (0);
				if ( hErr != ERR_OK )
				{
					HxLOG_Print("[xmgr_cas_NaIppvBuyingPopup_Rcs] BUS_MGR_Destroy fail!!\n");
				}

				// reservation
				if(pContents->IsThereRev == TRUE)
				{
					// create reminder
					BUS_MGR_Create( "AP_RsvMain_Proc",
										APP_DIALOGBOX_PRIORITY,
										AP_RsvMain_Proc,
										HANDLE_NULL, (HINT32)&pContents->RsvItem, 0, 0);
				}

			}
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY :
			eResult = xmgr_cas_NaPpvInfoPopMsgDestroy();
			break;

		/**********************************************************************
			APP MESSAGES
		**********************************************************************/
		case eMEVT_LIVE_NOTIFY_SVC_CHANGED:
			// 채널 체인지 돼면 close
			hErr = BUS_MGR_Destroy (0);
			if ( hErr != ERR_OK )
			{
				HxLOG_Print("[xmgr_cas_NaIppvBuyingPopup_Rcs] BUS_MGR_Destroy fail!!\n");
				break;
			}
			break;

		case eMEVT_XMGR_CAS_NA_NOTIFY_RCS_PURCHASE_EVENT:
			// 이미 구매한 이벤트라면 종료
			hErr = xmgr_cas_NaPpvIsPurchasedEvent((void*)&pContents->ApEvt.stEventInfo, pContents->ApEvt.aucShortEventName, &bPurchased);
			if(hErr != ERR_OK)
			{
				bPurchased = FALSE;
			}

			if(bPurchased == TRUE)
			{
				hErr = BUS_MGR_Destroy (0);
				if ( hErr != ERR_OK )
				{
					HxLOG_Print("[xproc_cas_NaPpvInfoPopup_Rcs] BUS_MGR_Destroy fail!!\n");
					break;
				}
			}
			break;

		default :
			break;
	}

	if ( eResult != ERR_BUS_NO_ERROR )
	{
		return eResult;
	}

	return BUS_ProcessMessageDefault (message, hAction, p1, p2, p3);
}

/*********************** End of File ******************************/

