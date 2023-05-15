/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
 * oemrsaoaeppssimpl.c
 *
 * This file implements RSA signing and encryption of variable length data.
 * It assumes the existence of routines to encrypt a single key sized buffer
 *
 */

#include <drmcommon.h>
#include <oemrsaoaeppssimpl.h>
#include <oemshahash.h>
#include <drmutilities.h>
#include <byteorder.h>
#include <oem.h>

ENTER_PK_NAMESPACE_CODE;

#ifndef RSA_REVERSE_OS2IP
#define RSA_REVERSE_OS2IP 0
#endif

/*********************************************************************
**
**  Function:  _GenerateMGF1Mask
**
**  Synopsis:  Generates an MGF1 mask using SHA1 as the hash ( see PKCS#1 v2.1 )
**
**  Arguments:
**     [f_pbSeed]   :  Specifies the seed to use for the mask.
**                     Must be the length specified in f_cbSeed
**     [f_cbSeed]   :  Specifies the size of the seed
**     [f_cbMask]   :  Specifies the size of the mask that is required
**                     Must be the length specified in f_cbMask
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**      DRM_E_ARITHMETIC_OVERFLOW
**          An overflow/underflow occurred while processing the data
*********************************************************************/
static DRM_NO_INLINE DRM_RESULT _GenerateMGF1Mask(
    __in const                    DRM_RSA_SUPPORTED_KEY_LENGTHS  f_eKeyLength,
    __in const                    DRM_DWORD                      f_cbSeed,
    __in_ecount( f_cbSeed ) const DRM_BYTE                      *f_pbSeed,
    __in const                    DRM_DWORD                      f_cbMask,
    __out_bcount( f_cbMask )      DRM_BYTE                      *f_pbMask
                                                   ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _GenerateMGF1Mask(
    __in const                    DRM_RSA_SUPPORTED_KEY_LENGTHS  f_eKeyLength,
    __in const                    DRM_DWORD                      f_cbSeed,
    __in_ecount( f_cbSeed ) const DRM_BYTE                      *f_pbSeed,
    __in const                    DRM_DWORD                      f_cbMask,
    __out_bcount( f_cbMask )      DRM_BYTE                      *f_pbMask
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT      dr           = DRM_SUCCESS;
    DRM_DWORD       cIterations  = 0;
    DRM_DWORD       i            = 0;
    DRM_DWORD       ibHashOut    = 0;
    DRM_SHA_CONTEXT contextSHA   = DRM_SHA_CONTEXT_DEFAULT;
    DRM_BYTE        rgbHashInput[__CB_DECL( MGF1_CB_MAX_SEED_MAX + SIZEOF( DRM_DWORD ) )] = { 0 };
    DRM_BYTE        rgbTempMask[__CB_DECL( MGF1_CB_MAX_MASK_MAX )]                        = { 0 };
    const DRM_DWORD cbHashInput = f_cbSeed + SIZEOF( DRM_DWORD );

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_RSA, PERF_FUNC__GenerateMGF1Mask );

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( f_eKeyLength ) );
    ChkArg( 0 < f_cbSeed );
    ChkArg( MGF1_CB_MAX_SEED( (DRM_DWORD)f_eKeyLength ) >= f_cbSeed );
    ChkArg( NULL != f_pbSeed );
    ChkArg( 0 < f_cbMask );
    ChkArg( MGF1_CB_MAX_MASK( (DRM_DWORD)f_eKeyLength ) >= f_cbMask );
    ChkArg( NULL != f_pbMask );

    ChkOverflow( cbHashInput, f_cbSeed );

    cIterations = f_cbMask / DRM_SHA1_DIGEST_LEN;

    if ( f_cbMask % DRM_SHA1_DIGEST_LEN > 0 )
    {
        cIterations++;
    }
    DRMASSERT( cIterations * DRM_SHA1_DIGEST_LEN <= MGF1_CB_MAX_MASK( (DRM_DWORD)f_eKeyLength ) );

    OEM_SECURE_MEMCPY( rgbHashInput, f_pbSeed, f_cbSeed );

    for ( i = 0; i < cIterations; i++ )
    {
        ChkDR( DRM_SHA_Init( &contextSHA, eDRM_SHA_1 ) );

        DWORD_TO_NETWORKBYTES( rgbHashInput, f_cbSeed, i );

        ChkDR( DRM_SHA_Update( rgbHashInput, cbHashInput, &contextSHA ) );
        ChkDR( DRM_SHA_Finalize( &contextSHA, DRM_SHA1_DIGEST_LEN, &( rgbTempMask[ibHashOut] ) ) );

        ibHashOut += DRM_SHA1_DIGEST_LEN;
    }
    OEM_SECURE_MEMCPY( f_pbMask, rgbTempMask, f_cbMask );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/*********************************************************************
**
**  Function:  _OAEPDecode
**
**  Synopsis:  performs the OAEP decode on an encoded buffer ( see PKCS#1 v2.1 )
**
**  Arguments:
**     [f_rgbEncData]  :  Specifies the encoded data
**     [f_rgbDecData]  :  Returns the decoded data.
**     [f_pcbDecData]  :  Returns the size of the decoded data
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pcbDecData is NULL
**      DRM_E_ARITHMETIC_OVERFLOW
**          An overflow/underflow occurred while processing the data
**      DRM_E_RSA_DECRYPTION_ERROR
**          The DB/EM used in OAEP decoding is not correct
**
*********************************************************************/
static DRM_RESULT _OAEPDecode(
    __in const                       DRM_RSA_SUPPORTED_KEY_LENGTHS  f_eKeyLength,
    __in const                       DRM_DWORD                      f_cbEncData,
    __in_ecount( f_cbEncData ) const DRM_BYTE                      *f_pbEncData,
    __inout                          DRM_DWORD                     *f_pcbDecData,
     __out_ecount( *f_pcbDecData )   DRM_BYTE                      *f_pbDecData
    )
{
    DRM_RESULT      dr          = DRM_SUCCESS;
    DRM_DWORD       ibCur       = DRM_SHA1_DIGEST_LEN;
    DRM_SHA_CONTEXT contextSHA  = DRM_SHA_CONTEXT_DEFAULT;
    DRM_BYTE        rgbLHash[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ]         = { 0 };
    DRM_BYTE        rgbSeed[__CB_DECL( DRM_SHA1_DIGEST_LEN )]            = { 0 };
    DRM_BYTE        rgbSeedMask[__CB_DECL( DRM_SHA1_DIGEST_LEN )]        = { 0 };
    DRM_BYTE        rgbDataBlock[__CB_DECL( OAEP_CB_DATABLOCK_MAX )]     = { 0 };
    DRM_BYTE        rgbDataBlockMask[__CB_DECL( OAEP_CB_DATABLOCK_MAX )] = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_RSA, PERF_FUNC__OAEPDecode );

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( f_eKeyLength ) );
    ChkArg( OAEP_CB_ENCRYPTED_MSG( (DRM_DWORD)f_eKeyLength ) == f_cbEncData );
    ChkArg( NULL != f_pbEncData );
    ChkArg( NULL != f_pcbDecData );
    if ( DRM_RSA_CB_MAX_PLAINTEXT( (DRM_DWORD)f_eKeyLength ) > *f_pcbDecData )
    {
        *f_pcbDecData = DRM_RSA_CB_MAX_PLAINTEXT( f_eKeyLength );
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    ChkArg( NULL != f_pbDecData );

    /*
    ** First byte of the EM must be 0
    */
    ChkBOOL( GET_BYTE( f_pbEncData, OAEP_IB_EM_START ) == OAEP_B_EM_START, DRM_E_RSA_DECRYPTION_ERROR );

    /*
    ** Set up seed buffer
    */
    DRM_BYT_CopyBytes( rgbSeed, 0, f_pbEncData, OAEP_IB_SEED, DRM_SHA1_DIGEST_LEN );

    /*
    ** Set up data block buffer
    */
    DRM_BYT_CopyBytes( rgbDataBlock, 0, f_pbEncData, OAEP_IB_DATABLOCK, OAEP_CB_DATABLOCK( f_eKeyLength ) );

    /*
    ** Compute the seed mask and decode the seed
    */
    ChkDR( _GenerateMGF1Mask( f_eKeyLength, OAEP_CB_DATABLOCK( f_eKeyLength ), rgbDataBlock, SIZEOF( rgbSeedMask ), rgbSeedMask ) );
    DRM_XOR( rgbSeed, rgbSeedMask, SIZEOF( rgbSeed ) );

    /*
    ** Compute data block mask from the seed and decode DB
    */
    ChkDR( _GenerateMGF1Mask( f_eKeyLength, SIZEOF( rgbSeed ), rgbSeed, OAEP_CB_DATABLOCK( f_eKeyLength ), rgbDataBlockMask ) );
    DRM_XOR( rgbDataBlock, rgbDataBlockMask, OAEP_CB_DATABLOCK( f_eKeyLength ) );

    /*
    ** Compute LHash for empty label and verify that DB starts with LHash
    */
    ChkDR( DRM_SHA_Init( &contextSHA, eDRM_SHA_1 ) );
    ChkDR( DRM_SHA_Finalize( &contextSHA, SIZEOF( rgbLHash ), rgbLHash ) );
    ChkBOOL( MEMCMP( rgbLHash, rgbDataBlock, SIZEOF( rgbLHash ) ) == 0, DRM_E_RSA_DECRYPTION_ERROR );

    /*
    ** Locate a 0x01 value byte to indicate start of original message
    */
    while( ( ibCur < DRM_RSA_CB_MODULUS( (DRM_DWORD)f_eKeyLength ) )
        && ( GET_BYTE( rgbDataBlock, ibCur ) == 0x00 ) )
    {
        ibCur++;
    }
    ChkBOOL( ibCur != DRM_RSA_CB_MODULUS( (DRM_DWORD)f_eKeyLength ), DRM_E_RSA_DECRYPTION_ERROR );
    ChkBOOL( GET_BYTE( rgbDataBlock, ibCur ) == OAEP_B_MSG_START, DRM_E_RSA_DECRYPTION_ERROR );

    /*
    ** Finally, output the decoded message
    */
    ibCur++;
    ChkOverflow( (DRM_DWORD)DRM_RSA_CB_MODULUS( f_eKeyLength ), ibCur );
    ChkOverflow( (DRM_DWORD)DRM_RSA_CB_MODULUS( f_eKeyLength ) - ibCur, (DRM_DWORD)OAEP_IB_DATABLOCK );
    *f_pcbDecData = ( DRM_RSA_CB_MODULUS( f_eKeyLength ) - ibCur ) - OAEP_IB_DATABLOCK;
    DRM_BYT_CopyBytes( f_pbDecData, 0, rgbDataBlock, ibCur, *f_pcbDecData );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/*********************************************************************
**
**  Function:  _OAEPEncode
**
**  Synopsis:  performs the OAEP encode on a buffer ( see PKCS#1 v2.1 )
**
**  Arguments:
**     [f_pbData]     :  Specifies the data to encode
**     [f_cbData]     :  Specifies the length of the data to encode
**     [f_rgbEncData] :  Returns the encoded data
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pbData is NULL of f_cbData is out of range
**
*********************************************************************/
static DRM_NO_INLINE DRM_RESULT _OAEPEncode(
    __in const                    DRM_RSA_SUPPORTED_KEY_LENGTHS  f_eKeyLength,
    __in_opt                      DRM_VOID                      *f_pOEMContext,
    __in const                    DRM_DWORD                      f_cbData,
    __in_ecount( f_cbData ) const DRM_BYTE                      *f_pbData,
    __inout                       DRM_DWORD                     *f_pcbEncData,
    __out_ecount( *f_pcbEncData ) DRM_BYTE                      *f_pbEncData
                                            ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _OAEPEncode(
    __in const                    DRM_RSA_SUPPORTED_KEY_LENGTHS  f_eKeyLength,
    __in_opt                      DRM_VOID                      *f_pOEMContext,
    __in const                    DRM_DWORD                      f_cbData,
    __in_ecount( f_cbData ) const DRM_BYTE                      *f_pbData,
    __inout                       DRM_DWORD                     *f_pcbEncData,
    __out_ecount( *f_pcbEncData ) DRM_BYTE                      *f_pbEncData
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT      dr                = DRM_SUCCESS;
    DRM_SHA_CONTEXT contextSHA        = DRM_SHA_CONTEXT_DEFAULT;
    DRM_BYTE        rgbHash[__CB_DECL( OAEP_CB_DATABLOCK_HASH )]         = { 0 };
    DRM_BYTE        rgbSeed[__CB_DECL( OAEP_CB_SEED )]                   = { 0 };
    DRM_BYTE        rgbSeedMask[__CB_DECL( OAEP_CB_SEED )]               = { 0 };
    DRM_BYTE        rgbDataBlock[__CB_DECL( OAEP_CB_DATABLOCK_MAX )]     = { 0 };
    DRM_BYTE        rgbDataBlockMask[__CB_DECL( OAEP_CB_DATABLOCK_MAX )] = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_RSA, PERF_FUNC__OAEPEncode );

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( f_eKeyLength ) );
    ChkArg( 0 < f_cbData );
    ChkArg( OAEP_CB_DATABLOCK_MSG_MAX( (DRM_DWORD)f_eKeyLength ) >= f_cbData );
    ChkArg( NULL != f_pbData );
    ChkArg( NULL != f_pcbEncData );
    if ( OAEP_CB_ENCRYPTED_MSG( (DRM_DWORD)f_eKeyLength ) > *f_pcbEncData )
    {
        *f_pcbEncData = OAEP_CB_ENCRYPTED_MSG( f_eKeyLength );
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    ChkArg( NULL != f_pbEncData );

    /*
    ** Set first byte of EM to 0
    EM.EMStart = 0x00
    */
    PUT_BYTE( f_pbEncData, OAEP_IB_EM_START, OAEP_B_EM_START );

    /*
    ** Compute Hash of the empty Label
    ** DB.lHash = Hash( L )
    */
    /* Note This is a constant calculation */
    ChkDR( DRM_SHA_Init( &contextSHA, eDRM_SHA_1 ) );
    ChkDR( DRM_SHA_Finalize( &contextSHA, DRM_SHA1_DIGEST_LEN, rgbHash ) );
    DRM_BYT_CopyBytes( f_pbEncData, OAEP_IB_DATABLOCK_HASH, rgbHash, 0, OAEP_CB_DATABLOCK_HASH );

    /*
    ** Construct PS
    ** DB.PS = OAEP_CB_DATABLOCK_PS( f_eKeyLength, f_cbData ) zeros.
    */
    DRM_BYT_SetBytes( f_pbEncData, OAEP_IB_DATABLOCK_PS, OAEP_CB_DATABLOCK_PS( f_eKeyLength, f_cbData ), 0 );

    /*
    ** Write the 0x01 value byte to mark start of original message
    ** DB.MsgStart = 0x01
    */
    PUT_BYTE( f_pbEncData, OAEP_IB_DATABLOCK_MSG_START( f_eKeyLength, f_cbData ), OAEP_B_MSG_START );

    /*
    ** Copy the original message
    ** DB.M = original message
    */
    DRM_BYT_CopyBytes( f_pbEncData, OAEP_IB_DATABLOCK_MSG( f_eKeyLength, f_cbData ), f_pbData, 0, f_cbData );

    /*
    ** Generate the seed in temporary variable
    */
    ChkDR( Oem_Random_GetBytes( f_pOEMContext, rgbSeed, SIZEOF( rgbSeed ) ) );

    /*
    ** Compute the DB Mask from the seed and XOR it with DB
    */
    ChkDR( _GenerateMGF1Mask( f_eKeyLength, SIZEOF( rgbSeed ), rgbSeed, OAEP_CB_DATABLOCK( f_eKeyLength ), rgbDataBlockMask ) );
    DRM_BYT_CopyBytes( rgbDataBlock, 0, f_pbEncData, OAEP_IB_DATABLOCK, OAEP_CB_DATABLOCK( f_eKeyLength ) );
    DRM_XOR( rgbDataBlock, rgbDataBlockMask, OAEP_CB_DATABLOCK( f_eKeyLength ) );

    /*
    ** Next, compute the seed mask and XOR it with the seed
    */
    ChkDR( _GenerateMGF1Mask( f_eKeyLength, OAEP_CB_DATABLOCK( f_eKeyLength ), rgbDataBlock, SIZEOF( rgbSeedMask ), rgbSeedMask ) );
    DRM_XOR( rgbSeed, rgbSeedMask, SIZEOF( rgbSeed ) );
    DRM_BYT_CopyBytes( f_pbEncData, OAEP_IB_SEED, rgbSeed, 0, OAEP_CB_SEED );
    DRM_BYT_CopyBytes( f_pbEncData, OAEP_IB_DATABLOCK, rgbDataBlock, 0, OAEP_CB_DATABLOCK( f_eKeyLength ) );

    *f_pcbEncData = OAEP_CB_ENCRYPTED_MSG( f_eKeyLength );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}
/*********************************************************************
**
**  Function:  _PSSVerify
**
**  Synopsis:  Does PSS Verification (see PKCS #1 v2.1)
**
**  Arguments:
**     [f_pbData]      :  Specifies the original data
**     [f_cbData]      :  Specifies the length of the original data
**     [f_ibData]      :  Specifies the offset from the beginning of the original data
**     [f_pbEncoding]  :  Specifies the encoded/signed data
**     [f_cbEMBits]    :  Specifies the size of the encoded bits
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**      DRM_E_RSA_SIGNATURE_ERROR
**          Failed to verify the signature
**
*********************************************************************/
static DRM_NO_INLINE DRM_RESULT _PSSVerify(
    __in const                                 DRM_RSA_SUPPORTED_KEY_LENGTHS  f_eKeyLength,
    __in                                       DRM_DWORD                      f_cbData,
    __in_ecount( f_cbData ) const              DRM_BYTE                      *f_pbData,
    __in                                       DRM_DWORD                      f_ibData,
    __in                                       DRM_DWORD                      f_cbEMBits,
    __in_ecount( BITS_TO_BYTES( f_cbEMBits ) ) DRM_BYTE                      *f_pbEncoding
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT      dr           = DRM_SUCCESS;
    DRM_DWORD       cbEncodedMsg = f_cbEMBits / BITS_PER_STANDARD_BYTE;
    DRM_DWORD       cbDataBlock  = 0;
    DRM_DWORD       cZeroBits    = 0;
    DRM_DWORD       i            = 0;
    DRM_SHA_CONTEXT contextSHA   = DRM_SHA_CONTEXT_DEFAULT;
    DRM_BYTE        rgbMPrime[__CB_DECL( PSS_CB_MPRIME )]            = { 0 };
    DRM_BYTE        rgbMPrimeHash [__CB_DECL( DRM_SHA1_DIGEST_LEN )] = { 0 };
    DRM_BYTE        rgbDBMask[__CB_DECL( MGF1_CB_MAX_MASK_MAX )]     = { 0 };
#if SIXTEEN_BIT_ADDRESSING
    DRM_BYTE        rgbTemp[__CB_DECL( DRM_SHA1_DIGEST_LEN )]        = { 0 };
#endif /* SIXTEEN_BIT_ADDRESSING */

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_RSA, PERF_FUNC__PSSVerify );

    if( f_cbEMBits % BITS_PER_STANDARD_BYTE != 0 )
    {
        cbEncodedMsg++;
    }
    cZeroBits = ( BITS_PER_STANDARD_BYTE * cbEncodedMsg ) - f_cbEMBits;

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( f_eKeyLength ) );
    ChkArg( cbEncodedMsg >= DRM_SHA1_DIGEST_LEN + PSS_CB_SALT + 2 );
    ChkArg( cbEncodedMsg <= DRM_RSA_CB_MODULUS( (DRM_DWORD)f_eKeyLength ) );

    /*
    ** Verify last bit of EM
    */
    ChkBOOL( GET_BYTE( f_pbEncoding, cbEncodedMsg - 1 ) == PSS_B_END_MSG,
             DRM_E_RSA_SIGNATURE_ERROR );

    /*
    ** Compute the original message hash
    */
    ChkDR( DRM_SHA_Init( &contextSHA, eDRM_SHA_1 ) );
    ChkDR( DRM_SHA_UpdateOffset( f_pbData, f_ibData, f_cbData, &contextSHA ) );
    ChkDR( DRM_SHA_Finalize( &contextSHA, DRM_SHA1_DIGEST_LEN, &( rgbMPrime[PSS_CB_ZERO_PAD] ) ) );

    /*
    ** Set up DB and Hash( M' ) in EM
    */
    cbDataBlock = ( cbEncodedMsg - DRM_SHA1_DIGEST_LEN  ) - 1;

