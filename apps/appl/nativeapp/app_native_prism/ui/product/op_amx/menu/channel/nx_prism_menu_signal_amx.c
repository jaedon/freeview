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
 * @file	  		nx_prism_menu_signal_amx.c
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
#include <nx_prism_menu_contents.h>
#include <nx_prism_ui.h>
#include <nx_prism_component.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_MENU_SIGNAL_AMX_MAX_TUNER_NUM						1

#define	NX_MENU_SIGNAL_AMX_INFO_TEXT_LS							28

#define	NX_MENU_SIGNAL_AMX_INFO_BOX_X							60
#define	NX_MENU_SIGNAL_AMX_INFO_BOX_Y							114
#define	NX_MENU_SIGNAL_AMX_INFO_BOX_W							750
#define	NX_MENU_SIGNAL_AMX_INFO_BOX_H							(112 + NX_MENU_SIGNAL_AMX_INFO_TEXT_LS)
#define	NX_MENU_SIGNAL_AMX_INFO_BOX_MG							10

#define	NX_MENU_SIGNAL_AMX_INFO_TUNER_TEXT_X					18
#define	NX_MENU_SIGNAL_AMX_INFO_TUNER_TEXT_Y					(34 - NX_PRISM_FONT_SIZE_20)
#define	NX_MENU_SIGNAL_AMX_INFO_TUNER_TEXT_W					100
#define	NX_MENU_SIGNAL_AMX_INFO_TUNER_TEXT_H					(NX_PRISM_FONT_SIZE_20 + NX_PRISM_FONT_VSPACE)

#define	NX_MENU_SIGNAL_AMX_INFO_TP_TEXT_X						122
#define	NX_MENU_SIGNAL_AMX_INFO_TP_TEXT_Y						(34 - NX_PRISM_FONT_SIZE_20)
#define	NX_MENU_SIGNAL_AMX_INFO_TP_TEXT_W						610
#define	NX_MENU_SIGNAL_AMX_INFO_TP_TEXT_H						(NX_PRISM_FONT_SIZE_20 + NX_PRISM_FONT_VSPACE)

#define	NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_X				18
#define	NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_Y				(66 - NX_PRISM_FONT_SIZE_18)
#define	NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_W				100
#define	NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_H				(NX_PRISM_FONT_SIZE_18 + NX_PRISM_FONT_VSPACE)

#define	NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_X					122
#define	NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_Y					58
#define	NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_W					530
#define	NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_H					3
#define	NX_MENU_SIGNAL_AMX_INFO_BAR_LS							28

#define	NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_X					662
#define	NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_Y					(68 - NX_PRISM_FONT_SIZE_18)
#define	NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_W					70
#define	NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_H					(NX_PRISM_FONT_SIZE_18 + NX_PRISM_FONT_VSPACE)

#define	NX_MENU_SIGNAL_AMX_INFO_QUALITY_TITLE_X					NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_X
#define	NX_MENU_SIGNAL_AMX_INFO_QUALITY_TITLE_Y					(NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_Y + NX_MENU_SIGNAL_AMX_INFO_TEXT_LS)
#define	NX_MENU_SIGNAL_AMX_INFO_QUALITY_TITLE_W					NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_W
#define	NX_MENU_SIGNAL_AMX_INFO_QUALITY_TITLE_H					NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_H

#define	NX_MENU_SIGNAL_AMX_INFO_QUALITY_BAR_X					NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_X
#define	NX_MENU_SIGNAL_AMX_INFO_QUALITY_BAR_Y					(NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_Y + NX_MENU_SIGNAL_AMX_INFO_BAR_LS)
#define	NX_MENU_SIGNAL_AMX_INFO_QUALITY_BAR_W					NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_W
#define	NX_MENU_SIGNAL_AMX_INFO_QUALITY_BAR_H					NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_H

#define	NX_MENU_SIGNAL_AMX_INFO_QUALITY_TEXT_X					NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_X
#define	NX_MENU_SIGNAL_AMX_INFO_QUALITY_TEXT_Y					(NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_Y +  NX_MENU_SIGNAL_AMX_INFO_TEXT_LS)
#define	NX_MENU_SIGNAL_AMX_INFO_QUALITY_TEXT_W					NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_W
#define	NX_MENU_SIGNAL_AMX_INFO_QUALITY_TEXT_H					NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_H

#define	NX_MENU_SIGNAL_AMX_INFO_BER_TITLE_X						NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_X
#define	NX_MENU_SIGNAL_AMX_INFO_BER_TITLE_Y						(NX_MENU_SIGNAL_AMX_INFO_QUALITY_TITLE_Y + NX_MENU_SIGNAL_AMX_INFO_TEXT_LS)
#define	NX_MENU_SIGNAL_AMX_INFO_BER_TITLE_W						NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_W
#define	NX_MENU_SIGNAL_AMX_INFO_BER_TITLE_H						NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_H

#define	NX_MENU_SIGNAL_AMX_INFO_BER_BAR_X						NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_X
#define	NX_MENU_SIGNAL_AMX_INFO_BER_BAR_Y						(NX_MENU_SIGNAL_AMX_INFO_QUALITY_BAR_Y + NX_MENU_SIGNAL_AMX_INFO_BAR_LS)
#define	NX_MENU_SIGNAL_AMX_INFO_BER_BAR_W						NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_W
#define	NX_MENU_SIGNAL_AMX_INFO_BER_BAR_H						NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_H

