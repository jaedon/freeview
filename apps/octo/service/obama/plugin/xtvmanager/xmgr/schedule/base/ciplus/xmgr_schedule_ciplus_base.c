/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_schedule_ciplus_base.c
	@brief

	Description:  									\n
	Module: OBAMA / plugin / tvmanager / schedule	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding �� ����ϴ� source file �Դϴ�.
* MS949 encoding�����Ī��� source file�Ǫ���
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

#if defined(CONFIG_MW_CI_PLUS)
/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <sapi.h>
#include <papi.h>
#include <octo_common.h>
#include <otl.h>

#include <db_svc.h>
#include <db_param.h>

#include <svc_fs.h>
#include <svc_pipe.h>
#include <svc_ringbuf.h>
#include <svc_mediaserver.h>
#include <svc_output.h>
#include <svc_sys.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_schedule.h>
#include <mgr_live.h>
#include <mgr_pvr_action.h>
#include <mgr_satip.h>
#include <mgr_ntls.h>
#include <mgr_epg.h>

#include "../../local_include/_xmgr_schedule.h"

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


STATIC HERROR xmgr_schedule_CiPlusRecordDescRecordCheckSchedule_Base(HUINT32 ulStartLimit, HUINT32 ulEndLimit)
{
	HUINT32			ulCnt, ulNumSch = 0;
	DxSchedule_t	*pstSchArray = NULL;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();

	// ��ü �������� ã�Ƽ�...1�� - 5�� ���̿� �������� �����ϴ��� ã�´�.
	// �������� �����ϸ� FAIL����...ã�� �� �����ص� FAIL����...
	(void)SAPI_CountSchedule(&ulNumSch);
	if (0 == ulNumSch)
	{
		HxLOG_Error("[%s:%d] No Schedule: Nothing to stop\n", __FUNCTION__, __LINE__);
		hErr = ERR_OK;
		goto END_FUNC;
	}

	pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
	if (NULL == pstSchArray)
	{
		HxLOG_Error("[%s:%d] Memory allocation failed\n", __FUNCTION__, __LINE__);
		goto END_FUNC;
	}

	hErr = SAPI_GetAllSchedules(ulNumSch, NULL, pstSchArray);
	if(hErr == ERR_OK)
	{
		for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
		{
			DxSchedule_t	*pstSchedule = &(pstSchArray[ulCnt]);
			HUINT32			ulSchStartTime, ulSchEndTime;
			if(HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstSchedule->stStartTime, &ulSchStartTime)==ERR_OK)
			{
				if(ulSchStartTime > ulEndLimit)
				{
					// 5������ �����ϴ� �������� ����...
					continue;
				}
			}

			ulSchEndTime = ulSchStartTime + pstSchedule->ulDuration;
			if(ulSchEndTime <= ulStartLimit)
			{
				// 1�� ������ �����ϴ� �������� ����...
				continue;
			}

			if(pstSchedule->eRsvType != DxRSVTYPE_IP_EPG_GATHERING
				&& pstSchedule->eRsvType != DxRSVTYPE_DESC_REC_STANDBY)
			{
				// IPEPG�� ������� ��....�������� �ڱ��ڽ��� ��...
				HxLOG_Error("[%s:%d] Found Schedule between %d and %d\n", __FUNCTION__, __LINE__, ulStartLimit, ulEndLimit);
				goto END_FUNC;
			}
		}
	}


	hErr = ERR_OK;

END_FUNC:


	if (pstSchArray)
	{
		HLIB_STD_MemFree(pstSchArray);
	}

	return hErr;
}

