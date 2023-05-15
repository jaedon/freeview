/**
 * vk_task.c
*/

/**
 * @defgroup		VK_TASK
 * @author		Tae-Hyuk Kang
 * @note			Virsual Task APIs
 * @file 			vk_task.c
 * @remarks		Copyright (c) 2008 HUMAX Co., Ltd. All rights reserved.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/queue.h>

#include "htype.h"
#include "vkernel.h"
#include "file_map.h"
#include "inc/vk_p_common_api.h"

#if defined(CONFIG_DEBUG)
#include "cmdtool.h"
#include <sys/prctl.h>
#include <sys/mman.h>
#include <malloc.h>
#endif

#define DEBUG_MODULE			TRACE_MODULE_VK_TASK

#if defined(CONFIG_DEBUG)
//#define TASK_DEBUG
#endif

#ifdef TASK_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else
#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)
#endif /* TASK_DEBUG */
//#define PRINT_CPUTIME

#define VK_TASK_PTHREAD_STACK_MIN	(16*1024)
#define VK_TASK_PTHREAD_DEFAULT_PRIO	SCHED_RR

#define VK_TASK_STACK_OVERHEAD_SIZE 			0x2000
#define VK_TASK_GUARD_SIZE 			16

#if (TOOLCHAIN_MAJOR == 4) && (TOOLCHAIN_MINOR >= 8)
#else
#define PTHREAD_STACK_MEMORY_LIMIT
#endif

struct vk_task_arg {
	pthread_mutex_t lock_;
	pthread_cond_t cond_;
	unsigned long priority_;
	unsigned long stackSize_;
	int IsJoinable;
	int start;
	unsigned long *pucTaskStack;
	unsigned long *pulStackStartAddress;
	void (*start_func_)(void *);
	void *arg_;
#if defined(CONFIG_DEBUG)
	pid_t pid;
	pid_t tid;
	int	thread_id;
#endif
	unsigned long task_id;
	char name[32];
};

struct vk_task {
	pthread_t task_;
	struct vk_task_arg targ_;
	TAILQ_ENTRY(vk_task) entries;
};

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
static unsigned long s_ulPMTaskId = 0;
#endif
static unsigned char s_ucTaskCount = 0;

static pthread_mutex_t s_mtxTaskList = PTHREAD_MUTEX_INITIALIZER;

TAILQ_HEAD(listhead, vk_task) task_list_head;

static void *thread_func_(void *arg)
{
	struct vk_task_arg *p = (struct vk_task_arg *)arg;
	int rc = 0;

	if (p == NULL)
	{
		PrintError("%s:%d Mutex id is NULL\n", __FUNCTION__, __LINE__);
		return NULL;
	}

#if defined(CONFIG_DEBUG)
	prctl(PR_SET_NAME, p->name, 0, 0, 0);
	p->pid = (pid_t)vk_p_common_GetPid();
	p->tid = (pid_t)vk_p_common_GetTid();
#endif

	pthread_mutex_lock(&p->lock_);

	while (p->start == 0 && 0 == rc) //wait signal
		rc = pthread_cond_wait(&p->cond_, &p->lock_);

	pthread_mutex_unlock(&p->lock_);

#if defined(CONFIG_DEBUG)

	p->thread_id = pthread_self();
	PrintDebug("[%s] task started...\n", p->name);
#endif

	(*p->start_func_)(p->arg_);

	return NULL;
}

static unsigned long	s_taskSelfDestroyMsgQueue;
static unsigned long	s_taskSelfDestroyTaskId;

static int P_TASK_Destroy(unsigned long taskId);

static void P_TASK_SelfDestroyTask(void *param)
{
	HUINT32		msg;
	int			vkResult;

	HAPPY(param);

	while (1)
	{
		vkResult = VK_MSG_Receive(s_taskSelfDestroyMsgQueue, &msg, sizeof(HUINT32));
		if (vkResult != VK_OK)
		{
//			PrintError("[P_TASK_SelfDestoryTask] VK_MSG_Receive error\n");
			continue;
		}
		P_TASK_Destroy(msg);
	}
}

