/***************************************************************************
 *    (c)2005-2011 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: osapi_linux.cpp $
 * $brcm_Revision: 2 $
 * $brcm_Date: 6/15/11 5:34p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/osapi/osapi_linux.cpp $
 * 
 * 2   6/15/11 5:34p steven
 * SW7425-714:Fix header issue
 * 
 * 1   6/15/11 5:25p steven
 * SW7425-714: Adding OSAPI to AppLibs
 *
 ***************************************************************************/
/**
 * @file osapi_linux.cpp
 *
 * Operating System API (OSAPI) source file. Provides an abstraction from the
 * operating system.
 *
 * This is the implementation for Linux
 *
 * $Id: osapi_linux.cpp,v 1.63.4.2 2007/03/20 18:29:53 rbehe Exp $
 */
#define POSIX_SEMAPHORE
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "dirent.h"
#include "fcntl.h"

#ifdef BDVD_PRCTL
#include <sys/prctl.h>
#endif
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>

#define BDBG_ERR(fmt) printf fmt
#define BDBG_MSG(fmt) printf fmt
#define BDBG_ASSERT(cond) assert(cond)
#define BDBG_MODULE(a)

#ifdef DMALLOC
#  include "dmalloc.h"
#endif
#include "osapi.h"

/* Set to how many seconds to wait before warning of a sem taken too long, 0 means do not warn */
static int16_t warnOnWaitForeverSecs = -1;

BDBG_MODULE(osapi);

/* MACROS AND DEFINES */

#define MAX_NAME_SIZE 16
#define OS_MSG_Q_HEAD 0
#define OS_MSG_Q_TAIL 1

/* TYPEDEFS */

#ifdef __CALC_PEAK_ALLOCATION
typedef struct tag_mem_info
{
    void * pvAddr;
    uint32_t size;
} mem_info;
#endif

/*
 *  Thread record
 */
typedef struct tagThreadRecord              /* prefix: thread */
{
    void *          pvFunction;
    void *          pvParam;
    void *          pvStackBase;
    pthread_t       pthread;
    pthread_attr_t  attr;
    const char     *strName;
} OS_THREAD;


/*
 *  Process record
 */
typedef struct
{
    int     pid;
    int     pfd[2];
    fd_set readfds;
} OS_PROCESS;


/*
 *  Semaphore record
 */
typedef struct tagSemaphoreRecord           /* prefix: sem */
{
    uint32_t        ulSemType;
    char            strName[MAX_NAME_SIZE];
    uint32_t        ulCount;
    uint32_t        ulMaxCount;
    pthread_cond_t  condWait;       /* Signal waiting threads */
    pthread_mutex_t mutex;          /* Critical sections */
    const char     *takenFname;
    int             takenLine;
    char            takenThread[OS_THREAD_NAME_SIZE];
} OS_SEMAPHORE;

/*
 *  Message Box record
 */
typedef struct tagMessageBoxRecord          /* prefix: mbox */
{
    uint32_t           ulDepth;                /* message queue depth */
    uint32_t           ulSize;                 /* message size */
    uint32_t           ulHead;
    uint32_t           ulTail;
    uint8_t              *pbBuffers;              /* buffer for messages */
    pthread_cond_t     condWaitFull;           /* Signal waiting threads */
    pthread_cond_t     condWaitEmpty;          /* Signal waiting threads */
    pthread_mutex_t    mutex;                  /* Critical sections */
} OS_MBOX;


typedef struct tagTimerRecord
{
    void *           pvFunction;
    void *           pvParam;
    timer_t         timerID;
} OS_TIMER;


/*
 * Memory Pool ID record
 */
typedef struct tagMemPoolRecord
{
    uint32_t          ulPgSize;           /* Size of each page of memory */
    uint32_t          ulNumPages;         /* Number of memory pages */
    struct Page                           /* A page from a memory pool */
    {
        void *        pvPgAddr;           /* Pointer to memory address */
        int32_t       lAttached;          /* Number of pages attached with page *
                                           * (including itself)                 */
    } *memPage;
} OS_MEMPOOL;


/* GLOABALS */

pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef __CALC_PEAK_ALLOCATION
mem_info test[512];
bool fFirstAlloc          = true;
uint32_t   ulCurrentAllocations = 0;
uint32_t   ulPeakAllocations    = 0;
#endif


/* private function prototypes */
char const* getPthreadReturnCodeName(int pthread_result);
static void * linuxThread(void *);
static void linuxTimer(union sigval sigval);

#ifdef VC_TIMERS
int vc_timer_delete(timer_t timerid);
int vc_timer_create(clockid_t clock_id, struct sigevent *evp, timer_t *timerid);
int vc_timer_settime(timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue);
int vc_timer_gettime(timer_t timerid, struct itimerspec *value);
static uint32_t TimerThreadProc( void * pvParam );

#define timer_delete  vc_timer_delete
#define timer_create  vc_timer_create
#define timer_settime vc_timer_settime
#define timer_gettime vc_timer_gettime
#endif

#ifdef BDVD_RT
int pthread_setschedprio(pthread_t thread, int prio);
#endif

static OS_STATUS OS_MsgQReceiveOption(OS_MSG_Q_ID mqId, char *strMsg, uint32_t ulSize, int iTimeout, int iOption);

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Task function definitions                                                **
**                                                                           **
*******************************************************************************
******************************************************************************/

/**
 * OS Task Spawn function.
 *
 * @param
 *    char *strName - task name
 *
 * @param
 *    BdvdThreadPriority iPrio - task priority
 *
 * @param
 *    int iStackSize - task stack size
 *
 * @param
 *    OS_FUNCPTR func - task procedure function
 *
 * @retval
 *    int pthreadRecord - pointer to the task record, if successful
 *    OS_FAILURE if not successful.
 */
OS_STATUS OS_ThreadSpawn(const char *strName,
                       int iPrio,
                       int iStackSize,
                       OS_FUNCPTR func
                       )
{
    return (OS_ThreadSpawnParam( strName, iPrio, iStackSize, (OS_FUNCPTR_PARAM)func, NULL, NULL ));
}


/**
 * OS Task Spawn Parameter function.
 *
 * @param
 *    char *strName - task name
 *
 * @param
 *    BdvdThreadPriority iPrio - task priority
 *
 * @param
 *    int iStackSize - task stack size
 *
 * @param
 *    OS_FUNCPTR func - task procedure function
 *
 * @param
 *    void * pvParam - task input parameter
 *
 * @param
 *    void * pvStackBaseParam - stack pointer, allows stack base to be passed
 *                             in rather than OS_Malloc'd.
 *
 * @retval
 *    int pthreadRecord - pointer to the task record, if successful
 *    OS_FAILURE if not successful.
 */
OS_STATUS OS_ThreadSpawnParam(const char *strName,
                            int _iPrio,
                            int iStackSize,
                            OS_FUNCPTR_PARAM func,
                            void * pvParam,
                            void * pvStackBaseParam
                            )
{
    OS_THREAD *pthreadRecord;
    int     iRetStatus      = 0;
    int     minStackSize = 0;
    int     stackSize = iStackSize;
    static  int once = 0;
    static  int realtime_enabled = 1;
#ifdef BDVD_RT
    int     priority;
    struct sched_param sc_cfg;
#endif
    int iPrio = (int)_iPrio;

    if (once == 0) {
        char *value;

        once = 1;

        if ((value = getenv("BDVD_RT_ENABLE")) != 0) {
            if (strcmp("ON", value) == 0)
                realtime_enabled  = 1;
            else if (strcmp("OFF", value) == 0)
                realtime_enabled  = 0;
        }
        if ((value = getenv("bdvd_rt_enable")) != 0) {
            if (strcmp("on", value) == 0)
                realtime_enabled  = 1;
            else if (strcmp("off", value) == 0)
                realtime_enabled  = 0;
        }

        if (realtime_enabled == 1)
            printf("enabling realtime scheduling\n");
        else
            printf("disabling realtime scheduling\n");
    }

    /* Allocate a record for the thread */
    pthreadRecord = (OS_THREAD *)OS_MemAlloc(sizeof(OS_THREAD));
    if (pthreadRecord == NULL)
    {
        BDBG_ERR(("OS_ThreadSpawn: Could not allocate thread record!\n"));
        goto error_handler;
    }

    /* Check the validity of the given priority */
    if ( (iPrio < OS_THREAD_MIN_PRIORITY) || (iPrio > OS_THREAD_MAX_PRIORITY) )
    {
        BDBG_ERR(("%s: Invalid thread priority (%d)!\n", __FUNCTION__, iPrio));
        goto error_handler;
    }

    /* Save the paramater */
    pthreadRecord->pvParam = pvParam;

    /* Record the pointer to the thread stack base in the thread record */
    pthreadRecord->pvStackBase = pvStackBaseParam;

    /* Record the function pointer */
    pthreadRecord->pvFunction = (void *)func;

    pthreadRecord->strName = strName;

    /* Create the thread attributes */
    iRetStatus = pthread_attr_init(&pthreadRecord->attr);
    if (iRetStatus != 0)
    {
        BDBG_ERR(("%s: ATTRIBUTE CREATION FAILED!\n", __FUNCTION__));
        goto error_handler;
    }

    /* Specify the threads stack size */
    if (minStackSize && (stackSize < minStackSize))
    {
        BDBG_ERR(("%s: Thread '%s' stack size (%d) too small, using OSAPI min (%d) instead!", __FUNCTION__, strName, stackSize, minStackSize));
        stackSize = minStackSize;
    }
    iRetStatus = pthread_attr_setstacksize(&pthreadRecord->attr, stackSize);
    if (iRetStatus != 0)
    {
        /* this is not a critical error so just let it be known and continue
         * setting up the thread */
        size_t iDefaultStackSize = 0;
        pthread_attr_getstacksize(&pthreadRecord->attr, &iDefaultStackSize);
        BDBG_ERR(("%s: Could not set thread '%s' stack size to %d (min is %d); using %d instead!", __FUNCTION__, strName, stackSize, PTHREAD_STACK_MIN, iDefaultStackSize));
    }

    if (pthread_attr_setinheritsched(&pthreadRecord->attr, PTHREAD_EXPLICIT_SCHED) ) {
        BDBG_ERR(("%s: pthread_attr_setinheritsched failed", __FUNCTION__));
    }

    if (realtime_enabled == 0)
        iPrio = OS_THREAD_MIN_PRIORITY;

#ifdef BDVD_RT
    /* default timeshare priority */
    priority = sched_get_priority_min(SCHED_OTHER);

    if (iPrio > OS_THREAD_MIN_PRIORITY)  {

        /* make sure we are set up for the right scheduling policy */
        iRetStatus = pthread_attr_setschedpolicy(&pthreadRecord->attr, SCHED_RR);
        if (iRetStatus != 0)
        {
            /* this is not a critical error so just let it be known and continue
             * setting up the thread */
            BDBG_ERR(("%s: Could not set our scheduling policy!\n", __FUNCTION__));
        }
        else
        {
            int min_priority;
            int max_priority;

            /* get that range of priorities available for this OS */
            /*
             * pthread manager gets the highest priority (max - 9).
             * isr_task gets the second highest (max - 10)
             * input parameter priority has a range from 1 to 256
             */

            max_priority = sched_get_priority_max(SCHED_RR) - 11;
            min_priority = sched_get_priority_min(SCHED_RR);

            /*
              BDBG_MSG(("%s: min_priority min = %d, max_priority = %d\n",
                                        __FUNCTION__, min_priority, max_priority));
            */

            /* map the specified priority to a like linux priority */

            priority = ((iPrio + 3) >> 2) + min_priority - 1;
            if (priority > max_priority)
                priority = max_priority;

        }
      } else {
            if (pthread_attr_setschedpolicy(&pthreadRecord->attr, SCHED_OTHER)) {
                BDBG_ERR(("%s: Could not set  scheduling policy SCHED_OTHER!\n", __FUNCTION__));
            }
      }

      pthread_attr_getschedparam(&pthreadRecord->attr, &sc_cfg);

      sc_cfg.sched_priority = priority;

      iRetStatus = pthread_attr_setschedparam(&pthreadRecord->attr, &sc_cfg);
      if (iRetStatus != 0) {
          BDBG_ERR(("%s: pthread_attr_setschedparam failed", __FUNCTION__));
      }

#endif

    /* Create the thread */
    iRetStatus = pthread_create(&pthreadRecord->pthread, &pthreadRecord->attr, linuxThread, (void *)pthreadRecord);
    if (iRetStatus != 0)
    {
        BDBG_ERR(("%s: Handle is NULL!\n", __FUNCTION__));
        goto error_handler;
    }

    /* Return the thread record as an integer */
    return ( (int)pthreadRecord );

error_handler:

    if (NULL != pthreadRecord)
    {
        OS_MemFree(pthreadRecord);
    }

    return (OS_FAILURE);
}


/**
 * OS Task Delete function.
 *
 * @param
 *    int taskId - task identifier
 *
 * @retval
 *    This function will print a status message if it was not successful.
 */
void OS_ThreadDelete(int taskId)
{
    OS_THREAD *pthreadRecord;
    int iRetStatus;

    /* Cast the given task identifier to a pointer to a task record */
    pthreadRecord = (OS_THREAD *)taskId;

    if (pthreadRecord != 0)
    {
        /* Delete the given thread */
        pthread_cancel(pthreadRecord->pthread);

        /* wait for the task to exit */
        OS_ThreadJoin(taskId);

        /* delete the pthread attributes */
        iRetStatus = pthread_attr_destroy(&pthreadRecord->attr);
        if (iRetStatus != 0)
        {
            BDBG_ERR(("%s: pthread_attr_destroy failed!\n", __FUNCTION__));
        }

        /* Delete the thread record */
        OS_MemFree(pthreadRecord);
    }
}


/**
 * OS Task Exit function.
 *
 * @param
 *    None.
 *
 * @retval
 *    None.
 */
void OS_ThreadExit(void)
{
    pthread_exit(0);
}


/**
 * Restarts the task from the beginning of the function passed into the spawn command.
 *
 * @param
 *    int taskId - task identifier
 *
 * @retval
 *    Returns OS_OK.
 *    This function is always successful.
 */
OS_STATUS OS_ThreadReset(int taskId)
{
    return (OS_NOT_IMPLEMENTED);
}


/**
 * OS Task Suspend function.
 *
 * @param
 *    int taskId - task identifier
 *
 * @retval
 *    Returns OS_OK.
 *    This function is always successful.
 */
OS_STATUS OS_ThreadSuspend(int taskId)
{
    return (OS_NOT_IMPLEMENTED);
}


/**
 * Resume a task after a suspend. This replaces OS_ThreadRestart!
 *
 * @param
 *    int taskId - task identifier
 *
 * @retval
 *    Returns OS_OK.
 *    This function is always successful.
 */
OS_STATUS OS_ThreadResume(int taskId)
{
    return (OS_NOT_IMPLEMENTED);
}


/**
 * OS Task Join function.
 *
 * @param
 *    int taskId - task identifier
 *
 * @retval
 *    Returns OS_OK.
 *    This function is always successful.
 */
OS_STATUS OS_ThreadJoin(int taskId)
{
    OS_STATUS status = OS_OK;
    OS_THREAD   *pthreadRecord;

    /* Cast the given task identifier to a pointer to a task record */
    pthreadRecord = (OS_THREAD *)taskId;

    if (pthreadRecord != 0)
    {
        /* Join the given thread */
        if (pthread_join(pthreadRecord->pthread, NULL) != 0)
        {
            status = OS_FAILURE;
        }
    }
    else
    {
        status = OS_FAILURE;
    }

    return (status);
}

