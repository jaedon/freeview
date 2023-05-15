/**
	@file     file_name.c File Name을 적습니다.
	@brief    file_name & simple comment.

	Description: File에 대한 설명을 적습니다.		\n
	Module: Layer와 모듈 명을 적습니다.			 	\n
	Remarks : 										\n

	<<<<<      주의 :      >>>>>

	시간 변환은 UNIXTIME 등의 System 의존 시간과 DATE, TIME 등의 UI 의존적 시간이 존재하는데
	시스템 내 UNIXTIME은 [반드시 GMT 0] 로써만 작동된다.
	GMT나 DST가 적용된 시간은 UNIXTIME 등을 오직 DATE, TIME 등의 UI 의존적 시간 오브젝트로 변환시킬 때
	자동적으로 적용하는 것을 원칙으로 한다.

			UNIXTIME	--->  offset + DST	---> DATE,TIME
			DATE,TIME	---> -offset - DST	---> UNIXTIME

	이 규칙은 OCTO 내에서는 절대적으로 지켜져야 하며, 오직 유일한 예외는 SI에서 사용하는
	BCD Time <-> UNIXTIME 만이 존재한다.

	그리고 timeconv.c 내부에서 local용 UNIXTIME <-> GMT0 UNIXTIME 변환하는 건 구현을 위한 부득이한 방책
	이며 해당되는 변환 함수가 global 등으로 재 매핑되어 밖에서 쓰이는 일은 절대로 없어야 한다.


	- 2008.11.12 Jinh

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
//#include <octo_common.h>
#include <hlib.h>
#include <htype.h>
#include <timeconv.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include <vkernel.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
#ifdef CONFIG_DEBUG
	HUINT32 g_Module_Level;
#endif


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HUINT32		 ulYear;
	UNIXTIME	 ulDstStartTime;		// Daylight Saving이 시작하는 시간 : GMT 0
	UNIXTIME	 ulDstEndTime;			// Daylight Saving이 끝나는 시간 : GMT 0
} DST_RANGE_t;



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static DST_RANGE_t			 s_stLastDstRange = { 0, 0, 0 };


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

//static HBOOL	local_tconv_IsDstAccepted (void);
static HBOOL	local_tconv_IsEuropeDstRange (UNIXTIME ulTime, HUINT32 ulYear);
//static HERROR	local_tconv_GetTimeOffsetWithDst (HINT32 *pnOffsetMin);
//static UNIXTIME local_tconv_AddOffsetTimeToUnixTime(UNIXTIME ulGmtTime, HINT32 nOffsetHour, HINT32 nOffsetMin);
static UNIXTIME	local_tconv_ConvGmtUnixTimeToLocalUnixTime (UNIXTIME ulGmtTime);
//static UNIXTIME	local_tconv_ConvLocalUnixTimeToGmtUnixTime (UNIXTIME ulLocalUnixTime);

//static HBOOL 	local_tconv_IsTimeDurationOverNHours(UNIXTIME startTime, UNIXTIME endTime, HUINT8 nHours);
//static HBOOL 	local_tconv_IsTimeOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
//static HBOOL 	local_tconv_IsTimeOverwrappedWithin24Hours(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
//static HBOOL	local_tconv_IsTimeOverwrappedWithinWeek(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
//static HBOOL 	local_tconv_IsWeekdayOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
//static HBOOL 	local_tconv_IsDateOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
//static HBOOL	local_tconv_IsWorkDay(UNIXTIME startTime, UNIXTIME endTime);
#if 0
static HBOOL	local_tconv_IsMondayToSaturday(UNIXTIME startTime, UNIXTIME endTime);
static HBOOL	local_tconv_IsTuesdayToSaturday(UNIXTIME startTime, UNIXTIME endTime);
#endif
static HERROR 	local_tconv_ConvertDateToJulian(DATE_T *date, HINT32 *julian);
#if 0
static HERROR 	local_tconv_ConvertJulianToDate(HINT32 julian, DATE_T *date);
static HERROR 	local_tconv_ConvertMJDToYMD(HUINT16 mjd, DATE_T *date);
static HERROR 	local_tconv_ConvertYMDToMJD(DATE_T *date, HUINT16 *mjd);
static HERROR 	local_tconv_GetWeekDayFromMJD(HUINT16 mjd, HUINT16 *weekday);
static HERROR 	local_tconv_GetWeekdayFromDate(DATE_T *date, HUINT16 *weekday);
static HERROR 	local_tconv_ConvertMJDBcdTimeToUnixTime(HUINT8 *mjdBcdTime, UNIXTIME *unixtime);
static HERROR 	local_tconv_ConvertUnixTimeToMJDBcdTime(UNIXTIME unixtime, HUINT8 *mjdUtc);
static HERROR 	local_tconv_ConvertUnixTimeToMJD(UNIXTIME unixtime, HUINT16 *mjd);
static HERROR 	local_tconv_ConvertMJDToDate(HUINT16 mjd, DATE_T *date);
static HERROR 	local_tconv_ConvertDateToMJD(DATE_T *date, HUINT16 *mjd);
#endif
static HERROR 	local_tconv_ConvertBcdTimeToDecimalTime(HUINT8 *bcdTime, HINT32 nLength, TIME_T *pTime);
#if 0
static HERROR 	local_tconv_ConvertDecimalTimeToBcdTime(TIME_T *time, HUINT8 *bcdTime);
static HERROR 	local_tconv_ConvertBcdTimeToUnixTime (HUINT8 *bcdTime, UNIXTIME *unixtime);
static HERROR 	local_tconv_ConvertUnixTimeToDateTime(UNIXTIME	unixtime, DATE_TIME_T *dateTime);
#endif
static HERROR 	local_tconv_ConvertDateTimeToUnixTime(DATE_TIME_T *dateTime, UNIXTIME *unixtime);
static HERROR 	local_tconv_ConvertUnixTimeToDate(UNIXTIME	unixtime, DATE_T *date);
static HERROR 	local_tconv_ConvertUnixTimeToTime(UNIXTIME	unixtime, TIME_T *time);
#if 0
static HERROR 	local_tconv_ConvertUnixTimeToWeekday(UNIXTIME	unixtime, HUINT8 *weekday);
static HERROR	local_tconv_GetNextDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
static HERROR	local_tconv_GetNextWeek(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
static HERROR	local_tconv_GetNextWorkDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
static HERROR	local_tconv_GetNextWithoutSunday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
static HERROR	local_tconv_GetNextTuesdayToSaturday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
static HERROR 	local_tconv_GetNextWeekend(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
static HBOOL	local_tconv_LeapYear(HUINT16 year);
static HUINT32	local_tconv_GetWeekDayIndex(DATE_T stDate);
static HUINT32	local_tconv_GetWeekNofYear(DATE_T stDate);
static HUINT32	local_tconv_GetWeekIndex(DATE_T stDate);
static HUINT8	local_tconv_GetLastDayOfMonth(HUINT16 year, HUINT8 month);
static HINT32	local_tconv_CompareDate(DATE_T stDate1, DATE_T stDate2);
#endif


#define _______UTIL_FUNCTION____________

unsigned long VK_CLOCK_GetOffsetTime (int *pnHour, int *pnMin)
{
	struct timeval tv;
	struct timezone		 stTimeZone;
	int					 nRet;

	if (pnHour == NULL || pnMin == NULL)
	{
		return ERR_FAIL;
	}

	nRet = gettimeofday(&tv, &stTimeZone);
	if (nRet == ERR_OK)
	{
		// Base unit : 15 min.
		*pnHour	= (stTimeZone.tz_minuteswest / 60);
		*pnMin	= ((stTimeZone.tz_minuteswest / 15) % 4) * 15;
		return ERR_OK;
	}

	return 15;
}



static VK_CLOCK_DST_MODE	s_eVk_Clock_DstMode = VK_CLOCK_DST_AUTO;

int VK_CLOCK_GetDstMode (VK_CLOCK_DST_MODE *peDstMode)
{
	if (peDstMode == NULL)		{ return ERR_FAIL; }

	*peDstMode = s_eVk_Clock_DstMode;
	return ERR_OK;
}


#define	______TCONV_GLOBAL_FUNC______

// ======================================================================
// Offset 적용이 필요 없는 함수들 :
// ======================================================================
#define _____NO_OFFSET_FUNC_____
#if 0
HBOOL UTIL_TCONV_IsTimeDurationOverNHours(UNIXTIME startTime, UNIXTIME endTime, HUINT8 nHours)
{

	return local_tconv_IsTimeDurationOverNHours(startTime, endTime, nHours) ;
}

HBOOL UTIL_TCONV_IsTimeOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	return local_tconv_IsTimeOverwrapped(startTime1, endTime1, startTime2, endTime2) ;
}

HERROR UTIL_TCONV_ConvertDateToJulian (DATE_T *date, HINT32 *julian)
{
	return local_tconv_ConvertDateToJulian(date, julian) ;
}

HERROR UTIL_TCONV_ConvertJulianToDate(HINT32 julian, DATE_T *date)
{
	return local_tconv_ConvertJulianToDate(julian, date) ;
}

HERROR UTIL_TCONV_ConvertMJDToYMD(HUINT16 mjd, DATE_T *date)
{
	return local_tconv_ConvertMJDToYMD(mjd, date) ;
}

HERROR UTIL_TCONV_ConvertYMDToMJD(DATE_T *date, HUINT16 *mjd)
{
	return local_tconv_ConvertYMDToMJD(date, mjd) ;
}

// MJDBCD Time은 SI에서 GMT 0로 오는 시그널링이기 때문에 Offset을 적용시키면 안 된다.
HERROR UTIL_TCONV_ConvertMJDBcdTimeToUnixTime(HUINT8 *mjdBcdTime, UNIXTIME *unixtime)
{
	return local_tconv_ConvertMJDBcdTimeToUnixTime(mjdBcdTime, unixtime);
}

HERROR UTIL_TCONV_ConvertUnixTimeToMJDBcdTime(UNIXTIME unixtime, HUINT8 *mjdUtc)
{
	return local_tconv_ConvertUnixTimeToMJDBcdTime(unixtime, mjdUtc);
}

HERROR UTIL_TCONV_GetWeekDayFromMJD(HUINT16 mjd, HUINT16 *weekday)
{
	return local_tconv_GetWeekDayFromMJD(mjd, weekday) ;
}

HERROR UTIL_TCONV_GetWeekdayFromDate(DATE_T *date, HUINT16 *weekday)
{
	return local_tconv_GetWeekdayFromDate(date, weekday);
}

HERROR UTIL_TCONV_ConvertBcdTimeToUnixTime (HUINT8 *bcdTime, UNIXTIME *unixtime)
{
	return local_tconv_ConvertBcdTimeToUnixTime (bcdTime, unixtime);
}

HERROR UTIL_TCONV_ConvertMJDToDate(HUINT16 mjd, DATE_T *date)
{
	return local_tconv_ConvertMJDToDate(mjd, date) ;
}

HERROR UTIL_TCONV_ConvertDateToMJD(DATE_T *date, HUINT16 *mjd)
{
	return local_tconv_ConvertDateToMJD(date, mjd) ;
}
#endif
//herer
HERROR UTIL_TCONV_ConvertBcdTimeToDecimalTime(HUINT8 *bcdTime, HINT32 nLength, TIME_T *pTime)
{
	return local_tconv_ConvertBcdTimeToDecimalTime(bcdTime, nLength, pTime);
}
#if 0
HERROR UTIL_TCONV_ConvertDecimalTimeToBcdTime(TIME_T *time, HUINT8 *bcdTime)
{
	return local_tconv_ConvertDecimalTimeToBcdTime(time, bcdTime) ;
}

HERROR UTIL_TCONV_ConvertUnixTimeToMJD (UNIXTIME unixtime, HUINT16 *mjd)
{
	return local_tconv_ConvertUnixTimeToMJD (unixtime, mjd);
}

// UNIXTIME이 Offset 재계산 없이 곧바로 Time 등으로 변환되어서는 안된다.
// 단 Duration(간격 시간)을 얻기 위한 경우에는 예외이므로 이 경우에는 따로 만들어 준다.
HERROR UTIL_TCONV_ConvertDurationToTime (UNIXTIME ulDuration, TIME_T *time)
{
	return local_tconv_ConvertUnixTimeToTime (ulDuration, time) ;
}


HERROR UTIL_TCONV_ConvertUnixTimeToDateTimeWithOutOffset (UNIXTIME unixtime, DATE_TIME_T *dateTime)
{
	HERROR		 hErr;

	hErr = local_tconv_ConvertUnixTimeToDateTime (unixtime, dateTime) ;
	if (hErr != ERR_TCONV_OK)
	{
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HERROR UTIL_TCONV_ConvertDateTimeToUnixTimeWithOutOffset (DATE_TIME_T *dateTime, UNIXTIME *unixtime)
{
	HERROR		 hErr;

	if (unixtime == NULL)				{ return ERR_FAIL; }

	hErr = local_tconv_ConvertDateTimeToUnixTime (dateTime, unixtime) ;
	if (hErr != ERR_TCONV_OK)
	{
		return	ERR_FAIL;
	}

	return	ERR_OK;
}


HBOOL		UTIL_TCONV_LeapYear( HUINT16 year )
{
	return local_tconv_LeapYear(year );
}

HUINT32		UTIL_TCONV_GetWeekDayIndex(DATE_T stDate)
{
	return local_tconv_GetWeekDayIndex(stDate);
}

HUINT32		UTIL_TCONV_GetWeekNofYear(DATE_T stDate)
{
	return local_tconv_GetWeekNofYear(stDate);
}

HUINT32		UTIL_TCONV_GetWeekIndex(DATE_T stDate)
{
	return local_tconv_GetWeekIndex(stDate);
}

HUINT8		UTIL_TCONV_GetLastDayOfMonth(HUINT16 year, HUINT8 month)
{
	return local_tconv_GetLastDayOfMonth(year, month);
}

HINT32		UTIL_TCONV_CompareDate(DATE_T stDate1, DATE_T stDate2)
{
	return local_tconv_CompareDate(stDate1, stDate2);
}

#endif

// ======================================================================
// Offset 적용이 필요한 함수 모음.
// ======================================================================

#define _____OFFSET_REQUIRED_FUNC_____

// Date 제외한 시간 단위로 Overlap이 되는지 확인하는 함수.
// Offset 적용이 필요할 수 있으므로 일단 분류해 둔다.
#if 0
HERROR UTIL_TCONV_GetTimeOffsetWithDst (HINT32 *pnOffsetMin)
{
	return local_tconv_GetTimeOffsetWithDst (pnOffsetMin);
}
#endif
#if 0

HBOOL UTIL_TCONV_IsTimeOverwrappedWithin24Hours(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	UNIXTIME	 ulLocalStartTime1, ulLocalEndTime1;
	UNIXTIME	 ulLocalStartTime2, ulLocalEndTime2;

	ulLocalStartTime1	= local_tconv_ConvGmtUnixTimeToLocalUnixTime (startTime1);
	ulLocalEndTime1		= local_tconv_ConvGmtUnixTimeToLocalUnixTime (endTime1);
	ulLocalStartTime2	= local_tconv_ConvGmtUnixTimeToLocalUnixTime (startTime2);
	ulLocalEndTime2		= local_tconv_ConvGmtUnixTimeToLocalUnixTime (endTime2);

	return local_tconv_IsTimeOverwrappedWithin24Hours (ulLocalStartTime1, ulLocalEndTime1, ulLocalStartTime2, ulLocalEndTime2);
}


HBOOL UTIL_TCONV_IsTimeOverwrappedWithinWeek(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	UNIXTIME	 ulLocalStartTime1, ulLocalEndTime1;
	UNIXTIME	 ulLocalStartTime2, ulLocalEndTime2;

	ulLocalStartTime1	= local_tconv_ConvGmtUnixTimeToLocalUnixTime (startTime1);
	ulLocalEndTime1		= local_tconv_ConvGmtUnixTimeToLocalUnixTime (endTime1);
	ulLocalStartTime2	= local_tconv_ConvGmtUnixTimeToLocalUnixTime (startTime2);
	ulLocalEndTime2		= local_tconv_ConvGmtUnixTimeToLocalUnixTime (endTime2);

	return local_tconv_IsTimeOverwrappedWithinWeek(ulLocalStartTime1, ulLocalEndTime1, ulLocalStartTime2, ulLocalEndTime2);
}


HBOOL UTIL_TCONV_IsWeekdayOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	UNIXTIME	 ulLocalStartTime1, ulLocalEndTime1;
	UNIXTIME	 ulLocalStartTime2, ulLocalEndTime2;

	ulLocalStartTime1	= local_tconv_ConvGmtUnixTimeToLocalUnixTime (startTime1);
	ulLocalEndTime1		= local_tconv_ConvGmtUnixTimeToLocalUnixTime (endTime1);
	ulLocalStartTime2	= local_tconv_ConvGmtUnixTimeToLocalUnixTime (startTime2);
	ulLocalEndTime2		= local_tconv_ConvGmtUnixTimeToLocalUnixTime (endTime2);

	return local_tconv_IsWeekdayOverwrapped (ulLocalStartTime1, ulLocalEndTime1, ulLocalStartTime2, ulLocalEndTime2);
}

HBOOL UTIL_TCONV_IsDateOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	UNIXTIME	 ulLocalStartTime1, ulLocalEndTime1;
	UNIXTIME	 ulLocalStartTime2, ulLocalEndTime2;

	ulLocalStartTime1	= local_tconv_ConvGmtUnixTimeToLocalUnixTime (startTime1);
	ulLocalEndTime1		= local_tconv_ConvGmtUnixTimeToLocalUnixTime (endTime1);
	ulLocalStartTime2	= local_tconv_ConvGmtUnixTimeToLocalUnixTime (startTime2);
	ulLocalEndTime2		= local_tconv_ConvGmtUnixTimeToLocalUnixTime (endTime2);

	return local_tconv_IsDateOverwrapped (ulLocalStartTime1, ulLocalEndTime1, ulLocalStartTime2, ulLocalEndTime2);
}


HBOOL UTIL_TCONV_IsWorkDay(UNIXTIME startTime, UNIXTIME endTime)
{
	return local_tconv_IsWorkDay (startTime, endTime);
}

HBOOL UTIL_TCONV_IsMondayToSaturday(UNIXTIME startTime, UNIXTIME endTime)
{
	return local_tconv_IsMondayToSaturday (startTime, endTime);
}

HBOOL UTIL_TCONV_IsTuesdayToSaturday(UNIXTIME startTime, UNIXTIME endTime)
{
	return local_tconv_IsTuesdayToSaturday (startTime, endTime);
}

HERROR UTIL_TCONV_ConvertLocalUnixTmeToUnixTime(UNIXTIME ulLocalUnixtime, UNIXTIME *pulUnixTime)
{
	if(pulUnixTime == NULL)
	{
		return ERR_FAIL;
	}

	*pulUnixTime = local_tconv_ConvLocalUnixTimeToGmtUnixTime (ulLocalUnixtime);

	return ERR_OK;
}

HERROR UTIL_TCONV_ConvertUnixTmeToLocalUnixTime(UNIXTIME unixtime, UNIXTIME *pulLocalTime)
{
	if(pulLocalTime == NULL)
	{
		return ERR_FAIL;
	}

	*pulLocalTime = local_tconv_ConvGmtUnixTimeToLocalUnixTime (unixtime);

	return ERR_OK;
}

HERROR UTIL_TCONV_ConvertUnixTimeToDateTime (UNIXTIME unixtime, DATE_TIME_T *dateTime)
{
	UNIXTIME	 ulLocalTime;

	ulLocalTime = local_tconv_ConvGmtUnixTimeToLocalUnixTime (unixtime);

	return local_tconv_ConvertUnixTimeToDateTime (ulLocalTime, dateTime) ;
}

HERROR UTIL_TCONV_ConvertUnixTimeToDateTimeWithOffset(UNIXTIME unixtime, HINT32 nOffsetHour, HINT32 nOffsetMin, DATE_TIME_T *dateTime)
{
	UNIXTIME	 ulLocalTime;

	ulLocalTime = local_tconv_AddOffsetTimeToUnixTime (unixtime, nOffsetHour, nOffsetMin);

	return local_tconv_ConvertUnixTimeToDateTime (ulLocalTime, dateTime) ;
}
#endif
#if 0
HERROR UTIL_TCONV_ConvertDateTimeToUnixTime (DATE_TIME_T *dateTime, UNIXTIME *unixtime)
{
	UNIXTIME	 ulLocalTime;
	HERROR		 hErr;

	if (unixtime == NULL)				{ return ERR_FAIL; }

	hErr = local_tconv_ConvertDateTimeToUnixTime (dateTime, &ulLocalTime) ;
	if (hErr == ERR_OK)
	{
		*unixtime = local_tconv_ConvLocalUnixTimeToGmtUnixTime (ulLocalTime);
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR UTIL_TCONV_ConvertUnixTimeToDate(UNIXTIME	unixtime, DATE_T *date)
{
	UNIXTIME	 ulLocalTime;

	ulLocalTime = local_tconv_ConvGmtUnixTimeToLocalUnixTime (unixtime);

	return local_tconv_ConvertUnixTimeToDate (ulLocalTime, date);
}
#endif
//hrer
HERROR UTIL_TCONV_ConvertUnixTimeToTime(UNIXTIME	unixtime, TIME_T *time)
{
	UNIXTIME	 ulLocalTime;

	ulLocalTime = local_tconv_ConvGmtUnixTimeToLocalUnixTime (unixtime);

	return local_tconv_ConvertUnixTimeToTime (ulLocalTime, time) ;
}
#if 0
HERROR UTIL_TCONV_ConvertUnixTimeToWeekday(UNIXTIME	unixtime, HUINT8 *weekday)
{
	UNIXTIME	 ulLocalTime;

	ulLocalTime = local_tconv_ConvGmtUnixTimeToLocalUnixTime (unixtime);

	return local_tconv_ConvertUnixTimeToWeekday (ulLocalTime, weekday) ;
}

HERROR	UTIL_TCONV_GetNextDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalNextTime;
	HERROR		 hErr;

	if (nextTime == NULL || timeGap == NULL)
	{
		return ERR_TCONV_FAIL;
	}

	ulLocalBaseTime = local_tconv_ConvGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = local_tconv_GetNextDay (ulLocalBaseTime, &ulLocalNextTime, timeGap);
	if (hErr == ERR_OK)
	{
		*nextTime = local_tconv_ConvLocalUnixTimeToGmtUnixTime (ulLocalNextTime);
		*timeGap = *nextTime - baseTime;
		return ERR_OK;
	}

	return ERR_TCONV_FAIL;
}

HERROR	UTIL_TCONV_GetNextWeek(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalNextTime;
	HERROR		 hErr;

	if (nextTime == NULL || timeGap == NULL)
	{
		return ERR_TCONV_FAIL;
	}

	ulLocalBaseTime = local_tconv_ConvGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = local_tconv_GetNextWeek (ulLocalBaseTime, &ulLocalNextTime, timeGap);
	if (hErr == ERR_OK)
	{
		*nextTime = local_tconv_ConvLocalUnixTimeToGmtUnixTime (ulLocalNextTime);
		*timeGap = *nextTime - baseTime;
		return ERR_OK;
	}

	return ERR_TCONV_FAIL;
}

HERROR	UTIL_TCONV_GetNextWorkDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalNextTime;
	HERROR		 hErr;

	if (nextTime == NULL || timeGap == NULL)
	{
		return ERR_TCONV_FAIL;
	}

	ulLocalBaseTime = local_tconv_ConvGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = local_tconv_GetNextWorkDay (ulLocalBaseTime, &ulLocalNextTime, timeGap);
	if (hErr == ERR_OK)
	{
		*nextTime = local_tconv_ConvLocalUnixTimeToGmtUnixTime (ulLocalNextTime);
		*timeGap = *nextTime - baseTime;
		return ERR_OK;
	}

	return ERR_TCONV_FAIL;
}

HERROR	UTIL_TCONV_GetNextWithoutSunday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalNextTime;
	HERROR		 hErr;

	if (nextTime == NULL || timeGap == NULL)
	{
		return ERR_TCONV_FAIL;
	}

	ulLocalBaseTime = local_tconv_ConvGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = local_tconv_GetNextWithoutSunday (ulLocalBaseTime, &ulLocalNextTime, timeGap);
	if (hErr == ERR_OK)
	{
		*nextTime = local_tconv_ConvLocalUnixTimeToGmtUnixTime (ulLocalNextTime);
		*timeGap = *nextTime - baseTime;
		return ERR_OK;
	}

	return ERR_TCONV_FAIL;
}

HERROR	UTIL_TCONV_GetNextTuesdayToSaturday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalNextTime;
	HERROR		 hErr;

	if (nextTime == NULL || timeGap == NULL)
	{
		return ERR_TCONV_FAIL;
	}

	ulLocalBaseTime = local_tconv_ConvGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = local_tconv_GetNextTuesdayToSaturday(ulLocalBaseTime, &ulLocalNextTime, timeGap);
	if (hErr == ERR_OK)
	{
		*nextTime = local_tconv_ConvLocalUnixTimeToGmtUnixTime (ulLocalNextTime);
		*timeGap = *nextTime - baseTime;
		return ERR_OK;
	}

	return ERR_TCONV_FAIL;
}


HERROR	UTIL_TCONV_GetNextWeekend (UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime;
	UNIXTIME	 ulLocalNextTime;
	HERROR		 hErr;

	if (nextTime == NULL || timeGap == NULL)
	{
		return ERR_TCONV_FAIL;
	}

	ulLocalBaseTime = local_tconv_ConvGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = local_tconv_GetNextWeekend (ulLocalBaseTime, &ulLocalNextTime, timeGap);
	if (hErr == ERR_OK)
	{
		*nextTime = local_tconv_ConvLocalUnixTimeToGmtUnixTime (ulLocalNextTime);
		*timeGap = *nextTime - baseTime;
		return ERR_OK;
	}

	return ERR_TCONV_FAIL;
}
#endif
#if 0
HERROR UTIL_TCONV_GetTimeOffsetByCountryIndex (Country_Idx_t eCountry, HINT32 *pnHour, HUINT32 *pnMin)
{
	if (pnHour == NULL || pnMin == NULL)
	{
		return ERR_FAIL;
	}

	switch (eCountry)
	{
	// UTC+0:0
	case eCountry_ENGLAND:
	case eCountry_IRELAND:
	case eCountry_SPAIN:
	case eCountry_PORTUGAL:
		*pnHour = 0;
		*pnMin = 0;
		break;

	// UTC+1:0
	case eCountry_AUSTRIA:
	case eCountry_CAMEROON:
	case eCountry_CROATIA:
	case eCountry_CZECH:
	case eCountry_DENMARK:
	case eCountry_FRANCE:
	case eCountry_GERMANY:
	case eCountry_ITALY:
	case eCountry_HUNGARY:
	case eCountry_NETHERLANDS:
	case eCountry_NORWAY:
	case eCountry_POLAND:
	case eCountry_SLOVAKIA:
	case eCountry_SLOVENIA:
	case eCountry_SWEDEN:
	case eCountry_SWITZERLAND:
	case eCountry_BELGIUM:
	case eCountry_LUXEMBOURG:
		*pnHour = 1;
		*pnMin = 0;
		break;

	// UTC+2:0
	case eCountry_ESTONIA:
	case eCountry_FINLAND:
	case eCountry_GREECE:
	case eCountry_LATVIA:
	case eCountry_LITHUANIA:
	case eCountry_TURKEY:
	case eCountry_UKRAINE:
	case eCountry_ROMANIA:
		*pnHour = 2;
		*pnMin = 0;
		break;

	// UTC+3:0
	case eCountry_RUSSIA:
		*pnHour = 3;
		*pnMin = 0;
		break;

	// UTC+7:0
	case eCountry_VIETNAM:
		*pnHour = 7;
		*pnMin = 0;
		break;

	// UTC+8:0
	case eCountry_SINGAPORE:
	case eCountry_AUSTRALIA:
		*pnHour = 8;
		*pnMin = 0;
		break;

	// UTC+9:0
	case eCountry_JAPAN:
	case eCountry_KOREA:
		*pnHour = 9;
		*pnMin = 0;
		break;

	// 미국은 UTC-5 ~ UTC-8 까지 존재하므로 후에 다른 방법으로 구현해야 한다.
	case eCountry_USA:
		*pnHour = -5;
		*pnMin = 0;
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}
#endif
#define	______TCONV_LOCAL_FUNC______
#if 0 //Not Used...
static HBOOL local_tconv_IsDstAccepted (void)
{
	return TRUE;
}
#endif

//here
static HBOOL local_tconv_IsEuropeDstRange (UNIXTIME ulTime, HUINT32 ulYear)
{
	HINT32			 nDate;
	DATE_TIME_T		 stDstStart, stDstEnd;
	UNIXTIME		 ulStartTime, ulEndTime;
	HERROR			 hErr;

	// DST 계산이 잦을 수 있으니 쓸데없는 계산량을 줄이기 위해 특정 년도일 경우에는 한 번만 계산하고
	// 그 이후로는 그 결과값만을 이용하도록 한다.

	// Daylight Saving 계산 Algorithm 출처
	// http://en.wikipedia.org/wiki/European_Summer_Time
	// http://www.webexhibits.org/daylightsaving/i.html

	if (s_stLastDstRange.ulYear != ulYear)
	{
		// Begin DST: Sunday March (31 - (5*y/4 + 4) mod 7) at 1h U.T.
		nDate = 31 - (((5 * (HINT32)ulYear) / 4 + 4) % 7);
		stDstStart.stDate.usYear	= (HUINT16)ulYear;
		stDstStart.stDate.ucMonth	= 3;			// March
		stDstStart.stDate.ucDay 	= (HUINT8)nDate;
		stDstStart.stTime.ucHour	= 1;
		stDstStart.stTime.ucMinute	= 0;
		stDstStart.stTime.ucSecond	= 0;
		stDstStart.stTime.usMillisecond 	= 0;

		// End DST: Sunday October (31 - (5*y/4 + 1) mod 7) at 1h U.T.
		nDate = 31 - (((5 * (HINT32)ulYear) / 4 + 1) % 7);
		stDstEnd.stDate.usYear		= (HUINT16)ulYear;
		stDstEnd.stDate.ucMonth 	= 10;			// October
		stDstEnd.stDate.ucDay		= (HUINT8)nDate;
		stDstEnd.stTime.ucHour		= 1;
		stDstEnd.stTime.ucMinute	= 0;
		stDstEnd.stTime.ucSecond	= 0;
		stDstEnd.stTime.usMillisecond		= 0;

		hErr  = local_tconv_ConvertDateTimeToUnixTime (&stDstStart, &ulStartTime);
		hErr |= local_tconv_ConvertDateTimeToUnixTime (&stDstEnd, &ulEndTime);
		if (hErr != ERR_OK)
		{
			return FALSE;
		}

		s_stLastDstRange.ulYear			= ulYear;
		s_stLastDstRange.ulDstStartTime	= ulStartTime;
		s_stLastDstRange.ulDstEndTime	= ulEndTime;
	}

	if (ulTime >= s_stLastDstRange.ulDstStartTime && ulTime < s_stLastDstRange.ulDstEndTime)
	{
		return TRUE;
	}

	return FALSE;
}
#if 0
static HERROR local_tconv_GetTimeOffsetWithDst (HINT32 *pnOffsetMin)
{
	int					 nHour, nMin;
	UNIXTIME			 ulGmtTime;
	DATE_T				 stDate;
	VK_CLOCK_DST_MODE	 eDstMode;
	HERROR				 hErr;

	if (pnOffsetMin == NULL)
	{
		return ERR_FAIL;
	}

	// 1. Get the offset
	hErr = VK_CLOCK_GetOffsetTime (&nHour, &nMin);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	hErr = VK_CLOCK_GetDstMode (&eDstMode);
	if (hErr != VK_OK)					{ eDstMode = VK_CLOCK_DST_AUTO; }

	if (eDstMode == VK_CLOCK_DST_AUTO)
	{
		hErr = VK_CLOCK_GetTime ((HULONG *)&ulGmtTime);
		if (hErr == ERR_OK)
		{
			hErr = local_tconv_ConvertUnixTimeToDate (ulGmtTime, &stDate);
			if (hErr == ERR_OK)
			{
				if (local_tconv_IsEuropeDstRange ((UNIXTIME)ulGmtTime, (HUINT32)stDate.usYear) == TRUE)
				{
					// DST 적용시 1시간 더한다.
					nHour ++;
				}
			}
		}

	}
	else if (eDstMode == VK_CLOCK_DST_ON_ALWAYS)
	{
		// DST 적용시 1시간 더한다.
		nHour ++;
	}

	*pnOffsetMin = (nHour * 60) + nMin;
	return ERR_OK;
}
#endif

#if 0
static UNIXTIME local_tconv_AddOffsetTimeToUnixTime(UNIXTIME ulGmtTime, HINT32 nOffsetHour, HINT32 nOffsetMin)
{
	int					 nLocalUnixTime;
	DATE_T				 stDate;
	VK_CLOCK_DST_MODE	 eDstMode;
	HERROR				 hErr;

	// 1. Add the offset :
	nLocalUnixTime = (int)ulGmtTime + nOffsetHour * SECONDS_PER_HOUR + nOffsetMin * SECONDS_PER_MIN;

	// 2. Get the year from the offset : to check
	hErr = VK_CLOCK_GetDstMode (&eDstMode);
	if (hErr != VK_OK)					{ eDstMode = VK_CLOCK_DST_AUTO; }

	if (eDstMode == VK_CLOCK_DST_AUTO)
	{
		hErr = local_tconv_ConvertUnixTimeToDate (ulGmtTime, &stDate);
		if (hErr == ERR_OK)
		{
			if (local_tconv_IsEuropeDstRange ((UNIXTIME)ulGmtTime, (HUINT32)stDate.usYear) == TRUE)
			{
				// DST 적용시 1시간 더한다.
				nLocalUnixTime += SECONDS_PER_HOUR;
			}
		}
	}
	else if (eDstMode == VK_CLOCK_DST_ON_ALWAYS)
	{
		// DST 적용시 1시간 더한다.
		nLocalUnixTime += SECONDS_PER_HOUR;
	}

	return (UNIXTIME)nLocalUnixTime;
}
#endif

//herer
static UNIXTIME local_tconv_ConvGmtUnixTimeToLocalUnixTime (UNIXTIME ulGmtTime)
{
	int					 nHour, nMin;
	int					 nLocalUnixTime;
	DATE_T				 stDate;
	VK_CLOCK_DST_MODE	 eDstMode;
	HERROR				 hErr;

	// 1. Get the offset
	hErr = HLIB_DATETIME_GetOffsetTime (&nHour, &nMin);
	if (hErr != ERR_OK)
	{
		return ulGmtTime;
	}

	// 2. Add the offset :
	nLocalUnixTime = (int)ulGmtTime + nHour * SECONDS_PER_HOUR + nMin * SECONDS_PER_MIN;

	// 3. Get the year from the offset : to check
	hErr = VK_CLOCK_GetDstMode (&eDstMode);
	if (hErr != ERR_OK)					{ eDstMode = VK_CLOCK_DST_AUTO; }

	if (eDstMode == VK_CLOCK_DST_AUTO)
	{
		hErr = local_tconv_ConvertUnixTimeToDate (ulGmtTime, &stDate);
		if (hErr == ERR_OK)
		{
			if (local_tconv_IsEuropeDstRange ((UNIXTIME)ulGmtTime, (HUINT32)stDate.usYear) == TRUE)
			{
				// DST 적용시 1시간 더한다.
				nLocalUnixTime += SECONDS_PER_HOUR;
			}
		}
	}
	else if (eDstMode == VK_CLOCK_DST_ON_ALWAYS)
	{
		// DST 적용시 1시간 더한다.
		nLocalUnixTime += SECONDS_PER_HOUR;
	}

	return (UNIXTIME)nLocalUnixTime;
}
#if 0
static UNIXTIME local_tconv_ConvLocalUnixTimeToGmtUnixTime (UNIXTIME ulLocalUnixTime)
{
	int					 nHour, nMin;
	int					 nGmtTime;
	DATE_T				 stDate;
	VK_CLOCK_DST_MODE	 eDstMode;
	HERROR				 hErr;

	// 1. Get the offset
	hErr = VK_CLOCK_GetOffsetTime (&nHour, &nMin);
	if (hErr != ERR_OK)
	{
		return ulLocalUnixTime;
	}

	// 2. Delete the offset
	nGmtTime = (int)ulLocalUnixTime - nHour * SECONDS_PER_HOUR - nMin * SECONDS_PER_MIN;

	// 3. Get the year from the offset : to check
	hErr = VK_CLOCK_GetDstMode (&eDstMode);
	if (hErr != VK_OK)					{ eDstMode = VK_CLOCK_DST_AUTO; }

	if (eDstMode == VK_CLOCK_DST_AUTO)
	{
		hErr = local_tconv_ConvertUnixTimeToDate ((UNIXTIME)nGmtTime, &stDate);
		if (hErr == ERR_OK)
		{
			if (local_tconv_IsEuropeDstRange ((UNIXTIME)nGmtTime - SECONDS_PER_HOUR, (HUINT32)stDate.usYear) == TRUE)
			{
				// DST 적용된 경우에는 한 시간 만큼 더 줄인다.
				// DST가 해체되는 날의 local time은 2:00 ~ 3:00 이 DST 적용, 비적용 두 경우로 나뉘게 된다.
				// (GMT 0의 0:00 ~ 1:00 과 1:00 ~ 2:00이 동일한 local time 2:00 ~ 3:00대)
				// 여기서는 앞의 0:00 ~ 1:00이 local time 2:00 ~ 3:00으로 간주한다.
				nGmtTime -= SECONDS_PER_HOUR;
			}
		}
	}
	else if (eDstMode == VK_CLOCK_DST_ON_ALWAYS)
	{
		// DST 언제나 적용되므로 한 시간 만큼 더 줄인다.
		nGmtTime -= SECONDS_PER_HOUR;
	}

	return (UNIXTIME)nGmtTime;
}

static HBOOL local_tconv_IsTimeDurationOverNHours(UNIXTIME startTime, UNIXTIME endTime, HUINT8 nHours)
{
	UNIXTIME	nHoursSec ;

	nHoursSec = ((UNIXTIME)nHours * SECONDS_PER_HOUR) ;

	if ( (endTime - startTime) > nHoursSec )
	{
		return TRUE ;
	}

	return FALSE ;
}

static HBOOL local_tconv_IsTimeOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	if ( endTime2 <= startTime1 || endTime1 <= startTime2 )
	{
		return FALSE ;
	}
	return TRUE ;
}

static HBOOL local_tconv_IsTimeOverwrappedWithin24Hours(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	UNIXTIME	subtractor ;
	HBOOL		leadingOne ;

	subtractor = (startTime1 <= startTime2 ? startTime1 : startTime2) ;
	leadingOne = (startTime1 <= startTime2 ? TRUE : FALSE) ;

	startTime1 -= subtractor ;
	endTime1 -= subtractor ;
	startTime2 -= subtractor ;
	endTime2 -= subtractor ;

	startTime1 %= SECONDS_PER_DAY ;
	endTime1 %= SECONDS_PER_DAY ;
	startTime2 %= SECONDS_PER_DAY ;
	endTime2 %= SECONDS_PER_DAY ;

	if ( leadingOne )
	{
		if ( (startTime2 <= endTime2) && (endTime1 <= startTime2) )
		{
			return FALSE ;
		}
	}
	else
	{
		if ( (startTime1 <= endTime1) && (endTime2 <= startTime1) )
		{
			return FALSE ;
		}
	}

	return TRUE ;
}

static HBOOL local_tconv_IsTimeOverwrappedWithinWeek(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	UNIXTIME	ulGap=0, ulSubtractor=0;

	if( startTime1 >= startTime2 )
	{
		ulGap =  startTime1 - startTime2;
		if( ulGap >= SECONDS_PER_WEEK )
		{
			ulSubtractor = (ulGap/SECONDS_PER_WEEK) * SECONDS_PER_WEEK;
			startTime1 -= ulSubtractor;
			endTime1 -= ulSubtractor;
		}
	}
	else
	{
		ulGap =  startTime2 - startTime1;
		if( ulGap >= SECONDS_PER_WEEK )
		{
			ulSubtractor = (ulGap/SECONDS_PER_WEEK) * SECONDS_PER_WEEK;
			startTime2 -= ulSubtractor;
			endTime2 -= ulSubtractor;
		}
	}

	if ( endTime2 <= startTime1 || endTime1 <= startTime2 )
		return FALSE ;

	return TRUE ;
}

static HBOOL local_tconv_IsWeekdayOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	HUINT8	weekday11, weekday12, weekday21, weekday22 ;

	local_tconv_ConvertUnixTimeToWeekday(startTime1, &weekday11) ;
	local_tconv_ConvertUnixTimeToWeekday(endTime1, &weekday12) ;
	local_tconv_ConvertUnixTimeToWeekday(startTime2, &weekday21) ;
	local_tconv_ConvertUnixTimeToWeekday(endTime2, &weekday22) ;

	if ( weekday11 > weekday22 || weekday12 < weekday21 )
	{
		return FALSE ;
	}
	return TRUE ;
}

static HBOOL local_tconv_IsDateOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	startTime1 /= SECONDS_PER_DAY ;
	endTime1 /= SECONDS_PER_DAY ;
	startTime2 /= SECONDS_PER_DAY ;
	endTime2 /= SECONDS_PER_DAY ;

	if ( startTime1 > endTime2 || startTime2 > endTime1 )
	{
		return FALSE ;
	}
	return TRUE ;
}


static HBOOL local_tconv_IsWorkDay(UNIXTIME startTime, UNIXTIME endTime)
{
	HUINT8	weekday1, weekday2;

	UTIL_TCONV_ConvertUnixTimeToWeekday(startTime, &weekday1);
	UTIL_TCONV_ConvertUnixTimeToWeekday(endTime, &weekday2);

	if(weekday2 < weekday1) { weekday2 += 7; }
	if(weekday2 - weekday1 >= 2)
	{
		return TRUE;
	}
	weekday2 %= 7;

	if( (weekday1 >= WEEKDAY_MONDAY && weekday1 <= WEEKDAY_FRIDAY)
		|| (weekday2 >= WEEKDAY_MONDAY && weekday2 <= WEEKDAY_FRIDAY) )
	{
		return TRUE;
	}

	return FALSE;
}

static HBOOL local_tconv_IsMondayToSaturday(UNIXTIME startTime, UNIXTIME endTime)
{
	HUINT8	weekday1, weekday2;

	UTIL_TCONV_ConvertUnixTimeToWeekday(startTime, &weekday1);
	UTIL_TCONV_ConvertUnixTimeToWeekday(endTime, &weekday2);

	if(weekday2 < weekday1) { weekday2 += 7; }
	if(weekday2 - weekday1 >= 1)
	{
		return TRUE;
	}
	weekday2 %= 7;

	if( (weekday1 >= WEEKDAY_MONDAY && weekday1 <= WEEKDAY_SATURDAY)
		|| (weekday2 >= WEEKDAY_MONDAY && weekday2 <= WEEKDAY_SATURDAY) )
	{
		return TRUE;
	}

	return FALSE;
}

static HBOOL local_tconv_IsTuesdayToSaturday(UNIXTIME startTime, UNIXTIME endTime)
{
	HUINT8	weekday1, weekday2;

	UTIL_TCONV_ConvertUnixTimeToWeekday(startTime, &weekday1);
	UTIL_TCONV_ConvertUnixTimeToWeekday(endTime, &weekday2);

	if( (weekday1 >= WEEKDAY_TUESDAY && weekday1 <= WEEKDAY_SATURDAY)
		|| (weekday2 >= WEEKDAY_TUESDAY && weekday2 <= WEEKDAY_SATURDAY) )
	{
		return TRUE;
	}

	return FALSE;
}
#endif


static HERROR local_tconv_ConvertDateToJulian(DATE_T *date, HINT32 *julian)
{
	if ( !julian )
	{
		return ERR_TCONV_FAIL ;
	}

	*julian = (date->ucDay - 32076) + 1461L * (date->usYear + 4800L + (date->ucMonth - 14) / 12) / 4 + 367 * (date->ucMonth - 2 - (date->ucMonth - 14) / 12 * 12) / 12 - 3 * ((date->usYear + 4900L + (date->ucMonth - 14) / 12) / 100) / 4 + 1 ;

	return ERR_TCONV_OK ;
}

//herer
static HERROR local_tconv_ConvertJulianToDate(HINT32 julian, DATE_T *date)
{
	HINT32	x, z, m, d, y ;

	if ( !date )
	{
		return ERR_TCONV_FAIL ;
	}

	x = julian + 68569L ;
	z = 4 * x / DAYS_400YEARS ;
	x = x - (DAYS_400YEARS * z + 3) / 4 ;
	y = 4000 * (x +  1) / FUDGED_DAYS_4000YEARS ;
	x = x - 1461 * y / 4 + 31 ;
	m = 80 * x / 2447;
	d = x - 2447 * m / 80 ;
	x = m / 11 ;
	m = m + 2 - 12 * x ;
	y = 100 * (z - 49) + y + x ;

	date->usYear = (HUINT16) y;
	date->ucMonth = (HUINT8) m;
	date->ucDay = (HUINT8) d;

	return ERR_TCONV_OK ;
}
#if 0
static HERROR local_tconv_ConvertMJDToYMD(HUINT16 mjd, DATE_T *date)
{
	HUINT32	YY, MM, K ;

	if ( !date )
	{
		return ERR_TCONV_FAIL ;
	}

	YY = (mjd * 100L - 1507820L) / 36525L ;
	MM = ((mjd * 10000L - 149561000L) - (YY * 36525L / 100L) * 10000L) / 306001L ;
	K = ((MM == 14) || (MM == 15) ? 1 : 0 ) ;

	date->usYear = (HUINT16)(YY + K + 1900) ;
	date->ucMonth = (HUINT8)(MM - 1 - K * 12) ;
	date->ucDay = (HUINT8)(mjd - 14956L - (YY * 36525L / 100L) - (MM * 306001L / 10000L)) ;

	return ERR_TCONV_OK ;
}

static HERROR local_tconv_ConvertYMDToMJD(DATE_T *date, HUINT16 *mjd)
{
	HUINT32	Y, M, D, L ;
	HUINT32	tmp1, tmp2 ;

	if ( !mjd )
	{
		return ERR_TCONV_FAIL ;
	}

	Y = (HUINT32)date->usYear - (HUINT32)1900 ;
	M = (HUINT32)date->ucMonth ;
	D = (HUINT32)date->ucDay ;

	L = ((M == 1) || (M == 2) ? 1 : 0) ;

	tmp1 = (Y - L) * 36525L / 100L ;
	tmp2 = (M + 1 + L * 12L) * 306001L / 10000L ;

	*mjd = (HUINT16)(14956 + D + tmp1 + tmp2) ;

	return ERR_TCONV_OK ;
}

static HERROR local_tconv_GetWeekDayFromMJD(HUINT16 mjd, HUINT16 *weekday)
{
	if ( !weekday )
	{
		return ERR_TCONV_FAIL ;
	}

	*weekday = (((mjd + 2) % 7) + 1) ;

	return ERR_TCONV_OK ;
}

static HERROR local_tconv_GetWeekdayFromDate(DATE_T *date, HUINT16 *weekday)
{
	HUINT16	mjd ;

	if ( !weekday )
	{
		return ERR_TCONV_FAIL ;
	}

	local_tconv_ConvertYMDToMJD(date, &mjd) ;
	return local_tconv_GetWeekDayFromMJD(mjd, weekday) ;
}




static HERROR local_tconv_ConvertMJDBcdTimeToUnixTime(HUINT8 *mjdBcdTime, UNIXTIME *unixtime)
{
	HERROR		result  = ERR_TCONV_FAIL;
	HUINT16		mjd ;
	HUINT8*		bcdtime = NULL ;
	DATE_TIME_T	dateTime;

	if ( !mjdBcdTime )
	{
		return ERR_TCONV_FAIL ;
	}

	if ( !unixtime )
	{
		return ERR_TCONV_FAIL ;
	}

	mjd = (HUINT16)((mjdBcdTime[0] << 8) + mjdBcdTime[1]) ;
	result = local_tconv_ConvertMJDToDate(mjd, &(dateTime.stDate)) ;
	if (result == ERR_TCONV_OK)
	{
		// 변환 후 UNIXTIME의 범위를 넘을 값을 미리 배제한다.
		if (dateTime.stDate.usYear >= 2038 || dateTime.stDate.usYear <= 1980)
		{
			return ERR_TCONV_FAIL;
		}

		bcdtime = &(mjdBcdTime[2]) ;
		result = local_tconv_ConvertBcdTimeToDecimalTime(bcdtime, 3, &(dateTime.stTime)) ;
		if (result == ERR_TCONV_OK)
		{
			return local_tconv_ConvertDateTimeToUnixTime(&dateTime, unixtime) ;
		}
	}

	return result;
}

static HERROR local_tconv_ConvertUnixTimeToMJDBcdTime(UNIXTIME unixtime, HUINT8 *mjdUtc)
{
	HERROR		result = ERR_TCONV_FAIL;
	DATE_TIME_T	dateTime;
	HUINT16		mjd;
	HUINT8		bcdTime[3];

	if ( !mjdUtc )
	{
		return ERR_TCONV_FAIL ;
	}

	local_tconv_ConvertUnixTimeToDateTime(unixtime, &dateTime);
	result = local_tconv_ConvertDateToMJD(&(dateTime.stDate), &mjd);
	if (result == ERR_TCONV_OK)
	{
		result = local_tconv_ConvertDecimalTimeToBcdTime(&(dateTime.stTime), bcdTime);
		if (result == ERR_TCONV_OK)
		{
			mjdUtc[0] = (HUINT8)((mjd >> 8) & 0x00ff);
			mjdUtc[1] = (HUINT8)(mjd & 0x00ff);
			mjdUtc[2] = bcdTime[0];
			mjdUtc[3] = bcdTime[1];
			mjdUtc[4] = bcdTime[2];
		}
	}

	return result;
}

static HERROR local_tconv_ConvertUnixTimeToMJD(UNIXTIME unixtime, HUINT16 *mjd)
{
	DATE_T		date ;

	if ( !mjd )
	{
		return ERR_TCONV_FAIL ;
	}

	local_tconv_ConvertUnixTimeToDate(unixtime, &date);
	return local_tconv_ConvertDateToMJD(&date, mjd);
}

static HERROR local_tconv_ConvertMJDToDate(HUINT16 mjd, DATE_T *date)
{
	HINT32	YY, MM, K;

	if ( !date )
	{
		return ERR_TCONV_FAIL ;
	}

	// 소수점 계산 때문 ==> 모든 계산은 10배 또는 10000배 후에 계산
	YY	= (mjd*100L-1507820L) / 36525L;
	MM	= ((mjd*10000L-149561000L) - (YY*36525L/100L)*10000L)/306001L;

	date->ucDay = (HUINT8)(mjd-14956L-(YY*36525L/100L)-(MM*306001L/10000L));

	K = ( (MM == 14) || (MM == 15) ? 1 : 0 ) ;

	date->usYear = (HUINT16)(YY + K + 1900);
	date->ucMonth = (HUINT8)(MM - 1 - K*12);

	return ERR_TCONV_OK ;
}

static HERROR local_tconv_ConvertDateToMJD(DATE_T *date, HUINT16 *mjd)
{
	HUINT32	Y, M, D, L;
	HUINT32	tmp1, tmp2;

	if ( !date )
	{
		return ERR_TCONV_FAIL ;
	}

	if ( !mjd )
	{
		return ERR_TCONV_FAIL ;
	}

	Y = (HUINT32)(date->usYear - 1900);
	M = (HUINT32)date->ucMonth;
	D = (HUINT32)date->ucDay;

	L = ( (M == 1) || (M == 2) ? 1 : 0 ) ;

	tmp1 = ( Y - L ) * 36525L / 100L;
	tmp2 = ( M + 1L + L * 12L ) * 306001L / 10000L;

	*mjd = (HUINT16)(14956 + D + tmp1 + tmp2);

	return ERR_TCONV_OK ;
}
#endif
//hrer
static HERROR local_tconv_ConvertBcdTimeToDecimalTime(HUINT8 *bcdTime, HINT32 nLength, TIME_T *pTime)
{
	HUINT8	dataU8;
	HUINT8	returnValue;
	HINT32		nIndex;

	if ( !bcdTime )
	{
		return ERR_TCONV_FAIL ;
	}

	if ( !pTime )
	{
		return ERR_TCONV_FAIL ;
	}
	HxSTD_MemSet (pTime, 0x00, sizeof (TIME_T));

	nIndex = 0;
	if (nLength <= nIndex)
	{
		return ERR_TCONV_FAIL;
	}
	dataU8 = bcdTime[nIndex];
	returnValue = (HUINT8)(((dataU8>>4) & 0x0f) * 10 + (dataU8 & 0x0f));
	if (returnValue <= 24)
	{
		pTime->ucHour = returnValue;

		nIndex++;
		if (nLength <= nIndex)
		{
			return ERR_TCONV_OK;
		}

		dataU8 = bcdTime[nIndex];
		returnValue = (HUINT8)(((dataU8>>4) & 0x0f) * 10 + (dataU8 & 0x0f));
		if (returnValue <= 60)
		{
			pTime->ucMinute = returnValue;

			nIndex++;
			if (nLength <= nIndex)
			{
				return ERR_TCONV_OK;
			}

			dataU8 = bcdTime[nIndex];
			returnValue = (HUINT8)(((dataU8>>4) & 0x0f) * 10 + (dataU8 & 0x0f));
			if (returnValue <= 60)
			{
				pTime->ucSecond = returnValue;
				pTime->usMillisecond = 0;
			}
		}
	}

	return ERR_TCONV_OK;
}

#if 0
static HERROR local_tconv_ConvertDecimalTimeToBcdTime(TIME_T *time, HUINT8 *bcdTime)
{
	if ( !time )
	{
		return ERR_TCONV_FAIL ;
	}

	if ( !bcdTime )
	{
		return ERR_TCONV_FAIL ;
	}

	bcdTime[0] = (HUINT8)((((time->ucHour / 10) << 4) & 0xf0) | (time->ucHour % 10));
	bcdTime[1] = (HUINT8)((((time->ucMinute / 10) << 4) & 0xf0) | (time->ucMinute % 10));
	bcdTime[2] = (HUINT8)((((time->ucSecond / 10) << 4) & 0xf0) | (time->ucSecond % 10));

	return ERR_TCONV_OK ;
}


static HERROR local_tconv_ConvertBcdTimeToUnixTime (HUINT8 *bcdTime, UNIXTIME *unixtime)
{
	UNIXTIME	sysTime ;
	HERROR		result = ERR_TCONV_FAIL;
	HUINT8		dataU8;
	HUINT8		returnValue;

	if ( !bcdTime )
	{
		return ERR_TCONV_FAIL ;
	}

	if ( !unixtime )
	{
		return ERR_TCONV_FAIL ;
	}

	*unixtime = 0;

	dataU8 = bcdTime[0];
	returnValue = (HUINT8)(((dataU8>>4) & 0x0f) * 10 + (dataU8 & 0x0f));

	sysTime = returnValue * SECONDS_PER_HOUR;

	dataU8 = bcdTime[1];
	returnValue = (HUINT8)(((dataU8>>4) & 0x0f) * 10 + (dataU8 & 0x0f));

	if (returnValue <= 60)
	{
		sysTime += returnValue * 60;

		dataU8 = bcdTime[2];
		returnValue = (HUINT8)(((dataU8>>4) & 0x0f) * 10 + (dataU8 & 0x0f));
		if (returnValue <= 60)
		{
			result = ERR_TCONV_OK;
			sysTime += returnValue;
		}
	}

	if ( result == ERR_TCONV_OK )
	{
		*unixtime = sysTime ;
	}

	return result;
}

static HERROR local_tconv_ConvertUnixTimeToDateTime(UNIXTIME	unixtime, DATE_TIME_T *dateTime)
{
	long jd;
	long hms;

	if ( !dateTime )
	{
		return ERR_TCONV_FAIL ;
	}

	jd = unixtime / SECONDS_PER_DAY;
	hms = unixtime % SECONDS_PER_DAY;

	local_tconv_ConvertJulianToDate(jd + UNIX_FIRST_DAY, &(dateTime->stDate)) ;

	dateTime->stTime.ucHour = (HUINT8)(hms / SECONDS_PER_HOUR);
	hms -= SECONDS_PER_HOUR * dateTime->stTime.ucHour;

	dateTime->stTime.ucMinute = (HUINT8)(hms / SECONDS_PER_MIN);
	hms -= SECONDS_PER_MIN * dateTime->stTime.ucMinute;

	dateTime->stTime.ucSecond = (HUINT8)hms;
	dateTime->stTime.usMillisecond = 0;

	return ERR_TCONV_OK ;
}
#endif
//here
static HERROR local_tconv_ConvertDateTimeToUnixTime(DATE_TIME_T *dateTime, UNIXTIME *unixtime)
{
	HERROR		result  = ERR_TCONV_FAIL;
	HINT32		julian ;

	if ( !dateTime )
	{
		return ERR_TCONV_FAIL ;
	}

	if ( !unixtime )
	{
		return ERR_TCONV_FAIL ;
	}

	result = local_tconv_ConvertDateToJulian(&(dateTime->stDate), &julian) ;
	*unixtime = (UNIXTIME)julian ;

	if (result != ERR_TCONV_OK && ((HINT32)(*unixtime - UNIX_FIRST_DAY)) < 0)
	{
		*unixtime = 0L;
	}
	else
	{
		*unixtime -= UNIX_FIRST_DAY ;
		*unixtime *= SECONDS_PER_DAY ;
		*unixtime += SECONDS_PER_HOUR * dateTime->stTime.ucHour;
		*unixtime += SECONDS_PER_MIN * dateTime->stTime.ucMinute;
		*unixtime += dateTime->stTime.ucSecond;
	}

	return ERR_TCONV_OK ;
}

//herer
static HERROR local_tconv_ConvertUnixTimeToDate(UNIXTIME	unixtime, DATE_T *date)
{
	if ( !date )
	{
		return ERR_TCONV_FAIL ;
	}

	return local_tconv_ConvertJulianToDate((unixtime / SECONDS_PER_DAY) + UNIX_FIRST_DAY, date) ;
}

//herer
static HERROR local_tconv_ConvertUnixTimeToTime(UNIXTIME	unixtime, TIME_T *time)
{
	HUINT32	hms;

	if ( !time )
	{
		return ERR_TCONV_FAIL ;
	}

	hms = unixtime % SECONDS_PER_DAY;

	time->ucHour = (HUINT8)(hms / SECONDS_PER_HOUR);	hms -= SECONDS_PER_HOUR * time->ucHour;
	time->ucMinute = (HUINT8)(hms / SECONDS_PER_MIN);	hms -= SECONDS_PER_MIN * time->ucMinute;
	time->ucSecond = (HUINT8)hms;
	time->usMillisecond = 0;

	return ERR_TCONV_OK ;
}
#if 0
static HERROR local_tconv_ConvertUnixTimeToWeekday(UNIXTIME	unixtime, HUINT8 *weekday)
{
	if ( !weekday )
	{
		return ERR_TCONV_FAIL ;
	}

	unixtime /= SECONDS_PER_DAY ;

	/* 1970.1.1은 목요일로 +4일을 더해서 일요일이 0이 되는 값이 나오게 한다. */
	*weekday = (HUINT8)((unixtime + 4) % 7) ;

	return ERR_TCONV_OK ;
}

