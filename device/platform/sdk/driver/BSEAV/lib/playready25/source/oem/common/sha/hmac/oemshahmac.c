/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMSHAHMAC_C
#include <oemshahmac.h>
#include <oem.h>
#include <drmerr.h>
#include <drmprofile.h>
#include <drmbytemanip.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*********************************************************************
**
**  Function:  HMACInit
**
**  Synopsis:  HMAC is a keyed-hash message authentication code.  It creates a
**             unique hash of data based on the data and an arbitrary key.
**
**  Arguments:
**     [f_pContextHMAC] : Context structure to contain a running HMAC calculation.
**     [f_eAlgorithm]   : Specifies the SHA algorithm to perform HMAC over
**     [f_pbKey]        : Pointer a array of bytes to as the HMAC key.
**     [f_cbKey]        : Length of pbKey in bytes.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument is NULL or invalid
**
*********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HMAC_Init(
    __inout_ecount( 1 )          DRM_HMAC_CONTEXT              *f_pContextHMAC,
    __in                         eDRM_SHA_SUPPORTED_ALGORITHMS  f_eShaAlgorithm,
    __in_bcount( f_cbKey ) const DRM_BYTE                      *f_pbKey,
    __in                         DRM_DWORD                      f_cbKey)
{
    DRM_RESULT dr             = DRM_SUCCESS;
    DRM_DWORD  cbShaBlockSize = 0;
    DRM_DWORD  iCount         = 0;

    ChkArg( f_pContextHMAC != NULL );
    ChkArg( f_cbKey        != 0 );
    ChkArg( f_pbKey        != NULL);

    f_pContextHMAC->fInitialized = FALSE;

    switch( f_eShaAlgorithm )
    {
    case eDRM_SHA_1:
        cbShaBlockSize = DRM_SHA1_BLOCK_SIZE;
        break;
    case eDRM_SHA_256:
        cbShaBlockSize = SHA256_BLOCK_SIZE_IN_BYTES;
        break;
    default:
        ChkArg( FALSE );
        break;
    }

    /*
    ** Only use the first SHA_BLOCK_SIZE bytes
    */
    if( f_cbKey > cbShaBlockSize )
    {
        f_cbKey = cbShaBlockSize;
    }

    ZEROMEM( f_pContextHMAC, SIZEOF( *f_pContextHMAC ) );

    for( iCount = 0; iCount < f_cbKey; iCount++ )
    {
        PUT_BYTE( f_pContextHMAC->rgbBuffer, iCount, (DRM_BYTE)( GET_BYTE( f_pbKey, iCount ) ^ 0x36 ) );
    }
    for( ; iCount < cbShaBlockSize ; iCount++ )
    {
        PUT_BYTE( f_pContextHMAC->rgbBuffer, iCount, 0 ^ 0x36 );
    }

    ChkDR( DRM_SHA_Init( &( f_pContextHMAC->shaContext ), f_eShaAlgorithm ) );
    ChkDR( DRM_SHA_Update( f_pContextHMAC->rgbBuffer, cbShaBlockSize, &( f_pContextHMAC->shaContext ) ) );

    /*
    ** Update the internal context buffer so we don't have to hold onto the key.
    */
    for( iCount = 0; iCount < f_cbKey; iCount++ )
    {
        PUT_BYTE( f_pContextHMAC->rgbBuffer, iCount, (DRM_BYTE)( GET_BYTE( f_pbKey, iCount ) ^ 0x5C ) );
    }
    for( ; iCount < cbShaBlockSize ; iCount++ )
    {
        PUT_BYTE( f_pContextHMAC->rgbBuffer, iCount, 0 ^ 0x5C );
    }

    f_pContextHMAC->fInitialized = TRUE;

ErrorExit:
    return dr;
}

