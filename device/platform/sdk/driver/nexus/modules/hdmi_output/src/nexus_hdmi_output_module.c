/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_hdmi_output_module.c $
 * $brcm_Revision: 7 $
 * $brcm_Date: 7/16/12 1:01p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/hdmi_output/7400/src/nexus_hdmi_output_module.c $
 * 
 * 7   7/16/12 1:01p vsilyaev
 * SW7420-2324: Use atomic verify and acquire for handle verification
 * 
 * 6   6/24/09 3:12p mphillip
 * PR54926: Add /proc/hdmi_output
 * 
 * 5   1/26/09 1:54p erickson
 * PR51468: global variable naming convention
 *
 * 4   4/4/08 4:39p erickson
 * PR41384: fix coverity warning
 *
 * 3   4/2/08 10:15a erickson
 * PR41216: move null check to top
 *
 * 2   2/21/08 5:58p jgarrett
 * PR 39017: Adding security handle
 *
 * 1   2/1/08 5:25p jgarrett
 * PR 39017: Adding hdmi_output
 *
 **************************************************************************/
#include "nexus_hdmi_output_module.h"
#include "nexus_base.h"
#include "priv/nexus_core.h"
#include "nexus_hdmi_output_init.h"

BDBG_MODULE(nexus_hdmi_output);

/* global module handle & data */
NEXUS_ModuleHandle g_NEXUS_hdmiOutputModule;
NEXUS_HdmiOutputModuleSettings g_NEXUS_hdmiOutputModuleSettings;

void NEXUS_HdmiOutputModule_GetDefaultSettings(
    NEXUS_HdmiOutputModuleSettings *pSettings    /* [out] */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_ModuleHandle NEXUS_HdmiOutputModule_Init(
    const NEXUS_HdmiOutputModuleSettings *pSettings  /* NULL will use default settings */
    )
{
    NEXUS_ModuleSettings moduleSettings;
    NEXUS_Error errCode;

    BDBG_ASSERT(NULL == g_NEXUS_hdmiOutputModule);

    #if NEXUS_HAS_SECURITY
    if (!pSettings || !pSettings->modules.security) {
        BDBG_WRN(("security module handle required"));
        return NULL;
    }
    #else
    if (!pSettings) {
        NEXUS_HdmiOutputModule_GetDefaultSettings(&g_NEXUS_hdmiOutputModuleSettings);
        pSettings = &g_NEXUS_hdmiOutputModuleSettings;
    }
    #endif

    /* init global module handle */
    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLow; /* hdmi interface is very slow */
    moduleSettings.dbgPrint = NEXUS_HdmiOutputModule_Print;
    moduleSettings.dbgModules = "nexus_hdmi_output";
    g_NEXUS_hdmiOutputModule = NEXUS_Module_Create("hdmi_output", &moduleSettings);
    if ( NULL == g_NEXUS_hdmiOutputModule )
    {
        errCode = BERR_TRACE(BERR_OS_ERROR);
        return NULL;
    }

    if (pSettings != &g_NEXUS_hdmiOutputModuleSettings) {
        g_NEXUS_hdmiOutputModuleSettings = *pSettings;
    }

    /* Success */
    return g_NEXUS_hdmiOutputModule;
}

void NEXUS_HdmiOutputModule_Uninit(void)
{
    BDBG_ASSERT(NULL != g_NEXUS_hdmiOutputModule);
    NEXUS_LockModule();
    NEXUS_HdmiOutput_P_Shutdown();
    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(g_NEXUS_hdmiOutputModule);
    BKNI_Memset(&g_NEXUS_hdmiOutputModuleSettings, 0, sizeof(g_NEXUS_hdmiOutputModuleSettings));
    g_NEXUS_hdmiOutputModule = NULL;
    return;
}

