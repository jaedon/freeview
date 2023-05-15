/**
	@file     mgr_dynamicupdate.c
	@brief    file_name & simple comment.

	Description:  \n
	Module:  \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n


	Copyright (c) 2009 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <bus.h>
#include <mgr_dynamicupdate.h>
#include <pmgr_dynamicupdate.h>

#include <svc_pipe.h>
#include <mgr_action.h>
#include <mgr_live.h>
#include <svc_si.h>

#include "local_include/_mgr_dynamicupdate.h"

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
/********************      	Module internal		 	********************/
/*******************************************************************/

/*******************************************************************/
/********************      	Global Function 	API    ********************/
/*******************************************************************/

// TODO: db_svc 를 보는 것은 wrapper를 둬서 dx_service를 보도록 하고 다 없앤다.
// TODO: Danamic update에서 delete 되는 service에 대한 처리들. 추가해야함.
// TODO: db_svc 와 dx_service가 다르다. dx_service에는 user delete 된 svc에 대한 처리가 없다. 추가해줘야 함.
// TODO: dynamic update proc의 on/off 기능에 대한 고려 필요.


// for batch
BUS_Callback_t	MGR_DYNAMICUPDATE_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType)
{
	HERROR	hErr = ERR_FAIL;

	hErr = pmgr_dynamicupdate_Start();
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("pmgr_dynamicupdate_Start error \n");
		return NULL;
	}

	return pmgr_dynamicupdate_GetProc();
}

HERROR	MGR_DYNAMICUPDATE_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType)
{
	HERROR	hErr = ERR_FAIL;

	// deinit은 이미 dynamic update 가  kill 된 이후에 불릴 것이기 때문에, err를 탈 것이지만...일단 넣어둔다.
	hErr = pmgr_dynamicupdate_Stop();
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("pmgr_dynamicupdate_Stop error, it might be destroyed already. \n");
	}

	return hErr;
}

HERROR	MGR_DYNAMICUPDATE_RescheduleBatch(DxBatchType_e eBatchType)
{
	// nothing to do.
	return ERR_OK;
}

HERROR MGR_DYNAMICUPDATE_GetBatchCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	fnBatchProcCBList->fnAppInit = MGR_DYNAMICUPDATE_InitBatch;
	fnBatchProcCBList->fnAppDeinit = MGR_DYNAMICUPDATE_DeinitBatch;
	fnBatchProcCBList->fnAppReSchedule = MGR_DYNAMICUPDATE_RescheduleBatch;

	return ERR_OK;
}

HERROR MGR_DYNAMICUPDATE_SvcStop(Handle_t hAction)
{
	return pmgr_dynamicupdate_SvcStop(hAction);
}

HERROR MGR_DYNAMICUPDATE_GetBatchTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration)
{
	// nothing to do.
	return ERR_OK;
}

BUS_Callback_t	MGR_DYNAMICUPDATE_GetProc(void)
{
	return pmgr_dynamicupdate_GetProc();
}

/*********************** End of File ******************************/

