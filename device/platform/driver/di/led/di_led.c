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
#include "bstd.h"

#include "vkernel.h"
#include "drv_err.h"
#include "drv_micom.h"
#include "drv_gpio.h"
#include "drv_pwm.h"
#include "drv_pm.h"

#include "di_err.h"
#include "di_led.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE				TRACE_MODULE_DI_LED

#define POWER_BLUE_LED_ON 		0x01
#define POWER_RED_LED_ON 		0x00
#define POWER_LED_OFF    		0x02

#ifndef SET_LED_OFF
#if defined(CONFIG_LED_BOTH)
	#define SET_LED_OFF
#elif defined(CONFIG_LED_MICOM)
	#define SET_LED_OFF    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_LED_OFF);\
	}while(0)
#elif defined(CONFIG_LED_GPIO_STBY_ONLY)
	#if defined(CONFIG_TEMP_CDIG2000C) || defined(CONFIG_TEMP_HTB100S)/* Rocky Project will be removed */
	#define SET_LED_OFF    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_LED_OFF);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
	}while(0)
	#else
	#define SET_LED_OFF    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_LED_OFF);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);\
	}while(0)
	#endif
#elif defined(CONFIG_LED_GPIO_PWM_AND_THREE_STATE)
	#define SET_LED_OFF    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_LED_OFF);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY2, GPIO_LOW);\
	}while(0)
#elif defined(CONFIG_LED_GPIO_AND_FOURE_STATE)
	#define SET_LED_OFF    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_LED_OFF);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);\
		(void)DRV_GPIO_ChangeMode(GPIO_ID_LED_STBY2, GPIO_MODE_READ, GPIO_INVAILD);\
		(void)DRV_GPIO_ChangeMode(GPIO_ID_LED_POWER2, GPIO_MODE_READ, GPIO_INVAILD);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBYCTL, GPIO_HI);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWERCTL, GPIO_HI);\
	}while(0)
#elif defined(CONFIG_LED_GPIO)
	#define SET_LED_OFF    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_LED_OFF);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);\
	}while(0)
#endif
#endif

#ifndef SET_LED_WATCHING
#if defined(CONFIG_LED_BOTH)
	#define SET_LED_WATCHING    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_BLUE_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_WORK, GPIO_HI);\
	}while(0)
#elif defined(CONFIG_LED_MICOM)
	#define SET_LED_WATCHING    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_BLUE_LED_ON);\
	}while(0)
#elif defined(CONFIG_LED_GPIO_STBY_ONLY)
	#if defined(CONFIG_TEMP_CDIG2000C) || defined(CONFIG_TEMP_HTB100S)/* Rocky Project will be removed */
	#define SET_LED_WATCHING    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_BLUE_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);\
	}while(0)
	#else
	#define SET_LED_WATCHING    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_BLUE_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
	}while(0)
	#endif
#elif defined(CONFIG_LED_GPIO_PWM_AND_THREE_STATE)
	#define SET_LED_WATCHING    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_BLUE_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY2, GPIO_HI);\
	}while(0)
#elif defined(CONFIG_LED_GPIO_AND_FOURE_STATE)
	#define SET_LED_WATCHING    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_BLUE_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_HI);\
		(void)DRV_GPIO_ChangeMode(GPIO_ID_LED_STBY2, GPIO_MODE_READ, GPIO_INVAILD);\
		(void)DRV_GPIO_ChangeMode(GPIO_ID_LED_POWER2, GPIO_MODE_READ, GPIO_INVAILD);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBYCTL, GPIO_HI);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWERCTL, GPIO_LOW);\
	}while(0)
#elif defined(CONFIG_LED_GPIO)
	#define SET_LED_WATCHING    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_BLUE_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);\
	}while(0)
#endif
#endif

#ifndef SET_LED_STANDBY
#if defined(CONFIG_LED_BOTH)
	#define SET_LED_STANDBY    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_WORK, GPIO_LOW);\
	}while(0)
#elif defined(CONFIG_LED_MICOM)
	#define SET_LED_STANDBY    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
	}while(0)
#elif defined(CONFIG_LED_GPIO_STBY_ONLY)
	#if defined(CONFIG_TEMP_CDIG2000C) || defined(CONFIG_TEMP_HTB100S)/* Rocky Project will be removed */
	#define SET_LED_STANDBY    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
	}while(0)
	#else
	#define SET_LED_STANDBY    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);\
	}while(0)
	#endif
#elif defined(CONFIG_LED_GPIO_PWM_AND_THREE_STATE)
	#define SET_LED_STANDBY    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_HI);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY2, GPIO_HI);\
	}while(0)
