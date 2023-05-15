/**
	@file     	xmgr_search.c
	@brief

	Description:  			\n
	Module: 					 					\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_search.h>
#include <pmgr_search.h>
#include "./local_include/_xmgr_search.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

STATIC MgrSearch_ResultContext_t	 *s_apstSrchRst_Contents[NUM_MAX_ACTION] = { NULL, };

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

MgrSearch_ResultContext_t *xmgr_search_ResultGetContext (HUINT32 ulActionId)
{
	if (ulActionId >= NUM_MAX_ACTION)
	{
		return NULL;
	}

	if (NULL != s_apstSrchRst_Contents[ulActionId])
	{
		return s_apstSrchRst_Contents[ulActionId];
	}

	s_apstSrchRst_Contents[ulActionId] = (MgrSearch_ResultContext_t *)OxAP_Malloc (sizeof(MgrSearch_ResultContext_t));
	if (NULL != s_apstSrchRst_Contents[ulActionId])
	{
		HxSTD_memset (s_apstSrchRst_Contents[ulActionId], 0, sizeof(MgrSearch_ResultContext_t));
	}

	return s_apstSrchRst_Contents[ulActionId];
}

HERROR xmgr_search_CheckAddTsInfoCondition (HUINT32 ulDeliType, DbSvc_TuningInfo_t *pstTune1, DbSvc_TuningInfo_t *pstTune2, HBOOL *pbAddTs, HBOOL *pbUpdateTs)
{
	if ((pstTune1 == NULL) || (pstTune2 == NULL) || (pbAddTs == NULL) || (pbUpdateTs == NULL))
	{
		return ERR_FAIL;
	}

	// set default return value
	*pbAddTs = FALSE;
	*pbUpdateTs = FALSE;

	// check tuning paramca
	if (DB_SVC_UtilCompareDbTuningParam(ulDeliType, pstTune1, pstTune2) != 0)
	{
		// tuning 정보가 바뀌었으므로 update ts info
		*pbUpdateTs = TRUE;

#if defined(CONFIG_OP_MIDDLE_EAST)
		// Middle East Satellite 에서는 OnId, TsId 가 동일한 TP 가 존재함(BADR - Freq11900, Freq12322). Delivery Info 가 다르면 다른 TS 로 Add 하여 예외 처리함. - mhoh
		if (pstTune1->sat.ulFrequency != pstTune2->sat.ulFrequency)
		{
			*pbAddTs = TRUE;
			*pbUpdateTs = FALSE;
		}
#endif
	}

	return ERR_OK;
}


/*********************** End of File ******************************/
