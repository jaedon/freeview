/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMECCP256_C
#include <drmerr.h>
#include <drmprofile.h>
#include <bigdefs.h>
#include <bignum.h>
#include <field.h>
#include <fieldpriv.h>
#include <ecurve.h>
#include <mprand.h>
#include "oemeccp160.h" /* include for OEM_ECC_InitializeBignumStack */
#include "oemeccp256impl.h"
#include "oemsha256.h"
#include <oem.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*************************************************************************************************
**
** Function: Convert_BigEndianBytesToDigits
**
** Description: Convert a DRM_BYTE array to a digit_t array. Input should have big-endian form.
**
** Args:
**    [f_rgbInBytes]:   The bytes to convert
**    [f_rgdOutDigits]: The output converted digits
**    [f_cBits]:        Number of bits to convert
**
** Returns: DRM_RESULT indicating success or the correct error code. Both parameters will be set
**          if function executes successfully.
**
** Notes:
**
*************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Convert_BigEndianBytesToDigits(
  __in_ecount( __CB_DECL( ( f_cBits + 7 ) / 8 ) ) const DRM_BYTE  f_rgbInBytes[],
  __out_ecount( BITS_TO_DIGITS( f_cBits ) )             digit_t   f_rgdOutDigits[],
                                             const DRM_DWORD f_cBits )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_BigEndianBytesToDigits );
