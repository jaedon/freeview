/**************************************************************
*	@file	rema.c
*	http://www.humaxdigital.com
*	@author			humax
*	@brief			rema
**************************************************************/

/*
* ¨Ï 2011-2012 Humax Co., Ltd.
* This program is produced by Humax Co., Ltd. ("Humax") and
* the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
* non-assignable, non-transferable and non-exclusive license to use this Software.
* You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
* you agree to the responsibility to take all reasonable efforts to protect the any information
* you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
* reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
* If you have no authorized license, discontinue using this Software immediately.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
* IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
* IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
*  CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.
*
* This License is effective until terminated. You may terminate this License at any time by destroying all copies
* of the Software including all documentation. This License will terminate immediately without notice from Humax
* to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
* of the Software and all documentation.
*
* The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
* All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
* in the Republic of Korea.
*/

#define	________________Header_Files_______________________________
#include <vkernel.h>
#include <wait.h>
#include <papi.h>
#include <hlib.h>
#include <hapi.h>
#include <napi.h>






#include "rema_int.h"
#include "rema_cwmp.h"
#include "rema_core.h"




#include "rema_int.h"
#include "rema_core.h"

static void	rema_Msg_Run(void *arg);
static Rema_t* rema_GetInstance(void);
static void	rema_ReleaseInstance(Rema_t *rema);

#include "rema_cmd.c"


#define ________________Static_Variables___________________________

static int g_nRunMain = 1;
static int g_nRunRemaApk = 1;

static HCHAR szTr069AbortTmpFile[256]	= "/tmp/tr069_aborted";

static HINT32   cmd_stop_rema(void *szArgList)
{
	REM_FUNC_ENTER;

	g_nRunRemaApk=0;
	g_nRunMain = 0;

	REM_FUNC_LEAVE;
	return 0;
}

#define ________________Internal_Functions_________________________

static void  rema_CMD_Register_Main(void)
{
	REM_FUNC_ENTER;

	HLIB_CMD_RegisterWord( "rmain", cmd_stop_rema, "rquit", "rquit", "quit rema");

	REM_FUNC_LEAVE;
}
static void __rema_debug_conf(void)
{
	REM_FUNC_ENTER;
	//RemLOG_Print("\n\n\n ********** START REMA  __rema_debug_conf *********** \n\n\n");

	HLIB_DBG_Init();
	//RemLOG_Print("\n\n\n ********** START REMA  __rema_debug_conf rema_CMD_Init *********** \n\n\n");

	rema_CMD_Init();
	rema_CMD_Register_Main();

	REM_FUNC_LEAVE;
  return;

}

static void sig_handler(int signo)
{
	 RemLOG_Print("I Received SIGINT(%d)\n", SIGINT);
	//cmd_stop_rema(NULL);
}

static void	local_tr069_AbortHandler(int sig)
{
	RemLOG_Print("TR069 local_tr069_AbortHandler sig[%d]\n", sig);

	if (sig == SIGABRT) {
		HLIB_FILE_Make(szTr069AbortTmpFile);
	}
}

#define ________________Main_Function______________________________



static int rema_main(int argc, char *argv[])
{
	REM_FUNC_ENTER;

	RemLOG_Print("[REMA] Process Start!!!!\n");
	RemLOG_Print("\n\n\n ********** START REMA *********** \n\n\n");

	//	Ignore BROKEN PIPE signal.
	signal(SIGPIPE, SIG_IGN);

	//  Register Aboar Handler
	signal(SIGABRT, local_tr069_AbortHandler);

	VK_Init();

	HLIB_EXTRA_DropRootPrivileges("rema");

	HLIB_JSON_Init (eHLIB_JSON_Conf_Minimum, (void *(*)(unsigned long))HLIB_STD_MemAlloc_CB, (int (*)(void *))HLIB_STD_MemFree_CB);

	rema_core_Init();

	__rema_debug_conf();

	REM_FUNC_LEAVE;

	return 0;
}

