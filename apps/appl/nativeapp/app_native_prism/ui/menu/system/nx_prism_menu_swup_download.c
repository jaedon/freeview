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
 * @file	  		nx_prism_menu_swup_download.c
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

#define	NX_MENU_SWUP_DN_ID						eNxSetting_SystemInfomation

#define NX_MENU_SWUP_DN_HALF_PROCESS			50

#define	NX_MENU_SWUP_DN_PROG_TEXT_X				1100
#define	NX_MENU_SWUP_DN_PROG_TEXT_Y				(470 - NX_PRISM_FONT_SIZE_22)
#define	NX_MENU_SWUP_DN_PROG_TEXT_W				100
#define	NX_MENU_SWUP_DN_PROG_TEXT_H				30

#define	NX_MENU_SWUP_DN_PROG_BAR_X				80
#define	NX_MENU_SWUP_DN_PROG_BAR_Y				485
#define	NX_MENU_SWUP_DN_PROG_BAR_W				1120
#define	NX_MENU_SWUP_DN_PROG_BAR_H				3


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum
{
	eNxMenuSwUpDn_BgLayer,
	eNxMenuSwUpDn_TitleText,
	eNxMenuSwUpDn_Msg01Text,
	eNxMenuSwUpDn_Msg02Text,
	eNxMenuSwUpDn_ProgressText,
	eNxMenuSwUpDn_ProgressBar,
	eNxMenuSwUpDn_CancelBtn,

	eNxMenuSwUpDn_MAX
} Nx_MenuSwUpDownloadcompIds_e;

