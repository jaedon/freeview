/**
	@file     ap_spec.c
	@brief    Application�� �������� ����� �� �ִ� utility API

	Description: �� action���� OP/��ǰ�� Spec�� ���� �޶����� SI, EPG type ���� �����ϴ� ���
	�� ��⿡�� spec�� ���� �޶����� �Ǵ� �ڵ带 ���� ���� ���⼭ ó
	Module: AP/����			 	\n

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

	/* ���� cable model�� �ϳ��ۿ� ���..
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

