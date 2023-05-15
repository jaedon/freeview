/**
	@file     	xmgr_tplist_base.c
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

HERROR	xmgr_tplist_GetSatHomeTuningInfoList_Base(DbSvc_SatTuningInfo_t **pastList, HUINT32 *pulNumList)
{
#if	defined(CONFIG_MW_CH_SATELLITE)
	static DbSvc_SatTuningInfo_t	 s_stBaseHomeTuneInfo_Sat[] =
	{
		{	12150,	27500,	eDxSAT_POLAR_HOR,	eDxSAT_CODERATE_3_4,	eDxSAT_TRANS_DVBS, 	eDxSAT_PSK_QPSK,	0,	0,	0},
	};

	*pastList = s_stBaseHomeTuneInfo_Sat;
	*pulNumList = (sizeof(s_stBaseHomeTuneInfo_Sat) / sizeof(DbSvc_SatTuningInfo_t));
	return ERR_OK;
#else
	*pastList = NULL;
	*pulNumList = 0;
	return ERR_FAIL;
#endif
}

HERROR	xmgr_tplist_GetCabHomeTuningInfoList_Base(DbSvc_CabTuningInfo_t **pastList, HUINT32 *pulNumList)
{
#if	defined(CONFIG_MW_CH_CABLE)
	static DbSvc_CabTuningInfo_t	 s_stBaseHomeTuneInfo_Cab[] =
	{
		{	450000,	6900,		0x2717,	eDxCAB_CONSTELLATION_64QAM,	eDxCAB_SPECTRUM_AUTO},
		{	0,		0,			0,		0,							0,				  },
	};

	*pastList = s_stBaseHomeTuneInfo_Cab;
	*pulNumList = (sizeof(s_stBaseHomeTuneInfo_Cab) / sizeof(DbSvc_CabTuningInfo_t));
	return ERR_OK;
#else
	*pastList = NULL;
	*pulNumList = 0;
	return ERR_FAIL;
#endif
}

HERROR	xmgr_tplist_GetTerHomeTuningInfoList_Base(DbSvc_TerTuningInfo_t **pastList, HUINT32 *pulNumList)
{
#if defined(CONFIG_MW_CH_TERRESTRIAL)
	*pastList = NULL;
	*pulNumList = 0;
	return ERR_FAIL;
#else
	*pastList = NULL;
	*pulNumList = 0;
	return ERR_FAIL;
#endif
}

STATIC HERROR xmgr_tplist_GetHomeTpListOta_Base (DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs)
{
	HUINT32					 ulCount, ulNumTs;
	DbSvc_TsInfo_t			*astTsInfo;
	HBOOL					 bAllocList;
#if	defined(CONFIG_MW_CH_SATELLITE)
	HUINT32					 ulNumList;
#endif
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
#endif
#if defined(CONFIG_MW_CH_TERRESTRIAL)
	HxCountry_e				 eCountryId;
	HUINT32 				 ulNumTsTer = 0;
//	DbSvc_TerTuningInfo_t	*astTerList = NULL;
#endif

	if (NULL == pulNumTs)		return ERR_FAIL;

	bAllocList = (NULL != ppstTsList) ? TRUE : FALSE;

	// Get Num
	ulNumTs = 0;
#if	defined(CONFIG_MW_CH_SATELLITE)
	hErr = xmgr_tplist_GetSatHomeTuningInfoList_Base(&astSatList, &ulNumList);
	if (ERR_OK == hErr && NULL != astSatList && 0 < ulNumList)
	{
		ulNumTsSat = 0;
		for (ulCount = 0; ulCount < ulNumList; ulCount++)
		{
			if (astSatList[ulCount].ulFrequency == 0) break;
			ulNumTsSat++;
		}
		ulNumTs += ulNumTsSat;
	}
#endif

#if	defined(CONFIG_MW_CH_CABLE)
	ulNumTsCab = 0;

	DB_SVC_InitTsFindCondition(&stCond);
	stCond.nDeliType = eDxDELIVERY_TYPE_CAB;

	hErr = DB_SVC_FindTsInfoList(&stCond, (HINT32*)&ulNumTsCab, &pstTsInfo);
	if (( hErr != ERR_OK) ||( ulNumTsCab == 0))
	{
		*pulNumTs	= 0;

		if(pstTsInfo)
		{
			OxDB_Free(pstTsInfo);
		}

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
			if (NULL != astSatList && 0 < ulNumList)
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

	if (ppstTsList)
	{
		*ppstTsList = astTsInfo;
	}
	else if(astTsInfo != NULL)
	{
		HLIB_STD_MemFree(astTsInfo);
	}

	if (pulNumTs)
	{
		*pulNumTs	= ulNumTs;
	}

	return (0 == ulNumTs) ? ERR_FAIL : ERR_OK;
}


// AP_SPEC_BASE_GetHomeTpList
HERROR xmgr_tplist_GetHomeTpList_Base (MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs)
{
	HERROR		hErr = ERR_FAIL;

	if (NULL == pulNumTs)		return ERR_FAIL;

	switch (eHomeTsKind)
	{
	case eHomeTs_Search:	break;
	case eHomeTs_OTA:		hErr = xmgr_tplist_GetHomeTpListOta_Base (ppstTsList, pulNumTs);		break;
	default:				break;
	}

	return hErr;
}


/*********************** End of File ******************************/
