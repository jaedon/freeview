/**
	@file     ap_spec.c
	@brief    Application이 공통으로 사용할 수 있는 utility API

	Description: 각 action에서 OP/제품별 Spec에 따라 달라지는 SI, EPG type 등을 결정하는 모듈
	각 모듈에서 spec에 따라 달라지는 판단 코드를 넣지 말고 여기서 처
	Module: AP/공통			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_common.h>
#include <mgr_appmgr.h>
#include <mgr_spec.h>
#include <svc_ch.h>
/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/**************** Static Function Prototypes  **********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

#if defined(CONFIG_MW_CH_CAB_AUTO_SR)
HERROR	xmgr_spec_RegisterAutoSymbolRateList(void)
{
	HxCountry_e			eCountryId = eCountryID_NONE;
	//DxOperator_e		eOperatorType;
	HERROR				hErr = ERR_FAIL;

	MGR_SPEC_GetCountryCode (&eCountryId);

	/* 현재 cable model은 하나밖에 없어서..
	err = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOperatorType, 0);
	if (ERR_OK != err)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
	}
	*/

	switch (eCountryId)
	{
	case eCountryID_RUS:
		{
			HUINT32	aulSymbolRateList[] = { 6875, 6900, (HUINT32)-1, };
			hErr = SVC_CH_RegisterAutoSRList(2, aulSymbolRateList);
		}
		break;

	default:
		break;
	}

	return hErr;
}
#endif

/*********************** End of File ******************************/

