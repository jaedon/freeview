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
* $brcm_Workfile: nexus_audio_return_channel.c $
* $brcm_Revision: 1 $
* $brcm_Date: 7/30/12 4:36p $
*
* API Description:
*   API name: Audio Return Channel (ARC)
*    Specific APIs related to Audio Return Channel output.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/src/ape_mips/nexus_audio_return_channel.c $
* 
* 1   7/30/12 4:36p jgarrett
* SW7435-266: Switching to unified audio module structure
* 
***************************************************************************/
#include "nexus_audio_module.h"

BDBG_MODULE(nexus_audio_return_channel);

typedef struct NEXUS_AudioArc
{
    BDBG_OBJECT(NEXUS_AudioArc)
    void *pDummy;
} NEXUS_AudioArc;

void NEXUS_AudioReturnChannel_GetDefaultSettings(
    NEXUS_AudioReturnChannelSettings *pSettings   /* [out] default settings */
    )
{
    BSTD_UNUSED(pSettings);
}

NEXUS_AudioReturnChannelHandle NEXUS_AudioReturnChannel_Open(
    unsigned index,
    const NEXUS_AudioReturnChannelSettings *pSettings     /* Pass NULL for default settings */
    )
{
    BSTD_UNUSED(index);
    BSTD_UNUSED(pSettings);    
    return NULL;
}

void NEXUS_AudioReturnChannel_Close(
    NEXUS_AudioReturnChannelHandle handle
    )
{
    BSTD_UNUSED(handle);
}

void NEXUS_AudioReturnChannel_GetSettings(
    NEXUS_AudioReturnChannelHandle handle,
    NEXUS_AudioReturnChannelSettings *pSettings    /* [out] Settings */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
}

NEXUS_Error NEXUS_AudioReturnChannel_SetSettings(
    NEXUS_AudioReturnChannelHandle handle,
    const NEXUS_AudioReturnChannelSettings *pSettings    /* [in] Settings */
    )
{
    BSTD_UNUSED(handle);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(BERR_NOT_SUPPORTED);
}

NEXUS_AudioOutput NEXUS_AudioReturnChannel_GetConnector(
    NEXUS_AudioReturnChannelHandle handle
    )
{
    BSTD_UNUSED(handle);
    return NULL;
}

