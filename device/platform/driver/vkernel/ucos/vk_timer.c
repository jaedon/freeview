/**
 * vk_timer.c
*/

/**
 * @defgroup		VK_TIMER
 * @author		Tae-Hyuk Kang
 * @note			Virsual Timer APIs
 * @file 			vk_timer.c
 * @remarks		Copyright (c) 2008 HUMAX Co., Ltd. All rights reserved.
*/


#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include <sys/queue.h>
#endif

#include <linden_trace.h>

#include "os.h"
#include "os_cfg_app.h"

#include "htype.h"
#include "vkernel.h"
#include "taskdef.h"


/************** Debuging option *******************/
//#define DEBUG                            //debug print를 위한 옵션
//#define FUNCTION_CALL_DEBUG  //function Enter Exit를 위한 option
/************** Debuging option *******************/
//#define TIMER_CALLBACK_WARNING

#define DEBUG_MODULE			TRACE_MODULE_VK_TIMER

#define MAX_TIMER_HALF	0x7FFFFFFF
typedef enum
{
	TIMER_TYPE_ONCE			= 0,
	TIMER_TYPE_EVERY
} TIMER_TYPE ;

typedef struct _timer_list_t
{
	unsigned long			WhenTimeOut ;
	unsigned char			TimerType ;
	unsigned char			TimeoutOverflow ;
	unsigned char			pad[2];
	unsigned long			TimerID ;
	unsigned long			Duration ;	//clock tick
	void 					(*CallBackFunc)(unsigned long, void *) ;
	void					*LocalArg ;
	struct _timer_list_t	*next ;
} TimerList_t ;

enum TIMER_TYPE
{
	TIMER_TYPE_ONESHOT,
	TIMER_TYPE_PERIODIC
};

static TimerList_t			TimerListHeader ;
static unsigned long		timer_sema_id ;
#if defined(CONFIG_DEBUG)
#define TEMP_TIMER_CALLBACK_TASK
#endif

#if defined(TEMP_TIMER_CALLBACK_TASK)
static unsigned long		timer_callback_id ;
#endif

static void	timer_task(void* params) ;
static int AddTimerItem(unsigned char	TimerType, unsigned long Duration, void	(*CallBackFunc)(unsigned long, void *), void *LocalArg, unsigned long LocalArgSize, TimerList_t	*TimerList);
static int DeleteTimerItem(unsigned long);
VK_TIMER_STATE time_after(unsigned long when, unsigned long now);

static unsigned long	sys_tick_init_val = 0;
static unsigned long	s_TimerTaskId ;

#if defined(TEMP_TIMER_CALLBACK_TASK)
static void P_TIMER_Intr(void *pEvent)
{
	TimerList_t		localEvent;
	unsigned long	startTick, endTick;

	PrintEnter();

	if (pEvent == NULL)
	{
		PrintError("[%s] Invalid Parameter. \n", __func__ );
		goto end;
	}

	localEvent = *((TimerList_t*)pEvent);
	if (localEvent.CallBackFunc!=NULL)
	{
		PrintDebug("TIMER CALLBACK %x Fired!\n", ((TimerList_t*)pEvent));
#if defined(CONFIG_DEBUG)
		startTick = VK_TIMER_GetSystemTick();
#endif
		localEvent.CallBackFunc(localEvent.TimerID, localEvent.LocalArg);
#if defined(CONFIG_DEBUG)
		endTick = VK_TIMER_GetSystemTick();
		if (VK_TIMER_TimeAfter(startTick + 10, endTick) == VK_TIMER_EXPIRED)
		{
			PrintDebug("[P_TIMER_Intr] too long execution time(%ld) func = %08X\n", (endTick - startTick), (int)localEvent.CallBackFunc);
		}
#endif
	}
	else
	{
		PrintError("[%s] TIMER CALLBACK %d not registered!\n", __func__);
	}

	if(localEvent.TimerType == TIMER_TYPE_ONCE)
	{
		if(localEvent.LocalArg != NULL)
		{
			DD_MEM_Free(localEvent.LocalArg);
		}
	}

end :
	PrintExit();

}
#endif

