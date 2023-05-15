/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcertcache.h>
#include <drmresults.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_CERTCACHE_VerifyCachedCertificate (
    __in                        DRM_BB_CONTEXT       *f_pcontextBBX,
    __in                        DRM_SECSTORE_CONTEXT *f_pcontextSST,
    __in                        DRM_DST              *f_pDatastore,
    __in_ecount(f_cbData) const DRM_BYTE             *f_pbData,
    __in                        DRM_DWORD             f_cbData )
{
    CLAW_AUTO_RANDOM_CIPHER
    return FALSE;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_CERTCACHE_AddCachedCertificate (
    __in                        DRM_BB_CONTEXT       *f_pcontextBBX,
    __in                        DRM_SECSTORE_CONTEXT *f_pcontextSSTRevocation,
    __in                        DRM_DST              *f_pDatastore,
    __in_ecount(f_cbData) const DRM_BYTE             *f_pbData,
    __in                        DRM_DWORD             f_cbData )
{
    CLAW_AUTO_RANDOM_CIPHER
    return DRM_SUCCESS;
}

EXIT_PK_NAMESPACE_CODE;

