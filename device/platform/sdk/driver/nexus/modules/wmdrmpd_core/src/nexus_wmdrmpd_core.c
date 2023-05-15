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
* $brcm_Workfile: nexus_wmdrmpd_core.c $
* $brcm_Revision: 5 $
* $brcm_Date: 10/4/12 5:04p $
*
* API Description:
*   API name: wmdrmpd
*    Specific APIs related to Microsoft Windows Media DRM PD
*
* Revision History:
*
* $brcm_Log: /nexus/modules/wmdrmpd_core/src/nexus_wmdrmpd_core.c $
* 
* 5   10/4/12 5:04p hugost
* SW7425-4027:Merge into main.
* 
* 3   10/3/12 9:41a hugost
* SW7425-3986: Merges into main.
* 
* SW7425-3986/2   10/2/12 10:14a hugost
* SW7425-3986: properly handle clearing active io upon failure
* 
* SW7425-3986/1   9/24/12 3:45p hugost
* Added 2 new funcitons NEXUS_WmDrmPdCore_GetDefaultAesCounterInfo() and
*  NEXUS_WmDrmPdCore_ProcessBlocksAesCounter()
* 
* 2   8/23/12 3:57p jgarrett
* SW7425-3757: Merge to main branch
* 
* SW7425-3757/1   8/15/12 4:54p hugost
* Added Fragmented MP4 support.
* Added policy callback support.
* 
* 1   7/27/12 2:26p jgarrett
* SW7425-3281: Merge to main branch
* 
* SW7425-3281/9   7/27/12 2:04p piyushg
* SW7425-3281: Add cocktail mode support(DRM10 backward compatibility)
* 
* SW7425-3281/8   7/19/12 1:57p piyushg
* SW7231-909: Code cleanup. Fix memory leak.
* 
* SW7425-3281/7   7/17/12 5:07p jgarrett
* SW7425-3281: Correcting cryptoEngine logic for generic key slots
* 
* SW7425-3281/6   7/17/12 4:44p jgarrett
* SW7425-3281: Incorporating code review feedback
* 
* SW7425-3281/5   7/13/12 5:46p piyushg
* W7425-3281: Fix bunch of compile time warnings in both usermode and
*  kernelmode
* 
* SW7425-3281/4   7/13/12 3:04p piyushg
* SW7425-3281: Modifications based on nexus review.
* 
* SW7425-3281/3   7/11/12 6:02p piyushg
* SW7425-3281: Initial checkin with nexus kernel mode support
* 
* SW7425-3281/2   7/8/12 4:54p piyushg
* SW7425-3281: Added support for playready licensce acquisition and Nexus
*  fileio
* 
* SW7425-3281/1   6/20/12 2:35p piyushg
* SW7425-3281: Initial checkin of Nexus directory structure and header
*  files.
* 
***************************************************************************/

#include "nexus_wmdrmpd_core_module.h"
#include "nexus_memory.h"
#include "priv/nexus_core.h"
#include "priv/nexus_playpump_priv.h"
#include "priv/nexus_core_security.h"
#include "nexus_security_types.h"
#include "prdy_core.h"
#include "prdy_mp4.h"
#include "prdy_priv.h"
#include "prdy_decryptor.h"

BDBG_MODULE(nexus_wmdrmpd_core);

#define MAX_LICENCE_RESPONSE_LENGTH (1024*64)
#define DEFAULT_LICENSE_CHALLENGE_LENGTH (1024*64)
#define DEFAULT_LICENSE_URL_LENGTH (1024*1)
#define POLICY_POOL_SIZE (2)

typedef struct NEXUS_WmDrmPdCore
{
    NEXUS_OBJECT(NEXUS_WmDrmPdCore);
    NEXUS_WmDrmPdCoreSettings settings;
    bdrm_t bdrm;
    NEXUS_TaskCallbackHandle policyCallback;
    bdrm_decryptor_t bdrmDecryptor;
    NEXUS_KeySlotHandle keySlot;
    NEXUS_WmDrmPdEncryptionType encrType;
    NEXUS_WmDrmPdLicenseChallenge licenseData;
    bdrm_policy_t policy[POLICY_POOL_SIZE];  /* Some license have more than 1 policy to enforce before playing content. This array is
                                                used to store them up when the bdrm policy callback fires. */
    uint32_t nbOfPolicyStacked;
} NEXUS_WmDrmPdCore;

