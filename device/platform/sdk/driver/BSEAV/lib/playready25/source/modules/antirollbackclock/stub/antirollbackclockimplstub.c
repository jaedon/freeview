/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmantirollbackclock.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_OverrideMachineDateTimeWithSavedValue(
    __inout     DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __out       DRM_UINT64          *f_pui64MachineDateTime )
    {
    /*
    ** Leave f_pui64MachineDateTime unchanged
    */
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_GetMachineDatetime(
    __inout DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __out   DRM_UINT64          *f_pui64MachineDateTime )
{
    /*
    ** Leave f_pui64MachineDateTime unchanged
    */
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_ASD_CheckRollback(
    __inout DRM_LICEVAL_CONTEXT *f_pContextLicEval )
{
    /*
    ** Clock rollback check simply succeeds if anti-rollback clock is not supported
    */
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_DisableOnRollback(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __in    const DRM_XMR_LICENSE     *f_pXMR,
    __out         DRM_BOOL            *f_pfDisabled )
{
    /*
    ** License is never disabled by clock rollback if anti-rollback clock is not supported
    */
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_SaveLicenseStoreTime(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __in    const DRM_XMR_LICENSE     *f_pXMR )
{
    /*
    ** The license store time doesn't need to be saved if anti-rollback clock is not supported
    */
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_ReportClockRollback(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __in    const DRM_XMR_LICENSE     *f_pXMR )
{
    /*
    ** There's nothing to report if anti-rollback clock is not supported
    */
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_DOMST_GetDomainSavedDateTime(
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                 f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __inout_opt                              DRM_VOID                *f_pOEMContext,
    __in                                     DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in                               const DRM_GUID                *f_poAccountID,
    __out_opt                                DRM_UINT64              *f_puiDomainSavedDateTime )
{
    /*
    ** There's nothing to get if anti-rollback clock is not supported
    */
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_DOMST_ClockRollbackDetected(
    __in  DRM_BB_CONTEXT          *f_poBBXContext,
    __in  DRM_UINT64               ui64GlobalSavedTime )
{
    /*
    ** There's nothing to do on rollback if anti-rollback clock is not supported
    */
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_DOMST_CreateDomainStorePassword(
    __in                                const DRM_GUID         *f_poAccountID,
    __in                                      DRM_BB_CONTEXT   *f_poBBXContext,
    __out_bcount( DRM_SHA1_DIGEST_LEN )       DRM_BYTE          f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)] )
{
    DRM_RESULT  dr  = DRM_SUCCESS;

    ChkArg( f_poAccountID != NULL );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_rgbPasswordSST != NULL );

    /*
    ** A zeroed password is all that's required if anti-rollback clock is not supported
    */
    MEMSET( f_rgbPasswordSST, 0, __CB_DECL(DRM_SHA1_DIGEST_LEN) );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_LIC_CheckClockRollback(
    __in                    DRM_LICEVAL_CONTEXT         *f_pLicEval,
    __in                    DRM_LICSTORE_CONTEXT        *f_pLicStoreXML,
    __in                    DRM_LICSTORE_CONTEXT        *f_pLicStoreXMR,
    __in                    DRM_LICSTOREENUM_CONTEXT    *f_pLicEnum,
    __in_ecount(f_cbBuffer) DRM_BYTE                    *f_pbBuffer,
    __in                    DRM_DWORD                    f_cbBuffer,
    __in                    DRM_DST                     *f_pDatastore,
    __in                    DRM_STACK_ALLOCATOR_CONTEXT *f_poStack )
{
    /*
    ** There's nothing to check if anti-rollback clock is not supported
    */
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_LIC_ResetClockRollback(
    __in DRM_LICEVAL_CONTEXT       *f_pLicEval )
{
    /*
    ** There's nothing to reset if anti-rollback clock is not supported
    */
    UNREFERENCED_PARAMETER( f_pLicEval );
    return DRM_SUCCESS;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

