/*
	@file     port_itkos.c
	@brief    port_itkos.c (Redkey2 Porting layer for OS)

	Description: Redkey2에서 요청하는 OS Porting Layer									\n
	Module: MW/mheg/itk 															\n
	Remarks : 																			\n
            1. 여기서 사용하는 Porting Layer의 Local 변수는 itk convention rule에 맞춘다     \n
              그 외 다른 모든 변수의 Convention rule은 OCTO의 형식으로 간다. 			\n
	Copyright (c) 2008 HUMAX Co., Ltd.													\n
	All rights reserved.																\n
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <hlib.h>
#include <vkernel.h>

#include <mheg_port_itk.h>
#include <port_itk_main.h>
#include <port_itk_os.h>

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/

#define MEM_BLOCK_SIZE_AREA_LEN				8							/* size of ITK_MemHandle_t */
#define ITK_OS_NAME_LEN						15

/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/
typedef struct tagITK_ThreadHandle_t
{
	itk_thread_t 					vtbl;

	itk_thread_entry_t 				entry_point;
	void*							argv;
	HULONG 							task_handle;
} ITK_ThreadHandle_t, *ITK_ThreadHandleList_t;

/*실제로 Host에서 할당하는 메모리 Block형태, Client로는 ptr pointer address만 전달  된다 */
typedef struct tagITK_MemHandle_t
{
	HUINT32 				size;
	void*					ptr;
}ITK_MemHandle_t;


typedef struct tagITK_SemaphoreHandle_t
{
	itk_semaphore_t 		vtbl;
	HULONG 				sem_handle;

} ITK_SemaphoreHandle_t;

typedef struct tagITK_TimerHandle_t
{
	itk_timer_t				vtbl;

	unsigned long			count_ms;
	itk_timer_callback_t	callback;
	void 					*callback_context;
	unsigned long			timer_handle;
} ITK_TimerHandle_t;



/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/
static itk_os_t 				s_stOsObject;				/* Object for OS  */


/********************************************************************/
/********************      proto-type functions  ********************/
/********************************************************************/

/************** Local Functions set for HEAP Object ***************************************/
static void*			port_itk_OS_AllocHeap(struct itk_heap_t* thiz, size_t size, const char* info);
static void 			port_itk_OS_FreeHeap(struct itk_heap_t* thiz, void* memory);
static void* 			port_itk_OS_ZallocHeap(struct itk_heap_t* thiz, size_t size, const char* info);
static itk_errcode_t	port_itk_OS_ReleaseHeap(struct itk_heap_t* thiz);
static size_t 			port_itk_OS_GetHeapAllocated(struct itk_heap_t* thiz);
static void* 			port_itk_OS_HeapAllocMemBlock(size_t ulSize);
static HUINT32 			port_itk_OS_HeapFreeMemBlock(void* pPtr);
static HUINT32			port_itk_OS_GetMemBlockSize(void* pPtr);


/************** Local Functions set for THREAD Object ***************************************/
static void				port_itk_OS_ThreadRelease(struct itk_thread_t* thiz);
static itk_errcode_t	port_itk_OS_ThreadJoin(struct itk_thread_t* thiz, itk_wait_t wait);
static void 			port_itk_OS_EntryThread(void* argv);


/************** Local Function sets for SEMAPHORE Object ***********************************/
static void 			port_itk_OS_SignalSemaphore(struct itk_semaphore_t* thiz);
static itk_errcode_t 	port_itk_OS_WaitSemaphore(struct itk_semaphore_t* thiz, itk_wait_t block);
static void 			port_itk_OS_ReleaseSemaphore(struct itk_semaphore_t* thiz);
static void 			port_itk_OS_CallbackTimer(unsigned long ulTimerId, void* context);


/************** Local Function sets for TIMER Object ***********************************/
static void 			port_itk_OS_ReleaseTimer(struct itk_timer_t* thiz);
static void 			port_itk_OS_ControlTimer(struct itk_timer_t* thiz, itk_control_t state);
static void 			port_itk_OS_SetCountTimer(struct itk_timer_t* thiz, itk_uint32_t ms);



/************** Local function for System or ETC *********************************************/





/*******************************************************************/
/********************      Functions       *************************/
/*******************************************************************/
HUINT32 port_itk_OS_GetMemBlockSize(void* pPtr)
{
	ITK_MemHandle_t*		pstMemBlock = NULL;
	//HUINT32					ulSize = 0;

	if(pPtr == NULL)
	{
		HxLOG_Critical("[%s:%d] input parameter is NULL\n", __FUNCTION__, __LINE__);
		HxLOG_Assert(0);

		return 0;
	}

	pstMemBlock = (ITK_MemHandle_t*)((HUINT8*)pPtr - MEM_BLOCK_SIZE_AREA_LEN);
	if(pstMemBlock->ptr != pPtr)
	{
		HxLOG_Critical("[%s:%d] Critical error on memory block (%p)\n", __FUNCTION__, __LINE__, pstMemBlock);
		HxLOG_Assert(0);

		return 0;
	}

	HxLOG_Debug("[%s:%d] mem info address = 0x%x\n", __FUNCTION__, __LINE__, pPtr);

	return pstMemBlock->size;
}

