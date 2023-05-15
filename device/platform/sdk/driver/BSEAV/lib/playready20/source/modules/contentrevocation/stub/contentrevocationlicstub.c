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

DRM_API DRM_RESULT DRM_CALL DRM_CONTENTREVOCATION_LIC_VerifyContentRevocation(
    IN const DRM_CONST_STRING   *pdstrContentRevocation,
    OUT      DRM_CONST_STRING   *pdstrLSPubKey,
    OUT      DRM_DWORD          *pdwSequenceNumber,
    OUT      DRM_CONST_STRING   *pdstrContentOwnerPubKey,
    OUT      DRM_CONST_STRING   *pdstrCondition,
    IN       DRM_CRYPTO_CONTEXT *pcontextCrypto )
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
    IN const DRM_CONST_STRING   *pdstrLicense,
    IN       DRM_DWORD           dwSequenceNumber,
    OUT      DRM_DWORD          *pdwSequenceNumber,
    OUT      DRM_CONST_STRING   *pdstrContentRevocation,
    OUT      DRM_CONST_STRING   *pdstrContentOwnerPubKey,
    OUT      DRM_CONST_STRING   *pdstrCondition,
    IN       DRM_CRYPTO_CONTEXT *pcontextCrypto )
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

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

