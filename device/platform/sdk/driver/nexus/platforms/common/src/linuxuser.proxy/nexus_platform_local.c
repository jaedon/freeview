/***************************************************************************
*     (c)2011-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_platform_local.c $
* $brcm_Revision: 3 $
* $brcm_Date: 10/16/12 4:26p $
*
* API Description: code that is common for linuxuser.client and linuxuser.proxy client libraries
* 
* Revision History:
*
* $brcm_Log: /nexus/platforms/common/src/linuxuser.proxy/nexus_platform_local.c $
* 
* 3   10/16/12 4:26p erickson
* SW7435-421: must init local magnum before app ipc starts
* 
* 2   9/28/12 11:19a erickson
* SW7435-362: stop scheduler threads before uninitializing modules. this
*  ensures no lock is held when it is destroyed.
* 
* 1   9/4/12 4:04p erickson
* SW7420-1123: refactor common, local code for linuxuser.client and
*  linuxuser.proxy
* 
***************************************************************************/
#include "nexus_platform_priv_proxy.h"
#if NEXUS_HAS_FILE
#include "nexus_file_init.h"
#endif
#if NEXUS_HAS_PLAYBACK
#include "nexus_playback_init.h"
#endif
#if NEXUS_HAS_RECORD
#include "nexus_record_init.h"
#endif
#if NEXUS_HAS_FILE_MUX
#include "nexus_file_mux_init.h"
#endif
#if NEXUS_HAS_WMDRMPD
#include "nexus_wmdrmpd_init.h"
#endif

extern NEXUS_HeapHandle NEXUS_Platform_CreateHeap_proxy( const NEXUS_PlatformCreateHeapSettings *pSettings );
extern void NEXUS_Platform_DestroyHeap_proxy( NEXUS_HeapHandle heap );

static bool g_NEXUS_magnum_init = false;

NEXUS_Error
NEXUS_Platform_P_Magnum_Init(void)
{
    BERR_Code rc;
    if (g_NEXUS_magnum_init==false)
    {
        rc = BKNI_Init();
        if(rc!=BERR_SUCCESS) {
            goto err_kni;
        }
        rc = BDBG_Init();
        if(rc!=BERR_SUCCESS) {
            goto err_dbg;
        }
        rc = NEXUS_Base_Core_Init();
        if(rc!=BERR_SUCCESS) {
            goto err_base;
        }
        g_NEXUS_magnum_init = true;
    }

    return BERR_SUCCESS;
err_base:
    BDBG_Uninit();
err_dbg:
    BKNI_Uninit();
err_kni:
    return rc;
}

void
NEXUS_Platform_P_Magnum_Uninit(void)
{
    if(g_NEXUS_magnum_init) {
        NEXUS_Base_Core_Uninit();
        BDBG_Uninit();
        BKNI_Uninit();
        g_NEXUS_magnum_init = false;
    }
    return;
}

