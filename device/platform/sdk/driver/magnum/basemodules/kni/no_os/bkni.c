/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni.c $
 * $brcm_Revision: Hydra_Software_Devel/36 $
 * $brcm_Date: 11/30/11 8:35p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/kni/no_os/bkni.c $
 * 
 * Hydra_Software_Devel/36   11/30/11 8:35p agin
 * SWNOOS-500:  Merge BUILD_SATFE.
 * 
 * Hydra_Software_Devel/35   9/10/11 6:41p agin
 * SWNOOS-485:  Relevel 97346 no-os to latest for B0.
 * 
 * Hydra_Software_Devel/34   5/12/11 6:03p agin
 * SWNOOS-458:  Fix compiler warnings.
 * 
 * Hydra_Software_Devel/33   1/27/11 5:46p agin
 * SWNOOS-439:  Use measured clock rate instead of define.
 * 
 * Hydra_Software_Devel/32   10/27/10 6:42p agin
 * SWNOOS-429:  Use NO_OS_DIAGS define.
 * 
 * Hydra_Software_Devel/31   10/27/10 4:44p agin
 * SWNOOS-429:  Remove unnecessary include.
 * 
 * Hydra_Software_Devel/30   10/24/10 8:35p agin
 * SWNOOS-429:  Eliminate unnecessary define.
 * 
 * Hydra_Software_Devel/28   3/10/10 3:04p mward
 * SW7400-2712:  Add annotation to indicate to Coverity that BKNI_Fail()
 * is a "kill path".
 * 
 * Hydra_Software_Devel/27   3/8/10 4:30p agin
 * SWNOOS-395:  Support 7401.
 * 
 * Hydra_Software_Devel/26   1/6/10 7:10p agin
 * SWNOOS-389:  Relevel 97335 no-os to Phase 6.5.
 * 
 * Hydra_Software_Devel/25   12/17/09 7:06p agin
 * SWNOOS-387:  Support 97468 no-os.
 * 
 * Hydra_Software_Devel/24   12/14/09 7:18p agin
 * SWNOOS-385:  Support 7125.
 * 
 * Hydra_Software_Devel/23   11/25/09 7:49a agin
 * SWNOOS-384:  Cleaned up #define.
 * 
 * Hydra_Software_Devel/22   11/24/09 4:12p agin
 * SWNOOS-384:  Support 7325.
 * 
 * Hydra_Software_Devel/21   10/11/09 4:59p agin
 * SWNOOS-366: Support 97420B0.
 * 
 * Hydra_Software_Devel/20   9/13/09 5:20p agin
 * SWNOOS-368: Add 7340 for include of bmips4380.h.
 * 
 * Hydra_Software_Devel/19   9/6/09 10:25p agin
 * SWNOOS-369: Include bmips4380.h, fixed warnings.
 * 
 * Hydra_Software_Devel/18   5/29/09 11:31a agin
 * PR55555: Fixed function name.
 * 
 * Hydra_Software_Devel/17   5/29/09 11:18a agin
 * PR55555: Add BKNI_WaitForEventDispatch.
 * 
 * Hydra_Software_Devel/16   5/21/09 7:02p agin
 * PR55375: Fixed BKNI_Delay so that it is always accurate.
 * 
 * Hydra_Software_Devel/15   5/11/09 8:18p agin
 * PR54339: Add support for BKNI_Vsnprintf.
 * 
 * Hydra_Software_Devel/14   4/19/09 6:55p agin
 * PR54339: Don't support timeout for BKNI_WaitForGroup.
 * 
 * Hydra_Software_Devel/13   2/24/09 12:58p agin
 * PR51987: Add support for group events.
 * 
 * Hydra_Software_Devel/12   1/28/09 11:50a farshidf
 * PR41450: add BKNI_AssertIsrContext
 * 
 * Hydra_Software_Devel/11   10/11/08 9:38a agin
 * PR47788: Use playpump to playback an embedded mpeg stream within no-os.
 * 
 * Hydra_Software_Devel/10   6/12/08 6:41p agin
 * PR43579: Check in changes for 20080530 BCM97325 non-os release.
 * 
 * Hydra_Software_Devel/9   11/11/03 3:33p enavarro
 * implemented BKNI_EnterCriticalSection() and BKNI_LeaveCriticalSection()
 * 
 * Hydra_Software_Devel/8   10/31/03 2:20p brianlee
 * Fixed WaitForEvent for INFINITE wait.  It was not waiting infinitely.
 * 
 * Hydra_Software_Devel/7   10/23/03 7:07p btan
 * Fixed issue with BKNI_INFINITE
 * 
 * Hydra_Software_Devel/6   10/18/03 1:21p dlwin
 * Added timeout feature to BKNI_WaitForEvent().
 * 
 * Hydra_Software_Devel/5   10/17/03 10:40a brianlee
 * Added BKNI_ResetEvent function.
 * 
 * Hydra_Software_Devel/4   10/9/03 5:10p brianlee
 * Make eventSet flag volatile.
 * 
 * Hydra_Software_Devel/3   10/7/03 6:24p brianlee
 * Fixed structure definition.
 * 
 * Hydra_Software_Devel/2   10/3/03 5:36p brianlee
 * Implemented BKNI_Delay() for no-os.
 * 
 * Hydra_Software_Devel/1   10/3/03 2:26p brianlee
 * Initial version.
 *
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "bkni_event_group.h"
#include "mipsclock.h"
#include "cpuctrl.h"
#include "blst_list.h"

