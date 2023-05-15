/***************************************************************************
*     (c)2004-2013 Broadcom Corporation
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
* API Description:
*   API name: "brcm_ca_session_manager_external.h"
*    External interface to session manager. The functions in this header should be called by the
*    application to configure transport session contextexts which are shared between DAL/App.
* 
*    Broadcom DAL level API provided to allow the Application to associate CA descramble
*    PIDS with Nagra Transport Session ID's. This module must be called from the CAK porting
*    level and is necessary due to NAGRA SEC 5.1 changes which no longer provide PID information
*    to SEC CA descramble sessions (secOpenStreamDecryptSession). Typical usage would be for the CAK
*    porting layer to create a descramble session request CA_REQUEST_TYPE_PROGRAM_DESCRAMBLING
*    for a TSID then call this API to associate the service PIDS with the TSID associated 
*    with the CA_REQUEST_TYPE_PROGRAM_DESCRAMBLING request. The SEC decrypt session will then need
*    to call the get function within this module in order to find the PIDS associated with the
*    descramble sessions TSID. To make matters slightly more complex this module must also update the SEC
*    descramble session PIDS on the fly and deal with the race conditions associated with this
*    architecture: i.e 1. A StreamDecryptSession may be created and keys set before this module
*    has been notified of the PIDS 2. A PID associated with an existing StreamDecryptSession may change.
*    In order to deal with the above the module need both a getter and notification mechanism for
*    intrested clients.
*
* 
***************************************************************************/

#ifndef BRCM_CA_SESSION_MANAGER_EXTERNAL_H
#define BRCM_CA_SESSION_MANAGER_EXTERNAL_H

#include "brcm_ca_session_manager_defs.h"


/**
 *  @brief
 *    Function type used for the notification of M2M keyhandle
 *    to the application in the case that a Stream type crypto
 *    session is created on the sec that requires application to
 *    do DMA decrypt (such as block mode decrypt).
 *    Implementation MUST NOT BLOCK OR TAKE LOCK.
 *  @param[in]  xTSid
 *                Transport Session ID
 *  @param[in]  m2mKeyHandle
 *                m2mkeyhandle allocated for TSID Above
 *                
*/
typedef TSessionManagerStatus (*BrcmCaStreamSessionKeyHandleCallback)
(
    TTransportSessionId   xTSid,
	NEXUS_KeySlotHandle   m2mKeyHandle /* handle of keyslot */
);



/**
 *  @brief
 *    Function type used for the notification of keyload
 *    to the application in the case that a Stream type crypto
 *    session is created on the sec that requires application to
 *    do DMA decrypt (such as block mode decrypt). This function
 *    will be called when any key is set on a TSecCipherSession
 *    associated with xTSid. Implementation MUST NOT BLOCK OR
 *    TAKE LOCK.
 *  @param[in]  xTSid
 *                Transport Session ID
 *  @param[in]  m2mKeyHandle
 *                m2mkeyhandle allocated for TSID Above
 *                
*/
typedef TSessionManagerStatus (*BrcmCaCipherSessionKeyLoadedCallback)
(
    TTransportSessionId   xTSid,
	NEXUS_KeySlotHandle   m2mKeyHandle /* handle of keyslot */
);


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
);


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
);


/**
 *  @brief
 *    This function add a PID to TSID context
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
);


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
);

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
);



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
);

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
);


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
);

#endif
