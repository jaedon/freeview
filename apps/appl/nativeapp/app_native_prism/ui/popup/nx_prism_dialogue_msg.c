/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/******************************************************************************/
/**
 * @file	  		nx_prism_dialogue_msg.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>

#include <nx_prism_app.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Status_Message_functions______________________________________
/******************************************************************************/

static ONDK_Result_t 	nx_statusmessage_SetUiObject(HUINT8 *pszMessage, HUINT32 p2, HUINT32 p3)
{
	HINT32				lLineCnt = 0, lAddLine = 0, y = 0, h = 0;
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };


	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, pszMessage, NX_PRISM_UI_DLG_STATUE_MSG_TEXT_W);

	if (lLineCnt < NX_PRISM_UI_DLG_STATUE_MSG_TEXT_MULTI_LINE)
	{
		h = NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_H;
		ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_X, NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_Y,
									NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_W, h);
		lAddLine = 0;
	}
	else
	{
		lAddLine = (lLineCnt - NX_PRISM_UI_DLG_STATUE_MSG_TEXT_MULTI_LINE);

		y = NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_MULTI_LINE_Y - (lAddLine * NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_Y_MG);
		y = (y < 0) ? 0 : y ;
		h = NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_MULTI_LINE_H + (lAddLine * NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_H_MG);

		ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_MULTI_LINE_X, y,
									NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_MULTI_LINE_W, h);
	}

	/* background */
	stRect = ONDK_Rect(0, 0, NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_W, h);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_DLG_STATUE_MSG_BG_ID, &stRect, COL(C_G_Bang_90));

	/* TOP Line */
	stRect = ONDK_Rect(0, 0, NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_W, NX_PRISM_UI_DLG_STATUE_MSG_LINE_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_DLG_STATUE_MSG_LINE_TOP_ID, &stRect, COL(C_G_Yoyo01));

	/* BOTTOM Line */
	stRect = ONDK_Rect(0, (h - NX_PRISM_UI_DLG_STATUE_MSG_LINE_H), NX_PRISM_UI_DLG_STATUE_MSG_WINDOW_W, NX_PRISM_UI_DLG_STATUE_MSG_LINE_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_DLG_STATUE_MSG_LINE_BOTTOM_ID, &stRect, COL(C_G_Yoyo01));

	/* ICON  */
	if (lLineCnt < NX_PRISM_UI_DLG_STATUE_MSG_TEXT_MULTI_LINE)
	{
		y = NX_PRISM_UI_DLG_STATUE_MSG_ICON_Y;
	}
	else
	{
		y = NX_PRISM_UI_DLG_STATUE_MSG_ICON_MULTI_LINE_Y + (lAddLine * NX_PRISM_UI_DLG_STATUE_MSG_ICON_Y_MG) ;
	}
	stRect = ONDK_Rect(NX_PRISM_UI_DLG_STATUE_MSG_ICON_X, y, NX_PRISM_UI_DLG_STATUE_MSG_ICON_W, NX_PRISM_UI_DLG_STATUE_MSG_ICON_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_UI_DLG_STATUE_MSG_ICON_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_UI_DLG_STATUE_MSG_ICON_ID, (HCHAR*)RES_311_00_DW_ICON_STATUS_PNG);

	/* Draw Message TEXT */
	y = (lLineCnt < NX_PRISM_UI_DLG_STATUE_MSG_TEXT_MULTI_LINE) ? NX_PRISM_UI_DLG_STATUE_MSG_TEXT_Y :  NX_PRISM_UI_DLG_STATUE_MSG_TEXT_MULTI_LINE_Y ;
	stRect = ONDK_Rect(NX_PRISM_UI_DLG_STATUE_MSG_TEXT_X, y,
						NX_PRISM_UI_DLG_STATUE_MSG_TEXT_W, (NX_PRISM_UI_DLG_STATUE_MSG_TEXT_LS * lLineCnt) + (NX_PRISM_FONT_VSPACE * (lLineCnt - 1)));
	if (lLineCnt == 1)
	{
		stRect.h += NX_PRISM_FONT_VSPACE;
	}
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_DLG_STATUE_MSG_TEXT_ID, &stRect, (HCHAR*)pszMessage);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_DLG_STATUE_MSG_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_DLG_STATUE_MSG_TEXT_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_DLG_STATUE_MSG_TEXT_ID, (TEXT_MULTILINED | TEXT_WORDWRAP));
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_DLG_STATUE_MSG_TEXT_ID, COL(C_T_Yoyo01), COL(C_T_Bang_100));

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	// no focus
	ONDK_GWM_APP_SetAttribute(GWM_HiddenUI_App);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_DIALOGUE_StatusMessage_Destroy(void)
{
	ONDK_GWM_App_t	pGwmApp = NULL;

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_DIALOGUE_StatusMessage_Proc);
	if (pGwmApp != NULL)
	{
		return ONDK_GWM_APP_Destroy(NX_PRISM_DIALOGUE_StatusMessage_Proc);
	}
	else
	{
		return	ONDK_RET_OK;
	}
}


ONDK_Result_t	NX_PRISM_DIALOGUE_StatusMessage_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;


	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			/* p1 : Message, p2 : NULL, p2 : NULL */
			eResult = nx_statusmessage_SetUiObject((HUINT8*)p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			break;
		case MSG_GWM_DESTROY:
			break;
		case MSG_GWM_HAPI_MESSAGE:
			break;
		default:
			break;
	}
	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}

//ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "NX_PRISM_DIALOGUE_StatusMessage_Proc", APP_CAS_PRIORITY, NX_PRISM_DIALOGUE_StatusMessage_Proc, 0, (HINT32)ONDK_GetString(RES_CANCEL_THIS_PROGRAMME_ID), 0, 0);


/******************************************************************************/
#define _________Response_Message_functions___________________________________
/******************************************************************************/

