/**
 * vk_event.c
*/

/**
 * @defgroup		VK_EVENT
 * @author		Tae-Hyuk Kang
 * @note			Virsual Event APIs
 * @file 			vk_event.c
 * @remarks		Copyright (c) 2008 HUMAX Co., Ltd. All rights reserved.
*/

/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include "vkernel.h"

#if defined(__mips__)
#include <linux/version.h>
/* Broadcom integrated NPTL with our Linux 2.6.18 release. */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18) && 0
#define HAS_NPTL 1
#endif /* LINUX_VERSION */
#endif /* __mips__ */

#if !HAS_NPTL
#include <sys/time.h>
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE			TRACE_MODULE_VK_EVENT

#ifdef EVENT_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)
#endif /* EVENT_DEBUG */

#define MAX_EVENT_NAME		16

/***************************************************************************
Summary:
	BKNI_INFINITE can be passed as a timeout value into BKNI_WaitForEvent
	for multi-threaded modules.
****************************************************************************/
#define B_VK_INFINITE -1

/***************************************************************************
Summary:
	Standard error code type.

Description:
	This error code may be a module specific error code created with
	BERR_MAKE_CODE or be one of the standard error codes:
	o BERR_SUCCESS
	o BERR_NOT_INITIALIZED
	o BERR_INVALID_PARAMETER
	o BERR_OUT_OF_SYSTEM_MEMORY
	o BERR_OUT_OF_DEVICE_MEMORY
	o BERR_TIMEOUT
	o BERR_OS_ERROR
	o BERR_LEAKED_RESOURCE
	o BERR_NOT_SUPPORTED
	o BERR_UNKNOWN
****************************************************************************/
typedef unsigned int BERR_Code;

/* standard error codes */
#define BERR_SUCCESS              0  /* success (always zero) */
#define BERR_NOT_INITIALIZED      1  /* parameter not initialized */
#define BERR_INVALID_PARAMETER    2  /* parameter is invalid */
#define BERR_OUT_OF_SYSTEM_MEMORY 3  /* out of KNI module memory */
#define BERR_OUT_OF_DEVICE_MEMORY 4  /* out of MEM module memory */
#define BERR_TIMEOUT              5  /* reached timeout limit */
#define BERR_OS_ERROR             6  /* generic OS error */
#define BERR_LEAKED_RESOURCE      7  /* resource being freed has attached
                                        resources that haven't been freed */
#define BERR_NOT_SUPPORTED 		  8  /* requested feature is not supported */
#define BERR_UNKNOWN              9  /* unknown */

/* {private} error code masks */
#define BERR_P_ID_MASK  UINT32_C(0xFFFF0000)   /* {private} */
#define BERR_P_ID_SHIFT  16                    /* {private} */
#define BERR_P_NUM_MASK  UINT32_C(0x0000FFFF)  /* {private} */

/**
BKNI_TRACK_MALLOCS is a simple way to track BKNI_Malloc memory leaks and bad BKNI_Frees.
It can also help find the location of bad BKNI_EventHandle and BKNI_MutexHandle instances.
**/
#ifndef BKNI_TRACK_MALLOCS
#if defined(LINUX) && !defined(__KERNEL__) && BDBG_DEBUG_BUILD
#define BKNI_TRACK_MALLOCS 1
#else
#define BKNI_TRACK_MALLOCS 0
#endif
#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef unsigned char bool;
#define false 0
#define true 1

struct B_VK_GroupObj
{
	pthread_mutex_t lock;							/* mutex for protecting signal and conditional variables */
	pthread_cond_t cond;							/* condition to wake up from event */
};

struct B_VK_EventObj
{
	struct B_VK_GroupObj *group;
	pthread_mutex_t lock;							/* mutex for protecting signal and conditional variables */
	pthread_cond_t	cond;							/* condition to wake up from event */
	bool signal;
};
/**************************************************************************
Summary:
 Event group handle
**************************************************************************/
typedef struct B_VK_GroupObj *B_VK_EventGroupHandle;
/***************************************************************************
Summary:
    Event handle created by BKNI_CreateEvent.
****************************************************************************/
typedef struct B_VK_EventObj *B_VK_EventHandle;

typedef struct {
	B_VK_EventHandle		handle;
	unsigned long			event;
	char					event_name[MAX_EVENT_NAME];
} VK_EVENT;

/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

/********************************************************************
 External Variable Declaration
 ********************************************************************/