#elif defined(CONFIG_LED_GPIO_AND_FOURE_STATE)
	#define SET_LED_STANDBY    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);\
		(void)DRV_GPIO_ChangeMode(GPIO_ID_LED_STBY2, GPIO_MODE_READ, GPIO_INVAILD);\
		(void)DRV_GPIO_ChangeMode(GPIO_ID_LED_POWER2, GPIO_MODE_READ, GPIO_INVAILD);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBYCTL, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWERCTL, GPIO_HI);\
	}while(0)
#elif defined(CONFIG_LED_GPIO)
	#define SET_LED_STANDBY    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_HI);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
	}while(0)
#endif
#endif

#ifndef SET_LED_RECORDING
#if defined(CONFIG_LED_BOTH)
	#define SET_LED_RECORDING    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
  		(void)DRV_GPIO_Write(GPIO_ID_LED_WORK, GPIO_HI);\
  	}while(0)
#elif defined(CONFIG_LED_MICOM)
	#define SET_LED_RECORDING    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
  	}while(0)
#elif defined(CONFIG_LED_GPIO_STBY_ONLY)
	#if defined(CONFIG_TEMP_CDIG2000C) || defined(CONFIG_TEMP_HTB100S)/* Rocky Project will be removed */
	#define SET_LED_RECORDING    do {\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
	}while(0)
	#else
	#define SET_LED_RECORDING    do {\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);\
	}while(0)
	#endif
#elif defined(CONFIG_LED_GPIO_PWM_AND_THREE_STATE)
	#define SET_LED_RECORDING    do {\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY2, GPIO_LOW);\
	}while(0)
#elif defined(CONFIG_LED_GPIO_AND_FOURE_STATE)
	#define SET_LED_RECORDING    do {\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);\
		(void)DRV_GPIO_ChangeMode(GPIO_ID_LED_STBY2, GPIO_MODE_READ, GPIO_INVAILD);\
		(void)DRV_GPIO_ChangeMode(GPIO_ID_LED_POWER2, GPIO_MODE_READ, GPIO_INVAILD);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBYCTL, GPIO_HI);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWERCTL, GPIO_HI);\
	}while(0)
#elif defined(CONFIG_LED_GPIO)
	#define SET_LED_RECORDING    do {\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_HI);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
	}while(0)
#endif
#endif

#ifndef SET_LED_STANDBY_LEDLOW
#if defined(CONFIG_LED_BOTH)
	#define SET_LED_STANDBY_LEDLOW    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_WORK, GPIO_LOW);\
	}while(0)
#elif defined(CONFIG_LED_MICOM)
	#define SET_LED_STANDBY_LEDLOW    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
	}while(0)
#elif defined(CONFIG_LED_GPIO_STBY_ONLY)
	#if defined(CONFIG_TEMP_CDIG2000C) || defined(CONFIG_TEMP_HTB100S)/* Rocky Project will be removed */
	#define SET_LED_STANDBY_LEDLOW    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
	}while(0)
	#else
	#define SET_LED_STANDBY_LEDLOW    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);\
	}while(0)
	#endif
#elif defined(CONFIG_LED_GPIO_PWM_AND_THREE_STATE)
	#define SET_LED_STANDBY_LEDLOW    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_HI);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY2, GPIO_LOW);\
	}while(0)
#elif defined(CONFIG_LED_GPIO_AND_FOURE_STATE)
	#define SET_LED_STANDBY_LEDLOW    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);\
		(void)DRV_GPIO_ChangeMode(GPIO_ID_LED_STBY2, GPIO_MODE_WRITE, GPIO_LOW);\
		(void)DRV_GPIO_ChangeMode(GPIO_ID_LED_POWER2, GPIO_MODE_READ, GPIO_INVAILD);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBYCTL, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWERCTL, GPIO_HI);\
	}while(0)
#elif defined(CONFIG_LED_GPIO)
	#define SET_LED_STANDBY_LEDLOW    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_HI);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
	}while(0)
#endif
#endif

#ifndef SET_LED_WATCHING_LEDLOW
#if defined(CONFIG_LED_BOTH)
	#define SET_LED_WATCHING_LEDLOW    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_WORK, GPIO_LOW);\
	}while(0)
#elif defined(CONFIG_LED_MICOM)
	#define SET_LED_WATCHING_LEDLOW    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
	}while(0)
#elif defined(CONFIG_LED_GPIO_STBY_ONLY)
	#if defined(CONFIG_TEMP_CDIG2000C) || defined(CONFIG_TEMP_HTB100S)/* Rocky Project will be removed */
	#define SET_LED_WATCHING_LEDLOW    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
	}while(0)
	#else
	#define SET_LED_WATCHING_LEDLOW    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);\
	}while(0)
	#endif
