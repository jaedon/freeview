/**
 * di_power.c
*/

/**
 * @defgroup		DI_POWER : DI_POWER module
 * @author
 * @note			DI_POWER APIs
 * @brief			Define DI_POWER APIs
 * @file 			di_power.c
*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/reboot.h>
#endif
#include <unistd.h>

#include "linden_trace.h"

#include "bstd_defs.h"

#include "vkernel.h"
#include "di_err.h"
#include "drv_err.h"
#include "di_power.h"
#include "drv_micom.h"
#include "di_channel.h"
#if defined(CONFIG_ACTIVE_SCART_IN_STANDBY)
#include "drv_scart.h"
#endif
#include "drv_pm.h"
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
#include <signal.h>
#include "drv_remocon.h"
#include "nexus_watchdog.h"
#include "drv_led.h"
#endif
#include "drv_gpio.h"
#include "di_audio.h"
#include "drv_audio.h"


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE	TRACE_MODULE_DI_FAN


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static int s_PowerSavinfFlag = FALSE;

#if defined(CONFIG_DEBUG)
static HBOOL s_bKeyAgingTest = FALSE;
static HUINT32 s_ulOnTimer = 0;
#endif

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
#if defined(CONFIG_QUANTENNA_BBIC_MAC)
extern DRV_Error DRV_WIFI_Off(HUINT32 ulDevId);
extern DRV_Error DRV_WIFI_On(HUINT32 ulDevId);
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
#if defined(CONFIG_MIPS)
#include "nexus_security_rawcommand.h"
int s3_warm_boot_calculate_cbc_mac(void)
{
   unsigned int input_cmd[16], outBuffer[16], actualOutSize;
   NEXUS_Error errCode;

   VK_printf("s3_warm_boot_calculate_cbc_mac\n");

   input_cmd[0] = 0x00000010;
   input_cmd[1] = 0x00000066;
   input_cmd[2] = 0xabcdef00;
   input_cmd[3] = 0xa055aa5f;
   input_cmd[4] = 0x789a000c;
   input_cmd[5] = 0x00000001;
   input_cmd[6] = 0x00000001;
   input_cmd[7] = 5; /* Hard coded to Key 5 */
   errCode = NEXUS_Security_SubmitRawCommand (
       input_cmd, /* attr{nelem=sizeInBuffer;reserved=90} */
       8, /* size of pInBuffer in integers */
       outBuffer, /* [out] attr{nelem=sizeOutBuffer;reserved=90} */
       6, /* size of pOutBuffer in integers */
       &actualOutSize /* [out] number of integers written into pOutBuffer */
   );

   if ( errCode || (outBuffer[5]!=0 ))
   {
       VK_printf("s3_warm_boot_calculate_cbc_mac BSP command failed\n");
       return 1;
   }

   return 0;
}
#endif
#endif

int DRV_POWER_GetPowerSavingMode(void)
{
	return s_PowerSavinfFlag;
}

void DRV_POWER_SetAgingTestMode(HBOOL bOn, HUINT32 ulOnTimer)
{
#if defined(CONFIG_DEBUG)
	s_bKeyAgingTest = bOn;
	s_ulOnTimer = ulOnTimer;
#else
	HAPPY(bOn);
	HAPPY(ulOnTimer);
#endif
}

DI_ERR_CODE DI_POWER_SetPowerOff (DI_POWER_OFFMODE mode)
{
	DRV_Error	nDrvErr = DRV_OK;
	HUINT32		retry;
	HBOOL		bPowerSavingMode = TRUE;
	DI_ERR_CODE	diError = DI_ERR_ERROR;

	PrintEnter();

	PrintDebug("[%s] DI_POWER_OFFMODE: %d\n", __func__, mode);

#if !defined(CONFIG_OS_UCOS)
	sync(); /* Need to sync cache data to disk before system reboot */
#endif

	// 2009.12.2 : power saving 모드에서는 SCART auto-startup 모드 사용 하지 않음.
	// 1W under 위해 MICOM CONTROL 필요.

	// HW 1W 사양 만족을 위해,
	// Micom 에 의한 UNDER_1W(SCART, L/T power control) Port 를 control 할 필요가 있다면,

	switch( mode )
	{
		case DI_POWER_OFFMODE_POWERSAVING_READY :
			s_PowerSavinfFlag = TRUE;
			return DI_ERR_OK;

		case DI_POWER_OFFMODE_STANDBY_POWERSAVING :
#if defined(CONFIG_ACTIVE_SCART_IN_STANDBY)
			/* UNDER_1W(SCART, L/T power control) port control 대신 SCART standby mode(full stop mode)  사용.   */
			DRV_SCART_Standby();
			bPowerSavingMode = FALSE;
			break;
#else
			bPowerSavingMode = TRUE;
			break;
#endif
		case DI_POWER_OFFMODE_STANDBY_NORMAL :
		default :
			bPowerSavingMode = FALSE;
			break;
	}

	for (retry = 0; retry<MICOM_FAIL_RETRY_CNT; retry++)
	{
		nDrvErr = DRV_MICOM_SetPowerSavingMode(bPowerSavingMode);
		if (nDrvErr == DRV_OK)
		{
			break;
		}
		VK_TASK_Sleep(100);
	}

	/* execute power down operations of di, first */
	DRV_PM_PowerDown();

	for (retry = 0; retry<MICOM_FAIL_RETRY_CNT; retry++)
	{
		// Power-Down ~!!!
		nDrvErr = DRV_MICOM_SetStandby();
		if (nDrvErr == DRV_OK)
		{
			diError = DI_ERR_OK;
			break;
		}
		PrintError("Error(0x%x) In DRV_MICOM_SetStandby() \n", nDrvErr);
		VK_TASK_Sleep(100);
	}

	PrintExit();

	return diError;
}

