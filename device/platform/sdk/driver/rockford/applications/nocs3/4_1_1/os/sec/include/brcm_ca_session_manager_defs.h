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
*   API name: "brcm_ca_session_manager_defs.h"
*    This header defines types shared by both the internal and external session manager interface.
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

#ifndef BRCM_CA_SESSION_MANAGER_DEFS_H
#define BRCM_CA_SESSION_MANAGER_DEFS_H

#include "ca_defs.h"
#include "ca_defsx.h"
#include "nexus_playpump.h"



/******************************************************************************/
/*                                                                            */
/*                           CONSTANTS DEFINITIONS                            */
/*                                                                            */
/******************************************************************************/

/**
 *  @brief
 *    MPEG PID of NULL packets.
 *
 *  @see TPid
*/
#define BRCM_NULL_PID  ((TUnsignedInt16) 0x1FFF)


/**
 *  @brief
 *    Invalid MPEG PID.
 *
 *  @see TPid
*/
#define BRCM_INVALID_PID ((TUnsignedInt16) 0xFFFF)


/**
 *  @brief
 *    Defines the maximum valid value for an MPEG PID. Mpeg Pids are stored on
 *    13 bits.
 *
 *  @see TPid
*/
#define BRCM_MAX_PID_VALUE ((TPid) 0x1FFF)


/******************************************************************************/
/*                                                                            */
/*                           SHARED TYPES                                     */
/*                                                                            */
/******************************************************************************/

/**
 *  @brief
 *    Status returned by Broadcom SESSION_MANAGER functions
 *
*/
typedef enum
{
  SESSION_MANAGER_NO_ERROR,
    /**<  Success
    */
  SESSION_MANAGER_ERROR_NO_MORE_RESOURCES,
    /**<  No more filter resources are available for the demultiplexer
    */
  SESSION_MANAGER_ERROR_BAD_PARAMETER,
    /**<  The given parameters are inconsistent
    */
  SESSION_MANAGER_ERROR
    /**<  Other error
    */
} TSessionManagerStatus;

/**
 *  @brief
 *    provides details on what operation has been performed on a
 *    PID
 *
*/
typedef enum
{
  SESSION_MANAGER_PID_REMOVED,
  SESSION_MANAGER_PID_ADDED,
  SESSION_MANAGER_PID_MAX
} TPidState;


typedef enum
{
    SESSION_TYPE_UNDEFINED = 0,
    SESSION_TYPE_DSC,
    SESSION_TYPE_SCR
} TSessionManagerSessionType;


#endif
