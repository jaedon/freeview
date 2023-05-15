/**************************************************************************************************/
#define ________CA_CAK_EXT_Private_Include_________________________________________________
/**************************************************************************************************/
#include "vkernel.h"
#include "na_glob.h"
#include "namgr_main.h"



/**************************************************************************************************/
#define ________CA_CAK_EXT_Golbal_Value___________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_EXT_Private_Define__________________________________________________
/**************************************************************************************************/






/**************************************************************************************************/
#define ________CA_CAK_EXT_Private_Type_Define____________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_EXT_Private_Static_Value____________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_EXT_Private_Static_Prototype_________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_EXT_Private_Static_Prototype_Body____________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_CAK_EXT_Public_Functions_Body___________________________________________
/**************************************************************************************************/
HERROR CA_NA_CakGetGmtTimeAsCalenderTime(TCalendarTime *pstCalendar)
{
	TSignedInt32	nErrCode;
	UNIXTIME 		ulUnixtime;
	HxDATETIME_t	stDateTime;

	if (pstCalendar == NULL)
		return ERR_FAIL;

	nErrCode = VK_CLOCK_GetTime((unsigned long*)&ulUnixtime);
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("[%s:%d] VK_CLOCK_GetTime() return error\n",  __FUNCTION__, __HxLINE__);
		pstCalendar->year = (unsigned char)(1970-1900);
		pstCalendar->month = (unsigned char)1;
		pstCalendar->day = (unsigned char)1;
		pstCalendar->hour= (unsigned char)00;
		pstCalendar->minute= (unsigned char)00;
		pstCalendar->second= (unsigned char)00;
		return ERR_OK;
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_MEDIASET)  ||  defined(CONFIG_MW_CAS_NAGRA_OP_SPAIN_TDT)
	nErrCode = HLIB_DATETIME_ConvertUnixTimeToDateTimeWithOutOffset(ulUnixtime, (HxDATETIME_t*)&stDateTime);
#else
	nErrCode = HLIB_DATETIME_ConvertUnixTimeToDateTime(ulUnixtime, (HxDATETIME_t*)&stDateTime);
#endif
	if (nErrCode != ERR_OK)
	{
		HxLOG_Error("HLIB_DATETIME_ConvertUnixTimeToDateTime() return error\n");
		return nErrCode;
	}

	HxLOG_Print("[%s:%d] YEAR=%d, MONTH=%d, DAY=%d, HOUR=%d, MIN=%d, SEC=%d\n", __FUNCTION__, __HxLINE__,
		stDateTime.stDate.usYear, stDateTime.stDate.ucMonth, stDateTime.stDate.ucDay,
		stDateTime.stTime.ucHour, stDateTime.stTime.ucMinute, stDateTime.stTime.ucSecond);

	pstCalendar->year 	= (unsigned char)(stDateTime.stDate.usYear - 1900);
	pstCalendar->month 	= (unsigned char)(stDateTime.stDate.ucMonth);
	pstCalendar->day 	= (unsigned char)(stDateTime.stDate.ucDay);
	pstCalendar->hour	= (unsigned char)(stDateTime.stTime.ucHour);
	pstCalendar->minute	= (unsigned char)(stDateTime.stTime.ucMinute);
	pstCalendar->second	= (unsigned char)(stDateTime.stTime.ucSecond);

	return ERR_OK;
}