DI_ERR_CODE DI_POWER_SetPowerOffWaitDelay (HUINT8 delay)
{
	BSTD_UNUSED(delay);
	PrintDebug("Not Supported Funtion. Why did you call %s in KRAKEN??? \n", __func__);
	return DI_ERR_OK;
}

DI_ERR_CODE DI_POWER_GetWakeupReason (DI_POWER_REASON *reason)
{
	DRV_MICOM_WAKEUP_SRC nWakeupSrc = DRV_MICOM_WAKEUP_BY_ACPOWER;
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;
	DRV_Error eDrvErr = DRV_OK;

	HAPPY(eDrvErr);

	PrintEnter();

	if(NULL == reason)
	{
		eDiErr = DI_ERR_INVALID_PARAM;
		DONE_ERR(eDiErr);
	}

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	eDrvErr = DRV_PM_GetWakeUpStatus(&nWakeupSrc);
	DONE_ERR((eDrvErr!=DRV_OK));

	if (nWakeupSrc == DRV_MICOM_WAKEUP_BY_TIMER
		|| nWakeupSrc == DRV_MICOM_WAKEUP_BY_WOL)
		goto process;
#endif
#if defined(CONFIG_MICOM)
	eDrvErr = DRV_MICOM_GetWakeupSrc(&nWakeupSrc);
	DONE_ERR((eDrvErr!=DRV_OK));
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
process:
#endif
	switch(nWakeupSrc)	{
	case DRV_MICOM_WAKEUP_BY_KEY:
		*reason = FRONT_KEY;
		break;
	case DRV_MICOM_WAKEUP_BY_RCU:
		*reason = IR_NEC_TYPE;
		break;
	case DRV_MICOM_WAKEUP_BY_TIMER:
		*reason = REAL_TIME_CLOCK;
		break;
	case DRV_MICOM_WAKEUP_BY_REBOOT:
		*reason = SYSTEM_REBOOT;
		break;
	case DRV_MICOM_WAKEUP_BY_RF4CE_RCU:
		*reason = RF4CE_RCU;
		break;
	case DRV_MICOM_WAKEUP_BY_FAN_LOCKED:
		*reason = FAN_LOCKED;
		break;
	case DRV_MICOM_WAKEUP_BY_OVER_TEMPERATURE:
		*reason = OVER_TEMPERATURE;
		break;
	case DRV_MICOM_WAKEUP_BY_WOL:
		*reason = WAKE_ON_LAN;
		break;
	case DRV_MICOM_WAKEUP_BY_ACPOWER:
	default:
		*reason = AC_ON;
		break;
	}

	PrintDebug("[%s] nWakeupSrc=0x%x, *reason=0x%x\n", __func__, nWakeupSrc, *reason);

#if defined(CONFIG_DEBUG)
	if(s_bKeyAgingTest == TRUE)
	{
		*reason = IR_NEC_TYPE;
		PrintError("Force Wakeup Reason to IR for Aging Test\n");
	}
#endif

	eDiErr = DI_ERR_OK;
done:
	PrintExit();
	return eDiErr;
}

