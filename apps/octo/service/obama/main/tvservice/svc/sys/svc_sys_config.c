/**
	@file     mw_sys.c
	@brief    IO device 및 전원 관련 모듈을 제어하는 service

	Description: \n
	Module: MW/SYS			 	\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <isosvc.h>

#include <pal_panel.h>
#include <pal_power.h>
#include <pal_key.h>
#include <pal_sys.h>
#include <db_param.h>
#include <svc_sys.h>

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
/******************* Static Function Prototypes  *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

HERROR SVC_SYS_GetBlinkingInfo(DxLedId_e *aulSupportLedList, HUINT32 *pulLedNum, KEY_Code_t *aulIgnoreKeyList, HUINT32 *pulKeyNum)
{
#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT) || defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT_V5)
	return ERR_OK;
#else
	HERROR				hErr;
	DxOperator_e		eOpType;

	if (aulSupportLedList == NULL || pulLedNum == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

	if (aulIgnoreKeyList == NULL || pulKeyNum == NULL)
	{
		HxLOG_Critical("\n\n");
		return ERR_FAIL;
	}

//	eOpType = MWC_PARAM_GetOperatorType();
	hErr = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOpType, 0);
	if (ERR_OK != hErr)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
	}

	switch (eOpType)
	{
	case eDxOPERATOR_Ziggo:
	case eDxOPERATOR_RussiaTvt:
		// Supported LED
		aulSupportLedList[0] = eDxLED_ID_TV;
		aulSupportLedList[1] = eDxLED_ID_RADIO;
		*pulLedNum = 2;
		// Keys to ignore for LED Blinking
		aulIgnoreKeyList[0] = KEY_STANDBY;
		aulIgnoreKeyList[1] = KEY_TVRADIO;
		*pulKeyNum = 2;
		return ERR_OK;
	default:
		break;
	}

// 각 Model 별로 정의 한다.
#if defined(CONFIG_HDPVR1000C)
	//
#else
	// Not Supported
	*pulLedNum = 0;
#endif

	return ERR_OK;
#endif
}



