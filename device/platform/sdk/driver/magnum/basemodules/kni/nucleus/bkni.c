/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni.c $
 * $brcm_Revision: Hydra_Software_Devel/7 $
 * $brcm_Date: 3/10/10 3:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/kni/nucleus/bkni.c $
 * 
 * Hydra_Software_Devel/7   3/10/10 3:03p mward
 * SW7400-2712:  Add annotation to indicate to Coverity that BKNI_Fail()
 * is a "kill path".
 * 
 * Hydra_Software_Devel/6   6/7/07 2:20p alexp
 * PR31476: Do not enter critical section in the Vprintf.
 * 
 * Hydra_Software_Devel/5   2/22/07 12:14p alexp
 * PR27242: Remember previous preemption as well as interrupt stance and
 * restore them.
 * 
 * Hydra_Software_Devel/4   2/20/07 6:08p alexp
 * PR27242: Disable preemption in critical sections. Enter critical
 * section before printing.
 * 
 * Hydra_Software_Devel/3   1/22/07 5:31p alexp
 * PR27242: Clenup and added configuration function.
 * 
 * Hydra_Software_Devel/2   11/7/06 11:43a alexp
 * PR25228: Added background printing.
 * 
 * Hydra_Software_Devel/1   10/26/06 6:36p alexp
 * PR25228: nucleus plus support
 * 
 * Hydra_Software_Devel/18   5/16/06 3:09p alexp
 * PR3733: Merge from CVS - T20060512B0_BCM
 * 
 * Hydra_Software_Devel/17   1/23/06 6:20p lwhite
 * PR3331: Integration with TMM 20060119 drop
 * 
 * Hydra_Software_Devel/16   12/4/05 4:01p alexp
 * PR3368: Added assert on memory deallocation error.
 * 
 * Hydra_Software_Devel/15   10/11/05 1:47p alexp
 * PR3368: Added BDBG_LOCK/UNLOCK macros that check context before
 * locking.
 * 
 * Hydra_Software_Devel/14   7/20/05 9:50a alexp
 * PR2966: Added confirmation of disabled interrupts
 * 
 * Hydra_Software_Devel/13   4/22/05 4:48p alexp
 * PR2966: Fix for wait less than 1 tick if asked to wait for 1 ms.
 * 
 * Hydra_Software_Devel/10   4/4/05 4:50p alexp
 * PR2966: Integrated frunction from hermes tree
 * 
 * Hydra_Software_Devel/9   4/4/05 4:30p alexp
 * PR3077: Updated BKNI_Malloc to always return aligned memory. Memory is
 * aligned on 8 bytes which should work for 32/64 bit modes.
 * 
 * Hydra_Software_Devel/8   4/4/05 11:58a liangl
 * PR3077: bkni_malloc returned 4 byte aluignment, even for double word
 * 
 * Hydra_Software_Devel/7   12/13/04 4:08p alexp
 * PR2487:
 * Updated for not throwing an exception but looping so you can get a
 * stack trace.
 * 
 * Hydra_Software_Devel/6   11/16/04 8:24p alexp
 * PR2487:
 * Update for new tick duration
 * 
 * Hydra_Software_Devel/5   11/3/04 4:42p alexp
 * PR2487:
 * Added vsnprintf which somehow was missing the first time.
 * 
 * Hydra_Software_Devel/4   10/28/04 12:39p alexp
 * PR2487:
 * Removed SDA optimization in syslib and moved free memory pointer to
 * bkni.c This way SDA can be optimized for this file.
 * 
 * Hydra_Software_Devel/3   10/26/04 2:59p alexp
 * PR2487:
 * Merged bkni_mem.c in to bkni.c.
 * Addressed code review comments
 * 
 * Hydra_Software_Devel/2   10/15/04 11:02a alexp
 * PR2487:
 * Fixed  BKNI_Delay implementation. Adjusted constants to match cpu
 * speed.
 * 
 * Hydra_Software_Devel/1   10/4/04 2:49p alexp
 * PR2487:
 * KNI nucleus port checkin
 * 
 * Hydra_Software_Devel/3   6/9/03 8:16p vsilyaev
 * Update to the latest kernelinterface document.
 * 
 * Hydra_Software_Devel/2   4/4/03 6:35p vsilyaev
 * Removed task support.
 * 
 * Hydra_Software_Devel/1   3/11/03 10:19p vsilyaev
 * The stubs version of the kernel interface functions. Required ISO C90
 * compatible compiler.
 * 
 ***************************************************************************/


#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

BDBG_MODULE(BKNI);

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* nucleus include file */
#include "nucleus.h"
#include "bkni_print.h"

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
#undef BKNI_Malloc
#undef BKNI_Free

