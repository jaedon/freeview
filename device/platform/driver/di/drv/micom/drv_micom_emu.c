/**
 * drv_micom.c
*/

/**
 * @defgroup		DRV_MICOM DRV_KPD : DRV_MICOM apis
 * @author			Kyu-young Cho
 * @note			DRV_MICOM APIs
 * @brief			Define DRV_MICOM APIs
 * @file 			drv_micom.c
*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
/* include  common utils headers */
#include "linden_trace.h"

#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include "bstd.h"

#include "vkernel.h"
#include "taskdef.h"
#include "di_uart.h"
#include "drv_key.h"
#include "drv_micom.h"
#include "drv_gpio.h"
#include "drv_stled.h"

#if defined(CONFIG_LED_GPIO_PWM_AND_THREE_STATE)
#include "nexus_pwm_init.h"
#include "nexus_pwm.h"
#endif


/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE TRACE_MODULE_DRV_MICOM

#if defined(CONFIG_STLED_MC2302)
#define MAX_SEGMENT_LENGTH	5
#else
#define MAX_SEGMENT_LENGTH	4
#endif
#if !defined(MICOM_MESSAGE_QUEUE_SIZE)
#define MICOM_MESSAGE_QUEUE_SIZE	(128)
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

typedef enum DRV_STLED_OPER_MODE
{
	DRV_STLED_OPER_MODE_ERROR,
	DRV_STLED_OPER_MODE_DISPLAY_TIME,
	DRV_STLED_OPER_MODE_TIME_OFF,
	DRV_STLED_OPER_MODE_WRITE_STRING
}STLED_OPER_MODE;

/*******************************************************************/
/************************ constant & gloval variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static unsigned long s_MessageId;
static unsigned long s_ulMicomNoneSema;

#if defined(CONFIG_STLED)
static HUINT8 s_aucWriteBuf[MAX_SEGMENT_LENGTH + 1];
#endif

static HINT32 s_timeOffset;

#define	GET_MICOM_SEMA 		VK_SEM_Get(s_ulMicomNoneSema)
#define	REL_MICOM_SEMA 		VK_SEM_Release(s_ulMicomNoneSema)

/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/
static void DRV_MICOM_NONE_Task(void *p);

/********************************************************************
 Function   : DRV_MICOM_Init
 Description :
 Input      : none
 Output     : none
 Return     : DRV_Error
 ********************************************************************/
DRV_Error DRV_MICOM_Init( void )
{
	unsigned long ulTaskID = 0;
	int nRetVal = 0;

 	nRetVal = VK_SEM_Create(&s_ulMicomNoneSema,"micom_rxtx", DEFAULT_SUSPEND_TYPE);
	if(nRetVal != VK_OK)
	{
		PrintError("[DRV_MICOM_Init] VK_SEM_Create Create Error\n");
		return DRV_ERR;
	}

	nRetVal = VK_MSG_Create(MICOM_MESSAGE_QUEUE_SIZE, sizeof(STLED_OPER_MODE), "Micom_Key_Msg", &s_MessageId, VK_SUSPENDTYPE_FIFO);
	if(nRetVal != VK_OK)
	{
		PrintError("[DRV_MICOM_Init] Micom_Key_Msg Create Error\n");
		return DRV_ERR;
	}

	nRetVal = VK_TASK_Create(DRV_MICOM_NONE_Task, MICOM_KEY_TASK_PRIORITY, MICOM_RX_TASK_STACK_SIZE, "MICOM_CheckTimeTask", (void *)0, &ulTaskID, FALSE);
	if(nRetVal != VK_OK)
	{
		PrintError("[DRV_MICOM_Init] DRV_MICOM_NONE_Task Create Error\n");
		return DRV_ERR;
	}

	VK_TASK_Start(ulTaskID);
 	return DRV_OK;
}

#if 0	/* compile warning 제거를 위해 막음. */
 /*******************************************************************
 Function   : DRV_MICOM_TX
 Description :
 Input      : ucTxNum, pDataBuffer
 Output     : none
 Return     : DRV_Error
 ********************************************************************/
