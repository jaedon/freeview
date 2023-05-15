/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_time_base.c
	@brief

	Description:  									\n
	Module: SAMA		\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/
/*
 * (c) 2011-2013 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <htype.h>
#include <hlib.h>
#include <vkernel.h>


#include "../local_include/sama_lib_int.h"


/*******************************************************************/
/********************      Local Definition         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

#define ___PRIVATE_FUNCTIONS_DECLARATION___
/*******************************************************************/
/********************      Private Functions Declaration    ********************/
/*******************************************************************/

#define ___PRIVATE_FUNCTIONS___
/*******************************************************************/
/********************      Private Functions    ********************/
/*******************************************************************/
STATIC SAMA_ScheduleWay_e samalib_time_base_getScheduleWayType (DxSchedule_t *pstSchedule)
{
	switch (pstSchedule->eRsvType)
	{
	// Two-way based reservation:
	case DxRSVTYPE_WATCHING_EBR:
	case DxRSVTYPE_WATCHING_TBR:
	case DxRSVTYPE_WATCHING_SERIES_EBR:
	case DxRSVTYPE_RECORD_EBR:
	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_INSTANT:
	case DxRSVTYPE_RECORD_DELAYED:
	case DxRSVTYPE_KEYWORD_RECORD_NOTSURE:
	case DxRSVTYPE_DLNA_EBR:
	case DxRSVTYPE_DLNA_TBR:
	case DxRSVTYPE_DLNA_DUBBING:
	case DxRSVTYPE_NETSTREAMING_LIVE:
	case DxRSVTYPE_NETSTREAMING_FILE:
	case DxRSVTYPE_RECORD_SERIES_EBR:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
	case DxRSVTYPE_ACTIVATION_TIME:
	case DxRSVTYPE_DESC_REC_STANDBY:
		return eSAMA_SchWay_TwoWay;

	// One-way based reservation:
	case DxRSVTYPE_POWER_ON:
	case DxRSVTYPE_POWER_OFF:
	case DxRSVTYPE_REGULAR_OTA:
	case DxRSVTYPE_EPG_GATHERING:
	case DxRSVTYPE_EMM:
	case DxRSVTYPE_UPDATE_TASK:
	case DxRSVTYPE_ASO_NOTIFY:
 	case DxRSVTYPE_MYEPG_UPDATE:
	case DxRSVTYPE_NET_UPDATE:
	case DxRSVTYPE_CAS_POWER_CTRL:
	case DxRSVTYPE_LOGO_DOWN:
	case DxRSVTYPE_RESCAN:
	case DxRSVTYPE_DUBBING:
	case DxRSVTYPE_OTA_BD:
	case DxRSVTYPE_IP_EPG_GATHERING:
	case DxRSVTYPE_IP_REMOTE_PROGRAMMING:
	case DxRSVTYPE_TVTVEPG_TIMER:
	case DxRSVTYPE_CHECK_SATRECORD_TIMER:
		return eSAMA_SchWay_OneWay;

	default:
		HxLOG_Error ("Unknown Rsv Type (%d)\n", pstSchedule->eRsvType);
		break;
	}

	return eSAMA_SchWay_OneWay;
}


