/**
	@file     	mgr_storage_power.c
	@brief    							\n

	Description:  						\n
	Module: 		mgr/storage			 	\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <octo_common.h>
#include <stdio.h>	/* For snprintf(3) */
#include <hlib.h>
#include <octo_common.h>
#include <mgr_common.h>

#include <bus.h>
#include <mgr_action.h>
#include <mgr_pvr_action.h>
#include <svc_fs.h>
#include <mgr_storage.h>


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Functions     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/******************* Static Function Prototypes  *******************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#if	defined(CONFIG_APCORE_ALWAYS_ACTIVE_STANDBY)
STATIC MgrStorage_PowerState_e	s_eTargetPowerState = ePS_State_Null;
STATIC MgrStorage_PowerState_e	s_eCurrentPowerState = ePS_State_Null;

STATIC void mgr_storage_doit(void)
{
	if (s_eTargetPowerState != s_eCurrentPowerState)
	{
		switch (s_eTargetPowerState)
		{
			case	ePS_On:
				HxLOG_Error("[Prepare Power HDD - On] \n");
				 //[2013.10.18] 필요시 UAPI로 기능 구현 해야함
				// SVC_PVR_TurnOnIntStorage();
				s_eCurrentPowerState = ePS_Waiting_On;
				break;
			case	ePS_Off:
				HxLOG_Error("[Prepare Power HDD - Off] \n");
				//[2013.10.18] 필요시 UAPI로 기능 구현 해야함
				// SVC_PVR_TurnOffIntStorage();
				s_eCurrentPowerState = ePS_Waiting_Off;
				break;
			default:
				break;
		}
	}
}

STATIC void	mgr_storage_doNextStep(HINT32 nCurrentState)
{
	HxLOG_Error("[%s] -- current[%x] -- prevState[%x] -- targetState[%x]\n", __FUNCTION__, nCurrentState, s_eCurrentPowerState, s_eTargetPowerState);
	switch (nCurrentState)
	{
		case eMEVT_STORAGE_NOTIFY_PLUGGED:
			s_eCurrentPowerState = ePS_On;
			break;
		case eMEVT_STORAGE_NOTIFY_UNPLUGGED:
			s_eCurrentPowerState = ePS_Off;
			break;
	}

	mgr_storage_doit();
}

BUS_Result_t MGR_STORAGE_IntHddSetPowerState(MgrStorage_PowerState_e ePowerState)
{
#if defined(CONFIG_MW_MM_PVR)
	s_eTargetPowerState = ePowerState;

	//	if not on going?
	if ((s_eCurrentPowerState != ePS_Waiting_Off) && (s_eCurrentPowerState != ePS_Waiting_On))
		mgr_storage_doit();
#endif
	return ERR_OK;
}

STATIC BUS_Result_t proc_mstorage_IntHddPowerMonitor(HINT32 message, Handle_t hAct, HINT32 p1, HINT32 p2, HINT32 p3)
{
	switch (message)
	{
		case eMEVT_BUS_CREATE:
			return MESSAGE_BREAK;

		//case EVT_FS_NOTIFY_INT_HDD_PLUGGED:
		case eMEVT_STORAGE_NOTIFY_PLUGGED:
			if (p2 == TRUE)
			{
				HxLOG_Error("eMEVT_STORAGE_NOTIFY_PLUGGED - [%s]\n", __FUNCTION__);
				mgr_storage_doNextStep(eMEVT_STORAGE_NOTIFY_PLUGGED);
			}
			return MESSAGE_PASS;

		//case EVT_FS_NOTIFY_INT_HDD_UNPLUGGED:
		case eMEVT_STORAGE_NOTIFY_UNPLUGGED:
			if (p2 == TRUE)
			{
				HxLOG_Error("eMEVT_STORAGE_NOTIFY_UNPLUGGED - [%s]\n", __FUNCTION__);
				mgr_storage_doNextStep(eMEVT_STORAGE_NOTIFY_UNPLUGGED);
			}
			return MESSAGE_PASS;

		default:
			return MESSAGE_PASS;

	}
}
#endif

HERROR		MGR_STORAGE_IntHddPowerMonitorStart(void)
{
#if	defined(CONFIG_APCORE_ALWAYS_ACTIVE_STANDBY)
	BUS_Result_t	eRes;

	eRes = BUS_MGR_Create("HDDPowerMonitor", APP_HDD_PWR_MONITOR_PRIORITY, proc_mstorage_IntHddPowerMonitor, 0, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
#else
	return ERR_FAIL;
#endif
}



/* end of file */