void* port_itk_OS_HeapAllocMemBlock(size_t ulSize)
{
	ITK_MemHandle_t*		pstMemBlock = NULL;
	HULONG					ulMemBlockSize = 0;

	if(ulSize == 0)
	{
		HxLOG_Critical("[%s:%d] invalid allocating size : 0\n", __FUNCTION__, __LINE__);
		HxLOG_Assert(0);

		return NULL;
	}

	/* re-calculate memory block size */
	if((ulMemBlockSize = MEM_BLOCK_SIZE_AREA_LEN + ulSize) < ulSize)
	{
		HxLOG_Critical("[%s:%d] invalid memBlockSize = %d\n", __FUNCTION__, __LINE__, ulMemBlockSize);
		HxLOG_Assert(0);

		return NULL;
	}

	/* allocate memoryblock with memblocksize */
	if((pstMemBlock = (ITK_MemHandle_t*)HLIB_STD_MemAlloc(ulMemBlockSize)) == NULL)
	{
		HxLOG_Critical("[%s:%d] can't create memory block in host, size(%d)\n", __FUNCTION__, __LINE__, ulMemBlockSize);
		HxLOG_Assert(0);

		return NULL;
	}

	HxLOG_Debug("[%s:%d] size : [%d]\n", __FUNCTION__, __LINE__, ulSize);

	/* make mem blok */
	pstMemBlock->size 	= ulSize;
	pstMemBlock->ptr 	= (HUINT8*)pstMemBlock + MEM_BLOCK_SIZE_AREA_LEN;

	return pstMemBlock->ptr;
}

HUINT32 port_itk_OS_HeapFreeMemBlock(void* pPtr)
{
	ITK_MemHandle_t*	pstMemBlock = NULL;
	HUINT32				ulSize = 0;

	/* check validation of input parameters */
	if(pPtr == NULL)
	{
		HxLOG_Critical("[%s:%d] input parameter is NULL\n", __FUNCTION__, __LINE__);
		HxLOG_Assert(0);

		return 0;
	}

	/* get actual starting block of memory created by malloc */
	pstMemBlock = (ITK_MemHandle_t*)((HUINT8*)pPtr - 8);
	if(pstMemBlock == NULL)
	{
		HxLOG_Critical("Memblock has not been created \r\n");
		HxLOG_Assert(0);

		return 0;
	}

	if(pstMemBlock->ptr == pPtr)
	{
		ulSize = pstMemBlock->size;
		HLIB_STD_MemFree(pstMemBlock);
	}
	else
	{
		ulSize = 0;
		HxLOG_Critical("Critical Problems in Memory Block!!! \r\n");
		HxLOG_Assert(0);
	}

	HxLOG_Debug("[%s:%d] pPtr (%p)\n", __FUNCTION__, __LINE__, pPtr);

	return ulSize;
}


void* port_itk_OS_ZallocHeap(struct itk_heap_t* thiz, size_t size, const char* info)
{
	ITK_HeapHandle_t*		pstHeap = NULL;
	void*					pPtr = NULL;

	PARAM_UNUSED(info);

	if((pstHeap = (ITK_HeapHandle_t*)thiz) == NULL)
	{
		HxLOG_Critical("Input parameter is invalid NULL\r\n");
		HxLOG_Assert(0);
		return NULL;
	}

	if(size == 0)
	{
		HxLOG_Critical("allocation fails due to inputparameter size 0 \r\n");
		return NULL;
	}

	/* check availabel heap size */
	if(pstHeap->size < (pstHeap->used + size))
	{
		/* we should notify current memory status to client with callback function registered before */
		if(pstHeap->callback)
		{
			while (pstHeap->callback(pstHeap->callback_context, (itk_heap_t*)pstHeap, size) != ITK_NOT_VALID)
				HxLOG_Critical("pstHeap->size = %d, pstHeap->used = %d, size = %d\n", pstHeap->size, pstHeap->used, size);
		}

		return NULL;
	}

	/* allocate memory block */
	if((pPtr = port_itk_OS_HeapAllocMemBlock(size)) == NULL)
	{
		HxLOG_Critical("can not create memory block size(%d) \r\n", size);
		HxLOG_Assert(0);
		return NULL;
	}

	HxLOG_Debug("[%s:%d] heap size (%d) heap used size (%d) request size (%d)\n", __FUNCTION__, __LINE__, pstHeap->size, pstHeap->used, size);

	/* make all content of memory to be Zero */
	HxSTD_memset(pPtr, 0x00, size);

	/* increase allocated memory size in this heap */
	pstHeap->used += size;

	return pPtr;
}