/****************************************************************************
    Data Structures
****************************************************************************/

/********************************************************************
 Static Function
 ********************************************************************/

BERR_Code B_VK_CreateEvent_tagged(B_VK_EventHandle *pEvent, const char *file, int line)
{
	B_VK_EventHandle event;
	int rc;
	BERR_Code result=BERR_SUCCESS;
	/* coverity[var_dec1: FALSE */
	pthread_condattr_t attr;

	HAPPY(file);
	HAPPY(line);

	event = DD_MEM_Alloc(sizeof(*event));
	*pEvent = event;
	if ( !event ) {
		result = BERR_OS_ERROR;
		goto err_no_memory;
	}

	rc = pthread_mutex_init (&event->lock, NULL /* default attributes */);
	if (rc != 0) {
		result = BERR_OS_ERROR;
		goto err_mutex;
	}

	/* coverity[uninit_use_in_call: FALSE] */
	rc = pthread_condattr_init(&attr);
	if (rc != 0 ) {
		result = BERR_OS_ERROR;
		goto err_condvar;
	}

#if HAS_NPTL
	rc = pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
	if (rc != 0) {
		result = BERR_OS_ERROR;
		goto err_condvar;
	}
#endif

	rc = pthread_cond_init(&event->cond, &attr);
	if (rc != 0) {
		result = BERR_OS_ERROR;
		goto err_condvar;
	}
	event->signal = false;
	event->group = NULL;

	return result;

err_condvar:
	pthread_mutex_destroy(&event->lock);
err_mutex:
	free(event);
err_no_memory:
	return result;
}

BERR_Code B_VK_CreateEvent(B_VK_EventHandle *pEvent)
{
    return B_VK_CreateEvent_tagged(pEvent, NULL, 0);
}

void
B_VK_SetEvent(B_VK_EventHandle event)
{
	int rc;
	B_VK_EventGroupHandle group;

	group = event->group;
	/* At this point, we may have been removed from the group and event->group is NULL,
	This is a real possibility because BKNI_SetEvent can be called from an ISR.
	Caching the group pointer allows us to safely unlock still. */

	if (group) {
		rc = pthread_mutex_lock(&group->lock);
		if (rc != 0) {
			PrintError("pthread_mutex_lock(group): %d", rc);
		}
	}
	rc = pthread_mutex_lock(&event->lock);
	if (rc != 0) {
		PrintError("pthread_mutex_lock: %d", rc);
	}
	event->signal = true;
	rc = pthread_cond_signal(&event->cond);
	if (rc != 0) {
		PrintError("pthread_cond_signal: %d", rc);
	}
	if (group) {
        rc = pthread_cond_signal(&group->cond);
        if (rc!=0) {
      		PrintError("pthread_cond_signal: %d, ignored", rc);
        }
    }
	rc = pthread_mutex_unlock(&event->lock);
	if (rc != 0) {
		PrintError("pthread_mutex_unlock: %d", rc);
	}
	if (group) {
		pthread_mutex_unlock(&group->lock);
	}
	return ;
}

/* return a timespec which is the current time plus an increment */
static int B_VK_P_SetTargetTime(struct timespec *target, int timeoutMsec)
{
	int rc;
#if !HAS_NPIL
	/* Unless pthread can set CLOCK_MONOTONIC, we cannot use clock_gettime(CLOCK_MONOTONIC). This is only available with NPIL linux. */
	struct timeval now;
	rc = gettimeofday(&now, NULL);
	if (rc != 0) {
		return BERR_OS_ERROR;
	}
	target->tv_nsec = now.tv_usec * 1000 + (timeoutMsec%1000)*1000000;
	target->tv_sec = now.tv_sec + (timeoutMsec/1000);
	if (target->tv_nsec >= 1000000000) {
		target->tv_nsec -= 1000000000;
		target->tv_sec ++;
	}
#else
	struct timespec now;
	rc = clock_gettime(CLOCK_MONOTONIC, &now);
	if (rc != 0) {
		return BERR_OS_ERROR;
	}
	target->tv_nsec = now.tv_nsec + (timeoutMsec%1000)*1000000;
	target->tv_sec = now.tv_sec + (timeoutMsec/1000);
	if (target->tv_nsec >= 1000000000) {
		target->tv_nsec -= 1000000000;
		target->tv_sec ++;
	}
#endif
	return 0;
}

