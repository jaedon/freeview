/**
	@file     	xmgr_tplist_hms1000s.c
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
#define ONLY_OTA_CHECK_IN_SEARCHED_TP	//OTA 체크시에 searh된 TP만 검색하도록 함.

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR	xmgr_tplist_GetSatHomeTuningInfoList_Fvp4000t(DbSvc_SatTuningInfo_t **pastList, HINT32 *pnNumList)
{
#if	defined(CONFIG_MW_CH_SATELLITE)
	static DbSvc_SatTuningInfo_t	 s_stTuneInfo_Hms1000s_ASTRA[] =
	{
		{	12603,	22000,	eDxSAT_POLAR_HOR,	eDxSAT_CODERATE_AUTO,	eDxSAT_TRANS_DVBS, eDxSAT_PSK_QPSK, 0,	0,	0},
		{	12552,	22000,	eDxSAT_POLAR_VER,	eDxSAT_CODERATE_AUTO,	eDxSAT_TRANS_DVBS, eDxSAT_PSK_QPSK, 0,	0,	0},
		{	10832,	22000,	eDxSAT_POLAR_HOR,	eDxSAT_CODERATE_AUTO,	eDxSAT_TRANS_DVBS, eDxSAT_PSK_QPSK, 0,	0,	0}
	};

	*pastList = s_stTuneInfo_Hms1000s_ASTRA;
	*pnNumList = (sizeof(s_stTuneInfo_Hms1000s_ASTRA) / sizeof(DbSvc_SatTuningInfo_t));
	return ERR_OK;
#else
	*pastList = NULL;
	*pnNumList = 0;
	return ERR_FAIL;
#endif
}

STATIC HERROR xmgr_tplist_GetHomeTpListOta_Fvp4000t (DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs)
{
	HUINT32					 ulNumTs;
	DbSvc_TsInfo_t			*astTsInfo = NULL;
	HBOOL					 bAllocList;
	HERROR					 hErr;
	HUINT32 				 ulNumTsTer = 0;
#if defined(ONLY_OTA_CHECK_IN_SEARCHED_TP)
	DbSvc_TsCondition_t stCond;
#else
	HUINT32					 ulCount;
	HxCountry_e				 eCountryId;
#endif

	if (NULL == pulNumTs)		return ERR_FAIL;

	bAllocList = (NULL != ppstTsList) ? TRUE : FALSE;

	// Get Num
	ulNumTs = 0;
	ulNumTsTer = 0;
#if defined(ONLY_OTA_CHECK_IN_SEARCHED_TP)
	DB_SVC_InitTsFindCondition(&stCond);
	stCond.nDeliType = eDxDELIVERY_TYPE_TER;

	hErr = DB_SVC_FindTsInfoList(&stCond, &ulNumTsTer, &astTsInfo);
	if(hErr != ERR_OK || ulNumTsTer == 0)
	{
		*pulNumTs	= 0;

		if(ppstTsList != NULL)
		{
			*ppstTsList = NULL;
		}

		if (astTsInfo != NULL)
		{
			DB_SVC_FreeTsInfoList(astTsInfo);
			astTsInfo = NULL;
		}
		return ERR_FAIL;
	}
	ulNumTs = ulNumTsTer;
#else
	MGR_SEARCHUTIL_GetTerCountryIdx (&eCountryId);
	hErr = MGR_TPLIST_TerCountDefaultTpList (eCountryId, &ulNumTsTer);
	if (hErr != ERR_OK) 				{ ulNumTsTer = 0; }

	ulNumTs += ulNumTsTer;

	if (0 >= ulNumTs)
	{
		*pulNumTs	= 0;
		return ERR_FAIL;
	}

	astTsInfo = NULL;
	if (bAllocList)
	{
		astTsInfo = (DbSvc_TsInfo_t *)HLIB_STD_MemAlloc(sizeof(DbSvc_TsInfo_t)*ulNumTs);
		if (NULL == astTsInfo)
			return ERR_FAIL;

		HxSTD_MemSet(astTsInfo, 0, sizeof(DbSvc_TsInfo_t)*ulNumTs);
		ulNumTs = 0;

		for (ulCount = 0; ulCount < ulNumTsTer; ulCount++)
		{
			hErr = MGR_TPLIST_TerGetDefaultTpList (eCountryId, ulCount, &astTsInfo[ulNumTs].stTuningParam);
			if (hErr != ERR_OK)
				continue;
			astTsInfo[ulNumTs].eDeliType = eDxDELIVERY_TYPE_TER;
			ulNumTs++;
		}
	}

	if (0 == ulNumTs)
	{
		if (astTsInfo)
		{
			HLIB_STD_MemFree(astTsInfo);
			astTsInfo = NULL;
		}
	}
#endif

	if (ppstTsList) *ppstTsList = astTsInfo;
	if (pulNumTs)	*pulNumTs	= ulNumTs;

	return (0 == ulNumTs) ? ERR_FAIL : ERR_OK;
}

HERROR xmgr_tplist_GetHomeTpList_FVP4000T (MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs)
{
	HERROR		hErr = ERR_FAIL;

	switch (eHomeTsKind)
	{
	case eHomeTs_Search:	break;
	case eHomeTs_OTA:		hErr = xmgr_tplist_GetHomeTpListOta_Fvp4000t(ppstTsList, pulNumTs);		break;
	default:				break;
	}

	return hErr;
}

/*********************** End of File ******************************/
