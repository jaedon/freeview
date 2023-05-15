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
 * $brcm_Workfile: nexus_file_module.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 10/4/12 11:32a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/file/src/nexus_file_module.c $
 * 
 * 6   10/4/12 11:32a erickson
 * SW7435-362: lock module during init and uninit
 * 
 * 5   9/6/11 10:10a erickson
 * SWNOOS-482: correct internal spelling
 * 
 * 4   9/6/11 10:05a erickson
 * SWNOOS-482: add NEXUS_FileModuleSettings.schedulerSettings[] per-thread
 *  array
 * 
 * 3   4/20/11 7:48p vsilyaev
 * SW7425-394: Allow application to set number of I/O worker threads that
 *  are serving I/O requests
 * 
 * 2   11/11/10 5:17p vsilyaev
 * SW7405-4995: Use BKNI_Memcmp to preinitialize structure used in
 *  GetDefaultSettings
 * 
 * 1   1/18/08 2:16p jgarrett
 * PR 38808: Merging to main branch
 * 
 * Nexus_Devel/2   10/11/07 6:24p vsilyaev
 * PR 35824: Fixed naming
 * 
 * Nexus_Devel/1   10/10/07 3:57p vsilyaev
 * PR 35824: File I/O module
 * 
 **************************************************************************/
#include "nexus_file_module.h"

BDBG_MODULE("nexus_file_module");

NEXUS_ModuleHandle NEXUS_FileModule;

void 
NEXUS_FileModule_GetDefaultSettings(NEXUS_FileModuleSettings *pSettings)
{
    unsigned i;
    BDBG_ASSERT(pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->maxQueuedElements = 30;
    pSettings->workerThreads = 2;
    for (i=0;i<NEXUS_FILE_MAX_IOWORKERS;i++) {
        NEXUS_Thread_GetDefaultSettings(&pSettings->schedulerSettings[i]);
    }
    return;
}

NEXUS_ModuleHandle 
NEXUS_FileModule_Init(const NEXUS_FileModuleSettings *pSettings)
{
    BERR_Code rc;
    NEXUS_FileModuleSettings settings;

    if(pSettings==NULL) {
        NEXUS_FileModule_GetDefaultSettings(&settings);
        pSettings = &settings;
    }
    NEXUS_FileModule = NEXUS_Module_Create("file", NULL);
    if(!NEXUS_FileModule) { rc = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY); goto err_module; }
    NEXUS_LockModule();
    rc = NEXUS_File_P_Scheduler_Start(pSettings);
    if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_scheduler;}
    NEXUS_UnlockModule();
    return NEXUS_FileModule;

err_scheduler:
    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(NEXUS_FileModule);
err_module:
    return NULL;
}
    
void 
NEXUS_FileModule_Uninit(void)
{
    NEXUS_LockModule();
    NEXUS_File_P_Scheduler_Stop();
    NEXUS_UnlockModule();
    NEXUS_Module_Destroy(NEXUS_FileModule);
    NEXUS_FileModule = NULL;
    return;
}




