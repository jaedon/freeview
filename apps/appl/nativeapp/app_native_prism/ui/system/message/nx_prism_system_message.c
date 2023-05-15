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
 * @file	  		nx_prism_systemmessage.c
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
#include <nx_main.h>
#include <nx_core.h>
#include <nx_port.h>
#include <nx_common.h>
#include <nx_core_message.h>


/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/
#define	NX_PRISM_SYSTEM_THERMAL_TIMER_ID				0x001
#define	NX_PRISM_SYSTEM_THERMAL_TIMER_INTERVAL			(NX_COMMON_TIME_SECOND * NX_COMMON_TIME_DEFAULT)		// 5sec


// TODO:  임시로 디자인 한 형상이니막 뒤집어도 됩니다.

typedef enum
{
	eSTATUS_LIVE_CLEAR						= 0x00000000,
	eSTATUS_LIVE_NOSIGNAL					= 0x00000001,
	eSTATUS_LIVE_SCRAMBLEDORNOTAVAILABLE	= 0x00000002,
	eSTATUS_LIVE_CHANNELLOCK				= 0x00000004,
	eSTATUS_LIVE_HDMI_BLOCKED				= 0x00000010,
	eSTATUS_LIVE_NO_VIDEO					= 0x00000020,
	eSTATUS_LIVE_NOK_ANTENNA				= 0x00000040,
	eSTATUS_LIVE_MOTOR_MOVING				= 0x00000080,
	eSTATUS_LIVE_LOCK_PIN					= 0x00000100,
	eSTATUS_LIVE_CAS_BLOCK		 			= 0x00000200,
	eSTATUS_LIVE_PARENTAL_LOCK				= 0x00000400,
	eSTATUS_LIVE_PIN_DENIED					= 0x00000800,
	eSTATUS_LIVE_NO_AV						= 0x00001000,
	eSTATUS_LIVE_NO_EIT 					= 0x00002000,
	eSTATUS_LIVE_NOT_RUNNING				= 0x00004000,

	eSTATUS_LIVE_UNDEFINED					= 0x80000000,
} eSYSMsgStatusLive_e;


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/

typedef struct
{
	eSYSMsgStatusLive_e				eLastLiveStatus;
	eSYSMsgStatusLive_e				eLiveStatus;
	OxMediaPlay_LiveViewState_e	 	eViewState;
	OxMediaPlay_LiveLockState_e		eLockState;
} NxUiSysMsgContext_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NxUiSysMsgContext_t		s_stUiSysMsg;
static NX_PinDialogInstance_t	s_stUiSysMsg_PinCodeDialog;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static ONDK_Result_t	nx_SysMsg_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_SysMsg_DestroyPinCodeDialog(void);
static void				nx_SysMsg_CreatePinCodeDialog(HINT8 *pszStrId, NX_PINCODE_DATA_TYPE_t	pinType);

static ONDK_Result_t	nx_SysMsg_StatusThermalProtectionDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_SysMsg_DestroyStatusThermalProtectionDialouge(void);
static void				nx_SysMsg_CreateStatusThermalProtectionDialouge(void);

static ONDK_Result_t	nx_SysMsg_HDCPErrMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static void				nx_SysMsg_HDCPErrMsgCreate(void);
static void				nx_SysMsg_HDCPErrMsgDestroy(void);
static ONDK_Result_t	nx_SysMsg_StatuesHDMIMessage(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);