static void NEXUS_WmDrmPdCore_P_Finalizer(
    NEXUS_WmDrmPdCoreHandle hCore
    )
{
    NEXUS_WMDRMPD_SET_ACTIVE_IO(hCore->settings.ioHandle);
    
    if(hCore->licenseData.pResponseBuffer != NULL) NEXUS_Memory_Free((void*)(hCore->licenseData.pResponseBuffer));
    if(hCore->licenseData.pData != NULL)  NEXUS_Memory_Free((void*)(hCore->licenseData.pData));
    if(hCore->licenseData.pUrl != NULL) NEXUS_Memory_Free((void*)(hCore->licenseData.pUrl));

    if(hCore->settings.transportType == NEXUS_TransportType_eAsf){
        if (hCore->encrType == NEXUS_WmDrmPdEncryptionType_eAesCtr) 
        {
            if(hCore->bdrm->headers.asf.psi.data != NULL) NEXUS_Memory_Free((void*)(hCore->bdrm->headers.asf.psi.data));
        }
        if (hCore->encrType == NEXUS_WmDrmPdEncryptionType_eWmdrm)
        {
            if(hCore->bdrm->headers.asf.encr.sec_data != NULL)  NEXUS_Memory_Free((void *)(hCore->bdrm->headers.asf.encr.sec_data));
            if(hCore->bdrm->headers.asf.encr.prot_type != NULL) NEXUS_Memory_Free((void *)(hCore->bdrm->headers.asf.encr.prot_type));
            if(hCore->bdrm->headers.asf.encr.keyid != NULL) NEXUS_Memory_Free((void *)(hCore->bdrm->headers.asf.encr.keyid));
            if(hCore->bdrm->headers.asf.encr.licurl != NULL) NEXUS_Memory_Free((void *)(hCore->bdrm->headers.asf.encr.licurl));
            if(hCore->bdrm->headers.asf.xencr.data != NULL)NEXUS_Memory_Free((void *)(hCore->bdrm->headers.asf.xencr.data));
        }
    }

    bdrm_close(hCore->bdrm);
    NEXUS_TaskCallback_Destroy(hCore->policyCallback);
    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    NEXUS_OBJECT_ASSERT(NEXUS_WmDrmPdCore, hCore);
    NEXUS_OBJECT_DESTROY(NEXUS_WmDrmPdCore, hCore);
    BKNI_Free(hCore);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_WmDrmPdCore, NEXUS_WmDrmPdCore_Destroy);


static bdrm_err NEXUS_WmDrmPdCore_P_PolicyCallback(
    bdrm_policy_t *policy,
    const void* ctx
    )
{
    NEXUS_WmDrmPdCoreHandle handle = (NEXUS_WmDrmPdCoreHandle)ctx;

    if(handle->nbOfPolicyStacked < POLICY_POOL_SIZE){ 
        BKNI_Memcpy(&handle->policy[handle->nbOfPolicyStacked], policy, sizeof(bdrm_policy_t));
        handle->nbOfPolicyStacked++;
    }
    else {
        BDBG_ERR(("Policy lost. The application needs to call NEXUS_WmDrmPdCore_GetPolicyStatus()"));
    }

    NEXUS_TaskCallback_Fire(handle->policyCallback);

    return bdrm_err_ok;
}

void NEXUS_WmDrmPdCore_GetDefaultSettings(
    NEXUS_WmDrmPdCoreSettings *pSettings    /* [out] default settings */
    )
{
    BDBG_ASSERT(NULL != pSettings);

    BKNI_Memset(pSettings, 0, sizeof(NEXUS_WmDrmPdCoreSettings));

    pSettings->heap = NULL;
    pSettings->playpump = NULL;
    pSettings->transportType = NEXUS_TransportType_eUnknown;
}

