/**
	@file     xxx_spec.c
	@brief    Application이 공통으로 사용할 수 있는 utility API

	Description: 각 action에서 OP/제품별 Spec에 따라 달라지는 SI, EPG type 등을 결정하는 모듈
	각 모듈에서 spec에 따라 달라지는 판단 코드를 넣지 말고 여기서 처
	Module: AP/공통			 	\n

	Copyright (c) 2011 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/


#include <mgr_spec.h>
#include <xmgr_spec.h>
#include <xsvc_si.h>
#include "../local_include/_xmgr_spec.h"

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
/********************      Functions       *************************/
/*******************************************************************/


/*
	To Do:
	OCTO 2.0
	AP_SPEC_RegisterAutoSymbolRateList 함수는 Application이 붙은 이후 동작하거나 해야 함.
*/
#if 0
HERROR	AP_SPEC_RegisterAutoSymbolRateList(void)
{
	HxCountry_e			eCountryId;
	DxOperator_e		eOperatorType;
	HERROR				err;

	MGR_SPEC_GetCountryCode (&eCountryId);
//	eOperatorType = MWC_PARAM_GetOperatorType();
	err = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOperatorType, 0);
	if (ERR_OK != err)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
	}

	switch (eCountryId)
	{
	case eCountryID_FIN:
		if (eOperatorType == eDxOPERATOR_FiCom)
		{
			HUINT32 aulSymbolRateList[] = { 6875, 6900, (HUINT32)-1, };
			return SVC_CH_RegisterAutoSRList(2, aulSymbolRateList);
		}
		break;

	case eCountryID_SWE:
		if (eOperatorType == eDxOPERATOR_Comhem /*|| eOperatorType == eDxOPERATOR_Tele2*/)
		{
			HUINT32	aulSymbolRateList[] = { 6875, 6900, (HUINT32)-1, };
			return SVC_CH_RegisterAutoSRList(2, aulSymbolRateList);
		}
		break;

	case eCountryID_NLD:
		if (eOperatorType == eDxOPERATOR_Ziggo)
		{
			HUINT32	aulSymbolRateList[] = { 6875, 6900, (HUINT32)-1, };
			return SVC_CH_RegisterAutoSRList(2, aulSymbolRateList);
		}
		break;

	case eCountryID_RUS:
		if (eOperatorType == eDxOPERATOR_Mostelecom)
		{
			HUINT32	aulSymbolRateList[] = { 6875, 6900, (HUINT32)-1, };
			return SVC_CH_RegisterAutoSRList(2, aulSymbolRateList);
		}
		else if (eOperatorType == eDxOPERATOR_RussiaTvt)
		{
			HUINT32	aulSymbolRateList[] = { 6875, 6900, (HUINT32)-1, };
			return SVC_CH_RegisterAutoSRList(2, aulSymbolRateList);
		}
		break;

	default:									// ETC Satellite : Eu Base SatLive
		break;
	}


	return ERR_OK;
}
#endif


/*********************** End of File ******************************/

