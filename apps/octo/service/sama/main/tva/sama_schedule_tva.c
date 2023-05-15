/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_schedule_tva.c
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

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <vkernel.h>
#include <hlib.h>
#include <dapi.h>
#include <dlib.h>
#include <oapi_types.h>
#include <sapi.h>
#include <hapi.h>

#include "sama_int.h"
#include <sama_lib.h>

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/
#define KEYWORD_CRID_PROGRAM 		"pCRID:"
#define KEYWORD_CRID_SERIES 		"sCRID:"
#define KEYWORD_CRID_DLIMITER		","

//#define TEST_NORDIG_ACCURATE_RECORD 0

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef	struct
{
	DxSchedule_t		*pstSchedule;
	HINT32				p1;
	HINT32				p2;
} SamaTvAnyTime_InternalMsg_t;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

static	HBOOL	s_bIsRecordingsLoaded = FALSE;

#define ___LOCAL_DEBUG_FUNCTIONS___
#if defined(CONFIG_DEBUG)
STATIC void	check_alt_slots(SAPI_AlternativeItems_t *pstAltItems)
{
	HUINT32 i,j;
	HUINT32 ulSlot_i, ulSlot_j;
	SERIES_SET *pstSeriesSet_i = NULL, *pstSeriesSet_j= NULL;

	for (i=0; i<pstAltItems->ulNumAlternatives; i++)
	{
		ulSlot_i = pstAltItems->astAltItemArray[i].ulSlot;
		pstSeriesSet_i = SAMALIB_TVA_GetSeriesSetBySlotId(ulSlot_i);
		if (pstSeriesSet_i == NULL)
		{
			HxLOG_Debug("SAMALIB_TVA_GetSeriesSetBySlotId failed. slot=(%d) \n", ulSlot_i);
			continue;
		}
		for (j=i; j<pstAltItems->ulNumAlternatives; j++)
		{
			ulSlot_j = pstAltItems->astAltItemArray[j].ulSlot;
			pstSeriesSet_j = SAMALIB_TVA_GetSeriesSetBySlotId(ulSlot_j);
			if (pstSeriesSet_j == NULL)
			{
				HxLOG_Debug("SAMALIB_TVA_GetSeriesSetBySlotId failed. slot=(%d) \n", ulSlot_j);
				continue;
			}
			if (pstSeriesSet_i == pstSeriesSet_j)
			{
				HxLOG_Debug("same series: slot (%d) = slot (%d) \n",ulSlot_i,ulSlot_j );
			}
		}
	}
}
#endif

#define ___LOCAL_FUNCTIONS___
STATIC HERROR	sama_sch_tva_UpdateSeriesSet(SERIES_SET *pstSeriesSet, SAMA_Context_t *pstContext);

STATIC void sama_sch_tva_DeleteEvent_CB (void *p)
{
	DxEVENT_Delete((DxEvent_t *)p);
}

STATIC void sama_sch_tva_ExceptConflictItems (HINT32 *nNumConflict, SAPI_ConflictItem_t *pstConfArray, SAPI_AlternativeItems_t *pstAltItems)
{
	HINT32 i,j,k;
	HUINT32	ulConflictSlot;
	HUINT32	ulAlternativeSlot;

	HUINT32	ulNewNumConflict;

	ulNewNumConflict = *nNumConflict;

	// alternative�� �ִ� slot�� conflict���� �������ش�.
	for (i=*nNumConflict-1; i>=0; i--)
	{
		ulConflictSlot = pstConfArray[i].ulSlot;
		for (j=0; j<pstAltItems->ulNumAlternatives; j++)
		{
			ulAlternativeSlot = pstAltItems->astAltItemArray[j].ulSlot;
			if (ulConflictSlot == ulAlternativeSlot)
			{
				ulNewNumConflict -= 1;
				for (k=i; k<*nNumConflict-1; k++)
				{
					pstConfArray[k].ulSlot = pstConfArray[k+1].ulSlot;
				}
				break;
			}
		}
	}

	*nNumConflict = ulNewNumConflict;
}

STATIC HERROR sama_sch_tva_GetScheduledSlot (HUINT32 ulSvcUid, HUINT32 ulEventId, HUINT32 *pulSlot)
{
	HxTREE_t		*pstScheduleTree;
	void			*pvIter;
	DxSchedule_t	*pstSchedule;

	pstScheduleTree = SAMA_SCHEDULE_GetScheduleTree();
	for (pvIter = HLIB_TREE_Iterator(pstScheduleTree); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		pstSchedule = (DxSchedule_t*)HLIB_TREE_IteratorValue(pvIter);
		if (pstSchedule == NULL )
			continue;

		if (pstSchedule->ulSvcUid == ulSvcUid
			&& pstSchedule->uExtInfo.stRec.nEvtId == ulEventId)
		{
			*pulSlot = pstSchedule->ulSlot;
			return ERR_OK;
		}

	}

	return ERR_FAIL;
}
STATIC HERROR sama_sch_tva_AddRecordedCrid (DxRecListData_t *pstRecInfo)
{
	HERROR			hErr = ERR_FAIL;
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	DxPVR_TvaCrid_t *pstCrids;
	HUINT32			i;
	HUINT32			ulSeriesCridIndex = DxMAX_CRID_DATA_NUM;
	SERIES_SET		*pstSeriesSet = NULL;

	if ( pstRecInfo == NULL )
	{
		HxLOG_Error("Invalid param. null rec info\n");
		return ERR_FAIL;
	}
	pstCrids = pstRecInfo->stMeta.astTvaCrids;

	for ( i = 0; i<DxMAX_CRID_DATA_NUM; i++ )
	{
		if ( pstCrids[i].ucType == 0 )
		{
			continue;
		}

		HxLOG_Debug("series recorded. name=[%s], cridtype=(%d), crid=[%s]\n",pstRecInfo->szName, pstCrids[i].ucType, pstCrids[i].szCrid);

		if ( pstCrids[i].ucType == SAMA_CRID_TYPE_SERIES )
		{
			ulSeriesCridIndex = i;
			break;
		}
	}

	// if didn't found series CRID
	if ( ulSeriesCridIndex >= DxMAX_CRID_DATA_NUM )
	{
		return ERR_FAIL;
	}

	pstSeriesSet = SAMALIB_TVA_GetSeriesSetByCRID(pstCrids[ulSeriesCridIndex].szCrid,SAMA_CRID_TYPE_SERIES, 0);
	if ( pstSeriesSet == NULL )
	{
		return ERR_FAIL;
	}

	for ( i = 0; i<DxMAX_CRID_DATA_NUM; i++ )
	{
		TVA_CONTENTID_T		stProgCrid;

		if ( pstCrids[i].ucType != SAMA_CRID_TYPE_PROG )
		{
			continue;
		}

		SAMALIB_TVA_ExtractEachPartCRID(pstCrids[i].szCrid
										, HLIB_STD_StrNLen(pstCrids[i].szCrid,DxPVR_TVANYTIME_CRID_TOTAL_LEN)
										,&stProgCrid , NULL );
		stProgCrid.type = SAMA_CRID_TYPE_PROG;
		hErr = SAMALIB_TVA_AddRecordedCridToSeriesSet(pstSeriesSet, &stProgCrid);
	}
#endif
	return hErr;
}

STATIC void sama_sch_tva_OnDamaRecordedProgReceived(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxRecListData_t		*pstRecInfo;
	DAxIterator_t		iter;
	HERROR				hErr = ERR_FAIL;

	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		pstRecInfo = (DxRecListData_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (pstRecInfo == NULL)
		{
			HxLOG_Debug("null recorded \n");
			DLIB_FreeType(iter.name, pstRecInfo);
			continue;
		}
		hErr = sama_sch_tva_AddRecordedCrid(pstRecInfo);
		(void)hErr;

		DLIB_FreeType(iter.name, pstRecInfo);
	}

	s_bIsRecordingsLoaded = TRUE;

	return;
}

STATIC void sama_sch_tva_OnDamaTrackingEventReceived(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxEvent_t			*pstEvent;
	DAxIterator_t		iter;

	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		pstEvent = (DxEvent_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (pstEvent == NULL)
		{
			HxLOG_Debug("null recorded \n");
			DLIB_FreeType(iter.name, pstEvent);
			continue;
		}
		HxLOG_Debug("event name: [%s][%s], running status: (%d) \n",DLIB_ConvertEnum2Str(DxNAMEOF(DxEPG_Type_e), pstEvent->type), SAMA_EVENT_GETNAME(pstEvent->name), pstEvent->runStatus);

		(void)SAMALIB_TRACKING_TrackEvent(pstEvent);

		DLIB_FreeType(iter.name, pstEvent);
	}

	return;
}

STATIC void sama_sch_tva_OnDamaChangedEventsReceived(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxEvent_t			*pstEvent;
	DAxIterator_t		iter;
	DxSchedule_t		*pstSchedule;
	HUINT32				ulSlot;
	SERIES_SET			*pstSeriesSet;
	HERROR				hErr;
	UNIXTIME			utStartTime;

	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		pstEvent = (DxEvent_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (pstEvent == NULL)
		{
			HxLOG_Debug("null recorded \n");
			DLIB_FreeType(iter.name, pstEvent);
			continue;
		}

		HxLOG_Debug("type[%s] event name: [%s], running status: (%d) \n",DLIB_ConvertEnum2Str(DxNAMEOF(DxEPG_Type_e), pstEvent->type),SAMA_EVENT_GETNAME(pstEvent->name), pstEvent->runStatus);
		hErr = sama_sch_tva_GetScheduledSlot(pstEvent->svcuid, pstEvent->eventId, &ulSlot);
		if (hErr != ERR_OK)
		{
			DLIB_FreeType(iter.name,pstEvent);
			continue;
		}

		pstSeriesSet = SAMALIB_TVA_GetTrackingSeriesSetBySlotId(ulSlot);
		if (pstSeriesSet != NULL)
		{
			HxLOG_Debug(HxANSI_COLOR_GREEN(" Ignore series tracking event. \n"));
			DLIB_FreeType(iter.name,pstEvent);
			continue;
		}

		pstSeriesSet = SAMALIB_TVA_GetSeriesSetBySlotId(ulSlot);
		// if not TVA series, then only update schedule
		if (pstSeriesSet == NULL )
		{
			pstSchedule = SAMA_SCHEDULE_GetSchedule(ulSlot);
			if (pstSchedule==NULL)
			{
				DLIB_FreeType(iter.name, pstEvent);
				continue;
			}

			if (pstSchedule->eStatus == DxRSVSTATUS_RUNNING)
			{
				HxLOG_Debug(HxANSI_COLOR_GREEN("Do not Update schedule while recording. \n"));
				DLIB_FreeType(iter.name, pstEvent);
				continue;
			}

			HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstSchedule->stStartTime,&utStartTime);
			if (utStartTime != pstEvent->startTime
				|| pstSchedule->ulDuration != pstEvent->duration)
			{
				{
					HxDATETIME_t	stStartTime, stEndTime;
					UNIXTIME		utStartTime, utEndTime;
					HxLOG_Debug("\033[30;47m[%s:%d] Update schedule. slot=(%d) \033[0m \n",__FUNCTION__, __LINE__,pstSchedule->ulSlot);

					HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstSchedule->stStartTime,&utStartTime);
					utEndTime = utStartTime + pstSchedule->ulDuration;
					HLIB_DATETIME_ConvertUnixTimeToDateTime(utEndTime,&stEndTime);
					HxLOG_Print(" Before %d/%d/%d (%02d:%02d:%02d ~ %02d:%02d:%02d) (ux:%d~%d)\n",
							pstSchedule->stStartTime.stDate.usYear, pstSchedule->stStartTime.stDate.ucMonth, pstSchedule->stStartTime.stDate.ucDay,
							pstSchedule->stStartTime.stTime.ucHour, pstSchedule->stStartTime.stTime.ucMinute, pstSchedule->stStartTime.stTime.ucSecond,
							stEndTime.stTime.ucHour, stEndTime.stTime.ucMinute, stEndTime.stTime.ucSecond, utStartTime, utEndTime);
					HLIB_DATETIME_ConvertUnixTimeToDateTime(pstEvent->startTime,&stStartTime);
					HLIB_DATETIME_ConvertUnixTimeToDateTime(pstEvent->startTime+pstEvent->duration,&stEndTime);
					HxLOG_Print("  After %d/%d/%d (%02d:%02d:%02d ~ %02d:%02d:%02d) (ux:%d~%d)\n",
							stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay,
							stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond,
							stEndTime.stTime.ucHour, stEndTime.stTime.ucMinute, stEndTime.stTime.ucSecond, pstEvent->startTime,pstEvent->startTime+pstEvent->duration);
				}
				HLIB_DATETIME_ConvertUnixTimeToDateTime(pstEvent->startTime,&pstSchedule->stStartTime);
				pstSchedule->ulDuration = pstEvent->duration;
				pstSchedule->eEpgType = pstEvent->type;

				SAMA_SCHEDULE_UpdateSchedule(ulSlot, FALSE, pstSchedule, SAMA_GetVirtualRequestId(), TRUE);
			}
		}
		// else if TVA series, then update schedule and update TVA structure
		else
		{
			/***********************************************************************
			* ���� dapi callback �ȿ��� AddDxEvent.. �Լ��� ����� ����, service cache�� �������� ����ϵ��� �Ѵ�.
			***********************************************************************/
			if ( SAMALIB_TVA_FindCachedService(pstEvent->svcuid) != ERR_OK)
			{
				HxLOG_Warning("fail to get default auth. skip this event=[%s]. \n",SAMA_EVENT_GETNAME(pstEvent->name));
				DLIB_FreeType(iter.name, pstEvent);
				continue;
			}

			if (SAMALIB_TVA_IsCurrentlyRecording(pstSeriesSet) == TRUE)
			{
				HxLOG_Debug(HxANSI_COLOR_GREEN("Do not Update seriesset while recording. \n"));
				DLIB_FreeType(iter.name, pstEvent);
				continue;
			}
			SAMALIB_TVA_AddDxEventToSeriesSet(pstEvent, pstSeriesSet, 0, FALSE, ulSlot);

			// TODO: single <-> split ���� ��ȯ �ÿ� ó���� �� �� �ֵ��� �߰������ �Ѵ�.
			/*
			// 1. single -> split ��ȯ �ÿ�
			//	1-1. single PCRID:AAA�� �ְ�
					split PCRID: AAA#1 AAA#1 �� event�� new instance set���� �߰� �� ��.
					-> �� ��� ù��° AAA#1�� AAA�� eventid�� ��ġ�ϴ� SI ������ ���� ������ ���� �ǹǷ�, time�� update�� �ȴ�.
					-> �ι�° AAA#1�� ���ο� instance set�� ���� �ǰ�, �̴� selected inst�� �ƴϹǷ� ���� ���Ѵ�......
			*/
			sama_sch_tva_UpdateSeriesSet(pstSeriesSet, NULL);

			// TODO: non selected inst�� ����������.
		}

		DLIB_FreeType(iter.name, pstEvent);
	}

	return;
}

STATIC HUINT32	sama_sch_tva_GetRecKind(HUINT32 ulContentKind, HBOOL bIsSeries)
{
	if ( ulContentKind == SAMA_SERIES_CONTENT_KIND_NONE )
	{
		return SAMA_SERIES_CONTENT_KIND_NONE;
	}

	if ( bIsSeries == TRUE )
	{
		return SAMA_SERIES_CONTENT_KIND_SERIES;
	}

	if ( (ulContentKind & SAMA_SERIES_CONTENT_KIND_SPLIT) != 0 )
	{
		return SAMA_SERIES_CONTENT_KIND_SPLIT;
	}

	return SAMA_SERIES_CONTENT_KIND_SINGLE;
}