static DRV_Error DRV_MICOM_TX( int ucTxNum, unsigned char *pDataBuffer )
{
	BSTD_UNUSED(ucTxNum);
	BSTD_UNUSED(pDataBuffer);

	return DRV_ERR;
}

/*******************************************************************
Function   : DRV_MICOM_RX
Description :
Input	   : ucTxNum, pDataBuffer
Output	   : none
Return	   : DRV_Error
********************************************************************/
static DRV_Error DRV_MICOM_RX(int ucRxNum, unsigned char *pDataBuffer)
{
	BSTD_UNUSED(ucRxNum);
	BSTD_UNUSED(pDataBuffer);

	return DRV_ERR;
}
#endif

/*******************************************************************
Function	  : DRV_MICOM_GetVersion
Description :
Input   : none
Output  : *ulVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_GetVersion( unsigned int *pulVersion )
{
	*pulVersion = 0;
	return DRV_OK;
}

/*******************************************************************
Function	  : DRV_MICOM_GetStrVersion
Description :
Input   : none
Output  : *pucStrVersion
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_GetStrVersion( unsigned char *pucStrVersion )
{
	BSTD_UNUSED(pucStrVersion);

	return DRV_OK;
}

/*******************************************************************
Function	  : DRV_MICOM_FrontLedControl
Description :
Input   : blue_red(blue:1, red:0)
Output  :
Return  : DRV_Error
********************************************************************/
DRV_Error DRV_MICOM_FrontLedControl( unsigned char blue_red)
{
	DRV_Error nErr = DRV_OK;

	HAPPY(blue_red);

#if defined(CONFIG_LED_GPIO)
#if defined(CONFIG_LED_GPIO_STBY_ONLY)
	if(blue_red)
	{
		nErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);
	}
	else
	{
		nErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);
	}
#elif defined(CONFIG_LED_GPIO_PWM_AND_THREE_STATE)
	BSTD_UNUSED(blue_red);

#elif defined(CONFIG_LED_GPIO_AND_FOURE_STATE)
	BSTD_UNUSED(blue_red);

#else
	if(blue_red)	/* BLUE */
	{
		nErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_HI);
		nErr = DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_LOW);
	}
	else	/* RED */
	{
		nErr = DRV_GPIO_Write(GPIO_ID_LED_STBY, GPIO_LOW);
		nErr = DRV_GPIO_Write(GPIO_ID_LED_POWER, GPIO_HI);
	}
#endif
#endif
	return nErr;
}

/********************************************************************
 Function   : DRV_MICOM_SetLocalTime
 Description :
 Input      : stDateTime
 Output     : none
 Return     : DRV_OK
********************************************************************/
DRV_Error DRV_MICOM_SetLocalTime(unsigned int unixTime)
{
   	unsigned long		currentUtcTime;
	unsigned long		vkError;

	PrintEnter();

	vkError = VK_CLOCK_GetTime(&currentUtcTime);
	if (vkError != VK_OK)
		return DRV_ERR;

	GET_MICOM_SEMA;
	s_timeOffset = unixTime - currentUtcTime;
 	PrintDebug("s_timeOffset = %d\n", s_timeOffset);
	REL_MICOM_SEMA;

	PrintExit();
	return DRV_OK;
 }

/********************************************************************
 Function   : DRV_MICOM_GetLocalTime
 Description :
 Input      : pstDateTime
 Output     : pstDateTime
 Return     : DRV_Error
 ********************************************************************/
DRV_Error  DRV_MICOM_GetLocalTime(unsigned int *punixTime)
{
	unsigned long vkError;
	unsigned long ulCurrentUtcTime;

	vkError = VK_CLOCK_GetTime(&ulCurrentUtcTime);
	if(vkError != VK_OK)
		return DRV_ERR;

	GET_MICOM_SEMA;
	ulCurrentUtcTime += s_timeOffset;
	REL_MICOM_SEMA;

	*punixTime = ulCurrentUtcTime;

	return DRV_OK;
}

