/*
 * Copyright (c) 2007 Strategy and Technology Limited. All Rights Reserved.
 *
 * Copyright in the whole and every part of this file belongs to
 * Strategy and Technology Limited ("the Owner") and may not be used,
 * sold, supplied, transferred, copied, reproduced in whole or in part,
 * in any manner or form or in or on any media for the benefit of any person
 * other than in accordance with the terms of the Owner's agreement or
 * otherwise without the prior written consent of the Owner.
 *
 */
#if !defined ITK_OS_H__
#define ITK_OS_H__
/**
 * @file
 * Operating system resources
 */
#include "itk_types.h"

/**
 * @ingroup host
 * @defgroup os Operating system interface
 * @{
 */

/**
 * @defgroup thread Thread
 * @{
 */

/**
 * Thread priority settings
 */
typedef enum itk_thread_priority_t
{
    ITK_PRIORITY_LOW = 25,      /**< A low level thread makes use of available spare cycles */
    ITK_PRIORITY_LOWER = 40,    /**< An application level without user interactivity */
    ITK_PRIORITY_NORMAL = 50,   /**< An application level priority, with user interaction */
    ITK_PRIORITY_HIGH = 75,     /**< A thread that has higher processing requirements, it should be difficult to unschedule */ 
    ITK_PRIORITY_REALTIME = 100 /**< The thread processes realtime information */
} itk_thread_priority_t;

/**
 * The prototype for the entry point of a thread. 
 * 
 * @param[in] argv The argv value given when the thread was created
 */
typedef void (*itk_thread_entry_t)(void *argv);

/**
 * Interface for a thread
 */
typedef struct itk_thread_t
{
    /**
     * Deletes resources associated with a thread
     *
     * The thread is deleted and any resources associated with it can be
     * freed up. Threads for deletion should be in the finished state - 
     * this can be confirmed by using itk_thread_t::join. If the thread is not
     * finished then "bad things" can happen (ranging from failure to 
     * delete to disconnected Zombie threads)
     *
     * @param[in] thiz The handle of the thread to delete
     *
     * @see itk_thread_t::join()
     */
    void (*release)(struct itk_thread_t *thiz);

    /**
     * Waits for a thread to complete.
     *
     * Threads should never be deleted before they have had a chance to exit.
     * Threads exit by returning from their entry point function. When this occurs
     * they enter a finished state, after which they can be deleted. This call 
     * optionally waits for a thread to enter the finished state. If @a wait is
     * ITK_WAIT the calling thread waits indefinitely for the joining thread to exit.
     * If @a wait is ITK_NO_WAIT then the call returns immediately - indicating if the
     * thread is in the finished state or not.
     *
     * @warning Using @a wait = ITK_WAIT may cause deadlock if the joining thread has
     *          not been told to exit. If in doubt use a polling call.
     *
     * @param[in] thiz The thread to wait for
     * @param[in] wait ITK_WAIT if the thread should be waited for before returning
     *                 ITK_NO_WAIT if the function should return immediately
     *
     * @return ITKE_OK if the thread has entered the finished state and can be deleted
     * @return ITKE_NOT_READY if the thread has not entered the finished state and @a wait is ITK_NO_WAIT
     *
     * @see itk_thread_t::release()
     */
    itk_errcode_t (*join)(struct itk_thread_t *thiz, itk_wait_t wait);
} itk_thread_t;
/**
 * @} 
 **/

/**
 * @defgroup heap Memory Heap
 * @{
 */
struct itk_heap_t;

