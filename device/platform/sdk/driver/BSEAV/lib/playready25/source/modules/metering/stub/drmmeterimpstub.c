/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmmeterimp.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

extern DRM_EXPORT_VAR const DRM_KID g_kidTID;
const DRM_EXPORT_VAR DRM_DISCARDABLE DRM_KID g_kidTID = {0};

__checkReturn DRM_RESULT DRM_NO_INLINE DRM_MTR_GetKIDData (
    __in                              DRM_METERING_CONTEXT         *f_pmeterstore,
    __in                              DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack,
    __deref_out_ecount(*f_pcbDataOut) DRM_BYTE                    **f_ppbDataOut,
    __out                             DRM_DWORD                    *f_pcbDataOut )
{
    UNREFERENCED_PARAMETER(f_pmeterstore);
    UNREFERENCED_PARAMETER(f_pstack);
    UNREFERENCED_PARAMETER(f_ppbDataOut);
    UNREFERENCED_PARAMETER(f_pcbDataOut);
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_NO_INLINE DRM_MTR_CreateNewTID(
    __in_opt DRM_VOID *f_pOEMContext,
    __in     DRM_TID  *f_ptid )
{
    UNREFERENCED_PARAMETER(f_pOEMContext);
    UNREFERENCED_PARAMETER(f_ptid);
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_NO_INLINE DRM_MTR_ReadTIDFromMeteringStore(
    __in            DRM_METERING_CONTEXT   *f_pmeterstore,
    __in      const DRM_BYTE                f_rgbPasswordSST [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in      const DRM_MID                *f_pmid,
    __out           DRM_TID                *f_ptid,
    __out_opt       DRM_CONST_STRING       *f_pdstrTidBase64,
    __in            DRM_DST                *f_pDatastore )
{
    UNREFERENCED_PARAMETER(f_pmeterstore);
    UNREFERENCED_PARAMETER(f_rgbPasswordSST);
    UNREFERENCED_PARAMETER(f_pmid);
    UNREFERENCED_PARAMETER(f_ptid);
    UNREFERENCED_PARAMETER(f_pdstrTidBase64);
    UNREFERENCED_PARAMETER(f_pDatastore);
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_NO_INLINE DRM_MTR_WriteTIDToMeteringStore (
    __in        DRM_METERING_CONTEXT *f_pmeterstore,
    __in  const DRM_BYTE              f_rgbPasswordSST [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in  const DRM_MID              *f_pmid,
    __in  const DRM_TID              *f_ptid,
    __in        DRM_DST              *f_pDatastore )
{
    UNREFERENCED_PARAMETER(f_pmeterstore);
    UNREFERENCED_PARAMETER(f_rgbPasswordSST);
    UNREFERENCED_PARAMETER(f_pmid);
    UNREFERENCED_PARAMETER(f_ptid);
    UNREFERENCED_PARAMETER(f_pDatastore);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_CreateMeterStorePassword(
    __in                                             const DRM_CONST_STRING *f_pdstrMID,
    __out_bcount( DRM_SHA1_DIGEST_LEN )                    DRM_BYTE          f_rgbPasswordSST[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ],
    __inout_ecount( 1 )                                    DRM_BB_CONTEXT   *f_pcontextBBX )
{
    UNREFERENCED_PARAMETER(f_pdstrMID);
    UNREFERENCED_PARAMETER(f_rgbPasswordSST);
    UNREFERENCED_PARAMETER(f_pcontextBBX);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_CreateMeterStorePasswordFromBinaryMID(
    __in                                const DRM_MID        *f_pmid,
    __out_bcount( DRM_SHA1_DIGEST_LEN )       DRM_BYTE        f_rgbPasswordSST[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ],
    __inout_ecount( 1 )                       DRM_BB_CONTEXT *f_pcontextBBX )
{
    UNREFERENCED_PARAMETER(f_pmid);
    UNREFERENCED_PARAMETER(f_rgbPasswordSST);
    UNREFERENCED_PARAMETER(f_pcontextBBX);
    return DRM_E_NOTIMPL;
}


DRM_API DRM_RESULT DRM_CALL DRM_MTR_OpenId (
    __in                                     DRM_METERING_CONTEXT *f_pcontextMTR,
    __in                               const DRM_MID              *f_pmid,
    __in                               const DRM_KID              *f_pkid,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE              f_rgbPasswordMTR[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ],
    __in                                     DRM_DST              *f_pDatastore )
{
    UNREFERENCED_PARAMETER(f_pcontextMTR);
    UNREFERENCED_PARAMETER(f_rgbPasswordMTR);
    UNREFERENCED_PARAMETER(f_pmid);
    UNREFERENCED_PARAMETER(f_pkid);
    UNREFERENCED_PARAMETER(f_pDatastore);
    return DRM_E_NOTIMPL;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_IncrementCount(
    __in       DRM_METERING_CONTEXT *f_pcontextMTR,
    __in const DRM_CONST_STRING     *f_pdstrAction )
{
    UNREFERENCED_PARAMETER(f_pcontextMTR);
    UNREFERENCED_PARAMETER(f_pdstrAction);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MTR_GetAllData (
    __in                           DRM_METERING_CONTEXT *f_pcontextMTR,
    __out_bcount_opt( *f_pcbData ) DRM_BYTE             *f_pbData,
    __out                          DRM_DWORD            *f_pcbData )
{
    UNREFERENCED_PARAMETER(f_pcontextMTR);
    UNREFERENCED_PARAMETER(f_pbData);
    UNREFERENCED_PARAMETER(f_pcbData);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MTR_CloseContext(
    __in DRM_METERING_CONTEXT *f_pcontextMTR,
    __in DRM_DST              *f_pDatastore)
{
    UNREFERENCED_PARAMETER(f_pcontextMTR);
    UNREFERENCED_PARAMETER(f_pDatastore);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_InitEnum(
    __in  const DRM_MID                   *f_pmid,
    __out       DRM_METERINGENUM_CONTEXT  *f_pcontextMTREnum,
    __in        DRM_DST                   *f_pDatastore )
{
    UNREFERENCED_PARAMETER(f_pmid);
    UNREFERENCED_PARAMETER(f_pcontextMTREnum);
    UNREFERENCED_PARAMETER(f_pDatastore);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_EnumNext(
    __in  DRM_METERINGENUM_CONTEXT *f_pcontextMTREnum,
    __out DRM_KID                  *f_pkid,
    __out DRM_DWORD                *f_pcbData )
{
    UNREFERENCED_PARAMETER(f_pcontextMTREnum);
    UNREFERENCED_PARAMETER(f_pkid);
    UNREFERENCED_PARAMETER(f_pcbData);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_EnumLoadCurrent(
    __in                                     DRM_METERINGENUM_CONTEXT *f_pcontextMTREnum,
    __in                                     DRM_METERING_CONTEXT     *f_pcontextMTR,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                  f_rgbPasswordMTR[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ],
    __out                                    DRM_KID                  *f_pkid,
    __out                                    DRM_DWORD                *f_pcbData )
{
    UNREFERENCED_PARAMETER(f_pcontextMTREnum);
    UNREFERENCED_PARAMETER(f_pcontextMTR);
    UNREFERENCED_PARAMETER(f_rgbPasswordMTR);
    UNREFERENCED_PARAMETER(f_pkid);
    UNREFERENCED_PARAMETER(f_pcbData);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MTR_EnumDeleteCurrent(
    __in DRM_METERINGENUM_CONTEXT *f_pcontextMTREnum,
    __in DRM_METERING_CONTEXT     *f_pcontextMTR )
{
    UNREFERENCED_PARAMETER(f_pcontextMTREnum);
    UNREFERENCED_PARAMETER(f_pcontextMTR);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_UpdateData(
    __in                    const DRM_MID                 *f_pmid,
    __in                    const DRM_LID                 *f_plid,
    __in                    const DRM_KID                 *f_pkid,
    __in_ecount(f_cActions) const DRM_CONST_STRING       **f_ppdstrActions,
    __in                          DRM_DWORD                f_cActions,
    __in                          DRM_METERING_CONTEXT    *f_pcontextMTR,
    __in                          DRM_DST                 *f_pDatastore,
    __in                          DRM_BB_CONTEXT          *f_pbcontextBBX )
{
    UNREFERENCED_PARAMETER(f_pmid);
    UNREFERENCED_PARAMETER(f_plid);
    UNREFERENCED_PARAMETER(f_pkid);
    UNREFERENCED_PARAMETER(f_ppdstrActions);
    UNREFERENCED_PARAMETER(f_cActions);
    UNREFERENCED_PARAMETER(f_pcontextMTR);
    UNREFERENCED_PARAMETER(f_pDatastore);
    UNREFERENCED_PARAMETER(f_pbcontextBBX);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MTR_ParseMeterCert(
    __in      const DRM_CONST_STRING    *f_pdstrMeterCert,
    __in            DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __out_opt       PUBKEY              *f_ppubkeyMAS,
    __out_opt       DRM_CONST_STRING    *f_pdstrDataMID,
    __out_opt       DRM_MID             *f_pmid,
    __out_opt       DRM_CONST_STRING    *f_pdstrDataURL )
{
    UNREFERENCED_PARAMETER(f_pdstrMeterCert);
    UNREFERENCED_PARAMETER(f_pcontextLEVL);
    UNREFERENCED_PARAMETER(f_ppubkeyMAS);
    UNREFERENCED_PARAMETER(f_pdstrDataMID);
    UNREFERENCED_PARAMETER(f_pmid);
    UNREFERENCED_PARAMETER(f_pdstrDataURL);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MTR_GenerateMeterChallenge(
    __in                                 DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __inout_bcount( f_cbStack )          DRM_BYTE            *f_pbStack,
    __in                                 DRM_DWORD            f_cbStack,
    __in                           const DRM_CONST_STRING    *f_pdstrMeterCert,
    __out                                _XMBContext         *f_pChallenge,
    __inout                              DRM_DWORD           *f_pcbChallenge,
    __out_ecount_opt( *f_pcchURL )       DRM_CHAR            *f_pchURL,
    __inout                              DRM_DWORD           *f_pcchURL )
{
    UNREFERENCED_PARAMETER(f_pcontextLEVL);
    UNREFERENCED_PARAMETER(f_pbStack);
    UNREFERENCED_PARAMETER(f_cbStack);
    UNREFERENCED_PARAMETER(f_pdstrMeterCert);
    UNREFERENCED_PARAMETER(f_pChallenge);
    UNREFERENCED_PARAMETER(f_pcbChallenge);
    UNREFERENCED_PARAMETER(f_pchURL);
    UNREFERENCED_PARAMETER(f_pcchURL);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MTR_ProcessMeterResponse(
    __in                           DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __inout_bcount( f_cbStack )    DRM_BYTE            *f_pbStack,
    __in                           DRM_DWORD            f_cbStack,
    __in_bcount( f_cbData )  const DRM_BYTE            *f_pbData,
    __in                           DRM_DWORD            f_cbData,
    __out                          DRM_DWORD           *f_pfFlags )
{
    UNREFERENCED_PARAMETER(f_pcontextLEVL);
    UNREFERENCED_PARAMETER(f_pbStack);
    UNREFERENCED_PARAMETER(f_cbStack);
    UNREFERENCED_PARAMETER(f_pbData);
    UNREFERENCED_PARAMETER(f_cbData);
    UNREFERENCED_PARAMETER(f_pfFlags);
    return DRM_E_NOTIMPL;
}

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

