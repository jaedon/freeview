
#ifndef _MXKERNEL_H_
#define _MXKERNEL_H_


#include "mxapi_define.h"
#include "mxlib_config.h"

#ifdef __cplusplus
extern "C"
{
#endif




// 2013/12/11 mhkang:
// mxKernel_waitThreadEx return value
//#define MXTHREAD_WAIT_SUCCESS               0
//#define MXTHREAD_WAIT_TIMEOUT               1
//#define MXTHREAD_WAIT_INVALID_THREADID      2
//#define MXTHREAD_WAIT_NO_WAIT               3

// MXKERNEL THREAD STACK SIZE
#define MXTHREAD_MIN_STACK_SIZE				16384		// 최소 값 16384 입니다. 이하 값은 HMS에서 setting 이 안됩니다.
#define MXTHREAD_DEFAULT_STACK_SIZE			65536		// 다른 부분에서 사용하는 기본 설정 값
#define MXTHREAD_MAX_STACK_SIZE				4194304		// 8388608 반

// MXKERNEL THREAD DETACH STATE
#define MXTHREAD_DETACHSTATE_JOINABLE		0
#define MXTHREAD_DETACHSTATE_DETACHABLE		1

// MXKERNEL THREAD PRIORITY
#define MXTHREAD_DEFAULT_PRIORITY_CLASS		0
#define MXTHREAD_IDLE_PRIORITY_CLASS		99

#ifndef WIN32

// GSHAN 2009.09.07 ; use define 'USE_PTHREAD_MUTEX' on Mac environment
#ifdef IPHONE_OS
	#define USE_PTHREAD_MUTEX
#endif

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>

	#include <pthread.h>

	#define Sleep(a) usleep(1000*(a))
	unsigned long GetTickCount(void);

#ifdef USE_PTHREAD_MUTEX
	#define MXKERNEL_CRITICALSECTION						pthread_mutex_t
	#define	mxKernel_initializeCriticalSection(h)			pthread_mutex_init( &(h), 0 )
	#define mxKernel_deleteCriticalSection(h)				pthread_mutex_destroy(&(h))
	#define mxKernel_enterCriticalSection(h)				pthread_mutex_lock(&(h))
	#define mxKernel_leaveCriticalSection(h)				pthread_mutex_unlock(&(h))

	#define	MXKERNEL_SEMAPHORE_HANDLE						pthread_mutex_t
	#define mxKernel_createSemaphore(h)						pthread_mutex_init( &(h), 0 )
	#define mxKernel_closeHandle(h)							pthread_mutex_destroy(&(h))
	#define mxKernel_wait(h)								pthread_mutex_lock(&(h))
	#define mxKernel_post(h)								pthread_mutex_unlock(&(h))
#else
	#include <semaphore.h>

	#define MXKERNEL_CRITICALSECTION						sem_t
	#define	mxKernel_initializeCriticalSection(h)			sem_init( &(h), 0, 1 )
	#define mxKernel_deleteCriticalSection(h)				sem_destroy(&(h))
	#define mxKernel_enterCriticalSection(h)				sem_wait(&(h))
	#define mxKernel_leaveCriticalSection(h)				sem_post(&(h))

	#define	MXKERNEL_SEMAPHORE_HANDLE						sem_t
	#define mxKernel_createSemaphore(h)						sem_init( &(h), 0, 0 )
	#define mxKernel_closeHandle(h)							sem_destroy(&(h))
	#define mxKernel_wait(h)								sem_wait(&(h))
	#define mxKernel_post(h)								sem_post(&(h))
#endif

	typedef pthread_t THREAD_HANDLE;

	#define mxKernel_getPid									getpid
#ifdef _ANDROID
	#define mxKernel_getTid									gettid
#else
	#define mxKernel_getTid									pthread_self
#endif


#else


#if defined(WINSOCK2)
	#include <winsock2.h>
	#include <ws2tcpip.h>
#elif defined(WINSOCK1)
	#include <winsock.h>
	#include <wininet.h>
#endif

	#include "windows.h"
	#define MXKERNEL_CRITICALSECTION						CRITICAL_SECTION
	#define	mxKernel_initializeCriticalSection(a)			InitializeCriticalSection(&a)
	#define mxKernel_deleteCriticalSection(a)				DeleteCriticalSection(&a)
	#define mxKernel_enterCriticalSection(a)				EnterCriticalSection(&a)
	#define mxKernel_leaveCriticalSection(a)				LeaveCriticalSection(&a)

	#define	MXKERNEL_SEMAPHORE_HANDLE						HANDLE
	#define mxKernel_createSemaphore(h)						{ h = CreateSemaphore( NULL, 0, 1000, 0 ); }
	#define mxKernel_closeHandle(h)							CloseHandle((HANDLE) h)
	#define mxKernel_wait(h)								WaitForSingleObject( (HANDLE)h, INFINITE );
	#define mxKernel_wait_time(h, t)						WaitForSingleObject( (HANDLE)h, t);
	#define mxKernel_post(h)								ReleaseSemaphore((HANDLE)h, 1, NULL)

	typedef HANDLE THREAD_HANDLE;

	#define mxKernel_getPid()					GetCurrentProcessId()
	#define mxKernel_getTid()					GetCurrentThreadId()

#endif


typedef void *(*MX_THREAD_FUNCTION)( void * );

typedef struct _MX_THREAD_HANDLE {

	THREAD_HANDLE hThread;
	MXKERNEL_CRITICALSECTION cs;
	int priority;

	MX_THREAD_FUNCTION callback;
	void *data;
	int detached;
	int running;

} MX_THREAD_DATA, *MX_THREAD_HANDLE;


__MXAPI int mxKernel_setThreadPriority( MX_THREAD_HANDLE handle, int n );
__MXAPI MX_THREAD_HANDLE mxKernel_startThread( MX_THREAD_FUNCTION func, void *data, int detached);
__MXAPI MX_THREAD_HANDLE mxKernel_startThreadEx( MX_THREAD_FUNCTION func, void *data, int priority, int detached, int stack_size);
__MXAPI void mxKernel_waitThread( MX_THREAD_HANDLE thread);
//__MXAPI int mxKernel_waitThreadEx( MX_THREAD_HANDLE thread, int waitTime);

#ifdef __cplusplus
}
#endif


