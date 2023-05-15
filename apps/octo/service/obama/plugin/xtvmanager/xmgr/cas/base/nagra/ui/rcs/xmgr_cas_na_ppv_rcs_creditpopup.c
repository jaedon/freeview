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

#include <surferui_coor.h>

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
#define D_PPV_CPOP_DIALOG_X			326
#define D_PPV_CPOP_DIALOG_Y			251
#define D_PPV_CPOP_DIALOG_W			628
#define D_PPV_CPOP_DIALOG_H			218

// frame
#define D_PPV_CPOP_DIALOG_FRAME_X	0
#define D_PPV_CPOP_DIALOG_FRAME_Y	0
#define D_PPV_CPOP_DIALOG_FRAME_W	D_PPV_CPOP_DIALOG_W
#define D_PPV_CPOP_DIALOG_FRAME_H	D_PPV_CPOP_DIALOG_H

// text
#define D_PPV_CPOP_D_TXT_B_X			36
#define D_PPV_CPOP_D_TXT_B_Y			(50)
#define D_PPV_CPOP_D_TXT_B_W			(D_PPV_CPOP_DIALOG_W - 36*2)
#define D_PPV_CPOP_D_TXT_B_H			(D_PPV_CPOP_DIALOG_H - 36*2)

// button
#define D_PPV_CPOP_D_BTN_B_X			(D_PPV_CPOP_DIALOG_W/2 - 180/2)
#define D_PPV_CPOP_D_BTN_B_Y			(D_PPV_CPOP_DIALOG_H - 54 - 25)
#define D_PPV_CPOP_D_BTN_B_W			180
#define D_PPV_CPOP_D_BTN_B_H			54


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/**@brief Reminder Conflict UI enum. */
typedef enum
{
	PPV_CPOP_MAIN_FRAME_ID = 0,
	PPV_CPOP_FRAME_ID,
	PPV_CPOP_TEXT_ID,
	PPV_CPOP_BTN_BASE_ID,
	PPV_CPOP_ID_MAX,
} XmgrCas_NaCPopUiId_e;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
const STATIC OSD_Rect_t		s_stCPOPRect[] =
{
		{	D_PPV_CPOP_DIALOG_X,				D_PPV_CPOP_DIALOG_Y, 				D_PPV_CPOP_DIALOG_W,				D_PPV_CPOP_DIALOG_H			},
		{	D_PPV_CPOP_DIALOG_FRAME_X,			D_PPV_CPOP_DIALOG_FRAME_Y,			D_PPV_CPOP_DIALOG_FRAME_W,			D_PPV_CPOP_DIALOG_FRAME_H	},
		{	D_PPV_CPOP_D_TXT_B_X,				D_PPV_CPOP_D_TXT_B_Y,				D_PPV_CPOP_D_TXT_B_W,				D_PPV_CPOP_D_TXT_B_H		},
		{	D_PPV_CPOP_D_BTN_B_X,				D_PPV_CPOP_D_BTN_B_Y,				D_PPV_CPOP_D_BTN_B_W,				D_PPV_CPOP_D_BTN_B_H		},
};

STATIC RSV_Item_t 	s_CreditPopRsvItem;
STATIC HBOOL		s_CreditPopIsThereRev = FALSE;


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define _________EDIT_FUNTIONS____________

STATIC void Xmgr_cas_NaPpvCreditPopGetUiPosition(XmgrCas_NaCPopUiId_e eUiId, OSD_Rect_t *rect)
{
	if ( eUiId >= PPV_CPOP_ID_MAX )
	{
		HxSTD_memcpy(rect, &s_stCPOPRect[PPV_CPOP_ID_MAX -1], sizeof(OSD_Rect_t));
	}
	else
	{
		HxSTD_memcpy(rect, &s_stCPOPRect[eUiId], sizeof(OSD_Rect_t));
	}
}

