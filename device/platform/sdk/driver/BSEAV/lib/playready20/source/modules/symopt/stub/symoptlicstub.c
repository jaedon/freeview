/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmsymopt.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_LIC_CheckSymmOptStatus(
    __in      const  DRM_CONST_STRING   *f_pdstrLicense,
    __out            DRM_SYMMOPT_STATUS *f_eStatus,
    __out_opt        DRM_CONST_STRING   *f_pdstrSymmSigNode,
    __out_opt        DRM_CONST_STRING   *f_pdstrSymmSigNodeData,
    __out_opt        DRM_GUID           *f_pCkVerifiedGUID )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pdstrLicense          != NULL );
    ChkArg( f_eStatus               != NULL );
    ChkArg( f_pdstrSymmSigNode      == NULL );
    ChkArg( f_pdstrSymmSigNodeData  == NULL );
    ChkArg( f_pCkVerifiedGUID       == NULL );

    *f_eStatus = DRM_SYMMOPT_STATUS_NONE;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_LIC_UpdateLicenseWithSymmetricData(
    __inout        DRM_STRING          *f_pdstrLicense,
    __in           DRM_DWORD            f_cchLicenseMax,
    __in           DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __in           DRM_BOOL             f_fVerifiedCK,
    __in_opt const DRM_SLK             *f_pslk )
{
    UNREFERENCED_PARAMETER( f_pdstrLicense );
    UNREFERENCED_PARAMETER( f_cchLicenseMax );
    UNREFERENCED_PARAMETER( f_pcontextLEVL );
    UNREFERENCED_PARAMETER( f_fVerifiedCK );
    UNREFERENCED_PARAMETER( f_pslk );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_LIC_VerifySymmetricSignatureIfPresent(
    IN const DRM_CONST_STRING *f_pdstrLicense,
    IN       DRM_BB_CONTEXT   *f_pcontextBBX,
    IN       DRM_CONST_STRING *f_pdstrLIData,
    IN       DRM_CONST_STRING *f_pdstrSignature,
       OUT   DRM_LONG         *f_plResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(  f_plResult != NULL );

    *f_plResult = 0;

    UNREFERENCED_PARAMETER( f_pdstrLicense );
    UNREFERENCED_PARAMETER( f_pcontextBBX );
    UNREFERENCED_PARAMETER( f_pdstrLIData );
    UNREFERENCED_PARAMETER( f_pdstrSignature );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

