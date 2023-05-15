/*******************************************************************************
 *
 * Copyright (c) 2000-2003 Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * - Neither name of Intel Corporation nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/*!
 * \addtogroup UPnPAPI
 *
 * @{
 *
 * \file
 */

#include "upnp_nflx_config.h"

#include "upnpapi.h"

#include "membuffer.h"
#include "ssdplib.h"
#include "sysdep.h"
#include "ThreadPool.h"
#include "UpnpStdInt.h"
#include "UpnpUniStd.h" /* for close() */
#include "upnp_timeout.h"
#include "uuid.h"
#include "upnpdebug.h"

#include "miniserver.h"

#include <sys/stat.h>

#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
	/* Do not include these files */
#else
	#include <sys/ioctl.h>
	#include <sys/param.h>
	#include <sys/types.h>
#endif

#ifndef IN6_IS_ADDR_GLOBAL
#define IN6_IS_ADDR_GLOBAL(a) \
		(((((__const uint8_t *) (a))[0] & htonl(0xff000000)) <= htonl(0x3f000000)   \
				&& (((__const uint8_t *) (a))[0] & htonl(0xff000000)) >= htonl(0x20000000)))
#endif /* IS ADDR GLOBAL */

#ifndef IN6_IS_ADDR_ULA
#define IN6_IS_ADDR_ULA(a) \
		((((__const uint32_t *) (a))[0] & htonl(0xfe000000)) \
		== htonl (0xfc000000))
#endif /* IS ADDR ULA */

#ifdef INCLUDE_CLIENT_APIS
/*! Mutex to synchronize the subscription handling at the client side. */
ithread_mutex_t GlobalClientSubscribeMutex;
#endif /* INCLUDE_CLIENT_APIS */

/*! rwlock to synchronize handles (root device or control point handle). */
ithread_rwlock_t GlobalHndRWLock;


/*! Mutex to synchronize the uuid creation process. */
ithread_mutex_t gUUIDMutex;

/*! Initialization mutex. */
ithread_mutex_t gSDKInitMutex = PTHREAD_MUTEX_INITIALIZER;

/*! Global timer thread. */
TimerThread gTimerThread;

/*! Send thread pool. */
ThreadPool gSendThreadPool;

/*! Receive thread pool. */
ThreadPool gRecvThreadPool;

/*! Mini server thread pool. */
ThreadPool gMiniServerThreadPool;

/*! Static buffer to contain interface name. (extern'ed in upnp.h) */
char gIF_NAME[LINE_SIZE] = { '\0' };

/*! Static buffer to contain interface IPv4 address. (extern'ed in upnp.h) */
char gIF_IPV4[22]/* INET_ADDRSTRLEN*/ = { '\0' };

/*! Static buffer to contain interface IPv6 address. (extern'ed in upnp.h) */
char gIF_IPV6[65]/* INET6_ADDRSTRLEN*/ = { '\0' };

/*! Static buffer to contain interface ULA or GUA IPv6 address. (extern'ed in upnp.h) */
char gIF_IPV6_ULA_GUA[INET6_ADDRSTRLEN] = { '\0' };

/*! Contains interface index. (extern'ed in upnp.h) */
unsigned gIF_INDEX = (unsigned)-1;

unsigned short gSSDP_PORT = SSDP_DEFAULT_PORT;
char gSSDP_TTL = SSDP_DEFAULT_TTL;

/*! UPnP device and control point handle table  */
#define NUM_HANDLE 200
static void *HandleTable[NUM_HANDLE];

/*! Maximum content-length (in bytes) that the SDK will process on an incoming
 * packet. Content-Length exceeding this size will be not processed and
 * error 413 (HTTP Error Code) will be returned to the remote end point. */
size_t g_maxContentLength = DEFAULT_CONTENT_LENGTH;

/*! Global variable to denote the state of Upnp SDK == 0 if uninitialized,
 * == 1 if initialized. */
int UpnpSdkInit = 0;

/*! Global variable to denote the state of Upnp SDK client registration.
 * == 0 if unregistered, == 1 if registered. */
int UpnpSdkClientRegistered = 0;

/*! Global variable to denote the state of Upnp SDK IPv4 device registration.
 * == 0 if unregistered, == 1 if registered. */
int UpnpSdkDeviceRegisteredV4 = 0;

/*! Global variable to denote the state of Upnp SDK IPv6 device registration.
 * == 0 if unregistered, == 1 if registered. */
int UpnpSdkDeviceregisteredV6 = 0;

/*! Global variable used in discovery notifications. */
Upnp_SID gUpnpSdkNLSuuid;


/*!
 * \brief (Windows Only) Initializes the Windows Winsock library.
 *
 * \return UPNP_E_SUCCESS on success, UPNP_E_INIT_FAILED on failure.
 */
static int WinsockInit(void)
{
	int retVal = UPNP_E_SUCCESS;
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		retVal = UPNP_E_INIT_FAILED;
		goto exit_function;
	}
	/* Confirm that the WinSock DLL supports 2.2.
	 * Note that if the DLL supports versions greater
	 * than 2.2 in addition to 2.2, it will still return
	 * 2.2 in wVersion since that is the version we
	 * requested. */
	if (LOBYTE(wsaData.wVersion) != 2 ||
	    HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable
		 * WinSock DLL. */
		WSACleanup();
		retVal = UPNP_E_INIT_FAILED;
		goto exit_function;
	}
	/* The WinSock DLL is acceptable. Proceed. */
exit_function:
#else
#endif
	return retVal;
}


/*!
 * \brief Initializes the global mutexes used by the UPnP SDK.
 *
 * \return UPNP_E_SUCCESS on success or UPNP_E_INIT_FAILED if a mutex could not
 * 	be initialized.
 */
static int UpnpInitMutexes(void)
{
#ifdef __CYGWIN__
	/* On Cygwin, pthread_mutex_init() fails without this memset. */
	/* TODO: Fix Cygwin so we don't need this memset(). */
	memset(&GlobalHndRWLock, 0, sizeof(GlobalHndRWLock));
#endif
	if (ithread_rwlock_init(&GlobalHndRWLock, NULL) != 0) {
		return UPNP_E_INIT_FAILED;
	}

	if (ithread_mutex_init(&gUUIDMutex, NULL) != 0) {
		return UPNP_E_INIT_FAILED;
	}
	/* initialize subscribe mutex. */
#ifdef INCLUDE_CLIENT_APIS
	if (ithread_mutex_init(&GlobalClientSubscribeMutex, NULL) != 0) {
		return UPNP_E_INIT_FAILED;
	}
#endif
	return UPNP_E_SUCCESS;
}


/*!
 * \brief Initializes the global threadm pools used by the UPnP SDK.
 *
 * \return UPNP_E_SUCCESS on success or UPNP_E_INIT_FAILED if a mutex could not
 * 	be initialized.
 */
static int UpnpInitThreadPools(void)
{
	int ret = UPNP_E_SUCCESS;
	ThreadPoolAttr attr;

	TPAttrInit(&attr);
	TPAttrSetMaxThreads(&attr, MAX_THREADS);
	TPAttrSetMinThreads(&attr, MIN_THREADS);
	TPAttrSetStackSize(&attr, THREAD_STACK_SIZE);
	TPAttrSetJobsPerThread(&attr, JOBS_PER_THREAD);
	TPAttrSetIdleTime(&attr, THREAD_IDLE_TIME);
	TPAttrSetMaxJobsTotal(&attr, MAX_JOBS_TOTAL);

	if (ThreadPoolInit(&gSendThreadPool, &attr) != UPNP_E_SUCCESS) {
		ret = UPNP_E_INIT_FAILED;
		goto exit_function;
	}

	if (ThreadPoolInit(&gRecvThreadPool, &attr) != UPNP_E_SUCCESS) {
		ret = UPNP_E_INIT_FAILED;
		goto exit_function;
	}

	if (ThreadPoolInit(&gMiniServerThreadPool, &attr) != UPNP_E_SUCCESS) {
		ret = UPNP_E_INIT_FAILED;
		goto exit_function;
	}

exit_function:
	if (ret != UPNP_E_SUCCESS) {
		UpnpSdkInit = 0;
		UpnpFinish();
	}

	return ret;
}


/*!
 * \brief Performs the initial steps in initializing the UPnP SDK.
 *
 *	\li Winsock library is initialized for the process (Windows specific).
 *	\li The logging (for debug messages) is initialized.
 *	\li Mutexes, Handle table and thread pools are allocated and initialized.
 *	\li The SDK timer thread is initialized.
 *
 * \return UPNP_E_SUCCESS on success.
 */
