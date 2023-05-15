/***************************************************************************
*     (c)2004-2010 Broadcom Corporation
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
* $brcm_Workfile: nexus_audio_input_priv.h $
* $brcm_Revision: 4 $
* $brcm_Date: 2/1/10 8:07p $
*
* API Description:
*   API name: AudioInput
*   Generic API for inputs to an audio mixer.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/audio/7400/include/priv/nexus_audio_input_priv.h $
* 
* 4   2/1/10 8:07p bandrews
* SW7405-3774: add mute control support to sync channel
* 
* 3   11/26/08 5:29p jgarrett
* PR49294: Added precision lipsync flag; ignore video input callback when
*  audio says it's in dsola mode
* 
* PR49294/1   11/26/08 5:10p bandrews
* PR49294: Added precision lipsync flag; ignore video input callback when
*  audio says it's in dsola mode
* 
* 2   10/30/08 11:07p erickson
* PR48453: merge
* 
* PR48453/1   10/30/08 10:31p bandrews
* PR48453: DSS support added
* 
* 1   1/31/08 9:49a erickson
* PR36808: added private SyncChannel interface
* 
***************************************************************************/
#ifndef NEXUS_AUDIO_INPUT_PRIV_H__
#define NEXUS_AUDIO_INPUT_PRIV_H__

#include "nexus_types.h"
#include "nexus_audio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NEXUS_AudioInputSyncSettings
{
    unsigned delay; /* Delay applied by sync channel to audio input.  This is always in 45 KHz units.  */
    bool mute;          /* Mute audio until SyncChannel is ready. */
    NEXUS_Callback sampleRateCallback_isr; /* Notify SyncChannel when the sample rate changes. */
    NEXUS_Callback startCallback_isr; /* Notify SyncChannel when audio starts or stops. */
    void *callbackContext; /* user context passed in isr callbacks */
} NEXUS_AudioInputSyncSettings;

void NEXUS_AudioInput_GetSyncSettings_priv(
    NEXUS_AudioInput audioInput,
    NEXUS_AudioInputSyncSettings *pSyncSettings /* [out] */
    );
    
NEXUS_Error NEXUS_AudioInput_SetSyncSettings_priv(
    NEXUS_AudioInput audioInput,
    const NEXUS_AudioInputSyncSettings *pSyncSettings
    );
    
typedef struct NEXUS_AudioInputSyncStatus 
{
    bool started;
    bool digital;
    bool dsolaEnabled;
    unsigned delay;
} NEXUS_AudioInputSyncStatus;
        
NEXUS_Error NEXUS_AudioInput_GetSyncStatus_isr(
    NEXUS_AudioInput audioInput,
    NEXUS_AudioInputSyncStatus *pSyncStatus /* [out] */
    );
    
#ifdef __cplusplus
}
#endif

#endif /* #ifndef NEXUS_AUDIO_INPUT_PRIV_H__ */