NEXUS_WmDrmPdCoreHandle NEXUS_WmDrmPdCore_Create( /* attr{destructor=NEXUS_WmDrmPdCore_Destroy} */
    const NEXUS_WmDrmPdCoreSettings *pSettings  
    )
{
    NEXUS_WmDrmPdCoreSettings defaults;
    NEXUS_WmDrmPdCoreHandle hCore;
    NEXUS_HeapHandle hHeap;
    NEXUS_MemoryAllocationSettings allocSettings;
    NEXUS_Error errCode;
    bdrm_cfg cfg;

    if ( NULL == pSettings )
    {
        NEXUS_WmDrmPdCore_GetDefaultSettings(&defaults);
        pSettings = &defaults;
    }

    hCore = BKNI_Malloc(sizeof(NEXUS_WmDrmPdCore));
    if ( NULL == hCore )
    {
        (void)BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
        goto err_malloc;
    }
    BKNI_Memset(hCore, 0, sizeof(NEXUS_WmDrmPdCore));

    hHeap = NEXUS_P_DefaultHeap(pSettings->heap, NEXUS_DefaultHeapType_eFull);
    if ( NULL == hHeap )
    {
        hHeap = g_pCoreHandles->nexusHeap[0];
    }

    if (!NEXUS_P_CpuAccessibleHeap(hHeap)) 
    {
        BDBG_ERR(("Core heap is not CPU accessible.  Please provide a CPU-accessible heap in NEXUS_WmDrmPdCoreSettings."));
        (void)BERR_TRACE(NEXUS_INVALID_PARAMETER);
        goto err_heap;
    }

    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    allocSettings.heap = hHeap;
    errCode = NEXUS_Memory_Allocate(MAX_LICENCE_RESPONSE_LENGTH, &allocSettings, (void *)(&hCore->licenseData.pResponseBuffer));
    if ( errCode )
    {
       (void)BERR_TRACE(errCode);
        goto err_alloc_response;
    }

    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    allocSettings.heap = hHeap;
    errCode = NEXUS_Memory_Allocate(DEFAULT_LICENSE_CHALLENGE_LENGTH, &allocSettings, (void *)(&hCore->licenseData.pData));
    if ( errCode )
    {
       (void)BERR_TRACE(errCode);
        goto err_alloc_data;
    }

    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    allocSettings.heap = hHeap;
    errCode = NEXUS_Memory_Allocate(DEFAULT_LICENSE_URL_LENGTH, &allocSettings, (void *)(&hCore->licenseData.pUrl));
    if ( errCode )
    {
       (void)BERR_TRACE(errCode);
        goto err_alloc_url;
    }

    hCore->settings.heap = hHeap;
    hCore->settings = *pSettings;
    NEXUS_OBJECT_SET(NEXUS_WmDrmPdCore, hCore);

    NEXUS_WMDRMPD_SET_ACTIVE_IO(hCore->settings.ioHandle);

    BKNI_Memset(&cfg, 0, sizeof(bdrm_cfg));
    cfg.ext_asf_scanning = true;

    switch(pSettings->transportType){
    case NEXUS_TransportType_eAsf:
        cfg.cnt_type = bdrm_cnt_asf;
        /* Get asf handle */
        if(pSettings->playpump)
        {
             if(cfg.cnt_type == bdrm_cnt_asf)
             {
                 cfg.asf = NEXUS_Playpump_GetAsfHandle_priv(pSettings->playpump);
                 if(  cfg.asf == NULL )
                 {
                     BDBG_WRN(("drm asf is NULL"));
                 }
             }
             else 
             {
                 cfg.asf = NULL;
             }
        }
        else
        {
            BDBG_WRN(("Playpump handle not set"));
        }
        break;
    case NEXUS_TransportType_eMp4Fragment:
        cfg.cnt_type = bdrm_cnt_mp4;
        break;
    default:
        errCode = NEXUS_INVALID_PARAMETER;
        (void)BERR_TRACE(errCode);
        goto err_type;
        break;
    }

    cfg.hHeap = hHeap;

    if(pSettings->policyCallback.callback){
        hCore->policyCallback = NEXUS_TaskCallback_Create(hCore, NULL);
        if(hCore->policyCallback == NULL){
            BDBG_ERR(("No policy callback provided in the configuration"));
            errCode = NEXUS_INVALID_PARAMETER;
            (void)BERR_TRACE(errCode);
            goto err_type;
        }
        
        NEXUS_TaskCallback_Set(hCore->policyCallback, &pSettings->policyCallback);

        cfg.opl_cback = NEXUS_WmDrmPdCore_P_PolicyCallback;
        cfg.opl_ctx = hCore;
    }
    else {
        BDBG_ERR(("No policy callback provided in the configuration"));
        errCode = NEXUS_INVALID_PARAMETER;
        (void)BERR_TRACE(errCode);
        goto err_type;    
    }

    if((hCore->bdrm = bdrm_open("", &cfg)) == NULL)
    {
        BDBG_ERR(("DRM Open failed"));
        goto err_drm_open;
    }
    else 
    {
        if( bdrm_init_handlers( hCore->bdrm ) == bdrm_err_ok )
        {
            if( bdrm_dcrypt_instance( hCore->bdrm, &hCore->bdrmDecryptor ) == bdrm_err_ok )
            {
                BDBG_MSG(("bdrm_dcrypt_instance success"));
            }
            else
            {
                BDBG_WRN(("bdrm_dcrypt_instance failed"));
            }
        }
        else
        {
            BDBG_WRN(("bdrm_init_handlers failed"));
        }
    }


    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    return hCore;

err_drm_open:
    NEXUS_TaskCallback_Destroy(hCore->policyCallback);
err_type:
    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    NEXUS_Memory_Free((void *)hCore->licenseData.pUrl);
err_alloc_url:
    NEXUS_Memory_Free((void *)hCore->licenseData.pData);
err_alloc_data:
    NEXUS_Memory_Free((void *)hCore->licenseData.pResponseBuffer);
err_alloc_response:
err_heap:
    BKNI_Free(hCore);
err_malloc:
    return NULL;
}

