/**
	@file	  port_itk_os.h														\n
	@brief	  redkey2 porting header file for OS									\n

	Description: Redkey2에서 제공하는 OS Porting Layer에 관련된 내부 구현된 함수의 Prototypes \n
	Module: MW/mheg	/itk								 								\n

	Copyright (c) 2008 HUMAX Co., Ltd.												\n
	All rights reserved.															\n
*/

#ifndef _PORT_ITK_OS_H_
#define _PORT_ITK_OS_H_

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>

#include "itk.h"
#include "itk_os.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagITK_HeapHandle_t
{
	itk_heap_t					vtbl;								/* 실제로 itk Porting Parameter로 사용되는 변수 */
	size_t						used;								/* total size of used memory in this heap */
	size_t						size;								/* total size of heap */
	itk_heap_callback_t			callback;							/* callback function for dealing with error cases */
	void						*callback_context;

#ifdef DEBUG_HOST_OS_HEAP											/* memory heap debug functions */
	struct tagITK_HeapHandle_t	*pPrev;
	struct tagITK_HeapHandle_t	*pNext;
#endif

} ITK_HeapHandle_t;





/*******************************************************************/
/********************      Public Functions     ********************/
/*******************************************************************/

/**
  @remark   initialize Heap Object
  @remark	type : sync

  @return	pointer of heap instance if success, otherwise return NULL

*/
itk_heap_t* PORT_ITK_OS_NewHeap(struct itk_os_t* thiz, const char* name, size_t capacity,
								itk_heap_callback_t callback, void* callbackContext);


/**
  @remark   initialize new thread and start it.
  @remark	type : sync
            this function should be called in side of Redkey2 Library. therefore do not call in Host side.

  @return	pointer of thread instance if success, otherwise return NULL

*/
itk_thread_t* PORT_ITK_OS_NewThread(struct itk_os_t *thiz, const char *name, const itk_thread_entry_t entryPoint,
 										void *argv, size_t stackSize, itk_thread_priority_t priority);


/**
  @remark   initialize new OS Object and Create.
  @remark	type : sync
  @return	ERR_OK :
            ERR_FAIL;

*/
HERROR PORT_ITK_OS_CreateObject(void);


/**
  @remark   sleeps the current Thread.
  @remark	type : sync
            this function should be called in side of Redkey2 Library. therefore do not call in Host side.
            ms : input parameter whose value should be mili sconds
  @return	NONE:
*/
void PORT_ITK_OS_Sleep(struct itk_os_t* thiz, itk_uint32_t ms);


/**
  @remark   creates new semaphore
  @remark	type : sync
            this function should be called in side of Redkey2 Library. therefore do not call in Host side.
   @return	pointer of semaphore object if success,
            otherwise return NULL
*/
itk_semaphore_t* PORT_ITK_OS_NewSemaphore(struct itk_os_t* thiz, const char* name, itk_uint16_t value);

/**
  @remark   creates new timer
  @remark	type : sync
            this function should be called in side of Redkey2 Library. therefore do not call in Host side.
   @return	pointer of timer object if success,
            otherwise return NULL
*/
itk_timer_t* PORT_ITK_OS_NewTimer(struct itk_os_t* thiz, const char* name, itk_uint32_t countMs,
									itk_timer_callback_t callback, void* callbackContext);


/**
  @remark   get current running tread pointer
  @remark	type : sync
            this function should be called in side of Redkey2 Library. therefore do not call in Host side.
   @return	pointer of tread object if success,
            otherwise return NULL
*/
itk_thread_t* PORT_ITK_OS_GetRunningThread(struct itk_os_t* thiz);


/**
  @remark   initialize OS object variables
  @remark	type : sync
  @return	NONE

*/
void PORT_ITK_OS_Init(void);

/**
  @remark   register OS object to RedkeyLibrary.
  @remark	type : sync
  @return	NONE

*/
void PORT_ITK_OS_RegisterObject(void);

#endif	/* port_itk_os */




