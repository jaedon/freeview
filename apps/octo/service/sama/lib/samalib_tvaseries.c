/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  sama_tvaseries.c
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
* MS949 encoding À» »ç¿ëÇÏ´Â source file ÀÔ´Ï´Ù.
* MS949 encodingªòÞÅéÄª·ªÆ source fileªÇª¹¡£
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



#include <sama_lib.h>
#include "./local_include/sama_lib_int.h"

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define EPGLIB_STARTTIME_BUFSIZE		(5)
#define EPGLIB_DURATION_BUFSIZE		(3)

#define IS_EVENT_VIEW(pEvent)	((pEvent)->endTime - (pEvent)->startTime == 1)

#define SAMA_TVA_MAX_SLOT		2048

#define MAX_CONFLICT_CHECK_LEVEL	2
#define MAX_CONFLICT_SLOT_NUM		30

#define KEYWORD_CRID_PROGRAM 		"pCRID:"
#define KEYWORD_CRID_SERIES 		"sCRID:"
#define KEYWORD_CRID_DLIMITER		","

#define SAMALIB_PRINT(bCmdPrint, ...) \
	if(bCmdPrint) {\
		if(s_pfCmdPrintFunc != NULL) {\
			s_pfCmdPrintFunc(__VA_ARGS__);\
		}\
	}\
	else {\
		HxLOG_Print(__VA_ARGS__);\
	}


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

typedef struct
{
	HUINT32 ulAltEventId;
	HUINT32 ulAltSvcUid;
	SERIES_SET *pstOrgSeriesSet;
} SERIES_ALTERNATIVE_MAP;


typedef SAPI_ConflictType_e (*ConflictChecker)(int *pNum_conflict, CONFLICT_LIST conflict_list[]);

//------------------------------------------------------------------------


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
STATIC HxList_t			*s_pstSama_SeriesList = NULL;	// SERIES_SET
STATIC sama_ConflictCheck_t *s_samaInst = NULL;
static	SERIES_ALTERNATIVE_MAP s_astAlternativeSeriesMap[SAMA_MAX_ALTERNATIVE_INSTANCE];
static	HUINT32				   s_AlternativeSeriesMapIndex = 0;

struct {
	int num_alloc_series_set;
	int num_alloc_prog_set;
	int num_alloc_inst_set;
	int num_alloc_event_period;
	int num_free_series_set;
	int num_free_prog_set;
	int num_free_inst_set;
	int num_free_event_period;
} tva_statistics;

#if defined(CONFIG_DEBUG)
STATIC HERROR (*s_pfCmdPrintFunc)(const HCHAR *format, ...) = HLIB_CMD_Printf;
#else
STATIC HERROR (*s_pfCmdPrintFunc)(const HCHAR *format, ...) = NULL;
#endif


/*******************************************************************/
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
#define ____DEBUG_LOCAL_FUNC____

STATIC HERROR samalib_tva_SetCmdPrintFunction(HERROR (*pfPrintFunc)(const HCHAR *format, ...))
{
	s_pfCmdPrintFunc = pfPrintFunc;
	return ERR_OK;
}
STATIC HCHAR samalib_tva_GetEventSourceChar(HUINT32 ulEventSource)
{
	if (ulEventSource & SAMA_EVENT_BY_EIT_RUNNING_STATUS)
		return 'R';
	if (ulEventSource & SAMA_EVENT_BY_EIT_PF)
		return 'P';
	if (ulEventSource & SAMA_EVENT_BY_EIT_SC)
		return 'S';
	if (ulEventSource & SAMA_EVENT_BY_IP)
		return 'I';

	return '-';
}

STATIC void samalib_tva_PrintEventPeriod(HBOOL bCmdPrint, EVENT_PERIOD *pstEventPeriod)
{
#if defined(CONFIG_DEBUG)
	HxDATETIME_t	stStartTime, stEndTime;
	UNIXTIME		utStartTime, utEndTime;

	utStartTime	= pstEventPeriod->startTime;
	utEndTime	= pstEventPeriod->endTime;

	HLIB_DATETIME_ConvertUnixTimeToDateTime(utStartTime,&stStartTime);
	HLIB_DATETIME_ConvertUnixTimeToDateTime(utEndTime,&stEndTime);

	SAMALIB_PRINT(bCmdPrint, "        %c%c%c%c%c%c (%2d)(E:%d)(S:%d)(%d/%d) - %d/%d/%d (%02d:%02d:%02d ~ %02d:%02d:%02d) EVENT [%.30s]\n",
			(samalib_tva_GetEventSourceChar(pstEventPeriod->flag)),
			((pstEventPeriod->flag & SAMA_EVENT_BEING_RECORDED) ? 'R' : '-'),
			((pstEventPeriod->flag & SAMA_EVENT_BEING_MONITORED) ? 'M' : '-'),
			((pstEventPeriod->flag & SAMA_EVENT_UPDATED) ? 'U' : '-'),
			((pstEventPeriod->flag & SAMA_EVENT_ADDED) ? 'A' : '-'),
			((pstEventPeriod->flag & SAMA_EVENT_SEASON_EPISODE_SETTED) ? 'S' : '-'),
			pstEventPeriod->ulSlot, pstEventPeriod->ulEventId, pstEventPeriod->ulSvcUid,
			pstEventPeriod->ulEpisode, pstEventPeriod->ulSeason,
			stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay,
			stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond,
			stEndTime.stTime.ucHour, stEndTime.stTime.ucMinute, stEndTime.stTime.ucSecond,
			pstEventPeriod->szEventName);
#endif
}

STATIC void samalib_tva_PrintInstanceSet(HBOOL bCmdPrint, INSTANCE_SET *pstInstSet)
{
#if defined(CONFIG_DEBUG)
	EVENT_PERIOD *pstEventPeriod;

	SAMALIB_PRINT(bCmdPrint, "    %c%c%c INSTANCE_SET [%s]\n",
			((pstInstSet->flag & SAMA_INSTANCE_DO_NOT_FIND_ALTERNATIVE) ? '!' : ' '),
			(pstInstSet == pstInstSet->pstProgSet->pstSelectedInst ? '+' : '-'),
			(pstInstSet == pstInstSet->pstProgSet->pstCandidateInst ? 'c' : '-'),
			(char *)&pstInstSet->imi);

	for (pstEventPeriod = pstInstSet->pstEventList; pstEventPeriod; pstEventPeriod = pstEventPeriod->pNext)
		samalib_tva_PrintEventPeriod(bCmdPrint, pstEventPeriod);
#endif
}

STATIC void samalib_tva_PrintProgramSet(HBOOL bCmdPrint, PROGRAM_SET *pstProgSet)
{
#if defined(CONFIG_DEBUG)
	INSTANCE_SET *pstInstSet;

	SAMALIB_PRINT(bCmdPrint, "    PROGRAM_SET (%d)[%s%s]\n",pstProgSet->stProgCrid.type, (char *)pstProgSet->stProgCrid.auth, (char *)pstProgSet->stProgCrid.data);

	for (pstInstSet = pstProgSet->pstInstList; pstInstSet; pstInstSet = pstInstSet->pNext)
	{
		samalib_tva_PrintInstanceSet(bCmdPrint, pstInstSet);
	}
#endif
}

STATIC void samalib_tva_PrintEventToRecord(HBOOL bCmdPrint, int i, EVENT_TO_RECORD *pEventToRecord)
{
#if defined(CONFIG_DEBUG)
	if (pEventToRecord->ulSvcUid)
	{
		HxDATETIME_t	stStartTime, stEndTime;
		UNIXTIME		utStartTime, utEndTime;

		utStartTime = pEventToRecord->startTime;
		utEndTime	= pEventToRecord->endTime;

		HLIB_DATETIME_ConvertDateTimeToUnixTime (&stStartTime, &utStartTime);
		HLIB_DATETIME_ConvertDateTimeToUnixTime (&stEndTime, &utEndTime);


		SAMALIB_PRINT(bCmdPrint, "    Event To Record[%d] [%s%s%s], %x, %d/%d/%d (%02d:%02d:%02d ~ %02d:%02d:%02d)\n", i,
				(char *)pEventToRecord->stProgCrid.auth,
				(char *)pEventToRecord->stProgCrid.data,
				(char *)&pEventToRecord->stProgCrid.imi,
				pEventToRecord->ulEventId,
				stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay,
				stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond,
				stEndTime.stTime.ucHour, stEndTime.stTime.ucMinute, stEndTime.stTime.ucSecond);
	}
#endif
}

STATIC void samalib_tva_PrintSeriesSet(HBOOL bCmdPrint, SERIES_SET *pstSeriesSet)
{
#if defined(CONFIG_DEBUG)
	PROGRAM_SET	*pstProgSet;
	INSTANCE_SET	*pstInstSet;
	EVENT_PERIOD	*pstEventPeriod;
	HINT32			num_prog = 0, num_inst = 0, num_event = 0, i;
	HINT32			num_recorded_crids = 0;
	TVA_CONTENTID_T	*pstRecordedCrid = NULL;



	SAMALIB_PRINT(bCmdPrint, "   samalib_tva_PrintSeriesSet ");

	if ( pstSeriesSet == NULL )
	{
		SAMALIB_PRINT(bCmdPrint, "[SERIES NULL]\n");
		return;
	}
	else
	{
		SAMALIB_PRINT(bCmdPrint, "[SERIES 0x%x] (rsv:%d)(hd:%d) \n",pstSeriesSet
			, (pstSeriesSet->eRsvType)
			, pstSeriesSet->eHdType);
	}

	if (pstSeriesSet->ulSeriesTrackingSlot != DxRSV_SLOT_NULL)
	{
		SAMALIB_PRINT(bCmdPrint, "[SERIES 0x%x tracking by %d]\n",pstSeriesSet, pstSeriesSet->ulSeriesTrackingSlot);
	}
	if (pstSeriesSet->stCrid.type == SAMA_CRID_TYPE_SERIES)
	{
		SAMALIB_PRINT(bCmdPrint, "[[SERIES CRID %s%s%s]]\n",
				(char *)pstSeriesSet->stCrid.auth,
				(char *)pstSeriesSet->stCrid.data,
				(char *)&pstSeriesSet->stCrid.imi);
	}
	else if (pstSeriesSet->stCrid.type == SAMA_CRID_TYPE_PROG)
	{
		SAMALIB_PRINT(bCmdPrint, "[[PROG CRID %s%s%s]]\n",
				(char *)pstSeriesSet->stCrid.auth,
				(char *)pstSeriesSet->stCrid.data,
				(char *)&pstSeriesSet->stCrid.imi);
	}
#if defined(CONFIG_OP_TDC)
	SAMALIB_PRINT(bCmdPrint, "[[SERIES TITLE %s]] (searchFlag:%d) (svc:%d)\n",
			(char *)pstSeriesSet->szSeriesTitle,pstSeriesSet->searchFlag, pstSeriesSet->ulSvcUid);
#endif


	for (pstProgSet = pstSeriesSet->pstProgList; pstProgSet; pstProgSet = pstProgSet->pNext)
	{
		num_prog++;
		samalib_tva_PrintProgramSet(bCmdPrint, pstProgSet);
		for (pstInstSet = pstProgSet->pstInstList; pstInstSet; pstInstSet = pstInstSet->pNext)
		{
			num_inst++;
			for (pstEventPeriod = pstInstSet->pstEventList; pstEventPeriod; pstEventPeriod = pstEventPeriod->pNext)
			{
				num_event++;
			}
		}
	}

	if (pstSeriesSet->pstRecordedProgCrids != NULL)
	{
		num_recorded_crids = HLIB_VECTOR_Length(pstSeriesSet->pstRecordedProgCrids);
	}

	for (i = 0; i < num_recorded_crids; i++)
	{
		pstRecordedCrid = HLIB_VECTOR_ItemAt(pstSeriesSet->pstRecordedProgCrids, i );
		if (pstRecordedCrid == NULL)
		{
			SAMALIB_PRINT(bCmdPrint, "Null crid. \n");
			continue;
		}

		if (pstRecordedCrid->type == SAMA_CRID_TYPE_PROG)
		{
			SAMALIB_PRINT(bCmdPrint, "    Recorded PROG CRID[%d] %s%s%s\n", i,
					(char *)pstRecordedCrid->auth,
					(char *)pstRecordedCrid->data,
					(char *)&pstRecordedCrid->imi);
		}
	}
	/*
	for (i = 0; i < DxRSV_NUM_CANCEL_PROG_SAVE; i++)
	{
		if (pstSeriesSet->stCanceledProgCrid[i].type == SAMA_CRID_TYPE_PROG)
		{
			SAMALIB_PRINT(bCmdPrint, "    Canceled PROG CRID[%d] %s%s%s\n", i,
					(char *)pstSeriesSet->stCanceledProgCrid[i].auth,
					(char *)pstSeriesSet->stCanceledProgCrid[i].data,
					(char *)&pstSeriesSet->stCanceledProgCrid[i].imi);
		}
	}
	*/
/*
	for (i = 0; i < DxRSV_NUM_TO_RECORD_PROG_SAVE; i++)
	{
		samalib_tva_PrintEventToRecord(bCmdPrint, i, &pstSeriesSet->stEventToRecord[i]);
	}
*/
	SAMALIB_PRINT(bCmdPrint, "Total program_set %d, instance_set %d, event %d\n",
			num_prog, num_inst, num_event);
#endif

}

STATIC void samalib_tva_PrintMemoryStatus(HBOOL bCmdPrint)
{
#if defined(CONFIG_DEBUG)
	register PROGRAM_SET *pstProgSet;
	register INSTANCE_SET *pstInstSet;
	register EVENT_PERIOD *pEventPeriod;
	int num_prog_set = 0, num_inst_set = 0, num_event_period = 0;
	int num_selected, num_candidate;
	int slot = 0;
	int len;

	HxList_t	*pvIter;

	for (pvIter = HLIB_LIST_First(s_pstSama_SeriesList); NULL != pvIter; pvIter = HLIB_LIST_Next(pvIter))
	{
		SERIES_SET		*pstSeriesSet = (SERIES_SET *)HLIB_LIST_Data(pvIter);

		if ( pstSeriesSet == NULL )
			continue;


		if (!MACRO_IS_TVA_SERIES_SET(pstSeriesSet))
		{
			if (pstSeriesSet->pstProgList != NULL)
			{
				SAMALIB_PRINT(bCmdPrint, "slot[%d]: Invalid Garbage in pstSeriesSet->pstProgList [%p]\n", slot, pstSeriesSet->pstProgList);
			}
			if (pstSeriesSet->stCrid.type != 0)
			{
				SAMALIB_PRINT(bCmdPrint, "slot[%d]: invalid pstSeriesSet->stCrid.type [%d]\n", slot, pstSeriesSet->stCrid.type);
			}
			if (strlen((char *)pstSeriesSet->stCrid.auth) != 0)
			{
				SAMALIB_PRINT(bCmdPrint, "slot[%d]: Garbage in pstSeriesSet->stCrid.auth [%s]\n", slot, (char *)pstSeriesSet->stCrid.auth);
			}
			if (strlen((char *)pstSeriesSet->stCrid.data) != 0)
			{
				SAMALIB_PRINT(bCmdPrint, "slot[%d]: Garbage in pstSeriesSet->stCrid.data [%s]\n", slot, (char *)pstSeriesSet->stCrid.data);
			}
			if (pstSeriesSet->stCrid.imi != 0)
			{
				SAMALIB_PRINT(bCmdPrint, "slot[%d]: Garbage in pstSeriesSet->stCrid.imi [%x]\n", slot, pstSeriesSet->stCrid.imi);
			}
			continue;
		}

		if (pstSeriesSet->stCrid.type != SAMA_CRID_TYPE_PROG && pstSeriesSet->stCrid.type != SAMA_CRID_TYPE_SERIES)
		{
			SAMALIB_PRINT(bCmdPrint, "slot[%d]: invalid pstSeriesSet->stCrid.type [%d]\n", slot, pstSeriesSet->stCrid.type);
		}

		len = strlen((char *)pstSeriesSet->stCrid.auth);
		if (len > DxRSV_CRID_AUTH_LEN)
		{
			SAMALIB_PRINT(bCmdPrint, "slot[%d]: Too long AUTHORITY in pstSeriesSet\n", slot);
		}

		len = strlen((char *)pstSeriesSet->stCrid.data);
		if (len > DxRSV_CRID_DATA_LEN)
		{
			SAMALIB_PRINT(bCmdPrint, "slot[%d]: Too long DATA in pstSeriesSet\n", slot);
		}
		else if (len == 0)
		{
			SAMALIB_PRINT(bCmdPrint, "slot[%d]: No DATA in pstSeriesSet\n", slot);
		}

		if (pstSeriesSet->stCrid.imi != 0)
		{
			SAMALIB_PRINT(bCmdPrint, "slot[%d]: Garbage in pstSeriesSet->stCrid.imi [%x]\n", slot, pstSeriesSet->stCrid.imi);
		}

		if (pstSeriesSet->pstProgList == NULL)
		{
			continue;
		}

		for (pstProgSet = pstSeriesSet->pstProgList; pstProgSet; pstProgSet = pstProgSet->pNext)
		{
			num_selected = num_candidate = 0;
			num_prog_set++;
			if (strlen((char *)pstProgSet->stProgCrid.auth) > DxRSV_CRID_AUTH_LEN)
			{
				SAMALIB_PRINT(bCmdPrint, "slot[%d]: Too long AUTHORITY\n", slot);
			}
			if (strlen((char *)pstProgSet->stProgCrid.data) > DxRSV_CRID_DATA_LEN)
			{
				SAMALIB_PRINT(bCmdPrint, "slot[%d]: Too long DATA\n", slot);
			}
			if (pstProgSet->stProgCrid.imi != 0)
			{
				SAMALIB_PRINT(bCmdPrint, "slot[%d]: IMI should be empty\n", slot);
			}
			if (pstProgSet->pstSeriesSet != pstSeriesSet)
			{
				SAMALIB_PRINT(bCmdPrint, "slot[%d]: Invalid pstProgSet->pstSeriesSet (%p)\n",
						slot, pstProgSet->pstSeriesSet);
			}
			if (pstProgSet->pstInstList == NULL)
			{
				SAMALIB_PRINT(bCmdPrint, "slot[%d]: pstProgSet->pstInstList is NULL\n", slot);
			}

			for (pstInstSet = pstProgSet->pstInstList; pstInstSet; pstInstSet = pstInstSet->pNext)
			{
				num_inst_set++;
				if (pstProgSet->pstSelectedInst == pstInstSet)
					num_selected++;
				if (pstProgSet->pstCandidateInst == pstInstSet)
					num_candidate++;
				if (pstInstSet->pstProgSet != pstProgSet)
				{
					SAMALIB_PRINT(bCmdPrint, "slot[%d]: Invalid pstInstSet->pstProgSet (%p)\n", slot, pstInstSet->pstProgSet);
				}
				if (pstInstSet->pstEventList == NULL)
				{
					SAMALIB_PRINT(bCmdPrint, "slot[%d]: pstInstSet->pstEventList is NULL\n", slot);
				}

				for (pEventPeriod = pstInstSet->pstEventList; pEventPeriod; pEventPeriod = pEventPeriod->pNext)
				{
					num_event_period++;

					if (pEventPeriod->pstInstSet != pstInstSet)
					{
						SAMALIB_PRINT(bCmdPrint, "slot[%d]: Invalid pEventPeriod->pstInstSet (%p)\n", slot, pEventPeriod->pstInstSet);
					}
				}
			}

			if (num_selected != 1)
			{
				SAMALIB_PRINT(bCmdPrint, "slot[%d]: Invalid pstProgSet->pstSelectedInst (%d, %p)\n", slot, num_selected, pstProgSet->pstSelectedInst);
			}
			if (num_candidate != 1)
			{
				SAMALIB_PRINT(bCmdPrint, "slot[%d]: Invalid pstProgSet->pstCandidateInst (%d, %p)\n", slot, num_candidate, pstProgSet->pstCandidateInst);
			}
		}
	}

	if (num_prog_set != (tva_statistics.num_alloc_prog_set - tva_statistics.num_free_prog_set))
	{
		SAMALIB_PRINT(bCmdPrint, "Invalid num_prog_set %d: should be %d\n", num_prog_set,
				tva_statistics.num_alloc_prog_set - tva_statistics.num_free_prog_set);
	}
	if (num_inst_set != (tva_statistics.num_alloc_inst_set - tva_statistics.num_free_inst_set))
	{
		SAMALIB_PRINT(bCmdPrint, "Invalid num_inst_set %d: should be %d\n", num_inst_set,
				tva_statistics.num_alloc_inst_set - tva_statistics.num_free_inst_set);
	}
	if (num_event_period != (tva_statistics.num_alloc_event_period - tva_statistics.num_free_event_period))
	{
		SAMALIB_PRINT(bCmdPrint, "Invalid num_event_period %d: should be %d\n", num_event_period,
				tva_statistics.num_alloc_event_period - tva_statistics.num_free_event_period);
	}

	SAMALIB_PRINT(bCmdPrint, "num_alloc_series_set = %d\n", tva_statistics.num_alloc_series_set);
	SAMALIB_PRINT(bCmdPrint, "num_free_series_set = %d\n", tva_statistics.num_free_series_set);
	SAMALIB_PRINT(bCmdPrint, "num_alloc_prog_set = %d\n", tva_statistics.num_alloc_prog_set);
	SAMALIB_PRINT(bCmdPrint, "num_free_prog_set = %d\n", tva_statistics.num_free_prog_set);
	SAMALIB_PRINT(bCmdPrint, "num_alloc_inst_set = %d\n", tva_statistics.num_alloc_inst_set);
	SAMALIB_PRINT(bCmdPrint, "num_free_inst_set = %d\n", tva_statistics.num_free_inst_set);
	SAMALIB_PRINT(bCmdPrint, "num_alloc_event_period = %d\n", tva_statistics.num_alloc_event_period);
	SAMALIB_PRINT(bCmdPrint, "num_free_event_period = %d\n", tva_statistics.num_free_event_period);

	SAMALIB_PRINT(bCmdPrint, "Total program_set %d, instance_set %d, event %d\n", num_prog_set, num_inst_set, num_event_period);
#endif
}
void samalib_tva_PrintAltInfo(TVA_ALTERNATIVE_INFO *pAltInfo)
{
#if defined(CONFIG_DEBUG)
	HINT32		i, j;

	HxLOG_Print("Num alternative: %d\n", pAltInfo->numAlternative);
	for (i = 0; i < (int)pAltInfo->numAlternative; i++)
	{
		HxLOG_Print(" * instance[%d]: slot[%d]\n", i, pAltInfo->instance[i].slot);

		if (pAltInfo->instance[i].numEvent)
		{
			for (j = 0; j < pAltInfo->instance[i].numEvent; j++)
			{
				HxLOG_Print("       %04x %04x\n", pAltInfo->instance[i].svcHandle[j], pAltInfo->instance[i].ulEventId[j]);
			}
		}
		else
		{
			/* print name from schedule of slot of [pAltInfo->instance[i].slot] */
		}

		if (pAltInfo->instance[i].numAltEvent)
		{
			HxLOG_Print("     -> ALT:\n");
			for (j = 0; j < pAltInfo->instance[i].numAltEvent; j++)
			{
				HxLOG_Print("       %04x %04x\n", pAltInfo->instance[i].altSvcHandle[j], pAltInfo->instance[i].ulAltEventId[j]);
			}
		}
	}

	HxLOG_Print("\n");
#endif
}

void samalib_tva_PrintReserveConflictInfo(SAPI_Conflict_t *pConfInfo)
{
#if defined(CONFIG_DEBUG)
	HUINT32 i;
	HxLOG_Print("ConflictType = %d\n", pConfInfo->eType);
	HxLOG_Print("ulNumOfRsv = %d\n", pConfInfo->ulNumConflict);
//	HxLOG_Print("ulTsIndex = %d\n", pConfInfo->usTsIndex);
//	HxLOG_Print("ulEventId = %d\n", pConfInfo->stTriedSchedule.);
//	HxLOG_Print("ulSvcUid = 0x%x\n", pConfInfo->ulSvcUid);
	for (i = 0; i < pConfInfo->ulNumConflict; i++)
	{
		HxLOG_Print("    Conflict[%d] -- slot[%d]\n", i, pConfInfo->pstConflictArray[i].ulSlot);
//		HxLOG_Print("        ulTsIndex[%d]  = %d\n", i, pConfInfo->ausTsIndex[i]);
//		HxLOG_Print("        aulEventId[%d] = %d\n", i, pConfInfo->aulEventId[i]);
//		HxLOG_Print("        ulSvcUid[%d] = 0x%x\n", i, pConfInfo->aulSvcUid[i]);
	}
#endif
}


#define ____DEBUG_GLOBAL_FUNC____
void SAMALIB_TVA_PrintSeriesSet(SERIES_SET *pstSeriesSet)
{
#if defined(CONFIG_DEBUG)
	samalib_tva_PrintSeriesSet(FALSE, pstSeriesSet);
#endif
}
void SAMALIB_TVA_PrintProgramSet(PROGRAM_SET *pstProgSet)
{
#if defined(CONFIG_DEBUG)
	samalib_tva_PrintProgramSet(FALSE, pstProgSet);
#endif
}
void SAMALIB_TVA_PrintInstanceSet(INSTANCE_SET *pstInstSet)
{
#if defined(CONFIG_DEBUG)
	samalib_tva_PrintInstanceSet(FALSE, pstInstSet);
#endif
}
void SAMALIB_TVA_PrintAltInfo(TVA_ALTERNATIVE_INFO *pstAltInfo)
{
#if defined(CONFIG_DEBUG)
	samalib_tva_PrintAltInfo(pstAltInfo);
#endif
}
void SAMALIB_TVA_PrintAllSeriesSet(HERROR (*pfPrintFunc)(const HCHAR *format, ...))
{
#if defined(CONFIG_DEBUG)
	HxList_t	*pvIter;

	pfPrintFunc("===================== series tree start =============================\n");
	samalib_tva_SetCmdPrintFunction(pfPrintFunc);
	for (pvIter = HLIB_LIST_First(s_pstSama_SeriesList); NULL != pvIter; pvIter = HLIB_LIST_Next(pvIter))
	{
		SERIES_SET		*pstSeriesSet = (SERIES_SET *)HLIB_LIST_Data(pvIter);

		if ( 1 )// pstSeriesSet && MACRO_IS_TVA_SERIES_SET(pstSeriesSet))
		{
			pfPrintFunc("\n========================= \n");
			samalib_tva_PrintSeriesSet(TRUE, pstSeriesSet);
		}
	}
	pfPrintFunc("===================== series tree end =============================\n");
#endif
}

void SAMALIB_TVA_PrintMemoryStatus(void)
{
#if defined(CONFIG_DEBUG)
	samalib_tva_PrintMemoryStatus(TRUE);
#endif
}

#define ____LOCAL_FUNC____
STATIC HERROR samalib_tva_AdjustEvent(
											SERIES_SET * const pstSeriesSet,
											EVENT_PERIOD	*event,
											const UNIXTIME	startTime,
											const UNIXTIME	endTime,
											const HUINT32	ulSeason,
											const HUINT32	ulEpisode,
											const HUINT32	eventSource);
