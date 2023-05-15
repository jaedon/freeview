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
#include <db_svc.h>
#include <bus.h>
#include <mgr_common.h>
#include <mgr_tplist.h>
#include "../local_include/_xmgr_tplist.h"
#include <xmgr_cas.h>

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
HERROR xmgr_tplist_GetSatHomeTuneInfo_Base (SVC_SatType_t eSatType, DbSvc_TuningInfo_t *pstTuningInfo, HBOOL bCardDefault)
{

	if (pstTuningInfo == NULL)
	{
		return ERR_FAIL;
	}

#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL) || defined(CONFIG_MW_CAS_IRDETO_RECOVERY_RESCAN_HOME)
	if(bCardDefault == TRUE)
	{
		if(XMGR_CAS_IrGetSmcHomeTuneInfo(pstTuningInfo) == ERR_OK)
		{
			return ERR_OK;
		}
	}
#endif

	switch (eSatType)
	{
	case eSatType_THAICOM_2_3:
		HxSTD_memset(pstTuningInfo, 0, sizeof(DbSvc_TuningInfo_t));
		pstTuningInfo->sat.ulFrequency	= 12438;
		pstTuningInfo->sat.ulSymbolRate	= 30000;
		pstTuningInfo->sat.ePolarization	= eDxSAT_POLAR_HOR;
		pstTuningInfo->sat.eFecCodeRate	= eDxSAT_CODERATE_2_3;

		pstTuningInfo->sat.eTransSpec	= eDxSAT_TRANS_DVBS;
		pstTuningInfo->sat.ePskMod		= eDxSAT_PSK_AUTO;
		break;

	// TODO:...

	default:
		return ERR_FAIL;
	}
	return ERR_OK;
}


/*********************** End of File ******************************/
