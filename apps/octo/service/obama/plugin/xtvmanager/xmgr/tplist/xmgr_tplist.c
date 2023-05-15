/**
	@file     	xmgr_tplist.c
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

HERROR xmgr_tplist_GetAntennaIndex (SVC_SatType_t eSatType, HUINT32 *pulAntIdx)
{
#if	defined(CONFIG_MW_CH_SATELLITE)
	HBOOL			 bFound;
	HUINT32			 ulAntIdx;
	HINT32			 nAntCnt, nTotalAntNum;
	HINT32			*pnAntIdxArray;
	DbSvc_AntInfo_t		 stAntInfo;
	HERROR			 hErr;

	if (pulAntIdx == NULL)				{ return ERR_BUS_TARGET_NULL; }

	bFound = FALSE;
	pnAntIdxArray = NULL;
	ulAntIdx = 0;

	hErr = DB_SVC_FindAllAntIdxList (&nTotalAntNum, &pnAntIdxArray);
	if (hErr == ERR_OK)
	{
		for (nAntCnt = 0; nAntCnt < nTotalAntNum; nAntCnt++)
		{
			hErr = DB_SVC_GetAntInfo ((HUINT16)pnAntIdxArray[nAntCnt], &stAntInfo);
			if (hErr == ERR_OK)
			{
				if (stAntInfo.eSatType == eSatType)
				{
					bFound = TRUE;
					ulAntIdx = (HUINT32)pnAntIdxArray[nAntCnt];
					break;
				}
			}
		}

		DB_SVC_FreeAntList (pnAntIdxArray);
	}

	if (bFound == FALSE)
	{
		HxLOG_Critical("\n\n\n");
		return ERR_FAIL;
	}

	*pulAntIdx = ulAntIdx;
	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR xmgr_tplist_GetCabHomeTuningInfoList(DbSvc_CabTuningInfo_t **pastList, HUINT32 *pulNumList)
{
#if	defined(CONFIG_MW_CH_CABLE)
	if (NULL == pastList)
		return ERR_FAIL;

	if (NULL == pulNumList)
		return ERR_FAIL;

#if defined(CONFIG_OP_ERTELECOM)
	return xmgr_tplist_GetCabHomeTuningInfoList_ERTelecom(pastList, pulNumList);
#else
	return xmgr_tplist_GetCabHomeTuningInfoList_Base(pastList, pulNumList);
#endif
#else
	return ERR_FAIL;
#endif
}

HERROR xmgr_tplist_GetTerHomeTuningInfoList(DbSvc_TerTuningInfo_t **pastList, HUINT32 *pulNumList)
{
#if defined(CONFIG_MW_CH_TERRESTRIAL)
	if (NULL == pastList)
		return ERR_FAIL;

	if (NULL == pulNumList)
		return ERR_FAIL;

	return xmgr_tplist_GetTerHomeTuningInfoList_Base(pastList, pulNumList);
#else
	return ERR_FAIL;
#endif
}

/*********************** End of File ******************************/