static CHAR mu_name[] = "kni_mtx";
static CHAR ev_name[] = "kni_evt";

static struct critical_stance_t {
    INT int_level;
    OPTION preempt;
} cs = {
    NU_ENABLE_INTERRUPTS,
    NU_PREEMPT
};

static void * memory_start;

BERR_Code BKNI_NU_InitSystemPool(void * mem_start);
void BKNI_NU_ReleaseSystemPool(void);

#define BKNI_MS_PER_NU_TICK (1000/NU_PLUS_Ticks_Per_Second)
#define BKNI_MS2TICKS(ms) ((0 == ms % BKNI_MS_PER_NU_TICK) ? (ms / BKNI_MS_PER_NU_TICK) : (ms / BKNI_MS_PER_NU_TICK + 1))
/*default heap size is 3MB*/
#define SYSTEM_POOL_SIZE 0x300000 


#define KNI_EVENT 0x00000001

struct BKNI_EventObj
{
    NU_EVENT_GROUP evt;
};

struct BKNI_MutexObj
{
    NU_SEMAPHORE sema;
};

struct BKNI_TaskObj
{
    NU_TASK task;
};

unsigned BKNI_Nucleus_Config(void * mem_start)
{
    memory_start = mem_start;
    return SYSTEM_POOL_SIZE;
}

#if BDBG_DEBUG_BUILD
/**
  Debug mutex outside of debug module. The one inside is static and not 
  accessable
*/
static BKNI_MutexHandle dbg_mutex;

BERR_Code BKNI_Init(void)
{
    BERR_Code berr;
    berr = BKNI_NU_InitSystemPool(memory_start);
    if(BERR_SUCCESS == berr){
        berr = BKNI_CreateMutex(&dbg_mutex);
    }
    BKNI_Print_Init();
    return berr;
}

void BKNI_Uninit(void)
{
    BKNI_DestroyMutex(dbg_mutex);
    BKNI_NU_ReleaseSystemPool();
    return;
}
#else
BERR_Code BKNI_Init(void)
{
    BERR_Code berr;
    berr = BKNI_NU_InitSystemPool(memory_start);
    BKNI_Print_Init();
    return berr;
}

void BKNI_Uninit(void)
{
    BKNI_NU_ReleaseSystemPool();
}
#endif


BERR_Code BKNI_CreateMutex(BKNI_MutexHandle *pMutex)
{
   struct BKNI_MutexObj * mutex;
   STATUS st = 0;
   mutex = BKNI_Malloc(sizeof (struct BKNI_MutexObj));
   if(NULL == mutex){
      return BERR_OS_ERROR;
   }else{
      BKNI_Memset(mutex, 0, sizeof(struct BKNI_MutexObj));
      st = NU_Create_Semaphore(&(mutex->sema), mu_name, 1, NU_FIFO);
      if(NU_SUCCESS != st){
         BKNI_Free(mutex);
         return BERR_OS_ERROR;
      }
   }
   BDBG_ASSERT(pMutex);
   *pMutex = mutex;
   return BERR_SUCCESS;

}

void BKNI_DestroyMutex(BKNI_MutexHandle mutex)
{
    BDBG_ASSERT(mutex);
/* dont need to check return as there is nothing we can do */
    NU_Delete_Semaphore(&(mutex->sema));
    BKNI_Free(mutex);
    return ;
}

BERR_Code BKNI_TryAcquireMutex(BKNI_MutexHandle mutex)
{
    STATUS st;
    BERR_Code err;
    BDBG_ASSERT(mutex);
    st = NU_Obtain_Semaphore(&(mutex->sema), NU_NO_SUSPEND);
    switch(st){
    case NU_SUCCESS:
        err = BERR_SUCCESS;
        break;
    case NU_TIMEOUT:
        err = BERR_TIMEOUT;
        break;
    case NU_INVALID_SEMAPHORE:
    case NU_INVALID_SUSPEND:
    case NU_SEMAPHORE_DELETED:
    case NU_SEMAPHORE_RESET:
    case NU_UNAVAILABLE:
    default:
        err = BERR_OS_ERROR;
        break;
    }
    return err;
}

BERR_Code BKNI_AcquireMutex(BKNI_MutexHandle mutex)
{
    STATUS st;
    BERR_Code err;
    BDBG_ASSERT(mutex);
    st = NU_Obtain_Semaphore(&(mutex->sema), NU_SUSPEND);
    switch(st){
    case NU_SUCCESS:
        err = BERR_SUCCESS;
        break;
    case NU_TIMEOUT:
    case NU_INVALID_SEMAPHORE:
    case NU_INVALID_SUSPEND:
    case NU_SEMAPHORE_DELETED:
    case NU_SEMAPHORE_RESET:
    case NU_UNAVAILABLE:
    default:
        err = BERR_OS_ERROR;
        break;
    }
    return err;
}

