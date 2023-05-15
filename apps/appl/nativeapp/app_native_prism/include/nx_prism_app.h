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
 * @file	  		nx_prism_app.h
 *
 * 	Description:  									\n
 *
 * @author											\n
 * @date											\n
 * @Attention		Copyright (c) 2013 HUMAX Co., Ltd. - All rights reserved.
 *
 **/
/******************************************************************************/

#ifndef __NX_PRISM_APP_H__
#define __NX_PRISM_APP_H__

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/
#include <ondk.h>
#include <ondk_type.h>
#include <ondk_gfx.h>
#include <ondk_gwm_component.h>
#include <ondk_gwm_guiobj.h>
#include <ondk_gwm_appobj.h>

#include <hlib.h>

#include <nx_prism_util.h>
#include <nx_prism_live.h>
#include <nx_prism_settings.h>
#include <nx_prism_subtitle.h>
#include <nx_prism_teletext.h>
#include <nx_prism_ui.h>
#include <nx_prism_resource.h>
#include <nx_prism_definition.h>
#include <nx_prism_component.h>
#include <nx_prism_keyboard.h>
#include <nx_prism_simplekeyboard.h>
#include <nx_prism_installwizard.h>
#include <nx_prism_menu_items.h>

/*******************************************************************/
/********************	Header Files		************************/
/*******************************************************************/


/*******************************************************************/
/********************	Extern variables	************************/
/*******************************************************************/


/*******************************************************************/
/********************	Macro Definition	 ***********************/
/*******************************************************************/


/*******************************************************************/
/********************		Typedef			************************/
/*******************************************************************/
/*
	- MSG_APP_ 로 시작하는 메시지는 application 공통으로 적용되는 convention이다.
	- MSG_모듈명_ 으로 시작하는 메시지는 특정 application이 발행하거나, 특정 application을 target으로 하는 convention이다.
	- MSG_GWM_ 으로 시작하는 메시지는 GWM이 발행한 메시지 convention이다.
*/
enum
{
	MSG_APP_UI = MSG_AP_UI_START,

	MSG_APP_CHECK_FIRST_ACTION,							/* - Description : operation 초기시 check 해야할 일들에 대한 msg */

	/* MSG_APP_XXX */
	MSG_APP_SETTING_VALUE_CHANGED,				/* - Description : Settings value changed by Menu | - Parameters Usage : handle : NA, p1 : changed value, p2 : NA, p3 : NA */
	MSG_APP_SETTING_INPUT_VALUE_CHANGED,		/* - Description : User Input value  | - Parameters Usage : handle : NA, p1 : Input value, p2 : NA, p3 : NA */
	MSG_APP_SETTING_ANTENNA_SETTING_CHANGED,	/* - Description : User Input value  | - Parameters Usage : handle : NA, p1 : Antenna Uid, p2 : Antenna Add(TRUE), Delete(FALSE), p3 : NA */
	MSG_APP_SETTING_TP_SETTING_END,				/* - Description : TP setting 에서 셋팅 값이 변경되었을 때 .- Parameters Usage :  handle : NA,  p1 : TRUE, p2 : NA, p3 : NA */

	MSG_APP_SYSTEM_DELETE_SERVICE,				/* - Description : Delete Service .- Parameters Usage :  handle : NA, p1 : NZ, p2 : NA, p3 : NA */
	MSG_APP_SYSTEM_DELETE_SERVICE_ALL,			/* - Description : Delete Service ALL .- Parameters Usage :  handle : NA, p1 : NZ, p2 : NA, p3 : NA */
	MSG_APP_SYSTEM_UPDATE_SERVICE,				/* - Description : Update Service - Parameters Usage :  handle : NA, p1 : NZ, p2 : NA, p3 : NA */
	MSG_APP_SYSTEM_RELOAD_SERVICELIST,			/* - Description : ReLoad Service List - Parameters Usage :	handle : NA, p1 : NZ, p2 : NA, p3 : NA */

