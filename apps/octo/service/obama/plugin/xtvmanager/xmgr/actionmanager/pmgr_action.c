/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  xmgr_manual_action.c
	@brief	  init manual action source file

	Description: manual action initialize  관련 내용들을 분류한 모듈이다. \n
	Module:
	Remarks : 										\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <mgr_common.h>
#include <mgr_action.h>
#include <pmgr_action.h>
#include "./local_include/_xmgr_action.h"
#include "./local_include/_xmgr_action_batch.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

/*		Action Common
 */
HUINT32 pmgr_action_GetMainViewId (void)
{
	return xmgr_action_GetMainViewId();
}

void pmgr_action_SetMainViewId (HUINT32 ulViewId)
{
	xmgr_action_SetMainViewId (ulViewId);
}

MgrAction_State_e pmgr_action_GetState(void)
{
	return xmgr_action_GetState();
}

void		pmgr_action_SetState(MgrAction_State_e eState)
{
	xmgr_action_SetState(eState);
}

MgrAction_SvcDB_e	pmgr_action_GetSvcDbState(void)
{
	return xmgr_action_GetSvcDbState();
}

void		pmgr_action_SetSvcDbState(MgrAction_SvcDB_e eState)
{
	xmgr_action_SetSvcDbState(eState);
}

/*		Action Normal
 */

HERROR pmgr_action_NormalStart(HBOOL hOperation)
{
	return BUS_MGR_Create("xproc_action_Normal", APP_ACTION_MGR_PRIORITY, xmgr_action_NormalGetProc(), 0, (HUINT32)hOperation, 0, 0);
}

HBOOL pmgr_action_NormalIsAvailable(void)
{
	return (BUS_MGR_Get(xmgr_action_NormalGetProc()) != NULL) ? FALSE : TRUE;
}

/*		Action Manual
 */

#if 0
HERROR pmgr_action_BatchInit(DxBatchType_e eBatchType)
{
	HERROR		hErr = ERR_OK;
	HUINT32		actionType = 0;

	hErr = xmgr_action_BatchInit(actionType, eBatchType);
	return hErr;
}

HERROR	pmgr_action_BatchGetStandbyProcList(DxBatchProc_b *peBatchProc)
{
#if	defined(CONFIG_OP_MIDDLE_EAST)
	return xmgr_action_BatchStandbyProcList_MeSat(peBatchProc);
#elif defined(CONFIG_OP_SES)
	return xmgr_action_BatchStandbyProcList_Ses(peBatchProc);
#else
	return xmgr_action_BatchStandbyProcList_Base(peBatchProc);
#endif
}
#else
HUINT32 pmgr_action_BatchCount(DxBatchType_e eBatchType)
{
#if defined(CONFIG_OP_MIDDLE_EAST)
	return xmgr_action_BatchCount_MeSat(eBatchType);
#elif defined(CONFIG_OP_SES)
	return xmgr_action_BatchCount_Ses(eBatchType);
#elif defined(CONFIG_OP_ERTELECOM)
	return xmgr_action_BatchCount_Ertelecom(eBatchType);
#elif defined(CONFIG_OP_AUS_DTT)
	return xmgr_action_BatchCount_AusDtt(eBatchType);
#elif defined(CONFIG_OP_UK_DTT)
	return xmgr_action_BatchCount_UkDtt(eBatchType);
#else
	return xmgr_action_BatchCount_Base(eBatchType);
#endif
}

MgrAction_BatchInitInfo * pmgr_action_BatchGetProcInfo(DxBatchType_e eBatchType)
{
#if defined(CONFIG_OP_MIDDLE_EAST)
	return xmgr_action_BatchGetProcInfo_MeSat(eBatchType);
#elif defined(CONFIG_OP_SES)
	return xmgr_action_BatchGetProcInfo_Ses(eBatchType);
#elif defined(CONFIG_OP_ERTELECOM)
	return xmgr_action_BatchGetProcInfo_Ertelecom(eBatchType);
#elif defined(CONFIG_OP_AUS_DTT)
	return xmgr_action_BatchGetProcInfo_AusDtt(eBatchType);
#elif defined(CONFIG_OP_UK_DTT)
	return xmgr_action_BatchGetProcInfo_UkDtt(eBatchType);
#else
	return xmgr_action_BatchGetProcInfo_Base(eBatchType);
#endif
}

HBOOL pmgr_action_BatchIsSerial(DxBatchType_e eBatchType)
{
#if defined(CONFIG_OP_MIDDLE_EAST)
	return xmgr_action_BatchIsSerial_MeSat(eBatchType);
#elif defined(CONFIG_OP_SES)
	return xmgr_action_BatchIsSerial_Ses(eBatchType);
#elif defined(CONFIG_OP_ERTELECOM)
	return xmgr_action_BatchIsSerial_Ertelecom(eBatchType);
#elif defined(CONFIG_OP_AUS_DTT)
	return xmgr_action_BatchIsSerial_AusDtt(eBatchType);
#elif defined(CONFIG_OP_UK_DTT)
	return xmgr_action_BatchIsSerial_UkDtt(eBatchType);
#else
	return xmgr_action_BatchIsSerial_Base(eBatchType);
#endif
}
#endif

HUINT32 pmgr_action_GetWakeupRcuKey(void)
{
	return xmgr_action_GetWakeupRcukey_Base();
}

HERROR pmgr_action_RegisterSiTimeoutCallback(void)
{
	return xmgr_action_RegisterSiTimeoutCallback_Base();
}

HBOOL pmgr_action_CheckWaitShutdown(void)
{
	return xmgr_action_CheckWaitShutdown_Base();
}


