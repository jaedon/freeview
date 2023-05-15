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
 * @file	  		nx_prism_component_pindialog.c
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
#include <nx_prism_component_pindialog.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_DLG_PINCODE_TIMER_ID								1
#define	NX_DLG_PINCODE_TIMER_RESET_ID						2
#define	NX_DLG_PINCODE_TIMER_RESET_TIME						500

#define	NX_DLG_PINCODE_TEXT_VS								8				// VSpace
#define	NX_DLG_PINCODE_TEXT_X								30
#define	NX_DLG_PINCODE_TEXT_Y								26
#define	NX_DLG_PINCODE_TEXT_W								640
#define	NX_DLG_PINCODE_TEXT_H								(64 + NX_DLG_PINCODE_TEXT_VS)
#define	NX_DLG_PINCODE_BUTTON_Y								76

//gap
#define NX_DLG_CHANGE_PINCODE_LINE_GAP 						70

#define	NX_DLG_CHANGE_PINCODE_NEWBUTTON_Y					65
#define	NX_DLG_CHANGE_PINCODE_VERIFYBUTTON_Y				(NX_DLG_CHANGE_PINCODE_NEWBUTTON_Y + NX_DLG_CHANGE_PINCODE_LINE_GAP)

#define	NX_DLG_CHANGEPIN_TEXT_X								23
#define	NX_DLG_CHANGEPIN_TEXT_Y								20
#define	NX_DLG_CHANGEPIN_TEXT_W								618
#define	NX_DLG_CHANGEPIN_TEXT_H								36

#define	NX_DLG_CHANGEPIN_NEWPIN_TEXT_X						45
#define	NX_DLG_CHANGEPIN_NEWPIN_TEXT_Y						79
#define	NX_DLG_CHANGEPIN_NEWPIN_TEXT_W						235
#define	NX_DLG_CHANGEPIN_NEWPIN_TEXT_H						40

#define	NX_DLG_CHANGEPIN_VERIFYPIN_TEXT_X					45
#define	NX_DLG_CHANGEPIN_VERIFYPIN_TEXT_Y					(79 + NX_DLG_CHANGE_PINCODE_LINE_GAP)
#define	NX_DLG_CHANGEPIN_VERIFYPIN_TEXT_W					235
#define	NX_DLG_CHANGEPIN_VERIFYPIN_TEXT_H					40

#define	NX_DLG_CHANGEPIN_WRONG_PIN_IMG_X					490
#define	NX_DLG_CHANGEPIN_WRONG_PIN_IMG_Y					90
#define	NX_DLG_CHANGEPIN_WRONG_PIN_IMG_W					23
#define	NX_DLG_CHANGEPIN_WRONG_PIN_IMG_H					23


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_comp_PinDlalog_SetOSDTimeOut(NX_PinDialogInstance_t *pstContents);

static HINT32			nx_comp_PinDlalog_CheckCurrNumber(NX_PinDialogInstance_t *pstContents, HINT32 number);
static HBOOL			nx_comp_PinDlalog_IsPinOk(NX_PinDialogInstance_t *pstContents);
static HINT32			nx_comp_PinDlalog_BackspacePosition(NX_PinDialogInstance_t *pstContents);
static ONDK_Result_t	nx_comp_PinDlalog_ProcessPinReset(NX_PinDialogInstance_t *pstContents);

static ONDK_Result_t	nx_comp_PinDlalog_DrawDefaultUI(NX_PinDialogInstance_t *pstContents);
static ONDK_Result_t	nx_comp_PinDlalog_InitContents(NX_PinDialogInstance_t *pstContents);

static void				nx_comp_PinDlalog_NotifyPinAuthorized(NX_PinDialogInstance_t *pstContents);
static void				nx_comp_PinDlalog_NotifyPinDenied(NX_PinDialogInstance_t *pstContents);
static ONDK_Result_t	nx_comp_PinDlalog_MsgGwmCreate(NX_PinDialogInstance_t *pstContents);

static HINT32			nx_comp_ChangePinDlalog_CheckNewPinNumber(NX_PinDialogInstance_t *pstContents, HINT32 number);
static HINT32			nx_comp_ChangePinDlalog_CheckConfirmPinNumber(NX_PinDialogInstance_t *pstContents, HINT32 number);
static ONDK_Result_t	nx_comp_ChangePinDlalog_ProcessPinReset(NX_PinDialogInstance_t *pstContents);
static HBOOL			nx_comp_ChangePinDlalog_IsChangePinOk(NX_PinDialogInstance_t *pstContents);
static HINT32			nx_comp_ChangePinDlalog_BackspacePosition (NX_PinDialogInstance_t *pstContents);

