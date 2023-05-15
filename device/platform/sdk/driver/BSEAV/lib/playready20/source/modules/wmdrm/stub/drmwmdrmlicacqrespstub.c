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

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LA_ProcessResponse(
    __in_bcount( f_cbResponse ) DRM_BYTE *f_pbResponse,
    IN     DRM_DWORD                             f_cbResponse,
    IN     DRM_LICEVAL_CONTEXT                  *f_pcontextLEVL,
    IN     DRM_LICSTORE_CONTEXT                 *f_pcontextLSTXML,
    IN     DRM_LICSTORE_CONTEXT                 *f_pcontextLSTXMR,
    __in   DRM_LICSTOREENUM_CONTEXT             *f_pLicStoreEnumContext,
    IN     DRM_INCLUSION_LIST_CALLBACK_STRUCT   *f_pInclusionListCallback,
    IN     pfnLicenseResponseCallback            f_pfnCallback,
    IN     DRM_VOID                             *f_pvCallbackContext,
    IN     DRM_DST                              *f_pDatastore,
    IN     DRM_VIEW_RIGHTS_CONTEXT              *f_pcontextASD,
    __in_bcount( DRM_MAX_LICENSESIZE ) DRM_BYTE f_rgbLicenseBuffer[__CB_DECL(DRM_MAX_LICENSESIZE)],
    IN     DRM_SYNC_CONTEXT                     *f_pcontextSync,
       OUT DRM_LONG                             *f_plReason)
{
    UNREFERENCED_PARAMETER( f_pbResponse             );
    UNREFERENCED_PARAMETER( f_cbResponse             );
    UNREFERENCED_PARAMETER( f_pcontextLEVL           );
    UNREFERENCED_PARAMETER( f_pcontextLSTXML         );
    UNREFERENCED_PARAMETER( f_pcontextLSTXMR         );
    UNREFERENCED_PARAMETER( f_pLicStoreEnumContext   );
    UNREFERENCED_PARAMETER( f_pInclusionListCallback );
    UNREFERENCED_PARAMETER( f_pfnCallback            );
    UNREFERENCED_PARAMETER( f_pvCallbackContext      );
    UNREFERENCED_PARAMETER( f_pDatastore             );
    UNREFERENCED_PARAMETER( f_pcontextASD            );
    UNREFERENCED_PARAMETER( f_rgbLicenseBuffer       );
    UNREFERENCED_PARAMETER( f_pcontextSync           );
    UNREFERENCED_PARAMETER( f_plReason               );
    return DRM_E_NOTIMPL;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