STATIC HERROR	sama_sch_tva_GenerateAltInfo(DxSchedule_t *pstTargetSch,SAPI_AlternativeItems_t *pstAltItems, SAPI_Conflict_t *pstConflictResult)
{
	TVA_ALTERNATIVE_INFO		stAltInfo;
	HUINT32						i;
	HERROR						hErr;
	DxSchedule_t				*pstAlternativeTargetSchedule;
	SERIES_SET					*pstSeriesSet;
	SERIES_SET					*pstAlternativeSeriesSet = NULL;

	if ( pstAltItems == NULL
		|| pstConflictResult == NULL )
	{
		HxLOG_Error("Invalid Param. alt=(0x%x), conf=(0x%x) \n",pstAltItems, pstConflictResult);
		return ERR_FAIL;
	}
	if ( pstConflictResult->ulNumConflict == 0 )
	{
		HxLOG_Error("conflict occured. but there is no conflict?\n");
		return ERR_FAIL;
	}

	stAltInfo.result = pstConflictResult->eType;
	SAMALIB_TVA_InitAlternativeSeriesSet();

	// 2. �õ��Ϸ��� �� Target schedule�� ���� alternative�� ã�´�.

	HxLOG_Print("\033[30;47m[%s:%d] Find Alt (Target)\033[0m \n",__FUNCTION__, __LINE__);
	while (1)
	{
		SAPI_AlternativeItem_t stAltItem;
		pstAlternativeTargetSchedule = pstTargetSch;
		pstSeriesSet = SAMALIB_TVA_GetSeriesSetBySlotId(pstTargetSch->ulSlot);
		hErr = SAMALIB_TVA_GetNextAlternativeInfo(pstSeriesSet, pstAlternativeTargetSchedule, &stAltInfo);
		if ( hErr != ERR_OK )
		{
			HxLOG_Debug(" reservation failed ? \n");
			break;
		}
		if ( stAltInfo.numAlternative == 0 )
		{
			HxLOG_Debug(" There is no alternative. \n");
			break;
		}

		HxSTD_memset(&stAltItem, 0x00, sizeof(SAPI_AlternativeItem_t));
		stAltItem.ulSlot			= stAltInfo.instance[0].slot;				// 0: ������ alt �� ù ��°��
		stAltItem.ulAltEvtId		= stAltInfo.instance[0].ulAltEventId[0];	// ù��° 0: ������ alt �� ù��°, �ι�° 0: ù��° split event
		stAltItem.ulAltSvcUid		= stAltInfo.instance[0].altSvcHandle[0];

		SAMALIB_TVA_AppendAlternativeItem(pstAltItems,&stAltItem);
		pstAlternativeSeriesSet = SAMALIB_TVA_GetSeriesSetBySlotId(pstTargetSch->ulSlot);
		SAMALIB_TVA_SetAlternativeSeriesSet(stAltItem.ulAltEvtId, stAltItem.ulAltSvcUid, pstAlternativeSeriesSet);
	}

	// alternative�� candidate�� ����� ���� ���� ���ش�.
	// 1. remove non selected inst? -> �׷��� �ȵȴ�? �ֳĸ� sch �ΰ��� ���ÿ� ���ؼ� conflict�� �ذ��� �� �� �����Ƿ�...
	// TODO: �ΰ��� ���ÿ� ���ؼ� conflict�� �ذ��ϴ� ��쿡 ...
	pstSeriesSet = SAMALIB_TVA_GetSeriesSetBySlotId(pstTargetSch->ulSlot);
	SAMALIB_TVA_SetCandidateAsSelected(pstSeriesSet);

	HxLOG_Print("\033[30;47m[%s:%d] Find alt (scheded)\033[0m \n",__FUNCTION__, __LINE__);
	// 3. conflict�� �� ������ ���Ե鿡 ���ؼ� alternative�� �ִ��� ã�´�.
	for ( i=0; i< pstConflictResult->ulNumConflict; i++ )
	{
		DxSchedule_t	*pstSchedule;
		HUINT32			ulSlot = pstConflictResult->pstConflictArray[i].ulSlot;
		pstSchedule = SAMA_SCHEDULE_GetSchedule(ulSlot);
		if ( pstSchedule == NULL )
		{
			HxLOG_Debug(" null schedule. slot=(%d)\n", ulSlot);
			continue;
		}

		HxLOG_Print("\033[30;47m[%s:%d] Find alt (sched:slot(%d))\033[0m \n",__FUNCTION__, __LINE__,ulSlot);
		while (1)
		{
			SAPI_AlternativeItem_t stAltItem;
			pstAlternativeTargetSchedule = pstSchedule;
			pstSeriesSet = SAMALIB_TVA_GetSeriesSetBySlotId(pstTargetSch->ulSlot);
			hErr = SAMALIB_TVA_GetNextAlternativeInfo(pstSeriesSet, pstAlternativeTargetSchedule, &stAltInfo);
			if ( hErr != ERR_OK )
			{
				HxLOG_Debug(" reservation failed ? \n");
				break;
			}
			if ( stAltInfo.numAlternative == 0 )
			{
				HxLOG_Debug(" There is no alternative. \n");
				break;
			}

			HxSTD_memset(&stAltItem, 0x00, sizeof(SAPI_AlternativeItem_t));
			stAltItem.ulSlot			= stAltInfo.instance[0].slot;				// 0: ������ alt �� ù ��°��
			stAltItem.ulAltEvtId		= stAltInfo.instance[0].ulAltEventId[0];	// ù��° 0: ������ alt �� ù��°, �ι�° 0: ù��° split event
			stAltItem.ulAltSvcUid		= stAltInfo.instance[0].altSvcHandle[0];
			SAMALIB_TVA_AppendAlternativeItem(pstAltItems,&stAltItem);
			pstAlternativeSeriesSet = SAMALIB_TVA_GetSeriesSetBySlotId(ulSlot);
			SAMALIB_TVA_SetAlternativeSeriesSet(stAltItem.ulAltEvtId, stAltItem.ulAltSvcUid, pstAlternativeSeriesSet);
		}

		pstSeriesSet = SAMALIB_TVA_GetSeriesSetBySlotId(pstSchedule->ulSlot);
		SAMALIB_TVA_SetCandidateAsSelected(pstSeriesSet);
	}

#if defined(CONFIG_DEBUG)
	check_alt_slots(pstAltItems);
#endif
	return ERR_OK;
}

STATIC void		sama_sch_tva_UpdateHdInfo (SERIES_SET *pstSeriesSet, SAMA_Context_t *pstContext, DxSchedule_t *pstNewSchedule)
{
	DxService_t stService;
	HERROR		hErr;

	HxLOG_Debug("New schedule's service uid (%d) \n",pstNewSchedule->ulSvcUid);
	hErr = SAMALIB_SVC_GetService(&(pstContext->stSamaConflictCheck), pstNewSchedule->ulSvcUid, &stService);
	if (hErr == ERR_OK && stService.highDefinition == eDxVIDEO_TYPE_HD)
	{
		HxLOG_Debug("HD Service detected. original(%d) \n",pstSeriesSet->eHdType);
		pstSeriesSet->eHdType = eDxVIDEO_TYPE_HD;
		pstNewSchedule->uExtInfo.stRec.ex.tva.ucRecKind |= SAMA_SERIES_CONTENT_KIND_HD;
		SAMA_SCHEDULE_UpdateSchedule(pstNewSchedule->ulSlot, FALSE, pstNewSchedule, SAMA_GetVirtualRequestId(), TRUE);
	}

}
STATIC HERROR	sama_sch_tva_UpdateSeriesSet(SERIES_SET *pstSeriesSet, SAMA_Context_t *pstContext)
{
	HERROR			hErr;
	PROGRAM_SET		*pstProgramSet;
	DxSchedule_RemovedInfo_t	stRemovedInfo = {0, };

	if ( pstSeriesSet == NULL )
	{
		HxLOG_Debug("Invalid param. null series set \n");
		return ERR_FAIL;
	}

	for ( pstProgramSet = pstSeriesSet->pstProgList; pstProgramSet != NULL; pstProgramSet = pstProgramSet->pNext)
	{
		EVENT_PERIOD *pstEventPeriod;
		for ( pstEventPeriod = pstProgramSet->pstSelectedInst->pstEventList; pstEventPeriod != NULL; pstEventPeriod = pstEventPeriod->pNext)
		{
			// TODO: split event�� atomic �ϰ� update�Ǵ°��� ��������� �Ѵ�. ( �� split�� make �ǰ� �ٸ� split�� make���� conflict ���� �� �ȴ�.)
			if ( pstEventPeriod->flag & SAMA_EVENT_ADDED )
			{
				DxSchedule_t *pstNewSchedule;
				SAPI_ConflictType_e 	eConflictResult;

				if (pstContext == NULL)
					continue;

				// TODO: series set�� crid�� prog�ε� add�Ǵ� ���� ���� �ȵȴ�.
				// TODO: split���... add �� ���� ������ type üũ�� �Ѵ�.
				pstNewSchedule= (DxSchedule_t *)SAMA_Malloc(sizeof(DxSchedule_t));
				if (pstNewSchedule == NULL )
				{
					HxLOG_Error("malloc failed. \n");
					continue;
				}

				hErr = SAMALIB_TVA_ConstructScheduleWithSeries(pstNewSchedule, pstEventPeriod);
				if (hErr != ERR_OK)
				{
					HxLOG_Debug("constrcut schedule failed. \n");
					SAMA_Free(pstNewSchedule);
					continue;
				}

				// conflict check
				(void)SAMALIB_CFLT_ClearConflictResult(&(pstContext->stSamaConflictCheck.stConflictResult));

				if (NULL != pstContext->stSamaConflictCheck.pstScheduleList)
				{
					pstContext->stSamaConflictCheck.pstScheduleList = HLIB_LIST_RemoveAllFunc (pstContext->stSamaConflictCheck.pstScheduleList, SAMA_Free_CB);
				}
				SAMA_SCHEDULE_TreeValueToList(SAMA_SCHEDULE_GetScheduleTree(), &(pstContext->stSamaConflictCheck.pstScheduleList));

				SAMALIB_CFLT_CheckConflictWithSchedule(&pstContext->stSamaConflictCheck, pstNewSchedule);

				eConflictResult = pstContext->stSamaConflictCheck.stConflictResult.eType;
				if ( eConflictResult != eSAPI_CONFLICT_TYPE_None )
				{
					TVA_CONTENTID_T	stDummyCrid;
					HxLOG_Debug("Conflict occured while update series. slot=(%d)\n ", pstEventPeriod->ulSlot);
					if (pstEventPeriod->pstInstSet != NULL)
					{
						SAMALIB_TVA_PrintInstanceSet(pstEventPeriod->pstInstSet);
					}
					{
						HUINT32 i;
						HUINT32 ulNumConflict = pstContext->stSamaConflictCheck.stConflictResult.ulNumConflict;
						for (i=0; i< ulNumConflict; i++)
						{
							HxLOG_Debug("conflict slot[%d/%d]=(%d) \n",i,ulNumConflict, pstContext->stSamaConflictCheck.stConflictResult.pstConflictArray[i].ulSlot);
						}
					}
					hErr = SAMALIB_TVA_RemoveMatchingEventPeriod(pstEventPeriod->ulSvcUid, pstEventPeriod->ulEventId, pstSeriesSet, &stDummyCrid);
					if (hErr != ERR_OK)
					{
						HxLOG_Debug("fail to remove matching event. svcUid=(%d), eventId=(%d)\n",pstEventPeriod->ulSvcUid, pstEventPeriod->ulEventId);
					}
					SAMA_Free(pstNewSchedule);
					continue;
				}

				// TVA LIB �ȿ��� ó�� ���ϴ� ������ �Ʒ����� ó�����ش�.
				if (pstEventPeriod->flag & SAMA_EVENT_ADDED)
				{
					HxLOG_Debug("null slot event period. crid=[%s]\n",SAMALIB_TVA_ConvertCridToStr(&(pstSeriesSet->stCrid)));
					hErr = SAMA_SCHEDULE_GetEmptySlotForSama(&(pstNewSchedule->ulSlot));
					if (hErr != ERR_OK)
					{
						HxLOG_Error("There is no empty slot for sama!\n");
						SAMA_Free(pstNewSchedule);
						continue;
					}
				}
				hErr = HAPI_GetRegistryInt("startpadding", (HUINT32 *)&pstNewSchedule->uExtInfo.stRec.ulPreOffsetTime);
				hErr |= HAPI_GetRegistryInt("endpadding", (HUINT32 *)&pstNewSchedule->uExtInfo.stRec.ulPostOffsetTime);
				if (hErr == ERR_FAIL)
				{
					HxLOG_Error("Get padding time error \n");
				}
				if ( MACRO_IS_TRACKING_SCHEDULE(pstNewSchedule))
				{
					pstNewSchedule->eRsvReady = DxRSVREADY_TVA_TRACKING;
				}
				else
				{
					pstNewSchedule->eRsvReady = DxRSVREADY_30_SEC; // opl scheduler���� ���� ���� �����ϰ� ��������. ������ ���� ��� ���� ����
				}

				hErr = SAMA_SCHEDULE_AddSchedule(pstNewSchedule->ulSlot, pstNewSchedule, TRUE, SAMA_GetVirtualRequestId());
				if ( hErr != ERR_OK )
				{
					HxLOG_Warning("add schedule failed. slot = (%d) \n",pstNewSchedule->ulSlot);
				}
				pstEventPeriod->flag &= ~SAMA_EVENT_ADDED;
				pstEventPeriod->ulSlot = pstNewSchedule->ulSlot;

				if (pstEventPeriod->flag & SAMA_EVENT_UPDATED )
				{
					HxLOG_Error("Updated happend with add event. slot=(%d) \n",pstNewSchedule->ulSlot);
				}

				if (pstSeriesSet->ulSeriesTrackingSlot != DxRSV_SLOT_NULL)
				{
					HxLOG_Debug("Series updated while series tracking. tracking slot=(%d) \n", pstSeriesSet->ulSeriesTrackingSlot);
					stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Completed;
					SAMA_SCHEDULE_RemoveSchedule(pstSeriesSet->ulSeriesTrackingSlot, &stRemovedInfo, TRUE, SAMA_GetVirtualRequestId(), TRUE);
					pstSeriesSet->ulSeriesTrackingSlot = DxRSV_SLOT_NULL;

					sama_sch_tva_UpdateHdInfo(pstSeriesSet, pstContext, pstNewSchedule);
				}

				SAMA_Free(pstNewSchedule);
				pstNewSchedule = NULL;
				continue;	// add�� �� ��� update�� ���� ���� ���̶�� ������ �Ѵ�.
			}

			if ( pstEventPeriod->flag & SAMA_EVENT_UPDATED )
			{
				HUINT32 ulSlot = pstEventPeriod->ulSlot;
				DxSchedule_t * pstSchedule = SAMA_SCHEDULE_GetSchedule(ulSlot);
				TVA_CONTENTID_T	stDummyCrid;

				if (pstSchedule == NULL)
				{
					HxLOG_Error("GetSchedule failed. slot=(%d) \n",ulSlot);
					hErr = SAMALIB_TVA_RemoveMatchingEventPeriod(pstEventPeriod->ulSvcUid, pstEventPeriod->ulEventId, pstSeriesSet, &stDummyCrid);
					if (hErr != ERR_OK)
					{
						HxLOG_Debug("fail to remove matching event. svcUid=(%d), eventId=(%d)\n",pstEventPeriod->ulSvcUid, pstEventPeriod->ulEventId);
					}

					continue;
				}


				{
					HxDATETIME_t	stStartTime, stEndTime;
					UNIXTIME		utStartTime, utEndTime;
					HxLOG_Debug("\033[30;47m[%s:%d] Update schedule. slot=(%d) \033[0m \n",__FUNCTION__, __LINE__,pstSchedule->ulSlot);

					HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstSchedule->stStartTime,&utStartTime);
					utEndTime = utStartTime + pstSchedule->ulDuration;
					HLIB_DATETIME_ConvertUnixTimeToDateTime(utEndTime,&stEndTime);
					HxLOG_Debug(" Before %d/%d/%d (%02d:%02d:%02d ~ %02d:%02d:%02d)\n",
							pstSchedule->stStartTime.stDate.usYear, pstSchedule->stStartTime.stDate.ucMonth, pstSchedule->stStartTime.stDate.ucDay,
							pstSchedule->stStartTime.stTime.ucHour, pstSchedule->stStartTime.stTime.ucMinute, pstSchedule->stStartTime.stTime.ucSecond,
							stEndTime.stTime.ucHour, stEndTime.stTime.ucMinute, stEndTime.stTime.ucSecond);
					HLIB_DATETIME_ConvertUnixTimeToDateTime(pstEventPeriod->startTime,&stStartTime);
					HLIB_DATETIME_ConvertUnixTimeToDateTime(pstEventPeriod->endTime,&stEndTime);
					HxLOG_Debug("  After %d/%d/%d (%02d:%02d:%02d ~ %02d:%02d:%02d)\n",
							stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay,
							stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond,
							stEndTime.stTime.ucHour, stEndTime.stTime.ucMinute, stEndTime.stTime.ucSecond);
				}

				if (pstEventPeriod->flag & SAMA_EVENT_SEASON_EPISODE_SETTED)
				{
					HxLOG_Debug("  Season Episode Set(%d) Season(%d) Episode(%d) \n",
							pstEventPeriod->flag & SAMA_EVENT_SEASON_EPISODE_SETTED, pstEventPeriod->ulSeason,pstEventPeriod->ulEpisode);

					pstSchedule->uExtInfo.stRec.ex.tva.ulSeason = pstEventPeriod->ulSeason;
					pstSchedule->uExtInfo.stRec.ex.tva.ulEpisodeNum = pstEventPeriod->ulEpisode;
					pstSchedule->uExtInfo.stRec.ex.tva.eContentKind |= DxRsvTva_ContentKind_SeasonEpisode_Done;
				}

				HLIB_DATETIME_ConvertUnixTimeToDateTime(pstEventPeriod->startTime, &pstSchedule->stStartTime);
				pstSchedule->ulDuration = pstEventPeriod->endTime - pstEventPeriod->startTime;
				pstSchedule->eEpgType = (pstEventPeriod->flag & SAMA_EVENT_BY_EIT_PF)? eDxEPG_TYPE_PF:pstSchedule->eEpgType;

				// TODO: split -> not split �� ������Ʈ �� ���, ������ split �̺�Ʈ�� removeSchedule ����� �Ѵ�
				SAMA_SCHEDULE_UpdateSchedule(ulSlot, FALSE, pstSchedule, SAMA_GetVirtualRequestId(), TRUE);
				pstEventPeriod->flag &= ~SAMA_EVENT_UPDATED;
				continue;
			}
		}
	}

	return ERR_OK;
}