static HERROR	local_tconv_GetNextDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if(!nextTime || !timeGap)
	{
		return ERR_TCONV_FAIL;
	}

	local_tconv_ConvertUnixTimeToWeekday(baseTime, &weekday);

	switch (weekday)
	{
	/* 일요일부터 목요일까지는 +1일을 해준다. */
	case WEEKDAY_SUNDAY :
	case WEEKDAY_MONDAY :
	case WEEKDAY_TUESDAY :
	case WEEKDAY_WEDNESDAY :
	case WEEKDAY_THURSDAY :
	case WEEKDAY_FRIDAY :
	case WEEKDAY_SATURDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY;
		break;

	default :
		HxLOG_Error("\n");
		return ERR_TCONV_FAIL;
	}

	*nextTime = baseTime + *timeGap;

	return ERR_TCONV_OK;
}

static HERROR	local_tconv_GetNextWeek(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if(!nextTime || !timeGap)
	{
		return ERR_TCONV_FAIL;
	}

	local_tconv_ConvertUnixTimeToWeekday(baseTime, &weekday);

	switch (weekday)
	{
	/* 일요일부터 목요일까지는 +7일을 해준다. */
	case WEEKDAY_SUNDAY :
	case WEEKDAY_MONDAY :
	case WEEKDAY_TUESDAY :
	case WEEKDAY_WEDNESDAY :
	case WEEKDAY_THURSDAY :
	case WEEKDAY_FRIDAY :
	case WEEKDAY_SATURDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY*7;
		break;

	default :
		HxLOG_Error("\n");
		return ERR_TCONV_FAIL;
	}

	*nextTime = baseTime + *timeGap;

	return ERR_TCONV_OK;
}

