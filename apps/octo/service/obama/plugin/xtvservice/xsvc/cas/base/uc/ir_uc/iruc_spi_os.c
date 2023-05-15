/*
 * (c) 2011-2012 Humax Co., Ltd.
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
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#include "octo_common.h"
#include <UniversalClient_Common_SPI.h>
#include <UniversalClient_Common_API.h>

#include <iruc_spi.h>

#define ______SPI_OS________________

//#define UC_OS_SEM_DEBUG
//#define UC_OS_MEM_DEBUG
//#define UC_OS_TASK_DEBUG

#define HIRUC_THREAD_INVALID_HANDLE (0xFFFFFFFF)
#define HIRUC_THREAD_MAX_CNT 	(5)

/* hjlee3 : mutex 동작중 mutex get 하지 못하는 현상이 발견 되어 semaphore 로 교체한다. */
#define IRUC_REPLACE_MUTEXTOSEMA

typedef struct _HIRUC_Threadinfo_
{
    HUINT32	task_id;

} HIRUC_Threadinfo_st;

STATIC HIRUC_Threadinfo_st ast_hiruc_thread_obj[HIRUC_THREAD_MAX_CNT] ;

/********************************************************************
*	Static variables
********************************************************************/


/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/


/********************************************************************
*	function name :	 local_SpiTimer_Callback
*	input : timer ID, parameter.
*	output : none
*	description :
********************************************************************/

void CAS_IRUC_SPI_OS_Init(void)
{
	HINT32 i;

	HxLOG_Print("%s\n", __FUNCTION__ );

	for(i=0; i < HIRUC_THREAD_MAX_CNT ; i++)
	{
		ast_hiruc_thread_obj[i].task_id = HIRUC_THREAD_INVALID_HANDLE;
	}
}

/** @defgroup memoryspi Universal Client Memory SPIs
 *  Universal Client Memory SPIs
 *
 *  These SPIs implement basic memory management functionality required for most operations.
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */
/**
 * Allocates a block of uninitialized memory.
 *
 * This function is called to allocate a block of memory and return a pointer to the beginning
 * of the block. If the system is out of memory, or another failure occurs, it must return NULL.
 * When the memory is no longer needed, ::UniversalClientSPI_Memory_Free will be called to free it.
 *
 * @param[in] bytes Number of bytes to allocate. If this is 0, the implementation MUST return an empty block
 *     of allocated memory.
 */
void* UniversalClientSPI_Memory_Malloc(uc_uint32 bytes)
{
	void *pvAdr = NULL;

	if(bytes > 0)
	{
		pvAdr = OxCAS_Calloc(bytes);
	}

#ifdef UC_OS_MEM_DEBUG
	HxLOG_Print("pvAdr = 0x%x, bytes = %d\n", pvAdr, bytes);
#endif

	return pvAdr;
}

/**
 * Frees a block of memory.
 *
 * This function is called to free a block of memory previously allocated using ::UniversalClientSPI_Memory_Malloc.
 *
 * @param[in] lpVoid Pointer to a block of previously allocated memory. If this is NULL, the implementation
 *     MUST handle it gracefully and ignore it. If this points to an area of memory that was not previously
 *     allocated by ::UniversalClientSPI_Memory_Malloc, OR it has already been freed, the result is undefined.
 */
void UniversalClientSPI_Memory_Free(void * lpVoid)
{
#ifdef UC_OS_MEM_DEBUG
	HxLOG_Print("pvAdr = 0x%x\n", lpVoid);
#endif
	OxCAS_Free(lpVoid);
	return;
}

/**
 * Report a fatal error.
 *
 * This function is called to report a fatal error.
 *
 * @param[in] type Error code, see ::uc_fatal_error_type.
 * @param[in] lpVoid Pointer to the null-terminated string to describe this error.
 */
void  UniversalClientSPI_FatalError(uc_fatal_error_type type,void* lpVoid)
{
	HxLOG_Error("*** Critical U-CAS Error type = %d, (%s)\n", type, (HUINT8 *)lpVoid);
	HxLOG_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
}

/** @} */ /* End of memory */

