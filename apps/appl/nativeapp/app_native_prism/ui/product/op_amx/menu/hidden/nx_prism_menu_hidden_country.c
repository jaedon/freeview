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
 * @file	  		nx_prism_menu_hidden_country.c
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
#include <nx_main.h>
#include <nx_core.h>
#include <nx_port.h>

#include <nx_common.h>
#include <nx_core_message.h>
#include <nx_prism_app.h>
#include <nx_prism_channellist.h>

#include "nx_prism_definition.h"


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_MENU_HIDDEN_COUNTRY_ITEM_ID						(eMenuFrame_Window_HiddenCountry)

#define	NX_PRISM_MENU_HIDDEN_COUNTRY_EXPIRE_TIMER_ID				(NX_PRISM_MENU_HIDDEN_COUNTRY_ITEM_ID + 1)
#define	NX_PRISM_MENU_HIDDEN_COUNTRY_EXPIRE_TIME					(30 * NX_COMMON_TIME_SECOND)

#define	NX_PRISM_MENU_HIDDEN_COUNTRY_SELECTLIST_MAX					8

#define	NX_PRISM_MENU_HIDDEN_COUNTRY_LIST_Y2						673


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef struct
{
	HxCountry_e		eCountryCode;

	// country Item
	HUINT32			ulNumOfItemTextList;
	HUINT8			**szItemTextList;
} NxUiHiddenCountryContents_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static HINT8		s_cMenuhiddenBackDoorKeySeq = 0;
static HINT8		s_cMenuhiddenTechnicalMenuKeySeq = 0;
static NxUiHiddenCountryContents_t		s_stUiHiddenCountryContents;
static Nx_PrismListDialog_Content_t		s_stHiddenCountryDialogContents;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_hiddenCountry_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_hiddenCountry_DestroyListPopup(void);

/* Channel List Option - Change Group */
static void				nx_hiddenCountry_CreateChangeCountryListPopup(NxUiHiddenCountryContents_t *pstContents);

/* Channel List Option */
static inline NxUiHiddenCountryContents_t* nx_hiddenCountry_GetContents(void);
static ONDK_Result_t	nx_hiddenCountry_ResetExpireTimer(void);
static ONDK_Result_t	nx_hiddenCountry_ChangedItemValue(NxUiHiddenCountryContents_t *pstContents, HUINT32 ulCurIdx);
static ONDK_Result_t	nx_hiddenCountry_InitOptionData(NxUiHiddenCountryContents_t *pstContents);

