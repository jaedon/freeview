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
 * @file	  		nx_prism_menu_sysinfo.c
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
#include <nx_core.h>

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>
#include <nx_prism_menu_items.h>

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	NX_MENU_SYSINFO_ID							eNxSetting_SystemInfomation

#define	NX_MENU_SYSINFO_INFOBOX_X					60
#define	NX_MENU_SYSINFO_INFOBOX_Y					114
#define	NX_MENU_SYSINFO_INFOBOX_W					750
#define	NX_MENU_SYSINFO_INFOBOX_H					418

#define	NX_MENU_SYSINFO_ITEM_LABEL_X				(NX_MENU_SYSINFO_INFOBOX_X + 18)
#define	NX_MENU_SYSINFO_ITEM_LABEL_Y				(NX_MENU_SYSINFO_INFOBOX_Y + 36 - NX_PRISM_FONT_SIZE_22)
#define	NX_MENU_SYSINFO_ITEM_LABEL_W				450
#define	NX_MENU_SYSINFO_ITEM_LABEL_LS				40

#define	NX_MENU_SYSINFO_ITEM_VALUE_X				(NX_MENU_SYSINFO_INFOBOX_X + 482)
#define	NX_MENU_SYSINFO_ITEM_VALUE_Y				(NX_MENU_SYSINFO_INFOBOX_Y + 36 - NX_PRISM_FONT_SIZE_22)
#define	NX_MENU_SYSINFO_ITEM_VALUE_W				250
#define	NX_MENU_SYSINFO_ITEM_VALUE_LS				40

#define	NX_MENU_SYSINFO_CHECK_NEW_SW_BTN_X			205
#define	NX_MENU_SYSINFO_CHECK_NEW_SW_BTN_Y			615
#define	NX_MENU_SYSINFO_CHECK_NEW_SW_BTN_W			460
#define	NX_MENU_SYSINFO_CHECK_NEW_SW_BTN_H			55

#define	NX_MENU_SYSINFO_ITEM_MAX_NUM				12

#define NX_MENU_SYSINFO_SWUPDATE_LODING_TIMER_ID	(NX_PRISM_UI_DLG_PROCESS_MSG_ITEM_ID + 10)
#define	NX_MENU_SYSINFO_SWUPDATE_LODING_TIMER_PERIOD			(500)	/* 700msec */


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum
{
	eNxMenuSysInfo_InfoBox,
	eNxMenuSysInfo_ModelName_Label,
	eNxMenuSysInfo_SoftwareVersion_Label,
	eNxMenuSysInfo_LoaderVersion_Label,
	eNxMenuSysInfo_CIPlusVersion_Label,
	eNxMenuSysInfo_SystemId_Label,
	eNxMenuSysInfo_UpdateDate_Label,
	eNxMenuSysInfo_MicomVersion_Label,
	eNxMenuSysInfo_LANMacAddr_Label,
	eNxMenuSysInfo_WLANMacAddr_Label,
	eNxMenuSysInfo_BluetoothMacAddr_Label,
	eNxMenuSysInfo_ModelName_Value,
	eNxMenuSysInfo_SoftwareVersion_Value,
	eNxMenuSysInfo_LoaderVersion_Value,
	eNxMenuSysInfo_CIPlusVersion_Value,
	eNxMenuSysInfo_SystemId_Value,
	eNxMenuSysInfo_UpdateDate_Value,
	eNxMenuSysInfo_MicomVersion_Value,
	eNxMenuSysInfo_LANMacAddr_Value,
	eNxMenuSysInfo_WLANMacAddr_Value,
	eNxMenuSysInfo_BluetoothMacAddr_Value,
	eNxMenuSysInfo_CheckNewSWButton,

	// swupdate
	eNxMenuSysInfo_SwUpdateLoadingBg,
	eNxMenuSysInfo_SwUpdateLodingImg,

	eNxMenuSysInfo_MAX
} Nx_MenuSysInfoUicompIds_e;


typedef struct tagSYSINFO_LIST
{
	HUINT32		ulSysItemNum;
	HUINT32		aulSysItemList[NX_MENU_SYSINFO_ITEM_MAX_NUM];
	HUINT32		aulSysItemValueList[NX_MENU_SYSINFO_ITEM_MAX_NUM];
} NxUiSysInfoItemList_t;