#if SIXTEEN_BIT_ADDRESSING
    DRM_BYT_CopyBytes( rgbTemp, 0, f_pbEncoding, cbDataBlock, DRM_SHA1_DIGEST_LEN );
#endif

    /*
    ** Check that the requisite leftmost bits of DB are 0
    */
    ChkBOOL( ( GET_BYTE( f_pbEncoding, 0 )
             & ( 0xFF << ( BITS_PER_STANDARD_BYTE - cZeroBits ) ) ) == 0,
             DRM_E_RSA_SIGNATURE_ERROR );

    /*
    ** Compute DBMask and extract DB
    */
#if SIXTEEN_BIT_ADDRESSING
    ChkDR( _GenerateMGF1Mask( f_eKeyLength,
                              rgbTemp,
                              DRM_SHA1_DIGEST_LEN,
                              cbDataBlock,
                              rgbDBMask ));
#else
    ChkDR( _GenerateMGF1Mask(    f_eKeyLength,
                                 DRM_SHA1_DIGEST_LEN,
                              &( f_pbEncoding[cbDataBlock] ),
                                 cbDataBlock,
                                 rgbDBMask ));
#endif
    DRM_XOR( f_pbEncoding, rgbDBMask, cbDataBlock );
    PUT_BYTE( f_pbEncoding,
              0,
              ( DRM_BYTE )( GET_BYTE( f_pbEncoding, 0 ) & ( 0xFF >> cZeroBits ) ) );

    /*
    ** Verify that bytes before the salt are all zero followed by 0x01
    */
    for ( ; i < ( cbDataBlock - PSS_CB_SALT ) - 1; i++ )
    {
        ChkBOOL( GET_BYTE( f_pbEncoding, i ) == 0, DRM_E_RSA_SIGNATURE_ERROR );
    }
    ChkBOOL( GET_BYTE( f_pbEncoding, i ) == OAEP_B_MSG_START, DRM_E_RSA_SIGNATURE_ERROR );

    /*
    ** Fill the salt in M' now, compute Hash( M' ) and check against the hash in EM
    */
    DRM_BYT_CopyBytes( rgbMPrime, PSS_IB_SALT, f_pbEncoding, cbDataBlock - PSS_CB_SALT, PSS_CB_SALT );
    ChkDR( DRM_SHA_Init( &contextSHA, eDRM_SHA_1 ) );
    ChkDR( DRM_SHA_Update( rgbMPrime, PSS_CB_MPRIME, &contextSHA ) );
    ChkDR( DRM_SHA_Finalize ( &contextSHA, DRM_SHA1_DIGEST_LEN, rgbMPrimeHash ) );

    ChkBOOL( DRM_BYT_CompareBytes( f_pbEncoding,
                                   cbDataBlock,
                                   rgbMPrimeHash,
                                   0,
                                   DRM_SHA1_DIGEST_LEN ) == 0,
             DRM_E_RSA_SIGNATURE_ERROR );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/*********************************************************************
**
**  Function:  _PSSEncode
**
**  Synopsis:  Does PSS Encoding on a variable sized buffer (see PKCS #1 v2.1)
**
**  Arguments:
**     [f_pbData]        :  Specifies the data to encode
**     [f_cbData]        :  Specifies the length of the data to encode
**     [f_ibData]        :  Specifies the offset from the beginning of the original data
**     [f_cbEMBits]      :  Specifies the bit length of the encoded message
**     [f_rgbEncodedMsg] :  Returns the PSS encoded data
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**
*********************************************************************/
static DRM_RESULT _PSSEncode(
    __in const                                  DRM_RSA_SUPPORTED_KEY_LENGTHS  f_eKeyLength,
    __in                                        DRM_DWORD                      f_cbData,
    __in_ecount( f_cbData ) const               DRM_BYTE                      *f_pbData,
    __in                                        DRM_DWORD                      f_ibData,
    __in                                        DRM_DWORD                      f_cbEMBits,
    __out_ecount( BITS_TO_BYTES( f_cbEMBits ) ) DRM_BYTE                      *f_pEncodedMsg
    )
{
    DRM_RESULT      dr           = DRM_SUCCESS;
    DRM_DWORD       ibCurr       = PSS_CB_ZERO_PAD;
    DRM_SHA_CONTEXT contextSHA   = DRM_SHA_CONTEXT_DEFAULT;
    DRM_BYTE        rgbMPrime[__CB_DECL( PSS_CB_MPRIME )]                = { 0 };
    DRM_BYTE        rgbDataBlockMask[__CB_DECL( OAEP_CB_DATABLOCK_MAX )] = { 0 };
#if SIXTEEN_BIT_ADDRESSING
    DRM_DWORD       ibData       = f_ibData;
    DRM_DWORD       cbData       = f_cbData;
    DRM_BYTE        bTemp        = 0x00;
#endif /* SIXTEEN_BIT_ADDRESSING */

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_RSA, PERF_FUNC__PSSEncode );

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( f_eKeyLength ) );
    ChkArg( f_pbData   != NULL );
    ChkArg( f_cbData   >  0 );
    ChkArg( BITS_TO_BYTES( f_cbEMBits ) == DRM_RSA_CB_MODULUS( (DRM_DWORD)f_eKeyLength ) ) ;

    /*
    ** Generate M' and compute its hash
    */
    ChkDR( DRM_SHA_Init( &contextSHA, eDRM_SHA_1 ) );
    ChkDR( DRM_SHA_Update( f_pbData, f_cbData, &contextSHA ) );
    ChkDR( DRM_SHA_Finalize( &contextSHA, DRM_SHA1_DIGEST_LEN, &( rgbMPrime[ibCurr] ) ) );
    ibCurr += DRM_SHA1_DIGEST_LEN;

    /*
    ** Calculate Hash( M' )
    */
    ChkDR( DRM_SHA_Init( &contextSHA, eDRM_SHA_1 ) );
    ChkDR( DRM_SHA_Update( rgbMPrime, ibCurr, &contextSHA ) );
    ChkDR( DRM_SHA_Finalize( &contextSHA, DRM_SHA1_DIGEST_LEN, &( f_pEncodedMsg[OAEP_CB_DATABLOCK( f_eKeyLength )] ) ) );

    ZEROMEM( f_pEncodedMsg, OAEP_CB_DATABLOCK( f_eKeyLength ) - 1 );

    /*
    ** 0x01 byte separating padding and salt
    */
    PUT_BYTE( f_pEncodedMsg, OAEP_CB_DATABLOCK( f_eKeyLength ) - 1, OAEP_B_MSG_START );

    ChkDR( _GenerateMGF1Mask(    f_eKeyLength,
                                 DRM_SHA1_DIGEST_LEN,
                              &( f_pEncodedMsg[OAEP_CB_DATABLOCK( f_eKeyLength )] ),
                                 OAEP_CB_DATABLOCK( f_eKeyLength ),
                                 rgbDataBlockMask ) );

    DRM_XOR( f_pEncodedMsg, rgbDataBlockMask, OAEP_CB_DATABLOCK( f_eKeyLength ) );
    f_pEncodedMsg[0] &= ( 0xFF >> ( DRM_RSA_CBITS_MODULUS( f_eKeyLength ) - f_cbEMBits ) );
    PUT_BYTE( f_pEncodedMsg, DRM_RSA_CB_MODULUS( f_eKeyLength ) - 1, PSS_B_END_MSG );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/*********************************************************************
**
**  Function:  OEM_RSA_OaepDecryptImpl
**
**  Synopsis:  Decrypts a cipher text that was encrypted using
**             PKCS #1 RSAES-OAEP v2.1. MGF1 is used as the mask
**             generation function and SHA1 is the hashing algorithm.
**             This version of OAEP decrypt uses an empty string for
**             the "Label".
**
**  Arguments:
**     [f_pPrivateKey]   : Specifies the private key to use.
**                         NULL indicates the private key baked into
**                         the device should be used.
**     [f_rgbCiphertext] : Specifies the encrypted text
**     [f_rgpbPlaintext] : Returns the decrypted data.
**     [f_pcbPlaintext]  : Returns the size of the original plaintext
**     [f_pCryptoCtx]     : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pcbPlaintext is NULL
**      DRM_E_OEM_RSA_DECRYPTION_ERROR
**          An error occurred while processing the data
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_RSA_OaepDecryptImpl(
    __in const                          DRM_RSA_PRIVATE_KEY *f_pPrivateKey,
    __in const                          DRM_DWORD            f_cbCiphertext,
    __in_ecount( f_cbCiphertext ) const DRM_BYTE            *f_pbCiphertext,
    __inout                             DRM_DWORD           *f_pcbPlaintext,
    __out_ecount( *f_pcbPlaintext )     DRM_BYTE            *f_pbPlaintext,
    __in                                DRM_CRYPTO_CONTEXT  *f_pCryptoCtx
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr           = DRM_SUCCESS;
    const DRM_BYTE  *pbData = NULL;
    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength = eDRM_RSA_UNKNOWN;
#if RSA_REVERSE_OS2IP == 1
    #if TARGET_LITTLE_ENDIAN
    DRM_DWORD  i            = 0;
    DRM_BYTE   rgbTempOS2IP[__CB_DECL( DRM_RSA_CB_MODULUS_MAX )]  = { 0 };
    #endif /* TARGET_LITTLE_ENDIAN */
