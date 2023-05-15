/**
@file     pmgr_dynamicupdate.c
@brief

Description:  \n
Module: \n
Remarks : \n

Copyright (c) 2009 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

#include <bus.h>

#include <mgr_dynamicupdate.h>
#include "./local_include/_xmgr_dynamicupdate.h"

static HBOOL					s_bStartedDynamicUpdated = FALSE;	/* OM에 있던 값을 여기로 옮겨옴.. */

STATIC HERROR _pmgr_dynamicupdate_Start(void)
{
#if defined(CONFIG_OP_AUS_DTT)
	return xmgr_dynamicupdate_Start_Ausdtt();
#elif defined(CONFIG_OP_UK_DTT)
	// dynamicupdate를 enable 할 경우 모든 확인 필요
	//	return xmgr_dynamicupdate_Start_Ukdtt();
	return ERR_FAIL;
#elif defined(CONFIG_OP_NORDIG)
	return xmgr_dynamicupdate_Start_Nordig();
#else
	return ERR_FAIL;
#endif
}

STATIC HERROR _pmgr_dynamicupdate_Stop(void)
{
#if defined(CONFIG_OP_AUS_DTT)
	return xmgr_dynamicupdate_Stop_Ausdtt();
#elif defined(CONFIG_OP_UK_DTT)
	// dynamicupdate를 enable 할 경우 모든 확인 필요
	//	return xmgr_dynamicupdate_Stop_Ukdtt();
	return ERR_FAIL;
#elif defined(CONFIG_OP_NORDIG)
	return xmgr_dynamicupdate_Stop_Nordig();
#else
	return ERR_FAIL;
#endif
}

HERROR	pmgr_dynamicupdate_Start(void)
{
	HERROR		hError = ERR_FAIL;
	HUINT32		ulValue = eDymicUpdate_Disable;

	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING, &ulValue, 0);
	if(hError == ERR_OK)
	{
		if(eDymicUpdate_Enable == (DbSvc_DynamicUpdate_e)ulValue)
		{
			if(TRUE != s_bStartedDynamicUpdated)
			{
				HxLOG_Print(HxANSI_COLOR_CYAN("START\n\n"));
				s_bStartedDynamicUpdated = TRUE;
				return _pmgr_dynamicupdate_Start();
			}
			else
			{
				HxLOG_Print("Already Started\n");
				return ERR_OK;
			}
		}
		else
		{
			HxLOG_Debug("eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING is disabled \n");
		}

	}
	else
	{
		HxLOG_Debug("cannot get eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING \n");
	}

	return hError;
}

HERROR	pmgr_dynamicupdate_Stop(void)
{
	HERROR		hError = ERR_FAIL;
	HUINT32		ulValue = eDymicUpdate_Disable;

	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING, &ulValue, 0);
	if(hError == ERR_OK)
	{
		if(eDymicUpdate_Enable == (DbSvc_DynamicUpdate_e)ulValue)
		{
			if(TRUE == s_bStartedDynamicUpdated)
			{
				HxLOG_Print(HxANSI_COLOR_CYAN("STOP\n\n"));
				s_bStartedDynamicUpdated = FALSE;
				return _pmgr_dynamicupdate_Stop();
			}
			else
			{
				HxLOG_Print("Already Stopped\n");
				return ERR_OK;
			}

		}
		else
		{
			HxLOG_Debug("eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING is disabled \n");
		}

	}
	else
	{
		HxLOG_Debug("cannot get eDB_PARAM_ITEM_DYNAMIC_UPDATE_SETTING \n");
	}

	return hError;
}

HERROR pmgr_dynamicupdate_SvcStop(Handle_t hAction)
{
#if defined(CONFIG_OP_AUS_DTT)
#elif defined(CONFIG_OP_UK_DTT)
#elif defined(CONFIG_OP_NORDIG)
	(void)xmgr_dynamicupdate_SvcStop_Nordig(hAction);
#else
	#error ("Make dynamicupdate proc in case of using CONFIG_MW_DYNAMIC_UPDATE!!!\n")
#endif

	return ERR_OK;
}

// for batch
BUS_Callback_t		pmgr_dynamicupdate_GetProc(void)
{
#if defined(CONFIG_OP_AUS_DTT)
	return xmgr_dynamicupdate_GetProc_Ausdtt();
#elif defined(CONFIG_OP_UK_DTT)
	return xmgr_dynamicupdate_GetProc_Ukdtt();
#elif defined(CONFIG_OP_NORDIG)
	return xmgr_dynamicupdate_GetProc_Nordig();
#else
	#error ("Make dynamicupdate proc in case of using CONFIG_MW_DYNAMIC_UPDATE!!!\n")
#endif
}