static int UpnpInitPreamble(void)
{
	int retVal = UPNP_E_SUCCESS;
	int i;
	uuid_upnp nls_uuid;

	retVal = WinsockInit();
	if (retVal != UPNP_E_SUCCESS) {
		return retVal;
	}

	/* needed by SSDP or other parts. */
	srand((unsigned int)time(NULL));

	/* Initialize debug output. */
	retVal = UpnpInitLog();
	if (retVal != UPNP_E_SUCCESS) {
		/* UpnpInitLog does not return a valid UPNP_E_*. */
		return UPNP_E_INIT_FAILED;
	}

	UpnpPrintf( UPNP_INFO, API, __FILE__, __LINE__, "Inside UpnpInitPreamble\n" );

	/* Initialize SDK global mutexes. */
	retVal = UpnpInitMutexes();
	if (retVal != UPNP_E_SUCCESS) {
		return retVal;
	}

	/* Create the NLS uuid. */
	uuid_create(&nls_uuid);
	uuid_unpack(&nls_uuid, gUpnpSdkNLSuuid);

	/* Initializes the handle list. */
	HandleLock();
	for (i = 0; i < NUM_HANDLE; ++i) {
		HandleTable[i] = NULL;
	}
	HandleUnlock();

	/* Initialize SDK global thread pools. */
	retVal = UpnpInitThreadPools();
	if (retVal != UPNP_E_SUCCESS) {
		return retVal;
	}

	/* Initialize the SDK timer thread. */
	retVal = TimerThreadInit( &gTimerThread, &gSendThreadPool );
	if (retVal != UPNP_E_SUCCESS) {
		UpnpFinish();

		return retVal;
	}

	return UPNP_E_SUCCESS;
}


/*!
 * \brief Finishes initializing the UPnP SDK.
 *	\li The MiniServer is started, if enabled.
 *
 * \return UPNP_E_SUCCESS on success or  UPNP_E_INIT_FAILED if a mutex could not
 * 	be initialized.
 */
static int UpnpInitStartServers(unsigned short advtPort, char advtTTL)
{
	int retVal = 0;

	UpnpPrintf( UPNP_INFO, API, __FILE__, __LINE__,
		"Entering UpnpInitStartServers\n" );

#if EXCLUDE_MINISERVER == 0
	retVal = StartMiniServer(advtPort, advtTTL);
	if (retVal != UPNP_E_SUCCESS) {
		UpnpPrintf(UPNP_CRITICAL, API, __FILE__, __LINE__,
			"Miniserver failed to start");
		UpnpFinish();
		return retVal;
	}
#endif

	UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
		"Exiting UpnpInitStartServers\n");

	return UPNP_E_SUCCESS;
}


int UpnpInit(const char *HostIP, unsigned short advtPort, char advtTTL)
{
	int retVal = UPNP_E_SUCCESS;

	/* Initializes the ithread library */
	ithread_initialize_library();

	ithread_mutex_lock(&gSDKInitMutex);

	/* Check if we're already initialized. */
	if (UpnpSdkInit == 1) {
		retVal = UPNP_E_INIT;
		goto exit_function;
	}

	/* Perform initialization preamble. */
	retVal = UpnpInitPreamble();
	if (retVal != UPNP_E_SUCCESS) {
		goto exit_function;
	}

	UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
		"UpnpInit with HostIP=%s, advtPort=%d.\n",
			   HostIP ? HostIP : "", advtPort);

	/* Verify HostIP, if provided, or find it ourselves. */
	if (HostIP != NULL) {
		strncpy(gIF_IPV4, HostIP, sizeof(gIF_IPV4));
	} else {
		if( getlocalhostname( gIF_IPV4, sizeof(gIF_IPV4) ) != UPNP_E_SUCCESS ) {
			retVal = UPNP_E_INIT_FAILED;
			goto exit_function;
		}
	}

	/* Set the UpnpSdkInit flag to 1 to indicate we're successfully initialized. */
	UpnpSdkInit = 1;

	/* Finish initializing the SDK. */
	retVal = UpnpInitStartServers(advtPort, advtTTL);
	if (retVal != UPNP_E_SUCCESS) {
		UpnpSdkInit = 0;
		goto exit_function;
	}

	UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
		"Host Ip: %s\n", gIF_IPV4);

exit_function:
	ithread_mutex_unlock(&gSDKInitMutex);

	return retVal;
}

#ifdef UPNP_ENABLE_IPV6
int UpnpInit2(const char *IfName, unsigned short advtPort, char advtTTL)
{
	int retVal;

	/* Initializes the ithread library */
	ithread_initialize_library();

	ithread_mutex_lock(&gSDKInitMutex);

	/* Check if we're already initialized. */
	if (UpnpSdkInit == 1) {
		retVal = UPNP_E_INIT;
		goto exit_function;
	}

	/* Perform initialization preamble. */
	retVal = UpnpInitPreamble();
	if (retVal != UPNP_E_SUCCESS) {
		goto exit_function;
	}

	UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
		"UpnpInit2 with IfName=%s, advtPort=%d.\n",
		IfName ? IfName : "", advtPort);

	/* Retrieve interface information (Addresses, index, etc). */
	retVal = UpnpGetIfInfo( IfName );
	if (retVal != UPNP_E_SUCCESS) {
		goto exit_function;
	}

	/* Set the UpnpSdkInit flag to 1 to indicate we're successfully initialized. */
	UpnpSdkInit = 1;

	/* Finish initializing the SDK. */
	retVal = UpnpInitStartServers(advtPort, advtTTL);
	if (retVal != UPNP_E_SUCCESS) {
		UpnpSdkInit = 0;
		goto exit_function;
	}

exit_function:
	ithread_mutex_unlock(&gSDKInitMutex);

	return retVal;
}
#endif

#ifdef DEBUG
/*!
 * \brief Prints thread pool statistics.
 */
void PrintThreadPoolStats(
	/*! [in] The thread pool. */
	ThreadPool *tp,
	/*! [in] The file name that called this function, use the macro
	 * __FILE__. */
	const char *DbgFileName,
	/*! [in] The line number that the function was called, use the macro
	 * __LINE__. */
	int DbgLineNo,
	/*! [in] The message. */
	const char *msg)
{
	ThreadPoolStats stats;
	ThreadPoolGetStats(tp, &stats);
	UpnpPrintf(UPNP_INFO, API, DbgFileName, DbgLineNo,
		"%s\n"
		"High Jobs pending: %d\n"
		"Med Jobs Pending: %d\n"
		"Low Jobs Pending: %d\n"
		"Average wait in High Q in milliseconds: %lf\n"
		"Average wait in Med Q in milliseconds: %lf\n"
		"Average wait in Low Q in milliseconds: %lf\n"
		"Max Threads Used: %d\n"
		"Worker Threads: %d\n"
		"Persistent Threads: %d\n"
		"Idle Threads: %d\n"
		"Total Threads: %d\n"
		"Total Work Time: %lf\n"
		"Total Idle Time: %lf\n",
		msg,
		stats.currentJobsHQ,
		stats.currentJobsMQ,
		stats.currentJobsLQ,
		stats.avgWaitHQ,
		stats.avgWaitMQ,
		stats.avgWaitLQ,
		stats.maxThreads,
		stats.workerThreads,
		stats.persistentThreads,
		stats.idleThreads,
		stats.totalThreads,
		stats.totalWorkTime,
		stats.totalIdleTime);
}
#else
static UPNP_INLINE void PrintThreadPoolStats(ThreadPool *tp,
	const char *DbgFileName, int DbgLineNo, const char *msg)
{
	(void)tp;
	(void)DbgFileName;
	(void)DbgLineNo;
	(void)msg;
	return;
}
#endif /* DEBUG */

int UpnpFinish(void)
{
#ifdef INCLUDE_DEVICE_APIS
	UpnpDevice_Handle device_handle;
#endif
#ifdef INCLUDE_CLIENT_APIS
	UpnpClient_Handle client_handle;
#endif
	struct Handle_Info *temp = NULL;

	if (UpnpSdkInit != 1)
		return UPNP_E_FINISH;
	UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
		   "Inside UpnpFinish: UpnpSdkInit is %d\n", UpnpSdkInit);
	if (UpnpSdkInit == 1)
		UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
			"UpnpFinish: UpnpSdkInit is ONE\n");
	PrintThreadPoolStats(&gSendThreadPool, __FILE__, __LINE__,
		"Send Thread Pool");
	PrintThreadPoolStats(&gRecvThreadPool, __FILE__, __LINE__,
		"Recv Thread Pool");
	PrintThreadPoolStats(&gMiniServerThreadPool, __FILE__, __LINE__,
		"MiniServer Thread Pool");
