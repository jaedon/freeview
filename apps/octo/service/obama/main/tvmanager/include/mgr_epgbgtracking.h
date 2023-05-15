/**
	@file     mgr_epgbgtracking.h
	@brief    file_name & simple comment.

	Description: Applicatuion for searching Services\n
	Module: Application(Layer), Menu(module) \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2013/03/27		initiated					chmin\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	__MGR_EPGBGTRACKING_H__
#define	__MGR_EPGBGTRACKING_H__

#include <mgr_common.h>
#include <mgr_action.h>

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Messages             ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/




HERROR	MGR_EPGBGTRACKING_Start (void);
HERROR	MGR_EPGBGTRACKING_Stop (void);
BUS_Callback_t  MGR_EPGBGTRACKING_GetProc (void);



// for batch
BUS_Callback_t	MGR_EPGBGTRACKING_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType);
HERROR	MGR_EPGBGTRACKING_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType);
HERROR	MGR_EPGBGTRACKING_RescheduleBatch(DxBatchType_e eBatchType);
HERROR MGR_EPGBGTRACKING_GetBatchCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
HERROR MGR_EPGBGTRACKING_GetBatchTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration);

#endif /* __MGR_EPGBGTRACKING_H__ */

