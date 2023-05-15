/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni.h $
 * $brcm_Revision: Hydra_Software_Devel/26 $
 * $brcm_Date: 10/18/12 9:20a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/kni/linuxkernel/bkni.h $
 * 
 * Hydra_Software_Devel/26   10/18/12 9:20a erickson
 * SW7420-1817: default BKNI_DEBUG_MUTEX_TRACKING off until FE init issues
 * resolved
 * 
 * Hydra_Software_Devel/25   10/4/12 10:39a erickson
 * SW7420-1817: fix redefinition
 * 
 * Hydra_Software_Devel/24   10/3/12 2:43p vsilyaev
 * SW7420-1817: Enable mutex tracking/deadlock detection
 * 
 * Hydra_Software_Devel/23   9/18/12 1:03p erickson
 * SW7435-281: document debug features
 * 
 * Hydra_Software_Devel/22   8/3/11 5:50p vsilyaev
 * SW7405-2178: Enable by defaul malloc tracking in linux kernel mode
 * 
 * Hydra_Software_Devel/21   5/3/11 1:41p erickson
 * SW7420-1819: remove copy to/from user from linuxkernel KNI
 * 
 * Hydra_Software_Devel/20   8/12/10 3:53p ttrammel
 * SW7420-943: Merge NFE to main branch.
 * 
 * Hydra_Software_Devel/20   8/12/10 2:11p ttrammel
 * SW7420-943: Merge NFE to main branch.
 * 
 * Hydra_Software_Devel/NFE/1   5/11/10 11:58a ttrammel
 * SW7405-4315: Initial NFE check-in.
 * 
 * Hydra_Software_Devel/19   4/13/09 3:50p jtna
 * PR53480: PR53480: update
 * PR53480: BKNI_TRACK_MALLOCS logic for linuxkernel
 * 
 * Hydra_Software_Devel/18   4/8/09 4:23p vsilyaev
 * PR 54015: Don't include bkni_multi.h into the bdbg.h. All thread-aware
 * modules should include explicitly bkni_multi.h
 * 
 * Hydra_Software_Devel/17   4/2/09 11:29a erickson
 * PR53778: extend BKNI_TRACK_MALLOCS to events and mutexes
 *
 * Hydra_Software_Devel/16   1/30/09 9:17p vsilyaev
 * PR 42495: Using matching types for the line number
 *
 * Hydra_Software_Devel/15   1/30/09 10:53a maivu
 * PR  51737: Fix conflicts with function prototypes
 *
 * Hydra_Software_Devel/14   12/29/08 1:01p erickson
 * PR50742: fix spelling of BKNI_AssertIsrContext
 *
 * Hydra_Software_Devel/13   8/21/08 4:43p vsilyaev
 * PR 32280: Added BKNI_ASSERT_ISR_CONTEXT
 *
 * Hydra_Software_Devel/12   1/10/07 3:44p erickson
 * PR25902: move linux kernel header files into .c
 *
 * Hydra_Software_Devel/11   10/11/06 3:01p jgarrett
 * PR 24364: Merging to main branch
 *
 * Hydra_Software_Devel/PR24364/2   10/10/06 4:27p jgarrett
 * PR 24364: Adding BKNI_LinuxKernel_SetIsrTasklet()
 *
 * Hydra_Software_Devel/PR24364/1   10/5/06 3:08p jgarrett
 * PR 24364: Revamping kni critical sections to use spin_lock_bh by
 * default and optionally tasklet_disable or spin_lock_irqsave.
 *
 * Hydra_Software_Devel/10   11/9/04 8:40p dlwin
 * PR 9698: Created _isr aliases for ISR safe functions.
 *
 * Hydra_Software_Devel/9   4/8/03 4:47p erickson
 * BKNI_ResetEvent now returns void
 * Split BKNI_AcquireMutex into Acquire and TryAcquire
 *
 * Hydra_Software_Devel/8   4/3/03 6:13p erickson
 * some pre 0.9 api rework
 *
 * Hydra_Software_Devel/7   4/3/03 3:57p erickson
 * linuxkernel work
 *
 * Hydra_Software_Devel/6   3/31/03 3:40p erickson
 * added BKNI_ResetEvent
 *
 * Hydra_Software_Devel/5   3/11/03 6:59p erickson
 * changed kernelinterface from using object ptrs to handles
 *
 * Hydra_Software_Devel/4   3/10/03 4:55p erickson
 * using standard bkni.h
 * no longer includes STD_INC include path
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
#ifndef BKNI_H__
#define BKNI_H__