#ifdef INCLUDE_DEVICE_APIS
	if (GetDeviceHandleInfo(AF_INET, &device_handle, &temp) == HND_DEVICE)
		UpnpUnRegisterRootDevice(device_handle);
	if (GetDeviceHandleInfo(AF_INET6, &device_handle, &temp) == HND_DEVICE)
		UpnpUnRegisterRootDevice(device_handle);
#endif
#ifdef INCLUDE_CLIENT_APIS
	if (GetClientHandleInfo(&client_handle, &temp) == HND_CLIENT)
		UpnpUnRegisterClient(client_handle);
#endif
	TimerThreadShutdown(&gTimerThread);
	StopMiniServer();

	ThreadPoolShutdown(&gMiniServerThreadPool);
	PrintThreadPoolStats(&gMiniServerThreadPool, __FILE__, __LINE__,
		"MiniServer Thread Pool");
	ThreadPoolShutdown(&gRecvThreadPool);
	PrintThreadPoolStats(&gSendThreadPool, __FILE__, __LINE__,
		"Send Thread Pool");
	ThreadPoolShutdown(&gSendThreadPool);
	PrintThreadPoolStats(&gRecvThreadPool, __FILE__, __LINE__,
		"Recv Thread Pool");
#ifdef INCLUDE_CLIENT_APIS
	ithread_mutex_destroy(&GlobalClientSubscribeMutex);
#endif
	ithread_rwlock_destroy(&GlobalHndRWLock);
	ithread_mutex_destroy(&gUUIDMutex);
	/* Clean-up ithread library resources */
	ithread_cleanup_library();
	UpnpSdkInit = 0;
	UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
		"Exiting UpnpFinish: UpnpSdkInit is :%d:\n", UpnpSdkInit);
	UpnpCloseLog();

	return UPNP_E_SUCCESS;
}

char *UpnpGetServerIpAddress(void)
{
	if (UpnpSdkInit != 1)
		return NULL;

	return gIF_IPV4;
}

unsigned short UpnpGetSSDPPort(void)
{
	return gSSDP_PORT;
}

char *UpnpGetServerIp6Address(void)
{
	if (UpnpSdkInit != 1)
		return NULL;

	return gIF_IPV6;
}

char *UpnpGetServerUlaGuaIp6Address(void)
{
	if (UpnpSdkInit != 1)
		return NULL;

	return gIF_IPV6_ULA_GUA;
}

/*!
 * \brief Get a free handle.
 *
 * \return On success, an integer greater than zero or UPNP_E_OUTOF_HANDLE on
 * 	failure.
 */
static int GetFreeHandle()
{
	/* Handle 0 is not used as NULL translates to 0 when passed as a handle */
	int i = 1;

	while (i < NUM_HANDLE && HandleTable[i] != NULL)
		++i;
	if (i == NUM_HANDLE)
		return UPNP_E_OUTOF_HANDLE;
	else
		return i;
}

/*!
 * \brief Free handle.
 *
 * \return UPNP_E_SUCCESS if successful or UPNP_E_INVALID_HANDLE if not
 */
static int FreeHandle(
	/*! [in] Handle index. */
	int Upnp_Handle)
{
	int ret = UPNP_E_INVALID_HANDLE;

	UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
		"FreeHandle: entering, Handle is %d\n", Upnp_Handle);
	if (Upnp_Handle < 1 || Upnp_Handle >= NUM_HANDLE) {
		UpnpPrintf(UPNP_CRITICAL, API, __FILE__, __LINE__,
			"FreeHandle: Handle %d is out of range\n",
			Upnp_Handle);
	} else if (HandleTable[Upnp_Handle] == NULL) {
		UpnpPrintf(UPNP_CRITICAL, API, __FILE__, __LINE__,
			"FreeHandle: HandleTable[%d] is NULL\n",
			Upnp_Handle);
	} else {
		free( HandleTable[Upnp_Handle] );
		HandleTable[Upnp_Handle] = NULL;
		ret = UPNP_E_SUCCESS;
	}
	UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
		"FreeHandle: exiting, ret = %d.\n", ret);

	return ret;
}

#ifdef INCLUDE_DEVICE_APIS
int UpnpRegisterRootDevice(const char *location,
                           const char *description_const,
                           size_t bufferLen,   /* ignored */
                           int config_baseURL,
                           int NumReplyAdvertCopy,
                           Upnp_FunPtr Fun,
                           const void *Cookie,
                           UpnpDevice_Handle *Hnd)
{
	struct Handle_Info *HInfo = NULL;
	int retVal = 0;
	char *description = (char *)description_const;
	unsigned long iNode;
	int ret_code;
	IXML_NodeList *nodeList;
    int friendlyNameFound = 0;

	(void)bufferLen;

	HandleLock();

	UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
		"Inside UpnpRegisterRootDevice\n");

	if (UpnpSdkInit != 1) {
		retVal = UPNP_E_FINISH;
		goto exit_function;
	}

	if (Hnd == NULL || Fun == NULL) {
		retVal = UPNP_E_INVALID_PARAM;
		goto exit_function;
	}

	if (UpnpSdkDeviceRegisteredV4 == 1) {
		retVal = UPNP_E_ALREADY_REGISTERED;
		goto exit_function;
	}

	*Hnd = GetFreeHandle();
	if (*Hnd == UPNP_E_OUTOF_HANDLE) {
		retVal = UPNP_E_OUTOF_MEMORY;
		goto exit_function;
	}

	HInfo = (struct Handle_Info *)malloc(sizeof (struct Handle_Info));
	if (HInfo == NULL) {
		retVal = UPNP_E_OUTOF_MEMORY;
		goto exit_function;
	}
	HandleTable[*Hnd] = HInfo;

	/* prevent accidental removal of a non-existent alias */
	HInfo->aliasInstalled = 0;

	strcpy(HInfo->DescURL, location);

	ret_code = ixmlParseBufferEx(description, &HInfo->DescDocument);
	if (ret_code != IXML_SUCCESS) {
		if (ret_code == IXML_INSUFFICIENT_MEMORY) {
			UpnpPrintf(UPNP_CRITICAL, API, __FILE__, __LINE__,
				"Out of memory, ixml error code: %d\n",
				ret_code);
			return UPNP_E_OUTOF_MEMORY;
		} else {
			UpnpPrintf(UPNP_CRITICAL, API, __FILE__, __LINE__,
				"Invalid Description, ixml error code: %d\n",
				ret_code);
			return UPNP_E_INVALID_DESC;
		}
	}

	strcpy(HInfo->LowerDescURL, HInfo->DescURL);
	UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
		"Following Root Device URL will be used when answering to legacy CPs %s\n",
		HInfo->LowerDescURL);
	HInfo->aliasInstalled = config_baseURL != 0;
	HInfo->HType = HND_DEVICE;
	HInfo->Callback = Fun;
	HInfo->Cookie = (void *)Cookie;
	HInfo->MaxAge = DEFAULT_MAXAGE;
	HInfo->DeviceList = NULL;
	HInfo->ServiceList = NULL;
    ListInit(&HInfo->DeviceReplyHeaderList, NULL, NULL);
#ifdef INCLUDE_CLIENT_APIS
	ListInit(&HInfo->SsdpSearchList, NULL, NULL);
    ListInit(&HInfo->HeaderPatternList, NULL, NULL);