void* port_itk_OS_AllocHeap(struct itk_heap_t* thiz, size_t size, const char* info)
{
	ITK_HeapHandle_t*		pstHeap = NULL;
	void*					pPtr = NULL;

	PARAM_UNUSED(info);

	if((pstHeap = (ITK_HeapHandle_t*)thiz) == NULL)
	{
		HxLOG_Critical("Input parameter is invalid NULL\r\n");
		HxLOG_Assert(0);
		return NULL;
	}

	if(size == 0)
	{
		HxLOG_Critical("allocation fails due to inputparameter size 0 \r\n");
		return NULL;
	}

	/* check available heap size */
	if(pstHeap->size < (pstHeap->used + size))
	{
		/* we should notify current memory status to client with callback function registered before */
		if(pstHeap->callback)
		{
			while (pstHeap->callback(pstHeap->callback_context, (itk_heap_t*)pstHeap, size) != ITK_NOT_VALID)
			{
				HxLOG_Critical("pstHeap->size = %d, pstHeap->used = %d, size = %d\n", pstHeap->size, pstHeap->used, size);
			}
		}

		return NULL;
	}

	/* allocates memory block along with size for being reqested by client */
	if((pPtr = port_itk_OS_HeapAllocMemBlock(size)) == NULL)
	{
		HxLOG_Critical("can not create memory block size(%d) \r\n", size);
		HxLOG_Assert(0);
		return NULL;
	}

	HxLOG_Debug("[%s:%d] size (%d)\n", __FUNCTION__, __LINE__, size);

	pstHeap->used += size;

	return pPtr;
}


void* port_itk_OS_Realloc(struct itk_heap_t* thiz, void* memory, size_t size)
{
	ITK_HeapHandle_t*	pstHeap = NULL;
	void*				pPtr = NULL;
	size_t				ulPrevBlockSize = 0;

	/* check validation of parameters */
	if(((pstHeap = (ITK_HeapHandle_t*)thiz) == NULL) || size == 0)
	{
		HxLOG_Critical("Invalid parameter thiz(%p), size(%d) \r\n", thiz, size);
		return NULL;
	}

	if(memory == NULL)
	{
		/*
		* S&T Comment : the revised documentation for realloc below:
		*
		* Memory points to a block of memory already allocated from a heap or NULL.
		* In the case of memory being NULL the call allocates memory using the same semantics as alloc.
		*/

		if(pstHeap->size < (pstHeap->used + size))
		{
			HxLOG_Debug("[%s:%d] memory is not enough to allocate size(%d), used(%d0 callback(%p) new size (%d)\n", __FUNCTION__, __LINE__, pstHeap->size, pstHeap->used, pstHeap->callback, size);

			if(pstHeap->callback)
			{
				while (pstHeap->callback(pstHeap->callback_context, (itk_heap_t*)pstHeap, size) != ITK_NOT_VALID)
					HxLOG_Critical("pstHeap->size = %d, pstHeap->used = %d, size = %d\n", pstHeap->size, pstHeap->used, size);
			}

			return NULL;
		}

		/* allocates memory block along with size for being reqested by client */
		if((pPtr = port_itk_OS_HeapAllocMemBlock(size)) == NULL)
		{
			HxLOG_Critical("can not create memory block size(%d) \r\n", size);
			HxLOG_Assert(0);
			return NULL;
		}

		pstHeap->used += size;
	}
	else
	{
		HxLOG_Debug("[%s:%d] size(%d), used(%d), size(%d)\n", __FUNCTION__, __LINE__, pstHeap->size, pstHeap->used, size);

		/* reallocate memory */
		if(pstHeap->size < (pstHeap->used + size))
		{
			HxLOG_Debug("[%s:%d] memory is not enough to allocate size(%d) used(%d) callback(%p) newsize (%d)\n", __FUNCTION__, __LINE__, pstHeap->size, pstHeap->used, pstHeap->callback, size);
			if(pstHeap->callback)
			{
				while (pstHeap->callback(pstHeap->callback_context, (itk_heap_t*)pstHeap, size) != ITK_NOT_VALID)
				{
					HxLOG_Critical("pstHeap->size = %d, pstHeap->used = %d, size = %d\n", pstHeap->size, pstHeap->used, size);
				}
			}

			return NULL;
		}

		if((pPtr = port_itk_OS_HeapAllocMemBlock(size)) == NULL)
		{
			HxLOG_Critical("can not create memory block size(%ld) \r\n", size);
			HxLOG_Assert(0);
			return NULL;
		}

		pstHeap->used += size;

		/*check which block is bigger */
		ulPrevBlockSize = port_itk_OS_GetMemBlockSize(memory);
		if(ulPrevBlockSize)
		{
			if(size > ulPrevBlockSize)
				HxSTD_memcpy(pPtr, memory, ulPrevBlockSize);
			else
				HxSTD_memcpy(pPtr, memory, size);

			ulPrevBlockSize = port_itk_OS_HeapFreeMemBlock(memory);
			pstHeap->used -= ulPrevBlockSize;
		}
		else
			pPtr = 0;

	}

	HxLOG_Debug("[%s:%d] size(%d), used(%d), size(%d)\n", __FUNCTION__, __LINE__, pstHeap->size, pstHeap->used, size);

	return pPtr;
}