/* No Linux include files are permitted here. Magnum code cannot depend on Linux
kernel header files. The only place where Linux kernel header files are allowed is inside
BKNI .c files. */

typedef struct BKNI_EventObj *BKNI_EventHandle;
#define BKNI_INFINITE -1

BERR_Code BKNI_Init(void);
void BKNI_Uninit(void);
void BKNI_Fail(void);

void *BKNI_Memset(void *mem, int ch, size_t n);
void *BKNI_Memcpy(void *dest, const void *src, size_t n);
int BKNI_Memcmp(const void *s1, const void *s2, size_t n);
void *BKNI_Memchr(const void *mem, int ch, size_t n);
void *BKNI_Memmove(void *dest, const void *src, size_t n);
int BKNI_Printf(const char *fmt, ...);
int BKNI_Snprintf(char *s, size_t n, const char *fmt, ...);
int BKNI_Vprintf(const char *fmt, va_list ap);
int BKNI_Vsnprintf(char *s, size_t n, const char *fmt, va_list ap);

/**
BKNI_TRACK_MALLOCS is a simple way to track BKNI_Malloc memory leaks and bad BKNI_Frees.
It can also help find the location of bad BKNI_EventHandle and BKNI_MutexHandle instances.
**/
#ifndef BKNI_TRACK_MALLOCS
#define BKNI_TRACK_MALLOCS 1
#endif

/**
BKNI_DEBUG_CS_TIMING can find critical sections which execute too long. This might happen because
of poorly written code or context switching on an overly busy system or a system with misconfigured
real-time threads.
**/
#define BKNI_DEBUG_CS_TIMING 0

/**
BKNI_DEBUG_MUTEX_TRACKING can find mutex objects thate were acquired for long time. Usually this condition
symptom and/or cause of a deadlock condition
**/
#ifndef BKNI_DEBUG_MUTEX_TRACKING
#define BKNI_DEBUG_MUTEX_TRACKING 0
#endif

#if BKNI_TRACK_MALLOCS
#define BKNI_Malloc(SIZE)			BKNI_Malloc_tagged(SIZE,__FILE__,__LINE__)
void *BKNI_Malloc_tagged(size_t size, const char *file, unsigned line);
#else
void *BKNI_Malloc(size_t size);
#endif


#if BKNI_TRACK_MALLOCS
#define BKNI_Free(MEM) 				BKNI_Free_tagged(MEM,__FILE__,__LINE__)
void BKNI_Free_tagged(void *mem, const char *file, unsigned line);
#else
void BKNI_Free(void *mem);
#endif


#define BKNI_Sleep(MILLISEC)		BKNI_Sleep_tagged(MILLISEC,__FILE__,__LINE__)
BERR_Code BKNI_Sleep_tagged(int millisec, const char *file, unsigned line);

#define BKNI_Delay(MICROSEC)		BKNI_Delay_tagged(MICROSEC,__FILE__,__LINE__)
void BKNI_Delay_tagged(int microsec, const char *file, unsigned line);

#if BKNI_TRACK_MALLOCS
#define BKNI_CreateEvent(EVENT) BKNI_CreateEvent_tagged(EVENT, __FILE__, __LINE__)
BERR_Code BKNI_CreateEvent_tagged(BKNI_EventHandle *p_event, const char *file, unsigned line);

