/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmmeterapi.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_InitEnum(
    __in   DRM_APP_CONTEXT     *f_pcontextMGR,
    __out  DRM_METERCERT_ENUM  *f_pcontextMeterCertEnum )
{
    UNREFERENCED_PARAMETER( f_pcontextMGR );
    UNREFERENCED_PARAMETER( f_pcontextMeterCertEnum );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_EnumNext(
    __in                                DRM_METERCERT_ENUM  *f_pcontextMeterCertEnum,
    __inout                             DRM_MID             *f_pmid,
    __out_ecount_opt( *f_pcchLAINFO )   DRM_WCHAR           *f_pwszLAINFO,
    __out_opt                           DRM_DWORD           *f_pcchLAINFO,
    __out_opt                           DRM_BOOL            *f_pfIsV2,
    __out_bcount_opt( *f_pcbMeterCert ) DRM_BYTE            *f_pbMeterCert,
    __out_opt                           DRM_DWORD           *f_pcbMeterCert )
{
    UNREFERENCED_PARAMETER( f_pcontextMeterCertEnum );
    UNREFERENCED_PARAMETER( f_pmid );
    UNREFERENCED_PARAMETER( f_pwszLAINFO );
    UNREFERENCED_PARAMETER( f_pcchLAINFO );
    UNREFERENCED_PARAMETER( f_pfIsV2 );
    UNREFERENCED_PARAMETER( f_pbMeterCert );
    UNREFERENCED_PARAMETER( f_pcbMeterCert );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_Update(
    __in                         DRM_APP_CONTEXT   *f_pcontextMGR,
    __in_bcount( f_cbMeterCert ) DRM_BYTE          *f_pbMeterCert,
    __in                         DRM_DWORD          f_cbMeterCert,
    __out_opt                    DRM_MID           *f_pmid )
{
    UNREFERENCED_PARAMETER( f_pcontextMGR );
    UNREFERENCED_PARAMETER( f_pbMeterCert );
    UNREFERENCED_PARAMETER( f_cbMeterCert );
    UNREFERENCED_PARAMETER( f_pmid );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_Invalidate(
    __in       DRM_APP_CONTEXT  *f_pcontextMGR,
    __in const DRM_MID          *f_pmid )
{
    UNREFERENCED_PARAMETER( f_pcontextMGR );
    UNREFERENCED_PARAMETER( f_pmid );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_Delete(
    __in       DRM_APP_CONTEXT  *f_pcontextMGR,
    __in const DRM_MID          *f_pmid )
{
    UNREFERENCED_PARAMETER( f_pcontextMGR );
    UNREFERENCED_PARAMETER( f_pmid );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Drm_Metering_GenerateChallenge(
    __in                                DRM_APP_CONTEXT   *f_poAppContext,
    __in_bcount( f_cbMeterCert ) const  DRM_BYTE          *f_pbMeterCert,
    __in                                DRM_DWORD          f_cbMeterCert,
    __in_ecount_opt( f_cchCustomData )  DRM_CHAR          *f_pchCustomData,
    __in_opt                            DRM_DWORD          f_cchCustomData,
    __out_ecount_opt( *f_pcchURL )      DRM_CHAR          *f_pchURL,
    __inout_opt                         DRM_DWORD         *f_pcchURL,
    __out_bcount_opt( *f_pcbChallenge ) DRM_BYTE          *f_pbChallenge,
    __inout                             DRM_DWORD         *f_pcbChallenge )
{
    UNREFERENCED_PARAMETER( f_poAppContext );
    UNREFERENCED_PARAMETER( f_pbMeterCert );
    UNREFERENCED_PARAMETER( f_cbMeterCert );
    UNREFERENCED_PARAMETER( f_pchCustomData );
    UNREFERENCED_PARAMETER( f_cchCustomData );
    UNREFERENCED_PARAMETER( f_pchURL );
    UNREFERENCED_PARAMETER( f_pcchURL );
    UNREFERENCED_PARAMETER( f_pbChallenge );
    UNREFERENCED_PARAMETER( f_pcbChallenge );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Drm_Metering_ProcessResponse(
    __in                        DRM_APP_CONTEXT  *f_poAppContext,
    __in_bcount( f_cbResponse ) DRM_BYTE         *f_pbResponse,
    __in                        DRM_DWORD         f_cbResponse,
    __out                       DRM_DWORD        *f_pfFlags )
{
    UNREFERENCED_PARAMETER( f_poAppContext );
    UNREFERENCED_PARAMETER( f_pbResponse );
    UNREFERENCED_PARAMETER( f_cbResponse );
    UNREFERENCED_PARAMETER( f_pfFlags );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_METERING_UpdateData(
    __in                        DRM_APP_CONTEXT   *f_poAppContext,
    __in                  const DRM_CONST_STRING **f_ppdstrActions,
    __in                        DRM_DWORD          f_cActions )
{
    UNREFERENCED_PARAMETER( f_poAppContext );
    UNREFERENCED_PARAMETER( f_ppdstrActions );
    UNREFERENCED_PARAMETER( f_cActions );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_GenerateChallenge(
    __in                                    DRM_APP_CONTEXT  *f_poAppContext,
    __in                              const DRM_MID          *f_poMID,
    __in_ecount_opt( f_cchCustomData) const DRM_CHAR         *f_pchCustomData,
    __in_opt                                DRM_DWORD         f_cchCustomData,
    __out_bcount_opt( *f_pcbChallenge )     DRM_BYTE         *f_pbChallenge,
    __inout                                 DRM_DWORD        *f_pcbChallenge )
{
    UNREFERENCED_PARAMETER( f_poAppContext );
    UNREFERENCED_PARAMETER( f_poMID );
    UNREFERENCED_PARAMETER( f_pchCustomData );
    UNREFERENCED_PARAMETER( f_cchCustomData );
    UNREFERENCED_PARAMETER( f_pbChallenge );
    UNREFERENCED_PARAMETER( f_pcbChallenge );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL Drm_MeterCert_ProcessResponse(
    __in                        DRM_APP_CONTEXT *f_poAppContext,
    __in_bcount( f_cbResponse ) DRM_BYTE        *f_pbResponse,
    __in                        DRM_DWORD        f_cbResponse,
    __out                       DRM_RESULT      *f_pResult )
{
    UNREFERENCED_PARAMETER( f_poAppContext );
    UNREFERENCED_PARAMETER( f_pbResponse );
    UNREFERENCED_PARAMETER( f_cbResponse );
    UNREFERENCED_PARAMETER( f_pResult );
    return DRM_E_NOTIMPL;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