STATIC HERROR	sama_sch_tva_IsAllowableUpdateSeries (void)
{
	static UNIXTIME utLastUpdateTime = 0;
	UNIXTIME		utCurrTime;
	UNIXTIME		ut5Min;

	// ���� ������ ���� ������ ���� 10�� �ȿ� ������ update series�� �����Ѵ�.
	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if (utCurrTime < utLastUpdateTime + 10*60)
	{
		HxLOG_Debug(HxANSI_COLOR_RED("Not past 10min since last update(%d). please try in (%d)sec \n"),utLastUpdateTime,utLastUpdateTime + 10*60-utCurrTime);
		return ERR_FAIL;
	}

	// eit pf�� ���� update�� section complete�� ���� update, running status�� ���� update���� �ִµ�,
	// �̷� update�� timer time up���� main task �ϳ����� ���Ƿ� �ϴ� �����ϱ�� ������, (time up�� �ʰ� �����ϴ��� ������ �ϴ�)
	// padding update�� ���� ������� �� �ߴ� update�� ������ ��ġ�� �ʴ´ٰ� ������ �� �Ͽ� ������ �Ǵ�.
	// ������ �����Ͽ� �Ʒ��� ���� �ڵ�� ���� �Ǿ�� �Ѵ�.
	ut5Min = utCurrTime % (5*60);
	if ((ut5Min > (5*60 - 120)) || (ut5Min < 30)) // 2���� update series�� 60��, padding update�� 30��, �����ϰ� 30�ʷ� ���� �����̴�. �̸�ŭ ���൵ update series�� ���� ���ͼ� ������.
	{
		HxLOG_Debug(HxANSI_COLOR_RED("Ignore Update around 0,5,10,15... minute \n"));
		return ERR_FAIL;
	}

	utLastUpdateTime = utCurrTime;
	HxLOG_Debug(HxANSI_COLOR_GREEN(" Series Update. set last update time=(%d)  \n"),utLastUpdateTime);

	return ERR_OK;
}
HERROR	sama_sch_tva_UpdateAllSeriesSet(SAMA_Context_t *pstContext)
{
	HxList_t		*pstSeriesList;
	HxList_t		*pvIter;
	HERROR			hErr = ERR_OK;
	DxSchedule_RemovedInfo_t	stRemovedInfo = {0, };
	DxSchedule_t	*pstTrackingSchedule;
	HUINT32			ulTrackingSlot;

	pstSeriesList = SAMALIB_TVA_GetSeriesList();
	for (pvIter = HLIB_LIST_First(pstSeriesList); NULL != pvIter; )
	{
		SERIES_SET		*pstSeriesSet = (SERIES_SET *)HLIB_LIST_Data(pvIter);
		pvIter = HLIB_LIST_Next(pvIter);

		// conflict check �߿� update series �߻���, conflict check�� ���� series set�� update�Ϸ��� �õ��� �� �� �ִ�.
		// �̷� series set�� conflict check�� ������ �ٷ� �޸𸮸� ��ȯ���ֹǷ�, ����ϴ� ���� �����ϴ�.
		// series set�� TVA �� ��쿡�� �׻� �޸𸮿� �ִ� ������ �������ش�.
		if ( pstSeriesSet == NULL )
			continue;

		/**************************************************************************************************************************
		*  series , programme ����: stCRID.type
		*  tracking O,X ����:     tracking slot�� null�ΰ�� X
		*
		*                       series         programme
		*   tracking X      update         ignore
		*   trackong O     update        update
		*
		*  RCT�� ���� ���� 7day epg pool �ȿ� ���� programme�� ���ؼ� programme CRID�� record�� �ɾ� ���� ���
		*  epg pool�ȿ� �ش��ϴ� programme CRID�� ���� ������ tracking�� ������.
		****************************************************************************************************************************/
		if ((pstSeriesSet->stCrid.type != SAMA_CRID_TYPE_SERIES
			&& pstSeriesSet->ulSeriesTrackingSlot == DxRSV_SLOT_NULL))
		{
			continue;
		}

		// 13�� ���� �ƹ� episode�� ���� series�� �������ش�.
		if (pstSeriesSet->pstProgList == NULL)
		{
			if (pstSeriesSet->ulSeriesTrackingSlot != DxRSV_SLOT_NULL)
			{
				ulTrackingSlot = pstSeriesSet->ulSeriesTrackingSlot;
				pstTrackingSchedule = SAMA_SCHEDULE_GetSchedule(ulTrackingSlot);
				if (SAMALIB_TVA_IsTVAScheduleRemovable(pstTrackingSchedule, TRUE) == ERR_OK)
				{
					HxLOG_Error("Series tracking completed. delete series. tracking slot=(%d) \n", ulTrackingSlot);
					SAMALIB_TVA_DeleteSeriesSet(pstSeriesSet);
					stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Completed;
					SAMA_SCHEDULE_RemoveSchedule(ulTrackingSlot, &stRemovedInfo, TRUE, SAMA_GetVirtualRequestId(), TRUE);

					continue;
				}
			}
			else
			{
				HxLOG_Error("Empty series without tracking slot \n");
				SAMALIB_TVA_DeleteSeriesSet(pstSeriesSet);

				continue;
			}
		}

		if (SAMALIB_TVA_IsCurrentlyRecording(pstSeriesSet) == TRUE)
		{
			HxLOG_Debug(HxANSI_COLOR_GREEN("Do not Update seriesset while recording. \n"));
			continue;
		}

		SAMALIB_TVA_BuildSeriesSet(pstSeriesSet);

		// program build �Ŀ� �̸� ������� �����Ƿ� �ּ� ó��, ���Ŀ� program build�� ���ؼ� alternative�� ����Ҷ� �ٽ� �Ѿ� �Ѵ�.
		//SAMALIB_TVA_BuildProgramSet(pstSeriesSet);

		SAMALIB_TVA_PrintSeriesSet(pstSeriesSet);

		SAMALIB_TVA_RemoveNonSelectedInstance(pstSeriesSet);

		hErr |= sama_sch_tva_UpdateSeriesSet(pstSeriesSet,pstContext);
	}

	return hErr;
}

STATIC void sama_sch_tva_OnDamaScheduleEventReceived (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	SamaTvAnyTime_InternalMsg_t 	*msg = (SamaTvAnyTime_InternalMsg_t *)userdata;
	DxSchedule_t				*pstSchedule = NULL;
	HUINT32						ulSeriesIndex;
	UNIXTIME					utCurrTime;

	DxEvent_t			*Event;
	DAxIterator_t		iter;

	HxLOG_Trace();

	if (msg == NULL
		|| msg->pstSchedule == NULL)
	{
		goto END_FUNC;
	}
	pstSchedule = msg->pstSchedule;
	ulSeriesIndex = msg->p1;

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if(UNIX_FIRST_DAY >= utCurrTime)
	{
		HxLOG_Error("time error. current sys time = (%d)\n",utCurrTime);
	}

	// case fail-to-find-event && SBR
	if ((size == 0)&&MACRO_IS_TVA_SERIES_TYPE(pstSchedule->eRsvType))
	{
		// fail to find event -> fail to find SERIES CRID -> fail
		HxLOG_Error("fail to find event. \n");
		SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
		goto END_FUNC;
	}

	// case fail-to-find-event && EBR
	if (size == 0)
	{
		DxEvent_t	*pstCurrentSeriesEvent;

		// fail to find event -> make dummy epg from schedule info
		pstCurrentSeriesEvent = (DxEvent_t *)SAMA_Calloc(sizeof(DxEvent_t));
		if (pstCurrentSeriesEvent == NULL)
		{
			HxLOG_Error("alloc failed \n");
			goto END_FUNC;
		}

		pstCurrentSeriesEvent->type			= eDxEPG_TYPE_SC;
		pstCurrentSeriesEvent->svcuid		= pstSchedule->ulSvcUid;
		pstCurrentSeriesEvent->crid			= NULL;
		pstCurrentSeriesEvent->eventId		= pstSchedule->uExtInfo.stRec.nEvtId;
		HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &(pstCurrentSeriesEvent->startTime));
		pstCurrentSeriesEvent->duration		= pstSchedule->ulDuration;

		SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_DONE, (HINT32)pstSchedule, (HINT32)ulSeriesIndex, (HINT32)pstCurrentSeriesEvent, 0, 0, 0);
	}

	// case normal
	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		Event = (DxEvent_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (Event == NULL)
		{
			HxLOG_Debug("null event \n");
			DLIB_FreeType(iter.name, Event);
			continue;
		}
		HxLOG_Debug("type[%s], Event name=[%s] received \n",DLIB_ConvertEnum2Str(DxNAMEOF(DxEPG_Type_e), Event->type), SAMA_EVENT_GETNAME(Event->name));

		if (Event != NULL)
		{
			DxEvent_t	*pstCurrentSeriesEvent;
			pstCurrentSeriesEvent = DxEVENT_Clone(Event);

			SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_DONE, (HINT32)pstSchedule, (HINT32)ulSeriesIndex, (HINT32)pstCurrentSeriesEvent, 0, 0, 0);
			DLIB_FreeType(iter.name, Event);

			break;
		}
	}

END_FUNC:
	if (msg != NULL)
	{
		SAMA_Free(msg);
	}
	return;
}

STATIC void sama_sch_tva_OnDamaCridEventsReceived (const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxEvent_t			*Event;
	DAxIterator_t		iter;
	SamaTvAnyTime_InternalMsg_t *msg = (SamaTvAnyTime_InternalMsg_t *)userdata;
	DxSchedule_t *		pstSchedule = NULL;
	HUINT32				ulSeriesIndex = 0;
	SERIES_SET			*pstSeriesSet	= NULL;
	HxList_t			*pstSeriesEventsList = NULL;

	HxLOG_Trace();


	pstSchedule = msg->pstSchedule;
	ulSeriesIndex = msg->p1;
	pstSeriesSet = (SERIES_SET *)msg->p2;

	if ( pstSchedule == NULL
		|| pstSeriesSet == NULL )
	{
		HxLOG_Error("series set should have been allocated before enter here.\n");
		return;
	}

	// find next item.
	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		Event = (DxEvent_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (Event)
		{
			DxEvent_t		*pstSeriesRecordEventInfo;
			pstSeriesRecordEventInfo = DxEVENT_Clone(Event);

			HxLOG_Debug("type[%s] appned event, event ID: %d \n",DLIB_ConvertEnum2Str(DxNAMEOF(DxEPG_Type_e), Event->type), Event->eventId);
			pstSeriesEventsList = HLIB_LIST_Append(pstSeriesEventsList, (void *)pstSeriesRecordEventInfo);
		}
		DLIB_FreeType(iter.name, Event);
	}

	HxLOG_Debug("send SAMAMESSAGE_CURRENT SERIES ITEM RECEIVED_DONE \n");

	SAMA_PostMessage(SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED_DONE, (HINT32)pstSchedule, (HINT32)ulSeriesIndex, (HINT32)pstSeriesSet, (HINT32)pstSeriesEventsList, 0, 0); //(HINT32)pList, 0, 0, 0, 0);
	SAMA_Free(msg);
	return;
}
STATIC void sama_sch_tva_cbScheduleOneWay (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long utTime)
{
	SAMA_PostMessage(SAMAMESSAGE_PROCESSONEWAYTIMER, ulSlot, eTimerType, utTime, 0, 0, 0);
}

STATIC void sama_sch_tva_cbScheduleTwoWay (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long utTime)
{
	SAMA_PostMessage(SAMAMESSAGE_PROCESSTWOWAYTIMER, ulSlot, eTimerType, utTime, 0, 0, 0);
}

HBOOL	sama_sch_tva_IsAllowableTrackingChange (DxSchedule_t *pstSchedule, SAPI_TimerItemType_e eTimerType)
{
	if ( pstSchedule == NULL )
	{
		HxLOG_Warning("Invalid param. null schedule \n");
		return FALSE;
	}
	if ( !MACRO_IS_TRACKING_SCHEDULE(pstSchedule) )
	{
		HxLOG_Debug("Only tracking schedule is applicapable. \n");
		return TRUE;
	}

	// TODO: switch (Timer type)�� ���ؼ� status�� üũ�ϴ� ������ �����ؾ� �Ѵ�.
	switch ( pstSchedule->eStatus )
	{
		case DxRSVSTATUS_READY:
			if ( eTimerType == eSAPI_TIMER_TYPE_Stop )
			{
				HxLOG_Warning("Can't stop ready schedule. slot=(%d) \n", pstSchedule->ulSlot);
				return FALSE;
			}
			break;
		case DxRSVSTATUS_RUNNING:
			if ( eTimerType == eSAPI_TIMER_TYPE_Ready)
				{
				HxLOG_Debug("Tracking Schedule already started. slot=(%d) \n",pstSchedule->ulSlot);
					return FALSE;
				}
			else if ( eTimerType == eSAPI_TIMER_TYPE_Start )
			{
				HxLOG_Error("schedule already started. slot=(%d) \n",pstSchedule->ulSlot);
				return FALSE;
			}
			else if ( eTimerType == eSAPI_TIMER_TYPE_StopPendingBroken)
			{
				HxLOG_Debug("Is not Pending broken. ignore pending broken timer. \n");
				return FALSE;
			}
			break;
		default:
			break;
	}

	return TRUE;
}

HERROR	sama_sch_tva_ChangeScheduleStatus (DxSchedule_t *pstSchedule, SAPI_TimerItemType_e eTimerType)
{
	HBOOL	bIsAllowable = FALSE;

	if ( pstSchedule == NULL ) return ERR_FAIL;

	if ( MACRO_IS_TRACKING_SCHEDULE(pstSchedule) )
	{
		bIsAllowable = sama_sch_tva_IsAllowableTrackingChange(pstSchedule, eTimerType);
		if ( bIsAllowable == FALSE )
		{
			HxLOG_Debug("Not allowable status change. status=(%d)[%s] timer=(%d)\n",pstSchedule->eStatus,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvStatus_e), pstSchedule->eStatus), eTimerType);
			return ERR_FAIL;
		}
	}

	switch (eTimerType)
	{
	case eSAPI_TIMER_TYPE_ReadyTracking:
		if ( MACRO_IS_TRACKING_SCHEDULE(pstSchedule) )
		{
			UNIXTIME	utStartTime;
			HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &utStartTime);
			SAMALIB_TRACKING_StartTracking(pstSchedule->ulSvcUid, pstSchedule->uExtInfo.stRec.nEvtId, pstSchedule->ulSlot, utStartTime);
 		}
		//pstSchedule->eStatus = DxRSVSTATUS_READY;
		break;
	case eSAPI_TIMER_TYPE_Ready:
#ifdef TEST_NORDIG_ACCURATE_RECORD
		if (MACRO_IS_TRACKING_SCHEDULE(pstSchedule) )
		{
			HUINT8 		ucRunningStatus = 0;
			UNIXTIME	utStartTime;
			if (SAMALIB_TRACKING_GetRunningStatus(pstSchedule->ulSlot,&ucRunningStatus) == ERR_OK)
			{
				if (ucRunningStatus == 0) // undefined = inited = not in eit p/f
				{
					// nordig ���: tracking fail�� ���(tracking event id�� eit p/f�� ����=>running status ==0) schedule �ð� ��� ��ȭ������
					SAMALIB_TRACKING_StopTracking(pstSchedule->ulSvcUid, pstSchedule->uExtInfo.stRec.nEvtId);

					// tracking -> normal mode ( by change padding time)
					pstSchedule->uExtInfo.stRec.ulPostOffsetTime = SAMA_TVA_REMOVED_PADDING_TIME;
					pstSchedule->uExtInfo.stRec.ulPreOffsetTime	 = SAMA_TVA_REMOVED_PADDING_TIME;

					HLIB_DATETIME_ConvertDateTimeToUnixTime(&pstSchedule->stStartTime, &utStartTime);
					SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Start, pstSchedule->eRsvType, utStartTime, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
					SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Stop, pstSchedule->eRsvType, utStartTime+pstSchedule->ulDuration, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
					HxLOG_Error("Fail to find tracking event. stop tracking & start scheduled time recording. slot=(%d) \n",pstSchedule->ulSlot);
				}else if (ucRunningStatus == 1)	// not running
				{
					HxLOG_Error("Delayed schedule (tracking but not in eit present when it's scheduled start time. slot=(%d) \n",pstSchedule->ulSlot);
					return ERR_FAIL;
				}
				else if (ucRunningStatus == 4) // running
				{
					HxLOG_Error("Normal tracking schedule start. slot=(%d) \n",pstSchedule->ulSlot);
				}
			}
		}
