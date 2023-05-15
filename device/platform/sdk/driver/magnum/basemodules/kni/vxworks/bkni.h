
/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni.h $
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 2/24/09 7:18p $
 * $brcm_Revision: Hydra_Software_Devel/15 $ *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/kni/vxworks/bkni.h $
 * 
 * Hydra_Software_Devel/15   2/24/09 7:18p rjlewis
 * PR32280: Added support for BKNI_ASSERT_ISR_CONTEXT.
 * 
 * Hydra_Software_Devel/12   2/11/05 6:44p dlwin
 * PR 14025: Removed the option to use intMask() for CriticalSection
 * implemenetation.
 * 
 * Hydra_Software_Devel/11   11/11/04 9:57a dlwin
 * PR 9698: Created _isr aliases for ISR safe functions.
 * 
 * Hydra_Software_Devel/10   7/26/04 7:33p liangl
 * PR11643: merging brutus vxworks support
 * 
 * Hydra_Software_Devel/9   2/20/04 4:36p dlwin
 * Hydra_Software_Devel/Brutus_Vxworks_PreRelease/1   6/17/04 1:18p liangl
 * compiled for vxWorks
 * 
 * macros for Interrupt and Task context.
 * 
 * Hydra_Software_Devel/8   12/9/03 5:45p dlwin
 * PR 8922: Fixed problem with building without BKNI_USE_TAGGED_API.
 * 
 * Hydra_Software_Devel/7   8/28/03 10:46a dlwin
 * Added 'stdlib.h' to the standard includes, this resolves the warnings
 * for 'malloc' and 'free'.
 * 
 * Hydra_Software_Devel/6   4/17/03 1:38p dlwin
 * Update new spec.
 * 
 * Hydra_Software_Devel/5   4/4/03 9:50a dlwin
 * Update API to match latest spec.
 * 
 * Hydra_Software_Devel/4   3/19/03 8:41p dlwin
 * Added more tagged functions, plus added TAG macros.
 * 
 * Hydra_Software_Devel/3   3/19/03 11:29a dlwin
 * Added BKNI_Snprintf() and BKNI_Vsnprintf() prototype.
 * 
 * Hydra_Software_Devel/2   3/18/03 6:05p dlwin
 * Fixed compiler errors, fixed run-time error, ru
 * 
 * Hydra_Software_Devel/1   3/14/03 2:54p dlwin
 * Initial versions
 * 
 ***************************************************************************/
#ifndef BKNI_H__
#define BKNI_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*====================== Module Overview ===========================

---++++ *Summary:*

The Kernel Interface is the set of operating system and standard C library functions
required by the Magnum architecture which is portable across all software
and hardware platforms which Broadcom uses.


---++++ *Requirements:*

*Portable* - every function must work on every platform in the same well-defined way.
Some error conditions cannot or should not be standardized, therefore we also include
usage rules (see BKNI_EnterCriticalSection for an example.)
Every platform implementationmust be regularly checked with compliance tests.

*Safe* - Because of the inherent dangers of multitasking,
each kernel interface implementation must ensure there are no race conditions within the
implementation.
The API must also be simple and explicit in order to help developers avoid race
conditions in their code.

*Minimal* - The number of functions should be just enough to allow us to implement the Magnum
architecture. Platform-dependent code should make use of platform-specific functions.


---++++ *Files:*

The kernel interface is divided into three sections:

*bkni.h* is the single-task kernel interface. It is used by code which follows the single-task
model of execution specified in ThreadSafe.

*bkni_multi.h* is the multi-tasking kernel interface. It is used by code which follows 
the multi-task model of execution specified in ThreadSafe. bkni_multi.h also includes
bkni.h.

*bkni_metrics.h* is the metrics interface. It must not be called by Magnum code, but
provides system monitoring and debugging to platform-specific code.


<nopublish>
---++++ *Summary of Changes:*

New naming convention.

Replaced structures with opaque handles.

Init and Uninit functions.

