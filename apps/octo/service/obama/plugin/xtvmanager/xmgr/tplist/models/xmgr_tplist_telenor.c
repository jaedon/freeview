/**
	@file     	xmgr_tplist_telenor.c
	@brief

	Description:  			\n
	Module: 					 					\n
	Remarks : 										\n

	Copyright (c) 2015 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_common.h>
#include <mgr_tplist.h>
#include <mgr_search.h>

#include "../local_include/_xmgr_tplist.h"

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

HERROR xmgr_tplist_GetTpListSearch_Telenor (MgrTplist_Type_e eTpListType, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs)
{
	HUINT32					 i, ulNumTs;
	DbSvc_TsInfo_t			*astTsInfo = NULL;
	HERROR					 hErr = ERR_FAIL;
	MgrTplist_Model_e		 eModelType = eTPLIST_MODEL_DEFAULT;
	HUINT32					 ulNumOfTps = 0;

	if (NULL == pulNumTs)		return ERR_FAIL;
	if (NULL == ppstTsList)		return ERR_FAIL;

	MGR_TPLIST_CabGetFrequencyModel(&eModelType);
	HxLOG_Print("freq eModelType = %d\n", eModelType);

	hErr = MGR_TPLIST_CabGetDefaultTuningInfoCount(eModelType, eTpListType, &ulNumOfTps);
	if ((ERR_OK != hErr) || (0 == ulNumOfTps))
		goto err_rtn;

	astTsInfo = (DbSvc_TsInfo_t *)HLIB_STD_MemAlloc(sizeof(DbSvc_TsInfo_t)*ulNumOfTps);
	if (NULL == astTsInfo)
		goto err_rtn;

	HxSTD_MemSet(astTsInfo, 0, sizeof(DbSvc_TsInfo_t)*ulNumOfTps);

	ulNumTs = 0;
	for (i = 0; i < ulNumOfTps; i++)
	{
		hErr = MGR_TPLIST_CabGetDefaultTuningInfo(eModelType, eTpListType, i, &astTsInfo[ulNumTs].stTuningParam);
		if (hErr != ERR_OK)
			continue;
		astTsInfo[ulNumTs].eDeliType = eDxDELIVERY_TYPE_CAB;
		ulNumTs++;
	}

	if (0 == ulNumTs)
		goto err_rtn;

	*ppstTsList = astTsInfo;
	*pulNumTs = ulNumTs;

	return (0 == ulNumTs) ? ERR_FAIL : ERR_OK;

err_rtn:
	if(NULL != astTsInfo)	HLIB_STD_MemFree(astTsInfo);
	return ERR_FAIL;
}

/*********************** End of File ******************************/