static NxUiSysMsgContext_t* 	nx_SysMsg_GetContext(void);
static void 			nx_SysMsg_StatusMsgCreate(HUINT8 *pMsg);
static void 			nx_SysMsg_StatusMsgDestroy(void);
static HERROR			nx_SysMsg_Update(NxUiSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static HERROR			nx_SysMsg_GetNextErrorByStates(NxUiSysMsgContext_t *pstContents);
static HERROR			nx_SysMsg_GetErrorFromNotifier(NxUiSysMsgContext_t *pstContents);
static HERROR			nx_SysMsg_CheckLockState(NxUiSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2);

static ONDK_Result_t 	nx_SysMsg_MsgGwmCreate(NxUiSysMsgContext_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_SysMsg_MsgGwmDestroy(NxUiSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_SysMsg_MsgGwmTimer(NxUiSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_SysMsg_MsgGwmApkMsg(NxUiSysMsgContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
#if defined(CONFIG_MW_CAS)
static ONDK_Result_t	nx_SysMsg_MsgCasStateChange(NxUiSysMsgContext_t *pstContents, HINT32 p1);
#endif


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Pin_Code_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_SysMsg_DisplayPinCodeDialog(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_COMP_PinDlalog_ProtoProc(&s_stUiSysMsg_PinCodeDialog, message, hAction, p1, p2, p3);
}


static ONDK_Result_t	nx_SysMsg_DestroyPinCodeDialog(void)
{
	return ONDK_GWM_APP_Destroy(nx_SysMsg_DisplayPinCodeDialog);
}


static void		nx_SysMsg_CreatePinCodeDialog(HINT8 *pszStrId, NX_PINCODE_DATA_TYPE_t	pinType)
{
	ONDK_Rect_t		stRect;
	HUINT32			ulAttribute = 0;
	ONDK_GWM_App_t					pGwmApp;

	pGwmApp = ONDK_GWM_APP_Get(nx_SysMsg_DisplayPinCodeDialog);
	if(pGwmApp == NULL)
	{
		stRect = ONDK_Rect(NX_PRISM_UI_PIN_DIALOG_X, NX_PRISM_UI_PIN_DIALOG_Y, NX_PRISM_UI_PIN_DIALOG_WIDTH, NX_PRISM_UI_PIN_DIALOG_HEIGHT);
		ulAttribute = (NX_DLG_TYPE_OSD_FULL|NX_DLG_TYPE_BG_DIM|NX_DLG_TYPE_MESSAGE|NX_DLG_TIMEOUT_NOTIMEOUT);
		NX_PRISM_COMP_PinDlalog_SetDialog(&s_stUiSysMsg_PinCodeDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
									&stRect, ulAttribute, COL(C_T_Whistle_100), ONDK_GetString(pszStrId), pinType);
		/* Create Pop Up */
		ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_SysMsg_DisplayPinCodeDialog", APP_DIALOGBOX_PRIORITY, nx_SysMsg_DisplayPinCodeDialog, 0, 0, 0, 0);
	}
}


/******************************************************************************/
#define __ThermalProtection_Dialog_Functions___________________________________
/******************************************************************************/

static ONDK_Result_t	nx_SysMsg_StatusThermalProtectionDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return	NX_PRISM_DIALOGUE_StatusMessage_Proc(message, hAction, p1, p2, p3);
}


static void		nx_SysMsg_DestroyStatusThermalProtectionDialouge(void)
{
	if (ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)nx_SysMsg_StatusThermalProtectionDialogProc) != NULL)
	{
		ONDK_GWM_APP_Destroy(nx_SysMsg_StatusThermalProtectionDialogProc);
	}
}


static void		nx_SysMsg_CreateStatusThermalProtectionDialouge(void)
{
	nx_SysMsg_DestroyStatusThermalProtectionDialouge();

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW, "nx_SysMsg_StatusThermalProtectionDialogProc", APP_AUTOPOWERDOWN_PRIORITY,
						nx_SysMsg_StatusThermalProtectionDialogProc, 0, (HINT32)ONDK_GetString(RES_THERMAL_PROTECTION_MSG), 0, 0);
}


/******************************************************************************/
#define __HDCP_Error_Dialog_Functions___________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_SysMsg_HDCPErrMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_StatusMessage_Proc(message, hAction, p1, p2, p3);
}


static void		nx_SysMsg_HDCPErrMsgCreate(void)
{
	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW,	"nx_SysMsg_HDCPErrMsgProc",	APP_INDEPENDENT_OSD_GRP_PRIORITY,
						nx_SysMsg_HDCPErrMsgProc, 0, (HINT32)ONDK_GetString(RES_HDCP_ERROR_MSG_ID), 0, 0);
}


static void		nx_SysMsg_HDCPErrMsgDestroy(void)
{
	if (ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)nx_SysMsg_HDCPErrMsgProc) != NULL)
	{
		ONDK_GWM_APP_Destroy(nx_SysMsg_HDCPErrMsgProc);
	}
}


static ONDK_Result_t	nx_SysMsg_StatuesHDMIMessage(Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR				hError = ERR_FAIL;
	ONDK_Result_t		ret = ONDK_RET_OK;
	DxHdcpStatus_e		eHdcpStatus = eDxHDCPSTATUS_OFF;

	NX_COMMON_NOT_USED_ARGUMENT(hAction, p1, p2, p3);

	hError = NX_SYSTEM_GetHdmiHdcpStatus(&eHdcpStatus);
	if (hError != ERR_OK)
	{
		eHdcpStatus = eDxHDCPSTATUS_OFF;
	}

	if(eHdcpStatus == eDxHDCPSTATUS_OK || eHdcpStatus == eDxHDCPSTATUS_OFF)
	{
		HxLOG_Warning("[status : eDxHDCPSTATUS_OK || eDxHDCPSTATUS_OFF] \r\n");
		nx_SysMsg_HDCPErrMsgDestroy();
	}
	else if(eHdcpStatus == eDxHDCPSTATUS_FAIL || eHdcpStatus == eDxHDCPSTATUS_NODEVICE)
	{
		HxLOG_Warning("[status : eDxHDCPSTATUS_FAIL || eDxHDCPSTATUS_NODEVICE] \r\n");
		nx_SysMsg_HDCPErrMsgDestroy();
		nx_SysMsg_HDCPErrMsgCreate();
	}
	else if(eHdcpStatus == eDxHDCPSTATUS_TRY)
	{
		HxLOG_Warning("[status : eDxHDCPSTATUS_TRY] \r\n");
	}

	ret =  GWM_MESSAGE_BREAK;

	return ret;
}


