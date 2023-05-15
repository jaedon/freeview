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

#ifndef	__MGR_DYNAMICUPDATE_H__
#define	__MGR_DYNAMICUPDATE_H__

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




// for batch
BUS_Callback_t	MGR_DYNAMICUPDATE_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType);
HERROR	MGR_DYNAMICUPDATE_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType);
HERROR	MGR_DYNAMICUPDATE_RescheduleBatch(DxBatchType_e eBatchType);
HERROR MGR_DYNAMICUPDATE_GetBatchCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
HERROR MGR_DYNAMICUPDATE_GetBatchTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration);
HERROR MGR_DYNAMICUPDATE_SvcStop(Handle_t hAction);
BUS_Callback_t	MGR_DYNAMICUPDATE_GetProc(void);


#endif /* __MGR_DYNAMICUPDATE_H__ */

