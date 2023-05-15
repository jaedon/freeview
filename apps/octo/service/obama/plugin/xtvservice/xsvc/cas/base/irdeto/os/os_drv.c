
#include "octo_common.h"
#include "s3_osdrv.h"
#include "s3_cadrv.h"

#include "os_drv.h"

#include "ir_platform.h"

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/
// TODO: 일단 OSDRV 전과정 Trace 한 이후에 Error 구분 하도록 한다..

/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/





//#define	IR_MEMORY_DEBUG
//#define	TASK_DEBUG
//#define	SEMAPHORE_DEBUG
//#define	MESSAGE_DEBUG

/********************************************************************
*	External variables
********************************************************************/

/********************************************************************
*	External function prototypes
********************************************************************/

/********************************************************************
*	Global variables
********************************************************************/


/********************************************************************
*	Static variables
********************************************************************/
static HUINT16				s_usSCellTaskCnt = 0;
static IR_OS_task_info		s_stSCellTaskInfo[8];

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
/********************************************************************
*	function name :	 OS_DRV_AllocateMemory
*	input : allocation request size
*	output : allocated memory pointer
*	description :
********************************************************************/
void *OS_DRV_AllocateMemory( ia_uint32 uiSize )
{
	void *pData;

	if ( uiSize > 0 )
	{
		pData = OxCAS_Malloc( uiSize );
		if (pData != NULL)
		{
			memset(pData, 0, uiSize);
		}
		else
		{
			HxLOG_Print("FAILED, size=%d\n", uiSize);
		}
	}
	else
	{
		pData = NULL;
		HxLOG_Print("requested size is ZERO\n");
	}

#ifdef  IR_MEMORY_DEBUG
	HxLOG_Info("address(0x%X) size(%d)\n", (unsigned int *)pData, (unsigned int *)uiSize) );
#endif

	return ( pData );
}

/********************************************************************
*	function name :	 OS_DRV_FreeMemory
*	input : free request pointer
*	output : none
*	description :
********************************************************************/
void OS_DRV_FreeMemory( void *pvMemory )
{
#ifdef  IR_MEMORY_DEBUG
	HxLOG_Info("address(0x%X)\n", pvMemory) );
#endif

	if ( pvMemory != NULL )
	{
		OxCAS_Free( pvMemory );
	}
}

/********************************************************************
*	function name :	 OS_DRV_CreateTask
*	input : ...
*	output : result
*	description :
********************************************************************/
os_drv_task_h OS_DRV_CreateTask( os_drv_task_fn tNewTask, void *pvTaskData, ia_byte bPriority, ia_word32 wStackSize, ia_word16 wQueueDepth )
{
	HUINT32 ret_value;
	HUINT8	task_name[32+1]={0};
	HUINT8	msg_name[32+1]={0};
	HUINT8	task_priority;

	HUINT32 resCreateTask, task_id;
	HUINT32 resCreateQueue, queue_id;

	/* Local valuables initialization */
	resCreateTask = resCreateQueue = VK_ERROR ;


#ifdef  TASK_DEBUG
	HxLOG_Info("tNewTask@(0x%X), bPriority(%d), wStackSize(0x%X), wQueueDepth(0x%X)\n", tNewTask, bPriority, wStackSize, wQueueDepth) );
