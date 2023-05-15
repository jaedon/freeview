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
* $brcm_Workfile: nexus_dvb_ci_module.c $
* $brcm_Revision: 1 $
* $brcm_Date: 2/19/08 3:32p $
*
* API Description:
*   API name: DvbCi
*    Specific APIs related DVB Common Interface
*
* Revision History:
*
* $brcm_Log: /nexus/modules/dvb_ci/3563/src/nexus_dvb_ci_module.c $
* 
* 1   2/19/08 3:32p jgarrett
* PR 39610: Adding DvbCi
*
***************************************************************************/

#include "nexus_dvb_ci_module.h"

BDBG_MODULE(nexus_dvb_ci);

NEXUS_ModuleHandle g_dvbCiModule;

BDBG_OBJECT_ID(NEXUS_DvbCi);

void NEXUS_DvbCiModule_GetDefaultSettings(
    NEXUS_DvbCiModuleSettings *pSettings
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_ModuleHandle NEXUS_DvbCiModule_Init(
    const NEXUS_DvbCiModuleSettings *pSettings
    )
{
    NEXUS_ModuleSettings moduleSettings;

    BDBG_ASSERT(NULL != pSettings);
    BDBG_ASSERT(NULL != pSettings->modules.gpio);

    NEXUS_Module_GetDefaultSettings(&moduleSettings);
    moduleSettings.priority = NEXUS_ModulePriority_eLow;    /* There are some long blocking routines in here */
    g_dvbCiModule = NEXUS_Module_Create("dvb_ci", &moduleSettings);

    if ( NULL == g_dvbCiModule )
    {
        BDBG_ERR(("Unable to create module"));
    }

    return g_dvbCiModule;
}

void NEXUS_DvbCiModule_Uninit(void)
{
    BDBG_ASSERT(NULL != g_dvbCiModule);
    NEXUS_Module_Destroy(g_dvbCiModule);
    g_dvbCiModule = NULL;
}