static ONDK_Result_t	nx_comp_ChangePinDlalog_DrawDefaultUI(NX_PinDialogInstance_t *pstContents);
static void				nx_comp_ChangePinDlalog_InitContents(NX_PinDialogInstance_t *pstContents);
static ONDK_Result_t	nx_comp_ChangePinDlalog_MsgGwmCreate(NX_PinDialogInstance_t *pstContents);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_comp_PinDlalog_SetOSDTimeOut(NX_PinDialogInstance_t *pstContents)
{
	HUINT32		ulTimeOutTime = 0;

	if (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK)
	{
		ulTimeOutTime = (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK) * NX_COMMON_TIME_SECOND;
		ONDK_GWM_SetTimer(NX_DLG_PINCODE_TIMER_ID, ulTimeOutTime);
	}

	NX_APP_Info("OSD Time Out Time (%d Sec)\n", ulTimeOutTime);

	return	ONDK_RET_OK;
}

static HINT32			nx_comp_PinDlalog_CheckCurrNumber(NX_PinDialogInstance_t *pstContents, HINT32 number)
{
	if (pstContents->nPinInputPos < NX_PRISM_UI_PIN_DIGIT_MAX-1)
	{
		/* set curr by pincode value */
		pstContents->szPinCodeStr[pstContents->nPinInputPos] = ('0' + number);

		/* invalidate Curr */
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_PIN_CODE_ID + pstContents->nPinInputPos);

		/* go to next */
		pstContents->nPinInputPos++;

		/* set focus next */
		ONDK_GWM_Obj_SetFocus (NX_PRISM_UI_PIN_CODE_ID + pstContents->nPinInputPos);
	}

	return pstContents->nPinInputPos;
}

static HBOOL			nx_comp_PinDlalog_IsPinOk(NX_PinDialogInstance_t *pstContents)
{
	HERROR		hError = ERR_FAIL;
	HBOOL		bRetVal = FALSE;

	if (pstContents->pinType == eNxPinCodeDataType_Installation)	//Used different pin-code, parental pin and install pin
	{
		hError = NX_SYSTEM_CheckInstallPinCode((HCHAR*)pstContents->szPinCodeStr);
		bRetVal = (hError == ERR_OK) ? TRUE : FALSE ;
	}
	else if (pstContents->pinType != eNxPinCodeDataType_LockMaturityRating && pstContents->pinType != eNxPinCodeDataType_LockChannel)
	{
		bRetVal = NX_SYSTEM_VerifyPinCode((HCHAR*)pstContents->szPinCodeStr);
	}
	else
	{
		bRetVal = NX_SYSTEM_CheckPinCode((HCHAR*)pstContents->szPinCodeStr);
	}

	return bRetVal;
}

static HINT32			nx_comp_PinDlalog_BackspacePosition(NX_PinDialogInstance_t *pstContents)
{
	if ((pstContents->nPinInputPos > 0) && (pstContents->nPinInputPos < NX_PRISM_UI_PIN_DIGIT_MAX-1))
	{
		/* backspace */
		pstContents->nPinInputPos--;

		/* set curr by pincode value */
		pstContents->szPinCodeStr[pstContents->nPinInputPos] = ('-');

		/* invalidate Curr */

		ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_PIN_CODE_ID + pstContents->nPinInputPos + 1 );

		/* set focus next */
		ONDK_GWM_Obj_SetFocus (NX_PRISM_UI_PIN_CODE_ID + pstContents->nPinInputPos);
	}

	return pstContents->nPinInputPos;
}

