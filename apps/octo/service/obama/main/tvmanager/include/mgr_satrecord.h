/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_satrecord.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef __MGR_SAPRECORD_H__
#define __MGR_SAPRECORD_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <svc_si.h>
#include <mgr_action.h>

enum
{
	eMEVT_SATRECORD			= eMEVT_SATRECORD_START,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_SATRECORD_NOTIFY_DETECT_FINISH,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_SATRECORD_ACTION_START,

};


/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/

BUS_Callback_t		MGR_SATRECORD_GetDownloadAction(void);
HERROR				MGR_SATRECORD_StartDetectPID (void);
HERROR				MGR_SATRECORD_StartDownloadSatRec(void);
HERROR				MGR_SATRECORD_DestroyDetectPid(void);
HERROR				MGR_SATRECORD_GetSatRecSiSpec (SvcSi_SatRecordSpec_t *pstSiSpec);
HERROR				MGR_SATRECORD_GetBatchSatRecCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
HERROR				MGR_SATRECORD_GetBatchSatRecTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration);
BUS_Callback_t		MGR_SATRECORD_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType); // Batch 에서만 사용할 init 함수
HERROR				MGR_SATRECORD_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType); // Batch 에서만 사용할 deinit 함수
HERROR				MGR_SATRECORD_RescheduleBatch(DxBatchType_e eBatchType); // Batch 에서만 사용할 reschedule 함수
#endif /* __MGR_SAPRECORD_H__ */
