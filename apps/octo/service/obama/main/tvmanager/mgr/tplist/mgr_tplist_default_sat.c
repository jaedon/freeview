/**
	@file     mgr_tplist_default_sat.c
	@brief    file_name & simple comment.

	Description: dimension header file for kraken \n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/07/16		init					wjmoh\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

#include <octo_common.h>

#include <db_svc.h>
#include <db_param.h>

#include <mgr_common.h>
#include <mgr_tplist.h>
#include "local_include/_mgr_tplist.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions            ********************/
/*******************************************************************/

/*
 *	향별 Default Antenna ID List
 */

#define _____EXTERNAL_FUNCTIONS_____

HERROR MGR_TPLIST_SatGetHomeTuneInfo (SVC_SatType_t eSatType, DbSvc_TuningInfo_t *pstTuningInfo, HBOOL bCardDefault)
{
	return mgr_tplist_GetSatHomeTuneInfo (eSatType, pstTuningInfo, bCardDefault);
}

// ========================================================================
// -- 위성 리스트를 Sorting 하는 함수
// 사양에 따라 위성 순서가 제각각 다를 수 있다.
#define ___SATELLITE_SORTING_____

HERROR MGR_TPLIST_SatGetSatelliteSortMethod (MgrTplist_SatSortMethod_e *peSortMethod)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HUINT32		 ulValue;
	HERROR		 hErr;

	if (peSortMethod == NULL)			{ return ERR_FAIL; }

	hErr = DB_PARAM_GetItem (eDB_PARAM_ITEM_ANTENNA_SORT_METHOD, &ulValue, 0);
	if (hErr != ERR_OK)
	{
		// Default는 Name Sorting
		ulValue = eSATSORT_EU_NAME;
	}

	*peSortMethod = (MgrTplist_SatSortMethod_e)ulValue;

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

HERROR MGR_TPLIST_SatSetSatelliteSortMethod (MgrTplist_SatSortMethod_e eSortMethod)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	HERROR		 hErr;

	if (eSortMethod >= eSATSORT_MAX)
	{
		// Default는 Name Sorting
		eSortMethod = eSATSORT_EU_NAME;
	}

	hErr = DB_PARAM_SetItem (eDB_PARAM_ITEM_ANTENNA_SORT_METHOD, (HUINT32)eSortMethod, 0);
	if (hErr != ERR_OK)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
#else
	return ERR_FAIL;
#endif
}

/* end of file */