#if defined(CONFIG_DEBUG)
static void P_TASK_PrintTaskInfo(void)
{
	char thread_name[32] = {0,};
	char process_name[32] = {0,};
	int pid = vk_p_common_GetPid();
	int tid = vk_p_common_GetTid();;

	(void)vk_p_common_GetProcessName(pid, process_name, sizeof(process_name));
	(void)vk_p_common_GetThreadName(pid, tid, thread_name, sizeof(thread_name));

	VK_printf("\n**********************************************************************");
	VK_printf("\n*                        ERROR(vkernel)                              *");
	VK_printf("\n**********************************************************************");
	VK_printf("\n** pid (%3d), name(%s)", pid, (char*)process_name);
	VK_printf("\n** tid (%3d), name(%s)", tid, (char*)thread_name);
	VK_printf("\n** reason (%s)", "unknown task-id");
	VK_printf("\n**********************************************************************");
	VK_printf("\n");

	return;
}
#endif

int VK_TASK_Init(void)
{
	int		vkResult;
	TAILQ_INIT(&task_list_head);
#if 0
#if defined(CONFIG_DEBUG)
	CMD_RegisterWord(CMD_VK_TASK_List,
		/* keyword */(char *)"vk_task",
		/* help */	(char *)"vk_task",
		/* usage */ 	(char *)"vk_task");
#endif
#endif

	vkResult = VK_MSG_Create(20, sizeof(HUINT32), "vk_task_self", &s_taskSelfDestroyMsgQueue, VK_SUSPENDTYPE_FIFO);
	if (vkResult != VK_OK)
	{
//        PrintError ("%s(%d) : error %s\n", __FUNCTION__, __LINE__, strerror (rc));
		return vkResult;
	}

	vkResult = VK_TASK_Create(P_TASK_SelfDestroyTask,
						68,
						1024*4,
						"vk_self_destroy_t",
						NULL,
						&s_taskSelfDestroyTaskId, FALSE);
	if( vkResult != VK_OK )
	{
		VK_MSG_Destroy(s_taskSelfDestroyMsgQueue);
		s_taskSelfDestroyMsgQueue = 0;
		return vkResult;
	}

	vkResult = VK_TASK_Start(s_taskSelfDestroyTaskId);
	if( vkResult != VK_OK )
	{
		VK_MSG_Destroy(s_taskSelfDestroyMsgQueue);
		s_taskSelfDestroyMsgQueue = 0;
		s_taskSelfDestroyTaskId = 0;
		return vkResult;
	}
	return VK_OK;
}

