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
* $brcm_Workfile: nexus_frontend_init.c $
* $brcm_Revision: 7 $
* $brcm_Date: 9/5/12 6:09p $
*
* API Description:
*   API name: Frontend Module
*    Frontend module private APIs and module data.
*
* Revision History:
*
* $brcm_Log: /nexus/modules/frontend/common/src/nexus_frontend_init.c $
*
* 7   9/5/12 6:09p jtna
* SW7425-3782: rework host and demod TSMF impl. both cases now go through
*  the host API
*
* 6   7/18/12 6:23p jtna
* SW7346-928: add support for MTSIF frontend<->PB mapping with frontend
*  daisy-chain
*
* 5   8/3/11 11:09a gmohile
* SW7125-1014 : Add module to active priority scheduler
*
* 4   1/26/09 11:29a erickson
* PR51468: global variable naming convention
*
* 3   8/18/08 10:28a katrep
* PR45674: Compiler warnings in DEBUG=n builds
*
* 2   4/2/08 11:31a erickson
* PR40198: fix DEBUG=n warning
*
* 1   1/18/08 2:19p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/2   10/11/07 9:24a erickson
* PR36017: clean up on Uninit
*
* Nexus_Devel/1   10/5/07 5:52p jgarrett
* PR 35551: Adding initial version
*
***************************************************************************/

#include "nexus_frontend_module.h"
#include "nexus_frontend_init.h"

NEXUS_ModuleHandle g_NEXUS_frontendModule;
NEXUS_FrontendModuleSettings g_NEXUS_frontendModuleSettings;
#if NEXUS_HAS_MXT
extern struct NEXUS_FrontendHostMtsifConfig g_NEXUS_Frontend_P_HostMtsifConfig;
#endif
/***************************************************************************
Summary:
    Get Default settings for the frontend module
See Also:
    NEXUS_FrontendModule_Init
 ***************************************************************************/
void NEXUS_FrontendModule_GetDefaultSettings(
    NEXUS_FrontendModuleSettings *pSettings /* [out] */
    )
{
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

/***************************************************************************
Summary:
    Initialize the frontend module
See Also:
    NEXUS_FrontendModule_Uninit
 ***************************************************************************/
NEXUS_ModuleHandle NEXUS_FrontendModule_Init(
    const NEXUS_FrontendModuleSettings *pSettings
    )
{
    NEXUS_ModuleSettings moduleSettings;
    int rc = 0;

    BSTD_UNUSED(pSettings);
    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pSettings->i2cModule);
    BDBG_ASSERT(NULL != pSettings->transport);
    BDBG_ASSERT(NULL == g_NEXUS_frontendModule);

    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eIdleActiveStandby; /* frontend interfaces are very slow */
    g_NEXUS_frontendModule = NEXUS_Module_Create("frontend", &moduleSettings);

    if (NULL ==g_NEXUS_frontendModule) {
        rc = BERR_TRACE(BERR_OS_ERROR);
        return NULL;
    }
#if !defined(NEXUS_FRONTEND_45216) && defined(NEXUS_HAS_MXT)
    BKNI_Memset(&g_NEXUS_Frontend_P_HostMtsifConfig, 0, sizeof(g_NEXUS_Frontend_P_HostMtsifConfig));
#endif
    NEXUS_Frontend_P_Init();

    g_NEXUS_frontendModuleSettings = *pSettings;
    return g_NEXUS_frontendModule;
}

/***************************************************************************
Summary:
    Un-Initialize the frontend module
See Also:
    NEXUS_FrontendModule_Init
 ***************************************************************************/
void NEXUS_FrontendModule_Uninit(void)
{
    NEXUS_Module_Destroy(g_NEXUS_frontendModule);
    g_NEXUS_frontendModule = NULL;
}