#endif // _MXKERNEL_H_



#if 0
	#ifdef __cplusplus
		#include <afxwin.h>
		#include <afxcmn.h>


	#define ICKERNEL_CRITICALSECTION						CRITICAL_SECTION
	#define	ickernel_initializeCriticalSection(a)			InitializeCriticalSection(a)
	#define ickernel_deleteCriticalSection(a)				DeleteCriticalSection(a)
	#define ickernel_enterCriticalSection(a)				EnterCriticalSection(a)
	#define ickernel_leaveCriticalSection(a)				LeaveCriticalSection(a)


	#define	ICKERNEL_SEMAPHORE_HANDLE						HANDLE
	#define ickernel_createSemaphore(h)						{ *h = CreateSemaphore( NULL, 0, 1000, 0 ); }
	#define ickernel_closeHandle(h)							CloseHandle(*h)
	#define ickernel_wait(h)								WaitForSingleObject( (HANDLE)*h, INFINITE );
	#define ickernel_post(h)								ReleaseSemaphore((HANDLE)*h, 1, NULL)



	// --------------------------------------------------------------------------------------------//

	typedef HANDLE THREAD_HANDLE;
	typedef void  *(*MX_THREAD_FUNCTION)(void *);

	int ickernel_setThreadPriority( THREAD_HANDLE handle, int n );
	THREAD_HANDLE ickernel_startThread( MX_THREAD_FUNCTION t , void *info );
	void ickernel_waitThread( THREAD_HANDLE thread);


	#define ickernel_getPid()								GetCurrentThreadId()
	#define ickernel_getTid()								GetCurrentThreadId()

	#else



		void _Sleep( int ms );
		#define Sleep _Sleep

	#endif

#endif


