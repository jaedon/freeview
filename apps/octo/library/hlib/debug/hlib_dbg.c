/**************************************************************
*	@file		hxdbg.c
*
*	http://www.humaxdigital.com
*
*	@data			2012/10/24
*	@author			humax
**************************************************************/


/*
* Character encoding.
*
* MS949
*
* This source file that uses MS949 encoding.
* MS949 encoding 을 사용하는 source file 입니다.
* MS949 encodingを使用して source fileです。
* Quelldatei, die MS949-Codierung verwendet.
*
*/


/*
* ⓒ 2011-2012 Humax Co., Ltd.
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


/**********************************************************************************************************/
#define ____INCLUDE_______________________
/**********************************************************************************************************/
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <execinfo.h>
#include <string.h>
#include <sys/types.h>
#include <sys/resource.h>

#include <hlib.h>
#include <_hlib_dbg.h>
#include <_hlib_mem.h>
#include <vkernel.h>

/**********************************************************************************************************/
#define ____INTERNAL_STRUCTURES_AND_VARIABLES_______________________
/**********************************************************************************************************/
#ifdef	USE_VK_SIG
#else
static void		(*s_pfnSignalUserHandler)(int signal, siginfo_t *info, void *context) = NULL;			// Signal Handler for user.
#endif



typedef struct {
    HUINT32         sema;
    HxHASH_t        *pTimer;
    HLIB_DBG_INFO_t *pKiller;
    HxQueue_t       *pHistory;
}HxDbg_Instance;


/**********************************************************************************************************/
#define ____PRIVATE_FUNCTIONS_______________________
/**********************************************************************************************************/
#define MAX_TIMER_NUM 50

#ifdef	ENABLE_DEBUG
#ifdef	USE_VK_SIG
#else
static inline char*	hlib_dbg_getReason(int code, int signal)
{
	switch(code)
	{
		case SI_USER:
			return "kill, sigsend or raise ";
		case SI_KERNEL:
			return "kernel";
		case SI_QUEUE:
			return "sigqueue";
	}

	if(SIGILL==signal)
	{
		switch(code)
		{
			case ILL_ILLOPC:
				return "illegal opcode";
			case ILL_ILLOPN:
				return "illegal operand";
			case ILL_ILLADR:
				return "illegal addressing mode";
			case ILL_ILLTRP:
				return "illegal trap";
			case ILL_PRVOPC:
				return "privileged register";
			case ILL_COPROC:
				return "coprocessor error";
			case ILL_BADSTK:
				return "internal stack error";
		}
	}

	if(SIGFPE==signal)
	{
		switch(code)
		{
			case FPE_INTDIV:
				return "integer divide by zero";
			case FPE_INTOVF:
				return "integer overflow";
			case FPE_FLTDIV:
				return "floating point divide by zero";
			case FPE_FLTOVF:
				return "floating point overflow";
			case FPE_FLTUND:
				return "floating point underflow";
			case FPE_FLTRES:
				return "floating point inexact result";
			case FPE_FLTINV:
				return "floating point invalid operation";
			case FPE_FLTSUB:
				return "subscript out of range";
		}
	}

	if(SIGSEGV==signal)
	{
		switch(code)
		{
			case SEGV_MAPERR:
				return "address not mapped to object";
			case SEGV_ACCERR:
				return "invalid permissions for mapped object";
		}
	}

	if(SIGBUS==signal)
	{
		switch(code)
		{
			case BUS_ADRALN:
				return "invalid address alignment";
			case BUS_ADRERR:
				return "non-existent physical address";
			case BUS_OBJERR:
				return "object specific hardware error";
		}
	}

	if(SIGTRAP==signal)
	{
		switch(code)
		{
			case TRAP_BRKPT:
				return "process breakpoint";
			case TRAP_TRACE:
				return "process trace trap";
		}
	}

	return "Unhandled signal handler";
}

static inline void		hlib_dbg_fault_handler(int signal, siginfo_t *siginfo, void *context)
{
#define	MAX_FRAME_NUM		128
	struct timespec timestamp;

	clock_gettime(CLOCK_REALTIME, &timestamp);
	HLIB_CMD_Printf("\n******************************************************");
	HLIB_CMD_Printf("\n*                   EXCEPTIONS                       *");
	HLIB_CMD_Printf("\n******************************************************");
	HLIB_CMD_Printf("\nProcess Id (%d)(%s)", HLIB_STD_GetPID(), HLIB_STD_GetCurProcessName());
	HLIB_CMD_Printf("\n Thread Id (%d)", (int)syscall(4222));
	HLIB_CMD_Printf("\n Exception (%s)", strsignal(signal));
	HLIB_CMD_Printf("\n Reason    (%s)", hlib_dbg_getReason(siginfo->si_code, signal));
	HLIB_CMD_Printf("\nFault Addr (%p)", siginfo->si_addr);
	HLIB_CMD_Printf("\nSignal err (%s)", strerror(siginfo->si_errno));
	HLIB_CMD_Printf("\nLast error (%s)", strerror(errno));
	HLIB_CMD_Printf("\nTime Stamp (%s)", ctime(&timestamp.tv_sec));
	HLIB_CMD_Printf("\n******************************************************\n\n");

#if	0	//	동작 안하는 경우 존재 : default로 막자.
{
	HINT32	i;
	char	**symbols;
	HINT32	num_frames;
	void	*frames[MAX_FRAME_NUM];

	num_frames = backtrace(frames, MAX_FRAME_NUM);
	symbols = backtrace_symbols(frames, num_frames);
	if(symbols)
	{
		for(i=0; i< num_frames; i++)
		{
			printf(" --> %s\n", symbols[i]);
		}
	}
	fflush(NULL);

	HLIB_STD_MemFree(symbols);
}
#endif

	//	Call user signal handler.
	if (s_pfnSignalUserHandler)
		s_pfnSignalUserHandler(signal, siginfo, context);

	/* Produce a core dump for in depth debugging */
	abort();
}
#endif
#endif