/**
 * OS Task Yield function.
 *
 * @param
 *    None.
 *
 * @retval
 *    This function is always successful.
 */
void OS_ThreadYield(void)
{
    if (sched_yield() == -1)
    {
        BDBG_ERR(("failed to yield\n"));
    }
}

/**
 * OS Task Delay function.
 *
 * @param
 *    int iTicks - time to delay the current task, in ticks
 *
 * @retval
 *    This function is always successful.
 */
void OS_ThreadDelay(int iTicks)
{
#if (CLOCKS_PER_SEC == 1000000)
    usleep(iTicks);
#else
    uint64_t microseconds;
    microseconds = (uint64_t)iTicks;
    microseconds *= 1000000;
    microseconds /= CLOCKS_PER_SEC;
    usleep( (unsigned long)microseconds );
#endif
}

/**
 * OS Task Delay function.
 *
 * @param
 *    milliseconds - time to delay the current task, in milliseconds
 *
 * @retval
 *    This function is always successful.
 */
void OS_ThreadDelayMsec(int milliseconds)
{
    usleep(milliseconds*1000);
}

/**
 * OS Task Identify Self function.
 *
 * @param
 *    None.
 *
 * @retval
 *    int tHandle - handle of current task, if successful
 *    OS_FAILURE - if not successful
 */
uint32_t OS_ThreadIdSelf(void)
{
    return((uint32_t)pthread_self());
}

/**
 * OS Task Lock function.
 *
 * @param
 *    None.
 *
 * @retval
 *    Returns false on success, true on failure.
 */
bool OS_ThreadLock(void)
{
    if (pthread_mutex_lock(&global_mutex) != 0)
    {
        return (true);
    }

    return (false);
}

/**
 * OS Task Unlock function.
 *
 * @param
 *    None.
 *
 * @retval
 *    Returns false on success, true on failure.
 */
bool OS_ThreadUnlock(void)
{
    if (pthread_mutex_unlock(&global_mutex) != 0)
    {
        return (true);
    }

    return (false);
}

/**
 * OS Task Name function.
 *
 * @param
 *    OS_THREAD taskId - task identifier
 *
 * @retval
 *    char *strTaskName - task name, if successful
 *    NULL - if not successful
 */
char *OS_ThreadName(int taskId)
{
    printf("OS_ThreadName: NOT IMPLEMENTED!\n");
    return (char *)NULL;
}

/**
 * OS Task Verify function.
 *
 * @param
 *    OS_THREAD taskId - task identifier
 *
 * @retval
 *    OS_OK if successful.
 *    OS_FAILURE if not successful.
 */
OS_STATUS OS_ThreadVerify(OS_THREAD taskId)
{
    printf("OS_ThreadVerify: NOT IMPLEMENTED!\n");
    return OS_FAILURE;
}

/**
 * OS Task Name to Identifier function.
 *
 * @param
 *    char *strName - task name
 *
 * @retval
 *    int taskId - identifier of current task, if successful
 *    0 - if not successful
 */
int OS_ThreadNameToId(char *strName)
{
    printf("OS_ThreadNameToId: NOT IMPLEMENTED!\n");
    return 0;
}

/**
 * OS Task Name function.
 *
 * @param
 *    OS_THREAD taskId - task identifier
 *    
 * @param
 *    char strName[OS_THREAD_NAME_SIZE] - string buffer for task name
 *
 * @retval
 *    none
 */
void OS_ThreadNameSelf(char strName[OS_THREAD_NAME_SIZE])
{
#ifdef BDVD_PRCTL
    prctl(PR_GET_NAME, strName, 0, 0.,0);
#endif
}

/**
 * OS Task Show PC function.
 *
 * @param
 *    int taskId - task identifier
 *
 * @retval
 *    None.
 */
void OS_ThreadShowPc(int taskId)
{
    printf("OS_ThreadShowPc: NOT IMPLEMENTED!\n");
}


/******************************************************************************
*******************************************************************************
**                                                                           **
**  Process function definitions                                             **
**                                                                           **
*******************************************************************************
******************************************************************************/
static OS_STATUS OS_Process_P_BuildArgvList(
    char                *argv[],
    int                 argv_sz,
    char                *pString,
    int                 *pargc)
{
    int     i = 0;

    if ( (pString == NULL) || (argv == NULL) || (pargc == NULL) )
    {
        BDBG_ERR(("%s(): Invalid parameter!", __FUNCTION__));
        return OS_FAILURE;
    }

    while( (i < argv_sz) && (*pString != '\0') )
    {
        argv[i] = pString;
        while ( (*pString != '\0') && !isspace(*pString) )
        {
            pString++;
        }
        if (*pString == '\0')
        {
            i+=1;
            break;
        }
        while (isspace(*pString))
        {
            *pString = '\0';
            pString++;
        }
        i++;
    }

    *pargc = i;
    return OS_OK;
}


/**
 * OS Task Spawn function.
 *
 * @param
 *    int iPrio - task priority
 *
 * @param
 *    char *pCommand - Process command to run
 *
 * @param
 *    char *pArgs - command arguments
 *
 * @retval
 *    int - Handle to the Process information
 *    OS_FAILURE if not successful.
 */
int OS_ProcessSpawn(int iPrio, char *pcCommand, char *pcArgs)
{
    OS_PROCESS *hProcess = NULL;
    int             maxFDs;
    char            *argv[OS_PROCESS_MAX_ARGUMENTS];
    int             argc = 0;
    int             i = 0;
    char            *pBuf = NULL;
    OS_STATUS       tRetCode = OS_OK;

    /* Check the validity of the given priority */
    if ( (iPrio < OS_THREAD_MIN_PRIORITY) || (iPrio > OS_THREAD_MAX_PRIORITY) )
    {
        BDBG_ERR(("%s: Invalid thread priority (%d)!\n", __FUNCTION__, iPrio));
        return OS_FAILURE;
    }

    /* Allocate a record for the thread */
    hProcess = (OS_PROCESS *)OS_MemAlloc(sizeof(OS_PROCESS));
    if (hProcess == NULL)
    {
        BDBG_ERR(("%s(): Could not allocate thread record!\n", __FUNCTION__));
        return OS_NO_MEMORY;
    }

    memset(hProcess, 0, sizeof(OS_PROCESS));

    /* Make sure all open fds are set to be closed when exec() is called */
    maxFDs = sysconf(_SC_OPEN_MAX);
    if (maxFDs == -1)
    {
        maxFDs = 8192;
    }

    for (i = 3; i < maxFDs; ++i)
    {
        int res = fcntl(i, F_GETFD);
        if (res >= 0)
        {
            fcntl(i, F_SETFD, res | FD_CLOEXEC);
        }
    }

    /* Create a pipe to capture output */
    if (pipe(hProcess->pfd) == -1)
    {
        BDBG_ERR(("%s Pipe failure!", __FUNCTION__));
        tRetCode = OS_FAILURE;
        goto err_out;
    }

    memset(argv, 0, sizeof(argv));

    /* Prepare the buffer for parsing the arguments */
    if (pcArgs != NULL)
    {
        pBuf = (char*)OS_MemAlloc(strlen(pcArgs)+1);
        if (pBuf == NULL)
        {
            BDBG_ERR(("%s(): Ran out of memory parameter!", __FUNCTION__));
            goto err_out;
        }
        memcpy(pBuf, pcArgs, strlen(pcArgs)+1);

        if (OS_Process_P_BuildArgvList(argv, sizeof(argv), pBuf, &argc) != OS_OK)
        {
            BDBG_ERR(("%s(): Failed to Build argument list! CMD:[%s]", __FUNCTION__, pcArgs));
            tRetCode = OS_FAILURE;
            goto err_out;
        }
        argv[argc] = NULL;
    }

    hProcess->pid = vfork();


    /* Child Process */
    if (hProcess->pid == 0)
    {
        struct sched_param  param;

        BDBG_MSG(("%s, Child Process about to Call CMD:[%s]", __FUNCTION__, pcCommand));

        close(hProcess->pfd[0]);
        if (dup2(hProcess->pfd[1], STDOUT_FILENO) < 0)
        {
            BDBG_ERR(("%s Failed to duplicating STDOUT error: %s",
                __FUNCTION__, strerror(errno)));
            _exit(EXIT_FAILURE);
        }

        /* Change the priority of the process to very low */
        param.sched_priority = sched_get_priority_min(SCHED_OTHER);
        if (sched_setscheduler(0, SCHED_OTHER, &param) < 0)
        {
            BDBG_ERR(("%s Failed to set scheduling priority error: %s",
                __FUNCTION__, strerror(errno)));
            _exit(EXIT_FAILURE);
        }

        if (execv(pcCommand, argv) < 0)
        {
            BDBG_ERR(("%s Failed to execute CMD:[%s] error: %s",
                __FUNCTION__, pcCommand, strerror(errno)));
            _exit(EXIT_FAILURE);
        }
    }


    /* Parent Process */
    else
    {
        FD_ZERO(&hProcess->readfds);
        FD_SET(hProcess->pfd[0], &hProcess->readfds);
        close(hProcess->pfd[1]);
    }

err_out:
    if (pBuf != NULL)
    {
        OS_MemFree(pBuf);
    }

    if ( (tRetCode != OS_OK) && (hProcess != NULL) )
    {
        if (hProcess->pfd[0] != 0)
        {
            close(hProcess->pfd[0]);
        }
        if (hProcess->pfd[1] != 0)
        {
            close(hProcess->pfd[1]);
        }
        OS_MemFree(hProcess);
        return tRetCode;
    }

    return (int)hProcess;
}


/**
 * OS Process Delete
 *
 * Send a kill signal (if the process is running) and then free up the memory handles.
 *
 * @param
 *    int taskId - Reference to the process handle
 *
 * @retval
 *    OS_OK - Sucessful
 *    OS_FAILURE - if not successful.
 */
OS_STATUS OS_ProcessDelete(int taskId)
{
    OS_PROCESS  *hProcess = (OS_PROCESS*)taskId;
    int         status = 0;

    if (hProcess == NULL)
    {
        BDBG_ERR(("%s: Invalid Parameter!\n", __FUNCTION__));
        return OS_FAILURE;
    }

    if (hProcess->pid == 0)
    {
        waitpid(hProcess->pid, &status, WNOHANG | WUNTRACED);
         if (!WIFEXITED(status))
         {
             kill(hProcess->pid, SIGKILL);
             /* Ensure proper termination of the child process */
             waitpid(hProcess->pid, &status, 0);
         }
    }

    if (hProcess->pfd[0] != 0)
    {
        close(hProcess->pfd[0]);
    }

    if (hProcess->pfd[1] != 0)
    {
        close(hProcess->pfd[1]);
    }

    OS_MemFree(hProcess);
    hProcess = NULL;
    return OS_OK;
}


/**
 * OS Process Wait
 *
 * Wait for the process to terminate for a given amount of time (miliseconds)
 * and then parse the results from the command out of the unidirectional
 * pipe.
 *
 *
 * @param
 *    int taskId - Reference to the process handle
 *
 * @param
 *    int miliseconds - Reference to the process handle
 *
 * @param
 *    bool *bStopCondition - Stop condition to trigger this function to unblock
 *
 * @param
 *    char *pcSearchStr - string to search for (or NULL)
 *
 * @param
 *    char *pResult - Buffer to store the results (after the search string) or NULL
 *
 * @param
 *    char ulLength - How much to copy into the buffer
 *
 * @retval
 *    OS_OK - Sucessful
 *    OS_FAILURE - if not successful.
 */
OS_STATUS   OS_ProcessWait(
    int             taskId,
    int             miliseconds)
{
    return OS_ProcessWaitFor(taskId, miliseconds, NULL, NULL, NULL, 0);
}


/**
 * OS Process Wait
 *
 * Wait for the process to terminate for a given amount of time (miliseconds)
 * and then parse the results from the command out of the unidirectional
 * pipe.
 *
 *
 * @param
 *    int taskId - Reference to the process handle
 *
 * @param
 *    int miliseconds - Reference to the process handle
 *
 * @param
 *    bool *bStopCondition - Stop condition to trigger this function to unblock
 *
 * @param
 *    char *pcSearchStr - string to search for (or NULL)
 *
 * @param
 *    char *pResult - Buffer to store the results (after the search string) or NULL
 *
 * @param
 *    char ulLength - How much to copy into the buffer
 *
 * @retval
 *    OS_OK - Sucessful
 *    OS_FAILURE - if not successful.
 */
OS_STATUS   OS_ProcessWaitFor(
    int             taskId,
    int             miliseconds,
    bool            *pStopCondition,
    char            *pcSearchStr,
    char            *pResults,
    uint32_t        ulLength)
{
    OS_PROCESS  *hProcess = (OS_PROCESS*)taskId;
    int         status = 0;
    int         tRetCode = OS_FAILURE;
    int         result = 0;

    if ( (hProcess == NULL) || (hProcess->pid == 0) )
    {
        BDBG_ERR(("%s: Invalid Parameter!\n", __FUNCTION__));
        return OS_FAILURE;
    }

    /* Now lets makes sure the process is gone, if not then kill it */
    if (miliseconds == OS_WAIT_FOREVER)
    {
        while ((pStopCondition == NULL) ? 1 : !(*pStopCondition))
        {
            result = waitpid(hProcess->pid, &status, WNOHANG | WUNTRACED);
            if (result != 0)
            {
                break;
            }
            OS_ThreadDelayMsec(10);
        }
        if ( (pStopCondition != NULL) && (*pStopCondition) )
        {
            BDBG_MSG(("%s, Received stop condition, canceling...", __FUNCTION__));
            tRetCode = OS_CANCELLED;
        }
    }
    else
    {
        int i = 0;
        for (i = 0; i < miliseconds; i+=10)
        {
            result = waitpid(hProcess->pid, &status, WNOHANG | WUNTRACED);
            if (result != 0)
            {
                break;
            }
            OS_ThreadDelayMsec(10);
        }
        if (result == 0)
        {
            BDBG_MSG(("%s, Parent:  Timeout", __FUNCTION__));
            tRetCode = OS_TIMEOUT;
        }
    }

    if (result < 0 )
    {
        BDBG_ERR(("%s waitpid failed! error: %s", __FUNCTION__, strerror(errno)));
        tRetCode = OS_FAILURE;
    }


    if ( (result > 0) && WIFEXITED(status) && (WEXITSTATUS(status) == 0) )
    {
        char cLine[256];
        BDBG_MSG(("%s, Parent: Command Finished gracefully", __FUNCTION__));

        /* Parse Results */
        if (pcSearchStr != NULL)
        {
            FILE *fd = fdopen(hProcess->pfd[0], "r");
            if (fd != NULL)
            {
                while (fgets(cLine, sizeof(cLine), fd) != NULL)
                {
                    char *pcResult = NULL;

                    BDBG_MSG(("%s", cLine));
                    if ((pcResult = strstr(cLine, pcSearchStr)) != NULL)
                    {
                        pcResult += strlen(pcSearchStr);
                        BDBG_MSG(("%s, Parent: Success!", __FUNCTION__));
                        if ( (pResults != NULL) && ulLength)
                        {
                            if (pcResult != NULL)
                            {
                                strncpy(pResults, pcResult, ulLength);
                            }
                            else
                            {
                                memset(pResults, 0, ulLength);
                            }
                        }
                        tRetCode = OS_OK;
                        break;
                    }
                }
                fclose(fd);
            }
        }
        else
        {
            BDBG_MSG(("%s, Parent: CMD Success!", __FUNCTION__));
            tRetCode = OS_OK;
        }
    }
    return tRetCode;
}


