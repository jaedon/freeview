/***************************************************************************
 *     (c)2010-2011 Broadcom Corporation
 *  
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *   
 *  Except as expressly set forth in the Authorized License,
 *   
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *   
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *  
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 *  ANY LIMITED REMEDY.
 * 
 * $brcm_Workfile: netapp_os.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 6/14/11 10:41a $
 */
#ifndef NETAPP_OS_H
#define NETAPP_OS_H
#include <stdlib.h>
#include "netapp.h"

/**
* @ingroup TaskFunctions
* @defgroup NetApp Task API's
* @{
*/

/**
 * @emun Task Priorities
 */
typedef enum
{
    NETAPP_OS_PRIORITY_LOWEST,             //!< Lowest Priority
    NETAPP_OS_PRIORITY_LOW,                //!< Low Priority
    NETAPP_OS_PRIORITY_NORMAL,             //!< Normal Priority
    NETAPP_OS_PRIORITY_HIGH,               //!< High Priority
    NETAPP_OS_PRIORITY_HIGHEST,            //!< Highest Priority
} NETAPP_OS_PRIORITY;

/**
 * @brief Generic Function pointer with parameters
 */
typedef void (*NETAPP_OS_FUNCPTR)(void*);



/**
 * @brief Spawn a Thread with parameters
 *
 * @param[in]  pName        Task name
 * @param[in]  tPriority    Task priority
 * @param[in]  ulStackSize  Stack size
 * @param[in]  tFunction    function to run the task
 * @param[in]  pvParam      Parameter
 *
 * @return int32_t the Task ID or < 0 for  failure
 */
int32_t NetAppOSTaskSpawn(
    const char          *pName,
    NETAPP_OS_PRIORITY  tPriority,
    uint32_t            ulStackSize,
    NETAPP_OS_FUNCPTR   tFunction,
    void                *pvParam);


/**
 * @brief Delete a Thread
 *
 * @param[in]  lTaskID      Task ID
 *
 * @return void
 */
void NetAppOSTaskDelete(
    int32_t             taskId);


/**
 * @brief Exit a Thread
 *
 * @return void
 */
void  NetAppOSTaskExit();


/**
 * @brief Delay a Thread
 *
 * @param[in]  lDelayMS      How long in ms to delay,
 *
 * @return void
 */
void NetAppOSTaskDelayMsec(
    int32_t             lDelayMS);


/**
 * @brief Join a Thread
 *
 * Function will block until the passed lTaskID has exited.
 * @param[in]  lTaskID   Task ID to join
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppOSTaskJoin(
    int32_t             lTaskID);
///@}



/**
* @ingroup SemFunctions
* @defgroup NetApp Semaphore function Prototypes
* @{
*/

/**
 * @brief Create a Binary Semaphore
 *
 * @return uint32_t Semaphore handle
 */
uint32_t NetAppOSSemBCreate();


/**
 * @brief Take the Semaphore
 *
 * @param[in]  ulSemId   Semaphore handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppOSSemTake(
    uint32_t            ulSemId);


/**
 * @brief Take the Semaphore with a timeout
 *
 * @param[in]  ulSemId          Semaphore handle
 * @param[in]  ulTimeoutMsec    Timeout (ms)
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppOSSemTakeMsec(
    uint32_t            ulSemId,
    uint32_t            ulTimeoutMsec);


/**
 * @brief Give the Semaphore
 *
 * @param[in]  ulSemId  Semaphore handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppOSSemGive(
    uint32_t            ulSemId);


/**
 * @brief Delete the Semaphore
 *
 * @param[in]  ulSemId  Semaphore handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppOSSemDelete(
    uint32_t            ulSemId);
///@}


/**
* @ingroup MemFunctions
* @defgroup Memory function prototypes
* @{
*/

/**
 * @brief Allocate memory
 *
 * @param[in]  ulSize  Number of bytes to allocate
 *
 * @return void*
 */
void* NetAppOSMemAlloc(
    uint32_t            ulSize);


/**
 * @brief Free previously allocated memory
 *
 * @param[in]  pvMemory  Handle to memory to be freed
 *
 * @return NETAPP_RETCODE
 * @see NetAppOSMemAlloc
 */
NETAPP_RETCODE NetAppOSMemFree(
    void                *pvMemory);
///@}


/**
* @ingroup TimerFunctions
* @defgroup Timer function prototypes
* @{
*/

/**
 * @brief Create a Timer
 *
 * @param[out] pulTimerId   Timer handle
 * @param[in]  tFunction    Function to run the timer
 * @param[in]  pParam       Parameter to pass to the timer
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppOSTimerCreate(
    uint32_t            *pulTimerId,
    NETAPP_OS_FUNCPTR   tFunction,
    void                *pParam);

/**
 * @brief Delete a Timer
 *
 * @param[in] ulTimerId Timer handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppOSTimerDelete(
    uint32_t            ulTimerId);


/**
 * @brief Stop a Timer
 *
 * @param[in] ulTimerId Timer handle
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE   NetAppOSTimerStop(
    uint32_t            ulTimerId);


/**
 * @brief Set a Timer to repeat every msecs
 *
 * @param[in] ulTimerId Timer handle
 * @param[in] ulPeriod  timer period
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE   NetAppOSTimerSetRepeatMsec(
    uint32_t            ulTimerId,
    uint32_t            ulPeriod);


/**
 * @brief Set a Timer to to go off after ulPeriod miliseconds
 *
 * @param[in] ulTimerId Timer handle
 * @param[in] ulPeriod  timer period
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE   NetAppOSTimerSetMsec(
    uint32_t            ulTimerId,
    uint32_t            ulPeriod);


/**
 * @brief Create a Message Queue
 *
 * @param[in] ulDepth       How many messages in the queue
 * @param[in] ulMaxMsgSize  Maximum size of a message in teh message queue
 *
 * @return uint32_t
 */
uint32_t NetAppOSMsgQCreate(
    uint32_t            ulDepth,
    uint32_t            ulMaxMsgSize);


/**
 * @brief Delete the message queue
 *
 * @param[in] mqId          The queue ID
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppOSMsgQDelete(
    uint32_t            mqId);


/**
 * @brief Send a message to the queue
 *
 * @param[in] mqId          The queue ID
 * @param[in] pMsg          Point to the message buffer
 * @param[in] ulSize        size of the message buffer
 * @param[in] lTimeout      How long to wait to sent the message
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppOSMsgQSend(
    uint32_t            mqId,
    void                *pMsg,
    uint32_t            ulSize,
    int32_t             lTimeout);


/**
 * @brief Receive a message from the queue
 *
 * @param[in] mqId          The queue ID
 * @param[in] pMsg          Point to the message buffer
 * @param[in] ulSize        size of the message buffer
 * @param[in] lTimeout      How long to wait to sent the message
 *
 * @return NETAPP_RETCODE
 */
NETAPP_RETCODE NetAppOSMsgQReceive(
     uint32_t           mqId,
     void               *pMsg,
     uint32_t           ulSize,
     int32_t            lTimeout);
///@}
#endif //NETAPP_OS_H
