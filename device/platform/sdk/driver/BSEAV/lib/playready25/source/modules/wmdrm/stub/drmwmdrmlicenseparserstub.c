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
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetAttribute(
    __in         const DRM_CONST_STRING        *pdstrLicense,
    __in_opt     const DRM_CONST_STRING        *pdstrAttrName,
    __in               eDRM_LICENSE_ATTRIBUTES  eAttribute,
    __inout_opt        DRM_CONST_STRING        *pdstrLIData,
    __out              DRM_CONST_STRING        *pdstrValue,
    __in               DRM_WCHAR                chXPathSeparator )
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
    __in        const DRM_CONST_STRING *pdstrLicense,
    __in        const DRM_CONST_STRING *pdstrEvent,
    __in_opt    const DRM_CONST_STRING *pdstrTypeValue,
    __inout_opt       DRM_CONST_STRING *pdstrLIData,
    __out_opt         DRM_CONST_STRING *pdstrCondition,
    __out_opt         DRM_CONST_STRING *pdstrAction,
    __out_opt         DRM_CONST_STRING *pdstrRestriction )
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
    __in  const DRM_CONST_STRING    *pdstrLicense,
    __in        DRM_BOOL             fCheckExpiry,
    __in        DRM_LICEVAL_CONTEXT *pcontextLEVL,
    __out       DRM_LONG            *plResult )
{
    UNREFERENCED_PARAMETER( pdstrLicense );
    UNREFERENCED_PARAMETER( fCheckExpiry );
    UNREFERENCED_PARAMETER( pcontextLEVL );
    UNREFERENCED_PARAMETER( plResult     );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetLSPubKey(
    __in const DRM_CONST_STRING   *pdstrLicense,
    __in       PUBKEY             *pbPubKey,
    __in       DRM_CRYPTO_CONTEXT *pcontextCrypto )
{
    UNREFERENCED_PARAMETER( pdstrLicense   );
    UNREFERENCED_PARAMETER( pbPubKey       );
    UNREFERENCED_PARAMETER( pcontextCrypto );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetEnablingBits(
    __in                                     const DRM_CONST_STRING   *pdstrLicense,
    __in                                           DRM_DWORD           dwIndex,
    __out                                          DRM_DWORD          *pdwAlgorithm,
    __out                                          PUBKEY             *pPubKey,
    __out_bcount_opt( *pdValueLen )                DRM_BYTE           *pbValue,
    __out                                          DRM_DWORD          *pdValueLen,
    __out                                          PUBKEY             *pVerPubKey,
    __out_bcount( DRM_ECC160_SIGNATURE_LEN )       DRM_BYTE           *pbSignature,
    __out_bcount_opt( CHECKSUM_LENGTH )            DRM_BYTE            rgbChainedChecksum[CHECKSUM_LENGTH],
    __in                                           DRM_CRYPTO_CONTEXT *pcontextCrypto )
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
    __in const DRM_CONST_STRING *pdstrLicense,
    __in       DRM_BB_CONTEXT   *pcontextBBX,
    __out      DRM_LONG         *plResult )
{
    UNREFERENCED_PARAMETER( pdstrLicense );
    UNREFERENCED_PARAMETER( pcontextBBX  );
    UNREFERENCED_PARAMETER( plResult     );
    return DRM_E_NOTIMPL;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;