STATIC HUINT32 Xmgr_cas_NaPpvCreditPopGetPurchaseResultStringId(TCaProcessingStatus nStatus)
{
	HUINT32 nRet;
	switch ( nStatus )
	{
	/**< the request has been processed successfully */
	case CA_PROCESSING_NO_ERROR:
		nRet = LOC_RCS_YOU_HAVE_PURCHASED_ITEM_ID;
		break;

	case CA_PROCESSING_NO_VALID_SMARTCARD:		/**< no valid smartcard has been found to process the request */
		nRet = LOC_RCS_INVALID_SC_INSERT_RCS_CARD_ITEM_ID;
		break;

	case CA_PROCESSING_LOW_CREDIT:				/**< The smartcard does not contain enough credit to process the request */
		nRet = LOC_RCS_NOT_ENOUGH_CREDIT_ITEM_ID;
		break;

	case CA_PROCESSING_NO_CREDIT:				/**< The smartcard does not contain any credit at all and this prevents the CAK to process the request */
		nRet = LOC_RCS_NO_CREDIT_IN_RCS_CARD_ITEM_ID;
		break;

	case CA_PROCESSING_CREDIT_SUSPENDED:		/**< The smartcard credit is suspended and this prevents the CAK to process the request. */
		nRet = LOC_RCS_CREDIT_IS_SUSPENDED_ITEM_ID;
		break;

	case CA_PROCESSING_CREDIT_EXPIRED:			/**< The smartcard credit is expired and this prevents the CAK to process the request. */
		nRet = LOC_RCS_CREDIT_IS_EXPIRED_ITEM_ID;
		break;

	case CA_PROCESSING_MEMORY_FULL:				/**< The smartcard memory is full and this prevents the CAK to process the request. */
		nRet = LOC_RCS_YOUR_SC_MEM_IS_FULL_ITEM_ID;
		break;

	case CA_PROCESSING_ERROR:					/**< an unspecified error occurred during the request processing */
	case CA_PROCESSING_PARAMETER_INVALID:		/**< a parameter of the request is invalid */
	case CA_PROCESSING_PARAMETER_OUT_OF_RANGE:	/**< a parameter of the request is out of range */
	case CA_PROCESSING_PARAMETER_MISSING:		/**< a required parameter of the request is missing */
	case CA_PROCESSING_PARAMETER_INCOMPATIBLE:	/**< one or more parameters are incompatible with this request */
	case CA_PROCESSING_TERMINATION:				/**< the process stopped due to the CAK termination */
	case CA_PROCESSING_NO_MORE_RESOURCES:		/**< the request processing stopped due to unavailability of a system resource (allocated memory, semaphore) */
	case CA_PROCESSING_NUM_STATUS:				/**< number of processing status */
	default:
		nRet = LOC_RESPONSE_MSG_ERR_UNKNOWN_ID;
		break;
	}

	return nRet;
}


STATIC void Xmgr_cas_NaPpvCreditPopInitObj(TCaProcessingStatus nStatus, HINT32 p2)
{
	STATIC HUINT8		pTextMsg[256];
	HUINT8			pTmpTextMsg[256];
	OSD_Rect_t		 stRect;
	HERROR			hError = ERR_FAIL;
	TCreditAmount	CreditAmount = 0;
	TCreditStatus 		xStatus = CA_CREDIT_ERROR;

	if(p2)
	{
		HxSTD_memcpy(&s_CreditPopRsvItem, p2, sizeof(RSV_Item_t));
		s_CreditPopIsThereRev = TRUE;
	}
	else
	{
		HxSTD_memset(&s_CreditPopRsvItem, 0, sizeof(RSV_Item_t));
		s_CreditPopIsThereRev = FALSE;
	}

	// make string
	HxSTD_memset(pTextMsg, 0, 256);
	HxSTD_memset(pTmpTextMsg, 0, 256);
	if(nStatus == CA_PROCESSING_NO_ERROR)
	{
		// 5. load card credit
		hError = xmgr_cas_NaPpvGetCreditSimple(&CreditAmount, &xStatus);
		if(hError != ERR_OK || xStatus != CA_CREDIT_SUCCESS)
		{
			// 못 읽은 경우엔 구입 한 정보만 표시 한다.
			CreditAmount = 0;
			MWC_UTIL_DvbStrcat(pTextMsg, GetStrRsc(LOC_RCS_YOU_HAVE_PURCHASED_ITEM_ID));
		}
		else
		{
			MWC_UTIL_DvbStrcat(pTextMsg, GetStrRsc(LOC_RCS_YOU_HAVE_PURCHASED_ITEM_ID));
			sprintf(pTmpTextMsg, GetStrRsc(LOC_RCS_YOUR_NEW_CREDIT_ITEM_ID), CreditAmount/100,  CreditAmount%100);
			MWC_UTIL_DvbStrcat(pTextMsg, (HUINT8 *)"\n");
			MWC_UTIL_DvbStrcat(pTextMsg, pTmpTextMsg);
		}
	}
	else
	{
		MWC_UTIL_DvbStrcat(pTextMsg, GetStrRsc(Xmgr_cas_NaPpvCreditPopGetPurchaseResultStringId(nStatus)));
	}

	/* Set Area. */
	Xmgr_cas_NaPpvCreditPopGetUiPosition(PPV_CPOP_MAIN_FRAME_ID,  &stRect);
	GWM_APP_SetAppArea (stRect.x, stRect.y, stRect.w, stRect.h);

	/* frame. */
	Xmgr_cas_NaPpvCreditPopGetUiPosition(PPV_CPOP_FRAME_ID, &stRect);
	GWM_Frame_Create (PPV_CPOP_FRAME_ID, &stRect, (char*)NULL);
	GWM_APP_SetObjectDrawMethod (PPV_CPOP_FRAME_ID, AP_LNF_WindowMD218Frame_Draw);

	/* text */
	Xmgr_cas_NaPpvCreditPopGetUiPosition(PPV_CPOP_TEXT_ID,  &stRect);
	GWM_Text_Create(PPV_CPOP_TEXT_ID, &stRect, pTextMsg);
	GWM_Text_SetSize(PPV_CPOP_TEXT_ID, FONT_SIZE_S);
	GWM_Text_SetAlign(PPV_CPOP_TEXT_ID, TEXT_ALIGN_CENTER);
	GWM_Text_ChangeColor(PPV_CPOP_TEXT_ID, COL(C_T_Dialog_MSG), 0);
	GWM_APP_SetObjectDrawMethod(PPV_CPOP_TEXT_ID, AP_LNF_PlainMultilineText_Draw);

	/* Button */
	Xmgr_cas_NaPpvCreditPopGetUiPosition(PPV_CPOP_BTN_BASE_ID,  &stRect);
	GWM_Button_Create (PPV_CPOP_BTN_BASE_ID, &stRect, (char*)GetStrRsc(LOC_DIALOG_OK_BUTTON_TEXT_ID));
	GWM_APP_SetObjectDrawMethod (PPV_CPOP_BTN_BASE_ID, AP_LNF_DialogButton_Draw);

	/* Set focus */
	GWM_Obj_SetFocus (PPV_CPOP_BTN_BASE_ID);
}

