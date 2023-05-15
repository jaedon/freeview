/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmwmdrm.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetAttribute(
    IN const DRM_CONST_STRING        *pdstrLicense,
    IN const DRM_CONST_STRING        *pdstrAttrName,
    IN       eDRM_LICENSE_ATTRIBUTES  eAttribute,
    IN  OUT  DRM_CONST_STRING        *pdstrLIData,
        OUT  DRM_CONST_STRING        *pdstrValue,
    IN       DRM_WCHAR                chXPathSeparator )
{
    UNREFERENCED_PARAMETER( pdstrLicense     );
    UNREFERENCED_PARAMETER( pdstrAttrName    );
    UNREFERENCED_PARAMETER( eAttribute       );
    UNREFERENCED_PARAMETER( pdstrLIData      );
    UNREFERENCED_PARAMETER( pdstrValue       );
    UNREFERENCED_PARAMETER( chXPathSeparator );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetEvent(
    IN const DRM_CONST_STRING *pdstrLicense,
    IN const DRM_CONST_STRING *pdstrEvent,
    IN const DRM_CONST_STRING *pdstrTypeValue,
    IN  OUT  DRM_CONST_STRING *pdstrLIData,
    OUT      DRM_CONST_STRING *pdstrCondition,
    OUT      DRM_CONST_STRING *pdstrAction,
    OUT      DRM_CONST_STRING *pdstrRestriction )
{
    UNREFERENCED_PARAMETER( pdstrLicense     );
    UNREFERENCED_PARAMETER( pdstrEvent       );
    UNREFERENCED_PARAMETER( pdstrTypeValue   );
    UNREFERENCED_PARAMETER( pdstrLIData      );
    UNREFERENCED_PARAMETER( pdstrCondition   );
    UNREFERENCED_PARAMETER( pdstrAction      );
    UNREFERENCED_PARAMETER( pdstrRestriction );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_VerifyCertChain(
    IN const DRM_CONST_STRING    *pdstrLicense,
    IN       DRM_BOOL             fCheckExpiry,
    IN       DRM_LICEVAL_CONTEXT *pcontextLEVL,
    OUT      DRM_LONG            *plResult )
{
    UNREFERENCED_PARAMETER( pdstrLicense );
    UNREFERENCED_PARAMETER( fCheckExpiry );
    UNREFERENCED_PARAMETER( pcontextLEVL );
    UNREFERENCED_PARAMETER( plResult     );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetLSPubKey(
    const DRM_CONST_STRING   *pdstrLicense,
          PUBKEY             *pbPubKey,
          DRM_CRYPTO_CONTEXT *pcontextCrypto )
{
    UNREFERENCED_PARAMETER( pdstrLicense   );
    UNREFERENCED_PARAMETER( pbPubKey       );
    UNREFERENCED_PARAMETER( pcontextCrypto );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetEnablingBits(
    IN const DRM_CONST_STRING   *pdstrLicense,
    IN       DRM_DWORD           dwIndex,
    OUT      DRM_DWORD          *pdwAlgorithm,
    OUT      PUBKEY             *pPubKey,
    __out_bcount_opt( *pdValueLen ) DRM_BYTE *pbValue,
    OUT      DRM_DWORD          *pdValueLen,
    OUT      PUBKEY             *pVerPubKey,
    __out_bcount( DRM_ECC160_SIGNATURE_LEN ) DRM_BYTE *pbSignature,
    __out_bcount_opt( CHECKSUM_LENGTH ) DRM_BYTE rgbChainedChecksum[__CB_DECL(CHECKSUM_LENGTH)],
    IN       DRM_CRYPTO_CONTEXT *pcontextCrypto )
{
    UNREFERENCED_PARAMETER( pdstrLicense       );
    UNREFERENCED_PARAMETER( dwIndex            );
    UNREFERENCED_PARAMETER( pdwAlgorithm       );
    UNREFERENCED_PARAMETER( pPubKey            );
    UNREFERENCED_PARAMETER( pbValue            );
    UNREFERENCED_PARAMETER( pdValueLen         );
    UNREFERENCED_PARAMETER( pVerPubKey         );
    UNREFERENCED_PARAMETER( pbSignature        );
    UNREFERENCED_PARAMETER( rgbChainedChecksum );
    UNREFERENCED_PARAMETER( pcontextCrypto     );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_LIC_VerifySignature(
    IN const DRM_CONST_STRING *pdstrLicense,
    IN       DRM_BB_CONTEXT   *pcontextBBX,
       OUT   DRM_LONG         *plResult )
{
    UNREFERENCED_PARAMETER( pdstrLicense );
    UNREFERENCED_PARAMETER( pcontextBBX  );
    UNREFERENCED_PARAMETER( plResult     );
    return DRM_E_NOTIMPL;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;
