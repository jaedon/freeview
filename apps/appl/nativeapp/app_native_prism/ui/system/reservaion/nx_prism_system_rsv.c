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
 * @file	  		nx_prism_system_rsv.c
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
#include <nx_port_system.h>
#include <nx_prism_app.h>

#include "nx_prism_definition.h"

/******************************************************************************/
#define _________Private_Macro________________________________________________
/******************************************************************************/


/******************************************************************************/
#define _________Private_Struct_______________________________________________
/******************************************************************************/
typedef enum {
	eNxSystemRsv_DialogType_None = 0,
	eNxSystemRsv_DialogType_PowerOff,
} Nx_SettingsItem_ScdSetting_DialogType_t;


/******************************************************************************/
#define _________Private_Value________________________________________________
/******************************************************************************/
static NXUI_DialogInstance_t			s_stDialogMessageInstance;


/******************************************************************************/
#define _________Private_Prototype____________________________________________
/******************************************************************************/
static NXUI_DialogInstance_t	*nx_systemrsv_GetDialogPopupContents(void);
static ONDK_Result_t	nx_systemrsv_SelectiveDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_systemrsv_CreateSelectiveDialog(HINT32 message);
static ONDK_Result_t	nx_systemrsv_DestroySelectiveDialog(void);

static ONDK_Result_t	nx_systemrsv_IsShowNotifiedMessage(HUINT32 ulSlotId);
static ONDK_Result_t	nx_systemrsv_StartPowerOffTime(HUINT32 ulSlotId, DxRsvType_e eRsvType);
static ONDK_Result_t	nx_systemrsv_StartPowerOnTime(HUINT32 ulSlotId, DxRsvType_e eRsvType);
static ONDK_Result_t	nx_systemrsv_StartReminder(HUINT32 ulSlotId, DxRsvType_e eRsvType);