static ONDK_Result_t	nx_comp_PinDlalog_ProcessPinReset(NX_PinDialogInstance_t *pstContents)
{
	HINT32		i = 0;

	/* reset word */
	HxSTD_memset(pstContents->szPinCodeStr, '-', sizeof(HUINT8) * (pstContents->ulInputMax));
	pstContents->szPinCodeStr[pstContents->ulInputMax] = '\0';

	/* reset pointer */
	pstContents->nPinInputPos = 0;

	/* invalidate word */
	for (i = 0; i < NX_PRISM_UI_PIN_DIGIT_MAX-1; i++)
	{
		ONDK_GWM_COM_Button_ChangeTitle(NX_PRISM_UI_PIN_CODE_ID + i, (HCHAR*)pstContents->szPinCodeStr + i);
	}

	ONDK_GWM_Obj_SetFocus (NX_PRISM_UI_PIN_CODE_ID);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_comp_PinDlalog_DrawDefaultUI(NX_PinDialogInstance_t *pstContents)
{
	HINT32			i, lCenter = 0, lStartX = 0, lCenterX = 0, lLeftCnt = 0;
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
		ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_PINDLG_BG_ID, &stRect, 0);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_PINDLG_BG_ID, NX_PRISM_COMP_DrawWindowPopUp);
	}

	// Draw Frame
	stRect = ONDK_Rect(pstContents->appArea.x, pstContents->appArea.y, pstContents->appArea.w, pstContents->appArea.h);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_PINDLG_FRAME_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_PINDLG_FRAME_ID, NX_PRISM_COMP_DrawDialugeWindow);

	// Draw Message
	stRect = ONDK_Rect(pstContents->appArea.x + NX_DLG_PINCODE_TEXT_X, pstContents->appArea.y + NX_DLG_PINCODE_TEXT_Y, NX_DLG_PINCODE_TEXT_W, NX_DLG_PINCODE_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_PIN_CODE_MESSAGE_ID, &stRect, (HCHAR*)pstContents->pszDlgMsg);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_PIN_CODE_MESSAGE_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_PIN_CODE_MESSAGE_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_PIN_CODE_MESSAGE_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_PIN_CODE_MESSAGE_ID, COL(C_G_Whistle_100), COL(C_G_Whistle_100));

	lCenter = (pstContents->appArea.w / 2);

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
		lCenterX = lCenter - (NX_PRISM_UI_PIN_CODE_W / 2);
	}

	lStartX = lCenterX - (lLeftCnt * (NX_PRISM_UI_PIN_CODE_W + NX_PRISM_UI_PIN_BTN_GAP));

	for (i = 0; i < pstContents->ulInputMax; i++)
	{
		stRect = ONDK_Rect(pstContents->appArea.x + lStartX + i * (NX_PRISM_UI_PIN_CODE_W + NX_PRISM_UI_PIN_BTN_GAP),
							pstContents->appArea.y + NX_DLG_PINCODE_BUTTON_Y, NX_PRISM_UI_PIN_CODE_W, NX_PRISM_UI_PIN_CODE_H);
		ONDK_GWM_COM_Button_Create(NX_PRISM_UI_PIN_CODE_ID + i, &stRect, (HCHAR*)pstContents->szPinCodeStr + i);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_PIN_CODE_ID + i, NX_PRISM_COMP_DrawButton_PinCodeButton);
	}

	stRect = ONDK_Rect(pstContents->appArea.x + NX_DLG_CHANGEPIN_WRONG_PIN_IMG_X, pstContents->appArea.y + NX_DLG_CHANGEPIN_WRONG_PIN_IMG_Y,
						NX_DLG_CHANGEPIN_WRONG_PIN_IMG_W, NX_DLG_CHANGEPIN_WRONG_PIN_IMG_H);
	ONDK_GWM_COM_Image_Create(NX_PRISM_UI_PIN_CODE_WRONG_ID, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_PRISM_UI_PIN_CODE_WRONG_ID, (HCHAR*)RES_311_00_DW_PIN_WRONG_PNG);
	ONDK_GWM_Obj_SetHide(NX_PRISM_UI_PIN_CODE_WRONG_ID, TRUE);

	ONDK_GWM_Obj_SetFocus (NX_PRISM_UI_PIN_CODE_ID);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_comp_PinDlalog_InitContents(NX_PinDialogInstance_t *pstContents)
{
	/* init word */
	HxSTD_memset(pstContents->szPinCodeStr, '-', sizeof(HUINT8) * (pstContents->ulInputMax));
	pstContents->szPinCodeStr[pstContents->ulInputMax] = '\0';

	/* init pointer */
	pstContents->nPinInputPos = 0;

	/* 죽을때 넘겨줄 this app func 저장. */
	pstContents->thisAppCallback = ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis());

	return	ONDK_RET_OK;
}

static void				nx_comp_PinDlalog_NotifyPinAuthorized(NX_PinDialogInstance_t *pstContents)
{
	ONDK_GWM_APP_Destroy(NULL);
	ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_SYSTEM_PIN_OK, (HINT32)NULL, pstContents->pinType, 0, 0);
}
static void				nx_comp_PinDlalog_NotifyPinDenied(NX_PinDialogInstance_t *pstContents)
{
	ONDK_GWM_APP_Destroy(NULL);
	ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_SYSTEM_PIN_DENIED, (HINT32)NULL, pstContents->pinType, 0, 0);
}

