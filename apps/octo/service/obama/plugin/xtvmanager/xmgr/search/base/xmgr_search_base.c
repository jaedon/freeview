/**
	@file     	xmgr_search_base.c
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
#include <pmgr_search.h>
#include <xmgr_search.h>
#include "../local_include/_xmgr_search.h"
#include <mgr_spec.h>
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

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR xmgr_search_SetSearchMode_Base (MgrSearch_NetType_e eSrcNetType, MgrSearch_TpType_e eSrcTpType, SVC_SatType_t eSatType, DbSvc_TuningInfo_t *pTuningInfo)
{
	if (NULL == pTuningInfo)
		return ERR_FAIL;

	return ERR_FAIL;
}

HERROR	xmgr_search_GetNetworkIdAndSearchParam_Base (HUINT16 *pusNetId, HUINT32 *pulStartFreq, HUINT32 *pulStopFreq)
{
	HERROR		hErr = ERR_FAIL;
	HUINT16 	usNetId;
	HUINT32		ulStartFreq, ulStopFreq;

	if (NULL == pusNetId)
		return ERR_FAIL;

	hErr = ERR_FAIL;

	usNetId = 0;
	ulStartFreq = 51000;
	ulStopFreq = 858000;
	hErr = ERR_OK;

	*pusNetId = usNetId;
	if (pulStartFreq)
	*pulStartFreq = ulStartFreq;
	if (pulStopFreq)
	*pulStopFreq = ulStopFreq;

	return hErr;
}

/* FLD 등 다른 주파수 TP 사이의 '동일한 (On/Ts/SvcId 동일)' 서비스들을 추가할 것인가 를 정하는 함수 */
/* ON/TS ID가 같은 TsInfo 이지만 Frequency가 다르다면 기본적으로는 추가시키지 말고 TsInfo를 Update 해 줘야 한다. */
/* 특정 지상파 향에 따라 강제로 추가하는 걸 허락할 수 있어야 하기 때문에 이를 사용한다. */
HBOOL	xmgr_search_IsTsInfoAppendableForSameOnTsIdAndDiffTuneInfo_Base (HUINT32 ulActionId, DxDeliveryType_e eDeliType)
{
	// TODO: 필요시 구현하세요~
	return FALSE;
}

HERROR xmgr_search_ResultCountService_Base (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 *pulSvcNum)
{
	MgrSearch_ResultContext_t		 *pstContents;

	if (NULL == pulSvcNum)									{ return ERR_FAIL; }

	pstContents = xmgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	if (eDxSVC_TYPE_All == eSvcType)
	{
		*pulSvcNum = pstContents->ulNumSvc;
	}
	else
	{
		HUINT32 	 ulCount, ulSvcNum = 0;

		for (ulCount = 0; ulCount < pstContents->ulNumSvc; ulCount++)
		{
			if ((DbSvc_GetSvcType(&pstContents->pstSvcArray[ulCount]) & eSvcType) != 0)
			{
				ulSvcNum ++;
			}
		}

		*pulSvcNum = ulSvcNum;
	}

	return ERR_OK;
}


HERROR xmgr_search_ResultGetService_Base (HUINT32 ulActionId, DxSvcType_e eSvcType, HUINT32 ulIndex, DbSvc_Info_t *pstSvcInfo)
{
	HUINT32					 ulCount, ulSvcCnt = 0;
	MgrSearch_ResultContext_t	*pstContents;

	if (NULL == pstSvcInfo)									{ return ERR_FAIL; }

	pstContents = xmgr_search_ResultGetContext (ulActionId);
	if (NULL == pstContents)								{ return ERR_FAIL; }

	for (ulCount = 0, ulSvcCnt = 0; ulCount < pstContents->ulNumSvc; ulCount++)
	{
		if ((eDxSVC_TYPE_All == eSvcType) ||
			((DbSvc_GetSvcType(&pstContents->pstSvcArray[ulCount]) & eSvcType) != 0))
		{
			if (ulIndex == ulSvcCnt)
			{
				*pstSvcInfo = pstContents->pstSvcArray[ulCount];
				return ERR_OK;
			}

			ulSvcCnt ++;
		}
	}

	return ERR_FAIL;
}

/*********************** End of File ******************************/
