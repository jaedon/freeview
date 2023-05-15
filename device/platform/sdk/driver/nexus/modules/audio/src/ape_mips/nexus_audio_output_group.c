/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_output_group.c $
* $brcm_Revision: 1 $
* $brcm_Date: 5/2/12 5:04p $
*
* API Description:
*   API name: AudioOutput
*    API to group multiple stereo outputs into a multichannel output
*
* Revision History:
*
* $brcm_Log: /nexus/modules/soft_audio/7408/src/nexus_audio_output_group.c $
* 
* 1   5/2/12 5:04p jgarrett
* SW7425-2802: Adding 7408 stubs for missing 7400/7405 routines
* 
***************************************************************************/

#include "nexus_audio_module.h"

void NEXUS_AudioOutputGroup_GetDefaultCreateSettings(
    NEXUS_AudioOutputGroupCreateSettings *pSettings
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_AudioOutputGroupHandle NEXUS_AudioOutputGroup_Create(
    const NEXUS_AudioOutputGroupCreateSettings *pSettings
    )
{
    BSTD_UNUSED(pSettings);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

void NEXUS_AudioOutputGroup_Destroy(
    NEXUS_AudioOutputGroupHandle handle
    )
{
    BSTD_UNUSED(handle);
}

NEXUS_AudioOutput NEXUS_AudioOutputGroup_GetConnector(
    NEXUS_AudioOutputGroupHandle handle
    )
{
    BSTD_UNUSED(handle);
    (void)BERR_TRACE(BERR_NOT_SUPPORTED);
    return NULL;
}