STATIC BUS_Result_t Xmgr_cas_NaPpvCreditPopMsgGwmDestroy(void)
{
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t Xmgr_cas_NaPpvCreditPopMsgGwmKeyDown(HINT32 p1)
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
/*
			hErr = BUS_MGR_Destroy (0);
			if ( hErr != ERR_OK )
			{
				HxLOG_Print("[local_PPV_NA_MsgGwmKeyDown] BUS_MGR_Destroy fail!!\n");
				break;
			}
			return MESSAGE_PASS;
*/
			return MESSAGE_BREAK;

		case KEY_BACK :
		case KEY_EXIT :
			hErr = BUS_MGR_Destroy (0);
			if ( hErr != ERR_OK )
			{
				HxLOG_Print("[local_PPV_NA_MsgGwmKeyDown] BUS_MGR_Destroy fail!!\n");
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

BUS_Result_t xmgr_cas_NaRCS_PPVCreditPopup_Proc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR 				hErr = ERR_FAIL;
	BUS_Result_t			eResult = ERR_BUS_NO_ERROR;

	switch (message)
	{
		/**********************************************************************f
			GWM MESSAGES
		**********************************************************************/

		case MSG_GWM_CREATE :
			Xmgr_cas_NaPpvCreditPopInitObj((TCaProcessingStatus)p1, p2);
			break;

		case MSG_GWM_KEYDOWN :
			eResult = Xmgr_cas_NaPpvCreditPopMsgGwmKeyDown(p1);
			break;

		case MSG_GWM_TIMER :
			break;

		case MSG_GWM_CLICKED :
			if(p1 == PPV_CPOP_BTN_BASE_ID)
			{
				hErr = BUS_MGR_Destroy (0);
				if ( hErr != ERR_OK )
				{
					HxLOG_Print("[xmgr_cas_NaIppvBuyingPopup_Rcs] BUS_MGR_Destroy fail!!\n");
				}

				// reservation
				if(s_CreditPopIsThereRev == TRUE)
				{
					// create reminder
					BUS_MGR_Create(	"AP_RsvMain_Proc",
										APP_DIALOGBOX_PRIORITY,
										AP_RsvMain_Proc,
										HANDLE_NULL, (HINT32)&s_CreditPopRsvItem, 0, 0);
				}

			}
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY :
			eResult = Xmgr_cas_NaPpvCreditPopMsgGwmDestroy();
			break;

		/**********************************************************************
			APP MESSAGES
		**********************************************************************/
		default :
			break;
	}

	if ( eResult != ERR_BUS_NO_ERROR )
	{
		return eResult;
	}

	return BUS_ProcessMessageDefault (message, hAction, p1, p2, p3);
}


//----------------------------------------------------------------------------------------------------------------
STATIC void Xmgr_cas_NaPpvCreditPopSimpleInitObj(HINT32 nStatus, HINT32 nCredit)
{
	STATIC HUINT8		pTextMsg[256];
	OSD_Rect_t		 stRect;
	HERROR			hError = ERR_FAIL;
	TCreditAmount	CreditAmount = 0;
	TCreditStatus 		xStatus = CA_CREDIT_ERROR;

	// load data

	// make string
	HxSTD_memset(pTextMsg, 0, 256);
	if(nStatus == 1)
	{
		CreditAmount = nCredit;
		sprintf(pTextMsg, GetStrRsc(LOC_RCS_YOUR_NEW_CREDIT_ITEM_ID), CreditAmount/100,  CreditAmount%100);
	}
	else
	{
		hError = xmgr_cas_NaPpvGetCreditSimple(&CreditAmount, &xStatus);
		if(hError != ERR_OK || xStatus != CA_CREDIT_SUCCESS)
		{
			// 못 읽은 경우엔 구입 한 정보만 표시 한다.
			CreditAmount = 0;
			MWC_UTIL_DvbStrcat(pTextMsg, GetStrRsc(Xmgr_cas_NaPpvCreditPopGetPurchaseResultStringId(CA_PROCESSING_ERROR)));
		}
		else
		{
			sprintf(pTextMsg, GetStrRsc(LOC_RCS_YOUR_NEW_CREDIT_ITEM_ID), CreditAmount/100,  CreditAmount%100);
		}
	}

	/* Set Area. */
	Xmgr_cas_NaPpvCreditPopGetUiPosition(PPV_CPOP_MAIN_FRAME_ID,  &stRect);
	GWM_APP_SetAppArea (stRect.x, stRect.y, stRect.w, stRect.h);

	/* frame. */
	Xmgr_cas_NaPpvCreditPopGetUiPosition(PPV_CPOP_FRAME_ID, &stRect);
	GWM_Frame_Create (PPV_CPOP_FRAME_ID, &stRect, (char*)NULL);
	GWM_APP_SetObjectDrawMethod (PPV_CPOP_FRAME_ID, AP_LNF_WindowMD218Frame_Draw);

	/* text */
	Xmgr_cas_NaPpvCreditPopGetUiPosition(PPV_CPOP_TEXT_ID,  &stRect);
	GWM_Text_Create(PPV_CPOP_TEXT_ID, &stRect, pTextMsg);
	GWM_Text_SetSize(PPV_CPOP_TEXT_ID, FONT_SIZE_S);
	GWM_Text_SetAlign(PPV_CPOP_TEXT_ID, TEXT_ALIGN_CENTER);
	GWM_Text_ChangeColor(PPV_CPOP_TEXT_ID, COL(C_T_Dialog_MSG), 0);
	GWM_APP_SetObjectDrawMethod(PPV_CPOP_TEXT_ID, AP_LNF_PlainMultilineText_Draw);

	/* Button */
	Xmgr_cas_NaPpvCreditPopGetUiPosition(PPV_CPOP_BTN_BASE_ID,  &stRect);
	GWM_Button_Create (PPV_CPOP_BTN_BASE_ID, &stRect, (char*)GetStrRsc(LOC_DIALOG_OK_BUTTON_TEXT_ID));
	GWM_APP_SetObjectDrawMethod (PPV_CPOP_BTN_BASE_ID, AP_LNF_DialogButton_Draw);

	/* Set focus */
	GWM_Obj_SetFocus (PPV_CPOP_BTN_BASE_ID);
}

BUS_Result_t xproc_cas_NaPpvCreditPopupSimple_Rcs (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR 				hErr = ERR_FAIL;
	BUS_Result_t			eResult = ERR_BUS_NO_ERROR;

	switch (message)
	{
		/**********************************************************************f
			GWM MESSAGES
		**********************************************************************/

		case MSG_GWM_CREATE :
			Xmgr_cas_NaPpvCreditPopSimpleInitObj(p1, p2);
			break;

		case MSG_GWM_KEYDOWN :
			eResult = Xmgr_cas_NaPpvCreditPopMsgGwmKeyDown(p1);
			break;

		case MSG_GWM_TIMER :
			break;

		case MSG_GWM_CLICKED :
			if(p1 == PPV_CPOP_BTN_BASE_ID)
			{
				hErr = BUS_MGR_Destroy (0);
				if ( hErr != ERR_OK )
				{
					HxLOG_Print("[xmgr_cas_NaIppvBuyingPopup_Rcs] BUS_MGR_Destroy fail!!\n");
				}
			}
			return MESSAGE_BREAK;

		case MSG_GWM_DESTROY :
			eResult = Xmgr_cas_NaPpvCreditPopMsgGwmDestroy();
			break;

		/**********************************************************************
			APP MESSAGES
		**********************************************************************/
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