#endif /* INCLUDE_CLIENT_APIS */
	HInfo->DeviceAf = AF_INET;
    HInfo->NumReplyAdvertCopy = NumReplyAdvertCopy > 0 ? NumReplyAdvertCopy : NUM_SSDP_COPY;

	UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
		"UpnpRegisterRootDevice: Valid Description\n"
		"UpnpRegisterRootDevice: DescURL : %s\n",
		HInfo->DescURL);

	HInfo->DeviceList =
		ixmlDocument_getElementsByTagName( HInfo->DescDocument, "device" );
	if (!HInfo->DeviceList) {
#ifdef INCLUDE_CLIENT_APIS
		ListDestroy(&HInfo->SsdpSearchList, 0);
#endif /* INCLUDE_CLIENT_APIS */
		ixmlDocument_free(HInfo->DescDocument);
		FreeHandle(*Hnd);
		UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
			"UpnpRegisterRootDevice: No devices found for RootDevice\n" );
		retVal = UPNP_E_INVALID_DESC;
		goto exit_function;
	}

	HInfo->ServiceList = ixmlDocument_getElementsByTagName(
		HInfo->DescDocument, "serviceList" );
	if (!HInfo->ServiceList) {
		UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
			"UpnpRegisterRootDevice: No services found for RootDevice\n");
	}

    HInfo->FriendlyName[0] = 0;

	nodeList = ixmlDocument_getElementsByTagName(HInfo->DescDocument, "friendlyName");
	if(nodeList)
	{
		for(iNode = 0; iNode < ixmlNodeList_length(nodeList); ++iNode)
		{
			IXML_Node *node = ixmlNodeList_item(nodeList, iNode);
			assert(node);
			node = ixmlNode_getFirstChild(node);
			if(node && ixmlNode_getNodeType(node) == eTEXT_NODE)
			{
				const DOMString friendlyName = ixmlNode_getNodeValue(node);
				strcpy(HInfo->FriendlyName, friendlyName);
                friendlyNameFound = 1;
                break;
			}
		}
	}

	if (!friendlyNameFound) {
		UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
			"UpnpRegisterRootDevice: No Friendly Name found for RootDevice\n");
		retVal = UPNP_E_INVALID_DESC;
		goto exit_function;
	}

	UpnpSdkDeviceRegisteredV4 = 1;

	retVal = UPNP_E_SUCCESS;

exit_function:
	UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
		"Exiting RegisterRootDevice, return value == %d\n", retVal);
	HandleUnlock();

	return retVal;
}
#endif /* INCLUDE_DEVICE_APIS */

#ifdef INCLUDE_DEVICE_APIS
int UpnpUnRegisterRootDevice(UpnpDevice_Handle Hnd)
{
	int retVal = 0;
	struct Handle_Info *HInfo = NULL;
    ListNode *node;
    char *header;

	if (UpnpSdkInit != 1)
		return UPNP_E_FINISH;
	UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
		   "Inside UpnpUnRegisterRootDevice\n");

	HandleLock();
	if ((int)(GetHandleInfo(Hnd, &HInfo)) == UPNP_E_INVALID_HANDLE) {
		HandleUnlock();
		return UPNP_E_INVALID_HANDLE;
	}
	HandleUnlock();

#if EXCLUDE_SSDP == 0
	retVal = AdvertiseAndReply(-1, Hnd, 0, (struct sockaddr *)NULL,
		(char *)NULL, (char *)NULL, (char *)NULL,
		HInfo->MaxAge);
#endif

	HandleLock();
	if ((int)(GetHandleInfo(Hnd, &HInfo)) == UPNP_E_INVALID_HANDLE) {
		HandleUnlock();
		return UPNP_E_INVALID_HANDLE;
	}
	ixmlNodeList_free(HInfo->DeviceList);
	ixmlNodeList_free(HInfo->ServiceList);
	ixmlDocument_free(HInfo->DescDocument);
#ifdef INCLUDE_CLIENT_APIS
	ListDestroy(&HInfo->SsdpSearchList, 0);
    ListDestroy(&HInfo->HeaderPatternList, 0);
#endif /* INCLUDE_CLIENT_APIS */
    /* clean up headers list */
    node = ListHead(&HInfo->DeviceReplyHeaderList);
    while (node != NULL) {
        header = (char *)node->item;
        if (header) {
            free(header);
        }
        ListDelNode(&HInfo->DeviceReplyHeaderList, node, 0);
        node = ListHead(&HInfo->DeviceReplyHeaderList);
    }
    ListDestroy(&HInfo->DeviceReplyHeaderList, 0);
	if (HInfo->DeviceAf == AF_INET)
		UpnpSdkDeviceRegisteredV4 = 0;
	else if (HInfo->DeviceAf == AF_INET6)
		UpnpSdkDeviceregisteredV6 = 0;
	FreeHandle(Hnd);
	HandleUnlock();

	UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
		   "Exiting UpnpUnRegisterRootDevice\n");

	return retVal;
}
#endif /* INCLUDE_DEVICE_APIS */

#ifdef INCLUDE_CLIENT_APIS
int UpnpRegisterClient(Upnp_FunPtr Fun,
                       const void *Cookie,
                       UpnpClient_Handle *Hnd)
{
    return UpnpRegisterClient2(Fun, Cookie, NULL, 0, Hnd);
}

int UpnpRegisterClient2(Upnp_FunPtr Fun,
                        const void *Cookie,
                        const char **apHeaderPatterns,
                        int patternCount,
                        UpnpClient_Handle *Hnd)
{
	struct Handle_Info *HInfo;
    int i;
    char *headerPattern;

	if (UpnpSdkInit != 1)
		return UPNP_E_FINISH;
	UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
               "Inside UpnpRegisterClient \n");
	if (Fun == NULL || Hnd == NULL)
		return UPNP_E_INVALID_PARAM;

	HandleLock();
	if (UpnpSdkClientRegistered) {
		HandleUnlock();
		return UPNP_E_ALREADY_REGISTERED;
	}

    if (patternCount < 0 || (patternCount > 0 && apHeaderPatterns == NULL))
    {
        return UPNP_E_INVALID_PARAM;
    }

    for(i = 0; i < patternCount; i++)
    {
        if(!apHeaderPatterns[i])
        {
            return UPNP_E_INVALID_PARAM;
        }
    }

	if ((*Hnd = GetFreeHandle()) == UPNP_E_OUTOF_HANDLE) {
		HandleUnlock();
		return UPNP_E_OUTOF_MEMORY;
	}
	HInfo = (struct Handle_Info *)malloc(sizeof(struct Handle_Info));
	if (HInfo == NULL) {
		HandleUnlock();
		return UPNP_E_OUTOF_MEMORY;
	}
	HInfo->HType = HND_CLIENT;
	HInfo->Callback = Fun;
	HInfo->Cookie = (void *)Cookie;
	ListInit(&HInfo->SsdpSearchList, NULL, NULL);
    ListInit(&HInfo->HeaderPatternList, NULL, NULL);
    for(i = 0; i < patternCount; i++)
    {
        headerPattern = (char *)malloc(strlen(apHeaderPatterns[i]) + 1);
        if(!headerPattern) {
            return UPNP_E_OUTOF_MEMORY;
        }
        strcpy(headerPattern, apHeaderPatterns[i]);
        ListAddTail(&HInfo->HeaderPatternList, headerPattern);
    }
    ListInit(&HInfo->DeviceReplyHeaderList, NULL, NULL);
#ifdef INCLUDE_DEVICE_APIS
	HInfo->MaxAge = 0;
#endif
	HandleTable[*Hnd] = HInfo;
	UpnpSdkClientRegistered = 1;
	HandleUnlock();

	UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
               "Exiting UpnpRegisterClient \n");

	return UPNP_E_SUCCESS;
}
#endif /* INCLUDE_CLIENT_APIS */

#ifdef INCLUDE_CLIENT_APIS
int UpnpUnRegisterClient(UpnpClient_Handle Hnd)
{
	struct Handle_Info *HInfo = NULL;
	ListNode *node = NULL;
	SsdpSearchArg *searchArg = NULL;
    char *headerPattern;

	if (UpnpSdkInit != 1)
		return UPNP_E_FINISH;
	UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
		   "Inside UpnpUnRegisterClient \n");

	HandleLock();
	if (!UpnpSdkClientRegistered) {
		HandleUnlock();
		return UPNP_E_INVALID_HANDLE;
	}
	HandleUnlock();

	HandleLock();
	if ((int)(GetHandleInfo(Hnd, &HInfo)) == UPNP_E_INVALID_HANDLE) {
		HandleUnlock();
		return UPNP_E_INVALID_HANDLE;
	}
	/* clean up search list */
	node = ListHead(&HInfo->SsdpSearchList);
	while (node != NULL) {
		searchArg = (SsdpSearchArg *) node->item;
		if (searchArg) {
			free(searchArg->searchTarget);
			free(searchArg);
		}
		ListDelNode(&HInfo->SsdpSearchList, node, 0);
		node = ListHead(&HInfo->SsdpSearchList);
	}
	ListDestroy(&HInfo->SsdpSearchList, 0);
    /* clean up headers list */
    node = ListHead(&HInfo->HeaderPatternList);
    while (node != NULL) {
        headerPattern = (char *)node->item;
        if (headerPattern) {
            free(headerPattern);
        }
        ListDelNode(&HInfo->HeaderPatternList, node, 0);
        node = ListHead(&HInfo->HeaderPatternList);
    }
    ListDestroy(&HInfo->HeaderPatternList, 0);
    ListDestroy(&HInfo->DeviceReplyHeaderList, 0);
	FreeHandle(Hnd);
	UpnpSdkClientRegistered = 0;
	HandleUnlock();

	UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
		   "Exiting UpnpUnRegisterClient \n");

	return UPNP_E_SUCCESS;
}
#endif /* INCLUDE_CLIENT_APIS */