int VK_TIMER_Init(void)
{
//	unsigned long	TimerID ;
	int		Error ;

#if defined(TEMP_TIMER_CALLBACK_TASK)
	Error=VK_INTR_Register((void (*)(void *))P_TIMER_Intr, "timer_callback", sizeof(TimerList_t), &timer_callback_id, VK_INTR_TASK0);
	if (Error!=VK_OK)
	{
		PrintError("[%s] SEM create failed.\n\r", __func__);
		return VK_CREATE_ERROR ;
	}
#endif

	Error = VK_SEM_Create(&timer_sema_id, "TIMER_SEM", DEFAULT_SUSPEND_TYPE) ;
	if(Error != VK_OK)
	{
		PrintError("Error::VK_TIMER_Init - VK_SEM_Create err = %d\n", Error);
		return VK_CREATE_ERROR ;
	}

	TimerListHeader.next = NULL ;

	if (sys_tick_init_val == 0)
	{
		sys_tick_init_val = VK_TIMER_GetSystemTick();
	}
	Error = VK_TASK_Create(
			timer_task,
			TIMER_TASK_PRIORITY,
			TIMER_TASK_STACK_SIZE,
			"TimerT",
			(void *)0,
			&s_TimerTaskId, FALSE);

	if(Error != VK_OK)
	{
		PrintError("Error::VK_TIMER_Init - task create error = %d\n", Error);
		return VK_CREATE_ERROR ;
	}

	VK_TASK_Start(s_TimerTaskId);

	return VK_OK ;
} /* VK_TIMER_Init */

int VK_TIMER_Check(unsigned long id)
{
    id = id;
    /* TODO */
    return 0;
}

int	VK_TIMER_Cancel(unsigned long TimerID)
{
	int		Error ;
#if defined(TEMP_TIMER_CALLBACK_TASK)
#else
	unsigned long currTaskId;
#endif

#if defined(TEMP_TIMER_CALLBACK_TASK)
#else
	VK_TASK_GetCurrentId(&currTaskId);
	if (s_TimerTaskId == currTaskId)
	{
		VK_SEM_Release(timer_sema_id) ;
		PrintError("[VK_TIMER_Cancel] error : do not use VK_TIMER_Cancel() in timer callback function!! TaskId = %lu\n", s_TimerTaskId);
	}
#endif

	VK_SEM_Get(timer_sema_id) ;
	Error = DeleteTimerItem(TimerID) ;
	VK_SEM_Release(timer_sema_id) ;
	if(Error)
	{
		PrintError("Error: VK_TIMER_Cancel - invalid timer id (%08X)\n", TimerID);
		return Error ;
	}
#if defined(TEMP_TIMER_CALLBACK_TASK)
#else
#if 0
	if (s_TimerTaskId == currTaskId)
	{
		VK_SEM_Get(timer_sema_id) ;
	}
#endif
#endif
	return VK_OK ;
} /* VK_TIMER_Cancel */

/********************************************************************
 function: 	VK_TIMER_EventAfter
 description :	timeout으로 지정된 시간 후에 CallBackFunc이 불려진다.
 argument: timeout = msec
 return:
*********************************************************************/
int	VK_TIMER_EventAfter(
		unsigned long	TimeOut,
		void		(*CallBackFunc)(unsigned long, void *),
		void		*LocalArg,
		int	LocalArgSize,
		unsigned long	*TimerID)
{
	TimerList_t		*TimerList ;
	int			Error ;

	PrintEnter();

	VK_SEM_Get(timer_sema_id) ;

	TimerList = (TimerList_t *)DD_MEM_Alloc(sizeof(TimerList_t)) ;
	if(TimerList == NULL)
	{
		PrintError("Error::VK_TimerEventAfter - DD_MEM_Alloc TimerList = %p\n", TimerList);
		VK_SEM_Release(timer_sema_id) ;
		return VK_ALLOC_ERROR ;
	}

	Error = AddTimerItem(TIMER_TYPE_ONCE, TimeOut, CallBackFunc,
							LocalArg, LocalArgSize, TimerList) ;
	if(Error)
	{
		DD_MEM_Free(TimerList) ;
		PrintError("Error::VK_TimerEventAfter - AddTimerItem:error=0x%x\n",Error);
		VK_SEM_Release(timer_sema_id) ;
		return VK_ERROR ;
	}

	*TimerID = TimerList->TimerID ;
	VK_SEM_Release(timer_sema_id) ;

	PrintExit();

	return VK_OK ;
} /* VK_TIMER_EventAfter */

