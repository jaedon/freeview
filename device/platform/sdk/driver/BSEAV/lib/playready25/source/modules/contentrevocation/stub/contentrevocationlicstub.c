/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcontentrevocation.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_CONTENTREVOCATION_LIC_VerifyContentRevocation(
    __in      const DRM_CONST_STRING   *pdstrContentRevocation,
    __out_opt       DRM_CONST_STRING   *pdstrLSPubKey,
    __out_opt       DRM_DWORD          *pdwSequenceNumber,
    __out_opt       DRM_CONST_STRING   *pdstrContentOwnerPubKey,
    __out_opt       DRM_CONST_STRING   *pdstrCondition,
    __in            DRM_CRYPTO_CONTEXT *pcontextCrypto )
{
    UNREFERENCED_PARAMETER( pdstrContentRevocation );
    UNREFERENCED_PARAMETER( pdstrLSPubKey );
    UNREFERENCED_PARAMETER( pdwSequenceNumber );
    UNREFERENCED_PARAMETER( pdstrContentOwnerPubKey );
    UNREFERENCED_PARAMETER( pdstrCondition );
    UNREFERENCED_PARAMETER( pcontextCrypto );
    return DRM_E_NOTIMPL;
}


DRM_API DRM_RESULT DRM_CALL DRM_CONTENTREVOCATION_LIC_GetContentRevocation(
    __in const DRM_CONST_STRING   *pdstrLicense,
    __in       DRM_DWORD           dwSequenceNumber,
    __out      DRM_DWORD          *pdwSequenceNumber,
    __out      DRM_CONST_STRING   *pdstrContentRevocation,
    __out      DRM_CONST_STRING   *pdstrContentOwnerPubKey,
    __out_opt  DRM_CONST_STRING   *pdstrCondition,
    __in       DRM_CRYPTO_CONTEXT *pcontextCrypto )
{
    UNREFERENCED_PARAMETER( pdstrLicense );
    UNREFERENCED_PARAMETER( pdstrLicense );
    UNREFERENCED_PARAMETER( pdwSequenceNumber );
    UNREFERENCED_PARAMETER( pdstrContentRevocation );
    UNREFERENCED_PARAMETER( pdstrContentOwnerPubKey );
    UNREFERENCED_PARAMETER( pdstrCondition );
    UNREFERENCED_PARAMETER( pcontextCrypto );
    return DRM_E_NOTIMPL;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

