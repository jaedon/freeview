/***************************************************************************
*     (c)2003-2013 Broadcom Corporation
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
*
* Module Description:
*    Broadcom DAL level API provided to allow the Application to associate CA descramble
*    PIDS with Nagra Transport Session ID's. This module must be called from the CAK porting
*    level and is necessary due to NAGRA SEC 5.1.1 changes which no longer provide PID information
*    to CA descramble sessions (secOpenStreamDecryptSession). Typical usage would be for the CAK
*    porting layer to create a descramble session request CA_REQUEST_TYPE_PROGRAM_DESCRAMBLING
*    for a TSID then call this API to associate the service PIDS with the TSID associated
*    with the CA_REQUEST_TYPE_PROGRAM_DESCRAMBLING request. The SEC decrypt session will then need
*    to call the get function within this module in order to find the PIDS associated with the
*    descramble sessions TSID. To make matters slightly more complex this module must also update the SEC
*    descramble session PIDS on the fly and deal with the race conditions associated with this
*    architecture: i.e 1. A StreamDecryptSession may be created and keys set before this module
*    has been notified of the PIDS 2. A PID associated with an existing StreamDecryptSession may change.
*    In order to deal with the above the module needs both a getter and notification mechanism for
*    intrested client.
*	Compatibility NOCS3.0
*	Compatibility NASC1.5
*
* Revision History:
*
*  31/05/13 - Version 1.0
*   Initial version internally tested with DALTS Version 3.12.0
* 	Based on DALAPI Version 7.1.0
*
*
***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "nocs_csd_impl.h"
#include "nocs_csd.h"
#include "brcm_ca_session_manager_internal.h"
#include "brcm_ca_session_manager_external.h"
#include "bdbg.h"

BDBG_MODULE(BRCM_SESSION_MANAGER);

#define MAX_NUMBER_OF_PIDS_PER_TSID 6
#define MAX_NUMBER_TS_CONTEXTS 5 /* 4 records + 1 Live/Play should be enough*/


/* transport session context structure */
typedef struct SessionManagerTSContext   *TSContextNode;

typedef struct SessionManagerTSContext
{
    TTransportSessionId   xTSid; /* Transport Session ID */
    NEXUS_PlaypumpHandle playpumpHandle;
    BrcmCaStreamSessionKeyHandleCallback streamSessionKeyHandleCallback;
    BrcmCaCipherSessionKeyLoadedCallback cipherSessionKeyLoadedCallback;
    NEXUS_KeySlotHandle   m2mKeyHandle; /* handle of keyslot */
    TUnsignedInt16 pPidList[MAX_NUMBER_OF_PIDS_PER_TSID]; /* Pid list */
    unsigned int xNumberOfPids; /* number of pids */
    BrcmCaPidChangedCallback pidChangeCallback;
    BrcmCaIvChangedCallback ivChangeCallback;
    TUnsignedInt8 xIv[16];
    size_t xIvSize;
    TSessionManagerSessionType xSessionType;
    struct SCsdScrKeyPathHandle* pScrKeyPath;
    TSContextNode	 next;
} SessionManagerTSContext;

SessionManagerTSContext *gSessionManagerTSContextHead = 0;
unsigned int gNumberTSContexts = 0;

BKNI_MutexHandle        gListMutexLock = 0;




