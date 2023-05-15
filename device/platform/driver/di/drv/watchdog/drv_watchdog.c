/*
 * $Header:   $
 */
/*********************************************************************
 * $Workfile:   drv_watchdog.c  $
 * $Modtime:   April 4 2013 01:07:04  $
 *
 * Author:
 * Description:
 *
 *                                 Copyright (c) 2008 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/



/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#endif
#include <sys/types.h>

#include "vkernel.h"
#include "taskdef.h"
#include "drv_watchdog.h"
#include "drv_micom.h"
#include "drv_pm.h"
#if defined(CONFIG_WATCHDOG_BRCM)
#include "nexus_watchdog.h"
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#if defined(CONFIG_DEBUG)
	//#define WATCHDOG_DEBUG
	#ifdef WATCHDOG_DEBUG
	#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
	#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
	#else
	#define PrintDebug(...)             ((void)0)
	#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
	#endif
#else
	#define PrintDebug(...)     VK_DBG_Print(__VA_ARGS__)
	#define PrintError(...)    	VK_DBG_Print(__VA_ARGS__)
#endif

#if defined(CONFIG_WATCHDOG_BRCM)
#define DRV_DOG_DEFAULT_KICK_PERIOD			(NEXUS_WATCHDOG_MIN_TIMEOUT/5) 	/* second */
#define DRV_DOG_DEFAULT_TIMEOUT				NEXUS_WATCHDOG_MIN_TIMEOUT 		/* second */
#else
#define DRV_DOG_DEFAULT_KICK_PERIOD			2 	/* second */
#define DRV_DOG_DEFAULT_TIMEOUT				5	/* second */
#endif

#define DRV_DOG_TASK_PRIORITY				USB_TASK_PRIORITY
#define DRV_DOG_TASK_STACKSIZE				USB_TASK_STACK_SIZE
#define WATCHDOG_MSG_QUEUE_SIZE				64

typedef enum {
	DOG_CMD_ENABLE = 0,
	DOG_CMD_SET_TIMEOUT,
	DOG_CMD_SET_KICK_PERIOD,
	DOG_CMD_KICK,
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	DOG_CMD_SUSPEND,
	DOG_CMD_RESUME,
#endif
	DOG_CMD_MAX
} WATCHDOG_CMD_E;

typedef struct {
	WATCHDOG_CMD_E	cmd;
	unsigned int	param;
} WATCHDOG_MSG_T;

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static unsigned long			ulWatchDogTaskId;
static unsigned long			ulWatchdogMsgId;
static unsigned long			ulWatchdogKickTimerId;
static unsigned int				uiKickPeriod;
static unsigned int				uiDogTimeout;
static unsigned char			ucDogEnabled;
static unsigned char			ucInitialized;
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
static HBOOL					bWatchdogInSuspend = FALSE;
#endif
/*******************************************************************/
/****************** Static Function Prototypes *********************/
/*******************************************************************/

/********************************************************************
Function    : P_DOG_KickTimerCb
Description :
Input	   : none
Output	   : none
Return	   : void
********************************************************************/
static void P_DOG_KickTimerCb(unsigned long tid, void *arg)
{
	int					ret;
	WATCHDOG_MSG_T		msg;

	(void)tid;
	(void)arg;

	VK_MEM_Memset(&msg, 0, sizeof(WATCHDOG_MSG_T));
	msg.cmd = DOG_CMD_KICK;
	msg.param = (unsigned int)0; //not used

	ret = VK_MSG_SendTimeout(ulWatchdogMsgId, &msg, sizeof(WATCHDOG_MSG_T), 0);
	if (ret != VK_OK) {
		PrintError("[%s] VK_MSG_Send Error!\n", __FUNCTION__);
	}

	return;
}