NEXUS_Error NEXUS_WmDrmPdCore_ConfigureKeySlot(
    NEXUS_WmDrmPdCoreHandle handle,
    NEXUS_KeySlotHandle keySlot
    )
{
    NEXUS_OBJECT_ASSERT(NEXUS_WmDrmPdCore, handle);
    NEXUS_OBJECT_ASSERT(NEXUS_KeySlot, keySlot);
    if ( keySlot->cryptoEngine != NEXUS_SecurityEngine_eGeneric )
    {
        BDBG_ERR(("A generic keyslot is required for DRM decryption"));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    if(handle->settings.transportType == NEXUS_TransportType_eAsf){
        keySlot->drmContext = (unsigned int)handle->bdrmDecryptor;
    }else {
        keySlot->drmContext = (unsigned int)handle->bdrm;
    }
    return BERR_SUCCESS;
}

void NEXUS_WmDrmPdCore_GetDefaultPsiObjectInfo(
    NEXUS_WmDrmPdPsiObjectInfo *pObject     /* [out] */
    )
{   
    BKNI_Memset(pObject, 0, sizeof(*pObject));
}

NEXUS_Error NEXUS_WmDrmPdCore_SetPsiObject(
    NEXUS_WmDrmPdCoreHandle handle,
    const NEXUS_WmDrmPdPsiObjectInfo *pInfo,
    const void *pData,
    size_t dataLength
    )
{
    NEXUS_Error errCode = NEXUS_SUCCESS;
    NEXUS_MemoryAllocationSettings allocSettings;

    NEXUS_WMDRMPD_SET_ACTIVE_IO(handle->settings.ioHandle); 
    NEXUS_OBJECT_ASSERT(NEXUS_WmDrmPdCore, handle);

    handle->encrType = NEXUS_WmDrmPdEncryptionType_eMax;

    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    allocSettings.heap = handle->settings.heap;
    errCode = NEXUS_Memory_Allocate(dataLength, &allocSettings, (void *)(&handle->bdrm->headers.asf.psi.data));
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
        return BERR_OUT_OF_SYSTEM_MEMORY;
    }

    BDBG_MSG(("SET PSI HEADER"));
    BKNI_Memcpy(handle->bdrm->headers.asf.psi.data, pData, dataLength);
    handle->bdrm->headers.asf.psi.size = dataLength;
    handle->bdrm->headers.asf.psi.sysversion = pInfo->sysVersion;
    BKNI_Memcpy(&(handle->bdrm->headers.asf.psi.systemId), &(pInfo->systemId), sizeof(NEXUS_WmDrmPdGuid));
    handle->bdrm->headers.asf.psi.state = bdrm_hdr_read_ok;
    handle->encrType = NEXUS_WmDrmPdEncryptionType_eAesCtr;

    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_WmDrmPdCore_SetCencrObject(
    NEXUS_WmDrmPdCoreHandle handle,
    const void *pSecurityData,  
    size_t securityLength,      
    const void *pProtocolData,  
    size_t protocolLength,      
    const void *pKeyIdData,     
    size_t keyIdLength,         
    const void *pLicenseUrlData,
    size_t licenseUrlLength     
    )
{
    NEXUS_Error errCode = NEXUS_SUCCESS;
    NEXUS_MemoryAllocationSettings allocSettings;

    NEXUS_WMDRMPD_SET_ACTIVE_IO(handle->settings.ioHandle); 
    NEXUS_OBJECT_ASSERT(NEXUS_WmDrmPdCore, handle);

    handle->encrType = NEXUS_WmDrmPdEncryptionType_eMax;

    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    allocSettings.heap = handle->settings.heap;
    errCode = NEXUS_Memory_Allocate(securityLength, &allocSettings, (void *)(&handle->bdrm->headers.asf.encr.sec_data));
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
        return BERR_OUT_OF_SYSTEM_MEMORY;
    }
    errCode = NEXUS_Memory_Allocate(protocolLength, &allocSettings, (void *)(&handle->bdrm->headers.asf.encr.prot_type));
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
        return BERR_OUT_OF_SYSTEM_MEMORY;
    }
    errCode = NEXUS_Memory_Allocate(keyIdLength, &allocSettings, (void *)(&handle->bdrm->headers.asf.encr.keyid));
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
        return BERR_OUT_OF_SYSTEM_MEMORY;
    }
    errCode = NEXUS_Memory_Allocate(licenseUrlLength, &allocSettings, (void *)(&handle->bdrm->headers.asf.encr.licurl));
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
        return BERR_OUT_OF_SYSTEM_MEMORY;
    }

    BDBG_MSG(("SET CENCR HEADER"));
    BKNI_Memcpy(handle->bdrm->headers.asf.encr.sec_data, pSecurityData, securityLength);
    BKNI_Memcpy(handle->bdrm->headers.asf.encr.prot_type, pProtocolData, protocolLength);
    BKNI_Memcpy(handle->bdrm->headers.asf.encr.keyid, pKeyIdData, keyIdLength);
    BKNI_Memcpy(handle->bdrm->headers.asf.encr.licurl, pLicenseUrlData, licenseUrlLength);
    handle->bdrm->headers.asf.encr.sec_dlen = securityLength;
    handle->bdrm->headers.asf.encr.prot_tlen = protocolLength;
    handle->bdrm->headers.asf.encr.keyid_len = keyIdLength;
    handle->bdrm->headers.asf.encr.licurl_len = licenseUrlLength;
    handle->bdrm->headers.asf.encr.state = bdrm_hdr_read_ok;
    handle->encrType = NEXUS_WmDrmPdEncryptionType_eWmdrm;

    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    return BERR_SUCCESS;
}