Replaced semaphores with mutexes.While a semaphore can be configured as a mutex, it can
easily be misconfigured and introduce race conditions. The combination of a mutex and an event
is simple and sufficient.

Removed sprintf and vsprintf because they introduce possible buffer overruns. snprintf
and vsnprintf are sufficient.

Memory map functionality (MmuMap and UnmmuMap) was moved to the MemoryManager.

Removed debug macros (DBGMSG) and data types (WORD, HANDLE, etc.).
</nopublish>


---++++ *Interrupt Safety:*

The following functions can be called from an ISR or a critical section:

	* BKNI_Memset, BKNI_Memcpy, BKNI_Memcmp, BKNI_Memchr, BKNI_Memmove
	* BKNI_Printf, BKNI_Vprintf
	* BKNI_Delay
	* BKNI_SetEvent
	* BKNI_WaitForEvent, but only with a timeout of zero.

All other functions are not callable from either an ISR or critical section.
*****************************************************************************/

/***************************************************************************
Summary:
	Event handle created by BKNI_CreateEvent.
****************************************************************************/
typedef struct BKNI_EventObj *BKNI_EventHandle;

/***************************************************************************
Summary:
	Initialize the kernel interface before use.
	
Description:
	The system must call BKNI_Init() before making any other kernel interface call.
	Magnum code cannot call BKNI_Init().
	
	The BKNI_Init call should reset the metrics interface. See BKNI_Metrics_Reset.

Returns:
	BERR_SUCCESS - The kernel interface successfully initialized.
	BERR_OS_ERROR - Initialization failed.
****************************************************************************/
BERR_Code BKNI_Init(void);


/***************************************************************************
Summary:
	Uninitialize the kernel interface after use.

Description:
	Cleans up the kernel interface. No kernel interface calls can be made after this,
	except BKNI_Init().
	Magnum code cannot call BKNI_Uninit().

Returns:
	<none>
****************************************************************************/
void BKNI_Uninit(void);


/***************************************************************************
Summary:
	Set byte array to a value.

Description:
	Copies the value of ch (converted to an unsigned char) into each of the first n
	characters of the memory pointed to by mem.

	Can be called from an interrupt-context.

Input:
	mem - memory to be set
	ch - 8 bit value to be copied into memory
	n - number of bytes to be copied into memory

Returns:
 	The value of mem
****************************************************************************/
/* void *BKNI_Memset(void *mem, int ch, size_t n); */
#define BKNI_Memset 			memset


/***************************************************************************
Summary:
	Copy non-overlapping memory.

Description:
	Copies n characters from the object pointed to by src into the object pointed
	to by dest. 
	
	If copying takes place between objects that overlap, the
	behavior is undefined. Use BKNI_Memmove instead.

	Can be called from an interrupt-context.

Input:
	dest - the destination byte array
	src - the source byte array
	n - number of bytes to copy

Returns:
	The value of dest
****************************************************************************/
/* void *BKNI_Memcpy(void *dest, const void *src, size_t n); */
#define BKNI_Memcpy					memcpy


/***************************************************************************
Summary:
	Compare two blocks of memory.
	
Description:
	Compares the first n characters of the object pointed to by s1 to the first n 
	characters of the object pointed to by s2.
	
	Can be called from an interrupt-context.

Input:
	s1 - byte array to be compared
	s2 - byte array to be compared
	n -	maximum number of bytes to be compared 

Returns:
	An integer greater than, equal to, or less than zero, accordingly as the object 
	pointed to by s1 is greater than, equal to, or less than the object pointed to by s2.
****************************************************************************/
/* int BKNI_Memcmp(const void *s1, const void *s2, size_t n); */
#define BKNI_Memcmp				 	memcmp