												/* - Description : PIN code appl에서 PIN code가 pass되었다는 이벤트 메시지.
																Broadcating하지 말고 PIN code appl을 생성한 parent appl에만 전달되어야 함.
												   - Parameters Usage : handle : NA, p1 : TBD, p2 : PIN_InputBox_Mode_t, p3 : 이 메시지를 발행한 PIN dialog application callback(app proc) */
	MSG_APP_SYSTEM_PIN_OK,
												/* - Description : PIN code appl에서 틀린 PIN code가 입력 되었다는 이벤트 메시지.
																Broadcating하지 말고 PIN code appl을 생성한 parent appl에만 전달되어야 함.
												   - Parameters Usage : handle : NA, p1 : TBD, p2 : PIN_InputBox_Mode_t, p3 : 이 메시지를 발행한 PIN dialog application callback(app proc) */
	MSG_APP_SYSTEM_PIN_DENIED,
												/* - Description : PIN Code입력이 fail되었다는 메세지
												- Parameters Usage :  handle : NA,  p1 : TBD,  p2 :TBD,  p3 : TBD */

	MSG_APP_SYSTEM_START_SVC,					/* - Description : Start Service to Live TV- Parameters Usage : handle : N.A. p1 : service handle. CURRENT_SVC_HANDLE로 지정하면 마지막 보던 채널로 튜닝함.
													p2 : service list group 지정. CURRENT_SVC_LIST_GROUP로 지정하면 마지막 보던 group으로 지정.
													p3 : Extra Filger tunning attribute를 지정함. */
	MGS_APP_SYSTEM_START_LASTSVC,				/* - Description : live 의 last channel 로 try tune 을 하고 싶은경우. */
	MSG_APP_SYSTEM_SVC_CHANGED,					/* - Description : Channel changed */
	MSG_APP_SYSTEM_SVC_STOPPED,					/* - Description : Channel stopped */

	MSG_APP_SYSTEM_CAS_STATE_UPDATE,			/* - Description : CAS State Update - Parameters Usage : handle : NA, p1 : CAS State, p2 : NA, p3 : NA */

	MSG_APP_CHANNEL_TUNE_LOCKED,				/* - Description : Service Lock OK .- Parameters Usage :  handle : NA, p1 : View ID, p2 : Media Type, p3 : NA */
	MSG_APP_CHANNEL_TUNE_NOSIGNAL,
	MSG_APP_CHANNEL_LIVE_VIEWSTATECHANGED,
	MSG_APP_CHANNEL_LIVE_LOCKSTATECHANGED,
	MSG_APP_CHANNEL_PINCTRLCHANGED,

//	MSG_APP_SYSTEM_POWER_ON_BYTIMER,			/* - Description : Action Power on Time by schedule */
//	MSG_APP_SYSTEM_POWER_OFF_BYTIMER,			/* - Description : Action Power Off Time by schedule */

	// TODO: Add Here

	/* MSG_APP_[MODULE]_XXX */
	// TODO: Add Here
	MSG_APP_DLG_INPUTED_KEYDOWN,				/* - Description : Dialog | - Parameters Usage :  handle : NA, p1 : KEY_DOWN value, p2 : attribute 2, p3 : sender procs */
	MSG_APP_DLG_CLICKED,
	MSG_APP_DLG_CANCELED,
	MSG_APP_DLG_TIMEOUT,
	MSG_APP_DLG_UPDATE_ANIMATE_ICON,

	MSG_APP_POPUP_LIST_INPUTED_KEYDOWN,			/* - Description : POPUP LIST | - Parameters Usage :  handle : NA, p1 : KEY_DOWN value, p2 : NA, p3 : sender procs */
	MSG_APP_POPUP_LIST_DESTROYED,				/* - Description : POPUP LIST | - Parameters Usage :  handle : NA, p1 : KEY_DOWN value, p2 : NA, p3 : sender procs */
	MSG_APP_POPUP_OPT_VALUE_CHANGED,			/* - Description : POPUP LIST | - Parameters Usage :  handle : NA, p1 : TBD , p2 : TBD, p3 : NA */

