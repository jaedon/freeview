/**
	@file     mgr_action.c
	@brief

	Description :			\n
	Module :				\n
	Remarks :				\n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/04/16		init							\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <string.h>

#include <octo_common.h>

#include <db_svc.h>

#include <mgr_appmgr.h>
#include <mgr_action.h>
#include <mgr_init.h>
#include <pmgr_action.h>
#include <hapi.h>
#include <papi.h>
#include <sapi.h>
#include <napi.h>
#include <mgr_epg.h>
#include <mgr_coldboot.h>
#include <mgr_storage.h>
#include <mgr_setting.h>
#include <mgr_swup.h>
#include <mgr_time.h>
#include <mgr_search.h>
#include <mgr_epggathering.h> /* fix prevent #176689~90, #189447~8 (Warning) */
#include <mgr_dynamicupdate.h> /* fix prevent #198895~6 (Warning) */
#include <svc_sys.h>
#include <svc_output.h>
#if defined(CONFIG_MW_CEC)
#include <svc_cec.h>
#endif
#include <svc_fs.h>

#include "./local_include/_mgr_action.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions            ********************/
/*******************************************************************/

HERROR MGR_ACTION_StartNormal(HBOOL bOperation)
{
	return pmgr_action_NormalStart(bOperation);
}

HBOOL MGR_ACTION_IsAvailableNormal(void)
{
	return pmgr_action_NormalIsAvailable();
}


HUINT32		mgr_action_GetMainViewId (void)
{
	return pmgr_action_GetMainViewId();
}

void		mgr_action_SetMainViewId (HUINT32 ulViewId)
{
	pmgr_action_SetMainViewId (ulViewId);
}

MgrAction_State_e	mgr_action_GetState (void)
{
	return pmgr_action_GetState();
}

STATIC void mgr_action_SetState (MgrAction_State_e eState)
{
	pmgr_action_SetState (eState);
}

MgrAction_SvcDB_e	MGR_ACTION_GetSvcDbState (void)
{
	return pmgr_action_GetSvcDbState();
}

void		mgr_action_SetSvcDbState (MgrAction_SvcDB_e eState)
{
	pmgr_action_SetSvcDbState(eState);
}

STATIC HUINT32 mgr_action_GetWakeupRcuKey(void)
{
	return pmgr_action_GetWakeupRcuKey();
}

HERROR mgr_action_RegisterSiTimeoutCallback(void)
{
	return pmgr_action_RegisterSiTimeoutCallback();
}

#define _________MGR_ACTION_PAMA_CONTROL
typedef	struct
{
	HBOOL					 bActionTimeRecorveryStart;
	HBOOL					 bActionTimeRecorveryEnd;

	HBOOL					 bActionBatchStart;
	HBOOL					 bActionBatchEnd;
} mgr_action_Contents_t;


STATIC void mgr_action_ShutdownStorage(void)
{
#define UNMOUNT_CHECK_MAX_COUNT 20
	HUINT32 ulRetryCount = 0;

	while(ulRetryCount++ < UNMOUNT_CHECK_MAX_COUNT)
	{
		HUINT32 i =0;
		for(i = 0; i < eSVC_FS_DEV_MAX; i++)
		{
			SvcFs_DevInfo_t stMwDevInfo;
			if(SVC_FS_GetDeviceInfo(i, &stMwDevInfo) == ERR_OK)
			{
				break;
			}
		}

		if(i == eSVC_FS_DEV_MAX && SVC_FS_CheckStoragePowerOff() == TRUE)
		{
			break;
		}
		else
		{
			HxLOG_Print("Internal HDD Unmaount Waitting Count : %d mounted device[%d] stroragePower[%d]\n", ulRetryCount, i, SVC_FS_CheckStoragePowerOff());
		}

		VK_TASK_Sleep(1000);
	}

	//if(ulRetryCount == UNMOUNT_CHECK_MAX_COUNT)
	{
		HxLOG_Warning("Internal HDD Unmaount Waitting Count : %d poweroff : %d\n", ulRetryCount, SVC_FS_CheckStoragePowerOff());
	}
#undef UNMOUNT_CHECK_MAX_COUNT

}

STATIC void mgr_action_ShutdownNetwork(void)
{
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
	HUINT32 starttick = 0;
	HUINT32 duration = 0;
	HINT32 timeout = 5000;
	HINT32 network_status = 0;

	// wait network shutdown
	starttick = HLIB_STD_GetSystemTick();
	while( 5 != network_status )
	{
		HxLOG_Print("network wait shutdown : duration : %d\n", duration);
		if(duration > timeout)
		{
			HxLOG_Print("network wait shutdown timeout!! \n");
			break;
		}

		NAPI_NetConf_GetStatus(&network_status);
		duration = HLIB_STD_GetSystemTick() - starttick;
		HLIB_STD_TaskSleep(10);
	}
#endif
}