static HERROR	local_tconv_GetNextWorkDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if(!nextTime || !timeGap)
	{
		return ERR_TCONV_FAIL;
	}

	local_tconv_ConvertUnixTimeToWeekday(baseTime, &weekday);

	switch (weekday)
	{
	/* 일요일부터 목요일까지는 +1일을 해준다. */
	case WEEKDAY_SUNDAY :
	case WEEKDAY_MONDAY :
	case WEEKDAY_TUESDAY :
	case WEEKDAY_WEDNESDAY :
	case WEEKDAY_THURSDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY;
		break;

	/* 금요일은 +3일(월요일)*/
	case WEEKDAY_FRIDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY * 3;
		break;

	/* 토요일은 +2일(월요일)*/
	case WEEKDAY_SATURDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY * 2;
		break;

	default :
		HxLOG_Error("\n");
		return ERR_TCONV_FAIL;
	}

	*nextTime = baseTime + *timeGap;

	return ERR_TCONV_OK;
}

static HERROR local_tconv_GetNextWithoutSunday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if (!nextTime || !timeGap)
	{
		return ERR_TCONV_FAIL;
	}

	local_tconv_ConvertUnixTimeToWeekday(baseTime, &weekday);

	switch (weekday)
	{
	/* 일 ~ 금요일은 +1일을 한다. */
	case WEEKDAY_SUNDAY :
	case WEEKDAY_MONDAY :
	case WEEKDAY_TUESDAY :
	case WEEKDAY_WEDNESDAY :
	case WEEKDAY_THURSDAY :
	case WEEKDAY_FRIDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY;
		break;

	/* 토요일은 +2일을 하여 월요일을 만들어 준다. */
	case WEEKDAY_SATURDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY * 2;
		break;

	default :
		HxLOG_Error("\n");
		return ERR_TCONV_FAIL;
	}

	*nextTime = baseTime + *timeGap;

	return ERR_TCONV_OK;
}

