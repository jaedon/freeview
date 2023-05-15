/**
	@file     	ap_bootui_none.c
	@brief    	UI for the cold boot processes of Nothing
	It does nothing, but destroys itself and finish.

	Description:  			\n
	Module: 		app/ui/surfer			 					\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_param.h>
#include <osd_gfx.h>
#include <bus.h>
#include <mgr_common.h>

#include <mgr_swup.h>
#include <svc_si.h>

#include <mgr_appmgr.h>


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

#define __________________Prototype__________________


#define __________________LOCAL_FUNCTION________________

#if 0
HERROR	xmgr_swup_WakeupOnOtaSuccess_Irdeto (HUINT32 ulActionId)
{
	HUINT8				ucControlByte = 0;
	HUINT8				ucMenuDisable, ucEmmDisable;
	HERROR				hError = ERR_FAIL;
	HINT32				lWakeUpOtaFlag = 0;
	DxOperator_e		eOperatorType = eDxOPERATOR_NOT_DEFINED;

//	eOperatorType = MWC_PARAM_GetOperatorType();
	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOperatorType, 0);
	if (ERR_OK != hError)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
	}

	switch (eOperatorType)
	{
	case eDxOPERATOR_RussiaTvt:
	case eDxOPERATOR_Ziggo:
		XMGR_SWUP_GetControlByte_Irdeto(&ucControlByte);
		MGR_SWUPINFO_GetWakeUpOtaFlag(&lWakeUpOtaFlag);

		// Ziggo Irdeto OTA에서는 Control byte == 0x03 일 경우 WakeUp OTA를 진행 진행 한다.
		if ((ucControlByte == 0x03) && (lWakeUpOtaFlag != 0))
		{
#if 0 //ycgo - for compile
			BUS_MGR_Create ("AP_OtaWakeUp_UiProc", APP_DEFAULT_PRIORITY,
						AP_OtaWakeUp_UiProc, 0, 0, 0, 0);
#endif
		}
		break;

	case eDxOPERATOR_Truevisions:
		XMGR_SWUP_CheckDisableIrdetoOta_Irdeto(&ucMenuDisable, &ucEmmDisable);
		if (ucMenuDisable == FALSE)
		{
			MGR_SWUP_SetSpec(eDxSwUpdate_SPEC_IRDETO);
			MGR_SWUP_SetDefaultTp ();
#if 0 //ycgo - for compile
			BUS_MGR_Create(NULL, APP_DEFAULT_PRIORITY, AP_OtaExec_Proc, 0, 0, 0, 0);
#endif
		}
		break;

	default:
		break;
	}

	MGR_SWUPINFO_SetWakeUpOtaFlag((HINT32)FALSE);

	return	ERR_OK;
}

HERROR	xmgr_swup_WakeupOnOtaCreate_Irdeto (HUINT32 ulActionId)
{
	HERROR					hError = ERR_FAIL;
	DxOperator_e			eOperatorType = eDxOPERATOR_NOT_DEFINED;

//	eOperatorType = MWC_PARAM_GetOperatorType();
	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE, (HUINT32 *)&eOperatorType, 0);
	if (ERR_OK != hError)
	{
		HxLOG_Warning("DB_PARAM_GetItem(eDB_PARAM_ITEM_OPERATOR_TYPE) Error!!\n");
	}

	switch (eOperatorType)
	{
	case eDxOPERATOR_RussiaTvt:
	case eDxOPERATOR_Ziggo:
	case eDxOPERATOR_Truevisions:
		HxLOG_Info(" MGR_SWUPINFO_SetWakeUpOtaFlag() == TRUEl!!\n");
		MGR_SWUPINFO_SetWakeUpOtaFlag((HINT32)TRUE);
		break;

	default:
		HxLOG_Info(" MGR_SWUPINFO_SetWakeUpOtaFlag() == FALSEl!!\n");
		MGR_SWUPINFO_SetWakeUpOtaFlag((HINT32)FALSE);
		BUS_MGR_Destroy (NULL);
		break;
	}

	return ERR_OK;
}
#endif

#define _________________GLOBAL_FUNCTION__________________



/*********************** End of File ******************************/