/**
 * The prototype for a heap tidy up callback
 *
 * During calls to alloc, zalloc and realloc the heap may fill to the
 * extent that the allocation will not fit. Rather than give up straight
 * away the heap should try and reduce the heap usage by invoking the
 * callback that is registered when the heap was created.
 *
 * The callback is invoked with the amount of memory required. This
 * will usually be the amount requested, not the difference between
 * the current available and the amount requested. The callback may try
 * to free up memory from the heap. If any memory is freed up then the
 * callback should return ITK_VALID, at which point the allocation will
 * be retried. It is likely that the callback will be invoked more than once,
 * so it can be implemented to free up as little or as much memory as 
 * desired.
 *
 * When the callback can no longer free up any more memory it must return
 * ITK_NOT_VALID.
 *
 * @param[in] context The context given when the callback is registered
 * @param[in] heap The heap that the callback is coming from
 * @param[in] status A status indicator
 *
 * @return ITK_VALID if some memory was recovered
 * @return ITK_NOT_VALID if no memory could be recovered
 */
typedef itk_validation_t (*itk_heap_callback_t)(void *context, struct itk_heap_t *heap, size_t required);

/**
 * Interface for a heap
 */
typedef struct itk_heap_t
{
    /**
     * Deletes a memory heap.
     *
     * @note This function does not free up all memory allocated from 
     * the heap. If there are still allocations from the heap when it is 
     * deleted this should be flagged as an error.
     *
     * @param[in] thiz The heap
     *
     * @return ITKE_OK if the heap is empty on release
     * @return ITKE_FAIL if the heap contains allocations
     */
    itk_errcode_t (*release)(struct itk_heap_t *thiz);

    /**
     * Allocates a block of memory from a heap
     *
     * Allocates at least size bytes of memory from a heap. The returned 
     * pointer is suitably aligned to be cast to all pointer types. The contents 
     * of the memory allocated is undefined on return.
     *
     * @note The info string pointer may be invalid on return. If the string is
     *       being used then it must be duplicated.
     *
     * @param[in] thiz The heap
     * @param[in] size The minimum size in bytes of the memory to allocate
     * @param[in] info An optional description string for the allocation, may be NULL
     *
     * @return NULL if the memory could not be allocated (size = 0 or heap is full)
     * @return A pointer to the allocated memory
     */
    void *(*alloc)(struct itk_heap_t *thiz, size_t size, const char *info);

    /**
     * Allocates a block of memory from the heap and initialises it to 0
     *
     * Allocates size bytes of memory from a heap. The returned pointer is
     * suitably aligned for all pointer types. The contents of the memory allocated
     * is zeroed before return.
     *
     * @note The info string pointer may be invalid on return. If the string is
     *       being used then it must be duplicated.
     *
     * @param[in] thiz The heap
     * @param[in] size The minimum size in bytes of the memory to allocate
     * @param[in] info An optional description string for the allocation, may be NULL
     *
     * @return NULL if the memory could not be allocated (size = 0 or heap is full)
     * @return A pointer to the allocated memory
     */
    void *(*zalloc)(struct itk_heap_t *thiz, size_t size, const char *info);

    /**
     * Reallocates a block of memory from the heap
     *
     * Memory points to a block of memory already allocated from a heap or NULL. 
     * In the case of memory being NULL the call allocates memory using the same 
     * semantics as alloc. The memory must be allocated from the same heap. 
     * The size can be smaller than the previous
     * allocation or larger. In all cases it is permissible for the return pointer
     * to be different from the calling pointer. The start of the new buffer is the
     * same as the previous allocation up to the smaller of the previous size and the
     * new size.
     *
     * @param[in] thiz The heap
     * @param[in] memory A previously allocated memory buffer or NULL
     * @param[in] size The minimum size in bytes of the memory to allocate
     *
     * @return NULL if the memory could not be reallocated. The memory parameter remains valid.
     * @return A pointer to the re-allocated memory if there is sufficient memory
     */
    void *(*realloc)(struct itk_heap_t *thiz, void *memory, size_t size);

    /**
     * Frees a memory allocation from the heap
     *
     * @param[in] thiz The heap
     * @param[in] memory A pointer to a previously allocated buffer
     */
    void (*free)(struct itk_heap_t *thiz, void *memory);

    /**
     * Returns the total size of heap memory allocated for client use.
     *
     * @note Subtracting the value returned from the heap's capacity does not
     *       necessarily indicate the size of the largest allocation possible
     *       from the heap due to fragmentation.
     *
     * @param[in] thiz The heap
     *
     * @return The total size of heap memory allocated for client use.
     */
    size_t (*getAllocated)(struct itk_heap_t *thiz);
} itk_heap_t;
/**
 * @} 
 */

