/******************************************************************************
*    (c)2008-2013 Broadcom Corporation
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
* Module Description:
*  Initial Implementation of DAL functionality
*	Compatibility NOCS3.0
*	Compatibility NASC1.5
*
* Revision History:
* 
*  26/11/13 - Version 1.0
*  Initial version internally tested with DALTS Version 3.12.0
* 	Based on DALAPI Version 7.1.0
*	Based on Nagra header ca_sec.h Revision 5.1.1
*
* 
* 
*****************************************************************************/


#include <string.h>

#include "bchp_sun_top_ctrl.h"
#include "bchp_aon_ctrl.h"
#include "bchp_nand.h"

#include "nexus_platform.h"
#include "nexus_dma.h"
#include "nexus_types.h"
#include "nexus_memory.h"
#include "nexus_security_datatypes.h"
#include "nexus_security.h"
#include "nexus_security_types.h"
#include "nexus_otpmsp.h"
#include "nexus_keyladder.h"
#include "nexus_security_rawcommand.h"
#include "ca_sec.h"
#include "nocs_sec_impl.h"
#include "ca_defs.h"
#include "bdbg.h"
#include "bchp_jtag_otp.h"

#include "nexus_random_number.h"
#include "nocs_csd_impl.h"
#include "nocs_csd.h"
#include "csd_brcm_extensions.h"
#include "brcm_ca_session_manager_external.h"
#include "brcm_ca_session_manager_internal.h"

#include "emitools.h"

#ifndef NO_C99
#include <openssl/bn.h> 

#include <openssl/rsa.h>
#include <openssl/dh.h>
#include <openssl/sha.h>
#include <openssl/ecdh.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/objects.h>
#include <openssl/hmac.h>
#include <openssl/err.h>
#endif

#include <stdio.h>

BDBG_MODULE(NSEC);
bool SEC_INITIALIZED = FALSE;

#define SEC_MAX_INDEX 2

#define CCI_METADATA_TAG                    0x00
#define COPY_CONTROL_METADATA_TAG           0x01
#define URI_METADATA_TAG                    0x04
#define PRM_METADATA_TAG                    0xA5
#define ES_PID_LIST_METADATA_TAG            0xA6
#define CAM_HOST_REVOCATION_METADATA_TAG    0xC0

#define MAX_NUMBER_OF_PIDS_PER_XSESSION     (6)

TSecInitParameters*  secInitParameters[SEC_MAX_INDEX]= {NULL};
extern bool CSD_INITIALIZED;
TCsdInitParameters InitParameters;


/**********************************************************************************************
SEC implementation specific structures
**********************************************************************************************/

/* In order to check for valid TSecCipherSession pointers passed from CAK we need to keep a list
   of those which we have allocated. This way on every API call we can check the xSession is Valid
   This type define this list*/

typedef struct ValidSecCipherSession   *ValidSecCipherSessionNode;

typedef struct ValidSecCipherSession
{
    TSecCipherSession   xSession;
    ValidSecCipherSessionNode	 next;
} ValidSecCipherSession;

TSecStatus secSet0LevelProtectedKey(
    TSecCipherSession   xSession,
    TUnsignedInt16      xEmi,
    size_t              xKeyIdSize,
    TUnsignedInt8*     pxKeyId,
    size_t              xCipheredContentKeySize,
    const TUnsignedInt8*     pxCipheredContentKey
) ;

static TSecStatus secSet2LevelStreamProtectedKey(
    TSecCipherSession   xSession,
    TUnsignedInt16      xEmi,
    size_t              xKeyIdSize,
    TUnsignedInt8*     pxKeyId,
    size_t              xCipheredContentKeySize,
    const TUnsignedInt8*     pxCipheredContentKey,
    size_t              xCipheredProtectingKeySize,
    const TUnsignedInt8*     pxL1CipheredProtectingKey,
    const TUnsignedInt8*     pxL2CipheredProtectingKey
);

/**********************************************************************************************
SEC implementation specific  variables
**********************************************************************************************/

static ValidSecCipherSession *gValidSecCipherSessionListHead = 0;

/* interface locks */
static BKNI_MutexHandle        gValidSecCipherSessionListMutexLock = 0; /* lock used to protect ValidSecCipherSession list above */
#ifndef NO_C99
static BKNI_MutexHandle        gRSAMutexLock = 0;
#endif
static BKNI_MutexHandle        gSHAMutexLock = 0;

static TChar* gpChipsetExtension = 0;
static TChar* gpChipsetRevision = 0;

#if 0 /* HUMAX_PLATFORM_BASE */ /* Call from Application, current not needed */
static TSecFunctionTable    gSecFunctionTable;
#endif

/* pDscKeyPath->initializationVector can end up pointing to this so it needs */
/* scope beyond the key set functions. */
static TCsdUnsignedInt8 xZeroVector[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#define BHSM_SLOT_NUM_INIT_VAL		0xFFFF


/**********************************************************************************************
SEC implementation specific internal functions/macros
**********************************************************************************************/


#if CFG_BIG_ENDIAN == 0
static void 
swap_pid_endian(unsigned char *input)
{
    unsigned char temp;

    temp = input[0];
    input[0] = input[1];
    input[1] = temp;

    return;
}
#else
static void 
swap_pid_endian(unsigned char *input)
{
    return;
}
#endif

/**
 * @brief
 * This function parses a PID descriptor payload buffer and 
 * finds the PIDS within. If found, it returns a pointer to a 
 * list of PIDS and the number of PIDS its returning. 
 *
 * @param pxInputData
 * In: Input buffer containing the descriptor payload to be 
 * parsed. 
 *  
 * @param xInputPayloadLength
 * In: Length of the descriptor payload (excluding header)
 * 
 * @param ppxPidList
 * Out: Pointer to the list of pids, if found.
 *
 * @param pxNumberOfPids
 * Out: Number of PIds, if found.
 *
*/
static void
brcmProcessPIDMetaDataDescriptor_priv (TUnsignedInt8 * pxInputData, TUnsignedInt32 xInputDataLength,
                                TUnsignedInt16 *pxPidList,
                                TUnsignedInt8 * pxNumberOfPids)
{
    TUnsignedInt32 i=0;
    TUnsignedInt8 numberOfPids =0;
    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(!pxInputData || !xInputDataLength ) {
        BDBG_ERR(("%s invalid buffer or length", __FUNCTION__));
        return;
    }

    if(!pxPidList || !pxNumberOfPids) {
        BDBG_ERR(("%s invalid output buffer or length", __FUNCTION__));
        return;
    }

    /*each Pid structure is 3 bytes long*/
    if(xInputDataLength % 3) {
        BDBG_ERR(("%s invalid output buffer or length (not multiple of 24", __FUNCTION__));
        return;
    }

    /* 
    PID descriptor looks like the following: 
     
    es_pid_list_descriptor() 
    { 
        tag 8 uimsbf hA6
        length 8 uimsbf
        for(i=0; i< length; i++){
            reserved 3 uimsbf b000
            pid 13 uimsbf
            type 8 uimsbf
        }
    }  
    */

    /* pull pids out - each Pid structure is 3 bytes long */
    while(i<xInputDataLength) {

        *(pxPidList+numberOfPids) = *(TUnsignedInt16*)(pxInputData+i);
        swap_pid_endian((unsigned char *)(pxPidList+numberOfPids));
        BDBG_MSG(("%s Found pid 0x%x numberOfPids = %d",__FUNCTION__, *(pxPidList+numberOfPids), numberOfPids ));
        numberOfPids++;
        i+=3;
        if(numberOfPids >= MAX_NUMBER_OF_PIDS_PER_XSESSION) {
            BDBG_ERR(("%s more pids in descriptor than supported by DAL", __FUNCTION__));
            return;
        }
    }

    *pxNumberOfPids = numberOfPids;

    BDBG_MSG(("LEAVE: %s",__FUNCTION__));
}

/**
 * @brief
 * This function parses a buffer of metadata and looks for a descriptor.
 * If found, it returns a pointer to the descriptor payload and its length.
 *
 * @param pxInputData
 * In: Input buffer containing the metadata to be parsed.
 * @param xInputDataLength
 * In: Length of the metadata buffer
 *
 * @param xTag
 * In: Tag of the descriptor wanted
 * 
 * @param ppxOutputData
 * Out: Pointer to the payload of the descriptor wanted, if found.
 *
 * @param pxOutputDataLength
 * Out: Length of the payload of the descriptor wanted, if found.
 *
*/
static void
brcmGetMetaDataDescriptor_priv (TUnsignedInt8 * pxInputData, TUnsignedInt32 xInputDataLength,
                                TUnsignedInt8 xTag, TUnsignedInt8 ** ppxOutputData,
                                TUnsignedInt8 * pxOutputDataLength)
{
    TUnsignedInt8 *pInputData = pxInputData;
    TUnsignedInt8 headerLength;
    TUnsignedInt8 tag;
    TUnsignedInt8 dataLength;
    *ppxOutputData = NULL;
    *pxOutputDataLength = 0;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    while ((TUnsignedInt32) (pInputData - pxInputData) < xInputDataLength)
    {
        tag = *pInputData;
        if ((*pInputData & 0x80) == 0)
        {   /* Format 1 */
            BDBG_MSG(("%s Format 1",__FUNCTION__));
            dataLength = *pInputData & 0x0F;
            headerLength = 1;
            tag = (*pInputData >> 4) & 0x0F;
        }
        else if((*pInputData & 0xC0) == 0x80) {   
            /* Format 2 */
            BDBG_MSG(("%s Format 2",__FUNCTION__));
            dataLength = *(pInputData+1);
            headerLength = 2;
        }
        else if((*pInputData & 0xF0) == 0xC0) {
            /* Format 3 */
            BDBG_MSG(("%s Format 3",__FUNCTION__));
            dataLength = 4;
            headerLength = 1;
        }
        else if((*pInputData & 0xF0) == 0xD0) {
            /* Format 4 */
            BDBG_MSG(("%s Format 4",__FUNCTION__));
            dataLength = 3;
            headerLength = 1;
        }
        else if((*pInputData & 0xF0) == 0xE0) {
            /* Format 5 */
            BDBG_MSG(("%s Format 5",__FUNCTION__));
            dataLength = 2;
            headerLength = 1;
        }
        else if((*pInputData & 0xF0) == 0xF0) {
            /* Format 6 */
            BDBG_MSG(("%s Format 6",__FUNCTION__));
            dataLength = 1;
            headerLength = 1;
        }
        else {
            BDBG_ERR(("%s unrecognized descriptor format", __FUNCTION__));
            return;
        }
        if(tag == xTag) { 
            /* Yes, this is the descriptor that I'm looking for. */
            BDBG_MSG(("%s Found meta data descritor for tag 0x%x dataLength = %d",__FUNCTION__, xTag, dataLength));
            *pxOutputDataLength = dataLength;
            *ppxOutputData = pInputData+headerLength;
            break;
        }
        else {   
            /* This is not the right descriptor, go to the next one. */
            pInputData = pInputData + headerLength + dataLength;
        }
    } /* while */

    BDBG_MSG(("LEAVE: %s",__FUNCTION__));
}





static void writeIt(unsigned char * pBuf,unsigned long size)
{
    while (size)
    {
        if (size >= 16)
        {
            BDBG_MSG(("%.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x",
                pBuf[0], pBuf[1], pBuf[2], pBuf[3], pBuf[4], pBuf[5], pBuf[6], pBuf[7],
                pBuf[8], pBuf[9], pBuf[10], pBuf[11], pBuf[12], pBuf[13], pBuf[14], pBuf[15]));
            size -= 16;
            pBuf += 16;
        }
        else
        {       
            while (size)
            {
                BDBG_MSG(("%.2x", pBuf[0]));
                size -= 1;
                pBuf += 1;
            }
        }
    }
    BDBG_MSG(("============"));
}


static TSessionManagerStatus brcmPidChangedCallback_priv(
    TTransportSessionId   xTSid,
    TUnsignedInt16 xPid,
    TPidState xState
)
{
    TCsdStatus status = CSD_NO_ERROR;
    ValidSecCipherSession *traverse = 0;

    BDBG_MSG(("%s Enter", __FUNCTION__));

    /* find associated xSession */
    traverse = gValidSecCipherSessionListHead;
    if(!traverse) {
        BDBG_ERR(("%s no valid SecCipherSession", __FUNCTION__));
        return SESSION_MANAGER_ERROR;
    }

    while (traverse) {
        if(traverse->xSession->xTransportSessionId == xTSid) {
            /* found the xSession */
            break;
        }
        traverse = traverse->next;
    }
    if(!traverse) {
        BDBG_ERR(("%s no xTSid %d in SecCipherSession list", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR;
    }

    /* Is this even a DSC session? */
    if (!traverse->xSession->pDscKeyPath)
        return SESSION_MANAGER_NO_ERROR;

    /* call CSD notifying PID change (removed/added)*/
    if(xState == SESSION_MANAGER_PID_ADDED) {
        status = csdBrcmSetDscPid((TCsdDscKeyPathHandle*)traverse->xSession->pDscKeyPath, xPid);
        if(status !=CSD_NO_ERROR) {
            BDBG_ERR(("%s csdBrcmSetDscPid failed for Pid 0x%x", __FUNCTION__, xPid));
            return SESSION_MANAGER_ERROR;
        }
    } else if(xState == SESSION_MANAGER_PID_REMOVED) {
        status = csdBrcmRemoveDscPid((TCsdDscKeyPathHandle*)traverse->xSession->pDscKeyPath, xPid);
        if(status !=CSD_NO_ERROR) {
            BDBG_ERR(("%s csdBrcmRemoveDscPid failed for Pid 0x%x", __FUNCTION__, xPid));
            return SESSION_MANAGER_ERROR;
        }
    } else {
        BDBG_ERR(("%s invalid xState", __FUNCTION__));
        return SESSION_MANAGER_ERROR;
    }

    return SESSION_MANAGER_NO_ERROR;
}



/**
 *  @brief
 *    This function creates a TSecCipherSession and adds it to
 *    the list of valid sessions, this is neccecerry in order to
 *    validate xSession pointers.
 *
 *
 *  @retval  TSecCipherSession pointer
 *
 *  @remarks
 *    -#
*/
static TSecCipherSession brcmCreateSecCipherSession_priv(void)
{
	TCsdStatus rc;
    ValidSecCipherSession *node=0;
    ValidSecCipherSession *traverse=0;
    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(!gValidSecCipherSessionListMutexLock) {
        if(BKNI_CreateMutex( &gValidSecCipherSessionListMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return 0;
        }
    }

	if(!CSD_INITIALIZED)
	{
		rc = csdInitialize(&InitParameters);
    	if(rc != CSD_NO_ERROR)
    	{
    		return 0;
    	}
	}

    /* check if there are any SecCipherSession contexts if not create one*/
    /* take list lock */
    BKNI_AcquireMutex(gValidSecCipherSessionListMutexLock);

    /* create a new node*/
    node=BKNI_Malloc(sizeof(ValidSecCipherSession));
    if(node == NULL)
    {
        BDBG_ERR(("%s Fail - out of memory", __FUNCTION__));
        BKNI_ReleaseMutex(gValidSecCipherSessionListMutexLock);
        return 0;
    }
    BKNI_Memset(node, 0, sizeof(ValidSecCipherSession));
    /* create new SecCipherSession*/
    node->xSession = BKNI_Malloc(sizeof(struct SSecCipherSession));
    if(!node->xSession) {
        BDBG_ERR(("%s Fail - out of memory", __FUNCTION__));
        BKNI_ReleaseMutex(gValidSecCipherSessionListMutexLock);
        return 0;
    }

    BKNI_Memset(node->xSession, 0, sizeof(struct SSecCipherSession));

    if(!gValidSecCipherSessionListHead) {
        gValidSecCipherSessionListHead = node;
        BDBG_MSG(("%s Added first new node", __FUNCTION__));
    } else {
        /* add to list */
        traverse = gValidSecCipherSessionListHead;
        while(traverse) {
            if(!traverse->next) {
                /* found next free */
                traverse->next = node;
                traverse = 0;
                BDBG_MSG(("%s Added new node", __FUNCTION__));
                continue;
            }
            traverse = traverse->next;
        }
    }

    node->xSession->xTransportSessionId = TRANSPORT_SESSION_ID_INVALID;
    BKNI_ReleaseMutex(gValidSecCipherSessionListMutexLock);
    return node->xSession;
}

/**
 *  @brief
 *    This function destroys a TSecCipherSession and removes it
 *    from the list of valid sessions, this is neccecerry in
 *    order to validate xSession pointers.
 *
 *  @param[in]  SecCipherSession
 *                session to remove from the list and free up
 *                memory.
 *
 *
 *  @retval  non zero success
 *
 *  @remarks
 *    -#
*/
static int brcmDestroySecCipherSession_priv(TSecCipherSession xSession)
{
    BDBG_MSG(("Enter %s", __FUNCTION__));
    /* validate parameters */
    if(!xSession) {
        BDBG_ERR(("%s: Invalid SecCipherSession", __FUNCTION__));
        return 0;
    }

    if(!gValidSecCipherSessionListMutexLock) {
        BDBG_ERR(("%s: !gValidSecCipherSessionListMutexLock", __FUNCTION__));
        return 0;
    }

    /* check if SecCipherSession exists if not error*/
    BKNI_AcquireMutex(gValidSecCipherSessionListMutexLock);
    if(!gValidSecCipherSessionListHead) {
        BDBG_ERR(("%s: no SecCipherSessions in list", __FUNCTION__));
        BKNI_ReleaseMutex(gValidSecCipherSessionListMutexLock);
        return 0;
    } else /* traverse list */
    {
        ValidSecCipherSession *traverse=gValidSecCipherSessionListHead;
        ValidSecCipherSession *prev=gValidSecCipherSessionListHead;

        while(traverse) {
            if(traverse->xSession==xSession) {
                if(traverse==gValidSecCipherSessionListHead) {
                    gValidSecCipherSessionListHead = traverse->next;
                    BKNI_Free(traverse->xSession);
                    BKNI_Free(traverse);
                    BDBG_MSG(("%s: removed head ", __FUNCTION__));
                    BKNI_ReleaseMutex(gValidSecCipherSessionListMutexLock);
                    return 1;
                } else {
                    prev->next = traverse->next;
                    BKNI_Free(traverse->xSession);
                    BKNI_Free(traverse);
                    BDBG_MSG(("%s: removed xSession", __FUNCTION__));
                    BKNI_ReleaseMutex(gValidSecCipherSessionListMutexLock);
                    return 1;
                }
            }
            prev = traverse;
            traverse = traverse->next;
        }

    }
    BKNI_ReleaseMutex(gValidSecCipherSessionListMutexLock);
    BDBG_ERR(("%s: failed to remove xSession = %d", __FUNCTION__));
    return 0;
}

/**
 *  @brief
 *    This function checks that the secCipher session pointer
 *    was allocated within this module and is therefore valid.
 *
 *  @param[in]  pSecCipherSession
 *                session pointer to validate
 *
 *
 *  @retval  non zero success
 *
 *  @remarks
 *    -#
*/
static int brcmValidateSecCipherSession_priv(TSecCipherSession xSession)
{
    ValidSecCipherSession *traverse=0;
    /* validate parameters */
    if(!xSession) {
        BDBG_ERR(("%s: Invalid SecCipherSession", __FUNCTION__));
        return 0;
    }

    if(!gValidSecCipherSessionListMutexLock) {
        BDBG_ERR(("%s: !gValidSecCipherSessionListMutexLock", __FUNCTION__));
        return 0;
    }

    /* check if SecCipherSession exists if not error*/
    BKNI_AcquireMutex(gValidSecCipherSessionListMutexLock);
    traverse = gValidSecCipherSessionListHead;
    while(traverse) {
        if(traverse->xSession == xSession) {
            /* found next free */
            BKNI_ReleaseMutex(gValidSecCipherSessionListMutexLock);
            BDBG_MSG(("%s xSession %p Valid", __FUNCTION__, xSession));
            return 1;
        }
        traverse = traverse->next;
    }
    BKNI_ReleaseMutex(gValidSecCipherSessionListMutexLock);
    BDBG_ERR(("%s xSession %p NOT Valid", __FUNCTION__, xSession));
    return 0;
}


static TSecStatus brcmSecDetermineScrOrDsc_priv(
    TUnsignedInt16      xEmi,
    TSecCipherSession xSession
)
{
    TSessionManagerSessionType xSessionType = SESSION_TYPE_UNDEFINED;
    TCsdStatus csdStatus = CSD_NO_ERROR;

    BDBG_MSG(("ENTER: %s xSession = %p", __FUNCTION__, xSession));

    if(xSession == NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("xSession not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* We only have to make a decision if we still have both */
    /* DSC and SCR key paths. */
    if((NULL == xSession->pDscKeyPath) || (NULL == xSession->pScrKeyPath))
        return SEC_NO_ERROR;

    /* See if a session type has been set */
    if(brcmCaGetTransportSessionType(xSession->xTransportSessionId, &xSessionType)!=SESSION_MANAGER_NO_ERROR) {
        BDBG_MSG(("brcmCaGetTransportSessionType failed")) ;
        return SEC_ERROR;
    }

    /* No, best guess based on EMI */
    if(SESSION_TYPE_UNDEFINED == xSessionType) {
        xSessionType = brcmEmiIsMpegTs(xEmi) ? SESSION_TYPE_DSC : SESSION_TYPE_SCR;
        brcmCaSetTransportSessionType(xSession->xTransportSessionId, xSessionType);
    }

    /* If DSC, close the SCR key path */
    if(SESSION_TYPE_DSC == xSessionType) {
        csdStatus = csdBrcmCloseScrKeyPath(xSession->pScrKeyPath);
        xSession->pScrKeyPath = NULL;
        brcmCaSetScrKeyPath(xSession->xTransportSessionId, NULL);
    }

    /* If SCR, close the DSC key path */
    if(SESSION_TYPE_SCR == xSessionType) {
        brcmCaDeRegisterTSIdPidNotificationCallback(xSession->xTransportSessionId, brcmPidChangedCallback_priv);
        csdStatus = csdBrcmCloseDscKeyPath(xSession->pDscKeyPath);
        xSession->pDscKeyPath = NULL;
    }

    return SEC_NO_ERROR;
}


static TSecStatus brcmSecSetSessionStreamIv_priv(
    TSecCipherSession xSession,
    const TUnsignedInt8 *pxIv,
    size_t xIvSize
)
{
    NEXUS_SecurityClearKey clearKey;

    BDBG_MSG(("ENTER: %s xSession = %p", __FUNCTION__, xSession));

    if(xSession == NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("xSession not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if((xSession->type!=SEC_SESSION_STREAM_DECRYPT_TYPE) && (xSession->type!=SEC_SESSION_STREAM_ENCRYPT_TYPE)) {
        BDBG_ERR(("%s xSession is not stream type", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* Remember the IV */
    if ((pxIv != NULL) && (xIvSize > 0)) {
        BKNI_Memcpy(xSession->xIv, pxIv, xIvSize);
        xSession->xIvSize = xIvSize;
    } else {
        xSession->xIvSize = 0;
        return SEC_NO_ERROR;
    }

    /* Do the IV key load now? */
    if (!xSession->bKeySet)
        return SEC_NO_ERROR;

    NEXUS_Security_GetDefaultClearKey(&clearKey);
    clearKey.keyIVType= NEXUS_SecurityKeyIVType_eIV;
    clearKey.keyEntryType = NEXUS_SecurityKeyType_eIv;
    clearKey.keySize = 16;
    BKNI_Memset(clearKey.keyData, 0, sizeof(TCsdUnsignedInt8)*16);

    if (xIvSize == 8)
        BKNI_Memcpy(&clearKey.keyData[8], pxIv, 8);
    else
        BKNI_Memcpy(clearKey.keyData, pxIv, xIvSize);

    if (NULL != xSession->pScrKeyPath)
    {
        BDBG_MSG(("%s: Setting IV for SCR", __FUNCTION__));
        if (NEXUS_Security_LoadClearKey(xSession->pScrKeyPath->keyHandle, &clearKey) != 0)
        {
            BDBG_ERR(("%s: Load IV failed @ %s:%d", __FUNCTION__, __FILE__, __LINE__));
            return SEC_ERROR;
        }
        BKNI_Memcpy(xSession->pScrKeyPath->ivArray, pxIv, xIvSize);\
    }

    if (NULL != xSession->pDscKeyPath)
    {
        BDBG_MSG(("%s: Setting IV for DSC", __FUNCTION__));
        clearKey.dest=NEXUS_SecurityAlgorithmConfigDestination_eCa;
        if (NEXUS_Security_LoadClearKey(xSession->pDscKeyPath->keyHandle, &clearKey) != 0)
        {
            BDBG_ERR(("%s: Load IV failed @ %s:%d", __FUNCTION__, __FILE__, __LINE__));
            return SEC_ERROR;
        }
        return SEC_NO_ERROR;
    }

    return SEC_NO_ERROR;
}

static TSessionManagerStatus brcmIvChangedCallback_priv(
    TTransportSessionId xTSid,
    const TUnsignedInt8 *pxIv,
    size_t xIvSize
)
{
    TSecStatus status = SEC_NO_ERROR;
    ValidSecCipherSession *traverse = 0;

    BDBG_MSG(("%s Enter", __FUNCTION__));

    /* find associated xSession */
    traverse = gValidSecCipherSessionListHead;
    if(!traverse) {
        BDBG_ERR(("%s no valid SecCipherSession", __FUNCTION__));
        return SESSION_MANAGER_ERROR;
    }

    while (traverse) {
        if(traverse->xSession->xTransportSessionId == xTSid) {
            /* found the xSession */
            break;
        }
        traverse = traverse->next;
    }
    if(!traverse) {
        BDBG_ERR(("%s no xTSid %d in SecCipherSession list", __FUNCTION__, xTSid));
        return SESSION_MANAGER_ERROR;
    }

    status = brcmSecSetSessionStreamIv_priv(traverse->xSession, pxIv, xIvSize);
    return (status == SEC_NO_ERROR) ? SESSION_MANAGER_NO_ERROR  : SESSION_MANAGER_ERROR;
}


const unsigned char Curve_Table160[6][21]= {
    {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF},
    {0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xF4,0xC8,0xF9,0x27,0xAE,0xD3,0xCA,0x75,0x22,0x57},
    {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFC},
    {0x00, 0x1C, 0x97, 0xBE, 0xFC, 0x54, 0xBD, 0x7A, 0x8B, 0x65, 0xAC, 0xF8, 0x9F, 0x81, 0xD4, 0xD4, 0xAD, 0xC5, 0x65, 0xFA, 0x45},
    {0x00, 0x4A, 0x96, 0xB5, 0x68, 0x8E, 0xF5, 0x73, 0x28, 0x46, 0x64, 0x69, 0x89, 0x68, 0xC3, 0x8B, 0xB9, 0x13, 0xCB, 0xFC, 0x82},
    {0x00, 0x23, 0xA6, 0x28, 0x55, 0x31, 0x68, 0x94, 0x7D, 0x59, 0xDC, 0xC9, 0x12, 0x04, 0x23, 0x51, 0x37, 0x7A, 0xC5, 0xFB, 0x32},
};


const unsigned char Curve_Table224[6][28]= {
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01},
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x16,0xA2,0xE0,0xB8,0xF0,0x3E,0x13,0xDD,0x29,0x45,0x5C,0x5C,0x2A,0x3D},
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE},
    {0xb4,0x05,0x0a,0x85,0x0c,0x04,0xb3,0xab,0xf5,0x41,0x32,0x56,0x50,0x44,0xb0,0xb7,0xd7,0xbf,0xd8,0xba,0x27,0x0b,0x39,0x43,0x23,0x55,0xff,0xb4},
    {0xb7,0x0e,0x0c,0xbd,0x6b,0xb4,0xbf,0x7f,0x32,0x13,0x90,0xb9,0x4a,0x03,0xc1,0xd3,0x56,0xc2,0x11,0x22,0x34,0x32,0x80,0xd6,0x11,0x5c,0x1d,0x21},
    {0xbd,0x37,0x63,0x88,0xb5,0xf7,0x23,0xfb,0x4c,0x22,0xdf,0xe6,0xcd,0x43,0x75,0xa0,0x5a,0x07,0x47,0x64,0x44,0xd5,0x81,0x99,0x85,0x00,0x7e,0x34},
};

const unsigned char Curve_Table256[6][32]=	{
    {0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
    {0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xBC,0xE6,0xFA,0xAD,0xA7,0x17,0x9E,0x84,0xF3,0xB9,0xCA,0xC2,0xFC,0x63,0x25,0x51},
    {0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC},
    {0x5a,0xc6,0x35,0xd8,0xaa,0x3a,0x93,0xe7,0xb3,0xeb,0xbd,0x55,0x76,0x98,0x86,0xbc,0x65,0x1d,0x06,0xb0,0xcc,0x53,0xb0,0xf6,0x3b,0xce,0x3c,0x3e,0x27,0xd2,0x60,0x4b},
    {0x6b,0x17,0xd1,0xf2,0xe1,0x2c,0x42,0x47,0xf8,0xbc,0xe6,0xe5,0x63,0xa4,0x40,0xf2,0x77,0x03,0x7d,0x81,0x2d,0xeb,0x33,0xa0,0xf4,0xa1,0x39,0x45,0xd8,0x98,0xc2,0x96},
    {0x4f,0xe3,0x42,0xe2,0xfe,0x1a,0x7f,0x9b,0x8e,0xe7,0xeb,0x4a,0x7c,0x0f,0x9e,0x16,0x2b,0xce,0x33,0x57,0x6b,0x31,0x5e,0xce,0xcb,0xb6,0x40,0x68,0x37,0xbf,0x51,0xf5},
};


const unsigned char Curve_Table384[6][48]=	{
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF},
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xC7,0x63,0x4D,0x81,0xF4,0x37,0x2D,0xDF,0x58,0x1A,0x0D,0xB2,0x48,0xB0,0xA7,0x7A,0xEC,0xEC,0x19,0x6A,0xCC,0xC5,0x29,0x73},
    {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFC},
    {0xb3,0x31,0x2f,0xa7,0xe2,0x3e,0xe7,0xe4,0x98,0x8e,0x05,0x6b,0xe3,0xf8,0x2d,0x19,0x18,0x1d,0x9c,0x6e,0xfe,0x81,0x41,0x12,0x03,0x14,0x08,0x8f,0x50,0x13,0x87,0x5a,0xc6,0x56,0x39,0x8d,0x8a,0x2e,0xd1,0x9d,0x2a,0x85,0xc8,0xed,0xd3,0xec,0x2a,0xef},
    {0xaa,0x87,0xca,0x22,0xbe,0x8b,0x05,0x37,0x8e,0xb1,0xc7,0x1e,0xf3,0x20,0xad,0x74,0x6e,0x1d,0x3b,0x62,0x8b,0xa7,0x9b,0x98,0x59,0xf7,0x41,0xe0,0x82,0x54,0x2a,0x38,0x55,0x02,0xf2,0x5d,0xbf,0x55,0x29,0x6c,0x3a,0x54,0x5e,0x38,0x72,0x76,0x0a,0xb7},
    {0x36,0x17,0xde,0x4a,0x96,0x26,0x2c,0x6f,0x5d,0x9e,0x98,0xbf,0x92,0x92,0xdc,0x29,0xf8,0xf4,0x1d,0xbd,0x28,0x9a,0x14,0x7c,0xe9,0xda,0x31,0x13,0xb5,0xf0,0xb8,0xc0,0x0a,0x60,0xb1,0xce,0x1d,0x7e,0x81,0x9d,0x7a,0x43,0x1d,0x7c,0x90,0xea,0x0e,0x5f}
};

void byteSwap(
    unsigned char *word
)
{
    unsigned char temp;

    temp = word[0];
    word[0] = word[3];
    word[3] = temp;

    temp = word[1];
    word[1] = word[2];
    word[2] = temp;

    return;
}

static void CompleteCallback(void *pParam, int iParam)
{
    iParam=iParam;
    BKNI_SetEvent(pParam);
}


TSecStatus secR2RCryptoOperation(
    NEXUS_KeySlotHandle keyHandle,
    const TUnsignedInt8* pxInputData,
    TUnsignedInt8* pxOutputData,
    TSize xDataSize
)
{
    NEXUS_DmaHandle dma;
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_DmaJobHandle dmaJob;
    NEXUS_DmaJobBlockSettings blockSettings;
    unsigned char *input_data;
    unsigned char *output_data;
    TSecStatus    retCode  =  SEC_NO_ERROR;
    BKNI_EventHandle event;
    NEXUS_Error rc;

    BDBG_MSG(("Enter %s", __FUNCTION__));

#if(CFG_BIG_ENDIAN==1)
    size_t i;
#endif
    BKNI_CreateEvent(&event);

    /* Need to allocate memory for M2M input and output buffers. */
    NEXUS_Memory_Allocate(sizeof(unsigned char) * xDataSize, NULL, (void *)&input_data);
    NEXUS_Memory_Allocate(sizeof(unsigned char) * xDataSize, NULL, (void *)&output_data);

    /* Copy the input data to M2M input buffer. */
    BKNI_Memcpy((void *)input_data, (void *)pxInputData, xDataSize );

#if(CFG_BIG_ENDIAN==1)
    for(i=0; i<xDataSize; i+=4)
        byteSwap(&(input_data[i]));
#endif

    /* Open DMA handle */
    dma = NEXUS_Dma_Open(0, NULL);

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks = 1;
    jobSettings.keySlot = keyHandle;
    jobSettings.dataFormat = NEXUS_DmaDataFormat_eBlock;
    jobSettings.completionCallback.callback = CompleteCallback;
    jobSettings.completionCallback.context = event;

    dmaJob = NEXUS_DmaJob_Create(dma, &jobSettings);

    NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
    blockSettings.pSrcAddr = input_data;
    blockSettings.pDestAddr = output_data;
    blockSettings.blockSize = xDataSize;
    blockSettings.resetCrypto = true;
    blockSettings.scatterGatherCryptoStart = true;
    blockSettings.scatterGatherCryptoEnd = true;
    blockSettings.cached = true;

    rc = NEXUS_DmaJob_ProcessBlocks(dmaJob, &blockSettings, 1);

    /* Wait for M2M Encryption to complete. */
    if (rc == NEXUS_DMA_QUEUED )
    {
        if (BKNI_WaitForEvent(event, BKNI_INFINITE) )
        {
        }
    }
    else if ( rc != NEXUS_SUCCESS )
    {
        BDBG_ERR(("\nDMA transfer failed"));
    }


#if(CFG_BIG_ENDIAN==1)
    for(i=0; i<xDataSize; i+=4)
        byteSwap(&(output_data[i]));
#endif

    /* Copy the data to M2M output buffer. */
    BKNI_Memcpy((void *)pxOutputData, (void *)output_data, xDataSize );

    /* Done.  Close DMA. */
    NEXUS_DmaJob_Destroy(dmaJob);

    NEXUS_Dma_Close(dma);
    BKNI_DestroyEvent(event);

    NEXUS_Memory_Free(output_data);
    NEXUS_Memory_Free(input_data);

    BDBG_MSG(("LEAVE: %s",__FUNCTION__));
    return retCode;
}


/**********************************************************************************************
SEC Public Functions
**********************************************************************************************/




TSecStatus secGetNuid
(
    TSecNuid* pxNuid
)
{
    BERR_Code errCode = BERR_SUCCESS;
    NEXUS_ReadOtpIO	otpIo;
    unsigned int i;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(pxNuid ==NULL) {
        BDBG_ERR(("%s pxNuid ==NULLr", __FUNCTION__));
        return SEC_ERROR;
    }

    /* Read the ID from OTP. */
    errCode = NEXUS_Security_ReadOTP (
                  NEXUS_OtpCmdReadRegister_eKeyID,
                  NEXUS_OtpKeyType_eA,
                  &otpIo);

    if (errCode != BERR_SUCCESS) {
        BDBG_ERR(("%s NEXUS_Security_ReadOTP Error", __FUNCTION__));
        return SEC_ERROR;
    }

    /* NUID is the second Dword */
    BKNI_Memcpy((void *)pxNuid, (void *)&(otpIo.otpKeyIdBuf[0+SEC_NUID_LENGTH_32/8]), SEC_NUID_LENGTH_32/8 );

    for (i=0; i<(SEC_NUID_LENGTH_32/8); i++)
        BDBG_MSG((" %02X ", ((TUnsignedInt8*)pxNuid)[i])) ;

    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

TSecStatus secGetNuid64
(
    TSecNuid64* pxNuid
)
{
    BERR_Code errCode = BERR_SUCCESS;
    NEXUS_ReadOtpIO	otpIo;
    unsigned int i;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(pxNuid ==NULL) {
        BDBG_ERR(("%s pxNuid ==NULL", __FUNCTION__));
        return SEC_ERROR;
    }

    /* Read the ID from OTP. */
    errCode = NEXUS_Security_ReadOTP (
                  NEXUS_OtpCmdReadRegister_eKeyID,
                  NEXUS_OtpKeyType_eA,
                  &otpIo);

    if (errCode != BERR_SUCCESS) {
        BDBG_ERR(("%s NEXUS_Security_ReadOTP Error", __FUNCTION__));
        return SEC_ERROR;
    }
    /* NUID is the second Dword */
    BKNI_Memcpy((void *)pxNuid, (void *)&(otpIo.otpKeyIdBuf[0]), SEC_NUID_LENGTH/8 );
    for (i=0; i<(SEC_NUID_LENGTH/8); i++)
        BDBG_MSG((" %02X ", ((TUnsignedInt8*)pxNuid)[i])) ;

    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

TSecStatus secGetChipsetRevision
(
    const TChar** ppxChipsetRevision
) {
    uint32_t uChipNum;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(ppxChipsetRevision ==NULL) {
        BDBG_ERR(("%s ppxChipsetRevision ==NULL", __FUNCTION__));
        return SEC_ERROR;
    }

    if(!gpChipsetRevision) {
        gpChipsetRevision = BKNI_Malloc(3);
        if(!gpChipsetRevision) {
            BDBG_ERR(("%s cannot allocate mem for gpChipsetRevision", __FUNCTION__));
            return SEC_ERROR;
        }
    }

    *gpChipsetRevision = 'A';
    *(gpChipsetRevision+1) = '0';

    NEXUS_Platform_ReadRegister( BCHP_SUN_TOP_CTRL_CHIP_FAMILY_ID, &uChipNum );
    uChipNum &= 0xff;
    *gpChipsetRevision += (uChipNum>>4);
    *(gpChipsetRevision+1) += uChipNum & 0x0f;
    *(gpChipsetRevision+2) = 0;

    *ppxChipsetRevision = gpChipsetRevision;

    BDBG_MSG(("Leave %s revision = %s", __FUNCTION__, *ppxChipsetRevision));
    return SEC_NO_ERROR;

}


/*
'****************************************************************************************
'	read a word from OTP map
'****************************************************************************************
Function read_OTP_data(bit_address)
*/
TSecStatus sec_P_ReadOtpData(
    unsigned int  row,
    unsigned int *pData
)
{
    unsigned int  regVal;

    NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_CTRL_1, &regVal);
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_1, regVal | 0x00000001);

    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x0000000f);
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003);


    regVal = 0 ;
    while (( regVal & 0x1 ) == 0 )	{
        BKNI_Sleep(10);
        NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal);
        BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
    }
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x00000004);

    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003);



    regVal = 0 ;
    while (( regVal & 0x1 ) == 0 )	{
        BKNI_Sleep(10);
        NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal);
        BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
    }


    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x00000008);

    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003);
    regVal = 0 ;
    while (( regVal & 0x1 ) == 0 )	{
        BKNI_Sleep(10);
        NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal);
        BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
    }

    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, 0x0000000D);

    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00200003);
    regVal = 0 ;
    while (( regVal & 0x1 ) == 0 )	{
        BKNI_Sleep(10);
        NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal);
        BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
    }


    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_3, row*32);

    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00000000);
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0x00A00001);
    regVal = 0 ;
    while (( regVal & 0x1 ) == 0 )	{
        BKNI_Sleep(10);
        NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_1, &regVal);
        BDBG_MSG(("Polling for status done status_reg = 0x%x\n",regVal));
    }
    NEXUS_Platform_ReadRegister(BCHP_JTAG_OTP_GENERAL_STATUS_0, &regVal);
    *pData = regVal;
    NEXUS_Platform_WriteRegister(BCHP_JTAG_OTP_GENERAL_CTRL_0, 0);

    return SEC_NO_ERROR;
}


