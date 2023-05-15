/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FVK_H_
#define _FVK_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "ftype.h"
#include "vkernel.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FVK_OK			=0,
	FVK_NO_ERR		=0,
	FVK_ERR
} FVK_ERR_CODE;

typedef enum
{
	FVK_TIMER_ONCE = 0,
	FVK_TIMER_REPEAT
} FVK_TIMER_MODE;

typedef FVK_ERR_CODE (*pfnFVK_TIMER_EVT_CALLBACK)(unsigned long ulTimerId, void *pvParams);

#define FVK_TASK_DEFAULT_PRIORITY 		FVK_TASK_GetDefaultPriority()
#define FVK_TASK_DEFAULT_STACK_SIZE 	FVK_TASK_GetDefaultStackSize()
#define FVK_TASK_LARGE_STACK_SIZE		FVK_TASK_GetLargeStackSize()
#define FVK_TASK_DEFAULT_QUEUE_SIZE 	FVK_TASK_GetDefaultQueueSize()

/******************************************************************************
* Global variables and structures
******************************************************************************/



/******************************************************************************
* External variables and functions
******************************************************************************/



/******************************************************************************
* Static variables and structures
******************************************************************************/



/******************************************************************************
* Static function prototypes
******************************************************************************/


/******************************************************************************
* function : FVK APIs for Memroy
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
void* FVK_MEM_Alloc(unsigned long ulSize);
void* FVK_MEM_AllocUncached(unsigned long ulSize, unsigned long alignsize);
FVK_ERR_CODE FVK_MEM_Free(void *p);
FVK_ERR_CODE FVK_MEM_FreeUncached(void *p);
void FVK_MEM_Copy(void *pDest, void *pSrc, unsigned int unSize);
void FVK_MEM_Set(void *pPtr, int nValue, unsigned int unSize);
int FVK_MEM_Compare(void *pPtr1, void *pPtr2, unsigned int unSize);





/******************************************************************************
* function : FVK APIs for Task
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_TASK_Create(void (*StartFunc)(void*), unsigned long ulPriority, unsigned long ulStackSize, char *szTaskName, void *vpArgs, unsigned long *pulTaskID);
FVK_ERR_CODE FVK_TASK_Start(unsigned long taskId);
FVK_ERR_CODE FVK_TASK_Sleep (unsigned long ulMsec);
unsigned long FVK_TASK_GetDefaultPriority(void);
unsigned long FVK_TASK_GetDefaultStackSize(void);
unsigned long FVK_TASK_GetDefaultQueueSize(void);





/******************************************************************************
* function : FVK APIs for Message
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_MSG_Create (unsigned long ulMsgQueueCount, unsigned long ulMsgSize, char szMsgQueueName[], unsigned long *pulMsgQueueID);
FVK_ERR_CODE FVK_MSG_Send (unsigned long ulMsgQueueID, void *pvMsg, unsigned long ulMsgSize);
FVK_ERR_CODE FVK_MSG_SendTimeout (unsigned long ulMsgQueueID, void *pvMsg, unsigned long ulMsgSize, unsigned long ulTimeOut);
FVK_ERR_CODE FVK_MSG_Receive (unsigned long ulMsgQueueID, void *pvMsg, unsigned long ulMsgSize);
FVK_ERR_CODE FVK_MSG_ReceiveTimeout (unsigned long ulMsgQueueID, void *pvMsg, unsigned long ulMsgSize, unsigned long ulTimeOut);





/******************************************************************************
* function : FVK APIs for Semaphore
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_SEM_Create(char *szSemName, unsigned long *pulSemID);
FVK_ERR_CODE FVK_SEM_CreateCount(char *szSemName, unsigned long ulIntialCount, unsigned long *pulSemID);
FVK_ERR_CODE FVK_SEM_Get(unsigned long ulSemID);
FVK_ERR_CODE FVK_SEM_GetTimeout(unsigned long ulSemID, unsigned long ulTimeoutMsec);
FVK_ERR_CODE FVK_SEM_Release(unsigned long ulSemID);
FVK_ERR_CODE FVK_SEM_GetCountvalue(unsigned long ulSemID, unsigned long* pulCountVal);





/******************************************************************************
* function : FVK APIs for Timer
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_TIMER_CreateStart(unsigned long ulTimeOut, pfnFVK_TIMER_EVT_CALLBACK pfnCallback, void *pArgs, unsigned long ulArgsSize, FVK_TIMER_MODE eMode, unsigned long *pulTimerId);
FVK_ERR_CODE FVK_TIMER_Destory(unsigned long ulTimerId);




#endif /* _FVK_H_ */

/* end of file */