NEXUS_Error NEXUS_WmDrmPdCore_SetXcencrObject(
    NEXUS_WmDrmPdCoreHandle handle,
    const void *pData,  
    size_t dataLength  
    )
{

    NEXUS_Error errCode = NEXUS_SUCCESS;
    NEXUS_MemoryAllocationSettings allocSettings;

    NEXUS_WMDRMPD_SET_ACTIVE_IO(handle->settings.ioHandle); 
    NEXUS_OBJECT_ASSERT(NEXUS_WmDrmPdCore, handle);

    handle->encrType = NEXUS_WmDrmPdEncryptionType_eMax;

    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    allocSettings.heap = handle->settings.heap;
    errCode = NEXUS_Memory_Allocate(dataLength, &allocSettings, (void *)(&handle->bdrm->headers.asf.xencr.data));
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
        return BERR_OUT_OF_SYSTEM_MEMORY;
    }

    BKNI_Memcpy(handle->bdrm->headers.asf.xencr.data, pData, dataLength);
    handle->bdrm->headers.asf.xencr.size = dataLength;
    handle->bdrm->headers.asf.xencr.state = bdrm_hdr_read_ok;
    handle->encrType = NEXUS_WmDrmPdEncryptionType_eWmdrm;

    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    return BERR_SUCCESS;
}

void NEXUS_WmDrmPdCore_GetDefaultDigsignObjectInfo(
    NEXUS_WmDrmPdDigsignObjectInfo *pObject     /* [out] */
    )
{   
    BKNI_Memset(pObject, 0, sizeof(*pObject));
}

NEXUS_Error NEXUS_WmDrmPdCore_SetDigsignObject(
    NEXUS_WmDrmPdCoreHandle handle,
    const NEXUS_WmDrmPdDigsignObjectInfo *pInfo,
    const void *pData,  
    size_t dataLength   
    )
{

    NEXUS_Error errCode = NEXUS_SUCCESS;
    NEXUS_MemoryAllocationSettings allocSettings;

    NEXUS_WMDRMPD_SET_ACTIVE_IO(handle->settings.ioHandle); 
    NEXUS_OBJECT_ASSERT(NEXUS_WmDrmPdCore, handle);

    handle->encrType = NEXUS_WmDrmPdEncryptionType_eMax;

    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    allocSettings.heap = handle->settings.heap;
    errCode = NEXUS_Memory_Allocate(dataLength, &allocSettings, (void *)(&handle->bdrm->headers.asf.sign.data));
    if ( errCode )
    {
        (void)BERR_TRACE(errCode);
        NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
        return BERR_OUT_OF_SYSTEM_MEMORY;
    }

    BDBG_MSG(("SET DIGISIGN HEADER"));
    BKNI_Memcpy(handle->bdrm->headers.asf.sign.data, pData, dataLength);
    handle->bdrm->headers.asf.sign.len = dataLength;
    handle->bdrm->headers.asf.sign.type = pInfo->type;
    handle->bdrm->headers.asf.sign.state = bdrm_hdr_read_ok;
    handle->encrType = NEXUS_WmDrmPdEncryptionType_eWmdrm;

    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    return BERR_SUCCESS;
}


void NEXUS_WmDrmPdCore_GetDefaultPsshBoxInfo(
    NEXUS_WmDrmPdMp4PsshBoxInfo *pObject     /* [out] */
    )
{   
    BKNI_Memset(pObject, 0, sizeof(*pObject));
}

NEXUS_Error NEXUS_WmDrmPdCore_SetPsshBox(
    NEXUS_WmDrmPdCoreHandle handle,
    const NEXUS_WmDrmPdMp4PsshBoxInfo *pInfo,
    const void *pData,
    size_t dataLength
    )
{
    NEXUS_Error errCode = NEXUS_SUCCESS;

    NEXUS_WMDRMPD_SET_ACTIVE_IO(handle->settings.ioHandle); 
    NEXUS_OBJECT_ASSERT(NEXUS_WmDrmPdCore, handle);

    if(bdrm_mp4_set_pssh(handle->bdrm, pData, dataLength, pInfo->systemId.data, 16) != bdrm_err_ok)
    {
        (void)BERR_TRACE(NEXUS_NOT_AVAILABLE);
        errCode = NEXUS_NOT_AVAILABLE;
    }

    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    return errCode;
}

void NEXUS_WmDrmPdCore_GetDefaultProtectionSchemeInfo(
    NEXUS_WmDrmPdMp4ProtectionSchemeInfo *pObject     /* [out] */
    )
{   
    BKNI_Memset(pObject, 0, sizeof(*pObject));
}