/**
 * OS Process Signal
 *
 * send a signal to a process
 *
 *
 * @param
 *    int taskId - Reference to the process handle
 *
 * @param
 *    int sig_num - the signal to send
 *
 * @retval
 *    OS_OK - Sucessful
 *    OS_FAILURE - if not successful.
 */
OS_STATUS   OS_ProcessSignal(int taskId, int signal)
{
    OS_PROCESS  *hProcess = (OS_PROCESS*)taskId;

    if ( (hProcess == NULL) || (hProcess->pid == 0) )
    {
        BDBG_ERR(("%s: Invalid Parameter!\n", __FUNCTION__));
        return OS_FAILURE;
    }

    return (kill(hProcess->pid, signal) == 0 ) ? OS_OK : OS_FAILURE;
}


/******************************************************************************
*******************************************************************************
**                                                                           **
**  Message queue function definitions                                       **
**                                                                           **
*******************************************************************************
******************************************************************************/

/**
 * OS_MsgQCreate creates a message queue.
 *
 * @param int iDepth     - queue depth
 * @param int iMsgSize   - message size, all messages placed into this queue must be of this size.
 * @param int iOSOptions - operating system options (not used in this implementation)
 *
 * @retval
 *    OS_MSG_Q_ID mqId - pointer to the message box record, if successful
 *    0 - if not successful
 */
OS_MSG_Q_ID OS_MsgQCreate(int iDepth, int iMsgSize, int iOSOptions)
{
    OS_MBOX *pmsg = NULL;

    /* Check the validity of the given message size */
    if (iMsgSize <= 0)
    {
        BDBG_ERR(("%s: Invalid message size (%d)!\n", __FUNCTION__, iMsgSize));
        goto error_handler;
    }

    /* Check the validity of the given depth */
    if (iDepth <= 0)
    {
        BDBG_ERR(("%s: Invalid queue depth (%d)!\n", __FUNCTION__, iDepth));
        goto error_handler;
    }

    /* Allocate Record */
    pmsg = (OS_MBOX *)OS_MemAlloc(sizeof(OS_MBOX));
    if (NULL == pmsg)
    {
        goto error_handler;
    }

    /* Allocate Buffers */
    pmsg->pbBuffers = (uint8_t *)OS_MemAlloc(iMsgSize * (iDepth + 1));
    if (NULL == pmsg->pbBuffers)
    {
        goto error_handler;
    }

    /* Setup Head and Tail */
    /* If Head == Tail Buffers are empty */
    pmsg->ulHead  = 0;
    pmsg->ulTail  = 0;
    pmsg->ulDepth = (uint32_t)iDepth;
    pmsg->ulSize  = (uint32_t)iMsgSize;

    /* Create the binary semaphore */
    if (pthread_cond_init(&pmsg->condWaitEmpty, NULL) != 0)
    {
        BDBG_ERR(("%s: Could not init condWait", __FUNCTION__));
        goto error_handler;
    }
    if (pthread_cond_init(&pmsg->condWaitFull, NULL) != 0)
    {
        BDBG_ERR(("%s: Could not init condWait", __FUNCTION__));
        goto error_handler;
    }
    if (pthread_mutex_init(&pmsg->mutex, NULL) != 0)
    {
        BDBG_ERR(("%s: Could not init mutex", __FUNCTION__));
        goto error_handler;
    }

    return ((OS_MSG_Q_ID)pmsg);

error_handler:
    if (NULL != pmsg)
    {
        if (NULL != pmsg->pbBuffers)
        {
            OS_MemFree(pmsg->pbBuffers);
        }

        OS_MemFree(pmsg);
    }

    return (0);
}

/**
 * OS Message Queue Delete function.
 *
 * @param
 *    OS_MSG_Q_ID mqId - message queue identifier
 *
 * @retval
 *    Returns OS_OK.
 *    This function is always successful.
 */
OS_STATUS OS_MsgQDelete(OS_MSG_Q_ID mqId)
{
    OS_MBOX   *pmsg  = (OS_MBOX *)mqId;

    if (mqId == 0)
    {
        BDBG_ERR(("%s: BAD POINTER\n", __FUNCTION__));
        return (OS_FAILURE);
    }

    /* take the mutex to protect during cond destroy and to make sure noone else is holding the mutex */
    pthread_mutex_lock(&pmsg->mutex);

    /* Destroy the wait conditions */
    int pthread_result = 0;
    pthread_result = pthread_cond_destroy(&pmsg->condWaitEmpty);
    if ( pthread_result != 0)
    {
        BDBG_ERR(("%s: Could not destroy condWait, got pthread return code: %s\n", __FUNCTION__, getPthreadReturnCodeName(pthread_result)));
    }
    pthread_result = pthread_cond_destroy(&pmsg->condWaitFull);
    if (pthread_result != 0)
    {
        BDBG_ERR(("%s: Could not destroy condWait, got pthread return code: %s\n", __FUNCTION__, getPthreadReturnCodeName(pthread_result)));
    }

    /* you cannot destroy a mutex that is locked */
    pthread_mutex_unlock(&pmsg->mutex);

    pthread_result = pthread_mutex_destroy(&pmsg->mutex);
    if (pthread_result != 0)
    {
        BDBG_ERR(("%s: Could not destroy mutex, got pthread return code: %s\n", __FUNCTION__, getPthreadReturnCodeName(pthread_result)));
    }


    /* DeaAllocate Buffers */
    OS_MemFree(pmsg->pbBuffers);

    /* Deallocate Record */
    OS_MemFree(pmsg);

    /* Return success */
    return (OS_OK);
}

/**
 * OS Message Queue Send function.
 *
 * @param OS_MSG_Q_ID mqId - message queue identifier
 * @param char *strMsg     - message pointer
 * @param OS_UINT uiSize   - message size
 * @param int iTimeout     - send timeout value, in ticks. (-1 = wait forever, 0 = no wait, > 1 = wait)
 * @param int iPrio        - message priority (not used)
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 *    OS_TIMEOUT if a timeout occurred
 */
OS_STATUS OS_MsgQSend(OS_MSG_Q_ID mqId, char *strMsg, uint32_t ulSize, int iTimeout, int iPrio)
{
    OS_STATUS iRet  = OS_OK;
    OS_MBOX   *pmsg = (OS_MBOX *)mqId;
    struct timeval  tv;
    struct timespec abstime;

    /* Check the validity of message queue */
    if (mqId == 0)
    {
        BDBG_ERR(("%s: BAD POINTER!\n", __FUNCTION__));
        goto error_handler;
    }

    /* Check the validity of message */
    if (strMsg == 0)
    {
        BDBG_ERR(("%s: BAD POINTER!\n", __FUNCTION__));
        goto error_handler;
    }

    if (ulSize != pmsg->ulSize)
    {
        BDBG_ERR(("%s:%u - Error here\n", __FILE__, __LINE__));
        goto error_handler;
    }

    /* Get Mutex */
    if (pthread_mutex_lock(&pmsg->mutex) != 0)
    {
        BDBG_ERR(("%s: Failed to obtain lock\n", __FUNCTION__));
        goto error_handler;
    }

    if ( (OS_WAIT_FOREVER != iTimeout) && (OS_NO_WAIT != iTimeout) )
    {
        /* get the current time */
        if (gettimeofday(&tv, NULL) != 0)
        {
            BDBG_ERR(("%s: gettimeofday Failed!\n", __FUNCTION__));
            pthread_mutex_unlock(&pmsg->mutex);
            iRet = OS_FAILURE;
            goto error_handler;
        }

        /* set our timeout */
        abstime.tv_sec  = tv.tv_sec + iTimeout/CLOCKS_PER_SEC;
        abstime.tv_nsec = ( tv.tv_usec + (iTimeout%CLOCKS_PER_SEC) ) * 1000;
    }

    /* If no space, wait */
    while ( pmsg->ulHead == ( (pmsg->ulTail + 1) % (pmsg->ulDepth + 1) ) )
    {
        if (OS_WAIT_FOREVER == iTimeout)
        {
            if (pthread_cond_wait(&pmsg->condWaitFull, &pmsg->mutex) != 0)
            {
                BDBG_ERR(("%s: pthread_cond_wait ERROR\n", __FUNCTION__));
                iRet = OS_FAILURE;
                break;
            }
        }
        else if (OS_NO_WAIT == iTimeout)
        {
            iRet = OS_TIMEOUT;
            break;
        }
        else
        {
            iRet = pthread_cond_timedwait(&pmsg->condWaitFull, &pmsg->mutex, &abstime);
            if (iRet != 0)
            {
                if (iRet == ETIMEDOUT)
                {
                    iRet = OS_TIMEOUT;
                    break;
                }
                else
                {
                    BDBG_ERR(("%s: pthread_cond_timedwait ERROR = %s!\n", __FUNCTION__, strerror(errno)));
                    iRet = OS_FAILURE;
                    break;
                }
            }
        }
    }

    /* If we did not time out put it in the queue */
    if (OS_OK == iRet)
    {
        /* Put it in the queue */
        memcpy(pmsg->pbBuffers + (pmsg->ulTail * pmsg->ulSize ), strMsg, pmsg->ulSize);
        pmsg->ulTail = ( (pmsg->ulTail + 1) % (pmsg->ulDepth + 1) );

        /* Signal any waiters */
        if (pthread_cond_signal(&pmsg->condWaitEmpty))
        {
            BDBG_ERR(("%s: Broadcast Failed\n", __FUNCTION__));
        }
    }

    /* Release mutex */
    if (pthread_mutex_unlock(&pmsg->mutex))
    {
        BDBG_ERR(("%s: Failed to release lock\n", __FUNCTION__));
        goto error_handler;
    }

    /* Return success */
    return (iRet);

error_handler:

    /* Return failure */
    return (OS_FAILURE);
}

/**
 * OS Message Queue Receive function.
 *
 * @param mqId     - message queue identifier
 * @param *strMsg  - message pointer, the location of where the received message is to be placed after it has been received
 * @param uiSize   - message size (not used)
 * @param iTimeout - send timeout value, in ticks. (-1 = wait forever, 0 = no wait, > 1 = wait)
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 *    OS_TIMEOUT if a timeout occurred
 */

OS_STATUS 
OS_MsgQReceive(OS_MSG_Q_ID mqId, char *strMsg, uint32_t ulSize, int iTimeout)
{
    /* Check the validity of message queue */
    if (mqId == 0)
    {
        BDBG_ERR(("%s: BAD POINTER.\n", __FUNCTION__));
        return (OS_FAILURE);
    }

    /* Receive message from the head by default */
    return (OS_MsgQReceiveOption(mqId, strMsg, ulSize, iTimeout, OS_MSG_Q_HEAD) );
}

/**
 * OS Message Queue Remove function.
 *
 * @param mqId     - message queue identifier
 * @param *strMsg  - message pointer, the location of where the removed message is to be placed after it has been removed
 * @param uiSize   - message size (not used)
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 *    OS_TIMEOUT if msg queue is empty
 */
OS_STATUS OS_MsgQRemove(OS_MSG_Q_ID mqId, char *strMsg, uint32_t ulSize)
{
    /* Check the validity of message queue */
    if (mqId == 0)
    {
        BDBG_ERR(("%s: BAD POINTER.\n", __FUNCTION__));
        return (OS_FAILURE);
    }

    /* Receive message from the head by default */
    return (OS_MsgQReceiveOption(mqId, strMsg, ulSize, OS_NO_WAIT, OS_MSG_Q_TAIL) );
}

/**
 * OS Message Queue Receive Option function. Same as OS_MsgQReceive, but allow message to received
 *      from either the head or the tail of the message queue.
 *
 * @param mqId     - message queue identifier
 * @param *strMsg  - message pointer, the location of where the received message is to be placed after it has been received
 * @param uiSize   - message size (not used)
 * @param iTimeout - send timeout value, in ticks. (-1 = wait forever, 0 = no wait, > 1 = wait)
 * @param iOption  - head or tail of message queue (OS_MSG_Q_HEAD or OS_MSG_Q_TAIL)
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 *    OS_TIMEOUT if a timeout occurred
 */
static OS_STATUS OS_MsgQReceiveOption(OS_MSG_Q_ID mqId, char *strMsg, uint32_t ulSize, int iTimeout, int iOption)
{
    OS_STATUS iRet  = OS_OK;
    OS_MBOX   *pmsg = (OS_MBOX *)mqId;
    struct timeval  tv;
    struct timespec abstime;

    /* Check the validity of message queue */
    if (mqId == 0)
    {
        BDBG_ERR(("%s: INVALID mqId!\n", __FUNCTION__));
        goto error_handler;
    }

    /* Check the validity of message */
    if (strMsg == 0)
    {
        BDBG_ERR(("%s: BAD POINTER!\n", __FUNCTION__));
        goto error_handler;
    }

    if (ulSize != pmsg->ulSize)
    {
        BDBG_ERR(("%s: INVALID SIZE (param(%d) != queued(%d)!\n", __FUNCTION__, ulSize, pmsg->ulSize));
        goto error_handler;
    }

    /* Get Mutex */
    if (pthread_mutex_lock(&pmsg->mutex) != 0)
    {
        BDBG_ERR(("%s: Failed to obtain lock\n", __FUNCTION__));
        goto error_handler;
    }

    if ( (OS_WAIT_FOREVER != iTimeout) && (OS_NO_WAIT != iTimeout) )
    {
        /* get the current time */
        if (gettimeofday(&tv, NULL) != 0)
        {
            BDBG_ERR(("%s: gettimeofday Failed!\n", __FUNCTION__));
            pthread_mutex_unlock(&pmsg->mutex);
            iRet = OS_FAILURE;
            goto error_handler;
        }

        /* set our timeout */
        abstime.tv_sec  = tv.tv_sec + iTimeout/CLOCKS_PER_SEC;
        abstime.tv_nsec = ( tv.tv_usec + (iTimeout%CLOCKS_PER_SEC) ) * 1000;

        while ( abstime.tv_nsec > 1000000000 )
        {
            abstime.tv_sec  += 1;
            abstime.tv_nsec -= 1000000000;
        }
    }

    /* If empty queue, wait */
    while (pmsg->ulHead == pmsg->ulTail)
    {
        if (OS_WAIT_FOREVER == iTimeout)
        {
            if (pthread_cond_wait(&pmsg->condWaitEmpty, &pmsg->mutex) != 0)
            {
                BDBG_ERR(("%s: pthread_cond_wait ERROR!\n", __FUNCTION__));
                iRet = OS_FAILURE;
                break;
            }
        }
        else if (OS_NO_WAIT == iTimeout)
        {
            iRet = OS_TIMEOUT;
            break;
        }
        else
        {
            iRet = pthread_cond_timedwait(&pmsg->condWaitEmpty, &pmsg->mutex, &abstime);
            if (iRet != 0)
            {
                if (iRet == ETIMEDOUT)
                {
                    iRet = OS_TIMEOUT;
                    break;
                }
                else
                {
                    switch ( iRet )
                    {
                        case EINVAL:	BDBG_ERR(("OS_MsgQReceive: pthread_cond_timedwait ERROR = EINVAL\n"));	break;
                        case EFAULT:    BDBG_ERR(("OS_MsgQReceive: pthread_cond_timedwait ERROR = EFAULT\n"));	break;
                        case ETIMEDOUT: BDBG_ERR(("OS_MsgQReceive: pthread_cond_timedwait ERROR = ETIMEDOUT\n"));	break;
                       	default:		BDBG_ERR(("OS_MsgQReceive: pthread_cond_timedwait ERROR = unknown\n"));	break;
                    }

                    iRet = OS_FAILURE;
                    break;
                }
            }
        }
    }

    /* if we didn't time out, receive it from the queue */
    if (OS_OK == iRet)
    {
        /* Receive it from the queue */
        if (iOption == OS_MSG_Q_TAIL)
        {
            /* Receive from tail of queue */
            pmsg->ulTail = ( (pmsg->ulTail + pmsg->ulDepth) % (pmsg->ulDepth + 1) );
            memcpy(strMsg, pmsg->pbBuffers + (pmsg->ulTail * pmsg->ulSize), pmsg->ulSize);
        }
        else
        {
            /* Receive from head of queue */
            memcpy(strMsg, pmsg->pbBuffers + (pmsg->ulHead * pmsg->ulSize), pmsg->ulSize);
            pmsg->ulHead = ( (pmsg->ulHead + 1) % (pmsg->ulDepth + 1) );
        }

        /* Signal any waiters */
        if (pthread_cond_signal(&pmsg->condWaitFull))
        {
            BDBG_ERR(("%s: Broadcast Failed\n", __FUNCTION__));
        }
    }

    /* Release mutex */
    if (pthread_mutex_unlock(&pmsg->mutex))
    {
        BDBG_ERR(("%s: Failed to release lock\n", __FUNCTION__));
        goto error_handler;
    }

    /* Return success */
    return (iRet);

error_handler:

    /* Return failure */
    return (OS_FAILURE);
}

