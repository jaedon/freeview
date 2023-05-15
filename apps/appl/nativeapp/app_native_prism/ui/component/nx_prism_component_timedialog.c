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
 * @file	  		nx_prism_component_timedialog.c
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
#include <nx_prism_util.h>
#include <nx_prism_component.h>
#include <nx_prism_component_timedialog.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_DLG_TIME_TIMER_ID							999
#define	NX_DLG_TIME_TEXT_VS								8				// VSpace
#define	NX_DLG_TIME_TEXT_X								30
#define	NX_DLG_TIME_TEXT_Y								26
#define	NX_DLG_TIME_TEXT_W								640
#define	NX_DLG_TIME_TEXT_H								(64 + NX_DLG_TIME_TEXT_VS)
#define	NX_DLG_TIME_BUTTON_Y							108

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_comp_TimeDlalog_SetOSDTimeOut(NX_TimeDialogInstance_t *pstContents);
static ONDK_Result_t	nx_comp_TimeDlalog_DrawDefaultUI(NX_TimeDialogInstance_t *pstContents);
static ONDK_Result_t	nx_comp_TimeDlalog_InitContents(NX_TimeDialogInstance_t *pstContents);
static ONDK_Result_t	nx_comp_TimeDialog_MsgGwmInput(NX_TimeDialogInstance_t *pstContents, HINT32 message, HINT32 lObjId);
static ONDK_Result_t	nx_comp_TimeDlalog_MsgGwmCreate(NX_TimeDialogInstance_t *pstContents);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_comp_TimeDlalog_SetOSDTimeOut(NX_TimeDialogInstance_t *pstContents)
{
	HUINT32		ulTimeOutTime = 0;

	if (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK)
	{
		ulTimeOutTime = (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK) * NX_COMMON_TIME_SECOND;
		ONDK_GWM_SetTimer(NX_DLG_TIME_TIMER_ID, ulTimeOutTime);
	}

	NX_APP_Info("OSD Time Out Time (%d Sec)\n", ulTimeOutTime);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_comp_TimeDlalog_DrawDefaultUI(NX_TimeDialogInstance_t *pstContents)
{
	HINT32			lCenter = 0, lStartX = 0, lCenterX = 0, lLeftCnt = 0;
	ONDK_Rect_t 	stBGRect, stRect;

	HxSTD_memset(&stBGRect, 0x00, sizeof(ONDK_Rect_t));
	HxSTD_memset(&stRect, 0x00, sizeof(ONDK_Rect_t));

	if (pstContents->ulDlgAttributes & NX_DLG_TYPE_OSD_FULL)
	{
		stRect = ONDK_Rect(0, 0, NX_PRISM_OSD_HORIZONTAL, NX_PRISM_OSD_VERTICAL);
	}
	else
	{
		stRect = ONDK_Rect(pstContents->appArea.x, pstContents->appArea.y, pstContents->appArea.w, pstContents->appArea.h);
	}

	// Set OSD Area
	ONDK_GWM_APP_SetAppArea(stRect.x, stRect.y, stRect.w, stRect.h);

	// Draw Background
	if (pstContents->ulDlgAttributes & NX_DLG_TYPE_BG_DIM)
	{
		ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_TIMEDLG_BG_ID, &stRect, 0);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_TIMEDLG_BG_ID, NX_PRISM_COMP_DrawWindowPopUp);
	}

	// Draw Frame
	stRect = ONDK_Rect(pstContents->appArea.x, pstContents->appArea.y, pstContents->appArea.w, pstContents->appArea.h);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_TIMEDLG_FRAME_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_TIMEDLG_FRAME_ID, NX_PRISM_COMP_DrawDialugeWindow);

	// Draw Message
	stRect = ONDK_Rect(pstContents->appArea.x + NX_DLG_TIME_TEXT_X, pstContents->appArea.y + NX_DLG_TIME_TEXT_Y, NX_DLG_TIME_TEXT_W, NX_DLG_TIME_TEXT_H);
	NX_PRISM_COMP_DrawDialugeWindowMessage(&stRect, pstContents->pszDlgMsg, pstContents->colorMessageText);

	lCenter = (pstContents->appArea.w / 2);

	if (pstContents->ulInputMax == 0)
	{
		return	ONDK_RET_INVARG;
	}

	lLeftCnt = ((pstContents->ulInputMax + 1) / 2);
	if ((pstContents->ulInputMax % 2) == 0)
	{
		lCenterX = lCenter;
	}
	else
	{
		lCenterX = lCenter - (NX_PRISM_UI_TIME_DATA_W / 2);
	}

	lStartX = lCenterX - (lLeftCnt * NX_PRISM_UI_TIME_DATA_W);

	// Draw Colon
	stRect = ONDK_Rect(pstContents->appArea.x + lStartX + 2 * NX_PRISM_UI_TIME_DATA_W, pstContents->appArea.y + NX_DLG_TIME_BUTTON_Y, NX_PRISM_UI_TIME_DATA_W, NX_PRISM_UI_TIME_DATA_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_TIME_COLON_ID, &stRect, ":");
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_TIME_COLON_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_TIME_COLON_ID, NX_PRISM_FONT_SIZE_36);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_TIME_COLON_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_TIME_COLON_ID, COL(C_G_Whistle_100), COL(C_G_Whistle_100));

	// Make the new GWM key input object
	stRect = ONDK_Rect(pstContents->appArea.x + lStartX, pstContents->appArea.y + NX_DLG_TIME_BUTTON_Y, (NX_PRISM_UI_TIME_DATA_W * (pstContents->ulInputMax + 1)), NX_PRISM_UI_TIME_DATA_H);
	ONDK_GWM_COM_Input_Create (NX_PRISM_UI_TIME_DATA_ID, &stRect, (char *)NULL, pstContents->ulInputMax, (char *)pstContents->szInputValueStr);
	ONDK_GWM_APP_SetObjectDrawMethod (NX_PRISM_UI_TIME_DATA_ID, NX_PRISM_COMP_DrawButton_TimeDataButton);

	ONDK_GWM_Obj_SetFocus (NX_PRISM_UI_TIME_DATA_ID);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_comp_TimeDlalog_InitContents(NX_TimeDialogInstance_t *pstContents)
{
	HUINT8		szdata[NX_TEXT_SIZE_128] = {'\0',};

	/* init word */
	HxSTD_memset(pstContents->szInputValueStr, '-', sizeof(HUINT8) * (pstContents->ulInputMax));
	pstContents->szInputValueStr[pstContents->ulInputMax] = '\0';

	if (pstContents->ulInputValue > 0)
	{
		HxSTD_sprintf((char*)szdata, "%04u", pstContents->ulInputValue);
		//HxSTD_sprintf(szdata, "% 0ld", pstContents->ulInputMax, pstContents->ulInputValue);
		szdata[pstContents->ulInputMax] = '\0';

		HxSTD_StrNCpy((char*)pstContents->szInputValueStr, (char*)szdata, pstContents->ulInputMax);
		pstContents->szInputValueStr[pstContents->ulInputMax] = '\0';
	}

	/* init pointer */
	pstContents->nTimeInputPos = 0;

	/* ï¿½ï¿½ï¿½ï¿½ï¿?ï¿½Ñ°ï¿½ï¿?this app func ï¿½ï¿½ï¿½ï¿½. */
	pstContents->thisAppCallback = ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis());

	return	ONDK_RET_OK;
}