void BKNI_ReleaseMutex(BKNI_MutexHandle mutex)
{
    STATUS st;
    CHAR sema_name[8];
    UNSIGNED count;
    OPTION type;
    UNSIGNED tasks_waiting;
    NU_TASK *first_task;
/* we need to check if sema already 1 so we do not up it too many times */
    BDBG_ASSERT(mutex);
    st = NU_Semaphore_Information(&(mutex->sema), sema_name,
                                  &count, &type, &tasks_waiting, &first_task);
    if(NU_SUCCESS != st){
        return;
    }
    if(0 ==  count){
        st = NU_Release_Semaphore(&(mutex->sema));
        /*don't check return, we cant do anything */
    }
    return ;
}

/* coverity[+kill]  */
void BKNI_Fail(void)
{
    NU_TASK * task_ptr;
    CHAR name[9];
    DATA_ELEMENT task_status;
    UNSIGNED sch_count, time_slice, stack_size, minimum_stack;
    OPTION priority, preempt;
    STATUS sts;
    VOID * stack_base;
    task_ptr = NU_Current_Task_Pointer();
    sts = sts;
    sts = NU_Task_Information( task_ptr, name, &task_status, &sch_count,
                               &priority, &preempt, &time_slice,
                               &stack_base, &stack_size, &minimum_stack);
    name[8] = 0;
    BKNI_Printf("Task %s failed. Looping in %s. Attach your debugger.",
                name, __PRETTY_FUNCTION__);
    for(;;) { } /* hang here */
}

int BKNI_Printf(const char *fmt, ...)
{
    va_list arglist;
    int rc;

    va_start( arglist, fmt );
    rc = BKNI_Vprintf(fmt, arglist);
    va_end(arglist);

    return rc;
}

int BKNI_Snprintf(char *str, size_t len, const char *fmt, ...)
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

#define BKNI_USE_PRINT

int BKNI_Vprintf(const char *fmt, va_list ap)
{
#ifndef BKNI_USE_PRINT
    return vprintf(fmt, ap);
#else
    return BKNI_Print_Vprintf(fmt, ap);
#endif
}

int BKNI_Vsnprintf(char *s, size_t n, const char *fmt, va_list ap)
{
     return vsnprintf(s, n, fmt, ap);
}

void BKNI_Delay(unsigned int microsec)
{
    volatile int l;

    while(microsec--) {
         for(l=100; l>0; l--) ;
    }
    return;
}

BERR_Code BKNI_Sleep(unsigned int millisec)
{
    if(0 == millisec){
        NU_Relinquish();
    }else{
        NU_Sleep(BKNI_MS2TICKS(millisec));
    }
    return BERR_SUCCESS;
}


BERR_Code BKNI_CreateEvent(BKNI_EventHandle *pEvent)
{
    struct BKNI_EventObj * event;
    STATUS st = 0;
    event = BKNI_Malloc(sizeof (struct BKNI_EventObj));
    if(NULL == event){
        return BERR_OS_ERROR;
    }else{
        BKNI_Memset(event, 0, sizeof(struct BKNI_EventObj));
        st = NU_Create_Event_Group(&(event->evt),ev_name);
        if(NU_SUCCESS != st){
            BKNI_Free(event);
            return BERR_OS_ERROR;
        }
    }
    BDBG_ASSERT(pEvent);
    *pEvent = event;
    return BERR_SUCCESS;
}

void BKNI_DestroyEvent(BKNI_EventHandle event)
{
    BDBG_ASSERT(event);
    NU_Delete_Event_Group(&(event->evt));
    /* discard return value, there is nothing we can do */
    BKNI_Free(event);
    return;
}

BERR_Code BKNI_WaitForEvent(BKNI_EventHandle event, int timeoutMsec)
{
    UNSIGNED suspend;
    STATUS st = 0;
    UNSIGNED ret_events;
    BDBG_ASSERT(event);
    if(BKNI_INFINITE == timeoutMsec){
        suspend = NU_SUSPEND;
    }else if(0 == timeoutMsec){
        suspend = NU_NO_SUSPEND;
    }else{
        suspend = BKNI_MS2TICKS(timeoutMsec);
    }
    st = NU_Retrieve_Events(&(event->evt),KNI_EVENT,
                            NU_AND_CONSUME, &ret_events, suspend);
    switch(st){
    case NU_SUCCESS:
        return BERR_SUCCESS;
    case NU_TIMEOUT:
    case NU_NOT_PRESENT:
        return BERR_TIMEOUT;
    case NU_INVALID_GROUP:
    case NU_INVALID_POINTER:
    case NU_INVALID_OPERATION:
    case NU_INVALID_SUSPEND:
    case NU_GROUP_DELETED:
    default:
        break;
    }
    return BERR_OS_ERROR;
}