/*******************************************************************************
 *
 *                                  SSDP interface
 *
 ******************************************************************************/


#ifdef INCLUDE_DEVICE_APIS
#if EXCLUDE_SSDP == 0
int UpnpSendAdvertisement(UpnpDevice_Handle Hnd, int Exp)
{
    struct Handle_Info *SInfo = NULL;
    int retVal = 0,
     *ptrMx;
    upnp_timeout *adEvent;
    ThreadPoolJob job;

    if( UpnpSdkInit != 1 ) {
        return UPNP_E_FINISH;
    }

    UpnpPrintf( UPNP_ALL, API, __FILE__, __LINE__,
        "Inside UpnpSendAdvertisement \n" );

    HandleLock();
    if( GetHandleInfo( Hnd, &SInfo ) != HND_DEVICE ) {
        HandleUnlock();
        return UPNP_E_INVALID_HANDLE;
    }
    if( Exp < 1 )
        Exp = DEFAULT_MAXAGE;
    SInfo->MaxAge = Exp;
    HandleUnlock();
    retVal = AdvertiseAndReply( 1, Hnd, 0, ( struct sockaddr * )NULL,
                                ( char * )NULL, ( char * )NULL,
                                ( char * )NULL, Exp );

    if( retVal != UPNP_E_SUCCESS )
        return retVal;
    ptrMx = ( int * )malloc( sizeof( int ) );
    if( ptrMx == NULL )
        return UPNP_E_OUTOF_MEMORY;
    adEvent = ( upnp_timeout * ) malloc( sizeof( upnp_timeout ) );

    if( adEvent == NULL ) {
        free( ptrMx );
        return UPNP_E_OUTOF_MEMORY;
    }
    *ptrMx = Exp;
    adEvent->handle = Hnd;
    adEvent->Event = ptrMx;

    HandleLock();
    if( GetHandleInfo( Hnd, &SInfo ) != HND_DEVICE ) {
        HandleUnlock();
        free( adEvent );
        free( ptrMx );
        return UPNP_E_INVALID_HANDLE;
    }

	// we just want the advertising interval to be exactly what we specified
	TPJobInit(&job, (start_routine)AutoAdvertise, adEvent);
	TPJobSetFreeFunction(&job, (free_routine)free_upnp_timeout);
	TPJobSetPriority(&job, MED_PRIORITY);
	if((retVal = TimerThreadSchedule(&gTimerThread, Exp, REL_SEC, &job, SHORT_TERM, &(adEvent->eventId))) != UPNP_E_SUCCESS)
	{
		HandleUnlock();
		free(adEvent);
		free(ptrMx);
		return retVal;
	}

    HandleUnlock();
    UpnpPrintf( UPNP_ALL, API, __FILE__, __LINE__,
        "Exiting UpnpSendAdvertisement \n" );

    return retVal;

}
#endif /* EXCLUDE_SSDP == 0 */
#endif /* INCLUDE_DEVICE_APIS */


#if EXCLUDE_SSDP == 0
#ifdef INCLUDE_CLIENT_APIS


int UpnpSearchAsync(
	UpnpClient_Handle Hnd,
	int Mx,
    int NumSearchCopy,
	const char *Target_const,
	const void *Cookie_const )
{
    struct Handle_Info *SInfo = NULL;
    char *Target = ( char * )Target_const;

    if( UpnpSdkInit != 1 ) {
        return UPNP_E_FINISH;
    }

    UpnpPrintf( UPNP_ALL, API, __FILE__, __LINE__,
        "Inside UpnpSearchAsync\n" );

    HandleReadLock();
    if( GetHandleInfo( Hnd, &SInfo ) != HND_CLIENT ) {
        HandleUnlock();
        return UPNP_E_INVALID_HANDLE;
    }
    if( Mx < 1 )
        Mx = DEFAULT_MX;

    if( Target == NULL ) {
        HandleUnlock();
        return UPNP_E_INVALID_PARAM;
    }

    NumSearchCopy = NumSearchCopy > 0 ? NumSearchCopy : NUM_SSDP_COPY;

    HandleUnlock();
    SearchByTarget( Mx, Target, ( void * )Cookie_const, NumSearchCopy );

    UpnpPrintf( UPNP_ALL, API, __FILE__, __LINE__,
        "Exiting UpnpSearchAsync \n" );

    return UPNP_E_SUCCESS;

}
#endif /* INCLUDE_CLIENT_APIS */
#endif

/*******************************************************************************
 *
 *                             Client API
 *
 ******************************************************************************/