/*
    VK_TASK_Create
    태스크를 생성한다.
*/
int VK_TASK_Create(void (*start_func)(void *), unsigned long prio,unsigned long stack_size, const char *name, void *arg, unsigned long *taskId, int IsJoinable)
{
    int rc;
    struct vk_task *task;
    pthread_attr_t thread_attr;
    struct sched_param	schParam;
#if defined(PTHREAD_STACK_MEMORY_LIMIT)
    unsigned char *pucDstStackAddr = NULL;
	unsigned char *pucTaskStack;
	int	page_size;
#endif

    rc = pthread_attr_init (&thread_attr);
    if (rc != 0)
    {
        return VK_ERROR;
    }

	rc = pthread_attr_setschedpolicy (&thread_attr, VK_TASK_PTHREAD_DEFAULT_PRIO);
    if (rc != 0)
    {
		return VK_ERROR;
	}

	pthread_attr_getschedparam(&thread_attr, &schParam);
	schParam.sched_priority = prio;
	rc = pthread_attr_setschedparam (&thread_attr, &schParam);
    if (rc != 0)
    {
		return VK_ERROR;
	}

#if defined(PTHREAD_STACK_MEMORY_LIMIT)
    if (stack_size < VK_TASK_PTHREAD_STACK_MIN)
        stack_size = VK_TASK_PTHREAD_STACK_MIN;

#if defined(CONFIG_DEBUG)
	/* get page size. guared area must be aligned as page size */
	page_size = sysconf(_SC_PAGE_SIZE);
	if (page_size > 0)
	{
		/* for alignment as page size */
		stack_size = (stack_size + page_size - 1) / page_size;
		stack_size = stack_size * page_size;
	}
	else
	{
		page_size = 4096;
	}

	pucTaskStack = memalign(page_size, stack_size + page_size*2);
#else
	page_size = 0;
	pucTaskStack = DD_MEM_Alloc(stack_size+VK_TASK_STACK_OVERHEAD_SIZE+VK_TASK_GUARD_SIZE);
#endif

	if (pucTaskStack == NULL)
		return VK_ERROR;

    PrintDebug("%s(%d) : pucTaskStack = %p, stack_size= %d\n", __FUNCTION__, __LINE__, pucTaskStack, stack_size);
    rc = pthread_attr_setstack(&thread_attr, (void *)(pucTaskStack + page_size), stack_size );
    if (rc != 0)
    {
        return VK_ERROR;
    }

    rc = pthread_attr_getstack(&thread_attr,(void *)&pucDstStackAddr, (size_t *)&stack_size );
    if (rc != 0)
    {
        return VK_ERROR;
    }
    PrintDebug("%s(%d) : pucDstStackAddr = %p, stack_size= %d\n", __FUNCTION__, __LINE__, pucDstStackAddr, stack_size);
#else
#endif /* PTHREAD_STACK_MEMORY_LIMIT */

    if (IsJoinable == TRUE)
    {
        rc = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
    }
    else if(IsJoinable == FALSE)
    {
        rc = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    }
    else
    {
        PrintError("%s(%s) : IsJoinable Parameter Error!!\n", __func__, __LINE__);
        return VK_ERROR;
    }

    if (rc != 0)
    {
        PrintError ("%s(%d) : Failed to IsJoinable %s\n", __FUNCTION__, __LINE__, strerror (rc));
        return VK_ERROR;
    }

    if (task = (struct vk_task *)DD_MEM_Alloc(sizeof(struct vk_task)), NULL == task) {
        *taskId = 0;
#if defined(PTHREAD_STACK_MEMORY_LIMIT)
#if defined(CONFIG_DEBUG)
		free(pucTaskStack);
#else
        DD_MEM_Free(pucTaskStack);
#endif
#endif
        return VK_ERROR;
    }

    task->targ_.start_func_	= start_func;
    task->targ_.arg_ 		= arg;
    task->targ_.priority_	= prio;
    task->targ_.stackSize_	= stack_size;
    task->targ_.IsJoinable	= IsJoinable;
#if defined(PTHREAD_STACK_MEMORY_LIMIT)
    task->targ_.pucTaskStack = (unsigned long *)pucTaskStack;
    task->targ_.pulStackStartAddress = (unsigned long *)pucDstStackAddr;
#else
    task->targ_.pucTaskStack = 0;
    task->targ_.pulStackStartAddress = 0;
#endif

    pthread_mutex_init(&task->targ_.lock_, NULL);
    pthread_cond_init(&task->targ_.cond_, NULL);
    task->targ_.start = 0;

    strncpy(task->targ_.name, name, sizeof(task->targ_.name)-1);

	pthread_mutex_lock(&s_mtxTaskList);
    TAILQ_INSERT_TAIL(&task_list_head, task, entries);
	pthread_mutex_unlock(&s_mtxTaskList);

    rc = pthread_create(&task->task_, &thread_attr, thread_func_, &task->targ_);
    if (rc) {
#if defined(PTHREAD_STACK_MEMORY_LIMIT)
#if defined(CONFIG_DEBUG)
		free(pucTaskStack);
#else
        DD_MEM_Free(pucTaskStack);
#endif
#endif
        DD_MEM_Free(task);
        *taskId = 0;
        return VK_ERROR;
    }

	task->targ_.task_id = (unsigned long)task;
    *taskId = (unsigned long)task;

#if defined(CONFIG_TASK_PRIORITY)
	pthread_setschedparam(task->task_, VK_TASK_PTHREAD_DEFAULT_PRIO, &schParam);
#endif

    pthread_attr_destroy(&thread_attr);

#if defined(CONFIG_DEBUG) && defined(PTHREAD_STACK_MEMORY_LIMIT)
	if (mprotect(pucTaskStack, page_size, PROT_NONE) < 0)
	{
		PrintError("[%s] Error(ignored)! fail to protect stack guard area(bottom). err(%s) addr(%p)\n",
					__FUNCTION__,
					strerror(errno),
					(char *)pucTaskStack);
	}
	if (mprotect((pucTaskStack + page_size + stack_size), page_size, PROT_NONE) < 0)
	{
		PrintError("[%s] Error(ignored)! fail to protect stack guard area(top). err(%s) addr(%p)\n",
					__FUNCTION__,
					strerror(errno),
					(char *)(pucTaskStack + page_size + stack_size));
	}
#endif

    s_ucTaskCount++;
    PrintDebug("%s(%d) : s_ucTaskCount = %d\n", __func__, __LINE__, s_ucTaskCount);
    return VK_OK;
}

