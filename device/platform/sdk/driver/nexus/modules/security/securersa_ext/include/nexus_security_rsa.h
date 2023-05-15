/***************************************************************************
 *     (c)2007-2009 Broadcom Corporation
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
 * $brcm_Workfile: nexus_security_rsa.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 8/24/09 4:48p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/security/7400/securersa_ext/include/nexus_security_rsa.h $
 * 
 * 1   8/24/09 4:48p erickson
 * SW7405-2870: add SecurityRsa interface
 *
 *
 *
 **************************************************************************/
#ifndef NEXUS_SECURITYRSA_H__
#define NEXUS_SECURITYRSA_H__

#include "nexus_security_datatypes.h"
#include "nexus_security.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
This enum defines the RSA key size

Description:
See Also:
NEXUS_SecurityRsa_DecryptRSAKey3
**/
typedef enum NEXUS_SecurityRsaKeySize /* DME: rename to Rsa instead of RSA throughout. */
{
    NEXUS_SecurityRsaKeySize_e1024 = 0,
    NEXUS_SecurityRsaKeySize_e2048,
    NEXUS_SecurityRsaKeySize_e1280,
    NEXUS_SecurityRsaKeySize_eMax
} NEXUS_SecurityRsaKeySize;

/**
Summary:
This enum defines the destination of key3: CA or CP

Description:
See Also:
NEXUS_SecurityRsa_DecryptRSAKey3
**/
typedef enum NEXUS_SecurityRsaKeyType
{
    NEXUS_SecurityRsaKey_eSTB_K_CA = 2,
    NEXUS_SecurityRsaKey_eSTB_K_CP = 3
}NEXUS_SecurityRsaKeyType;

/**
Summary:
This type defines the data structure for rsa initialization

Description:
See Also:
NEXUS_SecurityRsa_PerformRSAInit
**/
typedef struct NEXUS_SecurityRsaInitSettings {
    uint32_t        hashStorageDataSect;
    uint32_t        otpKeyDataSect;
    uint32_t        rsaDataSize;
    uint8_t            *rsaDataAddress; /* attr{memory=cached} */
} NEXUS_SecurityRsaInitSettings;

/**
Summary:
This struct defines the rsa key3 type, size, signature and encrypted value

Description:
See Also:
NEXUS_SecurityRsa_DecryptRSAKey3
**/
typedef struct NEXUS_SecurityRsaKey3Settings {
    NEXUS_SecurityRsaKeyType    rsaKeyType;
    NEXUS_SecurityRsaKeySize    rsaKeySize;
    uint8_t                *encryptedKey3Address; /* attr{memory=cached} */
    uint8_t                *publicCACPKeyAddress; /* attr{memory=cached} */
    uint8_t                *publicVerifyKeyAddress; /* attr{memory=cached} */
    uint8_t                *signatureAddress; /* attr{memory=cached} */
    bool                verifySignature;
} NEXUS_SecurityRsaKey3Settings;

/**
Summary:
This struct defines the rsa verification data type, size, signature

Description:
See Also:
NEXUS_SecurityRsa_Verify
**/
typedef struct NEXUS_SecurityRsaVerificationSettings {
    NEXUS_SecurityRsaKeySize    rsaKeySize;
    uint32_t                dataSize;
    uint8_t                    *dataAddress; /* attr{memory=cached} */
    uint8_t                *publicKeyAddress; /* attr{memory=cached} */
    uint8_t                *signatureAddress; /* attr{memory=cached} */
} NEXUS_SecurityRsaVerificationSettings;

/**
Summary:
This struct defines the rsa verification data type, size, signature

Description:
See Also:
NEXUS_SecurityRsa_Sign
**/
typedef struct NEXUS_SecurityRsaSignSettings {
    NEXUS_SecurityRsaKeySize    rsaKeySize;
    uint32_t                dataSize;
    uint8_t                    *dataAddress; /* attr{memory=cached} */
    uint8_t                *publicKeyAddress; /* attr{memory=cached} */
} NEXUS_SecurityRsaSignSettings;

/**
Summary:
Get default NEXUS_SecurityRsa_GetDefaultRSAInitSettings
**/
void NEXUS_SecurityRsa_GetDefaultInitSettings(
    NEXUS_SecurityRsaInitSettings *pRSAInitSetting /* [out] */
    );


/**
Summary:
Perform RSA Initialization
**/
NEXUS_Error NEXUS_SecurityRsa_PerformInit(
    const NEXUS_SecurityRsaInitSettings *pSecureRSAInit
    );

/**
Summary:
Get default NEXUS_SecurityRsa_GetDefaultRSAInitSettings
**/
void NEXUS_SecurityRsa_GetDefaultKey3Settings(
    NEXUS_SecurityRsaKey3Settings *pRSAKey3Setting /* [out] */
    );

/**
Summary:
Verify, decrypt and load key3 using RSA
**/
NEXUS_Error NEXUS_SecurityRsa_DecryptKey3(
    const NEXUS_SecurityRsaKey3Settings *pSecureRSAKey3
    );

/**
Summary:
Get default NEXUS_SecurityRsa_GetDefaultRSAVerifySettings
**/
void NEXUS_SecurityRsa_GetDefaultVerificationSettings(
    NEXUS_SecurityRsaVerificationSettings *pSecureRSAVerification /* [out] */
    );

/**
Summary:
Verify the signature of data
**/
NEXUS_Error NEXUS_SecurityRsa_Verify(
    const NEXUS_SecurityRsaVerificationSettings *pSecureRSAVerification
    );

/**
Summary:
Get default NEXUS_SecurityRsa_GetDefaultSignSettings
**/
void NEXUS_SecurityRsa_GetDefaultSignSettings(
    NEXUS_SecurityRsaSignSettings *pSecureRSASign /* [out] */
    );

/**
Summary:
Get the signature of secure RSA
**/
NEXUS_Error NEXUS_SecurityRsa_Sign(
    const NEXUS_SecurityRsaSignSettings *pSecureRSASign,
    uint8_t *pOutputBuf,  /* [out] attr{nelem=outputBufferSize} */
    unsigned outputBufferSize
    );

#ifdef __cplusplus
}
#endif

#endif