#endif
		pstSchedule->eStatus = DxRSVSTATUS_READY;
		break;

	case eSAPI_TIMER_TYPE_Start:
		pstSchedule->eStatus = DxRSVSTATUS_RUNNING;
		break;

	case eSAPI_TIMER_TYPE_Stop:
	case eSAPI_TIMER_TYPE_StopPendingBroken:
	case eSAPI_TIMER_TYPE_StopRunaway:
		{
			UNIXTIME		utStartTime;
			UNIXTIME		utCurrTime;
			DxSchedule_t	*pstStopedSchedule = pstSchedule;

			if ( SAMALIB_TRACKING_IsTrackingBySlotId(pstStopedSchedule->ulSlot) )
			{
				SAMALIB_TRACKING_StopTracking(pstStopedSchedule->ulSvcUid, pstStopedSchedule->uExtInfo.stRec.nEvtId);
			}

			// conflict�� �ɸ��� schedule�� stop �ÿ���, �� schedule�� ���� pending �� schedule�� ���� �� �����Ƿ�, pending �Ǿ��� schedule���� restart ���� ����.
			if ( MACRO_IS_CONFLICT_TYPE(pstStopedSchedule->eRsvType)
				&& pstStopedSchedule->eStatus == DxRSVSTATUS_RUNNING)
			{
				if ( MACRO_IS_TRACKING_SCHEDULE(pstStopedSchedule))
				{
					utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
					HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstStopedSchedule->stStartTime), &utStartTime);

					// conflict check�� ���� ������ tracking early OR delay �Ǿ� ����� �ð����� duration�� ������ �ش�.
					if (utCurrTime > utStartTime)
					{
						pstStopedSchedule->ulDuration = utCurrTime-utStartTime;
					}
					else
					{
						HxLOG_Error("curr time is less than start time. curr(%d) start(%d) \n",utCurrTime, utStartTime);
					}
				}

				SAMALIB_TRACKING_RestartPendingSchedules();
			}

			pstStopedSchedule->eStatus = (DxRSVREPEAT_ONCE == pstStopedSchedule->eRepeat) ? DxRSVSTATUS_STOP : DxRSVSTATUS_WAITING;
			break;
		}
	case eSAPI_TIMER_TYPE_UpdateDelayed:
		{
			UNIXTIME		utCurrTime;
			HUINT32			utEndTime;
			HUINT32			utStartTime;

			DxSchedule_t		*pstDelayedSchedule = pstSchedule;
			utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();

			// if delay ���� schedule�� �ƴ϶�� �����Ѵ�.
			if ( !SAMALIB_TRACKING_IsTrackingBySlotId(pstDelayedSchedule->ulSlot)
				|| !(pstDelayedSchedule->eStatus == DxRSVSTATUS_RUNNING))
			{
				HxLOG_Error("Updated Delayed Timer for not tracking schedule. slot=(%d) \n",pstDelayedSchedule->ulSlot);
				return ERR_FAIL;
			}

			HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstDelayedSchedule->stStartTime), &utStartTime);

			// conflict check�� ���� delay ���� �ð��� duration�� �ݿ��Ѵ�. + ������ SAMA_TVA_UPDATE_DELAYED_SCH_DURATION �� �Ŀ� delay�� schedule�� �����ٰ� �������ش�.
			utEndTime = utCurrTime + SAMA_TVA_UPDATE_DELAYED_SCH_DURATION;
			HxLOG_Debug("Delay schedule. slot=(%d) duration before(%d)->after=(%d)\n",pstDelayedSchedule->ulSlot, pstDelayedSchedule->ulDuration,utEndTime - utStartTime);

			// sama schedule�� schedule tree�� update�� �ð��� �ݿ��Ѵ�.
			pstDelayedSchedule->ulDuration = utEndTime - utStartTime;

			// series set ������ update�� �ð��� �ݿ��Ѵ�.
			SAMALIB_TVA_UpdateEventTimeBySlot(pstDelayedSchedule->ulSlot, utStartTime, utEndTime);

			// �ٸ� process�鿡 update�� �ð��� �ݿ��Ѵ�.
			(void)SAMA_PROTOCOL_NotifyScheduleModified(pstDelayedSchedule->ulSlot, pstDelayedSchedule, TRUE, SAMA_GetVirtualRequestId());

			// ���� update �ð��� �־��ش�.
			SAMA_TIMER_InsertTimer(pstDelayedSchedule->ulSlot, eSAPI_TIMER_TYPE_UpdateDelayed, pstDelayedSchedule->eRsvType, utCurrTime + SAMA_TVA_UPDATE_DELAYED_SCH_PERIOD, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);

			// notify ���� �ʱ� ���� fail ó�� �Ѵ�.
			return ERR_FAIL;
		}
	default:
		break;
	}

	return ERR_OK;
}
HERROR	sama_sch_tva_ProcessTimerConflict (SAMA_Context_t *pstContext, DxSchedule_t *pstSchedule, SAPI_TimerItemType_e eTimerType)
{
	HERROR hErr = ERR_OK;

	if (pstContext == NULL || pstSchedule == NULL ) return ERR_FAIL;

	switch ( eTimerType )
	{
		// tracking ���� schedule�� �ٸ� schedule�� delay�� ���� conflict���� pending ���� �ش�.
		case eSAPI_TIMER_TYPE_Ready:
		{
			UNIXTIME		utStartTime;
			UNIXTIME		utCurrTime;
			UNIXTIME		utEndTime;
			DxSchedule_t	*pstDummySchedule;

			// TODO: sapi���� conflict check�ÿ� tracking schedule�� endtime�� �����µ� running�̶�� end timeline�� �ð��� cur time + 60���� ��Ƽ� �ذ��Ѵ�.

			if ( !SAMALIB_TRACKING_IsTrackingBySlotId(pstSchedule->ulSlot) )
			{
				break;
			}

			pstDummySchedule = (DxSchedule_t *)SAMA_Malloc(sizeof(DxSchedule_t));
			if (pstDummySchedule == NULL)
			{
				break;
			}

			utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();

			SAMA_MemCpy(pstDummySchedule, pstSchedule, sizeof(DxSchedule_t));

			// conflict check�� ���� ���� start time, duration�� �ݿ��Ѵ�.
			HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstDummySchedule->stStartTime), &utStartTime);
			HxLOG_Debug("Change start time for tracking conflict check. before(%d)->after(%d) \n",utStartTime, utCurrTime);
			HLIB_DATETIME_ConvertUnixTimeToDateTime(utCurrTime, &(pstDummySchedule->stStartTime));

			utEndTime = utStartTime + pstDummySchedule->ulDuration;
			HxLOG_Debug("Change duration for tracking conflict check. before(%d)->after(%d) \n",pstDummySchedule->ulDuration, utEndTime - utCurrTime);
			pstDummySchedule->ulDuration = utEndTime - utCurrTime;


			if (NULL != pstContext->stSamaConflictCheck.pstScheduleList)
			{
				pstContext->stSamaConflictCheck.pstScheduleList = HLIB_LIST_RemoveAllFunc (pstContext->stSamaConflictCheck.pstScheduleList, SAMA_Free_CB);
			}
			SAMA_SCHEDULE_TreeRunningValueToList(SAMA_SCHEDULE_GetScheduleTree(), &(pstContext->stSamaConflictCheck.pstScheduleList));

			(void)SAMALIB_CFLT_ClearConflictResult(&(pstContext->stSamaConflictCheck.stConflictResult));
			pstContext->stSamaConflictCheck.bScheduleChange = TRUE; // ���⼭�� ������ TRUE. �ֳ��ϸ� �̹� sama schedule list �� �ֱ� ����.
			(void)SAMALIB_CFLT_CheckConflictWithSchedule(&(pstContext->stSamaConflictCheck), pstDummySchedule);

			if (eSAPI_CONFLICT_TYPE_None != pstContext->stSamaConflictCheck.stConflictResult.eType)
			{
				HUINT32	i;
				HUINT32	ulConflictedSlot;
				DxSchedule_t	*pstConflictedSchedule;
				HBOOL			bIsNeedPending = TRUE;

				HxLOG_Debug("Start Conflicted. slot=(%d) \n", pstDummySchedule->ulSlot);

				for (i=0;i<pstContext->stSamaConflictCheck.stConflictResult.ulNumConflict;i++)
				{
					ulConflictedSlot = pstContext->stSamaConflictCheck.stConflictResult.pstConflictArray[i].ulSlot;
					pstConflictedSchedule = SAMA_SCHEDULE_GetSchedule(ulConflictedSlot);
					if (pstConflictedSchedule == NULL)
					{
						HxLOG_Error("Conflict Schedule doesn't exist. slot=(%d) \n",ulConflictedSlot);
						continue;
					}

					// if conflict schedule started already
					if (pstConflictedSchedule->eStatus == DxRSVSTATUS_RUNNING
						|| pstConflictedSchedule->eStatus == DxRSVSTATUS_READY)
					{
						// ready�� ����ϴ� ����: tracking�� ��� ready�� ���� ���� schedule conflict�� ����Ͽ��� ���� start�� ������ ���� �Ѵ�.
						//				    ���� start�� ������ ���� �ʴ´ٸ� ready ~ start���̿� appkit���� confirm �޴� ���̿� �ٸ� schedule�� start�ع��� �� �ִ�.
						HxLOG_Debug("Start Conflicted. due to slot=(%d) status=(%d)\n"
											, pstConflictedSchedule->ulSlot,pstConflictedSchedule->eStatus);
					}
					else
					{
						//�̷��� �����ϴ� schedule�� ���� conflict�� ���� �������, �̷��� EITpf�� ��ȭ�� �ؼҵ� ���ɼ��� �����Ƿ� �����Ѵ�.
						// conflict �� �������� ���� schedule�� �ϳ��� �ִ� ���, pending ��Ű�� �ʴ´�.
						HxLOG_Debug("Start Conflicted. conflicted schedule is not started. slot=(%d) \n", pstConflictedSchedule->ulSlot);
						bIsNeedPending = FALSE;
					}
				}

				if (bIsNeedPending == TRUE)
				{
					HERROR		hGetTimeErr;
					unsigned long	ulDummyTime;

					HxLOG_Debug(HxANSI_COLOR_RED("Start Conflicted. set pending. slot=(%d) \n"), pstDummySchedule->ulSlot);
					(void)SAMALIB_TRACKING_SetPending(pstDummySchedule->ulSlot, TRUE, TRACKING_FAIL_REASON_REC_DELAYED);

					// ���� !! pending�� schedule�� restart�� "�ټ�"�� ���ÿ� �޼����� ���´�.
					hGetTimeErr = SAMA_TIMER_GetTimerTime(pstDummySchedule->ulSlot,eSAPI_TIMER_TYPE_Ready,&ulDummyTime);
					if (hGetTimeErr == ERR_OK)
					{
						HxLOG_Debug(HxANSI_COLOR_RED("Ready Timer already exists. slot=(%d) \n"),pstDummySchedule->ulSlot);
					}
					else
					{
						SAMA_TIMER_InsertTimer(pstDummySchedule->ulSlot, eSAPI_TIMER_TYPE_Ready, pstDummySchedule->eRsvType, utCurrTime + SAMA_TVA_RETRY_READY_SCH_PERIOD, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
						hErr=SAMA_TIMER_ExecuteTimer();
						if(hErr != ERR_OK)
						{
							HxLOG_Critical(" SAMA_TIMER_ExecuteTimer Error!!\n");	
						}
					}

					// change status���� ready�� ����� ���� status�� waiting���� �������´�. tracking schedule���� ready�� start(running)�� ������ �ǹ̴�.
					pstSchedule->eStatus = DxRSVSTATUS_WAITING;

					// pending ���� schedule�� start ���� �ʴ´�.
					hErr = ERR_FAIL;
				}
				else
				{
					HxLOG_Debug(HxANSI_COLOR_GREEN("conflict in the future. ignore conflict. slot=(%d) \n"), pstDummySchedule->ulSlot);
				}
			}

			if (hErr != ERR_FAIL)
			{
				SERIES_SET *pstSeriesSet = NULL;
				// event tracking���� ���� �ð� ��ȭ�� sama schedule�� series ����, �ٸ� ���μ����� ���� �ݿ����ش�.
				HxLOG_Debug("Update ready schedule start&duration slot=(%d) \n",pstSchedule->ulSlot);
				SAMA_MemCpy(&pstSchedule->stStartTime, &(pstDummySchedule->stStartTime), sizeof(HxDATETIME_t));
				pstSchedule->ulDuration = pstDummySchedule->ulDuration;

				SAMALIB_TVA_UpdateEventTimeBySlot(pstSchedule->ulSlot, utCurrTime, utEndTime);
				(void)SAMA_PROTOCOL_NotifyScheduleModified(pstSchedule->ulSlot, pstSchedule, TRUE, SAMA_GetVirtualRequestId());

				pstSeriesSet = SAMALIB_TVA_GetSeriesSetBySlotId(pstSchedule->ulSlot);
				SAMALIB_TVA_SetCurrentlyRecording(pstSeriesSet, pstSchedule->ulSlot);
			}

			SAMA_Free(pstDummySchedule);

			break;
		}
#ifdef SAMA_LAZY_PADDING_CONFLICT_REMOVE
		case eSAPI_TIMER_TYPE_PaddingStartReady:
		case eSAPI_TIMER_TYPE_PaddingStopReady:
		{
			HBOOL		bUpdate = FALSE;
			sama_PaddingRemoval_e		eUpdateFlag = eSAPI_PADDINGRMV_None;

			HxLOG_Debug("eTimerType: %d, check padding!!! \n", eTimerType);
			SAMA_SCHEDULE_TreeValueToList(SAMA_SCHEDULE_GetScheduleTree(), &(pstContext->stSamaConflictCheck.pstScheduleList));

			// Conflict Result�� �ϴ� ����� �Ѵ�.
			(void)SAMALIB_CFLT_ClearConflictResult(&(pstContext->stSamaConflictCheck.stConflictResult));
			pstContext->stSamaConflictCheck.bScheduleChange = TRUE; // ���⼭�� ������ TRUE. �ֳ��ϸ� �̹� sama schedule list �� �ֱ� ����.
			(void)SAMALIB_CFLT_CheckConflictWithSchedule(&(pstContext->stSamaConflictCheck), pstSchedule);

			if(eSAPI_TIMER_TYPE_PaddingStartReady == eTimerType)
			{
				eUpdateFlag = eSAPI_PADDINGRMV_StartPad;
			}
			else
			{
				eUpdateFlag = eSAPI_PADDINGRMV_StopPad;
			}

			bUpdate = SAMA_SCHEDULE_RemovePadding(pstContext, FALSE, eUpdateFlag);
#if defined(CONFIG_DEBUG)
			HxLOG_Debug("bUpdate: %d \n", bUpdate);
			HxLOG_Print("print all schedules and timer \n");
			SAMA_SCHEDULE_PrintAllSchedules(HLIB_CMD_Printf);
			SAMA_TIMER_PrintAllTimer(HLIB_CMD_Printf);
#endif


			HxLOG_Debug("stSamaConflictCheck.stConflictResult.eType: %d \n", pstContext->stSamaConflictCheck.stConflictResult.eType);

			// �� timer�� timer up notify ���� �ʴ´�.
			hErr = ERR_FAIL;
			break;
		}
#endif

		default:
			break;
	}

	return hErr;
}

#define ___GLOBAL_FUNCTIONS___

HERROR SAMA_SCHEDULE_TVA_Initialize (void)
{
	SAMA_Context_t		*pstContext = SAMA_GetContext();
	HERROR				hErr = ERR_OK;

	HxLOG_Debug("Register Tracking Notifier \n");
	hErr |= SAMALIB_TRACKING_RegisterNotifier(sama_sch_tva_cbScheduleTwoWay);

	HxLOG_Debug("init all series set\n");
	hErr |= SAMALIB_TVA_Init(&(pstContext->stSamaConflictCheck));

	// Schedule DB load�� Series set ������ ����:  schedule�� DateTime���� �ð��� ��� load�� �� ������, series set�� unix time���� �ð��� �־�� load �Ѵ�.
	hErr |= SAMA_SCHEDULE_ForEach(SAMALIB_TVA_UpdateSeriesSetFromDbToRsv, 0, NULL);
	pstContext->bSama_TvaInited = TRUE;

	return hErr;
}

void SAMA_SCHEDULE_TVA_ProcessTwoWayTimer (HUINT32 ulSlot, SAPI_TimerItemType_e eTimerType, unsigned long utTime)
{
	DxSchedule_t		*pstSchedule = SAMA_SCHEDULE_GetSchedule(ulSlot);
	SERIES_SET			*pstSeriesSet = NULL;
	HERROR				 hErr;

	SAMA_Context_t		*pstContext = SAMA_GetContext();
	DxSchedule_RemovedInfo_t stRemovedInfo = {0, };
	HBOOL				bIsPendingBroken = FALSE;

	HxLOG_Trace();

	{
		HCHAR *szTimerStr;
		switch (eTimerType)
		{
		case eSAPI_TIMER_TYPE_None: 			szTimerStr = "NONE"; break;
		case eSAPI_TIMER_TYPE_Ready:				szTimerStr =  "READY"; break;
		case eSAPI_TIMER_TYPE_Start:				szTimerStr =  "START"; break;
		case eSAPI_TIMER_TYPE_Stop: 			szTimerStr =  "STOP"; break;
#ifdef SAMA_LAZY_PADDING_CONFLICT_REMOVE
		case eSAPI_TIMER_TYPE_PaddingStartReady:					szTimerStr =  "READY_PAD_START"; break;
		case eSAPI_TIMER_TYPE_PaddingStopReady: 				szTimerStr =  "READY_PAD_STOP"; break;
#endif
		case eSAPI_TIMER_TYPE_CheckSeriesRecordReady:				szTimerStr =  "READY_CHECK_SERIES_RECORD"; break;
		case eSAPI_TIMER_TYPE_StopPendingBroken:				szTimerStr =  "PENDING_BROKEN"; break;
		case eSAPI_TIMER_TYPE_StopRunaway:				szTimerStr =  "RUNAWAY"; break;
		case eSAPI_TIMER_TYPE_ReadyTracking:			szTimerStr =  "READY_TRACKING"; break;
		case eSAPI_TIMER_TYPE_UpdateDelayed:			szTimerStr =  "UPDATE_DELAYED"; break;

		default: szTimerStr = "UNKNOWN"; break;
		}
		HxLOG_Debug("\033[30;47m ulSlot: %d, eTimerType: %d[%s] \033[0m\n", ulSlot, eTimerType,szTimerStr);
	}

	if (NULL == pstSchedule)
	{
		HxLOG_Error("No Schedule with the Slot ID (%d)\n", ulSlot);
		return;
	}

	hErr = sama_sch_tva_ChangeScheduleStatus(pstSchedule, eTimerType);
	if ( hErr != ERR_OK )
	{
		HxLOG_Debug("Fail to change status. slot(%d) status(%d)[%s], timer(%d) \n", ulSlot, pstSchedule->eStatus,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvStatus_e), pstSchedule->eStatus), eTimerType);
		goto END_FUNC;
	}

	hErr = sama_sch_tva_ProcessTimerConflict(pstContext, pstSchedule, eTimerType);
	if ( hErr != ERR_OK )
	{
		HxLOG_Debug("Fail to process timer conflict. slot(%d) status(%d)[%s], timer(%d) \n", ulSlot, pstSchedule->eStatus,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvStatus_e), pstSchedule->eStatus), eTimerType);
		goto END_FUNC;
	}

	switch (eTimerType)
	{
		case eSAPI_TIMER_TYPE_Ready:
			if (SAMALIB_TRACKING_IsTrackingBySlotId(ulSlot))
			{
				// if is tracking-start-without-running-status-signalling
				if (SAMALIB_TRACKING_GetTransitionTimeBySlotId(ulSlot) == 0)
				{
					UNIXTIME utStartTime;
					UNIXTIME utEndTime;

					HxLOG_Error("Start without running status signalling. slot=(%d) \n",ulSlot);
					HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &utStartTime);
					utEndTime = utStartTime + pstSchedule->ulDuration;
					SAMA_TIMER_InsertTimer(ulSlot, eSAPI_TIMER_TYPE_Stop, pstSchedule->eRsvType, utEndTime, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
				}
				// 1. tracking Running Status Transition�ÿ� ready timer�� �޴´�.
				// 2. sama ���ο����� pause�� ���ѳ���, ready timer�� APK���� ������.
				// 3. APK���� live tuner conflict�� ó���� �Ŀ� resume�ϰų� start error�� �ø���.
				// 4. resume�ÿ� ���� start�� �Ѵ�.
				SAMA_SCHEDULE_PauseSchedule(ulSlot, SAMA_GetVirtualRequestId());
			}
			break;
		case eSAPI_TIMER_TYPE_Start:
			pstSeriesSet = SAMALIB_TVA_GetSeriesSetBySlotId(ulSlot);
			if ( MACRO_IS_TVA_SCHEDULE(pstSchedule, pstSeriesSet) )
			{
				hErr = SAMALIB_TVA_SetCurrentlyRecording(pstSeriesSet, ulSlot);
				if (hErr != ERR_OK)
				{
					HxLOG_Error("SAMALIB_TVA_SetCurrentlyRecording failed. seriesSet=(0x%x), slot=(%d)\n",pstSeriesSet, ulSlot);
				}
			}
			break;
		case eSAPI_TIMER_TYPE_StopPendingBroken:
			bIsPendingBroken = TRUE;
		case eSAPI_TIMER_TYPE_StopRunaway:
			eTimerType = eSAPI_TIMER_TYPE_Stop;
			break;
		default:
			break;
	}

	// Call the timer execution:
	SAMA_PROTOCOL_NotifyTimeUp(ulSlot, eTimerType, pstSchedule);


END_FUNC:
	// Finish: ���� Schedule�� Repeat�� �ƴ϶�� ���⼭ ������ �Ѵ�.
	// tracking schedule �� ��쿡�� end transition�� ������ runaway timer�� ���� �����Ƿ� ������ �����ش�.
	if (SAMA_TIMER_CountRemainingTimer(ulSlot) == 0
		|| (
				MACRO_IS_TRACKING_SCHEDULE(pstSchedule)
				&& eTimerType == eSAPI_TIMER_TYPE_Stop
			))
	{
		HUINT32				 ulRemoveSlot = pstSchedule->ulSlot;
		HBOOL				 bUpdated = FALSE;

		hErr = SAMA_SCHEDULE_AdjustTime(pstSchedule, &bUpdated);
		if ((ERR_OK == hErr) && (TRUE == bUpdated))
		{
			hErr = SAMA_SCHEDULE_UpdateSchedule(ulRemoveSlot, TRUE, pstSchedule, SAMA_GetVirtualRequestId(), TRUE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SAMA_SCHEDULE_UpdateSchedule err: ulSlot(%d)\n", ulRemoveSlot);
			}
		}
		else
		{
			if (bIsPendingBroken == TRUE)
			{
				stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_PendingBroken;
			}
			else
			{
				stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_Completed;
			}
			hErr = SAMA_SCHEDULE_RemoveSchedule(ulRemoveSlot, &stRemovedInfo, FALSE, SAMA_GetVirtualRequestId(), TRUE);
			if (ERR_OK != hErr)
			{
				HxLOG_Error("SAMA_SCHEDULE_RemoveSchedule err: ulSlot(%d)\n", ulRemoveSlot);
			}
		}
	}
	else
	{
		// DLIB�� ���� �����Ѵ�. // rsv status �� ����...
		(void)DAPI_Set(DxNAMEOF(DxSchedule_t), (HUID)pstSchedule->ulSlot, (void*)pstSchedule, sizeof(DxSchedule_t));
		(void)DAPI_Sync(DxNAMEOF(DxSchedule_t));

	}

}