/********************************************************************
Function    : P_DOG_SetKickTimer
Description :
Input	   : none
Output	   : none
Return	   : int
********************************************************************/
static int P_DOG_SetKickTimer(unsigned char en)
{
	int		err;

	if (en) {
		err = VK_TIMER_EventEvery(uiKickPeriod*1000/*msec*/, P_DOG_KickTimerCb, NULL, 0, &ulWatchdogKickTimerId);
		if (err != VK_OK) {
			PrintError("[%s] Error! can't register kick timer\n", __FUNCTION__);
			return -1;
		}
	}
	else {
		if (ulWatchdogKickTimerId != 0) {
			err = VK_TIMER_Cancel(ulWatchdogKickTimerId);
			if (err != VK_OK) {
				PrintError("[%s] Error! can't cancel kick timer\n", __FUNCTION__);
				return -1;
			}
		}
	}

	return 0;
}

/********************************************************************
Function    : P_DOG_Kick
Description :
Input	   : none
Output	   : none
Return	   : void
********************************************************************/
static void P_DOG_Kick(void)
{
	if (ucDogEnabled == 0) {
		PrintError("[%s] Error! Watchdog not enabled\n", __FUNCTION__);
		return;
	}

#if defined(CONFIG_WATCHDOG_BRCM)
	if (NEXUS_Watchdog_StopTimer() != NEXUS_SUCCESS) {
		PrintError("[%s] Error! can't stop watchdog\n", __FUNCTION__);
		return;
	}
	if (NEXUS_Watchdog_StartTimer() != NEXUS_SUCCESS) {
		PrintError("[%s] Error! can't start watchdog\n", __FUNCTION__);
		return;
	}
#elif defined(CONFIG_WATCHDOG_MICOM)
	{
		unsigned int	uiWatchdogResetTime = 0;

		if(DRV_MICOM_GetLocalTime(&uiWatchdogResetTime) != DRV_OK) {
			PrintError("[%s] Error! can't kick micom watchdog\n", __FUNCTION__);
			return;
		}
	}
#endif

	PrintDebug("[%s] Watchdog Kicked\n", __FUNCTION__);
	return;
}

/********************************************************************
Function    : P_DOG_Enable
Description :
Input	   : none
Output	   : none
Return	   : int
********************************************************************/
static int P_DOG_Enable(unsigned int enable)
{
	if (ucDogEnabled == enable) {
		PrintError("[%d]%s enable %d\n",__LINE__,__func__,ucDogEnabled);
		return 0;
	}

#if defined(CONFIG_WATCHDOG_BRCM)
	if (enable) {
		if (NEXUS_Watchdog_SetTimeout((unsigned)uiDogTimeout) != NEXUS_SUCCESS) {
			PrintError("[%s] Error! can't set watchdog timeout\n", __FUNCTION__);
			return -1;
		}
		if (NEXUS_Watchdog_StartTimer() != NEXUS_SUCCESS) {
			PrintError("[%s] Error! can't start watchdog timer\n", __FUNCTION__);
			return -1;
		}

		if (P_DOG_SetKickTimer(TRUE) < 0) {
			if (NEXUS_Watchdog_StopTimer() != NEXUS_SUCCESS) {
				PrintError("[%s] Error! can't stop watchdog timer\n", __FUNCTION__);
				return -1;
			}
		}
	}
	else {
		if (NEXUS_Watchdog_StopTimer() != NEXUS_SUCCESS) {
			PrintError("[%s] Error! can't stop watchdog timer\n", __FUNCTION__);
			return -1;
		}
		P_DOG_SetKickTimer(FALSE);
	}
#elif defined(CONFIG_WATCHDOG_MICOM)
	if (DRV_MICOM_SetWatchdog(enable, uiDogTimeout) != DRV_OK) {
		PrintError("[%s] Error! can't ennable micom watchdog\n", __FUNCTION__);
		return -1;
	}
	if (enable) {
		if (P_DOG_SetKickTimer(TRUE) < 0) {
			if (DRV_MICOM_SetWatchdog(FALSE, uiDogTimeout) != DRV_OK) {
				PrintError("[%s] Error! can't stop watchdog timer\n", __FUNCTION__);
				return -1;
			}
		}
	}
	else {
		P_DOG_SetKickTimer(FALSE);
	}
#endif
	ucDogEnabled = enable;
	PrintDebug("[%s] WatchDog Enabled/Disable(%d)\n", __FUNCTION__, enable);

	return 0;
}

