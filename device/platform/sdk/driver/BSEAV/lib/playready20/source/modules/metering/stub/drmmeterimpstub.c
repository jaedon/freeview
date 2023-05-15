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

DRM_RESULT DRM_NO_INLINE DRM_MTR_GetKIDData (
    IN  DRM_METERING_CONTEXT        *f_pmeterstore,
    IN  DRM_STACK_ALLOCATOR_CONTEXT *f_pstack,
    OUT DRM_BYTE                   **f_ppbDataOut,
    OUT DRM_DWORD                   *f_pcbDataOut)
{
    UNREFERENCED_PARAMETER(f_pmeterstore);
    UNREFERENCED_PARAMETER(f_pstack);
    UNREFERENCED_PARAMETER(f_ppbDataOut);
    UNREFERENCED_PARAMETER(f_pcbDataOut);
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_NO_INLINE DRM_MTR_CreateNewTID (DRM_VOID *f_pOEMContext, DRM_TID *f_ptid)
{
    UNREFERENCED_PARAMETER(f_pOEMContext);
    UNREFERENCED_PARAMETER(f_ptid);
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_NO_INLINE DRM_MTR_ReadTIDFromMeteringStore (
          DRM_METERING_CONTEXT   *f_pmeterstore,
    const DRM_BYTE                f_rgbPasswordSST [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    const DRM_MID                *f_pmid,
          DRM_TID                *f_ptid,
          DRM_CONST_STRING       *f_pdstrTidBase64,
          IN DRM_DST             *f_pDatastore )
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
    IN        DRM_METERING_CONTEXT *f_pmeterstore,
    IN  const DRM_BYTE              f_rgbPasswordSST [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    IN  const DRM_MID              *f_pmid,
    IN        DRM_TID              *f_ptid,
    IN        DRM_DST             *f_pDatastore )
{
    UNREFERENCED_PARAMETER(f_pmeterstore);
    UNREFERENCED_PARAMETER(f_rgbPasswordSST);
    UNREFERENCED_PARAMETER(f_pmid);
    UNREFERENCED_PARAMETER(f_ptid);
    UNREFERENCED_PARAMETER(f_pDatastore);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_CreateMeterStorePassword(
    IN     DRM_CONST_STRING *f_pdstrMID,
    __out_bcount( __CB_DECL( DRM_SHA1_DIGEST_LEN ) ) DRM_BYTE f_rgbPasswordSST[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ],
    IN     DRM_BB_CONTEXT   *f_pcontextBBX)
{
    UNREFERENCED_PARAMETER(f_pdstrMID);
    UNREFERENCED_PARAMETER(f_rgbPasswordSST);
    UNREFERENCED_PARAMETER(f_pcontextBBX);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_CreateMeterStorePasswordFromBinaryMID(
    IN const DRM_MID  *f_pmid,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE f_rgbPasswordSST[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ],
    IN       DRM_BB_CONTEXT *f_pcontextBBX)
{
    UNREFERENCED_PARAMETER(f_pmid);
    UNREFERENCED_PARAMETER(f_rgbPasswordSST);
    UNREFERENCED_PARAMETER(f_pcontextBBX);
    return DRM_E_NOTIMPL;
}


DRM_API DRM_RESULT DRM_CALL DRM_MTR_OpenId (
    IN       DRM_METERING_CONTEXT *f_pcontextMTR,
    IN const DRM_MID              *f_pmid,
    IN const DRM_KID              *f_pkid,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE f_rgbPasswordMTR[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ],
    IN       DRM_DST              *f_pDatastore)
{
    UNREFERENCED_PARAMETER(f_pcontextMTR);
    UNREFERENCED_PARAMETER(f_rgbPasswordMTR);
    UNREFERENCED_PARAMETER(f_pmid);
    UNREFERENCED_PARAMETER(f_pkid);
    UNREFERENCED_PARAMETER(f_pDatastore);
    return DRM_E_NOTIMPL;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_IncrementCount(
    IN       DRM_METERING_CONTEXT *f_pcontextMTR,
    IN const DRM_CONST_STRING     *f_pdstrAction )
{
    UNREFERENCED_PARAMETER(f_pcontextMTR);
    UNREFERENCED_PARAMETER(f_pdstrAction);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MTR_GetAllData (
    IN     DRM_METERING_CONTEXT *f_pcontextMTR,
    __out_bcount_opt( *f_pcbData ) DRM_BYTE *f_pbData,
    IN OUT DRM_DWORD            *f_pcbData)
{
    UNREFERENCED_PARAMETER(f_pcontextMTR);
    UNREFERENCED_PARAMETER(f_pbData);
    UNREFERENCED_PARAMETER(f_pcbData);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MTR_CloseContext(
    IN DRM_METERING_CONTEXT *f_pcontextMTR,
    IN DRM_DST              *f_pDatastore)
{
    UNREFERENCED_PARAMETER(f_pcontextMTR);
    UNREFERENCED_PARAMETER(f_pDatastore);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_InitEnum (
    IN  const DRM_MID                   *f_pmid,
    OUT       DRM_METERINGENUM_CONTEXT  *f_pcontextMTREnum,
    IN        DRM_DST                   *f_pDatastore)
{
    UNREFERENCED_PARAMETER(f_pmid);
    UNREFERENCED_PARAMETER(f_pcontextMTREnum);
    UNREFERENCED_PARAMETER(f_pDatastore);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_EnumNext (
    IN  DRM_METERINGENUM_CONTEXT *f_pcontextMTREnum,
    OUT DRM_KID                  *f_pkid,
    OUT DRM_DWORD                *f_pcbData)
{
    UNREFERENCED_PARAMETER(f_pcontextMTREnum);
    UNREFERENCED_PARAMETER(f_pkid);
    UNREFERENCED_PARAMETER(f_pcbData);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_EnumLoadCurrent (
    IN       DRM_METERINGENUM_CONTEXT *f_pcontextMTREnum,
    IN       DRM_METERING_CONTEXT     *f_pcontextMTR,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE f_rgbPasswordMTR[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ],
       OUT   DRM_KID                  *f_pkid,
       OUT   DRM_DWORD                *f_pcbData)
{
    UNREFERENCED_PARAMETER(f_pcontextMTREnum);
    UNREFERENCED_PARAMETER(f_pcontextMTR);
    UNREFERENCED_PARAMETER(f_rgbPasswordMTR);
    UNREFERENCED_PARAMETER(f_pkid);
    UNREFERENCED_PARAMETER(f_pcbData);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_MTR_EnumDeleteCurrent (
    IN       DRM_METERINGENUM_CONTEXT *f_pcontextMTREnum,
    IN       DRM_METERING_CONTEXT     *f_pcontextMTR)
{
    UNREFERENCED_PARAMETER(f_pcontextMTREnum);
    UNREFERENCED_PARAMETER(f_pcontextMTR);
    return DRM_E_NOTIMPL;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_MTR_UpdateData(
    DRM_MID                 *f_pmid,
    DRM_LID                 *f_plid,
    DRM_KID                 *f_pkid,
    const DRM_CONST_STRING **f_ppdstrActions,
    DRM_DWORD                f_cActions,
    DRM_METERING_CONTEXT    *f_pcontextMTR,
    DRM_DST                 *f_pDatastore,
    __in  DRM_BB_CONTEXT    *f_pbcontextBBX)
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
    IN const DRM_CONST_STRING    *f_pdstrMeterCert,
    IN       DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
       OUT   PUBKEY              *f_ppubkeyMAS,
       OUT   DRM_CONST_STRING    *f_pdstrDataMID,
       OUT   DRM_MID             *f_pmid,
       OUT   DRM_CONST_STRING    *f_pdstrDataURL)
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
    IN                           DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __in_bcount( f_cbStack )     DRM_BYTE            *f_pbStack,
    IN                           DRM_DWORD            f_cbStack,
    IN const                     DRM_CONST_STRING    *f_pdstrMeterCert,
       OUT                       _XMBContext         *f_pChallenge,
    IN OUT                       DRM_DWORD           *f_pcbChallenge,
    __out_ecount_opt( *f_pcchURL ) DRM_CHAR          *f_pchURL,
    IN OUT                       DRM_DWORD           *f_pcchURL)
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
} /* DRM_MTR_GenerateMeterChallenge */

DRM_API DRM_RESULT DRM_CALL DRM_MTR_ProcessMeterResponse(
    IN     DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __in_bcount( f_cbStack ) DRM_BYTE *f_pbStack,
    IN     DRM_DWORD            f_cbStack,
    __in_bcount( f_cbData ) DRM_BYTE *f_pbData,
    IN     DRM_DWORD            f_cbData,
       OUT DRM_DWORD           *f_pfFlags)
{
    UNREFERENCED_PARAMETER(f_pcontextLEVL);
    UNREFERENCED_PARAMETER(f_pbStack);
    UNREFERENCED_PARAMETER(f_cbStack);
    UNREFERENCED_PARAMETER(f_pbData);
    UNREFERENCED_PARAMETER(f_cbData);
    UNREFERENCED_PARAMETER(f_pfFlags);
    return DRM_E_NOTIMPL;
} /* DRM_MTR_ProcessMeterResponse */

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