#endif  /* RSA_REVERSE_OS2IP */
    DRM_BYTE   rgbDecrypted[__CB_DECL( DRM_RSA_CB_MODULUS_MAX )]  = { 0 };

    ChkArg( NULL != f_pPrivateKey );
    ChkDR( OEM_RSA_GetPrivateKeyLength( f_pPrivateKey, &eKeyLength ) );
    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( eKeyLength ) );
    ChkArg( 0 < f_cbCiphertext );
    ChkArg( DRM_RSA_CB_CIPHERTEXT( (DRM_DWORD)eKeyLength ) >= f_cbCiphertext );
    ChkArg( SIZEOF( rgbDecrypted ) >= f_cbCiphertext );
    ChkArg( NULL != f_pbCiphertext );
    ChkArg( NULL != f_pcbPlaintext );
    if ( DRM_RSA_CB_MAX_PLAINTEXT( (DRM_DWORD)eKeyLength ) > *f_pcbPlaintext )
    {
        *f_pcbPlaintext = DRM_RSA_CB_MAX_PLAINTEXT( eKeyLength );
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    ChkArg( NULL != f_pbPlaintext );
    ChkArg( NULL != f_pCryptoCtx );


    /*
    ** Perform OS2IP on encrypted data if required
    */
#if RSA_REVERSE_OS2IP == 1

    #if TARGET_LITTLE_ENDIAN

    for( ; i < DRM_RSA_CB_MODULUS( eKeyLength ); i++ )
    {
        PUT_BYTE( rgbTempOS2IP, i, GET_BYTE( f_pbCiphertext, ( DRM_RSA_CB_MODULUS( eKeyLength ) - 1 ) - i ) );
    }
    pbData = rgbTempOS2IP;

    #else  /* TARGET_LITTLE_ENDIAN */

    pbData = f_pbCiphertext;

    #endif /* TARGET_LITTLE_ENDIAN */

#else  /* RSA_REVERSE_OS2IP == 1 */

    pbData = f_pbCiphertext;

#endif /* RSA_REVERSE_OS2IP == 1 */

    /*
    ** Decrypt the buffer with the private key
    */
    ChkDR( OEM_RSA_Decrypt( f_pPrivateKey, f_cbCiphertext, pbData, rgbDecrypted, f_pCryptoCtx ) );

    /*
    ** Perform I2OSP on decrypted data if required
    */
#if RSA_REVERSE_OS2IP == 1

    #if TARGET_LITTLE_ENDIAN

    for( i = 0; i < DRM_RSA_CB_MODULUS( eKeyLength ); i++ )
    {
        PUT_BYTE( rgbTempOS2IP, i, GET_BYTE( rgbDecrypted, ( DRM_RSA_CB_MODULUS( eKeyLength ) - 1 ) - i ) );
    }
    pbData = rgbTempOS2IP;

    #else /* TARGET_LITTLE_ENDIAN */

    pbData = rgbDecrypted;

    #endif /* TARGET_LITTLE_ENDIAN */

#else   /* RSA_REVERSE_OS2IP */

    pbData = rgbDecrypted;

#endif  /* RSA_REVERSE_OS2IP */

    /*
    ** Decode EM and hand out
    */
    ChkDR( _OAEPDecode( eKeyLength, f_cbCiphertext, pbData, f_pcbPlaintext, f_pbPlaintext ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
**  Function:  OEM_RSA_OaepEncryptImpl
**
**  Synopsis:  Encrypts a cipher text using PKCS #1 RSAES-OAEP v2.1.
**             MGF1 is used as the mask generation function and SHA1 is
**             the hashing algorithm. This version of OAEP encrypt uses
**             an empty string for the "Label".
**
**  Arguments:
**     [f_pPublicKey]    :  Specifies the public key to use
**     [f_rgbPlaintext]  :  Specifies the plain text to encrypt
**     [f_cbPlaintext]   :  Specifies the size of the plain text
**     [f_rgbCiphertext] :  Returns the encrypted data.
**     [f_pCryptoCtx]     : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**      DRM_E_OEM_RSA_ENCRYPTION_ERROR
**          An error occurred while processing the data
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_RSA_OaepEncryptImpl(
    __in_opt                               DRM_VOID           *f_pOEMContext,
    __in const                             DRM_RSA_PUBLIC_KEY *f_pPublicKey,
    __in const                             DRM_DWORD           f_cbPlaintext,
    __in_ecount( f_cbPlaintext )     const DRM_BYTE           *f_pbPlaintext,
    __inout                                DRM_DWORD          *f_pcbCiphertext,
    __out_ecount( *f_pcbCiphertext ) const DRM_BYTE           *f_pbCiphertext,
    __in                                   DRM_CRYPTO_CONTEXT *f_pCryptoCtx )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr                    = DRM_SUCCESS;
    const DRM_BYTE  *pbEncodedData   = NULL;
    const DRM_BYTE  *pbEncryptedData = NULL;
    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength = eDRM_RSA_UNKNOWN;
#if RSA_REVERSE_OS2IP == 1
    #if TARGET_LITTLE_ENDIAN
    DRM_DWORD  i                     = 0;
    DRM_BYTE   rgbTempOS2IP[__CB_DECL( OAEP_CB_ENCRYPTED_MSG_MAX )]  = { 0 };
    #endif /* TARGET_LITTLE_ENDIAN */
#endif  /* RSA_REVERSE_OS2IP */
    DRM_BYTE   rgbEncoded[__CB_DECL( OAEP_CB_ENCRYPTED_MSG_MAX )]    = { 0 };
    DRM_DWORD  cbEncoded = 0;
    DRM_DWORD  cbCiphertext = 0;

    ChkArg( NULL != f_pPublicKey );
    ChkDR( OEM_RSA_GetPublicKeyLength( f_pPublicKey, &eKeyLength ) );
    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( eKeyLength ) );
    ChkArg( 0 < f_cbPlaintext );
    ChkArg( OAEP_CB_DATABLOCK_MSG_MAX( (DRM_DWORD)eKeyLength ) >= f_cbPlaintext );
    ChkArg( NULL != f_pbPlaintext );
    ChkArg( NULL != f_pcbCiphertext );
    if ( OAEP_CB_ENCRYPTED_MSG( (DRM_DWORD)eKeyLength ) > *f_pcbCiphertext )
    {
         *f_pcbCiphertext = OAEP_CB_ENCRYPTED_MSG( eKeyLength );
         ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    ChkArg( NULL != f_pbCiphertext );
    ChkArg( NULL != f_pCryptoCtx );

    cbCiphertext = *f_pcbCiphertext;
    *f_pcbCiphertext = OAEP_CB_ENCRYPTED_MSG( eKeyLength );

    /*
    ** Compute encoded message
    */
    cbEncoded = SIZEOF( rgbEncoded );
    ChkDR( _OAEPEncode( eKeyLength, f_pOEMContext, f_cbPlaintext, f_pbPlaintext, &cbEncoded, rgbEncoded ) );
    if ( ( SIZEOF( rgbEncoded ) < cbEncoded )
      || ( cbCiphertext < cbEncoded ) )
    {
        ChkDR( DRM_E_CRYPTO_FAILED );
    }

/*
** Perform OS2IP( EM ): Reverse encoded message
*/
#if RSA_REVERSE_OS2IP == 1

    #if TARGET_LITTLE_ENDIAN

    for( ; i < cbEncoded; i++ )
    {
        PUT_BYTE( rgbTempOS2IP, i,  GET_BYTE( rgbEncoded, ( cbEncoded - 1 ) - i ) );
    }
    pbEncodedData = rgbTempOS2IP;
    pbEncryptedData = rgbEncoded;

    #else /* TARGET_LITTLE_ENDIAN */

    pbEncodedData = rgbEncoded;
    pbEncryptedData = f_pbCiphertext;

    #endif /* TARGET_LITTLE_ENDIAN */

#else   /* RSA_REVERSE_OS2IP == 1 */

    pbEncodedData = rgbEncoded;
    pbEncryptedData = f_pbCiphertext;

#endif  /* RSA_REVERSE_OS2IP == 1 */

    /*
    ** Encrypt the buffer with the public key
    */
    ChkDR( OEM_RSA_Encrypt( f_pPublicKey, cbEncoded, pbEncodedData, ( DRM_BYTE * )pbEncryptedData, f_pCryptoCtx ) );

/*
** Compute I2OSP for encrypted message: reverse encrypted message
*/
#if RSA_REVERSE_OS2IP == 1

    #if TARGET_LITTLE_ENDIAN

    for( i = 0; i < cbEncoded; i++ )
    {
        PUT_BYTE( f_pbCiphertext, i, GET_BYTE( pbEncryptedData, ( cbEncoded - 1 ) - i ) );
    }

    #endif /* TARGET_LITTLE_ENDIAN */

#endif  /* RSA_REVERSE_OS2IP */

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_PssSignImpl
**
**  Synopsis: RSA signs the given data with the privkey.
**            Uses PSS (see PKCS #1 v2.1)
**
**  Arguments:
**     [f_pPrivateKey]   : Specifies the private key to use.
**                         NULL indicates the private key baked into
**                         the device should be used
**     [f_pbMsg]         : Specifies the Message to sign
**     [f_cbMsg]         : Specifies the length of the original message
**     [f_ibMsg]         : Specifies the offset from the beginning of the original message
**                         buffer
**     [f_prgbSignature] : Returns the computed RSA signature
**     [f_pCryptoCtx]     : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_RSA_PssSignImpl(
    __in const                      DRM_RSA_PRIVATE_KEY *f_pPrivateKey,
    __in const                      DRM_DWORD            f_cbMsg,
    __in_ecount( f_cbMsg ) const    DRM_BYTE            *f_pbMsg,
    __in const                      DRM_DWORD            f_ibMsg,
    __inout                         DRM_DWORD           *f_pcbSignature,
    __out_ecount( *f_pcbSignature ) DRM_BYTE            *f_pbSignature,
    __in                            DRM_CRYPTO_CONTEXT  *f_pCryptoCtx
    )
{
    DRM_RESULT                       dr              = DRM_SUCCESS;
    const DRM_BYTE                  *pbDataEncoded   = NULL;
    DRM_BYTE                        *pbDataEncrypted = NULL;
    DRM_RSA_SUPPORTED_KEY_LENGTHS    eKeyLength      = eDRM_RSA_UNKNOWN;
#if RSA_REVERSE_OS2IP == 1
    #if TARGET_LITTLE_ENDIAN
    DRM_DWORD                        i               = 0;
    DRM_BYTE                         rgbTempOS2IP[__CB_DECL( DRM_RSA_CB_MODULUS_MAX )] = { 0 };
    #endif /* TARGET_LITTLE_ENDIAN */
#endif  /* RSA_REVERSE_OS2IP */
    DRM_BYTE                         rgbData[__CB_DECL( DRM_RSA_CB_MODULUS_MAX )]      = { 0 };

    ChkArg( NULL != f_pPrivateKey );
    ChkDR( OEM_RSA_GetPrivateKeyLength( f_pPrivateKey, &eKeyLength ) );
    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( eKeyLength ) );
    ChkArg( 0 < f_cbMsg );
    ChkArg( NULL != f_pbMsg );
    ChkArg( NULL != f_pcbSignature );
    if ( DRM_RSA_CB_MODULUS( (DRM_DWORD)eKeyLength ) > *f_pcbSignature )
    {
        *f_pcbSignature = DRM_RSA_CB_MODULUS( eKeyLength );
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    ChkArg( NULL != f_pbSignature );
    ChkArg( NULL != f_pCryptoCtx );

    *f_pcbSignature = DRM_RSA_CB_MODULUS( eKeyLength );

    ChkDR( _PSSEncode( eKeyLength,
                       f_cbMsg,
                       f_pbMsg,
                       f_ibMsg,
                       DRM_RSA_CBITS_MODULUS( eKeyLength ) - 1,
                       rgbData ) );

#if RSA_REVERSE_OS2IP == 1

    #if TARGET_LITTLE_ENDIAN

    for ( ; i < DRM_RSA_CB_MODULUS( eKeyLength ); i++ )
    {
        PUT_BYTE( rgbTempOS2IP,
                  i,
                  GET_BYTE( rgbData, ( DRM_RSA_CB_MODULUS( eKeyLength )( eKeyLength ) - 1 ) - i ) );
    }
    pbDataEncoded = rgbTempOS2IP;
    pbDataEncrypted = rgbData;

    #else /* TARGET_LITTLE_ENDIAN */

    pbDataEncoded = rgbData;
    pbDataEncrypted = f_pbSignature;

    #endif /* TARGET_LITTLE_ENDIAN */

#else  /* RSA_REVERSE_OS2IP */

    pbDataEncoded = rgbData;
    pbDataEncrypted = f_pbSignature;

#endif  /* RSA_REVERSE_OS2IP */

    /*
    ** Decrypt the buffer with the private key
    */
    ChkDR( OEM_RSA_Decrypt( f_pPrivateKey, DRM_RSA_CB_MODULUS( eKeyLength ), pbDataEncoded, pbDataEncrypted, f_pCryptoCtx ) );

#if RSA_REVERSE_OS2IP == 1

    #if TARGET_LITTLE_ENDIAN

    for ( i = 0; i < DRM_RSA_CB_MODULUS( eKeyLength ); i++ )
    {
        PUT_BYTE( f_pbSignature, i, GET_BYTE( pbDataEncrypted, ( DRM_RSA_CB_MODULUS( eKeyLength ) - 1 ) - i ) );
    }

    #endif /* TARGET_LITTLE_ENDIAN */

#endif  /* RSA_REVERSE_OS2IP */

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_PssVerifyImpl
**
**  Synopsis: Verifies an RSA signature using the given pubkey.
**            Uses PSS (see PKCS #1 v2.1)
**
**  Arguments:
**     [f_pPublicKey]   : Specifies the public key to use.
**     [f_pbMsg]        : Specifies the original message
**     [f_cbMsg]        : Specifies the length of the original message
**     [f_ibMsg]        : Specifies the offset from the beginning of the original message
**                        buffer
**     [f_rgbSignature] : Specifies the signature to verify
**     [f_pCryptoCtx]     : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**      DRM_E_RSA_SIGNATURE_ERROR
**          Failed to verify the signature
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_PssVerifyImpl(
    __in const                         DRM_RSA_PUBLIC_KEY  *f_pPublicKey,
    __in                               DRM_DWORD            f_cbMsg,
    __in_ecount( f_cbMsg ) const       DRM_BYTE            *f_pbMsg,
    __in const                         DRM_DWORD            f_ibMsg,
    __in const                         DRM_DWORD            f_cbSignature,
    __in_ecount( f_cbSignature ) const DRM_BYTE            *f_pbSignature,
    __in                               DRM_CRYPTO_CONTEXT  *f_pCryptoCtx
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr              = DRM_SUCCESS;
    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength = eDRM_RSA_UNKNOWN;
#if RSA_REVERSE_OS2IP == 1
    #if TARGET_LITTLE_ENDIAN
    DRM_DWORD  i               = 0;
    DRM_BYTE   rgbTempOS2IP[__CB_DECL( DRM_RSA_CB_MODULUS_MAX )] = { 0 };
    #endif /* TARGET_LITTLE_ENDIAN */
#endif  /* RSA_REVERSE_OS2IP */
    DRM_BYTE   rgbTempData[__CB_DECL( DRM_RSA_CB_MODULUS_MAX )]  = { 0 };
    DRM_BYTE  *pbData          = ( DRM_BYTE * )f_pbSignature;

    ChkArg( NULL != f_pPublicKey );
    ChkDR( OEM_RSA_GetPublicKeyLength( f_pPublicKey, &eKeyLength ) );
    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH( eKeyLength ) );
    ChkArg( 0 < f_cbMsg );
    ChkArg( NULL != f_pbMsg );
    ChkArg( 0 < f_cbSignature );
    ChkArg( NULL != f_pbSignature );
    ChkArg( f_pCryptoCtx != NULL );


    ChkArg( DRM_RSA_CB_MODULUS( (DRM_DWORD)eKeyLength ) == f_cbSignature );

    /*
    ** Perform OS2IP on the signature: reverse the signature
    */
#if RSA_REVERSE_OS2IP == 1

    #if TARGET_LITTLE_ENDIAN

    for ( ; i < DRM_RSA_CB_MODULUS( eKeyLength ); i++ )
    {
        PUT_BYTE( rgbTempOS2IP, i, GET_BYTE( f_pbSignature, ( DRM_RSA_CB_MODULUS( eKeyLength ) - 1 ) - i ) );
    }
    pbData = rgbTempOS2IP;

    #endif /* TARGET_LITTLE_ENDIAN */

#endif  /* RSA_REVERSE_OS2IP == 1 */

    /*
    ** Encrypt the buffer with the public key
    */
    dr = OEM_RSA_Encrypt( f_pPublicKey, f_cbSignature, pbData, rgbTempData, f_pCryptoCtx );
    ChkBOOL( DRM_SUCCEEDED( dr ), DRM_E_RSA_SIGNATURE_ERROR );

    /*
    ** Perform I2OSP on the decrypted data to get EM: reverse decrypted data
    */
#if RSA_REVERSE_OS2IP == 1

    #if TARGET_LITTLE_ENDIAN

    for ( i = 0; i < DRM_RSA_CB_MODULUS( eKeyLength ); i++ )
    {
        PUT_BYTE( rgbTempOS2IP, i, GET_BYTE( rgbTempData, ( DRM_RSA_CB_MODULUS( eKeyLength ) - 1 ) - i ) );
    }
    pbData = rgbTempOS2IP;

    #else /* TARGET_LITTLE_ENDIAN */

    pbData = rgbTempData;

    #endif /* TARGET_LITTLE_ENDIAN */

#else   /* RSA_REVERSE_OS2IP == 1 */

    pbData = rgbTempData;

#endif  /* RSA_REVERSE_OS2IP == 1 */

    /*
    ** Verify Encoded Message
    */
    ChkDR( _PSSVerify( eKeyLength,
                       f_cbMsg,
                       f_pbMsg,
                       f_ibMsg,
                       DRM_RSA_CBITS_MODULUS( eKeyLength ) - 1,
                       pbData ) );

ErrorExit:
    return dr;
}



EXIT_PK_NAMESPACE_CODE;

