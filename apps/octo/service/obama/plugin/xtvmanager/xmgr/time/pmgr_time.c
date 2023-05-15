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
#include <mgr_time.h>
#include <pmgr_time.h>
#include "./local_include/_xmgr_time.h"

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
HERROR pmgr_time_Init (void)
{
	HERROR hErr;

	hErr = xmgr_time_Init_Base ();
#if defined(CONFIG_PROD_HMX4KBBC)
	xmgr_time_Init_Hmx4kbbc();
#endif

	return hErr;
}

HERROR pmgr_time_ClockRecoverGetDstMode (VK_CLOCK_DST_MODE *peDstMode)
{
#if defined(CONFIG_OP_MIDDLE_EAST)
	return xmgr_time_ClockRecoverGetDstMode_MeSat(peDstMode);
#else
	return xmgr_time_ClockRecoverGetDstMode_Base(peDstMode);
#endif
}

MgrTime_ClockRecoverRefTpInfo_t *pmgr_time_ClockRecoverGetRefTpList (void)
{
#if defined(CONFIG_OP_MIDDLE_EAST)
	return xmgr_time_ClockRecoverGetRefTpList_MeSat();
#elif defined(CONFIG_OP_FREESAT)
	return xmgr_time_ClockRecoverGetRefTpList_FreeSat();
#elif defined(CONFIG_PROD_HMS1000S) || defined(CONFIG_PROD_HMSLITE)
	return xmgr_time_ClockRecoverGetRefTpList_Eu();
#elif defined(CONFIG_PROD_FVP4000T)
	return xmgr_time_ClockRecoverGetRefTpList_Ukdtt();
#else
	return xmgr_time_ClockRecoverGetRefTpList_Base ();
#endif
}

HERROR pmgr_time_ClockRecoverSearchByFirstSvc (MgrTime_ClockRecoverRefMethod_e eRefMethod, DxTuneParam_t	*tuningInfo)
{
	return xmgr_time_ClockRecoverSearchByFirstSvc_Base (eRefMethod, tuningInfo);
}

HERROR pmgr_time_SetGmtOffset(HINT32 nGmtOffset, HBOOL bUpdateTzFile)
{
#if defined(CONFIG_OP_FREESAT)
	return xmgr_time_SetGmtOffset_FreeSat(nGmtOffset, bUpdateTzFile);
#elif defined(CONFIG_PROD_HMX4KBBC)
	return xmgr_time_SetGmtOffset_Hmx4kbbc(nGmtOffset, bUpdateTzFile);
#else
	return xmgr_time_SetGmtOffset_Base(nGmtOffset, bUpdateTzFile);
#endif
}

HERROR pmgr_time_ClockRecoverSearchByTerTp(HUINT32 ulRefTpIdx, DxTuneParam_t	*tuningInfo)
{
#if defined(CONFIG_PROD_FVP4000T)
	return xmgr_time_ClockRecoverSearchByTerTp_Ukdtt (ulRefTpIdx, tuningInfo);
#else
	return xmgr_time_ClockRecoverSearchByTerTp_Base(ulRefTpIdx, tuningInfo);
#endif
}




/*********************** End of File ******************************/