static ONDK_Result_t 	nx_ResponseMessage_SetUiObject(HUINT8 *pszMessage, HUINT32 p2, HUINT32 ulTimeOut)
{
	HINT32			lLineCnt = 0, lAddLine = 0, h = 0;
	HUINT32			ulExpTime = 0;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };


	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, pszMessage, NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_W);

	if (lLineCnt < NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_MULTI_LINE)
	{
		h = NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_H;
		ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_X, NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_Y,
									NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_W, h);
		lAddLine = 0;
	}
	else
	{
		lAddLine = (lLineCnt - NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_MULTI_LINE);
		h = NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_MULTI_LINE_H + (lAddLine *NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_H_MG);

		ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_MULTI_LINE_X, NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_MULTI_LINE_Y,
									NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_MULTI_LINE_W, h);
	}

	/* Draw background */
	stRect = ONDK_Rect(0, 0, NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_W, h);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_DLG_RESPONSE_MSG_BG_ID, &stRect, COL(C_G_Bang_90));

	/* BOTTOM Line */
	stRect = ONDK_Rect(0, (h - NX_PRISM_UI_DLG_RESPONSE_MSG_LINE_H), NX_PRISM_UI_DLG_RESPONSE_MSG_WINDOW_W, NX_PRISM_UI_DLG_RESPONSE_MSG_LINE_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_DLG_RESPONSE_MSG_LINE_BOTTOM_ID, &stRect, COL(C_G_Whistle_100));

	/* Draw Message TEXT */
	stRect = ONDK_Rect(NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_X, NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_Y,
						NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_W, (NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_LS * lLineCnt) + NX_PRISM_FONT_VSPACE);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_ID, &stRect, (HCHAR*)pszMessage);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_ID, (TEXT_ALIGN_CENTER | TEXT_MULTILINED | TEXT_WORDWRAP));
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_DLG_RESPONSE_MSG_TEXT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Set Expier Time out */
	ulExpTime = (ulTimeOut == 0) ? NX_COMMON_DIALOGUE_EXP_TIMEOUT : ulTimeOut ;
	ONDK_GWM_SetTimer(NX_PRISM_UI_DLG_RESPONSE_MSG_EXP_TIMER_ID, ulExpTime);

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(ulTimeOut);

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_DIALOGUE_ResponseMessage_Destroy(void)
{
	ONDK_GWM_App_t	pGwmApp = NULL;

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_DIALOGUE_ResponseMessage_Proc);
	if (pGwmApp != NULL)
	{
		return ONDK_GWM_APP_Destroy(NX_PRISM_DIALOGUE_ResponseMessage_Proc);
	}
	else
	{
		return	ONDK_RET_OK;
	}
}


ONDK_Result_t	NX_PRISM_DIALOGUE_ResponseMessage_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;


	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			/* p1 : Message, p2 : NULL, p2 : TIME OUT */
			eResult = nx_ResponseMessage_SetUiObject((HUINT8*)p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			break;
		case MSG_GWM_TIMER:
			if (p1 == NX_PRISM_UI_DLG_RESPONSE_MSG_EXP_TIMER_ID)
			{
				ONDK_GWM_APP_Destroy(0);
				return	ONDK_MESSAGE_BREAK;
			}
			break;
		case MSG_GWM_DESTROY:
			ONDK_GWM_KillTimer(NX_PRISM_UI_DLG_RESPONSE_MSG_EXP_TIMER_ID);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			break;
		default:
			break;
	}

	if (eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}

//ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "NX_PRISM_DIALOGUE_ResponseMessage_Proc", APP_CAS_PRIORITY, NX_PRISM_DIALOGUE_ResponseMessage_Proc, 0, (HINT32)ONDK_GetString(RES_CANCEL_THIS_PROGRAMME_ID), 0, 0);


/******************************************************************************/
#define _________Confirm_Message_functions___________________________________
/******************************************************************************/

