/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  pmgr_emmupdate.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___PMGR_EMMUPDATE_H___
#define	___PMGR_EMMUPDATE_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

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
/********************      Plug-In Functions    ********************/
/*******************************************************************/
BUS_Callback_t		pmgr_emmupdate_GetProc(void);
HERROR	pmgr_emmupdate_Start (void);
HERROR	pmgr_emmupdate_Stop (void);

HERROR	pmgr_emmupdate_GetStartSvc (Handle_t *phSvc);
HERROR	pmgr_emmupdate_GetConfiguration (HUINT32 *pulWakeupInterval, HUINT32 *pulStartMin, HUINT32 *pulUpdateDuration);

HERROR pmgr_emmupdate_BatchDeleteExistSchedule(void);
HERROR pmgr_emmupdate_BatchMakeSchedule (HxDATETIME_t *pstStartTime);

HERROR pmgr_emmupdate_GetBatchCBList(MgrAction_BatchProcCBList_t * fnBatchProcCBList);
HERROR	pmgr_emmupdate_RescheduleBatch(void);

#endif /* !___PMGR_EMMUPDATE_H___ */

