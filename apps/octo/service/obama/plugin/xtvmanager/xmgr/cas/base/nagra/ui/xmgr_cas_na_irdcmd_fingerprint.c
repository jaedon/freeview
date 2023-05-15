//******************************************************************************
//	INCLUDE
//******************************************************************************
#include <db_svc.h>
#include <db_param.h>
#include <svc_sys.h>
#include <svc_pipe.h>
#include <svc_cas.h>
#include <isosvc.h>

#include <mgr_action.h>
#include <xmgr_cas_res_str.h>

#include <namgr_main.h>

#include "../local_include/_xmgr_cas_na.h"
#include "../local_include/_xmgr_cas_na_util.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#ifdef CONFIG_DEBUG
HUINT32 g_ApIRDFingerPrint = 0x00;
#endif
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




// dimension
// window
#define D_IRD_FINGER_DIALOG_X			326
#define D_IRD_FINGER_DIALOG_Y			251
#define D_IRD_FINGER_DIALOG_W			628
#define D_IRD_FINGER_DIALOG_H			218

// frame
#define D_IRD_FINGER_DIALOG_FRAME_X	0
#define D_IRD_FINGER_DIALOG_FRAME_Y	0
#define D_IRD_FINGER_DIALOG_FRAME_W	D_IRD_FINGER_DIALOG_W
#define D_IRD_FINGER_DIALOG_FRAME_H	D_IRD_FINGER_DIALOG_H

// text
#define D_IRD_FINGER_D_TXT_B_X			36
#define D_IRD_FINGER_D_TXT_B_Y			(50)
#define D_IRD_FINGER_D_TXT_B_W		(D_IRD_FINGER_DIALOG_W - 36*2)
#define D_IRD_FINGER_D_TXT_B_H			(D_IRD_FINGER_DIALOG_H - 36*2)

// button
#define D_IRD_FINGER_D_BTN_B_X			(D_IRD_FINGER_DIALOG_W/2 - 180/2)
#define D_IRD_FINGER_D_BTN_B_Y			(D_IRD_FINGER_DIALOG_H - 54 - 25)
#define D_IRD_FINGER_D_BTN_B_W		180
#define D_IRD_FINGER_D_BTN_B_H			54

#define IRD_FINGER_PRINT_MODE_NORMAL				0
#define IRD_FINGER_PRINT_TIMEOUT_TIMER_ID 			100
#define IRD_FINGER_PRINT_TIMEOUT_INTERVAL 			(30*1000)		//30sec


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/**@brief Reminder Conflict UI enum. */
typedef enum
{
	IRD_FINGER_MAIN_FRAME_ID = 0,
	IRD_FINGER_FRAME_ID,
	IRD_FINGER_TEXT_ID,
	IRD_FINGER_BTN_BASE_ID,
	IRD_FINGER_ID_MAX,
} eFingerUIID;


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
const STATIC OSD_Rect_t		s_stFingerRect[] =
{
		{	D_IRD_FINGER_DIALOG_X,				D_IRD_FINGER_DIALOG_Y, 				D_IRD_FINGER_DIALOG_W,				D_IRD_FINGER_DIALOG_H			},
		{	D_IRD_FINGER_DIALOG_FRAME_X,		D_IRD_FINGER_DIALOG_FRAME_Y,		D_IRD_FINGER_DIALOG_FRAME_W,		D_IRD_FINGER_DIALOG_FRAME_H	},
		{	D_IRD_FINGER_D_TXT_B_X,				D_IRD_FINGER_D_TXT_B_Y,				D_IRD_FINGER_D_TXT_B_W,				D_IRD_FINGER_D_TXT_B_H		},
		{	D_IRD_FINGER_D_BTN_B_X,				D_IRD_FINGER_D_BTN_B_Y,				D_IRD_FINGER_D_BTN_B_W,				D_IRD_FINGER_D_BTN_B_H		},
};