/********************************************************************
Function    : P_DOG_SetTimeout
Description :
Input	   : none
Output	   : none
Return	   : int
********************************************************************/
static int P_DOG_SetTimeout(unsigned int timeout)
{
#if defined(CONFIG_WATCHDOG_BRCM)
	if (ucDogEnabled) {
		if (NEXUS_Watchdog_StopTimer() != NEXUS_SUCCESS) {
			PrintError("[%s] Error! can't stop watchdog timer\n", __FUNCTION__);
			return -1;
		}
		if (NEXUS_Watchdog_SetTimeout((unsigned)timeout) != NEXUS_SUCCESS) {
			PrintError("[%s] Error! can't set watchdog timeout\n", __FUNCTION__);
			return -1;
		}
		if (NEXUS_Watchdog_StartTimer() != NEXUS_SUCCESS) {
			PrintError("[%s] Error! can't start watchdog timer\n", __FUNCTION__);
			return -1;
		}
	}
	else {
		if (NEXUS_Watchdog_SetTimeout((unsigned)timeout) != NEXUS_SUCCESS) {
			PrintError("[%s] Error! can't set watchdog timeout\n", __FUNCTION__);
			return -1;
		}
	}
#elif defined(CONFIG_WATCHDOG_MICOM)
	if (ucDogEnabled) {
		if(DRV_MICOM_SetWatchdog(ucDogEnabled, timeout) != DRV_OK) {
			PrintError("[%s] Error! can't set micom watchdog timeout\n", __FUNCTION__);
			return -1;
		}
	}
#endif
	uiDogTimeout = (unsigned int)timeout;
	uiKickPeriod = (unsigned int)timeout/5;
	PrintDebug("[%s] Dog SetTimeout(%d sec)\n", __FUNCTION__, timeout);

	return 0;
}

/********************************************************************
Function    : P_DOG_DogTask
Description :
Input	   : none
Output	   : none
Return	   : void
********************************************************************/
static void P_DOG_DogTask(void *param)
{
	WATCHDOG_MSG_T		msg;
	int					err;
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	static HBOOL		bWatchdogResume = FALSE;
#endif
	(void)param;

	/* initialize */
	uiDogTimeout = DRV_DOG_DEFAULT_TIMEOUT;
	uiKickPeriod = DRV_DOG_DEFAULT_KICK_PERIOD;
	ucDogEnabled = 0;

	/* enable watchdog  */
	P_DOG_Enable(1);

	while(1) {
		if (VK_MSG_Receive(ulWatchdogMsgId, &msg, sizeof(WATCHDOG_MSG_T)) != VK_OK)
			continue;
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
		if(bWatchdogInSuspend == TRUE && msg.cmd != DOG_CMD_RESUME)
		{
			if(msg.cmd == DOG_CMD_ENABLE)
			{
				/*Don't ignore this cmd arrived after bWatchdogInSuspend set TRUE*/
				/*Save it and use it when resume.*/
				bWatchdogResume = (HBOOL)msg.param;
			}
			continue;
		}
#endif
		switch(msg.cmd) {
		case DOG_CMD_ENABLE:
			err = P_DOG_Enable(msg.param);
			if(err != 0)
			{
				PrintError("[%s,%d] Failed to enable watchdog.\n", __FUNCTION__, __LINE__);
			}
			break;
		case DOG_CMD_SET_TIMEOUT:
			err = P_DOG_SetTimeout(msg.param);
			if(err != 0)
			{
				PrintError("[%s,%d] Failed to set timeout watchdog.\n", __FUNCTION__, __LINE__);
			}
			break;
		case DOG_CMD_SET_KICK_PERIOD:
			uiKickPeriod = (unsigned int)msg.param;
			if (ucDogEnabled) {
				err = P_DOG_SetKickTimer(FALSE);
				if (err < 0) break;

				P_DOG_SetKickTimer(TRUE);
			}
			break;
		case DOG_CMD_KICK:
			P_DOG_Kick();
			break;
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
		case DOG_CMD_SUSPEND:
			bWatchdogInSuspend = TRUE;
			if(ucDogEnabled != 0)
			{
				bWatchdogResume = TRUE;
				err = P_DOG_Enable(msg.param);
				if(err != 0)
				{
					PrintError("[%s,%d] Failed to stop watchdog.\n", __FUNCTION__, __LINE__);
				}
			}
			break;
		case DOG_CMD_RESUME:
			bWatchdogInSuspend = FALSE;
			if(bWatchdogResume == TRUE)
			{
				bWatchdogResume = FALSE;
				err = P_DOG_Enable(msg.param);
				if(err != 0)
				{
					PrintError("[%s,%d] Failed to start watchdog.\n", __FUNCTION__, __LINE__);
				}
			}
			break;
#endif
		default:
			break;
		}
	}

	return;
}


