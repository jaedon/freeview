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
 * @file	  		nx_prism_mgr_init.c
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


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum {
	eNxPrismMgrDlg_AutoPowerDown,
	eNxPrismMgrDlg_MAX
} NxPrismMgrDlg_e;

/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NXUI_DialogInstance_t				s_stAutoPowerDown_dInstance;
static HUINT8								s_strBuf[NX_TEXT_SIZE_128];

/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_prism_MgrInit_AutoPowerDownDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_prism_MgrInit_CreateAutoPowerDownDialog(void);
static ONDK_Result_t	nx_prism_MgrInit_DestroyAutoPowerDownDialog(void);
static ONDK_Result_t	nx_prism_MgrInit_Init(void);
static ONDK_Result_t	nx_prism_MgrInit_CreateMgrProc(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_prism_MgrInit_keyDownProc (HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

static ONDK_Result_t	nx_prism_MgrInit_MsgGwmCreate(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_prism_MgrInit_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_prism_MgrInit_MsgGwmHapiMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_prism_MgrInit_MsgAppDlgClicked(HINT32 ulDlgWhoseId);
static ONDK_Result_t	nx_prism_MgrInit_MsgAppDlgTimeout(HINT32 ulDlgWhoseId);
static void 			nx_prism_MgrInit_TempSettingValue(void);
static ONDK_Result_t	nx_prism_MgrInit_CheckFirstAction(void);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_prism_MgrInit_ResponseDialogProc(HINT32 lMessage, Handle_t hHandle, HINT32 lParam1, HINT32 lParam2, HINT32 lParam3)
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

ONDK_Result_t	nx_prism_MgrInit_DestroyResponseDialouge(void)
{
	ONDK_GWM_App_t	pGwmApp = NULL;

	pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)nx_prism_MgrInit_ResponseDialogProc);
	if (pGwmApp != NULL)
	{
		return ONDK_GWM_APP_Destroy(nx_prism_MgrInit_ResponseDialogProc);
	}
	else
	{
		return	ONDK_RET_OK;
	}
}

static void		nx_prism_MgrInit_CreateResponseDialouge(HUINT8* pszString, HINT32 nTimeout)
{
	if (pszString == NULL)
	{
		NX_APP_Error("Error!!! Pls check pszString was NULL!! \n");
		return;
	}

	nx_prism_MgrInit_DestroyResponseDialouge();

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_prism_MgrInit_ResponseDialogProc", APP_MULTI_CUSTOMCODE_PRIORITY,
						nx_prism_MgrInit_ResponseDialogProc, 0, (HINT32)pszString, 0, nTimeout);
}

static void nx_prism_MgrInit_GoStandy(void)
{
	ONDK_GWM_APP_Destroy(0); // destroy self
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_STANDBY_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_STANDBY_Proc, 0, 0, 0, 0);
}

static ONDK_Result_t	nx_prism_MgrInit_AutoPowerDownDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc(&s_stAutoPowerDown_dInstance, message, hAction, p1, p2, p3);
}


static void	nx_prism_MgrInit_CreateAutoPowerDownDialog(void)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = &s_stAutoPowerDown_dInstance;
	HxSTD_memset (pstDialog, 0, sizeof(NXUI_DialogInstance_t));

	pstDialog->ulDlgWhoseId = eNxPrismMgrDlg_AutoPowerDown;
	ulDlgAttr = (NX_DLG_OK|NX_DLG_TIMEOUT_2MIN|NX_DLG_TYPE_FOCUS_NO|NX_DLG_TYPE_MESSAGE);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, ONDK_GetString(RES_MESG_4532_ID));

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_prism_MgrInit_AutoPowerDownDialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_prism_MgrInit_AutoPowerDownDialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));
}


static ONDK_Result_t	nx_prism_MgrInit_DestroyAutoPowerDownDialog(void)
{
	return	ONDK_GWM_APP_Destroy(nx_prism_MgrInit_AutoPowerDownDialogProc);
}