#if 1	// pama로 옮겨가야 할 부분.
STATIC HBOOL mgr_action_WakeupReson(void)
{
	HSystemBootupStatus_t	stBootupReason;
	WakeUpSecondReason_t wakeUpSecondReason;
	WakeUpReason_t		sWakeUpReason;

	HxLOG_Trace();

	HxSTD_MemSet(&stBootupReason, 0, sizeof(HSystemBootupStatus_t));
	SVC_SYS_GetWakeUpReason(&sWakeUpReason);
	SVC_SYS_GetWakeUpSecondReason(&wakeUpSecondReason);

	switch (sWakeUpReason)
	{
		case eWAKEUP_BY_ACPOWER:
		case eWAKEUP_BY_REBOOT:
			{
				HINT32	eStandby = 0;
				HERROR	hErr;
				hErr = PAPI_GetLastStandby(&eStandby);
				if(hErr !=ERR_OK)
				{
					HxLOG_Error("[%s] PAPI_GetLastStandby() Error \n", __FUNCTION__);
				}
				stBootupReason.eBootUpReason = eHBootUpReason_POWER;
				stBootupReason.eSystemCurrentMode = ((eStandby == 1) ? eHSystemOperationMode_SLEEP : eHSystemOperationMode_NORMAL);
				HxLOG_Info("[%s] WakeUp Reason is ACPOWER or REBOOT : STB Last Mode is [%d]\n", __FUNCTION__, eStandby);

			}
			break;
		case eWAKEUP_BY_TIMER:
			SVC_SYS_DisableWakeUpTime();
			stBootupReason.eBootUpReason = eHBootUpReason_TIMER;
			stBootupReason.eSystemCurrentMode = eHSystemOperationMode_SLEEP;
			break;
		default:
			stBootupReason.eBootUpReason = eHBootUpReason_USER;
			stBootupReason.eSystemCurrentMode = eHSystemOperationMode_NORMAL;
			DB_PARAM_Sync();
			break;
	}

	HxLOG_Print("[%s] Send eHSIG_SystemBootupState \tWakeup Reson : %s \tCurrent Mode : %s\n" \
		,__FUNCTION__ \
		,stBootupReason.eBootUpReason == eHBootUpReason_POWER ? "eHBootUpReason_POWER" \
		:stBootupReason.eBootUpReason == eHBootUpReason_TIMER ? "eHBootUpReason_TIMER" : "eHBootUpReason_USER" \
		,stBootupReason.eSystemCurrentMode == eHSystemOperationMode_SLEEP ? "eHSystemOperationMode_SLEEP" :  "eHSystemOperationMode_NORMAL"\
		);
#if defined (CONFIG_PROD_DIGIMAXT2) // TODO: this is temporary code that will be deleted.
	stBootupReason.eSystemCurrentMode = eHSystemOperationMode_NORMAL;
#endif

	HAPI_BroadcastSignal("signal:onBootupStateChanged", 1, "b", &stBootupReason, sizeof(HSystemBootupStatus_t));

	if(stBootupReason.eSystemCurrentMode == eHSystemOperationMode_NORMAL)
		return TRUE;
	else
		return FALSE;
}

// Not implement In Pama Process
STATIC HERROR mgr_action_pamaShutdown(HBOOL	bPowerSavingStandby)
{
	HxLOG_Trace();
	return SVC_SYS_TurnOffMainBoard(bPowerSavingStandby);
}

// Not implement In Pama Process
STATIC void mgr_action_pamaReboot(void)
{
	mgr_action_ShutdownStorage();
#if defined (CONFIG_OP_TDC)
	// 관련 Issue : After downloading OTA screen goes black for 30 secs (https://yousee.jira.com/browse/NHYS-610)
	// 실제 reboot action이 시작되는 아래 function 호출 전에 화면 Off
	SVC_OUT_SetAvEnable(FALSE);
#endif
	SVC_SYS_Reboot();
}
#endif


STATIC void mgr_action_Reboot(void)
{
	HxLOG_Error("eMEVT_ACTION_REBOOT_STB\n");
#if defined (CONFIG_OP_TDC)
	// Not block AV this time, AV Block when doing real reboot action.
	// 관련 Issue : After downloading OTA screen goes black for 30 secs (https://yousee.jira.com/browse/NHYS-610)
	// OTA시 Reboot 후 Upgrader 화면이 뜨는데 까지 오래걸린다는 이슈로, 여기서 화면을 Off 하지 않고,
	// 실제 reboot 수행할때 off 한다.  이럴 경우 화면은 보이지만 중간에 control 할수 없는 구간이 발생한다.
	// 이를 TDC에 알렸고, 확인받고 적용함.
#else
	SVC_OUT_SetAvEnable(FALSE);
#endif
	SVC_SYS_Deactivate_SC();
}

STATIC void mgr_action_ShutdownDevice(HBOOL	bPowerSavingStandby,HBOOL bWifiPowerOnOff)
{
	HERROR	hErr = ERR_FAIL;

	mgr_action_ShutdownStorage();
	mgr_action_ShutdownNetwork();
	hErr = SVC_SYS_SetWakeOnLAN(bPowerSavingStandby == FALSE ? TRUE: FALSE);
	HxLOG_Print("mgr_action_shutdown SVC_SYS_SetWakeOnLAN\n");
	SVC_SYS_Deactivate_SC();
	SVC_SYS_WIFIPower(bWifiPowerOnOff);
	HxLOG_Print("mgr_action_shutdown finish\n");
}

#define _________MGR_ACTION_BATCH
STATIC HERROR mgr_action_BatchInitContents(mgr_action_Contents_t *pstContents)
{
	if (pstContents == NULL )
		return ERR_FAIL;

	pstContents->bActionBatchStart			=	FALSE;
	pstContents->bActionBatchEnd			=	FALSE;
	return ERR_OK;
}