/**
 *  @brief
 *    This function add a TSID context to the PID manager
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *
 *  @param[in]  playpumpHandle
 *                Playpump handle if this is a playback
 *
 *  @param[in]  streamSessionKeyHandleCallback
 *                callback which will be called when a stream
 *                session M2M keyhandle is allocated.
 *
 *  @param[in]  cipherSessionKeyLoadedCallback
 *                callback which will be called when a cipher
 *                session key is loaded.
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaAddTransportSessionContext
(
    TTransportSessionId   xTSid,
    NEXUS_PlaypumpHandle playpumpHandle,
    BrcmCaStreamSessionKeyHandleCallback streamSessionKeyHandleCallback,
    BrcmCaCipherSessionKeyLoadedCallback cipherSessionKeyLoadedCallback
)
{
    BDBG_MSG(("%s: xTSid = %d gNumberTSContexts=%d", __FUNCTION__, xTSid, gNumberTSContexts));
    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(!gListMutexLock) {
        if(BKNI_CreateMutex( &gListMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SESSION_MANAGER_ERROR_BAD_PARAMETER;
        }
    }

    /* check if there are any TSID contexts if not create one for xTSid*/
    /* take list lock */
    BKNI_AcquireMutex(gListMutexLock);

    if(gNumberTSContexts>=MAX_NUMBER_TS_CONTEXTS) {
        BDBG_ERR( ("Reached maximum number of TSContextx (%d)",gNumberTSContexts) );
        BKNI_ReleaseMutex(gListMutexLock);
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(!gSessionManagerTSContextHead) {
        /* first dsc instance */
        gSessionManagerTSContextHead=BKNI_Malloc(sizeof(SessionManagerTSContext));
        if(gSessionManagerTSContextHead == NULL)
        {
            BDBG_ERR(("%s Fail - out of memory", __FUNCTION__));
            BKNI_ReleaseMutex(gListMutexLock);
            return SESSION_MANAGER_ERROR;
        }

        BKNI_Memset(gSessionManagerTSContextHead, 0, sizeof(SessionManagerTSContext));
        gSessionManagerTSContextHead->xTSid=xTSid;
        gSessionManagerTSContextHead->playpumpHandle=playpumpHandle;
        gSessionManagerTSContextHead->streamSessionKeyHandleCallback=streamSessionKeyHandleCallback;
        gSessionManagerTSContextHead->cipherSessionKeyLoadedCallback=cipherSessionKeyLoadedCallback;
        gSessionManagerTSContextHead->xIvSize = 0;
        gSessionManagerTSContextHead->xSessionType = SESSION_TYPE_UNDEFINED;
    } else {
        /* check if there is already a node for TSID */
        /* create a new node*/
        SessionManagerTSContext *node=0;
        SessionManagerTSContext *traverse=0;
        node=BKNI_Malloc(sizeof(SessionManagerTSContext));
        if(node == NULL)
        {
            BDBG_ERR(("%s Fail - out of memory", __FUNCTION__));
            BKNI_ReleaseMutex(gListMutexLock);
            return SESSION_MANAGER_ERROR;
        }
        BKNI_Memset(node, 0, sizeof(SessionManagerTSContext));
        node->xTSid=xTSid;
        node->playpumpHandle=playpumpHandle;
        node->streamSessionKeyHandleCallback=streamSessionKeyHandleCallback;
        node->cipherSessionKeyLoadedCallback=cipherSessionKeyLoadedCallback;
        node->xIvSize = 0;
        node->xSessionType = SESSION_TYPE_UNDEFINED;
        /* traverse list */
        traverse = gSessionManagerTSContextHead;
        while(traverse) {
            if(traverse->xTSid == xTSid) {
                /*the xTSid already exists in the list error*/
                BDBG_ERR(("%s xTSid already exists in the list error", __FUNCTION__));
                BKNI_ReleaseMutex(gListMutexLock);
                BKNI_Free(node);
                return SESSION_MANAGER_ERROR;
            }
            if(!traverse->next) {
                /* found next free */
                traverse->next = node;
                traverse = 0;
                continue;
            }
            traverse = traverse->next;
        }
    }
    gNumberTSContexts++;
    BKNI_ReleaseMutex(gListMutexLock);

    return SESSION_MANAGER_NO_ERROR;
}

/**
 *  @brief
 *    This function gets a M2M keyslot handle associated with
 *    xTSid so that the application can decrypt data blocks
 *    using the Sec configured keyslot. It is assumed that the
 *    applicagtion will be aware a DAL Encrypt/Decrypt session
 *    has been created before this fiunction is called otherwise
 *    the function will return an invalid keyHandle.
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *
 *  @param[out]  pM2mKeyHandle
 *                pointer to NEXUS_KeySlotHandle
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaGetM2MKeyslotHandle
(
    TTransportSessionId   xTSid,
    NEXUS_KeySlotHandle   *pM2mKeyHandle /* handle of keyslot */
)
{
    SessionManagerTSContext *traverse=0;

    BDBG_MSG(("%s: xTSid = %d", __FUNCTION__, xTSid));
    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(!gListMutexLock) {
        if(BKNI_CreateMutex( &gListMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SESSION_MANAGER_ERROR;
        }
    }

    /* check if TSID context exists else create one */
    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid) {
            if(!traverse->m2mKeyHandle) {
                /* keyslot handle has not yet been allocated in SEC*/
                *pM2mKeyHandle = 0;
                BKNI_ReleaseMutex(gListMutexLock);
                BDBG_MSG(("%s failed to get M2M keyslot handle for xTSid %d", __FUNCTION__, xTSid));
                return SESSION_MANAGER_ERROR;
            } else {
                *pM2mKeyHandle = traverse->m2mKeyHandle;
                BKNI_ReleaseMutex(gListMutexLock);
                BDBG_MSG(("%s got M2M keyslot handle %d for xTSid %d", __FUNCTION__, *pM2mKeyHandle, xTSid));
                return SESSION_MANAGER_NO_ERROR;
            }
        }
        traverse = traverse->next;
    }

    BKNI_ReleaseMutex(gListMutexLock);
    BDBG_MSG(("%s did not find node for tsid %d", __FUNCTION__, xTSid));
    return SESSION_MANAGER_ERROR;
}

