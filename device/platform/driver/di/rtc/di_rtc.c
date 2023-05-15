/*
 * $Header:   $
 */
/*********************************************************************
 * $Workfile:   di_clock.c  $
 * $Modtime:   Jul 30 2007 01:07:04  $
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
#include "linden_trace.h"
#include "bstd_defs.h"

#include "di_rtc.h"
#include "drv_err.h"
#include "drv_micom.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE				TRACE_MODULE_DI_FAN

#define	DRV_CLOCK_UNIX_FIRST_DAY					2440588L
#define	DRV_CLOCK_DAYS_400YEARS						146097L		// 146097L;
#define	DRV_CLOCK_FUDGED_DAYS_4000YEARS				1461001L	// 1460970L + 31;
#define	DRV_CLOCK_SECONDS_PER_DAY					86400L

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
// Start typedef

// End typedef

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/
// Start global variablee


// End global variable

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
// Start static variablee

static pfnDI_RTC_Notify s_pfnRTCNotify ;
static HUINT32	s_WakeupTime = 0;
static HBOOL	s_bWakeupEnable = FALSE;
// End static variable

DI_ERR_CODE DRV_RTC_handler (HUINT8 timerStat) ;
DI_ERR_CODE DRV_RTC_SendNotify (DI_RTC_TIMER eTimer) ;

/*******************************************************************/
/*********************** Function Description***********************/
void DRV_RTC_MJD2YMD(unsigned short wMJD, DRV_CLOCK_DATE_T *pDate)
{
	 long YY, MM, K;

	 // attention !!!
	 YY 	 = ( wMJD * 100L - 1507820L  ) / 36525L;
	 MM 	 = (( wMJD * 10000L - 149561000L ) - ( YY * 36525L / 100L ) * 10000L ) / 306001L;

	 pDate->ucDay	 =(unsigned char)( wMJD - 14956L - ( YY * 36525L / 100L ) - ( MM * 306001L / 10000L ));

	 if ( MM == 14 || MM == 15 ) K = 1;
	 else						 K = 0;

	 pDate->usYear	 = (unsigned short) ( YY + K + 1900 );
	 pDate->ucMonth  = (unsigned char)( MM - 1 - K * 12 );
}

void DRV_RTC_YMD2MJD(DRV_CLOCK_DATE_T date, unsigned short *wMJD)
{
	long Y, M, D, L;
	long lTemp1, lTemp2;

	Y = (long)date.usYear -(long)1900;
	M = date.ucMonth;
	D = date.ucDay;

	if ( M == 1 || M == 2 )	L = 1;
	else					L = 0;

	lTemp1 = ( Y - L ) * 36525L / 100L;
	lTemp2 = ( M + 1L + L * 12L ) * 306001L / 10000L;

	*wMJD = (unsigned short)(14956 + D + lTemp1 + lTemp2);
}

DRV_Error DRV_RTC_ConvertDateTimeToUnixTime(DRV_DATE_TIME dateTime, unsigned long *unixtime)
{
	unsigned long		julian ;
	DRV_CLOCK_DATE_T	date;

	if ( !unixtime )
	{
		PrintError("unixtime is NULL~!\n");
		return DRV_ERR;
	}

	DRV_RTC_MJD2YMD(dateTime.usMjd, &date);
	julian = (date.ucDay - 32076) + 1461L * (date.usYear + 4800L + (date.ucMonth - 14) / 12) / 4 + 367 * (date.ucMonth - 2 - (date.ucMonth - 14) / 12 * 12) / 12 - 3 * ((date.usYear + 4900L + (date.ucMonth - 14) / 12) / 100) / 4 + 1 ;

	*unixtime = (unsigned long)julian ;

	*unixtime -= DRV_CLOCK_UNIX_FIRST_DAY ;
	*unixtime *= DRV_CLOCK_SECONDS_PER_DAY ;
	*unixtime += 3600L * dateTime.ucHour;
	*unixtime += 60L * dateTime.ucMin;
	*unixtime += dateTime.ucSec;

	return DRV_OK;
}