static ONDK_Result_t		nx_comp_TimeDialog_MsgGwmInput(NX_TimeDialogInstance_t *pstContents, HINT32 message, HINT32 lObjId)
{
	HUINT8		*szInputString = NULL;
	HUINT32		ulInputValue = 0, ulUnixTime = 0, ulInputLen = 0;
	HUINT8		ucHour, ucMinute;
	HxDATETIME_t	stDateTime;


	if (message == MSG_GWM_INPUTCOMPLETE)
	{
		szInputString = (HUINT8 *)ONDK_GWM_COM_Input_GetText (lObjId);
		if (szInputString == NULL)
		{
			return	ONDK_RET_INVARG;
		}

		ulInputValue = (HUINT32)HLIB_STD_StrToINT32((HCHAR*)szInputString);
		ulInputLen = HLIB_STD_StrNLen((char*)szInputString, NX_PRISM_UI_TIME_DIGIT_MAX);
	}
	else
	{
		ulInputValue = pstContents->ulInputValue;
	}

	ucHour = (HUINT8)(ulInputValue / 100);
	ucMinute = (HUINT8)(ulInputValue % 100);

	(void)NX_SYSTEM_GetTime(&ulUnixTime);

	HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUnixTime, &stDateTime);
	stDateTime.stTime.ucHour = ucHour;
	stDateTime.stTime.ucMinute = ucMinute;
	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stDateTime, &ulUnixTime);

	ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_SETTING_INPUT_VALUE_CHANGED, (HINT32)NULL, ulUnixTime, 0, 0);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_comp_TimeDlalog_MsgGwmCreate(NX_TimeDialogInstance_t *pstContents)
{
	nx_comp_TimeDlalog_InitContents(pstContents);
	nx_comp_TimeDlalog_DrawDefaultUI(pstContents);
	nx_comp_TimeDlalog_SetOSDTimeOut(pstContents);

	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
 ONDK_Result_t	NX_PRISM_COMP_TimeDlalog_SetDialog(NX_TimeDialogInstance_t *pstContents, ONDK_GWM_Callback_t parentAppCallback, ONDK_Rect_t *pRect,
														HUINT32 ulDlgAttr, ONDK_Color_t colorMessageText, HUINT8 *pszDlgMsg, HUINT32 ulInputValue, HUINT32 ulTimeDataType)
{
	HUINT32		ulTimeDlgWhoseId = 0;

	if (pstContents == NULL)
	{
		NX_APP_Error("[%s:%d] Error!!! pstContents is NULL(%d)\n", __FUNCTION__, __HxLINE__);
		return	ONDK_RET_INVARG;
	}

	// parent app
	if (parentAppCallback == NULL)
	{
		NX_APP_Error("[%s:%d] Error!!! parentAppCallback is NULL(%d)\n", __FUNCTION__, __HxLINE__);
		return	ONDK_RET_INVARG;
	}

	ulTimeDlgWhoseId = pstContents->ulTimeDlgWhoseId;
	HxSTD_memset(pstContents, 0x00, sizeof(NX_TimeDialogInstance_t));
	HxSTD_memcpy(&(pstContents->appArea), pRect, sizeof(ONDK_Rect_t));
	pstContents->ulTimeDlgWhoseId = ulTimeDlgWhoseId;
	pstContents->parentAppCallback = parentAppCallback;
	pstContents->ulDlgAttributes = ulDlgAttr;
	pstContents->colorMessageText = colorMessageText;
	pstContents->ulInputMax = NX_PRISM_UI_TIME_DIGIT_MAX-1;
	pstContents->timeDataType = ulTimeDataType;
	pstContents->ulInputValue = ulInputValue;

	if (pszDlgMsg == NULL)
	{
		pstContents->pszDlgMsg = NULL;
	}
	else
	{
		pstContents->pszDlgMsg = pszDlgMsg;
	}
	return	ONDK_RET_OK;
}

ONDK_Result_t	NX_PRISM_COMP_TimeDlalog_ProtoProc(NX_TimeDialogInstance_t *pstContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_APP_Trace("NX_PRISM_COMP_TimeDlalog_ProtoProc message(0x%x) +\n", message);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_MESSAGE_BREAK;
	}

	switch (message){
		case MSG_GWM_CREATE:
			nx_comp_TimeDlalog_MsgGwmCreate(pstContents);
			break;

		case MSG_GWM_KEYUP:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_INPUTCOMPLETE:
		case MSG_GWM_INPUTCANCELD:
			nx_comp_TimeDialog_MsgGwmInput(pstContents, message, p1);
			ONDK_GWM_APP_Destroy(NULL);
			break;

		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
			if (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK)
			{
				ONDK_GWM_ResetTimer(NX_DLG_TIME_TIMER_ID);
			}
 			switch (p1)
			{
				case KEY_GUIDE:
				case KEY_MENU:
				case KEY_EXIT:
				case KEY_STANDBY:
					ONDK_GWM_APP_Destroy(NULL);
					return	ONDK_MESSAGE_PASS;

				/* keys to ignore */
				case KEY_ARROWUP:
				case KEY_ARROWDOWN:
				case KEY_RED:
				case KEY_BLUE:
				case KEY_GREEN:
				case KEY_YELLOW:
				case KEY_VFORMAT:
				case KEY_SUBTITLE:
				case KEY_AUDIO:
				case KEY_TEXT:
					NX_PRISM_NotAvailable_Show();
					return	ONDK_MESSAGE_BREAK;

				default:
					break;
			}
			break;

		case MSG_GWM_TIMER:
			if (p1 == NX_DLG_TIME_TIMER_ID){
				nx_comp_TimeDialog_MsgGwmInput(pstContents, MSG_GWM_INPUTCANCELD, NX_PRISM_UI_TIME_DATA_ID);
				ONDK_GWM_APP_Destroy(NULL);
			}
			return ONDK_MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			break;
	}

	NX_APP_Trace("NX_PRISM_COMP_TimeDlalog_ProtoProc message(0x%x) -\n", message);

	return	ONDK_GWM_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