int VK_TASK_Join(unsigned long taskId)
{
    int iStatus;
    int iResult=VK_OK;

    struct vk_task *task = (struct vk_task *)taskId;

    if(task == NULL)
    {
        return VK_ERROR;
	}

	if (task->targ_.task_id != taskId)
	{
#if defined(CONFIG_DEBUG)
		P_TASK_PrintTaskInfo();
#endif
		PrintError("[%s] can't join task. unknown task-id\n", __FUNCTION__);
		return VK_ERROR;
	}

    pthread_join(task->task_, (void*)&iStatus);
    if (iStatus==0)
    	iResult=VK_OK;

    // Do not call "DD_MEM_Free(task->targ_.arg_)", because it should be done by MHEG engine.
    // 2008.2.21 dcchung
    #if 0
    if (task->targ_.arg_!=NULL)
    	DD_MEM_Free(task->targ_.arg_);
	if (task->targ_.pucTaskStack != NULL)
		DD_MEM_Free(task->targ_.pucTaskStack);
	#endif
    if (task!=NULL)
        DD_MEM_Free(task);

    return iResult;
}

int VK_TASK_Start(unsigned long taskId)
{
    struct vk_task *task = (struct vk_task *)taskId;
    PrintEnter();

    if (NULL == task)
    {
        PrintError("%s(%d) : task is NULL!!\n", __func__, __LINE__);
        return VK_ERROR;
    }

	if (task->targ_.task_id != taskId)
	{
#if defined(CONFIG_DEBUG)
		P_TASK_PrintTaskInfo();
#endif
		PrintError("[%s] can't start task. unknown task-id\n", __FUNCTION__);
		return VK_ERROR;
	}

    pthread_mutex_lock(&task->targ_.lock_);
    if (task->targ_.start == 0) {
    	task->targ_.start = 1;
    	pthread_cond_signal(&task->targ_.cond_);
    }
    pthread_mutex_unlock(&task->targ_.lock_);
    PrintExit();
    return VK_OK;
}

/*
    VK_TASK_Stop
    Task를 수행된 이후에 이 함수를 호출해도 STOP은 되지 않는다.
    단지 count만 0으로 만든다.
*/
int VK_TASK_Stop(unsigned long taskId)
{
    struct vk_task *task = (struct vk_task *)taskId;

    PrintEnter();

    if (task == NULL)
    {
        return VK_OK;
    }

	if (task->targ_.task_id != taskId)
	{
#if defined(CONFIG_DEBUG)
		P_TASK_PrintTaskInfo();
#endif
		PrintError("[%s] can't stop task. unknown task-id\n", __FUNCTION__);
		return VK_OK;
	}

    pthread_mutex_lock(&task->targ_.lock_);
    if (task->targ_.start != 0)
    {
        task->targ_.start = 0;
	}
    pthread_mutex_unlock(&task->targ_.lock_);

    return VK_OK;
}

/*
    VK_TASK_Sleep
    태스크를 잠재운다.
*/
int VK_TASK_Sleep(unsigned long ultime)
{
	struct timespec delay;
	struct timespec rem;
	int rc;

	if (ultime != 0)
	{
		VK_TASK_CheckCallback();
	}
	delay.tv_sec = ultime / 1000;
	delay.tv_nsec = (ultime % 1000) * 1000000;

	for (;;) {
		rc = nanosleep(&delay, &rem);
		if (0 != rc) {
			if (EINTR == errno) {
				delay = rem;
				continue;
			}
			return VK_ERROR;
		}
		break;
	}

	return VK_OK;
}

/*
    VK_TASK_GetCurrentId
    현재 Task Id를 얻는다.
*/
int VK_TASK_GetCurrentId(unsigned long *taskId)
{
    pthread_t			pthreadSelf;
    struct vk_task 	*task;

    pthreadSelf = pthread_self();

	pthread_mutex_lock(&s_mtxTaskList);
    for (task = task_list_head.tqh_first; task != NULL; task = task->entries.tqe_next)
    {
        if (task->task_ == pthreadSelf)
        {
            *taskId = (unsigned long)task;
			pthread_mutex_unlock(&s_mtxTaskList);
            return VK_OK;
        }
    }
	pthread_mutex_unlock(&s_mtxTaskList);
    return VK_ERROR;
}

