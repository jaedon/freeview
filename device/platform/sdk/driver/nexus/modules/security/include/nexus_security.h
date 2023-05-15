/***************************************************************************
 *     (c)2007-2010 Broadcom Corporation
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
 * $brcm_Workfile: nexus_security.h $
 * $brcm_Revision: 13 $
 * $brcm_Date: 10/7/10 11:04a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/security/7400/include/nexus_security.h $
 * 
 * 13   10/7/10 11:04a erickson
 * SW7420-1148: add destructor hints
 *
 * 12   5/10/10 10:47a erickson
 * SW7550-415: add NEXUS_Security_GetDefaultClearKey. remove redundant
 *  comments.
 *
 * 11   3/16/10 9:46a erickson
 * SW7400-2678: add NEXUS_Security_InvalidateKey
 *
 * 10   1/7/09 2:30p jgarrett
 * PR 50093: Adding Multi2
 *
 * 9   9/3/08 3:29p erickson
 * PR45612: update
 *
 * 8   6/23/08 6:15p mphillip
 * PR40027: Tidy up naming conventions to match with Nexus
 *
 * 7   6/17/08 6:16p mphillip
 * PR40027: Retrieve more information from KeySlotHandle
 *
 * 6   6/17/08 11:14a mphillip
 * PR40027: M2M function headers
 *
 * 5   6/17/08 12:31a mphillip
 * PR40027: Keyslot changes for shim layer
 *
 * 4   5/29/08 10:57a mphillip
 * PR38369: Fix consts to allow kernel builds
 *
 * 3   5/28/08 2:32p mphillip
 * PR38369: Refactor keyslot configuration to Security module from Crypto
 *  module
 *
 * 2   2/29/08 11:04a erickson
 * PR37137: create stub to remove sync thunk warning
 *
 * 1   1/18/08 2:21p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/1   11/15/07 3:54p erickson
 * PR37137: added Security module
 *
 **************************************************************************/
#ifndef NEXUS_SECURITY_H__
#define NEXUS_SECURITY_H__

#include "nexus_security_datatypes.h"

/*=***********************************
*************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/**
Summary:
This function retrieves keyslot information from a keyslot handle.

Description:
This function shall retrieve keyslot information from a keyslot handle.

See Also:
NEXUS_KeySlotInfo
**/
void NEXUS_Security_GetKeySlotInfo(
    NEXUS_KeySlotHandle keyHandle,
    NEXUS_SecurityKeySlotInfo * pKeyslotInfo /* [out] */
    );

/**
Summary:
Get default NEXUS_SecurityClearKey
**/
void NEXUS_Security_GetDefaultClearKey(
    NEXUS_SecurityClearKey *pClearKey /* [out] */
    );

/**
Summary:
This function loads a clear key to a keyslot.

Description:
This function shall load clear key into the keyslot.

Performance and Timing:
This is a synchronous function that will return when it is done.
**/
NEXUS_Error NEXUS_Security_LoadClearKey(
    NEXUS_KeySlotHandle keyHandle,
    const NEXUS_SecurityClearKey *pClearKey
    );


/**
Summary:
This function retrieves default keyslot settings.

See Also:
NEXUS_SecurityKeySlotSettings
**/
void NEXUS_Security_GetDefaultKeySlotSettings(
    NEXUS_SecurityKeySlotSettings *pSettings /* [out] */
    );

/**
Summary:
This function creates a keyslot handle.

Description:
This function shall create a keyslot handle based on the provided information.

Performance and Timing:
This is a synchronous function that will return when it is done.

Returns:
NEXUS_KeySlotHandle on success, NULL on failure.

See Also:
NEXUS_Security_GetDefaultKeySlotSettings
NEXUS_Security_FreeKeySlot

**/
NEXUS_KeySlotHandle NEXUS_Security_AllocateKeySlot( /* attr{destructor=NEXUS_Security_FreeKeySlot} */
    const NEXUS_SecurityKeySlotSettings *pSettings
    );

/**
Summary:
**/
NEXUS_KeySlotHandle NEXUS_Security_LocateCaKeySlotAssigned( /* attr{destructor=NEXUS_Security_FreeKeySlot} */
    unsigned long pidchannel
    );


/*****************************************************************************
Summary:
This function frees a keyslot handle.

Description:
See Also:
NEXUS_Security_AllocateKeySlot
**/
void NEXUS_Security_FreeKeySlot(
    NEXUS_KeySlotHandle keyHandle
    );


/**
Summary:
This function adds a PID channel to the CA or CACP keyslot.

Description:
This function shall add a PID channel to the CA or CACP keyslot. The newly
added PID channel will use the alogirthm and key value of the CA or CACP
keyslot for descrambling or descrambling followed by CPS.

See Also:
NEXUS_Security_RemovePidChannelFromKeySlot
**/
NEXUS_Error NEXUS_Security_AddPidChannelToKeySlot(
    NEXUS_KeySlotHandle keyHandle,
    unsigned int pidChannel
    );


/**
Summary:
This function removes a PID channel from the CA or CACP keyslot.

Description:
This function shall remove a PID channel from the CA or CACP keyslot. The
removed PID channel will NOT use the algorithm and key value of the CA or CACP
keyslot anymore.

See Also:
NEXUS_Security_AddPidChannelToKeySlot

**/
NEXUS_Error NEXUS_Security_RemovePidChannelFromKeySlot(
    NEXUS_KeySlotHandle keyHandle,
    unsigned int pidChannel
    );

/**
Summary:
This function retrieves default settings for CACP key invalidation.

Description:
This function shall retrieve default settings for CACP key invalidation.

See Also:
NEXUS_Security_InvalidateKey
**/
void NEXUS_Security_GetDefaultInvalidateKeySettings(
    NEXUS_SecurityInvalidateKeySettings *pSettings /* [out] */
    );


/**
Summary:
This function invalidates the current settings for the key slot specified by keyHandle

Description:
See Also:
NEXUS_SecurityInvalidateKeySettings
**/
NEXUS_Error NEXUS_Security_InvalidateKey(
    NEXUS_KeySlotHandle keyHandle,
    const NEXUS_SecurityInvalidateKeySettings *pSettings
    );


/**
Summary:
This function retrieves default algorithm settings.
**/
void NEXUS_Security_GetDefaultAlgorithmSettings(
    NEXUS_SecurityAlgorithmSettings *pSettings /* [out] */
    );

/**
Summary:
This function configures algorithm on a keyslot.

Description:
Performance and Timing:
This is a synchronous function that will return when it is done.

See Also:
NEXUS_Security_AllocateKeySlot
NEXUS_Security_GetDefaultAlgorithmSettings
**/
NEXUS_Error NEXUS_Security_ConfigAlgorithm(
    NEXUS_KeySlotHandle keyHandle,
    const NEXUS_SecurityAlgorithmSettings *pSettings
    );

/**
Summary:
This function retrieves default multi2 settings.
**/
void NEXUS_Security_GetDefaultMulti2Settings(
    NEXUS_SecurityMulti2Settings *pSettings /* [out] */
    );

/**
Summary:
This function configures Multi2 on a keyslot.

Description:
This function shall configure multi2 on a keyslot.

Performance and Timing:
This is a synchronous function that will return when it is done.

See Also:
NEXUS_Security_AllocateKeySlot
NEXUS_Security_GetDefaultAlgorithmSettings
**/
NEXUS_Error NEXUS_Security_ConfigMulti2(
    NEXUS_KeySlotHandle keyHandle,
    const NEXUS_SecurityMulti2Settings *pSettings
    );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