/** @defgroup semaphorespi Universal Client Semaphore SPIs
 *  Universal Client Semaphore SPIs
 *
 * These SPIs implement basic thread synchronization, semaphores.
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */
/**
 * Create a semaphore
 *
 * This function is called to create a semaphore.
 *
 * A 'semaphore' is a basic synchronization object with the following properties:
 * - It maintains a value, which is used internally to determine when to unblock waiters.
 * - Calling ::UniversalClient_Semaphore_Post atomically increments the value.
 * - Calling ::UniversalClient_Semaphore_Wait waits until the value > 0, then atomically decrements the value.
 *
 * Semaphores are used by the Universal Client to signal when events occur and to protect
 * data structures from simultaneous access.
 *
 * @param[in] initialValue Initial value for the semaphore.
 * @param[out] pSemaphoreHandle Receives a handle to a newly created semaphore. When the semaphore is no longer needed,
 *     it will be closed using ::UniversalClientSPI_Semaphore_Close.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Semaphore_Open(uc_uint32 initialValue,
                                             uc_semaphore_handle * pSemaphoreHandle)
{
	static HUINT32	semNumCount;
	HULONG 	sema_id;
	HUINT32	res;
	HUINT8 	semName[16]={0};

	semNumCount++;
	HxSTD_snprintf((char *)semName, 16, "IrUcasSem0x%X", semNumCount);

	res = VK_SEM_CreateWithCount(&sema_id, initialValue, semName, VK_SUSPENDTYPE_FIFO);
	if(res != VK_OK)
	{
		HxLOG_Error("Error,  Sem res = %d\n", res);
		return UC_ERROR_OS_FAILURE;
	}

	*pSemaphoreHandle = (uc_semaphore_handle) sema_id;

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Create Semaphore ID:OK(sem id  = 0x%X, pSemaphoreHandle = 0x%X)\n", sema_id, (HINT32)pSemaphoreHandle);
#endif

	return UC_ERROR_SUCCESS;
}
/**
 * Increment the semaphore value
 *
 * This function is called to atomically increment the value of a semaphore.
 * If the value goes above 0, this should cause threads waiting in a call to
 * ::UniversalClientSPI_Semaphore_Wait to be unblocked accordingly.
 *
 * @param[in] semaphoreHandle Handle of semaphore previously created using ::UniversalClientSPI_Semaphore_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Semaphore_Post(uc_semaphore_handle semaphoreHandle)
{
	HUINT32	res;

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Semaphore ID(0x%X) - Signal\n", semaphoreHandle);
#endif

	if(semaphoreHandle == NULL)
	{
		HxLOG_Error("semaphoreHandle NULL\n");
		return UC_ERROR_NULL_PARAM;
	}

	res = VK_SEM_Release( (HULONG) semaphoreHandle );
	if( res != VK_OK )
	{
		HxLOG_Error("Release Error -(error =0x%X), ID(0x%X)\n",res, (HINT32)semaphoreHandle);
		return UC_ERROR_OUT_OF_MEMORY;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Semaphore ID(0x%X) - Signal OK \n", semaphoreHandle);
#endif
	return UC_ERROR_SUCCESS;

}
/**
 * Decrement the semaphore value
 *
 * This function is called to atomically decrement the value of a semaphore.
 * It waits for the semaphore value to be greater than 0, then atomically decrements the value.
 * For example, if there are 3 threads blocked waiting on the semaphore, the value will be 0.
 * If another thread calls ::UniversalClientSPI_Semaphore_Post, it will increment the value to 1, and
 * ONE of the threads will be unblocked so that it can decrement the value back to 0. The other two
 * threads remain blocked.
 *
 *
 * @param[in] semaphoreHandle Handle of semaphore previously created using ::UniversalClientSPI_Semaphore_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
 uc_result UniversalClientSPI_Semaphore_Wait(uc_semaphore_handle semaphoreHandle)
{
	HUINT32	res;

	if (semaphoreHandle == NULL)
	{
		HxLOG_Error("semaphoreHandle NULL\n");
		return UC_ERROR_NULL_PARAM;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Semaphore ID(0x%X)- Get \n", semaphoreHandle);
#endif

	res = VK_SEM_Get( (HULONG) semaphoreHandle );
	if( res != VK_OK )
	{
		HxLOG_Error("VK_SEM_Get : Get Error - ID(0x%X)\n", (HINT32)semaphoreHandle);
		return UC_ERROR_OUT_OF_MEMORY;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Get OK\n", semaphoreHandle);
#endif
	return UC_ERROR_SUCCESS;
}

/**
 * Decrement the semaphore value.

This function is called to atomically decrement the value of a semaphore. It waits for the semaphore value to be greater than 0, then atomically decrements the value. For example, if there are 3 threads blocked waiting on the semaphore, the value will be 0. If another thread calls UniversalClientSPI_Semaphore_Post, it will increment the value to 1, and ONE of the threads will be unblocked so that it can decrement the value back to 0. The other two threads remain blocked.

Parameters:
[in]  semaphoreHandle  Handle of semaphore previously created using UniversalClientSPI_Semaphore_Open.

Return values:
 UC_ERROR_SUCCESS
 UC_ERROR_NULL_PARAM
 UC_ERROR_OUT_OF_MEMORY
 Other  The implementation may select any other suitable results to return from the Result Code List.
*/
uc_result UniversalClientSPI_Semaphore_WaitTimeout  ( uc_semaphore_handle  semaphoreHandle,  uc_uint32  milliseconds )
{
	HUINT32	res;

	if (semaphoreHandle == NULL)
	{
		HxLOG_Error("semaphoreHandle NULL\n");
		return UC_ERROR_NULL_PARAM;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Semaphore ID(0x%X)- Get \n", semaphoreHandle);
#endif

	res = VK_SEM_GetTimeout( (HULONG) semaphoreHandle, (HULONG)milliseconds );
	if( res != VK_OK )
	{
		HxLOG_Error("VK_SEM_GetTimeout : Get Error - ID(0x%X)\n", (HINT32)semaphoreHandle);
		return UC_ERROR_OUT_OF_MEMORY;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Get OK\n", semaphoreHandle);
#endif

	return UC_ERROR_SUCCESS;
}

/**
 * Destroy a semaphore object.
 *
 * This function is called to destroy a semaphore previously created using ::UniversalClientSPI_Semaphore_Open.
 *
 * @param[in] pSemaphoreHandle Pointer to handle of semaphore to destroy. The implementation
 *     should free any resources associated with the semaphore, and MUST set the value of *pSemaphoreHandle to NULL.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Semaphore_Close(uc_semaphore_handle * pSemaphoreHandle)
{
	int		res;

	if(pSemaphoreHandle == NULL)
	{
		HxLOG_Error("pSemaphoreHandle NULL\n");
		return UC_ERROR_NULL_PARAM;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Semaphore ID (0x%X)\n", *pSemaphoreHandle);
#endif

	res = VK_SEM_Destroy((HULONG) *pSemaphoreHandle);
	if( res != VK_OK )
	{
		HxLOG_Error("Destroy Fail - Semaphore ID(0x%X)\n", (HINT32)*pSemaphoreHandle);
		return UC_ERROR_OS_FAILURE;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Destroy OK\n");
#endif

	return UC_ERROR_SUCCESS;
}
/** @} */ /* End of semaphore */


/** @defgroup mutexspi Universal Client Mutex SPIs
 *  Universal Client Mutex SPIs
 *
 * These SPIs implement basic thread mutual exclusion via Mutex objects. This
 * is used to prevent multiple threads from simultaneously accessing
 * mutex-protected objects at the same time.
 *
 * Recursive mutex support is \b not required. That is, one thread will never
 * attempt to lock a mutex that is already locked by the thread. The mutex
 * implementation may safely ignore this case.
 *
 * \note The 'mutex' abstraction is provided to allow platforms to implement
 *    mutexes in a higher-performance manner than a typical semaphore implementation.
 *    Platforms which do not support a separate 'mutex' concept may safely use
 *    a semaphore internal to the implementation of the mutex.
 *
 *
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *
 *  @{
 */
/**
 * Create a mutex
 *
 * This function is called to create a mutex object.
 *
 * A 'mutex' is a basic synchronization object with the following properties:
 * - Calling ::UniversalClientSPI_Mutex_Lock locks a mutex. If the mutex was already locked, this
 *   waits until the mutex is unlocked, then locks it.
 * - Calling ::UniversalClientSPI_Mutex_Unlock unlocks the mutex.
 *
 * Mutexes are used by the Universal Client API to protect internal data structures
 * from being corrupted by simultaneous access.
 *
 * @param[out] pMutexHandle Receives a handle to a newly created mutex. When the mutex is no longer needed,
 *     it will be closed using ::UniversalClientSPI_Mutex_Close.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
 uc_result UniversalClientSPI_Mutex_Open(uc_mutex_handle * pMutexHandle)
{
#if defined(IRUC_REPLACE_MUTEXTOSEMA)
	uc_result res;
	uc_uint32 initialValue = 1;

	res = UniversalClientSPI_Semaphore_Open(initialValue, (uc_semaphore_handle *)pMutexHandle);
	if(res != UC_ERROR_SUCCESS)
	{
		HxLOG_Print("Creating pMutexHandle res = %d\n", res);
		return UC_ERROR_OS_FAILURE;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Create pMutexHandle ID:OK, sema id  = 0x%X, pMutexHandle = 0x%X\n", *pMutexHandle, pMutexHandle);
#endif
#else
	HUINT32	res;
	static HUINT32	semNumCount;
	HULONG 	sema_id;
	HUINT8 	semName[16]={0};

	semNumCount++;
	HxSTD_snprintf((char *)semName, 16, "IrUcasMutex0x%X", semNumCount);

	res = VK_MUTEX_Create(&sema_id, semName);
	if(res != VK_OK)
	{
		HxLOG_Error("Creating pMutexHandle res = %d\n", res);
		return UC_ERROR_OS_FAILURE;
	}

	*pMutexHandle = (uc_mutex_handle ) sema_id;

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Create pMutexHandle ID:OK, sema id  = 0x%X, pMutexHandle = 0x%X\n", sema_id, pMutexHandle);
#endif
#endif

	return UC_ERROR_SUCCESS;
}
/**
 * Locks a mutex
 *
 * This function is called to lock a mutex to prevent other threads from entering
 * a potentially unsafe section of code until the mutex is unlocked.
 *
 * @param[in] mutexHandle Handle of mutex previously created using ::UniversalClientSPI_Mutex_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
 uc_result UniversalClientSPI_Mutex_Lock(uc_mutex_handle mutexHandle)
{
#if defined(IRUC_REPLACE_MUTEXTOSEMA)
	uc_result res;

	if (mutexHandle == NULL)
	{
		HxLOG_Error("mutexHandle NULL\n");
		return UC_ERROR_NULL_PARAM;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("mutexHandle ID(0x%X)- Get \n",(HUINT32) mutexHandle);
#endif

	res = UniversalClientSPI_Semaphore_Wait((uc_semaphore_handle)mutexHandle);
	if( res != UC_ERROR_SUCCESS )
	{
		HxLOG_Error("mutexHandle : Get Error - ID(0x%X)\n",(HUINT32) mutexHandle);
		return UC_ERROR_OUT_OF_MEMORY;
	}

	HxLOG_Print("Get OK\n", mutexHandle);
#else
	HUINT32	res;

	if (mutexHandle == NULL)
	{
		HxLOG_Error("mutexHandle NULL\n");
		return UC_ERROR_NULL_PARAM;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("mutexHandle ID(0x%X)- Get \n",(HUINT32) mutexHandle);
#endif

	res = VK_MUTEX_Lock( (HULONG)mutexHandle );
	if( res != VK_OK )
	{
		HxLOG_Error("mutexHandle : Get Error - ID(0x%X)\n",(HUINT32) mutexHandle);
		return UC_ERROR_OUT_OF_MEMORY;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Get OK\n", mutexHandle);
#endif
#endif

	return UC_ERROR_SUCCESS;

}
/**
 * Unlocks a mutex
 *
 * This function is called to unlock a mutex, and release a single thread waiting in
 * call to ::UniversalClientSPI_Mutex_Lock, if any.  If no threads are waiting, then
 * the first thread to attempt to lock the mutex will not have to wait.
 *
 * @param[in] mutexHandle Handle of mutex previously created using ::UniversalClientSPI_Mutex_Open.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
 uc_result UniversalClientSPI_Mutex_Unlock(uc_mutex_handle mutexHandle)
{
#if defined(IRUC_REPLACE_MUTEXTOSEMA)
	uc_result res;

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("mutexHandle ID(0x%X) - Signal\n", mutexHandle);
#endif

	if(mutexHandle == NULL)
	{
		HxLOG_Error("mutexHandle NULL\n");
		return UC_ERROR_NULL_PARAM;
	}

	res = UniversalClientSPI_Semaphore_Post((uc_semaphore_handle) mutexHandle);
	if( res != UC_ERROR_SUCCESS )
	{
		HxLOG_Error("Release Error -(error =0x%X), ID(0x%X)\n",res, (HINT32)mutexHandle);
		return UC_ERROR_OUT_OF_MEMORY;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("mutexHandle ID(0x%X) - Signal OK \n", mutexHandle);
#endif
#else
	HUINT32	res;

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("mutexHandle ID(0x%X) - Signal\n", mutexHandle);
#endif
	if(mutexHandle == NULL)
	{
		HxLOG_Error("mutexHandle NULL\n");
		return UC_ERROR_NULL_PARAM;
	}

	res = VK_MUTEX_Unlock( (HULONG)mutexHandle );
	if( res != VK_OK )
	{
		HxLOG_Error("Release Error -(error =0x%X), ID(0x%X)\n",res, (HUINT32)mutexHandle);
		return UC_ERROR_OUT_OF_MEMORY;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("mutexHandle ID(0x%X) - Signal OK \n", mutexHandle);
#endif
#endif

	return UC_ERROR_SUCCESS;
}

/**
 * Destroy a mutex object.
 *
 * This function is called to destroy a mutex object previously created
 * using ::UniversalClientSPI_Mutex_Open.
 *
 * @param[in] pMutexHandle Pointer to handle of semaphore to destroy.
 *     The implementation
 *     should free any resources associated with the mutex, and MUST set
 *     the value of *pMutexHandle to NULL.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
 uc_result UniversalClientSPI_Mutex_Close(uc_mutex_handle * pMutexHandle)
{
#if defined(IRUC_REPLACE_MUTEXTOSEMA)
	uc_result res;

	if(pMutexHandle == NULL)
	{
		HxLOG_Error("pMutexHandle NULL\n");
		return UC_ERROR_NULL_PARAM;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Mutex ID (0x%X)\n", *pMutexHandle);
#endif

	res = UniversalClientSPI_Semaphore_Close((uc_semaphore_handle *)pMutexHandle);
	if( res != UC_ERROR_SUCCESS )
	{
		HxLOG_Error("Destroy Fail - Semaphore ID(0x%X)\n", (HINT32)*pMutexHandle);
		return UC_ERROR_OS_FAILURE;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Destroy OK, Mutex id = 0x%X\n", *pMutexHandle);
#endif
#else
	int res;

	if(pMutexHandle == NULL)
	{
		HxLOG_Error("pMutexHandle NULL\n");
		return UC_ERROR_NULL_PARAM;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Mutex ID (0x%X)\n", *pMutexHandle);
#endif

	res = VK_MUTEX_Destroy( (HULONG)*pMutexHandle);
	if( res != VK_OK )
	{
		HxLOG_Error("Destroy Fail - Mutex ID(0x%X)\n", (HUINT32) *pMutexHandle);
		return UC_ERROR_OS_FAILURE;
	}

#ifdef UC_OS_SEM_DEBUG
	HxLOG_Print("Destroy OK, Mutex id = 0x%X\n", *pMutexHandle);
#endif
#endif
	return UC_ERROR_SUCCESS;
}
/** @} */ /* End of mutexspi */


/** @defgroup threadspi Universal Client Thread SPIs
 *  Universal Client Thread SPIs
 *
 *  These SPIs implement basic thread management functions. The Universal Client requires
 *  one background thread that is uses for processing commands, ECMs, and EMMs.
 *
 * \note It is the integrator's responsibility to implement these methods for a particular platform.
 *  @{
 */
/**
 * Create a new thread and begin executing it.
 *
 * This function is called to create a new instance of a thread, a thread procedure and associated parameter.
 *
 * @param[in] threadProc Starting point of new thread
 * @param[in] lpVoid Parameter to pass to newly created thread
 * @param[out] pThreadHandle Receives an implementation-specific handle to the newly created thread.
 *     The caller will close the handle in a subsequent call to ::UniversalClientSPI_Thread_Close.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
 uc_result UniversalClientSPI_Thread_Open(uc_threadproc threadProc,
                                       void* lpVoid, uc_thread_handle * pThreadHandle)
{
	static HINT32 snTaskCnt=0;

	HINT32	i;
	HUINT8	task_name[32]={0};

	HUINT32	task_id;
	HUINT32	res = VK_ERROR;

#ifdef  UC_OS_TASK_DEBUG
	HxLOG_Print("\n");
#endif

	HxSTD_snprintf(task_name, 16, "IR-UCAS-TASK(%d)", snTaskCnt );

	res = VK_TASK_Create( (void *) threadProc, \
						(MW_CAS_TASK_PRIORITY+3), /*matching irdeto smartcard thread priority. IR3SCELL_TASK_PRIORITY_14, */ \
						MW_CAS_TASK_STACK_SIZE,/*minimumn 28k*/ \
						(char *)task_name, \
						lpVoid, \
						(unsigned long *)&task_id, \
						0 );

	if (res != VK_OK)
	{
		HxLOG_Error("Creaate thread failed, res = %d\n", res);
		return UC_ERROR_OS_FAILURE;
	}

	for(i=0; i< HIRUC_THREAD_MAX_CNT; i++)
	{
		if(ast_hiruc_thread_obj[i].task_id == HIRUC_THREAD_INVALID_HANDLE)
		{
			break;
		}
	}

	if (i  == HIRUC_THREAD_MAX_CNT)
	{
		HxLOG_Error(" there is no empty thread slot\n");
		return UC_ERROR_OS_FAILURE;
	}

	ast_hiruc_thread_obj[i].task_id = task_id;

	//pThreadHandle = (uc_thread_handle *) &(ast_hiruc_thread_obj[i].task_id) ;
	*pThreadHandle = (uc_thread_handle ) ast_hiruc_thread_obj[i].task_id;

	VK_TASK_Start(ast_hiruc_thread_obj[i].task_id);

#ifdef UC_OS_TASK_DEBUG
	HxLOG_Print("ok, Task ID = 0x%X, pThreadHandle= 0x%X \n", task_id, pThreadHandle);
#endif

	snTaskCnt++;

	return UC_ERROR_SUCCESS;
}
/**
 * Suspend the thread for the period specified.
 *
 * This function is called to suspend the thread for at least the period specified. The minimum granularity of the timer should be 10ms.
 *
 * \note If the requested delay time does not correspond with a boundary of the granularity, the delay time needs to be extended to the next available boundary of the granularity.
 *     For example, for platforms that have the 1 ms granularity, UniversalClientSPI_Thread_Sleep(hThreadHandle, 1) is to suspend the thread 1 ms;
 *       for platforms that have 5 ms granularity, UniversalClientSPI_Thread_Sleep(hThreadHandle, 1) is to suspend the thread 10 ms;
 *        for platforms that have 10 ms granularity, UniversalClientSPI_Thread_Sleep(hThreadHandle, 1) is to suspend the thread 10 ms.
 *
 * @param[in] hThreadHandle Pointer to handle of thread.
 * @param[in] wDelay is the time in milliseconds to suspend the thread.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
 uc_result UniversalClientSPI_Thread_Sleep(uc_thread_handle hThreadHandle, uc_uint16 wDelay)
{
    HUINT32   res;

#ifdef  UC_OS_TASK_DEBUG
	HxLOG_Print("wDelay(%d)\n", wDelay);
#endif

	HAPPY(hThreadHandle);

	/* If the value of hThreadHandle is NULL, it is an invalid value, the SPI implementation shall get the real handle of the thread which is
	calling UniversalClientSPI_Thread_Sleep. If the value of hThreadHandle is not NULL, it is a valid thread handle.  from developer guide. */
	/* TO DO: 다른 thread를 sleep 하는 기능 추가 필요 */
	if (wDelay < 10)
	{
		wDelay = 10;
	}

	res = VK_TASK_Sleep(wDelay);
	if( res != VK_OK )
	{
		HxLOG_Error("Thread Sleep Fail\n");
	}

	return UC_ERROR_SUCCESS;
}
/**
 * Closes a thread
 *
 * This function is called to close a thread object. This function should block until
 * the thread has completed execution. After this, the function should clean up any data associated with
 * the thread and free the handle.
 *
 * \note Threads are typically not closed until ::UniversalClient_Stop is called.
 *     For platforms that do not have the ability to properly wait for a thread to complete execution (i.e. thread 'join'),
 *     this can be simulated as necessary using some platform-specific approximation-- it will only affect the client during
 *     shutdown. For platforms that do not have the means to shut down, this can be ignored.
 *
 * @param[in] pThreadHandle Pointer to handle of thread to close.
 *
 * @retval
 *     ::UC_ERROR_SUCCESS
 * @retval
 *     ::UC_ERROR_NULL_PARAM
 * @retval
 *     ::UC_ERROR_OUT_OF_MEMORY
 * @retval
 *     Other The implementation may select any other suitable results to return from the \ref result "Result Code List".
 */
 uc_result UniversalClientSPI_Thread_Close(uc_thread_handle * pThreadHandle)
{
	 HUINT32 res;

#ifdef  UC_OS_TASK_DEBUG
	HxLOG_Print("\n");
#endif

	if(NULL == pThreadHandle)
	{
		HxLOG_Error("pThreadHandle NULL\n");
		return UC_ERROR_NULL_PARAM;
	}

	res = VK_TASK_Destroy((unsigned long)*pThreadHandle);
	if( res != VK_OK )
	{
		HxLOG_Error("res = 0x%X: Close Fail\n",res );
		return UC_ERROR_OUT_OF_MEMORY;
	}

	return UC_ERROR_SUCCESS;
}
/** @} */ /* End of thread */