/********************************************************************
 function: 	VK_TIMER_EventEvery
 description :	start timer in REPEAT_MODE
 argument: timeout = msec
 return:
*********************************************************************/
int VK_TIMER_EventEvery(unsigned long	 TimeOut, void (*CallBackFunc)(unsigned long, void *), void *LocalArg, int LocalArgSize, unsigned long *TimerID)
{
	TimerList_t		*TimerList ;
	int			Error ;

	PrintEnter();

	VK_SEM_Get(timer_sema_id) ;

	TimerList = (TimerList_t *)DD_MEM_Alloc(sizeof(TimerList_t)) ;
	if(TimerList == NULL)
	{
		PrintError("Error::VK_TIMER_EventEvery - DD_MEM_Alloc:error TimerList = %p\n", TimerList);
		VK_SEM_Release(timer_sema_id) ;
		return VK_ALLOC_ERROR ;
	}

	Error = AddTimerItem(TIMER_TYPE_EVERY, TimeOut, CallBackFunc, LocalArg, LocalArgSize, TimerList);
	if (Error != VK_OK)
	{
		PrintError("Error::VK_TIMER_EventEvery - AddTimerItem:error=0x%x\n",Error);
		DD_MEM_Free(TimerList) ;
		VK_SEM_Release(timer_sema_id) ;
		return Error ;
	}

	*TimerID = TimerList->TimerID ;
	VK_SEM_Release(timer_sema_id) ;

	PrintExit();

	return VK_OK ;
} /* VK_TIMER_EventEvery */

unsigned long VK_TIMER_GetTickFreq(void)
{
	return 1000;
}

unsigned long VK_TIMER_GetSystemTick(void)
{
	OS_TICK tick = 0;
	OS_ERR	err;

	tick = OSTimeGet(&err);
	if(OS_ERR_NONE != err)
	{
		return 0;
	}
	tick *= 10;
	return tick & 0xFFFFFFFF;
}

VK_TIMER_STATE VK_TIMER_TimeAfter(unsigned long whenTick, unsigned long nowTick)
{
	return time_after(whenTick, nowTick);
}

unsigned long time_now(void)
{
	return VK_TIMER_GetSystemTick();
}

unsigned long time_plus(unsigned long tick1, unsigned long tick2)
{
	unsigned long tick;
	tick = (unsigned long)(tick1 + tick2);
	return tick;
}

VK_TIMER_STATE time_after(unsigned long when, unsigned long now)
{
	if ((when - now) & 0x80000000)
	{
		return VK_TIMER_EXPIRED;
	}
	else
	{
		return VK_TIMER_RUNNING;
	}
}