/**
 *  @brief
 *    This function removes the TSID context and all Pids. Pid
 *    notification function will NOT be called if any Pids
 *    remain on this Context
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaRemoveTransportSessionContext
(
    TTransportSessionId   xTSid
)
{
    BDBG_MSG(("%s: xTSid = %d gNumberTSContexts=%d", __FUNCTION__, xTSid, gNumberTSContexts));
    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(!gListMutexLock) {
        BDBG_ERR(("%s: !gListMutexLock xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    /* check if TSID context exists if not error*/
    BKNI_AcquireMutex(gListMutexLock);
    if(!gSessionManagerTSContextHead) {
        BDBG_ERR(("%s: xTSid = %d does not exist", __FUNCTION__, xTSid));
        BKNI_ReleaseMutex(gListMutexLock);
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    } else /* traverse list */
    {
        SessionManagerTSContext *traverse=gSessionManagerTSContextHead;
        SessionManagerTSContext *prev=gSessionManagerTSContextHead;

        while(traverse) {
            if(traverse->xTSid==xTSid) {
                if(traverse==gSessionManagerTSContextHead) {
                    gSessionManagerTSContextHead = traverse->next;
                    BKNI_Free(traverse);
                    BDBG_MSG(("%s: removed head xTSid = %d", __FUNCTION__, xTSid));
                    gNumberTSContexts--;
                    BKNI_ReleaseMutex(gListMutexLock);
                    return SESSION_MANAGER_NO_ERROR;
                } else {
                    prev->next = traverse->next;
                    BKNI_Free(traverse);
                    BDBG_MSG(("%s: removed xTSid = %d", __FUNCTION__, xTSid));
                    gNumberTSContexts--;
                    BKNI_ReleaseMutex(gListMutexLock);
                    return SESSION_MANAGER_NO_ERROR;
                }
            }
            prev = traverse;
            traverse = traverse->next;
        }
    }
    BKNI_ReleaseMutex(gListMutexLock);
    BDBG_MSG(("%s: failed to remove xTSid = %d", __FUNCTION__, xTSid));
    return SESSION_MANAGER_ERROR;
}


/**
 *  @brief
 *    This function add a PID to TSID context
 *    TSID must have been created in call to
 *    brcmCaAddTransportSessionContext
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *
 *  @param[in]  xPid
 *                CA descramble PID
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaAddPidToTransportSessionId
(
    TTransportSessionId   xTSid,
    TUnsignedInt16 xPid
)
{
    unsigned int i=0;
    SessionManagerTSContext *traverse=0;
    BDBG_MSG(("%s: xTSid = %d", __FUNCTION__, xTSid));
    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if((xPid == BRCM_NULL_PID) || (xPid == BRCM_INVALID_PID)) {
        BDBG_ERR(("%s: Invalid xPid = %d", __FUNCTION__, xPid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(!gListMutexLock) {
        if(BKNI_CreateMutex( &gListMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SESSION_MANAGER_ERROR;
        }
    }

    /* if there is already a context just add xPid to the context */
    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid) {
            /* if PID already exists fail */
            for (i=0; i<traverse->xNumberOfPids; i++) {
                if(traverse->pPidList[i] == xPid) {
                    /* pid already in list */
                    BDBG_ERR( ("pid 0x%x already in list for TSID %d",xPid, xTSid) );
                    BKNI_ReleaseMutex(gListMutexLock);
                    return SESSION_MANAGER_ERROR;
                }
            }

            if(traverse->xNumberOfPids >= MAX_NUMBER_OF_PIDS_PER_TSID) {
                /* pid already in list */
                BDBG_ERR( ("already at max number of pids for pid 0x%x TSID %d",xPid, xTSid) );
                BKNI_ReleaseMutex(gListMutexLock);
                return SESSION_MANAGER_ERROR;
            }

            /* found the TSID so add a pid and make a callback if registerd */

            traverse->pPidList[traverse->xNumberOfPids] = xPid;
            traverse->xNumberOfPids++;
            BDBG_MSG(("Added pid 0x%x to TSID %d xNumberOfPids=%d",xPid,xTSid, traverse->xNumberOfPids));
            for (i=0; i<traverse->xNumberOfPids; i++) {
                BDBG_MSG(("TSID %d owns 0x%x", xTSid, traverse->pPidList[i]));
            }
            /* call callback if it exists */
            if(traverse->pidChangeCallback) {
                BDBG_MSG(("Calling pidChangeCallback"));
                traverse->pidChangeCallback(traverse->xTSid, xPid,SESSION_MANAGER_PID_ADDED);
                BDBG_MSG(("done Calling pidChangeCallback"));
            }

            break;
        }
        traverse = traverse->next;
    }

    if(!traverse) {
        BDBG_ERR( ("xTSid does not exit") );
        BKNI_ReleaseMutex(gListMutexLock);
        return SESSION_MANAGER_ERROR;
    }

    BKNI_ReleaseMutex(gListMutexLock);
    return SESSION_MANAGER_NO_ERROR;
}