	MSG_APP_KEYBOARD_RESULT,					/* - Keyboard 의 OK 등으로 , 데이터를 퍼가라고 알리기 위해. */
	MSG_APP_KEYBOARD_DESTROY_KEYBOARD,			/* - 외부 proc 에서 key board proc 을 종료하기 위해..		*/

	MSG_APP_DCN_INPUT,							/* p1: lcn, p2 : uid */
	MSG_APP_HOMEMENU_UI_LAUNCH,					/* p1: Menu type, p2 : NA, p3 : NA */
	MSG_APP_NETWORK_HIDDEN_RESULT,

	/* Media */
	MSG_APP_MEDIA_TARGET_STORAGE_SELECTED,
	MSG_APP_MEDIA_TYPE_SELECT_DLG_DESTROY,

	/* Player */
	MSG_APP_PLAYER_DO_START,
	MSG_APP_PLAYER_DO_STOP,
	MSG_APP_PLAYER_DO_SPEED_CHANGE,
	MSG_APP_PLAYER_DO_LEFTSEEK,
	MSG_APP_PLAYER_DO_RIGHTSEEK,
	MSG_APP_PLAYER_DESTROY,

	/* Play Manager */
	MSG_APP_PLAYMGR_PLAY_START,
	MSG_APP_PLAYMGR_PLAY_STOP,
	MSG_APP_PLAYMGR_PLAYINFO_UPDATED,

	/* recordings */
	MSG_APP_RECORDINGS_EXT_DESTROY,

	/* record */
	MSG_APP_RECORD_START,
	MSG_APP_RECORD_STOP,
	MSG_APP_RECORD_TSRBUFFER_COPYEND,

	MSG_APP_MAX
};

/*******************************************************************/
/********************	Global Variables		********************/
/*******************************************************************/


/*******************************************************************/
/********************	global function prototype   ****************/
/*******************************************************************/


/*******************************************************************/
#define _________NAPP_APP_Prototype________________________________
/*******************************************************************/
extern ONDK_Result_t	NX_PRISM_MgrInit_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MgrAction_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

/* Set menu Config */
extern void				NX_PRISM_MENUCONFIG_Init(void);

/* Dialogue Message */
// Status Message
extern ONDK_Result_t	NX_PRISM_DIALOGUE_StatusMessage_Destroy(void);
extern ONDK_Result_t	NX_PRISM_DIALOGUE_StatusMessage_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

// Response Message
extern ONDK_Result_t	NX_PRISM_DIALOGUE_ResponseMessage_Destroy(void);
extern ONDK_Result_t	NX_PRISM_DIALOGUE_ResponseMessage_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