static ONDK_Result_t	nx_systemrsv_ActionScheduleReady(HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_systemrsv_ActionScheduleStart(HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_systemrsv_ActionScheduleStop(HINT32 p2, HINT32 p3);
static void 			nx_systemrsv_SendMessageGoStandby(void);

static ONDK_Result_t	nx_systemrsv_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t 	nx_systemrsv_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_systemrsv_MsgGwmHapiMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_systemrsv_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_systemrsv_MsgCoreScheduleNotify(HINT32 p1, HINT32 p2, HINT32 p3);
static ONDK_Result_t	nx_systemrsv_MsgAppDlgClicked(HINT32 lClickedBtn, HINT32 ulDlgWhoseId);
static ONDK_Result_t	nx_systemrsv_MsgAppDlgCanceled(HINT32 ulDlgWhoseId);


/******************************************************************************/
#define _________Private_functions____________________________________________
/******************************************************************************/

/******************************************************************************/
#define __Select_Dialog_Functions_____________________________________________
/******************************************************************************/

static NXUI_DialogInstance_t	*nx_systemrsv_GetDialogPopupContents(void)
{
	return &s_stDialogMessageInstance;
}


static ONDK_Result_t	nx_systemrsv_SelectiveDialogProc(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	return NX_PRISM_DIALOGUE_Message_Proc (nx_systemrsv_GetDialogPopupContents(), message, hAction,p1, p2, p3);
}


static ONDK_Result_t	nx_systemrsv_CreateSelectiveDialog(HINT32 message)
{
	HUINT32 		ulDlgAttr = 0;
	NXUI_DialogInstance_t	*pstDialog = NULL;

	pstDialog = nx_systemrsv_GetDialogPopupContents();
	HxSTD_memset(pstDialog, 0x00, sizeof(NXUI_DialogInstance_t));

	pstDialog->ulDlgWhoseId = eNxSystemRsv_DialogType_PowerOff;
	ulDlgAttr = (NX_DLG_YES_NO|NX_DLG_TYPE_MESSAGE|NX_DLG_TIMEOUT_30SEC);
	NX_PRISM_DIALOGUE_SetDialog(pstDialog, ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis()),
				ulDlgAttr, NULL, (HUINT8 *)message);

	ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, "nx_systemrsv_SelectiveDialogProc",
						APP_DIALOGBOX_PRIORITY,	nx_systemrsv_SelectiveDialogProc,
						(int)NULL, 0, 0, (int)ONDK_GWM_APP_GetAppCallback(ONDK_GWM_APP_GetThis ()));

	NX_COMMON_NOT_USED_PARAM(message);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_systemrsv_DestroySelectiveDialog(void)
{
	ONDK_GWM_APP_Destroy(nx_systemrsv_SelectiveDialogProc);

	return ONDK_RET_OK;
}


/******************************************************************************/
#define __Local_Functions_____________________________________________________
/******************************************************************************/

static ONDK_Result_t	nx_systemrsv_IsShowNotifiedMessage(HUINT32 ulSlotId)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bShowNotiMsg = FALSE;
	HUINT32			ulSvcUid = 0;
	NX_Channel_t	stCurChannel, *pstCurPlayChannel = NULL;
	NX_ChListGroup_t	eChannelGroup = eNxChListGroup_TV;


	hError = NX_SCHEDULER_GetSeriveUidBySlotID(ulSlotId, &ulSvcUid);
	if (hError == ERR_OK)
	{
		// Check same channel in group
		eChannelGroup = NX_PRISM_LIVE_GetCurrentChGroupType();
		pstCurPlayChannel = NX_PRISM_LIVE_GetMasterChannel();
		if (pstCurPlayChannel == NULL)
		{
			NX_APP_Error("Error!!! NX_PRISM_LIVE_GetMasterChannel() is NULL \n");
			return	ONDK_RET_FAIL;
		}

		HxSTD_memset(&stCurChannel, 0x00, sizeof(NX_Channel_t));
		hError = NX_CHANNEL_GROUP_FindChannelBySvcUidInGroup(eChannelGroup, ulSvcUid, &stCurChannel);
		if ((hError == ERR_OK) && (pstCurPlayChannel->svcUid == ulSvcUid))
		{
			NX_APP_Info("Schedule : Same Service : pstCurPlayChannel->svcUid:(x0%x), ulSvcUid:(x0%x)!!\n", pstCurPlayChannel->svcUid, ulSvcUid);
			bShowNotiMsg = FALSE;
		}
		else
		{
			NX_APP_Info("Schedule : Different Service - Notified : pstCurPlayChannel->svcUid:(x0%x), ulSvcUid:(x0%x)!!\n", pstCurPlayChannel->svcUid, ulSvcUid);
			bShowNotiMsg = TRUE;
		}
#if defined(CONFIG_MW_MM_PVR)
	// TODO: [JHLEE] count Recorded, TSR, CHASE playback
#elif defined(CONFIG_MW_MM_MEDIA)
	// TODO: [JHLEE] Count of Media playbakc
#endif
	}
	else
	{
		NX_APP_Error("Error!!! NX_SCHEDULER_GetSeriveUidBySlotID() Fail - ulSlotId:(0x%x) !!\n", ulSlotId);
		bShowNotiMsg = FALSE;
	}

	if (bShowNotiMsg == TRUE)
	{
		return	ONDK_RET_OK;
	}
	else
	{
		return	ONDK_RET_FAIL;
	}

}