void port_itk_OS_FreeHeap(struct itk_heap_t* thiz, void* memory)
{
	ITK_HeapHandle_t*		pstHeap = NULL;
	HUINT32					ulSize = 0;

	if((pstHeap = (ITK_HeapHandle_t*)thiz) == NULL)
	{
		HxLOG_Critical("Error - Heap Object has not been created \r\n");
		return;
	}

	if(memory == NULL)
	{
		HxLOG_Critical("Error - Null pointer free Action \r\n");
		return;
	}

	HxLOG_Debug("Free heap [%p]\n", memory);

	ulSize = port_itk_OS_HeapFreeMemBlock(memory);
	if(pstHeap->used < ulSize)
	{
		HxLOG_Critical("removing size(%d) is bigger that used size(%d) \r\n", ulSize, pstHeap->used);
		HxLOG_Assert(0);
	}

	pstHeap->used -= ulSize;
}

itk_errcode_t port_itk_OS_ReleaseHeap(struct itk_heap_t* thiz)
{
	ITK_HeapHandle_t*		pstHeap = NULL;
	itk_errcode_t			eErrCode;

	if((pstHeap = (ITK_HeapHandle_t*)thiz) == NULL)
	{
		HxLOG_Critical("invalid parameter : NULL \r\n");
		return ITKE_FAIL;
	}

	if(pstHeap->used)
	{
		/* in case there still have been some of allocated memory block*/
		HxLOG_Critical("can not remove heap area due to memory block\r\n");
		eErrCode = ITKE_FAIL;
	}
	else
	{
		HxLOG_Debug("release heap [%p]\n", pstHeap);

		/* free heap hander */
		HLIB_STD_MemFree(pstHeap);
		eErrCode = ITKE_OK;
	}

	return eErrCode;
}

size_t port_itk_OS_GetHeapAllocated(struct itk_heap_t* thiz)
{
	ITK_HeapHandle_t*		pstHeap = NULL;

	if((pstHeap = (ITK_HeapHandle_t*)thiz) == NULL)
	{
		HxLOG_Critical("Invalid Input parameter : NULL \r\n");
		return 0;
	}

	HxLOG_Debug("GetHeap : (%p)\n", pstHeap);

	return pstHeap->used;
}


void port_itk_OS_ThreadRelease(struct itk_thread_t* thiz)
{
	ITK_ThreadHandle_t*		pstThread = NULL;

	/* check validation of parameter */
	if((pstThread = (ITK_ThreadHandle_t*)thiz) == NULL)
	{
		HxLOG_Critical("invalid input param : NULL \r\n");
		return;
	}


	if (pstThread->task_handle != 0)
	{
		// zombie threads
		HxLOG_Critical("_______D__[%s] Zombie Threads !\n",__func__);
	}

	HxLOG_Debug("[%s:%d] thread release (%p)\n", __FUNCTION__, __LINE__, pstThread);

	HLIB_STD_MemFree(pstThread);
	pstThread = NULL;
}

itk_errcode_t port_itk_OS_ThreadJoin(struct itk_thread_t * thiz, itk_wait_t wait)
{
	ITK_ThreadHandle_t*		pstThread = NULL;
	itk_errcode_t				eErrCode = ITKE_OK;

	if((pstThread = (ITK_ThreadHandle_t*)thiz) == NULL)
	{
		HxLOG_Critical("Invalid input parameter : NULL \r\n");
		return ITKE_FAIL;
	}

	HxLOG_Debug("[%s:%d] wait (%d)\n", __FUNCTION__, __LINE__, wait);

	switch(wait)
	{
		case ITK_NO_WAIT:
			/* if the function should return immediately */

			eErrCode = ITKE_NOT_READY;
			break;

		case ITK_WAIT:
			/* if the thread should be waited for before returning */
			/* we need function dealing with pthread_join() !!!!! */
			if (VK_TASK_Join(pstThread->task_handle)!=VK_OK)
			{
				HxLOG_Critical("_______E__[%s] VK_TASK_JoinEx() Error !\n",__func__);
				return ITKE_FAIL;
			}

			break;
		default:
			eErrCode = ITKE_FAIL;
			break;
	}

	return eErrCode;
}

void port_itk_OS_EntryThread(void* argv)
{
	ITK_ThreadHandle_t*	pstThread = (ITK_ThreadHandle_t*)argv;

	if(pstThread)
	{
		HxLOG_Debug("[%s:%d] entry point (%p)\n", __FUNCTION__, __LINE__, pstThread->entry_point);

		if(pstThread->entry_point)
			pstThread->entry_point(pstThread->argv);
	}

}


/* semphore P Operation
   : Waits for a semaphore to be non-zero and decrements */
