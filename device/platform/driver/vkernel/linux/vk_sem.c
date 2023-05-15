
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/syscall.h>

#include "vkernel.h"

#define DEBUG_MODULE			TRACE_MODULE_VK_SEM

#ifdef SEM_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else 
#define PrintDebug(fmt, ...)    	while (0) ((int (*)(char *, ...)) 0)
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 			while (0) ((int (*)(char *, ...)) 0)				
#define PrintExit(...)				while (0) ((int (*)(char *, ...)) 0)		
#endif /* SEM_DEBUG */


#define SEMA4_ERROR 			-1
#define MAX_SEM_NAMELENGTH 		16
#define MAX_SEM_FILENAMELENGTH 		256

#define	MAX_TASK_NUM	16

/*
thkang : 아래 변수는 세마포어 초기화시에 pshared 변수에 들어가는 값인데
		0 이나  0이 아닐 경우로 나누어 지는데 아래 값들은 어디서 나온것인고.....
*/
#define SEM_SUSPEND_FIFO  		0x10
#define SEM_SUSPEND_PRIORITY  	0x20   /*default */

typedef struct vk_sem {
	sem_t sema4;

#if defined(VK_SEM_DEBUG)
	TAILQ_ENTRY(vk_sem) entries;
	char name[MAX_SEM_NAMELENGTH];
	int last_thread_id;
	char waitfile[MAX_TASK_NUM][MAX_SEM_FILENAMELENGTH];
	int waitline[MAX_TASK_NUM];
	char lastfile[MAX_SEM_FILENAMELENGTH];
	int lastline;
	unsigned long tick;
	int nCount;
	unsigned long task_id[MAX_TASK_NUM];
	int current_owner;
#endif
}vk_sem;


#if defined(VK_SEM_DEBUG)
TAILQ_HEAD(listhead, vk_sem) sem_list_head;
#endif

static int VK_SEM_Create_Priv(unsigned long *sem_id, unsigned long count, int suspendType, const char *name)
{
	int nReturnCode = 0;
	vk_sem *pstSema = NULL;
#if defined(VK_SEM_DEBUG)
	int i;
#endif
	HAPPY(name);

	pstSema = (vk_sem *)malloc(sizeof(vk_sem));
	if (pstSema == NULL)
	{
		*sem_id = 0;
		PrintError("Fail to malloc Sema %d lines\n", __LINE__);
		return VK_ALLOC_ERROR;
	}

	memset( pstSema, 0, sizeof(vk_sem) );

       //0 = process사이에서 세마포어를 공유 하지 않게 함. 일반적으로 0을 사용함...
	suspendType = 0;

	nReturnCode = sem_init(&pstSema->sema4, suspendType, count);
	if (nReturnCode == SEMA4_ERROR)
	{
		*sem_id = 0;
		free(pstSema);
		PrintError("Fail to sem_init %d lines errno: %d\n", __LINE__, errno);
		return VK_ERROR;
	}

#if defined(VK_SEM_DEBUG)
	strncpy(pstSema->name, name ,(MAX_SEM_NAMELENGTH-1));
	pstSema->last_thread_id = 0;
	pstSema->tick = 0;
	pstSema->current_owner = -1;
	pstSema->nCount = 0;
	for( i=0; i<MAX_TASK_NUM; i++ ) {
		pstSema->task_id[i] = 0;
	}

	TAILQ_INSERT_TAIL(&sem_list_head, pstSema, entries);
#endif

	*sem_id = (unsigned long)pstSema;

	return VK_OK;
}


/*****************************************************************************
 function: 	VK_SEM_Init
 description:	init sem for debugging
 argument:
 return:
******************************************************************************/
int VK_SEM_Init(void)
{
#if defined(VK_SEM_DEBUG)
	TAILQ_INIT(&sem_list_head);
#endif
	return VK_OK;
}

/*****************************************************************************
 function: 	VK_SEM_Create
 description:	create a binary semaphore
 argument:
 return:
******************************************************************************/
int VK_SEM_Create(unsigned long * sem_id, const char *name, VK_SUSPENDTYPE etSuspendType)
{
    int nReturnCode = VK_OK;

    PrintEnter();

    if (sem_id == NULL)
    {
        PrintError("Error at %s() %d lines\n",__func__, __LINE__);
        return VK_ERROR;
    }
    if (name == NULL)
    {
        PrintError("Error at %s() %d lines\n",__func__, __LINE__);
        return VK_ERROR;
    }
    nReturnCode = VK_SEM_CreateWithCountEx(sem_id, 1, name, etSuspendType);
    if(VK_OK != nReturnCode)
    {
        PrintError("Error at %s() %d lines : VK_SEM_CreateWithCountEx Error\n",__func__, __LINE__);
        return nReturnCode;
    }

    PrintExit();
    return VK_OK;
}