int UpnpGetIfInfo(const char *IfName)
{
#ifdef WIN32
	/* ---------------------------------------------------- */
	/* WIN32 implementation will use the IpHlpAPI library. */
	/* ---------------------------------------------------- */
	PIP_ADAPTER_ADDRESSES adapts = NULL;
	PIP_ADAPTER_ADDRESSES adapts_item;
	PIP_ADAPTER_UNICAST_ADDRESS uni_addr;
	SOCKADDR *ip_addr;
	struct in_addr v4_addr;
	struct in6_addr v6_addr;
	ULONG adapts_sz = 0;
	ULONG ret;
	int ifname_found = 0;
	int valid_addr_found = 0;

	/* Get Adapters addresses required size. */
	ret = GetAdaptersAddresses(AF_UNSPEC,
				   GAA_FLAG_SKIP_ANYCAST |
				   GAA_FLAG_SKIP_DNS_SERVER, NULL, adapts,
				   &adapts_sz);
	if (ret != ERROR_BUFFER_OVERFLOW) {
		UpnpPrintf(UPNP_CRITICAL, API, __FILE__, __LINE__,
			   "GetAdaptersAddresses failed to find list of adapters\n");
		return UPNP_E_INIT;
	}
	/* Allocate enough memory. */
	adapts = (PIP_ADAPTER_ADDRESSES) malloc(adapts_sz);
	if (adapts == NULL) {
		return UPNP_E_OUTOF_MEMORY;
	}
	/* Do the call that will actually return the info. */
	ret = GetAdaptersAddresses(AF_UNSPEC,
				   GAA_FLAG_SKIP_ANYCAST |
				   GAA_FLAG_SKIP_DNS_SERVER, NULL, adapts,
				   &adapts_sz);
	if (ret != ERROR_SUCCESS) {
		free(adapts);
		UpnpPrintf(UPNP_CRITICAL, API, __FILE__, __LINE__,
			   "GetAdaptersAddresses failed to find list of adapters\n");
		return UPNP_E_INIT;
	}
	/* Copy interface name, if it was provided. */
	if (IfName != NULL) {
		if (strlen(IfName) > sizeof(gIF_NAME))
			return UPNP_E_INVALID_INTERFACE;

		strncpy(gIF_NAME, IfName, sizeof(gIF_NAME));
		ifname_found = 1;
	}
	adapts_item = adapts;
	while (adapts_item != NULL) {
		if (adapts_item->Flags & IP_ADAPTER_NO_MULTICAST) {
			continue;
		}
		if (ifname_found == 0) {
			/* We have found a valid interface name. Keep it. */
#ifdef UPNP_USE_MSVCPP
			/*
			 * Partial fix for VC - friendly name is wchar string,
			 * but currently gIF_NAME is char string. For now try
			 * to convert it, which will work with many (but not
			 * all) adapters. A full fix would require a lot of
			 * big changes (gIF_NAME to wchar string?).
			 */
			wcstombs(gIF_NAME, adapts_item->FriendlyName,
				sizeof(gIF_NAME));
#else
			strncpy(gIF_NAME, adapts_item->FriendlyName,
				sizeof(gIF_NAME));
#endif
			ifname_found = 1;
		} else {
#ifdef UPNP_USE_MSVCPP
			/*
			 * Partial fix for VC - friendly name is wchar string,
			 * but currently gIF_NAME is char string. For now try
			 * to convert it, which will work with many (but not
			 * all) adapters. A full fix would require a lot of
			 * big changes (gIF_NAME to wchar string?).
			 */
			char tmpIfName[LINE_SIZE] = { 0 };
			wcstombs(tmpIfName, adapts_item->FriendlyName,
				sizeof(tmpIfName));
			if (strncmp
			    (gIF_NAME, tmpIfName,
			     sizeof(gIF_NAME)) != 0) {
				/* This is not the interface we're looking for. */
				continue;
			}
#else
			if (strncmp
			    (gIF_NAME, adapts_item->FriendlyName,
			     sizeof(gIF_NAME)) != 0) {
				/* This is not the interface we're looking for. */
				continue;
			}
#endif
		}
		/* Loop thru this adapter's unicast IP addresses. */
		uni_addr = adapts_item->FirstUnicastAddress;
		while (uni_addr) {
			ip_addr = uni_addr->Address.lpSockaddr;
			switch (ip_addr->sa_family) {
			case AF_INET:
				memcpy(&v4_addr,
				       &((struct sockaddr_in *)ip_addr)->
				       sin_addr, sizeof(v4_addr));
				valid_addr_found = 1;
				break;
#if UPNP_ENABLE_IPV6
			case AF_INET6:
				/* Only keep IPv6 link-local addresses. */
				if (IN6_IS_ADDR_LINKLOCAL
				    (&((struct sockaddr_in6 *)ip_addr)->
				     sin6_addr)) {
					memcpy(&v6_addr,
					       &((struct sockaddr_in6 *)
						 ip_addr)->sin6_addr,
					       sizeof(v6_addr));
					valid_addr_found = 1;
				}
				break;
#endif
			default:
				if (valid_addr_found == 0) {
					/* Address is not IPv4 or IPv6 and no valid address has  */
					/* yet been found for this interface. Discard interface name. */
					ifname_found = 0;
				}
				break;
			}
			/* Next address. */
			uni_addr = uni_addr->Next;
		}
		if (valid_addr_found == 1) {
			gIF_INDEX = adapts_item->IfIndex;
			break;
		}
		/* Next adapter. */
		adapts_item = adapts_item->Next;
	}
	/* Failed to find a valid interface, or valid address. */
	if (ifname_found == 0 || valid_addr_found == 0) {
		UpnpPrintf(UPNP_CRITICAL, API, __FILE__, __LINE__,
			   "Failed to find an adapter with valid IP addresses for use.\n");
		return UPNP_E_INVALID_INTERFACE;
	}
	inet_ntop(AF_INET, &v4_addr, gIF_IPV4, sizeof(gIF_IPV4));
	inet_ntop(AF_INET6, &v6_addr, gIF_IPV6, sizeof(gIF_IPV6));
#elif (defined(BSD) && BSD >= 199306) || defined(__FreeBSD_kernel__)
	struct ifaddrs *ifap, *ifa;
	struct in_addr v4_addr;
	struct in6_addr v6_addr;
	int ifname_found = 0;
	int valid_addr_found = 0;

	/* Copy interface name, if it was provided. */
	if (IfName != NULL) {
		if (strlen(IfName) > sizeof(gIF_NAME))
			return UPNP_E_INVALID_INTERFACE;

		strncpy(gIF_NAME, IfName, sizeof(gIF_NAME));
		ifname_found = 1;
	}
	/* Get system interface addresses. */
	if (getifaddrs(&ifap) != 0) {
		UpnpPrintf(UPNP_CRITICAL, API, __FILE__, __LINE__,
			   "getifaddrs failed to find list of addresses\n");
		return UPNP_E_INIT;
	}
	/* cycle through available interfaces and their addresses. */
	for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
		/* Skip LOOPBACK interfaces, DOWN interfaces and interfaces that  */
		/* don't support MULTICAST. */
		if ((ifa->ifa_flags & IFF_LOOPBACK)
		    || (!(ifa->ifa_flags & IFF_UP))
		    || (!(ifa->ifa_flags & IFF_MULTICAST))) {
			continue;
		}
		if (ifname_found == 0) {
			/* We have found a valid interface name. Keep it. */
			strncpy(gIF_NAME, ifa->ifa_name, sizeof(gIF_NAME));
			ifname_found = 1;
		} else {
			if (strncmp(gIF_NAME, ifa->ifa_name, sizeof(gIF_NAME))
			    != 0) {
				/* This is not the interface we're looking for. */
				continue;
			}
		}
		/* Keep interface addresses for later. */
		switch (ifa->ifa_addr->sa_family) {
		case AF_INET:
			memcpy(&v4_addr,
			       &((struct sockaddr_in *)(ifa->ifa_addr))->
			       sin_addr, sizeof(v4_addr));
			valid_addr_found = 1;
			break;
#if UPNP_ENABLE_IPV6
		case AF_INET6:
			/* Only keep IPv6 link-local addresses. */
			if (IN6_IS_ADDR_LINKLOCAL
			    (&((struct sockaddr_in6 *)(ifa->ifa_addr))->
			     sin6_addr)) {
				memcpy(&v6_addr,
				       &((struct sockaddr_in6 *)(ifa->
								 ifa_addr))->
				       sin6_addr, sizeof(v6_addr));
				valid_addr_found = 1;
			}
			break;
#endif
		default:
			if (valid_addr_found == 0) {
				/* Address is not IPv4 or IPv6 and no valid address has  */
				/* yet been found for this interface. Discard interface name. */
				ifname_found = 0;
			}
			break;
		}
	}
	freeifaddrs(ifap);
	/* Failed to find a valid interface, or valid address. */
	if (ifname_found == 0 || valid_addr_found == 0) {
		UpnpPrintf(UPNP_CRITICAL, API, __FILE__, __LINE__,
			   "Failed to find an adapter with valid IP addresses for use.\n");
		return UPNP_E_INVALID_INTERFACE;
	}
	inet_ntop(AF_INET, &v4_addr, gIF_IPV4, sizeof(gIF_IPV4));
	inet_ntop(AF_INET6, &v6_addr, gIF_IPV6, sizeof(gIF_IPV6));
	gIF_INDEX = if_nametoindex(gIF_NAME);
#elif defined(HAVE_IOCTL_SIOCGIFCONF)
	char szBuffer[MAX_INTERFACES * sizeof(struct ifreq)];
	struct ifconf ifConf;
	struct ifreq ifReq;
	size_t i;
	int LocalSock;
#if UPNP_ENABLE_IPV6
	unsigned if_idx;
	FILE *inet6_procfd;
	struct in6_addr v6_addr;
	char addr6[8][5];
	char buf[65];		/* INET6_ADDRSTRLEN */
