
/*************************************************************
 * @file		hxdatetime.h
 * @date		2011/04/19
 * @author
 * @brief		HUMAX common library header, related with DateTime
 *
 * http://www.humaxdigital.com
 *************************************************************/

#ifndef	_HXDATETIME_H_
#define	_HXDATETIME_H_


/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include "_hlib_country.h"

/* *************************************************************************************************** */
#define ___Global_Variables______________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___Macro_Definition______________________________________________________________________________
/* *************************************************************************************************** */
#define		DAYS_400YEARS					146097L		// 146097L;
#define		FUDGED_DAYS_4000YEARS			1461001L	// 1460970L + 31;
#define		UNIX_FIRST_DAY					2440588L
#define		HOURS_PER_DAY					24
#define		MINS_PER_HOUR					60
#define		SECONDS_PER_MIN					60
#define		SECONDS_PER_HOUR				(SECONDS_PER_MIN*MINS_PER_HOUR)
#define		SECONDS_PER_HALF				(SECONDS_PER_HOUR/2)
#define		SECONDS_PER_DAY					(SECONDS_PER_HOUR*HOURS_PER_DAY)
#define 	SECONDS_PER_WEEK				(SECONDS_PER_DAY * 7)
#define		DAY_PER_WEEK					7


#define 	EVERY_WORKDAY					0xFFFFFFFF
#define		WEEKDAY_SUNDAY					0
#define		WEEKDAY_MONDAY					1
#define		WEEKDAY_TUESDAY					2
#define		WEEKDAY_WEDNESDAY				3
#define		WEEKDAY_THURSDAY				4
#define		WEEKDAY_FRIDAY					5
#define		WEEKDAY_SATURDAY				6
#define		TIME_DURATION_FOR_READY			60

#define		TZ_NAME_PATH			CONFIG_PRODUCT_DATA_DIR "/TZ"
#define		TZ_NAME_PATH_TMP		"/tmp/TZ"
#define		TZ_NAME_LENGTH			128

#if defined(CONFIG_MW_SI_LOCALTIME)
#define		LOCALTIME_ZONEINFO_PATH		"/usr/share/zoneinfo/"
#define		LOCALTIME_NAME_PATH			CONFIG_PRODUCT_DATA_DIR "/localtime"
#define		LOCALTIME_NAME_PATH_TMP		"/tmp/localtime"
#define 	LOCALTIME_COPY_CMD			"ln -s " LOCALTIME_NAME_PATH " " LOCALTIME_NAME_PATH_TMP
#define		LOCALTIME_NAME_LENGTH		128
#endif

#define VALID_UNIXTIME					(60 * 60 * 24 * 366)			// until 1971/01/02

typedef enum
{
	eWeekday_Sun,
	eWeekday_Mon,
	eWeekday_Tue,
	eWeekday_Wed,
	eWeekday_Thu,
	eWeekday_Fri,
	eWeekday_Sat
} HxDATETIME_Weekday_e;

typedef enum
{
	eWeeks_1TH		=	1,
	eWeeks_2TH,
	eWeeks_3TH,
	eWeeks_4TH,
	eWeeks_5TH,
	eWeeks_LAST		=	eWeeks_5TH,
} HxDATETIME_Weeks_e;

//#ifndef	UNIXTIME
//	typedef	HUINT32	UNIXTIME;
//#endif

typedef	struct
{
	HUINT16	usYear;
	HUINT8	ucMonth;
	HUINT8	ucDay;
} HxDATETIME_Date_t;

typedef struct
{
	HUINT8	ucHour;
	HUINT8	ucMinute;
	HUINT8	ucSecond;
	HUINT16	usMillisecond;
} HxDATETIME_Time_t;

typedef	struct
{
	HxDATETIME_Date_t	stDate;
	HxDATETIME_Time_t	stTime;
} HxDATETIME_t;

typedef struct
{
	HUINT8		 ucMonth;
	HxDATETIME_Weeks_e	 eWeekth;
	HxDATETIME_Weekday_e	 eWeekday;
	HUINT8		 ucHour;
	HUINT8		 ucMinute;
} HxDATETIME_Dst_t;