/***************************************************************************
Summary:
	Find a byte in a block of memory.

Description:
	Locates the first occurrence of ch (converted to an unsigned char) in the initial n
	characters (each interpreted as unsigned char) of the object pointed to by mem.

	Can be called from an interrupt-context.

Input:
	mem - byte array to be searched
	ch - 8 bit value to be searched for
	n - maximum number of bytes to be searched

Returns:
	A pointer to the located character, or a null pointer if the character does not
	occur in the object.
****************************************************************************/
/* void *BKNI_Memchr(const void *mem, int ch, size_t n); */
#define BKNI_Memchr			 		memchr


/***************************************************************************
Summary:
	Copy potentially overlapping memory.
	
Description:
	Copies n characters from the object pointed to by src into the object pointed
	to by dest. Copying takes place as if the n characters from the object pointed
	to by src are first copied into a temporary array of n characters that does
	not overlap the objects pointed to by dest and src, and then the n characters
	from the temporary array are copied into the object pointed to by dest.

	If the memory does not overlap, BKNI_Memcpy is preferred.

	Can be called from an interrupt-context.

Input:
	dest - destination memory
	src - source memory
	n - number of bytes to copy

Returns:
	The value of dest
****************************************************************************/
/* void *BKNI_Memmove(void *dest, const void *src, size_t n); */
#define BKNI_Memmove			 	memmove



/***************************************************************************
Summary:
	Print characters to the console.

Description:
	Although printing to the console is very important for development, it cannot
	and should not be guaranteed to actually print in all contexts.
	It is valid for the system developer to eliminate all BKNI_Printf output in
	release builds or if the context demands it (e.g. interrupt context).

	You should use BKNI_Printf instead of
	DebugInterface when you explicity want to print information to a console
	regardless of debug state (e.g. BXPT_PrintStatus, BPSI_PrintPsiInformation).
	BKNI_Printf is also used by the generic DebugInterface implementation.

	We only guarantee a subset of ANSI C format specifiers. These include:

	* %d  - int in decimal form
	* %u  - unsigned int in decimal form
	* %ld - long in decimal form
	* %lu - unsigned long in decimal form
	* %x  - unsigned int in lowercase hex form
	* %lx - unsigned long in lowercase hex form
	* %X  - unsigned int in uppercase hex form
	* %lX - unsigned long in uppercase hex form
	* %c  - an int argument converted to unsigned char
	* %s  - string
	* \n  - newline
	* \t  - tab
	* %%  - % character
	* %03d - Zero padding of integers, where '3' and 'd' are only examples. This can be applied to any of the preceding numeric format specifiers (not %c or %s).
	* Pass-through of non-control characters.

	Beyond these, we do not guarantee the output format. 
	
	For BKNI_Printf and BKNI_Vprintf, other ANSI C format specifiers
	may be used, and platforms should try to make sure that any combination of formats
	and parameters will not cause a system crash.
	
	When calling BKNI_Snprint and BKNI_Vsnprintf, Magnum code must only use the
	guaranteed format specifiers if the results must always be the same on all platforms.

	BKNI_Printf can be called from an interrupt-context.

Input:
	fmt - See BKNI_Printf
	va_list - See StandardTypes and stdarg.h

Returns:
 	>=0 is success. It is the number of characters transmitted.
	<0 is failure, either in encoding or in outputing.
****************************************************************************/
/* int BKNI_Printf(const char *fmt, ...); */
#define BKNI_Printf		 		printf


/***************************************************************************
Summary:
	Print characters to a null-terminated string.

Description:
	See BKNI_Printf for a description of the format specifiers supported.

	Can be called from an interrupt-context.

Returns:
	If the output is not truncated, it returns the number of characters printed, not
	including the trailing null byte.

	If the output is truncated, it should try to return the number
	of characters that would have been printed had the size of memory been large
	enough. However, this result is not required and no Magnum code should
	depend on this result.
****************************************************************************/
int BKNI_Snprintf(
	char *s, 			/* destination string */
	size_t n,			/* size of memory that can be used. It should include
							space for the trailing null byte. */
	const char *fmt, 	/* format string */
	...					/* variable arguments */
	);