HxDbg_Instance *hlib_dbg_get_instance(void)
{
    static HxDbg_Instance *s_instance= NULL;

    if(s_instance== NULL)
    {
        s_instance = (HxDbg_Instance*)HLIB_MEM_Calloc(sizeof(HxDbg_Instance));
        if(s_instance)
        {
            s_instance->pKiller = NULL;
            s_instance->pTimer = HLIB_HASH_NewFull(HLIB_HASH_GetIntKey, HLIB_HASH_IsIntEqual, NULL, HLIB_MEM_Free_CB);
            s_instance->pHistory = HLIB_QUEUE_NewEx(MAX_TIMER_NUM,FALSE,HLIB_MEM_Free_CB);
            if(HxSEMT_Create(&s_instance->sema,"hlib_dbg",HxSEMT_FIFO) !=VK_OK)
            {
               HLIB_QUEUE_Delete(s_instance->pHistory);
               HLIB_HASH_Destroy(s_instance->pTimer);
               HLIB_MEM_Free(s_instance);
               s_instance = NULL;
            }
        }
    }
    return s_instance;
}

void hlib_dbg_ExceedTimeLimit(unsigned long timerid,
                                     void   *userdata)
{
    HLIB_DBG_INFO_t    *bomb;
    HxDbg_Instance      *instance = hlib_dbg_get_instance();


    HxSEMT_Get(instance->sema);
    bomb = (HLIB_DBG_INFO_t*)userdata;

    if(instance->pKiller)
        HLIB_MEM_Free(instance->pKiller);
    instance->pKiller = HLIB_MEM_MemDup(bomb,sizeof(HLIB_DBG_INFO_t));
    instance->pKiller->timer = timerid;
    HxSEMT_Release(instance->sema);

	Hx_Print(HxANSI_COLOR_RED("******************************************************\n"));
	Hx_Print(HxANSI_COLOR_RED("                Exceed the Time Limit(%s)             \n"), HLIB_STD_GetCurProcessName());
	Hx_Print(HxANSI_COLOR_RED("******************************************************\n"));
	Hx_Print(HxANSI_COLOR_RED("[%s]msg:%s\n"),bomb->caller,bomb->message);
	Hx_Print(HxANSI_COLOR_RED("You will die, i will call abort()!!!!!!!!!!\n"));
	Hx_Print(HxANSI_COLOR_RED( "******************************************************\n"));

    abort();

/*
* 아래와 같이 디버깅해 보세요..
* 1) Log 를 통해 출력된 caller 혹은 Message 를 가지고 코드를 찾아본다..
* 2) 1번으로 여유치 않으면, core dump 를 gdb 로 열어
*     a. set logging on log.txt   = log.txt 로 gdb 출력을 저장
*     b. thread apply all bt full   = 모든 thread 의 backtrace 를 출력
*     c. log.txt 를 가지고, 1번 의 caller 혹은 Message 를 검색(아니면 thread 1 에 남아있는 정보를 활용 )하여, 원인이 되는 Thread number 를 찾아낸다.
*     d. 다시 gdb 를 활용해 thread  [thread num] 으로 해당 스레드 정보를 조사해보길..
*/
}

void hlib_dbg_hashforeach(void* key , void* value , void* user_data)
{
    HxDBGFunc_t pFunc = (HxDBGFunc_t)user_data;
    HLIB_DBG_INFO_t     *pInfo = (HLIB_DBG_INFO_t*)value;

    pFunc(pInfo,FALSE);
}


/**********************************************************************************************************/
#define ____PUBLIC_FUNCTIONS_______________________
/**********************************************************************************************************/
#ifdef	ENABLE_DEBUG
#ifdef	USE_VK_SIG
void	HLIB_DBG_Init(void)
#else
void	HLIB_DBG_Init(void (*pfnSignalUserHandler)(int signal, siginfo_t *siginfo, void *context))
#endif
{
#ifdef	USE_VK_SIG
#else
	s_pfnSignalUserHandler = pfnSignalUserHandler;
	HLIB_DBG_InitSignalHandler();
#endif
	HLIB_DBG_InitCoreDump();
	HLIB_DBG_InitTrace();

}