typedef struct tag_NX_MENUSWUPDOWNLOAD_SET_CONTENT
{
	/**@breif Specific value */
	HINT32      lPrevCurIdx;				// menu system 화면으로 돌린다.
	DxSwUpdate_Step_e	eSwUpdateCurStep;
	DxSwUpdate_Source_e	eSwUpSource;
	Handle_t	hSwupdateHandle;

	HUINT8	    ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];
	HUINT8		szProgressPercent[NX_TEXT_SIZE_8];

	HCHAR		*pszTitleText, *pszMsg1Text, *pszMsg2Text;

} Nx_MenuSwUpDnContent_t;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_MenuSwUpDnContent_t		s_stSwUpDownloadContent;

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static inline Nx_MenuSwUpDnContent_t* nx_menuSwUpDownload_GetContents(void);
static void		nx_menuSwUpDownload_UpdateCurrentTime(Nx_MenuSwUpDnContent_t *pstContents);
static ONDK_Result_t	nx_menuSwUpDownload_SetDownloadUI(Nx_MenuSwUpDnContent_t *pstContents);
static ONDK_Result_t	nx_menuSwUpDownload_MsgGwmCreate(Nx_MenuSwUpDnContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuSwUpDownload_MsgGwmDestroy(Nx_MenuSwUpDnContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSwUpDownload_MsgGwmClicked(Nx_MenuSwUpDnContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSwUpDownload_MsgGwmKeyDown(Nx_MenuSwUpDnContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSwUpDownload_MsgGwmTimer(Nx_MenuSwUpDnContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_menuSwUpDownload_UpdateProgress(Nx_MenuSwUpDnContent_t *pstContents, HUINT32 ulPercent);
static ONDK_Result_t	nx_menuSwUpDownload_RecvSwUpEvent(Nx_MenuSwUpDnContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSwUpDownload_StartDownload(Nx_MenuSwUpDnContent_t *pstContents);
static ONDK_Result_t	nx_menuSwUpDownload_StartInstall(Nx_MenuSwUpDnContent_t *pstContents);

/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static inline Nx_MenuSwUpDnContent_t* nx_menuSwUpDownload_GetContents(void)
{
	return &s_stSwUpDownloadContent;
}

static void		nx_menuSwUpDownload_UpdateCurrentTime(Nx_MenuSwUpDnContent_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	HCHAR 			*pszDayOfWeek;
	HUINT32 		ulUnixtime = 0, ulWeekIndex = 0;
	HxDATETIME_t 	stDatetime;

	hError = NX_SYSTEM_GetTime(&ulUnixtime);
	if (hError == ERR_OK)
	{
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUnixtime, &stDatetime);
		ulWeekIndex = HLIB_DATETIME_GetWeekDayIndex(stDatetime.stDate);
		pszDayOfWeek = (HCHAR*)ONDK_GetString(NX_PRISM_UTIL_GetResStringIdByWeekIndex(ulWeekIndex));

		HxSTD_sprintf((char*)pstContents->szDate, "%02d.%02d.%04d", stDatetime.stDate.ucDay, stDatetime.stDate.ucMonth,
										stDatetime.stDate.usYear);

		HxSTD_sprintf((char*)pstContents->szTime, "%s %02d:%02d", pszDayOfWeek, stDatetime.stTime.ucHour,
												stDatetime.stTime.ucMinute);
	}
	else
	{
		pstContents->szDate[0] = '\0';
		pstContents->szTime[0] = '\0';
	}
}

static ONDK_Result_t	nx_menuSwUpDownload_SetDownloadUI(Nx_MenuSwUpDnContent_t *pstContents)
{
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };
	HUINT32 ulProgress = 0;

	if ((pstContents->eSwUpSource == eDxSWUPDATE_SOURCE_Rf) ||
		(pstContents->eSwUpSource == eDxSWUPDATE_SOURCE_Ip))
	{
	}
	else if ((pstContents->eSwUpSource == eDxSWUPDATE_SOURCE_Usb) ||
			 (pstContents->eSwUpSource == eDxSWUPDATE_SOURCE_File))
	{
	}
	else
	{
	}

	// title text
	stRect = ONDK_Rect(400, 180, 480, 40);
	pstContents->pszTitleText = (HCHAR*)ONDK_GetString(RES_DOWNLOAD_IN_PROGRESS_01_ID);
	ONDK_GWM_COM_Text_Create(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_TitleText, &stRect, pstContents->pszTitleText);
	ONDK_GWM_COM_Text_SetFont(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_TitleText, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_TitleText, NX_PRISM_FONT_SIZE_36);
	ONDK_GWM_COM_Text_SetAlign(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_TitleText, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_TitleText, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	// msg01 text
	stRect = ONDK_Rect(0, 250, 1280, 40);
	pstContents->pszMsg1Text = (HCHAR*)ONDK_GetString(RES_MESG_3834_ID);
	ONDK_GWM_COM_Text_Create(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_Msg01Text, &stRect, pstContents->pszMsg1Text);
	ONDK_GWM_COM_Text_SetFont(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_Msg01Text, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_Msg01Text, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_Msg01Text, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_Msg01Text, COL(C_T_Gong08), COL(C_T_Bang_100));

	// msg02 text
	stRect = ONDK_Rect(0, 300, 1280, 40);
	pstContents->pszMsg2Text = (HCHAR*)ONDK_GetString(RES_MESG_3905_ID);
	ONDK_GWM_COM_Text_Create(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_Msg02Text, &stRect, pstContents->pszMsg2Text);
	ONDK_GWM_COM_Text_SetFont(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_Msg02Text, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_Msg02Text, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_Msg02Text, TEXT_ALIGN_CENTER);
	ONDK_GWM_COM_Text_ChangeColor(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_Msg02Text, COL(C_T_Yoyo01), COL(C_T_Bang_100));


	HxSTD_snprintf((HCHAR *)pstContents->szProgressPercent, NX_TEXT_SIZE_8, "%d%%", (int)ulProgress);
	stRect = ONDK_Rect(NX_MENU_SWUP_DN_PROG_TEXT_X, NX_MENU_SWUP_DN_PROG_TEXT_Y,
						NX_MENU_SWUP_DN_PROG_TEXT_W, NX_MENU_SWUP_DN_PROG_TEXT_H);
	ONDK_GWM_COM_Text_Create((NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_ProgressText), &stRect, (HCHAR*)pstContents->szProgressPercent);
	ONDK_GWM_COM_Text_SetFont((NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_ProgressText), eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize((NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_ProgressText), NX_PRISM_FONT_SIZE_22);
	ONDK_GWM_COM_Text_SetAlign((NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_ProgressText), TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor((NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_ProgressText), COL(C_T_Pitch01), COL(C_T_Bang_100));

	// status bar
	stRect = ONDK_Rect(NX_MENU_SWUP_DN_PROG_BAR_X, NX_MENU_SWUP_DN_PROG_BAR_Y,
						NX_MENU_SWUP_DN_PROG_BAR_W, NX_MENU_SWUP_DN_PROG_BAR_H);
	ONDK_GWM_COM_Status_Create((NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_ProgressBar), &stRect, 100, 0);
	ONDK_GWM_COM_Status_SetColor((NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_ProgressBar), COL(C_G_Whistle_20), COL(C_G_Pitch01));
	ONDK_GWM_APP_SetObjectDrawMethod((NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_ProgressBar), NX_PRISM_COMP_DrawStatusBarB1_B2_B3);

	// cancel button
	stRect = ONDK_Rect(530, 600, 220, 50);
	ONDK_GWM_COM_Button_Create((NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_CancelBtn), &stRect, (HCHAR*)ONDK_GetString(RES_CANCEL_03_ID));
	ONDK_GWM_APP_SetObjectDrawMethod((NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_CancelBtn), NX_PRISM_COMP_DrawButton_A1);

	ONDK_GWM_Obj_SetFocus(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_CancelBtn);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSwUpDownload_MsgGwmCreate(Nx_MenuSwUpDnContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Rect_t 	stRect = { 0, 0, 0, 0 };

	HxSTD_MemSet(pstContents, 0, sizeof(Nx_MenuSwUpDnContent_t));

	pstContents->hSwupdateHandle = (Handle_t)HANDLE_NULL;
	pstContents->eSwUpSource = (DxSwUpdate_Source_e)p2;
	pstContents->lPrevCurIdx = p3;
	pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Unknown;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	stRect = ONDK_Rect(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);
	ONDK_GWM_COM_Rect_Create(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_BgLayer, &stRect, COL(C_G_Bang_100));

	// Draw - Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_SYSTEM_INFORMATION_ID), MAX_ELEMENT_TEXT_LENGTH);
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuSwUpDownload_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	NX_PRISM_COMP_Settings_DrawTitleLine();

	nx_menuSwUpDownload_SetDownloadUI(pstContents);

	// Start Download
	nx_menuSwUpDownload_StartDownload(pstContents);

	NX_COMMON_NOT_USED_PARAM(p1);

	return ONDK_RET_OK;
}

static ONDK_Result_t 	nx_menuSwUpDownload_MsgGwmDestroy(Nx_MenuSwUpDnContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSwUpDownload_MsgGwmClicked(Nx_MenuSwUpDnContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch ((HUINT32)p1)
	{
	case (NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_CancelBtn):
		if (pstContents->eSwUpdateCurStep == eDxSWUPDATE_STEP_Download)
		{
			ONDK_GWM_APP_Destroy(0);
			HxLOG_Print("\n\n\n[%s.%d] ----------->>> Stop DownLoad\n\n\n\n",__FUNCTION__,__LINE__);

		}
		else if (pstContents->eSwUpdateCurStep == eDxSWUPDATE_STEP_Install)
		{
			ONDK_GWM_APP_Destroy(0);
			HxLOG_Print("\n\n\n[%s.%d] ----------->>> Stop Install\n\n\n\n",__FUNCTION__,__LINE__);
		}
		break;

	default:
		break;
	}
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSwUpDownload_MsgGwmKeyDown(Nx_MenuSwUpDnContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
			//ONDK_GWM_APP_Destroy(0);

			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_GUIDE:
		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
			break;

		default:
			NX_APP_Print("nx_menuSysInfo_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSwUpDownload_MsgGwmTimer(Nx_MenuSwUpDnContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSwUpDownload_UpdateProgress(Nx_MenuSwUpDnContent_t *pstContents, HUINT32 ulPercent)
{
	HxSTD_snprintf((HCHAR *)pstContents->szProgressPercent, NX_TEXT_SIZE_8, "%d%%", (int)ulPercent);
	ONDK_GWM_COM_Text_SetText(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_ProgressText, (HCHAR*)pstContents->szProgressPercent);
	ONDK_GWM_COM_Status_SetValue(NX_MENU_SWUP_DN_ID | eNxMenuSwUpDn_ProgressBar, ulPercent);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSwUpDownload_RecvSwUpEvent(Nx_MenuSwUpDnContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(p1)
	{
	case eDxSWUPDATE_EVENT_DownloadProgress:
		HxLOG_Print("\n\n\n[%s.%d] eDxSWUPDATE_EVENT_DownloadProgress. event(p1)(%d), progress(p2)(%d), max(p3)(%d)\n\n\n",__FUNCTION__,__LINE__,
			p1, p2, p3);

		nx_menuSwUpDownload_UpdateProgress(pstContents, (HUINT32)p2);
		break;

	case eDxSWUPDATE_EVENT_DownloadSuccess:
		HxLOG_Print("\n\n\n[%s.%d] eDxSWUPDATE_EVENT_DownloadSuccess. event(p1)(%d), progress(p2)(%d), max(p3)(%d)\n\n\n",__FUNCTION__,__LINE__,
			p1, p2, p3);

		if (p2 >= NX_MENU_SWUP_DN_HALF_PROCESS)
		{
			pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Install;
			nx_menuSwUpDownload_StartInstall(pstContents);
		}
		break;

	case eDxSWUPDATE_EVENT_DownloadFail:
		pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Unknown;
		HxLOG_Print("\n\n\n[%s.%d] eDxSWUPDATE_EVENT_DownloadSuccess\n\n\n",__FUNCTION__,__LINE__);
		break;

	case eDxSWUPDATE_EVENT_InstallProgress:
		HxLOG_Print("\n\n\n[%s.%d] eDxSWUPDATE_EVENT_InstallProgress. event(p1)(%d), progress(p2)(%d), max(p3)(%d)\n\n\n",__FUNCTION__,__LINE__,
			p1, p2, p3);
		nx_menuSwUpDownload_UpdateProgress(pstContents, (HUINT32)p2);
		break;

	case eDxSWUPDATE_EVENT_InstallSuccess:
		HxLOG_Print("\n\n\n[%s.%d] eDxSWUPDATE_EVENT_InstallSuccess. event(p1)(%d), progress(p2)(%d), max(p3)(%d)\n\n\n",__FUNCTION__,__LINE__,
			p1, p2, p3);

		if (p2 >= NX_MENU_SWUP_DN_HALF_PROCESS)
		{
			pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Unknown;

		}
		break;

	case eDxSWUPDATE_EVENT_InstallFail:
		pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Unknown;
		HxLOG_Print("\n\n\n[%s.%d] eDxSWUPDATE_EVENT_InstallFail\n\n\n",__FUNCTION__,__LINE__);
		break;

	case eDxSWUPDATE_EVENT_Finish:
		HxLOG_Print("\n\n\n[%s.%d] received s/w update msg(eDxSWUPDATE_EVENT_Finish)\n\n\n",__FUNCTION__,__LINE__);
		break;

	default:
		break;
	}

	NX_COMMON_NOT_USED_PARAM(handle);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSwUpDownload_StartDownload(Nx_MenuSwUpDnContent_t *pstContents)
{
	if (NX_CORE_SWUP_StartDownloadWithType(NX_CORE_SWUP_GetSwUpSpec(), &pstContents->hSwupdateHandle) == ERR_FAIL)
	{
		HxLOG_Error("\n\nCan't Not Start S/W Update!!!\n\n");
		return ONDK_RET_FAIL;
	}
	pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Download;

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSwUpDownload_StartInstall(Nx_MenuSwUpDnContent_t *pstContents)
{
	if ((pstContents->hSwupdateHandle != (Handle_t)HANDLE_NULL) &&
		(pstContents->eSwUpdateCurStep == eDxSWUPDATE_STEP_Install))
	{
		NX_CORE_SWUP_StartInstallWithType(pstContents->hSwupdateHandle);
	}
	return ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_SwUpDownload_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{

	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuSwUpDnContent_t	*pstContents = NULL;

	pstContents = nx_menuSwUpDownload_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuSwUpDownload_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_menuSwUpDownload_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			eResult = nx_menuSwUpDownload_MsgGwmClicked(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuSwUpDownload_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuSwUpDownload_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_CORE_SWUP_NOTIFY:
			eResult = nx_menuSwUpDownload_RecvSwUpEvent(pstContents, (Handle_t)NULL, p1, p2, p3);
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

