/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcertcache.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_CERTCACHE_VerifyCachedCertificate (
    IN       DRM_BB_CONTEXT       *f_pcontextBBX,
    IN       DRM_SECSTORE_CONTEXT *f_pcontextSST,
    IN       DRM_DST              *f_pDatastore,
    IN const DRM_BYTE             *f_pbData,
    IN       DRM_DWORD             f_cbData )
{
    return FALSE;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_CERTCACHE_AddCachedCertificate (
    IN       DRM_BB_CONTEXT       *f_pcontextBBX,
    IN       DRM_SECSTORE_CONTEXT *f_pcontextSSTRevocation,
    IN       DRM_DST              *f_pDatastore,
    IN const DRM_BYTE             *f_pbData,
    IN       DRM_DWORD             f_cbData )
{
    return DRM_SUCCESS;
}

EXIT_PK_NAMESPACE_CODE;