static ONDK_Result_t	nx_comp_PinDlalog_MsgGwmCreate(NX_PinDialogInstance_t *pstContents)
{
	nx_comp_PinDlalog_InitContents(pstContents);
	nx_comp_PinDlalog_DrawDefaultUI(pstContents);
	nx_comp_PinDlalog_SetOSDTimeOut(pstContents);

	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
 ONDK_Result_t	NX_PRISM_COMP_PinDlalog_SetDialog(NX_PinDialogInstance_t *pstContents, ONDK_GWM_Callback_t parentAppCallback, ONDK_Rect_t *pRect,
														HUINT32 ulDlgAttr, ONDK_Color_t colorMessageText, HUINT8 *pszDlgMsg, HUINT32 ulPinCodeType)
{
	HUINT32		ulPinDlgWhoseId = 0;

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

	ulPinDlgWhoseId = pstContents->ulPinDlgWhoseId;
	HxSTD_memset(pstContents, 0x00, sizeof(NX_PinDialogInstance_t));
	HxSTD_memcpy(&(pstContents->appArea), pRect, sizeof(ONDK_Rect_t));
	pstContents->ulPinDlgWhoseId = ulPinDlgWhoseId;
	pstContents->parentAppCallback = parentAppCallback;
	pstContents->ulDlgAttributes = ulDlgAttr;
	pstContents->colorMessageText = colorMessageText;
	pstContents->ulInputMax = NX_PRISM_UI_PIN_DIGIT_MAX-1;
	pstContents->pinType = ulPinCodeType;
	pstContents->bPinInputLock = FALSE;

	if (pszDlgMsg == NULL)
	{
		pstContents->pszDlgMsg = (HUINT8*)" ";
	}
	else
	{
		pstContents->pszDlgMsg = pszDlgMsg;
	}
	return	ONDK_RET_OK;
}

ONDK_Result_t	NX_PRISM_COMP_PinDlalog_ProtoProc(NX_PinDialogInstance_t *pstContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL		bIsPinOk = FALSE;


	NX_APP_Trace("NX_PRISM_COMP_PinDlalog_ProtoProc message(0x%x) +\n", message);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_MESSAGE_BREAK;
	}

	switch (message){
		case MSG_GWM_CREATE:
			nx_comp_PinDlalog_MsgGwmCreate(pstContents);
			break;

		case MSG_GWM_KEYUP:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
			if (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK)
			{
				ONDK_GWM_ResetTimer(NX_DLG_PINCODE_TIMER_ID);
			}
 			switch (p1)
			{
				/* keys to pin quit */
				case KEY_BACK:
				case KEY_EXIT:
					ONDK_GWM_APP_Destroy(NULL);
					return	ONDK_MESSAGE_BREAK;

				/* go back */
				case KEY_ARROWLEFT:
					if((pstContents->pinType == eNxPinCodeDataType_LockMaturityRating || pstContents->pinType == eNxPinCodeDataType_LockChannel) && pstContents->nPinInputPos <= 0)
					{
						ONDK_GWM_APP_Destroy(NULL);
						return	ONDK_MESSAGE_PASS;
					}
					else
					{
						nx_comp_PinDlalog_BackspacePosition(pstContents);
					}
					return	ONDK_MESSAGE_BREAK;

				case KEY_ARROWRIGHT:
					if((pstContents->pinType == eNxPinCodeDataType_LockMaturityRating || pstContents->pinType == eNxPinCodeDataType_LockChannel) && pstContents->nPinInputPos <= 0)
					{
						ONDK_GWM_APP_Destroy(NULL);
						return	ONDK_MESSAGE_PASS;
					}
					return	ONDK_MESSAGE_BREAK;

				case KEY_MENU:
					if(pstContents->pinType == eNxPinCodeDataType_LockMaturityRating || pstContents->pinType == eNxPinCodeDataType_LockChannel)
					{
						return	ONDK_MESSAGE_PASS;
					}
					return	ONDK_MESSAGE_BREAK;

				case KEY_GUIDE:
					if(pstContents->pinType == eNxPinCodeDataType_LockMaturityRating || pstContents->pinType == eNxPinCodeDataType_LockChannel)
					{
						ONDK_GWM_APP_Destroy(NULL);
						return	ONDK_MESSAGE_PASS;
					}
					return	ONDK_MESSAGE_BREAK;

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
					if (pstContents->bPinInputLock == TRUE)
					{
						NX_PRISM_NotAvailable_Show();
						return	ONDK_MESSAGE_BREAK;
					}

					if (nx_comp_PinDlalog_CheckCurrNumber(pstContents, (p1 - KEY_0)) >= NX_PRISM_UI_PIN_DIGIT_MAX-1)
					{
						bIsPinOk = nx_comp_PinDlalog_IsPinOk(pstContents);
						if (bIsPinOk == TRUE)
						{
							nx_comp_PinDlalog_NotifyPinAuthorized(pstContents);
						}
						else
						{
							pstContents->bPinInputLock = TRUE;
							ONDK_GWM_Obj_SetHide(NX_PRISM_UI_PIN_CODE_WRONG_ID, FALSE);
							ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_PIN_CODE_WRONG_ID);
							ONDK_GWM_SetTimer(NX_DLG_PINCODE_TIMER_RESET_ID, NX_DLG_PINCODE_TIMER_RESET_TIME);
						}
					}
				return	ONDK_MESSAGE_BREAK;

				/* keys to ignore */
				case KEY_ARROWUP:
				case KEY_ARROWDOWN:
				case KEY_RED:
				case KEY_BLUE:
				case KEY_GREEN:
				case KEY_YELLOW:
					return	ONDK_MESSAGE_BREAK;
				default:
					break;
			}
			break;

		case MSG_GWM_TIMER:
			if (p1 == NX_DLG_PINCODE_TIMER_ID)
			{
				ONDK_GWM_KillTimer(NX_DLG_PINCODE_TIMER_ID);
				nx_comp_PinDlalog_NotifyPinDenied(pstContents);
			}
			else if (p1 == NX_DLG_PINCODE_TIMER_RESET_ID)
			{
				ONDK_GWM_KillTimer(NX_DLG_PINCODE_TIMER_RESET_ID);
				pstContents->bPinInputLock = FALSE;
				ONDK_GWM_Obj_SetHide(NX_PRISM_UI_PIN_CODE_WRONG_ID, TRUE);
				ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_PIN_CODE_WRONG_ID);
				nx_comp_PinDlalog_ProcessPinReset(pstContents);
			}
			else
			{
				NX_APP_Error("Error!!! Invalid Timer ID : (0x%x)", p1);
			}
			return ONDK_MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			break;
	}

	NX_APP_Trace("NX_PRISM_COMP_PinDlalog_ProtoProc message(0x%x) -\n", message);

	return	ONDK_GWM_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

