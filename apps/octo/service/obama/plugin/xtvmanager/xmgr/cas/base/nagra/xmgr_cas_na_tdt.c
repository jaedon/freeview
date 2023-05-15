/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <mgr_action.h>
#include <mgr_live.h>

#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_cas.h>

#include <settings.h>

#include "local_include/_xmgr_cas_na_util.h"
#include "local_include/_xmgr_cas_na_adapt.h"
#include "local_include/_xmgr_cas_na.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Definitions         **************************/
/*******************************************************************/
#define	EMM_TDT_AUTOUPDATE_TIME_DUR			(SECONDS_PER_MIN*30)
#if defined(CONFIG_MW_CAS_NAGRA_CAK_TEST)
#define	EMM_TDT_AUTOUPDATE_DEFAULT_TIME		(3)
#else
#define	EMM_TDT_AUTOUPDATE_DEFAULT_TIME		(4)
#endif

/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
STATIC HERROR		xmgr_cas_NaTdtUpdateEmmWakeUp (void);
STATIC HERROR 		xmgr_cas_NaTdtGetRsvUpdateUnixTime (const HxDATETIME_Time_t *updateTime, UNIXTIME *pulRsvUnixTime);
STATIC void 		xmgr_cas_NaTdtAddUpdateTimeRsv (RsvType_t eRsvType, UNIXTIME utMenuTime);

/*******************************************************************/
/********************      Public Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_NA_TDT_LOCAL_FUNCTION______________________________________________________

HERROR			xmgr_cas_NaTdtEMMUpdate(void)
{
	return xmgr_cas_NaTdtUpdateEmmWakeUp();
}


#define _____XMGR_CAS_NA_TDT_LOCAL_FUNCTION______________________________________________________


STATIC HERROR			xmgr_cas_NaTdtUpdateEmmWakeUp(void)
{
	UNIXTIME		ulRsvUnixtime = 0;
	HxDATETIME_Time_t		inputedTime = {0, };
	HERROR			hError = ERR_FAIL;


	HxLOG_Info("[xmgr_cas_NaTdtUpdateEmmWakeUp] Enter!!\n");

#if defined(CONFIG_MW_CAS_NAGRA_CAK_TEST)
{
	UNIXTIME		ulCurLocalUnixtime;
	HxDATETIME_t	stCurLocalDate;

	if(VK_CLOCK_GetTime((HULONG *)&ulCurLocalUnixtime) == ERR_OK)
	{
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurLocalUnixtime, &stCurLocalDate);
	}
	inputedTime.ucHour = stCurLocalDate.stTime.ucHour;
	inputedTime.ucMinute = EMM_TDT_AUTOUPDATE_DEFAULT_TIME;
}
#else
	inputedTime.ucHour = EMM_TDT_AUTOUPDATE_DEFAULT_TIME; //default 4 o'clock
#endif

	hError = xmgr_cas_NaTdtGetRsvUpdateUnixTime((const HxDATETIME_Time_t *)&inputedTime, &ulRsvUnixtime);
	if(hError != ERR_OK)
	{
		HxLOG_Error("[xmgr_cas_NaTdtUpdateEmmWakeUp] xmgr_cas_NaTdtGetRsvUpdateUnixTime fail!!\n");
	}

	xmgr_cas_NaTdtAddUpdateTimeRsv(RSV_TYPE_EMM, ulRsvUnixtime);


	return ERR_OK;
}



STATIC HERROR 		xmgr_cas_NaTdtGetRsvUpdateUnixTime(const HxDATETIME_Time_t *updateTime, UNIXTIME *pulRsvUnixTime)
{
	UNIXTIME			ulCurLocalUnixtime;		/**< time from system */
	HxDATETIME_t		stCurLocalDate;			/**< current date */
	HxDATETIME_t		stRsvDate = { {0, }, {0, }, };

	HxLOG_Info("[xmgr_cas_NaTdtGetRsvUpdateUnixTime] ENTER !!\n");

	if(VK_CLOCK_GetTime((HULONG *)&ulCurLocalUnixtime) == ERR_OK)
	{
		HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurLocalUnixtime, &stCurLocalDate);
	}
	else
	{
		HxLOG_Error("[xmgr_cas_NaTdtGetRsvUpdateUnixTime] VK_CLOCK_GetTime fail!!\n");
		return ERR_FAIL;
	}

	VK_memcpy32(&stRsvDate, &stCurLocalDate, sizeof(HxDATETIME_t));

	stRsvDate.stTime.ucHour = updateTime->ucHour;
	stRsvDate.stTime.ucMinute = updateTime->ucMinute;
	stRsvDate.stTime.ucSecond = updateTime->ucSecond;
	stRsvDate.stTime.usMillisecond = updateTime->usMillisecond;

	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stRsvDate, pulRsvUnixTime);

	if(*pulRsvUnixTime <= ulCurLocalUnixtime)
	{
		*pulRsvUnixTime += SECONDS_PER_DAY;
	}

	HxLOG_Info("[xmgr_cas_NaTdtGetRsvUpdateUnixTime] stRsvDate(%d.%02d.%02d   %02d:%02d)!!\n",
														stRsvDate.stDate.usYear,
														stRsvDate.stDate.ucMonth,
														stRsvDate.stDate.ucDay,
														stRsvDate.stTime.ucHour,
														stRsvDate.stTime.ucMinute));

	return ERR_OK;
}



