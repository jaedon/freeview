/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <oemshahash.h>
#include <drmcommon.h>

ENTER_PK_NAMESPACE_CODE;


/*********************************************************************
**
**  Function:  DRM_SHA_Init
**
**  Synopsis:  Start a clean new instance of Sha
**
**  Arguments:
**     [f_pShaContext] : Pointer to context to hold current state of sha
**     [f_eAlgorithm]  : Enum specifying the hash algorithm to initialize
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument is NULL or invalid
**
*********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SHA_Init(
    __inout_ecount( 1 ) DRM_SHA_CONTEXT               *f_pShaContext,
    __in                eDRM_SHA_SUPPORTED_ALGORITHMS  f_eAlgorithm )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pShaContext != NULL );

    f_pShaContext->fInitialized = FALSE;

    switch( f_eAlgorithm )
    {
    case eDRM_SHA_1:
        ChkDR( DRM_SHA1_Init( &f_pShaContext->context.sha1Context ) );
        break;
    case eDRM_SHA_256: /* Fall through */
    case eDRM_SHA_256_With_SHA_1_Size:
        ChkDR( DRM_SHA256_Init( &f_pShaContext->context.sha256Context ) );
        break;
    default:
        ChkArg( FALSE );
        break;
    }
    f_pShaContext->eAlgorithm = f_eAlgorithm;

    f_pShaContext->fInitialized = TRUE;

ErrorExit:
    return dr;
}


/*********************************************************************
**
**  Function:  DRM_SHA_Update
**
**  Synopsis:  Update a SHA context with an array of bytes
**
**  Arguments:
**     [f_pbData]      : Array of bytes to add to the sha computation
**     [f_cbData]      : Length of pbData in bytes
**     [f_pShaContext] : Running sha context
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument is NULL or invalid
**
*********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SHA_Update(
    __in_bcount( f_cbData )const DRM_BYTE         *f_pbData,
    __in                         DRM_DWORD         f_cbData,
    __inout_ecount( 1 )          DRM_SHA_CONTEXT  *f_pShaContext )
{
    return DRM_SHA_UpdateOffset( f_pbData, 0, f_cbData, f_pShaContext );
}


/*********************************************************************
**
**  Function:  DRM_SHA_UpdateOffset
**
**  Synopsis:  Update a SHA context with an array of bytes and offset
**
**  Arguments:
**     [f_pbData]      : Array of bytes to add to the sha computation
**     [f_ibData]      : Index into pbData to start
**     [f_cbData]      : Length of pbData in bytes
**     [f_pShaContext] : Running sha context
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument is NULL or invalid
**
*********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SHA_UpdateOffset(
    __in_bcount( f_cbData ) const DRM_BYTE         *f_pbData,
    __in                          DRM_DWORD         f_ibData,
    __in                          DRM_DWORD         f_cbData,
    __inout_ecount( 1 )           DRM_SHA_CONTEXT  *f_pShaContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pShaContext != NULL );
    ChkArg( f_pShaContext->fInitialized );

    switch( f_pShaContext->eAlgorithm )
    {
    case eDRM_SHA_1:
        ChkDR( DRM_SHA1_UpdateOffset( f_pbData,
                                      f_ibData,
                                      f_cbData,
                                     &f_pShaContext->context.sha1Context ) );
        break;
    case eDRM_SHA_256: /* Fall through */
    case eDRM_SHA_256_With_SHA_1_Size:
        ChkDR( DRM_SHA256_UpdateOffset( &f_pShaContext->context.sha256Context,
                                         f_pbData,
                                         f_cbData,
                                         f_ibData ) );
        break;
    default:
        ChkDR( DRM_E_UNSUPPORTEDALGORITHM );
        break;
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
**  Function:  DRM_SHA_Finalize
**
**  Synopsis:  Finalize a SHA algorithm
**
**  Arguments:
**     [f_pShaContext] : Running sha context
**     [f_cbDigest]    : Length of f_pbDigest
**     [f_pbDigest]    : Resultant sha digest from sha operation
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument is NULL or invalid
**      DRM_E_BUFFERTOOSMALL
**          f_pbDigest is not large enough for the digest
**
*********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_SHA_Finalize(
    __in_ecount( 1 )            DRM_SHA_CONTEXT *f_pShaContext,
    __in                        DRM_DWORD        f_cbDigest,
    __inout_bcount( f_cbDigest) DRM_BYTE        *f_pbDigest )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pShaContext != NULL );
    ChkArg( f_pShaContext->fInitialized );

    switch( f_pShaContext->eAlgorithm )
    {
    case eDRM_SHA_1:
        ChkBOOL( f_cbDigest >= DRM_SHA1_DIGEST_LEN, DRM_E_BUFFERTOOSMALL );
        ChkDR( DRM_SHA1_Finalize( &f_pShaContext->context.sha1Context,
                                   f_pbDigest ) );
        break;
    case eDRM_SHA_256:
        ChkBOOL( f_cbDigest >= SIZEOF( DRM_SHA256_Digest ), DRM_E_BUFFERTOOSMALL );
        ChkDR( DRM_SHA256_Finalize( &f_pShaContext->context.sha256Context,
                                    ( DRM_SHA256_Digest * )f_pbDigest ) );
        break;
    case eDRM_SHA_256_With_SHA_1_Size:
        ChkBOOL( f_cbDigest >= DRM_SHA1_DIGEST_LEN, DRM_E_BUFFERTOOSMALL );
        {
            DRM_DWORD idx                                  = 0;
            DRM_BYTE  rgbHash[SHA256_DIGEST_SIZE_IN_BYTES] = {0};
            ChkDR( DRM_SHA256_Finalize( &f_pShaContext->context.sha256Context,
                                        ( DRM_SHA256_Digest * )rgbHash ) );

            /*
            ** XOR the last 12 bytes with the begining 12 bytes of hash to fold in
            ** the 32 byte hash to a 20 byte hash.
            */
            MEMCPY( f_pbDigest, rgbHash, DRM_SHA1_DIGEST_LEN );

            for( idx = 0; idx < ( SHA256_DIGEST_SIZE_IN_BYTES - DRM_SHA1_DIGEST_LEN ); idx++ )
            {
                f_pbDigest[idx] ^= rgbHash[DRM_SHA1_DIGEST_LEN + idx];
            }
        }
        break;
    default:
        ChkArg( FALSE );
        break;
    }

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