static HERROR local_tconv_GetNextTuesdayToSaturday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if (!nextTime || !timeGap)
	{
		return ERR_TCONV_FAIL;
	}

	local_tconv_ConvertUnixTimeToWeekday(baseTime, &weekday);

	switch (weekday)
	{
	/* 월 ~ 금요일은 +1일을 한다. */
	case WEEKDAY_MONDAY :
	case WEEKDAY_TUESDAY :
	case WEEKDAY_WEDNESDAY :
	case WEEKDAY_THURSDAY :
	case WEEKDAY_FRIDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY;
		break;

	/* 토요일은 +3일을 하여 화요일을 만들어 준다. */
	case WEEKDAY_SATURDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY * 3;
		break;
	/* 일요일은 +2일(화요일)*/
	case WEEKDAY_SUNDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY * 2;
		break;
	default :
		HxLOG_Error("Error");
		return ERR_TCONV_FAIL;
	}

	*nextTime = baseTime + *timeGap;

	return ERR_TCONV_OK;
}


static HERROR local_tconv_GetNextWeekend(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if (!nextTime || !timeGap)
	{
		return ERR_TCONV_FAIL;
	}

	local_tconv_ConvertUnixTimeToWeekday(baseTime, &weekday);

	switch (weekday)
	{
	/* 토요일은 일요일로 만든다. */
	case WEEKDAY_SATURDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY;
		break;

	/* 일 ~ 금요일은 토요일로 만들어 준다. */
	case WEEKDAY_SUNDAY :
	case WEEKDAY_MONDAY :
	case WEEKDAY_TUESDAY :
	case WEEKDAY_WEDNESDAY :
	case WEEKDAY_THURSDAY :
	case WEEKDAY_FRIDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY * (WEEKDAY_SATURDAY - weekday);
		break;

	default :
		HxLOG_Error("\n");
		return ERR_TCONV_FAIL;
	}

	*nextTime = baseTime + *timeGap;

	return ERR_TCONV_OK;
}