#define ___MEMBER_FUNCTIONS___
/*******************************************************************/
/********************      Member Functions       ******************/
/*******************************************************************/
HERROR samalib_time_base_GetPrevTime (DxSchedule_t *pstSchedule, UNIXTIME utSrcTime, UNIXTIME *putPrevTime)
{
	UNIXTIME			 utPrevTime, utTimeGap;
	HERROR				 hErr;

	utPrevTime = utSrcTime;

	switch (pstSchedule->eRepeat)
	{
	case DxRSVREPEAT_DAILY:
		hErr = HLIB_DATETIME_GetPrevDay (utSrcTime, &utPrevTime, &utTimeGap);
		break;

	case DxRSVREPEAT_WEEKLY:
		hErr = HLIB_DATETIME_GetPrevWeek (utSrcTime, &utPrevTime, &utTimeGap);
		break;

	case DxRSVREPEAT_WEEKDAYS:
		hErr = HLIB_DATETIME_GetPrevWorkDay (utSrcTime, &utPrevTime, &utTimeGap);
		break;

	case DxRSVREPEAT_WITHOUT_SUNDAY:
		hErr = HLIB_DATETIME_GetPrevWithoutSunday (utSrcTime, &utPrevTime, &utTimeGap);
		break;

	case DxRSVREPEAT_WEEKEND:
		hErr = HLIB_DATETIME_GetPrevWorkDay (utSrcTime, &utPrevTime, &utTimeGap);
		break;

	case DxRSVREPEAT_FEWDAY_OF_WEEK:
		hErr = HLIB_DATETIME_GetPrevFewDayofWeek (pstSchedule->ulRepeatDays, utSrcTime, &utPrevTime, &utTimeGap);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Debug("repeat(%d) hErr(%d) \n", pstSchedule->eRepeat, hErr);
	}
	if (NULL != putPrevTime)					{ *putPrevTime = utPrevTime; }
	return (utSrcTime != utPrevTime) ? ERR_OK : ERR_FAIL;
}

HERROR samalib_time_base_GetNextTime (DxSchedule_t *pstSchedule, UNIXTIME utSrcTime, UNIXTIME *putNextTime)
{
	UNIXTIME			 utNextTime, utTimeGap;
	HERROR				 hErr;

	utNextTime = utSrcTime;

	switch (pstSchedule->eRepeat)
	{
	case DxRSVREPEAT_DAILY:
		hErr = HLIB_DATETIME_GetNextDay (utSrcTime, &utNextTime, &utTimeGap);
		break;

	case DxRSVREPEAT_WEEKLY:
		hErr = HLIB_DATETIME_GetNextWeek (utSrcTime, &utNextTime, &utTimeGap);
		break;

	case DxRSVREPEAT_WEEKDAYS:
		hErr = HLIB_DATETIME_GetNextWorkDay (utSrcTime, &utNextTime, &utTimeGap);
		break;

	case DxRSVREPEAT_WITHOUT_SUNDAY:
		hErr = HLIB_DATETIME_GetNextWithoutSunday (utSrcTime, &utNextTime, &utTimeGap);
		break;

	case DxRSVREPEAT_WEEKEND:
		hErr = HLIB_DATETIME_GetNextWorkDay (utSrcTime, &utNextTime, &utTimeGap);
		break;

	case DxRSVREPEAT_FEWDAY_OF_WEEK:
		hErr = HLIB_DATETIME_GetNextFewDayofWeek (pstSchedule->ulRepeatDays, utSrcTime, &utNextTime, &utTimeGap);
		break;

	default:
		hErr = ERR_FAIL;
		break;
	}

	if (hErr != ERR_OK)
	{
		HxLOG_Debug("repeat(%d) hErr(%d) \n", pstSchedule->eRepeat, hErr);
	}

	if (NULL != putNextTime)					{ *putNextTime = utNextTime; }
	return (utSrcTime != utNextTime) ? ERR_OK : ERR_FAIL;
}

HERROR samalib_time_base_GetRepeatedScheduleStartTime (DxSchedule_t *pstSchedule, UNIXTIME utBaseTime, UNIXTIME *putStartTime)
{
	UNIXTIME			 utOrgTime, utStartTime, utNextTime;
	HERROR				 hErr;

	if (NULL == pstSchedule)				{ return ERR_FAIL; }

	hErr = HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstSchedule->stStartTime), &utOrgTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("HLIB_DATETIME_ConvertDateTimeToUnixTime err:\n");
		return ERR_FAIL;
	}

	utStartTime = utOrgTime;

	while (utStartTime > utBaseTime)
	{
		hErr = SAMALIB_TIME_GetPrevTime (pstSchedule, utStartTime, &utNextTime);
		if ((ERR_OK != hErr) || (utStartTime == utNextTime))
		{
			break;
		}

		utStartTime = utNextTime;
	}

	while ((utStartTime + pstSchedule->ulDuration) < utOrgTime)
	{
		hErr = SAMALIB_TIME_GetNextTime (pstSchedule, utStartTime, &utNextTime);
		if ((ERR_OK != hErr) || (utStartTime == utNextTime))
		{
			break;
		}

		utStartTime = utNextTime;
	}

	if (NULL != putStartTime)					{ *putStartTime = utStartTime; }
	return ERR_OK;
}