static ONDK_Result_t	nx_hiddenCountry_MsgGwmCreate(NxUiHiddenCountryContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_hiddenCountry_MsgGwmDestroy(NxUiHiddenCountryContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_hiddenCountry_MsgGwmHapiMsg(NxUiHiddenCountryContents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_hiddenCountry_MsgGwmKeyDown(NxUiHiddenCountryContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_hiddenCountry_MsgGwmTimer(NxUiHiddenCountryContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_hiddenCountry_DisplayListPopup(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_ListDlg_ProtoProc(&s_stHiddenCountryDialogContents, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_hiddenCountry_DestroyListPopup(void)
{
	return ONDK_GWM_APP_Destroy(nx_hiddenCountry_DisplayListPopup);
}


/******************************************************************************/
#define __Option_ChangeGroup_Functions________________________________________
/******************************************************************************/

static void		nx_hiddenCountry_CreateChangeCountryListPopup(NxUiHiddenCountryContents_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	HINT32		lLineCnt = 0;
	HUINT32		ulTextListNum = 0, ulIndex = 0, ulAttribute = 0, ulMaxListNum = 0;
	HUINT8		**pptextList = NULL;
	ONDK_Rect_t		stRect = { 0, 0, 0, 0 }, stListRect = { 0, 0, 0, 0 };


	ulTextListNum = pstContents->ulNumOfItemTextList;
	pptextList = pstContents->szItemTextList;

	/* List의 Index를 얻어온다. */
	eResult = NX_PRISM_MENU_MenuItem_GetExpandIndexFromValue((Nx_MenuSubItemId_t)NX_MENUITEM_ID_SET_COUNTRYSETTING, (HINT32)(pstContents->eCountryCode), &ulIndex);
	if (eResult != ONDK_RET_OK)
	{
		ulIndex = 0;
	}

	lLineCnt = NX_PRISM_UTIL_GetTextLineCount(eFont_SystemNormal, NX_PRISM_FONT_SIZE_22, ONDK_GetString(RES_SELECTED_REGION_ID), NX_PRISM_UI_DLG_MULTISEL_MSG_TEXT_W);

	// Set Text List
	ulMaxListNum = (ulTextListNum > NX_PRISM_MENU_HIDDEN_COUNTRY_SELECTLIST_MAX) ? NX_PRISM_MENU_HIDDEN_COUNTRY_SELECTLIST_MAX : ulTextListNum;
	ulAttribute = (eListDlg_OSD_USER | eListDlg_OSD_BG | eListDlg_Title | eListDlg_SelectItemApp);
	stRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_List, lLineCnt, ulMaxListNum);
	if (ulAttribute & eListDlg_OSD_USER)
	{
		stListRect = NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(eNX_DLG_MultiSel_OSD, lLineCnt, ulMaxListNum);
		stRect.x = stListRect.x + stRect.x;
		stRect.y = stListRect.y + stRect.y;
	}

	NX_PRISM_COMP_ListDlg_SetListPopup(&s_stHiddenCountryDialogContents, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
							&stRect, (NX_PRISM_UI_DLG_MULTISEL_MSG_LIST_H + NX_PRISM_UI_DLG_MULTISEL_MSG_CELL_MG),
							ulAttribute, ulTextListNum, ulMaxListNum, ulIndex, (HUINT32)pptextList);

	NX_PRISM_COMP_ListDlg_SetFontSize(&s_stHiddenCountryDialogContents, NX_PRISM_FONT_SIZE_22);
	NX_PRISM_COMP_ListDlg_SetTitle(&s_stHiddenCountryDialogContents, ONDK_GetString(RES_SELECTED_REGION_ID));

	if (ulAttribute & eListDlg_OSD_USER)
	{
		NX_PRISM_COMP_ListDlg_SetExternalArea(&s_stHiddenCountryDialogContents, &stListRect);
	}

	/* Create Pop Up */
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_hiddenCountry_DisplayGroupListPopup", APP_DEFAULT_POPUP_PRIORITY, nx_hiddenCountry_DisplayListPopup, 0, 0, 0, 0);
}


/******************************************************************************/
#define __Channel_OPT_list_Functions______________________________________________
/******************************************************************************/

static inline NxUiHiddenCountryContents_t* nx_hiddenCountry_GetContents(void)
{
	return	&s_stUiHiddenCountryContents;
}


static ONDK_Result_t	nx_hiddenCountry_ResetExpireTimer(void)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	// set timer
	eResult = ONDK_GWM_SetTimer(NX_PRISM_MENU_HIDDEN_COUNTRY_EXPIRE_TIMER_ID, NX_PRISM_MENU_HIDDEN_COUNTRY_EXPIRE_TIME);
	if (eResult == GWM_RET_LINK_CONFLICT)	// 이미 등록되어 종료되지 않은거라면..
	{
		ONDK_GWM_ResetTimer(NX_PRISM_MENU_HIDDEN_COUNTRY_EXPIRE_TIMER_ID);
	}

	return eResult;
}


static ONDK_Result_t	nx_hiddenCountry_ChangedItemValue(NxUiHiddenCountryContents_t *pstContents, HUINT32 ulCurIdx)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	HERROR		hError = ERR_FAIL;
	HINT32		lSubItemValue = 0;
	HINT32		lHour = 0, lGmtOffset = 0;
	HUINT32 	ulMin = 0;
	HxCountry_e		eCountryCode = eCountryID_NONE;


	eResult = NX_PRISM_MENU_MenuItem_GetExpandValueFromIndex(NX_MENUITEM_ID_SET_COUNTRYSETTING, ulCurIdx, &lSubItemValue);
	if (eResult != ONDK_RET_OK)
	{
		lSubItemValue = (HINT32)pstContents->eCountryCode;
		NX_APP_Error("Error!!! NX_PRISM_MENU_MenuItem_GetValueFromIndex() - NX_MENUITEM_ID_SET_COUNTRYSETTING, ulCurIdx :(%d) Fail!! \n", ulCurIdx);
	}

	if (pstContents->eCountryCode != (HxCountry_e)lSubItemValue)
	{
		pstContents->eCountryCode = (HxCountry_e)lSubItemValue;
		hError = NX_SYSTEM_SetCountryCode(pstContents->eCountryCode);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_SYSTEM_SetCountryCode() Fail!! \n");
		}

		// Other country GWT offset Time is like eCountryID_GTM, it is OP Spec.
		if (pstContents->eCountryCode == eCountryID_OTHER)
		{
			eCountryCode = eCountryID_GTM;
		}
		else
		{
			eCountryCode = pstContents->eCountryCode;
		}

		hError = HLIB_DATETIME_GetTimeOffsetByCountryIndex(eCountryCode, &lHour, &ulMin);
		if (hError == ERR_OK)
		{
			lGmtOffset = (lHour * NX_COMMON_TIME_MINS_PER_HOUR) + (HINT32)ulMin;
			hError = NX_SYSTEM_SetGMTOffset(lGmtOffset);
			if (hError != ERR_OK)
			{
				NX_APP_Error("Error!!! NX_SYSTEM_SetCountryCode() Fail!! \n");
			}
		}
		else
		{
			NX_APP_Error("Error!!! HLIB_DATETIME_GetTimeOffsetByCountryIndex() - eCountryCode :(0x%x) Fail!! \n", eCountryCode);
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_hiddenCountry_InitOptionData(NxUiHiddenCountryContents_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	HERROR			hError = ERR_FAIL;
	HUINT8			**ppListOfItem = NULL;
	HUINT32			ulItems = 0;
	HxCountry_e		eCountryCode = eCountryID_NONE;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents was NULL !! \n");
		return ONDK_RET_INITFAIL;
	}

	if (pstContents->szItemTextList)
	{
		NX_PRISM_MENU_MenuItem_FreeTextList(pstContents->ulNumOfItemTextList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
		pstContents->ulNumOfItemTextList = 0;
	}

	hError = NX_SYSTEM_GetCountryCode(&eCountryCode);
	if (hError != ERR_OK)
	{
		eCountryCode = eCountryID_NONE;
		NX_APP_Error("Error!!! NX_SYSTEM_GetCountryCode() Faile! \n");
	}

	pstContents->eCountryCode = eCountryCode;

	/* Count SubItems */
	eResult = NX_PRISM_MENU_Menuitem_CountExpandSubitems((Nx_MenuSubItemId_t)NX_MENUITEM_ID_SET_COUNTRYSETTING, &ulItems);
	if ((eResult != ONDK_RET_OK) || (ulItems == 0))
	{
		NX_APP_Error("Error!! NX_PRISM_MENU_Menuitem_CountExpandSubitems() - NX_MENUITEM_ID_SET_COUNTRYSETTING eResult(0x%x), ulItems:(%u)\n", eResult, ulItems);
		return	eResult;
	}

	/* List를 만든다. */
	ppListOfItem = NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId((Nx_MenuSubItemId_t)NX_MENUITEM_ID_SET_COUNTRYSETTING, TRUE);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_MENU_MenuItem_MakeExpandTextListFromMenuItemId() - NX_MENUITEM_ID_SET_COUNTRYSETTING\n");
		return	ONDK_RET_FAIL;
	}

	pstContents->ulNumOfItemTextList = ulItems;
	pstContents->szItemTextList = ppListOfItem;

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_hiddenCountry_MsgGwmCreate(NxUiHiddenCountryContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_UI_SET_WINDOW_X, NX_PRISM_UI_SET_WINDOW_Y,
								NX_PRISM_UI_SET_WINDOW_W, NX_PRISM_UI_SET_WINDOW_H);

	HxSTD_MemSet(pstContents, 0x00, sizeof(NxUiHiddenCountryContents_t));

	// Init Item Data
	eResult = nx_hiddenCountry_InitOptionData(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_FAIL;
	}

	nx_hiddenCountry_CreateChangeCountryListPopup(pstContents);
	nx_hiddenCountry_ResetExpireTimer();

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_hiddenCountry_MsgGwmDestroy(NxUiHiddenCountryContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	nx_hiddenCountry_DestroyListPopup();

	if (pstContents->szItemTextList)
	{
		NX_PRISM_MENU_MenuItem_FreeTextList(pstContents->ulNumOfItemTextList, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
		pstContents->ulNumOfItemTextList = 0;
	}

	ONDK_GWM_KillTimer(NX_PRISM_MENU_HIDDEN_COUNTRY_EXPIRE_TIMER_ID);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_hiddenCountry_MsgGwmHapiMsg(NxUiHiddenCountryContents_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_hiddenCountry_MsgGwmKeyDown(NxUiHiddenCountryContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lKeyId = p1;


	nx_hiddenCountry_ResetExpireTimer();
	switch (lKeyId)
	{
		case KEY_OPT:
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_BREAK;

		case KEY_STANDBY:
		case KEY_MENU:
		case KEY_GUIDE:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		case KEY_VFORMAT:
		case KEY_SUBTITLE:
		case KEY_AUDIO:
		case KEY_TEXT:
			NX_PRISM_NotAvailable_Show();
			return	ONDK_MESSAGE_BREAK;

		default:
			return ONDK_MESSAGE_PASS;
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
}


static ONDK_Result_t	nx_hiddenCountry_MsgGwmTimer(NxUiHiddenCountryContents_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_PRISM_MENU_HIDDEN_COUNTRY_EXPIRE_TIMER_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	return GWM_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/

ONDK_Result_t	NX_PRISM_MENU_HiddenCountry_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;
	NxUiHiddenCountryContents_t	*pstContents = NULL;

	pstContents = nx_hiddenCountry_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_hiddenCountry_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_hiddenCountry_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			eResult = nx_hiddenCountry_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_hiddenCountry_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER\n");
			eResult = nx_hiddenCountry_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_POPUP_LIST_DESTROYED:
			nx_hiddenCountry_DestroyListPopup();
			ONDK_GWM_APP_Destroy(0);
			break;

		case MSG_APP_POPUP_LIST_INPUTED_KEYDOWN:
			eResult = nx_hiddenCountry_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SETTING_VALUE_CHANGED:
			NX_APP_Message("MSG_APP_POPUP_LIST_DESTROYED & MSG_APP_SETTING_VALUE_CHANGED\n");
			if (lMessage == MSG_APP_SETTING_VALUE_CHANGED)
			{
				NX_APP_Message("MSG_APP_SETTING_VALUE_CHANGED\n");
				nx_hiddenCountry_DestroyListPopup();
				eResult = nx_hiddenCountry_ChangedItemValue(pstContents, p1);
			}
			break;

		case MSG_APP_HOMEMENU_UI_LAUNCH:
			NX_APP_Message("MSG_APP_HOMEMENU_UI_LAUNCH\n");
			if (p1 != eNX_UCT_Live)
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

	return ONDK_GWM_ProcessMessageDefault(lMessage, handle, p1, p2, p3);
}


ONDK_Result_t	NX_PRISM_MENU_HiddenTechnicalMenu(void)
{
	HINT32		lValue = 0, lUserAntCount = 0, i = 0, lAntIdx = NX_ANT_IDX_NULL;
	HERROR		hError = ERR_FAIL;
	NX_SAT_AntennaType_e	eAntennaType = eNxSat_AntType_LNB_Only;


	hError = NX_ANTENNA_GetAntennaType(&lValue);
	if (hError != ERR_OK)
	{
		eAntennaType = eNxSat_AntType_LNB_Only;
		NX_APP_Error("Error!! NX_ANTENNA_GetAntennaType()\n");
	}
	else
	{
		eAntennaType = (NX_SAT_AntennaType_e)lValue;
	}

	switch (eAntennaType)
	{
		case eNxSat_AntType_LNB_Only:
		case eNxSat_AntType_Monoblock_LNB:
		case eNxSat_AntType_DiSEqC_1_0:
		case eNxSat_AntType_DiSEqC_2_0:
		case eNxSat_AntType_Multiswitch:
			break;

		default:
			NX_APP_Error("Error!! Invalid Antenna Type : [%d]\n", eAntennaType);
			eAntennaType = eNxSat_AntType_LNB_Only;
			break;
	}

	lUserAntCount = NX_ANTENNA_GetUserAntCountUDB();
	NX_APP_Debug("NX_ANTENNA_GetUserAntCountUDB() [%d]\n", lUserAntCount);
	for (i = 0; i < lUserAntCount; i++)
	{
		lAntIdx = NX_ANT_IDX_NULL;
		hError = NX_ANTENNA_GetUserAntIndexUDB((eNxSat_ANTENAA_SLOT1 + i), &lAntIdx);
		if (hError == ERR_OK)
		{
			NX_APP_Debug("NX_ANTENNA_GetUserAntIndexUDB() found Antenna [%d]\n", (eNxSat_ANTENAA_SLOT1 + i));
			break;
		}
		else
		{
			NX_APP_Debug("NX_ANTENNA_GetUserAntIndexUDB() Not found Antenna [%d]\n", (eNxSat_ANTENAA_SLOT1 + i));
		}
	}

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Search_SatSatelliteSetting_Proc",
						(APP_DEFAULT_PRIORITY | UISTATE_MENU), NX_PRISM_MENU_Search_SatSatelliteSetting_Proc,
						HANDLE_NULL, eAntennaType, lAntIdx, TRUE);

	return	ONDK_RET_OK;
}


HBOOL	NX_PRISM_MENU_HiddenManagement_CheckHiddenBackDoorKeySeq(HINT32 lKey)
{
	switch(lKey)
	{
		case KEY_1:
			if (s_cMenuhiddenBackDoorKeySeq == 0)
			{
				s_cMenuhiddenBackDoorKeySeq++;
			}
			else
			{
				s_cMenuhiddenBackDoorKeySeq = 0;
			}
			break;

		case KEY_3:
			if (s_cMenuhiddenBackDoorKeySeq == 1)
			{
				s_cMenuhiddenBackDoorKeySeq++;
			}
			else
			{
				s_cMenuhiddenBackDoorKeySeq = 0;
			}
			break;

		case KEY_7:
			if (s_cMenuhiddenBackDoorKeySeq == 2)
			{
				s_cMenuhiddenBackDoorKeySeq++;
			}
			else
			{
				s_cMenuhiddenBackDoorKeySeq = 0;
			}
			break;

		case KEY_9:
			if (s_cMenuhiddenBackDoorKeySeq == 3)
			{
				s_cMenuhiddenBackDoorKeySeq = 0;
				return	TRUE;
			}
			else
			{
				s_cMenuhiddenBackDoorKeySeq = 0;
			}
			break;

		default:
			s_cMenuhiddenBackDoorKeySeq = 0;
			break;
	}

	return FALSE;
}


HBOOL	NX_PRISM_MENU_HiddenManagement_CheckHiddenTechnicalMenuKeySeq(HINT32 lKey)
{
	switch(lKey)
	{
		case KEY_1:
			if (s_cMenuhiddenTechnicalMenuKeySeq == 0)
			{
				s_cMenuhiddenTechnicalMenuKeySeq++;
			}
			else
			{
				s_cMenuhiddenTechnicalMenuKeySeq = 0;
			}
			break;

		case KEY_3:
			if (s_cMenuhiddenTechnicalMenuKeySeq == 1)
			{
				s_cMenuhiddenTechnicalMenuKeySeq++;
			}
			else
			{
				s_cMenuhiddenTechnicalMenuKeySeq = 0;
			}
			break;

		case KEY_5:
			if (s_cMenuhiddenTechnicalMenuKeySeq == 2)
			{
				s_cMenuhiddenTechnicalMenuKeySeq++;
			}
			else
			{
				s_cMenuhiddenTechnicalMenuKeySeq = 0;
			}
			break;

		case KEY_4:
			if (s_cMenuhiddenTechnicalMenuKeySeq == 3)
			{
				s_cMenuhiddenTechnicalMenuKeySeq = 0;
				return	TRUE;
			}
			else
			{
				s_cMenuhiddenTechnicalMenuKeySeq = 0;
			}
			break;

		default:
			s_cMenuhiddenTechnicalMenuKeySeq = 0;
			break;
	}

	return FALSE;
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

