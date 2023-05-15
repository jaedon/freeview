
/********************************************************************
 * Workfile   : vk_task.c
 * Project    : Loader 3.0
 * Author     : Junho Park
 * Description: vk task
 *
 *                                 Copyright (c) 2014 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

#define _______________________________________________________________________
#define ____HEADER_FILES____
#include "vkernel.h"

#include <linden_trace.h>

#include "stdlib.h"

#include "os.h"
#include "os_cfg_app.h"
#include "taskdef.h"



#define _______________________________________________________________________
#define ____MACRO_DEFINITION____

#define DEBUG_MODULE	TRACE_MODULE_VK_TASK

#define MS_TO_TICKS(x)      ((x * OS_CFG_TICK_RATE_HZ)/ 1000)

#define VK_TASK_PTHREAD_STACK_MIN 		(64*1024)
#define VK_TASK_GUARD_SIZE 				16

#define _______________________________________________________________________
#define ____EXTERNAL_VARIABLE_DEFINITION____


#define _______________________________________________________________________
#define ____EXTERNAL_FUNCTION_DEFINITION____


#define _______________________________________________________________________
#define ____TYPE_DEFINITION____


typedef struct TASK_INFO_t
{
	char 			name[32];
	unsigned char 	ucosTaskId;
	unsigned long 	stackSize;
	unsigned long 	*pucTaskStack;
	void 			(*start_func_)(void *);
	void 			*arg_;
	HBOOL 			start;
	int 			IsJoinable;
	OS_TCB			*p_tcb;

} TASK_INFO_T;



#define _______________________________________________________________________
#define ____GLOBAL_VARIABLE_DEFINITION____

#define _______________________________________________________________________
#define ____STATIC_VARIABLE_DEFINITION____

#define _______________________________________________________________________
#define ____STATIC_FUNCTION_DEFINITION____

#define _______________________________________________________________________
#define ____STATIC_FUNCTION_BODY____


static unsigned char INT_TASK_AllocPrioritySlot(unsigned long user_priority)
{
	unsigned long priority = 0;

	switch(user_priority)
	{
		case USER_PRIORITY0:	priority = 27;	break;
		case USER_PRIORITY1:	priority = 26;	break;
		case USER_PRIORITY2:	priority = 25;	break;
		case USER_PRIORITY3:	priority = 24;	break;
		case USER_PRIORITY4:	priority = 23;	break;
		case USER_PRIORITY5:	priority = 22;	break;
		case USER_PRIORITY6:	priority = 21;	break;
		case USER_PRIORITY7:	priority = 20;	break;
		case USER_PRIORITY8:	priority = 19;	break;
		case USER_PRIORITY9:	priority = 18;	break;
		case USER_PRIORITY10:	priority = 17;	break;
		case USER_PRIORITY11:	priority = 16;	break;
		case USER_PRIORITY12:	priority = 15;	break;
		case USER_PRIORITY13:	priority = 14;	break;
		case USER_PRIORITY14:	priority = 13;	break;
		case USER_PRIORITY15:	priority = 12;	break;

		default:
			priority = 19;	break;
			break;
	}

	return priority;
}


int VK_TASK_Init(void)
{
	return VK_OK;
}



int VK_TASK_Create(void (*start_func)(void *), unsigned long prio, unsigned long stack_size, const char *name, void *arg, unsigned long *taskId, int IsJoinable)
{
    OS_ERR 				err;
	TASK_INFO_T			*taskInfo = NULL;
//	TASK_HANDLE 		hTaskHandle;
	unsigned char 		*pucTaskStack = NULL;
	unsigned char 		uCosPrio;


	/* Allocate taskInfo */
	taskInfo = (TASK_INFO_T *)DD_MEM_Alloc(sizeof(TASK_INFO_T));
	if (NULL == taskInfo)
	{
	    *taskId = 0;
	    return VK_ERROR;
	}

	/* Allocate Task Stack */
	if (stack_size < VK_TASK_PTHREAD_STACK_MIN)
	{
		stack_size = VK_TASK_PTHREAD_STACK_MIN;
	}
	pucTaskStack = DD_MEM_Alloc(stack_size + VK_TASK_GUARD_SIZE);
	if (pucTaskStack == NULL)
	{
		*taskId = 0;
		DD_MEM_Free(taskInfo);
		return VK_ERROR;
	}
	VK_memset32(pucTaskStack, 0xFF, stack_size + VK_TASK_GUARD_SIZE);



    /* Allocate for the TCB */
    taskInfo->p_tcb = DD_MEM_Alloc(sizeof(OS_TCB));
    if (!taskInfo->p_tcb) {
		PrintError("%s(%d) : Priority can't be allocated !!!\n", __func__, __LINE__);
		*taskId = 0;
		DD_MEM_Free(pucTaskStack);
		DD_MEM_Free(taskInfo);
		return VK_ERROR;
    }

	VK_memset32(taskInfo->p_tcb, 0, sizeof(OS_TCB));

	uCosPrio = INT_TASK_AllocPrioritySlot(prio);
	if ( (uCosPrio <= 1 ) || (uCosPrio >= (OS_CFG_PRIO_MAX - 2)) )
	{
		PrintError("%s(%d) : Priority can't be allocated !!!\n", __func__, __LINE__);
		*taskId = 0;
		DD_MEM_Free(pucTaskStack);
		DD_MEM_Free(taskInfo);
		DD_MEM_Free(taskInfo->p_tcb);
		return VK_ERROR;
	}


	/* Task NAME */
	VK_strncpy(taskInfo->name, name, sizeof(taskInfo->name) - 1);
	taskInfo->name[sizeof(taskInfo->name) - 1] = '\0';

	/* Task Priority */
	taskInfo->ucosTaskId		= uCosPrio;

	/* Task General Setting */
	taskInfo->start_func_		= start_func;
	taskInfo->arg_			= arg;
	taskInfo->stackSize 		= stack_size;
	taskInfo->pucTaskStack	= (unsigned long *)pucTaskStack;
	taskInfo->IsJoinable		= IsJoinable;
	taskInfo->start 			= FALSE;

	PrintDebug("Create Task. uCosPrio : [%d]\n", uCosPrio);
	/* Create Task */
	OSTaskCreate((OS_TCB	 *)taskInfo->p_tcb,
				 (CPU_CHAR	 *)name,
				 (OS_TASK_PTR)start_func,
				 (void		 *)arg,
				 (OS_PRIO	  )uCosPrio,
				 (CPU_STK	 *)pucTaskStack,
				 (CPU_STK_SIZE)stack_size/10,
				 (CPU_STK_SIZE)stack_size/4,
				 (OS_MSG_QTY )0,
				 (OS_TICK	  )0,
				 (void		 *)0,
				 (OS_OPT	  )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
				 (OS_ERR	 *)&err);
	if (err != OS_ERR_NONE) {
		PrintError("Error creating uC/OS task [%d] %d\n", uCosPrio, err);
		*taskId = 0;
		DD_MEM_Free(pucTaskStack);
		DD_MEM_Free(taskInfo);
		DD_MEM_Free(taskInfo->p_tcb);
		return VK_ERROR;
	}

	PrintDebug("Create Success.\n\n");

	*taskId = (unsigned long)taskInfo;
	PrintDebug("*taskId : %x\n", *taskId);

	return VK_OK;
}