/*****************************************************************************
 function: 	VK_SEM_CreateWithCount
 description:	create a binary semaphore
 argument:
 return:
******************************************************************************/
int VK_SEM_CreateWithCount(unsigned long *sem_id, unsigned long count, const char *name, VK_SUSPENDTYPE etSuspendType)
{
    int nReturnCode = VK_OK;

    PrintEnter();

    if (sem_id == NULL)
    {
        PrintError("Error at %s() %d lines\n",__func__, __LINE__);
        return VK_ERROR;
    }
    if (name == NULL)
    {
        PrintError("Error at %s() %d lines\n",__func__, __LINE__);
        return VK_ERROR;
    }

    nReturnCode = VK_SEM_CreateWithCountEx(sem_id, count, name, etSuspendType);
    if(VK_OK != nReturnCode)
    {
        PrintError("Error at %s() %d lines : VK_SEM_CreateWithCountEx Error\n",__func__, __LINE__);
        return nReturnCode;
    }

    return VK_OK;
}


#if defined(VK_SEM_DEBUG)
/*****************************************************************************
 function: 	VK_SEM_Get
 description:	acquires a semaphore token
 argument:
 return:
******************************************************************************/
int VK_DBGSEM_Get(unsigned long sem_id, char *file, int line)
{
	int nReturnCode = 0;
	int vk_res;
	vk_sem *sem = (vk_sem *)sem_id;
	unsigned long i, ulTask_id=0, ulFound=0,ulFoundSlot=0xff;

	PrintEnter();
	if (sem == NULL)
	{
		PrintError("Error at %s() %d lines %s(%d)\n",__func__, __LINE__, file, line);
		return VK_ERROR;
	}

	VK_TASK_CheckCallback();

	sem->nCount++;
	vk_res = VK_TASK_GetCurrentId( &ulTask_id );
	if ( vk_res == VK_OK )
	{
		sem->last_thread_id = ulTask_id;
	}

	for( i=0; i<MAX_TASK_NUM; i++ )
	{
		if( sem->task_id[i] == ulTask_id )
		{
			ulFound = 1;
			ulFoundSlot = i;
			break;
		}
	}

	if( ulFound == 0 )
	{
		for( i=0; i<MAX_TASK_NUM; i++ )
		{
			if( sem->task_id[i] == 0 )
			{
				sem->task_id[i] = ulTask_id;
				VK_strncpy(sem->waitfile[i], file ,MAX_SEM_FILENAMELENGTH-1);
				sem->waitline[i] = line;
				ulFoundSlot = i;
				break;
			}
		}
	}
	else
	{
		VK_strncpy(sem->waitfile[ulFoundSlot], file ,MAX_SEM_FILENAMELENGTH-1);
		sem->waitline[ulFoundSlot] = line;
	}

	VK_strncpy(sem->lastfile, file ,MAX_SEM_FILENAMELENGTH-1);
	sem->lastline = line;
	sem->tick = VK_TIMER_GetSystemTick();

	nReturnCode = sem_wait(&sem->sema4);
	if (nReturnCode == SEMA4_ERROR)
	{
//		sem->waitfile[ulFoundSlot][0] = 0;
//		sem->waitline[ulFoundSlot] = 0;
		PrintError("Fail to sem_wait %d lines errno: %d\n", __LINE__, errno);
		return VK_ERROR;
	}
	sem->current_owner = syscall(SYS_gettid);

	return VK_OK;

}

