/*******************************************************************
	File Description
********************************************************************/
/**
	@file	 	timeconv.h
	@brief

	Description: 	\n
	Module: 		\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MW_TCONV_H__
#define	__MW_TCONV_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <octo_common.h>
#include "ci_hwtest.h"
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define		DAYS_400YEARS					146097L		// 146097L;
#define		FUDGED_DAYS_4000YEARS			1461001L	// 1460970L + 31;
#define		UNIX_FIRST_DAY					2440588L
#define		HOURS_PER_DAY					24
#define		MINS_PER_HOUR					60
#define		SECONDS_PER_MIN					60
#define		SECONDS_PER_HOUR				(SECONDS_PER_MIN*MINS_PER_HOUR)
#define		SECONDS_PER_HALF				(SECONDS_PER_HOUR/2)
#define		SECONDS_PER_DAY					(SECONDS_PER_HOUR*HOURS_PER_DAY)
#define 		SECONDS_PER_WEEK				(SECONDS_PER_DAY * 7)


#define 	EVERY_WORKDAY					0xFFFFFFFF
#define		WEEKDAY_SUNDAY					0
#define		WEEKDAY_MONDAY					1
#define		WEEKDAY_TUESDAY					2
#define		WEEKDAY_WEDNESDAY				3
#define		WEEKDAY_THURSDAY				4
#define		WEEKDAY_FRIDAY					5
#define		WEEKDAY_SATURDAY				6
#define		TIME_DURATION_FOR_READY			60

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef	enum
{
	ERR_TCONV_OK 					= 0,
	ERR_TCONV_FAIL,

}TCONV_ERR;


typedef	struct tagDateType
{
	HUINT16	usYear;
	HUINT8	ucMonth;
	HUINT8	ucDay;
} DATE_T, SYS_DATE_T ;


typedef struct tagTimeType
{
	HUINT8	ucHour;
	HUINT8	ucMinute;
	HUINT8	ucSecond;
	HUINT16	usMillisecond;
} TIME_T, SYS_TIME_T ;


typedef	struct tagDateTime
{
	DATE_T	stDate;
	TIME_T	stTime;
} DATE_TIME_T;


/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

#if 1 //disable for HW_TEST
HBOOL 		UTIL_TCONV_IsTimeDurationOverNHours(UNIXTIME startTime, UNIXTIME endTime, HUINT8 nHours);
HBOOL 		UTIL_TCONV_IsTimeOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
HBOOL 		UTIL_TCONV_IsTimeOverwrappedWithin24Hours(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
HBOOL 		UTIL_TCONV_IsTimeOverwrappedWithinWeek(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
HBOOL 		UTIL_TCONV_IsWeekdayOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
HBOOL 		UTIL_TCONV_IsDateOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
HBOOL 		UTIL_TCONV_IsWorkDay(UNIXTIME startTime, UNIXTIME endTime);
HERROR		UTIL_TCONV_GetTimeOffsetWithDst (HINT32 *pnOffsetMin);
HERROR 		UTIL_TCONV_ConvertDateToJulian(DATE_T *date, HINT32 *julian);
HERROR 		UTIL_TCONV_ConvertJulianToDate(HINT32 julian, DATE_T *date);
HERROR 		UTIL_TCONV_ConvertMJDToYMD(HUINT16 mjd, DATE_T *date);
HERROR 		UTIL_TCONV_ConvertYMDToMJD(DATE_T *date, HUINT16 *mjd);
HERROR 		UTIL_TCONV_GetWeekDayFromMJD(HUINT16 mjd, HUINT16 *weekday);
HERROR 		UTIL_TCONV_GetWeekdayFromDate(DATE_T *date, HUINT16 *weekday);
HERROR 		UTIL_TCONV_ConvertMJDBcdTimeToUnixTime(HUINT8 *mjdBcdTime, UNIXTIME *unixtime);
HERROR 		UTIL_TCONV_ConvertUnixTimeToMJDBcdTime(UNIXTIME unixtime, HUINT8 *mjdUtc);
HERROR 		UTIL_TCONV_ConvertUnixTimeToMJD(UNIXTIME unixtime, HUINT16 *mjd);
HERROR 		UTIL_TCONV_ConvertMJDToDate(HUINT16 mjd, DATE_T *date);
HERROR 		UTIL_TCONV_ConvertDateToMJD(DATE_T *date, HUINT16 *mjd);
HERROR 		UTIL_TCONV_ConvertBcdTimeToDecimalTime(HUINT8 *bcdTime, HINT32 nLength, TIME_T *pTime);
HERROR 		UTIL_TCONV_ConvertDecimalTimeToBcdTime(TIME_T *time, HUINT8 *bcdTime);
HERROR 		UTIL_TCONV_ConvertBcdTimeToUnixTime (HUINT8 *bcdTime, UNIXTIME *unixtime);
HERROR 		UTIL_TCONV_ConvertUnixTimeToDateTime(UNIXTIME	unixtime, DATE_TIME_T *dateTime);
HERROR 		UTIL_TCONV_ConvertDateTimeToUnixTime(DATE_TIME_T *dateTime, UNIXTIME *unixtime);
HERROR 		UTIL_TCONV_ConvertUnixTimeToDate(UNIXTIME	unixtime, DATE_T *date);
HERROR 		UTIL_TCONV_ConvertUnixTimeToTime(UNIXTIME	unixtime, TIME_T *time);
HERROR		UTIL_TCONV_ConvertDurationToTime (UNIXTIME ulDuration, TIME_T *time);
HERROR		UTIL_TCONV_GetNextDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
HERROR		UTIL_TCONV_GetNextWeek(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
HERROR 		UTIL_TCONV_ConvertUnixTimeToWeekday(UNIXTIME	unixtime, HUINT8 *weekday);
HERROR		UTIL_TCONV_GetNextWorkDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
HERROR		UTIL_TCONV_GetNextWithoutSunday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
HERROR		UTIL_TCONV_GetNextWeekend(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
/*HERROR		UTIL_TCONV_GetTimeOffsetByCountryIndex (Country_Idx_t eCountry, HINT32 *pnHour, HUINT32 *pnMin);*/
HBOOL		UTIL_TCONV_LeapYear( HUINT16 year );
HUINT32		UTIL_TCONV_GetWeekDayIndex(DATE_T stDate);
HUINT32		UTIL_TCONV_GetWeekNofYear(DATE_T stDate);
HUINT32		UTIL_TCONV_GetWeekIndex(DATE_T stDate);
HUINT8		UTIL_TCONV_GetLastDayOfMonth(HUINT16 year, HUINT8 month);
#endif //disable for HW_TEST
#endif /* !__MW_TCONV_H__ */