static ONDK_Result_t	nx_systemrsv_StartPowerOffTime(HUINT32 ulSlotId, DxRsvType_e eRsvType)
{
	HERROR	hError = ERR_FAIL;
	HBOOL	bStandbyMode = FALSE;


	bStandbyMode = NX_SYSTEM_IsStandbyMode();
	if (bStandbyMode == FALSE)
	{
		// Stop Poweroff schedule because sama timer was action
		hError = NX_SCHEDULER_CancelAllScheduleByType(DxRSVTYPE_POWER_OFF);
		if (hError != ERR_OK)
		{
			NX_APP_Error("Error!!! NX_SCHEDULER_CancelAllScheduleByType() - DxRSVTYPE_POWER_OFF !!\n");
		}

		nx_systemrsv_CreateSelectiveDialog((HINT32)ONDK_GetString(RES_MESG_3204_ID));
	}
	else
	{
		NX_APP_Error("Skip Power Off dialog message in standby mode bStandbyMode :(%d)!!\n", bStandbyMode);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_systemrsv_StartPowerOnTime(HUINT32 ulSlotId, DxRsvType_e eRsvType)
{
	HERROR			hError = ERR_FAIL;
	HBOOL			bStandbyMode = FALSE;
	HINT32			lGroupType = (HINT32)eNxChListGroup_TV;
	HUINT32			ulVolume = 0, ulMaxVolume = 0;
	DxService_t		*pstFindService = NULL, *pstLastService = NULL;
	DxSchedule_t 	stSchedule;
	NX_ChListGroup_t	eFindChGroup = eNxChListGroup_TV;


	bStandbyMode = NX_SYSTEM_IsStandbyMode();
	if (bStandbyMode == TRUE)
	{
		HxSTD_memset(&stSchedule, 0x00, sizeof(DxSchedule_t));
		hError = NX_SCHEDULER_GetScheduleBySlotID(ulSlotId, &stSchedule);
		if (hError == ERR_OK)
		{
			// Get Last Group
			hError = NX_CHANNEL_GetLastServiceGroup(&lGroupType);
			if (hError != ERR_OK)
			{
				NX_APP_Error("\n\n Error!!! NX_CHANNEL_GetLastServiceGroup() !!\n\n\n");
				lGroupType = (HINT32)eNxChListGroup_TV;
				NX_CHANNEL_SetLastServiceGroup(lGroupType);
			}

			// Get Last Service
			pstLastService = NX_CHANNEL_GetLastService();
			if (pstLastService == NULL)
			{
				pstLastService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid((NX_ChListGroup_t)lGroupType, eNxFindSvcDir_First, 0);
				if (pstLastService == NULL)
				{
					pstLastService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_TV, eNxFindSvcDir_First, 0);
					if (pstLastService == NULL)
					{
						pstLastService = NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid(eNxChListGroup_RADIO, eNxFindSvcDir_First, 0);
					}
				}

				if (pstLastService == NULL)
				{
					NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindServiceInGroupByServiceUid() Fail !!\n");
				}
			}

			// Check Schedule Service
			pstFindService = NX_CHANNEL_FindServiceBySvcUid(stSchedule.ulSvcUid);
			if (pstFindService != NULL)
			{
				pstFindService = NX_CHANNEL_GROUP_FindAvailableServiceInGroupList((NX_ChListGroup_t)lGroupType, stSchedule.ulSvcUid, &eFindChGroup);
				if (pstFindService != NULL)
				{
					if ((NX_ChListGroup_t)lGroupType != eFindChGroup)
					{
						NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindAvailableServiceInGroupList() Change Group lGroupType(0x%x) -> eFindChGroup(0x%x) !!\n", lGroupType, eFindChGroup);
						NX_CHANNEL_SetLastServiceGroup((HINT32)eFindChGroup);
					}

					NX_CHANNEL_SaveLastChannel(pstFindService->uid);
				}
			}

			ulVolume = (HUINT32)stSchedule.uExtInfo.stPwr.ucVolume;
			hError= NX_SYSTEM_GetVolumeMax(&ulMaxVolume);
			if (hError != ERR_OK)
			{
				ulMaxVolume = NX_SYS_VOLUME_MAX;
			}

			ulVolume = (ulMaxVolume - NX_SYS_VOLUME_MIN)/NX_SYS_VOLUME_STEP * ulVolume;
			HxLOG_Print("\n\n=====>[JHLEE] NX_SYSTEM_SetVolume() - Volume : (%d) \n\n", ulVolume);
			NX_SYSTEM_SetVolume(ulVolume);
		}
		else
		{
			NX_APP_Error("Error!!! NX_SCHEDULER_GetScheduleBySlotID() !!\n");
		}

		NX_SYSTEM_RequestOperation();
	}
	else
	{
		NX_APP_Error("Error!!! Only action support when STB Standby mode:(%d) !!\n", bStandbyMode);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_systemrsv_StartReminder(HUINT32 ulSlotId, DxRsvType_e eRsvType)
{
	HBOOL			bStandbyMode = FALSE;
	HERROR			hError = ERR_FAIL;
	HINT32			lGroupType = (HINT32)eNxChListGroup_TV;
	DxSchedule_t 	stSchedule;
	DxService_t		*pstFindService = NULL;
	NX_ChListGroup_t	eFindChGroup = eNxChListGroup_TV;
	ONDK_Result_t	eResult = ONDK_RET_FAIL;


	bStandbyMode = NX_SYSTEM_IsStandbyMode();
	if (bStandbyMode == TRUE)
	{
		HxSTD_memset(&stSchedule, 0x00, sizeof(DxSchedule_t));
		hError = NX_SCHEDULER_GetScheduleBySlotID(ulSlotId, &stSchedule);
		if (hError == ERR_OK)
		{
			// Get Last Group
			hError = NX_CHANNEL_GetLastServiceGroup(&lGroupType);
			if (hError != ERR_OK)
			{
				NX_APP_Error("\n\n Error!!! NX_CHANNEL_GetLastServiceGroup() !!\n\n\n");
				lGroupType = (HINT32)eNxChListGroup_TV;
				NX_CHANNEL_SetLastServiceGroup(lGroupType);
			}

			// Check Schedule Service
			pstFindService = NX_CHANNEL_FindServiceBySvcUid(stSchedule.ulSvcUid);
			if (pstFindService != NULL)
			{
				pstFindService = NX_CHANNEL_GROUP_FindAvailableServiceInGroupList((NX_ChListGroup_t)lGroupType, stSchedule.ulSvcUid, &eFindChGroup);
				if (pstFindService != NULL)
				{
					if ((NX_ChListGroup_t)lGroupType != eFindChGroup)
					{
						NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindAvailableServiceInGroupList() Change Group lGroupType(0x%x) -> eFindChGroup(0x%x) !!\n", lGroupType, eFindChGroup);
						NX_CHANNEL_SetLastServiceGroup((HINT32)eFindChGroup);
					}

					NX_CHANNEL_SaveLastChannel(pstFindService->uid);
				}
				else
				{
					NX_APP_Error("Error!!! NX_CHANNEL_GROUP_FindAvailableServiceInGroupList() lGroupType:(0x%x), ulSvcUid:(0x%x) Fail !!\n", lGroupType, stSchedule.ulSvcUid);
				}
			}
			else
			{
				NX_APP_Error("Error!!! NX_CHANNEL_FindServiceBySvcUid() - ulSvcUid:(0x%x) Fail !!\n", stSchedule.ulSvcUid);
			}

			NX_SYSTEM_RequestOperation();
		}
		else
		{
			NX_APP_Error("Error!!! NX_SCHEDULER_GetScheduleBySlotID() - ulSlotId:(0x%x) Fail !!\n", ulSlotId);
		}
	}
	else
	{
		eResult = nx_systemrsv_IsShowNotifiedMessage(ulSlotId);
		if (eResult == ONDK_RET_OK)
		{
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_DIALOGUE_RsvNoti_Proc", APP_DIALOGBOX_PRIORITY,
									(ONDK_GWM_Callback_t)NX_PRISM_DIALOGUE_RsvNoti_Proc, 0, (HINT32)ulSlotId, 0, 0);
		}
		else
		{
			NX_APP_Error("Schedule nx_systemrsv_IsShowNotifiedMessage() Fail !!\n");
		}
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_systemrsv_ActionScheduleReady(HINT32 p2, HINT32 p3)
{
	HUINT32				ulSlotId = (HUINT32)p3;
	DxRsvType_e 		eRsvType = (DxRsvType_e)p2;
	Nx_ScheduleMode_t	eScheduleMode = eNxSchedle_ModeWatching;

	switch (eRsvType)
	{
		case DxRSVTYPE_WATCHING_EBR:
		case DxRSVTYPE_WATCHING_TBR:
		case DxRSVTYPE_WATCHING_SERIES_EBR:
		case DxRSVTYPE_DLNA_TBR:
		case DxRSVTYPE_DLNA_EBR:
			eScheduleMode = eNxSchedle_ModeWatching;
			break;

		case DxRSVTYPE_RECORD_EBR:
		case DxRSVTYPE_RECORD_TBR:
		case DxRSVTYPE_RECORD_SERIES_EBR:
		case DxRSVTYPE_RECORD_SATRECORD_TBR:
		case DxRSVTYPE_KEYWORD_RECORD_NOTSURE:
			eScheduleMode = eNxSchedle_ModeRecord;
			break;

		case DxRSVTYPE_POWER_OFF:
			eScheduleMode = eNxSchedle_ModePowerOff;
			break;

		case DxRSVTYPE_POWER_ON:
			eScheduleMode = eNxSchedle_ModePowerOn;
			break;

		default:
			eScheduleMode = eNxSchedle_ModeUnKnow;
			NX_APP_Error("Error!! Unknow eRsvType:[0x%x] !!\n", eRsvType);
			break;
	}

	if (eScheduleMode == eNxSchedle_ModeWatching)
	{
		NX_APP_Error("Schedule Reay Action skiped (It's action start) - eNxSchedle_ModeWatching !!\n");
		// Reminder process the start only
		return	ONDK_RET_OK;
	}
	else if (eScheduleMode == eNxSchedle_ModeRecord)
	{
		NX_APP_Error("Schedule Reay Action - eNxSchedle_ModeWatching !!\n");
#if defined(CONFIG_MW_MM_PVR)
		// TODO: [JHLEE] Add PVR Action
#endif
	}
	else if (eScheduleMode == eNxSchedle_ModePowerOff)
	{
		NX_APP_Error("Schedule Reay Action - eNxSchedle_ModePowerOff !!\n");
		nx_systemrsv_StartPowerOffTime(ulSlotId, eRsvType);
	}
	else if (eScheduleMode == eNxSchedle_ModePowerOn)
	{
		NX_APP_Error("Schedule Reay Action skiped (It's action start) - eNxSchedle_ModePowerOn !!\n");
	}
	else
	{
		NX_APP_Error("Error!!! Invalid eScheduleMode : (0x%x) !!\n", eScheduleMode);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_systemrsv_ActionScheduleStart(HINT32 p2, HINT32 p3)
{
	HBOOL				bStandbyMode = FALSE;
	HUINT32				ulSlotId = (HUINT32)p3;
	DxRsvType_e 		eRsvType = (DxRsvType_e)p2;
	Nx_ScheduleMode_t	eScheduleMode = eNxSchedle_ModeWatching;


	switch (eRsvType)
	{
		case DxRSVTYPE_WATCHING_EBR:
		case DxRSVTYPE_WATCHING_TBR:
		case DxRSVTYPE_WATCHING_SERIES_EBR:
		case DxRSVTYPE_DLNA_TBR:
		case DxRSVTYPE_DLNA_EBR:
			eScheduleMode = eNxSchedle_ModeWatching;
			break;

		case DxRSVTYPE_RECORD_EBR:
		case DxRSVTYPE_RECORD_TBR:
		case DxRSVTYPE_RECORD_SERIES_EBR:
		case DxRSVTYPE_RECORD_SATRECORD_TBR:
		case DxRSVTYPE_KEYWORD_RECORD_NOTSURE:
			eScheduleMode = eNxSchedle_ModeRecord;
			break;

		case DxRSVTYPE_POWER_OFF:
			eScheduleMode = eNxSchedle_ModePowerOff;
			break;

 		case DxRSVTYPE_POWER_ON:
			eScheduleMode = eNxSchedle_ModePowerOn;
			break;

		default:
			NX_APP_Error("Error!! Unknow eRsvType:[0x%x] !!\n", eRsvType);
			eScheduleMode = eNxSchedle_ModeUnKnow;
			break;
	}

	bStandbyMode = NX_SYSTEM_IsStandbyMode();

	if (eScheduleMode == eNxSchedle_ModeWatching)
	{
		NX_APP_Error("Schedule Action Start - eNxSchedle_ModeWatching !!\n");
		nx_systemrsv_StartReminder(ulSlotId, eRsvType);
	}
	else if (eScheduleMode == eNxSchedle_ModeRecord)
	{
		NX_APP_Error("Schedule Action Start - eNxSchedle_ModeRecord !!\n");
#if defined(CONFIG_MW_MM_PVR)
		// TODO: [JHLEE] Add PVR Action
#endif
	}
	else if (eScheduleMode == eNxSchedle_ModePowerOff)
	{
		NX_APP_Error("Schedule Start Action Skiped (It's action redy) - eNxSchedle_ModePowerOff !!\n");
	}
	else if (eScheduleMode == eNxSchedle_ModePowerOn)
	{
		NX_APP_Error("Schedule Action Start - eNxSchedle_ModePowerOn !!\n");
		nx_systemrsv_StartPowerOnTime(ulSlotId, eRsvType);
	}
	else
	{
		NX_APP_Error("Error!!! Invalid eScheduleMode : (0x%x) !!\n", eScheduleMode);
	}

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_systemrsv_ActionScheduleStop(HINT32 p2, HINT32 p3)
{
	return	ONDK_RET_OK;
}


static void		nx_systemrsv_SendMessageGoStandby(void)
{
	nx_systemrsv_DestroySelectiveDialog();
	ONDK_GWM_SendMessage(NULL, MSG_GWM_KEYDOWN, (Handle_t)NULL, KEY_STANDBY, (int)NULL, (int)NULL);
}


static ONDK_Result_t	nx_systemrsv_MsgGwmCreate(HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_APP_SetUIHideFlag(GWM_UI_Hide_Never);

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_systemrsv_MsgGwmDestroy(HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_systemrsv_MsgGwmHapiMsg(Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_systemrsv_MsgGwmKeyDown(HINT32 p1, HINT32 p2, HINT32 p3)
{
	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_MESSAGE_PASS;
}


static ONDK_Result_t	nx_systemrsv_MsgCoreScheduleNotify(HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_GWM_App_t			pGwmApp = NULL;
	APK_META_SCH_MsgType_e		eMsgType = (APK_META_SCH_MsgType_e)p1;

	switch(eMsgType)
	{
		case eAPK_METASCH_MSG_ScheduleAdded:
			NX_APP_Error("eAPK_METASCH_MSG_ScheduleAdded\n");
			break;
		case eAPK_METASCH_MSG_ScheduleRemoved:
			NX_APP_Error("eAPK_METASCH_MSG_ScheduleRemoved\n");
			break;
		case eAPK_METASCH_MSG_ScheduleChanged:
			NX_APP_Error("eAPK_METASCH_MSG_ScheduleChanged\n");
			break;

		case eAPK_METASCH_MSG_TimeUpReady:
			NX_APP_Error("eAPK_METASCH_MSG_TimeUpReady\n");
			nx_systemrsv_ActionScheduleReady(p2, p3);
			break;
		case eAPK_METASCH_MSG_TimeUpStart:
			NX_APP_Error("eAPK_METASCH_MSG_TimeUpStart\n");
			nx_systemrsv_ActionScheduleStart(p2, p3);
			break;
		case eAPK_METASCH_MSG_TimeUpStop:
			NX_APP_Error("eAPK_METASCH_MSG_TimeUpStop\n");
			nx_systemrsv_ActionScheduleStop(p2, p3);
			break;

		case eAPK_METASCH_MSG_ConflictScheduleAdded:
			NX_APP_Error("eAPK_METASCH_MSG_ConflictScheduleAdded\n");
			pGwmApp = ONDK_GWM_APP_Get((ONDK_GWM_Callback_t)NX_PRISM_DIALOGUE_RsvConflict_Proc);
			if (pGwmApp != NULL)
			{
				ONDK_GWM_APP_Destroy((ONDK_GWM_Callback_t)NX_PRISM_DIALOGUE_RsvConflict_Proc);
			}
			ONDK_GWM_APP_Create(ONDK_UPPER_WINDOW, (HCHAR*)"NX_PRISM_DIALOGUE_RsvConflict_Proc", APP_DIALOGBOX_PRIORITY,
									(ONDK_GWM_Callback_t)NX_PRISM_DIALOGUE_RsvConflict_Proc, 0, (HINT32)p1, 0, 0);
			break;
		case eAPK_METASCH_MSG_AlternativeScheduleAdded:
			NX_APP_Error("eAPK_METASCH_MSG_AlternativeScheduleAdded\n");
			break;

		default:
			NX_APP_Error("Invalid Schedule Type:(%u)\n", eMsgType);
			break;
	}

	NX_COMMON_NOT_USED_PARAM(p1);
	NX_COMMON_NOT_USED_PARAM(p2);
	NX_COMMON_NOT_USED_PARAM(p3);

	return	ONDK_RET_OK;
}


static ONDK_Result_t	nx_systemrsv_MsgAppDlgClicked(HINT32 lClickedBtn, HINT32 ulDlgWhoseId)
{
	switch (lClickedBtn)
	{
		case eNX_DLG_CLICKED_BTN_LEFT:	// YES
			if (ulDlgWhoseId == eNxSystemRsv_DialogType_PowerOff)
			{
				nx_systemrsv_DestroySelectiveDialog();
				nx_systemrsv_SendMessageGoStandby();
			}
			break;
		default:
			if (ulDlgWhoseId == eNxSystemRsv_DialogType_PowerOff)
			{
				nx_systemrsv_DestroySelectiveDialog();
			}
			break;
	}

	return ONDK_MESSAGE_BREAK;
}


static ONDK_Result_t	nx_systemrsv_MsgAppDlgCanceled(HINT32 ulDlgWhoseId)
{
	if (ulDlgWhoseId == eNxSystemRsv_DialogType_PowerOff)
	{
		nx_systemrsv_DestroySelectiveDialog();
	}

	return ONDK_RET_OK;
}


/******************************************************************************/
#define _________Public_Functions_____________________________________________
/******************************************************************************/
ONDK_Result_t	NX_PRISM_SYSTEM_RSV_Proc(HINT32 lMessage, Handle_t handle, HINT32 p1, HINT32 p2, HINT32 p3)
{
	ONDK_Result_t		eResult = ONDK_RET_OK;

	switch (lMessage)
	{
		case MSG_GWM_CREATE:
			NX_APP_Message("MSG_GWM_CREATE\n");
			eResult = nx_systemrsv_MsgGwmCreate(p1, p2, p3);
			break;
		case MSG_GWM_KEYUP:
			break;
		case MSG_GWM_KEYDOWN:
			NX_APP_Message("MSG_GWM_KEYDOWN\n");
			eResult = nx_systemrsv_MsgGwmKeyDown(p1, p2, p3);
			break;
		case MSG_GWM_DESTROY:
			NX_APP_Message("MSG_GWM_DESTROY\n");
			eResult = nx_systemrsv_MsgGwmDestroy(p1, p2, p3);
			break;
		case MSG_GWM_HAPI_MESSAGE:
			NX_APP_Message("MSG_GWM_HAPI_MESSAGE\n");
			eResult = nx_systemrsv_MsgGwmHapiMsg(handle, p1, p2, p3);
			break;

		case MSG_CORE_SCHEDULE_NOTIFY:
			NX_APP_Message("MSG_CORE_SCHEDULE_NOTIFY\n");
			eResult = nx_systemrsv_MsgCoreScheduleNotify(p1, p2, p3);
			break;

		case MSG_APP_DLG_CLICKED:
		case MSG_APP_DLG_TIMEOUT:
			NX_APP_Message("MSG_APP_DLG_CLICKED || MSG_APP_DLG_TIMEOUT\n");
			eResult = nx_systemrsv_MsgAppDlgClicked(p1, p2);
			break;
		case MSG_APP_DLG_CANCELED:
			NX_APP_Message("MSG_APP_DLG_CANCELED\n");
			eResult = nx_systemrsv_MsgAppDlgCanceled(p2);
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


/******************************************************************************/
#define _________End_Of_File____________________________________________________
/******************************************************************************/
