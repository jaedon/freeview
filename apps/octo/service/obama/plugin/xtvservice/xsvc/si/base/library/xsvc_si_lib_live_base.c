/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xsvc_si_lib_live_base.c
	@brief

	Description:  									\n
	Module: Porting Layer/TV Service/SI /LIBRARY		\n

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
#include <hlib.h>
#include <silib.h>
#include <vkernel.h>
#include <octo_common.h>
#include <isosvc.h>

#include <pal_sirawpool.h>
#include <pal_pipe.h>
#include <svc_si.h>
#include <_svc_si.h>
#include <_svc_si_lib.h>
#include <_svc_si_filtering.h>
#include <_svc_si_live.h>
#include <_svc_si_hbbtv.h>


/*******************************************************************/
/********************      Defines         *************************/
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
#define ___LOCAL_FUNCTIONS___

STATIC void xsvc_si_LibLiveFreeAitPidList (void *pvData)
{
#if defined(CONFIG_MW_SI_AIT)
	HUINT32		*pulAitPid = (HUINT32 *)pvData;

	if (NULL != pulAitPid)
	{
		OxSI_Free(pulAitPid);
	}
#endif

	return;
}

#define ___MEMBER_FUNCTIONS___
HERROR xsvc_si_LibLiveOnPatReceived_Base (svcSi_LiveData_t *pstLive, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	HUINT16					 usPmtPid = PID_NULL;
	HUINT32					 ulActionId = PAL_PIPE_GetActionId (hAction);
	HERROR					 hErr;

	if (eSI_TABLE_TIMEOUT == pstSecCb->ulTableStatus)
	{
		if ((FILTER_ID_NULL == pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Pat]) ||
			(pstSecCb->ulFilterId != pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Pat]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_PAT_TIMEOUT, hAction, 0, 0, 0);
		}

		pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Pat] = pstSecCb->ulFilterId;
		return ERR_OK;
	}

	// 약 1000ms 는 이전 TP의 PAT가 올라올 수 있는 MAX time으로 잡는다.
	if (HLIB_STD_GetSystemTick() < (pstLive->ulPatTimeTick + 1000))
	{
		HBOOL				 bFound = FALSE;
		HUINT16 			 usTsIdInPat = 0;
		SIxTable_PsiTable_t *pstTable, *pstTableHead;

		pstTableHead = PAL_SIRAWPOOL_ParseTable(ulActionId, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_ALL);
		for (pstTable = pstTableHead; NULL != pstTable; pstTable = pstTable->pstNextTable)
		{
			usTsIdInPat = pstTable->unInfo.stPat.usTsId;
			if (pstTable->unInfo.stPat.usTsId == pstLive->usSdtTsId)
			{
				bFound = TRUE;
				break;
			}
		}

		if (NULL != pstTableHead)			{ PAL_SIRAWPOOL_FreeTable(pstTableHead); }

		if (TRUE != bFound)
		{
			// PAT Filter를 빼고 다시 걸도록 한다. PAL쪽에 monitor 모드가 monitor_by_version 방식이라 새 데이터가 들어오는 걸 인지하는
			// 것이 PAT 단위로는 매우 길어져 버리므로 여기서 다시 건다.
			HxLOG_Error("PAT with wrong TS ID: TsId(SvcTsId(0x%04x), InPat(0x%04x)\n", pstLive->usSdtTsId, usTsIdInPat);

			(void)PAL_SIRAWPOOL_FreeRawSection(ulActionId, eSIxTABLETYPE_Pat, FALSE, 0);
			(void)svc_si_FilterStartTableSection(hAction, pstLive->ulDemuxId, eSIxTABLETYPE_Pat, SILIB_TABLE_EXT_ID_NOT, PAT_PID, eSI_TABLE_FILTER_MODE_Monitor, NULL);
			return ERR_OK;
		}
	}

	hErr = svc_si_LibGetPmtPidFromPat (ulActionId, pstLive->usPmtSvcId, &usPmtPid);
	if ((ERR_OK != hErr) || (PID_NULL == usPmtPid))
	{
		pstLive->bAbortPatPmt = TRUE;

		pstLive->usPmtPid = PID_NULL;
		svc_si_PostMsgToAp (eSEVT_SI_PMT_PIDREMOVED, hAction, 0, 0, 0);
		return ERR_OK;
	}
	else if (pstLive->usPmtPid != usPmtPid)
	{
		// PMT Changed:
		(void)svc_si_FilterStopTableSection (hAction, eSIxTABLETYPE_Pmt, (HUINT32)pstLive->usPmtSvcId);

		pstLive->bAbortPatPmt = TRUE;
		pstLive->usPmtPid = usPmtPid;
		svc_si_PostMsgToAp (eSEVT_SI_PMT_PIDCHANGED, hAction, 0, 0, 0);
		return ERR_OK;
	}

	// 이미 걸은 PMT PID는 유효하다.
	pstLive->bAbortPatPmt = FALSE;
	return ERR_OK;
}

