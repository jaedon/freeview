/*******************************************************************
	File Description
********************************************************************/
/**
	@file	  _mgr_action.h
	@brief

	Description:			\n
	Module: 			 	\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_ACTION_INT_H___
#define	___MGR_ACTION_INT_H___


/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <db_svc.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/
#define PROC_PARAM_WARNING_REMOVE(arg1, arg2, arg3, arg4, arg5)	\
			{													\
				(void)arg1;										\
				(void)arg2;										\
				(void)arg3;										\
				(void)arg4;				 						\
				(void)arg5;				 						\
			}

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

HUINT32		mgr_action_GetMainViewId (void);
void		mgr_action_SetMainViewId (HUINT32 ulViewId);
MgrAction_State_e	mgr_action_GetState (void);

HERROR		MGR_ACTION_BatchStart (DxBatchType_e eBatchType);
#if 0
HERROR		mgr_action_BatchStart (DxBatchType_e eBatchType);
#endif
HERROR		MGR_Action_BatchInstallProc(DxBatchType_e eBatchType, DxBatchProc_b * eBatchProc);

HERROR		mgr_action_RegisterSiTimeoutCallback(void);


#endif /* !___MGR_ACTION_INT_H___ */