/******************************************************************************/
#define __Local_Functions_____________________________________________________
/******************************************************************************/

static NxUiSysMsgContext_t*	nx_SysMsg_GetContext(void)
{
	return &s_stUiSysMsg;
}

static ONDK_Result_t	nx_SysMsg_StatusMsgProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_StatusMessage_Proc(message, hAction, p1, p2, p3);
}

static void		nx_SysMsg_StatusMsgCreate(HUINT8 *pMsg)
{
	nx_SysMsg_StatusMsgDestroy();

	ONDK_GWM_APP_Create (ONDK_UPPER_WINDOW,	"nx_SysMsg_StatusMsgProc", APP_ACTION_PRIORITY,
							nx_SysMsg_StatusMsgProc, 0, (HINT32)pMsg, 0, 0);
}

static void		nx_SysMsg_StatusMsgDestroy(void)
{
	ONDK_GWM_App_t	gwmApp = NULL;

	gwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)nx_SysMsg_StatusMsgProc);
	if (gwmApp != NULL)
	{
		ONDK_GWM_APP_Destroy(nx_SysMsg_StatusMsgProc);
	}
}

static HERROR	nx_SysMsg_GetNextErrorByStates(NxUiSysMsgContext_t *pstContents)
{
	NX_APP_Trace("[+] nx_SysMsg_GetNextErrorByStates() \n");

	// 1. antenna short-circuit
	if (eOxMP_LiveViewState_NOK_ANTENNA == pstContents->eViewState)
	{
		NX_APP_Info(" - Next Error State : eOxMP_LiveViewState_NOK_ANTENNA \n");
		pstContents->eLiveStatus = eSTATUS_LIVE_NOK_ANTENNA;
		return	ERR_OK;
	}

	// 2. Channel Unlock
	if ((eOxMP_LiveViewState_NO_SIGNAL == pstContents->eViewState) ||
		(eOxMP_LiveViewState_NO_SIGNAL_TIMEOUT == pstContents->eViewState))
	{
		NX_APP_Info(" - Next Error State : eOxMP_LiveViewState_NO_SIGNAL || eOxMP_LiveViewState_NO_SIGNAL_TIMEOUT \n");
		pstContents->eLiveStatus = eSTATUS_LIVE_NOSIGNAL;
		return	ERR_OK;
	}

	// 3. Motor
	if (eOxMP_LiveViewState_MOTOR_MOVING == pstContents->eViewState)
	{
		NX_APP_Info(" - Next Error State : eOxMP_LiveViewState_MOTOR_MOVING\n");
		pstContents->eLiveStatus = eSTATUS_LIVE_MOTOR_MOVING;
		return	ERR_OK;
	}

	// 4: Empty Service
	if (eOxMP_LiveViewState_EMPTY_SVC == pstContents->eViewState)
	{
		NX_APP_Info(" - Next Error State : eOxMP_LiveViewState_EMPTY_SVC\n");
		pstContents->eLiveStatus = eSTATUS_LIVE_SCRAMBLEDORNOTAVAILABLE;
		return	ERR_OK;
	}

	// 5: User Lock 관련
	if (eOxMP_LiveLockState_LOCK_PIN == pstContents->eLockState)
	{
		NX_APP_Info(" - Next Error State : eOxMP_LiveLockState_LOCK_PIN\n");
		pstContents->eLiveStatus = eSTATUS_LIVE_LOCK_PIN;
		return	ERR_OK;
	}
	if (eOxMP_LiveLockState_LOCK_PIN_DENIED == pstContents->eLockState)
	{
		NX_APP_Info(" - Next Error State : eOxMP_LiveLockState_LOCK_PIN_DENIED\n");
		pstContents->eLiveStatus = eSTATUS_LIVE_PIN_DENIED;
		return	ERR_OK;
	}

	// 6: CAS
	if (eOxMP_LiveViewState_CAS == pstContents->eViewState)
	{
		NX_APP_Info(" - Next Error State : eOxMP_LiveViewState_CAS\n");
		pstContents->eLiveStatus = eSTATUS_LIVE_CAS_BLOCK;
		return	ERR_OK;
	}

	// 7: SVC Lock 외의 Lock State들:
	switch (pstContents->eLockState)
	{
		case eOxMP_LiveLockState_RATING_PIN:
		case eOxMP_LiveLockState_RATING_PIN_DENIED:
		case eOxMP_LiveLockState_SC_PIN:
			NX_APP_Info(" - Next Error State : eSTATUS_LIVE_PARENTAL_LOCK\n");
			pstContents->eLiveStatus = eSTATUS_LIVE_PARENTAL_LOCK;
			return	ERR_OK;

		case eOxMP_LiveLockState_SC_PIN_DENIED:
			NX_APP_Info(" - Next Error State : eSTATUS_LIVE_PIN_DENIED\n");
			pstContents->eLiveStatus = eSTATUS_LIVE_PIN_DENIED;
			return	ERR_OK;

		case eOxMP_LiveLockState_NO_EIT:
			NX_APP_Info(" - Next Error State : eSTATUS_LIVE_NO_EIT\n");
			pstContents->eLiveStatus = eSTATUS_LIVE_NO_EIT;
			return	ERR_OK;

		default:
			break;
	}

	// 8: No SVC, A/V 관련
	switch (pstContents->eViewState)
	{
		case eOxMP_LiveViewState_AV_UNDER_RUN:
			NX_APP_Info(" - Next Error State : eOxMP_LiveViewState_AV_UNDER_RUN\n");
			pstContents->eLiveStatus = eSTATUS_LIVE_SCRAMBLEDORNOTAVAILABLE;
			return	ERR_OK;

		case eOxMP_LiveViewState_HDMI_BLOCKED:
			NX_APP_Info(" - Next Error State : eOxMP_LiveViewState_HDMI_BLOCKED\n");
			pstContents->eLiveStatus = eSTATUS_LIVE_HDMI_BLOCKED;
			return	ERR_OK;

		case eOxMP_LiveViewState_NO_VIDEO:
			NX_APP_Info(" - Next Error State : eOxMP_LiveViewState_NO_VIDEO\n");
			pstContents->eLiveStatus = eSTATUS_LIVE_NO_VIDEO;
			return	ERR_OK;

		case eOxMP_LiveViewState_NO_AV:
			NX_APP_Info(" - Next Error State : eOxMP_LiveViewState_NO_AV\n");
			pstContents->eLiveStatus = eSTATUS_LIVE_NO_AV;
			return	ERR_OK;

		case eOxMP_LiveViewState_NOT_RUNNING:
			NX_APP_Info(" - Next Error State : eOxMP_LiveViewState_NOT_RUNNING\n");
			pstContents->eLiveStatus = eSTATUS_LIVE_NOT_RUNNING;
			return	ERR_OK;

		default:
			break;
	}

	if (((eOxMP_LiveViewState_OK == pstContents->eViewState) || (eOxMP_LiveViewState_NONE == pstContents->eViewState)) &&
		((eOxMP_LiveLockState_OK == pstContents->eLockState) || (eOxMP_LiveLockState_NONE == pstContents->eLockState)))
	{
		NX_APP_Info(" - Next Error State : eSTATUS_LIVE_CLEAR\n");
		pstContents->eLiveStatus = eSTATUS_LIVE_CLEAR;
		return	ERR_OK;
	}

	NX_APP_Info(" - Next Error State : eSTATUS_LIVE_UNDEFINED\n");
	pstContents->eLiveStatus = eSTATUS_LIVE_UNDEFINED;

	return ERR_OK;
}


