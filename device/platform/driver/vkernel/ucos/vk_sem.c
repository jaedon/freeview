
#include "vkernel.h"

#include <linden_trace.h>

#include "os.h"
#include "os_cfg_app.h"
#include "taskdef.h"

#define DEBUG_MODULE		TRACE_MODULE_VK_SEM

//#include "bkni.h"
#define MAX_SEM_INVALID_ID	0x70000000

#define UNUSED(x) (void)(x)

#define MAX_SEM_COUNT 			1024	//need to review :: OS_MAX_SEMAPHORES
#define SEMA_BINARY				1

#if defined(VK_SEM_DEBUG)
#define MAX_SEM_NAMELENGTH 		16
#define MAX_SEM_FILENAMELENGTH 	256
#define UCOS_TASK_MAX_NUM		60	//need to review :: OS_MAX_TASKS

typedef struct vk_sem
{
	OS_SEM 				hSemId;
	char 				name[MAX_SEM_NAMELENGTH];
	unsigned long 		last_task_id;
	char 				waitfile[UCOS_TASK_MAX_NUM][MAX_SEM_FILENAMELENGTH];
	int 				waitline[UCOS_TASK_MAX_NUM];
	char 				lastfile[MAX_SEM_FILENAMELENGTH];
	int 				lastline;
	unsigned long 		tick;
	int 				nCount;
	unsigned long 		task_id[UCOS_TASK_MAX_NUM];
}vk_sem;

static vk_sem *pVkSemList[MAX_SEM_COUNT];
#endif

typedef struct ucos_sem_t
{
	OS_SEM 	hSemId;
	HBOOL 	isUsed;
}ucos_sem_t;

static ucos_sem_t ucos_sem_array[MAX_SEM_COUNT];
int timeoutMsec = 10;

#define MS_TO_TICKS(x)      ((x * OS_CFG_TICK_RATE_HZ)/ 1000)


extern HINT8 *VK_DBG_OSErr2Str(HINT32 ucosErr/* OS_ERR */);


/*****************************************************************************
 function: 	VK_SEM_Init
 description:	init sem for debugging
 argument:
 return:
******************************************************************************/
int VK_SEM_Init(void)
{
	int i;

	for(i = 0; i < MAX_SEM_COUNT; i++)
	{
		ucos_sem_array[i].isUsed = 0;
	}

#if defined(VK_SEM_DEBUG)
	for(i = 0; i < MAX_SEM_COUNT; i++)
	{
		pVkSemList[i] = NULL;
	}
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
	return VK_SEM_CreateWithCount(sem_id, SEMA_BINARY, name, etSuspendType);
}