#define BKNI_DestroyEvent(EVENT) BKNI_DestroyEvent_tagged(EVENT, __FILE__, __LINE__)
void BKNI_DestroyEvent_tagged(BKNI_EventHandle event, const char *file, unsigned line);
#else
BERR_Code BKNI_CreateEvent(BKNI_EventHandle *p_event);
void BKNI_DestroyEvent(BKNI_EventHandle event);
#endif

#define BKNI_SetEvent(EVENT)		BKNI_SetEvent_tagged(EVENT,__FILE__,__LINE__)
void BKNI_SetEvent_tagged(BKNI_EventHandle event, const char *file, unsigned line);

#define BKNI_WaitForEvent(EVENT,TIMEOUT)	BKNI_WaitForEvent_tagged(EVENT,TIMEOUT,__FILE__,__LINE__)
BERR_Code BKNI_WaitForEvent_tagged(BKNI_EventHandle event, int timeoutMsec, const char *file, unsigned line);

#define BKNI_ResetEvent(EVENT) BKNI_ResetEvent_tagged(EVENT, __FILE__, __LINE__)
void BKNI_ResetEvent_tagged(BKNI_EventHandle event, const char *file, unsigned line);

#define BKNI_EnterCriticalSection()	BKNI_EnterCriticalSection_tagged(__FILE__,__LINE__)
void BKNI_EnterCriticalSection_tagged(const char *file, unsigned line);

#define BKNI_LeaveCriticalSection()	BKNI_LeaveCriticalSection_tagged(__FILE__,__LINE__)
void BKNI_LeaveCriticalSection_tagged(const char *file, unsigned line);

/***************************************************************************
Summary:
	Provide a tasklet pointer to KNI to allow finer-grain locking for
	BKNI_EnterCriticalSection.

Description:
    This allows BKNI_EnterCriticalSection to only stop the specified tasklet
	from executing during critical sections and does not require masking
	interrupts or disabling all bottom-halves on the system.  This call is
	optional and specific only to the linuxkernel implementation.

	A (struct tasklet_struct *) is expected to be passed as a parameter
	to this function or NULL will cause KNI to revert to broader locking.

	This function does not need to be implemented or called unless your
	application desires this finer-grain locking.

Returns:
	BERR_SUCCESS - The tasklet handle has been changed.
	BERR_NOT_SUPPORTED - This feature is not supported.
****************************************************************************/
BERR_Code BKNI_LinuxKernel_SetIsrTasklet(void *pTasklet);


#ifdef BDBG_DEBUG_BUILD
#define BKNI_ASSERT_ISR_CONTEXT() BKNI_AssertIsrContext(__FILE__, __LINE__)
#else
#define BKNI_ASSERT_ISR_CONTEXT() (void)0
#endif
void BKNI_AssertIsrContext(const char *filename, unsigned lineno);


/* lines below provides aliases for functions safe to call from the interrupt handler */
#define BKNI_Memset_isr BKNI_Memset
#define BKNI_Memcpy_isr BKNI_Memcpy
#define BKNI_Memcmp_isr BKNI_Memcmp
#define BKNI_Memchr_isr BKNI_Memchr
#define BKNI_Memmove_isr BKNI_Memmove
#define BKNI_Delay_isr BKNI_Delay
#define BKNI_SetEvent_isr BKNI_SetEvent
#define BKNI_ResetEvent_isr BKNI_ResetEvent

typedef struct BKNI_MallocEntryInfo {
    const void *mem;

    size_t size;
    const char *malloc_file;
    unsigned malloc_line;
    bool alive; /* sets to true if block is still allocated */
    const char *free_file;
    unsigned free_line;
} BKNI_MallocEntryInfo;

/* Called from DBG, and should only be used for the debug purposes */
BERR_Code BKNI_GetMallocEntryInfo(
    const void *memory, /* pointer memory allocated by BKNI_Malloc */
    BKNI_MallocEntryInfo *info
    );

#define BKNI_GetTrackMallocEntry_isr BKNI_GetTrackMallocEntry

#endif /* BKNI_H__ */