/*
    VK_TASK_Destroy
    태스크를 소멸한다.
*/
#if 1
static int P_TASK_Destroy(unsigned long taskId)
{
    int rc;
    struct vk_task *task = (struct vk_task *)taskId;
#if defined(CONFIG_DEBUG) && defined(PTHREAD_STACK_MEMORY_LIMIT)
	char *p;
	int page_size;
#endif

    if( taskId == (unsigned long)NULL )
    {
        PrintDebug("%s(%d) taskId == NULL\n", __FUNCTION__, __LINE__);
        return VK_ERROR;
    }

	pthread_mutex_lock(&task->targ_.lock_);
	if (task->targ_.start == 1)
	{
		task->targ_.start = 0;
	}
	rc = pthread_cancel(task->task_);
	if (rc)
	{
		PrintError("[VK_TASK_Destroy] warning : there is no thread in pthread taskId(%08X) pthreadId(%08X)\n", taskId, task->task_);
	}
	VK_TASK_Sleep(1);
	if (task->targ_.IsJoinable)
	{
		pthread_join(task->task_, NULL);
	}

#if defined(PTHREAD_STACK_MEMORY_LIMIT)
#if defined(CONFIG_DEBUG)
	/* convert 'unsigned long *' to 'char *' for pointer calculating */
	p = (char*)task->targ_.pucTaskStack;
	page_size = sysconf(_SC_PAGE_SIZE);
	if (page_size <= 0)
	{
		page_size = 4096;
	}

	if (mprotect(p, page_size, PROT_READ|PROT_WRITE|PROT_EXEC) < 0)
	{
		PrintError("[%s] Error(ignored)! Fail to unprotect stack guard area(bottom). err(%s) addr(%p)\n",
					__FUNCTION__,
					strerror(errno),
					(char*)p);
	}
	if (mprotect((p + page_size + task->targ_.stackSize_), page_size, PROT_READ|PROT_WRITE|PROT_EXEC) < 0)
	{
		PrintError("[%s] Error(ignored)! Fail to unprotect stack guard area(top). err(%s) addr(%p)\n",
					__FUNCTION__,
					strerror(errno),
					(char *)(p + page_size + task->targ_.stackSize_));
	}
	free(p);
#else
	DD_MEM_Free(task->targ_.pucTaskStack);
#endif
#endif
	pthread_mutex_unlock(&task->targ_.lock_);

	pthread_cond_destroy(&task->targ_.cond_);
	pthread_mutex_destroy(&task->targ_.lock_);

	if( s_ucTaskCount > 0 )
	{
        s_ucTaskCount--;
        PrintDebug("s_ucTaskCount = %d\n", s_ucTaskCount);
	}

	pthread_mutex_lock(&s_mtxTaskList);
    TAILQ_REMOVE(&task_list_head, task, entries);
	pthread_mutex_unlock(&s_mtxTaskList);

	task->targ_.task_id = 0;
    DD_MEM_Free(task);

    return VK_OK;
}

int VK_TASK_Destroy(unsigned long taskId)
{
	int	vkResult;
	struct vk_task 	*task;

    if( taskId == (unsigned long)NULL )
    {
        PrintDebug("%s(%d) taskId == NULL\n", __FUNCTION__, __LINE__);
        return VK_ERROR;
    }

	task = (struct vk_task *)taskId;
	if (task->targ_.task_id != taskId)
	{
#if defined(CONFIG_DEBUG)
		P_TASK_PrintTaskInfo();
#endif
		PrintError("[%s] can't destroy task. unknown task-id\n", __FUNCTION__);
		return VK_ERROR;
	}

	vkResult = VK_MSG_SendTimeout(s_taskSelfDestroyMsgQueue, &taskId, sizeof(HUINT32), 0);
	if (vkResult != VK_OK)
	{
//		PrintError("[P_TASK_SelfDestoryTask] VK_MSG_Receive error\n");
		return vkResult;
	}

    return vkResult;
}

