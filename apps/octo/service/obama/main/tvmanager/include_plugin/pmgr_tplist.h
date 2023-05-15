/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_tplist.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_TPLIST_H___
#define	___PMGR_TPLIST_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_tplist.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Plug-In Functions    ********************/
/*******************************************************************/

HERROR	pmgr_tplist_GetHomeTsList(DxOperator_e eOperatorType, MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **pastTsList, HUINT32 *pulNumTs);

// CONFIG_MW_CH_CABLE
HERROR	pmgr_tplist_GetDefaultCabTpListModel (MgrTplist_Model_e *peModelType);
HERROR	pmgr_tplist_GetDefaultCabTpList (MgrTplist_Model_e eModelType, MgrTplist_Type_e eSearchMode, MgrTplist_CabTsInfo_t **ppstTsList, HUINT32 *pulTsCount);
HERROR	pmgr_tplist_GetDefaultCabTpListCount(MgrTplist_Model_e eModelType, MgrTplist_Type_e eTplistType, HUINT32 *pulTsCount);
HERROR	pmgr_tplist_GetCabSearchTsList(DxOperator_e eOperatorType, MgrTplist_Type_e eTpListType, DbSvc_TsInfo_t **pastTsList, HUINT32 *pulNumTs);

// CONFIG_MW_CH_SATELLITE
HERROR	pmgr_tplist_GetSatHomeTuneInfo (SVC_SatType_t eSatType, DbSvc_TuningInfo_t *pstTuningInfo, HBOOL bCardDefault);

// CONFIG_MW_CH_TERRESTRIAL
HERROR	pmgr_tplist_GetDefaultFrequencyTableByCountryIndex (HxCountry_e eCountryId, MgrTplist_TerTsInfo_t **ppstFreqTable, HUINT32 *pulMaxFreqNum);
HERROR	pmgr_tplist_GetVHFBandIII (HxCountry_e eCountryId, HUINT32 ulFrequency, MgrTplist_TerTsInfo_t *pstTerTsInfo);



#endif /* !___PMGR_TPLIST_H___ */

