/*
	Viaccess OS API Adaptation Layer
	some part of codes make confliction with Humax MW. so, they should be put in a separated file
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "va_def.h"
#include <tapi.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/




#define MJD_1970_1_1	0x9e8b

/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Static variables
********************************************************************/

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/

static HERROR local_va_os_GetTime(HUINT32 *pulTime)
{
	HUINT32 ulErr;

	if (VK_CLOCK_GetTime(pulTime) == VK_OK)
		return ERR_OK;

	return ERR_FAIL;
}

#if defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
static HERROR local_va_os_ConvertDateTimeToUnixTime(HxDATETIME_t *pstDateTime, UNIXTIME *pstUnixTime)
{
	return UTIL_TCONV_ConvertDateTimeToUnixTime(pstDateTime, pstUnixTime);
}
#endif

static HERROR local_va_os_ConvertUnixTimeToDateTime(UNIXTIME stUnixTime, HxDATETIME_t *pstDateTime)
{
	return HLIB_DATETIME_ConvertUnixTimeToDateTime(stUnixTime, pstDateTime);
}

unsigned long VA_OS_Adapt_GetTime()
{
	HUINT32 ulCurrentTime;
	UNIXTIME stUnixtime;
	UNIXTIME  ulTimeOffset;
	HxDATETIME_t stDateTime;
	HBOOL bSign_Plus;
	HUINT16 usMJD;

	HxLOG_Print("[Enter]\n");

	// time functions are copied from vtm_api.c (for ACS 1.1) and modified for ACS 3.0
	//	void tm_time_get ( UINT32 current_time );
	//	tm_time_from_gmt (	const gmt_time	*current_gmt_time , va_time			*current_time );
	//	va_YMD2MJD ( u_int8 year, u_int8 month, u_int8 mday );

	local_va_os_GetTime( &stUnixtime );
	local_va_os_ConvertUnixTimeToDateTime( stUnixtime, &stDateTime );

//DDI_UART_Print("****** UnixTime = %ld\n", stUnixtime);

	usMJD = VA_OS_YMD2MJD(stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay);

	ulCurrentTime = (usMJD - MJD_1970_1_1) * 24 * 3600
						   + stDateTime.stTime.ucHour * 3600
						   + stDateTime.stTime.ucMinute  * 60
						   + stDateTime.stTime.ucSecond;

#if 0 // TM_GetLocalUnixTime() returns a time value regargind the time offset
	TM_GetTimeOffset(&bSign_Plus, &ulTimeOffset);

	if ( bSign_Plus == TRUE )
	{
		ulCurrentTime += ulTimeOffset;
	}
	else
	{
		ulCurrentTime -= ulTimeOffset;
	}
#endif

	HxLOG_Info("[%ld][%d-%d-%d %d:%d:%d]\n", ulCurrentTime, stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay,
									stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond);

	HxLOG_Print("[Exit]\n");
	return ulCurrentTime;
}

#if defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
void VA_OS_Adapt_SetTime()
{
	unsigned long ulUnixtime;
	HxDATETIME_t stDateTime;
	char strMonth[10], strTmp[100];
	char *strMonthName[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	int i;
	int d, y, h, m, s;

	// (Sep 19 2009) (10:10:10)
	sprintf(strTmp, "%s %s", __DATE__, __TIME__);
	sscanf(strTmp, "%3s %d %d %d:%d:%d", strMonth, &d, &y, &h, &m, &s);

	stDateTime.stDate.ucDay = d;
	stDateTime.stDate.usYear = y;
	stDateTime.stTime.ucHour = h;
	stDateTime.stTime.ucMinute = m;
	stDateTime.stTime.ucSecond = s;
	stDateTime.stTime.usMillisecond = 0;

	stDateTime.stDate.ucMonth = 1;

	for (i = 0; i < 12; i++)
	{
		if (VA_strcmp(strMonth, strMonthName[i]) == 0)
		{
			stDateTime.stDate.ucMonth = i + 1;
			break;
		}
	}

	HxLOG_Info("**** for test, set current time as %d-%d-%d %d:%d:%d ****\n", stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay,
						stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond);


	local_va_os_ConvertDateTimeToUnixTime(&stDateTime, &ulUnixtime);

	if (TAPI_SetTime(TxSOURCE_SET_BY_USERINPUT, ulUnixtime) != ERR_OK)
		HxLOG_Error("[TAPI_SetTime is Failed - time %u]\n", ulUnixtime);
}
#endif


