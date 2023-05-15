
/* *************************************************************************************************** */
#define ___HEADER________________________________________________________________________________________
/* *************************************************************************************************** */
#include <vkernel.h>
#include <hlib.h>

/* *************************************************************************************************** */
#define ___GLOBAL_HEADER_________________________________________________________________________________
/* *************************************************************************************************** */

/* *************************************************************************************************** */
#define ___DEFINE________________________________________________________________________________________
/* *************************************************************************************************** */
#undef	HxLOG_DOMAIN
#define	HxLOG_DOMAIN	"HxDATETIME"

/* *************************************************************************************************** */
#define ___TYPEDEF_______________________________________________________________________________________
/* *************************************************************************************************** */
typedef struct
{
	HUINT32		 ulYear;
	UNIXTIME	 ulDstStartTime;		// Daylight Saving이 시작하는 시간 : GMT 0
	UNIXTIME	 ulDstEndTime;			// Daylight Saving이 끝나는 시간 : GMT 0
} DST_RANGE_t;

typedef enum
{
	TZ_TOKENTYPE_NONE,
	TZ_TOKENTYPE_STRING,
	TZ_TOKENTYPE_NUMBER,
	TZ_TOKENTYPE_PLUS,			// +
	TZ_TOKENTYPE_MINUS,			// -
	TZ_TOKENTYPE_COLON,			// :
	TZ_TOKENTYPE_COMMA,			// ,
	TZ_TOKENTYPE_PERIOD,		// .
	TZ_TOKENTYPE_SLASH,			// /
	TZ_TOKENTYPE_END			// String 끝
} _tzTokenType_e;

typedef enum
{
	TZ_PARSE_PHASE_START,

	TZ_PARSE_PHASE_NORMAL_STR,
	TZ_PARSE_PHASE_NORMAL_OFFSET,
	TZ_PARSE_PHASE_DST_STR,
	TZ_PARSE_PHASE_DST_OFFSET,
	TZ_PARSE_PHASE_DSTSTART_DAY,
	TZ_PARSE_PHASE_DSTSTART_TIME,
	TZ_PARSE_PHASE_DSTEND_DAY,
	TZ_PARSE_PHASE_DSTEND_TIME,

	TZ_PARSE_PHASE_END
} _tzParsePhase_e;

typedef struct
{
	_tzTokenType_e		 eType;
	union
	{
		HCHAR			 szString[8];
		HINT32			 nNumber;
	} unVal;
} _tzToken_t;


/* *************************************************************************************************** */
#define ___STATIC_VARIABLES______________________________________________________________________________
/* *************************************************************************************************** */
//static DST_RANGE_t			 s_stLastDstRange = { 0, 0, 0 };
static	HUINT8	s_szTzString[TZ_NAME_LENGTH];			// TZ 정보 저장.
static	HINT32  s_nOffsetHour;							// TimeOffset Hour 저장
static	HINT32  s_nOffsetMin;							// TimeOffset Min 저장



HxDATETIME_TzDstInfo_t		 s_stTime_TzDsts[] =
{
/* Germany */						{ eCountryID_DEU, 	0, 	1 * 60, 	2 * 60, 	{ 3, eWeeks_LAST, eWeekday_Sun, 2, 0 }, 	{ 10, eWeeks_LAST, eWeekday_Sun, 3,  0 } 	},
/* Italy */ 								{ eCountryID_ITA,   0,	1 * 60, 	2 * 60, 	{ 3, eWeeks_LAST, eWeekday_Sun, 2, 0 }, 	{ 10, eWeeks_LAST, eWeekday_Sun, 3,  0 }	},
/* Swiss */ 							{ eCountryID_CHE,	0,	1 * 60, 	2 * 60, 	{ 3, eWeeks_LAST, eWeekday_Sun, 2, 0 }, 	{ 10, eWeeks_LAST, eWeekday_Sun, 3,  0 }	},
/* Austria */							{ eCountryID_AUT,	0,	1 * 60, 	2 * 60, 	{ 3, eWeeks_LAST, eWeekday_Sun, 2, 0 }, 	{ 10, eWeeks_LAST, eWeekday_Sun, 3,  0 }	},
/* Japan */ 							{ eCountryID_JPN,	0,	9 * 60, 	0 * 60, 	{ 1, eWeeks_1TH,  eWeekday_Sun, 0, 0 }, 	{ 12, eWeeks_LAST, eWeekday_Sun, 12, 0 }	},


/* WA, QLD, NT는 DST 사용하지 않음. 나머지는 DST 사용 (+1) - http://australia.gov.au/about-australia/our-country/time */
/* Australia - NSW/ACT 	*/			{ eCountryID_AUS, 	2, 	10 * 60, 	11 * 60, 	{ 10, eWeeks_1TH, eWeekday_Sun, 2, 0 }, 	{ 4, eWeeks_1TH, eWeekday_Sun, 3,  0 } 		},
/* Australia - VIC		*/				{ eCountryID_AUS,	3,	10 * 60,	11 * 60,	{ 10, eWeeks_1TH, eWeekday_Sun, 2, 0 }, 	{ 4, eWeeks_1TH, eWeekday_Sun, 3,  0 }		},
/* Australia - QLD 		*/				{ eCountryID_AUS,	4,	10 * 60,	10 * 60,	{ 10, eWeeks_1TH, eWeekday_Sun, 2, 0 }, 	{ 4, eWeeks_1TH, eWeekday_Sun, 3,  0 }		},
/* Australia - SA 			*/			{ eCountryID_AUS,	5,	9.5 * 60,	10.5 * 60,	{ 10, eWeeks_1TH, eWeekday_Sun, 2, 0 }, 	{ 4, eWeeks_1TH, eWeekday_Sun, 3,  0 }		},
/* Australia - WA 		*/				{ eCountryID_AUS,	6,	8 * 60,		8 * 60,		{ 10, eWeeks_1TH, eWeekday_Sun, 2, 0 }, 	{ 4, eWeeks_1TH, eWeekday_Sun, 3,  0 }		},
/* Australia - TAS 		*/				{ eCountryID_AUS,	7,	10 * 60,	11 * 60,	{ 10, eWeeks_1TH, eWeekday_Sun, 2, 0 }, 	{ 4, eWeeks_1TH, eWeekday_Sun, 3,  0 }		},
/* Australia - NT 			*/			{ eCountryID_AUS,	8,	9.5 * 60,	9.5 * 60,	{ 10, eWeeks_1TH, eWeekday_Sun, 2, 0 }, 	{ 4, eWeeks_1TH, eWeekday_Sun, 3,  0 }		},
/* UK */								{ eCountryID_GBR,	0,	0 * 60, 	1 * 60, 	{ 3, eWeeks_LAST, eWeekday_Sun, 1, 0 }, 	{ 10, eWeeks_LAST, eWeekday_Sun, 2,  0 }	},
/* Sweden */							{ eCountryID_SWE,	0,	1 * 60, 	2 * 60, 	{ 3, eWeeks_LAST, eWeekday_Sun, 2, 0 }, 	{ 10, eWeeks_LAST, eWeekday_Sun, 3,  0 }	},

/* NONE TEST Central Europe */		{ eCountryID_NONE, 	0, 	1 * 60, 	2 * 60, 	{ 3, eWeeks_LAST, eWeekday_Sun, 2, 0 }, 	{ 10, eWeeks_LAST, eWeekday_Sun, 0,  0 }	},
/* OTHER TEST Central Europe */	{ eCountryID_OTHER, 0, 	1 * 60, 	2 * 60, 	{ 3, eWeeks_LAST, eWeekday_Sun, 2, 0 },		{ 10, eWeeks_LAST, eWeekday_Sun, 0,  0 }	},
};

#if defined(CONFIG_MW_SI_LOCALTIME) && !defined(CONFIG_MW_SI_TIMEZONEDB_BYTOT)
HxDATETIME_LTZoneInfo_t		s_stLocalTime_ZoneInfo[] =
{
	/* Sweden */	{	eCountryID_SWE,		0, 	"Europe/Stockholm"		},
	/* Finland */	{	eCountryID_FIN, 	0, 	"Europe/Helsinki"		},
	/* Germany */	{	eCountryID_DEU, 	0, 	"Europe/Berlin"			},
	/* Demark */	{	eCountryID_DNK, 	0, 	"Europe/Copenhagen"		},

	/* End Of */		{	eCountryID_NONE, 	0,  ""						}
};
#endif


/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
static HERROR hlib_datetime_GetOffsetTimeAtSpecificTime(UNIXTIME utTime, HINT32 *pnHour, HINT32 *pnMin, HBOOL bLocalTime);
static HBOOL hlib_datetime_IsTimeDurationOverNHours(UNIXTIME startTime, UNIXTIME endTime, HUINT8 nHours);
static HBOOL hlib_datetime_IsTimeOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
static HBOOL hlib_datetime_IsTimeOverwrappedWithin24Hours(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
static HBOOL hlib_datetime_IsTimeOverwrappedWithRepeatWithin24Hours(UNIXTIME startTime1, UNIXTIME endTime1, HUINT32 Repeat1, UNIXTIME startTime2, UNIXTIME endTime2, HUINT32 Repeat2);
static HBOOL hlib_datetime_IsTimeOverwrappedWithinWeek(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
static HBOOL hlib_datetime_IsWeekdayOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
static HBOOL hlib_datetime_IsDateOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2);
static HBOOL hlib_datetime_IsWorkDay(UNIXTIME startTime, UNIXTIME endTime);
static HBOOL hlib_datetime_IsMondayToSaturday(UNIXTIME startTime, UNIXTIME endTime);
static HBOOL hlib_datetime_IsTuesdayToSaturday(UNIXTIME startTime, UNIXTIME endTime);
static HERROR hlib_datetime_ConvertDateToJulian(HxDATETIME_Date_t *date, HINT32 *julian);
static HERROR hlib_datetime_ConvertJulianToDate(HINT32 julian, HxDATETIME_Date_t *date);
static HERROR hlib_datetime_ConvertMJDToYMD(HUINT16 mjd, HxDATETIME_Date_t *date);
static HERROR hlib_datetime_ConvertYMDToMJD(HxDATETIME_Date_t *date, HUINT16 *mjd);
static HERROR hlib_datetime_GetWeekDayFromMJD(HUINT16 mjd, HUINT16 *weekday);
static HERROR hlib_datetime_GetWeekdayFromDate(HxDATETIME_Date_t *date, HUINT16 *weekday);
static HERROR hlib_datetime_ConvertMJDBcdTimeToUnixTime(HUINT8 *mjdBcdTime, UNIXTIME *unixtime);
static HERROR hlib_datetime_ConvertUnixTimeToMJDBcdTime(UNIXTIME unixtime, HUINT8 *mjdUtc);
static HERROR hlib_datetime_ConvertUnixTimeToMJD(UNIXTIME unixtime, HUINT16 *mjd);
static HERROR hlib_datetime_ConvertMJDToDate(HUINT16 mjd, HxDATETIME_Date_t *date);
static HERROR hlib_datetime_ConvertDateToMJD(HxDATETIME_Date_t *date, HUINT16 *mjd);
static HERROR hlib_datetime_ConvertBcdTimeToDecimalTime(HUINT8 *bcdTime, HINT32 nLength, HxDATETIME_Time_t *pTime);
static HERROR hlib_datetime_ConvertDecimalTimeToBcdTime(HxDATETIME_Time_t *time, HUINT8 *bcdTime);
static HERROR hlib_datetime_ConvertBcdTimeToUnixTime (HUINT8 *bcdTime, UNIXTIME *unixtime);
static HERROR hlib_datetime_GetOffsetTime(UNIXTIME unixtime, HINT32 *pnHour, HINT32 *pnMin);
static HERROR hlib_datetime_ConvertUnixTimeToDateTime(UNIXTIME	unixtime, HxDATETIME_t *dateTime);
static HERROR hlib_datetime_ConvertDateTimeToUnixTime(HxDATETIME_t *dateTime, UNIXTIME *unixtime);
static HERROR hlib_datetime_ConvertUnixTimeToDate(UNIXTIME	unixtime, HxDATETIME_Date_t *date);
static HERROR hlib_datetime_ConvertUnixTimeToTime(UNIXTIME	unixtime, HxDATETIME_Time_t *time);
static HERROR hlib_datetime_ConvertUnixTimeToWeekday(UNIXTIME	unixtime, HUINT8 *weekday);
static UNIXTIME hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (UNIXTIME ulGmtTime);
static UNIXTIME hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (UNIXTIME ulLocalUnixTime);
static HERROR	hlib_datetime_GetPrevDay(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap);
static HERROR	hlib_datetime_GetNextDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
static HERROR	hlib_datetime_GetPrevWeek(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap);
static HERROR	hlib_datetime_GetNextWeek(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
static HERROR	hlib_datetime_GetPrevWorkDay(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap);
static HERROR	hlib_datetime_GetNextWorkDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
static HERROR	hlib_datetime_GetPrevWithoutSunday(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap);
static HERROR	hlib_datetime_GetNextWithoutSunday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
static HERROR	hlib_datetime_GetPrevTuesdayToSaturday(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap);
static HERROR 	hlib_datetime_GetNextTuesdayToSaturday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
static HERROR	hlib_datetime_GetPrevWeekend(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap);
static HERROR 	hlib_datetime_GetNextWeekend(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
static HERROR	hlib_datetime_GetPrevFewDayofWeek(HUINT32 ulDays, UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap);
static HERROR   hlib_datetime_GetNextFewDayofWeek(HUINT32 ulDays, UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap);
static HBOOL	hlib_datetime_LeapYear( HUINT16 year );
static HUINT32	hlib_datetime_GetWeekDayIndex(HxDATETIME_Date_t stDate);
static HUINT32	hlib_datetime_GetWeekNofYear (HxDATETIME_Date_t stDate);
static HUINT32	hlib_datetime_GetWeekIndex (HxDATETIME_Date_t stDate);
static HUINT8	hlib_datetime_GetLastDayOfMonth(HUINT16 year, HUINT8 month);
static HUINT32	hlib_datetime_GetTotalDays(HxDATETIME_Date_t stDate);
static HUINT32	hlib_datetime_GetDaysInYear(HxDATETIME_Date_t stDate);
static HINT32	hlib_datetime_CompareDate(HxDATETIME_Date_t stDate1, HxDATETIME_Date_t stDate2);
static HINT32	hlib_datetime_CompareTime(HxDATETIME_Time_t stTime1, HxDATETIME_Time_t stTime2);
static HxDATETIME_Date_t	hlib_datetime_GetDateFromWeekAndWeekDay (HUINT16 usYear, HUINT8 ucMonth, HUINT8 ucWeek, HxDATETIME_Weekday_e eWeekday);
static _tzTokenType_e	hlib_datetime_getCharToken (HCHAR ucChar);
static HCHAR*	hlib_datetime_getNextToken (HCHAR *szString, _tzToken_t *pstToken);
static HCHAR*	hlib_datetime_getHourMiniteFromTzString (HCHAR *szTzString, HBOOL *pbTimeFound, HINT32 *pnHour, HINT32 *pnMin);
static HERROR	hlib_datetime_parseTzString (HCHAR *szTzString, HxDATETIME_TimeZone_t *pstTimezone);
static HERROR 	hlib_datetime_getProperTimeOffsetFromTzString (HCHAR *szTzString, UNIXTIME utCurrTime, HINT32 *pnOffsetHour, HINT32 *pnOffsetMin, HBOOL bLocalTime);
static HCHAR*	hlib_datetime_getMonthWeekDayFromTzString (HCHAR *szTzString, HBOOL *pbTimeFound, HINT32 *pnMonth, HINT32 *pnWeek, HINT32 *pnWeekday);
static HERROR 	hlib_datetime_makeDateTzString (HxDATETIME_Date_t *pstDate, HCHAR *szDateTzString);
static HBOOL 	hlib_datetime_isEqualKnownTzCodeWithChangeDate(HxDATETIME_Dst_t dstCode, HxDATETIME_Date_t changeDate);
static HERROR 	hlib_datetime_makeTotDstTzStringFromKnownTzCode(HUINT32 ulCurrTime, HxCountry_e country, HUINT32 ulTimeOfChange, HINT32 nCurrOffset, HINT32 nNextOffset, HCHAR *szDstString);
static HERROR 	hlib_datetime_presumeTotDstTzString (HUINT32 ulCurrTime, HUINT32 ulTimeOfChange, HINT32 nCurrOffset, HINT32 nNextOffset, HCHAR *szDstString);


/* *************************************************************************************************** */
#define ___PUBLIC_FUNCTION_______________________________________________________________________________
/* *************************************************************************************************** */
HERROR	HLIB_DATETIME_Init(void)
{
	// Copy Origin Tz File To Tmp Path.
	// origin tz file is always lastest datetime infomation file.
	// so .. copy origin tz file to tmp path always on obama init
	HLIB_FILE_Copy(TZ_NAME_PATH, TZ_NAME_PATH_TMP);

#if defined(CONFIG_MW_SI_LOCALTIME)
	//HLIB_FILE_Copy(LOCALTIME_NAME_PATH, LOCALTIME_NAME_PATH_TMP);
	VK_SYSTEM_Command(LOCALTIME_COPY_CMD);
#endif

	return ERR_OK;
}
HERROR HLIB_DATETIME_IsValidUnixTime(UNIXTIME ulUnixTime)
{
	if (ulUnixTime > VALID_UNIXTIME)
	{
		return ERR_OK;

	}
	return ERR_FAIL;
}

HBOOL HLIB_DATETIME_IsTimeDurationOverNHours(UNIXTIME startTime, UNIXTIME endTime, HUINT8 nHours)
{

	return hlib_datetime_IsTimeDurationOverNHours(startTime, endTime, nHours) ;
}

HBOOL HLIB_DATETIME_IsTimeOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	return hlib_datetime_IsTimeOverwrapped(startTime1, endTime1, startTime2, endTime2) ;
}


HERROR HLIB_DATETIME_GetOffsetTime(HINT32 *pnHour, HINT32 *pnMin)
{
	unsigned long		 utCurrTime;

	utCurrTime = HLIB_STD_GetSystemTime();
	if (utCurrTime < VALID_UNIXTIME)
	{
		utCurrTime	= 0;
	}

	return hlib_datetime_GetOffsetTimeAtSpecificTime(utCurrTime, pnHour, pnMin, FALSE);
}

HERROR HLIB_DATETIME_GetTimeOffsetWithDst (HINT32 *pnOffsetMin)
{
	HINT32		nHour = 0, nMin = 0;

	if ( HLIB_DATETIME_GetOffsetTime(&nHour, &nMin) == ERR_OK )
	{
		*pnOffsetMin	= nHour * 60 + nMin;

		return ERR_OK;
	}
	return ERR_FAIL;
}

HERROR HLIB_DATETIME_GetTimeOffsetWithDstAtSpecificLocalTime (HUINT32 ulLocalTime, HINT32 *pnOffsetMin)
{
	HINT32		nHour = 0, nMin = 0;

	if ( hlib_datetime_GetOffsetTimeAtSpecificTime((HUINT32)ulLocalTime, &nHour, &nMin, TRUE) == ERR_OK )
	{
		*pnOffsetMin	= nHour * 60 + nMin;

		return ERR_OK;
	}
	return ERR_FAIL;
}


HBOOL HLIB_DATETIME_IsTimeOverwrappedWithin24Hours(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	UNIXTIME	 ulLocalStartTime1, ulLocalEndTime1;
	UNIXTIME	 ulLocalStartTime2, ulLocalEndTime2;

	ulLocalStartTime1	= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (startTime1);
	ulLocalEndTime1		= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (endTime1);
	ulLocalStartTime2	= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (startTime2);
	ulLocalEndTime2		= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (endTime2);

	return hlib_datetime_IsTimeOverwrappedWithin24Hours (ulLocalStartTime1, ulLocalEndTime1, ulLocalStartTime2, ulLocalEndTime2);
}

HBOOL HLIB_DATETIME_IsTimeOverwrappedWithRepeatWithin24Hours(UNIXTIME startTime1, UNIXTIME endTime1, HUINT32 Repeat1, UNIXTIME startTime2, UNIXTIME endTime2, HUINT32 Repeat2)
{
	UNIXTIME	 ulLocalStartTime1, ulLocalEndTime1;
	UNIXTIME	 ulLocalStartTime2, ulLocalEndTime2;

	ulLocalStartTime1	= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (startTime1);
	ulLocalEndTime1		= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (endTime1);
	ulLocalStartTime2	= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (startTime2);
	ulLocalEndTime2		= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (endTime2);

	return hlib_datetime_IsTimeOverwrappedWithRepeatWithin24Hours (ulLocalStartTime1, ulLocalEndTime1, Repeat1, ulLocalStartTime2, ulLocalEndTime2, Repeat2);
}

HBOOL HLIB_DATETIME_IsTimeOverwrappedWithinWeek(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	UNIXTIME	 ulLocalStartTime1, ulLocalEndTime1;
	UNIXTIME	 ulLocalStartTime2, ulLocalEndTime2;

	ulLocalStartTime1	= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (startTime1);
	ulLocalEndTime1		= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (endTime1);
	ulLocalStartTime2	= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (startTime2);
	ulLocalEndTime2		= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (endTime2);

	return hlib_datetime_IsTimeOverwrappedWithinWeek(ulLocalStartTime1, ulLocalEndTime1, ulLocalStartTime2, ulLocalEndTime2);
}

HBOOL HLIB_DATETIME_IsWeekdayOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	UNIXTIME	 ulLocalStartTime1, ulLocalEndTime1;
	UNIXTIME	 ulLocalStartTime2, ulLocalEndTime2;

	ulLocalStartTime1	= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (startTime1);
	ulLocalEndTime1		= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (endTime1);
	ulLocalStartTime2	= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (startTime2);
	ulLocalEndTime2		= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (endTime2);

	return hlib_datetime_IsWeekdayOverwrapped (ulLocalStartTime1, ulLocalEndTime1, ulLocalStartTime2, ulLocalEndTime2);
}

HBOOL HLIB_DATETIME_IsDateOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	UNIXTIME	 ulLocalStartTime1, ulLocalEndTime1;
	UNIXTIME	 ulLocalStartTime2, ulLocalEndTime2;

	ulLocalStartTime1	= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (startTime1);
	ulLocalEndTime1		= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (endTime1);
	ulLocalStartTime2	= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (startTime2);
	ulLocalEndTime2		= hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (endTime2);

	return hlib_datetime_IsDateOverwrapped (ulLocalStartTime1, ulLocalEndTime1, ulLocalStartTime2, ulLocalEndTime2);
}