/********************************************************************
 Function   : DRV_MICOM_SetWakeupTime
 Description :
 Input      : stDateTime
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_SetWakeupTime(unsigned int unixTime)
{
	BSTD_UNUSED(unixTime);

	return DRV_OK;
}
  /********************************************************************
 Function   : DRV_MICOM_RequestWakeupTime
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_RequestWakeupTime(unsigned int *punixTime , unsigned char *OnOff)
{
	BSTD_UNUSED(punixTime);
	BSTD_UNUSED(OnOff);

	return DRV_OK;
}

/********************************************************************
 Function   : DRV_MICOM_DelWakeupTime
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_DelWakeupTime( void )
{
	return DRV_OK;
}

 /********************************************************************
 Function   : DRV_MICOM_SetStandby
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_SetStandby( void )
{
	return DRV_OK;	/* exit loop (di_power.c) */
}

 /********************************************************************
 Function   : DRV_MICOM_STBPowerOn
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_STBPowerOn( void )
{
	return DRV_ERR;
}

/********************************************************************
Function   : DRV_MICOM_STBPowerReboot
Description :
Input	  : none
Output	  : none
Return	  : DRV_OK
********************************************************************/
DRV_Error DRV_MICOM_STBPowerReboot( void )
{
	return DRV_ERR;
}

 /********************************************************************
 Function   : DRV_MICOM_RequestPowerState
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_RequestPowerState(unsigned char *powerState)
{
	BSTD_UNUSED(powerState);

	return DRV_ERR;
}

 /********************************************************************
 Function   : DRV_MICOM_SetWatchdog
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_SetWatchdog(HBOOL bOn_nOff, unsigned char ucTimeOut)
{
	BSTD_UNUSED(bOn_nOff);
	BSTD_UNUSED(ucTimeOut);

	return DRV_OK;
}


/********************************************************************
 Function   : DRV_MICOM_DisplayTime
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_DisplayTime(unsigned char on_off)
{
#if defined(CONFIG_STLED)
	STLED_OPER_MODE eMessageType;

	if(on_off)
	{
		PrintError("DRV_MICOM_DisplayTime() : on_off = true\n");
		eMessageType = DRV_STLED_OPER_MODE_DISPLAY_TIME;
		VK_MSG_Send(s_MessageId, &eMessageType, sizeof(STLED_OPER_MODE));
	}
	else
	{
		PrintError("DRV_MICOM_DisplayTime() : on_off = false\n");
		eMessageType = DRV_STLED_OPER_MODE_TIME_OFF;
		VK_MSG_Send(s_MessageId, &eMessageType, sizeof(STLED_OPER_MODE));
	}
	return DRV_OK;
#endif
	HAPPY(on_off);

	return DRV_ERR;
}

/********************************************************************
 Function   : DRV_MICOM_GetWakeupSrc
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_GetWakeupSrc(DRV_MICOM_WAKEUP_SRC *peWakeupSrc)
{
	BSTD_UNUSED(peWakeupSrc);

	return DRV_OK;
}

#if defined(CONFIG_FRONT_MICOM_TOUCH)
/********************************************************************
 Function   : DRV_MICOM_GetWakeupSrc
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_GetTouchSensitive(unsigned char *pSensitiveData, unsigned char *pKeyNum)
{
	return DRV_OK;
}

DRV_Error DRV_MICOM_SetTouchThreshold(unsigned char ucThreshold)
{
	return DRV_ERR;
}
#endif

DRV_Error DRV_MICOM_EnableRescueReset(HBOOL bOn_nOff)
{
	BSTD_UNUSED(bOn_nOff);

	return DRV_ERR;
}

/********************************************************************
 Function   : DRV_MICOM_SET_Icon
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_SET_Icon(unsigned char position, unsigned char value)
{
	BSTD_UNUSED(position);
	BSTD_UNUSED(value);

	return DRV_ERR;
}

/********************************************************************
 Function   : DRV_MICOM_SET_Icon
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_SetLedControl(HUINT32 ulLedMask, HBOOL bOn_nOff)
{
	BSTD_UNUSED(ulLedMask);
	BSTD_UNUSED(bOn_nOff);

	return DRV_ERR;
}


/********************************************************************
 Function   : DRV_MICOM_WriteString
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error  DRV_MICOM_WriteString(unsigned char *string, unsigned char len)
{
#if defined(CONFIG_STLED)
	STLED_OPER_MODE eMessageType;

	GET_MICOM_SEMA;
	if(VK_strlen((char *)string) > MAX_SEGMENT_LENGTH)
	{
		VK_memcpy(s_aucWriteBuf, string, MAX_SEGMENT_LENGTH);
		s_aucWriteBuf[MAX_SEGMENT_LENGTH] = '\0';
	}
	else
	{
		VK_strncpy((char *)s_aucWriteBuf, (char *)string, MAX_SEGMENT_LENGTH);
		s_aucWriteBuf[MAX_SEGMENT_LENGTH] = '\0';
	}
	REL_MICOM_SEMA;

	eMessageType = DRV_STLED_OPER_MODE_WRITE_STRING;
	VK_MSG_Send(s_MessageId, &eMessageType, sizeof(STLED_OPER_MODE));
#endif

	HAPPY(string);
	BSTD_UNUSED(len);

	return DRV_OK;
}

DRV_Error DRV_MICOM_SetPowerSavingMode(HBOOL bOn_nOff)
{
	BSTD_UNUSED(bOn_nOff);

	return DRV_OK;	/* exit loop (di_power.c) */
}

