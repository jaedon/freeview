/**
 * di_led.c
*/

/**
 * @defgroup		DI_LED DI_LED : LED display module
 * @author		Jin-hong Kim
 * @note			LED display APIs
 * @brief			Define DI_LED APIs
 * @file 			di_led.c
*/



/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"

#include "vkernel.h"
#include "drv_err.h"
#include "drv_gpio.h"
#include "drv_pm.h"
#include "drv_led.h"

#include "di_err.h"
#include "di_led.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE				TRACE_MODULE_DI_LED


/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/



/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/



/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static struct led_platform_driver_t tLedDrv;


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

/********************************************************************
Function	: P_LED_GetDefaultSetting
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
static void P_LED_GetDefaultSetting(struct led_setting_t *pSetting)
{
	DONE_ERR((!pSetting));
	VK_MEM_Memset(pSetting, 0, sizeof(struct led_setting_t));

done:
	return;
}

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
/********************************************************************
Function	: DRV_LED_EarlyResume
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
void DRV_LED_EarlyResume(void)
{
	int rc;
	DRV_Error eDrvErr;
	DRV_MICOM_WAKEUP_SRC nWakeupSrc;
	struct led_setting_t tLedSetting;

	eDrvErr = DRV_PM_GetWakeUpStatus(&nWakeupSrc);
	if (eDrvErr != DRV_OK)
	{
		PrintError("[%s,%d] Error! DRV_PM_GetWakeUpStatus fails\n", __FUNCTION__, __LINE__);
		return;
	}
	
	switch(nWakeupSrc) {
	case DRV_MICOM_WAKEUP_BY_KEY:
	case DRV_MICOM_WAKEUP_BY_RCU:
	case DRV_MICOM_WAKEUP_BY_WOL:
		P_LED_GetDefaultSetting(&tLedSetting);

		tLedSetting.id = LED_ID_POWER;
		tLedSetting.func = LED_FUNC_WATCHING;
		
		if (tLedDrv.set)
		{
			rc = tLedDrv.set(&tLedSetting);
			DONE_ERR((rc!=0));
		}
		break;
	case DRV_MICOM_WAKEUP_BY_TIMER:
	case DRV_MICOM_WAKEUP_BY_ACPOWER:
	case DRV_MICOM_WAKEUP_BY_REBOOT:
	case DRV_MICOM_WAKEUP_BY_RF4CE_RCU:
	case DRV_MICOM_WAKEUP_BY_FAN_LOCKED:
	case DRV_MICOM_WAKEUP_BY_OVER_TEMPERATURE:
	default:
		break;
	}

done:
	return;
}


/********************************************************************
Function	: DRV_LED_Resume
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
void DRV_LED_Resume(void)
{
#if (NEXUS_VERSION < 1501)
	DRV_LED_EarlyResume();
#endif

	return;
}

/********************************************************************
Function	: DRV_LED_Suspend
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
void DRV_LED_Suspend(void)
{
	/* nothing here */
}
#endif

