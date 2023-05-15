/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni_multi.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 10/4/04 2:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSSS/VxTeam/patches/nucleus/bkni_multi.h $
 * 
 * Hydra_Software_Devel/1   10/4/04 2:50p alexp
 * PR2487:
 * KNI nucleus port checkin
 * 
 * Hydra_Software_Devel/19   4/8/03 4:47p erickson
 * BKNI_ResetEvent now returns void
 * Split BKNI_AcquireMutex into Acquire and TryAcquire
 * 
 * Hydra_Software_Devel/18   4/4/03 12:24p erickson
 * trying to fix list item
 * 
 * Hydra_Software_Devel/17   4/4/03 12:22p erickson
 * updated docs
 * 
 * Hydra_Software_Devel/16   4/4/03 11:27a erickson
 * updated documentation
 * 
 * Hydra_Software_Devel/15   4/3/03 6:13p erickson
 * some pre 0.9 api rework
 * 
 * Hydra_Software_Devel/14   4/1/03 7:02p erickson
 * updated comments
 * 
 * Hydra_Software_Devel/13   4/1/03 3:16p erickson
 * update AcquireMutex notes
 * 
 * Hydra_Software_Devel/12   3/25/03 11:44a erickson
 * Updated mutex commnets
 * 
 * Hydra_Software_Devel/11   3/24/03 11:38a erickson
 * reworked docs
 * 
 * Hydra_Software_Devel/10   3/12/03 12:04p erickson
 * updated return codes, comments
 * 
 * Hydra_Software_Devel/9   3/11/03 6:59p erickson
 * changed kernelinterface from using object ptrs to handles
 * 
 * Hydra_Software_Devel/8   3/10/03 8:28p vsilyaev
 * Added support for tagged kernel interface.
 * 
 * Hydra_Software_Devel/7   3/10/03 6:37p vsilyaev
 * Uses extern "C" { } brackets.
 *
 * Hydra_Software_Devel/6   3/10/03 2:35p vsilyaev
 * Fixed the BKNI_TaskPriority enum.
 *
 * Hydra_Software_Devel/5   3/10/03 2:34p erickson
 * expanded comments
 *
 * Hydra_Software_Devel/5   3/10/03 2:29p erickson
 * updated docs
 *
 * Hydra_Software_Devel/5   3/10/03 12:36p erickson
 * linuxkernel tasks initial implementationd
 *
 * Hydra_Software_Devel/3   3/7/03 5:20p erickson
 * linux kernel interface work
 *
 * Hydra_Software_Devel/2   3/6/03 6:27p erickson
 * rework KNI api
 *
 * Hydra_Software_Devel/1   3/5/03 5:16p erickson
 * Initial kernelinterface work
 *
 ***************************************************************************/
#ifndef BKNI_MULTI_H__
#define BKNI_MULTI_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=*************************************************************************
The multi-tasking kernel interface can only be used by multi-tasking
modules. In the Magnum architecture, this can only be SysLib modules.

See bkni.h for a kernel interface overview.
****************************************************************************/

/***************************************************************************
Summary:
	BKNI_INFINITE can be passed as a timeout value into BKNI_WaitForEvent
	for multi-threaded modules.
****************************************************************************/
#define BKNI_INFINITE -1

/***************************************************************************
Summary:
	Mutex object initialized by BKNI_CreateMutex.
****************************************************************************/
typedef struct BKNI_MutexObj *BKNI_MutexHandle;

/***************************************************************************
Summary:
	Create a mutex in an unacquired state.

Description:
	The mutex is returned in an unacquired state, which means that the first call
	to BKNI_AcquireMutex or BKNI_TryAcquireMutex for that mutex is guaranteed to succeed immediately.

	Note that there is no name parameter in BKNI_CreateMutex. We do not support named
	mutexes because they are essentially global variables that can lead to deadlock.
	Passing in a name for debugging purposes might lead to someone to think we
	support named mutexes.

Returns:
	BERR_SUCCESS if the mutex is created.
	BERR_OS_ERROR if mutex was not created.
****************************************************************************/
BERR_Code BKNI_CreateMutex(BKNI_MutexHandle *mutex);


