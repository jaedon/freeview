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
 * @file	  		nx_prism_component_ipdialog.c
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
#include <nx_prism_component_ipdialog.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_DLG_IP_TIMER_ID						    	1000

#define	NX_DLG_IP_TEXT_X								30
#define	NX_DLG_IP_TEXT_Y								23
#define	NX_DLG_IP_TEXT_W								640
#define	NX_DLG_IP_TEXT_H								64 + 8

#define	NX_DLG_IP_BUTTON_X						    	65
#define	NX_DLG_IP_BUTTON_Y						    	108
#define	NX_DLG_IP_BUTTON_W						    	40
#define	NX_DLG_IP_BUTTON_H						    	50

#define	NX_DLG_IP_BG_X							        290
#define	NX_DLG_IP_BG_Y							        266
#define	NX_DLG_IP_BG_W							        700
#define	NX_DLG_IP_BG_H							        188

#define	NX_DLG_IP_TEXT_DOT_X						    190
#define	NX_DLG_IP_TEXT_DOT_X_GAP					    150
#define	NX_DLG_IP_TEXT_DOT_Y						    158
#define	NX_DLG_IP_TEXT_DOT_W						    20
#define	NX_DLG_IP_TEXT_DOT_H							36

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_comp_IpDlalog_SetOSDTimeOut(NX_IpDialogInstance_t *pstContents);
static ONDK_Result_t	nx_comp_IpDlalog_DrawDefaultUI(NX_IpDialogInstance_t *pstContents);
static ONDK_Result_t	nx_comp_IpDlalog_InitContents(NX_IpDialogInstance_t *pstContents);
static ONDK_Result_t	nx_comp_IpDlalog_MsgGwmCreate(NX_IpDialogInstance_t *pstContents);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/


static HERROR	nx_comp_IpDlalog_AddrN2A( HULONG ulAddr, HCHAR *pszAddr, HINT32 nbufSize )
{
	snprintf( pszAddr, nbufSize, "%03ld.%03ld.%03ld.%03ld",
			(ulAddr & 0x000000FF), (ulAddr & 0x0000FF00)>>8, (ulAddr &0x00FF0000)>>16, (ulAddr & 0xFF000000)>>24 );

	return ERR_OK;
}

static HERROR   nx_comp_IpDlalog_AddrA2N( HCHAR *pszAddr, HULONG *pulAddr )
{
	HULONG	octet[4] = { 0 };

	sscanf ( pszAddr, "%ld.%ld.%ld.%ld", &octet[0], &octet[1], &octet[2], &octet[3] );

	*pulAddr = 0;
	*pulAddr |=	octet[0];
	*pulAddr |= ( octet[1] << 8 );
	*pulAddr |= ( octet[2] << 16 );
	*pulAddr |= ( octet[3] << 24 );

	return ERR_OK;
}