HERROR SAMA_SCHEDULE_TVA_InsertScheduleToTimer (DxSchedule_t *pstSchedule)
{
	UNIXTIME			 utCurrTime;
	UNIXTIME			 utStartTime;
	HERROR				 hErr = ERR_FAIL;
	UNIXTIME			 utReady;
	UNIXTIME			 utPaddingStartTime;
	UNIXTIME			 utPaddingEndTime;

	if (NULL == pstSchedule)					{ return ERR_FAIL; }

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if ((SAMA_SCHEDULE_IsScheduleVolatile(pstSchedule) != TRUE) &&
		(utCurrTime < SAMA_TIME_AVAIL_MIN))
	{
		HxLOG_Debug("VK_CLOCK_GetTime err:\n");
		return ERR_FAIL;
	}

	if (pstSchedule->bRemoved == TRUE)
	{
		HxLOG_Debug("ignore removed schedule \n");
		return ERR_OK;
	}

	hErr = HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSchedule->stStartTime), &utStartTime);
	if (ERR_OK != hErr)
	{
		HxLOG_Error("HLIB_DATETIME_ConvertDateTimeToUnixTime err:\n");
		return ERR_FAIL;
	}

#if 0
	/******************************************************************************
	*
	* DST�� not DST �ð��� ��ġ�� ���� ������ �����.
	*                                           3��           10��
	*  not DST (globalTime)  ------>6          4512----->
	*       DST (localTime)                    6------45
	*  globalTime�� 12������ Unixtime -> localTime�� 45���� HxDATETIME
	*  localTime�� 45������ HxDATETIME -> globalTime�� 45���� Unixtime
	*  �̷� ���� 1�� ����/���ϴ� schedule��
	*  4�� ����/�� �ϴ� ������ �����.
	*
	*  pstSchedule->stStartTime�� localTime�� HxDATETIME�̴�.
	*
	*******************************************************************************/
	{
		UNIXTIME utGlobalTime;
		UNIXTIME utLocalTime = utStartTime;

		(void)utGlobalTime;
		(void)utLocalTime;
	}
#endif
	// ������ ���ýð����� ���� Slot�� ������ Timer ó���� ���� ������ �Ǵ��� �� �ִ�
	utReady = (pstSchedule->eRsvReady > 1) ? (UNIXTIME)pstSchedule->eRsvReady : 2;

	switch (pstSchedule->eRsvType)
	{
	// Two-way based reservation: Ready, Start, Stop:
	case DxRSVTYPE_WATCHING_EBR:
	case DxRSVTYPE_WATCHING_TBR:
	case DxRSVTYPE_WATCHING_SERIES_EBR:
	case DxRSVTYPE_RECORD_EBR:
	case DxRSVTYPE_RECORD_TBR:
	case DxRSVTYPE_RECORD_SERIES_EBR:
	case DxRSVTYPE_RECORD_SATRECORD_TBR:
	case DxRSVTYPE_DESC_REC_STANDBY:
		SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utStartTime, &utPaddingStartTime, &utPaddingEndTime);
		HxLOG_Debug("pstSchedule->bPaused: %d, utCurrTime: %ld, utStartTime: %ld, utEndTime: %ld \n", pstSchedule->bPaused, utCurrTime, utPaddingStartTime, utPaddingEndTime);
#if 0
		if (utCurrTime < utStartTime)
		{
			// nothing to do.
			;
		}
		else
		{

			pstSchedule->bPaused = TRUE;
		}
#endif

		if (MACRO_IS_TRACKING_SCHEDULE(pstSchedule))
		{
			// tracking schedule�� start�� eit pf running status transition���� �ؾ� �Ѵ�.
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_ReadyTracking, pstSchedule->eRsvType, utPaddingStartTime - utReady, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
#ifdef TEST_NORDIG_ACCURATE_RECORD
			// tracking fail �ÿ� schedule time���� �����ϱ� ���� timer
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Ready, pstSchedule->eRsvType, utPaddingStartTime - 30, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
#else
			// tracking fail �ÿ� schedule time + 5min ���� �����ϱ� ���� timer
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Ready, pstSchedule->eRsvType, utPaddingStartTime +300, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
#endif
			// tracking schedule�� tracking fail �ÿ� duration/2�� �ð��� ������ pending broken���� stop ��Ų��.
			// DTR test EITpf streaming�� timing�� �ָ��ؼ� +60�� �� ���ش�.
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_StopPendingBroken, pstSchedule->eRsvType, utStartTime + pstSchedule->ulDuration/2 + 60, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);

			// tracking schedule�� end�� eit pf running status transition�� runnaway time ( 2hr )���� �ؾ� �Ѵ�.
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_StopRunaway, pstSchedule->eRsvType, utPaddingEndTime - 2 + SAMA_TVA_RUNAWAY_TIME, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);

			// tracking schedule�� delayed �Ǿ��� ��� �̸� �ٸ� process�鵵 �˾ƾ� �ϹǷ� �ݿ����ش�.
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_UpdateDelayed, pstSchedule->eRsvType, utPaddingEndTime - SAMA_TVA_UPDATE_DELAYED_SCH_PERIOD, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
		}
		else
		{
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Ready, pstSchedule->eRsvType, utPaddingStartTime - utReady, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Start, pstSchedule->eRsvType, utPaddingStartTime, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Stop, pstSchedule->eRsvType, utPaddingEndTime - SAMA_PADDING_STOP_TIME, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
		}

#ifdef SAMA_LAZY_PADDING_CONFLICT_REMOVE
		if( pstSchedule->eRsvType == DxRSVTYPE_RECORD_EBR ||
			pstSchedule->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR ||
			(pstSchedule->eRsvType == DxRSVTYPE_RECORD_TBR && SAMALIB_SPEC_IsEnableTBRPadding()) )
		{
			if ( ! MACRO_IS_TRACKING_SCHEDULE(pstSchedule) )
			{
				SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_PaddingStartReady, pstSchedule->eRsvType, utPaddingStartTime - utReady - SAMA_PADDING_READY_TIME, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
				SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_PaddingStopReady, pstSchedule->eRsvType, utStartTime + pstSchedule->ulDuration - SAMA_PADDING_READY_TIME, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
			}
		}
#endif


		break;

	case DxRSVTYPE_ACTIVATION_TIME:
		SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Ready, pstSchedule->eRsvType, utStartTime - utReady, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
		SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Start, pstSchedule->eRsvType, utStartTime, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
		SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Stop, pstSchedule->eRsvType, utStartTime + pstSchedule->ulDuration, sama_sch_tva_cbScheduleTwoWay, pstSchedule->bPaused);
		break;

	case DxRSVTYPE_RECORD_INSTANT:
	case DxRSVTYPE_RECORD_DELAYED:
	case DxRSVTYPE_NETSTREAMING_LIVE: // TODO: ��ġ ���.
	case DxRSVTYPE_NETSTREAMING_FILE:
		SAMALIB_TIME_GetPaddingStartEndTime(pstSchedule, utStartTime, &utPaddingStartTime, &utPaddingEndTime);
		HxLOG_Debug("pstSchedule->bPaused: %d, utCurrTime: %ld, utStartTime: %ld, utEndTime: %ld, eStatus: %d \n", pstSchedule->bPaused, utCurrTime, utPaddingStartTime, utPaddingEndTime, pstSchedule->eStatus);
		if(pstSchedule->eStatus == DxRSVSTATUS_RUNNING)
		{
			;
			HxLOG_Error("it's already running schedule. so do not add start timer. \n");
		}
		else
		{
			SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Start, pstSchedule->eRsvType, utPaddingStartTime, sama_sch_tva_cbScheduleTwoWay, FALSE);
		}
		SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Stop, pstSchedule->eRsvType, utPaddingEndTime - 2, sama_sch_tva_cbScheduleTwoWay, FALSE);
		break;

	// One-way based reservation: Ready, Start
	case DxRSVTYPE_POWER_ON:
	case DxRSVTYPE_POWER_OFF:
	case DxRSVTYPE_REGULAR_OTA:
	case DxRSVTYPE_EPG_GATHERING:
	case DxRSVTYPE_EMM:
	case DxRSVTYPE_UPDATE_TASK:
	case DxRSVTYPE_ASO_NOTIFY:
 	case DxRSVTYPE_MYEPG_UPDATE:
	case DxRSVTYPE_NET_UPDATE:
	case DxRSVTYPE_IP_EPG_GATHERING:
	case DxRSVTYPE_IP_REMOTE_PROGRAMMING:
	case DxRSVTYPE_TVTVEPG_TIMER:
	case DxRSVTYPE_CHECK_SATRECORD_TIMER:
		SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Ready, pstSchedule->eRsvType, utStartTime - utReady, sama_sch_tva_cbScheduleOneWay, FALSE);
		SAMA_TIMER_InsertTimer(pstSchedule->ulSlot, eSAPI_TIMER_TYPE_Start, pstSchedule->eRsvType, utStartTime, sama_sch_tva_cbScheduleOneWay, FALSE);
		break;

	default:
		HxLOG_Error("RsvType(%d) not supported:\n", pstSchedule->eRsvType);
		return ERR_FAIL;
	}

	return hErr;
}