BDBG_MODULE(kernelinterface);

#include <stdio.h>

extern uint32_t get_cpu_clock_rate_hz(void);

/* needed to support tagged interface */
#undef BKNI_Delay
#undef BKNI_Sleep
#undef BKNI_CreateEvent
#undef BKNI_DestroyEvent
#undef BKNI_WaitForEvent
#undef BKNI_SetEvent
#undef BKNI_EnterCriticalSection
#undef BKNI_LeaveCriticalSection
#undef BKNI_CreateMutex
#undef BKNI_DestroyMutex
#undef BKNI_TryAcquireMutex
#undef BKNI_AcquireMutex
#undef BKNI_ReleaseMutex
#undef BKNI_CreateTask
#undef BKNI_DestroyTask

#if defined(NO_OS_DIAGS) && !defined(BUILD_SMALL)
void NEXUS_Base_NO_OS_Scheduler_Dispatch(void);
#endif

typedef struct BKNI_GroupObj 
{
	BLST_D_HEAD(group, BKNI_EventObj) members;
	BKNI_MutexHandle lock;			 /* mutex for protecting signal and conditional variables */
	BKNI_MutexHandle cond;		    /* condition to wake up from event*/
} BKNI_GroupObj;

typedef struct BKNI_EventObj 
{
	BLST_D_ENTRY(BKNI_EventObj) list;
	struct BKNI_GroupObj *group;
	BKNI_MutexHandle lock;			 /* mutex for protecting signal and conditional variables */
	BKNI_MutexHandle cond;		    /* condition to wake up from event*/
/*	bool signal; */
	volatile bool eventSet;
} BKNI_EventObj;

typedef struct BKNI_MutexObj
{
    int dummy;
} BKNI_MutexObj;

typedef struct BKNI_TaskObj
{
    int dummy;
} BKNI_TaskObj;


BERR_Code 
BKNI_Init(void)
{
    return  BERR_SUCCESS;
}

void 
BKNI_Uninit(void)
{
    return;
}


BERR_Code 
BKNI_CreateMutex(BKNI_MutexHandle *pMutex)
{
    *pMutex = (BKNI_MutexHandle)&pMutex;
    return BERR_SUCCESS;
}

void
BKNI_DestroyMutex(BKNI_MutexHandle mutex)
{
	BSTD_UNUSED(mutex);
    return ;
}

BERR_Code 
BKNI_TryAcquireMutex(BKNI_MutexHandle mutex)
{
	BSTD_UNUSED(mutex);
    return BERR_SUCCESS;
}

BERR_Code 
BKNI_AcquireMutex(BKNI_MutexHandle mutex)
{
	BSTD_UNUSED(mutex);
    return BERR_SUCCESS;
}

void
BKNI_ReleaseMutex(BKNI_MutexHandle mutex)
{
	BSTD_UNUSED(mutex);
    return ;
}

/* coverity[+kill]  */
void 
BKNI_Fail(void)
{
    volatile int i;


    i = *(int *)0; /* try to fail, take 1 */
    i = 0;
    i = 1/i;  /* try to fail, take 2 */

    for(;;) { } /* hang here */
}