TSecStatus secGetChipsetExtension
(
    const TChar** ppxChipsetExtension
) {
    unsigned status_reg;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(ppxChipsetExtension == NULL) {
        BDBG_ERR(("%s ppxChipsetExtension == NULL", __FUNCTION__));
        return SEC_ERROR;
    }

    if(!gpChipsetExtension) {
        gpChipsetExtension = BKNI_Malloc(3);
        if(!gpChipsetExtension) {
            BDBG_ERR(("%s cannot allocate mem for gpChipsetExtension", __FUNCTION__));
            return SEC_ERROR;
        }
    }

    sec_P_ReadOtpData(13, &status_reg);

#if 1
    *gpChipsetExtension = (unsigned char)(((status_reg>>20)&0x3f)|0x40);
    *(gpChipsetExtension+1) = (unsigned char)(((status_reg>>14)&0x3f)|0x40);
    *(gpChipsetExtension+2) = 0;
#else
    /* for test purpose before have a real part */
    *pBuffer = 'N';
    *(pBuffer+1) = 'P';
    *(gpChipsetExtension+1) = 0;
#endif

    *ppxChipsetExtension = gpChipsetExtension;
    BDBG_MSG(("Leave %s revision = %s", __FUNCTION__, *ppxChipsetExtension));

    return SEC_NO_ERROR;
}