HERROR	SAMA_SCHEDULE_TVA_AddCurrentSeriesEvent (DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex, DxEvent_t *pstCurrentSeriesEvent)
{
	SAMA_Context_t			*pstContext = SAMA_GetContext();
	UNIXTIME					utCurrTime;

	HUINT32			i,n;
	HBOOL			bIsTvaSchedule = FALSE;

	HUINT8				ucRecKind;
	HUINT32				ulContentKind;
	HERROR				hErr = ERR_FAIL;
	HERROR				hResult = ERR_FAIL;
	SERIES_SET			*pstSeriesSet= NULL;

	HxVector_t		*vCrid;
	DxEventCRID_t	*contentIdentifier;
	HUINT8			ucCridType;
	HBOOL			bIsSeries = FALSE;
	HBOOL			bIsFirstSeries = FALSE;
	HUINT32 		ulFirstSeriesIndex = 0;

	DxService_t		stService = {0, };
	HCHAR			szDefaultAuth[DxDEFAULT_AUTHORITY_LEN] = {0,};
	DxRSV_TvAnyTimeRecord_t	*pstTvAnyTime = NULL;

	HxLOG_Trace();

	if (pstSchedule == NULL
		|| pstCurrentSeriesEvent == NULL)
	{
		HxLOG_Error("invalid param. pstSchedule=(0x%x), pstCurrentSeriesEvent=(0x%x) \n",pstSchedule, pstCurrentSeriesEvent);
		goto END_FUNC;
	}

	if ( SAMALIB_SVC_GetService(&(pstContext->stSamaConflictCheck), pstCurrentSeriesEvent->svcuid, &stService) != ERR_OK)
	{
		HxLOG_Error("samalib_svc_GetService failed. svcuid=(%d) \n", pstCurrentSeriesEvent->svcuid);
		goto END_FUNC;
	}

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if(UNIX_FIRST_DAY >= utCurrTime)
	{
		HxLOG_Error("time error. current sys time = (%d)\n",utCurrTime);
	}

	HxLOG_Debug("Event name=[%s] received \n", SAMA_EVENT_GETNAME(pstCurrentSeriesEvent->name));

	vCrid		= pstCurrentSeriesEvent->crid;
	if (vCrid == NULL)
	{
		HxLOG_Debug("there is no crid. slot=(%d), event name=[%s] \n",pstSchedule->ulSlot,SAMA_EVENT_GETNAME(pstCurrentSeriesEvent->name));
	}

	/*
	*   Content Kind : event�� crid�� ���� ���� ( series+prog, prog+split, series+split ���� ������ ���� )
	*   Rec kind       : ���� ����� ��ȭ ������ � �ɷ� ����Ǿ����� ( series, single, split ���� ) -> event�� kind�� �� event�� series�� ���� ������ �Է¿� ���� ����
	*   crid type       : rec kind�� �ش��ϴ� Crid�� �����ϱ� ���� ����
	*/
	ulContentKind	= SAMALIB_TVA_GetContentKind(vCrid);
	bIsSeries		= (MACRO_IS_TVA_SERIES_TYPE(pstSchedule->eRsvType))? TRUE: FALSE;
	ucRecKind		= sama_sch_tva_GetRecKind(ulContentKind, bIsSeries);
	if ( ucRecKind == SAMA_SERIES_CONTENT_KIND_SERIES )
	{
		ucCridType = SAMA_CRID_TYPE_SERIES;
	}
	else
	{
		ucCridType = SAMA_CRID_TYPE_PROG;
	}
	HxLOG_Debug("content kind = (%d), reckind=(%d), cridtype=(%d)\n",ulContentKind, ucRecKind, ucCridType);

	if (MACRO_IS_TVA_RECORD_TYPE(pstSchedule->eRsvType))
	{
		pstTvAnyTime = &pstSchedule->uExtInfo.stRec.ex.tva;
	}
	else if (MACRO_IS_TVA_WATCH_TYPE(pstSchedule->eRsvType))
	{
		pstTvAnyTime = &pstSchedule->uExtInfo.stWch.ex.tva;
	}
	else
	{
		HxLOG_Error("Unknown rsv type. type=(%d) \n",pstSchedule->eRsvType);
		hResult = ERR_FAIL;
		goto END_FUNC;
	}


	if ( vCrid != NULL )	{		n = HLIB_VECTOR_Length(vCrid);	}
	else					{		n = 0;	}
	/******************************************************************/
	// ���� Event crid vector
	// SCRID s1     <-seriesindex 0�� ��쿡 s1�� ã��, s1�� crid index�� 1�� �ѱ��
	// PCRID p1
	// RCRID r1
	// RCRID r2
	// SCRID s2     <-seriesindex 1�� ��쿡 s2�� ã��, s2�� crid index�� 4�� �ѱ��
	// SCRID s3     <-seriesindex 4�� ��쿡 s3�� ã��, s3�� crid index�� 5�� �ѱ��.
	/******************************************************************/
	// �� event�� �������� series�� ���� �� �����Ƿ�, ���ڷ� ���� series index���� �����Ѵ�.
	for (i =0 ; i< n ; i++)
	{
		contentIdentifier = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(vCrid, i);
		if (contentIdentifier == NULL)
			continue;

		// ���� ��ȭ�� rec kind�� �ش��ϴ� crid �� �ƴ϶�� �����Ѵ�.
		if ( contentIdentifier->type != ucCridType )
			continue;

		if (bIsFirstSeries == FALSE )
		{
			bIsFirstSeries = TRUE;
			ulFirstSeriesIndex = i;
		}

		// i�� ���ڷ� ���� seriesIndex���� �����ؾ� �Ѵ�.
		if (i < ulSeriesIndex)
			continue;

		// ó�� ������ event�� ������ series set�� �����Ѵ�.
		SAMA_MemSet(szDefaultAuth,0x00,sizeof(HCHAR)*DxDEFAULT_AUTHORITY_LEN);
		SAMALIB_TVA_GetDefaultAuthority(pstCurrentSeriesEvent->svcuid,szDefaultAuth);

		if (pstTvAnyTime->ucRecKind & SAMA_SERIES_CONTENT_KIND_ALTINST
			&& MACRO_IS_TVA_SERIES_TYPE(pstSchedule->eRsvType))
		{
			pstSeriesSet = SAMALIB_TVA_GetAlternativeSeriesSet(pstSchedule->uExtInfo.stRec.nEvtId, pstSchedule->ulSvcUid);
		}

		if (pstSeriesSet == NULL)
		{
			pstSeriesSet = SAMALIB_TVA_NewSeriesSet(contentIdentifier->crid, contentIdentifier->type, szDefaultAuth);
		}

		if ( pstSeriesSet == NULL )
		{
			HxLOG_Error("null series set");
			continue;
		}
		pstSeriesSet->eHdType = stService.highDefinition;
		pstSeriesSet->eRsvType = pstSchedule->eRsvType;
		//SAMALIB_TVA_ReleaseSeriesSet(pstSeriesSet);

#if defined(CONFIG_OP_TDC)
		pstSeriesSet->searchFlag = contentIdentifier->search_flag;
		pstSeriesSet->ulSvcUid = pstSchedule->ulSvcUid;
		if( pstSeriesSet->stCrid.type == DxCRID_TYPE_SERIES && contentIdentifier->title)
		{
			HxSTD_snprintf(pstSeriesSet->szSeriesTitle, DxRSV_CRID_SERIES_TITLE_LEN, "%s", contentIdentifier->title);
		}
#endif
		HxLOG_Debug("slot=(%d), crid=[%s], pstSeriesSet=(0x%x)\n",pstSchedule->ulSlot, contentIdentifier->crid, pstSeriesSet);
		bIsTvaSchedule = TRUE;

		// �� event�� �������� series�� ���� ��� ù��° series���� �־��ش�.
		if (i == ulFirstSeriesIndex)
		{
			if ( pstTvAnyTime->ucRecKind & SAMA_SERIES_CONTENT_KIND_ALTINST )
			{
				/*
				* slot�� ���� ����
				* 1. alternative�� ��쿡�� sapi slot�� ���� programme�� schedule�� ������ �����Ƿ�, ������ �����ϴ�.
				* 2. �׷��Ƿ� sapi slot�� release�� ���ְ� �ӽ÷� SAMA_SERIES_ALTERNATIVE_SLOT�� ����Ѵ�.
				* 3. ������ MakeSchedule�ÿ� sama���� ���� slot�� �޴´�.
				*/
				hErr = SAMA_SCHEDULE_ReleaseSlotForSapi(pstSchedule->ulSlot);
				if ( hErr != ERR_OK )
				{
					HxLOG_Error("release slot failed. slot=(%d)\n",pstSchedule->ulSlot);
				}
				pstSchedule->ulSlot = SAMA_SERIES_ALTERNATIVE_SLOT;
			}

			hErr = SAMALIB_TVA_AddDxEventToSeriesSet(pstCurrentSeriesEvent, pstSeriesSet, utCurrTime, TRUE, pstSchedule->ulSlot);
			if ( hErr != ERR_OK )
			{
				HxLOG_Warning("add DxEvent to series set failed. eventId=(%d), svcUid=(%d)\n",pstCurrentSeriesEvent->eventId, pstCurrentSeriesEvent->svcuid);
				continue;
			}
		}

		break;
	}

	if ( bIsTvaSchedule == TRUE )
	{
		// case 1. SCRID�� ã�� series event
		// case 2. PCRID �� ã�� single event
		HxLOG_Debug("send SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED, ulSlot: %d \n", pstSchedule->ulSlot);
		SAMA_PostMessage(SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED, (HINT32)pstSchedule, (HINT32)i, (HINT32)pstSeriesSet, 0, 0, 0);
	}
	else
	{
		HxLOG_Debug("Possibly failed series. i=(%d)\n",i);
		if (MACRO_IS_TVA_SERIES_TYPE(pstSchedule->eRsvType))
		{
			// case 3. SCRID �� ã�� ���� series event. multiple series�� �������� �׻� ����� �ȴ�.
			HxLOG_Debug("Multiple Series record completed. \n");
			HxLOG_Debug("\033[30;47m send SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM_DONE \033[0m , ulSlot: %d \n", pstSchedule->ulSlot);
			SAMA_PostMessage(SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM_DONE, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
		}
		else
		{
			// case 4. PCRID �� ã�� ���� single event. event�� PCRID�� ���� ��� �����.
			// �ش� event�� conflict�� ����Ű�� schedule�� alternative�� ���� �� �����Ƿ� check next series item ��ƾ�� �¿��� �Ѵ�.
			// check next series item ��ƾ�� Ÿ�� ���� �� crid�� ������ series set�� ����� �ش�.
			HxLOG_Debug("\033[30;47m[%s:%d] Series kind none. slot=(%d) \033[0m \n",__FUNCTION__, __LINE__,pstSchedule->ulSlot);
			pstSeriesSet = SAMALIB_TVA_NewSeriesSet(NULL, 0, 0);
			pstSeriesSet->eRsvType = pstSchedule->eRsvType;
			SAMALIB_TVA_AddDxEventToSeriesSet(pstCurrentSeriesEvent, pstSeriesSet, utCurrTime, TRUE, pstSchedule->ulSlot);
			HxLOG_Debug("send SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM, ulSlot: %d \n", pstSchedule->ulSlot);
			SAMA_PostMessage(SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM, (HINT32)pstSchedule, (HINT32)i, (HINT32)pstSeriesSet, 0, 0, 0);
		}
	}

	hResult = ERR_OK;

END_FUNC:
	if (pstCurrentSeriesEvent != NULL)
	{
		DxEVENT_Delete(pstCurrentSeriesEvent);
	}
	if (hResult != ERR_OK)
	{
		HxLOG_Debug("\033[30;47m[%s:%d] SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL \033[0m \n",__FUNCTION__, __LINE__);
		SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
	}

	return hResult;
}

HERROR	SAMA_SCHEDULE_TVA_AddSeriesEvents (DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex, SERIES_SET *pstSeriesSet, HxList_t *pstSeriesEventList)
{
	DxEvent_t			*Event;
	UNIXTIME			utCurrTime;
	HxList_t			*pvIter;
	HERROR				hErr = ERR_FAIL;
	HERROR				hResult = ERR_FAIL;
	DxRSV_TvAnyTimeRecord_t	*pstTvaRecord = NULL;

	HxLOG_Trace();

	if (pstSchedule == NULL
		|| pstSeriesSet== NULL)
	{
		HxLOG_Error("invalid param. pstSchedule=(0x%x), pstSeriesSet=(0x%x) pstSeriesEventList(0x%x)\n",pstSchedule, pstSeriesSet, pstSeriesEventList);
		goto END_FUNC;
	}

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if(UNIX_FIRST_DAY >= utCurrTime)
	{
		HxLOG_Debug("time error. current sys time = (%d)\n",utCurrTime);
	}

	if (MACRO_IS_TVA_RECORD_TYPE(pstSchedule->eRsvType))
	{
		pstTvaRecord = &(pstSchedule->uExtInfo.stRec.ex.tva);
	}
	else if (MACRO_IS_TVA_WATCH_TYPE(pstSchedule->eRsvType))
	{
		pstTvaRecord = &(pstSchedule->uExtInfo.stWch.ex.tva);
	}
	else
	{
		HxLOG_Error("Not applicapable type. rsv type = (%d) \n",pstSchedule->eRsvType);
		goto END_FUNC;
	}

	// ������ �õ��� schedule�� �����Ѵ�.
	// ������ �õ��� schedule�� �������� series�� ���� �� �����Ƿ�, ù��° series���� �־���� �ϱ� �����̴�.
	for (pvIter = HLIB_LIST_First(pstSeriesEventList); pvIter != NULL; )
	{
		Event = (DxEvent_t *)HLIB_LIST_Data(pvIter);
		pvIter = HLIB_LIST_Next(pvIter);
		if (Event == NULL)
		{
			HxLOG_Debug("null event \n");
			// continue? break???
			continue;
		}

		if (Event->svcuid == pstSchedule->ulSvcUid
			&& Event->eventId == pstSchedule->uExtInfo.stRec.nEvtId)
		{
			HxLOG_Debug("Already added event. skip \n");
			pstSeriesEventList = HLIB_LIST_Remove(pstSeriesEventList, Event);
			DxEVENT_Delete(Event);
			continue;
		}
	}

	hErr = SAMALIB_TVA_AddDxEventsToSeriesSet(pstSeriesEventList, pstSeriesSet);
	if (hErr != ERR_OK)
	{
		HxLOG_Debug("add series events failed. list=(0x%x), series set=(0x%x)\n", pstSeriesEventList, pstSeriesSet);
	}

	if (pstSeriesSet->pstProgList == NULL  // RCT�� recordCrid�� �� ���, 7day epg pool �ȿ� ���� crid prorgamme�� schedule �� �� �ִ�. ( progList == NULL)
		&& pstTvaRecord->ucRecKind & SAMA_SERIES_CONTENT_KIND_CRID)
	{
		HINT32 nUniqueEvtId;
		// tracking ���·� �����ϰ� �������ش�.
		pstSeriesSet->ulSeriesTrackingSlot = pstSchedule->ulSlot;

		utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();

		// web������ ������ ���� unique event id�� �����ؼ� �־��ش�.
		nUniqueEvtId = (HINT32)utCurrTime;
		pstSchedule->uExtInfo.stRec.nEvtId = nUniqueEvtId;

		utCurrTime -= 24*60*60;		// timer�� ���� �Ϸ縸 ���ش�.
		pstSchedule->bRemoved = TRUE;
		pstSchedule->stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_SeriesTracking;
		if (pstSeriesSet->stCrid.type == SAMA_CRID_TYPE_PROG)
		{
			HxSTD_snprintf(pstTvaRecord->szProgCRID,DxRSV_CRID_LEN + 1,"%s%s",(char *)pstSeriesSet->stCrid.auth, (char *)pstSeriesSet->stCrid.data);
		}
		HLIB_DATETIME_ConvertUnixTimeToDateTime(utCurrTime, &pstSchedule->stStartTime);
		SAMA_SCHEDULE_AddSchedule(pstSchedule->ulSlot, pstSchedule, FALSE, SAMA_GetVirtualRequestId());

		hResult = ERR_OK;
		goto END_FUNC;
	}

	HxLOG_Debug("send SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM \n");
	SAMA_PostMessage(SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM, (HINT32)pstSchedule, (HINT32)ulSeriesIndex, (HINT32)pstSeriesSet, 0, 0, 0); //(HINT32)pList, 0, 0, 0, 0);

	hResult = ERR_OK;

END_FUNC:

	HLIB_LIST_RemoveAllFunc(pstSeriesEventList, sama_sch_tva_DeleteEvent_CB);

	if (hResult != ERR_OK)
	{
		HxLOG_Debug("\033[30;47m[%s:%d] SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL \033[0m \n",__FUNCTION__, __LINE__);
		SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
	}

	return hResult;
}

HERROR SAMA_SCHEDULE_TVA_GetEventBySchedule(DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex)
{
	HCHAR		query[1024];
	HUINT32		id;
	SamaTvAnyTime_InternalMsg_t	*msg;
	HERROR		hResult = ERR_FAIL;

	if (NULL == pstSchedule)
	{
		HxLOG_Error("pstSchedule is NULL ! \n");
		goto END_FUNC;
	}

	HxLOG_Debug("epg type = (%d)[%s] \n", pstSchedule->eEpgType ,DLIB_ConvertEnum2Str(DxNAMEOF(DxEPG_Type_e), pstSchedule->eEpgType));

	msg = (SamaTvAnyTime_InternalMsg_t *)SAMA_Malloc(sizeof(SamaTvAnyTime_InternalMsg_t));
	if (msg == NULL)
	{
		HxLOG_Error("malloc failed. \n");
		goto END_FUNC;
	}
	msg->pstSchedule	= pstSchedule;
	msg->p1				= ulSeriesIndex;

	HxSTD_PrintToStrN(query, 1024
					, "SELECT '%s' AS tablename, uid, link, linkdata, svcuid, eventid FROM %s WHERE svcuid=%d AND eventid=%d UNION SELECT '%s' AS tablename, uid, link, linkdata, svcuid, eventid FROM %s WHERE svcuid=%d AND eventid=%d UNION SELECT '%s' AS tablename, uid, link, linkdata, svcuid, eventid FROM %s WHERE svcuid=%d AND eventid=%d"
					,DxNAMEOF(eDxEPG_TYPE_IP),DxNAMEOF(eDxEPG_TYPE_IP), pstSchedule->ulSvcUid , pstSchedule->uExtInfo.stRec.nEvtId
					,DxNAMEOF(eDxEPG_TYPE_PF),DxNAMEOF(eDxEPG_TYPE_PF), pstSchedule->ulSvcUid , pstSchedule->uExtInfo.stRec.nEvtId
					,DxNAMEOF(eDxEPG_TYPE_SC),DxNAMEOF(eDxEPG_TYPE_SC), pstSchedule->ulSvcUid , pstSchedule->uExtInfo.stRec.nEvtId);


	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n",query);
	HxLOG_Debug("==============================================================================\n");
	id = DAPI_Query(query, (DAPI_Getter)sama_sch_tva_OnDamaScheduleEventReceived, (void*)msg);
	HxLOG_Debug("DAPI_Query() id: %u \n", id);

	hResult = ERR_OK;

END_FUNC:
	if (hResult != ERR_OK)
	{
		HxLOG_Debug("\033[30;47m[%s:%d] SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL \033[0m \n",__FUNCTION__, __LINE__);
		SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
	}

	return hResult;
}

HERROR SAMA_SCHEDULE_TVA_GetEventsByCrid (DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex, SERIES_SET *pstSeriesSet)
{
	HCHAR				*query;
	HCHAR				keyword[1024] = {0, }, cridstr[DxRSV_CRID_LEN+1] = {0,};
	HUINT32 			id;
	HERROR				hResult = ERR_FAIL;
	SamaTvAnyTime_InternalMsg_t *msg;

	if (pstSchedule == NULL
		|| pstSeriesSet == NULL )
	{
		HxLOG_Error("NULL param. \n");
		goto END_FUNC;
	}

	msg = (SamaTvAnyTime_InternalMsg_t *)SAMA_Malloc(sizeof(SamaTvAnyTime_InternalMsg_t));
	if (msg == NULL )
	{
		HxLOG_Error("malloc failed. \n");
		goto END_FUNC;
	}

	msg->pstSchedule = pstSchedule;
	msg->p1			 = ulSeriesIndex;
	msg->p2			 = (HINT32)pstSeriesSet;
	if ( pstSeriesSet->stCrid.imi != 0 )
	{
		HxSTD_snprintf(cridstr, DxRSV_CRID_LEN+1, "%s%s", (char*)pstSeriesSet->stCrid.data,(char*)&(pstSeriesSet->stCrid.imi));
	}
	else
	{
		HxSTD_snprintf(cridstr, DxRSV_CRID_LEN+1, "%s", (char*)pstSeriesSet->stCrid.data);
	}
	HxLOG_Debug("Get Events. CRID=[%s]\n",cridstr);

	/***************************
	HxSTD_StrNCat(keyword,"keyword LIKE ",HxSTD_StrLen("keyword like "));
	HxSTD_StrNCat(keyword ," '%",HxSTD_StrLen(" '%"));
	if ( pstSeriesSet->stCrid.type == SAMA_CRID_TYPE_SERIES )
	{
		HxSTD_StrNCat(keyword ,KEYWORD_CRID_SERIES,HxSTD_StrLen(KEYWORD_CRID_SERIES));
	}
	else
	{
		HxSTD_StrNCat(keyword ,KEYWORD_CRID_PROGRAM,HxSTD_StrLen(KEYWORD_CRID_PROGRAM));
	}
	HxSTD_StrNCat(keyword ,"%%",HxSTD_StrLen("%%"));										// match auth
	HxSTD_StrNCat(keyword ,cridstr,HxSTD_StrLen(cridstr));										// match data
	HxSTD_StrNCat(keyword ,KEYWORD_CRID_DLIMITER,HxSTD_StrLen(KEYWORD_CRID_DLIMITER));			// exact match
	HxSTD_StrNCat(keyword ,"%' ",HxSTD_StrLen("%' "));
	OR as belows ****************************/
	if (pstSeriesSet->stCrid.type == SAMA_CRID_TYPE_SERIES )
	{
		HxSTD_snprintf(keyword,1024,"keyword LIKE '%%%s%%%s%s%%'",KEYWORD_CRID_SERIES, cridstr, KEYWORD_CRID_DLIMITER);
	}
	else
	{
		HxSTD_snprintf(keyword,1024,"keyword LIKE '%%%s%%%s%s%%'",KEYWORD_CRID_PROGRAM, cridstr, KEYWORD_CRID_DLIMITER);
	}

	query = SAMALIB_TVA_MakeCridQuery(keyword, pstSeriesSet);

	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n", query);
	HxLOG_Debug("==============================================================================\n");

	id = DAPI_Query(query, (DAPI_Getter)sama_sch_tva_OnDamaCridEventsReceived, (void*)msg);
	HxLOG_Debug("DAPI_Query() id: %u \n", id);

	if (query)
		HLIB_RWSTREAM_MemFree(query);

	hResult = ERR_OK;
END_FUNC:
	if (hResult != ERR_OK)
	{
		HxLOG_Debug("\033[30;47m[%s:%d] SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL \033[0m \n",__FUNCTION__, __LINE__);
		SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
	}

	return hResult;

}

HERROR SAMA_SCHEDULE_TVA_MakeCridSchedule (DxSchedule_t *pstSchedule)
{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	SAMA_Context_t		*pstContext = SAMA_GetContext();
	DxService_t			stService;
	TVA_CONTENTID_T		stCrid;
	DxRSV_TvAnyTimeRecord_t	*pstTvaRecord = NULL;
#endif
	HCHAR 				*szCrid;
	HUINT32				ulCridType;
	SERIES_SET 			*pstSeriesSet;
	HERROR				hResult = ERR_FAIL;


	HxLOG_Debug(" Make Crid Schedule. \n");

	if (pstSchedule == NULL)
	{
		goto END_FUNC;
	}

	if (MACRO_IS_TVA_RECORD_TYPE(pstSchedule->eRsvType))
	{
		pstTvaRecord = &(pstSchedule->uExtInfo.stRec.ex.tva);
	}
	else if (MACRO_IS_TVA_WATCH_TYPE(pstSchedule->eRsvType))
	{
		pstTvaRecord = &(pstSchedule->uExtInfo.stWch.ex.tva);
	}
	else
	{
		HxLOG_Error("Not applicapable type. rsv type=(%d) \n", pstSchedule->eRsvType);
		goto END_FUNC;
	}

	szCrid = pstTvaRecord->szCRID;
	ulCridType = pstTvaRecord->ucCRIDType;

	// 1. default authority set
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	SAMALIB_SVC_GetService(&pstContext->stSamaConflictCheck, pstSchedule->ulSvcUid, &stService);
	if (HxSTD_StrEmpty(stService.ex.tva.szDefaultAuth))
	{
		(void)SAMALIB_TVA_ExtractEachPartCRID(szCrid, HLIB_STD_StrNLen((char *)szCrid,DxRSV_CRID_LEN), &stCrid, NULL );
	}
	else
	{
		(void)SAMALIB_TVA_ExtractEachPartCRID(szCrid, HLIB_STD_StrNLen((char *)szCrid,DxRSV_CRID_LEN), &stCrid, (int*)stService.ex.tva.szDefaultAuth );
	}
	if ( stCrid.imi != 0 )
	{
		HxSTD_snprintf(szCrid, DxRSV_CRID_LEN+1, "%s%s%s", (char*)stCrid.auth, (char*)stCrid.data,(char*)&stCrid.imi);
	}
	else
	{
		HxSTD_snprintf(szCrid, DxRSV_CRID_LEN+1, "%s%s", (char*)stCrid.auth, (char*)stCrid.data);
	}
#endif
	HxLOG_Debug("type=(%d), crid = [%s] \n",ulCridType, szCrid);

	// 2. check if already tracking
	pstSeriesSet = SAMALIB_TVA_GetSeriesSetByCRID(szCrid, ulCridType, NULL);
	if (pstSeriesSet != NULL)
	{
		HxLOG_Error("Already Made Schedule. \n");
		goto END_FUNC;
	}

	/*********************************
	* crid schedule�̶�� ��, crid�� tracking �ǰ� �ִ� schedule �̴�.
	* �׷��Ƿ� remove �Ǿ� �־�� �ϸ�, reason ���� series tracking�̿��� �Ѵ�.
	* RemoveSchedule�� series tracking�� �������, tracking�� �ƴ� ���¿��� tracking���� ���� �� �����ϹǷ�
	* �������� ���� ��� �������ش�.
	**********************************/

	// 3. start tracking (= newSeriesSet)
	pstSeriesSet = SAMALIB_TVA_NewSeriesSet(szCrid, ulCridType, NULL);
	if (pstSeriesSet == NULL)
	{
		HxLOG_Error("SAMALIB_TVA_NewSeriesSet failed. \n");
		goto END_FUNC;
	}
	pstSeriesSet->eRsvType = pstSchedule->eRsvType;

	HxLOG_Debug("\033[30;47m[%s:%d] SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED \033[0m \n",__FUNCTION__, __LINE__);
	SAMA_PostMessage(SAMAMESSAGE_CURRENT_SERIES_ITEM_RECEIVED, (HINT32)pstSchedule, (HINT32)0, (HINT32)pstSeriesSet, 0, 0, 0); //(HINT32)pList, 0, 0, 0, 0);

	hResult = ERR_OK;
END_FUNC:
	if (hResult != ERR_OK)
	{
		HxLOG_Debug("\033[30;47m[%s:%d] SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL \033[0m \n",__FUNCTION__, __LINE__);
		SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
	}

	return hResult;
}
HERROR SAMA_SCHEDULE_TVA_MakeSeriesSchedules (DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex, SERIES_SET *pstSeriesSet)
{
	SAMA_Context_t			*pstContext = SAMA_GetContext();
	HERROR					hErr = ERR_FAIL;
	HERROR					hResult = ERR_FAIL;
	SAPI_ConflictType_e		eConflictResult;
	HBOOL					bCancel;

	PROGRAM_SET				*pstProgSet;
	EVENT_PERIOD			*pstEventPeriod;

	DxSchedule_t		*pstScheduleArray = NULL;
	HUINT32				ulSchedTotal;
	SAPI_Conflict_t		stConflictResult;

	HxLOG_Trace();

	if(pstSchedule == NULL)
	{
		HxLOG_Error("pstSchedule == NULL \n");
		goto END_FUNC;
	}

	// SAMALIB_TVA �ȿ��� SAMALIB_CFLT�� ȣ�� �ϰ�, �� �� context ���� schedule list�� ����ϹǷ�, �� ������ �־��ش�.
	if (NULL != pstContext->stSamaConflictCheck.pstScheduleList)
	{
		pstContext->stSamaConflictCheck.pstScheduleList = HLIB_LIST_RemoveAllFunc (pstContext->stSamaConflictCheck.pstScheduleList, SAMA_Free_CB);
	}
	SAMA_SCHEDULE_TreeValueToList(SAMA_SCHEDULE_GetScheduleTree(), &(pstContext->stSamaConflictCheck.pstScheduleList));

	hErr = SAMA_SCHEDULE_GetAllSchedules(&ulSchedTotal, &pstScheduleArray);
	if ( hErr != ERR_OK )
	{
		HxLOG_Debug("SAMA_SCHEDULE_GetAllSchedules failed. \n");
		ulSchedTotal = 0;
		// TODO: schedule array�� ���� ó��
	}

	/* Make Series Set of Non Series Reservations */
	hErr = SAMALIB_TVA_PrepareConflictChecking(ulSchedTotal,pstScheduleArray);
	if ( hErr != ERR_OK )
	{
		HxLOG_Debug("prepare conflict checking failed. seriesIndex=(%d) seriesSet=(0x%x) \n",ulSeriesIndex, pstSeriesSet);
	}

	/**************************************************************************/
	// SAMALIB_CFLT�� conflict check�� ���� conflict ������ pstContext->stSamaConflictCheck�� �Ѱ��� �ְ�,
	// conflict result�� ���� conflict ������ local stConflictResult ������ �޾ƿ´�.

	// result�� check�� ���� ����ü�� �������� �� ���� ������ ����ϴ� ������
	// SAMALIB_TVA �ȿ��� conflict check�� ������ �������� ������, result�� ���� ������ SAMALIB_TVA �ȿ��� ��������� �ϰ�,
	// �Ϲ����� conflict check�� ����� context->stSamaConflictcheck�� �޾ƿ� �� �ֱ� �����̴�.
	/**************************************************************************/
	HxSTD_memset(&stConflictResult, 0x00, sizeof(SAPI_Conflict_t));
	eConflictResult = SAMALIB_TVA_CheckScheduleConflict(pstSeriesSet, &stConflictResult);
	if ( eConflictResult != eSAPI_CONFLICT_TYPE_None )
	{
		HxLOG_Debug("conflict occured. seriesIndex=(%d) seriesSet=(0x%x) \n",ulSeriesIndex, pstSeriesSet);
		HxLOG_Debug("conflict occured. TVA result=(%d) CFLT result=(%d) context=(%d)\n",eConflictResult, stConflictResult.eType, pstContext->stSamaConflictCheck.stConflictResult.eType);

		// conflict�� �� series�� event�� ���� series �� ù��° series�� �ƴ϶�� conflict�� �����Ѵ�. ( ù��° series �� pstSchedule->ulSlot�� ���� schedule�� �����ȴ�. )
		// �ֳ��ϸ� ù��° series�� ������ �ߴµ� ���� series�� conflict�� ���� ��쿡 ���� �ó������� ���ϱ� �����ٰ�,
		//               live���� multiple series�� ������ ��쵵 ����, �̿� ���� �ó������� ����� �� ���ʿ��� ���̱� �����̴�. �ᱹ �����ϴ� ������ ���⼭ ������.
		if (SAMA_SCHEDULE_GetSchedule(pstSchedule->ulSlot) != NULL)
		{
			HxLOG_Error("Conflict occured while making multiple series. series Index=(%d) \n",ulSeriesIndex);
			HxLOG_Error("Ignore this conflict And don't notify. \n");
			hErr = SAMALIB_TVA_ConcludeConflictChecking(pstSeriesSet, TRUE);
			if (hErr != ERR_OK)
			{
				HxLOG_Debug("SAMALIB_TVA_ConcludeConflictChecking failed. Null slot\n");
			}
			HxLOG_Debug("\033[30;47m[%s:%d] SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM_DONE \033[0m \n",__FUNCTION__, __LINE__);
			SAMA_PostMessage(SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM_DONE, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
			hResult = ERR_OK;
			goto END_FUNC;
		}

		(void) SAMALIB_TVA_ClearAlternativeItems(&(pstContext->stSamaConflictCheck.stAltItems));

		hErr = sama_sch_tva_GenerateAltInfo(pstSchedule,&(pstContext->stSamaConflictCheck.stAltItems), &stConflictResult);
		if ( hErr != ERR_OK )
		{
			HxLOG_Debug("generate alt info failed. slot=(%d) \n",pstSchedule->ulSlot);
		}

		if (MACRO_IS_TVA_RECORD_TYPE(pstSchedule->eRsvType))
		{
			if (pstContext->stSamaConflictCheck.stAltItems.ulNumAlternatives > 0 )
			{
				// �ó������� ���� alternative�� �ִ� schedule�� conflict�� ���� ��Ű�� �ʴ´�.
				sama_sch_tva_ExceptConflictItems(&stConflictResult.ulNumConflict, stConflictResult.pstConflictArray,&(pstContext->stSamaConflictCheck.stAltItems));

				SAMA_PROTOCOL_NotifyAlternativeAddTried(pstSchedule
														, eSAPI_CONFLICT_TYPE_Alternative
														, stConflictResult.ulNumConflict
														, stConflictResult.pstConflictArray
														, &(pstContext->stSamaConflictCheck.stAltItems)
														, SAMA_GetVirtualRequestId());
			}
			else
			{
				if (eConflictResult == eSAPI_CONFLICT_TYPE_Tp)
				{
					/* Tp conflict is used for satelite box. Terrestrial&Cable box doesn't need to handle Tp conflict.
					    If conflict occured and do need to cancel multiple schedules at once, then use TP conflict */
					HxLOG_Error("Tp Conflicted. Change it to over-capacity-conflict \n");
					eConflictResult = eSAPI_CONFLICT_TYPE_OverCapacity;
				}
				SAMA_PROTOCOL_NotifyConflictAddTried(pstSchedule
														, eConflictResult
														, stConflictResult.ulNumConflict
														, stConflictResult.pstConflictArray
														, SAMA_GetVirtualRequestId());
			}
		}
		else if (MACRO_IS_TVA_WATCH_TYPE(pstSchedule->eRsvType))
		{
			//�޴��ʿ��� alternative watch�� ���� �غ� �ȵǾ� �����Ƿ� watch�� alternative�� �����ϴ��� �ø��� �ʴ´�.
			if (pstContext->stSamaConflictCheck.stAltItems.ulNumAlternatives > 0 )
			{
				HxLOG_Error("There is possible scenario of alternative. But ignore it \n");
			}
			SAMA_PROTOCOL_NotifyConflictAddTried(pstSchedule
													, eConflictResult
													, stConflictResult.ulNumConflict
													, stConflictResult.pstConflictArray
													, SAMA_GetVirtualRequestId());
		}
		/* Remove Series Set of Non Series Reservations */
		bCancel = TRUE;
		hErr = SAMALIB_TVA_ConcludeConflictChecking(pstSeriesSet, bCancel);
		if (hErr != ERR_OK)
		{
			HxLOG_Debug("SAMALIB_TVA_ConcludeConflictChecking failed. slot=(%d)\n",pstSchedule->ulSlot);
		}

		goto END_FUNC;

	}

	HxLOG_Debug("Make series schedules. seriesIndex=(%d) seriesSet=(0x%x) \n",ulSeriesIndex, pstSeriesSet);
	SAMALIB_TVA_PrintSeriesSet(pstSeriesSet);
	for ( pstProgSet = pstSeriesSet->pstProgList; pstProgSet != NULL; pstProgSet = pstProgSet->pNext )
	{
		for ( pstEventPeriod = pstProgSet->pstSelectedInst->pstEventList
			  ; pstEventPeriod != NULL
			  ; pstEventPeriod = pstEventPeriod->pNext)
		{
			DxSchedule_t *pstNewSchedule;

			pstNewSchedule= (DxSchedule_t *)SAMA_Malloc(sizeof(DxSchedule_t));
			if (pstNewSchedule == NULL )
			{
				HxLOG_Error("malloc failed. \n");
				continue;
			}

			hErr = SAMALIB_TVA_ConstructScheduleWithSeries(pstNewSchedule, pstEventPeriod);
			if (hErr != ERR_OK)
			{
				HxLOG_Debug("constrcut scheduled failed. \n");
				SAMA_Free(pstNewSchedule);
				continue;
			}

			// TVA LIB �ȿ��� ó�� ���ϴ� ������ �Ʒ����� ó�����ش�.
			if (pstEventPeriod->flag & SAMA_EVENT_ADDED)
			{
				HxLOG_Debug("null slot event period. crid=[%s]\n",SAMALIB_TVA_ConvertCridToStr(&(pstSeriesSet->stCrid)));
				hErr = SAMA_SCHEDULE_GetEmptySlotForSama(&(pstNewSchedule->ulSlot));
				if (hErr != ERR_OK)
				{
					HxLOG_Error("There is no empty slot for sama!\n");
					SAMA_Free(pstNewSchedule);
					continue;
				}
			}
			if (MACRO_IS_TVA_RECORD_TYPE(pstNewSchedule->eRsvType))
			{
				hErr = HAPI_GetRegistryInt("startpadding", (HUINT32 *)&pstNewSchedule->uExtInfo.stRec.ulPreOffsetTime);
				hErr |= HAPI_GetRegistryInt("endpadding", (HUINT32 *)&pstNewSchedule->uExtInfo.stRec.ulPostOffsetTime);
				if (hErr == ERR_FAIL)
				{
					HxLOG_Error("Get padding time error \n");
				}
			}
			if ( MACRO_IS_TRACKING_SCHEDULE(pstNewSchedule))
			{
				pstNewSchedule->eRsvReady = DxRSVREADY_TVA_TRACKING;
			}
			else
			{
				pstNewSchedule->eRsvReady = DxRSVREADY_30_SEC; // opl scheduler���� ���� ���� �����ϰ� ��������. ������ ���� ��� ���� ����
			}

			HxLOG_Debug("adding schedule. slot=(%d), name=[%s] \n",pstNewSchedule->ulSlot, pstNewSchedule->uExtInfo.stRec.szEventName);

			if ( pstEventPeriod->flag & SAMA_EVENT_ADDED )
			{
				HxLOG_Debug("added ok \n");
				pstEventPeriod->flag &= ~SAMA_EVENT_ADDED;
				pstEventPeriod->ulSlot = pstNewSchedule->ulSlot;
				(void)SAMA_SCHEDULE_AddSchedule(pstNewSchedule->ulSlot, pstNewSchedule, TRUE, SAMA_GetVirtualRequestId());
			}
			else
			{
				HxLOG_Debug("original sapi add schedule. \n");
				(void)SAMA_SCHEDULE_AddSchedule(pstNewSchedule->ulSlot, pstNewSchedule, FALSE, SAMA_GetVirtualRequestId());
			}

			// RCT�� ���� recordCrid�� �� ��� HD ����
			if ((MACRO_IS_TVA_RECORD_TYPE(pstSchedule->eRsvType)&& pstSchedule->uExtInfo.stRec.ex.tva.ucRecKind & SAMA_SERIES_CONTENT_KIND_CRID)
				||( MACRO_IS_TVA_WATCH_TYPE(pstSchedule->eRsvType)&& pstSchedule->uExtInfo.stWch.ex.tva.ucRecKind & SAMA_SERIES_CONTENT_KIND_CRID))
			{
				sama_sch_tva_UpdateHdInfo(pstSeriesSet, pstContext, pstNewSchedule);
			}

			SAMA_Free(pstNewSchedule);
			pstNewSchedule = NULL;
		}
	}

	/* Remove Series Set of Non Series Reservations */
	bCancel = FALSE;
	(void)SAMALIB_TVA_ConcludeConflictChecking(pstSeriesSet, bCancel);

	// RCT�� ���� recordCrid�� �� ��쿡 ���� slot ����
	if ((MACRO_IS_TVA_RECORD_TYPE(pstSchedule->eRsvType)&& pstSchedule->uExtInfo.stRec.ex.tva.ucRecKind & SAMA_SERIES_CONTENT_KIND_CRID)
		||( MACRO_IS_TVA_WATCH_TYPE(pstSchedule->eRsvType)&& pstSchedule->uExtInfo.stWch.ex.tva.ucRecKind & SAMA_SERIES_CONTENT_KIND_CRID))
	{
		// recordCrid�� SAPI_MakeSchedule �� slot�� release ���ش�.
		HxLOG_Debug(HxANSI_COLOR_GREEN("recordCrid tried. it is normal for next error message  \n"));
		hResult = ERR_FAIL;
		goto END_FUNC;
	}

	if (MACRO_IS_TVA_SERIES_TYPE(pstSchedule->eRsvType))
	{
		// �� event�� �������� series CRID �� ���� ���, CRID �ϳ� ���� check series record �޼����� Ÿ���� ������ ����.
		HxLOG_Debug("Check Next Series. series index (%d) -> (%d) \n", ulSeriesIndex, ulSeriesIndex+1);
		SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD, (HINT32)pstSchedule, ulSeriesIndex+1, 0, 0, 0, 0);

		if ( pstScheduleArray != NULL ) { SAMA_Free(pstScheduleArray);}

		// END_FUNC�� Ÿ�� series item done msg�� ������ schedule�� free�Ѵ�. ���� �ʾƾ� �ϹǷ� ���⼭ return
		return ERR_OK;
	}

	hResult = ERR_OK;

END_FUNC:
	if (hResult != ERR_OK)
	{
		HxLOG_Debug("\033[30;47m[%s:%d] SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL \033[0m \n",__FUNCTION__, __LINE__);
		SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
	}
	else
	{
		HxLOG_Debug("\033[30;47m[%s:%d] SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM_DONE \033[0m \n",__FUNCTION__, __LINE__);
		SAMA_PostMessage(SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM_DONE, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
	}

	if ( pstScheduleArray != NULL ) { SAMA_Free(pstScheduleArray);}

	return hResult;

}

HERROR SAMA_SCHEDULE_TVA_MakeAlternativeSeriesSchedules (DxSchedule_t *pstSchedule, HUINT32 ulSeriesIndex, SERIES_SET *pstSeriesSet)
{
	SAMA_Context_t			*pstContext = SAMA_GetContext();
	HERROR					hErr = ERR_FAIL;
	HERROR					hResult = ERR_FAIL;
	HBOOL					bCancel;

	DxSchedule_t		*pstScheduleArray = NULL;
	HUINT32				ulSchedTotal;

	TVA_ALTERNATIVE_INFO		stAltInfo;

	HxLOG_Trace();

	if(pstSchedule == NULL)
	{
		HxLOG_Error("pstSchedule == NULL \n");
		goto END_FUNC;
	}

	// SAMALIB_TVA �ȿ��� SAMALIB_CFLT�� ȣ�� �ϰ�, �� �� context ���� schedule list�� ����ϹǷ�, �� ������ �־��ش�.
	if (NULL != pstContext->stSamaConflictCheck.pstScheduleList)
	{
		pstContext->stSamaConflictCheck.pstScheduleList = HLIB_LIST_RemoveAllFunc (pstContext->stSamaConflictCheck.pstScheduleList, SAMA_Free_CB);
	}
	SAMA_SCHEDULE_TreeValueToList(SAMA_SCHEDULE_GetScheduleTree(), &(pstContext->stSamaConflictCheck.pstScheduleList));

	hErr = SAMA_SCHEDULE_GetAllSchedules(&ulSchedTotal, &pstScheduleArray);
	if ( hErr != ERR_OK )
	{
		HxLOG_Debug("SAMA_SCHEDULE_GetAllSchedules failed. \n");
		ulSchedTotal = 0;
		// TODO: schedule array�� ���� ó��
	}

	/* Make Series Set of Non Series Reservations */
	hErr = SAMALIB_TVA_PrepareConflictChecking(ulSchedTotal,pstScheduleArray);
	if ( hErr != ERR_OK )
	{
		HxLOG_Debug("prepare conflict checking failed. seriesIndex=(%d) seriesSet=(0x%x) \n",ulSeriesIndex, pstSeriesSet);
	}

	while (1)
	{
		hErr = SAMALIB_TVA_GetNextAlternativeInfo(pstSeriesSet, pstSchedule, &stAltInfo);
		if (hErr != ERR_OK)
			break;
		if (stAltInfo.numAlternative == 0)
			break;
		}

	hErr = SAMALIB_TVA_SetSelectedAsCandidate(pstSeriesSet);
	if (hErr != ERR_OK)
	{
		HxLOG_Warning("SAMALIB_TVA_SetSelectedAsCandidate failed. \n");
	}

	// 2. update series set
	HxLOG_Debug("Make Alternative Series Schedule. \n");
	SAMALIB_TVA_PrintSeriesSet(pstSeriesSet);
	hErr = sama_sch_tva_UpdateSeriesSet(pstSeriesSet,pstContext);
	if (hErr != ERR_OK)
	{
		HxLOG_Warning("sama_sch_tva_UpdateSeriesSet failed. \n");
	}

	/* Remove Series Set of Non Series Reservations */
	bCancel = FALSE;
	(void)SAMALIB_TVA_ConcludeConflictChecking(pstSeriesSet, bCancel);

	HxLOG_Debug("\033[30;47m[%s:%d] SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM_DONE \033[0m \n",__FUNCTION__, __LINE__);
	SAMA_PostMessage(SAMAMESSAGE_CHECK_NEXT_SERIES_ITEM_DONE, (HINT32)pstSchedule, 0, 0, 0, 0, 0);

	hResult = ERR_OK;

END_FUNC:
	if (hResult != ERR_OK)
	{
		HxLOG_Debug("\033[30;47m[%s:%d] SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL \033[0m \n",__FUNCTION__, __LINE__);
		SAMA_PostMessage(SAMAMESSAGE_CHECK_SERIES_RECORD_FAIL, (HINT32)pstSchedule, 0, 0, 0, 0, 0);
	}

	if ( pstScheduleArray != NULL ) { SAMA_Free(pstScheduleArray);}

	return hResult;

}



HERROR SAMA_SCHEDULE_TVA_RemoveSchedule (HUINT32 ulSlot,DxSchedule_t *pstSchedule, DxSchedule_RemovedInfo_t *pstRemovedInfo)
{
	SERIES_SET		*pstSeriesSet = NULL;
	TVA_CONTENTID_T	stDummyCrid;
	HERROR			hErr;
	DxSchedule_t	*pstSeriesTrackingSchedule = NULL;

	if ( pstSchedule == NULL )
	{
		HxLOG_Error("null schedule\n");
		return ERR_FAIL;
	}

	// tracking ���� ���� stop tracking, pending restart�� ���ش�.
	if ( SAMALIB_TRACKING_IsTrackingBySlotId(ulSlot) )
	{
		(void)SAMALIB_TRACKING_StopTracking(pstSchedule->ulSvcUid, pstSchedule->uExtInfo.stRec.nEvtId);
	}

	if (pstSchedule->eStatus == DxRSVSTATUS_RUNNING )
	{
		if ( SAMALIB_TRACKING_IsTrackingBySlotId(ulSlot)
			|| MACRO_IS_CONFLICT_TYPE(pstSchedule->eRsvType) )
		{
			DxSchedule_t	*pstStopedSchedule = pstSchedule;
			UNIXTIME		utCurrTime, utStartTime;

			utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
			HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstStopedSchedule->stStartTime), &utStartTime);

			// restart conflict check�� ���� ������ tracking early OR delay �Ǿ� ����� �ð����� duration�� ������ �ش�.
			pstStopedSchedule->ulDuration = utCurrTime-utStartTime;

			HxLOG_Debug("Stop running tracking schedule. 1. stop tracking. 2. restart pending schedules \n");
			SAMALIB_TRACKING_RestartPendingSchedules();
		}
	}

	if ( !MACRO_IS_TVA_SCHEDULE_TYPE(pstSchedule->eRsvType))
	{
		HxLOG_Debug("not a series type. rsv type=(%d)[%s]\n",pstSchedule->eRsvType,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), pstSchedule->eRsvType));
		return ERR_OK;
	}

	pstSeriesSet = SAMALIB_TVA_GetSeriesSetBySlotId(ulSlot);
	if ( pstSeriesSet == NULL )
	{
		HxLOG_Debug("null series set. slot=(%d)\n", ulSlot);
		return ERR_OK;
	}

	hErr = SAMALIB_TVA_RemoveMatchingEventPeriod(pstSchedule->ulSvcUid, pstSchedule->uExtInfo.stRec.nEvtId, pstSeriesSet, &stDummyCrid);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error("remove matching event period failed. svcid=(%d), evtid=(%d)\n",pstSchedule->ulSvcUid, pstSchedule->uExtInfo.stRec.nEvtId);
	}

	// user �Է¿� ���Ҷ��� sama protocol TvAnyTime���� series set�� ������ ����������
	// ��ȭ�� ���� �������� ����� 13�� tracking�� ���� series tracking schedule�� ���� �д�.
	if ( pstSeriesSet->pstProgList == NULL)						// series�� �����ִ� episode�� ����
	{
		if (pstSeriesSet->stCrid.type == SAMA_CRID_TYPE_SERIES
			&& pstRemovedInfo->eRemovedReason != DxSchedule_RemovedReason_ServiceDeleted)	// service delete�� ����� ��� tracking�� ���� ���Ѿ� �Ѵ�.
		{
			HINT32	nUniqueEvtId;

			HxLOG_Warning("Series's last schedule is delete. Add Series Tracking schedule. \n");

			pstSeriesTrackingSchedule = (DxSchedule_t *)SAMA_Malloc(sizeof(DxSchedule_t));
			if (pstSeriesTrackingSchedule != NULL)
			{
				SAMA_MemCpy(pstSeriesTrackingSchedule, pstSchedule, sizeof(DxSchedule_t));

				(void)SAMA_SCHEDULE_GetEmptySlotForSama(&(pstSeriesTrackingSchedule->ulSlot));

				pstSeriesTrackingSchedule->bRemoved = TRUE;
				pstSeriesTrackingSchedule->stRemovedInfo.eRemovedReason = DxSchedule_RemovedReason_SeriesTracking;

				if (MACRO_IS_TVA_RECORD_TYPE(pstSeriesTrackingSchedule->eRsvType))
				{
					pstSeriesTrackingSchedule->uExtInfo.stRec.ex.tva.ucRecKind |= SAMA_SERIES_CONTENT_KIND_CRID;
				}
				else if (MACRO_IS_TVA_WATCH_TYPE(pstSeriesTrackingSchedule->eRsvType))
				{
					pstSeriesTrackingSchedule->uExtInfo.stWch.ex.tva.ucRecKind |= SAMA_SERIES_CONTENT_KIND_CRID;
				}
				else
				{
					HxLOG_Error("Not applicapable type. rsv type=(%d) \n",pstSeriesTrackingSchedule->eRsvType);
				}

				{
					UNIXTIME utStartTime;
					HLIB_DATETIME_ConvertDateTimeToUnixTime(&(pstSeriesTrackingSchedule->stStartTime), &utStartTime);
					utStartTime -= 24*60*60;	// OPL�� web���� slot�� �ƴ� start time�� üũ�ϹǷ� �ٸ� ������ �ٲ��ֵ��� ���ش�.
					HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime, &(pstSeriesTrackingSchedule->stStartTime));
				}
				nUniqueEvtId = (UNIXTIME)HLIB_STD_GetSystemTime();
				pstSeriesTrackingSchedule->uExtInfo.stRec.nEvtId = nUniqueEvtId;

				HxLOG_Debug(HxANSI_COLOR_GREEN("New series tracking. slot=(%d) \n"),pstSeriesTrackingSchedule->ulSlot);

				SAMA_SCHEDULE_AddSchedule(pstSeriesTrackingSchedule->ulSlot,pstSeriesTrackingSchedule,TRUE, SAMA_GetVirtualRequestId());

				pstSeriesSet->ulSeriesTrackingSlot = pstSeriesTrackingSchedule->ulSlot;
				SAMA_Free(pstSeriesTrackingSchedule);
			}
		}
		else
		{
			SAMALIB_TVA_DeleteSeriesSet(pstSeriesSet);
		}
	}

	return ERR_OK;
}
HERROR SAMA_SCHEDULE_TVA_UpdateSeries (void)
{
	SAMA_Context_t			*pstContext = SAMA_GetContext();
	HERROR					hErr = ERR_OK;

	// ����: recording�� �ϳ��� ���� ��쿡�� update series�� �� �� ����. -> update series�� �ʿ��ѵ� recording�� �ϳ��� ���� ���� ���ٰ� ����....
	// TODO: recording�� ���� ��쿡�� scan �� recording count�� 0�̱� ������ oapi record add message�� ���� �� ����.
	// TODO: �׷��Ƿ� recording�� ���� ����� ���� sama�� �˱� ���ؼ��� oapi record scan done�� ����ų�, record count�� 0�� ��쿡�� add �� �� �ֵ��� ���踦 �����ؾ� �Ѵ�.
	/*
	if ( s_bIsRecordingsLoaded == FALSE )
	{
		HxLOG_Error("recordings not loaded yet. \n");
		return ERR_FAIL;
	}
	*/

	if (pstContext->bSama_TvaInited != TRUE)
	{
		HxLOG_Error("Tva not inited \n");
		return ERR_FAIL;
	}

	hErr = sama_sch_tva_IsAllowableUpdateSeries();
	if (hErr != ERR_OK)
	{
		HxLOG_Debug(HxANSI_COLOR_RED("Update serise not allowable.  \n"));
		return ERR_FAIL;
	}

	hErr = sama_sch_tva_UpdateAllSeriesSet(pstContext);
	if (hErr != ERR_OK)
	{
		HxLOG_Debug(HxANSI_COLOR_RED("Update serise failed.  \n"));
		return ERR_FAIL;
	}

	return ERR_OK;
}

