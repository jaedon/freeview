/**
	@file     mgr_emmupdate.h
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

#ifndef	__MGR_EMMUPDATE_H__
#define	__MGR_EMMUPDATE_H__

#include <mgr_common.h>
#include <mgr_action.h>
/*
	Related Config(s)
	CONFIG_APCORE_EMM_UPDATE : Dependent
*/

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
enum
{
	eMEVT_EMMUPDATE		= eMEVT_EMMUPDATE_START,

	/* - Description : EMM Update action done.
	   - Parameters Usage :
		   handle : N.A.
		   p1 : TBD
		   p2 : TBD
		   p3 : TBD */
	eMEVT_EMMUPDATE_DONE,
};

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
HERROR	MGR_EMMUpdate_Start (void);
HERROR	MGR_EMMUpdate_Stop (void);
HERROR	MGR_EMMUpdate_GetStartSvc (Handle_t *phSvc);

// Base Time : e.g. Current Time : 10:00 AM
// pulWakeupInteral 	: e.g. 1 Hour 		: Repeate EMM Update every 1hour. 11:00 AM, 12:00 PM, 1:00 PM, ...
// pulStartMin 			: e.g. 10 minutes	: Start Emm Update after 10 minutes. 11:10 AM, 12:10 PM, 1:10 PM, ...
// pulUpdateDuration 	: e.g. 30 minutes	: Do EMM Update For 30 minutes. 11:10 AM ~ 11:40 AM, 12:10 PM ~ 12:40 PM, ...
HERROR	MGR_EMMUpdate_GetConfiguration (HUINT32 *pulWakeupInterval, HUINT32 *pulStartMin, HUINT32 *pulUpdateDuration);

HERROR MGR_EMMUpdate_BatchDeleteExistSchedule(void);
HERROR MGR_EMMUpdate_BatchMakeSchedule (HxDATETIME_t *pstStartTime);

// For New Batch.
BUS_Callback_t			MGR_EMMUpdate_InitBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType);
HERROR					MGR_EMMUpdate_DeinitBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType);
HERROR					MGR_EMMUpdate_RescheduleBatch(DxBatchType_e eBatchType);
HERROR 					MGR_EMMUpdate_GetBatchCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
HERROR 					MGR_EMMUpdate_GetBatchTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration);


#endif /* __MGR_EMMUPDATE_H__ */

