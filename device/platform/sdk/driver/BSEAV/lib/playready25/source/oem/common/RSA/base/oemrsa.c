/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMRSA_C
#include <bignum.h>
#include <oemrsaimpl.h>
#include <oemrsa.h>
#include <oemeccp160.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

/******************************************************************************
**
** WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
**
** DO NOT MOVE OR USE THESE CONSTANTS OR TYPES OUTSIDE THIS FILE!!!
**
** Current maximum supported length is 4096BIT.
** If you reuse them, you will prevent extending RSA support beyond 4096BIT.
** They should only be defined/used inside the RSA implementation.
**
** WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
**
******************************************************************************/
typedef DRM_RSA_SUPPORTED_KEY_LENGTHS_4096BIT   DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT;
#define eDRM_RSA_MAXIMUM_KEY_LENGTH_ALLBIT      4096
#define CDIGITS_MODULUS_MAX_ALLBIT              CDIGITS_MODULUS_MAX_4096BIT
#define IS_VALID_DRM_RSA_KEY_LENGTH_ALLBIT(x)   IS_VALID_DRM_RSA_KEY_LENGTH_4096BIT(x)
/******************************************************************************
**
** WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
**
******************************************************************************/

/*
**  To ensure that no name mangling occurs this function declarations are wrapped
**  into PK namespace block
*/
ENTER_PK_NAMESPACE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER_25057, "f_pbData and f_pDigits lengths defined by f_cBitsData" )
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL _BigEndianBytesToDigits(
    __in_ecount( BITS_TO_BYTES( f_cBitsData ) )   const DRM_BYTE  *f_pbData,
    __in                                          const DRM_DWORD  f_cBitsData,
    __out_ecount( BITS_TO_DIGITS( f_cBitsData ) )       digit_t   *f_pDigits ) DRM_NO_INLINE_ATTRIBUTE;
PREFAST_POP

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER_25057, "f_pbData and f_pDigits lengths defined by f_cBitsData" )
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL _DigitsToBigEndianBytes(
    __in_ecount( BITS_TO_DIGITS( f_cBitsData ) ) const digit_t   *f_pDigits,
    __in                                         const DRM_DWORD  f_cBitsData,
    __out_ecount( BITS_TO_BYTES( f_cBitsData ) )       DRM_BYTE  *f_pbData ) DRM_NO_INLINE_ATTRIBUTE;
PREFAST_POP

EXIT_PK_NAMESPACE;

ENTER_PK_NAMESPACE_CODE;

DRM_EXPORT_VAR const DRM_BYTE rgbDRM_RSA_DEFAULT_PUBEXP[__CB_DECL( DRM_RSA_CB_PUBEXP )] =
{
    0x00, 0x01, 0x00, 0x01 /* 65537 */
};

/**********************************************************************
**
** Function:    _BigEndianBytesToDigits
**
** Synopsis:    Copy a big-endian DRM_BYTE array to a digit_t array.
**
** Arguments:
**      [f_pbData]   : Input array of DRM_BYTEs
**      [f_cBitsData]: Length of array in BITs
**      [f_pDigits]  : Output array of digit_ts
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the pointer args was NULL or f_cBitsData was less
**          than or equal to zero.
**
**********************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER_25057, "f_pbData and f_pDigits lengths defined by f_cBitsData" )
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL _BigEndianBytesToDigits(
    __in_ecount( BITS_TO_BYTES( f_cBitsData ) ) const DRM_BYTE  *f_pbData,
    __in const                                        DRM_DWORD  f_cBitsData,
    __out_ecount( BITS_TO_DIGITS( f_cBitsData ) )     digit_t   *f_pDigits )
PREFAST_POP
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT      dr          = DRM_SUCCESS;
    DRM_DWORD       iDigits     = 0;
    DRM_DWORD       iBytes      = 0;
    DRM_DWORD       cbRemaining = 0;
    const DRM_DWORD cDigits     = BITS_TO_DIGITS( f_cBitsData );

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_RSA, PERF_FUNC__BigEndianBytesToDigits );

    ChkArg( f_pbData    != NULL );
    ChkArg( f_cBitsData >  0 );
    ChkArg( f_pDigits   != NULL );

    ZEROMEM( f_pDigits, cDigits * SIZEOF( digit_t ) );

    for( iDigits = 0; iDigits < cDigits; iDigits++ )
    {
        cbRemaining = ( f_cBitsData + BITS_PER_STANDARD_BYTE - 1 ) / BITS_PER_STANDARD_BYTE
                    - RADIX_BYTES * iDigits;
        f_pDigits[iDigits] = 0;

        for( iBytes = 0; iBytes < MIN( RADIX_BYTES, cbRemaining ); iBytes++ )
        {
            f_pDigits[iDigits] ^= ( digit_t )GET_BYTE( f_pbData, ( cbRemaining - 1 ) - iBytes )
                               << ( 8 * iBytes );
        }
    }

    /*
    ** Strip high bits
    */
    f_pDigits[cDigits - 1] &= RADIXM1 >> ( RADIX_BITS * cDigits - f_cBitsData );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/**********************************************************************
**
** Function:    _DigitsToBigEndianBytes
**
** Synopsis:    Convert a digit_t array to big-endian bytes
**
** Arguments:
**      [f_pDigits]    : Input array of digit_ts
**      [f_cBitsData]  : Length of data in bits
**      [f_pbData]     : Output array of BYTEs
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the pointer args was NULL or f_cBitsData was less
**          than or equal to zero.
**
**********************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER_25057, "f_pbData and f_pDigits lengths defined by f_cBitsData" )
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL _DigitsToBigEndianBytes(
    __in_ecount( BITS_TO_DIGITS( f_cBitsData ) ) const digit_t   *f_pDigits,
    __in const                                         DRM_DWORD  f_cBitsData,
    __out_ecount( BITS_TO_BYTES( f_cBitsData ) )       DRM_BYTE  *f_pbData )
PREFAST_POP
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT      dr          = DRM_SUCCESS;
    DRM_DWORD       iDigits     = 0;
    DRM_DWORD       iBytes      = 0;
    DRM_DWORD       cbRemaining = 0;
    digit_t         digCurrent  = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_RSA, PERF_FUNC__DigitsToBigEndianBytes );

    ChkArg( f_pDigits   != NULL );
    ChkArg( f_cBitsData >  0 );
    ChkArg( f_pbData    != NULL );

    for ( iDigits = 0; iDigits < BITS_TO_DIGITS( f_cBitsData ); iDigits++ )
    {
        cbRemaining = ( f_cBitsData + BITS_PER_STANDARD_BYTE - 1 ) / BITS_PER_STANDARD_BYTE
                    - RADIX_BYTES * iDigits;
        digCurrent = f_pDigits[iDigits];
        for ( iBytes = 0; iBytes != MIN( cbRemaining, RADIX_BYTES ); iBytes++ )
        {
            AssertChkArg( ( cbRemaining - 1 ) - iBytes < BITS_TO_BYTES( f_cBitsData ) );

            PUT_BYTE( f_pbData,
                     ( cbRemaining - 1 ) - iBytes,
                     ( DRM_BYTE )( digCurrent & 0xFF ) );
            digCurrent >>= 8;
        }
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/**********************************************************************
**
**  Function: _ModularExponentiate
**
**  Synopsis: Does modular exponentiation on the input buffer
**            rgbDataOut = rgbDataIn^pbExp MOD rgbModulus
**
**  Arguments:
**     [f_eKeyLength] : Specifies the key length
**     [f_rgbDataIn]  : Specifies the data to exponentiate
**     [f_pbExp]      : Specifies the exponent
**     [f_cbExp]      : Specifies the length of the exponent
**     [f_rgbModulus] : Specifies the modulus
**     [f_rgbDataOut] : Returns the processed data
**     [f_pCryptoCtx] : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pPublicKey was NULL
**      DRM_E_OEM_RSA_MESSAGE_TOO_BIG
**          The message data is too long to sign with the given modulus
**      DRM_E_MODULAR_ARITHMETIC_FAILURE
*           A failure occurred in the modular arithmetic
**
**********************************************************************/
DRM_NO_INLINE static DRM_RESULT DRM_CALL _ModularExponentiate(
    __in const                       DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT   f_eKeyLength,
    __in const                       DRM_DWORD                              f_cbDataIn,
    __in_ecount( f_cbDataIn ) const  DRM_BYTE                              *f_pbDataIn,
    __in const                       DRM_DWORD                              f_cdExp,
    __in_ecount( f_cdExp ) const     digit_t                               *f_pdExp,
    __in const                       DRM_DWORD                              f_cdModulus,
    __in_ecount( f_cdModulus ) const digit_t                               *f_pdModulus,
    __out_ecount( f_cbDataIn )       DRM_BYTE                              *f_pbDataOut,
    __inout                          DRM_CRYPTO_CONTEXT                    *f_pCryptoCtx
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_NO_INLINE static DRM_RESULT DRM_CALL _ModularExponentiate(
    __in const                       DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT   f_eKeyLength,
    __in const                       DRM_DWORD                              f_cbDataIn,
    __in_ecount( f_cbDataIn ) const  DRM_BYTE                              *f_pbDataIn,
    __in const                       DRM_DWORD                              f_cdExp,
    __in_ecount( f_cdExp ) const     digit_t                               *f_pdExp,
    __in const                       DRM_DWORD                              f_cdModulus,
    __in_ecount( f_cdModulus ) const digit_t                               *f_pdModulus,
    __out_ecount( f_cbDataIn )       DRM_BYTE                              *f_pbDataOut,
    __inout                          DRM_CRYPTO_CONTEXT                    *f_pCryptoCtx
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT      dr                                  = DRM_SUCCESS;
    digit_t         rgdTemp[CDIGITS_MODULUS_MAX_ALLBIT];
    mp_modulus_t    modulus_data;
    DRM_BOOL        fModulusCreated                     = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_RSA, PERF_FUNC__ModularExponentiate );

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH_ALLBIT( f_eKeyLength ) );
    ChkArg( DRM_RSA_CB_MODULUS( (DRM_DWORD)f_eKeyLength ) == f_cbDataIn );
    ChkArg( NULL != f_pbDataIn );
    ChkArg( 0 < f_cdExp );
    ChkArg( CDIGITS_PUBEXP >= f_cdExp );
    ChkArg( NULL != f_pdExp );
    ChkArg( 0 < f_cdModulus );
    ChkArg( CDIGITS_MODULUS( (DRM_DWORD)f_eKeyLength ) >= f_cdModulus );
    ChkArg( NULL != f_pdModulus );
    ChkArg( NULL != f_pbDataOut );
    ChkArg( NULL != f_pCryptoCtx );

    OEM_SECURE_DIGITZEROMEMORY( rgdTemp, NO_OF(rgdTemp) );
    OEM_SECURE_ZERO_MEMORY( &modulus_data, SIZEOF(modulus_data) ); 
    
    DRM_RSA_PKInitSize( f_pCryptoCtx->rgbCryptoContext, sizeof(f_pCryptoCtx->rgbCryptoContext)  );

    /*
    ** Convert message to digits
    */
    ChkDR( _BigEndianBytesToDigits( f_pbDataIn, DRM_RSA_CBITS_MODULUS( f_eKeyLength ), rgdTemp ) );

    /*
    ** Make sure the base is less than the modulus
    */
    ChkBOOL( compare_same( f_pdModulus, rgdTemp, CDIGITS_MODULUS( f_eKeyLength ) ) > 0,
             DRM_E_OEM_RSA_MESSAGE_TOO_BIG );

    /*
    ** Create modulus-specific structures
    */
    ChkBOOL( create_modulus( f_pdModulus,
                             f_cdModulus,
                             FROM_RIGHT,
                            &modulus_data,
                             (struct bigctx_t *)f_pCryptoCtx->rgbCryptoContext,
                             (struct bigctx_t *)f_pCryptoCtx->rgbCryptoContext ), DRM_E_MODULAR_ARITHMETIC_FAILURE );
    fModulusCreated = TRUE;

    /*
    ** Convert message to modular representation
    */
    ChkBOOL( to_modular( rgdTemp, CDIGITS_MODULUS( f_eKeyLength ), rgdTemp, &modulus_data, NULL ),
             DRM_E_MODULAR_ARITHMETIC_FAILURE );

    /*
    ** Do the modular exponentiation
    */
    ChkBOOL( mod_exp( rgdTemp,
                      f_pdExp,
                      f_cdExp,
                      rgdTemp,
                      &modulus_data,
                      (struct bigctx_t *)f_pCryptoCtx->rgbCryptoContext ), DRM_E_MODULAR_ARITHMETIC_FAILURE );

    /*
    ** Convert message from modular representation
    */
    ChkBOOL( from_modular( rgdTemp, rgdTemp, &modulus_data ),
             DRM_E_MODULAR_ARITHMETIC_FAILURE );

    /*
    ** Convert back from digits
    */
    ChkDR( _DigitsToBigEndianBytes( rgdTemp, DRM_RSA_CBITS_MODULUS( f_eKeyLength ), f_pbDataOut ) );

