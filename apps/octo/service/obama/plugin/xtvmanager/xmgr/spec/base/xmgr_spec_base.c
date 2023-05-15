/**
	@file     xxx_spec.c
	@brief    Application�� �������� ����� �� �ִ� utility API

	Description: �� action���� OP/��ǰ�� Spec�� ���� �޶����� SI, EPG type ���� �����ϴ� ���
	�� ��⿡�� spec�� ���� �޶����� �Ǵ� �ڵ带 ���� ���� ���⼭ ó
	Module: AP/����			 	\n

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
	AP_SPEC_RegisterAutoSymbolRateList �Լ��� Application�� ���� ���� �����ϰų� �ؾ� ��.
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

