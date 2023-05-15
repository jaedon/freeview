/**
	@file     default_cab_info.c
	@brief    file_name & simple comment.

	Description: dimension header file for kraken \n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2008/07/16		init					wjmoh\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


#include <string.h>

#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_ch.h>
#include <db_svc.h>
#include <db_param.h>

#include <mgr_search.h>
#include "local_include/_mgr_tplist.h"


#define _____INTERNAL_FUNC_____


// 향이 추가될 수록 다음과 같이 TP List를 추가해서 구현하도록 한다.

#define _____EXT_DEFAULT_TP_____

HERROR MGR_TPLIST_CabGetFrequencyModel (MgrTplist_Model_e *peModelType)
{
	if (NULL == peModelType)
	{
		return ERR_FAIL;
	}

	return mgr_tplist_GetDefaultCabTpListModel(peModelType);
}

HERROR MGR_TPLIST_CabGetDefaultTuningInfoCount (MgrTplist_Model_e eModelType, MgrTplist_Type_e eTplistType, HUINT32 *pulNumOfTps)
{
	if (NULL == pulNumOfTps)
	{
		return ERR_FAIL;
	}

	return mgr_tplist_GetDefaultCabTpListCount(eModelType, eTplistType, pulNumOfTps);
}

HERROR MGR_TPLIST_CabGetDefaultTuningInfo (MgrTplist_Model_e eModelType, MgrTplist_Type_e eTplistType, HUINT32 ulIndex, DbSvc_TuningInfo_t *pstTuneInfo)
{
	HUINT32					 ulTotalNum;
	MgrTplist_CabTsInfo_t	*pstTsList;
	HERROR					 hErr;
	HUINT16					 usNetworkId;

	if (NULL == pstTuneInfo)
	{
		return ERR_FAIL;
	}

	pstTsList = NULL;
	ulTotalNum = 0;
	hErr = mgr_tplist_GetDefaultCabTpList(eModelType, eTplistType, &pstTsList, &ulTotalNum);
	if (ERR_OK != hErr || NULL == pstTsList || ulTotalNum <= ulIndex)
	{
		return ERR_FAIL;
	}

	pstTuneInfo->cab.ulFreq	= pstTsList[ulIndex].freq;
	pstTuneInfo->cab.usSR	= (HUINT16)pstTsList[ulIndex].SR;
	pstTuneInfo->cab.ucCon	= (HUINT8)pstTsList[ulIndex].constellation;
	pstTuneInfo->cab.ucSpec	= (HUINT8)eDxCAB_SPECTRUM_AUTO;

	usNetworkId = 0;
	MGR_SEARCH_GetNetworkIdAndSearchParam(&usNetworkId, NULL, NULL);
	pstTuneInfo->cab.usNetworkId	= usNetworkId; // Network Search 를 위해 초기화 한다.

	return ERR_OK;
}


/* end of file */

