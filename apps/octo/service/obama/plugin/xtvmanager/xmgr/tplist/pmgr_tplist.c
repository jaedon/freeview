/**
	@file     	pmgr_search.c
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
#include <mgr_appmgr.h>
#include <mgr_tplist.h>
#include <xmgr_tplist.h>
#include "./local_include/_xmgr_tplist.h"

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

HERROR	pmgr_tplist_GetDefaultCabTpListModel(MgrTplist_Model_e *peModelType)
{
	HERROR	hErr = ERR_FAIL;

#if defined(CONFIG_MW_CH_CABLE)
	if (NULL == peModelType)
		return ERR_FAIL;

	hErr = xmgr_tplist_GetCabDefaultTpListModel_Base(peModelType);
#endif

	return hErr;
}

HERROR	pmgr_tplist_GetDefaultCabTpList(MgrTplist_Model_e eModelType, MgrTplist_Type_e eTplistType, MgrTplist_CabTsInfo_t **ppstTsList, HUINT32 *pulTsCount)
{
	HERROR	hErr = ERR_FAIL;

#if defined(CONFIG_MW_CH_CABLE)
	hErr = xmgr_tplist_GetCabDefaultTpList_Base(eModelType, eTplistType, ppstTsList, pulTsCount);
#endif

	return hErr;
}

HERROR	pmgr_tplist_GetDefaultCabTpListCount(MgrTplist_Model_e eModelType, MgrTplist_Type_e eTplistType, HUINT32 *pulTsCount)
{
	HERROR	hErr = ERR_FAIL;

#if defined(CONFIG_MW_CH_CABLE)
	hErr = xmgr_tplist_GetCabDefaultTpList_Count(eModelType, eTplistType, pulTsCount);
#endif

	return hErr;
}

HERROR pmgr_tplist_GetSatHomeTuneInfo (SVC_SatType_t eSatType, DbSvc_TuningInfo_t *pstTuningInfo, HBOOL bCardDefault)
{
	HERROR	hErr = ERR_FAIL;

#if defined(CONFIG_MW_CH_SATELLITE)
	if (NULL == pstTuningInfo)
		return ERR_FAIL;

	hErr = xmgr_tplist_GetSatHomeTuneInfo_Base (eSatType, pstTuningInfo, bCardDefault);
#endif

	return hErr;
}

HERROR	pmgr_tplist_GetHomeTsList(DxOperator_e eOperatorType, MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **pastTsList, HUINT32 *pulNumTs)
{
	HERROR			 hErr;
	DbSvc_TsInfo_t 	*astTsList;
	HUINT32			 ulNumTs;

	if (NULL == pastTsList)		return ERR_FAIL;
	if (NULL == pulNumTs)		return ERR_FAIL;

	hErr = ERR_FAIL;
	astTsList = NULL;
	ulNumTs = 0;
	switch (eOperatorType)
	{
	case eDxOPERATOR_ERtelecom:
#if defined(CONFIG_OP_ERTELECOM)
		hErr = xmgr_tplist_GetHomeTpList_ERTelecom(eHomeTsKind, &astTsList, &ulNumTs);
#endif
		break;

	case eDxOPERATOR_Ziggo:
	case eDxOPERATOR_Comhem:
	case eDxOPERATOR_KDG:
	case eDxOPERATOR_SkyD:
		break;
	case eDxOPERATOR_Ses:
#if defined(CONFIG_OP_SES)
		// TODO: 아래 함수와 파일을 SES로 고치길 권장 합니다.
		hErr = xmgr_tplist_GetHomeTpList_IcordPro(eHomeTsKind, &astTsList, &ulNumTs);
#endif
		break;
	case eDxOPERATOR_FiCom:
	case eDxOPERATOR_CanalReady:
	case eDxOPERATOR_Rcs:
	case eDxOPERATOR_Freesat:
	case eDxOPERATOR_NtvPlus:
	case eDxOPERATOR_UPC:
	case eDxOPERATOR_Digiturk:
	case eDxOPERATOR_ADTV:
	case eDxOPERATOR_Aljazeera:
	case eDxOPERATOR_CNE:
	case eDxOPERATOR_SpainTdt:
	case eDxOPERATOR_Truevisions:
	case eDxOPERATOR_Boxer:
	case eDxOPERATOR_TIVU:
	case eDxOPERATOR_JCOM:
	case eDxOPERATOR_SKAPA:
	case eDxOPERATOR_Mostelecom:
	case eDxOPERATOR_SwissCanal:
	case eDxOPERATOR_RussiaTvt:
	case eDxOPERATOR_Almajd:
	case eDxOPERATOR_NOVA:
	case eDxOPERATOR_SkyCable:
	case eDxOPERATOR_GermanCab:
	case eDxOPERATOR_Primacom:
	case eDxOPERATOR_VTC:
	case eDxOPERATOR_MiddleEast:
		break;

	case eDxOPERATOR_Tdc:
#if defined(CONFIG_OP_TDC)
		hErr = xmgr_tplist_GetHomeTpList_Tdc(eHomeTsKind, &astTsList, &ulNumTs);
#endif
		break;

	case eDxOPERATOR_Telenor:
		hErr = xmgr_tplist_GetHomeTpList_Base(eHomeTsKind, &astTsList, &ulNumTs);
		break;

	case eDxOPERATOR_BBC:
#if defined(CONFIG_PROD_FVP4000T)
		hErr = xmgr_tplist_GetHomeTpList_FVP4000T(eHomeTsKind, &astTsList, &ulNumTs);
#endif
		break;
	case eDxOPERATOR_AMX:
#if defined(CONFIG_PROD_HA7100S)
		hErr = xmgr_tplist_GetHomeTpList_Ha7100s(eHomeTsKind, &astTsList, &ulNumTs);
#endif
		break;
	case eDxOPERATOR_None:
#if defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
		hErr = xmgr_tplist_GetHomeTpList_HMS1000S(eHomeTsKind, &astTsList, &ulNumTs);
#endif
		break;


	case eDxOPERATOR_NOT_DEFINED:
	default:
		hErr = xmgr_tplist_GetHomeTpList_Base(eHomeTsKind, &astTsList, &ulNumTs);
		break;
	}

	HxLOG_Print("\n[pmgr_tplist_GetHomeTpList] ============ (+) eHomeTsKind(%d) eOperatorType(%d) hErr(0x%x) astTsList(0x%x) ulNumTs(%d) \n\n", eHomeTsKind, eOperatorType , hErr, (HUINT32)astTsList, ulNumTs);

	if (ERR_OK != hErr)
	{
		goto err_exit;
	}

	if (pastTsList)		*pastTsList	= astTsList;
	if (pulNumTs)		*pulNumTs		= ulNumTs;

	return hErr;

err_exit:
	if (astTsList)
	{
		HLIB_STD_MemFree(astTsList);
		astTsList = NULL;
	}

	if (pastTsList)		*pastTsList	= NULL;
	if (pulNumTs)		*pulNumTs		= 0;

	return ERR_FAIL;
}

HERROR	pmgr_tplist_GetCabSearchTsList(DxOperator_e eOperatorType, MgrTplist_Type_e eTpListType, DbSvc_TsInfo_t **pastTsList, HUINT32 *pulNumTs)
{
	switch (eOperatorType)
	{
#ifdef CONFIG_OP_TELENOR
	case eDxOPERATOR_Telenor:
		return xmgr_tplist_GetTpListSearch_Telenor(eTpListType, pastTsList, pulNumTs);
#elif CONFIG_OP_TDC
	case eDxOPERATOR_Tdc:
		return xmgr_tplist_GetTpListSearch_Tdc(eTpListType, pastTsList, pulNumTs);
#endif

	default:
		break;
	}

	HxLOG_Warning("tp list is not defined - eOperatorType(%d), eTpListType(%d)\n", eOperatorType, eTpListType);
	return ERR_FAIL;
}

HERROR	pmgr_tplist_GetDefaultFrequencyTableByCountryIndex (HxCountry_e eCountryId, MgrTplist_TerTsInfo_t **pastTsList, HUINT32 *pulNumTs)
{
	HERROR	hErr = ERR_FAIL;

	if (NULL == pulNumTs)		return ERR_FAIL;

#if defined(CONFIG_MW_CH_TERRESTRIAL)
	hErr = xmgr_tplist_GetDefaultFrequencyTableByCountryIndex_Base (eCountryId, pastTsList, pulNumTs);
#endif

	return hErr;
}

HERROR	pmgr_tplist_GetVHFBandIII (HxCountry_e eCountryId, HUINT32 ulFrequency, MgrTplist_TerTsInfo_t *pstTsInfo)
{
	HERROR	hErr = ERR_FAIL;

	if (NULL == pstTsInfo)		return ERR_FAIL;

#if defined(CONFIG_MW_CH_TERRESTRIAL)
	hErr = xmgr_tplist_GetVHFBandIII_Base (eCountryId, ulFrequency, pstTsInfo);
#endif

	return hErr;
}



/*********************** End of File ******************************/