static HERROR	nx_SysMsg_GetErrorFromNotifier(NxUiSysMsgContext_t *pstContents)
{
	NX_APP_Trace("[+] nx_SysMsg_GetErrorFromNotifier() \n");

	/*휴지중인 경우에는 lock state가 항상 none일 것이기 때문에 휴지중인 경우는 lock state와 상관 없이 따로 check한다.*/
	if ((eOxMP_LiveViewState_NONE == pstContents->eViewState) || (eOxMP_LiveLockState_NONE == pstContents->eLockState))
	{
		NX_APP_Info(" - Error State : eOxMP_LiveViewState_NONE || eOxMP_LiveLockState_NONE \n");
		if (eOxMP_LiveViewState_OK != pstContents->eViewState)
		{
			NX_APP_Info(" - Error State : !eOxMP_LiveViewState_OK \n");
			nx_SysMsg_GetNextErrorByStates(pstContents);
		}
		else
		{
			NX_APP_Info(" - Error State : eOxMP_LiveViewState_OK \n");
			pstContents->eLiveStatus = eSTATUS_LIVE_CLEAR;
		}
	}
	else if ((eOxMP_LiveViewState_OK == pstContents->eViewState) && (eOxMP_LiveLockState_OK == pstContents->eLockState))
	{
		NX_APP_Info(" - Error State : eOxMP_LiveViewState_OK && eOxMP_LiveLockState_OK  \n");
		pstContents->eLiveStatus = eSTATUS_LIVE_CLEAR;
	}
	else
	{
		NX_APP_Info(" - Error State : Find Next Error \n");
		nx_SysMsg_GetNextErrorByStates(pstContents);
	}

	return ERR_OK;
}