#define	NX_MENU_SIGNAL_AMX_INFO_BER_TEXT_X						NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_X
#define	NX_MENU_SIGNAL_AMX_INFO_BER_TEXT_Y						(NX_MENU_SIGNAL_AMX_INFO_QUALITY_TEXT_Y +  NX_MENU_SIGNAL_AMX_INFO_TEXT_LS)
#define	NX_MENU_SIGNAL_AMX_INFO_BER_TEXT_W						NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_W
#define	NX_MENU_SIGNAL_AMX_INFO_BER_TEXT_H						NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_H

#define	NX_MENU_SIGNAL_AMX_INFO_BTN_ANTENNA_CONNECTION_X		205
#define	NX_MENU_SIGNAL_AMX_INFO_BTN_ANTENNA_CONNECTION_Y		615
#define	NX_MENU_SIGNAL_AMX_INFO_BTN_ANTENNA_CONNECTION_W		460
#define	NX_MENU_SIGNAL_AMX_INFO_BTN_ANTENNA_CONNECTION_H		55

#define	NX_MENU_SIGNAL_AMX_INFO_UPDATE_TIME						(5*NX_COMMON_TIME_SECOND)	/* 5 Second */


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef enum {
	eNxSettingsItem_Signal_AMX_InfoBox = 0,
	eNxSettingsItem_Signal_AMX_TunerTitle,
	eNxSettingsItem_Signal_AMX_TunerInfo,
	eNxSettingsItem_Signal_AMX_S_Title,
	eNxSettingsItem_Signal_AMX_S_Bar,
	eNxSettingsItem_Signal_AMX_S_Value,
	eNxSettingsItem_Signal_AMX_Q_Title,
	eNxSettingsItem_Signal_AMX_Q_Bar,
	eNxSettingsItem_Signal_AMX_Q_Value,
	eNxSettingsItem_Signal_AMX_ItemMax
} NxSettingsItem_SignalAMXItem_t;

typedef enum {
	eNxSettingsItem_Signal_AMX_T1_InfoBox = 0,
	eNxSettingsItem_Signal_AMX_T1_TunerTitle,
	eNxSettingsItem_Signal_AMX_T1_TunerInfo,
	eNxSettingsItem_Signal_AMX_T1_S_Title,
	eNxSettingsItem_Signal_AMX_T1_S_Bar,
	eNxSettingsItem_Signal_AMX_T1_S_Value,
	eNxSettingsItem_Signal_AMX_T1_Q_Title,
	eNxSettingsItem_Signal_AMX_T1_Q_Bar,
	eNxSettingsItem_Signal_AMX_T1_Q_Value,
	eNxSettingsItem_Signal_AMX_T1_B_Title,
	eNxSettingsItem_Signal_AMX_T1_B_Bar,
	eNxSettingsItem_Signal_AMX_T1_B_Value,

	eNxSettingsItem_Signal_AMX_Check_Antenna,	// 36
	eNxSettingsItem_Signal_AMX_UpdateId,		// 37
	eNxSettingsItem_Signal_AMX_Done,			// 38

	eNxSettingsItem_Signal_AMX_MAX
} NxSettingsItem_SignalAMX_t;


typedef struct tag_NX_MENUSIGNALAMX_SET_CONTENT
{
	/**@breif Public value */
	HUINT32					ulCurFocus;
	HUINT32					ulNumOfSubItem, ulMaxNumOfSubItem;
	Nx_SettingsLeafSubItem_t	*subItems;

	/**@breif Specific value */
	HUINT8					ucTitle[MAX_ELEMENT_TEXT_LENGTH];

	/* tuner */
	HUINT32					ulTunerNum;		// Number of tuner
#if defined(CONFIG_MW_CH_SATELLITE)
	NX_SAT_AntennaType_e	eAntType;
#endif

	DxTuneParam_t			*pstTuneParam;

	HUINT32					aulStrength[NX_MENU_SIGNAL_AMX_MAX_TUNER_NUM];
	HUINT32 				aulQuality[NX_MENU_SIGNAL_AMX_MAX_TUNER_NUM];
	HUINT32 				aulBER[NX_MENU_SIGNAL_AMX_MAX_TUNER_NUM];

	HUINT8					szTunerTitle[NX_MENU_SIGNAL_AMX_MAX_TUNER_NUM][NX_TEXT_SIZE_32];
	HUINT8					szTPInfo[NX_MENU_SIGNAL_AMX_MAX_TUNER_NUM][NX_TEXT_SIZE_128];
	HUINT8					szStrengthPer[NX_MENU_SIGNAL_AMX_MAX_TUNER_NUM][NX_TEXT_SIZE_8];
	HUINT8					szQualityPer[NX_MENU_SIGNAL_AMX_MAX_TUNER_NUM][NX_TEXT_SIZE_8];
	HUINT8					szBERPer[NX_MENU_SIGNAL_AMX_MAX_TUNER_NUM][NX_TEXT_SIZE_8];

	HINT32					lPrevCurIdx;

	HUINT8					szDate[NX_TEXT_SIZE_32];
	HUINT8					szTime[NX_TEXT_SIZE_32];
} Nx_MenuSignalAMXSetContent_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static Nx_MenuSignalAMXSetContent_t 		s_stSignalAMXContent;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_menuSignalTestAMX_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3);
static void				nx_menuSignalTestAMX_CreateResponseDialouge(HUINT8* pszString);

static inline			Nx_MenuSignalAMXSetContent_t* nx_menuSignalTestAMX_GetContents(void);

static HINT32			nx_menuSignalTestAMX_ConvertObjIdToItemIdx(HINT32 lObjId);
static HINT32			nx_menuSignalTestAMX_ConvertItemIdxToObjId(HINT32 lItemId);
static HCHAR			*nx_menuSignalTestAMX_GetTunerResource(HUINT32 ulIndex);