#else
int VK_TASK_Destroy(unsigned long taskId)
{
    int rc;
    struct vk_task *task = (struct vk_task *)taskId;

    if( taskId == (unsigned long)NULL )
    {
        PrintDebug("%s(%d) taskId == NULL\n", __FUNCTION__, __LINE__);
        return VK_ERROR;
    }

	pthread_mutex_lock(&task->targ_.lock_);
	if (task->targ_.start == 1)
	{
		task->targ_.start = 0;
	}
	rc = pthread_cancel(task->task_);
	if (rc)
	{
		PrintError("[VK_TASK_Destroy] warning : there is no thread in pthread taskId(%08X) pthreadId(%08X)\n", taskId, task->task_);
	}
	VK_TASK_Sleep(1);
#if defined(CONFIG_THREAD_JOIN_TEMPORARY)
#else
	if (task->targ_.IsJoinable)
#endif
	{
		pthread_join(task->task_, NULL);
	}

	DD_MEM_Free(task->targ_.pucTaskStack);
	pthread_mutex_unlock(&task->targ_.lock_);
	if( s_ucTaskCount > 0 )
	{
        s_ucTaskCount--;
        PrintDebug("s_ucTaskCount = %d\n", s_ucTaskCount);
	}

	pthread_mutex_lock(&s_mtxTaskList);
    TAILQ_REMOVE(&task_list_head, task, entries);
	pthread_mutex_unlock(&s_mtxTaskList);

    DD_MEM_Free(task);

    return VK_OK;
}
#endif

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
pthread_t P_TASK_GetTID(unsigned long taskId)
{
	struct vk_task *task = (struct vk_task *)taskId;
	return task->task_;
}

int VK_TASK_RegisterPM(unsigned long taskId)
{
	s_ulPMTaskId = taskId;
	return VK_OK;
}

int VK_TASK_SendPMMsg(int sig)
{
	pthread_t thread_id;
	if(s_ulPMTaskId != 0)
	{
		thread_id = P_TASK_GetTID(s_ulPMTaskId);
		pthread_kill((pthread_t)thread_id, sig);
	}
	return 0;
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
	struct vk_task *task = (struct vk_task *)taskId;

    if( task == NULL )
	{
		return VK_ERROR;
	}

	if (task->targ_.task_id != taskId)
	{
#if defined(CONFIG_DEBUG)
		P_TASK_PrintTaskInfo();
#endif
		PrintError("[%s] can't get current task priority. unknown task-id\n", __FUNCTION__);
		return VK_ERROR;
	}

	*currpriority = task->targ_.priority_;

	return VK_OK;
}

/*
    VK_TASK_SetCurrentPriority
    새로운 우선순위를 세팅한다.
*/
int VK_TASK_SetCurrentPriority(unsigned long taskId, unsigned long newpriority)
{
	int rc;
	int policy;
	struct vk_task *task = (struct vk_task *)taskId;
	struct sched_param	schParam;

	if (task == NULL)
	{
		return VK_ERROR;
	}

	if (task->targ_.task_id != taskId)
	{
#if defined(CONFIG_DEBUG)
		P_TASK_PrintTaskInfo();
#endif
		PrintError("[%s] can't set current task priority. unknown task-id\n", __FUNCTION__);
		return VK_ERROR;
	}

	rc = pthread_getschedparam(task->task_, &policy, &schParam);
	if (schParam.sched_priority == (int)newpriority)
	{
		return VK_OK;
	}

	schParam.sched_priority 	= newpriority;

	pthread_mutex_lock(&task->targ_.lock_);
	rc = pthread_setschedparam(task->task_, policy, &schParam);
	pthread_mutex_unlock(&task->targ_.lock_);

	if (rc != 0)
	{
		return VK_ERROR;
	}

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
	struct vk_task *task = (struct vk_task *)ulTaskId;

	if (task == NULL)
	{
		return;
	}

	if (task->targ_.task_id != ulTaskId)
	{
#if defined(CONFIG_DEBUG)
		P_TASK_PrintTaskInfo();
#endif
		PrintError("[%s] can't get task name. unknown task-id\n", __FUNCTION__);
		return;
	}

	VK_memcpy(pszName, task->targ_.name, sizeof(task->targ_.name) );
}