ErrorExit:
    if( fModulusCreated )
    {
        uncreate_modulus( &modulus_data, (struct bigctx_t *)f_pCryptoCtx->rgbCryptoContext );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_GetPublicKeyLength_2048BIT
**
**  Synopsis: Get the length of the public key in bits.
**
**  Arguments:
**     [f_pPublicKey]   : Specifies the public key
**     [f_peKeyLength]  : Returns the public key length in bits
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pPublicKey was NULL
**
**********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_RSA_GetPublicKeyLength_2048BIT(
    __in const DRM_RSA_PUBLIC_KEY_2048BIT            *f_pPublicKey,
    __out      DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT *f_peKeyLength
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_RSA_GetPublicKeyLength_2048BIT(
    __in const DRM_RSA_PUBLIC_KEY_2048BIT            *f_pPublicKey,
    __out      DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT *f_peKeyLength
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    const INTERNAL_DRM_RSA_PUBLIC_KEY_2048BIT *pPubKeyInternal = ( const INTERNAL_DRM_RSA_PUBLIC_KEY_2048BIT * )f_pPublicKey;

    ChkArg( NULL != f_pPublicKey );
    ChkArg( NULL != f_peKeyLength );

    *f_peKeyLength = pPubKeyInternal->eKeyLength;
    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH_2048BIT( *f_peKeyLength ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_GetPublicKeyLength_4096BIT
**
**  Synopsis: Get the length of the public key in bits.
**
**  Arguments:
**     [f_pPublicKey]   : Specifies the public key
**     [f_peKeyLength]  : Returns the public key length in bits
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pPublicKey was NULL
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_GetPublicKeyLength_4096BIT(
    __in const DRM_RSA_PUBLIC_KEY_4096BIT            *f_pPublicKey,
    __out      DRM_RSA_SUPPORTED_KEY_LENGTHS_4096BIT *f_peKeyLength
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_GetPublicKeyLength_4096BIT(
    __in const DRM_RSA_PUBLIC_KEY_4096BIT            *f_pPublicKey,
    __out      DRM_RSA_SUPPORTED_KEY_LENGTHS_4096BIT *f_peKeyLength
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    const INTERNAL_DRM_RSA_PUBLIC_KEY_4096BIT *pPubKeyInternal = ( const INTERNAL_DRM_RSA_PUBLIC_KEY_4096BIT * )f_pPublicKey;

    ChkArg( NULL != f_pPublicKey );
    ChkArg( NULL != f_peKeyLength );

    *f_peKeyLength = pPubKeyInternal->eKeyLength;
    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH_4096BIT( *f_peKeyLength ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_GetPrivateKeyLength_2048BIT
**
**  Synopsis: Get the length of the private key in bits.
**
**  Arguments:
**     [f_pPrivateKey]  : Specifies the private key
**     [f_peKeyLength]  : Returns the private key length in bits
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pPublicKey was NULL
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_GetPrivateKeyLength_2048BIT(
    __in const DRM_RSA_PRIVATE_KEY_2048BIT           *f_pPrivateKey,
    __out      DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT *f_peKeyLength
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_GetPrivateKeyLength_2048BIT(
    __in const DRM_RSA_PRIVATE_KEY_2048BIT           *f_pPrivateKey,
    __out      DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT *f_peKeyLength
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    const INTERNAL_DRM_RSA_PRIVATE_KEY_2048BIT *pPvKeyInternal = ( const INTERNAL_DRM_RSA_PRIVATE_KEY_2048BIT * )f_pPrivateKey;

    ChkArg( NULL != f_pPrivateKey );
    ChkArg( NULL != f_peKeyLength );

    *f_peKeyLength = pPvKeyInternal->eKeyLength;
    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH_2048BIT( *f_peKeyLength ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_GetPrivateKeyLength_4096BIT
**
**  Synopsis: Get the length of the private key in bits.
**
**  Arguments:
**     [f_pPrivateKey]  : Specifies the private key
**     [f_peKeyLength]  : Returns the private key length in bits
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pPublicKey was NULL
**
**********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_RSA_GetPrivateKeyLength_4096BIT(
    __in const DRM_RSA_PRIVATE_KEY_4096BIT           *f_pPrivateKey,
    __out      DRM_RSA_SUPPORTED_KEY_LENGTHS_4096BIT *f_peKeyLength
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_RSA_GetPrivateKeyLength_4096BIT(
    __in const DRM_RSA_PRIVATE_KEY_4096BIT           *f_pPrivateKey,
    __out      DRM_RSA_SUPPORTED_KEY_LENGTHS_4096BIT *f_peKeyLength
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    const INTERNAL_DRM_RSA_PRIVATE_KEY_4096BIT *pPvKeyInternal = ( const INTERNAL_DRM_RSA_PRIVATE_KEY_4096BIT * )f_pPrivateKey;

    ChkArg( NULL != f_pPrivateKey );
    ChkArg( NULL != f_peKeyLength );

    *f_peKeyLength = pPvKeyInternal->eKeyLength;
    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH_4096BIT( *f_peKeyLength ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_GetKeyLengthFromModulusLength_2048BIT
**
**  Synopsis: Get the length of the public key in bits.
**
**  Arguments:
**     [f_cbModulus]    : Specifies the modulus length of the key
**     [f_peKeyLength]  : Returns the public key length in bits
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_cbModulus was an invalid value
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_GetKeyLengthFromModulusLength_2048BIT(
    __in const DRM_DWORD                              f_cbModulus,
    __out      DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT *f_peKeyLength
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_GetKeyLengthFromModulusLength_2048BIT(
    __in const DRM_DWORD                              f_cbModulus,
    __out      DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT *f_peKeyLength
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_peKeyLength );

    if ( DRM_RSA_CB_MODULUS( eDRM_RSA_KEY_LENGTH_1024_2048BIT ) == f_cbModulus )
    {
        *f_peKeyLength = eDRM_RSA_KEY_LENGTH_1024_2048BIT;
    }
    else if ( DRM_RSA_CB_MODULUS( eDRM_RSA_KEY_LENGTH_2048_2048BIT ) == f_cbModulus )
    {
        *f_peKeyLength = eDRM_RSA_KEY_LENGTH_2048_2048BIT;
    }
    else
    {
        DRMASSERT( !"invalid modulus length" );
        dr = DRM_E_INVALIDARG;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_GetKeyLengthFromModulusLength_4096BIT
**
**  Synopsis: Get the length of the public key in bits.
**
**  Arguments:
**     [f_cbModulus]    : Specifies the modulus length of the key
**     [f_peKeyLength]  : Returns the public key length in bits
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_cbModulus was an invalid value
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_GetKeyLengthFromModulusLength_4096BIT(
    __in const DRM_DWORD                              f_cbModulus,
    __out      DRM_RSA_SUPPORTED_KEY_LENGTHS_4096BIT *f_peKeyLength
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_GetKeyLengthFromModulusLength_4096BIT(
    __in const DRM_DWORD                              f_cbModulus,
    __out      DRM_RSA_SUPPORTED_KEY_LENGTHS_4096BIT *f_peKeyLength
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_peKeyLength );

    if ( DRM_RSA_CB_MODULUS( eDRM_RSA_KEY_LENGTH_1024_4096BIT ) == f_cbModulus )
    {
        *f_peKeyLength = eDRM_RSA_KEY_LENGTH_1024_4096BIT;
    }
    else if ( DRM_RSA_CB_MODULUS( eDRM_RSA_KEY_LENGTH_2048_4096BIT ) == f_cbModulus )
    {
        *f_peKeyLength = eDRM_RSA_KEY_LENGTH_2048_4096BIT;
    }
    else if ( DRM_RSA_CB_MODULUS( eDRM_RSA_KEY_LENGTH_4096_4096BIT ) == f_cbModulus )
    {
        *f_peKeyLength = eDRM_RSA_KEY_LENGTH_4096_4096BIT;
    }
    else
    {
        DRMASSERT( !"invalid modulus length" );
        dr = DRM_E_INVALIDARG;
    }

ErrorExit:
    return dr;
}

static DRMFORCEINLINE DRM_RESULT _RSA_SetPublicKey(
    __in const                       DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT   f_eKeyLength,
    __in const                       DRM_DWORD                              f_cbPubExp,
    __in_ecount( f_cbPubExp ) const  DRM_BYTE                              *f_pbPubExp,
    __in const                       DRM_DWORD                              f_cbModulus,
    __in_ecount( f_cbModulus ) const DRM_BYTE                              *f_pbModulus,
    __out                            DRM_DWORD                             *f_peKeyLength,
    __out                            digit_t                               *f_rgdModulus,
    __out                            DRM_DWORD                             *f_pcdModulus,
    __out                            digit_t                               *f_rgdPubExp )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( DRM_RSA_CB_PUBEXP == f_cbPubExp );
    ChkArg( NULL != f_pbPubExp );
    ChkArg( DRM_RSA_CB_MODULUS( (DRM_DWORD)f_eKeyLength ) == f_cbModulus );
    ChkArg( NULL != f_pbModulus );

    *f_peKeyLength = (DRM_DWORD)f_eKeyLength;

    ChkDR( _BigEndianBytesToDigits( f_pbModulus,
                                    DRM_RSA_CBITS_MODULUS( f_eKeyLength ),
                                    f_rgdModulus ) );

    *f_pcdModulus = significant_digit_count( f_rgdModulus, CDIGITS_MODULUS( f_eKeyLength ) );

    ChkDR( _BigEndianBytesToDigits( f_pbPubExp,
                                    CBITS_PUBEXP,
                                    f_rgdPubExp ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_SetPublicKey_2048BIT
**
**  Synopsis: Sets up an internal form of an RSA public key
**
**  Arguments:
**     [f_eKeyLength]   : Specifies resultant public key length in bits
**     [f_cbPubExp]     : Length of f_pbPubExp in bytes
**     [f_pbPubExp]     : Buffer containing public key exponent
**     [f_cbModulus]    : Length of f_pbModulus in bytes
**     [f_pbModulus]    : Buffer containing public key modulus
**     [f_pPublicKey]   : Returns public key in external format
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pPublicKey was NULL
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_SetPublicKey_2048BIT(
    __in const                       DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT  f_eKeyLength,
    __in const                       DRM_DWORD                              f_cbPubExp,
    __in_ecount( f_cbPubExp ) const  DRM_BYTE                              *f_pbPubExp,
    __in const                       DRM_DWORD                              f_cbModulus,
    __in_ecount( f_cbModulus ) const DRM_BYTE                              *f_pbModulus,
    __out                            DRM_RSA_PUBLIC_KEY_2048BIT            *f_pPublicKey
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_SetPublicKey_2048BIT(
    __in const                       DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT  f_eKeyLength,
    __in const                       DRM_DWORD                              f_cbPubExp,
    __in_ecount( f_cbPubExp ) const  DRM_BYTE                              *f_pbPubExp,
    __in const                       DRM_DWORD                              f_cbModulus,
    __in_ecount( f_cbModulus ) const DRM_BYTE                              *f_pbModulus,
    __out                            DRM_RSA_PUBLIC_KEY_2048BIT            *f_pPublicKey
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    INTERNAL_DRM_RSA_PUBLIC_KEY_2048BIT *pPubKeyInternal = ( INTERNAL_DRM_RSA_PUBLIC_KEY_2048BIT * )f_pPublicKey;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_RSA, PERF_FUNC_OEM_RSA_SetPublicKey_2048BIT );

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH_2048BIT( f_eKeyLength ) );

    ChkArg( NULL != f_pPublicKey );

    ChkDR( _RSA_SetPublicKey(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)f_eKeyLength, /* Upgrade key length */
        f_cbPubExp,
        f_pbPubExp,
        f_cbModulus,
        f_pbModulus,
        (DRM_DWORD*)&pPubKeyInternal->eKeyLength,
        pPubKeyInternal->rgdModulus,
        &pPubKeyInternal->cdModulus,
        pPubKeyInternal->rgdPubExp ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_SetPublicKey_4096BIT
**
**  Synopsis: Sets up an internal form of an RSA public key
**
**  Arguments:
**     [f_eKeyLength]   : Specifies resultant public key length in bits
**     [f_cbPubExp]     : Length of f_pbPubExp in bytes
**     [f_pbPubExp]     : Buffer containing public key exponent
**     [f_cbModulus]    : Length of f_pbModulus in bytes
**     [f_pbModulus]    : Buffer containing public key modulus
**     [f_pPublicKey]   : Returns public key in external format
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pPublicKey was NULL
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_SetPublicKey_4096BIT(
    __in const                       DRM_RSA_SUPPORTED_KEY_LENGTHS_4096BIT  f_eKeyLength,
    __in const                       DRM_DWORD                              f_cbPubExp,
    __in_ecount( f_cbPubExp ) const  DRM_BYTE                              *f_pbPubExp,
    __in const                       DRM_DWORD                              f_cbModulus,
    __in_ecount( f_cbModulus ) const DRM_BYTE                              *f_pbModulus,
    __out                            DRM_RSA_PUBLIC_KEY_4096BIT            *f_pPublicKey
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    INTERNAL_DRM_RSA_PUBLIC_KEY_4096BIT *pPubKeyInternal = ( INTERNAL_DRM_RSA_PUBLIC_KEY_4096BIT * )f_pPublicKey;

    ChkArg( NULL != f_pPublicKey );

    ChkDR( _RSA_SetPublicKey(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)f_eKeyLength, /* Upgrade key length */
        f_cbPubExp,
        f_pbPubExp,
        f_cbModulus,
        f_pbModulus,
        (DRM_DWORD*)&pPubKeyInternal->eKeyLength,
        pPubKeyInternal->rgdModulus,
        &pPubKeyInternal->cdModulus,
        pPubKeyInternal->rgdPubExp ) );

ErrorExit:
    return dr;
}

static DRMFORCEINLINE DRM_RESULT _RSA_SetPrivateKey(
    __in const                       DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT   f_eKeyLength,
    __in const                       DRM_DWORD                              f_cbPrivExp,
    __in_ecount( f_cbPrivExp ) const DRM_BYTE                              *f_pbPrivExp,
    __in const                       DRM_DWORD                              f_cbPrimes,
    __in_ecount( f_cbPrimes ) const  DRM_BYTE                              *f_pbPrime0,
    __in_ecount( f_cbPrimes ) const  DRM_BYTE                              *f_pbPrime1,
    __out                            DRM_DWORD                             *f_pcdPrimes0,
    __out                            DRM_DWORD                             *f_pcdPrimes1,
    __out                            digit_t                               *f_prgdPrimes0,
    __out                            digit_t                               *f_prgdPrimes1,
    __out                            digit_t                               *f_prgdCRTExponents0,
    __out                            digit_t                               *f_prgdCRTExponents1,
    __out                            digit_t                               *f_pdIQMP,
    __inout                          DRM_CRYPTO_CONTEXT                    *f_pCryptoCtx )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    digit_t    rgdTemp1[CDIGITS_MODULUS_MAX_ALLBIT];
    digit_t    rgdTemp2[CDIGITS_MODULUS_MAX_ALLBIT];
    digit_t    rgdTemp3[CDIGITS_MODULUS_MAX_ALLBIT];
    digit_t   *pdTemp1 = rgdTemp1;
    digit_t   *pdTemp2 = rgdTemp2;

    ChkArg( IS_VALID_DRM_RSA_KEY_LENGTH_ALLBIT( f_eKeyLength ) );
    ChkArg( DRM_RSA_CB_PRIVEXP( (DRM_DWORD)f_eKeyLength ) == f_cbPrivExp );
    ChkArg( NULL != f_pbPrivExp );
    ChkArg( DRM_RSA_CB_PRIME( (DRM_DWORD)f_eKeyLength ) == f_cbPrimes );
    ChkArg( NULL != f_pbPrime0 );
    ChkArg( NULL != f_pbPrime1 );
    ChkArg( NULL != f_pCryptoCtx );

    OEM_SECURE_MEMSET( rgdTemp1, 0, SIZEOF( rgdTemp1 ) ); 
    OEM_SECURE_MEMSET( rgdTemp2, 0, SIZEOF( rgdTemp2 ) ); 
    OEM_SECURE_MEMSET( rgdTemp3, 0, SIZEOF( rgdTemp3 ) ); 

    DRM_RSA_PKInitSize( f_pCryptoCtx->rgbCryptoContext, sizeof(f_pCryptoCtx->rgbCryptoContext)  );

    ChkDR( _BigEndianBytesToDigits( f_pbPrime0, DRM_RSA_CBITS_MODULUS( f_eKeyLength ), rgdTemp1 ) );
    ChkDR( _BigEndianBytesToDigits( f_pbPrime1, DRM_RSA_CBITS_MODULUS( f_eKeyLength ), rgdTemp2 ) );

    /*
    ** Determine the larger prime (p) and copy both primes (p and q)
    */
    if ( compare_same( rgdTemp1, rgdTemp2, CDIGITS_MODULUS( f_eKeyLength ) ) < 0 )
    {
        pdTemp1 = rgdTemp2;
        pdTemp2 = rgdTemp1;
    }

    OEM_SECURE_MEMCPY( f_prgdPrimes0, pdTemp1, DRM_RSA_CB_PRIME( f_eKeyLength ) );
    OEM_SECURE_MEMCPY( f_prgdPrimes1, pdTemp2, DRM_RSA_CB_PRIME( f_eKeyLength ) );

    *f_pcdPrimes0 = significant_digit_count( pdTemp1, CDIGITS_MODULUS( f_eKeyLength ) );
    *f_pcdPrimes1 = significant_digit_count( pdTemp2, CDIGITS_MODULUS( f_eKeyLength ) );

    ChkArg( *f_pcdPrimes0 <= CDIGITS_MODULUS_MAX_ALLBIT &&
            *f_pcdPrimes1 <= CDIGITS_MODULUS_MAX_ALLBIT );

    /*
    ** Compute CRT Exponents: d mod ( prime - 1 )
    */
    ChkBOOL( sub_immediate( pdTemp1, 1, pdTemp1, *f_pcdPrimes0 ) == 0,
             DRM_E_MODULAR_ARITHMETIC_FAILURE );
    ChkBOOL( sub_immediate( pdTemp2, 1, pdTemp2, *f_pcdPrimes0 ) == 0,
             DRM_E_MODULAR_ARITHMETIC_FAILURE );
    ChkDR( _BigEndianBytesToDigits( f_pbPrivExp, CBITS_PRIVEXP( f_eKeyLength ), f_pdIQMP ) );
    ChkBOOL( divide( f_pdIQMP,
                     CDIGITS_PRIVEXP( f_eKeyLength ),
                     pdTemp1,
                     *f_pcdPrimes0,
                     NULL,
                     rgdTemp3,
                     f_prgdCRTExponents0 ), DRM_E_MODULAR_ARITHMETIC_FAILURE );
    ChkBOOL( divide( f_pdIQMP,
                     CDIGITS_PRIVEXP( f_eKeyLength ),
                     pdTemp2,
                     *f_pcdPrimes1,
                     NULL,
                     rgdTemp3,
                     f_prgdCRTExponents1 ), DRM_E_MODULAR_ARITHMETIC_FAILURE );

    /*
    ** Compute iqmp: modular inverse of q (smaller prime) modulo p (larger prime)
    */
    ChkBOOL( mp_invert( f_prgdPrimes1,
                        f_prgdPrimes0,
                        *f_pcdPrimes0,
                        f_pdIQMP,
                        NULL,
                        NULL,
                        (struct bigctx_t *)f_pCryptoCtx->rgbCryptoContext ), DRM_E_MODULAR_ARITHMETIC_FAILURE );

ErrorExit:
    return dr;
}


/**********************************************************************
**
**  Function: OEM_RSA_SetPrivateKey_2048BIT
**
**  Synopsis: Sets up an internal form of an RSA private key
**
**  Arguments:
**  [f_eKeyLength]  : Specifies resultant private key length in bits
**  [f_cbPrivExp]   : Length of f_pbPrivExp in bytes
**  [f_pbPrivExp]   : Buffer containing private key exponent
**  [f_cbPrimes]    : Length of f_pbPrime0 and f_pbPrime1 in bytes (the buffers are same size)
**  [f_pbPrime0]    : Buffer containing private key prime
**  [f_pbPrime1]    : Buffer containing private key prime
**  [f_pPrivateKey] : Returns private key in external format
**  [f_pCryptoCtx]  : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pPrivateKey was NULL or an argument was out of range
**      DRM_E_MODULAR_ARITHMETIC_FAILURE
**           A failure occurred in the modular arithmetic
**
**  Note:    Size of the primes is the same as f_eKeyLength (not f_eKeyLength/2).
**           Primes uses lowest half part of the buffer.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_SetPrivateKey_2048BIT(
    __in const                       DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT  f_eKeyLength,
    __in const                       DRM_DWORD                              f_cbPrivExp,
    __in_ecount( f_cbPrivExp ) const DRM_BYTE                              *f_pbPrivExp,
    __in const                       DRM_DWORD                              f_cbPrimes,
    __in_ecount( f_cbPrimes ) const  DRM_BYTE                              *f_pbPrime0,
    __in_ecount( f_cbPrimes ) const  DRM_BYTE                              *f_pbPrime1,
    __out                            DRM_RSA_PRIVATE_KEY_2048BIT           *f_pPrivateKey,
    __inout                          DRM_CRYPTO_CONTEXT                    *f_pCryptoCtx
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr      = DRM_SUCCESS;
    INTERNAL_DRM_RSA_PRIVATE_KEY_2048BIT *pPvKeyInternal = ( INTERNAL_DRM_RSA_PRIVATE_KEY_2048BIT * )f_pPrivateKey;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_RSA, PERF_FUNC_OEM_RSA_SetPrivateKey_2048BIT );

    ChkArg( NULL != f_pPrivateKey );

    pPvKeyInternal->eKeyLength = f_eKeyLength;

    ChkDR( _RSA_SetPrivateKey(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)f_eKeyLength, /* Upgrade key length */
        f_cbPrivExp,
        f_pbPrivExp,
        f_cbPrimes,
        f_pbPrime0,
        f_pbPrime1,
        &pPvKeyInternal->cdPrimes[0],
        &pPvKeyInternal->cdPrimes[1],
        pPvKeyInternal->rgdPrimes[0],
        pPvKeyInternal->rgdPrimes[1],
        pPvKeyInternal->rgdCRTExponents[0],
        pPvKeyInternal->rgdCRTExponents[1],
        pPvKeyInternal->rgdIQMP,
        f_pCryptoCtx ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_SetPrivateKey_4096BIT
**
**  Synopsis: Sets up an internal form of an RSA private key
**
**  Arguments:
**  [f_eKeyLength]  : Specifies resultant private key length in bits
**  [f_cbPrivExp]   : Length of f_pbPrivExp in bytes
**  [f_pbPrivExp]   : Buffer containing private key exponent
**  [f_cbPrimes]    : Length of f_pbPrime0 and f_pbPrime1 in bytes (the buffers are same size)
**  [f_pbPrime0]    : Buffer containing private key prime
**  [f_pbPrime1]    : Buffer containing private key prime
**  [f_pPrivateKey] : Returns private key in external format
**  [f_pCryptoCtx]  : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pPrivateKey was NULL or an argument was out of range
**      DRM_E_MODULAR_ARITHMETIC_FAILURE
**           A failure occurred in the modular arithmetic
**
**  Note:    Size of the primes is the same as f_eKeyLength (not f_eKeyLength/2).
**           Primes uses lowest half part of the buffer.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_SetPrivateKey_4096BIT(
    __in const                       DRM_RSA_SUPPORTED_KEY_LENGTHS_4096BIT  f_eKeyLength,
    __in const                       DRM_DWORD                              f_cbPrivExp,
    __in_ecount( f_cbPrivExp ) const DRM_BYTE                              *f_pbPrivExp,
    __in const                       DRM_DWORD                              f_cbPrimes,
    __in_ecount( f_cbPrimes ) const  DRM_BYTE                              *f_pbPrime0,
    __in_ecount( f_cbPrimes ) const  DRM_BYTE                              *f_pbPrime1,
    __out                            DRM_RSA_PRIVATE_KEY_4096BIT           *f_pPrivateKey,
    __inout                          DRM_CRYPTO_CONTEXT                    *f_pCryptoCtx
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_SetPrivateKey_4096BIT(
    __in const                       DRM_RSA_SUPPORTED_KEY_LENGTHS_4096BIT  f_eKeyLength,
    __in const                       DRM_DWORD                              f_cbPrivExp,
    __in_ecount( f_cbPrivExp ) const DRM_BYTE                              *f_pbPrivExp,
    __in const                       DRM_DWORD                              f_cbPrimes,
    __in_ecount( f_cbPrimes ) const  DRM_BYTE                              *f_pbPrime0,
    __in_ecount( f_cbPrimes ) const  DRM_BYTE                              *f_pbPrime1,
    __out                            DRM_RSA_PRIVATE_KEY_4096BIT           *f_pPrivateKey,
    __inout                          DRM_CRYPTO_CONTEXT                    *f_pCryptoCtx
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr      = DRM_SUCCESS;
    INTERNAL_DRM_RSA_PRIVATE_KEY_4096BIT *pPvKeyInternal = ( INTERNAL_DRM_RSA_PRIVATE_KEY_4096BIT * )f_pPrivateKey;

    ChkArg( NULL != f_pPrivateKey );

    pPvKeyInternal->eKeyLength = f_eKeyLength;

    ChkDR( _RSA_SetPrivateKey(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)f_eKeyLength, /* Upgrade key length */
        f_cbPrivExp,
        f_pbPrivExp,
        f_cbPrimes,
        f_pbPrime0,
        f_pbPrime1,
        &pPvKeyInternal->cdPrimes[0],
        &pPvKeyInternal->cdPrimes[1],
        pPvKeyInternal->rgdPrimes[0],
        pPvKeyInternal->rgdPrimes[1],
        pPvKeyInternal->rgdCRTExponents[0],
        pPvKeyInternal->rgdCRTExponents[1],
        pPvKeyInternal->rgdIQMP,
        f_pCryptoCtx ) );

ErrorExit:
    return dr;
}

static DRMFORCEINLINE DRM_RESULT _RSA_ParsePublicKeyPubExp(
    __in                              DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT   f_eKeyLength,
    __in                        const digit_t                               *f_prgdPubExp,
    __inout_opt                       DRM_DWORD                             *f_pcbPubExp,
    __out_ecount_opt( *f_pcbPubExp )  DRM_BYTE                              *f_pbPubExp )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if ( NULL != f_pcbPubExp )
    {
        DRM_DWORD cbPubExp = *f_pcbPubExp;
        *f_pcbPubExp = DRM_RSA_CB_PUBEXP;
        if ( cbPubExp < *f_pcbPubExp )
        {
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }
        if ( NULL != f_pbPubExp )
        {
            ChkDR( _DigitsToBigEndianBytes( f_prgdPubExp,
                                            CBITS_PUBEXP,
                                            f_pbPubExp ) );
        }
    }

ErrorExit:
    return dr;
}

static DRMFORCEINLINE DRM_RESULT _RSA_ParsePublicKeyModulus(
    __in                              DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT   f_eKeyLength,
    __in                        const digit_t                               *f_prgdModulus,
    __inout_opt                       DRM_DWORD                             *f_pcbModulus,
    __out_ecount_opt( *f_pcbModulus ) DRM_BYTE                              *f_pbModulus )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if ( NULL != f_pcbModulus )
    {
        DRM_DWORD cbModulus = *f_pcbModulus;
        *f_pcbModulus = DRM_RSA_CB_MODULUS( f_eKeyLength );
        if ( cbModulus < *f_pcbModulus )
        {
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }
        if ( NULL != f_pbModulus )
        {
            ChkDR( _DigitsToBigEndianBytes( f_prgdModulus,
                                            DRM_RSA_CBITS_MODULUS( f_eKeyLength ),
                                            f_pbModulus ) );
        }
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_ParsePublicKey_2048BIT
**
**  Synopsis: Parses the elements from an internal form of an RSA public key.
**
**  Arguments:
**  [f_pPublicKey]  : Specifies the public key in external format
**  [f_peKeyLength] : Returns the public key length in bits (optional)
**  [f_pcbPubExp]   : Size of f_pbPubExp in bytes if it is not NULL
**  [f_pbPubExp]    : Buffer to return the public key exponent (optional, may be NULL)
**  [f_pcbModulus]  : Size of f_pbModulus in bytes if it is not NULL
**  [f_pbModulus]   : Buffer to return the public key modulus (optional, may be NULL)
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pPublicKey was NULL
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_ParsePublicKey_2048BIT(
    __in const                        DRM_RSA_PUBLIC_KEY_2048BIT            *f_pPublicKey,
    __out_opt                         DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT *f_peKeyLength,
    __inout_opt                       DRM_DWORD                             *f_pcbPubExp,
    __out_ecount_opt( *f_pcbPubExp )  DRM_BYTE                              *f_pbPubExp,
    __inout_opt                       DRM_DWORD                             *f_pcbModulus,
    __out_ecount_opt( *f_pcbModulus ) DRM_BYTE                              *f_pbModulus
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_ParsePublicKey_2048BIT(
    __in const                        DRM_RSA_PUBLIC_KEY_2048BIT            *f_pPublicKey,
    __out_opt                         DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT *f_peKeyLength,
    __inout_opt                       DRM_DWORD                             *f_pcbPubExp,
    __out_ecount_opt( *f_pcbPubExp )  DRM_BYTE                              *f_pbPubExp,
    __inout_opt                       DRM_DWORD                             *f_pcbModulus,
    __out_ecount_opt( *f_pcbModulus ) DRM_BYTE                              *f_pbModulus
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    const INTERNAL_DRM_RSA_PUBLIC_KEY_2048BIT *pPubKeyInternal = ( const INTERNAL_DRM_RSA_PUBLIC_KEY_2048BIT * )f_pPublicKey;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_RSA, PERF_FUNC_OEM_RSA_ParsePublicKey_2048BIT );

    ChkArg( NULL != f_pPublicKey );

    if ( NULL != f_peKeyLength )
    {
        *f_peKeyLength = pPubKeyInternal->eKeyLength;
    }

    ChkDR( _RSA_ParsePublicKeyPubExp(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)pPubKeyInternal->eKeyLength, /* Upgrade key length */
        pPubKeyInternal->rgdPubExp,
        f_pcbPubExp,
        f_pbPubExp ) );

    ChkDR( _RSA_ParsePublicKeyModulus(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)pPubKeyInternal->eKeyLength, /* Upgrade key length */
        pPubKeyInternal->rgdModulus,
        f_pcbModulus,
        f_pbModulus ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_ParsePublicKey_4096BIT
**
**  Synopsis: Parses the elements from an internal form of an RSA public key.
**
**  Arguments:
**  [f_pPublicKey]  : Specifies the public key in external format
**  [f_peKeyLength] : Returns the public key length in bits (optional)
**  [f_pcbPubExp]   : Size of f_pbPubExp in bytes if it is not NULL
**  [f_pbPubExp]    : Buffer to return the public key exponent (optional, may be NULL)
**  [f_pcbModulus]  : Size of f_pbModulus in bytes if it is not NULL
**  [f_pbModulus]   : Buffer to return the public key modulus (optional, may be NULL)
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pPublicKey was NULL
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_ParsePublicKey_4096BIT(
    __in const                        DRM_RSA_PUBLIC_KEY_4096BIT            *f_pPublicKey,
    __out_opt                         DRM_RSA_SUPPORTED_KEY_LENGTHS_4096BIT *f_peKeyLength,
    __inout_opt                       DRM_DWORD                             *f_pcbPubExp,
    __out_ecount_opt( *f_pcbPubExp )  DRM_BYTE                              *f_pbPubExp,
    __inout_opt                       DRM_DWORD                             *f_pcbModulus,
    __out_ecount_opt( *f_pcbModulus ) DRM_BYTE                              *f_pbModulus
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_ParsePublicKey_4096BIT(
    __in const                        DRM_RSA_PUBLIC_KEY_4096BIT            *f_pPublicKey,
    __out_opt                         DRM_RSA_SUPPORTED_KEY_LENGTHS_4096BIT *f_peKeyLength,
    __inout_opt                       DRM_DWORD                             *f_pcbPubExp,
    __out_ecount_opt( *f_pcbPubExp )  DRM_BYTE                              *f_pbPubExp,
    __inout_opt                       DRM_DWORD                             *f_pcbModulus,
    __out_ecount_opt( *f_pcbModulus ) DRM_BYTE                              *f_pbModulus
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    const INTERNAL_DRM_RSA_PUBLIC_KEY_4096BIT *pPubKeyInternal = ( const INTERNAL_DRM_RSA_PUBLIC_KEY_4096BIT * )f_pPublicKey;

    ChkArg( NULL != f_pPublicKey );

    if ( NULL != f_peKeyLength )
    {
        *f_peKeyLength = pPubKeyInternal->eKeyLength;
    }

    ChkDR( _RSA_ParsePublicKeyPubExp(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)pPubKeyInternal->eKeyLength, /* Upgrade key length */
        pPubKeyInternal->rgdPubExp,
        f_pcbPubExp,
        f_pbPubExp ) );

    ChkDR( _RSA_ParsePublicKeyModulus(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)pPubKeyInternal->eKeyLength, /* Upgrade key length */
        pPubKeyInternal->rgdModulus,
        f_pcbModulus,
        f_pbModulus ) );

ErrorExit:
    return dr;
}

static DRMFORCEINLINE DRM_RESULT _RSA_ParsePrivateKeyPrivExp(
    __in                              const DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT   f_eKeyLength,
    __inout_opt                             DRM_DWORD                             *f_pcbPrivExp,
    __out_ecount_opt( *f_pcbPrivExp ) const DRM_BYTE                              *f_pbPrivExp )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( f_pcbPrivExp != NULL )
    {
        DRM_DWORD cbPrivExp = *f_pcbPrivExp;
        *f_pcbPrivExp = DRM_RSA_CB_PRIVEXP( f_eKeyLength );
        ChkBOOL ( cbPrivExp >= *f_pcbPrivExp, DRM_E_BUFFERTOOSMALL );

        if( f_pbPrivExp != NULL )
        {
            /*
            ** Extracting the private exponent from the privkey structure isn't implemented yet
            */
            DRMASSERT( FALSE );
            ChkDR( DRM_E_NOTIMPL );
        }
    }

ErrorExit:
    return dr;
}

static DRMFORCEINLINE DRM_RESULT _RSA_ParsePrivateKeyPrimes(
    __in                        const DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT   f_eKeyLength,
    __in                        const digit_t                               *f_prgdPrimes0,
    __in                        const digit_t                               *f_prgdPrimes1,
    __inout_opt                       DRM_DWORD                             *f_pcbPrimes,
    __out_ecount_opt( *f_pcbPrimes )  DRM_BYTE                              *f_pbPrime0,
    __out_ecount_opt( *f_pcbPrimes )  DRM_BYTE                              *f_pbPrime1 )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if ( NULL != f_pcbPrimes )
    {
        DRM_DWORD cbPrimes = *f_pcbPrimes;
        *f_pcbPrimes = DRM_RSA_CB_PRIME( f_eKeyLength );
        ChkBOOL ( cbPrimes >= *f_pcbPrimes, DRM_E_BUFFERTOOSMALL );
        if( f_pbPrime0 != NULL )
        {
            ChkDR( _DigitsToBigEndianBytes( f_prgdPrimes0,
                                            DRM_RSA_CBITS_MODULUS( f_eKeyLength ),
                                            f_pbPrime0 ) );
        }
        if( f_pbPrime1 != NULL )
        {
            ChkDR( _DigitsToBigEndianBytes( f_prgdPrimes1,
                                            DRM_RSA_CBITS_MODULUS( f_eKeyLength ),
                                            f_pbPrime1 ) );
        }
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_ParsePrivateKey_2048BIT
**
**  Synopsis: Parses the elements from an internal form of an RSA private key
**
**  Arguments:
**  [f_pPrivateKey] : Specifies the private key in external format
**  [f_peKeyLength] : Returns the private key length in bits (optional)
**  [f_pcbPrivExp]  : Size of f_pbPrivExp in bytes if it is not NULL
**  [f_pbPrivExp]   : Buffer to return the private key exponent (optional, may be NULL)
**  [f_pcbPrimes]   : Size of f_pbPrime0 and f_pbPrime1 in bytes if they are not NULL (sizes are the same)
**  [f_pbPrime0]    : Buffer to return the private key prime (optional, may be NULL)
**  [f_pbPrime1]    : Buffer to return the private key prime (optional, may be NULL)
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pPublicKey was NULL
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_RSA_ParsePrivateKey_2048BIT(
    __in                              const DRM_RSA_PRIVATE_KEY_2048BIT           *f_pPrivateKey,
    __out_opt                               DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT *f_peKeyLength,
    __inout_opt                             DRM_DWORD                             *f_pcbPrivExp,
    __out_ecount_opt( *f_pcbPrivExp ) const DRM_BYTE                              *f_pbPrivExp,
    __inout_opt                             DRM_DWORD                             *f_pcbPrimes,
    __out_ecount_opt( *f_pcbPrimes )        DRM_BYTE                              *f_pbPrime0,
    __out_ecount_opt( *f_pcbPrimes )        DRM_BYTE                              *f_pbPrime1 )
{
    DRM_RESULT dr = DRM_SUCCESS;
    const INTERNAL_DRM_RSA_PRIVATE_KEY_2048BIT *pPvKeyInternal = ( const INTERNAL_DRM_RSA_PRIVATE_KEY_2048BIT * )f_pPrivateKey;

    if ( NULL != f_peKeyLength )
    {
        *f_peKeyLength = pPvKeyInternal->eKeyLength;
    }

    ChkDR( _RSA_ParsePrivateKeyPrivExp(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)pPvKeyInternal->eKeyLength, /* Upgrade key length */
        f_pcbPrivExp,
        f_pbPrivExp ) );

    ChkDR( _RSA_ParsePrivateKeyPrimes(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)pPvKeyInternal->eKeyLength, /* Upgrade key length */
        pPvKeyInternal->rgdPrimes[0],
        pPvKeyInternal->rgdPrimes[1],
        f_pcbPrimes,
        f_pbPrime0,
        f_pbPrime1 ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_ParsePrivateKey_4096BIT
**
**  Synopsis: Parses the elements from an internal form of an RSA private key
**
**  Arguments:
**  [f_pPrivateKey] : Specifies the private key in external format
**  [f_peKeyLength] : Returns the private key length in bits (optional)
**  [f_pcbPrivExp]  : Size of f_pbPrivExp in bytes if it is not NULL
**  [f_pbPrivExp]   : Buffer to return the private key exponent (optional, may be NULL)
**  [f_pcbPrimes]   : Size of f_pbPrime0 and f_pbPrime1 in bytes if they are not NULL (sizes are the same)
**  [f_pbPrime0]    : Buffer to return the private key prime (optional, may be NULL)
**  [f_pbPrime1]    : Buffer to return the private key prime (optional, may be NULL)
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          f_pPublicKey was NULL
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_RSA_ParsePrivateKey_4096BIT(
    __in                              const DRM_RSA_PRIVATE_KEY_4096BIT           *f_pPrivateKey,
    __out_opt                               DRM_RSA_SUPPORTED_KEY_LENGTHS_4096BIT *f_peKeyLength,
    __inout_opt                             DRM_DWORD                             *f_pcbPrivExp,
    __out_ecount_opt( *f_pcbPrivExp ) const DRM_BYTE                              *f_pbPrivExp,
    __inout_opt                             DRM_DWORD                             *f_pcbPrimes,
    __out_ecount_opt( *f_pcbPrimes )        DRM_BYTE                              *f_pbPrime0,
    __out_ecount_opt( *f_pcbPrimes )        DRM_BYTE                              *f_pbPrime1 )
{
    DRM_RESULT dr = DRM_SUCCESS;
    const INTERNAL_DRM_RSA_PRIVATE_KEY_4096BIT *pPvKeyInternal = ( const INTERNAL_DRM_RSA_PRIVATE_KEY_4096BIT * )f_pPrivateKey;

    if ( NULL != f_peKeyLength )
    {
        *f_peKeyLength = pPvKeyInternal->eKeyLength;
    }

    ChkDR( _RSA_ParsePrivateKeyPrivExp(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)pPvKeyInternal->eKeyLength, /* Upgrade key length */
        f_pcbPrivExp,
        f_pbPrivExp ) );

    ChkDR( _RSA_ParsePrivateKeyPrimes(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)pPvKeyInternal->eKeyLength, /* Upgrade key length */
        pPvKeyInternal->rgdPrimes[0],
        pPvKeyInternal->rgdPrimes[1],
        f_pcbPrimes,
        f_pbPrime0,
        f_pbPrime1 ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_ZeroPublicKey_2048BIT
**
**  Synopsis: Securely zeroes out the internal representation of a
**            public key
**
**  Arguments:
**     [f_pPublicKey] : Specifies the key to clear
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          The key parameter is NULL
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_ZeroPublicKey_2048BIT(
    __in       DRM_RSA_PUBLIC_KEY_2048BIT            *f_pPublicKey
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_ZeroPublicKey_2048BIT(
    __in       DRM_RSA_PUBLIC_KEY_2048BIT            *f_pPublicKey
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_RSA, PERF_FUNC_OEM_RSA_ZeroPublicKey_2048BIT );

    ChkArg( NULL != f_pPublicKey );

    OEM_SECURE_ZERO_MEMORY( f_pPublicKey, SIZEOF( *f_pPublicKey ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_ZeroPublicKey_4096BIT
**
**  Synopsis: Securely zeroes out the internal representation of a
**            public key
**
**  Arguments:
**     [f_pPublicKey] : Specifies the key to clear
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          The key parameter is NULL
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_ZeroPublicKey_4096BIT(
    __in       DRM_RSA_PUBLIC_KEY_4096BIT            *f_pPublicKey
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_ZeroPublicKey_4096BIT(
    __in       DRM_RSA_PUBLIC_KEY_4096BIT            *f_pPublicKey
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_pPublicKey );

    OEM_SECURE_ZERO_MEMORY( f_pPublicKey, SIZEOF( *f_pPublicKey ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_ZeroPrivateKey_2048BIT
**
**  Synopsis: Securely zeroes out the internal representation of a
**            private key
**
**  Arguments:
**     [f_pPrivateKey] : Specifies the key to clear
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          The key parameter is NULL
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_ZeroPrivateKey_2048BIT(
    __in       DRM_RSA_PRIVATE_KEY_2048BIT           *f_pPrivateKey
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_ZeroPrivateKey_2048BIT(
    __in       DRM_RSA_PRIVATE_KEY_2048BIT           *f_pPrivateKey
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_RSA, PERF_FUNC_OEM_RSA_ZeroPrivateKey_2048BIT );

    ChkArg( NULL != f_pPrivateKey );

    OEM_SECURE_ZERO_MEMORY( f_pPrivateKey, SIZEOF( *f_pPrivateKey ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_ZeroPrivateKey_4096BIT
**
**  Synopsis: Securely zeroes out the internal representation of a
**            private key
**
**  Arguments:
**     [f_pPrivateKey] : Specifies the key to clear
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          The key parameter is NULL
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_ZeroPrivateKey_4096BIT(
    __in       DRM_RSA_PRIVATE_KEY_4096BIT           *f_pPrivateKey
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_ZeroPrivateKey_4096BIT(
    __in       DRM_RSA_PRIVATE_KEY_4096BIT           *f_pPrivateKey
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( NULL != f_pPrivateKey );

    OEM_SECURE_ZERO_MEMORY( f_pPrivateKey, SIZEOF( *f_pPrivateKey ) );

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_Encrypt_2048BIT
**
**  Synopsis: Encrypts a fixed size buffer using RSA
**
**  Arguments:
**     [f_pPublicKey] : Specifies the public key in the internal representation to use for encryption.
**     [f_cbDataIn]   : Size in bytes of the data buffer to be encrypted
**                      and size of resultant data buffer (should be the same)
**     [f_pbDataIn]   : Buffer containing data for encryption.
**     [f_pbDataOut]  : Buffer with resultant encrypted data.
**     [f_pCryptoCtx] : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**      DRM_E_OEM_RSA_ENCRYPTION_ERROR
**          An error occurred during encryption
**  Notes:
**      Both data buffers and key elements are in big endian byte format.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_Encrypt_2048BIT (
    __in const                      DRM_RSA_PUBLIC_KEY_2048BIT *f_pPublicKey,
    __in const                      DRM_DWORD                   f_cbDataIn,
    __in_ecount( f_cbDataIn ) const DRM_BYTE                   *f_pbDataIn,
    __out_ecount( f_cbDataIn )      DRM_BYTE                   *f_pbDataOut,
    __in                            DRM_CRYPTO_CONTEXT         *f_pCryptoCtx
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_Encrypt_2048BIT (
    __in const                      DRM_RSA_PUBLIC_KEY_2048BIT *f_pPublicKey,
    __in const                      DRM_DWORD                   f_cbDataIn,
    __in_ecount( f_cbDataIn ) const DRM_BYTE                   *f_pbDataIn,
    __out_ecount( f_cbDataIn )      DRM_BYTE                   *f_pbDataOut,
    __in                            DRM_CRYPTO_CONTEXT         *f_pCryptoCtx
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    const INTERNAL_DRM_RSA_PUBLIC_KEY_2048BIT *pPubKeyInternal = ( const INTERNAL_DRM_RSA_PUBLIC_KEY_2048BIT * )f_pPublicKey;

    ChkArg( NULL != f_pPublicKey );
    ChkArg( DRM_RSA_CB_MODULUS( (DRM_DWORD)pPubKeyInternal->eKeyLength ) == f_cbDataIn );
    ChkArg( NULL != f_pbDataIn );
    ChkArg( NULL != f_pbDataOut );
    ChkArg( NULL != f_pCryptoCtx);

    ChkArg( pPubKeyInternal->cdModulus > 0 &&
            pPubKeyInternal->cdModulus <= CDIGITS_MODULUS_MAX_2048BIT );

    ChkBOOL( DRM_SUCCEEDED( _ModularExponentiate(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)pPubKeyInternal->eKeyLength, /* Upgrade key length */
        f_cbDataIn,
        f_pbDataIn,
        CDIGITS_PUBEXP,
        pPubKeyInternal->rgdPubExp,
        pPubKeyInternal->cdModulus,
        pPubKeyInternal->rgdModulus,
        f_pbDataOut,
        f_pCryptoCtx ) ),
        DRM_E_OEM_RSA_ENCRYPTION_ERROR );

ErrorExit:
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_Encrypt_4096BIT
**
**  Synopsis: Encrypts a fixed size buffer using RSA
**
**  Arguments:
**     [f_pPublicKey] : Specifies the public key in the internal representation to use for encryption.
**     [f_cbDataIn]   : Size in bytes of the data buffer to be encrypted
**                      and size of resultant data buffer (should be the same)
**     [f_pbDataIn]   : Buffer containing data for encryption.
**     [f_pbDataOut]  : Buffer with resultant encrypted data.
**     [f_pCryptoCtx] : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**      DRM_E_OEM_RSA_ENCRYPTION_ERROR
**          An error occurred during encryption
**  Notes:
**      Both data buffers and key elements are in big endian byte format.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_Encrypt_4096BIT (
    __in const                      DRM_RSA_PUBLIC_KEY_4096BIT *f_pPublicKey,
    __in const                      DRM_DWORD                   f_cbDataIn,
    __in_ecount( f_cbDataIn ) const DRM_BYTE                   *f_pbDataIn,
    __out_ecount( f_cbDataIn )      DRM_BYTE                   *f_pbDataOut,
    __in                            DRM_CRYPTO_CONTEXT         *f_pCryptoCtx
    ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_Encrypt_4096BIT (
    __in const                      DRM_RSA_PUBLIC_KEY_4096BIT *f_pPublicKey,
    __in const                      DRM_DWORD                   f_cbDataIn,
    __in_ecount( f_cbDataIn ) const DRM_BYTE                   *f_pbDataIn,
    __out_ecount( f_cbDataIn )      DRM_BYTE                   *f_pbDataOut,
    __in                            DRM_CRYPTO_CONTEXT         *f_pCryptoCtx
    )
{
    CLAW_AUTO_RANDOM_CIPHER

    DRM_RESULT dr = DRM_SUCCESS;
    const INTERNAL_DRM_RSA_PUBLIC_KEY_4096BIT *pPubKeyInternal = ( const INTERNAL_DRM_RSA_PUBLIC_KEY_4096BIT * )f_pPublicKey;

    ChkArg( NULL != f_pPublicKey );
    ChkArg( DRM_RSA_CB_MODULUS( (DRM_DWORD)pPubKeyInternal->eKeyLength ) == f_cbDataIn );
    ChkArg( NULL != f_pbDataIn );
    ChkArg( NULL != f_pbDataOut );
    ChkArg( NULL != f_pCryptoCtx);

    ChkArg( pPubKeyInternal->cdModulus > 0 &&
            pPubKeyInternal->cdModulus <= CDIGITS_MODULUS_MAX_4096BIT );

    ChkBOOL( DRM_SUCCEEDED( _ModularExponentiate(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)pPubKeyInternal->eKeyLength, /* Upgrade key length */
        f_cbDataIn,
        f_pbDataIn,
        CDIGITS_PUBEXP,
        pPubKeyInternal->rgdPubExp,
        pPubKeyInternal->cdModulus,
        pPubKeyInternal->rgdModulus,
        f_pbDataOut,
        f_pCryptoCtx ) ),
        DRM_E_OEM_RSA_ENCRYPTION_ERROR );

ErrorExit:
    return dr;
}

static DRMFORCEINLINE DRM_RESULT _RSA_Decrypt(
    __in                      const DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT   f_eKeyLength,
    __in                      const DRM_DWORD                              f_cdPrimes0,
    __in                      const DRM_DWORD                              f_cdPrimes1,
    __in                      const digit_t                               *f_prgdPrimes0,
    __in                      const digit_t                               *f_prgdPrimes1,
    __in                      const digit_t                               *f_prgdCRTExponents0,
    __in                      const digit_t                               *f_prgdCRTExponents1,
    __in                      const digit_t                               *f_pdIQMP,
    __in                      const DRM_DWORD                              f_cbDataIn,
    __in_ecount( f_cbDataIn ) const DRM_BYTE                              *f_pbDataIn,
    __out_ecount( f_cbDataIn )      DRM_BYTE                              *f_pbDataOut,
    __in                            DRM_CRYPTO_CONTEXT                    *f_pCryptoCtx )
{
    DRM_RESULT       dr                                   = DRM_SUCCESS;
    digit_t          rgdTemp1[CDIGITS_MODULUS_MAX_ALLBIT];
    digit_t          rgdTemp2[CDIGITS_MODULUS_MAX_ALLBIT];
    digit_t          rgdTemp3[CDIGITS_MODULUS_MAX_ALLBIT];
    mp_modulus_t     mod;
    DRM_BOOL         fModCreated                          = FALSE;
    DRM_BOOL         fNegModular                          = FALSE;
    struct bigctx_t *pBigCtx                              = NULL;

    ChkArg( DRM_RSA_CB_MODULUS( (DRM_DWORD)f_eKeyLength ) == f_cbDataIn );
    ChkArg( NULL != f_pbDataIn );
    ChkArg( NULL != f_pbDataOut );
    ChkArg( NULL != f_pCryptoCtx );
    ChkArg( ( SIZEOF( rgdTemp1 ) / SIZEOF( rgdTemp1[ 0 ] ) ) >= f_cdPrimes1 );

    OEM_SECURE_MEMSET( rgdTemp1, 0, SIZEOF( rgdTemp1 ) ); 
    OEM_SECURE_MEMSET( rgdTemp2, 0, SIZEOF( rgdTemp2 ) ); 
    OEM_SECURE_MEMSET( rgdTemp3, 0, SIZEOF( rgdTemp3 ) ); 
    OEM_SECURE_MEMSET( &mod, 0, SIZEOF( mod ) );        
   
    pBigCtx = (struct bigctx_t *)f_pCryptoCtx->rgbCryptoContext;
    ChkDR( OEM_ECC_InitializeBignumStack( pBigCtx ) );

    ChkArg( f_cdPrimes0 <= CDIGITS_MODULUS( (DRM_DWORD)f_eKeyLength ) );
    ChkArg( f_cdPrimes1 <= CDIGITS_MODULUS( (DRM_DWORD)f_eKeyLength ) );

    ChkDR( _BigEndianBytesToDigits( f_pbDataIn, DRM_RSA_CBITS_MODULUS( f_eKeyLength ), rgdTemp1 ) );

    /*
    ** Compute m2 = cipher^CRTExponents[1] mod q
    */
    ChkBOOL( create_modulus( f_prgdPrimes1,
                             f_cdPrimes1,
                             FROM_RIGHT,
                            &mod,
                             pBigCtx,
                             pBigCtx ), DRM_E_OEM_RSA_DECRYPTION_ERROR );
    fModCreated = TRUE;
    ChkBOOL( to_modular( rgdTemp1, CDIGITS_MODULUS( f_eKeyLength ), rgdTemp2, &mod, pBigCtx ),
             DRM_E_OEM_RSA_DECRYPTION_ERROR );
    ChkBOOL( mod_exp( rgdTemp2,
                      f_prgdCRTExponents1,
                      f_cdPrimes1,
                      rgdTemp2,
                     &mod,
                      pBigCtx ), DRM_E_OEM_RSA_DECRYPTION_ERROR );
    ChkBOOL( from_modular( rgdTemp2, rgdTemp2, &mod ), DRM_E_OEM_RSA_DECRYPTION_ERROR );
    uncreate_modulus( &mod, pBigCtx );
    fModCreated = FALSE;

    /*
    ** Compute m1 = cipher^CRTExponents[0] mod p
    */
    ChkBOOL( create_modulus( f_prgdPrimes0,
                             f_cdPrimes0,
                             FROM_RIGHT,
                            &mod,
                             pBigCtx,
                             pBigCtx ), DRM_E_OEM_RSA_DECRYPTION_ERROR );
    fModCreated = TRUE;
    ChkBOOL( to_modular( rgdTemp1, CDIGITS_MODULUS( f_eKeyLength ), rgdTemp1, &mod, pBigCtx ),
             DRM_E_OEM_RSA_DECRYPTION_ERROR );
    ChkBOOL( mod_exp( rgdTemp1,
                      f_prgdCRTExponents0,
                      f_cdPrimes0,
                      rgdTemp1,
                     &mod,
                      pBigCtx ), DRM_E_OEM_RSA_DECRYPTION_ERROR );
    ChkBOOL( from_modular( rgdTemp1, rgdTemp1, &mod ), DRM_E_OEM_RSA_DECRYPTION_ERROR );

    /*
    ** Compute iqmp * ( m1 - m2 ) mod p
    */
    if( compare_diff( rgdTemp1,
                      f_cdPrimes0,
                      rgdTemp2,
                      f_cdPrimes1 ) < 0 )
    {
        /*
        ** ( m1 - m2 ) < 0, works around negative modular division
        */
        fNegModular = TRUE;
        ChkBOOL( sub_same( rgdTemp2,
                           rgdTemp1,
                           rgdTemp1,
                           f_cdPrimes1 ) == 0, DRM_E_OEM_RSA_DECRYPTION_ERROR );
    }
    else
    {
        ChkBOOL( sub_diff( rgdTemp1,
                           f_cdPrimes0,
                           rgdTemp2,
                           f_cdPrimes1,
                           rgdTemp1,
                           NULL ), DRM_E_OEM_RSA_DECRYPTION_ERROR );
    }

    /*
    ** multiply by iqmp % p
    */
    ChkBOOL( to_modular( f_pdIQMP,
                         f_cdPrimes0,
                         rgdTemp3,
                        &mod,
                         pBigCtx ), DRM_E_OEM_RSA_DECRYPTION_ERROR );
    ChkBOOL( to_modular( rgdTemp1,
                         f_cdPrimes0,
                         rgdTemp1,
                        &mod,
                         pBigCtx ), DRM_E_OEM_RSA_DECRYPTION_ERROR );
    ChkBOOL( mod_mul( rgdTemp1, rgdTemp3, rgdTemp1, &mod, NULL, pBigCtx ),
             DRM_E_OEM_RSA_DECRYPTION_ERROR );
    ChkBOOL( from_modular( rgdTemp1, rgdTemp1, &mod ), DRM_E_OEM_RSA_DECRYPTION_ERROR );

    if( fNegModular && compare_immediate( rgdTemp1, 0, f_cdPrimes0 ) != 0 )
    {
        /*
        ** if remainder != 0, subtract from modulus (negative modular division)
        */
        ChkBOOL( sub_same( f_prgdPrimes0,
                           rgdTemp1,
                           rgdTemp1,
                           f_cdPrimes0 ) == 0, DRM_E_OEM_RSA_DECRYPTION_ERROR );
    }

    /*
    ** Compute message = m2 + ( iqmp * ( m1 - m2 ) % p ) * q
    */
    DRM_BYT_SetBytes( rgdTemp1,
                      f_cdPrimes0,
                      SIZEOF( digit_t ) * ( CDIGITS_MODULUS( f_eKeyLength ) - f_cdPrimes0 ),
                      0x00 );
    multiply_low( rgdTemp1, f_prgdPrimes1, rgdTemp3, CDIGITS_MODULUS( f_eKeyLength ) );
    ChkBOOL( add_diff( rgdTemp3,
                       CDIGITS_MODULUS( f_eKeyLength ),
                       rgdTemp2,
                       f_cdPrimes1,
                       rgdTemp2,
                       NULL ), DRM_E_OEM_RSA_DECRYPTION_ERROR );

    /*
    ** Convert back from digits
    */
    ChkDR( _DigitsToBigEndianBytes( rgdTemp2, DRM_RSA_CBITS_MODULUS( f_eKeyLength ), f_pbDataOut ) );

ErrorExit:
    if( fModCreated )
    {
        uncreate_modulus( &mod, pBigCtx );
    }

    return dr;
}


/**********************************************************************
**
**  Function: OEM_RSA_Decrypt_2048BIT
**
**  Synopsis: Decrypts a fixed size buffer using RSA
**
**  Arguments:
**     [f_pPrivateKey] : Specifies the private key in the internal representation to use for decryption.
**     [f_cbDataIn]    : Size in bytes of the encrypted data buffer
**                       and size of resultant data buffer (should be the same)
**     [f_pbDataIn]    : Buffer containing encrypted data.
**     [f_pbDataOut]   : Buffer with resultant decrypted (clear) data.
**     [f_pCryptoCtx]  : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**      DRM_E_OEM_RSA_DECRYPTION_ERROR
**          An error occurred during encryption
**
**  Notes:
**      Both data buffers and key elements are in big endian byte format.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_Decrypt_2048BIT(
    __in const                      DRM_RSA_PRIVATE_KEY_2048BIT *f_pPrivateKey,
    __in const                      DRM_DWORD                    f_cbDataIn,
    __in_ecount( f_cbDataIn ) const DRM_BYTE                    *f_pbDataIn,
    __out_ecount( f_cbDataIn )      DRM_BYTE                    *f_pbDataOut,
    __in                            DRM_CRYPTO_CONTEXT          *f_pCryptoCtx
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT                            dr              = DRM_SUCCESS;
    const INTERNAL_DRM_RSA_PRIVATE_KEY_2048BIT *pPvKeyInternal  = ( const INTERNAL_DRM_RSA_PRIVATE_KEY_2048BIT * )f_pPrivateKey;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_RSA, PERF_FUNC_OEM_RSA_Decrypt_2048BIT );

    ChkArg( NULL != f_pPrivateKey );

    ChkDR( _RSA_Decrypt(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)pPvKeyInternal->eKeyLength, /* Upgrade key length */
        pPvKeyInternal->cdPrimes[0],
        pPvKeyInternal->cdPrimes[1],
        pPvKeyInternal->rgdPrimes[0],
        pPvKeyInternal->rgdPrimes[1],
        pPvKeyInternal->rgdCRTExponents[0],
        pPvKeyInternal->rgdCRTExponents[1],
        pPvKeyInternal->rgdIQMP,
        f_cbDataIn,
        f_pbDataIn,
        f_pbDataOut,
        f_pCryptoCtx ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/**********************************************************************
**
**  Function: OEM_RSA_Decrypt_4096BIT
**
**  Synopsis: Decrypts a fixed size buffer using RSA
**
**  Arguments:
**     [f_pPrivateKey] : Specifies the private key in the internal representation to use for decryption.
**     [f_cbDataIn]    : Size in bytes of the encrypted data buffer
**                       and size of resultant data buffer (should be the same)
**     [f_pbDataIn]    : Buffer containing encrypted data.
**     [f_pbDataOut]   : Buffer with resultant decrypted (clear) data.
**     [f_pCryptoCtx]  : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL or out of range
**      DRM_E_OEM_RSA_DECRYPTION_ERROR
**          An error occurred during encryption
**
**  Notes:
**      Both data buffers and key elements are in big endian byte format.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_RSA_Decrypt_4096BIT(
    __in const                      DRM_RSA_PRIVATE_KEY_4096BIT *f_pPrivateKey,
    __in const                      DRM_DWORD                    f_cbDataIn,
    __in_ecount( f_cbDataIn ) const DRM_BYTE                    *f_pbDataIn,
    __out_ecount( f_cbDataIn )      DRM_BYTE                    *f_pbDataOut,
    __in                            DRM_CRYPTO_CONTEXT          *f_pCryptoCtx
    )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT                            dr              = DRM_SUCCESS;
    const INTERNAL_DRM_RSA_PRIVATE_KEY_4096BIT *pPvKeyInternal  = ( const INTERNAL_DRM_RSA_PRIVATE_KEY_4096BIT * )f_pPrivateKey;

    ChkArg( NULL != f_pPrivateKey );

    ChkDR( _RSA_Decrypt(
        (DRM_RSA_SUPPORTED_KEY_LENGTHS_ALLBIT)pPvKeyInternal->eKeyLength, /* Upgrade key length */
        pPvKeyInternal->cdPrimes[0],
        pPvKeyInternal->cdPrimes[1],
        pPvKeyInternal->rgdPrimes[0],
        pPvKeyInternal->rgdPrimes[1],
        pPvKeyInternal->rgdCRTExponents[0],
        pPvKeyInternal->rgdCRTExponents[1],
        pPvKeyInternal->rgdIQMP,
        f_cbDataIn,
        f_pbDataIn,
        f_pbDataOut,
        f_pCryptoCtx ) );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