HERROR xsvc_si_LibLiveOnPmtReceived_Base (svcSi_LiveData_t *pstLive, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	if (TRUE == pstLive->bAbortPatPmt)
	{
		return ERR_OK;
	}

	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		if ((FILTER_ID_NULL == pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Pmt]) ||
			(pstSecCb->ulFilterId != pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Pmt]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_PMT_TIMEOUT, hAction, 0, 0, 0);
		}

		pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Pmt] = pstSecCb->ulFilterId;
		break;

	case eSI_TABLE_RECEIVED:
	default:
		svc_si_PostMsgToAp (eSEVT_SI_PMT, hAction, 0, 0, 0);
		break;
	}

	return ERR_OK;
}

HERROR xsvc_si_LibLiveOnEitActPfReceived_Base (svcSi_LiveData_t *pstLive, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
//	HUINT32				 ulActionId = PAL_PIPE_GetActionId (hAction);

	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		if ((FILTER_ID_NULL == pstLive->aulTimeoutFilterId[eSIxTABLETYPE_EitActPf]) ||
			(pstSecCb->ulFilterId != pstLive->aulTimeoutFilterId[eSIxTABLETYPE_EitActPf]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_EIT_TIMEOUT, hAction, 0, 0, 0);
		}

		pstLive->aulTimeoutFilterId[eSIxTABLETYPE_EitActPf] = pstSecCb->ulFilterId;
		break;

	case eSI_TABLE_RECEIVED:
	default:
		svc_si_PostMsgToAp (eSEVT_SI_EIT, hAction, 0, 0, 0);
		break;
	}

	return ERR_OK;
}

HERROR xsvc_si_LibLiveOnBatReceived_Base (svcSi_LiveData_t *pstLive, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{

	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		if ((FILTER_ID_NULL == pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Bat]) ||
			(pstSecCb->ulFilterId != pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Bat]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_BAT_TIMEOUT, hAction, 0, 0, 0);
		}

		pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Bat] = pstSecCb->ulFilterId;
		break;

	case eSI_TABLE_RECEIVED:
	default:
		svc_si_PostMsgToAp (eSEVT_SI_BAT, hAction, 0, 0, 0);
		break;
	}

	return ERR_OK;
}

HERROR xsvc_si_LibLiveOnTdtReceived_Base (svcSi_LiveData_t *pstLive, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	SIxTable_PsiTable_t	*pstTdtTable;
	HUINT32				 ulActionId = PAL_PIPE_GetActionId (hAction);

	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		if ((FILTER_ID_NULL == pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Tdt]) ||
			(pstSecCb->ulFilterId != pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Tdt]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_TDT_TIMEOUT, hAction, 0, 0, 0);
		}

		pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Tdt] = pstSecCb->ulFilterId;
		break;

	case eSI_TABLE_RECEIVED:
	default:
		pstTdtTable = PAL_SIRAWPOOL_ParseTable (ulActionId, eSIxTABLETYPE_Tdt, SILIB_TABLE_EXT_ID_NOT);
		if (NULL != pstTdtTable)
		{
			if (svc_si_LibUpdateTimeByTdt (ulActionId, pstLive->hService, pstTdtTable) == ERR_OK)
			{
				svc_si_PostMsgToAp (eSEVT_SI_TDT, hAction, 0, 0, 0);
			}

			PAL_SIRAWPOOL_FreeTable (pstTdtTable);
		}
		break;
	}

	return ERR_OK;
}