int 
BKNI_Printf(const char *fmt, ...)
{
    va_list arglist;
    int rc;

    va_start( arglist, fmt );
    rc = vprintf(fmt, arglist);
    va_end(arglist);

    return rc;
}


int 
BKNI_Snprintf(char *str, size_t len, const char *fmt, ...)
{
    va_list arglist;
    int rc;

    va_start( arglist, fmt );
    rc = vsprintf(str, fmt, arglist);
    va_end(arglist);
    if ((size_t)rc > len) {
        BDBG_ERR(("Buffer overflow in the BKNI_Snprintf"));
    }
    return rc;
}

int 
BKNI_Vprintf(const char *fmt, va_list ap)
{
    return vprintf(fmt, ap);
}

int BKNI_Vsnprintf(char *s, size_t n, const char *fmt, va_list ap)
{
     return vsnprintf(s, n, fmt, ap);
}

void BKNI_Delay(unsigned int microsec)
{
    uint32_t ticks,clk;

	/* 
	 * CPU_CLOCK_RATE is in units of Hz 
	 * 1 microsecond = CPU_CLOCK_RATE/1000000
	 */
	ticks = (get_cpu_clock_rate_hz()/(INPUT_CLK_CYCLES_PER_COUNT_REG_INCR * 1000000)) * microsec;
		
	clk = CpuCountGet();
	/*
	 * if the counter has this many ticks left 
	 * before wrapping around, then wait for count
	 */
	if ( (0xffffffff - clk) > ticks)
	{
		ticks = clk + ticks;
		while (clk  < ticks)
			clk = CpuCountGet();
	}
	/* 
	 * else it'll wrap around so compute tick after wrap
	 * wait for the wrap then wait for the rest
	 */
	else
	{
		ticks = ticks - (0xffffffff - clk);
		while (clk  > ticks)
			clk = CpuCountGet();
		while (clk  < ticks)
			clk = CpuCountGet();
	}
}

BERR_Code 
BKNI_Sleep(unsigned int millisec)
{
    BKNI_Delay(millisec*1000);
    return BERR_SUCCESS;
}


BERR_Code 
BKNI_CreateEvent(BKNI_EventHandle *pEvent)
{
    BKNI_EventHandle event;
	BERR_Code result=BERR_SUCCESS;
    
    event = BKNI_Malloc(sizeof(*event));
    *pEvent = event;
	if ( !event) {
		result = BERR_TRACE(BERR_OS_ERROR);
		goto err_no_memory;
	}
    event->eventSet = false;
	event->group = NULL;
	return result;

err_no_memory:
	return result;
}



void
BKNI_DestroyEvent(BKNI_EventHandle hEvent)
{
	BKNI_EventObj *pEventObj;

	pEventObj = (BKNI_EventObj *) hEvent;
	BKNI_Free((void *) pEventObj);
	return;
}

BERR_Code 
BKNI_WaitForEvent(BKNI_EventHandle hEvent, int timeoutMsec)
{
	BERR_Code retVal;
	BKNI_EventObj *pEventObj;
	int cnt;

	retVal = BERR_SUCCESS;
	pEventObj = (BKNI_EventObj *) hEvent;

	cnt = 0;
	while ( pEventObj->eventSet == false )
	{
		if (timeoutMsec != BKNI_INFINITE)
		{
			if( timeoutMsec > cnt )
			{
				cnt++;
				BKNI_Sleep( 1 );
			}
			else
			{
				retVal = BERR_TIMEOUT;
				break;
			}
		}
	}
	if( retVal == BERR_SUCCESS )
	{
		pEventObj->eventSet = false;
	}

	return(retVal);
}

#if defined(NO_OS_DIAGS) && !defined(BUILD_SATFE) && !defined(BUILD_SMALL)
BERR_Code 
BKNI_WaitForEventDispatch(BKNI_EventHandle hEvent, int timeoutMsec)
{
	BERR_Code retVal;
	BKNI_EventObj *pEventObj;
	int cnt;

	retVal = BERR_SUCCESS;
	pEventObj = (BKNI_EventObj *) hEvent;

	cnt = 0;
	while ( pEventObj->eventSet == false )
	{
		if (timeoutMsec != BKNI_INFINITE)
		{
			if( timeoutMsec > cnt )
			{
				cnt++;
				BKNI_Sleep( 1 );
			}
			else
			{
				retVal = BERR_TIMEOUT;
				break;
			}
		}
		NEXUS_Base_NO_OS_Scheduler_Dispatch();
	}
	if( retVal == BERR_SUCCESS )
	{
		pEventObj->eventSet = false;
	}

	return(retVal);
}
#endif