static ONDK_Result_t	nx_comp_IpDlalog_SetOSDTimeOut(NX_IpDialogInstance_t *pstContents)
{
	HUINT32		ulTimeOutTime = 0;

	if (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK)
	{
		ulTimeOutTime = (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK) * NX_COMMON_TIME_SECOND;
		ONDK_GWM_SetTimer(NX_DLG_IP_TIMER_ID, ulTimeOutTime);
	}

	NX_APP_Info("OSD Time Out Time (%d Sec)\n", ulTimeOutTime);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_comp_IpDlalog_DrawDefaultUI(NX_IpDialogInstance_t *pstContents)
{
	ONDK_Rect_t 	stBGRect, stRect;
    HINT32          x = 0, y = 0, w = 0, h = 0;

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
		ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_IPDLG_BG_ID, &stRect, 0);
		ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_IPDLG_BG_ID, NX_PRISM_COMP_DrawWindowPopUp);
	}

	// Draw Frame
	stRect = ONDK_Rect(pstContents->appArea.x, pstContents->appArea.y, pstContents->appArea.w, pstContents->appArea.h);
	ONDK_GWM_COM_Rect_Create(NX_PRISM_UI_IPDLG_FRAME_ID, &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_PRISM_UI_IPDLG_FRAME_ID, NX_PRISM_COMP_DrawDialugeWindow);

	// Draw Message
	x = pstContents->appArea.x + NX_DLG_IP_TEXT_X;
    y = pstContents->appArea.y + NX_DLG_IP_TEXT_Y;
    w = NX_DLG_IP_TEXT_W;
    h = NX_DLG_IP_TEXT_H;
	stRect = ONDK_Rect(x, y,w,h);

	NX_PRISM_COMP_DrawDialugeWindowMessage(&stRect, pstContents->pszDlgMsg, pstContents->colorMessageText);

	if (pstContents->ulInputMax == 0)
	{
		return	ONDK_RET_INVARG;
	}

	// Make the new GWM key input object
	if(eNxIPDialog_Type_Ip4_Input == pstContents->eDialogType)
    {
    	x = pstContents->appArea.x + NX_DLG_IP_BUTTON_X;
        y = pstContents->appArea.y + NX_DLG_IP_BUTTON_Y;
        w = NX_DLG_IP_BUTTON_W * (pstContents->ulInputMax + 1);
        h = NX_DLG_IP_BUTTON_H;

    	stRect = ONDK_Rect(x, y , w, h);

    	ONDK_GWM_COM_Input_Create (NX_PRISM_UI_IP_DATA_ID, &stRect, (char *)NULL, pstContents->ulInputMax - 1, (char *)pstContents->szIp4InputValueStr);
    	ONDK_GWM_APP_SetObjectDrawMethod (NX_PRISM_UI_IP_DATA_ID, NX_PRISM_COMP_DrawButton_IpDataButton);
        ONDK_GWM_COM_Input_SetStringMode(NX_PRISM_UI_IP_DATA_ID,eGUI_INPUTOBJ_MODE_IP);
    }

	ONDK_GWM_Obj_SetFocus (NX_PRISM_UI_IP_DATA_ID);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_comp_IpDlalog_InitContents(NX_IpDialogInstance_t *pstContents)
{
	/* init word */
	HxSTD_memset(pstContents->szIp4InputValueStr, '\0', sizeof(HUINT8) * (pstContents->ulInputMax));
	pstContents->szIp4InputValueStr[pstContents->ulInputMax] = '\0';

    if(eNxIPDialog_Type_Ip4_Input == pstContents->eDialogType)
    {
        if (pstContents->ulInputValue >= 0)
    	{
	        nx_comp_IpDlalog_AddrN2A((HULONG)pstContents->ulInputValue, (HCHAR*)pstContents->szIp4InputValueStr, pstContents->ulInputMax);
        }
    }
	/* init pointer */
	pstContents->nIpInputPos = 0;
	pstContents->thisAppCallback = ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis());

	return	ONDK_RET_OK;
}

static ONDK_Result_t    nx_comp_IpDlalog_MsgGwmInput(NX_IpDialogInstance_t *pstContents, HINT32 message, HINT32 lObjId)
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
		ulInputLen = HLIB_STD_StrNLen((char*)szInputString, NX_PRISM_UI_IP_V4_INPUT_MAX);
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

static ONDK_Result_t	nx_comp_IpDlalog_MsgGwmCreate(NX_IpDialogInstance_t *pstContents)
{
	nx_comp_IpDlalog_InitContents(pstContents);
	nx_comp_IpDlalog_DrawDefaultUI(pstContents);
	nx_comp_IpDlalog_SetOSDTimeOut(pstContents);

	return	ONDK_RET_OK;
}