int VK_TASK_GetStackInfo(unsigned long taskId, void **ppvStactStartAddr, unsigned long *pStackSize)
{
	struct vk_task *task = (struct vk_task *)taskId;

	if (task == NULL)
	{
		PrintError("[VK_TASK_GetStackInfo]%s(%d) error null task id\n", __FUNCTION__, __LINE__);
		return VK_INVALID_ID;
	}

	if (task->targ_.task_id != taskId)
	{
#if defined(CONFIG_DEBUG)
		P_TASK_PrintTaskInfo();
#endif
		PrintError("[%s] can't get task stack info. unknown task-id\n", __FUNCTION__);
		return VK_INVALID_ID;
	}

	if (ppvStactStartAddr == NULL)
	{
		PrintError("[VK_TASK_GetStackInfo]%s(%d) error null ppvStactStartAddr input param\n", __FUNCTION__, __LINE__);
		return VK_INVALID_ID;
	}
	if (pStackSize == NULL)
	{
		PrintError("[VK_TASK_GetStackInfo]%s(%d) error null pStackSize input param\n", __FUNCTION__, __LINE__);
		return VK_INVALID_ID;
	}
#if defined(PTHREAD_STACK_MEMORY_LIMIT)
	*ppvStactStartAddr = task->targ_.pulStackStartAddress;
	*pStackSize = task->targ_.stackSize_;
#endif
	return VK_OK;
}

int VK_TASK_GetArgument(unsigned long taskId, void **ppTaskArguement)
{
	struct vk_task *task = (struct vk_task *)taskId;

	if (task == NULL)
	{
		PrintError("[VK_TASK_GetArgument]%s(%d) error null task id\n", __FUNCTION__, __LINE__);
		return VK_INVALID_ID;
	}

	if (task->targ_.task_id != taskId)
	{
#if defined(CONFIG_DEBUG)
		P_TASK_PrintTaskInfo();
#endif
		PrintError("[%s] can't get task argument. unknown task-id\n", __FUNCTION__);
		return VK_INVALID_ID;
	}

	if (ppTaskArguement == NULL)
	{
		PrintError("[VK_TASK_GetArgument]%s(%d) error null input param\n", __FUNCTION__, __LINE__);
		return VK_INVALID_ID;
	}

	*ppTaskArguement = task->targ_.arg_;
	return VK_OK;
}

int VK_TASK_GetCountTasks (int *pnTaskNum)
{
	int					nTaskNum;
    struct vk_task 		*task;

	if (NULL == pnTaskNum)			{ return VK_ERROR; }

	pthread_mutex_lock(&s_mtxTaskList);
    for (task = task_list_head.tqh_first, nTaskNum = 0; task != NULL; task = task->entries.tqe_next, nTaskNum++);
	pthread_mutex_unlock(&s_mtxTaskList);

	*pnTaskNum = nTaskNum;
	return VK_OK;
}

int VK_TASK_GetAllTaskIds (int nArraySize, int *pnTaskNum, unsigned long *pulTaskIdArray)
{
	int					nTaskCnt;
    struct vk_task 		*task;

	if ((NULL == pnTaskNum) || (NULL ==	 pulTaskIdArray))		{ return VK_ERROR; }

	pthread_mutex_lock(&s_mtxTaskList);
    for (task = task_list_head.tqh_first, nTaskCnt = 0; (task != NULL) && (nTaskCnt < nArraySize); task = task->entries.tqe_next, nTaskCnt++)
    {
		pulTaskIdArray[nTaskCnt] = (unsigned long)task;
    }
	pthread_mutex_unlock(&s_mtxTaskList);

	*pnTaskNum = nTaskCnt;
	return VK_OK;
}


void VK_TASK_PrintCallTrace(void)
{
	vk_p_common_ShowBackTrace(NULL, NULL);
	return;
}