/******************************************************************************/
#define _________Change_Pincode_Private_functions_____________________________________________
/******************************************************************************/
static HINT32			nx_comp_ChangePinDlalog_CheckNewPinNumber(NX_PinDialogInstance_t *pstContents, HINT32 number)
{
	if (pstContents->nPinInputPos < NX_PRISM_UI_PIN_DIGIT_MAX-1)
	{
		/* set curr by pincode value */
		pstContents->szNewPinCodeStr[pstContents->nPinInputPos] = ('0' + number);

		/* invalidate Curr */
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_CHANGEPIN_BTN_ID + pstContents->nPinInputPos);

		/* go to next */
		pstContents->nPinInputPos++;

		/* set focus next */
		ONDK_GWM_Obj_SetFocus (NX_PRISM_UI_CHANGEPIN_BTN_ID + pstContents->nPinInputPos);
	}

	return pstContents->nPinInputPos;
}
static HINT32			nx_comp_ChangePinDlalog_CheckConfirmPinNumber(NX_PinDialogInstance_t *pstContents, HINT32 number)
{
	if (pstContents->nPinInputPos < NX_PRISM_UI_PIN_DIGIT_MAX-1)
	{
		/* set curr by pincode value */
		pstContents->szVerifyPinCodeStr[pstContents->nPinInputPos] = ('0' + number);

		/* invalidate Curr */
		ONDK_GWM_APP_InvalidateObject(NX_PRISM_UI_CHANGEPIN_VERIFYBTN_ID + pstContents->nPinInputPos);

		/* go to next */
		pstContents->nPinInputPos++;

		/* set focus next */
		ONDK_GWM_Obj_SetFocus (NX_PRISM_UI_CHANGEPIN_VERIFYBTN_ID + pstContents->nPinInputPos);
	}

	return pstContents->nPinInputPos;
}

static ONDK_Result_t	nx_comp_ChangePinDlalog_ProcessPinReset(NX_PinDialogInstance_t *pstContents)
{
	HINT32		i = 0;

	/* reset word */
	HxSTD_memset(pstContents->szNewPinCodeStr, '-', sizeof(HUINT8) * (pstContents->ulInputMax));
	pstContents->szPinCodeStr[pstContents->ulInputMax] = '\0';

	HxSTD_memset(pstContents->szVerifyPinCodeStr, '-', sizeof(HUINT8) * (pstContents->ulInputMax));
	pstContents->szVerifyPinCodeStr[pstContents->ulInputMax] = '\0';

	/* reset pointer */
	pstContents->nPinInputPos = 0;
	pstContents->bConfirmFlag = FALSE;

	/* invalidate word */
	for (i = 0; i < NX_PRISM_UI_PIN_DIGIT_MAX-1; i++)
	{
		ONDK_GWM_COM_Button_ChangeTitle(NX_PRISM_UI_CHANGEPIN_BTN_ID + i, (HCHAR*)pstContents->szNewPinCodeStr + i);
		ONDK_GWM_COM_Button_ChangeTitle(NX_PRISM_UI_CHANGEPIN_VERIFYBTN_ID + i, (HCHAR*)pstContents->szVerifyPinCodeStr + i);
	}

	ONDK_GWM_Obj_SetFocus (NX_PRISM_UI_CHANGEPIN_BTN_ID);

	return	ONDK_RET_OK;
}