STATIC EVENT_PERIOD *samalib_tva_GetEventInSeriesSet(SERIES_SET *pstSeriesSet, HUINT32 ulSvcUid, HUINT32 ulEventId);
STATIC EVENT_PERIOD *samalib_tva_GetEventInScheduleList(HINT32 ulSvcUid, HUINT16 ulEventId);
STATIC HBOOL		samalib_tva_GetEventString(void *data, void *language);


STATIC void *alloc_series_set(void)
{
	tva_statistics.num_alloc_series_set++;
	return SAMA_Malloc(sizeof(SERIES_SET));
}

STATIC void free_series_set(void *series_set)
{
	SERIES_SET *pstSeriesSet = (SERIES_SET *)series_set;
	tva_statistics.num_free_series_set++;
	if (pstSeriesSet != NULL && pstSeriesSet->pstRecordedProgCrids != NULL)
	{
		HLIB_VECTOR_Delete(pstSeriesSet->pstRecordedProgCrids);
		pstSeriesSet->pstRecordedProgCrids = NULL;
	}

	SAMA_Free(series_set);
}

STATIC void *alloc_prog_set(void)
{
	tva_statistics.num_alloc_prog_set++;
	return SAMA_Malloc(sizeof(PROGRAM_SET));
}

STATIC void free_prog_set(void *prog_set)
{
	tva_statistics.num_free_prog_set++;
	SAMA_Free(prog_set);
}

STATIC void *alloc_inst_set(void)
{
	tva_statistics.num_alloc_inst_set++;
	return SAMA_Malloc(sizeof(PROGRAM_SET));
}

STATIC void free_inst_set(void *inst_set)
{
	tva_statistics.num_free_inst_set++;
	SAMA_Free(inst_set);
}

STATIC void *alloc_event_period(void)
{
	tva_statistics.num_alloc_event_period++;
	return SAMA_Malloc(sizeof(EVENT_PERIOD));
}

STATIC void free_event_period(void *event_period)
{
	tva_statistics.num_free_event_period++;
	SAMA_Free(event_period);
}

STATIC HINT32 crid_cmp(const HUINT32 *s1, const HUINT32 *s2)
{
	if ( s1 == NULL || s2 == NULL )
	{
		return 1;	// different crid
	}
	if (*s1++ != *s2++)
		return 1;

	while (*s1 != 0 && *s1 == *s2)
		s1++, s2++;

	if (*s1 == 0 && *s2 == 0)
		return 0;	// same crid

	return 1;	// different crid
}

STATIC void tva_memcpy(void *dst0, const void *src0, int num_byte)
{
	int *dst = (int *)dst0;
	const int *src = (const int *)src0;
	int len;

	len = (num_byte + 3) / 4;
	while (len--)
		*dst++ = *src++;
}

STATIC SERIES_SET *get_series_set(const TVA_CONTENTID_T *pCrid)
{
	HxList_t	*pvIter;

	for (pvIter = HLIB_LIST_First(s_pstSama_SeriesList); NULL != pvIter; pvIter = HLIB_LIST_Next(pvIter))
	{
		SERIES_SET		*pstSeriesSet = (SERIES_SET *)HLIB_LIST_Data(pvIter);

		if ( pCrid->type == pstSeriesSet->stCrid.type
			&& crid_cmp(pCrid->auth,pstSeriesSet->stCrid.auth)
			&& crid_cmp(pCrid->data,pstSeriesSet->stCrid.data))
		{
			return pstSeriesSet;
		}
	}

	return NULL;
}

STATIC SERIES_SET *new_series_set(const TVA_CONTENTID_T *pCrid)
{
	/* alloc new SERIES_SET and set it */
	SERIES_SET *pstSeriesSet = alloc_series_set();
	if (pstSeriesSet)
	{
		pstSeriesSet->pstProgList = NULL;
		pstSeriesSet->pstRecordedProgCrids = HLIB_VECTOR_NewEasy(0, SAMA_Free_CB, NULL);
		tva_memcpy(&(pstSeriesSet->stCrid), pCrid, sizeof(TVA_CONTENTID_T));
		pstSeriesSet->eHdType = eDxVIDEO_TYPE_ALL;
		pstSeriesSet->ulSeriesTrackingSlot = DxRSV_SLOT_NULL;
	}
	return pstSeriesSet;
}

STATIC PROGRAM_SET *get_program_set(SERIES_SET * const pstSeriesSet, const TVA_CONTENTID_T *pProgCrid)
{
	PROGRAM_SET *pstProgSet;

	for (pstProgSet = pstSeriesSet->pstProgList; pstProgSet; pstProgSet = pstProgSet->pNext)
	{
		if (crid_cmp(pstProgSet->stProgCrid.auth, pProgCrid->auth) == 0
			&& crid_cmp(pstProgSet->stProgCrid.data, pProgCrid->data) == 0)
		{
			return pstProgSet;
		}
	}
	return NULL;
}

STATIC PROGRAM_SET *new_program_set(const TVA_CONTENTID_T *pProgCrid)
{
	/* alloc new PROGRAM_SET and set it */
	PROGRAM_SET *pstProgSet = alloc_prog_set();
	if (pstProgSet)
	{
		pstProgSet->pstInstList = NULL;
		pstProgSet->pstSelectedInst = NULL;
		pstProgSet->pstCandidateInst = NULL;
		pstProgSet->flag = 0;
		tva_memcpy(&(pstProgSet->stProgCrid), pProgCrid, sizeof(pstProgSet->stProgCrid));
		pstProgSet->stProgCrid.imi = 0;
	}
	return pstProgSet;
}

STATIC INSTANCE_SET *get_instance_set(PROGRAM_SET * const pstProgSet, const int imi)
{
	INSTANCE_SET *pstInstSet;

	for (pstInstSet = pstProgSet->pstInstList; pstInstSet; pstInstSet = pstInstSet->pNext)
	{
		if (pstInstSet->imi != 0 && pstInstSet->imi == imi)
			return pstInstSet;
	}
	return NULL;
}

STATIC INSTANCE_SET *new_instance_set(PROGRAM_SET * const pstProgSet, const int imi)
{
	INSTANCE_SET *pstInstSet = alloc_inst_set();

	if (pstInstSet)
	{
		pstInstSet->pstEventList = NULL;
		pstInstSet->imi = imi;
		pstInstSet->flag = 0;
		pstInstSet->pstProgSet = pstProgSet;
		pstInstSet->pNext = NULL;
	}
	return pstInstSet;
}

STATIC EVENT_PERIOD *new_event_period(INSTANCE_SET * const pstInstSet, UNIXTIME startTime, UNIXTIME endTime, HUINT32 ulSvcUid, HUINT32 event_id, HUINT32 flag, HUINT32 ulSlot, const HCHAR *szEventName)
{
	EVENT_PERIOD *event = alloc_event_period();
	EVENT_PERIOD *tmp_event;

	if (event)
	{
		event->startTime = startTime;
		event->endTime = endTime;
		event->ulSvcUid = ulSvcUid;
		event->ulEventId = event_id;
		event->pstInstSet = pstInstSet;
		event->ulSeason = 0;
		event->ulEpisode = 0;
		event->flag = flag;
		event->ulSlot = ulSlot;
		HxSTD_snprintf(event->szEventName, DxRSV_EVENT_NAME_LEN, "%s", (szEventName == NULL)?"": szEventName);

		// Ç×»óÀÌ ¾Æ´Ï¶ó, Æ¯Á¤ slotÀÏ °æ¿ì¿¡¸¸ add flag¸¦ ºÙÀÌ´Â ÀÌÀ¯´Â, ½ÇÁ¦·Î SAMA_SCHEDULE_MakeSchedule ÇÒ¶§, ÀÌ slotÀÌ sama¿¡¼­ ¹ÞÀº °ÍÀÎÁö ±¸ºÐÇØ¾ß ÇÏ±â ¶§¹®ÀÌ´Ù.
		if ( (event->ulSlot == DxRSV_SLOT_NULL)
			 || (event->ulSlot == SAMA_SERIES_ALTERNATIVE_SLOT))
		{
			event->flag |= SAMA_EVENT_ADDED;
		}

		if (pstInstSet->pstEventList == NULL)
		{
			event->pNext = NULL;
			pstInstSet->pstEventList = event;
			return event;
		}

		if (event->startTime < pstInstSet->pstEventList->startTime)
		{
			/* prepend */
			event->pNext = pstInstSet->pstEventList;
			pstInstSet->pstEventList = event;
			return event;
		}

		tmp_event = pstInstSet->pstEventList;
		while (tmp_event->pNext)
		{
			if (tmp_event->startTime <= event->startTime && event->startTime <= tmp_event->pNext->startTime)
			{
				/* insert */
				event->pNext = tmp_event->pNext;
				tmp_event->pNext = event;
				return event;
			}
			tmp_event = tmp_event->pNext;
		}
		/* append */
		event->pNext = NULL;
		tmp_event->pNext = event;
	}
	return event;
}

int get_start_time_diff(INSTANCE_SET *pstInstSet0, INSTANCE_SET *pstInstSet)
{
	int time_diff;

	time_diff = pstInstSet0->pstEventList->startTime - pstInstSet->pstEventList->startTime;
	if (time_diff < 0)
		time_diff = -time_diff;

	return time_diff;
}

void add_instance_set(PROGRAM_SET *pstProgSet, INSTANCE_SET *pstInstSet)
{
	INSTANCE_SET *tmp_inst;
	EVENT_PERIOD *tmp_evtperiod;
	int time_diff_new, time_diff_prev, time_diff_next;
	EVENT_PERIOD *pToBeRemoved;

	if (pstProgSet->pstInstList == NULL)
	{
		pstProgSet->pstInstList = pstInstSet;
		return;
	}

	/* ÀÌ¹Ì ReservationÀ» µî·ÏµÇ¾î ÀÖ´Â EventÀÇ Event ID°¡ º¯°æµÇ¾î µé¾î¿À´Â °æ¿ì¸¦ À§ÇÑ Ã³¸®
	- µ¿ÀÏ Service¿¡¼­ µ¿ÀÏ ½Ã°£À» °®´Â Event´Â 1°³¸¸ Á¸ÀçÇÔ. µû¶ó¼­ ÃÖ½Å Á¤º¸¸¦ À¯ÁöÇØ ÁÖ¾î¾ß ÇÔ */
	tmp_inst = pstProgSet->pstInstList;
	while (tmp_inst)
	{
		tmp_evtperiod = tmp_inst->pstEventList;
		while(tmp_evtperiod)
		{
			if((tmp_evtperiod->flag & SAMA_EVENT_BEING_RECORDED) == 0
				&& (tmp_evtperiod->flag & SAMA_EVENT_BEING_MONITORED) == 0
				&& tmp_evtperiod->startTime == pstInstSet->pstEventList->startTime
				&& tmp_evtperiod->endTime == pstInstSet->pstEventList->endTime
				&& tmp_evtperiod->ulSvcUid == pstInstSet->pstEventList->ulSvcUid
				&& tmp_evtperiod->ulEventId != pstInstSet->pstEventList->ulEventId)
			{
				tmp_evtperiod->ulEventId = pstInstSet->pstEventList->ulEventId;

				while(pstInstSet->pstEventList)
				{
					pToBeRemoved = pstInstSet->pstEventList;
					pstInstSet->pstEventList = pToBeRemoved->pNext;
					free_event_period(pToBeRemoved);
				}
				free_inst_set(pstInstSet);
				return;
			}
			tmp_evtperiod = tmp_evtperiod->pNext;
		}
		tmp_inst = tmp_inst->pNext;
	}

	if (pstProgSet->pstInstList->pNext == NULL)
	{
		/* append */
		pstProgSet->pstInstList->pNext = pstInstSet;
		return;
	}
	time_diff_new = get_start_time_diff(pstProgSet->pstInstList, pstInstSet);

	tmp_inst = pstProgSet->pstInstList;
	while (tmp_inst->pNext)
	{
		time_diff_prev = get_start_time_diff(pstProgSet->pstInstList, tmp_inst);
		time_diff_next = get_start_time_diff(pstProgSet->pstInstList, tmp_inst->pNext);
		if (time_diff_prev <= time_diff_new && time_diff_new <= time_diff_next)
		{
			/* insert */
			pstInstSet->pNext = tmp_inst->pNext;
			tmp_inst->pNext = pstInstSet;
			return;
		}
		tmp_inst = tmp_inst->pNext;
	}

	/* append */
	tmp_inst->pNext = pstInstSet;
}

STATIC HUINT32	samalib_tva_GetEventSource (HUINT32 ulEventFlag)
{
	// event source priority
	// transition > PF > SC > IP

	if (ulEventFlag & SAMA_EVENT_BY_EIT_RUNNING_STATUS)
		return SAMA_EVENT_BY_EIT_RUNNING_STATUS;

	if (ulEventFlag & SAMA_EVENT_BY_EIT_PF)
		return SAMA_EVENT_BY_EIT_PF;

	if (ulEventFlag & SAMA_EVENT_BY_EIT_SC)
		return SAMA_EVENT_BY_EIT_SC;

	if (ulEventFlag & SAMA_EVENT_BY_IP)
		return SAMA_EVENT_BY_IP;

	HxLOG_Error("Fail to get event source. event flag=(0x%x) \n",ulEventFlag);

	return SAMA_EVENT_BY_IP;
}

STATIC HBOOL	samalib_tva_IsMorePrioritizedEventSource (HUINT32 ulOrgEventSource, HUINT32 ulNewEventSource)
{
	if (ulNewEventSource >= ulOrgEventSource)
		return TRUE;

	return FALSE;
}

/**************************************************
* ÁÖÀÇ dapi thread¿¡¼­´Â »ç¿ëÇÏÁö ¾Ê´Â´Ù.
***************************************************/
STATIC HERROR	samalib_tva_GetDefaultAuthority (HUINT32 ulSvcUid, HCHAR *szDefaultAuthority)
{
#if defined(CONFIG_OP_PVR_BASED_ON_TVANYTIME)
	DxService_t		stService;
	DxTransponder_t	stTransponder;
	DxNetwork_t		stNetwork;
	HERROR			hErr;
	HUINT32			ulTsUid;
	HUINT32			ulNetUid;
	HCHAR			*pdefaultAuth;

	hErr = samalib_svc_GetService(s_samaInst, ulSvcUid, &stService);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("samalib_svc_GetService failed. inst=(0x%x), ulSvcUid=(%d)\n",s_samaInst, ulSvcUid);
		return ERR_FAIL;
	}
	pdefaultAuth = stService.ex.tva.szDefaultAuth;

	if (!HxSTD_StrEmpty(pdefaultAuth))
	{
		HxSTD_StrNCpy((char *)szDefaultAuthority, pdefaultAuth, DxDEFAULT_AUTHORITY_LEN);
		return ERR_OK;
	}

	ulTsUid = stService.tsuid;
	HxLOG_Debug("\033[30;47m TS def auth. tsuid=(%d) \033[0m \n", ulTsUid);
	hErr = samalib_svc_GetTransponder(s_samaInst, ulTsUid, &stTransponder);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("samalib_svc_GetTransponder failed. inst=(0x%x), ulTsUid=(%d)\n",s_samaInst, ulTsUid);
		return ERR_FAIL;
	}

	pdefaultAuth = stTransponder.ex.tva.defaultAuth;
	if (!HxSTD_StrEmpty(pdefaultAuth))
	{
		HxSTD_StrNCpy((char *)szDefaultAuthority, pdefaultAuth, DxDEFAULT_AUTHORITY_LEN);
		return ERR_OK;
	}

	ulNetUid = stTransponder.netuid;
	HxLOG_Debug("\033[30;47m Net def auth. ulNetUid=(%d) \033[0m \n", ulNetUid);
	hErr = samalib_svc_GetNetwork(s_samaInst, ulNetUid, &stNetwork);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("samalib_svc_GetNetwork failed. inst=(0x%x), ulNetUid=(%d)\n",s_samaInst, ulNetUid);
		return ERR_FAIL;
	}

	pdefaultAuth = stNetwork.ex.tva.defaultAuth;
	if (!HxSTD_StrEmpty(pdefaultAuth))
	{
		HxSTD_StrNCpy((char *)szDefaultAuthority, pdefaultAuth, DxDEFAULT_AUTHORITY_LEN);
		return ERR_OK;
	}

#endif

	HxLOG_Warning("Fail to find default authority. svcuid=(%d) \n",ulSvcUid);

	return ERR_FAIL;
}
STATIC HBOOL	samalib_tva_IsImiExists (HUINT8 *szProgCRID)
{
	if (szProgCRID == NULL)
	{
		return FALSE;
	}

	if (HLIB_STD_StrCountChar(szProgCRID,'#') != 0)
	{
		return TRUE;
	}

	return FALSE;
}

// dapi callback¿¡¼­ default auth¸¦ ¾òÀ» ¼ö ÀÖ´ÂÁö ¹Ì¸® Ã¼Å©ÇÏ±â À§ÇÑ ¿ëµµ·Î »ç¿ëÇÑ´Ù.
STATIC HERROR	samalib_tva_FindCachedService (HUINT32 ulSvcUid)
{
	HBOOL					 bFoundCache = FALSE;
	HxList_t				*pstListItem;
	DxService_t				*pstSrcSvc;
	sama_ServiceCache_t		*pstCache = samalib_svc_GetCache(s_samaInst);

	if (pstCache == NULL)
	{
		HxLOG_Error("sama inst not inited. \n");
		return ERR_FAIL;
	}

	HxSEMT_Get(pstCache->ulSemId);

	for (pstListItem = pstCache->pstSvcList; NULL != pstListItem; pstListItem = pstListItem->next)
	{
		pstSrcSvc = (DxService_t *)HLIB_LIST_Data(pstListItem);
		if ((NULL != pstSrcSvc) && (ulSvcUid == pstSrcSvc->uid))
		{
			bFoundCache = TRUE;
			break;
		}
	}

	HxSEMT_Release(pstCache->ulSemId);

	if (TRUE == bFoundCache)
	{
		return ERR_OK;
	}

	HxLOG_Warning("Fail to find cached service. svcuid=(%d) \n",ulSvcUid);
	return ERR_FAIL;
}

STATIC HUINT8 	samalib_tva_MakeUpperCase(HUINT8 ch)
{
	if ( isalpha(ch) ) {
		if ( ch >= 'a' && ch <= 'z' ) {
			ch += ('A' - 'a') ;
		}
	}
	return ch ;
}

STATIC void		samalib_tva_ConvertStringUpper(HUINT8 *string)
{
	HUINT32	strLen, i ;

	strLen =  HxSTD_StrLen(string);
	for ( i = 0 ; i < strLen ; i++ ) {
		string[i] = samalib_tva_MakeUpperCase(string[i]) ;
	}
}

STATIC void samalib_tva_ReleaseProgramSet(PROGRAM_SET *pstProgSet)
{
	INSTANCE_SET *pstInstSet;
	EVENT_PERIOD *pstEventPeriod;
	void *pToBeRemoved;

	for (pstInstSet = pstProgSet->pstInstList; pstInstSet; )
	{
		for (pstEventPeriod = pstInstSet->pstEventList; pstEventPeriod; )
		{
			pToBeRemoved = pstEventPeriod;
			pstEventPeriod = pstEventPeriod->pNext;
			free_event_period(pToBeRemoved);
		}
		pToBeRemoved = pstInstSet;
		pstInstSet = pstInstSet->pNext;
		free_inst_set(pToBeRemoved);
	}

	free_prog_set(pstProgSet);
}

STATIC HERROR samalib_tva_ReleaseSeriesSet(SERIES_SET *pstSeriesSet)
{
	PROGRAM_SET *pstProgSet, *pToBeRemoved;

	if(pstSeriesSet == NULL)
		return ERR_FAIL;

	for (pstProgSet = pstSeriesSet->pstProgList; pstProgSet; )
	{
		pToBeRemoved = pstProgSet;
		pstProgSet = pstProgSet->pNext;
		samalib_tva_ReleaseProgramSet(pToBeRemoved);
	}

	free_series_set(pstSeriesSet);

	return ERR_OK;
}

STATIC HBOOL samalib_tva_cbCmpCRIDKey(void *user_data, void *list_data)
{
	TVA_CONTENTID_T		*userCrid = (TVA_CONTENTID_T *)user_data;
	SERIES_SET			*listSeriesSet = (SERIES_SET *)list_data;
	TVA_CONTENTID_T		*listCrid;

	if ( userCrid == NULL
		|| listSeriesSet == NULL )
	{
		return FALSE;	// different
	}

	listCrid = &(listSeriesSet->stCrid);

	if (userCrid->type == 0
		|| listCrid->type == 0 )
	{
		return FALSE;
	}

	if ( userCrid->type == listCrid->type
		&& crid_cmp(userCrid->auth, listCrid->auth) == 0
		&& crid_cmp(userCrid->data, listCrid->data) == 0 )
	{
		return TRUE; // same crid;
	}

	return FALSE;
}

STATIC void samalib_tva_cbDestroySeriesSet(void *data)
{
	HERROR		hErr;

	SERIES_SET *pstSeriesSet = (SERIES_SET *)data;

	// 2. ½ÇÁ¦ memory ÀÛ¾÷
	hErr = samalib_tva_ReleaseSeriesSet(pstSeriesSet);
	if ( hErr != ERR_OK )
	{
		HxLOG_Error("series set deleted failed. series set = (0x%x)",pstSeriesSet);
		return ;
	}

}

STATIC HERROR samalib_tva_RemoveEventPeriod(INSTANCE_SET *pstInstSet, EVENT_PERIOD *pToBeRemoved)
{
	if (pstInstSet->pstEventList == pToBeRemoved)
	{
		pstInstSet->pstEventList = pToBeRemoved->pNext;
	}
	else
	{
		EVENT_PERIOD *pTmpEventPeriod = pstInstSet->pstEventList;
		while (pTmpEventPeriod && pTmpEventPeriod->pNext != pToBeRemoved)
			pTmpEventPeriod = pTmpEventPeriod->pNext;

		if (pTmpEventPeriod == NULL)
			return ERR_FAIL;

		pTmpEventPeriod->pNext = pToBeRemoved->pNext;
	}

	free_event_period(pToBeRemoved);

	return ERR_OK;
}

STATIC void samalib_tva_RemoveInstSet(PROGRAM_SET *pProgSet, INSTANCE_SET *pToBeRemoved)
{
	EVENT_PERIOD *pEvent, *pTmpEvent;

	if (pProgSet->pstInstList == pToBeRemoved)
	{
		pProgSet->pstInstList = pToBeRemoved->pNext;
	}
	else
	{
		INSTANCE_SET *pTmp = pProgSet->pstInstList;
		while (pTmp && pTmp->pNext != pToBeRemoved)
			pTmp = pTmp->pNext;

		if (pTmp == NULL)
			return;

		pTmp->pNext = pToBeRemoved->pNext;
	}

	for (pEvent = pToBeRemoved->pstEventList; pEvent; )
	{
		pTmpEvent = pEvent;
		pEvent = pEvent->pNext;
		free_event_period(pTmpEvent);
	}
	free_inst_set(pToBeRemoved);
	return;
}

STATIC void samalib_tva_RemoveProgSet(SERIES_SET *pstSeriesSet, PROGRAM_SET *pToBeRemoved)
{
	if (pstSeriesSet->pstProgList == pToBeRemoved)
	{
		pstSeriesSet->pstProgList = pToBeRemoved->pNext;
	}
	else
	{
		PROGRAM_SET *pTmp = pstSeriesSet->pstProgList;
		while (pTmp && pTmp->pNext != pToBeRemoved)
		{
			pTmp = pTmp->pNext;
		}

		if (pTmp == NULL)
		{
			return;
		}
		pTmp->pNext = pToBeRemoved->pNext;
	}

	samalib_tva_ReleaseProgramSet(pToBeRemoved);
}

HERROR samalib_tva_RemoveNonSelectedInstance(SERIES_SET *pstSeriesSet)
{
	PROGRAM_SET *cur_prog;
	INSTANCE_SET *pstInstSet, *pToBeRemoved;

	for (cur_prog = pstSeriesSet->pstProgList; cur_prog; cur_prog = cur_prog->pNext)
	{
		/* remove non-selected instances */
		for (pstInstSet = cur_prog->pstInstList; pstInstSet; )
		{
			pToBeRemoved = pstInstSet;
			pstInstSet = pstInstSet->pNext;
			if (pToBeRemoved != cur_prog->pstSelectedInst)
			{
				samalib_tva_RemoveInstSet(cur_prog, pToBeRemoved);
			}
		}
	}

	return ERR_OK;
}
STATIC void samalib_tva_SortSeriesSet(SERIES_SET *pstSeriesSet)
{
	PROGRAM_SET *prog1;
	PROGRAM_SET *prev1;
	PROGRAM_SET *prog2;
	PROGRAM_SET *prev2;
	PROGRAM_SET *tmp;

	prev1 = NULL;
	prog1 = pstSeriesSet->pstProgList;
	while (prog1 && prog1->pNext) {
		prev2 = prog1;
		prog2 = prog1->pNext;
		while (prog2) {
			if (prog1->pstSelectedInst->pstEventList->startTime > prog2->pstSelectedInst->pstEventList->startTime) {
				/* swap */
				if (prev1 == NULL)
					pstSeriesSet->pstProgList = prog2;
				else
					prev1->pNext = prog2;

				tmp = prog2->pNext;
				if (prog1->pNext == prog2)
					prog2->pNext = prog1;
				else
					prog2->pNext = prog1->pNext;
				prog1->pNext = tmp;

				if (prev2 != prog1)
					prev2->pNext = prog1;

				tmp = prog1;
				prog1 = prog2;
				prog2 = tmp;
			}
			prev2 = prog2;
			prog2 = prog2->pNext;
		}
		prev1 = prog1;
		prog1 = prog1->pNext;
	}
}

STATIC HUINT8 *	samalib_tva_ConvertCridToStr(TVA_CONTENTID_T *pstCrid)
{
	static char pszReturnStr[DxRSV_CRID_LEN + 2]; /* CRID type(1byte) + MAX CRID + '\0' */

	SAMA_MemSet(pszReturnStr, 0, sizeof(HUINT8) * (DxRSV_CRID_LEN + 2));

	if(pstCrid != NULL)
	{
		pszReturnStr[0] = pstCrid->type; /* 0x31, 0x32, 0x33 -> Ascii '1', '2', '3' */
		strncat(pszReturnStr, (char *)pstCrid->auth, (DxRSV_CRID_LEN + 2) - strlen(pszReturnStr));
		strncat(pszReturnStr, (char *)pstCrid->data, (DxRSV_CRID_LEN + 2) - strlen(pszReturnStr));
		if(pstCrid->imi != 0)
		{
			strncat(pszReturnStr, (char *)&pstCrid->imi, (DxRSV_CRID_LEN + 2) - strlen(pszReturnStr));
		}
		pszReturnStr[DxRSV_CRID_LEN + 1] = '\0';
	}

	return (HUINT8 *)pszReturnStr;
}

