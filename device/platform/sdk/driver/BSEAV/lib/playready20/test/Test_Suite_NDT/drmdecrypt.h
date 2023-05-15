/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMDECRYPT_H__
#define __DRMDECRYPT_H__

#include "PKTestNameSpace.h"

ENTER_PKTEST_NAMESPACE

typedef enum tagWMDRMNET_ENCRYPTION_MODE
{
    WMDRMNET_ENCRYPTION_MODE_ASF = 0,
    WMDRMNET_ENCRYPTION_MODE_MPEG = 1,
    WMDRMNET_ENCRYPTION_MODE_LINK = 2
} WMDRMNET_ENCRYPTION_MODE;

typedef struct tagWMDRMNET_CRYPTO_DATA_ASF
{
    DRM_UINT64      qwSampleID;
    DRM_UINT64      qwOffset;
} WMDRMNET_CRYPTO_DATA_ASF;

typedef struct tagWMDRMNET_CRYPTO_DATA_MPEG
{
    DRM_UINT64      qwBaseCounter;
    DRM_UINT64      qwMinorCounter;
} WMDRMNET_CRYPTO_DATA_MPEG;


/******************************************************************************
** Function :   NDTMGR_Encrypt
**
** Synopsis :   Encrypts the sample data in place using AES in CTR mode.
**              
** Arguments :  [f_pNdtMgrContext]  : Pointer to WMDRM-NDT manager context
**              [f_idSession]       : Session ID
**              [f_EncryptionMode]  : NDT_ENCRYPTION_MODE - ASF, MPEG, LINK. Determines AES CTR Block ID
**              [f_CryptoData]      : IN/OUT pointer to sample data and parameters for specific encryption mode
**                                        Mode                      | Struct
**                                        --------------------------+----------------------
**                                        NDT_ENCRYPTION_MODE_ASF   | NDT_CRYPTO_DATA_ASF
**                                        NDT_ENCRYPTION_MODE_MPEG  | NDT_CRYPTO_DATA_MPEG
**                                        NDT_ENCRYPTION_MODE_LINK  | NDT_CRYPTO_DATA_LINK
** 
** Returns:     DRM_SUCCESS 
** 
******************************************************************************/
DRM_RESULT DRM_API WmdrmNetLinkDecrypt(
    IN      DRM_AES_KEY              *f_pContentEncryptionKey,
    IN      WMDRMNET_ENCRYPTION_MODE  f_EncryptionMode,
    IN      DRM_VOID                 *f_pvCryptoDataDescriptor,
    IN OUT  DRM_BYTE                *f_pbCryptoData,
    IN OUT  DRM_DWORD                *f_pcbCryptoData
    );

EXIT_PKTEST_NAMESPACE

#endif