#endif
	int ifname_found = 0;
	int valid_addr_found = 0;

	/* Copy interface name, if it was provided. */
	if (IfName != NULL) {
		if (strlen(IfName) > sizeof(gIF_NAME))
			return UPNP_E_INVALID_INTERFACE;

		strncpy(gIF_NAME, IfName, sizeof(gIF_NAME));
		ifname_found = 1;
	}
	/* Create an unbound datagram socket to do the SIOCGIFADDR ioctl on.  */
	if ((LocalSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
			   "Can't create addrlist socket\n");
		return UPNP_E_INIT;
	}
	/* Get the interface configuration information...  */
	ifConf.ifc_len = sizeof szBuffer;
	ifConf.ifc_ifcu.ifcu_buf = (caddr_t) szBuffer;

	if (ioctl(LocalSock, SIOCGIFCONF, &ifConf) < 0) {
		UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
			   "DiscoverInterfaces: SIOCGIFCONF returned error\n");
		return UPNP_E_INIT;
	}
	/* Cycle through the list of interfaces looking for IP addresses.  */
	for (i = 0; i < (size_t)ifConf.ifc_len;) {
		struct ifreq *pifReq =
		    (struct ifreq *)((caddr_t) ifConf.ifc_req + i);
		i += sizeof *pifReq;
		/* See if this is the sort of interface we want to deal with. */
		strcpy(ifReq.ifr_name, pifReq->ifr_name);
		if (ioctl(LocalSock, SIOCGIFFLAGS, &ifReq) < 0) {
			UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
				   "Can't get interface flags for %s:\n",
				   ifReq.ifr_name);
		}
		/* Skip LOOPBACK interfaces, DOWN interfaces and interfaces that  */
		/* don't support MULTICAST. */
		if ((ifReq.ifr_flags & IFF_LOOPBACK)
		    || (!(ifReq.ifr_flags & IFF_UP))
		    || (!(ifReq.ifr_flags & IFF_MULTICAST))) {
			continue;
		}
		if (ifname_found == 0) {
			/* We have found a valid interface name. Keep it. */
			strncpy(gIF_NAME, pifReq->ifr_name, sizeof(gIF_NAME));
			ifname_found = 1;
		} else {
			if (strncmp
			    (gIF_NAME, pifReq->ifr_name,
			     sizeof(gIF_NAME)) != 0) {
				/* This is not the interface we're looking for. */
				continue;
			}
		}
		/* Check address family. */
		if (pifReq->ifr_addr.sa_family == AF_INET) {
			/* Copy interface name, IPv4 address and interface index. */
			strncpy(gIF_NAME, pifReq->ifr_name, sizeof(gIF_NAME));
			inet_ntop(AF_INET,
				  &((struct sockaddr_in *)&pifReq->ifr_addr)->
				  sin_addr, gIF_IPV4, sizeof(gIF_IPV4));
			gIF_INDEX = if_nametoindex(gIF_NAME);
			valid_addr_found = 1;
			break;
		} else {
			/* Address is not IPv4 */
			ifname_found = 0;
		}
	}
	close(LocalSock);
	/* Failed to find a valid interface, or valid address. */
	if (ifname_found == 0 || valid_addr_found == 0) {
		UpnpPrintf(UPNP_CRITICAL, API, __FILE__, __LINE__,
			   "Failed to find an adapter with valid IP addresses for use.\n");

		return UPNP_E_INVALID_INTERFACE;
	}
#if UPNP_ENABLE_IPV6
	/* Try to get the IPv6 address for the same interface  */
	/* from "/proc/net/if_inet6", if possible. */
	inet6_procfd = fopen("/proc/net/if_inet6", "r");
	if (inet6_procfd) {
		while (fscanf(inet6_procfd,
			      "%4s%4s%4s%4s%4s%4s%4s%4s %02x %*02x %*02x %*02x %*20s\n",
			      addr6[0], addr6[1], addr6[2], addr6[3],
			      addr6[4], addr6[5], addr6[6], addr6[7],
			      &if_idx) != EOF) {
			/* Get same interface as IPv4 address retrieved. */
			if (gIF_INDEX == if_idx) {
				snprintf(buf, sizeof(buf),
					 "%s:%s:%s:%s:%s:%s:%s:%s", addr6[0],
					 addr6[1], addr6[2], addr6[3], addr6[4],
					 addr6[5], addr6[6], addr6[7]);
				/* Validate formed address and check for link-local. */
				if (inet_pton(AF_INET6, buf, &v6_addr) > 0) {
					if (IN6_IS_ADDR_ULA(&v6_addr)) {
						/* Got valid IPv6 ula. */
						strncpy(gIF_IPV6_ULA_GUA, buf,
							sizeof
							(gIF_IPV6_ULA_GUA));
					} else if (IN6_IS_ADDR_GLOBAL(&v6_addr)
						   && strlen(gIF_IPV6_ULA_GUA)
						   == 0) {
						/* got a GUA, should store it while no ULA is found */
						strncpy(gIF_IPV6_ULA_GUA, buf,
							sizeof
							(gIF_IPV6_ULA_GUA));
					} else
					    if (IN6_IS_ADDR_LINKLOCAL(&v6_addr)
						&& strlen(gIF_IPV6) == 0) {
						/* got a Link local IPv6 address. */
						strncpy(gIF_IPV6, buf,
							sizeof(gIF_IPV6));
					}
				}
			}
		}
		fclose(inet6_procfd);
	}
#endif
#else
	(void)IfName;
	gIF_NAME[0] = '\0';
	gIF_INDEX = 0;
	gethostname(gIF_IPV4, sizeof(gIF_IPV4));
	gIF_IPV6[0] = '\0';
	gIF_IPV6_ULA_GUA[0] = '\0';
#endif
	UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
		   "Interface name=%s, index=%d, v4=%s, v6=%s, ULA or GUA v6=%s\n",
		   gIF_NAME, gIF_INDEX, gIF_IPV4, gIF_IPV6, gIF_IPV6_ULA_GUA);

	return UPNP_E_SUCCESS;
}

/*!
 * \brief Get callback function ptr from a handle.
 *
 * \return Upnp_FunPtr
 */
Upnp_FunPtr GetCallBackFn(UpnpClient_Handle Hnd)
{
	return ((struct Handle_Info *)HandleTable[Hnd])->Callback;
}


/* Assumes at most one client */
Upnp_Handle_Type GetClientHandleInfo(
	UpnpClient_Handle *client_handle_out,
	struct Handle_Info **HndInfo)
{
	Upnp_Handle_Type ret = HND_CLIENT;
	UpnpClient_Handle client;

	if (GetHandleInfo(1, HndInfo) == HND_CLIENT) {
		client = 1;
	} else if (GetHandleInfo(2, HndInfo) == HND_CLIENT) {
		client = 2;
	} else {
		client = -1;
		ret = HND_INVALID;
	}

	*client_handle_out = client;
	return ret;
}


Upnp_Handle_Type GetDeviceHandleInfo(
	const int AddressFamily,
	UpnpDevice_Handle *device_handle_out,
	struct Handle_Info **HndInfo)
{
	/* Check if we've got a registered device of the address family specified. */
	if ((AddressFamily == AF_INET  && UpnpSdkDeviceRegisteredV4 == 0) ||
	    (AddressFamily == AF_INET6 && UpnpSdkDeviceregisteredV6 == 0)) {
		*device_handle_out = -1;
		return HND_INVALID;
	}

	/* Find it. */
	for (*device_handle_out=1; *device_handle_out < NUM_HANDLE; (*device_handle_out)++) {
		if (GetHandleInfo(*device_handle_out, HndInfo) == HND_DEVICE) {
			if ((*HndInfo)->DeviceAf == AddressFamily) {
				return HND_DEVICE;
			}
		}
	}

	*device_handle_out = -1;
	return HND_INVALID;
}


Upnp_Handle_Type GetHandleInfo(
	UpnpClient_Handle Hnd,
	struct Handle_Info **HndInfo)
{
	Upnp_Handle_Type ret = HND_INVALID;

	UpnpPrintf( UPNP_INFO, API, __FILE__, __LINE__,
		"GetHandleInfo: entering, Handle is %d\n", Hnd);

	if (Hnd < 1 || Hnd >= NUM_HANDLE) {
		UpnpPrintf(UPNP_INFO, API, __FILE__, __LINE__,
			"GetHandleInfo: Handle out of range\n");
	} else if (HandleTable[Hnd] == NULL) {
		UpnpPrintf(UPNP_CRITICAL, API, __FILE__, __LINE__,
			"GetHandleInfo: HandleTable[%d] is NULL\n",
			Hnd);
	} else if (HandleTable[Hnd] != NULL) {
		*HndInfo = (struct Handle_Info *)HandleTable[Hnd];
		ret = ((struct Handle_Info *)*HndInfo)->HType;
	}

	UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__, "GetHandleInfo: exiting\n");

	return ret;
}


int PrintHandleInfo(UpnpClient_Handle Hnd)
{
    struct Handle_Info * HndInfo;
    if (HandleTable[Hnd] != NULL) {
        HndInfo = HandleTable[Hnd];
            UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
                "Printing information for Handle_%d\n", Hnd);
            UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
                "HType_%d\n", HndInfo->HType);
#ifdef INCLUDE_DEVICE_APIS
                if(HndInfo->HType != HND_CLIENT)
                    UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
                        "DescURL_%s\n", HndInfo->DescURL);
#endif /* INCLUDE_DEVICE_APIS */
    } else {
        return UPNP_E_INVALID_HANDLE;
    }

    return UPNP_E_SUCCESS;
}