STATIC HUINT32	s_nFingerMode = IRD_FINGER_PRINT_MODE_NORMAL;
/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define _________EDIT_FUNTIONS____________

STATIC void local_IRD_NA_FingerGetUiPosition(eFingerUIID eUiId, OSD_Rect_t *rect)
{
	if ( eUiId >= IRD_FINGER_ID_MAX )
	{
		HxSTD_memcpy(rect, &s_stFingerRect[IRD_FINGER_ID_MAX -1], sizeof(OSD_Rect_t));
	}
	else
	{
		HxSTD_memcpy(rect, &s_stFingerRect[eUiId], sizeof(OSD_Rect_t));
	}
}

#if 0
STATIC BUS_Result_t local_IRD_NA_Finger_LNF_DialogDisableButton_Draw(OSD_Screen_t *pOsdScreen, OSD_Area_t *appArea,
										OSD_Rect_t *objRect, GWM_GuiObject_t object, void *arg, int bUiTop)
{
	BUTTON_OBJECT	btn = (BUTTON_OBJECT)arg;
	int				x, y, x1, y1, x2, y2, w, h;
	HUINT32			status, length = 0;
	OSD_Font_t		*pFont = NULL;
	char 			*str = NULL;
	OSD_Color_t		txtCol = COLOR_WHITE;
	HUINT32			fontSize = FONT_SIZE_M;
	OSD_StockImage_t		img;
	OSD_Rect_t				dstRect;

	x1 = appArea->x1 + objRect->x;
	y1 = appArea->y1 + objRect->y;
	x2 = appArea->x1 + objRect->x + objRect->w;
	y2 = appArea->y1 + objRect->y + objRect->h;
	w = x2 - x1;
	h = y2 - y1;

	status = GWM_Button_GetStatus(btn);
	if (status & GUIOBJ_DISABLED)
	{
		x1 += 3;
		w -= 6;
		y1 += 3;
		h -= 6;

		if (!AP_StockImg_GetImage(e123_1_00_BTN_D1_N, &img))
		{
			OSD_GFX_SetRect(&dstRect, x1, y1, img.width, img.height);
			OSD_GFX_FB_Blend((unsigned long)img.hBuffer, img.width, img.height, pOsdScreen, &dstRect);
		}
		txtCol = COL(C_T_Disable);
		fontSize = FONT_SIZE_S;
	}
	else if (status & GUIOBJ_HILIGHTED)
	{	/* got focused */
		if (!AP_StockImg_GetImage(e123_1_00_BTN_D1_C, &img))
		{
			if (status & GUIOBJ_PUSHED)
			{
				OSD_GFX_SetRect(&dstRect, x1, y1 + 2, img.width, img.height);
			}
			else
			{
				OSD_GFX_SetRect(&dstRect, x1, y1, img.width, img.height);
			}

			OSD_GFX_FB_Blend((unsigned long)img.hBuffer, img.width, img.height, pOsdScreen, &dstRect);
		}
		txtCol = COL(C_T_Enable);
		fontSize = FONT_SIZE_M;
	}
	else
	{	/* normal, encrypted */
		x1 += 3;
		w -= 6;
		y1 += 3;
		h -= 6;

		if (!AP_StockImg_GetImage(e123_1_00_BTN_D1_N, &img))
		{
			OSD_GFX_SetRect(&dstRect, x1, y1, img.width, img.height);
			OSD_GFX_FB_Blend((unsigned long)img.hBuffer, img.width, img.height, pOsdScreen, &dstRect);
		}
		txtCol = COLOR_WHITE;
		fontSize = FONT_SIZE_S;
	}

	pFont = OSD_FONT_Get(eFont_SystemNormal);
	if (!OSD_FONT_SetSize(pFont, fontSize))
	{
		y = y1 + ((h - pFont->height)>>1);//+ (h /2 - pFont->height / 2);		/* mid & center aligned */

		str = GWM_Button_GetTitlePtr(btn);
		if (str != NULL)
		{
			length = OSD_GFX_GetStringWidth(pFont, str);
			x =  x1 + ((w - length)>>1);
			OSD_GFX_DrawString(pOsdScreen, pFont, x, y, str, txtCol);
		}
	}


	OSD_FONT_Release (pFont);

	return ERR_OK;
}
#endif

