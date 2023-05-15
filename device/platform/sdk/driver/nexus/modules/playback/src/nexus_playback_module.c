/***************************************************************************
 *     (c)2007-2009 Broadcom Corporation
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
 * $brcm_Workfile: nexus_playback_module.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 12/17/09 5:52p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/playback/src/nexus_playback_module.c $
 * 
 * 2   12/17/09 5:52p vsilyaev
 * SW3548-2677: Added configuration that allows user to choose what
 *  context should be used for synchronous I/O
 * 
 * 1   1/18/08 2:36p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/3   11/7/07 5:34p vsilyaev
 * PR 36788: Updated file API
 * 
 * Nexus_Devel/2   10/15/07 5:12p vsilyaev
 * PR 35824: Added module initialization
 * 
 * Nexus_Devel/1   10/11/07 6:23p vsilyaev
 * PR 35824: Playback module
 * 
 * 
 **************************************************************************/
#include "nexus_playback_module.h"

NEXUS_ModuleHandle NEXUS_PlaybackModule;

void 
NEXUS_PlaybackModule_GetDefaultSettings(NEXUS_PlaybackModuleSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    return;
}

NEXUS_ModuleHandle 
NEXUS_PlaybackModule_Init(const NEXUS_PlaybackModuleSettings *pSettings)
{
    NEXUS_Error rc;
    NEXUS_ModuleSettings moduleSettings;

    if(pSettings==NULL) { 
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_settings;
    }
    if(pSettings->modules.file == NULL || pSettings->modules.videoDecoder == NULL || pSettings->modules.audioDecoder == NULL || pSettings->modules.playpump == NULL) {
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_settings;
    }
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLow;
    NEXUS_PlaybackModule = NEXUS_Module_Create("playback", &moduleSettings);
    if(!NEXUS_PlaybackModule) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_module; }

    NEXUS_UseModule(pSettings->modules.file);
    NEXUS_UseModule(pSettings->modules.videoDecoder);
    NEXUS_UseModule(pSettings->modules.audioDecoder);
    NEXUS_UseModule(pSettings->modules.playpump);

    return NEXUS_PlaybackModule;

err_module:
err_settings:
    return NULL;
}
    
void 
NEXUS_PlaybackModule_Uninit(void)
{
    NEXUS_Module_Destroy(NEXUS_PlaybackModule);
    NEXUS_PlaybackModule = NULL;
    return;
}