#endif

	if (bPriority <= 15) //( (bPriority >= 0) && (bPriority <= 15) )
	{
		snprintf( (char *)task_name, 32, "%s%02d", "IR3SCELL_TASK", s_usSCellTaskCnt );

		// TODO: SoftCell 3.x.x requestes sequencially 13, 14, 13 priority value, and Queue is not requested.

		//iPriority is 0~15, 15 is the highest.
		if(bPriority == 11) 		task_priority = IR3SCELL_TASK_PRIORITY_11;
		else if(bPriority == 12)	task_priority = IR3SCELL_TASK_PRIORITY_12;
		else if(bPriority == 13)	task_priority = IR3SCELL_TASK_PRIORITY_13;
		else if(bPriority == 14)	task_priority = IR3SCELL_TASK_PRIORITY_14;
		else
		{
			/* Cautions: if unexpected priority value is coming, you shall adjust it with the value in valid range.*/
			HxLOG_Info("Critical error occured when creating softcell task.");
			return OS_DRV_TASK_ERROR;
		}

		// the minimum size of stack is 1024
		if ( wStackSize < IR3SCELL_TASK_STACK_SIZE )
		{
			wStackSize	 = IR3SCELL_TASK_STACK_SIZE;
		}

		resCreateTask = VK_TASK_Create( tNewTask,
										task_priority,
										wStackSize ,
										(char *)task_name,
										pvTaskData,
										(unsigned long *)&task_id,
										0 );

		if ( resCreateTask == VK_OK )
		{
			snprintf( (char *)msg_name, 32, "%s%02d", "qIR", s_usSCellTaskCnt );

			if( wQueueDepth == 0 )
			{
#ifdef TASK_DEBUG
				HxLOG_Info("No Msg Queue is needed - Task ID(0x%X)\n", task_id) );
#endif
				resCreateQueue = VK_OK;
				queue_id = 0;	// 생성 안되었다는 개념인데.... 생성되어도 0을 받을 수 있다면...
			}
			else
			{
				if(wQueueDepth <  IR3SCELL_TASK_MSGQ_SIZE)
				{
					wQueueDepth = IR3SCELL_TASK_MSGQ_SIZE;
				}
				resCreateQueue = VK_MSG_Create( wQueueDepth ,
												sizeof(OS_DRV_MESSAGE),
												(char *)msg_name,
												(unsigned long *)&queue_id,
												VK_SUSPENDTYPE_FIFO);
			}

			if ( resCreateQueue == VK_OK )
			{
				s_stSCellTaskInfo[s_usSCellTaskCnt].task_id   = task_id;
				s_stSCellTaskInfo[s_usSCellTaskCnt].task_ptr  = tNewTask;
				s_stSCellTaskInfo[s_usSCellTaskCnt].queue_id  = queue_id;

				VK_TASK_Start(s_stSCellTaskInfo[s_usSCellTaskCnt].task_id);

				s_usSCellTaskCnt++;
				ret_value = task_id;
			}
			else
			{
				VK_TASK_Destroy(task_id);
				HxLOG_Print("Msg Queue Create Fail - bPriority(%d)\n", bPriority);
				HxLOG_Critical("\n\n");
				ret_value = OS_DRV_TASK_ERROR;
			}
		}
		else
		{
			HxLOG_Print("Task Create Fail - bPriority(%d)\n", bPriority);
			HxLOG_Critical("\n\n");
			ret_value = OS_DRV_TASK_ERROR;
		}

	}
	else
	{
		HxLOG_Print("Invalid priority - bPriority(%d)\n", bPriority);
		HxLOG_Critical("\n\n");
		ret_value = OS_DRV_TASK_ERROR;
	}

#ifdef TASK_DEBUG
	HxLOG_Info("Task ID(0x%X), Queue ID(0x%X)\n", task_id, queue_id) );
#endif

    return ret_value;
}

/********************************************************************
*	function name :	 OS_DRV_DelayTask
*	input : delay time - in milliseconds.
*	output : result
*	description :
********************************************************************/
void OS_DRV_DelayTask( ia_word16 wDelay )
{
    HUINT32   resDelayTask;

#ifdef  TASK_DEBUG
	HxLOG_Info("wDelay(%d)\n", wDelay) );
#endif

	resDelayTask = VK_TASK_Sleep(wDelay * 1L);
	if( resDelayTask == VK_ERROR )
	{
		HxLOG_Print("OS_DRV_DelayTask : Task Sleep Fail\n");
	}
}

/********************************************************************
*	function name :	 OS_DRV_SendMessage
*	input : hTask - Create의 return값(task ID)...
*	output : result
*	description :
********************************************************************/
ia_result OS_DRV_SendMessage( os_drv_task_h hTask, ia_word16 wMessageLength, void *pvMsg )
{
    HUINT16				i, find_flag, ret_value;
    HUINT32				queue_id, resSendQueue;
	OS_DRV_MESSAGE		msg;

#ifdef MESSAGE_DEBUG
	HxLOG_Info("Task ID(0x%X), msgSize(0x%X)\n", hTask, wMessageLength) );