void
BKNI_SetEvent(BKNI_EventHandle hEvent)
{
	BKNI_EventObj *pEventObj;

	pEventObj = (BKNI_EventObj *) hEvent;
	pEventObj->eventSet = true;

	return;
}

void
BKNI_ResetEvent(BKNI_EventHandle hEvent)
{
	BKNI_EventObj *pEventObj;

	pEventObj = (BKNI_EventObj *) hEvent;
	pEventObj->eventSet = false;	
}

void 
BKNI_EnterCriticalSection(void)
{
	uint32_t cp0_status = CpuStatusGet();
	cp0_status &= ~0x1;
	CpuStatusSet(cp0_status);
    return ;
}

void
BKNI_LeaveCriticalSection(void)
{
	uint32_t cp0_status = CpuStatusGet();
	cp0_status |= 0x1;
	CpuStatusSet(cp0_status);
    return ;
}

BERR_Code 
BKNI_CreateEventGroup(BKNI_EventGroupHandle *pGroup)
{
	BKNI_EventGroupHandle group;
	BERR_Code result;

	group = BKNI_Malloc(sizeof(*group));
	if (!group) {
		result = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		goto err_no_memory;
	}
	BLST_D_INIT(&group->members);
	*pGroup = group;

	return BERR_SUCCESS;

err_no_memory:
	return result;
}

void
BKNI_DestroyEventGroup(BKNI_EventGroupHandle group)
{
	BKNI_EventHandle event;

	while(NULL != (event=BLST_D_FIRST(&group->members)) ) {
		BDBG_ASSERT(event->group == group);
		event->group = NULL;
		BLST_D_REMOVE_HEAD(&group->members, list);
	}
    
	BKNI_Free(group);
	return;
}

BERR_Code
BKNI_AddEventGroup(BKNI_EventGroupHandle group, BKNI_EventHandle event)
{
	BERR_Code result = BERR_SUCCESS;

	if (event->group != NULL) {
		BDBG_ERR(("Event %#x already connected to the group %#x", (unsigned)event, (unsigned)group));
		result = BERR_TRACE(BERR_OS_ERROR);
	} else {
		BLST_D_INSERT_HEAD(&group->members, event, list);
		event->group = group;
		if (event->eventSet) {
			/* signal condition if signal already set */
            #if 0 /* TBDAG */
			pthread_cond_signal(&group->cond);
            #endif
		}
	}
	return result;
}

BERR_Code
BKNI_RemoveEventGroup(BKNI_EventGroupHandle group, BKNI_EventHandle event)
{
	BERR_Code result = BERR_SUCCESS;

	if (event->group != group) {
		BDBG_ERR(("Event %#x doesn't belong to the group %#x", event, group));
		result = BERR_TRACE(BERR_OS_ERROR);
	} else {
		BLST_D_REMOVE(&group->members, event, list);
		event->group = NULL;
	}
	return result;
}

static unsigned 
group_get_events(BKNI_EventGroupHandle group, BKNI_EventHandle *events, unsigned max_events)
{
	BKNI_EventHandle ev;
	unsigned event;

	for(event=0, ev=BLST_D_FIRST(&group->members); ev && event<max_events ; ev=BLST_D_NEXT(ev, list)) {
			if (ev->eventSet) {
				ev->eventSet = false;
				events[event] = ev;
				event++;
			}
	}
	return event;
}

BERR_Code 
BKNI_WaitForGroup(BKNI_EventGroupHandle group, int timeoutMsec, BKNI_EventHandle *events, unsigned max_events, unsigned *nevents)
{
	BSTD_UNUSED(timeoutMsec);
	if (max_events<1) {
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}
	*nevents = group_get_events(group, events, max_events);
	return *nevents ? BERR_SUCCESS:BERR_TIMEOUT;
}

void
BKNI_AssertIsrContext(const char *filename, unsigned lineno)
{
	BSTD_UNUSED(filename);
	BSTD_UNUSED(lineno);
	return ;
}