static ONDK_Result_t 	nx_Message_SetUiObject(NXUI_DialogInstance_t *pstContents)
{
	HINT32				lLineCnt = 0, lAddLine = 0, y = 0, h = 0, lOffset_y = 0;
	ONDK_Rect_t			stRect = { 0, 0, 0, 0 };


	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, pstContents->pszDialogMsg, NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_W);

	if (lLineCnt < NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_MULTI_LINE)
	{
		h = NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_H;
		ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_X, NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_Y,
									NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_W, h);
		lAddLine = 0;
	}
	else
	{
		lAddLine = (lLineCnt - NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_MULTI_LINE);

		y = NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_MULTI_LINE_Y - (lAddLine *NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_Y_MG);
		y = (y < 0) ? 0 : y ;
		h = NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_MULTI_LINE_H + (lAddLine *NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_H_MG);

		ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_MULTI_LINE_X, y,
									NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_MULTI_LINE_W, h);
	}

	/* background */
	stRect = ONDK_Rect(0, 0, NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_W, h);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_DLG_CONFIRM_MSG_BG_ID, &stRect, COL(C_G_Bang_90));

	/* TOP Line */
	stRect = ONDK_Rect(0, 0, NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_W, NX_PRISM_UI_DLG_CONFIRM_MSG_LINE_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_DLG_CONFIRM_MSG_LINE_TOP_ID, &stRect, COL(C_G_Whistle_100));

	/* BOTTOM Line */
	stRect = ONDK_Rect(0, (h - NX_PRISM_UI_DLG_CONFIRM_MSG_LINE_H), NX_PRISM_UI_DLG_CONFIRM_MSG_WINDOW_W, NX_PRISM_UI_DLG_CONFIRM_MSG_LINE_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_DLG_CONFIRM_MSG_LINE_BOTTOM_ID, &stRect, COL(C_G_Whistle_100));

	if (pstContents->ulDlgAttributes & NX_DLG_TYPE_TITLE)
	{
		// TODO: Not Used attribute. Add Draw Function if you need
	}

	/* Draw Message TEXT */
	y = (lLineCnt < NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_MULTI_LINE) ? NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_Y :  NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_MULTI_LINE_Y ;
	stRect = ONDK_Rect(NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_X, y,
						NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_W, (NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_LS * lLineCnt) + NX_PRISM_FONT_VSPACE);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_ID, &stRect, (HCHAR*)pstContents->pszDialogMsg);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_ID, (TEXT_ALIGN_CENTER | TEXT_MULTILINED | TEXT_WORDWRAP));
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* button attributes */
	y = (lLineCnt < NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_MULTI_LINE) ? NX_PRISM_UI_DLG_CONFIRM_BTN_Y :  NX_PRISM_UI_DLG_CONFIRM_BTN_MULTI_LINE_Y ;
	lOffset_y = (lAddLine*NX_PRISM_UI_DLG_CONFIRM_MSG_BUTTON_Y_MG);
	y += lOffset_y;

	switch (pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK)
	{
		case NX_DLG_YES_NO:
		case NX_DLG_OK_CANCEL:
		case NX_DLG_CUSTOM_LR:
			if ((pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK) == NX_DLG_YES_NO)
			{
				pstContents->pszButtonL = (HUINT8 *)ONDK_GetString(RES_YES_01_ID);
				pstContents->pszButtonR = (HUINT8 *)ONDK_GetString(RES_NO_03_ID);
			}
			else if ((pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK) == NX_DLG_OK_CANCEL)
			{
				pstContents->pszButtonL = (HUINT8 *)ONDK_GetString(RES_OK_ID);
				pstContents->pszButtonR = (HUINT8 *)ONDK_GetString(RES_CANCEL_03_ID);
			}
			else	//DLG_CUSTOM_LR
			{
				// Alreay Set custom String
			}

			// left button
			stRect = ONDK_Rect(NX_PRISM_UI_DLG_CONFIRM_BTN_L_X, y, NX_PRISM_UI_DLG_CONFIRM_BTN_L_W, NX_PRISM_UI_DLG_CONFIRM_BTN_L_H);
			ONDK_GWM_COM_Button_Create(NX_PRISM_UI_DLG_CONFIRM_BTN_L_ID, &stRect, (char *)pstContents->pszButtonL);
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_CONFIRM_BTN_L_ID, NX_PRISM_COMP_DrawButton_A1);

			// right button
			stRect = ONDK_Rect(NX_PRISM_UI_DLG_CONFIRM_BTN_R_X, y, NX_PRISM_UI_DLG_CONFIRM_BTN_R_W, NX_PRISM_UI_DLG_CONFIRM_BTN_R_H);
			ONDK_GWM_COM_Button_Create(NX_PRISM_UI_DLG_CONFIRM_BTN_R_ID, &stRect, (char *)pstContents->pszButtonR);
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_CONFIRM_BTN_R_ID, NX_PRISM_COMP_DrawButton_A1);

			if ((pstContents->ulDlgAttributes & NX_DLG_TYPE_FOCUS_CANCEL) || (pstContents->ulDlgAttributes & NX_DLG_TYPE_FOCUS_NO)
				|| (pstContents->ulDlgAttributes & NX_DLG_TYPE_FOCUS_RIGHT))
			{
				ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_DLG_CONFIRM_BTN_R_ID);
			}
			else
			{
				ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_DLG_CONFIRM_BTN_L_ID);
			}
			break;

		case NX_DLG_CUSTOM_3B:
			// left button
			stRect = ONDK_Rect(NX_PRISM_UI_DLG_CONFIRM_BTN_X - NX_PRISM_UI_DLG_CONFIRM_MSG_BUTTON_MG - NX_PRISM_UI_DLG_CONFIRM_BTN_W,
								y, NX_PRISM_UI_DLG_CONFIRM_BTN_W, NX_PRISM_UI_DLG_CONFIRM_BTN_H);
			ONDK_GWM_COM_Button_Create(NX_PRISM_UI_DLG_CONFIRM_BTN_L_ID, &stRect, (char *)pstContents->pszButtonL);
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_CONFIRM_BTN_L_ID, NX_PRISM_COMP_DrawButton_A1);

			// middle button
			stRect = ONDK_Rect(NX_PRISM_UI_DLG_CONFIRM_BTN_X, y, NX_PRISM_UI_DLG_CONFIRM_BTN_W, NX_PRISM_UI_DLG_CONFIRM_BTN_H);
			ONDK_GWM_COM_Button_Create(NX_PRISM_UI_DLG_CONFIRM_BTN_M_ID, &stRect, (char *)pstContents->pszButtonM);
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_CONFIRM_BTN_M_ID, NX_PRISM_COMP_DrawButton_A1);

			// right button
			stRect = ONDK_Rect(NX_PRISM_UI_DLG_CONFIRM_BTN_X + NX_PRISM_UI_DLG_CONFIRM_BTN_W + NX_PRISM_UI_DLG_CONFIRM_MSG_BUTTON_MG,
								y, NX_PRISM_UI_DLG_CONFIRM_BTN_W, NX_PRISM_UI_DLG_CONFIRM_BTN_H);
			ONDK_GWM_COM_Button_Create(NX_PRISM_UI_DLG_CONFIRM_BTN_R_ID, &stRect, (char *)pstContents->pszButtonR);
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_CONFIRM_BTN_R_ID, NX_PRISM_COMP_DrawButton_A1);

			if (pstContents->ulDlgAttributes & NX_DLG_TYPE_FOCUS_RIGHT)
			{
				ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_DLG_CONFIRM_BTN_R_ID);
			}
			else
			{
				ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_DLG_CONFIRM_BTN_L_ID);
			}

			break;

		case NX_DLG_NO_BUTTON:
			NX_APP_Info("nx_Message_SetUiObject() No Button!! \n");
			break;

		case NX_DLG_CUSTOM_1B:
			if (pstContents->pszButtonM == NULL)
			{
				pstContents->pszButtonM = (HUINT8 *)ONDK_GetString(RES_OK_ID);
			}
			stRect = ONDK_Rect(NX_PRISM_UI_DLG_CONFIRM_BTN_X, y, NX_PRISM_UI_DLG_CONFIRM_BTN_W, NX_PRISM_UI_DLG_CONFIRM_BTN_H);
			ONDK_GWM_COM_Button_Create(NX_PRISM_UI_DLG_CONFIRM_BTN_M_ID, &stRect, (HCHAR*)pstContents->pszButtonM);
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_CONFIRM_BTN_M_ID, NX_PRISM_COMP_DrawButton_A1);
			ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_DLG_CONFIRM_BTN_M_ID);
			break;

		case NX_DLG_OK:
		default:
			pstContents->pszButtonM = (HUINT8 *)ONDK_GetString(RES_OK_ID);
			stRect = ONDK_Rect(NX_PRISM_UI_DLG_CONFIRM_BTN_X, y, NX_PRISM_UI_DLG_CONFIRM_BTN_W, NX_PRISM_UI_DLG_CONFIRM_BTN_H);
			ONDK_GWM_COM_Button_Create(NX_PRISM_UI_DLG_CONFIRM_BTN_M_ID, &stRect, (char *)pstContents->pszButtonM);
			ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_CONFIRM_BTN_M_ID, NX_PRISM_COMP_DrawButton_A1);
			ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_DLG_CONFIRM_BTN_M_ID);
			break;
	}

	/* Set OSD Time out  */
	if (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK)
	{
		pstContents->ulTimeout = (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK) * NX_COMMON_TIME_SECOND;
		ONDK_GWM_SetTimer(NX_PRISM_UI_DLG_CONFIRM_MSG_EXP_TIMER_ID, pstContents->ulTimeout);
	}
	else
	{
		pstContents->ulTimeout = 0;
	}

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_DIALOGUE_SetCustomButton(NXUI_DialogInstance_t *pstContents, HUINT8 *pucLeftBt, HUINT8 *pucMiddleBt, HUINT8 *pucRightBt)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return	ONDK_RET_INITFAIL;
	}

	pstContents->pszButtonL = pucLeftBt;
	pstContents->pszButtonM = pucMiddleBt;
	pstContents->pszButtonR = pucRightBt;

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_DIALOGUE_SetParentApp(NXUI_DialogInstance_t *pstContents, ONDK_GWM_Callback_t parentApp)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return	ONDK_RET_INITFAIL;
	}

	if (parentApp == NULL)
	{
		NX_APP_Error("ERROR!!! parentApp was NULL!!\n");
		return	ONDK_RET_INITFAIL;
	}

	pstContents->parentAppCallback = parentApp;

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_DIALOGUE_SetDialogAttributes(NXUI_DialogInstance_t *pstContents, HUINT32 ulDlgAttr)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return	ONDK_RET_INITFAIL;
	}

	pstContents->ulDlgAttributes = ulDlgAttr;

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_DIALOGUE_SetDialogTitle(NXUI_DialogInstance_t *pstContents, HUINT8 *pucDlgTitle)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return	ONDK_RET_INITFAIL;
	}

	if (pucDlgTitle == NULL)
	{
		/* title은 NULL일 수도 있지. */
		pstContents->pszDialogTitle = (HUINT8*)" ";
	}
	else
	{
		pstContents->pszDialogTitle = pucDlgTitle;
	}

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_DIALOGUE_SetDialogMessage(NXUI_DialogInstance_t *pstContents, HUINT8 *pucDlgMsg)
{
	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return ONDK_RET_INITFAIL;
	}

	if (pucDlgMsg == NULL)
	{
		NX_APP_Error("ERROR!!! pucDlgMsg was NULL!!\n");
		return ONDK_RET_INITFAIL;
	}

	pstContents->pszDialogMsg = pucDlgMsg;

	return	ONDK_RET_OK;
}