/*********************************************************************
**
**  Function:  DRM_HMAC_UpdateOffset
**
**  Synopsis:  Update the HMAC with the specified data, using an
**             offset (index)
**
**  Arguments:
**     [f_pContextHMAC] : Valid DRM_HMAC_CONTEXT create with DRM_HMAC_Init
**     [f_pbData]       : Data to add the the secure hash
**     [f_ibData]       : Offset of data buffer from f_pbData
**     [f_cbData]       : Length of pbData in bytes
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument is NULL or invalid
**
*********************************************************************/
DRM_API DRMFORCEINLINE DRM_RESULT DRM_CALL DRM_HMAC_UpdateOffset(
    __inout_ecount( 1 )                 DRM_HMAC_CONTEXT  *f_pContextHMAC,
    __in_bcount( f_cbData + f_ibData )  const DRM_BYTE    *f_pbData,
    __in                                DRM_DWORD          f_ibData,
    __in                                DRM_DWORD          f_cbData  )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pContextHMAC != NULL );
    ChkArg( f_pContextHMAC->fInitialized );

    if( f_cbData > 0 )
    {
        ChkArg( f_pbData != NULL );
        ChkDR( DRM_SHA_UpdateOffset( f_pbData,
                                     f_ibData,
                                     f_cbData,
                                    &( f_pContextHMAC->shaContext ) ) );
    }

ErrorExit:
    return dr;

}

/*********************************************************************
**
**  Function:  DRM_HMAC_Update
**
**  Synopsis:  Update the HMAC with the specified data
**
**  Arguments:
**     [f_pContextHMAC] : Valid DRM_HMAC_CONTEXT create with DRM_HMAC_Init
**     [f_pbData]       : Data to add the the secure hash
**     [f_cbData]       : Length of pbData in bytes
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument is NULL or invalid
**
*********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HMAC_Update(
    __inout_ecount( 1 )           DRM_HMAC_CONTEXT  *f_pContextHMAC,
    __in_bcount( f_cbData ) const DRM_BYTE          *f_pbData,
    __in                          DRM_DWORD          f_cbData  )
{
    return DRM_HMAC_UpdateOffset( f_pContextHMAC, f_pbData, 0, f_cbData );
}

/*********************************************************************
**
**  Function:  DRM_HMAC_Finalize
**
**  Synopsis:  Closes the MHAC context and returns the computed secure hash.
**
**  Arguments:
**     [f_pContextHMAC] : Valid DRM_HMAC_CONTEXT created with DRM_HMAC_Init
**     [f_pbKeyedHash]  : Pointer to byte array that will recieve the secure hash
**     [f_cbKeyedHash]  : Length of pbKeyedHash in bytes
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument is NULL or invalid
**
**  Notes:  cbKeyedHash must be less than or equal to SHA_DIGEST_LEN and greater than 0.
**
*********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HMAC_Finalize(
    __inout_ecount( 1 )           DRM_HMAC_CONTEXT  *f_pContextHMAC,
    __out_bcount( f_cbKeyedHash ) DRM_BYTE          *f_pbKeyedHash,
    __in                          DRM_DWORD          f_cbKeyedHash )
{
    return DRM_HMAC_FinalizeOffset( f_pContextHMAC, f_pbKeyedHash, 0, f_cbKeyedHash );
}

/*********************************************************************
**
**  Function:  DRM_HMAC_FinalizeOffset
**
**  Synopsis:  Closes the MHAC context and returns the computed secure hash,
**             using an offset (index)
**
**  Arguments:
**     [f_pContextHMAC] : Valid DRM_HMAC_CONTEXT created with DRM_HMAC_Init
**     [f_pbKeyedHash]  : Pointer to byte array that will recieve the secure hash
**     [f_ibKeyedHash]  : Offset of keyed hash buffer from pbKeyedHash
**     [f_cbKeyedHash]  : Length of pbKeyedHash in bytes
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument is NULL or invalid
**
**  Notes:  cbKeyedHash must be less than or equal to SHA_DIGEST_LEN and greater than 0.
**
*********************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER, "f_pbKeyedHash length defined by f_cbKeyedHash and f_ibKeyedHash" )
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HMAC_FinalizeOffset(
    __inout_ecount( 1 )                             DRM_HMAC_CONTEXT  *f_pContextHMAC,
    __out_bcount( f_cbKeyedHash + f_ibKeyedHash )   DRM_BYTE          *f_pbKeyedHash,
    __in                                            DRM_DWORD          f_ibKeyedHash,
    __in                                            DRM_DWORD          f_cbKeyedHash )
PREFAST_POP
{
    DRM_RESULT dr             = DRM_SUCCESS;
    DRM_DWORD  cbShaDigestLen = 0;
    DRM_DWORD  cbShaBlockSize = 0;

    ChkArg( f_pContextHMAC != NULL );
    ChkArg( f_pContextHMAC->fInitialized );
    ChkArg( f_pbKeyedHash  != NULL );
    ChkArg( f_cbKeyedHash  != 0 );

    switch( f_pContextHMAC->shaContext.eAlgorithm )
    {
    case eDRM_SHA_1:
        cbShaDigestLen = DRM_SHA1_DIGEST_LEN;
        cbShaBlockSize = DRM_SHA1_BLOCK_SIZE;
        f_cbKeyedHash = min( f_cbKeyedHash, ( DRM_DWORD )DRM_SHA1_DIGEST_LEN );
        break;
    case eDRM_SHA_256:
        cbShaDigestLen = SHA256_DIGEST_SIZE_IN_BYTES;
        cbShaBlockSize = SHA256_BLOCK_SIZE_IN_BYTES;
        f_cbKeyedHash = min( f_cbKeyedHash, ( DRM_DWORD )SHA256_DIGEST_SIZE_IN_BYTES );
        break;
    default:
        ChkArg( FALSE );
        break;
    }

    /*
    ** Finish the first SHA operation.
    */
    ChkDR( DRM_SHA_Finalize( &( f_pContextHMAC->shaContext ), cbShaDigestLen, f_pContextHMAC->shaDigest ) );

    /*
    ** Start second SHA operation
    */
    ChkDR( DRM_SHA_Init( &( f_pContextHMAC->shaContext ), f_pContextHMAC->shaContext.eAlgorithm ) );
    ChkDR( DRM_SHA_Update( f_pContextHMAC->rgbBuffer, cbShaBlockSize, &( f_pContextHMAC->shaContext ) ) );
    ChkDR( DRM_SHA_Update( f_pContextHMAC->shaDigest, cbShaDigestLen, &( f_pContextHMAC->shaContext ) ) );
    ChkDR( DRM_SHA_Finalize( &( f_pContextHMAC->shaContext ), cbShaDigestLen, f_pContextHMAC->shaDigest ) );

    OEM_SECURE_MEMCPY( f_pbKeyedHash + f_ibKeyedHash, f_pContextHMAC->shaDigest, f_cbKeyedHash );