STATIC HERROR samalib_tva_ExtractEachPartCRID(HUINT8 *cridstr, HUINT32 cridLen, TVA_CONTENTID_T *tva_crid, int *auth)
{
	HUINT32 i;
	HUINT8 *p, *p_crid = cridstr;

	if ( cridstr == NULL )
	{
		HxLOG_Error("null crid string");
		return ERR_FAIL;
	}

	tva_crid->imi = 0;
	for (i = 0; i < (int)(sizeof(tva_crid->auth) / sizeof(tva_crid->auth[0])); i++)
		tva_crid->auth[i] = 0;
	for (i = 0; i < (int)(sizeof(tva_crid->data) / sizeof(tva_crid->data[0])); i++)
		tva_crid->data[i] = 0;

	if (cridstr[0] == '/')
	{
		/* Set Default Authority */
		if(auth)
			tva_memcpy(tva_crid->auth, auth, sizeof(tva_crid->auth));

		p = (HUINT8 *)tva_crid->data;
	}
	else if ( HLIB_STD_StrNCaseCmp(p_crid, "crid://", 7) == 0 ) // 7 = strlen("crid://")
	{
		p_crid += 7;
		cridLen-=7;

		p = (HUINT8*)tva_crid->auth;
	}

	else
	{
		p = (HUINT8 *)tva_crid->auth;
	}

	for (i = 0; i < cridLen; i++)
	{
		if (*p_crid == '#')
		{
			*p = '\0';
			p = (HUINT8 *)&tva_crid->imi;
			switch(cridLen - i)
			{
				case 3:
					p[2] = p_crid[2];
				case 2:
					p[1] = p_crid[1];
				case 1:
					p[0] = p_crid[0];
					break;

				default:
					p[0] = p_crid[0];
					break;
			}

			if (cridLen - i >= 3)
				p[3] = '\0';
			else
				p[cridLen - i] = '\0';

			samalib_tva_ConvertStringUpper((HUINT8 *)tva_crid->auth);
			samalib_tva_ConvertStringUpper((HUINT8 *)tva_crid->data);
			samalib_tva_ConvertStringUpper((HUINT8 *)&tva_crid->imi);
			return ERR_OK;
		}
		else if (*p_crid == '/' && p < (HUINT8 *)tva_crid->data)
		{
			*p = '\0';
			p = (HUINT8 *)tva_crid->data;
		}
		*p++ = *p_crid++;
	}
	*p = '\0';

	samalib_tva_ConvertStringUpper((HUINT8 *)tva_crid->auth);
	samalib_tva_ConvertStringUpper((HUINT8 *)tva_crid->data);
	samalib_tva_ConvertStringUpper((HUINT8 *)&tva_crid->imi);

	return ERR_OK;
}

STATIC HERROR samalib_tva_MakeCrid (HCHAR *szCrid, HCHAR *szDefaultAuth, TVA_CONTENTID_T *pstCrid)
{
	HERROR			hErr = ERR_OK;

	if (szCrid == NULL
		|| pstCrid == NULL)
	{
		HxLOG_Error("NULL param. \n");
		return ERR_FAIL;
	}

	if (szDefaultAuth != NULL
		&& szDefaultAuth[0] != 0)
	{
		(void)samalib_tva_ExtractEachPartCRID(szCrid, HLIB_STD_StrNLen((char *)szCrid,DxRSV_CRID_LEN), pstCrid, (int*)szDefaultAuth );
	}
	else
	{
		(void)samalib_tva_ExtractEachPartCRID(szCrid, HLIB_STD_StrNLen((char *)szCrid,DxRSV_CRID_LEN), pstCrid, NULL );
	}
	if ( pstCrid->imi != 0 )
	{
		HxLOG_Debug("made crid: [%s][%s][%s] \n",pstCrid->auth,pstCrid->data,&pstCrid->imi);
	}
	else
	{
		HxLOG_Debug("made crid: [%s][%s] \n",pstCrid->auth,pstCrid->data);
	}

	return hErr;
}
STATIC HERROR samalib_tva_ExtractEachPartCRIDString(HUINT8 *cridstr, HUINT32 cridLen, HUINT8 *authStr, HUINT8 *dataStr, HUINT8 *imiStr)
{
	HUINT32 i;
	HUINT8 *p, *p_crid = cridstr;

	if (cridstr[0] == '/')
	{
		p = dataStr;
	}
	else
	{
		p = authStr;
	}

	for (i = 0; i < cridLen; i++)
	{
		if (*p_crid == '#')
		{
			*p = '\0';
			p = imiStr;
			switch(cridLen - i)
			{
				case 3:
					p[2] = p_crid[2];
				case 2:
					p[1] = p_crid[1];
				case 1:
					p[0] = p_crid[0];
					break;

				default:
					p[0] = p_crid[0];
					break;
			}

			if (cridLen - i >= 3)
				p[3] = '\0';
			else
				p[cridLen - i] = '\0';

			samalib_tva_ConvertStringUpper(authStr);
			samalib_tva_ConvertStringUpper(dataStr);
			samalib_tva_ConvertStringUpper(imiStr);
			return ERR_OK;
		}
		else if (*p_crid == '/' && p < dataStr)
		{
			*p = '\0';
			p = dataStr;
		}
		*p++ = *p_crid++;
	}
	*p = '\0';

	samalib_tva_ConvertStringUpper(authStr);
	samalib_tva_ConvertStringUpper(dataStr);
	samalib_tva_ConvertStringUpper(imiStr);

	return ERR_OK;
}
STATIC SERIES_SET * samalib_tva_NewSeriesSet(HUINT8 *szCrid, HUINT8 ucCridType, HCHAR *szDefaultAuth)
{
	SERIES_SET *pstSeriesSet = NULL;
	TVA_CONTENTID_T stCrid;

	SAMA_MemSet(&stCrid, 0x00, sizeof(TVA_CONTENTID_T));
	if ( szCrid != NULL
 		&& HLIB_STD_StrNLen((char *)szCrid,DxRSV_CRID_LEN) > 0)
	{
		samalib_tva_MakeCrid(szCrid,szDefaultAuth,&stCrid);
		stCrid.type = ucCridType;
	}

	HxLOG_Debug("new series set.\n");
	HxLOG_Debug("input crid=[%s], type=(%d) auth=[%s].\n",szCrid, ucCridType, szDefaultAuth);
	HxLOG_Debug("local crid=[%s].\n",samalib_tva_ConvertCridToStr(&stCrid));

	pstSeriesSet = new_series_set(&stCrid);
	if ( pstSeriesSet == NULL )
	{
		HxLOG_Error("series set malloc failed. type=(%d), crid=[%s]\n",ucCridType, szCrid);
		return NULL;
	}

	HxLOG_Debug("new series set. crid=[%s], cridtype=(%d)\n", szCrid, ucCridType);
	s_pstSama_SeriesList = HLIB_LIST_Append(s_pstSama_SeriesList, (void *)pstSeriesSet);

	return pstSeriesSet;
}

HERROR samalib_tva_DeleteSeriesSet(SERIES_SET *pstSeriesSet)
{
	HCHAR szCrid[DxRSV_CRID_LEN+1] = {0,};
	HINT32	i;

	if ( pstSeriesSet == NULL )
	{
		HxLOG_Error("null series set. \n");
		return ERR_FAIL;
	}

	snprintf(szCrid,DxRSV_CRID_LEN+1,"%s%s",(char*)pstSeriesSet->stCrid.auth,(char*)pstSeriesSet->stCrid.data);
	HxLOG_Debug("delete series set. crid=[%s], pstSeriesSet=(0x%x)\n",szCrid,pstSeriesSet);

	s_pstSama_SeriesList = HLIB_LIST_Remove(s_pstSama_SeriesList, pstSeriesSet);
	for (i=0;i<s_AlternativeSeriesMapIndex&&i<SAMA_MAX_ALTERNATIVE_INSTANCE;i++)
	{
		if (s_astAlternativeSeriesMap[i].pstOrgSeriesSet == pstSeriesSet)
		{
			HxLOG_Error("delete alternative series set. pstSeriesSet=(0x%x) \n",pstSeriesSet);
			s_astAlternativeSeriesMap[i].pstOrgSeriesSet = NULL;
		}
	}
	samalib_tva_ReleaseSeriesSet(pstSeriesSet);

	return ERR_OK;
}
EVENT_PERIOD * samalib_tva_GetEventPeriodBySlotId(HUINT32 ulSlot)
{
	HxList_t	*pvIter;
	SERIES_SET		*pstSeriesSet;
	PROGRAM_SET		*pstProgSet;
	INSTANCE_SET	*pstInstSet;
	EVENT_PERIOD	*pstEventPeriod;

	// SAMA_SERIES_ALTERNATIVE_SLOTÀÎ °æ¿ì¿¡´Â series setÀ» Ã£À» ¼ö ÀÖ¾î¾ßÇÏ¹Ç·Î ¿¹¿Ü¸¦ µÐ´Ù.
	if ( (ulSlot >= SAMA_TVA_MAX_SLOT)&&(ulSlot!=SAMA_SERIES_ALTERNATIVE_SLOT))
	{
		HxLOG_Debug("slot exceed the max number. slot=(%d)\n",ulSlot);
		return NULL;
	}

	for (pvIter = HLIB_LIST_First(s_pstSama_SeriesList); NULL != pvIter; pvIter = HLIB_LIST_Next(pvIter))
	{
		pstSeriesSet = (SERIES_SET *)HLIB_LIST_Data(pvIter);
		if ( pstSeriesSet == NULL )
		{
			continue;
		}
		for (pstProgSet = pstSeriesSet->pstProgList; pstProgSet != NULL; pstProgSet=pstProgSet->pNext)
		{
			for (pstInstSet = pstProgSet->pstSelectedInst; pstInstSet != NULL; pstInstSet = pstInstSet->pNext)
			{
				for (pstEventPeriod = pstInstSet->pstEventList; pstEventPeriod != NULL; pstEventPeriod = pstEventPeriod->pNext)
				{
					if (pstEventPeriod->ulSlot == ulSlot)
					{
						return pstEventPeriod;
					}
				}
			}
		}
	}

	return NULL;
}

SERIES_SET * samalib_tva_GetSeriesSetBySlotId(HUINT32 ulSlot)
{
	HxList_t	*pvIter;
	SERIES_SET		*pstSeriesSet;
	PROGRAM_SET		*pstProgSet;
	INSTANCE_SET	*pstInstSet;
	EVENT_PERIOD	*pstEventPeriod;

	// SAMA_SERIES_ALTERNATIVE_SLOTÀÎ °æ¿ì¿¡´Â series setÀ» Ã£À» ¼ö ÀÖ¾î¾ßÇÏ¹Ç·Î ¿¹¿Ü¸¦ µÐ´Ù.
	if ( (ulSlot >= SAMA_TVA_MAX_SLOT)&&(ulSlot!=SAMA_SERIES_ALTERNATIVE_SLOT))
	{
		HxLOG_Debug("slot exceed the max number. slot=(%d)\n",ulSlot);
		return NULL;
	}

	for (pvIter = HLIB_LIST_First(s_pstSama_SeriesList); NULL != pvIter; pvIter = HLIB_LIST_Next(pvIter))
	{
		pstSeriesSet = (SERIES_SET *)HLIB_LIST_Data(pvIter);
		if ( pstSeriesSet == NULL )
		{
			continue;
		}
		for (pstProgSet = pstSeriesSet->pstProgList; pstProgSet != NULL; pstProgSet=pstProgSet->pNext)
		{
			for (pstInstSet = pstProgSet->pstSelectedInst; pstInstSet != NULL; pstInstSet = pstInstSet->pNext)
			{
				for (pstEventPeriod = pstInstSet->pstEventList; pstEventPeriod != NULL; pstEventPeriod = pstEventPeriod->pNext)
				{
					if (pstEventPeriod->ulSlot == ulSlot)
					{
						return pstSeriesSet;
					}
				}
			}
		}
	}

	return NULL;
}
SERIES_SET * samalib_tva_GetTrackingSeriesSetBySlotId(HUINT32 ulSlot)
{
	HxList_t	*pvIter;
	SERIES_SET		*pstSeriesSet;

	// SAMA_SERIES_ALTERNATIVE_SLOTÀÎ °æ¿ì¿¡´Â series setÀ» Ã£À» ¼ö ÀÖ¾î¾ßÇÏ¹Ç·Î ¿¹¿Ü¸¦ µÐ´Ù.
	if ( (ulSlot >= SAMA_TVA_MAX_SLOT)&&(ulSlot!=SAMA_SERIES_ALTERNATIVE_SLOT))
	{
		HxLOG_Debug("slot exceed the max number. slot=(%d)\n",ulSlot);
		return NULL;
	}

	for (pvIter = HLIB_LIST_First(s_pstSama_SeriesList); NULL != pvIter; pvIter = HLIB_LIST_Next(pvIter))
	{
		pstSeriesSet = (SERIES_SET *)HLIB_LIST_Data(pvIter);
		if ( pstSeriesSet == NULL )
		{
			continue;
		}

		if (pstSeriesSet->ulSeriesTrackingSlot == ulSlot)
		{
			return pstSeriesSet;
		}
	}

	return NULL;
}

STATIC HBOOL samalib_tva_IsSameAuth (DxEvent_t *event, SERIES_SET *pstSeriesSet)
{
	HCHAR 			szDefaultAuth[DxDEFAULT_AUTHORITY_LEN] = {0,};
	TVA_CONTENTID_T stCrid;
	HxVector_t		*vCrid;
	HUINT32		i;
	DxEventCRID_t	*contentIdentifier;
	HERROR			hErr;

	vCrid		= event->crid;
	if (vCrid == NULL)
	{
		HxLOG_Debug("there is no crid. event name=[%s] \n",SAMA_EVENT_GETNAME(event->name));
		return FALSE;
	}

	// 1. event ¾È¿¡ series set¿¡¼­ ¿ä±¸ÇÏ´Â typeÀÇ crid°¡ ÀÖ´ÂÁö Ã£´Â´Ù.
	for (i=0; i<HLIB_VECTOR_Length(vCrid); i++)
	{
		contentIdentifier = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(vCrid, i);
		if (contentIdentifier == NULL)
			continue;

		if (contentIdentifier->type != pstSeriesSet->stCrid.type)
			continue;

		(void)samalib_tva_ExtractEachPartCRID(contentIdentifier->crid
										, HLIB_STD_StrNLen(contentIdentifier->crid,DxPVR_TVANYTIME_CRID_TOTAL_LEN)
										,&stCrid , NULL );
		// 2. ÀÖ´Ù¸é eventÀÇ crid¿¡ auth°¡ ÀÖ´ÂÁö Ã£´Â´Ù.
		if (HxSTD_StrEmpty(stCrid.auth) == TRUE)
		{
			continue;
		}
		if (crid_cmp(stCrid.auth,pstSeriesSet->stCrid.auth) == 0)
		{
			return TRUE;
		}
		else
		{
			HxLOG_Debug("\033[30;47m auth diff: evt auth[%s] series auth[%s] \033[0m \n", stCrid.auth, pstSeriesSet->stCrid.auth);
			return FALSE;
		}
	}

	if (s_samaInst == NULL)
	{
		HxLOG_Error("tva inst not initialized. can't compare authority \n");
		return FALSE;
	}

	// 3. ¾ø´Ù¸é eventÀÇ serviceÀÇ default auth¸¦ Ã£´Â´Ù.
	hErr = samalib_tva_GetDefaultAuthority(event->svcuid,szDefaultAuth);
	if (hErr != ERR_OK)
	{
		HxLOG_Debug("fail to find default auth. svcuid=(%d) \n",event->svcuid);
	}

	samalib_tva_ConvertStringUpper(szDefaultAuth);

	if (crid_cmp((HUINT32*)szDefaultAuth, pstSeriesSet->stCrid.auth) == 0)
	{
		return TRUE;
	}

	HxLOG_Debug("\033[30;47m auth diff: default auth[%s] series auth[%s] \033[0m \n", szDefaultAuth, pstSeriesSet->stCrid.auth);
	return FALSE;
}

STATIC HERROR samalib_tva_AddEventToSeriesSet(
											SERIES_SET * const	pstSeriesSet,
											const HUINT32		ulSvcUid,
											const TVA_CONTENTID_T *pProgCrid,
											const HUINT16		event_id,
											const UNIXTIME		startTime,
											const UNIXTIME		endTime,
											const HUINT32		eventSource,
											const HUINT32		ulSlot,
											const HCHAR			*szEventName)
{
	PROGRAM_SET *pstProgSet;
	INSTANCE_SET *pstInstSet;
	EVENT_PERIOD *event;
	HBOOL is_new_prog = FALSE;
	HBOOL is_new_inst = FALSE;
	HINT32 i;
	HUINT32 num_recorded_crids = 0;
	TVA_CONTENTID_T	*pstRecordedCrid = NULL;


	if (pstSeriesSet->pstRecordedProgCrids != NULL)
	{
		num_recorded_crids = HLIB_VECTOR_Length(pstSeriesSet->pstRecordedProgCrids);
	}

	/* do not add if this prog crid is already recorded */
	for (i = 0; i < num_recorded_crids; i++)
	{
		pstRecordedCrid = HLIB_VECTOR_ItemAt(pstSeriesSet->pstRecordedProgCrids, i );
		if (pstRecordedCrid == NULL)
		{
			HxLOG_Debug("Null crid. \n");
			continue;
		}

		if (pstRecordedCrid->type == SAMA_CRID_TYPE_PROG
			&& crid_cmp(pstRecordedCrid->data, pProgCrid->data) == 0
			&& crid_cmp(pstRecordedCrid->auth, pProgCrid->auth) == 0)
		{
			return ERR_FAIL;
		}
	}

	/*
	for (i = 0; i < DxRSV_NUM_CANCEL_PROG_SAVE; i++)
	{
		if (pstSeriesSet->stCanceledProgCrid[i].type == SAMA_CRID_TYPE_PROG
			&& crid_cmp(pstSeriesSet->stCanceledProgCrid[i].data, pProgCrid->data) == 0
			&& crid_cmp(pstSeriesSet->stCanceledProgCrid[i].auth, pProgCrid->auth) == 0)
		{
			return ERR_FAIL;
		}
	}
	*/

	/* find PROGRAM_SET */
	pstProgSet = NULL;
	if (pProgCrid->data[0])
		pstProgSet = get_program_set(pstSeriesSet, pProgCrid);

	if (pstProgSet == NULL)
	{
		pstProgSet = new_program_set(pProgCrid);
		if (pstProgSet == NULL)
			return ERR_FAIL;

		is_new_prog = TRUE;
	}

	/* find INSTANCE_SET */
	pstInstSet = get_instance_set(pstProgSet, pProgCrid->imi);
	if (pstInstSet == NULL)
	{
		/* alloc new INSTANCE_SET, set and append it */
		pstInstSet = new_instance_set(pstProgSet, pProgCrid->imi);
		if (pstInstSet == NULL)
		{
			if (is_new_prog)
				free_prog_set(pstProgSet);

			return ERR_FAIL;
		}
		is_new_inst = TRUE;
	}
	else
	{
		/* founded INSTANCE_SET with same IMI value */
		EVENT_PERIOD *tmp_event;
		INSTANCE_SET *tmp_instSet;

		tmp_instSet = pstInstSet;

find_instance_set_to_link:

		tmp_event = tmp_instSet->pstEventList;
		while (tmp_event)
		{
			if (tmp_event->ulSvcUid != ulSvcUid)
			{
				if (tmp_event->startTime == startTime && tmp_event->endTime == endTime)
				{
					/* Çä, °°Àº ½Ã°£À» °¡Áø split event°¡ ´Ù¸¥ ¼­ºñ½º¿¡¼­ µé¾î¿Ô´Ù. ¾ÆÇÏ regional channelÀÇ multiple eventÀÓÀÌ Æ²¸²¾øÀ»°ÍÀÌ´Ù  */
					/* ÀÌ¹Ì »ý¼ºµÈ instance_setÀÌ ¾ø´ÂÁö Ã£¾Æ¼­ °Å±â¿¡ ¸Å´Þ¾ÆÁÖÀÚ, alternative·Î ÀÌ¿ë ÇÒ¼ö ÀÖ°Ô. */
					if (tmp_instSet->pNext != NULL)
					{
						tmp_instSet = tmp_instSet->pNext;
						goto find_instance_set_to_link;
					}

					 /* regional channel¿¡ ÀÖ´Â °°Àº ProgCRID & IMI¸¦ °¡Áø multiple event µéÀÌ ´Ù¸¥ instance setÀ» ±¸¼ºÇÏµµ·Ï ÇÏÀÚ. */
					 /* ±×·¸Áö ¾Ê°í °°Àº instance_set¾È¿¡¼­ Event_period¸¦ ±¸¼ºÇÏ¸é ÀÚ±âµé³¢¸® Ãæµ¹ÀÌ ³­´Ù.*/
					/* create new instance */
					pstInstSet = new_instance_set(pstProgSet, pProgCrid->imi);
					if (pstInstSet == NULL)
					{
						if (is_new_prog)
							free_prog_set(pstProgSet);

						return ERR_FAIL;
					}

					is_new_inst = TRUE;
					break;
				}
			}
			else
			{
				if (tmp_event->startTime != startTime || tmp_event->endTime != endTime)
				{
					/* °°Àº svc handleÀ» °¡Áø split event°¡ linkµÇ¾î ÀÖ´Â instance¿¡, ½Ã°£ÀÌ ´Ù¸¥ split event¸¦ link ÇÏµµ·Ï ÇÏÀÚ. */
					pstInstSet = tmp_instSet;
				}
			}
			tmp_event = tmp_event->pNext;
		}
	}

	event = new_event_period(pstInstSet, startTime, endTime, ulSvcUid, event_id, eventSource, ulSlot, szEventName);
	if (event == NULL)
	{
		if (is_new_prog)
			free_prog_set(pstProgSet);
		if (is_new_inst)
			free_inst_set(pstInstSet);

		return ERR_FAIL;
	}

	/* XXX: check if split events are not apart more than 3 hours */

	/* link them */
	if (is_new_prog)
	{
		pstProgSet->pstSelectedInst = pstInstSet;
		pstProgSet->pstCandidateInst = pstInstSet;
		pstProgSet->pstSeriesSet = pstSeriesSet;
		pstProgSet->pNext = pstSeriesSet->pstProgList;
		pstSeriesSet->pstProgList = pstProgSet;
	}

	if (is_new_inst)
	{
		add_instance_set(pstProgSet, pstInstSet);
	}

	return ERR_OK;
}
/***************************************************************************************************
*       ÁÖÀÇ !!!!
*    Dapi callback¿¡¼­´Â ÀÌ ÇÔ¼ö¸¦ ºÎ¸£¸é ¾ÈµÈ´Ù !
*    ¿Ö³ÄÇÏ¸é makecrid¸¦ È£ÃâÇÏ±â À§ÇØ¼­ dapi get DxService_t °¡ ÇÊ¿äÇÏ±â ¶§¹®ÀÌ´Ù.
*    ¹Ýµå½Ã »ç¿ëÇØ¾ß ÇÑ´Ù¸é, service cache°¡ ÀÖ´Â°æ¿ì¿¡¸¸ »ç¿ëÇÏµµ·Ï ÇÑ´Ù.
****************************************************************************************************/
HERROR samalib_tva_AddDxEventToSeriesSet(DxEvent_t *event, SERIES_SET *pstSeriesSet, UNIXTIME ulTimeAfter, HBOOL bInitial, HUINT32 ulSlot)
{
	UNIXTIME			startTime, endTime;
	TVA_CONTENTID_T		tmpProgCrid;
	EVENT_PERIOD		*instance;
	HUINT32				ulEventSource = 0;

	HxVector_t			*pContentIdentifier;
	DxEventCRID_t		*pvEvtCID;
	HUINT32				ulNumOfCids, ulCidIndex;

	HUINT32				ulSeason = 0;
	HUINT32				ulEpisode = 0;

	if(event == NULL || pstSeriesSet == NULL)
	{
		return ERR_FAIL;

	}

	// ÀÌ¹Ì ½ÃÀÛÇÑ event´Â ³ÖÁö ¾Ê´Â´Ù.
	startTime = event->startTime;
	endTime = event->startTime+event->duration;

	if(ulTimeAfter > 0)
	{
		if (startTime < ulTimeAfter)
		{
			HxLOG_Debug("event already started. evt StartTime = (%d), curr time = (%d)\n",startTime, ulTimeAfter);
			return ERR_FAIL;
		}
	}

	pContentIdentifier = event->crid;
	if ( pContentIdentifier != NULL )
	{
		ulNumOfCids = HLIB_VECTOR_Length(pContentIdentifier);
	}
	else
	{
		ulNumOfCids = 0;
	}

	SAMA_MemSet(&tmpProgCrid, 0, sizeof(TVA_CONTENTID_T));
	for ( ulCidIndex=0; ulCidIndex<ulNumOfCids; ulCidIndex++)
	{
		pvEvtCID = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(pContentIdentifier, ulCidIndex);
		if (pvEvtCID->type != SAMA_CRID_TYPE_PROG)
		{
			continue;
		}

		samalib_tva_MakeCrid(pvEvtCID->crid, (HCHAR*)pstSeriesSet->stCrid.auth, &tmpProgCrid);
		tmpProgCrid.type = SAMA_CRID_TYPE_PROG;

		break;
	}

	// TODO: scrid´Â ÀÖÀ¸³ª pcrid´Â ¾ø´Â °æ¿ì¿¡ ´ëÇØ¼­ Ã³¸®ÇØÁØ´Ù.
	if(tmpProgCrid.type != SAMA_CRID_TYPE_PROG && bInitial == FALSE)
	{
		// Series Crid ¸¸ ÀüÀçÇÒ °æ¿ì ÃÊ±â Initial series set ±¸¼º½Ã¿¡¸¸ series set À¸·Î ±¸¼º.
		HxLOG_Debug("Programme crid doesn't exists. \n");
//		return ERR_FAIL;
	}

	HxLOG_Debug("series set:%s add eventid(%d) svcuid(%d) %s \n", samalib_tva_ConvertCridToStr(&(pstSeriesSet->stCrid)),event->svcuid, event->eventId, samalib_tva_ConvertCridToStr(&tmpProgCrid));

	if (event->type == eDxEPG_TYPE_PF)
	{
		ulEventSource = SAMA_EVENT_BY_EIT_PF;
	}
	else if (event->type == eDxEPG_TYPE_SC)
	{
		ulEventSource = SAMA_EVENT_BY_EIT_SC;
	}
	else
	{
		ulEventSource = SAMA_EVENT_BY_IP;
	}

	instance = samalib_tva_GetEventInScheduleList(event->svcuid, event->eventId);
	if (instance == NULL)
		instance = samalib_tva_GetEventInSeriesSet(pstSeriesSet, event->svcuid, event->eventId);

	if (instance)
	{
		/*
		// Ã£¾Æ³½ instanceÀÇ imi¿Í ÀÔ·Â ¹ÞÀº eventÀÇ imi°¡ ´Ù¸£´Ù´Â °ÍÀº single <-> split °£ÀÇ ÀüÈ¯ÀÌ ÀÖÀ» ¼ö ÀÖ´Ù´Â °Í....
		if (instance->pstInstSet.imi != tmpProgCrid.imi)
		{
			// TODO: single <-> split°£ÀÇ ÀüÈ¯ÀÌ ÀÏ¾î³µ´Ù. ÀÌ¿¡ ´ëÇÑ Ã³¸®°¡ ÇÊ¿äÇÏ´Ù.
		}
		*/

		if (event->type == eDxEPG_TYPE_IP)
		{
			ulSeason  = event->extension.ipEpg.season;
			ulEpisode  = event->extension.ipEpg.episodeNum;
		}
		samalib_tva_AdjustEvent(pstSeriesSet, instance, startTime, endTime, ulSeason,ulEpisode, ulEventSource);
	}
	else
	{
		DxEventString_t *eventString = NULL;
		HCHAR	buf[4];
		HxLANG_Id_e language;
		HERROR		hErr;

		hErr = HAPI_GetRegistryStr("mainlang", buf, 4);
		language = (ERR_OK == hErr) ? HLIB_LANG_639ToId(buf) : eLangID_NONE;

		if (event->name != NULL)
		{
			eventString = HLIB_VECTOR_Find (event->name, samalib_tva_GetEventString, (void *)language);
			if(eventString == NULL)
				eventString = HLIB_VECTOR_First(event->name);
		}

		if(eventString)
		{
			samalib_tva_AddEventToSeriesSet(pstSeriesSet, event->svcuid, &tmpProgCrid,
									event->eventId, startTime, endTime, ulEventSource, ulSlot,eventString->string);
		}
		else
		{
			samalib_tva_AddEventToSeriesSet(pstSeriesSet, event->svcuid, &tmpProgCrid,
									event->eventId, startTime, endTime, ulEventSource, ulSlot,NULL);
		}
	}

	return ERR_OK;
}

