/**
	@file     mgr_emmupdate.c
	@brief    file_name & simple comment.

	Description: for portal UI \n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2009/07/11		modified					jhlee\n

	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <hapi.h>
#include <papi.h>

#include <db_svc.h>

#include <svc_pipe.h>
#include <svc_resources.h>
#include <svc_ch.h>
#include <svc_si.h>
#include <svc_epg.h>
#include <svc_cas.h>

#include <bus.h>

#include <mgr_common.h>
#include <mgr_action.h>
#include <mgr_live.h>

#include <mgr_rscmap.h>
#include <mgr_cas.h>
#include <mgr_emmupdate.h>

#include "./local_include/_xmgr_emmupdate.h"
#if defined (CONFIG_OP_SES)
#include "../cas/local_include/_xmgr_cas.h"
#endif

#if defined(CONFIG_MW_CAS)
#include <xmgr_cas.h>
#endif
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
/********************      	Static Function 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

HERROR	xmgr_emmupdate_GetStartSvc(Handle_t *phSvc)
{
	HERROR		hError;

	if (NULL == phSvc)
		return ERR_FAIL;

	// �̰� �ٲ�� �ϴµ�
	hError = DB_PARAM_GetItem(eDB_PARAM_ITEM_CUR_SVC, (HUINT32 *)phSvc, 0);	// lastsvc
	if( hError == ERR_OK )
	{
		hError = DB_SVC_CheckValidServiceHandle(*phSvc);
		if(hError == ERR_OK)
		{
			HxLOG_Print("[%s: %04d] Valid Service Handle(0x%x) \n", __FUNCTION__, __LINE__, *phSvc);
		}
		else
		{
			*phSvc = HANDLE_NULL;
			HxLOG_Print("[%s: %04d] Invalid Service Handle(0x%x) \n", __FUNCTION__, __LINE__, *phSvc);
			return ERR_FAIL;
		}
	}
	else
	{
		*phSvc = HANDLE_NULL;
		HxLOG_Print("[%s: %04d] not existing current service \n", __FUNCTION__, __LINE__);
		return ERR_FAIL;
	}

	return ERR_OK;
}

BUS_Callback_t	XMGR_EMMUpdate_GetProc(void)
{
	BUS_Callback_t	pfnProc = NULL;

#if defined(CONFIG_OP_ZIGGO_EMM_UPDATE)
	pfnProc = xmgr_emmupdate_GetProc_Base();
#elif defined(CONFIG_MW_CAS)
	pfnProc = XMGR_CAS_GetEmmupdateProc();
	if (NULL == pfnProc)
		pfnProc = xmgr_emmupdate_GetProc_Base();
#else
	pfnProc = xmgr_emmupdate_GetProc_Base();
#endif

	return pfnProc;
}

BUS_Callback_t	xmgr_emmupdate_GetProc_Cas(void)
{
#if defined(CONFIG_MW_CAS)
	BUS_Callback_t		pfnProc;

	pfnProc = XMGR_CAS_GetEmmupdateProc();
	if (NULL == pfnProc)
		pfnProc = xmgr_emmupdate_GetProc_Base();

	return pfnProc;
#else
	return xmgr_emmupdate_GetProc_Base();
#endif
}


HERROR	xmgr_emmupdate_Start_Cas(void)
{
#if defined(CONFIG_MW_CAS)
	BUS_Callback_t		pfnProc;
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	pfnProc = XMGR_CAS_GetEmmupdateProc();
	if (NULL == pfnProc)
		pfnProc = xmgr_emmupdate_GetProc_Base();

	eRes = BUS_MGR_Create((char*)"proc_emmupdate_Action", APP_ACTION_PRIORITY, pfnProc, HANDLE_NULL, 0, 0, 0);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
#else
	return ERR_FAIL;
#endif
}


HERROR	xmgr_emmupdate_Stop_Cas(void)
{
#if defined(CONFIG_MW_CAS)
	BUS_Callback_t		pfnProc;
	BUS_Result_t		eRes = ERR_BUS_SOURCE_NULL;

	pfnProc = XMGR_CAS_GetEmmupdateProc();
	if (NULL == pfnProc)
		pfnProc = xmgr_emmupdate_GetProc_Base();

	eRes = BUS_MGR_Destroy(pfnProc);

	return (eRes != ERR_BUS_NO_ERROR) ? ERR_FAIL : ERR_OK;
#else
	return ERR_FAIL;
#endif
}




HERROR	xmgr_emmupdate_GetConfiguration_Cas(HUINT32 *pulWakeupInterval, HUINT32 *pulStartMin, HUINT32 *pulUpdateDuration)
{
#if defined(CONFIG_MW_CAS)

#if defined(CONFIG_MW_CAS_NAGRA)
	#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
		return xmgr_cas_NaEmmUpdate_GetConfiguration(pulWakeupInterval, pulStartMin, pulUpdateDuration);
	#else
		if (pulWakeupInterval)		*pulWakeupInterval		= (0);
		if (pulStartMin)		*pulStartMin		= (0);
		if (pulUpdateDuration)	*pulUpdateDuration	= (0);
	#endif
#else
		return xmgr_emmupdate_GetConfiguration_Base(pulWakeupInterval, pulStartMin, pulUpdateDuration);
#endif


#else
		return ERR_FAIL;
#endif

}


HERROR	xmgr_emmupdate_GetBatchCBList_Cas(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
#if defined(CONFIG_MW_CAS)

#if defined(CONFIG_MW_CAS_NAGRA)
	#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
		// init �Լ��� ���ٴ� ����, ���� batch ������ ���� �ʰ�, batch mgr���� finish proc���� �ٷ� ����.
		// �� ��, reschedule �Լ��� �θ��鼭, ���� ��� interval �Ŀ� emm update�� ���� �����ϰ� sapi�� ������ �Ǵ�.
		// sapi���� �ð��� �Ǽ� ����, �� �� init �Լ��� batch �� �ž��ִ� ���̴�. (xmgr_schedule_base.c)
		fnBatchProcCBList->fnAppInit = NULL;
		fnBatchProcCBList->fnAppDeinit = NULL;
		fnBatchProcCBList->fnAppReSchedule = MGR_EMMUpdate_RescheduleBatch;
		return ERR_OK;
	#else
		// hd9000i �� ��� batch �� �� �����ϰ�, ������ ���ӵȴ�.
		fnBatchProcCBList->fnAppInit = MGR_EMMUpdate_InitBatch;
		fnBatchProcCBList->fnAppDeinit = MGR_EMMUpdate_DeinitBatch;
		fnBatchProcCBList->fnAppReSchedule = MGR_EMMUpdate_RescheduleBatch;
		return ERR_OK;
	#endif
#elif defined(CONFIG_MW_CAS_CONAX)
{
	HBOOL bPowerSavingStandby = FALSE;

#if defined(CONFIG_APCORE_ALWAYS_ACTIVE_STANDBY)
	bPowerSavingStandby = FALSE;
#else
	PAPI_GetPowerSavingOnStandby(&bPowerSavingStandby);
#endif

	if(bPowerSavingStandby != TRUE)
	{
		// CONAX�� ��� ���� EMM update ������ Active Standby ���¿��� ���� ���� ������ �ȴ�.
		// batch���� ������ ���ӵǵ��� �Ѵ�.
		fnBatchProcCBList->fnAppInit = MGR_EMMUpdate_InitBatch;
		fnBatchProcCBList->fnAppDeinit = MGR_EMMUpdate_DeinitBatch;
		fnBatchProcCBList->fnAppReSchedule = MGR_EMMUpdate_RescheduleBatch;
		return ERR_OK;
	}
	else
	{
		return xmgr_emmupdate_GetBatchCBList_Base(fnBatchProcCBList);
	}
}
#else
		return xmgr_emmupdate_GetBatchCBList_Base(fnBatchProcCBList);
#endif


#else
		return ERR_FAIL;
#endif

}


HERROR	xmgr_emmupdate_RescheduleBatch_Cas(void)
{
#if defined(CONFIG_MW_CAS)

#if defined(CONFIG_MW_CAS_NAGRA)
	#if defined(CONFIG_MW_CAS_NAGRA_IRDCMD_EMMWAKEUP)
			return xmgr_cas_NaEmmUpdateRescheduleBatch();
	#else
			return xmgr_emmupdate_RescheduleBatch_Base();
	#endif
#else
	return xmgr_emmupdate_RescheduleBatch_Base();
#endif


#else
	return ERR_FAIL;
#endif
}


/*********************** End of File ******************************/