NEXUS_Error NEXUS_WmDrmPdCore_SetProtectionSchemeBox(
    NEXUS_WmDrmPdCoreHandle handle,
    const NEXUS_WmDrmPdMp4ProtectionSchemeInfo *pInfo
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    bdrm_mp4_protect_scheme scheme;
    bdrm_encr_state encrState;

    NEXUS_WMDRMPD_SET_ACTIVE_IO(handle->settings.ioHandle); 
    NEXUS_OBJECT_ASSERT(NEXUS_WmDrmPdCore, handle);

    scheme.frma.codingname = pInfo->originalFormat.codingName;
    
    scheme.schm.fullbox.version = pInfo->schemeType.version;
    scheme.schm.fullbox.flags = pInfo->schemeType.flags;
    scheme.schm.scheme_type = pInfo->schemeType.schemeType;
    scheme.schm.scheme_version = pInfo->schemeType.schemeVersion;

    scheme.te.fullbox.version = pInfo->trackEncryption.version;
    scheme.te.fullbox.flags =  pInfo->trackEncryption.flags;
    scheme.te.info.alg_id  = pInfo->trackEncryption.info.algorithm;
    scheme.te.info.iv_size = pInfo->trackEncryption.info.ivSize;
    BKNI_Memcpy(&scheme.te.info.kid, pInfo->trackEncryption.info.keyId, NEXUS_WMDRMPD_KEY_ID_LENGTH);

    if(bdrm_mp4_setProtectionScheme(handle->bdrm, (const bdrm_mp4_protect_scheme *)&scheme, pInfo->trackId) != bdrm_err_ok)
    {
        (void)BERR_TRACE(NEXUS_NOT_AVAILABLE);
        rc =  NEXUS_NOT_AVAILABLE;
    }
    else {
        encrState = bdrm_is_drmencrypted(handle->bdrm);
        if(encrState == bdrm_encr_aes_ctr){
            handle->encrType = NEXUS_WmDrmPdEncryptionType_eAesCtr;
        } else if(encrState == bdrm_encr_aes_cbc){
            handle->encrType = NEXUS_WmDrmPdEncryptionType_eAesCbc;
        }
        else {
            handle->encrType = NEXUS_WmDrmPdEncryptionType_eNone;
        }
    }

    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    return rc;
}

NEXUS_Error NEXUS_WmDrmPdCore_LoadLicense(
    NEXUS_WmDrmPdCoreHandle handle
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS; 
    bdrm_err err = bdrm_err_ok;
    bdrm_license_t license = NULL;

    NEXUS_WMDRMPD_SET_ACTIVE_IO(handle->settings.ioHandle); 
    if(( err = bdrm_load_license(handle->bdrm, &license)) != bdrm_err_ok)
    {
        BDBG_ERR(("bdrm_load_license failed"));
        rc = NEXUS_UNKNOWN; /*Make new nexus error codes. Until then using NEXUS_UNKNOWN*/
    }
    else
    {
        bdrm_decryptor_assign(handle->bdrm->dcrypt, handle->bdrm);
        bdrm_decryptor_set_encr_state(handle->bdrm->dcrypt, (bdrm_encr_state)handle->encrType);
    }

    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    return rc;
}