HBOOL HLIB_DATETIME_IsWorkDay(UNIXTIME startTime, UNIXTIME endTime)
{
	return hlib_datetime_IsWorkDay (startTime, endTime);
}

HBOOL HLIB_DATETIME_IsMondayToSaturday(UNIXTIME startTime, UNIXTIME endTime)
{
	return hlib_datetime_IsMondayToSaturday (startTime, endTime);
}

HBOOL HLIB_DATETIME_IsTuesdayToSaturday(UNIXTIME startTime, UNIXTIME endTime)
{
	return hlib_datetime_IsTuesdayToSaturday (startTime, endTime);
}

HERROR HLIB_DATETIME_ConvertLocalUnixTmeToUnixTime(UNIXTIME ulLocalUnixtime, UNIXTIME *pulUnixTime)
{
	if(pulUnixTime == NULL)
	{
		return ERR_FAIL;
	}

	*pulUnixTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalUnixtime);

	return ERR_OK;
}

HERROR HLIB_DATETIME_ConvertUnixTmeToLocalUnixTime(UNIXTIME unixtime, UNIXTIME *pulLocalTime)
{
	if(pulLocalTime == NULL)
	{
		return ERR_FAIL;
	}

	*pulLocalTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (unixtime);

	return ERR_OK;
}

HERROR HLIB_DATETIME_ConvertDateToJulian (HxDATETIME_Date_t *date, HINT32 *julian)
{
	return hlib_datetime_ConvertDateToJulian(date, julian) ;
}

HERROR HLIB_DATETIME_ConvertJulianToDate(HINT32 julian, HxDATETIME_Date_t *date)
{
	return hlib_datetime_ConvertJulianToDate(julian, date) ;
}

HERROR HLIB_DATETIME_ConvertMJDToYMD(HUINT16 mjd, HxDATETIME_Date_t *date)
{
	return hlib_datetime_ConvertMJDToYMD(mjd, date) ;
}

HERROR HLIB_DATETIME_ConvertYMDToMJD(HxDATETIME_Date_t *date, HUINT16 *mjd)
{
	return hlib_datetime_ConvertYMDToMJD(date, mjd) ;
}

// MJDBCD Time은 SI에서 GMT 0로 오는 시그널링이기 때문에 Offset을 적용시키면 안 된다.
HERROR HLIB_DATETIME_ConvertMJDBcdTimeToUnixTime(HUINT8 *mjdBcdTime, UNIXTIME *unixtime)
{
	return hlib_datetime_ConvertMJDBcdTimeToUnixTime(mjdBcdTime, unixtime);
}

HERROR HLIB_DATETIME_ConvertUnixTimeToMJDBcdTime(UNIXTIME unixtime, HUINT8 *mjdUtc)
{
	return hlib_datetime_ConvertUnixTimeToMJDBcdTime(unixtime, mjdUtc);
}

HERROR HLIB_DATETIME_GetWeekDayFromMJD(HUINT16 mjd, HUINT16 *weekday)
{
	return hlib_datetime_GetWeekDayFromMJD(mjd, weekday) ;
}

HERROR HLIB_DATETIME_GetWeekdayFromDate(HxDATETIME_Date_t *date, HUINT16 *weekday)
{
	return hlib_datetime_GetWeekdayFromDate(date, weekday);
}