STATIC HBOOL mgr_action_CheckBatchProc(mgr_action_Contents_t *pstContents)
{
	// run Batch_Proc
	HxLOG_Debug("pstContents->bActionBatchStart: %d, pstContents->bActionBatchEnd: %d \n", pstContents->bActionBatchStart, pstContents->bActionBatchEnd);


	if(MGR_ACTION_GetBatchContext() != NULL
		&& pstContents->bActionBatchStart == TRUE
		&& pstContents->bActionBatchEnd == TRUE
		)
	{
		pstContents->bActionBatchStart = FALSE;
		pstContents->bActionBatchEnd = FALSE;
	}

	HxLOG_Debug("pstContents->bActionBatchStart: %d, pstContents->bActionBatchEnd: %d \n", pstContents->bActionBatchStart, pstContents->bActionBatchEnd);


	if(pstContents->bActionBatchStart == FALSE)
	{
		pstContents->bActionBatchStart = TRUE;
		MGR_ACTION_BatchInstallStandbyProc();
		if(MGR_ACTION_BatchStart (eDxBAT_TYPE_STANDBY) == ERR_FAIL)
		{
			pstContents->bActionBatchEnd = TRUE;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else if(pstContents->bActionBatchEnd == FALSE)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

STATIC void mgr_action_FinishBatchProc(mgr_action_Contents_t *pstContents, DxBatchType_e eBatchType, HBOOL bFail)
{
	// run Batch_Proc
	HxLOG_Trace();

	if(eBatchType != eDxBAT_TYPE_STANDBY)
		return;

	if(bFail == 0) // Succed
	{
		pstContents->bActionBatchEnd = TRUE;
	}
	else // fail
	{
		pstContents->bActionBatchStart = FALSE;
		pstContents->bActionBatchEnd = FALSE;
	}
}


#define _________MGR_ACTION_SHUTDOWN_CONTROL
STATIC HBOOL mgr_action_CheckTime(mgr_action_Contents_t *pstContents)
{
	unsigned long	ulUnixTime;
	HUINT32			ulRes;
	HxLOG_Trace();
	ulRes = VK_CLOCK_GetTime(&ulUnixTime);

	if ( ulRes != VK_OK  || HLIB_DATETIME_IsValidUnixTime(ulUnixTime) != ERR_OK )
	{
		HxLOG_Print("[%s] Time is Invalid\n", __FUNCTION__);
		if(pstContents->bActionTimeRecorveryStart == FALSE)
		{
			HxLOG_Print("ClockRecorvery Start Return FALSE\n");
			pstContents->bActionTimeRecorveryStart = TRUE;
			MGR_TIME_ClockRecoverStart();
			return FALSE;
		}
		else if(pstContents->bActionTimeRecorveryEnd == FALSE)
		{
			HxLOG_Print("ClockRecorvery Running  Return FALSE\n");
			return FALSE;
		}
		else
		{
			HxLOG_Print("ClockRecorvery Finished Return TRUE\n");
			return TRUE;
		}
	}
	else
	{
		HxLOG_Print("Time is OK Return TRUE\n");
		return TRUE;
	}
}
#define TURNOFF_TIME	20
STATIC HBOOL mgr_action_IsSchedule(mgr_action_Contents_t *pstContents)
{
	DxSchedule_t	stSchedule;
	UNIXTIME		ulUnixTime = HLIB_STD_GetSystemTime();
	UNIXTIME		ulWakeUpUnixTime = 0;
	HERROR		hErr = ERR_FAIL;

	if(HLIB_DATETIME_IsValidUnixTime(ulUnixTime) == ERR_FAIL)
	{
		HxLOG_Debug("system time is not yet valid \n");
		return FALSE;
	}

	if(SAPI_IsScheduleLoaded() == FALSE)
	{
		// 만약 schedule로 부팅 후, standby 떨어지기 전에 곧 시작될 schedule이 있으면, sama 에서 wait shutdown 날릴 것임.
		HxLOG_Error("schedule data is not yet loaded. wait shutdown! \n");
		return TRUE;
	}

	hErr = SAPI_GetEarliestScheduleWithPaddingAndReadyTime(&stSchedule, &ulWakeUpUnixTime);
	if(hErr == ERR_OK && ulWakeUpUnixTime != 0)
	{

#if defined(CONFIG_DEBUG)
		HxDATETIME_t dateTime;
		HxDATETIME_t dateTimeWithPaddingAndRsvTime;


		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUnixTime, &dateTime);

		HxLOG_Debug("NOW Time[%d:%d:%d]\n"
			, dateTime.stTime.ucHour, dateTime.stTime.ucMinute, dateTime.stTime.ucSecond);

		HxLOG_Debug("stSchedule.eStatus: %d, slot: %d, type: %d \n", stSchedule.eStatus, stSchedule.ulSlot, stSchedule.eRsvType);

		HxLOG_Debug("SCH Time[%d:%d:%d] \n"
			, stSchedule.stStartTime.stTime.ucHour, stSchedule.stStartTime.stTime.ucMinute, stSchedule.stStartTime.stTime.ucSecond);

		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulWakeUpUnixTime, &dateTimeWithPaddingAndRsvTime);

		HxLOG_Debug("SCH Time[%d:%d:%d] (with padding and rsv ready)\n"
			, dateTimeWithPaddingAndRsvTime.stTime.ucHour, dateTimeWithPaddingAndRsvTime.stTime.ucMinute, dateTimeWithPaddingAndRsvTime.stTime.ucSecond);
#endif

		if((stSchedule.eStatus == DxRSVSTATUS_READY || stSchedule.eStatus == DxRSVSTATUS_RUNNING)
					// watching 이 running 인데 standby 가 들어오면 standby로..
			&& !(stSchedule.eStatus == DxRSVSTATUS_RUNNING && (stSchedule.eRsvType == DxRSVTYPE_WATCHING_EBR || stSchedule.eRsvType == DxRSVTYPE_WATCHING_TBR || stSchedule.eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR))
			)
		{
			return TRUE;
		}

//		hErr = HLIB_DATETIME_ConvertDateTimeToUnixTime(&(stSchedule.stStartTime), &ulWakeUpUnixTime);
//		if(hErr == ERR_OK)
		{
//			ulWakeUpUnixTime -= stSchedule.eRsvReady;
			ulWakeUpUnixTime -= SAPI_GetExtraBootTime();

#if defined(CONFIG_DEBUG)
			{
				HxDATETIME_t dateTimeWakeUpUnix;

				HLIB_DATETIME_ConvertUnixTimeToDateTime(ulWakeUpUnixTime, &dateTimeWakeUpUnix);
				HxLOG_Error("Wakeup Time[%d:%d:%d]\n", dateTimeWakeUpUnix.stTime.ucHour, dateTimeWakeUpUnix.stTime.ucMinute, dateTimeWakeUpUnix.stTime.ucSecond);
			}
#endif
			if(ulUnixTime >= ulWakeUpUnixTime)
			{
				HxLOG_Error("Wakeup Time is Already Past. So STB Does Not turn off.\n");
				return TRUE;
			}
			else if(ulUnixTime + TURNOFF_TIME >= ulWakeUpUnixTime )
			{
				HxLOG_Debug("It can't Wakeup at that Time cause of Shutdown Time. STB Does Not turn off.\n");
				return TRUE;
			}
			else
			{
				HxLOG_Error("Set Wakeup Time. >>>\n");
				(void)SVC_SYS_SetWakeUpTime(ulWakeUpUnixTime);
			}
		}
	}
	else
	{
		HxLOG_Error("There is No Schedule. >>> \n");
	}

	return FALSE;
}





STATIC HBOOL mgr_action_CheckWaitShutdown(void)
{
	HxLOG_Trace();
	return pmgr_action_CheckWaitShutdown();
}
STATIC HERROR	mgr_action_InitContents(mgr_action_Contents_t *pstContents)
{
	HxLOG_Trace();

	if (pstContents == NULL )
		return ERR_FAIL;

	pstContents->bActionTimeRecorveryStart 	= 	FALSE;
	pstContents->bActionTimeRecorveryEnd	= 	FALSE;
	pstContents->bActionBatchStart 			= 	FALSE;
	pstContents->bActionBatchEnd			= 	FALSE;
	return ERR_OK;
}

STATIC HBOOL mgr_action_SetMicom(void)
{
	HERROR			hErr;
	unsigned long	ulUnixTime;
	HxLOG_Trace();

	hErr = VK_CLOCK_GetTime(&ulUnixTime) ;
	if ( VK_OK == hErr	&& HLIB_DATETIME_IsValidUnixTime(ulUnixTime) == ERR_OK )
	{
		// Set Rtc
		SVC_SYS_SetRtcTime(ulUnixTime);
	}
	HxLOG_Print("[%s] return TRUE\n", __FUNCTION__);
	return TRUE;
}


STATIC void	mgr_action_RemoveScheduleBGDescramble(void)
{
	HUINT32 		ulCnt = 0, ulNumSch = 0, ulNumSch2 = 0;
	DxSchedule_t	*pstSchArray = NULL;
	HERROR			hErr = ERR_FAIL;

	hErr = SAPI_CountScheduleByType(DxRSVTYPE_DESC_REC_STANDBY, &ulNumSch);
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("Fail to Count Schedules.\n");
		return;
	}
	HxLOG_Debug("Schedule Count ... (%d) \n", ulNumSch);
	if(ulNumSch > 0)
	{
		pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
		if (pstSchArray)
		{
			hErr = SAPI_GetAllSchedulesByRsvType(ulNumSch, &ulNumSch2, pstSchArray, DxRSVTYPE_DESC_REC_STANDBY);
			if(hErr == ERR_OK)
			{
				for (ulCnt = 0; ulCnt < ulNumSch2; ulCnt++)
				{
					DxSchedule_t	*pstSchedule = &(pstSchArray[ulCnt]);
					if(pstSchedule != NULL && pstSchedule->eRsvType == DxRSVTYPE_DESC_REC_STANDBY)
					{
						HxLOG_Debug("Cancel Schedule Slot (%d) \n", pstSchedule->ulSlot);
						SAPI_CancelSchedule(pstSchedule->ulSlot);
					}
				}
			}
			HLIB_STD_MemFree(pstSchArray);
		}
	}
}

