/***************************************************************************
*     (c)2004-2008 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_output_priv.h $
* $brcm_Revision: 2 $
* $brcm_Date: 3/11/08 3:08p $
*
* API Description:
*   API name: AudioOutput
*    Generic APIs for audio outputs from an audio mixer.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7400/include/priv/nexus_audio_output_priv.h $
* 
* 2   3/11/08 3:08p erickson
* PR40222: added independent audio output delay for both the user and
*  SyncChannel
* 
* 1   1/31/08 9:49a erickson
* PR36808: added private SyncChannel interface
*
***************************************************************************/
#ifndef NEXUS_AUDIO_OUTPUT_PRIV_H__
#define NEXUS_AUDIO_OUTPUT_PRIV_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_AudioOutputSyncSettings
{
	unsigned delay; /* SyncChannel independent audio output delay. this is added to NEXUS_AudioOutputSettings.additionalDelay. */
    NEXUS_Callback startCallback_isr;
    void *callbackContext;
} NEXUS_AudioOutputSyncSettings;

void NEXUS_AudioOutput_GetSyncSettings_priv(
    NEXUS_AudioOutput audioOutput,
    NEXUS_AudioOutputSyncSettings *pSyncSettings /* [out] */
    );
    
NEXUS_Error NEXUS_AudioOutput_SetSyncSettings_priv(
    NEXUS_AudioOutput audioOutput,
    const NEXUS_AudioOutputSyncSettings *pSyncSettings
    );

typedef struct NEXUS_AudioOutputSyncStatus 
{
    bool started;
    bool compressed;
} NEXUS_AudioOutputSyncStatus;
        
NEXUS_Error NEXUS_AudioOutput_GetSyncStatus_isr(
    NEXUS_AudioOutput audioOutput,
    NEXUS_AudioOutputSyncStatus *pSyncStatus /* [out] */
    );
    
#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUDIO_OUTPUT_PRIV_H__ */

