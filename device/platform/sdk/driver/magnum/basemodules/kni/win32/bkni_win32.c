/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bkni_win32.c $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 6/10/10 7:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/kni/win32/bkni_win32.c $
 * 
 * Hydra_Software_Devel/13   6/10/10 7:20p vsilyaev
 * SW7468-69: Fixed Win32/MINGW build
 * 
 * Hydra_Software_Devel/13   6/10/10 1:38p vsilyaev
 * SW7468-69: Fixed Win32/MINGW build
 * 
 * Hydra_Software_Devel/12   3/10/10 3:12p mward
 * SW7400-2712:  Add annotation to indicate to Coverity that BKNI_Fail()
 * is a "kill path".
 * 
 * Hydra_Software_Devel/11   4/30/04 3:32p hongtaoz
 * PR8761: fixed C++ compile error;
 * 
 * Hydra_Software_Devel/10   9/5/03 11:16a jasonh
 * Warning police.
 * 
 * Hydra_Software_Devel/9   4/8/03 6:02p vsilyaev
 * Modified according to latest spec.
 * 
 * Hydra_Software_Devel/8   4/4/03 6:36p vsilyaev
 * Updated to the latest KNI spec.
 * 
 * Hydra_Software_Devel/7   4/1/03 5:43p vsilyaev
 * Added function ResetEvent.
 * 
 * Hydra_Software_Devel/6   4/1/03 5:23p vsilyaev
 * Change timeout type in Sleep and Delay functions from from int to
 * unsigned.
 * 
 * Hydra_Software_Devel/5   3/25/03 8:24p vsilyaev
 * Removed task API.
 * 
 * Hydra_Software_Devel/4   3/11/03 9:54p vsilyaev
 * Use handles instead objects
 * 
 * Hydra_Software_Devel/3   3/10/03 8:28p vsilyaev
 * Added support for the tagged kernet interface API.
 * 
 * Hydra_Software_Devel/2   3/10/03 2:44p vsilyaev
 * More complete implementation.
 * 
 * Hydra_Software_Devel/1   3/10/03 9:56a vsilyaev
 * Win32 implementation of the kernel interface.
 *
 ***************************************************************************/


#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

/* define following to get TryEnterCriticalSection */
#define _WIN32_WINNT    0x400 
#include <windows.h>
#include <stdio.h>

BDBG_MODULE(kernelinterface);

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
#undef BKNI_AcquireMutex
#undef BKNI_ReleaseMutex
#undef BKNI_CreateTask
#undef BKNI_DestroyTask

#define ASSERT_NOT_CRITICAL()
#define B_TRACK_ALLOC_LOCK() 
#define B_TRACK_ALLOC_UNLOCK() 
#define B_TRACK_ALLOC_ALLOC(size) malloc(size)
#define B_TRACK_ALLOC_FREE(ptr) free(ptr)
#define B_TRACK_ALLOC_OS "generic"

#include "bkni_track_mallocs.inc"

struct BKNI_EventObj
{
    HANDLE hEvent;
};

struct BKNI_MutexObj
{
    CRITICAL_SECTION mutex;
};


struct {
    CRITICAL_SECTION global_cs;
    LARGE_INTEGER sys_freq;
    bool use_PerfomanceCounter;
} g_kni;


BERR_Code 
BKNI_Init(void)
{
    InitializeCriticalSection(&g_kni.global_cs);
    g_kni.use_PerfomanceCounter = (bool)QueryPerformanceFrequency(&g_kni.sys_freq);
    BKNI_P_TrackAlloc_Init();
    return  BERR_SUCCESS;
}

void 
BKNI_Uninit(void)
{
    BKNI_P_TrackAlloc_Uninit();
    DeleteCriticalSection(&g_kni.global_cs);
    return;
}



BERR_Code 
BKNI_CreateMutex(BKNI_MutexHandle *pMutex)
{
    BKNI_MutexHandle mutex;

    mutex = (BKNI_MutexHandle) malloc(sizeof(*mutex));
    *pMutex = mutex;
    if (!mutex) {
        return BERR_TRACE(BERR_OS_ERROR);
    }   
    InitializeCriticalSection(&mutex->mutex);
    return BERR_SUCCESS;
}

void
BKNI_DestroyMutex(BKNI_MutexHandle mutex)
{
    DeleteCriticalSection(&mutex->mutex);
    free(mutex);
    return;
}

BERR_Code 
BKNI_AcquireMutex(BKNI_MutexHandle mutex)
{
    EnterCriticalSection(&mutex->mutex);
    return BERR_SUCCESS;
}