ONDK_Result_t 	NX_PRISM_DIALOGUE_SetDialog(NXUI_DialogInstance_t *pstContents, ONDK_GWM_Callback_t parentApp, HUINT32 ulDlgAttr, HUINT8 *pDlgTitle, HUINT8 *pDlgMsg)
{
	HUINT32		ulDlgWhoseId = 0;

	if (pstContents)
	{
		// 동일한 APP 에서 2개 이상의 용도로 Dialog 를 사용하는 경우에 ulDlgId 값을 설정한다.
		ulDlgWhoseId = pstContents->ulDlgWhoseId;
		NX_PRISM_DIALOGUE_SetInitContents(pstContents);
		pstContents->ulDlgWhoseId = ulDlgWhoseId;

		NX_PRISM_DIALOGUE_SetParentApp(pstContents, parentApp);
		NX_PRISM_DIALOGUE_SetDialogAttributes(pstContents, ulDlgAttr);
		NX_PRISM_DIALOGUE_SetDialogTitle(pstContents, pDlgTitle);
		NX_PRISM_DIALOGUE_SetDialogMessage(pstContents, pDlgMsg);
	}

	return ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_DIALOGUE_SetInitContents(NXUI_DialogInstance_t *pstContents)
{
	HxSTD_memset(pstContents, 0, sizeof(NXUI_DialogInstance_t));

	return	ONDK_RET_OK;
}


ONDK_Result_t	NX_PRISM_DIALOGUE_Message_Proc(NXUI_DialogInstance_t *pstContents, HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lObjId = -1;
	NX_DLG_ClickedButton_t	eDlgBtnType = eNX_DLG_CLICKED_BTN_NONE;
	NXUI_DialogInstance_t	stBackupInstance;

	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return	ONDK_MESSAGE_BREAK;
	}

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			nx_Message_SetUiObject(pstContents);
			pstContents->thisAppCallback = ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis());
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_DLG_INPUTED_KEYDOWN, (Handle_t)NULL,
									p1, 0, (HINT32)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()));
			if (pstContents->ulTimeout > 0)
			{
				ONDK_GWM_ResetTimer(NX_PRISM_UI_DLG_CONFIRM_MSG_EXP_TIMER_ID);
			}

			switch (p1)
			{
				case KEY_IP_BROWSER:
				case KEY_MEDIA:
				case KEY_GUIDE:
				case KEY_SCHEDULE:
				case KEY_MENU:
					if ((pstContents->ulDlgAttributes & NX_DLG_TYPE_NO_EXIT) || (pstContents->ulDlgAttributes & NX_DLG_TYPE_NOT_BTN_CLICK_MASK))
					{
						return ONDK_MESSAGE_BREAK;
					}
					else
					{
						ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_DLG_CANCELED, (Handle_t)NULL, eNX_DLG_CLICKED_BTN_RIGHT,
												(HINT32)pstContents->ulDlgWhoseId, (HINT32)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()));
						ONDK_GWM_APP_Destroy(0);
					}
					return ONDK_MESSAGE_PASS;

				case KEY_BACK:
				case KEY_EXIT:
					if ((pstContents->ulDlgAttributes & NX_DLG_TYPE_NO_EXIT) || (pstContents->ulDlgAttributes & NX_DLG_TYPE_NOT_BTN_CLICK_MASK))
					{
						/* modal dialog should ignore the exit key */
						return ONDK_MESSAGE_BREAK;
					}
					else
					{
						// Backup
						HxSTD_memcpy(&stBackupInstance, pstContents, sizeof(NXUI_DialogInstance_t));

						/* notify the dialog exit */
						ONDK_GWM_APP_Destroy(0);
						ONDK_GWM_SendMessage(stBackupInstance.parentAppCallback, MSG_APP_DLG_CANCELED, (Handle_t)NULL, eNX_DLG_CLICKED_BTN_RIGHT,
												(int)stBackupInstance.ulDlgWhoseId, (int)stBackupInstance.thisAppCallback);
						return ONDK_MESSAGE_BREAK;
					}
					break;
				case KEY_STANDBY:
					/* 어떤 경우에라도 stand by key는 먹어야 하므로 pass */
					NX_APP_Info("NX_PRISM_DIALOGUE_Message_Proc() MSG_GWM_KEYDOWN: KEY_STANDBY\n");
					ONDK_GWM_APP_Destroy(0);
					return ONDK_MESSAGE_PASS;

				/*
					일부 dialog가 떠 있는 mother app에 focus object가
					default로 처리하는 key일 수 있으므로 이는 내려주면 안된다. wjmoh
				*/
				case KEY_0:
				case KEY_1:
				case KEY_2:
				case KEY_3:
				case KEY_4:
				case KEY_5:
				case KEY_6:
				case KEY_7:
				case KEY_8:
				case KEY_9:
				case KEY_ARROWUP:
				case KEY_ARROWDOWN:
				case KEY_VFORMAT:
				case KEY_SUBTITLE:
				case KEY_AUDIO:
				case KEY_TEXT:
					NX_PRISM_NotAvailable_Show();
					return	ONDK_MESSAGE_BREAK;

				case KEY_CH_PLUS:
				case KEY_CH_MINUS:
				case KEY_OPT:
					if (pstContents->ulDlgAttributes & NX_DLG_TYPE_NOT_BTN_CLICK_MASK)
					{
						return ONDK_MESSAGE_BREAK;
					}
					break;

				/*
					OK 버튼 하나만 있으면 break
					left, right, center 버튼이 있으면 내려 줘야 한다.
					대신 이 popup dialg object가 key를 먹을 것이다.	wjmoh
				*/
				case KEY_ARROWLEFT:
				case KEY_ARROWRIGHT:
					if(((pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK) == NX_DLG_YES_NO)
						|| ((pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK) == NX_DLG_OK_CANCEL)
						|| ((pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK) == NX_DLG_CUSTOM_LR)
						|| ((pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK) == NX_DLG_CUSTOM_3B))
					{
						break;
					}
					return ONDK_MESSAGE_BREAK;

				/* Simple RCU에서 입력 전환 관련 Key로 No DB Action manager가 처리하도록 Pass 시킨다. */
				case KEY_EXT_AV_PLUS:
				case KEY_EXT_AV_MINUS:
				default:
					break;
			}
			break;

		case MSG_GWM_CLICKED:
			if (pstContents->ulDlgAttributes & NX_DLG_SUICIDE_MODE)
			{
				// 호출한 Application에서 MSG_APP_DLG_CLICKED Event를 처리하기 힘든 경우 자살해 버려랏!!!!!
				ONDK_GWM_APP_Destroy(0);
			}
			else
			{
				if (p1 == NX_PRISM_UI_DLG_CONFIRM_BTN_L_ID)
				{	/* left button pressed */
					ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_DLG_CLICKED, (Handle_t)NULL, eNX_DLG_CLICKED_BTN_LEFT, (int)pstContents->ulDlgWhoseId, (int)pstContents->thisAppCallback);
				}
				else if (p1 == NX_PRISM_UI_DLG_CONFIRM_BTN_R_ID)
				{	/* right button pressed */
					ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_DLG_CLICKED, (Handle_t)NULL, eNX_DLG_CLICKED_BTN_RIGHT, (int)pstContents->ulDlgWhoseId, (int)pstContents->thisAppCallback);
				}
				else if (p1 == NX_PRISM_UI_DLG_CONFIRM_BTN_M_ID)
				{	/* middle button pressed */
					ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_DLG_CLICKED, (Handle_t)NULL, eNX_DLG_CLICKED_BTN_MIDDLE, (int)pstContents->ulDlgWhoseId, (int)pstContents->thisAppCallback);
				}
				else
				{
					NX_APP_Error("ERROR!!! Unknow Button-(0x%x) fail!!\n", p1);
				}
			}
			return ONDK_MESSAGE_BREAK;

		case MSG_GWM_TIMER:
			if (p1 == NX_PRISM_UI_DLG_CONFIRM_MSG_EXP_TIMER_ID)
			{
				if (pstContents->ulDlgAttributes & NX_DLG_TYPE_NO_EXIT)
				{
					return	ONDK_MESSAGE_BREAK;
				}
				else
				{
					// Backup
					if (pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK)
					{
						lObjId = GWM_GetFocusedObjectID();
						if (lObjId == NX_PRISM_UI_DLG_CONFIRM_BTN_L_ID)
						{
							eDlgBtnType = eNX_DLG_CLICKED_BTN_LEFT;
						}
						else if (p1 == NX_PRISM_UI_DLG_CONFIRM_BTN_R_ID)
						{
							eDlgBtnType = eNX_DLG_CLICKED_BTN_RIGHT;
							//ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_DLG_CLICKED, (Handle_t)NULL, eNX_DLG_CLICKED_BTN_RIGHT, (int)pstContents->ulDlgWhoseId, (int)pstContents->thisAppCallback);
						}
						else if (p1 == NX_PRISM_UI_DLG_CONFIRM_BTN_M_ID)
						{
							eDlgBtnType = eNX_DLG_CLICKED_BTN_MIDDLE;
						}
						else
						{
							eDlgBtnType = eNX_DLG_CLICKED_BTN_NONE;
						}
					}
					HxSTD_memcpy(&stBackupInstance, pstContents, sizeof(NXUI_DialogInstance_t));
					ONDK_GWM_APP_Destroy(0);
					ONDK_GWM_SendMessage(stBackupInstance.parentAppCallback, MSG_APP_DLG_TIMEOUT, (Handle_t)NULL, eDlgBtnType, (int)stBackupInstance.ulDlgWhoseId, (int)stBackupInstance.thisAppCallback);
					return	ONDK_MESSAGE_BREAK;
				}
			}
			break;

		case MSG_GWM_DESTROY:
			ONDK_GWM_KillTimer(NX_PRISM_UI_DLG_CONFIRM_MSG_EXP_TIMER_ID);
			NX_PRISM_DIALOGUE_SetInitContents(pstContents);
			return	ONDK_MESSAGE_BREAK;

		default:
			break;

	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________Process_Message_functions_____________________________________
