/**
	@file     mgr_dynamicupdate.h
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

#ifndef	__MGR_EPGGATHERING_H__
#define	__MGR_EPGGATHERING_H__

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



#if 0
HERROR	MGR_EPGGATHERING_Start (void);
HERROR	MGR_EPGGATHERING_Stop (void);
#endif

// for batch
BUS_Callback_t	MGR_EPGGATHERING_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType);
HERROR	MGR_EPGGATHERING_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType);
HERROR	MGR_EPGGATHERING_RescheduleBatch(DxBatchType_e eBatchType);
HERROR MGR_EPGGATHERING_GetBatchCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
HERROR MGR_EPGGATHERING_GetBatchTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration);

#endif /* __MGR_EPGGATHERING_H__ */