/***************************************************************************
Summary:
	Print characters to the console using a variable argument list.

Description:
	Equivalent to BKNI_Printf, with the variable argument list replaced by the va_list
	parameter. va_list must initialized by the va_start macro (and possibly
	subsequent va_arg calls). The BKNI_Vprintf function does not invoke the va_end macro.

	The value of the va_list parameter may be modified and so it is indeterminate upon return.
	
	See BKNI_Printf for a description of the format specifiers supported.

	Can be called from an interrupt-context.

Input:
	fmt - See BKNI_Printf
	va_list - See StandardTypes and stdarg.h

Returns:
 	>=0 is success. It is the number of characters transmitted.
	<0 is failure, either in encoding or in outputing.
****************************************************************************/
/* int BKNI_Vprintf(const char *fmt, va_list ap); */
#define BKNI_Vprintf		 		vprintf


/***************************************************************************
Summary:
	Print characters to a null-terminated string using a variable argument list.

Description:
	See BKNI_Printf for a description of the format specifiers supported.
	See BKNI_Vprintf for a description of the va_list parameter.

	Can be called from an interrupt-context.

Input:
	s - memory to print into
	n - size of memory that can be used. It should include space for the trailing null byte.
	fmt - See BKNI_Printf
	va_list - See StandardTypes and stdarg.h

Returns:
	If the output is not truncated, it returns the number of characters printed, not
	including the trailing null byte.

	If the output is truncated, it should try to return the number
	of characters that would have been printed had the size of memory been large
	enough. However, this result is not required and no Magnum code should
	depend on this result.
****************************************************************************/
int BKNI_Vsnprintf(char *s, size_t n, const char *fmt, va_list ap);


/***************************************************************************
Summary:
	Busy sleep.
	
Description:
	BKNI_Delay is a busy sleep which guarantees you will delay for at least the specified 
	number of microseconds. It does not call the scheduler, therefore the Delay is able to be 
	less than the system clock time. This consumes CPU time, so it should be used for only 
	short sleeps and only when BKNI_Sleep cannot be used.
	
	Be aware that on a preemptive system, any task can be interrupted and the scheduler can
	run, and so there is no guarantee of maximum delay time. If you have maximum time
	constraints, you should be using an interrupt.
	
	Can be called from an interrupt-context.

Input:
	microsec - minimum number of microseconds to delay

Returns:
	<none>
****************************************************************************/
void BKNI_Delay(unsigned int microsec);


/***************************************************************************
Summary:
	Allocate system memory.

Description:
	Allocates space for an object whose size is specified by size and whose
	value is indeterminate.

	System memory is usually managed by an operating system. It differs
	from memory managed by a MemoryManager in that it is not
	guaranteed to be physically continuous and you cannot request alignment.

	Passing a size of 0 is not allowed and leads to undefined behavior.

	The caller is responsible to also call BKNI_Free to free the memory
	when done. Memory that is not explicitly freed
	may or may not remain allocated beyond the life-cycle of a particular
	application.

Input:
	size - Number of bytes to allocate

Returns:
	NULL - Memory could not be allocated
	Non-NULL - Memory was allocated
****************************************************************************/
/* void *BKNI_Malloc(size_t size); */
#define BKNI_Malloc				malloc


/***************************************************************************
Summary:
	Dellocate system memory.

Description:
	Causes the memory pointed to by mem to be deallocated, that is, made available for
	further allocation.

	The following scenarios are not allowed and lead to undefined behavior:
	
	- Passing pointer which was not returned by earlier BKNI_Malloc call
	- Passing pointer which was already freed
	- Passing NULL
	* Passing a pointer which was not returned by an earlier BKNI_Malloc call
	* Passing a pointer which was already freed
	* Passing NULL

Input:
	mem	- Pointer to memory allocated by BKNI_Malloc

Returns:
 	<none>
****************************************************************************/
/* void BKNI_Free(void *mem); */
#define BKNI_Free 				free


