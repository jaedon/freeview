/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmmetercertstore.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_MCS_DeleteMeterCert(
    IN       DRM_METERCERT_CONTEXT  *f_pcontextMeterCert,
    IN const DRM_MID                *f_pmid)
{
    UNREFERENCED_PARAMETER( f_pcontextMeterCert );
    UNREFERENCED_PARAMETER( f_pmid              );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MCS_InvalidateMeterCert(
    IN       DRM_METERCERT_CONTEXT  *f_pcontextMeterCert,
    IN const DRM_MID                *f_pmid)
{
    UNREFERENCED_PARAMETER( f_pcontextMeterCert );
    UNREFERENCED_PARAMETER( f_pmid              );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MCS_OpenStore  (
    IN  DRM_DST                        *f_pDatastore,
    __in_bcount( f_cbBuffer ) DRM_BYTE *f_pbBuffer,
    IN  DRM_DWORD                       f_cbBuffer,
    OUT DRM_METERCERT_CONTEXT          *f_pcontextMeterCert)
{
    UNREFERENCED_PARAMETER( f_pDatastore        );
    UNREFERENCED_PARAMETER( f_pbBuffer          );
    UNREFERENCED_PARAMETER( f_cbBuffer          );
    UNREFERENCED_PARAMETER( f_pcontextMeterCert );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MCS_CloseStore (
    IN  DRM_METERCERT_CONTEXT       *f_pcontextMeterCert)
{
    UNREFERENCED_PARAMETER( f_pcontextMeterCert );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MCS_InitEnum   (
    IN  DRM_METERCERT_CONTEXT       *f_pcontextMeterCert,
    OUT DRM_METERCERT_ENUM          *f_pcontextMeterCertEnum)
{
    UNREFERENCED_PARAMETER( f_pcontextMeterCert     );
    UNREFERENCED_PARAMETER( f_pcontextMeterCertEnum );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MCS_EnumNext   (
    IN  DRM_METERCERT_ENUM          *f_pcontextMeterCertEnum,
    OUT DRM_MID                     *f_pmid,
    OUT DRM_DST_SLOT_HINT           *f_pslotHint)
{
    UNREFERENCED_PARAMETER( f_pcontextMeterCertEnum );
    UNREFERENCED_PARAMETER( f_pmid                  );
    UNREFERENCED_PARAMETER( f_pslotHint             );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MCS_GetMeterCertByMID(
    IN                                   DRM_METERCERT_CONTEXT   *f_pcontextMeterCert,
    IN const                             DRM_MID                 *f_pmid,
       __out_ecount_opt(*f_pcchLAINFO)   DRM_WCHAR               *f_pwszLAINFO,
    IN OUT                               DRM_DWORD               *f_pcchLAINFO,
       OUT                               DRM_BOOL                *f_pfIsV2,
       __out_bcount_opt(*f_pcbMeterCert) DRM_BYTE                *f_pbMeterCert,
    IN OUT                               DRM_DWORD               *f_pcbMeterCert )
{
    UNREFERENCED_PARAMETER( f_pcontextMeterCert );
    UNREFERENCED_PARAMETER( f_pmid              );
    UNREFERENCED_PARAMETER( f_pwszLAINFO        );
    UNREFERENCED_PARAMETER( f_pcchLAINFO        );
    UNREFERENCED_PARAMETER( f_pfIsV2            );
    UNREFERENCED_PARAMETER( f_pbMeterCert       );
    UNREFERENCED_PARAMETER( f_pcbMeterCert      );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MCS_UpdateMeterCert(
    IN                                     DRM_METERCERT_CONTEXT      *f_pcontextMeterCert, 
    IN                                     DRM_CONST_STRING           *f_pdstrLAINFO,
    IN                                     DRM_BOOL                    f_fIsV2,
    __in_bcount_opt( f_cbMeterCert ) const DRM_BYTE                   *f_pbMeterCert,
    __in                                   DRM_DWORD                   f_cbMeterCert,
    IN                               const DRM_MID                    *f_pmid )
{
    UNREFERENCED_PARAMETER( f_pcontextMeterCert );
    UNREFERENCED_PARAMETER( f_pdstrLAINFO       );
    UNREFERENCED_PARAMETER( f_fIsV2             );
    UNREFERENCED_PARAMETER( f_pbMeterCert       );
    UNREFERENCED_PARAMETER( f_cbMeterCert       );
    UNREFERENCED_PARAMETER( f_pmid              );
    return DRM_E_NOTIMPL;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

