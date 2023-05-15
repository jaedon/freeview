/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#ifdef PRDY_ROBUSTNESS
  #include "nexus_keyladder.h"
  #include "nexus_dma.h"
  #include "common_crypto.h"
  #include "bcm_oem.h"
#endif

#include "nexus_memory.h"
#include "bkni.h"
#include "bstd.h"
#include "bdbg.h"

BDBG_MODULE(oem_init);

DRM_VOID * BCM_Oem_MemAlloc(DRM_DWORD f_cbSize );
DRM_VOID BCM_Oem_MemFree(DRM_VOID *f_pv );

DRM_VOID * BCM_Oem_MemHeapAlloc(
    DRM_DWORD f_cbSize 
)
{
    void *pBuf;

    if(f_cbSize == 0)
        return NULL;

    if(NEXUS_Memory_Allocate(f_cbSize, NULL, &pBuf) != 0)
        return NULL;

    return pBuf;
}

DRM_VOID BCM_Oem_MemHeapFree(
    DRM_VOID *f_pv
)
{

    NEXUS_Memory_Free(f_pv);

    return;
}

#if PRDY_ROBUSTNESS
DRM_VOID * BCM_Oem_Create_1st(void)
{
    oem_ctx_t *pOemCtx = NULL;

    CommonCryptoSettings  cryptoSettings;
 
#if (BDBG_DEBUG_BUILD)
    BDBG_SetModuleLevel("oem_init", BDBG_eMsg);
    BDBG_SetModuleLevel("oem_fileio", BDBG_eMsg);
    BDBG_SetModuleLevel("oem_key_region", BDBG_eMsg);
#endif

    pOemCtx = (oem_ctx_t *)BCM_Oem_MemAlloc(sizeof(oem_ctx_t));
    if ( pOemCtx == NULL ){
        goto ErrorExit;
    }

    BKNI_Memset(pOemCtx, 0, sizeof(oem_ctx_t));

    /*/ Initialize the Nexus Common Crypto module*/
    CommonCrypto_GetDefaultSettings(&cryptoSettings);
    pOemCtx->commonCryptoHandle = CommonCrypto_Open(&cryptoSettings);
    if(pOemCtx->commonCryptoHandle == NULL)
    {
        goto ErrorExit;
    }
        
    /* Allocate memory for WMDRM internal data */
    pOemCtx->pWmdrmBuf = (uint8_t *)BCM_Oem_MemHeapAlloc((DRM_DWORD)sizeof(bdvd_wmdrm_pd_data_t));
    if(pOemCtx->pWmdrmBuf == NULL)
    {
        goto ErrorExit;
    }
    pOemCtx->pWmdrmData = (bdvd_wmdrm_pd_data_t *)pOemCtx->pWmdrmBuf;

    pOemCtx->pPrBuf = (uint8_t *)BCM_Oem_MemHeapAlloc((DRM_DWORD)sizeof(bdvd_playready_data_t)); 
    if(pOemCtx->pPrBuf == NULL)
    {
        goto ErrorExit;
    }
    pOemCtx->pPrData = (bdvd_playready_data_t *)pOemCtx->pPrBuf;
	
	return (DRM_VOID *)pOemCtx;
	
ErrorExit:
	if(pOemCtx){
		if(pOemCtx->pPrBuf) BCM_Oem_MemHeapFree(pOemCtx->pPrBuf);
		if(pOemCtx->pWmdrmBuf) BCM_Oem_MemHeapFree(pOemCtx->pWmdrmBuf);
		if(pOemCtx->commonCryptoHandle) CommonCrypto_Close(pOemCtx->commonCryptoHandle);
		BCM_Oem_MemFree(pOemCtx);
	}
	return NULL;
}

DRM_VOID BCM_Oem_Create_2nd(DRM_VOID *f_pOEMContext)
{
#ifndef PLAYREADY_WITH_OTP_PROTECTION_ENABLED
#else
	uint8_t procInForKey3[COMMON_CRYPTO_PROC_SIZE] = {0x2a, 0x34, 0x9a, 0x86, 0x31, 0x2a, 0x33, 0xf5,
															 0x2c, 0x68, 0xa1, 0xbf, 0x36, 0x9a, 0xb2, 0xc1};
	uint8_t procInForKey4[COMMON_CRYPTO_PROC_SIZE] = {0x34, 0x18, 0xd4, 0x2c, 0x1f, 0x7f, 0x00, 0xb2,
															 0x09, 0x29, 0x28, 0x82, 0x23, 0xa6, 0x76, 0x6e};
#endif

    oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

	if(pOemCtx == NULL)
	{
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

    return;

ErrorExit:
    if(pOemCtx){
        if(pOemCtx->pPrBuf) BCM_Oem_MemHeapFree(pOemCtx->pPrBuf);
        if(pOemCtx->pWmdrmBuf) BCM_Oem_MemHeapFree(pOemCtx->pWmdrmBuf);
        if(pOemCtx->commonCryptoHandle) CommonCrypto_Close(pOemCtx->commonCryptoHandle);
        BCM_Oem_MemFree(pOemCtx);
		pOemCtx = NULL;
    }
    return;
}

DRM_VOID BCM_Oem_Destroy(DRM_VOID *f_pOEMContext)
{
    oem_ctx_t *pOemCtx = (oem_ctx_t *)f_pOEMContext;

    if(pOemCtx){
        if(pOemCtx->pPrBuf) BCM_Oem_MemHeapFree( pOemCtx->pPrBuf);
        if(pOemCtx->pWmdrmBuf) BCM_Oem_MemHeapFree(pOemCtx->pWmdrmBuf);
        if(pOemCtx->commonCryptoHandle) CommonCrypto_Close(pOemCtx->commonCryptoHandle);
        BCM_Oem_MemFree(pOemCtx);
    }

    return;
}

#endif