/******************************************************************************/

static HUINT8	s_ucCurAniIconIndex = 0;


static ONDK_Result_t		nx_processingmessage_UpdateAnimateIcon(void)
{
	ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_ID);
	ONDK_GWM_DirectRedrawAll(ONDK_UPPER_WINDOW);

	return ONDK_RET_OK;
}


static HCHAR	*nx_processingmessage_GetAnimationResource(HUINT8 ucAniIconIdx)
{
	HCHAR		*pszResource = NULL;

	switch (ucAniIconIdx)
	{
		case 0:		pszResource = RES_206_00_LOADING_01_PNG;	break;
		case 1:		pszResource = RES_206_00_LOADING_02_PNG;	break;
		case 2:		pszResource = RES_206_00_LOADING_03_PNG;	break;
		case 3:		pszResource = RES_206_00_LOADING_04_PNG;	break;
		default:	pszResource = NULL;						break;
	}

	return	pszResource;
}


static ONDK_Result_t	nx_processingmessage_DrawAnimateIcon(ONDKSurfaceHandle screen, ONDK_Region_t *area,
												ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HCHAR		*stStockImgID = NULL;
	HINT32		x1 = 0, y1 = 0, x2 = 0, y2 = 0, w = 0, h = 0;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 };


	/* init coordinate */
	x1 = area->x1 + rect->x;
	y1 = area->y1 + rect->y;
	x2 = area->x1 + rect->x + rect->w;
	y2 = area->y1 + rect->y + rect->h;

	w = rect->w;
	h = rect->h;

	s_ucCurAniIconIndex = (s_ucCurAniIconIndex >= NX_PRISM_UI_DLG_PROCESS_ANIMATION_ICON_NUM) ? 0 :  s_ucCurAniIconIndex;
	stStockImgID = nx_processingmessage_GetAnimationResource(s_ucCurAniIconIndex);;
	s_ucCurAniIconIndex = (s_ucCurAniIconIndex+1)%NX_PRISM_UI_DLG_PROCESS_ANIMATION_ICON_NUM;

	stRect = ONDK_Rect(x1, y1, w, h);
	ONDK_GUICOM_IMG_Blit(screen, stRect, (HCHAR*)stStockImgID);

	NX_COMMON_NOT_USED_ARGUMENT(NULL, object, arg, bUiTop);

	return ONDK_RET_OK;
}


