/**
	@file     db_main.c
	@brief

	Description: 									\n
	Module: 										\n
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <db_common.h>
#include <pdb_svc.h>

#include "local_include/_db_main.h"

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
STATIC PostMsgToMgrCb_t		s_fnNotifyToMgr = NULL;
/*******************************************************************/
/**************    local function prototypes    ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/


HERROR	DB_Init(PostMsgToMgrCb_t fnNotify)
{
	HERROR		hErr = ERR_FAIL;

	s_fnNotifyToMgr = fnNotify;

#if defined(CONFIG_MW_CAS_IRDETO_LOADER_DVT)
#else
	hErr = db_param_Init();
	// CHECK_BOOT_TIME("db_param_Init");

	hErr = db_svc_Init();
	// CHECK_BOOT_TIME("db_svc_Init");
#endif

#if defined(CONFIG_DEBUG)
	pdb_cmd_Register ();
#endif

	return hErr;
}

HERROR db_main_NotifyUpdate(HINT32 nMsg, Handle_t hAct,
							HUINT32 ulParam1, HUINT32 ulParam2, HUINT32 ulParam3)
{
	if (s_fnNotifyToMgr)
	{
		(* s_fnNotifyToMgr)(nMsg, hAct, (HINT32)ulParam1, (HINT32)ulParam2, (HINT32)ulParam3);
		return ERR_OK;
	}

	return ERR_FAIL;
}


/*********************** End of File ******************************/