HERROR xsvc_si_LibLiveOnTotReceived_Base (svcSi_LiveData_t *pstLive, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		if ((FILTER_ID_NULL == pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Tot]) ||
			(pstSecCb->ulFilterId != pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Tot]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_TOT_TIMEOUT, hAction, 0, 0, 0);
		}

		pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Tot] = pstSecCb->ulFilterId;
		break;

	case eSI_TABLE_RECEIVED:
	default:
		// TOT는 mgr level에서 처리할 것이므로 여기서는 msg만 날려준다
		// xproc_action_Normal_Base()에서 TOT msg를 받으면 region id를 사용해서 TZ file update하고 시간 설정할 것이다 .
		svc_si_PostMsgToAp (eSEVT_SI_TOT, hAction, (HINT32)pstLive->hService, 0, 0);
		break;
	}

	return ERR_OK;
}

HERROR xsvc_si_LibLiveOnNitActReceived_Base (svcSi_LiveData_t *pstLive, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
//	HUINT32				 ulActionId = PAL_PIPE_GetActionId (hAction);

	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		if ((FILTER_ID_NULL == pstLive->aulTimeoutFilterId[eSIxTABLETYPE_NitAct]) ||
			(pstSecCb->ulFilterId != pstLive->aulTimeoutFilterId[eSIxTABLETYPE_NitAct]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_NIT_TIMEOUT, hAction, 0, 0, 0);
		}

		pstLive->aulTimeoutFilterId[eSIxTABLETYPE_NitAct] = pstSecCb->ulFilterId;
		break;

	case eSI_TABLE_RECEIVED:
	default:
		svc_si_PostMsgToAp (eSEVT_SI_NIT, hAction, 0, 0, 0);
		break;
	}

	return ERR_OK;
}

HERROR xsvc_si_LibLiveOnSdtActReceived_Base (svcSi_LiveData_t *pstLive, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
//	HUINT32				 ulActionId = PAL_PIPE_GetActionId (hAction);

	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		if ((FILTER_ID_NULL == pstLive->aulTimeoutFilterId[eSIxTABLETYPE_SdtAct]) ||
			(pstSecCb->ulFilterId != pstLive->aulTimeoutFilterId[eSIxTABLETYPE_SdtAct]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_SDT_TIMEOUT, hAction, 0, 0, 0);
		}

		pstLive->aulTimeoutFilterId[eSIxTABLETYPE_SdtAct] = pstSecCb->ulFilterId;
		break;

	case eSI_TABLE_RECEIVED:
	default:
		svc_si_PostMsgToAp (eSEVT_SI_SDT, hAction, 0, 0, 0);
		break;
	}

	return ERR_OK;
}

HERROR xsvc_si_LibLiveOnCatReceived_Base (svcSi_LiveData_t *pstLive, Handle_t hAction, SvcSi_Arg_SecCb_t *pstSecCb)
{
	switch (pstSecCb->ulTableStatus)
	{
	case eSI_TABLE_TIMEOUT:
	case eSI_TABLE_FAIL:
		if ((FILTER_ID_NULL == pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Cat]) ||
			(pstSecCb->ulFilterId != pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Cat]))
		{
			svc_si_PostMsgToAp (eSEVT_SI_CAT_TIMEOUT, hAction, 0, 0, 0);
		}

		pstLive->aulTimeoutFilterId[eSIxTABLETYPE_Cat] = pstSecCb->ulFilterId;
		break;

	case eSI_TABLE_RECEIVED:
	default:

		svc_si_PostMsgToAp (eSEVT_SI_CAT, hAction, 0, 0, 0);
		break;
	}

	return ERR_OK;

}

/* end of file */