BERR_Code 
BKNI_TryAcquireMutex(BKNI_MutexHandle mutex)
{
    if (!TryEnterCriticalSection(&mutex->mutex)) {
       return BERR_TIMEOUT; /* no BERR_TRACE here */
    }
    return BERR_SUCCESS;
}

void
BKNI_ReleaseMutex(BKNI_MutexHandle mutex)
{
    LeaveCriticalSection(&mutex->mutex);
    return ;
}

/* coverity[+kill]  */
void 
BKNI_Fail(void)
{
    abort();
}


int 
BKNI_Printf(const char *fmt, ...)
{
    va_list arglist;
    int rc;

    va_start( arglist, fmt );
    rc = vfprintf(stderr, fmt, arglist);
    va_end(arglist);

    return rc;
}


int 
BKNI_Snprintf(char *str, size_t len, const char *fmt, ...)
{
    va_list arglist;
    int rc;

    va_start( arglist, fmt );
    rc = _vsnprintf(str, len, fmt, arglist);
    va_end(arglist);

    return rc;
}

int 
BKNI_Vprintf(const char *fmt, va_list ap)
{
    return vfprintf(stderr, fmt, ap);
}

void 
BKNI_Delay(unsigned int microsec)
{

    if (g_kni.use_PerfomanceCounter) {
    LARGE_INTEGER begin, cur, diff;
        
        diff.QuadPart = (microsec * g_kni.sys_freq.QuadPart)/(1000000);
        for (QueryPerformanceCounter(&begin);QueryPerformanceCounter(&cur) && (cur.QuadPart - begin.QuadPart)<diff.QuadPart;) { }
    } else {
      volatile long l;

      while(microsec--) {
          for(l=1000;l<0;l++) {}
      }
    }
    return;
}

BERR_Code 
BKNI_Sleep(unsigned int millisec)
{
    Sleep( millisec);
    return BERR_SUCCESS;
}


BERR_Code 
BKNI_CreateEvent(BKNI_EventHandle *pEvent)
{
    BKNI_EventHandle event;

    event = (BKNI_EventHandle) malloc(sizeof(*event));
    *pEvent = event;
    if (!event) {
        return BERR_TRACE(BERR_OS_ERROR);
    }
    event->hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (!event->hEvent) {
        free(event);
        return BERR_TRACE(BERR_OS_ERROR);
    }
    return BERR_SUCCESS;
}



void
BKNI_DestroyEvent(BKNI_EventHandle event)
{
    CloseHandle(event->hEvent);
    free(event);
    return;
}

BERR_Code 
BKNI_WaitForEvent(BKNI_EventHandle event, int timeoutMsec)
{
    DWORD winTimeout;
    DWORD rc;
    
    winTimeout = timeoutMsec==BKNI_INFINITE ? INFINITE : (DWORD)timeoutMsec;
    rc = WaitForSingleObject(event->hEvent, winTimeout);
    if (rc==WAIT_OBJECT_0) {
        return BERR_SUCCESS;
    } else if (rc==WAIT_TIMEOUT) {
        return BERR_TIMEOUT; /* don't use timeout BERR_TRACE here */
    } 
    return BERR_TRACE(BERR_OS_ERROR);
}

void
BKNI_SetEvent(BKNI_EventHandle event)
{
    if (!SetEvent(event->hEvent)) {
        BDBG_ERR(("SetEvent failed"));
        BDBG_ASSERT(false);
    }
    return;
}

void
BKNI_ResetEvent(BKNI_EventHandle event)
{
    if (!ResetEvent(event->hEvent)) {
        BDBG_ASSERT(false);
    }
    return;
}


void
BKNI_EnterCriticalSection(void)
{
    EnterCriticalSection(&g_kni.global_cs);
    return ;
}

void
BKNI_LeaveCriticalSection(void)
{
    LeaveCriticalSection(&g_kni.global_cs);
    return ;
}


int 
BKNI_Vsnprintf(char *s, size_t n, const char *fmt, va_list ap)
{
    return _vsnprintf(s, n, fmt, ap);   
}

void *
BKNI_Memset(void *b, int c, size_t len)
{
    return memset(b, c, len);
}

void *
BKNI_Memcpy(void *dst, const void *src, size_t len)
{
    return memcpy(dst, src, len);
}

int 
BKNI_Memcmp(const void *b1, const void *b2, size_t len)
{
    return memcmp(b1, b2, len);
}

void *
BKNI_Memchr(const void *b, int c, size_t len)
{
    return (void*)memchr(b, c, len);

}

void *
BKNI_Memmove(void *dst, const void *src, size_t len)
{
    return memmove(dst, src, len);
}