void BKNI_SetEvent(BKNI_EventHandle event)
{
    STATUS st = 0;
    BDBG_ASSERT(event);
    st= NU_Set_Events(&(event->evt), KNI_EVENT, NU_OR);
    if(NU_SUCCESS != st){
        return;
    }
    return ;
}

void BKNI_ResetEvent(BKNI_EventHandle event)
{
    STATUS st = 0;
    BDBG_ASSERT(event);
    st= NU_Set_Events(&(event->evt), 0, NU_AND);
    if(NU_SUCCESS != st){
        return;
    }
    return ;
}


void BKNI_EnterCriticalSection(void)
{
    cs.preempt = NU_Change_Preemption(NU_NO_PREEMPT);
    cs.int_level = NU_Control_Interrupts(NU_DISABLE_INTERRUPTS);
    return ;
}

void BKNI_LeaveCriticalSection(void)
{
    NU_Control_Interrupts(cs.int_level);
    NU_Change_Preemption(cs.preempt);
    return ;
}

void * BKNI_Memset(void *b, int c, size_t len)
{
    return memset(b, c, len);
}

void * BKNI_Memcpy(void *dst, const void *src, size_t len)
{
    return memcpy(dst, src, len);
}

int BKNI_Memcmp(const void *b1, const void *b2, size_t len)
{
    return memcmp(b1, b2, len);
}

void * BKNI_Memchr(const void *b, int c, size_t len)
{
    return memchr(b, c, len);

}

void * BKNI_Memmove(void *dst, const void *src, size_t len)
{
    return memmove(dst, src, len);
}

/*minimum bytes allocated is 1*/
#define MIN_ALLOC_BYTES 1
/*memory must be aligned on dword(8bytes) for 64 bit mode and on word(4bytes) for 32*/
#define ALIGN_BYTES 8

static NU_MEMORY_POOL system_pool;
static CHAR mp_name[] = "system";
/** state variable to prevent errors on multiple inits */
static INT init_completed = 0;

BERR_Code BKNI_NU_InitSystemPool(void * mem_start)
{
    STATUS st;
    if(1 == init_completed){
        return BERR_SUCCESS;
    }
    st = NU_Create_Memory_Pool(&system_pool, mp_name, mem_start,
                               SYSTEM_POOL_SIZE, MIN_ALLOC_BYTES, NU_FIFO);
    if(NU_SUCCESS == st){
        init_completed = 1;
        return BERR_SUCCESS;
    }
    else
        return BERR_OS_ERROR;
}

void BKNI_NU_ReleaseSystemPool(void)
{
    init_completed = 0;
    NU_Delete_Memory_Pool(&system_pool);
    /*ignore return value. we cant do much*/
    return;
} 

void * BKNI_Malloc(size_t size)
{
    void * ptr;
    STATUS st;
    if(0 == size)
        return NULL;
    st = NU_Allocate_Aligned_Memory(&system_pool, &ptr, size, ALIGN_BYTES, 
                                    NU_NO_SUSPEND);

    return ((NU_SUCCESS == st) ? ptr : NULL);
}

void BKNI_Free(void * ptr)
{
    STATUS st;
    BSTD_UNUSED(st);
    st = NU_Deallocate_Memory(ptr);
    /*ignore return, we cant do anything*/
    BDBG_ASSERT(NU_SUCCESS == st);
}

/*****************************************************************************
 * Function Name : BKNI_GetSystemPoolPointer
 * Author        : Martin Gibson
 * Creation date : 12/3/2004
 * Description   : Returns a pointer to the system memory pool allocated inside 
 *                 BKNI (by NU_Create_Memory_Pool)
 *    Input        - none
 *    Output       - none
 *    Returns      - pointer to pool resource if OK, else NULL
 *    
 * History :
 * Date        Author     Modification
 * 12/3/2004  Martin     Created                     
 *****************************************************************************/
void* BKNI_GetSystemPoolPointer( void )
{
    return (void*)(&system_pool);
}
/* BKNI_GetSystemPoolPointer */

#if BDBG_DEBUG_BUILD
void BDBG_NU_Lock(void)
{
    if(NU_NULL != NU_Current_Task_Pointer()){
        BKNI_AcquireMutex(dbg_mutex);
    }
}

void BDBG_NU_Unlock(void)
{
    if(NU_NULL != NU_Current_Task_Pointer()){
        BKNI_ReleaseMutex(dbg_mutex);
    }
}
#endif
