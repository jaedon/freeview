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
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <pthread.h>
#include <errno.h>

#include "vkernel.h"

#include <linden_trace.h>

#if defined(__mips__)
#include <linux/version.h>
/* Broadcom integrated NPTL with our Linux 2.6.18 release. */
#if !defined(CONFIG_OS_UCOS)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18) && 0
#define HAS_NPTL 1
#endif /* LINUX_VERSION */
#endif
#endif /* __mips__ */

#if !HAS_NPTL
#include <sys/time.h>
#endif

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE			TRACE_MODULE_VK_EVENT

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


	HAPPY(file);
	HAPPY(line);
	HAPPY(pEvent);

	return BERR_SUCCESS;
}

BERR_Code B_VK_CreateEvent(B_VK_EventHandle *pEvent)
{
	HAPPY(pEvent);

	return BERR_SUCCESS;

}

void B_VK_SetEvent(B_VK_EventHandle event)
{
	HAPPY(event);

}

BERR_Code B_VK_WaitForEvent(B_VK_EventHandle event, int timeoutMsec)
{
	HAPPY(event);
	HAPPY(timeoutMsec);

	return BERR_SUCCESS;
}

void B_VK_DestroyEvent_tagged(B_VK_EventHandle event, const char *file, int line)
{

	HAPPY(event);
	HAPPY(file);
	HAPPY(line);

}

void B_VK_DestroyEvent(B_VK_EventHandle event)
{
	HAPPY(event);
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
	HAPPY(eventId);
	HAPPY(eventName);
	PrintDebug("%s(%d) : Not Support Yet\n", __FUNCTION__, __LINE__);
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
	HAPPY(eventId);
	HAPPY(event);
	PrintDebug("%s(%d) : Not Support Yet\n", __FUNCTION__, __LINE__);
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
	HAPPY(eventId);
	HAPPY(event);
	PrintDebug("%s(%d) : Not Support Yet\n", __FUNCTION__, __LINE__);
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
	HAPPY(eventId);
	HAPPY(event);
	HAPPY(timeout);
	PrintDebug("%s(%d) : Not Support Yet\n", __FUNCTION__, __LINE__);
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
	HAPPY(eventId);
	PrintDebug("%s(%d) : Not Support Yet\n", __FUNCTION__, __LINE__);
	return VK_OK;
}