static void				nx_menuSignalTestAMX_ActionGoPrevMenu(Nx_MenuSignalAMXSetContent_t *pstContents);
static void				nx_menuSignalTestAMX_UpdateCurrentTime(Nx_MenuSignalAMXSetContent_t *pstContents);
static ONDK_Result_t 	nx_menuSignalTestAMX_UpdateSignalInfo(Nx_MenuSignalAMXSetContent_t *pstContents, HUINT32 ulIndex);
static ONDK_Result_t	nx_menuSignalTestAMX_CheckAntennaConnection(Nx_MenuSignalAMXSetContent_t *pstContents);

#if defined(CONFIG_MW_CH_SATELLITE)
static ONDK_Result_t 	nx_menuSignalTestAMX_DrawForSatellite(Nx_MenuSignalAMXSetContent_t *pstContents);
#endif

static ONDK_Result_t 	nx_menuSignalTestAMX_ReDrawSignalInfo(Nx_MenuSignalAMXSetContent_t *pstContents);
static ONDK_Result_t 	nx_menuSignalTestAMX_DrawSignalInfo(Nx_MenuSignalAMXSetContent_t *pstContents, HUINT32 ulIndex);
static ONDK_Result_t 	nx_menuSignalTestAMX_DrawDefaultUI(Nx_MenuSignalAMXSetContent_t *pstContents);
static ONDK_Result_t	nx_menuSignalTestAMX_LoadSettingsItems(Nx_MenuSignalAMXSetContent_t *pstContents);
static ONDK_Result_t	nx_menuSignalTestAMX_InitSettingsItems(Nx_MenuSignalAMXSetContent_t *pstContents);

static ONDK_Result_t	nx_menuSignalTestAMX_MsgGwmCreate(Nx_MenuSignalAMXSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_menuSignalTestAMX_MsgGwmDestroy(Nx_MenuSignalAMXSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSignalTestAMX_MsgGwmHapiMsg(Nx_MenuSignalAMXSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSignalTestAMX_MsgGwmKeyDown(Nx_MenuSignalAMXSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_menuSignalTestAMX_MsgGwmClicked(Nx_MenuSignalAMXSetContent_t *pstContents, HINT32 lObjId);
static ONDK_Result_t	nx_menuSignalTestAMX_MsgGwmTimer(Nx_MenuSignalAMXSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Response_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_menuSignalTestAMX_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
{
	switch (lMessage)
	{
		case MSG_GWM_KEYDOWN:
			NX_PRISM_UTIL_KeyConv_UiCommon (&lParam1);
			switch (lParam1)
			{
				case KEY_RED:
				case KEY_GREEN:
				case KEY_YELLOW:
				case KEY_BLUE:
				case KEY_ARROWUP :
				case KEY_ARROWDOWN:
				case KEY_TVRADIO:
				case KEY_EXT_AV:
				case KEY_COMBO:
				case KEY_TEXT:
				case KEY_TTX_HOLD:
				case KEY_TTX_SUBPAGE:
				case KEY_AUDIO:
				case KEY_SUBTITLE:
				case KEY_WIDE:
				case KEY_3D:
				case KEY_INFO:
				case KEY_BACK:
				case KEY_EXT_AV_PLUS:
				case KEY_EXT_AV_MINUS:
					ONDK_GWM_APP_Destroy(0);
					return	ONDK_MESSAGE_BREAK;
				default:
					break;
			}
			break;

		case MSG_APP_SYSTEM_SVC_CHANGED :
			ONDK_GWM_APP_Destroy(0);
			return	ONDK_MESSAGE_PASS;

		default :
			break;
	}

	return	NX_PRISM_DIALOGUE_ResponseMessage_Proc(lMessage, hHandle, lParam1, lParam2, lParam3);
}


static void		nx_menuSignalTestAMX_CreateResponseDialouge(HUINT8* pszString)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_menuSignalTestAMX_ResponseDialogProc", APP_DEFAULT_PRIORITY,
						nx_menuSignalTestAMX_ResponseDialogProc, 0, (HINT32)pszString, 0, 0);
}


/******************************************************************************/
#define __Functions______________________________________________
/******************************************************************************/

static inline Nx_MenuSignalAMXSetContent_t* nx_menuSignalTestAMX_GetContents(void)
{
	return	&s_stSignalAMXContent;
}


static HINT32	nx_menuSignalTestAMX_ConvertObjIdToItemIdx(HINT32 lObjId)
{
	HINT32	lItemId = 0;

	lItemId = (lObjId & ~eNxSetting_SignalTest);

	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_Signal_AMX_MAX))
	{
		return -1;
	}

	return	lItemId;
}


static HINT32	nx_menuSignalTestAMX_ConvertItemIdxToObjId(HINT32 lItemId)
{
	if ((lItemId < 0) || (lItemId >= eNxSettingsItem_Signal_AMX_MAX))
	{
		return 0;
	}

	return	(eNxSetting_SignalTest | lItemId);
}


static HCHAR	*nx_menuSignalTestAMX_GetTunerResource(HUINT32 ulIndex)
{
	HCHAR	*szTunerRes = NULL;

	switch (ulIndex)
	{
		case 0:		szTunerRes = RES_TUNER_1_ID;	break;
		case 1:		szTunerRes = RES_TUNER_2_ID;	break;
		case 2:		szTunerRes = RES_TUNER_3_ID;	break;
		case 3:		szTunerRes = RES_TUNER_4_ID;	break;
		default:	szTunerRes = RES_TUNER_1_ID;	break;
	}

	return szTunerRes;
}


static void		nx_menuSignalTestAMX_ActionGoPrevMenu(Nx_MenuSignalAMXSetContent_t *pstContents)
{
	HINT32			lPrevCurIdx = 0;

	lPrevCurIdx = pstContents->lPrevCurIdx;
	ONDK_GWM_APP_Destroy(0);
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_MENU_Channel_Proc", (APP_DEFAULT_PRIORITY | UISTATE_MENU),
						(ONDK_GWM_Callback_t)NX_PRISM_MENU_Channel_Proc, 0, 0, 0, lPrevCurIdx);
}