static HERROR	nx_SysMsg_CheckLockState(NxUiSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2)
{

	HUINT32							ulViewId = 0;
	OxMediaPlay_MediaType_e			eMediaType = eOxMP_MEDIATYPE_NONE;
	OxMediaPlay_LiveLockState_e 	lockState;
	OxMediaPlay_Event_e				eMediaEvent = (OxMediaPlay_Event_e)p1;
	HBOOL							bAvBlock = (HBOOL)p2;

	NX_MEDIA_PLAY_GetMainViewId (&ulViewId);
	NX_MEDIA_PLAY_GetPlayType(ulViewId, &eMediaType);

	if(eMediaType == eOxMP_MEDIATYPE_LIVE || eMediaType == eOxMP_MEDIATYPE_PVRPLAYBACK)
	{
		if(eMediaEvent == eOxMP_EVENT_PinCtrlChanged && bAvBlock == TRUE)
		{
			pstContents->eLastLiveStatus = pstContents->eLiveStatus;
			pstContents->eViewState = eOxMP_LiveViewState_NONE;
			pstContents->eLockState = eOxMP_LiveLockState_RATING_PIN;
			pstContents->eLiveStatus = eSTATUS_LIVE_PARENTAL_LOCK;

			return nx_SysMsg_Update(pstContents, 0, 0, 0);
		}

		NX_CHANNEL_GetLockState(&lockState);

		if((pstContents->eViewState == eOxMP_LiveViewState_OK || pstContents->eViewState == eOxMP_LiveViewState_NONE || pstContents->eViewState == eOxMP_LiveViewState_SVC_LOCKED)
			&&(pstContents->eLiveStatus == eSTATUS_LIVE_CLEAR || pstContents->eLiveStatus == eSTATUS_LIVE_PARENTAL_LOCK || pstContents->eLiveStatus == eSTATUS_LIVE_LOCK_PIN))
		{
			pstContents->eLastLiveStatus = pstContents->eLiveStatus;
			pstContents->eViewState = eOxMP_LiveViewState_NONE;
			pstContents->eLockState = eOxMP_LiveLockState_NONE;

			switch (lockState)
			{
				case eOxMP_LiveLockState_RATING_PIN:
				case eOxMP_LiveLockState_RATING_PIN_DENIED:
					pstContents->eLockState = lockState;
					pstContents->eViewState = eOxMP_LiveViewState_SVC_LOCKED;
					pstContents->eLiveStatus = eSTATUS_LIVE_PARENTAL_LOCK;
					break;

				case eOxMP_LiveLockState_LOCK_PIN:
				case eOxMP_LiveLockState_LOCK_PIN_DENIED:
					pstContents->eViewState = eOxMP_LiveViewState_SVC_LOCKED;
					pstContents->eLiveStatus = eSTATUS_LIVE_LOCK_PIN;
					break;

				default:
					pstContents->eLiveStatus = eSTATUS_LIVE_UNDEFINED;
					break;
			}

			return nx_SysMsg_Update(pstContents, 0, 0, 0);
		}
	}
	return ERR_FAIL;
}