/***************************************************************************
Summary:
	Yield the current task to the scheduler.

Description:
	BKNI_Sleep is a sheduler sleep which guarantees you will delay for at least the 
	specified number of milliseconds. It puts the process to sleep and allows the scheduler 
	to run. The minimum sleep time is dependent on the system clock time. If you need 
	a minimum time which is less that the system clock time, you'll need to use BKNI_Delay.
	
	Actual sleep time is dependent on the scheduler but will be at least as long as
	the value specified by the millisec parameter.
	
	A sleep value of 0 should cause the scheduler to execute. This may or may not result in
	any delay.

	BKNI_Sleep cannot be called from an interrupt context. Use BKNI_Delay instead.

Input:
	millisec - The minimum number of milliseconds to sleep

Returns:
 	BERR_SUCCESS - The system slept for at least the specified number of milliseconds.
 	BERR_OS_ERROR - The sleep was interrupted before the specified time.
****************************************************************************/
BERR_Code BKNI_Sleep(unsigned int millisec);


/***************************************************************************
Summary:
	Create an event used by one task to signal another task.

Description:
	Note that there is no 'name' parameter in BKNI_CreateEvent. We do not support named
	events because they are essentially global variables that can lead to unwanted behavior.
	Passing in a name for debugging purposes might lead to someone to think we
	support named events.

	The event is created in an unsignalled state.

Input:
	event - point to an event handle which will be initialized.

Returns:
 	BERR_SUCCESS - The event is allocated and initialized.
 	BERR_OS_ERROR - The event could not be allocated or initialized.
****************************************************************************/
BERR_Code BKNI_CreateEvent(BKNI_EventHandle *event);


/***************************************************************************
Summary:
	Destroy an event.
	
Description:
	If any signal is pending, it is lost.

Input:
	event - event initialized by BKNI_CreateEvent
****************************************************************************/
void BKNI_DestroyEvent(BKNI_EventHandle event);


/***************************************************************************
Summary:
	Wait until an event is signalled by BKNI_SetEvent.

Description:
	Suspends execution of the current task until the event specified by the event parameter
	becomes signalled. The task should be blocked by the scheduler, resulting in no wasted
	CPU time.

	This function fails if the event does not become signalled within
	the period specified by the timeoutMsec parameter. A timeout of zero instructs this
	function to fail immediately if the event is not signalled. A timeout value of
	BKNI_INFINTE (defined in bkni_multi.h) causes this function not to return until successful.

	BKNI_WaitForEvent can be called from a single-task module only if that module follows
	the rules specified under PortingInterface interrupt handling.
	For this reason, BKNI_INFINITE is defined in bkni_multi.h. An event
	that blocks for a less-than-infinite time can be considered as an interruptible
	sleep. See PortingInterface for a description of interrupt handling in a single-threaded
	module.

	A single BKNI_WaitForEvent call will consume all pending signals.

	However, if multiple tasks call BKNI_WaitForEvent for the same event, it is only
	guaranteed that at least one will be woken up with the next BKNI_SetEvent call. Some platforms
	may wake up one, others may wake up all. To avoid problems, Magnum code
	should only wait on a particular event from a single task.

	BKNI_WaitForEvent can be called from an interrupt context, but only if timeoutMsec
	is 0. In this case, it checks if the event has been set, and does not block if it
	has not. Calling BKNI_WaitForEvent from an interrupt context with a non-zero timeout
	leads to undefined behavior.
 
Returns:
	BERR_SUCCESS - An event happened and was consumed.
	BERR_TIMEOUT - Either the timeout was 0 and no event was already pending, or the timeout expired before an event happened.
	BERR_OS_ERROR - The system interruped the wait and an event did not happen.
****************************************************************************/
BERR_Code BKNI_WaitForEvent(BKNI_EventHandle event, int timeoutMsec);


