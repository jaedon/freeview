/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author : 
* description :
*
* Copyright (c) 2009 by Humax Co., Ltd.
* All right reserved
******************************************************************************/

/******************************************************************************
* header and definitions
******************************************************************************/
/* humax header files */
#include "taskdef.h"
#include "vkernel.h"
#include "fvk.h"


/******************************************************************************
* Debug level
******************************************************************************/

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/


/******************************************************************************
* Global variables and structures
******************************************************************************/

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */

/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/******************************************************************************
* function : FVK_TASK_Create
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_TASK_Create(void (*StartFunc)(void*), unsigned long ulPriority, unsigned long ulStackSize, char *szTaskName,
								void *vpArgs, unsigned long *pulTaskID)
{
	VK_ERR_CODE eVkRet;
	
	eVkRet = VK_TASK_Create(StartFunc, ulPriority, ulStackSize, szTaskName, vpArgs, pulTaskID, 0);
	if(eVkRet != VK_OK)
	{
		return FVK_ERR;
	}
	return FVK_OK;
}

/******************************************************************************
* function : FVK_TASK_Start
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_TASK_Start(unsigned long taskId)
{
	VK_ERR_CODE eVkRet;

	eVkRet = VK_TASK_Start(taskId);
	if(eVkRet != VK_OK)
	{
		return FVK_ERR;
	}

	return FVK_OK;
}

/******************************************************************************
* function : FVK_TASK_Sleep
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
FVK_ERR_CODE FVK_TASK_Sleep (unsigned long ulMsec)
{
	VK_TASK_Sleep(ulMsec);
	return FVK_OK;
}

/******************************************************************************
* function : FVK_TASK_GetDefaultPriority
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
unsigned long FVK_TASK_GetDefaultPriority(void)
{
	return USER_PRIORITY7;
}

/******************************************************************************
* function : FVK_TASK_GetDefaultPriority
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
unsigned long FVK_TASK_GetDefaultStackSize(void)
{
	return NORMAL_TASK_STACK_SIZE;
}

/******************************************************************************
* function : FVK_TASK_GetLargeStackSize
* description : some shared libraries allocate buffers from it's stack and hand it over to kernel. 
                     In this case, small stack size can make stack overflow. 
* input : none
* output : none
* return : size of stack
* 기타 참고자료 및 파일
******************************************************************************/
unsigned long FVK_TASK_GetLargeStackSize(void)
{
	/*FIX_ME: Value is hardcoded not to affect di layer. ;)*/
	return  (1*1024*1024); /*LARGE_TASK_STACK_SIZE*/
}

/******************************************************************************
* function : FVK_TASK_GetDefaultPriority
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
******************************************************************************/
unsigned long FVK_TASK_GetDefaultQueueSize(void)
{
	return MSGQ_SIZE_NORMAL;
}

/* end of file */

