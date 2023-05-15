/**
	@file     	xmgr_clockrecover_base.c
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

#include <db_param.h>
#include <db_svc.h>
#include <db_param.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_time.h>
#include <pmgr_time.h>
#include "../local_include/_xmgr_time.h"

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

STATIC MgrTime_ClockRecoverRefTpInfo_t	s_stClockRefTp_Me[] =
{ /* Clock time search will be done according to the order of the below TP info until the clock time is successfully set up.
	Modify the Antenna order and list according to the clock setup scenario */
#if defined(CONFIG_MW_CH_SATELLITE)
	{eREF_SAT_TP, eSatType_NILESAT_101_102, 	12091, 27500, eDxSAT_CODERATE_AUTO, eDxSAT_POLAR_AUTO, eDxSAT_TRANS_DVBS2, eDxSAT_PSK_QPSK, eDxSAT_PILOT_OFF, eDxSAT_ROLL_035},	// AD Sports
	{eREF_SAT_TP, eSatType_BADR_4_6,			12130, 27500, eDxSAT_CODERATE_AUTO, eDxSAT_POLAR_AUTO, eDxSAT_TRANS_DVBS, eDxSAT_PSK_QPSK, eDxSAT_PILOT_OFF, eDxSAT_ROLL_035},		// Dubai One
	{eREF_SAT_TP, eSatType_HOTBIRD, 			10949, 27500, eDxSAT_CODERATE_AUTO, eDxSAT_POLAR_AUTO, eDxSAT_TRANS_DVBS, eDxSAT_PSK_QPSK, eDxSAT_PILOT_OFF, eDxSAT_ROLL_035},		// Al Fady
#endif
	{eREF_END, 0, 0, 0, 0, 0, 0, 0, 0} /* eREF_END will terminate the clock setup process */
};

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR xmgr_time_ClockRecoverGetDstMode_MeSat (VK_CLOCK_DST_MODE *peDstMode)
{
	HERROR			hErr = ERR_FAIL;
	HxCountry_e		eCountryId = eCountryID_NONE;
	HINT32			ulValue	= 0;

	DB_PARAM_GetItem(eDxSETUP_SYSTEM_DAYLIGHT_SAVING, (HUINT32 *)&ulValue, 0);
	if(ulValue == TRUE)
	{
		hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID, (HUINT32 *)&eCountryId, 0);
		if (ERR_OK != hErr)
		{
			HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_COUNTRY_ID) Error!!\n");
		}
		switch(eCountryId)
		{
			case eCountryID_GRC:
			case eCountryID_TUR:
				// Use Day light saving hard coding
				*peDstMode = VK_CLOCK_DST_AUTO;
				hErr = ERR_OK;
				break;
			default:
				// Always +1
				*peDstMode = VK_CLOCK_DST_ON_ALWAYS;
				hErr = ERR_OK;
				break;
		}
	}
	else
	{
		// Off
		*peDstMode = VK_CLOCK_DST_OFF;
		hErr = ERR_OK;
	}

	return hErr;
}

MgrTime_ClockRecoverRefTpInfo_t *xmgr_time_ClockRecoverGetRefTpList_MeSat (void)
{
	return s_stClockRefTp_Me;
}


/*********************** End of File ******************************/