STATIC HERROR samalib_tva_AddDxEventsToSeriesSet (HxList_t *pstSeriesEventsList, SERIES_SET *pstSeriesSet)
{
	DxEvent_t			*Event;
	UNIXTIME			utCurrTime;
	HxList_t			*pvIter;
	HBOOL				bIsSameAuth = FALSE;
	HERROR				hErr;

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if(UNIX_FIRST_DAY >= utCurrTime)
	{
		HxLOG_Error("time error. current sys time = (%d)\n",utCurrTime);
	}

	for (pvIter = HLIB_LIST_First(pstSeriesEventsList); pvIter != NULL; pvIter = HLIB_LIST_Next(pvIter))
	{
		Event = (DxEvent_t *)HLIB_LIST_Data(pvIter);
		if (Event == NULL)
		{
			HxLOG_Debug("null event \n");
			// continue? break???
			continue;
		}

		bIsSameAuth = samalib_tva_IsSameAuth(Event, pstSeriesSet);
		if (bIsSameAuth != TRUE)
		{
			HxLOG_Debug("Auth is not same. \n");
			continue;
		}

		// 1. series set¿¡ Ãß°¡
		hErr = samalib_tva_AddDxEventToSeriesSet(Event, pstSeriesSet, utCurrTime, TRUE, DxRSV_SLOT_NULL);
		if (hErr != ERR_OK)
		{
			HxLOG_Debug("SAMALIB_TVA_AddDxEventToSeriesSet failed. event=[%s]\n",SAMA_EVENT_GETNAME(Event->name));
		}

	}

	return ERR_OK;
}
STATIC HERROR samalib_tva_AdjustEvent(
											SERIES_SET * const pstSeriesSet,
											EVENT_PERIOD	*event,
											const UNIXTIME	startTime,
											const UNIXTIME	endTime,
											const HUINT32	ulSeason,
											const HUINT32	ulEpisode,
											const HUINT32	eventSource)
{
	HUINT32	ulOrgEventSource;
	HBOOL	bIsMorePrioritized;

	// 1. priority¸¦ µûÁú ¼ö ¾ø´Â ÇÊµå¸¦ ¸ÕÀú ¾÷µ¥ÀÌÆ® ÇÑ´Ù.
	if (eventSource == SAMA_EVENT_BY_IP)
	{
		if (event->flag & SAMA_EVENT_SEASON_EPISODE_SETTED)
		{
			// already setted. ignore
		}
		else
		{
			event->ulSeason = ulSeason;
			event->ulEpisode = ulEpisode;
			HxLOG_Debug(HxANSI_COLOR_GREEN("Event updated name=[%s]. season=(%d) episode=(%d) \n"),event->szEventName, ulSeason, ulEpisode);
			event->flag |= SAMA_EVENT_UPDATED;
			event->flag |= SAMA_EVENT_SEASON_EPISODE_SETTED;
		}
	}

	// 2. priority¸¦ µûÁö´Â ÇÊµå¸¦ ¾÷µ¥ÀÌÆ® ÇÑ´Ù.
	ulOrgEventSource = samalib_tva_GetEventSource(event->flag);
	bIsMorePrioritized = samalib_tva_IsMorePrioritizedEventSource(ulOrgEventSource, eventSource);
	if (bIsMorePrioritized != TRUE)
	{
		HxLOG_Debug(HxANSI_COLOR_RED("Can't update with less prioritized source. org Source(%d) > new source(%d) \n"),ulOrgEventSource, eventSource);
		return ERR_FAIL;
	}

	if (event->pstInstSet->pstProgSet->pstSeriesSet == pstSeriesSet && (event->startTime != startTime || event->endTime != endTime))
	{
		{
			HxDATETIME_t	stStartTime, stEndTime;
			HxLOG_Debug("\033[30;47m[%s:%d] Update schedule. slot=(%d) \033[0m \n",__FUNCTION__, __LINE__,event->ulSlot);

			HLIB_DATETIME_ConvertUnixTimeToDateTime(event->startTime,&stStartTime);
			HLIB_DATETIME_ConvertUnixTimeToDateTime(event->endTime,&stEndTime);
			HxLOG_Debug(" Before %d/%d/%d (%02d:%02d:%02d ~ %02d:%02d:%02d)\n",
					stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay,
					stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond,
					stEndTime.stTime.ucHour, stEndTime.stTime.ucMinute, stEndTime.stTime.ucSecond);
			HLIB_DATETIME_ConvertUnixTimeToDateTime(startTime,&stStartTime);
			HLIB_DATETIME_ConvertUnixTimeToDateTime(endTime,&stEndTime);
			HxLOG_Debug("  After %d/%d/%d (%02d:%02d:%02d ~ %02d:%02d:%02d)\n",
					stStartTime.stDate.usYear, stStartTime.stDate.ucMonth, stStartTime.stDate.ucDay,
					stStartTime.stTime.ucHour, stStartTime.stTime.ucMinute, stStartTime.stTime.ucSecond,
					stEndTime.stTime.ucHour, stEndTime.stTime.ucMinute, stEndTime.stTime.ucSecond);

		}
		event->flag |= SAMA_EVENT_UPDATED;

		/* overwrite it */
		event->startTime = startTime;
		event->endTime = endTime;
	}

	// TODO: updated flag¸¦ ¿©±â¿¡ ³õÀ¸¸é DxScheduleÀÇ epg type±îÁö ¾÷µ¥ÀÌÆ® ÇÒ ¼ö ÀÖ´Âµ¥ ÇØ¾ß ÇÏ³ª?
	event->flag |= eventSource;

	return ERR_OK;
}

STATIC HERROR samalib_tva_AddEventToProgSet(
													PROGRAM_SET * const	pstProgSet,
													const HUINT32		ulSvcUid,
													const TVA_CONTENTID_T *pProgCrid,
													const HUINT16		event_id,
													const UNIXTIME		startTime,
													const UNIXTIME		endTime,
													const HUINT32		eventSource,
													const HUINT32		ulSlot)
{
	INSTANCE_SET *pstInstSet;
	EVENT_PERIOD *event;
	SERIES_SET *pstSeriesSet;
	HBOOL is_new_inst = FALSE;
	HINT32 i;
	HUINT32 num_recorded_crids = 0;
	TVA_CONTENTID_T	*pstRecordedCrid = NULL;


	pstSeriesSet = pstProgSet->pstSeriesSet;

	if (pstSeriesSet->pstRecordedProgCrids != NULL)
	{
		num_recorded_crids = HLIB_VECTOR_Length(pstSeriesSet->pstRecordedProgCrids);
	}

	/* do not add if this prog crid is already recorded */
	for (i = 0; i < num_recorded_crids; i++)
	{
		pstRecordedCrid = HLIB_VECTOR_ItemAt(pstSeriesSet->pstRecordedProgCrids, i );
		if (pstRecordedCrid == NULL)
		{
			HxLOG_Debug("Null crid. \n");
			continue;
		}

		if (pstRecordedCrid->type == SAMA_CRID_TYPE_PROG
			&& crid_cmp(pstRecordedCrid->data, pProgCrid->data) == 0
			&& crid_cmp(pstRecordedCrid->auth, pProgCrid->auth) == 0)
		{
			return ERR_FAIL;
		}
	}

	/*
	for (i = 0; i < DxRSV_NUM_CANCEL_PROG_SAVE; i++)
	{
		if (pstSeriesSet->stCanceledProgCrid[i].type == SAMA_CRID_TYPE_PROG
			&& crid_cmp(pstSeriesSet->stCanceledProgCrid[i].data, pProgCrid->data) == 0
			&& crid_cmp(pstSeriesSet->stCanceledProgCrid[i].auth, pProgCrid->auth) == 0)
		{
			return ERR_FAIL;
		}
	}
	*/

	/* find INSTANCE_SET */
	pstInstSet = get_instance_set(pstProgSet, pProgCrid->imi);
	if (pstInstSet == NULL)
	{
		/* alloc new INSTANCE_SET, set and append it */
		pstInstSet = new_instance_set(pstProgSet, pProgCrid->imi);
		if (pstInstSet == NULL)
			return ERR_FAIL;

		is_new_inst = TRUE;
	}

	event = new_event_period(pstInstSet, startTime, endTime, ulSvcUid, event_id, eventSource, ulSlot, NULL);
	if (event == NULL)
	{
		if (is_new_inst)
			free_inst_set(pstInstSet);

		return ERR_FAIL;
	}

	/* XXX: check if split events are not apart more than 3 hours */

	/* link them */
	if (is_new_inst)
	{
		add_instance_set(pstProgSet, pstInstSet);
	}

	return ERR_OK;
}

STATIC HERROR samalib_tva_AddPeriodToSeriesSet(HUINT32 ulSvcUid, HUINT32 ulEventId, UNIXTIME startTime, UNIXTIME endTime, SERIES_SET *pstSeriesSet, HUINT32 ulSlot)
{
	PROGRAM_SET	*pProgSet;
	INSTANCE_SET	*pInstSet;
	EVENT_PERIOD	*event;
	HBOOL			is_new_prog = FALSE;
	HBOOL			is_new_inst = FALSE;
	TVA_CONTENTID_T	tmp_crid;

	SAMA_MemSet(&tmp_crid, 0, sizeof(tmp_crid));

	/* find PROGRAM_SET */
	pProgSet = get_program_set(pstSeriesSet, &tmp_crid);
	if (pProgSet == NULL)
	{
		pProgSet = new_program_set(&tmp_crid);
		if (pProgSet == NULL)
		{
			return ERR_FAIL;
		}
		is_new_prog = TRUE;

		pInstSet = new_instance_set(pProgSet, tmp_crid.imi);
		if (pInstSet == NULL)
		{
			free_prog_set(pProgSet);
			return ERR_FAIL;
		}
		is_new_inst = TRUE;
	}
	else
	{
		pInstSet = pProgSet->pstInstList;
	}

	event = new_event_period(pInstSet, startTime, endTime, ulSvcUid, ulEventId, 0, ulSlot, NULL);
	if (event == NULL)
	{
		if (is_new_prog)
			free_prog_set(pProgSet);
		if (is_new_inst)
			free_inst_set(pInstSet);
		return ERR_FAIL;
	}

	/* link them */
	if (is_new_prog)
	{
		pProgSet->pstSelectedInst = pInstSet;
		pProgSet->pstCandidateInst = pInstSet;
		pProgSet->pstSeriesSet = pstSeriesSet;
		pProgSet->pNext = pstSeriesSet->pstProgList;
		pstSeriesSet->pstProgList = pProgSet;
	}
	if (is_new_inst)
	{
		if (pProgSet->pstInstList == NULL)
		{
			pProgSet->pstInstList = pInstSet;
		}
		else
		{
			INSTANCE_SET *tmp_inst;
			for (tmp_inst = pProgSet->pstInstList; tmp_inst->pNext; tmp_inst = tmp_inst->pNext) ;
			tmp_inst->pNext = pInstSet;
		}
	}

	return ERR_OK;
}


STATIC EVENT_PERIOD *samalib_tva_GetEventInSeriesSet(SERIES_SET *pstSeriesSet, HUINT32 ulSvcUid, HUINT32 ulEventId)
{
	PROGRAM_SET *pstProgSet;
	INSTANCE_SET *pstInstSet;
	EVENT_PERIOD *event;
#if defined(CONFIG_OP_FREESAT)
	HERROR		hSrcErr = ERR_FAIL, hCmpErr = ERR_FAIL;
	HUINT16		usSrcFsatSvcId 	= FSAT_SVCID_INVALID;
	HUINT16		usCmpFsatSvcId = FSAT_SVCID_INVALID;
#endif

#if defined(CONFIG_OP_FREESAT)
	hSrcErr = FSAT_DB_SVC_GetSvcIdentifier(ulSvcUid, &usSrcFsatSvcId);
#endif

	for (pstProgSet = pstSeriesSet->pstProgList; pstProgSet; pstProgSet = pstProgSet->pNext)
	{
		for (pstInstSet = pstProgSet->pstInstList; pstInstSet; pstInstSet = pstInstSet->pNext)
		{
			for (event = pstInstSet->pstEventList; event; event = event->pNext)
			{
#if defined(CONFIG_OP_FREESAT)
				hCmpErr = FSAT_DB_SVC_GetSvcIdentifier(event->ulSvcUid, &usCmpFsatSvcId);
				if(hSrcErr == ERR_OK && hCmpErr == ERR_OK)
				{
					if(usCmpFsatSvcId == usSrcFsatSvcId && event->ulEventId == ulEventId)
					{
						HxLOG_Debug("\033[30;43m event id %x    fsatsvcid : [%x][%x]  handle : [%x][%x]\033[0m\n",
							ulEventId, usSrcFsatSvcId, usCmpFsatSvcId, ulSvcUid, event->ulSvcUid);
						return event;
					}
				}
				else
#endif
				{
					if (event->ulSvcUid == ulSvcUid && event->ulEventId == ulEventId)
					{
						/* it's duplicated. */
						return event;
					}
				}

			}
		}
	}

	return NULL;
}

STATIC EVENT_PERIOD *samalib_tva_GetEventInScheduleList(HINT32 ulSvcUid, HUINT16 ulEventId)
{
	PROGRAM_SET	*pProgSet;
	INSTANCE_SET	*pInstSet;
	EVENT_PERIOD	*event;
	HxList_t	*pvIter;

	for (pvIter = HLIB_LIST_First(s_pstSama_SeriesList); NULL != pvIter; pvIter = HLIB_LIST_Next(pvIter))
	{
		SERIES_SET		*pstSeriesSet = (SERIES_SET *)HLIB_LIST_Data(pvIter);

		if ( pstSeriesSet == NULL )
			continue;

		for (pProgSet = pstSeriesSet->pstProgList; pProgSet; pProgSet = pProgSet->pNext)
		{
			for (pInstSet = pProgSet->pstInstList; pInstSet; pInstSet = pInstSet->pNext)
			{
				for (event = pInstSet->pstEventList; event; event = event->pNext)
				{
					if (event->ulSvcUid == ulSvcUid && event->ulEventId == ulEventId)
					{
						/* it's duplicated. */
						return event;
					}
				}
			}
		}
	}

	return NULL;
}

#if 0
STATIC HERROR samalib_tva_CheckAndAddEvent(
												SERIES_SET	*pstSeriesSet,
												HUINT32		ulSvcUid,
												HUINT32		ulEventId,
												UNIXTIME	startTime,
												UNIXTIME	endTime,
												TVA_CONTENTID_T *pSeriesCrid,
												TVA_CONTENTID_T *pProgCrid,
												HBOOL		bCheckSchedule,
												HBOOL		bByEitPf)
{
	EVENT_PERIOD *event = NULL;

	if (pstSeriesSet->stCrid.type == SAMA_CRID_TYPE_SERIES)
	{
		if (pSeriesCrid->type == SAMA_CRID_TYPE_SERIES)
		{
			if (crid_cmp(pstSeriesSet->stCrid.data, pSeriesCrid->data) == 0
				 && crid_cmp(pstSeriesSet->stCrid.auth, pSeriesCrid->auth) == 0)
			{
				/* This is original series */
				if (bCheckSchedule)
					event = samalib_tva_GetEventInScheduleList(ulSvcUid, ulEventId);

				if (event == NULL)
					event = samalib_tva_GetEventInSeriesSet(pstSeriesSet, ulSvcUid, ulEventId);

				if (event)
				{
					samalib_tva_AdjustEvent(pstSeriesSet, event, startTime, endTime, bByEitPf);
				}
				else
				{
					samalib_tva_AddEventToSeriesSet(pstSeriesSet, ulSvcUid, pProgCrid,
													ulEventId, startTime, endTime, bByEitPf, DxRSV_SLOT_NULL, NULL);
				}
				return ERR_OK;
			}
		}

		if (pProgCrid->type == SAMA_CRID_TYPE_PROG)
		{
			PROGRAM_SET *pstProgSet;

			for (pstProgSet = pstSeriesSet->pstProgList; pstProgSet; pstProgSet = pstProgSet->pNext)
			{
				if (crid_cmp(pstProgSet->stProgCrid.data, pProgCrid->data) == 0
					&& crid_cmp(pstProgSet->stProgCrid.auth, pProgCrid->auth) == 0)
				{
					/* This is an alternative event of original series event */
					if (bCheckSchedule)
						event = samalib_tva_GetEventInScheduleList(ulSvcUid, ulEventId);

					if (event == NULL)
						event = samalib_tva_GetEventInSeriesSet(pstSeriesSet, ulSvcUid, ulEventId);

					if (event)
					{
						samalib_tva_AdjustEvent(pstSeriesSet, event, startTime, endTime, bByEitPf);
					}
					else
					{
						samalib_tva_AddEventToProgSet(pstProgSet, ulSvcUid, pProgCrid,
														ulEventId, startTime, endTime, bByEitPf, DxRSV_SLOT_NULL);
					}
					return ERR_OK;
				}
			}
		}
	}
	else if (pstSeriesSet->stCrid.type == SAMA_CRID_TYPE_PROG)
	{
		if (pProgCrid->type == SAMA_CRID_TYPE_PROG)
		{
			if (crid_cmp(pstSeriesSet->stCrid.data, pProgCrid->data) == 0
				&& crid_cmp(pstSeriesSet->stCrid.auth, pProgCrid->auth) == 0)
			{
				if (bCheckSchedule)
					event = samalib_tva_GetEventInScheduleList(ulSvcUid, ulEventId);

				if (event == NULL)
					event = samalib_tva_GetEventInSeriesSet(pstSeriesSet, ulSvcUid, ulEventId);

				if (event)
				{
					samalib_tva_AdjustEvent(pstSeriesSet, event, startTime, endTime, bByEitPf);
				}
				else
				{
					samalib_tva_AddEventToSeriesSet(pstSeriesSet, ulSvcUid, pProgCrid,
														ulEventId, startTime, endTime, bByEitPf, DxRSV_SLOT_NULL, NULL);
				}
				return ERR_OK;
			}
		}
	}
	return ERR_OK;
}
#endif

STATIC HERROR samalib_tva_RemoveMatchingEventPeriod(HUINT32 ulSvcUid, HUINT32 ulEventId, SERIES_SET *pstSeriesSet, TVA_CONTENTID_T *pstRemovedProgCrid)
{
	PROGRAM_SET	*pstProgSet;
	EVENT_PERIOD	*pstEventPeriod;
	void				*pToBeRemoved;
	HERROR			result = ERR_FAIL;

	pstRemovedProgCrid->type = 0;
	pstProgSet = pstSeriesSet->pstProgList;
	while (pstProgSet)
	{
		pstEventPeriod = pstProgSet->pstSelectedInst->pstEventList;
		while (pstEventPeriod)
		{
			if (pstEventPeriod->ulSvcUid == ulSvcUid && pstEventPeriod->ulEventId == ulEventId)
			{
				pToBeRemoved = pstEventPeriod;

				result = samalib_tva_RemoveEventPeriod(pstProgSet->pstSelectedInst, pToBeRemoved);
				break;
			} else {
				pstEventPeriod = pstEventPeriod->pNext;
			}
		}

		if (pstProgSet->pstSelectedInst->pstEventList == NULL)
		{
			/* all events are removed. remove this prog_set */
			tva_memcpy(pstRemovedProgCrid, &pstProgSet->stProgCrid, sizeof(TVA_CONTENTID_T));
			pToBeRemoved = pstProgSet;
			pstProgSet = pstProgSet->pNext;

			samalib_tva_RemoveProgSet(pstSeriesSet, pToBeRemoved);
		}
		else
		{
			pstProgSet = pstProgSet->pNext;
		}

		if (result == ERR_OK)
			break;
	}

	return result;
}

STATIC HERROR samalib_tva_RemovePastEventPeriod(SERIES_SET *pstSeriesSet, UNIXTIME cur_time, HBOOL bReplaceSelectedInst)
{
	PROGRAM_SET *pProgSet;
	INSTANCE_SET *pInstSet;
	EVENT_PERIOD *pEventPeriod;
	void *pToBeRemoved;
	HERROR result = ERR_FAIL;

#if CONFIG_DEBUG && 0
	HxDATETIME_t	time;

	UTIL_TCONV_ConvertUnixTimeToTime(cur_time, &time);
	HxLOG_Debug("[samalib_tva_RemovePastEventPeriod] cur_time = (%dh:%dm:%ds)\n",(int)time.ucHour,(int)time.ucMinute,(int)time.ucSecond);
#endif

	pProgSet = pstSeriesSet->pstProgList;
	while (pProgSet)
	{
		pInstSet = pProgSet->pstInstList;
		while (pInstSet)
		{
			pEventPeriod = pInstSet->pstEventList;
			while (pEventPeriod)
			{
				if (pEventPeriod->startTime < cur_time)
				{
					pToBeRemoved = pEventPeriod;
					pEventPeriod = pEventPeriod->pNext;

					HxLOG_Debug("[REMOVE Event Period]\n");
					samalib_tva_PrintEventPeriod(FALSE, pToBeRemoved);

					result = samalib_tva_RemoveEventPeriod(pInstSet, pToBeRemoved);
				}
				else
				{
					pEventPeriod = pEventPeriod->pNext;
				}
			}

			if (pInstSet->pstEventList == NULL)
			{
				/* all events are removed. remove this inst_set */
				if (bReplaceSelectedInst)
				{
					if (pProgSet->pstSelectedInst == pInstSet)
						pProgSet->pstCandidateInst = pProgSet->pstSelectedInst = pInstSet->pNext;
				}
				else
				{
					if (pProgSet->pstSelectedInst == pInstSet)
					{
						pProgSet->pstSelectedInst = NULL;
						break;
					}
				}
				pToBeRemoved = pInstSet;
				pInstSet = pInstSet->pNext;

				if (pProgSet->pstInstList == pToBeRemoved)
				{
					pProgSet->pstInstList = pInstSet;
				}
				else
				{
					INSTANCE_SET *pTmpInstSet = pProgSet->pstInstList;

					while (pTmpInstSet && pTmpInstSet->pNext != pToBeRemoved)
						pTmpInstSet = pTmpInstSet->pNext;

					if(pTmpInstSet)
						pTmpInstSet->pNext = pInstSet;
				}

				HxLOG_Debug("[REMOVE InstanceSet]\n");
				samalib_tva_PrintInstanceSet(FALSE, pToBeRemoved);

				/* µÎ°³°¡ °°À» ¼ö ¾ø´Ù. ÀÌ °æ¿ì¶ó¸é Linked List°¡ È¯¿µ ±¸Á¶°¡ µÈ °æ¿ì´Ù. Prevent Ã³¸®¸¦ À§ÇØ¼­ Ãß°¡. */
				if(pInstSet == pToBeRemoved)
					pInstSet = NULL;

				free_inst_set(pToBeRemoved);
			}
			else
			{
				pInstSet = pInstSet->pNext;
			}
		}

		if (pProgSet->pstSelectedInst == NULL)
		{
			/* all events are removed. remove this prog_set */
			pToBeRemoved = pProgSet;
			pProgSet = pProgSet->pNext;

			HxLOG_Debug("[REMOVE ProgSet]\n");
			samalib_tva_PrintProgramSet(FALSE, pToBeRemoved);

			samalib_tva_RemoveProgSet(pstSeriesSet, pToBeRemoved);
		}
		else
		{
			pProgSet = pProgSet->pNext;
		}
	}

	return result;
}

STATIC HERROR samalib_tva_RearrangeSeriesSet(HUINT32 ulRsvCnt, DxSchedule_t **ppstScheduleList)
{
#if 0
	HUINT32			i, j;
	HBOOL			bRsv;
	SERIES_SET		*pstSeriesSet;

	void*			pvIter;

	for (pvIter = HLIB_TREE_Iterator(s_pstSama_SeriesList); NULL != pvIter; pvIter = HLIB_TREE_NextIterator(pvIter))
	{
		SERIES_SET		*pstSeriesSet = (SERIES_SET *)HLIB_TREE_IteratorValue(pvIter);

		if ( pstSeriesSet == NULL )
			continue;
	}

	for(i = 0; i < MAX_RSV_ALL; i++)
	{
		bRsv = FALSE;
		for(j = 0; j < ulRsvCnt; j++)
		{
			if(ppstScheduleList[j]->ulSlot == i)
			{
				bRsv = TRUE;
				break;
			}
		}

		if(bRsv == FALSE)
		{
			/* Reservation µÇ¾îÀÖÁö ¾ÊÀº Series Set Á¤¸® */
			pstSeriesSet = &(ppstScheduleList->uExtInfo.stRec.ex.freesat.stSeriesSet);
			samalib_tva_ReleaseSeriesSet(pstSeriesSet);
			SAMA_MemSet(pstSeriesSet, 0, sizeof(SERIES_SET));
		}
	}

	samalib_tva_ReleaseSeriesSet(&s_stSeries_Set[MAX_RSV_ALL]);
	SAMA_MemSet(&s_stSeries_Set[MAX_RSV_ALL], 0, sizeof(SERIES_SET));
#endif

	return ERR_OK;
}

STATIC HBOOL	samalib_tva_GetEventString(void *data, void *language)
{
	DxEventString_t *pstEventString = (DxEventString_t *)data;
	HxLANG_Id_e eLanguage = (HxLANG_Id_e)language;

	if(pstEventString && pstEventString->language == eLanguage)
		return TRUE;
	else
		return FALSE;
}

STATIC void		samalib_tva_DeleteEvent_CB (void *p)
{
	DxEVENT_Delete((DxEvent_t *)p);
}

STATIC DxRsvType_e		samalib_tva_GetRsvTypeFromSeriesSet (SERIES_SET *pstSeriesSet)
{
	if (pstSeriesSet == NULL)
	{
		HxLOG_Error("null param. \n");
		return DxRSVTYPE_NOTHING;
	}

	return pstSeriesSet->eRsvType;
}

STATIC DxRsvType_e		samalib_tva_GetRsvTypeFromEventPeriod (EVENT_PERIOD *pstEvent)
{
	if (pstEvent == NULL
		|| pstEvent->pstInstSet == NULL
		|| pstEvent->pstInstSet->pstProgSet == NULL
		|| pstEvent->pstInstSet->pstProgSet->pstSeriesSet == NULL)
	{
		HxLOG_Error("null event period. \n");
		return DxRSVTYPE_NOTHING;
	}

	return samalib_tva_GetRsvTypeFromSeriesSet(pstEvent->pstInstSet->pstProgSet->pstSeriesSet);
}


#define ____GLOBAL_FUNC____


HERROR	SAMALIB_TVA_Init(sama_ConflictCheck_t *pstInst)
{
	s_samaInst = pstInst;
	return ERR_OK;
}