// Confirm&Select Message
extern ONDK_Result_t	NX_PRISM_DIALOGUE_SetCustomButton(NXUI_DialogInstance_t *pstContents, HUINT8 *pucLeftBt, HUINT8 *pucMiddleBt, HUINT8 *pucRightBt);
extern ONDK_Result_t	NX_PRISM_DIALOGUE_SetParentApp(NXUI_DialogInstance_t *pstContents, ONDK_GWM_Callback_t parentApp);
extern ONDK_Result_t	NX_PRISM_DIALOGUE_SetDialogAttributes(NXUI_DialogInstance_t *pstContents, HUINT32 ulDlgAttr);
extern ONDK_Result_t	NX_PRISM_DIALOGUE_SetDialogTitle(NXUI_DialogInstance_t *pstContents, HUINT8 *pucDlgTitle);
extern ONDK_Result_t	NX_PRISM_DIALOGUE_SetDialogMessage(NXUI_DialogInstance_t *pstContents, HUINT8 *pucDlgMsg);
extern ONDK_Result_t 	NX_PRISM_DIALOGUE_SetDialog(NXUI_DialogInstance_t *pstContents, ONDK_GWM_Callback_t parentApp, HUINT32 ulDlgAttr, HUINT8 *pDlgTitle, HUINT8 *pDlgMsg);
extern ONDK_Result_t	NX_PRISM_DIALOGUE_SetInitContents(NXUI_DialogInstance_t *pstContents);
extern ONDK_Result_t	NX_PRISM_DIALOGUE_Message_Proc(NXUI_DialogInstance_t *pstContents, HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

// Processing Message
extern ONDK_Result_t	NX_PRISM_DIALOGUE_ProcessingMessage_Proc(NXUI_DialogInstance_t *pstContents, HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

// Multi Select Message
extern ONDK_Rect_t	 	NX_PRISM_DIALOGUE_MultiSelectMessage_GetRect(NX_DLG_MultiSelRect_t eRectType, HINT32 lMsgLineCnt, HINT32 lItemCnt);

/* System Pop-up */
extern ONDK_Result_t	NX_PRISM_NotAvailable_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_NotAvailable_Show(void);

extern void 			NX_PRISM_Dialogue_CreateNoService(void);
extern void 			NX_PRISM_Dialogue_DestroyNoService(void);

#if defined(CONFIG_MW_MM_PVR)
extern void				NX_PRISM_START_RECORD_CreateResponseDialouge(HINT32 nSvcUid, HINT32 nSessionId);
extern void				NX_PRISM_STOP_RECORD_CreateResponseDialouge(void);
#endif

/* Reservation */
extern ONDK_Result_t	NX_PRISM_DIALOGUE_RsvNoti_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_DIALOGUE_RsvConflict_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

/* System Manager */
extern ONDK_Result_t	NX_PRISM_SYSMESSAGE_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_SYSTEM_RSV_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_SYSTEM_DEVICE_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

/* LiveTV Mode */
extern ONDK_Result_t	NX_PRISM_SYSTEMVolume_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MUTE_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_WIDE_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_DOLBY_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern HERROR			NX_PRISM_DOLBY_ShowDolbyLogo(DxAudioCodec_e eAudCodec);
extern HERROR			NX_PRISM_DOLBY_CloseDolbyLogo(void);
extern ONDK_Result_t	NX_PRISM_SUBTTL_UI_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_AUDIO_UI_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_OPTION_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

extern ONDK_Result_t	NX_PRISM_LIVE_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_CHINFO_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_CHANNELLIST_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_INFOHUB_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern HERROR			NX_PRISM_INFOHUB_Close(void);
extern ONDK_Result_t	NX_PRISM_CHANNELLIST_OPT_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_CHANNELLIST_ADDFAV_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_RADIO_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern HERROR			NX_PRISM_RADIO_CreateStill(void);
extern HERROR			NX_PRISM_RADIO_CloseStill(void);
extern ONDK_Result_t	NX_PRISM_NO_SERVICE_MGR_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);

/* Program Detail */
extern ONDK_Result_t	Nx_PRISM_ProgramDetail_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

/* GUIDE */
extern ONDK_Result_t	NX_PRISM_GUIDE_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_GUIDE_JUMPTODATE_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_GUIDE_GENRESEARCH_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

/* Home Menu */
extern ONDK_Result_t	NX_PRISM_HomeMenu_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

/* Menu - Settings */
extern ONDK_Result_t	NX_PRISM_MENU_Settings_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_General_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_System_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MEMNU_Network_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_Pvr_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_ParentalControl_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_Language_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_Time_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

extern ONDK_Result_t	NX_PRISM_MENU_Video_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_Audio_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_Channel_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_GWM_Callback_t		NX_PRISM_MENU_Channel_GetSearchChannelsLaunchSubMenu(void);
extern ONDK_Result_t	NX_PRISM_MENU_Osd_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_LipSyncPopupBar_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

/* Menu - System */
extern ONDK_Result_t	NX_PRISM_MENU_SysInfo_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_SwUpInfo_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_SwUpDownload_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

#if defined(CONFIG_MW_CAS_NAGRA)
extern ONDK_Result_t	NX_PRISM_MENU_SmtCardInfo_NA_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_CASMESSAGE_NA_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

#if defined(CONFIG_MW_CAS_IREDETO)
extern ONDK_Result_t	NX_PRISM_MENU_SmtCardInfo_IR_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

extern ONDK_Result_t	NX_PRISM_MENU_PowerManagement_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_PowerTimer_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_PowerTimerSettings_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_FactoryDefault_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_HDMICEC_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_ConditionalAccess_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_CA_ServiceList_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_CA_ServiceStatus_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

#if defined(CONFIG_APUI_SET_HIDDEN_INFO)
extern ONDK_Result_t	NX_PRISM_MENU_HiddenManagement_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern HBOOL			NX_PRISM_MENU_HiddenManagement_CheckHiddenKeySeq(HINT32 lKey, HINT32 *peMenuHiddenType);
extern void				NX_PRISM_MENU_HiddenManagement_ResetHiddenKeySeq(void);
extern HBOOL			NX_PRISM_MENU_HiddenManagement_CheckHiddenAllKeySeq(HINT32 lKey);
#if defined(CONFIG_APUI_SET_HIDDEN_INFO_PWD_RESET)
extern HBOOL			NX_PRISM_MENU_HiddenManagement_CheckHiddenPwdResetKeySeq(HINT32 lKey);
#endif
#if defined(CONFIG_APUI_SET_HIDDEN_INFO_COUNTRY_CODE)
extern HBOOL			NX_PRISM_MENU_HiddenManagement_CheckHiddenCountryCodeKeySeq(HINT32 lKey);
#endif
#if defined(CONFIG_APUI_SET_HIDDEN_INFO_FUSING_INFO)
extern ONDK_Result_t	NX_PRISM_MENU_HiddenFusingInfo_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif
#endif	// End of defined(CONFIG_APUI_SET_HIDDEN_INFO)

/* Menu - PVR */
#if defined(CONFIG_SUPPORT_IPEPG_APP)
extern ONDK_Result_t	NX_PRISM_MENU_IpEpg_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif//CONFIG_SUPPORT_IPEPG_APP
#if defined(CONFIG_MW_MM_PVR)
#if defined(CONFIG_APUI_SET_RECORDING_UI_DISPLAY)
extern ONDK_Result_t    NX_PRISM_MENU_Storage_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_Storage_DeviceSetting_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_PlaybackOption_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_RemoteRecord_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_RecordOption_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif
#endif//CONFIG_MW_MM_PVR

/* Menu - Network */
#if defined (CONFIG_MW_INET)
extern ONDK_Result_t	NX_PRISM_MENU_ConfigureLan_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_ConfigureWLan_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_ConnectionStatus_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif//CONFIG_MW_INET

/* Menu - Search */
extern ONDK_Result_t	NX_PRISM_MENU_FavChList_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_FavChList_Option_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_EditChList_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_EditChList_Option_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_SignalTest_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_Search_UI_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_Search_Option_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_Search_SetOption_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

/* Menu - Channel Satellite */
#if defined(CONFIG_MW_CH_SATELLITE)
extern ONDK_Result_t	NX_PRISM_MENU_Search_SatFixedAntna_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_Search_SatSatelliteSetting_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_Search_SatTransponderSetting_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_Search_SatScdAntena_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_Search_ScdSetting_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

// life cycle
extern ONDK_Result_t	NX_PRISM_STANDBY_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_START_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

// Media
#if defined(CONFIG_MW_MM_MEDIA)
#if defined(CONFIG_APUI_SET_MEDIA_UI_DISPLAY)
extern ONDK_Result_t	NX_PRISM_MEDIA_TypeSelect_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MEDIA_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif
extern ONDK_Result_t	NX_PRISM_SUBTITLE_Option_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_SUBTITLE_Sync_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

// Recordings
#if defined(CONFIG_MW_MM_PVR)
#if defined(CONFIG_APUI_SET_RECORDING_UI_DISPLAY)
extern ONDK_Result_t	NX_PRISM_RECORDINGS_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_RECORDINGS_Ext_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif
#endif

// Schedule
extern ONDK_Result_t	NX_PRISM_SCHEDULE_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_SCHEDULE_Option_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_SCHEDULE_Add_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_SCHEDULE_Repeat_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

// demo test
extern ONDK_Result_t	NX_PRISM_DEMO_MEMINFO_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

// TEST progress
extern ONDK_Result_t	NX_PRISM_progress_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

//KEYBOARD
extern ONDK_Result_t	NX_PRISM_KeyBoard_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern HCHAR *			NX_PRISM_KeyBoard_GetResultString(void);
extern ONDK_Result_t	NX_PRISM_KeyBoard_Destroy(void);

extern ONDK_Result_t	NX_PRISM_SimpleKeyboard_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3);
extern HCHAR *			NX_PRISM_SimpleKeyBoard_GetResultString(void);