static HBOOL			nx_comp_ChangePinDlalog_IsChangePinOk(NX_PinDialogInstance_t *pstContents)
{
	HBOOL		bChangePinCode = FALSE;
	if(pstContents == NULL)
	{
		return FALSE;
	}

	 /*  new와 verify를 비교 한다 */
	if (HLIB_STD_StrNCaseCmp((HCHAR*)pstContents->szVerifyPinCodeStr, (HCHAR*)pstContents->szNewPinCodeStr, NX_PRISM_UI_PIN_DIGIT_MAX-1) == 0)
	{
		/* for safety copy */
		pstContents->szVerifyPinCodeStr[NX_PRISM_UI_PIN_DIGIT_MAX - 1] = '\0';

		bChangePinCode = NX_SYSTEM_ResetPinCode((HCHAR*)pstContents->szVerifyPinCodeStr);
		if(!bChangePinCode){
			NX_APP_Error("[%s:%d] Error!!! New PIN CODE is Okay(%s)(%s)\n", __FUNCTION__, __HxLINE__, pstContents->szNewPinCodeStr, pstContents->szVerifyPinCodeStr);
			return FALSE;
		}
		return TRUE;
	}
	else
	{
		NX_APP_Error("[%s:%d] Error!!! Differnt Pin Code (%s)(%s)\n", __FUNCTION__, __HxLINE__, pstContents->szNewPinCodeStr, pstContents->szVerifyPinCodeStr);
		return FALSE;
	}
}

static HINT32			nx_comp_ChangePinDlalog_BackspacePosition (NX_PinDialogInstance_t *pstContents)
{
	HINT32	nObjId = 0;

	if((pstContents->nPinInputPos > 0) && (pstContents->nPinInputPos < NX_PRISM_UI_PIN_DIGIT_MAX-1))
	{
		/* backspace */
		pstContents->nPinInputPos--;

		if(pstContents->bConfirmFlag == FALSE)
		{
			/* set curr by inputed value */
			pstContents->szNewPinCodeStr[pstContents->nPinInputPos] = ('-');
			nObjId = NX_PRISM_UI_CHANGEPIN_BTN_ID;
		}
		else
		{
			/* set curr by inputed value */
			pstContents->szVerifyPinCodeStr[pstContents->nPinInputPos] = ('-');
			nObjId = NX_PRISM_UI_CHANGEPIN_VERIFYBTN_ID;
		}

		/* invalidate Curr */
		ONDK_GWM_APP_InvalidateObject(nObjId + pstContents->nPinInputPos + 1 );

		/* set focus next */
		ONDK_GWM_Obj_SetFocus (nObjId + pstContents->nPinInputPos);
	}
	else if(pstContents->nPinInputPos == 0)
	{
		if(pstContents->bConfirmFlag == TRUE)
		{
			pstContents->bConfirmFlag = FALSE; /* veryfy flag setting */
			pstContents->nPinInputPos = NX_PRISM_UI_PIN_DIGIT_MAX - 2;

			pstContents->szNewPinCodeStr[pstContents->nPinInputPos] = ('-');
			ONDK_GWM_Obj_SetFocus (nObjId + pstContents->nPinInputPos);
			ONDK_GWM_Obj_SetFocus (NX_PRISM_UI_CHANGEPIN_BTN_ID + pstContents->nPinInputPos);
		}
	}
	else
	{
		/* nothing to do */
	}

	return pstContents->nPinInputPos;
}