/**
 * Removes all messages from the queue.
 *
 * @param mqId - message queue identifier
 *
 * @retval OS_OK if successful, else an osapi error code.
 */
OS_STATUS OS_MsgQReset(OS_MSG_Q_ID mqId)
{
    OS_MBOX *pmsg = (OS_MBOX *)mqId;

    /* Check the validity of message queue */
    if (mqId == 0)
    {
        BDBG_ERR(("%s: BAD POINTER!\n", __FUNCTION__));
        goto error_handler;
    }

    /* Get Mutex */
    if (pthread_mutex_lock(&pmsg->mutex) != 0)
    {
        BDBG_ERR(("%s: Failed to obtain lock\n", __FUNCTION__));
        goto error_handler;
    }

    /* reset queue */
    pmsg->ulHead = pmsg->ulTail;

    /* Signal any waiters */
    if (pthread_cond_broadcast(&pmsg->condWaitEmpty))
    {
        BDBG_ERR(("%s: Broadcast Failed\n", __FUNCTION__));
    }
    if (pthread_cond_broadcast(&pmsg->condWaitFull))
    {
        BDBG_ERR(("%s: Broadcast Failed\n", __FUNCTION__));
    }

    /* Release mutex */
    if (pthread_mutex_unlock(&pmsg->mutex))
    {
        BDBG_ERR(("%s: Failed to release lock\n", __FUNCTION__));
        goto error_handler;
    }

    /* Return success */
    return (OS_OK);

error_handler:

    /* Return failure */
    return (OS_FAILURE);
}

/**
 * OS Message Queue Number of Messages function.
 *
 * @param
 *    OS_MSG_Q_ID mqId - message queue identifier
 *
 * @retval
 *    Returns the number of messages in the given message queue.
 */
int OS_MsgQNumMsgs(OS_MSG_Q_ID mqId)
{
    OS_MBOX *pmsg        = (OS_MBOX *)mqId;
    int     num_messages = 0;

    /* Check the validity of message queue */
    if (mqId == 0)
    {
        BDBG_ERR(("%s: BAD POINTER!\n", __FUNCTION__));
        return (OS_FAILURE);
    }

    /* Get Mutex */
    if (pthread_mutex_lock(&pmsg->mutex) == 0)
    {
        if (pmsg->ulHead > pmsg->ulTail)
        {
            num_messages = pmsg->ulTail + pmsg->ulDepth + 1 - pmsg->ulHead;
        }
        else
        {
            num_messages = pmsg->ulTail - pmsg->ulHead;
        }

        /* Release mutex */
        pthread_mutex_unlock(&pmsg->mutex);
    }
    else
    {
        num_messages = OS_FAILURE;
    }

    return (num_messages);
}

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Semaphore function definitions                                           **
**                                                                           **
*******************************************************************************
******************************************************************************/

/**
 * OS Semaphore Binary Create function.
 *
 * @param
 *    int iOSOptions - operating system options (not used)
 *
 * @param
 *    OS_SEM_B_STATE semsInitialState - initial semaphore state
 *
 * @retval
 *    OS_SEM_ID semId - semaphore identifier, if successful
 *    0 - if not successful
 */
OS_SEM_ID OS_SemBCreate(int iOSOptions, OS_SEM_B_STATE semsInitialState)
{
    OS_SEMAPHORE *psemRecord = NULL;

    /* Initialize warning flag for debugging deadlocks */
    if (warnOnWaitForeverSecs < 0)
    {
        char *varStr = getenv("WARN_ON_WAIT_FOREVER_SECS");
        warnOnWaitForeverSecs = 0;
        if (varStr != NULL)
        {
            long result = strtol(varStr, NULL, 0);
            if ((result > 0) &&
                (result < 65536))
            {
                warnOnWaitForeverSecs = (int16_t) result;
                BDBG_ERR(("%s: Semaphores taken for longer than %d secs will print warning",
                          __FUNCTION__, warnOnWaitForeverSecs));
            }
            else
            {
                BDBG_ERR(("%s: WARN_ON_WAIT_FOREVER_SECS value (%ld) is out of range, 1-65535",
                          __FUNCTION__, result));
            }
        }
    }

    /* Check the validity of the initial semaphore state */
    if ( (semsInitialState != OS_SEM_EMPTY) && (semsInitialState != OS_SEM_FULL) )
    {
        BDBG_ERR(("%s: Invalid initial state (%d)!\n", __FUNCTION__, semsInitialState));
        goto error;
    }

    /* Allocate a record for the semaphore */
    psemRecord = (OS_SEMAPHORE *)OS_MemAlloc(sizeof(OS_SEMAPHORE));
    if (psemRecord == NULL)
    {
        BDBG_ERR(("%s: Could not allocate semaphore record!\n", __FUNCTION__));
        goto error;
    }

    /* Set the semaphore type as binary */
    psemRecord->ulSemType = OS_SEM_BINARY;

    /* Set maximum to 1  */
    psemRecord->ulMaxCount = 0;

    /* Set count to the parameter */
    psemRecord->ulCount = semsInitialState;

    psemRecord->takenFname = NULL;
    psemRecord->takenLine = 0;
    memset(psemRecord->takenThread, 0, sizeof(psemRecord->takenThread));

    /* No name was specified for the semaphore */
    strncpy(psemRecord->strName, "\0", MAX_NAME_SIZE);

    /* Create the binary semaphore */
    if (pthread_cond_init(&psemRecord->condWait, NULL) != 0)
    {
        BDBG_ERR(("%s: Could not init condWait", __FUNCTION__));
        goto error;
    }
    if (pthread_mutex_init(&psemRecord->mutex, NULL) != 0)
    {
        BDBG_ERR(("%s: Could not init mutex", __FUNCTION__));
        goto error;
    }

    /* Return a handle to the binary semaphore */
    return (OS_SEM_ID)psemRecord;

error:
    if (NULL != psemRecord)
    {
        OS_MemFree(psemRecord);
    }
    return (0);
}

/**
 * OS Semaphore Binary Create function.
 *
 * @param iOSOptions - operating system options (not used)
 * @param semsInitialState - initial semaphore state
 * @param strName - Null terminated string.
 *
 * @retval
 *    OS_SEM_ID semId - semaphore identifier, if successful
 *    0 - if not successful
 */
OS_SEM_ID OS_SemBCreateNamed(int iOSOptions, OS_SEM_B_STATE semsInitialState, const char *strName)
{
    OS_SEM_ID semId = 0;
    OS_SEMAPHORE *psemRecord;

    /*
     *  Create a counting semaphore
     */
    semId = OS_SemBCreate(iOSOptions, semsInitialState);
    if (semId == 0)
    {
        BDBG_ERR(("%s: Could not create named semaphore!\n", __FUNCTION__));
        return 0;
    }

    /*
     *  Name the semaphore
     */
    psemRecord = (OS_SEMAPHORE *)semId;
    strncpy(psemRecord->strName, strName, MAX_NAME_SIZE);
    psemRecord->strName[MAX_NAME_SIZE-1] = '\0';

    /*
     *  Return a handle to the counting semaphore
     */
    return (OS_SEM_ID)psemRecord;
}

/**
 * OS Semaphore Count Create function.
 *
 * @param
 *    int iOSOptions - operating system options
 *
 * @param
 *    int iInitialCount - initial reference count
 *
 * @retval
 *    OS_SEM_ID semId - semaphore identifier, if successful
 *    0 - if not successful
 */
OS_SEM_ID OS_SemCCreate(int iOSOptions, int iInitialCount)
{
    OS_SEMAPHORE *psemRecord = NULL;

    /* Check the validity of the given initial count */
    if (iInitialCount < 0)
    {
        BDBG_ERR(("%s: Invalid initial count (%d)!\n", __FUNCTION__, iInitialCount));
        goto error;
    }

    /* Allocate a record for the semaphore */
    psemRecord = (OS_SEMAPHORE *)OS_MemAlloc(sizeof(OS_SEMAPHORE));
    if (psemRecord == NULL)
    {
        BDBG_ERR(("%s: Could not allocate semaphore record!\n", __FUNCTION__));
        goto error;
    }

    /* Set the semaphore type as counting */
    psemRecord->ulSemType = OS_SEM_COUNTING;

    /* No name was specified for the semaphore */
    strncpy(psemRecord->strName, "\0", MAX_NAME_SIZE);

    /* Set maximum count to some large number... */
    psemRecord->ulMaxCount = 32678;

    /* Set count to the parameter */
    psemRecord->ulCount = iInitialCount;

    psemRecord->takenFname = NULL;
    psemRecord->takenLine = 0;
    memset(psemRecord->takenThread, 0, sizeof(psemRecord->takenThread));

    /* Create the binary semaphore */
    if (pthread_cond_init(&psemRecord->condWait, NULL) != 0)
    {
        BDBG_ERR(("%s: Could not init condWait", __FUNCTION__));
        goto error;
    }
    if (pthread_mutex_init(&psemRecord->mutex, NULL) != 0)
    {
        BDBG_ERR(("%s: Could not init mutex", __FUNCTION__));
        goto error;
    }

    /* Return a handle to the counting semaphore */
    return (OS_SEM_ID)psemRecord;

error:
    if (NULL != psemRecord)
    {
        OS_MemFree(psemRecord);
    }
    return (0);
}

/**
 * OS Semaphore Count Create (Named) function.
 *
 * @param
 *    int iOSOptions - operating system options
 *
 * @param
 *    int iInitialCount - initial reference count
 *
 * @param
 *    char *strName - semaphore name
 *
 * @retval
 *    OS_SEM_ID semId - semaphore identifier, if successful
 *    0 - if not successful
 */
OS_SEM_ID OS_SemCCreateNamed(int iOSOptions, int iInitialCount, const char *strName)
{
    OS_SEM_ID semId = 0;
    OS_SEMAPHORE *psemRecord;

    /*
     *  Create a counting semaphore
     */
    semId = OS_SemCCreate( iOSOptions, iInitialCount );
    if (semId == 0)
    {
        BDBG_ERR(("%s: Could not create named semaphore!\n", __FUNCTION__));
        return 0;
    }

    /*
     *  Name the counting semaphore
     */
    psemRecord = (OS_SEMAPHORE *)semId;
    strncpy(psemRecord->strName, strName, MAX_NAME_SIZE);
    psemRecord->strName[MAX_NAME_SIZE-1] = 0;

    /*
     *  Return a handle to the counting semaphore
     */
    return (OS_SEM_ID)psemRecord;
}

/**
 * OS Semaphore Delete function.
 *
 * @param
 *    OS_SEM_ID semId - semaphore identifier
 *
 * @retval
 *    Returns OS_OK.
 *    This command is always successful.
 */
OS_STATUS OS_SemDelete(OS_SEM_ID semId)
{
    /* Cast the given semaphore identifier to a semaphore record */
    OS_SEMAPHORE *psemRecord = (OS_SEMAPHORE *)semId;

    if (semId == 0)
    {
        BDBG_ERR(("%s: BAD POINTER.\n", __FUNCTION__));
        return (OS_FAILURE);
    }

    /* release all wait states */
    OS_SemFlush(semId);

    /* Destroy the semaphore */
    if (pthread_cond_destroy(&psemRecord->condWait) != 0)
    {
        BDBG_ERR(("%s: Could not init condWait", __FUNCTION__));
    }
    if (pthread_mutex_destroy(&psemRecord->mutex) != 0)
    {
        BDBG_ERR(("%s: Could not init mutex", __FUNCTION__));
    }

    /* Delete the semaphore record */
    OS_MemFree(psemRecord);

    /* Return success */
    return (OS_OK);
}

/**
 * OS Semaphore Take function. Basically decrements the semaphore count, if
 * possible.
 *
 * @param
 *    OS_SEM_ID semId - semaphore identifier
 *
 * @param
 *    int iTimeout - semaphore take timeout value
 *
 * @retval
 *    OS_OK if successful
 *    OS_TIMEOUT if timout
 *    OS_FAILURE if not successful
 */
#undef OS_SemTake
extern "C" OS_STATUS OS_SemTake(OS_SEM_ID semId, int iTimeout)
{
    return OS_SemTakeTrack(semId, iTimeout, "unknown", 0);
}

