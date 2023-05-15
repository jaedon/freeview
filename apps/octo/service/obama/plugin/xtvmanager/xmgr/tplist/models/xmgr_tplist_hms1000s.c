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

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR	xmgr_tplist_GetSatHomeTuningInfoList_Hms1000s(DbSvc_SatTuningInfo_t **pastList, HINT32 *pnNumList)
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

STATIC HERROR xmgr_tplist_GetHomeTpListOta_HMS1000S (DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs)
{
	HUINT32					 ulCount, ulNumTs;
	DbSvc_TsInfo_t			*astTsInfo;
	HBOOL					 bAllocList;
	HINT32					 nNumList;
	HERROR					 hErr = ERR_FAIL;
#if	defined(CONFIG_MW_CH_SATELLITE)
	HUINT32 				 ulAntIdx;
	HUINT32					 ulNumTsSat = 0;
	DbSvc_SatTuningInfo_t	*astSatList = NULL;
#endif
#if	defined(CONFIG_MW_CH_CABLE)
	HUINT32 				 ulNumTsCab = 0;
	DbSvc_TsCondition_t		 stCond;
	DbSvc_TsInfo_t			*pstTsInfo = NULL;
	DbSvc_CabTuningInfo_t	*astCabList = NULL;
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
	HxCountry_e				 eCountryId;
	HUINT32 				 ulNumTsTer = 0;
	DbSvc_TerTuningInfo_t	*astTerList = NULL;
#endif

	if (NULL == pulNumTs)		return ERR_FAIL;

	bAllocList = (NULL != ppstTsList) ? TRUE : FALSE;

	// Get Num
	ulNumTs = 0;
#if	defined(CONFIG_MW_CH_SATELLITE)
	hErr = xmgr_tplist_GetSatHomeTuningInfoList_Hms1000s(&astSatList, &nNumList);
	if (ERR_OK == hErr && NULL != astSatList && 0 < nNumList)
	{
		ulNumTsSat = 0;
		for (ulCount = 0; ulCount < nNumList; ulCount++)
		{
			if (astSatList[ulCount].ulFrequency == 0) break;
			ulNumTsSat++;
		}
		ulNumTs += ulNumTsSat;
	}
#endif

#if	defined(CONFIG_MW_CH_CABLE)

/*	//original
	ulNumTsCab = 0;
	for (ulCount = 0; ulCount < MAX_HOMETP_NUM; ulCount++)
	{
		if (s_stBaseHomeTuneInfo_Cab[ulCount].ulFreq == 0)	break;
		ulNumTsCab++;
	}
	ulNumTs += ulNumTsCab;

*/
	ulNumTsCab = 0;

	DB_SVC_InitTsFindCondition(&stCond);
	stCond.nDeliType = eDxDELIVERY_TYPE_CAB;

	hErr = DB_SVC_FindTsInfoList(&stCond, &ulNumTsCab, &pstTsInfo);
	if (( hErr != ERR_OK) ||( ulNumTsCab == 0))
	{
		*pulNumTs	= 0;
		return ERR_FAIL;
	}

	ulNumTs += ulNumTsCab;

#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
	{
		ulNumTsTer = 0;
		MGR_SEARCHUTIL_GetTerCountryIdx (&eCountryId);
		hErr = MGR_TPLIST_TerCountDefaultTpList (eCountryId, &ulNumTsTer);
		if (hErr != ERR_OK) 				{ ulNumTsTer = 0; }

		ulNumTs += ulNumTsTer;
	}
#endif

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

#if	defined(CONFIG_MW_CH_SATELLITE)
		hErr = xmgr_tplist_GetAntennaIndex (eSatType_ASTRA_1, &ulAntIdx);
		if (hErr == ERR_OK)
		{
			if (NULL != astSatList && 0 < nNumList)
			{
				for (ulCount = 0; ulCount < ulNumTsSat; ulCount++)
				{
					if (astSatList[ulCount].ulFrequency == 0) break;
					astTsInfo[ulNumTs].eDeliType					= eDxDELIVERY_TYPE_SAT;
					astTsInfo[ulNumTs].stTuningParam.sat			= astSatList[ulCount];
					astTsInfo[ulNumTs].stTuningParam.sat.ucAntId	= (HUINT8)ulAntIdx;
					ulNumTs++;
				}
			}
		}
#endif

#if	defined(CONFIG_MW_CH_CABLE)
		for ( ulCount = 0; ulCount < ulNumTsCab; ulCount++ )
		{
			astTsInfo[ulNumTs].eDeliType			= eDxDELIVERY_TYPE_CAB;
			astTsInfo[ulNumTs].stTuningParam.cab	= pstTsInfo[ulCount].stTuningParam.cab;

			ulNumTs++;

			HxLOG_Print("[%s:%d]add (%d th TP) %dMhz, SR:%d, Netwok ID : %d, Con: %d\n", __FUNCTION__, __LINE__,
				ulCount, pstTsInfo[ulCount].stTuningParam.cab.ulFreq, pstTsInfo[ulCount].stTuningParam.cab.usSR, pstTsInfo[ulCount].stTuningParam.cab.usNetworkId, pstTsInfo[ulCount].stTuningParam.cab.ucCon);

		}
/*
		for (ulCount = 0; ulCount < ulNumTsCab; ulCount++)
		{
			if (0 == s_stBaseHomeTuneInfo_Cab[ulCount].ulFreq)	break;
			astTsInfo[ulNumTs].eDeliType			= eDxDELIVERY_TYPE_CAB;
			astTsInfo[ulNumTs].stTuningParam.cab	= s_stBaseHomeTuneInfo_Cab[ulCount];
			ulNumTs++;
		}
*/
#endif

#if defined(CONFIG_MW_CH_TERRESTRIAL)
		for (ulCount = 0; ulCount < ulNumTsTer; ulCount++)
		{
			hErr = MGR_TPLIST_TerGetDefaultTpList (eCountryId, ulCount, &astTsInfo[ulNumTs].stTuningParam);
			if (hErr != ERR_OK)
				continue;
			astTsInfo[ulNumTs].eDeliType = eDxDELIVERY_TYPE_TER;
			ulNumTs++;
		}
#endif
	}

	if (0 == ulNumTs)
	{
		if (astTsInfo)
		{
			HLIB_STD_MemFree(astTsInfo);
			astTsInfo = NULL;
		}
	}

	if (ppstTsList) *ppstTsList = astTsInfo;
	if (pulNumTs)	*pulNumTs	= ulNumTs;

	return (0 == ulNumTs) ? ERR_FAIL : ERR_OK;
}

HERROR xmgr_tplist_GetHomeTpList_HMS1000S (MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs)
{
	HERROR		hErr = ERR_FAIL;

	switch (eHomeTsKind)
	{
	case eHomeTs_Search:	break;
	case eHomeTs_OTA:		hErr = xmgr_tplist_GetHomeTpListOta_HMS1000S(ppstTsList, pulNumTs);		break;
	default:				break;
	}

	return hErr;
}

/*********************** End of File ******************************/