/*****************************************************************************
 function: 	VK_SEM_CreateWithCount
 description:	create a binary semaphore
 argument:
 return:
******************************************************************************/
int VK_SEM_CreateWithCount(unsigned long *sem_id, unsigned long count, const char *name, VK_SUSPENDTYPE etSuspendType)
{
	OS_ERR		err;
	int 		i;
	int 		sem_index;

#if defined(VK_SEM_DEBUG)
	vk_sem		*pstSema = NULL;
#endif

	HAPPY(etSuspendType);

	PrintEnter("VK_SEM_CreateWithCount()");
	PrintDebug("[%s:%d] VK_SEM_Create(%s) count(%d)\r\n", __FUNCTION__, __LINE__, name, count);
	if (sem_id == NULL)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	*sem_id = MAX_SEM_INVALID_ID;
	for(i=1; i<MAX_SEM_COUNT; i++)
	{
		if(ucos_sem_array[i].isUsed == 0)
		{
			ucos_sem_array[i].isUsed = 1;
			sem_index = i;
			break;
		}
	}

	if(i >= MAX_SEM_COUNT)
	{
		PrintError("[%s:%d] out of sem count, name(%s)\n",__FUNCTION__, __LINE__, name);
		return VK_ERROR;
	}

#if defined(VK_SEM_DEBUG)
	if(VK_strlen(name) > MAX_SEM_NAMELENGTH)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	pstSema = (vk_sem *)VK_MEM_Alloc(sizeof(vk_sem));
	if (pstSema == NULL)
	{
		*sem_id = 0;
		PrintError("Fail to VK_MEM_Alloc Sema %d lines\n", __LINE__);
		return VK_ALLOC_ERROR;
	}
#endif

	//* June :: Create Semaphore *//
	OSSemCreate(&ucos_sem_array[sem_index].hSemId,
				(char *)name,
				count,
				&err);
	if (err != OS_ERR_NONE)
	{
#if defined(VK_SEM_DEBUG)
		VK_MEM_Free((void *)pstSema);
#endif
		PrintError("[%s:%d] OSSemCreate Error at - error(%d, %s)\n",__FUNCTION__, __LINE__, err, VK_DBG_OSErr2Str(err));
		return VK_ERROR;
	}

	PrintDebug("sem_index : %d, name : %s, type : %d\n", sem_index, name, ucos_sem_array[sem_index].hSemId.Type);
	*sem_id = sem_index;

#if defined(VK_SEM_DEBUG)
	VK_memset( pstSema, 0, sizeof(vk_sem) );

	pstSema->hSemId = hSemHandle;
	VK_strncpy(pstSema->name, name , VK_strlen(name));
	pstSema->last_task_id = 0;
	pstSema->nCount = 0;

	for( i = 0; i < MAX_SEM_COUNT; i++ )
	{
		pstSema->task_id[i] = 0;
	}

	for( i = 0; i < MAX_SEM_COUNT; i++ )
	{
		if(pVkSemList[i] == NULL)
		{
			pVkSemList[i] = pstSema;
			break;
		}
	}

	if(i == MAX_SEM_COUNT)
	{
		VK_MEM_Free((void *)pstSema);

		//* June :: Destroy Semaphore *//
		OSSemDel(&hSemHandle,
				  OS_OPT_DEL_NO_PEND,
				  &err);
		 if (err != OS_ERR_NONE) {
			 PrintError("[%s:%d] Error at - error(%d, %s)\n",__FUNCTION__, __LINE__, err, VK_DBG_OSErr2Str(err));
			 return VK_ERROR;
		 }

		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}
#endif

	PrintExit("VK_SEM_CreateWithCount()");

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

	HAPPY(sem_id);
	HAPPY(file);
	HAPPY(line);

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

	HAPPY(sem_id);
	HAPPY(timeout);
	HAPPY(file);
	HAPPY(line);

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

	HAPPY(sem_id);
	HAPPY(file);
	HAPPY(line);

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
	OS_ERR		err;
	CPU_TS		ts;

	PrintEnter("VK_SEM_Get()");

#if 0//defined(VK_CALLBACK_DEBUG)
	VK_TASK_CheckCallback();
#endif

	if(sem_id >= MAX_SEM_COUNT)
	{
		PrintError("[%s:%d] sem_id >= MAX_SEM_COUNT, semid(%d)\n",__FUNCTION__, __LINE__, sem_id);
		return VK_ERROR;
	}

	if(0 == ucos_sem_array[sem_id].isUsed)
	{
		PrintError("[%s:%d] semid(%d) is not initialized\n",__FUNCTION__, __LINE__, sem_id);
		return VK_ERROR;
	}

	//* June :: Pend Semaphore *//
	OSSemPend(&ucos_sem_array[sem_id].hSemId,
			  0,
			  OS_OPT_PEND_BLOCKING,
			  &ts,
			  &err);
	if ((err != OS_ERR_NONE) && (err != OS_ERR_TIMEOUT)) {
		PrintError("[%s:%d] Error at - error(%d, %s)\n",__FUNCTION__, __LINE__, err, VK_DBG_OSErr2Str(err));
		return VK_ERROR;
	}

	PrintExit("VK_SEM_Get()");

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
	OS_ERR		err;
	int 			ticks;
	CPU_TS		ts;

	PrintEnter("VK_SEM_GetTimeout()");

	if(timeout == 0xFFFFFFFFUL)
	{
		PrintExit("VK_SEM_GetTimeout() Infinite -> VK_SEM_Get()");
		return VK_SEM_Get(sem_id);	
	}
	
	ticks = MS_TO_TICKS(timeout);
	if(ticks == 0)
	{
		ticks = 1;
		PrintDebug("ticks : %d\n", ticks);
	}

	if(sem_id >= MAX_SEM_COUNT)
	{
		PrintError("[%s:%d] sem_id >= MAX_SEM_COUNT, semid(%d)\n",__FUNCTION__, __LINE__, sem_id);
		return VK_ERROR;
	}

	if(0 == ucos_sem_array[sem_id].isUsed)
	{
		PrintError("[%s:%d] do not create sem, semid(%d)\n",__FUNCTION__, __LINE__, sem_id);
		return VK_ERROR;
	}

	//* June :: Pend Semaphore *//
	OSSemPend(&ucos_sem_array[sem_id].hSemId,
			  ticks,
			  OS_OPT_PEND_BLOCKING,
			  &ts,
			  &err);
			  
	if(err == OS_ERR_NONE)
	{
		PrintExit("VK_SEM_GetTimeout()");
		return VK_OK;
	}
	else if(err != OS_ERR_TIMEOUT)
	{
		PrintError("[%s:%d] Error at - error(%d, %s)\n",__FUNCTION__, __LINE__, err, VK_DBG_OSErr2Str(err));
		return VK_INVALID_ID;
	}

	PrintExit("VK_SEM_GetTimeout()");
	
	return VK_TIMEOUT;
}