static ONDK_Result_t	nx_comp_ChangePinDlalog_DrawDefaultUI(NX_PinDialogInstance_t *pstContents)
{
	HINT32			i, j;
	ONDK_Rect_t 	stBGRect, stRect;

	HxSTD_memset(&stBGRect, 0x00, sizeof(ONDK_Rect_t));
	HxSTD_memset(&stRect, 0x00, sizeof(ONDK_Rect_t));

	stRect = ONDK_Rect(pstContents->appArea.x, pstContents->appArea.y, pstContents->appArea.w, pstContents->appArea.h);

	// Set OSD Area
	ONDK_GWM_APP_SetAppArea(stRect.x, stRect.y, stRect.w, stRect.h);

	// Draw Frame
	stRect = ONDK_Rect(0,0, pstContents->appArea.w, pstContents->appArea.h);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_PINDLG_FRAME_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_PINDLG_FRAME_ID, NX_PRISM_COMP_DrawDialugeWindow);

	// Draw Message
	stRect = ONDK_Rect(NX_DLG_CHANGEPIN_TEXT_X, NX_DLG_CHANGEPIN_TEXT_Y, NX_DLG_CHANGEPIN_TEXT_W, NX_DLG_CHANGEPIN_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_PIN_CODE_MESSAGE_ID, &stRect, (HCHAR*)pstContents->pszDlgMsg);
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_PIN_CODE_MESSAGE_ID, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_PIN_CODE_MESSAGE_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_PIN_CODE_MESSAGE_ID, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_PIN_CODE_MESSAGE_ID, COL(C_G_Whistle_100), COL(C_G_Whistle_100));

	// New pin text
	stRect = ONDK_Rect(NX_DLG_CHANGEPIN_NEWPIN_TEXT_X, NX_DLG_CHANGEPIN_NEWPIN_TEXT_Y, NX_DLG_CHANGEPIN_NEWPIN_TEXT_W, NX_DLG_CHANGEPIN_NEWPIN_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_CHANGEPIN_NEWPIN_ID, &stRect, (HCHAR*)HLIB_STD_StrUpper((HCHAR*)ONDK_GetString(RES_NEW_PASSWORD_ID)));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_CHANGEPIN_NEWPIN_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_CHANGEPIN_NEWPIN_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_CHANGEPIN_NEWPIN_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_CHANGEPIN_NEWPIN_ID, COL(C_G_Whistle_100), COL(C_G_Whistle_100));

	// Verify pin text
	stRect = ONDK_Rect(NX_DLG_CHANGEPIN_VERIFYPIN_TEXT_X, NX_DLG_CHANGEPIN_VERIFYPIN_TEXT_Y, NX_DLG_CHANGEPIN_VERIFYPIN_TEXT_W, NX_DLG_CHANGEPIN_VERIFYPIN_TEXT_H);
	ONDK_GWM_COM_Text_Create(NX_PRISM_UI_CHANGEPIN_VERIFYPIN_ID, &stRect, (HCHAR*)HLIB_STD_StrUpper((HCHAR*)ONDK_GetString(RES_VERIFY_PASSWORD_ID)));
	ONDK_GWM_COM_Text_SetFont(NX_PRISM_UI_CHANGEPIN_VERIFYPIN_ID, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_PRISM_UI_CHANGEPIN_VERIFYPIN_ID, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_PRISM_UI_CHANGEPIN_VERIFYPIN_ID, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_PRISM_UI_CHANGEPIN_VERIFYPIN_ID, COL(C_G_Whistle_100), COL(C_G_Whistle_100));

	for (i = 0; i < pstContents->ulInputMax; i++)
	{
		stRect = ONDK_Rect((NX_DLG_CHANGEPIN_NEWPIN_TEXT_X + NX_DLG_CHANGEPIN_NEWPIN_TEXT_W + 17) + i * (NX_PRISM_UI_PIN_CODE_W + NX_PRISM_UI_PIN_BTN_GAP), NX_DLG_CHANGE_PINCODE_NEWBUTTON_Y, NX_PRISM_UI_PIN_CODE_W, NX_PRISM_UI_PIN_CODE_H);
		ONDK_GWM_COM_Button_Create(NX_PRISM_UI_CHANGEPIN_BTN_ID + i, &stRect, (HCHAR*)pstContents->szNewPinCodeStr + i);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_CHANGEPIN_BTN_ID + i, NX_PRISM_COMP_DrawButton_PinCodeButton);
	}

	for (j = 0; j < pstContents->ulInputMax; j++)
	{
		stRect = ONDK_Rect((NX_DLG_CHANGEPIN_VERIFYPIN_TEXT_X + NX_DLG_CHANGEPIN_VERIFYPIN_TEXT_W + 17) + j * (NX_PRISM_UI_PIN_CODE_W + NX_PRISM_UI_PIN_BTN_GAP), NX_DLG_CHANGE_PINCODE_VERIFYBUTTON_Y, NX_PRISM_UI_PIN_CODE_W, NX_PRISM_UI_PIN_CODE_H);
		ONDK_GWM_COM_Button_Create(NX_PRISM_UI_CHANGEPIN_VERIFYBTN_ID + j, &stRect, (HCHAR*)pstContents->szVerifyPinCodeStr + j);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_CHANGEPIN_VERIFYBTN_ID + j, NX_PRISM_COMP_DrawButton_PinCodeButton);
	}
	ONDK_GWM_Obj_SetFocus (NX_PRISM_UI_CHANGEPIN_BTN_ID);

	return	ONDK_RET_OK;
}

