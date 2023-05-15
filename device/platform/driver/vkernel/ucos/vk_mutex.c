#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <assert.h>
#include <pthread.h>
#include <sys/queue.h>
#include <unistd.h>
#include <sys/time.h>
#include <semaphore.h>
#include <errno.h>

#include "htype.h"
#include "vkernel.h"

#include <linden_trace.h>

#define DEBUG_MODULE			TRACE_MODULE_VK

#define MAX_NAME_OF_MUTEX 16

typedef struct {
	pthread_mutex_t mutex_ptr;
	pthread_mutexattr_t mutexAttr_ptr;
	char ucName[MAX_NAME_OF_MUTEX];
	unsigned long taskId;
	unsigned char bIsUsing;
}vk_mutex_t;

void P_GET_FreeMutex(vk_mutex_t **freeMutex)
{

	HAPPY(freeMutex);
	return;
}

int VK_MUTEX_Create(unsigned long *mutex_id, const char *name)
{

	HAPPY(mutex_id);
	HAPPY(name);

	return VK_OK;
}

int VK_MUTEX_Destroy(unsigned long mutex_id)
{

	HAPPY(mutex_id);
	return VK_OK;
}

int VK_MUTEX_Lock(unsigned long mutexId)
{

	HAPPY(mutexId);
	return VK_OK;
}

int VK_MUTEX_Trylock(unsigned long mutexId)
{

	HAPPY(mutexId);
	return VK_OK;
}

int VK_MUTEX_Unlock(unsigned long mutexId)
{

	HAPPY(mutexId);
	return VK_OK;
}

void VK_MUTEX_PrintList(void)
{

}