#elif defined(CONFIG_LED_GPIO_PWM_AND_THREE_STATE)
	#define SET_LED_WATCHING_LEDLOW    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY2, GPIO_LOW);\
	}while(0)
#elif defined(CONFIG_LED_GPIO_AND_FOURE_STATE)
	#define SET_LED_WATCHING_LEDLOW    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);\
		(void)DRV_GPIO_ChangeMode(GPIO_ID_LED_STBY2, GPIO_MODE_READ, GPIO_INVAILD);\
		(void)DRV_GPIO_ChangeMode(GPIO_ID_LED_POWER2, GPIO_MODE_WRITE, GPIO_LOW);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBYCTL, GPIO_HI);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWERCTL, GPIO_LOW);\
	}while(0)
#elif defined(CONFIG_LED_GPIO)
	#define SET_LED_WATCHING_LEDLOW    do {\
		(void)DRV_MICOM_FrontLedControl(POWER_RED_LED_ON);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_HI);\
		(void)DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);\
	}while(0)
#endif
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
enum {
	LED_STATE_OFF = 0,
	LED_STATE_WATCHING,
	LED_STATE_STANDBY,
	LED_STATE_STANDBY_LEDLOW,
	LED_STATE_WATCHING_LEDLOW,
	LED_STATE_RECORDING,
	LED_STATE_MAX
};

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
//DRV_LED_INFO g_led_instance[LED_MAX];

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
//HUINT32		s_ulLedStatus;
#if defined(CONFIG_LED_CAPABILITY_MODEL) /* LED_CAPABILITY in di_config.c */
extern HUINT32 LED_CAPABILITY;
#else
#if	defined(CONFIG_FRONT_MICOM_JAPAN)
static HUINT32 LED_CAPABILITY = (DI_LED_NUM0|DI_LED_NUM1|DI_LED_NUM2|DI_LED_NUM3);
#else	// EU Model
#if	defined(CONFIG_PANEL_LCD)
static HUINT32 LED_CAPABILITY = (DI_LED_NUM0|DI_LED_NUM1
								|DI_LED_NUM2|DI_LED_NUM3|DI_LED_NUM4|DI_LED_NUM5
								|DI_LED_NUM6|DI_LED_NUM7|DI_LED_NUM8|DI_LED_NUM9
								|DI_LED_NUM10|DI_LED_NUM11|DI_LED_NUM12|DI_LED_NUM13
								|DI_LED_NUM14|DI_LED_NUM15|DI_LED_NUM16|DI_LED_NUM17
								|DI_LED_NUM18|DI_LED_NUM19|DI_LED_NUM20|DI_LED_NUM21);
#elif defined(CONFIG_PANEL_VFD)
static HUINT32 LED_CAPABILITY = (DI_LED_NUM0|DI_LED_NUM1									// TV | RADIO
								|DI_LED_NUM2|DI_LED_NUM3|DI_LED_NUM4|DI_LED_NUM5			// FULL | HD | DOLBI | +
								|DI_LED_NUM6|DI_LED_NUM7|DI_LED_NUM8|DI_LED_NUM9			// DOLBI+ | USB | REC | PLAY
								|DI_LED_NUM10|DI_LED_NUM11|DI_LED_NUM12|DI_LED_NUM13		// PAUSE | TIME | WIFI | LINK
								|DI_LED_NUM14|DI_LED_NUM15|DI_LED_NUM16|DI_LED_NUM17		// MAIL | BETTERY 1+BORDER | BETTERY 2 | BETTERY 3
								|DI_LED_NUM18|DI_LED_NUM19|DI_LED_NUM20|DI_LED_NUM21);		// BETTERY 4 | LED 1 | LED 2 | LED 3
#elif defined(CONFIG_PANEL_7SEG)
static HUINT32 LED_CAPABILITY = (DI_LED_NUM0|DI_LED_NUM1);									// TV | RADIO
#else
static HUINT32 LED_CAPABILITY = (DI_LED_NUM0|DI_LED_NUM1
								|DI_LED_NUM2|DI_LED_NUM3|DI_LED_NUM4|DI_LED_NUM5
								|DI_LED_NUM6|DI_LED_NUM7|DI_LED_NUM8|DI_LED_NUM9
								|DI_LED_NUM10|DI_LED_NUM11|DI_LED_NUM12|DI_LED_NUM13
								|DI_LED_NUM14|DI_LED_NUM15|DI_LED_NUM16|DI_LED_NUM17
								|DI_LED_NUM18|DI_LED_NUM19|DI_LED_NUM20|DI_LED_NUM21);