static ONDK_Result_t		nx_processingmessage_SetupUI(NXUI_DialogInstance_t *pstContents)
{
	HINT32				lLineCnt = 0, lAddLine = 0, x = 0, y = 0, w = 0, h = 0, lOffset_y = 0;
	HINT32				lOffset_Ani_x = 0;
	ONDK_Rect_t 		stRect = { 0, 0, 0, 0 };


	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, pstContents->pszDialogMsg, NX_PRISM_UI_DLG_CONFIRM_MSG_TEXT_W);

	x = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_X;
	w = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_W;

	if (lLineCnt < NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_MULTI_LINE)
	{
		if ((pstContents->ulDlgAttributes & NX_DLG_TYPE_PM_BAR) && (pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK))
		{
			y = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_BAR_BTN_Y;
			h = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_BAR_BTN_H;
		}
		else if (pstContents->ulDlgAttributes & NX_DLG_TYPE_PM_BAR)
		{
			y = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_BAR_Y;
			h = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_BAR_H;
		}
		else if (pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK)
		{
			y = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_BTN_Y;
			h = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_BTN_H;
		}
		else
		{
			y = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_Y;
			h = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_H;
		}

		ONDK_GWM_APP_SetAppArea(x, y, w, h);
		lAddLine = 0;
	}
	else
	{
		lAddLine = (lLineCnt - NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_MULTI_LINE);
		if ((pstContents->ulDlgAttributes & NX_DLG_TYPE_PM_BAR) && (pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK))
		{
			y = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_BAR_BTN_Y - (lAddLine *NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_Y_MG);
			y = (y < 0) ? 0 : y ;
			h = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_BAR_BTN_H + (lAddLine *NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_H_MG);
		}
		else if (pstContents->ulDlgAttributes & NX_DLG_TYPE_PM_BAR)
		{
			y = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_BAR_Y - (lAddLine *NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_Y_MG);
			y = (y < 0) ? 0 : y ;
			h = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_BAR_H + (lAddLine *NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_H_MG);
		}
		else if (pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK)
		{
			y = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_BTN_Y - (lAddLine *NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_Y_MG);
			y = (y < 0) ? 0 : y ;
			h = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_BTN_H + (lAddLine *NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_H_MG);
		}
		else
		{
			y = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_Y;
			h = NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_MULTI_LINE_H + (lAddLine *NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_H_MG);
		}

		ONDK_GWM_APP_SetAppArea(x, y, w, h);
	}

	/* background */
	stRect = ONDK_Rect(0, 0, NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_W, h);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_DLG_PROCESS_MSG_BG_ID, &stRect, COL(C_G_Bang_90));

	/* TOP Line */
	stRect = ONDK_Rect(0, 0, NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_W, NX_PRISM_UI_DLG_CONFIRM_MSG_LINE_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_DLG_PROCESS_MSG_LINE_TOP_ID, &stRect, COL(C_G_Whistle_100));

	/* BOTTOM Line */
	stRect = ONDK_Rect(0, (h - NX_PRISM_UI_DLG_PROCESS_MSG_LINE_H), NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_W, NX_PRISM_UI_DLG_PROCESS_MSG_LINE_H);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_DLG_PROCESS_MSG_LINE_BOTTOM_ID, &stRect, COL(C_G_Whistle_100));

	/* Animation Icon */
	lOffset_Ani_x = 0;
	if (pstContents->ulDlgAttributes & NX_DLG_TYPE_PM_ANIMAION)
	{
		x = NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_X;
		y = (lLineCnt < NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_MULTI_LINE) ? NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_Y :  NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_MULTI_LINE_Y;
		stRect = ONDK_Rect(x, y, NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_W, NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_H);
		ONDK_GWM_COM_Image_Create(NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_ID, &stRect);
		//ONDK_GWM_COM_Image_SetImage(NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_ID, NULL);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_ID, nx_processingmessage_DrawAnimateIcon);
		s_ucCurAniIconIndex = 0;
		lOffset_Ani_x = (NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_X + NX_PRISM_UI_DLG_PROCESS_MSG_ANI_ICON_W);
	}

	/* Draw Message TEXT */
	y = (lLineCnt < NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_MULTI_LINE) ? NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_Y :  NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_MULTI_LINE_Y ;
	stRect = ONDK_Rect((lOffset_Ani_x + NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_X), y, NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_W - lOffset_Ani_x,
						(NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_LS * lLineCnt) + NX_PRISM_FONT_VSPACE);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_ID, &stRect, (HCHAR*)pstContents->pszDialogMsg);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_ID, (TEXT_ALIGN_LEFT| TEXT_MULTILINED | TEXT_WORDWRAP));
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_ID, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	/* Progress Bar attributes */
	if (pstContents->ulDlgAttributes & NX_DLG_TYPE_PM_BAR)
	{
		y = (lLineCnt < NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_MULTI_LINE) ? NX_PRISM_UI_DLG_PROCESS_MSG_BAR_Y :  NX_PRISM_UI_DLG_PROCESS_MSG_BAR_MULTI_LINE_Y ;
		lOffset_y = (lAddLine *NX_PRISM_UI_DLG_PROCESS_MSG_WINDOW_H_MG);
		y += lOffset_y;
		stRect = ONDK_Rect(NX_PRISM_UI_DLG_PROCESS_MSG_BAR_X, y, NX_PRISM_UI_DLG_PROCESS_MSG_BAR_W, NX_PRISM_UI_DLG_PROCESS_MSG_BAR_H);
		ONDK_GWM_COM_Status_Create(NX_PRISM_UI_DLG_PROCESS_MSG_BAR_ID, &stRect, 100, pstContents->ulProgressStatus);
		ONDK_GWM_COM_Status_SetColor(NX_PRISM_UI_DLG_PROCESS_MSG_BAR_ID, COL(C_G_Whistle_30), COL(C_G_Yoyo01));
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_PROCESS_MSG_BAR_ID, NX_PRISM_COMP_DrawStatusBarB5);
		ONDK_GWM_COM_Status_SetValue(NX_PRISM_UI_DLG_PROCESS_MSG_BAR_ID, pstContents->ulProgressStatus);
	}

	/* button attributes */
	if (pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK)
	{
		if (pstContents->ulDlgAttributes & NX_DLG_TYPE_PM_BAR)
		{
			y = (lLineCnt < NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_MULTI_LINE) ? NX_PRISM_UI_DLG_PROCESS_BTN_BAR_Y :	NX_PRISM_UI_DLG_PROCESS_BTN_BAR_MULTI_LINE_Y;
			lOffset_y = (lAddLine*NX_PRISM_UI_DLG_PROCESS_MSG_BUTTON_Y_MG);
			y += lOffset_y;
		}
		else
		{
			y = (lLineCnt < NX_PRISM_UI_DLG_PROCESS_MSG_TEXT_MULTI_LINE) ? NX_PRISM_UI_DLG_PROCESS_BTN_Y :	NX_PRISM_UI_DLG_PROCESS_BTN_MULTI_LINE_Y;
			lOffset_y = (lAddLine*NX_PRISM_UI_DLG_PROCESS_MSG_BUTTON_Y_MG);
			y += lOffset_y;
		}

		switch (pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK)
		{
			case NX_DLG_YES_NO:
			case NX_DLG_OK_CANCEL:
			case NX_DLG_CUSTOM_LR:
			case NX_DLG_CUSTOM_3B:
				NX_APP_Info("nx_Message_SetUiObject() Only support One button!!\n");
				break;

			case NX_DLG_NO_BUTTON:
				NX_APP_Info("nx_Message_SetUiObject() No Button!! \n");
				break;

			case NX_DLG_CUSTOM_1B:
				if (pstContents->pszButtonM == NULL)
				{
					pstContents->pszButtonM = (HUINT8 *)ONDK_GetString(RES_OK_ID);
				}
				stRect = ONDK_Rect(NX_PRISM_UI_DLG_PROCESS_BTN_X, y, NX_PRISM_UI_DLG_PROCESS_BTN_W, NX_PRISM_UI_DLG_PROCESS_BTN_H);
				ONDK_GWM_COM_Button_Create(NX_PRISM_UI_DLG_PROCESS_BTN_ID, &stRect, (HCHAR*)pstContents->pszButtonM);
				ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_PROCESS_BTN_ID, NX_PRISM_COMP_DrawButton_A1);
				ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_DLG_PROCESS_BTN_ID);
				break;

			case NX_DLG_OK:
			default:
				pstContents->pszButtonM = (HUINT8 *)ONDK_GetString(RES_OK_ID);
				stRect = ONDK_Rect(NX_PRISM_UI_DLG_CONFIRM_BTN_X, y, NX_PRISM_UI_DLG_CONFIRM_BTN_W, NX_PRISM_UI_DLG_CONFIRM_BTN_H);
				ONDK_GWM_COM_Button_Create(NX_PRISM_UI_DLG_PROCESS_BTN_ID, &stRect, (char *)pstContents->pszButtonM);
				ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_DLG_PROCESS_BTN_ID, NX_PRISM_COMP_DrawButton_A1);
				ONDK_GWM_Obj_SetFocus(NX_PRISM_UI_DLG_PROCESS_BTN_ID);
				break;
		}
	}

	/* Set OSD Time out  */
	if (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK)
	{
		pstContents->ulTimeout = (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK) * NX_COMMON_TIME_SECOND;
		ONDK_GWM_SetTimer(NX_PRISM_UI_DLG_PROCESS_MSG_EXP_TIMER_ID, pstContents->ulTimeout);
	}
	else
	{
		pstContents->ulTimeout = 0;
	}

	/* Set Animation Timer  */
	if (pstContents->ulDlgAttributes & NX_DLG_TYPE_PM_ANIMAION)
	{
		ONDK_GWM_SetTimer(NX_PRISM_UI_DLG_PROCESS_ANIMATION_TIMER_ID, (HUINT32)NX_PRISM_UI_DLG_PROCESS_ANIMATION_TIMER_PERIOD);
	}

	return	ONDK_RET_OK;
}