#if defined(CONFIG_DEBUG)
void VK_TASK_PrintTaskList(void)
{
    struct vk_task *np;
    unsigned long totalStackSize = 0;
    int i;
#if defined(PRINT_CPUTIME)
    char procFilename[256] ;
    char buffer[1024] ;
    int fd, num_read ;
    long jiffies_user = -1;
    long jiffies_sys = -1;
    char *ptrUsr;
    int j;
#endif

/*
    task_id : 생성된 Task ID
    threadId : 생성된 Thread Id
    stack_ptr : 스택포인터
    count :  Thread의 시작여부.
    priority : Task의 우선순위
    stackSize : Stack 사이즈
    name : Task 이름
*/
    PrintError("***************************************************************\n", 0);
#if defined(PRINT_CPUTIME)
    PrintError("    cpu_time     task_id    threadId(  pid)   stack_ptr  stackSize pri start name\n");
#else
    PrintError("    task_id    threadId(  pid)   stack_ptr  stackSize pri start name\n", 0);
#endif
    for (np = task_list_head.tqh_first, i=0; np != NULL; np = np->entries.tqe_next, i++)
    {
#if defined(PRINT_CPUTIME)
    	memset(procFilename, 0x0, 256);
		memset(buffer, 0x0, 1024);
		sprintf(procFilename, "/proc/%d/task/%d/stat",np->targ_.pid,np->targ_.tid);
		fd = open(procFilename, O_RDONLY, 0);
		if(fd != -1)
		{
			num_read = read(fd, buffer, 1023);
			close(fd);
			buffer[num_read] = '\0';
			ptrUsr = strrchr(buffer, ')') + 1 ;
			for(j = 3 ; j != 14 ; ++j) ptrUsr = strchr(ptrUsr+1, ' ') ;

			ptrUsr++;
			jiffies_user = atol(ptrUsr) ;
			jiffies_sys = atol(strchr(ptrUsr,' ') + 1) ;
		}
        PrintError("%02d [%8x] : 0x%08x %8d(%7d) 0x%08x 0x%05x   %03d %1d     %s\n",
                                    i, (jiffies_sys+jiffies_user), np, np->targ_.tid, np->targ_.pid, np->targ_.pucTaskStack, np->targ_.stackSize_, np->targ_.priority_, np->targ_.start, np->targ_.name);

#else
        PrintError("%02d : 0x%08x %8d(%7d) 0x%08x 0x%05x   %03d %1d     %s\n",
                                    i, np, np->targ_.tid, np->targ_.pid, np->targ_.pucTaskStack, np->targ_.stackSize_, np->targ_.priority_, np->targ_.start, np->targ_.name);
#endif
        totalStackSize += np->targ_.stackSize_;
    }
    PrintError("totalStackSize = %ld\n",totalStackSize);
    PrintError("***************************************************************\n", 0);
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
	unsigned long	currTaskId;
	unsigned long	i;

	VK_TASK_GetCurrentId(&currTaskId);
	for (i=0; i<MAX_NUM_CHECK_BLOCK_CALLBACK; i++)
	{
		if (s_aulCheckCallbackTask[i] == 0)
		{
			s_aulCheckCallbackTask[i] = currTaskId;
			s_paCheckCallbackFunc[i] = addrFunc;
			s_aulCheckCallbackMask |= (1<<i);
			return;
		}
	}
}

void VK_TASK_CheckCallbackBody(char *file, int line)
{
	if (s_aulCheckCallbackMask)
	{
		unsigned long	currTaskId;
		unsigned long	i;

		VK_TASK_GetCurrentId(&currTaskId);
		for (i=0; i<MAX_NUM_CHECK_BLOCK_CALLBACK; i++)
		{
			if (s_aulCheckCallbackTask[i] == currTaskId)
			{
				PrintError("Critical Warning : do not use blocking kernel function in callback %s(%d) <- function addr(0x%08x)\n", file, line, s_paCheckCallbackFunc[i]);
				PrintError("%s(%d) VK_TASK_Sleep() : X\n", __FUNCTION__, __LINE__);
				PrintError("%s(%d) VK_SEM_Get()    : X\n", __FUNCTION__, __LINE__);
				PrintError("%s(%d) VK_MSG_Send() -> VK_MSG_SendTimeout(..., 0)\n", __FUNCTION__, __LINE__);
				return;
			}
		}
	}
}

void VK_TASK_LeaveCallback(void)
{
	unsigned long	currTaskId;
	unsigned long	i;

	VK_TASK_GetCurrentId(&currTaskId);
	for (i=0; i<MAX_NUM_CHECK_BLOCK_CALLBACK; i++)
	{
		if (s_aulCheckCallbackTask[i] == currTaskId)
		{
			s_aulCheckCallbackTask[i] = 0;
			s_paCheckCallbackFunc[i] = 0;
			s_aulCheckCallbackMask &= (~(1<<i));
			return;
		}
	}
}
#endif

