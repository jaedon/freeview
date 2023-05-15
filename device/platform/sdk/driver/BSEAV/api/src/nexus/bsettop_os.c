/***************************************************************************
 *     Copyright (c) 2004-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_os.c $
 * $brcm_Revision: 9 $
 * $brcm_Date: 10/8/11 10:30a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_os.c $
 * 
 * 9   10/8/11 10:30a agin
 * SWNOOS-489:  Merge PR52128.
 * 
 * 8   8/18/09 6:38p katrep
 * PR56109: Use calllbacks instead of using events for callbacks to the
 * app.
 * 
 * 7   7/6/09 1:21p jgarrett
 * PR 55572: Switching thread termination mode
 * 
 * 6   5/12/09 5:38p jrubio
 * PR55063: Scheduler Destroy must be called before the Task Thread is
 * Destroyed, Scheduler Destroy will wait for task to finish
 * 
 * 5   4/14/09 3:23p jtna
 * PR51960: fixing memleak due to scheduler not getting destroyed
 * 
 * 4   6/9/08 5:18p rjlewis
 * PR40352: warnings.
 * 
 * 3   6/4/08 11:24a rjlewis
 * PR40352: extra functions needed for VxWorks.
 * 
 * 2   3/31/08 6:59p jgarrett
 * PR 40606: Fixing thread pointer
 * 
 * 1   3/31/08 6:04p jgarrett
 * PR 40606: Adding oslib support
 * 
 ***************************************************************************/
#include "bsettop_impl.h"

BDBG_MODULE(os);

static B_ThreadHandle g_thread;
static B_MutexHandle g_lock, g_slow, g_idle;
static B_SchedulerHandle g_scheduler;

static void b_os_p_thread_main(void *pParam)
{
    B_SchedulerHandle scheduler=pParam;

    B_Scheduler_Run(scheduler);
}

#ifdef NO_OS_DIAGS
void b_os_p_thread_main_no_os(void)
{
	b_os_p_thread_main(g_scheduler);
}
#endif

bresult b_os_init(void)
{
    bresult rc;
    B_Error errCode;
    B_ThreadSettings settings;

    errCode = B_Os_Init();
    if ( errCode )
    {
        return BSETTOP_ERROR(berr_external_error);
    }

    g_lock = B_Mutex_Create(NULL);
    if ( NULL == g_lock )
    {
        rc = BSETTOP_ERROR(berr_external_error);
        goto err_lock;
    }
    g_slow = B_Mutex_Create(NULL);
    if ( NULL == g_slow )
    {
        rc = BSETTOP_ERROR(berr_external_error);
        goto err_slow;
    }
    g_idle = B_Mutex_Create(NULL);
    if ( NULL == g_idle )
    {
        rc = BSETTOP_ERROR(berr_external_error);
        goto err_idle;
    }
    g_scheduler = B_Scheduler_Create(NULL);
    if ( NULL == g_scheduler )
    {
        rc = BSETTOP_ERROR(berr_external_error);
        goto err_scheduler;
    }
    B_Thread_GetDefaultSettings(&settings);
    settings.terminate = false;
    g_thread = B_Thread_Create("b_event", b_os_p_thread_main, g_scheduler, &settings);
    if ( NULL == g_thread )
    {
        rc = BSETTOP_ERROR(berr_external_error);
        goto err_thread;
    }

    /* Success */
    return b_ok;

err_thread:
    B_Scheduler_Destroy(g_scheduler);
err_scheduler:
    B_Mutex_Destroy(g_idle);
err_idle:
    B_Mutex_Destroy(g_slow);
err_slow:
    B_Mutex_Destroy(g_lock);
err_lock:
    B_Os_Uninit();
    return rc;
}

bresult b_os_uninit(void)
{
    
    if ( g_scheduler )
        {
            B_Scheduler_Stop(g_scheduler);
            B_Scheduler_Destroy(g_scheduler);
        }

    if ( g_thread )
    {
        B_Thread_Destroy(g_thread);
    }
    
    B_Os_Uninit();
      
    return b_ok;
}

/* this function is used to acquire global lock */
void b_lock(void)
{
    B_Mutex_Lock(g_lock);
}

/* this function is used to release global lock (lock must be held by the current task before function is called) */
void b_unlock(void)
{
    B_MUTEX_ASSERT_LOCKED(g_lock);
    B_Mutex_Unlock(g_lock);
}

/* this function returns trus if current task is holds the lock, the sole purpose of this function is to be used with conjunction with BDBG_ASSERT, e.g. BDBG_ASSERT(b_lock_assert()); */
bool b_lock_assert(void)
{
    return B_Mutex_P_IsLocked(g_lock);
}
#define B_LOCK_ASSERT() BDBG_ASSERT(b_lock_assert())

/**
Request a timer callback to occur no sooner than the specified in milliseconds.
It may come any amount of time later. If this function called within SettopApi core
callback would be called with global mutex (b_lock) held. Otherwise no particular
mutex held and it's user responsibility to call necessary synchronization functions.
**/
b_timer_t b_timer_schedule(unsigned delay /* ms */, b_timer_callback_t callback, void *cntx)
{
    return B_Scheduler_StartTimer(g_scheduler,
                                  g_lock,
                                  delay,
                                  callback,
                                  cntx);
}

/**
Cancel a timer callback.
**/
void b_timer_cancel(b_timer_t timer)
{
    B_Scheduler_CancelTimer(g_scheduler, timer);
}


/**
Get the current time.
The value of b_time_t is platform dependent. However, you can obtain a platform-independent
time diff by using b_time_diff.
**/
void b_time_get(b_time_t *time)
{
    B_Time_Get(time);
}