/*****************************************************************************
 function: 	VK_SEM_Get
 description:	acquires a semaphore token
 argument:
 return:
******************************************************************************/
int VK_DBGSEM_GetTimeout(unsigned long sem_id, unsigned long timeout, char *file, int line)
{
	int nReturnCode = 0;
	vk_sem *sem = (vk_sem *)sem_id;
	struct timespec 	timeo;
	struct timespec cur_time;
	long long exp_nsec;
	unsigned long i, ulTask_id=0, ulFound=0,ulFoundSlot=0xff;

	PrintEnter();
	if (timeout == 0xFFFFFFFF)
	{
		return VK_DBGSEM_Get(sem_id, file, line);
	}

	if (sem == NULL)
	{
		PrintError("Error at %s() %d lines\n",__func__, __LINE__);
		return VK_ERROR;
	}

	if (timeout != 0)
	{
		VK_TASK_CheckCallback();
	}

	sem->nCount++;
	if( VK_TASK_GetCurrentId( &ulTask_id ) == VK_OK )
	{
		sem->last_thread_id = ulTask_id;
	}

	for( i=0; i<MAX_TASK_NUM; i++ ) {
		if( sem->task_id[i] == ulTask_id ) {
			ulFound = 1;
			ulFoundSlot = i;
			break;
		}
	}

	if( ulFound == 0 ) {
		for( i=0; i<MAX_TASK_NUM; i++ ) {
			if( sem->task_id[i] == 0 ) {
				sem->task_id[i] = ulTask_id;
				VK_strncpy(sem->waitfile[i], file ,MAX_SEM_FILENAMELENGTH-1);
				sem->waitline[i] = line;
				ulFoundSlot = i;
				break;
			}
		}
	}
	else
	{
		VK_strncpy(sem->waitfile[ulFoundSlot], file ,MAX_SEM_FILENAMELENGTH-1);
		sem->waitline[ulFoundSlot] = line;
	}


	sem->tick = VK_TIMER_GetSystemTick();

	if (timeout == 0)
	{
		nReturnCode = sem_trywait(&sem->sema4);
		if (nReturnCode == SEMA4_ERROR && EAGAIN == errno)
		{
			PrintDebug("Fail to sem_trywait %d lines errno: %d\n", __LINE__, errno);
//			sem->waitfile[ulFoundSlot][0] = 0;
//			sem->waitline[ulFoundSlot] = 0;
			return VK_TIMEOUT;
		}
	}
	else
	{
		clock_gettime(CLOCK_REALTIME, &cur_time);
		exp_nsec = (long long)cur_time.tv_sec * 1000 * 1000 * 1000 + cur_time.tv_nsec;
		if (timeout != 0)
			exp_nsec += (timeout * 1000 - 999) * 1000;
		timeo.tv_sec = exp_nsec / (1000 * 1000 * 1000);
		timeo.tv_nsec = exp_nsec % (1000 * 1000 * 1000);

		nReturnCode = sem_timedwait(&sem->sema4, &timeo );
		if (nReturnCode == SEMA4_ERROR && errno != ETIMEDOUT)
		{
			PrintError("sem_timedwait Error !! errno : %d %s() %d line\n", errno,  __func__, __LINE__);
//			sem->waitfile[ulFoundSlot][0] = 0;
//			sem->waitline[ulFoundSlot] = 0;
			return VK_ERROR;
		}
		else if(nReturnCode == SEMA4_ERROR && errno == ETIMEDOUT)
		{
			PrintDebug("sem_timedwait Time out errno : %d %s() %d line\n", errno,  __func__, __LINE__);
//			sem->waitfile[ulFoundSlot][0] = 0;
//			sem->waitline[ulFoundSlot] = 0;
			return VK_TIMEOUT;
		}
	}

	sem->current_owner = syscall(SYS_gettid);

	PrintExit();

	return VK_OK;

}

/********************************************************************
 function: 	VK_SEM_Release
 description:	release a semaphore token
 argument:
 return:
*********************************************************************/
int VK_DBGSEM_Release(unsigned long sem_id, char *file, int line)
{
	int nReturnCode = 0;
	vk_sem *sem = (vk_sem *)sem_id;
	unsigned long ulTask_id=0,i,ulFoundSlot=0xff;

	HAPPY(file);
	HAPPY(line);

	PrintEnter();

	if (sem == NULL)
	{
		PrintError("Error at %s() %d lines\n",__func__, __LINE__);
		return VK_ERROR;
	}

	VK_TASK_GetCurrentId( &ulTask_id );
	for(i=0; i<MAX_TASK_NUM; i++) {
		if( ulTask_id == sem->task_id[i] || sem->nCount == 0 ) {
			sem->task_id[i] = 0;
			ulFoundSlot = i;
			sem->waitfile[i][0] = 0;
			sem->waitline[i] = 0;
		}
	}

	sem->last_thread_id = 0;
	sem->lastfile[0] = 0;
	sem->lastline = 0;
	sem->tick = 0;
	if( sem->nCount > 0 ) sem->nCount--;
	sem->current_owner = -1;

	nReturnCode = sem_post(&sem->sema4);
	if (nReturnCode == SEMA4_ERROR)
	{
		PrintError("Fail to sem_post %d lines errno: %d\n", __LINE__, errno);
		return VK_ERROR;
	}

	PrintExit();

	return VK_OK;
}
#else
/*****************************************************************************
 function: 	VK_SEM_Get
 description:	acquires a semaphore token
 argument:
 return:
******************************************************************************/
int VK_SEM_Get(unsigned long sem_id)
{
	int nReturnCode = 0;
	vk_sem *sem = (vk_sem *)sem_id;
	PrintEnter();
	
	if (sem == NULL)
	{
		PrintError("Error at %s() %d lines\n",__func__, __LINE__);
		return VK_ERROR;
	}

	VK_TASK_CheckCallback();
	nReturnCode = sem_wait(&sem->sema4);
	if (nReturnCode == SEMA4_ERROR)
	{
		PrintError("Fail to sem_wait %d lines errno: %d\n", __LINE__, errno);
		return VK_ERROR;
	}

	return VK_OK;

}

