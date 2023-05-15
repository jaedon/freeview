/**
	@file     mgr_epgbgtracking.c
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
#include <mgr_epgbgtracking.h>
#include <pmgr_epgbgtracking.h>

#include <svc_pipe.h>
#include <mgr_action.h>

#include "local_include/_mgr_epgbgtracking.h"

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


HERROR	MGR_EPGBGTRACKING_Start(void)
{
	return pmgr_epgbgtracking_Start();
}

HERROR	MGR_EPGBGTRACKING_Stop(void)
{
	return pmgr_epgbgtracking_Stop();
}

BUS_Callback_t  MGR_EPGBGTRACKING_GetProc (void)
{
	return pmgr_epgbgtracking_GetProc();
}



// for batch
BUS_Callback_t	MGR_EPGBGTRACKING_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType)
{
	HERROR	hErr = ERR_FAIL;

	hErr = pmgr_epgbgtracking_Start();
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("pmgr_epgbgtracking_Start error \n");
		return NULL;
	}

	return pmgr_epgbgtracking_GetProc();
}

HERROR	MGR_EPGBGTRACKING_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType)
{
	HERROR	hErr = ERR_FAIL;

	hErr = pmgr_epgbgtracking_Stop();
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("pmgr_epgbgtracking_Stop error, it might be destroyed already. \n");
	}

	return hErr;
}

HERROR	MGR_EPGBGTRACKING_RescheduleBatch(DxBatchType_e eBatchType)
{
	// nothing to do.
	return ERR_OK;
}

HERROR MGR_EPGBGTRACKING_GetBatchCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	fnBatchProcCBList->fnAppInit = MGR_EPGBGTRACKING_InitBatch;
	fnBatchProcCBList->fnAppDeinit = MGR_EPGBGTRACKING_DeinitBatch;
	fnBatchProcCBList->fnAppReSchedule = MGR_EPGBGTRACKING_RescheduleBatch;

	return ERR_OK;
}

HERROR MGR_EPGBGTRACKING_GetBatchTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration)
{
	// nothing to do.
	return ERR_OK;
}



/*********************** End of File ******************************/

