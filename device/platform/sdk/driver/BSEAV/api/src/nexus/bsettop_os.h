/***************************************************************************
 *     Copyright (c) 2004-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_os.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 8/18/09 6:38p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_os.h $
 * 
 * 3   8/18/09 6:38p katrep
 * PR56109: Use calllbacks instead of using events for callbacks to the
 * app.
 * 
 * 2   6/4/08 11:24a rjlewis
 * PR40352: extra functions needed for VxWorks.
 * 
 * 1   3/31/08 6:04p jgarrett
 * PR 40606: Creating bsettop_os for nexus
 * 
 ***************************************************************************/
#ifndef BSETTOP_OS_H__
#define BSETTOP_OS_H__

/*=**********************************************
Provides additional operating system functionality
beyond BKNI. Each OS needs to implement these functions.

These functions run without respect to any thunk layer, therefore
they can be executed by high or low level settop api code.
*************************************************/

#include "bstd.h"
#include "bsettop_types.h"
#include "b_os_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

/* the following interface is used to set one shot timer callbacks */
typedef B_SchedulerTimerId b_timer_t;
typedef void (*b_timer_callback_t)(void *cntx);

/* Init/uninit os services */
bresult b_os_init(void);
bresult b_os_uninit(void);

/* this function is used to acquire global lock */
void b_lock(void);
/* this function is uses to release global lock (lock must be held by the current task before function is called) */
void b_unlock(void);
/* this function returns trus if current task is holds the lock, the sole purpose of this function is to be used with conjunction with BDBG_ASSERT, e.g. BDBG_ASSERT(b_lock_assert()); */
bool b_lock_assert(void);
#define B_LOCK_ASSERT() BDBG_ASSERT(b_lock_assert())

/**
Request a timer callback to occur no sooner than the specified in milliseconds.
It may come any amount of time later. If this function called within SettopApi core
callback would be called with global mutex (b_lock) held. Otherwise no particular
mutex held and it's user responsibility to call necessary synchronization functions.
**/
b_timer_t b_timer_schedule(unsigned delay /* ms */, b_timer_callback_t callback, void *cntx);

/**
Cancel a timer callback.
**/
void b_timer_cancel(b_timer_t timer);


typedef B_Time b_time_t;

/**
Get the current time.
The value of b_time_t is platform dependent. However, you can obtain a platform-independent
time diff by using b_time_diff.
**/
void b_time_get(b_time_t *time); /* this functions returns current system time (timestamp) */

/**
Returns difference in milliseconds between two timestamps obtained with b_time_get.

It's recomended that code would use this function in a way that it could return
only positive values (where applicable, it helps in handling wraparound conditions)

TODO: Regarding the above comment, we need to define specific behavior for wraparound. 
This either handles wrap-around and always returns a positive value, or it does not.
**/
long b_time_diff(const b_time_t *future,  const b_time_t *past);

/**
This function flushes and invalidates data cache in a specified memory region.

This function is used before a DMA output (write) operation to be sure that the DMA uses the flushed data.
This function is also used before a DMA input (read) operation to be sure that when the data is read it 
comes from the newly DMAed data (and not stale data found in the cache).

Note: the implementation ensures that both a flush AND an invailidate operation is performed.
**/
void b_cacheflush(const void *addr, size_t nbytes);

/* 
Summary:
The following two functions convert from b_lock state to idle state, and vice versa.

Description:
Both function is place for a race conditione (they don't swap locks atomically)
so critical state might need to be reexamined after function call.
*/
void b_lock2idle(void);
void b_idle2lock(void);

/* 
Summary:
The following two functions convert from b_lock state to slow state, and vice versa.

Description:
Both function is place for a race conditione (they don't swap locks atomically)
so critical state might need to be reexamined after function call.
*/
void b_lock2slow(void);
void b_slow2lock(void);


/* 
The b_event interface provides for receiving a task-context callback function when a KNI event is fired.

The callback function is called with b_lock already held.

The caller should execute the callback with the absolute minimum amount of time. All events
are serialized. You should never convert from lock2idle or lock2slow in an event callback because it will
cause all other event processing to block.

It is possible to call b_event_unregister from inside an event.

It is required that all b_event functions are called with b_lock held.
*/

typedef void (*b_event_callback_t)(void *cntx);

typedef B_SchedulerEventId b_event_id_t;

b_event_id_t b_event_register(B_EventHandle event, b_event_callback_t callback, void *cntx);
void b_event_unregister(b_event_id_t id);

/* 
The b_callback interface provides for receiving a task-context callback function when a KNI event is fired.


The caller should execute the callback with the absolute minimum amount of time. All callbacks 
are serialized by scheduler. 
*/
typedef void (*b_callback_t)(void *cntx,int param2);

typedef B_SchedulerCallbackHandle b_calllback_id_t;

b_calllback_id_t b_callback_create(void *pInstanceHandle, b_callback_t function,void *cntx,int param);
void b_callback_destroy(b_calllback_id_t callback);
void b_callback_fire(b_calllback_id_t callback);
void b_callback_stop(void *pInstanceHandle);


/***********************
*
* b_task api - used for starting tasks in different environments
*
*/
typedef struct b_task *b_task_t;
typedef void *(*b_task_func)(void *data);

/**
Summary:
Params for b_start_task. This is optional.
**/
typedef struct b_task_params
{
    unsigned priority;   /* 0 is highest, 100 is lowest. this is a hint and should
        not be considered a system-critical setting. */
    size_t stack_size; /* if 0, then default */
    const char *name; /* optional. used for debug. may be truncated depending on system capabilities. */
} b_task_params;

/**
Summary:
Set b_task_params defaults.
**/
void b_task_params_init(b_task_params *params);

/**
Summary:
Create a new task and call the function with the data parameter.
**/
bresult b_start_task(
    b_task_t *handle,   /* [out] new thread handle, passed to b_stop_task when finished */
    const b_task_params *params, /* param structure containing priority, task name, and stack size. */
    b_task_func func,   /* function to be called by new thread. when this func returns, the
        thread is done processing. */
    void *data          /* data to be passed to func */
    );

/**
Summary:
Clean up a finished task.
Description:
It is very important to realize that this function is not guaranteed to stop the task.
It assumes that the function has already exited or will exit on its own.
The application should set whatever conditions are required in order to make the
task function exit.
b_stop_task will block until the function is actually stopped and then it
will clean up whatever resources were allocated for the task.
**/
void b_stop_task(b_task_t handle);

#ifdef __vxworks

/**
Summary:
Return the size of a file.
Description:
Some OSs don't have support for large files.  This provides size for a large file.
**/
off_t b_file_size(int desc);

/**
Summary:
Seek to location in long file.
Description:
Some OSs don't have support for large files.  This provides a seek operation on a long file.
**/
off_t b_file_seek(int desc, off_t offset, int whence);

#endif

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_OS_H__ */