/*****************************************************************************
 function: 	VK_SEM_Get
 description:	acquires a semaphore token
 argument:
 return:
******************************************************************************/
int VK_SEM_GetTimeout(unsigned long sem_id, unsigned long timeout )
{
	int nReturnCode = 0;
	vk_sem *sem = (vk_sem *)sem_id;
	struct timespec 	timeo;
	struct timespec cur_time;
	long long exp_nsec;

	PrintEnter();

	if (timeout == 0xFFFFFFFFUL)
	{
		return VK_SEM_Get(sem_id);
	}

	if (sem == NULL)
	{
		PrintError("Error at %s() %d lines\n",__func__, __LINE__);
		return VK_ERROR;
	}

	if (timeout != 0UL)
	{
		VK_TASK_CheckCallback();
	}

	if (timeout == 0UL)
	{
		nReturnCode = sem_trywait(&sem->sema4);
		if (nReturnCode == SEMA4_ERROR && EAGAIN == errno)
		{
			PrintDebug("Fail to sem_trywait %d lines errno: %d\n", __LINE__, errno);
			return VK_TIMEOUT;
		}
	}
	else
	{
		clock_gettime(CLOCK_REALTIME, &cur_time);
		exp_nsec = (long long)cur_time.tv_sec * 1000 * 1000 * 1000 + cur_time.tv_nsec;
		if (timeout != 0)
			exp_nsec += (timeout * 1000) * 1000;
		timeo.tv_sec = exp_nsec / (1000 * 1000 * 1000);
		timeo.tv_nsec = exp_nsec % (1000 * 1000 * 1000);
		
		nReturnCode = sem_timedwait(&sem->sema4, &timeo );
		if (nReturnCode == SEMA4_ERROR && errno != ETIMEDOUT)
		{
			PrintError("sem_timedwait Error !! errno : %d %s() %d line\n", errno,  __func__, __LINE__);
			return VK_ERROR;
		}
		else if(nReturnCode == SEMA4_ERROR && errno == ETIMEDOUT)
		{
			PrintDebug("sem_timedwait Time out errno : %d %s() %d line\n", errno,  __func__, __LINE__);
			return VK_TIMEOUT;
		}
	}

	PrintExit();

	return VK_OK;

}

/********************************************************************
 function: 	VK_SEM_Release
 description:	release a semaphore token
 argument:
 return:
*********************************************************************/
int VK_SEM_Release(unsigned long sem_id)
{
	int nReturnCode = 0;
	vk_sem *sem = (vk_sem *)sem_id;

	PrintEnter();

	if (sem == NULL)
	{
		PrintError("Error at %s() %d lines\n",__func__, __LINE__);
		return VK_ERROR;
	}

	nReturnCode = sem_post(&sem->sema4);
	if (nReturnCode == SEMA4_ERROR)
	{
		PrintError("Fail to sem_post %d lines errno: %d\n", __LINE__, errno);
		return VK_ERROR;
	}

	PrintExit();

	return VK_OK;
}
#endif  //#if defined(VK_SEM_DEBUG)

/*****************************************************************************
 function: 	VK_SEM_Destroy
 description:	delete a semaphore
 argument:
 return:
******************************************************************************/
int VK_SEM_Destroy(unsigned long sem_id)
{
	int nReturnCode = 0;
	vk_sem *sem = (vk_sem *)sem_id;

	PrintEnter();

	if (sem == NULL)
	{
		PrintError("Error at %s() %d lines\n",__func__, __LINE__);
		return VK_ERROR;
	}

	nReturnCode = sem_destroy(&sem->sema4);
	if (nReturnCode == SEMA4_ERROR)
	{
		PrintError("Fail to sem_destroy %d lines errno: %d\n", __LINE__, errno);
		return VK_ERROR;
	}
#if defined(VK_SEM_DEBUG)
	TAILQ_REMOVE (&sem_list_head, sem, entries);
#endif

	free(sem);

	PrintExit();

	return VK_OK;
}