STATIC void local_IRD_NA_FingerInitObj(HUINT32 nMode)
{
	STATIC HUINT8			pTextMsg[256];
	OSD_Rect_t		 	stRect;
	HERROR				hErr = ERR_FAIL;
	TSystemInfoStatus 	xStatus = CA_SYSTEM_INFO_ERROR;
	TIrdInfo 				*pxIrdInfo = NULL;
	TUnsignedInt8 		xNumberOfSmartcards = 0;
	TSmartcardInfoItem 	*pxSmartcardInfoArray = NULL;

	// load data
	hErr = xmgr_cas_NaGetSystemScInfo(&xStatus, &pxIrdInfo, &xNumberOfSmartcards, &pxSmartcardInfoArray);

	if ( hErr != ERR_OK
		|| xStatus != CA_SYSTEM_INFO_SUCCESS
		|| pxIrdInfo == NULL
		|| xNumberOfSmartcards == 0
		|| pxSmartcardInfoArray == NULL )
	{
		HxLOG_Print("ERROR: Fail On Getting System Information\n");
		BUS_MGR_Destroy (0);
		return;
	}

	HxSTD_memset(pTextMsg, 0, 256);
	sprintf((char*)pTextMsg, "IRD S/N : %s\nSmartcard S/N : %s", pxIrdInfo->pSerialNumber, pxSmartcardInfoArray[0].pSerialNumber);


	/* Set Area. */
	local_IRD_NA_FingerGetUiPosition(IRD_FINGER_MAIN_FRAME_ID,  &stRect);
	//GWM_APP_SetAppArea (stRect.x, stRect.y, stRect.w, stRect.h);

	/* frame. */
	local_IRD_NA_FingerGetUiPosition(IRD_FINGER_FRAME_ID, &stRect);
	//GWM_Frame_Create (IRD_FINGER_FRAME_ID, &stRect, (char*)NULL);
	//GWM_APP_SetObjectDrawMethod (IRD_FINGER_FRAME_ID, AP_LNF_WindowMD218Frame_Draw);

	/* text */
	local_IRD_NA_FingerGetUiPosition(IRD_FINGER_TEXT_ID,  &stRect);
	//GWM_Text_Create(IRD_FINGER_TEXT_ID, &stRect, pTextMsg);
	//GWM_Text_SetSize(IRD_FINGER_TEXT_ID, FONT_SIZE_S);
	//GWM_Text_SetAlign(IRD_FINGER_TEXT_ID, TEXT_ALIGN_CENTER);
	//GWM_Text_ChangeColor(IRD_FINGER_TEXT_ID, COL(C_T_Dialog_MSG), 0);
	//GWM_APP_SetObjectDrawMethod(IRD_FINGER_TEXT_ID, AP_LNF_PlainMultilineText_Draw);

	/* Button */
	local_IRD_NA_FingerGetUiPosition(IRD_FINGER_BTN_BASE_ID,  &stRect);
	//GWM_Button_Create (IRD_FINGER_BTN_BASE_ID, &stRect, (char*)GetStrRsc(LOC_DIALOG_OK_BUTTON_TEXT_ID));
	//GWM_APP_SetObjectDrawMethod (IRD_FINGER_BTN_BASE_ID, local_IRD_NA_Finger_LNF_DialogDisableButton_Draw);

	// normal mode 인 경우만
	if(nMode == IRD_FINGER_PRINT_MODE_NORMAL)
	{
		/* Set focus */
		//GWM_Obj_SetFocus (IRD_FINGER_BTN_BASE_ID);
	}
	else
	{
		//GWM_Obj_DisableObject(IRD_FINGER_BTN_BASE_ID );
		//GWM_Obj_SetFocus (IRD_FINGER_FRAME_ID);
	}

	//1 To do 여기에서 Appication으로 전달할 Data를 만들자.
}

