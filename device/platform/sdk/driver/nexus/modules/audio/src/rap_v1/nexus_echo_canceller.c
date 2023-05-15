/***************************************************************************
 *     (c)2006-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_echo_canceller.c $
 * $brcm_Revision: 1 $
 * $brcm_Date: 7/30/12 4:36p $
 *
 * Module Description: Audio Decoder Interface
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/audio/src/rap_v1/nexus_echo_canceller.c $
 * 
 * 1   7/30/12 4:36p jgarrett
 * SW7435-266: Switching to unified audio module structure
 * 
 ***************************************************************************/

#include "nexus_types.h"
#include "nexus_audio_module.h"

BDBG_MODULE(nexus_echo_canceller);

BDBG_OBJECT_ID(NEXUS_EchoCanceller);

typedef struct NEXUS_EchoCanceller
{
    BDBG_OBJECT(NEXUS_EchoCanceller)
    void *pDummy;
} NEXUS_EchoCanceller;

void NEXUS_EchoCanceller_GetDefaultSettings(
    NEXUS_EchoCancellerSettings *pSettings   /* [out] */
    )
{
    BSTD_UNUSED(pSettings);
}

NEXUS_EchoCancellerHandle NEXUS_EchoCanceller_Create(
    const NEXUS_EchoCancellerSettings *pSettings
    )
{
    BSTD_UNUSED(pSettings);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

void NEXUS_EchoCanceller_Destroy(
    NEXUS_EchoCancellerHandle handle
    )
{
    BSTD_UNUSED(handle);
}

void NEXUS_EchoCanceller_GetSettings(
    NEXUS_EchoCancellerHandle handle,
    NEXUS_EchoCancellerSettings *pSettings    /* [out] Settings */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

NEXUS_Error NEXUS_EchoCanceller_SetSettings(
    NEXUS_EchoCancellerHandle handle,
    const NEXUS_EchoCancellerSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

void NEXUS_EchoCanceller_GetAlgorithmSettings(
    NEXUS_EchoCancellerHandle handle,
    NEXUS_EchoCancellerAlgorithm algorithm,
    NEXUS_EchoCancellerAlgorithmSettings *pSettings  /* [out] */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(algorithm);
    BSTD_UNUSED(pSettings);
}

NEXUS_Error NEXUS_EchoCanceller_SetAlgorithmSettings(
    NEXUS_EchoCancellerHandle handle,
    const NEXUS_EchoCancellerAlgorithmSettings *pSettings
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

NEXUS_AudioInput NEXUS_EchoCanceller_GetConnector(
    NEXUS_EchoCancellerHandle handle
    )
{
    BSTD_UNUSED(handle);
    return NULL;
}

NEXUS_Error NEXUS_EchoCanceller_AddLocalInput(
    NEXUS_EchoCancellerHandle handle,
    NEXUS_AudioInput input
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(input);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_EchoCanceller_RemoveLocalInput(
    NEXUS_EchoCancellerHandle handle,
    NEXUS_AudioInput input
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(input);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_EchoCanceller_AddRemoteInput(
    NEXUS_EchoCancellerHandle handle,
    NEXUS_AudioInput input
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(input);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_EchoCanceller_RemoveRemoteInput(
    NEXUS_EchoCancellerHandle handle,
    NEXUS_AudioInput input
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(input);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_EchoCanceller_RemoveAllInputs(
    NEXUS_EchoCancellerHandle handle
    )
{
    BSTD_UNUSED(handle);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}


