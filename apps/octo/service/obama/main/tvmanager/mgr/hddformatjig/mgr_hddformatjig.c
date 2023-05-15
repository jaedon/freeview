/**
	@file     mgr_hddformatjig.c
	@brief    .

	Description:
	Module: MGR/CopyrightControl			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
  *	이 파일은 생산에서 HDD format 을 위해서 제작된 JIG SW 이다. octo 2에서 사용되는 rule을 다수 어겼다.
  *	그러므로 절대로 참고하지 말길 바랍니다.
  */

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include <hlib.h>
#include <octo_common.h>
#include <mgr_hddformatjig.h>
#include <mgr_storage.h>
#include <uapi.h>
#include <svc_fs.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define HDD_FORMAT_PROCESS_BLUE_TIMER_ID			100
#define HDD_FORMAT_PROCESS_RED_TIMER_ID			101
#define HDD_FORMAT_PROCESS_OK_DURATION			(1*1000)			// 1 seconds

#define HDD_FORMAT_FINISH_NOTE_TIMER_ID			102
#define HDD_FORMAT_FINISH_NOTE_DURATION			(20*1000)			// 20 seconds

#define HDD_FORMAT_RESTART_TIMER_ID				103
#define HDD_FORMAT_RESTART_DURATION				(3*1000)			// 3 seconds


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef enum
{
	FDI_FRONT_KEY = 0,
	FDI_RCU_KEY
} FDI_KEY_DEVICE;

typedef enum
{
	FDI_KEY_PRESSED = 0,
	FDI_KEY_REPEATED,
	FDI_KEY_RELEASED
} FDI_KEY_TYPE;

typedef HINT32 (* pfnDI_KEY_Notify)(FDI_KEY_DEVICE eDevice, HUINT32 ulKeyCode, FDI_KEY_TYPE eKeyType);
typedef void (*FDI_KEY_EVENT_CALLBACK)(FDI_KEY_DEVICE eDev, unsigned int nKeyCode, FDI_KEY_TYPE eKeyType);


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
STATIC HBOOL		s_bFormatDone = FALSE;
FDI_KEY_EVENT_CALLBACK pfnFdiKeyEventCallback;




extern void DI_KEY_RegisterKeyEventCallback (pfnDI_KEY_Notify pfnNotify) ;


/********************************************************************/
/********************	Application Proc.	*************************/
/********************************************************************/

STATIC void 		proc_INTRN_FAI_APP_KeyCallback(FDI_KEY_DEVICE eDev, unsigned int nKeyCode, FDI_KEY_TYPE eKeyType)
{
	HxLOG_Error(" ## FAI APP CALL BACK ### \n");
}

STATIC HUINT32	proc_INTRN_FDI_KEY_EventCallBack(FDI_KEY_DEVICE eDevice, HUINT32 ulKeyCode, FDI_KEY_DEVICE eKeyType)
{
	HxLOG_Error(" ## CALL BACK ### (eDevice=%d)\n", eDevice);
	switch( eDevice )
	{
		//case DI_FRONT_KEY :
		case FDI_FRONT_KEY :			
			(void)SVC_SYS_SetLedOnOff(eDxLED_ID_BLUE_COLOR, FALSE); 	// BLUE LED OFF
			//BUS_SetTimer(HDD_FORMAT_RESTART_TIMER_ID, HDD_FORMAT_RESTART_DURATION);

			HxLOG_Error(" ### DI STOR HDD POWER ON ########################### \n");
			// hdd power on
			HxLOG_Error(" UAPI_FS_HddPowerOn() call \n");
			UAPI_FS_HddPowerOn(0, "aa");

			s_bFormatDone = FALSE;

			break;
		default:
			break;
	}
}

STATIC HUINT32	proc_FDI_KEY_RegisterEventCallback(FDI_KEY_EVENT_CALLBACK pfnCallback) /* wrapping */
{
	pfnFdiKeyEventCallback = pfnCallback;

	DI_KEY_RegisterKeyEventCallback(proc_INTRN_FDI_KEY_EventCallBack);

	return 0;
}

