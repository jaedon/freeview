/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmantirollbackclock.h>

ENTER_PK_NAMESPACE_CODE;

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
    IN OUT DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    OUT    DRM_UINT64          *f_pui64MachineDateTime )
{
    /*
    ** Leave f_pui64MachineDateTime unchanged
    */
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_ASD_CheckRollback(
    IN OUT DRM_LICEVAL_CONTEXT *f_pContextLicEval )
{
    /*
    ** Clock rollback check simply succeeds if anti-rollback clock is not supported
    */
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_DisableOnRollback(
    IN OUT DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    IN     DRM_XMR_LICENSE     *f_pXMR,
    OUT    DRM_BOOL            *f_pfDisabled )
{
    /*
    ** License is never disabled by clock rollback if anti-rollback clock is not supported
    */
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_SaveLicenseStoreTime(
    IN OUT DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    IN     DRM_XMR_LICENSE     *f_pXMR )
{
    /*
    ** The license store time doesn't need to be saved if anti-rollback clock is not supported
    */
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_ReportClockRollback(
    IN OUT DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    IN     DRM_XMR_LICENSE     *f_pXMR )
{
    /*
    ** There's nothing to report if anti-rollback clock is not supported
    */
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_DOMST_GetDomainSavedDateTime(
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE           f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __inout_opt                        DRM_VOID                *f_pOEMContext,
    IN                                 DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    IN                                 const DRM_GUID          *f_poAccountID,
    OUT                                DRM_UINT64              *f_puiDomainSavedDateTime )
{
    /*
    ** There's nothing to get if anti-rollback clock is not supported
    */
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_DOMST_ClockRollbackDetected(
    IN  DRM_BB_CONTEXT          *f_poBBXContext,
    IN  DRM_UINT64               ui64GlobalSavedTime )
{
    /*
    ** There's nothing to do on rollback if anti-rollback clock is not supported
    */
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_ARCLK_DOMST_CreateDomainStorePassword(
    IN  const DRM_GUID          *f_poAccountID,
    IN  DRM_BB_CONTEXT          *f_poBBXContext,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)] )
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
    IN DRM_LICEVAL_CONTEXT         *f_pLicEval,
    IN DRM_LICSTORE_CONTEXT        *f_pLicStoreXML,
    IN DRM_LICSTORE_CONTEXT        *f_pLicStoreXMR,
    IN DRM_LICSTOREENUM_CONTEXT    *f_pLicEnum,
    IN DRM_BYTE                    *f_pbBuffer,
    IN DRM_DWORD                    f_cbBuffer,
    IN DRM_DST                     *f_pDatastore,
    IN DRM_STACK_ALLOCATOR_CONTEXT *f_poStack )
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

EXIT_PK_NAMESPACE_CODE;

