/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmblobcache.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BLOBCACHE_Verify(
    __in_opt    DRM_BB_CONTEXT       *f_poBBXContext,
    __in_opt    DRM_SECSTORE_CONTEXT *f_poSecStoreContext,
    __in_opt    DRM_DST              *f_poDatastoreHDS,
    __in_ecount( f_cbBlob1 )
          const DRM_BYTE             *f_pbBlob1,
    __in        DRM_DWORD             f_cbBlob1,
    __in_ecount_opt( f_cbBlob2 )
          const DRM_BYTE             *f_pbBlob2,
    __in        DRM_DWORD             f_cbBlob2,
    __out       DRM_BOOL             *f_pfResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pfResult != NULL );

    /* No cache to verify against - always a cache miss. */
    *f_pfResult = FALSE;

ErrorExit:
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_BLOBCACHE_Update(
    __in        DRM_BB_CONTEXT       *f_poBBXContext,
    __in        DRM_SECSTORE_CONTEXT *f_poSecStoreContext,
    __in        DRM_DST              *f_poDatastoreHDS,
    __in_ecount( f_cbBlob1 )
          const DRM_BYTE             *f_pbBlob1,
    __in        DRM_DWORD             f_cbBlob1,
    __in_ecount_opt( f_cbBlob2 )
          const DRM_BYTE             *f_pbBlob2,
    __in        DRM_DWORD             f_cbBlob2 )
{
    /* No cache to update. */
    return DRM_SUCCESS;
}

EXIT_PK_NAMESPACE_CODE;