HERROR HLIB_DATETIME_GetPrevWorkDay(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalPrevTime;
	HERROR		 hErr;

	if (prevTime == NULL || timeGap == NULL)
	{
		return ERR_FAIL;
	}

	ulLocalBaseTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = hlib_datetime_GetPrevWorkDay (ulLocalBaseTime, &ulLocalPrevTime, timeGap);
	if (hErr == ERR_OK)
	{
		*prevTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalPrevTime);
		*timeGap = baseTime - *prevTime;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_GetNextWorkDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalNextTime;
	HERROR		 hErr;

	if (nextTime == NULL || timeGap == NULL)
	{
		return ERR_FAIL;
	}

	ulLocalBaseTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = hlib_datetime_GetNextWorkDay (ulLocalBaseTime, &ulLocalNextTime, timeGap);
	if (hErr == ERR_OK)
	{
		*nextTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalNextTime);
		*timeGap = *nextTime - baseTime;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_GetPrevWithoutSunday(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalPrevTime;
	HERROR		 hErr;

	if (prevTime == NULL || timeGap == NULL)
	{
		return ERR_FAIL;
	}

	ulLocalBaseTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = hlib_datetime_GetPrevWithoutSunday (ulLocalBaseTime, &ulLocalPrevTime, timeGap);
	if (hErr == ERR_OK)
	{
		*prevTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalPrevTime);
		*timeGap = baseTime - *prevTime;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_GetNextWithoutSunday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalNextTime;
	HERROR		 hErr;

	if (nextTime == NULL || timeGap == NULL)
	{
		return ERR_FAIL;
	}

	ulLocalBaseTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = hlib_datetime_GetNextWithoutSunday (ulLocalBaseTime, &ulLocalNextTime, timeGap);
	if (hErr == ERR_OK)
	{
		*nextTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalNextTime);
		*timeGap = *nextTime - baseTime;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_GetPrevTuesdayToSaturday(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalPrevTime;
	HERROR		 hErr;

	if (prevTime == NULL || timeGap == NULL)
	{
		return ERR_FAIL;
	}

	ulLocalBaseTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = hlib_datetime_GetPrevTuesdayToSaturday(ulLocalBaseTime, &ulLocalPrevTime, timeGap);
	if (hErr == ERR_OK)
	{
		*prevTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalPrevTime);
		*timeGap = baseTime - *prevTime;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_GetNextTuesdayToSaturday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalNextTime;
	HERROR		 hErr;

	if (nextTime == NULL || timeGap == NULL)
	{
		return ERR_FAIL;
	}

	ulLocalBaseTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = hlib_datetime_GetNextTuesdayToSaturday(ulLocalBaseTime, &ulLocalNextTime, timeGap);
	if (hErr == ERR_OK)
	{
		*nextTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalNextTime);
		*timeGap = *nextTime - baseTime;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_GetPrevFewDayofWeek(HUINT32 ulDays, UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalPrevTime;
	HERROR		 hErr;

	if (prevTime == NULL || timeGap == NULL || ulDays == 0)
	{
		return ERR_FAIL;
	}

	ulLocalBaseTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = hlib_datetime_GetPrevFewDayofWeek(ulDays, ulLocalBaseTime, &ulLocalPrevTime, timeGap);
	if (hErr == ERR_OK)
	{
		*prevTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalPrevTime);
		*timeGap = baseTime - *prevTime;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_GetNextFewDayofWeek(HUINT32 ulDays, UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalNextTime;
	HERROR		 hErr;

	if (nextTime == NULL || timeGap == NULL || ulDays == 0)
	{
		return ERR_FAIL;
	}

	ulLocalBaseTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = hlib_datetime_GetNextFewDayofWeek(ulDays, ulLocalBaseTime, &ulLocalNextTime, timeGap);
	if (hErr == ERR_OK)
	{
		*nextTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalNextTime);
		*timeGap = *nextTime - baseTime;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_GetPrevWeekend(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime;
	UNIXTIME	 ulLocalPrevTime;
	HERROR		 hErr;

	if (prevTime == NULL || timeGap == NULL)
	{
		return ERR_FAIL;
	}

	ulLocalBaseTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = hlib_datetime_GetPrevWeekend (ulLocalBaseTime, &ulLocalPrevTime, timeGap);
	if (hErr == ERR_OK)
	{
		*prevTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalPrevTime);
		*timeGap = baseTime - *prevTime;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_GetNextWeekend(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime;
	UNIXTIME	 ulLocalNextTime;
	HERROR		 hErr;

	if (nextTime == NULL || timeGap == NULL)
	{
		return ERR_FAIL;
	}

	ulLocalBaseTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = hlib_datetime_GetNextWeekend (ulLocalBaseTime, &ulLocalNextTime, timeGap);
	if (hErr == ERR_OK)
	{
		*nextTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalNextTime);
		*timeGap = *nextTime - baseTime;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_ConvertBcdTimeToUnixTime (HUINT8 *bcdTime, UNIXTIME *unixtime)
{
	return hlib_datetime_ConvertBcdTimeToUnixTime (bcdTime, unixtime);
}

HERROR HLIB_DATETIME_ConvertMJDToDate(HUINT16 mjd, HxDATETIME_Date_t *date)
{
	return hlib_datetime_ConvertMJDToDate(mjd, date) ;
}

HERROR HLIB_DATETIME_ConvertDateToMJD(HxDATETIME_Date_t *date, HUINT16 *mjd)
{
	return hlib_datetime_ConvertDateToMJD(date, mjd) ;
}

HERROR HLIB_DATETIME_ConvertBcdTimeToDecimalTime(HUINT8 *bcdTime, HINT32 nLength, HxDATETIME_Time_t *pTime)
{
	return hlib_datetime_ConvertBcdTimeToDecimalTime(bcdTime, nLength, pTime);
}

HERROR HLIB_DATETIME_ConvertDecimalTimeToBcdTime(HxDATETIME_Time_t *time, HUINT8 *bcdTime)
{
	return hlib_datetime_ConvertDecimalTimeToBcdTime(time, bcdTime) ;
}

HERROR HLIB_DATETIME_ConvertUnixTimeToMJD (UNIXTIME unixtime, HUINT16 *mjd)
{
	return hlib_datetime_ConvertUnixTimeToMJD (unixtime, mjd);
}

// 기본적으로 UnixTime2Date 나 Time 변환 함수는 offset 이 적용된 값.
// 순수하게 변환을 하려면 XXX_WithOut_XXX 류를 사용.
HERROR HLIB_DATETIME_ConvertUnixTimeToDate(UNIXTIME	unixtime, HxDATETIME_Date_t *date)
{
	UNIXTIME	 ulLocalUnixTime;

	ulLocalUnixTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime(unixtime);

	return hlib_datetime_ConvertUnixTimeToDate(ulLocalUnixTime, date);
}

HERROR HLIB_DATETIME_ConvertUnixTimeToDateWithOutOffset(UNIXTIME	unixtime, HxDATETIME_Date_t *date)
{
	return hlib_datetime_ConvertUnixTimeToDate(unixtime, date);
}

HERROR HLIB_DATETIME_ConvertUnixTimeToTime(UNIXTIME	unixtime, HxDATETIME_Time_t *time)
{
	UNIXTIME	 ulLocalUnixTime;

	ulLocalUnixTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime(unixtime);

	return hlib_datetime_ConvertUnixTimeToTime(ulLocalUnixTime, time);
}

HERROR HLIB_DATETIME_ConvertUnixTimeToTimeWithOutOffset(UNIXTIME	unixtime, HxDATETIME_Time_t *time)
{
	return hlib_datetime_ConvertUnixTimeToTime(unixtime, time);
}

// UNIXTIME이 Offset 재계산 없이 곧바로 Time 등으로 변환되어서는 안된다.
// 단 Duration(간격 시간)을 얻기 위한 경우에는 예외이므로 이 경우에는 따로 만들어 준다.
HERROR HLIB_DATETIME_ConvertDurationToTime (UNIXTIME ulDuration, HxDATETIME_Time_t *time)
{
	return hlib_datetime_ConvertUnixTimeToTime (ulDuration, time) ;
}

HERROR HLIB_DATETIME_ConvertUnixTimeToDateTime(UNIXTIME unixtime, HxDATETIME_t *dateTime)
{
	int					 nLocalUnixTime;
	int					 nOffsetHour, nOffsetMin;
	HERROR				 hErr;

	hErr = hlib_datetime_GetOffsetTime(unixtime, &nOffsetHour, &nOffsetMin);

	if (hErr == ERR_OK)
	{
		nLocalUnixTime = (int)unixtime + nOffsetHour * SECONDS_PER_HOUR + nOffsetMin * SECONDS_PER_MIN;
		return hlib_datetime_ConvertUnixTimeToDateTime(nLocalUnixTime, dateTime);
	}

	return hlib_datetime_ConvertUnixTimeToDateTime(unixtime, dateTime);
}

HERROR	HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOffset(UNIXTIME unixtime, HINT32 nOffsetHour, HINT32 nOffsetMin, HxDATETIME_t *dateTime)
{
	int					 nLocalUnixTime;
	int					 nHour, nMin;
	HERROR				 hErr;

	nLocalUnixTime = (int)unixtime + nOffsetHour * SECONDS_PER_HOUR + nOffsetMin * SECONDS_PER_MIN;

	hErr = hlib_datetime_GetOffsetTime(unixtime, &nHour, &nMin);

	if (hErr != ERR_OK)
	{
		hlib_datetime_ConvertUnixTimeToDateTime(nLocalUnixTime, dateTime);
		return ERR_OK;
	}

	nLocalUnixTime = (int)nLocalUnixTime + nHour * SECONDS_PER_HOUR + nMin * SECONDS_PER_MIN;
	hlib_datetime_ConvertUnixTimeToDateTime(nLocalUnixTime, dateTime);

	return ERR_OK;
}

HERROR HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset (UNIXTIME unixtime, HxDATETIME_t *dateTime)
{
	return hlib_datetime_ConvertUnixTimeToDateTime (unixtime, dateTime) ;
}

HERROR HLIB_DATETIME_ConvertDateTimeToUnixTimeWithOutOffset (HxDATETIME_t *dateTime, UNIXTIME *unixtime)
{
	return hlib_datetime_ConvertDateTimeToUnixTime (dateTime, unixtime) ;
}

HERROR HLIB_DATETIME_ConvertUnixTimeToWeekday(UNIXTIME	unixtime, HUINT8 *weekday)
{
	UNIXTIME	 ulLocalTime;

	ulLocalTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (unixtime);

	return hlib_datetime_ConvertUnixTimeToWeekday (ulLocalTime, weekday) ;
}

HERROR HLIB_DATETIME_GetPrevDay(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalPrevTime;
	HERROR		 hErr;

	if (prevTime == NULL || timeGap == NULL)
	{
		return ERR_FAIL;
	}

	ulLocalBaseTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = hlib_datetime_GetPrevDay (ulLocalBaseTime, &ulLocalPrevTime, timeGap);
	if (hErr == ERR_OK)
	{
		*prevTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalPrevTime);
		*timeGap = baseTime - *prevTime;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_GetNextDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalNextTime;
	HERROR		 hErr;

	if (nextTime == NULL || timeGap == NULL)
	{
		return ERR_FAIL;
	}

	ulLocalBaseTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = hlib_datetime_GetNextDay (ulLocalBaseTime, &ulLocalNextTime, timeGap);
	if (hErr == ERR_OK)
	{
		*nextTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalNextTime);
		*timeGap = *nextTime - baseTime;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_GetPrevWeek(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalPrevTime;
	HERROR		 hErr;

	if (prevTime == NULL || timeGap == NULL)
	{
		return ERR_FAIL;
	}

	ulLocalBaseTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = hlib_datetime_GetPrevWeek (ulLocalBaseTime, &ulLocalPrevTime, timeGap);
	if (hErr == ERR_OK)
	{
		*prevTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalPrevTime);
		*timeGap = baseTime - *prevTime;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_GetNextWeek(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	UNIXTIME	 ulLocalBaseTime, ulLocalNextTime;
	HERROR		 hErr;

	if (nextTime == NULL || timeGap == NULL)
	{
		return ERR_FAIL;
	}

	ulLocalBaseTime = hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (baseTime);

	hErr = hlib_datetime_GetNextWeek (ulLocalBaseTime, &ulLocalNextTime, timeGap);
	if (hErr == ERR_OK)
	{
		*nextTime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (ulLocalNextTime);
		*timeGap = *nextTime - baseTime;
		return ERR_OK;
	}

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_ConvertDateTimeToUnixTime(HxDATETIME_t *dateTime, UNIXTIME *unixtime)
{
	HERROR hErr;

	hErr = hlib_datetime_ConvertDateTimeToUnixTime(dateTime, unixtime);

	if (hErr == ERR_OK)
	{
		*unixtime = hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime(*unixtime);
		return ERR_OK;
	}

	return ERR_FAIL;
}

HBOOL		HLIB_DATETIME_LeapYear( HUINT16 year )
{
	return hlib_datetime_LeapYear(year );
}

HUINT32		HLIB_DATETIME_GetWeekDayIndex(HxDATETIME_Date_t stDate)
{
	return hlib_datetime_GetWeekDayIndex(stDate);
}

HUINT32		HLIB_DATETIME_GetWeekNofYear(HxDATETIME_Date_t stDate)
{
	return hlib_datetime_GetWeekNofYear(stDate);
}

HUINT32		HLIB_DATETIME_GetWeekIndex(HxDATETIME_Date_t stDate)
{
	return hlib_datetime_GetWeekIndex(stDate);
}

HUINT32		HLIB_DATETIME_GetDaysInYear(HxDATETIME_Date_t stDate)
{
	return hlib_datetime_GetDaysInYear(stDate);
}

HUINT8		HLIB_DATETIME_GetLastDayOfMonth(HUINT16 year, HUINT8 month)
{
	return hlib_datetime_GetLastDayOfMonth(year, month);
}

HINT32		HLIB_DATETIME_CompareDate(HxDATETIME_Date_t stDate1, HxDATETIME_Date_t stDate2)
{
	return hlib_datetime_CompareDate(stDate1, stDate2);
}

HINT32		HLIB_DATETIME_CompareTime(HxDATETIME_Time_t stTime1, HxDATETIME_Time_t stTime2)
{
	return hlib_datetime_CompareTime(stTime1, stTime2);
}

HxDATETIME_Date_t	HLIB_DATETIME_GetDateFromWeekAndWeekDay (HUINT16 usYear, HUINT8 ucMonth, HUINT8 ucWeek, HxDATETIME_Weekday_e eWeekday)
{
	return hlib_datetime_GetDateFromWeekAndWeekDay (usYear, ucMonth, ucWeek, eWeekday);
}

HCHAR *		HLIB_DATETIME_UnixTimeToString (UNIXTIME unixtime, HCHAR *buf, HUINT32 n)
{
	static HCHAR	s_timeBuf[26];

	HxDATETIME_t	dt;
	HCHAR			*time;

	if (HLIB_DATETIME_ConvertUnixTimeToDateTime(unixtime, &dt) != ERR_OK)
		return NULL;

	if (buf == NULL || n == 0)
	{
		time = s_timeBuf;
		n    = 26;
	}
	else
	{
		time = buf;
	}

	HxSTD_PrintToStrN(time, n
		// ISO-8601 format: YYYY-MM-DDTHH:mm:ss.sssZ
		, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ"
		, (int)dt.stDate.usYear
		, (int)dt.stDate.ucMonth
		, (int)dt.stDate.ucDay
		, (int)dt.stTime.ucHour
		, (int)dt.stTime.ucMinute
		, (int)dt.stTime.ucSecond
		, (int)dt.stTime.usMillisecond
	);
	return time;
}

HCHAR *		HLIB_DATETIME_UnixTimeToStringWithOutOffset (UNIXTIME unixtime, HCHAR *buf, HUINT32 n)
{
	static HCHAR	s_timeBuf[26];

	HxDATETIME_t	dt;
	HCHAR			*time;

	if (HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(unixtime, &dt) != ERR_OK)
		return NULL;

	if (buf == NULL || n == 0)
	{
		time = s_timeBuf;
		n    = 26;
	}
	else
	{
		time = buf;
	}

	HxSTD_PrintToStrN(time, n
		// ISO-8601 format: YYYY-MM-DDTHH:mm:ss.sssZ
		, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ"
		, (int)dt.stDate.usYear
		, (int)dt.stDate.ucMonth
		, (int)dt.stDate.ucDay
		, (int)dt.stTime.ucHour
		, (int)dt.stTime.ucMinute
		, (int)dt.stTime.ucSecond
		, (int)dt.stTime.usMillisecond
	);
	return time;
}

HERROR HLIB_DATETIME_ParseTzString (HCHAR *szTzString, HxDATETIME_TimeZone_t *pstTimezone)
{
	if ((NULL == szTzString) || ('\0' == szTzString[0]) || (NULL == pstTimezone))
	{
		return ERR_FAIL;
	}

	return hlib_datetime_parseTzString (szTzString, pstTimezone);
}

HERROR HLIB_DATETIME_GetProperTimeOffsetFromTzString (HCHAR *szTzString, UNIXTIME utCurrTime, HINT32 *pnOffsetHour, HINT32 *pnOffsetMin, HBOOL bLocalTime)
{
	HINT32					 nOffsetHour, nOffsetMin;
	HERROR					 hErr;

	if ((NULL == szTzString) || ('\0' == szTzString[0]))
	{
		return ERR_FAIL;
	}

	hErr = hlib_datetime_getProperTimeOffsetFromTzString (szTzString, utCurrTime, &nOffsetHour, &nOffsetMin, bLocalTime);
	if (ERR_OK != hErr)
	{
		return ERR_FAIL;
	}

	if (NULL != pnOffsetHour)							{ *pnOffsetHour = nOffsetHour; }
	if (NULL != pnOffsetMin)							{ *pnOffsetMin = nOffsetMin; }

	return ERR_OK;
}

HERROR	HLIB_DATETIME_GetStringByDateTime(const HxDATETIME_t *pstDateTime, HCHAR *pszResult, HINT32 len)
{
	HxSTD_snprintf(pszResult, len - 1, "%d.%d.%d-%d:%d:%d", pstDateTime->stDate.usYear, pstDateTime->stDate.ucMonth, pstDateTime->stDate.ucDay,
														pstDateTime->stTime.ucHour, pstDateTime->stTime.ucMinute, pstDateTime->stTime.ucSecond);

	return ERR_OK;
}

HERROR	HLIB_DATETIME_GetDateTimeByString(const HCHAR *pszString, HxDATETIME_t *pstDateTime)
{
	HINT32	y, m, d, h, mm, s;

	sscanf(pszString, "%d.%d.%d-%d:%d:%d", &y, &m, &d, &h, &mm, &s);
	pstDateTime->stDate.usYear = y;
	pstDateTime->stDate.ucMonth = m;
	pstDateTime->stDate.ucDay = d;
	pstDateTime->stTime.ucHour = h;
	pstDateTime->stTime.ucMinute = mm;
	pstDateTime->stTime.ucSecond = s;

	return ERR_OK;
}

HERROR HLIB_DATETIME_GetTimeOffsetByCountryIndex (HxCountry_e eCountry, HINT32 *pnHour, HUINT32 *pnMin)
{
	if (pnHour == NULL || pnMin == NULL)
	{
		return ERR_FAIL;
	}

	switch (eCountry)
	{
	// UTC+0:0
	case eCountryID_GBR:
	case eCountryID_IRL:
	case eCountryID_ESP:
	case eCountryID_PRT:
	case eCountryID_MAR:
		*pnHour = 0;
		*pnMin = 0;
		break;

	// UTC+1:0
	case eCountryID_AUT:
	case eCountryID_CMR:
	case eCountryID_HRV:
	case eCountryID_CZE:
	case eCountryID_DNK:
	case eCountryID_FRA:
	case eCountryID_DEU:
	case eCountryID_ITA:
	case eCountryID_HUN:
	case eCountryID_NLD:
	case eCountryID_NOR:
	case eCountryID_POL:
	case eCountryID_SVK:
	case eCountryID_SVN:
	case eCountryID_SWE:
	case eCountryID_CHE:
	case eCountryID_BEL:
	case eCountryID_LUX:
	case eCountryID_DZA:
	case eCountryID_TUN:
		*pnHour = 1;
		*pnMin = 0;
		break;

	// UTC+2:0
	case eCountryID_EST:
	case eCountryID_FIN:
	case eCountryID_GRC:
	case eCountryID_LVA:
	case eCountryID_LTU:
	case eCountryID_TUR:
	case eCountryID_UKR:
	case eCountryID_ROU:
	case eCountryID_EGY:
		*pnHour = 2;
		*pnMin = 0;
		break;

	// UTC+3:0
	case eCountryID_RUS:
	case eCountryID_BHR:
	case eCountryID_IRQ:
	case eCountryID_KWT:
	case eCountryID_SAU:
		*pnHour = 3;
		*pnMin = 0;
		break;

	// UTC+3:30
	case eCountryID_IRN:
		*pnHour = 3;
		*pnMin = 30;
		break;

	// UTC+4:00
	case eCountryID_ARE:
	case eCountryID_OMN:
		*pnHour = 4;
		*pnMin = 0;
		break;

	// UTC+7:0
	case eCountryID_VNM:
		*pnHour = 7;
		*pnMin = 0;
		break;

	// UTC+8:0
	case eCountryID_SGP:
	case eCountryID_AUS:
		*pnHour = 8;
		*pnMin = 0;
		break;

	// UTC+9:0
	case eCountryID_JPN:
	case eCountryID_KOR:
		*pnHour = 9;
		*pnMin = 0;
		break;

	// 미국은 UTC-5 ~ UTC-8 까지 존재하므로 후에 다른 방법으로 구현해야 한다.
	case eCountryID_USA:
		*pnHour = -5;
		*pnMin = 0;
		break;

	// UTC-4:0
	case eCountryID_DOM:
		*pnHour = -4;
		*pnMin = 0;
		break;

	// UTC-5:0
	case eCountryID_PAN:
		*pnHour = -5;
		*pnMin = 0;
		break;

	// UTC-6:0
	case eCountryID_GTM:
	case eCountryID_SLV:
	case eCountryID_HND:
	case eCountryID_NIC:
	case eCountryID_CRI:
		*pnHour = -6;
		*pnMin = 0;
		break;

	default:
		return ERR_FAIL;
	}

	return ERR_OK;
}

// year 와 month 를 이용해 , 마지막 일요일의 주차수를 알아온다.
HERROR	HLIB_DATETIME_GetLastSundayXThFromDate(HxDATETIME_Date_t stDate, HxDATETIME_Date_t *lastSundayDate, HUINT32 *lastSundayXTh)
{
	HUINT32	weekIndex  = 0 ,lastDay	= 1, sundayXTh = 5;

	lastDay = (HUINT32)HLIB_DATETIME_GetLastDayOfMonth(stDate.usYear, stDate.ucMonth);

	stDate.ucDay	=	1;		// 무조건 1일로 설정해서 찾아간다.

	weekIndex = HLIB_DATETIME_GetWeekDayIndex(stDate);

	// 첫주 일요일을 찾는다.
	stDate.ucDay	+= (7-weekIndex) % 7;

	stDate.ucDay += 28;

	if(stDate.ucDay > lastDay)
	{
		stDate.ucDay -= 7;
		sundayXTh -= 1;
	}
	*lastSundayDate = stDate;
	*lastSundayXTh 	= sundayXTh;

	return ERR_OK;
}

// year&month 를 이용해 해당 요일과 , 해당 주차수에 해당하는 HxDATETIME_Date_t 정보를 얻어온다.
HINT32	HLIB_DATETIME_GetWeekDayXThFromDate(	HxDATETIME_Date_t 	stDate,			/* 얻고자 하는 날이 포함된 year&month */
											HUINT32 	ulWeekIndex, 	/* 얻고자 하는 날의 요일(sunday =0) */
											HUINT32 	ulWeeks,		/* 얻고자 하는 날의 주차수 */
											HxDATETIME_Date_t	*pstDate		/* 해당되는 날의 HxDATETIME_Date_t */
										)
{
	HUINT32	weekIndex =	0, dayXTh =	ulWeeks, lastDay = 1;

	stDate.ucDay	=	1;

	weekIndex = HLIB_DATETIME_GetWeekDayIndex(stDate);

	if (dayXTh > 4)		// 4주차보다 큰것은 5주차만 존재하므로.
		dayXTh	=	5;

	// 처음 요일에 해당되는 일을 찾음.
	stDate.ucDay += ((7 + ulWeekIndex) - weekIndex) % 7;

	lastDay = (HUINT32)HLIB_DATETIME_GetLastDayOfMonth(stDate.usYear, stDate.ucMonth);

	stDate.ucDay += (dayXTh -1) * 7;

	if(stDate.ucDay > lastDay)
	{
		stDate.ucDay -= 7;
		dayXTh -= 1;
	}

	*pstDate = stDate;

	if ( dayXTh == ulWeeks )		// 정확히 해당되는 날을 찾음.
		return 0;
	else						// 마지막 주에 해당하는 날을 찾음.
		return 1;
}

HERROR HLIB_DATETIME_MakeTzFileFromKnownTzCode(HUINT32 ulCurrTime, HxCountry_e eCountry, HUINT32 ulRegionId, HCHAR *szTzDstString)
{
	HUINT32					 ulCount, ulListNum = sizeof(s_stTime_TzDsts) / sizeof(HxDATETIME_TzDstInfo_t);
	HBOOL					 bFound = FALSE;

	if (ulRegionId > 0x3f)
	{
		// spec상 나올 수 없는 값으로 region id 사용하지 않는다
		HxLOG_Error("invalid region id(0x%X) !! set to 0 \n", ulRegionId);
		ulRegionId = 0;
	}

	for (ulCount = 0; ulCount < ulListNum; ulCount++)
	{
		HxDATETIME_TzDstInfo_t		 *pstDstInfo = s_stTime_TzDsts + ulCount;

		if (ulRegionId > 0)
		{
			if ((pstDstInfo->eCountry == eCountry) && (pstDstInfo->ucRegionId == (HUINT8)ulRegionId))
			{
				bFound = TRUE;
			}
		}
		else
		{
			if (pstDstInfo->eCountry == eCountry)
			{
				bFound = TRUE;
			}
		}

		if (bFound == TRUE)
		{
			// 2가지 type 으로 만드는데,,,,
			// type 1. std offset 만 들어간 type
			// type 2. std offset & dst offset 이 모두 들어간 type

			// type 1. - DST 추가 offsettime 이  == 0 라면 GMT0 + Local Offset Time 의 정보만 만든다.( 무의미하므로)
			if ( pstDstInfo->nDstOffsetMin <= 0 )
			{
				HxDATETIME_t	stDateTime;

				HxSTD_PrintToStrN (szTzDstString, TZ_NAME_LENGTH, "GMT%c%02d:%02d",
									(pstDstInfo->nStdOffsetMin > 0) ? '-':'+',
									abs(pstDstInfo->nStdOffsetMin / 60), abs(pstDstInfo->nStdOffsetMin % 60) );
				HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset (ulCurrTime, &stDateTime);
				return ERR_OK;
			}
			// type 2.
			else
			{
				HxDATETIME_t	stDateTime;
				HxDATETIME_Date_t		stDate, stResultDate;
				HINT32			dstResult;
				HCHAR			szStartDateStr[16], szEndDateStr[16];

				HxSTD_MemSet(&stDateTime, 	0, sizeof(HxDATETIME_t));
				HxSTD_MemSet(&stDate, 		0, sizeof(HxDATETIME_Date_t));
				HxSTD_MemSet(&stResultDate, 0, sizeof(HxDATETIME_Date_t));

				// start date 를 구하고.
				HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset (ulCurrTime, &stDateTime);
				stDate.usYear	=	stDateTime.stDate.usYear;
				stDate.ucMonth	=	pstDstInfo->DstStartInfo.ucMonth;
				// 아래 함수에서 day 는 셋팅을 안해도 상관없다.
				dstResult = HLIB_DATETIME_GetWeekDayXThFromDate(stDate,
															 pstDstInfo->DstStartInfo.eWeekday,
															 pstDstInfo->DstStartInfo.eWeekth, &stResultDate);
				hlib_datetime_makeDateTzString (&stResultDate, szStartDateStr);		// 얻어온 HxDATETIME_Date_t 를 설정.

				// end date 를 구하고.
				stDate.ucMonth	=	pstDstInfo->DstEndInfo.ucMonth;
				// 아래 함수에서 day 는 셋팅을 안해도 상관없다.
				dstResult = HLIB_DATETIME_GetWeekDayXThFromDate(stDate,
															 pstDstInfo->DstEndInfo.eWeekday,
															 pstDstInfo->DstEndInfo.eWeekth, &stResultDate);
				hlib_datetime_makeDateTzString (&stResultDate, szEndDateStr);		// 얻어온 HxDATETIME_Date_t 를 설정.

				HxSTD_PrintToStrN (szTzDstString, TZ_NAME_LENGTH, "GMT%c%02d:%02dDST%c%02d:%02d,%s/%d:%02d,%s/%d:%02d",
									(pstDstInfo->nStdOffsetMin > 0) ? '-':'+',
									abs(pstDstInfo->nStdOffsetMin / 60), abs(pstDstInfo->nStdOffsetMin % 60),
									(pstDstInfo->nDstOffsetMin > 0) ? '-':'+',
									abs(pstDstInfo->nDstOffsetMin / 60), abs(pstDstInfo->nDstOffsetMin % 60),
									szStartDateStr,
									pstDstInfo->DstStartInfo.ucHour, pstDstInfo->DstStartInfo.ucMinute,
									szEndDateStr,
									pstDstInfo->DstEndInfo.ucHour, pstDstInfo->DstEndInfo.ucMinute);
				return ERR_OK;
			}
		}
	}// end for

	return ERR_FAIL;
}

HERROR HLIB_DATETIME_MakeTzFileFromTot(UNIXTIME ulCurrTime, HxCountry_e country, UNIXTIME ulTimeOfChange,
												HINT32 nLocalTimeOffset, HINT32 nNextLocalTimeOffset, HCHAR *szTzDstString)
{
	HERROR hErr	= ERR_FAIL;

	// TOT를 받았으면 prefixed table에서 찾지말고 TOT 내용으로 그대로 TZ string을 만들자
	hErr = hlib_datetime_makeTotDstTzStringFromKnownTzCode (ulCurrTime, country, ulTimeOfChange, nLocalTimeOffset, nNextLocalTimeOffset, szTzDstString);
	if (ERR_OK != hErr)
	{
		hErr = hlib_datetime_presumeTotDstTzString (ulCurrTime, ulTimeOfChange, nLocalTimeOffset, nNextLocalTimeOffset, szTzDstString);
	}

	return hErr;
}

#if defined(CONFIG_MW_SI_LOCALTIME) && !defined(CONFIG_MW_SI_TIMEZONEDB_BYTOT)
HERROR HLIB_DATETIME_GetLocalTimeZoneInfoFile(HxCountry_e eCountry, HUINT8 ucRegionId, HCHAR **szFilename)
{
	HxDATETIME_LTZoneInfo_t		*zoneInfo;
	HINT32						 index = 0;
	HERROR 						 hErr = ERR_FAIL;

	*szFilename = NULL;

	while(s_stLocalTime_ZoneInfo[index].eCountry != eCountryID_NONE)
	{
		zoneInfo = &s_stLocalTime_ZoneInfo[index];

		if((zoneInfo->eCountry == eCountry) && (zoneInfo->ucRegionId == ucRegionId))
		{
			hErr = ERR_OK;
			*szFilename = zoneInfo->szZoneInfoFile;
			break;
		}

		index++;
	}

	return(hErr);
}
#endif


/* *************************************************************************************************** */
#define ___Private_FUNCTION______________________________________________________________________________
/* *************************************************************************************************** */
static HERROR hlib_datetime_GetOffsetTimeAtSpecificTime(UNIXTIME utTime, HINT32 *pnHour, HINT32 *pnMin, HBOOL bLocalTime)
{
	int 				 nOffsetHour = 0, nOffsetMin = 0;
	HCHAR				szTzString[128];
	FILE				*pfTzFile;
	HERROR				 hErr;

	if (pnHour == NULL || pnMin == NULL)
	{
		return ERR_FAIL;
	}

	pfTzFile = fopen (TZ_NAME_PATH_TMP, "rt");
	if (NULL == pfTzFile)							{ return ERR_FAIL; }

	fgets (szTzString, 127, pfTzFile);
	fclose (pfTzFile);

#if 0
	if(HxSTD_StrCmp( s_szTzString, szTzString) == 0)		// TZ 변경이 없을경우 기존 Offset 사용.
	{
		*pnHour	= s_nOffsetHour;
		*pnMin	= s_nOffsetMin;

		return ERR_OK;
	}
#endif

	hErr = HLIB_DATETIME_GetProperTimeOffsetFromTzString (szTzString, utTime, &nOffsetHour, &nOffsetMin, bLocalTime);
	if (ERR_OK == hErr)
	{
		*pnHour	= nOffsetHour;
		*pnMin	= nOffsetMin;

		s_nOffsetMin	= *pnMin;
		s_nOffsetHour	= *pnHour;
		HxSTD_MemSet(s_szTzString, 0, TZ_NAME_LENGTH);
		HxSTD_MemCopy(s_szTzString, szTzString, TZ_NAME_LENGTH -1 );

		return ERR_OK;
	}

	return ERR_FAIL;
}


static HBOOL hlib_datetime_IsTimeDurationOverNHours(UNIXTIME startTime, UNIXTIME endTime, HUINT8 nHours)
{
	UNIXTIME	nHoursSec ;

	nHoursSec = ((UNIXTIME)nHours * SECONDS_PER_HOUR) ;

	if ( (endTime - startTime) > nHoursSec )
	{
		return TRUE ;
	}

	return FALSE ;
}

static HBOOL hlib_datetime_IsTimeOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	if ( endTime2 <= startTime1 || endTime1 <= startTime2 )
	{
		return FALSE ;
	}
	return TRUE ;
}

static HBOOL hlib_datetime_IsTimeOverwrappedWithin24Hours(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
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

static HBOOL hlib_datetime_IsTimeOverwrappedWithRepeatWithin24Hours(UNIXTIME startTime1, UNIXTIME endTime1, HUINT32 Repeat1, UNIXTIME startTime2, UNIXTIME endTime2, HUINT32 Repeat2)
{
	UNIXTIME	subtractor;
	HBOOL		leadingOne ;
	HUINT8		SourceWeekDay1, SourceWeekDay2, DestWeekDay1, DestWeekDay2;
	HUINT8		SourceEndWeekDay1, SourceEndWeekDay2, DestEndWeekDay1, DestEndWeekDay2;
	HINT32		GapWeekDay1, GapWeekDay2, GapDestWeekDay1, GapDestWeekDay2;
	HUINT32		ulRepeatTemp;

	subtractor = (startTime1 <= startTime2 ? startTime1 : startTime2) ;
	leadingOne = (startTime1 <= startTime2 ? TRUE : FALSE) ;

	if(endTime1 != 0)	// endtime과 starttime이 같은 경우는 confilct이 아니기 때문에 1초를 빼고 계산해야 한다.
		endTime1 -= 1;
	if(endTime2 != 0)
		endTime2 -= 1;

	HLIB_DATETIME_ConvertUnixTimeToWeekday(startTime1, &SourceWeekDay1);
	HLIB_DATETIME_ConvertUnixTimeToWeekday(startTime2, &SourceWeekDay2);
	HLIB_DATETIME_ConvertUnixTimeToWeekday(endTime1, &SourceEndWeekDay1);
	HLIB_DATETIME_ConvertUnixTimeToWeekday(endTime2, &SourceEndWeekDay2);

	if( Repeat1 == 0 )
		Repeat1 |= (0x1 << SourceWeekDay1);
	if( Repeat2 == 0 )
		Repeat2 |= (0x1 << SourceWeekDay2);

	startTime1 -= subtractor ;
	endTime1 -= subtractor ;
	startTime2 -= subtractor ;
	endTime2 -= subtractor ;

	HLIB_DATETIME_ConvertUnixTimeToWeekday(startTime1, &DestWeekDay1);
	HLIB_DATETIME_ConvertUnixTimeToWeekday(startTime2, &DestWeekDay2);
	HLIB_DATETIME_ConvertUnixTimeToWeekday(endTime1, &DestEndWeekDay1);
	HLIB_DATETIME_ConvertUnixTimeToWeekday(endTime2, &DestEndWeekDay2);

	if( startTime1 > SECONDS_PER_DAY)
		startTime1 %= SECONDS_PER_DAY ;
	if( endTime1 > SECONDS_PER_DAY)
		endTime1 %= SECONDS_PER_DAY;
	if( startTime2 > SECONDS_PER_DAY)
		startTime2 %= SECONDS_PER_DAY ;
	if( endTime2 > SECONDS_PER_DAY)
		endTime2 %= SECONDS_PER_DAY;

	GapWeekDay1 = SourceWeekDay1 - DestWeekDay1;
	GapWeekDay2 = SourceWeekDay2 - DestWeekDay2;
	GapDestWeekDay1 = SourceEndWeekDay1 - DestEndWeekDay1;
	GapDestWeekDay2 = SourceEndWeekDay2 - DestEndWeekDay2;

	if ( leadingOne )
	{
		if ( (startTime2 <= endTime2) && (endTime1 <= startTime2) )
		{
			return FALSE ;
		}

		if( !(Repeat1&Repeat2) )
		{
			if( GapWeekDay1 != GapWeekDay2 )
			{
				ulRepeatTemp = Repeat2 & 0x01;
				Repeat2 = (Repeat2>>1) | (ulRepeatTemp<<WEEKDAY_SATURDAY);
				if( (Repeat1&Repeat2) && (startTime2 <= endTime1) )
				{
					return TRUE;
				}
			}
			if( SourceWeekDay2 != SourceEndWeekDay2)
			{
				ulRepeatTemp = (Repeat2 & 0x70) ? 1: 0;
				Repeat2 = (Repeat2<<1) | (ulRepeatTemp);
				if( (Repeat1&Repeat2) && (GapDestWeekDay1 == GapDestWeekDay2) )
				{
					return TRUE;
				}
			}

			return FALSE;
		}
	}
	else
	{
		if ( (startTime1 <= endTime1) && (endTime2 <= startTime1) )
		{
			return FALSE ;
		}

		if( !(Repeat1&Repeat2) )
		{
			if( GapWeekDay1 != GapWeekDay2 )
			{
				ulRepeatTemp = Repeat1 & 0x01;
				Repeat1 = (Repeat1>>1) | (ulRepeatTemp<<WEEKDAY_SATURDAY);
				if( (Repeat1&Repeat2) && (startTime1 <= startTime2) && (startTime1 <= endTime2) )
				{
					return TRUE;
				}
			}
			if( SourceWeekDay1 != SourceEndWeekDay1)
			{
				ulRepeatTemp = (Repeat1 & 0x70) ? 1: 0;
				Repeat1 = (Repeat1<<1) | (ulRepeatTemp);
				if( (Repeat1&Repeat2) && (GapDestWeekDay1 == GapDestWeekDay2) )
				{
					return TRUE;
				}
			}
			return FALSE;
		}
	}

	return TRUE ;
}

static HBOOL hlib_datetime_IsTimeOverwrappedWithinWeek(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
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

static HBOOL hlib_datetime_IsWeekdayOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
{
	HUINT8	weekday11, weekday12, weekday21, weekday22 ;

	hlib_datetime_ConvertUnixTimeToWeekday(startTime1, &weekday11) ;
	hlib_datetime_ConvertUnixTimeToWeekday(endTime1, &weekday12) ;
	hlib_datetime_ConvertUnixTimeToWeekday(startTime2, &weekday21) ;
	hlib_datetime_ConvertUnixTimeToWeekday(endTime2, &weekday22) ;

	if ( weekday11 > weekday22 || weekday12 < weekday21 )
	{
		return FALSE ;
	}
	return TRUE ;
}

static HBOOL hlib_datetime_IsDateOverwrapped(UNIXTIME startTime1, UNIXTIME endTime1, UNIXTIME startTime2, UNIXTIME endTime2)
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

static HBOOL hlib_datetime_IsWorkDay(UNIXTIME startTime, UNIXTIME endTime)
{
	HUINT8	weekday1, weekday2;

	HLIB_DATETIME_ConvertUnixTimeToWeekday(startTime, &weekday1);
	HLIB_DATETIME_ConvertUnixTimeToWeekday(endTime, &weekday2);

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

static HBOOL hlib_datetime_IsMondayToSaturday(UNIXTIME startTime, UNIXTIME endTime)
{
	HUINT8	weekday1, weekday2;

	HLIB_DATETIME_ConvertUnixTimeToWeekday(startTime, &weekday1);
	HLIB_DATETIME_ConvertUnixTimeToWeekday(endTime, &weekday2);

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

static HBOOL hlib_datetime_IsTuesdayToSaturday(UNIXTIME startTime, UNIXTIME endTime)
{
	HUINT8	weekday1, weekday2;

	HLIB_DATETIME_ConvertUnixTimeToWeekday(startTime, &weekday1);
	HLIB_DATETIME_ConvertUnixTimeToWeekday(endTime, &weekday2);

	if( (weekday1 >= WEEKDAY_TUESDAY && weekday1 <= WEEKDAY_SATURDAY)
		|| (weekday2 >= WEEKDAY_TUESDAY && weekday2 <= WEEKDAY_SATURDAY) )
	{
		return TRUE;
	}

	return FALSE;
}

static HERROR hlib_datetime_ConvertDateToJulian(HxDATETIME_Date_t *date, HINT32 *julian)
{
	if ( !julian )
	{
		return ERR_FAIL ;
	}

	*julian = (date->ucDay - 32076) + 1461L * (date->usYear + 4800L + (date->ucMonth - 14) / 12) / 4 + 367 * (date->ucMonth - 2 - (date->ucMonth - 14) / 12 * 12) / 12 - 3 * ((date->usYear + 4900L + (date->ucMonth - 14) / 12) / 100) / 4 + 1 ;

	return ERR_OK ;
}

static HERROR hlib_datetime_ConvertJulianToDate(HINT32 julian, HxDATETIME_Date_t *date)
{
	HINT32	x, z, m, d, y ;

	if ( !date )
	{
		return ERR_FAIL ;
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

	return ERR_OK ;
}

static HERROR hlib_datetime_ConvertMJDToYMD(HUINT16 mjd, HxDATETIME_Date_t *date)
{
	HUINT32	YY, MM, K ;

	if ( !date )
	{
		return ERR_FAIL ;
	}

	YY = (mjd * 100L - 1507820L) / 36525L ;
	MM = ((mjd * 10000L - 149561000L) - (YY * 36525L / 100L) * 10000L) / 306001L ;
	K = ((MM == 14) || (MM == 15) ? 1 : 0 ) ;

	date->usYear = (HUINT16)(YY + K + 1900) ;
	date->ucMonth = (HUINT8)(MM - 1 - K * 12) ;
	date->ucDay = (HUINT8)(mjd - 14956L - (YY * 36525L / 100L) - (MM * 306001L / 10000L)) ;

	return ERR_OK ;
}

static HERROR hlib_datetime_ConvertYMDToMJD(HxDATETIME_Date_t *date, HUINT16 *mjd)
{
	HUINT32	Y, M, D, L ;
	HUINT32	tmp1, tmp2 ;

	if ( !mjd )
	{
		return ERR_FAIL ;
	}

	Y = (HUINT32)date->usYear - (HUINT32)1900 ;
	M = (HUINT32)date->ucMonth ;
	D = (HUINT32)date->ucDay ;

	L = ((M == 1) || (M == 2) ? 1 : 0) ;

	tmp1 = (Y - L) * 36525L / 100L ;
	tmp2 = (M + 1 + L * 12L) * 306001L / 10000L ;

	*mjd = (HUINT16)(14956 + D + tmp1 + tmp2) ;

	return ERR_OK ;
}

static HERROR hlib_datetime_GetWeekDayFromMJD(HUINT16 mjd, HUINT16 *weekday)
{
	if ( !weekday )
	{
		return ERR_FAIL ;
	}

	*weekday = (((mjd + 2) % 7) + 1) ;

	return ERR_OK ;
}

static HERROR hlib_datetime_GetWeekdayFromDate(HxDATETIME_Date_t *date, HUINT16 *weekday)
{
	HUINT16	mjd ;

	if ( !weekday )
	{
		return ERR_FAIL ;
	}

	hlib_datetime_ConvertYMDToMJD(date, &mjd) ;
	return hlib_datetime_GetWeekDayFromMJD(mjd, weekday) ;
}

static HERROR hlib_datetime_ConvertMJDBcdTimeToUnixTime(HUINT8 *mjdBcdTime, UNIXTIME *unixtime)
{
	HERROR		result  = ERR_FAIL;
	HUINT16		mjd ;
	HUINT8*		bcdtime = NULL ;
	HxDATETIME_t	dateTime;

	if ( !mjdBcdTime )
	{
		return ERR_FAIL ;
	}

	if ( !unixtime )
	{
		return ERR_FAIL ;
	}

	mjd = (HUINT16)((mjdBcdTime[0] << 8) + mjdBcdTime[1]) ;
	result = hlib_datetime_ConvertMJDToDate(mjd, &(dateTime.stDate)) ;
	if (result == ERR_OK)
	{
		// 변환 후 UNIXTIME의 범위를 넘을 값을 미리 배제한다.
		if (dateTime.stDate.usYear >= 2038 || dateTime.stDate.usYear <= 1980)
		{
			return ERR_FAIL;
		}

		bcdtime = &(mjdBcdTime[2]) ;
		result = hlib_datetime_ConvertBcdTimeToDecimalTime(bcdtime, 3, &(dateTime.stTime)) ;
		if (result == ERR_OK)
		{
			return hlib_datetime_ConvertDateTimeToUnixTime(&dateTime, unixtime) ;
		}
	}

	return result;
}

static HERROR hlib_datetime_ConvertUnixTimeToMJDBcdTime(UNIXTIME unixtime, HUINT8 *mjdUtc)
{
	HERROR		result = ERR_FAIL;
	HxDATETIME_t	dateTime;
	HUINT16		mjd;
	HUINT8		bcdTime[3];

	if ( !mjdUtc )
	{
		return ERR_FAIL ;
	}

	hlib_datetime_ConvertUnixTimeToDateTime(unixtime, &dateTime);
	result = hlib_datetime_ConvertDateToMJD(&(dateTime.stDate), &mjd);
	if (result == ERR_OK)
	{
		result = hlib_datetime_ConvertDecimalTimeToBcdTime(&(dateTime.stTime), bcdTime);
		if (result == ERR_OK)
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

static HERROR hlib_datetime_ConvertUnixTimeToMJD(UNIXTIME unixtime, HUINT16 *mjd)
{
	HxDATETIME_Date_t		date ;

	if ( !mjd )
	{
		return ERR_FAIL ;
	}

	hlib_datetime_ConvertUnixTimeToDate(unixtime, &date);
	return hlib_datetime_ConvertDateToMJD(&date, mjd);
}

static HERROR hlib_datetime_ConvertMJDToDate(HUINT16 mjd, HxDATETIME_Date_t *date)
{
	HINT32	YY, MM, K;

	if ( !date )
	{
		return ERR_FAIL ;
	}

	// 소수점 계산 때문 ==> 모든 계산은 10배 또는 10000배 후에 계산
	YY	= (mjd*100L-1507820L) / 36525L;
	MM	= ((mjd*10000L-149561000L) - (YY*36525L/100L)*10000L)/306001L;

	date->ucDay = (HUINT8)(mjd-14956L-(YY*36525L/100L)-(MM*306001L/10000L));

	K = ( (MM == 14) || (MM == 15) ? 1 : 0 ) ;

	date->usYear = (HUINT16)(YY + K + 1900);
	date->ucMonth = (HUINT8)(MM - 1 - K*12);

	return ERR_OK ;
}

static HERROR hlib_datetime_ConvertDateToMJD(HxDATETIME_Date_t *date, HUINT16 *mjd)
{
	HUINT32	Y, M, D, L;
	HUINT32	tmp1, tmp2;

	if ( !date )
	{
		return ERR_FAIL ;
	}

	if ( !mjd )
	{
		return ERR_FAIL ;
	}

	Y = (HUINT32)(date->usYear - 1900);
	M = (HUINT32)date->ucMonth;
	D = (HUINT32)date->ucDay;

	L = ( (M == 1) || (M == 2) ? 1 : 0 ) ;

	tmp1 = ( Y - L ) * 36525L / 100L;
	tmp2 = ( M + 1L + L * 12L ) * 306001L / 10000L;

	*mjd = (HUINT16)(14956 + D + tmp1 + tmp2);

	return ERR_OK ;
}

static HERROR hlib_datetime_ConvertBcdTimeToDecimalTime(HUINT8 *bcdTime, HINT32 nLength, HxDATETIME_Time_t *pTime)
{
	HUINT8	dataU8;
	HUINT8	returnValue;
	HINT32		nIndex;

	if ( !bcdTime )
	{
		return ERR_FAIL ;
	}

	if ( !pTime )
	{
		return ERR_FAIL ;
	}
	HxSTD_MemSet (pTime, 0x00, sizeof (HxDATETIME_Time_t));

	nIndex = 0;
	if (nLength <= nIndex)
	{
		return ERR_FAIL;
	}
	dataU8 = bcdTime[nIndex];
	returnValue = (HUINT8)(((dataU8>>4) & 0x0f) * 10 + (dataU8 & 0x0f));
	if (returnValue <= 24)
	{
		pTime->ucHour = returnValue;

		nIndex++;
		if (nLength <= nIndex)
		{
			return ERR_OK;
		}

		dataU8 = bcdTime[nIndex];
		returnValue = (HUINT8)(((dataU8>>4) & 0x0f) * 10 + (dataU8 & 0x0f));
		if (returnValue <= 60)
		{
			pTime->ucMinute = returnValue;

			nIndex++;
			if (nLength <= nIndex)
			{
				return ERR_OK;
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

	return ERR_OK;
}


static HERROR hlib_datetime_ConvertDecimalTimeToBcdTime(HxDATETIME_Time_t *time, HUINT8 *bcdTime)
{
	if ( !time )
	{
		return ERR_FAIL ;
	}

	if ( !bcdTime )
	{
		return ERR_FAIL ;
	}

	bcdTime[0] = (HUINT8)((((time->ucHour / 10) << 4) & 0xf0) | (time->ucHour % 10));
	bcdTime[1] = (HUINT8)((((time->ucMinute / 10) << 4) & 0xf0) | (time->ucMinute % 10));
	bcdTime[2] = (HUINT8)((((time->ucSecond / 10) << 4) & 0xf0) | (time->ucSecond % 10));

	return ERR_OK ;
}


static HERROR hlib_datetime_ConvertBcdTimeToUnixTime (HUINT8 *bcdTime, UNIXTIME *unixtime)
{
	UNIXTIME	sysTime ;
	HERROR		result = ERR_FAIL;
	HUINT8		dataU8;
	HUINT8		returnValue;

	if ( !bcdTime )
	{
		return ERR_FAIL ;
	}

	if ( !unixtime )
	{
		return ERR_FAIL ;
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
			result = ERR_OK;
			sysTime += returnValue;
		}
	}

	if ( result == ERR_OK )
	{
		*unixtime = sysTime ;
	}

	return result;
}

HERROR hlib_datetime_GetOffsetTime(UNIXTIME unixtime, HINT32 *pnHour, HINT32 *pnMin)
{
	int 				 nOffsetHour = 0, nOffsetMin = 0;
	HCHAR				szTzString[128];
	FILE				*pfTzFile;
	HERROR				 hErr;

	if (pnHour == NULL || pnMin == NULL)
	{
		return ERR_FAIL;
	}

	pfTzFile = fopen (TZ_NAME_PATH_TMP, "rt");
	if (NULL == pfTzFile)							{ return ERR_FAIL; }

	fgets (szTzString, 127, pfTzFile);
	fclose (pfTzFile);

#if 0
	if(HxSTD_StrCmp( s_szTzString, szTzString) == 0)		// TZ 변경이 없을경우 기존 Offset 사용.
	{
		*pnHour	= s_nOffsetHour;
		*pnMin	= s_nOffsetMin;

		return ERR_OK;
	}
#endif

	if (unixtime < VALID_UNIXTIME)
	{
		unixtime	= 0;
	}

	hErr = HLIB_DATETIME_GetProperTimeOffsetFromTzString (szTzString, unixtime, &nOffsetHour, &nOffsetMin, FALSE);
	if (ERR_OK == hErr)
	{
		*pnHour	= nOffsetHour;
		*pnMin	= nOffsetMin;

		s_nOffsetMin	= *pnMin;
		s_nOffsetHour	= *pnHour;
		HxSTD_MemSet(s_szTzString, 0, TZ_NAME_LENGTH);
		HxSTD_MemCopy(s_szTzString, szTzString, TZ_NAME_LENGTH -1 );

		return ERR_OK;
	}

	return ERR_FAIL;
}

static HERROR hlib_datetime_ConvertUnixTimeToDateTime(UNIXTIME	unixtime, HxDATETIME_t *dateTime)
{
	long jd;
	long hms;

	if ( !dateTime )
	{
		return ERR_FAIL ;
	}

	jd = unixtime / SECONDS_PER_DAY;
	hms = unixtime % SECONDS_PER_DAY;

	hlib_datetime_ConvertJulianToDate(jd + UNIX_FIRST_DAY, &(dateTime->stDate)) ;

	dateTime->stTime.ucHour = (HUINT8)(hms / SECONDS_PER_HOUR);
	hms -= SECONDS_PER_HOUR * dateTime->stTime.ucHour;

	dateTime->stTime.ucMinute = (HUINT8)(hms / SECONDS_PER_MIN);
	hms -= SECONDS_PER_MIN * dateTime->stTime.ucMinute;

	dateTime->stTime.ucSecond = (HUINT8)hms;
	dateTime->stTime.usMillisecond = 0;

	return ERR_OK ;
}

static HERROR hlib_datetime_ConvertDateTimeToUnixTime(HxDATETIME_t *dateTime, UNIXTIME *unixtime)
{
	HERROR		result  = ERR_FAIL;
	HINT32		julian ;

	if ( !dateTime )
	{
		return ERR_FAIL ;
	}

	if ( !unixtime )
	{
		return ERR_FAIL ;
	}

	result = hlib_datetime_ConvertDateToJulian(&(dateTime->stDate), &julian) ;
	*unixtime = (UNIXTIME)julian ;

	if (result != ERR_OK && ((HINT32)(*unixtime - UNIX_FIRST_DAY)) < 0)
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

	return ERR_OK ;
}

static HERROR hlib_datetime_ConvertUnixTimeToDate(UNIXTIME	unixtime, HxDATETIME_Date_t *date)
{
	if ( !date )
	{
		return ERR_FAIL ;
	}

	return hlib_datetime_ConvertJulianToDate((unixtime / SECONDS_PER_DAY) + UNIX_FIRST_DAY, date) ;
}

static HERROR hlib_datetime_ConvertUnixTimeToTime(UNIXTIME	unixtime, HxDATETIME_Time_t *time)
{
	HUINT32	hms;

	if ( !time )
	{
		return ERR_FAIL ;
	}

	hms = unixtime % SECONDS_PER_DAY;

	time->ucHour = (HUINT8)(hms / SECONDS_PER_HOUR);	hms -= SECONDS_PER_HOUR * time->ucHour;
	time->ucMinute = (HUINT8)(hms / SECONDS_PER_MIN);	hms -= SECONDS_PER_MIN * time->ucMinute;
	time->ucSecond = (HUINT8)hms;
	time->usMillisecond = 0;

	return ERR_OK ;
}

static HERROR hlib_datetime_ConvertUnixTimeToWeekday(UNIXTIME	unixtime, HUINT8 *weekday)
{
	if ( !weekday )
	{
		return ERR_FAIL ;
	}

	unixtime /= SECONDS_PER_DAY ;

	/* 1970.1.1은 목요일로 +4일을 더해서 일요일이 0이 되는 값이 나오게 한다. */
	*weekday = (HUINT8)((unixtime + 4) % 7) ;

	return ERR_OK ;
}

static UNIXTIME hlib_datetime_ConvertGmtUnixTimeToLocalUnixTime (UNIXTIME ulGmtTime)
{
	int					 nHour, nMin;
	int					 nLocalUnixTime;
	HERROR				 hErr;

	// 1. Get the offset
	hErr = hlib_datetime_GetOffsetTime(ulGmtTime, &nHour, &nMin);

	if (hErr != ERR_OK)
	{
		return ulGmtTime;
	}

	// 2. Add the offset : 이전 사용되던 TimeConv 함수와는 다르게 TZ offset 이 바로 DST 적용된 값이다.
	nLocalUnixTime = (int)ulGmtTime + nHour * SECONDS_PER_HOUR + nMin * SECONDS_PER_MIN;

	return (UNIXTIME)nLocalUnixTime;
}

static UNIXTIME hlib_datetime_ConvertLocalUnixTimeToGmtUnixTime (UNIXTIME ulLocalUnixTime)
{
	int					 nHour, nMin;
	int					 nGmtTime;
	HERROR				 hErr;

	// 1. Get the offset
	//hErr = HLIB_DATETIME_GetOffsetTime (&nHour, &nMin);
	hErr = hlib_datetime_GetOffsetTimeAtSpecificTime(ulLocalUnixTime, &nHour, &nMin, TRUE);
	if (hErr != ERR_OK)
	{
		return ulLocalUnixTime;
	}

	// 2. Delete the offset. timeconv 와는 다르게 TZ get offset 적용이 된것이므로 dst 적용을 바로 해도 된다.
	nGmtTime = (int)ulLocalUnixTime - nHour * SECONDS_PER_HOUR - nMin * SECONDS_PER_MIN;

	return (UNIXTIME)nGmtTime;
}

static HERROR	hlib_datetime_GetPrevDay(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if(!prevTime || !timeGap)
	{
		return ERR_FAIL;
	}

	hlib_datetime_ConvertUnixTimeToWeekday(baseTime, &weekday);

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
		HxLOG_Assert(0);
		return ERR_FAIL;
	}

	*prevTime = baseTime - *timeGap;

	return ERR_OK;
}

static HERROR	hlib_datetime_GetNextDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if(!nextTime || !timeGap)
	{
		return ERR_FAIL;
	}

	hlib_datetime_ConvertUnixTimeToWeekday(baseTime, &weekday);

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
		HxLOG_Assert(0);
		return ERR_FAIL;
	}

	*nextTime = baseTime + *timeGap;

	return ERR_OK;
}

static HERROR	hlib_datetime_GetPrevWeek(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if(!prevTime || !timeGap)
	{
		return ERR_FAIL;
	}

	hlib_datetime_ConvertUnixTimeToWeekday(baseTime, &weekday);

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
		HxLOG_Assert(0);
		return ERR_FAIL;
	}

	*prevTime = baseTime - *timeGap;

	return ERR_OK;
}

static HERROR	hlib_datetime_GetNextWeek(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if(!nextTime || !timeGap)
	{
		return ERR_FAIL;
	}

	hlib_datetime_ConvertUnixTimeToWeekday(baseTime, &weekday);

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
		HxLOG_Assert(0);
		return ERR_FAIL;
	}

	*nextTime = baseTime + *timeGap;

	return ERR_OK;
}

static HERROR	hlib_datetime_GetPrevWorkDay(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if(!prevTime || !timeGap)
	{
		return ERR_FAIL;
	}

	hlib_datetime_ConvertUnixTimeToWeekday(baseTime, &weekday);

	switch (weekday)
	{
	/* 화요일부터 토요일까지는 +1일을 해준다. */
	case WEEKDAY_TUESDAY :
	case WEEKDAY_WEDNESDAY :
	case WEEKDAY_THURSDAY :
	case WEEKDAY_FRIDAY :
	case WEEKDAY_SATURDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY;
		break;

	/* 월요일은 +3일(금요일)*/
	case WEEKDAY_MONDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY * 3;
		break;

	/* 일요일은 +2일(월요일)*/
	case WEEKDAY_SUNDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY * 2;
		break;

	default :
		HxLOG_Assert(0);
		return ERR_FAIL;
	}

	*prevTime = baseTime - *timeGap;

	return ERR_OK;
}

static HERROR	hlib_datetime_GetNextWorkDay(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if(!nextTime || !timeGap)
	{
		return ERR_FAIL;
	}

	hlib_datetime_ConvertUnixTimeToWeekday(baseTime, &weekday);

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
		HxLOG_Assert(0);
		return ERR_FAIL;
	}

	*nextTime = baseTime + *timeGap;

	return ERR_OK;
}

static HERROR hlib_datetime_GetPrevWithoutSunday(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if (!prevTime || !timeGap)
	{
		return ERR_FAIL;
	}

	hlib_datetime_ConvertUnixTimeToWeekday(baseTime, &weekday);

	switch (weekday)
	{
	/* 화 ~ 일요일은 +1일을 한다. */
	case WEEKDAY_SUNDAY :
	case WEEKDAY_TUESDAY :
	case WEEKDAY_WEDNESDAY :
	case WEEKDAY_THURSDAY :
	case WEEKDAY_FRIDAY :
	case WEEKDAY_SATURDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY;
		break;

	/* 월요일은 +2일을 하여 토요일을 만들어 준다. */
	case WEEKDAY_MONDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY * 2;
		break;

	default :
		HxLOG_Assert(0);
		return ERR_FAIL;
	}

	*prevTime = baseTime - *timeGap;

	return ERR_OK;
}

static HERROR hlib_datetime_GetNextWithoutSunday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if (!nextTime || !timeGap)
	{
		return ERR_FAIL;
	}

	hlib_datetime_ConvertUnixTimeToWeekday(baseTime, &weekday);

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
		HxLOG_Assert(0);
		return ERR_FAIL;
	}

	*nextTime = baseTime + *timeGap;

	return ERR_OK;
}

static HERROR hlib_datetime_GetPrevTuesdayToSaturday(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if (!prevTime || !timeGap)
	{
		return ERR_FAIL;
	}

	hlib_datetime_ConvertUnixTimeToWeekday(baseTime, &weekday);

	switch (weekday)
	{
	/* 수 ~ 일요일은 -1일을 한다 */
	case WEEKDAY_WEDNESDAY :
	case WEEKDAY_THURSDAY :
	case WEEKDAY_FRIDAY :
	case WEEKDAY_SATURDAY :
	case WEEKDAY_SUNDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY;
		break;

	/* 월요일은 -2일을 하여 토요일로 만든다 */
	case WEEKDAY_MONDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY * 2;
		break;

	/* 화요일은 -3일을 하여 토요일로 만든다 */
	case WEEKDAY_TUESDAY:
		*timeGap = (UNIXTIME)SECONDS_PER_DAY * 3;
		break;

	default :
		HxLOG_Error("Error");
		return ERR_FAIL;
	}

	*prevTime = baseTime - *timeGap;

	return ERR_OK;
}

static HERROR hlib_datetime_GetNextTuesdayToSaturday(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if (!nextTime || !timeGap)
	{
		return ERR_FAIL;
	}

	hlib_datetime_ConvertUnixTimeToWeekday(baseTime, &weekday);

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
		return ERR_FAIL;
	}

	*nextTime = baseTime + *timeGap;

	return ERR_OK;
}

static HERROR hlib_datetime_GetPrevWeekend(UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if (!prevTime || !timeGap)
	{
		return ERR_FAIL;
	}

	hlib_datetime_ConvertUnixTimeToWeekday(baseTime, &weekday);

	switch (weekday)
	{
	/* 일요일은 토요일로 만든다 */
	case WEEKDAY_SUNDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY;
		break;

	/* 월 ~ 토요일은 일요일로 만들어야 한다. */
	case WEEKDAY_MONDAY :
	case WEEKDAY_TUESDAY :
	case WEEKDAY_WEDNESDAY :
	case WEEKDAY_THURSDAY :
	case WEEKDAY_FRIDAY :
	case WEEKDAY_SATURDAY :
		*timeGap = (UNIXTIME)SECONDS_PER_DAY * weekday;
		break;

	default :
		HxLOG_Assert(0);
		return ERR_FAIL;
	}

	*prevTime = baseTime - *timeGap;

	return ERR_OK;
}

static HERROR hlib_datetime_GetNextWeekend(UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;

	if (!nextTime || !timeGap)
	{
		return ERR_FAIL;
	}

	hlib_datetime_ConvertUnixTimeToWeekday(baseTime, &weekday);

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
		HxLOG_Assert(0);
		return ERR_FAIL;
	}

	*nextTime = baseTime + *timeGap;

	return ERR_OK;
}

static HERROR hlib_datetime_GetPrevFewDayofWeek(HUINT32 ulDays, UNIXTIME baseTime, UNIXTIME *prevTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;
	HINT32	i;

	if (!prevTime || !timeGap || ulDays == 0)
	{
		return ERR_FAIL;
	}

	hlib_datetime_ConvertUnixTimeToWeekday(baseTime, &weekday);

	*timeGap = 0;
	weekday = (weekday == 0) ? WEEKDAY_SATURDAY : weekday - 1;
	*timeGap += (UNIXTIME)SECONDS_PER_DAY;

	for(i=WEEKDAY_SUNDAY; i<=WEEKDAY_SATURDAY;i++)	// 최대 7바퀴
	{
		if( ulDays & (0x1 << weekday) )	// 이 값이 TRUE 라면 weekday는 예약을 실행 해야 되는 날이다.
			break;
		else
			*timeGap += (UNIXTIME)SECONDS_PER_DAY;

		weekday = (weekday == 0) ? WEEKDAY_SATURDAY : weekday - 1;
	}

	*prevTime = baseTime - *timeGap;

	return ERR_OK;
}

static HERROR hlib_datetime_GetNextFewDayofWeek(HUINT32 ulDays, UNIXTIME baseTime, UNIXTIME *nextTime, UNIXTIME *timeGap)
{
	HUINT8	weekday;
	HINT32	i;

	if (!nextTime || !timeGap || ulDays == 0)
	{
		return ERR_FAIL;
	}

	hlib_datetime_ConvertUnixTimeToWeekday(baseTime, &weekday);

	*timeGap = 0;
	weekday += 1;
	weekday %= (WEEKDAY_SATURDAY+1);
	*timeGap += (UNIXTIME)SECONDS_PER_DAY;

	for(i=WEEKDAY_SUNDAY; i<=WEEKDAY_SATURDAY;i++)	// 최대 7바퀴
	{
		if( ulDays & (0x1 << weekday) )	// 이 값이 TRUE 라면 weekday는 예약을 실행 해야 되는 날이다.
			break;
		else
			*timeGap += (UNIXTIME)SECONDS_PER_DAY;

		weekday += 1;
		weekday %= (WEEKDAY_SATURDAY+1);
	}

	*nextTime = baseTime + *timeGap;

	return ERR_OK;
}

static HBOOL		hlib_datetime_LeapYear( HUINT16 year )
{
	if( (year%4) == 0 &&  ((year) % 100 != 0 ||  (year) % 400 ==0) )
	{
		return TRUE;
	}

	return FALSE;
}

/* AD(anno domini) 1년 1월 1일을 월요일 기준으로 해당 날짜의 요일을 나타내는 함수. */
static HUINT32		hlib_datetime_GetWeekDayIndex(HxDATETIME_Date_t stDate)
{
	HUINT16			year = 1;	/* 0년도는 제외한다 */
	HUINT32			day = 0;
	HUINT16			*aSumDay = NULL;
	HUINT16			sumDay[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	HUINT16			sumLDay[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

	while( year < stDate.usYear)
	{
		if( hlib_datetime_LeapYear(year) )
		{
			day += 366;
		}
		else
		{
			day += 365;
		}
		year++;
	}

	if( hlib_datetime_LeapYear(stDate.usYear) )
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
static HUINT32		hlib_datetime_GetWeekNofYear (HxDATETIME_Date_t stDate)
{
	HUINT32		day = 0, firstDay;
	HUINT16		*aSumDay = NULL;
	HUINT16		sumDay[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	HUINT16		sumLDay[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };
	HxDATETIME_Date_t		date;

	/* 해당 년도 1월 1일 요일 Index를 얻어 온다. */
	date.usYear = stDate.usYear;
	date.ucMonth = 1;
	date.ucDay = 1;

	firstDay = hlib_datetime_GetWeekDayIndex(date);

	if( hlib_datetime_LeapYear(stDate.usYear) )
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
static HUINT32		hlib_datetime_GetWeekIndex (HxDATETIME_Date_t stDate)
{
	HUINT16			year = 1;	/* 0년도는 제외한다 */
	HUINT32			day = 0;
	HUINT16			*aSumDay = NULL;
	HUINT16			sumDay[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	HUINT16			sumLDay[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

	while( year < stDate.usYear)
	{
		if( hlib_datetime_LeapYear(year) )
		{
			day += 366;
		}
		else
		{
			day += 365;
		}
		year++;
	}

	if( hlib_datetime_LeapYear(stDate.usYear) )
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

static HUINT8	hlib_datetime_GetLastDayOfMonth(HUINT16 year, HUINT8 month)
{
	HUINT8		allDay[]  = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	HUINT8		allLDay[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if( (month < 1) || (month > 12) )
	{
		return 0;
	}

	if( hlib_datetime_LeapYear(year) )
	{
		return allLDay[month-1];
	}
	else
	{
		return allDay[month-1];
	}
}

static HUINT32	hlib_datetime_GetTotalDays(HxDATETIME_Date_t stDate)
{
	HUINT16			year = 1;	/* 0년도는 제외한다 */
	HUINT32			day = 0;
	HUINT16			*aSumDay = NULL;
	HUINT16			sumDay[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	HUINT16			sumLDay[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

	while( year < stDate.usYear)
	{
		if( hlib_datetime_LeapYear(year) )
		{
			day += 366;
		}
		else
		{
			day += 365;
		}
		year++;
	}

	if( hlib_datetime_LeapYear(stDate.usYear) )
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

static HUINT32	hlib_datetime_GetDaysInYear(HxDATETIME_Date_t stDate)
{
	HUINT32			day = 0;
	HUINT16			*aSumDay = NULL;
	HUINT16			sumDay[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
	HUINT16			sumLDay[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };

	if( hlib_datetime_LeapYear(stDate.usYear) )
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

static HINT32	hlib_datetime_CompareDate(HxDATETIME_Date_t stDate1, HxDATETIME_Date_t stDate2)
{
	return (HINT32)(hlib_datetime_GetTotalDays(stDate1) - hlib_datetime_GetTotalDays(stDate2));
}

static HINT32	hlib_datetime_CompareTime(HxDATETIME_Time_t stTime1, HxDATETIME_Time_t stTime2)
{
	HINT32	lhs = ((stTime1.ucHour * 60 * 60)  + (stTime1.ucMinute * 60) + (stTime1.ucSecond)) * 1000 + stTime1.usMillisecond;
	HINT32	rhs = ((stTime2.ucHour * 60 * 60)  + (stTime2.ucMinute * 60) + (stTime2.ucSecond)) * 1000 + stTime2.usMillisecond;
	return lhs - rhs;
}

static HxDATETIME_Date_t	hlib_datetime_GetDateFromWeekAndWeekDay (HUINT16 usYear, HUINT8 ucMonth, HUINT8 ucWeek, HxDATETIME_Weekday_e eWeekday)
{
	HUINT8					 ucLastDay, ucCurrWeek;
	HxDATETIME_Weekday_e				 eCurrWeekday;
	HxDATETIME_Date_t				 stDate;

	stDate.usYear	= usYear;
	stDate.ucMonth	= ucMonth;
	stDate.ucDay	= 1;
	ucLastDay		= hlib_datetime_GetLastDayOfMonth (usYear, ucMonth);

	ucCurrWeek		= 0;
	eCurrWeekday	= hlib_datetime_GetWeekDayIndex (stDate);

	for (stDate.ucDay = 1; stDate.ucDay <= ucLastDay; stDate.ucDay++)
	{
		if ((stDate.ucDay % 7) == 1)			{ ucCurrWeek ++; }

		if ((ucWeek == ucCurrWeek) && (eWeekday == eCurrWeekday))
		{
			return stDate;
		}

		eCurrWeekday = (eCurrWeekday + 1) % 7;
	}

	stDate.ucDay --;
	return stDate;
}

//
//	TZ String 관련 함수들
//
static _tzTokenType_e hlib_datetime_getCharToken (HCHAR ucChar)
{
	switch (ucChar)
	{
	case '+':
		return TZ_TOKENTYPE_PLUS;

	case '-':
		return TZ_TOKENTYPE_MINUS;

	case ':':
		return TZ_TOKENTYPE_COLON;

	case ',':
		return TZ_TOKENTYPE_COMMA;

	case '.':
		return TZ_TOKENTYPE_PERIOD;

	case '/':
		return TZ_TOKENTYPE_SLASH;

	case '\0':
		return TZ_TOKENTYPE_END;

	default:
		break;
	}

	if ((ucChar >= '0') && (ucChar <= '9'))
	{
		return TZ_TOKENTYPE_NUMBER;
	}
	else if (((ucChar >= 'A') && (ucChar <= 'Z')) || ((ucChar >= 'a') && (ucChar <= 'z')))
	{
		return TZ_TOKENTYPE_STRING;
	}

	return TZ_TOKENTYPE_NONE;
}

static HCHAR *hlib_datetime_getNextToken (HCHAR *szString, _tzToken_t *pstToken)
{
	_tzTokenType_e		 eType;

	while ('\0' != *szString)
	{
		eType = hlib_datetime_getCharToken (*szString);

		switch (eType)
		{
		case TZ_TOKENTYPE_STRING:
		{
			HUINT32			 ulStrCount = 0;
			HCHAR			*szTokenString = szString;

			pstToken->eType = TZ_TOKENTYPE_STRING;

			HxSTD_MemSet (pstToken->unVal.szString, 0, 8);
			while ((eType = hlib_datetime_getCharToken (*szTokenString)) == TZ_TOKENTYPE_STRING)
			{
				if (ulStrCount < 7)
				{
					pstToken->unVal.szString[ulStrCount] = *szTokenString;
				}
				ulStrCount ++;
				szTokenString ++;
			}

			return szTokenString;
		}

		case TZ_TOKENTYPE_NUMBER:
		case TZ_TOKENTYPE_PLUS:
		case TZ_TOKENTYPE_MINUS:
		{
			HINT32			 nSign = 1, nValue = 0;
			HCHAR			*szTokenString = szString;

			pstToken->eType = TZ_TOKENTYPE_NUMBER;

			if (TZ_TOKENTYPE_MINUS == eType)
			{
				nSign = -1;
			}
			else if (TZ_TOKENTYPE_NUMBER == eType)
			{
				nValue = (HINT32)(*szTokenString - '0');
			}

			szTokenString ++;
			while ((eType = hlib_datetime_getCharToken (*szTokenString)) == TZ_TOKENTYPE_NUMBER)
			{
				nValue *= 10;
				nValue += (HINT32)(*szTokenString - '0');

				szTokenString ++;
			}

			pstToken->unVal.nNumber = nSign * nValue;
			return szTokenString;
		}

		case TZ_TOKENTYPE_COLON:
		case TZ_TOKENTYPE_COMMA:
		case TZ_TOKENTYPE_PERIOD:
		case TZ_TOKENTYPE_SLASH:
			pstToken->eType = eType;
			return szString + 1;

		default:
			break;
		}

		szString ++;
	}

	pstToken->eType = TZ_TOKENTYPE_END;
	return szString;
}

static HCHAR *hlib_datetime_getHourMiniteFromTzString (HCHAR *szTzString, HBOOL *pbTimeFound, HINT32 *pnHour, HINT32 *pnMin)
{
	HBOOL				 bTimeFound = FALSE;
	HINT32				 nHour = 0, nMin = 0;
	_tzToken_t			 stTzToken;
	HCHAR				*szString, *szNextString, *szResultString;

	szString = hlib_datetime_getNextToken (szTzString, &stTzToken);

	// 여기는 당연히 숫자가 와야 한다: 시간 값
	if (TZ_TOKENTYPE_NUMBER != stTzToken.eType)
	{
		szResultString = NULL;
		goto END_FUNC;
	}

	// 여기에 오는 시간은 일반 Time의 Offset
	nHour = stTzToken.unVal.nNumber;
	bTimeFound = TRUE;

	szNextString = hlib_datetime_getNextToken (szString, &stTzToken);
	if (TZ_TOKENTYPE_COLON == stTzToken.eType)
	{
		szString = szNextString;
		szNextString = hlib_datetime_getNextToken (szString, &stTzToken);
		if (TZ_TOKENTYPE_NUMBER == stTzToken.eType)
		{
			// : 다음에 오는 숫자 : 분 임.
			nMin = stTzToken.unVal.nNumber;
			szResultString = szNextString;
			goto END_FUNC;
		}
		else
		{
			// : 다음에 숫자가 오지 않았다 : 버그
			HxLOG_Error ("String Bug : \n");
			szResultString = NULL;
			goto END_FUNC;
		}
	}
	else
	{
		// : 이외의 토큰이 왔다 : 시간만 온 거니 여기서 처리를 끝낸다.
		szResultString = szNextString;
		goto END_FUNC;
	}

END_FUNC:
	*pbTimeFound	= bTimeFound;
	*pnHour			= nHour;
	*pnMin			= nMin;

	return szResultString;
}

static HERROR hlib_datetime_parseTzString (HCHAR *szTzString, HxDATETIME_TimeZone_t *pstTimezone)
{
	HBOOL				 bTimeFound, bDstStart = FALSE, bDstEnd = FALSE;
	HINT32				 nHour, nMin, nMonth, nWeek, nWeekday;
	HxDATETIME_TimeZone_t		 stTimezone;
	HCHAR				*szNextString;
	_tzParsePhase_e		 ePhase = TZ_PARSE_PHASE_NORMAL_STR;
	_tzToken_t			 stTzToken;

	HxSTD_MemSet (&stTimezone, 0, sizeof(HxDATETIME_TimeZone_t));

	while (1)
	{
		szNextString = hlib_datetime_getNextToken (szTzString, &stTzToken);
		if (TZ_TOKENTYPE_END == stTzToken.eType)
		{
			break;
		}

		switch (ePhase)
		{
		case TZ_PARSE_PHASE_NORMAL_STR:
			// PST 등의 String이 오지 않으면 모든 문법이 잘못되었다고 판단된다.
			if (TZ_TOKENTYPE_STRING != stTzToken.eType)
			{
				return ERR_FAIL;
			}

			HLIB_STD_StrNCpySafe(stTimezone.szTzStdStr, stTzToken.unVal.szString, 8);

			ePhase = TZ_PARSE_PHASE_NORMAL_OFFSET;
			szTzString = szNextString;
			break;

		case TZ_PARSE_PHASE_NORMAL_OFFSET:
			// 여기는 당연히 숫자가 와야 한다.
			if (TZ_TOKENTYPE_NUMBER != stTzToken.eType)
			{
				goto END_FUNC;
			}

			szNextString = hlib_datetime_getHourMiniteFromTzString (szTzString, &bTimeFound, &nHour, &nMin);
			if (NULL == szNextString)
			{
				goto END_FUNC;
			}

			if (TRUE == bTimeFound)
			{
				stTimezone.nStdOffsetHour	= nHour * -1;				// Offset은 항상 역으로 되어 있음.
				stTimezone.nStdOffsetMin	= nMin;
			}

			ePhase = TZ_PARSE_PHASE_DST_STR;
			szTzString = szNextString;
			break;

		case TZ_PARSE_PHASE_DST_STR:
			// PST 등의 String이 오지 않으면 여기서 그냥 끝난 것으로 간주한다.
			if (TZ_TOKENTYPE_STRING != stTzToken.eType)
			{
				goto END_FUNC;
			}

			stTimezone.bDstEnabled = TRUE;
			HxSTD_StrNCpy (stTimezone.szTzDstStr, stTzToken.unVal.szString, 8);
			stTimezone.nDstOffsetHour	= stTimezone.nStdOffsetHour;
			stTimezone.nDstOffsetMin	= stTimezone.nStdOffsetMin;

			ePhase = TZ_PARSE_PHASE_DST_OFFSET;
			szTzString = szNextString;
			break;

		case TZ_PARSE_PHASE_DST_OFFSET:
			// Offset 이 생략될 수 있다. 이 경우에는 , 가 붙을 것이다.
			if (TZ_TOKENTYPE_COMMA == stTzToken.eType)
			{
				ePhase = TZ_PARSE_PHASE_DSTSTART_DAY;
				szTzString = szNextString;
			}
			else if (TZ_TOKENTYPE_NUMBER == stTzToken.eType)
			{
				szNextString = hlib_datetime_getHourMiniteFromTzString (szTzString, &bTimeFound, &nHour, &nMin);
				if (NULL == szNextString)
				{
					goto END_FUNC;
				}

				if (TRUE == bTimeFound)
				{
					stTimezone.nDstOffsetHour	= nHour * -1;				// Offset은 항상 역으로 되어 있음.
					stTimezone.nDstOffsetMin	= nMin;
				}

				// 다음에 , 가 오지 않으면 여기서 parsing 안 되는 걸로 간주한다.
				szNextString = hlib_datetime_getNextToken (szNextString, &stTzToken);
				if (TZ_TOKENTYPE_COMMA == stTzToken.eType)
				{
					ePhase = TZ_PARSE_PHASE_DSTSTART_DAY;
					szTzString = szNextString;
				}
				else
				{
					goto END_FUNC;
				}
			}
			else
			{
				goto END_FUNC;
			}

			break;

		case TZ_PARSE_PHASE_DSTSTART_DAY:
			szNextString = hlib_datetime_getMonthWeekDayFromTzString (szTzString, &bTimeFound, &nMonth, &nWeek, &nWeekday);
			if (NULL == szNextString)
			{
				goto END_FUNC;
			}

			if (TRUE == bTimeFound)
			{
				bDstStart = TRUE;
				stTimezone.bDstRuleEnabled			= TRUE;
				stTimezone.stDstStart.ucMonth		= (HUINT8)nMonth;
				stTimezone.stDstStart.eWeekth		= (nWeek > 0) ? (HxDATETIME_Weeks_e)nWeek : eWeeks_1TH;	// Week는 1주차로 시작한다.
				stTimezone.stDstStart.eWeekday		= (HxDATETIME_Weekday_e)nWeekday;
			}

			szNextString = hlib_datetime_getNextToken (szNextString, &stTzToken);
			if (TZ_TOKENTYPE_COMMA == stTzToken.eType)
			{
				ePhase = TZ_PARSE_PHASE_DSTEND_DAY;
				szTzString = szNextString;
			}
			else if (TZ_TOKENTYPE_SLASH == stTzToken.eType)
			{
				ePhase = TZ_PARSE_PHASE_DSTSTART_TIME;
				szTzString = szNextString;
			}
			else
			{
				goto END_FUNC;
			}

			break;

		case TZ_PARSE_PHASE_DSTSTART_TIME:
			// 여기는 당연히 숫자가 와야 한다.
			if (TZ_TOKENTYPE_NUMBER != stTzToken.eType)
			{
				goto END_FUNC;
			}

			szNextString = hlib_datetime_getHourMiniteFromTzString (szTzString, &bTimeFound, &nHour, &nMin);
			if (NULL == szNextString)
			{
				goto END_FUNC;
			}

			if (TRUE == bTimeFound)
			{
				stTimezone.stDstStart.ucHour	= (HUINT8)nHour;
				stTimezone.stDstStart.ucMinute	= (HUINT8)nMin;
			}

			szNextString = hlib_datetime_getNextToken (szNextString, &stTzToken);
			if (TZ_TOKENTYPE_COMMA == stTzToken.eType)
			{
				ePhase = TZ_PARSE_PHASE_DSTEND_DAY;
				szTzString = szNextString;
			}
			else
			{
				goto END_FUNC;
			}

			break;

		case TZ_PARSE_PHASE_DSTEND_DAY:
			szNextString = hlib_datetime_getMonthWeekDayFromTzString (szTzString, &bTimeFound, &nMonth, &nWeek, &nWeekday);
			if (NULL == szNextString)
			{
				goto END_FUNC;
			}

			if (TRUE == bTimeFound)
			{
				bDstEnd = TRUE;
				stTimezone.stDstEnd.ucMonth			= (HUINT8)nMonth;
				stTimezone.stDstEnd.eWeekth			= (nWeek > 0) ? (HxDATETIME_Weeks_e)nWeek : eWeeks_1TH;	// Week는1주차로 시작한다.
				stTimezone.stDstEnd.eWeekday		= (HxDATETIME_Weekday_e)nWeekday;
			}

			szNextString = hlib_datetime_getNextToken (szNextString, &stTzToken);
			if (TZ_TOKENTYPE_SLASH == stTzToken.eType)
			{
				ePhase = TZ_PARSE_PHASE_DSTEND_TIME;
				szTzString = szNextString;
			}
			else
			{
				goto END_FUNC;
			}

			break;

		case TZ_PARSE_PHASE_DSTEND_TIME:
			// 여기는 당연히 숫자가 와야 한다.
			if (TZ_TOKENTYPE_NUMBER != stTzToken.eType)
			{
				goto END_FUNC;
			}

			szNextString = hlib_datetime_getHourMiniteFromTzString (szTzString, &bTimeFound, &nHour, &nMin);
			if (NULL == szNextString)
			{
				goto END_FUNC;
			}

			if (TRUE == bTimeFound)
			{
				stTimezone.stDstEnd.ucHour	= (HUINT8)nHour;
				stTimezone.stDstEnd.ucMinute	= (HUINT8)nMin;
			}

			// Parsing이 끝났으므로 나간다.
			goto END_FUNC;

		default:
			HxLOG_Error ("[%s:%d] !!!!! Something wrong !!!!!!\n");
			goto END_FUNC;
		}
	}

END_FUNC:
	if ((TRUE == bDstStart) && (TRUE != bDstEnd))
	{
		stTimezone.stDstEnd.ucMonth		= 12;
		stTimezone.stDstEnd.eWeekth		= eWeeks_2TH;
		stTimezone.stDstEnd.eWeekday	= eWeekday_Sat;
		stTimezone.stDstEnd.ucHour		= 23;
		stTimezone.stDstEnd.ucMinute	= 59;
	}

	HxSTD_MemCopy (pstTimezone, &stTimezone, sizeof(HxDATETIME_TimeZone_t));
	return ERR_OK;
}

static HERROR hlib_datetime_getProperTimeOffsetFromTzString (HCHAR *szTzString, UNIXTIME utCurrTime, HINT32 *pnOffsetHour, HINT32 *pnOffsetMin, HBOOL bLocalTime)
{
	UNIXTIME				 utDstStartTime, utDstEndTime;
	HxDATETIME_Date_t				 stCurrDate;
	HxDATETIME_t			 stDstStartDateTime, stDstEndDateTime;
	HxDATETIME_TimeZone_t			 stTimezone;
	HERROR					 hErr;

	hErr = hlib_datetime_parseTzString (szTzString, &stTimezone);
	if (ERR_OK != hErr)
	{
		return ERR_FAIL;
	}

	*pnOffsetHour 		= stTimezone.nStdOffsetHour;
	*pnOffsetMin		= stTimezone.nStdOffsetMin;

	if ((TRUE == stTimezone.bDstRuleEnabled) && (0 != utCurrTime))
	{
		hlib_datetime_ConvertUnixTimeToDate (utCurrTime, &stCurrDate);

		stDstStartDateTime.stDate = hlib_datetime_GetDateFromWeekAndWeekDay (stCurrDate.usYear, stTimezone.stDstStart.ucMonth, stTimezone.stDstStart.eWeekth, stTimezone.stDstStart.eWeekday);
		stDstStartDateTime.stTime.ucHour	= stTimezone.stDstStart.ucHour;
		stDstStartDateTime.stTime.ucMinute	= stTimezone.stDstStart.ucMinute;

		stDstEndDateTime.stDate = hlib_datetime_GetDateFromWeekAndWeekDay (stCurrDate.usYear, stTimezone.stDstEnd.ucMonth, stTimezone.stDstEnd.eWeekth, stTimezone.stDstEnd.eWeekday);
		stDstEndDateTime.stTime.ucHour		= stTimezone.stDstEnd.ucHour;
		stDstEndDateTime.stTime.ucMinute	= stTimezone.stDstEnd.ucMinute;

		hlib_datetime_ConvertDateTimeToUnixTime (&stDstStartDateTime, &utDstStartTime);
		hlib_datetime_ConvertDateTimeToUnixTime (&stDstEndDateTime, &utDstEndTime);

		// DST Start Time이 End Time보다 크면, 이는 역전으로 계산 해야 한다
		// 10월 ~ 2월, STD 2, DST 3 --> 2월 ~ 10월 ,  STD 3, DST 2
		if (utDstStartTime > utDstEndTime)
		{
			HINT32				 nSwapValue;
			UNIXTIME			 utSwapTime;

			*pnOffsetHour		= stTimezone.nDstOffsetHour;
			*pnOffsetMin		= stTimezone.nDstOffsetMin;

			utSwapTime = utDstStartTime;
			utDstStartTime = utDstEndTime;
			utDstEndTime = utSwapTime;

			nSwapValue = stTimezone.nStdOffsetHour;
			stTimezone.nStdOffsetHour = stTimezone.nDstOffsetHour;
			stTimezone.nDstOffsetHour = nSwapValue;

			nSwapValue = stTimezone.nStdOffsetMin;
			stTimezone.nStdOffsetMin = stTimezone.nDstOffsetMin;
			stTimezone.nDstOffsetMin = nSwapValue;
		}

		// TZ String에 적힌 값은 Local Time이므로 utDstStartTime은 StdOffset, utDstEndTime은 DstOffset 만큼 빼야 정확하다.
		if(bLocalTime == FALSE)
		{
			if (utDstStartTime > 0) 				{ utDstStartTime -= (stTimezone.nStdOffsetHour * 3600 + stTimezone.nStdOffsetMin * 60); }
			if (utDstEndTime > 0)					{ utDstEndTime	 -= (stTimezone.nDstOffsetHour * 3600 + stTimezone.nDstOffsetMin * 60); }
		}

		if ((utCurrTime >= utDstStartTime) && (utCurrTime <= utDstEndTime))
		{
			*pnOffsetHour		= stTimezone.nDstOffsetHour;
			*pnOffsetMin		= stTimezone.nDstOffsetMin;
		}
	}

	return ERR_OK;
}



static HCHAR *hlib_datetime_getMonthWeekDayFromTzString (HCHAR *szTzString, HBOOL *pbTimeFound, HINT32 *pnMonth, HINT32 *pnWeek, HINT32 *pnWeekday)
{
	HBOOL				 bTimeFound = FALSE;
	HINT32				 nMonth = 0, nWeek = 0, nWeekday = 0;
	_tzToken_t			 stTzToken;
	HCHAR				*szString, *szResultString;

	szString = hlib_datetime_getNextToken (szTzString, &stTzToken);

	// 여기는 M?.?.? 포맷을 맞춰야 한다.
	if ((TZ_TOKENTYPE_STRING != stTzToken.eType) || ('M' != stTzToken.unVal.szString[0]))
	{
		szResultString = NULL;
		goto END_FUNC;
	}

	// Month
	szString = hlib_datetime_getNextToken (szString, &stTzToken);
	if (TZ_TOKENTYPE_NUMBER != stTzToken.eType)
	{
		szResultString = NULL;
		goto END_FUNC;
	}

	nMonth = stTzToken.unVal.nNumber;

	szString = hlib_datetime_getNextToken (szString, &stTzToken);
	if (TZ_TOKENTYPE_PERIOD != stTzToken.eType)
	{
		szResultString = NULL;
		goto END_FUNC;
	}

	// Week
	szString = hlib_datetime_getNextToken (szString, &stTzToken);
	if (TZ_TOKENTYPE_NUMBER != stTzToken.eType)
	{
		szResultString = NULL;
		goto END_FUNC;
	}

	nWeek = stTzToken.unVal.nNumber;

	szString = hlib_datetime_getNextToken (szString, &stTzToken);
	if (TZ_TOKENTYPE_PERIOD != stTzToken.eType)
	{
		szResultString = NULL;
		goto END_FUNC;
	}

	// Weekday
	szString = hlib_datetime_getNextToken (szString, &stTzToken);
	if (TZ_TOKENTYPE_NUMBER != stTzToken.eType)
	{
		szResultString = NULL;
		goto END_FUNC;
	}

	bTimeFound = TRUE;
	nWeekday= stTzToken.unVal.nNumber;

	szResultString = szString;

END_FUNC:
	*pbTimeFound	= bTimeFound;
	*pnMonth		= nMonth;
	*pnWeek			= nWeek;
	*pnWeekday		= nWeekday;

	return szResultString;
}

static HERROR hlib_datetime_makeDateTzString (HxDATETIME_Date_t *pstDate, HCHAR *szDateTzString)
{
	HINT32					 nRemainDay;
	HUINT8					 ucLastDay;
	HUINT32					 ulWeeks;
	HUINT32 				 ulWeekDay, ulDays;

	ulWeekDay = HLIB_DATETIME_GetWeekDayIndex (*pstDate);
	ulDays = HLIB_DATETIME_GetDaysInYear (*pstDate);

	ucLastDay = HLIB_DATETIME_GetLastDayOfMonth (pstDate->usYear, pstDate->ucMonth);
	if (0)
	{
		ulWeeks = 5;
	}
	else
	{
		ulWeeks = 0;
		nRemainDay = (HINT32)pstDate->ucDay;
		while (nRemainDay > 0)
		{
			ulWeeks ++;
			nRemainDay -= 7;
		}
	}

	HxSTD_PrintToStrN (szDateTzString, 16, "M%d.%d.%d", pstDate->ucMonth, ulWeeks, ulWeekDay);
	return ERR_OK;
}

static HBOOL hlib_datetime_isEqualKnownTzCodeWithChangeDate(HxDATETIME_Dst_t dstCode, HxDATETIME_Date_t changeDate)
{
	HxDATETIME_Date_t dstDate;
	HINT32	 dstResult;

	dstResult = HLIB_DATETIME_GetWeekDayXThFromDate(changeDate, dstCode.eWeekday, dstCode.eWeekth, &dstDate);

	if ((changeDate.usYear 	== dstDate.usYear) 	&&
		(changeDate.ucMonth	== dstDate.ucMonth)	&&
		(changeDate.ucDay	== dstDate.ucDay))
		return TRUE;
	else
		return FALSE;
}

// nCurrOffset, nNextOffset : min.
static HERROR hlib_datetime_makeTotDstTzStringFromKnownTzCode (HUINT32 ulCurrTime, HxCountry_e country, HUINT32 ulTimeOfChange, HINT32 nCurrOffset, HINT32 nNextOffset, HCHAR *szDstString)
{
	HINT32					 nDstOffsetMin = nNextOffset - nCurrOffset;
	HUINT32					 ulCount, ulListNum = sizeof(s_stTime_TzDsts) / sizeof(HxDATETIME_TzDstInfo_t);
	HCHAR					 szChangeStartDateStr[16], szChangeEndDateStr[16];
	HxDATETIME_t			 stChangeDateTime;

	HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset (ulTimeOfChange, &stChangeDateTime);

	for (ulCount = 0; ulCount < ulListNum; ulCount++)
	{
		// Offset, Local Time
		HxDATETIME_TzDstInfo_t	*pstDstInfo = s_stTime_TzDsts + ulCount;

		if (pstDstInfo->eCountry == country)
		{
			// No Offset, UTC
			HINT32			nChangedTimeMinute = stChangeDateTime.stTime.ucHour * 60 + stChangeDateTime.stTime.ucMinute;
			HINT32			nDstStartTime = (pstDstInfo->DstStartInfo.ucHour * 60 + pstDstInfo->DstStartInfo.ucMinute);
			HINT32			nDstEndTime = (pstDstInfo->DstEndInfo.ucHour * 60 + pstDstInfo->DstEndInfo.ucMinute);

			// Start Time 에 적용시 값이 맞는다
			if ( (hlib_datetime_isEqualKnownTzCodeWithChangeDate(pstDstInfo->DstStartInfo, stChangeDateTime.stDate)) &&
				 (nChangedTimeMinute + pstDstInfo->nStdOffsetMin == nDstStartTime) &&
				 (nDstOffsetMin == pstDstInfo->nDstOffsetMin))
			{
				HxDATETIME_t	stChangeEndDateTime;
				HxDATETIME_Date_t		stChangeEndDate, stResultEndDate;
				HINT32			dstResult;


				HxSTD_MemSet(&stChangeEndDateTime, 0, sizeof(HxDATETIME_t));
				HxSTD_MemSet(&stChangeEndDate, 0, sizeof(HxDATETIME_Date_t));
				HxSTD_MemSet(&stResultEndDate, 0, sizeof(HxDATETIME_Date_t));

				// start time 을 알고 end time 을 만들기 위해서는 현재를 기준으로 해야 현재시간 기준으로 잘 돈다.
				HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset (ulCurrTime, &stChangeEndDateTime);
				stChangeEndDate.usYear	=	stChangeEndDateTime.stDate.usYear;
				stChangeEndDate.ucMonth	=	pstDstInfo->DstEndInfo.ucMonth;
				// 아래 함수에서 day 는 셋팅을 안해도 상관없다.
				dstResult = HLIB_DATETIME_GetWeekDayXThFromDate(stChangeEndDate,
															 pstDstInfo->DstEndInfo.eWeekday,
															 pstDstInfo->DstEndInfo.eWeekth, &stResultEndDate);
				if (dstResult != -1)
				{
					stChangeEndDateTime.stDate = stResultEndDate;		// 얻어온 HxDATETIME_Date_t 를 설정.
				}
				else
				{
					return ERR_FAIL;
				}

				hlib_datetime_makeDateTzString (&stChangeDateTime.stDate, szChangeStartDateStr);
				hlib_datetime_makeDateTzString (&stChangeEndDateTime.stDate, szChangeEndDateStr);

				HxSTD_PrintToStrN (szDstString, TZ_NAME_LENGTH, "GMT%c%02d:%02dDST%c%02d:%02d,%s/%d:%02d,%s/%d:%02d",
										(nCurrOffset > 0) ? '-':'+',
										abs(nCurrOffset / 60), abs(nCurrOffset % 60),
										(nNextOffset > 0) ? '-':'+',
										abs(nNextOffset / 60), abs(nNextOffset % 60),
										szChangeStartDateStr,
										(pstDstInfo->DstStartInfo.ucHour),
										(pstDstInfo->DstStartInfo.ucMinute),
										szChangeEndDateStr,
										(pstDstInfo->DstEndInfo.ucHour),
										(pstDstInfo->DstEndInfo.ucMinute));

				return ERR_OK;
			}
			// End Time 에 적용 시 값이 맞는가
			else if ( (hlib_datetime_isEqualKnownTzCodeWithChangeDate(pstDstInfo->DstEndInfo, stChangeDateTime.stDate)) &&
				 (nChangedTimeMinute + pstDstInfo->nDstOffsetMin == nDstEndTime) &&
				(nDstOffsetMin == pstDstInfo->nDstOffsetMin * -1))
			{
				HxDATETIME_t	stChangeStartDateTime;
				HxDATETIME_Date_t		stChangeStartDate, stResultStartDate;
				HINT32			dstResult;



				HxSTD_MemSet(&stChangeStartDateTime, 0, sizeof(HxDATETIME_t));
				HxSTD_MemSet(&stChangeStartDate, 0, sizeof(HxDATETIME_Date_t));
				HxSTD_MemSet(&stResultStartDate, 0, sizeof(HxDATETIME_Date_t));

				// end time 을 알고 start time 을 만들기 위해서는 현재를 기준으로 해야 현재시간 기준으로 잘 돈다.
				HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset (ulCurrTime, &stChangeStartDateTime);
				stChangeStartDate.usYear	=	stChangeStartDateTime.stDate.usYear;
				stChangeStartDate.ucMonth	=	pstDstInfo->DstStartInfo.ucMonth;
				// 아래 함수에서 day 는 셋팅을 안해도 상관없다.
				dstResult = HLIB_DATETIME_GetWeekDayXThFromDate(stChangeStartDate,
															 pstDstInfo->DstStartInfo.eWeekday,
															 pstDstInfo->DstStartInfo.eWeekth, &stResultStartDate);

				stChangeStartDateTime.stDate = stResultStartDate;		// 얻어온 HxDATETIME_Date_t 를 설정.

				hlib_datetime_makeDateTzString (&stChangeStartDateTime.stDate, szChangeStartDateStr);
				hlib_datetime_makeDateTzString (&stChangeDateTime.stDate, szChangeEndDateStr);

				HxSTD_PrintToStrN (szDstString, TZ_NAME_LENGTH, "GMT%c%02d:%02dDST%c%02d:%02d,%s/%d:%02d,%s/%d:%02d",
										(nNextOffset > 0) ? '-':'+',
										abs(nNextOffset / 60), abs(nNextOffset % 60),
										(nCurrOffset > 0) ? '-':'+',
										abs(nCurrOffset / 60), abs(nCurrOffset % 60),
										szChangeStartDateStr,
										(pstDstInfo->DstStartInfo.ucHour),
										(pstDstInfo->DstStartInfo.ucMinute),
										szChangeEndDateStr,
										(pstDstInfo->DstEndInfo.ucHour),
										(pstDstInfo->DstEndInfo.ucMinute));

				return ERR_OK;
			}
		}	// is equal country code
	}

	return ERR_FAIL;
}

// 정확한 DST용 TZ String을 만드는 대신 근사치로 만드는 방법
static HERROR hlib_datetime_presumeTotDstTzString (HUINT32 ulCurrTime, HUINT32 ulTimeOfChange, HINT32 nCurrOffset, HINT32 nNextOffset, HCHAR *szDstString)
{
	HUINT32					 ulStartTime, ulEndTime;
	HINT32					 nNormalOffset, nDstOffset;
	HCHAR					 szDstStartString[16], szDstEndString[16];
	HxDATETIME_t			 stCurrDateTime, stChangeDateTime;
	HxDATETIME_t			 stStartDateTime, stEndDateTime;
	HUINT32					 ulStartTimeHour, ulStartTimeMin;
	HUINT32					 ulEndTimeHour, ulEndTimeMin;

	HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset (ulCurrTime, &stCurrDateTime);
	HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset (ulTimeOfChange, &stChangeDateTime);

	if (ulCurrTime < ulTimeOfChange)
	{
		// 1. Offset 변환이 같은 연도이다 !
		if (stCurrDateTime.stDate.usYear == stChangeDateTime.stDate.usYear)
		{
			// 1.1 현재 시간이 3월 이상이다 : Current 2달 전 ~ ChangeTime 까지가 DST 적용 기간이라고 간주한다.
			// 미적용시에는 ulNextOffset, 적용시에는 ulCurrOffset
			if (stCurrDateTime.stDate.ucMonth > 2)
			{
				// Start Time은 Current 2달 전의 1일 12시:
				HxSTD_MemCopy (&stStartDateTime, &stCurrDateTime, sizeof(HxDATETIME_t));
				stStartDateTime.stDate.ucMonth -= 2;
				stStartDateTime.stDate.ucDay = 1;
				ulStartTime = (12 * 60);

				// End Time은 Change Time
				HxSTD_MemCopy (&stEndDateTime, &stChangeDateTime, sizeof(HxDATETIME_t));
				ulEndTime = (stEndDateTime.stTime.ucHour * 60) + stEndDateTime.stTime.ucMinute;

				nNormalOffset = nNextOffset;
				nDstOffset = nCurrOffset;
			}
			// 1.2 현재 기간이 3월 미만이다 : ChangeTime ~ 12월 말 까지가 DST 적용.
			// 미적용시에는 ulCurrOffset, 적용시에는 ulNextOffset
			else
			{
				// Start Time은 Changed Time
				HxSTD_MemCopy (&stStartDateTime, &stChangeDateTime, sizeof(HxDATETIME_t));
				ulStartTime = (stStartDateTime.stTime.ucHour * 60) + stStartDateTime.stTime.ucMinute;

				// End Time은 12월 31일 12시
				stEndDateTime.stDate.usYear = stStartDateTime.stDate.usYear;
				stEndDateTime.stDate.ucMonth = 12;
				stEndDateTime.stDate.ucDay = 31;
				ulEndTime = (12 * 60);

				nNormalOffset = nCurrOffset;
				nDstOffset = nNextOffset;
			}
		}
		// 2. 현재와 Offset 변하는 연도가 다른 연도 : (ex: 현재 2012년 11월 3일, Change일 2013년 3월 31일)
		else
		{
			// TimeOfChange가 현재 시간의 1년 이상
			if ((ulTimeOfChange - ulCurrTime) >= (60 * 60 * 24 * 365))
			{
				HxSTD_PrintToStrN (szDstString, TZ_NAME_LENGTH, "GMT%c%02d:%02d",
										(nCurrOffset > 0) ? '-':'+',
										(nCurrOffset / 60), (nCurrOffset % 60));
				return ERR_OK;
			}
			// 2.2 현재 기간 -2달이 change일의 달보다 크다
			// (연도를 제거한) Change 일 ~ 현재 기간 - 2달 이 DST 적용 기간.
			// 미 적용시에는 ulCurrOffset, 적용시에는 ulNextOffset
			else if (stCurrDateTime.stDate.ucMonth > (stChangeDateTime.stDate.ucMonth + 2))
			{
				// Start Time은 Changed Time
				HxSTD_MemCopy (&stStartDateTime, &stChangeDateTime, sizeof(HxDATETIME_t));
				ulStartTime = (stStartDateTime.stTime.ucHour * 60) + stStartDateTime.stTime.ucMinute;

				// End Time은 현재 기간 -2월 12시
				stEndDateTime.stDate.usYear = stCurrDateTime.stDate.usYear;
				stEndDateTime.stDate.ucMonth = stCurrDateTime.stDate.ucMonth - 2;
				stEndDateTime.stDate.ucDay = 1;
				ulEndTime = (12 * 60);

				nNormalOffset = nCurrOffset;
				nDstOffset = nNextOffset;
			}
			// 2.3 현재 기간 -1달이 change일의 날보다 작다 : 이런 경우라면 TZ로 커버할 수 없으므로 (TZ에 Year가 들어가야 한다)
			// change Day ~ 12월 31일 까지를 적용기간으로 잡는다.
			// 미 적용시에는 ulCurrOffset, 적용시에는 ulNextOffset
			else
			{
				// Start Time은 Changed Time
				HxSTD_MemCopy (&stStartDateTime, &stChangeDateTime, sizeof(HxDATETIME_t));
				ulStartTime = (stStartDateTime.stTime.ucHour * 60) + stStartDateTime.stTime.ucMinute;

				// End Time은 12월 31일 12시
				stEndDateTime.stDate.usYear = stStartDateTime.stDate.usYear;
				stEndDateTime.stDate.ucMonth = 12;
				stEndDateTime.stDate.ucDay = 31;
				ulEndTime = (12 * 60);

				nNormalOffset = nCurrOffset;
				nDstOffset = nNextOffset;
			}
		}
	}
	// Current Time이 ChangeTime을 넘어가면 더더욱 비교해서 추측할 요소가 없어진다.
	else
	{
		// Changed Date Time이 Current Date Time의 연도 + 1 조차 넘어섰다
		// changed를 적용할 의미가 없어진다.
		if (stCurrDateTime.stDate.usYear > (stChangeDateTime.stDate.usYear + 1))
		{
			HxSTD_PrintToStrN (szDstString, TZ_NAME_LENGTH, "GMT%c%02d:%02d",
									(nCurrOffset > 0) ? '-':'+',
									(nCurrOffset / 60), (nCurrOffset % 60));
			return ERR_OK;
		}
		// Changed가 3월 이후 : DST 적용은 1월 1일 ~ Changed
		// 미적용시에는 ulNextOffset, 적용시에는 ulCurrOffset
		else if (stChangeDateTime.stDate.ucMonth > 3)
		{
			// Start Time은 그해 1월 1일 12시
			stStartDateTime.stDate.usYear = stChangeDateTime.stDate.usYear;
			stStartDateTime.stDate.ucMonth = 1;
			stStartDateTime.stDate.ucDay = 1;

			ulStartTime = (12 * 60);

			// End Time은 Change Time
			HxSTD_MemCopy (&stEndDateTime, &stChangeDateTime, sizeof(HxDATETIME_t));
			ulEndTime = (stEndDateTime.stTime.ucHour * 60) + stEndDateTime.stTime.ucMinute;

			nNormalOffset = nNextOffset;
			nDstOffset = nCurrOffset;
		}
		// Changed가 3월 이전 : DST 적용은 Changed ~ 12월 31일
		// 미적용시에는 ulCurrOffset, 적용시에는 ulNextOffset
		else
		{
			// Start Time은 Changed Time
			HxSTD_MemCopy (&stStartDateTime, &stChangeDateTime, sizeof(HxDATETIME_t));
			ulStartTime = (stStartDateTime.stTime.ucHour * 60) + stStartDateTime.stTime.ucMinute;

			// End Time은 12월 31일 12시
			stEndDateTime.stDate.usYear = stStartDateTime.stDate.usYear;
			stEndDateTime.stDate.ucMonth = 12;
			stEndDateTime.stDate.ucDay = 31;
			ulEndTime = (12 * 60);
			nNormalOffset = nCurrOffset;
			nDstOffset = nNextOffset;
		}
	}

	/* hour가 24시간 이상인 경우 예외처리 해준다 */
	ulStartTimeHour 	= (ulStartTime + nNormalOffset) / 60;
	ulStartTimeMin 		= (ulStartTime + nNormalOffset) % 60;
	if (ulStartTimeHour >= HOURS_PER_DAY)
	{
		// 24시간을 넘는 경우엔 day를 늘려주고 시간을 조정한다
		// 이렇게 늘린 day가 invalid한  경우에 대한 처리는.. 지금은 발생하지 않으니 다음 타자가....! (ex-1월 32일 등..)
		stStartDateTime.stDate.ucDay += (ulStartTimeHour / HOURS_PER_DAY);
		ulStartTimeHour = ulStartTimeHour % HOURS_PER_DAY;
	}

	ulEndTimeHour 		= (ulEndTime + nDstOffset) / 60;
	ulEndTimeMin 		= (ulEndTime + nDstOffset) % 60;
	if (ulEndTimeHour >= HOURS_PER_DAY)
	{
		// 24시간을 넘는 경우엔 day를 늘려주고 시간을 조정한다
		// 이렇게 늘린 day가 invalid한 경우에 대한 처리는.. 지금은 발생하지 않으니 다음 타자가....! (ex-1월 32일 등..)
		stEndDateTime.stDate.ucDay += (ulEndTimeHour / HOURS_PER_DAY);
		ulEndTimeHour = ulEndTimeHour % HOURS_PER_DAY;
	}

	hlib_datetime_makeDateTzString (&stStartDateTime.stDate, szDstStartString);
	hlib_datetime_makeDateTzString (&stEndDateTime.stDate, szDstEndString);

	HxSTD_PrintToStrN (szDstString, TZ_NAME_LENGTH, "GMT%c%02d:%02dDST%c%02d:%02d,%s/%d:%02d,%s/%d:%02d",
							(nNormalOffset > 0) ? '-':'+',
							abs(nNormalOffset / 60), abs(nNormalOffset % 60),
							(nDstOffset > 0) ? '-':'+',
							abs(nDstOffset / 60), abs(nDstOffset % 60),
							/*"M2.1.1",*/szDstStartString,
							ulStartTimeHour, ulStartTimeMin,
							/*"M12.1.1",*/szDstEndString,
							ulEndTimeHour, ulEndTimeMin);


	return ERR_OK;
}

void	HLIB_DATETIME_DEFINED_BUT_NOT_USED (void)
{
	//(void)hlib_datetime_GetNextDay;
	//(void)hlib_datetime_GetNextWeek;
	//(void)hlib_datetime_GetNextWorkDay;
	//(void)hlib_datetime_GetNextWithoutSunday;
	//(void)hlib_datetime_GetNextWeekend;
}

// debug
HERROR HLIB_DATETIME_PrintCurrentDateTime(void)
{
	unsigned long UnixTime;
	HxDATETIME_t dateTime;

	HxSTD_MemSet(&dateTime,0,sizeof(HxDATETIME_t));

	UnixTime = HLIB_STD_GetSystemTime();

	/* 만약 local time이 invalide 하다면, 에러가 나겠지. */
	HLIB_DATETIME_ConvertUnixTimeToDateTime(UnixTime, &dateTime);

	HxLOG_Print("\n\rCURRENT TIME is [%d-%d-%d] : [%d-%d-%d-%d]\n\n\r",dateTime.stDate.usYear,dateTime.stDate.ucMonth,dateTime.stDate.ucDay,
		dateTime.stTime.ucHour,dateTime.stTime.ucMinute,dateTime.stTime.ucSecond,dateTime.stTime.usMillisecond);
	return ERR_OK;
}

HERROR HLIB_DATETIME_PrintDateTime(UNIXTIME unixtime)
{
	HxDATETIME_t dateTime;

	HxSTD_MemSet(&dateTime,0,sizeof(HxDATETIME_t));

	/* 만약 local time이 invalide 하다면, 에러가 나겠지. */
	HLIB_DATETIME_ConvertUnixTimeToDateTime(unixtime, &dateTime);

	HxLOG_Print("\r TIME is (0x%x) [%d-%d-%d] : [%d-%d-%d-%d]\r\n",unixtime, dateTime.stDate.usYear,dateTime.stDate.ucMonth,dateTime.stDate.ucDay,
		dateTime.stTime.ucHour,dateTime.stTime.ucMinute,dateTime.stTime.ucSecond,dateTime.stTime.usMillisecond);
	return ERR_OK;
}