BERR_Code B_VK_WaitForEvent(B_VK_EventHandle event, int timeoutMsec)
{
	int rc;
	BERR_Code result = BERR_SUCCESS;
	struct timespec target;

	if (timeoutMsec != 0)
	{
		//ASSERT_NOT_CRITICAL();
	}

	if (timeoutMsec != 0 && timeoutMsec != B_VK_INFINITE) {
		if (timeoutMsec < 0) {
			/* If your app is written to allow negative values to this function, then it's highly likely you would allow -1, which would
			result in an infinite hang. We recommend that you only pass positive values to this function unless you definitely mean BKNI_INFINITE. */
			PrintDebug("BKNI_WaitForEvent given negative timeout. Possible infinite hang if timeout happens to be -1 (BKNI_INFINITE). timeoutMsec = %d", timeoutMsec);
		}
		if (timeoutMsec < 100) {
			timeoutMsec = 100; /* Wait at least 100 msec. If the kernel is busy servicing interrupts (e.g. heavy network traffic), it could starve
														even the highest priority user mode thread. BKNI_WaitForEvent is used inside Magnum and much of that code cannot survive a timeout.
														BKNI_WaitForGroup can have a smaller timeout because it's used outside of Magnum (e.g. Nexus) and timeouts are normal. */
		}
		rc = B_VK_P_SetTargetTime(&target, timeoutMsec);
		if (rc) {
			return BERR_OS_ERROR;
		}
	}

	rc = pthread_mutex_lock(&event->lock);
	if (rc != 0) {
		return BERR_OS_ERROR;
	}
	if (event->signal) {
		event->signal = false;
		goto done;
	}
	if (timeoutMsec == 0) { /* wait without timeout */
		/* It is normal that BKNI_WaitForEvent could time out. Do not use BERR_TRACE. */
		result = BERR_TIMEOUT;
		goto done;
	}
	do {
		if (timeoutMsec == B_VK_INFINITE) {
			rc = pthread_cond_wait(&event->cond, &event->lock);
		} else {
			rc = pthread_cond_timedwait(&event->cond, &event->lock, &target);
			if (rc == ETIMEDOUT) {
				/*PrintError("BKNI_WaitForEvent(%#x): timeout", (unsigned)(unsigned long)event);*/
				result = BERR_TIMEOUT;
				goto done;
			}
		}
		if (rc == EINTR) {
			PrintError("BKNI_WaitForEvent(%#x): interrupted", (unsigned)(unsigned long)event);
			continue;
		}
		if (rc != 0) {
			result = BERR_OS_ERROR;
			goto done;
		}
	} while (!event->signal);	/* we might have been wokenup and then event has been cleared */

	event->signal = false;
done:
	pthread_mutex_unlock(&event->lock);
	return result;
}

void
B_VK_DestroyEvent_tagged(B_VK_EventHandle event, const char *file, int line)
{
	int rc;
	B_VK_EventGroupHandle group;

	HAPPY(file);
	HAPPY(line);

	group = event->group;
	/* At this point, we may have been removed from the group and event->group is NULL.
	This would be poor application code, but KNI should protect itself. */

	if (group) {
		PrintDebug("Event %#x still in the group %#x, removing it", (unsigned)(unsigned long)event, (unsigned)(unsigned long)group);
		rc = pthread_mutex_lock(&group->lock);
		if (rc != 0) {
			PrintError("pthread_mutex_lock %d", rc);
		}
		/* if the group does not match, then the caller needs to fix their code. we can't have an event being added & removed from various
		groups and being destroyed at the same time. */
		pthread_mutex_unlock(&group->lock);
	}
	rc = pthread_mutex_destroy(&event->lock);
	if (rc != 0) {
		PrintError("pthread_mutex_destroy: %d", rc);
	}
	rc = pthread_cond_destroy(&event->cond);
	if (rc != 0) {
		PrintError("pthread_cond_destroy: %d", rc);
	}

	DD_MEM_Free(event);
	return;
}

void B_VK_DestroyEvent(B_VK_EventHandle event)
{
	B_VK_DestroyEvent_tagged(event, NULL, 0);
}

/********************************************************************
 Global Function
 ********************************************************************/

