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
 * @file	  		nx_prism_menu_smartcard_info_na.c
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
#include <nx_core_cas.h>
#include <nx_core_cas_na.h>

#include <nx_prism_app.h>
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>
#include <nx_prism_menu_items.h>
#include <nx_prism_util_na.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/

#define	NX_MENU_SMTCARDINFO_ID							eNxSetting_SystemInfomation

#define	NX_MENU_SMTCARDINFO_INFOBOX_X					60
#define	NX_MENU_SMTCARDINFO_INFOBOX_Y					114
#define	NX_MENU_SMTCARDINFO_INFOBOX_W					750
#define	NX_MENU_SMTCARDINFO_INFOBOX_H					418

#define	NX_MENU_SMTCARDINFO_ITEM_LABEL_X				(NX_MENU_SMTCARDINFO_INFOBOX_X + 18)
#define	NX_MENU_SMTCARDINFO_ITEM_LABEL_Y				(NX_MENU_SMTCARDINFO_INFOBOX_Y + 36 - NX_PRISM_FONT_SIZE_22)
#define	NX_MENU_SMTCARDINFO_ITEM_LABEL_W				450
#define	NX_MENU_SMTCARDINFO_ITEM_LABEL_LS				40

#define	NX_MENU_SMTCARDINFO_ITEM_VALUE_X				(NX_MENU_SMTCARDINFO_INFOBOX_X + 482)
#define	NX_MENU_SMTCARDINFO_ITEM_VALUE_Y				(NX_MENU_SMTCARDINFO_INFOBOX_Y + 36 - NX_PRISM_FONT_SIZE_22)
#define	NX_MENU_SMTCARDINFO_ITEM_VALUE_W				250
#define	NX_MENU_SMTCARDINFO_ITEM_VALUE_LS				40

#define	NX_MENU_SMTCARDINFO_ERRORMSG_TEXT_X				70
#define	NX_MENU_SMTCARDINFO_ERRORMSG_TEXT_W				740
#define	NX_MENU_SMTCARDINFO_ERRORMSG_TEXT_H				(NX_MENU_SMTCARDINFO_ITEM_LABEL_LS * 3)

#define	NX_MENU_SMTCARDINFO_ITEM_MAX_NUM				12

#define	NX_MENU_SMTCARDINFO_PARSE_ITEM_MAX				32

#define	NX_MENU_SMTCARDINFO_UPDATE_TIMER_ID				100
#define NX_MENU_SMTCARDINFO_UPDATE_TIME					(NX_COMMON_TIME_DEFAULT*NX_COMMON_TIME_SECOND)	/* 5 Second */


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef	enum
{
	eNxMenuSmtCardInfo_InfoBox,
	eNxMenuSmtCardInfo_ProjName_Label,
	eNxMenuSmtCardInfo_SmtCardVer_Label,
	eNxMenuSmtCardInfo_ChipsetNUID_Label,
	eNxMenuSmtCardInfo_CAVersion_Label,
	eNxMenuSmtCardInfo_SmtSerialNum_Label,
	eNxMenuSmtCardInfo_CASerialNum_Label,
#if defined(CONFIG_MW_CI)
	eNxMenuSmtCardInfo_HDPlusCardCreditValidUntil_Label,
#endif
	eNxMenuSmtCardInfo_ProjName_Value,
	eNxMenuSmtCardInfo_SmtCardVer_Value,
	eNxMenuSmtCardInfo_ChipsetNUID_Value,
	eNxMenuSmtCardInfo_CAVersion_Value,
	eNxMenuSmtCardInfo_SmtSerialNum_Value,
	eNxMenuSmtCardInfo_CASerialNum_Value,
#if defined(CONFIG_MW_CI)
	eNxMenuSmtCardInfo_HDPlusCardCreditValidUntil_Value,
#endif
	eNxMenuSmtCardInfo_ErrorMessage,
	eNxMenuSmtCardInfo_MAX
} Nx_MenuSmtCardInfoUiCompIds_e;


typedef struct tagSMTCARDINFO_LIST
{
	HUINT32		ulSmtCardItemNum;
	HUINT32		aulSmtCardItemList[NX_MENU_SMTCARDINFO_ITEM_MAX_NUM];
	HUINT32		aulSmtCardItemValueList[NX_MENU_SMTCARDINFO_ITEM_MAX_NUM];
} NxUiSmtCardInfoItemList_t;


typedef struct tag_NX_MENUSMTCARDINFONA_SET_CONTENT
{
	HUINT8	    ucTitle[MAX_ELEMENT_TEXT_LENGTH];
	HINT32      lPrevCurIdx;

	HUINT8		**szItemTextList;
	NxUiSmtCardInfoItemList_t	stItemList;

	HUINT8		szDate[NX_TEXT_SIZE_32];
	HUINT8		szTime[NX_TEXT_SIZE_32];

	HUINT32 	ulSessionHandleKey;
	NX_CasNaMessage_Type 	eCasMessageType;

} Nx_MenuSmtCardInfoNAContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/

static Nx_MenuSmtCardInfoNAContent_t		s_stSmtCardInfoContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/

static inline Nx_MenuSmtCardInfoNAContent_t* nx_menuSCInfo_na_GetContents(void);

static void				nx_menuSCInfo_na_UpdateCurrentTime(Nx_MenuSmtCardInfoNAContent_t *pstContents);

