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
 * @file	  		nx_prism_comp_dialog_input.c
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
#include <nx_prism_menu_contents.h>
#include <nx_prism_component.h>
#include <nx_prism_comp_dialog_input.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_DLG_INPUT_TIMER_ID								999
#define	NX_DLG_MESSAGE_TEXT_VS								8				// VSpace
#define	NX_DLG_MESSAGE_TEXT_X								30
#define	NX_DLG_MESSAGE_TEXT_Y								38
#define	NX_DLG_MESSAGE_TEXT_W								640
#define	NX_DLG_MESSAGE_TEXT_H								(64 + NX_DLG_MESSAGE_TEXT_VS)

#define	NX_DLG_INPUT_BUTTON_Y								108

#define	NX_DLG_INPUT_MAX_5									5
#define	NX_DLG_INPUT_MAX_4									4
#define	NX_DLG_INPUT_MAX_3									3
#define	NX_DLG_INPUT_MAX_2									2
#define	NX_DLG_INPUT_MAX_1									1


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_comp_DialogInput_SetOSDTimeOut(Nx_PrismDialogInput_Content_t *pstContents);

static ONDK_Result_t	nx_comp_DialogInput_DrawDefaultUI(Nx_PrismDialogInput_Content_t *pstContents);
static ONDK_Result_t	nx_comp_DialogInput_InitContents(Nx_PrismDialogInput_Content_t *pstContents);
static ONDK_Result_t	nx_comp_DialogInput_MsgGwmInput(Nx_PrismDialogInput_Content_t *pstContents, HINT32 message, HINT32 lObjId);
static ONDK_Result_t	nx_comp_DialogInput_MsgGwmCreate(Nx_PrismDialogInput_Content_t *pstContents);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_comp_DialogInput_SetOSDTimeOut(Nx_PrismDialogInput_Content_t *pstContents)
{
	HUINT32		ulTimeOutTime = 0;

	if (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK)
	{
		ulTimeOutTime = (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK) * NX_COMMON_TIME_SECOND;
		ONDK_GWM_SetTimer(NX_DLG_INPUT_TIMER_ID, ulTimeOutTime);
	}

	NX_APP_Info("OSD Time Out Time (%d Sec)\n", ulTimeOutTime);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_comp_DialogInput_DrawDefaultUI(Nx_PrismDialogInput_Content_t *pstContents)
{
	HINT32			lCenter = 0, lBTnMg = 0, lStartX = 0, lStartY = 0, lCenterX = 0, lLeftCnt = 0;
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
		ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_INPUTDLG_BG_ID, &stRect, 0);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_INPUTDLG_BG_ID, NX_PRISM_COMP_DrawWindowPopUp);
	}

	// Draw Frame
	stRect = ONDK_Rect(pstContents->appArea.x, pstContents->appArea.y, pstContents->appArea.w, pstContents->appArea.h);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_INPUTDLG_FRAME_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_INPUTDLG_FRAME_ID, NX_PRISM_COMP_DrawDialugeWindow);

	// Draw Message
	stRect = ONDK_Rect(pstContents->appArea.x + NX_DLG_MESSAGE_TEXT_X, pstContents->appArea.y + NX_DLG_MESSAGE_TEXT_Y, NX_DLG_MESSAGE_TEXT_W, NX_DLG_MESSAGE_TEXT_H);
	NX_PRISM_COMP_DrawDialugeWindowMessage(&stRect, pstContents->pszDlgMsg, pstContents->colorMessageText);

	lCenter = (pstContents->appArea.w / 2);
	lBTnMg = (NX_PRISM_UI_DIALOG_BUTTON_INPUT_W / 2);
	lStartY = NX_DLG_INPUT_BUTTON_Y;

	if (pstContents->ulInputMax == 0)
	{
		return	ONDK_RET_INVARG;
	}

	lLeftCnt = (pstContents->ulInputMax / 2);
	if ((pstContents->ulInputMax % 2) == 0)
	{
		lCenterX = lCenter;
	}
	else
	{
		lCenterX = lCenter - (NX_PRISM_UI_DIALOG_BUTTON_INPUT_W / 2);
	}

	lStartX = lCenterX - (lLeftCnt * NX_PRISM_UI_DIALOG_BUTTON_INPUT_W);

	// Make the new GWM key input object
	stRect = ONDK_Rect(pstContents->appArea.x + lStartX, pstContents->appArea.y + NX_DLG_INPUT_BUTTON_Y,
							(NX_PRISM_UI_DIALOG_BUTTON_INPUT_W * pstContents->ulInputMax), NX_PRISM_UI_DIALOG_BUTTON_INPUT_H);
	ONDK_GWM_COM_Input_Create (NX_PRISM_UI_DIALOG_BUTTON_INPUT_ID, &stRect, (char *)NULL, pstContents->ulInputMax, (char *)pstContents->szInputVlueStr);
	ONDK_GWM_APP_SetObjectDrawMethod (NX_PRISM_UI_DIALOG_BUTTON_INPUT_ID, NX_PRISM_COMP_DrawDialugeInputButton);
	ONDK_GWM_Obj_SetFocus (NX_PRISM_UI_DIALOG_BUTTON_INPUT_ID);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_comp_DialogInput_InitContents(Nx_PrismDialogInput_Content_t *pstContents)
{
	HUINT8		szdata[NX_TEXT_SIZE_128] = {'\0',};

	/* init word */
	HxSTD_memset(pstContents->szInputVlueStr, '-', sizeof(HUINT8) * (pstContents->ulInputMax));
	pstContents->szInputVlueStr[pstContents->ulInputMax] = '\0';

	if (pstContents->ulInputValue > 0)
	{
		switch (pstContents->ulInputMax)
		{
			case NX_DLG_INPUT_MAX_5:
				HxSTD_sprintf((char*)szdata, "%05u", pstContents->ulInputValue);
				break;
			case NX_DLG_INPUT_MAX_4:
				HxSTD_sprintf((char*)szdata, "%04u", pstContents->ulInputValue);
				break;
			case NX_DLG_INPUT_MAX_3:
				HxSTD_sprintf((char*)szdata, "%03u", pstContents->ulInputValue);
				break;
			case NX_DLG_INPUT_MAX_2:
				HxSTD_sprintf((char*)szdata, "%02u", pstContents->ulInputValue);
				break;
			default:
				HxSTD_sprintf((char*)szdata, "%u", pstContents->ulInputValue);
				break;
		}
		szdata[pstContents->ulInputMax] = '\0';

		HxSTD_StrNCpy((char*)pstContents->szInputVlueStr, (char*)szdata, pstContents->ulInputMax);
		pstContents->szInputVlueStr[pstContents->ulInputMax] = '\0';
	}

	/* init pointer */
	pstContents->lValueInputPos = 0;

	/* ������ �Ѱ��� this app func ����. */
	pstContents->thisAppCallback = ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis());

	return	ONDK_RET_OK;
}