itk_errcode_t port_itk_OS_WaitSemaphore(struct itk_semaphore_t* thiz, itk_wait_t block)
{
	ITK_SemaphoreHandle_t*		pstSema = NULL;
	itk_errcode_t				eRet = ITKE_OK;

	if((pstSema = (ITK_SemaphoreHandle_t*)thiz) == NULL)
	{
		HxLOG_Critical("Sema Object has not been created yet !!\n");
		HxLOG_Assert(0);
		return ITKE_FAIL;
	}

	HxLOG_Debug("block [%d]\n", block);
	switch(block)
	{
		case ITK_NO_WAIT:
			/* The sem_trywait() function locks the semaphore referenced by sem only if the semaphore is currently not locked;
			    that is, if the semaphore value is currently positive. Otherwise, it does not lock the semaphore */

			if(VK_SEM_GetTimeout(pstSema->sem_handle, 0) == VK_OK)
				eRet = ITKE_OK;
			else
				eRet = ITKE_FAIL;
			break;

		case ITK_WAIT:
#if 0 // 2009_10_21 (swkim) default로 사용하도록 함 defined(CONFIG_3RD_MHEG_UK_PROFILE1) || defined(CONFIG_3RD_MHEG_UK_PROFILE_HD) //let's check later.2009/07/12
			VK_SEM_GetTimeout(pstSema->sem_handle, 0xffffffff);
#else
			VK_SEM_Get(pstSema->sem_handle);
			//VK_SEM_GetTimeout(pstSema->sem_handle, 0xffffffff);
#endif
			/* we consider return value in cas of ITKE_WAIT as a ITK_OK implicitly */
			eRet = ITKE_OK;

			break;

		default:
			HxLOG_Assert(0);
			eRet = ITKE_FAIL;
	}

	return eRet;

}



/* semaphore V Operation */
void port_itk_OS_SignalSemaphore(struct itk_semaphore_t* thiz)
{
	ITK_SemaphoreHandle_t*		pstSema = NULL;

	if((pstSema = (ITK_SemaphoreHandle_t*)thiz) == NULL)
	{
		HxLOG_Critical("Sema Object has not been created yet !!\r\n");
		HxLOG_Assert(0);
		return;
	}

	HxLOG_Debug("signal sem\n");

	/* increases a semphore V value */
	if(VK_SEM_Release(pstSema->sem_handle) != VK_OK)
	{
		HxLOG_Assert(0);
	}
}


void port_itk_OS_ReleaseSemaphore(struct itk_semaphore_t* thiz)
{
	ITK_SemaphoreHandle_t*		pstSema = NULL;
	HUINT32						ulRet = VK_OK;

	if((pstSema = (ITK_SemaphoreHandle_t*)thiz) == NULL)
	{
		HxLOG_Critical("Sema Object has not been created yet !!\r\n");
		HxLOG_Assert(0);
		return;
	}

	HxLOG_Debug("release sem\n");

#if 1 // 2009_10_21 (swkim) default로 사용하도록 함 defined(CONFIG_3RD_MHEG_UK_PROFILE1) || defined(CONFIG_3RD_MHEG_UK_PROFILE_HD) //let's check later.2009/07/12
	// Neo doesn't care uRet. maybe MHEG dosn't return a correct value.
	ulRet = VK_SEM_Release(pstSema->sem_handle);
#else
	while(ulRet == VK_OK)
	{
		/* S&T Document :
		   Authors should endevour to ensure that all threads using the semaphore are prevented from
			accessing the semaphore before it is released */
		ulRet = VK_SEM_Release(pstSema->sem_handle);
	}
#endif

	/* destory semaphore */
	if(VK_SEM_Destroy(pstSema->sem_handle) != VK_OK)
		HxLOG_Assert(0);

	/* remove Semaphore handler */
	HLIB_STD_MemFree(pstSema);
	pstSema = NULL;

	UNUSED(ulRet);
}


void port_itk_OS_CallbackTimer(unsigned long ulTimerId, void* context)
{
	ITK_TimerHandle_t*		pstTimer = NULL;
	HUINT32 param = 0;

	if(context == NULL)
	{
		HxLOG_Assert(0);
		return;
	}

	HxSTD_memcpy(&param, context, sizeof(param));
	pstTimer = (ITK_TimerHandle_t *)param;

	HxLOG_Debug("TimeCallback +++ pstTimer->ulTimerId(0x%x)\n", ulTimerId);

	if(pstTimer)
	{

		if(pstTimer->timer_handle == ulTimerId)
		{
			HxLOG_Debug("TimerCallback : invoke timer ID (0x%x)\n", ulTimerId);

			/*
			* S&T MHEG engine은 callback 안에서 timer->itk_timer->control(timer->itk_timer, ITK_DISABLE);을 호출한다.
			* 이 경우 VK_TIMER_Task에서 VK_TIMER_Cancel를 호출하게 되어 dead lock에 걸린다. 결국 callback 수행 후 VK_TIMER는
			* 자동으로 소멸되므로 미리 timer_handle을 0으로 설정하여 VK_TIMER_Cancel가 호출되지 않도록 한다.
			*/
			pstTimer->timer_handle = 0;
			if(pstTimer->callback)
				pstTimer->callback(pstTimer->callback_context, (itk_timer_t*)pstTimer);

		}
	}

}