/**
 * @defgroup semaphore Semaphores
 * @{
 */
/**
 * Semaphore interface
 */
typedef struct itk_semaphore_t
{
    /**
     * Deletes resources associated with a semaphore.
     *
     * @note The effect of releasing a semaphore that a thread is waiting on
     *       is undefined.
     *
     * @param[in] thiz The semaphore
     */
    void (*release)(struct itk_semaphore_t *thiz);

    /**
     * Waits for a semaphore to be non-zero and decrements
     *
     * The order for scheduling multiple threads woken on a semaphore is
     * not defined.
     *
     * @param[in] thiz The semaphore
     * @param[in] block If set to ITK_NO_WAIT then the call does not wait if the
     *            semaphore is 0, but returns immediately with ITKE_FAIL,
     *            otherwise the thread sleeps until the semaphore value > 0
     *
     * @return ITKE_OK if the call is successful
     * @return ITKE_FAIL if the call fails
     */
    itk_errcode_t (*wait)(struct itk_semaphore_t *thiz, itk_wait_t block);

    /**
     * Increments a semaphore
     *
     * @param[in] thiz The semaphore
     */
    void (*signal)(struct itk_semaphore_t *thiz);
} itk_semaphore_t;
/**
 * @} 
 */

/**
 * @defgroup timer Timers
 * @{
 */
struct itk_timer_t;

/**
 * Prototype for a timer callback
 *
 * @param[in] context The context given when the timer was created
 * @param[in] timer The timer that has been fired
 */
typedef void (*itk_timer_callback_t)(void *context, struct itk_timer_t *timer);

/**
 * Interface for a timer
 */
typedef struct itk_timer_t
{
    /**
     * Releases a timer instance
     *
     * A timer can be released while it is running/enabled. In this case
     * it is the host's responsibility to disable the timer before deleting it.
     *
     * @param[in] thiz The timer
     */
    void (*release)(struct itk_timer_t *thiz);

    /**
     * Sets the counter for the timer.
     *
     * The timer must be disabled to change the timeout counter.
     *
     * @param[in] thiz The timer
     * @param[in] ms The new timeout value in ms
     */
    void (*setCount)(struct itk_timer_t *thiz, itk_uint32_t ms);

    /**
     * Enables and disables a timer
     *
     * @param[in] thiz The timer
     * @param[in] state The new state to set the timer to
     */
    void (*control)(struct itk_timer_t *thiz, itk_control_t state);
} itk_timer_t;
/**
 * @} 
 */

/**
 * Operating System resource jump table
 */