static void  rema_term(void)
{
	REM_FUNC_ENTER;

//	HAPI_DeleteMyApplication(HLIB_STD_GetPID());
	rema_core_Deinit();

	REM_FUNC_LEAVE;
}

static void	rema_Msg_Run(void *arg)
{
	REM_FUNC_ENTER;

	HUINT32		ulMsgQueueId = (HUINT32)arg;
	Rema_t	*rema;
	REMA_MSG_t	msg;

	while(1){
		if (HLIB_MSG_ReceiveTimeout(ulMsgQueueId, &msg, sizeof(REMA_MSG_t), 400) == ERR_OK)
		{
			rema = rema_GetInstance();
			RemLOG_Print("\n\n\n rema_Msg_Run msg (%d) \n\n\n",msg.eMsg);
			switch (msg.eMsg)
			{
				case eREMA_TR069CwmpCpeOpen:
					rema_core_cpe_open(rema);
					break;
				case eREMA_TR069CwmpCpeClose:
					rema_core_cpe_close(rema);
					break;
			}
			RemLOG_Print("\n\n\n rema_Msg_Run Process Name (%s) \n\n\n",msg.pszProcessName);
			if (msg.pszProcessName){
				HLIB_STD_MemFree(msg.pszProcessName);
			}
			rema_ReleaseInstance(rema);
		}
	}
	REM_FUNC_LEAVE;
	return TRUE;
}


static Rema_t* rema_GetInstance(void)
{
	static Rema_t	*s_pstRema = NULL;

	if (s_pstRema == NULL)
	{
		unsigned long	tid;

		s_pstRema = (Rema_t*)HLIB_STD_MemCalloc(sizeof(Rema_t));
		HxSEMT_Create(&s_pstRema->ulSemId, "remasem", HxSEMT_FIFO);
		HxSEMT_Get(s_pstRema->ulSemId);

		HLIB_MSG_Create(REMA_MSGQ_SIZE, sizeof(REMA_MSG_t), "remaQ", (HULONG*)&s_pstRema->ulMainMsgQueue, eHLIB_SUSPENDTYPE_FIFO);

/*
		HAPI_AddVirtualWindow(eAWINDOW_PAMA);

		HAPI_ConnectSignal("signal:onBootupStateChanged",	NULL, pama_system_onBootupStateChanged);
		HAPI_ConnectSignal("signal:onMainApplicationReady", NULL, pama_system_onMainApplicationReady);

		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_POWER, TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_CHANNEL_UP, TRUE);
		HAPI_RegisterAcceptableKey(eAWINDOW_PAMA, DIKS_CHANNEL_DOWN, TRUE);
		HAPI_SetWindowStyle(eAWINDOW_PAMA, eHWS_WindowInputAlways, FALSE);
*/

		//	for main task
		VK_TASK_Create(rema_Msg_Run, VK_TASK_PRIORITY_MW_REF, SIZE_32K, "rema", (void*)s_pstRema->ulMainMsgQueue, &tid, 0);
		VK_TASK_Start(tid);
/*
		HLIB_RPC_RegisterCall(s_pstPama->nRpcHandle, "pama.setCommand", "iis", pama_set_cmd,
		s_pstPama, "set Command: (PxSTATUS_e cmd, HINT32 time, HCHAR * procName)");
*/
	}
	else
	{
		HxSEMT_Get(s_pstRema->ulSemId);
	}

	return s_pstRema;

}

static void	rema_ReleaseInstance(Rema_t *rema)
{
	HxSEMT_Release(rema->ulSemId);
}



int		main(int argc, char *argv[])
{
#if	defined(CONFIG_SUPPORT_REMA)
	rema_main(argc, argv);
	while (g_nRunMain)
	{
		HLIB_STD_TaskSleep(10000000);
	}

	rema_term();
#endif
	return 0;
}


//static HERROR


#define _____END_OF_FILE_________