typedef struct tag_NX_MENUSYSINFO_SET_CONTENT
{
	/**@breif Specific value */
	HUINT8	    ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	HCHAR       *pszLANMacAddr;
	HCHAR       *pszWLANMacAddr;
	HCHAR       *pszBluetoothMacAddr;

	HINT32      lPrevCurIdx;

	HUINT8		**szItemTextList;
	NxUiSysInfoItemList_t	stItemList;

	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];

	// S/W Update data
	Handle_t	hSwupdateHandle;
	DxSwUpdate_Step_e	eSwUpdateCurStep;
	HINT32		nLoadingImgIndex;
	HCHAR		szCurrentVer[MAX_ELEMENT_TEXT_LENGTH], szNewVer[MAX_ELEMENT_TEXT_LENGTH];
	HCHAR		*pszResultTitle, *pszResultMsg1, *pszResultMsg2, *pszCurVerText, *pszNewVerText;
	HCHAR		*pszSwupYesBtn, *pszSwupNoBtn;

} Nx_MenuSysInfoContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_MenuSysInfoContent_t		s_stSysInfoContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static inline Nx_MenuSysInfoContent_t* nx_menuSysInfo_GetContents(void);

static void				nx_menuSysInfo_UpdateCurrentTime(Nx_MenuSysInfoContent_t *pstContents);

static ONDK_Result_t	nx_menuSysInfo_DrawCheckNewSWButton(void);
static ONDK_Result_t	nx_menuSysInfo_DrawSystemInfo(Nx_MenuSysInfoContent_t *pstContents);
static ONDK_Result_t	nx_menuSysInfo_DrawInfoBox(Nx_MenuSysInfoContent_t *pstContents);
static ONDK_Result_t	nx_menuSysInfo_DrawDefaultUI(Nx_MenuSysInfoContent_t *pstContents);

static ONDK_Result_t	nx_menuSysInfo_AddItemList(NxUiSysInfoItemList_t *pstSysItemList, HUINT32 ulItemId, HUINT32 ulItemValueId);
static ONDK_Result_t	nx_menuSysInfo_LoadSysInfoItemsValue(Nx_MenuSysInfoContent_t *pstContents);
static ONDK_Result_t	nx_menuSysInfo_InitSysInfoItems(Nx_MenuSysInfoContent_t *pstContents);