static ONDK_Result_t 	nx_menuSignalTestAMX_UpdateSignalInfo(Nx_MenuSignalAMXSetContent_t *pstContents, HUINT32 ulIndex)
{
	HINT32			lObjId = -1, lItemidx = -1;


	// Tuner Info
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_TunerInfo + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_SetText(lObjId, (HCHAR *)pstContents->szTPInfo[ulIndex]);

	// Strength Bar
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_S_Bar + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Status_SetValue(lObjId, pstContents->aulStrength[ulIndex]);

	// Strength Value
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_S_Value + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_SetText(lObjId, (HCHAR*)pstContents->szStrengthPer[ulIndex]);

	// Quality Bar
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_Q_Bar + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Status_SetValue(lObjId, pstContents->aulQuality[ulIndex]);

	// Quality Value
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_Q_Value + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_SetText(lObjId, (HCHAR*)pstContents->szQualityPer[ulIndex]);

	// BER Bar
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_B_Bar + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Status_SetValue(lObjId, pstContents->aulBER[ulIndex]);

	// Quality Value
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_B_Value + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_SetText(lObjId, (HCHAR*)pstContents->szBERPer[ulIndex]);

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static void		nx_menuSignalTestAMX_UpdateCurrentTime(Nx_MenuSignalAMXSetContent_t *pstContents)
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


static ONDK_Result_t		nx_menuSignalTestAMX_CheckAntennaConnection(Nx_MenuSignalAMXSetContent_t *pstContents)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT8					*szStringId = NULL;
	DxAntennaConnection_e 	eAntConnetion = eDxANTCONN_ONECABLE;

	NX_ANTENNA_GetConnectionType(&eAntConnetion);

	if (pstContents->eAntType == eNxSat_AntType_SCD)
	{
		szStringId = (HUINT8*)RES_MESG_4607_ID;
	}
	else
	{
		switch (eAntConnetion)
		{
			case eDxANTCONN_ONECABLE:
				szStringId = (HUINT8*)RES_MESG_4605_ID;
				break;
			case eDxANTCONN_TWODIFF:
			case eDxANTCONN_TWOSAME:
				szStringId = (HUINT8*)RES_MESG_4606_ID;
				break;
			default:
				szStringId = NULL;
				break;
		}
	}

	if (szStringId != NULL)
	{
		nx_menuSignalTestAMX_CreateResponseDialouge((HUINT8*)ONDK_GetString((HINT8*)szStringId));
	}
#endif
	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSignalTestAMX_LoadSettingsItems(Nx_MenuSignalAMXSetContent_t *pstContents)
{
	HERROR		hError = ERR_FAIL;
	HBOOL 		bTunerLock = FALSE;
	HCHAR		*szTunerTitle = NULL;
	HINT32		lBERValue = 0, lAllChNum = 0, lDecimal = 0, lFloat = 0, lExp = 0;
	HUINT32		i = 0, ulTunerNum = 0;
	NX_SignalInfo_t	stSignalInfo;
#if defined(CONFIG_MW_CH_SATELLITE)
	HINT32		lValue = 0;
	NX_SAT_AntennaType_e	eAntennaType = eNxSat_AntType_LNB_Only;
#endif


#if defined(CONFIG_MW_CH_SATELLITE)
	hError = NX_ANTENNA_GetAntennaType(&lValue);
	if (hError != ERR_FAIL)
	{
		lValue = (HINT32)eNxSat_AntType_LNB_Only;
	}
	eAntennaType = (NX_SAT_AntennaType_e)lValue;
	pstContents->eAntType = eAntennaType;
#endif

	lAllChNum = NX_CHANNEL_GetChanenlCountAll();
	NX_SYSTEM_GetTunerDeviceNum(&ulTunerNum);
	pstContents->ulTunerNum = ulTunerNum;
	NX_APP_Info("- Info : Tuner Number:(%d) !! \n", pstContents->ulTunerNum);

	for (i = 0; i < pstContents->ulTunerNum && i < NX_MENU_SIGNAL_AMX_MAX_TUNER_NUM; i++)
	{
		szTunerTitle = nx_menuSignalTestAMX_GetTunerResource(i);
		HxSTD_StrNCpy((char*)pstContents->szTunerTitle[i], (HCHAR*)ONDK_GetString(szTunerTitle), (NX_TEXT_SIZE_32 - 1));

		if (lAllChNum < 1)
		{
			pstContents->aulStrength[i] = 0;
			pstContents->aulQuality[i] = 0;
			pstContents->aulBER[i] = 0;
			HxSTD_sprintf((char*)pstContents->szStrengthPer[i], "%s", " ");
			HxSTD_sprintf((char*)pstContents->szQualityPer[i], "%s", " ");
			HxSTD_sprintf((char*)pstContents->szTPInfo[i], "%s", " ");
			continue;
		}

		hError = NX_SYSTEM_GetTunerStatus(i, &bTunerLock);
		if ((hError == ERR_OK) && (bTunerLock == TRUE))
		{
			HxSTD_memset(&stSignalInfo, 0x00, sizeof(NX_SignalInfo_t));
			hError = NX_SYSTEM_GetSignalInfo(i, &stSignalInfo);
			if (hError != ERR_OK)
			{
				continue;
			}
			pstContents->aulStrength[i] = stSignalInfo.ulSignalLevel;
			pstContents->aulQuality[i] = stSignalInfo.ulSignalQuality;
			pstContents->aulBER[i] = stSignalInfo.ulSignalQuality;
			NX_PRISM_UTIL_GetMakeChannelBerValue((HINT32)stSignalInfo.fBer, &lDecimal, &lFloat, &lExp);
			HxSTD_sprintf((char*)pstContents->szStrengthPer[i], "%u%%", stSignalInfo.ulSignalLevel);
			HxSTD_sprintf((char*)pstContents->szQualityPer[i], "%u%%", stSignalInfo.ulSignalQuality);
			HxSTD_sprintf((char*)pstContents->szBERPer[i], "%d.%dE-%d", lDecimal, lFloat, lExp);

			HxSTD_memset(pstContents->pstTuneParam, 0x00, sizeof(DxTuneParam_t));
			hError = NX_SYSTEM_GetTuneParam(i, pstContents->pstTuneParam);
			if (hError != ERR_OK)
			{
				continue;
			}

			NX_PRISM_UTIL_MakeTpStringByTuneParam(pstContents->pstTuneParam, pstContents->szTPInfo[i]);
		}
		else
		{
			HxSTD_memset(&stSignalInfo, 0x00, sizeof(NX_SignalInfo_t));
			pstContents->aulStrength[i] = stSignalInfo.ulSignalLevel;
			pstContents->aulQuality[i] = stSignalInfo.ulSignalQuality;
			lBERValue = (HINT32)(stSignalInfo.fBer);
			lBERValue = (HINT32)(stSignalInfo.fBer);
			if (lBERValue > 0)
			{
				lBERValue = lBERValue/100;
			}
			pstContents->aulBER[i] = (HUINT32)(lBERValue);
			HxSTD_sprintf((char*)pstContents->szStrengthPer[i], "%u%%", stSignalInfo.ulSignalLevel);
			HxSTD_sprintf((char*)pstContents->szQualityPer[i], "%u%%", stSignalInfo.ulSignalQuality);
			HxSTD_sprintf((char*)pstContents->szBERPer[i], "%u%%", lBERValue);

			HxSTD_memset(pstContents->pstTuneParam, 0x00, sizeof(DxTuneParam_t));
			hError = NX_SYSTEM_GetTuneParam(i, pstContents->pstTuneParam);
			if (hError != ERR_OK)
			{
				continue;
			}

			NX_PRISM_UTIL_MakeTpStringByTuneParam(pstContents->pstTuneParam, pstContents->szTPInfo[i]);
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSignalTestAMX_InitSettingsItems(Nx_MenuSignalAMXSetContent_t *pstContents)
{
	ONDK_Result_t	eResult = ONDK_RET_OK;

	if (pstContents == NULL)
	{
		NX_APP_Error("Error!!! pstContents == NULL !! \n");
		ONDK_GWM_APP_Destroy(0);
		return ONDK_RET_INITFAIL;
	}

	/* Init the sub items */
	eResult = NX_PRISM_AppContents_InitSubitemArray((void *)pstContents);
	if (eResult != ONDK_RET_OK)
	{
		NX_APP_Error("Error!!! NX_PRISM_AppContents_InitSubitemArray() fail!! \n");
		ONDK_GWM_APP_Destroy(0);
		return eResult;
	}

	// Make Title
	HxSTD_StrNCpy((char*)pstContents->ucTitle, (char*)ONDK_GetString(RES_SIGNAL_TEST_01_ID), MAX_ELEMENT_TEXT_LENGTH);

	pstContents->pstTuneParam = (DxTuneParam_t*)NX_APP_Calloc(sizeof(DxTuneParam_t));
	if (pstContents->pstTuneParam == NULL)
	{
		NX_APP_Error("Error!!! NX_APP_Calloc() Fail!! \n");
		ONDK_GWM_APP_Destroy(0);
		return	ONDK_RET_NOLOCALMEMORY;
	}

	return	ONDK_RET_OK;
}


#if defined(CONFIG_MW_CH_SATELLITE)
static ONDK_Result_t 	nx_menuSignalTestAMX_DrawForSatellite(Nx_MenuSignalAMXSetContent_t *pstContents)
{
	HINT32			lObjId = -1;
	ONDK_Rect_t     stRect = { 0, 0, 0, 0 };

#if defined(CONFIG_MW_CH_SAT_1TUNER)
	// Draw Done Button
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(eNxSettingsItem_Signal_AMX_Done);
	stRect = ONDK_Rect(NX_MENU_SIGNAL_AMX_INFO_BTN_ANTENNA_CONNECTION_X, NX_MENU_SIGNAL_AMX_INFO_BTN_ANTENNA_CONNECTION_Y,
					NX_MENU_SIGNAL_AMX_INFO_BTN_ANTENNA_CONNECTION_W, NX_MENU_SIGNAL_AMX_INFO_BTN_ANTENNA_CONNECTION_H);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)(HCHAR*)ONDK_GetString(RES_DONE_01_ID));
#else
	// Draw Antenna Connection Button
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(eNxSettingsItem_Signal_AMX_Check_Antenna);
	stRect = ONDK_Rect(NX_MENU_SIGNAL_AMX_INFO_BTN_ANTENNA_CONNECTION_X, NX_MENU_SIGNAL_AMX_INFO_BTN_ANTENNA_CONNECTION_Y,
					NX_MENU_SIGNAL_AMX_INFO_BTN_ANTENNA_CONNECTION_W, NX_MENU_SIGNAL_AMX_INFO_BTN_ANTENNA_CONNECTION_H);
	ONDK_GWM_COM_Button_Create(lObjId, &stRect, (HCHAR*)(HCHAR*)ONDK_GetString(RES_CHECK_ANTENNA_CONNECTION_ID));
#endif	// end of defined(CONFIG_MW_CH_SAT_1TUNER)

	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawButton_B1);
	ONDK_GWM_Obj_SetFocus(lObjId);

	return	ONDK_RET_OK;
}
#endif


static ONDK_Result_t 	nx_menuSignalTestAMX_ReDrawSignalInfo(Nx_MenuSignalAMXSetContent_t *pstContents)
{
	HUINT32		i = 0;

	nx_menuSignalTestAMX_LoadSettingsItems(pstContents);
	// Draw Signal Info
	for (i = 0; i < pstContents->ulTunerNum && i < NX_MENU_SIGNAL_AMX_MAX_TUNER_NUM; i++)
	{
		nx_menuSignalTestAMX_UpdateSignalInfo(pstContents, i);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_menuSignalTestAMX_DrawSignalInfo(Nx_MenuSignalAMXSetContent_t *pstContents, HUINT32 ulIndex)
{
	HINT32			lObjId = -1, lItemidx = -1, y = 0;
	ONDK_Rect_t     stInfoRect = { 0, 0, 0, 0 }, stRect = { 0, 0, 0, 0 };


	// Info Box
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_InfoBox + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	y = NX_MENU_SIGNAL_AMX_INFO_BOX_Y + (ulIndex * NX_MENU_SIGNAL_AMX_INFO_BOX_H) + (ulIndex * NX_MENU_SIGNAL_AMX_INFO_BOX_MG);
	stInfoRect = ONDK_Rect(NX_MENU_SIGNAL_AMX_INFO_BOX_X, y, NX_MENU_SIGNAL_AMX_INFO_BOX_W, NX_MENU_SIGNAL_AMX_INFO_BOX_H);
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Rect_Create(lObjId, &stInfoRect, 0);
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawFrameBox_A);

	// Tuner Title
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_TunerTitle + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	stRect = ONDK_Rect(stInfoRect.x + NX_MENU_SIGNAL_AMX_INFO_TUNER_TEXT_X, stInfoRect.y + NX_MENU_SIGNAL_AMX_INFO_TUNER_TEXT_Y,
						NX_MENU_SIGNAL_AMX_INFO_TUNER_TEXT_W, NX_MENU_SIGNAL_AMX_INFO_TUNER_TEXT_H);
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR *)pstContents->szTunerTitle[ulIndex]);
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemBold);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Whistle_100), COL(C_T_Bang_100));

	// Tuner Info
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_TunerInfo + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	stRect = ONDK_Rect(stInfoRect.x + NX_MENU_SIGNAL_AMX_INFO_TP_TEXT_X, stInfoRect.y + NX_MENU_SIGNAL_AMX_INFO_TP_TEXT_Y,
						NX_MENU_SIGNAL_AMX_INFO_TP_TEXT_W, NX_MENU_SIGNAL_AMX_INFO_TP_TEXT_H);
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR *)pstContents->szTPInfo[ulIndex]);
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_20);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Gong03), COL(C_T_Bang_100));

	// Strength Title
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_S_Title + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	stRect = ONDK_Rect(stInfoRect.x + NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_X, stInfoRect.y + NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_Y,
						NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_W, NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TITLE_H);
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR *)ONDK_GetString(RES_STRENGTH_ID));
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Orgel01), COL(C_T_Bang_100));

	// Strength Bar
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_S_Bar + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	stRect = ONDK_Rect(stInfoRect.x + NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_X, stInfoRect.y + NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_Y,
						NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_W, NX_MENU_SIGNAL_AMX_INFO_STRENGTH_BAR_H);
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Status_Create(lObjId, &stRect, 100, 0);
	ONDK_GWM_COM_Status_SetColor(lObjId, COL(C_G_Whistle_20), COL(C_G_Orgel02));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawStatusBarB1_B2_B3);
	ONDK_GWM_COM_Status_SetValue(lObjId, pstContents->aulStrength[ulIndex]);

	// Strength Value
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_S_Value + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	stRect = ONDK_Rect(stInfoRect.x + NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_X, stInfoRect.y + NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_Y,
						NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_W, NX_MENU_SIGNAL_AMX_INFO_STRENGTH_TEXT_H);
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR*)pstContents->szStrengthPer[ulIndex]);
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Orgel01), COL(C_T_Bang_100));

	// Quality Title
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_Q_Title + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	stRect = ONDK_Rect(stInfoRect.x + NX_MENU_SIGNAL_AMX_INFO_QUALITY_TITLE_X, stInfoRect.y + NX_MENU_SIGNAL_AMX_INFO_QUALITY_TITLE_Y,
						NX_MENU_SIGNAL_AMX_INFO_QUALITY_TITLE_W, NX_MENU_SIGNAL_AMX_INFO_QUALITY_TITLE_H);
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR *)ONDK_GetString(RES_QUALITY_ID));
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_T_Groove01), COL(C_T_Bang_100));

	// Quality Bar
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_Q_Bar + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	stRect = ONDK_Rect(stInfoRect.x + NX_MENU_SIGNAL_AMX_INFO_QUALITY_BAR_X, stInfoRect.y + NX_MENU_SIGNAL_AMX_INFO_QUALITY_BAR_Y,
						NX_MENU_SIGNAL_AMX_INFO_QUALITY_BAR_W, NX_MENU_SIGNAL_AMX_INFO_QUALITY_BAR_H);
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Status_Create(lObjId, &stRect, 100, 0);
	ONDK_GWM_COM_Status_SetColor(lObjId, COL(C_G_Whistle_20), COL(C_G_Groove01));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawStatusBarB1_B2_B3);
	ONDK_GWM_COM_Status_SetValue(lObjId, pstContents->aulQuality[ulIndex]);

	// Quality Value
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_Q_Value + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	stRect = ONDK_Rect(stInfoRect.x + NX_MENU_SIGNAL_AMX_INFO_QUALITY_TEXT_X, stInfoRect.y + NX_MENU_SIGNAL_AMX_INFO_QUALITY_TEXT_Y,
						NX_MENU_SIGNAL_AMX_INFO_QUALITY_TEXT_W, NX_MENU_SIGNAL_AMX_INFO_QUALITY_TEXT_H);
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR*)pstContents->szQualityPer[ulIndex]);
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_G_Groove01), COL(C_T_Bang_100));

	// BER Title
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_B_Title + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	stRect = ONDK_Rect(stInfoRect.x + NX_MENU_SIGNAL_AMX_INFO_BER_TITLE_X, stInfoRect.y + NX_MENU_SIGNAL_AMX_INFO_BER_TITLE_Y,
						NX_MENU_SIGNAL_AMX_INFO_BER_TITLE_W, NX_MENU_SIGNAL_AMX_INFO_BER_TITLE_H);
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR *)ONDK_GetString(RES_BER_03_ID));
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_LEFT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_G_Rythme01), COL(C_T_Bang_100));

	// BER Bar
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_B_Bar + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	stRect = ONDK_Rect(stInfoRect.x + NX_MENU_SIGNAL_AMX_INFO_BER_BAR_X, stInfoRect.y + NX_MENU_SIGNAL_AMX_INFO_BER_BAR_Y,
						NX_MENU_SIGNAL_AMX_INFO_BER_BAR_W, NX_MENU_SIGNAL_AMX_INFO_BER_BAR_H);
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Status_Create(lObjId, &stRect, 100, 0);
	ONDK_GWM_COM_Status_SetColor(lObjId, COL(C_G_Whistle_20), COL(C_G_Rythme01));
	ONDK_GWM_APP_SetObjectDrawMethod(lObjId, NX_PRISM_COMP_DrawStatusBarB1_B2_B3);
	ONDK_GWM_COM_Status_SetValue(lObjId, pstContents->aulBER[ulIndex]);

	// BER Value
	lItemidx = (eNxSettingsItem_Signal_AMX_T1_B_Value + (ulIndex * eNxSettingsItem_Signal_AMX_ItemMax));
	stRect = ONDK_Rect(stInfoRect.x + NX_MENU_SIGNAL_AMX_INFO_BER_TEXT_X, stInfoRect.y + NX_MENU_SIGNAL_AMX_INFO_BER_TEXT_Y,
						NX_MENU_SIGNAL_AMX_INFO_BER_TEXT_W, NX_MENU_SIGNAL_AMX_INFO_BER_TEXT_H);
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(lItemidx);
	ONDK_GWM_COM_Text_Create(lObjId, &stRect, (HCHAR*)pstContents->szBERPer[ulIndex]);
	ONDK_GWM_COM_Text_SetFont(lObjId, eFont_SystemNormal);
	ONDK_GWM_COM_Text_SetSize(lObjId, NX_PRISM_FONT_SIZE_18);
	ONDK_GWM_COM_Text_SetAlign(lObjId, TEXT_ALIGN_RIGHT);
	ONDK_GWM_COM_Text_ChangeColor(lObjId, COL(C_G_Rythme01), COL(C_T_Bang_100));

	NX_COMMON_NOT_USED_PARAM(pstContents);

	return	ONDK_RET_OK;
}


