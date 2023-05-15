/***************************************************************************
 *    (c)2010-2011 Broadcom Corporation
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
 * $brcm_Workfile: netapp_os.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 6/15/11 5:27p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AppLibs/broadcom/netapp/netapp_os.c $
 * 
 * 2   6/15/11 5:27p steven
 * SW7425-714: Adding NetApp support to AppLibs
 * 
 * 1   6/14/11 10:41a steven
 * SW7425-714: Moved NetApp to AppLibs
 *
 ***************************************************************************/
/**
 * @brief NetApp Operating System API
 *
 * @author Steven Hartley (steven@broadcom.com)
 *
 * Thin API used by NetApp to hook into osapi  for i386 or mipsel (BD/DTV)
 *
 */
#include "netapp_os.h"
#include "osapi.h"

/* Macro to convert OSAPI to NetApp nomenclature */
#define NETAPP_OSAPI_CHECK(api)                                                 \
{                                                                               \
    NETAPP_RETCODE status;                                                      \
    switch (api)                                                                \
    {                                                                           \
    case OS_OK:                                                                 \
        status = NETAPP_SUCCESS;                                                \
        break;                                                                  \
    case OS_NULL_POINTER:                                                       \
        status = NETAPP_NULL_PTR;                                               \
        break;                                                                  \
    case OS_NOT_IMPLEMENTED:                                                    \
        status = NETAPP_NOT_IMPLEMENTED;                                        \
        break;                                                                  \
    case OS_NO_MEMORY:                                                          \
    case OS_INVALID_MEMPOOL:                                                    \
        status = NETAPP_OUT_OF_MEMORY;                                          \
        break;                                                                  \
    case OS_TIMEOUT:                                                            \
        status = NETAPP_TIMEOUT;                                                \
        break;                                                                  \
    case OS_FAILURE:                                                            \
    default:                                                                    \
        status = NETAPP_FAILURE;                                                \
    }                                                                           \
    return status;                                                              \
}

#define OS_TASK_MIN_PRIORITY     OS_THREAD_MIN_PRIORITY
#define OS_TASK_MAX_PRIORITY     OS_THREAD_MAX_PRIORITY
#define OS_TASK_LOW_PRIORITY     OS_THREAD_LOW_PRIORITY
#define OS_TASK_NORMAL_PRIORITY  OS_THREAD_NORMAL_PRIORITY
#define OS_TASK_RAISED_PRIORITY  OS_THREAD_RAISED_PRIORITY
#define OS_TASK_HIGH_PRIORITY    OS_THREAD_HIGH_PRIORITY
#define OS_TASK_NAME_SIZE        OS_THREAD_NAME_SIZE

#define OS_TaskSpawnParam       OS_ThreadSpawnParam
#define OS_TaskExit             OS_ThreadExit
#define OS_TaskDelayMsec        OS_ThreadDelayMsec
#define OS_TaskDelete           OS_ThreadDelete
#define OS_TaskJoin             OS_ThreadJoin

int32_t NetAppOSTaskSpawn(
    const char          *pName,
    NETAPP_OS_PRIORITY  tPriority,
    uint32_t            ulStackSize,
    NETAPP_OS_FUNCPTR   tFunction,
    void                *pvParam)
{
    int32_t lPrio;
    switch(tPriority)
    {
    case NETAPP_OS_PRIORITY_LOWEST:
        lPrio = OS_TASK_MIN_PRIORITY;
        break;
    case NETAPP_OS_PRIORITY_LOW:
        lPrio = OS_TASK_LOW_PRIORITY;
        break;
    case NETAPP_OS_PRIORITY_HIGH:
        lPrio = OS_TASK_RAISED_PRIORITY;
        break;
    case NETAPP_OS_PRIORITY_HIGHEST:
        lPrio = OS_TASK_HIGH_PRIORITY;
        break;
    case NETAPP_OS_PRIORITY_NORMAL:
    default:
        lPrio = OS_TASK_NORMAL_PRIORITY;
        break;
    }
    return OS_TaskSpawnParam(pName, lPrio, ulStackSize, (OS_FUNCPTR_PARAM)tFunction, pvParam, NULL);
}


