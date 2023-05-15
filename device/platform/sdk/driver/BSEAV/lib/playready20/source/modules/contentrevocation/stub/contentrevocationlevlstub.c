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

DRM_API DRM_RESULT DRM_CALL DRM_CONTENTREVOCATION_LEVL_SetOrUpdateContentRevocation(
          DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    const DRM_DST             *f_pDatastore )
{
    UNREFERENCED_PARAMETER( f_pContextLicEval );
    UNREFERENCED_PARAMETER( f_pDatastore );

    /*
    ** No revocation lists to process if feature is unsupported.
    */
    return DRM_SUCCESS;
}

DRM_API DRM_BOOL DRM_CALL DRM_CONTENTREVOCATION_LEVL_IsLicenseRevoked(
    IN const DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    IN const DRM_DST             *f_pDatastore )
{
    UNREFERENCED_PARAMETER( f_pContextLicEval );
    UNREFERENCED_PARAMETER( f_pDatastore );

    /*
    ** License is never revoked if feature is unsupported.
    */
    return FALSE;
}

DRM_API DRM_RESULT DRM_CALL DRM_CONTENTREVOCATION_LEVL_VerifyHeaderSignature(
    __inout   DRM_LICEVAL_CONTEXT          *f_pContextLicEval,
    __in      eDRM_LICEVAL_OPERATION_STATE  f_eOperationState,
    __in      DRM_CONST_STRING             *f_pdstrKID,
    __in      DRM_CONST_STRING             *f_pdstrLIData )
{
    UNREFERENCED_PARAMETER( f_pContextLicEval );
    UNREFERENCED_PARAMETER( f_eOperationState );
    UNREFERENCED_PARAMETER( f_pdstrKID );
    UNREFERENCED_PARAMETER( f_pdstrLIData );

    /*
    ** Header signature verification for the purpose of content revocation
    ** should always succeed if the feature is unsupported.
    */
    return DRM_SUCCESS;
}


PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

