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
 * $brcm_Workfile: osapi.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 6/15/11 5:25p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/osapi/osapi.h $
 * 
 * 1   6/15/11 5:25p steven
 * SW7425-714: Adding OSAPI to AppLibs
 *
 ***************************************************************************/
/**
 * @file osapi.h
 *
 * Operating System API (OSAPI) header file. Provides an abstraction from the
 * operating system
 *
 * $Id: osapi.h,v 1.18.14.9.6.1.2.2 2008/11/06 19:58:44 micah Exp $
 */
#ifndef OSAPI_H
#define OSAPI_H

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Header files                                                             **
**                                                                           **
*******************************************************************************
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*******************************************************************************
**                                                                           **
**  OSAPI defines                                                            **
**                                                                           **
*******************************************************************************
******************************************************************************/

/*
 *  General
 */

/*
 *  Error codes
 */
#define OS_OK                   0
#define OS_FAILURE              (-1)
#define OS_NULL_POINTER         (-2)
#define OS_NOT_IMPLEMENTED      (-3)
#define OS_NO_MEMORY            (-4)
#define OS_INVALID_MEMPOOL      (-5)
#define OS_FILE_ERROR           (-6)
#define OS_FILE_EOF             (-7)
#define OS_CANCELLED            (-8)
#define OS_TIMEOUT              (-9)
#define OS_MBOX_FULL            (-10)

/*
 * Time Delays
 */
#define OS_WAIT_FOREVER         (-1)
#define OS_NO_WAIT              0       /* JCA - ADDED */
#define OS_WAIT_1S              (OS_GetTickRate())
#define OS_WAIT_2S              (OS_GetTickRate() << 1)
#define OS_WAIT_4S              (OS_GetTickRate() << 2)
#define OS_WAIT_8S              (OS_GetTickRate() << 3)
#define OS_WAIT_16S             (OS_GetTickRate() << 4)


/*
 *  Message queues
 */
#define OS_MSG_Q_FIFO           0x00    /* Used with message queues */
#define OS_MSG_PRI_NORMAL       0
#define OS_MSG_PRI_URGENT       1

/*
 *  Semaphores
 */
#define OS_SEM_Q_FIFO           0x00
#define OS_SEM_Q_PRIORITY       0x01
#define OS_SEM_EMPTY            0       /* For OS_SEM_B_STATE type */
#define OS_SEM_FULL             1
#define OS_SEM_BINARY           0       /* JCA - ADDED */
#define OS_SEM_COUNTING         1       /* JCA - ADDED */
#define OS_SEM_NULL             0       /* JMU - ADDED */

/*
 *  Miscellaneous
 */
#define MSB(x) (((x) >> 8) & 0xff)  /* Used only in vcd_fifo.c */
#define LSB(x) ((x) & 0xff)

/******************************************************************************
*******************************************************************************
**                                                                           **
**  OSAPI typedefs                                                           **
**                                                                           **
*******************************************************************************
******************************************************************************/
/*
 *  General
 */
typedef int OS_STATUS;              /* prefix: none  */


/*
 *  Tasks
 */
typedef uint32_t (*OS_FUNCPTR)(void);          /* prefix: func  */
typedef uint32_t (*OS_FUNCPTR_PARAM)(void *);   /* prefix: func  */


/* Message Queue Handle */
typedef uint32_t OS_MSG_Q_ID;

/* Semaphores Handle */
typedef uint32_t OS_SEM_ID;
typedef bool OS_SEM_B_STATE;

/* Memory Handle */
typedef void * OS_MEMPOOL_ID;

/* Timers Handle */
typedef uint32_t OS_TIMER_ID;

typedef OS_STATUS OS_THREAD_ID;

/* FILE IO */
/**
 * @brief File handle used in OS file I/O abstraction API.
 */
typedef int OS_FILE_HANDLE;

/**
 * @brief Description of the directory entry (file or directory).
 *
 * This is the structure that fully describes directory entry (file or directory), includint its full name and attributes.
 */
typedef struct
{
    bool        isDirectory;            /**< true if entry is a directory, false if it is a file. */
    uint32_t          fileSize;               /**< File size in bytes, valid only if entry is a file (isDirectory is false). */
    char           name[256];  /**< Name or full name (path or full path) of the entry. */
} OS_DIR_ENTRY_DESC;

/**
 * What type of open to use.
 */
typedef enum
{
    OS_RDONLY,
    OS_WRONLY,
    OS_RDWR
} OS_ACCESS_FLAGS;

extern const char OS_c_directorySeparator[];

/*
 *  Devices
 */
typedef struct                      /* prefix: node  */
{
    struct os_node *next;
    struct os_node *previous;
} OS_NODE;

