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

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_LoadLicenseAttributes(
    __in  const DRM_CONST_STRING    *f_rgpdstrEvents[],
    __in        DRM_DWORD            f_cRights,
    __inout     DRM_LICEVAL_CONTEXT *f_pLicEval )
{
    UNREFERENCED_PARAMETER( f_rgpdstrEvents );
    UNREFERENCED_PARAMETER( f_cRights       );
    UNREFERENCED_PARAMETER( f_pLicEval      );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_PerformActionsOPLCallbacks(
    __inout       DRM_FFLICENSE               *f_pFFLicense,
    __inout       DRM_STACK_ALLOCATOR_CONTEXT *f_pstack,
    __in    const DRM_CONST_STRING            *f_rgpdstrRights[],
    __in          DRM_DWORD                    f_iRight,
    __in          DRMPFNPOLICYCALLBACK         f_pfnPolicyCallback )
{
    UNREFERENCED_PARAMETER( f_pFFLicense        );
    UNREFERENCED_PARAMETER( f_pstack            );
    UNREFERENCED_PARAMETER( f_rgpdstrRights     );
    UNREFERENCED_PARAMETER( f_iRight            );
    UNREFERENCED_PARAMETER( f_pfnPolicyCallback );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_ExtractRevocationVersions(
    __inout DRM_FFLICENSE    *f_pFFLicense,
    __out   DRM_DWORD        *f_pidCRLWMDRMAppRequired,
    __out   DRM_DWORD        *f_pidCRLDeviceRequired,
    __out   DRM_DWORD        *f_pidCRLWMDRMNETRequired,
    __out   DRM_DWORD        *f_pdwRevInfo )
{
    UNREFERENCED_PARAMETER( f_pFFLicense             );
    UNREFERENCED_PARAMETER( f_pidCRLWMDRMAppRequired );
    UNREFERENCED_PARAMETER( f_pidCRLDeviceRequired   );
    UNREFERENCED_PARAMETER( f_pidCRLWMDRMNETRequired );
    UNREFERENCED_PARAMETER( f_pdwRevInfo             );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetLicenseInfoAndSetup(
    __inout       DRM_FFLICENSE    *f_pFFLicense,
    __in    const DRM_CONST_STRING *f_rgpdstrRights[],
    __in          DRM_DWORD         f_cRights )
{
    UNREFERENCED_PARAMETER( f_pFFLicense    );
    UNREFERENCED_PARAMETER( f_rgpdstrRights );
    UNREFERENCED_PARAMETER( f_cRights       );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetSourceIDFromLicense(
    __inout DRM_FFLICENSE *f_pFFLicense )
{
    UNREFERENCED_PARAMETER( f_pFFLicense );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetInclusionList(
    __in const DRM_CONST_STRING  *f_pdstrLicense,
    __inout    DRM_GUID           f_rgInclusionList[DRM_MAX_INCLUSION_GUIDS],
    __inout    DRM_BOOL           f_rgfGuidValid[DRM_MAX_INCLUSION_GUIDS][DRM_MAX_LICENSE_CHAIN_DEPTH],
    __in       DRM_DWORD          f_dwChainDepth )
{
    UNREFERENCED_PARAMETER( f_pdstrLicense    );
    UNREFERENCED_PARAMETER( f_rgInclusionList );
    UNREFERENCED_PARAMETER( f_rgfGuidValid    );
    UNREFERENCED_PARAMETER( f_dwChainDepth    );
    return DRM_E_NOTIMPL;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

