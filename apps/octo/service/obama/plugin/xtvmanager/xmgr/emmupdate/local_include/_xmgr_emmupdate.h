/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _xmgr_emmupdate.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___XMGR_EMMUPDATE_INT_H___
#define	___XMGR_EMMUPDATE_INT_H___

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
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/*	Common Functions
 */
HERROR	xmgr_emmupdate_GetStartSvc(Handle_t *phSvc);

/*	Base Functions
 */
BUS_Callback_t xmgr_emmupdate_GetProc_Base(void);
HERROR	xmgr_emmupdate_Start_Base(void);
HERROR	xmgr_emmupdate_Stop_Base(void);
HERROR	xmgr_emmupdate_GetConfiguration_Base (HUINT32 *pulWakeupInterval, HUINT32 *pulStartMin, HUINT32 *pulUpdateDuration);

BUS_Callback_t	xmgr_emmupdate_GetProc_Cas(void);
HERROR	xmgr_emmupdate_Start_Cas(void);
HERROR	xmgr_emmupdate_Stop_Cas(void);

BUS_Callback_t	xmgr_emmupdate_GetProc_Ziggo(void);
HERROR	xmgr_emmupdate_Start_Ziggo(void);
HERROR	xmgr_emmupdate_Stop_Ziggo(void);
HERROR	xmgr_emmupdate_GetConfiguration_Ziggo (HUINT32 *pulWakeupInterval, HUINT32 *pulStartMin, HUINT32 *pulUpdateDuration);


HERROR	xmgr_emmupdate_GetConfiguration_Cas(HUINT32 *pulWakeupInterval, HUINT32 *pulStartMin, HUINT32 *pulUpdateDuration);

HERROR xmgr_emmupdate_BatchDeleteExistSchedule(void);
HERROR xmgr_emmupdate_BatchMakeSchedule (HxDATETIME_t *pstStartTime);

HERROR	xmgr_emmupdate_GetBatchCBList_Ziggo(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
HERROR	xmgr_emmupdate_GetBatchCBList_Cas(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
HERROR	xmgr_emmupdate_GetBatchCBList_Base(MgrAction_BatchProcCBList_t * fnBatchProcCBList);

HERROR	xmgr_emmupdate_RescheduleBatch_Ziggo(void);
HERROR	xmgr_emmupdate_RescheduleBatch_Cas(void);
HERROR	xmgr_emmupdate_RescheduleBatch_Base(void);


#endif /* !___XMGR_EMMUPDATE_INT_H___ */

