/**
	@file     	pmgr_antenna.c
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
#include <mgr_antenna.h>
#include "./local_include/_xmgr_antenna.h"

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

HERROR	pmgr_antenna_GetDefaultAntInfo(SVC_SatType_t *peSatType)
{
#if defined(CONFIG_MW_CH_SATELLITE)
	return xmgr_antenna_GetDefaultAntInfo_Base(peSatType);
#else
	return ERR_FAIL;
#endif
}


/*********************** End of File ******************************/