/***************************************************************************
Summary:
	Signal an event which causes BKNI_WaitForEvent to return.

Description:
	Causes the event specified by the event parameter to become signalled. At least one
	task waiting on the event becomes unblocked.

	If no task is waiting on this event, the signal is remembered and the next call
	to BKNI_WaitForEvent will succeed immediately.
	
	BKNI_SetEvent may be called from an interrupt context.
	
See Also:
	See BKNI_WaitForEvent for more details and usage rules.
****************************************************************************/
void BKNI_SetEvent(BKNI_EventHandle event);


/***************************************************************************
Summary:
Reset an event so that there are no signals pending.

Description:
In certain cases it is necessary to reset an event in order to avoid a race condition.
Consider the following code:

<verbatim>
	foo() {
		SetHardware();
		err = BKNI_WaitForEvent(event, timeout);
		if (err)
			return err;
		else
			return AcquireData();
	}

	interrupt() {
	 BKNI_SetEvent(event);
	}
</verbatim>

And now consider the following execution sequence:

<verbatim>
	foo() called
		setHardware() called
		BKNI_WaitForEvent() called
		BKNI_WaitForEvent times out and foo returns an error.

	Interrupt happens and BKNI_SetEvent called

	foo() called again
		setHardware() called
		BKNI_WaitForEvent() called
		--> BKNI_WaitForEvent returns BERR_SUCCESS from the old interrupt
		AcquireData() is called before it should be.
</verbatim>

In order to avoid this problem, BKNI_ResetEvent must be called before setting or
resetting hardware. Be careful that there is not a race condition between
BKNI_ResetEvent and the actual resetting of hardware. If you cannot disable
the particular interrupt, you need to use a critical section.

<verbatim>
	foo() {
		BKNI_EnterCriticalSection();
		BKNI_ResetEvent(event);
		SetHardware();
		BKNI_LeaveCriticalSection();
		err = BKNI_WaitForEvent(event, timeout);
		if (err)
			return err;
			return AcquireData();
	}
</verbatim>

A simple implementation of BKNI_ResetEvent is to call BKNI_WaitForEvent with a timeout
of 0. For some platforms, there may be a more optimal implementation.

Returns:
	<none>
****************************************************************************/
void BKNI_ResetEvent(BKNI_EventHandle event);


/***************************************************************************
Summary:
	Cause the application or system to abort immediately if possible.
	On platforms that support it, the system state should be captured.

Description:
	This is called from a failed BDBG_ASSERT() (see DebugInterface).

	Can be called from an interrupt-context.

	There is no required behavior for this function. It can be completely
	empty. There is no need to print an error message from inside BKNI_Fail
	because the DebugInterface will have printed something before calling
	it.

See Also:
	DebugInterface, BDBG_ASSERT
****************************************************************************/
void BKNI_Fail(void);


/***************************************************************************
Summary:
	Create a critical section which protects against concurrent execution by
	another critical section or ISR (interrupt service routine).

Description:
	A "critical section" is defined as a block of code between a BKNI_EnterCriticalSection
	call and a BKNI_LeaveCriticalSection call or any code inside an ISR.

	Critical sections cannot be preempted by other critical sections. They protect against
	both interrupts and context switching. For interrupt-context systems, critical sections
	must be implemented by disabling interrupts. For task-context only systems, critical
	sections must be implemented with a global mutex.

	Be aware that on task-context only systems, critical sections may not prevent the scheduler
	from time-slicing and executing non-critical section code concurrently with the
	critical section.

	Critical sections are also used to protect concurrent access to a register shared
	with another module. Ideally, there are no registers shared between disconnected
	software modules, but sometimes this is unavoidable.

	Magnum code cannot nest critical sections. Calling BKNI_EnterCriticalSection from
	inside a critical section is not allowed and leads to undefined behavior.
	Possible results include deadlock or undetected race conditions.

	A platform implementation might chose to allow BKNI_EnterCriticalSection to nest
	because of platform-specific considerations.

See Also:
	BKNI_LeaveCriticalSection, Magnum InterruptSafe rules
****************************************************************************/
void BKNI_EnterCriticalSection(void);


