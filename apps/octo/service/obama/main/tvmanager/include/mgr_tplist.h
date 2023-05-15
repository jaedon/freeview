/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_tplist.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_TPLIST_H___
#define	___MGR_TPLIST_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define	MAX_TER_CHANNEL_NAME_LEN		5


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

// Satellite Sorting Method
typedef enum
{
	eSATSORT_EU_NAME		= 0,
	eSATSORT_GERNAN,

	// Spec이 추가되면 위에 계속 붙이고 처리한다.
	eSATSORT_MAX
} MgrTplist_SatSortMethod_e;

typedef enum
{
	eHomeTs_Search,
	eHomeTs_OTA,
	eHomeTs_Max,
} MgrTplist_HomeTsKind_e;

typedef struct _CABSINFO
{
	HUINT32				freq;
	HUINT32				SR;
	DxCab_Constellation_e	constellation;
} MgrTplist_CabTsInfo_t;

typedef	struct
{
	HUINT8	ucChannelNo;
	HUINT8	ucSystem;
	HUINT8	ucBw;
	HUINT32 ulFreq;
	HUINT8	szChannelName[MAX_TER_CHANNEL_NAME_LEN];
} MgrTplist_TerTsInfo_t;

typedef enum
{
	eTPLIST_MODEL_DEFAULT,
	eTPLIST_MODEL_GERMAN_BASE,
	eTPLIST_MODEL_KDG_DEFAULT,
	eTPLIST_MODEL_KDG_BASE,
	eTPLIST_MODEL_SKYD_DEFAULT,
	eTPLIST_MODEL_SKYD_BASE,
	eTPLIST_MODEL_FINLAND_BASE,
	eTPLIST_MODEL_ZIGGO_DEFAULT,
	eTPLIST_MODEL_ZIGGO_BASE,
	eTPLIST_MODEL_ROMANIA_BASE,
	eTPLIST_MODEL_OPENCABLE_BASE,
	eTPLIST_MODEL_JCOM,
	eTPLIST_MODEL_MOSTELECOM_DEFAULT,
	eTPLIST_MODEL_MOSTELECOM_BASE,
	eTPLIST_MODEL_TVT_DEFAULT,
	eTPLIST_MODEL_TVT_BASE,
	eTPLIST_MODEL_TELENOR,
	eTPLIST_MODEL_TDC,

	eTPLIST_MODEL_MAXNUM
} MgrTplist_Model_e;


// Cable Search Mode : Linear, Auto 등
typedef enum
{
	eTPLIST_TYPE_AUTO,				// Auto Search TP :
	eTPLIST_TYPE_LINEAR,			// Linear Search TP : for cab and ter

	eSEARCH_MODE_MAX
} MgrTplist_Type_e;


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/*		Home Tp List
 */
HERROR				MGR_TPLIST_GetHomeTsList (DxOperator_e eOpType, MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **pastTsList, HUINT32 *pulNumTs);


/*		Tune Info
 */
HERROR				MGR_TPLIST_ClearTuneInfo (void);
HERROR				MGR_TPLIST_AddTuneInfo (DbSvc_TuningInfo_t *pstTuneInfo);
HERROR				MGR_TPLIST_GetFirstTuneInfo (DbSvc_TuningInfo_t **ppstTuneInfo);
HERROR				MGR_TPLIST_RemoveFirstTuneInfo (HBOOL bFreeContents);

/*		TS Info
 */
HERROR				MGR_TPLIST_ClearTsInfo (void);
HERROR				MGR_TPLIST_AddTsInfo (DbSvc_TsInfo_t *pstTsInfo);
HERROR				MGR_TPLIST_CountTsInfo (HUINT32 *pulTpNum);
HERROR				MGR_TPLIST_GetTsInfo (HUINT32 ulIndex, DbSvc_TsInfo_t *pstTsInfo);
HERROR				MGR_TPLIST_GetTsList (HUINT32 *pulNumTp, DbSvc_TsInfo_t **ppstTsList);
HERROR				MGR_TPLIST_FreeTsList (DbSvc_TsInfo_t *pstTsList);


/*		Default TP
 *		Dependancy : if not defined : CONFIG_MW_CH_SATELLITE
 *					 return ERR_FAIL
 */