int VK_TASK_Join(unsigned long taskId)
{
	// NOT SUPPORT
  	HAPPY(taskId);
    return VK_OK;
}

int VK_TASK_Start(unsigned long taskId)
{
	TASK_INFO_T *taskInfo = (TASK_INFO_T *)taskId;

	if(taskInfo == NULL)
	{
	    PrintError("%s(%d) : task_param is NULL!!\n", __func__, __LINE__);
	    return VK_ERROR;
	}

	if (taskInfo->start == FALSE)
	{
		taskInfo->start = TRUE;
	}

	return VK_OK;
}

/*
    VK_TASK_Stop
    Task를 수행된 이후에 이 함수를 호출해도 STOP은 되지 않는다.
    단지 count만 0으로 만든다.
*/
int VK_TASK_Stop(unsigned long taskId)
{

	TASK_INFO_T *taskInfo = (TASK_INFO_T *)taskId;

	if(taskInfo == NULL)
	{
	    PrintError("%s(%d) : task_param is NULL!!\n", __func__, __LINE__);
	    return VK_ERROR;
	}


	if (taskInfo->start == TRUE)
	{
		taskInfo->start = FALSE;
	}

	return VK_OK;

}

/*
    VK_TASK_Sleep
    태스크를 잠재운다.
*/
int VK_TASK_Sleep(unsigned long ultime)
{
    unsigned int ticks = MS_TO_TICKS(ultime);
    OS_ERR err;

    if (ticks <= 0)
    {
        ticks = 1;
    }
    OSTimeDly(ticks,
              OS_OPT_TIME_DLY,
              &err);
    if (err != OS_ERR_NONE) {
        PrintError(("OSTimeDly did not return OS_ERR_NONE\n"));
		return VK_ERROR;
    }
    return VK_OK;

}