static ONDK_Result_t	nx_menuSCInfo_na_DrawSmartCardErrorMessage(Nx_MenuSmtCardInfoNAContent_t *pstContents, HINT32 lh, HBOOL bReDraw);
static ONDK_Result_t	nx_menuSCInfo_na_ReDrawSmartCardInfo(Nx_MenuSmtCardInfoNAContent_t *pstContents);
static ONDK_Result_t	nx_menuSCInfo_na_DrawSmartCardInfo(Nx_MenuSmtCardInfoNAContent_t *pstContents);
static ONDK_Result_t	nx_menuSCInfo_na_DrawInfoBox(Nx_MenuSmtCardInfoNAContent_t *pstContents, HINT32 *plh);
static ONDK_Result_t	nx_menuSCInfo_na_DrawDefaultUI(Nx_MenuSmtCardInfoNAContent_t *pstContents);

static ONDK_Result_t	nx_menuSCInfo_na_ConvertMwCASIdToApCASId(HCHAR *pszItemIdStrId, Nx_MenuSmtCardInfoUiCompIds_e *eMenuItemId);
static ONDK_Result_t	nx_menuSCInfo_na_UpdateInfoItemsValue(Nx_MenuSmtCardInfoNAContent_t *pstContents, const HCHAR *pszItemIdStrId, const HCHAR *pszItemValue);
static ONDK_Result_t	nx_menuSCInfo_na_LoadInfoItemsValue(Nx_MenuSmtCardInfoNAContent_t *pstContents);
static ONDK_Result_t	nx_menuSCInfo_na_AddItemList(NxUiSmtCardInfoItemList_t *pstSysItemList, HUINT32 ulItemId, HUINT32 ulItemValueId);
static ONDK_Result_t	nx_menuSCInfo_na_InitSysInfoItems(Nx_MenuSmtCardInfoNAContent_t *pstContents);

static ONDK_Result_t	nx_menuSCInfo_na_ParseJsonString(Nx_MenuSmtCardInfoNAContent_t *pstContents, const HCHAR* pJsonString);
static ONDK_Result_t	nx_menuSCInfo_na_ClosedSessionMenuData(Nx_MenuSmtCardInfoNAContent_t *pstContents);
static ONDK_Result_t	nx_menuSCInfo_na_RequestSessionMenuData(Nx_MenuSmtCardInfoNAContent_t *pstContents);