/********************************************************************
 function: 	timer_task
 description :
 argument:
 return:
*********************************************************************/
void timer_task(void *params)
{
	TimerList_t		*TimerListPtr ;
	TimerList_t		*TimerListBeforePtr ;
	TimerList_t		*BackupTimerListPtr;
	VK_TIMER_STATE	timerState;
	unsigned long	ulTimeout;
#if defined(TEMP_TIMER_CALLBACK_TASK)
#else
	void 			(*func)(unsigned long, void *);
#if defined(CONFIG_DEBUG)
	unsigned long	startTick, endTick;
#endif
#endif
	HAPPY(params);

    PrintEnter();

	while(1)
	{
		VK_SEM_Get(timer_sema_id) ;

		TimerListBeforePtr	= &TimerListHeader ;
		TimerListPtr		= TimerListHeader.next ;

		while(TimerListPtr)
		{
			timerState = time_after(TimerListPtr->WhenTimeOut, time_now());
			if (timerState == VK_TIMER_RUNNING)
			{
				TimerListPtr		= TimerListPtr->next ;
				TimerListBeforePtr	= TimerListBeforePtr->next ;
			}
			else if (TimerListPtr->TimeoutOverflow == 1)
			{
				TimerListPtr->WhenTimeOut		= time_plus(time_now(), MAX_TIMER_HALF) ;
				TimerListPtr->TimeoutOverflow	= 0;

				TimerListPtr		= TimerListPtr->next ;
				TimerListBeforePtr	= TimerListBeforePtr->next ;
			}
			else
			{
				if(TimerListPtr->TimerType == TIMER_TYPE_ONCE)
				{
					BackupTimerListPtr = TimerListPtr;
					TimerListBeforePtr->next = TimerListPtr->next ;
					TimerListPtr = TimerListBeforePtr->next ;

#if defined(TEMP_TIMER_CALLBACK_TASK)
					VK_INTR_Event(timer_callback_id, BackupTimerListPtr);
#else
#if defined(CONFIG_DEBUG)
					startTick = VK_TIMER_GetSystemTick();
#endif
					func = (void (*)(unsigned long, void *))(BackupTimerListPtr->CallBackFunc);
#if defined(TIMER_CALLBACK_WARNING)
					VK_TASK_EnterCallback((unsigned long)func);
#endif
					if (func)
                    {
						func(BackupTimerListPtr->TimerID, BackupTimerListPtr->LocalArg);
					}
#if defined(TIMER_CALLBACK_WARNING)
					VK_TASK_LeaveCallback();
#endif
#if defined(CONFIG_DEBUG)
					endTick = VK_TIMER_GetSystemTick();
#endif
					if(BackupTimerListPtr->LocalArg != NULL)
						DD_MEM_Free(BackupTimerListPtr->LocalArg) ;
#endif
					DD_MEM_Free(BackupTimerListPtr) ;
				}
				else
				{
#if defined(TEMP_TIMER_CALLBACK_TASK)
					VK_INTR_Event(timer_callback_id, TimerListPtr);
#else
#if defined(CONFIG_DEBUG)
					startTick = VK_TIMER_GetSystemTick();
#endif
					func = (void (*)(unsigned long, void *))(TimerListPtr->CallBackFunc);
#if defined(TIMER_CALLBACK_WARNING)
					VK_TASK_EnterCallback((unsigned long)func);
#endif
					if (func)
					{
						func(TimerListPtr->TimerID, TimerListPtr->LocalArg);
					}
#if defined(TIMER_CALLBACK_WARNING)
					VK_TASK_LeaveCallback();
#endif
                    PrintDebug("---- TimerListPtr->TimerID = %x\n", TimerListPtr->TimerID);
#if defined(CONFIG_DEBUG)
					endTick = VK_TIMER_GetSystemTick();
#endif
#endif
					if (TimerListPtr->Duration & 0x80000000)
					{
						TimerListPtr->TimeoutOverflow	= 1;
						ulTimeout = TimerListPtr->Duration - MAX_TIMER_HALF;
					}
					else
					{
						TimerListPtr->TimeoutOverflow	= 0;
						ulTimeout = TimerListPtr->Duration;
					}
					TimerListPtr->WhenTimeOut = time_plus(TimerListPtr->WhenTimeOut, ulTimeout) ;
					TimerListPtr		= TimerListPtr->next ;
					TimerListBeforePtr	= TimerListBeforePtr->next ;
				}
#if defined(TEMP_TIMER_CALLBACK_TASK)
#else
#if defined(CONFIG_DEBUG)
				if (VK_TIMER_TimeAfter(startTick + 10, endTick) == VK_TIMER_EXPIRED)
				{
					PrintDebug("[timer_task] too long execution time(%ld) func = %08X\n", (endTick - startTick), (int)func);
				}
#endif
#endif
			}
		}
		VK_SEM_Release(timer_sema_id) ;
#if defined(CONFIG_WORKAROUND_TIMER_RESOLUTION)
		VK_TASK_Sleep(5);
#else
		VK_TASK_Sleep(10);
#endif
	}

    PrintExit();
} /* timer_task */

static TimerList_t *GetTimerList(unsigned long timerId)
{
	TimerList_t				*TimerListPtr ;

	TimerListPtr = &TimerListHeader ;
	while(TimerListPtr)
	{
		if (TimerListPtr->TimerID == timerId)
		{
			return TimerListPtr;
		}
		TimerListPtr = TimerListPtr->next ;
	}
	return (TimerList_t*)NULL;
}

static unsigned long GetNextTimerId(void)
{
	static unsigned long	TimerIDCounter = 1 ;

	do
	{
		TimerIDCounter++;
		if (TimerIDCounter == 0xFFFFFFFF)
		{
			TimerIDCounter = 1;
		}
	} while (GetTimerList(TimerIDCounter) != NULL);
	return TimerIDCounter;
}

