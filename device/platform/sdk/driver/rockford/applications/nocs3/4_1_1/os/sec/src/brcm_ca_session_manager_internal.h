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
*   API name: "brcm_ca_session_manager_internal.h"
*    Internal interface to session manager. The functions in this header are called ONLY by the
*    DAL to synchronize session contextext information which are shared between DAL/App.
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

#ifndef BRCM_CA_SESSION_MANAGER_INTERNAL_H
#define BRCM_CA_SESSION_MANAGER_INTERNAL_H

#include "brcm_ca_session_manager_defs.h"


/**
 *  @brief
 *    Function type used for the notification of PID changes
 *    related the associated TSID. Any callbacks registerd in
 *    brcmCaRegisterTSIdPidNotificationCallback will be called
 *    when a PID associated with a TSID is changed. 
 *  @param[in]  xTSid
 *                Transport Session ID
 *  @param[in]  xPid
 *                CA descramble PID
 *  @param[in]  xState
 *                 Provides details on if xPid has been added or
 *                 removed from the PIDS associated with xTSid
 *                
*/
typedef TSessionManagerStatus (*BrcmCaPidChangedCallback)
(
    TTransportSessionId   xTSid,
    TUnsignedInt16 xPid,
    TPidState xState
);

/**
 *  @brief
 *    Function type used for the notification of PID changes
 *    related the associated TSID. Any callbacks registerd in
 *    brcmCaRegisterTSIdPidNotificationCallback will be called
 *    when a PID associated with a TSID is changed. 
 *  @param[in]  xTSid
 *                Transport Session ID
 *  @param[in]  pxIv
 *                Pointer to a buffer containing the new IV
 *  @param[in]  The size of the IV
*/
typedef TSessionManagerStatus (*BrcmCaIvChangedCallback)
(
    TTransportSessionId xTSid,
    const TUnsignedInt8 *pxIv,
    size_t xIvSize
);


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
);


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
);


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
);


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
);


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
);


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
);

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
);

/* Internal function required by the RPC testharness */
TSessionManagerStatus brcmCaSetScrKeyPath
(
    TTransportSessionId          xTSid, 
    struct SCsdScrKeyPathHandle *pScrKeyPath
);

/* Internal function required by the RPC testharness */
TSessionManagerStatus brcmCaGetScrKeyPath
(
    TTransportSessionId           xTSid, 
    struct SCsdScrKeyPathHandle **ppScrKeyPath
);

/**
 *  @brief
 *
 *  @param[in]  xTSid
 *                Transport Session ID
 *  
 *  @param[out]  pxSessionType
 *                The session type
 *
 *  @retval  TSessionManagerStatus
 *
 *  @remarks
 *    -#
*/
TSessionManagerStatus brcmCaGetTransportSessionType
(
    TTransportSessionId        xTSid,
    TSessionManagerSessionType *pxSessionType
);

#endif
