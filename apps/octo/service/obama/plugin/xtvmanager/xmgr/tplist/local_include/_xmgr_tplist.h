/**
	@file     _xmgr_tplist.h
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

#ifndef	__XMGR_TPLIST_INT_H__
#define	__XMGR_TPLIST_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <db_svc.h>
#include <stb_params.h>
#include <mgr_tplist.h>
#include <pmgr_tplist.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR	xmgr_tplist_GetAntennaIndex (SVC_SatType_t eSatType, HUINT32 *pulAntIdx);
HERROR	xmgr_tplist_GetCabHomeTuningInfoList(DbSvc_CabTuningInfo_t **pastList, HUINT32 *pulNumList);
HERROR	xmgr_tplist_GetTerHomeTuningInfoList(DbSvc_TerTuningInfo_t **pastList, HUINT32 *pulNumList);

HERROR	xmgr_tplist_GetSatHomeTuningInfoList_Base(DbSvc_SatTuningInfo_t **pastList, HUINT32 *pulNumList);
HERROR	xmgr_tplist_GetCabHomeTuningInfoList_Base(DbSvc_CabTuningInfo_t **pastList, HUINT32 *pulNumList);
HERROR	xmgr_tplist_GetTerHomeTuningInfoList_Base(DbSvc_TerTuningInfo_t **pastList, HUINT32 *pulNumList);

//	CONFIG_MW_CH_SATELLITE
HERROR	xmgr_tplist_GetSatHomeTuneInfo_Base (SVC_SatType_t eSatType, DbSvc_TuningInfo_t *pstTuningInfo, HBOOL bCardDefault);

//	CONFIG_MW_CH_CABLE
HERROR	xmgr_tplist_GetCabDefaultTpListModel_Base (MgrTplist_Model_e *peModelType);
HERROR	xmgr_tplist_GetCabDefaultTpList_Base (MgrTplist_Model_e eModelType, MgrTplist_Type_e eTplistType, MgrTplist_CabTsInfo_t **ppstTsList, HUINT32 *pulTsCount);
HERROR	xmgr_tplist_GetCabDefaultTpList_Count (MgrTplist_Model_e eModelType, MgrTplist_Type_e eTplistType, HUINT32 *pulTsCount);

// CONFIG_MW_CH_TERRESTRIAL
HERROR	xmgr_tplist_GetDefaultFrequencyTableByCountryIndex_Base (HxCountry_e eCountryId, MgrTplist_TerTsInfo_t **pastTsList, HUINT32 *pulNumTs);
HERROR	xmgr_tplist_ConvDefaultTerFrequencyToTuningInfo_Base (MgrTplist_TerTsInfo_t *pstDefaultFrequency, DbSvc_TuningInfo_t *pstTuningInfo);
HERROR	xmgr_tplist_GetVHFBandIII_Base (HxCountry_e eCountryId, HUINT32 ulFrequency, MgrTplist_TerTsInfo_t *pstTsInfo);

/*	Home TpList
 */
HERROR	xmgr_tplist_GetHomeTpList_Base (MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs);
HERROR	xmgr_tplist_GetHomeTpList_ERTelecom (MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs);
HERROR	xmgr_tplist_GetHomeTpList_HMS1000S (MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs);
HERROR	xmgr_tplist_GetHomeTpList_FVP4000T (MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs);
HERROR  xmgr_tplist_GetHomeTpList_IcordPro (MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs);
HERROR	xmgr_tplist_GetHomeTpList_Tdc (MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs);
HERROR	xmgr_tplist_GetHomeTpList_Ha7100s (MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs);

// CONFIG_OP_TELENOR
HERROR	xmgr_tplist_GetTpListSearch_Telenor (MgrTplist_Type_e eTpListType, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs);

//	CONFIG_OP_FREESAT
HERROR	xmgr_tplist_GetHomeTpList_Freesat (MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs);

#endif /* __XMGR_TPLIST_INT_H__ */