STATIC void xmgr_schedule_CiPlusRecordDescRecordDataReceived_Base (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxRecListData_t	*pstRecData = (DxRecListData_t *)userdata;
	DAxIterator_t	stIter;
	HBOOL			bFound = FALSE;

	HxLOG_Trace();

	if (size == 0)
	{
		HxLOG_Error("size is 0 \n");
		return;
	}

	DAPI_InitIterator(&stIter, data, size, 0);
	while (DAPI_NextIterator(&stIter))
	{
		DxRecListData_t 	*pstTempRecData = (DxRecListData_t *)stIter.data;
		// �⺻������ ���� ������ ���� ������ Sorting...
		if(pstTempRecData)
		{
			// �켱���� 1����....��û�� ���� ���� ����...�� ? �÷����� �� �ִ� �Ŷ� �ٸ��ǰ�...
			HxLOG_Debug("[%s:%d] Priority 1 ... (%s)\n", __FUNCTION__, __LINE__, pstTempRecData->szName);
			if(pstTempRecData->bScrambled == TRUE && pstTempRecData->bPlayed == FALSE)
			{
				HxSTD_MemSet(pstRecData, 0, sizeof(DxRecListData_t));
				HxSTD_MemCopy(pstRecData, pstTempRecData, sizeof(DxRecListData_t));
				bFound = TRUE;
				break;
			}
		}
	}

	if(bFound ==FALSE)
	{
		DAPI_InitIterator(&stIter, data, size, 0);
		while (DAPI_NextIterator(&stIter))
		{
			DxRecListData_t 	*pstTempRecData = (DxRecListData_t *)stIter.data;
			// �⺻������ ���� ������ ���� ������ Sorting...
			if(pstTempRecData)
			{
				// 1������ ������ Case�� �ϴ�
				HxLOG_Debug("[%s:%d] Priority 2 ... (%s)\n", __FUNCTION__, __LINE__, pstTempRecData->szName);
				if(pstTempRecData->bScrambled == TRUE && pstTempRecData->bPlayed == TRUE)
				{
					HxSTD_MemSet(pstRecData, 0, sizeof(DxRecListData_t));
					HxSTD_MemCopy(pstRecData, pstTempRecData, sizeof(DxRecListData_t));
					break;
				}
			}
		}
	}
}

STATIC void xmgr_schedule_CiPlusBatchDescRecordDataReceived_Base (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxRecListData_t	*pstRecData = (DxRecListData_t *)userdata;
	DAxIterator_t	stIter;
	HBOOL			bFound = FALSE;

	HxLOG_Trace();

	if (size == 0)
	{
		HxLOG_Error("size is 0 \n");
		return;
	}

	DAPI_InitIterator(&stIter, data, size, 0);
	while (DAPI_NextIterator(&stIter))
	{
		DxRecListData_t 	*pstTempRecData = (DxRecListData_t *)stIter.data;
		// �⺻������ ���� ������ ���� ������ Sorting...
		if(pstTempRecData)
		{
			// �켱���� 1����....��û�� ���� ���� ����...�� ? �÷����� �� �ִ� �Ŷ� �ٸ��ǰ�...
			HxLOG_Debug("Priority 1 ... (%s)\n", pstTempRecData->szName);
			if(pstTempRecData->bScrambled == TRUE && pstTempRecData->bPlayed == FALSE)
			{
				HxSTD_MemSet(pstRecData, 0, sizeof(DxRecListData_t));
				HxSTD_MemCopy(pstRecData, pstTempRecData, sizeof(DxRecListData_t));
				bFound = TRUE;
				break;
			}
		}
	}

	if(bFound ==FALSE)
	{
		DAPI_InitIterator(&stIter, data, size, 0);
		while (DAPI_NextIterator(&stIter))
		{
			DxRecListData_t 	*pstTempRecData = (DxRecListData_t *)stIter.data;
			// �⺻������ ���� ������ ���� ������ Sorting...
			if(pstTempRecData)
			{
				// 1������ ������ Case�� �ϴ�
				HxLOG_Debug("Priority 2 ... (%s)\n", pstTempRecData->szName);
				if(pstTempRecData->bScrambled == TRUE && pstTempRecData->bPlayed == TRUE)
				{
					HxSTD_MemSet(pstRecData, 0, sizeof(DxRecListData_t));
					HxSTD_MemCopy(pstRecData, pstTempRecData, sizeof(DxRecListData_t));
					break;
				}
			}
		}
	}
}

