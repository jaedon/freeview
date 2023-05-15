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
 * $brcm_Workfile: nexus_record_module.c $
 * $brcm_Revision: 2 $
 * $brcm_Date: 4/21/09 3:32p $
 *
 * Module Description:
 *  PVR Record
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/record/src/nexus_record_module.c $
 * 
 * 2   4/21/09 3:32p erickson
 * PR45902: playback module is optional. only required for timeshifting.
 *  add NEXUS_RecordModule_SetPlaybackModule for dynamic changes. add
 *  runtime check.
 *
 * 1   1/18/08 2:36p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/2   11/9/07 6:03p vsilyaev
 * PR 36788: Added timeshifting
 *
 * Nexus_Devel/1   11/7/07 5:07p vsilyaev
 * PR 36788: PVR record module
 *
 **************************************************************************/
#include "nexus_record_module.h"

NEXUS_ModuleHandle NEXUS_RecordModule;
NEXUS_Record_P_ModuleState g_NEXUS_Record_P_ModuleState;


void
NEXUS_RecordModule_GetDefaultSettings(NEXUS_RecordModuleSettings *pSettings)
{
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    return;
}

NEXUS_ModuleHandle
NEXUS_RecordModule_Init(const NEXUS_RecordModuleSettings *pSettings)
{
    NEXUS_Error rc;

    if(!pSettings) {
        rc = BERR_TRACE(BERR_NOT_SUPPORTED);
        goto err_settings;
    }
    /* The playback module is not required. */
    if(pSettings->modules.file == NULL || pSettings->modules.recpump == NULL) {
        rc = BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto err_settings;
    }
    NEXUS_RecordModule = NEXUS_Module_Create("record", NULL);
    if(!NEXUS_RecordModule) {
        rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_module;
    }
    NEXUS_UseModule(pSettings->modules.file);
    NEXUS_UseModule(pSettings->modules.recpump);
    g_NEXUS_Record_P_ModuleState.cfg = *pSettings;

    return NEXUS_RecordModule;
err_module:
err_settings:
    return NULL;
}

void
NEXUS_RecordModule_Uninit(void)
{
    NEXUS_Module_Destroy(NEXUS_RecordModule);
    NEXUS_RecordModule = NULL;
    return;
}

void NEXUS_RecordModule_SetPlaybackModule( NEXUS_ModuleHandle playback )
{
    g_NEXUS_Record_P_ModuleState.cfg.modules.playback = playback;
}