int getlocalhostname(char *out, size_t out_len)
{
	int ret = UPNP_E_SUCCESS;
	char tempstr[16];
	const char *p = NULL;

#if defined(WIN32) || !defined(HAVE_IOCTL_SIOCGIFCONF)
	struct hostent *h = NULL;
	struct sockaddr_in LocalAddr;

	gethostname(out, out_len);
	h = gethostbyname(out);
	if (h != NULL) {
		memcpy(&LocalAddr.sin_addr, h->h_addr_list[0], 4);
		p = inet_ntop(AF_INET, &LocalAddr.sin_addr, tempstr, sizeof(tempstr));
		if (p) {
			strncpy(out, p, out_len);
		} else {
			UpnpPrintf( UPNP_ALL, API, __FILE__, __LINE__,
				"getlocalhostname: inet_ntop returned error\n" );
			ret = UPNP_E_INIT;
		}
	} else {
		UpnpPrintf( UPNP_ALL, API, __FILE__, __LINE__,
			"getlocalhostname: gethostbyname returned error\n" );
		ret = UPNP_E_INIT;
	}

#elif (defined(BSD) && BSD >= 199306) || defined(__FreeBSD_kernel__)
	struct ifaddrs *ifap, *ifa;

	if (getifaddrs(&ifap) != 0) {
		UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
			"DiscoverInterfaces: getifaddrs() returned error\n");
		return UPNP_E_INIT;
	}

	/* cycle through available interfaces */
	for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
		/* Skip loopback, point-to-point and down interfaces,
		 * except don't skip down interfaces
		 * if we're trying to get a list of configurable interfaces. */
		if ((ifa->ifa_flags & IFF_LOOPBACK) ||
		    (!( ifa->ifa_flags & IFF_UP))) {
			continue;
		}
		if (ifa->ifa_addr->sa_family == AF_INET) {
			/* We don't want the loopback interface. */
			if (((struct sockaddr_in *)(ifa->ifa_addr))->sin_addr.s_addr ==
			    htonl(INADDR_LOOPBACK)) {
				continue;
			}
			p = inet_ntop(AF_INET,
				&((struct sockaddr_in *)(ifa->ifa_addr))->sin_addr,
				tempstr, sizeof(tempstr));
			if (p) {
				strncpy(out, p, out_len);
			} else {
				UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
					"getlocalhostname: inet_ntop returned error\n");
				ret = UPNP_E_INIT;
			}
			UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
				"Inside getlocalhostname: after strncpy %s\n", out);
			break;
		}
	}
	freeifaddrs(ifap);

	ret = ifa ? UPNP_E_SUCCESS : UPNP_E_INIT;
#else
	char szBuffer[MAX_INTERFACES * sizeof (struct ifreq)];
	struct ifconf ifConf;
	struct ifreq ifReq;
	int nResult;
	long unsigned int i;
	int LocalSock;
	struct sockaddr_in LocalAddr;
	int j = 0;

	/* purify */
	memset(&ifConf,  0, sizeof(ifConf));
	memset(&ifReq,   0, sizeof(ifReq));
	memset(szBuffer, 0, sizeof(szBuffer));

	/* Create an unbound datagram socket to do the SIOCGIFADDR ioctl on.  */
	LocalSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (LocalSock < 0) {
		UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
			"Can't create addrlist socket\n");
		return UPNP_E_INIT;
	}
	/* Get the interface configuration information... */
	ifConf.ifc_len = sizeof szBuffer;
	ifConf.ifc_ifcu.ifcu_buf = (caddr_t) szBuffer;
	nResult = ioctl(LocalSock, SIOCGIFCONF, &ifConf);
	if (nResult < 0) {
		UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
			"DiscoverInterfaces: SIOCGIFCONF returned error\n");

		return UPNP_E_INIT;
	}

	/* Cycle through the list of interfaces looking for IP addresses. */
	for (i = 0; i < (long unsigned int)ifConf.ifc_len && j < DEFAULT_INTERFACE; ) {
		struct ifreq *pifReq =
			(struct ifreq *)((caddr_t)ifConf.ifc_req + i);
		i += sizeof *pifReq;
		/* See if this is the sort of interface we want to deal with. */
		strcpy(ifReq.ifr_name, pifReq->ifr_name);
		if (ioctl(LocalSock, SIOCGIFFLAGS, &ifReq) < 0) {
			UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
				"Can't get interface flags for %s:\n",
				ifReq.ifr_name);
		}
		/* Skip loopback, point-to-point and down interfaces,
		 * except don't skip down interfaces
		 * if we're trying to get a list of configurable interfaces. */
		if ((ifReq.ifr_flags & IFF_LOOPBACK) ||
		    (!(ifReq.ifr_flags & IFF_UP))) {
			continue;
		}
		if (pifReq->ifr_addr.sa_family == AF_INET) {
			/* Get a pointer to the address...*/
			memcpy(&LocalAddr, &pifReq->ifr_addr,
				sizeof pifReq->ifr_addr);
			/* We don't want the loopback interface. */
			if (LocalAddr.sin_addr.s_addr ==
			    htonl(INADDR_LOOPBACK)) {
				continue;
			}
		}
		/* increment j if we found an address which is not loopback
		 * and is up */
		j++;
	}
	close(LocalSock);

	p = inet_ntop(AF_INET, &LocalAddr.sin_addr, tempstr, sizeof(tempstr));
	if (p) {
		strncpy(out, p, out_len);
	} else {
		UpnpPrintf( UPNP_ALL, API, __FILE__, __LINE__,
			"getlocalhostname: inet_ntop returned error\n" );
		ret = UPNP_E_INIT;
	}
	UpnpPrintf(UPNP_ALL, API, __FILE__, __LINE__,
		"Inside getlocalhostname: after strncpy %s\n", out);
#endif
	return ret;
}


#ifdef INCLUDE_DEVICE_APIS
#if EXCLUDE_SSDP == 0
void AutoAdvertise(void *input)
{
	upnp_timeout *event = (upnp_timeout *)input;

	UpnpSendAdvertisement(event->handle, *((int *)event->Event));
	free_upnp_timeout(event);
}
#endif /* EXCLUDE_SSDP == 0 */
#endif /* INCLUDE_DEVICE_APIS */

int UpnpSetContentLength(UpnpClient_Handle Hnd, size_t contentLength)
{
	int errCode = UPNP_E_SUCCESS;
	struct Handle_Info *HInfo = NULL;

	do {
		if (UpnpSdkInit != 1) {
			errCode = UPNP_E_FINISH;
			break;
		}

		HandleLock();

		errCode = GetHandleInfo(Hnd, &HInfo);
		if (errCode != HND_DEVICE) {
			errCode = UPNP_E_INVALID_HANDLE;
			break;
		}
		if (contentLength > MAX_CONTENT_LENGTH) {
			errCode = UPNP_E_OUTOF_BOUNDS;
			break;
		}
		g_maxContentLength = contentLength;
	} while (0);

	HandleUnlock();
	return errCode;
}

int UpnpSetMaxContentLength(size_t contentLength)
{
	int errCode = UPNP_E_SUCCESS;

	do {
		if (UpnpSdkInit != 1) {
			errCode = UPNP_E_FINISH;
			break;
		}
		g_maxContentLength = contentLength;
	} while(0);

	return errCode;
}

int UpnpSetDeviceReplyHeaders(UpnpDevice_Handle Hnd,
                              const char *apReplyHeaders[],
                              int replyHeaderCount)
{
	int errCode = UPNP_E_SUCCESS;
	struct Handle_Info *HInfo = NULL;
    int i;
    char *header;
    ListNode *node;

    if(replyHeaderCount > 0 && !apReplyHeaders) {
        return UPNP_E_INVALID_PARAM;
    }

	do {
		if (UpnpSdkInit != 1) {
			errCode = UPNP_E_FINISH;
			break;
		}

		HandleLock();

		errCode = GetHandleInfo(Hnd, &HInfo);
		if (errCode != HND_DEVICE) {
			errCode = UPNP_E_INVALID_HANDLE;
			break;
		}

        errCode = UPNP_E_SUCCESS;

        /* remove existing headers */
        node = ListHead(&HInfo->DeviceReplyHeaderList);
        while (node != NULL) {
            header = (char *)node->item;
            if (header) {
                free(header);
            }
            ListDelNode(&HInfo->DeviceReplyHeaderList, node, 0);
            node = ListHead(&HInfo->DeviceReplyHeaderList);
        }

        /* add new headers */
        for(i = 0; i < replyHeaderCount; i++)
        {
            header = (char *)malloc(strlen(apReplyHeaders[i]) + 1);
            if(!header) {
                return UPNP_E_OUTOF_MEMORY;
            }
            strcpy(header, apReplyHeaders[i]);
            ListAddTail(&HInfo->DeviceReplyHeaderList, header);
        }
	} while (0);

	HandleUnlock();
	return errCode;
}
/* @} UPnPAPI */