HERROR	SAMALIB_TVA_GetDefaultAuthority (HUINT32 ulSvcUid, HCHAR *szDefaultAuthority)
{
	HERROR	hErr;
	hErr = samalib_tva_GetDefaultAuthority(ulSvcUid,szDefaultAuthority);
	return hErr;
}

HERROR	SAMALIB_TVA_FindCachedService (HUINT32 ulSvcUid)
{
	return samalib_tva_FindCachedService(ulSvcUid);
}
HERROR	SAMALIB_TVA_RemovePastEventPeriod(SERIES_SET *pstSeriesSet, UNIXTIME cur_time, HBOOL bReplaceSelectedInst)
{
	HERROR	hErr;


	hErr = samalib_tva_RemovePastEventPeriod(pstSeriesSet, cur_time, bReplaceSelectedInst);


	return hErr;
}

HERROR	SAMALIB_TVA_SetCurrentlyRecording(SERIES_SET *pstSeriesSet, HUINT32 ulSlot)
{
	PROGRAM_SET		*pstProgSet = NULL;
	INSTANCE_SET	*pstInstSet = NULL;
	EVENT_PERIOD	*pstEvent = NULL;


	if (pstSeriesSet == NULL) return ERR_FAIL;

	for (pstProgSet = pstSeriesSet->pstProgList; pstProgSet != NULL; pstProgSet=pstProgSet->pNext)
	{
		for (pstInstSet = pstProgSet->pstSelectedInst;pstInstSet != NULL; pstInstSet=pstInstSet->pNext)
		{
			for (pstEvent = pstInstSet->pstEventList;pstEvent != NULL; pstEvent=pstEvent->pNext)
			{
				if (pstEvent->ulSlot == ulSlot)
				{
					pstEvent->flag		|= SAMA_EVENT_BEING_RECORDED;
					pstInstSet->flag	|= SAMA_INSTANCE_DO_NOT_FIND_ALTERNATIVE;

					return ERR_OK;
				}
			}
		}
	}

	return ERR_FAIL;
}

HUINT8 *		SAMALIB_TVA_ConvertCridToStr(TVA_CONTENTID_T *pstCrid)
{
	HUINT8 *pStr;


	pStr = samalib_tva_ConvertCridToStr(pstCrid);


	return pStr;
}

HERROR SAMALIB_TVA_ExtractEachPartCRID(HUINT8 *cridstr, HUINT32 cridLen, TVA_CONTENTID_T *tva_crid, int *auth)
{
	HERROR hErr;

	hErr = samalib_tva_ExtractEachPartCRID(cridstr, cridLen, tva_crid, auth);

	return hErr;
}

HERROR	SAMALIB_TVA_ConstructScheduleWithSeries (DxSchedule_t *pstSchedule, EVENT_PERIOD *pstEventPeriod)
{
	HCHAR				*szCridStr;

	HERROR				hErr;
	TVA_CONTENTID_T		stProgCrid;
	DxService_t			stService;
	SERIES_SET			*pstSeriesSet;
	INSTANCE_SET		*pstInstanceSet;
	DxRSV_TvAnyTimeRecord_t *pstTvAnyTime;

	if (  pstSchedule == NULL
		|| pstEventPeriod == NULL
		|| pstEventPeriod->pstInstSet == NULL
		|| pstEventPeriod->pstInstSet->pstProgSet == NULL
		|| pstEventPeriod->pstInstSet->pstProgSet->pstSeriesSet == NULL)
	{
		HxLOG_Debug("Invalid param. null eveny period. \n");
		return ERR_FAIL;
	}

	if ( s_samaInst == NULL )
	{
		HxLOG_Error("Null inst. Maybe time is not inited. \n");
		return ERR_FAIL;
	}


	SAMA_MemSet(pstSchedule, 0x00, sizeof(DxSchedule_t));

	pstInstanceSet = pstEventPeriod->pstInstSet;
	pstSeriesSet   = pstInstanceSet->pstProgSet->pstSeriesSet;

	pstSchedule->ulSlot = pstEventPeriod->ulSlot;
	pstSchedule->eScheduleSpec = SAMALIB_SPEC_Get();
	pstSchedule->eRsvType = samalib_tva_GetRsvTypeFromSeriesSet(pstSeriesSet);
	pstSchedule->ulDuration = pstEventPeriod->endTime - pstEventPeriod->startTime;
	pstSchedule->ulSvcUid = pstEventPeriod->ulSvcUid;
	pstSchedule->eRepeat = DxRSVREPEAT_ONCE;
	pstSchedule->ulRepeatDays = 0;

	hErr = samalib_svc_GetService(s_samaInst, (pstEventPeriod->ulSvcUid & 0xFFFF), &stService);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("Can't tripple id for svcuid=(%d) \n",pstEventPeriod->ulSvcUid);
		return ERR_FAIL;
	}

	pstSchedule->stTripleId.usOnId	= (HUINT16)stService.onid;
	pstSchedule->stTripleId.usTsId	= (HUINT16)stService.tsid;
	pstSchedule->stTripleId.usSvcId = (HUINT16)stService.svcid;

	if (MACRO_IS_TVA_RECORD_TYPE(pstEventPeriod->pstInstSet->pstProgSet->pstSeriesSet->eRsvType))
	{
		pstTvAnyTime = &(pstSchedule->uExtInfo.stRec.ex.tva);
		pstSchedule->uExtInfo.stRec.nEvtId = pstEventPeriod->ulEventId;
		HxSTD_snprintf(pstSchedule->uExtInfo.stRec.szEventName,DxRSV_EVENT_NAME_LEN,"%s",pstEventPeriod->szEventName);
	}
	else if (MACRO_IS_TVA_WATCH_TYPE(pstEventPeriod->pstInstSet->pstProgSet->pstSeriesSet->eRsvType))
	{
		pstTvAnyTime = &(pstSchedule->uExtInfo.stWch.ex.tva);
		pstSchedule->uExtInfo.stWch.nEvtId = pstEventPeriod->ulEventId;
		HxSTD_snprintf(pstSchedule->uExtInfo.stWch.szEvtName,DxRSV_EVENT_NAME_LEN,"%s",pstEventPeriod->szEventName);
	}
	else
	{
		HxLOG_Debug("Invalid param. unknown rsv type (%d). \n",pstEventPeriod->pstInstSet->pstProgSet->pstSeriesSet->eRsvType);
		return ERR_FAIL;
	}

	HLIB_DATETIME_ConvertUnixTimeToDateTime(pstEventPeriod->startTime, &(pstSchedule->stStartTime));

	szCridStr = samalib_tva_ConvertCridToStr(&(pstSeriesSet->stCrid));
	pstTvAnyTime->ucCRIDType = pstSeriesSet->stCrid.type;
	++szCridStr;
	HxSTD_snprintf(pstTvAnyTime->szCRID,DxRSV_CRID_LEN+1, "%s",szCridStr);

#if defined(CONFIG_OP_TDC)
	if( pstSeriesSet->stCrid.type == DxCRID_TYPE_SERIES )
	{
		HxSTD_snprintf(pstTvAnyTime->szSeriesTitle, DxRSV_CRID_SERIES_TITLE_LEN, "%s", pstSeriesSet->szSeriesTitle);
		pstTvAnyTime->ucSearchFlag = pstSeriesSet->searchFlag;
	}
#endif

	SAMA_MemCpy(&stProgCrid, &(pstInstanceSet->pstProgSet->stProgCrid), sizeof(TVA_CONTENTID_T));

	// program set¿¡´Â imi°¡ Æ÷ÇÔµÇÁö ¾ÊÀ¸¹Ç·Î selected instance¿¡¼­ imi¸¦ °¡Á®¿Í¾ß ÇÑ´Ù.
	if ( pstInstanceSet->imi != 0 )
	{
		stProgCrid.imi = pstInstanceSet->imi;
	}
	szCridStr = samalib_tva_ConvertCridToStr(&stProgCrid);
	++szCridStr;
	HxSTD_snprintf(pstTvAnyTime->szProgCRID,DxRSV_CRID_LEN+1, "%s",szCridStr);

	if (pstSeriesSet->eHdType == eDxVIDEO_TYPE_HD)
	{
		pstTvAnyTime->ucRecKind |= SAMA_SERIES_CONTENT_KIND_HD;
	}

	if (pstEventPeriod->flag & SAMA_EVENT_SEASON_EPISODE_SETTED)
	{
		pstTvAnyTime->ulSeason = pstEventPeriod->ulSeason;
		pstTvAnyTime->ulEpisodeNum = pstEventPeriod->ulEpisode;
		pstTvAnyTime->eContentKind |= DxRsvTva_ContentKind_SeasonEpisode_Done;
	}

	if (pstEventPeriod->flag & SAMA_EVENT_BY_EIT_PF
		|| pstEventPeriod->flag & SAMA_EVENT_BY_EIT_RUNNING_STATUS )
	{
		pstSchedule->eEpgType = eDxEPG_TYPE_PF;
	}
	else if (pstEventPeriod->flag & SAMA_EVENT_BY_EIT_SC)
	{
		pstSchedule->eEpgType = eDxEPG_TYPE_SC;
	}
	else
	{
		pstSchedule->eEpgType = eDxEPG_TYPE_IP;
	}


	return ERR_OK;
}

SERIES_SET * SAMALIB_TVA_NewSeriesSet (HUINT8 *szCrid, HUINT8 ucCridType, HCHAR *szDefaultAuth)
{
	SERIES_SET	*pstSeriesSet = NULL;
	pstSeriesSet = samalib_tva_NewSeriesSet(szCrid, ucCridType, szDefaultAuth);
	return pstSeriesSet;
}

HERROR SAMALIB_TVA_DeleteSeriesSet(SERIES_SET *pstSeriesSet)
{
	HERROR hErr;
	hErr = samalib_tva_DeleteSeriesSet(pstSeriesSet);
	return hErr;
}

HxList_t * SAMALIB_TVA_GetSeriesList(void)
{
	return s_pstSama_SeriesList;
}

SERIES_SET * SAMALIB_TVA_GetSeriesSetByCRID (HUINT8 *szCrid, HUINT8 ucCridType, HCHAR *szDefaultAuth)
{
	TVA_CONTENTID_T stCrid;
	HxList_t		*pvIter;
	if ( szCrid == NULL )
	{
		HxLOG_Error("null crid\n");
		return NULL;
	}

	SAMA_MemSet(&stCrid, 0, sizeof(TVA_CONTENTID_T));

	samalib_tva_MakeCrid(szCrid, szDefaultAuth, &stCrid);
	stCrid.type = ucCridType;

	pvIter = HLIB_LIST_FindEx(s_pstSama_SeriesList,&stCrid,samalib_tva_cbCmpCRIDKey);
//	pstSeriesSet = get_series_set(&stCrid);
	if ( pvIter == NULL )
	{
		HxLOG_Debug("get series set failed. type=(%d), crid=[%s]\n",ucCridType, szCrid);
		return NULL;
	}

	return HLIB_LIST_Data(pvIter);
}

SERIES_SET * SAMALIB_TVA_GetSeriesSetBySlotId(HUINT32 ulSlot)
{
	return samalib_tva_GetSeriesSetBySlotId(ulSlot);
}

SERIES_SET * SAMALIB_TVA_GetTrackingSeriesSetBySlotId (HUINT32 ulSlot)
{
	return samalib_tva_GetTrackingSeriesSetBySlotId(ulSlot);
}
HUINT32 SAMALIB_TVA_GetContentKind(HxVector_t *vCrid)
{
	HUINT32		ulContentKind = SAMA_SERIES_CONTENT_KIND_NONE;
	HUINT32		ulNumOfCrids;
	HUINT32		i;
	DxEventCRID_t	*contentIdentifier;

	if ( vCrid == NULL )
	{
		HxLOG_Error("vCrid is NULL \n");
		return SAMA_SERIES_CONTENT_KIND_NONE;
	}

	ulNumOfCrids = HLIB_VECTOR_Length(vCrid);
	for ( i=0; i< ulNumOfCrids; i++ )
	{
		contentIdentifier = (DxEventCRID_t *)HLIB_VECTOR_ItemAt(vCrid, i);
		if (contentIdentifier == NULL)
			continue;

		// set event contentKind...
		switch (contentIdentifier->type)
		{
			case SAMA_CRID_TYPE_PROG:
				// if crid include imi
				if (strchr((char *)contentIdentifier->crid, '#') != NULL)
				{
					ulContentKind |= SAMA_SERIES_CONTENT_KIND_SPLIT;
				}
				else
				{
					ulContentKind |= SAMA_SERIES_CONTENT_KIND_SINGLE;
				}
				break;

			case SAMA_CRID_TYPE_SERIES:
				ulContentKind |= SAMA_SERIES_CONTENT_KIND_SERIES;
				break;

			case SAMA_CRID_TYPE_RECOMMEND:
				ulContentKind |= SAMA_SERIES_CONTENT_KIND_RECOMMEND;
				break;

			default:
				break;
		}
	}

	return ulContentKind;
}



HERROR SAMALIB_TVA_RemoveNonSelectedInstance(SERIES_SET *pstSeriesSet)
{
	HERROR	hErr;
	hErr = samalib_tva_RemoveNonSelectedInstance(pstSeriesSet);
	return hErr;
}

HERROR SAMALIB_TVA_RemoveMatchingEventPeriod(HUINT32 ulSvcUid, HUINT32 ulEventId, SERIES_SET *pstSeriesSet, TVA_CONTENTID_T *pstRemovedProgCrid)
{
	HERROR	hErr;
	hErr = samalib_tva_RemoveMatchingEventPeriod(ulSvcUid, ulEventId, pstSeriesSet, pstRemovedProgCrid);
	return hErr;
}

HBOOL SAMALIB_TVA_IsSeriesReservedItem(DxSchedule_t *pstScheduleItem)
{
	SERIES_SET *pstSeriesSet = NULL;

	if(pstScheduleItem == NULL)
		return FALSE;

	pstSeriesSet = samalib_tva_GetSeriesSetBySlotId(pstScheduleItem->ulSlot);
	if(MACRO_IS_TVA_SCHEDULE(pstScheduleItem,pstSeriesSet))
	{
		return TRUE;
	}

	return FALSE;
}

HBOOL SAMALIB_TVA_IsCurrentlyRecording (SERIES_SET *pstSeriesSet)
{
	PROGRAM_SET		*pstProgSet = NULL;
	INSTANCE_SET	*pstInstSet = NULL;
	EVENT_PERIOD	*pstEventPeriod = NULL;

	if (pstSeriesSet == NULL)
		return TRUE;

	for (pstProgSet = pstSeriesSet->pstProgList; pstProgSet != NULL; pstProgSet=pstProgSet->pNext)
	{
		for (pstInstSet = pstProgSet->pstSelectedInst; pstInstSet != NULL; pstInstSet = pstInstSet->pNext)
		{
			for (pstEventPeriod = pstInstSet->pstEventList; pstEventPeriod != NULL; pstEventPeriod = pstEventPeriod->pNext)
			{
				if (pstEventPeriod->flag & SAMA_EVENT_BEING_RECORDED)
				{
					HxLOG_Debug(HxANSI_COLOR_GREEN("Currently recording event. slot=(%d) \n"),pstEventPeriod->ulSlot);
					samalib_tva_PrintEventPeriod(FALSE, pstEventPeriod);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}


/***************************************************************************************************
*       ÁÖÀÇ !!!!
*    Dapi callback¿¡¼­´Â ÀÌ ÇÔ¼ö¸¦ ºÎ¸£¸é ¾ÈµÈ´Ù !
*    ¿Ö³ÄÇÏ¸é makecrid¸¦ È£ÃâÇÏ±â À§ÇØ¼­ dapi get DxService_t °¡ ÇÊ¿äÇÏ±â ¶§¹®ÀÌ´Ù.
*    ¹Ýµå½Ã »ç¿ëÇØ¾ß ÇÑ´Ù¸é, service cache°¡ ÀÖ´Â°æ¿ì¿¡¸¸ »ç¿ëÇÏµµ·Ï ÇÑ´Ù.
****************************************************************************************************/

HERROR SAMALIB_TVA_AddDxEventToSeriesSet(DxEvent_t *event, SERIES_SET *pstSeriesSet, UNIXTIME ulTimeAfter, HBOOL bInitial, HUINT32 ulSlot)
{
	HERROR	hErr;

	hErr = samalib_tva_AddDxEventToSeriesSet(event, pstSeriesSet, ulTimeAfter, bInitial, ulSlot);

	return hErr;
}
HERROR SAMALIB_TVA_AddDxEventsToSeriesSet (HxList_t *pstSeriesEventsList, SERIES_SET *pstSeriesSet)
{
	HERROR	hErr;

	hErr = samalib_tva_AddDxEventsToSeriesSet(pstSeriesEventsList, pstSeriesSet);

	return hErr;
}

HERROR SAMALIB_TVA_AddRecordedCridToSeriesSet (SERIES_SET *pstSeriesSet, TVA_CONTENTID_T *pstProgCrid)
{
	HUINT32 	i;
	HUINT32 num_recorded_crids = 0;
	TVA_CONTENTID_T	*pstRecordedCrid = NULL;

	if ( pstSeriesSet == NULL
		|| pstProgCrid == NULL )
	{
		HxLOG_Error("Invalid null param. seriesSet=(0x%x), progCrid=(0x%x)\n",pstSeriesSet, pstProgCrid);
		return ERR_FAIL;
	}

	if (pstSeriesSet->pstRecordedProgCrids == NULL)
	{
		HxLOG_Error("invalid context. \n");
		return ERR_FAIL;
	}

	num_recorded_crids = HLIB_VECTOR_Length(pstSeriesSet->pstRecordedProgCrids);
	for (i = 0; i < num_recorded_crids; i++)
	{
		pstRecordedCrid = HLIB_VECTOR_ItemAt(pstSeriesSet->pstRecordedProgCrids, i );
		if (pstRecordedCrid == NULL)
		{
			HxLOG_Debug("Null crid. \n");
			continue;
		}

		if (pstRecordedCrid->type == SAMA_CRID_TYPE_PROG
			&& crid_cmp(pstRecordedCrid->data, pstProgCrid->data) == 0
			&& crid_cmp(pstRecordedCrid->auth, pstProgCrid->auth) == 0)
		{
			HxLOG_Debug("Already added crid. crid=[%s]\n", samalib_tva_ConvertCridToStr(pstProgCrid));
			return ERR_OK;
		}
		}

	pstRecordedCrid = (TVA_CONTENTID_T *)SAMA_Calloc(sizeof(TVA_CONTENTID_T));
	if (pstRecordedCrid == NULL)
	{
		HxLOG_Error("malloc failed. \n");
		return ERR_FAIL;
	}

			HxLOG_Debug("Add recorded crid=[%s], pstSeriesSet=(0x%x)\n",samalib_tva_ConvertCridToStr(pstProgCrid), pstSeriesSet);
	SAMA_MemCpy(pstRecordedCrid, pstProgCrid, sizeof(TVA_CONTENTID_T));

	HLIB_VECTOR_Add(pstSeriesSet->pstRecordedProgCrids, pstRecordedCrid);

	return ERR_OK;
}

HERROR				SAMALIB_TVA_UpdateEventTimeBySlot(HUINT32 ulSlot, HUINT32 ulStartTime, HUINT32 ulEndTime)
{
	EVENT_PERIOD *pstEventPeriod = NULL;

	pstEventPeriod = samalib_tva_GetEventPeriodBySlotId(ulSlot);
	if (pstEventPeriod == NULL)
	{
		HxLOG_Debug(HxANSI_COLOR_GREEN("Can't Find Event. slot=(%d)"),ulSlot);
		return ERR_FAIL;
	}

	pstEventPeriod->startTime = ulStartTime;
	pstEventPeriod->endTime = ulEndTime;
	pstEventPeriod->flag |= SAMA_EVENT_BY_EIT_RUNNING_STATUS;

	return ERR_OK;

}

SAMA_ForEachResult_e SAMALIB_TVA_UpdateSeriesSetFromDbToRsv(DxSchedule_t *pDbItem, HINT32 nArgc, HUINT32 *pulArgv)
{
	SERIES_SET			*pstSeriesSet = NULL;
	TVA_CONTENTID_T		stProgCrid;	// Programme CRID

	HINT32				nEvtId			= 0;
	HCHAR				*szProgCridStr	= NULL;
	HCHAR				*szCridStr		= NULL;
	HUINT8				ucCridType;
	HUINT8				ucRecKind;
	DxSchedule_Spec_e	 eSpec			= SAMALIB_SPEC_Get();

	UNIXTIME			utSchStartTime	= 0;
	UNIXTIME			utSchEndTime	= 0;
	HERROR				hErr;
	HUINT32				ulEventSource	= 0;

	DxRSV_TvAnyTimeRecord_t	*pstTvAnyTime = NULL;
	/* Series SetÀº Ã³À½ ºÎÆÃ½Ã 1¹ø¸¸ DB¿¡¼­ ÀÐ¾î¿Í ¸Þ¸ð¸®¿¡ ÀûÀçÇØÁÖ°í, ±× ÀÌÈÄ¿¡ »ç¿ëÀº Pointer¸¸ ¸Å´Þ¾Æ ÁÖ¸é µÈ´Ù
	MW_RSV_ChangeReservedItemÇÔ¼ö µî¿¡¼­ ¹Ù·Î SaveÇÏ´Â °ÍÀÌ ¾Æ´Ï¶ó Read ÈÄ Save¸¦ ÇÏ±â ¶§¹®¿¡
	Save¸¦ ÇÏ·Á ÇÏ¿´À» ¶§ DB¿¡¼­ ÀÐ¾î¿Í SaveÇÏ·Á´Â SeriesSetÀ» º¯°æÇÏ´Â µî ¿©·¯°¡Áö ¿¹¿Ü »óÈ²ÀÌ ¹ß»ýÇÒ ¼ö ÀÖ´Ù.
	if(pRsvItem->ext.stRec.pstSeriesSet == NULL)
		return;
	*/

	(void)nArgc;
	(void)pulArgv;

	if ( pDbItem == NULL )
	{
		HxLOG_Error("null db item\n");
		return eSAMA_Result_FAIL;
	}

	if (!MACRO_IS_TVA_SCHEDULE_TYPE(pDbItem->eRsvType))
	{
		HxLOG_Debug("Is not tva schedule. slot=(%d), type=(%d)[%s] \n", pDbItem->ulSlot,pDbItem->eRsvType,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), pDbItem->eRsvType));
		return eSAMA_Result_FAIL;
	}

	switch ( eSpec )
	{
		case eDxSchedule_Spec_TvAnyTime:
			{
				if (MACRO_IS_TVA_RECORD_TYPE(pDbItem->eRsvType))
				{
					pstTvAnyTime = &(pDbItem->uExtInfo.stRec.ex.tva);
				}
				else if (MACRO_IS_TVA_WATCH_TYPE(pDbItem->eRsvType))
				{
					pstTvAnyTime = &(pDbItem->uExtInfo.stWch.ex.tva);
				}
				else
				{
					HxLOG_Error("not supported type. type=(%d)\n", pDbItem->eRsvType);
					return eSAMA_Result_FAIL;
				}

				szProgCridStr = pstTvAnyTime->szProgCRID;
				szCridStr	  = pstTvAnyTime->szCRID;
				ucCridType	  = pstTvAnyTime->ucCRIDType;
				ucRecKind	  = pstTvAnyTime->ucRecKind;
				break;
			}
		default:
			HxLOG_Error("not supported spec. spec=(%d)\n", eSpec);
			return eSAMA_Result_FAIL;
	}

	if (samalib_tva_IsImiExists(szProgCridStr)
		|| ucCridType == SAMA_CRID_TYPE_SERIES)
	{
		pstSeriesSet = SAMALIB_TVA_GetSeriesSetByCRID(szCridStr,ucCridType, NULL);
	}

	if ( pstSeriesSet == NULL )
	{
		HxLOG_Debug("make new series set. slot=(%d) \n", pDbItem->ulSlot);
		pstSeriesSet = samalib_tva_NewSeriesSet(szCridStr
												,ucCridType
												,NULL);
		if ( pstSeriesSet == NULL )
		{
			HxLOG_Debug("get series set failed. CRID=(%s) slot=(%d)\n", szCridStr, pDbItem->ulSlot);
			return eSAMA_Result_FAIL;
		}

		pstSeriesSet->eHdType = (ucRecKind & SAMA_SERIES_CONTENT_KIND_HD)?eDxVIDEO_TYPE_HD: eDxVIDEO_TYPE_SD;
		pstSeriesSet->eRsvType = pDbItem->eRsvType;
#if defined(CONFIG_OP_TDC)
		pstSeriesSet->searchFlag = pstTvAnyTime->ucSearchFlag;
		pstSeriesSet->ulSvcUid = pDbItem->ulSvcUid;
		if( pstTvAnyTime->ucCRIDType == DxCRID_TYPE_SERIES && !HxSTD_StrEmpty(pstTvAnyTime->szSeriesTitle))
		{
			HxSTD_snprintf(pstSeriesSet->szSeriesTitle, DxRSV_CRID_SERIES_TITLE_LEN, "%s", pstTvAnyTime->szSeriesTitle);
		}
#endif
	}

	if ( !MACRO_IS_TVA_SCHEDULE(pDbItem, pstSeriesSet))
	{
		HxLOG_Debug("not a tva schedule. rsv type=(%d)[%s], crid type=(%d)\n",pDbItem->eRsvType,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), pDbItem->eRsvType), pstSeriesSet->stCrid.type);
		samalib_tva_DeleteSeriesSet(pstSeriesSet);
		return eSAMA_Result_FAIL;
	}

	switch ( pDbItem->eRsvType )
	{
		case DxRSVTYPE_WATCHING_EBR:
		case DxRSVTYPE_WATCHING_TBR:
		case DxRSVTYPE_WATCHING_SERIES_EBR:
			nEvtId = pDbItem->uExtInfo.stWch.nEvtId;
			break;
		case DxRSVTYPE_RECORD_EBR:
		case DxRSVTYPE_RECORD_TBR:
		case DxRSVTYPE_RECORD_SERIES_EBR:
			nEvtId = pDbItem->uExtInfo.stRec.nEvtId;
			break;
		case DxRSVTYPE_RECORD_INSTANT:
		case DxRSVTYPE_RECORD_DELAYED:
		default :
			break;
	}

	// trackingÀ» À§ÇØ ¸¶Áö¸·À¸·Î ³²°Ü ³õÀº schedule ÀÎ °æ¿ì¿¡ addevent¸¦ »ý·«ÇØÁØ´Ù.
	if ( pDbItem->stRemovedInfo.eRemovedReason == DxSchedule_RemovedReason_SeriesTracking)
	{
		HxLOG_Debug("seriestracking schedule. slot=(%d)\n",pDbItem->ulSlot);
		pstSeriesSet->ulSeriesTrackingSlot = pDbItem->ulSlot;
		return eSAMA_Result_OK;
	}

	HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pDbItem->stStartTime), &utSchStartTime);
	utSchEndTime = utSchStartTime + pDbItem->ulDuration;

	SAMA_MemSet(&stProgCrid,0,sizeof(TVA_CONTENTID_T));
	samalib_tva_MakeCrid(szProgCridStr, NULL, &stProgCrid);
	// if programme crid exists
	if (HLIB_STD_StrNLen(szProgCridStr,DxRSV_CRID_LEN) > 0 )
	{
		stProgCrid.type = SAMA_CRID_TYPE_PROG;
	}

	if (pDbItem->eEpgType == eDxEPG_TYPE_PF)
	{
		ulEventSource = SAMA_EVENT_BY_EIT_PF;
	}
	else if (pDbItem->eEpgType == eDxEPG_TYPE_SC)
	{
		ulEventSource = SAMA_EVENT_BY_EIT_SC;
	}
	else
	{
		ulEventSource = SAMA_EVENT_BY_IP;
	}

	hErr = samalib_tva_AddEventToSeriesSet(pstSeriesSet, pDbItem->ulSvcUid, &stProgCrid, nEvtId, utSchStartTime, utSchEndTime, ulEventSource, pDbItem->ulSlot, pDbItem->uExtInfo.stRec.szEventName);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("samalib_tva_AddEventToSeriesSet failed. slot=(%d)\n",pDbItem->ulSlot);

		// series set¿¡ ³²¾Æ ÀÖ´Â event°¡ ¾Æ¹« °Íµµ ¾øÀ» °æ¿ì¿¡´Â series setÀ» Á¦°ÅÇØ ÁØ´Ù.
		if (pstSeriesSet->pstProgList->pstSelectedInst->pstEventList == NULL)
		{
			samalib_tva_DeleteSeriesSet(pstSeriesSet);
		}
	}

	return eSAMA_Result_OK;
}