typedef struct itk_os_t
{
    /**
     * Creates a thread within the host's process space.
     *
     * A thread is created and the specified entry point is called. The
     * value specified in argv is passed to the entry point. The call
     * includes a suggested stack size and priority. Higher values of 
     * priority indicate that the thread should be scheduled more often, 
     * but there is no explicit relationship between values of priority
     * given in this call and the other values used by the host. The
     * stack size may be ignored if the host does not support variable
     * stack sizes. 
     *
     * The function is synchronous. The thread created is immediately 
     * available for scheduling, but the timing of its first timeslice is not defined.
     *
     * @note The stack size does not take into account the stack 
     *       requirements of any host functions called by the thread.
     *
     * @note The name pointer may not be valid on return from the call. If the
     *       host requires this string then it must be duplicated.
     *
     * @param[in] thiz A pointer to the OS
     * @param[in] name A read-only pointer to a string that identifies the thread to create.
     *            It may be NULL and is not required to be unique
     * @param[in] entryPoint A read-only pointer to the entry point function of the thread
     * @param[in] argv A pointer to send as a parameter to the entry point function
     * @param[in] stackSize A recommended stack allocation in bytes
     * @param[in] priority A recommended (relative) thread priority
     *
     * @return A valid handle if the thread is created, NULL otherwise
     *
     * @see itk_os_t::getRunningThread, itk_thread_t::join, itk_thread_t::release
     */
    itk_thread_t *(*newThread)(struct itk_os_t* thiz,
                               const char *name,
                               const itk_thread_entry_t entryPoint,
                               void *argv,
                               size_t stackSize,
                               itk_thread_priority_t priority);

    /**
     * Creates a memory heap.
     *
     * @note Any information strings provided for allocations should not
     *       included in calculations of the total heap allocated.
     *
     * @note The name pointer may not be valid on return from the call. If the
     *       host requires this string then it must be duplicated.
     *
     * @param[in] thiz A pointer to the OS
     * @param[in] name A pointer to a name for the heap
     *            It may be NULL and is not required to be unique
     * @param[in] capacity The minimum total capacity of the heap
     * @param[in] callback A callback function to notify when an allocation can not be
     *            made as it would cause the heap to overflow.
     *            It may be NULL
     * @param[in] callbackContext Data to return to the callback
     *
     * @return NULL if the heap can not be created
     * @return A pointer to the created heap instance
     */
    itk_heap_t *(*newHeap)(struct itk_os_t* thiz,
                           const char *name, 
                           size_t capacity,
                           itk_heap_callback_t callback,
                           void *callbackContext);

    /**
     * Creates a semaphore.
     *
     * @note The name pointer may not be valid on return from the call. If the
     *       host requires this string then it must be duplicated.
     *
     * @param[in] thiz A pointer to the OS
     * @param[in] name A pointer to a name for the semaphore
     *            It may be NULL and is not required to be unique
     * @param[in] value The initial count
     *
     * @return NULL if the semaphore can not be created
     * @return A pointer to the created semaphore
     */
    itk_semaphore_t *(*newSemaphore)(struct itk_os_t* thiz,
                                     const char *name, 
                                     itk_uint16_t value);

    /**
     * Creates a timer
     *
     * The timer is created disabled. To enable it call itk_timer_t::control().
     *
     * @note The name pointer may not be valid on return from the call. If the
     *       host requires this string then it must be duplicated.
     *
     * @param[in] thiz A pointer to the OS
     * @param[in] name A pointer to a name for the timer
     *            It may be NULL and is not required to be unique
     * @param[in] countMs The initial timeout value. A value of 0 will cause the
     *            timer to fire immediately when it is enabled
     * @param[in] callback A callback function to invoke when the timer fires, 
     *            it may not be NULL
     * @param[in] callbackContext A pointer to pass to the callback when it is
     *            invoked
     *
     * @return NULL if the timer can not be created
     * @return A pointer to the created timer instance
     */
    itk_timer_t *(*newTimer)(struct itk_os_t* thiz,
                             const char *name,
                             itk_uint32_t countMs,
                             itk_timer_callback_t callback,
                             void *callbackContext);

    /**
     * Sleeps the current thread
     *
     * The time to sleep is in ms. A value of 0 indicates that the thread should 
     * relinquish the remainder of its timeslice and be rescheduled. The sleep
     * period is expected to be rounded up to the nearest scheduling quantum.
     *
     * @param[in] thiz A pointer to the OS
     * @param[in] ms The number of ms to sleep for, 0 = reschedule
     */
    void (*sleep)(struct itk_os_t* thiz, itk_uint32_t ms);

    /**
     * Gets the handle of the current thread
     *
     * Returns the handle of the current thread. If the current thread was not
     * created with itk_os_t::newThread() then the call returns NULL.
     *
     * @param[in] thiz A pointer to the OS
     *
     * @return The thread handle if the thread was created with itk_os_t::newThread
     * @return NULL if the thread was not created with itk_os_t::newThread
     *
     * @see itk_os_t::newThread
     */
    struct itk_thread_t *(*getRunningThread)(struct itk_os_t* thiz);
} itk_os_t;

/**
 * @} 
 **/
#endif
