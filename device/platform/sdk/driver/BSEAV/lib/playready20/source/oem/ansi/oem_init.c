/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <drmcommon.h>
#include <oemcommon.h>
#include <drmtypes.h>
#include <drmutilities.h>

ENTER_PK_NAMESPACE_CODE;

#ifdef PRDY_ROBUSTNESS
  #include "nexus_keyladder.h"
  #include "nexus_dma.h"
  #include "common_crypto.h"
#endif

#include "./inc/oem_brcm.h"

#include "bstd.h"
#include "bdbg.h"
BDBG_MODULE(oem_init);

#if PRDY_ROBUSTNESS


DRM_API DRM_VOID * Oem_Create()
{
    DRM_RESULT  dr;
    oem_ctx_t *pOemCtx = NULL;

    CommonCryptoSettings  cryptoSettings;

    uint8_t procInForKey3[COMMON_CRYPTO_PROC_SIZE] = {0x2a, 0x34, 0x9a, 0x86, 0x31, 0x2a, 0x33, 0xf5,
                                                            0x2c, 0x68, 0xa1, 0xbf, 0x36, 0x9a, 0xb2, 0xc1};
    uint8_t procInForKey4[COMMON_CRYPTO_PROC_SIZE] = {0x34, 0x18, 0xd4, 0x2c, 0x1f, 0x7f, 0x00, 0xb2,
                                                            0x09, 0x29, 0x28, 0x82, 0x23, 0xa6, 0x76, 0x6e};

#if (BDBG_DEBUG_BUILD)
    BDBG_SetModuleLevel("oem_init", BDBG_eMsg);
    BDBG_SetModuleLevel("oem_fileio", BDBG_eMsg);
    BDBG_SetModuleLevel("oem_key_region", BDBG_eMsg);
#endif

    pOemCtx = Oem_MemAlloc(sizeof(oem_ctx_t));
    if ( pOemCtx == NULL ){
        goto ErrorExit;
    }

    BKNI_Memset(pOemCtx, 0, sizeof(oem_ctx_t));

    // Initialize the Nexus Common Crypto module
    CommonCrypto_GetDefaultSettings(&cryptoSettings);
    pOemCtx->commonCryptoHandle = CommonCrypto_Open(&cryptoSettings);
    if(pOemCtx->commonCryptoHandle == NULL)
    {
        goto ErrorExit;
    }
        
    // Allocate memory for WMDRM internal data
    pOemCtx->pWmdrmBuf = Oem_MemHeapAlloc(SIZEOF(bdvd_wmdrm_pd_data_t));
    if(pOemCtx->pWmdrmBuf == NULL)
    {
        goto ErrorExit;
    }
    pOemCtx->pWmdrmData = (bdvd_wmdrm_pd_data_t *)pOemCtx->pWmdrmBuf;

    pOemCtx->pPrBuf =Oem_MemHeapAlloc(SIZEOF(bdvd_playready_data_t)); 
    if(pOemCtx->pPrBuf == NULL)
    {
        goto ErrorExit;
    }
    pOemCtx->pPrData = (bdvd_playready_data_t *)pOemCtx->pPrBuf;

    // Read Bin file from file system.
    dr = _ReadBinFile(pOemCtx);
    if ( DRM_FAILED(dr) ){
        BDBG_ERR(("%s - _ReadBinFile() failed. Please make sure the pr_bin_filename variable in drm_playready.inc point to the right file\n", __FUNCTION__));
        goto ErrorExit;
    }
        

    if(pOemCtx->pr_keys_valid == false
        && pOemCtx->wmdrm_keys_valid == false)
    {
        goto ErrorExit;
    }

#ifndef PLAYREADY_WITH_OTP_PROTECTION_ENABLED
    pOemCtx->keySrc = CommonCrypto_eCustKey;
#else
    pOemCtx->keySrc = CommonCrypto_eOtpKey;
    BKNI_Memcpy(pOemCtx->optKeySettings.procInForKey3, &procInForKey3, COMMON_CRYPTO_PROC_SIZE);
    BKNI_Memcpy(pOemCtx->optKeySettings.procInForKey4, &procInForKey4, COMMON_CRYPTO_PROC_SIZE);
#endif

    dr = Oem_Device_SaveRevocationInfo((DRM_VOID *)pOemCtx);
    if ( DRM_FAILED(dr) ){
        BDBG_ERR(("%s - Oem_Device_SaveRevocationInfo() failed. Please make sure write permission to the folder pointed to by the pr_hds_dir variable in drm_playready.inc\n", __FUNCTION__));
        goto ErrorExit;
    }

    return (DRM_VOID *)pOemCtx;

ErrorExit:
    if(pOemCtx){
        if(pOemCtx->pPrBuf) Oem_MemHeapFree(pOemCtx->pPrBuf);
        if(pOemCtx->pWmdrmBuf) Oem_MemHeapFree(pOemCtx->pWmdrmBuf);
        if(pOemCtx->commonCryptoHandle) CommonCrypto_Close(pOemCtx->commonCryptoHandle);
        Oem_MemFree(pOemCtx);
    }
    return NULL;
}

DRM_VOID Oem_Destroy(__in_opt DRM_VOID *f_pOEMContext)
{
    oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

    if(pOemCtx){
        if(pOemCtx->pPrBuf) Oem_MemHeapFree( pOemCtx->pPrBuf);
        if(pOemCtx->pWmdrmBuf) Oem_MemHeapFree(pOemCtx->pWmdrmBuf);
        if(pOemCtx->commonCryptoHandle) CommonCrypto_Close(pOemCtx->commonCryptoHandle);
        Oem_MemFree(pOemCtx);
    }

    return;
}

#endif

DRM_API DRM_VOID * DRM_CALL Oem_MemHeapAlloc(
    __in DRM_DWORD f_cbSize 
)
{
    void *pBuf;

    if(f_cbSize == 0)
        return NULL;

    if(NEXUS_Memory_Allocate(f_cbSize, NULL, &pBuf) != 0)
        return NULL;

    return pBuf;
}

DRM_API DRM_VOID DRM_CALL Oem_MemHeapFree(
    __in DRM_VOID *f_pv
)
{

    NEXUS_Memory_Free(f_pv);

    return;
}

EXIT_PK_NAMESPACE_CODE;
