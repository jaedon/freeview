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

#ifndef TRUE
    #define TRUE  1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef NULL
    #define NULL  0
#endif

typedef int32_t                 DRM_BOOL;               /* 4 bytes             2 bytes */
typedef int8_t 			        DRM_BYTE;               /* 1 byte              1 byte */
typedef unsigned long           DRM_DWORD;              /* 4 bytes             2 bytes */
typedef int32_t                 DRM_RESULT;               /* 4 bytes             2 bytes */
typedef void      				DRM_VOID;
typedef          long long int 	DRM_INT64;
typedef unsigned long long int 	DRM_UINT64;

#define DRM_I64LITERAL(a,b) (DRM_INT64)(((DRM_INT64)a)<<32|b)
#define DRM_UI64LITERAL(a,b)(DRM_UINT64)(((DRM_UINT64)a)<<32|b)

#define SIXTEEN_BIT_ADDRESSING 0

#ifdef PRDY_ROBUSTNESS
#include "nexus_keyladder.h"
#include "nexus_dma.h"
#include "common_crypto.h"

#include "drm_metadata.h"
    
#include "berr.h"

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

#if !defined( min )
    #define min( a, b ) ( (a) < (b) ? (a) : (b) )
#endif

#define DRM_LIKELY( expr ) __builtin_expect(!!(expr),1)
#define DRM_UNLIKELY( expr ) __builtin_expect(!!(expr),0)

#define DRM_SUCCESS                      ((DRM_RESULT)0x00000000L)
#define DRM_E_PRIVKEYREADERROR           ((DRM_RESULT)0x8004C047L)
#define DRM_E_CRYPTO_FAILED              ((DRM_RESULT)0x8004C054L)
#define DRM_E_INVALIDARG                 ((DRM_RESULT)0x80070057L)
#define DRM_E_BUFFERTOOSMALL             ((DRM_RESULT)0x8007007AL)

#define DRM_FAILED(Status) ((DRM_RESULT)(Status)<0)

#define ChkDR(expr) do {            \
            dr = (expr);            \
            if( DRM_FAILED( dr ) )  \
            {                       \
                goto ErrorExit;     \
            }                       \
        } while(FALSE)

#define ChkArg(expr) do {                                                                       \
            if( DRM_UNLIKELY( !(expr) ) )                                                       \
            {                                                                                   \
                dr = DRM_E_INVALIDARG;                                                          \
                goto ErrorExit;                                                                 \
            }                                                                                   \
        } while(FALSE)

#define ChkBOOL(fExpr,err) do {         \
						if( !(fExpr) )				\
						{							\
							dr = (err); 			\
							goto ErrorExit; 		\
						}							\
					} while(FALSE)


typedef enum _DRM_DEVICE_ID_TYPE
{
	DRM_DIT_WMDRMPD_ID = 1,
	DRM_DIT_PLAYREADY_ID,
} DRM_DEVICE_ID_TYPE;

typedef enum _DRM_DEVICE_KEY_TYPE
{
    DRM_DKT_INVALID=0,
    DRM_DKT_WMDRMPD_GROUP = 1,
    DRM_DKT_WMDRMPD_FALLBACK,
    DRM_DKT_PLAYREADY_DEVICE_SIGN,
    DRM_DKT_PLAYREADY_DEVICE_ENCRYPT,
    DRM_DKT_PLAYREADY_MODEL,
    DRM_DKT_PLAYREADY_NDR,
    DRM_DKT_PLAYREADY_NDR_MODEL,
    DRM_DKT_WMDRMPD_DEVICE,
    DRM_DKT_SECURE_STORE
} DRM_DEVICE_KEY_TYPE;

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

    CommonCryptoKeySrc keySrc;     /* Key source for encryption/decryption of data saved in flash based
                                            on the PLAYREADY_WITH_OTP_PROTECTION_ENABLED compilation flag.
                                            Can either be CommonCrypto_eCustKey
                                            or CommonCrypto_eOtpKey,*/
    
    CommonCryptoKeyLadderSettings  optKeySettings; /* Key ladder setting used to protect files when keySrc = CommonCrypto_eOtpKey*/

    bool revoked;
}oem_ctx_t;

DRM_RESULT _ReadWmdrmKeyData(drm_key_info_t *pKi);

DRM_RESULT _ReadPrdyBinFile(oem_ctx_t *pOemCtx);

DRM_RESULT _Crypto_AesEcb(
    CommonCryptoHandle commonCryptoHandle,
    CommonCryptoKeySrc keySrc,
    CommonCryptoKeyLadderSettings *pKeyLadderSettings,
    uint8_t *pBuf,
    uint32_t buf_size,
    DRM_BYTE *pEncKey,  /* Set to NULL when keySrc = CommonCrypto_eOtpKey*/
    NEXUS_SecurityOperation opType
);

DRM_VOID * Oem_Create(void);
DRM_VOID Oem_Destroy( DRM_VOID *f_pOEMContext);

#endif

 DRM_VOID *  Oem_MemHeapAlloc( DRM_DWORD f_cbSize);
 DRM_VOID  Oem_MemHeapFree( DRM_VOID *f_pv);

#ifdef __cplusplus
}
#endif

#endif /*OEM_BRCM_H*/
