/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmsecurestore.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_SST_LoadAttributesIntoCache(
    __in const DRM_BYTE        *pbSlotData,
    __in       DRM_DWORD        cbSlotData,
    __out      CachedAttribute  rgAttributes[DRM_MAX_ATTRIBUTES_PER_SST_KEY],
    __out      DRM_WORD        *pwNumAttributes )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_GetTokenValueFromCache(
    __in const DRM_CONST_STRING     *pdstrAttribute,
    __in       CachedAttribute       rgAttributes[DRM_MAX_ATTRIBUTES_PER_SST_KEY],
    __in       DRM_WORD              wNumAttributes,
    __out      TOKEN                *pToken )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_LoadKeyTokens(
    __inout    DRM_SECSTORE_CONTEXT   *pcontextSST,
    __in       DRM_DWORD               dwFlags,
    __in       DRM_DST                *pDatastore )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_OpenKeyTokens(
    __inout                                  DRM_SECSTORE_CONTEXT   *pcontextSST,
    __in                               const DRM_ID                 *pKey1,
    __in_opt                           const DRM_ID                 *pKey2,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                rgbPassword [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                                     DRM_DWORD               dwFlags,
    __in                                     eDRM_SECURE_STORE_TYPE  eType,
    __in                                     DRM_DST                *pDatastore )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_GetTokenValue(
    __in       DRM_SECSTORE_CONTEXT *pcontextSST,
    __in const DRM_CONST_STRING     *pdstrAttribute,
    __out      TOKEN                *pToken )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_SetTokenValue(
    __in       DRM_SECSTORE_CONTEXT *pcontextSST,
    __in const DRM_CONST_STRING     *pdstrAttribute,
    __in const TOKEN                *pToken )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_SetExplicitResolutionTokenValue(
    __in       DRM_SECSTORE_CONTEXT *pcontextSST,
    __in const DRM_CONST_STRING     *pdstrAttribute,
    __in const TOKEN                *pToken,
    __in       DRM_DWORD             dwFlags )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_CloseKey(
    __in           DRM_SECSTORE_CONTEXT *pcontextSST,
    __in           DRM_DST              *pDatastore,
    __in_opt       DRMPFNPOLICYCALLBACK  pfnMergeCallback,
    __in_opt const DRM_VOID             *pvCallbackData )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_OpenAndLockSlot(
    __in                                     DRM_DST                *f_pDatastore,
    __in                                     eDRM_SECURE_STORE_TYPE  f_eType,
    __in                               const DRM_ID                 *f_pKey1,
    __in_opt                           const DRM_ID                 *f_pKey2,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                f_rgbPassword[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                                     DRM_DWORD               f_dwFlags,
    __out                                    DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __out_opt                                DRM_DWORD              *f_pcbData )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_GetLockedData(
    __in                           DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __out_bcount_opt( *f_pcbData ) DRM_BYTE               *f_pbData,
    __out                          DRM_DWORD              *f_pcbData )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_SetLockedData(
    __in                          DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __in                          DRM_DWORD               f_cbData,
    __in_bcount( f_cbData ) const DRM_BYTE               *f_pbData )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_CloseLockedSlot(
    __in  DRM_SECSTORE_CONTEXT *f_pcontextSST )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_GetData(
    __in                                     DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __in                               const DRM_ID                 *f_pKey1,
    __in_opt                           const DRM_ID                 *f_pKey2,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                f_rgbPassword[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                                     eDRM_SECURE_STORE_TYPE  f_eType,
    __in                                     DRM_DST                *f_pDatastore,
    __out_bcount_opt( *f_pcbData )           DRM_BYTE               *f_pbData,
    __out                                    DRM_DWORD              *f_pcbData )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_SetData(
    __in                                     DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __in                               const DRM_ID                 *f_pKey1,
    __in_opt                           const DRM_ID                 *f_pKey2,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                f_rgbPassword[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                                     eDRM_SECURE_STORE_TYPE  f_eType,
    __in                                     DRM_DST                *f_pDatastore,
    __in_bcount( f_cbData )            const DRM_BYTE               *f_pbData,
    __in                                     DRM_DWORD               f_cbData )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_DeleteKey(
    __in       DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __in       eDRM_SECURE_STORE_TYPE  f_typeSST,
    __in const DRM_ID                 *f_pid1,
    __in const DRM_ID                 *f_pid2,
    __in       DRM_DST                *f_pDatastore)
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_GetAllData(
    __in                         DRM_SECSTORE_CONTEXT *pcontextSST,
    __out_bcount_opt( *pcbData ) DRM_BYTE             *pbData,
    __inout                      DRM_DWORD            *pcbData )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_OpenEnumerator(
    __in           eDRM_SECURE_STORE_TYPE    eType,
    __in_opt const DRM_ID                   *pKey1,
    __out          DRM_SECSTOREENUM_CONTEXT *pcontextSSTEnum,
    __in           DRM_DST                  *pDatastore,
    __in           DRM_BOOL                  fExclusiveLock )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_EnumNext(
    __in                DRM_SECSTOREENUM_CONTEXT *pcontextSSTEnum,
    __out_ecount_opt(1) DRM_ID                   *pKey1,
    __out_ecount_opt(1) DRM_ID                   *pKey2,
    __out_ecount(1)     DRM_DWORD                *pcbData )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_EnumLoadCurrent(
    __in                                     DRM_SECSTOREENUM_CONTEXT *pcontextSSTEnum,
    __inout                                  DRM_SECSTORE_CONTEXT     *pcontextSST,
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                  rgbPassword[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __out                                    DRM_ID                   *pKey1,
    __out_opt                                DRM_ID                   *pKey2,
    __inout                                  DRM_DWORD                *pcbData )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_EnumDeleteCurrent(
    __in       DRM_SECSTOREENUM_CONTEXT *pcontextSSTEnum,
    __inout    DRM_SECSTORE_CONTEXT     *pcontextSST )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_CreateGlobalStorePassword(
    __in_opt DRM_VOID *f_pOEMContext,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE  f_rgbPasswordSST[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                                DRM_BYTE *f_pbContextBBX )
{
    /* Successfully "creating" this password is harmless since all the functions using it return DRM_E_NOTIMPL. */
    return DRM_SUCCESS;
}

DRM_API DRM_RESULT DRM_CALL DRM_SST_CreateLicenseStatePassword(
    __in                                const DRM_LID        *f_plid,
    __out_bcount( DRM_SHA1_DIGEST_LEN )       DRM_BYTE        f_rgbPasswordLST[__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                                      DRM_BB_CONTEXT *f_pContextBBX )
{
    return DRM_E_NOTIMPL;
}

EXIT_PK_NAMESPACE_CODE;
