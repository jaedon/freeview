/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_APPPOLICYIMPLREAL_C
#include <drmapppolicy.h>
#include <drmlastinclude.h>
ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_APPP_ExtractWM7AppPolicyData(
    __in_bcount(f_cbPolicyData)  const DRM_BYTE*             f_pbPolicyData,
    __in                               DRM_DWORD             f_cbPolicyData,
    __out_ecount( 1 )                  DRM_WM7AppPolicyData *f_pWM7AppPolicyData,
    __out_ecount( 1 )                  DRM_BOOL             *f_fIsWM7AppPolicy )
{
    UNREFERENCED_PARAMETER( f_pbPolicyData );
    UNREFERENCED_PARAMETER( f_cbPolicyData );
    UNREFERENCED_PARAMETER( f_pWM7AppPolicyData );
    UNREFERENCED_PARAMETER( f_fIsWM7AppPolicy );
    return DRM_SUCCESS;
}

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_APPP_RequireExecutePolicy(
    __in                       DRM_WORD                     f_wRightID,
    __in_ecount( 1 )     const DRM_XMR_GLOBAL_REQUIREMENTS *f_pGlobalPolicies,
    __in_ecount( 1 )     const DRM_XMR_UNKNOWN_OBJECT      *f_pUnknownObject,
    __inout_ecount( 1 )        DRM_LICEVAL_CONTEXT         *f_pContextLicEval )
{
    CLAW_AUTO_RANDOM_CIPHER
    UNREFERENCED_PARAMETER( f_wRightID );
    UNREFERENCED_PARAMETER( f_pGlobalPolicies );
    UNREFERENCED_PARAMETER( f_pUnknownObject );
    UNREFERENCED_PARAMETER( f_pContextLicEval );
    return DRM_SUCCESS;
}

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_APPP_AllowExecutePolicy(
    __in_ecount( 1 )     const DRM_XMR_GLOBAL_REQUIREMENTS *f_pGlobalPolicies,
    __in_ecount( 1 )     const DRM_XMR_UNKNOWN_CONTAINER   *f_pUnknownContainer,
    __inout_ecount( 1 )        DRM_LICEVAL_CONTEXT         *f_pContextLicEval )
{
    CLAW_AUTO_RANDOM_CIPHER
    UNREFERENCED_PARAMETER( f_pGlobalPolicies );
    UNREFERENCED_PARAMETER( f_pUnknownContainer );
    UNREFERENCED_PARAMETER( f_pContextLicEval );
    return DRM_SUCCESS;
}

DRM_API DRM_BOOL DRM_CALL DRM_APPP_CanBind(
    __in_opt                       const DRM_XMR_LICENSE  *f_plicenseXMR,
    __in_ecount( f_dwChainLength ) const DRM_BINDING_INFO *f_pBindingInfo,
    __in                                 DRM_DWORD         f_dwChainLength,
    __inout_ecount( 1 )                  DRM_BB_CONTEXT   *f_pcontextBBX,
    __out_opt                            DRM_RESULT       *f_pDr )
{
    UNREFERENCED_PARAMETER( f_plicenseXMR );
    return DRM_BBX_CanBind(
        f_pBindingInfo,
        f_dwChainLength,
        f_pcontextBBX,
        f_pDr );
}

DRM_API DRM_RESULT DRM_CALL DRM_APPP_AsymmetricToSymmetricKeyXMR(
    __inout_ecount( 1 )               DRM_BB_CONTEXT                *f_pcontextBBX,
    __inout_ecount_opt( 1 )           DRM_BINDING_INFO              *f_pbindinfo,
    __inout_bcount( f_cbLicense )     DRM_BYTE                      *f_pbLicense,
    __in                              DRM_DWORD                      f_cbLicense,
    __inout_ecount( 1 )               DRM_DWORD                     *f_pcbLicenseUsed,
    __inout_ecount( 1 )               DRM_STACK_ALLOCATOR_CONTEXT   *f_pStack,
    __out_ecount( 1 )                 DRM_DWORD                     *f_pcbSymmKey )
{
    return DRM_BBX_AsymmetricToSymmetricKeyXMR(
        f_pcontextBBX,
        f_pbindinfo,
        f_pbLicense,
        f_cbLicense,
        f_pcbLicenseUsed,
        f_pStack,
        f_pcbSymmKey );
}

DRM_API DRM_RESULT DRM_CALL DRM_APPP_CipherKeySetup(
    __in_opt                       const DRM_XMR_LICENSE     *f_plicenseXMR,
    __in_ecount( f_dwChainLength ) const DRM_BINDING_INFO    *f_pBindingInfo,
    __in                                 DRM_DWORD            f_dwChainLength,
    __out_ecount( 1 )                    DRM_CIPHER_CONTEXT  *f_pCipherContext,
    __inout_ecount( 1 )                  DRM_BB_CONTEXT      *f_pcontextBBX )
{
    UNREFERENCED_PARAMETER( f_plicenseXMR );
    return DRM_BBX_CipherKeySetup(
        f_pBindingInfo,
        f_dwChainLength,
        f_pCipherContext,
        f_pcontextBBX );
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

