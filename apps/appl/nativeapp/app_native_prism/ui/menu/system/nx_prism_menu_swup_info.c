/*
 * (c) 2011-2015 Humax Co., Ltd.
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
 * @file	  		nx_prism_menu_swup_info.c
 *
 * 	Description:  													\n
 *
 * @author															\n
 * @date															\n
 * @Attention		Copyright (c) 2015 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/


/******************************************************************************/
#define _________Private_Include______________________________________________
/******************************************************************************/
#include <nx_common.h>
#include <nx_core.h>

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>
#include <nx_prism_menu_items.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_MENU_SWUP_INFO_ID					eNxSetting_SystemInfomation

#define NX_MENU_SWUP_INFO_AREA_X		290
#define NX_MENU_SWUP_INFO_AREA_Y		197
#define NX_MENU_SWUP_INFO_AREA_W		700
#define NX_MENU_SWUP_INFO_AREA_H		326

/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/



typedef enum
{
	eNxMenuSwUpInfo_BgLayer,
	eNxMenuSwUpInfo_TitleTopImg,
	eNxMenuSwUpInfo_TitleText,
	eNxMenuSwUpInfo_TitleBottomImg,
	eNxMenuSwUpInfo_Title_Msg01Text,
	eNxMenuSwUpInfo_Title_Msg02Text,
	eNxMenuSwUpInfo_Title_CurVerLeftText,
	eNxMenuSwUpInfo_Title_CurVerRightText,
	eNxMenuSwUpInfo_Title_NewVerLeftText,
	eNxMenuSwUpInfo_Title_NewVerRightText,
	eNxMenuSwUpInfo_OkBtn,
	eNxMenuSwUpInfo_NoBtn,

	eNxMenuSwUpInfo_MAX
} Nx_MenuSwUpInfocompIds_e;