/***************************************************************************
Summary:
	Leave a critical section.

Description:
	Calling BKNI_LeaveCriticalSection when you are not in a critical section
	is not allowed and leads to undefined behavior.

See Also:
	BKNI_EnterCriticalSection, Magnum InterruptSafe rules
****************************************************************************/
void BKNI_LeaveCriticalSection(void);

/***************************************************************************
Summary:
	Verifies a critical section.

Description:
    Assert that code is running in interrupt context, either from an ISR 
    or inside critical section 

See Also:
	BKNI_EnterCriticalSection, Magnum InterruptSafe rules
****************************************************************************/
void BKNI_AssertIsrContext(void);

#ifdef BDBG_DEBUG_BUILD
#define BKNI_ASSERT_ISR_CONTEXT() BKNI_AssertIsrContext()
#else
#define BKNI_ASSERT_ISR_CONTEXT() (void)0
#endif

#if defined(VXWORKS) && (BINT_NO_TASK_ISR==1)
#error This mode of handling interrupts is no longer supportted. Please use Task context to service interrupts.
#endif /* defined(VXWORKS) && (BINT_NO_TASK_ISR==1) */

#if 0
#define BKNI_USE_TAGGED_API 1
#else
#define BKNI_USE_TAGGED_API 0
#endif

#if BKNI_USE_TAGGED_API

void BKNI_DelayTagged(unsigned int microsec, const char *filename, unsigned lineno);
BERR_Code BKNI_SleepTagged(unsigned int millisec, const char *filename, unsigned lineno);
BERR_Code BKNI_CreateEventTagged(BKNI_EventHandle *event, const char *filename, unsigned lineno);
void BKNI_DestroyEventTagged(BKNI_EventHandle event, const char *filename, unsigned lineno);
BERR_Code BKNI_WaitForEventTagged(BKNI_EventHandle event, int timeoutMsec, const char *filename, unsigned lineno);
void BKNI_SetEventTagged(BKNI_EventHandle event, const char *filename, unsigned lineno);
void BKNI_EnterCriticalSectionTagged(const char *filename, unsigned lineno);
void BKNI_LeaveCriticalSectionTagged(const char *filename, unsigned lineno);
void BKNI_AssertIsrContextTagged(const char *filename, unsigned lineno);

#define BKNI_Delay(microsec) BKNI_DelayTagged(microsec, __FILE__, __LINE__)
#define BKNI_Sleep(millisec) BKNI_SleepTagged(millisec, __FILE__, __LINE__)
#define BKNI_CreateEvent(event) BKNI_CreateEventTagged(event, __FILE__, __LINE__)
#define BKNI_DestroyEvent(event) BKNI_DestroyEventTagged(event, __FILE__, __LINE__)
#define BKNI_WaitForEvent(event, timeoutMsec) BKNI_WaitForEventTagged(event, timeoutMsec, __FILE__, __LINE__)
#define BKNI_SetEvent(event)  BKNI_SetEventTagged(event, __FILE__, __LINE__) 
#define BKNI_EnterCriticalSection() BKNI_EnterCriticalSectionTagged(__FILE__, __LINE__)
#define BKNI_LeaveCriticalSection() BKNI_LeaveCriticalSectionTagged(__FILE__, __LINE__)
#define BKNI_AssertIsrContext() BKNI_AssertIsrContextTagged(__FILE__, __LINE__)

#endif

/* lines below provides aliases for functions safe to call from the interrupt handler */
#define BKNI_Memset_isr BKNI_Memset
#define BKNI_Memcpy_isr BKNI_Memcpy
#define BKNI_Memcmp_isr BKNI_Memcmp
#define BKNI_Memchr_isr BKNI_Memchr
#define BKNI_Memmove_isr BKNI_Memmove
#define BKNI_Delay_isr BKNI_Delay
#define BKNI_SetEvent_isr BKNI_SetEvent


#ifdef __cplusplus
}
#endif

#endif /* BKNI_H__ */