#endif

    ChkDR( Convert_BigEndianBytesToDigitsImpl(
        f_rgbInBytes,
        f_rgdOutDigits,
        f_cBits ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end Convert_BigEndianBytesToDigits */

/*************************************************************************************************
**
** Function: Convert_DigitsToBigEndianBytes
**
** Description: Convert digit_t array to bytes. Put most significant byte first.
**
** Args:
**    [f_rgdInDigits]: The input digits to be converted
**    [f_rgbOutBytes]: The output converted bytes
**    [f_cBits]:       Number of bits to convert
**
** Returns: DRM_RESULT indicating success or the correct error code. Both parameters will be set
**          if function executes successfully.
**
** Notes:
**
*************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Convert_DigitsToBigEndianBytes(
  __in_ecount( BITS_TO_DIGITS( f_cBits ) )          const digit_t   f_rgdInDigits[],
  __out_ecount( __CB_DECL( ( f_cBits + 7 ) / 8 ) )        DRM_BYTE  f_rgbOutBytes[],
  __in                                              const DRM_DWORD f_cBits )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_DigitsToBigEndianBytes );
#endif

    ChkDR( Convert_DigitsToBigEndianBytesImpl(
        f_rgdInDigits,
        f_rgbOutBytes,
        f_cBits ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end digits_to_endian_bytes */

/*************************************************************************************************
**
** Function: Convert_P256_BigEndianBytesToDigits
**
** Description:
**
** Args:
**    [f_rgbInBytes]:   The input bytes to be converted
**    [f_rgdOutDigits]: The output converted digits
**
** Returns: DRM_RESULT indicating success or the correct error code. Both parameters will be set
**          if function executes successfully.
**
** Notes:
**
*************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Convert_P256_BigEndianBytesToDigits(
    __in_ecount( __CB_DECL( ECC_P256_INTEGER_SIZE_IN_BYTES ) ) const DRM_BYTE f_rgbInBytes[],
    __out_ecount( ECC_P256_INTEGER_SIZE_IN_DIGITS )                  digit_t  f_rgdOutDigits[] )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_BigEndianBytesToDigits );
#endif

    ChkDR( Convert_P256_BigEndianBytesToDigitsImpl(
        f_rgbInBytes,
        f_rgdOutDigits ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end Convert_P256_BigEndianBytesToDigits */


/*************************************************************************************************
**
** Function: Convert_P256_BigEndianBytesToDigitsModOrder
**
** Description:
**
** Args:
**    [f_rgbBytes]:  The input bytes to be converted
**    [f_pEcurve ]:  EC that we are working on
**    [f_rgdDigits]: The output converted digits mod order
**
** Returns: DRM_RESULT indicating success or the correct error code. Both parameters will be set
**          if function executes successfully.
**
** Notes:
**
*************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Convert_P256_BigEndianBytesToDigitsModOrder(
    __in_ecount( ECC_P256_INTEGER_SIZE_IN_BYTES )   const DRM_BYTE  f_rgbBytes[],
    __in_ecount( 1 )                                const ecurve_t *f_pEcurve,
    __out_ecount( ECC_P256_INTEGER_SIZE_IN_DIGITS )       digit_t   f_rgdDigits[] )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_BigEndianBytesToDigitsModOrder );
#endif

    ChkDR( Convert_P256_BigEndianBytesToDigitsModOrderImpl(
        f_rgbBytes,
        f_pEcurve,
        f_rgdDigits ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end Convert_P256_BigEndianBytesToDigitsModOrder */

/*************************************************************************************************
**
** Function: Convert_P256_BigEndianBytesToPoint
**
** Description: Converts big endian bytes into a modular EC point ( ecaffines )
**
** Args:
**    [f_rgbInBytes]:   The input bytes to be converted
**    [f_pEcurve ]:     EC that we are working on
**    [f_rgptOutPoint]: The output converted data in modular point format
**    [f_pBigCtx]:      Bignum context pointer
**
** Returns: DRM_RESULT indicating success or the correct error code. Both parameters will be set
**          if function executes successfully.
**
** Notes:
**
*************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Convert_P256_BigEndianBytesToPoint(
    __in_ecount( __CB_DECL( ECC_P256_POINT_SIZE_IN_BYTES ) )  const  DRM_BYTE   f_rgbInBytes[],
    __in_ecount( 1 )                                          const  ecurve_t  *f_pEcurve,
    __out_ecount( ECC_P256_POINT_SIZE_IN_ECAFFINES )                 digit_t    f_rgptOutPoint[],
    __inout                                                   struct bigctx_t  *f_pBigCtx )
{
    DRM_RESULT dr = DRM_SUCCESS;
    CLAW_AUTO_RANDOM_CIPHER

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_BigEndianBytesToPoint );
#endif

    ChkDR( Convert_P256_BigEndianBytesToPointImpl(
        f_rgbInBytes,
        f_pEcurve,
        f_rgptOutPoint,
        f_pBigCtx ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end Convert_P256_BigEndianBytesToPoint */

/*************************************************************************************************
**
** Function: Convert_P256_DigitsToBigEndianBytes
**
** Description: Convert EC integer in digits format into big endian bytes format
**
** Args:
**    [f_rgdInDigits]: The digits to convert into bytes
**    [f_rgbOutBytes]: The converted bytes output buffer
**
** Returns: DRM_RESULT indicating success or the correct error code. Both parameters will be set
**          if function executes successfully.
**
** Notes:
**
*************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Convert_P256_DigitsToBigEndianBytes(
    __in_ecount( ECC_P256_INTEGER_SIZE_IN_DIGITS )              const digit_t  f_rgdInDigits[],
    __out_ecount( __CB_DECL( ECC_P256_INTEGER_SIZE_IN_BYTES ) )       DRM_BYTE f_rgbOutBytes[] )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_DigitsToBigEndianBytes );
#endif

    ChkDR( Convert_P256_DigitsToBigEndianBytesImpl(
        f_rgdInDigits,
        f_rgbOutBytes ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end Convert_P256_DigitsToBigEndianBytes */

/*************************************************************************************************
**
** Function: Convert_P256_ModularIntToDigitsModOrder
**
** Description: Converts a modular format EC integer into digits mod the curve's order
**
** Args:
**    [f_rgecInModularInt]: The modular format integer to be converted into big endian bytes % o
**    [f_pEcurve ]:         EC that we are working on
**    [f_rgbOutDigits]:     The output buffer containing digits formatted data ( mod order )
**
** Returns: DRM_RESULT indicating success or the correct error code. Both parameters will be set
**          if function executes successfully.
**
** Notes:
**
*************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Convert_P256_ModularIntToDigitsModOrder(
    __in_ecount( ECC_P256_INTEGER_SIZE_IN_ECAFFINES ) const  digit_t   f_rgecInModularInt[],
    __in_ecount( 1 )                                  const  ecurve_t *f_pEcurve,
    __out_ecount( ECC_P256_INTEGER_SIZE_IN_DIGITS )          digit_t   f_rgbOutDigits[] )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_ModularIntToDigitsModOrder );
#endif

    ChkDR( Convert_P256_ModularIntToDigitsModOrderImpl(
        f_rgecInModularInt,
        f_pEcurve,
        f_rgbOutDigits ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end Convert_P256_ModularIntToDigitsModOrder */

/************************************************************************************************
**
** Function:  OEM_ECC_MapX2PointP256
**
** Synopsis:  Given X coordinate of a point tries to calculate Y coordinate. Such that the
**            resulting point is on the P256 curve. Fails if it is not possible.
**
** Arguments:
**    [f_pX]:               X Coordinate
**    [f_pY]:               Output Y Coordinate
**    [f_rgdSuppliedTemps]: Optional temps Array for math functions
**    [f_pBigCtx]:          Bignum context pointer
**
** Returns: DRM_RESULT indicating success or the corresponding error code,
**          DRM_E_P256_PLAINTEXT_MAPPING_FAILURE is returned if mapping was not possible.
**          f_pY is filled in case of success.
**
** Notes:
**
*************************************************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_ECC_MapX2PointP256(
    __in_ecount(ECC_P256_INTEGER_SIZE_IN_DIGITS)   const digit_t          *f_pX,
    __out_ecount(ECC_P256_INTEGER_SIZE_IN_DIGITS)        digit_t          *f_pY,
    __inout_opt                                          digit_t           f_rgdSuppliedTemps[],
    __inout                                       struct bigctx_t         *f_pBigCtx )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_DRM_ECC_MapX2PointP256);
#endif

    ChkDR( OEM_ECC_MapX2PointP256Impl(
        f_pX,
        f_pY,
        f_rgdSuppliedTemps,
        f_pBigCtx ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
}

/*************************************************************************************************
**
** Function: Convert_P256_PointToPlaintext
**
** Description: Converts points into plaintext, assuming a lossless integer-to-point formula
**              see OEM_ECC_Encrpyt_P256 for details
**
** Arguements:
**    [f_rgptPoint]:  EC point to convert
**    [f_pEcurve ]:   EC that we are working on
**    [f_pPlaintext]: Convert plaintext output
**
** Returns: DRM_RESULT indicating success or the correct error code. Both parameters will be set
**          if function executes successfully.
**
** Notes:
**
*************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Convert_P256_PointToPlaintext(
    __in_ecount( ECC_P256_POINT_SIZE_IN_ECAFFINES ) const digit_t          f_rgptPoint[],
    __in_ecount( 1 )                                const ecurve_t        *f_pEcurve,
    __out_ecount( 1 )                                     PLAINTEXT_P256  *f_pPlaintext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    CLAW_AUTO_RANDOM_CIPHER

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_Convert_P256_PointToPlaintext );
#endif

    ChkDR( Convert_P256_PointToPlaintextImpl(
        f_rgptPoint,
        f_pEcurve,
        f_pPlaintext ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end Convert_P256_PointToPlaintext */

/*************************************************************************************************
**
** Function:    OEM_ECC_GenerateHMacKey_P256
**
** Description: This function assumes that PLAINTEXT_P256 stucture can contain 2 128bit AES keys.
**              It will, given a PLAINTEXT_P256 struct with the 2nd key set, generate the 1st key
**              in the struct (used for XMR HMACing ) such that will have a 100% chance of being
**              Mapped to an EC point without losing any information. (See OEM_ECC_Encrypt_P256
**              function header for more details ).
**
** Arguements:
**    [f_pKeys]   : A struct containing the ALREADY SET 2nd key (content key ) for XMR lics.
**    [f_pBigCtx] : Pointer to DRMBIGNUM_CONTEXT_STRUCT for temporary memory allocations.
**
** Returns: DRM_RESULT indicating success or the correct error code. Both parameters will be set
**          if function executes successfully.
**
** Notes: This function uses the P256 bit curve.
**        ( !!!) There is a very small chance that this function may fail to find a HMAC key
**        that, coupled with the content key will map to an EC Point. While the probability
**        is very low (way less then .01%) the caller should check if the DRM_RESULT returned
**        is DRM_E_P256_HMAC_KEYGEN_FAILURE. In which case calling the function another time will
**        probably solve the issue.
**
*************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECC_GenerateHMACKey_P256(
    __inout PLAINTEXT_P256  *f_pKeys,
    __inout struct bigctx_t *f_pBigCtx )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    CLAW_AUTO_RANDOM_CIPHER

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECC_GenerateHMACKey_P256 );
#endif

    ChkDR( OEM_ECC_GenerateHMACKey_P256Impl(
        f_pKeys,
        f_pBigCtx ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
}


/************************************************************************************************
**
** Function:  OEM_ECC_CanMapToPoint_P256
**
** Synopsis:  This function test to see if a number can be mapped into an EC point, to do this
**            the function checks if the number has a solution to the equation y^2 = x^3 + ax + b,
**            where the passed in number is the 'x' value
**
** Arguments:
**    [f_rgdNumber]: Number to test if it can map to an EC point
**    [f_pBigCtx]  : Pointer to DRMBIGNUM_CONTEXT_STRUCT for temporary memory allocations.
**
** Returns: DRM_SUCCESS if the number can be mapped, DRM_S_FALSE if it can not, and returns
**          an error code if there has an error while executing the function
**
** Notes:
**
*************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECC_CanMapToPoint_P256(
    __in_ecount( ECC_P256_INTEGER_SIZE_IN_DIGITS ) const  digit_t   f_rgdNumber[],
    __inout                                        struct bigctx_t *f_pBigCtx )
{
    DRM_RESULT dr      = DRM_SUCCESS;

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECC_CanMapToPoint_P256 );
#endif

    ChkDR( OEM_ECC_CanMapToPoint_P256Impl(
        f_rgdNumber,
        f_pBigCtx ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end OEM_ECC_CanMapToPoint_P256 */

DRM_API DRM_VOID DRM_CALL OEM_ECC_ZeroPublicKey_P256(
    __inout        PUBKEY_P256     *f_pPubkey )
{
    OEM_ECC_ZeroPublicKey_P256Impl( f_pPubkey );
}

DRM_API DRM_VOID DRM_CALL OEM_ECC_ZeroPrivateKey_P256(
    __inout        PRIVKEY_P256    *f_pPrivkey )
{
    OEM_ECC_ZeroPrivateKey_P256Impl( f_pPrivkey );
}

/************************************************************************************************
**
** Function: OEM_ECC_Decrypt_P256
**
** Synopsis: Performs EC El-Gamal P256 decryption on a buffer. The equation is:
**           P = C2 - k*C1, where ( C1, C2 ) is the ciphertext, P is the plaintext
**           and k is the private key.
**
** Arguments:
**    [f_pprivkey] -- Private key to decrypt with
**    [f_pCiphertext]    -- Encrypted bytes that are to be decrypted
**    [f_pPlaintext]   -- Clear text result
**    [f_pBigCtx]  -- Pointer to DRMBIGNUM_CONTEXT_STRUCT for temporary memory allocations.
**
** Returns: DRM_RESULT indicating success or the correct error code. Both parameters will be set
**          if function executes successfully.
**
** Notes: It is put in the [f_pPlaintext]. This function uses
**        the private key, be sure to protect/obfuscate/encrypt this function if the private key
**        is an asset. Also refer to the OEM_ECC_Decrypt_P256 function comments for more details
**        about the exact protocol for encyption and plaintext convesion.
**
*************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECC_Decrypt_P256(
    __in    const  PRIVKEY_P256    *f_pPrivkey,
    __in    const  CIPHERTEXT_P256 *f_pCiphertext,
    __out          PLAINTEXT_P256  *f_pPlaintext,
    __inout struct bigctx_t        *f_pBigCtx )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECC_Decrypt_P256 );
#endif

    ChkDR( OEM_ECC_Decrypt_P256Impl(
        f_pPrivkey,
        f_pCiphertext,
        f_pPlaintext,
        f_pBigCtx ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
}/* end OEM_ECC_Decrypt_P256 */

/************************************************************************************************
**
** Function:  OEM_ECC_Encrypt_P256
**
** Synopsis:  Encrpyts a block of plaintext, given a public key. The algorithm is EC El-Gamal:
**            C1 = r*G, C2 = P + r*K, where ( C1, C2 ) is the ciphertext, r is a runtime generated
**            random integer (256 bit) K is the public key, G is the generator or basepoint for
**            the elliptic curve and P is the plaintext.
**
** Arguments:
**    [f_pPubkey]:     Public key used to encrypt the plaintext
**    [f_pPlaintext]:  Plaintext data to be encrypted. Not all plaintext can map to Ciphertext!
**    [f_pCiphertext]: Ciphertext result, this will be output if function succeeds
**    [f_pBigCtx]:     Pointer to DRMBIGNUM_CONTEXT_STRUCT for temporary memory allocations.
**
** Returns: DRM_RESULT indicating success or the correct error code. Both parameters will be set
**          if function executes successfully.
**
** Notes:  Data is not encrpyted in place.  It is put in the f_pCiphertext. Not all plaintext
**         maps to ciphertext. There are two methods in which to deal with this:
**         1) Shorten the plaintext buffer such that the left over space can be arbirarily changed.
**            At decryption time we will not be able to retrieve those spare bytes.
**         2) Choose the ciphertext such that we know that it will be able to map to ciphertext.
**            In this way we can use all 32 bytes in the plaintext buffer, but if someone does not
**            choose a good plaintext then there is a chance that encryption will fail!
**         This function implements the 2nd of the two options, so your plaintext must be good!
**
*************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECC_Encrypt_P256(
    __in    const   PUBKEY_P256      *f_pPubkey,
    __in    const   PLAINTEXT_P256   *f_pPlaintext,
    __out           CIPHERTEXT_P256  *f_pCiphertext,
    __inout struct  bigctx_t         *f_pBigCtx )
{
    DRM_RESULT dr = DRM_SUCCESS;
    CLAW_AUTO_RANDOM_CIPHER

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECC_Encrypt_P256 );
#endif

    ChkDR( OEM_ECC_Encrypt_P256Impl(
        f_pPubkey,
        f_pPlaintext,
        f_pCiphertext,
        f_pBigCtx ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end OEM_ECC_Encrypt_P256 */

/************************************************************************************************
**
** Function:  OEM_ECC_GenKeyPair_P256
**
** Synopsis:  Generates a public-private key pair for the P256 FIPS Elliptic Curve
**
** Arguments:
**    [f_pPubKey]:  A pointer to the structure that will be populated with the new public key
**    [f_pPrivKey]: A pointer to the structure that will be populated with the new public key
**    [f_pBigCtx]:  Pointer to DRMBIGNUM_CONTEXT_STRUCT for temporary memory allocations.
**
** Returns: DRM_RESULT indicating success or the correct error code. Both parameters will be set
**          if function executes successfully.
**
** Notes: Generates a private key, this function should be protected the key is deemed as an asset
**
*************************************************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_ECC_GenKeyPair_P256(
    __out          PUBKEY_P256  *f_pPubKey,
    __out          PRIVKEY_P256 *f_pPrivKey,
    __inout struct bigctx_t     *f_pBigCtx )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECC_GenKeyPair_P256 );
#endif

    ChkDR( OEM_ECC_GenKeyPair_P256Impl(
        f_pPubKey,
        f_pPrivKey,
        f_pBigCtx ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end OEM_ECC_GenKeyPair_P256 */

/************************************************************************************************
**
** Function:  OEM_ECC_GenKeyPairRestrictedPriv_P256
**
** Synopsis:  Generates a public-private key pair for the P256 FIPS Elliptic Curve
**
** Arguments:
**    [f_pPubKey]:  A pointer to the structure that will be populated with the new public key
**    [f_pPrivKey]: A pointer to the structure that will be populated with the new public key
**    [f_pBigCtx]:  Pointer to DRMBIGNUM_CONTEXT_STRUCT for temporary memory allocations.
**
** Returns: DRM_RESULT indicating success or the correct error code. Both parameters will be set
**          if function executes successfully.
**
** Notes: Generates a private key, this function should be protected the key is deemed as an asset
**
*************************************************************************************************/
DRM_API DRM_RESULT DRM_CALL OEM_ECC_GenKeyPairRestrictedPriv_P256(
    __out          PUBKEY_P256  *f_pPubKey,
    __out          PRIVKEY_P256 *f_pPrivKey,
    __inout struct bigctx_t     *f_pBigCtx )
{
    DRM_RESULT dr         = DRM_SUCCESS;

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECC_GenKeyPairRestrictedPriv_P256 );
#endif

    ChkDR( OEM_ECC_GenKeyPairRestrictedPriv_P256Impl(
        f_pPubKey,
        f_pPrivKey,
        f_pBigCtx ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end OEM_ECC_GenKeyPairEncryptablePriv_P256 */

/*************************************************************************************************
**
**  Function: OEM_ECDSA_Sign_P256
**
**  Synopsis: Creates an ECDSA P256 signature for a message, given a private keys
**
**  Algorithm: x = message, P = public key, G = generator pt,
**             ( s, t ) = signature, q = field order and the ECDSA modulus
**             w = inverse( t ) MOD q
**             i = w * SHA256( x ) MOD q
**             j = w * s MOD q
**             ( u, v ) = iG + jP
**             Verify( x, ( s, t ) ) := u MOQ q == s
**
**  Arguments:
**   IN   [f_rgbMessage ]:   Message to check signature
**   IN   [f_cbMessageLen]: Message length in bytes
**   IN   [f_pPrivkey]:      Pointer to ECC Priv Key struct containing the privkey
**   OUT  [f_pSignature ]:   Pointer to the signature for the message
**        [f_pBigCtx] :   Pointer to DRMBIGNUM_CONTEXT_STRUCT for temporary memory allocations.
**
**  Notes: This function deals with the private key thus it must be protected/obfuscated/encrypted
**
*************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECDSA_Sign_P256(
    __in_ecount( f_cbMessageLen ) const  DRM_BYTE         f_rgbMessage[],
    __in                          const  DRM_DWORD        f_cbMessageLen,
    __in                          const  PRIVKEY_P256    *f_pPrivkey,
    __out                                SIGNATURE_P256  *f_pSignature,
    __inout                       struct bigctx_t        *f_pBigCtx )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT  dr            = DRM_SUCCESS;

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECDSA_Sign_P256 );
#endif

    ChkDR( OEM_ECDSA_Sign_P256Impl(
        f_rgbMessage,
        f_cbMessageLen,
        f_pPrivkey,
        f_pSignature,
        f_pBigCtx ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end OEM_ECDSA_Sign_P256 */

/*************************************************************************************************
**
**  Function: OEM_ECDSA_Verify_P256
**
**  Synopsis: Test to see if a message and a ECDSA signature match
**
**  Algorithm: x = message, P = public key, G = generator pt
**             (s,t) = signature, q = field order and the ECDSA modulus
**             w = inverse( t ) MOD q
**             i = w * SHA1(x) MOD q
**             j = w * s MOD q
**             (u, v) = iG + jP
**             Verify(x, (s, t) ) := u MOQ q == s
**
**  Arguments:
**   IN   [f_rgbMessage ]:   Message to check signature
**   IN   [f_cbMessageLen]: Message length in bytes
**   IN   [f_pPubkey]:      Pointer to ECC Public Key struct containing the pubkey
**   IN   [f_pSignature ]:   Pointer to the signature for the message
**     OUT[f_pfVerified]:   Returned true if signature was valid, else false
**        [f_pBigCtx] :   Pointer to DRMBIGNUM_CONTEXT_STRUCT for temporary memory allocations.
**
**  Returns:    S_OK if not errors in the function, else corresponding
**              DRM_RESULT error code. [f_pfVerified] param tells if the
**              signature matched the message ( and pub key )
**
*************************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECDSA_Verify_P256(
    __in_ecount( __CB_DECL( f_cbMessageLen ) ) const  DRM_BYTE         f_rgbMessage[],
    __in                                       const  DRM_DWORD        f_cbMessageLen,
    __in                                       const  PUBKEY_P256     *f_pPubkey,
    __in                                       const  SIGNATURE_P256  *f_pSignature,
    __inout                                    struct bigctx_t        *f_pBigCtx )
{
    DRM_RESULT  dr        = DRM_SUCCESS;
    CLAW_AUTO_RANDOM_CIPHER

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_ECC_P256, PERF_FUNC_OEM_ECDSA_Verify_P256 );
#endif

    ChkDR( OEM_ECDSA_Verify_P256Impl(
        f_rgbMessage,
        f_cbMessageLen,
        f_pPubkey,
        f_pSignature,
        f_pBigCtx ) );

ErrorExit:

#if DRM_SUPPORT_ECCPROFILING
    DRM_PROFILING_LEAVE_SCOPE;
#endif

    return dr;
} /* end OEM_ECDSA_Verify_P256 */


EXIT_PK_NAMESPACE_CODE;