/**
	@file     ap_skyd_emmupdate_mgr.c
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
#include <bus.h>
#include <mgr_emmupdate.h>
#include <mgr_action.h>
#include <pmgr_emmupdate.h>
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

#define	________________Static_Function_________________________

/*******************************************************************/
/********************      	Static Function 	********************/
/*******************************************************************/

#define	________________Global_Function_________________________

/*******************************************************************/
/********************      	Global Function 	********************/
/*******************************************************************/

HERROR	MGR_EMMUpdate_Start(void)
{
	return pmgr_emmupdate_Start();
}

HERROR	MGR_EMMUpdate_Stop(void)
{
	return pmgr_emmupdate_Stop();
}

HERROR	MGR_EMMUpdate_GetStartSvc (Handle_t *phSvc)
{
	if (NULL == phSvc)
		return ERR_FAIL;

	return pmgr_emmupdate_GetStartSvc (phSvc);
}

HERROR	MGR_EMMUpdate_GetConfiguration (HUINT32 *pulWakeupInterval, HUINT32 *pulStartMin, HUINT32 *pulUpdateDuration)
{
	return pmgr_emmupdate_GetConfiguration(pulWakeupInterval, pulStartMin, pulUpdateDuration);
}


HERROR MGR_EMMUpdate_BatchDeleteExistSchedule(void)
{
	return pmgr_emmupdate_BatchDeleteExistSchedule();
}

HERROR MGR_EMMUpdate_BatchMakeSchedule (HxDATETIME_t *pstStartTime)
{
	return pmgr_emmupdate_BatchMakeSchedule(pstStartTime);
}

// For Batch.
BUS_Callback_t			MGR_EMMUpdate_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType) // Batch 에서만 사용할 init 함수
{
	HERROR	hErr = ERR_FAIL;

	hErr = pmgr_emmupdate_Start();
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("pmgr_emmupdate_Start error \n");
		return NULL;
	}

	return pmgr_emmupdate_GetProc();
}

HERROR			MGR_EMMUpdate_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType)
{
	HERROR	hErr = ERR_FAIL;


	// deinit은 이미 emm update가  kill 된 이후에 불릴 것이기 때문에, err를 탈 것이지만...일단 넣어둔다.
	hErr = pmgr_emmupdate_Stop();
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("pmgr_emmupdate_Stop error, it might be destroyed already. \n");
	}

	// 여기서 sapi로 예약 건 emm schedule을 지우지는 않는다.
	// batch 에서 deinit은 go operation, exit standby 일 경우 불리므로, 원래 지워져야 한다고 생각할 수 있지만,
	// batch 가 정상 종료할 때도 deinit이 불리기 때문에, 여기서 schedule을 지워버리면 안된다.
	// 그대신 xmgr schedule 에서 active standby 일 때만 emm 예약 처리하도록 되어 있으므로 괜찮다.
	// 그리고, reschedule 에서도, 이전의 emm 예약을 지우는 코드가 있다.
	return hErr;
}


HERROR			MGR_EMMUpdate_RescheduleBatch(DxBatchType_e eBatchType)
{


	HxLOG_Trace();
	return pmgr_emmupdate_RescheduleBatch();
}

HERROR MGR_EMMUpdate_GetBatchCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	return pmgr_emmupdate_GetBatchCBList(fnBatchProcCBList);
}

HERROR MGR_EMMUpdate_GetBatchTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration)
{
	// nothing to do.
	return ERR_OK;
}


/*********************** End of File ******************************/