typedef struct                      /* prefix: dev   */
{
    OS_NODE node;
    int16_t   drvNum;
    char    *name;
} OS_DEV_HDR;

/****************************************************
 *	Task Control Central
 ***************************************************/

#define OS_THREAD_MIN_PRIORITY     1
#define OS_THREAD_MAX_PRIORITY     256
#define OS_THREAD_LOW_PRIORITY     64
#define OS_THREAD_NORMAL_PRIORITY  128
#define OS_THREAD_RAISED_PRIORITY  160
#define OS_THREAD_HIGH_PRIORITY    192
#define OS_THREAD_NAME_SIZE        17

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Thread function prototypes                                               **
**                                                                           **
*******************************************************************************
******************************************************************************/
int         OS_ThreadSpawn(const char *strName, int iPrio, int iStackSize, OS_FUNCPTR func);
OS_STATUS   OS_ThreadSpawnParam(const char *strName, int iPrio, int iStackSize, OS_FUNCPTR_PARAM func, void * pvParam, void * pvStackBaseParam);
void        OS_ThreadDelete(int taskId);
void        OS_ThreadExit(void);
OS_STATUS   OS_ThreadReset(int taskId);
OS_STATUS   OS_ThreadSuspend(int taskId);
OS_STATUS   OS_ThreadResume(int taskId);
#define     OS_ThreadRestart OS_ThreadResume   /**< for legacy code */
void        OS_ThreadYield(void);
void        OS_ThreadDelay(int iTicks);
void        OS_ThreadDelayMsec(int milliseconds);
uint32_t    OS_ThreadIdSelf(void);
bool        OS_ThreadLock(void);
bool        OS_ThreadUnlock(void);
OS_STATUS   OS_ThreadJoin(int taskId);
OS_STATUS   OS_ThreadVerify(int taskId);
int         OS_ThreadNameToId(char *strName);
char*       OS_ThreadName(int taskId);
void        OS_ThreadNameSelf(char strName[OS_THREAD_NAME_SIZE]);
void        OS_ThreadShowPc(int taskId);


/******************************************************************************
*******************************************************************************
**                                                                           **
**  Process function prototypes                                              **
**                                                                           **
*******************************************************************************
******************************************************************************/
#define     OS_PROCESS_MAX_ARGUMENTS    10

/* TODO: Add signal handlers */

int         OS_ProcessSpawn(int iPrio, char *pCommand, char *pArgs);
OS_STATUS   OS_ProcessDelete(int taskId);
OS_STATUS   OS_ProcessWait(int taskId, int miliseconds);
OS_STATUS   OS_ProcessWaitFor(int taskId, int miliseconds, bool *pStop, char *pcSearchStr, char *pResults, uint32_t ulLength);
OS_STATUS   OS_ProcessSignal(int taskId, int sig_num);


/******************************************************************************
*******************************************************************************
**                                                                           **
**  Message queue function prototypes                                        **
**                                                                           **
*******************************************************************************
******************************************************************************/
OS_MSG_Q_ID OS_MsgQCreate(int iDepth, int iMaxMsgSize, int iOSOptions);
OS_STATUS   OS_MsgQDelete(OS_MSG_Q_ID mqId);
OS_STATUS   OS_MsgQSend(OS_MSG_Q_ID mqId, char *strMsg, uint32_t ulSize, int iTimeout, int iPrio);
OS_STATUS   OS_MsgQReceive(OS_MSG_Q_ID mqId, char *strMsg, uint32_t ulSize, int iTimeout);
OS_STATUS   OS_MsgQRemove(OS_MSG_Q_ID mqId, char *strMsg, uint32_t ulSize);
OS_STATUS   OS_MsgQReset(OS_MSG_Q_ID mqId);
int         OS_MsgQNumMsgs(OS_MSG_Q_ID mqId);

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Semaphore function prototypes                                            **
**                                                                           **
*******************************************************************************
******************************************************************************/
OS_SEM_ID   OS_SemBCreate(int iOSOptions, OS_SEM_B_STATE semsInitialState);
OS_SEM_ID   OS_SemBCreateNamed(int iOSOptions, OS_SEM_B_STATE semsInitialState, const char *strName);
OS_SEM_ID   OS_SemCCreate(int iOSOptions, int iInitialCount);
OS_SEM_ID   OS_SemCCreateNamed(int iOSOptions, int iInitialCount, const char *strName);
OS_STATUS   OS_SemTakeTrack(OS_SEM_ID semId, int iTimeout, const char *fname, int lineNum);
#define     OS_SemTake(id, to) OS_SemTakeTrack(id, to, __FILE__, __LINE__)
OS_STATUS   OS_SemTakeMsec(OS_SEM_ID semId, int milliseconds);
OS_STATUS   OS_SemGive(OS_SEM_ID semId);
OS_STATUS   OS_SemDelete(OS_SEM_ID semId);
OS_STATUS   OS_SemFlush(OS_SEM_ID semId);

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Memory function prototypes                                               **
**                                                                           **
*******************************************************************************
******************************************************************************/
void        OS_MemoryInfo(OS_MEMPOOL_ID pool);
OS_STATUS   OS_InitDramTop(void * pStartAddr, uint32_t ulMemSize);
#ifdef DMALLOC
#  define   OS_MemAlloc(size) dmalloc_malloc(__FILE__, __LINE__, (size), DMALLOC_FUNC_MALLOC, 0, 0)
#  define   OS_MemFree(ptr)   dmalloc_free(__FILE__, __LINE__, (ptr), DMALLOC_FUNC_FREE)
#else
  void *      OS_MemAlloc(uint32_t ulSize);
  OS_STATUS   OS_MemFree(void * pvMemory);
