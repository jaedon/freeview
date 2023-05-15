#include "htype.h" //hee
#include "timeconv.h"
#include "vkernel.h"

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

static HERROR local_tconv_ConvertUnixTimeToDate(UNIXTIME	unixtime, DATE_T *date)
{
	if ( !date )
	{
		return ERR_TCONV_FAIL ;
	}

	return local_tconv_ConvertJulianToDate((unixtime / SECONDS_PER_DAY) + UNIX_FIRST_DAY, date) ;
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

HERROR UTIL_TCONV_ConvertUnixTimeToMJD (UNIXTIME unixtime, HUINT16 *mjd)
{
	return local_tconv_ConvertUnixTimeToMJD (unixtime, mjd);
}


static UNIXTIME local_tconv_ConvGmtUnixTimeToLocalUnixTime (UNIXTIME ulGmtTime)
{
	int					 nHour, nMin;
	int					 nLocalUnixTime;
	DATE_T				 stDate;
	VK_CLOCK_DST_MODE	 eDstMode;
	HERROR				 hErr;

	// 1. Get the offset
	hErr = VK_CLOCK_GetOffsetTime (&nHour, &nMin);
	if (hErr != ERR_OK)
	{
		return ulGmtTime;
	}

	// 2. Add the offset :
	nLocalUnixTime = (int)ulGmtTime + nHour * SECONDS_PER_HOUR + nMin * SECONDS_PER_MIN;

	// 3. Get the year from the offset : to check
	hErr = VK_CLOCK_GetDstMode (&eDstMode);
	if (hErr != VK_OK)					{ eDstMode = VK_CLOCK_DST_AUTO; }

	if (eDstMode == VK_CLOCK_DST_AUTO)
	{
		hErr = local_tconv_ConvertUnixTimeToDate (ulGmtTime, &stDate);
		if (hErr == ERR_OK)
		{
#if 0 //hee
			if (local_tconv_IsEuropeDstRange ((UNIXTIME)ulGmtTime, (HUINT32)stDate.usYear) == TRUE)
			{
				// DST 적용시 1시간 더한다.
				nLocalUnixTime += SECONDS_PER_HOUR;
			}
#endif //hee
		}
	}
	else if (eDstMode == VK_CLOCK_DST_ON_ALWAYS)
	{
		// DST 적용시 1시간 더한다.
		nLocalUnixTime += SECONDS_PER_HOUR;
	}

	return (UNIXTIME)nLocalUnixTime;
}

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

HERROR UTIL_TCONV_ConvertUnixTimeToTime(UNIXTIME	unixtime, TIME_T *time)
{
	UNIXTIME	 ulLocalTime;

	ulLocalTime = local_tconv_ConvGmtUnixTimeToLocalUnixTime (unixtime);

	return local_tconv_ConvertUnixTimeToTime (ulLocalTime, time) ;
}