#define ________LOCAL_CONFLICT_FUNC_______

STATIC HERROR samalib_tva_AdjustTime(DxRsvRepeat_e eMode, UNIXTIME sysTime, UNIXTIME *utTime)
{
#if 1
	HERROR		hErr;
	UNIXTIME	stTime, nextTime, timeGap;
	HBOOL		isWorkday = FALSE;

	if ( (utTime == NULL) || (*utTime == 0) )
	{
		return ERR_FAIL;
	}

	stTime = *utTime;

	switch (eMode)
	{
	case DxRSVREPEAT_DAILY :
		while ( stTime <= sysTime )
		{
			hErr = HLIB_DATETIME_GetNextDay(stTime, &nextTime, &timeGap);
			if ( hErr != ERR_OK )
			{
				return ERR_FAIL;
			}
			stTime = nextTime;
		}
		break;

	case DxRSVREPEAT_WEEKLY :
		while ( stTime <= sysTime )
		{
			hErr = HLIB_DATETIME_GetNextWeek(stTime, &nextTime, &timeGap);
			if ( hErr != ERR_OK )
			{
				return ERR_FAIL;
			}
			stTime = nextTime;
		}
		break;

	case DxRSVREPEAT_WEEKDAYS :
		isWorkday = HLIB_DATETIME_IsWorkDay(stTime, stTime);
		if (isWorkday == FALSE)
		{
			hErr = HLIB_DATETIME_GetNextWorkDay(stTime, &nextTime, &timeGap);
			if ( hErr != ERR_OK )
			{
				return ERR_FAIL;
			}

			stTime = nextTime;
		}

		while ( stTime <= sysTime )
		{
			hErr = HLIB_DATETIME_GetNextWorkDay(stTime, &nextTime, &timeGap);
			if ( hErr != ERR_OK )
			{
				return ERR_FAIL;
			}
			stTime = nextTime;
		}
		break;

	case DxRSVREPEAT_WITHOUT_SUNDAY :
		while ( stTime <= sysTime )
		{
			hErr = HLIB_DATETIME_GetNextWithoutSunday(stTime, &nextTime, &timeGap);
			if ( hErr != ERR_OK )
			{
				return ERR_FAIL;
			}
			stTime = nextTime;
		}
		break;

	case DxRSVREPEAT_WEEKEND :
		isWorkday = HLIB_DATETIME_IsWorkDay(stTime, stTime);
		if (isWorkday == TRUE)
		{
			hErr = HLIB_DATETIME_GetNextWeekend(stTime, &nextTime, &timeGap);
			if ( hErr != ERR_OK )
			{
				return ERR_FAIL;
			}

			stTime = nextTime;
		}

		while ( stTime <= sysTime )
		{
			hErr = HLIB_DATETIME_GetNextWeekend(stTime, &nextTime, &timeGap);
			if ( hErr != ERR_OK )
			{
				return ERR_FAIL;
			}
			stTime = nextTime;
		}
		break;

	default :
		break;
	}

	*utTime = stTime;
#endif
	return ERR_OK;
}


STATIC HERROR samalib_tva_MakeSeriesSetForPeriodicTBR(HUINT32 ulSvcUid, SERIES_SET *pstSeriesSet, DxRsvRepeat_e eRsvRepeat,
																UNIXTIME startTime, UNIXTIME duration, UNIXTIME curTime, HUINT32 ulSlot)
{
	UNIXTIME ulStartTime;
	UNIXTIME start_time_future;
	UNIXTIME ulChkEndDate = startTime + SECONDS_PER_DAY * 14;	/* Repeat ModeÀÏ °æ¿ì Current TimeºÎÅÍ +14ÀÏ ±îÁö µî·ÏÇÏ¿© Conflict Check */

	start_time_future = ulStartTime = startTime;
	do
	{
		if(curTime < start_time_future)
			samalib_tva_AddPeriodToSeriesSet(ulSvcUid, 0, start_time_future, start_time_future + duration, pstSeriesSet, ulSlot);

		samalib_tva_AdjustTime(eRsvRepeat, ulStartTime, &start_time_future);
		ulStartTime = start_time_future;
	}
	while(ulStartTime < ulChkEndDate);

	return ERR_OK;
}

STATIC HERROR samalib_tva_SetCandidateInstAsSelectedInst(SERIES_SET *pstSeriesSet)
{
	PROGRAM_SET *cur_prog;

	for (cur_prog = pstSeriesSet->pstProgList; cur_prog; cur_prog = cur_prog->pNext)
	{
		cur_prog->pstCandidateInst = cur_prog->pstSelectedInst;
		cur_prog->flag = 0;
	}

	return ERR_OK;
}

STATIC HERROR samalib_tva_SetSelectedInstAsCandidateInst(SERIES_SET *pstSeriesSet)
{
	PROGRAM_SET *cur_prog;

	for (cur_prog = pstSeriesSet->pstProgList; cur_prog; cur_prog = cur_prog->pNext)
	{
		cur_prog->pstSelectedInst = cur_prog->pstCandidateInst;
	}

	return ERR_OK;
}


STATIC void samalib_tva_CorrectSeriesSetPtr(SERIES_SET *pstSeries)
{
	SERIES_SET *pstSeriesSet;
	PROGRAM_SET *pProgSet;

	pstSeriesSet = pstSeries;
	for (pProgSet = pstSeriesSet->pstProgList; pProgSet; pProgSet = pProgSet->pNext)
	{
		pProgSet->pstSeriesSet = pstSeriesSet;
	}
}

STATIC INLINE HBOOL samalib_tva_IsOverlap(UNIXTIME startTime, UNIXTIME endTime, EVENT_PERIOD *ev)
{
	if (startTime < ev->endTime && ev->startTime < endTime)
		return TRUE;
	return FALSE;
}

STATIC HINT32 samalib_tva_GetOverlappedList (EVENT_PERIOD *src_event,
													OVERLAPPED_EVENT overlapped[SAMA_TVA_MAX_SLOT])
{
	PROGRAM_SET	*target_prog;
	EVENT_PERIOD	*target_event;
	HINT32			num_overlap;
	HxList_t		*pvIter;

	/* index 0Àº source */
	overlapped[0].pEventPeriod = src_event;
	overlapped[0].slot = src_event->ulSlot;
	num_overlap = 1;

	for (pvIter = HLIB_LIST_First(s_pstSama_SeriesList); NULL != pvIter; pvIter = HLIB_LIST_Next(pvIter))
	{
		SERIES_SET		*pstSeriesSet = (SERIES_SET *)HLIB_LIST_Data(pvIter);
		// TODO: legacy¿¡ ÀÇÇÑ Ãß°¡ »çÇ× - not conflict type || except conflict || running schedule ÀÎ °æ¿ì¿¡ Á¦¿Ü ÇØ¾ß ÇÑ´Ù
		for (target_prog = pstSeriesSet->pstProgList; target_prog; target_prog = target_prog->pNext)
		{
			for (target_event = target_prog->pstCandidateInst->pstEventList; target_event; target_event = target_event->pNext)
			{
				if (target_event == src_event)
					continue;

				if (samalib_tva_IsOverlap(src_event->startTime, src_event->endTime, target_event))
				{
					overlapped[num_overlap].pEventPeriod = target_event;
					overlapped[num_overlap].slot = target_event->ulSlot;
					num_overlap++;
				}
			}
		}
	}

#if defined(CONFIG_DEBUG)
{
	HUINT32			i;

	for (i = 0; i < num_overlap; i++)
	{
			HxLOG_Debug("--  overlapped[%d][slot=%d]", i,overlapped[i].slot);
			samalib_tva_PrintEventPeriod(FALSE, overlapped[i].pEventPeriod);
	}
}
#endif

	return num_overlap;
}

STATIC HUINT32	samalib_tva_AssignSlot(HUINT32 ulOrgSlot, HUINT32 i)
{
	if (ulOrgSlot != DxRSV_SLOT_NULL)
	{
		return ulOrgSlot;
	}
	else
	{
		// conflict check¸¦ À§ÇØ ÀÓ½Ã·Î i°ª¿¡ µû¶ó uniqueÇÏ°Ô slotÀ» ¸¸µé¾î ÁØ´Ù.
		return DxRSV_SLOT_NULL -i -1;
	}
}

STATIC void samalib_tva_SetConfInfo(
		HINT32 num_conflit,
		CONFLICT_LIST conflict_list[],
		SAPI_ConflictType_e eResultConfType,
		SAPI_Conflict_t *pConfInfo)
{
	HINT32 					i;
	SAPI_ConflictItem_t 	stConflictItem;

	if (pConfInfo == NULL)
	{
		HxLOG_Debug("Null conf info. Doesn't need conf info\n");
		return;
	}

	SAMALIB_CFLT_ClearConflictResult(pConfInfo);

	// conflict_list[0] ÀÇ °ªÀº ¿¹¾àÀ» ½ÃµµÇÑ scheduleÀÌ´Ù.
	HxLOG_Debug("Set Conflict Info. Num of conf=(%d)\n",num_conflit-1);

	// conflict¶ó°í ¿Ã·Á¾ß ÇÏ´Â scheduleÀº ¿¹¾àÀ» ½ÃµµÇÑ scheduleÀ» Æ÷ÇÔÇÏÁö ¾ÊÀ¸¹Ç·Î 1ºÎÅÍ ½ÃÀÛÇÑ´Ù.
	pConfInfo->eType = eResultConfType;
	for (i = 1; i < num_conflit; i++)
	{
		stConflictItem.ulSlot = conflict_list[i].slot;
		(void)SAMALIB_CFLT_AppendConflictResult(pConfInfo, &stConflictItem);
	}
}

STATIC OVERLAPPED_EVENT *	samalib_tva_GetOverlappedEvent (HUINT32 ulSlot, HINT32 num_overlap, OVERLAPPED_EVENT overlapped[])
{
	HUINT32 i =0;
	for (i=0;i<num_overlap; i++)
	{
		if (overlapped[i].slot == ulSlot)
		{
			return &(overlapped[i]);
		}
	}
	return NULL;
}

STATIC HERROR				samalib_tva_MakeConflictList_v2(HINT32 num_overlap, OVERLAPPED_EVENT overlapped[], HINT32 *pNum_conflict, CONFLICT_LIST conflict_list[])
{
	HUINT32 i;
	HERROR				hErr = ERR_OK;
	OVERLAPPED_EVENT	*pstOverlappedEvent;
	SAPI_ConflictItem_t	*pstConflictList;
	pstConflictList = s_samaInst->stConflictResult.pstConflictArray;

	*pNum_conflict = s_samaInst->stConflictResult.ulNumConflict;

	for (i=0; (i<*pNum_conflict)&&(i+1<MAX_CONFLICT_SLOT_NUM); i++)
	{
		pstOverlappedEvent = samalib_tva_GetOverlappedEvent(pstConflictList[i].ulSlot, num_overlap, overlapped);
		if (pstOverlappedEvent == NULL)
		{
			HxLOG_Error("samalib_tva_GetOverlappedEvent failed. conflict slot=(%d) \n", pstConflictList[i].ulSlot);
			hErr = ERR_FAIL;
			continue;
		}
		{
			HxLOG_Debug("-- conf [%d](c=%d)(o=%d)", i,pstConflictList[i].ulSlot, pstOverlappedEvent->pEventPeriod->ulSlot);
			samalib_tva_PrintEventPeriod(FALSE, pstOverlappedEvent->pEventPeriod);
		}
		/* i+1ÀÇ ÀÇ¹Ì: index 0 ¿¡´Â source event°¡ µé¾î°¡±â ¶§¹®ÀÌ´Ù..*/
		conflict_list[i+1].slot = pstOverlappedEvent->pEventPeriod->ulSlot;
		conflict_list[i+1].pEventPeriod = pstOverlappedEvent->pEventPeriod;
		conflict_list[i+1].pProgSet = pstOverlappedEvent->pEventPeriod->pstInstSet->pstProgSet;
	}

	// +1ÀÇ ÀÇ¹Ì:
	// SAMALIB_CFLTÀÇ conflict´Â src(=tried)¸¦ ¹«½ÃÇÏÁö¸¸
	// SAMALIB_TVAÀÇ confilct´Â src¸¦ index=0¿¡ ³Ö±â ¶§¹®ÀÌ´Ù.
	*pNum_conflict += 1;
	if (*pNum_conflict > MAX_CONFLICT_SLOT_NUM )
		*pNum_conflict = MAX_CONFLICT_SLOT_NUM;

	return hErr;
}
STATIC SAPI_ConflictType_e	samalib_tva_GetOverlappedListConflict (HINT32 num_overlap, OVERLAPPED_EVENT overlapped[])
{
	// overlapped index 0 Àº tried schedule (= source schedule )ÀÌ´Ù.

	SAPI_ConflictType_e 	eConflictType;
	DxSchedule_t			stNewSched;
	HUINT32 				i;

	SAMA_MemSet(&stNewSched, 0x00, sizeof(DxSchedule_t));

	// dummy schedule for conflict check
	{
		SERIES_SET *pstSeriesSet;
		pstSeriesSet = overlapped[0].pEventPeriod->pstInstSet->pstProgSet->pstSeriesSet;
		stNewSched.eRsvType = samalib_tva_GetRsvTypeFromSeriesSet(pstSeriesSet);
	}

	stNewSched.ulSlot = samalib_tva_AssignSlot(overlapped[0].slot, 0); // CFLT¿¡¼­ ¿À·ù¸¦ ÇÇÇÏ±â À§ÇØ uniqueÇÑ slot ºÎ¿©
	overlapped[0].slot = stNewSched.ulSlot;							   // CFLT¿¡¼­ ¾òÀº slotÀ¸·Î OVERLAPPED EVENT¸¦ ¾ò¾î¾ß ÇÏ¹Ç·Î, ÀÌ °ªÀ» ´Ù½Ã OVERLAPPED¿¡ ºÎ¿©
	HLIB_DATETIME_ConvertUnixTimeToDateTime(overlapped[0].pEventPeriod->startTime,&stNewSched.stStartTime);
	stNewSched.ulDuration	= overlapped[0].pEventPeriod->endTime - overlapped[0].pEventPeriod->startTime;
	stNewSched.ulSvcUid 	= overlapped[0].pEventPeriod->ulSvcUid;
	stNewSched.uExtInfo.stRec.nEvtId	=	overlapped[0].pEventPeriod->ulEventId;

	if (NULL != s_samaInst->pstScheduleList)
	{
		s_samaInst->pstScheduleList = HLIB_LIST_RemoveAllFunc(s_samaInst->pstScheduleList, SAMA_Free_CB);
	}
	for ( i=1; i<num_overlap;i++)
	{
		DxSchedule_t			*pstDstSchedule = (DxSchedule_t *)SAMA_Malloc (sizeof(DxSchedule_t));
		SAMA_MemSet(pstDstSchedule, 0x00, sizeof(DxSchedule_t));
		if (NULL != pstDstSchedule)
		{
			pstDstSchedule->eRsvType	= samalib_tva_GetRsvTypeFromEventPeriod(overlapped[i].pEventPeriod);
			// TODO: conflict check ·çÆ¾¿¡ ÇØ´çÇÏ´Â slotÀÌ ¿©·¯°³°¡ NULLÀÏ °æ¿ì ¹®Á¦°¡ ¾ø´ÂÁö È®ÀÎÇÑ´Ù. ÀÖ´Ù¸é ¼öÁ¤ÇØ¾ß ÇÑ´Ù.
			pstDstSchedule->ulSlot		= samalib_tva_AssignSlot(overlapped[i].slot,i);
			overlapped[i].slot			= pstDstSchedule->ulSlot;
			HLIB_DATETIME_ConvertUnixTimeToDateTime(overlapped[i].pEventPeriod->startTime,&(pstDstSchedule->stStartTime));
			pstDstSchedule->ulDuration	= overlapped[i].pEventPeriod->endTime - overlapped[i].pEventPeriod->startTime;
			pstDstSchedule->ulSvcUid		= overlapped[i].pEventPeriod->ulSvcUid;
			pstDstSchedule->uExtInfo.stRec.nEvtId	=	overlapped[i].pEventPeriod->ulEventId;
			s_samaInst->pstScheduleList = HLIB_LIST_Append (s_samaInst->pstScheduleList, (void *)pstDstSchedule);
		}
	}

	{
		HUINT32 i;
		for ( i=0;i<num_overlap;i++)
		{
			HxLOG_Debug("-- overlap[%d](s=%d)", i,overlapped[i].slot);
			samalib_tva_PrintEventPeriod(FALSE, overlapped[i].pEventPeriod);
		}
	}

	HxLOG_Debug("Number of Conflict check schedules =(%d)\n",HLIB_LIST_Length(s_samaInst->pstScheduleList));
	HxLOG_Debug("Number of overlapped schedules =(%d)\n",num_overlap);
	(void)SAMALIB_CFLT_CheckConflictWithSchedule(s_samaInst, &stNewSched);
	eConflictType = s_samaInst->stConflictResult.eType;
	HxLOG_Debug("\033[30;47m[%s:%d] got eConflictType (%d) \033[0m \n",__FUNCTION__, __LINE__,eConflictType);
	HxLOG_Debug("\033[30;47m[%s:%d] got eConflictType (%d) \033[0m \n",__FUNCTION__, __LINE__,eConflictType);
	HxLOG_Debug("\033[30;47m[%s:%d] got eConflictType (%d) \033[0m \n",__FUNCTION__, __LINE__,eConflictType);

	return eConflictType;
}

/*
 * Heart of getting overlapped events
 */
STATIC SAPI_ConflictType_e samalib_tva_GetConflictList(EVENT_PERIOD *src_event, HINT32 *pNum_conflict, CONFLICT_LIST conflict_list[], SAPI_Conflict_t *pConfInfo)
{
	static OVERLAPPED_EVENT overlapped[SAMA_TVA_MAX_SLOT];
	HINT32	num_overlap = 0;
	SAPI_ConflictType_e	result;

	*pNum_conflict = 0;

	/* find overlapped event */
	num_overlap = samalib_tva_GetOverlappedList(src_event, overlapped);

	conflict_list[0].slot = overlapped[0].slot;
	conflict_list[0].pEventPeriod = overlapped[0].pEventPeriod;
	conflict_list[0].pProgSet = overlapped[0].pEventPeriod->pstInstSet->pstProgSet;

	HxLOG_Debug("\033[30;47m[%s:%d] check conflict \033[0m \n",__FUNCTION__, __LINE__);
	result = samalib_tva_GetOverlappedListConflict(num_overlap, overlapped);
	if (result != eSAPI_CONFLICT_TYPE_None)
	{
		// TVA¸¦ À§ÇÑ CONFLICT_LIST¸¦ ÀÛ¼ºÇØÁØ´Ù.
		samalib_tva_MakeConflictList_v2(num_overlap, overlapped, pNum_conflict, conflict_list);

		// CFLT¸¦ À§ÇÑ Sapi_conflict_T¸¦ ÀÛ¼ºÇØÁØ´Ù.
		samalib_tva_SetConfInfo(*pNum_conflict, conflict_list, result, pConfInfo);
		return result;
	}

	return eSAPI_CONFLICT_TYPE_None;
}

STATIC void samalib_tva_SetAltInfoResult(HERROR result, TVA_ALTERNATIVE_INFO *pAltInfo)
{
	if (pAltInfo)
		pAltInfo->result = result;
}

STATIC void samalib_tva_AddToAltInfo(
		HINT32 slot,
		INSTANCE_SET *original,
		INSTANCE_SET *alternative,
		TVA_ALTERNATIVE_INFO *pAltInfo)
{
	EVENT_PERIOD	*pEvent;
	HINT32			i;

	if (pAltInfo && pAltInfo->numAlternative < SAMA_MAX_ALTERNATIVE_INSTANCE)
	{
		i = pAltInfo->numAlternative;
		pAltInfo->instance[i].slot = slot;

		/* set original */
		pAltInfo->instance[i].numEvent = 0;
		for (pEvent = original->pstEventList; pEvent; pEvent = pEvent->pNext)
		{
			pAltInfo->instance[i].svcHandle[pAltInfo->instance[i].numEvent] = pEvent->ulSvcUid;
			pAltInfo->instance[i].ulEventId[pAltInfo->instance[i].numEvent] = pEvent->ulEventId;
			pAltInfo->instance[i].numEvent++;

			if (pAltInfo->instance[i].numEvent >= SAMA_MAX_SPLIT_EVENT_IN_ALT_INFO)
				break;
		}

		/* set alternative */
		pAltInfo->instance[i].numAltEvent = 0;
		for (pEvent = alternative->pstEventList; pEvent; pEvent = pEvent->pNext)
		{
			pAltInfo->instance[i].altSvcHandle[pAltInfo->instance[i].numAltEvent] = pEvent->ulSvcUid;
			pAltInfo->instance[i].ulAltEventId[pAltInfo->instance[i].numAltEvent] = pEvent->ulEventId;
			pAltInfo->instance[i].numAltEvent++;

			if (pAltInfo->instance[i].numAltEvent >= SAMA_MAX_SPLIT_EVENT_IN_ALT_INFO)
				break;
		}

		pAltInfo->numAlternative++;
	}
}


STATIC void samalib_tva_InitAltInfoList(TVA_ALTERNATIVE_INFO *pAltInfo)
{
	HINT32			i, j;

	if(pAltInfo == NULL)
		return;

	SAMA_MemSet(pAltInfo, 0, sizeof(*pAltInfo));

	for (i = 0; i < (int)(sizeof(pAltInfo->instance) / sizeof(pAltInfo->instance[0])); i++)
	{
		for (j = 0; j < SAMA_MAX_SPLIT_EVENT_IN_ALT_INFO; j++)
		{
			pAltInfo->instance[i].svcHandle[j] = HANDLE_NULL;
			pAltInfo->instance[i].altSvcHandle[j] = HANDLE_NULL;
		}
	}
}


STATIC HERROR samalib_tva_DeleteSeriesSetOfNonTVASchedule(void)
{
	HxList_t	*pvIter;
	HERROR	hErr = ERR_OK;


	for (pvIter = HLIB_LIST_First(s_pstSama_SeriesList); NULL != pvIter; )
	{
		SERIES_SET		*pstSeriesSet = (SERIES_SET *)HLIB_LIST_Data(pvIter);

		pvIter = HLIB_LIST_Next(pvIter);

		if (!MACRO_IS_TVA_SERIES_SET(pstSeriesSet))
		{
			HxLOG_Debug("Delete Series Set. pstSeriesSet= 0x%x",pstSeriesSet);
			hErr |= samalib_tva_DeleteSeriesSet(pstSeriesSet);
		}
	}

	return hErr;
}

STATIC HERROR samalib_tva_ConcludeConflictChecking (SERIES_SET *pstTriedSeriesSet, HBOOL cancel)
{
	HERROR		hErr = ERR_OK;
	HxList_t	*pvIter;

	// ¿¹¾à ½ÃµµÇÑ scheduleÀÌ conflict¿´À» °æ¿ì cancel ÇÑ´Ù.
	if (cancel && pstTriedSeriesSet)
	{
		/* release series_set */
		HxLOG_Debug("Cancel Series Set by flag. pstSeriesSet= 0x%x\n", pstTriedSeriesSet);
		hErr = samalib_tva_DeleteSeriesSet(pstTriedSeriesSet);
		if (hErr != ERR_OK)
		{
			HxLOG_Debug("samalib_tva_DeleteSeriesSet failed. \n");
			return ERR_FAIL;
		}
	}

	for (pvIter = HLIB_LIST_First(s_pstSama_SeriesList); NULL != pvIter; )
	{
		SERIES_SET		*pstSeriesSet = (SERIES_SET *)HLIB_LIST_Data(pvIter);

		pvIter = HLIB_LIST_Next(pvIter);

		// SeriesSet ±¸Á¶·Î ¸¸µé¾î ³õÀº TBR, IR, Reminder¸¦ Á¦°Å ÇÑ´Ù.
		if (!MACRO_IS_TVA_SERIES_SET(pstSeriesSet))
		{
			HxLOG_Debug("Delete Series Set. pstSeriesSet= 0x%x\n ",pstSeriesSet);
			hErr |= samalib_tva_DeleteSeriesSet(pstSeriesSet);
			continue;
		}

		// alternative¸¦ Ã£´Ù°¡ ¹Ù²î¾îÁø selected¸¦ prepare conflict checking »óÅÂ ÀüÀ¸·Î µ¹¸°´Ù.
//		(void)samalib_tva_SetSelectedInstAsCandidateInst(pstSeriesSet);

		// alternative¸¦ À§ÇØ Ã£¾Æ³õÀº candidateµéÀ» Á¦°Å ÇÑ´Ù.
		(void)samalib_tva_RemoveNonSelectedInstance(pstSeriesSet);

		// alternative¸¦ À§ÇØ º¯°æµÈ candidate¸¦ selected·Î µÇµ¹·Á ÁØ´Ù.
		(void)samalib_tva_SetCandidateInstAsSelectedInst(pstSeriesSet);

	}



	return ERR_OK;
}


STATIC HERROR samalib_tva_SetSelectedInstAsCandidateInstAll(HUINT32 ulRsvCount, DxSchedule_t **ppstScheduleList)
{
	HUINT32		i;
	SERIES_SET *pstSeriesSet;
	DxSchedule_t	*pstScheduleInfo;
	HERROR			hErr;

	for(i = 0; i < ulRsvCount; i++)
	{
		pstScheduleInfo = ppstScheduleList[i];
		pstSeriesSet = samalib_tva_GetSeriesSetBySlotId(pstScheduleInfo->ulSlot);
		if (MACRO_IS_TVA_SCHEDULE(pstScheduleInfo,pstSeriesSet))
		{
			if(pstSeriesSet)
				samalib_tva_PrintSeriesSet(FALSE, pstSeriesSet);
			hErr = samalib_tva_SetSelectedInstAsCandidateInst(pstSeriesSet);
			if ( hErr != ERR_OK )
			{
				HxLOG_Debug("samalib_tva_SetSelectedInstAsCandidateInst failed. slot=(%d) \n",pstScheduleInfo->ulSlot);
			}
			hErr = samalib_tva_RemoveNonSelectedInstance(pstSeriesSet);
			if ( hErr != ERR_OK )
			{
				HxLOG_Debug("samalib_tva_RemoveNonSelectedInstance failed. slot=(%d) \n",pstScheduleInfo->ulSlot);
			}
		}
	}

	return ERR_OK;
}