STATIC HBOOL mgr_action_StartShutDownProc(mgr_action_Contents_t *pstContents)
{
	// 순서 유지.
	if( mgr_action_CheckTime(pstContents) == FALSE
		|| mgr_action_IsSchedule(pstContents) == TRUE
		|| mgr_action_CheckBatchProc(pstContents) == FALSE
		)
	{
#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP) && defined(CONFIG_MW_CAS_NAGRA_OP_SES_CAKTEST)
		// 2015.11 - 현재로써는 이 방법이 최선인듯...
		// mgr_action_CheckBatchProc() 에서 wakeup schedule 등록하고, 다시 여기로 와서
		// mgr_action_IsSchedule() 함수에서 wakeup timer 를 셋팅해야 하기 때문이다.
		HxLOG_Error("[CAKTEST][EMMWAKEUP] >>> MGR_ACTION_WaitShutDown() 4sec 로 변경 ========================> \n\n\n");
		MGR_ACTION_WaitShutDown(4000);
#else
		MGR_ACTION_WaitShutDown(6000);
#endif
		return FALSE;
	}

	// shutdown 시 모든 것이 완료된 상태.
	mgr_action_CheckWaitShutdown();
	return TRUE;
}

#if defined(CONFIG_MW_CEC_RECEIVE_POWER_OFF)
STATIC void mgr_action_CEC_PowerOff(void)
{
	HBOOL				bEnable = FALSE;
	HERROR              hErr;

	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_ENABLE, (HUINT32 *)&bEnable, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_HDMI_CEC_ENABLE) Error!!!\n");
	}

	/* check CEC on */
	if (bEnable == TRUE)
	{
	        HxLOG_Warning("********* CEC RECEIVE POWER OFF (Request ShutDown) ***********\n");
	        PAPI_RequestShutDown();
	}
}
#endif