#ifdef	USE_VK_SIG
#else
void	HLIB_DBG_InitSignalHandler(void)
{
	HINT32	ret;
	struct sigaction	action;

	action.sa_sigaction = hlib_dbg_fault_handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_SIGINFO;

	ret  = sigaction(SIGSEGV, &action, NULL);
	ret |= sigaction(SIGILL,  &action, NULL);
	ret |= sigaction(SIGFPE,  &action, NULL);
	ret |= sigaction(SIGBUS,  &action, NULL);
	ret |= sigaction(SIGQUIT, &action, NULL);

	HxLOG_Print("Signal (SIGSEGV/SIGILL/SIGFPE/SIGBUS/SIGQUIT) is registered!\n");
}
#endif

void	HLIB_DBG_InitCoreDump(void)
{
	struct rlimit	rlim;

	rlim.rlim_cur = RLIM_INFINITY;
	rlim.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &rlim);
}
#endif


HULONG HLIB_DBG_SetTimeLimitEx(const HCHAR     *caller,
                                    HUINT32     starttime,
                                    HUINT16     settime,
                                const HCHAR     *format,
                                                ...)
{
    HULONG              timer =0;
    HLIB_DBG_INFO_t     bomb;
	va_list	            ap;
    HxDbg_Instance      *instance;

    instance = hlib_dbg_get_instance();
    if(instance == NULL) return 0;
    HxSEMT_Get(instance->sema);

    HLIB_STD_StrNCpySafe(bomb.caller,caller,128 -1);
    bomb.starttime    = starttime;
    bomb.settime      = settime;
    bomb.tid          = HLIB_STD_GetTID();
    HxSTD_MemSet(bomb.message,0x0,1024);
	va_start(ap, format);
    vsprintf(bomb.message, format, ap);
	va_end(ap);

    if(VK_TIMER_EventAfter(settime*1000,hlib_dbg_ExceedTimeLimit,(void*)&bomb,sizeof(HLIB_DBG_INFO_t),&timer) != VK_OK)
    {
        HxLOG_Warning("Set Timer[%s] Failed.....\n",caller);
        HxSEMT_Release(instance->sema);
        return 0;
    }
    bomb.timer = timer;
    HLIB_HASH_Insert(instance->pTimer,(void*)timer,HLIB_MEM_MemDup(&bomb,sizeof(HLIB_DBG_INFO_t)));

    HxSEMT_Release(instance->sema);
    return timer;
}

HERROR HLIB_DBG_CancelTimeLimitEx(HULONG timer)
{
    HxDbg_Instance      *instance;
    HLIB_DBG_INFO_t     *pInfo,*pHistory;

    instance = hlib_dbg_get_instance();
    if(instance == NULL) return ERR_FAIL;

    HxSEMT_Get(instance->sema);
    if(VK_TIMER_Cancel(timer) != VK_OK)
    {
        HxLOG_Warning("Cancel Timer[%d] Failed.....\n",timer);
        HxSEMT_Release(instance->sema);
        return ERR_FAIL;
    }

    pInfo =HLIB_HASH_Lookup(instance->pTimer,(void*)timer);

    if(pInfo)
    {
        pHistory = (HLIB_DBG_INFO_t*)HLIB_MEM_MemDup(pInfo,sizeof(HLIB_DBG_INFO_t));
        if(HLIB_QUEUE_Push(instance->pHistory,pHistory)==FALSE)
        {
            pInfo = (HLIB_DBG_INFO_t *)HLIB_QUEUE_Pop(instance->pHistory);
            if(pInfo)
                HLIB_MEM_Free(pInfo);
            HLIB_QUEUE_Push(instance->pHistory,pHistory);
        }
    }

    HLIB_HASH_Remove(instance->pTimer,(void*)timer);
    HxSEMT_Release(instance->sema);

    return ERR_OK;
}


HERROR HLIB_DBG_GetTimerList(HxDBGFunc_t pFunc)
{
    HxDbg_Instance      *instance;
    HLIB_DBG_INFO_t     *pInfo;

    instance = hlib_dbg_get_instance();
    if(instance == NULL) return ERR_FAIL;

    HxSEMT_Get(instance->sema);

   	while ((pInfo = (HLIB_DBG_INFO_t *)HLIB_QUEUE_Pop(instance->pHistory)) != NULL)
   	{
        pFunc(pInfo,TRUE);
        HLIB_MEM_Free(pInfo);
   	}

    HLIB_HASH_Foreach(instance->pTimer,hlib_dbg_hashforeach,(void*)pFunc);
    HxSEMT_Release(instance->sema);

    return ERR_OK;
}

HLIB_DBG_INFO_t*    HLIB_DBG_GetExceedTimer(void)
{
    HxDbg_Instance      *instance;

    instance = hlib_dbg_get_instance();
    if(instance == NULL) return NULL;

    return instance->pKiller;;
}


