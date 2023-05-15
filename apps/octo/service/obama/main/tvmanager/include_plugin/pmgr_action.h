/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_action.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_ACTION_H___
#define	___PMGR_ACTION_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>

#include <mgr_common.h>
#include <mgr_action.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
// PROC_NormalActionMgr() create 후 10 초 후에 NETWORK 정상 유무를 판단한다.
#define		NETWORK_CHECK_TIMER					0xfc0d

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HUINT32				pmgr_action_GetMainViewId (void);
void					pmgr_action_SetMainViewId (HUINT32 ulViewId);
MgrAction_State_e		pmgr_action_GetState (void);
void					pmgr_action_SetState(MgrAction_State_e eState);
MgrAction_SvcDB_e	pmgr_action_GetSvcDbState(void);
void					pmgr_action_SetSvcDbState(MgrAction_SvcDB_e eState);


/*		Normal Action
 */
HERROR 		pmgr_action_NormalStart(HBOOL hOperation);
HBOOL		pmgr_action_NormalIsAvailable (void);


/*		Standby Action
 */
HUINT32		pmgr_action_GetWakeupRcuKey(void);
#if 0
HERROR 		pmgr_action_BatchInit(DxBatchType_e eBatchType);
HERROR		pmgr_action_BatchGetStandbyProcList(DxBatchProc_b *peBatchProc);
#endif
HUINT32 					pmgr_action_BatchCount(DxBatchType_e eBatchType);
MgrAction_BatchInitInfo 	*pmgr_action_BatchGetProcInfo(DxBatchType_e eBatchType);
HBOOL						pmgr_action_BatchIsSerial(DxBatchType_e eBatchType);

HERROR		pmgr_action_RegisterSiTimeoutCallback(void);
HBOOL		pmgr_action_CheckWaitShutdown(void);

#endif /* !___PMGR_ACTION_H___ */