STATIC HUINT32	proc_hddformat_checkHdd(void)
{
	switch (SVC_FS_GetPvrHddStatus(SVC_FS_GetDefaultPvrIdx()))	// 아직 안쓴다. internal 용으로 구현된 듯 하다.
	{
		case eDevStatus_OK:
			HxLOG_Error("OK !!!\n");
			break;
		case eDevStatus_NotInitiated:
			HxLOG_Error("NO INIT or NO HDD !!!\n");
			break;
		case eDevStatus_NoDevice:
			HxLOG_Error("NO HDD !!!\n");
			break;
		case eDevStatus_Malfunction:
			HxLOG_Error("MALFUNCTION !!!\n");
			break;
		case eDevStatus_Formatting:
			HxLOG_Error("IN FORMAT !!!\n");
			break;
		default:
			break;
	}

	return 0;
}

STATIC HUINT32	proc_hddformat_Format(void)
{
	HxLOG_Error("HDD Format Starting >> \n");
	return UAPI_FS_Format(0, "hmx_int_stor");
}

STATIC BUS_Result_t proc_HddFormatJIG_Action(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	HUINT32 		ulDeviceId;

	switch (message)
	{
		case eMEVT_BUS_CREATE:
			HxLOG_Error("eMEVT_BUS_CREATE >> \n");

			DRV_KEY_Init();
			proc_FDI_KEY_RegisterEventCallback(proc_INTRN_FAI_APP_KeyCallback);

			(void)SVC_SYS_SetLedOnOff(eDxLED_ID_BLUE_COLOR, TRUE); 	// BLUE LED ON
			BUS_MGR_SetMgrName(BUS_MGR_GetThis(), "proc_HddFormatJIG_Action");
			return MESSAGE_BREAK;

		case eMEVT_STORAGE_NOTIFY_PLUGGED:
			HxLOG_Error("eMEVT_STORAGE_NOTIFY_PLUGGED >> \n");

			if (p2 == TRUE && s_bFormatDone == FALSE)
			{
				/* start to format */
				proc_hddformat_Format();

				(void)SVC_SYS_SetLedOnOff(eDxLED_ID_BLUE_COLOR, TRUE);	// BLUE LED ON
				BUS_SetTimer(HDD_FORMAT_PROCESS_BLUE_TIMER_ID, HDD_FORMAT_PROCESS_OK_DURATION);
			}
			return MESSAGE_BREAK;

		case eMEVT_STORAGE_NOTIFY_UNPLUGGED:
			HxLOG_Error("eMEVT_STORAGE_NOTIFY_UNPLUGGED >> \n");
			return MESSAGE_BREAK;
			
		case eMEVT_STORAGE_NOTIFY_FORMAT_PROCESS:
			HxLOG_Error("eMEVT_STORAGE_NOTIFY_FORMAT_PROCESS >> \n");
			ulDeviceId = p1;
			return MESSAGE_BREAK;

		case eMEVT_STORAGE_NOTIFY_FORMAT_FAIL:
			HxLOG_Error("eMEVT_BUS_TIMER\n");

			(void)SVC_SYS_SetLedOnOff(eDxLED_ID_RED_COLOR, TRUE);	// RED LED OFF
			return MESSAGE_BREAK;

		case eMEVT_STORAGE_NOTIFY_FORMAT_DONE:
			HxLOG_Error("eMEVT_STORAGE_NOTIFY_FORMAT_DONE >> \n");
			ulDeviceId = p1;
			s_bFormatDone = TRUE;

			HxLOG_Error("eMEVT_STORAGE_NOTIFY_FORMAT_DONE (ulDeviceId=%d, SVC_FS_GetDefaultPvrIdx()=%d) \n", ulDeviceId, SVC_FS_GetDefaultPvrIdx());

			if (SVC_FS_GetPvrHddStatus(SVC_FS_GetDefaultPvrIdx()) == eDevStatus_OK)
			{
				HxLOG_Error("[HDD Formating is succeeded !!! >>\n");

				BUS_SetTimer(HDD_FORMAT_FINISH_NOTE_TIMER_ID, HDD_FORMAT_FINISH_NOTE_DURATION);

				// hdd power off
				HxLOG_Error("UAPI_FS_HddPowerOff() call \n");
				UAPI_FS_HddPowerOff(0, "aa");

			}
			else
			{
				HxLOG_Error("[HDD Formating is failed !!!>> \n");

				// hdd power off
				HxLOG_Error("UAPI_FS_HddPowerOff() call \n");
				UAPI_FS_HddPowerOff(0, "aa");

				BUS_KillTimer(HDD_FORMAT_PROCESS_RED_TIMER_ID);
				BUS_KillTimer(HDD_FORMAT_PROCESS_BLUE_TIMER_ID);
				
				(void)SVC_SYS_SetLedOnOff(eDxLED_ID_RED_COLOR, FALSE);		// RED LED OFF
				(void)SVC_SYS_SetLedOnOff(eDxLED_ID_BLUE_COLOR, FALSE);		// BLUE LED OFF

				(void)SVC_SYS_SetLedOnOff(eDxLED_ID_RED_COLOR, TRUE);		// RED LED ON
			}
			return MESSAGE_BREAK;

		case eMEVT_BUS_TIMER:
			HxLOG_Print("eMEVT_BUS_TIMER\n");

			// processing
			if (p1 == HDD_FORMAT_PROCESS_BLUE_TIMER_ID)
			{
				(void)SVC_SYS_SetLedOnOff(eDxLED_ID_BLUE_COLOR, FALSE);	// BLUE LED OFF
				(void)SVC_SYS_SetLedOnOff(eDxLED_ID_RED_COLOR, TRUE);		// RED LED ON
				BUS_KillTimer(HDD_FORMAT_PROCESS_BLUE_TIMER_ID);

				BUS_SetTimer(HDD_FORMAT_PROCESS_RED_TIMER_ID, HDD_FORMAT_PROCESS_OK_DURATION);
			}
			else if (p1 == HDD_FORMAT_PROCESS_RED_TIMER_ID)
			{
				(void)SVC_SYS_SetLedOnOff(eDxLED_ID_RED_COLOR, FALSE);		// RED LED OFF
				(void)SVC_SYS_SetLedOnOff(eDxLED_ID_BLUE_COLOR, TRUE);		// BLUE LED ON
				BUS_KillTimer(HDD_FORMAT_PROCESS_RED_TIMER_ID);

				BUS_SetTimer(HDD_FORMAT_PROCESS_BLUE_TIMER_ID, HDD_FORMAT_PROCESS_OK_DURATION);
			}
			else if (p1 == HDD_FORMAT_FINISH_NOTE_TIMER_ID)
			{
				HxLOG_Error("20초 경과....BLUE LED ON \n\n\n\n");
				
				(void)SVC_SYS_SetLedOnOff(eDxLED_ID_RED_COLOR, FALSE);		// RED LED OFF
				(void)SVC_SYS_SetLedOnOff(eDxLED_ID_BLUE_COLOR, FALSE);		// BLUE LED OFF

				BUS_KillTimer(HDD_FORMAT_PROCESS_RED_TIMER_ID);
				BUS_KillTimer(HDD_FORMAT_PROCESS_BLUE_TIMER_ID);
				BUS_KillTimer(HDD_FORMAT_FINISH_NOTE_TIMER_ID);
				
				(void)SVC_SYS_SetLedOnOff(eDxLED_ID_BLUE_COLOR, TRUE);		// BLUE LED ON
			}
			else if (p1 == HDD_FORMAT_RESTART_TIMER_ID)
			{
				HxLOG_Error("HDD Formating is failed !!!>> \n");
				BUS_KillTimer(HDD_FORMAT_RESTART_TIMER_ID);

				// hdd power off
				HxLOG_Error("UAPI_FS_HddPowerOff() call \n");
				UAPI_FS_HddPowerOff(0, "aa");

				BUS_KillTimer(HDD_FORMAT_PROCESS_RED_TIMER_ID);
				BUS_KillTimer(HDD_FORMAT_PROCESS_BLUE_TIMER_ID);
				
				(void)SVC_SYS_SetLedOnOff(eDxLED_ID_RED_COLOR, FALSE);		// RED LED OFF
				(void)SVC_SYS_SetLedOnOff(eDxLED_ID_BLUE_COLOR, FALSE);		// BLUE LED OFF

				(void)SVC_SYS_SetLedOnOff(eDxLED_ID_RED_COLOR, TRUE);		// RED LED ON

				s_bFormatDone = TRUE;
			}
			break;

		case eMEVT_BUS_DESTROY:
			HxLOG_Print("eMEVT_BUS_DESTROY\n");
			return MESSAGE_BREAK;

		default:
			break;
	}

	return BUS_ProcessMessageDefault(message, hAct, p1, p2, p3);
}



/* ======================================================================== */
/* Functions. 																*/
/* ======================================================================== */


HERROR	MGR_HddFormatJIG_Start(void)
{
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("proc_HddFormatJIG_Action", APP_ACTION_PRIORITY, proc_HddFormatJIG_Action, 0, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
}


/* End of File. ---------------------------------------------------------- */