/********************************************************************
 function: 	VK_SEM_Release
 description:	release a semaphore token
 argument:
 return:
*********************************************************************/
int VK_SEM_Release(unsigned long sem_id)
{
	OS_ERR		err;

	PrintEnter("VK_SEM_Release()");

	if(sem_id >= MAX_SEM_COUNT)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	if(0 == ucos_sem_array[sem_id].isUsed)
	{
		PrintError("[%s:%d] do not create sem, semid(%d)\n",__FUNCTION__, __LINE__, sem_id);
		return VK_ERROR;
	}

	//* June :: Post Semaphore *//
    OSSemPost(&ucos_sem_array[sem_id].hSemId,
                OS_OPT_POST_1,
                &err);
    if (err != OS_ERR_NONE) {
		PrintError("[%s:%d] Error at - error(%d, %s)\n",__FUNCTION__, __LINE__, err, VK_DBG_OSErr2Str(err));
		return VK_ERROR;
    }

	PrintExit("VK_SEM_Release()");

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
	OS_ERR		err;
#if defined(VK_SEM_DEBUG)
	int 		i;
#endif

	PrintEnter("VK_SEM_Destroy()");

	if(sem_id >= MAX_SEM_COUNT)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}

	//* June :: Destroy Semaphore *//
	OSSemDel(&ucos_sem_array[sem_id].hSemId,
			  OS_OPT_DEL_NO_PEND,
			  &err);
	if (err != OS_ERR_NONE) {
		PrintError("[%s:%d] Error at - error(%d, %s)\n",__FUNCTION__, __LINE__, err, VK_DBG_OSErr2Str(err));
		return VK_ERROR;
	}

	ucos_sem_array[sem_id].isUsed = 0;

#if defined(VK_SEM_DEBUG)
	for( i = 0; i < MAX_SEM_COUNT; i++ )
	{
		if(pVkSemList[i]->hSemId == (OS_SEM)sem_id)
		{
			VK_MEM_Free((void *)pVkSemList[i]);
			pVkSemList[i] = NULL;
			break;
		}
	}

	if(i == MAX_SEM_COUNT)
	{
		PrintError("Error at %s() %d lines\n",__FUNCTION__, __LINE__);
		return VK_ERROR;
	}
#endif

	PrintExit("VK_SEM_Destroy()");

	return VK_OK;
}


int VK_SEM_CreateWithCountEx(unsigned long *sem_id, unsigned long count, const char *name, unsigned char suspend_type)
{

	HAPPY(sem_id);
	HAPPY(count);
	HAPPY(name);
	HAPPY(suspend_type);

	return VK_OK;
}

int VK_SEM_DestroyEx(unsigned long sem_id)
{

	HAPPY(sem_id);
	return VK_OK;
}


#if defined(VK_SEM_DEBUG)
int VK_DBGSEM_ReleaseEx(unsigned long sem_id, char *file, int line)
{
	HAPPY(sem_id);
	HAPPY(file);
	HAPPY(line);

	return VK_OK;

}

int VK_DBGSEM_GetTimeoutEx(unsigned long sem_id, unsigned long timeout, char *file, int line)
{
	HAPPY(sem_id);
	HAPPY(timeout);
	HAPPY(file);
	HAPPY(line);

	return VK_OK;

}
#else
int VK_SEM_ReleaseEx(unsigned long sem_id)
{
	HAPPY(sem_id);
	return VK_OK;
}


int VK_SEM_GetTimeoutEx(unsigned long sem_id, unsigned long timeout)
{
	HAPPY(sem_id);
	HAPPY(timeout);

	return VK_OK;
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

	vk_sem 			*sem = NULL;
	unsigned long 	i = 0, j = 0;
	int 			ulCnt = 0;

	DI_UART_Print("*****************************************************************************************\n");
	for( i = 0; i < MAX_SEM_COUNT; i++ )
	{
		sem = pVkSemList[i];
		if( (sem != NULL))
		{
			DI_UART_Print("[%03d]  SEM_ID (0x%08X)	  Name :%16s	Last_TASK (%02d)	%s(%d)	 tick(%d)	 Count(%d )\n",
				ulCnt, sem->hSemId, sem->name, sem->last_task_id, sem->lastfile, sem->lastline, sem->tick, sem->nCount);

			for( j = 0; j < UCOS_TASK_MAX_NUM; j++ )
			{
				if( sem->task_id[j] != 0 )
				{
					DI_UART_Print(" 	   -> [%03d] TASK (%02d) [%s:%d] \r\n", j, sem->task_id[j], sem->waitfile[j], sem->waitline[j]);
				}
			}
			ulCnt++;
		}
	}
	DI_UART_Print("*****************************************************************************************\n");

	return VK_OK;

}
#endif


