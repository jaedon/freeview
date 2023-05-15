/**
	@file     mgr_hbbtv.h
	@brief    file_name & simple comment.

	Description: \n
	Module:  \n
	Remarks : \n
	 Date			Modification				Name\n
	-----------	-------------------------------------	------------------\n
	 2016/04/14		initiated					Jinh\n

	Copyright (c) 2016 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/

#ifndef	___MGR_HBBTV_H___
#define	___MGR_HBBTV_H___

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#include <mgr_action.h>

/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/

enum
{
	eMEVT_HBBTV			= eMEVT_HBBTV_START,

	/* - Description :
	   - Parameters Usage :
		   handle : NA
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_HBBTV_STOP_MONITORING,

	/* - Description :
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_HBBTV_NOTIFY_SESSION_STARTED,

	/* - Description :
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_HBBTV_NOTIFY_SESSION_STOPPED,

	/* - Description :
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_HBBTV_NOTIFY_PMT,

	/* - Description :
	   - Parameters Usage :
		   handle : Action Handle
		   p1 : NA
		   p2 : NA
		   p3 : NA */
	eMEVT_HBBTV_NOTIFY_AIT,


	eMEVT_HBBTV_END
};

typedef enum
{
	eMgrHbbtvAitState_None		= 0,

	eMgrHbbtvAitState_NoAit,
	eMgrHbbtvAitState_Timeout,
	eMgrHbbtvAitState_Exist,

	eMgrHbbtvAitState_EndOfCase
} MgrHbbtv_AitState_e;

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

extern HERROR	MGR_HBBTV_Init (void);
extern HERROR	MGR_HBBTV_GetActionId (HUINT32 *pulActionId);


#endif /* ___MGR_HBBTV_H___ */