STATIC HERROR xmgr_schedule_CiPlusBatchDescRecordMakeRsvItem_Base (HUINT16 ulSvcuid, HxDATETIME_t *pstStartTime, UNIXTIME utDuration)
{
	DxSchedule_t	stSchedule = {0, };
	HERROR			hErr = ERR_FAIL;
	Handle_t 		hSvc;
	SAPI_Conflict_t	stConflict;

	VK_TASK_Sleep(500);

	if(pstStartTime==NULL)
	{
		HxLOG_Error("Can not found Time to reserve.\n");
		return ERR_FAIL;
	}

	if(utDuration>DESC_REC_AVAILABLE_TIME)
	{
		// 4�ð��̻��� ���� �� ������...Ȥ�ó�...���߿� ��Ȳ�� ���� �� �����Ƿ�...
		HxLOG_Error("This file is over than 4 hours.\n");
		return ERR_FAIL;
	}

	VK_memset(&stSchedule, 0, sizeof(DxSchedule_t) );
	HxSTD_MemCopy(&stSchedule.stStartTime, pstStartTime, sizeof(HxDATETIME_t));

	hErr = SAPI_GetAvailableSlot(&stSchedule.ulSlot);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("There is no empty slot for sama!\n");
		return ERR_FAIL;
	}
	HxLOG_Debug("Available Slot ... (%d)\n", stSchedule.ulSlot);

	HxLOG_Debug("ulSvcuid (%d) \n", ulSvcuid);

	stSchedule.eRsvType 		= DxRSVTYPE_DESC_REC_STANDBY;
	hErr = DB_SVC_GetServiceHandleBySvcIdx(ulSvcuid, &hSvc);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("DB_SVC_GetServiceHandleBySvcIdx Error !\n" );
		return ERR_FAIL;
	}
	stSchedule.ulSvcUid			= DB_SVC_GetServiceUID(hSvc);
	stSchedule.eRsvReady		= DxRSVREADY_30_SEC;
	stSchedule.ulDuration		= utDuration;
	stSchedule.eRepeat			= DxRSVREPEAT_ONCE;
	stSchedule.ulRepeatDays 	= 0;
	stSchedule.eStatus			= DxRSVSTATUS_WAITING;
	stSchedule.bExceptConflict	= FALSE;
	stSchedule.eEpgType 		= eDxEPG_TYPE_SC;

	HxLOG_Debug("on Solt: %d,  SVC UID: 0x%x\n",  stSchedule.ulSlot, stSchedule.ulSvcUid);

	hErr = SAPI_MakeSchedule (stSchedule.ulSlot, &stSchedule, &stConflict);

	return ERR_OK;
}

STATIC HERROR xmgr_schedule_CiPlusBatchDescRecordCheckSchedule_Base(HUINT32 ulStartLimit, HUINT32 ulEndLimit)
{
	HUINT32			ulCnt, ulNumSch = 0;
	DxSchedule_t	*pstSchArray = NULL;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();

	// ��ü �������� ã�Ƽ�...1�� - 5�� ���̿� �������� �����ϴ��� ã�´�.
	// �������� �����ϸ� FAIL����...ã�� �� �����ص� FAIL����...
	(void)SAPI_CountSchedule(&ulNumSch);
	if (0 == ulNumSch)
	{
		HxLOG_Error("No Schedule: Nothing to stop\n");
		return ERR_OK;
	}

	pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
	if (NULL == pstSchArray)
	{
		HxLOG_Error("Memory allocation failed\n");
		goto END_FUNC;
	}

	hErr = SAPI_GetAllSchedules(ulNumSch, NULL, pstSchArray);
	if(hErr == ERR_OK)
	{
		for (ulCnt = 0; ulCnt < ulNumSch; ulCnt++)
		{
			DxSchedule_t	*pstSchedule = &(pstSchArray[ulCnt]);
			HUINT32			ulSchStartTime, ulSchEndTime;
			if(HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstSchedule->stStartTime, &ulSchStartTime)==ERR_OK)
			{
				if(ulSchStartTime > ulEndLimit)
				{
					// 5������ �����ϴ� �������� ����...
					continue;
				}
			}

			ulSchEndTime = ulSchStartTime + pstSchedule->ulDuration;
			if(ulSchEndTime <= ulStartLimit)
			{
				// 1�� ������ �����ϴ� �������� ����...
				continue;
			}

			if(pstSchedule->eRsvType == DxRSVTYPE_DESC_REC_STANDBY)
			{
				// ���� ���� ������ �ʰ�����...
				SAPI_CancelSchedule(pstSchedule->ulSlot);
				continue;
			}
			else if(pstSchedule->eRsvType != DxRSVTYPE_IP_EPG_GATHERING)
			{
				// IPEPG�� ������� ��....
				HxLOG_Error("Found Schedule between %d and %d\n", ulStartLimit, ulEndLimit);
				hErr = ERR_FAIL;
				goto END_FUNC;
			}
		}
	}


	hErr = ERR_OK;