HBOOL SAMA_SCHEDULE_TVA_KeepRemoveSchedule (SAMA_Context_t *pstContext, DxSchedule_t *pstSchedule, DxSchedule_RemovedReason_e eRemoveReason)
{
	SAPI_ActionType_e	eActionType = eSAPI_ACTION_TYPE_None;

	if(pstContext == NULL || pstSchedule == NULL)
	{
		HxLOG_Error("pstContext == NULL || pstSchedule == NULL \n");
		return FALSE;
	}

	if ( eRemoveReason == DxSchedule_RemovedReason_SeriesTracking)
	{
		HxLOG_Debug("Keep schedule condition. slot=(%d), removedReason=(%d) \n",pstSchedule->ulSlot, eRemoveReason);
		return TRUE;
	}

	eActionType = SAMALIB_CFLT_GetSapiActionTypeBySchedule(pstSchedule);

	HxLOG_Debug("pstContext->bSama_ScheduleLoaded: %d, pstContext->bSama_PvrDeviceMounted: %d, eActionType: %d, eRemoveReason: %d \n", pstContext->bSama_ScheduleLoaded, pstContext->bSama_PvrDeviceMounted, eActionType, eRemoveReason);
	if( (pstContext->bSama_ScheduleLoaded == TRUE)
		&& (pstContext->bSama_PvrDeviceMounted == FALSE)
		&& (eSAPI_ACTION_TYPE_Record == eActionType)
		&& (DxSchedule_RemovedReason_Expired == eRemoveReason || DxSchedule_RemovedReason_NoPvrDevice == eRemoveReason))
	{
		return TRUE;
	}

	return FALSE;
}
HERROR SAMA_SCHEDULE_TVA_UpdateRecordedProgramme (HUINT32 ulContentId, HBOOL bIsLoadAll)
{
	HCHAR		query[1024] = {0,};
	HUINT32 	id;

	HxLOG_Debug(" Update Recorded programme \n");

	if ( bIsLoadAll == TRUE )
	{
		HxLOG_Debug(" Load all \n");

		HxSTD_PrintToStrN(query, 1024
						, "SELECT * FROM %s " , DxNAMEOF(DxRecListData_t));
	}
	else
	{
		HxLOG_Debug(" Load one \n");

		HxSTD_PrintToStrN(query, 1024
						, "SELECT * FROM %s WHERE uid = %d" , DxNAMEOF(DxRecListData_t),ulContentId);
	}

	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n",query);
	HxLOG_Debug("==============================================================================\n");
	id = DAPI_Query(query, (DAPI_Getter)sama_sch_tva_OnDamaRecordedProgReceived, NULL);
	HxLOG_Debug("DAPI_Query() id: %u \n", id);

	return ERR_OK;
}