/**
 *  @brief
 *    This function associates an IV value with a transport session
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *
 *  @param[in]  pxIv
 *                A pointer to the IV buffer
 *
 *  @param[in]  xIvSize
 *                The size of the IV
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaAssociateIvToTransportSessionId
(
    TTransportSessionId   xTSid, 
    const TUnsignedInt8  *pxIv,
    size_t                xIvSize
)
{
    SessionManagerTSContext *traverse=0;
    BDBG_MSG(("%s: pxIv = %p, xIvSize = %d", __FUNCTION__, pxIv, xIvSize));

    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if((pxIv == NULL) || (xIvSize == 0) || (xIvSize > 16)) {
        BDBG_ERR(("%s: Invalid parameters", __FUNCTION__));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(!gListMutexLock) {
        if(BKNI_CreateMutex( &gListMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SESSION_MANAGER_ERROR;
        }
    }

    /* if there is already a context just add the IV to the context */
    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid)
            break;
        traverse = traverse->next;
    }

    if(!traverse) {
        BDBG_ERR( ("xTSid does not exit") );
        BKNI_ReleaseMutex(gListMutexLock);
        return SESSION_MANAGER_ERROR;
    }

    /* Set the new IV */
    BKNI_Memcpy(traverse->xIv, pxIv, xIvSize);
    traverse->xIvSize = xIvSize;

    /* call callback if it exists */
    if(traverse->ivChangeCallback) {
        BDBG_MSG(("Calling ivChangeCallback"));
        traverse->ivChangeCallback(traverse->xTSid, pxIv, xIvSize);
        BDBG_MSG(("done Calling ivChangeCallback"));
    }

    BKNI_ReleaseMutex(gListMutexLock);
    return SESSION_MANAGER_NO_ERROR;
}



/**
 *  @brief
 *    This function removes a CA descramble PID from a TSID
 *    context
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *
 *  @param[in]  xPid
 *                CA descramble PID
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaRemovePidFromTransportSessionId
(
    TTransportSessionId   xTSid,
    TUnsignedInt16 xPid
)
{
    unsigned int i=0;
    SessionManagerTSContext *traverse=0;
    BDBG_MSG(("%s: xTSid = %d", __FUNCTION__, xTSid));
    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if((xPid == BRCM_NULL_PID) || (xPid == BRCM_INVALID_PID)) {
        BDBG_ERR(("%s: Invalid xPid = %d", __FUNCTION__, xPid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(!gListMutexLock) {
        BDBG_ERR( ("BKNI_CreateMutex error") );
        return SESSION_MANAGER_ERROR;
    }

    if(!gSessionManagerTSContextHead) {
        BDBG_ERR( ("xTSid %d not valid", xTSid) );
        return SESSION_MANAGER_ERROR;
    }

    /* if there is already a context remove xPid and call the callback */
    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while(traverse) {
        if(traverse->xTSid == xTSid) {
            /* found xTSid now check to see if pid exists in list*/
            if(traverse->xNumberOfPids == 0) {
                BDBG_ERR( ("xPid 0x%x not in xTSid %d",xPid , xTSid) );
                BKNI_ReleaseMutex(gListMutexLock);
                return SESSION_MANAGER_ERROR;
            }
            for(i=0; i<traverse->xNumberOfPids; i++) {
                /* find the pid*/
                if(traverse->pPidList[i] == xPid) {
                    /* found pid */
                    BDBG_MSG( ("Found xPid 0x%x in xTSid %d",xPid , xTSid) );
                    /* Remove it */
                    traverse->pPidList[i] = traverse->pPidList[--traverse->xNumberOfPids];
                    /* call callback if it exists */
                    if(traverse->pidChangeCallback) {
                        BDBG_MSG(("Calling pidChangeCallback"));
                        traverse->pidChangeCallback(traverse->xTSid, xPid,SESSION_MANAGER_PID_REMOVED);
                        BDBG_MSG(("done Calling pidChangeCallback"));
                    }
                    BKNI_ReleaseMutex(gListMutexLock);
                    return SESSION_MANAGER_NO_ERROR;
                }
            }
        }
        traverse = traverse->next;
    }

    if(!traverse) {
        /* we didnt find xTSid*/
        BDBG_ERR( ("xTSid %d not found", xTSid) );
        BKNI_ReleaseMutex(gListMutexLock);
        return SESSION_MANAGER_ERROR;
    }

    /* call any registerd callbacks for xTSid*/
    BKNI_ReleaseMutex(gListMutexLock);
    return SESSION_MANAGER_NO_ERROR;
}