static HERROR	nx_SysMsg_Update(NxUiSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HERROR		hError = ERR_FAIL;
	HBOOL		bStateChanged = FALSE;
	HINT8		*pSzStrID = NULL;
	HUINT32		ulAppCnt = 0;

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	NX_APP_Trace("[+] nx_SysMsg_Update() \n");

	NX_APP_Error("=====>[JHLEE] nx_SysMsg_Update() eLiveStatus:(0x%x), eLastLiveStatus:(0x%x) \n", pstContents->eLiveStatus, pstContents->eLastLiveStatus);
	if ((eOxMP_LiveViewState_NONE == pstContents->eViewState) && (eOxMP_LiveLockState_NONE == pstContents->eLockState))
	{
		NX_APP_Info(" - Check Update : eOxMP_LiveViewState_NONE && eOxMP_LiveLockState_NONE \n");
		if (pstContents->eLiveStatus != pstContents->eLastLiveStatus)
		{
			NX_APP_Info(" - Check Update : (pstContents->eLiveStatus != pstContents->eLastLiveStatus) \n");
			bStateChanged = TRUE;
		}
		else
		{
			NX_APP_Info(" - Check Update : Same View status \n");
			bStateChanged = FALSE;
		}
	}
	else
	{
		NX_APP_Info(" - Check Update : View State Change \n");
		hError = nx_SysMsg_GetErrorFromNotifier(pstContents);
		if (hError != ERR_OK)
		{
			NX_APP_Error(" - nx_SysMsg_GetErrorFromNotifier() \n");
			bStateChanged = FALSE;
		}
	}

	/*rating pin에 대한 error는 parentalRatingChanged event로 올려줘야한다.*/
	if ((pstContents->eLockState == eOxMP_LiveLockState_RATING_PIN) || (pstContents->eLockState == eOxMP_LiveLockState_RATING_PIN_DENIED))
	{
		NX_APP_Info(" - Check Update : eOxMP_LiveLockState_RATING_PIN || eOxMP_LiveLockState_RATING_PIN_DENIED \n");
		ulAppCnt += ONDK_GWM_APP_CountAppByUiState(UISTATE_MENU);
		ulAppCnt += ONDK_GWM_APP_CountAppByUiState(UISTATE_EPG);
		ulAppCnt += ONDK_GWM_APP_CountAppByUiState(UISTATE_RECORDINGS);
		ulAppCnt += ONDK_GWM_APP_CountAppByUiState(UISTATE_MEDIA);
		ulAppCnt += ONDK_GWM_APP_CountAppByUiState(UISTATE_SCHEDULE);

		if(((pstContents->eLiveStatus == eSTATUS_LIVE_PARENTAL_LOCK) || (pstContents->eLiveStatus == eSTATUS_LIVE_PIN_DENIED)) && ulAppCnt == 0)
		{
			NX_APP_Info(" - Check Update : eSTATUS_LIVE_PARENTAL_LOCK || eSTATUS_LIVE_PIN_DENIED \n");
			bStateChanged = TRUE;
		}
		else
		{
			NX_APP_Info(" - Check Update : ! eSTATUS_LIVE_PARENTAL_LOCK || ! eSTATUS_LIVE_PIN_DENIED \n");
			bStateChanged = FALSE;
		}
	}
	else
	{
		NX_APP_Info(" - Check Update : ! eOxMP_LiveLockState_RATING_PIN || ! eOxMP_LiveLockState_RATING_PIN_DENIED \n");
		bStateChanged = TRUE;
	}

	if (bStateChanged == FALSE)
	{
		NX_APP_Error("Error!!! nx_SysMsg_Update() bStateChanged:(%d) \n", bStateChanged);
		return ERR_FAIL;
	}
	else
	{
		NX_APP_Info(" - Check Update : bStateChanged == TRUE \n");
	}

	switch (pstContents->eLiveStatus)
	{
		case eSTATUS_LIVE_CLEAR:
			nx_SysMsg_StatusMsgDestroy();
			break;
		case eSTATUS_LIVE_NOSIGNAL:					pSzStrID = RES_MESG_1014_ID;					break;
		case eSTATUS_LIVE_SCRAMBLEDORNOTAVAILABLE:	pSzStrID = RES_LIVE_SCRAMBED_OR_NOTAVAILABLE;	break;
		case eSTATUS_LIVE_HDMI_BLOCKED:				pSzStrID = RES_HDCP_ERROR_MSG_ID;				break;
		case eSTATUS_LIVE_NO_VIDEO:					pSzStrID = RES_MESG_117_ID;						break;
		case eSTATUS_LIVE_NOK_ANTENNA:				pSzStrID = RES_MESG_007_ID;						break;
		case eSTATUS_LIVE_MOTOR_MOVING:				pSzStrID = RES_MOTOR_IS_MOVING_ID;				break;
		case eSTATUS_LIVE_LOCK_PIN:					pSzStrID = RES_MESG_806_ID;						break;
		case eSTATUS_LIVE_CAS_BLOCK:				pSzStrID = RES_LIVE_SCRAMBED_OR_NOTAVAILABLE;	break;
		case eSTATUS_LIVE_PARENTAL_LOCK:			pSzStrID = RES_MESG_3391_ID;					break;
		case eSTATUS_LIVE_PIN_DENIED:				pSzStrID = NULL;								break;
		case eSTATUS_LIVE_NO_AV:					pSzStrID = RES_MESG_076_ID;						break;
		case eSTATUS_LIVE_NO_EIT:					pSzStrID = NULL;								break;
		case eSTATUS_LIVE_NOT_RUNNING:				pSzStrID = NULL;								break;

		default:									pSzStrID = NULL;								break;
	}


	switch (pstContents->eLiveStatus)
	{
		case eSTATUS_LIVE_LOCK_PIN:
			nx_SysMsg_StatusMsgDestroy();
			nx_SysMsg_CreatePinCodeDialog(pSzStrID, eNxPinCodeDataType_LockChannel);
			break;
		case eSTATUS_LIVE_PARENTAL_LOCK:
			nx_SysMsg_StatusMsgDestroy();
			nx_SysMsg_CreatePinCodeDialog(pSzStrID, eNxPinCodeDataType_LockMaturityRating);
			break;
		case eSTATUS_LIVE_CAS_BLOCK:
			// TODO: [JHLEE] Add Cas
			break;
		default:
			if (pSzStrID)
			{
				NX_APP_Info(" - Check Update : nx_SysMsg_StatusMsgCreate() \n");
				nx_SysMsg_StatusMsgCreate(ONDK_GetString(pSzStrID));
			}
			break;
	}

	return ERR_OK;
}

static HERROR	nx_SysMsg_Initilization(NxUiSysMsgContext_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);
	NX_COMMON_NOT_USED_PARAM(hAct);

	HxSTD_MemSet(pstContents, 0x00, sizeof(NxUiSysMsgContext_t));
	pstContents->eLastLiveStatus = eSTATUS_LIVE_CLEAR;
	pstContents->eLiveStatus = eSTATUS_LIVE_CLEAR;
	pstContents->eViewState = eOxMP_LiveViewState_NONE;
	pstContents->eLockState = eOxMP_LiveLockState_NONE;

	return ERR_OK;
}