void port_itk_OS_ControlTimer(struct itk_timer_t* thiz, itk_control_t state)
{
	ITK_TimerHandle_t*		pstTimer = (ITK_TimerHandle_t*)thiz;
	HUINT32					ulParam = 0;

	if(pstTimer == NULL)
	{
		HxLOG_Critical("invalid parameter !!\r\n");
		HxLOG_Assert(0);
		return;
	}

	HxLOG_Debug("TimerCallback : state (%d) timer_handle(0x%x) count_ms(%d)\n", state, pstTimer->timer_handle, pstTimer->count_ms);

	switch(state)
	{
		case ITK_DISABLE:
			if(pstTimer->timer_handle)
			{
				if(VK_TIMER_Cancel(pstTimer->timer_handle) == VK_OK)
					pstTimer->timer_handle = 0;
				else
					HxLOG_Critical("fail to Cancel timer !! \n\n");
			}
			else
				HxLOG_Debug("TimerCallback : Can't disable timer, Already been DIALBED\n");

			break;

		case ITK_ENABLE:
			ulParam = (HUINT32)pstTimer;
			if(pstTimer->timer_handle)
			{
				/* we must cancel timer that already been created before go further */
				VK_TIMER_Cancel(pstTimer->timer_handle);
			}

			if(VK_TIMER_EventAfter(pstTimer->count_ms, port_itk_OS_CallbackTimer,(void*)&ulParam,
								(int)sizeof(HUINT32), &pstTimer->timer_handle) != VK_OK)
			{
				HxLOG_Critical("Fail to create TIMER !! \r\n");
				pstTimer->timer_handle = 0;		/* set dummy data for a safty */
				HxLOG_Assert(0);
			}

			HxLOG_Debug("TimerCallback : state(%d) timer_handle(0x%x) count_ms(%d)\n", state, pstTimer->timer_handle, pstTimer->count_ms);
			break;

		default:
			HxLOG_Assert(0);
			break;
	}

}


void port_itk_OS_ReleaseTimer(struct itk_timer_t* thiz)
{
	ITK_TimerHandle_t*		pstTimer = NULL;

	/* check validation of input parameter */
	if((pstTimer = (ITK_TimerHandle_t*)thiz) == NULL)
	{
		HxLOG_Assert(0);
		return;
	}

	HxLOG_Debug("Timer ++++\n");

	/* Timers should be disabled before they are released */
	if(pstTimer->timer_handle)
		pstTimer->vtbl.control((itk_timer_t*)pstTimer, ITK_DISABLE);

	HLIB_STD_MemFree(pstTimer);
}

void port_itk_OS_SetCountTimer(struct itk_timer_t* thiz, itk_uint32_t ms)
{
	ITK_TimerHandle_t*		pstTimer = NULL;

	if((pstTimer = (ITK_TimerHandle_t*)thiz) == NULL)
	{
		HxLOG_Critical("invalid paramter !!\r\n");
		return;
	}

	HxLOG_Debug("Timer (ms (%d))\n", ms);

	/* make current timer to be disabled before change time */
	if(pstTimer->timer_handle)
		pstTimer->vtbl.control((itk_timer_t*)pstTimer, ITK_DISABLE);

	/* change timer value with new one */
	pstTimer->count_ms = ms;
}


#define _____PUBLIC_APIs________________________________________________________________