static ONDK_Result_t 	nx_menuSignalTestAMX_DrawDefaultUI(Nx_MenuSignalAMXSetContent_t *pstContents)
{
	HUINT32			i = 0;


	// Draw - Background - Black_80
	NX_PRISM_COMP_Settings_DrawBackground();

	// Draw - Title
	NX_PRISM_COMP_Settings_DrawTitle(pstContents->ucTitle);

	// Draw - Time
	nx_menuSignalTestAMX_UpdateCurrentTime(pstContents);
	NX_PRISM_COMP_Settings_DrawCurrentDateTime(pstContents->szDate, pstContents->szTime);

	NX_PRISM_COMP_Settings_DrawTitleLine();

	// Draw Signal Info
	for (i = 0; i < pstContents->ulTunerNum && i < NX_MENU_SIGNAL_AMX_MAX_TUNER_NUM; i++)
	{
		nx_menuSignalTestAMX_DrawSignalInfo(pstContents, i);
	}

#if defined(CONFIG_MW_CH_SATELLITE)
	nx_menuSignalTestAMX_DrawForSatellite(pstContents);
#endif

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSignalTestAMX_MsgGwmCreate(Nx_MenuSignalAMXSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lObjId = -1, lAllChNum = 0;
	HUINT8		*pszPanelStr = NULL;

	ONDK_GWM_APP_SetAppArea(NX_PRISM_SETTINGS_WINDOW_X, NX_PRISM_SETTINGS_WINDOW_Y, NX_PRISM_SETTINGS_WINDOW_WIDTH, NX_PRISM_SETTINGS_WINDOW_HEIGHT);

	// Init Menu Item
	nx_menuSignalTestAMX_InitSettingsItems(pstContents);
	nx_menuSignalTestAMX_LoadSettingsItems(pstContents);
	pstContents->lPrevCurIdx = p3;

	// Draw Menu Item
	nx_menuSignalTestAMX_DrawDefaultUI(pstContents);

	ONDK_GWM_SetTimer(NX_SETTING_TIMEOUT_ID, NX_SETTING_TIMEOUT);
	ONDK_GWM_SetTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID, NX_SETTING_TIME_UPDATE_TIME);

	lAllChNum = NX_CHANNEL_GetChanenlCountAll();
	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(eNxSettingsItem_Signal_AMX_UpdateId);
	if ((lObjId > 0) && (lAllChNum > 0))
	{
		ONDK_GWM_SetTimer(lObjId, NX_MENU_SIGNAL_AMX_INFO_UPDATE_TIME);
	}

	pszPanelStr = NX_PRISM_SPEC_GetPanelString(eNX_PANEL_STR_Settings);
	if (pszPanelStr == NULL)
	{
		NX_APP_Error("Error!! NX_PRISM_SPEC_GetPanelString() - eNX_PANEL_STR_Settings !!!\n");
	}
	NX_PANEL_SetString(pszPanelStr, eDxPANEL_ALIGN_CENTER, FALSE);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSignalTestAMX_MsgGwmDestroy(Nx_MenuSignalAMXSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32	lObjId = -1;

	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(eNxSettingsItem_Signal_AMX_UpdateId);

	ONDK_GWM_KillTimer(NX_SETTING_TIMEOUT_ID);
	ONDK_GWM_KillTimer(NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID);
	ONDK_GWM_KillTimer(lObjId);

	if (pstContents == NULL)
	{
		return ONDK_RET_FAIL;
	}

	if (pstContents->pstTuneParam)
	{
		NX_APP_Free(pstContents->pstTuneParam);
		pstContents->pstTuneParam = NULL;
	}

	NX_PRISM_AppContents_FreeSubitemArray ((void *)pstContents);
	HxSTD_memset(pstContents, 0x00, sizeof(Nx_MenuSignalAMXSetContent_t));

	NX_PANEL_ClearString();

	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSignalTestAMX_MsgGwmHapiMsg(Nx_MenuSignalAMXSetContent_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(pstContents);
	NX_COMMON_NOT_USED_PARAM(handle);
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}



static ONDK_Result_t	nx_menuSignalTestAMX_MsgGwmKeyDown(Nx_MenuSignalAMXSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32			lKeyId = p1;
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
		case KEY_ARROWLEFT:
			nx_menuSignalTestAMX_ActionGoPrevMenu(pstContents);
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
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;

		case KEY_STANDBY:
			ONDK_GWM_APP_Destroy(0);
			return ONDK_MESSAGE_PASS;

		default:
			NX_APP_Print("nx_menuGeneral_MsgGwmKeyDown() Not used lKeyId(0x%x)\n", lKeyId);
			return	ONDK_MESSAGE_PASS;
	}

	return	ONDK_RET_OK;
}



static ONDK_Result_t	nx_menuSignalTestAMX_MsgGwmClicked(Nx_MenuSignalAMXSetContent_t *pstContents, HINT32 lObjId)
{
	HINT32		lItemIdx = -1;

	lItemIdx = nx_menuSignalTestAMX_ConvertObjIdToItemIdx(lObjId);
	if (lItemIdx == eNxSettingsItem_Signal_AMX_Check_Antenna)
	{
		nx_menuSignalTestAMX_CheckAntennaConnection(pstContents);
		return	ONDK_MESSAGE_BREAK;
	}
	else if (lItemIdx == eNxSettingsItem_Signal_AMX_Done)
	{
		nx_menuSignalTestAMX_ActionGoPrevMenu(pstContents);
		return	ONDK_MESSAGE_BREAK;
	}
	else
	{
		NX_APP_Error("Error!!! Unknow button lObjId:(0x%x), lItemIdx:(%d) \n", lObjId, lItemIdx);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_menuSignalTestAMX_MsgGwmTimer(Nx_MenuSignalAMXSetContent_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HINT32		lObjId = -1;

	lObjId = nx_menuSignalTestAMX_ConvertItemIdxToObjId(eNxSettingsItem_Signal_AMX_UpdateId);

	if (p1 == NX_SETTING_TIMEOUT_ID)
	{
		ONDK_GWM_APP_Destroy(0);
	}
	else if (p1 == NX_PRISM_UI_SET_CUR_TIME_UPDATE_ID)
	{
		nx_menuSignalTestAMX_UpdateCurrentTime(pstContents);
		NX_PRISM_COMP_Settings_DrawUpdateCurrentDateTime(pstContents->szDate, pstContents->szTime);
	}
	else if (p1 == lObjId)
	{
		nx_menuSignalTestAMX_ReDrawSignalInfo(pstContents);
	}

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MENU_SignalTestAMX_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_OK;
	Nx_MenuSignalAMXSetContent_t	*pstContents = NULL;

	pstContents = nx_menuSignalTestAMX_GetContents();

	switch (nMessage)
	{
		case MSG_GWM_CREATE:
			eResult = nx_menuSignalTestAMX_MsgGwmCreate(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			eResult = nx_menuSignalTestAMX_MsgGwmKeyDown(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			eResult = nx_menuSignalTestAMX_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_menuSignalTestAMX_MsgGwmHapiMsg(pstContents, handle, p1, p2, p3);
			break;
		case MSG_GWM_CLICKED:
			NX_APP_Message("MSG_GWM_CLICKED!\n");
			eResult = nx_menuSignalTestAMX_MsgGwmClicked(pstContents, p1);
			break;
		case MSG_GWM_TIMER:
			NX_APP_Message("MSG_GWM_TIMER!\n");
			eResult = nx_menuSignalTestAMX_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_APP_SYSTEM_SVC_CHANGED:
			NX_APP_Message("MSG_APP_SYSTEM_SVC_CHANGED!\n");
			eResult = nx_menuSignalTestAMX_ReDrawSignalInfo(pstContents);
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

	return ONDK_GWM_ProcessMessageDefault(nMessage, handle, p1, p2, p3);
}


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/