HERROR SAMA_SCHEDULE_TVA_TrackEitPf (HUID* uidlist, HUINT32 size)
{
	HINT32		c;
	HUID		evtuid;
	HCHAR		*query;
	HBOOL		bIsFirst = TRUE;
	HINT32 		nStream =0;
	HUINT32		id;

	if ( size == 0 || uidlist == NULL)
	{
		return ERR_FAIL;
	}

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 1024, SAMA_Malloc_CB, SAMA_Free_CB);

	if (nStream ==0 )
		return ERR_FAIL;

	HLIB_RWSTREAM_Print(nStream,"SELECT * FROM %s WHERE" , DxNAMEOF(eDxEPG_TYPE_PF));

	for (c = (HINT32)(size / sizeof(HUID)) ; c-- ; uidlist++)
	{
		HBOOL		bIsTracking = FALSE;
		HINT32		svcUid;

		evtuid = *uidlist;
		svcUid = (HINT32)(evtuid >> 32);

		bIsTracking = SAMALIB_TRACKING_IsTrackingBySvcUid(svcUid);
		if ( bIsTracking == FALSE )
		{
			continue;
		}

		if ( bIsFirst)
		{
			HLIB_RWSTREAM_Print(nStream," uid=%lld",evtuid);
			bIsFirst = FALSE;
		}
		else
		{
			HLIB_RWSTREAM_Print(nStream," OR uid=%lld",evtuid);
		}
	}

	query = HLIB_RWSTREAM_CloseWithoutBuf(nStream);

	// if there is a tracking event
	if ( bIsFirst == FALSE )
	{
		HxLOG_Debug("query: \t%s\n",query);
		id = DAPI_Query(query, (DAPI_Getter)sama_sch_tva_OnDamaTrackingEventReceived, NULL);

	}

	if (query)
		HLIB_RWSTREAM_MemFree(query);

	return ERR_OK;
}

HERROR SAMA_SCHEDULE_TVA_UpdateEitPf (HUID* uidlist, HUINT32 size)
{
	SAMA_Context_t			*pstContext = SAMA_GetContext();
	HINT32		c;
	HUID		evtuid;
	HCHAR		*query;
	HBOOL		bIsFirst = TRUE;
	HINT32 		nStream =0;
	HUINT32		ulDummySlot;
	HERROR		hErr;
	HUINT32		id;

	if ( size == 0 )
	{
		return ERR_FAIL;
	}

	if (pstContext->bSama_TvaInited != TRUE)
	{
		HxLOG_Error("Tva not inited \n");
		return ERR_FAIL;
	}

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 1024, SAMA_Malloc_CB, SAMA_Free_CB);

	if (nStream ==0 )
		return ERR_FAIL;

	HLIB_RWSTREAM_Print(nStream,"SELECT * FROM %s WHERE" , DxNAMEOF(eDxEPG_TYPE_PF));

	for (c = (HINT32)(size / sizeof(HUID)) ; c-- ; uidlist++)
	{
		HINT32		eventId = 0;
		HINT32		svcUid;

		evtuid = *uidlist;

#if defined CONFIG_FUNC_EPG_USE_SVCUID
		eventId = DxEVENT_UID_EVENTID(evtuid);
#endif

		svcUid = (HINT32)(evtuid >> 32);

		hErr = sama_sch_tva_GetScheduledSlot(svcUid, eventId, &ulDummySlot);
		if ( hErr != ERR_OK )
		{
			continue;
		}

		if ( bIsFirst)
		{
			HLIB_RWSTREAM_Print(nStream," uid=%lld",evtuid);
			bIsFirst = FALSE;
		}
		else
		{
			HLIB_RWSTREAM_Print(nStream," OR uid=%lld",evtuid);
		}
	}

	query = HLIB_RWSTREAM_CloseWithoutBuf(nStream);

	// if there is a tracking event
	if ( bIsFirst == FALSE )
	{
		HxLOG_Debug("query:\t%s\n",query);
		id = DAPI_Query(query, (DAPI_Getter)sama_sch_tva_OnDamaChangedEventsReceived, NULL);
	}

	if (query)
		HLIB_RWSTREAM_MemFree(query);

	return ERR_OK;
}

HERROR SAMA_SCHEDULE_TVA_UpdateEitSc (HUID* uidlist, HUINT32 size)
{
	SAMA_Context_t			*pstContext = SAMA_GetContext();
	HINT32		c;
	HUID		evtuid;
	HCHAR		*query;
	HBOOL		bIsFirst = TRUE;
	HINT32 		nStream =0;
	HUINT32		ulDummySlot;
	HERROR		hErr;
	HUINT32		id;

	if ( size == 0 )
	{
		return ERR_FAIL;
	}

	if (pstContext->bSama_TvaInited != TRUE)
	{
		HxLOG_Error("Tva not inited \n");
		return ERR_FAIL;
	}

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 1024, SAMA_Malloc_CB, SAMA_Free_CB);

	if (nStream ==0 )
		return ERR_FAIL;

	HLIB_RWSTREAM_Print(nStream,"SELECT * FROM %s WHERE" , DxNAMEOF(eDxEPG_TYPE_SC));

	for (c = (HINT32)(size / sizeof(HUID)) ; c-- ; uidlist++)
	{
		HINT32		eventId = 0;
		HINT32		svcUid;

		evtuid = *uidlist;

#if defined CONFIG_FUNC_EPG_USE_SVCUID
		eventId = DxEVENT_UID_EVENTID(evtuid);
#endif

		svcUid = (HINT32)(evtuid >> 32);


		hErr = sama_sch_tva_GetScheduledSlot(svcUid, eventId, &ulDummySlot);
		if ( hErr != ERR_OK )
		{
			continue;
		}

		if ( bIsFirst)
		{
			HLIB_RWSTREAM_Print(nStream," uid=%lld",evtuid);
			bIsFirst = FALSE;
		}
		else
		{
			HLIB_RWSTREAM_Print(nStream," OR uid=%lld",evtuid);
		}

	}

	query = HLIB_RWSTREAM_CloseWithoutBuf(nStream);

	// if there is a tracking event
	if ( bIsFirst == FALSE )
	{
		HxLOG_Debug("query:\t%s\n",query);
		id = DAPI_Query(query, (DAPI_Getter)sama_sch_tva_OnDamaChangedEventsReceived, NULL);
	}

	if (query)
		HLIB_RWSTREAM_MemFree(query);
	return ERR_OK;
}


HERROR	SAMA_SCHEDULE_TVA_ResumeSchedule (HUINT32 ulSlot)
{
	HBOOL			isTracking = FALSE;
	DxSchedule_t	*pstTrackingSchedule = NULL;

	isTracking = SAMALIB_TRACKING_IsTrackingBySlotId(ulSlot);
	if (isTracking != TRUE)
	{
		return ERR_OK;
	}

	pstTrackingSchedule = SAMA_SCHEDULE_GetSchedule(ulSlot);
	if (pstTrackingSchedule == NULL)
	{
		return ERR_FAIL;
	}

	if (pstTrackingSchedule->eStatus == DxRSVSTATUS_READY)
	{
		SAMA_PostMessage(SAMAMESSAGE_PROCESSTWOWAYTIMER, ulSlot, eSAPI_TIMER_TYPE_Start, 0, 0, 0, 0);
		return ERR_OK;
	}

	HxLOG_Error("Tracking Schedule is not ready. slot=(%d) \n",ulSlot);
	return ERR_FAIL;
}