TSecStatus secOpenRam2RamEncryptSession
(
    TSecCipherSession*     pxSession
) {
    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_KeySlotHandle keyHandle;

    BDBG_MSG(("ENTER: %s", __FUNCTION__));

    if(pxSession == NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    *pxSession = brcmCreateSecCipherSession_priv();
    if(!*pxSession) {
        BDBG_ERR(("secOpenRam2RamDecryptSession: brcmCreateSecCipherSession_priv failed")) ;
        return SEC_ERROR;
    }

    BDBG_MSG(("%s, session id is %p", __FUNCTION__, *pxSession));
    (*pxSession)->type = SEC_SESSION_R2R_TYPE;
    (*pxSession)->bEncrypt = true;
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    if((*pxSession)->keyHandle == NULL) {
        keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
        (*pxSession)->keyHandle  = keyHandle;
        if( keyHandle == NULL)
        {
            BDBG_ERR(("%s: NEXUS_Security_AllocateKeySlot failed", __FUNCTION__));
            return  SEC_ERROR;  /* return if no keyslot is available*/
        }
        BDBG_MSG(("%s allocated keyhandle %p", __FUNCTION__,(*pxSession)->keyHandle));
    }
    BDBG_MSG(("LEAVE: %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

TSecStatus secOpenRam2RamDecryptSession
(
    TSecCipherSession*     pxSession
) {
    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_KeySlotHandle keyHandle;

    BDBG_MSG(("ENTER: %s", __FUNCTION__));

    if(pxSession == NULL) {
        BDBG_ERR(("%s, invalid parameter", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    *pxSession = brcmCreateSecCipherSession_priv();
    if(!*pxSession) {
        BDBG_ERR(("secOpenRam2RamDecryptSession: brcmCreateSecCipherSession_priv failed")) ;
        return SEC_ERROR;
    }
    BDBG_MSG(("%s, session id is %p", __FUNCTION__, *pxSession));

    (*pxSession)->type = SEC_SESSION_R2R_TYPE;
    (*pxSession)->bEncrypt = false;
    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    if((*pxSession)->keyHandle == NULL) {
        keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);
        (*pxSession)->keyHandle  = keyHandle;
        if( keyHandle == NULL)
        {
            BDBG_ERR(("%s: NEXUS_Security_AllocateKeySlot failed", __FUNCTION__));
            return  SEC_ERROR;  /* return if no keyslot is available*/
        }
    }


    BDBG_MSG(("LEAVE: %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

TSecStatus secCloseSession
(
    TSecCipherSession      xSession
) {
    TCsdStatus status = CSD_NO_ERROR;
    BDBG_MSG(("ENTER: %s xSession = %p", __FUNCTION__, xSession));
    if(xSession == NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("xSession not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if((xSession->type!=SEC_SESSION_STREAM_DECRYPT_TYPE) && (xSession->type!=SEC_SESSION_STREAM_ENCRYPT_TYPE) && (xSession->keyHandle == NULL)) { /* not a valid ram2ram session */
        BDBG_ERR(("%s !xSession->keyHandle", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* clean up any resources */
    if (xSession->type==SEC_SESSION_STREAM_DECRYPT_TYPE)
        brcmCaDeRegisterTSIdPidNotificationCallback(xSession->xTransportSessionId, brcmPidChangedCallback_priv);

    if ((xSession->type==SEC_SESSION_STREAM_DECRYPT_TYPE) || (xSession->type==SEC_SESSION_STREAM_ENCRYPT_TYPE))
        brcmCaDeRegisterTSIdIvNotificationCallback(xSession->xTransportSessionId, brcmIvChangedCallback_priv);

    if(xSession->pDscKeyPath) {
        BDBG_MSG(("closing DSC CSD keypath"));
        status = csdBrcmCloseDscKeyPath((TCsdDscKeyPathHandle*)xSession->pDscKeyPath);
        if(status!=CSD_NO_ERROR) {
            BDBG_ERR(("csdBrcmCloseDscKeyPath failed")) ;
        }
        /* clear keyslot with application */
        brcmCaAssociateM2MKeyslotHandle(xSession->xTransportSessionId, 0);
    }

    if(xSession->pScrKeyPath) {
        BDBG_MSG(("closing SCR CSD keypath"));
        status = csdBrcmCloseScrKeyPath((TCsdScrKeyPathHandle*)xSession->pScrKeyPath);
        if(status!=CSD_NO_ERROR) {
            BDBG_ERR(("csdBrcmCloseScrKeyPath failed")) ;
        }
        /* clear keyslot with application */
        brcmCaAssociateM2MKeyslotHandle(xSession->xTransportSessionId, 0);
        brcmCaSetScrKeyPath(xSession->xTransportSessionId, NULL);
    }

    if(xSession->keyHandle) {
        NEXUS_Security_FreeKeySlot(xSession->keyHandle);
    }

    if(xSession->pxMetadata) {
        BKNI_Free(xSession->pxMetadata);
    }

    /*Free session mem*/
    if(!brcmDestroySecCipherSession_priv(xSession)) {
        BDBG_ERR(("%s failed to remove xSession", __FUNCTION__));
    }
    BDBG_MSG(("LEAVE: %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

TSecStatus secSetClearTextKey
(
    TSecCipherSession   xSession,
    TUnsignedInt16      xEmi,
    size_t              xKeyIdSize,
    TUnsignedInt8*      pxKeyId,
    size_t              xClearTextKeySize,
    const TUnsignedInt8*     pxClearTextKey
) {

    NEXUS_KeySlotHandle keyHandle;
    NEXUS_SecurityAlgorithmSettings algorithm;
    NEXUS_SecurityClearKey key;
    TSecStatus				   retCode = SEC_NO_ERROR;

    BDBG_MSG(("Enter %s xSession=%p xEmi=0x%x xKeyIdSize=%d pxKeyId=%p xClearTextKeySize=%d pxClearTextKey=%p", __FUNCTION__, 
              xSession, xEmi, 
              xKeyIdSize, pxKeyId,
              xClearTextKeySize,
              pxClearTextKey));

    if(!pxClearTextKey || !xClearTextKeySize) {
        BDBG_ERR(("invalid key pointer")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(xSession== NULL) {
        BDBG_ERR(("invalid xSession")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("xSession not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if (!brcmEmiIsValid(xEmi)) {
        BDBG_ERR(("invalid xEmi")) ;
        return SEC_ERROR_BAD_EMI;
    }

    /* validate pxKeyId*/
    /* if this is R2R should always be NULL*/
#ifndef RPC_TEST_HARNESS_WORKAROUND
    if(xSession->type == SEC_SESSION_R2R_TYPE) {
        if(xKeyIdSize || pxKeyId) {
            BDBG_ERR(("xKeyIdSize passed for R2R session")) ;
            return SEC_ERROR_BAD_PARAMETER;
        }
    }
#endif

    /* if this is Stream type pxKeyId should have value only if is a MPEG TS EMI */
    if((xSession->type == SEC_SESSION_STREAM_DECRYPT_TYPE) || (xSession->type == SEC_SESSION_STREAM_ENCRYPT_TYPE)) {
        if(brcmEmiIsMpegTs(xEmi)){
            if(!xKeyIdSize || !pxKeyId) {
                BDBG_ERR(("xKeyIdSize NOT passed for MPEG TS EMI Stream session")) ;
                return SEC_ERROR_BAD_PARAMETER;
            }
        }
        /* Note: The NOCS3 test harness always passes pxKeyId regardless */
        /* of EMI, so we just ignore this case. */
#ifndef RPC_TEST_HARNESS_WORKAROUND
        else {
            if(xKeyIdSize || pxKeyId) {
                BDBG_ERR(("xKeyIdSize passed for Block mode Stream session")) ;
                return SEC_ERROR_BAD_PARAMETER;
            }
        }
#endif
        BDBG_MSG(("DSC type xSession")) ;
        return secSet0LevelProtectedKey(xSession,xEmi,xKeyIdSize,pxKeyId,xClearTextKeySize,pxClearTextKey);
    }


    /* Make sure algorithm is block type. */
    if(brcmEmiIsMpegTs(xEmi)) {
        BDBG_ERR(("EMI not valid"));
        return SEC_ERROR_BAD_EMI;
    }

    /* Make sure key size is correct */
    if ((xClearTextKeySize != brcmEmiGetKeySize(xEmi)) != 0) {
        BDBG_ERR(("invalid xClearTextKeySize"));
        return SEC_ERROR_BAD_PARAMETER;
    }

    keyHandle = xSession->keyHandle ;
    xSession->xEmi = xEmi;

    /* Set up encryption key */
    if (!brcmEmiSetupAlgorithm(xEmi, &algorithm)) {
        BDBG_ERR(("%s brcmEmiSetupAlgorithm failed", __FUNCTION__));
        retCode = SEC_ERROR;
        goto SEC_P_DONE;
    }

    algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
    if(xSession->bEncrypt)
        algorithm.operation = NEXUS_SecurityOperation_eEncrypt;
    else
        algorithm.operation = NEXUS_SecurityOperation_eDecrypt;
    /* Load algorithm to key table.  Set it up to do encryption. */
    if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
    {
        BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));
        retCode =  SEC_ERROR;
        goto SEC_P_DONE;
    }

    /*
    BKNI_Memcpy((void *)&(xSession->algorithmConfig), (void *)&algorithm, sizeof(NEXUS_SecurityAlgorithmSettings));
    */
    NEXUS_Security_GetDefaultClearKey(&key);

    key.keyIVType	= NEXUS_SecurityKeyIVType_eNoIV;
    key.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key.keySize = xClearTextKeySize;

    BKNI_Memcpy(key.keyData, pxClearTextKey, sizeof(TUnsignedInt8)*xClearTextKeySize);

    /* Load clear key to key table. */
    if (NEXUS_Security_LoadClearKey(keyHandle, &key) != 0)
    {
        BDBG_WRN(("\nLoad decryption key failed \n"));
        retCode = SEC_ERROR;
        goto SEC_P_DONE;
    }

    if(!xSession->bKeySet) {
        xSession->bKeySet = true;
        /* notify application that key has been set */
        if (xSession->xTransportSessionId != TRANSPORT_SESSION_ID_INVALID)
            brcmCaNotifyCipherSessionKeyLoaded(xSession->xTransportSessionId);
    } 
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;

SEC_P_DONE:
    return (retCode);
}

TSecStatus secSet2LevelProtectedKey
(
    TSecCipherSession   xSession,
    TUnsignedInt16      xEmi,
    size_t              xKeyIdSize,
    TUnsignedInt8*      pxKeyId,
    size_t              xCipheredContentKeySize,
    const TUnsignedInt8*     pxCipheredContentKey,
    size_t              xCipheredProtectingKeySize,
    const TUnsignedInt8*     pxL1CipheredProtectingKey,
    const TUnsignedInt8*     pxL2CipheredProtectingKey
) {
    NEXUS_KeySlotHandle keyHandle=NULL;
    NEXUS_SecurityAlgorithmSettings algorithm;
    NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
    NEXUS_SecurityEncryptedControlWord encrytedCW, key4;
    TUnsignedInt8 proc_in3[2][16];
    TSecStatus				   retCode = SEC_NO_ERROR;

    BDBG_MSG(("Enter %s xSession=%p xEmi=0x%x xKeyIdSize=%d pxKeyId=%d xCipheredContentKeySize=%d pxCipheredContentKey=%p xCipheredProtectingKeySize=%d pxL1CipheredProtectingKey=%p pxL2CipheredProtectingKey=%p", __FUNCTION__, 
              xSession, xEmi, 
              xKeyIdSize, pxKeyId,
              xCipheredContentKeySize,
              pxCipheredContentKey,
              xCipheredProtectingKeySize,
              pxL1CipheredProtectingKey,
              pxL2CipheredProtectingKey));

    if(xSession== NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("xSession not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if((xSession->type == SEC_SESSION_STREAM_DECRYPT_TYPE) || (xSession->type == SEC_SESSION_STREAM_ENCRYPT_TYPE)) {
        return secSet2LevelStreamProtectedKey(
            xSession,
            xEmi,
            xKeyIdSize,
            pxKeyId,
            xCipheredContentKeySize,
            pxCipheredContentKey,
            xCipheredProtectingKeySize,
            pxL1CipheredProtectingKey,
            pxL2CipheredProtectingKey);
    }

    if(!brcmEmiIsValid(xEmi)) {
        BDBG_ERR(("invalid xEmi")) ;
        return SEC_ERROR_BAD_EMI;
    }

    /* Make sure key is valid */
    if(xCipheredContentKeySize != brcmEmiGetKeySize(xEmi) || !pxCipheredContentKey) {
        BDBG_ERR(("xCipheredContentKeySize not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(xCipheredProtectingKeySize != 16) {
        BDBG_ERR(("xCipheredProtectingKeySize not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(!xCipheredProtectingKeySize || !pxL1CipheredProtectingKey || !pxL2CipheredProtectingKey) {
        BDBG_ERR(("CipheredProtectingKey pointer not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* only usefull for R2R type sessions */
    if(xSession->type != SEC_SESSION_R2R_TYPE) {
        BDBG_ERR(("%s SEC_ERROR", __FUNCTION__));
        return SEC_ERROR_BAD_USAGE;
    }

    /* Make sure algorithm is block type. */
    if(brcmEmiIsMpegTs(xEmi)) {
        BDBG_ERR(("EMI not valid"));
        return SEC_ERROR_BAD_EMI;
    }

    keyHandle = xSession->keyHandle;
    xSession->xEmi = xEmi;

    if( keyHandle == NULL)
    {
        BDBG_ERR(("%s keyHandle error", __FUNCTION__));
        return	SEC_ERROR_BAD_PARAMETER;	/* return if no keyslot is available*/
    }

    /* Set up encryption key */
    if (!brcmEmiSetupAlgorithm(xEmi, &algorithm)) {
        BDBG_ERR(("%s brcmEmiSetupAlgorithm failed", __FUNCTION__));
        retCode = SEC_ERROR;
        goto SEC_P_DONE;
    }

    algorithm.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
    if(xSession->bEncrypt)
        algorithm.operation = NEXUS_SecurityOperation_eEncrypt;
    else
        algorithm.operation = NEXUS_SecurityOperation_eDecrypt;

    algorithm.caVendorID=0x97ed;

    if(SEC_R2R_NB_OF_PROTECTING_KEYS == 2)
        algorithm.askmModuleID = 0xE0;
    else
        algorithm.askmModuleID = NEXUS_SecurityAskmModuleID_eModuleID_8;

    algorithm.otpId = NEXUS_SecurityOtpId_eOtpVal;
    algorithm.testKey2Select = 0x0;
    algorithm.key2Select =	NEXUS_SecurityKey2Select_eFixedKey;
    /* Load algorithm to key table.  Set it up to do encryption. */
    if(NEXUS_Security_ConfigAlgorithm(keyHandle, &algorithm) != 0)
    {
        BDBG_WRN(("\nConfigAlg enc keyslot failed \n"));
        retCode =  SEC_ERROR;
        goto SEC_P_DONE;
    }

    /*
    BKNI_Memcpy((void *)&xSession->algorithmConfig, (void *)&algorithm, sizeof(NEXUS_SecurityAlgorithmSettings));
    */
    BKNI_Memset((void *)&encryptedSessionkey, 0, sizeof(encryptedSessionkey));

    /* Load session key3 */
    encryptedSessionkey.keyladderID 	= NEXUS_SecurityKeyladderID_eA;

    encryptedSessionkey.keyGenCmdID 	= NEXUS_SecurityKeyGenCmdID_eKeyGen;
    encryptedSessionkey.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
    encryptedSessionkey.hwklLen 		= NEXUS_SecurityHWKLLenSelect_eLen3;
    encryptedSessionkey.keyladderType	= NEXUS_SecurityKeyladderType_e3Des;
    encryptedSessionkey.rootKeySrc		= NEXUS_SecurityRootKeySrc_eOtpKeyA;
    encryptedSessionkey.swizzleType 	= NEXUS_SecuritySwizzleType_eNone;
    encryptedSessionkey.keyEntryType	= NEXUS_SecurityKeyType_eOdd;
    encryptedSessionkey.keyDestIVType	= NEXUS_SecurityKeyIVType_eNoIV;
    encryptedSessionkey.operation		= NEXUS_SecurityOperation_eDecrypt;
    encryptedSessionkey.operationKey2	= NEXUS_SecurityOperation_eEncrypt;
    encryptedSessionkey.bASKMMode	= true;
#if 0
    encryptedSessionkey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
#else
    encryptedSessionkey.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
#endif
    encryptedSessionkey.sessionKeyOp = NEXUS_SecuritySessionKeyOp_eNoProcess;
    encryptedSessionkey.bSwapAESKey = false;

    encryptedSessionkey.cusKeyL 		= 0x00;
    encryptedSessionkey.cusKeyH 		= 0x00;
    encryptedSessionkey.cusKeyVarL	= 0x00;
    encryptedSessionkey.cusKeyVarH	= 0xFF;
    encryptedSessionkey.bRouteKey		= false;
    encryptedSessionkey.hwklDestAlg = algorithm.algorithm;
    encryptedSessionkey.rootKeySwap = false;

    if(SEC_R2R_NB_OF_PROTECTING_KEYS == 2)
        encryptedSessionkey.custSubMode =NEXUS_SecurityCustomerSubMode_eReserved20;
    else
        encryptedSessionkey.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;

    encryptedSessionkey.keyMode = NEXUS_SecurityKeyMode_eRegular;

    /* Perform key swap for proc-in. */
    BKNI_Memcpy(&(proc_in3[0][0]), &(pxL2CipheredProtectingKey[8]), sizeof(TUnsignedInt8)*8);
    BKNI_Memcpy(&(proc_in3[0][8]), &(pxL2CipheredProtectingKey[0]), sizeof(TUnsignedInt8)*8);

    BKNI_Memcpy(encryptedSessionkey.keyData, &(proc_in3[0][0]), sizeof(TUnsignedInt8)*16);

    if (NEXUS_Security_GenerateSessionKey(keyHandle, &encryptedSessionkey) !=0)
    {
        BDBG_ERR(("Loading session key failed"));
        retCode = SEC_ERROR;
        goto SEC_P_DONE;
    }
    BKNI_Memset((void *)&key4, 0, sizeof(key4));
    /* Load key4 */
    key4.keyladderID = NEXUS_SecurityKeyladderID_eA;
    key4.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
    key4.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
    key4.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;
    key4.bASKMMode	= true;
    key4.keyEntryType = NEXUS_SecurityKeyType_eOdd;
    key4.operation = NEXUS_SecurityOperation_eDecrypt;
#if 0
    key4.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
#else
    key4.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
#endif
    key4.hwklDestAlg = algorithm.algorithm;
    if(SEC_R2R_NB_OF_PROTECTING_KEYS == 2)
    {
        key4.bRouteKey = false;
        key4.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved20;
        key4.keySize = 16;
        /* Perform key swap for proc-in. */
        BKNI_Memcpy(&(proc_in3[1][0]), &(pxL1CipheredProtectingKey[8]), sizeof(TUnsignedInt8)*8);
        BKNI_Memcpy(&(proc_in3[1][8]), &(pxL1CipheredProtectingKey[0]), sizeof(TUnsignedInt8)*8);

        BKNI_Memcpy(key4.keyData,  &(proc_in3[1][0]), sizeof(TUnsignedInt8)*16);

    }
    else
    {
        key4.bRouteKey = true;
        key4.custSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
        key4.keySize = xCipheredContentKeySize;
        BKNI_Memcpy(key4.keyData,  pxCipheredContentKey, sizeof(TUnsignedInt8)*xCipheredContentKeySize);
    }

    key4.keyMode = NEXUS_SecurityKeyMode_eRegular;

    if (NEXUS_Security_GenerateControlWord(keyHandle, &key4))
    {
        BDBG_ERR(("Loading key4 failed for video ODD key"));
        retCode = SEC_ERROR;
        goto SEC_P_DONE;
    }

    if(SEC_R2R_NB_OF_PROTECTING_KEYS == 2)
    {
        /* Load key5 */
        BKNI_Memset((void *)&encrytedCW, 0, sizeof(encrytedCW));
        encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;
        encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
        encrytedCW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eHWKL;
        encrytedCW.hwklLen			= NEXUS_SecurityHWKLLenSelect_eLen3;
        encrytedCW.bASKMMode	= true;
        encrytedCW.keySize = xCipheredContentKeySize;
        encrytedCW.keyEntryType = NEXUS_SecurityKeyType_eOdd;

        BKNI_Memcpy(encrytedCW.keyData, pxCipheredContentKey, sizeof(TUnsignedInt8)*xCipheredContentKeySize);

        encrytedCW.operation = NEXUS_SecurityOperation_eDecrypt;
        encrytedCW.bRouteKey = true;
#if 0
        encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL1;
#else
        encrytedCW.virtualKeyLadderID = NEXUS_SecurityVirtualKeyladderID_eVKL4;
#endif
        encrytedCW.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved20;
        encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;
        encrytedCW.hwklDestAlg = algorithm.algorithm;

        if (NEXUS_Security_GenerateKey5(keyHandle, &encrytedCW))
        {
            BDBG_ERR(("Loading CW key failed for video ODD key"));
            retCode = SEC_ERROR;
            goto SEC_P_DONE;
        }
    }

    if(!xSession->bKeySet) {
        xSession->bKeySet = true;
        /* notify application that key has been set */
        if (xSession->xTransportSessionId != TRANSPORT_SESSION_ID_INVALID)
            brcmCaNotifyCipherSessionKeyLoaded(xSession->xTransportSessionId);
    }

    BDBG_MSG(("LEAVE: %s",__FUNCTION__));
    return SEC_NO_ERROR;

SEC_P_DONE:
    return (retCode);


}
 

void sec_P_AckCertKey(void)
{
    /* load a dummy key to consume the CERT key in the case there is a SEC level CERt op error */
    NEXUS_SecurityAlgorithmSettings	NexusConfig;
    NEXUS_SecurityEncryptedControlWord	CW;
    NEXUS_KeySlotHandle keyHandle = 0;
    NEXUS_SecurityKeySlotSettings keySettings;

    BDBG_MSG(("ENTER: %s", __FUNCTION__));

    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);

    NEXUS_Security_GetDefaultAlgorithmSettings(&NexusConfig);

    NexusConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
    NexusConfig.operation = NEXUS_SecurityOperation_eEncrypt;
    NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOddAndEven;
    NexusConfig.algorithm = NEXUS_SecurityAlgorithm_eAes;
    NexusConfig.algorithmVar = NEXUS_SecurityAlgorithmVariant_eEcb;
    NexusConfig.terminationMode = NEXUS_SecurityTerminationMode_eClear;
    NexusConfig.solitarySelect = NEXUS_SecuritySolitarySelect_eClear;
    NexusConfig.ivMode = NEXUS_SecurityIVMode_eRegular;
    NexusConfig.caVendorID=0x97ed;
    NexusConfig.otpId = NEXUS_SecurityOtpId_eOtpVal;
    NexusConfig.testKey2Select = false;
    NexusConfig.key2Select = NEXUS_SecurityKey2Select_eFixedKey;

    NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig);


    CW.keySize = 16;
    CW.keyEntryType = NEXUS_SecurityKeyType_eOddAndEven;
    CW.rootKeySwap = false;
    CW.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved21;
    CW.bASKMMode = true;
    CW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_ePKL;
    CW.bkeyGenBlocked = true;
    CW.keyGenCmdID = NEXUS_SecurityKeyGenCmdID_eKeyGen;
    CW.keyladderID = NEXUS_SecurityKeyladderID_eA;
    CW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen;
    CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
    CW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA;
    CW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
    CW.swizzleType = NEXUS_SecuritySwizzleType_eNone;
    CW.bASKMMode = true;
    CW.keyMode = NEXUS_SecurityKeyMode_eRegular;
    CW.operation = NEXUS_SecurityOperation_eEncrypt;
    CW.bSwapAESKey = false;
    CW.bRouteKey = true;
    CW.keySize = 16;
    CW.virtualKeyLadderID = 0x0;
    CW.sc01Polarity[NEXUS_SecurityPacketType_eGlobal] = 0x0;
    CW.sc01Polarity[NEXUS_SecurityPacketType_eRestricted] = 0x0;

    memset(CW.keyData, 0, sizeof(unsigned char) * 16 );

    NEXUS_Security_GenerateControlWord(keyHandle , &CW);


    NEXUS_Security_FreeKeySlot(keyHandle);
    BDBG_MSG(("LEAVE: %s",__FUNCTION__));
}

TSecStatus secUseCertKey
(
    TSecCipherSession   xSession,
    TUnsignedInt16      xEmi,
    size_t              xKeyIdSize,
    TUnsignedInt8*      pxKeyId
) {
    NEXUS_SecurityAlgorithmSettings	NexusConfig;
    NEXUS_SecurityKeySlotSettings	keySlotSettings;
    NEXUS_SecurityEncryptedControlWord	CW;
    NEXUS_KeySlotHandle keyHandle = 0;
    unsigned char useCaSecurityEngine = 0;
    NEXUS_Error errCode;
    BDBG_MSG(("Enter %s xSession=%p xEmi=0x%x", __FUNCTION__, xSession, xEmi));

    memset(&keySlotSettings, 0, sizeof(NEXUS_SecurityKeySlotSettings));
    memset(&CW, 0, sizeof(NEXUS_SecurityEncryptedControlWord));

    if(xSession== NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        /* ack the cert key */
        sec_P_AckCertKey();
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("xSession not valid")) ;
        /* ack the cert key */
        sec_P_AckCertKey();
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* check the xsessin type is valid */
    if((xSession->type != SEC_SESSION_STREAM_DECRYPT_TYPE) &&
       (xSession->type != SEC_SESSION_STREAM_ENCRYPT_TYPE) &&
       (xSession->type != SEC_SESSION_R2R_TYPE)) 
    {
        BDBG_ERR(("xSession type not valid")) ;
        /* ack the cert key */
        sec_P_AckCertKey();
        return SEC_ERROR_BAD_USAGE;
    }

    if (!brcmEmiIsValid(xEmi))
    {
        BDBG_ERR(("xEmi is not valid")) ;
        /* ack the cert key */
        sec_P_AckCertKey();
        return SEC_ERROR_BAD_EMI;
    }

    /* if R2R session check EMI is only 0x40xx type*/
    if(xSession->type == SEC_SESSION_R2R_TYPE) {
        BDBG_MSG(("SEC_SESSION_R2R_TYPE type xSession"));

        /* check Emi is valid */
        if(brcmEmiIsMpegTs(xEmi))
        {
            BDBG_ERR(("invalid xEmi for R2R session")) ;
            /* ack the cert key */
            sec_P_AckCertKey();
            return SEC_ERROR_BAD_EMI;
        }

        /*select M2M keyslot*/
        keyHandle = xSession->keyHandle;
    } else    /* ELSE if "stream" type check EMI is either 0x00XX or 0x40xx */
    if((xSession->type == SEC_SESSION_STREAM_DECRYPT_TYPE) || (xSession->type == SEC_SESSION_STREAM_ENCRYPT_TYPE)) {
        BDBG_MSG(("SEC_SESSION_STREAM type xSession")) ;

        /* Determine if it is DSC or SCR */
        if (SEC_NO_ERROR != brcmSecDetermineScrOrDsc_priv(xEmi, xSession)) {
            BDBG_ERR(("brcmSecDetermineScrOrDsc_priv failed"));
            /* ack the cert key */
            sec_P_AckCertKey();
            return SEC_ERROR;
        }

        /* Select SCR or DSC key */
        if(xSession->pDscKeyPath) {
            /* use DSC keyslot */
            BDBG_MSG(("Using CA keyslot")) ;
            keyHandle = csdBrcmGetDscKeyHandle(xSession->pDscKeyPath);
            useCaSecurityEngine = 1;
            /* check polarity is defined correctly */
            if(!xKeyIdSize || !pxKeyId) {
                BDBG_ERR(("Expect polarity for DSC EMI")) ;
                /* ack the cert key */
                sec_P_AckCertKey();
                return SEC_ERROR_BAD_USAGE;
            }
            if((*pxKeyId != 0) && (*pxKeyId != 1)) { /* 1 is odd 0 is even */
                BDBG_ERR(("Invalid polarity for DSC EMI")) ;
                /* ack the cert key */
                sec_P_AckCertKey();
                return SEC_ERROR_BAD_USAGE;
            }
        } else if(xSession->pScrKeyPath) {
            /* use SCR keyslot */
            BDBG_MSG(("Using M2M Stream keyslot")) ;
            keyHandle = csdBrcmGetScrKeyHandle((TCsdScrKeyPathHandle*)xSession->pScrKeyPath);
        } else {
            BDBG_MSG(("No valid key paths(!)"));
            /* ack the cert key */
            sec_P_AckCertKey();
            return SEC_ERROR;
        }
    }

    if (!brcmEmiSetupAlgorithm(xEmi, &NexusConfig))
    {
        BDBG_ERR(("%s: brcmEmiSetupAlgorithm failed\n", __FUNCTION__));
        /* ack the cert key */
        sec_P_AckCertKey();
        return SEC_ERROR;
    }

    if(useCaSecurityEngine) {
        BDBG_MSG(("%s Ca (DSC) Operation", __FUNCTION__));
        NexusConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eCa;
    } else {
        BDBG_MSG(("%s M2M (SCR) Operation", __FUNCTION__));
        NexusConfig.dest = NEXUS_SecurityAlgorithmConfigDestination_eMem2Mem;
    }

    if( xSession->bEncrypt) {
        BDBG_MSG(("%s Encrypt Operation", __FUNCTION__));
        NexusConfig.operation = NEXUS_SecurityOperation_eEncrypt;
    } else {
        BDBG_MSG(("%s Decrypt Operation", __FUNCTION__));
        NexusConfig.operation = NEXUS_SecurityOperation_eDecrypt;
    }

    if(useCaSecurityEngine) {
        if(*pxKeyId == 0) {
            BDBG_MSG(("%s alg conf EVEN key", __FUNCTION__));
            NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eEven;
        } else 
        {
            BDBG_MSG(("%s alg conf ODD key", __FUNCTION__));
            NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOdd;
        }
    } else {
        BDBG_MSG(("%s NEXUS_SecurityKeyType_eOddAndEven", __FUNCTION__));
        NexusConfig.keyDestEntryType = NEXUS_SecurityKeyType_eOddAndEven;
    }

    CW.keySize = brcmEmiGetKeySize(xEmi);
    xSession->xEmi = xEmi;

    NexusConfig.caVendorID=0x97ed;
    NexusConfig.otpId = NEXUS_SecurityOtpId_eOtpVal;
    NexusConfig.testKey2Select = false;
    NexusConfig.key2Select = NEXUS_SecurityKey2Select_eFixedKey;

    errCode = NEXUS_Security_ConfigAlgorithm(keyHandle, &NexusConfig);
    if (errCode != 0)
    {
        BDBG_MSG(("NEXUS_Security_ConfigAlgorithm(odd) errCode: %x", errCode ));
        /* ack the cert key */
        sec_P_AckCertKey();
        return SEC_ERROR_BAD_USAGE;
    }

    if(useCaSecurityEngine) {
        if(*pxKeyId == 0) {
            BDBG_MSG(("%s cw conf EVEN key", __FUNCTION__));
            CW.keyEntryType = NEXUS_SecurityKeyType_eEven;
        } else 
        {
            BDBG_MSG(("%s cw conf ODD key", __FUNCTION__));
            CW.keyEntryType = NEXUS_SecurityKeyType_eOdd;
        }
    } else {
        BDBG_MSG(("%s NEXUS_SecurityKeyType_eOddAndEven", __FUNCTION__));
        CW.keyEntryType = NEXUS_SecurityKeyType_eOddAndEven;
    }

    CW.rootKeySwap = false;
    CW.custSubMode = NEXUS_SecurityCustomerSubMode_eReserved21;
    CW.bASKMMode = true;
    CW.keyLadderSelect = NEXUS_SecurityKeyLadderSelect_ePKL;
    CW.bkeyGenBlocked = true;
    CW.keyGenCmdID = NEXUS_SecurityKeyGenCmdID_eKeyGen;
    CW.keyladderID = NEXUS_SecurityKeyladderID_eA;
    CW.keyGenCmdID   = NEXUS_SecurityKeyGenCmdID_eKeyGen;
    CW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
    CW.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA;
    CW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
    CW.swizzleType = NEXUS_SecuritySwizzleType_eNone;
    CW.bASKMMode = true;
    CW.keyMode = NEXUS_SecurityKeyMode_eRegular;
    if( xSession->bEncrypt) {
        CW.operation = NEXUS_SecurityOperation_eEncrypt;
    }else {
        CW.operation = NEXUS_SecurityOperation_eDecrypt;
    }
    CW.bSwapAESKey = false;
    CW.bRouteKey = true;
    CW.keySize = 16;
    CW.virtualKeyLadderID = 0x0;
    CW.sc01Polarity[NEXUS_SecurityPacketType_eGlobal] = 0x0;
    CW.sc01Polarity[NEXUS_SecurityPacketType_eRestricted] = 0x0;

    memset(CW.keyData, 0, sizeof(unsigned char) * 16 );

    errCode = NEXUS_Security_GenerateControlWord(keyHandle , &CW);
    if (errCode != 0)
    {
        BDBG_ERR(("NEXUS_Security_GenerateControlWord errCode: 0x%x", errCode ));
        return SEC_ERROR_BAD_USAGE;
    }

    /* load IV key if needed */
    if( !xSession->bEncrypt) 
    {
        size_t xIvSize = brcmEmiGetIvSize(xEmi);
        if ((xIvSize > 0) && brcmEmiIsZeroIv(xEmi))
        {
            NEXUS_SecurityClearKey ivKey;

            BDBG_MSG(("%s Loading null IV", __FUNCTION__));

            BKNI_Memset((void *)&ivKey, 0, sizeof(ivKey));
            NEXUS_Security_GetDefaultClearKey(&ivKey);
            ivKey.keyIVType = NEXUS_SecurityKeyIVType_eIV;
            ivKey.keyEntryType = NEXUS_SecurityKeyType_eIv;
            ivKey.keySize = 16;

            errCode = NEXUS_Security_LoadClearKey(keyHandle , &ivKey);
            if (errCode != 0)
            {
                BDBG_ERR(("Load IV failed"));
                return SEC_ERROR_BAD_USAGE;
            }
        }
    }

    if(!xSession->bKeySet) {
        xSession->bKeySet = true;
        /* notify application that key has been set */
        brcmCaNotifyCipherSessionKeyLoaded(xSession->xTransportSessionId);
    } 

    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}


#define AON_OFFSET 0x354
TSecStatus secUseFlashProtKey
(
    TSecCipherSession   xSession,
    TUnsignedInt16      xEmi)
{
    unsigned char fekKey[16] = {0};
    unsigned int i;
    unsigned int regVal;
    TSecStatus errCode;

    BDBG_MSG(("Enter %s xSession=%p xEmi=0x%x", __FUNCTION__, xSession, xEmi));

    if(xSession == NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("xSession not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(xSession->keyHandle == NULL) { /* not a valid ram2ram session */
        BDBG_ERR(("%s keyHandle error", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    regVal= 0;
    for(i=0; i<16; i+=4) {
        NEXUS_Platform_ReadRegister(BCHP_AON_CTRL_RESET_CTRL+0x354+i,&regVal);
        fekKey[i] = (regVal>>24)&0xff;
        fekKey[i+1] = (regVal>>16)&0xff;
        fekKey[i+2] = (regVal>>8)&0xff;
        fekKey[i+3] = (regVal)&0xff;

    }
    /*for(i=0; i<16; i++){printf("fek[%d]=%x\n", i, fekKey[i]); }*/
    errCode=  secSetClearTextKey(xSession,xEmi,0,0,16,fekKey);
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return errCode;
}


TSecStatus secEncryptFlashProtKey
(
    const TUnsignedInt8*     pxInput,
    TUnsignedInt8*     pxOutput,
    size_t              xSize
) {
    NEXUS_Error errCode;
    unsigned int inBuffer[16], outBuffer[16], actualOutSize;
    NEXUS_SecurityKeySlotInfo keyInfo;
    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_KeySlotHandle keyHandle=NULL;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySettings);


    if( keyHandle == NULL)
    {
        BDBG_ERR(("%s keyHandle error", __FUNCTION__));
        return  SEC_ERROR;  /* return if no keyslot is available*/
    }

    if((pxInput == NULL)||(pxOutput == NULL) || (xSize!=16)) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER error", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }
    NEXUS_Security_GetKeySlotInfo(keyHandle, &keyInfo);

    inBuffer[0] = 0x00000010;
    inBuffer[1] = 0x00000022;
    inBuffer[2] = 0xabcdef00;
    inBuffer[3] = 0xde55aa21;
    inBuffer[4] = 0x789a0008;
    inBuffer[5] = keyInfo.keySlotNumber;
    inBuffer[6] = 1; /* encrypt */


    errCode = NEXUS_Security_SubmitRawCommand (
                  inBuffer,	  /* attr{nelem=sizeInBuffer;reserved=90} */
                  7, 	   /* size of pInBuffer in integers */
                  outBuffer, 		/* [out] attr{nelem=sizeOutBuffer;reserved=90} */
                  6, 	   /* size of pOutBuffer in integers */
                  &actualOutSize 	 /* [out] number of integers written into pOutBuffer */
              );

    if ( errCode || (outBuffer[5]!=0 ))
    {
        BDBG_ERR(("%s NEXUS_Security_SubmitRawCommand error", __FUNCTION__));
        return SEC_ERROR;
    }
    errCode = secR2RCryptoOperation(keyHandle , pxInput, pxOutput, xSize);

    NEXUS_Security_FreeKeySlot(keyHandle);

    BDBG_MSG(("Leave %s", __FUNCTION__));
    return errCode;
}

TSecStatus sec_P_SessionCcryptoOperation
(
    TSecCipherSession   xSession,
    const TUnsignedInt8*     pxInput,
    TUnsignedInt8*     pxOutput,
    size_t              xMessageSize,
    const TUnsignedInt8*     pxInitVector,
    size_t              xInitVectorSize)
{

    NEXUS_SecurityClearKey key;
    unsigned int xEmi = xSession->xEmi;
    size_t requiredIvSize;

    BDBG_MSG(("Enter %s xEmi 0x%x", __FUNCTION__, xEmi));

    if(xSession == NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("xSession not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(xSession->keyHandle == NULL) {
        BDBG_ERR(("R2R key is not loaded!\n"));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(pxInput == NULL || pxOutput == NULL ) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER 1", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }
    /* load IV key if needed */
    requiredIvSize = brcmEmiGetIvSize(xEmi);
    if (requiredIvSize > 0)
    {
        const TUnsignedInt8 xZeroIv[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        const TUnsignedInt8 *pxIv = pxInitVector;
        size_t xIvSize = xInitVectorSize;

        BDBG_MSG(("%s IV load needed ", __FUNCTION__));
        if (brcmEmiIsZeroIv(xEmi))
        {
            pxIv = xZeroIv;
            xIvSize = requiredIvSize;
        }

        if((pxIv == NULL) || (requiredIvSize != xIvSize)) {
            BDBG_ERR(("%s IV error", __FUNCTION__));
            return SEC_ERROR_BAD_PARAMETER;
        }

        BKNI_Memset((void *)&key, 0, sizeof(key));
        NEXUS_Security_GetDefaultClearKey(&key);
        key.keyIVType	= NEXUS_SecurityKeyIVType_eIV;
        key.keyEntryType = NEXUS_SecurityKeyType_eIv;
        key.keySize = 16;

        if(xIvSize == 8) {
            BKNI_Memcpy(&(key.keyData[8]), pxIv, xIvSize);
        }
        else
            BKNI_Memcpy(key.keyData, pxIv, xIvSize);
        /* Load IV. */
        if (NEXUS_Security_LoadClearKey(xSession->keyHandle , &key) != 0)
        {
            BDBG_WRN(("Load IV failed"));
            return SEC_ERROR;
        }
    }
    if(secR2RCryptoOperation(xSession->keyHandle , pxInput, pxOutput, xMessageSize)) {
        BDBG_ERR(("%s secR2RCryptoOperation error", __FUNCTION__));
        return SEC_ERROR;
    }
    BDBG_MSG(("LEAVE: %s",__FUNCTION__));
    return SEC_NO_ERROR;
}

TSecStatus secSessionEncrypt
(
    TSecCipherSession   xSession,
    const TUnsignedInt8*     pxInput,
    TUnsignedInt8*     pxOutput,
    size_t              xMessageSize,
    const TUnsignedInt8*     pxInitVector,
    size_t              xInitVectorSize)
{
    TSecStatus retVal;
    BDBG_MSG(("Enter %s  xSession %p pxInput %p pxOutput %p xMessageSize %d pxInitVector %p xInitVectorSize %d", __FUNCTION__, xSession, pxInput, pxOutput, xMessageSize, pxInitVector, xInitVectorSize));

    if(xSession == NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("xSession not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(!xSession->bEncrypt) {
        BDBG_ERR(("trying to encrypt with a decrypt xsession")) ;
        return SEC_ERROR_BAD_USAGE;
    }
    if(!xSession->bKeySet) {
        BDBG_ERR(("key not set on xSession %p", xSession)) ;
        return SEC_ERROR;
    }

    retVal = sec_P_SessionCcryptoOperation(xSession,pxInput,pxOutput,xMessageSize,pxInitVector, xInitVectorSize);
    BDBG_MSG(("LEAVE: %s",__FUNCTION__));
    return retVal;
}

TSecStatus secSessionDecrypt
(
    TSecCipherSession   xSession,
    const TUnsignedInt8*     pxInput,
    TUnsignedInt8*     pxOutput,
    size_t              xMessageSize,
    const TUnsignedInt8*     pxInitVector,
    size_t              xInitVectorSize
) {
    TSecStatus retVal;
    BDBG_MSG(("Enter %s  xSession %p pxInput %p pxOutput %p xMessageSize %d pxInitVector %p xInitVectorSize %d", __FUNCTION__, xSession, pxInput, pxOutput, xMessageSize, pxInitVector, xInitVectorSize));

    if(xSession == NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("xSession not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(xSession->bEncrypt) {
        BDBG_ERR(("trying to decrypt with an encrypt session")) ;
        return SEC_ERROR_BAD_USAGE;
    }
    if(!xSession->bKeySet) {
        BDBG_ERR(("key not set on xSession %p", xSession)) ;
        return SEC_ERROR;
    }

    retVal = sec_P_SessionCcryptoOperation(xSession,pxInput,pxOutput,xMessageSize,pxInitVector, xInitVectorSize);
    BDBG_MSG(("LEAVE: %s",__FUNCTION__));
    return retVal;
}

TSecStatus secEncryptData
(
    TUnsignedInt8*  pxOutput,
    const TUnsignedInt8*  pxInput,
    TSize           xDataSize
) {

    TSecCipherSession r2rSession;
    const TUnsignedInt8 CONSTANT[16] = {0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
                                        0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e
                                       };
    TUnsignedInt16      xEmi = CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR;
    TSecStatus errcode = SEC_NO_ERROR;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(pxInput == NULL || pxOutput == NULL) {
        BDBG_ERR(("%s SEC_ERROR", __FUNCTION__));
        return SEC_ERROR;
    }

    if(!xDataSize || (xDataSize % 8)) {
        BDBG_ERR(("%s chunk size error ", __FUNCTION__));
        return SEC_ERROR;
    }

    if(secOpenRam2RamEncryptSession(&r2rSession)){
        BDBG_ERR(("%s secOpenRam2RamEncryptSession Failed", __FUNCTION__));
        return SEC_ERROR;
    }

    if(secSet2LevelProtectedKey(r2rSession, xEmi,0,0,sizeof(CONSTANT), CONSTANT,
                                sizeof(CONSTANT), CONSTANT,CONSTANT )!=SEC_NO_ERROR) {
        BDBG_ERR(("%s secSet2LevelProtectedKey Failed", __FUNCTION__));
        return SEC_ERROR;

    }
    errcode = secSessionEncrypt(r2rSession, pxInput, pxOutput, xDataSize, NULL, 0);
    if(errcode) {
        BDBG_ERR(("%s secSessionEncrypt Failed", __FUNCTION__));
        return errcode;
    }

    errcode = secCloseSession(r2rSession);
    if(errcode) {
        BDBG_ERR(("%s secCloseSession Failed", __FUNCTION__));
        return errcode;
    }
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

TSecStatus secDecryptData
(
    TUnsignedInt8*  pxOutput,
    const TUnsignedInt8*  pxInput,
    TSize           xDataSize
) {
    TSecCipherSession r2rSession;
    const TUnsignedInt8 CONSTANT[16] = {0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
                                        0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e
                                       };
    TUnsignedInt16      xEmi = CSD_Algorithm_RAW_TDES_ABA_ECB_CLEAR;
    TSecStatus errcode = SEC_NO_ERROR;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(pxInput == NULL || pxOutput == NULL) {
        BDBG_ERR(("%s SEC_ERROR", __FUNCTION__));
        return SEC_ERROR;
    }

    if(!xDataSize || (xDataSize % 8)) {
        BDBG_ERR(("%s chunk size error ", __FUNCTION__));
        return SEC_ERROR;
    }

    if(secOpenRam2RamDecryptSession(&r2rSession)) {
        BDBG_ERR(("%s secOpenRam2RamDecryptSession failed", __FUNCTION__));
        return SEC_ERROR;
    }

    if(secSet2LevelProtectedKey(r2rSession, xEmi,0,0, sizeof(CONSTANT),CONSTANT,
                                sizeof(CONSTANT), CONSTANT,CONSTANT )!=SEC_NO_ERROR) {
        BDBG_ERR(("%s secSet2LevelProtectedKey failed", __FUNCTION__));
        return SEC_ERROR;
    }
    errcode = secSessionDecrypt(r2rSession, pxInput, pxOutput, xDataSize, NULL, 0);
    if(errcode) {
        BDBG_ERR(("%s secSessionDecrypt failed", __FUNCTION__));
        return errcode;
    }
#ifdef SEC_DEBUG
    for(i=0; i<16; i++) printf("%x, ", pxInput[i]);
    printf("\n");
    for(i=0; i<16; i++) printf("%x, ", pxOutput[i]);
#endif
    errcode = secCloseSession(r2rSession);
    if(errcode) {
        BDBG_ERR(("%s secCloseSession failed", __FUNCTION__));
        return errcode;
    }
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

TSecStatus secSha1Init
(
    TSecHashContext*    pxContext
) {
    TSecHashContext shaCtx;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(!gSHAMutexLock) {
        if(BKNI_CreateMutex( &gSHAMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SEC_ERROR;
        }
    }

    if(pxContext == NULL ) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }
    shaCtx = BKNI_Malloc(sizeof(SSecHashContext));
    if(shaCtx == NULL) {
        BDBG_ERR(("Not enough memory!\n"));
        return SEC_ERROR;
    }
    BKNI_Memset(shaCtx, 0, sizeof(SSecHashContext));
    BDBG_MSG(("%s context pointer = %p", __FUNCTION__, shaCtx));

#ifdef USE_OSSL_FOR_SHA

    if(SHA1_Init(&shaCtx->c)!=1) return SEC_ERROR;
    *pxContext = shaCtx;
#else

    /* Unfortunately at present NEXUS SHA does not support multiple contexts
       so muti-threaded SHA operations are going to be a problem.
       The only solution it to cache updates and submit all the buffers in the
       finalize call under lock (nexus thunking wont work as will be calling
       NEXUS_HMACSHA_PerformOp multiple times.*/

    NEXUS_HMACSHA_GetDefaultOpSettings(&(shaCtx->hmacSha1Settings));
    shaCtx->hmacSha1Settings.shaType = NEXUS_HMACSHA_SHAType_eSha160;
    shaCtx->hmacSha1Settings.opMode = NEXUS_HMACSHA_Op_eSHA;
    shaCtx->hmacSha1Settings.byteSwap= true;
#endif

    *pxContext = shaCtx;
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

TSecStatus secSha1Update
(
    TSecHashContext  xContext,
    const TUnsignedInt8*  pxMessageChunk,
    size_t           xChunkSize
) {
    NEXUS_Error rc;
    BDBG_MSG(("Enter %s xContext %p pxMessageChunk %p xChunkSize %d", __FUNCTION__, xContext, pxMessageChunk, xChunkSize));

    if(xContext == NULL || ((pxMessageChunk == NULL) && (xChunkSize > 0))) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* spec required 0 chunk size is processed: nothing to do */
    if((pxMessageChunk == NULL) && (xChunkSize == 0)) {
        BDBG_MSG(("%s 0 chunk size", __FUNCTION__));
        return SEC_NO_ERROR;
    }

#if USE_OSSL_FOR_SHA
    if(SHA1_Update(&xContext->c, pxMessageChunk, xChunkSize)!=1) return SEC_ERROR;
#else


    /* allocate new chunk struct*/
    BDBG_MSG(("%s allocating new chunk at position %d size = %d", __FUNCTION__,xContext->numberofChunks, xChunkSize));
    writeIt((unsigned char *)pxMessageChunk,xChunkSize);
    if(!xContext->pMessageChunkList) { /* first chunk */
        BDBG_MSG(("%s first chunk", __FUNCTION__));
        xContext->pMessageChunkList = BKNI_Malloc(sizeof(SMessageChunk));
        if(!xContext->pMessageChunkList)
        {
            BDBG_ERR(("%s Fail - out of memory", __FUNCTION__));
            return SEC_NO_ERROR;
        }
        /* allocate chunk mem and copy buffer */
        rc = NEXUS_Memory_Allocate(xChunkSize, NULL, (void *)&xContext->pMessageChunkList->pMessageChunk);
        if (rc || !xContext->pMessageChunkList->pMessageChunk) {
            rc = BERR_TRACE(rc); 
            BDBG_ERR(("%s failed to allocate %d bytes", __FUNCTION__,xChunkSize)) ;
            return SEC_NO_ERROR;
        }
        xContext->pMessageChunkList->next=0;
        xContext->pMessageChunkList->xChunkSize=xChunkSize;
        BKNI_Memcpy((void *)xContext->pMessageChunkList->pMessageChunk, (void *)pxMessageChunk, xChunkSize );
        xContext->totalDataSize += xChunkSize;
        xContext->numberofChunks++; 
    } else /* traverse list */
    {
        int count =1; /* belt and braces */
        SMessageChunk *pChunk = xContext->pMessageChunkList;

        while (pChunk->next) {
            count++;
            pChunk = pChunk->next;
        }

        BDBG_ASSERT(count == xContext->numberofChunks);

        BDBG_MSG(("%s next chunk %d", __FUNCTION__, count+1));

        pChunk->next = BKNI_Malloc(sizeof(SMessageChunk));
        if(!pChunk->next)
        {
            BDBG_ERR(("%s Fail - out of memory", __FUNCTION__));
            return SEC_NO_ERROR;
        }
        pChunk = pChunk->next;

        /* allocate chunk mem and copy buffer */
        rc = NEXUS_Memory_Allocate(xChunkSize, NULL, (void *)&pChunk->pMessageChunk);
        if (rc || !pChunk->pMessageChunk) {
            rc = BERR_TRACE(rc); 
            BDBG_ERR(("%s failed to allocate %d bytes", __FUNCTION__,xChunkSize)) ;
            return SEC_NO_ERROR;
        }
        pChunk->next=0;
        pChunk->xChunkSize=xChunkSize;
        BKNI_Memcpy((void *)pChunk->pMessageChunk, (void *)pxMessageChunk, xChunkSize );
        xContext->totalDataSize += xChunkSize;
        xContext->numberofChunks++; 
    }

    BDBG_MSG(("%s updated context total bytes = %d %d", __FUNCTION__, xContext->totalDataSize));

#endif
    BDBG_MSG(("Leave %s xContext %p", __FUNCTION__, xContext));
    return SEC_NO_ERROR;

}

TSecStatus secSha1Final
(
    TSecHashContext  xContext,
    TUnsignedInt8*  pxMessageDigest
) {
    NEXUS_Error rc;
    TUnsignedInt8*  pSubmitBuffer = 0;
    TUnsignedInt16  submitBufferSize = 0;
    SMessageChunk *pChunk = 0;
    NEXUS_HMACSHA_DigestOutput hmacOut;
    BDBG_MSG(("Enter %s xContext %p pxMessageDigest %p", __FUNCTION__, xContext, pxMessageDigest));

    if(xContext == NULL || pxMessageDigest == NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* if there has never been an update still return success but digest buffer shall equal to "ER_NULL_SHA1"*/
    if(!xContext->numberofChunks || !xContext->pMessageChunkList) {
        unsigned char er_null_sha1[20] = {0xDA,0x39,0xA3,0xEE,0x5E,0x6B,0x4B,0x0D,0x32,0x55,0xBF,0xEF,0x95,0x60,0x18,0x90,
            0xAF,0xD8,0x07,0x09 };
        BDBG_MSG(("%s no update has been called for this context returning ER_NULL_SHA1", __FUNCTION__));
        BKNI_Memcpy(pxMessageDigest, er_null_sha1, 20);
        BDBG_MSG(("Leave %s", __FUNCTION__));
        return SEC_NO_ERROR;
    }

#if USE_OSSL_FOR_SHA
    if(SHA1_Final(pxMessageDigest, &xContext->c)!=1) return SEC_ERROR;
#else

   /* Nagra expects to be able to update buffers of any size
      but in chunk mode BHSM only allows buffers which are multiples of 64 (other than last buffer).
      So my solution is a scatter gather approach - gather all the updates into a single buffer and
      do the SHA in one go. Of course the disadvantage is the requirement for one large linear buffer
      which if the client is hashing a particularaly large block of data may cause mem problems such
      as: lack of mem or fragmentation preventing large buffer allocation.*/
    

    rc = NEXUS_Memory_Allocate(xContext->totalDataSize, NULL, (void *)&pSubmitBuffer);
    if (rc || !pSubmitBuffer) {
        rc = BERR_TRACE(rc); 
        BDBG_ERR(("%s failed to allocate %d bytes for submit buffer", __FUNCTION__,xContext->totalDataSize)) ;
        return SEC_NO_ERROR;
    }

    pChunk = xContext->pMessageChunkList;

    /* gather all the buffers into one submit */
    while(pChunk) {
        BKNI_Memcpy(pSubmitBuffer+submitBufferSize, pChunk->pMessageChunk, pChunk->xChunkSize );
        submitBufferSize += pChunk->xChunkSize;
        BDBG_MSG(("%s Added %dbytes to submit buffer total size now %d", __FUNCTION__, pChunk->xChunkSize, submitBufferSize));
        pChunk = pChunk->next;
    }

    /* belt and braces */
    BKNI_AcquireMutex(gSHAMutexLock);

    xContext->hmacSha1Settings.dataMode = NEXUS_HMACSHA_DataMode_eAllIn;
    xContext->hmacSha1Settings.dataAddress = pSubmitBuffer;
    xContext->hmacSha1Settings.dataSize = submitBufferSize;
    writeIt((unsigned char*)pSubmitBuffer,submitBufferSize);
    if(NEXUS_HMACSHA_PerformOp(&(xContext->hmacSha1Settings), &hmacOut)) {
        BKNI_ReleaseMutex(gSHAMutexLock);
        BDBG_ERR(("%s NEXUS_HMACSHA_PerformOp Failed", __FUNCTION__));
        return SEC_ERROR;
    }

    BDBG_MSG(("%s Successfully Submitted SHA1 resquest to BSP", __FUNCTION__));

    BKNI_Memcpy(pxMessageDigest, hmacOut.digestData, hmacOut.digestSize);

    writeIt((unsigned char*)pxMessageDigest,hmacOut.digestSize);
    BKNI_ReleaseMutex(gSHAMutexLock);
    /* clean up */
    if(xContext->pMessageChunkList) {
        SMessageChunk *pNextChunk = 0;

        pChunk = xContext->pMessageChunkList;
        /* traverse list freeing buffers */
        while(pChunk->next) {
            /* save next */
            pNextChunk = pChunk->next;

            /* free chunk buffer */
            BDBG_ASSERT(pChunk->pMessageChunk);
            NEXUS_Memory_Free((void *)pChunk->pMessageChunk);

            /* free stuct */
            BKNI_Free(pChunk);
            BDBG_MSG(("%s removed chunk %d", __FUNCTION__, xContext->numberofChunks));
            xContext->numberofChunks--;
            pChunk = pNextChunk;
        }

        /* free chunk buffer */
        BDBG_ASSERT(pChunk->pMessageChunk);
        NEXUS_Memory_Free((void *)pChunk->pMessageChunk);

        /* free stuct */
        BKNI_Free(pChunk);
    }

    NEXUS_Memory_Free(pSubmitBuffer);

    BKNI_Free(xContext);

#endif
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

TSecStatus secSha256Init
(
    TSecHashContext*    pxContext
) {
    TSecHashContext shaCtx;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(!gSHAMutexLock) {
        if(BKNI_CreateMutex( &gSHAMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SEC_ERROR;
        }
    }

    if(pxContext == NULL ) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }
    shaCtx = BKNI_Malloc(sizeof(SSecHashContext));
    if(shaCtx == NULL) {
        BDBG_ERR(("Not enough memory!\n"));
        return SEC_ERROR;
    }
    BKNI_Memset(shaCtx, 0, sizeof(SSecHashContext));
    BDBG_MSG(("%s context pointer = %p", __FUNCTION__, shaCtx));

#if 0
    if(SHA256_Init(&shaCtx->c256)!=1) return SEC_ERROR;
#else

    NEXUS_HMACSHA_GetDefaultOpSettings(&(shaCtx->hmacSha1Settings));
    shaCtx->hmacSha1Settings.shaType = NEXUS_HMACSHA_SHAType_eSha256;
    shaCtx->hmacSha1Settings.opMode = NEXUS_HMACSHA_Op_eSHA;
    shaCtx->hmacSha1Settings.byteSwap= true;
#endif
    *pxContext = shaCtx;
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

TSecStatus secSha256Update
(
    TSecHashContext  xContext,
    const TUnsignedInt8*  pxMessageChunk,
    size_t           xChunkSize
) {
    NEXUS_Error rc;
    BDBG_MSG(("Enter %s xContext %p pxMessageChunk %p xChunkSize %d", __FUNCTION__, xContext, pxMessageChunk, xChunkSize));

    if(xContext == NULL || ((pxMessageChunk == NULL) && (xChunkSize > 0))) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* spec required 0 chunk size is processed: nothing to do */
    if((pxMessageChunk == NULL) && (xChunkSize == 0)) {
        BDBG_ERR(("%s 0 chunk size", __FUNCTION__));
        return SEC_NO_ERROR;
    }

#if USE_OSSL_FOR_SHA
    if(SHA256_Update(&xContext->c, pxMessageChunk, xChunkSize)!=1) return SEC_ERROR;
#else

    /* allocate new chunk struct*/
    BDBG_MSG(("%s allocating new chunk at position %d size = %d", __FUNCTION__,xContext->numberofChunks, xChunkSize));
    if(!xContext->pMessageChunkList) { /* first chunk */
        BDBG_MSG(("%s first chunk", __FUNCTION__));
        xContext->pMessageChunkList = BKNI_Malloc(sizeof(SMessageChunk));
        if(!xContext->pMessageChunkList)
        {
            BDBG_ERR(("%s Fail - out of memory", __FUNCTION__));
            return SEC_NO_ERROR;
        }
        /* allocate chunk mem and copy buffer */
        rc = NEXUS_Memory_Allocate(xChunkSize, NULL, (void *)&xContext->pMessageChunkList->pMessageChunk);
        if (rc || !xContext->pMessageChunkList->pMessageChunk) {
            rc = BERR_TRACE(rc); 
            BDBG_ERR(("%s failed to allocate %d bytes", __FUNCTION__,xChunkSize)) ;
            return SEC_NO_ERROR;
        }
        xContext->pMessageChunkList->next=0;
        xContext->pMessageChunkList->xChunkSize=xChunkSize;
        BKNI_Memcpy((void *)xContext->pMessageChunkList->pMessageChunk, (void *)pxMessageChunk, xChunkSize );
        xContext->totalDataSize += xChunkSize;
        xContext->numberofChunks++; 
    } else /* traverse list */
    {
        int count =1; /* belt and braces */
        SMessageChunk *pChunk = xContext->pMessageChunkList;

        while (pChunk->next) {
            count++;
            pChunk = pChunk->next;
        }

        BDBG_ASSERT(count == xContext->numberofChunks);

        BDBG_MSG(("%s next chunk %d", __FUNCTION__, count+1));

        pChunk->next = BKNI_Malloc(sizeof(SMessageChunk));
        if(!pChunk->next)
        {
            BDBG_ERR(("%s Fail - out of memory", __FUNCTION__));
            return SEC_NO_ERROR;
        }
        pChunk = pChunk->next;

        /* allocate chunk mem and copy buffer */
        rc = NEXUS_Memory_Allocate(xChunkSize, NULL, (void *)&pChunk->pMessageChunk);
        if (rc || !pChunk->pMessageChunk) {
            rc = BERR_TRACE(rc); 
            BDBG_ERR(("%s failed to allocate %d bytes", __FUNCTION__,xChunkSize)) ;
            return SEC_NO_ERROR;
        }
        pChunk->next=0;
        pChunk->xChunkSize=xChunkSize;
        BKNI_Memcpy((void *)pChunk->pMessageChunk, (void *)pxMessageChunk, xChunkSize );
        xContext->totalDataSize += xChunkSize;
        xContext->numberofChunks++; 
    }

    BDBG_MSG(("%s updated context total bytes = %d %d", __FUNCTION__, xContext->totalDataSize));

#endif
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;

}

TSecStatus secSha256Final
(
    TSecHashContext  xContext,
    TUnsignedInt8*  pxMessageDigest
) {
    NEXUS_Error rc;
    TUnsignedInt8*  pSubmitBuffer = 0;
    TUnsignedInt16  submitBufferSize = 0;
    SMessageChunk *pChunk = 0;
    NEXUS_HMACSHA_DigestOutput hmacOut;
    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(xContext == NULL || pxMessageDigest == NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }


    /* if there has never been an update still return success but digest buffer shall equal to "ER_NULL_SHA256"*/
    if(!xContext->numberofChunks || !xContext->pMessageChunkList) {
        unsigned char er_null_sha256[32] = {0xE3,0xB0,0xC4,0x42,0x98,0xFC,0x1C,0x14,0x9A,0xFB,0xF4,0xC8,0x99,0x6F,0xB9,0x24
        ,0x27,0xAE,0x41,0xE4,0x64,0x9B,0x93,0x4C,0xA4,0x95,0x99,0x1B,0x78,0x52,0xB8,0x55 };
        BDBG_MSG(("%s no update has been called for this context returning ER_NULL_SHA256", __FUNCTION__));
        BKNI_Memcpy(pxMessageDigest, er_null_sha256, 32);
        BDBG_MSG(("Leave %s", __FUNCTION__));
        return SEC_NO_ERROR;
    }

#if USE_OSSL_FOR_SHA
    if(SHA256_Final(pxMessageDigest, &xContext->c)!=1) return SEC_ERROR;
#else

   /* Nagra expects to be able to update buffers of any size
      but in chunk mode BHSM only allows buffers which are multiples of 64 (other than last buffer).
      So my solution is a scatter gather approach - gather all the updates into a single buffer and
      do the SHA in one go. Of course the disadvantage is the requirement for one large linear buffer
      which if the client is hashing a particularaly large block of data may cause mem problems such
      as: lack of mem or fragmentation preventing large buffer allocation.*/
    

    rc = NEXUS_Memory_Allocate(xContext->totalDataSize, NULL, (void *)&pSubmitBuffer);
    if (rc || !pSubmitBuffer) {
        rc = BERR_TRACE(rc); 
        BDBG_ERR(("%s failed to allocate %d bytes for submit buffer", __FUNCTION__,xContext->totalDataSize)) ;
        return SEC_NO_ERROR;
    }

    pChunk = xContext->pMessageChunkList;

    /* gather all the buffers into one submit */
    while(pChunk) {
        BKNI_Memcpy(pSubmitBuffer+submitBufferSize, pChunk->pMessageChunk, pChunk->xChunkSize );
        submitBufferSize += pChunk->xChunkSize;
        BDBG_MSG(("%s Added %dbytes to submit buffer total size now %d", __FUNCTION__, pChunk->xChunkSize, submitBufferSize));
        pChunk = pChunk->next;
    }

    /* belt and braces */
    BKNI_AcquireMutex(gSHAMutexLock);

    xContext->hmacSha1Settings.dataMode = NEXUS_HMACSHA_DataMode_eAllIn;
    xContext->hmacSha1Settings.dataAddress = pSubmitBuffer;
    xContext->hmacSha1Settings.dataSize = submitBufferSize;
    if(NEXUS_HMACSHA_PerformOp(&(xContext->hmacSha1Settings), &hmacOut)) {
        BKNI_ReleaseMutex(gSHAMutexLock);
        BDBG_ERR(("%s NEXUS_HMACSHA_PerformOp Failed", __FUNCTION__));
        return SEC_ERROR;
    }

    BKNI_ReleaseMutex(gSHAMutexLock);
    BDBG_MSG(("%s Successfully Submitted SHA256 resquest to BSP", __FUNCTION__));

    BKNI_Memcpy(pxMessageDigest, hmacOut.digestData, hmacOut.digestSize);

    /* clean up */
    if(xContext->pMessageChunkList) {
        SMessageChunk *pNextChunk = 0;

        pChunk = xContext->pMessageChunkList;
        /* traverse list freeing buffers */
        while(pChunk->next) {
            /* save next */
            pNextChunk = pChunk->next;

            /* free chunk buffer */
            BDBG_ASSERT(pChunk->pMessageChunk);
            NEXUS_Memory_Free((void *)pChunk->pMessageChunk);

            /* free stuct */
            BKNI_Free(pChunk);
            BDBG_MSG(("%s removed chunk %d", __FUNCTION__, xContext->numberofChunks));
            xContext->numberofChunks--;
            pChunk = pNextChunk;
        }

        /* free chunk buffer */
        BDBG_ASSERT(pChunk->pMessageChunk);
        NEXUS_Memory_Free((void *)pChunk->pMessageChunk);

        /* free stuct */
        BKNI_Free(pChunk);
    }

    NEXUS_Memory_Free(pSubmitBuffer);

    BKNI_Free(xContext);

#endif
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

#ifndef NO_C99
TSecStatus secSha384Init
(
    TSecHashContext*    pxContext
) {
    TSecHashContext shaCtx;
    hash_state *md;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(pxContext == NULL ) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }
    shaCtx = malloc(sizeof(SSecHashContext));

    md = malloc(sizeof(hash_state)); /* where is this freed??*/
    if(!md) {
        BDBG_ERR(("%s md allocation error", __FUNCTION__));
        return SEC_ERROR;
    }
#if 1
#if 0   /* openssl */
    if(SHA384_Init(&shaCtx->c384)!=1) return SEC_ERROR;
#else     /* libtm */

    shaCtx->md = md;
    register_hash(&sha384_desc);
    if(sha384_init(shaCtx->md)) {
        BDBG_ERR(("%s sha384_init failed", __FUNCTION__));
        return SEC_ERROR;
    }
#endif
#else

    NEXUS_HMACSHA_GetDefaultOpSettings(&(shaCtx->hmacSha1Settings));
    shaCtx->hmacSha1Settings.shaType = NEXUS_HMACSHA_SHAType_eSha256;
    shaCtx->hmacSha1Settings.opMode = NEXUS_HMACSHA_Op_eSHA;
    shaCtx->hmacSha1Settings.byteSwap= true;
#endif
    *pxContext = shaCtx;
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}
#endif

#ifndef NO_C99
TSecStatus secSha384Update
(
    TSecHashContext  xContext,
    const TUnsignedInt8*  pxMessageChunk,
    size_t           xChunkSize
) {

    BDBG_MSG(("Enter %s xContext %p pxMessageChunk %p xChunkSize %d", __FUNCTION__, xContext, pxMessageChunk, xChunkSize));

    if(xContext == NULL || ((pxMessageChunk == NULL) && (xChunkSize > 0))) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* spec required 0 chunk size is processed: nothing to do */
    if((pxMessageChunk == NULL) && (xChunkSize == 0)) {
        BDBG_MSG(("%s 0 chunk size", __FUNCTION__));
        return SEC_NO_ERROR;
    }

#if 0

    if(SHA384_Update(&xContext->c384, pxMessageChunk, xChunkSize)!=1) return SEC_ERROR;
#else
    if(sha384_process(xContext->md, pxMessageChunk, xChunkSize)) {
        BDBG_ERR(("%s sha384_process failure", __FUNCTION__));
        return SEC_ERROR;
    }
#endif
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;

}
#endif

#ifndef NO_C99
TSecStatus secSha384Final
(
    TSecHashContext  xContext,
    TUnsignedInt8*  pxMessageDigest
) {

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(xContext == NULL || pxMessageDigest == NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }
#if 0
    if(SHA384_Final(pxMessageDigest, &xContext->c384)!=1) return SEC_ERROR;
#else
    if(sha384_done(xContext->md, pxMessageDigest)){
        BDBG_ERR(("%s sha384_done Failed", __FUNCTION__));
        return SEC_ERROR;
    }
#endif
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}
#endif


TSecStatus secHmacSha256Init
(
    const TUnsignedInt8*      pxKey,
    size_t               xKeySize,
    TSecHashContext*    pxContext
) {
    TSecHashContext shaCtx;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    /* Nexus Hmac SHA only supports up to 32 byte key size passing
       a key size bigger than this will cause an internal buffer overflow
       followed by Bus error. To protect stability will return unsupported
       untill Nexus is updated to support this.*/
#ifndef USE_OSSL_FOR_HMAC /* HUMAX_PLATFORM_BASE */
    if(xKeySize > 32 ) {
        BDBG_ERR(("xKeySize > 32 NEXUS unsupported"));
        return SEC_ERROR_BAD_PARAMETER;
    }
#endif
    if(xKeySize > 64 ) {
        BDBG_ERR(("xKeySize > 64"));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(pxContext == NULL ) {
        BDBG_ERR(("pxContext == NULL"));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(pxKey == NULL) {
        BDBG_ERR(("pxKey == NULL"));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(!xKeySize && pxKey) {
        BDBG_ERR(("!xKeySize && pxKey"));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(!gSHAMutexLock) {
        if(BKNI_CreateMutex( &gSHAMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SEC_ERROR;
        }
    }

    shaCtx = BKNI_Malloc(sizeof(SSecHashContext));
    if(shaCtx == NULL) {
        BDBG_ERR(("Not enough memory!\n"));
        return SEC_ERROR;
    }
    BKNI_Memset(shaCtx, 0, sizeof(SSecHashContext));
    BDBG_MSG(("%s context pointer = %p", __FUNCTION__, shaCtx));

#ifndef USE_OSSL_FOR_HMAC
    NEXUS_HMACSHA_GetDefaultOpSettings(&(shaCtx->hmacSha1Settings));
    BKNI_Memcpy(shaCtx->hmacSha1Settings.keyData, pxKey, xKeySize);
    shaCtx->hmacSha1Settings.shaType = NEXUS_HMACSHA_SHAType_eSha256;
    shaCtx->hmacSha1Settings.opMode = NEXUS_HMACSHA_Op_eHMAC;
    shaCtx->hmacSha1Settings.byteSwap= true;

    shaCtx->hmacSha1Settings.keyLength= xKeySize;
    *pxContext = shaCtx;
#else

    ENGINE_load_builtin_engines();
    ENGINE_register_all_complete();
    HMAC_CTX_init(&shaCtx->hmac);

    if(HMAC_Init_ex(&shaCtx->hmac, pxKey, xKeySize, EVP_sha256(), NULL)!=1) return SEC_ERROR;
    *pxContext = shaCtx;

#endif
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

TSecStatus secHmacSha256Update
(
    TSecHashContext  xContext,
    const TUnsignedInt8*  pxMessageChunk,
    size_t           xChunkSize
) {
    NEXUS_Error rc;

    BDBG_MSG(("Enter %s xContext %p pxMessageChunk %p xChunkSize %d", __FUNCTION__, xContext, pxMessageChunk, xChunkSize));

    if(xContext == NULL || ((pxMessageChunk == NULL) && (xChunkSize > 0))) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* spec required 0 chunk size is processed: nothing to do */
    if((pxMessageChunk == NULL) && (xChunkSize == 0)) {
        BDBG_MSG(("%s 0 chunk size", __FUNCTION__));
        return SEC_NO_ERROR;
    }

    if(pxMessageChunk && (xChunkSize == 0)) {
        BDBG_MSG(("%s 0 chunk size", __FUNCTION__));
#if 0 /* HUMAX_PLATFORM_BASE */
        return SEC_NO_ERROR;
#else
        return SEC_ERROR_BAD_PARAMETER;
#endif
    }

#ifdef USE_OSSL_FOR_HMAC
    if(HMAC_Update(&xContext->hmac, pxMessageChunk, xChunkSize)!=1) return SEC_ERROR;
#else

    /* allocate new chunk struct*/
    BDBG_MSG(("%s allocating new chunk at position %d size = %d", __FUNCTION__,xContext->numberofChunks, xChunkSize));
    if(!xContext->pMessageChunkList) { /* first chunk */
        BDBG_MSG(("%s first chunk", __FUNCTION__));
        xContext->pMessageChunkList = BKNI_Malloc(sizeof(SMessageChunk));
        if(!xContext->pMessageChunkList)
        {
            BDBG_ERR(("%s Fail - out of memory", __FUNCTION__));
            return SEC_NO_ERROR;
        }
        /* allocate chunk mem and copy buffer */
        rc = NEXUS_Memory_Allocate(xChunkSize, NULL, (void *)&xContext->pMessageChunkList->pMessageChunk);
        if (rc || !xContext->pMessageChunkList->pMessageChunk) {
            rc = BERR_TRACE(rc); 
            BDBG_ERR(("%s failed to allocate %d bytes", __FUNCTION__,xChunkSize)) ;
            return SEC_NO_ERROR;
        }
        xContext->pMessageChunkList->next=0;
        xContext->pMessageChunkList->xChunkSize=xChunkSize;
        BKNI_Memcpy((void *)xContext->pMessageChunkList->pMessageChunk, (void *)pxMessageChunk, xChunkSize );
        xContext->totalDataSize += xChunkSize;
        xContext->numberofChunks++; 
    } else /* traverse list */
    {
        int count =1; /* belt and braces */
        SMessageChunk *pChunk = xContext->pMessageChunkList;

        while (pChunk->next) {
            count++;
            pChunk = pChunk->next;
        }

        BDBG_ASSERT(count == xContext->numberofChunks);

        BDBG_MSG(("%s next chunk %d", __FUNCTION__, count+1));

        pChunk->next = BKNI_Malloc(sizeof(SMessageChunk));
        if(!pChunk->next)
        {
            BDBG_ERR(("%s Fail - out of memory", __FUNCTION__));
            return SEC_NO_ERROR;
        }
        pChunk = pChunk->next;

        /* allocate chunk mem and copy buffer */
        rc = NEXUS_Memory_Allocate(xChunkSize, NULL, (void *)&pChunk->pMessageChunk);
        if (rc || !pChunk->pMessageChunk) {
            rc = BERR_TRACE(rc); 
            BDBG_ERR(("%s failed to allocate %d bytes", __FUNCTION__,xChunkSize)) ;
            return SEC_NO_ERROR;
        }
        pChunk->next=0;
        pChunk->xChunkSize=xChunkSize;
        BKNI_Memcpy((void *)pChunk->pMessageChunk, (void *)pxMessageChunk, xChunkSize );
        xContext->totalDataSize += xChunkSize;
        xContext->numberofChunks++; 
    }

    BDBG_MSG(("%s updated context total bytes = %d %d", __FUNCTION__, xContext->totalDataSize));

#endif
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;

}

TSecStatus secHmacSha256Final
(
    TSecHashContext  xContext,
    TUnsignedInt8*  pxMessageDigest
) {
    NEXUS_Error rc;
    TUnsignedInt8*  pSubmitBuffer = 0;
    TUnsignedInt16  submitBufferSize = 0;
    SMessageChunk *pChunk = 0;
    NEXUS_HMACSHA_DigestOutput hmacOut;
    BDBG_MSG(("Enter %s xContext %p pxMessageDigest %p", __FUNCTION__, xContext, pxMessageDigest));

    if(xContext == NULL || pxMessageDigest == NULL) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

#ifndef USE_OSSL_FOR_HMAC /* HUMAX_PLATFORM_BASE */
    if(!xContext->numberofChunks || !xContext->pMessageChunkList) {
        unsigned char er_null_sha256[32] = {0xE3,0xB0,0xC4,0x42,0x98,0xFC,0x1C,0x14,0x9A,0xFB,0xF4,0xC8,0x99,0x6F,0xB9,0x24
        ,0x27,0xAE,0x41,0xE4,0x64,0x9B,0x93,0x4C,0xA4,0x95,0x99,0x1B,0x78,0x52,0xB8,0x55 };
        BDBG_MSG(("%s no update has been called for this context returning ER_NULL_SHA256", __FUNCTION__));
        BKNI_Memcpy(pxMessageDigest, er_null_sha256, 32);
        BDBG_MSG(("Leave %s", __FUNCTION__));
        return SEC_NO_ERROR;
    }
#endif
#ifdef USE_OSSL_FOR_HMAC
    unsigned int len;
    if(HMAC_Final(&xContext->hmac, pxMessageDigest, &len)!=1) return SEC_ERROR;
#else

   /* Nagra expects to be able to update buffers of any size
      but in chunk mode BHSM only allows buffers which are multiples of 64 (other than last buffer).
      So my solution is a scatter gather approach - gather all the updates into a single buffer and
      do the SHA in one go. Of course the disadvantage is the requirement for one large linear buffer
      which if the client is hashing a particularaly large block of data may cause mem problems such
      as: lack of mem or fragmentation preventing large buffer allocation.*/
    

    rc = NEXUS_Memory_Allocate(xContext->totalDataSize, NULL, (void *)&pSubmitBuffer);
    if (rc || !pSubmitBuffer) {
        rc = BERR_TRACE(rc); 
        BDBG_ERR(("%s failed to allocate %d bytes for submit buffer", __FUNCTION__,xContext->totalDataSize)) ;
        return SEC_NO_ERROR;
    }

    pChunk = xContext->pMessageChunkList;

    /* gather all the buffers into one submit */
    while(pChunk) {
        BKNI_Memcpy(pSubmitBuffer+submitBufferSize, pChunk->pMessageChunk, pChunk->xChunkSize );
        submitBufferSize += pChunk->xChunkSize;
        BDBG_MSG(("%s Added %dbytes to submit buffer total size now %d", __FUNCTION__, pChunk->xChunkSize, submitBufferSize));
        pChunk = pChunk->next;
    }

    /* belt and braces */
    BKNI_AcquireMutex(gSHAMutexLock);

    xContext->hmacSha1Settings.dataMode = NEXUS_HMACSHA_DataMode_eAllIn;
    xContext->hmacSha1Settings.dataAddress = pSubmitBuffer;
    xContext->hmacSha1Settings.dataSize = submitBufferSize;
    if(NEXUS_HMACSHA_PerformOp(&(xContext->hmacSha1Settings), &hmacOut)) {
        BKNI_ReleaseMutex(gSHAMutexLock);
        BDBG_ERR(("%s NEXUS_HMACSHA_PerformOp Failed", __FUNCTION__));
        return SEC_ERROR;
    }

    BKNI_ReleaseMutex(gSHAMutexLock);
    BDBG_MSG(("%s Successfully Submitted SHA256 resquest to BSP", __FUNCTION__));

    BKNI_Memcpy(pxMessageDigest, hmacOut.digestData, hmacOut.digestSize);

    /* clean up */
    if(xContext->pMessageChunkList) {
        SMessageChunk *pNextChunk = 0;

        pChunk = xContext->pMessageChunkList;
        /* traverse list freeing buffers */
        while(pChunk->next) {
            /* save next */
            pNextChunk = pChunk->next;

            /* free chunk buffer */
            BDBG_ASSERT(pChunk->pMessageChunk);
            NEXUS_Memory_Free((void *)pChunk->pMessageChunk);

            /* free stuct */
            BKNI_Free(pChunk);
            BDBG_MSG(("%s removed chunk %d", __FUNCTION__, xContext->numberofChunks));
            xContext->numberofChunks--;
            pChunk = pNextChunk;
        }

        /* free chunk buffer */
        BDBG_ASSERT(pChunk->pMessageChunk);
        NEXUS_Memory_Free((void *)pChunk->pMessageChunk);

        /* free stuct */
        BKNI_Free(pChunk);
    }

    NEXUS_Memory_Free(pSubmitBuffer);

    BKNI_Free(xContext);

#endif
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}






#ifndef NO_C99
TSecStatus secRsaGenerateKey(
    size_t                xKeySize,
    TUnsignedInt32        xE,
    TUnsignedInt8*       pxN,
    TUnsignedInt8*       pxP,
    TUnsignedInt8*       pxQ,
    TUnsignedInt8*       pxDP,
    TUnsignedInt8*       pxDQ,
    TUnsignedInt8*       pxQInv

) {
    int ret = 0;
    unsigned long out_size;
    rsa_key rsaKey;
    int prng_idx ;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if((pxN == NULL) || (pxP == NULL) || (pxQ == NULL)||
            (pxDQ== NULL) ||(pxDP== NULL) || (pxQInv == NULL)) {
        BDBG_ERR( ("SEC_ERROR_BAD_PARAMETER") );
        return SEC_ERROR_BAD_PARAMETER;
    }

    if((xE != 3) && (xE != 17) && (xE != 65537)) {
        BDBG_ERR(("Invalid xE %d", xE));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((xKeySize%8)||(xKeySize<64)||(xKeySize>256)) {
        BDBG_ERR(("Invalid key size %d", xKeySize));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(!gRSAMutexLock) {
        if(BKNI_CreateMutex( &gRSAMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SEC_ERROR;
        }
        BDBG_MSG( ("BKNI_CreateMutex success") );
    }

    BKNI_AcquireMutex(gRSAMutexLock);
    BDBG_MSG(("%s calling rsa_make_key xKeySize %d xE %d", __FUNCTION__, xKeySize, xE));
 /*   ltc_mp = ltm_desc; */
    ltc_mp = ltm_desc;

    register_prng(&sprng_desc);

    prng_idx= find_prng("sprng");
    BDBG_MSG(("%s prng_idx %d", __FUNCTION__,prng_idx));
    ret = rsa_make_key(NULL,prng_idx,xKeySize,xE,&rsaKey);
    if(ret) {
        BDBG_ERR(("%s rsa_make_key Failed", __FUNCTION__));
        goto err;
    }
    out_size = xKeySize/2;

    ret = mp_to_unsigned_bin_n((mp_int*)(rsaKey.qP),pxQInv,&out_size);
    if(ret) {
        BDBG_ERR(("%s mp_to_unsigned_bin_n Failed", __FUNCTION__));
        goto err;
    }
    if(out_size != xKeySize/2) {
        memmove(&pxQInv[xKeySize/2-out_size],&pxQInv[0],out_size);
        memset(&pxQInv[0],0,xKeySize/2-out_size);
    }

    out_size = xKeySize;

    ret = mp_to_unsigned_bin_n((mp_int*)(rsaKey.N),pxN,&out_size);
    if(ret) {
        BDBG_ERR(("%s mp_to_unsigned_bin_n Failed", __FUNCTION__));
        goto err;
    }
    if(out_size != xKeySize) {
        memmove(&pxN[xKeySize-out_size],&pxN[0],out_size);
        memset(&pxN[0],0,xKeySize-out_size);
    }

    out_size = xKeySize/2;

    ret = mp_to_unsigned_bin_n((mp_int*)(rsaKey.p),pxP,&out_size);
    if(ret) {
        BDBG_ERR(("%s mp_to_unsigned_bin_n Failed", __FUNCTION__));
        goto err;
    }
    if(out_size != xKeySize/2) {
        memmove(&pxP[xKeySize/2-out_size],&pxP[0],out_size);
        memset(&pxP[0],0,xKeySize/2-out_size);
    }
    out_size = xKeySize/2;
    ret = mp_to_unsigned_bin_n((mp_int*)(rsaKey.q),pxQ,&out_size);
    if(ret) {
        BDBG_ERR(("%s mp_to_unsigned_bin_n Failed", __FUNCTION__));
        goto err;
    }
    if(out_size != xKeySize/2) {
        memmove(&pxQ[xKeySize/2-out_size],&pxQ[0],out_size);
        memset(&pxQ[0],0,xKeySize/2-out_size);
    }

    out_size = xKeySize/2;

    ret = mp_to_unsigned_bin_n((mp_int*)(rsaKey.dP),pxDP,&out_size);
    if(ret) {
        BDBG_ERR(("%s mp_to_unsigned_bin_n Failed", __FUNCTION__));
        goto err;
    }
    if(out_size != xKeySize/2) {
        memmove(&pxDP[xKeySize/2-out_size],&pxDP[0],out_size);
        memset(&pxDP[0],0,xKeySize/2-out_size);
    }

    out_size = xKeySize/2;
    ret = mp_to_unsigned_bin_n((mp_int*)(rsaKey.dQ),pxDQ,&out_size);
    if(ret) {
        BDBG_ERR(("%s mp_to_unsigned_bin_n Failed", __FUNCTION__));
        goto err;
    }
    if(out_size != xKeySize/2) {
        memmove(&pxDQ[xKeySize/2-out_size],&pxDQ[0],out_size);
        memset(&pxDQ[0],0,xKeySize/2-out_size);
    }
    rsa_free(&rsaKey);

    BDBG_MSG(("%s pxN %p", __FUNCTION__, pxN));
    writeIt((unsigned char*)pxN,xKeySize);
    BDBG_MSG(("%s pxP %p", __FUNCTION__, pxP));
    writeIt((unsigned char*)pxP,xKeySize/2);
    BDBG_MSG(("%s pxQ %p", __FUNCTION__, pxQ));
    writeIt((unsigned char*)pxQ,xKeySize/2);
    BDBG_MSG(("%s pxDP %p", __FUNCTION__, pxDP));
    writeIt((unsigned char*)pxDP,xKeySize/2);
    BDBG_MSG(("%s pxDQ %p", __FUNCTION__, pxDQ));
    writeIt((unsigned char*)pxDQ,xKeySize/2);
    BDBG_MSG(("%s pxQInv %p", __FUNCTION__, pxQInv));
    writeIt((unsigned char*)pxQInv,xKeySize/2);

    BKNI_ReleaseMutex(gRSAMutexLock);
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;

err:
    BKNI_ReleaseMutex(gRSAMutexLock);
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_ERROR;

}
#endif

#ifndef NO_C99
TSecStatus secRsaComputeCrtParams
(
    size_t                xKeySize,
    TUnsignedInt32        xE,
    const TUnsignedInt8*       pxP,
    const TUnsignedInt8*       pxQ,
    TUnsignedInt8*       pxDP,
    TUnsignedInt8*       pxDQ,
    TUnsignedInt8*       pxQInv
) {

    RSA *rsaKey = RSA_new();
    int ret = 0;
    BIGNUM *p;
    BIGNUM *r0=NULL,*r1=NULL,*r2=NULL, *d0=NULL;
    BIGNUM *local_dmp=NULL, *local_dmq=NULL, *local_p=NULL;
    unsigned char pd0[128] = {0};
    BIGNUM *bne=BN_new();
    unsigned long e = xE;
    BN_CTX *ctx=NULL;

    BDBG_MSG(("Enter %s xKeySize %d xE %d", __FUNCTION__, xKeySize, xE ));

    if((pxP == NULL) || (pxQ == NULL)||
            (pxDQ== NULL) ||(pxDP== NULL) || (pxQInv == NULL)) {
        BDBG_ERR(("Invalid Parameter %d", xKeySize));
        BN_clear_free(bne);
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((xKeySize%8)||(xKeySize<64)||(xKeySize>256)) {
        BDBG_ERR(("Invalid key size %d", xKeySize));
        BN_clear_free(bne);
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((xE != 3) && (xE != 17) && (xE != 65537)) {
        BDBG_ERR(("Invalid xE %d", xE));
        BN_clear_free(bne);
        return SEC_ERROR_BAD_PARAMETER;
    }
    


    BN_set_word(bne, e);
    rsaKey->e = bne;

    /* print bne here */
    BDBG_MSG(("%s 1st bne:", __FUNCTION__));
    ret = BN_bn2bin(bne, pd0);
    writeIt((unsigned char*)pd0,xKeySize/2);
    BKNI_Memset(pd0, 0, 128);

    rsaKey->p = BN_bin2bn(pxP, xKeySize/2, rsaKey->p);
    rsaKey->q = BN_bin2bn(pxQ, xKeySize/2, rsaKey->q);

    ctx=BN_CTX_new();
    if (ctx == NULL) {
        BDBG_ERR(("%s BN_CTX_new Failed", __FUNCTION__));
        goto err;
    }
    BN_CTX_start(ctx);
    d0 = BN_CTX_get(ctx);
    r0 = BN_CTX_get(ctx);
    r1 = BN_CTX_get(ctx);
    r2 = BN_CTX_get(ctx);
    local_p = BN_CTX_get(ctx);
    local_dmp = BN_CTX_get(ctx);
    local_dmq = BN_CTX_get(ctx);
    if (!BN_sub(r1,rsaKey->p,BN_value_one())) {
        BDBG_ERR(("%s BN_sub Failed", __FUNCTION__));
        goto err;	   /* p-1 */
    }




    BDBG_MSG(("%s p-1:", __FUNCTION__));
    ret = BN_bn2bin(r1, pd0);
    writeIt((unsigned char*)pd0,xKeySize/2);
    BKNI_Memset(pd0, 0, 128);

    if (!BN_sub(r2,rsaKey->q,BN_value_one())) {
        BDBG_ERR(("%s BN_sub Failed", __FUNCTION__));
        goto err;	   /* q-1 */
    }
    BDBG_MSG(("%s q-1:", __FUNCTION__));
    ret = BN_bn2bin(r2, pd0);
    writeIt((unsigned char*)pd0,xKeySize/2);
    BKNI_Memset(pd0, 0, 128);

    if (!BN_mul(r0,r1,r2,ctx)) {
        BDBG_ERR(("%s BN_mul Failed", __FUNCTION__));
        goto err;    /* (p-1)(q-1) */
    }

    BDBG_MSG(("%s Compute the totient of the product as (p - 1)(q - 1) giving :", __FUNCTION__));
    ret = BN_bn2bin(r0, pd0);
    writeIt((unsigned char*)pd0,64);
    BKNI_Memset(pd0, 0, 128);

    /*
    if (!(rsaKey->flags & RSA_FLAG_NO_CONSTTIME))
    {
        pr0 = &local_r0;
        BN_with_flags(pr0, r0, BN_FLG_CONSTTIME);
    }
    else {
        pr0 = r0;
    }
*/

    if (!BN_mod_inverse(d0,bne,r0,ctx)) {
        ERR_load_crypto_strings();
        BDBG_ERR(("%s BN_mod_inverse 0 Failed Error %s", __FUNCTION__, ERR_error_string(ERR_peek_last_error(), NULL)));
        goto err;   /* d */
    }

    BDBG_MSG(("%s Compute d, the modular multiplicative inverse of e (mod ?(n)) yielding:", __FUNCTION__));
    ret = BN_bn2bin(d0, pd0);
    writeIt((unsigned char*)pd0,64);
    BKNI_Memset(pd0, 0, 128);

    rsaKey->d = d0;
    if (!BN_mod(local_dmp, d0,r1,ctx)) {
        ERR_load_crypto_strings();
        BDBG_ERR(("%s BN_mod Failed Error %s", __FUNCTION__, ERR_error_string(ERR_peek_last_error(), NULL)));
        goto err;
    }

    /* calculate d mod (q-1) */
    if (!BN_mod(local_dmq, d0,r2,ctx)) {
        ERR_load_crypto_strings();
        BDBG_ERR(("%s BN_mod 2 Failed Error %s", __FUNCTION__, ERR_error_string(ERR_peek_last_error(), NULL)));
        goto err;
    }

    /* calculate inverse of q mod p */
    p = rsaKey->p;
    if (!BN_mod_inverse(local_p,rsaKey->q,rsaKey->p,ctx)) {
        ERR_load_crypto_strings();
        BDBG_ERR(("%s BN_mod_inverse 1 Failed Error %s", __FUNCTION__, ERR_error_string(ERR_peek_last_error(), NULL)));
        goto err;
    }

    ret = BN_bn2bin(local_dmp, pxDP);
    if((unsigned int)ret != xKeySize/2) {
        BDBG_MSG(("%s pxDP (ret != xKeySize/2)", __FUNCTION__));
        memmove(&pxDP[xKeySize/2-ret],&pxDP[0],ret);
        memset(&pxDP[0],0,xKeySize/2-ret);
    }
    ret = BN_bn2bin(local_dmq, pxDQ);
    if((unsigned int)ret != xKeySize/2) {
        BDBG_MSG(("%s pxDQ (ret != xKeySize/2)", __FUNCTION__));
        memmove((void *)&pxQ[xKeySize/2-ret],&pxQ[0],ret);
        memset((void *)&pxQ[0],0,xKeySize/2-ret);
    }
    ret = BN_bn2bin(local_p, pxQInv);
    if((unsigned int)ret != xKeySize/2) {
        BDBG_MSG(("%s pxQInv (ret != xKeySize/2)", __FUNCTION__));
        memmove((void *)&pxQInv[xKeySize/2-ret],&pxQInv[0],ret);
        memset((void *)&pxQInv[0],0,xKeySize/2-ret);
    }

    if(ctx) {
        BN_CTX_end(ctx);
        BN_CTX_free(ctx);
    }

    BN_clear_free(bne);

    BDBG_MSG(("%s DP calculated as", __FUNCTION__));
    writeIt((unsigned char*)pxDP,xKeySize/2);
    BDBG_MSG(("%s DQ calculated as", __FUNCTION__));
    writeIt((unsigned char*)pxDQ,xKeySize/2);
    BDBG_MSG(("%s Qinv calculated as", __FUNCTION__));
    writeIt((unsigned char*)pxQInv,xKeySize/2);
    
            
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
err:

    if(ctx) {
        BN_CTX_end(ctx);
        BN_CTX_free(ctx);
    }
    BN_clear_free(bne);
    BDBG_ERR(("SEC_ERROR"));
    return SEC_ERROR;
}
#endif

#ifndef NO_C99
TSecStatus secRsaPublicEncrypt
(
    const TUnsignedInt8*       pxInput,
    size_t                xInputSize,
    TUnsignedInt8*       pxOutput,
    TUnsignedInt32        xE,
    const TUnsignedInt8*       pxN,
    size_t                xKeySize,
    TSecRsaPadding        xPadding
) {
    int ret = 0;
    unsigned char *tmpbuff;
    unsigned long tmplen;
    unsigned long mudulusbitlen = xKeySize*8;
    rsa_key rsaKey;
    int prng_idx;
    int hash_idx;


    BDBG_MSG(("Enter %s", __FUNCTION__));

    tmpbuff = malloc(xKeySize);

    if(pxInput == NULL || pxOutput == NULL) {
        BDBG_ERR(("SEC_ERROR_BAD_PARAMETER"));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(pxN==NULL) {
        BDBG_ERR(("SEC_ERROR_BAD_PARAMETER1"));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((xKeySize%8)||(xKeySize<64)||(xKeySize>256)) {
        BDBG_ERR(("Invalid key size %d", xKeySize));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((xE != 3) && (xE != 17) && (xE != 65537)) {
        BDBG_ERR(("Invalid xE %d", xE));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(!xInputSize) {
        BDBG_ERR(("Invalid xInputSize %d", xInputSize));
        return SEC_ERROR_BAD_PARAMETER;
    }

    BDBG_MSG(("%s pxInput %p xInputSize %d pxOutput %p xE %d pxN %p  xKeySize %d xPadding %d"
              , __FUNCTION__
              , pxInput 
              , xInputSize 
              , pxOutput 
              , xE 
              , pxN 
              , xKeySize 
              , xPadding));
    writeIt((unsigned char *)pxInput, xInputSize);

    /* check padding consistency */
    switch(xPadding) {
    case SEC_RSA_PKCS1_V1_5_PADDING:
        if(xInputSize > (xKeySize - 11)) {
            BDBG_ERR(("Invalid xInputSize %d for SEC_RSA_PKCS1_V1_5_PADDING", xInputSize));
            return SEC_ERROR_BAD_PARAMETER;
        }
        break;
    case SEC_RSA_OAEP_SHA_1_PADDING:
        if(xInputSize > (xKeySize - 2*20 - 2)) {
            BDBG_ERR(("Invalid xInputSize %d for SEC_RSA_OAEP_SHA_1_PADDING", xInputSize));
            return SEC_ERROR_BAD_PARAMETER;
        }
        break;
    case SEC_RSA_OAEP_SHA_256_PADDING:
        if(xInputSize > (xKeySize - 2*32 - 2)) {
            BDBG_ERR(("Invalid xInputSize %d for SEC_RSA_OAEP_SHA_256_PADDING", xInputSize));
            return SEC_ERROR_BAD_PARAMETER;
        }
        break;
    case SEC_RSA_OAEP_SHA_384_PADDING:
        if(xInputSize > (xKeySize - 2*48 - 2 )) {
            BDBG_ERR(("Invalid xInputSize %d for SEC_RSA_OAEP_SHA_384_PADDING", xInputSize));
            return SEC_ERROR_BAD_PARAMETER;
        }
        break;
    case SEC_RSA_NO_PADDING:
        if(xInputSize != xKeySize) {
            BDBG_ERR(("Invalid xInputSize %d for SEC_RSA_NO_PADDING", xInputSize));
            return SEC_ERROR_BAD_PARAMETER;
        }
        break;
    default:
        BDBG_ERR(("Invalid xPadding %d", xPadding));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(!gRSAMutexLock) {
        if(BKNI_CreateMutex( &gRSAMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SEC_ERROR;
        }
    }

    BKNI_AcquireMutex(gRSAMutexLock);

    ltc_mp = ltm_desc;

    register_prng(&sprng_desc);

    prng_idx= find_prng("sprng");
    hash_idx = find_hash("sha1");


    ret = rsa_make_key(NULL,prng_idx,xKeySize,xE,&rsaKey);
    if(ret) {
        BKNI_ReleaseMutex(gRSAMutexLock);
        BDBG_ERR(("%s rsa_make_key Failed", __FUNCTION__));
        return SEC_ERROR;
    }
    rsaKey.type = PK_PUBLIC;
    ret = mp_read_unsigned_bin((mp_int *)rsaKey.N,pxN,xKeySize);
    if(ret) {
        BDBG_ERR(("%s mp_read_unsigned_bin Failed", __FUNCTION__));
        goto err;
    }
    mudulusbitlen = mp_count_bits(rsaKey.N);
    tmplen = xKeySize;
    switch(xPadding)
    {
    case SEC_RSA_PKCS1_V1_5_PADDING :
        ret = pkcs_1_v1_5_encode(pxInput,xInputSize,LTC_LTC_PKCS_1_EME,mudulusbitlen,NULL,prng_idx,tmpbuff,&tmplen);
        BDBG_MSG(("%s back from pkcs_1_v1_5_encode %d result buffer:", __FUNCTION__, tmplen));
        if(ret) {
            BDBG_ERR(("%s pkcs_1_v1_5_encode Failed", __FUNCTION__));
            goto err;
        }
        writeIt((unsigned char*)tmpbuff, tmplen);
        break;
    case SEC_RSA_OAEP_SHA_1_PADDING :
        register_hash(&sha1_desc);
        hash_idx = find_hash("sha1");
        ret = pkcs_1_oaep_encode(pxInput,xInputSize,NULL,0,mudulusbitlen,NULL,prng_idx,hash_idx,tmpbuff,&tmplen);
        if(ret) {
            BDBG_ERR(("%s pkcs_1_oaep_encode Failed", __FUNCTION__));
            goto err;
        }
        unregister_hash(&sha1_desc);
        break;
    case SEC_RSA_OAEP_SHA_256_PADDING:
        register_hash(&sha256_desc);
        hash_idx = find_hash("sha256");
        ret = pkcs_1_oaep_encode(pxInput,xInputSize,NULL,0,mudulusbitlen,NULL,prng_idx,hash_idx,tmpbuff,&tmplen);
        if(ret) {
            BDBG_ERR(("%s pkcs_1_oaep_encode Failed", __FUNCTION__));
            goto err;
        }
        unregister_hash(&sha256_desc);
        break;
    case SEC_RSA_NO_PADDING:
        memcpy(tmpbuff,pxInput,xInputSize);
        break;
    case SEC_RSA_OAEP_SHA_384_PADDING:
        register_hash(&sha384_desc);
        hash_idx = find_hash("sha384");
        ret = pkcs_1_oaep_encode(pxInput,xInputSize,NULL,0,mudulusbitlen,NULL,prng_idx,hash_idx,tmpbuff,&tmplen);
        if(ret) {
            BDBG_ERR(("%s pkcs_1_oaep_encode Failed", __FUNCTION__));
            goto err;
        }
        unregister_hash(&sha384_desc);
        break;
    case LAST_SEC_RSA_PADDING:
        BDBG_ERR(("SEC_ERROR_BAD_PADDING"));
        return SEC_ERROR_BAD_PADDING;
        break;
    }
    if(ret!=CRYPT_OK) {
        BDBG_ERR(("Padding error!"));
        goto err;
    }

    ret = ltc_mp.rsa_me(tmpbuff, xKeySize, pxOutput, &tmplen, PK_PUBLIC, &rsaKey);
    if(ret != CRYPT_OK) {
        BDBG_ERR(("%s rsa_me Failed", __FUNCTION__));
        goto err;
    }

    rsa_free(&rsaKey);
    free(tmpbuff);
    BKNI_ReleaseMutex(gRSAMutexLock);
    BDBG_MSG(("%s Output", __FUNCTION__));
    writeIt((unsigned char*)pxOutput, xKeySize);
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
err:
    rsa_free(&rsaKey);
    BKNI_ReleaseMutex(gRSAMutexLock);
    BDBG_ERR(("SEC_ERROR"));
    return SEC_ERROR;
}
#endif

#ifndef NO_C99
TSecStatus secRsaPrivateEncrypt
(
    const TUnsignedInt8*       pxInput,
    size_t                xInputSize,
    TUnsignedInt8*       pxOutput,
    TUnsignedInt32        xE,
    const TUnsignedInt8*       pxN,
    const TUnsignedInt8*       pxP,
    const TUnsignedInt8*       pxQ,
    const TUnsignedInt8*       pxDP,
    const TUnsignedInt8*       pxDQ,
    const TUnsignedInt8*       pxQInv,
    size_t                xKeySize,
    TSecRsaPadding        xPadding
) {
    int ret = 0;
    unsigned char *tmpbuff;
    unsigned long tmplen;
    unsigned long mudulusbitlen = xKeySize*8;
    rsa_key rsaKey;
    int prng_idx;
    int hash_idx;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    tmpbuff = malloc(xKeySize);
    if(pxInput == NULL || pxOutput == NULL) {
        BDBG_ERR(("SEC_ERROR_BAD_PARAMETER"));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((pxN == NULL) || (pxP == NULL) || (pxQ == NULL) || (pxDQ== NULL) || (pxDP== NULL) || (pxQInv == NULL)) {
        BDBG_ERR(("SEC_ERROR_BAD_PARAMETER 1"));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((xKeySize%8)||(xKeySize<64)||(xKeySize>256)) {
        BDBG_ERR(("Invalid key size %d", xKeySize));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((xE != 3) && (xE != 17) && (xE != 65537)) {
        BDBG_ERR(("Invalid xE %d", xE));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(!xInputSize) {
        BDBG_ERR(("Invalid xInputSize %d", xInputSize));
        return SEC_ERROR_BAD_PARAMETER;
    }

    BDBG_MSG(("%s pxInput %p xInputSize %d pxOutput %p xE %d pxN %p  xKeySize %d xPadding %d"
              , __FUNCTION__
              , pxInput 
              , xInputSize 
              , pxOutput 
              , xE 
              , pxN 
              , xKeySize 
              , xPadding));

    /* check padding consistency */
    switch(xPadding) {
    case SEC_RSA_PKCS1_V1_5_PADDING:
        if(xInputSize > (xKeySize - 11)) {
            BDBG_ERR(("Invalid xInputSize %d for SEC_RSA_PKCS1_V1_5_PADDING", xInputSize));
            return SEC_ERROR_BAD_PARAMETER;
        }
        break;
    case SEC_RSA_OAEP_SHA_1_PADDING:
        if(xInputSize > (xKeySize - 2*20 - 2)) {
            BDBG_ERR(("Invalid xInputSize %d for SEC_RSA_OAEP_SHA_1_PADDING", xInputSize));
            return SEC_ERROR_BAD_PARAMETER;
        }
        break;
    case SEC_RSA_OAEP_SHA_256_PADDING:
        if(xInputSize > (xKeySize - 2*32 - 2)) {
            BDBG_ERR(("Invalid xInputSize %d for SEC_RSA_OAEP_SHA_256_PADDING", xInputSize));
            return SEC_ERROR_BAD_PARAMETER;
        }
        break;
    case SEC_RSA_OAEP_SHA_384_PADDING:
        if(xInputSize > (xKeySize - 2*48 - 2 )) {
            BDBG_ERR(("Invalid xInputSize %d for SEC_RSA_OAEP_SHA_384_PADDING", xInputSize));
            return SEC_ERROR_BAD_PARAMETER;
        }
        break;
    case SEC_RSA_NO_PADDING:
        if(xInputSize != xKeySize) {
            BDBG_ERR(("Invalid xInputSize %d for SEC_RSA_NO_PADDING", xInputSize));
            return SEC_ERROR_BAD_PARAMETER;
        }
        break;
    default:
        BDBG_ERR(("Invalid xPadding %d", xPadding));
        return SEC_ERROR_BAD_PARAMETER;
    }

    writeIt((unsigned char*)pxInput, xInputSize);

    if(!gRSAMutexLock) {
        if(BKNI_CreateMutex( &gRSAMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SEC_ERROR;
        }
    }

    BKNI_AcquireMutex(gRSAMutexLock);

    ltc_mp = ltm_desc;

    register_prng(&sprng_desc);

    prng_idx= find_prng("sprng");
    hash_idx = find_hash("sha1");

    ret = rsa_make_key(NULL,prng_idx,xKeySize,xE,&rsaKey);
    if(ret) {
        BKNI_ReleaseMutex(gRSAMutexLock);
        BDBG_ERR(("%s rsa_make_key Failed", __FUNCTION__));
        return SEC_ERROR;
    }
    ret = mp_read_unsigned_bin((mp_int *)rsaKey.N,pxN,xKeySize);
    if(ret) {
        BDBG_ERR(("mp_read_unsigned_bin"));
        goto err;
    }
    ret = mp_read_unsigned_bin((mp_int *)rsaKey.p,pxP,xKeySize/2);
    if(ret) {
        BDBG_ERR(("mp_read_unsigned_bin"));
        goto err;
    }
    ret = mp_read_unsigned_bin((mp_int *)rsaKey.q,pxQ,xKeySize/2);
    if(ret) {
        BDBG_ERR(("mp_read_unsigned_bin"));
        goto err;
    }
    ret = mp_read_unsigned_bin((mp_int *)rsaKey.dP,pxDP,xKeySize/2);
    if(ret) {
        BDBG_ERR(("mp_read_unsigned_bin"));
        goto err;
    }
    ret = mp_read_unsigned_bin((mp_int *)rsaKey.dQ,pxDQ,xKeySize/2);
    if(ret) {
        BDBG_ERR(("mp_read_unsigned_bin"));
        goto err;
    }
    ret = mp_read_unsigned_bin((mp_int *)rsaKey.qP,pxQInv,xKeySize/2);
    if(ret) {
        BDBG_ERR(("mp_read_unsigned_bin"));
        goto err;
    }

    tmplen = xKeySize;
    switch(xPadding)
    {
    case SEC_RSA_PKCS1_V1_5_PADDING :
        ret = pkcs_1_v1_5_encode(pxInput,xInputSize,LTC_LTC_PKCS_1_EME,mudulusbitlen,NULL,prng_idx,tmpbuff,&tmplen);
        if(ret) {
            BDBG_ERR(("pkcs_1_v1_5_encode"));
            goto err;
        }
        break;
    case SEC_RSA_OAEP_SHA_1_PADDING :
        register_hash(&sha1_desc);
        hash_idx = find_hash("sha1");
        ret = pkcs_1_oaep_encode(pxInput,xInputSize,NULL,0,mudulusbitlen,NULL,prng_idx,hash_idx,tmpbuff,&tmplen);
        if(ret) {
            BDBG_ERR(("pkcs_1_oaep_encode"));
            goto err;
        }
        unregister_hash(&sha1_desc);
        break;
    case SEC_RSA_OAEP_SHA_256_PADDING:
        register_hash(&sha256_desc);
        hash_idx = find_hash("sha256");
        ret = pkcs_1_oaep_encode(pxInput,xInputSize,NULL,0,mudulusbitlen,NULL,prng_idx,hash_idx,tmpbuff,&tmplen);
        if(ret) {
            BDBG_ERR(("pkcs_1_oaep_encode"));
            goto err;
        }
        unregister_hash(&sha256_desc);
        break;
    case SEC_RSA_NO_PADDING:
        memcpy(tmpbuff,pxInput,xInputSize);
        break;
    case SEC_RSA_OAEP_SHA_384_PADDING:
        register_hash(&sha384_desc);
        hash_idx = find_hash("sha384");
        ret = pkcs_1_oaep_encode(pxInput,xInputSize,NULL,0,mudulusbitlen,NULL,prng_idx,hash_idx,tmpbuff,&tmplen);
        if(ret) {
            BDBG_ERR(("pkcs_1_oaep_encode"));
            goto err;
        }
        unregister_hash(&sha384_desc);
        break;
    case LAST_SEC_RSA_PADDING:
        BKNI_ReleaseMutex(gRSAMutexLock);
        BDBG_ERR(("SEC_ERROR_BAD_PADDING"));
        return SEC_ERROR_BAD_PADDING;
        break;
    }
    if(ret!=CRYPT_OK) {
        BDBG_ERR(("Padding error!"));
        goto err;
    }
    ret = ltc_mp.rsa_me(tmpbuff, xKeySize, pxOutput, &tmplen, PK_PRIVATE, &rsaKey);
    if(ret != CRYPT_OK) {
        BDBG_ERR(("ret != CRYPT_OK %d", ret));
        goto err;
    }
    rsa_free(&rsaKey);
    free(tmpbuff);
    BKNI_ReleaseMutex(gRSAMutexLock);
    writeIt((unsigned char*)pxOutput, xKeySize);
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
err:
    BKNI_ReleaseMutex(gRSAMutexLock);
    rsa_free(&rsaKey);
    BDBG_ERR(("SEC_ERROR"));
    return SEC_ERROR;
}
#endif

#ifndef NO_C99
TSecStatus secRsaPublicDecrypt
(
    const TUnsignedInt8*       pxInput,
    TUnsignedInt8*       pxOutput,
    size_t*              pxOutputSize,
    TUnsignedInt32        xE,
    const TUnsignedInt8*       pxN,
    size_t                xKeySize,
    TSecRsaPadding        xPadding
) {
    int ret = 0;
    unsigned char *tmpbuff;
    unsigned long tmplen;
    unsigned long mudulusbitlen = xKeySize*8;
    int prng_idx;
    int hash_idx;
    int sign_valid = 0;
    int blocktype=0;
    rsa_key rsaKey;
    tmpbuff = malloc(xKeySize);

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(pxInput == NULL || pxOutput == NULL || pxOutputSize == NULL) {
        BDBG_ERR(("SEC_ERROR_BAD_PARAMETER"));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(pxN==NULL) {
        BDBG_ERR(("SEC_ERROR_BAD_PARAMETER1"));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((xKeySize%8)||(xKeySize<64)||(xKeySize>256)) {
        BDBG_ERR(("Invalid key size %d", xKeySize));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((xE != 3) && (xE != 17) && (xE != 65537)) {
        BDBG_ERR(("Invalid xE %d", xE));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(xPadding >= LAST_SEC_RSA_PADDING) {
        BDBG_ERR(("Invalid xPadding%d", xPadding));
        return SEC_ERROR_BAD_PARAMETER;
    }

    BDBG_MSG(("%s pxOutputSize %d, xKeySize %d xPadding  %d xE %d", __FUNCTION__, *pxOutputSize, xKeySize, xPadding, xE));
    writeIt((unsigned char*)pxInput,xKeySize);
    writeIt((unsigned char*)pxN,xKeySize);

    if(!gRSAMutexLock) {
        if(BKNI_CreateMutex( &gRSAMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SEC_ERROR;
        }
    }

    BKNI_AcquireMutex(gRSAMutexLock);


    ltc_mp = ltm_desc;

    register_prng(&sprng_desc);

    prng_idx= find_prng("sprng");
    hash_idx = find_hash("sha1");

    ret =  rsa_make_key(NULL,prng_idx,xKeySize,xE,&rsaKey);
    if(ret) {
        BKNI_ReleaseMutex(gRSAMutexLock);
        BDBG_ERR(("rsa_make_key"));
        return SEC_ERROR;
    }
    rsaKey.type = PK_PUBLIC;
    ret = mp_read_unsigned_bin((mp_int *)rsaKey.N,pxN,xKeySize);
    if(ret) {
        BDBG_ERR(("mp_read_unsigned_bin"));
        goto err;
    }
    tmplen = xKeySize;
    ret = ltc_mp.rsa_me(pxInput, xKeySize, tmpbuff, &tmplen, PK_PUBLIC, &rsaKey);
    if(ret) {
        BDBG_ERR(("rsa_me failure %d", ret));
        goto err;
    }

    BDBG_MSG(("%s buffer after decode:", __FUNCTION__));
    writeIt((unsigned char*)tmpbuff,tmplen);

    mudulusbitlen = mp_count_bits(rsaKey.N);
    BDBG_MSG(("%s mudulusbitlen %d", __FUNCTION__, mudulusbitlen));


    switch(xPadding)
    {
    case SEC_RSA_PKCS1_V1_5_PADDING :
        blocktype = (int) tmpbuff[1];
        if((blocktype <   LTC_LTC_PKCS_1_EMSA) || (blocktype > LTC_LTC_PKCS_1_EME)) {
            BDBG_ERR(("blocktype invalid"));
            return SEC_ERROR;
        }
        ret =	pkcs_1_v1_5_decode(tmpbuff,xKeySize,blocktype,mudulusbitlen,pxOutput,&tmplen,&sign_valid);
        BDBG_MSG(("%s pkcs_1_v1_5_decode sign_valid %d ret %d tmplen %d blocktype %d", __FUNCTION__, sign_valid, ret, tmplen, blocktype));
        break;
    case SEC_RSA_OAEP_SHA_1_PADDING :
        register_hash(&sha1_desc);
        hash_idx = find_hash("sha1");
        ret =	pkcs_1_oaep_decode(tmpbuff,xKeySize,NULL,0,mudulusbitlen,hash_idx,pxOutput,&tmplen,&sign_valid);
        BDBG_MSG(("%s pkcs_1_oaep_decode sha1 sign_valid %d ret %d tmplen %d", __FUNCTION__, sign_valid, ret, tmplen));
        unregister_hash(&sha1_desc);
        break;
    case SEC_RSA_OAEP_SHA_256_PADDING:
        register_hash(&sha256_desc);
        hash_idx = find_hash("sha256");
        ret =	pkcs_1_oaep_decode(tmpbuff,xKeySize,NULL,0,mudulusbitlen,hash_idx,pxOutput,&tmplen,&sign_valid);
        BDBG_MSG(("%s pkcs_1_oaep_decode sha256 sign_valid %d ret %d tmplen %d", __FUNCTION__, sign_valid, ret, tmplen));
        unregister_hash(&sha256_desc);
        break;
    case SEC_RSA_NO_PADDING:
        memcpy(pxOutput,tmpbuff,xKeySize);
        break;
    case SEC_RSA_OAEP_SHA_384_PADDING:
        register_hash(&sha384_desc);
        hash_idx = find_hash("sha384");
        ret =	pkcs_1_oaep_decode(tmpbuff,xKeySize,NULL,0,mudulusbitlen,hash_idx,pxOutput,&tmplen,&sign_valid);
        BDBG_MSG(("%s pkcs_1_oaep_decode sha384 sign_valid %d ret %d tmplen %d", __FUNCTION__, sign_valid, ret, tmplen));
        unregister_hash(&sha384_desc);
        break;
    case LAST_SEC_RSA_PADDING:
        BKNI_ReleaseMutex(gRSAMutexLock);
        BDBG_ERR(("SEC_ERROR_BAD_PADDING"));
        return SEC_ERROR_BAD_PADDING ;
        break;
    }
    *pxOutputSize = tmplen;
    if(ret!=CRYPT_OK) {
        BKNI_ReleaseMutex(gRSAMutexLock);
        BDBG_ERR(("Padding error ret = %d!", ret));
        return SEC_ERROR;
    }

    writeIt((unsigned char*)pxOutput,*pxOutputSize);

    free(tmpbuff);
    rsa_free(&rsaKey);
    BKNI_ReleaseMutex(gRSAMutexLock);
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
err:
    rsa_free(&rsaKey);
    BKNI_ReleaseMutex(gRSAMutexLock);
    BDBG_ERR(("SEC_ERROR"));
    return SEC_ERROR;
}
#endif

#ifndef NO_C99
TSecStatus secRsaPrivateDecrypt
(
    const TUnsignedInt8*       pxInput,
    TUnsignedInt8*       pxOutput,
    size_t*              pxOutputSize,
    TUnsignedInt32        xE,
    const TUnsignedInt8*       pxN,
    const TUnsignedInt8*       pxP,
    const TUnsignedInt8*       pxQ,
    const TUnsignedInt8*       pxDP,
    const TUnsignedInt8*       pxDQ,
    const TUnsignedInt8*       pxQInv,
    size_t                xKeySize,
    TSecRsaPadding        xPadding
) {

    int ret = 0;
    unsigned char *tmpbuff;
    unsigned long tmplen;
    unsigned long mudulusbitlen = xKeySize*8;
    int sign_valid = 0;
    int prng_idx;
    int hash_idx;
    rsa_key rsaKey;

    tmpbuff = malloc(xKeySize);
    if(pxInput == NULL || pxOutput == NULL || pxOutputSize == NULL) {
        BDBG_ERR(("Invalid pointer parameter"));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((pxN == NULL) || (pxP == NULL) || (pxQ == NULL) || (pxDQ== NULL) || (pxDP== NULL) || (pxQInv == NULL)) {
        BDBG_ERR(("Invalid pointer parameter 1"));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((xKeySize%8)||(xKeySize<64)||(xKeySize>256)) {
        BDBG_ERR(("Invalid key size %d", xKeySize));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((xE != 3) && (xE != 17) && (xE != 65537)) {
        BDBG_ERR(("Invalid xE %d", xE));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(xPadding >= LAST_SEC_RSA_PADDING) {
        BDBG_ERR(("Invalid xPadding %d", xPadding));
        return SEC_ERROR_BAD_PARAMETER;
    }

    BDBG_MSG(("Enter %s pxOutputSize %d, xKeySize %d xPadding  %d", __FUNCTION__, *pxOutputSize, xKeySize, xPadding));
    if(!gRSAMutexLock) {
        if(BKNI_CreateMutex( &gRSAMutexLock))
        {
            BDBG_ERR( ("BKNI_CreateMutex error") );
            return SEC_ERROR;
        }
    }

    BKNI_AcquireMutex(gRSAMutexLock);

    ltc_mp = ltm_desc;

    register_prng(&sprng_desc);

    prng_idx= find_prng("sprng");
    hash_idx = find_hash("sha1");

    ret = rsa_make_key(NULL,prng_idx,xKeySize,xE,&rsaKey);
    if(ret) {
        BKNI_ReleaseMutex(gRSAMutexLock);
        BDBG_ERR(("rsa_make_key error"));
        return SEC_ERROR;
    }
    ret = mp_read_unsigned_bin((mp_int *)rsaKey.N,pxN,xKeySize);
    if(ret) {
        BDBG_ERR(("mp_read_unsigned_bin error"));
        goto err;
    }
    ret = mp_read_unsigned_bin((mp_int *)rsaKey.p,pxP,xKeySize/2);
    if(ret) {
        BDBG_ERR(("mp_read_unsigned_bin error"));
        goto err;
    }
    ret = mp_read_unsigned_bin((mp_int *)rsaKey.q,pxQ,xKeySize/2);
    if(ret) {
        BDBG_ERR(("mp_read_unsigned_bin error"));
        goto err;
    }
    ret = mp_read_unsigned_bin((mp_int *)rsaKey.dP,pxDP,xKeySize/2);
    if(ret) {
        BDBG_ERR(("mp_read_unsigned_bin error"));
        goto err;
    }
    ret = mp_read_unsigned_bin((mp_int *)rsaKey.dQ,pxDQ,xKeySize/2);
    if(ret) {
        BDBG_ERR(("mp_read_unsigned_bin error"));
        goto err;
    }
    ret = mp_read_unsigned_bin((mp_int *)rsaKey.qP,pxQInv,xKeySize/2);
    if(ret) {
        BDBG_ERR(("mp_read_unsigned_bin error"));
        goto err;
    }
    tmplen = xKeySize;
    ret = ltc_mp.rsa_me(pxInput, xKeySize, tmpbuff, &tmplen, PK_PRIVATE, &rsaKey);
    if(ret != CRYPT_OK) {
        BKNI_ReleaseMutex(gRSAMutexLock);
        BDBG_ERR(("ret != CRYPT_OK"));
        return SEC_ERROR;
    }
    mudulusbitlen = mp_count_bits(rsaKey.N);

    switch(xPadding)
    {
    case SEC_RSA_PKCS1_V1_5_PADDING :
        ret =   pkcs_1_v1_5_decode(tmpbuff,xKeySize,LTC_LTC_PKCS_1_EME,mudulusbitlen,pxOutput,&tmplen,&sign_valid);
        break;
    case SEC_RSA_OAEP_SHA_1_PADDING :
        register_hash(&sha1_desc);
        hash_idx = find_hash("sha1");
        ret =   pkcs_1_oaep_decode(tmpbuff,xKeySize,NULL,0,mudulusbitlen,hash_idx,pxOutput,&tmplen,&sign_valid);
        unregister_hash(&sha1_desc);
        break;
    case SEC_RSA_OAEP_SHA_256_PADDING:
        register_hash(&sha256_desc);
        hash_idx = find_hash("sha256");
        ret =   pkcs_1_oaep_decode(tmpbuff,xKeySize,NULL,0,mudulusbitlen,hash_idx,pxOutput,&tmplen,&sign_valid);
        unregister_hash(&sha256_desc);
        break;
    case SEC_RSA_NO_PADDING:
        memcpy(pxOutput,tmpbuff,xKeySize);
        break;
    case SEC_RSA_OAEP_SHA_384_PADDING:
        register_hash(&sha384_desc);
        hash_idx = find_hash("sha384");
        ret =   pkcs_1_oaep_decode(tmpbuff,xKeySize,NULL,0,mudulusbitlen,hash_idx,pxOutput,&tmplen,&sign_valid);
        unregister_hash(&sha384_desc);
        break;
    case LAST_SEC_RSA_PADDING:
        BDBG_ERR(("Padding error!"));
        return SEC_ERROR_BAD_PADDING;
        break;
    }
    *pxOutputSize = tmplen;
    if(ret!=CRYPT_OK) {
        BKNI_ReleaseMutex(gRSAMutexLock);
        BDBG_ERR(("Padding error 1 !"));
        return SEC_ERROR;
    }

    rsa_free(&rsaKey);
    free(tmpbuff);
    BKNI_ReleaseMutex(gRSAMutexLock);
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
err:
    rsa_free(&rsaKey);
    BKNI_ReleaseMutex(gRSAMutexLock);
    BDBG_ERR(("SEC_ERROR"));
    return SEC_ERROR;
}
#endif

#ifndef NO_C99
TSecStatus secDhGenerateKey
(
    const TUnsignedInt8*   pxG,
    const TUnsignedInt8*   pxP,
    const TUnsignedInt8*   pxInputPrivKey,
    TUnsignedInt8*   pxOutputPrivKey,
    TUnsignedInt8*   pxPubKey,
    size_t            xKeySize
) {
    DH *dhKey = DH_new();
    int ret=0;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if((pxP == NULL) || (pxG == NULL)||
            (pxOutputPrivKey== NULL) || (pxPubKey == NULL)) {
        BDBG_ERR(("SEC_ERROR_BAD_PARAMETER"));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((xKeySize%8)||(xKeySize<64)||(xKeySize>256)) {
        BDBG_ERR(("Invalid key size %d", xKeySize));
        return SEC_ERROR_BAD_PARAMETER;
    }
    dhKey->g=BN_bin2bn(pxG, xKeySize, dhKey->g);
    dhKey->p=BN_bin2bn(pxP, xKeySize, dhKey->p);
    if(pxInputPrivKey)
        dhKey->priv_key=BN_bin2bn(pxInputPrivKey, xKeySize, dhKey->priv_key);

    ret = DH_generate_key(dhKey);
    if(ret!=1) {
        BDBG_ERR(("DH_generate_key error"));
        return SEC_ERROR;
    }
    ret = BN_bn2bin(dhKey->priv_key, pxOutputPrivKey);
    if((unsigned int)ret != xKeySize) {
        memmove(&pxOutputPrivKey[xKeySize-ret],&pxOutputPrivKey[0],ret);
        memset(&pxOutputPrivKey[0],0,xKeySize-ret);
    }
    ret =  BN_bn2bin(dhKey->pub_key, pxPubKey);
    if((unsigned int)ret != xKeySize) {
        memmove(&pxPubKey[xKeySize-ret],&pxPubKey[0],ret);
        memset(&pxPubKey[0],0,xKeySize-ret);
    }

    DH_free(dhKey);
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}
#endif

#ifndef NO_C99
TSecStatus secDhComputeKey
(
    const TUnsignedInt8*   pxP,
    const TUnsignedInt8*   pxPrivKey,
    const TUnsignedInt8*   pxOtherPubKey,
    TUnsignedInt8*   pxSharedSecret,
    size_t            xKeySize
) {
    DH *dhKey = DH_new();
    int ret=0;
    BIGNUM *pub_key = BN_new();

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if((pxP == NULL) ||
            (pxPrivKey== NULL) ||(pxOtherPubKey== NULL) || (pxSharedSecret == NULL)) {
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((xKeySize%8)||(xKeySize<64)||(xKeySize>256)) {
        BDBG_ERR(("Invalid key size %d", xKeySize));
        return SEC_ERROR_BAD_PARAMETER;
    }

    dhKey->p=BN_bin2bn(pxP, xKeySize, dhKey->p);
    dhKey->priv_key = BN_bin2bn(pxPrivKey, xKeySize, dhKey->priv_key);
    pub_key = BN_bin2bn(pxOtherPubKey, xKeySize, pub_key);

    ret = DH_compute_key(pxSharedSecret,pub_key, dhKey);
    if(ret==0) {
        BDBG_ERR(("%s DH_compute_key Failed", __FUNCTION__));
        return SEC_ERROR;
    }
    DH_free(dhKey);
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}
#endif

#ifndef NO_C99
unsigned int sec_P_getNidnumber(unsigned int keySize, unsigned char *q, unsigned char *a, unsigned char *b, unsigned char *GX, unsigned char *GY, unsigned char *n, unsigned char h) {

    unsigned int nid = 0;

    BDBG_MSG(("Enter %s keySize = %d h = %d", __FUNCTION__, keySize, h));

    if((q==NULL)||(a==NULL)||(n==NULL) ||(b==NULL)||(GX==NULL)||(GY==NULL) ) {
        BDBG_ERR(("%s invalid parameter", __FUNCTION__));
        return nid;
    }
    switch(keySize) {
    case 21:
        if(h==1)
            if(!memcmp(q, Curve_Table160[0], keySize))
                if(!memcmp(n, Curve_Table160[1], keySize))
                    if(!memcmp(a, Curve_Table160[2], keySize))
                        if(!memcmp(b, Curve_Table160[3], keySize))
                            if(!memcmp(GX, Curve_Table160[4], keySize))
                                if(!memcmp(GY, Curve_Table160[5], keySize))nid = NID_secp160r1;

        break;
    case 28:
        if(h==1)
            if(!memcmp(q, Curve_Table224[0], keySize))
                if(!memcmp(n, Curve_Table224[1], keySize))
                    if(!memcmp(a, Curve_Table224[2], keySize))
                        if(!memcmp(b, Curve_Table224[3], keySize))
                            if(!memcmp(GX, Curve_Table224[4], keySize))
                                if(!memcmp(GY, Curve_Table224[5], keySize))nid = NID_secp224r1;

        break;
    case 32:
        if(h==1)
            if(!memcmp(q, Curve_Table256[0], keySize))
                if(!memcmp(n, Curve_Table256[1], keySize))
                    if(!memcmp(a, Curve_Table256[2], keySize))
                        if(!memcmp(b, Curve_Table256[3], keySize))
                            if(!memcmp(GX, Curve_Table256[4], keySize))
                                if(!memcmp(GY, Curve_Table256[5], keySize))nid = NID_X9_62_prime256v1;

        break;
    case 48:
        if(h==1)
            if(!memcmp(q, Curve_Table384[0], keySize))
                if(!memcmp(n, Curve_Table384[1], keySize))
                    if(!memcmp(a, Curve_Table384[2], keySize))
                        if(!memcmp(b, Curve_Table384[3], keySize))
                            if(!memcmp(GX, Curve_Table384[4], keySize))
                                if(!memcmp(GY, Curve_Table384[5], keySize))nid = NID_secp384r1;

        break;
    default :
        BDBG_ERR(("%s unsupported keysize", __FUNCTION__));
        nid=0;
        break;

    }
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return nid;
}
#endif

#ifndef NO_C99
TSecStatus secEcdhGenerateKey
(
    TSecEcParams     xParams,
    const TUnsignedInt8*  pxInputPrivKey,
    TUnsignedInt8*  pxOutputPrivKey,
    TUnsignedInt8*  pxPubKeyX,
    TUnsignedInt8*  pxPubKeyY
) {
    EC_GROUP *ec_group =NULL;
    EC_KEY   *eckey = NULL;
    EC_POINT *Pt;
    BIGNUM *pvtkey = BN_new();
    BIGNUM *opvtkey = BN_new();
    BIGNUM *pubKeyX = BN_new();
    BIGNUM *pubKeyY = BN_new();
    int ret;
    unsigned int nid = 0;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if((pxOutputPrivKey == NULL) ||
            (pxPubKeyX == NULL) ||(pxPubKeyY == NULL)) {
        BDBG_ERR(("SEC_ERROR_BAD_PARAMETER"));
        return SEC_ERROR_BAD_PARAMETER;
    }
 
    if(!xParams.h) {
        BDBG_ERR(("SEC_ERROR_BAD_PARAMETER 1"));
        return SEC_ERROR_BAD_PARAMETER;
    }

    nid = sec_P_getNidnumber(xParams.keySize,xParams.q,xParams.a,xParams.b,xParams.GX,xParams.GY,xParams.n,xParams.h[xParams.keySize-1]);
    BDBG_MSG(("nid = %d", nid));
    if(!nid) {
        BDBG_ERR(("!nid"));
        return SEC_ERROR_BAD_PARAMETER;
    }
    eckey = EC_KEY_new_by_curve_name(nid);
#if 1
    ret= EC_KEY_generate_key(eckey);
    if(ret <0) {
        BDBG_ERR(("Error call EC_KEY_generate_key"));
        return SEC_ERROR;
    }
#endif

    if(pxInputPrivKey) {
        pvtkey=BN_bin2bn(pxInputPrivKey, xParams.keySize, pvtkey);
        ret = EC_KEY_set_private_key(eckey, pvtkey);
        if(ret <0) {
            BDBG_ERR(("Error call EC_KEY_set_private_key"));
            return SEC_ERROR;
        }
    }

    opvtkey = (BIGNUM*)EC_KEY_get0_private_key(eckey);
    ret = BN_bn2bin(opvtkey, pxOutputPrivKey);
    if((unsigned int)ret != xParams.keySize) {
        memmove(&pxOutputPrivKey[xParams.keySize-ret],&pxOutputPrivKey[0],ret);
        memset(&pxOutputPrivKey[0],0,xParams.keySize-ret);
    }
    ec_group = (EC_GROUP*)EC_KEY_get0_group(eckey);
    Pt = EC_POINT_new(ec_group);
    Pt = (EC_POINT*)EC_KEY_get0_public_key(eckey);


    if (EC_METHOD_get_field_type(EC_GROUP_method_of(ec_group)) ==
            NID_X9_62_prime_field) {
        if (!EC_POINT_get_affine_coordinates_GFp(ec_group, Pt, pubKeyX, pubKeyY,
                NULL)) {
                BDBG_ERR(("Error call EC_POINT_get_affine_coordinates_GFp"));
                return SEC_ERROR;
        }

    } else {
        if (!EC_POINT_get_affine_coordinates_GF2m(ec_group, Pt, pubKeyX, pubKeyY,
                NULL)) {
                BDBG_ERR(("Error call EC_POINT_get_affine_coordinates_GF2m"));
                return SEC_ERROR;
    }

    }

    ret = BN_bn2bin(pubKeyX, pxPubKeyX);
    if((unsigned int)ret != xParams.keySize) {
        memmove(&pxPubKeyX[xParams.keySize-ret],&pxPubKeyX[0],ret);
        memset(&pxPubKeyX[0],0,xParams.keySize-ret);
    }
    ret = BN_bn2bin(pubKeyY, pxPubKeyY);
    if((unsigned int)ret != xParams.keySize) {
        memmove(&pxPubKeyY[xParams.keySize-ret],&pxPubKeyY[0],ret);
        memset(&pxPubKeyY[0],0,xParams.keySize-ret);
    }

    BN_free(pvtkey);
    BN_free(opvtkey);
    BN_free(pubKeyX);
    BN_free(pubKeyY);

    EC_POINT_free(Pt);

    EC_GROUP_free(ec_group);
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;

}
#endif

#ifndef NO_C99
TSecStatus secEcdhComputeKey
(

    TSecEcParams	   xParams,
    const TUnsignedInt8*  pxPrivKey,
    const TUnsignedInt8*  pxOtherPubKeyX,
    const TUnsignedInt8*  pxOtherPubKeyY,
    TUnsignedInt8*  pxSharedSecret

) {
    EC_GROUP *ec_group =NULL;
    EC_KEY	 *eckey = NULL;
    EC_POINT *Pt;
    EC_POINT *shared_point;

    BIGNUM *pvtkey = BN_new();
    BIGNUM *pubKeyX = BN_new();
    BIGNUM *pubKeyY = BN_new();
    BIGNUM *shared_point_x = BN_new();
    BIGNUM *shared_point_y = BN_new();
    unsigned int nid;
    int ret;


    BDBG_MSG(("Enter %s", __FUNCTION__));

    if((pxPrivKey == NULL) ||(pxSharedSecret == NULL) ||
            (pxOtherPubKeyX == NULL) ||(pxOtherPubKeyY == NULL)) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }
 
    if(!xParams.h) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER 1", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    nid = sec_P_getNidnumber(xParams.keySize,xParams.q,xParams.a,xParams.b,xParams.GX,xParams.GY,xParams.n,xParams.h[xParams.keySize-1]);
#ifdef SEC_DEBUG
    BDBG_ERR(("nid = %d", nid));
#endif
    if(!nid) {
        BDBG_ERR(("%s sec_P_getNidnumber failed", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }
    eckey = EC_KEY_new_by_curve_name(nid);
    BDBG_MSG(("Privkey %x, %d, %x", pxPrivKey, xParams.keySize, eckey));
    writeIt((unsigned char*)pxPrivKey,xParams.keySize);


    pvtkey=BN_bin2bn(pxPrivKey, xParams.keySize, pvtkey);
    ret = EC_KEY_set_private_key(eckey, pvtkey);
#if 1

    if(ret <0) {
        BDBG_ERR(("Error call EC_KEY_set_private_key\n"));
        return SEC_ERROR;
    }
#ifdef SC_DEBUG
    printf("\nPubkeyX\n");
    for(i=0; i<xParams.keySize; i++) printf("%02x ", pxOtherPubKeyX[i]);
    printf("\nPubkeyY\n");
    for(i=0; i<xParams.keySize; i++) printf("%02x ", pxOtherPubKeyY[i]);
#endif
    ec_group = (EC_GROUP*)EC_KEY_get0_group(eckey);
    Pt = EC_POINT_new(ec_group);
    pubKeyX = BN_bin2bn(pxOtherPubKeyX,xParams.keySize,pubKeyX);
    pubKeyY= BN_bin2bn(pxOtherPubKeyY,xParams.keySize,pubKeyY);
    if (EC_METHOD_get_field_type(EC_GROUP_method_of(ec_group)) ==
            NID_X9_62_prime_field) {
        ret = EC_POINT_set_affine_coordinates_GFp(ec_group, Pt, pubKeyX, pubKeyY, NULL);
        if(ret <0) {
            BDBG_ERR(("Error call EC_POINT_set_affine_coordinates_GFp\n"));
            return SEC_ERROR;
        }
    } else {
        ret = EC_POINT_set_affine_coordinates_GF2m(ec_group, Pt, pubKeyX, pubKeyY, NULL);
        if(ret <0) {
            BDBG_ERR(("Error call EC_POINT_set_affine_coordinates_GFp\n"));
            return SEC_ERROR;
        }
    }
    EC_KEY_set_public_key(eckey,Pt);

    ret = EC_KEY_check_key(eckey);
    /*if(ret){

    BDBG_ERR(("Error call EC_KEY_check_key\n"));
    return SEC_ERROR;
    }
    */
    shared_point = EC_POINT_new(ec_group);

    ret =EC_POINT_mul(ec_group,shared_point,NULL,Pt,pvtkey,NULL);
    if(ret!=1) {
        BDBG_ERR(("EC_POINT_mul falied"));
        return SEC_ERROR;
    }

#if 1
    if (EC_METHOD_get_field_type(EC_GROUP_method_of(ec_group)) ==
            NID_X9_62_prime_field) {
        ret = EC_POINT_get_affine_coordinates_GFp(ec_group,shared_point,shared_point_x,shared_point_y,NULL);
    } else {

        ret = EC_POINT_get_affine_coordinates_GF2m(ec_group, shared_point, shared_point_x, shared_point_y, NULL);
    }

    if(ret <0) {
        BDBG_ERR(("Error call EC_POINT_get_affine_coordinates_GFp\n"));
        return SEC_ERROR;
    }
    ret = BN_bn2bin(shared_point_x,pxSharedSecret);
    if((unsigned int)ret != xParams.keySize) {
        memmove(&pxSharedSecret[xParams.keySize-ret],&pxSharedSecret[0],ret);
        memset(&pxSharedSecret[0],0,xParams.keySize-ret);
    }


#else
    EC_POINT_point2bn(ec_group, shared_point,EC_GROUP_get_point_conversion_form(ec_group) ,shared_point_x, NULL);
    BN_bn2bin(shared_point_x,pxSharedSecret);
#endif
    BN_free(pubKeyX);

    /*BN_free(pubKeyY); */

    BN_free(pvtkey);
    BN_free(shared_point_x);
    BN_free(shared_point_y);
    EC_POINT_free(shared_point);
    EC_POINT_free(Pt);
    EC_GROUP_free(ec_group);
#endif
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;


}
#endif

#ifndef NO_C99
TSecStatus secEcdsaGenerateKey
(
    TSecEcParams     xParams,
    const TUnsignedInt8*  pxInputPrivKey,
    TUnsignedInt8*  pxOutputPrivKey,
    TUnsignedInt8*  pxPubKeyX,
    TUnsignedInt8*  pxPubKeyY
) {
    TSecStatus retval;
    BDBG_MSG(("Enter %s", __FUNCTION__));
    retval = secEcdhGenerateKey(xParams, pxInputPrivKey, pxOutputPrivKey, pxPubKeyX, pxPubKeyY);
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return retval;
}
#endif

#ifndef NO_C99
TSecStatus secEcdsaSign
(
    TSecEcParams 	xParams,
    TSecHashType 	xHashType,
    const TUnsignedInt8*  pxPrivKey,
    const TUnsignedInt8*  pxMessage,
    size_t			xMessageSize,
    TUnsignedInt8*  pxSigR,
    TUnsignedInt8*  pxSigS

) {
    EC_KEY	 *eckey = EC_KEY_new();
    BIGNUM *pvtkey = BN_new();
    unsigned int nid;
    unsigned char *digest;
    unsigned int dgst_len;
    ECDSA_SIG *signature = NULL;
    EVP_MD_CTX md_ctx;
    const EC_GROUP *group;
    int ret;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if((pxPrivKey == NULL) ||(pxMessage == NULL) ||(pxSigR == NULL) ||(pxSigS == NULL)) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }
    nid = sec_P_getNidnumber(xParams.keySize,xParams.q,xParams.a,xParams.b,xParams.GX,xParams.GY,xParams.n,xParams.h[xParams.keySize-1]);
#ifdef SEC_DEBUG
    BDBG_ERR(("nid = %d", nid));
#endif
    if(!nid) {
        BDBG_ERR(("%s sec_P_getNidnumber failed", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }
    eckey = EC_KEY_new_by_curve_name(nid);
    pvtkey=BN_bin2bn(pxPrivKey, xParams.keySize, pvtkey);
    group = EC_KEY_get0_group(eckey);
    ret = EC_KEY_set_private_key(eckey, pvtkey);
    if(ret <0) {
        BDBG_ERR(("Error call EC_KEY_set_private_key\n"));
        return SEC_ERROR;
    }

    ret =EC_KEY_check_key(eckey);

    EVP_MD_CTX_init(&md_ctx);
    if(xHashType == SEC_HASH_SHA1) {

        EVP_DigestInit(&md_ctx, EVP_sha1());

        digest = malloc(SHA_DIGEST_LENGTH*sizeof(unsigned char));
    }
    else if(xHashType ==SEC_HASH_SHA256) {

        EVP_DigestInit(&md_ctx, EVP_sha256());
        digest = malloc(SHA256_DIGEST_LENGTH*sizeof(unsigned char));
    }
    else {
        BDBG_ERR(("%s xHashType Error", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    EVP_DigestUpdate(&md_ctx, (const void*)pxMessage, xMessageSize);

    EVP_DigestFinal(&md_ctx, digest, &dgst_len);

#if 0
    ret = ECDSA_sign(0, pxMessage, xMessageSize,
                     pxSignature,&sig_len, eckey);
    if(ret!=1) {
        BDBG_ERR(("Error calling ECDSA_sign()\n"));
        return SEC_ERROR;
    }
#endif
    signature = ECDSA_do_sign(digest, dgst_len, eckey);

    ret = BN_bn2bin(signature->r,pxSigR);
    if((unsigned int)ret != xParams.keySize) {
        memmove(&pxSigR[xParams.keySize-ret],&pxSigR[0],ret);
        memset(&pxSigR[0],0,xParams.keySize-ret);
    }
    ret =  BN_bn2bin(signature->s,pxSigS);
    if((unsigned int)ret != xParams.keySize) {
        memmove(&pxSigS[xParams.keySize-ret],&pxSigS[0],ret);
        memset(&pxSigS[0],0,xParams.keySize-ret);
    }
#if 0
    p = EC_POINT_new(group);

    EC_POINT_set_affine_coordinates_GFp(group,p,signature->r,signature->s,NULL);
    EC_POINT_point2bn(group,p,EC_GROUP_get_point_conversion_form(group),bn_p,NULL);
    sig_len = BN_bn2bin(bn_p,pxSignature);

    EC_POINT_free(p);
#endif
    EC_GROUP_free((EC_GROUP *)group);
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}
#endif

#ifndef NO_C99
TSecStatus secEcdsaVerify
(
    TSecEcParams	   xParams,
    TSecHashType	   xHashType,
    const TUnsignedInt8*  pxPubKeyX,
    const TUnsignedInt8*  pxPubKeyY,
    const TUnsignedInt8*  pxMessage,
    size_t		   xMessageSize,
    const TUnsignedInt8*  pxSigR,
    const TUnsignedInt8*  pxSigS

) {
    EC_GROUP *ec_group =NULL;
    EC_KEY	 *eckey = NULL;
    EC_POINT *Pt;
    BIGNUM *pubKeyX= BN_new();
    BIGNUM *pubKeyY= BN_new();
    ECDSA_SIG *signature = NULL;
    unsigned int nid;
    unsigned char *digest;
    unsigned int dgst_len;
    EVP_MD_CTX md_ctx;
    int ret;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if((pxPubKeyX == NULL) ||(pxPubKeyY == NULL) ||(pxMessage == NULL) ||
            (pxSigR == NULL) ||(pxSigS == NULL)) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(!xParams.h) {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER 1", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    nid = sec_P_getNidnumber(xParams.keySize,xParams.q,xParams.a,xParams.b,xParams.GX,xParams.GY,xParams.n,xParams.h[xParams.keySize-1]);
#ifdef SEC_DEBUG
    BDBG_ERR(("nid = %d", nid));
#endif
    if(!nid) {
        BDBG_ERR(("%s sec_P_getNidnumber Error", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }


    eckey = EC_KEY_new_by_curve_name(nid);
    ec_group = (EC_GROUP*)EC_KEY_get0_group(eckey);
    Pt = EC_POINT_new(ec_group);
    pubKeyX=BN_bin2bn(pxPubKeyX, xParams.keySize, pubKeyX);
    pubKeyY=BN_bin2bn(pxPubKeyY, xParams.keySize, pubKeyY);
    if (EC_METHOD_get_field_type(EC_GROUP_method_of(ec_group)) ==
            NID_X9_62_prime_field) {
        ret = EC_POINT_set_affine_coordinates_GFp(ec_group, Pt, pubKeyX, pubKeyY, NULL);
        if(ret <0) {
            BDBG_ERR(("Error call EC_POINT_set_affine_coordinates_GFp\n"));
            goto err;
        }
    } else {
        ret = EC_POINT_set_affine_coordinates_GFp(ec_group, Pt, pubKeyX, pubKeyY, NULL);
        if(ret <0) {
            BDBG_ERR(("Error call EC_POINT_set_affine_coordinates_GFp\n"));
            goto err;
        }
    }
    ret = EC_KEY_set_public_key(eckey,Pt);
    if(ret==0) {
        BDBG_ERR(("%s EC_KEY_set_public_key error", __FUNCTION__));
        goto err;
    }
    ret = EC_KEY_check_key(eckey);
    if(ret==0) {
        BDBG_ERR(("%s EC_KEY_check_key error", __FUNCTION__));
        goto err;
    }

    EVP_MD_CTX_init(&md_ctx);
    /* get the message digest */
    if(xHashType == SEC_HASH_SHA1) {
        EVP_DigestInit(&md_ctx, EVP_sha1());
        digest = malloc(SHA_DIGEST_LENGTH*sizeof(unsigned char));
    }
    else if(xHashType ==SEC_HASH_SHA256) {
        EVP_DigestInit(&md_ctx, EVP_sha256());
        digest = malloc(SHA256_DIGEST_LENGTH*sizeof(unsigned char));
    }
    else {
        BDBG_ERR(("%s xHashType error", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    ret = EVP_DigestUpdate(&md_ctx, (const void*)pxMessage, xMessageSize);
    if(ret==0) {
        BDBG_ERR(("%s EVP_DigestUpdate error", __FUNCTION__));
        goto err;
    }
    ret = EVP_DigestFinal(&md_ctx, digest, &dgst_len);
    if(ret==0) {
        BDBG_ERR(("%s EVP_DigestFinal error", __FUNCTION__));
        goto err;
    }

    signature = ECDSA_SIG_new();


    signature->r = BN_bin2bn(pxSigR,xParams.keySize,signature->r);
    signature->s = BN_bin2bn(pxSigS,xParams.keySize,signature->s );

    ret =ECDSA_do_verify(digest,dgst_len,signature,eckey);
    if(ret !=1) {
        BDBG_ERR(("ECDSA SIGN FAIL"));
        return SEC_ERROR_BAD_SIGNATURE;
    }
    BDBG_MSG(("ECDSA SIGN OK"));
    return SEC_NO_ERROR;
err:

    BDBG_ERR(("%s SEC_ERROR", __FUNCTION__));
    return SEC_ERROR;
}
#endif

TSecStatus secGenerateRandomBytes
(
    size_t           xNumOfBytes,
    TUnsignedInt8*  pxRandomBytes
) {
    NEXUS_RandomNumberGenerateSettings settings;
    NEXUS_RandomNumberOutput output;
    unsigned int NumOfBytesToGenerate = 0;
    unsigned int TotalBytesGenerated = 0;


    BDBG_MSG(("Enter %s xNumOfBytes = %d", __FUNCTION__, xNumOfBytes));

    if(pxRandomBytes == NULL) {
        BDBG_ERR(("%s pxRandomBytes == NULL ", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if((xNumOfBytes >1024 )|| (!xNumOfBytes)) {
        BDBG_ERR(("%s xNumOfBytes Error %d passed to function", __FUNCTION__, xNumOfBytes));
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* NEXUS_RandomNumber_Generate has a maximum size of 344: need to call multiple times to generate up to the required max of 1024 bytes */
    while(TotalBytesGenerated < xNumOfBytes) {
        NumOfBytesToGenerate = ((xNumOfBytes - TotalBytesGenerated) > 344) ? 344 : (xNumOfBytes - TotalBytesGenerated);
        BDBG_MSG(("%s: a NumOfBytesToGenerate %d TotalBytesGenerated %d", __FUNCTION__, NumOfBytesToGenerate, TotalBytesGenerated));
        NEXUS_RandomNumber_GetDefaultGenerateSettings(&settings);
        settings.randomNumberSize =( NumOfBytesToGenerate/4)*4;
        if(NumOfBytesToGenerate%4)settings.randomNumberSize +=4;
        if(NEXUS_RandomNumber_Generate(&settings, &output)) {
            BDBG_ERR(("%s NEXUS_RandomNumber_Generate error", __FUNCTION__));
            return SEC_ERROR;
        }
        BKNI_Memcpy(pxRandomBytes+TotalBytesGenerated, output.buffer, NumOfBytesToGenerate);
        TotalBytesGenerated += NumOfBytesToGenerate;
        BDBG_MSG(("%s: b TotalBytesGenerated %d", __FUNCTION__, TotalBytesGenerated));
    }

    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

TUnsignedInt32 secApiVersion(void)
{
    BDBG_MSG(("Enter %s", __FUNCTION__));
    return SECAPI_VERSION_INT;
}

/****************************************************************************************************/
/***************************************Start 5.1.1 API additions *****************************************/
/****************************************************************************************************/

void secChipReset
(
    void
) {
    BDBG_MSG(("Enter %s", __FUNCTION__));
#if 0 /* HUMAX_PLATFORM_BASE */
    BKNI_Fail();
#endif
    system("reboot");
}


TSecStatus secOpenStreamEncryptSession(
    TSecCipherSession*     pxSession,
    TTransportSessionId     xTransportSessionId
) {
    TCsdStatus status = CSD_ERROR;
    TUnsignedInt8 *pxIv = NULL;
    size_t xIvSize = 0;
    TSessionManagerSessionType xSessionType = SESSION_TYPE_UNDEFINED;

    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(!pxSession) {
        BDBG_ERR(("secOpenStreamEncryptSession !pxSession")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(xTransportSessionId == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("secOpenStreamEncryptSession xTransportSessionId == TRANSPORT_SESSION_ID_INVALID")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    *pxSession = brcmCreateSecCipherSession_priv();
    if(!*pxSession) {
        BDBG_ERR(("secOpenStreamEncryptSession: brcmCreateSecCipherSession_priv failed")) ;
        return SEC_ERROR;
    }

    (*pxSession)->xTransportSessionId = xTransportSessionId;
    (*pxSession)->type = SEC_SESSION_STREAM_ENCRYPT_TYPE;
    (*pxSession)->bEncrypt = true;

    /* See if a session type has been set */
    if(brcmCaGetTransportSessionType(xTransportSessionId, &xSessionType)!=SESSION_MANAGER_NO_ERROR) {
        BDBG_MSG(("brcmCaGetTransportSessionType failed")) ;
    }

    /* We're encrypting, so it must be SCR */
    switch (xSessionType)
    {
        case SESSION_TYPE_UNDEFINED:
            /* Set it to SCR */
            brcmCaSetTransportSessionType(xTransportSessionId, SESSION_TYPE_SCR);
            break;

        case SESSION_TYPE_DSC:
            /* Invalid for encryption */
            BDBG_ERR(("Attempt to use DSC transport session for encryption"));
            brcmDestroySecCipherSession_priv(*pxSession);
            return SEC_ERROR;

        case SESSION_TYPE_SCR:
            /* OK */
            break;
    }

    status = csdBrcmOpenScrKeyPath((TCsdScrKeyPathHandle**)&(*pxSession)->pScrKeyPath, xTransportSessionId);
    if((status!=CSD_NO_ERROR)||!(*pxSession)->pScrKeyPath) {
        BDBG_ERR(("csdBrcmOpenScrKeyPath failed")) ;
        brcmDestroySecCipherSession_priv(*pxSession);
        return SEC_ERROR;
    }

    /* notify the session manager about the keyhandle*/
    if(brcmCaAssociateM2MKeyslotHandle(xTransportSessionId, csdBrcmGetScrKeyHandle((TCsdScrKeyPathHandle*)(*pxSession)->pScrKeyPath))!=SESSION_MANAGER_NO_ERROR) {
        BDBG_MSG(("brcmCaAssociateM2MKeyslotHandle failed")) ;
    }

    if(brcmCaSetScrKeyPath(xTransportSessionId, (*pxSession)->pScrKeyPath)!=SESSION_MANAGER_NO_ERROR) {
        BDBG_MSG(("brcmCaAssociateM2MKeyslotHandle failed")) ;
    }

    /*register for IV notifications on session manager module */
    if(brcmCaRegisterTSIdIvNotificationCallback(xTransportSessionId, brcmIvChangedCallback_priv, &pxIv, &xIvSize)!=SESSION_MANAGER_NO_ERROR) {
        BDBG_MSG(("brcmCaRegisterTSIdPidNotificationCallback failed")) ;
    }

    /* Set any IV the session manager already knows about */
    if(NULL != pxIv)
    {
        if (brcmSecSetSessionStreamIv_priv(*pxSession, pxIv, xIvSize)) {
            BDBG_MSG(("brcmSecSetSessionStreamIv_priv failed")) ;
        }
        BKNI_Free(pxIv);
    }

    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}


TSecStatus secOpenStreamDecryptSession(
    TSecCipherSession*     pxSession,
    TTransportSessionId     xTransportSessionId
) {
    TUnsignedInt16 *pPidList = 0;
    unsigned int xNumberOfPids = 0;
    unsigned int i = 0;
    TUnsignedInt8 *pxIv = NULL;
    size_t xIvSize = 0;
    TSessionManagerSessionType xSessionType = SESSION_TYPE_UNDEFINED;
    TCsdStatus status = CSD_ERROR;

    BDBG_MSG(("Enter %s xTransportSessionId = %d", __FUNCTION__,xTransportSessionId));

    if(!pxSession) {
        BDBG_ERR(("secOpenStreamDecryptSession !pxSession")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(xTransportSessionId == TRANSPORT_SESSION_ID_INVALID) {
        BDBG_ERR(("secOpenStreamDecryptSession xTransportSessionId == TRANSPORT_SESSION_ID_INVALID")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    *pxSession = brcmCreateSecCipherSession_priv();
    if(!*pxSession) {
        BDBG_ERR(("secOpenStreamDecryptSession xTransportSessionId == TRANSPORT_SESSION_ID_INVALID")) ;
        return SEC_ERROR;
    }

    if(brcmCaGetPlaypumpHandle(xTransportSessionId, &(*pxSession)->playpumpHandle)!=SESSION_MANAGER_NO_ERROR) {
        BDBG_MSG(("brcmCaGetPlaypumpHandle failed")) ;
    }

    /* See if a session type has been set */
    if(brcmCaGetTransportSessionType(xTransportSessionId, &xSessionType)!=SESSION_MANAGER_NO_ERROR) {
        BDBG_MSG(("brcmCaGetTransportSessionType failed")) ;
    }

    (*pxSession)->xTransportSessionId = xTransportSessionId;
    (*pxSession)->bEncrypt = false;

    /* If not, allocate both types of keys for now */
    if ((xSessionType == SESSION_TYPE_UNDEFINED) || (xSessionType == SESSION_TYPE_DSC)) {
        status = csdBrcmOpenDscKeyPath(&(*pxSession)->pDscKeyPath, xTransportSessionId, (*pxSession)->playpumpHandle);
        if((status!=CSD_NO_ERROR)||!(*pxSession)->pDscKeyPath) {
            BDBG_ERR(("csdBrcmOpenDscKeyPath failed")) ;
            brcmDestroySecCipherSession_priv(*pxSession);
            return SEC_ERROR;
        }
    }

    if ((xSessionType == SESSION_TYPE_UNDEFINED) || (xSessionType == SESSION_TYPE_SCR)) {
        status = csdBrcmOpenScrKeyPath(&(*pxSession)->pScrKeyPath, xTransportSessionId);
        if((status!=CSD_NO_ERROR)||!(*pxSession)->pScrKeyPath) {
            BDBG_ERR(("csdBrcmOpenDscKeyPath failed")) ;
            brcmDestroySecCipherSession_priv(*pxSession);
            return SEC_ERROR;
        }

        /* notify the session manager about the scr keyhandle (for block mode playbacks)*/
        if(brcmCaAssociateM2MKeyslotHandle(xTransportSessionId, csdBrcmGetScrKeyHandle((*pxSession)->pScrKeyPath))!=SESSION_MANAGER_NO_ERROR) {
            BDBG_MSG(("brcmCaAssociateM2MKeyslotHandle failed")) ;
        }

        if(brcmCaSetScrKeyPath(xTransportSessionId, (*pxSession)->pScrKeyPath)!=SESSION_MANAGER_NO_ERROR) {
            BDBG_MSG(("brcmCaAssociateM2MKeyslotHandle failed")) ;
        }
    }

    (*pxSession)->type = SEC_SESSION_STREAM_DECRYPT_TYPE;

    if ((*pxSession)->pDscKeyPath) {
        /*register for PID notifications on Pid manager module */
        if(brcmCaRegisterTSIdPidNotificationCallback(xTransportSessionId, brcmPidChangedCallback_priv, &pPidList, &xNumberOfPids)!=SESSION_MANAGER_NO_ERROR) {
            BDBG_MSG(("brcmCaRegisterTSIdPidNotificationCallback failed")) ;
        }

        /* Set any pids the session manager already knows about */
        for(i=0; i<xNumberOfPids; i++) {
            BDBG_MSG(("calling csdBrcmSetDscPid for pid 0x%x xNumberOfPids=%d", pPidList[i], xNumberOfPids)) ;
            status = csdBrcmSetDscPid((TCsdDscKeyPathHandle*)(*pxSession)->pDscKeyPath, pPidList[i]);
            if(status!=CSD_NO_ERROR) {
                BDBG_ERR(("csdBrcmSetDscPid failed for pid 0x%x", pPidList[i])) ;
            }
        }

        /* free the pid list (as caller owns memory)*/
        if(pPidList && xNumberOfPids) {
            BKNI_Free(pPidList);
        }
    }

    /*register for IV notifications on session manager module */
    if(brcmCaRegisterTSIdIvNotificationCallback(xTransportSessionId, brcmIvChangedCallback_priv, &pxIv, &xIvSize)!=SESSION_MANAGER_NO_ERROR) {
        BDBG_MSG(("brcmCaRegisterTSIdPidNotificationCallback failed")) ;
    }

    /* Set any IV the session manager already knows about */
    if(NULL != pxIv)
    {
        if (brcmSecSetSessionStreamIv_priv(*pxSession, pxIv, xIvSize)) {
            BDBG_MSG(("brcmSecSetSessionStreamIv_priv failed")) ;
        }
        BKNI_Free(pxIv);
    }

    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}


TSecStatus secSet0LevelProtectedKey(
    TSecCipherSession   xSession,
    TUnsignedInt16      xEmi,
    size_t              xKeyIdSize,
    TUnsignedInt8*     pxKeyId,
    size_t              xCipheredContentKeySize,
    const TUnsignedInt8*     pxCipheredContentKey
) {
    TCsdDscKeyPathHandle* pDscKeyPath = 0;
    TCsdScrKeyPathHandle* pScrKeyPath = 0;
    TCsdStatus rc;
    TCsdUnsignedInt8 *  pIv=NULL;
    size_t              IvSize=0;
    size_t              requiredIvSize=0;
    NEXUS_PlaypumpHandle playpumpHandle;
    BDBG_MSG(("Enter %s xSession=%p xEmi=0x%x", __FUNCTION__, xSession, xEmi));

    if(!xCipheredContentKeySize || !pxCipheredContentKey) {
        BDBG_ERR(("invalid key pointer")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if (!brcmEmiIsValid(xEmi)) {
        BDBG_ERR(("invalid xEmi")) ;
        return SEC_ERROR_BAD_EMI;
    }

    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("secSet0LevelProtectedKey xSession not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* only usefull for sream type sessions */
    if((xSession->type != SEC_SESSION_STREAM_DECRYPT_TYPE) && (xSession->type != SEC_SESSION_STREAM_ENCRYPT_TYPE)) {
        BDBG_ERR(("%s SEC_ERROR", __FUNCTION__));
        return SEC_ERROR_BAD_USAGE;
    }

    /* if this is Stream type pxKeyId should have value only if is a MPEG TS EMI */
    if(brcmEmiIsMpegTs(xEmi)){
        if(!xKeyIdSize || !pxKeyId) {
            BDBG_ERR(("xKeyIdSize NOT passed for MPEG TS EMI Stream session")) ;
            return SEC_ERROR_BAD_PARAMETER;
        }
    }
    /* Note: The NOCS3 test harness always passes pxKeyId regardless */
    /* of EMI, so we just ignore this case. */
#ifndef RPC_TEST_HARNESS_WORKAROUND
    else {
        if(xKeyIdSize || pxKeyId) {
            BDBG_ERR(("xKeyIdSize passed for Block mode Stream session")) ;
            return SEC_ERROR_BAD_PARAMETER;
        }
    }
#endif

    if (xCipheredContentKeySize != brcmEmiGetKeySize(xEmi)) {
        BDBG_ERR(("%s Invalid xCipheredContentKeySize", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    requiredIvSize = brcmEmiGetIvSize(xEmi);
    if (requiredIvSize > 0)
    {
        /* If its a zero IV EMI, use the zero buffer */
        if (brcmEmiIsZeroIv(xEmi))
        {
            IvSize = requiredIvSize;
            pIv = xZeroVector;
            /*printf("Setting EMI zero IV of %d bytes\n", IvSize);*/
        }
        else
        {
            /* Has someone (somehow) configured an IV? */
            if (xSession->xIvSize > 0)
            {
                if (xSession->xIvSize != requiredIvSize)
                {
                    BDBG_WRN(("%s Incorrect IV size set (%d vs. %d)", __FUNCTION__, xSession->xIvSize, requiredIvSize));
                }

                pIv = xSession->xIv;
                IvSize = xSession->xIvSize;
                /*printf("Setting session IV of %d bytes\n", IvSize);*/
            }
            /* Is the key id the right size? */
            else if ((pxKeyId != NULL) && (xKeyIdSize == requiredIvSize))
            {
                pIv = pxKeyId;
                IvSize = xKeyIdSize;
                /*printf("Setting keyid IV of %d bytes\n", IvSize);*/
            }
            /* No, default to zero IV */
            else
            {
                IvSize = requiredIvSize;
                pIv = xZeroVector;
                /*printf("Setting zero IV of %d bytes\n", IvSize);*/
            }
        }
    }

    if(SEC_NO_ERROR != brcmSecDetermineScrOrDsc_priv(xEmi, xSession)) {
        BDBG_ERR(("brcmSecDetermineScrOrDsc_priv failed"));
#if 0 /* HUMAX_PLATFORM_BASE */
	return SEC_NO_ERROR; /* In case of DAL-SEC test, TransportSessionId is not added in ca session manager. But when all parameters are ok, test is success */
#else
        return SEC_ERROR;
#endif
    }

    if(xSession->pDscKeyPath)
    {
        /* If the playpumpHandle has changed, notify CSD */
        if(brcmCaGetPlaypumpHandle(xSession->xTransportSessionId, &playpumpHandle)==SESSION_MANAGER_NO_ERROR) {
            if (playpumpHandle != xSession->playpumpHandle) {
                if (CSD_NO_ERROR != csdBrcmSetDscPlaypumpHandle(xSession->pDscKeyPath, playpumpHandle)) {
                    BDBG_ERR(("csdBrcmSetDscPlaypumpHandle failed"));
                    return SEC_ERROR;
                }
                xSession->playpumpHandle = playpumpHandle;
            }
        }

        pDscKeyPath = xSession->pDscKeyPath;
        pDscKeyPath->initializationVector = pIv;
        rc = csdSetClearTextDscHostKeys(xEmi,
                                ((*pxKeyId)==1 ? (TCsdUnsignedInt8*)pxCipheredContentKey : 0), (*pxKeyId)==1 ? xCipheredContentKeySize : 0,
                                ((*pxKeyId)==0 ? (TCsdUnsignedInt8*)pxCipheredContentKey : 0), (*pxKeyId)==0 ? xCipheredContentKeySize : 0,
                                pDscKeyPath);

    }
    else
    {
    	pScrKeyPath = xSession->pScrKeyPath;
		if( !xSession->bEncrypt)
        {
            rc = csdSetClearTextScrDecryptionKey(xEmi, 
                            (TCsdUnsignedInt8 *) pxCipheredContentKey, xCipheredContentKeySize, 
                            (TCsdUnsignedInt8 *) pIv, IvSize, 
                            pScrKeyPath);
        }
		else {
            rc = csdSetClearTextScrEncryptionKey(xEmi, 
                            (TCsdUnsignedInt8 *) pxCipheredContentKey, xCipheredContentKeySize, 
                            (TCsdUnsignedInt8 *) pIv, IvSize, 
                            pScrKeyPath);
        }
		 	
    }

    if(rc == CSD_ERROR_INVALID_PARAMETERS)
    {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }
    if(rc)
    {
        BDBG_ERR(("%s SEC_ERROR", __FUNCTION__));
        return SEC_ERROR_BAD_USAGE;
    }
    if(!xSession->bKeySet) {
        xSession->bKeySet = true;
        /* notify application that key has been set */
        if (xSession->xTransportSessionId != TRANSPORT_SESSION_ID_INVALID)
            brcmCaNotifyCipherSessionKeyLoaded(xSession->xTransportSessionId);
    } 
    BDBG_MSG(("Leaving %s", __FUNCTION__));
    return SEC_NO_ERROR;
}



TSecStatus secSet1LevelProtectedKey(
    TSecCipherSession   xSession,
    TUnsignedInt16      xEmi,
    size_t              xKeyIdSize,
    TUnsignedInt8*     pxKeyId,
    size_t              xCipheredContentKeySize,
    const TUnsignedInt8*     pxCipheredContentKey,
    size_t              xCipheredProtectingKeySize,
    const TUnsignedInt8*     pxL1CipheredProtectingKey
) {
    TCsdDscKeyPathHandle* pDscKeyPath = 0;
    TCsdScrKeyPathHandle* pScrKeyPath = 0;	
    TCsdDscCipheredProtectingKeys ProtectingKeys;
    TCsdUnsignedInt8 *  pIv=NULL;
    size_t              IvSize=0;
    size_t              requiredIvSize=0;
    TCsdStatus rc;
    NEXUS_PlaypumpHandle playpumpHandle;
    const TCsdDscCipheredProtectingKeys CONSTANT_1 =
    {   {   0xA9, 0x32, 0x30, 0x31, 0x31, 0x4E, 0x61, 0x67,
            0x72, 0x61, 0x76, 0x69, 0x73, 0x69, 0x6f, 0x6e
        }
    };

    BDBG_MSG(("Enter %s xSession=%p xEmi=0x%x", __FUNCTION__, xSession, xEmi));

    if(!xCipheredContentKeySize || !pxCipheredContentKey || !xCipheredProtectingKeySize || !pxL1CipheredProtectingKey) {
        BDBG_ERR(("invalid key pointer")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if (!brcmEmiIsValid(xEmi)) {
        BDBG_ERR(("invalid xEmi")) ;
        return SEC_ERROR_BAD_EMI;
    }

    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("secSet1LevelProtectedKey xSession not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* only usefull for sream type sessions */
    if((xSession->type != SEC_SESSION_STREAM_DECRYPT_TYPE) && (xSession->type != SEC_SESSION_STREAM_ENCRYPT_TYPE)) {
        BDBG_ERR(("%s SEC_ERROR", __FUNCTION__));
        return SEC_ERROR_BAD_USAGE;
    }

    /* if this is Stream type pxKeyId should have value only if is a MPEG TS EMI */
    if(brcmEmiIsMpegTs(xEmi)){
        if(!xKeyIdSize || !pxKeyId) {
            BDBG_ERR(("xKeyIdSize NOT passed for MPEG TS EMI Stream session")) ;
            return SEC_ERROR_BAD_PARAMETER;
        }
    }
         /* Note: The NOCS3 test harness always passes pxKeyId regardless */
    /* of EMI, so we just ignore this case. */
#ifndef RPC_TEST_HARNESS_WORKAROUND
    else {
        if(xKeyIdSize || pxKeyId) {
            BDBG_ERR(("xKeyIdSize passed for Block mode Stream session")) ;
            return SEC_ERROR_BAD_PARAMETER;
        }
    }
#endif

    if(xCipheredProtectingKeySize != 16) {
        BDBG_ERR(("%s Fail - Invalid protecting key", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(xCipheredContentKeySize != brcmEmiGetKeySize(xEmi))
    {
        BDBG_ERR(("%s Invalid xCipheredContentKeySize", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    requiredIvSize = brcmEmiGetIvSize(xEmi);
    if (requiredIvSize > 0)
    {
        /* If its a zero IV EMI, use the zero buffer */
        if (brcmEmiIsZeroIv(xEmi))
        {
            IvSize = requiredIvSize;
            pIv = xZeroVector;
            /*printf("Setting EMI zero IV of %d bytes\n", IvSize);*/
        }
        else
        {
            /* Has someone (somehow) configured an IV? */
            if (xSession->xIvSize > 0)
            {
                if (xSession->xIvSize != requiredIvSize)
                {
                    BDBG_WRN(("%s Incorrect IV size set (%d vs. %d)", __FUNCTION__, xSession->xIvSize, requiredIvSize));
                }

                pIv = xSession->xIv;
                IvSize = xSession->xIvSize;
                /*printf("Setting session IV of %d bytes\n", IvSize);*/
            }
            /* Is the key id the right size? */
            else if ((pxKeyId != NULL) && (xKeyIdSize == requiredIvSize))
            {
                pIv = pxKeyId;
                IvSize = xKeyIdSize;
                /*printf("Setting keyid IV of %d bytes\n", IvSize);*/
            }
            /* No, default to zero IV */
            else
            {
                IvSize = requiredIvSize;
                pIv = xZeroVector;
                /*printf("Setting zero IV of %d bytes\n", IvSize);*/
            }
        }
    }

    BKNI_Memcpy(&ProtectingKeys[1], pxL1CipheredProtectingKey, 16);
    BKNI_Memcpy(&ProtectingKeys[0], CONSTANT_1, 16);

    if(SEC_NO_ERROR != brcmSecDetermineScrOrDsc_priv(xEmi, xSession)) {
        BDBG_ERR(("brcmSecDetermineScrOrDsc_priv failed"));
        return SEC_ERROR;
    }

    if(xSession->pDscKeyPath)
    {
        pDscKeyPath = xSession->pDscKeyPath;

        /* If the playpumpHandle has changed, notify CSD */
        if(brcmCaGetPlaypumpHandle(xSession->xTransportSessionId, &playpumpHandle)==SESSION_MANAGER_NO_ERROR) {
            if (playpumpHandle != xSession->playpumpHandle) {
                if (CSD_NO_ERROR != csdBrcmSetDscPlaypumpHandle(xSession->pDscKeyPath, playpumpHandle)) {
                    BDBG_ERR(("csdBrcmSetDscPlaypumpHandle failed"));
                    return SEC_ERROR;
                }
            }
        }

        pDscKeyPath->initializationVector = pIv;
        rc = csdSetProtectedDscContentKeys(xEmi, (const TCsdUnsignedInt8 (*)[16])ProtectingKeys,
                                       ((*pxKeyId)==1 ? (TCsdUnsignedInt8*)pxCipheredContentKey : 0), (*pxKeyId)==1 ? xCipheredContentKeySize : 0,
                                       ((*pxKeyId)==0 ? (TCsdUnsignedInt8*)pxCipheredContentKey : 0), (*pxKeyId)==0 ? xCipheredContentKeySize : 0,
                                       (TCsdDscKeyPathHandle*)pDscKeyPath);
    }
    else
    {
    	pScrKeyPath = xSession->pScrKeyPath;
		if( !xSession->bEncrypt)
        {
            rc = csdSetProtectedScrDecryptionKey(xEmi, 
                                (const TCsdUnsignedInt8 (*)[16])ProtectingKeys, 
                                (TCsdUnsignedInt8 *) pxCipheredContentKey, xCipheredContentKeySize,
                                (TCsdUnsignedInt8 *) pIv, IvSize, 
                                (TCsdScrKeyPathHandle*)pScrKeyPath);
        }
		else {
		 	rc = csdSetProtectedScrEncryptionKey(xEmi, 
                            (const TCsdUnsignedInt8 (*)[16])ProtectingKeys, 
                            (TCsdUnsignedInt8 *) pxCipheredContentKey, xCipheredContentKeySize,
                            (TCsdUnsignedInt8 *) pIv, IvSize, 
                            (TCsdScrKeyPathHandle*)pScrKeyPath);
        }
		 	
    }
	
    if(rc == CSD_ERROR_INVALID_PARAMETERS)
    {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(rc)
    {
        BDBG_ERR(("%s SEC_ERROR", __FUNCTION__));
        return SEC_ERROR_BAD_USAGE;
    }

    if(!xSession->bKeySet) {
        xSession->bKeySet = true;
        /* notify application that key has been set */
        BDBG_MSG(("%s calling brcmCaNotifyCipherSessionKeyLoaded", __FUNCTION__));
        if (xSession->xTransportSessionId != TRANSPORT_SESSION_ID_INVALID)
            brcmCaNotifyCipherSessionKeyLoaded(xSession->xTransportSessionId);
    } 

    BDBG_MSG(("Leaving %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

static TSecStatus secSet2LevelStreamProtectedKey(
    TSecCipherSession   xSession,
    TUnsignedInt16      xEmi,
    size_t              xKeyIdSize,
    TUnsignedInt8*     pxKeyId,
    size_t              xCipheredContentKeySize,
    const TUnsignedInt8*     pxCipheredContentKey,
    size_t              xCipheredProtectingKeySize,
    const TUnsignedInt8*     pxL1CipheredProtectingKey,
    const TUnsignedInt8*     pxL2CipheredProtectingKey
) {
    TCsdDscKeyPathHandle* pDscKeyPath = 0;
    TCsdScrKeyPathHandle* pScrKeyPath = 0;	
    TCsdDscCipheredProtectingKeys ProtectingKeys;
    TCsdUnsignedInt8 *  pIv=NULL;
    size_t              IvSize=0;
    size_t              requiredIvSize=0;
    TCsdStatus rc;
    NEXUS_PlaypumpHandle playpumpHandle;

    BDBG_MSG(("Enter %s xSession=%p xEmi=0x%x", __FUNCTION__, xSession, xEmi));

    if(!xCipheredContentKeySize ||
       !pxCipheredContentKey ||
       !xCipheredProtectingKeySize ||
       !pxL1CipheredProtectingKey ||
       !pxL2CipheredProtectingKey) {
        BDBG_ERR(("invalid key pointer")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if (!brcmEmiIsValid(xEmi)) {
        BDBG_ERR(("invalid xEmi")) ;
        return SEC_ERROR_BAD_EMI;
    }

    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("secSet2LevelStreamProtectedKey xSession not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* only usefull for sream type sessions */
    if((xSession->type != SEC_SESSION_STREAM_DECRYPT_TYPE) && (xSession->type != SEC_SESSION_STREAM_ENCRYPT_TYPE)) {
        BDBG_ERR(("%s SEC_ERROR", __FUNCTION__));
        return SEC_ERROR_BAD_USAGE;
    }

    /* if this is Stream type pxKeyId should have value only if is a MPEG TS EMI */
    if(brcmEmiIsMpegTs(xEmi)){
        if(!xKeyIdSize || !pxKeyId) {
            BDBG_ERR(("xKeyIdSize NOT passed for MPEG TS EMI Stream session")) ;
            return SEC_ERROR_BAD_PARAMETER;
        }
    }
    /* Note: The NOCS3 test harness always passes pxKeyId regardless */
    /* of EMI, so we just ignore this case. */
#ifndef RPC_TEST_HARNESS_WORKAROUND
    else {
        if(xKeyIdSize || pxKeyId) {
            BDBG_ERR(("xKeyIdSize passed for Block mode Stream session")) ;
            return SEC_ERROR_BAD_PARAMETER;
        }
    }
#endif

    if(xCipheredProtectingKeySize != 16) {
        BDBG_ERR(("%s Fail - Invalid protecting key", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(xCipheredContentKeySize != brcmEmiGetKeySize(xEmi))
    {
        BDBG_ERR(("%s Invalid xCipheredContentKeySize", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    requiredIvSize = brcmEmiGetIvSize(xEmi);
    if (requiredIvSize > 0)
    {
        /* If its a zero IV EMI, use the zero buffer */
        if (brcmEmiIsZeroIv(xEmi))
        {
            IvSize = requiredIvSize;
            pIv = xZeroVector;
            /*printf("Setting EMI zero IV of %d bytes\n", IvSize);*/
        }
        else
        {
            /* Has someone (somehow) configured an IV? */
            if (xSession->xIvSize > 0)
            {
                if (xSession->xIvSize != requiredIvSize)
                {
                    BDBG_WRN(("%s Incorrect IV size set (%d vs. %d)", __FUNCTION__, xSession->xIvSize, requiredIvSize));
                }

                pIv = xSession->xIv;
                IvSize = xSession->xIvSize;
                /*printf("Setting session IV of %d bytes\n", IvSize);*/
            }
            /* Is the key id the right size? */
            else if ((pxKeyId != NULL) && (xKeyIdSize == requiredIvSize))
            {
                pIv = pxKeyId;
                IvSize = xKeyIdSize;
                /*printf("Setting keyid IV of %d bytes\n", IvSize);*/
            }
            /* No, default to zero IV */
            else
            {
                IvSize = requiredIvSize;
                pIv = xZeroVector;
                /*printf("Setting zero IV of %d bytes\n", IvSize);*/
            }
        }
    }

    BKNI_Memcpy(&ProtectingKeys[1], pxL1CipheredProtectingKey, 16);
    BKNI_Memcpy(&ProtectingKeys[0], pxL2CipheredProtectingKey, 16);

    if(SEC_NO_ERROR != brcmSecDetermineScrOrDsc_priv(xEmi, xSession)) {
        BDBG_ERR(("brcmSecDetermineScrOrDsc_priv failed"));
        return SEC_ERROR;
    }

    if(xSession->pDscKeyPath)
    {
        pDscKeyPath = xSession->pDscKeyPath;

        /* If the playpumpHandle has changed, notify CSD */
        if(brcmCaGetPlaypumpHandle(xSession->xTransportSessionId, &playpumpHandle)==SESSION_MANAGER_NO_ERROR) {
            if (playpumpHandle != xSession->playpumpHandle) {
                if (CSD_NO_ERROR != csdBrcmSetDscPlaypumpHandle(xSession->pDscKeyPath, playpumpHandle)) {
                    BDBG_ERR(("csdBrcmSetDscPlaypumpHandle failed"));
                    return SEC_ERROR;
                }
            }
        }

        pDscKeyPath->initializationVector = pIv;
        rc = csdSetProtectedDscContentKeys(xEmi, (const TCsdUnsignedInt8 (*)[16])ProtectingKeys,
                                       ((*pxKeyId)==1 ? (TCsdUnsignedInt8*)pxCipheredContentKey : 0), (*pxKeyId)==1 ? xCipheredContentKeySize : 0,
                                       ((*pxKeyId)==0 ? (TCsdUnsignedInt8*)pxCipheredContentKey : 0), (*pxKeyId)==0 ? xCipheredContentKeySize : 0,
                                       (TCsdDscKeyPathHandle*)pDscKeyPath);
    }
    else
    {
    	pScrKeyPath = xSession->pScrKeyPath;
		if( !xSession->bEncrypt)
        {
            rc = csdSetProtectedScrDecryptionKey(xEmi, 
                                (const TCsdUnsignedInt8 (*)[16])ProtectingKeys, 
                                (TCsdUnsignedInt8 *) pxCipheredContentKey, xCipheredContentKeySize,
                                (TCsdUnsignedInt8 *) pIv, IvSize, 
                                (TCsdScrKeyPathHandle*)pScrKeyPath);
        }
		else {
		 	rc = csdSetProtectedScrEncryptionKey(xEmi, 
                            (const TCsdUnsignedInt8 (*)[16])ProtectingKeys, 
                            (TCsdUnsignedInt8 *) pxCipheredContentKey, xCipheredContentKeySize,
                            (TCsdUnsignedInt8 *) pIv, IvSize, 
                            (TCsdScrKeyPathHandle*)pScrKeyPath);
        }
		 	
    }
	
    if(rc == CSD_ERROR_INVALID_PARAMETERS)
    {
        BDBG_ERR(("%s SEC_ERROR_BAD_PARAMETER", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(rc)
    {
        BDBG_ERR(("%s SEC_ERROR", __FUNCTION__));
        return SEC_ERROR_BAD_USAGE;
    }

    if(!xSession->bKeySet) {
        xSession->bKeySet = true;
        /* notify application that key has been set */
        BDBG_MSG(("%s calling brcmCaNotifyCipherSessionKeyLoaded", __FUNCTION__));
        if (xSession->xTransportSessionId != TRANSPORT_SESSION_ID_INVALID)
            brcmCaNotifyCipherSessionKeyLoaded(xSession->xTransportSessionId);
    } 

    BDBG_MSG(("Leaving %s", __FUNCTION__));
    return SEC_NO_ERROR;
}



TSecStatus secSetMetadata(
    TSecCipherSession   xSession,
    size_t              xMetadataSize,
    const TUnsignedInt8*     pxMetadata
) {
    TUnsignedInt8 *pxOutputData =0;
    TUnsignedInt8 outputDataLength =0;
    TUnsignedInt16 xPidList[MAX_NUMBER_OF_PIDS_PER_XSESSION];
    TUnsignedInt8  numberOfPids=0;
    int i=0;
    TCsdStatus status = CSD_NO_ERROR;
    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("secSetMetadata xSession not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(xMetadataSize && !pxMetadata) {
        BDBG_ERR(("%s invalid pxMetadata ", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    if(!xMetadataSize && pxMetadata) {
        BDBG_ERR(("%s invalid xMetadataSize ", __FUNCTION__));
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* as we are curently only handling PID descriptors this only applies to DSC type cypher sessions */
    /* only usefull for sream type sessions */
    if((xSession->type != SEC_SESSION_STREAM_DECRYPT_TYPE) && (xSession->type != SEC_SESSION_STREAM_ENCRYPT_TYPE)) {
        BDBG_MSG(("%s ignoring metadats for non stream xsession", __FUNCTION__));
        return SEC_NO_ERROR;
    }

    /* Is this even a DSC session? */
    if (!xSession->pDscKeyPath)
        return SESSION_MANAGER_NO_ERROR;

    /* if !Metadatasize and !MetaData Buffer then the meta data is being removed */
    if(!xMetadataSize && !pxMetadata) {
        BDBG_MSG(("%s removing metadata",__FUNCTION__));
        /* remove metadata */
        if(xSession->pxMetadata) {
            BDBG_MSG(("%s Removing old metadata",__FUNCTION__));
            /* we had some data set previousely so remove any pids */
            brcmGetMetaDataDescriptor_priv (xSession->pxMetadata, xSession->xMetadataSize,
                                        ES_PID_LIST_METADATA_TAG, &pxOutputData,
                                        &outputDataLength);
            if(outputDataLength) {
                /* there were pid descriptors in the metadata*/
                brcmProcessPIDMetaDataDescriptor_priv (pxOutputData, outputDataLength,
                                            xPidList,
                                            &numberOfPids);
                if(numberOfPids) {
                    BDBG_MSG(("%s removing %d pids",__FUNCTION__, numberOfPids));
                    /* there were PIDS in the descritor*/
                    /* remove PIDS from xsession */
                    for(i=0;i<numberOfPids;i++) {
                        status = csdBrcmRemoveDscPid((TCsdDscKeyPathHandle*)xSession->pDscKeyPath, *(xPidList+i));
                        if(status !=CSD_NO_ERROR) {
                            BDBG_ERR(("%s csdBrcmRemoveDscPid failed for Pid 0x%x", __FUNCTION__, *(xPidList+i)));
                        }
                        BDBG_MSG(("%s removed PID 0x%x",__FUNCTION__, *(xPidList+i)));
                    }
                }
            }

            /* free the metadata buffer */
            BKNI_Free(xSession->pxMetadata);
            xSession->pxMetadata = 0;
            xSession->xMetadataSize = 0;
        }
    } else {

    	TUnsignedInt16 xOldPidList[MAX_NUMBER_OF_PIDS_PER_XSESSION];
	TUnsignedInt8  numberOfOldPids=0;
	int j=0;
		
        BDBG_MSG(("%s Adding new metadata",__FUNCTION__));
        if(xSession->pxMetadata) {
            BDBG_MSG(("%s Removing old metadata",__FUNCTION__));
            /* already some metadata so remove any old pids and add new ones */
            brcmGetMetaDataDescriptor_priv (xSession->pxMetadata, xSession->xMetadataSize,
                                        ES_PID_LIST_METADATA_TAG, &pxOutputData,
                                        &outputDataLength);
            if(outputDataLength) {
                /* there were pid descriptors in the metadata*/
#if 0 /* HUMAX_PLATFORM_BASE */
                brcmProcessPIDMetaDataDescriptor_priv (pxOutputData, outputDataLength,
                                            xPidList,
                                            &numberOfPids);
                if(numberOfPids) {
                    BDBG_MSG(("%s removing %d pids",__FUNCTION__, numberOfPids));
                    /* there were PIDS in the descritor*/
                    /* remove PIDS from xsession */
                    for(i=0;i<numberOfPids;i++) {
                        status = csdBrcmRemoveDscPid((TCsdDscKeyPathHandle*)xSession->pDscKeyPath, *(xPidList+i));
                        if(status !=CSD_NO_ERROR) {
                            BDBG_ERR(("%s csdBrcmRemoveDscPid failed for Pid 0x%x", __FUNCTION__, *(xPidList+i)));
                        }
                        BDBG_MSG(("%s removed PID 0x%x",__FUNCTION__, *(xPidList+i)));
                    }
                }
#else
                brcmProcessPIDMetaDataDescriptor_priv (pxOutputData, outputDataLength,
                                            xOldPidList,
                                            &numberOfOldPids);
#endif

            }
            /* free the metadata buffer */
            BKNI_Free(xSession->pxMetadata);
            xSession->pxMetadata = 0;
            xSession->xMetadataSize = 0;
        }
        xSession->pxMetadata=BKNI_Malloc(xMetadataSize);
        if(!xSession->pxMetadata) {
            BDBG_ERR(("%s memory error ", __FUNCTION__));
            return SEC_ERROR;
        }

        BKNI_Memcpy(xSession->pxMetadata, pxMetadata, xMetadataSize);
        xSession->xMetadataSize = xMetadataSize;
        /* process the new metadata*/

        brcmGetMetaDataDescriptor_priv (xSession->pxMetadata, xSession->xMetadataSize,
                                    ES_PID_LIST_METADATA_TAG, &pxOutputData,
                                    &outputDataLength);
        if(outputDataLength) {
            /* there were pid descriptors in the metadata*/
            brcmProcessPIDMetaDataDescriptor_priv (pxOutputData, outputDataLength,
                                        xPidList,
                                        &numberOfPids);
#if 0 /* HUMAX_PLATFORM_BASE */
            if(numberOfPids) {
                BDBG_MSG(("%s adding %d pids",__FUNCTION__, numberOfPids));
                /* there were PIDS in the descritor*/
                /* remove PIDS from xsession */
                for(i=0;i<numberOfPids;i++) {
                    status = csdBrcmSetDscPid((TCsdDscKeyPathHandle*)xSession->pDscKeyPath, *(xPidList+i));
                    if(status !=CSD_NO_ERROR) {
                        BDBG_ERR(("%s csdBrcmSetDscPid failed for Pid 0x%x", __FUNCTION__, *(xPidList+i)));
                    }
                    BDBG_MSG(("%s added PID 0x%x",__FUNCTION__, *(xPidList+i)));
                }
            }
#endif
        }

#if 1   /* HUMAX_PLATFORM_BASE */
        for(i=0;i<numberOfOldPids;i++) {
            for(j=0;j<numberOfPids;j++) {
                if(*(xOldPidList+i) == *(xPidList+j))
                    break;
            }

            if(j==numberOfPids) {
                status = csdBrcmRemoveDscPid((TCsdDscKeyPathHandle*)xSession->pDscKeyPath, *(xOldPidList+i));
                if(status !=CSD_NO_ERROR) {
                    BDBG_ERR(("%s csdBrcmRemoveDscPid failed for Pid 0x%x", __FUNCTION__, *(xOldPidList+i)));
                }
                BDBG_MSG(("%s removed PID 0x%x",__FUNCTION__, *(xOldPidList+i)));
            }
        }

        for(i=0;i<numberOfPids;i++) {
            for(j=0;j<numberOfOldPids;j++) {
                if(*(xPidList+i) == *(xOldPidList+j))
                    break;
            }

            if(j==numberOfOldPids) {
                status = csdBrcmSetDscPid((TCsdDscKeyPathHandle*)xSession->pDscKeyPath, *(xPidList+i));
                if(status !=CSD_NO_ERROR) {
                    BDBG_ERR(("%s csdBrcmSetDscPid failed for Pid 0x%x", __FUNCTION__, *(xPidList+i)));
                }
                BDBG_MSG(("%s added PID 0x%x",__FUNCTION__, *(xPidList+i)));
            }
        }
#endif
    }

    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}

TSecStatus secUseLegacyKey(
    TSecCipherSession   xSession
) {
    BDBG_MSG(("Enter %s", __FUNCTION__));

    if(!brcmValidateSecCipherSession_priv(xSession)) {
        BDBG_ERR(("xSession not valid")) ;
        return SEC_ERROR_BAD_PARAMETER;
    }

    /* nothing to do as next key that is set on this xSession will be treated according to EMI*/

    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}


TUnsignedInt8* secAllocateBuffer(
    size_t  xBufferSize
) {
    NEXUS_Error rc;
    TUnsignedInt8* pBuffer;

    BDBG_MSG(("Enter: %s", __FUNCTION__)) ;
    if(!xBufferSize) {
        BDBG_ERR(("%s invalid buffer size", __FUNCTION__)) ;
        return 0;
    }
    rc = NEXUS_Memory_Allocate(sizeof(TUnsignedInt8) * xBufferSize, NULL, (void *)&pBuffer);
    if (rc) {
        rc = BERR_TRACE(rc); 
        BDBG_ERR(("%s failed to allocate %d bytes", __FUNCTION__,xBufferSize)) ;
        return 0;
    }
    BDBG_MSG(("%s allocated %d bytes", __FUNCTION__,xBufferSize)) ;
    return pBuffer;
}

TSecStatus secFreeBuffer(
    TUnsignedInt8*     pxBuffer
) {
    BDBG_MSG(("Enter: %s", __FUNCTION__)) ;
    if(!pxBuffer) {
        BDBG_ERR(("%s invalid buffer but API expected to return success", __FUNCTION__)) ;
        return SEC_NO_ERROR;
    }
    NEXUS_Memory_Free(pxBuffer);
    BDBG_MSG(("Leave %s", __FUNCTION__));
    return SEC_NO_ERROR;
}



/****************************************************************************************************/
/***************************************End 5.1.1 API additions *****************************************/
/****************************************************************************************************/


#if 0 /* HUMAX_PLATFORM_BASE */ /* Call from Application, current not needed */
TSecFunctionTable* secGetFunctionTable(void)
{
    BKNI_Memset(&gSecFunctionTable, 0, sizeof(TSecFunctionTable));
    gSecFunctionTable.secApiVersion = SECAPI_VERSION_INT;
    gSecFunctionTable.secGetNuid64 = secGetNuid64;
    gSecFunctionTable.secGetNuid = secGetNuid;
    gSecFunctionTable.secGetChipsetRevision = secGetChipsetRevision;
    gSecFunctionTable.secGetChipsetExtension = secGetChipsetExtension;
    gSecFunctionTable.secOpenRam2RamDecryptSession = secOpenRam2RamDecryptSession;
    gSecFunctionTable.secOpenRam2RamEncryptSession = secOpenRam2RamEncryptSession;
    gSecFunctionTable.secCloseSession				= secCloseSession;
    gSecFunctionTable.secSet2LevelProtectedKey		= secSet2LevelProtectedKey;
    gSecFunctionTable.secSetClearTextKey			= secSetClearTextKey;
    gSecFunctionTable.secEncryptData 				= secEncryptData;
    gSecFunctionTable.secDecryptData				= secDecryptData;
    gSecFunctionTable.secSessionDecrypt 			= secSessionDecrypt;
    gSecFunctionTable.secSessionEncrypt			= secSessionEncrypt;
#ifndef NO_C99
    gSecFunctionTable.secRsaGenerateKey = secRsaGenerateKey;
    gSecFunctionTable.secRsaComputeCrtParams = secRsaComputeCrtParams;
    gSecFunctionTable.secRsaPrivateDecrypt = secRsaPrivateDecrypt;
    gSecFunctionTable.secRsaPrivateEncrypt = secRsaPrivateEncrypt;
    gSecFunctionTable.secRsaPublicDecrypt = secRsaPublicDecrypt;
    gSecFunctionTable.secRsaPublicEncrypt = secRsaPublicEncrypt;
    gSecFunctionTable.secDhGenerateKey = secDhGenerateKey;
    gSecFunctionTable.secDhComputeKey = secDhComputeKey;
    gSecFunctionTable.secEcdhGenerateKey = secEcdhGenerateKey;
    gSecFunctionTable.secEcdhComputeKey  = secEcdhComputeKey;
    gSecFunctionTable.secEcdsaGenerateKey = secEcdsaGenerateKey;
    gSecFunctionTable.secEcdsaSign = secEcdsaSign;
    gSecFunctionTable.secEcdsaVerify = secEcdsaVerify;
    gSecFunctionTable.secSha384Init		= secSha384Init;
    gSecFunctionTable.secSha384Update    = secSha384Update;
    gSecFunctionTable.secSha384Final 	= secSha384Final;
#else
    gSecFunctionTable.secRsaGenerateKey = NULL;
    gSecFunctionTable.secRsaComputeCrtParams = NULL;
    gSecFunctionTable.secRsaPrivateDecrypt = NULL;
    gSecFunctionTable.secRsaPrivateEncrypt = NULL;
    gSecFunctionTable.secRsaPublicDecrypt = NULL;
    gSecFunctionTable.secRsaPublicEncrypt = NULL;
    gSecFunctionTable.secDhGenerateKey = NULL;
    gSecFunctionTable.secDhComputeKey = NULL;
    gSecFunctionTable.secEcdhGenerateKey = NULL;
    gSecFunctionTable.secEcdhComputeKey  = NULL;
    gSecFunctionTable.secEcdsaGenerateKey = NULL;
    gSecFunctionTable.secEcdsaSign = NULL;
    gSecFunctionTable.secEcdsaVerify = NULL;
    gSecFunctionTable.secSha384Init		= NULL;
    gSecFunctionTable.secSha384Update    = NULL;
    gSecFunctionTable.secSha384Final 	= NULL;
#endif
    gSecFunctionTable.secGenerateRandomBytes = secGenerateRandomBytes;
    gSecFunctionTable.secSha1Init		= secSha1Init;
    gSecFunctionTable.secSha1Update    = secSha1Update;
    gSecFunctionTable.secSha1Final 	= secSha1Final;
    gSecFunctionTable.secSha256Init		= secSha256Init;
    gSecFunctionTable.secSha256Update    = secSha256Update;
    gSecFunctionTable.secSha256Final 	= secSha256Final;
    gSecFunctionTable.secHmacSha256Final = secHmacSha256Final;
    gSecFunctionTable.secHmacSha256Init = secHmacSha256Init;
    gSecFunctionTable.secHmacSha256Update = secHmacSha256Update;
    gSecFunctionTable.secUseCertKey = secUseCertKey;
    gSecFunctionTable.secUseFlashProtKey = secUseFlashProtKey;
    gSecFunctionTable.secEncryptFlashProtKey = secEncryptFlashProtKey;
    /****************************************************************************************************/
    /***************************************Start 5.1.1 API additions *****************************************/
    /****************************************************************************************************/
    gSecFunctionTable.secOpenStreamEncryptSession = secOpenStreamEncryptSession;
    gSecFunctionTable.secOpenStreamDecryptSession = secOpenStreamDecryptSession;
    gSecFunctionTable.secSet0LevelProtectedKey = secSet0LevelProtectedKey;
    gSecFunctionTable.secSet1LevelProtectedKey = secSet1LevelProtectedKey;
    gSecFunctionTable.secSetMetadata = secSetMetadata;
    gSecFunctionTable.secAllocateBuffer = secAllocateBuffer;
    gSecFunctionTable.secFreeBuffer = secFreeBuffer;
    gSecFunctionTable.secUseLegacyKey = secUseLegacyKey;
    /****************************************************************************************************/
    /***************************************End 5.1.1 API additions *****************************************/
    /****************************************************************************************************/
    return &gSecFunctionTable;
}
#endif