/**
 *  @brief
 *    This function sets whether the specified transport session
 *    is going to be used for DSC or SCR operations. It can only
 *    be set once.
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *  
 *  @param[in]  xSessionType
 *                SESSION_TYPE_DSC, or SESSION_TYPE_SCR
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaSetTransportSessionType
(
    TTransportSessionId        xTSid,
    TSessionManagerSessionType xSessionType
)
{
    SessionManagerTSContext *traverse=0;
    BDBG_MSG(("Enter %s", __FUNCTION__));

    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if((xSessionType != SESSION_TYPE_DSC) && (xSessionType != SESSION_TYPE_SCR)) {
        BDBG_ERR(("%s: Invalid parameters", __FUNCTION__));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(!gListMutexLock) {
        if(BKNI_CreateMutex( &gListMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SESSION_MANAGER_ERROR;
        }
    }

    /* if there is already a context just add the IV to the context */
    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid)
            break;
        traverse = traverse->next;
    }

    if(!traverse) {
        BDBG_ERR( ("xTSid does not exit") );
        BKNI_ReleaseMutex(gListMutexLock);
        return SESSION_MANAGER_ERROR;
    }

    /* Already set? */
    if (traverse->xSessionType != SESSION_TYPE_UNDEFINED)
    {
        BDBG_ERR( ("Session type already set") );
        BKNI_ReleaseMutex(gListMutexLock);
        return SESSION_MANAGER_ERROR;
    }

    /* Set the type */
    traverse->xSessionType = xSessionType;

    BKNI_ReleaseMutex(gListMutexLock);
    return SESSION_MANAGER_NO_ERROR;
}


/**********************************Below functions are for internal DAL only ******************************/
/*********************************************************************************************************/


/**
 *  @brief
 *    This function associates a M2M keyslot handle with xTSid
 *    so that the application can decrypt data blocks using the
 *    Sec configured keyslot
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *
 *  @param[in]  m2mKeyHandle
 *                NEXUS_KeySlotHandle
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaAssociateM2MKeyslotHandle
(
    TTransportSessionId   xTSid,
    NEXUS_KeySlotHandle   m2mKeyHandle /* handle of keyslot */
)
{
    SessionManagerTSContext *traverse=0;

    BDBG_MSG(("%s: xTSid = %d", __FUNCTION__, xTSid));
    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(!gListMutexLock) {
        if(BKNI_CreateMutex( &gListMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SESSION_MANAGER_ERROR;
        }
    }

    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid) {
            traverse->m2mKeyHandle = m2mKeyHandle;
            if(traverse->streamSessionKeyHandleCallback) {
                BDBG_MSG(("%s calling streamSessionKeyHandleCallback for xTSid %d", __FUNCTION__, xTSid));
                traverse->streamSessionKeyHandleCallback(xTSid, m2mKeyHandle);
            }

            BKNI_ReleaseMutex(gListMutexLock);
            BDBG_MSG(("%s added M2M keyslot handle %d to xTSid %d", __FUNCTION__, m2mKeyHandle, xTSid));
            return SESSION_MANAGER_NO_ERROR;
        }
        traverse = traverse->next;
    }

    BKNI_ReleaseMutex(gListMutexLock);
    BDBG_MSG(("%s did not find node", __FUNCTION__));
    return SESSION_MANAGER_ERROR;
}


