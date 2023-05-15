/**
	@file     	xmgr_time_eu.c
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
#include <hapi.h>
#include <db_svc.h>
#include <db_param.h>

#include <isosvc.h>

#include <bus.h>
#include <mgr_common.h>
#include <mgr_svclist.h>
#include <mgr_time.h>
#include <mgr_search.h>
#include <pmgr_time.h>
#include "../local_include/_xmgr_time.h"

#include <otl.h>

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

STATIC MgrTime_ClockRecoverRefTpInfo_t	s_stClockRefTp_Ger[] =
{ /* Clock time search will be done according to the order of the below TP info until the clock time is successfully set up.
	Modify the Antenna order and list according to the clock setup scenario */
#if defined(CONFIG_MW_CH_SATELLITE)
	{eREF_SAT_TP, eSatType_ASTRA_1, 12604, 22000, eDxSAT_CODERATE_5_6, eDxSAT_POLAR_HOR, eDxSAT_TRANS_DVBS, eDxSAT_PSK_QPSK, eDxSAT_PILOT_OFF, eDxSAT_ROLL_035},			// WDR
	{eREF_SAT_TP, eSatType_ASTRA_1, 12552, 22000, eDxSAT_CODERATE_5_6, eDxSAT_POLAR_VER, eDxSAT_TRANS_DVBS, eDxSAT_PSK_QPSK, eDxSAT_PILOT_OFF, eDxSAT_ROLL_035},
	{eREF_SAT_TP, eSatType_ASTRA_1, 10832, 22000, eDxSAT_CODERATE_5_6, eDxSAT_POLAR_HOR, eDxSAT_TRANS_DVBS, eDxSAT_PSK_QPSK, eDxSAT_PILOT_OFF, eDxSAT_ROLL_035},
#endif
	{eREF_END, 0, 0, 0, 0, 0, 0, 0, 0} /* eREF_END will terminate the clock setup process */
};

STATIC MgrTime_ClockRecoverRefTpInfo_t	s_stClockRefTp_Ita[] =
{ /* Clock time search will be done according to the order of the below TP info until the clock time is successfully set up.
	Modify the Antenna order and list according to the clock setup scenario */
#if defined(CONFIG_MW_CH_SATELLITE)
	{eREF_SAT_TP, eSatType_HOTBIRD, 10992, 27500, eDxSAT_CODERATE_AUTO, eDxSAT_POLAR_VER, eDxSAT_TRANS_DVBS, eDxSAT_PSK_QPSK, eDxSAT_PILOT_OFF, eDxSAT_ROLL_035},
#endif
	{eREF_END, 0, 0, 0, 0, 0, 0, 0, 0} /* eREF_END will terminate the clock setup process */
};

STATIC MgrTime_ClockRecoverRefTpInfo_t	s_stClockRefTp_Che[] =
{ /* Clock time search will be done according to the order of the below TP info until the clock time is successfully set up.
	Modify the Antenna order and list according to the clock setup scenario */
#if defined(CONFIG_MW_CH_SATELLITE)
	{eREF_SAT_TP, eSatType_HOTBIRD, 10971, 29700, eDxSAT_CODERATE_AUTO, eDxSAT_POLAR_HOR, eDxSAT_TRANS_DVBS2, eDxSAT_PSK_8PSK, eDxSAT_PILOT_OFF, eDxSAT_ROLL_035},
#endif
	{eREF_END, 0, 0, 0, 0, 0, 0, 0, 0} /* eREF_END will terminate the clock setup process */
};


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR xmgr_time_ClockRecoverGetDstMode_Eu (VK_CLOCK_DST_MODE *peDstMode)
{
	return ERR_FAIL;
}

MgrTime_ClockRecoverRefTpInfo_t *xmgr_time_ClockRecoverGetRefTpList_Eu (void)
{
	MgrTime_ClockRecoverRefTpInfo_t *pTpList = NULL;
	HxCountry_e		 eCountryId = eCountryID_NONE;
	MGR_SEARCHUTIL_GetSatCountryIdx(&eCountryId);

	switch(eCountryId){
		case eCountryID_DEU:
		case eCountryID_AUT:
			HxLOG_Print("[%s:%d] getting German TP list for clock !! \n", __FUNCTION__, __LINE__);
			pTpList = s_stClockRefTp_Ger;
			break;

		case eCountryID_CHE:
			HxLOG_Print("[%s:%d] getting Swiss TP list for clock !! \n", __FUNCTION__, __LINE__);
			pTpList = s_stClockRefTp_Che;
			break;
		case eCountryID_ITA:
			HxLOG_Print("[%s:%d] getting Italian TP list for clock !! \n", __FUNCTION__, __LINE__);
			pTpList = s_stClockRefTp_Ita;
			break;
		default:
			HxLOG_Print("[%s:%d] getting Default TP list for clock !! \n", __FUNCTION__, __LINE__);
			pTpList = s_stClockRefTp_Ger;
			break;
	}

	return pTpList;
}

/*********************** End of File ******************************/
