/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcertcache.h>
#include <drmblobcache.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_CERTCACHE_VerifyCachedCertificate (
    IN       DRM_BB_CONTEXT       *f_pcontextBBX,
    IN       DRM_SECSTORE_CONTEXT *f_pcontextSST,
    IN       DRM_DST              *f_pDatastore,
    IN const DRM_BYTE             *f_pbData,
    IN       DRM_DWORD             f_cbData ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_CERTCACHE_VerifyCachedCertificate (
    IN       DRM_BB_CONTEXT       *f_pcontextBBX,
    IN       DRM_SECSTORE_CONTEXT *f_pcontextSST,
    IN       DRM_DST              *f_pDatastore,
    IN const DRM_BYTE             *f_pbData,
    IN       DRM_DWORD             f_cbData )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT  dr        = DRM_SUCCESS;
    DRM_BOOL    fVerified = FALSE;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMUTILITIES, PERF_FUNC_DRM_CERTCACHE_VerifyCachedCertificate);

    ChkArg( f_pcontextBBX           != NULL
         && f_pcontextSST           != NULL
         && f_pDatastore            != NULL
         && f_pbData                != NULL
         && f_cbData                 > 0 );

    ChkDR( DRM_BLOBCACHE_Verify( f_pcontextBBX,
                                 f_pcontextSST,
                                 f_pDatastore,
                                 f_pbData,
                                 f_cbData,
                                 NULL,
                                 0,
                                 &fVerified ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return fVerified;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_CERTCACHE_AddCachedCertificate (
    IN       DRM_BB_CONTEXT       *f_pcontextBBX,
    IN       DRM_SECSTORE_CONTEXT *f_pcontextSSTRevocation,
    IN       DRM_DST              *f_pDatastore,
    IN const DRM_BYTE             *f_pbData,
    IN       DRM_DWORD             f_cbData ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_CERTCACHE_AddCachedCertificate (
    IN       DRM_BB_CONTEXT       *f_pcontextBBX,
    IN       DRM_SECSTORE_CONTEXT *f_pcontextSSTRevocation,
    IN       DRM_DST              *f_pDatastore,
    IN const DRM_BYTE             *f_pbData,
    IN       DRM_DWORD             f_cbData )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT  dr        = DRM_SUCCESS;

    ChkArg( f_pcontextBBX           != NULL
         && f_pDatastore            != NULL
         && f_pcontextSSTRevocation != NULL
         && f_pbData                != NULL
         && f_cbData                 > 0 );

    ChkDR( DRM_BLOBCACHE_Update( f_pcontextBBX,
                                 f_pcontextSSTRevocation,
                                 f_pDatastore,
                                 f_pbData,
                                 f_cbData,
                                 NULL,
                                 0 ) );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