#endif

	// searching the queue ID of the requested task ID
	find_flag = FALSE;
	queue_id = 0;
	for ( i = 0; i < s_usSCellTaskCnt; i++ )
	{
		if ( s_stSCellTaskInfo[i].task_id == hTask )
		{
			if( s_stSCellTaskInfo[i].queue_id != 0 )
			{
				find_flag = TRUE;
				queue_id = s_stSCellTaskInfo[i].queue_id;
			}
			else
			{
				HxLOG_Print("No Valid Queue ID\n");
				find_flag = FALSE;
			}
			break;
		}
	}

	if ( find_flag == TRUE )
	{
		msg.pvMsg = pvMsg;
		msg.usMsgLength = wMessageLength;
		resSendQueue = VK_MSG_Send( queue_id, (void *)(&msg), sizeof(OS_DRV_MESSAGE) );

		if( resSendQueue == VK_OK )
		{
			ret_value = IA_SUCCESS;
		}
		else if( resSendQueue == VK_INVALID_ID )
		{
			HxLOG_Print("Invalid Msg Queue ID(0x%X) of Task ID(0x%X)\n", queue_id, hTask);
			ret_value = IA_INVALID_PARAMETER;
		}
		else
		{
			HxLOG_Print("Msg Send Error Queue ID(0x%X) of Task ID(0x%X)\n", queue_id, hTask);
			ret_value = IA_FULL;
		}
	}
	else
	{
		HxLOG_Print("Invalid Task ID (0x%X)\n", hTask);
		ret_value = IA_INVALID_OS_HANDLE;
	}

	return ret_value;
}

/********************************************************************
*	function name :	 OS_DRV_ReceiveMessage
*	input :
*	output : Pointer to a message or 0 on error.
*	description :
********************************************************************/
void *OS_DRV_ReceiveMessage( ia_word16 *pwMessageLength )
{
	HUINT16				i, find_flag;
	HUINT32				current_task_id, queue_id, resReceiveQueue;
	void 				*retMsg;
	OS_DRV_MESSAGE		msg;

#ifdef MESSAGE_DEBUG
	HxLOG_Info("msg length=%d", *pwMessageLength) );
#endif

	queue_id = 0;
	if( VK_TASK_GetCurrentId((unsigned long *)&current_task_id) == VK_ERROR )
	{
		HxLOG_Print("Get CurrentTaskID(0x%X) error\n", current_task_id);
		*pwMessageLength = 0;
		return NULL;
	}

	find_flag = FALSE;
	for ( i = 0; i < s_usSCellTaskCnt; i++ )
	{
		if ( s_stSCellTaskInfo[i].task_id == current_task_id )
		{
			if( s_stSCellTaskInfo[i].queue_id != 0 )
			{
			    find_flag = TRUE;
			    queue_id = s_stSCellTaskInfo[i].queue_id;
			}
			else
			{
				HxLOG_Print("No Valid Queue ID\n");
				find_flag = FALSE;
			}
			break;
		}
	}

	if ( find_flag == TRUE )
	{
		resReceiveQueue = VK_MSG_Receive( queue_id, &msg, sizeof(OS_DRV_MESSAGE) );

		if ( resReceiveQueue == VK_OK )
		{
			retMsg = (void *)(msg.pvMsg);
			*pwMessageLength = msg.usMsgLength;
		}
		else
		{
			HxLOG_Print("Error on VK_MSG_Receive (0x%X)\n", resReceiveQueue);
			*pwMessageLength = 0;
			return NULL;
		}
	}
	else
	{
		HxLOG_Print("Invalid Queue ID\n");
		*pwMessageLength = 0;
		return NULL;
	}
	return retMsg;
}

