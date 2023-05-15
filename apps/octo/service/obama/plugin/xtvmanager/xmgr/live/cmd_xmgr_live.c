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
//#include <si_util.h>
#include <svc_cas.h>

#include <bus.h>


#include <mgr_common.h>

#include <mgr_rscmap.h>
#include <mgr_live.h>
//#include <mwc_util.h>


#include <mgr_live.h>
#include <xmgr_live.h>
#include "./local_include/_xmgr_live.h"
#include "./local_include/_xmgr_live_view.h"

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

#if 0
static void	CMD_PrintXmgrLiveHelp(void)
{
	HLIB_CMD_Printf("===========================================\n");
	HLIB_CMD_Printf("============  xmgr live cmd start  =============\n\n");
	HLIB_CMD_Printf("print_live - print xmgr live context informations \n");
	HLIB_CMD_Printf("\n\n============  xmgr live cmd end  ===============\n");
	HLIB_CMD_Printf("===========================================\n\n");
}

/* TEMP */
static HERROR CMD_Xmgr_LIve_Cmd(void *szArgList)
{
	HCHAR	*aucArg=NULL;
	HERROR 	hErr = ERR_OK;
	HUINT8	 aucStr[20];
	/********************
	 * get 1st argument *
	 ********************/
	aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( sscanf(aucArg, "%s", (char *)&aucStr) == 0 ) return HxCMD_ERR;
	HLIB_STD_StrUpper(aucStr);

	if (aucStr  != NULL)
	{
		if(!strcmp(aucStr, "TEMP"))
		{
			// Do nothing
		}
		else
		{
			CMD_PrintXmgrLiveHelp();
			hErr = HxCMD_ERR;
		}

	}
	else
	{
		CMD_PrintXmgrLiveHelp();
		hErr = HxCMD_ERR;
	}
	return hErr;
}
#endif

/************************
	Command 수행 함수
*************************/

/**********************************************
	Command 수행 함수를 등록하는 통합 등록 함수.
***********************************************/
void CMD_Register_Xmgr_Live(void)
{
#define	hCmdHandle		"mgr_live"

#ifdef CONFIG_DEBUG
	//<< View context
	HLIB_CMD_RegisterWord(hCmdHandle,(void*)XMGR_LiveView_BASE_DBG_PrintContext,
			"print_view",	/* keyword */
			"print xmgr live view context informations", /*	 help  */
			"print_view");	/*	usage  */

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

	//<< mgr live context
	HLIB_CMD_RegisterWord(hCmdHandle,(void*)MGR_LIVE_PrintContext,
			"print_mgrlive",	/* keyword */
			"print xmgr live context informations", /*	 help  */
			"print_mgrlive");	/*	usage  */

	HLIB_CMD_RegisterWord(hCmdHandle,(void*)XMGR_LiveView_BASE_DBG_PrintCasDrmInfo,
			"print_drm",	/* keyword */
			"print xmgr live drm informations", /*	 help  */
			"print_drm");	/*	usage  */
#endif
}

/* EOF */