itk_thread_t* PORT_ITK_OS_NewThread(struct itk_os_t *thiz, const char *name, const itk_thread_entry_t entryPoint,
 										void *argv, size_t stackSize, itk_thread_priority_t priority)
{
	ITK_ThreadHandle_t*	pstThread = NULL;
	HUINT32					ulPriority;
	HUINT8					strTaskName[ITK_OS_NAME_LEN + 1];			/* Plus 1 means buffer to store null character */
	static HUINT8			s_ucTaskUnnamedIndex = 0;

	if(thiz == NULL || entryPoint == NULL)
	{
		HxLOG_Critical("Invalid parameter ..............\n");
		return NULL;
	}

	if(stackSize == 0 || stackSize < SIZE_32K)
	{
		HxLOG_Debug("THREAD : input stack size is 0, make it default stack size\n");
		stackSize = SIZE_32K;
	}

	/* change priorities to OCTOs */
	switch(priority)				// <== check point #54
	{
		case ITK_PRIORITY_LOW:
			ulPriority = (VK_TASK_PRIORITY_MW_REF - 2);
			break;

		case ITK_PRIORITY_LOWER:
			ulPriority = (VK_TASK_PRIORITY_MW_REF - 1);
			break;
		case ITK_PRIORITY_NORMAL:
			ulPriority = VK_TASK_PRIORITY_MW_REF;	/* Task Priority that sames with GWM */
			break;
		case ITK_PRIORITY_HIGH:
			ulPriority = (VK_TASK_PRIORITY_MW_REF + 1);
			break;

		case ITK_PRIORITY_REALTIME:
			ulPriority = (VK_TASK_PRIORITY_MW_REF + 5);
			break;

		default:
			ulPriority = VK_TASK_PRIORITY_MW_REF;
			HxLOG_Critical("invalid Priotiry(%d)-> enfore it to be normal one explicitly \r\n",priority);
			break;

	}

	/* set task name */
	HxSTD_memset(strTaskName, 0x00, ITK_OS_NAME_LEN+1);
	if(name == NULL)
	{
		if(s_ucTaskUnnamedIndex > 250)
			s_ucTaskUnnamedIndex = 0;
		else
			s_ucTaskUnnamedIndex++;

		snprintf(strTaskName, ITK_OS_NAME_LEN+1, "ITK_TASK%03d", s_ucTaskUnnamedIndex);
		strTaskName[ITK_OS_NAME_LEN] = '\0';

	}
	else
	{
		HLIB_STD_StrNCpySafe(strTaskName, name, ITK_OS_NAME_LEN);
	}

	/* make and create new thread */
	pstThread = (ITK_ThreadHandle_t*)HLIB_STD_MemAlloc(sizeof(ITK_ThreadHandle_t));
	if(pstThread == NULL)
	{
		HxLOG_Assert(0);
		return NULL;
	}

	pstThread->vtbl.join	= port_itk_OS_ThreadJoin;
	pstThread->vtbl.release	= port_itk_OS_ThreadRelease;
	pstThread->entry_point	= entryPoint;
	pstThread->argv			= argv;
	pstThread->task_handle	= (HUINT32)VK_TASK_JOINABLE;

	/* start Task */
	if(VK_TASK_Create(port_itk_OS_EntryThread, ulPriority, stackSize + (1<<16), strTaskName, pstThread, &pstThread->task_handle, 1) != VK_OK)
	{
		HxLOG_Critical("Task creating fails !!!! \r\n");
		HxLOG_Assert(0);

		HLIB_STD_MemFree(pstThread);
		return NULL;
	}

	/* start Task */
	VK_TASK_Start(pstThread->task_handle);

	HxLOG_Debug("[PORT_ITK_OS_NewThread] Task %s is created !! \r\n", strTaskName);

	return (itk_thread_t*)pstThread;

}

void PORT_ITK_OS_Sleep(struct itk_os_t* thiz, itk_uint32_t ms)
{
	if(thiz == NULL)
	{
		HxLOG_Critical("OS object has not been created yet !!! \r\n");
		HxLOG_Assert(0);
		return;
	}

	HxLOG_Debug("sleep (%d)\n", ms);

	/* sleep for ms */
	/* A value of 0 indicates that the thread should relinquish the remainder
	   of its timeslice and be rescheduled*/
	VK_TASK_Sleep(ms);
}


itk_semaphore_t* PORT_ITK_OS_NewSemaphore(struct itk_os_t* thiz, const char* name, itk_uint16_t value)
{
	ITK_SemaphoreHandle_t*		pstSema = NULL;
	char						strSemName[ITK_OS_NAME_LEN + 1];
	static HUINT8				s_ucSemaNameIndex = 0;

	/* checks validation of input parameter */
	if(thiz == NULL)
	{
		HxLOG_Critical("OS Object has not been created yet !!!! \r\n");
		return NULL;
	}

	HxLOG_Debug("Sem name (%s)\n", name);

	/* set semaphore name */
	HxSTD_memset(strSemName, 0x00, ITK_OS_NAME_LEN + 1);		/* extra 1 byte is a room for NULL character */
	if(name == NULL)
	{
		if(s_ucSemaNameIndex > 250)
			s_ucSemaNameIndex = 1;
		else
			s_ucSemaNameIndex++;

		snprintf(strSemName, ITK_OS_NAME_LEN+1, "ITK_SEM%03d", s_ucSemaNameIndex);
		strSemName[ITK_OS_NAME_LEN] = '\0';
	}
	else
	{
		HUINT32		ulLen = 0;

		ulLen = strlen(name);
		if(ulLen >= ITK_OS_NAME_LEN)
			strncpy(strSemName, name, ITK_OS_NAME_LEN);
		else
			strncpy(strSemName, name, ulLen);
	}

	/* creates buffer for semaphore handle and initalizes~ */
	if((pstSema = (ITK_SemaphoreHandle_t*)HLIB_STD_MemAlloc(sizeof(ITK_SemaphoreHandle_t))) == NULL)
	{
		HxLOG_Assert(0);
		return NULL;
	}

	pstSema->vtbl.release	= port_itk_OS_ReleaseSemaphore;
	pstSema->vtbl.signal	= port_itk_OS_SignalSemaphore;
	pstSema->vtbl.wait		= port_itk_OS_WaitSemaphore;

	if(VK_SEM_CreateWithCountEx(&pstSema->sem_handle, value, strSemName, VK_SUSPEND_FIFO) != VK_OK)
	{
		HxLOG_Critical("fail to create Semaphore !!!!\r\n");
		HxLOG_Assert(0);

		HLIB_STD_MemFree(pstSema);
		return NULL;
	}

	HxLOG_Debug("[PORT_ITK_OS_NewSemaphore] Success to create Semaphore : %s \r\n", strSemName);

	return (itk_semaphore_t*)pstSema;
}



