/**
	@file     mgr_epggathering.c
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
#include <mgr_epggathering.h>
#include <pmgr_epggathering.h>

#include <svc_pipe.h>
#include <mgr_action.h>

#include "local_include/_mgr_epggathering.h"

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

#if 0
HERROR	MGR_EPGGATHERING_Start(void)
{
	HxLOG_Print("(+)\n\n");

	return pmgr_epggathering_Start();
}

HERROR	MGR_EPGGATHERING_Stop(void)
{
	HxLOG_Print("(-)\n\n");

	return pmgr_epggathering_Stop();
}
#endif

// for batch
BUS_Callback_t	MGR_EPGGATHERING_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType)
{
	HERROR	hErr = ERR_FAIL;

	hErr = pmgr_epggathering_Start();
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("pmgr_epggathering_Start error \n");
		return NULL;
	}

	return pmgr_epggathering_GetProc();
}

HERROR	MGR_EPGGATHERING_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType)
{
	HERROR	hErr = ERR_FAIL;

	// deinit은 이미 epg gathering이  kill 된 이후에 불릴 것이기 때문에, err를 탈 것이지만...일단 넣어둔다.
	hErr = pmgr_epggathering_Stop();
	if(hErr == ERR_FAIL)
	{
		HxLOG_Error("pmgr_emmupdate_Stop error, it might be destroyed already. \n");
	}

	return hErr;
}

HERROR	MGR_EPGGATHERING_RescheduleBatch(DxBatchType_e eBatchType)
{
	// nothing to do.
	return ERR_OK;
}

HERROR MGR_EPGGATHERING_GetBatchCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList)
{
	fnBatchProcCBList->fnAppInit = MGR_EPGGATHERING_InitBatch;
	fnBatchProcCBList->fnAppDeinit = MGR_EPGGATHERING_DeinitBatch;
	fnBatchProcCBList->fnAppReSchedule = MGR_EPGGATHERING_RescheduleBatch;

	return ERR_OK;
}

HERROR MGR_EPGGATHERING_GetBatchTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration)
{
	// nothing to do.
	return ERR_OK;
}



/*********************** End of File ******************************/