DRV_Error DRV_MICOM_SetDisplayAni(HBOOL bOn_nOff)
{
	BSTD_UNUSED(bOn_nOff);

	return DRV_ERR;
}

DRV_Error DRV_MICOM_SetLCDPower(HBOOL bOn_nOff)
{
	BSTD_UNUSED(bOn_nOff);

	return DRV_OK;
}

DRV_Error DRV_MICOM_SetLcdDimmingLevel(unsigned char ucLcdDimmingLevel)
{
	BSTD_UNUSED(ucLcdDimmingLevel);

	return DRV_OK;
}

DRV_Error DRV_MICOM_SetLEDDimmingLevel(unsigned char ucLcdDimmingLevel)
{
	BSTD_UNUSED(ucLcdDimmingLevel);

	return DRV_OK;
}

DRV_Error DRV_MICOM_SetCustomCode(unsigned short usCCandCCB)	/*CustomCode and CustomCodeBar*/
{
	BSTD_UNUSED(usCCandCCB);

	return DRV_OK;
}
/********************************************************************
Function   : DRV_MICOM_SetUnitTest
Description : This APi is testing micom functions
Input      : none
Output     : none
Return	 : DDI_MICOM_OK

********************************************************************/
DRV_Error DRV_MICOM_SetUnitTest(unsigned char ucFuncNum, HBOOL bOn_nOff)
{
	HINT32 nResult = 0;

	HAPPY(bOn_nOff);
	BSTD_UNUSED(ucFuncNum);

#if defined(CONFIG_STLED)
	nResult = DRV_STLED_7SegmentTest(bOn_nOff);
	if(nResult != DRV_OK)
	{
		PrintError("DRV_STLED_7SegmentTest() Error\n");
	}
#endif
	return nResult;
}

/********************************************************************
 Function   : DRV_MICOM_SetWakeupRcuKey
 Description :
 Input      : none
 Output     : none
 Return     : DRV_OK
 ********************************************************************/