NEXUS_Error
NEXUS_Platform_P_ModulesInit(const NEXUS_PlatformSettings *pSettings)
{
    NEXUS_Error errCode;

#if NEXUS_HAS_PLAYBACK
    NEXUS_PlaybackModuleSettings playbackSettings;
    NEXUS_ModuleHandle playback;
#endif
#if NEXUS_HAS_RECORD
    NEXUS_RecordModuleSettings recordSettings;
#endif
#if NEXUS_HAS_FILE
    NEXUS_ModuleHandle file;
    NEXUS_ModuleHandle record;
    NEXUS_FileModuleSettings  fileModuleSettings;
#endif
#if NEXUS_HAS_WMDRMPD
    NEXUS_ModuleHandle wmdrmpd;
    NEXUS_WmDrmPdModuleSettings wmdrmpdSettings;
#endif

#if NEXUS_HAS_FILE
    BDBG_MSG((">FILE"));
    NEXUS_FileModule_GetDefaultSettings(&fileModuleSettings);
    /* TODO: need separate struct that can be nested in NEXUS_ClientAuthenticationSettings so that clients can get module init settings here */
    if(pSettings) {
        fileModuleSettings.workerThreads = pSettings->fileModuleSettings.workerThreads;
        BKNI_Memcpy(fileModuleSettings.schedulerSettings, pSettings->fileModuleSettings.schedulerSettings, sizeof(fileModuleSettings.schedulerSettings));
    }
    file = NEXUS_FileModule_Init(&fileModuleSettings);
    if (!file) {
        BDBG_ERR(("Unable to init file"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    }
#endif

#if NEXUS_HAS_PLAYBACK
    BDBG_MSG((">PLAYBACK"));
    NEXUS_PlaybackModule_GetDefaultSettings(&playbackSettings);
    playbackSettings.modules.file = file;
    playbackSettings.modules.audioDecoder = NEXUS_Platform_P_State.module;
    playbackSettings.modules.videoDecoder = NEXUS_Platform_P_State.module;
    playbackSettings.modules.playpump = NEXUS_Platform_P_State.module;
    playback = NEXUS_PlaybackModule_Init(&playbackSettings);
    if (!playback) {
        BDBG_ERR(("Unable to init playback"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    }
#endif

#if NEXUS_HAS_RECORD
    BDBG_MSG((">RECORD"));
    NEXUS_RecordModule_GetDefaultSettings(&recordSettings);
    recordSettings.modules.file = file;
    recordSettings.modules.recpump = NEXUS_Platform_P_State.module;
#if NEXUS_HAS_PLAYBACK
    recordSettings.modules.playback = playback;
#endif
    record = NEXUS_RecordModule_Init(&recordSettings);
    if (!record) {
        BDBG_ERR(("Unable to init record"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    }
#endif
#if NEXUS_HAS_FILE_MUX
    BDBG_MSG((">FILE_MUX"));
    NEXUS_FileMuxModule_Init(NULL);
#endif

#if NEXUS_HAS_WMDRMPD
    NEXUS_WmDrmPdModule_GetDefaultSettings(&wmdrmpdSettings);
    wmdrmpdSettings.modules.wmdrmpdCore = NEXUS_Platform_P_State.module;
    wmdrmpdSettings.modules.wmdrmpdIo = NEXUS_Platform_P_State.module;
    wmdrmpd = NEXUS_WmDrmPdModule_Init(&wmdrmpdSettings);
    if (!wmdrmpd) {
        BDBG_ERR(("Unable to init wmdrmpd"));
        errCode = BERR_TRACE(NEXUS_NOT_SUPPORTED);
        goto err;
    }
#endif
    return NEXUS_SUCCESS;

err:
    return errCode;
}

void
NEXUS_Platform_P_ModulesUninit(void)
{
    NEXUS_Base_Stop();
    
#if NEXUS_HAS_WMDRMPD
    NEXUS_WmDrmPdModule_Uninit();
#endif
#if NEXUS_HAS_FILE_MUX
    NEXUS_FileMuxModule_Uninit();
#endif
#if NEXUS_HAS_RECORD
    BDBG_MSG(("<RECORD"));
    NEXUS_RecordModule_Uninit();
#endif
#if NEXUS_HAS_PLAYBACK
    BDBG_MSG(("<PLAYBACK"));
    NEXUS_PlaybackModule_Uninit();
#endif
#if NEXUS_HAS_FILE
    BDBG_MSG(("<FILE"));
    NEXUS_FileModule_Uninit();
#endif
}

void NEXUS_Platform_GetDefaultStandbySettings( NEXUS_PlatformStandbySettings *pSettings )
{
    NEXUS_Platform_P_Magnum_Init();
    memset(pSettings, 0, sizeof(*pSettings));
}

void NEXUS_Platform_GetDefaultClientAuthenticationSettings( NEXUS_ClientAuthenticationSettings *pSettings )
{
    NEXUS_Platform_P_Magnum_Init();
    memset(pSettings, 0, sizeof(*pSettings));
}

NEXUS_HeapHandle NEXUS_Platform_CreateHeap( const NEXUS_PlatformCreateHeapSettings *pSettings )
{
    NEXUS_HeapHandle heap;
    unsigned i;

    /* find a slot */
    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        if (!NEXUS_Platform_P_State.mmaps[i].heap) break;
    }
    if (i == NEXUS_MAX_HEAPS) {
        BERR_TRACE(NEXUS_NOT_AVAILABLE);
        return NULL;
    }

    heap = NEXUS_Platform_CreateHeap_proxy(pSettings);
    if (heap) {
        int rc;
        rc = nexus_p_add_heap(i, heap, pSettings->offset, pSettings->size, pSettings->memoryType, true, pSettings->userAddress);
        if (rc) {rc = BERR_TRACE(rc); goto error;}
    }
    return heap;

error:
    NEXUS_Platform_DestroyHeap_proxy(heap);
    return NULL;
}

void NEXUS_Platform_DestroyHeap( NEXUS_HeapHandle heap )
{
    unsigned i;

    for (i=0;i<NEXUS_MAX_HEAPS;i++) {
        if (NEXUS_Platform_P_State.mmaps[i].heap == heap && NEXUS_Platform_P_State.mmaps[i].dynamic) {
            nexus_p_remove_heap(i);
            NEXUS_Platform_DestroyHeap_proxy(heap);
            break;
        }
    }
}