HERROR 				MGR_TPLIST_SatGetHomeTuneInfo (SVC_SatType_t eSatType, DbSvc_TuningInfo_t *pstTuningInfo, HBOOL bCardDefault);

HERROR				MGR_TPLIST_SatGetSatelliteSortMethod (MgrTplist_SatSortMethod_e *peSortMethod);
HERROR				MGR_TPLIST_SatSetSatelliteSortMethod (MgrTplist_SatSortMethod_e eSortMethod);


/*		Default TP (Ter)
 *		Dependancy : if not defined : CONFIG_MW_CH_TERRESTRIAL
 *					 return ERR_FAIL
 */
HERROR				MGR_TPLIST_TerCountDefaultTpList (HxCountry_e eCountryId, HUINT32 *pulTpNum);
HERROR				MGR_TPLIST_TerGetDefaultTpList (HxCountry_e eCountryId, HUINT32 ulTpIdx, DbSvc_TuningInfo_t *pstTuneInfo);
HERROR				MGR_TPLIST_TerGetDefaultChNum (HxCountry_e eCountryId, HUINT32 ulTpIdx, HUINT32 *pulChNum, HUINT8 *pucBW);
HERROR				MGR_TPLIST_TerGetTpIdxByChNum (HxCountry_e eCountryId, HUINT32 ulChNum, HUINT32 *pulTpIdx);
HERROR 				MGR_TPLIST_TerGetTpIdxByFreqency (HxCountry_e eCountryId, HUINT32 ulFrequency, HUINT32 *pulTpIdx);
HERROR				MGR_TPLIST_TerGetDefaultChName (HxCountry_e eCountryId, HUINT32 ulTpIdx, HUINT8 *szChanName, HINT32 nLen);
HERROR				MGR_TPLIST_TerCheckFreqRangeValid (HxCountry_e eCountryId, HUINT32 ulUserFreq);
HERROR				MGR_TPLIST_TerGetCenterFreqByUserFreq (HxCountry_e eCountryId, HUINT32 ulUserFreq, HUINT32 *pulCenterFreq);
HERROR				MGR_TPLIST_TerGetChNumByFrequency (HxCountry_e eCountryId, HUINT32 ulFrequency, HUINT32 *pulChNum, HUINT8 * pucBW);
HERROR				MGR_TPLIST_TerGetChNameByFrequency (HxCountry_e eCountryId, HUINT32 ulFrequency, HUINT8 *szChanName);
HERROR				MGR_TPLIST_TerGetFrequencyByChNum (HxCountry_e eCountryId, HUINT32 ulChNum, HUINT32 *pulFrequency, HUINT8 * pucBW);
HERROR				MGR_TPLIST_TerGetFrequencyByTpIndex (HxCountry_e eCountryId, HUINT32 ulTpIdx, HUINT32 *pulFrequency, HUINT8 * pucBW, HUINT32 *pulChNum);
HERROR				MGR_TPLIST_TerGetFirstAndLastChNum (HxCountry_e eCountryId, HUINT32 *ulFirstChNum, HUINT32 *ulLastChNum);
HERROR				MGR_TPLIST_TerGetFullInfoByChNum (HxCountry_e eCountryId, HUINT32 ulChNum,HUINT32 *pulFrequency, HUINT8 * pucBW, HUINT8 * pucSystem);

/*		Default TP (Cab)
 *		Dependancy : if not defined : CONFIG_MW_CH_CABLE
 *					 return ERR_FAIL
 */
HERROR				MGR_TPLIST_CabGetFrequencyModel (MgrTplist_Model_e *peModelType);
HERROR				MGR_TPLIST_CabGetDefaultTuningInfoCount (MgrTplist_Model_e eModelType, MgrTplist_Type_e eSearchMode, HUINT32 *pulNumOfTps);
HERROR				MGR_TPLIST_CabGetDefaultTuningInfo (MgrTplist_Model_e eModelType, MgrTplist_Type_e eSearchMode, HUINT32 ulIndex, DbSvc_TuningInfo_t *pstTuneInfo);

HERROR				MGR_TPLIST_GetCabSearchTsList (DxOperator_e eOpType, MgrTplist_Type_e eSearchMode, DbSvc_TsInfo_t **pastTsList, HUINT32 *pulNumTs);


#endif /* !___MGR_TPLIST_H___ */