END_FUNC:


	if (pstSchArray)
	{
		HLIB_STD_MemFree(pstSchArray);
	}

	return hErr;
}

#define ___GLOBAL_FUNCTIONS___

HERROR  xmgr_schedule_CiPlusStartDescRecordFinedItem_Base(DxRecListData_t	*pstRecData)
{
	DxRecListData_t	*pstTmpRecData = NULL;
	HCHAR			query[256];
	HUINT32 		ulReqId;
	HUINT32			ulCurTime = 0,ulLimitTime = 0, ulLimitDuration = 0;
	HxDATETIME_t	stCurtTime,	stLimitTime;

	HxLOG_Trace();

	if(pstRecData == NULL)
	{
		HxLOG_Error("[%s:%d] pstRecData is NULL. \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	// 1. ����ð� ���ϱ�...
	ulCurTime = HLIB_STD_GetSystemTime();
	if(HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurTime, &stCurtTime) == ERR_FAIL)
	{
		HxLOG_Error("[%s:%d] Fail to Convert Current Time.\n",  __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	// 2. Descramble Recording���ѽð� ���ϱ�...(���� 5��)
	stLimitTime.stDate.usYear			= stCurtTime.stDate.usYear;
	stLimitTime.stDate.ucMonth			= stCurtTime.stDate.ucMonth;
	stLimitTime.stDate.ucDay			= stCurtTime.stDate.ucDay;
	stLimitTime.stTime.ucHour			= DESC_REC_END_TIME;
	stLimitTime.stTime.ucMinute 		= 0;
	stLimitTime.stTime.ucSecond 		= 0;
	stLimitTime.stTime.usMillisecond	= 0;

	if(HLIB_DATETIME_ConvertDateTimeToUnixTime(&stLimitTime, &ulLimitTime)==ERR_FAIL)
	{
		HxLOG_Error("[%s:%d] Fail to Convert Limit Time.\n", __FUNCTION__, __LINE__);\
		return ERR_FAIL;
	}
	//  1:00:00 - 4:59:59
	if(stCurtTime.stTime.ucHour >= DESC_REC_START_TIME && stCurtTime.stTime.ucHour < DESC_REC_END_TIME)
	{
		// (1�� - 5��... ���� �ؾ��Ѵٸ� 5�� - ����ð� ��ŭ ����...)
		ulLimitDuration = ulLimitTime - ulCurTime;

		if(xmgr_schedule_CiPlusRecordDescRecordCheckSchedule_Base(ulCurTime, ulLimitTime)==ERR_OK)
		{
			// ���� ������ ���Ϻ���...��ȭ�Ѵ�.... 1�ÿ��� 5�ÿ� Descramble ������...
			HxSTD_PrintToStrN(query, 256
				, "SELECT * FROM %s WHERE duration <= %d ORDER BY starttime ASC"
				, DxNAMEOF(DxRecListData_t)
				, ulLimitDuration
			);
			HxLOG_Debug("[%s:%d] Query = %s\n", __FUNCTION__, __LINE__, query);

			pstTmpRecData = (DxRecListData_t *)HLIB_STD_MemAlloc(sizeof(DxRecListData_t));
			if (pstTmpRecData == NULL)
			{
				HxLOG_Error("Memory allocation failed\n");
				return ERR_FAIL;
			}

			HxSTD_MemSet(pstTmpRecData, 0, sizeof(DxRecListData_t));

			//Descramble �� ���� ����....
			ulReqId = DAPI_Query(query, (DAPI_Getter)xmgr_schedule_CiPlusRecordDescRecordDataReceived_Base, (void *)pstTmpRecData);
			DAPI_Wait(3000, ulReqId);
			if (ulReqId != 0 && pstTmpRecData->ulRecDuration > 0)
			{
				// ���� ������ �������� ã�� ���... ���� �ɰ� ������ �ȴ�....
				HxSTD_MemCopy(pstRecData, pstTmpRecData, sizeof(DxRecListData_t));
				if(pstTmpRecData != NULL)
				{
					HLIB_STD_MemFree(pstTmpRecData);
					pstTmpRecData = NULL;
				}
				return ERR_OK;
			}
		}
	}

	if(pstTmpRecData != NULL)
	{
		HLIB_STD_MemFree(pstTmpRecData);
		pstTmpRecData = NULL;
	}
	return ERR_FAIL;
}



// TODO: xmgr_action_BatchDescRecordProcessDoProcess �� ������ �Űܿ� ����.
// TODO: new batch ����������, dapi_wait(), sleep �� �� ����������. ���� �� ��� �� ��Ȳ�̶�� ���� task �ξ����.
// TODO: new batch�� batch�� �޸� ��� proc���� ���ÿ� start �Ǵ� �����̱� ������, obama main task�� ����� ���� �ȵȴ�.
HERROR	xmgr_schedule_CiPlusRescheduleDescrambleRecordBatch_Base()
{
	DxRecListData_t *pstRecData = NULL;
	HCHAR			query[256];
	HUINT32 		ulReqId;
	HUINT32 		ulCurTime = 0,ulLimitTime = 0, ulStartTime = 0, ulLimitDuration = 0, ulTimeGap;
	HxDATETIME_t	stCurtTime, stLimitTime, stStartTime;
	HBOOL			bDescrambleOnOff = FALSE;
	HUINT32 		ulCnt = 0, ulNumSch = 0, ulNumSch2 = 0;
	DxSchedule_t	*pstSchArray = NULL;
	HERROR			hErr = ERR_FAIL;

	HxLOG_Trace();

	if(DB_PARAM_GetItem(eDB_PARAM_ITEM_DESCRAMBLEONOFF, (HUINT32 *)&bDescrambleOnOff, 0)==ERR_FAIL)
	{
		HxLOG_Error("Fail to Get DB.\n");
		return ERR_FAIL;
	}

	if(bDescrambleOnOff == FALSE)
	{
		HxLOG_Error("This Action is not available by user select.\n");
		(void)SAPI_CountScheduleByType(DxRSVTYPE_DESC_REC_STANDBY, &ulNumSch);
		if (0 == ulNumSch)
		{
			HxLOG_Error("No Schedule: Nothing to stop\n");
			return ERR_FAIL;
		}

		pstSchArray = (DxSchedule_t *)HLIB_STD_MemAlloc(sizeof(DxSchedule_t) * ulNumSch);
		if (NULL == pstSchArray)
		{
			HxLOG_Error("Memory allocation failed\n");
			return ERR_FAIL;

		}

		hErr = SAPI_GetAllSchedulesByRsvType(ulNumSch, &ulNumSch2, pstSchArray, DxRSVTYPE_DESC_REC_STANDBY);
		if(hErr == ERR_OK)
		{
			for (ulCnt = 0; ulCnt < ulNumSch2; ulCnt++)
			{
				DxSchedule_t	*pstSchedule = &(pstSchArray[ulCnt]);
				if(pstSchedule != NULL && pstSchedule->eRsvType == DxRSVTYPE_DESC_REC_STANDBY)
				{
					SAPI_CancelSchedule(pstSchedule->ulSlot);
				}
				continue;
			}
		}
		HLIB_STD_MemFree(pstSchArray);

		return ERR_FAIL;

	}

	// 1. ����ð� ���ϱ�...
	ulCurTime = HLIB_STD_GetSystemTime();
	if(HLIB_DATETIME_ConvertUnixTimeToDateTime(ulCurTime, &stCurtTime) == ERR_FAIL)
	{
		HxLOG_Error("Fail to Convert Current Time.\n");
		return ERR_FAIL;

	}



	// 2. Descramble Recording���ѽð� ���ϱ�...(���� 5��)
	stLimitTime.stDate.usYear			= stCurtTime.stDate.usYear;
	stLimitTime.stDate.ucMonth			= stCurtTime.stDate.ucMonth;
	stLimitTime.stDate.ucDay			= stCurtTime.stDate.ucDay;
	stLimitTime.stTime.ucHour			= DESC_REC_END_TIME;
	stLimitTime.stTime.ucMinute 		= 0;
	stLimitTime.stTime.ucSecond 		= 0;
	stLimitTime.stTime.usMillisecond	= 0;

	if(HLIB_DATETIME_ConvertDateTimeToUnixTime(&stLimitTime, &ulLimitTime)==ERR_FAIL)
	{
		HxLOG_Error("Fail to Convert Limit Time.\n");
		return ERR_FAIL;
	}

	if(stCurtTime.stTime.ucHour >= DESC_REC_START_TIME && stCurtTime.stTime.ucHour < DESC_REC_END_TIME)
	{
		// (1�� - 5��... ���� �ؾ��Ѵٸ� 5�� - ����ð� ��ŭ ����...)
		ulLimitDuration = ulLimitTime - ulCurTime;

		if(xmgr_schedule_CiPlusBatchDescRecordCheckSchedule_Base(ulCurTime, ulLimitTime)==ERR_OK)
		{
			// ���� ������ ���Ϻ���...��ȭ�Ѵ�.... 1�ÿ��� 5�ÿ� Descramble ������...
			HxSTD_PrintToStrN(query, 256
				, "SELECT * FROM %s WHERE duration <= %d ORDER BY starttime ASC"
				, DxNAMEOF(DxRecListData_t)
				, ulLimitDuration
			);
			HxLOG_Debug("Query = %s\n", query);

			pstRecData = (DxRecListData_t *)HLIB_STD_MemAlloc(sizeof(DxRecListData_t));
			if (pstRecData == NULL)
			{
				HxLOG_Error("Memory allocation failed\n");
				return ERR_FAIL;
			}
			HxSTD_MemSet(pstRecData, 0, sizeof(DxRecListData_t));

			//Descramble �� ���� ����....
			ulReqId = DAPI_Query(query, (DAPI_Getter)xmgr_schedule_CiPlusBatchDescRecordDataReceived_Base, (void *)pstRecData);
			DAPI_Wait(3000, ulReqId);
			if (ulReqId != 0 && pstRecData->ulRecDuration > 0)
			{
				// ���� ������ �������� ã�� ���... ���� �ɰ� ������ �ȴ�....
				xmgr_schedule_CiPlusBatchDescRecordMakeRsvItem_Base((HUINT16)(pstRecData->svcUId & 0xFFFF), &stCurtTime, pstRecData->ulRecDuration);
				if(pstRecData != NULL)
				{
					HLIB_STD_MemFree(pstRecData);
					pstRecData = NULL;
				}

				return ERR_FAIL;
			}

			if(pstRecData != NULL)
			{
				HLIB_STD_MemFree(pstRecData);
				pstRecData = NULL;
			}
		}
	}

	ulLimitDuration = DESC_REC_AVAILABLE_TIME;

	// ������ ���۵� �ð��� ���Ѵ�.
	if(stCurtTime.stTime.ucHour < DESC_REC_START_TIME)
	{
		// 1�ú��ʹ� ������ ������ �ϳ��� Ǫ�� ���� ����...
		stStartTime.stDate.usYear			= stCurtTime.stDate.usYear;
		stStartTime.stDate.ucMonth			= stCurtTime.stDate.ucMonth;
		stStartTime.stDate.ucDay			= stCurtTime.stDate.ucDay;
		stStartTime.stTime.ucHour			= DESC_REC_START_TIME;
		stStartTime.stTime.ucMinute 		= 0;
		stStartTime.stTime.ucSecond 		= 0;
		stStartTime.stTime.usMillisecond	= 0;
	}
	else
	{
		// �������� ���� ���ϴ� ��Ȳ ...
		// 5������ ��Ǫ�� ���� ������ �ѽ÷�...
		HLIB_DATETIME_GetNextDay(ulCurTime, &ulStartTime, &ulTimeGap);
		if(HLIB_DATETIME_ConvertUnixTimeToDateTime(ulStartTime, &stStartTime) == ERR_FAIL)
		{
			HxLOG_Error("Fail to Convert Next Time.\n");
			return ERR_FAIL;
		}
		stStartTime.stTime.ucHour			= DESC_REC_START_TIME;
		stStartTime.stTime.ucMinute 		= 0;
		stStartTime.stTime.ucSecond 		= 0;
		stStartTime.stTime.usMillisecond	= 0;

		HLIB_DATETIME_GetNextDay(ulLimitTime, &ulLimitTime, &ulTimeGap);
	}

	HLIB_DATETIME_ConvertDateTimeToUnixTime(&stStartTime, &ulStartTime);

	HxLOG_Debug("START Time : %s\n", HLIB_DATETIME_UnixTimeToString(ulStartTime, NULL, 0));

	if(xmgr_schedule_CiPlusBatchDescRecordCheckSchedule_Base(ulStartTime, ulLimitTime)==ERR_FAIL)
	{
		// �������� ã�ų�, ã�� ������ ������ ����� �ϴ� ��ȿ...
		return ERR_FAIL;
	}

	// ���� ������ ���Ϻ���...��ȭ�Ѵ�.... 1�ÿ��� 5�ÿ� Descramble ������...
	HxSTD_PrintToStrN(query, 256
		, "SELECT * FROM %s WHERE duration <= %d ORDER BY starttime ASC"
		, DxNAMEOF(DxRecListData_t)
		, ulLimitDuration
	);
	HxLOG_Debug("Query = %s\n", query);

	pstRecData = (DxRecListData_t *)HLIB_STD_MemAlloc(sizeof(DxRecListData_t));
	if (pstRecData == NULL)
	{
		HxLOG_Error("Memory allocation failed\n");
		return ERR_FAIL;
	}

	HxSTD_MemSet(pstRecData, 0, sizeof(DxRecListData_t));

	//Descramble �� ���� ����....
	ulReqId = DAPI_Query(query, (DAPI_Getter)xmgr_schedule_CiPlusBatchDescRecordDataReceived_Base, (void *)pstRecData);
	DAPI_Wait(3000, ulReqId);
	if (ulReqId== 0)
	{
		HxLOG_Error("Query FAIL \n");

		if(pstRecData != NULL)
		{
			HLIB_STD_MemFree(pstRecData);
			pstRecData = NULL;
		}
		return ERR_FAIL;
	}

	if(pstRecData->ulRecDuration == 0)
	{
		// ��ȭ���� ���� ��쳪 ã�� ������ ���� ���...
		HxLOG_Error("There is no file to descramble.\n");
		if(pstRecData != NULL)
		{
			HLIB_STD_MemFree(pstRecData);
			pstRecData = NULL;
		}
		return ERR_FAIL;
	}
	// ������ ���Ͽ� ���� ����...

	HxLOG_Debug("Name : %s(Duration: %d) %s%s\n", pstRecData->szName, pstRecData->ulRecDuration, pstRecData->stMeta.szPath, pstRecData->stMeta.szFileName);
	// �ϴ� ������..1�ÿ��� 5�÷� �ɰ�...
	xmgr_schedule_CiPlusBatchDescRecordMakeRsvItem_Base((HUINT16)(pstRecData->svcUId & 0xFFFF), &stStartTime, pstRecData->ulRecDuration);

	if(pstRecData != NULL)
	{
		HLIB_STD_MemFree(pstRecData);
		pstRecData = NULL;
	}

	return ERR_OK;
}

#define ___MESSAGE_FUNCTIONS___











#define _____DEBUG_FUNCTION_____

#endif