/**
Returns difference in milliseconds between two timestamps obtained with b_time_get.

It's recomended that code would use this function in a way that it could return
only positive values (where applicable, it helps in handling wraparound conditions)

TODO: Regarding the above comment, we need to define specific behavior for wraparound. 
This either handles wrap-around and always returns a positive value, or it does not.
**/
long b_time_diff(const b_time_t *future,  const b_time_t *past)
{
    return B_Time_Diff(future, past);
}

/**
This function flushes and invalidates data cache in a specified memory region.

This function is used before a DMA output (write) operation to be sure that the DMA uses the flushed data.
This function is also used before a DMA input (read) operation to be sure that when the data is read it 
comes from the newly DMAed data (and not stale data found in the cache).

Note: the implementation ensures that both a flush AND an invailidate operation is performed.
**/
void b_cacheflush(const void *addr, size_t nbytes)
{
    NEXUS_FlushCache(addr, nbytes);
}

/* 
Summary:
The following two functions convert from b_lock state to idle state, and vice versa.

Description:
Both function is place for a race conditione (they don't swap locks atomically)
so critical state might need to be reexamined after function call.
*/
void b_lock2idle(void)
{
    b_unlock();
    B_Mutex_Lock(g_idle);
}

void b_idle2lock(void)
{
    B_MUTEX_ASSERT_LOCKED(g_idle);
    B_Mutex_Unlock(g_idle);
    b_lock();
}

/* 
Summary:
The following two functions convert from b_lock state to slow state, and vice versa.

Description:
Both function is place for a race conditione (they don't swap locks atomically)
so critical state might need to be reexamined after function call.
*/
void b_lock2slow(void)
{
    b_unlock();
    B_Mutex_Lock(g_slow);
}

void b_slow2lock(void)
{
    B_MUTEX_ASSERT_LOCKED(g_slow);
    B_Mutex_Unlock(g_slow);
    b_lock();
}

/* 
The b_event interface provides for receiving a task-context callback function when a KNI event is fired.

The callback function is called with b_lock already held.

The caller should execute the callback with the absolute minimum amount of time. All events
are serialized. You should never convert from lock2idle or lock2slow in an event callback because it will
cause all other event processing to block.

It is possible to call b_event_unregister from inside an event.

It is required that all b_event functions are called with b_lock held.
*/

b_event_id_t b_event_register(B_EventHandle event, b_event_callback_t callback, void *cntx)
{
    return B_Scheduler_RegisterEvent(g_scheduler,
                                     g_lock,
                                     event,
                                     callback,
                                     cntx);
}

void b_event_unregister(b_event_id_t id)
{
    B_Scheduler_UnregisterEvent(g_scheduler, id);
}


void b_task_params_init(b_task_params *params)
{
    BKNI_Memset(params, 0, sizeof(*params));
    params->name = "b_task";
}

typedef struct b_task
{
    B_ThreadHandle thread;
    b_task_func func;
    void *data;
} b_task;

static void b_thread_launcher(void *param)
{
    b_task *task = param;

    task->func(task->data);
}

bresult b_start_task(b_task_t *handle, const b_task_params *params, b_task_func func, void *data)
{
    B_ThreadSettings threadSettings;

    BDBG_ASSERT(NULL != handle);
    BDBG_ASSERT(NULL != params);
    BDBG_ASSERT(NULL != func);

    B_Thread_GetDefaultSettings(&threadSettings);

    threadSettings.priority = params->priority;
    if ( params->stack_size )
    {
        threadSettings.stackSize = params->stack_size;
    }
    else
    {
        threadSettings.stackSize = 16*1024;
    }

    *handle = BKNI_Malloc(sizeof(b_task));
    if ( NULL == *handle )
    {
        return BSETTOP_ERROR(berr_out_of_memory);
    }

    (*handle)->func = func;
    (*handle)->data = data;
    threadSettings.terminate = false;
    (*handle)->thread = B_Thread_Create(params->name,
                                        b_thread_launcher,
                                        *handle,
                                        &threadSettings);
    if ( (*handle)->thread )
    {
        return b_ok;
    }
    else
    {
        BKNI_Free(handle);
        return BSETTOP_ERROR(berr_external_error);
    }
}

void b_stop_task(b_task_t handle)
{    
    B_Thread_Destroy(handle->thread);
    BKNI_Free(handle);
}

b_calllback_id_t b_callback_create(void *pInstanceHandle, b_callback_t function,void *cntx,int param)
{
    B_SchedulerCallbackHandle hCallback = B_SchedulerCallback_Create(g_scheduler,pInstanceHandle,NULL);
    if(!hCallback)
    {
        BDBG_ERR(("Failed to create callback"));
        return NULL;
    }
    if(B_SchedulerCallback_SetFunction(hCallback,function,cntx,param)!=B_ERROR_SUCCESS)
    {
        BDBG_ERR(("Failed to set callback function"));
        return NULL;
    }
    return (b_calllback_id_t)hCallback;
}
void b_callback_destroy(b_calllback_id_t callback)
{
    B_SchedulerCallback_Destroy(callback);
}
void b_callback_fire(b_calllback_id_t callback)
{
    B_SchedulerCallback_Fire(callback);
}

void b_callback_stop(void *pInstanceHandle)
{
    B_Scheduler_StopCallbacks(g_scheduler,pInstanceHandle);
}



#ifdef __vxworks

/*#include "nexus_file_posix.h" */
/* We have to define these here because the file above is defined to be a private include. */
off_t bfile_io_seek(int desc, off_t offset, int whence);
off_t bfile_io_size(int desc);

off_t b_file_seek(int desc, off_t offset, int whence)
{
    return bfile_io_seek(desc,offset,whence);
}

off_t b_file_size(int desc)
{
    return bfile_io_size(desc);
}

#endif

