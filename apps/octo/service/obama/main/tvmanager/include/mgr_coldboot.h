/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  mgr_coldboot.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_COLDBOOT_H___
#define	___MGR_COLDBOOT_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
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
	 eMEVT_COLDBOOT			 = eMEVT_COLDBOOT_START,

	 /* - Description : 각 사양, 시나리오별 cold boot 처리 완료를 통보하는 메시지.
		- Parameters Usage :
			handle : N.A.
			p1 : TBD
			p2 : TBD
			p3 : TBD */
	eMEVT_COLDBOOT_NOTIFY_DONE,

	 /* - Description : 각 사양, 시나리오별 cold boot next process 처리 하라는 메시지..
		- Parameters Usage :
			handle : N.A.
			p1 : TBD
			p2 : TBD
			p3 : TBD */
	eMEVT_COLDBOOT_NEXTPROCESS
};

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/
BUS_Callback_t	MGR_ColdBoot_GetProc(void);
HERROR			R_ColdBoot_Start(void);
BUS_Callback_t	MGR_ColdBoot_InitColdBootBatch(void **ppvUserData, HUINT32 *pulUserDataLength, DxBatchType_e eBatchType);
HERROR			MGR_ColdBoot_DeinitColdBootBatch(void *pvUserData, HUINT32 ulUserDataLength, DxBatchType_e eBatchType);
HERROR			MGR_ColdBoot_RescheduleColdBootBatch(DxBatchType_e eBatchType);
HERROR 			MGR_ColdBoot_GetBatchColdBootCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
HERROR 			MGR_ColdBoot_GetBatchColdBootTime(UNIXTIME *ulStartTime, HUINT32 *ulDuration);

#endif /* !___MGR_COLDBOOT_H___ */