static ONDK_Result_t	nx_menuSysInfo_MsgGwmCreate(Nx_MenuSysInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuSysInfo_MsgGwmDestroy(Nx_MenuSysInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSysInfo_MsgGwmKeyDown(Nx_MenuSysInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSysInfo_MsgGwmClicked(Nx_MenuSysInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSysInfo_MsgGwmTimer(Nx_MenuSysInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_menuSysInfo_EndLoadingAnimation(void);
static ONDK_Result_t	nx_menuSysInfo_UpdateLoadingAnimation(void);
static ONDK_Result_t	nx_menuSysInfo_CbLoadingDrawAnimation(ONDKSurfaceHandle screen, ONDK_Region_t *area,
												ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop);
static ONDK_Result_t 	nx_menuSysInfo_AddItemSwupdateLoading(Nx_MenuSysInfoContent_t *pstContents);
static ONDK_Result_t	nx_menuSysInfo_RecvSwUpEvent(Nx_MenuSysInfoContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSysInfo_StartDetect(Nx_MenuSysInfoContent_t *pstContents);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_MenuSysInfoContent_t* nx_menuSysInfo_GetContents(void)
{
	return	&s_stSysInfoContent;
}


static void		nx_menuSysInfo_UpdateCurrentTime(Nx_MenuSysInfoContent_t *pstContents)
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


static ONDK_Result_t    nx_menuSysInfo_DrawCheckNewSWButton(void)
{
	ONDK_Rect_t     stRect = { 0, 0, 0, 0 };

	stRect = ONDK_Rect(NX_MENU_SYSINFO_CHECK_NEW_SW_BTN_X, NX_MENU_SYSINFO_CHECK_NEW_SW_BTN_Y,
	                    NX_MENU_SYSINFO_CHECK_NEW_SW_BTN_W, NX_MENU_SYSINFO_CHECK_NEW_SW_BTN_H);
	ONDK_GWM_COM_Button_Create((NX_MENU_SYSINFO_ID | eNxMenuSysInfo_CheckNewSWButton), &stRect, (HCHAR *)ONDK_GetString(RES_CHECK_NEW_SOFTWARE_ID));
	ONDK_GWM_APP_SetObjectDrawMethod((NX_MENU_SYSINFO_ID | eNxMenuSysInfo_CheckNewSWButton), NX_PRISM_COMP_DrawButton_B1);

	return  ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSysInfo_DrawSystemInfo(Nx_MenuSysInfoContent_t *pstContents)
{
	HINT8			*pstItemStrID = NULL;
	HUINT32			i = 0;
	ONDK_Rect_t     stItemRect = { 0, 0, 0, 0 }, stValueRect = { 0, 0, 0, 0 };

	stItemRect = ONDK_Rect(NX_MENU_SYSINFO_ITEM_LABEL_X, NX_MENU_SYSINFO_ITEM_LABEL_Y, NX_MENU_SYSINFO_ITEM_LABEL_W, NX_MENU_SYSINFO_ITEM_LABEL_LS);
	stValueRect = ONDK_Rect(NX_MENU_SYSINFO_ITEM_VALUE_X, NX_MENU_SYSINFO_ITEM_VALUE_Y, NX_MENU_SYSINFO_ITEM_VALUE_W, NX_MENU_SYSINFO_ITEM_VALUE_LS);

	for (i = 0; i < pstContents->stItemList.ulSysItemNum; i++)
	{
		switch (pstContents->stItemList.aulSysItemList[i])
		{
			case eNxMenuSysInfo_ModelName_Label:			pstItemStrID = RES_MODEL_NAME_02_ID;		break;
			case eNxMenuSysInfo_SoftwareVersion_Label:		pstItemStrID = RES_SOFTWARE_VERSION_01_ID;	break;
			case eNxMenuSysInfo_LoaderVersion_Label:		pstItemStrID = RES_LOADER_VERSION_00_ID;	break;
			case eNxMenuSysInfo_CIPlusVersion_Label:		pstItemStrID = RES_CIPLUS_VERSION_ID;		break;
			case eNxMenuSysInfo_SystemId_Label:				pstItemStrID = RES_SYSTEM_ID_ID;			break;
			case eNxMenuSysInfo_UpdateDate_Label:			pstItemStrID = RES_UPDATE_DATE_01_ID;		break;
			case eNxMenuSysInfo_MicomVersion_Label:			pstItemStrID = RES_MICOM_VERSION_01_ID;		break;
			case eNxMenuSysInfo_LANMacAddr_Label:			pstItemStrID = RES_LAN_ID;					break;
			case eNxMenuSysInfo_WLANMacAddr_Label:			pstItemStrID = RES_WLAN_ID;					break;
			case eNxMenuSysInfo_BluetoothMacAddr_Label:		pstItemStrID = RES_BLUETOOTH_01_ID;			break;
			default:										pstItemStrID = RES_UNKNOWN_ID;				break;
		}

		switch (pstContents->stItemList.aulSysItemList[i])
		{
			case eNxMenuSysInfo_LANMacAddr_Label:
				pstContents->pszLANMacAddr = (HCHAR *)NX_APP_Malloc(sizeof(HCHAR) * NX_TEXT_SIZE_128);
				if (pstContents->pszLANMacAddr)
				{
					HxSTD_snprintf(pstContents->pszLANMacAddr, NX_TEXT_SIZE_128, "%s (%s)", (HCHAR *)ONDK_GetString(RES_MAC_ADDRESS_01_ID), (HCHAR *)ONDK_GetString(pstItemStrID));
					ONDK_GWM_COM_Text_Create((NX_MENU_SYSINFO_ID | pstContents->stItemList.aulSysItemList[i]), &stItemRect, (HCHAR *)pstContents->pszLANMacAddr);
				}
				break;
			case eNxMenuSysInfo_WLANMacAddr_Label:
				pstContents->pszWLANMacAddr = (HCHAR *)NX_APP_Malloc(sizeof(HCHAR) * NX_TEXT_SIZE_128);
				if (pstContents->pszWLANMacAddr)
				{
					HxSTD_snprintf(pstContents->pszWLANMacAddr, NX_TEXT_SIZE_128, "%s (%s)", (HCHAR *)ONDK_GetString(RES_MAC_ADDRESS_01_ID), (HCHAR *)ONDK_GetString(pstItemStrID));
					ONDK_GWM_COM_Text_Create((NX_MENU_SYSINFO_ID | pstContents->stItemList.aulSysItemList[i]), &stItemRect, (HCHAR *)pstContents->pszWLANMacAddr);
				}
				break;
			case eNxMenuSysInfo_BluetoothMacAddr_Label:
				pstContents->pszBluetoothMacAddr = (HCHAR *)NX_APP_Malloc(sizeof(HCHAR) * NX_TEXT_SIZE_128);
				if (pstContents->pszBluetoothMacAddr)
				{
					HxSTD_snprintf(pstContents->pszBluetoothMacAddr, NX_TEXT_SIZE_128, "%s (%s)", (HCHAR *)ONDK_GetString(RES_MAC_ADDRESS_01_ID), (HCHAR *)ONDK_GetString(pstItemStrID));
					ONDK_GWM_COM_Text_Create((NX_MENU_SYSINFO_ID | pstContents->stItemList.aulSysItemList[i]), &stItemRect, (HCHAR *)pstContents->pszBluetoothMacAddr);
				}
				break;
			default:
				ONDK_GWM_COM_Text_Create((NX_MENU_SYSINFO_ID | pstContents->stItemList.aulSysItemList[i]), &stItemRect, (HCHAR *)ONDK_GetString(pstItemStrID));
				break;
		}
		ONDK_GWM_COM_Text_SetFont((NX_MENU_SYSINFO_ID | pstContents->stItemList.aulSysItemList[i]), eFont_SystemNormal);
		ONDK_GWM_COM_Text_SetSize((NX_MENU_SYSINFO_ID | pstContents->stItemList.aulSysItemList[i]), NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_COM_Text_SetAlign((NX_MENU_SYSINFO_ID | pstContents->stItemList.aulSysItemList[i]), TEXT_ALIGN_LEFT);
		ONDK_GWM_COM_Text_ChangeColor((NX_MENU_SYSINFO_ID | pstContents->stItemList.aulSysItemList[i]), COL(C_T_Gong03), COL(C_T_Bang_100));

		ONDK_GWM_COM_Text_Create((NX_MENU_SYSINFO_ID | pstContents->stItemList.aulSysItemValueList[i]), &stValueRect, (HCHAR *)pstContents->szItemTextList[i]);
		ONDK_GWM_COM_Text_SetFont((NX_MENU_SYSINFO_ID | pstContents->stItemList.aulSysItemValueList[i]), eFont_SystemBold);
		ONDK_GWM_COM_Text_SetSize((NX_MENU_SYSINFO_ID | pstContents->stItemList.aulSysItemValueList[i]), NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_COM_Text_SetAlign((NX_MENU_SYSINFO_ID | pstContents->stItemList.aulSysItemValueList[i]), TEXT_ALIGN_RIGHT);
		ONDK_GWM_COM_Text_ChangeColor((NX_MENU_SYSINFO_ID | pstContents->stItemList.aulSysItemValueList[i]), COL(C_T_Whistle_100), COL(C_T_Bang_100));

		stItemRect.y += NX_MENU_SYSINFO_ITEM_LABEL_LS;
		stValueRect.y += NX_MENU_SYSINFO_ITEM_VALUE_LS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuSysInfo_DrawInfoBox(Nx_MenuSysInfoContent_t *pstContents)
{
	HINT32			lh = 0;
	ONDK_Rect_t     stRect = { 0, 0, 0, 0 };

	if (pstContents->stItemList.ulSysItemNum > 0)
	{
		lh = (pstContents->stItemList.ulSysItemNum * NX_MENU_SYSINFO_ITEM_LABEL_LS) + NX_MENU_SYSINFO_ITEM_LABEL_LS;
	}
	else
	{
		lh = NX_MENU_SYSINFO_INFOBOX_H;
	}

	stRect = ONDK_Rect(NX_MENU_SYSINFO_INFOBOX_X, NX_MENU_SYSINFO_INFOBOX_Y, NX_MENU_SYSINFO_INFOBOX_W, lh);
	ONDK_GWM_COM_Rect_Create((NX_MENU_SYSINFO_ID | eNxMenuSysInfo_InfoBox), &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod((NX_MENU_SYSINFO_ID | eNxMenuSysInfo_InfoBox), NX_PRISM_COMP_DrawFrameBox_A);

	return  ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSysInfo_DrawDefaultUI(Nx_MenuSysInfoContent_t *pstContents)
{
	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuSysInfo_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	NX_PRISM_COMP_Settings_DrawTitleLine();

	nx_menuSysInfo_DrawInfoBox(pstContents);
	nx_menuSysInfo_DrawSystemInfo(pstContents);
	nx_menuSysInfo_DrawCheckNewSWButton();

	// swupdate
	nx_menuSysInfo_AddItemSwupdateLoading(pstContents);

	ONDK_GWM_Obj_SetFocus(NX_MENU_SYSINFO_ID | eNxMenuSysInfo_CheckNewSWButton);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSysInfo_LoadSysInfoItemsValue(Nx_MenuSysInfoContent_t *pstContents)
{
	HUINT8			**ppListOfItem = NULL;
	HUINT8			szTextBuffer[NX_TEXT_SIZE_128];
	HUINT32			i = 0;

	ppListOfItem = NX_PRISM_MENU_MenuItem_AllocTextList (pstContents->stItemList.ulSysItemNum, NX_TEXT_SIZE_128);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("ERROR!!! NX_PRISM_MENU_MenuItem_AllocTextList() Fail !!\n");
		return	ONDK_RET_FAIL;
	}
	else
	{
		for (i = 0; i < pstContents->stItemList.ulSysItemNum; i++)
		{
			HxSTD_memset(ppListOfItem[i], 0x00, NX_TEXT_SIZE_128);
			ppListOfItem[i][0] = '\0';

			switch (pstContents->stItemList.aulSysItemList[i])
			{
				case eNxMenuSysInfo_ModelName_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					NX_SYSTEM_GetModelName((HCHAR*)szTextBuffer, NX_TEXT_SIZE_128);
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
				case eNxMenuSysInfo_SoftwareVersion_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					NX_SYSTEM_GetSoftwareVersion((HCHAR*)szTextBuffer, MAX_ELEMENT_TEXT_LENGTH);
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
				case eNxMenuSysInfo_LoaderVersion_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					NX_SYSTEM_GetLoaderVersion((HCHAR*)szTextBuffer, MAX_ELEMENT_TEXT_LENGTH);
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
				case eNxMenuSysInfo_CIPlusVersion_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					NX_SYSTEM_GetCIPlusVersion((HCHAR*)szTextBuffer);
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
				case eNxMenuSysInfo_SystemId_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					NX_SYSTEM_GetSystemId((HCHAR*)szTextBuffer, NX_TEXT_SIZE_128);
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
				case eNxMenuSysInfo_UpdateDate_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					NX_SYSTEM_GetUpdateDate((HCHAR*)szTextBuffer, NX_TEXT_SIZE_128);
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
				case eNxMenuSysInfo_MicomVersion_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					NX_SYSTEM_GetMicomVersion((HCHAR*)szTextBuffer, NX_TEXT_SIZE_128);
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
#if defined(CONFIG_MW_INET)
				case eNxMenuSysInfo_LANMacAddr_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					NX_NETWORK_GetLANMacAddr((HCHAR*)szTextBuffer, NX_TEXT_SIZE_128);
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
#endif
#if defined(CONFIG_MW_INET_WLAN)
				case eNxMenuSysInfo_WLANMacAddr_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					NX_NETWORK_GetWLANMacAddr((HCHAR*)szTextBuffer, NX_TEXT_SIZE_128);
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
#endif
#if defined(CONFIG_MW_BLUETOOTH)
				case eNxMenuSysInfo_BluetoothMacAddr_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					NX_NETWORK_GetBluetoothMacAddr((HCHAR*)szTextBuffer);
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
#endif
				default:
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)ONDK_GetString(RES_UNKNOWN_ID));
					break;
			}
		}

		pstContents->szItemTextList = ppListOfItem;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSysInfo_AddItemList(NxUiSysInfoItemList_t *pstSysItemList, HUINT32 ulItemId, HUINT32 ulItemValueId)
{
	HUINT32		ulIndex = 0;

	if (pstSysItemList->ulSysItemNum >= NX_MENU_SYSINFO_ITEM_MAX_NUM)
	{
		NX_APP_Error("Error!!! OUT OF RANGE!! pstSysItemList->ulSysItemNum(%d), NX_UTIL_OPTION_MAX_NUM(%d)\n", pstSysItemList->ulSysItemNum, NX_MENU_SYSINFO_ITEM_MAX_NUM);
		return	ONDK_RET_FAIL;
	}

	ulIndex = pstSysItemList->ulSysItemNum;
	pstSysItemList->aulSysItemList[ulIndex] = ulItemId;
	pstSysItemList->aulSysItemValueList[ulIndex] = ulItemValueId;
	pstSysItemList->ulSysItemNum = (ulIndex + 1);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSysInfo_InitSysInfoItems(Nx_MenuSysInfoContent_t *pstContents)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL)\n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

	// Hidden Key Reset
	NX_PRISM_MENU_HiddenManagement_ResetHiddenKeySeq();

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_SYSTEM_INFORMATION_ID), MAX_ELEMENT_TEXT_LENGTH);
	pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Unknown;

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		default:
			nx_menuSysInfo_AddItemList(&pstContents->stItemList, eNxMenuSysInfo_ModelName_Label, eNxMenuSysInfo_ModelName_Value);
			nx_menuSysInfo_AddItemList(&pstContents->stItemList, eNxMenuSysInfo_SoftwareVersion_Label, eNxMenuSysInfo_SoftwareVersion_Value);
			nx_menuSysInfo_AddItemList(&pstContents->stItemList, eNxMenuSysInfo_LoaderVersion_Label, eNxMenuSysInfo_LoaderVersion_Value);
#if defined(CONFIG_MW_CI_PLUS)
			nx_menuSysInfo_AddItemList(&pstContents->stItemList, eNxMenuSysInfo_CIPlusVersion_Label, eNxMenuSysInfo_CIPlusVersion_Value);
#endif
			nx_menuSysInfo_AddItemList(&pstContents->stItemList, eNxMenuSysInfo_SystemId_Label, eNxMenuSysInfo_SystemId_Value);
			nx_menuSysInfo_AddItemList(&pstContents->stItemList, eNxMenuSysInfo_UpdateDate_Label, eNxMenuSysInfo_UpdateDate_Value);
#if defined(CONFIG_APUI_SET_USED_MICOM)
			nx_menuSysInfo_AddItemList(&pstContents->stItemList, eNxMenuSysInfo_MicomVersion_Label, eNxMenuSysInfo_MicomVersion_Value);
#endif
#if defined(CONFIG_MW_INET)
			nx_menuSysInfo_AddItemList(&pstContents->stItemList, eNxMenuSysInfo_LANMacAddr_Label, eNxMenuSysInfo_LANMacAddr_Value);
#endif
#if defined(CONFIG_MW_INET_WLAN)
			nx_menuSysInfo_AddItemList(&pstContents->stItemList, eNxMenuSysInfo_WLANMacAddr_Label, eNxMenuSysInfo_WLANMacAddr_Value);
#endif
#if defined(CONFIG_MW_BLUETOOTH)
			nx_menuSysInfo_AddItemList(&pstContents->stItemList, eNxMenuSysInfo_BluetoothMacAddr_Label, eNxMenuSysInfo_BluetoothMacAddr_Value);
#endif
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSysInfo_MsgGwmCreate(Nx_MenuSysInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	HUINT8		*pszPanelStr = NULL;


	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Init Menu Item
	eResult = nx_menuSysInfo_InitSysInfoItems(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		NX_APP_Error("Error!!! nx_menuSysInfo_InitSysInfoItems() Fail\n");
		return	eResult;
	}
	pstContents->lPrevCurIdx = p3;

	// Load Menu Item Value
	eResult = nx_menuSysInfo_LoadSysInfoItemsValue(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		NX_APP_Error("Error!!! nx_menuSysInfo_LoadSysInfoItemsValue() Fail\n");
		return	eResult;
	}

	// Draw Menu Item
	nx_menuSysInfo_DrawDefaultUI(pstContents);

	ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
	ONDK_GWM_SetTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID, NX_SETTING_TIME_UPDATE_TIME);

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Settings);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Settings !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSysInfo_MsgGwmDestroy(Nx_MenuSysInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);

	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	if (pstContents->szItemTextList)
	{
		NX_PRISM_MENU_MenuItem_FreeTextList(pstContents->stItemList.ulSysItemNum, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}

	if (pstContents->pszLANMacAddr)
	{
		NX_APP_Free(pstContents->pszLANMacAddr);
		pstContents->pszLANMacAddr = NULL;
	}
	if (pstContents->pszWLANMacAddr)
	{
		NX_APP_Free(pstContents->pszWLANMacAddr);
		pstContents->pszWLANMacAddr = NULL;
	}
	if (pstContents->pszBluetoothMacAddr)
	{
		NX_APP_Free(pstContents->pszBluetoothMacAddr);
		pstContents->pszBluetoothMacAddr = NULL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuSysInfoContent_t));
	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSysInfo_MsgGwmKeyDown(Nx_MenuSysInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL			bHiddenMenu = FALSE;
	HINT32			lKeyId = p1, lPrevCurIdx = 0, lMenuHiddenType = (NxUiMenuHiddenType_t)eNxUiMenuHiddenType_None;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	NxUiMenuHiddenType_t	eMenuHiddenType = eNxUiMenuHiddenType_None;


	if (pstContents == NULL)
	{
		NX_APP_Error("(pstContents == NULL)\n");
		return	GWM_RET_SOURCE_NULL;
	}

	// Rest timer
	eResult = ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
	if (eResult == GWM_RET_LINK_CONFLICT)	// Alreay added Timer
	{
		ONDK_GWM_ResetTimer(NX_SETTING_TIMEOUT_ID);
	}

#if defined(CONFIG_APUI_SET_HIDDEN_INFO)
	// Check Hidden key
	bHiddenMenu = NX_PRISM_MENU_HiddenManagement_CheckHiddenKeySeq(lKeyId, &lMenuHiddenType);
	eMenuHiddenType = (NxUiMenuHiddenType_t)lMenuHiddenType;
	if ((bHiddenMenu == TRUE) && (eMenuHiddenType != eNxUiMenuHiddenType_None))
	{
		ONDK_GWM_APP_Destroy(0);
		ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_HiddenManagement_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
								(ONDK_GWM_Callback_t)NX_PRISM_MENU_HiddenManagement_Proc, 0, 0, eMenuHiddenType, lPrevCurIdx);
		return	ONDK_MESSAGE_BREAK;
	}
#endif

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch (lKeyId)
	{
		/* keys to exit */
		case KEY_BACK:
	        lPrevCurIdx = pstContents->lPrevCurIdx;
			ONDK_GWM_APP_Destroy(0);
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_System_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
	                (ONDK_GWM_Callback_t)NX_PRISM_MENU_System_Proc, 0, 0, 0, lPrevCurIdx);
			return	ONDK_MESSAGE_BREAK;

		case KEY_EXIT:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

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
		case KEY_TVRADIO:
		case KEY_VFORMAT:
		case KEY_AUDIO:
		case KEY_SUBTITLE:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		case KEY_OK:
			break;

		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuSysInfo_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSysInfo_MsgGwmClicked(Nx_MenuSysInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch ((HUINT32)p1)
	{
	case (NX_MENU_SYSINFO_ID | eNxMenuSysInfo_CheckNewSWButton):
		if (pstContents->eSwUpdateCurStep != eDxSWUPDATE_STEP_Detect)
		{
			HxLOG_Print("Start S/W Update Detect!\n");
			nx_menuSysInfo_StartDetect(pstContents);
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

static ONDK_Result_t	nx_menuSysInfo_MsgGwmTimer(Nx_MenuSysInfoContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuSysInfo_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}
	else if (p1 == NX_MENU_SYSINFO_SWUPDATE_LODING_TIMER_ID)
	{
		nx_menuSysInfo_UpdateLoadingAnimation();
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


#define ____________SOFTWARE_UPDATE___________________

static ONDK_Result_t		nx_menuSysInfo_EndLoadingAnimation(void)
{
	ONDK_GWM_KillTimer(NX_MENU_SYSINFO_SWUPDATE_LODING_TIMER_ID);
	ONDK_GWM_Obj_SetVisible(NX_MENU_SYSINFO_ID | eNxMenuSysInfo_SwUpdateLoadingBg, FALSE);
	ONDK_GWM_Obj_SetVisible(NX_MENU_SYSINFO_ID | eNxMenuSysInfo_SwUpdateLodingImg, FALSE);
	ONDK_GWM_APP_InvalidateObject(NX_MENU_SYSINFO_ID | eNxMenuSysInfo_SwUpdateLoadingBg);
	ONDK_GWM_APP_InvalidateObject(NX_MENU_SYSINFO_ID | eNxMenuSysInfo_SwUpdateLodingImg);

	ONDK_GWM_Obj_SetFocus(NX_MENU_SYSINFO_ID | eNxMenuSysInfo_CheckNewSWButton);

	return ONDK_RET_OK;
}

static ONDK_Result_t		nx_menuSysInfo_UpdateLoadingAnimation(void)
{
	ONDK_GWM_APP_InvalidateObject(NX_MENU_SYSINFO_ID | eNxMenuSysInfo_SwUpdateLodingImg);

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSysInfo_CbLoadingDrawAnimation(ONDKSurfaceHandle screen, ONDK_Region_t *area,
												ONDK_Rect_t *rect, ONDK_GWM_GuiObject_t object, void *arg, int bUiTop)
{
	HCHAR		*stStockImgID = NULL;
	ONDK_Rect_t     stRect = { 0, 0, 0, 0 };
	Nx_MenuSysInfoContent_t	*pstContents = NULL;

	pstContents = nx_menuSysInfo_GetContents();

	switch(pstContents->nLoadingImgIndex)
	{
	case 0:
		stStockImgID = RES_206_00_LOADING_01_PNG;			pstContents->nLoadingImgIndex = 1;
		break;
	case 1:
		stStockImgID = RES_206_00_LOADING_02_PNG;			pstContents->nLoadingImgIndex = 2;
		break;
	case 2:
		stStockImgID = RES_206_00_LOADING_03_PNG;			pstContents->nLoadingImgIndex = 3;
		break;
	case 3:
		stStockImgID = RES_206_00_LOADING_04_PNG;			pstContents->nLoadingImgIndex = 0;
		break;
	default:
		break;
	}
	stRect = ONDK_Rect(594, 20, 46, 46);
	ONDK_GUICOM_IMG_Blit(screen, stRect, (HCHAR*)stStockImgID);
	NX_COMMON_NOT_USED_ARGUMENT(bUiTop, arg, object, rect);
	NX_COMMON_NOT_USED_PARAM(area);

	return ONDK_RET_OK;
}

static ONDK_Result_t 	nx_menuSysInfo_AddItemSwupdateLoading(Nx_MenuSysInfoContent_t *pstContents)
{
	ONDK_Rect_t     stRect = { 0, 0, 0, 0 };

	stRect = ONDK_Rect(440, 0, 400, 100);
	ONDK_GWM_COM_Rect_Create(NX_MENU_SYSINFO_ID | eNxMenuSysInfo_SwUpdateLoadingBg, &stRect, COL(C_G_Whistle_10));

	ONDK_GWM_Obj_SetVisible(NX_MENU_SYSINFO_ID | eNxMenuSysInfo_SwUpdateLoadingBg, FALSE);

	stRect = ONDK_Rect(594, 20, 46, 46);
	ONDK_GWM_COM_Image_Create(NX_MENU_SYSINFO_ID | eNxMenuSysInfo_SwUpdateLodingImg, &stRect);
	ONDK_GWM_APP_SetObjectDrawMethod(NX_MENU_SYSINFO_ID | eNxMenuSysInfo_SwUpdateLodingImg, nx_menuSysInfo_CbLoadingDrawAnimation);
	ONDK_GWM_Obj_SetVisible(NX_MENU_SYSINFO_ID | eNxMenuSysInfo_SwUpdateLodingImg, FALSE);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return ONDK_RET_OK;

}

static ONDK_Result_t	nx_menuSysInfo_RecvSwUpEvent(Nx_MenuSysInfoContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;

	switch(p1)
	{
	case eDxSWUPDATE_EVENT_DetectSuccess:
		nx_menuSysInfo_EndLoadingAnimation();
		{
			DxSwUpdate_EventData_t *pstDetectedInform = NX_CORE_SWUP_GetDetectedInform(NX_CORE_SWUP_GetSwUpSpec());

			if (pstDetectedInform != NULL)
			{
				eResult = ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
				if (eResult == GWM_RET_LINK_CONFLICT)	// Alreay added Timer
				{
					ONDK_GWM_ResetTimer(NX_SETTING_TIMEOUT_ID);
				}
				ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_MENU_SwUpDownload_Proc", (APP_DEFAULT_PRIORITY | UISTATE_NORMAL),
								NX_PRISM_MENU_SwUpInfo_Proc, 0, 0, pstDetectedInform->eSourceType, eNxSetting_SystemInfomation);
			}
		}
		break;

	case eDxSWUPDATE_EVENT_DetectFail:
		HxLOG_Error("Recv S/W Up Msg (eDxSWUPDATE_EVENT_DetectFail)\n");
		nx_menuSysInfo_EndLoadingAnimation();
		pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Unknown;
		break;

	case eDxSWUPDATE_EVENT_Erorr:
		HxLOG_Error("Recv S/W Up Msg (eDxSWUPDATE_EVENT_Erorr)\n");
		pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Unknown;
		break;

	case eDxSWUPDATE_EVENT_Finish:
		HxLOG_Error("Recv S/W Up Msg (eDxSWUPDATE_EVENT_Finish)\n");
		break;

	default:
		break;
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);
	NX_COMMON_NOT_USED_PARAM(handle);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_menuSysInfo_StartDetect(Nx_MenuSysInfoContent_t *pstContents)
{
	NX_CHANNEL_StopChannel();

	if (NX_CORE_SWUP_StartDetectWithType(NX_CORE_SWUP_GetSwUpSpec(), &pstContents->hSwupdateHandle) == ERR_FAIL)
	{
		HxLOG_Error("\n\nCan't Not Start S/W Update!!!\n\n");
		return ONDK_RET_FAIL;
	}
	pstContents->eSwUpdateCurStep = eDxSWUPDATE_STEP_Detect;

	pstContents->nLoadingImgIndex = 0;
	ONDK_GWM_Obj_SetVisible(NX_MENU_SYSINFO_ID | eNxMenuSysInfo_SwUpdateLoadingBg, TRUE);
	ONDK_GWM_Obj_SetVisible(NX_MENU_SYSINFO_ID | eNxMenuSysInfo_SwUpdateLodingImg, TRUE);

	{
		ONDK_Result_t	retTimer = ONDK_RET_FAIL;
		//if (pstContents->eLastSearchType != eGuideSearchType_Pf)
		retTimer = ONDK_GWM_SetTimer(NX_MENU_SYSINFO_SWUPDATE_LODING_TIMER_ID, NX_MENU_SYSINFO_SWUPDATE_LODING_TIMER_PERIOD);

		if (retTimer == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
		{
			ONDK_GWM_ResetTimer(NX_MENU_SYSINFO_SWUPDATE_LODING_TIMER_ID);
		}
	}

	return ONDK_RET_OK;
}

/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_SysInfo_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuSysInfoContent_t	*pstContents = NULL;

	pstContents = nx_menuSysInfo_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuSysInfo_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_menuSysInfo_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			eResult = nx_menuSysInfo_MsgGwmClicked(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuSysInfo_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuSysInfo_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_CORE_SWUP_NOTIFY:
			nx_menuSysInfo_RecvSwUpEvent(pstContents, (Handle_t)NULL, p1, p2, p3);
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


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