OS_STATUS OS_SemTakeTrack(OS_SEM_ID semId, int iTimeout, const char *fname, int lineNum)
{
    int iRet                 = OS_OK;
    OS_SEMAPHORE *psemRecord = (OS_SEMAPHORE *)semId;
    struct timeval  tv;
    struct timespec abstime;
    char thisThreadName[OS_THREAD_NAME_SIZE];

    OS_ThreadNameSelf(thisThreadName);

    /* Check the validity of the semaphore */
    if (semId == 0)
    {
        BDBG_ERR(("%s: BAD POINTER.\n", __FUNCTION__));
        goto error_handler;
    }

    /* Check the validity of the given timeout value */
    if (iTimeout < OS_WAIT_FOREVER)
    {
        BDBG_ERR(("%s: Invalid timeout value (%d)!\n", __FUNCTION__, iTimeout));
        goto error_handler;
    }

    if (pthread_mutex_lock(&psemRecord->mutex) != 0)
    {
        BDBG_ERR(("%s: Failed to obtain lock\n", __FUNCTION__));
        goto error_handler;
    }

    if ( (OS_WAIT_FOREVER != iTimeout) && (OS_NO_WAIT != iTimeout) )
    {
        /* get the current time */
        if (gettimeofday(&tv, NULL) != 0)
        {
            BDBG_ERR(("%s: gettimeofday Failed!\n", __FUNCTION__));
            pthread_mutex_unlock(&psemRecord->mutex);
            iRet = OS_FAILURE;
            goto error_handler;
        }

        /* set our timeout */
        abstime.tv_sec  = tv.tv_sec + iTimeout/CLOCKS_PER_SEC;
        abstime.tv_nsec = ( tv.tv_usec + (iTimeout%CLOCKS_PER_SEC) ) * 1000;

        while ( abstime.tv_nsec > 1000000000 )
        {
            abstime.tv_sec  += 1;
            abstime.tv_nsec -= 1000000000;
        }
    }

    while (psemRecord->ulCount == 0)
    {
        if (OS_WAIT_FOREVER == iTimeout)
        {
            if ((warnOnWaitForeverSecs > 0) &&
                (psemRecord->ulSemType == OS_SEM_BINARY) &&
                (psemRecord->takenFname != NULL))
            {
                /* get the current time */
                if (gettimeofday(&tv, NULL) != 0)
                {
                    BDBG_ERR(("%s: gettimeofday Failed!\n", __FUNCTION__));
                    pthread_mutex_unlock(&psemRecord->mutex);
                    iRet = OS_FAILURE;
                    goto error_handler;
                }
                /* set our timeout to some future time to put out message */
                abstime.tv_sec  = tv.tv_sec + warnOnWaitForeverSecs;
                abstime.tv_nsec = ( tv.tv_usec ) * 1000;
                iRet = pthread_cond_timedwait(&psemRecord->condWait, &psemRecord->mutex, &abstime);
                if (iRet != 0)
                {
                    if (iRet == ETIMEDOUT)
                    {
                        BDBG_ERR(("Sem timeout (%dsec), Taken: %s@%d [%s], Waiting: %s@%d [%s]",
                                  warnOnWaitForeverSecs,
                                  psemRecord->takenFname, psemRecord->takenLine, psemRecord->takenThread,
                                  fname, lineNum, thisThreadName));
                        iRet = 0;
                    }
                    else
                    {
                        BDBG_ERR(("%s: pthread_cond_timedwait ERROR = %s!\n", __FUNCTION__, strerror(errno)));
                        iRet = OS_FAILURE;
                        break;
                    }
                }
            }
            else
            {
                iRet = pthread_cond_wait(&psemRecord->condWait, &psemRecord->mutex);
                if (iRet != 0)
                {
                    BDBG_ERR(("%s: pthread_cond_wait ERROR = %s!\n", __FUNCTION__, strerror(errno)));
                    iRet = OS_FAILURE;
                    break;
                }
            }
        }
        else if (OS_NO_WAIT == iTimeout)
        {
            iRet = OS_TIMEOUT;
            break;
        }
        else
        {
            iRet = pthread_cond_timedwait(&psemRecord->condWait, &psemRecord->mutex, &abstime);
            if (iRet != 0)
            {
                if (iRet == ETIMEDOUT)
                {
                    iRet = OS_TIMEOUT;
                    break;
                }
                else
                {
                    BDBG_ERR(("%s: pthread_cond_timedwait ERROR = %s!\n", __FUNCTION__, strerror(errno)));
                    iRet = OS_FAILURE;
                    break;
                }
            }
        }
    }

    if (psemRecord->ulCount != 0)
    {
        psemRecord->ulCount--;
        psemRecord->takenFname = fname;
        psemRecord->takenLine  = lineNum;
        OS_ThreadNameSelf(psemRecord->takenThread);
        iRet = OS_OK;
    }

    if (pthread_mutex_unlock(&psemRecord->mutex))
    {
        BDBG_ERR(("%s: Failed to release lock\n",__FUNCTION__));
        goto error_handler;
    }

    /* Return status */
    return iRet;

error_handler:

    /* Return failure */
    return (OS_FAILURE);
}

/**
 * OS Semaphore Take function.
 *
 * @param
 *    semId - semaphore identifier
 *
 * @param
 *    ulMilliSeconds - semaphore take timeout value in milliseconds
 *
 * @retval
 *    OS_OK if successful
 *    OS_TIMEOUT if timout
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_SemTakeMsec(OS_SEM_ID semId, int milliseconds)
{
    if ( (milliseconds == OS_WAIT_FOREVER) || (milliseconds == OS_NO_WAIT) )
    {
        return ( OS_SemTake(semId, milliseconds) );
    }
    else
    {
        return ( OS_SemTake(semId, milliseconds*(CLOCKS_PER_SEC/1000)) );
    }
}

/**
 * OS Semaphore Give function. Basically increments the semaphore count,
 * if possible.
 *
 * @param
 *    OS_SEM_ID semId - semaphore identifier
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_SemGive(OS_SEM_ID semId)
{
    OS_SEMAPHORE *psemRecord = (OS_SEMAPHORE *)semId;

    /* Check the validity of the semaphore */
    if (semId == 0)
    {
        BDBG_ERR(("%s: BAD POINTER.\n", __FUNCTION__));
        goto error_handler;
    }

    if (pthread_mutex_lock(&psemRecord->mutex) != 0)
    {
        BDBG_ERR(("%s: Failed to obtain lock\n", __FUNCTION__));
        goto error_handler;
    }

    /* If the semaphore is binary and the count is 1, return an error */
    if (psemRecord->ulSemType == OS_SEM_BINARY)
    {
        if (psemRecord->ulCount == 1)
        {
            pthread_mutex_unlock(&psemRecord->mutex);
            goto error_handler;
        }
    }

    /* Increment the semaphore count */
    psemRecord->ulCount++;

    /* wake one waiting thread */
    if (pthread_cond_signal(&psemRecord->condWait))
    {
        BDBG_ERR(("%s: Broadcast Failed\n", __FUNCTION__));
    }

    if (pthread_mutex_unlock(&psemRecord->mutex))
    {
        BDBG_ERR(("%s: Failed to release lock\n", __FUNCTION__));
        goto error_handler;
    }

    /* Return successfully */
    return OS_OK;

error_handler:

    /* Return failure */
    return (OS_FAILURE);
}

/**
 * OS Semaphore Flush function
 *
 * @param
 *    OS_SEM_ID semId - semaphore identifier
 *
 * @retval
 *    Returns OS_OK is successful, OS_FAILURE otherwise.
 */
OS_STATUS OS_SemFlush(OS_SEM_ID semId)
{
    OS_SEMAPHORE *psemRecord = (OS_SEMAPHORE *)semId;
    OS_STATUS    status      = OS_OK;

    /* Check the validity of the semaphore */
    if (0 == semId)
    {
        BDBG_ERR(("%s: BAD POINTER\n", __FUNCTION__));
        return (OS_FAILURE);
    }

    if (pthread_mutex_lock(&psemRecord->mutex) == 0)
    {
        if (pthread_cond_broadcast(&psemRecord->condWait) != 0)
        {
            BDBG_ERR(("%s: Broadcast Failed\n", __FUNCTION__));
            status = OS_FAILURE;
        }

        pthread_mutex_unlock(&psemRecord->mutex);
    }
    else
    {
        status = OS_FAILURE;
    }

    return (status);
}


/**
 * OS memory pool Information function. Prints out information about
 * the specified memory pool.  This information includes the number of
 * pages in the memory pool, the page size, the address of each memory
 * page, and if each page is allocated or not.
 *
 * @param pool   - The memory pool ID
 *
 * @retval
 *    None.
 */
void OS_MemoryInfo(OS_MEMPOOL_ID pool)
{
    if (pool == NULL)
    {
#ifdef __CALC_PEAK_ALLOCATION
        BDBG_MSG(("OS_MemoryInfo:\n"));
        BDBG_MSG(("/tPeak Allocations    = %d Bytes\n", ulPeakAllocations));
        BDBG_MSG(("/tCurrent Allocations = %d Bytes\n", ulCurrentAllocations));
#endif
    }
    else
    {
        OS_MEMPOOL *tmpPool = (OS_MEMPOOL *)pool;  /* Pointer to memory pool */
        int8_t       cUsed;                          /* Indicates if page allocated */
        int32_t       lAtt;                           /* Indicates # pages attached */

        BDBG_ERR( ("\n----- Memory Pool Information -----\n") );
        BDBG_ERR( ("Number of Pages: %d\n", tmpPool->ulNumPages) );
        BDBG_ERR( ("Page Size:       %d\n", tmpPool->ulPgSize) );
        BDBG_ERR( ("Memory Pages:\n") );
        BDBG_ERR( ("\tPage\tAddress\t\tAllocated?\n") );
        for (uint32_t i = 0; i < tmpPool->ulNumPages; i++)
        {
            if (tmpPool->memPage[i].lAttached < 0)
            {
                lAtt  =  -1 * tmpPool->memPage[i].lAttached;
                cUsed = 'Y';
            }
            else
            {
                lAtt  = tmpPool->memPage[i].lAttached;
                cUsed = 'N';
            }
            BDBG_ERR( ("\t%d:\t%x\t\t%d\t%c\n", i + 1, (uint32_t)tmpPool->memPage[i].pvPgAddr, lAtt, cUsed) );
        }
    }

    return;
}

/**
 * Creates a memory pool with the specified page size. This function does not
 * allocate the memory for the pool. The memory address is provided as an input.
 *
 * @param pAddr      - pointer to allocated memory buffer
 * @param ulSize     - Size of the memory pool
 * @param ulPageSize - Page size of allocations from the pool. All allocations
 *                     will be a multiple of this size.
 *
 * @retval non-negative memPoolID if success, NULL otherwise.
 *
 * @remark
 *    If ulSize is not a multiple of ulPageSize, then the memory pool
 *    will only contain the number of whole pages that fit in the size
 *    ulSize.
 */
OS_MEMPOOL_ID OS_CreateMemPool(void * pvAddr, uint32_t ulSize, uint32_t ulPageSize)
{
    void *          pvTmpAddr;           /* Address of a page in memory pool */
    OS_MEMPOOL     *tmpPool = NULL;     /* Pointer to memory pool */
    uint32_t          ulArraySize;         /* Size of array to hold memory pool */

    /*
     * If any of the parameters are invalid, return.
     */
    if ( (pvAddr == NULL) || (ulPageSize == 0) )
    {
        return (NULL);
    }

    /*
     * Allocate memory for the memory pool.
     */
    tmpPool = (OS_MEMPOOL *)OS_MemAlloc(sizeof(OS_MEMPOOL) );
    if (tmpPool == NULL)
    {
        return (NULL);
    }

    /*
     * Set value for memory pool page size and the number of pages.
     */
    tmpPool->ulPgSize   = ulPageSize;
    tmpPool->ulNumPages = ulSize / ulPageSize;

    /*
     * Allocate an array large enough to hold the memory pool.
     */
    ulArraySize         = sizeof(OS_MEMPOOL::Page) * tmpPool->ulNumPages;
    tmpPool->memPage    = (OS_MEMPOOL::Page *)OS_MemAlloc(ulArraySize);
    if (tmpPool->memPage == NULL)
    {
        (void)OS_MemFree(tmpPool);
        return (NULL);
    }

    /*
     * Give a memory address to each page in the memory pool.  Also,
     * each page is given a value of the number of pages attached to
     * it, including itself.
     */
    pvTmpAddr = pvAddr;
    for (uint32_t i = 0; i < tmpPool->ulNumPages; i++)
    {
        tmpPool->memPage[i].pvPgAddr   = pvTmpAddr;
        tmpPool->memPage[i].lAttached  = tmpPool->ulNumPages - i;
        pvTmpAddr                      = (uint8_t *)pvTmpAddr + ulPageSize;
    }

    return ( (OS_MEMPOOL_ID)tmpPool);
}

/**
 * Deletes a memory pool created by OS_CreateMemPool. This does not release the memory
 * buffer used by the pool. This memory should be released by the application.
 *
 * @param pool   - The memory pool ID
 *
 * @retval OS_OK if successful, an osapi error code otherwise.
 */
OS_STATUS OS_DeleteMemPool(OS_MEMPOOL_ID pool)
{
    OS_MEMPOOL *tmpPool = (OS_MEMPOOL *)pool;   /* Pointer to memory pool */

    /*
     * If memory pool ID is NULL, then return.
     */
    if (pool == NULL)
    {
        return (OS_INVALID_MEMPOOL);
    }

    /*
     * Free any memory used by memory pool.
     */
    OS_MemFree(tmpPool->memPage);
    OS_MemFree(pool);

    return (OS_OK);
}

/**
 * Allocates a block of memory from the specified memory pool.
 *
 * @param pool   - The memory pool ID
 * @param ulSize - Size of memory block to allocate.
 *
 * @retval - pointer to allocated memory if successful, NULL if not successful
 */
void * OS_MemPoolAlloc(OS_MEMPOOL_ID pool, uint32_t ulSize)
{
    int32_t        lPgNeeded;                     /* Number of pages needed to allocate the *
                                                * requested block of memory.             */
    OS_MEMPOOL  *tmpPool = (OS_MEMPOOL *)pool; /* Pointer to memory pool */
    uint32_t i;                                   /* Loop iterator */

    /*
     * If invalid memory pool, return.
     */
    if (pool == NULL)
    {
        return NULL;
    }

    /*
     * Determine how many pages are needed for the requested amount
     * of memory.
     */
    lPgNeeded = (ulSize / tmpPool->ulPgSize);
    if (ulSize % tmpPool->ulPgSize != 0)
    {
        ++lPgNeeded;
    }

    /* Search for a block of pages that have at least the number
     * of pages needed consecutively.  Once a block is found, return
     * the address of the first page of this block. If no blocks large
     * enough are found, then return NULL.
     */
    i = 0;
    while (i < tmpPool->ulNumPages)
    {
        /*
         * Look at the number of pages attached to the current page to
         * determine if the block of pages is large enough to use.  If
         * the number of pages attached is a negative value, then move ahead
         * that number of pages (positive value) because those pages are allocated.
         * If the number of pages attached is less than the number of pages
         * needed, then move ahead that number of pages because the block of
         * pages is not large enough.  Otherwise, the block is large enough,
         * so allocate a block starting at current page.
         */
        if (tmpPool->memPage[i].lAttached < 0)
        {
            i = i + (-1 * tmpPool->memPage[i].lAttached);
        }
        else if (tmpPool->memPage[i].lAttached < lPgNeeded)
        {
            i = i + tmpPool->memPage[i].lAttached;
        }
        else
        {
            /*
             * Adjust the number of pages attached to the allocated pages.
             * Simply change number to negative.
             */
            int32_t lPages = lPgNeeded;
            for (uint32_t j = i; j < (i + lPgNeeded); j++)
            {
                tmpPool->memPage[j].lAttached = -1 * lPages;
                lPages = lPages - 1;
            }

            return (tmpPool->memPage[i].pvPgAddr);
        }
    }

    return (NULL);
}

/**
 * Frees the given memory block back to the specified memory pool
 *
 * @param pool   - The memory pool ID
 * @param pvMemory - The address
 *
 * @retval - OS_OK if sucessful, an osapi error code otherwise.
 */
