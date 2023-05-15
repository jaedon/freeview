/**
	@file     	xmgr_tplist_ertelecom.c
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

HERROR	xmgr_tplist_GetCabHomeTuningInfoList_ERTelecom (DbSvc_CabTuningInfo_t **pastList, HUINT32 *pulNumList)
{
#if	defined(CONFIG_MW_CH_CABLE)
	static DbSvc_CabTuningInfo_t	 s_stBaseHomeTuneInfo_Ertelecom[] =
	{
		{	386000, 6875,		0x2717, eDxCAB_CONSTELLATION_AUTO,	eDxCAB_SPECTRUM_AUTO},
		{	0,		0,			0,		0,							0,				  },
	};

	*pastList = s_stBaseHomeTuneInfo_Ertelecom;
	*pulNumList = (sizeof(s_stBaseHomeTuneInfo_Ertelecom) / sizeof(DbSvc_CabTuningInfo_t));
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

STATIC HERROR xmgr_tplist_GetHomeTpListOta_ERTelecom (DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs)
{
	HUINT32					 ulCount, ulNumTs;
	DbSvc_TsInfo_t			*astTsInfo;
	HBOOL					 bAllocList;
#if	defined(CONFIG_MW_CH_CABLE)
	HUINT32 				 ulNumTsCab = 0, ulNumTsCab2=0;
	DbSvc_TsCondition_t		 stCond;
	HERROR					 hErr = ERR_FAIL;
	DbSvc_TsInfo_t			*pstTsInfo = NULL;
	DbSvc_CabTuningInfo_t	*astList = NULL;
	HUINT32					 ulNumList = 0;
#endif

	if (NULL == pulNumTs)		return ERR_FAIL;

	bAllocList = (NULL != ppstTsList) ? TRUE : FALSE;

	// Get Num
	ulNumTs = 0;

#if	defined(CONFIG_MW_CH_CABLE)
	ulNumTsCab = 0;

	DB_SVC_InitTsFindCondition(&stCond);
	stCond.nDeliType = eDxDELIVERY_TYPE_CAB;

	hErr = DB_SVC_FindTsInfoList(&stCond, &ulNumTsCab, &pstTsInfo);

	//don't return..... for OTA wizard
	if (( hErr != ERR_OK) ||( ulNumTsCab == 0))
	{
		hErr = xmgr_tplist_GetCabHomeTuningInfoList (&astList, &ulNumList);
		if (ERR_OK == hErr && NULL != astList && 0 < ulNumList)
		{
			for (ulCount = 0; ulCount < ulNumList; ulCount++)
			{
				if (astList[ulCount].ulFreq == 0)	break;
				ulNumTsCab2++;
			}
		}
		ulNumTs += ulNumTsCab2;
	}

	ulNumTs += ulNumTsCab;
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


#if	defined(CONFIG_MW_CH_CABLE)

		HxLOG_Print("[%s:%d]SYKIM3----------step1(%d)\n", __FUNCTION__, __LINE__,ulNumTsCab);
		for ( ulCount = 0; ulCount < ulNumTsCab; ulCount++ )
		{
			astTsInfo[ulNumTs].eDeliType			= eDxDELIVERY_TYPE_CAB;
			astTsInfo[ulNumTs].stTuningParam.cab = pstTsInfo[ulCount].stTuningParam.cab;

			ulNumTs++;

			HxLOG_Print("[%s:%d]add (%d th TP) %dMhz, SR:%d, Netwok ID : %d, Con: %d\n", __FUNCTION__, __LINE__,
				ulCount, pstTsInfo[ulCount].stTuningParam.cab.ulFreq, pstTsInfo[ulCount].stTuningParam.cab.usSR, pstTsInfo[ulCount].stTuningParam.cab.usNetworkId, pstTsInfo[ulCount].stTuningParam.cab.ucCon);

		}

		if(ulNumTsCab == 0)		//if no home tp. refer to s_stBaseHomeTuneInfo_Cab
		{
			HxLOG_Print("[%s:%d]SYKIM3----------step2(%d)\n", __FUNCTION__, __LINE__,ulNumTs);

			for (ulCount = 0; ulCount < ulNumTsCab2; ulCount++)
			{
				if (0 == astList[ulCount].ulFreq)	break;
				astTsInfo[ulNumTs].eDeliType			= eDxDELIVERY_TYPE_CAB;
				astTsInfo[ulNumTs].stTuningParam.cab	= astList[ulCount];
				ulNumTs++;
			}
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

HERROR xmgr_tplist_GetHomeTpList_ERTelecom (MgrTplist_HomeTsKind_e eHomeTsKind, DbSvc_TsInfo_t **ppstTsList, HUINT32 *pulNumTs)
{
	HERROR		hErr = ERR_FAIL;

	switch (eHomeTsKind)
	{
	case eHomeTs_Search:	break;
	case eHomeTs_OTA:		hErr = xmgr_tplist_GetHomeTpListOta_ERTelecom (ppstTsList, pulNumTs);		break;
	default:				break;
	}

	return hErr;
}



/*********************** End of File ******************************/