STATIC void samalib_tva_OnDamaEventsReceived(const HCHAR *name, HUID uid, const void *data, HUINT32 size, void *userdata)
{
	DxEvent_t			*Event;
	DAxIterator_t		iter;
	HxList_t			**pstSeriesEventsList = (HxList_t**)userdata;
	HxList_t			*pstList = NULL;
	UNIXTIME			utCurrTime;

	HxLOG_Trace();

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if (UNIX_FIRST_DAY >= utCurrTime)
	{
		HxLOG_Error("systime not inited. \n");
		return ;
	}

	DAPI_InitIterator(&iter, data, size, 0);
	while (DAPI_NextIterator(&iter))
	{
		Event = (DxEvent_t *)DLIB_NewType(iter.name, iter.uid, iter.data, iter.size);
		if (Event)
		{
			DxEvent_t		*pstSeriesRecordEventInfo;
			pstSeriesRecordEventInfo = DxEVENT_Clone(Event);


			HxLOG_Debug("type[%s] appned event, event ID: %d \n",DLIB_ConvertEnum2Str(DxNAMEOF(DxEPG_Type_e), Event->type), Event->eventId);
			pstList = HLIB_LIST_Append(pstList, (void *)pstSeriesRecordEventInfo);
		}
		DLIB_FreeType(iter.name, Event);
	}

	*pstSeriesEventsList = pstList;

	return;
}

STATIC HCHAR * samalib_tva_MakeCridQuery (HCHAR *cridKeyword, SERIES_SET *pstSeriesSet)
{
	HINT32 				nStream =0;
	HCHAR				*pRet;

	if (cridKeyword == NULL || pstSeriesSet == NULL)
	{
		HxLOG_Error("invalid param. crid=(0x%x), seriesset=(0x%x)",cridKeyword, pstSeriesSet);
		return NULL;
	}

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 2048, SAMA_Malloc_CB, SAMA_Free_CB);
	if (nStream ==0 )
		return NULL;


	HLIB_RWSTREAM_Print(nStream,"SELECT '%s' AS tablename, uid, svcuid, eventid, start_time, keyword, link, linkdata FROM %s" , DxNAMEOF(eDxEPG_TYPE_PF), DxNAMEOF(eDxEPG_TYPE_PF));
	HLIB_RWSTREAM_Print(nStream," WHERE %s.%s",DxNAMEOF(eDxEPG_TYPE_PF),cridKeyword);
	// validÇÑ SD,HD typeÀÎ °æ¿ì¿¡ ±¸º°ÇÑ´Ù. (valid: hd typeÀÇ ÃÊ±â°ª(all)ÀÌ ¾Æ´Ô)
	if (pstSeriesSet->eHdType != eDxVIDEO_TYPE_ALL)
	{
		HLIB_RWSTREAM_Print(nStream," AND %s.svcuid in ",DxNAMEOF(eDxEPG_TYPE_PF));
		// video typeÀº all, sd, hd°¡ ÀÖÀ¸¹Ç·Î, SD seriesÀÎ °æ¿ì HD°¡ ¾Æ´Ñ service¸¦, HD series´Â SD°¡ ¾Æ´Ñ service¸¦ Ã£´Â´Ù.
		HLIB_RWSTREAM_Print(nStream," (SELECT uid from DxService_t where DxService_t.hd!=%d) ",(pstSeriesSet->eHdType == eDxVIDEO_TYPE_HD)? eDxVIDEO_TYPE_SD: eDxVIDEO_TYPE_HD);
	}
#if defined(CONFIG_OP_TDC)
	if(pstSeriesSet->searchFlag == DxEPG_CRID_SEARCH_FLAG_SAMESERVICE)
	{
		HLIB_RWSTREAM_Print(nStream," AND svcuid = %d", pstSeriesSet->ulSvcUid);
	}
#endif

	HLIB_RWSTREAM_Print(nStream," UNION");
	HLIB_RWSTREAM_Print(nStream," SELECT '%s' AS tablename, uid, svcuid, eventid, start_time, keyword, link, linkdata FROM %s" , DxNAMEOF(eDxEPG_TYPE_SC), DxNAMEOF(eDxEPG_TYPE_SC));
	HLIB_RWSTREAM_Print(nStream," WHERE %s.%s",DxNAMEOF(eDxEPG_TYPE_SC),cridKeyword);
	if (pstSeriesSet->eHdType != eDxVIDEO_TYPE_ALL)
	{
		HLIB_RWSTREAM_Print(nStream," AND %s.svcuid in ",DxNAMEOF(eDxEPG_TYPE_SC));
		HLIB_RWSTREAM_Print(nStream," (SELECT uid from DxService_t where DxService_t.hd!=%d) ",(pstSeriesSet->eHdType == eDxVIDEO_TYPE_HD)? eDxVIDEO_TYPE_SD: eDxVIDEO_TYPE_HD);
	}
#if defined(CONFIG_OP_TDC)
	if(pstSeriesSet->searchFlag == DxEPG_CRID_SEARCH_FLAG_SAMESERVICE)
	{
		HLIB_RWSTREAM_Print(nStream," AND svcuid = %d", pstSeriesSet->ulSvcUid);
	}
#endif

#if 0 //!defined(CONFIG_OP_TDC) // TDCÀÇ ipepg ÀÇ »óÈ°ÀÌ ¾î¶»°Ô µÉ±î...
	HLIB_RWSTREAM_Print(nStream," UNION");
	HLIB_RWSTREAM_Print(nStream," SELECT '%s' AS tablename, uid, svcuid, eventid, start_time, keyword, link, linkdata FROM %s" , DxNAMEOF(eDxEPG_TYPE_IP), DxNAMEOF(eDxEPG_TYPE_IP));
	HLIB_RWSTREAM_Print(nStream," WHERE %s.%s",DxNAMEOF(eDxEPG_TYPE_IP),cridKeyword);
	HLIB_RWSTREAM_Print(nStream," AND %s.svcuid in ",DxNAMEOF(eDxEPG_TYPE_IP));
	HLIB_RWSTREAM_Print(nStream," (SELECT uid from DxService_t where DxService_t.hd!=%d) ",(pstSeriesSet->isHD == TRUE)? eDxVIDEO_TYPE_SD: eDxVIDEO_TYPE_HD);

#endif
	HLIB_RWSTREAM_Print(nStream," ORDER BY start_time ASC");

	pRet = HLIB_RWSTREAM_CloseWithoutBuf(nStream);

	return pRet;
}
STATIC HERROR samalib_tva_BuildAlternative(SERIES_SET *pstSeriesSet, PROGRAM_SET *pstProgSet)
{
	HCHAR				*query=NULL;
	HCHAR				keyword[1024] = {0, }, cridstr[DxRSV_CRID_DATA_LEN+DxRSV_CRID_IMI_LEN+1] = {0, };
	HUINT32				id;
	HxList_t			*pstSeriesEventsList = NULL;
	DERROR				dErr;


	if ( pstSeriesSet == NULL
		|| pstProgSet == NULL )
	{
		HxLOG_Debug("Invalid param. null param. seriesSet=(0x%x), progSet=(0x%x) \n",pstSeriesSet, pstProgSet);
		return ERR_FAIL;
	}

	if ( pstProgSet->stProgCrid.type != SAMA_CRID_TYPE_PROG )
	{
		HxLOG_Debug("crid type is not prog. type=(%d) crid=[%s]\n", pstProgSet->stProgCrid.type,samalib_tva_ConvertCridToStr(&pstProgSet->stProgCrid));
		return ERR_FAIL;
	}

	HxSTD_snprintf(cridstr,DxRSV_CRID_DATA_LEN+DxRSV_CRID_IMI_LEN+1,"%s",(char*)pstProgSet->stProgCrid.data);

	HxLOG_Debug("Get Events. CRID=[%s]\n",cridstr);

	/*****************************
	HxSTD_StrNCat(keyword,"keyword LIKE ",HxSTD_StrLen("keyword like "));
	HxSTD_StrNCat(keyword ," '%",HxSTD_StrLen(" '%"));									// match any start
	HxSTD_StrNCat(keyword ,KEYWORD_CRID_PROGRAM,HxSTD_StrLen(KEYWORD_CRID_PROGRAM));	// match KEYWORD_CRID_PROGRAM
	HxSTD_StrNCat(keyword ,"%","%");													// match auth (if exists)
	HxSTD_StrNCat(keyword ,cridstr,HxSTD_StrLen(cridstr));								// match data
	// DLIMITER¸¦ µÚ¿¡ ³Ö´Â ÀÌÀ¯´Â exact match¸¦ ÇÏ±â À§ÇØ¼­ÀÌ´Ù. progCRID·Î ½ÃÀÛÇÏ´Â crid¸¦ ´Ù Ã£µµ·Ï ÇÏ±â À§ÇØ exact match¸¦ ÇÏÁö ¾Ê´Â´Ù.
	//HxSTD_StrNCat(keyword ,KEYWORD_CRID_DLIMITER,HxSTD_StrLen(KEYWORD_CRID_DLIMITER));
	HxSTD_StrNCat(keyword ,"%' ",HxSTD_StrLen("%' "));									// match any end
	OR as belows *************************/
	HxSTD_snprintf(keyword, 1024, "keyword LIKE '%%%s%%%s%%'",KEYWORD_CRID_PROGRAM, cridstr);	// match any (=%)¸¦ escape½ÃÅ°±â À§ÇØ %%¸¦ »ç¿ëÇÑ´Ù.

	query = samalib_tva_MakeCridQuery(keyword, pstSeriesSet);

	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n", query);
	HxLOG_Debug("==============================================================================\n");

	id = DAPI_Query(query, (DAPI_Getter)samalib_tva_OnDamaEventsReceived, (void*)&pstSeriesEventsList);
	HxLOG_Debug("DAPI_Query() id: %u \n", id);

	if (query)
		HLIB_RWSTREAM_MemFree(query);

	dErr = DAPI_Wait(10000, id);
	if (id == 0 || dErr != DERR_OK)
	{
		HLIB_LIST_RemoveAllFunc(pstSeriesEventsList, samalib_tva_DeleteEvent_CB);
		HxLOG_Error("Query FAIL \n");
		return ERR_FAIL;
	}


	samalib_tva_AddDxEventsToSeriesSet(pstSeriesEventsList,pstSeriesSet);

	HLIB_LIST_RemoveAllFunc(pstSeriesEventsList, samalib_tva_DeleteEvent_CB);

	return ERR_OK;
}

STATIC HERROR samalib_tva_BuildProgramSet(SERIES_SET *pstSeriesSet)
{
	HCHAR				*query = NULL;
	HCHAR				*keyword, cridstr[DxRSV_CRID_DATA_LEN+DxRSV_CRID_IMI_LEN+1] = {0, };
	HUINT32				id;
	HxList_t			*pstSeriesEventsList = NULL;

	PROGRAM_SET		*pstProgSet;
	INSTANCE_SET	*pstInstSet;
	HBOOL			bIsFirstKeyword = TRUE;
	HBOOL			bFoundPcrid		= FALSE;
	HINT32 			nStream =0;
	DERROR				dErr;

	nStream = HLIB_RWSTREAM_OpenEx(NULL, 2048, SAMA_Malloc_CB, SAMA_Free_CB);

	if (nStream ==0)
		return ERR_FAIL;

	for (pstProgSet = pstSeriesSet->pstProgList; pstProgSet; pstProgSet = pstProgSet->pNext)
	{
		if ( pstProgSet->stProgCrid.type != SAMA_CRID_TYPE_PROG )
		{
			HxLOG_Debug("crid type is not prog. type=(%d) \n",pstProgSet->stProgCrid.type);
			continue;
		}

		pstInstSet = pstProgSet->pstSelectedInst;
		if (pstInstSet == NULL)
		{
			HxLOG_Warning("Null selected \n.");
			continue;
		}

		HxSTD_snprintf(cridstr,DxRSV_CRID_DATA_LEN+DxRSV_CRID_IMI_LEN+1,"%s",(char*)pstProgSet->stProgCrid.data);

		HxLOG_Debug("Get Events. CRID=[%s]\n",cridstr);

		if (HxSTD_StrEmpty(cridstr))
		{
			HxLOG_Error("Empty crid!! \n");
			continue;
		}

		if ( bIsFirstKeyword == FALSE )
		{
			HLIB_RWSTREAM_Print(nStream," OR ");
		}

		/*
		HLIB_RWSTREAM_Print(nKeywordStream,"keyword LIKE ");
		HLIB_RWSTREAM_Print(nKeywordStream,"'%");													// match any start
		HLIB_RWSTREAM_Print(nKeywordStream,"%s",KEYWORD_CRID_PROGRAM);								// match pcrid:
		HLIB_RWSTREAM_Print(nKeywordStream,"%");													// match auth (if exists)
		HLIB_RWSTREAM_Print(nKeywordStream,"%s",cridstr);											// match data (+imi if exists)
		// DLIMITER¸¦ µÚ¿¡ ³Ö´Â ÀÌÀ¯´Â exact match¸¦ ÇÏ±â À§ÇØ¼­ÀÌ´Ù. progCRID·Î ½ÃÀÛÇÏ´Â crid¸¦ ´Ù Ã£µµ·Ï ÇÏ±â À§ÇØ exact match¸¦ ÇÏÁö ¾Ê´Â´Ù.
		//HLIB_RWSTREAM_Print(nKeywordStream,"%s",KEYWORD_CRID_DLIMITER);								// match , (for exact match)
		HLIB_RWSTREAM_Print(nKeywordStream,"%'");													// match any end
		 OR as below */
		HLIB_RWSTREAM_Print(nStream,"keyword LIKE '%%%s%%%s%%'",KEYWORD_CRID_PROGRAM, cridstr );

		bIsFirstKeyword = FALSE;
		bFoundPcrid		= TRUE;
	}

	// if there is no pcrid
	if ( bFoundPcrid == FALSE )
	{
		HLIB_RWSTREAM_Close(nStream);
		HxLOG_Debug("There is no crid. Stop Build Progset\n");
		return ERR_OK;
	}

	keyword = HLIB_RWSTREAM_CloseWithoutBuf(nStream);
	query = samalib_tva_MakeCridQuery(keyword, pstSeriesSet);

	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n", query);
	HxLOG_Debug("==============================================================================\n");

	id = DAPI_Query(query, (DAPI_Getter)samalib_tva_OnDamaEventsReceived, (void*)&pstSeriesEventsList);
	HxLOG_Debug("DAPI_Query() id: %u \n", id);

	if (keyword)
		HLIB_RWSTREAM_MemFree(keyword);
	if (query)
		HLIB_RWSTREAM_MemFree(query);

	dErr = DAPI_Wait(10000, id);
	if (id == 0 || dErr != DERR_OK)
	{
		HLIB_LIST_RemoveAllFunc(pstSeriesEventsList, samalib_tva_DeleteEvent_CB);
		HxLOG_Error("Query FAIL \n");
		return ERR_FAIL;
	}

	samalib_tva_AddDxEventsToSeriesSet(pstSeriesEventsList,pstSeriesSet);

	HLIB_LIST_RemoveAllFunc(pstSeriesEventsList, samalib_tva_DeleteEvent_CB);

	return ERR_OK;
}

STATIC HERROR samalib_tva_BuildSeriesSet(SERIES_SET *pstSeriesSet)
{
	HCHAR				*query;
	HCHAR				keyword[1024] = {0, }, cridstr[DxRSV_CRID_DATA_LEN+DxRSV_CRID_IMI_LEN+1] = {0, };
	HUINT32				id;
	HxList_t			*pstSeriesEventsList = NULL;
	DERROR				dErr;

	if ( pstSeriesSet->stCrid.imi != 0 )
	{
		HxSTD_snprintf(cridstr,DxRSV_CRID_DATA_LEN+DxRSV_CRID_IMI_LEN+1,"%s%s",(char*)pstSeriesSet->stCrid.data, (char*)&pstSeriesSet->stCrid.imi);
	}
	else
	{
		HxSTD_snprintf(cridstr,DxRSV_CRID_DATA_LEN+DxRSV_CRID_IMI_LEN+1,"%s",(char*)pstSeriesSet->stCrid.data);
	}
	HxLOG_Debug("Get Events. CRID=[%s]\n",cridstr);

	if ( HxSTD_StrEmpty(cridstr) )
	{
		HxLOG_Error("Empty crid !! \n");
		return ERR_FAIL;
	}


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
	HxSTD_StrNCat(keyword ,cridstr,HxSTD_StrLen(cridstr));
	HxSTD_StrNCat(keyword ,KEYWORD_CRID_DLIMITER,HxSTD_StrLen(KEYWORD_CRID_DLIMITER));
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

	query = samalib_tva_MakeCridQuery(keyword, pstSeriesSet);

	HxLOG_Debug("==============================================================================\n");
	HxLOG_Debug("query:\n\t%s\n", query);
	HxLOG_Debug("==============================================================================\n");

	id = DAPI_Query(query, (DAPI_Getter)samalib_tva_OnDamaEventsReceived, (void*)&pstSeriesEventsList);
	HxLOG_Debug("DAPI_Query() id: %u \n", id);

	if (query)
		HLIB_RWSTREAM_MemFree(query);

	dErr = DAPI_Wait(10000, id);
	if (id == 0 || dErr != DERR_OK)
	{
		HLIB_LIST_RemoveAllFunc(pstSeriesEventsList, samalib_tva_DeleteEvent_CB);
		HxLOG_Error("Query FAIL \n");
		return ERR_FAIL;
	}

	samalib_tva_AddDxEventsToSeriesSet(pstSeriesEventsList,pstSeriesSet);

	HLIB_LIST_RemoveAllFunc(pstSeriesEventsList, samalib_tva_DeleteEvent_CB);

	/* sort by earliest event of each selected instance */
	samalib_tva_SortSeriesSet(pstSeriesSet);

	return ERR_OK;
}

STATIC HERROR	samalib_tva_MakeInitialConflictSeriesSet(DxSchedule_t *pstSchedule, SERIES_SET *pstSeriesSet)
{
	HERROR hErr = ERR_OK;
	(void)pstSchedule;

	hErr = samalib_tva_BuildSeriesSet(pstSeriesSet);

	//: build programme setÀº build series setÀÇ °á°ú·Î event°¡ prog list¿¡ µé¾î°¡´Â °Í¿¡ ÀÇÁ¸ÇÑ´Ù. °í·Î sync call ÀÌ¾î¾ß ÇÑ´Ù.
	hErr |= samalib_tva_BuildProgramSet(pstSeriesSet);

	return hErr;
}


STATIC HERROR samalib_tva_MakeSeriesSet(DxSchedule_t *pstScheduleItem, UNIXTIME utCurrTime)
{
	UNIXTIME	start_time, duration;
	HERROR		hError = ERR_OK;
	SERIES_SET	*pstSeriesSet;

	if ( pstScheduleItem == NULL )
	{
		return ERR_FAIL;
	}

	if ( !MACRO_IS_CONFLICT_TYPE(pstScheduleItem->eRsvType))
		return ERR_FAIL;

	if(pstScheduleItem->eRsvType == DxRSVTYPE_WATCHING_EBR || pstScheduleItem->eRsvType == DxRSVTYPE_WATCHING_TBR || pstScheduleItem->eRsvType == DxRSVTYPE_WATCHING_SERIES_EBR)
	{
		 //WatchingÀº DurationÀ» 1sec·Î µÐ´Ù. --> º¯°æ series watchingÀÌ µé¾î°¡¸é¼­ eventÀÇ durationÀ» ±×´ë·Î »ç¿ëÇÏµµ·Ï º¯°æ
		(void)HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstScheduleItem->stStartTime), &start_time);
		duration = pstScheduleItem->ulDuration;
	}
	else if(pstScheduleItem->eRsvType == DxRSVTYPE_RECORD_EBR || pstScheduleItem->eRsvType == DxRSVTYPE_RECORD_TBR
			|| pstScheduleItem->eRsvType == DxRSVTYPE_RECORD_SERIES_EBR)
	{
		(void)HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstScheduleItem->stStartTime), &start_time);
		duration = pstScheduleItem->ulDuration;
	}
	else if(pstScheduleItem->eRsvType == DxRSVTYPE_RECORD_INSTANT || pstScheduleItem->eRsvType == DxRSVTYPE_RECORD_DELAYED)
	{
		(void)HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstScheduleItem->stStartTime), &start_time);
		duration = pstScheduleItem->ulDuration;
	}
	else if(pstScheduleItem->eRsvType == DxRSVTYPE_NETSTREAMING_LIVE || pstScheduleItem->eRsvType == DxRSVTYPE_NETSTREAMING_FILE)
	{
		(void)HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstScheduleItem->stStartTime), &start_time);
		duration = pstScheduleItem->ulDuration;
	}
	else
	{
		HxLOG_Warning("unknown rsv type. type=(%d)\n",pstScheduleItem->eRsvType);
		return ERR_FAIL;
	}

	pstSeriesSet = samalib_tva_GetSeriesSetBySlotId(pstScheduleItem->ulSlot);
	if ( pstSeriesSet == NULL )
	{
		HxLOG_Debug("make new ss. slot=(%d), rsv type=(%d)[%s]\n",pstScheduleItem->ulSlot, pstScheduleItem->eRsvType,DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), pstScheduleItem->eRsvType));
		pstSeriesSet = samalib_tva_NewSeriesSet(NULL, 0, 0);
		if ( pstSeriesSet == NULL )
		{
			HxLOG_Error("null series set \n");
			return ERR_FAIL;
		}

		pstSeriesSet->eRsvType = pstScheduleItem->eRsvType;
	}

	/* Series Set ±¸¼º */
	if (MACRO_IS_TVA_SCHEDULE(pstScheduleItem,pstSeriesSet))
	{
		/* Series Set ±¸¼º È®ÀÎ */
		if (   pstSeriesSet == NULL
			|| pstSeriesSet->pstProgList == NULL)
		{
			// series set, prorgamme set µÑ´Ù ¸¸µé¾î¾ß ÇÑ´Ù.
			hError |= samalib_tva_MakeInitialConflictSeriesSet(pstScheduleItem, pstSeriesSet);
		}
#if 0
		else
		{
			/* reset pstCandidateInst of all program_sets */
			hError |= samalib_tva_SetCandidateInstAsSelectedInst(pstSeriesSet);
		}
		samalib_tva_RemoveNonSelectedInstance(pstSeriesSet);
#endif

	}
	else
	{
		HxLOG_Debug("Not TVA Schedule. slot=(%d), type=(%d)[%s], seriesSet=(0x%x)\n",pstScheduleItem->ulSlot,pstScheduleItem->eRsvType,
											DLIB_ConvertEnum2Str(DxNAMEOF(DxRsvType_e), pstScheduleItem->eRsvType),pstSeriesSet);
		/* Series°¡ ¾Æ´Ò °æ¿ì¿¡µµ conflict check¸¦ À§ÇØ¼­ series set Ã³·³ once/repeat ±¸¼º */
		switch (pstScheduleItem->eRepeat)
		{
			case DxRSVREPEAT_DAILY:
			case DxRSVREPEAT_WEEKLY:
			case DxRSVREPEAT_WEEKDAYS:
			case DxRSVREPEAT_WEEKEND:
			case DxRSVREPEAT_WITHOUT_SUNDAY:
				hError |= samalib_tva_MakeSeriesSetForPeriodicTBR(pstScheduleItem->ulSvcUid, pstSeriesSet,
															pstScheduleItem->eRepeat, start_time, duration, utCurrTime, pstScheduleItem->ulSlot);
				break;

			default:
				hError |= samalib_tva_AddPeriodToSeriesSet(pstScheduleItem->ulSvcUid, pstScheduleItem->uExtInfo.stRec.nEvtId,
															start_time, start_time + duration, pstSeriesSet, pstScheduleItem->ulSlot);
				break;

		}
	}

	// series or programme °¡ ¾Æ´Ñ typeÀÇ °æ¿ì¿¡´Â alternative¸¦ Ã£Áö ¾Ê´Â´Ù.
	if ( !MACRO_IS_TVA_SCHEDULE(pstScheduleItem, pstSeriesSet))
	{
		PROGRAM_SET *pProgSet;
		INSTANCE_SET *pInstSet;

		pProgSet = pstSeriesSet->pstProgList;
		while(pProgSet)
		{
			pInstSet = pProgSet->pstInstList;
			while(pInstSet)
			{
				pInstSet->flag |= SAMA_INSTANCE_DO_NOT_FIND_ALTERNATIVE;
				pInstSet = pInstSet->pNext;
			}
			pProgSet = pProgSet->pNext;
		}
	}
	return hError;
}




STATIC HERROR	samalib_tva_FindAlternativeOfProgSet(SERIES_SET *pstSeriesSet, PROGRAM_SET *pProgSet)
{
	HUINT32			ulSvcUid;
	HERROR			hErr;

	if(pstSeriesSet == NULL || pProgSet == NULL)
	{
		HxLOG_Debug("null param\n");
		return ERR_FAIL;
	}

	if (pProgSet->flag & SAMA_ALTERNATIVE_INSTANCE_SEARCHED)
	{
		HxLOG_Debug("\033[30;46m already search done  \033[0m\n");
		return ERR_OK;
	}

	if (pProgSet->stProgCrid.data[0] == 0)
	{
		HxLOG_Debug("there is no crid\n");
		return ERR_FAIL;
	}

	ulSvcUid = pProgSet->pstSelectedInst->pstEventList->ulSvcUid;
	if(ulSvcUid == HANDLE_NULL)
	{
		HxLOG_Debug("invalid svc uid\n");
		return ERR_FAIL;
	}

	hErr = samalib_tva_BuildAlternative(pstSeriesSet, pProgSet);
	if (hErr != ERR_OK )
	{
		HxLOG_Debug("samalib_tva_BuildAlternative failed. \n");
		return ERR_FAIL;
	}

	pProgSet->flag = SAMA_ALTERNATIVE_INSTANCE_SEARCHED;

	return ERR_OK;
}



/*
 * Heart of alternative selection process
 */