/*******************************************************************/
/****************** Global Function Prototypes *********************/
/*******************************************************************/

/********************************************************************
Function    : DRV_DOG_Enable
Description :
Input	   : none
Output	   : none
Return	   : DRV_Error
********************************************************************/
DRV_Error DRV_DOG_Enable(int en)
{
	int					ret;
	WATCHDOG_MSG_T		msg;

	VK_MEM_Memset(&msg, 0, sizeof(WATCHDOG_MSG_T));
	msg.cmd = DOG_CMD_ENABLE;
	msg.param = (unsigned int)en;

	ret = VK_MSG_SendTimeout(ulWatchdogMsgId, &msg, sizeof(WATCHDOG_MSG_T), 0);
	if (ret != VK_OK) {
		PrintError("[%s] Error! can't send message!\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

/********************************************************************
Function    : DRV_DOG_Kick
Description :
Input	   : none
Output	   : none
Return	   : DRV_Error
********************************************************************/
DRV_Error DRV_DOG_Kick(void)
{
	int					ret;
	WATCHDOG_MSG_T		msg;

	VK_MEM_Memset(&msg, 0, sizeof(WATCHDOG_MSG_T));
	msg.cmd = DOG_CMD_KICK;
	msg.param = (unsigned int)0; //not used

	ret = VK_MSG_SendTimeout(ulWatchdogMsgId, &msg, sizeof(WATCHDOG_MSG_T), 0);
	if (ret != VK_OK) {
		PrintError("[%s] Error! can't send message!\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

/********************************************************************
Function    : DRV_DOG_SetTimeout
Description :
Input	   : none
Output	   : none
Return	   : DRV_Error
********************************************************************/
DRV_Error DRV_DOG_SetTimeout(unsigned int	sec)
{
	int					ret;
	WATCHDOG_MSG_T		msg;

	VK_MEM_Memset(&msg, 0, sizeof(WATCHDOG_MSG_T));
	msg.cmd = DOG_CMD_SET_TIMEOUT;
	msg.param = (unsigned int)sec;

	ret = VK_MSG_SendTimeout(ulWatchdogMsgId, &msg, sizeof(WATCHDOG_MSG_T), 0);
	if (ret != VK_OK) {
		PrintError("[%s] Error! can't send message!\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

/********************************************************************
Function    : DRV_DOG_SetKickPeriod
Description :
Input	   : none
Output	   : none
Return	   : DRV_Error
********************************************************************/
DRV_Error DRV_DOG_SetKickPeriod(unsigned int	sec)
{
	int					ret;
	WATCHDOG_MSG_T		msg;

	VK_MEM_Memset(&msg, 0, sizeof(WATCHDOG_MSG_T));
	msg.cmd = DOG_CMD_SET_KICK_PERIOD;
	msg.param = (unsigned int)sec;

	ret = VK_MSG_SendTimeout(ulWatchdogMsgId, &msg, sizeof(WATCHDOG_MSG_T), 0);
	if (ret != VK_OK) {
		PrintError("[%s] Error! can't send message!\n", __FUNCTION__);
		return DRV_ERR;
	}

	return DRV_OK;
}

/********************************************************************
Function    : DRV_DOG_IsEnabled
Description :
Input	   : none
Output	   : none
Return	   : DRV_Error
********************************************************************/
DRV_Error DRV_DOG_IsEnabled(unsigned char *enabled)
{
	*enabled = ucDogEnabled;

	return DRV_OK;
}

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
/********************************************************************
Function    : DRV_DOG_Suspend
Description :
Input	   : none
Output	   : none
Return	   : DRV_Error
********************************************************************/
void DRV_DOG_Suspend(void)
{
#if defined(CONFIG_WATCHDOG_BRCM)
	int					ret;
	WATCHDOG_MSG_T		msg;

	VK_MEM_Memset(&msg, 0, sizeof(WATCHDOG_MSG_T));
	msg.cmd = DOG_CMD_SUSPEND;
	msg.param = (unsigned int)0; /*0: disable watchdog*/

	ret = VK_MSG_SendUrgent(ulWatchdogMsgId, &msg, sizeof(WATCHDOG_MSG_T));
	if (ret != VK_OK) {
		PrintError("[%s] Error! can't send message!\n", __FUNCTION__);
		return;
	}
#endif
	return;
}

/********************************************************************
Function    : DRV_DOG_Resume
Description :
Input	   : none
Output	   : none
Return	   : DRV_Error
********************************************************************/
void DRV_DOG_Resume(void)
{
#if defined(CONFIG_WATCHDOG_BRCM)
	int					ret;
	WATCHDOG_MSG_T		msg;
#endif
	/*NEXUS_Watchdog_StopTimer shall be called regardless of watchdog mode. (MICOM or BRCM)*/
	/*It cancels watchdog set by loader*/
	if (NEXUS_Watchdog_StopTimer() != NEXUS_SUCCESS) {
		PrintError("[%s] Error! can't stop watchdog timer\n", __FUNCTION__);
	}
#if defined(CONFIG_WATCHDOG_BRCM)
	VK_MEM_Memset(&msg, 0, sizeof(WATCHDOG_MSG_T));
	msg.cmd = DOG_CMD_RESUME;
	msg.param = (unsigned int)1; /*1: enable watchdog*/

	ret = VK_MSG_SendUrgent(ulWatchdogMsgId, &msg, sizeof(WATCHDOG_MSG_T));
	if (ret != VK_OK) {
		PrintError("[%s] Error! can't send message!\n", __FUNCTION__);
		return;
	}
#endif
	return;

}
#endif

/********************************************************************
Function    : DRV_DOG_Init
Description :
Input	   : none
Output	   : none
Return	   : DRV_Error
********************************************************************/
DRV_Error DRV_DOG_Init(void)
{
	int		ret;

	if (ucInitialized == TRUE) {
		return DRV_OK;
	}

	ret = VK_MSG_Create(WATCHDOG_MSG_QUEUE_SIZE, sizeof(WATCHDOG_MSG_T), "WatchdogMsg", &ulWatchdogMsgId, VK_SUSPENDTYPE_FIFO);
	if (ret != VK_OK) {
		PrintError("[%s] Error! can't initialize dog message queue\n", __FUNCTION__);
		return DRV_ERR;
	}
	ret = VK_TASK_Create(P_DOG_DogTask, DRV_DOG_TASK_PRIORITY, DRV_DOG_TASK_STACKSIZE, "WatchDogTask", NULL, &ulWatchDogTaskId, FALSE);
	if (ret != VK_OK) {
		PrintError("[%s] Error! can't initialize dog task\n", __FUNCTION__);
		return DRV_ERR;
	}

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	DRV_PM_RegisterCallback(DRV_DOG_Suspend, DRV_DOG_Resume);
#endif

	VK_TASK_Start(ulWatchDogTaskId);
	ucInitialized = TRUE;

	return DRV_OK;
}
