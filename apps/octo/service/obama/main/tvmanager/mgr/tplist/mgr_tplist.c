/**
	@file     mgr_tplist.c
	@brief

	Description :			\n
	Module :				\n
	Remarks :				\n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/04/16		init							\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <string.h>

#include <db_svc.h>
#include <mgr_tplist.h>
#include <pmgr_tplist.h>
#include "./local_include/_mgr_tplist.h"

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
/********************      Functions            ********************/
/*******************************************************************/

HERROR	mgr_tplist_GetDefaultCabTpListModel (MgrTplist_Model_e *peModelType)
{
	return pmgr_tplist_GetDefaultCabTpListModel(peModelType);
}

HERROR	mgr_tplist_GetDefaultCabTpList (MgrTplist_Model_e eModelType, MgrTplist_Type_e eTplistType, MgrTplist_CabTsInfo_t **ppstTsList, HUINT32 *pulTsCount)
{
	return pmgr_tplist_GetDefaultCabTpList(eModelType, eTplistType, ppstTsList, pulTsCount);
}

HERROR	mgr_tplist_GetDefaultCabTpListCount (MgrTplist_Model_e eModelType, MgrTplist_Type_e eTplistType, HUINT32 *pulTsCount)
{
	return pmgr_tplist_GetDefaultCabTpListCount(eModelType, eTplistType, pulTsCount);
}

HERROR	mgr_tplist_GetSatHomeTuneInfo (SVC_SatType_t eSatType, DbSvc_TuningInfo_t *pstTuningInfo, HBOOL bCardDefault)
{
	return pmgr_tplist_GetSatHomeTuneInfo (eSatType, pstTuningInfo, bCardDefault);
}

HERROR	mgr_tplist_GetDefaultFrequencyTableByCountryIndex (HxCountry_e eCountryId, MgrTplist_TerTsInfo_t **pastTsList, HUINT32 *pulNumTs)
{
	return pmgr_tplist_GetDefaultFrequencyTableByCountryIndex (eCountryId, pastTsList, pulNumTs);
}

HERROR	mgr_tplist_GetVHFBandIII (HxCountry_e eCountryId, HUINT32 ulFrequency, MgrTplist_TerTsInfo_t *pstTsInfo)
{
	return pmgr_tplist_GetVHFBandIII (eCountryId, ulFrequency, pstTsInfo);
}

HERROR	MGR_TPLIST_GetHomeTsList(DxOperator_e eOpType, MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **pastTsList, HUINT32 *pulNumTs)
{
	return pmgr_tplist_GetHomeTsList(eOpType, eHomeTsKind, pastTsList, pulNumTs);
}

HERROR	MGR_TPLIST_GetCabSearchTsList (DxOperator_e eOpType, MgrTplist_Type_e eSearchMode, DbSvc_TsInfo_t **pastTsList, HUINT32 *pulNumTs)
{
	return pmgr_tplist_GetCabSearchTsList(eOpType, eSearchMode, pastTsList, pulNumTs);
}

/* end of file */