#endif
#endif
#endif

#if defined(CONFIG_LED_GPIO_PWM_AND_THREE_STATE)
#define PWM_PERIOD_INTERVAL_FREQ_50KHZ		(0xfe)
#define PWM_CONTROL_WORD_FREQ_50KHZ			(0x7900)
#endif


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
static DRV_Error P_LED_SetPowerLedState(HUINT32 nState)
{
	PrintEnter();

	switch(nState) {
	case LED_STATE_WATCHING:
#if defined(SET_LED_WATCHING)
		SET_LED_WATCHING;
#endif
		break;
	case LED_STATE_STANDBY:
#if defined(SET_LED_STANDBY)
		SET_LED_STANDBY;
#endif
		break;
	case LED_STATE_STANDBY_LEDLOW:
#if defined(SET_LED_STANDBY_LEDLOW)
		SET_LED_STANDBY_LEDLOW;
#endif
		break;
	case LED_STATE_WATCHING_LEDLOW:
#if defined(SET_LED_WATCHING_LEDLOW)
		SET_LED_WATCHING_LEDLOW;
#endif
		break;
	case LED_STATE_RECORDING:
#if defined(SET_LED_RECORDING)
		SET_LED_RECORDING;
#endif
		break;
	case LED_STATE_OFF:
	default:
#if defined(SET_LED_OFF)
		SET_LED_OFF;
#endif
		break;
	}

	PrintExit();
	return DRV_OK;
}

#if defined(CONFIG_LED_GPIO_PWM_AND_THREE_STATE)
DRV_Error P_LED_PwmInit(void)
{
	DRV_Error eDrvErr = DRV_ERR;

	PrintEnter();

	eDrvErr = DRV_PWM_Init(CONFIG_LED_GPIO_PWM_CHANNEL);
	DONE_ERR((eDrvErr!=DRV_OK));

	eDrvErr = DRV_PWM_SetControlWord(CONFIG_LED_GPIO_PWM_CHANNEL, PWM_CONTROL_WORD_FREQ_50KHZ);
	DONE_ERR((eDrvErr!=DRV_OK));

	eDrvErr = DRV_PWM_SetPeriodInterval(CONFIG_LED_GPIO_PWM_CHANNEL, PWM_PERIOD_INTERVAL_FREQ_50KHZ);
	DONE_ERR((eDrvErr!=DRV_OK));

	eDrvErr = DRV_PWM_SetOnInterval(CONFIG_LED_GPIO_PWM_CHANNEL, 255);
	DONE_ERR((eDrvErr!=DRV_OK));

	eDrvErr = DRV_PWM_Start(CONFIG_LED_GPIO_PWM_CHANNEL);
	DONE_ERR((eDrvErr!=DRV_OK));

	eDrvErr = DRV_OK;
done:
	PrintExit();
	return eDrvErr;
}