/********************************************************************
 function: 	VK_EVENT_Create
 description :	create a event
 argument:
 return:
*********************************************************************/
int	VK_EVENT_Create(unsigned long *eventId, char eventName[])
{
	VK_EVENT *vk_event;	/* BKNI_EventHandle is pointer */
	BERR_Code returnCodeBcm;

	vk_event = DD_MEM_Alloc(sizeof(VK_EVENT));
	if (vk_event == NULL)
	{
		*eventId = 0;
		PrintError("Error::VK_EVENT_Create - DD_MEM_Alloc error vk_event = %p\n", vk_event);
		return VK_ALLOC_ERROR;
	}

	returnCodeBcm = B_VK_CreateEvent(&vk_event->handle);
	if (returnCodeBcm != BERR_SUCCESS)
	{
		*eventId = 0;
		DD_MEM_Free(vk_event);
		PrintError("Error::VK_EVENT_Create - error = %d\n", returnCodeBcm);
		return VK_ALLOC_ERROR;
	}

	vk_event->event = 0;

	if( eventName != NULL )
	{
		strncpy(vk_event->event_name, eventName, MAX_EVENT_NAME-1);
		vk_event->event_name[MAX_EVENT_NAME-1] = '\0';
	}
	else
	{
		vk_event->event_name[0] = 'e';
		vk_event->event_name[1] = 'v';
		vk_event->event_name[2] = 't';
		vk_event->event_name[3] = '\0';
	}

	*eventId = (unsigned long)vk_event;

	return VK_OK;
}

/********************************************************************
 function: 	VK_EVENT_Send
 description :	send a event
 argument:
 return:
*********************************************************************/
int	VK_EVENT_Send(unsigned long eventId, unsigned long event)
{
	VK_EVENT *vk_event = (VK_EVENT *)eventId;

	if (vk_event == NULL)
	{
		PrintError("Error::VK_EVENT_Send - error vk_event = %p\n", vk_event);
		return VK_ERROR;
	}

	vk_event->event = event;
	B_VK_SetEvent(vk_event->handle);

	return VK_OK;
}

/********************************************************************
 function: 	VK_EVENT_Receive
 description :	receive a event
 argument:
 return:
*********************************************************************/
int	VK_EVENT_Receive(unsigned long eventId,	unsigned long *event)
{
	VK_EVENT *vk_event = (VK_EVENT *)eventId;
	BERR_Code returnCodeBcm;

	if (vk_event == NULL)
	{
		PrintError("Error::VK_EVENT_Receive - error vk_event = %p\n", vk_event);
		return VK_ERROR;
	}

	returnCodeBcm = B_VK_WaitForEvent(vk_event->handle, B_VK_INFINITE);
	if( returnCodeBcm != BERR_SUCCESS )
	{
        PrintError("%s : Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);
		return VK_ERROR;
	}
	*event = vk_event->event;

	return VK_OK;
}

/********************************************************************
 function: 	VK_EVENT_ReceiveTimeout
 description :	receive a event
 argument:
 return:
*********************************************************************/
/* timeout 100ms 이하로는 설정할 수 없음. 100ms 이하로 설정해도 100ms로 처리됨. */
int	VK_EVENT_ReceiveTimeout(unsigned long eventId,	unsigned long *event, unsigned long timeout)
{
	VK_EVENT *vk_event = (VK_EVENT *)eventId;
	BERR_Code returnCodeBcm;

	if (vk_event == NULL)
	{
		PrintError("Error::VK_EVENT_ReceiveTimeout - error vk_event = %p\n", vk_event);
		return VK_ERROR;
	}

	if (event == NULL)
	{
		PrintError("Error::VK_EVENT_ReceiveTimeout - error vk_event = %p\n", vk_event);
		return VK_ERROR;
	}

	returnCodeBcm = B_VK_WaitForEvent(vk_event->handle, timeout);
	if( returnCodeBcm != BERR_SUCCESS )
	{
		if (returnCodeBcm == BERR_TIMEOUT)
		{
        	return VK_TIMEOUT;
        }
        else
        {
        	PrintError("%s : Line(%d), %s\n", __FUNCTION__, __LINE__, __FILE__);

			return VK_ERROR;
		}
	}
	*event = vk_event->event;

	return VK_OK;
}

/********************************************************************
 function: 	VK_EVENT_Destroy
 description :	destroy a event
 argument:
 return:
*********************************************************************/
int	VK_EVENT_Destroy(unsigned long eventId)
{
	VK_EVENT *vk_event = (VK_EVENT *)eventId;

	if (vk_event == NULL)
	{
		PrintError("Error::VK_EVENT_Destroy - error vk_event = %p\n", vk_event);
		return VK_ERROR;
	}

	B_VK_DestroyEvent(vk_event->handle);
	DD_MEM_Free(vk_event);

	return VK_OK;
}