/***************************************************************************
Summary:
	Destroy a mutex.

Description:
	Destroying a mutex in an acquired state is not allowed and leads to undefined behavior.
****************************************************************************/
void BKNI_DestroyMutex(BKNI_MutexHandle mutex);


/***************************************************************************
Summary:
	Try acquiring exclusive ownership of a mutex, but do not block.

Description:
	If no other task currently owns the mutex, BKNI_TryAcquireMutex will return
	with BERR_SUCCESS and the caller retains exclusive ownership until
	BKNI_ReleaseMutex is called for that mutex.

	If another task currently owns the mutex, BKNI_TryAcquireMutex will not block
	but will return immediately with BERR_TIMEOUT.

	Magnum code cannot nest calls to BKNI_TryAcquireMutex or BKNI_AcquireMutex.
	If a mutex has already been acquired in a task, that same task cannot acquire it
	a second time. It leads to undefined behavior. Some possible results include:

	* Deadlock. The function will never return and the task is hung forever.
	* System halts.
	* Error returned and mutex is only acquired once. If the error code is not handled (which is a violation of Magnum rules), race conditions will be introduced.
	* No error returned and mutex is acquired twice (two releases are required). This may allow bad code to be hidden until another platform uncovers the problem.

	A platform's implementation may chose any of the above results, although we strongly
	recommend that the function fail and return an error code if possible.

Returns:
	BERR_SUCCESS - Mutex was acquired successfully.
	BERR_TIMEOUT - The mutex is already acquired by another task and so the mutex was not acquired.
	BERR_OS_ERROR - The system failed and the mutex was not acquired.

See Also:
	BKNI_AcquireMutex, BKNI_ReleaseMutex, Magnum ThreadSafe rules
****************************************************************************/
BERR_Code BKNI_TryAcquireMutex(
	BKNI_MutexHandle mutex
	);


/***************************************************************************
Summary:
	Acquire exclusive ownership of a mutex, possibly blocking.

Description:
	Acquire exclusive ownership of a mutex. If another task currently
	owns the mutex, BKNI_AcquireMutex will block until the mutex can be acquired.
	After acquiring the mutex, the caller retains exclusive ownership until
	BKNI_ReleaseMutex is called for that mutex.

	Magnum code cannot nest calls to BKNI_AcquireMutex or BKNI_TryAcquireMutex.
	If a mutex has already been acquired in a task, that same task cannot acquire it
	a second time. It leads to undefined behavior. Some possible results include:

	* Deadlock. The function will never return and the task is hung forever.
	* System halts.
	* Error returned and mutex is only acquired once. If the error code is not handled (which is a violation of Magnum rules), race conditions will be introduced.
	* No error returned and mutex is acquired twice (two releases are required). This may allow bad code to be hidden until another platform uncovers the problem.

	A platform's implementation may chose any of the above results, although we strongly
	recommend that the function fail and return an error code if possible.

Returns:
	BERR_SUCCESS - Mutex was acquired successfully.
	BERR_OS_ERROR - The system failed or was interrupted, and the mutex was not acquired.

See Also:
	BKNI_TryAcquireMutex, BKNI_ReleaseMutex, Magnum ThreadSafe rules
****************************************************************************/
BERR_Code BKNI_AcquireMutex(
	BKNI_MutexHandle mutex
	);

/***************************************************************************
Summary:
	Release exclusive ownership of a mutex.

Description:
	If you successfully acquired the mutex using BKNI_AcquireMutex or BKNI_TryAcquireMutex,
	BKNI_ReleaseMutex will release the mutex.

	In Magnum code, releasing a mutex which was acquired in a different task is not
	allowed	and leads to undefined behavior.
	Also, releasing a mutex which is not in an acquired state is not allowed
	and leads to undefined behavior.

	A platform implementation does not have to enforce the usage rules noted
	above. This is because it may not be possible in all cases, and the platform
	may want to actually allow this behavior in platform-specific (non-Magnum) code.

See Also:
	BKNI_AcquireMutex, BKNI_TryAcquireMutex, Magnum ThreadSafe rules
****************************************************************************/
void BKNI_ReleaseMutex(BKNI_MutexHandle mutex);

#ifdef __cplusplus
}
#endif

#endif /* BKNI_MULTI_H__ */