DRV_Error P_LED_PwmReInit(void)
{
	DRV_Error eDrvErr = DRV_OK;

	PrintEnter();

	eDrvErr = DRV_PWM_SetControlWord(CONFIG_LED_GPIO_PWM_CHANNEL, PWM_CONTROL_WORD_FREQ_50KHZ);
	DONE_ERR((eDrvErr!=DRV_OK));

	eDrvErr = DRV_PWM_SetPeriodInterval(CONFIG_LED_GPIO_PWM_CHANNEL, PWM_PERIOD_INTERVAL_FREQ_50KHZ);
	DONE_ERR((eDrvErr!=DRV_OK));

	eDrvErr = DRV_PWM_SetOnInterval(CONFIG_LED_GPIO_PWM_CHANNEL, 255);
	DONE_ERR((eDrvErr!=DRV_OK));

	eDrvErr = DRV_PWM_Start(CONFIG_LED_GPIO_PWM_CHANNEL);
	DONE_ERR((eDrvErr!=DRV_OK));

	eDrvErr = DRV_OK;
done:
	PrintExit();
	return eDrvErr;
}
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
/********************************************************************
Function	: DRV_LED_Resume
Description :
Input		:
Output	: none
Return	: DRV_OK
*********************************************************************/
void DRV_LED_EarlyResume(void)
{
	DRV_Error eDrvErr;
	DRV_MICOM_WAKEUP_SRC nWakeupSrc;

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
		eDrvErr = P_LED_SetPowerLedState(LED_STATE_WATCHING);
		if (eDrvErr != DRV_OK)
		{
			PrintError("[%s,%d] Error! P_LED_SetPowerLedState fails\n", __FUNCTION__, __LINE__);
			/* go through */
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

	return;
}

/********************************************************************
Function	: DRV_LED_Resume
Description :
Input		:
Output	: none
Return	: DRV_OK
*********************************************************************/
void DRV_LED_Resume(void)
{
	DRV_Error eDrvErr;

	HAPPY(eDrvErr);

#if (NEXUS_VERSION < 1501)
	DRV_LED_EarlyResume();
#endif
#if defined(CONFIG_LED_GPIO_PWM_AND_THREE_STATE)
 	eDrvErr = P_LED_PwmReInit();
	if (eDrvErr != DRV_OK)
	{
		PrintError("[%s,%d] Error! DRV_LED_PWM_Init fails\n", __FUNCTION__, __LINE__);
		/* go through */
	}
#endif

	return;
}

/********************************************************************
Function	: DRV_LED_Suspend
Description :
Input		:
Output	: none
Return	: DRV_OK
*********************************************************************/
void DRV_LED_Suspend(void)
{
	/* nothing here */
}
#endif

DI_ERR_CODE	DI_LED_GetCapability(HUINT32 *pulLedCapaMask)
{
	*pulLedCapaMask = LED_CAPABILITY;

	return DI_ERR_OK;
}


DI_ERR_CODE DI_LED_SetPowerLedUse (HBOOL onOff)
{
	BSTD_UNUSED(onOff);

	PrintError("[%s] icordhd2 can not off POWER LED, POWER LED is always Blue or Red \n", __func__);

	return DI_ERR_NOT_SUPPORTED;
}

DI_ERR_CODE DI_LED_SetAll (void)
{
	DRV_Error nErr = DRV_OK;

	PrintEnter();

#if defined(CONFIG_LED_MICOM)
	nErr = DRV_MICOM_SetLedControl(LED_CAPABILITY, TRUE);
	if(nErr != DRV_OK)
	{
		PrintError("DRV_MICOM_FrontLedControl() returns error\n");
		return DI_ERR_ERROR;
	}
#endif

	PrintExit();

	return nErr;
}

DI_ERR_CODE DI_LED_ClearAll (void)
{
	DRV_Error	nErr = DRV_OK;

	HAPPY(nErr);
	PrintEnter();

#if defined(SET_LED_OFF)
	SET_LED_OFF;
#endif
#if defined(CONFIG_LED_MICOM)
	nErr = DRV_MICOM_SetLedControl(LED_CAPABILITY, FALSE);
	if(nErr != DRV_OK)
	{
		PrintError("DRV_MICOM_FrontLedControl() returns error\n");
		return DI_ERR_ERROR;
	}
#endif

	PrintExit();
	return DI_ERR_OK;
}

DI_ERR_CODE DI_LED_SetPowerLedState (DI_LED_STATE state)
{
	DRV_Error	eDrvErr = DRV_OK;
	HUINT32 nLedState;

	PrintEnter();

	switch(state) {
    case DI_LED_WATCHING:
		nLedState = LED_STATE_WATCHING;
		break;
    case DI_LED_STANDBY:
		nLedState = LED_STATE_STANDBY;
		break;
    case DI_LED_STANDBY_LEDLOW:
		nLedState = LED_STATE_STANDBY_LEDLOW;
		break;
    case DI_LED_WATCHING_LEDLOW:
		nLedState = LED_STATE_WATCHING_LEDLOW;
		break;
    case DI_LED_RECORDING:
		nLedState = LED_STATE_RECORDING;
		break;
	case DI_LED_OFF:
	default:
		nLedState = LED_STATE_OFF;
		break;
	}

	eDrvErr = P_LED_SetPowerLedState(nLedState);

	PrintExit();
	return eDrvErr;
}

DI_ERR_CODE DI_LED_SetOn (DI_LED_ID ledId)
{
	DRV_Error	nErr = DRV_OK;

	HAPPY(ledId);
	 
	PrintEnter();

#if	defined(CONFIG_LED_GPIO)
	{
		GPIO_ID_t	nLED_GPIO_ID;

		switch((ledId & LED_CAPABILITY)) {
		case DI_LED_NUM0:
			nLED_GPIO_ID = GPIO_ID_LED_TV;
			break;
		case DI_LED_NUM1:
			nLED_GPIO_ID = GPIO_ID_LED_RADIO;
			break;
		case DI_LED_NUM8:
			nLED_GPIO_ID = GPIO_ID_LED_STBY;
			break;
		case DI_LED_NUM10:
			nLED_GPIO_ID = GPIO_ID_IP_LED;
			break;
		default:
			PrintError("[%s] Warning! LED Id(0x%x) is not supported. Capability(0x%x)\n", __FUNCTION__, ledId, LED_CAPABILITY);
			return DI_ERR_OK;
		}

		nErr = DRV_GPIO_Write(nLED_GPIO_ID, GPIO_HI);
		if(nErr != DRV_OK)
		{
			PrintError("DRV_GPIO_Write() returns error\n");
			return DI_ERR_ERROR;
		}
	}
#elif defined(CONFIG_LED_MICOM)
	nErr = DRV_MICOM_SetLedControl((HUINT32)ledId, TRUE);
	if(nErr != DRV_OK)
	{
		PrintError("DRV_MICOM_FrontLedControl() returns error\n");
		return DI_ERR_ERROR;
	}
#endif

#if defined(CONFIG_LED_BOTH)
	nErr = DRV_GPIO_Write(GPIO_ID_LED_WORK, GPIO_HI);
	if(nErr != DRV_OK)
	{
		PrintError("DRV_GPIO_Write() returns error\n");
		return DI_ERR_ERROR;
	}
#endif

	PrintExit();
	return nErr;
}

DI_ERR_CODE DI_LED_SetRGBOn (DI_LED_ID eLedID, HUINT8 ucRed, HUINT8 ucGreen, HUINT8 ucBlue)
{
	PrintEnter();
#if defined(CONFIG_LED_GPIO_RGB_FULL_COLOR)

	PrintDebug(" [DI_LED_SetRGBOn] eLedID(%d), ucRed(%d), ucGreen(%d), ucBlue(%d), LED_CAPABILITY(0x%x)\n", eLedID, ucRed, ucGreen, ucBlue, LED_CAPABILITY);

	switch((eLedID & LED_CAPABILITY))
	{
		case DI_LED_NUM2:
			/* Set R, G and B to Open drain output */
			DRV_GPIO_ChangeMode(GPIO_ID_LED_REC_RED, GPIO_MODE_READ, GPIO_INVAILD);
			DRV_GPIO_ChangeMode(GPIO_ID_LED_REC_GREEN, GPIO_MODE_READ, GPIO_INVAILD);
			DRV_GPIO_ChangeMode(GPIO_ID_LED_REC_BLUE, GPIO_MODE_READ, GPIO_INVAILD);

			/* Set color : Active Low */
			if (ucRed)
			{
				DRV_GPIO_ChangeMode(GPIO_ID_LED_REC_RED, GPIO_MODE_WRITE, GPIO_LOW);
			}
			if (ucGreen)
			{
				DRV_GPIO_ChangeMode(GPIO_ID_LED_REC_GREEN, GPIO_MODE_WRITE, GPIO_LOW);
			}
			if (ucBlue)
			{
				DRV_GPIO_ChangeMode(GPIO_ID_LED_REC_BLUE, GPIO_MODE_WRITE, GPIO_LOW);
			}
			break;

		case DI_LED_NUM3:
			/* Set R, G and B to Open drain output */
			DRV_GPIO_ChangeMode(GPIO_ID_LED_NETWORK_RED, GPIO_MODE_READ, GPIO_INVAILD);
			DRV_GPIO_ChangeMode(GPIO_ID_LED_NETWORK_GREEN, GPIO_MODE_READ, GPIO_INVAILD);
			DRV_GPIO_ChangeMode(GPIO_ID_LED_NETWORK_BLUE, GPIO_MODE_READ, GPIO_INVAILD);

			/* Set color : Active Low */
			if (ucRed)
			{
				DRV_GPIO_ChangeMode(GPIO_ID_LED_NETWORK_RED, GPIO_MODE_WRITE, GPIO_LOW);
			}
			if (ucGreen)
			{
				DRV_GPIO_ChangeMode(GPIO_ID_LED_NETWORK_GREEN, GPIO_MODE_WRITE, GPIO_LOW);
			}
			if (ucBlue)
			{
				DRV_GPIO_ChangeMode(GPIO_ID_LED_NETWORK_BLUE, GPIO_MODE_WRITE, GPIO_LOW);
			}
			break;

		default:
			PrintError("[%s] Warning! LED Id(0x%x) is not supported. Capability(0x%x)\n", __FUNCTION__, eLedID, LED_CAPABILITY);
			return DI_ERR_OK;
	}

#else
	BSTD_UNUSED(eLedID);
	BSTD_UNUSED(ucRed);
	BSTD_UNUSED(ucGreen);
	BSTD_UNUSED(ucBlue);
#endif

	PrintExit();

	return DI_ERR_OK;

}


DI_ERR_CODE DI_LED_SetOff (DI_LED_ID ledId)
{
	DRV_Error	nErr = DRV_OK;

	HAPPY(ledId);

	PrintEnter();

#if	defined(CONFIG_LED_GPIO)
	{
		GPIO_ID_t	nLED_GPIO_ID;

		switch((ledId & LED_CAPABILITY)) {
		case DI_LED_NUM0:
			nLED_GPIO_ID = GPIO_ID_LED_TV;
			break;
		case DI_LED_NUM1:
			nLED_GPIO_ID = GPIO_ID_LED_RADIO;
			break;
		case DI_LED_NUM8:
			nLED_GPIO_ID = GPIO_ID_LED_STBY;
			break;
		case DI_LED_NUM10:
			nLED_GPIO_ID = GPIO_ID_IP_LED;
			break;
		default:
			PrintError("[%s] Warning! LED Id(0x%x) is not supported. Capability(0x%x)\n", __FUNCTION__, ledId, LED_CAPABILITY);
			return DI_ERR_OK;
		}

		nErr = DRV_GPIO_Write(nLED_GPIO_ID, GPIO_LOW);
		if(nErr != DRV_OK)
		{
			PrintError("DRV_GPIO_Write() returns error\n");
			return DI_ERR_ERROR;
		}
	}
#elif defined(CONFIG_LED_MICOM)
	nErr = DRV_MICOM_SetLedControl((HUINT32)ledId, FALSE);
	if(nErr != DRV_OK)
	{
		PrintError("DRV_MICOM_FrontLedControl() returns error\n");
		return DI_ERR_ERROR;
	}
#endif

#if defined(CONFIG_LED_BOTH)
	nErr = DRV_GPIO_Write(GPIO_ID_LED_WORK, GPIO_LOW);
	if(nErr != DRV_OK)
	{
		PrintError("DRV_GPIO_Write() returns error\n");
		return DI_ERR_ERROR;
	}
#endif

	PrintExit();

	return nErr;
}


DI_ERR_CODE DI_LED_MultiSetOnOff (HUINT32 ledMask, HUINT32 onOffMask)
{
	BSTD_UNUSED(ledMask);
	BSTD_UNUSED(onOffMask);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_LED_SetBlinkOn (DI_LED_ID ledId, HUINT32 delay)
{
	BSTD_UNUSED(ledId);
	BSTD_UNUSED(delay);

	return DI_ERR_OK;
}

DI_ERR_CODE DI_LED_SetBlinkOff (void)
{
	return DI_ERR_OK;
}

DI_ERR_CODE DI_LED_MultiSetBlinkOn (HUINT32 ledMask, HUINT32 onOffMask, HUINT32 delay)
{
	BSTD_UNUSED(ledMask);
	BSTD_UNUSED(onOffMask);
	BSTD_UNUSED(delay);

	return DI_ERR_OK;
}

static unsigned char P_LED_DimmingTransLevel(HUINT8 ucLevel)
{
	unsigned char ucTransLevel = 0;

	/*
		ucLevel		ucTransLevel
		~~~~~~      ~~~~~~~~~
		100			255
		99			252
		98			249
		...			...
		51			108
		50			105
		49			103
		...			...
		2			9
		1			7
		0			0
	*/
	if (ucLevel >= DI_LED_DIM_LEVEL_MAX)
		ucTransLevel = 255;
	else if (ucLevel == 0)
		ucTransLevel = 0;
	else if(ucLevel >= (DI_LED_DIM_LEVEL_MAX/2) )		// >50 && <100
		ucTransLevel = 255 - 3*(DI_LED_DIM_LEVEL_MAX-ucLevel);
	else	// >0 && <50
		ucTransLevel = ucLevel*2+5;

	return ucTransLevel;
}


DI_ERR_CODE DI_LED_SetPowerLedDimmingLevel (HUINT8 ucDimLevel)
{
	DRV_Error	nDrvErr = DRV_OK;
	unsigned char ucMicomDimLevel;

	PrintEnter();

	ucMicomDimLevel = P_LED_DimmingTransLevel(ucDimLevel);
#if defined(CONFIG_LED_GPIO_PWM_AND_THREE_STATE)
	nDrvErr = DRV_PWM_SetOnInterval(CONFIG_LED_GPIO_PWM_CHANNEL, ucMicomDimLevel);
#else
	nDrvErr = DRV_MICOM_SetLEDDimmingLevel(ucMicomDimLevel);
#endif
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_SetLEDDimmingLevel() \n", nDrvErr);
		return	DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_LED_SetStandbyLedDimmingLevel (HUINT8 ucDimLevel)
{
	DRV_Error	nDrvErr = DRV_OK;
	unsigned char ucMicomDimLevel;

	PrintEnter();

	ucMicomDimLevel = P_LED_DimmingTransLevel(ucDimLevel);
	nDrvErr = DRV_MICOM_SetLEDDimmingLevel(ucMicomDimLevel);
	if (nDrvErr != DRV_OK)
	{
		PrintError("Error(0x%x) In DRV_MICOM_SetLEDDimmingLevel() \n", nDrvErr);
		return	DI_ERR_ERROR;
	}

	PrintExit();

	return DI_ERR_OK;
}

DI_ERR_CODE DI_LED_SetLedsDimmingLevel (DI_LED_ID eLedID, HUINT8 ucDimLevel)
{
#if defined(CONFIG_LED_GPIO_DIMMING_FIXED_LEVEL)
	PrintEnter();

	switch((eLedID & LED_CAPABILITY))
	{
		case DI_LED_NUM2:
			if (ucDimLevel ==100) /* turn off */
			{
				DRV_GPIO_ChangeMode(GPIO_ID_LED_REC_HIGH_BRIGHTNESS, GPIO_MODE_READ, GPIO_INVAILD);
				DRV_GPIO_ChangeMode(GPIO_ID_LED_REC_LOW_BRIGHTNESS, GPIO_MODE_READ, GPIO_INVAILD);
			}
			else if (ucDimLevel > 50) /* Low Brightness */
			{
				DRV_GPIO_ChangeMode(GPIO_ID_LED_REC_HIGH_BRIGHTNESS, GPIO_MODE_READ, GPIO_INVAILD);
				DRV_GPIO_ChangeMode(GPIO_ID_LED_REC_LOW_BRIGHTNESS, GPIO_MODE_WRITE, GPIO_LOW);
			}
			else  /* High Brightness */
			{
				DRV_GPIO_ChangeMode(GPIO_ID_LED_REC_LOW_BRIGHTNESS, GPIO_MODE_READ, GPIO_INVAILD);
				DRV_GPIO_ChangeMode(GPIO_ID_LED_REC_HIGH_BRIGHTNESS, GPIO_MODE_WRITE, GPIO_LOW);
			}
			break;

		case DI_LED_NUM3:
			if (ucDimLevel ==100) /* turn off */
			{
				DRV_GPIO_ChangeMode(GPIO_ID_LED_NETWORK_HIGH_BRIGHTNESS, GPIO_MODE_READ, GPIO_INVAILD);
				DRV_GPIO_ChangeMode(GPIO_ID_LED_NETWORK_LOW_BRIGHTNESS, GPIO_MODE_READ, GPIO_INVAILD);
			}
			else if (ucDimLevel > 50) /* Low Brightness */
			{
				DRV_GPIO_ChangeMode(GPIO_ID_LED_NETWORK_HIGH_BRIGHTNESS, GPIO_MODE_READ, GPIO_INVAILD);
				DRV_GPIO_ChangeMode(GPIO_ID_LED_NETWORK_LOW_BRIGHTNESS, GPIO_MODE_WRITE, GPIO_LOW);
			}
			else /* High Brightness */
			{
				DRV_GPIO_ChangeMode(GPIO_ID_LED_NETWORK_LOW_BRIGHTNESS, GPIO_MODE_READ, GPIO_INVAILD);
				DRV_GPIO_ChangeMode(GPIO_ID_LED_NETWORK_HIGH_BRIGHTNESS, GPIO_MODE_WRITE, GPIO_LOW);
			}
			break;

		default:
			PrintError("[%s] Warning! LED Id(0x%x) is not supported. Capability(0x%x)\n", __FUNCTION__, eLedID, LED_CAPABILITY);
			return DI_ERR_OK;
	}

	PrintExit();
#else
	BSTD_UNUSED(eLedID);
	BSTD_UNUSED(ucDimLevel);
#endif

	return DI_ERR_OK;
}

/********************************************************************
Function	: DRV_LED_Init
Description :
Input		: none
Output	: none
Return	: DRV_OK
*********************************************************************/
DRV_Error DRV_LED_Init(void)
{
	DRV_Error eDrvErr = DRV_OK;

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	DRV_PM_RegisterCallback(DRV_LED_Suspend, DRV_LED_Resume);
#endif
#if defined(CONFIG_LED_GPIO_PWM_AND_THREE_STATE)
	eDrvErr = P_LED_PwmInit();
	if(eDrvErr != DRV_OK)
	{
		PrintError("ERROR %s,%d\n", __FUNCTION__, __LINE__);
		return eDrvErr;
	}
#endif

	return eDrvErr;
}

