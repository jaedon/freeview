/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_epg.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_EPG_H___
#define	___PMGR_EPG_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_epg.h>

#include <mgr_action.h>
/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Plug-In Functions    ********************/
/*******************************************************************/

/*	EPG Action
 */
HERROR	pmgr_epg_Start (MgrEpg_Kind_e eType, MgrEpg_ProcessType_e eProcessType, MgrEpg_CallType_e eCallType);
HERROR	pmgr_epg_Stop (MgrEpg_Kind_e eType);
BUS_Callback_t	pmgr_epg_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength);
HERROR			pmgr_epg_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength);
HERROR			pmgr_epg_RescheduleBatch(void);
HERROR 			pmgr_epg_GetBatchEpgCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
/*	EIT Monitor
*/
//HERROR xmgr_epg_RfStart_Base (void);

#endif /* !___PMGR_EPG_H___ */