/**
 *  @brief
 *    This function registers a callback function which will be
 *    called when PID/s change on xTSid. The function returns
 *    any PIDs curently associated with the TSID context and
 *    will NOT call pidChangedCallback in the context of this
 *    call
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *
 *  @param[in]  pidChangedCallback
 *                pointer to function which will be call every
 *                time pids change on xTSid
 *
 *  @param[out]  pPidList
 *                xNumberOfPids list of CA descramble PIDs -
 *                memory for pPidList is allocated inside this
 *                function and must be freed by the caller.
 *
 *  @param[out]  xNumberOfPids
 *                number of pids in pPidList
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaRegisterTSIdPidNotificationCallback
(
    TTransportSessionId   xTSid,
    BrcmCaPidChangedCallback pidChangedCallback,
    TUnsignedInt16 **pPidList,
    unsigned int *xNumberOfPids
)
{
    SessionManagerTSContext *traverse=0;

    BDBG_MSG(("%s: xTSid = %d", __FUNCTION__, xTSid));
    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }
    if(!pidChangedCallback) {
        BDBG_ERR(("%s: Invalid pidChangedCallback", __FUNCTION__));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }
    if(!pPidList) {
        BDBG_ERR(("%s: Invalid pPidList pointer", __FUNCTION__));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }
    if(!xNumberOfPids) {
        BDBG_ERR(("%s: Invalid xNumberOfPids pointer", __FUNCTION__));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(!gListMutexLock) {
        if(BKNI_CreateMutex( &gListMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SESSION_MANAGER_ERROR;
        }
    }

    /* check if TSID context exists else create one */
    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid) {
            break;
        }
        traverse = traverse->next;
    }
    if(!traverse) {
        BDBG_MSG(("%s: NO context for xTSid %d", __FUNCTION__, xTSid));
        BKNI_ReleaseMutex(gListMutexLock);
        BDBG_MSG(("%s exit", __FUNCTION__));
        return SESSION_MANAGER_ERROR;
    }
    /* save the callback */
    traverse->pidChangeCallback = pidChangedCallback;

    /* if we have any pids registerd allocate some memory and copy the PID's in, update the *xNumberOfPids*/
    if(traverse->xNumberOfPids) {

        *pPidList=BKNI_Malloc(sizeof(TUnsignedInt16)*traverse->xNumberOfPids);
        BKNI_Memcpy(*pPidList, traverse->pPidList, sizeof(TUnsignedInt16)*traverse->xNumberOfPids);
        *xNumberOfPids = traverse->xNumberOfPids;
        BDBG_MSG(("%s copied %d pids", __FUNCTION__, *xNumberOfPids));
    } else {
        /* Ensure we reutrn NULL/0 if there are no PIDs */
        *pPidList=NULL;
        *xNumberOfPids = 0;
    }
    

    BKNI_ReleaseMutex(gListMutexLock);
    BDBG_MSG(("%s exit", __FUNCTION__));
    return SESSION_MANAGER_NO_ERROR;
}

/**
 *  @brief
 *    This function de-registers a callback function associated
 *    with xTSid
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *
 *  @param[in]  pidChangedCallback
 *                pointer to function which will be removed from
 *                the notification list.
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaDeRegisterTSIdPidNotificationCallback
(
    TTransportSessionId   xTSid,
    BrcmCaPidChangedCallback pidChangedCallback
)
{
    SessionManagerTSContext *traverse=0;
    BDBG_MSG(("%s: xTSid = %d", __FUNCTION__, xTSid));
    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }
    if(!pidChangedCallback) {
        BDBG_ERR(("%s: Invalid pidChangedCallback", __FUNCTION__));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }
    if(!gListMutexLock) {
        BDBG_ERR(("%s: !gListMutexLock xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }
    /* check if TSID context exists */
    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid) {
            /* remove the callback */
            traverse->pidChangeCallback = 0;
            break;
        }
        traverse = traverse->next;
    }
    BKNI_ReleaseMutex(gListMutexLock);
    return SESSION_MANAGER_NO_ERROR;
}

/**
 *  @brief
 *    This function gets a playpump handle is one has been set
 *    on this Transport session
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *
 *  @param[out]  playpumpHandle
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaGetPlaypumpHandle
(
    TTransportSessionId   xTSid,
    NEXUS_PlaypumpHandle *pPlaypumpHandle
)
{
    SessionManagerTSContext *traverse=0;

    BDBG_MSG(("%s: xTSid = %d", __FUNCTION__, xTSid));
    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(!gListMutexLock) {
        if(BKNI_CreateMutex( &gListMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SESSION_MANAGER_ERROR;
        }
    }

    /* check if TSID context exists else create one */
    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid) {
            *pPlaypumpHandle = traverse->playpumpHandle;
            BKNI_ReleaseMutex(gListMutexLock);
            BDBG_MSG(("%s found node", __FUNCTION__));
            return SESSION_MANAGER_NO_ERROR;
        }
        traverse = traverse->next;
    }

    BKNI_ReleaseMutex(gListMutexLock);
    BDBG_MSG(("%s did not find node", __FUNCTION__));
    return SESSION_MANAGER_ERROR;
}