/********************************************************************
Function	: DI_LED_GetCapability
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE	DI_LED_GetCapability(HUINT32 *pulLedCapaMask)
{
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;

	PrintEnter();
	
	DONE_ERR((!pulLedCapaMask));
	*pulLedCapaMask = (HUINT32)tLedDrv.led_cap;

	eDiErr = DI_ERR_OK;
done:
	PrintExit();
	return eDiErr;
}

/********************************************************************
Function	: DI_LED_SetPowerLedUse
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE DI_LED_SetPowerLedUse (HBOOL onOff)
{
	HAPPY(onOff);
	return DI_ERR_NOT_SUPPORTED;
}

/********************************************************************
Function	: DI_LED_SetAll
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE DI_LED_SetAll (void)
{
	int rc;
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;
	struct led_setting_t tLedSetting;

	PrintEnter();
	
	P_LED_GetDefaultSetting(&tLedSetting);
	
	tLedSetting.id = tLedDrv.led_cap;
	tLedSetting.func = LED_FUNC_NORMAL;
	tLedSetting.on = TRUE;

	if (tLedDrv.set)
	{
		rc = tLedDrv.set(&tLedSetting);
		DONE_ERR((rc!=0));
	}

	eDiErr = DI_ERR_OK;
done:
	PrintExit();
	return eDiErr;
}

/********************************************************************
Function	: DI_LED_ClearAll
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE DI_LED_ClearAll (void)
{
	int rc;
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;
	struct led_setting_t tLedSetting;

	PrintEnter();
	
	P_LED_GetDefaultSetting(&tLedSetting);
	
	tLedSetting.id = tLedDrv.led_cap;
	tLedSetting.func = LED_FUNC_NORMAL;
	tLedSetting.on = FALSE;
	
	if (tLedDrv.set)
	{
		rc = tLedDrv.set(&tLedSetting);
		DONE_ERR((rc!=0));
	}

	eDiErr = DI_ERR_OK;
done:
	PrintExit();
	return eDiErr;
}

/********************************************************************
Function	: DI_LED_SetPowerLedState
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE DI_LED_SetPowerLedState (DI_LED_STATE state)
{
	int rc;
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;
	struct led_setting_t tLedSetting;

	PrintEnter();
	
	P_LED_GetDefaultSetting(&tLedSetting);
	
	tLedSetting.id = LED_ID_POWER;
	switch(state) {
	case DI_LED_OFF:
		tLedSetting.func = LED_FUNC_NORMAL;
		tLedSetting.on = FALSE;
		break;
	case DI_LED_WATCHING_LEDLOW:
    case DI_LED_WATCHING:
		tLedSetting.func = LED_FUNC_WATCHING;
		break;
	case DI_LED_STANDBY_LEDLOW:
    case DI_LED_STANDBY:
		tLedSetting.func = LED_FUNC_STANDBY;
		break;    
    case DI_LED_RECORDING:
		tLedSetting.func = LED_FUNC_RECORDING;
		break;
    default:
    	break;
	}	
	
	if (tLedDrv.set)
	{
		rc = tLedDrv.set(&tLedSetting);
		DONE_ERR((rc!=0));
	}

	eDiErr = DI_ERR_OK;
done:
	PrintExit();
	return eDiErr;
}


/********************************************************************
Function	: DI_LED_SetOn
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE DI_LED_SetOn (DI_LED_ID ledId)
{
	int rc;
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;
	struct led_setting_t tLedSetting;

	PrintEnter();
	DONE_ERR(!(ledId&tLedDrv.led_cap));

	P_LED_GetDefaultSetting(&tLedSetting);
	
	tLedSetting.id = (unsigned)ledId;
	tLedSetting.func = LED_FUNC_NORMAL;
	tLedSetting.on = TRUE;
	
	if (tLedDrv.set)
	{
		rc = tLedDrv.set(&tLedSetting);
		DONE_ERR((rc!=0));
	}

	eDiErr = DI_ERR_OK;
done:
	PrintExit();
	return eDiErr;
}


/********************************************************************
Function	: DI_LED_SetRGBOn
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE DI_LED_SetRGBOn (DI_LED_ID eLedID, HUINT8 ucRed, HUINT8 ucGreen, HUINT8 ucBlue)
{
	int rc;
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;
	struct led_setting_t tLedSetting;

	PrintEnter();
	DONE_ERR(!(eLedID&tLedDrv.led_cap));

	P_LED_GetDefaultSetting(&tLedSetting);
	
	tLedSetting.id = (unsigned)eLedID;
	tLedSetting.func = LED_FUNC_NORMAL;
	tLedSetting.on = TRUE;

	if (ucRed)
		tLedSetting.rgb |= LED_RGB_R;
	if (ucGreen)
		tLedSetting.rgb |= LED_RGB_G;
	if (ucBlue)
		tLedSetting.rgb |= LED_RGB_B;
	
	if (tLedDrv.set)
	{
		rc = tLedDrv.set(&tLedSetting);
		DONE_ERR((rc!=0));
	}

	eDiErr = DI_ERR_OK;
done:
	PrintExit();
	return eDiErr;
}



/********************************************************************
Function	: DI_LED_SetOff
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE DI_LED_SetOff (DI_LED_ID ledId)
{
	int rc;
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;
	struct led_setting_t tLedSetting;

	PrintEnter();
	DONE_ERR(!(ledId&tLedDrv.led_cap));

	P_LED_GetDefaultSetting(&tLedSetting);
	
	tLedSetting.id = ledId;
	tLedSetting.func = LED_FUNC_NORMAL;
	tLedSetting.on = FALSE;
	
	if (tLedDrv.set)
	{
		rc = tLedDrv.set(&tLedSetting);
		DONE_ERR((rc!=0));
	}

	eDiErr = DI_ERR_OK;
done:
	PrintExit();
	return eDiErr;
}

/********************************************************************
Function	: DI_LED_MultiSetOnOff
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE DI_LED_MultiSetOnOff (HUINT32 ledMask, HUINT32 onOffMask)
{
	HAPPY(ledMask);
	HAPPY(onOffMask);
	return DI_ERR_NOT_SUPPORTED;
}

/********************************************************************
Function	: DI_LED_SetBlinkOn
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE DI_LED_SetBlinkOn (DI_LED_ID ledId, HUINT32 delay)
{
	HAPPY(ledId);
	HAPPY(delay);
	return DI_ERR_NOT_SUPPORTED;
}

/********************************************************************
Function	: DI_LED_SetBlinkOff
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE DI_LED_SetBlinkOff (void)
{
	return DI_ERR_NOT_SUPPORTED;
}

/********************************************************************
Function	: DI_LED_MultiSetBlinkOn
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE DI_LED_MultiSetBlinkOn (HUINT32 ledMask, HUINT32 onOffMask, HUINT32 delay)
{
	HAPPY(ledMask);
	HAPPY(onOffMask);
	HAPPY(delay);
	return DI_ERR_NOT_SUPPORTED;
}

/********************************************************************
Function	: DI_LED_SetPowerLedDimmingLevel
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE DI_LED_SetPowerLedDimmingLevel (HUINT8 ucDimLevel)
{
	int rc;
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;
	struct led_setting_t tLedSetting;

	PrintEnter();
	DONE_ERR((!tLedDrv.use_dimming));
	
	P_LED_GetDefaultSetting(&tLedSetting);
	
	tLedSetting.id = LED_ID_POWER;
	tLedSetting.func = LED_FUNC_WATCHING;
	tLedSetting.dim_set = TRUE;
	tLedSetting.dim_level = ucDimLevel;
	
	if (tLedDrv.set)
	{
		rc = tLedDrv.set(&tLedSetting);
		DONE_ERR((rc!=0));
	}

	eDiErr = DI_ERR_OK;
done:
	PrintExit();
	return eDiErr;
}



/********************************************************************
Function	: DI_LED_SetStandbyLedDimmingLevel
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE DI_LED_SetStandbyLedDimmingLevel (HUINT8 ucDimLevel)
{
	int rc;
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;
	struct led_setting_t tLedSetting;

	PrintEnter();
	DONE_ERR((!tLedDrv.use_dimming));

	P_LED_GetDefaultSetting(&tLedSetting);
	
	tLedSetting.id = LED_ID_POWER;
	tLedSetting.func = LED_FUNC_STANDBY;
	tLedSetting.dim_set = TRUE;
	tLedSetting.dim_level = ucDimLevel;
	
	if (tLedDrv.set)
	{
		rc = tLedDrv.set(&tLedSetting);
		DONE_ERR((rc!=0));
	}

	eDiErr = DI_ERR_OK;
done:
	PrintExit();
	return eDiErr;
}


/********************************************************************
Function	: DI_LED_SetLedsDimmingLevel
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DI_ERR_CODE DI_LED_SetLedsDimmingLevel (DI_LED_ID eLedID, HUINT8 ucDimLevel)
{
	int rc;
	DI_ERR_CODE eDiErr = DI_ERR_ERROR;
	struct led_setting_t tLedSetting;

	PrintEnter();
	DONE_ERR(!(eLedID&tLedDrv.led_cap));
	DONE_ERR((!tLedDrv.use_dimming));

	P_LED_GetDefaultSetting(&tLedSetting);
	
	tLedSetting.id = (unsigned)eLedID;
	tLedSetting.func = LED_FUNC_NORMAL;
	tLedSetting.on = TRUE;
	tLedSetting.dim_set = TRUE;
	tLedSetting.dim_level = ucDimLevel;
	
	if (tLedDrv.set)
	{
		rc = tLedDrv.set(&tLedSetting);
		DONE_ERR((rc!=0));
	}

	eDiErr = DI_ERR_OK;
done:
	PrintExit();
	return eDiErr;
}


/********************************************************************
Function	: DRV_LED_Init
Description :
Input		: 
Output	: 
Return	: 
*********************************************************************/
DRV_Error DRV_LED_Init(void)
{
	DRV_Error eDrvErr = DRV_ERR;
	
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	DRV_PM_RegisterCallback(DRV_LED_Suspend, DRV_LED_Resume);
#endif

	eDrvErr = DRV_CFG_LED_PlatformDrvInit(&tLedDrv);
	DONE_ERR((eDrvErr!=DRV_OK));

	eDrvErr = DRV_OK;
done:
	return eDrvErr;
}