itk_heap_t* PORT_ITK_OS_NewHeap(struct itk_os_t* thiz, const char* name, size_t capacity,
								itk_heap_callback_t callback, void* callbackContext)
{
	ITK_HeapHandle_t*		pstHeap = NULL;

	HxLOG_Debug("mem info : capacity(%d) name(%s)\n", capacity, name);

	/* check validation of input parameter */
	if(thiz == NULL || callback == NULL)
	{
		HxLOG_Critical("Invalid input parameter !!! \r\n");
		return NULL;
	}

	if(capacity == 0)
	{
		HxLOG_Critical("invalid heap capacity --> 0 \r\n");
		return NULL;
	}

	/* create Heap handle instance */
	if((pstHeap = (ITK_HeapHandle_t*)HLIB_STD_MemAlloc(sizeof(ITK_HeapHandle_t))) == NULL)
	{
		HxLOG_Assert(0);
		return NULL;
	}

	/* fill out heap instance's attributes and actions */
	pstHeap->vtbl.release		= port_itk_OS_ReleaseHeap;
	pstHeap->vtbl.free			= port_itk_OS_FreeHeap;
	pstHeap->vtbl.getAllocated	= port_itk_OS_GetHeapAllocated;
	pstHeap->vtbl.alloc			= port_itk_OS_AllocHeap;
	pstHeap->vtbl.zalloc		= port_itk_OS_ZallocHeap;
	pstHeap->vtbl.realloc		= port_itk_OS_Realloc;

	pstHeap->callback			= callback;
	pstHeap->size				= capacity;
	pstHeap->callback_context	= callbackContext;
	pstHeap->used				= 0;


	HxLOG_Debug("[PORT_ITK_OS_NewHeap] Success OS heap Objecgt !!! \r\n");

	return (itk_heap_t*)pstHeap;

}


/* we don't care of paraeter name in this function */
itk_timer_t* PORT_ITK_OS_NewTimer(struct itk_os_t* thiz, const char* name, itk_uint32_t countMs,
									itk_timer_callback_t callback, void* callbackContext)
{
	ITK_TimerHandle_t*		pstTimer = NULL;

	/* check validation of input parameter */
	if(thiz == NULL || callback == NULL)
	{
		HxLOG_Critical("invalid paramter !!!\r\n");
		return NULL;
	}

	HxLOG_Debug("Timer : name (%s)\n", name);

	/* create timer handler */
	if((pstTimer = (ITK_TimerHandle_t*)HLIB_STD_MemAlloc(sizeof(ITK_TimerHandle_t))) == NULL)
	{
		HxLOG_Critical("fail to allocate memory !!!!\r\n");
		HxLOG_Assert(0);
		return NULL;
	}

	pstTimer->vtbl.control	= port_itk_OS_ControlTimer;
	pstTimer->vtbl.release	= port_itk_OS_ReleaseTimer;
	pstTimer->vtbl.setCount	= port_itk_OS_SetCountTimer;

	pstTimer->callback			= callback;
	pstTimer->callback_context	= callbackContext;
	pstTimer->count_ms			= countMs;
	pstTimer->timer_handle		= 0;	/* timer handle is created in function ControlTimer with parameter ENABLE */

	return (itk_timer_t*)pstTimer;

}


itk_thread_t* PORT_ITK_OS_GetRunningThread(struct itk_os_t* thiz)
{
	struct itk_thread_t *pstThread = NULL;
	unsigned long ulTaskId = 0;

	if (thiz == NULL)
	{
		HxLOG_Critical("_______E__[%s] thiz is NULL !\n",__func__);
		return NULL;
	}

	if(VK_TASK_GetCurrentId(&ulTaskId) != VK_OK)
	{
		HxLOG_Critical("fail to get current id !!!\r\n");
		return NULL;
	}

	VK_TASK_GetArgument(ulTaskId, (void **)&pstThread);

	return (itk_thread_t*)pstThread;
}
void PORT_ITK_OS_Init()
{
}

HERROR PORT_ITK_OS_CreateObject()
{
	s_stOsObject.newThread			= PORT_ITK_OS_NewThread;
	s_stOsObject.newHeap			= PORT_ITK_OS_NewHeap;
	s_stOsObject.newSemaphore		= PORT_ITK_OS_NewSemaphore;
	s_stOsObject.newTimer			= PORT_ITK_OS_NewTimer;
	s_stOsObject.sleep				= PORT_ITK_OS_Sleep;
	s_stOsObject.getRunningThread	= PORT_ITK_OS_GetRunningThread;

	PORT_ITK_OS_RegisterObject();
	return ERR_OK;
}

void PORT_ITK_OS_RegisterObject()
{
	ITK_registerManager(ITK_OS, &s_stOsObject);
}