#endif
OS_MEMPOOL_ID OS_CreateMemPool(void * pAddr, uint32_t ulSize, uint32_t ulPageSize);
OS_STATUS     OS_DeleteMemPool(OS_MEMPOOL_ID pool);
void *        OS_MemPoolAlloc(OS_MEMPOOL_ID pool, uint32_t ulSize);
OS_STATUS     OS_MemPoolFree(OS_MEMPOOL_ID pool, void * pvMemory);
OS_STATUS     OS_MemPoolReset(OS_MEMPOOL_ID pool);

/******************************************************************************
*******************************************************************************
**                                                                           **
**  Timer function prototypes                                                **
**                                                                           **
*******************************************************************************
******************************************************************************/
OS_STATUS   OS_TimerCreate(OS_TIMER_ID *ptimerId, OS_FUNCPTR func);
OS_STATUS   OS_TimerCreateParam(OS_TIMER_ID *ptimerId, OS_FUNCPTR_PARAM func, void * pvParam);
OS_STATUS   OS_TimerDelete(OS_TIMER_ID ptimerId);
OS_STATUS   OS_TimerStop(OS_TIMER_ID ptimerId);
OS_STATUS   OS_TimerSet(OS_TIMER_ID ptimerId, uint32_t ulPeriod);
OS_STATUS   OS_TimerSetMsec(OS_TIMER_ID ptimerId, uint32_t ulPeriod);
OS_STATUS   OS_TimerSetRepeat(OS_TIMER_ID ptimerId, uint32_t ulPeriod);
OS_STATUS   OS_TimerSetRepeatMsec(OS_TIMER_ID ptimerId, uint32_t ulPeriod);
OS_STATUS   OS_TimerGetTime(OS_TIMER_ID ptimerID, uint32_t *pulTimeRemaining);
OS_STATUS   OS_TimerGetTimeMsec(OS_TIMER_ID ptimerID, uint32_t *pulTimeRemaining);
OS_FUNCPTR  GetTimerFunc(OS_TIMER_ID ptimerId);
uint32_t    OS_GetTickRate(void);
uint32_t    OS_GetTicks(void);
#define     OS_GetTicksPerSecond OS_GetTickRate    /**< for legacy code */
#define     OS_GetTickCount      OS_GetTicks       /**< for legacy code */

/******************************************************************************
*******************************************************************************
**                                                                           **
**  I/O function prototypes                                                  **
**                                                                           **
*******************************************************************************
******************************************************************************/
OS_STATUS   OS_Open(const char *strPath, OS_ACCESS_FLAGS iOFlags, OS_FILE_HANDLE* file);
OS_STATUS   OS_Close(OS_FILE_HANDLE fd);
OS_STATUS 	OS_iosDevAdd(OS_DEV_HDR *pdevHdr, char *name, int iDrvnum);
OS_STATUS 	OS_iosDrvInstall( OS_FUNCPTR funcCreate,
                              OS_FUNCPTR funcDelete,
                              OS_FUNCPTR funcOpen,
                              OS_FUNCPTR funcClose,
                              OS_FUNCPTR funcRead,
                              OS_FUNCPTR funcWrite,
                              OS_FUNCPTR funcIOCTL );
OS_STATUS 	OS_IntLock(void);
void      	OS_IntUnlock(int iLockKey);

/* Directory access */
OS_STATUS OS_OpenDirectoryAndReadFirstEntry(const char* directoryName, const char* fileFilter,
                                            OS_FILE_HANDLE* directory, OS_DIR_ENTRY_DESC* dirEntryDesc);
OS_STATUS OS_ReadNextDirectoryEntry(OS_FILE_HANDLE directory, OS_DIR_ENTRY_DESC* dirEntryDesc);
void      OS_CloseDirectory(OS_FILE_HANDLE directory);

#ifdef __cplusplus
}
#endif

#endif