DRV_Error DRV_RTC_ConvertUnixTimeToDate(unsigned long	unixtime, DRV_CLOCK_DATE_T *pdate)
{
	unsigned long julian;
	unsigned long	x, z, m, d, y ;

	if ( !pdate )
	{
		PrintError("pdate is NULL~!\n");
		return DRV_ERR;
	}

	julian = (unixtime / DRV_CLOCK_SECONDS_PER_DAY) + DRV_CLOCK_UNIX_FIRST_DAY;

	x = julian + 68569L ;
	z = 4 * x / DRV_CLOCK_DAYS_400YEARS ;
	x = x - (DRV_CLOCK_DAYS_400YEARS * z + 3) / 4 ;
	y = 4000 * (x +  1) / DRV_CLOCK_FUDGED_DAYS_4000YEARS ;
	x = x - 1461 * y / 4 + 31 ;
	m = 80 * x / 2447;
	d = x - 2447 * m / 80 ;
	x = m / 11 ;
	m = m + 2 - 12 * x ;
	y = 100 * (z - 49) + y + x ;

	pdate->usYear = (unsigned short) y;
	pdate->ucMonth = (unsigned char) m;
	pdate->ucDay = (unsigned char) d;

	return DRV_OK;
}

DRV_Error DRV_RTC_ConvertUnixTimeToTime(unsigned long	unixtime, DRV_CLOCK_TIME_T *ptime)
{
	unsigned long hms;

	if ( !ptime )
	{
		PrintError("ptime is NULL~!\n");
		return DRV_ERR;
	}

	hms = unixtime % DRV_CLOCK_SECONDS_PER_DAY;

	ptime->ucHour = (unsigned char)(hms / 3600);	hms -= 3600L * ptime->ucHour;
	ptime->ucMinute = (unsigned char)(hms / 60);	hms -= 60 * ptime->ucMinute;
	ptime->ucSecond = (unsigned char)hms;
	ptime->usMillisecond = 0;

	return DRV_OK;
}


/*******************************************************************/
// Function Name: DI_RTC_RegisterRTCEventCallback
// Function Description: clockCallback 함수를 등록한다.
// Parameter: pfnNotify: 등록하고자 하는 함수의 포인터 값
// Return Value: none
/*******************************************************************/