typedef struct tag_NX_MENUSWUPINFO_SET_CONTENT
{
	/**@breif Specific value */
	HINT32      lPrevCurIdx;				// menu system ȭ������ ������.
	DxSwUpdate_Step_e	eSwUpdateCurStep;
	DxSwUpdate_Source_e	eDetectedSource;

	HCHAR		szCurrentVer[MAX_ELEMENT_TEXT_LENGTH], szNewVer[MAX_ELEMENT_TEXT_LENGTH];
	HCHAR		*pszResultTitle, *pszResultMsg1, *pszResultMsg2, *pszCurVerText, *pszNewVerText;
	HCHAR		*pszSwupYesBtn, *pszSwupNoBtn;
} Nx_MenuSwUpInfoContent_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_MenuSwUpInfoContent_t		s_stSwUpInfoContent;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_MenuSwUpInfoContent_t* nx_menuSwUpInfo_GetContents(void);
static ONDK_Result_t	nx_menuSwUpInfo_SetNormalResultUI(Nx_MenuSwUpInfoContent_t *pstContents, DxSwUpdate_EventData_t *pstEventData);
static ONDK_Result_t	nx_menuSwUpInfo_SetInfoUI(Nx_MenuSwUpInfoContent_t *pstContents, DxSwUpdate_EventData_t *pstEventData);
static ONDK_Result_t	nx_menuSwUpInfo_MsgGwmCreate(Nx_MenuSwUpInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuSwUpInfo_MsgGwmDestroy(Nx_MenuSwUpInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSwUpInfo_MsgGwmClicked(Nx_MenuSwUpInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSwUpInfo_MsgGwmKeyDown(Nx_MenuSwUpInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSwUpInfo_MsgGwmTimer(Nx_MenuSwUpInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static inline Nx_MenuSwUpInfoContent_t* nx_menuSwUpInfo_GetContents(void)
{
	return &s_stSwUpInfoContent;
}

static ONDK_Result_t	nx_menuSwUpInfo_SetNormalResultUI(Nx_MenuSwUpInfoContent_t *pstContents, DxSwUpdate_EventData_t *pstEventData)
{
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	stRect = ONDK_Rect(NX_MENU_SWUP_INFO_AREA_X, NX_MENU_SWUP_INFO_AREA_Y, NX_MENU_SWUP_INFO_AREA_W, NX_MENU_SWUP_INFO_AREA_H);
	ONDK_GWM_COM_Rect_Create(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_BgLayer, &stRect, COL(C_G_Bang_90));

	// top line
	stRect = ONDK_Rect(NX_MENU_SWUP_INFO_AREA_X, NX_MENU_SWUP_INFO_AREA_Y, NX_MENU_SWUP_INFO_AREA_W, 1);
	ONDK_GWM_COM_Image_Create(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_TitleTopImg, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_TitleTopImg, (HCHAR*)RES_107_00_LINE_CONTEXT_L_PNG);

	// title text
	stRect = ONDK_Rect(NX_MENU_SWUP_INFO_AREA_X + 30, NX_MENU_SWUP_INFO_AREA_Y + 46, 640, 22);
	pstContents->pszResultTitle = (HCHAR*)ONDK_GetString(RES_SOFTWARE_UPDATE_02_ID);
	ONDK_GWM_COM_Text_Create(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_TitleText, &stRect, pstContents->pszResultTitle);
	ONDK_GWM_COM_Text_SetFont(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_TitleText, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_TitleText, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_TitleText, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_TitleText, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	// bottom line
	stRect = ONDK_Rect(NX_MENU_SWUP_INFO_AREA_X, NX_MENU_SWUP_INFO_AREA_Y + NX_MENU_SWUP_INFO_AREA_H, NX_MENU_SWUP_INFO_AREA_W, 1);
	ONDK_GWM_COM_Image_Create(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_TitleBottomImg, &stRect);
	ONDK_GWM_COM_Image_SetImage(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_TitleBottomImg, (HCHAR*)RES_107_00_LINE_CONTEXT_L_PNG);

	// msg 1
	stRect = ONDK_Rect(NX_MENU_SWUP_INFO_AREA_X + 30, NX_MENU_SWUP_INFO_AREA_Y + 81, 640, 22);
	pstContents->pszResultMsg1 = (HCHAR*)ONDK_GetString(RES_MESG_3192_ID);
	ONDK_GWM_COM_Text_Create(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_Msg01Text, &stRect, pstContents->pszResultMsg1);
	ONDK_GWM_COM_Text_SetFont(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_Msg01Text, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_Msg01Text, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_Msg01Text, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_Msg01Text, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	// msg 2
	stRect = ONDK_Rect(NX_MENU_SWUP_INFO_AREA_X + 30, NX_MENU_SWUP_INFO_AREA_Y + 81 + 32, 640, 22);
	pstContents->pszResultMsg2 = (HCHAR*)ONDK_GetString(RES_MESG_3193_ID);
	ONDK_GWM_COM_Text_Create(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_Msg02Text, &stRect, pstContents->pszResultMsg2);
	ONDK_GWM_COM_Text_SetFont(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_Msg02Text, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_Msg02Text, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_Msg02Text, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_Msg02Text, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	// current version left text
	stRect = ONDK_Rect(NX_MENU_SWUP_INFO_AREA_X + 38, NX_MENU_SWUP_INFO_AREA_Y + 138 + 36, 380, 22);
	pstContents->pszCurVerText = (HCHAR*)ONDK_GetString(RES_CURRENT_SOFTWARE_VERSION_ID);
	ONDK_GWM_COM_Text_Create(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_CurVerLeftText, &stRect, pstContents->pszCurVerText);
	ONDK_GWM_COM_Text_SetFont(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_CurVerLeftText, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_CurVerLeftText, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_CurVerLeftText, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_CurVerLeftText, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	// current version right text
	stRect = ONDK_Rect(NX_MENU_SWUP_INFO_AREA_X + 432, NX_MENU_SWUP_INFO_AREA_Y + 138 + 36, 230, 22);
	HxSTD_snprintf(pstContents->szCurrentVer, MAX_ELEMENT_TEXT_LENGTH, "%s", pstEventData->unEventData.stDetectEvent.szCurrentVer);
	ONDK_GWM_COM_Text_Create(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_CurVerRightText, &stRect, pstContents->szCurrentVer);
	ONDK_GWM_COM_Text_SetFont(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_CurVerRightText, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_CurVerRightText, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_CurVerRightText, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_CurVerRightText, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	// new version left text
	stRect = ONDK_Rect(NX_MENU_SWUP_INFO_AREA_X + 38, NX_MENU_SWUP_INFO_AREA_Y + 138 + 36 + 32, 380, 221);
	pstContents->pszNewVerText = (HCHAR*)ONDK_GetString(RES_MESG_3195_ID);
	ONDK_GWM_COM_Text_Create(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_NewVerLeftText, &stRect, pstContents->pszNewVerText);
	ONDK_GWM_COM_Text_SetFont(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_NewVerLeftText, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_NewVerLeftText, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_NewVerLeftText, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_NewVerLeftText, COL(C_T_Yoyo01), COL(C_T_Bang_100));

	// new version right text
	stRect = ONDK_Rect(NX_MENU_SWUP_INFO_AREA_X + 432, NX_MENU_SWUP_INFO_AREA_Y + 138 + 36 + 32, 230, 22);
	HxSTD_snprintf(pstContents->szNewVer, MAX_ELEMENT_TEXT_LENGTH, "%s", pstEventData->unEventData.stDetectEvent.szFoundVer);
	ONDK_GWM_COM_Text_Create(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_NewVerRightText, &stRect, pstContents->szNewVer);
	ONDK_GWM_COM_Text_SetFont(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_NewVerRightText, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_NewVerRightText, NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_NewVerRightText, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_Title_NewVerRightText, COL(C_T_Yoyo01), COL(C_T_Bang_100));

	// Ok button
	pstContents->pszSwupYesBtn = (HCHAR*)ONDK_GetString(RES_YES_01_ID);
	stRect = ONDK_Rect(NX_MENU_SWUP_INFO_AREA_X + 123, NX_MENU_SWUP_INFO_AREA_Y + 246, 220, 50);
	ONDK_GWM_COM_Button_Create(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_OkBtn, &stRect, pstContents->pszSwupYesBtn);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_OkBtn, NX_PRISM_COMP_DrawButton_A1);

	// No button
	pstContents->pszSwupNoBtn = (HCHAR*)ONDK_GetString(RES_NO_03_ID);
	stRect = ONDK_Rect(NX_MENU_SWUP_INFO_AREA_X + 123 + 220 + 15, NX_MENU_SWUP_INFO_AREA_Y + 246, 220, 50);
	ONDK_GWM_COM_Button_Create(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_NoBtn, &stRect, pstContents->pszSwupNoBtn);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_NoBtn, NX_PRISM_COMP_DrawButton_A1);

	ONDK_GWM_Obj_SetFocus(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_OkBtn);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSwUpInfo_SetInfoUI(Nx_MenuSwUpInfoContent_t *pstContents, DxSwUpdate_EventData_t *pstEventData)
{

	if ((pstContents->eDetectedSource == eDxSWUPDATE_SOURCE_Rf) ||
		(pstContents->eDetectedSource == eDxSWUPDATE_SOURCE_Ip))
	{
		nx_menuSwUpInfo_SetNormalResultUI(pstContents, pstEventData);
	}
	else if ((pstContents->eDetectedSource == eDxSWUPDATE_SOURCE_Usb) ||
			 (pstContents->eDetectedSource == eDxSWUPDATE_SOURCE_File))
	{
	}
	else
	{
	}

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSwUpInfo_MsgGwmCreate(Nx_MenuSwUpInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	DxSwUpdate_EventData_t *pstDetectedInform = NULL;

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_MenuSwUpInfoContent_t));


	pstContents->lPrevCurIdx = p3;
	pstContents->eDetectedSource  = eDxSWUPDATE_SOURCE_None;
	pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Unknown;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	pstDetectedInform = NX_CORE_SWUP_GetDetectedInform(NX_CORE_SWUP_GetSwUpSpec());
	if(pstDetectedInform != NULL)
	{
		pstContents->eDetectedSource = pstDetectedInform->eSourceType;
		if (pstContents->eDetectedSource != eDxSWUPDATE_SOURCE_None)
		{
			pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Detect;
		}
	}

	nx_menuSwUpInfo_SetInfoUI(pstContents, pstDetectedInform);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);

	return ONDK_RET_OK;
}

static ONDK_Result_t 	nx_menuSwUpInfo_MsgGwmDestroy(Nx_MenuSwUpInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSwUpInfo_MsgGwmClicked(Nx_MenuSwUpInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch ((HUINT32)p1)
	{
	case (NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_OkBtn):
		if (pstContents->eSwUpdateCurStep == eDxSWUPDATE_STEP_Detect)
		{
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_MENU_SwUpDownload_Proc",
								(APP_DEFAULT_PRIORITY | UISTATE_NORMAL),
								NX_PRISM_MENU_SwUpDownload_Proc, 0, 0, pstContents->eDetectedSource, pstContents->lPrevCurIdx);
		}
	break;

	case (NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_NoBtn):
		{
			ONDK_GWM_APP_Destroy(0);
			//ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_System_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
	             //   (ONDK_GWM_Callback_t)NX_PRISM_MENU_System_Proc, 0, 0, 0, pstContents->lPrevCurIdx);
		}
		break;

	default:
		break;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSwUpInfo_MsgGwmKeyDown(Nx_MenuSwUpInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, lPrevCurIdx = 0;

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
	        lPrevCurIdx = pstContents->lPrevCurIdx;
			//ONDK_GWM_APP_Destroy(0);
			//ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_System_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
	         //       (ONDK_GWM_Callback_t)NX_PRISM_MENU_System_Proc, 0, 0, 0, lPrevCurIdx);
			return	ONDK_MESSAGE_BREAK;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_GUIDE:
		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_ARROWLEFT:
			if (pstContents->eSwUpdateCurStep == eDxSWUPDATE_STEP_Detect)
			{
				ONDK_GWM_Obj_SetFocus(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_OkBtn);
			}
			return	ONDK_MESSAGE_BREAK;

		case KEY_ARROWRIGHT:
			if (pstContents->eSwUpdateCurStep == eDxSWUPDATE_STEP_Detect)
			{
				ONDK_GWM_Obj_SetFocus(NX_MENU_SWUP_INFO_ID | eNxMenuSwUpInfo_NoBtn);
			}
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
			break;

		default:
			NX_APP_Print("nx_menuSysInfo_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSwUpInfo_MsgGwmTimer(Nx_MenuSwUpInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_SwUpInfo_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{

	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuSwUpInfoContent_t	*pstContents = NULL;

	pstContents = nx_menuSwUpInfo_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuSwUpInfo_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_menuSwUpInfo_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			eResult = nx_menuSwUpInfo_MsgGwmClicked(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuSwUpInfo_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuSwUpInfo_MsgGwmTimer(pstContents, p1, p2, p3);
			break;
		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Menu)
			{
				ONDK_GWM_APP_Destroy(0);
			}
			break;

		default:
			break;
	}

	if(eResult != ONDK_RET_OK)
	{
		return eResult;
	}

	return	ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}

