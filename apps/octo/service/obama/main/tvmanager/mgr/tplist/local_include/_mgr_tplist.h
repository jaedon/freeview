/**
	@file     _mgr_tplist.h
	@brief    file_name & simple comment.

	Description :			\n
	Module :				\n
	Remarks :				\n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/04/12		initiated					moonsg\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MGR_TPLIST_INT_H__
#define	__MGR_TPLIST_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <db_svc.h>
#include <stb_params.h>
#include <mgr_tplist.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
// CONFIG_MW_CH_CABLE
HERROR	mgr_tplist_GetDefaultCabTpListModel(MgrTplist_Model_e *peModelType);
HERROR	mgr_tplist_GetDefaultCabTpList(MgrTplist_Model_e eModelType, MgrTplist_Type_e eTplistType, MgrTplist_CabTsInfo_t **ppstTsList, HUINT32 *pulTsCount);
HERROR	mgr_tplist_GetDefaultCabTpListCount (MgrTplist_Model_e eModelType, MgrTplist_Type_e eTplistType, HUINT32 *pulTsCount);

// CONFIG_MW_CH_SATELLITE
HERROR	mgr_tplist_GetDefaultSatTpListCount (SVC_SatType_t eSatType, HUINT32 ulTpIdx, DbSvc_TuningInfo_t *pstTuningInfo);
HERROR	mgr_tplist_GetDefaultSatTpList (SVC_SatType_t eSatType, HUINT32 ulTpIdx, DbSvc_TuningInfo_t *pstTuningInfo);

HERROR	mgr_tplist_GetSatHomeTuneInfo (SVC_SatType_t eSatType, DbSvc_TuningInfo_t *pstTuningInfo, HBOOL bCardDefault);


// CONFIG_MW_CH_TERRESTRIAL
HERROR	mgr_tplist_GetDefaultFrequencyTableByCountryIndex (HxCountry_e eCountryId, MgrTplist_TerTsInfo_t **pastTsList, HUINT32 *pulNumTs);
HERROR	mgr_tplist_GetVHFBandIII (HxCountry_e eCountryId, HUINT32 ulFrequency, MgrTplist_TerTsInfo_t *pstTsInfo);

#endif /* __MGR_TPLIST_INT_H__ */