//InstallWizard
extern ONDK_Result_t	NX_PRISM_Install_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

// ScreenSaver
#if defined(CONFIG_APUI_SCREEN_SAVER)
extern ONDK_Result_t	NX_PRISM_SCREENSAVER_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

#if defined(CONFIG_APUI_SET_SMARTSEARCH_UI_DISPLAY)
//Smart Search
extern ONDK_Result_t	NX_PRISM_SMARTSEARCH_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#endif

//Playbar
#if defined(CONFIG_MW_MM_PVR) || defined(CONFIG_MW_MM_MEDIA)
extern ONDK_Result_t	NX_PRISM_PLAYBAR_Proc(HINT32 lMessage,Handle_t handle,HINT32 p1,HINT32 p2,HINT32 p3);
extern ONDK_Result_t	NX_PRISM_PLAYMGR_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern HERROR			NX_PRISM_PLAYMGR_PlayStart(NxPlayerType_e ePlayerType, void *playData, HUINT32 ulExtraFilter, HBOOL bUsedSendMsg);
#endif

//Shahid application
#if defined(CONFIG_OP_SHAHID_APPS)
extern ONDK_Result_t	NX_PRISM_SHAHID_BG_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_SHAHID_Proc(HINT32 lMessage,Handle_t handle,HINT32 p1,HINT32 p2,HINT32 p3);
extern ONDK_Result_t	NX_PRISM_SHAHID_DETAIL_Proc(HINT32 lMessage,Handle_t handle,HINT32 p1,HINT32 p2,HINT32 p3);
extern ONDK_Result_t	NX_PRISM_SHAHID_Player_Proc(HINT32 lMessage,Handle_t handle,HINT32 p1,HINT32 p2,HINT32 p3);

#endif

#if defined(CONFIG_OP_AMX)
extern ONDK_Result_t	NX_PRISM_MENU_HiddenCountry_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_HiddenTechnicalMenu(void);
extern HBOOL			NX_PRISM_MENU_HiddenManagement_CheckHiddenBackDoorKeySeq(HINT32 lKey);
extern HBOOL			NX_PRISM_MENU_HiddenManagement_CheckHiddenTechnicalMenuKeySeq(HINT32 lKey);

extern ONDK_Result_t	NX_PRISM_MENU_TimeInfo_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
extern ONDK_Result_t	NX_PRISM_MENU_SignalTestAMX_Proc(HINT32 nMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);;
#endif

extern ONDK_Result_t	NX_PRISM_KEYWORDSEARCH_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);

#endif // __NX_PRISM_APP_H__