/**
 *  @brief
 *    This function sets a playpump handle
 *    on this Transport session
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *
 *  @param[in]  playpumpHandle
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaSetPlaypumpHandle
(
    TTransportSessionId   xTSid,
    NEXUS_PlaypumpHandle  xPlaypumpHandle
)
{
    SessionManagerTSContext *traverse=0;

    BDBG_MSG(("%s: xTSid = %d", __FUNCTION__, xTSid));
    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(!gListMutexLock) {
        if(BKNI_CreateMutex( &gListMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SESSION_MANAGER_ERROR;
        }
    }

    /* check if TSID context exists else create one */
    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid) {
            traverse->playpumpHandle = xPlaypumpHandle;
            BKNI_ReleaseMutex(gListMutexLock);
            BDBG_MSG(("%s found node", __FUNCTION__));
            return SESSION_MANAGER_NO_ERROR;
        }
        traverse = traverse->next;
    }

    BKNI_ReleaseMutex(gListMutexLock);
    BDBG_MSG(("%s did not find node", __FUNCTION__));
    return SESSION_MANAGER_ERROR;
}



/**
 *  @brief
 *    This function notifies the application that a key has been
 *    loaded for a Cipher Session associated with xTSid. Its
 *    main purpose is to allow the application to decrypt data
 *    blocks using the Sec configured keyslot after the first IV
 *    has been loaded.
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaNotifyCipherSessionKeyLoaded
(
    TTransportSessionId   xTSid
)
{
    SessionManagerTSContext *traverse=0;

    BDBG_MSG(("%s: xTSid = %d", __FUNCTION__, xTSid));
    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(!gListMutexLock) {
        if(BKNI_CreateMutex( &gListMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SESSION_MANAGER_ERROR;
        }
    }

    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid) {
            if(traverse->cipherSessionKeyLoadedCallback) {
                BDBG_MSG(("%s calling cipherSessionKeyLoadedCallback for xTSid %d", __FUNCTION__, xTSid));
                traverse->cipherSessionKeyLoadedCallback(xTSid, traverse->m2mKeyHandle);
            }

            BKNI_ReleaseMutex(gListMutexLock);
            return SESSION_MANAGER_NO_ERROR;
        }
        traverse = traverse->next;
    }

    BKNI_ReleaseMutex(gListMutexLock);
    BDBG_MSG(("%s did not find node", __FUNCTION__));
    return SESSION_MANAGER_ERROR;
}



/**
 *  @brief
 *    This function registers a callback function which will be
 *    called when the IV changes on xTSid.
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *
 *  @param[in]  pidChangedCallback
 *                pointer to function which will be call every
 *                time pids change on xTSid
 *  
 *  @param[out]  ppxIv
 *                A buffer containing the current IV (or NULL
 *                if no IV is set). Memory for pxIv is allocated
 *                inside this function and must be freed by the
 *                caller.
 * 
 *  @param[out]  pxIvSize
 *                The size of the IV (or 0 if no IV)
 * 
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaRegisterTSIdIvNotificationCallback
(
    TTransportSessionId   xTSid, 
    BrcmCaIvChangedCallback ivChangedCallback,
    TUnsignedInt8 **ppxIv, 
    unsigned int *pxIvSize
)
{
    SessionManagerTSContext *traverse=0;

    BDBG_MSG(("%s: xTSid = %d", __FUNCTION__, xTSid));

    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }
    if(!ivChangedCallback) {
        BDBG_ERR(("%s: Invalid pidChangedCallback", __FUNCTION__));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }
    if(!ppxIv) {
        BDBG_ERR(("%s: Invalid ppxIv pointer", __FUNCTION__));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }
    if(!pxIvSize) {
        BDBG_ERR(("%s: Invalid pxIvSize pointer", __FUNCTION__));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(!gListMutexLock) {
        if(BKNI_CreateMutex( &gListMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SESSION_MANAGER_ERROR;
        }
    }

    /* check if TSID context exists */
    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid) {
            break;
        }
        traverse = traverse->next;
    }
    if(!traverse) {
        BDBG_MSG(("%s: NO context for xTSid %d", __FUNCTION__, xTSid));
        BKNI_ReleaseMutex(gListMutexLock);
        BDBG_MSG(("%s exit", __FUNCTION__));
        return SESSION_MANAGER_ERROR;
    }
    /* save the callback */
    traverse->ivChangeCallback = ivChangedCallback;

    /* if we have an IV registered allocate some memory and copy it in */
    if(traverse->xIvSize) {

        *ppxIv=BKNI_Malloc(traverse->xIvSize);
        BKNI_Memcpy(*ppxIv, traverse->xIv, traverse->xIvSize);
        *pxIvSize = traverse->xIvSize;
    } else {
        /* Ensure we return NULL/0 if there is no IV */
        *ppxIv = NULL;
        *pxIvSize = 0;
    }

    BKNI_ReleaseMutex(gListMutexLock);
    BDBG_MSG(("%s exit", __FUNCTION__));
    return SESSION_MANAGER_NO_ERROR;
}


