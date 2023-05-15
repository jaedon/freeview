/**
@file     pmgr_emmupdate.c
@brief Installation wizard install option(simple/advanced) configuration file.

Description:  \n
Module: Application(Layer), util(module) \n
Remarks : \n

Copyright (c) 2009 HUMAX Co., Ltd.				\n
All rights reserved.							\n
*/

#include <bus.h>

#include <mgr_emmupdate.h>
#include <xmgr_cas.h>
#include "./local_include/_xmgr_emmupdate.h"

BUS_Callback_t		pmgr_emmupdate_GetProc(void)
{
#if defined(CONFIG_OP_ZIGGO_EMM_UPDATE)
	return xmgr_emmupdate_GetProc_Ziggo();
#elif defined(CONFIG_MW_CAS)
	return xmgr_emmupdate_GetProc_Cas();
#else
	return xmgr_emmupdate_GetProc_Base();
#endif
}

HERROR	pmgr_emmupdate_Start(void)
{
#if defined(CONFIG_OP_ZIGGO_EMM_UPDATE)
	return xmgr_emmupdate_Start_Ziggo();
#elif defined(CONFIG_MW_CAS)
	return xmgr_emmupdate_Start_Cas();
#else
	return xmgr_emmupdate_Start_Base();
#endif
}

HERROR	pmgr_emmupdate_Stop(void)
{
#if defined(CONFIG_OP_ZIGGO_EMM_UPDATE)
	return xmgr_emmupdate_Stop_Ziggo();
#elif defined(CONFIG_MW_CAS)
	return xmgr_emmupdate_Stop_Cas();
#else
	return xmgr_emmupdate_Stop_Base();
#endif
}

HERROR	pmgr_emmupdate_GetStartSvc (Handle_t *phSvc)
{
	return xmgr_emmupdate_GetStartSvc(phSvc);
}

HERROR	pmgr_emmupdate_GetConfiguration(HUINT32 *pulWakeupInterval, HUINT32 *pulStartMin, HUINT32 *pulUpdateDuration)
{
#if defined(CONFIG_OP_ZIGGO_EMM_UPDATE)
	return xmgr_emmupdate_GetConfiguration_Ziggo (pulWakeupInterval, pulStartMin, pulUpdateDuration);
#elif defined(CONFIG_MW_CAS)
	return xmgr_emmupdate_GetConfiguration_Cas(pulWakeupInterval, pulStartMin, pulUpdateDuration);
#else
	return xmgr_emmupdate_GetConfiguration_Base (pulWakeupInterval, pulStartMin, pulUpdateDuration);
#endif
}

HERROR pmgr_emmupdate_BatchDeleteExistSchedule(void)
{
	return xmgr_emmupdate_BatchDeleteExistSchedule();
}

HERROR pmgr_emmupdate_BatchMakeSchedule (HxDATETIME_t *pstStartTime)
{
	return xmgr_emmupdate_BatchMakeSchedule(pstStartTime);
}

HERROR pmgr_emmupdate_GetBatchCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{

#if defined(CONFIG_OP_ZIGGO_EMM_UPDATE)
	return xmgr_emmupdate_GetBatchCBList_Ziggo(fnBatchProcCBList);
#elif defined(CONFIG_MW_CAS)
	return xmgr_emmupdate_GetBatchCBList_Cas(fnBatchProcCBList);
#else
	return xmgr_emmupdate_GetBatchCBList_Base(fnBatchProcCBList);
#endif


}

HERROR			pmgr_emmupdate_RescheduleBatch(void)
{
#if defined(CONFIG_OP_ZIGGO_EMM_UPDATE)
		return xmgr_emmupdate_RescheduleBatch_Ziggo();
#elif defined(CONFIG_MW_CAS)
		return xmgr_emmupdate_RescheduleBatch_Cas();
#else
		return xmgr_emmupdate_RescheduleBatch_Base();
#endif

}