static ONDK_Result_t		nx_comp_DialogInput_MsgGwmInput(Nx_PrismDialogInput_Content_t *pstContents, HINT32 message, HINT32 lObjId)
{
	HUINT8		*szInputString = NULL;
	HUINT32		ulInputValue = 0;


	if (message == MSG_GWM_INPUTCOMPLETE)
	{
		szInputString = (HUINT8 *)ONDK_GWM_COM_Input_GetText (lObjId);
		if (szInputString == NULL)
		{
			return	ONDK_RET_INVARG;
		}

		ulInputValue = (HUINT32)HLIB_STD_StrToINT32((HCHAR*)szInputString);
	}
	else
	{
		ulInputValue = pstContents->ulInputValue;
	}

	ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_SETTING_INPUT_VALUE_CHANGED, (HINT32)NULL, ulInputValue, 0, 0);

	return	ONDK_RET_OK;
}


static ONDK_Result_t		nx_comp_DialogInput_MsgGwmCreate(Nx_PrismDialogInput_Content_t *pstContents)
{
	nx_comp_DialogInput_InitContents(pstContents);
	nx_comp_DialogInput_DrawDefaultUI(pstContents);

	nx_comp_DialogInput_SetOSDTimeOut(pstContents);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_COMP_DialogInput_SetDialog(Nx_PrismDialogInput_Content_t *pstContents, ONDK_GWM_Callback_t parentAppCallback, ONDK_Rect_t *pRect,
														HUINT32 ulDlgAttr, ONDK_Color_t colorMessageText, HUINT8 *pszDlgMsg, HUINT32 ulInputValue, HUINT32 ulInputMax)
{
	HUINT32		ulInputDialogId = 0;

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

	ulInputDialogId = pstContents->ulInputDialogId;
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_PrismDialogInput_Content_t));
	HxSTD_memcpy(&(pstContents->appArea), pRect, sizeof(ONDK_Rect_t));
	pstContents->ulInputDialogId = ulInputDialogId;
	pstContents->parentAppCallback = parentAppCallback;
	pstContents->ulDlgAttributes = ulDlgAttr;
	pstContents->colorMessageText = colorMessageText;
	pstContents->ulInputValue = ulInputValue;
	pstContents->ulInputMax = (NX_PRISM_UI_INPUT_DIGHT_MAX < ulInputMax) ? NX_PRISM_UI_INPUT_DIGHT_MAX : ulInputMax;

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


ONDK_Result_t	NX_PRISM_COMP_DialogInput_ProtoProc(Nx_PrismDialogInput_Content_t *pstContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_APP_Trace("NX_PRISM_COMP_DialogInput_ProtoProc message(0x%x) +\n", message);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_MESSAGE_BREAK;
	}

	switch (message)
	{
		case MSG_GWM_CREATE:
			nx_comp_DialogInput_MsgGwmCreate(pstContents);
			break;

		case MSG_GWM_KEYUP:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_INPUTCOMPLETE:
		case MSG_GWM_INPUTCANCELD:
			nx_comp_DialogInput_MsgGwmInput(pstContents, message, p1);
			ONDK_GWM_APP_Destroy(NULL);
			break;

		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
			if (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK)
			{
				ONDK_GWM_ResetTimer(NX_DLG_INPUT_TIMER_ID);
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
			if (p1 == NX_DLG_INPUT_TIMER_ID)
			{
				nx_comp_DialogInput_MsgGwmInput(pstContents, MSG_GWM_INPUTCANCELD, NX_PRISM_UI_DIALOG_BUTTON_INPUT_ID);
				ONDK_GWM_APP_Destroy(NULL);
			}
			return ONDK_MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			break;
	}

	NX_APP_Trace("NX_PRISM_COMP_DialogInput_ProtoProc message(0x%x) -\n", message);

	return	ONDK_GWM_ProcessMessageDefault(message, hAction, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