static void				nx_comp_ChangePinDlalog_InitContents(NX_PinDialogInstance_t *pstContents)
{
	/* new/verify pin code 초기 화 */
	HxSTD_memset(pstContents->szNewPinCodeStr, '-', sizeof(HUINT8) * (pstContents->ulInputMax));
	pstContents->szNewPinCodeStr[pstContents->ulInputMax] = '\0';

	HxSTD_memset(pstContents->szVerifyPinCodeStr, '-', sizeof(HUINT8) * (pstContents->ulInputMax));
	pstContents->szVerifyPinCodeStr[pstContents->ulInputMax] = '\0';

	pstContents->nPinInputPos = 0;

	/* 초기에 new pin code부터 시작 한다 */
	pstContents->bConfirmFlag = FALSE;

	/* 죽을때 넘겨줄 this app func 저장. */
	pstContents->thisAppCallback = ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis());
}

static ONDK_Result_t	nx_comp_ChangePinDlalog_MsgGwmCreate(NX_PinDialogInstance_t *pstContents)
{
	nx_comp_ChangePinDlalog_InitContents(pstContents);
	nx_comp_ChangePinDlalog_DrawDefaultUI(pstContents);
	nx_comp_PinDlalog_SetOSDTimeOut(pstContents);

	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Change_Pincode_Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t			NX_PRISM_COMP_ChangePinDlalog_ProtoProc(NX_PinDialogInstance_t *pstContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_APP_Trace("NX_PRISM_COMP_ChangePinDlalog_ProtoProc message(0x%x) +\n", message);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_MESSAGE_BREAK;
	}

	switch (message){
		case MSG_GWM_CREATE:
			nx_comp_ChangePinDlalog_MsgGwmCreate(pstContents);
			break;

		case MSG_GWM_KEYUP:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
			if (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK)
			{
				ONDK_GWM_ResetTimer(NX_DLG_PINCODE_TIMER_ID);
			}
 			switch (p1)
			{
				case KEY_GUIDE:
				case KEY_MENU:
				case KEY_STANDBY:
					ONDK_GWM_APP_Destroy(NULL);
					return ONDK_MESSAGE_PASS;

				/* keys to pin quit */
				case KEY_BACK:
				case KEY_EXIT:
					ONDK_GWM_APP_Destroy(NULL);
					return	ONDK_MESSAGE_BREAK;

				/* go back */
				case KEY_ARROWLEFT:
					nx_comp_ChangePinDlalog_BackspacePosition(pstContents);
					return	ONDK_MESSAGE_BREAK;

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
					if(pstContents->bConfirmFlag == FALSE){
						if (nx_comp_ChangePinDlalog_CheckNewPinNumber(pstContents, (p1 - KEY_0)) >= NX_PRISM_UI_PIN_DIGIT_MAX-1){
							/* 자동으로 verify pin code로 전환 */
							pstContents->bConfirmFlag = TRUE; /* veryfy flag setting */
							pstContents->nPinInputPos = 0;
							ONDK_GWM_Obj_SetFocus (NX_PRISM_UI_CHANGEPIN_VERIFYBTN_ID);
						}
					}else{
						if (nx_comp_ChangePinDlalog_CheckConfirmPinNumber(pstContents, (p1 - KEY_0)) >= NX_PRISM_UI_PIN_DIGIT_MAX-1){
							if (nx_comp_ChangePinDlalog_IsChangePinOk(pstContents) == TRUE){
								nx_comp_PinDlalog_NotifyPinAuthorized(pstContents);
							}else{
								nx_comp_PinDlalog_NotifyPinDenied(pstContents);
							}
						}
					}
					return	ONDK_MESSAGE_BREAK;

				/* keys to ignore */
				case KEY_ARROWRIGHT:
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
			if (p1 == NX_DLG_PINCODE_TIMER_ID){
				nx_comp_PinDlalog_NotifyPinDenied(pstContents);
			}
			return ONDK_MESSAGE_BREAK;

		case MSG_GWM_DESTROY:
			break;
	}

	NX_APP_Trace("NX_PRISM_COMP_ChangePinDlalog_ProtoProc message(0x%x) -\n", message);

	return	ONDK_GWM_ProcessMessageDefault(message, hAction, p1, p2, p3);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