/********************************************************************
 function: 	AddTimerItem
 description :
 argument:
 return:
*********************************************************************/
static int AddTimerItem(unsigned char TimerType, unsigned long Duration, void (*CallBackFunc)(unsigned long, void *), void *LocalArg, unsigned long LocalArgSize, TimerList_t *TimerList)
{
	TimerList_t				*TimerListPtr ;
	TimerList_t				*TimerListPtrLast ;
	unsigned long			timerId;
	unsigned long			ulTimeout;

	PrintEnter();

	timerId = GetNextTimerId();
	TimerListPtr = &TimerListHeader ;
	while(TimerListPtr->next)
		TimerListPtr = TimerListPtr->next ;
	TimerListPtrLast = TimerListPtr;
	TimerListPtr->next = TimerList ;
	TimerListPtr = TimerListPtr->next ;
	if(TimerListPtr == NULL)
	{
        PrintError("%s : TimerListPtr == NULL Error!!, FILE(%s), LINE(%d)\n", __FUNCTION__, __FILE__, __LINE__);
		return VK_ERROR ;
	}

	if((LocalArg != NULL) && (LocalArgSize != 0))
	{
		TimerListPtr->LocalArg = (void *)DD_MEM_Alloc(LocalArgSize) ;
		if(TimerListPtr->LocalArg == NULL)
		{
			TimerListPtrLast->next = NULL;
            PrintError("%s : DD_MEM_Alloc() Error!!, FILE(%s), LINE(%d)\n", __FUNCTION__, __FILE__, __LINE__);
			return VK_ALLOC_ERROR ;
		}
	}
	else
	{
		TimerListPtr->LocalArg = NULL ;
	}
	if (Duration & 0x80000000)
	{
		TimerListPtr->TimeoutOverflow	= 1;
		ulTimeout = Duration - MAX_TIMER_HALF;
	}
	else
	{
		TimerListPtr->TimeoutOverflow	= 0;
		ulTimeout = Duration;
	}
	TimerListPtr->WhenTimeOut		= time_plus(time_now(), ulTimeout) ;
	TimerListPtr->TimerType		= TimerType ;
//	TimerListPtr->TimerID 		= (unsigned long)TimerList;
	TimerListPtr->TimerID 		= timerId;
	TimerListPtr->Duration		= Duration ;
	TimerListPtr->CallBackFunc	= CallBackFunc ;
	if(TimerListPtr->LocalArg != NULL)
		VK_memcpy(TimerListPtr->LocalArg, LocalArg, LocalArgSize) ;
	TimerListPtr->next			= NULL ;

	PrintExit();
	return VK_OK ;
} /* AddTimerItem */


/********************************************************************
 function: 	DeleteTimerItem
 description :
 argument:
 return:
*********************************************************************/
static int DeleteTimerItem(unsigned long TimerID)
{
	TimerList_t		*TimerListPtr ;
	TimerList_t		*TimerListDelPtr ;

	PrintEnter();

	TimerListPtr	= &TimerListHeader ;
	TimerListDelPtr = TimerListHeader.next ;
	while(TimerListDelPtr)
	{
		if(TimerListDelPtr->TimerID == TimerID)
		{
			TimerListPtr->next = TimerListDelPtr->next ;
			if(TimerListDelPtr->LocalArg != NULL)
				DD_MEM_Free(TimerListDelPtr->LocalArg) ;
			DD_MEM_Free(TimerListDelPtr) ;
			return VK_OK ;
		}

		TimerListPtr	= TimerListPtr->next ;
		TimerListDelPtr	= TimerListDelPtr->next ;
	}
	/* fail to find out TimerItem which is matched with TimerID */

	PrintExit();
	return VK_INVALID_ID ;
} /* DeleteTimerItem */

/*
	타이머를 출력한다.
*/
#if defined(VK_TIMER_DEBUG)
void VK_TIMER_PrintTimerList(void)
{
	TimerList_t	*TimerListPtr;
	TimerList_t	*TimerListDisplayPtr;
	unsigned long ulTimerCount = 0;

	VK_SEM_Get(timer_sema_id);
	TimerListPtr		= &TimerListHeader;
	TimerListDisplayPtr = TimerListPtr->next;

	PrintDebug("**************************************************************************************************\n");
	PrintDebug("Count       Cur_Time          TimerID         WhenTimeOut           Duration          CallBackFunc          LocalArg\n");
	while(TimerListDisplayPtr)
	{
		ulTimerCount++;
		PrintDebug("%5d     0x%08x          0x%05x          0x%08x           %8d            0x%08x          0x%08x\n",
		                        ulTimerCount, time_now(), TimerListDisplayPtr->TimerID, TimerListDisplayPtr->WhenTimeOut,
		                        TimerListDisplayPtr->Duration, (int)TimerListDisplayPtr->CallBackFunc, TimerListDisplayPtr->LocalArg);
		TimerListDisplayPtr = TimerListDisplayPtr->next;
	}
	PrintDebug("**************************************************************************************************\n");
	VK_SEM_Release(timer_sema_id);
}
#endif
