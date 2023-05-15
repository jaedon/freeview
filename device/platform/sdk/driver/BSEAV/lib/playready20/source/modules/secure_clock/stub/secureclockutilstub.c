/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmsecureclock.h>
#include <drmchain.h>
#include <drmlicreason.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL DRM_CLK_IsSecureClockSupported(DRM_VOID)
{
    return FALSE;
}

DRM_API DRM_BOOL DRM_CALL DRM_CLK_IsSecureClockUnsupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_VOID DRM_CALL DRM_LIC_LoadSecureTimeData(
    __inout  DRM_FFLICENSE               *f_pFFLicense,
    __in     DRM_DST                     *f_pDatastore )
{
    /* Do nothing */
    return;
}

DRM_API DRM_RESULT DRM_CALL DRM_LIC_CheckGracePeriod(
    __in     DRM_FFLICENSE               *f_pFFLicense )
{
    /*
    ** If secure clock is not supported
    ** then a grace period check is always successful.
    */
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL DRM_LEVL_CheckGracePeriodXMR(
    __inout     DRM_LICEVAL_CONTEXT          *f_pContextLicEval,
    __in        DRM_UINT64                    f_ui64CurrentDateTime,
    __in        DRM_XMR_LICENSE              *f_pXMR,
    __out       DRM_BOOL                     *f_pfGracePeriodExceeded )
{
    /*
    ** If secure clock is not supported
    ** then a grace period check is always successful.
    */
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL DRM_CLK_VariableSecureTimeGetorSet(
    __in    const DRM_CONST_STRING  *pdstrToken,
    __in          TOKEN             *pNewValue,
    __inout       TOKEN             *pResult,
    __inout       DRM_VOID          *pvOpaqueData,
    __in          DRM_BOOL           fSet )
{
    /*
    ** If secure clock is not supported
    ** then the variable is invalid.
    */
    return CPRMEXP_INVALID_VARIABLE;
}

EXIT_PK_NAMESPACE_CODE;