DI_ERR_CODE DI_POWER_SetMainChipReset (void)
{
	DI_ERR_CODE	returnValue = DI_ERR_ERROR;
#if defined(CONFIG_NO_MICOM_POWER_CONTROL)
#else
	DRV_Error drvError;
	HUINT32	retry;
#endif

	PrintEnter();

#if !defined(CONFIG_OS_UCOS)
	sync(); /* Need to sync cache data to disk before system reboot */
#endif

#if defined(CONFIG_NO_MICOM_POWER_CONTROL)
#if defined(CONFIG_OS_UCOS)
	(void) VK_SYSTEM_Command("reboot");
#else
	reboot(RB_AUTOBOOT);
#endif
#else
	for (retry = 0; retry<MICOM_FAIL_RETRY_CNT; retry++)
	{
		drvError = DRV_MICOM_STBPowerReboot();
		if (drvError == DRV_OK)
		{
			returnValue = DI_ERR_OK;
			break;
		}
		PrintError( "DRV_MICOM_STBPowerReboot error (0x%x) \n", returnValue );
		VK_TASK_Sleep(50);
	}
#endif

	PrintExit();
	return returnValue;
}

DI_ERR_CODE DI_POWER_SetMainChipResetWaitDelay (HUINT8 delay)
{
	BSTD_UNUSED(delay);
	PrintDebug("Not Supported Funtion. Why did you call %s in KRAKEN??? \n", __func__);
	return DI_ERR_OK;
}

DI_ERR_CODE DI_POWER_PrepareStandby(DI_PM_POWERSTATE eState)
{
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	HAPPY(eState);
	/*send message*/
	return DI_ERR_OK;
#else
	HAPPY(eState);
	return DI_ERR_ERROR;
#endif
}


DI_ERR_CODE DI_POWER_CancelStandby(void)
{
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	/*send cancel message to tasks */
	return DI_ERR_OK;
#else
	return DI_ERR_ERROR;
#endif
}

DI_ERR_CODE DI_POWER_SetStandby(DI_PM_POWERSTATE eState, DI_PM_WAKEUP_SRC stSrc, DI_POWER_BOOTMODE eBootMode)
{
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	DRV_Error eDrvErr = DRV_OK;
	DRV_PM_STBY_Settings stbySettings;
	DRV_GPIO_STANDBY_STATE_E pm_state = DRV_GPIO_STANDBY_STATE_MAX;

	if(eState >= DI_PM_POWERSTATE_MAX)
	{
		PrintError("Invalied Powerstate %d\n", eState);
		return DI_ERR_ERROR;
	}

	if(stSrc.bIR == FALSE && stSrc.bFrontKey == FALSE && stSrc.bWOL == FALSE && stSrc.bTimer == FALSE)
	{
		PrintError("No wakeup sources specified. \n");
		return DI_ERR_ERROR;
	}

	if(stSrc.bTimer == TRUE && stSrc.stInfo.stTimer.ulInterval == 0)
	{
		PrintError("Time interval shall be greater than 0.\n");
		return DI_ERR_ERROR;
	}

	if(eState == DI_PM_POWERSTATE_IDLE)
	{
		PrintError("Powerstate %d is not supported yet.\n", eState);
		return DI_ERR_ERROR;
	}

	if (stSrc.bWOL)
	{
#if defined(CONFIG_ETH_EXT_PHY)
		/* do nothing */
#else
		/* if wol runs on internal phy, we should use s2 mode */
		if (eState != DI_PM_POWERSTATE_LIGHT_SLEEP)
		{
			PrintError("we should use DI_PM_POWERSTATE_LIGHT_SLEEP mode in order to support wol on internal phy\n");
			PrintError("please change sleep mode to DI_PM_POWERSTATE_LIGHT_SLEEP\n");
			return DI_ERR_ERROR;
		}
#endif
	}

	VK_TASK_SendPMMsg(SIGUSR1);
	DRV_PM_PowerDown();
	switch(eState) {
	case DI_PM_POWERSTATE_LIGHT_SLEEP:
		pm_state = DRV_GPIO_STANDBY_STATE_LIGHT_SLEEP;
		break;
	case DI_PM_POWERSTATE_DEEP_SLEEP:
		pm_state = DRV_GPIO_STANDBY_STATE_DEEP_SLEEP;
		break;
	default:
		break;
	}
	(void)DRV_GPIO_PM_Suspend(pm_state);

	eDrvErr = DRV_MICOM_SetFastBoot(TRUE);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_MICOM_SetFastBoot error!!\n", __func__);
		return DI_ERR_ERROR;
	}

	stbySettings.bIR = stSrc.bIR;
	stbySettings.bFrontKey = stSrc.bFrontKey;
	stbySettings.bWOL = stSrc.bWOL;
	stbySettings.bTimer = stSrc.bTimer;
	stbySettings.stInfo.stTimer.ulInterval = (stbySettings.bTimer)?stSrc.stInfo.stTimer.ulInterval:0;

#if defined(CONFIG_DEBUG)
	if(s_bKeyAgingTest == TRUE)
	{
		stbySettings.bTimer = TRUE;
		stbySettings.stInfo.stTimer.ulInterval = s_ulOnTimer;
		PrintError("Force Timer on for Aging Test.\n");
	}