/**
 *  @brief
 *    This function de-registers an IV callback function associated
 *    with xTSid
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *
 *  @param[in]  pidChangedCallback
 *                pointer to function which will be removed from
 *                the notification list.
 * 
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaDeRegisterTSIdIvNotificationCallback
(
    TTransportSessionId   xTSid, 
    BrcmCaIvChangedCallback ivChangedCallback
)
{
    SessionManagerTSContext *traverse=0;
    BDBG_MSG(("%s: xTSid = %d", __FUNCTION__, xTSid));

    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }
    if(!ivChangedCallback) {
        BDBG_ERR(("%s: Invalid ivChangedCallback", __FUNCTION__));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }
    if(!gListMutexLock) {
        BDBG_ERR(("%s: !gListMutexLock xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    /* check if TSID context exists */
    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid) {
            /* remove the callback */
            traverse->ivChangeCallback = 0;
            break;
        }
        traverse = traverse->next;
    }
    BKNI_ReleaseMutex(gListMutexLock);
    return SESSION_MANAGER_NO_ERROR;
}



/* Internal function required by the RPC testharness */
TSessionManagerStatus brcmCaSetScrKeyPath
(
    TTransportSessionId          xTSid, 
    struct SCsdScrKeyPathHandle *pScrKeyPath
)
{
    SessionManagerTSContext *traverse=0;
    BDBG_MSG(("%s: xTSid = %d", __FUNCTION__, xTSid));

    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }
    if(!gListMutexLock) {
        BDBG_ERR(("%s: !gListMutexLock xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    /* check if TSID context exists */
    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid) {
            traverse->pScrKeyPath = pScrKeyPath;
            break;
        }
        traverse = traverse->next;
    }
    BKNI_ReleaseMutex(gListMutexLock);
    return SESSION_MANAGER_NO_ERROR;
}

/* Internal function required by the RPC testharness */
TSessionManagerStatus brcmCaGetScrKeyPath
(
    TTransportSessionId           xTSid, 
    struct SCsdScrKeyPathHandle **ppScrKeyPath
)
{
    SessionManagerTSContext *traverse=0;
    BDBG_MSG(("%s: xTSid = %d", __FUNCTION__, xTSid));

    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }
    if(!ppScrKeyPath) {
        BDBG_ERR(("%s: !ppScrKeyPath xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }
    if(!gListMutexLock) {
        BDBG_ERR(("%s: !gListMutexLock xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    *ppScrKeyPath = NULL;

    /* check if TSID context exists */
    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid) {
            *ppScrKeyPath = traverse->pScrKeyPath;
            break;
        }
        traverse = traverse->next;
    }
    BKNI_ReleaseMutex(gListMutexLock);
    return SESSION_MANAGER_NO_ERROR;
}

/**
 *  @brief
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *  
 *  @param[out] pxSessionType
 *                The session type
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaGetTransportSessionType
(
    TTransportSessionId         xTSid,
    TSessionManagerSessionType *pxSessionType
)
{
    SessionManagerTSContext *traverse=0;
    BDBG_MSG(("Enter %s: xTSid = %d", __FUNCTION__, xTSid));

    /* validate parameters */
    if(xTSid == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("%s: Invalid xTSid = %d", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    if(NULL == pxSessionType) {
        BDBG_ERR(("%s: Invalid parameters", __FUNCTION__));
        return SESSION_MANAGER_ERROR_BAD_PARAMETER;
    }

    /* Default to SESSION_TYPE_UNDEFINED */
    *pxSessionType = SESSION_TYPE_UNDEFINED;

    if(!gListMutexLock) {
        if(BKNI_CreateMutex( &gListMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SESSION_MANAGER_ERROR;
        }
    }

    /* if there is already a context just add the IV to the context */
    BKNI_AcquireMutex(gListMutexLock);
    traverse = gSessionManagerTSContextHead;
    while (traverse) {
        if(traverse->xTSid == xTSid)
            break;
        traverse = traverse->next;
    }

    if(!traverse) {
        BDBG_ERR( ("xTSid does not exit") );
        BKNI_ReleaseMutex(gListMutexLock);
        return SESSION_MANAGER_ERROR;
    }

    *pxSessionType = traverse->xSessionType;

    BKNI_ReleaseMutex(gListMutexLock);
    BDBG_MSG(("Leave %s: *pxSessionType = %d", __FUNCTION__, *pxSessionType));
    return SESSION_MANAGER_NO_ERROR;
}
