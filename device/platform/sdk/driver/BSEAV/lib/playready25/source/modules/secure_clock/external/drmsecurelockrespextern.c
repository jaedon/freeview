/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_SECURECLOCKRESPONSE_C
#include <drmsecureclock.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_EXPORT_VAR const DRM_ID g_idSData     = { 'S', 0, 'E', 0, 'C', 0, 'U', 0, 'R', 0, 'E', 0, '1', 0, '\0', 0 };
DRM_EXPORT_VAR const DRM_ID g_idSDataPrev = { 'S', 0, 'E', 0, 'C', 0, 'U', 0, 'R', 0, 'E', 0, '2', 0, '\0', 0 };

DRM_API DRM_RESULT DRM_CALL DRM_CLK_ProcessResponse(
    __in                    const PUBKEY                *ppubkeySecureClockServer,
    __in_ecount(cbResponse)       DRM_BYTE              *pbResponse,
    __in                          DRM_DWORD              cbResponse,
    __in                    const DRM_BYTE               rgbSecStoreInPswd [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __out                         DRM_RESULT            *pResult,
    __in                          DRM_CRYPTO_CONTEXT    *pContext,
    __in                          DRM_DST               *poDatastore,
    __in                          DRM_SECSTORE_CONTEXT  *poSecureStoreContext,
    __in                          DRM_LICEVAL_CONTEXT   *poLicEvalContext,
    __in                          DRM_VOID              *pOEMContext )
{
    return DRM_E_NOTIMPL;
}

DRM_API DRM_VOID DRM_CALL _PrepareSecureClockDataForWriting( DRM_SECSTORE_CLOCK_DATA *pData )
{
    /*
    ** Do nothing.
    */
    return;
}

EXIT_PK_NAMESPACE_CODE;