OS_STATUS OS_MemPoolFree(OS_MEMPOOL_ID pool, void * pvMemory)
{
    int32_t        lIndex,                          /* Page number in memory pool */
                lNextIndex;                      /* Page number of next block  */
    OS_MEMPOOL  *tmpPool = (OS_MEMPOOL *)pool;   /* Pointer to memory pool */
    uint32_t       ulFront,                         /* Beginning and end address of */
                ulEnd;                           /* memory pool.                 */
    /*
     * If null pointer was passed, return.
     */
    if (pvMemory == NULL)
    {
        return (OS_NULL_POINTER);
    }

    /*
     * Check that memory is in the specified memory pool. If it's not
     * then do not free the memory.
     */
    ulFront = (uint32_t)tmpPool->memPage[0].pvPgAddr;
    ulEnd   = (uint32_t)tmpPool->memPage[tmpPool->ulNumPages - 1].pvPgAddr +
              tmpPool->ulPgSize;
    if ( ( (uint32_t)pvMemory < ulFront) || ( (uint32_t)pvMemory >= ulEnd) ||
         (pool == NULL) )
    {
        return (OS_INVALID_MEMPOOL);
    }

    /*
     * Determine, from the address, which page(s) to free.  If address
     * is not a page address, then return.
     */
    lIndex     = (uint8_t *)pvMemory - (uint8_t *)tmpPool->memPage[0].pvPgAddr;
    if (lIndex % tmpPool->ulPgSize != 0)
    {
        return (OS_FAILURE);
    }
    lIndex     = lIndex / tmpPool->ulPgSize;

    /*
     * Make sure that the memory address given is not already free.
     */
    if (tmpPool->memPage[lIndex].lAttached >= 0)
    {
        return (OS_FAILURE);
    }

    /*
     * Adjust the number of pages attached to the freed page(s).
     */
    lNextIndex = lIndex + (-1 * tmpPool->memPage[lIndex].lAttached);
    int32_t i;
    if (tmpPool->memPage[lNextIndex].lAttached > 0)
    {
        /*
         * Need to add the number of pages attached to the pages
         * following the freed pages.
         */
        for (i = lIndex; i < lNextIndex; i++)
        {
            tmpPool->memPage[i].lAttached =
                    (-1 * tmpPool->memPage[i].lAttached) +
                    tmpPool->memPage[lNextIndex].lAttached;
        }
    }
    else
    {
        /*
         * No non-allocated pages following the freed pages.
         */
        for (i = lIndex; i < lNextIndex; i++)
        {
            tmpPool->memPage[i].lAttached =
                    (-1 * tmpPool->memPage[i].lAttached);
        }
    }

    /*
     * Adjust the number of pages attached to any pages preceding the
     * freed pages.
     */
    i = lIndex - 1;
    while ( (tmpPool->memPage[i].lAttached >= 0) && (i >= 0) )
    {
        tmpPool->memPage[i].lAttached += tmpPool->memPage[lIndex].lAttached;
        i = i - 1;
    }

    return (OS_OK);
}

/**
 * Frees all memory blocks back to the specified memory pool
 *
 * @param pool   - The memory pool ID
 *
 * @retval - OS_OK if sucessful, an osapi error code otherwise.
 */
OS_STATUS OS_MemPoolReset(OS_MEMPOOL_ID pool)
{
    OS_MEMPOOL *pMemPool = (OS_MEMPOOL *)pool;

    if (pool == NULL)
    {
        BDBG_ERR(("%s: NULL pointer!\n", __FUNCTION__));
        return (OS_NULL_POINTER);
    }

    /* Free all pages of mempool */
    for (uint32_t i = 0; i < pMemPool->ulNumPages; i++)
    {
        pMemPool->memPage[i].lAttached  = pMemPool->ulNumPages - i;
    }

    return (OS_OK);
}

/**
 * Allocates a block of memory from the OS memory pool.
 *
 * @param ulSize - memory size.
 *
 * @retval
 *    pvMemory - pointer to allocated memory, if successful
 *    NULL - if not successful
 */
#ifdef DMALLOC
/*
** Look at the bottom of this file for definitions of
** OS_MemAlloc/OS_MemFree and why they are needed.
*/
#else
void * OS_MemAlloc(uint32_t ulSize)
{
    void * pvMemory = NULL;

#ifdef __CALC_PEAK_ALLOCATION
    if (fFirstAlloc == true)
    {
        memset(test, 0, sizeof(mem_info) * 512);
        fFirstAlloc = false;
    }
#endif

    /*
     *  Check the validity of the given memory size
     */
    if (ulSize == 0)
    {
        BDBG_ERR(("%s: Memory size is 0!\n", __FUNCTION__));
        return (NULL);
    }

    /*
     *  Allocate a block of the given size from the OS memory
     */
    pvMemory = malloc(ulSize);

#ifdef __CALC_PEAK_ALLOCATION
    if (pvMemory != NULL)
    {
        for (int i=0; i<512; i++)
        {
            if (test[i].pvAddr == NULL)
            {
                test[i].pvAddr = pvMemory;
                test[i].size   = ulSize;
                break;
            }
        }

        ulCurrentAllocations += ulSize;
        if (ulPeakAllocations < ulCurrentAllocations)
        {
            ulPeakAllocations = ulCurrentAllocations;
        }
    }
#endif

    BDBG_ASSERT(pvMemory != NULL);

    /*
     *  Return a pointer to the allocated block
     */
    return (pvMemory);
}

/**
 * Frees the given memory block back to the OS memory pool.
 *
 * @param pvMemory - pointer to the memory to be freed
 *
 * @retval OS_OK if successful, otherwise an osapi error code
 */
OS_STATUS OS_MemFree(void * pvMemory)
{
    /*
     *  Check the validity of the given memory pointer
     */
    if (pvMemory == NULL)
    {
        BDBG_ERR(("%s: Memory pointer is NULL!\n", __FUNCTION__));
        return (OS_NULL_POINTER);
    }

#ifdef __CALC_PEAK_ALLOCATION
    for (int i=0; i<512; i++)
    {
        if (test[i].pvAddr == pvMemory)
        {
            ulCurrentAllocations -= test[i].size;

            test[i].pvAddr = NULL;
            test[i].size   = 0;
            break;
        }
    }
#endif

    /*
     *  Make sure the OS memory pool has been created
     */

    /*
     *  Free the given block back to the OS memory
     */
    free(pvMemory);

    return (OS_OK);
}
#endif 
/* #endif on line above is for DMALLOC */


/**
 * OS Initialize DRAM Top function.
 *
 * @param
 *    None.
 *
 * @retval
 *    OS_MEMORY_BASE - offset to the top of DRAM, if successful
 *    NULL - if not successful
 */
OS_STATUS OS_InitDramTop(void * pStartAddr, uint32_t ulMemSize)
{
    return (OS_NOT_IMPLEMENTED);
}

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Timer function prototypes                                                **
**                                                                           **
*******************************************************************************
******************************************************************************/

#ifdef VC_TIMERS

typedef struct tagThreadTimer
{
    int iThreadID;              /* the ID of the thread in charge of this timer */
    timer_t timerid;            /* the slot value and unique timer id */
    struct sigevent evp;        /* description of event, only callback type supported */
    char strThreadName[17];     /* name of thread, this shall be VCTimerThreadXXX */
    struct itimerspec itimer;   /* this contains the timeout or repeat value */
    OS_SEM_ID semTimerSet;      /* used to block for timeout specified */
    bool fTimerSetPending;   /* set when timer set is pending / cleared once timer starts counting */
    bool fTimerCounting;     /* set while timer is counting */
    bool fTimerExitPending;  /* set when timer exit is pending */
    struct timespec abstime;
} ThreadTimer;

#define TOTAL_TIMERS 200
#define VC_TIMER_THREAD_STACK_SIZE  16384

/* @todo Track the timer list with a dynamic clist */
ThreadTimer *pttMasterList[TOTAL_TIMERS] = { NULL };

/**
 * VCI Pthread-based, Posix-style Timer
 *
 * @param
 *    clockid_t clock_id
 *
 * @param
 *    struct sigevent *evp
 *
 * @param
 *    timer_t *timerid

 * @retval
 *    0 if successful
 *    -1 if not successful
 */
int vc_timer_create( clockid_t clockid, struct sigevent *evp, timer_t *timerid )
{

    int iReturn = 0;
    int i = 0;
    const char *strThreadName = "VCTimerThread";
    ThreadTimer *pttThreadTimer = NULL;
    bool fFoundOpen = false;

    /* we only handle REALTIME at this time */
    if ( CLOCK_REALTIME != clockid )
    {
        BDBG_ERR(("%s - clockid problem %08X\n", __FUNCTION__, clockid) );
        iReturn = -1;
        goto leave;
    }

    /* we only treat call-back signals */
    if ( NULL == evp )
    {
        BDBG_ERR(("%s - NULL POINTER\n", __FUNCTION__) );
        iReturn = -1;
        goto leave;
    }

    /* malloc a structure to store all of the timer info including the threadid, timerid, and all of the private params */
    if ( NULL == ( pttThreadTimer = (ThreadTimer*)OS_MemAlloc(sizeof(ThreadTimer)) ) )
    {
        BDBG_ERR(("%s - OUT OF MEMORY\n", __FUNCTION__) );
        iReturn = -1;
        goto leave;
    }

    /* @todo - add new pThreadTimer to a list of some sort(possibly a clist) */
    /* find a free slot in the global timer storage */
    for (i = 0; i < TOTAL_TIMERS; i++)
    {
        if (pttMasterList[i] == NULL)
        {
            pttMasterList[i] = pttThreadTimer;
            fFoundOpen = true;
            break;
        }
    }

    if ( false == fFoundOpen )
    {
        BDBG_ERR(("%s - can't find an open timer slot\n", __FUNCTION__) );
        iReturn = -1;
        goto leave;
    }

    /* Intially timer is waiting for a timer to be set and not counting */
    pttThreadTimer->fTimerExitPending = false;
    pttThreadTimer->fTimerSetPending  = false;
    pttThreadTimer->fTimerCounting    = false;

    /* This is a semaphore that allows the thread to wait for the time that timer is set to */
    pttThreadTimer->semTimerSet = OS_SemBCreate(OS_SEM_Q_FIFO, OS_SEM_EMPTY);

    /* The evp contains the function that gets called from the timer and its context variable */
    pttThreadTimer->evp = *evp;

    /* Store the global index value for referring to this timer in the future */
    *timerid = (timer_t)i;
    pttThreadTimer->timerid = (timer_t)i;

    /* Set a unique number for each new thread timer */
    snprintf( pttThreadTimer->strThreadName, sizeof( pttThreadTimer->strThreadName ), "%s%02d", strThreadName, i );

    /* Spawn a new task with high priority to act as a timer handler */
    if ( OS_FAILURE == ( pttThreadTimer->iThreadID = OS_ThreadSpawnParam(pttThreadTimer->strThreadName,
																	   OSAPI_THREAD_PRIORITY_TIMER,
																	   VC_TIMER_THREAD_STACK_SIZE,
																	   TimerThreadProc,
																	   pttThreadTimer, NULL ) ) )
    {
        BDBG_ERR(("%s - iThreadID problem %08X\n", __FUNCTION__, pttThreadTimer->iThreadID) );
        iReturn = -1;
        goto leave;
    }

    BDBG_MSG(("%s - %d Launched %s with PID %d\n", __FUNCTION__, pttThreadTimer->timerid,
             pttThreadTimer->strThreadName, pttThreadTimer->iThreadID) );

leave:

    if ( 0 != iReturn )
    {
        free( pttThreadTimer );
        pttThreadTimer = NULL;

        if ( true == fFoundOpen )
        {
            pttMasterList[(int)*timerid] = NULL;
        }
    }

    return( iReturn );

} /* end vc_timer_create() */

/**
 * VCI Pthread-based, Posix-style Timer
 *
 * @param
 *    timer_t timerid
 *
 * @retval
 *    0 if successful
 *    -1 if not successful
 */
int vc_timer_delete( timer_t timerid )
{
    int iReturn = 0;
    ThreadTimer *pttThreadTimer;

    /* get the right record */
    if ( NULL == ( pttThreadTimer = pttMasterList[(int)timerid] ) )
    {
        BDBG_ERR(("%s - invalid timerid %08X\n", __FUNCTION__, timerid) );
        iReturn = -1;
        goto leave;
    }

    BDBG_MSG(("%s - Deleting timer %d\n", __FUNCTION__, timerid) );

    /* timer should stop counting and exit */
    pttThreadTimer->fTimerSetPending  = false;
    pttThreadTimer->fTimerCounting    = false;
    pttThreadTimer->fTimerExitPending = true;

    /* zero out the time values */
    pttThreadTimer->itimer.it_value.tv_sec = 0;
    pttThreadTimer->itimer.it_value.tv_nsec = 0;
    pttThreadTimer->itimer.it_interval.tv_sec = 0;
    pttThreadTimer->itimer.it_interval.tv_nsec = 0;

    /* make sure timer is not counting or waiting for a time to be set */
    OS_SemGive(pttThreadTimer->semTimerSet);

    /* delete the timer thread */
    OS_ThreadJoin(pttThreadTimer->iThreadID);
    OS_ThreadDelete(pttThreadTimer->iThreadID);

    /* if this fails, continue to muscle through the rest of deletion */
    if ( OS_FAILURE == OS_SemDelete( pttThreadTimer->semTimerSet ) )
    {
        BDBG_ERR(("%s - OS_SemDelete failure %d\n", __FUNCTION__, pttThreadTimer->iThreadID) );
        iReturn = -1;
    }

    /* free the structure for this timerthread */
    free( pttThreadTimer );
    pttThreadTimer = NULL;

    /* remove it from the global list */
    pttMasterList[(int)timerid] = NULL;

leave:

    return( iReturn );

} /* end vc_timer_delete() */

/**
 * VCI Pthread-based, Posix-style Timer
 *
 * @param
 *    timer_t timerid
 *
 * @param
 *    int flags
 *
 * @param
 *    const struct itimerspec *value
 *
 * @param
 *    struct itimerspec *ovalue
 *
 * @retval
 *    0 if successful
 *    -1 if not successful
 */
int vc_timer_settime( timer_t timerid, int flags, const struct itimerspec *value, struct itimerspec *ovalue )
{
    int          iReturn = 0;
    ThreadTimer  *pttThreadTimer;
    OS_SEMAPHORE *psemRecord;

    if (NULL == value)
    {
        BDBG_ERR(("%s - NULL POINTER\n", __FUNCTION__) );
        iReturn = -1;
        goto leave;
    }

    /* get the right record */
    if ( NULL == ( pttThreadTimer = pttMasterList[(int)timerid] ) )
    {
        BDBG_ERR(("%s - invalid timerid %08X\n", __FUNCTION__, timerid) );
        iReturn = -1;
        goto leave;
    }

    /* Check the validity of the semaphore */
    if (pttThreadTimer->semTimerSet == 0)
    {
        BDBG_ERR(("%s - Failure, NULL semaphore\n", __FUNCTION__) );
        iReturn = -1;
        goto leave;
    }

    /* init our local semaphore pointer */
    psemRecord = (OS_SEMAPHORE *)pttThreadTimer->semTimerSet;

    /* lock things down */
    if (pthread_mutex_lock(&psemRecord->mutex) != 0)
    {
        BDBG_ERR(("%s: Failed to obtain lock\n",__FUNCTION__));
        iReturn = -1;
        goto leave;
    }

    /* this is going to set the timer expiration value for select() */
    pttThreadTimer->itimer = *value;

    if ( ( pttThreadTimer->itimer.it_value.tv_sec == 0 ) &&
        ( pttThreadTimer->itimer.it_value.tv_nsec == 0 ) &&
        ( pttThreadTimer->itimer.it_interval.tv_sec == 0 ) &&
        ( pttThreadTimer->itimer.it_interval.tv_nsec == 0 ) )
    {
        /*
         * Time was set to 0, which means to stop the timer. If the timer
         * is currently couting, then we want it to stop counting.  Otherwise,
         * just let timer to continue to wait until it is set to a non-zero value.
         */

        if (pttThreadTimer->fTimerCounting)
        {
            pttThreadTimer->fTimerSetPending = false;
            pttThreadTimer->fTimerCounting   = false;

            if (psemRecord->ulCount == 0)
            {
                /* Increment the semaphore count */
                psemRecord->ulCount++;

                /* wake the timer thread */
                pthread_cond_signal(&psemRecord->condWait);
            }
        }
        else if (pttThreadTimer->fTimerSetPending)
        {
            pttThreadTimer->fTimerSetPending = false;
        }
    }
    else
    {
        /*
         * Time was set to a non-zero value.
         * If the timer is not currently counting, then it will start counting.
         * If the timer is counting, then it will continue to count, but it will
         * count from the new time value.
         */
        pttThreadTimer->fTimerSetPending = true;

        if (psemRecord->ulCount == 0)
        {
            /* Increment the semaphore count */
            psemRecord->ulCount++;

            /* wake the timer thread */
            pthread_cond_signal(&psemRecord->condWait);
        }
    }

    /* release the mutex */
    pthread_mutex_unlock(&psemRecord->mutex);

leave:

    return( iReturn );

} /* end vc_timer_settime() */

