/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_time.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_TIME_H___
#define	___MGR_TIME_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <svc_ch.h>

#include <bus.h>

#include <mgr_common.h>

#include <mgr_action.h>
/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
enum
{
	eMEVT_TIME			= eMEVT_TIME_START,

	/* - Description : Clock Recover setup을 종료하고 날리는 메시지.
	   - Parameters Usage :
		   handle : NA
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_TIME_CLOCKRECOVER_SETUP_DONE,
};
/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HERROR	MGR_TIME_ClockRecoverStart (void);

HERROR	MGR_TIME_ClockRecoverStop(void);


HERROR	MGR_TIME_ClockRecoverTune (DxTuneParam_t *pTuningInfo);

HERROR	MGR_TIME_ClockRecoverGetDstMode (VK_CLOCK_DST_MODE *peDstMode);

HERROR	MGR_TIME_SetGmtOffset (HINT32 nGmtOffset, HBOOL bUpdateTzFile);

HERROR	MGR_TIME_Init (void);

// For Batch.
BUS_Callback_t			MGR_TIME_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType);
HERROR			MGR_TIME_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType);
HERROR			MGR_TIME_RescheduleBatch(DxBatchType_e eBatchType);
HERROR MGR_TIME_GetBatchTimeRecoveryCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
HERROR MGR_TIME_GetBatchTimeRecoveryTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration);
HERROR MGR_TIME_GetBatchAllTimeRecoveryCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList);

#endif /* !___MGR_TIME_H___ */

