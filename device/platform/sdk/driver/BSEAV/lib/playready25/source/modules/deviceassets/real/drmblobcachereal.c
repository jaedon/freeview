/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMBLOBCACHE_C
#include <drmblobcache.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

static const DRM_ID g_idBlobCache = { 'B', 0, 'L', 0, 'O', 0, 'B', 0,
                                      'C', 0, 'A', 0, 'C', 0, 'H', 0 };

/*********************************************************************
**
** Function: _CalculateBlobHash
**
** Synopsis: Function that calculates a 16 bytes hash over one or two BLOBs.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a black box context.
** [f_pbBlob1]              -- Pointer to a buffer of the first BLOB.
** [f_cbBlob1]              -- Size of the buffer of the first BLOB.
** [f_pbBlob2]              -- Optional pointer to a buffer of the second BLOB.
** [f_cbBlob2]              -- Size of the buffer of the second BLOB. If
**                             the second BLOB does not exist, the value should
**                             be 0.
** [f_pbPassword]           -- Pointer to a buffer to receive the secure store
**                             password.
** [f_pbHashValue]          -- Pointer to a buffer to receive the generated
**                             hash.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "Buffer sizes are fixed and the function is static.")
static DRM_NO_INLINE DRM_RESULT _CalculateBlobHash(
    __in                                                   DRM_BB_CONTEXT *f_poBBXContext,
    __in_ecount( f_cbBlob1 )                         const DRM_BYTE       *f_pbBlob1,
    __in                                                   DRM_DWORD       f_cbBlob1,
    __in_ecount_opt( f_cbBlob2 )                     const DRM_BYTE       *f_pbBlob2,
    __in                                                   DRM_DWORD       f_cbBlob2,
    __out_bcount( __CB_DECL( DRM_SHA1_DIGEST_LEN ) )       DRM_BYTE        f_pbPassword[DRM_SHA1_DIGEST_LEN],
    __out_bcount( __CB_DECL( DRM_ID_SIZE ) )               DRM_BYTE        f_pbHashValue[DRM_ID_SIZE] ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _CalculateBlobHash(
    __in                                                   DRM_BB_CONTEXT *f_poBBXContext,
    __in_ecount( f_cbBlob1 )                         const DRM_BYTE       *f_pbBlob1,
    __in                                                   DRM_DWORD       f_cbBlob1,
    __in_ecount_opt( f_cbBlob2 )                     const DRM_BYTE       *f_pbBlob2,
    __in                                                   DRM_DWORD       f_cbBlob2,
    __out_bcount( __CB_DECL( DRM_SHA1_DIGEST_LEN ) )       DRM_BYTE        f_pbPassword[DRM_SHA1_DIGEST_LEN],
    __out_bcount( __CB_DECL( DRM_ID_SIZE ) )               DRM_BYTE        f_pbHashValue[DRM_ID_SIZE] )
{
    CLAW_AUTO_RANDOM_CIPHER
PREFAST_POP
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SHA_CONTEXT oSHAContext = { 0 };
    DRM_BYTE rgbSHAHash[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ] = { 0 };
    DRM_DWORD i = 0;

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_pbBlob1 != NULL && f_cbBlob1 > 0 );
    ChkArg( ( f_pbBlob2 != NULL && f_cbBlob2 > 0 )
         || ( f_pbBlob2 == NULL && f_cbBlob2 == 0 ) );
    ChkArg( f_pbPassword != NULL && f_pbHashValue != NULL );

    /*
    ** Use the first piece of BLOB as the seed to generate a
    ** secure store password.
    */
    ChkDR( DRM_BBX_HashValue( f_pbBlob1,
                              f_cbBlob1,
                              f_pbPassword,
                              f_poBBXContext ) );

    ChkDR( DRM_SHA_Init( &oSHAContext, eDRM_SHA_1 ) );

    /* Run SHA-1 hash over the secure store password. */
    ChkDR( DRM_SHA_Update( f_pbPassword, DRM_SHA1_DIGEST_LEN, &oSHAContext ) );

    /* Run SHA-1 hash over the second BLOB if it is not NULL. */
    if ( f_pbBlob2 != NULL )
    {
        ChkDR( DRM_SHA_Update( f_pbBlob2, f_cbBlob2, &oSHAContext ) );
    }

    ChkDR( DRM_SHA_Finalize( &oSHAContext, DRM_SHA1_DIGEST_LEN, rgbSHAHash ) );

    MEMCPY( f_pbHashValue, rgbSHAHash, DRM_ID_SIZE );

    DRMCASSERT( DRM_SHA1_DIGEST_LEN >= DRM_ID_SIZE
             && DRM_SHA1_DIGEST_LEN - DRM_ID_SIZE <= DRM_ID_SIZE );

    /* XOR the extra bytes of the SHA-1 hash with the beginning bytes of the SHA-1 hash. */
    for ( i = 0; i < DRM_SHA1_DIGEST_LEN - DRM_ID_SIZE; i++ )
    {
        DRM_BYTE bTmp = (DRM_BYTE)( GET_BYTE( f_pbHashValue, i )
                                  ^ GET_BYTE( rgbSHAHash, DRM_ID_SIZE + i ) );

        PUT_BYTE( f_pbHashValue, i, bTmp );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_BLOBCACHE_Verify
**
** Synopsis: Function that verifies whether the hash of one or two
**           BLOBs already exists in the secure store.
**
** Arguments:
**
** [f_poBBXContext]         -- Optional pointer to a black box context.
** [f_poSecStoreContext]    -- Optional pointer to a secure store context.
** [f_poDatastoreHDS]       -- Optional pointer to a data store context.
** [f_pbBlob1]              -- Pointer to a buffer of the first BLOB.
** [f_cbBlob1]              -- Size of the buffer of the first BLOB.
** [f_pbBlob2]              -- Optional pointer to a buffer of the second BLOB.
** [f_cbBlob2]              -- Size of the buffer of the second BLOB. If
**                             the second BLOB does not exist, the value should
**                             be 0.
** [f_pfResult]             -- Pointer to a variable to receive the result
**                             of verification.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note: If any one of f_poBBXContext, f_poSecStoreContext and f_poDatastoreHDS
**       is NULL, the function just exits.
**
**********************************************************************/
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
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE rgbPassword[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ] = { 0 };
    DRM_BYTE rgbHashValue[ __CB_DECL( DRM_ID_SIZE ) ] = { 0 };
    DRM_DWORD dwSlotData = 0;
    DRM_DWORD cbSlotData = SIZEOF( DRM_DWORD );

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BLOBCACHE, PERF_FUNC_DRM_BLOBCACHE_Verify );

    ChkArg( f_pbBlob1 != NULL && f_cbBlob1 > 0 );
    ChkArg( ( f_pbBlob2 != NULL && f_cbBlob2 > 0 )
         || ( f_pbBlob2 == NULL && f_cbBlob2 == 0 ) );
    ChkArg( f_pfResult != NULL );

    *f_pfResult = FALSE;

    if ( f_poBBXContext == NULL
      || f_poSecStoreContext == NULL
      || f_poDatastoreHDS == NULL )
    {
        goto ErrorExit;
    }

    /* Calculate a hash of the BLOBs being passed in. */
    ChkDR( _CalculateBlobHash( f_poBBXContext,
                               f_pbBlob1,
                               f_cbBlob1,
                               f_pbBlob2,
                               f_cbBlob2,
                               rgbPassword,
                               rgbHashValue ) );

    /*
    ** Try to read data from a secure store slot using the hash as the primary
    ** key.
    */
    dr = DRM_SST_GetData( f_poSecStoreContext,
                          ( DRM_ID * )rgbHashValue,
                          &g_idBlobCache,
                          rgbPassword,
                          SECURE_STORE_CACHED_BLOB_DATA,
                          f_poDatastoreHDS,
                          ( DRM_BYTE * )&dwSlotData,
                          &cbSlotData );

    if ( dr == DRM_E_DST_SLOT_NOT_FOUND || dr == DRM_E_BUFFERTOOSMALL )
    {
        /*
        ** It is OK that the slot is not found or in size other than
        ** the size of a DWORD.
        **/
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( dr ) ;

        if ( cbSlotData == SIZEOF( DRM_DWORD ) && dwSlotData == 1 )
        {
            /*
            ** If the slot size is of a DWORD and the value is 1,
            ** we got a match.
            */
            *f_pfResult = TRUE;
        }
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_BLOBCACHE_Update
**
** Synopsis: Function that updates the hash of of one or two BLOBs
**           in the secure store. Next time when the same set of
**           BLOBs is verified the result should be TRUE.
**
** Arguments:
**
** [f_poBBXContext]         -- Optional pointer to a black box context.
** [f_poSecStoreContext]    -- Optional pointer to a secure store context.
** [f_poDatastoreHDS]       -- Optional pointer to a data store context.
** [f_pbBlob1]              -- Pointer to a buffer of the first BLOB.
** [f_cbBlob1]              -- Size of the buffer of the first BLOB.
** [f_pbBlob2]              -- Optional pointer to a buffer of the second BLOB.
** [f_cbBlob2]              -- Size of the buffer of the second BLOB. If
**                             the second BLOB does not exist, the value should
**                             be 0.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note: If any one of f_poBBXContext, f_poSecStoreContext and f_poDatastoreHDS
**       is NULL, the function just exits.
**
**********************************************************************/
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
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE rgbPassword[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ] = { 0 };
    DRM_BYTE rgbHashValue[ __CB_DECL( DRM_ID_SIZE) ] = { 0 };
    DRM_DWORD dwSlotData = 1;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BLOBCACHE, PERF_FUNC_DRM_BLOBCACHE_Update );

    ChkArg( f_pbBlob1 != NULL && f_cbBlob1 > 0 );
    ChkArg( ( f_pbBlob2 != NULL && f_cbBlob2 > 0 )
         || ( f_pbBlob2 == NULL && f_cbBlob2 == 0 ) );
    ChkArg( f_poBBXContext      != NULL );
    ChkArg( f_poSecStoreContext != NULL );
    ChkArg( f_poDatastoreHDS    != NULL );

    /* Calculate a hash over the BLOBs being passed in. */
    ChkDR( _CalculateBlobHash( f_poBBXContext,
                               f_pbBlob1,
                               f_cbBlob1,
                               f_pbBlob2,
                               f_cbBlob2,
                               rgbPassword,
                               rgbHashValue ) );

    /*
    ** Update the secure store slot using the hash as the primary
    ** key.
    */
    ChkDR( DRM_SST_SetData( f_poSecStoreContext,
                            ( DRM_ID * )rgbHashValue,
                            &g_idBlobCache,
                            rgbPassword,
                            SECURE_STORE_CACHED_BLOB_DATA,
                            f_poDatastoreHDS,
                            ( DRM_BYTE * )&dwSlotData,
                            SIZEOF( DRM_DWORD ) ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