/**
 * VCI Pthread-based, Posix-style Timer
 *
 * @param
 *    timer_t timerid
 *
 * @param
 *    struct itimerspec *value
 *
 * @retval
 *    0 if successful
 *    -1 if not successful
 */
int vc_timer_gettime( timer_t timerid, struct itimerspec *ptimerspec )
{
    int            iReturn = 0;
    struct timeval time_start;
    ThreadTimer    *pttThreadTimer;
    OS_SEMAPHORE   *psemRecord;

    if (NULL == ptimerspec)
    {
        BDBG_ERR(("%s - NULL POINTER\n", __FUNCTION__) );
        iReturn = -1;
        goto leave;
    }

    /* get the right record */
    if ( NULL == ( pttThreadTimer = pttMasterList[(int)timerid] ) )
    {
        BDBG_ERR(("%s - invalid timerid %08X\n", __FUNCTION__, timerid) );
        iReturn = -1;
        goto leave;
    }

    /* Check the validity of the semaphore */
    if (pttThreadTimer->semTimerSet == 0)
    {
        BDBG_ERR(("%s - Failure, NULL semaphore\n", __FUNCTION__) );
        iReturn = -1;
        goto leave;
    }

    /* init our local semaphore pointer */
    psemRecord = (OS_SEMAPHORE *)pttThreadTimer->semTimerSet;

    /* lock things down */
    if (pthread_mutex_lock(&psemRecord->mutex) != 0)
    {
        BDBG_ERR(("%s: Failed to obtain lock\n",__FUNCTION__));
        iReturn = -1;
        goto leave;
    }

    /* if there is a set pending then just take the time from that */
    if (pttThreadTimer->fTimerSetPending)
    {
        ptimerspec->it_value.tv_sec  = pttThreadTimer->itimer.it_value.tv_sec;
        ptimerspec->it_value.tv_nsec = pttThreadTimer->itimer.it_value.tv_nsec;

        /* release the mutex */
        pthread_mutex_unlock(&psemRecord->mutex);
        goto leave;
    }

    if ( ( pttThreadTimer->itimer.it_value.tv_sec == 0 ) &&
         ( pttThreadTimer->itimer.it_value.tv_nsec == 0 ) &&
         ( pttThreadTimer->itimer.it_interval.tv_sec == 0 ) &&
         ( pttThreadTimer->itimer.it_interval.tv_nsec == 0 ) )
    {
        /* timer is stopped */
        ptimerspec->it_interval.tv_sec  = 0;
        ptimerspec->it_interval.tv_nsec = 0;
        ptimerspec->it_value.tv_sec     = 0;
        ptimerspec->it_value.tv_nsec    = 0;
    }
    else
    {
        /* get the current time */
        if (gettimeofday(&time_start, NULL) == 0)
        {
            /* return the timer info */
            ptimerspec->it_interval.tv_sec  = pttThreadTimer->itimer.it_interval.tv_sec;
            ptimerspec->it_interval.tv_nsec = pttThreadTimer->itimer.it_interval.tv_nsec;

            /* projected time must be greater than current for comparison */
            if (pttThreadTimer->abstime.tv_sec > time_start.tv_sec)
            {
                ptimerspec->it_value.tv_sec  = pttThreadTimer->abstime.tv_sec  - time_start.tv_sec;
                ptimerspec->it_value.tv_nsec = pttThreadTimer->abstime.tv_nsec - (time_start.tv_usec * 1000);
            }
            else if (pttThreadTimer->abstime.tv_nsec > (time_start.tv_usec * 1000))
            {
                ptimerspec->it_value.tv_sec  = pttThreadTimer->abstime.tv_sec  - time_start.tv_sec;
                ptimerspec->it_value.tv_nsec = pttThreadTimer->abstime.tv_nsec - (time_start.tv_usec * 1000);
            }
            else
            {
                /* already expired */
                ptimerspec->it_value.tv_sec  = 0;
                ptimerspec->it_value.tv_nsec = 0;
            }
        }
        else
        {
            iReturn = -1;
        }
    }

    /* release the mutex */
    pthread_mutex_unlock(&psemRecord->mutex);

leave:

    return( iReturn );

} /* end vc_timer_gettime() */

/**
 * VCI Pthread-based, Posix-style Timer
 *
 * @param
 *    void * pvParam
 *
 * @retval
 *    0 if successful
 *    -1 if not successful
 */
uint32_t TimerThreadProc( void * pvParam )
{
    ThreadTimer  *pttThreadTimer = (ThreadTimer*)pvParam;
    struct       timeval  time;
    struct       timeval  time_start;
    bool      fRepeat = false;
    OS_SEMAPHORE *psemRecord;

    if (NULL == pttThreadTimer)
    {
        BDBG_ERR(("%s - Failure, NULL pttThreadTimer\n", __FUNCTION__) );
        goto leave;
    }

    /* Check the validity of the semaphore */
    if (pttThreadTimer->semTimerSet == 0)
    {
        BDBG_ERR(("%s - Failure, NULL semaphore\n", __FUNCTION__) );
        goto leave;
    }

    /* init our local semaphore pointer */
    psemRecord = (OS_SEMAPHORE *)pttThreadTimer->semTimerSet;

    /* lock things down */
    if (pthread_mutex_lock(&psemRecord->mutex) != 0)
    {
        BDBG_ERR(("%s: Failed to obtain lock\n",__FUNCTION__));
        goto leave;
    }

    /* Get rid of compiler warnings by initializing time */
    time.tv_sec  = 0;
    time.tv_usec = 0;

    /*
     * Do not exit task until the timer has been deleted.  Each time a timer is expired,
     * just continue to wait until a new value is set or until the timer has been deleted.
     */
    do
    {
        /* wait until the timer rate has been issued */
        while (psemRecord->ulCount == 0)
        {
            /* wait forever */
            if (pthread_cond_wait(&psemRecord->condWait, &psemRecord->mutex) != 0)
            {
                /* kick out due to an error */
                break;
            }
        }
        if (psemRecord->ulCount != 0)
        {
            psemRecord->ulCount--;
        }

        /* If a timer set is pending, then proceed to start the timer */
        if ( (pttThreadTimer->fTimerSetPending == true) && (pttThreadTimer->fTimerExitPending == false) )
        {
            /*
             * Continue counting while there is a repeat time value set, or while
             * new time values are being set.
             */
            do
            {
                /* get the current time */
                if (gettimeofday(&time_start, NULL) != 0)
                {
                    BDBG_ERR(("%s: gettimeofday Failed!\n",__FUNCTION__));
                    OS_ThreadYield();
                    continue;
                }

                /* check to see if the timer variables need set */
                if (pttThreadTimer->fTimerSetPending)
                {
                    /* set the initial absolute time */
                    pttThreadTimer->abstime.tv_sec  = time_start.tv_sec;
                    pttThreadTimer->abstime.tv_nsec = time_start.tv_usec * 1000;

                    /* check to see if it has been updated */
                    time.tv_sec  = pttThreadTimer->itimer.it_value.tv_sec;
                    time.tv_usec = pttThreadTimer->itimer.it_value.tv_nsec/1000;

                    /* is this a repeating timer? */
                    if ( (pttThreadTimer->itimer.it_interval.tv_sec > 0) ||
                        (pttThreadTimer->itimer.it_interval.tv_nsec > 0) )
                    {
                        fRepeat = true;
                    }
                    else
                    {
                        fRepeat = false;
                    }

                    pttThreadTimer->fTimerSetPending = false;
                    pttThreadTimer->fTimerCounting   = true;
                }

                /* increment the absolute time each loop by the timer delay length. Then use this as
                 * the absolute timeout of the semaphore below. This will make sure the timer rate
                 * is not skewed by the processing time of the timer callback */
                pttThreadTimer->abstime.tv_sec  += time.tv_sec;
                pttThreadTimer->abstime.tv_nsec += time.tv_usec * 1000;
                if (pttThreadTimer->abstime.tv_nsec > 1000000000)
                {
                    pttThreadTimer->abstime.tv_sec  += 1;
                    pttThreadTimer->abstime.tv_nsec -= 1000000000;
                }

                /* if the absolute time has already passed then we have to reset
                 * which will result in a missed timer fire. To minimize this the timer callback
                 * should execute very quickly. */
                if ( (time_start.tv_sec > pttThreadTimer->abstime.tv_sec) ||
                    ( ((time_start.tv_usec * 1000) > pttThreadTimer->abstime.tv_nsec) &&
                    (time_start.tv_sec == pttThreadTimer->abstime.tv_sec) ) )
                {
                    /* set the initial absolute time */
                    pttThreadTimer->abstime.tv_sec  = time_start.tv_sec + time.tv_sec;
                    if (time_start.tv_usec + time.tv_usec > 1000000)
                    {
                        pttThreadTimer->abstime.tv_sec++;
                        pttThreadTimer->abstime.tv_nsec = ( (time_start.tv_usec + time.tv_usec) * 1000 ) - 1000000000;
                }
                    else
                {
                    pttThreadTimer->abstime.tv_nsec = (time_start.tv_usec + time.tv_usec) * 1000;
                }
                }

                /*
                 * Wait for the specified amount of time.  If the timer is set again while we
                 * are waiting, then we will stop waiting and start waiting for the new specified
                 * amount of time.  OS_ThreadDelay() will not work here because if a new time is
                 * set, it will not know to restart the timer at the new time value.
                 */
                while (psemRecord->ulCount == 0)
                {
                    if (pthread_cond_timedwait(&psemRecord->condWait, &psemRecord->mutex, &pttThreadTimer->abstime) != 0)
                    {
                        /* kick out due to timeout or error */
                        break;
                    }
                }
                if (psemRecord->ulCount != 0)
                {
                    psemRecord->ulCount--;
                }

                /* Send a callback.
                 * If the timer is in the process of exiting, do not send a callback.
                 * If the timer has been set to a new value (timer set pending), then
                 * do not send a callback.
                 */
                if ( (NULL != pttThreadTimer->evp.sigev_notify_function) &&
                     !(pttThreadTimer->fTimerExitPending || pttThreadTimer->fTimerSetPending) &&
                     (pttThreadTimer->fTimerCounting == true) )
                {
                    /* unlock mutex during the callback to prevent possible deadlocks */
                    pthread_mutex_unlock(&psemRecord->mutex);

                    if (NULL != pttThreadTimer->evp.sigev_value.sival_ptr)
                    {
                        ( (OS_FUNCPTR_PARAM)(pttThreadTimer->evp.sigev_notify_function) )(pttThreadTimer->evp.sigev_value.sival_ptr);
                    }
                    else
                    {
                        ( (OS_FUNCPTR)(pttThreadTimer->evp.sigev_notify_function) )();
                    }

                    /* reaquire lock */
                    pthread_mutex_lock(&psemRecord->mutex);
                }

            } while ( ( (fRepeat == true) || (pttThreadTimer->fTimerSetPending == true) ) &&
                (pttThreadTimer->fTimerExitPending == false) );

            /* Timer is no longer counting */
            pttThreadTimer->fTimerCounting = false;
        }

    } while (pttThreadTimer->fTimerExitPending == false);

    /* release the mutex */
    pthread_mutex_unlock(&psemRecord->mutex);

leave:

    pthread_exit(0);
    return (OS_OK);

} /* end TimerThreadProc() */

#endif

/**
 * OS Timer Create function
 *
 * @param
 *    OS_TIMER_ID **ptimerId - pointer to an timer identifier pointer
 *
 * @param
 *    OS_FUNCPTR func - timer procedure function
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerCreate(OS_TIMER_ID *ptimerId, OS_FUNCPTR func)
{
    return (OS_TimerCreateParam(ptimerId, (OS_FUNCPTR_PARAM)func, NULL));
}

/**
 * OS Timer Create function
 *
 * @param
 *    OS_TIMER_ID **ptimerId - pointer to an timer identifier pointer
 *
 * @param
 *    OS_FUNCPTR func - timer procedure function
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerCreateParam(OS_TIMER_ID *ptimerId, OS_FUNCPTR_PARAM func, void * pvParam)
{
    OS_TIMER          *pTimer = NULL;
    OS_STATUS         status  = OS_OK;

    /* allocate the timer structure */
    pTimer = (OS_TIMER*)OS_MemAlloc(sizeof(OS_TIMER));
    if (pTimer != NULL)
    {
        struct sigevent sigev;
        memset (&sigev, 0, sizeof (struct sigevent));

        pTimer->pvFunction = (void *)func;
        pTimer->pvParam    = pvParam;

        /* info to pass to the callback function */
        sigev.sigev_value.sival_int = 0;
        sigev.sigev_value.sival_ptr = pTimer;

        sigev.sigev_notify = SIGEV_THREAD;
        sigev.sigev_notify_attributes = NULL;
        sigev.sigev_notify_function = linuxTimer;

        if (timer_create(CLOCK_REALTIME, &sigev, &pTimer->timerID) < 0)
        {

            BDBG_ERR(("OS_TimerCreate: FAILED\n"));

            /* free memory */
            OS_MemFree(pTimer);

            /* return an error */
            status = OS_FAILURE;

        }
        else
        {
            *ptimerId = (OS_TIMER_ID)pTimer;
        }
    }
    else
    {
        status = OS_NO_MEMORY;
    }

    return (status);
}

/**
 * OS Timer Delete function
 *
 * @param
 *    OS_TIMER_ID *ptimerId - pointer to an timer identifier
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerDelete(OS_TIMER_ID ptimerId)
{
    OS_TIMER  *pTimer = (OS_TIMER *)ptimerId;
    OS_STATUS status  = OS_OK;

    if (pTimer != NULL)
    {
        if (timer_delete(pTimer->timerID) < 0)
        {
            status = OS_FAILURE;
        }

        /* free memory */
        OS_MemFree(pTimer);
    }
    else
    {
        status = OS_FAILURE;
    }

    if (status != OS_OK) {
        BDBG_ERR(("OS_TimerDelete: FAILED\n"));
    }

    return (status);
}

/**
 * OS Timer Stop function
 *
 * @param
 *    OS_TIMER_ID *ptimerId - pointer to an timer identifier
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerStop(OS_TIMER_ID ptimerId)
{
    OS_TIMER  *pTimer = (OS_TIMER *)ptimerId;
    OS_STATUS status  = OS_OK;

    if (NULL != pTimer)
    {
        struct itimerspec itimer;

        /* setting the timer period to 0 stop it */
        itimer.it_interval.tv_sec  = 0;
        itimer.it_interval.tv_nsec = 0;
        itimer.it_value.tv_sec     = 0;
        itimer.it_value.tv_nsec    = 0;

        /* add the timer */
        if (timer_settime (pTimer->timerID, 0, &itimer, NULL) < 0)
        {
            status = OS_FAILURE;
        }
    }
    else
    {
        status = OS_FAILURE;
    }

    if (status != OS_OK) {
        BDBG_ERR(("OS_TimerStop: FAILED\n"));
    }

    return (status);
}