DRV_Error DRV_MICOM_SetWakeupRcuKey(DRV_MICOM_SET_WAKEUPKEY_TYPE nSetType, unsigned char *pucKey, unsigned char ucKeyNum)
{
	BSTD_UNUSED(nSetType);
	BSTD_UNUSED(pucKey);
	BSTD_UNUSED(ucKeyNum);

	return DRV_OK;
}

 /********************************************************************
Function   : DRV_MICOM_GetWakeupRcuKey
Description :
Input	   : none
Output	   : none
Return	   : DRV_OK
********************************************************************/
DRV_Error DRV_MICOM_GetWakeupRcuKey(unsigned char *pucRcuKeyCode)
{
	BSTD_UNUSED(pucRcuKeyCode);

	return DRV_OK;
}

DRV_Error DRV_MICOM_SetRCUInfo(HBOOL bOn_nOff)
{
	BSTD_UNUSED(bOn_nOff);

	return DRV_OK;

}

DRV_Error DRV_MICOM_SetFastBoot(HBOOL bOn_nOff)
{
	BSTD_UNUSED(bOn_nOff);

	return DRV_OK;

}




static void DRV_MICOM_NONE_Task(void *p)
{
 	DRV_Error nRetVal = 0;
	STLED_OPER_MODE eCurrentOperMode = DRV_STLED_OPER_MODE_ERROR;
#if defined(CONFIG_STLED)
	DRV_CLOCK_TIME_T tConvertTime;
	HUINT8 aucDisplayTimeData[MAX_SEGMENT_LENGTH + 1];
	unsigned long ulNowUnixTime = 0;
	unsigned long vkError = VK_OK;
#endif
	BSTD_UNUSED(p);

	while(1)
	{
		nRetVal = VK_MSG_ReceiveTimeout(s_MessageId, &eCurrentOperMode, sizeof(STLED_OPER_MODE), 500);
		if (nRetVal != VK_OK)
		{
			/* update the time periodically */
			if ( nRetVal != VK_TIMEOUT || eCurrentOperMode != DRV_STLED_OPER_MODE_DISPLAY_TIME )
			{
				continue;
			}
		}

#if defined(CONFIG_STLED)
		switch (eCurrentOperMode)
		{
		case DRV_STLED_OPER_MODE_DISPLAY_TIME :
			vkError = VK_CLOCK_GetTime(&ulNowUnixTime);
			if(vkError != VK_OK)
			{
				PrintError("Getting time failed, vkError = %d\n", vkError);
			}
			else
			{
				GET_MICOM_SEMA;
				ulNowUnixTime += s_timeOffset;
				REL_MICOM_SEMA;

				DRV_RTC_ConvertUnixTimeToTime(ulNowUnixTime, &tConvertTime);

				aucDisplayTimeData[0] = tConvertTime.ucHour / 10 + '0';
				aucDisplayTimeData[1] = tConvertTime.ucHour % 10 + '0';
				aucDisplayTimeData[2] = tConvertTime.ucMinute / 10 + '0';
				aucDisplayTimeData[3] = tConvertTime.ucMinute % 10 + '0';
#if defined(CONFIG_STLED_MC2302)
				aucDisplayTimeData[4] = ':';
				aucDisplayTimeData[5] = '\0';
#else
				aucDisplayTimeData[4] = '\0';
#endif

				DRV_STLED_WriteString(aucDisplayTimeData, MAX_SEGMENT_LENGTH, TRUE);
			}
			break;
		case DRV_STLED_OPER_MODE_TIME_OFF :
			VK_memset(aucDisplayTimeData, 0x00, MAX_SEGMENT_LENGTH + 1);
			DRV_STLED_WriteString(aucDisplayTimeData, MAX_SEGMENT_LENGTH, FALSE);
			break;
		case DRV_STLED_OPER_MODE_WRITE_STRING :
			GET_MICOM_SEMA;
			nRetVal = DRV_STLED_WriteString(s_aucWriteBuf, MAX_SEGMENT_LENGTH, FALSE);
			if(nRetVal != DRV_OK)
			{
				PrintError("DRV_STLED_WriteString() returns error.\n");
			}
			REL_MICOM_SEMA;
			break;
		default :
			break;
		}
#endif
	}
}