void NetAppOSTaskDelete(
   int32_t          ulTaskId)
{
    OS_TaskDelete(ulTaskId);
}


void NetAppOSTaskExit(void)
{
    OS_TaskExit();
}


void NetAppOSTaskDelayMsec(
    int32_t             lDelayMS)
{
    OS_TaskDelayMsec(lDelayMS);
}


NETAPP_RETCODE NetAppOSTaskJoin(int32_t lTaskID)
    NETAPP_OSAPI_CHECK(OS_TaskJoin(lTaskID));


uint32_t NetAppOSSemBCreate(void)
{
    return OS_SemBCreate(OS_SEM_Q_FIFO, OS_SEM_FULL);
}


NETAPP_RETCODE NetAppOSSemTake(uint32_t ulSemId)
    NETAPP_OSAPI_CHECK(OS_SemTake(ulSemId, -1));


NETAPP_RETCODE NetAppOSSemTakeMsec(
    uint32_t            ulSemId,
    uint32_t            ulTimeoutMsec)
    NETAPP_OSAPI_CHECK(OS_SemTakeMsec(ulSemId, ulTimeoutMsec));


NETAPP_RETCODE NetAppOSSemGive(uint32_t ulSemId)
    NETAPP_OSAPI_CHECK(OS_SemGive(ulSemId));


NETAPP_RETCODE NetAppOSSemDelete(uint32_t ulSemId)
    NETAPP_OSAPI_CHECK(OS_SemDelete(ulSemId));


void* NetAppOSMemAlloc(uint32_t ulSize)
{
    return OS_MemAlloc(ulSize);
}


NETAPP_RETCODE NetAppOSMemFree(void *pvMemory)
    NETAPP_OSAPI_CHECK(OS_MemFree(pvMemory));


NETAPP_RETCODE NetAppOSTimerCreate(
    uint32_t            *pulTimerId,
    NETAPP_OS_FUNCPTR   tFunction,
    void                *pParam)
    NETAPP_OSAPI_CHECK(OS_TimerCreateParam(pulTimerId, (OS_FUNCPTR_PARAM)tFunction, pParam));


NETAPP_RETCODE NetAppOSTimerDelete(uint32_t ulTimerId)
    NETAPP_OSAPI_CHECK(OS_TimerDelete(ulTimerId));


NETAPP_RETCODE NetAppOSTimerStop(uint32_t ulTimerId)
    NETAPP_OSAPI_CHECK(OS_TimerStop(ulTimerId));


NETAPP_RETCODE NetAppOSTimerSetRepeatMsec(uint32_t ulTimerId, uint32_t ulPeriod)
    NETAPP_OSAPI_CHECK(OS_TimerSetRepeatMsec(ulTimerId, ulPeriod));


NETAPP_RETCODE   NetAppOSTimerSetMsec(uint32_t  ulTimerId, uint32_t ulPeriod)
    NETAPP_OSAPI_CHECK(OS_TimerSetMsec(ulTimerId, ulPeriod));


uint32_t NetAppOSMsgQCreate(uint32_t ulDepth, uint32_t ulMaxMsgSize)
{
    return OS_MsgQCreate(ulDepth, ulMaxMsgSize, OS_MSG_Q_FIFO);
}


NETAPP_RETCODE NetAppOSMsgQDelete(uint32_t mqId)
    NETAPP_OSAPI_CHECK(OS_MsgQDelete(mqId));


NETAPP_RETCODE NetAppOSMsgQSend(uint32_t mqId, void *pMsg, uint32_t ulSize, int32_t lTimeout)
    NETAPP_OSAPI_CHECK(OS_MsgQSend(mqId, pMsg, ulSize, lTimeout, 0));


NETAPP_RETCODE NetAppOSMsgQReceive(uint32_t mqId, void *pMsg, uint32_t ulSize, int32_t lTimeout)
    NETAPP_OSAPI_CHECK(OS_MsgQReceive(mqId, pMsg, ulSize, lTimeout));