void DI_RTC_RegisterRTCEventCallback (pfnDI_RTC_Notify pfnNotify)
{
	PrintEnter();
	s_pfnRTCNotify = pfnNotify ;
	PrintExit();
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name: DI_RTC_SetCurrentTime
// Function Description: 현재 시간을 Micom에 set하는 함수
// Parameter: unixTime - 현재시간
// Return Value: DI_ERR_CODE
/*******************************************************************/

DI_ERR_CODE DI_RTC_SetCurrentTime (HUINT32 unixTime)
{
	DRV_Error ret;

	PrintEnter();

	PrintError("\n Current Time(unixtime) :: %d(0x%x)\n\n", unixTime, unixTime);

	ret = DRV_MICOM_SetLocalTime(unixTime);
	if (ret != DRV_OK)
	{
		PrintError("Error In DRV_MICOM_SetLocalTime()\n");
		return DI_ERR_ERROR ;
	}

	PrintExit();

	return DI_ERR_OK ;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name: DI_RTC_SetOnTime
// Function Description: TV ON 타임을 설정하는 함수
// Parameter: unixTime - TV ON 시키고자 하는 시간
// Return Value: DI_ERR_CODE
/*******************************************************************/

DI_ERR_CODE DI_RTC_SetOnTime (HUINT32 unixTime)
{
	DRV_Error ret;

	PrintEnter();

	ret = DRV_MICOM_SetWakeupTime(unixTime);
	if (ret != DRV_OK)
	{
		PrintError("Error In DRV_MICOM_SetWakeupTime()\n");
		return DI_ERR_ERROR ;
	}

	/* Store wake up time */
	s_WakeupTime = unixTime;
	s_bWakeupEnable = TRUE;

	PrintDebug("[%s]Wakeup unixtime = %d(0x%x)\n\n", __func__, unixTime, unixTime);

	PrintExit();

	return DI_ERR_OK ;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name: DI_RTC_OnTimerOnOff
// Function Description: TV ON 타이머를 on/off 설정하는 함수
// Parameter: onOff - 타이머 on/off
// Return Value: DI_ERR_CODE
/*******************************************************************/

DI_ERR_CODE DI_RTC_OnTimerOnOff (HBOOL onOff)
{
	DRV_Error ret=DRV_OK;

	PrintEnter();

	if(onOff == TRUE)
	{
		if (s_bWakeupEnable == FALSE)	// 이미 저장되어 있다면, 다시 wakeup time set 할 필요없다.
		{
			ret = DRV_MICOM_SetWakeupTime(s_WakeupTime);
		}
	}
	else
	{
		ret = DRV_MICOM_DelWakeupTime();
	}

	if (ret != DRV_OK)
	{
		PrintError("Error In %s(), onOff(%d)\n", onOff?"DRV_MICOM_SetWakeupTime":"DRV_MICOM_DelWakeupTime", onOff);
		return DI_ERR_ERROR ;
	}

	PrintDebug("[%s]s_WakeupTime = %d(0x%x), onOff(%d), s_bWakeupEnable(%d)\n",
		__func__, s_WakeupTime, s_WakeupTime, onOff, s_bWakeupEnable);

	s_bWakeupEnable = onOff;

	PrintExit();

	return DI_ERR_OK ;
}


/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name: DI_RTC_GetCurrentTime
// Function Description: 현재 시간을 읽어 오는 함수
// Parameter: unixTime - 현재 시간을 읽어들이는 변수
// Return Value: DI_ERR_CODE
/*******************************************************************/

DI_ERR_CODE DI_RTC_GetCurrentTime (HUINT32 *unixTime)
{
	DRV_Error ret;

	ret = DRV_MICOM_GetLocalTime(unixTime);
	if (ret != DRV_OK)
	{
		PrintError("Error In DRV_MICOM_RequestWakeupTime()\n");
		return DI_ERR_ERROR ;
	}

	return DI_ERR_OK ;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name: DI_RTC_GetOnTime
// Function Description: ON 시간을 읽어 오는 함수
// Parameter: unixTime - ON 시간을 읽어들이는 변수
// Return Value: DI_ERR_CODE
/*******************************************************************/

DI_ERR_CODE DI_RTC_GetOnTime (HUINT32 *unixTime)
{
	DRV_Error ret;
	unsigned char	onoff;

	ret = DRV_MICOM_RequestWakeupTime(unixTime, &onoff);
	if (ret != DRV_OK)
	{
		PrintError("Error In DRV_MICOM_RequestWakeupTime()\n");
		return DI_ERR_ERROR ;
	}

	return DI_ERR_OK ;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name: DI_RTC_GetBootTime
// Function Description: micom으로부터 boot time을 읽어오는 함수.
// Parameter:
// Return Value: DI_ERR_CODE
/*******************************************************************/
DI_ERR_CODE DI_RTC_GetBootTime(HUINT16 *pusMJD, HUINT32 *pulBootTime)
{
#if	defined(CONFIG_FRONT_MICOM_JAPAN)
	DRV_Error         nDrvErr = DRV_OK;

	nDrvErr = DRV_MICOM_GetBootTime(pusMJD, pulBootTime);
	if (nDrvErr != DRV_OK)
	{
		 PrintError("Error(0x%x) In DRV_MICOM_GetVersion() \n", nDrvErr);
		 return   DI_ERR_ERROR;
	}

	PrintDebug("[%s] pusMJD: 0x%x pulBootTime : 0x%x \n", __func__, *pusMJD, *pulBootTime);
#else
	pusMJD = pusMJD;
	pulBootTime = pulBootTime;
#endif
	return DI_ERR_OK;
}


/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name: DRV_RTC_handler
// Function Description: OFF 시간을 읽어 오는 함수
// Parameter: timerStat - Timer event 정보를 갖고 있는 변수
// Return Value: DI_ERR_CODE
/*******************************************************************/

DI_ERR_CODE DRV_RTC_handler (HUINT8 timerStat)
{
	HUINT8 ret ;

	if (timerStat & 0x01)
	{
		ret = DRV_RTC_SendNotify (DI_RTC_ON_TIMER) ;
	}
	else if (timerStat & 0x02)
	{
		ret = DRV_RTC_SendNotify (DI_RTC_OFF_TIMER) ;
	}
	else
	{
		return DI_ERR_ERROR ;
	}

	if (ret != DI_ERR_OK)
	{
		return DI_ERR_ERROR ;
	}
	return DI_ERR_OK ;
}

/*******************************************************************/
/*********************** Function Description***********************/
/*******************************************************************/
// Function Name: DRV_RTC_SendNotify
// Function Description: clock callback 함수의 등록을 확인하고
//						 그 함수를 호출한다.
// Parameter: 	eTimer: Timer
// Return Value: DI_ERR_CODE
/*******************************************************************/

DI_ERR_CODE DRV_RTC_SendNotify (DI_RTC_TIMER eTimer)
{
	PrintDebug("[%s] Timer:%06lX\n", __FUNCTION__, eTimer);
	if( s_pfnRTCNotify != NULL)
	{
		s_pfnRTCNotify (eTimer) ;
		return DI_ERR_OK ;
	}
	return DI_ERR_ERROR ;
}