ONDK_Result_t		NX_PRISM_DIALOGUE_ProcessingMessage_Proc(NXUI_DialogInstance_t *pstContents, HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32				lObjId = -1;
	ONDK_Result_t		hResult = ONDK_RET_INITFAIL;
	NX_DLG_ClickedButton_t	eDlgBtnType = eNX_DLG_CLICKED_BTN_NONE;
	NXUI_DialogInstance_t	stBackupInstance;


	if (pstContents == NULL)
	{
		NX_APP_Error("ERROR!!! pstContents was NULL!!\n");
		return	ONDK_MESSAGE_BREAK;
	}

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			/* Create UI Object */
			hResult = nx_processingmessage_SetupUI(pstContents);
			if (hResult != ONDK_RET_OK)
			{
				NX_APP_Error("ERROR!!! nx_processingmessage_SetupUI() fail!!\n");
				break;
			}

			/* 죽을때 넘겨줄 this app func 저장. */
			pstContents->thisAppCallback = ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis());
			break;

		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);

			switch (p1)
			{
				case KEY_STANDBY:
					if (pstContents->ulDlgAttributes & NX_DLG_TYPE_NO_STANDBY)
					{
						return ONDK_MESSAGE_PASS;
					}

					ONDK_GWM_APP_Destroy(0);
					return	ONDK_MESSAGE_PASS;

				case KEY_GUIDE:
				case KEY_SCHEDULE:
				case KEY_MENU:
					if (pstContents->ulDlgAttributes & NX_DLG_TYPE_NO_EXIT)
					{
						/* modal dialog should ignore the exit key */
						return ONDK_MESSAGE_PASS;
					}

					ONDK_GWM_APP_Destroy(0);
					return	ONDK_MESSAGE_PASS;

				case KEY_BACK:
				case KEY_EXIT:
					if (pstContents->ulDlgAttributes & NX_DLG_TYPE_NO_EXIT)
					{
						/* modal dialog should ignore the exit key */
						return	ONDK_MESSAGE_BREAK;
					}
					else
					{
						// Backup
						HxSTD_memcpy(&stBackupInstance, pstContents, sizeof(NXUI_DialogInstance_t));

						/* notify the dialog exit */
						ONDK_GWM_APP_Destroy(0);
						ONDK_GWM_SendMessage(stBackupInstance.parentAppCallback, MSG_APP_DLG_CANCELED, (Handle_t)NULL, 0, (int)stBackupInstance.ulDlgWhoseId, (int)stBackupInstance.thisAppCallback);
						return	ONDK_MESSAGE_BREAK;
					}
					break;

				case KEY_OK:
				case KEY_ARROWLEFT:
				case KEY_ARROWRIGHT:
				case KEY_ARROWUP:
				case KEY_ARROWDOWN:
					/* GWM_ProcessMessageDefault()로 넘김. */
					break;
			}
			break;

		case MSG_GWM_CLICKED:
			if (pstContents->ulDlgAttributes & NX_DLG_SUICIDE_MODE)
			{
				// 호출한 Application에서 MSG_APP_DLG_CLICKED Event를 처리하기 힘든 경우 자살해 버려랏!!!!!
				ONDK_GWM_APP_Destroy(0);
			}
			else
			{
				if (p1 == NX_PRISM_UI_DLG_PROCESS_BTN_ID)
				{	/* left button pressed */
					ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_DLG_CLICKED, (Handle_t)NULL, eNX_DLG_CLICKED_BTN_LEFT, (int)pstContents->ulDlgWhoseId, (int)pstContents->thisAppCallback);
				}
				else
				{
					NX_APP_Error("ERROR!!! Unknow Button-(0x%x) fail!!\n", p1);
				}
			}
			return ONDK_MESSAGE_BREAK;

		case MSG_GWM_TIMER:
			if (p1 == NX_PRISM_UI_DLG_PROCESS_ANIMATION_TIMER_ID)
			{
				// kill timer 하지 않는다.. keep going..
				nx_processingmessage_UpdateAnimateIcon();
				return	ONDK_MESSAGE_BREAK;
			}
			else if (p1 == NX_PRISM_UI_DLG_PROCESS_MSG_EXP_TIMER_ID)
			{
				// Backup
				if (pstContents->ulDlgAttributes & NX_DLG_BUTTONTYPE_MASK)
				{
					lObjId = GWM_GetFocusedObjectID();
					if (lObjId == NX_PRISM_UI_DLG_PROCESS_BTN_ID)
					{
						eDlgBtnType = eNX_DLG_CLICKED_BTN_LEFT;
					}
					else
					{
						NX_APP_Error("ERROR!!! Unknow Button-(0x%x) fail!!\n", lObjId);
					}
				}
				HxSTD_memcpy(&stBackupInstance, pstContents, sizeof(NXUI_DialogInstance_t));
				ONDK_GWM_APP_Destroy(0);
				ONDK_GWM_SendMessage(stBackupInstance.parentAppCallback, MSG_APP_DLG_TIMEOUT, (Handle_t)NULL, eDlgBtnType, (int)stBackupInstance.ulDlgWhoseId, (int)stBackupInstance.thisAppCallback);
				return	ONDK_MESSAGE_BREAK;
			}
			break;

		case MSG_APP_DLG_UPDATE_ANIMATE_ICON:
			nx_processingmessage_UpdateAnimateIcon();
			return	ONDK_MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			NX_PRISM_DIALOGUE_SetInitContents(pstContents);
			return	ONDK_MESSAGE_BREAK;

	}

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________MultiSelect_Message_functions___________________________________
/******************************************************************************/
/* Multi Select Dialogue 는 Popup List를 사용하여 출력 한다. */