HERROR samalib_time_base_AdjustScheduleTime (DxSchedule_t *pstSchedule, UNIXTIME utBaseTime, HBOOL *pbUpdated)
{
	UNIXTIME			 utOrgTime, utStartTime;
	HERROR				 hErr;

	if (NULL == pstSchedule)				{ return ERR_FAIL; }

	hErr = HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstSchedule->stStartTime), &utOrgTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("HLIB_DATETIME_ConvertDateTimeToUnixTime err:\n");
		return ERR_FAIL;
	}

	hErr = SAMALIB_TIME_GetRepeatedScheduleStartTime (pstSchedule, utBaseTime, &utStartTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error ("samalib_time_base_GetRepeatedScheduleStartTime err:\n");
		return ERR_FAIL;
	}

	if (utOrgTime != utStartTime)
	{
		hErr = HLIB_DATETIME_ConvertUnixTimeToDateTime (utStartTime, &(pstSchedule->stStartTime));
		if (ERR_OK != hErr)
		{
			HxLOG_Error ("HLIB_DATETIME_ConvertUnixTimeToDateTime err: utStartTime(%d)\n", utStartTime);
			return ERR_FAIL;
		}
	}

	if (NULL != pbUpdated)					{ *pbUpdated = (utOrgTime != utStartTime); }
	return ERR_OK;
}

HERROR samalib_time_base_GetPaddingStartEndTime (DxSchedule_t *pstSchedule, UNIXTIME utStartTime, UNIXTIME *putStartTime, UNIXTIME *putEndTime)
{
	UNIXTIME				 utPadStartTime, utPadEndTime;
	HUINT32					 ulPadStartOffset, utPadEndOffset;

	if (NULL == pstSchedule)				{ return ERR_FAIL; }

	if (pstSchedule->uExtInfo.stRec.ulPreOffsetTime == SAMA_TVA_REMOVED_PADDING_TIME)
	{
		// padding conflict에 의해 padding이 지워지게 되면 Removed padding time으로 mark 하고,
		// 실제 동작은 removed 되어야 하는 값인 0으로 사용한다.
		ulPadStartOffset = 0;
	}
	else
	{
		ulPadStartOffset = pstSchedule->uExtInfo.stRec.ulPreOffsetTime;
	}

	if (pstSchedule->uExtInfo.stRec.ulPostOffsetTime == SAMA_TVA_REMOVED_PADDING_TIME)
	{
		utPadEndOffset = 0;
	}
	else
	{
		utPadEndOffset = pstSchedule->uExtInfo.stRec.ulPostOffsetTime;
	}


	switch (pstSchedule->eRsvType)
	{
	case DxRSVTYPE_RECORD_EBR:
	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_SERIES_EBR:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
		utPadStartTime = utStartTime - ulPadStartOffset;
		utPadEndTime = utStartTime + pstSchedule->ulDuration + utPadEndOffset;
		break;

	default:
		utPadStartTime = utStartTime;
		if (samalib_time_base_getScheduleWayType (pstSchedule) == eSAMA_SchWay_TwoWay)
		{
			utPadEndTime = utStartTime + pstSchedule->ulDuration;
		}
		else
		{
			utPadEndTime = utStartTime + 2;
		}

		break;
	}

	if ((utPadStartTime + 2) >= utPadEndTime)	{ utPadEndTime = utPadStartTime + 2; }

	if (NULL != putStartTime)					{ *putStartTime = utPadStartTime; }
	if (NULL != putEndTime)						{ *putEndTime = utPadEndTime; }

	return ERR_OK;
}

