/**************************************************************************
///    Copyright (c)2010, Broadcom Corporation
///    All Rights Reserved
///    Confidential Property of Broadcom Corporation
///
/// THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
/// AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
/// EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
///
/// @brief Header file containing hardware specific functions for the OEM module
///
*****************************************************************************/

#ifndef OEM_BRCM_H
#define OEM_BRCM_H

#ifdef __cplusplus
extern "C" {
#endif

ENTER_PK_NAMESPACE_CODE

#ifdef PRDY_ROBUSTNESS

#include "nexus_keyladder.h"
#include "nexus_otpmsp.h"
#include "nexus_dma.h"
#include "common_crypto.h"

#include "drm_metadata.h"
    
#include "berr.h"
#include "bmem.h"

#define GET_UINT32_FROM_BUF(pBuf) \
    (((uint32_t)(((uint8_t*)(pBuf))[0]) << 24) | \
     ((uint32_t)(((uint8_t*)(pBuf))[1]) << 16) | \
     ((uint32_t)(((uint8_t*)(pBuf))[2]) << 8)  | \
     ((uint8_t *)(pBuf))[3])

#define SET_UINT32_IN_BUF(pBuf, val) \
{   *pBuf       = (uint8_t)((val & 0xFF000000) >> 24);  \
    *(pBuf + 1) = (uint8_t)((val & 0xFF0000) >> 16);    \
    *(pBuf + 2) = (uint8_t)((val & 0xFF00) >> 8);       \
    *(pBuf + 3) = (uint8_t)((val & 0xFF));			    \
}


typedef struct drm_key_info_s{

    uint8_t  serial_nb[BDVD_DRM_SERIAL_NUMBER_LENGTH];        /* 16 bytes serial number*/

    union{
        bdvd_wmdrm_pd_data_t  wmdrm;   /* Data section of the wmdrm region in flash Contains the encrypted keys*/
        bdvd_playready_data_t pr;      /* Data section of the playready region in flash. Contains the encrypted keys*/
    }data;

    CommonCryptoKeyLadderSettings info;
}drm_key_info_t;


typedef struct oem_ctx_s{
    CommonCryptoHandle commonCryptoHandle;   /* Nexus common crypto handle */

    drm_key_info_t wmdrm_ki;             /* WMDRM keys loaded from flash */
    bool wmdrm_keys_valid;               /* Flag used to monitor if the wmdrm keys stored
                                            in wmdrm_ki are valid*/
    bdvd_wmdrm_pd_data_t *pWmdrmData;    
    uint8_t *pWmdrmBuf;                  /* Buffer used to decrypt WMDRM keys at run time. */
    uint8_t *pDevTemplate;               /* Device Certificate template read from the Playready bin file*/
    uint32_t devTemplateSize;            /* Size of the devuce Certificate in bytes */

    drm_key_info_t pr_ki;                /* Playready keys loaded from flash */
    bool pr_keys_valid;                  /* Flag used to monitor if the wmdrm keys stored
                                            in play_keys_ki are valid*/  
    bdvd_playready_data_t *pPrData;
    uint8_t *pPrBuf;                     /* Buffer used to decrypt PDRY keys at run time. */
    uint8_t *pZGroupCert;                /* ZGroup Certificate read from the Playready bin file*/
    uint32_t zgroupCertSize;             /* Size of the zgroup certificate in bytes */

    CommonCryptoKeySrc keySrc;           /* Key source for encryption/decryption of data saved in flash based
                                            on the PLAYREADY_WITH_OTP_PROTECTION_ENABLED compilation flag.
                                            Can either be CommonCrypto_eCustKey
                                            or CommonCrypto_eOtpKey,*/
    
    CommonCryptoKeyLadderSettings  optKeySettings; /* Key ladder setting used to protect files when keySrc = CommonCrypto_eOtpKey*/

    NEXUS_ReadOtpIO    otp;
    bool revoked;
}oem_ctx_t;

DRM_RESULT _ReadWmdrmKeyData(__in drm_key_info_t *pKi);

DRM_RESULT _ReadPrdyBinFile(oem_ctx_t *pOemCtx);

DRM_RESULT _Crypto_AesEcb(
    __in     CommonCryptoHandle commonCryptoHandle,
    __in     CommonCryptoKeySrc keySrc,
    __in     CommonCryptoKeyLadderSettings *pKeyLadderSettings,
    __inout  uint8_t *pBuf,
    __inout  uint32_t buf_size,
    __in     DRM_BYTE *pEncKey,  /* Set to NULL when keySrc = CommonCrypto_eOtpKey*/
    __in     NEXUS_SecurityOperation opType
);

DRM_API DRM_VOID * Oem_Create(NEXUS_ReadOtpIO *pOtp);
DRM_VOID Oem_Destroy(__in_opt DRM_VOID *f_pOEMContext);

#endif

DRM_API DRM_VOID * DRM_CALL Oem_MemHeapAlloc(__in DRM_DWORD f_cbSize);
DRM_API DRM_VOID DRM_CALL Oem_MemHeapFree(__in DRM_VOID *f_pv);

EXIT_PK_NAMESPACE_CODE


#ifdef __cplusplus
}
#endif

#endif /*OEM_BRCM_H*/