ONDK_Rect_t	 NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(NX_DLG_MultiSelRect_t eRectType, HINT32 lMsgLineCnt, HINT32 lItemCnt)
{
	HINT32			lAddLine = 0, x = 0, y = 0, w = 0, h = 0;
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };


	if (eRectType == eNX_DLG_MultiSel_OSD)
	{
		x = NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_X;
		w = NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_W;

		// Item Count
		if (lItemCnt <= NX_PRISM_UI_DLG_MULTISEL_MSG_DEFAULT_ITEM)
		{
			lAddLine = 0;
		}
		else
		{
			lAddLine = (lItemCnt - NX_PRISM_UI_DLG_MULTISEL_MSG_DEFAULT_ITEM);
		}

		y = NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_Y - (lAddLine * NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_MG);
		h = NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_H + (lAddLine * NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_MG);

		if (lMsgLineCnt <= NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_MULTI_LINE)
		{
			lAddLine = 0;
		}
		else
		{
			lAddLine = (lMsgLineCnt - NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_MULTI_LINE);
		}

		y = y - (lAddLine *NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_MULTI_MG);
		h = h + (lAddLine *NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_MULTI_MG);
	}
	else if (eRectType == eNX_DLG_MultiSel_List)
	{
		x = NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_X;
		w = NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_W;

		if (lMsgLineCnt <= NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_MULTI_LINE)
		{
			lAddLine = 0;
		}
		else
		{
			lAddLine = (lMsgLineCnt - NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_MULTI_LINE);
		}

		y = NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_Y - (lAddLine * NX_PRISM_UI_DLG_MULTISEL_MSG_WINDOW_MULTI_MG);
		h = (NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_H * lItemCnt) + (NX_PRISM_UI_DLG_MULTISEL_MSG_CELL_MG * (lItemCnt - 1));
	}

	stRect.x = x;
	stRect.y = y;
	stRect.w = w;
	stRect.h = h;

	return stRect;
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