typedef struct
{
	HINT32			 nStdOffsetHour, nStdOffsetMin;
	HCHAR			 szTzStdStr[8];

	HBOOL			 bDstEnabled;
	HINT32			 nDstOffsetHour, nDstOffsetMin;
	HCHAR			 szTzDstStr[8];

	HBOOL			 bDstRuleEnabled;
	HxDATETIME_Dst_t	 stDstStart;
	HxDATETIME_Dst_t	 stDstEnd;
} HxDATETIME_TimeZone_t;

typedef struct
{
	HxCountry_e			 eCountry;
	HUINT8				 ucRegionId;						// country region id, TOT 사용하는 국가에서만 사용, default 0(no time zone extension used)
	HINT32				 nStdOffsetMin;						// Local Time Offset Minute
	HINT32				 nDstOffsetMin;						// DST 적용시 + 되는 추가 Offset Minute
	HxDATETIME_Dst_t	 DstStartInfo;					// GMT0 일 때 DST가 시작하는 날
	HxDATETIME_Dst_t	 DstEndInfo;					// GMT0 일 때 DST가 시작하는 날
} HxDATETIME_TzDstInfo_t;

#if defined(CONFIG_MW_SI_LOCALTIME) && !defined(CONFIG_MW_SI_TIMEZONEDB_BYTOT)
typedef struct
{
	HxCountry_e			  eCountry;
	HUINT8				  ucRegionId;						// country region id, TOT 사용하는 국가에서만 사용, default 0(no time zone extension used)
	HCHAR				 *szZoneInfoFile;					// /usr/share/zoneinfo 하위에 위치한 Zone Info File
} HxDATETIME_LTZoneInfo_t;
#endif

