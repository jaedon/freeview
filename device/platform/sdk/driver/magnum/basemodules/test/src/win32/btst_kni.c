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
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 5/21/04 8:54p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/test/win32/btst_kni.c $
 * 
 * Hydra_Software_Devel/4   5/21/04 8:54p vsilyaev
 * PR 11175: Merged into the integration branch
 * 
 * Hydra_Software_Devel/BCM7411/1   5/17/04 6:55p vsilyaev
 * Fixed compiller warning
 * 
 * Hydra_Software_Devel/3   4/30/04 3:32p hongtaoz
 * PR8761: fixed C++ compile error.
 * 
 * Hydra_Software_Devel/2   11/25/03 10:26a hongtaoz
 * Removed compile warning.
 * 
 * Hydra_Software_Devel/1   3/31/03 7:38p vsilyaev
 * WIN32 implementation of the BTST module.
 * 
 * 
 ***************************************************************************/
#include "bstd.h"
#include "btst_kni.h"

#include <windows.h>

BDBG_MODULE(test_kni);

struct BTST_TaskObj
{
    HANDLE hTask;
    DWORD ThreadId;
    BTST_TaskFunction task_fn;
    void *taskData;
};

static struct {
        LARGE_INTEGER freq;
} state = {
    0
};


void 
BTST_GetTime(BTST_Time *pTime)
{
    LARGE_INTEGER cntr;

    BDBG_ASSERT(pTime);

    if ( !state.freq.QuadPart ) {
        if (!QueryPerformanceFrequency(&state.freq)) {
            BDBG_ERR(("Win32 Performance timer not avaliable"));
            pTime->sec = pTime->usec = 0;
            return;
        }
    }
    QueryPerformanceCounter(&cntr);

    pTime->sec = (uint32_t)(cntr.QuadPart/state.freq.QuadPart);
    pTime->usec = (uint32_t)((cntr.QuadPart*1000000)/state.freq.QuadPart)%1000000;
    return;
}


static DWORD WINAPI
Win32ThreadProc(LPVOID data)
{
    BTST_TaskHandle task = (BTST_TaskHandle)data;  

    task->task_fn(task, task->taskData);

    ExitThread(0);
    
	/* unreached */
	return 0;
}

BERR_Code 
BTST_CreateTask(BTST_TaskHandle *pTask,  BTST_TaskFunction task_fn, void *data)
{
    BTST_TaskHandle task;

    task = (BTST_TaskHandle) malloc(sizeof(*task));
    *pTask = task;
    if (!task) {
        return BERR_TRACE(BERR_OS_ERROR);
    }
    task->taskData = data;
    task->task_fn = task_fn;

    task->hTask = CreateThread(NULL, 0 /* default stack size */, Win32ThreadProc, task, 0 /* no flags */, &task->ThreadId);
    if (!task->hTask) {
        free(task);
        return BERR_TRACE(BERR_OS_ERROR);
    }

    return BERR_SUCCESS;
}


BERR_Code 
BTST_DestroyTask(BTST_TaskHandle task)
{
    DWORD rc;
    BERR_Code err;
    
    rc = WaitForSingleObject(task->hTask, INFINITE);
    if (rc==WAIT_OBJECT_0) {
        err = BERR_SUCCESS;
        if (!GetExitCodeThread(task->hTask, &rc)) {
            err = BERR_TRACE(BERR_OS_ERROR);
        }
    } else {
        err = BERR_TRACE(BERR_OS_ERROR);
    }
    CloseHandle(task->hTask);
    free(task);
    return err;
}