static ONDK_Result_t	nx_SysMsg_MsgGwmCreate(NxUiSysMsgContext_t *pstContents, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	nx_SysMsg_Initilization(pstContents, hAct, p1, p2, p3);

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_SysMsg_MsgGwmDestroy(NxUiSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Trace();

	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	// TODO: destroy all messages
	ONDK_GWM_KillTimer(NX_PRISM_SYSTEM_THERMAL_TIMER_ID);
	nx_SysMsg_StatusMsgDestroy();

	return	ONDK_RET_OK;
}

static ONDK_Result_t	nx_SysMsg_MsgGwmTimer(NxUiSysMsgContext_t *pstContents, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_ARGUMENT(pstContents, p1, p2, p3);

	switch(p1)
	{
		case NX_PRISM_SYSTEM_THERMAL_TIMER_ID:
			ONDK_GWM_KillTimer(NX_PRISM_SYSTEM_THERMAL_TIMER_ID);
			nx_SysMsg_DestroyStatusThermalProtectionDialouge();
			ONDK_GWM_SendMessage(NULL, MSG_GWM_KEYDOWN, (Handle_t)NULL, KEY_STANDBY, (int)NULL, (int)NULL);
			return ONDK_MESSAGE_BREAK;
		default:

			break;
	}

	return ONDK_RET_OK;
}


static ONDK_Result_t	nx_SysMsg_MsgGwmApkMsg(NxUiSysMsgContext_t *pstContents, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			eResult = ONDK_RET_FAIL;
	GWM_APK_MSG_Type_e		eAPK_Type = (GWM_APK_MSG_Type_e)handle;


	switch(eAPK_Type)
	{
		case GWM_APK_THERMAL:
			nx_SysMsg_CreateStatusThermalProtectionDialouge();
			eResult = ONDK_GWM_SetTimer(NX_PRISM_SYSTEM_THERMAL_TIMER_ID, NX_PRISM_SYSTEM_THERMAL_TIMER_INTERVAL);
			if (eResult == GWM_RET_LINK_CONFLICT)
			{
				ONDK_GWM_ResetTimer(NX_PRISM_SYSTEM_THERMAL_TIMER_ID);
			}
			break;

		case GWM_APK_HDMI :
			return	nx_SysMsg_StatuesHDMIMessage(handle, p1, p2, p3);

		default:
			break;
	}

	return	ONDK_MESSAGE_PASS;
}


#if defined(CONFIG_MW_CAS)
static ONDK_Result_t	nx_SysMsg_MsgCasStateChange(NxUiSysMsgContext_t *pstContents, HINT32 p1)
{
	NX_CasStatus_t	eCasState = eNxCasStatus_None;

	eCasState = (NX_CasStatus_t)p1;
	switch (eCasState)
	{
		case eNxCasStatus_Clear:
			break;

		case eNxCasStatus_Error:
			break;

		case eNxCasStatus_Lock:
			break;

		default:
			NX_APP_Error("Error!! Invalid Cas State : (0x%x) \n", eCasState);
			break;
	}

	return ONDK_RET_OK;
}
#endif


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t		NX_PRISM_SYSMESSAGE_Proc(HINT32 lMessage, Handle_t hHandle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t			lRet = ONDK_RET_OK;
	OxMediaPlay_LiveViewState_e 	eViewState = eOxMP_LiveViewState_NONE;
	OxMediaPlay_LiveLockState_e		eLockState = eOxMP_LiveLockState_NONE;
	OxMediaPlay_MediaType_e			eMediaType = eOxMP_MEDIATYPE_NONE;
	NxUiSysMsgContext_t				*pstContents = nx_SysMsg_GetContext();

	ONDK_Print("Message(%d) : %s, handle: %p p1(0x%x) p2(0x%x) p3(0x%x) \n", lMessage, ONDK_GWM_GwmMsg2String(lMessage), p1, p2, p3);

	switch(lMessage)
	{
		case MSG_GWM_CREATE:
			nx_SysMsg_MsgGwmCreate(pstContents, hHandle, p1, p2, p3);
			break;

		case MSG_GWM_TIMER:
			lRet = nx_SysMsg_MsgGwmTimer(pstContents, p1, p2, p3);
			break;

		case MSG_GWM_KEYDOWN:
			break;

		case MSG_GWM_KEYUP:
			break;

		case MSG_GWM_APK_MESSAGE:
			lRet = nx_SysMsg_MsgGwmApkMsg(pstContents, hHandle, p1, p2, p3);
			break;

		case MSG_APP_SYSTEM_PIN_OK:
			NX_APP_Message("MSG_APP_SYSTEM_PIN_OK\n");
			nx_SysMsg_DestroyPinCodeDialog();
			lRet = ONDK_MESSAGE_BREAK;
			break;

		case MSG_APP_SYSTEM_PIN_DENIED:
			NX_APP_Message("MSG_APP_SYSTEM_PIN_DENIED\n");
			nx_SysMsg_DestroyPinCodeDialog();
			lRet = ONDK_MESSAGE_BREAK;
			break;

		/********************************************************
		* Live
		********************************************************/
		case MSG_APP_SYSTEM_SVC_CHANGED:
			NX_APP_Message("____MSG_APP_SYSTEM_SVC_CHANGED\n");
			pstContents->eLastLiveStatus = pstContents->eLiveStatus;
			pstContents->eLiveStatus = eSTATUS_LIVE_CLEAR;
			pstContents->eViewState	= eOxMP_LiveViewState_NONE;
			pstContents->eLockState	= eOxMP_LiveLockState_NONE;
			nx_SysMsg_DestroyPinCodeDialog();
			nx_SysMsg_Update(pstContents, p1, p2, p3);
			break;

		case MSG_APP_CHANNEL_TUNE_LOCKED:
			NX_APP_Message("____MSG_APP_CHANNEL_TUNE_LOCKED\n");
			pstContents->eLastLiveStatus = pstContents->eLiveStatus;
			pstContents->eLiveStatus = eSTATUS_LIVE_CLEAR;
			pstContents->eViewState	= eOxMP_LiveViewState_NONE;
			pstContents->eLockState	= eOxMP_LiveLockState_NONE;
			nx_SysMsg_Update(pstContents, p1, p2, p3);
			break;

		case MSG_APP_CHANNEL_TUNE_NOSIGNAL:
			NX_APP_Message("____MSG_APP_CHANNEL_TUNE_NOSIGNAL\n");
			pstContents->eLastLiveStatus = pstContents->eLiveStatus;
			pstContents->eLiveStatus = eSTATUS_LIVE_NOSIGNAL;
			pstContents->eViewState	= eOxMP_LiveViewState_NONE;
			pstContents->eLockState	= eOxMP_LiveLockState_NONE;
			nx_SysMsg_DestroyPinCodeDialog();
			nx_SysMsg_Update(pstContents, p1, p2, p3);
			break;

		case MSG_APP_CHANNEL_LIVE_VIEWSTATECHANGED:
			eMediaType = (OxMediaPlay_MediaType_e)p2;
			eViewState = (OxMediaPlay_LiveViewState_e)p3;
			NX_APP_Message("____MSG_APP_CHANNEL_LIVE_VIEWSTATECHANGED [%d]\n", eViewState);
			pstContents->eLastLiveStatus = pstContents->eLiveStatus;
			pstContents->eViewState	= eViewState;
			nx_SysMsg_Update(pstContents, p1, p2, p3);
			break;

		case MSG_APP_CHANNEL_LIVE_LOCKSTATECHANGED:
			eMediaType = (OxMediaPlay_MediaType_e)p2;
			eLockState = (OxMediaPlay_LiveLockState_e)p3;
			NX_APP_Message("____MSG_APP_CHANNEL_LIVE_LOCKSTATECHANGED [%d]\n", eLockState);
			pstContents->eLastLiveStatus = pstContents->eLiveStatus;
			pstContents->eLockState = eLockState;
			nx_SysMsg_Update(pstContents, p1, p2, p3);
			break;

		case MSG_APP_CHANNEL_PINCTRLCHANGED:
			NX_APP_Message("____MSG_APP_CHANNEL_PINCTRLCHANGED\n");
			nx_SysMsg_CheckLockState(pstContents, p1, p2);
			break;

		case MSG_GWM_STACK_CHANGED :
			NX_APP_Message("____MSG_GWM_STACK_CHANGED\n");
			nx_SysMsg_CheckLockState(pstContents, eOxMP_EVENT_None, 0);
			break;

		/********************************************************
		* PVR
		********************************************************/
#if defined(CONFIG_MW_MM_PVR)
		// TODO: Move to Pvr Manager
		case MSG_APP_RECORD_START:
			NX_PRISM_START_RECORD_CreateResponseDialouge(p1, p2);
			break;
			
		case MSG_APP_RECORD_STOP:
			NX_PRISM_STOP_RECORD_CreateResponseDialouge();
			break;
			
		case MSG_APP_RECORD_TSRBUFFER_COPYEND:
			break;
#endif

		/********************************************************
		* CAS
		********************************************************/
		case MSG_APP_SYSTEM_CAS_STATE_UPDATE:
#if defined(CONFIG_MW_CAS)
			nx_SysMsg_MsgCasStateChange(pstContents, p1);
#endif
			break;

		case MSG_GWM_DESTROY:
			lRet = nx_SysMsg_MsgGwmDestroy(pstContents, p1, p2, p3);
			break;
	}

	if(lRet != ONDK_RET_OK)
	{
		return lRet;
	}

	return ONDK_GWM_ProcessMessageDefault(lMessage, hHandle, p1, p2, p3);
}

/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/