static ONDK_Result_t		nx_comp_IpDialog_MsgGwmInput(NX_IpDialogInstance_t *pstContents, HINT32 message, HINT32 lObjId)
{
	HUINT8		*szInputString = NULL;
	HULONG		ulInputValue = 0;

	if (message == MSG_GWM_INPUTCOMPLETE)
	{
		szInputString = (HUINT8 *)ONDK_GWM_COM_Input_GetText (lObjId);
		if (szInputString == NULL)
		{
			return	ONDK_RET_INVARG;
		}
        nx_comp_IpDlalog_AddrA2N((HCHAR*)szInputString,&ulInputValue);
	}
	else
	{
		ulInputValue = pstContents->ulInputValue;
	}

	ONDK_GWM_SendMessage(pstContents->parentAppCallback, MSG_APP_SETTING_INPUT_VALUE_CHANGED, (HINT32)NULL, ulInputValue, 0, 0);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

extern ONDK_Result_t	NX_PRISM_COMP_IpDlalog_SetIp4InputDialog(NX_IpDialogInstance_t *pstContents, ONDK_GWM_Callback_t parentAppCallback,HUINT32 ulDlgAttr, HUINT8 *pszDlgMsg, HULONG ulInputValue)
{
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

	HxSTD_memset(pstContents, 0x00, sizeof(NX_IpDialogInstance_t));
    pstContents->appArea = ONDK_Rect(NX_DLG_IP_BG_X, NX_DLG_IP_BG_Y, NX_DLG_IP_BG_W, NX_DLG_IP_BG_H);

	pstContents->parentAppCallback = parentAppCallback;
	pstContents->ulDlgAttributes = ulDlgAttr;
	pstContents->ulInputMax = NX_PRISM_UI_IP_V4_INPUT_MAX;
	pstContents->eDialogType = eNxIPDialog_Type_Ip4_Input;
	pstContents->ulInputValue = ulInputValue;
    pstContents->colorMessageText = COL(C_T_Whistle_100);

	if (pszDlgMsg == NULL)
	{
		pstContents->pszDlgMsg = NULL;
	}
	else
	{
		pstContents->pszDlgMsg = pszDlgMsg;
	}

	// parent app
	if (parentAppCallback == NULL)
	{
		NX_APP_Error("[%s:%d] Error!!! parentAppCallback is NULL(%d)\n", __FUNCTION__, __HxLINE__);
		return	ONDK_RET_INVARG;
	}

	return	ONDK_RET_OK;
}

ONDK_Result_t	NX_PRISM_COMP_IpDlalog_ProtoProc(NX_IpDialogInstance_t *pstContents, HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_APP_Trace("NX_PRISM_COMP_TimeDlalog_ProtoProc message(0x%x) +\n", message);

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL) \n");
		return	ONDK_MESSAGE_BREAK;
	}

	switch (message){
		case MSG_GWM_CREATE:
			nx_comp_IpDlalog_MsgGwmCreate(pstContents);
			break;

		case MSG_GWM_KEYUP:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
			break;

		case MSG_GWM_INPUTCOMPLETE:
		case MSG_GWM_INPUTCANCELD:
			nx_comp_IpDialog_MsgGwmInput(pstContents, message, p1);
			ONDK_GWM_APP_Destroy(NULL);
			break;
		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon(&p1);
			if (pstContents->ulDlgAttributes & NX_DLG_TIMEOUT_MASK)
			{
				ONDK_GWM_ResetTimer(NX_DLG_IP_TIMER_ID);
			}
 			switch (p1)
			{
				case KEY_GUIDE:
				case KEY_MENU:
				case KEY_STANDBY:
					ONDK_GWM_APP_Destroy(NULL);
					return	ONDK_MESSAGE_PASS;
				case KEY_EXIT:
                case KEY_BACK:
                    ONDK_GWM_APP_Destroy(NULL);
					return	ONDK_MESSAGE_BREAK;
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
            /*
			if (p1 == NX_DLG_IP_TIMER_ID)
            {
				nx_comp_IpDialog_MsgGwmInput(pstContents, MSG_GWM_INPUTCANCELD, NX_PRISM_UI_IP_DATA_ID);
				ONDK_GWM_APP_Destroy(NULL);
			}
            */
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