/**
 * OS Timer Set function
 *
 * @param
 *    OS_TIMER_ID *ptimerId - pointer to an timer identifier
 *
 * @param
 *    uint32_t ulPeriod - timer period
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerSet(OS_TIMER_ID ptimerId, uint32_t ulPeriod)
{
    OS_TIMER  *pTimer = (OS_TIMER *)ptimerId;
    OS_STATUS status  = OS_OK;
    
    if (NULL != pTimer)
    {
        struct itimerspec itimer;
        
        /* configure timer
         * it_interval is zero for non-repeating timers */
        itimer.it_interval.tv_sec  = 0;
        itimer.it_interval.tv_nsec = 0;
        itimer.it_value.tv_sec     = ulPeriod/CLOCKS_PER_SEC;
        itimer.it_value.tv_nsec    = (ulPeriod%CLOCKS_PER_SEC)*1000;
        
        /* add the timer */
        if (timer_settime (pTimer->timerID, 0, &itimer, NULL) < 0)
        {
            status = OS_FAILURE;
        }
    }
    else
    {
        status = OS_FAILURE;
    }
    
    if (status != OS_OK) {
        BDBG_ERR(("OS_TimerSet: FAILED\n"));
    }
    return (status);
}

/**
 * OS Timer Set (msec) function
 *
 * @param
 *    OS_TIMER_ID *ptimerId - pointer to an timer identifier
 *
 * @param
 *    uint32_t ulPeriod - timer period (in msec)
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS 
OS_TimerSetMsec(
    OS_TIMER_ID ptimerId, 
    uint32_t ulPeriod)
{
    /* convert milliseconds to ticks */
    ulPeriod = (ulPeriod*(CLOCKS_PER_SEC/1000));

    /* schedule the timer */
    return (OS_TimerSet(ptimerId, ulPeriod));
}

/**
 * OS Timer Set Repeat function
 *
 * @param
 *    OS_TIMER_ID *ptimerId - pointer to an timer identifier
 *
 * @param
 *    uint32_t ulPeriod - timer period
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerSetRepeat(OS_TIMER_ID ptimerId, uint32_t ulPeriod)
{
    OS_TIMER  *pTimer = (OS_TIMER *)ptimerId;
    OS_STATUS status  = OS_OK;

    if (NULL != pTimer)
    {
        struct itimerspec itimer;

        /* configure timer
         * it_interval is zero for non-repeating timers */
        itimer.it_interval.tv_sec  = ulPeriod/CLOCKS_PER_SEC;;
        itimer.it_interval.tv_nsec = (ulPeriod%CLOCKS_PER_SEC)*1000;
        itimer.it_value.tv_sec     = itimer.it_interval.tv_sec;
        itimer.it_value.tv_nsec    = itimer.it_interval.tv_nsec;

        /* add the timer */
        if (timer_settime (pTimer->timerID, 0, &itimer, NULL) < 0)
        {
            status = OS_FAILURE;
        }
    }
    else
    {
        status = OS_FAILURE;
    }

    if (status != OS_OK) {
        BDBG_ERR(("OS_TimerDelete: FAILED\n"));
    }

    return (status);
}

/**
 * OS Timer Set Repeat (msec) function
 *
 * @param
 *    OS_TIMER_ID *ptimerId - pointer to an timer identifier
 *
 * @param
 *    uint32_t ulPeriod - timer period (in msec)
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS  OS_TimerSetRepeatMsec(OS_TIMER_ID ptimerId, uint32_t ulPeriod)
{
    /* convert milliseconds to ticks */
    ulPeriod = (ulPeriod*(CLOCKS_PER_SEC/1000));

    /* schedule the timer */
    return (OS_TimerSetRepeat(ptimerId, ulPeriod));
}

OS_STATUS OS_TimerGetTime(OS_TIMER_ID ptimerID, uint32_t *pulTimeRemaining)
{
    OS_TIMER  *pTimer = (OS_TIMER *)ptimerID;
    OS_STATUS status  = OS_OK;

    if ( (NULL != pTimer) && (NULL != pulTimeRemaining) )
    {
        struct itimerspec itimer;

        if (timer_gettime (pTimer->timerID, &itimer) < 0)
        {
            status = OS_FAILURE;
        }
        else
        {
            *pulTimeRemaining = ((itimer.it_value.tv_sec * CLOCKS_PER_SEC) + (itimer.it_value.tv_nsec / 1000));
        }
    }
    else
    {
        status = OS_FAILURE;
    }

    return (status);
}

/**
 * OS Timer Get Time (msec) function
 *
 * @param
 *    OS_TIME_ID *ptimerId - pointer to an timer identifier
 *
 * @param
 *    uint32_t *pulTimeRemaining - pointer to time remaining (in msec)
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_TimerGetTimeMsec(OS_TIMER_ID ptimerID, uint32_t *pulTimeRemaining)
{
    OS_TIMER  *pTimer = (OS_TIMER *)ptimerID;
    OS_STATUS status  = OS_OK;

    if ( (NULL != pTimer) && (NULL != pulTimeRemaining) )
    {
        struct itimerspec itimer;

        if (timer_gettime (pTimer->timerID, &itimer) < 0)
        {
            status = OS_FAILURE;
        }
        else
        {
            /* calculate time and convert to milliseconds */
            *pulTimeRemaining = (itimer.it_value.tv_sec * 1000) + (itimer.it_value.tv_nsec / 1000000);
        }
    }
    else
    {
        status = OS_FAILURE;
    }

    return (status);
}

/**
 * Get Timer Func function
 *
 * @param
 *    OS_TIMER_ID *ptimerId - pointer to an timer identifier
 *
 * @retval
 *    OS_FUNCPTR func - timer function,if successful
 *    NULL - if not successful
 */
OS_FUNCPTR GetTimerFunc(OS_TIMER_ID ptimerId)
{
    OS_TIMER  *pTimer = (OS_TIMER *)ptimerId;

    if (NULL != pTimer)
    {
        return ((OS_FUNCPTR)pTimer->pvFunction);
    }
    else
    {
        return (OS_FUNCPTR)NULL;
    }
}

uint32_t OS_GetTickRate(void)
{
    return (CLOCKS_PER_SEC);
}

uint32_t OS_GetTicks(void)
{
    struct timeval  tp;
    gettimeofday(&tp, NULL);
    return ((tp.tv_sec * CLOCKS_PER_SEC) + tp.tv_usec);
}


/******************************************************************************
*******************************************************************************
**                                                                           **
**  I/O function prototypes                                                  **
**                                                                           **
*******************************************************************************
******************************************************************************/

/**
 * OS Open function
 *
 * @param strPath - file path
 * @param fd      - Returned file descriptor on success.
 *
 * @return OS_STATUS
 */
OS_STATUS OS_Open(const char *strPath, OS_ACCESS_FLAGS iOFlags, OS_FILE_HANDLE* fd)
{
    int io_flags = O_RDONLY;

    switch(iOFlags)
    {
        case OS_RDONLY:
            io_flags = O_RDONLY;
            break;
        case OS_WRONLY:
            io_flags = O_WRONLY;
            break;
        case OS_RDWR:
            io_flags = O_RDWR;
            break;
    }

    *fd = (OS_FILE_HANDLE)open(strPath, io_flags);
    if (*fd < 0)
    {
        BDBG_ERR(("%s(): ERROR - Couldn't open file\n", __FUNCTION__));
        return (OS_FAILURE);
    }

    return (OS_OK);
}

/**
 * OS Close function
 *
 * @param fd - file descriptor
 *
 * @return OS_STATUS
 */
OS_STATUS OS_Close(OS_FILE_HANDLE fd)
{
    if (close(fd) < 0)
    {
        return (OS_FAILURE);
    }

    return (OS_OK);
}

/**
 * @brief OS-dependant directory separator.
 *
 * Should be used by OS abstraction API user module when creating or parsing directory path.
 */
extern const char OS_c_directorySeparator[] = "/";

/**
 * Opens directory and attempts to read first entry
 *
 * @param directoryName     Full path to the directory whose content should be read, without the trailing delimiter
 *                          (forward slash on Unix).
 * @param fileFilter        Filter for files to be read, e.g. "*.mp3" or "m*.txt". Currently not used, all the files are always read.
 * @param directory         Pointer to the handle of the directory whose read was requested. To be used
 *                          in subsequent calls to OS_ReadNextDirectoryEntry.
 * @param dirEntryDesc      Pointer to the structure description of the first entry will be copied to if first entry
 *                          is found.
 *
 * @retval OS_OK           - If directory exists and first entry (sub-directory or file) is found.
 * @retval OS_FAILURE      - In case no files that match the specified filter are found in the directory.
 * @retval OS_NULL_POINTER - If any of parameters is NULL.
 */
OS_STATUS OS_OpenDirectoryAndReadFirstEntry(const char* directoryName, const char* fileFilter,
                                            OS_FILE_HANDLE* directory, OS_DIR_ENTRY_DESC* dirEntryDesc)
{
    if ((directoryName == NULL) || (directory == NULL) || (dirEntryDesc == NULL))
    {
        BDBG_ERR(("%s(): ERROR - Null pointer passed in.\n", __FUNCTION__));
        return (OS_NULL_POINTER);
    }

    OS_STATUS status = OS_OK;
    char dirName[255];  /* filter the files that will be read from the directory */

    strcpy(dirName, directoryName);
    strcat(dirName, OS_c_directorySeparator);
    //strcat(dirFilter, fileFilter);

    DIR* hDirectory = opendir(dirName);
    if ( hDirectory )
    {
        *directory = (OS_FILE_HANDLE)hDirectory; /* Return directory handle */

        /* Read directory entry */
        dirent* dirEntry;
        if ( (dirEntry = readdir(hDirectory)) )
        {
            strcpy(dirEntryDesc->name, dirEntry->d_name); /* Get entry name */
        }
        else
        {
            status = OS_FAILURE;
        }
    }
    else
    {
        BDBG_ERR(("opendir() unsuccessful for %s\n",dirName));
        status = OS_FAILURE;
    }

    return status;
}



/**
 * Reads next entry of the directory previously open with OS_OpenDirectoryAndReadFirstEntry
 *
 * @param directory    - Handle of the directory to read next entry of. Obtained by calling OS_OpenDirectoryAndReadFirstEntry.
 * @param dirEntryDesc - Pointer to the structure description of the next entry will be copied to if next entry is found.
 *
 * @retval OS_OK           - If next entry (sub-directory or file) is found.
 * @retval OS_FAILURE      - In case there are no more files in the directory.
 * @retval OS_NULL_POINTER - If dirEntryDesc parameter is NULL.
 */
OS_STATUS OS_ReadNextDirectoryEntry(OS_FILE_HANDLE directory, OS_DIR_ENTRY_DESC* dirEntryDesc)
{
    if ((directory == 0) || (dirEntryDesc == NULL))
    {
        BDBG_ERR(("%s(): ERROR - Null pointer passed in.\n", __FUNCTION__));
        return (OS_NULL_POINTER);
    }

    OS_STATUS status = OS_OK;

    DIR* hDirectory = (DIR*)directory;

    /* Read directory entry */
    dirent* dirEntry;
    if ( (dirEntry = readdir(hDirectory)) )
    {
        strcpy(dirEntryDesc->name, dirEntry->d_name); /* Get entry name */
    }
    else
    {
        status = OS_FAILURE;
    }

    return status;
}



/**
 * Closes the directory opened by OS_OpenDirectoryAndReadFirstEntry
 *
 * @param directory - Handle of the directory to close.
 */
void OS_CloseDirectory(OS_FILE_HANDLE directory)
{
    DIR* hDirectory = (DIR*)directory;
    if ( hDirectory )
    {
        closedir(hDirectory);
    }
}


/**
 * OS Device Add function
 *
 * @param
 *    OS_DEV_HDR *pdevHdr - pointer to a device header
 *
 * @param
 *    char *name - device name
 *
 * @param
 *    int iDrvnum - drive number
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_iosDevAdd(OS_DEV_HDR *pdevHdr, char *name, int iDrvnum)
{
    return OS_OK;
}

/**
 * OS Device Install function
 *
 * @param
 *    OS_FUNCPTR funcCreate - create function
 *
 * @param
 *    OS_FUNCPTR funcDelete - delete function
 *
 * @param
 *    OS_FUNCPTR funcOpen - open function
 *
 * @param
 *    OS_FUNCPTR funcClose - close function
 *
 * @param
 *    OS_FUNCPTR funcRead - read function
 *
 * @param
 *    OS_FUNCPTR funcWrite - write function
 *
 * @param
 *    OS_FUNCPTR funcIOCTL - IOCTL function
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_iosDrvInstall(OS_FUNCPTR funcCreate,
                           OS_FUNCPTR funcDelete,
                           OS_FUNCPTR funcOpen,
                           OS_FUNCPTR funcClose,
                           OS_FUNCPTR funcRead,
                           OS_FUNCPTR funcWrite,
                           OS_FUNCPTR funcIOCTL
                           )
{
    return OS_OK;
}

/**
 * OS Interrupt Lock function
 *
 * @param
 *    None.
 *
 * @retval
 *    OS_OK if successful
 *    OS_FAILURE if not successful
 */
OS_STATUS OS_IntLock(void)
{
    return OS_OK;
}

/**
 * OS Interrupt Unlock function
 *
 * @param
 *    int iLockKey - key used to unlock
 *
 * @retval
 *    None.
 */
void OS_IntUnlock(int iLockKey)
{
}

/*
 * Converts a pthread result code into a string. Not all codes are supported.
 * When this function returns "unknown pthread return code," this function
 * ought to be modified with the correct value. (Google it).
 */
char const* getPthreadReturnCodeName(int pthread_result)
{
    switch(pthread_result)
    {
        case EBUSY:
            return "EBUSY";
        break;
        case EINVAL:
            return "EINVAL";
        break;
        case EAGAIN:
            return "EAGAIN";
        break;
        case ENOMEM:
            return "ENOMEM";
        break;
        default:
            return "Unknown pthread return code";
        break;
    }
}

/*
 * PRIVATE FUNCTIONS
 *****************************************************************************/

static void * linuxThread(void * param)
{
    OS_THREAD *pthreadRecord = (OS_THREAD*)param;

    if (NULL != pthreadRecord)
    {
#ifdef BDVD_PRCTL
        if (pthreadRecord->strName)
            prctl(PR_SET_NAME, pthreadRecord->strName, 0, 0.,0);
        else
            prctl(PR_SET_NAME, "dvd_spawn", 0, 0.,0);
#endif

        if (NULL != pthreadRecord->pvParam)
        {
            ( (OS_FUNCPTR_PARAM)(pthreadRecord->pvFunction) )(pthreadRecord->pvParam);
        }
        else
        {
            ( (OS_FUNCPTR)(pthreadRecord->pvFunction) )();
        }
    }

    pthread_exit(0);
    return 0;
}

static void linuxTimer(union sigval value)
{
    OS_TIMER *pTimer = (OS_TIMER *)value.sival_ptr;

    if (NULL != pTimer)
    {
        if (NULL != pTimer->pvParam)
        {
            ( (OS_FUNCPTR_PARAM)(pTimer->pvFunction) )(pTimer->pvParam);
        }
        else
        {
            ( (OS_FUNCPTR)(pTimer->pvFunction) )();
        }
    }
}

#ifdef DMALLOC
/*
** We need to declare these functions anyway because sometimes we are
** dealing with object files that cannot be recompiled and this object
** files will have references to OS_MemAlloc/OS_MemFree.
*/
#undef OS_MemAlloc
#undef OS_MemFree
extern "C" void * 
OS_MemAlloc(uint32_t ulSize) { 
    return (void *) malloc(ulSize); 
}
extern "C" OS_STATUS 
OS_MemFree(void * pvMemory) { 
    return (OS_STATUS) free(pvMemory); 
}

#endif