ErrorExit:
    if ( f_pContextHMAC != NULL )
    {
        OEM_SECURE_ZERO_MEMORY( f_pContextHMAC->rgbBuffer, SIZEOF(f_pContextHMAC->rgbBuffer) );
    }
    return dr;

}


/*********************************************************************
**
**  Function:  DRM_HMAC_CreateMAC
**
**  Synopsis:  Creates a MAC using a key provided by the caller.
**
**  Arguments:
**     [f_eAlgorithm] : Specifies the SHA algorithm to perform HMAC over
**     [f_pbKey]      : Key to create MAC
**     [f_cbKey]      : Length of pbKey
**     [f_pbData]     : Data to create the MAC over
**     [f_ibData]     : Offset of data buffer from f_pbData
**     [f_cbData]     : Length of f_pbData in DRM_BYTEs
**     [f_cbMAC]      : Length of f_pbMAC in DRM_BYTEs
**     [f_ibMAC]      : Offset of MAC buffer from f_pbData
**     [f_pbMAC]      : Buffer where the signature will be returned.
**
**  Notes:  Use DRM_HMAC_VerifyMAC to verify a signature created with DRM_HMAC_CreateMAC.
**
*********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_HMAC_CreateMAC(
    __in                                      eDRM_SHA_SUPPORTED_ALGORITHMS  f_eAlgorithm,
    __in_bcount( f_cbKey )              const DRM_BYTE                      *f_pbKey,
    __in                                      DRM_DWORD                      f_cbKey,
    __in_bcount( f_cbData + f_ibData )  const DRM_BYTE                      *f_pbData,
    __in                                      DRM_DWORD                      f_ibData,
    __in                                      DRM_DWORD                      f_cbData,
    __out_bcount( f_cbMAC + f_ibMAC )         DRM_BYTE                      *f_pbMAC,
    __in                                      DRM_DWORD                      f_ibMAC,
    __in                                      DRM_DWORD                      f_cbMAC )
{
    DRM_RESULT       dr    = DRM_SUCCESS;
    DRM_HMAC_CONTEXT hmac  = DRM_HMAC_CONTEXT_DEFAULT;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMHMAC, PERF_FUNC_DRM_HMAC_CreateMAC );

    ChkArg( f_cbData > 0 );
    ChkOverflow( f_cbData + f_ibData, f_cbData );

    ChkDR( DRM_HMAC_Init( &hmac, f_eAlgorithm, f_pbKey, f_cbKey ) );
    ChkDR( DRM_HMAC_UpdateOffset( &hmac, f_pbData, f_ibData, f_cbData ) );
    ChkDR( DRM_HMAC_FinalizeOffset( &hmac, f_pbMAC, f_ibMAC, f_cbMAC ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*********************************************************************
**
**  Function:  DRM_HMAC_VerifyMAC
**
**  Synopsis:  Verifies a MAC created with DRM_HMAC_CreateMAC.
**
**  Arguments:
**     [f_eAlgorithm] : Specifies the SHA algorithm to perform HMAC over
**     [f_pbKey]      : Key used to create MAC
**     [f_cbKey]      : Length of pbKey
**     [f_pbData]     : Data to verify
**     [f_ibData]     : Offset of data buffer from f_pbData
**     [f_cbData]     : Length of f_pbData in DRM_BYTEs
**     [f_cbMAC]      : Length of f_pbMAC in DRM_BYTEs
**     [f_ibMAC]      : Offset of MAC buffer from f_pbData
**     [f_pbMAC]      : Buffer containing the signature to verify
**
**  Notes:  Use DRM_HMAC_CreateMAC to create a MAC
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_HMAC_VerifyMAC(
    __in                                      eDRM_SHA_SUPPORTED_ALGORITHMS  f_eAlgorithm,
    __in_bcount( f_cbKey )              const DRM_BYTE                      *f_pbKey,
    __in                                      DRM_DWORD                      f_cbKey,
    __in_bcount( f_cbData + f_ibData )  const DRM_BYTE                      *f_pbData,
    __in                                      DRM_DWORD                      f_ibData,
    __in                                      DRM_DWORD                      f_cbData,
    __in_bcount( f_cbMAC + f_ibMAC )    const DRM_BYTE                      *f_pbMAC,
    __in                                      DRM_DWORD                      f_ibMAC,
    __in                                      DRM_DWORD                      f_cbMAC )
{
    DRM_RESULT       dr   = DRM_SUCCESS;
    DRM_BYTE         rgbMAC[__CB_DECL( DRM_SHA_DIGEST_LEN_MAX )] = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMHMAC, PERF_FUNC_DRM_HMAC_VerifyMAC );

    ChkBOOL( f_cbMAC <= DRM_SHA_DIGEST_LEN_MAX, DRM_E_INVALID_SIGNATURE );
    ChkArg( f_cbData > 0 );

    ChkDR( DRM_HMAC_CreateMAC( f_eAlgorithm,
                               f_pbKey,
                               f_cbKey,
                               f_pbData,
                               f_ibData,
                               f_cbData,
                               rgbMAC,
                               0,
                               f_cbMAC ) );

    ChkBOOL( DRM_BYT_CompareBytes( rgbMAC, 0, f_pbMAC, f_ibMAC, f_cbMAC ) == 0,
             DRM_E_INVALID_SIGNATURE );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


EXIT_PK_NAMESPACE_CODE;