/* *************************************************************************************************** */
#define ___Function_Definition___________________________________________________________________________
/* *************************************************************************************************** */
HERROR		HLIB_DATETIME_Init(void);
HERROR 		HLIB_DATETIME_IsValidUnixTime(UNIXTIME ulUnixTime);
HBOOL 		HLIB_DATETIME_IsTimeDurationOverNHours(UNIXTIME startTime, UNIXTIME endTime, HUINT8 nHours);
HBOOL 		HLIB_DATETIME_IsTimeOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
HBOOL 		HLIB_DATETIME_IsTimeOverwrappedWithin24Hours(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
HBOOL 		HLIB_DATETIME_IsTimeOverwrappedWithRepeatWithin24Hours(UNIXTIME startTime1, UNIXTIME endTime1, HUINT32 Repeat1, UNIXTIME startTime2, UNIXTIME endTime2, HUINT32 Repeat2);
HBOOL 		HLIB_DATETIME_IsTimeOverwrappedWithinWeek(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
HBOOL 		HLIB_DATETIME_IsWeekdayOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
HBOOL 		HLIB_DATETIME_IsDateOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
HBOOL 		HLIB_DATETIME_IsWorkDay(UNIXTIME startTime, UNIXTIME endTime);
HBOOL 		HLIB_DATETIME_IsMondayToSaturday(UNIXTIME startTime, UNIXTIME endTime);
HBOOL		HLIB_DATETIME_IsTuesdayToSaturday(UNIXTIME startTime, UNIXTIME endTime);

HERROR		HLIB_DATETIME_ConvertLocalUnixTmeToUnixTime(UNIXTIME ulLocalUnixtime, UNIXTIME *pulUnixTime);
HERROR		HLIB_DATETIME_ConvertUnixTmeToLocalUnixTime(UNIXTIME unixtime, UNIXTIME *pulLocalTime);
HERROR 		HLIB_DATETIME_ConvertDateToJulian(HxDATETIME_Date_t *date, HINT32 *julian);
HERROR 		HLIB_DATETIME_ConvertJulianToDate(HINT32 julian, HxDATETIME_Date_t *date);
HERROR 		HLIB_DATETIME_ConvertMJDToYMD(HUINT16 mjd, HxDATETIME_Date_t *date);
HERROR 		HLIB_DATETIME_ConvertYMDToMJD(HxDATETIME_Date_t *date, HUINT16 *mjd);
HERROR 		HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime(HUINT8 *mjdBcdTime, UNIXTIME *unixtime);
HERROR 		HLIB_DATETIME_ConvertUnixTimeToMJDBcdTime(UNIXTIME unixtime, HUINT8 *mjdUtc);
HERROR 		HLIB_DATETIME_ConvertUnixTimeToMJD(UNIXTIME unixtime, HUINT16 *mjd);
HERROR 		HLIB_DATETIME_ConvertMJDToDate(HUINT16 mjd, HxDATETIME_Date_t *date);
HERROR 		HLIB_DATETIME_ConvertDateToMJD(HxDATETIME_Date_t *date, HUINT16 *mjd);
HERROR 		HLIB_DATETIME_ConvertBcdTimeToDecimalTime(HUINT8 *bcdTime, HINT32 nLength, HxDATETIME_Time_t *pTime);
HERROR 		HLIB_DATETIME_ConvertDecimalTimeToBcdTime(HxDATETIME_Time_t *time, HUINT8 *bcdTime);
HERROR 		HLIB_DATETIME_ConvertBcdTimeToUnixTime (HUINT8 *bcdTime, UNIXTIME *unixtime);

HERROR 		HLIB_DATETIME_ConvertUnixTimeToDateTime(UNIXTIME	unixtime, HxDATETIME_t *dateTime);
HERROR		HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOffset(UNIXTIME unixtime, HINT32 nOffsetHour, HINT32 nOffsetMin, HxDATETIME_t *dateTime);
HERROR 		HLIB_DATETIME_ConvertDateTimeToUnixTime(HxDATETIME_t *dateTime, UNIXTIME *unixtime);
HERROR 		HLIB_DATETIME_ConvertUnixTimeToDate(UNIXTIME	unixtime, HxDATETIME_Date_t *date);
HERROR 		HLIB_DATETIME_ConvertUnixTimeToDateWithOutOffset(UNIXTIME	unixtime, HxDATETIME_Date_t *date);
HERROR 		HLIB_DATETIME_ConvertUnixTimeToTime(UNIXTIME	unixtime, HxDATETIME_Time_t *time);
HERROR 		HLIB_DATETIME_ConvertUnixTimeToTimeWithOutOffset(UNIXTIME	unixtime, HxDATETIME_Time_t *time);
HERROR 		HLIB_DATETIME_ConvertUnixTimeToWeekday(UNIXTIME	unixtime, HUINT8 *weekday);
HERROR		HLIB_DATETIME_ConvertDurationToTime (UNIXTIME ulDuration, HxDATETIME_Time_t *time);
HERROR		HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset (UNIXTIME unixtime, HxDATETIME_t *dateTime);
HERROR		HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset (HxDATETIME_t *dateTime, UNIXTIME *unixtime);
HERROR		HLIB_DATETIME_GetPrevDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
HERROR		HLIB_DATETIME_GetNextDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
HERROR		HLIB_DATETIME_GetPrevWeek(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap);
HERROR		HLIB_DATETIME_GetNextWeek(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
HERROR		HLIB_DATETIME_GetTimeOffsetWithDst (HINT32 *pnOffsetMin);
HERROR 		HLIB_DATETIME_GetTimeOffsetWithDstAtSpecificLocalTime (HUINT32 ulLocalTime, HINT32 *pnOffsetMin);
HERROR		HLIB_DATETIME_GetOffsetTime(HINT32 *pnHour, HINT32 *pnMin);
HERROR 		HLIB_DATETIME_GetWeekDayFromMJD(HUINT16 mjd, HUINT16 *weekday);
HERROR 		HLIB_DATETIME_GetWeekdayFromDate(HxDATETIME_Date_t *date, HUINT16 *weekday);
HERROR		HLIB_DATETIME_GetPrevWorkDay(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap);
HERROR		HLIB_DATETIME_GetNextWorkDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
HERROR		HLIB_DATETIME_GetPrevWithoutSunday(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap);
HERROR		HLIB_DATETIME_GetNextWithoutSunday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
HERROR		HLIB_DATETIME_GetPrevTuesdayToSaturday(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap);
HERROR		HLIB_DATETIME_GetNextTuesdayToSaturday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
HERROR		HLIB_DATETIME_GetPrevFewDayofWeek(HUINT32 ulDays, UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap);
HERROR		HLIB_DATETIME_GetNextFewDayofWeek(HUINT32 ulDays, UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
HERROR		HLIB_DATETIME_GetPrevWeekend(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap);
HERROR		HLIB_DATETIME_GetNextWeekend(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
HUINT32		HLIB_DATETIME_GetWeekDayIndex(HxDATETIME_Date_t stDate);
HUINT32		HLIB_DATETIME_GetWeekNofYear(HxDATETIME_Date_t stDate);
HUINT32		HLIB_DATETIME_GetWeekIndex(HxDATETIME_Date_t stDate);
HUINT32		HLIB_DATETIME_GetDaysInYear(HxDATETIME_Date_t stDate);
HUINT8		HLIB_DATETIME_GetLastDayOfMonth(HUINT16 year, HUINT8 month);
HxDATETIME_Date_t	HLIB_DATETIME_GetDateFromWeekAndWeekDay (HUINT16 usYear, HUINT8 ucMonth, HUINT8 ucWeek, HxDATETIME_Weekday_e eWeekday);
HERROR		HLIB_DATETIME_GetProperTimeOffsetFromTzString (HCHAR *szTzString, UNIXTIME utCurrTime, HINT32 *pnOffsetHour, HINT32 *pnOffsetMin, HBOOL bLocalTime);
HBOOL		HLIB_DATETIME_LeapYear( HUINT16 year );
HINT32		HLIB_DATETIME_CompareDate(HxDATETIME_Date_t stDate1, HxDATETIME_Date_t stDate2);
HINT32		HLIB_DATETIME_CompareTime(HxDATETIME_Time_t stTime1, HxDATETIME_Time_t stTime2);
HCHAR *		HLIB_DATETIME_UnixTimeToString (UNIXTIME unixtime, HCHAR *buf, HUINT32 n);
HCHAR *		HLIB_DATETIME_UnixTimeToStringWithOutOffset (UNIXTIME unixtime, HCHAR *buf, HUINT32 n);
HERROR		HLIB_DATETIME_ParseTzString (HCHAR *szTzString, HxDATETIME_TimeZone_t *pstTimezone);
HERROR		HLIB_DATETIME_GetStringByDateTime(const HxDATETIME_t *pstDateTime, HCHAR *pszResult, HINT32 len);
HERROR		HLIB_DATETIME_GetDateTimeByString(const HCHAR *pszString, HxDATETIME_t *pstDateTime);
HERROR 		HLIB_DATETIME_GetTimeOffsetByCountryIndex (HxCountry_e eCountry, HINT32 *pnHour, HUINT32 *pnMin);
HERROR		HLIB_DATETIME_GetLastSundayXThFromDate(HxDATETIME_Date_t stDate, HxDATETIME_Date_t *lastSundayDate, HUINT32 *lastSundayXTh);
HINT32		HLIB_DATETIME_GetWeekDayXThFromDate(HxDATETIME_Date_t  stDate, HUINT32 ulWeekIndex, HUINT32 ulWeeks, HxDATETIME_Date_t *pstDate);
HERROR 		HLIB_DATETIME_MakeTzFileFromKnownTzCode(HUINT32 ulCurrTime, HxCountry_e eCountry, HUINT32 ulRegionId, HCHAR *szTzDstString);
HERROR 		HLIB_DATETIME_MakeTzFileFromTot(UNIXTIME ulCurrTime, HxCountry_e country, UNIXTIME ulTimeOfChange,
												HINT32 nLocalTimeOffset, HINT32 nNextLocalTimeOffset, HCHAR *szTzDstString);

#if defined(CONFIG_MW_SI_LOCALTIME) && !defined(CONFIG_MW_SI_TIMEZONEDB_BYTOT)
HERROR 		HLIB_DATETIME_GetLocalTimeZoneInfoFile(HxCountry_e eCountry, HUINT8 ucRegionId, HCHAR **szFilename);
#endif

// debug
HERROR 		HLIB_DATETIME_PrintDateTime(UNIXTIME unixtime);
HERROR 		HLIB_DATETIME_PrintCurrentDateTime(void);


/* *************************************************************************************************** */
#define ___Debug_Definition___________________________________________________________________________
/* *************************************************************************************************** */

#endif	/* _HXDATETIME_H_ */

