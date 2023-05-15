/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmsymopt.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_BOOL DRM_CALL DRM_SYMOPT_LA_PrepareSLKFromLicense(
    IN           DRM_DST                              *f_pDatastore,
    IN           DRM_LICEVAL_CONTEXT                  *f_pcontextLEVL,
    IN     const DRM_CHAR                             *f_pbResponse,
    IN           DRM_SUBSTRING                        *f_pdasstrLicenseList,
    OUT          DRM_SLKDATA                          *f_pslkdata )
{
    UNREFERENCED_PARAMETER( f_pDatastore );
    UNREFERENCED_PARAMETER( f_pcontextLEVL );
    UNREFERENCED_PARAMETER( f_pbResponse );
    UNREFERENCED_PARAMETER( f_pdasstrLicenseList );
    UNREFERENCED_PARAMETER( f_pslkdata );

    return FALSE;
}

DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_LIC_UpdateLicenseWithSymmetricDataDuringLacq(
    __in_bcount( DRM_MAX_LICENSESIZE )  DRM_BYTE             f_rgbLicenseBuffer[__CB_DECL(DRM_MAX_LICENSESIZE)],
    __inout                             DRM_CONST_STRING    *f_pdstrKID,
    __inout                             DRM_DWORD           *f_pcbVersionLicense,
    __inout                             DRM_BOOL            *f_pfSLKValid,
    __inout                             DRM_BOOL            *f_pfTriedLoadingSLK,
    __in                                DRM_DST             *f_pDatastore,
    __in                                DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __in_bcount( f_cbResponse )         DRM_BYTE            *f_pbResponse,
    __in                                DRM_DWORD            f_cbResponse,
    __inout                             DRM_SUBSTRING       *f_pdasstrLicenseList,
    __inout                             DRM_SLKDATA         *f_pslkdata )
{
    UNREFERENCED_PARAMETER( f_rgbLicenseBuffer );
    UNREFERENCED_PARAMETER( f_pdstrKID );
    UNREFERENCED_PARAMETER( f_pcbVersionLicense );
    UNREFERENCED_PARAMETER( f_pfSLKValid );
    UNREFERENCED_PARAMETER( f_pfTriedLoadingSLK );
    UNREFERENCED_PARAMETER( f_pDatastore );
    UNREFERENCED_PARAMETER( f_pcontextLEVL );
    UNREFERENCED_PARAMETER( f_pbResponse );
    UNREFERENCED_PARAMETER( f_cbResponse );
    UNREFERENCED_PARAMETER( f_pdasstrLicenseList );
    UNREFERENCED_PARAMETER( f_pslkdata );

    /* No update required */
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_LA_UpdateSLK(
    IN                                 DRM_ID               *f_pSLKID,
    __in_bcount( f_cbKeyLength ) const DRM_BYTE             *f_pbSLKKey,
    IN                                 DRM_DWORD             f_cbKeyLength,
    IN                                 DRM_DST              *f_pDatastore,
    IN                                 DRM_LICEVAL_CONTEXT  *f_pcontextLEVL,
    OUT                                DRM_SLKDATA          *f_pSLK )
{
    UNREFERENCED_PARAMETER( f_pSLKID );
    UNREFERENCED_PARAMETER( f_pbSLKKey );
    UNREFERENCED_PARAMETER( f_cbKeyLength );
    UNREFERENCED_PARAMETER( f_pDatastore );
    UNREFERENCED_PARAMETER( f_pcontextLEVL );
    UNREFERENCED_PARAMETER( f_pSLK );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_LA_GetSLK(
    OUT     DRM_SLKDATA           *f_pSLK,
    IN      DRM_DST               *f_pDatastore,
    IN      DRM_LICEVAL_CONTEXT   *f_pcontextLEVL )
{
    UNREFERENCED_PARAMETER( f_pSLK );
    UNREFERENCED_PARAMETER( f_pDatastore );
    UNREFERENCED_PARAMETER( f_pcontextLEVL );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_LA_ExtractSLKDataFromLicenseResponse(
    __in  DRM_LICRESP_CONTEXT_V3 *f_poLicRespContext,
    __in  DRM_ANSI_CONST_STRING  *f_pdastrResponse,
    __in  DRM_SUBSTRING          *f_pdasstrResponse,
    __out DRM_SLKDATA            *f_pSLKData,
    __out DRM_BOOL               *f_pfValidSLK )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pfValidSLK != NULL );

    *f_pfValidSLK = FALSE;

    UNREFERENCED_PARAMETER( f_poLicRespContext );
    UNREFERENCED_PARAMETER( f_pdastrResponse );
    UNREFERENCED_PARAMETER( f_pdasstrResponse );
    UNREFERENCED_PARAMETER( f_pSLKData );

ErrorExit:
    return dr;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