/********************************************************************
*	function name :	 OS_DRV_CreateSemaphore
*	input : Initial Count
*	output : Semaphore ID
*	description :
********************************************************************/
os_drv_semaphore_h OS_DRV_CreateSemaphore( ia_uint32 ulInitialValue )
{
	HUINT32			resCreateSemaphore, semaphore_id, ret_value;
	static HUINT32	semNumCount;
	HUINT8 			semName[10+1] = {0,};

#ifdef SEMAPHORE_DEBUG
	HxLOG_Info("ulInitialValud (0x%X)\n", ulInitialValue) );
#endif
	semNumCount++;
	snprintf((char *)semName, 10, "irSem%x", semNumCount);

	resCreateSemaphore = VK_SEM_CreateWithCount((unsigned long *)&semaphore_id, ulInitialValue, semName, VK_SUSPENDTYPE_FIFO);
	if(resCreateSemaphore == VK_OK)
	{
		ret_value = semaphore_id;
	}
	else
	{
		HxLOG_Print("resCreateSemaphore : 0x%X\n",resCreateSemaphore);
		ret_value = OS_DRV_SEMAPHORE_ERROR;
	}

#ifdef SEMAPHORE_DEBUG
	HxLOG_Info("Create Semaphore ID:OK(0x%X)\n", semaphore_id) );
#endif
	return ret_value;
}

/********************************************************************
*	function name :	 OS_DRV_DeleteSemaphore
*	input : Semaphore ID
*	output : result
*	description :
********************************************************************/
ia_result OS_DRV_DeleteSemaphore( os_drv_semaphore_h hSemaphore )
{
	int		resDestroySemaphore;
	HUINT16	result;

#ifdef SEMAPHORE_DEBUG
	HxLOG_Info("Semaphore ID (0x%X)\n", hSemaphore) );
#endif

	resDestroySemaphore = VK_SEM_Destroy(hSemaphore);
	if( resDestroySemaphore == VK_OK )
	{
		result = IA_SUCCESS;
	}
	else
	{
		HxLOG_Print("Destroy Fail - Semaphore ID(0x%X)\n", hSemaphore);
		result = IA_FAIL;
	}

	return result;
}

/********************************************************************
*	function name :	 OS_DRV_DeleteSemaphore
*	input : Semaphore ID
*	output : result
*	description :
********************************************************************/
ia_result OS_DRV_SignalSemaphore( os_drv_semaphore_h hSemaphore )
{
	HUINT32	resReleaseSemaphore;
	HUINT16	result;

#ifdef SEMAPHORE_DEBUG
	HxLOG_Info("Semaphore ID(0x%X) - Signal\n", hSemaphore) );
#endif
	resReleaseSemaphore = VK_SEM_Release( hSemaphore );
	if( resReleaseSemaphore == VK_OK )
	{
#ifdef SEMAPHORE_DEBUG
		HxLOG_Info("Semaphore ID(0x%X) - Signal OK \n", hSemaphore) );
#endif
		result = IA_SUCCESS;
	}
	else
	{
		HxLOG_Print("Release Error -(error =0x%X), ID(0x%X)\n",resReleaseSemaphore, hSemaphore);
		result = IA_FAIL;
	}
	return result;
}

/********************************************************************
*	function name :	 OS_DRV_WaitSemaphore
*	input : Semaphore ID
*	output : result
*	description :
********************************************************************/
ia_result OS_DRV_WaitSemaphore( os_drv_semaphore_h hSemaphore )
{
	HUINT32	resWaitSemaphore;
	HUINT16	result;

#ifdef SEMAPHORE_DEBUG
	HxLOG_Info("Semaphore ID(0x%X)- Get \n", hSemaphore) );
#endif

	resWaitSemaphore = VK_SEM_Get( hSemaphore );
	if( resWaitSemaphore == VK_OK )
	{
#ifdef SEMAPHORE_DEBUG
		HxLOG_Info("Get OK\n", hSemaphore) );
#endif
		result = IA_SUCCESS;
	}
	else
	{
		HxLOG_Print("OS_DRV_WaitSemaphore : Get Error - ID(0x%X)\n", hSemaphore);
		result = IA_FAIL;
	}

	return result;
}





/* end of file */