/*
    VK_TASK_GetCurrentId
    현재 Task Id를 얻는다.
*/
int VK_TASK_GetCurrentId(unsigned long *taskId)
{
  	HAPPY(taskId);
    return VK_OK;
}

/*
    VK_TASK_Destroy
    태스크를 소멸한다.
*/
int VK_TASK_Destroy(unsigned long taskId)
{

	TASK_INFO_T *taskInfo = (TASK_INFO_T *)taskId;
    OS_ERR 		err;

	if(taskInfo == NULL)
	{
		PrintError("%s(%d) : task_param is NULL!!\n", __func__, __LINE__);
		return VK_ERROR;
	}

	PrintDebug("*taskId : %x, *taskInfo : %x\n", taskId, *taskInfo);

	if (taskInfo->start == TRUE)
	{
		taskInfo->start = FALSE;
	}

	#if OS_CFG_TASK_DEL_EN > 0
		OSTaskDel(taskInfo->p_tcb, &err);
	#endif

	return VK_OK;

}


#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
pthread_t P_TASK_GetTID(unsigned long taskId)
{
  	HAPPY(taskId);
    return VK_OK;

}

int VK_TASK_RegisterPM(unsigned long taskId)
{
  	HAPPY(taskId);
    return VK_OK;

}

int VK_TASK_SendPMMsg(int sig)
{
  	HAPPY(sig);
    return VK_OK;

}
#endif

void VK_TASK_Lock(void)
{
//20070130 :: POOH :: Can not support Preemprion
}

void VK_TASK_Unlock(void)
{
//20070130 :: POOH :: Can not support Preemprion
}
int VK_TASK_GetCurrentPriority(unsigned long taskId, unsigned long *currpriority)
{
  	HAPPY(taskId);
  	HAPPY(currpriority);

	return VK_OK;
}

/*
    VK_TASK_SetCurrentPriority
    새로운 우선순위를 세팅한다.
*/
int VK_TASK_SetCurrentPriority(unsigned long taskId, unsigned long newpriority)
{
  	HAPPY(taskId);
  	HAPPY(newpriority);
	return VK_OK;
}

int VK_TASK_GetState(unsigned long taskId, unsigned char *pStatus)
{
	HAPPY(taskId);
	HAPPY(pStatus);
	return VK_ERROR;
}

void VK_TASK_GetTaskName(unsigned long ulTaskId, char *pszName)
{
	HAPPY(ulTaskId);
	HAPPY(pszName);
	return;

}

int VK_TASK_GetStackInfo(unsigned long taskId, void **ppvStactStartAddr, unsigned long *pStackSize)
{
	HAPPY(taskId);
	HAPPY(ppvStactStartAddr);
	HAPPY(pStackSize);

	return VK_OK;
}

int VK_TASK_GetArgument(unsigned long taskId, void **ppTaskArguement)
{
	HAPPY(taskId);
	HAPPY(ppTaskArguement);

	return VK_OK;
}

void VK_TASK_PrintCallTrace(void)
{

}

#if defined(CONFIG_DEBUG)
void VK_TASK_PrintTaskList(void)
{

}
#endif


void	VK_TASK_CheckGuardArea( void )
{
	/* Not Support!!  */
}

#if defined(VK_CALLBACK_DEBUG)
#define MAX_NUM_CHECK_BLOCK_CALLBACK	4
static unsigned long s_aulCheckCallbackTask[MAX_NUM_CHECK_BLOCK_CALLBACK];
static unsigned long s_paCheckCallbackFunc[MAX_NUM_CHECK_BLOCK_CALLBACK];
static unsigned long s_aulCheckCallbackMask;
void VK_TASK_EnterCallback(unsigned long addrFunc)
{
	HAPPY(addrFunc);

}

void VK_TASK_CheckCallbackBody(char *file, int line)
{
	HAPPY(file);
	HAPPY(line);

}

void VK_TASK_LeaveCallback(void)
{

}
#endif