static void		nx_prism_MgrInit_TempSettingValue(void)
{
	HINT32				i = 0, lRealAntNum = 0;
	HERROR				hError = ERR_FAIL;
	DxAntInfo_t 		stAntInfo;

	// Load Antenna Info
	for (i = 0; i < eNxSat_ANTENAA_SLOT_MAX; i++)
	{
		HxSTD_memset(&stAntInfo, 0x00, sizeof(DxAntInfo_t));
		hError = NX_ANTENNA_GetInfo(eNxSat_ANTENAA_SLOT1 + i, &stAntInfo);
		if (hError == ERR_OK)
		{
			lRealAntNum++;
		}
	}
	if (lRealAntNum < 1)
	{
		HxSTD_memset(&stAntInfo, 0x00, sizeof(DxAntInfo_t));
		NX_ANTENNA_MakeDefaultAntennaInfo(eDxANT_TYPE_LNB_ONLY, &stAntInfo);
		stAntInfo.uid = eNxSat_ANTENAA_SLOT1;

		NX_ANTENNA_SetInfo(eNxSat_ANTENAA_SLOT1, &stAntInfo);
	}
}


static ONDK_Result_t	nx_prism_MgrInit_Init(void)
{

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_prism_MgrInit_CheckFirstAction(void)
{
	ONDK_GWM_PostMessage(NULL, MSG_APP_CHECK_FIRST_ACTION, 0, 0, 0, 0);
	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_prism_MgrInit_CreateMgrProc(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	// message
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_SYSMESSAGE_Proc", APP_BACKGROUND_OSD_PRIORITY, NX_PRISM_SYSMESSAGE_Proc, handle, p1, p2, p3);

	// live
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_LIVE_Proc", APP_BACKGROUND_OSD_PRIORITY, NX_PRISM_LIVE_Proc, handle, p1, p2, p3);

	// Device Manager - HDD, USB, Mobile, els if exteral Device
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_SYSTEM_DEVICE_Proc", APP_BACKGROUND_OSD_PRIORITY, NX_PRISM_SYSTEM_DEVICE_Proc, handle, p1, p2, p3);

	// Play Manager
#if defined(CONFIG_MW_MM_PVR) || defined(CONFIG_MW_MM_MEDIA)
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_PLAYMGR_Proc", APP_BACKGROUND_OSD_PRIORITY, NX_PRISM_PLAYMGR_Proc, handle, p1, p2, p3);
#endif

	// CAS - Nagra
#if defined(CONFIG_MW_CAS_NAGRA)
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_CASMESSAGE_NA_Proc", APP_BACKGROUND_OSD_PRIORITY, NX_PRISM_CASMESSAGE_NA_Proc, handle, p1, p2, p3);
#endif

#if defined(CONFIG_OP_SHAHID_APPS)
	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_SHAHID_BG_Proc", APP_BACKGROUND_OSD_PRIORITY, NX_PRISM_SHAHID_BG_Proc, handle, p1, p2, p3);
#endif


	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_prism_MgrInit_DestroyMgrProc( HINT32 p1, HINT32 p2, HINT32 p3)
{
	// message
	ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_SYSMESSAGE_Proc);

	// live
	ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_LIVE_Proc);

	// Device Manager - HDD, USB, Mobile, els if exteral Device
	ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_SYSTEM_DEVICE_Proc);

	// Play Manager
#if defined(CONFIG_MW_MM_PVR) || defined(CONFIG_MW_MM_MEDIA)
	ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_PLAYMGR_Proc);
#endif

#if defined(CONFIG_MW_CAS_NAGRA)
	ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_CASMESSAGE_NA_Proc);
