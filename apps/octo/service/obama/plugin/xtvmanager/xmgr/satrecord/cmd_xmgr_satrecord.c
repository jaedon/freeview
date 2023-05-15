/**
	@file     cmd_xmgr_live.c
	@brief    Reservation manager command tool

	Description: 	Reservation Manager Debug.\n
	Module: live/ap								 	\n
	Remarks : 										\n

	Copyright (c) 2008 HUMAX Co., Ltd.				\n
	All rights reserved.							\n
*/


/********************************************************************
 Header Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>
	 
#include <htype.h>
#include <vkernel.h>
#include <hlib.h>
	 
#include <db_svc.h>
#include <db_param.h>
	 
#include <svc_pipe.h>
#include <svc_si.h>
#include <svc_cas.h>
	 
#include <bus.h>
	 
	 
#include <mgr_common.h>
	 
#include <mgr_rscmap.h>
#include <mgr_live.h>
	 
#include <_xmgr_satrecord_base.h>
/*	 
#include <mgr_live.h>
#include <xmgr_live.h>
#include "./local_include/_xmgr_live.h"
#include "./local_include/_xmgr_live_view.h"*/

/********************************************************************
 Macro Definition
 ********************************************************************/

/********************************************************************
 External Variable Declaration
 ********************************************************************/

/********************************************************************
 Static Function
 ********************************************************************/
/********************************************************************
 Global/Static Variable Definition
 ********************************************************************/


/************************
	Command 수행 함수
*************************/

/**********************************************
	Command 수행 함수를 등록하는 통합 등록 함수.
***********************************************/
void CMD_Register_Xmgr_SatRecord(void)
{
#if defined(CONFIG_DEBUG)
#define	hCmdHandle		"mgr_satrecord"

	//<< View context
	HLIB_CMD_RegisterWord(hCmdHandle,(void*)XMGR_SatRecord_DBG_MakeSchedule,
			"make_schedule",	/* keyword */
			"make satlite record type schedule", /*	 help  */
			"make_schedule");	/*	usage  */

#if 0//def CONFIG_DEBUG
	//<< Lock context
	HLIB_CMD_RegisterWord(hCmdHandle,(void*)xmgr_live_PrintLiveLockContext,
			"print_lock",	/* keyword */
			"print xmgr live lock context informations", /*	 help  */
			"print_lock");	/*	usage  */

	//<< View + Lock context
	HLIB_CMD_RegisterWord(hCmdHandle,(void*)xmgr_live_PrintWholeLiveContext,
			"print_live",	/* keyword */
			"print xmgr live view+lock context informations", /*	 help  */
			"print_live");	/*	usage  */

#endif
#endif
}

/* EOF */

