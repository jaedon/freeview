/**
	@file     _xmgr_action.h
	@brief    file_name & simple comment.

	Module : 	\n
	Remarks :	\n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n

	Copyright (c) 2013 HUMAX Co., Ltd.				\n
	All rights reserved.							\n

Default Installation :

*/


#ifndef __XMGR_ACTION_INT_H__
#define __XMGR_ACTION_INT_H__

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>

#include <hapi.h>

#include <bus.h>

#include <mgr_action.h>
#include <pmgr_action.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/


/*******************************************************************/
/********************      Functions        ************************/
/*******************************************************************/

HUINT32				xmgr_action_GetMainViewId (void);
void					xmgr_action_SetMainViewId (HUINT32 ulViewId);
MgrAction_State_e		xmgr_action_GetState (void);
void					xmgr_action_SetState (MgrAction_State_e eState);

MgrAction_SvcDB_e	xmgr_action_GetSvcDbState(void);
void					xmgr_action_SetSvcDbState(MgrAction_SvcDB_e eSvcDB);

/*		Action Normal
 */
BUS_Callback_t		xmgr_action_NormalGetProc (void);
BUS_Result_t			xproc_action_Normal_Base(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3);
HERROR				xmgr_action_RegisterSiTimeoutCallback_Base(void);

/*		Action Standby
 */


HERROR				xmgr_action_GetLocalTime (UNIXTIME *pulLocalTime);

void					xmgr_action_PrintDateTime (UNIXTIME ulTime);
void					xmgr_action_PrintDuration (UNIXTIME ulStartTime, UNIXTIME ulEndTime);

HUINT32				xmgr_action_GetWakeupRcukey_Base(void);
HERROR				xmgr_action_GetSiTableTimeout(Handle_t hAction, SiTableType_e eTableType, HUINT32 *pulTimeout);

HERROR	xmgr_action_SiGetDefaultTimeout_Base(SiTableType_e eTableType, HUINT32 *pulTimeout);
HERROR	xmgr_action_SiGetDefaultTimeout_Mesat(SiTableType_e eTableType, HUINT32 *pulTimeout);
HERROR	xmgr_action_SiGetDefaultTimeout_Ziggo(SiTableType_e eTableType, HUINT32 *pulTimeout);
HERROR	xmgr_action_SiGetDefaultTimeout_Mostelecom(SiTableType_e eTableType, HUINT32 *pulTimeout);
HERROR	xmgr_action_SiGetDefaultTimeout_Tvt(SiTableType_e eTableType, HUINT32 *pulTimeout);
HERROR	xmgr_action_SiGetDefaultTimeout_Irdeto(SiTableType_e eTableType, HUINT32 *pulTimeout);
HERROR	xmgr_action_SiGetDefaultTimeout_NtvPlus(SiTableType_e eTableType, HUINT32 *pulTimeout);
HERROR	xmgr_action_SiGetDefaultTimeout_UkDtt(SiTableType_e eTableType, HUINT32 *pulTimeout);
HERROR	xmgr_action_SiGetDefaultTimeout_Japan(SiTableType_e eTableType, HUINT32 *pulTimeout);
HBOOL 	xmgr_action_CheckWaitShutdown_Base(void);


#endif	// __XMGR_ACTION_INT_H__