#define ________________MSG_PROCESS_FUNCTIONS
STATIC BUS_Result_t mgr_action_onSignalBusProcess(HINT32 nMessage, Handle_t hAction, HINT32 nParam1, HINT32 nParam2, HINT32 nParam3)
{
	PxSTATUS_e	ePapiStatus;
	BUS_Callback_t	pfnColdBoot;

	HxLOG_Assert(nMessage == eMEVT_BUS_SIGNAL);

	ePapiStatus = nParam1;
	pfnColdBoot = BUS_MGR_GetMgrCallback(BUS_MGR_Get(MGR_ColdBoot_GetProc()));
	if (pfnColdBoot)
		HxLOG_Warning("Now in Cold-Boot Operation --> PAMA message will be processed only in coldboot proc\n");

	if(ePapiStatus == ePAMA_STATUS_Operation || ePapiStatus == ePAMA_STATUS_Reboot || ePapiStatus == ePAMA_STATUS_Shutdown)
	{
		DAPI_Sync(DxDBSETUP_SYSTEM);
	}

	if(ePapiStatus == ePAMA_STATUS_Operation)
	{
		HxLOG_Warning("eMEVT_BUS_GO_OPERATION\n");
		BUS_PostMessage(pfnColdBoot, eMEVT_BUS_GO_OPERATION, (Handle_t)0, 0, 0, 0);
	}
	else if(ePapiStatus == ePAMA_STATUS_Reboot)
	{
		HxLOG_Warning("eMEVT_ACTION_REBOOT_STB\n");
		BUS_PostMessage(pfnColdBoot, eMEVT_ACTION_REBOOT_STB, (Handle_t)0, 0, 0, 0);
	}
	else if(ePapiStatus == ePAMA_STATUS_Shutdown)
	{
		MgrAction_State_e eAmState = eAmState_IDLE;
		eAmState = mgr_action_GetState();
		HxLOG_Warning("eMEVT_BUS_GO_SHUTDOWN (%d)\n", eAmState);
		if(eAmState == eAmState_NORMAL
			|| eAmState == eAmState_IDLE)
		{
			BUS_PostMessage(pfnColdBoot, eMEVT_BUS_READY_SHUTDOWN, (Handle_t)0, 0, 0, 0);
		}
		else
		{
			BUS_PostMessage(NULL, eMEVT_BUS_GO_SHUTDOWN, (Handle_t)0, 0, 0, 0);
		}
	}
	else if(ePapiStatus == ePAMA_STATUS_RebootDone)
	{
		HxLOG_Warning("ePAMA_STATUS_RebootDone\n");
		BUS_PostMessage (NULL, eMEVT_BUS_REBOOT_DONE, (Handle_t)0, 0, 0, 0);
	}
	else if(ePapiStatus == ePAMA_STATUS_ShutdownDone)
	{
		HxLOG_Warning("ePAMA_STATUS_ShutdownDone\n");
		BUS_PostMessage(NULL, eMEVT_BUS_SHUTDOWN_DONE, (Handle_t)0, 0, 0, 0);
	}
	else
	{
		HxLOG_Error("ePAMA_STATUS_Unknown\n");
	}

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t mgr_action_EvtSearchStarted (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	//--------------------------------------------------------
 	PROC_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t mgr_action_EvtSearchFinished (HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{

	//--------------------------------------------------------
 	PROC_PARAM_WARNING_REMOVE(message, hAction, p1, p2, p3);
	return ERR_BUS_NO_ERROR;
}





#define ________________MGR_ACTION_S3MODE

STATIC HERROR mgr_action_CheckS3Mode(HINT32 nSleepMode,HINT32 nBootMode,HBOOL bPowerSavingStandby)
{
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
	HUINT32 ulRetryCount = 0;
	HxLOG_Print("mgr_action_CheckS3Mode\n");

#if !defined(CONFIG_SUPPORT_S3MODE_FASTBOOT_WITHOUT_MICOM)
	if(bPowerSavingStandby == TRUE)
		return ERR_FAIL;
#endif

	do
	{
		if(SVC_SYS_PrepareFastBootStandby() == ERR_OK)
		{
			HxLOG_Print("SVC_SYS_PrepareFastBootStandby ERR_OK\n");
			if(SVC_SYS_SetFastBootStandby(nSleepMode,nBootMode,bPowerSavingStandby) == ERR_OK)
			{
				HxLOG_Error("SVC_SYS_SetFastBootStandby ERR_OK\n");
				return ERR_OK;
			}
		}
		else
		{
			SVC_SYS_CancelFastBootStandby();
			HxLOG_Print("SVC_SYS_CancelFastBootStandby\n");
		}

		VK_TASK_Sleep (1000);
		if(ulRetryCount == 5)
			break;

		HxLOG_Print("Retry Count [%d]\n", ulRetryCount);
	}while(ulRetryCount++);
#endif
	return ERR_FAIL;
}


STATIC void	mgr_action_WakeupFromS3Mode(mgr_action_Contents_t	*pstContents)
{
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
	HxLOG_Print("mgr_action_CheckS3Mode SUCCESS Go Operation\n");
	SVC_SYS_MountNandFlash();
	SVC_SYS_SetWakeOnLAN(FALSE);
	SVC_SYS_PwrInit();
	SVC_SYS_WIFIPower(TRUE); // Operation 상태에서는 항상 power-on
	mgr_action_InitContents(pstContents);
	mgr_action_WakeupReson();
	MGR_ACTION_BatchInstallColdBootProc();
	MGR_ACTION_BatchStart (eDxBAT_TYPE_COLDBOOT);
#endif
}

#define ________________MGR_ACTION_PROC
STATIC mgr_action_Contents_t			s_stContents;

STATIC INLINE mgr_action_Contents_t*	mgr_action_GetContents(void)
{
	return &s_stContents;
}

STATIC void mgr_action_StartSubAction(void)
{
	MGR_EPG_Start (eEpgKind_RF, 0, 0);
	MGR_EPG_Start (eEpgKind_HumaxIp, eEpgStartType_Normal, eEpgCallType_NomalAction);
	MGR_STORAGE_Start();

#if	defined(CONFIG_APCORE_ALWAYS_ACTIVE_STANDBY)
	MGR_STORAGE_IntHddPowerMonitorStart();
#endif
	MGR_SETTING_Start();
	MGR_ACTION_BatchInstallColdBootProc();
	MGR_ACTION_BatchStart (eDxBAT_TYPE_COLDBOOT);
}

STATIC void	mgr_action_TurnOff(HBOOL	bPowerSavingStandby)
{
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
	HxLOG_Print("mgr_action_CheckS3Mode FAIL Go Shutdown\n");
#if defined(CONFIG_MW_SYS_ACTIVESTBY_BY_PWRSAVINGONSTBY)
	SVC_SYS_SetPanelTimeDisplay(TRUE); // Active Standby 에서는 무조건 Time Display가 동작 하도록 한다.
#else
	SVC_SYS_SetPanelTimeDisplay((bPowerSavingStandby) ? FALSE : TRUE);
#endif
#endif

#if (defined(CONFIG_PROD_ICORDPRO) || defined(CONFIG_PROD_HDNEO)) && defined(CONFIG_MW_SYS_COLOR_LED)
	if(bPowerSavingStandby == TRUE)
	{
		SVC_SYS_SetPowerLedState(ePOWER_LED_OFF);
	}
#endif
	mgr_action_pamaShutdown(bPowerSavingStandby);
}

STATIC BUS_Result_t mgr_action_MsgCreate(HINT32 nMsg, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("p1:[0x%X], p2:[0x%X], p3:[0x%X]\n",p1, p2, p3);
	MGR_INIT_Module(TRUE);
	mgr_action_WakeupReson();
	mgr_action_StartSubAction();
	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t mgr_action_MsgGoOperation(HINT32 nMsg, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("p1:[0x%X], p2:[0x%X], p3:[0x%X]\n",p1, p2, p3);
#if defined(CONFIG_MW_CI_PLUS)
	mgr_action_RemoveScheduleBGDescramble();
#endif
#if defined(CONFIG_SUPPORT_S3MODE_FASTBOOT)
	SVC_SYS_SetWakeOnLAN(FALSE);
	SVC_SYS_ClearWakeupRcuKey();
#endif
#if defined(CONFIG_MW_CEC)
	SVC_CEC_SetPowerState(TRUE);
	SVC_CEC_PowerOnTv();
#endif
	MGR_ACTION_BatchInstallUserProc();
	MGR_ACTION_BatchStart (eDxBAT_TYPE_USER);

	return ERR_BUS_NO_ERROR;
}

STATIC BUS_Result_t mgr_action_MsgReadyShutdown(HINT32 nMsg, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HxLOG_Debug("p1:[0x%X], p2:[0x%X], p3:[0x%X]\n",p1, p2, p3);
	SVC_SYS_SetWakeupRcuKey(mgr_action_GetWakeupRcuKey());
	mgr_action_SetMicom();

	MGR_ACTION_BatchStop(eDxBAT_TYPE_USER);
#if defined(CONFIG_MW_CEC_SEND_POWER_OFF)
	SVC_CEC_SetPowerState(FALSE);
	SVC_CEC_PowerOffTv();
#endif
	return ERR_BUS_NO_ERROR;
}

static HCHAR *mgr_action_GetSleepModeString (PxSLEEP_MODE_e mode)
{
	switch (mode)
	{
		ENUM_TO_STR (ePAMA_POWERSTATE_IDLE)
		ENUM_TO_STR (ePAMA_POWERSTATE_LIGHT_SLEEP)
		ENUM_TO_STR (ePAMA_POWERSTATE_DEEP_SLEEP)
		ENUM_TO_STR (ePAMA_POWERSTATE_MAX)
	default:
		break;
	}

	return "Unknown";
}

static HCHAR *mgr_action_GetBootModeString (PxBOOT_MODE_e mode)
{
	switch (mode)
	{
		ENUM_TO_STR (ePAMA_BOOTMODE_WARM)
		ENUM_TO_STR (ePAMA_BOOTMODE_COLD)
		ENUM_TO_STR (ePAMA_BOOTMODE_MAX)
	default:
		break;
	}

	return "Unknown";
}



BUS_Result_t	PROC_action(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t				eGwmResult = ERR_BUS_NO_ERROR;
	mgr_action_Contents_t		*pstContents = mgr_action_GetContents();

	switch (message)
	{
		case eMEVT_BUS_CREATE :
		{
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "PROC_action");
			mgr_action_SetState (eAmState_IDLE);
			mgr_action_MsgCreate(message, hAct, p1, p2, p3);
			break;
		}
		case eMEVT_BUS_GO_OPERATION :
		{
			HxLOG_Print("eMEVT_BUS_GO_OPERATION\n");
			if(mgr_action_GetState() != eAmState_NORMAL)
			{
				mgr_action_InitContents(pstContents);
				mgr_action_MsgGoOperation(message, hAct, p1, p2, p3);
				mgr_action_SetState (eAmState_NORMAL);
			}
			break;
		}
		case eMEVT_BUS_READY_SHUTDOWN :
		case eMEVT_BUS_GO_SHUTDOWN :
		{
			HxLOG_Print("eMEVT_BUS_GO_SHUTDOWN\n");

			if(message == eMEVT_BUS_READY_SHUTDOWN)
			{
				mgr_action_MsgReadyShutdown(message, hAct, p1, p2, p3);
				mgr_action_SetState (eAmState_ACTIVE_STANDBY);
			}

			if(mgr_action_StartShutDownProc(pstContents) == TRUE)
			{
				mgr_action_SetMicom();
			}
			eGwmResult = MESSAGE_BREAK;
			break;
		}
		case eMEVT_BUS_SHUTDOWN_DONE:
		{
			HBOOL			bPowerSavingStandby = FALSE;
			PxSLEEP_MODE_e	eSleepMode = ePAMA_POWERSTATE_DEEP_SLEEP;
			PxBOOT_MODE_e	eBootMode  = ePAMA_BOOTMODE_WARM;
			HBOOL			bWifiOnOff = TRUE;
			HBOOL			bDoNothing = FALSE;

			HxLOG_Warning("[%s] ePAMA_STATUS_ShutdownDone\n",__FUNCTION__);

			if (PAPI_GetPowerSavingOnStandby(&bPowerSavingStandby) != ERR_OK)
			{
				HxLOG_Error("[%s] PAPI_GetPowerSavingOnStandby() Error\n",__FUNCTION__);
			}
#if defined(CONFIG_APCORE_ALWAYS_ACTIVE_STANDBY)
			bDoNothing = TRUE;
#endif
#if defined(CONFIG_MW_SYS_ACTIVESTBY_BY_PWRSAVINGONSTBY)
			if (bPowerSavingStandby == FALSE)
			{
				bDoNothing = TRUE;
			}
#endif
			if (bDoNothing)
			{
				goto STAY_ACTIVE_STANDBY;
			}

			if(PAPI_GetSleepModeOnStandby(&eSleepMode) != ERR_OK)
			{
				HxLOG_Error("PAPI_GetSleepModeOnStandby() Error\n");
				(void)PAPI_SetSleepModeOnStandby(eSleepMode);
			}
			if(PAPI_GetBootModeOnStandby(&eBootMode) != ERR_OK)
			{
				HxLOG_Error("PAPI_GetBootModeOnStandby() Error\n");
				(void)PAPI_SetBootModeOnStandby(eBootMode);
			}

			if(PAPI_GetWifiPowerOnStandby(&bWifiOnOff) != ERR_OK)
			{
				HxLOG_Error("PAPI_GetWifiPowerOnStandby() Error\n");
				PAPI_SetWifiPowerOnStandby(TRUE);
			}

			mgr_action_ShutdownDevice(bPowerSavingStandby,bWifiOnOff);
#if defined(CONFIG_DEBUG)
			HxLOG_Warning("**********************************\n");
			HxLOG_Warning("SleepMode:[%s], BootMode:[%s], Wifi:%s \n",
				mgr_action_GetSleepModeString(eSleepMode),mgr_action_GetBootModeString(eBootMode),(bWifiOnOff==TRUE)? "TURN-ON" : "TURN-OFF");
			HxLOG_Warning("**********************************\n");
#endif

#if 1
			{
				HUINT32		i = 0;
				HxLOG_Print("It will wait 10 sec before entering s3 mode for system stability!!\n");
				for(i = 0; i < 10; i++)
				{
					HLIB_STD_TaskSleep(1000);
				}
			}
#endif

			if(mgr_action_CheckS3Mode(eSleepMode,eBootMode,bPowerSavingStandby) == ERR_OK)
			{
				mgr_action_WakeupFromS3Mode(pstContents);
			}
			else
			{
				mgr_action_TurnOff(bPowerSavingStandby);
			}

STAY_ACTIVE_STANDBY:
			eGwmResult = MESSAGE_BREAK;
			break;
		}

		case eMEVT_COLDBOOT_NOTIFY_DONE:
			HxLOG_Print("[%s] eMEVT_COLDBOOT_NOTIFY_DONE StartNormal\n",__FUNCTION__);
			MGR_ACTION_BatchInstallUserProc();
			MGR_ACTION_BatchStart (eDxBAT_TYPE_USER);
			MGR_ACTION_StartNormal(FALSE);
			break;

		case eMEVT_BUS_START_BATCH :
			HxLOG_Print("[%s] eMEVT_BUS_START_BATCH\n",__FUNCTION__);
			MGR_ACTION_BatchStart ((DxBatchType_e) p1);
			break;

		case eMEVT_TIME_CLOCKRECOVER_SETUP_DONE :
			if(pstContents->bActionTimeRecorveryStart == TRUE)
				pstContents->bActionTimeRecorveryEnd = TRUE;
			break;

		case eMEVT_ACTION_FINISHED_BATCH_ACTION_ITEMS :
			HxLOG_Print("[%s] eMEVT_ACTION_FINISHED_BATCH_ACTION_ITEMS %s\n",__FUNCTION__, p3?"Failed":"Succeed");
			if(p1 == eDxBAT_TYPE_COLDBOOT)
				BUS_PostMessage(NULL, eMEVT_COLDBOOT_NOTIFY_DONE, 0, 0, 0, 0);
			else if(p1 == eDxBAT_TYPE_STANDBY)
				mgr_action_FinishBatchProc(pstContents,p1, p3);
			break;

		case eMEVT_ACTION_REBOOT_STB:
				if (DB_PARAM_KeepingItemFinished())
				{
					mgr_action_Reboot();
				}
				else
				{
					PAPI_WaitForReboot(2000);
				}
			break;
		case eMEVT_BUS_REBOOT_DONE :
			HxLOG_Warning("[%s] eMEVT_ACTION_REBOOT_STB reboot done\n",__FUNCTION__);
			mgr_action_pamaReboot();
			break;
		case eSEVT_DB_PARAM_CHECK_ALL_REMOVED:
			{
				// Check Registry and Dama Completely Removed. (Obama's data)
				if (DB_PARAM_KeepingItemAllTableRemoved((void *)p1, p2) == TRUE)
				{
					HxLOG_Debug("eSEVT_DB_PARAM_CHECK_ALL_REMOVED\n");
					// Remove Cache
					(void)DB_PARAM_RemoveCache();
					// Restore Keeping Item
					(void)DB_PARAM_KeepingItemWrite((void *)p1);
				}
			}
			break;

		case eMEVT_BUS_DESTROY :
			return MESSAGE_BREAK;

		case eMEVT_BUS_SIGNAL:
			mgr_action_onSignalBusProcess(message, hAct, p1, p2, p3);
			return MESSAGE_PASS;

		case eMEVT_SEARCH_STARTED:
			mgr_action_EvtSearchStarted(message, hAct, p1, p2, p3);
			break;

		case eMEVT_SEARCH_FINISHED:
			mgr_action_EvtSearchFinished(message, hAct, p1, p2, p3);
			break;
#if defined(CONFIG_MW_CEC) && defined(CONFIG_MW_CEC_RECEIVE_POWER_OFF)
		/**********************************************************************
			CEC에 의한 power off
		***********************************************************************/
		case eSEVT_MW_CEC_POWER_OFF:
                     {
                            HBOOL bPwrState;
                            HxLOG_Warning("[%s] eSEVT_MW_CEC_POWER_OFF\n",__FUNCTION__);
                            SVC_CEC_GetPowerState((HBOOL*)&bPwrState);
                            if(bPwrState == TRUE)
                            {
                                   mgr_action_CEC_PowerOff();
                            }
                     }
			break;
#endif
		default:
			break;
	}

	if( (eGwmResult == MESSAGE_BREAK) || (eGwmResult == MESSAGE_PASS) )
	{
		return eGwmResult;
	}
	else
	{
		return BUS_ProcessMessageDefault(message, hAct, p1, p2, p3);
	}
}


static HERROR	mgr_action_onSystemStateChanged(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Assert(nArgc == 1);

	//	hapi로 의한 것이 아닌 다른 것으로 인한 것일 경우 nParam1의 값을 변경하던지, Message값을 변경하던지 고민이 필요.
	BUS_PostMessage(NULL, eMEVT_BUS_SIGNAL, HANDLE_NULL, HxOBJECT_INT(apArgv[0]), 0, 0);

	return ERR_OK;
}

static HERROR	mgr_action_FormatOccured(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Assert(nArgc == 2);
	BUS_PostMessage(NULL, eMEVT_STORAGE_FORMAT_DEVICE, HANDLE_NULL, HxOBJECT_INT(apArgv[0]), 0, 0);
	return ERR_OK;
}

static HERROR	mgr_action_onMainApplicationReady(HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *pvUserData)
{
	HxLOG_Assert(nArgc == 1);
	BUS_PostMessage(NULL, eMEVT_ACTION_NOTIFY_READY_MAIN_APPLICATION, HANDLE_NULL, HxOBJECT_INT(apArgv[0]), 0, 0);
	return ERR_OK;
}


HERROR MGR_ACTION_ReinitBatch(void)
{
	return mgr_action_BatchInitContents(mgr_action_GetContents());
}

HERROR MGR_ACTION_Start(void)
{
	BUS_Result_t	eRes;
	eRes = BUS_MGR_Create("PROC_action", APP_ACTION_MGR_PRIORITY, PROC_action, 0, 0, 0, 0);

	HAPI_ConnectSignal("signal:onSystemStateChanged", NULL, mgr_action_onSystemStateChanged);
	HAPI_ConnectSignal("signal:onFormatOccurred", NULL, mgr_action_FormatOccured);
	HAPI_ConnectSignal("signal:onMainApplicationReady", NULL, mgr_action_onMainApplicationReady);
	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}

/* end of file */