static ONDK_Result_t	nx_menuSCInfo_na_MsgGwmCreate(Nx_MenuSmtCardInfoNAContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuSCInfo_na_MsgGwmDestroy(Nx_MenuSmtCardInfoNAContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSCInfo_na_MsgGwmKeyDown(Nx_MenuSmtCardInfoNAContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSCInfo_na_MsgGwmTimer(Nx_MenuSmtCardInfoNAContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSCInfo_na_MsgGwmApkMsg(Nx_MenuSmtCardInfoNAContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSCInfo_na_MsgGwmApkMsgCasUiTriggered(Nx_MenuSmtCardInfoNAContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

static inline Nx_MenuSmtCardInfoNAContent_t* nx_menuSCInfo_na_GetContents(void)
{
	return	&s_stSmtCardInfoContent;
}


static void		nx_menuSCInfo_na_UpdateCurrentTime(Nx_MenuSmtCardInfoNAContent_t *pstContents)
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


static ONDK_Result_t	nx_menuSCInfo_na_DrawSmartCardErrorMessage(Nx_MenuSmtCardInfoNAContent_t *pstContents, HINT32 lh, HBOOL bReDraw)
{
	HINT8			*pszStrId = NULL;
	HINT32			lObjId = -1, y = 0;
	ONDK_Rect_t 	stRect = {0, 0, 0, 0};

	lObjId = (NX_MENU_SMTCARDINFO_ID | eNxMenuSmtCardInfo_ErrorMessage);
	if (bReDraw == FALSE)
	{
		y = (NX_MENU_SMTCARDINFO_INFOBOX_Y + lh + NX_MENU_SMTCARDINFO_ITEM_LABEL_LS);
		stRect = ONDK_Rect(NX_MENU_SMTCARDINFO_ERRORMSG_TEXT_X, y, NX_MENU_SMTCARDINFO_ERRORMSG_TEXT_W, NX_MENU_SMTCARDINFO_ERRORMSG_TEXT_H);
		ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR*)" ");
		ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemNormal);
		ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_COM_Text_SetAlign(lObjId, (TEXT_MULTILINED));
		ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Yoyo01), COL(C_T_Bang_100));
	}
	else
	{
		pszStrId = NX_PRISM_UTIL_NA_GetResStringIdByErrorType(pstContents->eCasMessageType);
		if (pszStrId != NULL)
		{
			ONDK_GWM_COM_Text_SetText(lObjId, (HCHAR *)ONDK_GetString(pszStrId));
		}
		else
		{
			ONDK_GWM_COM_Text_SetText(lObjId, (HCHAR *)" ");
		}
		ONDK_GWM_APP_InvalidateObject(lObjId);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSCInfo_na_ReDrawSmartCardInfo(Nx_MenuSmtCardInfoNAContent_t *pstContents)
{
	HUINT32			i = 0;

	for (i = 0; i < pstContents->stItemList.ulSmtCardItemNum; i++)
	{
		ONDK_GWM_COM_Text_SetText((NX_MENU_SMTCARDINFO_ID | pstContents->stItemList.aulSmtCardItemValueList[i]), (HCHAR *)pstContents->szItemTextList[i]);
		ONDK_GWM_APP_InvalidateObject((NX_MENU_SMTCARDINFO_ID | pstContents->stItemList.aulSmtCardItemValueList[i]));
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSCInfo_na_DrawSmartCardInfo(Nx_MenuSmtCardInfoNAContent_t *pstContents)
{
	HINT8			*pstItemStrID = NULL;
	HUINT32			i = 0;
	ONDK_Rect_t     stItemRect = { 0, 0, 0, 0 }, stValueRect = { 0, 0, 0, 0 };

	stItemRect = ONDK_Rect(NX_MENU_SMTCARDINFO_ITEM_LABEL_X, NX_MENU_SMTCARDINFO_ITEM_LABEL_Y, NX_MENU_SMTCARDINFO_ITEM_LABEL_W, NX_MENU_SMTCARDINFO_ITEM_LABEL_LS);
	stValueRect = ONDK_Rect(NX_MENU_SMTCARDINFO_ITEM_VALUE_X, NX_MENU_SMTCARDINFO_ITEM_VALUE_Y, NX_MENU_SMTCARDINFO_ITEM_VALUE_W, NX_MENU_SMTCARDINFO_ITEM_VALUE_LS);

	for (i = 0; i < pstContents->stItemList.ulSmtCardItemNum; i++)
	{
		switch (pstContents->stItemList.aulSmtCardItemList[i])
		{
			case eNxMenuSmtCardInfo_ProjName_Label:			pstItemStrID = RES_PROJECT_NAME_ID;					break;
			case eNxMenuSmtCardInfo_SmtCardVer_Label:		pstItemStrID = RES_SMARTCARD_VERSION_ID;			break;
			case eNxMenuSmtCardInfo_ChipsetNUID_Label:		pstItemStrID = RES_CHIPSET_NUID_ID;					break;
			case eNxMenuSmtCardInfo_CAVersion_Label:		pstItemStrID = RES_CA_VERSION_ID;					break;
			case eNxMenuSmtCardInfo_SmtSerialNum_Label:		pstItemStrID = RES_SMARTCARD_SERIAL_NUMBER_01_ID;	break;
			case eNxMenuSmtCardInfo_CASerialNum_Label:		pstItemStrID = RES_CA_SERIAL_NUMBER_ID;				break;
#if defined(CONFIG_MW_CI)
			case eNxMenuSmtCardInfo_HDPlusCardCreditValidUntil_Label:	pstItemStrID = RES_MESG_2448_ID;		break;
#endif
			default:										pstItemStrID = RES_UNKNOWN_ID;						break;
		}
		ONDK_GWM_COM_Text_Create((NX_MENU_SMTCARDINFO_ID | pstContents->stItemList.aulSmtCardItemList[i]), &stItemRect, (HCHAR *)ONDK_GetString(pstItemStrID));

		ONDK_GWM_COM_Text_SetFont((NX_MENU_SMTCARDINFO_ID | pstContents->stItemList.aulSmtCardItemList[i]), eFont_SystemNormal);
		ONDK_GWM_COM_Text_SetSize((NX_MENU_SMTCARDINFO_ID | pstContents->stItemList.aulSmtCardItemList[i]), NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_COM_Text_SetAlign((NX_MENU_SMTCARDINFO_ID | pstContents->stItemList.aulSmtCardItemList[i]), TEXT_ALIGN_LEFT);
		ONDK_GWM_COM_Text_ChangeColor((NX_MENU_SMTCARDINFO_ID | pstContents->stItemList.aulSmtCardItemList[i]), COL(C_T_Gong03), COL(C_T_Bang_100));

		ONDK_GWM_COM_Text_Create((NX_MENU_SMTCARDINFO_ID | pstContents->stItemList.aulSmtCardItemValueList[i]), &stValueRect, (HCHAR *)pstContents->szItemTextList[i]);
		ONDK_GWM_COM_Text_SetFont((NX_MENU_SMTCARDINFO_ID | pstContents->stItemList.aulSmtCardItemValueList[i]), eFont_SystemBold);
		ONDK_GWM_COM_Text_SetSize((NX_MENU_SMTCARDINFO_ID | pstContents->stItemList.aulSmtCardItemValueList[i]), NX_PRISM_FONT_SIZE_22);
		ONDK_GWM_COM_Text_SetAlign((NX_MENU_SMTCARDINFO_ID | pstContents->stItemList.aulSmtCardItemValueList[i]), TEXT_ALIGN_RIGHT);
		ONDK_GWM_COM_Text_ChangeColor((NX_MENU_SMTCARDINFO_ID | pstContents->stItemList.aulSmtCardItemValueList[i]), COL(C_T_Whistle_100), COL(C_T_Bang_100));

		stItemRect.y += NX_MENU_SMTCARDINFO_ITEM_LABEL_LS;
		stValueRect.y += NX_MENU_SMTCARDINFO_ITEM_VALUE_LS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t    nx_menuSCInfo_na_DrawInfoBox(Nx_MenuSmtCardInfoNAContent_t *pstContents, HINT32 *plh)
{
	HINT32			lh = 0;
	ONDK_Rect_t     stRect = { 0, 0, 0, 0 };

	if (pstContents->stItemList.ulSmtCardItemNum > 0)
	{
		lh = (pstContents->stItemList.ulSmtCardItemNum * NX_MENU_SMTCARDINFO_ITEM_LABEL_LS) + NX_MENU_SMTCARDINFO_ITEM_LABEL_LS;
	}
	else
	{
		lh = NX_MENU_SMTCARDINFO_INFOBOX_H;
	}

	stRect = ONDK_Rect(NX_MENU_SMTCARDINFO_INFOBOX_X, NX_MENU_SMTCARDINFO_INFOBOX_Y, NX_MENU_SMTCARDINFO_INFOBOX_W, lh);
	ONDK_GWM_COM_Rect_Create((NX_MENU_SMTCARDINFO_ID | eNxMenuSmtCardInfo_InfoBox), &stRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod((NX_MENU_SMTCARDINFO_ID | eNxMenuSmtCardInfo_InfoBox), NX_PRISM_COMP_DrawFrameBox_A);

	*plh = lh;

	return  ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSCInfo_na_DrawDefaultUI(Nx_MenuSmtCardInfoNAContent_t *pstContents)
{
	HINT32		lh = 0;

	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	nx_menuSCInfo_na_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	NX_PRISM_COMP_Settings_DrawTitleLine();

	nx_menuSCInfo_na_DrawInfoBox(pstContents, &lh);
	nx_menuSCInfo_na_DrawSmartCardInfo(pstContents);
	nx_menuSCInfo_na_DrawSmartCardErrorMessage(pstContents, lh, FALSE);

	ONDK_GWM_Obj_SetFocus(NX_MENU_SMTCARDINFO_ID);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSCInfo_na_ConvertMwCASIdToApCASId(HCHAR *pszItemIdStrId, Nx_MenuSmtCardInfoUiCompIds_e *eMenuItemId)
{
	HINT32		lStrLen = 0;


	if (pszItemIdStrId == NULL)
	{
		NX_APP_Error("Error!!! (pszItemIdStrId == NULL) \n");
		*eMenuItemId = eNxMenuSmtCardInfo_MAX;
		return	ONDK_RET_FAIL;
	}

	lStrLen = HxSTD_StrLen(pszItemIdStrId);
	if (lStrLen < 0)
	{
		NX_APP_Error("Error!!! (lStrLen < 0) \n");
		*eMenuItemId = eNxMenuSmtCardInfo_MAX;
		return	ONDK_RET_FAIL;
	}

	NX_APP_Debug("[Info] pszItemIdStrId:(%s), lStrLen:(%d)\n", pszItemIdStrId, lStrLen);

	if (0 == HxSTD_StrNCmp((char*)NX_CAS_MENU_SMTCARDINFO_PROJECT_NAME_ID, (char*)pszItemIdStrId, lStrLen))
	{
		*eMenuItemId = eNxMenuSmtCardInfo_ProjName_Label;
	}
	else if (0 == HxSTD_StrNCmp((char*)NX_CAS_MENU_SMTCARDINFO_SMARTCARD_VERSION_ID, (char*)pszItemIdStrId, lStrLen))
	{
		*eMenuItemId = eNxMenuSmtCardInfo_SmtCardVer_Label;
	}
	else if (0 == HxSTD_StrNCmp((char*)NX_CAS_MENU_SMTCARDINFO_CA_VERSION_ID, (char*)pszItemIdStrId, lStrLen))
	{
		*eMenuItemId = eNxMenuSmtCardInfo_CAVersion_Label;
	}
	else if (0 == HxSTD_StrNCmp((char*)NX_CAS_MENU_SMTCARDINFO_SMARTCARD_SERIAL_NUMBER_ID, (char*)pszItemIdStrId, lStrLen))
	{
		*eMenuItemId = eNxMenuSmtCardInfo_SmtSerialNum_Label;
	}
	else if (0 == HxSTD_StrNCmp((char*)NX_CAS_MENU_SMTCARDINFO_CA_SERIAL_NUMBER_ID, (char*)pszItemIdStrId, lStrLen))
	{
		*eMenuItemId = eNxMenuSmtCardInfo_CASerialNum_Label;
	}
	else if (0 == HxSTD_StrNCmp((char*)NX_CAS_MENU_SMTCARDINFO_CHIPSET_NUID_ID, (char*)pszItemIdStrId, lStrLen))
	{
		*eMenuItemId = eNxMenuSmtCardInfo_ChipsetNUID_Label;
	}
	else if (0 == HxSTD_StrNCmp((char*)NX_CAS_MENU_SMTCARDINFO_CHIPSET_TYPE_ID, (char*)pszItemIdStrId, lStrLen))
	{
		*eMenuItemId = eNxMenuSmtCardInfo_MAX;
	}
	else if (0 == HxSTD_StrNCmp((char*)NX_CAS_MENU_SMTCARDINFO_CHIPSET_REV_ID, (char*)pszItemIdStrId, lStrLen))
	{
		*eMenuItemId = eNxMenuSmtCardInfo_MAX;
	}
	else
	{
		*eMenuItemId = eNxMenuSmtCardInfo_MAX;
	}

	NX_APP_Debug("[Info] eMenuItemId:(%d)\n", *eMenuItemId);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSCInfo_na_UpdateInfoItemsValue(Nx_MenuSmtCardInfoNAContent_t *pstContents, const HCHAR *pszItemIdStrId, const HCHAR *pszItemValue)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bFindItem = FALSE;
	HINT32			lStrLen = 0;
	HUINT32			i = 0, lItemIdx = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	NX_CasNaMessage_Type eCasNaMessageType = eNxCasNaMessage_Type_None;
	Nx_MenuSmtCardInfoUiCompIds_e eMenuItemId = eNxMenuSmtCardInfo_MAX;

	eResult = nx_menuSCInfo_na_ConvertMwCASIdToApCASId((HCHAR*)pszItemIdStrId, &eMenuItemId);
	if ((eResult == ONDK_RET_OK) && (eMenuItemId != eNxMenuSmtCardInfo_MAX))
	{
		bFindItem = FALSE;
		lItemIdx = 0;
		for (i = 0; i < pstContents->stItemList.ulSmtCardItemNum; i++)
		{
			if (pstContents->stItemList.aulSmtCardItemList[i] == (HUINT32)eMenuItemId)
			{
				bFindItem = TRUE;
				lItemIdx = i;
			}
		}

		if (bFindItem == TRUE)
		{
			HxSTD_memset(pstContents->szItemTextList[lItemIdx], 0x00, NX_TEXT_SIZE_128);
			pstContents->szItemTextList[lItemIdx][0] = '\0';
			if (pszItemValue != NULL)
			{
				lStrLen = HxSTD_StrLen(pszItemValue);
				if (lStrLen > 0)
				{
					HxSTD_snprintf ((char*)pstContents->szItemTextList[lItemIdx], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)pszItemValue);
				}
				else
				{
					NX_APP_Error("Error!!! (lStrLen < 1) \n");
				}
			}
			else
			{
				NX_APP_Error("Error!!! (pszItemValue != NULL) \n");
			}
		}
	}
	else
	{
		hError = NX_CAS_NAGRA_GetErrorMessage((HCHAR*)pszItemIdStrId, &eCasNaMessageType);
		if ((hError != ERR_OK) || (eCasNaMessageType == eNxCasNaMessage_Type_None))
		{
			NX_APP_Error("Error!!! nx_menuSCInfo_na_ConvertMwCASIdToApCASId() - pszItemIdStrId :(%s)\n", pszItemIdStrId);
		}
		else
		{
			pstContents->eCasMessageType = eCasNaMessageType;
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSCInfo_na_LoadInfoItemsValue(Nx_MenuSmtCardInfoNAContent_t *pstContents)
{
	HUINT8			**ppListOfItem = NULL;
	HUINT8			szTextBuffer[NX_TEXT_SIZE_128];
	HUINT32			i = 0;

	ppListOfItem = NX_PRISM_MENU_MenuItem_AllocTextList (pstContents->stItemList.ulSmtCardItemNum, NX_TEXT_SIZE_128);
	if (ppListOfItem == NULL)
	{
		NX_APP_Error("ERROR!!! NX_PRISM_MENU_MenuItem_AllocTextList() Fail !!\n");
		return	ONDK_RET_FAIL;
	}
	else
	{
		for (i = 0; i < pstContents->stItemList.ulSmtCardItemNum; i++)
		{
			HxSTD_memset(ppListOfItem[i], 0x00, NX_TEXT_SIZE_128);
			ppListOfItem[i][0] = '\0';

			switch (pstContents->stItemList.aulSmtCardItemList[i])
			{
				case eNxMenuSmtCardInfo_ProjName_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
				case eNxMenuSmtCardInfo_SmtCardVer_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
				case eNxMenuSmtCardInfo_ChipsetNUID_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
				case eNxMenuSmtCardInfo_CAVersion_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
				case eNxMenuSmtCardInfo_SmtSerialNum_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
				case eNxMenuSmtCardInfo_CASerialNum_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
					HxSTD_snprintf ((char*)ppListOfItem[i], NX_TEXT_SIZE_128, (char*)"%s", (HUINT8*)szTextBuffer);
					break;
#if defined(CONFIG_MW_CI)
				case eNxMenuSmtCardInfo_HDPlusCardCreditValidUntil_Label:
					HxSTD_memset(szTextBuffer, 0x00, NX_TEXT_SIZE_128);
					szTextBuffer[0] = '\0';
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


static ONDK_Result_t	nx_menuSCInfo_na_AddItemList(NxUiSmtCardInfoItemList_t *pstSysItemList, HUINT32 ulItemId, HUINT32 ulItemValueId)
{
	HUINT32		ulIndex = 0;

	if (pstSysItemList->ulSmtCardItemNum >= NX_MENU_SMTCARDINFO_ITEM_MAX_NUM)
	{
		NX_APP_Error("Error!!! OUT OF RANGE!! pstSysItemList->ulSmtCardItemNum(%d), NX_UTIL_OPTION_MAX_NUM(%d)\n", pstSysItemList->ulSmtCardItemNum, NX_MENU_SMTCARDINFO_ITEM_MAX_NUM);
		return	ONDK_RET_FAIL;
	}

	ulIndex = pstSysItemList->ulSmtCardItemNum;
	pstSysItemList->aulSmtCardItemList[ulIndex] = ulItemId;
	pstSysItemList->aulSmtCardItemValueList[ulIndex] = ulItemValueId;
	pstSysItemList->ulSmtCardItemNum = (ulIndex + 1);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSCInfo_na_InitSysInfoItems(Nx_MenuSmtCardInfoNAContent_t *pstContents)
{
	DxOperator_e	eOpType = eDxOPERATOR_None;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! (pstContents == NULL)\n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

	pstContents->ulSessionHandleKey = eNxCasNAHandleKey_None;
	pstContents->eCasMessageType = eNxCasNaMessage_Type_None;

	// Open Session and Request CAS Data
	nx_menuSCInfo_na_RequestSessionMenuData(pstContents);

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_SMARTCARD_INFORMATION_ID), MAX_ELEMENT_TEXT_LENGTH);

	// Operator Type
	NX_SYSTEM_GetOperatorType(&eOpType);
	switch (eOpType)
	{
		default:
			nx_menuSCInfo_na_AddItemList(&pstContents->stItemList, eNxMenuSmtCardInfo_ProjName_Label, eNxMenuSmtCardInfo_ProjName_Value);
			nx_menuSCInfo_na_AddItemList(&pstContents->stItemList, eNxMenuSmtCardInfo_SmtCardVer_Label, eNxMenuSmtCardInfo_SmtCardVer_Value);
			nx_menuSCInfo_na_AddItemList(&pstContents->stItemList, eNxMenuSmtCardInfo_ChipsetNUID_Label, eNxMenuSmtCardInfo_ChipsetNUID_Value);
			nx_menuSCInfo_na_AddItemList(&pstContents->stItemList, eNxMenuSmtCardInfo_CAVersion_Label, eNxMenuSmtCardInfo_CAVersion_Value);
			nx_menuSCInfo_na_AddItemList(&pstContents->stItemList, eNxMenuSmtCardInfo_SmtSerialNum_Label, eNxMenuSmtCardInfo_SmtSerialNum_Value);
			nx_menuSCInfo_na_AddItemList(&pstContents->stItemList, eNxMenuSmtCardInfo_CASerialNum_Label, eNxMenuSmtCardInfo_CASerialNum_Value);
#if defined(CONFIG_MW_CI)
			nx_menuSCInfo_na_AddItemList(&pstContents->stItemList, eNxMenuSmtCardInfo_HDPlusCardCreditValidUntil_Label, eNxMenuSmtCardInfo_HDPlusCardCreditValidUntil_Value);
#endif
			break;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSCInfo_na_ParseJsonString(Nx_MenuSmtCardInfoNAContent_t *pstContents, const HCHAR* pJsonString)
{
	HxJSON_t	root = NULL, szJsonUidata = NULL, szJsonGriditem = NULL, szJsonMenuData = NULL, szJsonItem = NULL;
	HxJSON_t	szJsonMenuItemId = NULL, szJsonMenuItemValue = NULL;
	const HCHAR		*pszString = NULL, *pszCasType = NULL, *pszMenuItemId = NULL, *pszMenuItemValue = NULL;
	HINT32			i = 0, lMenuItemCnt = 0, lIndex = 0, lStrlen = 0, lSlotNum = 0;
	DxCAS_GroupId_e	eCasGroupId = eDxCAS_GROUPID_NONE;

	if (pJsonString == NULL)
	{
		NX_APP_Error("Error!!! (pJsonString == NULL) \n");
		return	ONDK_RET_INITFAIL;
	}

	/*
		[ ] : Array
		{ } : Object
	*/

	root = HLIB_JSON_Decode(pJsonString);
	if (root == NULL)
	{
		NX_APP_Error("Error!!! (root == NULL) \n");
		return	ONDK_RET_INITFAIL;
	}

	pszString = HLIB_JSON_Encode(root, NULL, 0);
	NX_APP_Info("\n\n=====>[JSON Data] root : (%s) \n\n", pszString);

	// Get CasType
	pszCasType = HLIB_JSON_Object_GetString(root, (const HCHAR*)NX_CAS_PARSE_CASTYPE_ID);
	NX_APP_Print("[MENU Info] - castype : (%s)\n", pszCasType);

	eCasGroupId = NX_CAS_GetCasGroupIdByCasTypeText((HUINT8*)pszCasType);
	if (eCasGroupId != eDxCAS_GROUPID_NA)
	{
		NX_APP_Error("Error!!! Not Support Cas Group:(0x%x)\n", eCasGroupId);
		return	ONDK_RET_INITFAIL;
	}

	// Get Slot Number
	lSlotNum = HLIB_JSON_Object_GetInteger(root, NX_CAS_PARSE_SLOTNUMBER_ID);
	NX_APP_Print("[MENU Info] - slotnumber : (%d) \n\n", lSlotNum);

	// Check UI Type : Grid
	if (0 == HxSTD_StrNCmp(HLIB_JSON_Object_GetString(root, (HCHAR*)NX_CAS_PARSE_UITYPE_ID), (char*)NX_CAS_PARSE_UITYPE_GRID_ID,  NX_MENU_SMTCARDINFO_PARSE_ITEM_MAX - 1))
	{
		// Get UI Data
		szJsonUidata = HLIB_JSON_Object_GetObject(root, (HCHAR*)NX_CAS_PARSE_UIDATA_ID);
		if (szJsonUidata != NULL)
		{
			pszString = HLIB_JSON_Encode(szJsonUidata, NULL, 0);
			NX_APP_Info("\n\n=====>[JSON Data] uidata : (%s) \n\n", pszString);

			// Get Grid Item
			szJsonGriditem = HLIB_JSON_Object_GetArray(szJsonUidata, (HCHAR*)NX_CAS_PARSE_UIDATA_GRIDITME_ID);
			if (szJsonGriditem != NULL)
			{
				pszString = HLIB_JSON_Encode(szJsonGriditem, NULL, 0);
				NX_APP_Info("\n\n=====>[JSON Data] griditem : (%s) \n\n", pszString);

				lMenuItemCnt = HLIB_JSON_GetSize(szJsonGriditem);
				NX_APP_Print("[MENU Info] - lMenuItemCnt :(%d)\n", lMenuItemCnt);
				for (i = 0; i < lMenuItemCnt; ++i)
				{
					// Get Item
					szJsonItem = HxJSON_Array_Get(szJsonGriditem, i);
					if (szJsonItem != NULL)
					{
						pszString = HLIB_JSON_Encode(szJsonItem, NULL, 0);
						NX_APP_Info("=====>[JSON Data] i:[%d], pszString:(%s)\n", i, pszString);

						lIndex = HLIB_JSON_Object_GetInteger(szJsonItem, (HCHAR*)NX_CAS_PARSE_INDEX_ID);
						NX_APP_Print("[MENU Info] - index:(%d)\n", lIndex);

						lStrlen = HLIB_JSON_Object_GetInteger(szJsonItem, (HCHAR*)NX_CAS_PARSE_STRINGLENGTH_ID);
						NX_APP_Print("[MENU Info] - lStrlen:(%d)\n", lStrlen);

						// Get Menu Data
						szJsonMenuData = HLIB_JSON_Object_GetArray(szJsonItem, (HCHAR*)NX_CAS_PARSE_STRING_ID);
						if (szJsonMenuData)
						{
							// Get Menu Item id
							szJsonMenuItemId = HxJSON_Array_Get(szJsonMenuData, NX_CAS_PARSE_GRIDITEM_MENUINFO_ITEMID_INDEX);
							if (szJsonMenuItemId)
							{
								pszMenuItemId = HLIB_JSON_Object_GetString(szJsonMenuItemId, NX_CAS_PARSE_ID_ID);
								NX_APP_Print("[MENU Info] - pszMenuItemId:(%s)\n", pszMenuItemId);
							}

							// Get Menu Item Vlue
							szJsonMenuItemValue = HxJSON_Array_Get(szJsonMenuData, NX_CAS_PARSE_GRIDITEM_MENUINFO_VALUE_INDEX);
							if (szJsonMenuItemValue && HxJSON_IsString(szJsonMenuItemValue))
							{
								pszMenuItemValue = HxJSON_String_Get(szJsonMenuItemValue);
								NX_APP_Print("[MENU Info] - pszMenuItemValue:(%s)\n", pszMenuItemValue);
							}

							// Update Item Value
							nx_menuSCInfo_na_UpdateInfoItemsValue(pstContents, pszMenuItemId, pszMenuItemValue);
						}
					}
				}

				nx_menuSCInfo_na_ReDrawSmartCardInfo(pstContents);
				nx_menuSCInfo_na_DrawSmartCardErrorMessage(pstContents, 0, TRUE);
			}
		}
	}

	return	ONDK_RET_FAIL;
}


static ONDK_Result_t	nx_menuSCInfo_na_ClosedSessionMenuData(Nx_MenuSmtCardInfoNAContent_t *pstContents)
{
	HERROR		hError = ERR_FAIL;

	hError = NX_CAS_CloseSession(eNxCasNAHandleKey_SmartCardInfo, eDxCAS_GROUPID_NA);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_CAS_CloseSession() Fail!! \n");
	}

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSCInfo_na_RequestSessionMenuData(Nx_MenuSmtCardInfoNAContent_t *pstContents)
{
	HERROR			hError = ERR_FAIL;
	Nx_CasSessionInfo_t		stCasSessionInfo;

	HxSTD_memset(&stCasSessionInfo, 0x00, sizeof(Nx_CasSessionInfo_t));

	stCasSessionInfo.ulSessionHandleKey = eNxCasNAHandleKey_SmartCardInfo;
	stCasSessionInfo.eCasGroupId = eDxCAS_GROUPID_NA;
	stCasSessionInfo.ulSlot = eNxCasSlot_0;
	stCasSessionInfo.eSessionType = eDxCAS_SESSIONTYPE_MENU;
	stCasSessionInfo.pszParamData = (HUINT8*)NX_CAS_NA_MENU_PARAM_DATA;

	hError = NX_CAS_RequestCasData(&stCasSessionInfo);
	if (hError != ERR_OK)
	{
		NX_APP_Error("Error!!! NX_CAS_NAGRA_RequestCasData() Fail!! \n");
	}
	else
	{
		pstContents->ulSessionHandleKey = eNxCasNAHandleKey_SmartCardInfo;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSCInfo_na_MsgGwmCreate(Nx_MenuSmtCardInfoNAContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t	eResult = ONDK_RET_FAIL;
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Init Menu Item
	eResult = nx_menuSCInfo_na_InitSysInfoItems(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		NX_APP_Error("Error!!! nx_menuSCInfo_na_InitSysInfoItems() Fail\n");
		return	eResult;
	}
	pstContents->lPrevCurIdx = p3;

	// Load Menu Item Value
	eResult = nx_menuSCInfo_na_LoadInfoItemsValue(pstContents);
	if (eResult != ONDK_RET_OK)
	{
		ONDK_GWM_APP_Destroy(0);
		NX_APP_Error("Error!!! nx_menuSCInfo_na_LoadInfoItemsValue() Fail\n");
		return	eResult;
	}

	// Draw Menu Item
	nx_menuSCInfo_na_DrawDefaultUI(pstContents);

	// OSD Time Out
	ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);

	// Timer Update
	ONDK_GWM_SetTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID, NX_SETTING_TIME_UPDATE_TIME);

	// Smart Card Update Timer
	ONDK_GWM_SetTimer(NX_MENU_SMTCARDINFO_UPDATE_TIMER_ID, NX_MENU_SMTCARDINFO_UPDATE_TIME);

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


static ONDK_Result_t	nx_menuSCInfo_na_MsgGwmDestroy(Nx_MenuSmtCardInfoNAContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// Kill Timer
	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);
	ONDK_GWM_KillTimer(NX_MENU_SMTCARDINFO_UPDATE_TIMER_ID);

	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	// Closed Session
	nx_menuSCInfo_na_ClosedSessionMenuData(pstContents);

	if (pstContents->szItemTextList)
	{
		NX_PRISM_MENU_MenuItem_FreeTextList(pstContents->stItemList.ulSmtCardItemNum, pstContents->szItemTextList);
		pstContents->szItemTextList = NULL;
	}

	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuSmtCardInfoNAContent_t));

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSCInfo_na_MsgGwmKeyDown(Nx_MenuSmtCardInfoNAContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1, lPrevCurIdx = 0;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! Please check!! \n");
		return	GWM_RET_SOURCE_NULL;
	}

	// Rest timer
	eResult = ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
	if (eResult == GWM_RET_LINK_CONFLICT)	// Alreay added Timer
	{
		ONDK_GWM_ResetTimer(NX_SETTING_TIMEOUT_ID);
	}

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
			NX_APP_Print("nx_menuSCInfo_na_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSCInfo_na_MsgGwmTimer(Nx_MenuSmtCardInfoNAContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuSCInfo_na_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}
	else if (p1 == NX_MENU_SMTCARDINFO_UPDATE_TIMER_ID)
	{
		nx_menuSCInfo_na_ClosedSessionMenuData(pstContents);
		nx_menuSCInfo_na_RequestSessionMenuData(pstContents);
	}
	else
	{
		NX_APP_Error("Error Invalid Timer - ID : (0x%x) \n", p1);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSCInfo_na_MsgGwmApkMsg(Nx_MenuSmtCardInfoNAContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	GWM_APK_MSG_Type_e		eAPK_Type = (GWM_APK_MSG_Type_e)handle;


	switch(eAPK_Type)
	{
		case GWM_APK_CAS_UI_TRIGGERED:
			nx_menuSCInfo_na_MsgGwmApkMsgCasUiTriggered(pstContents, p1, p2, p3);
			break;

		default:
			NX_APP_Error("Error!!! SKIP APK Message : (0x%x) \n", eAPK_Type);
			break;
	}

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_menuSCInfo_na_MsgGwmApkMsgCasUiTriggered(Nx_MenuSmtCardInfoNAContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HBOOL							bIsValidSession = FALSE;
	Handle_t						hSession = HANDLE_NULL;
	DxCAS_MMI_Event_t				*pstEvent = NULL;
	DxCAS_MMI_SessionEventType_e	eEventType = eDxCASMMI_EVENTTYPE_NONE;
	DxCAS_MmiSessionType_e			eSessionType = eDxCAS_SESSIONTYPE_NONE;


	eEventType = (DxCAS_MMI_SessionEventType_e)p1;
	eSessionType = (DxCAS_MmiSessionType_e)p2;
	hSession = (Handle_t)p3;

	bIsValidSession = NX_CAS_FindSessionWithHandleAndTypeInGroup((HUINT32)hSession, eSessionType, eDxCAS_GROUPID_NA);
	if (bIsValidSession == FALSE)
	{
		NX_APP_Error("Error!!! Invalid Session hSession:(0x%x), eSession Type:(0x%x) \n", hSession, eSessionType);
		return GWM_RET_FAIL;
	}

	switch (eEventType)
	{
		case eDxCASMMI_EVENTTYPE_DISPLAYUI:
			NX_CAS_MMI_GetUiEvent(eSessionType, hSession, &pstEvent);
			if ((pstEvent != NULL))
			{
				if ((pstEvent->eModule == eDxCAS_GROUPID_NA) && (pstEvent->hSession == hSession) && (pstEvent->sessionType == eSessionType))
				{
					nx_menuSCInfo_na_ParseJsonString(pstContents, (const HCHAR*)pstEvent->pszEncodedUiEvent);
				}
				NX_CAS_MMI_RemoveUiEvent(eSessionType, pstEvent);
			}
			else
			{
				NX_APP_Error("Error!!! NX_CAS_MMI_GetUiEvent() is NULL \n");
			}
			nx_menuSCInfo_na_ClosedSessionMenuData(pstContents);
			break;

		case eDxCASMMI_EVENTTYPE_REQUESTSESSIONSTOP:
			break;

		default:
			NX_APP_Error("Error!!! Not Support Event Type : (0x%x) \n", eEventType);
			break;
	}

	return GWM_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_SmtCardInfo_NA_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuSmtCardInfoNAContent_t	*pstContents = NULL;

	pstContents = nx_menuSCInfo_na_GetContents();

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE!\n");
			eResult = nx_menuSCInfo_na_MsgGwmCreate(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_menuSCInfo_na_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY!\n");
			eResult = nx_menuSCInfo_na_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuSCInfo_na_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_APK_MESSAGE:
			NX_APP_Message("MSG_GWM_APK_MESSAGE!\n");
			eResult = nx_menuSCInfo_na_MsgGwmApkMsg(pstContents, handle, p1, p2, p3);
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