static HBOOL		local_tconv_LeapYear( HUINT16 year )
{
	if( (year%4) == 0 &&  ((year) % 100 != 0 ||  (year) % 400 ==0) )
	{
		return TRUE;
	}

	return FALSE;
}

/* AD(anno domini) 1년 1월 1일을 월요일 기준으로 해당 날짜의 요일을 나타내는 함수. */
static HUINT32		local_tconv_GetWeekDayIndex(DATE_T stDate)
{
	HUINT16			year = 1;	/* 0년도는 제외한다 */
	HUINT32			day = 0;
	HUINT16			*aSumDay = NULL;
	HUINT16			sumDay[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	HUINT16			sumLDay[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

	while( year < stDate.usYear)
	{
		if( local_tconv_LeapYear(year) )
		{
			day += 366;
		}
		else
		{
			day += 365;
		}
		year++;
	}

	if( local_tconv_LeapYear(stDate.usYear) )
	{
		aSumDay = sumLDay;
	}
	else
	{
		aSumDay = sumDay;
	}

	day += aSumDay[stDate.ucMonth - 1];
	day += stDate.ucDay;

	return day % 7;

}


/* 해당 년도의 주차를 나타내는 Index Value. */
static HUINT32		local_tconv_GetWeekNofYear (DATE_T stDate)
{
	HUINT32		day = 0, firstDay;
	HUINT16		*aSumDay = NULL;
	HUINT16		sumDay[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	HUINT16		sumLDay[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };
	DATE_T		date;

	/* 해당 년도 1월 1일 요일 Index를 얻어 온다. */
	date.usYear = stDate.usYear;
	date.ucMonth = 1;
	date.ucDay = 1;

	firstDay = local_tconv_GetWeekDayIndex(date);

	if( local_tconv_LeapYear(stDate.usYear) )
	{
		aSumDay = sumLDay;
	}
	else
	{
		aSumDay = sumDay;
	}

	day += aSumDay[stDate.ucMonth - 1];
	day += stDate.ucDay;

	/* 1주차부터 시작이기 때문에 +1을 해준다.*/
	return (day + firstDay) / 7 + 1;

}


/* AD 1년 부터 해당 날짜가 나타내는 주차 Index Value.
   ex) 2007.8.29일은 104775번째 주임.*/
static HUINT32		local_tconv_GetWeekIndex (DATE_T stDate)
{
	HUINT16			year = 1;	/* 0년도는 제외한다 */
	HUINT32			day = 0;
	HUINT16			*aSumDay = NULL;
	HUINT16			sumDay[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	HUINT16			sumLDay[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

	while( year < stDate.usYear)
	{
		if( local_tconv_LeapYear(year) )
		{
			day += 366;
		}
		else
		{
			day += 365;
		}
		year++;
	}

	if( local_tconv_LeapYear(stDate.usYear) )
	{
		aSumDay = sumLDay;
	}
	else
	{
		aSumDay = sumDay;
	}

	day += aSumDay[stDate.ucMonth - 1];
	day += stDate.ucDay;

	return day / 7 + 1;	/* 1주차부터 시작이기 때문에 +1을 해준다.*/
}

static HUINT8	local_tconv_GetLastDayOfMonth(HUINT16 year, HUINT8 month)
{
	HUINT8		allDay[]  = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	HUINT8		allLDay[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if( (month < 1) || (month > 12) )
	{
		return 0;
	}

	if( local_tconv_LeapYear(year) )
	{
		return allLDay[month-1];
	}
	else
	{
		return allDay[month-1];
	}
}

static HUINT32	local_tconv_GetTotalDays(DATE_T stDate)
{
	HUINT16			year = 1;	/* 0년도는 제외한다 */
	HUINT32			day = 0;
	HUINT16			*aSumDay = NULL;
	HUINT16			sumDay[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	HUINT16			sumLDay[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

	while( year < stDate.usYear)
	{
		if( local_tconv_LeapYear(year) )
		{
			day += 366;
		}
		else
		{
			day += 365;
		}
		year++;
	}

	if( local_tconv_LeapYear(stDate.usYear) )
	{
		aSumDay = sumLDay;
	}
	else
	{
		aSumDay = sumDay;
	}

	day += aSumDay[stDate.ucMonth - 1];
	day += stDate.ucDay;

	return day;
}

static HINT32	local_tconv_CompareDate(DATE_T stDate1, DATE_T stDate2)
{
	return (HINT32)(local_tconv_GetTotalDays(stDate1) - local_tconv_GetTotalDays(stDate2));
}

#if defined(CONFIG_DEBUG)

HERROR UTIL_PrintCurrentDateTime(void)
{
	unsigned long UnixTime;
	DATE_TIME_T dateTime;

	VK_memset(&dateTime,0,sizeof(DATE_TIME_T));

	if(VK_CLOCK_GetTime(&UnixTime) != ERR_OK)
	{
		return ERR_FAIL;
	}

	/* 만약 local time이 invalide 하다면, 에러가 나겠지. */
	UTIL_TCONV_ConvertUnixTimeToDateTime(UnixTime, &dateTime);

	HxLOG_Print("\n\rCURRENT TIME is [%d-%d-%d] : [%d-%d-%d-%d]\n\n\r",dateTime.stDate.usYear,dateTime.stDate.ucMonth,dateTime.stDate.ucDay,
		dateTime.stTime.ucHour,dateTime.stTime.ucMinute,dateTime.stTime.ucSecond,dateTime.stTime.usMillisecond);
	return ERR_OK;
}

HERROR UTIL_PrintDateTime(UNIXTIME unixtime)
{
	DATE_TIME_T dateTime;

	VK_memset(&dateTime,0,sizeof(DATE_TIME_T));

	/* 만약 local time이 invalide 하다면, 에러가 나겠지. */
	UTIL_TCONV_ConvertUnixTimeToDateTime(unixtime, &dateTime);

	HxLOG_Print("\r TIME is (0x%x) [%d-%d-%d] : [%d-%d-%d-%d]\r\n",unixtime, dateTime.stDate.usYear,dateTime.stDate.ucMonth,dateTime.stDate.ucDay,
		dateTime.stTime.ucHour,dateTime.stTime.ucMinute,dateTime.stTime.ucSecond,dateTime.stTime.usMillisecond);
	return ERR_OK;
}

#endif
#endif
/* end of file */
