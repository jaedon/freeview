#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#include <semaphore.h>
#include <errno.h>

#include "htype.h"
#include "vkernel.h"

#define DEBUG_MODULE			TRACE_MODULE_VK

#ifdef MUTEX_DEBUG
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_DBG_Print("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_DBG_Print("(-)%s\n", __FUNCTION__)
#else 
#define PrintDebug(fmt, ...)    
#define PrintError(fmt, ...)		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...) 							
#define PrintExit(...)						
#endif /* MUTEX_DEBUG */



#define MAX_NAME_OF_MUTEX 16
#define MAX_NUMBER_OF_MUTEX 128

typedef struct {
	pthread_mutex_t mutex_ptr;
	pthread_mutexattr_t mutexAttr_ptr;
	char ucName[MAX_NAME_OF_MUTEX];
	unsigned long taskId;
	unsigned char bIsUsing;
}vk_mutex_t;

static vk_mutex_t g_mutexInfo[MAX_NUMBER_OF_MUTEX];

void P_GET_FreeMutex(vk_mutex_t **freeMutex)
{
	unsigned char i;

	for(i=0; i<MAX_NUMBER_OF_MUTEX; i++)
	{
		if(!g_mutexInfo[i].bIsUsing)
		{
			*freeMutex = &g_mutexInfo[i];
			return;
		}
	}

	*freeMutex = NULL;

	return;
}

int VK_MUTEX_Create(unsigned long *mutex_id, const char *name)
{
	int ret = 0;
	unsigned char name_len;
	unsigned long task_id;
	vk_mutex_t *pMutex = NULL;

	P_GET_FreeMutex(&pMutex);
	if (pMutex == NULL)
	{
		PrintError("%s:%d Mutex creation failed, pMutex = %x\n", __FUNCTION__, __LINE__, pMutex);
		return VK_ERROR;
	}

	ret = pthread_mutexattr_init(&pMutex->mutexAttr_ptr);
	if (ret != 0)
	{
		PrintError("%s:%d Mutex creation failed(%d)\n", __FUNCTION__, __LINE__, ret);
		return VK_ERROR;
	}

	ret = pthread_mutexattr_settype(&pMutex->mutexAttr_ptr, PTHREAD_MUTEX_RECURSIVE);
	if (ret != 0)
	{
		pthread_mutexattr_destroy(&pMutex->mutexAttr_ptr);
		PrintError("%s:%d Mutex creation failed(%d)\n", __FUNCTION__, __LINE__, ret);
		return VK_ERROR;
	}

	ret = pthread_mutex_init(&pMutex->mutex_ptr, &pMutex->mutexAttr_ptr);
	if (ret != 0)
	{
		pthread_mutexattr_destroy(&pMutex->mutexAttr_ptr);
		PrintError("%s:%d Mutex creation failed(%d)\n", __FUNCTION__, __LINE__, ret);
		return VK_ERROR;
	}

	name_len = strlen(name);
	if(name_len > MAX_NAME_OF_MUTEX -1)
	{
		name_len = MAX_NAME_OF_MUTEX -1;
	}

	strncpy(pMutex->ucName, name, name_len);
	pMutex->bIsUsing = TRUE;
	ret = VK_TASK_GetCurrentId(&task_id);
	if(ret != VK_OK)
	{
		return VK_ERROR;
	}
	pMutex->taskId = task_id;
	*mutex_id = (unsigned long)pMutex;

	return VK_OK;
}

int VK_MUTEX_Destroy(unsigned long mutex_id)
{
	int ret;
	vk_mutex_t *pMutex;

	pMutex = (vk_mutex_t*)mutex_id;
	if (pMutex == NULL)
	{
		PrintError("%s:%d Mutex id is NULL\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	ret = pthread_mutexattr_destroy(&pMutex->mutexAttr_ptr);
	if (ret != 0)
	{
		PrintError("%s:%d Mutex destroy failed(%d)\n", __FUNCTION__, __LINE__, ret);
		return VK_ERROR;
	}

	ret = pthread_mutex_destroy(&pMutex->mutex_ptr);
	if (ret != 0)
	{
		PrintError("%s:%d Mutex destroy failed(%d)\n", __FUNCTION__, __LINE__, ret);
		return VK_ERROR;
	}

	pMutex->bIsUsing = FALSE;
	return VK_OK;
}

int VK_MUTEX_Lock(unsigned long mutexId)
{
	int ret;
	vk_mutex_t *pMutex;

	pMutex = (vk_mutex_t*)mutexId;
	if (pMutex == NULL)
	{
		PrintError("%s:%d Mutex id is NULL\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	ret = pthread_mutex_lock(&pMutex->mutex_ptr);
	if (ret != 0)
	{
		PrintError("%s:%d Requested mutex lock failed(%d)\n", __FUNCTION__, __LINE__, ret);
		return VK_ERROR;
	}

	return VK_OK;
}

int VK_MUTEX_Trylock(unsigned long mutexId)
{
	int ret;
	vk_mutex_t *pMutex;

	pMutex = (vk_mutex_t*)mutexId;
	if (pMutex == NULL)
	{
		PrintError("%s:%d Mutex id is NULL\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	ret = pthread_mutex_trylock(&pMutex->mutex_ptr);
	if (ret != 0)
	{
		PrintError("%s:%d Requested try mutex lock failed(%d)\n", __FUNCTION__, __LINE__, ret);
		return VK_ERROR;
	}

	return VK_OK;
}

int VK_MUTEX_Unlock(unsigned long mutexId)
{
	int ret;
	vk_mutex_t *pMutex;

	pMutex = (vk_mutex_t*)mutexId;
	if (pMutex == NULL)
	{
		PrintError("%s:%d Mutex id is NULL\n", __FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	ret = pthread_mutex_unlock(&pMutex->mutex_ptr);
	if (ret != 0)
	{
		PrintError("%s:%d Requested mutex unlock failed(%d)\n", __FUNCTION__, __LINE__, ret);
		return VK_ERROR;
	}

	return VK_OK;
}

void VK_MUTEX_PrintList(void)
{
	unsigned char i;

	for(i=0; i<MAX_NUMBER_OF_MUTEX; i++)
	{
		if(g_mutexInfo[i].bIsUsing)
		{
			VK_DBG_Print("[%d]NAME : %s, TASK_ID : %lu, mutex_ptr = %p\n", i, g_mutexInfo[i].ucName, g_mutexInfo[i].taskId, (void *)&g_mutexInfo[i].mutex_ptr);
		}
	}
}