int VK_SEM_CreateWithCountEx(unsigned long *sem_id, unsigned long count, const char *name, unsigned char suspend_type)
{
	int nReturnCode = VK_OK;

	PrintEnter();

	if (sem_id == NULL)
	{
		PrintError("Error at %s() %d lines\n",__func__, __LINE__);
		return VK_ERROR;
	}
	if (name == NULL)
	{
		PrintError("Error at %s() %d lines\n",__func__, __LINE__);
		return VK_ERROR;
	}

	if (suspend_type == VK_SUSPEND_FIFO)
	{
		nReturnCode = VK_SEM_Create_Priv(sem_id, count, SEM_SUSPEND_FIFO, name);
	}
	else
	{
		nReturnCode = VK_SEM_Create_Priv(sem_id, count, SEM_SUSPEND_PRIORITY, name);
	}

	PrintExit();

	return nReturnCode;
}

int VK_SEM_DestroyEx(unsigned long sem_id)
{
	int nReturnCode = VK_OK;
	PrintEnter();
	nReturnCode = VK_SEM_Destroy(sem_id);
	PrintExit();
	return nReturnCode;
}


#if defined(VK_SEM_DEBUG)
int VK_DBGSEM_ReleaseEx(unsigned long sem_id, char *file, int line)
{
	int nReturnCode = VK_OK;
	PrintEnter();
	nReturnCode = VK_DBGSEM_Release(sem_id, file, line);
	PrintExit();
	return nReturnCode;
}

int VK_DBGSEM_GetTimeoutEx(unsigned long sem_id, unsigned long timeout, char *file, int line)
{
	int nReturnCode = VK_OK;
	PrintEnter();
	nReturnCode = VK_DBGSEM_GetTimeout(sem_id, timeout, file, line);
	PrintExit();
	return nReturnCode;
}
#else
int VK_SEM_ReleaseEx(unsigned long sem_id)
{
	int nReturnCode = VK_OK;
	PrintEnter();
	nReturnCode = VK_SEM_Release(sem_id);
	PrintExit();
	return nReturnCode;
}


int VK_SEM_GetTimeoutEx(unsigned long sem_id, unsigned long timeout)
{
	int nReturnCode = VK_OK;
	PrintEnter();
	nReturnCode = VK_SEM_GetTimeout(sem_id, timeout);
	PrintExit();
	return nReturnCode;
}
#endif

/*****************************************************************************
 function: 	VK_SEM_InfoPrint
 description:	Print Out of System's Semaphore Information
 argument:	None
 return:	VK_OK
******************************************************************************/
int VK_SEM_InfoPrint(void)
{
	return VK_OK;
}


/*****************************************************************************
 function: 	VK_SEM_PrintSemList
 description:	Print Out of System's Semaphore Information
 argument:	None
 return:	VK_OK
******************************************************************************/
#if defined(VK_SEM_DEBUG)
int VK_SEM_PrintSemList(void)
{
	vk_sem *sem;
	unsigned long i=0;
	int value=0,ulCnt=0;
/*
    sem_id : 세마포어 포인터
    name : 세마포어 이름
    count : 세마포어 카운트
    threadId : 세마포어를 Get한 스레드 ID, 0이면 Get한 스레드가 없는것
*/
	VK_DBG_Print("*****************************************************************************************\n");
	VK_DBG_Print("  sem_id                      name count    threadId          \n");
	for (sem = sem_list_head.tqh_first; sem != NULL; sem = sem->entries.tqe_next)
	{
		//value = sem_getvalue(&sem->sema4, &value);
		if( sem == NULL ) break;
		if( (sem->nCount <= 0)  || (sem->nCount > MAX_TASK_NUM) ) continue;
		VK_DBG_Print("[%03d]%d          %16s %1d  0x%08X %s(%d) tick(%d), nCount(%d)  owner=%d\n", ulCnt,
			(int)sem, sem->name, value,  sem->last_thread_id, sem->lastfile, sem->lastline, sem->tick, sem->nCount, sem->current_owner);
		for( i=0; i<MAX_TASK_NUM; i++ )
		{
			if( sem->task_id[i] != 0 )
				VK_DBG_Print("                                          %d) 0x%08X [%s:%d]\r\n",
					i, sem->task_id[i],sem->waitfile[i],sem->waitline[i]);
		}
		ulCnt++;
	}
	VK_DBG_Print("*****************************************************************************************\n");
	return VK_OK;
}
#endif