#endif

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_prism_MgrInit_MsgGwmCreate(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	nx_prism_MgrInit_Init();
	nx_prism_MgrInit_CreateMgrProc(handle, p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_prism_MgrInit_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	nx_prism_MgrInit_DestroyMgrProc(p1, p2, p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_prism_MgrInit_MsgGwmHapiMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hError = ERR_FAIL;
	HAPIMSG_e	eMsg	= (HAPIMSG_e)handle;
	HBOOL 		eAutomaticPowerDown = TRUE;

	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	switch(eMsg)
	{
	case eHMSG_USER_ACTION_IDLE_NOTIFY:
		hError = NX_SYSTEM_GetAutomaticPowerdownOnOff(&eAutomaticPowerDown);
		if ((hError == ERR_OK) && (eAutomaticPowerDown == TRUE))
		{
			nx_prism_MgrInit_CreateAutoPowerDownDialog();
		}
		break;
	default:
		return ONDK_MESSAGE_PASS;
		break;
	}

	return	ONDK_MESSAGE_PASS;
}

static ONDK_Result_t	nx_prism_MgrInit_MsgGwmApkMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	GWM_APK_MSG_Type_e			eAPK_Type = (GWM_APK_MSG_Type_e)handle;

	switch(eAPK_Type)
	{
		default:
			break;
	}

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_prism_MgrInit_MsgAppDlgClicked(HINT32 ulDlgWhoseId)
{
	switch(ulDlgWhoseId)
	{
		case eNxPrismMgrDlg_AutoPowerDown:
			nx_prism_MgrInit_DestroyAutoPowerDownDialog();
			return ONDK_MESSAGE_CONSUMED;
		default:
			NX_APP_Error("[%s:%d] Invalid Dialog Id : [%d]\n", __FUNCTION__, __LINE__, ulDlgWhoseId);
			return	ONDK_RET_FAIL;
	}
}


static ONDK_Result_t	nx_prism_MgrInit_MsgAppDlgTimeout(HINT32 ulDlgWhoseId)
{
	switch(ulDlgWhoseId)
	{
		case eNxPrismMgrDlg_AutoPowerDown:
			nx_prism_MgrInit_DestroyAutoPowerDownDialog();
			nx_prism_MgrInit_GoStandy();
			return ONDK_MESSAGE_CONSUMED;
		default:
			NX_APP_Error("[%s:%d] Invalid Dialog Id : [%d]\n", __FUNCTION__, __LINE__, ulDlgWhoseId);
			return	ONDK_RET_FAIL;
	}
}

#if defined(CONFIG_DEBUG)
#include "ondk_debug.h"
#endif

static ONDK_Result_t	nx_prism_MgrInit_keyDownProc (HINT32 message, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32	ulCustomCode = 1;

	NX_PRISM_UTIL_KeyConv_UiCommon(&p1);

	switch (p1)
	{
		case KEY_STANDBY:
			nx_prism_MgrInit_GoStandy();
			return ONDK_MESSAGE_PASS;
		case KEY_MENU:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_HomeMenu_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_HomeMenu_Proc, handle, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case KEY_MUTE:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_MUTE_Proc", APP_INDEPENDENT_OSD_GRP_PRIORITY, NX_PRISM_MUTE_Proc, handle, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case KEY_WIDE:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_WIDE_Proc", APP_INDEPENDENT_OSD_GRP_PRIORITY, NX_PRISM_WIDE_Proc, handle, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case KEY_VOLUMEUP:
		case KEY_VOLUMEDOWN:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_SYSTEMVolume_Proc", APP_INDEPENDENT_OSD_GRP_PRIORITY, NX_PRISM_SYSTEMVolume_Proc, handle, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case KEY_GUIDE:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_GUIDE_Proc", APP_DEFAULT_PRIORITY | UISTATE_EPG, NX_PRISM_GUIDE_Proc, handle, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case KEY_AUDIO:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_AUDIO_UI_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_AUDIO_UI_Proc, handle, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case KEY_SUBTITLE:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_SUBTTL_UI_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_SUBTTL_UI_Proc, handle, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case KEY_OPT:
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "NX_PRISM_OPTION_Proc", APP_DEFAULT_POPUP_PRIORITY, NX_PRISM_OPTION_Proc, handle, p1, p2, p3);
			return ONDK_MESSAGE_BREAK;

		case KEY_CHLIST:
			break;

		case KEY_PLAY:
#if defined(CONFIG_MW_MM_PVR) || defined(CONFIG_MW_MM_MEDIA)
			NX_PRISM_PLAYMGR_PlayStart(0, 0, 0, FALSE);
#endif
			break;
		case KEY_INFO:
			break;
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
			break;

		case KEY_SLEEP:
		case KEY_VFORMAT:
		case KEY_SOURCE:
		case KEY_EXT_AV:
		case KEY_GREEN:
		case KEY_MEDIA:
		case KEY_TEXT:
			break;

			// custom key setting
		case KEY_CUSTOM_MULTI_CUSTOM:
			(void)NX_SYSTEM_GetCustomCode(&ulCustomCode);
			HxSTD_MemSet(s_strBuf, 0, NX_TEXT_SIZE_128);
			HxSTD_snprintf((char*)s_strBuf, NX_TEXT_SIZE_128, "%s %d", ONDK_GetString(RES_CUSTOMCODE_CHANGE), ulCustomCode);
			nx_prism_MgrInit_CreateResponseDialouge(s_strBuf, 24*60*60*1000); // wait forevere
			return ONDK_MESSAGE_BREAK;

		case KEY_CUSTOM_MULTI_CUSTOM_SUCCESS:
			(void)NX_SYSTEM_GetCustomCode(&ulCustomCode);
			HxSTD_MemSet(s_strBuf, 0, NX_TEXT_SIZE_128);
			HxSTD_snprintf((char*)s_strBuf, NX_TEXT_SIZE_128, "%s %d", ONDK_GetString(RES_CUSTOMCODE_CHANGE_SUCCESS), ulCustomCode);
			nx_prism_MgrInit_CreateResponseDialouge(s_strBuf, 0);
			return ONDK_MESSAGE_BREAK;

		case KEY_CUSTOM_MULTI_CUSTOM_FAIL:
			nx_prism_MgrInit_CreateResponseDialouge(ONDK_GetString(RES_CUSTOMCODE_CHANGE_FAILURE), 0);
			return ONDK_MESSAGE_BREAK;

		default:
			NX_APP_Debug("[%s:%d] key = %d \n", __FUNCTION__, __HxLINE__, p1);
			break;
	}

	// Not available icon
	switch (p1)
	{
		case KEY_BACK:
		case KEY_EXIT:
		case KEY_OK:
		case KEY_ARROWLEFT:
		case KEY_ARROWRIGHT:
		case KEY_ARROWUP:
		case KEY_ARROWDOWN:
			break;
		default:
			NX_PRISM_NotAvailable_Show();
			break;
	}

#if 0//defined(CONFIG_DEBUG)
	// test
	switch(p1)
	{
	case KEY_RED:
		ONDK_TEST_Start(NULL);
		break;
	case KEY_BLUE:
		ONDK_TEST_FONT_Start();
		break;
	case KEY_YELLOW:
		ONDK_TEST_O2ANI_Start();
		break;
	case KEY_GREEN:
		ONDK_TEST_FPS_Start();
		break;
	default:
		break;
	}
#endif

	return ONDK_RET_OK;
}

static ONDK_Result_t	nx_prism_MgrInit_MsgGwmTimer(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch(p1)
	{
		default:
			break;
	}

	return GWM_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_MgrInit_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			nx_prism_MgrInit_MsgGwmCreate(handle, p1, p2, p3);
			ONDK_GWM_APP_SetUIHideFlag(GWM_UI_Hide_Never);
			nx_prism_MgrInit_CheckFirstAction();
			break;

		case MSG_GWM_KEYDOWN:
			eResult = nx_prism_MgrInit_keyDownProc(lMessage, handle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			eResult = nx_prism_MgrInit_MsgGwmTimer(handle, p1, p2, p3);
			break;

		case MSG_GWM_HAPI_MESSAGE:
			eResult = nx_prism_MgrInit_MsgGwmHapiMsg(handle, p1, p2, p3);
			break;

		case MSG_GWM_APK_MESSAGE:
			eResult = nx_prism_MgrInit_MsgGwmApkMsg(handle, p1, p2, p3);
			break;

		case MSG_GWM_DESTROY:
			nx_prism_MgrInit_MsgGwmDestroy( p1, p2, p3);
			break;

		case MSG_APP_DLG_CLICKED:
			eResult = nx_prism_MgrInit_MsgAppDlgClicked(p2);
			break;

		case MSG_APP_DLG_TIMEOUT:
			eResult = nx_prism_MgrInit_MsgAppDlgTimeout(p2);
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