STATIC SAPI_ConflictType_e samalib_tva_FindAlternative(HINT32 level, PROGRAM_SET *pProgSet, TVA_ALTERNATIVE_INFO *pAltInfo, SAPI_Conflict_t *pConfInfo, DxSchedule_t *pstTarget)
{
	// Series <-> Series °£ÀÇ conflict´Â conflict³ª´Â event °³¼ö°¡ ¸¹´Ù. ( ±Ø´Ü ÀûÀÎ °æ¿ì¿¡´Â µ¿½Ã °¡´É rec * ½ÃµµÇÑ series episode num -> 4 * 20 80°³ ±îÁö)
	// conflict result¸¦ ¶ç¿ï ¶§´Â MAX_CONFLICT_SLOT_NUM ¸¸Å­À¸·Î Á¦ÇÑÇÑ´Ù.
	// conflict °³¼ö°¡ MAX_CONFLICT_SLOT_NUMÀ» ÃÊ°úÇÏ´Â °æ¿ì¿¡´Â

	// sama:conflict noti -> web: cancel conflict -> web: record new -> sama: ÀÌÀü¿¡ MAX_CONFLICT_SLOT_NUMÀ» ÃÊ°úÇØ¼­ noti ¸ø ÇÑ conflict noti ·Î º¹±¸°¡ µÈ´Ù.
	static CONFLICT_LIST s_conflict_list[MAX_CONFLICT_CHECK_LEVEL][MAX_CONFLICT_SLOT_NUM];
	HINT32			num_conflict;
	SAPI_ConflictType_e	result;
	HINT32			i;
	INSTANCE_SET	*original = NULL, *candidate;
	EVENT_PERIOD	*src_event;
	HERROR			hErr = ERR_OK;


	if (level >= MAX_CONFLICT_CHECK_LEVEL)
		return eSAPI_CONFLICT_TYPE_OverCapacity;

	HxLOG_Debug("\033[30;47m check at level %d \033[0m \n", level);

RECHECK_PROG_SET:

	result = eSAPI_CONFLICT_TYPE_None;
	num_conflict = 0;

	for (src_event = pProgSet->pstCandidateInst->pstEventList; src_event; src_event = src_event->pNext)
	{
		result = samalib_tva_GetConflictList(src_event, &num_conflict, s_conflict_list[level], pConfInfo);
		if (result != eSAPI_CONFLICT_TYPE_None)
		{
			// ÀÏ¹ÝÀûÀÎ schedule add ½Ã¿¡ ( not series update ) pstTargetÀÌ null ÀÌ´Ù.
			if(pstTarget == NULL)
				return result;
			break;
		}
	}

	if (result == eSAPI_CONFLICT_TYPE_None)
	{
		HxLOG_Debug("\033[30;47m[%s:%d] eSAPI_CONFLICT_TYPE_None\033[0m \n",__FUNCTION__, __LINE__);
		if ( pAltInfo != NULL )
		{
			pAltInfo->result = result;
		}
		return result;
	}

	HxLOG_Debug("\033[30;47m conflict_list: \033[0m\n");
	for (i = 0; i < num_conflict; i++)
	{
		samalib_tva_PrintProgramSet(FALSE, s_conflict_list[level][i].pProgSet);
	}

	if (level == (MAX_CONFLICT_CHECK_LEVEL - 1))
	{
		/* stop finding alternatives */
		if ( pAltInfo != NULL )
		{
			pAltInfo->result = result;
		}
		return result;
	}
#if 0
	for (i = 0; i < num_conflict; i++)
	{
		if (IS_EVENT_VIEW(s_conflict_list[level][i].pEventPeriod))
		{
			/* stop finding alternatives - by scenario */
			if ( pAltInfo != NULL )
			{
				pAltInfo->result = result;
			}
			return result;
		}
	}
#endif
CHECK_NEXT_ALTERNATIVE:
	HxLOG_Debug("\033[30;47m () choose next alternative at level %d \033[0m \n", level);

	candidate = NULL;

	/* get alternative list */
	for (i = 0; i < num_conflict; i++)
	{
		SERIES_SET *pstSeriesSet;
		if (s_conflict_list[level][i].pProgSet->pstCandidateInst->flag & SAMA_INSTANCE_DO_NOT_FIND_ALTERNATIVE)
			continue;
		// alternative selecting ±â´É ±¸ÇöÀ» À§ÇØ target ÀÌ µÇ´Â programme set ÀÌ ¾Æ´Ò °æ¿ì Alternative ¸¦ Ã£Áö ¾Ê´Â´Ù.
		if (pstTarget != NULL)
		{
			pstSeriesSet = samalib_tva_GetSeriesSetBySlotId(pstTarget->ulSlot);
			if ( s_conflict_list[level][i].pProgSet->pstSeriesSet != pstSeriesSet)
				continue;
		}
		hErr = samalib_tva_FindAlternativeOfProgSet(s_conflict_list[level][i].pProgSet->pstSeriesSet, s_conflict_list[level][i].pProgSet);
		if ( hErr != ERR_OK )
		{
			HxLOG_Debug("Doesn't need to find alt prog set.\n");
			continue;
		}
	}

	for (i = 0; i < num_conflict; i++)
	{
		SERIES_SET *pstSeriesSet;

		if (s_conflict_list[level][i].pProgSet->pstCandidateInst->flag & SAMA_INSTANCE_DO_NOT_FIND_ALTERNATIVE)
			continue;
		// alternative selecting ±â´É ±¸ÇöÀ» À§ÇØ target ÀÌ µÇ´Â programme set ÀÌ ¾Æ´Ò °æ¿ì Alternative ¸¦ Ã£Áö ¾Ê´Â´Ù.
		if (pstTarget != NULL)
		{
			pstSeriesSet = samalib_tva_GetSeriesSetBySlotId(pstTarget->ulSlot);
			if ( s_conflict_list[level][i].pProgSet->pstSeriesSet != pstSeriesSet)
				continue;
		}


		/* choose next alternative candidate */
		if (s_conflict_list[level][i].pProgSet->pstCandidateInst->pNext)
			candidate = s_conflict_list[level][i].pProgSet->pstCandidateInst->pNext;
		else
			candidate = s_conflict_list[level][i].pProgSet->pstInstList;

#if (MAX_CONFLICT_CHECK_LEVEL == 2)
		original = s_conflict_list[level][i].pProgSet->pstSelectedInst;
		s_conflict_list[level][i].pProgSet->pstCandidateInst = candidate;
#endif
		if (candidate != s_conflict_list[level][i].pProgSet->pstSelectedInst)
		{
			HxLOG_Debug(" Change candidate inst :  \n");

			SAMALIB_TVA_PrintInstanceSet(candidate);
			break;
		}
	}

	if (i < num_conflict && candidate)
	{
		/* Found alternative candidate that had never been checked. */
#if (MAX_CONFLICT_CHECK_LEVEL != 2)
		original = s_conflict_list[level][i].pProgSet->pstSelectedInst;
		s_conflict_list[level][i].pProgSet->pstCandidateInst = candidate;
#endif

		HxLOG_Info("\033[30;47m recursive check [%d] \033[0m\n", i);
		samalib_tva_PrintProgramSet(FALSE, s_conflict_list[level][i].pProgSet);

		result = samalib_tva_FindAlternative(level + 1, s_conflict_list[level][i].pProgSet, NULL, NULL, pstTarget);
		if (result == eSAPI_CONFLICT_TYPE_None)
		{
			HxLOG_Info("\033[30;47m () recheck at level %d \033[0m\n", level);
			samalib_tva_AddToAltInfo(s_conflict_list[level][i].slot, original, candidate, pAltInfo);
			goto RECHECK_PROG_SET;
		}
		else
		{
			goto CHECK_NEXT_ALTERNATIVE;
		}
	}

#if defined(CONFIG_DEBUG)
	/* no more candidate */
	HxLOG_Debug("\033[30;47m no more candidate at level(%d) \033[0m {\n",level);
	for (i = 0; i < num_conflict; i++)
	{
		samalib_tva_PrintProgramSet(FALSE, s_conflict_list[level][i].pProgSet);
	}
	HxLOG_Debug("\033[30;47m } \033[0m\n");
#endif
	if ( pAltInfo != NULL )
	{
		pAltInfo->result = result;
	}

	return result;
}

STATIC SAPI_ConflictType_e samalib_tva_CheckScheduleConflict(SERIES_SET *pstSeriesSet,
														TVA_ALTERNATIVE_INFO *pAltInfo,
														SAPI_Conflict_t *pConfInfo,
														HBOOL bStopOnAlternative,
														DxSchedule_t *pstTarget)
{
	PROGRAM_SET	*pProgSetHead;
	PROGRAM_SET	*pProgSet;
	SAPI_ConflictType_e	result = eSAPI_CONFLICT_TYPE_None;
	HBOOL			bAlternative = FALSE;

	if (pstSeriesSet == NULL )
	{
		HxLOG_Debug("null param. \n ");
		return eSAPI_CONFLICT_TYPE_Tp;
	}

	/* correct series set pointer */
	for (pProgSet = pstSeriesSet->pstProgList; pProgSet; pProgSet = pProgSet->pNext)
	{
		pProgSet->pstSeriesSet = pstSeriesSet;
	}

	/* detach series tree */
	pProgSetHead = pstSeriesSet->pstProgList;

	/* InitAltInfoList */
	SAMA_MemSet(pAltInfo, 0, sizeof(TVA_ALTERNATIVE_INFO));

	/* °¢ programme setÀÌ conflict ¹ß»ýÇÏ´ÂÁö check */
	while (pProgSetHead)
	{
		pProgSet = pProgSetHead;
		pProgSetHead = pProgSetHead->pNext;

		if(pAltInfo && bStopOnAlternative)
		{
			SAMA_MemSet(pAltInfo, 0, sizeof(TVA_ALTERNATIVE_INFO));
		}

		/* test the prog set */
		//local_mwc_tva_PrintProgramSet(pProgSet);
		result = samalib_tva_FindAlternative(0, pProgSet, pAltInfo, pConfInfo, pstTarget);
		if (result != eSAPI_CONFLICT_TYPE_None)
		{
			/* stop checking conflict and return error */
			if (bStopOnAlternative == FALSE)
			{
				bAlternative = FALSE;
			}

			break;
		}

		if (pAltInfo && pAltInfo->numAlternative)
		{
			bAlternative = TRUE;
		}

		if (bStopOnAlternative && bAlternative)
		{
			break;
		}
	}

	if (/*result == eSAPI_CONFLICT_TYPE_None && */bAlternative)
	{
		if(pConfInfo)
		{
			pConfInfo->eType = eSAPI_CONFLICT_TYPE_Alternative;
		}
		result = eSAPI_CONFLICT_TYPE_Alternative;
	}


	return result;
}



#define ________GLOBAL_CONFLICT_FUNC______

HCHAR * SAMALIB_TVA_MakeCridQuery (HCHAR *szCridKeyword, SERIES_SET *pstSeriesSet)
{
	return samalib_tva_MakeCridQuery(szCridKeyword, pstSeriesSet);
}
HERROR SAMALIB_TVA_BuildProgramSet (SERIES_SET *pstSeriesSet)
{
	HERROR hErr;
	hErr = samalib_tva_BuildProgramSet(pstSeriesSet);
	return hErr;
}
HERROR SAMALIB_TVA_BuildSeriesSet (SERIES_SET *pstSeriesSet)
{
	HERROR hErr;
	hErr = samalib_tva_BuildSeriesSet(pstSeriesSet);
	return hErr;

}

SAPI_ConflictType_e SAMALIB_TVA_CheckScheduleConflict (SERIES_SET* pstSeriesSet, SAPI_Conflict_t *pConfInfo)
{
	SAPI_ConflictType_e		eConflictType;
	TVA_ALTERNATIVE_INFO	stDummyAltInfo;
	HBOOL					bStopOnAlternative = FALSE;
	DxSchedule_t			*pstTarget = NULL;

	eConflictType = samalib_tva_CheckScheduleConflict(pstSeriesSet, &stDummyAltInfo, pConfInfo, bStopOnAlternative, pstTarget);
	return eConflictType;
}


HERROR SAMALIB_TVA_GetNextAlternativeInfo(SERIES_SET *pstSeriesSet, DxSchedule_t *pstTarget, TVA_ALTERNATIVE_INFO *pAltInfo)
{
	SAPI_ConflictType_e result;
	HERROR			hError = ERR_FAIL;
	int i, j;

	if(pstSeriesSet == NULL || pAltInfo == NULL)
		return ERR_FAIL;

	SAMA_MemSet(pAltInfo, 0, sizeof(TVA_ALTERNATIVE_INFO));
	for (i = 0; i < SAMA_MAX_ALTERNATIVE_INSTANCE; i++) // 2:MAX_NUM_RECORD
	{
		for (j = 0; j < SAMA_MAX_SPLIT_EVENT_IN_ALT_INFO; j++)
		{
			pAltInfo->instance[i].svcHandle[j] = HANDLE_NULL;
			pAltInfo->instance[i].altSvcHandle[j] = HANDLE_NULL;
		}
	}

	result = samalib_tva_CheckScheduleConflict(pstSeriesSet, pAltInfo, NULL, TRUE, pstTarget);
	if (result == eSAPI_CONFLICT_TYPE_Alternative)
		hError = ERR_OK;

#if defined(CONFIG_DEBUG)
	if (hError == ERR_OK)
	{
		if(pAltInfo->numAlternative)
		{
			HxLOG_Debug("(): Alternative\n");
			SAMALIB_TVA_PrintAltInfo(pAltInfo);
		}
		else
		{
			HxLOG_Debug("(): OK\n");
		}
	}
	else
	{
		HxLOG_Debug("(): conflict\n");
	}
#endif

	return hError;
}

HERROR	SAMALIB_TVA_PrepareConflictChecking (HUINT32 ulSchedTotal, DxSchedule_t *pstScheduleArray)
{
	HUINT32			i;
	UNIXTIME		utCurrTime;
	HERROR			hErr = ERR_OK;

	if ( ulSchedTotal == 0
		|| pstScheduleArray == NULL )
	{
		HxLOG_Error("null param\n");
		return ERR_FAIL;
	}

	utCurrTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if(UNIX_FIRST_DAY >= utCurrTime)
	{
		HxLOG_Error("time error. current sys time = (%d)\n",utCurrTime);
	}

	for (i=0; i<ulSchedTotal; i++)
	{
		DxSchedule_t		*pstScheduled = (DxSchedule_t *)&(pstScheduleArray[i]);
		if ( pstScheduled == NULL )
		{
			HxLOG_Debug("null schedule\n");
			continue;
		}

		if ( pstScheduled->bRemoved == TRUE
			|| pstScheduled->stRemovedInfo.eRemovedReason == DxSchedule_RemovedReason_SeriesTracking )
		{
			HxLOG_Debug("removed schedule. slot=(%d) \n",pstScheduled->ulSlot);
			continue;
		}

		if( ! MACRO_IS_CONFLICT_TYPE(pstScheduled->eRsvType))
		{
			continue;
		}

		hErr |= samalib_tva_MakeSeriesSet(pstScheduled,utCurrTime);
	}

	return hErr;
}

HERROR SAMALIB_TVA_ConcludeConflictChecking(SERIES_SET *pstSeriesSet, HBOOL cancel)
{
	HERROR hErr;
	hErr = samalib_tva_ConcludeConflictChecking(pstSeriesSet, cancel);
	return hErr;
}

HERROR SAMALIB_TVA_SetSelectedAsCandidate (SERIES_SET *pstSeriesSet)
{
	HERROR hErr;

	if (pstSeriesSet == NULL)
	{
		HxLOG_Error("Null param. \n");
		return ERR_FAIL;
	}

	hErr = samalib_tva_SetSelectedInstAsCandidateInst(pstSeriesSet);
	return hErr;
}
HERROR SAMALIB_TVA_SetCandidateAsSelected (SERIES_SET *pstSeriesSet)
{
	HERROR hErr;

	if (pstSeriesSet == NULL)
	{
		HxLOG_Error("Null param. \n");
		return ERR_FAIL;
	}

	hErr = samalib_tva_SetCandidateInstAsSelectedInst(pstSeriesSet);
	return hErr;
}


HERROR	SAMALIB_TVA_IsTVAScheduleRemovable(DxSchedule_t *pstScheduleItem, HBOOL bCallByUpdate)
{
	UNIXTIME	ulSysTime;
	UNIXTIME	utStartTime;
	UNIXTIME	utEndTime;
	UNIXTIME	utTrackingEndTime;
	HUINT8		ucRecKind;
	HUINT8		ucCRIDType;

	ulSysTime = (UNIXTIME)HLIB_STD_GetSystemTime();
	if(UNIX_FIRST_DAY >= ulSysTime)
		return ERR_FAIL;

	if(pstScheduleItem == NULL)
		return ERR_FAIL;

	switch (pstScheduleItem->eScheduleSpec)
	{
		case eDxSchedule_Spec_TvAnyTime:
			{
				DxRSV_TvAnyTimeRecord_t *pstTvAnyTime = NULL;
				if (MACRO_IS_TVA_RECORD_TYPE(pstScheduleItem->eRsvType))
				{
					pstTvAnyTime = &pstScheduleItem->uExtInfo.stRec.ex.tva;
				}
				else if (MACRO_IS_TVA_WATCH_TYPE(pstScheduleItem->eRsvType))
				{
					pstTvAnyTime = &pstScheduleItem->uExtInfo.stWch.ex.tva;
				}
				else
				{
					HxLOG_Error("unsupported type. type=(%d) \n",pstScheduleItem->eRsvType);
					return ERR_FAIL;
				}

				ucRecKind = pstTvAnyTime->ucRecKind;
				ucCRIDType = pstTvAnyTime->ucCRIDType;
			}
            break;
		default:
			HxLOG_Error("unsupported spec. spec=(%d) \n",pstScheduleItem->eScheduleSpec);
			return ERR_FAIL;
	}
	if (!(ucRecKind & SAMA_SERIES_CONTENT_KIND_CRID))
	{
		HxLOG_Error("Invalid schedule kind. kind=(0x%x) \n", ucRecKind);
		return ERR_FAIL;
	}

	(void)HLIB_DATETIME_ConvertDateTimeToUnixTime (&(pstScheduleItem->stStartTime), &utStartTime);
	utEndTime = utStartTime + pstScheduleItem->ulDuration;
	if (ucCRIDType == SAMA_CRID_TYPE_SERIES)
	{
		// 13 weeks
		utTrackingEndTime = utEndTime + (86400 * 13 * 7);
	}
	else if(ucCRIDType == SAMA_CRID_TYPE_PROG)
	{
		// 3 Hours
		utTrackingEndTime = utEndTime + (3 * 60 * 60);
	}
	else
	{
		HxLOG_Error("Not crid tracking schedule !!!. slot=(%d) \n", pstScheduleItem->ulSlot);
		return ERR_OK;
	}
	if (utTrackingEndTime <= ulSysTime)
	{
		HxLOG_Debug("[01;36m()[00m slot[%d] can be removed\n", pstScheduleItem->ulSlot);
		return ERR_OK;
	}

	return ERR_FAIL;
#if 0
	//if(utStartTime == 0 && pstScheduleItem->ulDuration == 0)
	//	return ERR_FAIL;

	//pstSeriesSet = samalib_tva_GetSeriesSetBySlotId(pstScheduleItem->ulSlot);
	//if(!MACRO_IS_TVA_SCHEDULE(pstScheduleItem,pstSeriesSet))
	//	return ERR_FAIL;


	if(pstSeriesSet->pstProgList == NULL)
	{
		utEndTime = utStartTime + pstScheduleItem->ulDuration;
		if(pstSeriesSet->stCrid.type == SAMA_CRID_TYPE_SERIES)
		{
			/* 13ÁÖ */
			if (utEndTime + (86400 * 13 * 7) <= ulSysTime)
			{
				HxLOG_Print("[01;36m()[00m slot[%d] can be removed\n", pstScheduleItem->ulSlot);
				return ERR_OK;
			}
		}
		/*
		else if(bCallByUpdate && pstSeriesSet->stCrid.type == SAMA_CRID_TYPE_PROG)
		{
			 ÂÁ... RCT °¡ ÄÚµå ´Ù ¸ÁÄ£´Ù... °°¾Æ º¸ÀÌ¸é¼­µµ Á¶±Ý¾¿ ´Ù¸¥ °ÍÀ» ¿ä±¸ÇØ ÇÔ¼öµéÀÇ ParamÀÌ ¼³°è ´ç½Ã¿Í ´Þ¶óÁ®¹ö¸®°í ÀÖ´Ù.
			    ¾î°ÅÁö·Î ÇÔ¼ö¸¦ ¹Ù²ã¼­ »ç¿ë ÁßÀÓ....
			    Prog CRID : Split or Single - 3½Ã°£ ¾È¿¡ EIT Update°¡ ÀÏ¾î³¯ ¼ö ÀÖÀ¸¹Ç·Î Spec¿¡¼­ Á¤ÀÇÇÑ 3½Ã°£Àº À¯ÁöÇÏ¿© ÁØ´Ù.
			if (utEndTime + (3 * 60 * 60) <= ulSysTime)
			{
				HxLOG_Print("[01;36m()[00m slot[%d] can be removed\n", pstScheduleItem->ulSlot);
				return ERR_OK;
			}
		}
		*/
		else
		{
			HxLOG_Print("[01;36m()[00m slot[%d] can be removed\n", pstScheduleItem->ulSlot);
			return ERR_OK;
		}
	}

	return ERR_FAIL;
#endif
}

HERROR SAMALIB_TVA_ClearAlternativeItems (SAPI_AlternativeItems_t *pstAltItems)
{
	HUINT32	i;
	if (pstAltItems == NULL)
	{
		HxLOG_Error("Invalid param. null alternative\n");
		return ERR_FAIL;
	}

	pstAltItems->ulNumAlternatives = 0;
	for (i=0; i<SAMA_MAX_ALTERNATIVE; i++)
	{
		pstAltItems->astAltItemArray[i].ulSlot = DxRSV_SLOT_NULL;
		pstAltItems->astAltItemArray[i].ulAltEvtId = 0;
		pstAltItems->astAltItemArray[i].ulAltSvcUid= 0;
	}

	return ERR_OK;
}
HERROR SAMALIB_TVA_AppendAlternativeItem (SAPI_AlternativeItems_t *pstAltItems, SAPI_AlternativeItem_t *pstItem)
{
	SAPI_AlternativeItem_t 	*pstArrayItem;

	if (pstAltItems->ulNumAlternatives +1 > SAMA_MAX_ALTERNATIVE )
	{
		HxLOG_Warning("Only %d alternatives can be delivered. \n",SAMA_MAX_ALTERNATIVE);
		return ERR_FAIL;
	}

	HxLOG_Debug("%d th alternative item pstItem->ulSlot: %d\n", pstAltItems->ulNumAlternatives, pstItem->ulSlot);

	pstArrayItem = &(pstAltItems->astAltItemArray[pstAltItems->ulNumAlternatives]);
	SAMA_MemCpy(pstArrayItem, pstItem, sizeof(SAPI_AlternativeItem_t));
	(pstAltItems->ulNumAlternatives)++;

	return ERR_OK;
}

void   SAMALIB_TVA_InitAlternativeSeriesSet (void)
{
	HINT32 i;
	for (i=0;i<SAMA_MAX_ALTERNATIVE_INSTANCE;i++)
	{
		s_astAlternativeSeriesMap[i].ulAltEventId = 0;
		s_astAlternativeSeriesMap[i].ulAltSvcUid = 0;
		s_astAlternativeSeriesMap[i].pstOrgSeriesSet = NULL;
	}

	s_AlternativeSeriesMapIndex = 0;
}

SERIES_SET *SAMALIB_TVA_GetAlternativeSeriesSet (HUINT32 ulEvtId, HUINT32 ulSvcUid)
{
	/**************************************************************************************************************************
	// ÇÔ¼öÀÇ Á¸Àç ÀÌÀ¯
	// 1. ¹®Á¦:  alternative series´Â original series¿Í series CRID°¡ ´Ù¸£´Ù
 ( °°Àº programme CRID°¡ ÀÖ´õ¶óµµ)
	//     1-1. ¿¹) E4 C4CA1234
 E4+1 C4CI1234
	//     1-2 GetEventsBySchedule·Î alternative event¸¦ series ¿¹¾à °É¸é alternative series¿¡ ÇØ´çÇÏ´Â "¸ðµç" episode¸¦ ´Ù °É¾î¹ö¸°´Ù.
	// 2.  ¹®Á¦: series CRID°¡ ´Ù¸£¹Ç·Î ·¹ÄÚµù ÆÄÀÏÀÌ "´Ù¸¥ series"·Î ¹­¿©¹ö¸°´Ù.
	// 3. ÀÌ¸¦ ÇØ°á ÇÏ±â À§ÇØ¼­´Â
	//     3-1 original series set¿¡ alternative°¡ ÇÊ¿äÇÑ episode¸¸ °É¾î¾ß ÇÑ´Ù.             => ¹®Á¦ 1 ÇØ°á
	//     3-2 alternative seriesÀÏÁö¶óµµ original seriesÀÇ series CRID¸¦ »ç¿ëÇØ¾ß ÇÑ´Ù.  => ¹®Á¦ 2 ÇØ°á
	// 4.  3ÀÇ ±¸Çö ¹æ¹ý
	//     4-1 alternative series setÀº original series setÀ» ¾ò¾î¿À°í,  => 3-1 ±¸Çö
	//     4-2 MakeAlternativeSchedule ·çÆ¾À» ÅÂ¿ö¼­ selected programme inst¸¦ alternative programmeÀ¸·Î ¸¸µç´Ù.   =>3-2 ±¸Çö
	// 5. alternative series setÀÇ »ç¿ë¹ýÀº
	//     5-1 ÀÏ¹Ý schedule add¿¡¼­ alternative notify¸¦ ÇÏ´Â °æ¿ì¿¡ Init, SetAlternativeSeriesSetÀ» ÇÑ´Ù.
	//     5-2 Web¿¡¼­ alternative notify¿¡ µû¶ó ¿Ã¶ó¿Â alt evt¸¦ ¼±ÅÃÇÑ´Ù.
	//     5-3 ¼±ÅÃµÈ alt evt¿¡ ´ëÇÑ alt series setÀ» Àü¿ª mapÀ» ÅëÇØ ¾ò´Â´Ù.
	****************************************************************************************************************************/

	HINT32 i;

	for (i=0; i<s_AlternativeSeriesMapIndex && i<SAMA_MAX_ALTERNATIVE_INSTANCE; i++)
	{
		if (   s_astAlternativeSeriesMap[i].ulAltEventId == ulEvtId
			&& s_astAlternativeSeriesMap[i].ulAltSvcUid == ulSvcUid )
		{
			HxLOG_Debug("Found alternative series set. evtid=(%d), svcid=(%d), seriesset=(0x%x) \n", ulEvtId, ulSvcUid, s_astAlternativeSeriesMap[i].pstOrgSeriesSet);
			samalib_tva_PrintSeriesSet(FALSE, s_astAlternativeSeriesMap[i].pstOrgSeriesSet);
			return s_astAlternativeSeriesMap[i].pstOrgSeriesSet;
		}
	}

	HxLOG_Error("Fail to find alternative seires set. evtid=(%d), svcid=(%d), map index=(%d) \n", ulEvtId, ulSvcUid, s_AlternativeSeriesMapIndex);
	return NULL;
}

HERROR SAMALIB_TVA_SetAlternativeSeriesSet (HUINT32 ulEvtId, HUINT32 ulSvcUid, SERIES_SET *pstSeriesSet)
{
	if (s_AlternativeSeriesMapIndex >= SAMA_MAX_ALTERNATIVE_INSTANCE)
	{
		HxLOG_Error("Fail to add alternative series set. map index=(%d) \n",s_AlternativeSeriesMapIndex);
		return ERR_FAIL;
	}

	if (pstSeriesSet->stCrid.type != SAMA_CRID_TYPE_SERIES)
	{
		HxLOG_Debug("not series type. type=(%d) \n", pstSeriesSet->stCrid.type);
		return ERR_OK;
	}

	s_astAlternativeSeriesMap[s_AlternativeSeriesMapIndex].ulAltEventId = ulEvtId;
	s_astAlternativeSeriesMap[s_AlternativeSeriesMapIndex].ulAltSvcUid = ulSvcUid;
	s_astAlternativeSeriesMap[s_AlternativeSeriesMapIndex].pstOrgSeriesSet = pstSeriesSet;

	s_AlternativeSeriesMapIndex++;

	return ERR_OK;
}

/* end of file */