STATIC BUS_Result_t local_IRD_NA_FingerMsgGwmDestroy(void)
{
	// kill timer
	BUS_KillTimer(IRD_FINGER_PRINT_TIMEOUT_TIMER_ID);

	// do finger print mode
	if(s_nFingerMode != IRD_FINGER_PRINT_MODE_NORMAL)
	{
		HxLOG_Print("Finger Print : Force Standby...\n");

#if 0
		SVC_SYS_OnOffKeyInput(FALSE); /* 전원키가 또 들어와서 꼬이는 것 방지 */
#endif

		MGR_ACTION_RequestShutdown();
	}

	s_nFingerMode = IRD_FINGER_PRINT_MODE_NORMAL;
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t local_IRD_NA_FingerMsgGwmKeyDown(HINT32 p1)
{
	//HERROR 		hErr = ERR_FAIL;

#if 0
	switch (p1)
	{
		case KEY_INFO :
		case KEY_GUIDE :
		case KEY_MENU :
		case KEY_OPT :
		case KEY_MEDIA :
		case KEY_CHLIST :
			return MESSAGE_BREAK;

		case KEY_BACK :
		case KEY_EXIT :
			hErr = BUS_MGR_Destroy (0);
			if ( hErr != ERR_OK )
			{
				HxLOG_Print("[local_IRD_NA_FingerMsgGwmKeyDown] BUS_MGR_Destroy fail!!(KEY_DOWN)\n");
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
#endif

	return ERR_BUS_NO_ERROR;
}

BUS_Result_t xmgr_cas_NaIRD_FingerPrint_Proc (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR 				hErr = ERR_FAIL;
	BUS_Result_t			eResult = ERR_BUS_NO_ERROR;

	switch (message)
	{
		/**********************************************************************f
			GWM MESSAGES
		**********************************************************************/

		case eMEVT_BUS_CREATE :
			s_nFingerMode = (HUINT32)p1;
			HxLOG_Print("[%s] Finger Print Mode [%d]\n", __FUNCTION__, s_nFingerMode);
			BUS_SetTimer(IRD_FINGER_PRINT_TIMEOUT_TIMER_ID, IRD_FINGER_PRINT_TIMEOUT_INTERVAL);
			local_IRD_NA_FingerInitObj(s_nFingerMode);
			break;

		case MSG_OAPI_CASUI_INPUT_NOTIFY :
			eResult = local_IRD_NA_FingerMsgGwmKeyDown(p1);
			break;

		case eMEVT_BUS_TIMER :
			if(p1 == IRD_FINGER_PRINT_TIMEOUT_TIMER_ID)
			{
				hErr = BUS_MGR_Destroy (0);
				if ( hErr != ERR_OK )
				{
					HxLOG_Print("[xmgr_cas_NaIRD_FingerPrint_Proc] BUS_MGR_Destroy fail!!(MSG_GWM_TIMER)\n");
				}
			}
			break;

#if 0
		case MSG_BUS_CLICKED :
			if(s_nFingerMode == IRD_FINGER_PRINT_MODE_NORMAL && p1 == IRD_FINGER_BTN_BASE_ID)
			{
				hErr = BUS_MGR_Destroy (0);
				if ( hErr != ERR_OK )
				{
					HxLOG_Print("[xmgr_cas_NaIRD_FingerPrint_Proc] BUS_MGR_Destroy fail!!(MSG_GWM_CLICKED)\n");
				}
			}
			return MESSAGE_BREAK;
#endif

		case eMEVT_BUS_DESTROY :
			eResult = local_IRD_NA_FingerMsgGwmDestroy();
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