#endif
#if defined(CONFIG_DEBUG)
	PrintError("************************************************\n");
	PrintError("** Wake-up Setting\n");
	PrintError("** \t%s  %s  \n", (eState==DI_PM_POWERSTATE_LIGHT_SLEEP)?"S2":"S3",(eBootMode==DI_POWER_BOOTMODE_WARM)?"WARM":"COLD");
	PrintError("** \tIR  : %d\n", stbySettings.bIR);
	PrintError("** \tFront : %d\n", stbySettings.bFrontKey);
	PrintError("** \tCEC : %d\n", 0);  //not yet
	PrintError("** \tWOL: %d\n", stbySettings.bWOL);
	PrintError("** \tTimer: %s (%d seconds)\n", (stbySettings.bTimer)?"On":"Off", stbySettings.stInfo.stTimer.ulInterval);
	PrintError("************************************************\n");
#endif

#if defined(CONFIG_QUANTENNA_BBIC_MAC)
	if((eState == DI_PM_POWERSTATE_LIGHT_SLEEP)&&(stbySettings.bWOL==TRUE))
	{
		/*  not used */
	}
	else
	{
		(void)DRV_WIFI_Off(0);
	}
#endif

	switch(eState) {
	case DI_PM_POWERSTATE_IDLE:
		PrintError("[%s,%d] Error! not implemented for this eState(%d)\n", __FUNCTION__, __LINE__, eState);
		break;
	case DI_PM_POWERSTATE_LIGHT_SLEEP:
#if defined(CONFIG_CHANNEL)
		(void)MAIN_CH_UnInitChannel();
#endif
		eDrvErr = DRV_PM_StandbyStates2(stbySettings);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s,%d] Error in StandbyStates2\n", __FUNCTION__, __LINE__);
		}
		break;
	case DI_PM_POWERSTATE_DEEP_SLEEP:
#if defined(CONFIG_CHANNEL)
		(void)MAIN_CH_UnInitChannel();
#endif

#if defined(CONFIG_MIPS)
		if( s3_warm_boot_calculate_cbc_mac() != 0)
		{
			PrintError("Error In s3_warm_boot_calculate_cbc_mac \n");
			return DI_ERR_ERROR;
		}
#endif

		eDrvErr = DRV_PM_StandbyStates3(eBootMode, stbySettings);
		if(eDrvErr != DRV_OK)
		{
			PrintError("[%s,%d] Error in StandbyStates3\n", __FUNCTION__, __LINE__);
		}
		break;
	default:
		PrintError("[%s,%d] Error! unknown eState(%d)\n", __FUNCTION__, __LINE__, eState);
		break;
	}

#if defined(CONFIG_MICOM)
#else
	DRV_REMOCON_ClearWakeupCode();
#endif
	(void)DRV_GPIO_PM_Resume();
 	DRV_AUDIO_HWMUTE(FALSE);
	DRV_PM_PowerUp();
#if defined(CONFIG_WATCHDOG)
	/*Watchdog mudule cares stopping watchdog set by loader. Nothing to be done here.*/
#else
	/*Stop watchdog timer set by loader*/
	if (NEXUS_Watchdog_StopTimer() != NEXUS_SUCCESS)
	{
		PrintError("[%s] Error! can't stop watchdog timer\n", __FUNCTION__);
	}
#endif
#if defined(CONFIG_CHANNEL)
	if(DI_CH_OK != MAIN_CH_ReInitChannel()) /*reinit channel driver*/
	{
		PrintError("[%s] MAIN_CH_ReInitChannel error!!\n", __func__);
		reboot(RB_AUTOBOOT);
	}
	else
	{
		PrintDebug("[%s] MAIN_CH_ReInitChannel Success!!\n", __func__);
	}
#endif

	eDrvErr = DRV_MICOM_SetFastBoot(FALSE);
	if(eDrvErr != DRV_OK)
	{
		PrintError("[%s] DRV_MICOM_SetFastBoot error!!\n", __func__);
		return DI_ERR_ERROR;
	}

	VK_TASK_SendPMMsg(SIGCONT);

#if defined(CONFIG_QUANTENNA_BBIC_MAC)
	(void)DRV_WIFI_Off(0);
	VK_TASK_Sleep(10);
	(void)DRV_WIFI_On(0);
	VK_TASK_Sleep(10);
	(void)DRV_WIFI_Off(0);
	VK_TASK_Sleep(10);
	(void)DRV_WIFI_On(0);
#endif

	return (eDrvErr==DRV_OK)?DI_ERR_OK:DI_ERR_ERROR;
#else
	HAPPY(eBootMode);
	HAPPY(eState);
	HAPPY(stSrc);
	return DI_ERR_ERROR;
#endif
}

