/***************************************************************************
 *     Copyright (c) 2003, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btst_kni.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 4/1/03 7:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/test/linuxuser/btst_kni.c $
 * 
 * Hydra_Software_Devel/1   4/1/03 7:28p vsilyaev
 * Linux-user implementation of the test interface.
 * 
 * Hydra_Software_Devel/1   3/31/03 7:38p vsilyaev
 * WIN32 implementation of the BTST module.
 * 
 * 
 ***************************************************************************/
#include "bstd.h"
#include "btst_kni.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

struct BTST_TaskObj
{
    pthread_t task;
    BTST_TaskFunction task_fn;
    void *taskData;
};

void 
BTST_GetTime(BTST_Time *pTime) 
{
	struct timeval tv;
	if (gettimeofday(&tv, NULL)!=0) {
		pTime->sec = pTime->usec = 0;
		return;
	}
	pTime->sec =  (uint32_t) tv.tv_sec;
	pTime->usec = (uint32_t) tv.tv_usec;
	return;
}

static void *
pthreadProc(void *data)
{
    BTST_TaskHandle task = (BTST_TaskHandle)data;  
    
	task->task_fn(task, task->taskData);

    return NULL;
}

BERR_Code 
BTST_CreateTask(BTST_TaskHandle *pTask,  BTST_TaskFunction task_fn, void *data)
{
    BTST_TaskHandle task;
	int rc;

    task = malloc(sizeof(*task));
    *pTask = task;
    if (!task) {
        return BERR_TRACE(BERR_OS_ERROR);
    }
    task->taskData = data;
    task->task_fn = task_fn;

    rc = pthread_create(&task->task, NULL /* default attributes */, pthreadProc, task);
    if (rc!=0) {
        free(task);
        return BERR_TRACE(BERR_OS_ERROR);
    }

    return BERR_SUCCESS;
}

BERR_Code 
BTST_DestroyTask(BTST_TaskHandle task)
{
    int rc;
   
    rc = pthread_cancel(task->task);	
	/* ignore error */
	rc = pthread_join(task->task, NULL);
	if (rc!=0) {
		return BERR_TRACE(BERR_OS_ERROR);
	}
    free(task);
    return BERR_SUCCESS;
}