NEXUS_Error NEXUS_WmDrmPdCore_P_GetCustomLicenseChallenge(
    NEXUS_WmDrmPdCoreHandle handle,
    const void *pCustomData, /* attr{nelem=customDataLength;reserved=128} */
    size_t customDataLength,
    NEXUS_WmDrmPdLicenseChallenge *pChallenge   /* [out] */
    )
{
    bdrm_license_challenge challenge;
    bdrm_err err = bdrm_err_ok;
    
    NEXUS_WMDRMPD_SET_ACTIVE_IO(handle->settings.ioHandle);

    if((err = bdrm_gen_challenge(handle->bdrm, pCustomData, customDataLength, &challenge, NULL)) == bdrm_err_ok)
    {
        BKNI_Memset((void *)(handle->licenseData.pData), 0, DEFAULT_LICENSE_CHALLENGE_LENGTH);
        BKNI_Memcpy((void *)(handle->licenseData.pData), challenge.data, challenge.len);
        handle->licenseData.dataLength = challenge.len;

        if (challenge.url) 
        {
            BKNI_Memset((void *)(handle->licenseData.pUrl), 0, DEFAULT_LICENSE_URL_LENGTH);
            BKNI_Memcpy((void *)(handle->licenseData.pUrl), challenge.url, b_strlen(challenge.url));
        }
        else 
        {
            handle->licenseData.pUrl = NULL;
        }
        
        BKNI_Memcpy((void *)pChallenge, &(handle->licenseData), sizeof(NEXUS_WmDrmPdLicenseChallenge));
        
        /*
        pChallenge = &(handle->licenseData);
        */
        handle->licenseData.pResponseBuffer = pChallenge->pResponseBuffer;

        bdrm_clean_challenge(&challenge);
    }
    else
    {
        BDBG_ERR(("bdrm_gen_challenge failed\n"));
    }

    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_WmDrmPdCore_GetLicenseChallenge(
    NEXUS_WmDrmPdCoreHandle handle,
    NEXUS_WmDrmPdLicenseChallenge *pChallenge   /* [out] */
    )
{
    return NEXUS_WmDrmPdCore_P_GetCustomLicenseChallenge(handle, NULL, 0, pChallenge);
}

NEXUS_Error NEXUS_WmDrmPdCore_GetCustomLicenseChallenge(
    NEXUS_WmDrmPdCoreHandle handle,
    const void *pCustomData, /* attr{nelem=customDataLength;reserved=128} */
    size_t customDataLength,
    NEXUS_WmDrmPdLicenseChallenge *pChallenge   /* [out] */
    )
{
    return NEXUS_WmDrmPdCore_P_GetCustomLicenseChallenge(handle, pCustomData, customDataLength, pChallenge);
}

void NEXUS_WmDrmPdCore_GetDefaultAesCounterInfo(
    NEXUS_WmdrmPdAesCounterInfo *pInfo
    )
{
    BKNI_Memset(pInfo, 0, sizeof(NEXUS_WmdrmPdAesCounterInfo));
}


NEXUS_Error NEXUS_WmDrmPdCore_ProcessBlocksAesCounter(
    NEXUS_WmDrmPdCoreHandle handle,
    const NEXUS_WmdrmPdAesCounterInfo *pInfo,
    const NEXUS_DmaJobBlockSettings *pDmaBlocks,
    unsigned nBlocks
    )
{
    NEXUS_Error rc;
    NEXUS_WMDRMPD_SET_ACTIVE_IO(handle->settings.ioHandle); 

    if(bdrm_decrypt_aes_ctr_samples(handle->bdrm, pInfo->nonce, pInfo->blockCounter, pInfo->byteOffset, pDmaBlocks, nBlocks) != bdrm_err_ok){
        rc = NEXUS_UNKNOWN; /*Make new nexus error codes. Until then using NEXUS_UNKNOWN*/
    }

    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_WmDrmPdCore_LicenseChallengeComplete(
    NEXUS_WmDrmPdCoreHandle handle,
    uint32_t responseLength,        /* Response length in bytes */
    unsigned responseOffset         /* Offset to the start of the response within the data buffer */
    )
{
    bdrm_license_t license = NULL;
    bdrm_license_response response;
    bdrm_err err = bdrm_err_ok;
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_WMDRMPD_SET_ACTIVE_IO(handle->settings.ioHandle); 

    if(responseLength)
    {
        response.data = (unsigned char*)(handle->licenseData.pResponseBuffer);
        response.resp_start = (unsigned char*)(handle->licenseData.pResponseBuffer + responseOffset);
        response.len = responseLength;

        if((err = bdrm_feed_license(handle->bdrm, &response, license)) != bdrm_err_ok)
        {
            BDBG_ERR(("bdrm_feed_license failed=0x%x", err));
            rc = NEXUS_UNKNOWN; /*Make new nexus error codes. Until then using NEXUS_UNKNOWN*/
        }

        else {
            if(( err = bdrm_load_license(handle->bdrm, &license)) != bdrm_err_ok)
            {
                BDBG_ERR(("bdrm_load_license failed=0x%x", err));
                rc = NEXUS_UNKNOWN; /*Make new nexus error codes. Until then using NEXUS_UNKNOWN*/
            }
            else {
                bdrm_decryptor_assign(handle->bdrm->dcrypt, handle->bdrm);
                bdrm_decryptor_set_encr_state(handle->bdrm->dcrypt, (bdrm_encr_state)handle->encrType);
            }
        }
    }
    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    return rc;
}

static NEXUS_Error NEXUS_WmDrmPdCore_P_GetPlayPolicy(
    NEXUS_WmDrmPdPlayPolicy *pDest,
    bdrm_opl_play_t *pSrc
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;

    if(pSrc->i_vop_cEntries > NEXUS_WMDRMPD_MAX_VIDEO_OUTPUT_PROTECTION){
        BDBG_ERR(("Not enough space available in the videoOutputEntries array to copy all the entries. Please increase NEXUS_DRM_MAX_VIDEO_OUTPUT_PROTECTION. The number of entries needed is %d", pSrc->i_vop_cEntries));
        rc = NEXUS_INVALID_PARAMETER;
    }
    else {
        pDest->compressedDigitalVideo = pSrc->i_compressedDigitalVideo;
        pDest->uncompressedDigitalVideo = pSrc->i_uncompressedDigitalVideo;
        pDest->analogVideo = pSrc->i_analogVideo;
        pDest->compressedDigitalAudio = pSrc->i_compressedDigitalAudio;
        pDest->uncompressedDigitalAudio = pSrc->i_uncompressedDigitalAudio;

        pDest->numVideoOutputEntries = (unsigned)pSrc->i_vop_cEntries;
        BKNI_Memcpy(pDest->videoOutputEntries, pSrc->i_vop,  sizeof(NEXUS_WmDrmPdVideoOutProtection) * pDest->numVideoOutputEntries);
    }

    return NEXUS_SUCCESS;
}


static NEXUS_Error NEXUS_WmDrmPdCore_P_GetCopyPolicy(
    NEXUS_WmDrmPdCopyPolicy *pDest,
    bdrm_opl_copy_t *pSrc
    )
{

    if(pSrc->i_includes_cIds > NEXUS_WMDRMPD_MAX_INCLUSION_GUIDS){
        BDBG_ERR(("Not enough space available in the inclusionIds array to copy all the entries. Please increase NEXUS_MAX_INCLUSION_GUIDS. The number of entries needed is %d", pSrc->i_includes_cIds));
        return NEXUS_INVALID_PARAMETER;
    }

    if(pSrc->i_excludes_cIds > NEXUS_WMDRMPD_MAX_EXCLUSION_GUIDS){
        BDBG_ERR(("Not enough space available in the exclusionIds array to copy all the entries. Please increase NEXUS_MAX_EXCLUSION_GUIDS. The number of entries needed is %d", pSrc->i_excludes_cIds));
        return NEXUS_INVALID_PARAMETER;
    }

    pDest->minimumCopyLevel = pSrc->i_minimumCopyLevel;

    pDest->numInclusionIds = pSrc->i_includes_cIds;
    BKNI_Memcpy(pDest->inclusionIds, pSrc->i_includes_rgIds,  sizeof(NEXUS_WmDrmPdGuid) * pDest->numInclusionIds);

    pDest->numExclusionIds = pSrc->i_excludes_cIds;
    BKNI_Memcpy(pDest->exclusionIds, pSrc->i_excludes_rgIds,  sizeof(NEXUS_WmDrmPdGuid) * pDest->numExclusionIds);

    return NEXUS_SUCCESS;
}

static NEXUS_Error NEXUS_WmDrmPdCore_P_GetInclusionListPolicy(
    NEXUS_WmDrmPdInclusionList *pDest,
    bdrm_inclusion_list_t *pSrc
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;

    if(BDRM_MAX_INCLUSION_GUIDS > NEXUS_WMDRMPD_MAX_INCLUSION_GUIDS){
        BDBG_ERR(("Not enough space available in inclusionList array to copy all the entries. Please increase NEXUS_MAX_INCLUSION_GUIDS. The number of entries is %d", BDRM_MAX_INCLUSION_GUIDS));
        rc =  NEXUS_INVALID_PARAMETER;
    }
    else {
        pDest->chainDepth = pSrc->dwChainDepth;

        BKNI_Memcpy(pDest->inclusionList, pSrc->rgInclusionList, sizeof(pSrc->rgInclusionList));
        BKNI_Memcpy(pDest->inclusionListValid, pSrc->rgfInclusionListValid, sizeof(pSrc->rgfInclusionListValid));
    }

    return NEXUS_SUCCESS;
}

NEXUS_Error NEXUS_WmDrmPdCore_GetPolicyStatus(
    NEXUS_WmDrmPdCoreHandle handle,
    NEXUS_WmDrmPdPolicyStatus *pStatus  /* [out] */
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;

    NEXUS_WMDRMPD_SET_ACTIVE_IO(handle->settings.ioHandle); 

    if(handle->nbOfPolicyStacked > 0){
        handle->nbOfPolicyStacked--;

        pStatus->policyType = handle->policy[handle->nbOfPolicyStacked].type;
        switch(pStatus->policyType){
            case PLAY_OPL:
                BDBG_MSG(("PLAY_OPL policy detected\n"));
                rc = NEXUS_WmDrmPdCore_P_GetPlayPolicy(&pStatus->policy.play, &handle->policy[handle->nbOfPolicyStacked].t.play);
                break;
            case COPY_OPL:
                BDBG_MSG(("COPY_OPL policy detected\n"));
                rc = NEXUS_WmDrmPdCore_P_GetCopyPolicy(&pStatus->policy.copy, &handle->policy[handle->nbOfPolicyStacked].t.copy);
                break;
            case INCLUSION_LIST:
                BDBG_MSG(("INCLUSION_LIST policy detected\n"));
                rc = NEXUS_WmDrmPdCore_P_GetInclusionListPolicy(&pStatus->policy.inclusionList, &handle->policy[handle->nbOfPolicyStacked].t.inc_list);
                break;
            case EXTENDED_RESTRICTION_CONDITION:
            case EXTENDED_RESTRICTION_ACTION:
            case EXTENDED_RESTRICTION_QUERY:
            case SECURE_STATE_TOKEN_RESOLVE:
            default:
                BDBG_ERR(("%s - policy_type %d NOT IMPLEMENTED\n", __FUNCTION__, pStatus->policyType));
                rc = NEXUS_NOT_SUPPORTED;
                break;
        }
    }

    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    return rc;
}

void NEXUS_WmDrmPdCore_CleanupLicenseStore(
    NEXUS_WmDrmPdCoreHandle handle
    )
{
    bdrm_err err;
    NEXUS_WMDRMPD_SET_ACTIVE_IO(handle->settings.ioHandle); 

    if((err = bdrm_cleanup_license_store(handle->bdrm)) != bdrm_err_ok)
    {
        BDBG_ERR(("bdrm_cleanup_hds failed=0x%x", err));
    }

    NEXUS_WMDRMPD_CLEAR_ACTIVE_IO();
    return;
}