STATIC void 		xmgr_cas_NaTdtAddUpdateTimeRsv(RsvType_t eRsvType, UNIXTIME utMenuTime)
{
	HERROR		errError = ERR_FAIL;
	HUINT32		newSlot = 0;
	//HUINT32		ulMasterActionId = 0;
	RSV_Item_t	rsvItem = {0, };
	RsvRepeat_t	rsvRepeat = RSV_DAILY;
	Handle_t	phSvcHandle = HANDLE_NULL;
	DbSvc_Info_t	stSvcInfo = {0, };

	HxLOG_Info("[xmgr_cas_NaTdtAddUpdateTimeRsv] ENTER !!\n");

	/* invalidate service handle, name */
	phSvcHandle = HANDLE_NULL;
	HxSTD_memset(&stSvcInfo, 0x00, sizeof(DbSvc_Info_t));

	/* 기존 동일 예약이 있는지 검사. */
	errError = MW_RSV_CheckUpReservedItem(eRsvType, &newSlot, &rsvItem);
	if (errError == ERR_OK)
	{
		/* 기존 예약 변경. */
		rsvItem.eRsvType	= eRsvType;
		rsvItem.hSvc		= phSvcHandle;
		rsvItem.utStTime	= utMenuTime;
		rsvItem.utDuration	= EMM_TDT_AUTOUPDATE_TIME_DUR;
		rsvItem.eRepeat		= rsvRepeat;
		HLIB_STD_StrUtf8NCpy (rsvItem.ext.stWch.szEvtName, DbSvc_GetName(&stSvcInfo), MAX_RSV_EVT_NAME);

		MW_RSV_ChangeReservedItem(newSlot, &rsvItem, FALSE);
		HxLOG_Info("[xmgr_cas_NaTdtAddUpdateTimeRsv] MW_RSV_ChangeReservedItem avail !!\n");
	}
	else
	{	/* 새 예약 추가 */
		errError = MW_RSV_GetAvailableSlot(eRsvType, &newSlot);
		if (errError != ERR_OK)
		{
			return;
		}

		rsvItem.eRsvType	= eRsvType;
		rsvItem.hSvc		= phSvcHandle;
		rsvItem.eReady		= RSV_READY_20_SEC;
		rsvItem.utStTime	= utMenuTime;
		rsvItem.utDuration	= EMM_TDT_AUTOUPDATE_TIME_DUR;
		rsvItem.eRepeat		= rsvRepeat;
		MWC_UTIL_DvbStrcpy(rsvItem.ext.stWch.szEvtName, DbSvc_GetName(&stSvcInfo));
		MW_RSV_MakeReservedItem(newSlot, &rsvItem);

		HxLOG_Info("[xmgr_cas_NaTdtAddUpdateTimeRsv] MW_RSV_MakeReservedItem !!\n");
	}
#if 1
{
	HxDATETIME_t	stRsvDate = { {0, }, {0, }, };

	HLIB_DATETIME_ConvertUnixTimeToDateTime(utMenuTime, &stRsvDate);
	HxLOG_Info("[xmgr_cas_NaTdtAddUpdateTimeRsv] stRsvDate(%d.%02d.%02d   %02d:%02d)!!\n",
														stRsvDate.stDate.usYear,
														stRsvDate.stDate.ucMonth,
														stRsvDate.stDate.ucDay,
														stRsvDate.stTime.ucHour,
														stRsvDate.stTime.ucMinute));

}
#endif
}
